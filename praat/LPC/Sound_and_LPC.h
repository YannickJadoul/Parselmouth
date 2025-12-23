#ifndef _Sound_and_LPC2_h_
#define _Sound_and_LPC2_h_
/* Sound_and_LPC.h
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

/*
 djmw 19971103
 djmw 20020812 GPL header
*/

#include "SoundFrameIntoSampledFrame.h"
#include "Sound.h"
#include "LPC.h"
#include "SVD.h"

/*
	20240603:
	The output of the Sound_to_LPC_<x> might be a little bit different from previous outputs because:
	1. The sound frame now always has an odd number of samples, irrespective of the sampling frequency or the window shape.
		This means that also the window function will have an odd number of samples. Therefore the sample at the centre of the sound frame always has weight 1.0.
		Previously the number of samples could be even or odd, depending on how rounding turned out.
	2. The Gaussian window function was slightly improved.
	3. The precision of the autocorrelation and covariance method have been improved a little by using some `longdouble` accumulators.
*/

Thing_define (SoundFrameIntoLPCFrame, SoundFrameIntoSampledFrame) {
	
	mutableLPC outputLPC;
	integer order;
	integer currentOrder;  // TODO djmw 20250825 is this one necessary 
	integer orderp1; 	// convenience order+1
	autoVEC a;			// common work vector of dimension orderp1

	virtual void initBasicSoundFrameIntoLPCFrame (constSound input, mutableLPC outLPC, double effectiveAnalysisWidth, kSound_windowShape windowShape);
	
	void copyBasic (constSampledFrameIntoSampledFrame other)
		override;

	void initHeap ()
		override;
};

/*********************** Autocorrelation method *************************************************************/

/*
	Precondition:
	Sound and LPC have the same sampling.
*/
void Sound_into_LPC_auto (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth);

autoLPC Sound_to_LPC_auto (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency);

/*********************** Covariance method *************************************************************/

/*
	Precondition:
	Sound and LPC have the same sampling.
*/
void Sound_into_LPC_covar (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth);

autoLPC Sound_to_LPC_covar (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency);

/*********************** Burg method *******************************************************/

/*
	Precondition:
	Sound and LPC have the same sampling.
*/
void Sound_into_LPC_burg (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth);

autoLPC Sound_to_LPC_burg (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency);

void soundFrameIntoLPCFrame_burg (VEC soundFrame, LPC_Frame lpcFrame, VEC b1, VEC b2, 
	VEC aa, integer &info);

/*********************** Marple method *****************************************************/

/*
	Precondition:
	Sound and LPC have the same sampling.
*/
void Sound_into_LPC_marple (constSound me, mutableLPC thee, double analysisWidth, double tol1, double tol2);

autoLPC Sound_to_LPC_marple (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt,
	double preEmphasisFrequency, double tol1, double tol2);

/*********************** PLP (Hermansky) method ******************************************************/

Thing_define (SoundFrameIntoLPCFramePLP, SoundFrameIntoLPCFrame) {

	integer numberOfCriticalBandFilters;
	autoVEC equalLoudnessPreemphasis;	
	
	void initBasicSoundFrameIntoLPCFramePLP (constSound inputSound, mutableLPC outputLPC, double effectiveAnalysisWidth,
		kSound_windowShape windowShape);

	void copyBasic (constSampledFrameIntoSampledFrame other)
		override;

	void initHeap ()
		override;

	bool inputFrameIntoOutputFrame (integer iframe)
		override;
		
	void getFilterCharacteristics ();

};

/*********************** Robust method (LPC & Sound) *******************************************************/

Thing_define (RobustLPCWorkspace, Thing) {
	
	constLPC inputLPC;
	mutableLPC outputLPC;
	VEC soundFrame;
	integer order, currentPredictionOrder;
	double k_stdev;
	integer iter;
	integer itermax;
	integer huber_iterations = 5; // = 5;
	bool wantLocation = true;	//
	bool wantScale = true;	//
	double location = 0.0;	// not a parameter in initBasic!
	double scale;
	double tol1;
	double tolSVD = 1e-10;	// not a parameter in initBasic!
	autoVEC error; 			// soundFrameSize
	autoVEC sampleWeights;	// soundFrameSize
	autoVEC coefficients;	// inputLPC -> maxnCoefficients
	autoVEC covariancesw;	// inputLPC -> maxnCoefficients
	autoMAT covarmatrixw;	// inputLPC -> maxnCoefficients, inputLPC -> maxnCoefficients
	integer computedSVDworksize;
	autoSVD svd;
	autoVEC svdwork1;		// computedSVDworksize
	autoVEC svdwork2;		// order
	autoVEC filterMemory;	// order
	autoVEC huberwork;		// soundFrameSize)

	void init (constLPC inputLPC, constSound inputSound, mutableLPC outputLPC, double effectiveAnalysisWidth,
		kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, bool wantlocation);

	void resize ();
	void setSampleWeights ();
	void setCovariances ();
	void solvelpc ();
	void inputFrameIntoOutputFrame (LPC_Frame inputLPCFrame, LPC_Frame outputLPCFrame,
		VEC soundFrame, integer& info);
};

