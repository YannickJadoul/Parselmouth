/*
 * Copyright (C) 2017-2023  Yannick Jadoul
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

#include <optional>
#include <stdexcept>

#include <pybind11/stl.h>

#include "Parselmouth.h"

#include "Harmonicity_docstrings.h"
#include "TimeClassAspects.h"

#include <praat/fon/Harmonicity.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(Harmonicity) {
	addTimeFrameSampledMixin(*this);

  doc() = HARMONICITY_CLASS_DOCSTRING;

	// TODO Mixins (or something else?) for TimeFrameSampled, TimeFunction, and
	// TimeVector functionality

	def(py::init([](std::vector<double> data, std::optional<double> T,
	                double tmin, double tmax, std::optional<double> t1) {
		    // set domain and sample interval
		    auto n = data.size();
		    auto tfirst = t1 ? *t1 : tmin;
		    if (tmax <= tfirst && (!T || *T <= 0))
			    throw std::invalid_argument("Neither positive dt is defined nor "
			                                "end_time > t1");

		    if (tmax <= tfirst) tmax = tmin + *T * n;
		    double dt = T ? *T : (tmax - tmin) / double(n);

		    // create Praat object & copy the data
		    auto self = Harmonicity_create(tmin, tmax, n, dt, tfirst);
		    std::copy(data.begin(), data.end(), self->z.cells);
		    return self;
	    }),
	    "data"_a, "time_step"_a = nullptr, "start_time"_a = 0.0,
	    "end_time"_a = 0.0, "t1"_a = nullptr);

	// Make PointProcess class a s sequence-like Python class
	def(
	        "__getitem__",
	        [](Harmonicity self, long i) {
		        if (i < 0) i += self->nx;
		        if (i < 0 || i >= self->nx)
			        throw std::out_of_range("index out of range");
		        return self->z[1][i + 1];
	        },
	        "i"_a);

	def("__len__", [](Harmonicity self) { return self->nx; });

	def(
	        "__iter__",
	        [](Harmonicity self) {
		        return py::make_iterator(self->z.cells, self->z.cells + self->nx);
	        },
	        py::keep_alive<0, 1>());

	// Harmonicity: Get value at time...
	def(
	        "get_value",// TODO Should be part of Vector class
	        [](Harmonicity self, double time,
	           kVector_valueInterpolation interpolation) {
		        return Vector_getValueAtX(self, time, 1, interpolation);
	        },
	        "time"_a, "interpolation"_a = kVector_valueInterpolation::CUBIC,
	        GET_VALUE_DOCSTRING);

	// Harmonicity: Get value in frame...
	def(
	        "get_value_in_frame",
	        [](Harmonicity self, int frameNumber) {
		        return (frameNumber < 1 || frameNumber > self->nx
		                        ? undefined
		                        : self->z[1][frameNumber]);
	        },
	        "frame_number"_a, GET_VALUE_IN_FRAME_DOCSTRING);

	// Harmonicity: Formula...
	// def(
	// 	"formula",
	// 	[](Harmonicity self, conststring32 formula) {
	// 		Interpreter interpreter;
	// 		return Matrix_formula(self, formula, interpreter, nullptr);
	// 	},
	// 	"formula"_a);

	// Harmonicity: Get maximum...
	def(
	        "get_maximum",
	        [](Harmonicity self, double fromTime, double toTime,
	           kVector_peakInterpolation interpolation) {
		        return Vector_getMaximum(self, fromTime, toTime, interpolation);
	        },
	        "from_time"_a = 0.0, "to_time"_a = 0.0,
	        "interpolation"_a = kVector_peakInterpolation::PARABOLIC,
	        GET_MAXIMUM_DOCSTRING);

	// Harmonicity: Get mean...
	def(
	        "get_mean",
	        [](Harmonicity self, double fromTime, double toTime) {
		        return Harmonicity_getMean(self, fromTime, toTime);
	        },
	        "from_time"_a = 0.0, "to_time"_a = 0.0, GET_MEAN_DOCSTRING);

	// Harmonicity: Get minimum...
	def(
	        "get_minimum",
	        [](Harmonicity self, double fromTime, double toTime,
	           kVector_peakInterpolation interpolation) {
		        return Vector_getMinimum(self, fromTime, toTime, interpolation);
	        },
	        "from_time"_a = 0.0, "to_time"_a = 0.0,
	        "interpolation"_a = kVector_peakInterpolation::PARABOLIC,
	        GET_MINIMUM_DOCSTRING);

	// Harmonicity: Get standard deviation...
	def(
	        "get_standard_deviation",
	        [](Harmonicity self, double fromTime, double toTime) {
		        return Harmonicity_getStandardDeviation(self, fromTime, toTime);
	        },
	        "from_time"_a = 0.0, "to_time"_a = 0.0, GET_STANDARD_DEVIATION_DOCSTRING);

	// Harmonicity: Get time of maximum...
	def(
	        "get_time_of_maximum",
	        [](Harmonicity self, double fromTime, double toTime,
	           kVector_peakInterpolation interpolation) {
		        return Vector_getXOfMaximum(self, fromTime, toTime, interpolation);
	        },
	        "from_time"_a = 0.0, "to_time"_a = 0.0,
	        "interpolation"_a = kVector_peakInterpolation::PARABOLIC,
	        GET_TIME_OF_MAXIMUM_DOCSTRING);

	// Harmonicity: Get time of minimum...
	def(
	        "get_time_of_minimum",
	        [](Harmonicity self, double fromTime, double toTime,
	           kVector_peakInterpolation interpolation) {
		        return Vector_getXOfMinimum(self, fromTime, toTime, interpolation);
	        },
	        "from_time"_a = 0.0, "to_time"_a = 0.0,
	        "interpolation"_a = kVector_peakInterpolation::PARABOLIC,
	        GET_TIME_OF_MINIMUM_DOCSTRING);

	// double Harmonicity_getQuantile (Harmonicity me, double quantile) {
	def(
	        "get_quantile",
	        [](Harmonicity self, double quantile) {
		        return Harmonicity_getQuantile(self, quantile);
	        },
	        "quantile"_a, GET_QUANTILE_DOCSTRING);
}

}// namespace parselmouth
