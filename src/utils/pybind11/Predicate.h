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
#ifndef INC_PARSELMOUTH_PREDICATE_H
#define INC_PARSELMOUTH_PREDICATE_H

#include <pybind11/pybind11.h>

#include "utils/SignatureCast.h"

#include <utility>

namespace parselmouth {

template <typename T, typename Impl>
class Predicate {
private:
	using This = Predicate<T, Impl>;

public:
	Predicate() : m_wrapped() {}

	template <typename... Args>
	Predicate(Args &&... args) : m_wrapped(std::forward<Args>(args)...) {
		if (!Impl::check(m_wrapped))
			throw std::domain_error(std::string(Impl::name()) + " constructed with invalid value");
	}

	Predicate(const This &other) = default;
	Predicate(This &&other) = default;

	Predicate &operator=(const This &other) = default;
	Predicate &operator=(This &&other) = default;

	operator T &() & { return m_wrapped; }
	operator const T &() const & { return m_wrapped; }
	operator T &&() && { return std::move(m_wrapped); }

private:
	T m_wrapped;
};

template <typename Impl>
class Predicate<signature_cast_placeholder::_, Impl>;

namespace detail {

template <typename Impl>
struct ReplaceSignatureCastPlaceholderImpl<Predicate<signature_cast_placeholder::_, Impl>> {
	template <typename Arg>
	using Type = Predicate<Arg, Impl>;
};

} // namespace detail

} // namespace parselmouth

namespace pybind11::detail {

template <typename T, typename Impl>
class type_caster<parselmouth::Predicate<T, Impl>> {
public:
	using PredicateT = parselmouth::Predicate<T, Impl>;
	using TCaster = make_caster<T>;

	bool load(handle src, bool convert) {
		TCaster subCaster;

		if (!subCaster.load(src, convert))
			return false;

		auto subValue = cast_op<T>(subCaster);
		if (!Impl::check(subValue))
			return false;

		value = std::move(subValue);

		return true;
	}

	static handle cast(const PredicateT &src, return_value_policy policy, handle parent) {
		return TCaster::cast(src, policy, parent);
	}

	PYBIND11_TYPE_CASTER(PredicateT, _(Impl::name()) + _("[") + TCaster::name + _("]")); // TODO Python implementation of what Positive[T]/NonNegative[T]/... is, to get typecheckers happy?
};

} // namespace pybind11::detail

#endif // INC_PARSELMOUTH_PREDICATE_H
