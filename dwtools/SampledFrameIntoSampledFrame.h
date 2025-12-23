#ifndef _SampledFrameIntoSampledFrame_h_
#define _SampledFrameIntoSampledFrame_h_
/* SampledFrameIntoSampledFrame.h
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

#include "Data.h"
#include "Sampled.h"

Thing_define (SampledFrameIntoSampledFrame, Thing) {
	
	constSampled input;
	mutableSampled output;
	integer frameAnalysisInfo = 0;

	/*
		Step 1 to partially initialize a new object:
		Supply the object with enough data such that the second step, initHeap(), can
		complete the object initialization.
	*/
	virtual void initBasic (constSampled initialInput, mutableSampled initialOutput) {
		our input = initialInput;
		our output = initialOutput;
	}
	
	/*
		Step 2 to completely initialize a new object:
		Allocate all the heap structures like autovectors that can be dimensioned only after the
		basic initialization in step 1 has been done.
	*/
	virtual void initHeap () { }

	virtual void copyBasic (constSampledFrameIntoSampledFrame other) {
		our input = other -> input;
		our output = other -> output;
	}
	
	virtual void getInputFrame (integer /* iframe */) { }

	virtual bool inputFrameIntoOutputFrame (integer /* iframe */) { // the actual analysis
		return true;
	}
	
	virtual void saveOutputFrame (integer /* iframe */) { }
};

#endif /* _SampledFrameIntoSampledFrame_h_ */
