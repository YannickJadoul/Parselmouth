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

// TODO Move to its own header?
template <typename Type>
void make_implicitly_convertible_from_string(pybind11::enum_<Type> &enumType, bool ignoreCase=false)
{
	enumType.def("__init__",
	             [enumType, ignoreCase] (Type &self, const std::string &value)
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
#define PRAAT_ENUM_BINDING(Type, ...) ENUM_BINDING(Type, Type) BINDING_CREATOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING_ALIAS(Alias, Type, ...) using Alias = Type; PRAAT_ENUM_BINDING(Alias, __VA_ARGS__)


enum class Interpolation;


#define PRAAT_CLASSES            \
        Thing,                   \
        Vector,                  \
        Sound,                   \
        Spectrum,                \
        Spectrogram,             \
        Pitch,                   \
        Intensity,               \
        Harmonicity,             \
        Formant,                 \
        MFCC

#define PRAAT_ENUMS              \
        Interpolation,           \
        WindowShape,             \
        AmplitudeScaling,        \
        SignalOutsideTimeDomain


CLASS_BINDING(Thing, structThing, autoThing)
BINDING_CREATOR(Thing, "Thing")

PRAAT_CLASS_BINDING(Vector, Thing)
PRAAT_CLASS_BINDING(Sound, Vector)
PRAAT_CLASS_BINDING(Spectrum, Thing)
PRAAT_CLASS_BINDING(Spectrogram, Thing)
PRAAT_CLASS_BINDING(Pitch, Thing)
PRAAT_CLASS_BINDING(Intensity, Thing)
PRAAT_CLASS_BINDING(Harmonicity, Thing)
PRAAT_CLASS_BINDING(Formant, Thing)
PRAAT_CLASS_BINDING(MFCC, Thing)

PRAAT_ENUM_BINDING(Interpolation)
PRAAT_ENUM_BINDING_ALIAS(WindowShape, kSound_windowShape)
PRAAT_ENUM_BINDING_ALIAS(AmplitudeScaling, kSounds_convolve_scaling)
PRAAT_ENUM_BINDING_ALIAS(SignalOutsideTimeDomain, kSounds_convolve_signalOutsideTimeDomain)


using PraatBindings = Bindings<PRAAT_CLASSES, PRAAT_ENUMS>;

void initThing(PraatBindings &bindings);
void initVector(PraatBindings &bindings);
void initSound(PraatBindings &bindings);
void initSoundEnums(PraatBindings &bindings);

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
