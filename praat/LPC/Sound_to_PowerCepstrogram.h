#ifndef _Sound_to_PowerCepstrogram_h_
#define _Sound_to_PowerCepstrogram_h_
/* Sound_to_PowerCepstrogram.h
 *
 * Copyright (C) 2012-2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PowerCepstrum.h"
#include "PowerCepstrogram.h"
#include "Sound.h"

autoPowerCepstrogram Sound_to_PowerCepstrogram (constSound me, double pitchFloor, double dt, double maximumFrequency,
	double preEmphasisFrequency);

autoPowerCepstrogram Sound_to_PowerCepstrogram_hillenbrand (constSound me, double analysisWidth, double dt);

#endif /* _Sound_to_PowerCepstrogram_h_ */
