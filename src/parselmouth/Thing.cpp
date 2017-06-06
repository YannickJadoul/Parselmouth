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

#include "Parselmouth.h"

#include "praat/MelderInfoInterceptor.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<Thing>::init()
{
	def("__" PYBIND11_STRING_NAME "__", // Python 2 vs. Python 3 - __unicode__ vs. __str__
	    [](Thing self) { MelderInfoInterceptor info; self->v_info(); return info.string(); });

	def("__" PYBIND11_BYTES_NAME "__", // Python 2 vs. Python 3 - __str__ vs. __bytes__
	    [](Thing self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.bytes()); });
}

} // namespace parselmouth
