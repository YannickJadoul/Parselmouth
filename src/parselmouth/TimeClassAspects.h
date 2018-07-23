/*
 * Copyright (C) 2017-2018  Yannick Jadoul
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

#include <algorithm>
#include <type_traits>

namespace py = pybind11;
using namespace py::literals;

#define WRAP_BINDING_METHOD(method_name) auto method_name = [&binding](auto &&... args) { binding.method_name(std::forward<decltype(args)>(args)...); }


namespace parselmouth {

template <typename Class, typename... Extra>
void initTimeFunction(ClassBinding<Class, Extra...> &binding) {
	static_assert(std::is_base_of<structFunction, Class>::value, "Class needs to be a Praat Function subclass");


	WRAP_BINDING_METHOD(def);
	WRAP_BINDING_METHOD(def_property);
	WRAP_BINDING_METHOD(def_property_readonly);

	using signature_cast_placeholder::_;

	// TODO Get rid of code duplication with Function?

	// TODO What about this?
	/* binding.attr("tmin") = binding.attr("xmin");
	binding.attr("tmax") = binding.attr("xmax");
	binding.attr("nt") = binding.attr("nx");
	binding.attr("t1") = binding.attr("x1");
	binding.attr("dt") = binding.attr("dx");*/

	def("get_start_time", [](Class *self) { return self->xmin; });

	def_property("start_time",
	             [](Class *self) { return self->xmin; },
	             [](Class *self, double time) { Function_shiftXTo(self, self->xmin, time); });

	def("get_end_time", [](Class *self) { return self->xmax; });

	def_property("end_time",
	             [](Class *self) { return self->xmax; },
	             [](Class *self, double time) { Function_shiftXTo(self, self->xmax, time); });

	def_property("centre_time",
	             [](Class *self) { return (self->xmax - self->xmin) / 2; },
	             [](Class *self, double time) { Function_shiftXTo(self, (self->xmax - self->xmin) / 2, time); });

	def_property("time_range",
	             [](Class *self) { return std::make_pair(self->xmin, self->xmax); },
	             [](Class *self, std::pair<double, double> value) {
		             if (value.first >= value.second)
			             Melder_throw (U"New end time should be greater than new start time.");
		             Function_scaleXTo(self, value.first, value.second);
	             });

	def("get_total_duration", [](Class *self) { return self->xmax - self->xmin; });

	def_property_readonly("total_duration", [](Class *self) { return self->xmax - self->xmin; });

	def_property_readonly("duration", [](Class *self) { return self->xmax - self->xmin; });

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

	// TODO tmin, tmax ?
}

template <typename Class, typename... Extra>
void initTimeFrameSampled(ClassBinding<Class, Extra...> &binding) {
	static_assert(std::is_base_of<structSampled, Class>::value, "Class needs to be a Praat Sampled subclass");


	WRAP_BINDING_METHOD(def);
	WRAP_BINDING_METHOD(def_readonly);

	using signature_cast_placeholder::_;


	initTimeFunction(binding);

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

	// TODO get_frame_times() ?
	// TODO t1, dt, nt, ts ?
	}

} // namespace parselmouth

#endif // INC_PARSELMOUTH_TIMECLASSASPECTS_H
