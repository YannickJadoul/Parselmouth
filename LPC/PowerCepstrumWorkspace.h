#ifndef _PowerCepstrumWorkspace_h_
#define _PowerCepstrumWorkspace_h_
/* PowerCepstrumWorkspace_def.h
 *
 * Copyright (C) 2025 David Weenink
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

#include "PowerCepstrum.h"
#include "SlopeSelector.h"

Thing_define (PowerCepstrumWorkspace, Thing) {

	/*
		To do some of the calculations efficiently we use a Workspace to maintain state.
		The following data will never be saved or copied when a PowerCepstrum is saved.
	*/
	constPowerCepstrum powerCepstrum;
	integer numberOfPoints = 0;
	integer imin, imax;			// [imin,imax]  the slope calculation interval
	autoVEC x, y; 				// numberOfPoints (in dB's)
	autoMatrix asdBs;			// for peak detection
	autoSlopeSelector slopeSelector;
	bool slopeKnown = false, peakKnown = false, trendSubtracted = false;
	kSlopeSelector_method method;
	kCepstrum_trendType trendLineType;
	kVector_peakInterpolation peakInterpolationType = kVector_peakInterpolation :: PARABOLIC;
	double qminPeakSearch, qmaxPeakSearch; // peak in [pitchFloor, pitchCeiling]
	double slope, intercept;
	double cpp; 				// = peakdB - trenddB
	double trenddB, peakdB;
	double peakQuefrency;
	integer maximumNumberOfRhamonics, numberOfRhamonics;
	autoMAT rhamonics; 			// maximumNumberOfRhamonics x 5: power, q1, q, q2, amplitude
	
	void initWorkspace (constPowerCepstrum me, double qminFit, double qmaxFit, kCepstrum_trendType trendLineType, kCepstrum_trendFit method);
	
	void initPeakSearchPart (double qminPeakSearch,	double qmaxPeakSearch, kVector_peakInterpolation peakInterpolationType);
	
	void newData (constPowerCepstrum thee);

	void getSlopeAndIntercept ();

	void getPeakAndPosition ();

	void subtractTrend ();

	double getTrend (double quefrency);

	void getCPP ();

	void todBs ();
	
	void fromdBs ();
	
	void setMaximumNumberOfRhamonics (integer maximumNumberOfRhamonics);
	
	void getNumberOfRhamonics (double qmin, double qmax);
	
	void getRhamonicPeaks (double qmin, double qmax);
	
	void getRhamonicsPower (double qmin, double qmax, double f0fractionalWidth);
			
	double getRNR (double qmin, double qmax, double f0fractionalWidth);
	

};

autoPowerCepstrumWorkspace PowerCepstrumWorkspace_create (constPowerCepstrum thee, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method);

/*void PowerCepstrum_initWorkspace (PowerCepstrum me, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method);


void PowerCepstrumWorkspace_init (mutablePowerCepstrumWorkspace me, constPowerCepstrum thee, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method);

autoPowerCepstrumWorkspace PowerCepstrumWorkspace_create (constPowerCepstrum thee, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method);

void PowerCepstrumWorkspace_initPeakSearchPart (mutablePowerCepstrumWorkspace me, double qminPeakSearch,
	double qmaxPeakSearch, kVector_peakInterpolation peakInterpolationType);
*/

#endif /* PowerCepstrumWorkspace_h_ */
