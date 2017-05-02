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

template <typename Class, typename... Extra>
struct ClassBinding {
	using Creator = ClassBinding<Class, Extra...>;
	using Type = pybind11::class_<Class, Extra...>;

	static Type create(pybind11::handle &scope);
};


#define CLASS_BINDING(Type, ...) template<> struct Binding<Type> : ClassBinding<__VA_ARGS__> {};
#define CLASS_BINDING_CREATOR(Type, ...) template <> inline BindingType<Type> Binding<Type>::Creator::create(pybind11::handle &scope) { return { scope, __VA_ARGS__ }; }


#define PRAAT_CLASS_BINDING(Type, Base, ...) CLASS_BINDING(Type, struct##Type, auto##Type, struct##Base) CLASS_BINDING_CREATOR(Type, #Type, __VA_ARGS__)



#define PRAAT_CLASSES \
	Thing,            \
    Sound,            \
    Spectrum,         \
    Spectrogram,      \
    Pitch,            \
    Intensity,        \
	Harmonicity,      \
    Formant,          \
    MFCC


CLASS_BINDING(Thing, structThing, autoThing)
CLASS_BINDING_CREATOR(Thing, "Thing")

PRAAT_CLASS_BINDING(Sound, Thing)
PRAAT_CLASS_BINDING(Spectrum, Thing)
PRAAT_CLASS_BINDING(Spectrogram, Thing)
PRAAT_CLASS_BINDING(Pitch, Thing)
PRAAT_CLASS_BINDING(Intensity, Thing)
PRAAT_CLASS_BINDING(Harmonicity, Thing)
PRAAT_CLASS_BINDING(Formant, Thing)
PRAAT_CLASS_BINDING(MFCC, Thing)


using PraatBindings = Bindings<PRAAT_CLASSES>;

void initThing(PraatBindings &bindings);
void initSound(PraatBindings &bindings);

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
