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
#include "utils/pybind11/NumericPredicates.h"

#include <praat/dwtools/Spectrum_extensions.h>
#include <praat/fon/Spectrum.h>
#include <praat/fon/Spectrum_and_Spectrogram.h>
#include <praat/fon/Sound_and_Spectrum.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

PRAAT_CLASS_BINDING(Spectrum) {
	using signature_cast_placeholder::_;

	def(py::init([](py::array_t<double, 0> values, Positive<double> maximumFrequency) {
		    auto ndim = values.ndim();
		    if (ndim > 2) {
			    throw py::value_error("Cannot create Spectrum from an array with more than 2 dimensions");
		    }
		    if (ndim == 2 && values.shape(0) > 2) {
			    throw py::value_error("Cannot create Spectrum from 2-dimensional array where the first dimension is greater than 2");
		    }

		    auto n = values.shape(ndim-1);
		    auto result = Spectrum_create(maximumFrequency, n);

		    if (ndim == 2) {
			    auto unchecked = values.unchecked<2>();
			    for (py::ssize_t i = 0; i < n; ++i) {
				    result->z[1][i+1] = unchecked(0, i);
				    result->z[2][i+1] = values.shape(1) == 2 ? unchecked(1, i) : 0.0;
			    }
		    }
		    else {
			    auto unchecked = values.unchecked<1>();
			    for (py::ssize_t i = 0; i < n; ++i) {
				    result->z[1][i+1] = unchecked(i);
				    result->z[2][i+1] = 0;
			    }
		    }

		    return result;
	    }),
	    "values"_a, "maximum_frequency"_a);

	def(py::init([](py::array_t<std::complex<double>, 0> values, Positive<double> maximumFrequency) {
		    auto ndim = values.ndim();
		    if (ndim > 1) {
			    throw py::value_error("Cannot create Spectrum from a complex array with more than 1 dimension");
		    }

		    auto n = values.shape(0);
		    auto result = Spectrum_create(maximumFrequency, n);

		    auto unchecked = values.unchecked<1>();
		    for (py::ssize_t i = 0; i < n; ++i) {
			    result->z[1][i+1] = unchecked(i).real();
			    result->z[2][i+1] = unchecked(i).imag();
		    }

		    return result;
	    }),
	    "values"_a, "maximum_frequency"_a);

	// TODO Constructor from Sound?

	// TODO Tabulate - List ?

	def("get_lowest_frequency", [](Spectrum self) { return self->xmin; });

	def_readonly("lowest_frequency", &structSpectrum::xmin);

	def_readonly("fmin", &structSpectrum::xmin);

	def("get_highest_frequency", [](Spectrum self) { return self->xmax; });

	def_readonly("highest_frequency", &structSpectrum::xmax);

	def_readonly("fmax", &structSpectrum::xmax);

	def("get_number_of_bins", [](Spectrum self) { return self->nx; });

	def_readonly("n_bins", &structSpectrum::nx);

	def_readonly("nf", &structSpectrum::nx);

	def("get_bin_width", [](Spectrum self) { return self->dx; });

	def_readonly("bin_width", &structSpectrum::dx);

	def_readonly("df", &structSpectrum::dx);

	def("get_frequency_from_bin_number", // TODO Somehow link with "Sampled" class?
	    [](Spectrum self, Positive<integer> bandNumber) { return Sampled_indexToX(self, bandNumber); },
	    "band_number"_a);

	def("get_bin_number_from_frequency", // TODO Somehow link with "Sampled" class?
	    [](Spectrum self, double frequency) { return Sampled_xToIndex(self, frequency); },
	    "frequency"_a);

	def("get_real_value_in_bin",
	    [](Spectrum self, Positive<integer> binNumber) {
		    if (binNumber > self->nx)
			    Melder_throw (U"Bin number must not exceed number of bins.");
		    return self->z[1][binNumber];
	    },
	    "bin_number"_a);

	def("get_imaginary_value_in_bin",
	    [](Spectrum self, Positive<integer> binNumber) {
		    if (binNumber > self->nx)
			    Melder_throw (U"Bin number must not exceed number of bins.");
		    return self->z[2][binNumber];
	    },
	    "bin_number"_a);

	def("get_value_in_bin",
	    [](Spectrum self, Positive<integer> binNumber) {
		    if (binNumber > self->nx)
			    Melder_throw (U"Bin number must not exceed number of bins.");
		    return std::complex<double>(self->z[1][binNumber], self->z[2][binNumber]);
	    },
	    "bin_number"_a);

	def("__getitem__",
	    [](Spectrum self, integer index) {
		    if (index < 0 || index >= self->nx)
			    throw py::index_error("bin index out of range");
		    return std::complex<double>(self->z[1][index+1], self->z[2][index+1]);
	    },
	    "index"_a);

	def("set_real_value_in_bin",
	    [](Spectrum self, Positive<integer> binNumber, double value) {
		    if (binNumber > self->nx)
			    Melder_throw (U"Bin number must not exceed number of bins.");
		    self->z[1][binNumber] = value;
	    },
	    "bin_number"_a, "value"_a);

	def("set_imaginary_value_in_bin",
	    [](Spectrum self, Positive<integer> binNumber, double value) {
		    if (binNumber > self->nx)
			    Melder_throw (U"Bin number must not exceed number of bins.");
		    self->z[2][binNumber] = value;
	    },
	    "bin_number"_a, "value"_a);

	def("set_value_in_bin",
	    [](Spectrum self, Positive<integer> binNumber, std::complex<double> value) {
		    if (binNumber > self->nx)
			    Melder_throw (U"Bin number must not exceed number of bins.");
		    self->z[1][binNumber] = value.real();
		    self->z[2][binNumber] = value.imag();
	    },
	    "bin_number"_a, "value"_a);

	def("__setitem__",
	    [](Spectrum self, integer index, std::complex<double> value) {
		    if (index < 0 || index >= self->nx)
			    throw py::index_error("bin index out of range");
		    self->z[1][index+1] = value.real();
		    self->z[2][index+1] = value.imag();
	    },
	    "index"_a, "value"_a);

	def("get_band_energy",
	    [](Spectrum self, std::optional<double> bandFloor, std::optional<double> bandCeiling) { return Spectrum_getBandEnergy(self, bandFloor.value_or(self->xmin), bandCeiling.value_or(self->xmax)); },
	    "band_floor"_a = std::nullopt, "band_ceiling"_a = std::nullopt);

	def("get_band_energy",
	    [](Spectrum self, std::pair<std::optional<double>, std::optional<double>> band) { return Spectrum_getBandEnergy(self, band.first.value_or(self->xmin), band.second.value_or(self->xmax)); },
	    "band"_a = std::pair(std::nullopt, std::nullopt));

	def("get_band_density",
	    [](Spectrum self, std::optional<double> bandFloor, std::optional<double> bandCeiling) { return Spectrum_getBandDensity(self, bandFloor.value_or(self->xmin), bandCeiling.value_or(self->xmax)); },
	    "band_floor"_a = std::nullopt, "band_ceiling"_a = std::nullopt);

	def("get_band_density",
	    [](Spectrum self, std::pair<std::optional<double>, std::optional<double>> band) { return Spectrum_getBandDensity(self, band.first.value_or(self->xmin), band.second.value_or(self->xmax)); },
	    "band"_a = std::pair(std::nullopt, std::nullopt));

	def("get_band_energy_difference",
	    [](Spectrum self, std::optional<double> lowBandFloor, std::optional<double> lowBandCeiling, std::optional<double> highBandFloor, std::optional<double> highBandCeiling) { return Spectrum_getBandEnergyDifference(self, lowBandFloor.value_or(self->xmin), lowBandCeiling.value_or(self->xmax), highBandFloor.value_or(self->xmin), highBandCeiling.value_or(self->xmax)); },
	    "low_band_floor"_a = std::nullopt, "low_band_ceiling"_a = std::nullopt, "high_band_floor"_a = std::nullopt, "high_band_ceiling"_a = std::nullopt);

	def("get_band_energy_difference",
	    [](Spectrum self, std::pair<std::optional<double>, std::optional<double>> lowBand, std::pair<std::optional<double>, std::optional<double>> highBand) { return Spectrum_getBandEnergyDifference(self, lowBand.first.value_or(self->xmin), lowBand.second.value_or(self->xmax), highBand.first.value_or(self->xmin), highBand.second.value_or(self->xmax)); },
	    "low_band"_a = std::pair(std::nullopt, std::nullopt), "high_band"_a = std::pair(std::nullopt, std::nullopt));

	def("get_band_density_difference",
	    [](Spectrum self, std::optional<double> lowBandFloor, std::optional<double> lowBandCeiling, std::optional<double> highBandFloor, std::optional<double> highBandCeiling) { return Spectrum_getBandDensityDifference(self, lowBandFloor.value_or(self->xmin), lowBandCeiling.value_or(self->xmax), highBandFloor.value_or(self->xmin), highBandCeiling.value_or(self->xmax)); },
	    "low_band_floor"_a = std::nullopt, "low_band_ceiling"_a = std::nullopt, "high_band_floor"_a = std::nullopt, "high_band_ceiling"_a = std::nullopt);

	def("get_band_density_difference",
	    [](Spectrum self, std::pair<std::optional<double>, std::optional<double>> lowBand, std::pair<std::optional<double>, std::optional<double>> highBand) { return Spectrum_getBandDensityDifference(self, lowBand.first.value_or(self->xmin), lowBand.second.value_or(self->xmax), highBand.first.value_or(self->xmin), highBand.second.value_or(self->xmax)); },
	    "low_band"_a = std::pair(std::nullopt, std::nullopt), "high_band"_a = std::pair(std::nullopt, std::nullopt));

	def("get_centre_of_gravity",
	    args_cast<_, Positive<_>>(Spectrum_getCentreOfGravity),
	    "power"_a = 2.0);

	def("get_center_of_gravity",
	    args_cast<_, Positive<_>>(Spectrum_getCentreOfGravity),
	    "power"_a = 2.0);

	def("get_standard_deviation",
	    args_cast<_, Positive<_>>(Spectrum_getStandardDeviation),
	    "power"_a = 2.0);

	def("get_skewness",
	    args_cast<_, Positive<_>>(Spectrum_getSkewness),
	    "power"_a = 2.0);

	def("get_kurtosis",
	    args_cast<_, Positive<_>>(Spectrum_getKurtosis),
	    "power"_a = 2.0);

	def("get_central_moment",
	    args_cast<_, Positive<_>, Positive<_>>(Spectrum_getCentralMoment),
	    "moment"_a, "power"_a = 2.0);

	// TODO Filter Hann bands

	def("cepstral_smoothing",
	    args_cast<_, Positive<_>>(Spectrum_cepstralSmoothing),
	    "bandwidth"_a = 500.0);

	def("lpc_smoothing",
	    args_cast<_, Positive<_>, Positive<_>>(Spectrum_lpcSmoothing),
	    "num_peaks"_a = 5, "pre_emphasis_from"_a = 50.0);

	def("to_sound",
	    &Spectrum_to_Sound);

	def("to_spectrogram",
	    &Spectrum_to_Spectrogram);

	// TODO More stuff in praat_David_init, for some reason
}

} // namespace parselmouth
