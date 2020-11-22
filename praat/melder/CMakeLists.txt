target_sources(praat PRIVATE
	melder.cpp complex.cpp melder_alloc.cpp melder_colour.cpp
	melder_ftoa.cpp melder_console.cpp melder_textencoding.cpp melder_atof.cpp melder_files.cpp
	melder_sort.cpp melder_debug.cpp MelderFile.cpp melder_strings.cpp
	melder_search.cpp
	melder_info.cpp melder_error.cpp melder_warning.cpp melder_fatal.cpp melder_progress.cpp
	melder_play.cpp melder_help.cpp melder_time.cpp
	melder_audio.cpp melder_audiofiles.cpp melder_quantity.cpp MelderReadText.cpp melder_tensorio.cpp
	abcio.cpp melder_sysenv.cpp regularExp.cpp
	NUMmath.cpp
	NUMspecfunc.cpp NUMear.cpp NUMinterpol.cpp NUMmetrics.cpp NUMrandom.cpp NUMfilter.cpp NUMlinprog.cpp
	NUM.cpp STR.cpp VEC.cpp MAT.cpp STRVEC.cpp
)
