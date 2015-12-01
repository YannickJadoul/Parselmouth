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
#include "AutoThingUtils.h"
#include <boost/python.hpp>

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
		.def("__init__", make_constructor(&readSound, default_call_policies(), arg("path"))) //  (arg("self"), arg("path"))
		.def("read", &readSound, return_value_policy<manage_new_object>())
		.staticmethod("read")
		.def("info", &structSound::v_info)
		.def("to_mfcc", returnsAutoThing(&Sound_to_MFCC), return_value_policy<manage_new_object>(), (arg("self"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0))
		.def("upsample", returnsAutoThing(&Sound_upsample), return_value_policy<manage_new_object>(), arg("self"))
	;

	class_<structMFCC, boost::noncopyable>("MFCC", no_init)
		.def("info", &structMFCC::v_info)
	;
}

