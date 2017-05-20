#include "Parselmouth.h"

#include "dwtools/Sound_extensions.h"
#include "dwtools/Sound_to_MFCC.h"
#include "fon/Sound_and_Spectrogram.h"
#include "fon/Sound_to_Harmonicity.h"
#include "fon/Sound_to_Intensity.h"
#include "fon/Sound_to_Pitch.h"

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

template <typename T>
using optional = std::experimental::optional<T>;
using std::experimental::nullopt;

namespace {

template <typename Class>
void constructInstanceHolder(py::handle self, typename Class::holder_type &&holder)
{
	// TODO HACK DETECTED: Remove/cleanup
	auto instance = reinterpret_cast<typename Class::instance_type *>(self.ptr());
	auto &internals = py::detail::get_internals();
	internals.registered_instances.erase(internals.registered_instances.find(instance->value));
	::operator delete(instance->value);
	instance->value = py::detail::holder_helper<typename Class::holder_type>::get(holder);
	instance->holder = std::move(holder);
	instance->holder_constructed = true;
	instance->owned = true;
	internals.registered_instances.emplace(instance->value, instance);
}

template <typename T, typename Container>
OrderedOf<T> referencesToOrderedOf(const Container &container)
{
	OrderedOf<T> orderedOf;
	std::for_each(begin(container), end(container), [&orderedOf] (T &item) { orderedOf.addItem_ref(&item); });
	return orderedOf;
}

} // namespace

