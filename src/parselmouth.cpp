#undef NDEBUG
#include "fon/Sound.h"
#include "dwtools/Sound_to_MFCC.h"
#include "sys/melder.h"
#undef I
#undef trace
#define NDEBUG

#include <locale>
#include <memory>
#include <string>

#include "functor_signature.h"
#include <boost/python.hpp>

#include "AutoThingUtils.h"

#include <iostream>

template <typename T> struct format_string;
template <> struct format_string<char>               { static constexpr const char *value = "b"; };
template <> struct format_string<unsigned char>      { static constexpr const char *value = "B"; };
template <> struct format_string<bool>               { static constexpr const char *value = "?"; };
template <> struct format_string<short>              { static constexpr const char *value = "h"; };
template <> struct format_string<unsigned short>     { static constexpr const char *value = "H"; };
template <> struct format_string<int>                { static constexpr const char *value = "i"; };
template <> struct format_string<unsigned int>       { static constexpr const char *value = "I"; };
template <> struct format_string<long>               { static constexpr const char *value = "l"; };
template <> struct format_string<unsigned long>      { static constexpr const char *value = "L"; };
template <> struct format_string<long long>          { static constexpr const char *value = "q"; };
template <> struct format_string<unsigned long long> { static constexpr const char *value = "Q"; };
template <> struct format_string<float>              { static constexpr const char *value = "f"; };
template <> struct format_string<double>             { static constexpr const char *value = "d"; };

template <typename T>
struct buffer_info
{
	typedef T contained_type;

	void *buf;
	bool readonly;
	std::vector<Py_ssize_t> shape;
	std::vector<Py_ssize_t> strides;
	std::vector<Py_ssize_t> suboffsets;
	std::function<void()> release;
};

template <typename T>
class def_buffer
{
public:
	template <typename GetBuffer>
	def_buffer(GetBuffer&& getBuffer)
	{
		static auto type_info = boost::python::type_id<T>();

		static PyTypeObject *pyType = nullptr;
		if (pyType)
			throw std::runtime_error(std::string("class ") + type_info.name() + " already implements the buffer protocol");

		auto registration = boost::python::converter::registry::query(type_info);
		if (!registration)
			throw std::runtime_error(std::string("cannot implement the buffer protocol for unregistered class ") + type_info.name());

		pyType = registration->get_class_object();
		if ((pyType->tp_flags & Py_TPFLAGS_HEAPTYPE) == Py_TPFLAGS_HEAPTYPE) {
			pyType->tp_as_buffer = &reinterpret_cast<PyHeapTypeObject*>(pyType)->as_buffer;
			pyType->tp_flags |= Py_TPFLAGS_HAVE_NEWBUFFER;
		}
		else {
			static PyBufferProcs bufferProcs = {};
			pyType->tp_as_buffer = &bufferProcs;
			pyType->tp_flags |= Py_TPFLAGS_HAVE_NEWBUFFER;
		}

		if (pyType->tp_as_buffer->bf_getbuffer)
			throw std::runtime_error(std::string("class ") + type_info.name() + " already implements the buffer protocol");

		static GetBuffer staticGetBuffer(std::forward<GetBuffer>(getBuffer));
		getBufferImpl = &staticGetBuffer;
		pyType->tp_as_buffer->bf_getbuffer = &getBufferWrapper<GetBuffer>;
		pyType->tp_as_buffer->bf_releasebuffer = &releaseBufferWrapper;
	}

private:
	static void *getBufferImpl;

