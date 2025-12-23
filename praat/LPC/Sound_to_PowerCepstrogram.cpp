/* Sound_to_PowerCepstrogram.cpp
 *
 * Copyright (C) 2012-2025 David Weenink, 2025 Paul Boersma
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

#include "NUM2.h"
#include "Cepstrum_and_Spectrum.h"
#include "SampledIntoSampled.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "Sound_to_PowerCepstrogram.h"
#include "SoundFrameIntoSampledFrame.h"   // needed only for getPhysicalAnalysisWidth2; TODO: move that function into the right location

static void Sound_into_PowerCepstrogram (constSound input, mutablePowerCepstrogram output, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	SampledIntoSampled_assertEqualDomains (input, output);
	constexpr integer thresholdNumberOfFramesPerThread = 40;
	const integer numberOfFrames = output -> nx;
	//autoMelderProgress progress (U"Analyse power cepstrogram...");

	/*
		The stuff that is constant, and equal among threads.
	*/
	const double physicalAnalysisWidth = getPhysicalAnalysisWidth2 (effectiveAnalysisWidth, windowShape);
	const integer soundFrameSize = getSoundFrameSize2 (physicalAnalysisWidth, input -> dx);
	autoVEC windowFunction = raw_VEC (soundFrameSize);
	windowShape_into_VEC (windowShape, windowFunction.get());

	MelderThread_PARALLELIZE (numberOfFrames, thresholdNumberOfFramesPerThread)
		bool subtractFrameMean = true;   // TODO: check
		integer fftInterpolationFactor = 1;

		autoPowerCepstrum powerCepstrum = PowerCepstrum_create (output -> ymax, output -> ny);
		autoSound frameAsSound = Sound_create (1, 0.0, soundFrameSize * input -> dx, soundFrameSize, input -> dx, 0.5 * input -> dx);
		VEC soundFrame = frameAsSound -> z.row (1);
		Melder_assert (soundFrame.size == soundFrameSize);

		/*
			Spectrum.
		*/
		integer numberOfFourierSamples = frameAsSound -> nx;
		if (fftInterpolationFactor > 0) {
			numberOfFourierSamples = Melder_iroundUpToPowerOfTwo (numberOfFourierSamples);
			for (integer imultiply = fftInterpolationFactor; imultiply > 1; imultiply --)
				numberOfFourierSamples *= 2;
		}
		autoVEC fourierSamples = raw_VEC (numberOfFourierSamples);
		const integer numberOfFrequencies = numberOfFourierSamples / 2 + 1;
		autoNUMFourierTable fourierTable = NUMFourierTable_create (numberOfFourierSamples);
		autoSpectrum spectrum = Spectrum_create (0.5 / frameAsSound -> dx, numberOfFrequencies);
		spectrum -> dx = 1.0 / (frameAsSound -> dx * numberOfFourierSamples);
	MelderThread_FOR (iframe) {
		if (MelderThread_IS_MASTER && 0) {
			const double estimatedProgress = MelderThread_ESTIMATED_PROGRESS;
			Melder_progress (0.98 * estimatedProgress,
				U"Analysed approximately ", Melder_iround (numberOfFrames * estimatedProgress),
				U" out of ", numberOfFrames, U" frames"
			);
		}
		const double midTime = Sampled_indexToX (output, iframe);
		integer soundFrameBegin = Sampled_xToNearestIndex (input, midTime - 0.5 * physicalAnalysisWidth);   // approximation

		for (integer isample = 1; isample <= soundFrame.size; isample ++, soundFrameBegin ++)
			soundFrame [isample] = ( soundFrameBegin > 0 && soundFrameBegin <= input -> nx ? input -> z [1] [soundFrameBegin] : 0.0 );
		if (subtractFrameMean)
			centre_VEC_inout (soundFrame, nullptr);
		//const double soundFrameExtremum = NUMextremum_u (soundFrame);   // not used
		soundFrame  *=  windowFunction.get();

		const integer numberOfChannels = frameAsSound -> ny;
		if (numberOfChannels == 1)
			fourierSamples.part (1, soundFrameSize)  <<=  frameAsSound -> z.row (1);
		else {
			/*
				Multiple channels: take the average.
			*/
			for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
				fourierSamples.part (1, soundFrameSize)  +=  frameAsSound -> z.row (ichan);
			fourierSamples.part (1, soundFrameSize)  *=  1.0 / numberOfChannels;
		}
		fourierSamples.part (soundFrameSize + 1, numberOfFourierSamples)  <<=  0.0;
		NUMfft_forward (fourierTable.get(), fourierSamples.get());

		const VEC re = spectrum -> z.row (1);
		const VEC im = spectrum -> z.row (2);
		const integer numberOfFrequencies = spectrum -> nx;
		const double scaling = output -> dx;
		re [1] = fourierSamples [1] * scaling;
		im [1] = 0.0;
		for (integer i = 2; i < numberOfFrequencies; i ++) {
			re [i] = fourierSamples [i + i - 2] * scaling;   // fourierSamples [2], [4], ...
			im [i] = fourierSamples [i + i - 1] * scaling;   // fourierSamples [3], [5], ...
		}
		if ((numberOfFourierSamples & 1) != 0) {
			if (numberOfFourierSamples > 1) {
				re [numberOfFrequencies] = fourierSamples [numberOfFourierSamples - 1] * scaling;
				im [numberOfFrequencies] = fourierSamples [numberOfFourierSamples] * scaling;
			}
		} else {
			re [numberOfFrequencies] = fourierSamples [numberOfFourierSamples] * scaling;
			im [numberOfFrequencies] = 0.0;
		}

		/*
			Step 1: spectrum of the sound frame
			a. soundFrameToForwardFourierTransform ()
			b. scaling
		*/

		for (integer i = 1 ; i <= numberOfFourierSamples; i ++)
			fourierSamples [i] *= frameAsSound -> dx;

		/*
			step 2: log of the spectrum power values log (re * re + im * im)
		*/
		fourierSamples [1] = log (fourierSamples [1] * fourierSamples [1] + 1e-300);
		for (integer i = 1; i < numberOfFourierSamples / 2; i ++) {
			const double re = fourierSamples [2 * i], im = fourierSamples [2 * i + 1];
			fourierSamples [2 * i] = log (re * re + im * im + 1e-300);
			fourierSamples [2 * i + 1] = 0.0;
		}
		fourierSamples [numberOfFourierSamples] = log (fourierSamples [numberOfFourierSamples] * fourierSamples [numberOfFourierSamples] + 1e-300);
		/*
			Step 3: inverse fft of the log spectrum
		*/
		NUMfft_backward (fourierTable.get(), fourierSamples.get());
		const double df = 1.0 / (frameAsSound -> dx * numberOfFourierSamples);
		for (integer i = 1; i <= powerCepstrum -> nx; i ++) {
			const double val = fourierSamples [i] * df;
			powerCepstrum -> z [1] [i] = val * val;
		}

		output -> z.column (iframe)  <<=  powerCepstrum -> z.row (1);
	} MelderThread_ENDFOR
}

