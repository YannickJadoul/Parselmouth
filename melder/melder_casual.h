#ifndef _melder_casual_h_
#define _melder_casual_h_
/* melder_casual.h
 *
 * Copyright (C) 1992-2018,2020,2025 Paul Boersma
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

/*
	Function:
		Sends a message without user interference.
	Behaviour:
		Writes to stderr on Unix, otherwise to a special window.
*/

extern std::mutex theMelder_casual_mutex;

template <typename... Arg>
void Melder_casual (const Arg... arg) {
	std::lock_guard lock (theMelder_casual_mutex);
	(  MelderConsole::write (MelderArg { arg }. _arg, true), ...  );   // fold the comma over the parameter pack
	MelderConsole::write (U"\n", true);
}

void MelderCasual_memoryUse (integer message = 0);

/* End of file melder_casual.h */
#endif
