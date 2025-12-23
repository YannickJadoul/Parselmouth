#ifndef _LPC_and_LineSpectralFrequencies_h_
#define _LPC_and_LineSpectralFrequencies_h_
/* LPC_and_LineLineSpectralFrequencies.h
 *
 * Copyright (C) 2016,2025 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "LPC.h"
#include "LineSpectralFrequencies.h"
#include "Polynomial.h"
#include "Roots.h"

void LPC_into_LineSpectralFrequencies (constLPC me, mutableLineSpectralFrequencies outputLSF, double gridSize);

autoLineSpectralFrequencies LPC_to_LineSpectralFrequencies (constLPC me, double gridSize);

void LineSpectralFrequencies_into_LPC (constLineSpectralFrequencies me, mutableLPC outputLPC);

autoLPC LineSpectralFrequencies_to_LPC (constLineSpectralFrequencies me);

#endif /* _LPC_and_LineSpectralFrequencies_h_ */
