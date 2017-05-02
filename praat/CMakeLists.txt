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

set(PRAAT_DEFINITIONS "-DFLAC__NOASM -DNO_AUDIO -DNO_GRAPHICS -DNO_NETWORK")

set(PRAAT_C_FLAGS "-std=gnu99 -pthread")
set(PRAAT_CXX_FLAGS "-std=c++11 -pthread")


# Trying to convert the flags from the Praat makefiles...
if (UNIX)
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DUNIX")
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -Dlinux")
	endif()
else ()
	message(FATAL_ERROR "Other build environments than Unix (including OS X) are not supported.")
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
