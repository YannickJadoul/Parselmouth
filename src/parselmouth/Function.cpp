/*
 * Copyright (C) 2017-2019  Yannick Jadoul
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
#include "utils/pybind11/NumericPredicates.h"

#include <praat/fon/Function.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(Function) {
	using signature_cast_placeholder::_;

	// TODO Unit handling

	def_property("xmin",
	             [](Function self) { return self->xmin; },
	             [](Function self, double time) { Function_shiftXTo(self, self->xmin, time); });

	def_property("xmax",
	             [](Function self) { return self->xmax; },
	             [](Function self, double time) { Function_shiftXTo(self, self->xmax, time); });

	def_property("xrange",
	             [](Function self) { return std::make_pair(self->xmin, self->xmax); },
	             [](Function self, std::pair<double, double> value) {
		             if (value.first >= value.second)
			             Melder_throw (U"New xmin should be greater than new xmax.");
		             Function_scaleXTo(self, value.first, value.second);
	             });

	def("shift_x_by",
	    &Function_shiftXBy,
	    "shift"_a);

	def("shift_x_to",
	    &Function_shiftXTo,
	    "x"_a, "new_x"_a);

	def("scale_x_by",
	    args_cast<_, Positive<_>>(Function_scaleXBy),
	    "scale"_a);

	def("scale_x_to",
	    [](Function self, double newXmin, double newXmax) {
		    if (newXmin >= newXmax)
			    Melder_throw (U"New xmin should be greater than new xmax.");
		    Function_scaleXTo(self, newXmin, newXmax);
	    },
	    "new_xmin"_a, "new_xmax"_a);
}

} // namespace parselmouth
