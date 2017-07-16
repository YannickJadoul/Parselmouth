/*
 * Copyright (C) 2016-2017  Yannick Jadoul
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

#ifndef INC_MELDER_INFO_INTERCEPTOR_H
#define INC_MELDER_INFO_INTERCEPTOR_H

#include "sys/melder.h"
#include "UndefPraatMacros.h"

class MelderInfoInterceptor
{
public:
	MelderInfoInterceptor() : m_string(), m_divertInfo(&m_string) {}
	std::u32string string() { return m_string.string; }
	std::string bytes() { return Melder_peek32to8(m_string.string); }
	std::string get() { return Melder_peek32to8(m_string.string); }

private:
	autoMelderString m_string;
	autoMelderDivertInfo m_divertInfo;
};

#endif // INC_MELDER_INFO_INTERCEPTOR_H