void initSound(parselmouth::PraatBindings &bindings)
{
	bindings.get<Sound>()
			// TODO Constructors: from file (?)
			.def("__init__", // TODO sampling_frequency is POSITIVE // TODO Use init_factory once part of pybind11
			     [] (py::handle self, py::array_t<double> samples, double samplingFrequency, double startTime) {
				     auto ndim = samples.ndim();
				     if (ndim > 2) {
					     throw py::value_error("Cannot create Sound from an array with more than 2 dimensions");
				     }

				     auto nx = samples.shape(0);
				     auto ny = ndim == 2 ? samples.shape(1) : 1;
				     auto result = Sound_create(ny, startTime, startTime + nx / samplingFrequency, nx, 1.0 / samplingFrequency, startTime + 0.5 / samplingFrequency);
				     for (auto i = 0; i < nx; ++i) {
					     if (ndim == 2) {
						     for (auto j = 0; j < ny; ++j) {
							     result->z[j + 1][i + 1] = samples.at(i, j); // TODO Unsafe accessor in later versions of pybind11?
						     }
					     }
					     else {
						     result->z[1][i+1] = samples.at(i);
					     }
				     }

				     constructInstanceHolder<Binding<Sound>>(self, std::move(result)); // TODO init_factory
			     },
			     "samples"_a, "sampling_frequency"_a, "start_time"_a = 0.0)

			.def("__init__",
			     [] (py::handle self, const std::string &filePath) { // TODO Think about bytes vs unicode again // TODO Use init_factory once part of pybind11
				     structMelderFile file = {};
				     Melder_relativePathToFile(Melder_peek8to32(filePath.c_str()), &file);
				     constructInstanceHolder<Binding<Sound>>(self, Sound_readFromSoundFile(&file));
			     },
			     "file_path"_a)

			// TODO Constructor from file or io.IOBase?
			// TODO Constructor from Praat-format file?

			.def("autocorrelate",
			     &Sound_autoCorrelate,
			     "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			// TODO Make sure pybind11's std::reference_wrapper does not accept None/nullptr
			.def_static("combine_to_stereo",
			            [] (const std::vector<std::reference_wrapper<structSound>> &sounds) { auto ordered = referencesToOrderedOf<structSound>(sounds); return Sounds_combineToStereo(&ordered); },
			            "sounds"_a)

			.def_static("concatenate", // TODO Overlap is POSITIVE
			            [] (const std::vector<std::reference_wrapper<structSound>> &sounds, double overlap) { auto ordered = referencesToOrderedOf<structSound>(sounds); return Sounds_concatenate(ordered, overlap); },
			            "sounds"_a, "overlap"_a = 0.0)
			// TODO concatenate recoverably
			// TODO concatenate as member function?

			.def("convert_to_mono",
			     &Sound_convertToMono)

			.def("convert_to_stereo",
			     &Sound_convertToStereo)

			// TODO Automatically wrap to have Sound -> structSound& ?
			.def("convolve",
			     [] (Sound self, structSound &other, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain &signal_outside_time_domain) { return Sounds_convolve(self, &other, scaling, signal_outside_time_domain); },
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)

			.def("cross_correlate",
			     [] (Sound self, structSound &other, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain &signal_outside_time_domain) { return Sounds_crossCorrelate(self, &other, scaling, signal_outside_time_domain); },
			     "other"_a, "scaling"_a = kSounds_convolve_scaling_PEAK_099, "signal_outside_time_domain"_a = kSounds_convolve_signalOutsideTimeDomain_ZERO)
			// TODO Cross-correlate (short)?

			.def("de_emphasize",
			     [] (Sound self, double fromFrequency, bool normalize) {
				     Sound_deEmphasis (self, fromFrequency);
				     if (normalize) {
					     Vector_scale(self, 0.99);
				     }
			     },
			     "from_frequency"_a = 50.0, "normalize"_a = true) // TODO "from" / "from_frequency" ? Not POSITIVE now!?

			.def("deepen_band_modulation", // TODO All arguments POSITIVE
			     &Sound_deepenBandModulation,
			     "enhancement"_a = 20.0, "from_frequency"_a = 300.0, "to_frequency"_a = 8000.0, "slow_modulation"_a = 3.0, "fast_modulation"_a = 30.0, "band_smoothing"_a = 100.0)

			.def("extract_all_channels",
			     [] (Sound self) {
				     std::vector<Sound> result; // TODO Make std::vector<autoSound>
				     result.reserve(self->ny);
				     for (auto i = 1; i <= self->ny; ++i) {
					     result.emplace_back(Sound_extractChannel(self, i).releaseToAmbiguousOwner());
				     }
				     return result;
			     })

			.def("extract_channel", // TODO Channel POSITIVE? (Actually CHANNEL; >= 1, but does not always have intended result (e.g., Set value at sample...))
			     &Sound_extractChannel,
			     "channel"_a)

			.def("extract_channel", // TODO Channel enum type?
			     [] (Sound self, std::string channel) {
				     std::transform(channel.begin(), channel.end(), channel.begin(), static_cast<int (*)(int)>(&std::tolower));
				     if (channel == "left")
					     return Sound_extractChannel(self, 1);
				     if (channel == "right")
					     return Sound_extractChannel(self, 2);
				     Melder_throw(U"'channel' can only be 'left' or 'right'"); // TODO Melder_throw or throw PraatError ?
			     })

			.def("extract_left_channel",
			     [] (Sound self) { return Sound_extractChannel(self, 1); })

			.def("extract_right_channel",
			     [] (Sound self) { return Sound_extractChannel(self, 2); })

			.def("extract_part", // TODO relativeWidth is POSITIVE // TODO Something for optional<double> for from and to in Sounds?
			     [] (Sound self,optional<double> from, optional<double> to, kSound_windowShape windowShape, double relativeWidth, bool preserveTimes) { return Sound_extractPart(self, from.value_or(self->xmin), to.value_or(self->xmax), windowShape, relativeWidth, preserveTimes); },
			     "from"_a = nullopt, "to"_a = nullopt, "window_shape"_a = kSound_windowShape_RECTANGULAR, "relative_width"_a = 1.0, "preserve_times"_a = false)

			.def("extract_part_for_overlap", // TODO Overlap is POSITIVE
			     [] (Sound self, optional<double> from, optional<double> to, double overlap) { return Sound_extractPartForOverlap(self, from.value_or(self->xmin), to.value_or(self->xmax), overlap); },
			     "from"_a = nullopt, "to"_a = nullopt, "overlap"_a)

			// TODO Filters
			// TODO Group different filters into enum/class/...?

			// TODO Formula and Formula (part)

			.def("get_energy",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getEnergy(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullopt, "to"_a = nullopt)

			.def("get_energy_in_air",
			     &Sound_getEnergyInAir)

			.def("get_index_from_time",
			     &Sampled_xToIndex,
			     "time"_a)

			.def("get_intensity", // TODO Get intensity (dB) -> get_intensity_dB/get_intensity_db
			     &Sound_getIntensity_dB)

			.def("get_nearest_zero_crossing", // TODO Channel is CHANNEL
			     [](Sound self, double time, long channel) {
				     if (channel > self->ny) channel = 1;
				     return Sound_getNearestZeroCrossing (self, time, channel);
			     },
			     "time"_a, "channel"_a = 1)

			.def("get_number_of_channels",
			     [] (Sound self) { return self->ny; })

			.def_readonly("num_channels",
			              static_cast<long structSound::*>(&structSound::ny)) // TODO Remove static_cast once SampledXY is exported, or once this is fixed

			.def("get_number_of_samples",
			     [] (Sound self) { return self->nx; })

			.def_readonly("num_samples",
			              static_cast<int32 structSound::*>(&structSound::nx)) // TODO Remove static_cast once Sampled is exported, or once this is fixed

			.def("get_power",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getPower(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullopt, "to"_a = nullopt)

			.def("get_power_in_air",
			     &Sound_getPowerInAir)

			.def("get_root_mean_square",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getRootMeanSquare(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullopt, "to"_a = nullopt)

			.def("get_rms",
			     [] (Sound self, optional<double> from, optional<double> to) { return Sound_getRootMeanSquare(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullopt, "to"_a = nullopt)

			.def("get_sampling_period",
			     [] (Sound self) { return self->dx; })

			.def_property("sampling_period",
			              [] (Sound self) { return self->dx; },
			              [] (Sound self, double period) { Sound_overrideSamplingFrequency(self, 1 / period); })

			.def("get_sampling_frequency",
			     [] (Sound self) { return 1 / self->dx; })

			.def_property("sampling_frequency",
			              [] (Sound self) { return 1 / self->dx; },
			              [] (Sound self, double frequency) { Sound_overrideSamplingFrequency(self, frequency); })

			.def("get_time_from_index", // TODO PraatIndex to distinguish 1-based silliness?
			     [] (Sound self, long sample) { return Sampled_indexToX(self, sample); },
			     "sample"_a)

			// TODO Get value at sample index
			// TODO Set value at sample index


			.def("lengthen", // TODO Lengthen (Overlap-add) ? // TODO All parameters are POSITIVE
			     [] (Sound self, double minimumPitch, double maximumPitch, double factor) {
				     if (minimumPitch >= maximumPitch) Melder_throw (U"Maximum pitch should be greater than minimum pitch.");
				     return Sound_lengthen_overlapAdd(self, minimumPitch, maximumPitch, factor);
			     },
			     "minimum_pitch"_a = 75.0, "maximum_pitch"_a = 600.0, "factor"_a)

			.def("multiply_by_window",
			     &Sound_multiplyByWindow,
			     "window_shape"_a)

			.def("override_sample_frequency", // TODO Rate vs. frequency? // TODO Setter of sample_rate? // TODO newFrequency is POSITIVE
			     [] (Sound self, double newFrequency) { Sound_overrideSamplingFrequency(self, newFrequency); },
			     "new_frequency"_a)

			.def("pre_emphasize",
			     [] (Sound self, double fromFrequency, bool normalize) {
				     Sound_preEmphasis(self, fromFrequency);
				     if (normalize) {
					     Vector_scale(self, 0.99);
				     }
			     },
			     "from_frequency"_a = 50.0, "normalize"_a = true) // TODO "from" / "from_frequency" ? Not POSITIVE now!?

			.def("resample",
			     &Sound_resample,
			     "new_frequency"_a, "precision"_a = 50)

			.def("reverse",
			     [] (Sound self, optional<double> from, optional<double> to) { Sound_reverse(self, from.value_or(self->xmin), to.value_or(self->xmax)); },
			     "from"_a = nullopt, "to"_a = nullopt)

			.def("scale_intensity",
			     &Sound_scaleIntensity,
			     "new_average_intensity"_a)

			.def("set_to_zero", // TODO Set part to zero
			     [] (Sound self, optional<double> from, optional<double> to, bool roundToNearestZeroCrossing) { Sound_setZero(self, from.value_or(self->xmin), to.value_or(self->xmax), roundToNearestZeroCrossing); },
				"from"_a = nullopt, "to"_a = nullopt, "round_to_nearest_zero_crossing"_a = true)

			// TODO Sound to Intensity, Formant, Harmonicity, ...


			// TODO Reading files, obviously
			// TODO Writing files
			;

	// TODO For some reason praat_David_init.cpp also still contains Sound functionality
}

} // namespace parselmouth
