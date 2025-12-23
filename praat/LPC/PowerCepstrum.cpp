/* PowerCepstrum.cpp
 *
 * Copyright (C) 2012-2025 David Weenink
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

#include "PowerCepstrum.h"
#include "PowerCepstrumWorkspace.h"
#include "NUM2.h"

Thing_implement (PowerCepstrum, Cepstrum, 2);   // derives from Matrix; therefore also version 2

Thing_implement (PowerCepstrumWorkspace, Thing, 0);


static inline bool greaterThanOrEqual (double x, double y) {
	// x >= y means !(x < y) 
	return ( (x == 0.0 || y == 0.0) ? (x - y) : (x - y) / fabs (x) ) > -1e-12;
}

static inline bool greaterThan (double x, double y) {
	return ( (x == 0.0 || y == 0.0) ? (x - y) : (x - y) / fabs (x) ) > 1e-12;
}

static void Vector_getMaximumAndX_twoSideApproach (constVector me, double xmin, double xmax, integer channelNumber, 
	kVector_peakInterpolation peakInterpolationType, bool startHigh, double *out_maximum, double *out_xOfMaximum)
{
	Melder_assert (channelNumber >= 1 && channelNumber <= my ny);
	constVEC y = my z.row (channelNumber);
	double maximum, x;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No samples between xmin and xmax.
			Try to return the greater of the values at these two points.
		*/
		kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
				kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
		const double yleft = Vector_getValueAtX (me, xmin, channelNumber, valueInterpolationType);
		const double yright = Vector_getValueAtX (me, xmax, channelNumber, valueInterpolationType);
		maximum = std::max (yleft, yright);
		x = ( yleft == yright ? (xmin + xmax) / 2.0 : yleft > yright ? xmin : xmax );
	} else  {
		maximum = y [imin];
		x = imin;
		if (y [imax] > maximum) {
			maximum = y [imax];
			x = imax;
		}
		if (imin == 1)
			imin ++;
		if (imax == my nx)
			imax --;
		if (! startHigh) { // approach from the start
			for (integer i = imin; i <= imax; i ++) {
				if (greaterThan (y [i], y [i - 1]) && greaterThanOrEqual (y [i], y [i + 1])) {
					double i_real;
					const double localMaximum = NUMimproveMaximum (y, i, kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType), & i_real);
					if (localMaximum > maximum) {
						maximum = localMaximum;
						x = i_real;
					}
				}
			}
		} else { // approach from the end
			for (integer i = imax; i >= imin; i --) {
				if (greaterThan (y [i], y [i + 1]) && greaterThanOrEqual (y [i], y [i - 1])) {
					double i_real;
					const double localMaximum = NUMimproveMaximum (y, i, kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType), & i_real);
					if (localMaximum > maximum) {
						maximum = localMaximum;
						x = i_real;
					}
				}
			}
		}
		x = my x1 + (x - 1) * my dx;   // convert sample to x
		Melder_clip (xmin, & x, xmax);
		
	}
	if (out_maximum)
		*out_maximum = maximum;
	if (out_xOfMaximum)
		*out_xOfMaximum = x;
}

void structPowerCepstrumWorkspace :: getSlopeAndIntercept () {
	SlopeSelector_getSlopeAndIntercept (slopeSelector.get(), slope, intercept, method);
	slopeKnown = true;
}

double structPowerCepstrumWorkspace :: getTrend (double quefrency) {
	if (! slopeKnown)
		getSlopeAndIntercept ();
	const double xq = ( trendLineType == kCepstrum_trendType::EXPONENTIAL_DECAY ? log (quefrency) : quefrency );
	trenddB = slope * xq + intercept;
	return trenddB;
}

