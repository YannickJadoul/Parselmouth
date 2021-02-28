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

#include "LineSpectralFrequencies_docstrings.h"
#include "TimeClassAspects.h"

#include <praat/LPC/LPC_and_LineSpectralFrequencies.h>
#include <praat/LPC/LineSpectralFrequencies.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth
{

PRAAT_CLASS_BINDING(LineSpectralFrequencies)
{
  addTimeFrameSampledMixin(*this);

  doc() = CLASS_DOCSTRING;

  oo_DOUBLE(maximumFrequency) oo_INT(maximumNumberOfFrequencies)

      def_readonly("maximum_frequency",
                   &structLineSpectralFrequencies::maximumFrequency,
                   MAXIMUM_FREQUENCY_DOCSTRING);
  def_readonly("maximum_number_of_frequencies",
               &structLineSpectralFrequencies::maximumNumberOfFrequencies,
               MAXIMUM_NUMBER_OF_FREQUENCIES_DOCSTRING);

  def(
      "__len__", [](LineSpectralFrequencies self) { return self->nx; },
      LEN_DOCSTRING);
  def(
      "__getitem__",
      [](LineSpectralFrequencies self, int i) {
        if (i < 0) i += self->nx; // Python-style negative indexing
        if (i < 0 || i >= self->nx) throw py::index_error("Index out of range");
        structLineSpectralFrequencies_Frame &item = self->d_frames.cells[i];
        return py::array({item.numberOfFrequencies}, item.frequencies.cells);
      },
      "i"_a, py::return_value_policy::reference_internal, GETITEM_DOCSTRING);

  // DIRECT (NEW_LineSpectralFrequencies_to_LPC) {
  def("to_lpc", &LineSpectralFrequencies_to_LPC, TO_LPC_DOCSTRING);
}

} // namespace parselmouth
