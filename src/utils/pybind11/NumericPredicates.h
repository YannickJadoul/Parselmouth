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
#ifndef INC_PARSELMOUTH_NUMERICPREDICATES_H
#define INC_PARSELMOUTH_NUMERICPREDICATES_H

#include "Predicate.h"

namespace parselmouth {

class PositiveImpl {
public:
	template <typename T>
	static bool check(const T &value) { return value > 0; }

	static constexpr auto &name() { return "Positive"; }
};

template <typename T>
using Positive = Predicate<T, PositiveImpl>;


class NonNegativeImpl {
public:
	template <typename T>
	static bool check(const T &value) { return value >= 0; }

	static constexpr auto &name() { return "NonNegative"; }
};

template <typename T>
using NonNegative = Predicate<T, NonNegativeImpl>;

} // namespace parselmouth

#endif // INC_PARSELMOUTH_NUMERICPREDICATES_H