void structPowerCepstrumWorkspace :: subtractTrend () {
	if (! slopeKnown)
		getSlopeAndIntercept ();
	for (integer j = 1; j <= powerCepstrum -> nx; j ++) {
		/*
			For the exponential decay function, y = slope*log(quefrency)+intercept, the PowerCepstrum's first quefrency
			value (at x1) has quefrency == 0 and therefore value y(0) is not defined. As an approximation for y(0) 
			we use y(0.5*dx). This is no problem because the value at quefrency == 0 is not relevant.
		*/
		const double quefrency = ( j == 1 && trendLineType == kCepstrum_trendType::EXPONENTIAL_DECAY ? 0.5 * powerCepstrum -> dx : (j - 1) * powerCepstrum -> dx );
		const double db_background = getTrend (quefrency);
		const double db_cepstrum = powerCepstrum -> v_getValueAtSample (j, 1, 1);
		const double diff = Melder_clippedLeft (0.0, db_cepstrum - db_background);
		powerCepstrum -> z [1] [j] = exp (diff * NUMln10 / 10.0);
	}
	trendSubtracted = true;
}

void structPowerCepstrumWorkspace :: newData (constPowerCepstrum thee) {
	Melder_assert (thy nx == powerCepstrum -> nx);
	for (integer ipoint = 1, i = imin; i <= imax; i ++, ipoint ++) {
		double xval = Sampled_indexToX (thee, i);
		if (trendLineType == kCepstrum_trendType::EXPONENTIAL_DECAY)
			xval = log (xval);
		x [ipoint] = xval;
		y [ipoint] = thy v_getValueAtSample (i, 1, 1); // dB's
	}
	slopeSelector -> newDataPoints (x.get(), y.get());
	todBs ();
	peakKnown = slopeKnown = trendSubtracted = false;
}

void structPowerCepstrumWorkspace :: getPeakAndPosition () {
	Matrix thee = asdBs.get();
	double peakdBR, peakQuefrencyR;
	Vector_getMaximumAndX_twoSideApproach ((Vector) thee, qminPeakSearch, qmaxPeakSearch,
		1, peakInterpolationType, false, & peakdB, & peakQuefrency);
	Vector_getMaximumAndX_twoSideApproach ((Vector) thee, qminPeakSearch, qmaxPeakSearch,
		1, peakInterpolationType, true, & peakdBR, & peakQuefrencyR);
	const integer index = Sampled_xToIndex (thee, peakQuefrency), indexR = Sampled_xToIndex(thee, peakQuefrencyR);
	if (index != indexR && (indexR - index) <= 5) {
		double indexm = 0.5 * (index + indexR);
		peakQuefrency = thy x1 + (indexm - 1) * thy dx;
		peakdB = thy z [1] [index]; // always with flat peak
	}
	peakKnown = true;
}

void structPowerCepstrumWorkspace :: getCPP () {
	if (! slopeKnown)
		getSlopeAndIntercept ();
	if (! peakKnown)
		getPeakAndPosition ();
	trenddB = getTrend (peakQuefrency);
	cpp = peakdB - trenddB;
}

void structPowerCepstrumWorkspace :: todBs () {
	for (integer i = 1; i <= powerCepstrum -> nx; i ++)
		asdBs -> z [1] [i] = powerCepstrum -> v_getValueAtSample (i, 1, 1); // 10 log val^2
}

void structPowerCepstrumWorkspace :: fromdBs () {
	for (integer i = 1; i <= powerCepstrum -> nx; i ++)
		powerCepstrum -> z [1] [i] = pow (10.0, powerCepstrum -> z [1] [i] / 10.0) - 1e-30;
}

void structPowerCepstrumWorkspace :: setMaximumNumberOfRhamonics (integer maximumNumberOfRhamonics) {
	our maximumNumberOfRhamonics = maximumNumberOfRhamonics;
	// resize when we calculate values.
}

void structPowerCepstrumWorkspace :: getNumberOfRhamonics (double qmin, double qmax) {
		integer numberOfRhamonics = 2;
		if (! peakKnown)
			getPeakAndPosition ();
		while (peakQuefrency >= qmin && peakQuefrency * numberOfRhamonics <= qmax)
			numberOfRhamonics ++;
		numberOfRhamonics --;
		numberOfRhamonics = std:: min (numberOfRhamonics, maximumNumberOfRhamonics);
}

