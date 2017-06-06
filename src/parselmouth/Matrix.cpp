/*
 * Copyright (C) 2017  Yannick Jadoul
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

#include <pybind11/numpy.h> // TODO numpy dependency of python library
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

using std::experimental::optional;
using std::experimental::nullopt;

void Binding<Matrix>::init() {
	def_property_readonly("values",
	                      [](Matrix self) { return py::array_t<double, py::array::f_style>({static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, &self->z[1][1], py::cast(self)); });

	def("as_array",
	    [](Matrix self) { return py::array_t<double, py::array::f_style>({static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, &self->z[1][1], py::cast(self)); });

	def_buffer([](Matrix self) { return py::buffer_info(&self->z[1][1], {static_cast<ssize_t>(self->nx), static_cast<ssize_t>(self->ny)}, {static_cast<ssize_t>(sizeof(double)), static_cast<ssize_t>(self->nx * sizeof(double))}); });

	def("formula", // TODO Make formula into some kind of class?
	    [] (Matrix self, const std::u32string &formula, optional<double> fromX, optional<double> toX, optional<double> fromY, optional<double> toY) {
		    if (!fromX && !toX && !fromY && !toY) {
			    Matrix_formula(self, formula.c_str(), nullptr, nullptr);
		    }
		    else {
			    Matrix_formula_part(self, fromX.value_or(self->xmin), toX.value_or(self->xmax), fromY.value_or(self->ymin), toY.value_or(self->ymax), formula.c_str(), nullptr, nullptr);
		    }
	    },
	    "formula"_a, "from_x"_a = nullopt, "to_x"_a = nullopt, "from_y"_a = nullopt, "to_y"_a = nullopt);

	def("formula",
	    [] (Matrix self, const std::u32string &formula, std::pair<optional<double>, optional<double>> xRange, std::pair<optional<double>, optional<double>> yRange) {
		    Matrix_formula_part(self, xRange.first.value_or(self->xmin), xRange.second.value_or(self->xmax), yRange.first.value_or(self->ymin), yRange.second.value_or(self->ymax), formula.c_str(), nullptr, nullptr);
	    },
	    "formula"_a, "x_range"_a = std::make_pair(nullopt, nullopt), "y_range"_a = std::make_pair(nullopt, nullopt));
}

} // namespace parselmouth
