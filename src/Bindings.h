#pragma once
#ifndef INC_BINDINGS_H
#define INC_BINDINGS_H

#include <type_traits>
#include <utility>

namespace parselmouth {
namespace detail {

// Taken from from pybind11's details
template <bool B> using bool_constant = std::integral_constant<bool, B>;
template <typename T> struct negation : bool_constant<!T::value> {};

template <bool...> struct bools {};
template <class... Ts> using all_of = std::is_same<bools<Ts::value..., true>, bools<true, Ts::value...>>;
template <class... Ts> using any_of = negation<all_of<negation<Ts>...>>;
template <class... Ts> using none_of = negation<any_of<Ts...>>;

} // namespace detail



template <typename Type>
struct Binding;

template <typename T>
using BindingType = typename Binding<T>::Type;


namespace detail {

template <typename... Types>
class BindingsImpl;

template <typename First, typename... Rest>
class BindingsImpl<First, Rest...> {
public:
	static constexpr bool all_unique = none_of<std::is_same<First, Rest>...>::value && BindingsImpl<Rest...>::all_unique;

	template <typename... Args>
	BindingsImpl(Args &&... args) : m_first(Binding<First>::create(args...)), m_rest(std::forward<Args>(args)...) {}

	template <typename T, std::enable_if_t<std::is_same<T, First>::value>* = nullptr>
	BindingType<T> &get() {
		return m_first;
	}

	template <typename T, std::enable_if_t<!std::is_same<T, First>::value>* = nullptr>
	BindingType<T> &get() {
		return m_rest.template get<T>();
	}

private:
	BindingType<First> m_first;
	BindingsImpl<Rest...> m_rest;
};

template <>
class BindingsImpl<> {
public:
	static constexpr bool all_unique = true;

	template <typename... Args>
	BindingsImpl(Args &&...) {}
};

} // namespace detail


template <typename... Types>
class Bindings {
public:
	using Impl = detail::BindingsImpl<Types...>;

	static_assert(Impl::all_unique, "Multiple identical template parameter types are specified");

	template <typename... Args>
	Bindings(Args &&... args) : m_impl(std::forward<Args>(args)...) {}

	template <typename T>
	BindingType<T> &get() {
		static_assert(detail::any_of<std::is_same<T, Types>...>::value, "The specified type is not a member of these bindings");

		return m_impl.template get<T>();
	}

private:
	Impl m_impl;
};

} // namespace parselmouth

#endif // INC_BINDINGS_H
