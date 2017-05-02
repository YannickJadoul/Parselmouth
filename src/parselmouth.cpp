#include "parselmouth/Parselmouth.h"

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "fon/Formant.h"
#include "fon/Manipulation.h"
#include "fon/Sound_and_Spectrogram.h"
#include "dwsys/NUMmachar.h"
#include "dwtools/Spectrogram_extensions.h"


void initializePraat()
{
	// TODO Look at praat initialization again, see if there's a better solution that ad-hoc copy-pasting (praatlib_init?)
	NUMmachar ();
	NUMinit ();
	Melder_alloc_init ();
	Melder_message_init ();

	Melder_batch = true;
}


namespace py = pybind11;
using namespace py::literals;


PYBIND11_PLUGIN(parselmouth) {
	initializePraat();

	py::module m("parselmouth");
	parselmouth::PraatBindings bindings(m);


    static py::exception<MelderError> melderErrorException(m, "PraatError", PyExc_RuntimeError);
	py::register_exception_translator([](std::exception_ptr p) {
			try {
				if (p) std::rethrow_exception(p);
			}
			catch (const MelderError &e) {
				std::string message(Melder_peek32to8(Melder_getError()));
				message.erase(message.length() - 1);
				Melder_clearError();
				melderErrorException(message.c_str());
			}});


	enum class Interpolation
	{
		NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
		LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
		CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
		SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
		SINC700 = Vector_VALUE_INTERPOLATION_SINC700
	};

	py::enum_<Interpolation>(m, "Interpolation")
		.value("nearest", Interpolation::NEAREST)
		.value("linear", Interpolation::LINEAR)
		.value("cubic", Interpolation::CUBIC)
		.value("sinc70", Interpolation::SINC70)
		.value("sinc700", Interpolation::SINC700)
	;


	py::enum_<kSound_windowShape>(m, "WindowShape")
		.value("rectangular", kSound_windowShape_RECTANGULAR)
		.value("triangular", kSound_windowShape_TRIANGULAR)
		.value("parabolic", kSound_windowShape_PARABOLIC)
		.value("hanning", kSound_windowShape_HANNING)
		.value("hamming", kSound_windowShape_HAMMING)
		.value("gaussian1", kSound_windowShape_GAUSSIAN_1)
		.value("gaussian2", kSound_windowShape_GAUSSIAN_2)
		.value("gaussian3", kSound_windowShape_GAUSSIAN_3)
		.value("gaussian4", kSound_windowShape_GAUSSIAN_4)
		.value("gaussian5", kSound_windowShape_GAUSSIAN_5)
		.value("kaiser1", kSound_windowShape_KAISER_1)
		.value("kaiser2", kSound_windowShape_KAISER_2)
	;

	py::enum_<kSounds_convolve_scaling>(m, "AmplitudeScaling")
		.value("integral", kSounds_convolve_scaling_INTEGRAL)
		.value("sum", kSounds_convolve_scaling_SUM)
		.value("normalize", kSounds_convolve_scaling_NORMALIZE)
		.value("peak_0_99", kSounds_convolve_scaling_PEAK_099)
	;

	py::enum_<kSounds_convolve_signalOutsideTimeDomain>(m, "SignalOutsideTimeDomain")
		.value("zero", kSounds_convolve_signalOutsideTimeDomain_ZERO)
		.value("similar", kSounds_convolve_signalOutsideTimeDomain_SIMILAR)
	;

	py::enum_<kSound_to_Spectrogram_windowShape>(m, "SpectralAnalysisWindowShape")
		.value("square", kSound_to_Spectrogram_windowShape_SQUARE)
		.value("hamming", kSound_to_Spectrogram_windowShape_HAMMING)
		.value("bartlett", kSound_to_Spectrogram_windowShape_BARTLETT)
		.value("welch", kSound_to_Spectrogram_windowShape_WELCH)
		.value("hanning", kSound_to_Spectrogram_windowShape_HANNING)
		.value("gaussian", kSound_to_Spectrogram_windowShape_GAUSSIAN)
	;

	initThing(bindings);
	initSound(bindings);

	bindings.get<MFCC>()
		//.def(constructor(&Sound_to_MFCC,
		//		(arg("self"), arg("sound"), arg("number_of_coefficients") = 12, arg("analysis_width") = 0.015, arg("dt") = 0.005, arg("f1_mel") = 100.0, arg("fmax_mel") = 0.0, arg("df_mel") = 100.0)))

		.def("get_coefficients",
				[] (MFCC mfcc)
					{
						auto maxCoefficients = CC_getMaximumNumberOfCoefficients(mfcc, 1, mfcc->nx);
						py::array_t<double> array({static_cast<size_t>(mfcc->nx), static_cast<size_t>(maxCoefficients + 1)}, nullptr);

						for (auto i = 0; i < mfcc->nx; ++i) {
							*array.mutable_data(i, 0) = mfcc->frame[i+1].c0;
							for (auto j = 1; j <= maxCoefficients; ++j) {
								*array.mutable_data(i, j) = (j <= mfcc->frame[i+1].numberOfCoefficients) ? mfcc->frame[i+1].c[j] : std::numeric_limits<double>::quiet_NaN();
							}
						}

						return array;
					})
		.def("to_mel_spectrogram",
				&MFCC_to_MelSpectrogram,
				"from_coefficient"_a = 0, "to_coefficient"_a = 0, "include_c0"_a = true);
	;


	py::enum_<kPitch_unit>(m, "PitchUnit")
		.value("hertz", kPitch_unit_HERTZ)
		.value("hertz_logarithmic", kPitch_unit_HERTZ_LOGARITHMIC)
		.value("mel", kPitch_unit_MEL)
		.value("log_hertz", kPitch_unit_LOG_HERTZ)
		.value("mel", kPitch_unit_MEL)
		.value("semitones_1", kPitch_unit_SEMITONES_1)
		.value("semitones_100", kPitch_unit_SEMITONES_100)
		.value("semitones_200", kPitch_unit_SEMITONES_200)
		.value("semitones_440", kPitch_unit_SEMITONES_440)
		.value("erb", kPitch_unit_ERB)
	;

	bindings.get<Pitch>()
		.def("get_value",
				[] (Pitch self, double time, kPitch_unit unit, bool interpolate) { return Pitch_getValueAtTime(self, time, unit, interpolate); },
				"time"_a, "unit"_a = kPitch_unit_HERTZ, "interpolate"_a = true)
	;


	bindings.get<Intensity>()
		.def("get_value", // TODO Should be part of Vector class
				[] (Intensity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				"time"_a, "interpolation"_a = Interpolation::CUBIC)
	;


	bindings.get<Harmonicity>()
		.def("get_value", // TODO Should be part of Vector class
				[] (Harmonicity self, double time, Interpolation interpolation) { return Vector_getValueAtX(self, time, 1, static_cast<int>(interpolation)); },
				"time"_a, "interpolation"_a = Interpolation::CUBIC)
	;

	py::class_<structMelSpectrogram, autoMelSpectrogram, structThing>(m, "MelSpectrogram")
		.def("as_array",
				[] (MelSpectrogram self) { { return py::array_t<double>({ static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)}, &self->z[1][1], py::cast(self)); } })

		.def_readonly("xmin",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::xmin))

		.def_readonly("xmax",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::xmax))

		.def_readonly("x1",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::x1))

		.def_readonly("dx",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::dx))

		.def_readonly("nx",
				static_cast<int structMelSpectrogram::*>(&structMelSpectrogram::nx))

		.def_readonly("fmin",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::ymin))

		.def_readonly("fmax",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::ymax))

		.def_readonly("f1",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::y1))

		.def_readonly("df",
				static_cast<double structMelSpectrogram::*>(&structMelSpectrogram::dy))

		.def_readonly("nf",
				static_cast<long structMelSpectrogram::*>(&structMelSpectrogram::ny))

		.def("frequency_to_hertz",
				&structMelSpectrogram::v_frequencyToHertz,
				"frequency"_a)
	;

	bindings.get<Spectrogram>()
		.def("as_array",
				[] (Spectrogram self) { { return py::array_t<double>({ static_cast<size_t>(self->nx), static_cast<size_t>(self->ny)}, {sizeof(double), static_cast<size_t>(self->nx) * sizeof(double)}, &self->z[1][1], py::cast(self)); } })

		.def_readonly("xmin",
				static_cast<double structSpectrogram::*>(&structSpectrogram::xmin))

		.def_readonly("xmax",
				static_cast<double structSpectrogram::*>(&structSpectrogram::xmax))

		.def_readonly("x1",
				static_cast<double structSpectrogram::*>(&structSpectrogram::x1))

		.def_readonly("dx",
				static_cast<double structSpectrogram::*>(&structSpectrogram::dx))

		.def_readonly("nx",
				static_cast<int structSpectrogram::*>(&structSpectrogram::nx))

		.def_readonly("fmin",
				static_cast<double structSpectrogram::*>(&structSpectrogram::ymin))

		.def_readonly("fmax",
				static_cast<double structSpectrogram::*>(&structSpectrogram::ymax))

		.def_readonly("f1",
				static_cast<double structSpectrogram::*>(&structSpectrogram::y1))

		.def_readonly("df",
				static_cast<double structSpectrogram::*>(&structSpectrogram::dy))

		.def_readonly("nf",
				static_cast<long structSpectrogram::*>(&structSpectrogram::ny))
	;

	bindings.get<Sound>()
		.def("to_manipulation",
		      &Sound_to_Manipulation,
		      "time_step"_a = 0.01, "minimum_pitch"_a = 75.0, "maximum_pitch"_a = 600.0)
	;

	py::class_<structPitchTier, autoPitchTier, structThing>(m, "PitchTier")
		.def("shift_frequencies",
				[] (PitchTier self, double tmin, double tmax, double shift, kPitch_unit unit) { PitchTier_shiftFrequencies(self, tmin, tmax, shift, unit); },
				"tmin"_a, "tmax"_a, "shift"_a = -20.0, "unit"_a = kPitch_unit::kPitch_unit_HERTZ)
		.def_readonly("tmin",
				static_cast<double structPitchTier::*>(&structPitchTier::xmin))
		.def_readonly("tmax",
				static_cast<double structPitchTier::*>(&structPitchTier::xmax))
	;

	py::class_<structManipulation, autoManipulation, structThing>(m, "Manipulation")
		.def("get_resynthesis_lpc",
		     [] (Manipulation self) { return Manipulation_to_Sound(self, Manipulation_PITCH_LPC); })
		.def("get_resynthesis_overlap_add",
		     [] (Manipulation self) { return Manipulation_to_Sound(self, Manipulation_OVERLAPADD); })
		.def_property_readonly("pitch",
				[] (Manipulation self) { return self->pitch.get(); })
	;

	return m.ptr();
}
