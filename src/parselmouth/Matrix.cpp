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

#include "utils/SignatureCast.h"
#include "utils/praat/MelderUtils.h"
#include "utils/pybind11/NumericPredicates.h"

#include <praat/fon/Matrix.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(Matrix, py::buffer_protocol()) {
	using signature_cast_placeholder::_;

	// TODO Constructors (i.e., from numpy array, ...)

	def_property("values", // TODO Check Row-major/column-major things
	             [](Matrix self) { return py::array_t<double, py::array::c_style>({static_cast<size_t>(self->ny), static_cast<size_t>(self->nx)}, &self->z[1][1], py::cast(self)); },
	             [](Matrix self, py::array_t<double, 0> values) {
		             auto ndim = values.ndim();
		             if (ndim > 2) {
			             throw py::value_error("Cannot set Matrix values with an array with more than two dimensions");
		             }

		             auto nx = values.shape(ndim-1);
		             auto ny = ndim == 2 ? values.shape(0) : 1;

		             if (ndim == 2 && values.data(0, 0) == &self->z[1][1] && nx == self->nx && ny == self->ny && values.strides(0) == sizeof(double) && values.strides(1) == py::ssize_t{sizeof(double)} * nx) {
			             // This is the exact same array as we would return as the getter of this property would return, pointing to the memory we already have and own!
			             return;
		             }
		             else if (nx != self->nx || ny != self->ny) {
			             throw py::value_error("Cannot change dimensions of Matrix values");
		             }
		             else {
			             if (ndim == 2) {
				             auto unchecked = values.unchecked<2>();
				             for (py::ssize_t i = 0; i < ny; ++i)
					             for (py::ssize_t j = 0; j < nx; ++j)
						             self->z[i+1][j+1] = unchecked(i, j);
			             }
			             else {
				             auto unchecked = values.unchecked<1>();
				             for (py::ssize_t j = 0; j < nx; ++j)
					             self->z[1][j+1] = unchecked(j);
			             }
		             }
	             });

	def("as_array",
	    [](Matrix self) { return py::array_t<double, py::array::c_style>({static_cast<size_t>(self->ny), static_cast<size_t>(self->nx)}, &self->z[1][1], py::cast(self)); });

	def_buffer([](Matrix self) { return py::buffer_info(&self->z[1][1], {static_cast<py::ssize_t>(self->ny), static_cast<py::ssize_t>(self->nx)}, {static_cast<py::ssize_t>(self->nx * sizeof(double)), static_cast<py::ssize_t>(sizeof(double))}); });

	def("save_as_matrix_text_file",
	    [](Matrix self, const std::u32string &filePath) {
		    auto file = pathToMelderFile(filePath);
		    Matrix_writeToMatrixTextFile(self, &file);
	    },
	    "file_path"_a);

	def("save_as_headerless_spreadsheet_file",
	    [](Matrix self, const std::u32string &filePath) {
		    auto file = pathToMelderFile(filePath);
		    Matrix_writeToHeaderlessSpreadsheetFile(self, &file);
	    },
	    "file_path"_a);

	def("get_lowest_x", [](Matrix self) { return self->xmin; });

	def("get_highest_x", [](Matrix self) { return self->xmax; });

	def("get_lowest_y", [](Matrix self) { return self->ymin; });

	def("get_highest_y", [](Matrix self) { return self->ymax; });

	def("get_number_of_rows", [](Matrix self) { return self->ny; });

	def_readonly("n_rows", &structMatrix::ny);

	def("get_number_of_columns", [](Matrix self) { return self->nx; });

	def_readonly("n_columns", &structMatrix::nx);

	def("get_row_distance", [](Matrix self) { return self->dy; });

	def("get_column_distance", [](Matrix self) { return self->dx; });

	def("get_y_of_row",
	    args_cast<_, Positive<integer>>(Matrix_rowToY),
	    "row_number"_a);

	def("get_x_of_column",
	    args_cast<_, Positive<integer>>(Matrix_columnToX),
	    "column_number"_a);

	def("get_value_in_cell",
	    [](Matrix self, Positive<integer> rowNumber, Positive<integer> columnNumber) {
		    if (rowNumber > self->ny) Melder_throw (U"Row number must not exceed number of rows.");
		    if (columnNumber > self->nx) Melder_throw (U"Column number must not exceed number of columns.");
		    return self->z[rowNumber][columnNumber]; },
	    "row_number"_a, "column_number"_a);

	def("get_value_at_xy",
	    &Matrix_getValueAtXY,
	    "x"_a, "y"_a);

	def("at_xy",
	    &Matrix_getValueAtXY,
	    "x"_a, "y"_a);

	def("get_minimum",
		[](Matrix self) {
			double minimum = undefined;
			double maximum = undefined;
			Matrix_getWindowExtrema(self, 0, 0, 0, 0, &minimum, &maximum);
			return minimum;
		});

	def("get_maximum",
	    [](Matrix self) {
		    double minimum = undefined;
		    double maximum = undefined;
		    Matrix_getWindowExtrema(self, 0, 0, 0, 0, &minimum, &maximum);
		    return maximum;
	    });

	def("get_sum",
	    &Matrix_getSum);

	def("formula", // TODO Make formula into some kind of class?
	    [](Matrix self, const std::u32string &formula, std::optional<double> fromX, std::optional<double> toX, std::optional<double> fromY, std::optional<double> toY) {
		    if (!fromX && !toX && !fromY && !toY) {
			    Matrix_formula(self, formula.c_str(), nullptr, nullptr);
		    }
		    else {
			    Matrix_formula_part(self, fromX.value_or(self->xmin), toX.value_or(self->xmax), fromY.value_or(self->ymin), toY.value_or(self->ymax), formula.c_str(), nullptr, nullptr);
		    }
	    },
	    "formula"_a, "from_x"_a = std::nullopt, "to_x"_a = std::nullopt, "from_y"_a = std::nullopt, "to_y"_a = std::nullopt);

	def("formula",
	    [](Matrix self, const std::u32string &formula, std::pair<std::optional<double>, std::optional<double>> xRange, std::pair<std::optional<double>, std::optional<double>> yRange) {
		    Matrix_formula_part(self, xRange.first.value_or(self->xmin), xRange.second.value_or(self->xmax), yRange.first.value_or(self->ymin), yRange.second.value_or(self->ymax), formula.c_str(), nullptr, nullptr);
	    },
	    "formula"_a, "x_range"_a = std::pair(std::nullopt, std::nullopt), "y_range"_a = std::pair(std::nullopt, std::nullopt));

	def("set_value",
	    [](Matrix self, Positive<integer> rowNumber, Positive<integer> columnNumber, double newValue) {
		    if (rowNumber > self->ny) Melder_throw (U"Your row number should not be greater than your number of rows.");
		    if (columnNumber > self->nx) Melder_throw (U"Your column number should not be greater than your number of columns.");
		    self->z[rowNumber][columnNumber] = newValue;
	    },
	    "row_number"_a, "column_number"_a, "new_value"_a);

	// TODO But wait, there's more! Analyse, Synthesize, & Cast subsections, and of course praat_David_init...
}

} // namespace parselmouth
