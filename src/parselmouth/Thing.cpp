#include "Parselmouth.h"

#include "praat/MelderInfoInterceptor.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<Thing>::init()
{
	def("__" PYBIND11_STRING_NAME "__", // Python 2 vs. Python 3 - __unicode__ vs. __str__
	    [](Sound self) { MelderInfoInterceptor info; self->v_info(); return info.string(); });

	def("__" PYBIND11_BYTES_NAME "__", // Python 2 vs. Python 3 - __str__ vs. __bytes__
	    [](Sound self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.bytes()); });
}

} // namespace parselmouth
