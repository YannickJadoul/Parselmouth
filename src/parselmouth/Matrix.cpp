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
	                      [](Matrix self) {
		                      return py::array({static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)},
		                                       {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)},
		                                       &self->z[1][1], py::cast(self));
	                      });

	def("as_array",
	    [](Matrix self) {
		    return py::array({static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)},
		                     {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)},
		                     &self->z[1][1], py::cast(self));
	    });

	def_buffer([](Matrix self) { // TODO Simplify once pybind11 gets updated // TODO Think about the children!
		return py::buffer_info(&self->z[1][1], sizeof(double), py::format_descriptor<double>::format(), 2,
		                       {static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)},
		                       {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)});
	});

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
