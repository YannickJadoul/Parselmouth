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

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

void Binding<Data>::init()
{
	// TODO Cast to intermediate type (i.e., Sound not known to parselmouth, then return Vector Python object instead of Data)
	//def_static("read", // TODO Praat-format files not recognized because we cannot call praat_uvafon_init because INCLUDE_MANPAGES cannot be used because somehow that's not supposed to be called after only praatlib_init() instead of praat_init()
	//           [](const std::string &filePath) { // TODO std::string to MelderFile functionality in separate function? Cfr. Sound.__init__
	//	           structMelderFile file = {};
	//	           Melder_relativePathToFile(Melder_peek8to32(filePath.c_str()), &file);
	//	           return Data_readFromFile(&file);
	//           },
	//           "file_path"_a);

	def("copy",
	    &Data_copy<structData>);

	def("__copy__",
	    &Data_copy<structData>);

	def("__deepcopy__",
	    &Data_copy<structData>);

	// TODO Pickling?

	def("__eq__",
		&Data_equal,
		"other"_a.none(false), py::is_operator());

	def("__ne__",
	    [](Data self, Data other) { return !Data_equal(self, other); },
	    "other"_a.none(false), py::is_operator());
}

} // namespace parselmouth
