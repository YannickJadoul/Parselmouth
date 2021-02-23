/*
 * Copyright (C) 2017-2021  Yannick Jadoul
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

// #include "utils/praat/MelderUtils.h"
// #include "utils/pybind11/ImplicitStringToEnumConversion.h"
// #include "utils/pybind11/NumericPredicates.h"

#include <praat/LPC/LPC.h>
// #include <praat/LPC/LPC_and_Cepstrumc.h>
// #include <praat/LPC/LPC_and_Formant.h>
// #include <praat/LPC/LPC_and_LFCC.h>
// #include <praat/LPC/LPC_and_LineSpectralFrequencies.h>
// #include <praat/LPC/LPC_and_Polynomial.h>
// #include <praat/LPC/LPC_and_Tube.h>
// #include <praat/LPC/LPC_to_Spectrogram.h>
// #include <praat/LPC/LPC_to_Spectrum.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_STRUCT_BINDING(Frame, LPC_Frame) {
	def_readonly("n_coefficients", &structLPC_Frame::nCoefficients);
	def_readonly("gain", &structLPC_Frame::gain);
	// def_property_readonly("a", [](LPC_Frame self) { return std::vector<double>(self->a.cells, self->a.cells + self->nCoefficients); });
	def_property_readonly("a",
	                      [](LPC_Frame self) {
		                    //   return std::vector<double>(, self->a.cells + self->nCoefficients);
		                      return py::array_t<double>(
		                              {self->nCoefficients},
		                              self->a.cells);
	                      });
}

PRAAT_CLASS_BINDING(LPC) {
	addTimeFrameSampledMixin(*this);

	NESTED_BINDINGS(LPC_Frame)

	// using signature_cast_placeholder::_;


	// oo_DOUBLE (samplingPeriod)   // from Sound
	// oo_INT (maxnCoefficients)
	// oo_STRUCTVEC (LPC_Frame, d_frames, nx)

	def_readonly("sampling_period", &structLPC::samplingPeriod);
	def_readonly("max_n_coefficients", &structLPC::maxnCoefficients);

	def("__len__", [](LPC self) { return self->nx; });
	def(
	        "__getitem__", [](LPC self, int i) {
		        if (i < 0) i += self->nx;// Python-style negative indexing
		        if (i < 0 || i >= self->nx) throw py::index_error("LPC index out of range");
		        return &self->d_frames.cells[i];
	        },
	        "i"_a, py::return_value_policy::reference_internal);

	def(
	        "__iter__",
	        [](LPC self) { return py::make_iterator(self->d_frames.cells, self->d_frames.cells + self->nx); },
	        py::keep_alive<0, 1>());
}

}// namespace parselmouth
