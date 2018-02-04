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
#ifndef INC_PARSELMOUTH_OPTIONAL_H
#define INC_PARSELMOUTH_OPTIONAL_H

#include <pybind11/stl.h>

namespace parselmouth {

#ifdef PYBIND11_HAS_OPTIONAL
	using std::optional;
	using std::nullopt;
	using std::make_optional;
#elif PYBIND11_HAS_EXP_OPTIONAL
	using std::experimental::optional;
	using std::experimental::nullopt;
	using std::experimental::make_optional;
#else
	#error Using incompatible compiler: compiler should provide either <optional> or <experimental/optional>
#endif

}

#endif // INC_PARSELMOUTH_OPTIONAL_H
