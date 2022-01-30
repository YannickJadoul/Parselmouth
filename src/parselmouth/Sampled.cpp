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

#include <praat/fon/Sampled.h>

#include <pybind11/numpy.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(Sampled)
{
	// TODO Unit handling

	// TODO What about setting these properties? Any desired effect
	def_readonly("nx", &structSampled::nx);

	def("__len__", [](Sampled self) { return self->nx; });

	def_readonly("x1", &structSampled::x1);

	def_readonly("dx", &structSampled::dx);

	def("xs",
		[](Sampled self) { // TODO This or rather use Python call to numpy?
			py::array_t<double> xs(static_cast<size_t>(self->nx));
			auto unchecked = xs.mutable_unchecked<1>();
			for (auto i = 0; i < self->nx; ++i) {
				unchecked(i) = self->x1 + i * self->dx;
			}
			return xs;
		});

	def("x_grid",
	    [](Sampled self) {
		    py::array_t<double> grid(static_cast<size_t>(self->nx) + 1);
		    auto unchecked = grid.mutable_unchecked<1>();
		    for (auto i = 0; i < self->nx + 1; ++i) {
			    unchecked(i) = self->x1 + (i - 0.5) * self->dx;
		    }
		    return grid;
	    });

	def("x_bins",
	    [](Sampled self) {
		    py::array_t<double> bins({self->nx, integer{2}});
		    auto unchecked = bins.mutable_unchecked<2>();
		    for (auto i = 0; i < self->nx; ++i) {
			    unchecked(i, 0) = self->x1 + (i - 0.5) * self->dx;
			    unchecked(i, 1) = self->x1 + (i + 0.5) * self->dx;
		    }
		    return bins;
	    });

	// TODO Sampled_indexToX, Sampled_xToIndex, etc
	// TODO WindowSamplesX
}


CLASS_BINDING(TimeFrameSampled, TimeFrameSampled, structSampled, detail::PraatHolder<TimeFrameSampled>)
BINDING_CONSTRUCTOR(TimeFrameSampled, "TimeFrameSampled")
BINDING_INIT(TimeFrameSampled) {
	using signature_cast_placeholder::_;

	addTimeFunctionMixin(*this);

	def_readonly("nt", &structSampled::nx);
	def_readonly("t1", &structSampled::x1);
	def_readonly("dt", &structSampled::dx);

	// TODO Get rid of code duplication with Sampled
	def("ts",
	    [](Sampled self) { // TODO This or rather use Python call to numpy?
		    py::array_t<double> ts(static_cast<size_t>(self->nx));
		    auto unchecked = ts.mutable_unchecked<1>();
		    for (auto i = 0; i < self->nx; ++i) {
			    unchecked(i) = self->x1 + i * self->dx;
		    }
		    return ts;
	    });

	def("t_grid",
	    [](Sampled self) {
		    py::array_t<double> grid(static_cast<size_t>(self->nx) + 1);
		    auto unchecked = grid.mutable_unchecked<1>();
		    for (auto i = 0; i < self->nx + 1; ++i) {
			    unchecked(i) = self->x1 + (i - 0.5) * self->dx;
		    }
		    return grid;
	    });

	def("t_bins",
	    [](Sampled self) {
		    py::array_t<double> bins({self->nx, integer{2}});
		    auto unchecked = bins.mutable_unchecked<2>();
		    for (auto i = 0; i < self->nx; ++i) {
			    unchecked(i, 0) = self->x1 + (i - 0.5) * self->dx;
			    unchecked(i, 1) = self->x1 + (i + 0.5) * self->dx;
		    }
		    return bins;
	    });

	def("get_number_of_frames", [](Sampled self) { return self->nx; });

	def_readonly("n_frames", &structSampled::nx);

	def("get_time_step", [](Sampled self) { return self->dx; });

	def_readonly("time_step", &structSampled::dx);

	def("get_time_from_frame_number",
	    args_cast<_, Positive<_>>(Sampled_indexToX<integer>),
	    "frame_number"_a);

	def("frame_number_to_time",
	    args_cast<_, Positive<_>>(Sampled_indexToX<integer>),
	    "frame_number"_a);

	def("get_frame_number_from_time", Sampled_xToIndex, "time"_a);

	def("time_to_frame_number", Sampled_xToIndex, "time"_a);

	// TODO get_sample_times() ? (cfr. "Get sample times" / Sampled_getX_numvec)
}

} // namespace parselmouth