void structPowerCepstrumWorkspace :: getRhamonicsPower (double qmin, double qmax, double f0fractionalWidth) {
	getSlopeAndIntercept ();
	getPeakAndPosition ();
	getNumberOfRhamonics (qmin, qmax);
	rhamonics.resize (numberOfRhamonics, 6_integer);
	// q, peakdB, power, q1, q2, trenddb
	for (integer rhamonic = 1; rhamonic <= numberOfRhamonics; rhamonic ++) {
		const double quefrency = rhamonic * peakQuefrency;
		const double f = 1.0 / quefrency;
		const double flow = f * (1.0 - f0fractionalWidth);
		const double fhigh = f * (1.0 + f0fractionalWidth);
		const double qlow =  1.0 / fhigh;
		const double qhigh = ( f0fractionalWidth >= 1.0 ? powerCepstrum -> xmax : 1.0 / flow );
		integer iqmin, iqmax;
		Matrix_getWindowSamplesX (powerCepstrum, qlow, qhigh, & iqmin, & iqmax);
		double power = 0.0;
		for (integer iq = iqmin; iq <= iqmax; iq ++)
			power += powerCepstrum -> v_getValueAtSample (iq, 1, 0);
		rhamonics [rhamonic] [1] = power;
		rhamonics [rhamonic] [2] = qlow;
		rhamonics [rhamonic] [3] = quefrency;
		rhamonics [rhamonic] [4] = qhigh;
		rhamonics [rhamonic] [5] = peakdB;
	}
}

void structPowerCepstrumWorkspace :: getRhamonicPeaks (double qmin, double qmax) {
	getPeakAndPosition ();
	getNumberOfRhamonics (qmin, qmax);
	rhamonics.resize (numberOfRhamonics, 6_integer);
	rhamonics [1] [2] = peakQuefrency;
	rhamonics [1] [1] = peakdB;
	kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
			kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
	for (integer rhamonic = 2; rhamonic <= numberOfRhamonics; rhamonic ++) {
		const double quefrency = rhamonic * peakQuefrency;
		const double amplitudedB = Vector_getValueAtX ((Vector) asdBs.get(), quefrency, 1, valueInterpolationType);
		rhamonics [rhamonic] [2] = quefrency;
		rhamonics [rhamonic] [1] = amplitudedB;	
	}
}

double structPowerCepstrumWorkspace :: getRNR (double qmin, double qmax, double f0fractionalWidth) {
	getRhamonicsPower (qmin, qmax, f0fractionalWidth);
	const double sumOfRhamonics = NUMsum (rhamonics.column (3));
	double power = 0.0;
	for (integer iq = imin; iq <= imax; iq ++)
		power += powerCepstrum -> v_getValueAtSample (iq, 1, 0);
	const double rnr = sumOfRhamonics / (power - sumOfRhamonics);
	return rnr;
}

double structPowerCepstrum :: v_getValueAtSample (integer isamp, integer row, int units) const {
	double result = undefined;
	if (row == 1) {
		if (units == 0)
			result = z [1] [isamp];
		else
			result = 10.0 * log10 (z [1] [isamp] + 1e-30); // result >= -300
	}
	return result;
}

