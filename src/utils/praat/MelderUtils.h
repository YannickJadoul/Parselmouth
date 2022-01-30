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
#ifndef INC_PARSELMOUTH_MELDERUTILS_H
#define INC_PARSELMOUTH_MELDERUTILS_H

#include <praat/melder/melder.h>
#include "UndefPraatMacros.h"

#include <string>

namespace parselmouth {

class MelderInfoInterceptor
{
public:
	MelderInfoInterceptor() : m_string(), m_divertInfo(&m_string) {}

	MelderInfoInterceptor(const MelderInfoInterceptor &) = delete;
	MelderInfoInterceptor &operator=(const MelderInfoInterceptor &) = delete;
	MelderInfoInterceptor(MelderInfoInterceptor &&) = delete;
	MelderInfoInterceptor &operator=(MelderInfoInterceptor &&) = delete;

	std::u32string get() { return m_string.string == nullptr ? U"" : m_string.string; }

private:
	autoMelderString m_string;
	autoMelderDivertInfo m_divertInfo;
};

inline structMelderFile pathToMelderFile(const std::u32string &filePath) { // TODO type_caster structMelderFile?
	structMelderFile file = {};
	Melder_relativePathToFile(filePath.c_str(), &file);
	return file;
}

}

#endif // INC_PARSELMOUTH_MELDERUTILS_H
