#ifndef INC_BUFFER_PROTOCOL_H
#define INC_BUFFER_PROTOCOL_H

#include <boost/python/extract.hpp>
#include <boost/python/type_id.hpp>
#include <boost/python/converter/registry.hpp>

#include <functional>
#include <vector>

namespace boost {
namespace python {

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

} // python
} // boost

#endif // INC_BUFFER_PROTOCOL_H