void structPowerCepstrumWorkspace :: initWorkspace (constPowerCepstrum me, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method) 
{
	powerCepstrum = me;
	Function_unidirectionalAutowindow (powerCepstrum, & qminFit, & qmaxFit);
	Melder_require (qminFit >= powerCepstrum ->xmin && qmaxFit <= powerCepstrum ->xmax,
		U"Your quefrency range is outside the domain.");
	(void) Matrix_getWindowSamplesX (powerCepstrum, qminFit, qmaxFit, & imin, & imax);
	Melder_clipLeft (2_integer, & imin); // never use q=0 in fitting
	numberOfPoints = imax - imin + 1;
	Melder_require (numberOfPoints > 2,
		U"Not enough points in the selection.");
	our trendLineType = trendLineType;
	our method = ( method == kCepstrum_trendFit::LEAST_SQUARES ? kSlopeSelector_method::LEAST_SQUARES :
		(method == kCepstrum_trendFit::ROBUST_SLOW ? kSlopeSelector_method::THEILSEN : kSlopeSelector_method::SIEGEL));
	x = raw_VEC (numberOfPoints);
	y = raw_VEC (numberOfPoints);
	asdBs = Matrix_create (powerCepstrum -> xmin, powerCepstrum -> xmax, powerCepstrum -> nx, powerCepstrum -> dx, powerCepstrum -> x1,
		powerCepstrum -> ymin, powerCepstrum -> ymax, powerCepstrum -> ny, powerCepstrum -> dy, powerCepstrum ->y1);
	maximumNumberOfRhamonics = 15;
	rhamonics = raw_MAT (maximumNumberOfRhamonics, 5_integer);
	slopeSelector = SlopeSelector_create (x.get(), y.get()); // only reference to the x and y vectors
	newData (powerCepstrum); // new xp and yp reference
}

void structPowerCepstrumWorkspace :: initPeakSearchPart (double qminPeakSearch, double qmaxPeakSearch,
	kVector_peakInterpolation peakInterpolationType)
{
	our qminPeakSearch = qminPeakSearch;
	our qmaxPeakSearch = qmaxPeakSearch;
	our peakInterpolationType = peakInterpolationType;
}

autoPowerCepstrumWorkspace PowerCepstrumWorkspace_create (constPowerCepstrum powerCepstrum, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod)
{
	try {
		autoPowerCepstrumWorkspace me = Thing_new (PowerCepstrumWorkspace);
		my initWorkspace (powerCepstrum, qminFit, qmaxFit,	trendLineType, fitMethod);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create PowerCepstrumWorkspace.");
	}
	
}

autoPowerCepstrum PowerCepstrum_create (double qmax, integer nq) {
	try {
		autoPowerCepstrum me = Thing_new (PowerCepstrum);
		const double dq = qmax / (nq - 1);
		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PowerCepstrum not created.");
	}
}


void PowerCepstrum_draw (constPowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish) {
	Cepstrum_draw (me, g, qmin, qmax, dBminimum, dBmaximum, true, garnish);
}

void PowerCepstrum_fitTrendLine (PowerCepstrum me, double qstartFit, double qendFit, double *out_slope, double *out_intercept, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qstartFit, qendFit, lineType, fitMethod);
	workspace -> getSlopeAndIntercept ();
	if (out_slope) 
		*out_slope = workspace -> slope;
	if (out_intercept)
		*out_intercept = workspace -> intercept;
}

