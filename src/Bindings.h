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
#ifndef INC_BINDINGS_H
#define INC_BINDINGS_H

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
#ifdef __cpp_fold_expressions
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
class Binding;


template <typename... Types>
class Bindings {
public:
#ifndef _MSC_VER
	static_assert(detail::all_unique_v<Types...>, "Multiple identical template parameter types are specified");
#endif

	template <typename... Args>
	Bindings(Args &&... args) : m_bindings{Binding<Types>(args...)...} {}

	void init() {
		int unused[] = { (std::get<Binding<Types>>(m_bindings).init(), 0)... };
		(void) unused;
	}

	template <typename T>
	Binding<T> &get() {
		static_assert(detail::any_of<std::is_same<T, Types>...>::value, "The specified type is not a member of these bindings");
		return std::get<Binding<T>>(m_bindings);
	}

private:
	std::tuple<Binding<Types>...> m_bindings;
};

} // namespace parselmouth

#endif // INC_BINDINGS_H
