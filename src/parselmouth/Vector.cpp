/*
 * Copyright (C) 2017-2022  Yannick Jadoul
 *
 * This file is part of Parselmouth.
 *
 * Parselmouth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parselmouth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>
 */

#include "Parselmouth.h"

#include "utils/SignatureCast.h"
#include "utils/pybind11/ImplicitStringToEnumConversion.h"
#include "utils/pybind11/NumericPredicates.h"

#include <praat/fon/Vector.h>

#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_ENUM_BINDING(ValueInterpolation) {
	value("NEAREST", kVector_valueInterpolation::NEAREST);
	value("LINEAR", kVector_valueInterpolation::LINEAR);
	value("CUBIC", kVector_valueInterpolation::CUBIC);
	value("SINC70", kVector_valueInterpolation::SINC70);
	value("SINC700", kVector_valueInterpolation::SINC700);

	make_implicitly_convertible_from_string(*this);
}

PRAAT_CLASS_BINDING(Vector) {
	using signature_cast_placeholder::_;

	// TODO Something to get rid of duplicate functions with different names?
	def("add",
	    &Vector_addScalar,
	    "number"_a);

	def("__iadd__",
	    [](Vector self, double number) { Vector_addScalar(self, number); return self; },
	    "number"_a, py::is_operator());

	def("__add__",
	    [](Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), number); return result; },
	    "number"_a, py::is_operator());

	def("__radd__",
	    [](Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), number); return result; },
	    "number"_a, py::is_operator());

	def("subtract",
	    [](Vector self, double number) { Vector_addScalar(self, -number); },
	    "number"_a);

	def("__isub__",
	    [](Vector self, double number) { Vector_addScalar(self, -number); return self; },
	    "number"_a, py::is_operator());

	def("__sub__",
	    [](Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), -number); return result; },
	    "number"_a, py::is_operator());

	def("subtract_mean",
	    &Vector_subtractMean);

	def("multiply",
	    &Vector_multiplyByScalar,
	    "factor"_a);

	def("__imul__",
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, factor); return self; },
	    "factor"_a, py::is_operator());

	def("__mul__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), factor); return result; },
	    "factor"_a, py::is_operator());

	def("__rmul__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), factor); return result; },
	    "factor"_a, py::is_operator());

	def("divide", // TODO Not zero?
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); },
	    "factor"_a);

	def("__itruediv__",
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); return self; },
	    "factor"_a, py::is_operator());

	def("__truediv__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), 1 / factor); return result; },
	    "factor"_a, py::is_operator());

#if PY_MAJOR_VERSION < 3
	def("__idiv__",
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); return self; },
	    "factor"_a, py::is_operator());

	def("__div__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), 1 / factor); return result; },
	    "factor"_a, py::is_operator());
#endif

	def("scale",
	    args_cast<_, Positive<_>>(Vector_scale),
	    "scale"_a);

	def("scale_peak",
	    args_cast<_, Positive<_>>(Vector_scale),
	    "new_peak"_a = 0.99);

	def("get_value", // TODO Default for interpolation? Different for Sound (SINC70), Harmonicity/Intensity/Formants (CUBIC) and Ltas (LINEAR); take praat_TimeFunction.h into account
	    [](Vector self, double x, std::optional<long> channel, kVector_valueInterpolation interpolation) { return Vector_getValueAtX (self, x, channel.value_or(Vector_CHANNEL_AVERAGE), interpolation); },
	    "x"_a, "channel"_a = std::nullopt, "interpolation"_a = kVector_valueInterpolation::CUBIC);
}

} // namespace parselmouth