void PowerCepstrum_drawTrendLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum,
	double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	Function_unidirectionalAutowindow (me, & qmin, & qmax);
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qstartFit, qendFit, lineType, fitMethod);
	workspace -> getSlopeAndIntercept ();

	if (dBminimum >= dBmaximum) {   // autoscaling
		MelderExtremaWithInit extrema_db;
		for (integer i = workspace -> imin; i <= workspace -> imax; i ++)
			extrema_db.update (workspace -> y[i]);
		dBmaximum = extrema_db.max;
		dBminimum = extrema_db.min;
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, qmin, qmax, dBminimum, dBmaximum);
	const double slope = workspace -> slope, intercept = workspace -> intercept;
	/*
		Don't draw part outside window
	*/
	const double lineWidth = Graphics_inqLineWidth (g);
	Graphics_setLineWidth (g, 2);
	if (lineType == kCepstrum_trendType::EXPONENTIAL_DECAY ) {
		integer n = 500;
		const double dq = (qendFit - qstartFit) / (n + 1);
		const double q1 = qstartFit;
		if (qstartFit <= 0.0) {
			qstartFit = 0.1 * dq;   // some small offset to avoid log(0)
			n --;
		}
		autoVEC y = raw_VEC (n);
		
		for (integer i = 1; i <= n; i ++) {
			const double q = q1 + (i - 1) * dq;
			y [i] = slope * log (q) + intercept;
		}
		Graphics_function (g, y.asArgumentToFunctionThatExpectsOneBasedArray(), 1, n, qstartFit, qendFit);
	} else {
		const double y1 = slope * qstartFit + intercept;
		const double y2 = slope * qendFit + intercept;
		if (y1 >= dBminimum && y2 >= dBminimum) {
			Graphics_line (g, qstartFit, y1, qendFit, y2);
		} else if (y1 < dBminimum) {
			qstartFit = (dBminimum - intercept) / slope;
			Graphics_line (g, qstartFit, dBminimum, qendFit, y2);
		} else if (y2 < dBminimum) {
			qendFit = (dBminimum - intercept) / slope;
			Graphics_line (g, qstartFit, y1, qendFit, dBminimum);
		} else {
			// don't draw anything below lower limit
		}
	}
	Graphics_setLineWidth (g, lineWidth);
	Graphics_unsetInner (g);
}

double PowerCepstrum_getTrendLineValue (PowerCepstrum me, double quefrency, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	double trenddb = undefined;
	if (quefrency >= my xmin && quefrency <= my xmax) {
		autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qstartFit, qendFit, lineType, fitMethod);
		trenddb = workspace -> getTrend (quefrency);
	}
	return trenddb;
}

void PowerCepstrum_subtractTrend_inplace (mutablePowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qstartFit, qendFit, lineType, fitMethod);
	workspace -> subtractTrend ();
}

autoPowerCepstrum PowerCepstrum_subtractTrend (constPowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	try {
		autoPowerCepstrum thee = Data_copy (me);
		PowerCepstrum_subtractTrend_inplace (thee.get(), qstartFit, qendFit, lineType, fitMethod);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": couldn't subtract trend line.");
	}
}

