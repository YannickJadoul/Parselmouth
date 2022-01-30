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
	             [](Function self) { return std::pair(self->xmin, self->xmax); },
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


CLASS_BINDING(TimeFunction, TimeFunction, structFunction, detail::PraatHolder<TimeFunction>)
BINDING_CONSTRUCTOR(TimeFunction, "TimeFunction")
BINDING_INIT(TimeFunction) {
	using signature_cast_placeholder::_;

	// TODO Get rid of code duplication (also with Function)? (attr("tmin") = attr("xmin"), attr("shift_times_by") = attr("shift_x_by")) ?

	auto get_tmin = [](Function self) { return self->xmin; };
	auto set_tmin = [](Function self, double time) { Function_shiftXTo(self, self->xmin, time); };
	auto get_tmax = [](Function self) { return self->xmax; };
	auto set_tmax = [](Function self, double time) { Function_shiftXTo(self, self->xmax, time); };
	auto get_trange = [](Function self) { return std::pair(self->xmin, self->xmax); };
	auto set_trange = [](auto tmin_name, auto tmax_name) {
		                  return [=](Function self, std::pair<double, double> value) {
			                  if (value.first >= value.second)
				                  Melder_throw (U"New end time", tmax_name, U" should be greater than new ", tmin_name, U".");
			                  Function_scaleXTo(self, value.first, value.second);
		                  };
	                  };
	auto get_duration = [](Function self) { return self->xmax - self->xmin; };

	def_property("tmin", get_tmin, set_tmin);
	def_property("tmax", get_tmax, set_tmax);
	def_property("trange", get_trange, set_trange(U"tmin", U"tmax"));

	def("get_start_time", get_tmin);
	def("get_end_time", get_tmax);

	def_property("start_time", get_tmin, set_tmin);
	def_property("end_time", get_tmax, set_tmax);
	def_property("time_range", get_trange, set_trange(U"start time", U"end time"));

	def_property("centre_time",
	             [](Function self) { return (self->xmax - self->xmin) / 2; },
	             [](Function self, double time) { Function_shiftXTo(self, (self->xmax - self->xmin) / 2, time); });

	def("get_total_duration", get_duration);
	def_property_readonly("total_duration", get_duration);
	def_property_readonly("duration", get_duration);

	def("shift_times_by", Function_shiftXBy, "seconds"_a);

	def("shift_times_to", Function_shiftXTo, "time"_a, "new_time"_a);

	def("shift_times_to",
	    [](Function self, std::string fromTime, double toTime) {
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

	def("scale_times_by", args_cast<Function, Positive<_>>(Function_scaleXBy), "scale"_a);

	def("scale_times_to",
	    [](Function self, double newStartTime, double newEndTime) {
		    if (newStartTime >= newEndTime)
			    Melder_throw (U"New end time should be greater than new start time.");
		    Function_scaleXTo(self, newStartTime, newEndTime);
	    },
	    "new_start_time"_a, "new_end_time"_a);
}

} // namespace parselmouth
