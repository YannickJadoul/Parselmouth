#ifndef _melder_time_h_
#define _melder_time_h_
/* melder_time.h
 *
 * Copyright (C) 1992-2016,2018,2020,2021,2025 Paul Boersma
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

double Melder_stopwatch ();

void Melder_sleep (double duration);

double Melder_clock ();   // typically the number of seconds since system start-up, with microsecond precision

class MelderStopwatch {
	double _lastTime { 0.0 };
public:
	MelderStopwatch () {
		our _lastTime = Melder_clock ();
	}
	double operator() () {
		const double now = Melder_clock ();
		const double timeElapsed = now - our _lastTime;
		our _lastTime = now;
		return timeElapsed;
	}
};

autostring32 date_STR ();
autostring32 date_utc_STR ();

autostring32 date_iso_STR ();
autostring32 date_utc_iso_STR ();

autoVEC date_VEC ();
autoVEC date_utc_VEC ();

/* End of file melder_time.h */
#endif
