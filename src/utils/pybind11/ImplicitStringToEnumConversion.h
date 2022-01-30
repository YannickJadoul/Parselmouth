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
#ifndef INC_PARSELMOUTH_IMPLICITSTRINGTOENUMCONVERSION_H
#define INC_PARSELMOUTH_IMPLICITSTRINGTOENUMCONVERSION_H

#include <pybind11/pybind11.h>

namespace parselmouth {

template <typename Type>
void make_implicitly_convertible_from_string(pybind11::enum_<Type> &enumType)
{
	enumType.def(pybind11::init([enumType](pybind11::str value)
	                            {
		                            auto values = pybind11::dict(enumType.attr("__members__"));
		                            if (values.contains(value)) {
			                            return pybind11::cast<Type>(values[value]);
		                            }

		                            throw pybind11::value_error("\"" + pybind11::cast<std::string>(value) + "\" is not a valid value for enum type " + pybind11::cast<std::string>(enumType.attr("__name__")));
	                            }));

	pybind11::implicitly_convertible<std::string, Type>();
};

}

#endif // INC_PARSELMOUTH_IMPLICITSTRINGTOENUMCONVERSION_H
