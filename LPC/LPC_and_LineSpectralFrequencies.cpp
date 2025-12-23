/* LPC_and_LineSpectralFrequencies.cpp
 *
 * Copyright (C) 2016-2021,2024,2025 David Weenink
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

/*
 djmw 20160421  Initial version
*/

#include "LPC_and_LineSpectralFrequencies.h"
#include "NUM2.h"
#include "SampledIntoSampled.h"

/*
	Conversion from Y(w) to a polynomial in x (= 2 cos (w))
	From: Joseph Rothweiler (1999), "On Polynomial Reduction in the Computation of LSP Frequencies."
	IEEE Trans. on ASSP 7, 592--594.
*/
static void cos2x (VECVU const& g) {
	for (integer i = 3; i <= g.size; i ++) {
		for (integer j = g.size; j > i; j --)
			g [j - 2] -= g [j];
		g [i - 2] -= 2.0 * g [i];
	}
}

static void LPC_Frame_into_Polynomial_sum (LPC_Frame me, Polynomial psum) {
	/*
		Fs (z) = A(z) + z^-(p+1) A(1/z)
	*/
	const integer order = my nCoefficients, g_order = (order + 1) / 2; // orders
	psum -> coefficients [order + 2] = 1.0;
	for (integer i = 1; i <= order; i ++)
		psum -> coefficients [order + 2 - i] = my a [i] + my a [order + 1 - i];

	psum -> coefficients [1] = 1.0;
	psum -> numberOfCoefficients = order + 2;

	if (order % 2 == 0) // order even
		Polynomial_divide_firstOrderFactor (psum, -1.0, nullptr);
	/*
		Transform to cos(w) terms
	*/
	for (integer i = 1; i <= g_order + 1; i ++)
		psum ->  coefficients [i] = psum -> coefficients [g_order + i];

	psum -> numberOfCoefficients = g_order + 1;
	/*
		To Chebychev
	*/
	cos2x (psum -> coefficients.part (1, psum -> numberOfCoefficients));
}

static void LPC_Frame_into_Polynomial_dif (LPC_Frame me, Polynomial pdif) {
	/*
		Fa (z) = A(z) - z^-(p+1)A(1/z)
	*/
	const integer order = my nCoefficients;
	pdif -> coefficients [order + 2] = -1.0;
	for (integer i = 1; i <= order; i ++)
		pdif -> coefficients [order + 2 - i] = - my a [i] + my a [order + 1 - i];

	pdif -> coefficients [1] = 1.0;
	pdif -> numberOfCoefficients = order + 2;

	if (order % 2 == 0) {
		/*
			Fa(z)/(z-1)
		*/
		Polynomial_divide_firstOrderFactor (pdif, 1.0, nullptr);
	} else {
		/*
			Fa(z) / (z^2 - 1)
		*/
		Polynomial_divide_secondOrderFactor (pdif, 1.0);
	}
	/*
		Transform to cos(w) terms
	*/
	integer g_order = pdif -> numberOfCoefficients / 2;
	for (integer i = 1; i <= g_order + 1; i ++)
		pdif -> coefficients [i] = pdif -> coefficients [g_order + i];

	pdif -> numberOfCoefficients = g_order + 1;
	/*
		To Chebychev
	*/
	cos2x (pdif -> coefficients.part (1, pdif -> numberOfCoefficients));
}

static integer Polynomial_into_Roots_searchOnGrid (Polynomial me, Roots thee, double gridSize) {
	Melder_assert (thy numberOfRoots >= my numberOfCoefficients - 1);
	double xmin = my xmin;
	integer numberOfRootsFound = 0;
	while (xmin < my xmax && numberOfRootsFound < my numberOfCoefficients - 1) {
		double xmax = xmin + gridSize;
		xmax = xmax > my xmax ? my xmax : xmax;
		const double root = Polynomial_findOneSimpleRealRoot_ridders (me, xmin, xmax);
		if (isdefined (root) && (numberOfRootsFound == 0 || thy roots [numberOfRootsFound].real() != root)) {
			thy roots [++ numberOfRootsFound]. real (root); // root not at border of interval
			thy roots [numberOfRootsFound]. imag (0.0);
		}
		xmin = xmax;
	}
	return numberOfRootsFound;
}

