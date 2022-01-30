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

#pragma once
#ifndef INC_PARSELMOUTH_TIMECLASSASPECTS_H
#define INC_PARSELMOUTH_TIMECLASSASPECTS_H

#include "Parselmouth.h"

#include <praat/fon/Function.h>
#include <praat/fon/Sampled.h>

namespace py = pybind11;

namespace parselmouth {

struct TimeFunction : public structFunction {};
struct TimeFrameSampled : public structSampled {};


template <typename Mixin>
void addMixinBase(py::handle &binding) {
	binding.attr("__bases__") = py::make_tuple(py::type::of<Mixin>()) + binding.attr("__bases__");
}

template <typename Class, typename... Extra>
void addTimeFunctionMixin(py::class_<Class, Extra...> &binding) {
	static_assert(std::is_base_of_v<structFunction, Class>, "Class needs to be a Praat Function subclass");

	addMixinBase<TimeFunction>(binding);
}

template <typename Class, typename... Extra>
void addTimeFrameSampledMixin(py::class_<Class, Extra...> &binding) {
	static_assert(std::is_base_of_v<structSampled, Class>, "Class needs to be a Praat Sampled subclass");

	addMixinBase<TimeFrameSampled>(binding);
}

} // namespace parselmouth

#endif // INC_PARSELMOUTH_TIMECLASSASPECTS_H