autoRobustLPCWorkspace RobustLPCWorkspace_create (constLPC inputLPC, constSound inputSound,
	mutableLPC outputLPC, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double k_stdev, integer itermax, double tol, bool wantlocation);

/*
	Precondition:
	Sound and LPC have the same sampling.
*/
void LPC_and_Sound_into_LPC_robust (constLPC inputLPC, constSound inputSound, mutableLPC outputLPC,
	double effectiveAnalysisWidth, double k_stdev, integer itermax, double tol, bool wantlocation);

/*
	Precondition:
	Sound and LPC have the same sampling.
*/
autoLPC LPC_and_Sound_to_LPC_robust (constLPC inputLPC, constSound inputSound, double effectiveAnalysisWidth,
	double preEmphasisFrequency, double k_stdev, integer itermax, double tol, bool wantlocation);

void soundFrameAndLPCFrameIntoLPCFrame (VEC soundFrame, LPC_Frame inputLPCFrame, LPC_Frame outputLPCFrame,
	RobustLPCWorkspace ws, integer& info);

/*********************** Robust method (Sound) *************************************************************/

void Sound_into_LPC_robust (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth,
	double k_stdev, integer itermax, double tol, bool wantlocation);

autoLPC Sound_to_LPC_robust (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt,
	double preEmphasisFrequency, double k_stdev, integer itermax, double tol, bool wantlocation);

/*
	Precondition:
		Sound has been resampled and pre-emphasized
*/

/*
 * Function:
 *	Calculate linear prediction coefficients according to following model:
 *  Minimize E(m) = Sum(n=n0;n=n1; (x [n] + Sum(k=1;k=m; a [k]*x [n-k])))
 * Method:
 *  The minimization is carried out by solving the equations:
 *  Sum(i=1;i=m; a [i]*c [i] [k]) = -c [0] [k] for k=1,2,...,m
 *  where c [i] [k] = Sum(n=n0;n=n1;x [n-i]*x [n-k])
 *  1. Covariance:
 *		n0=m; n1 = N-1;
 *      c [i] [k] is symmetric, positive semi-definite matrix
 *  	Markel&Gray, LP of Speech, page 221;
 *  2. Autocorrelation
 *		signal is zero outside the interval;
 *      n0=-infinity; n1=infinity
 *      c [i] [k] symmetric, positive definite Toeplitz matrix
 *  	Markel&Gray, LP of Speech, page 219;
 * Preconditions:
 *	predictionOrder > 0;
 *  preEmphasisFrequency >= 0;
 *
 * Burg method: see Numerical recipes Chapter 13.
 *
 * Marple method: see Marple, L. (1980), A new autoregressive spectrum analysis
 *		algorithm, IEEE Trans. on ASSP 28, 441-453.
 *	tol1 : stop iteration when E(m) / E(0) < tol1
 *	tol2 : stop iteration when (E(m)-E(m-1)) / E(m-1) < tol2,
 */

autoSound LPC_Sound_filter (constLPC me, constSound thee, bool useGain);
/*
	E(z) = X(z)A(z),
	A(z) = 1 + Sum (k=1, k=m, a(k)z^-k);

	filter:
		given e & a, determine x;
		x(n) = e(n) - Sum (k=1, m, a(k)x(n-k))
	useGain determines whether the LPC-gain is used in the synthesis.
*/

void LPC_Sound_filterWithFilterAtTime_inplace (constLPC me, mutableSound thee, integer channel, double time);

autoSound LPC_Sound_filterWithFilterAtTime (constLPC me, constSound thee, integer channel, double time);

autoSound LPC_Sound_filterInverse (constLPC me, constSound thee);
/*
	E(z) = X(z)A(z),
	A(z) = 1 + Sum (k=1, k=m, a(k)z^-k);

	filter inverse:
		given x & a, determine e;
		e(n) = x(n) + Sum (k=1, m, a(k)x(n-k))
*/

autoSound LPC_Sound_filterInverseWithFilterAtTime (constLPC me, constSound thee, integer channel, double time);

void LPC_Sound_filterInverseWithFilterAtTime_inplace (constLPC me, mutableSound thee, integer channel, double time);

/*
	For all LPC analysis
*/
void checkLPCAnalysisParameters_e (double sound_dx, integer sound_nx, double physicalAnalysisWidth, integer predictionOrder);

inline void Sound_and_LPC_require_equalDomainsAndSamplingPeriods (constSound me, constLPC thee) {
	Melder_require (my xmin == thy xmin && thy xmax == my xmax,
		U"The domains of the Sound and the LPC should be equal.");
	Melder_require (my dx == thy samplingPeriod,
		U"The sampling periods of the Sound and the LPC should be equal.");
}

#endif /* _Sound_and_LPC_h_ */
