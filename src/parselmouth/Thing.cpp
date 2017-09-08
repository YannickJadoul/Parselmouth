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

#include "praat/MelderUtils.h"

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

py::bytes encodeAsPreferredEncoding(const py::str &unicode) {
	static auto locale = py::module::import("locale");
	auto preferredencoding = locale.attr("getpreferredencoding")();
	return unicode.attr("encode")(preferredencoding, "replace");
}

void Binding<Thing>::init()
{
#if PY_MAJOR_VERSION >= 3
	def("__str__",
	    [](Thing self) { MelderInfoInterceptor info; self->v_info(); return info.string(); });

	def("__bytes__",
	    [](Thing self) { MelderInfoInterceptor info; self->v_info(); return py::bytes(info.bytes()); });
#else
	def("__unicode__",
	    [](Thing self) { MelderInfoInterceptor info; self->v_info(); return info.string(); });

	def("__str__",
	    [](Thing self) { MelderInfoInterceptor info; self->v_info(); return encodeAsPreferredEncoding(py::cast(info.string())); });
#endif
}

} // namespace parselmouth
