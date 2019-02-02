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

#pragma once
#ifndef INC_PARSELMOUTH_TIMECLASSASPECTS_H
#define INC_PARSELMOUTH_TIMECLASSASPECTS_H

#include "Parselmouth.h"

#include "utils/SignatureCast.h"
#include "utils/pybind11/NumericPredicates.h"

#include <praat/fon/Function.h>
#include <praat/fon/Sampled.h>

#include <pybind11/numpy.h>

#include <algorithm>
#include <type_traits>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

struct TimeFunction : public structFunction {};
struct TimeFrameSampled : public structSampled {};

template <typename Mixin>
void addMixinBase(py::handle &binding) {
	auto typeHandle = py::detail::get_type_handle(typeid(Mixin), true);
	binding.attr("__bases__") = py::make_tuple(typeHandle).attr("__add__")(binding.attr("__bases__"));
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
