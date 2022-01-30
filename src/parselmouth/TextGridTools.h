/*
 * Copyright (C) 2019-2022  Yannick Jadoul
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
#ifndef INC_PARSELMOUTH_TEXTGRIDTOOLS_H
#define INC_PARSELMOUTH_TEXTGRIDTOOLS_H

#include <pybind11/pybind11.h>

#include <praat/sys/praat.h>

Thing_declare(TextGrid);

namespace parselmouth {

inline bool isTgtTextGrid(pybind11::handle o) {
	try {
		return pybind11::isinstance(o, pybind11::module::import("tgt").attr("TextGrid"));
	}
	catch (pybind11::error_already_set &e) {
		return false;
	}
};

class TgtTextGrid : public pybind11::object {
public:
	PYBIND11_OBJECT_DEFAULT(TgtTextGrid, pybind11::object, isTgtTextGrid)
};

TgtTextGrid toTgtTextGrid(TextGrid textGrid, bool includeEmptyIntervals = false);
autoTextGrid fromTgtTextGrid(TgtTextGrid textGrid);

} // namespace parselmouth

template <> struct pybind11::detail::handle_type_name<parselmouth::TgtTextGrid> { static constexpr auto name = _("tgt.core.TextGrid"); };

#endif // INC_PARSELMOUTH_TEXTGRIDTOOLS_H
