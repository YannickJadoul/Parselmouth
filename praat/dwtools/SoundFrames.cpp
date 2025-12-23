/* SoundFrames.cpp
 *
 * Copyright (C) 2025 David Weenink
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

#include "SoundFrames.h"

Thing_implement (SoundFrames, Thing, 0);

void structSoundFrames :: init (constSound input, double effectiveAnalysisWidth, double timeStep, 
	kSound_windowShape windowShape, bool subtractFrameMean)
{
	our inputSound = input;
	our physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	if (timeStep == 0.0) {
		// calculate output_dt
	}
	our dt = timeStep;
	Sampled_shortTermAnalysis (inputSound, physicalAnalysisWidth, dt, & our numberOfFrames, & our t1);
	initCommon (windowShape, subtractFrameMean);
}
	
void structSoundFrames :: initWithSampled (constSound input, constSampled output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, bool subtractFrameMean)
{
	Melder_require (input -> xmin == output -> xmin && input -> xmax == output -> xmax,
		U"The domains of Sound ", input, U" and Sampled ", output , U" should be equal.");
	our inputSound = input;
	our t1 = output -> x1;
	our numberOfFrames = output -> nx;
	our dt = output -> dx;
	our physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);	
	initCommon (windowShape, subtractFrameMean);
}

void structSoundFrames :: initCommon (kSound_windowShape windowShape, bool subtractFrameMean)
{
	our windowShape = windowShape;
	our subtractFrameMean = subtractFrameMean;
	soundFrameSize = getSoundFrameSize (physicalAnalysisWidth, inputSound -> dx);
	windowFunction = raw_VEC (soundFrameSize);   // TODO: move out of thread repetition
	windowShape_into_VEC (windowShape, windowFunction.get());
	frameAsSound = Sound_create (1_integer, 0.0, soundFrameSize * inputSound -> dx, soundFrameSize,
		inputSound -> dx, 0.5 * inputSound -> dx);
	soundFrame = frameAsSound -> z.row (1);
	Melder_assert (soundFrame.size == soundFrameSize);
}

VEC structSoundFrames :: getFrame (integer iframe) {
	const double midTime = t1 + (iframe - 1) * dt;
	integer soundFrameBegin = Sampled_xToNearestIndex (inputSound, midTime - 0.5 * physicalAnalysisWidth);   // approximation
	for (integer isample = 1; isample <= soundFrame.size; isample ++, soundFrameBegin ++) {
		double sample = 0.0;
		if (soundFrameBegin > 0 && soundFrameBegin <= inputSound -> nx) {
			for (integer ichannel = 1; ichannel <= inputSound -> ny; ichannel ++)
				sample += inputSound -> z [ichannel] [soundFrameBegin];
			sample /= inputSound -> ny;
		}
		soundFrame [isample] = sample;
	}
	if (subtractFrameMean)
		centre_VEC_inout (soundFrame, nullptr);
	soundFrameExtremum = NUMextremum_u (soundFrame);
	soundFrame  *=  windowFunction.get();
	return soundFrame;
}

autoSoundFrames SoundFrames_createWithSampled (constSound input, constSampled output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, bool subtractFrameMean)
{
	try {
		autoSoundFrames me = Thing_new (SoundFrames);
		my initWithSampled (input, output, effectiveAnalysisWidth, windowShape, subtractFrameMean);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundFrames (with Sampled) could not be created.");
	}
}

/* End of file SoundFrames.cpp */