void LPC_into_LineSpectralFrequencies (constLPC inputLPC, mutableLineSpectralFrequencies outputLSF, double gridSize) {
	SampledIntoSampled_requireEqualDomainsAndSampling (inputLPC, outputLSF);
	if (gridSize <= 0.0)
		gridSize = 0.02;
	const integer numberOfFrames = inputLPC -> nx, thresholdNumberOfFramesPerThread = 40;
	const integer numberOfCoefficients = inputLPC -> maxnCoefficients + 1;
	autoMelderProgress progress (U"LPC into LineSpectralFrequencies...");

	MelderThread_PARALLELIZE (numberOfFrames, thresholdNumberOfFramesPerThread)

		autoPolynomial gsum = Polynomial_create (-2.0, 2.0, numberOfCoefficients);   // large enough
		autoPolynomial gdif = Polynomial_create (-2.0, 2.0, numberOfCoefficients);
		autoRoots roots = Roots_create ((numberOfCoefficients + 1) / 2);

	MelderThread_FOR (iframe) {

		if (MelderThread_IS_MASTER) {
			const double estimatedProgress = MelderThread_ESTIMATED_PROGRESS;
			Melder_progress (0.98 * estimatedProgress,
				U"Analysed approximately ", Melder_iround (numberOfFrames * estimatedProgress),
				U" out of ", numberOfFrames, U" frames"
			);
		}
		LPC_Frame inputFrame = & inputLPC -> d_frames [iframe];
		LineSpectralFrequencies_Frame outputFrame = & outputLSF -> d_frames [iframe];
		Melder_assert (inputFrame -> nCoefficients == inputFrame -> a.size); // check invariant
		const double maximumFrequency = outputLSF -> maximumFrequency;
		/*
			Construct Fs and Fa
			divide out the zeros
			transform to polynomial equations gsum and gdif of half the order
		*/
		LPC_Frame_into_Polynomial_sum (inputFrame, gsum.get());
		const integer half_order_gsum = gsum -> numberOfCoefficients - 1;
		LPC_Frame_into_Polynomial_dif (inputFrame, gdif.get());
		const integer half_order_gdif = gdif -> numberOfCoefficients - 1;
		double currentGridSize = gridSize;
		integer numberOfBisections = 0, numberOfRootsFound = 0;
		while (numberOfRootsFound  < half_order_gsum && numberOfBisections < 10) {
			numberOfRootsFound = Polynomial_into_Roots_searchOnGrid (gsum.get(), roots.get(), currentGridSize);
			currentGridSize *= 0.5;
			numberOfBisections++;
		}
		integer frameAnalysisInfo = 0;
		if (numberOfBisections >= 10)
			frameAnalysisInfo = 1; // too many bisections
		/*
			[gsum-> xmin, gsum -> xmax] <==> [nyquistFrequency, 0],
			i.e. highest root corresponds to lowest frequency
		*/
		for (integer i = 1; i <= half_order_gsum; i ++)
			outputFrame -> frequencies [2 * i - 1] = acos (roots -> roots [half_order_gsum + 1 - i].real() / 2.0) / NUMpi * maximumFrequency;
		/*
			The roots of gdif lie inbetween the roots of gsum
		*/
		for (integer i = 1; i <= half_order_gdif; i ++) {
			const double xmax = roots -> roots [half_order_gsum + 1 - i].real();
			const double xmin = ( i == half_order_gsum ? gsum -> xmin : roots -> roots [half_order_gsum - i].real() );
			const double root = Polynomial_findOneSimpleRealRoot_ridders (gdif.get(), xmin, xmax);
			if (isdefined (root))
				outputFrame -> frequencies [2 * i] = acos (root / 2.0) / NUMpi * maximumFrequency;
			else
				outputFrame -> numberOfFrequencies --;
		}
		outputFrame -> frequencies.resize (outputFrame -> numberOfFrequencies); // maintain invariant		

	} MelderThread_ENDFOR
}

autoLineSpectralFrequencies LPC_to_LineSpectralFrequencies (constLPC me, double gridSize) {
	try {
		const double nyquistFrequency = 0.5 / my samplingPeriod;
		autoLineSpectralFrequencies outputLSF = LineSpectralFrequencies_create (my xmin, my xmax, my nx, my dx, my x1, my maxnCoefficients, nyquistFrequency);
		for (integer iframe = 1; iframe <= outputLSF -> nx; iframe ++)
			LineSpectralFrequencies_Frame_init (& outputLSF -> d_frames [iframe], outputLSF -> maximumNumberOfFrequencies);
		LPC_into_LineSpectralFrequencies (me, outputLSF.get(), gridSize);
		return outputLSF;
	} catch (MelderError) {
		Melder_throw (me, U": no LineSpectralFrequencies created.");
	}
}

