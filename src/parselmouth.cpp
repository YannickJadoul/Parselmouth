#include "fon/Sound.h"
#include "dwtools/Sound_to_MFCC.h"
#include "sys/melder.h"
#undef I
#undef trace

#include "buffer_protocol.h"
#include "functor_signature.h"
#include <boost/python.hpp>
#include <boost/numpy.hpp>

#include "PraatUtils.h"

Sound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file).transfer();
}

boost::numpy::ndarray getCoefficients(MFCC cc)
{
	auto maxCoefficients = CC_getMaximumNumberOfCoefficients(cc, 1, cc->nx);
	auto array = boost::numpy::empty(2, std::vector<Py_intptr_t>({cc->nx, maxCoefficients + 1}).data(), boost::numpy::dtype::get_builtin<double>());

	auto data = array.get_data();
	auto strides = array.get_strides();
	for (auto i = 0; i < cc->nx; ++i) {
		*reinterpret_cast<double*>(data + i * strides[0]) = cc->frame[i+1].c0;
		for (auto j = 1; j <= maxCoefficients; ++j) {
			*reinterpret_cast<double*>(data + i * strides[0] + j * strides[1]) = (j <= cc->frame[i+1].numberOfCoefficients) ? cc->frame[i+1].c[j] : std::numeric_limits<double>::quiet_NaN();
		}
	}

	return array;
}

BOOST_PYTHON_MODULE(parselmouth)
{
	Melder_batch = true;
	
	using namespace boost::python;
	using namespace boost::numpy;

	boost::numpy::initialize();

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
		.def("__str__", [] (structSound &s) { MelderInfoInterceptor info; s.v_info(); return info.get(); })
		.def("read_file", &readSound, return_value_policy<manage_new_object>()).staticmethod("read_file")
		.def("upsample", returnsAutoThing(&Sound_upsample), return_value_policy<manage_new_object>(), arg("self"))
		.def("to_mfcc", returnsAutoThing(&Sound_to_MFCC), return_value_policy<manage_new_object>(), (arg("self"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0))
	;

	class_<structMFCC, boost::noncopyable>("MFCC", no_init)
		.def("__init__", make_constructor(returnsAutoThing(&Sound_to_MFCC), default_call_policies(), (arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))
		.def("info", &structMFCC::v_info)
		.def("get_coefficients", &getCoefficients)
	;
}

