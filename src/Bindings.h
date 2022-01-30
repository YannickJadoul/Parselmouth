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
#ifndef INC_PARSELMOUTH_BINDINGS_H
#define INC_PARSELMOUTH_BINDINGS_H

#include <pybind11/pybind11.h>

#include <tuple>
#include <type_traits>
#include <utility>

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

	pybind11::handle get();

private:
	std::unique_ptr<BindingType<Type>> m_binding;
};

template <typename>
class ModuleWrapper : public pybind11::module {
public:
	template <typename... Args>
	explicit ModuleWrapper(pybind11::handle &scope, Args &&... args) : pybind11::module(scope.cast<pybind11::module>().def_submodule(std::forward<Args>(args)...)) {}
};

template <typename... Types>
class Bindings;

template <>
class Bindings<> {
public:
	template <typename... Args>
	explicit Bindings(Args &&...) {}

	void init() {}
};

template <typename First, typename... Rest>
class Bindings<First, Rest...> {
public:
#ifndef _MSC_VER
	static_assert(!(std::is_same_v<First, Rest> || ...), "Multiple identical template parameter types are specified");
#endif

	template <typename... Args>
	explicit Bindings(Args &&... args) : m_first{args...}, m_rest{std::forward<Args>(args)...} {}

	void init() {
		m_first.init();
		m_rest.init();
	}

	template <typename T>
	Binding<T> &get() {
		static_assert(std::is_same_v<T, First> || (std::is_same_v<T, Rest> || ...), "The specified type is not a member of these bindings");
		if constexpr (std::is_same_v<T, First>)
			return m_first;
		else
			return m_rest.template get<T>();
	}

private:
	Binding<First> m_first;
	Bindings<Rest...> m_rest;
};

#define BINDING(Type, Kind, ...) \
	template <> class BindingType<Type> : public Kind<__VA_ARGS__> { using Base = Kind<__VA_ARGS__>; public: explicit BindingType(pybind11::handle &); void init(); }; \
	template <> Binding<Type>::Binding(pybind11::handle &scope) : m_binding(std::make_unique<BindingType<Type>>(scope)) {} \
	template <> Binding<Type>::~Binding() {} \
	template <> void Binding<Type>::init() { m_binding->init(); } \
	template <> pybind11::handle Binding<Type>::get() { return *m_binding; }

#define CLASS_BINDING(Type, ...) BINDING(Type, pybind11::class_, __VA_ARGS__)
#define ENUM_BINDING(Type, Enum) BINDING(Type, pybind11::enum_, Enum)
#define MODULE_BINDING(Type) BINDING(Type, ModuleWrapper, Type)
#define EXCEPTION_BINDING(Type, Exception) BINDING(Type, pybind11::exception, Exception)

#define BINDING_CONSTRUCTOR(Type, ...) BindingType<Type>::BindingType(pybind11::handle &scope) : Base{scope, __VA_ARGS__} {}
#define BINDING_INIT(Type) void BindingType<Type>::init()
#define NO_BINDING_INIT(Type) BINDING_INIT(Type) {}
#define NESTED_BINDINGS(...) { Bindings<__VA_ARGS__> nestedBindings(*this); nestedBindings.init(); }

} // namespace parselmouth

#endif // INC_PARSELMOUTH_BINDINGS_H
