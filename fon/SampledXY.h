#ifndef _SampledXY_h_
#define _SampledXY_h_
/* SampledXY.h
 *
 * Copyright (C) 1992-2011,2013-2017,2023,2024 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

#include "Sampled.h"

#include "SampledXY_def.h"

void SampledXY_init (mutableSampledXY me, double xmin, double xmax, integer nx, double dx, double x1,
                                          double ymin, double ymax, integer ny, double dy, double y1);

template <typename T> static inline double SampledXY_indexToY (constSampledXY me, T index) { return my y1 + (index - (T) 1) * my dy; }
static inline double SampledXY_yToIndex (constSampledXY me, double y) { return (y - my y1) / my dy + 1.0; }
static inline integer SampledXY_yToLowIndex     (constSampledXY me, double y) { return Melder_ifloor   ((y - my y1) / my dy + 1.0); }
static inline integer SampledXY_yToHighIndex    (constSampledXY me, double y) { return Melder_iceiling ((y - my y1) / my dy + 1.0); }
static inline integer SampledXY_yToNearestIndex (constSampledXY me, double y) { return Melder_iround   ((y - my y1) / my dy + 1.0); }

integer SampledXY_getWindowSamplesY (
	const constSampledXY me,
	const double ymin,
	const double ymax,
	integer *const iymin,
	integer *const iymax
);

void SampledXY_unidirectionalAutowindowY (
	const constSampledXY me,
	double *const ymin,
	double *const ymax
);
void SampledXY_bidirectionalAutowindowY (
	const constSampledXY me,
	double *const y1,
	double *const y2
);

/* End of file SampledXY.h */
#endif
