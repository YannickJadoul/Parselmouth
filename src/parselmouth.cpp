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

Sound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	Melder_relativePathToFile(Melder_peek8to32(path.c_str()), &file);
	return Sound_readFromSoundFile(&file).transfer();
}

/*MFCC toMFCC(Sound s)
{
	return Sound_to_MFCC(s, 12, 0.015, 0.005, 100.0, 0.0, 100.0);
}*/

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
		//.def("to_mfcc", &toMFCC, return_value_policy<manage_new_object>())
		//.def("to_mfcc_default", &Sound_to_MFCC, (arg("a") = 12, arg("b") = 0.015, arg("c") = 0.005, arg("d") = 100.0, arg("e") = 0.0, arg("f") = 100.0), return_value_policy<manage_new_object>())
		//.def("upsample", &Sound_upsample, return_value_policy<manage_new_object>())
	;

	class_<structMFCC, boost::noncopyable>("MFCC", no_init)
		.def("info", &structMFCC::v_info)
	;
}

