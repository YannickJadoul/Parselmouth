# Copyright (C) 2016-2023  Yannick Jadoul
#
# This file is part of Parselmouth.
#
# Parselmouth is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Parselmouth is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>

function(add_praat_subdir)
	cmake_parse_arguments(ARG "" "" "SOURCES;INCLUDE_DIRS" ${ARGN})

	target_sources(praat PRIVATE ${ARG_SOURCES})
	get_target_property(PRAAT_SOURCE_DIR praat SOURCE_DIR)
	list(TRANSFORM ARG_INCLUDE_DIRS PREPEND "${PRAAT_SOURCE_DIR}/" OUTPUT_VARIABLE ABSOLUTE_INCLUDE_DIRS)
	set_source_files_properties(${ARG_SOURCES} DIRECTORY "${PRAAT_SOURCE_DIR}" PROPERTIES INCLUDE_DIRECTORIES "${ABSOLUTE_INCLUDE_DIRS}")
endfunction()

set(PRAAT_SUBDIRECTORIES
	external/clapack
	external/gsl
	external/glpk
	external/lame
	external/mp3
	external/flac
	external/portaudio
	external/espeak
	external/vorbis
	external/opusfile
	kar
	melder
	sys
	dwsys
	stat
	fon
	foned
	dwtools
	LPC
	EEG
	sensors
	gram
	FFNet
	artsynth
)

set(PRAAT_INCLUDE_DIRS ${PRAAT_SUBDIRECTORIES})
list(FILTER PRAAT_INCLUDES EXCLUDE REGEX "^external/")

set(PRAAT_FLAGS)
set(PRAAT_FLAGS_PRIVATE)
set(PRAAT_DEFINITIONS NO_GUI NO_AUDIO NO_GRAPHICS NO_NETWORK PRAAT_INSIDE_PARSELMOUTH)
set(PRAAT_DEFINITIONS_PRIVATE _FILE_OFFSET_BITS=64)

# Trying to convert the flags from the Praat makefiles...
if (UNIX)
	set(PRAAT_FLAGS ${PRAAT_FLAGS} -pthread)
	set(PRAAT_FLAGS ${PRAAT_FLAGS} -ffp-contract=off) # Necessary for architectures with assembly instructions like fmadd (aarch64, or x64 with newer -march=): results can differ from Praat when these are used
	set(PRAAT_FLAGS_PRIVATE ${PRAAT_FLAGS_PRIVATE} -Wno-trigraphs)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(PRAAT_FLAGS_PRIVATE ${PRAAT_FLAGS_PRIVATE} -Wno-logical-op-parentheses -Wno-shift-op-parentheses) # Clang complains a lot about '-Wlogical-op-parentheses' and '-Wshift-op-parentheses' warnings, but GCC doesn't know this option
	endif()
	
	set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} UNIX)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} PLATFORM_POSIX) # eSpeak
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} linux)
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} macintosh)
	endif ()
elseif (WIN32)
	# set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} FLAC__NO_DLL) # flac_config.h should already have this set
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} PLATFORM_WINDOWS) # eSpeak
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /wd4068) # "warning C4068: unknown pragma": #pragma mark
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /EHs) # Some 'extern "C"' functions throw exceptions: make sure they're caught
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /fp:precise) # /fp:precise is default does not allow contractions starting from VS 2022, and /fp:strict does not allow for arithmetic expressions in constants
	set(PRAAT_FLAGS_PRIVATE ${PRAAT_FLAGS_PRIVATE} /wd4554) # "warning C4554: '<<': check operator precedence for possible error; use parentheses to clarify precedence"
endif ()

add_library(praat)
foreach(SUBDIR ${PRAAT_SUBDIRECTORIES})
	add_subdirectory(${SUBDIR})
endforeach()

target_include_directories(praat INTERFACE ${PRAAT_INCLUDE_DIRS})
target_compile_definitions(praat PUBLIC ${PRAAT_DEFINITIONS} PRIVATE ${PRAAT_DEFINITIONS_PRIVATE})
target_compile_options(praat PUBLIC ${PRAAT_FLAGS} PRIVATE ${PRAAT_FLAGS_PRIVATE})
target_link_libraries(praat PUBLIC fmt::fmt)

set_target_properties(praat PROPERTIES C_STANDARD 99 CXX_STANDARD 17 CXX_STANDARD_REQUIRED TRUE)
