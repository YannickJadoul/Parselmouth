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
#ifndef INC_PARSELMOUTH_SIGNATURECAST_H
#define INC_PARSELMOUTH_SIGNATURECAST_H

#include <utility>

namespace parselmouth {
namespace signature_cast_placeholder {

struct _;

} // namespace signature_cast_placeholder

namespace detail {

template <typename T>
struct ReplaceSignatureCastPlaceholderImpl {
	template <typename Arg>
	using Type = T;
};

template <>
struct ReplaceSignatureCastPlaceholderImpl<signature_cast_placeholder::_> {
	template <typename Arg>
	using Type = Arg;
};


template <typename CastArg, typename ActualArg>
using ReplaceSignatureCastPlaceholder = typename ReplaceSignatureCastPlaceholderImpl<CastArg>::template Type<ActualArg>;


template <typename Functor>
struct RemoveClass;

template <typename R, typename C, typename... A>
struct RemoveClass<R C::*(A...)> { using Type = R (A...); };

template <typename R, typename C, typename... A>
struct RemoveClass<R C::*(A...) const> { using Type = R (A...); };

template<typename Functor>
using RemoveClassT = typename RemoveClass<Functor>::Type;


template<typename Signature, typename Function>
struct CompleteSignatureImpl;

template<typename Return, typename... Args, typename R, typename... A>
struct CompleteSignatureImpl<Return (Args...), R (A...)> {
	using Type = ReplaceSignatureCastPlaceholder<Return, R> (ReplaceSignatureCastPlaceholder<Args, A>...);
};


template<typename Signature, typename Function>
struct CompleteSignature : CompleteSignatureImpl<Signature, RemoveClassT<decltype(&Function::operator())>> {};

template<typename Signature, typename R, typename... A>
struct CompleteSignature<Signature, R (&)(A...)> : CompleteSignatureImpl<Signature, R (A...)> {};

template<typename Signature, typename R, typename... A>
struct CompleteSignature<Signature, R (*)(A...)> : CompleteSignatureImpl<Signature, R (A...)> {};

template<typename Signature, typename R, typename C, typename... A>
struct CompleteSignature<Signature, R (C::*)(A...)> : CompleteSignatureImpl<Signature, R (C &, A...)> {};

template<typename Signature, typename R, typename C, typename... A>
struct CompleteSignature<Signature, R (C::*)(A...) const> : CompleteSignatureImpl<Signature, R (const C &, A...)> {};


template<typename Signature>
struct SignatureCastImpl;

template<typename Return, typename... Args>
struct SignatureCastImpl<Return (Args...)> {
	template<typename Function>
	static auto exec(Function &&f) { return [f = std::forward<Function>(f)](Args... args) -> Return { return f(std::forward<Args>(args)...); }; }

	template<typename R, typename... A>
	static auto exec(R (*f)(A...)) { return [f](Args... args) -> Return { return f(std::forward<Args>(args)...); }; }

	template<typename R, typename C, typename... A>
	static auto exec(R (C::*f)(A...)) { return [f](C &c, Args... args) -> Return { return c.*f(std::forward<Args>(args)...); }; }

	template<typename R, typename C, typename... A>
	static auto exec(R (C::*f)(A...) const) { return [f](const C &c, Args... args) -> Return { return c.*f(std::forward<Args>(args)...); };	}
};

} // namespace detail

template<typename Signature, typename Function>
auto signature_cast(Function &&f) {
	return detail::SignatureCastImpl<typename detail::CompleteSignature<Signature, Function>::Type>::exec(std::forward<Function>(f));
}

template<typename... Args, typename Function>
auto args_cast(Function &&f) {
	return signature_cast<signature_cast_placeholder::_ (Args...)>(std::forward<Function>(f));
}

} // namespace parselmouth

#endif // INC_PARSELMOUTH_SIGNATURECAST_H
