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
enum class kVector_valueInterpolation;

namespace parselmouth::detail {

// Because Praat's autoSomeThing doesn't have a constructor from the raw pointer anymore (not sure why?), but only the
// non-static member function "adoptFromAmbiguousOwner", we create a zero-overhead wrapper to be used as holder.
// This seems cleaner/safer/less risky/more future-proof than adding this as extra constructor in the Praat class.
template<typename T>
class PraatHolder {
public:
	PraatHolder(autoSomeThing<T> &&thing) : m_thing(std::move(thing)) {}
	PraatHolder(T *thing) : m_thing() { m_thing.adoptFromAmbiguousOwner(thing); }
	operator autoSomeThing<T> &&() { return std::move(m_thing); }

	T *get() const { return m_thing.get(); }

private:
	autoSomeThing<T> m_thing;
};

}

PYBIND11_DECLARE_HOLDER_TYPE(T, parselmouth::detail::PraatHolder<T>);

namespace pybind11::detail {

template <typename T>
class type_caster<autoSomeThing<T>> : public type_caster<parselmouth::detail::PraatHolder<T>> {};

}

#define PRAAT_CLASS_BINDING(Type, ...) CLASS_BINDING(Type, struct##Type, parselmouth::detail::PraatHolder<struct##Type>, Type##_Parent) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_CLASS_BINDING_BASE(Type, Base, ...) CLASS_BINDING(Type, struct##Type, parselmouth::detail::PraatHolder<struct##Type>, struct##Base) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_ENUM_BINDING(Type, ...) ENUM_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_STRUCT_BINDING(Name, Type, ...) CLASS_BINDING(Type, struct##Type) BINDING_CONSTRUCTOR(Type, #Name, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_MODULE_BINDING(Name, Type, ...) MODULE_BINDING(Type) BINDING_CONSTRUCTOR(Type, #Name, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_EXCEPTION_BINDING(Type, ...) EXCEPTION_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)

namespace parselmouth {

enum class SoundFileFormat;

using ValueInterpolation = kVector_valueInterpolation;
using WindowShape = kSound_windowShape;
using AmplitudeScaling = kSounds_convolve_scaling;
using SignalOutsideTimeDomain = kSounds_convolve_signalOutsideTimeDomain;
using SpectralAnalysisWindowShape = kSound_to_Spectrogram_windowShape;
using FormantUnit = kFormant_unit;
using PitchUnit = kPitch_unit;

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