static autoPowerCepstrogram Sound_to_PowerCepstrogram_new (Sound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency) {
	try {
		const kSound_windowShape windowShape = kSound_windowShape::GAUSSIAN_2;
		const double effectiveAnalysisWidth = 3.0 / pitchFloor; // minimum analysis window has 3 periods of lowest pitch
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth2 (effectiveAnalysisWidth, windowShape);
		const double physicalSoundDuration = my dx * my nx;
		volatile const double windowDuration = Melder_clippedRight (physicalAnalysisWidth, physicalSoundDuration);
		Melder_require (physicalSoundDuration >= physicalAnalysisWidth,
			U"Your sound is too short:\n"
			U"it should be longer than ", physicalAnalysisWidth, U" s."
		);
		const double samplingFrequency = 2.0 * maximumFrequency;
		autoSound input = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50_integer, preEmphasisFrequency);
		double t1;
		integer nFrames;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & nFrames, & t1);
		const integer soundFrameSize = getSoundFrameSize2 (physicalAnalysisWidth, input -> dx);
		const integer nfft = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (soundFrameSize));
		const integer nq = nfft / 2 + 1;
		const double qmax = 0.5 * nfft / samplingFrequency, dq = 1.0 / samplingFrequency;
		autoPowerCepstrogram output = PowerCepstrogram_create (my xmin, my xmax, nFrames, dt, t1, 0, qmax, nq, dq, 0);
		Sound_into_PowerCepstrogram (input.get(), output.get(), effectiveAnalysisWidth, windowShape);
		return output;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrogram created.");
	}
}

