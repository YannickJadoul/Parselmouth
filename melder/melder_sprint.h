#ifndef _melder_sprint_h_
#define _melder_sprint_h_
/* melder_sprint.h
 *
 * Copyright (C) 1992-2018,2020,2022,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

template <typename... Arg>
void Melder_sprint (mutablestring32 buffer, int64 bufferSize, const Arg... arg) {
	const integer length = MelderArg__length (arg...);
	if (length >= bufferSize) {
		for (int64 i = 0; i < bufferSize; i ++)
			buffer [i] = U'?';
		if (bufferSize > 0)
			buffer [bufferSize - 1] = U'\0';
		return;
	}
	char32 *p = & buffer [0];
	auto addOneStringElement = [& p] (conststring32 string) {
		if (string) {
			char32 *newEndOfStringLocation = stp32cpy (p, string);
			p = newEndOfStringLocation;
		}
	};
	(// fold
		addOneStringElement (MelderArg { arg }. _arg)
				, ...
	);
}

/* End of file melder_sprint.h */
#endif
