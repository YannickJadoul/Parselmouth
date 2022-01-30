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

#include "TimeClassAspects.h"

#include <praat/fon/Harmonicity.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(Harmonicity) {
	addTimeFrameSampledMixin(*this);

	// TODO Get value in frame

	// TODO Mixins (or something else?) for TimeFrameSampled, TimeFunction, and TimeVector functionality

	def("get_value", // TODO Should be part of Vector class
	    [](Harmonicity self, double time, kVector_valueInterpolation interpolation) { return Vector_getValueAtX(self, time, 1, interpolation); },
	    "time"_a, "interpolation"_a = kVector_valueInterpolation::CUBIC);
}

} // namespace parselmouth