static void PowerCepstrum_smooth_inplaceRectangular (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		const double halfWindwow = 0.5 * quefrencyAveragingWindow;
		const double numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
		if (numberOfQuefrencyBins > 1.0) {
			autoVEC qout = raw_VEC (my nx);
			for (integer k = 1; k <= numberOfIterations; k ++) {
				for (integer isamp = 1; isamp <= my nx; isamp ++) {
					const double xmid = Sampled_indexToX (me, isamp);
					qout [isamp] = Sampled_getMean (me, xmid - halfWindwow, xmid + halfWindwow, 1, 0, true);
				}
				my z.row (1)  <<=  qout.all();
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static void PowerCepstrum_smooth_inplaceRectangular_old (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		const integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dx);
		if (numberOfQuefrencyBins > 1) {
			autoVEC qin = copy_VEC (my z.row (1));
			autoVEC qout = raw_VEC (my nx);
			for (integer k = 1; k <= numberOfIterations; k ++)
				if (k % 2 == 1) 
					VECsmoothByMovingAverage_preallocated (qout.get(), qin.get(), numberOfQuefrencyBins);
				else
					VECsmoothByMovingAverage_preallocated (qin.get(), qout.get(), numberOfQuefrencyBins);
			my z.row (1)  <<=  ( numberOfIterations % 2 == 1 ? qout.get() : qin.get() );
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static void PowerCepstrum_smooth_inplaceGaussian (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		const double numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
		if (numberOfQuefrencyBins > 1.0) {
			/*
				Applying two Gaussians after another is associative: (G(s2)*(G(s1)*f) = (G(s2)*G(s1))*f.
				G(s2) * G(s1) = G(s), where s^2=s1^2+s2^2
			*/
			const double numberOfSigmasInWindow = 4.0;
			const double sigma = numberOfQuefrencyBins / numberOfSigmasInWindow; // 3sigma -> 99.7 % of the data (2sigma -> 95.4%)g 
			const double sigma_n = sqrt (numberOfIterations) * sigma;
			VECsmooth_gaussian_inplace (my z.row (1), sigma_n);
			/*
				Due to imprecise arithmatic some values might turn out to be negative
				(but very small). Just make them positive.
			*/
			abs_VEC_inout (my z.row (1));
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

void PowerCepstrum_smooth_inplace (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	if (Melder_debug == -4)
		PowerCepstrum_smooth_inplaceRectangular_old (me, quefrencyAveragingWindow, numberOfIterations);
	else if (Melder_debug == -5)
		PowerCepstrum_smooth_inplaceGaussian (me, quefrencyAveragingWindow, numberOfIterations);
	else
		PowerCepstrum_smooth_inplaceRectangular (me, quefrencyAveragingWindow, numberOfIterations);
}

autoPowerCepstrum PowerCepstrum_smooth (constPowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_smooth_inplace (thee.get(), quefrencyAveragingWindow, numberOfIterations);
	return thee;
}


void PowerCepstrum_getMaximumAndQuefrency_q (PowerCepstrum me, double qminPeakSearch, double qmaxPeakSearch,
	kCepstrum_peakInterpolation peakInterpolationType, double& peakdB, double& quefrency)
{
	kVector_peakInterpolation interpolation = ( peakInterpolationType == kCepstrum_peakInterpolation :: PARABOLIC ?
		kVector_peakInterpolation :: PARABOLIC : (peakInterpolationType == kCepstrum_peakInterpolation :: CUBIC ?
		kVector_peakInterpolation :: CUBIC : kVector_peakInterpolation :: NONE)
	);
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qminPeakSearch, qmaxPeakSearch, kCepstrum_trendType::LINEAR, kCepstrum_trendFit::ROBUST_FAST);
	workspace -> initPeakSearchPart (qminPeakSearch, qmaxPeakSearch, interpolation);
	workspace -> getPeakAndPosition ();
	peakdB = workspace -> peakdB;
	quefrency = workspace -> peakQuefrency;	
}

//TODO
void PowerCepstrum_getMaximumAndQuefrency_pitch (PowerCepstrum me, double pitchFloor, double pitchCeiling,
	kVector_peakInterpolation peakInterpolationType, double& peakdB, double& quefrency)
{
	const double qminPeakSearch = 1.0 / pitchCeiling;
	const double qmaxPeakSearch = 1.0 / pitchFloor;
	
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qminPeakSearch, qmaxPeakSearch, kCepstrum_trendType::LINEAR, kCepstrum_trendFit::ROBUST_FAST);
	workspace -> initPeakSearchPart (qminPeakSearch, qmaxPeakSearch, peakInterpolationType);
	workspace -> getPeakAndPosition ();
	peakdB = workspace -> peakdB;
	quefrency = workspace -> peakQuefrency;	
}

autoTable PowerCepstrum_tabulateRhamonics (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType) {
	try {
		const double qminPeakSearch = 1.0 / pitchCeiling;
		const double qmaxPeakSearch = 1.0 / pitchFloor;
		autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qminPeakSearch, qmaxPeakSearch,
			kCepstrum_trendType::LINEAR, kCepstrum_trendFit::ROBUST_FAST);

		workspace -> initPeakSearchPart (qminPeakSearch, qmaxPeakSearch, peakInterpolationType);
		workspace -> getRhamonicPeaks (qminPeakSearch, qmaxPeakSearch);
		
		const conststring32 columnNames [] = { U"peak(dB)", U"quefrency(s)" };
		autoTable thee = Table_createWithColumnNames (workspace -> numberOfRhamonics, ARRAY_TO_STRVEC (columnNames));

		for (integer rhamonic = 1; rhamonic <= workspace -> numberOfRhamonics; rhamonic ++) {
			Table_setNumericValue (thee.get(), rhamonic, 1, workspace -> rhamonics [rhamonic][1]);
			Table_setNumericValue (thee.get(), rhamonic, 2, workspace -> rhamonics [rhamonic][2]);
		}		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rhamonics could not be tabulated.");
	}
}

static autoMAT PowerCepstrum_getRhamonicsPower (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	try {
		const double qminPeakSearch = 1.0 / pitchCeiling;
		const double qmaxPeakSearch = 1.0 / pitchFloor;
		autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, qminPeakSearch, qmaxPeakSearch,
			kCepstrum_trendType::LINEAR, kCepstrum_trendFit::ROBUST_FAST);
		workspace -> initPeakSearchPart (qminPeakSearch, qmaxPeakSearch, kVector_peakInterpolation :: CUBIC);
		workspace -> getRhamonicsPower (qminPeakSearch, qmaxPeakSearch, f0fractionalWidth);
		autoMAT result = copy_MAT (workspace -> rhamonics.get());
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": rhamonics could not be calculated.");
	}
}

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	const double qminPeakSearch = 1.0 / pitchCeiling;
	const double qmaxPeakSearch = 1.0 / pitchFloor;
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me,qminPeakSearch, qmaxPeakSearch, 
		kCepstrum_trendType::LINEAR, kCepstrum_trendFit::ROBUST_FAST);
	workspace -> initPeakSearchPart (qminPeakSearch, qmaxPeakSearch, kVector_peakInterpolation :: CUBIC);
	const double rnr = workspace -> getRNR (qminPeakSearch, qmaxPeakSearch, f0fractionalWidth);
	return rnr;
}

