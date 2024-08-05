/* CCs_to_DTW.c
 *
 *	Dynamic Time Warp of two CCs.
 *
 * Copyright (C) 1993-2018 David Weenink
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
 djmw 2001
 djmw 20020315 GPL header
 djmw 20080122 float -> double
 */

#include "CCs_to_DTW.h"

static void regression (const VEC r, const CC me, const integer frameNumber, const integer numberOfCoefficients) {

	// sum(i^2;i=-n..n) = 2n^3/3 + n^2 +n/3 = n (n (2n/3 + 1) + 1/3);

	const integer numberOfCoefficientsd2 = numberOfCoefficients / 2;   // numberOfCoefficients is always odd and > 2!
	const double sumsq = numberOfCoefficientsd2 * (numberOfCoefficientsd2 * (numberOfCoefficients / 3.0 + 1.0) + 1.0 / 3.0);

	if (frameNumber <= numberOfCoefficientsd2 || frameNumber >= my nx - numberOfCoefficientsd2)
		return;

	r  <<=  0.0;

	const integer nmin = CC_getMinimumNumberOfCoefficients (me, frameNumber - numberOfCoefficientsd2, frameNumber + numberOfCoefficientsd2);
	const integer nmax = CC_getMaximumNumberOfCoefficients (me, frameNumber - numberOfCoefficientsd2, frameNumber + numberOfCoefficientsd2);

	//TRACE
	trace (U"frame number: ", frameNumber);
	trace (U"numberOfCoefficients: ", numberOfCoefficients);
	trace (U"numberOfCoefficientsd2: ", numberOfCoefficientsd2);
	trace (U"number of frames: ", my frame.size);
	trace (U"minimumNumberOfCoefficients: ", nmin);
	trace (U"maximumNumberOfCoefficients: ", nmax);
	trace (U"sumsq: ", sumsq);
	for (integer i = 1; i <= nmin + 1; i ++) {
		longdouble ri = 0.0;
		for (integer j = -numberOfCoefficientsd2; j <= numberOfCoefficientsd2; j ++) {
			const CC_Frame cf = & my frame [frameNumber + j];
			if (i <= cf -> c.size) {
				const double c = ( i == 1 ? cf -> c0 : cf -> c [i] );
				ri += c * j;
			}
		}
		r [i] = double (ri) / sumsq / my dx;
	}
}

autoDTW CCs_to_DTW (const CC me, const CC thee,
	const double coefficientWeight, const double logEnergyWeight,
	const double coefficientRegressionWeight, const double logEnergyRegressionWeight,
	const double regressionWindowLength
) {
	try {
		integer numberOfCoefficients = Melder_ifloor (regressionWindowLength / my dx);
		
		Melder_require (my maximumNumberOfCoefficients == thy maximumNumberOfCoefficients,
			U"The maximum number of coefficients should be equal.");
		Melder_require (! (coefficientRegressionWeight != 0.0 && numberOfCoefficients < 2),
			U"Time window for regression is too small.");

		if (numberOfCoefficients % 2 == 0)
			numberOfCoefficients ++;

		Melder_assert (numberOfCoefficients > 1 || (coefficientRegressionWeight == 0.0 && logEnergyRegressionWeight == 0.0));
//TRACE
trace(1, U" regression window length ", regressionWindowLength, U" dx ", my dx, U" #coeff ", numberOfCoefficients);
		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
trace(2);
		autoVEC ri = raw_VEC (my maximumNumberOfCoefficients + 1);
trace(3);
		autoVEC rj = raw_VEC (my maximumNumberOfCoefficients + 1);
trace(4);

		/*
			Calculate distance matrix.
		*/
		autoMelderProgress progress (U"CCs_to_DTW");
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			trace (U"iframe ", iframe, U"/", my nx);
			const CC_Frame fi = & my frame [iframe];
trace(5);
			if (coefficientRegressionWeight != 0.0 || logEnergyRegressionWeight != 0.0)
				regression (ri.get(), me, iframe, numberOfCoefficients);

trace(6);
			for (integer jframe = 1; jframe <= thy nx; jframe ++) {
				//trace (U"jframe ", jframe, U"/", thy nx);
				const CC_Frame fj = & thy frame [jframe];
				longdouble dist = 0.0;

				if (coefficientWeight != 0.0) {
					for (integer k = 1; k <= fj -> numberOfCoefficients; k ++) {
						const double d = fi -> c [k] - fj -> c [k];
						dist += d * d;
					}
					dist *= coefficientWeight;
				}

				if (logEnergyWeight != 0.0) {
					const double d = fi -> c0 - fj -> c0;
					dist += logEnergyWeight * d * d;
				}

				if (coefficientRegressionWeight != 0.0) {
					longdouble distr = 0.0;
					regression (rj.get(), thee, jframe, numberOfCoefficients);
					for (integer k = 2; k <= fj -> numberOfCoefficients + 1; k ++) {
						const double d = ri [k] - rj [k];
						distr += d * d;
					}
					dist += coefficientRegressionWeight * distr;
				}

				if (logEnergyRegressionWeight != 0.0) {
					if (coefficientRegressionWeight == 0.0)
						regression (rj.get(), thee, jframe, numberOfCoefficients);
					const double d = ri [1] - rj [1];
					dist += logEnergyRegressionWeight * d * d;
				}

				dist /= coefficientWeight + logEnergyWeight + coefficientRegressionWeight + logEnergyRegressionWeight;
				his z [iframe] [jframe] = sqrt ((double) dist);   // prototype along y-direction
			}

			if (iframe % 10 == 1)
				Melder_progress (0.999 * iframe / my nx, U"Calculate distances: frame ", iframe, U" from ", my nx, U".");
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created from CCs.");
	}
}

/* End of file CCs_to_DTW.cpp */
