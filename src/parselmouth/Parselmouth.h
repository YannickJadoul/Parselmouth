/*
 * Copyright (C) 2017-2019  Yannick Jadoul
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

#pragma once
#ifndef INC_PARSELMOUTH_PARSELMOUTH_H
#define INC_PARSELMOUTH_PARSELMOUTH_H

#include <pybind11/pybind11.h>

#include "Bindings.h"

#include <praat/sys/Thing.h>

enum class kFormant_unit;
enum class kPitch_unit;
enum class kSound_to_Spectrogram_windowShape;
enum class kSound_windowShape;
enum class kSounds_convolve_scaling;
enum class kSounds_convolve_signalOutsideTimeDomain;

PYBIND11_DECLARE_HOLDER_TYPE(T, _Thing_auto<T>);

namespace parselmouth {

enum class Interpolation;
enum class SoundFileFormat;

using WindowShape = kSound_windowShape;
using AmplitudeScaling = kSounds_convolve_scaling;
using SignalOutsideTimeDomain = kSounds_convolve_signalOutsideTimeDomain;
using SpectralAnalysisWindowShape = kSound_to_Spectrogram_windowShape;
using FormantUnit = kFormant_unit;
using PitchUnit = kPitch_unit;

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
