#include "Parselmouth.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

enum class Interpolation
{
	NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
	LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
	CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
	SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
	SINC700 = Vector_VALUE_INTERPOLATION_SINC700
};

void initVector(PraatBindings &bindings)
{
	bindings.get<Interpolation>()
			.value("NEAREST", Interpolation::NEAREST)
			.value("LINEAR", Interpolation::LINEAR)
			.value("CUBIC", Interpolation::CUBIC)
			.value("SINC70", Interpolation::SINC70)
			.value("SINC700", Interpolation::SINC700)
			;

	// TODO Something to get rid of duplicate functions with different names?
	bindings.get<Vector>()
			.def("add",
			     &Vector_addScalar,
			     "number"_a)

			.def("__iadd__",
			     [] (Vector self, double number) { Vector_addScalar(self, number); return self; },
			     "number"_a)

			.def("__add__",
			     [] (Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), number); return result; },
			     "number"_a)

			.def("__radd__",
			     [] (Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), number); return result; },
			     "number"_a)

			.def("subtract",
			     [] (Vector self, double number) { Vector_addScalar(self, -number); },
			     "number"_a)

			.def("__isub__",
			     [] (Vector self, double number) { Vector_addScalar(self, -number); return self; },
			     "number"_a)

			.def("__sub__",
			     [] (Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), -number); return result; },
			     "number"_a)

			.def("subtract_mean",
			     &Vector_subtractMean)

			.def("multiply",
			     &Vector_multiplyByScalar,
			     "factor"_a)

			.def("__imul__",
			     [] (Vector self, double factor) { Vector_multiplyByScalar(self, factor); return self; },
			     "factor"_a)

			.def("__mul__",
			     [] (Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), factor); return result; },
			     "factor"_a)

			.def("__rmul__",
			     [] (Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), factor); return result; },
			     "factor"_a)

			.def("divide",
			     [] (Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); },
			     "factor"_a)

			.def("__itruediv__",
			     [] (Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); return self; },
			     "factor"_a)

			.def("__truediv__",
			     [] (Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), 1 / factor); return result; },
			     "factor"_a)

			.def("__idiv__",
			     [] (Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); return self; },
			     "factor"_a)

			.def("__div__",
			     [] (Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), 1 / factor); return result; },
			     "factor"_a)

			;
}

} // namespace parselmouth
