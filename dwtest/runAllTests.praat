# dwtest/runAllTests.praat
# 2024-12-01 writing settings to UTF-8 for Praat 7
# 2025-08-16 make more reproducible

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

tests = Create Strings as file list: "tests", "test_*.praat"
ntests = Get number of strings
for itest to ntests
	selectObject: tests
	test$ = Get string: itest
	appendInfoLine: test$
	report_before$ = Report memory use
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
	runScript: test$
	@check_memory: report_before$, "   "
endfor
random_initializeSafelyAndUnpredictably()
Debug multi-threading: "yes", 0, 0, "no"

procedure check_memory: .report_before$, .preprint$
	.m$[1] = "Strings: "
	.m$[2] = "Arrays: "
	.m$[3] = "Things: "
	.report_after$ = Report memory use
	appendInfoLine: .preprint$, "Memory:"
	for .i to 3
		.nb = extractNumber (.report_before$, .m$[.i])
		.na = extractNumber (.report_after$, .m$[.i])
		.post$ = if .nb <> .na then " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" else "" endif
		appendInfoLine: .preprint$, .m$[.i], .nb, " ", .na, .post$
	endfor
endproc
removeObject: tests

appendInfoLine: "                 ALL PRAAT TESTS WENT OK"
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
