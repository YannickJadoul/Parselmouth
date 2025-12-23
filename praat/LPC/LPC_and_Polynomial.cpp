/* LPC_and_Polynomial.cpp
 *
 * Copyright (C) 1994-2020, 2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

/*
 djmw 20020812 GPL header
*/

#include "LPC_and_Polynomial.h"

autoPolynomial LPC_Frame_to_Polynomial (constLPC_Frame me) {
	const integer numberOfPolynomialCoefficients = my nCoefficients + 1;
	autoPolynomial thee = Polynomial_create (-1, 1, my nCoefficients);
	LPC_Frame_into_Polynomial (me, thee.get());
	for (integer icof = 1; icof <= my nCoefficients; icof ++)
		thy coefficients [icof] = my a [numberOfPolynomialCoefficients - icof];
	thy coefficients [numberOfPolynomialCoefficients] = 1.0;
	return thee;
}

void LPC_Frame_into_Polynomial (constLPC_Frame me, mutablePolynomial p) {
	/*
		The lpc coefficients are a [1..nCoefficients]. a[0] == 1 is not stored.
		For the polynomial we therefore need one extra coefficient. 
		Since the a's are stored in reverse order in the polynomial and a[0]
		represents the highest power (==degree) it is stored into the last position
		of the polynomial.
	*/
	const integer numberOfPolynomialCoefficientsNeeded = my nCoefficients + 1;
	p -> resize (numberOfPolynomialCoefficientsNeeded);
	for (integer icof = 1; icof <= my nCoefficients; icof ++)
		p -> coefficients [icof] = my a [numberOfPolynomialCoefficientsNeeded - icof];
	p -> coefficients [numberOfPolynomialCoefficientsNeeded] = 1.0;
}

autoPolynomial LPC_to_Polynomial (constLPC me, double time) {
	try {
		integer iFrame = Sampled_xToIndex (me, time);
		Melder_clip (1_integer, & iFrame, my nx);   // constant extrapolation
		autoPolynomial thee = LPC_Frame_to_Polynomial (& my d_frames [iFrame]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Polynomial created.");
	}
}

/* End of file LPC_and_Polynomial.cpp */
