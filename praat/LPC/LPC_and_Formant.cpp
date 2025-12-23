/* LPC_and_Formant.cpp
 *
 * Copyright (C) 1994-2025 David Weenink
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

#include "LPC_and_Formant.h"
#include "LPC_and_Polynomial.h"
#include "NUM2.h"
#include "Roots_and_Formant.h"
#include "SampledAndSampled.h"

/*- Start of to be removed */
#if 0
Thing_implement (LPCFrameIntoFormantFrame, SampledFrameIntoSampledFrame, 0);

void structLPCFrameIntoFormantFrame :: initBasicLPCFrameIntoFormantFrame (constLPC inputLPC, mutableFormant outputFormant, double margin) {
	LPCFrameIntoFormantFrame_Parent :: initBasic (inputLPC, outputFormant);
	our inputLPC = inputLPC;
	our outputFormant = outputFormant;
	our margin = margin;
}

void structLPCFrameIntoFormantFrame :: copyBasic (constSampledFrameIntoSampledFrame other2) {
	constLPCFrameIntoFormantFrame other = static_cast <constLPCFrameIntoFormantFrame> (other2);
	LPCFrameIntoFormantFrame_Parent :: copyBasic (other);
	our inputLPC = other -> inputLPC;
	our outputFormant = other -> outputFormant;
	our margin = other -> margin;
}

void structLPCFrameIntoFormantFrame :: initHeap () {
	LPCFrameIntoFormantFrame_Parent :: initHeap ();
	our order = inputLPC -> maxnCoefficients;
	bufferSize = order * order + order + order + 11 * order;
	buffer = raw_VEC (bufferSize);		
	p = Polynomial_create (-1.0, 1.0, order);
	roots = Roots_create (order);
}

bool structLPCFrameIntoFormantFrame :: inputFrameIntoOutputFrame (integer iframe) {
	Formant_Frame formantFrame = & outputFormant -> frames [iframe];
	LPC_Frame inputLPCFrame = & inputLPC -> d_frames [iframe];
	formantFrame -> intensity = inputLPCFrame -> gain;
	integer frameAnalysisInfo = 0;
	if (inputLPCFrame -> nCoefficients == 0) {
		formantFrame -> numberOfFormants = 0;
		formantFrame -> formant.resize (formantFrame -> numberOfFormants); // maintain invariant
		frameAnalysisInfo = 1;	
		return true;
	}
	frameAnalysisInfo = 0;
	const double samplingFrequency = 1.0 / inputLPC -> samplingPeriod;
	autoPolynomial p = LPC_Frame_to_Polynomial (inputLPCFrame);
	Polynomial_into_Roots (p.get(), roots.get(), buffer.get());
	Roots_fixIntoUnitCircle (roots.get());
	Roots_into_Formant_Frame (roots.get(), formantFrame, samplingFrequency, margin);
	return true;
}

