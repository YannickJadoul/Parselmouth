/*
 * Copyright (C) 2017-2018  Yannick Jadoul
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

#include <praat/fon/Vector.h>

enum class kFormant_unit;
enum class kPitch_unit;
enum class kSound_to_Spectrogram_windowShape;
enum class kSound_windowShape;
enum class kSounds_convolve_scaling;
enum class kSounds_convolve_signalOutsideTimeDomain;

PYBIND11_DECLARE_HOLDER_TYPE(T, _Thing_auto<T>);

namespace parselmouth {

template <typename Type>
class BindingType;

template <typename Type>
class Binding
{
public:
	explicit Binding(pybind11::handle &scope);
	~Binding();

	void init();

private:
	std::unique_ptr<BindingType<Type>> m_binding;
};

template <typename Class, typename... Extra>
using ClassBinding = pybind11::class_<Class, Extra...>;

template <typename Enum>
using EnumBinding = pybind11::enum_<Enum>;

#define BINDING(Kind, Type, ...) \
	template <> class BindingType<Type> : public Kind<__VA_ARGS__> { using Base = Kind<__VA_ARGS__>; public: explicit BindingType(pybind11::handle &); void init(); }; \
	template <> Binding<Type>::Binding(pybind11::handle &scope) : m_binding(std::make_unique<BindingType<Type>>(scope)) {} \
	template <> Binding<Type>::~Binding() = default; \
	template <> void Binding<Type>::init() { m_binding->init(); }

#define CLASS_BINDING(Type, ...) BINDING(ClassBinding, Type, __VA_ARGS__)
#define ENUM_BINDING(Type, ...) BINDING(EnumBinding, Type, __VA_ARGS__)

#define BINDING_CONSTRUCTOR(Type, ...) BindingType<Type>::BindingType(pybind11::handle &scope) : Base{scope, __VA_ARGS__} {}
#define BINDING_INIT(Type) void BindingType<Type>::init()
#define NO_BINDING_INIT(Type) BINDING_INIT(Type) {}

#define PRAAT_CLASS_BINDING(Type, ...) CLASS_BINDING(Type, struct##Type, auto##Type, Type##_Parent) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_CLASS_BINDING_BASE(Type, Base, ...) CLASS_BINDING(Type, struct##Type, auto##Type, struct##Base) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING(Type, ...) ENUM_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_STRUCT_BINDING(Name, Type, ...) CLASS_BINDING(Type, struct##Type) BINDING_CONSTRUCTOR(Type, #Name, __VA_ARGS__) BINDING_INIT(Type)

enum class Interpolation // TODO Remove/move to own/shared header
{
	NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
	LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
	CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
	SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
	SINC700 = Vector_VALUE_INTERPOLATION_SINC700
};

enum class SoundFileFormat;

using structData = structDaata;
using Data = Daata;
using autoData = autoDaata;
using Data_Parent = Daata_Parent;

using WindowShape = kSound_windowShape;
using AmplitudeScaling = kSounds_convolve_scaling;
using SignalOutsideTimeDomain = kSounds_convolve_signalOutsideTimeDomain;
using SpectralAnalysisWindowShape = kSound_to_Spectrogram_windowShape;
using FormantUnit = kFormant_unit;
using PitchUnit = kPitch_unit;

void initPraatModule(pybind11::module m);

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H
