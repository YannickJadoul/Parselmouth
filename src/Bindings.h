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
#ifndef INC_PARSELMOUTH_BINDINGS_H
#define INC_PARSELMOUTH_BINDINGS_H

#include <pybind11/pybind11.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace parselmouth {
namespace detail {

// Taken from from pybind11's details
// Backports of std::bool_constant and std::negation to accomodate older compilers
template <bool B> using bool_constant = std::integral_constant<bool, B>;
template <typename T> struct negation : bool_constant<!T::value> {};

// Compile-time all/any/none of that check the boolean value of all template types
#if defined(__cpp_fold_expressions) && !(defined(_MSC_VER) && (_MSC_VER < 1916))
template <class... Ts> using all_of = bool_constant<(Ts::value && ...)>;
template <class... Ts> using any_of = bool_constant<(Ts::value || ...)>;
#elif !defined(_MSC_VER)
template <bool...> struct bools {};
template <class... Ts> using all_of = std::is_same<bools<Ts::value..., true>, bools<true, Ts::value...>>;
template <class... Ts> using any_of = negation<all_of<negation<Ts>...>>;
#else
// MSVC has trouble with the above, but supports std::conjunction, which we can use instead (albeit
// at a slight loss of compilation efficiency).
template <class... Ts> using all_of = std::conjunction<Ts...>;
template <class... Ts> using any_of = std::disjunction<Ts...>;
#endif
template <class... Ts> using none_of = negation<any_of<Ts...>>;


template <typename... Types>
struct all_unique;

template <>
struct all_unique<> : std::true_type {};

template <typename Type, typename... Others>
struct all_unique<Type, Others...> : bool_constant<none_of<std::is_same<Type, Others>...>::value && all_unique<Others...>::value> {};

template <typename... Types>
constexpr auto all_unique_v = all_unique<Types...>::value;

} // namespace detail

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

template <typename Type, typename... Rest>
class Bindings<Type, Rest...> {
public:
#ifndef _MSC_VER
	static_assert(detail::none_of<std::is_same<Type, Rest>...>::value, "Multiple identical template parameter types are specified");
#endif

	template <typename... Args>
	explicit Bindings(Args &&... args) : m_binding{args...}, m_rest{std::forward<Args>(args)...} {}

	void init() {
		m_binding.init();
		m_rest.init();
	}

	template <typename T>
	Binding<T> &get() {
		static_assert(std::is_same<T, Type>::value || detail::any_of<std::is_same<T, Rest>...>::value, "The specified type is not a member of these bindings");
		return getImpl<T>();
	}

	template <typename T>
	Binding<T> &getImpl() {
		return getImpl<T>(std::is_same<T, Type>());
	}

	template <typename T>
	Binding<Type> &getImpl(std::true_type) {
		return m_binding;
	}

	template <typename T>
	Binding<T> &getImpl(std::false_type) {
		return m_rest.template getImpl<T>();
	}

private:
	Binding<Type> m_binding;
	Bindings<Rest...> m_rest;
};

#define BINDING(Type, Kind, ...) \
	template <> class BindingType<Type> : public Kind<__VA_ARGS__> { using Base = Kind<__VA_ARGS__>; public: explicit BindingType(pybind11::handle &); void init(); }; \
	template <> Binding<Type>::Binding(pybind11::handle &scope) : m_binding(std::make_unique<BindingType<Type>>(scope)) {} \
	template <> Binding<Type>::~Binding() {} \
	template <> void Binding<Type>::init() { m_binding->init(); }

#define CLASS_BINDING(Type, ...) BINDING(Type, pybind11::class_, __VA_ARGS__)
#define ENUM_BINDING(Type, Enum) BINDING(Type, pybind11::enum_, Enum)
#define MODULE_BINDING(Type) BINDING(Type, ModuleWrapper, Type)
#define EXCEPTION_BINDING(Type, Exception) BINDING(Type, pybind11::exception, Exception)

#define BINDING_CONSTRUCTOR(Type, ...) BindingType<Type>::BindingType(pybind11::handle &scope) : Base{scope, __VA_ARGS__} {}
#define BINDING_INIT(Type) void BindingType<Type>::init()
#define NO_BINDING_INIT(Type) BINDING_INIT(Type) {}
#define NESTED_BINDINGS(...) { Bindings<__VA_ARGS__> nestedBindings(*this); nestedBindings.init(); }

#define PRAAT_CLASS_BINDING(Type, ...) CLASS_BINDING(Type, struct##Type, auto##Type, Type##_Parent) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_CLASS_BINDING_BASE(Type, Base, ...) CLASS_BINDING(Type, struct##Type, auto##Type, struct##Base) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING(Type, ...) ENUM_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_STRUCT_BINDING(Name, Type, ...) CLASS_BINDING(Type, struct##Type) BINDING_CONSTRUCTOR(Type, #Name, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_MODULE_BINDING(Name, Type, ...) MODULE_BINDING(Type) BINDING_CONSTRUCTOR(Type, #Name, __VA_ARGS__) BINDING_INIT(Type)
#define PRAAT_EXCEPTION_BINDING(Type, ...) EXCEPTION_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__) BINDING_INIT(Type)

} // namespace parselmouth

#endif // INC_PARSELMOUTH_BINDINGS_H
