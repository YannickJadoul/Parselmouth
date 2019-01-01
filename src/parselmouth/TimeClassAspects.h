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

#define WRAP_BINDING_METHOD(method_name) auto method_name = [&binding](auto &&... args) { binding.method_name(std::forward<decltype(args)>(args)...); }


namespace parselmouth {

template <typename Class, typename... Extra>
void initTimeFunction(py::class_<Class, Extra...> &binding) {
	static_assert(std::is_base_of<structFunction, Class>::value, "Class needs to be a Praat Function subclass");


	WRAP_BINDING_METHOD(def);
	WRAP_BINDING_METHOD(def_property);
	WRAP_BINDING_METHOD(def_property_readonly);

	using signature_cast_placeholder::_;

	// TODO Get rid of code duplication (also with Function)? (attr("tmin") = attr("xmin"), attr("shift_times_by") = attr("shift_x_by")) ?

	auto get_tmin = [](Class *self) { return self->xmin; };
	auto set_tmin = [](Class *self, double time) { Function_shiftXTo(self, self->xmin, time); };
	auto get_tmax = [](Class *self) { return self->xmax; };
	auto set_tmax = [](Class *self, double time) { Function_shiftXTo(self, self->xmax, time); };
	auto get_trange = [](Class *self) { return std::make_pair(self->xmin, self->xmax); };
	auto set_trange = [](auto tmin_name, auto tmax_name) {
							return [=](Class *self, std::pair<double, double> value) {
								if (value.first >= value.second)
									Melder_throw (U"New ", tmax_name, U" should be greater than new ", tmin_name, U".");
								Function_scaleXTo(self, value.first, value.second);
							}; };
	auto get_duration = [](Class *self) { return self->xmax - self->xmin; };

	def_property("tmin", get_tmin, set_tmin);
	def_property("tmax", get_tmax, set_tmax);
	def_property("trange", get_trange, set_trange(U"tmin", U"tmax"));

	def("get_start_time", get_tmin);
	def("get_end_time", get_tmax);

	def_property("start_time", get_tmin, set_tmin);
	def_property("end_time", get_tmax, set_tmax);
	def_property("time_range", get_trange, set_trange(U"start time", U"end time"));

	def_property("centre_time",
	             [](Class *self) { return (self->xmax - self->xmin) / 2; },
	             [](Class *self, double time) { Function_shiftXTo(self, (self->xmax - self->xmin) / 2, time); });

	def("get_total_duration", get_duration);
	def_property_readonly("total_duration", get_duration);
	def_property_readonly("duration", get_duration);

	def("shift_times_by",
	    args_cast<Class *, _>(Function_shiftXBy),
		"seconds"_a);

	def("shift_times_to",
	    args_cast<Class *, _, _>(Function_shiftXTo),
		"time"_a, "new_time"_a);

	def("shift_times_to",
	    [](Class *self, std::string fromTime, double toTime) {
		    std::transform(fromTime.begin(), fromTime.end(), fromTime.begin(), ::tolower);

		    if (fromTime == "start" || fromTime == "start time") {
			    Function_shiftXTo(self, self->xmin, toTime);
		    }
		    else if (fromTime == "end" || fromTime == "end time") {
			    Function_shiftXTo(self, self->xmax, toTime);
		    }
		    else if (fromTime == "centre" || fromTime == "centre time") {
			    Function_shiftXTo(self, (self->xmax - self->xmin) / 2, toTime);
		    }
		    else {
			    throw py::value_error("'to_time' can be \"begin\", \"begin time\", \"centre\", \"centre time\", \"end\", or \"end time\"");
		    }
	    },
		"time"_a, "new_time"_a);

	def("scale_times_by",
		args_cast<Class *, Positive<_>>(Function_scaleXBy),
		"scale"_a);

	def("scale_times_to",
	    [](Class *self, double newStartTime, double newEndTime) {
		    if (newStartTime >= newEndTime)
			    Melder_throw (U"New end time should be greater than new start time.");
		    Function_scaleXTo(self, newStartTime, newEndTime);
	    },
	    "new_start_time"_a, "new_end_time"_a);
}

template <typename Class, typename... Extra>
void initTimeFrameSampled(py::class_<Class, Extra...> &binding) {
	static_assert(std::is_base_of<structSampled, Class>::value, "Class needs to be a Praat Sampled subclass");


	WRAP_BINDING_METHOD(def);
	WRAP_BINDING_METHOD(def_readonly);

	using signature_cast_placeholder::_;


	initTimeFunction(binding);

	def_readonly("nt", &Class::nx);
	def_readonly("t1", &Class::x1);
	def_readonly("dt", &Class::dx);

	// TODO Get rid of code duplication with Sampled
	def("ts",
	    [](Class *self) { // TODO This or rather use Python call to numpy?
		    py::array_t<double> ts(static_cast<size_t>(self->nx));
		    auto unchecked = ts.mutable_unchecked<1>();
		    for (auto i = 0; i < self->nx; ++i) {
			    unchecked(i) = self->x1 + i * self->dx;
		    }
		    return ts;
	    });

	def("t_grid",
	    [](Class *self) {
		    py::array_t<double> grid(static_cast<size_t>(self->nx) + 1);
		    auto unchecked = grid.mutable_unchecked<1>();
		    for (auto i = 0; i < self->nx + 1; ++i) {
			    unchecked(i) = self->x1 + (i - 0.5) * self->dx;
		    }
		    return grid;
	    });

	def("t_bins",
	    [](Class *self) {
		    py::array_t<double> bins({self->nx, integer{2}});
		    auto unchecked = bins.mutable_unchecked<2>();
		    for (auto i = 0; i < self->nx; ++i) {
			    unchecked(i, 0) = self->x1 + (i - 0.5) * self->dx;
			    unchecked(i, 1) = self->x1 + (i + 0.5) * self->dx;
		    }
		    return bins;
	    });

	def("get_number_of_frames", [](Class *self) { return self->nx; });

	def_readonly("n_frames", &Class::nx);

	def("get_time_step", [](Class *self) { return self->dx; });

	def_readonly("time_step", &Class::dx);

	def("get_time_from_frame_number",
	    args_cast<Class *, Positive<_>>(Sampled_indexToX<integer>),
		"frame_number"_a);

	def("frame_number_to_time",
	    args_cast<Class *, Positive<_>>(Sampled_indexToX<integer>),
	    "frame_number"_a);

	def("get_frame_number_from_time",
	    args_cast<Class *, _>(Sampled_xToIndex),
		"time"_a);

	def("time_to_frame_number",
	    args_cast<Class *, _>(Sampled_xToIndex),
	    "time"_a);

	// TODO get_sample_times() ? (cfr. "Get sample times" / Sampled_getX_numvec)
	}

} // namespace parselmouth

#endif // INC_PARSELMOUTH_TIMECLASSASPECTS_H
