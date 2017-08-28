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

#ifndef INC_PARSELMOUTH_IMPLICIT_STRING_TO_ENUM_CONVERSION_H
#define INC_PARSELMOUTH_IMPLICIT_STRING_TO_ENUM_CONVERSION_H

#include <pybind11/pybind11.h>

namespace parselmouth {

template <typename Type>
void make_implicitly_convertible_from_string(pybind11::enum_<Type> &enumType, bool ignoreCase=false)
{
	enumType.def(pybind11::init([enumType, ignoreCase](const std::string &value)
	                            {
		                            auto values = enumType.attr("__members__").template cast<pybind11::dict>();

		                            auto strValue = pybind11::str(value);
		                            if (values.contains(strValue)) {
			                            return Type(values[strValue].template cast<Type>());
		                            }

		                            if (ignoreCase) {
			                            auto upperStrValue = strValue.attr("upper")();
			                            for (auto &item : values) {
				                            if (item.first.attr("upper")().attr("__eq__")(upperStrValue).template cast<bool>()) {
					                            return Type(item.second.template cast<Type>());
				                            }
			                            }
		                            }

		                            throw pybind11::value_error("\"" + value + "\" is not a valid value for enum type " + enumType.attr("__name__").template cast<std::string>());
	                            }));

	pybind11::implicitly_convertible<std::string, Type>();
};

}

#endif // INC_PARSELMOUTH_IMPLICIT_STRING_TO_ENUM_CONVERSION_H
