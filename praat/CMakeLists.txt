# Copyright (C) 2016-2019  Yannick Jadoul
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

include(add_sources)

set(PRAAT_SUBDIRECTORIES external/gsl
                         external/glpk
                         external/mp3
                         external/flac
                         external/portaudio
                         external/espeak
                         kar
                         num
                         sys
                         dwsys
                         stat
                         fon
                         dwtools
                         LPC
                         EEG
                         gram
                         FFNet
                         artsynth
                         contrib/ola)

set(PRAAT_INCLUDE_DIRS ${PRAAT_SUBDIRECTORIES})

set(PRAAT_DEFINITIONS NO_GUI NO_AUDIO NO_GRAPHICS NO_NETWORK)

# Trying to convert the flags from the Praat makefiles...
if (UNIX)
	set(PRAAT_FLAGS ${PRAAT_FLAGS} -pthread)
	set(PRAAT_FLAGS_PRIVATE -Wno-trigraphs -Wno-logical-op-parentheses)
	
	set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} UNIX)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} PLATFORM_POSIX HAVE_MKSTEMP) # eSpeak
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} linux)
	endif()
elseif (WIN32)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} _CRT_SECURE_NO_WARNINGS UNICODE) # Windows no "safe alternatives" warning, and Windows unicode API by default
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} FLAC__NO_DLL) # flac
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} PLATFORM_WINDOWS) # eSpeak
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /wd4068) # "warning C4068: unknown pragma": #pragma mark
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /EHs) # Some 'extern "C"' functions throw exceptions: make sure they're caught
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} _FILE_OFFSET_BITS=64)
else ()
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} _FILE_OFFSET_BITS=32)
endif ()

foreach(SUBDIR ${PRAAT_SUBDIRECTORIES})
	add_subdirectory(${SUBDIR})
endforeach()

add_library(praat ${SRC})
target_include_directories(praat PUBLIC ${PRAAT_INCLUDE_DIRS})
target_compile_definitions(praat PUBLIC ${PRAAT_DEFINITIONS} PRIVATE ${PRAAT_DEFINITIONS_PRIVATE})
target_compile_options(praat PUBLIC ${PRAAT_FLAGS} PRIVATE ${PRAAT_FLAGS_PRIVATE})
export(TARGETS praat FILE exports.cmake)

set_target_properties(praat PROPERTIES C_STANDARD 99 CXX_STANDARD 11 CXX_STANDARD_REQUIRED TRUE)