autoLPCFrameIntoFormantFrame LPCFrameIntoFormantFrame_create (constLPC inputLPC, mutableFormant outputFormant, double margin) {
	try {
		autoLPCFrameIntoFormantFrame me = Thing_new (LPCFrameIntoFormantFrame);
		my initBasicLPCFrameIntoFormantFrame (inputLPC, outputFormant, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create LPCFrameIntoFormantFrame.");
	}
}
#endif
/*- End of to be removed */

void Formant_Frame_init (Formant_Frame me, integer numberOfFormants) {
	if (numberOfFormants > 0)
		my formant = newvectorzero <structFormant_Formant> (numberOfFormants);
	my numberOfFormants = my formant.size; // maintain invariant
}

void LPC_into_Formant (constLPC inputLPC, mutableFormant outputFormant, double margin) {
	SampledAndSampled_requireEqualDomainsAndSampling (inputLPC, outputFormant);
	const integer numberOfFrames = inputLPC -> nx, thresholdNumberOfFramesPerThread = 40;
	autoMelderProgress progress (U"LPC into LineSpectralFrequencies...");
	const integer order = inputLPC -> maxnCoefficients;
	const integer bufferSize = order * order + order + order + 11 * order;
	const double samplingFrequency = 1.0 / inputLPC -> samplingPeriod;

	MelderThread_PARALLELIZE (numberOfFrames, thresholdNumberOfFramesPerThread)
		autoVEC buffer = raw_VEC (bufferSize);
		autoPolynomial p = Polynomial_create (-1.0, 1.0, order);
		autoRoots roots = Roots_create (order);
	MelderThread_FOR (iframe) {
		Formant_Frame formantFrame = & outputFormant -> frames [iframe];
		LPC_Frame inputLPCFrame = & inputLPC -> d_frames [iframe];
		formantFrame -> intensity = inputLPCFrame -> gain;
		if (inputLPCFrame -> nCoefficients == 0) {
			formantFrame -> numberOfFormants = 0; // TODO Formant_Frame -> resize (newNumberOfFormants)
			formantFrame -> formant.resize (formantFrame -> numberOfFormants); // maintain invariant
		} else {
			LPC_Frame_into_Polynomial (inputLPCFrame, p.get());
			Polynomial_into_Roots (p.get(), roots.get(), buffer.get());
			Roots_fixIntoUnitCircle (roots.get());
			Roots_into_Formant_Frame (roots.get(), formantFrame, samplingFrequency, margin);
		}
	} MelderThread_ENDFOR

	Formant_sort (outputFormant);
}

autoFormant LPC_to_Formant (constLPC me, double margin) {
	try {
		/*
			In very extreme case all roots of the lpc polynomial might be real.
			A real root gives either a frequency at 0 Hz or at the Nyquist frequency.
			If margin > 0 these frequencies are filtered out and the number of formants can never exceed
			(my maxnCoefficients+1) / 2.
		*/
		const integer maximumNumberOfFormants = ( margin == 0.0 ? my maxnCoefficients : (my maxnCoefficients + 1) / 2 );
		Melder_require (my maxnCoefficients < 100,
			U"We cannot find the roots of a polynomial of order > 99.");
		autoFormant outputFormant = Formant_create (my xmin, my xmax, my nx, my dx, my x1, maximumNumberOfFormants);
		for (integer iframe = 1; iframe <= outputFormant -> nx; iframe ++) {
			Formant_Frame_init (& outputFormant -> frames [iframe], maximumNumberOfFormants);
		}
		LPC_into_Formant (me, outputFormant.get(), margin);
		return outputFormant;
	} catch (MelderError) {
		Melder_throw (me, U": no Formant created.");
	}
}

void Formant_Frame_scale (Formant_Frame me, double scale) {
	for (integer iformant = 1; iformant <= my numberOfFormants; iformant ++) {
		my formant [iformant]. frequency *= scale;
		my formant [iformant]. bandwidth *= scale;
	}
}

void LPC_Frame_into_Formant_Frame (constLPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin) {
	Melder_assert (my nCoefficients == my a.size); // check invariant
	thy intensity = my gain;
	if (my nCoefficients == 0) {
		thy formant. resize (0);
		thy numberOfFormants = thy formant.size; // maintain invariant
		return;
	}
	autoPolynomial p = LPC_Frame_to_Polynomial (me);
	autoRoots r = Polynomial_to_Roots (p.get());
	Roots_fixIntoUnitCircle (r.get());
	Roots_into_Formant_Frame (r.get(), thee, 1.0 / samplingPeriod, margin);
}

void LPC_Frame_into_Formant_Frame (constLPC_Frame me, Formant_Frame thee, double samplingPeriod, 
	double margin, Polynomial p, Roots roots, VEC polynomialIntoRootsWorkspace)
{
	thy intensity = my gain;
	if (my nCoefficients == 0) {
		thy formant. resize (0);
		return;
	}
	LPC_Frame_into_Polynomial (me, p);
	Polynomial_into_Roots (p, roots, polynomialIntoRootsWorkspace);
	Roots_fixIntoUnitCircle (roots);
	Roots_into_Formant_Frame (roots, thee, 1.0 / samplingPeriod, margin);
}

void Formant_Frame_into_LPC_Frame (constFormant_Frame me, LPC_Frame thee, double samplingPeriod) {
	if (my numberOfFormants < 1)
		return;
	const double nyquistFrequency = 0.5 / samplingPeriod;
	integer numberOfPoles = 2 * my numberOfFormants;
	autoVEC lpc = zero_VEC (numberOfPoles + 2);   // all odd coefficients have to be initialized to zero
	lpc [2] = 1.0;
	integer m = 2;
	for (integer iformant = 1; iformant <= my numberOfFormants; iformant ++) {
		const double formantFrequency = my formant [iformant]. frequency;
		if (formantFrequency > nyquistFrequency)
			continue;
		/*
			D(z): 1 + p z^-1 + q z^-2
		*/
		const double r = exp (- NUMpi * my formant [iformant]. bandwidth * samplingPeriod);
		const double p = - 2.0 * r * cos (NUM2pi * formantFrequency * samplingPeriod);
		const double q = r * r;
		/*
			By setting the two extra elements (0, 1) in the lpc vector we can avoid boundary testing;
			lpc [3..n+2] come to contain the coefficients.
		*/
		for (integer j = m + 2; j > 2; j --)
			lpc [j] += p * lpc [j - 1] + q * lpc [j - 2];
		m += 2;
	}
	if (thy nCoefficients < numberOfPoles)
		numberOfPoles = thy nCoefficients;
	for (integer i = 1; i <= numberOfPoles; i ++)
		thy a [i] = lpc [i + 2];
	thy gain = my intensity;
}

autoLPC Formant_to_LPC (constFormant me, double samplingPeriod) {
	try {
		autoLPC outputLPC = LPC_create (my xmin, my xmax, my nx, my dx, my x1, 2 * my maxnFormants, samplingPeriod);
		const integer thresholdNumberOfFramesPerThread = 80;

		MelderThread_PARALLELIZE (my nx, thresholdNumberOfFramesPerThread)
		MelderThread_FOR (iframe) {
			const Formant_Frame f = & my frames [iframe];
			const LPC_Frame lpcFrame = & outputLPC -> d_frames [iframe];
			const integer numberOfCoefficients = 2 * f -> numberOfFormants;
			LPC_Frame_init (lpcFrame, numberOfCoefficients);
			Formant_Frame_into_LPC_Frame (f, lpcFrame, samplingPeriod);
		} MelderThread_ENDFOR

		return outputLPC;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created.");
	}
}

/* End of file LPC_and_Formant.cpp */
