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

#include <boost/python.hpp>

Sound readSound(const std::string &path)
{
	structMelderFile file = { nullptr };
	std::u32string path_32(path.begin(), path.end());
	Melder_relativePathToFile(path_32.c_str(), &file);
	return Sound_readFromSoundFile(&file).transfer();
}

/*MFCC toMFCC(Sound s)
{
	return Sound_to_MFCC(s, 12, 0.015, 0.005, 100.0, 0.0, 100.0);
}*/

BOOST_PYTHON_MODULE(praat_mfcc)
{
	Melder_batch = true;
	
	using namespace boost::python;

	class_<structSound, boost::noncopyable>("Sound", no_init)
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

