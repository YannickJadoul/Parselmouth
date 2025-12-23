/* MelderThread.cpp
 *
 * Copyright (C) 2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include <vector>
#include <thread>

integer MelderThread_getNumberOfProcessors () {
	//return 1;   // un-comment-out to force single-threading
	return Melder_clippedLeft (1_integer, uinteger_to_integer_a (std::thread::hardware_concurrency ()));
}

/* global */ std::atomic <integer> theMelder_error_threadId;

integer Melder_thisThread_getUniqueID () {
	static std::atomic <integer> uniqueID = 0;
	static thread_local integer thisThread_uniqueID = uniqueID ++;
	//TRACE
	trace (thisThread_uniqueID);
	return thisThread_uniqueID;
}

void MelderThread_run (
	std::atomic <bool> *p_errorFlag,
	const integer numberOfElements,
	const integer thresholdNumberOfElementsPerThread,
	std::function <void (integer threadNumber, integer firstElement, integer lastElement)> const& threadFunction
) {
	const integer numberOfThreads = MelderThread_computeNumberOfThreads (numberOfElements, thresholdNumberOfElementsPerThread);
	if (numberOfThreads == 1) {
		threadFunction (0, 1, numberOfElements);
	} else {
		const integer numberOfExtraThreads = numberOfThreads - 1;   // at least 1
		/*
			The following cannot be an `autovector`, because autovectors don't destroy their elements.
			So it has to be std::vector.
			Also, the default initialization of a std::thread may not be guaranteed to be all zeroes.
		*/
		std::vector <std::thread> spawns;
		try {
			spawns. resize (uinteger (numberOfExtraThreads));   // default-construct a number of empty (non-joinable) threads
		} catch (...) {
			/*
				Turn the system error into a MelderError.
			*/
			Melder_throw (U"Out of memory creating a thread vector. Contact the author if this happens more often.");
		}
		const integer base = numberOfElements / numberOfThreads;
		const integer remainder = numberOfElements % numberOfThreads;
		integer firstElement = 1;
		try {
			for (integer ispawn1 = 1; ispawn1 <= numberOfExtraThreads; ispawn1 ++) {   // ispawn1 is base-1
				const integer lastElement = firstElement + base - 1 + ( ispawn1 <= remainder );
				spawns [uinteger (ispawn1 - 1)] = std::thread (threadFunction, ispawn1, firstElement, lastElement);
				firstElement = lastElement + 1;
			}
		} catch (...) {
			*p_errorFlag = true;   // try to stop any threads that were already spawned
			for (size_t ispawn0 = 0; ispawn0 < spawns.size(); ispawn0 ++)   // ispawn0 is base-0
				if (spawns [ispawn0]. joinable ())   // any extra thread already spawned
					spawns [ispawn0]. join ();   // wait for the spawned thread to finish, hopefully soon
			Melder_throw (U"Couldn't start a thread. Contact the author.");
		}
		Melder_assert (firstElement + base - 1 == numberOfElements);
		threadFunction (0, firstElement, numberOfElements);
		for (size_t ispawn0 = 0; ispawn0 < spawns.size(); ispawn0 ++)   // ispawn0 is base-0
			spawns [ispawn0]. join ();
	}
	if (*p_errorFlag) {
		theMelder_error_threadId = Melder_thisThread_getUniqueID ();
		throw MelderError();   // turn the error flag back into a MelderError
	}
}

integer MelderThread_computeNumberOfThreads (
	const integer numberOfElements,
	const integer thresholdNumberOfElementsPerThread)
{
	if (! MelderThread_getUseMultithreading ())
		return 1;
	integer minimumNumberOfElementsPerThread = MelderThread_getMinimumNumberOfElementsPerThread ();
	if (minimumNumberOfElementsPerThread <= 0)
		minimumNumberOfElementsPerThread = thresholdNumberOfElementsPerThread;
	/* mutable clip */ integer numberOfThreads =
		#if defined (macintosh)
			Melder_iroundDown ((double) numberOfElements / minimumNumberOfElementsPerThread);
				// round down, assuming that the first spawned thread is the costliest
		#elif defined (_WIN32)
			Melder_iroundDown ((double) numberOfElements / 2.0 / minimumNumberOfElementsPerThread);
				// round down, assuming that the first spawned thread is the costliest
		#elif defined (linux)
			Melder_iround ((double) numberOfElements / 1.5 / minimumNumberOfElementsPerThread);
				// round to nearest, assuming that all spawned threads are equally costly
		#else
			#error Undefined platform for MelderThread_computeNumberOfThreads().
		#endif
	Melder_clipRight (& numberOfThreads, MelderThread_getMaximumNumberOfConcurrentThreads ());
	Melder_clipRight (& numberOfThreads, NUMrandom_maximumNumberOfParallelThreads);
	Melder_clipLeft (1_integer, & numberOfThreads);
	return numberOfThreads;
}

/*
	Preferences.
*/

static struct {
	bool useMultithreading = true;
	integer maximumNumberOfConcurrentThreads = 0;   // "0" signals automatic
	integer minimumNumberOfElementsPerThread = 0;   // "0" signals the factory-tuned value
	bool traceThreads = false;
} preferences;

void MelderThread_debugMultithreading (bool useMultithreading, integer maximumNumberOfConcurrentThreads,
	integer minimumNumberOfElementsPerThread, bool traceThreads)
{
	preferences. useMultithreading = useMultithreading;
	preferences. maximumNumberOfConcurrentThreads = maximumNumberOfConcurrentThreads;
	preferences. minimumNumberOfElementsPerThread = minimumNumberOfElementsPerThread;
	preferences. traceThreads = traceThreads;
}

bool MelderThread_getUseMultithreading () {
	return preferences. useMultithreading;
}

integer MelderThread_getMaximumNumberOfConcurrentThreads () {
	if (! preferences. useMultithreading)
		return 1;
	if (preferences. maximumNumberOfConcurrentThreads <= 0)
		return MelderThread_getNumberOfProcessors ();
	return preferences. maximumNumberOfConcurrentThreads;
}

integer MelderThread_getMinimumNumberOfElementsPerThread () {
	if (! preferences. useMultithreading)
		return 1;
	if (preferences. minimumNumberOfElementsPerThread <= 0)
		return 0;   // signals factory tuning
	return preferences. minimumNumberOfElementsPerThread;
}

bool MelderThread_getTraceThreads () {
	return preferences. traceThreads;
}

static thread_local integer thisThread_firstElement { 0 }, thisThread_lastElement { 0 }, thisThread_currentElement { 0 };

void Melder_thisThread_setRange (integer firstElement, integer lastElement) {
	thisThread_firstElement = firstElement;
	thisThread_lastElement = lastElement;
}

void Melder_thisThread_setCurrentElement (integer currentElement) {
	thisThread_currentElement = currentElement;
}

double Melder_thisThread_estimateProgress () {
	return (thisThread_currentElement - thisThread_firstElement + 0.5) / (thisThread_lastElement - thisThread_firstElement + 1.0);
}

/* End of file MelderThread.cpp */
