#pragma once
#ifndef INC_BINDINGS_H
#define INC_BINDINGS_H

#include <tuple>
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
	static_assert(detail::all_unique_v<Types...>, "Multiple identical template parameter types are specified");

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