double PowerCepstrum_getPeakProminence_hillenbrand (PowerCepstrum me, double pitchFloor, double pitchCeiling, double& qpeak) {
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me, 0.001, my xmax, kCepstrum_trendType::LINEAR,
		kCepstrum_trendFit::LEAST_SQUARES);
	const double qmaxPeakSearch = 1.0 / pitchFloor, qminPeakSearch = 1.0 /pitchCeiling;
	workspace -> initPeakSearchPart (qminPeakSearch,qmaxPeakSearch, kVector_peakInterpolation :: NONE);
	workspace -> getCPP ();
	qpeak = workspace -> peakQuefrency;
	return workspace -> cpp;
}

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, 
	kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, 
	kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod, double& qpeak)
{
	autoPowerCepstrumWorkspace workspace = PowerCepstrumWorkspace_create (me,qstartFit, qendFit, trendLineType, fitMethod);
	const double qmaxPeakSearch = 1.0 / pitchFloor, qminPeakSearch = 1.0 /pitchCeiling;
	workspace -> initPeakSearchPart (qminPeakSearch,qmaxPeakSearch, peakInterpolationType);
	workspace -> getCPP ();
	qpeak = workspace -> peakQuefrency;
	return workspace -> cpp;
}

autoPowerCepstrum Cepstrum_downto_PowerCepstrum (constCepstrum me) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		for (integer i = 1; i <= my nx; i ++)
			thy z [1] [i] = sqr (my z [1] [i]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" not converted.");
	}
}

autoMatrix PowerCepstrum_to_Matrix (constPowerCepstrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum (constMatrix me) {
	try {
		Melder_require (my ny == 1,
			U"Matrix should have exactly 1 row.");
		autoPowerCepstrum thee = Thing_new (PowerCepstrum);
		my structMatrix :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PowerCepstrum.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum_row (constMatrix me, integer row) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		Melder_require (row > 0 && row <= my ny,
			U"Row number should be between 1 and ", my ny, U" inclusive.");
		thy z.row (1)  <<=  my z.row (row);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum_column (constMatrix me, integer col) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		Melder_require (col > 0 && col <= my nx,
			U"Column number should be between 1 and ", my nx, U" inclusive.");
		thy z.row (1)  <<=  my z.column (col);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

/* End of file PowerCepstrum.cpp */
