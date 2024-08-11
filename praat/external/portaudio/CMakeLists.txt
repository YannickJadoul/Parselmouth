add_praat_subdir(
SOURCES
	pa_linux_alsa.c pa_jack.c pa_ringbuffer.c
	pa_front.c pa_debugprint.c pa_cpuload.c
	pa_allocation.c pa_process.c pa_converters.c pa_dither.c
	pa_stream.c
)
if (UNIX)
	add_praat_subdir(SOURCES pa_unix_hostapis.c pa_unix_util.c)

	if (CMAKE_SYSTEM_NAME EQUAL "Linux")
		add_praat_subdir(SOURCES pa_linux_alsa.c)
	elseif(CMAKE_SYSTEM_NAME EQUAL "Darwin")
		add_praat_subdir(SOURCES
			pa_mac_core_blocking.c pa_mac_core_utilities.c pa_mac_core.c pa_mac_hostapis.c pa_mac_util.c
			pa_jack.c pa_ringbuffer.c pa_trace.c
		)
	endif()
elseif (WIN32)
	add_praat_subdir(SOURCES pa_win_hostapis.c pa_win_util.c pa_win_wmme.c pa_win_waveformat.c)
endif ()
