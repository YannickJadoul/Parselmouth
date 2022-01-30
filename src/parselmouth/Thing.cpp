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

#include "Parselmouth.h"

#include "utils/praat/MelderUtils.h"

#include <praat/sys/praatP.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

CLASS_BINDING(Thing, structThing, detail::PraatHolder<structThing>)
BINDING_CONSTRUCTOR(Thing, "Thing")
BINDING_INIT(Thing) {
#if PY_MAJOR_VERSION >= 3
	def("__str__",
	    [](Thing self) { MelderInfoInterceptor info; Thing_info(self); return info.get(); });
#else
	def("__unicode__",
	    [](Thing self) { MelderInfoInterceptor info; Thing_info(self); return info.get(); });

	def("__str__",
	    [](Thing self) { MelderInfoInterceptor info; Thing_info(self); return info.get(); });
#endif

	def_property("name", &Thing_getName, [](Thing self, char32 *name) { if (name) praat_cleanUpName(name); Thing_setName(self, name); });

	def_property_readonly("class_name", &Thing_className);

	def_property_readonly("full_name", &Thing_messageName);

	def("info", [](Thing self) { MelderInfoInterceptor info; Thing_info(self); return info.get(); });
}

} // namespace parselmouth