/**************************** LineSpectralFrequencies to LPC **********************************/

void LineSpectralFrequencies_into_LPC (constLineSpectralFrequencies me, mutableLPC outputLPC) {
	SampledIntoSampled_requireEqualDomainsAndSampling (me, outputLPC);
	const integer numberOfFrames = my nx, thresholdNumberOfFramesPerThread = 40;
	autoMelderProgress progress (U"LineSpectralFrequencies_into_LPC...");

	MelderThread_PARALLELIZE (numberOfFrames, thresholdNumberOfFramesPerThread)
		autoPolynomial fs = Polynomial_create (-1.0, 1.0, my maximumNumberOfFrequencies + 2);
		autoPolynomial fa = Polynomial_create (-1.0, 1.0, my maximumNumberOfFrequencies + 2);
	MelderThread_FOR (iframe) {
		if (MelderThread_IS_MASTER) {
			const double estimatedProgress = MelderThread_ESTIMATED_PROGRESS;
			Melder_progress (0.98 * estimatedProgress,
				U"Analysed approximately ", Melder_iround (numberOfFrames * estimatedProgress),
				U" out of ", numberOfFrames, U" frames"
			);
		}
		LPC_Frame lpcFrame = & outputLPC -> d_frames [iframe];
		VEC a = lpcFrame -> a.get();
		LineSpectralFrequencies_Frame lsfFrame = & my d_frames [iframe];
		const double maximumFrequency = my maximumFrequency;
		const integer numberOfFrequencies = lsfFrame -> numberOfFrequencies;
		integer numberOfOmegas = (numberOfFrequencies + 1) / 2;
		/*
			Reconstruct Fs (z)
			Use lpcFrame -> a as a buffer whose size changes!!!
		*/
		for (integer i = 1; i <= numberOfOmegas; i ++) {
			const double omega = lsfFrame -> frequencies [2 * i - 1] / maximumFrequency * NUMpi;
			a [i] = -2.0 * cos (omega);
		}
		Polynomial_initFromProductOfSecondOrderTerms (fs.get(), a.part (1, numberOfOmegas));
		/*
			Reconstruct Fa (z)
		*/
		numberOfOmegas = numberOfFrequencies / 2;
		for (integer i = 1; i <= numberOfOmegas; i ++) {
			const double omega = lsfFrame -> frequencies [2 * i] / maximumFrequency * NUMpi;
			a [i] = -2.0 * cos (omega);
		}
		Polynomial_initFromProductOfSecondOrderTerms (fa.get(), a.part (1, numberOfOmegas));
	
		if (numberOfFrequencies % 2 == 0) {
			Polynomial_multiply_firstOrderFactor (fs.get(), -1.0);   // * (z + 1)
			Polynomial_multiply_firstOrderFactor (fa.get(), 1.0);   // * (z - 1)
		} else {
			Polynomial_multiply_secondOrderFactor (fa.get(), 1.0);   // * (z^2 - 1)
		}
		Melder_assert (fs -> numberOfCoefficients == fa -> numberOfCoefficients);
		/*
			A(z) = (Fs(z) + Fa(z) / 2
		*/
		for (integer i = 1; i <= fs -> numberOfCoefficients - 2; i ++)
			a [lsfFrame -> numberOfFrequencies - i + 1] = 0.5 * (fs -> coefficients [i + 1] + fa -> coefficients [i + 1]);
	} MelderThread_ENDFOR
}

autoLPC LineSpectralFrequencies_to_LPC (constLineSpectralFrequencies me) {
	try {
		autoLPC thee = LPC_createCompletelyInitialized (my xmin, my xmax, my nx, my dx, my x1, my maximumNumberOfFrequencies,
			0.5 / my maximumFrequency);
		LineSpectralFrequencies_into_LPC (me, thee.get());	
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created from LineSpectralFrequencies.");
	}
}

/* End of file LPC_and_LineSpectralFrequencies.cpp */
