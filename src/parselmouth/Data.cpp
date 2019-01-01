/*
 * Copyright (C) 2017-2019  Yannick Jadoul
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

#include "utils/pybind11/ImplicitStringToEnumConversion.h"

#include <praat/sys/Data.h>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

enum class DataFileFormat {
	TEXT,
	SHORT_TEXT,
	BINARY
};

using FileFormat = DataFileFormat;
PRAAT_ENUM_BINDING(FileFormat) {
	value("TEXT", DataFileFormat::TEXT);
	value("SHORT_TEXT", DataFileFormat::SHORT_TEXT);
	value("BINARY", DataFileFormat::BINARY);

	make_implicitly_convertible_from_string(*this);
}

// Because we'd like to expose this class as Data and not as Daata
using structData = structDaata;
using Data = Daata;
using autoData = autoDaata;
using Data_Parent = Daata_Parent;

PRAAT_CLASS_BINDING(Data) {
	NESTED_BINDINGS(FileFormat)

	// TODO Cast to intermediate type? (i.e., Sound not known to parselmouth, then return Vector Python object instead of Data)
	// TODO Reading a Praat Collection
	def_static("read", // TODO Praat-format files not recognized because we cannot call praat_uvafon_init because INCLUDE_MANPAGES cannot be used because somehow that's not supposed to be called after only praatlib_init() instead of praat_init()
	           [](const std::string &filePath) { // TODO std::string to MelderFile functionality in separate function? Cfr. Sound.__init__
		           structMelderFile file = {};
		           Melder_relativePathToFile(Melder_peek8to32(filePath.c_str()), &file);
		           return Data_readFromFile(&file);
	           },
	           "file_path"_a);

	auto save = [](Data self, const std::string &filePath, DataFileFormat format) {
		structMelderFile file = {};
		Melder_relativePathToFile(Melder_peek8to32(filePath.c_str()), &file);
		switch (format) {
			case DataFileFormat::TEXT:
				Data_writeToTextFile(self, &file);
				break;
			case DataFileFormat::SHORT_TEXT:
				Data_writeToShortTextFile(self, &file);
				break;
			case DataFileFormat::BINARY:
				Data_writeToBinaryFile(self, &file);
				break;
		}
	};

	def("save",
	    save,
	    "file_path"_a, "format"_a = DataFileFormat::TEXT);

	def("save_as_text_file",
	    [save](Data self, const std::string &filePath) {
		    save(self, filePath, DataFileFormat::TEXT);
	    },
	    "file_path"_a);

	def("save_as_short_text_file",
	    [save](Data self, const std::string &filePath) {
		    save(self, filePath, DataFileFormat::SHORT_TEXT);
	    },
	    "file_path"_a);

	def("save_as_binary_file",
	    [save](Data self, const std::string &filePath) {
		    save(self, filePath, DataFileFormat::BINARY);
	    },
	    "file_path"_a);

	// TODO Write multiple objects as one collection?

	// TODO How about derived Python classes?
	def("copy",
	    &Data_copy<structData>);

	def("__copy__",
	    &Data_copy<structData>);

	def("__deepcopy__",
	    [](Data self, py::dict) { return Data_copy<structData>(self); },
		"memo"_a);

	// TODO Pickling?

	def("__eq__",
		&Data_equal,
		"other"_a.none(false), py::is_operator());

	def("__ne__",
	    [](Data self, Data other) { return !Data_equal(self, other); },
	    "other"_a.none(false), py::is_operator());
}

} // namespace parselmouth
