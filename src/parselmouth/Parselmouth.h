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

#pragma once
#ifndef INC_PARSELMOUTH_PARSELMOUTH_H
#define INC_PARSELMOUTH_PARSELMOUTH_H

#include <pybind11/pybind11.h>

#include "Bindings.h"

// TODO Forward declarations?
#include "dwtools/MFCC.h"
#include "fon/Formant.h"
#include "fon/Harmonicity.h"
#include "fon/Intensity.h"
#include "fon/Pitch.h"
#include "fon/Sound.h"
#include "fon/Sound_and_Spectrogram.h"
#include "fon/Spectrogram.h"
#include "fon/Spectrum.h"
#include "sys/Thing.h"
#include "praat/UndefPraatMacros.h"


PYBIND11_DECLARE_HOLDER_TYPE(T, _Thing_auto<T>);

namespace parselmouth {

// TODO Move to its own header?
template <typename Type>
void make_implicitly_convertible_from_string(pybind11::enum_<Type> &enumType, bool ignoreCase=false)
{
	enumType.def("__init__",
	             [enumType, ignoreCase](Type &self, const std::string &value)
	             {
		             auto values = enumType.attr("__members__").template cast<pybind11::dict>();

		             auto strValue = pybind11::str(value);
		             if (values.contains(strValue)) {
			             new (&self) Type(values[strValue].template cast<Type>());
			             return;
		             }

		             if (ignoreCase) {
			             auto upperStrValue = strValue.attr("upper")();
			             for (auto &item : values) {
				             if (item.first.attr("upper")().attr("__eq__")(upperStrValue)) {
					             new (&self) Type(item.second.template cast<Type>());
					             return;
				             }
			             }
		             }

		             throw pybind11::value_error("\"" + value + "\" is not a valid value for enum type " + enumType.attr("__name__").template cast<std::string>());
	             });
	pybind11::implicitly_convertible<std::string, Type>();
};


template <typename Class, typename... Extra>
using ClassBinding = pybind11::class_<Class, Extra...>;

template <typename Enum>
using EnumBinding = pybind11::enum_<Enum>;


#define CLASS_BINDING(Type, ...) template<> class Binding<Type> : public ClassBinding<__VA_ARGS__> { using Base = ClassBinding<__VA_ARGS__>; public: Binding(pybind11::handle &scope); void init(); };
#define ENUM_BINDING(Type, ...) template<> class Binding<Type> : public EnumBinding<__VA_ARGS__> { using Base = EnumBinding<__VA_ARGS__>; public: Binding(pybind11::handle &scope); void init(); };
#define BINDING_CONSTRUCTOR(Type, ...) inline Binding<Type>::Binding(pybind11::handle &scope) : Base{scope, __VA_ARGS__} {}

#define PRAAT_CLASS_BINDING(Type, ...) CLASS_BINDING(Type, struct##Type, auto##Type, Type##_Parent) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_CLASS_BINDING_BASE(Type, Base, ...) CLASS_BINDING(Type, struct##Type, auto##Type, struct##Base) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING(Type, ...) ENUM_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING_ALIAS(Alias, Type, ...) using Alias = Type; PRAAT_ENUM_BINDING(Alias, __VA_ARGS__)

#define NO_BINDING_INIT(Type) inline void Binding<Type>::init() {}


enum class Interpolation;
enum class SoundFileFormat;

using structData = structDaata;
using Data = Daata;
using autoData = autoDaata;
using Data_Parent = Daata_Parent;


#define PRAAT_CLASSES                \
        Thing,                       \
        Data,                        \
        Matrix,                      \
        Vector,                      \
        Sound,                       \
        Spectrum,                    \
        Spectrogram,                 \
        Pitch,                       \
        Intensity,                   \
        Harmonicity,                 \
        Formant,                     \
        MFCC

#define PRAAT_ENUMS                  \
        Interpolation,               \
        WindowShape,                 \
        AmplitudeScaling,            \
        SignalOutsideTimeDomain,     \
        SoundFileFormat,             \
        SpectralAnalysisWindowShape


CLASS_BINDING(Thing, structThing, autoThing)
BINDING_CONSTRUCTOR(Thing, "Thing")

PRAAT_CLASS_BINDING(Data)
PRAAT_CLASS_BINDING_BASE(Matrix, Data, pybind11::buffer_protocol())
PRAAT_CLASS_BINDING(Vector)
PRAAT_CLASS_BINDING(Sound)
PRAAT_CLASS_BINDING(Spectrum)
PRAAT_CLASS_BINDING(Spectrogram)
PRAAT_CLASS_BINDING_BASE(Pitch, Data) // TODO Expose bindings for Sampled
PRAAT_CLASS_BINDING(Intensity)
PRAAT_CLASS_BINDING(Harmonicity)
PRAAT_CLASS_BINDING_BASE(Formant, Data) // TODO Expose bindings for Sampled
PRAAT_CLASS_BINDING_BASE(MFCC, Data) // TODO Expose bindings for CC & Sampled

PRAAT_ENUM_BINDING(Interpolation)
PRAAT_ENUM_BINDING_ALIAS(WindowShape, kSound_windowShape)
PRAAT_ENUM_BINDING_ALIAS(AmplitudeScaling, kSounds_convolve_scaling)
PRAAT_ENUM_BINDING_ALIAS(SignalOutsideTimeDomain, kSounds_convolve_signalOutsideTimeDomain)
PRAAT_ENUM_BINDING(SoundFileFormat)
PRAAT_ENUM_BINDING_ALIAS(SpectralAnalysisWindowShape, kSound_to_Spectrogram_windowShape)

NO_BINDING_INIT(Formant)
NO_BINDING_INIT(MFCC)


using PraatBindings = Bindings<PRAAT_ENUMS, PRAAT_CLASSES>;

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
