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

#include "Interpolation.h"
#include "TimeClassAspects.h"

#include <praat/fon/Harmonicity.h>

namespace parselmouth {

PRAAT_CLASS_BINDING(Harmonicity) {
	// TODO Get value in frame

	// TODO Mixins (or something else?) for TimeFrameSampled, TimeFunction, and TimeVector functionality

	initTimeFrameSampled(*this);

	def("get_value", // TODO Should be part of Vector class
	    [](Harmonicity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
	    "time"_a, "interpolation"_a = Interpolation::CUBIC);
}

} // namespace parselmouth