static autoPowerCepstrogram Sound_to_PowerCepstrogram_old (Sound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency) {
	try {
		const double analysisWidth = 3.0 / pitchFloor; // minimum analysis window has 3 periods of lowest pitch
		const double physicalAnalysisWidth = 2.0 * analysisWidth;
		const double physicalSoundDuration = my dx * my nx;
		volatile const double windowDuration = Melder_clippedRight (2.0 * analysisWidth, my dx * my nx);   // gaussian window
		Melder_require (physicalSoundDuration >= physicalAnalysisWidth,
			U"Your sound is too short:\n"
			U"it should be longer than 6.0 / pitchFloor (", physicalAnalysisWidth, U" s)."
		);
		// Convenience: analyse the whole sound into one Cepstrogram_frame
		const double samplingFrequency = 2.0 * maximumFrequency;
		autoSound sound = Sound_resample (me, samplingFrequency, 50);
		Sound_preEmphasize_inplace (sound.get(), preEmphasisFrequency);
		double t1;
		integer nFrames;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & nFrames, & t1);
		autoSound sframe = Sound_createSimple (1_integer, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		/*
			Find out the size of the FFT
		*/
		const integer nfft = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (sframe -> nx));   // TODO: explain edge case
		const integer nq = nfft / 2 + 1;
		const double qmax = 0.5 * nfft / samplingFrequency, dq = 1.0 / samplingFrequency;
		autoPowerCepstrogram thee = PowerCepstrogram_create (my xmin, my xmax, nFrames, dt, t1, 0, qmax, nq, dq, 0);

		autoMelderProgress progress (U"Cepstrogram analysis");

		for (integer iframe = 1; iframe <= nFrames; iframe++) {
			const double t = Sampled_indexToX (thee.get(), iframe); // TODO express the following 3 lines more clearly
			Sound_into_Sound (sound.get(), sframe.get(), t - windowDuration / 2);
			Vector_subtractMean (sframe.get());
			Sounds_multiply (sframe.get(), window.get());
			autoSpectrum spec = Sound_to_Spectrum (sframe.get(), true);   // FFT yes
			autoPowerCepstrum cepstrum = Spectrum_to_PowerCepstrum (spec.get());
			for (integer i = 1; i <= nq; i ++)
				thy z [i] [iframe] = cepstrum -> z [1] [i];

			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / nFrames, U"PowerCepstrogram analysis of frame ",
						iframe, U" out of ", nFrames, U".");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrogram created.");
	}
}

autoPowerCepstrogram Sound_to_PowerCepstrogram (Sound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency) {
	autoPowerCepstrogram result;
	if (Melder_debug == -10)
		result = Sound_to_PowerCepstrogram_old (me, pitchFloor, dt, maximumFrequency, preEmphasisFrequency);
	else
		result = Sound_to_PowerCepstrogram_new (me, pitchFloor, dt, maximumFrequency, preEmphasisFrequency);
	return result;
}

/* End of file Sound_to_PowerCepstrogram.cpp */
