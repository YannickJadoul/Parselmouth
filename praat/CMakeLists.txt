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

set(PRAAT_DEFINITIONS "-DNO_AUDIO -DNO_GRAPHICS -DNO_NETWORK")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Trying to convert the flags from the Praat makefiles...
if (UNIX)
	set(PRAAT_C_FLAGS "-pthread")
	set(PRAAT_CXX_FLAGS "-pthread")
	
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DUNIX")
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DPLATFORM_POSIX") # eSpeak
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -Dlinux")
	endif()
elseif (WIN32)
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -D_CRT_SECURE_NO_WARNINGS -DUNICODE") # Windows no "safe alternatives" warning, and Windows unicode API by default
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DFLAC__NO_DLL") # flac
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DPLATFORM_WINDOWS") # eSpeak
	set(PRAAT_CXX_FLAGS "${PRAAT_CXX_FLAGS} /wd4068") # "warning C4068: unknown pragma": #pragma mark
	set(PRAAT_CXX_FLAGS "${PRAAT_CXX_FLAGS} /EHs") # Some 'extern "C"' functions throw exceptions: make sure they're caught
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_definitions(-D_FILE_OFFSET_BITS=64)
else ()
	add_definitions(-D_FILE_OFFSET_BITS=32)
endif ()

include_directories(${PRAAT_INCLUDE_DIRS})
add_definitions(${PRAAT_DEFINITIONS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PRAAT_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PRAAT_CXX_FLAGS}")

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)


foreach(SUBDIR ${PRAAT_SUBDIRECTORIES})
  add_subdirectory(${SUBDIR})
endforeach()

add_library(praat ${SRC})
target_link_libraries(praat)

foreach(X ${PRAAT_INCLUDE_DIRS})
	get_filename_component(X ${X} ABSOLUTE)
	set(PRAAT_INCLUDE_DIRS_ABS ${PRAAT_INCLUDE_DIRS_ABS} ${X})
endforeach()

set(PRAAT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${PRAAT_INCLUDE_DIRS_ABS} PARENT_SCOPE)
set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} PARENT_SCOPE)
set(PRAAT_C_FLAGS ${PRAAT_C_FLAGS} PARENT_SCOPE)
set(PRAAT_CXX_FLAGS ${PRAAT_CXX_FLAGS} PARENT_SCOPE)
