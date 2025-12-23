#ifndef _SoundFrameIntoSampledFrame_h_
#define _SoundFrameIntoSampledFrame_h_
/* SoundFrameIntoSampledFrame.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "melder.h"
#include "NUMFourier.h"
#include "Sound_extensions.h"
#include "SampledFrameIntoSampledFrame.h"
#include "Spectrum.h"

inline integer getSoundFrameSize2_odd (double approximatePhysicalAnalysisWidth, double samplingPeriod) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / samplingPeriod);
	return 2 * halfFrameSamples + 1;
}

inline integer getSoundFrameSize2 (double physicalAnalysisWidth, double samplingPeriod) {
	Melder_assert (physicalAnalysisWidth > 0.0);
	Melder_assert (samplingPeriod > 0.0);
	return Melder_iround (physicalAnalysisWidth / samplingPeriod);
}

inline double getPhysicalAnalysisWidth2 (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = ( windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth )
	;
	return physicalAnalysisWidth;
}

Thing_define (SoundFrameIntoSampledFrame, SampledFrameIntoSampledFrame) {

	constSound inputSound;
	double physicalAnalysisWidth; 			// depends on the effectiveAnalysiswidth and the window window shape
	integer soundFrameSize; 				// determined by the physicalAnalysisWidth and the samplingFrequency of the Sound
	autoSound frameAsSound;
	double soundFrameExtremum;				// the largest amplitude in the inputSound frame either positive or negative
	autoVEC windowFunction;					// the actual window used of size soundFrameSize
	VEC soundFrame;
	kSound_windowShape windowShape;			// Type: Rectangular, triangular, hamming, etc..
	bool subtractFrameMean = true;			// if true, the frame mean will be subtracted before the windowing operation
	bool wantSpectrum = false;				// the spectrum of the frameAsSound;
	autoSpectrum spectrum;
	integer fftInterpolationFactor = 1;		// 0 = DFT, 1 = FFT, 2, 4, 8 FFT with extra zero's
	integer numberOfFourierSamples;
	autoVEC fourierSamples;					// size = numberOfFourierSamples
	autoNUMFourierTable fourierTable;		// of dimension numberOfFourierSamples;

	void initBasicSoundFrameIntoSampledFrame (constSound initialInput, mutableSampled initialOutput, double effectiveAnalysisWidth,
		kSound_windowShape initialWindowShape)
	{
		our SoundFrameIntoSampledFrame_Parent :: initBasic (initialInput, initialOutput);
		our inputSound = initialInput;
		our windowShape = initialWindowShape;
		our physicalAnalysisWidth = getPhysicalAnalysisWidth2 (effectiveAnalysisWidth, initialWindowShape);
	}

	void copyBasic (constSampledFrameIntoSampledFrame other2) override {
		constSoundFrameIntoSampledFrame other = static_cast <constSoundFrameIntoSampledFrame> (other2);
		our SoundFrameIntoSampledFrame_Parent :: copyBasic (other);
		our inputSound = other -> inputSound;
		our physicalAnalysisWidth = other -> physicalAnalysisWidth;
		our windowShape = other -> windowShape;
		our subtractFrameMean = other -> subtractFrameMean;
		our wantSpectrum = other -> wantSpectrum;
		our fftInterpolationFactor = other -> fftInterpolationFactor;
	}

	void getInputFrame (integer currentFrame) override {
		const double midTime = Sampled_indexToX (our output, currentFrame);
		integer soundFrameBegin = Sampled_xToNearestIndex (our inputSound, midTime - 0.5 * our physicalAnalysisWidth);   // approximation

		for (integer isample = 1; isample <= soundFrame.size; isample ++, soundFrameBegin ++)
			our soundFrame [isample] = ( soundFrameBegin > 0 && soundFrameBegin <= our inputSound -> nx ? our inputSound -> z [1] [soundFrameBegin] : 0.0 );
		if (our subtractFrameMean)
			centre_VEC_inout (our soundFrame, nullptr);
		our soundFrameExtremum = NUMextremum_u (our soundFrame);
		our soundFrame  *=  windowFunction.get();
		if (our wantSpectrum)
			our soundFrameIntoSpectrum ();
	}

	void initHeap () override {
		our SoundFrameIntoSampledFrame_Parent :: initHeap ();
		our soundFrameSize = getSoundFrameSize2 (our physicalAnalysisWidth, our inputSound -> dx);
		our windowFunction = raw_VEC (our soundFrameSize);   // TODO: move out of thread repetition
		windowShape_into_VEC (our windowShape, our windowFunction.get());   // TODO: move out of thread repetition
		our frameAsSound = Sound_create (1_integer, 0.0, our soundFrameSize * input -> dx, our soundFrameSize, our input -> dx, 0.5 * our input -> dx); //
		our soundFrame = our frameAsSound -> z.row (1);
		Melder_assert (our soundFrame.size == our soundFrameSize);
		if (our wantSpectrum) {
			our numberOfFourierSamples = our frameAsSound -> nx;
			if (our fftInterpolationFactor > 0) {
				our numberOfFourierSamples = Melder_iroundUpToPowerOfTwo (our numberOfFourierSamples);
				for (integer imultiply = fftInterpolationFactor; imultiply > 1; imultiply --)
					our numberOfFourierSamples *= 2;
			}
			our fourierSamples = raw_VEC (our numberOfFourierSamples);
			const integer numberOfFrequencies = our numberOfFourierSamples / 2 + 1;
			our fourierTable = NUMFourierTable_create (our numberOfFourierSamples);
			our spectrum = Spectrum_create (0.5 / our frameAsSound -> dx, numberOfFrequencies);
			our spectrum -> dx = 1.0 / (our frameAsSound -> dx * our numberOfFourierSamples);
		}
	}

	void soundFrameToForwardFourierTransform () {
		const integer numberOfChannels = our frameAsSound -> ny;
		if (numberOfChannels == 1)
			our fourierSamples.part (1, our soundFrameSize)  <<=  our frameAsSound -> z.row (1);
		else {
			/*
				Multiple channels: take the average.
			*/
			for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
				our fourierSamples.part (1, our soundFrameSize)  +=  our frameAsSound -> z.row (ichan);
			our fourierSamples.part (1, our soundFrameSize)  *=  1.0 / numberOfChannels;
		}
		our fourierSamples.part (our soundFrameSize + 1, our numberOfFourierSamples)  <<=  0.0;
		NUMfft_forward (our fourierTable.get(), our fourierSamples.get());
	}

	void soundFrameIntoSpectrum () {

		our soundFrameToForwardFourierTransform ();

		const VEC re = our spectrum -> z.row (1);
		const VEC im = our spectrum -> z.row (2);
		const integer numberOfFrequencies = our spectrum -> nx;
		const double scaling = our output -> dx;
		re [1] = our fourierSamples [1] * scaling;
		im [1] = 0.0;
		for (integer i = 2; i < numberOfFrequencies; i ++) {
			re [i] = our fourierSamples [i + i - 2] * scaling;   // fourierSamples [2], [4], ...
			im [i] = our fourierSamples [i + i - 1] * scaling;   // fourierSamples [3], [5], ...
		}
		if ((our numberOfFourierSamples & 1) != 0) {
			if (our numberOfFourierSamples > 1) {
				re [numberOfFrequencies] = our fourierSamples [our numberOfFourierSamples - 1] * scaling;
				im [numberOfFrequencies] = our fourierSamples [our numberOfFourierSamples] * scaling;
			}
		} else {
			re [numberOfFrequencies] = our fourierSamples [our numberOfFourierSamples] * scaling;
			im [numberOfFrequencies] = 0.0;
		}
	}
};

#endif /* _SoundFrameIntoSampledFrame_h_ */
