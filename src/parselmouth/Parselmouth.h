#pragma once
#ifndef INC_PARSELMOUTH_PARSELMOUTH_H
#define INC_PARSELMOUTH_PARSELMOUTH_H

#include "Bindings.h"

#include "dwtools/MFCC.h"
#include "fon/Formant.h"
#include "fon/Harmonicity.h"
#include "fon/Intensity.h"
#include "fon/Pitch.h"
#include "fon/Sound.h"
#include "fon/Spectrogram.h"
#include "fon/Spectrum.h"
#include "sys/Thing.h"
#include "praat/UndefPraatMacros.h"

#include <pybind11/pybind11.h>


PYBIND11_DECLARE_HOLDER_TYPE(T, _Thing_auto<T>);


namespace parselmouth {

template <template<typename...> class Class, typename... Args>
struct PyBinding {
	using Creator = PyBinding<Class, Args...>;
	using Type = Class<Args...>;

	static Type create(pybind11::handle &scope);
};

template <typename Class, typename... Extra>
using ClassBinding = PyBinding<pybind11::class_, Class, Extra...>;

template <typename Enum>
using EnumBinding = PyBinding<pybind11::enum_, Enum>;

#define CLASS_BINDING(Type, ...) template<> struct Binding<Type> : ClassBinding<__VA_ARGS__> {};
#define ENUM_BINDING(Type, ...) template<> struct Binding<Type> : EnumBinding<__VA_ARGS__> {};
#define BINDING_CREATOR(Type, ...) template <> inline BindingType<Type> Binding<Type>::Creator::create(pybind11::handle &scope) { return { scope, __VA_ARGS__ }; }

#define PRAAT_CLASS_BINDING(Type, Base, ...) CLASS_BINDING(Type, struct##Type, auto##Type, struct##Base) BINDING_CREATOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING(Type, EnumType, ...) using Type = EnumType; ENUM_BINDING(Type, EnumType) BINDING_CREATOR(Type, #Type, __VA_ARGS__)



#define PRAAT_CLASSES            \
        Thing,                   \
        Sound,                   \
        Spectrum,                \
        Spectrogram,             \
        Pitch,                   \
        Intensity,               \
        Harmonicity,             \
        Formant,                 \
        MFCC

#define PRAAT_ENUMS              \
        WindowShape,             \
        AmplitudeScaling,        \
        SignalOutsideTimeDomain


CLASS_BINDING(Thing, structThing, autoThing)
BINDING_CREATOR(Thing, "Thing")

PRAAT_CLASS_BINDING(Sound, Thing)
PRAAT_CLASS_BINDING(Spectrum, Thing)
PRAAT_CLASS_BINDING(Spectrogram, Thing)
PRAAT_CLASS_BINDING(Pitch, Thing)
PRAAT_CLASS_BINDING(Intensity, Thing)
PRAAT_CLASS_BINDING(Harmonicity, Thing)
PRAAT_CLASS_BINDING(Formant, Thing)
PRAAT_CLASS_BINDING(MFCC, Thing)

PRAAT_ENUM_BINDING(WindowShape, kSound_windowShape)
PRAAT_ENUM_BINDING(AmplitudeScaling, kSounds_convolve_scaling)
PRAAT_ENUM_BINDING(SignalOutsideTimeDomain, kSounds_convolve_signalOutsideTimeDomain)


using PraatBindings = Bindings<PRAAT_CLASSES, PRAAT_ENUMS>;

void initThing(PraatBindings &bindings);
void initSound(PraatBindings &bindings);
void initSoundEnums(PraatBindings &bindings);

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
