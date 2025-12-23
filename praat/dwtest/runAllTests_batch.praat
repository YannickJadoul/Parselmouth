# Praat script test/runAllTests_batch.praat
# Paul Boersma 2020-04-18
# 2024-12-01 writing settings to UTF-8 for Praat 7
# 2025-08-16 make more reproducible
#
# This script runs all Praat scripts in its subdirectories.
# This script is to be called from the command line:
#     praat --run runAllTests_batch.praat
#
# Scripts containing `_GUI_` in their names are ignored.
#

#
# Some tests require standard settings for the text input encoding:
# files in UTF-8 format should always be readable.
#
Text writing preferences: "UTF-8"
if macintosh
	Text reading preferences: "try UTF-8, then MacRoman"
elif windows
	Text reading preferences: "try UTF-8, then Windows Latin-1"
elif unix
	Text reading preferences: "try UTF-8, then ISO Latin-1"
else
	exitScript: "Unknown operating system."
endif

writeInfoLine: "Running all tests..."

files = Create Strings as file list: "files", "*.praat"
numberOfFiles = Get number of strings
for file to numberOfFiles
		selectObject: files
		file$ = Get string: file
		if not index (file$, "runAllTests") and not index (file$, "_GUI_")
			appendInfoLine: "### executing ", file$, ":"
			#
			# The following two lines aim at making the test reproducible.
			# That is, the test should give the same result on different
			# computers and different platforms and at different times.
			# The random seed is allowed to change with the Praat release;
			# just run all the tests if you change the random seed,
			# even if there was no change in the Praat source code.
			# This is because the success of some precision tests
			# has been reported (by maintainers) to depend on the random state.
			# Also make sure that the random seed is the same
			# for the other two "runAlltests" scripts.
			#
			random_initializeWithSeedUnsafelyButPredictably (5489)
			Debug multi-threading: "yes", 8, 0, "no"
			runScript: file$
		endif
	endfor
	removeObject: files
endif
random_initializeSafelyAndUnpredictably()
Debug multi-threading: "yes", 0, 0, "no"

writeInfoLine: "                 ALL PRAAT TESTS WENT OK"
appendInfoLine: ""
line$ [5] = "        #####          #####        #####   #####"
line$ [8] = "        #####          #####        #######"
line$ [1] = "               ######               #####           #####"
line$ [2] = "           ##############           #####         #####"
line$ [4] = "        #####          #####        #####     #####"
line$ [7] = "        #####          #####        #########"
line$ [9] = "        #####          #####        #####"
line$ [3] = "         #####        #####         #####       #####"
line$ [6] = "        #####          #####        ##### #####"
for line from 1 to 9
	appendInfoLine: line$ [line]
endfor
for line from 1 to 8
	appendInfoLine: line$ [9 - line]
endfor
