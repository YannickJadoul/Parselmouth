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

#include <pybind11/numpy.h>

#include <praat/dwtools/CC.h>
#include <praat/dwtools/Spectrogram_extensions.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_STRUCT_BINDING(Frame, CC_Frame) {
	def_readwrite("c0", &structCC_Frame::c0);

	def_property_readonly("c", [](CC_Frame self) { return py::array(self->numberOfCoefficients, &self->c[1], py::cast(self)); });

	def("__getitem__",
	    [](CC_Frame self, long i) {
		    if (i < 0) i += self->numberOfCoefficients; // Python-style negative indexing // TODO Index type?
			if (i < 0 || i >= self->numberOfCoefficients) throw py::index_error("CC Frame index out of range");
		    return i == 0 ? self->c0 : self->c[i];
	    },
		"i"_a);

	def("__setitem__",
	    [](CC_Frame self, long i, double value) {
		    if (i < 0) i += self->numberOfCoefficients; // Python-style negative indexing
		    if (i < 0 || i >= self->numberOfCoefficients) throw py::index_error("CC Frame index out of range");
		    (i == 0 ? self->c0 : self->c[i]) = value;
	    },
	    "i"_a, "value"_a);

	def("__len__",
	    [](CC_Frame self) { return self->numberOfCoefficients + 1; });

	def("to_array",
	    [](CC_Frame self) {
		    py::array_t<double> array(self->numberOfCoefficients + 1);
		    auto unchecked = array.mutable_unchecked<1>();
		    unchecked(0) = self->c0;
		    for (long i = 0; i < self->numberOfCoefficients; ++i) {
			    unchecked(i+1) = self->c[i+1];
		    }
		    return array;
	    });

	// TODO Make number of coefficients changeable?
}

PRAAT_CLASS_BINDING(CC) {
	addTimeFrameSampledMixin(*this);

	NESTED_BINDINGS(CC_Frame)

	using signature_cast_placeholder::_;

	def("get_number_of_coefficients",
	    args_cast<_, Positive<_>>(CC_getNumberOfCoefficients),
	    "frame_number"_a);

	def("get_value_in_frame",
	    args_cast<_, Positive<_>, Positive<_>>(CC_getValueInFrame),
	    "frame_number"_a, "index"_a);

	def("get_c0_value_in_frame",
	    args_cast<_, Positive<_>>(CC_getC0ValueInFrame),
	    "frame_number"_a);

	def("to_matrix",
	    &CC_to_Matrix);

	// TODO To DTW... -> depends on (maybe) having DTW

	def_readonly("fmin", &structCC::fmin);

	def_readonly("fmax", &structCC::fmax);

	def_readonly("max_n_coefficients", &structCC::maximumNumberOfCoefficients);

	def("get_frame",
		[](CC self, Positive<integer> frameNumber) {
			if (frameNumber > self->nx) Melder_throw(U"Frame number out of range");
			return &self->frame[frameNumber];
		},
		"frame_number"_a, py::return_value_policy::reference_internal);

	def("__getitem__",
	    [](CC self, long i) {
		    if (i < 0) i += self->nx; // Python-style negative indexing
		    if (i < 0 || i >= self->nx) throw py::index_error("CC index out of range");
		    return &self->frame[i+1];
	    },
	    "i"_a, py::return_value_policy::reference_internal);

	def("__getitem__",
	    [](CC self, std::tuple<long, long> ij) {
		    auto &[i, j] = ij;
		    if (i < 0) i += self->nx; // Python-style negative indexing
		    if (i < 0 || i >= self->nx) throw py::index_error("CC index out of range");
		    auto &frame = self->frame[i+1];
		    if (j < 0) j += frame.numberOfCoefficients; // Python-style negative indexing
		    if (j < 0 || j > frame.numberOfCoefficients) throw py::index_error("CC Frame index out of range");
		    return j == 0 ? frame.c0 : frame.c[j];
	    },
	    "ij"_a);

	def("__setitem__",
	    [](CC self, std::tuple<long, long> ij, double value) {
		    auto &[i, j] = ij;
		    if (i < 0) i += self->nx; // Python-style negative indexing
		    if (i < 0 || i >= self->nx) throw py::index_error("CC index out of range");
		    auto &frame = self->frame[i+1];
		    if (j < 0) j += frame.numberOfCoefficients; // Python-style negative indexing
		    if (j < 0 || j > frame.numberOfCoefficients) throw py::index_error("CC Frame index out of range");
		    (j == 0 ? frame.c0 : frame.c[j]) = value;
	    },
	    "ij"_a, "value"_a);

	def("__iter__",
	    [](CC self) { return py::make_iterator(&self->frame[1], &self->frame[self->nx+1]); },
	    py::keep_alive<0, 1>());

	def("to_array",
	    [](CC self) {
		    auto maxCoefficients = CC_getMaximumNumberOfCoefficients(self, 1, self->nx);
		    py::array_t<double> array({static_cast<size_t>(maxCoefficients + 1), static_cast<size_t>(self->nx)});

		    auto unchecked = array.mutable_unchecked<2>();
		    for (auto i = 0; i < self->nx; ++i) {
			    auto &frame = self->frame[i+1];
			    unchecked(0, i) = frame.c0;
			    for (auto j = 0; j < maxCoefficients; ++j) {
				    unchecked(j+1, i) = (j < frame.numberOfCoefficients) ? frame.c[j+1] : std::numeric_limits<double>::quiet_NaN();
			    }
		    }

		    return array;
	    });
}

} // namespace parselmouth
