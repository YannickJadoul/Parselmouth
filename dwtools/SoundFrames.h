#ifndef _SoundFrames_h_
#define _SoundFrames_h_
/* SoundFrames.h
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

//#include "melder.h"
#include "NUMFourier.h"
#include "Sound_extensions.h"
//#include "SampledFrameIntoSampledFrame.h"
#include "Spectrum.h"

inline integer getSoundFrameSize_odd (double approximatePhysicalAnalysisWidth, double samplingPeriod) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / samplingPeriod);
	return 2 * halfFrameSamples + 1;
}

inline integer getSoundFrameSize (double physicalAnalysisWidth, double samplingPeriod) {
	Melder_assert (physicalAnalysisWidth > 0.0);
	Melder_assert (samplingPeriod > 0.0);
	return Melder_iround (physicalAnalysisWidth / samplingPeriod);
}

inline double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = ( windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth )
	;
	return physicalAnalysisWidth;
}

Thing_define (SoundFrames, Thing) {

	constSound inputSound;
	double t1, dt;
	integer numberOfFrames;
	double physicalAnalysisWidth;			// depends on the effectiveAnalysiswidth and the window window shape
	integer soundFrameSize; 				// determined by the physicalAnalysisWidth and the samplingFrequency of the Sound
	autoSound frameAsSound;					// Always a one-channel Sound, default channel 1
	double soundFrameExtremum;				// the largest amplitude in the inputSound frame either positive or negative
	autoVEC windowFunction;					// the actual window used of size soundFrameSize
	VEC soundFrame;							// row [1] of the frameAsSound
	kSound_windowShape windowShape;			// Type: Rectangular, triangular, hamming, etc..
	bool subtractFrameMean = true;			// if true, the frame mean will be subtracted before windowing
	#if 0
	bool wantSpectrum = false;				// the spectrum of the frameAsSound;
	autoSpectrum spectrum;
	integer fftInterpolationFactor = 1;		// 0 = DFT, 1 = FFT, 2, 4, 8 FFT with extra zero's
	integer numberOfFourierSamples;
	autoVEC fourierSamples;					// size = numberOfFourierSamples
	autoNUMFourierTable fourierTable;		// of dimension numberOfFourierSamples;
	#endif

private:
	
	void initCommon (kSound_windowShape windowShape, bool subtractFrameMean);
	
public:
	
	void init (constSound input, double effectiveAnalysisWidth, double timeStep,
		kSound_windowShape windowShape, bool subtractFrameMean);
	
	/*
		Initialise the object and use the sampling (x1, dx, nx) of the Sampled.
	*/
	void initWithSampled (constSound input, constSampled output, double effectiveAnalysisWidth,
		kSound_windowShape windowShape, bool subtractFrameMean);
	
	VEC getFrame (integer iframe);

	void soundFrameToForwardFourierTransform ();

	void soundFrameIntoSpectrum ();
};

autoSoundFrames SoundFrames_createWithSampled (constSound input, constSampled output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, bool subtractFrameMean);

autoSoundFrames SoundFrames_create (constSound input, double effectiveAnalysisWidth, double timeStep,
	kSound_windowShape windowShape, bool subtractFrameMean);

#endif /* _SoundFrames_h_ */
