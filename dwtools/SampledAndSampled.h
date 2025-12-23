#ifndef _SampledAndSampled_h_
#define _SampledAndSampled_h_
/* SampledAndSampled.h
 *
 * Copyright (C) 2025 David Weenink
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

#include "Sampled.h"

inline void SampledAndSampled_requireEqualSampling (constSampled me,  constSampled thee) {
	Melder_require (my x1 == thy x1 && my nx == thy nx && my dx == thy dx,
		U"The sampling of ", me, U" and ", thee, U" should be equal.");
}

inline void SampledAndSampled_assertEqualSampling (constSampled me,  constSampled thee) {
	Melder_assert (my x1 == thy x1 && my nx == thy nx && my dx == thy dx);
}

inline void SampledAndSampled_requireEqualDomains (constSampled me,  constSampled thee) {
	Melder_require (my xmin == thy xmin && my xmax == thy xmax,
		U"The domains of ", me, U" and ", thee, U" should be equal.");
}

inline void SampledAndSampled_assertEqualDomains (constSampled me,  constSampled thee) {
	Melder_assert (my xmin == thy xmin && my xmax == thy xmax);
}

inline void SampledAndSampled_requireEqualDomainsAndSampling (constSampled me,  constSampled thee) {
	SampledAndSampled_requireEqualDomains (me, thee);
	SampledAndSampled_requireEqualSampling (me, thee);
}

inline void SampledAndSampled_assertEqualDomainsAndSampling (constSampled me,  constSampled thee) {
	SampledAndSampled_assertEqualDomains (me, thee);
	SampledAndSampled_assertEqualSampling (me, thee);
}

#endif /* _SampledAndSampled_h_ */

