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

#include "utils/pybind11/ImplicitStringToEnumConversion.h"

#include <praat/fon/Intensity.h>

#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

enum class AveragingMethod {
	MEDIAN = Intensity_averaging_MEDIAN,
	ENERGY = Intensity_averaging_ENERGY,
	SONES = Intensity_averaging_SONES,
	DB = Intensity_averaging_DB
};

PRAAT_ENUM_BINDING(AveragingMethod) {
	value("MEDIAN", AveragingMethod::MEDIAN);
	value("ENERGY", AveragingMethod::ENERGY);
	value("SONES", AveragingMethod::SONES);
	value("DB", AveragingMethod::DB);

	make_implicitly_convertible_from_string(*this);
}

PRAAT_CLASS_BINDING(Intensity) {
	addTimeFrameSampledMixin(*this);

	NESTED_BINDINGS(AveragingMethod)

	// TODO Get value in frame

	// TODO Mixins (or something else?) for TimeFrameSampled, TimeFunction, and TimeVector functionality

	def("get_value", // TODO Should be part of Vector class
	    [](Intensity self, double time, kVector_valueInterpolation interpolation) { return Vector_getValueAtX(self, time, 1, interpolation); },
	    "time"_a, "interpolation"_a = kVector_valueInterpolation::CUBIC);

	// TODO 'Get mean' should probably also be added to Sampled once units get figured out?

	def("get_average",
	    [](Intensity self, std::optional<double> fromTime, std::optional<double> toTime, AveragingMethod averagingMethod) {
		    return Intensity_getAverage(self, fromTime.value_or(self->xmin), toTime.value_or(self->xmax), static_cast<int>(averagingMethod));
	    },
	    "from_time"_a = std::nullopt, "to_time"_a = std::nullopt, "averaging_method"_a = AveragingMethod::ENERGY);

	// TODO Pitch_Intensity_getMean & Pitch_Intensity_getMeanAbsoluteSlope ? (cfr. Pitch)
}

} // namespace parselmouth
