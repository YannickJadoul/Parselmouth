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

#include "LPC_docstrings.h"
#include "TimeClassAspects.h"
#include "utils/pybind11/NumericPredicates.h"

#include <praat/LPC/LPC.h>
// #include <praat/LPC/LPC_and_Cepstrumc.h>
// #include <praat/LPC/LPC_and_Formant.h>
// #include <praat/LPC/LPC_and_LFCC.h>
#include <praat/LPC/LPC_and_LineSpectralFrequencies.h>
// #include <praat/LPC/LPC_and_Polynomial.h>
// #include <praat/LPC/LPC_and_Tube.h>
#include <praat/LPC/LPC_to_Spectrogram.h>
#include <praat/LPC/LPC_to_Spectrum.h>
#include <praat/LPC/Sound_and_LPC_robust.h>
#include <praat/fon/Sound.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth
{

PRAAT_STRUCT_BINDING(Frame, LPC_Frame)
{

  doc() = FRAME_CLASS_DOCSTRING;

  def_readonly("n_coefficients", &structLPC_Frame::nCoefficients,
               FRAME_N_COEFFICIENTS);
  def_readonly("gain", &structLPC_Frame::gain, FRAME_GAIN);
  def_property_readonly(
      "a",
      [](LPC_Frame self) {
        return py::array({self->nCoefficients}, self->a.cells);
      },
      py::return_value_policy::reference_internal, FRAME_A);
}

PRAAT_CLASS_BINDING(LPC)
{
  addTimeFrameSampledMixin(*this);

  NESTED_BINDINGS(LPC_Frame)

  doc() = CLASS_DOCSTRING;

  def_readonly("sampling_period", &structLPC::samplingPeriod,
               SAMPLING_PERIOD_DOCSTRING);
  def_readonly("max_n_coefficients", &structLPC::maxnCoefficients,
               MAX_N_COEFFICIENTS_DOCSTRING);

  def(
      "__len__", [](LPC self) { return self->nx; }, LEN_DOCSTRING);
  def(
      "__getitem__",
      [](LPC self, int i) {
        if (i < 0) i += self->nx; // Python-style negative indexing
        if (i < 0 || i >= self->nx)
          throw py::index_error("LPC index out of range");
        return &self->d_frames.cells[i];
      },
      "i"_a, py::return_value_policy::reference_internal, GETITEM_DOCSTRING);

  def(
      "__iter__",
      [](LPC self) {
        return py::make_iterator(self->d_frames.cells,
                                 self->d_frames.cells + self->nx);
      },
      py::keep_alive<0, 1>(), ITER_DOCSTRING);

  // FORM (NEW_LPC_to_Spectrum_slice
  def("to_spectrum_slice", &LPC_to_Spectrum, "time"_a,
      "minimum_frequency_resolution"_a = 20.0, "bandwidth_reduction"_a = 0.0,
      "deemphasis_frequency"_a = 50.0, TO_SPECTRUM_SLICE_DOCSTRING);

  // FORM (NEW_LPC_to_Spectrogram
  def("to_spectrogram", &LPC_to_Spectrogram,
      "minimum_frequency_resolution"_a = 20.0, "bandwidth_reduction"_a = 0.0,
      "deemphasis_frequency"_a = 50.0, TO_SPECTROGRAM_DOCSTRING);

  // FORM (NEW_LPC_to_LineSpectralFrequencies)
  def("to_line_spectral_frequencies", &LPC_to_LineSpectralFrequencies,
      "grid_size"_a = 0.0, TO_LINE_SPECTRAL_FREQUENCIES);

  // FORM (NEW1_LPC_Sound_to_LPC_robust
  def(
      "to_lpc_robust", [](LPC self, Sound sound, Positive<double> windowLength, Positive<double> preEmphasisFrequency, Positive<double> numberOfStandardDeviations, Positive<int> maximumNumberOfIterations, double tolerance, bool locationVariable) {
        return LPC_Sound_to_LPC_robust(self, sound, windowLength, preEmphasisFrequency, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, locationVariable);
      },
      "sound"_a.none(false), "window_length"_a = 0.025, "preemphasis_frequency"_a = 50.0, "number_of_std_dev"_a = 1.5, "maximum_number_of_iterations"_a = 5, "tolerance"_a = 0.000001, "variable_location"_a = false);
}

} // namespace parselmouth
