#ifndef _melder_cat_h_
#define _melder_cat_h_
/* melder_cat.h
 *
 * Copyright (C) 1992-2018,2020,2024,2025 Paul Boersma
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

namespace MelderCat {
	constexpr int _k_NUMBER_OF_BUFFERS = 33;
	extern MelderString _buffers [_k_NUMBER_OF_BUFFERS];
	extern int _bufferNumber;
}

template <typename... Arg>
conststring32 Melder_cat (const Arg... arg) {
	if (++ MelderCat::_bufferNumber == MelderCat::_k_NUMBER_OF_BUFFERS)
		MelderCat::_bufferNumber = 0;
	MelderString_copy (& MelderCat::_buffers [MelderCat::_bufferNumber], arg...);
	return MelderCat::_buffers [MelderCat::_bufferNumber].string;
}

/* End of file melder_cat.h */
#endif
