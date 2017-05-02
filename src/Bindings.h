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


template <typename Type, typename Tuple>
struct index_of;

template <typename Type, typename... Others>
struct index_of<Type, std::tuple<Type, Others...>> {
	static constexpr unsigned int value = 0;
};

template <typename Type, typename Other, typename... Others>
struct index_of<Type, std::tuple<Other, Others...>> {
	static constexpr auto value = index_of<Type, std::tuple<Others...>>::value + 1;
};

template <typename Type, typename Tuple>
constexpr auto index_of_v = index_of<Type, Tuple>::value;

} // namespace detail


template <typename Type>
struct Binding;

template <typename T>
using BindingType = typename Binding<T>::Type;


template <typename... Types>
class Bindings {
public:
	static_assert(detail::all_unique_v<Types...>, "Multiple identical template parameter types are specified");

	template <typename... Args>
	Bindings(Args &&... args) : m_bindings{Binding<Types>::create(args...)...} {}

	template <typename T>
	BindingType<T> &get() {
		static_assert(detail::any_of<std::is_same<T, Types>...>::value, "The specified type is not a member of these bindings");
		return std::get<detail::index_of_v<T, std::tuple<Types...>>>(m_bindings);
	}

private:
	std::tuple<BindingType<Types>...> m_bindings;
};

} // namespace parselmouth

#endif // INC_BINDINGS_H