	template <typename GetBuffer>
	static int getBufferWrapper(PyObject *exporter, Py_buffer *view, int flags)
	{
		if (!exporter || !view)
			throw std::runtime_error("Internal error in the buffer protocol call");
		if (!getBufferImpl)
			throw std::runtime_error("Internal error in the implementation of the buffer protocol");

		boost::python::extract<const T&> extractor(exporter);
		if (!extractor.check())
			throw std::runtime_error("Internal error in the implementation of the buffer protocol");

		try {
			auto info = (*reinterpret_cast<GetBuffer*>(getBufferImpl))(extractor(), flags);
			typedef typename decltype(info)::contained_type contained_type;

			view->obj = exporter;
			view->buf = info.buf;
			view->len = sizeof(contained_type);
			for (auto n : info.shape)
				view->len *= n;
			view->itemsize = sizeof(contained_type);
			view->ndim = static_cast<int>(info.shape.size());
			view->readonly = info.readonly;
			view->format = flags & PyBUF_FORMAT ? const_cast<char*>(format_string<contained_type>::value) : nullptr;

			view->shape = new Py_ssize_t[view->ndim]();
			std::copy(info.shape.begin(), info.shape.end(), view->shape);
			view->strides = nullptr;
			if (info.strides.size() > 0) {
				if (info.strides.size() != view->ndim)
					throw std::runtime_error("Internal error in the implementation of the buffer protocol");
				view->strides = new Py_ssize_t[view->ndim]();
				std::copy(info.strides.begin(), info.strides.end(), view->strides);
			}
			view->suboffsets = nullptr;
			if (info.suboffsets.size() > 0) {
				if (info.suboffsets.size() > view->ndim)
					throw std::runtime_error("Internal error in the implementation of the buffer protocol");
				view->suboffsets = new Py_ssize_t[view->ndim]();
				std::fill(view->suboffsets, view->suboffsets + view->ndim, -1);
				std::copy(info.suboffsets.begin(), info.suboffsets.end(), view->suboffsets);
			}

			view->internal = info.release ? new std::function<void()>(std::move(info.release)) : nullptr;

			return 0;
		}
		catch (std::exception &e)
		{
			PyErr_SetString(PyExc_BufferError, e.what());
			view->obj = nullptr;
			return -1;
		}
	}

	static void releaseBufferWrapper(PyObject *exporter, Py_buffer *view)
	{
		if (!exporter || !view)
			throw std::runtime_error("Internal error in the buffer protocol call");
		if (!getBufferImpl)
			throw std::runtime_error("Internal error in the implementation of the buffer protocol");

		boost::python::extract<const T&> extractor(exporter);
		if (!extractor.check())
			throw std::runtime_error("Internal error in the implementation of the buffer protocol");

		if (view->internal) {
			auto f = reinterpret_cast<std::function<void()>*>(view->internal);
			(*f)();
			delete f;
		}

		delete[] view->shape;
		delete[] view->strides;
		delete[] view->suboffsets;
	}
};

template <typename T>
void *def_buffer<T>::getBufferImpl = nullptr;

Sound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file).transfer();
}

BOOST_PYTHON_MODULE(parselmouth)
{
	Melder_batch = true;
	
	using namespace boost::python;

	docstring_options docstringOptions(true, true, false);

	register_exception_translator<MelderError>(
			[] (const MelderError &) {
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1);
				Melder_clearError();
				throw std::runtime_error(message);
	        });

	class_<structSound, boost::noncopyable>("Sound", no_init)
		.def("__init__", make_constructor(&readSound, default_call_policies(), arg("path")))
		.def("read_file", &readSound, return_value_policy<manage_new_object>())
		.staticmethod("read_file")
		.def("info", &structSound::v_info)
		.def("to_mfcc", returnsAutoThing(&Sound_to_MFCC), return_value_policy<manage_new_object>(), (arg("self"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0))
		.def("upsample", returnsAutoThing(&Sound_upsample), return_value_policy<manage_new_object>(), arg("self"))
	;

	class_<structMFCC, boost::noncopyable>("MFCC", no_init)
		.def("__init__", make_constructor(returnsAutoThing(&Sound_to_MFCC), default_call_policies(), (arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))
		.def("info", &structMFCC::v_info)
	;

	def_buffer<structMFCC>(
			[] (const structMFCC &exporter, int flags)
				{
					if ((flags | PyBUF_WRITABLE) != PyBUF_FULL)
						std::runtime_error("MFCC can only export an indirect buffer");

					return buffer_info<double> { &exporter.frame[1].c,
					                             false,
					                             { exporter.nx, exporter.maximumNumberOfCoefficients },
					                             { sizeof(structCC_Frame), sizeof(double) },
					                             { sizeof(double), -1 } };
				});
}

