find_package(GTK2 REQUIRED gtk)

add_definitions(${GTK2_DEFINITIONS})

set(PRAAT_INCLUDE_DIRS kar
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
                       contrib/ola
                       external/gsl
                       external/glpk
                       external/mp3
                       external/flac
                       external/portaudio
                       external/espeak
                       ${GTK2_INCLUDE_DIRS})

set(PRAAT_DEFINITIONS ${GTK2_DEFINITIONS})

set(PRAAT_C_FLAGS "-std=gnu99 -pthread")
set(PRAAT_CXX_FLAGS "-std=c++11 -pthread")


# Trying to convert the flags from the Praat makefiles...
if (APPLE)
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DuseCarbon=0 -Dmacintosh")
elseif (UNIX)
	set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -DUNIX")
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(PRAAT_DEFINITIONS "${PRAAT_DEFINITIONS} -Dlinux")
	endif()
else ()
	message(FATAL_ERROR "Other build environments than Unix and OSX are not supported.")
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


add_subdirectory(artsynth)
add_subdirectory(contrib/ola)
add_subdirectory(dwsys)
add_subdirectory(dwtools)
add_subdirectory(external/espeak)
add_subdirectory(external/flac)
add_subdirectory(external/glpk)
add_subdirectory(external/gsl)
add_subdirectory(external/mp3)
add_subdirectory(external/portaudio)
add_subdirectory(EEG)
add_subdirectory(FFNet)
add_subdirectory(fon)
add_subdirectory(gram)
add_subdirectory(kar)
add_subdirectory(LPC)
add_subdirectory(num)
add_subdirectory(stat)
add_subdirectory(sys)

add_library(praat dummy.cpp)
target_link_libraries(praat fon ola artsynth FFNet gram EEG LPC dwtools fon stat dwsys sys num kar espeak portaudio flac mp3 glpk gsl ${GTK2_LIBRARIES})


foreach(X ${PRAAT_INCLUDE_DIRS})
	get_filename_component(X ${X} ABSOLUTE)
	set(PRAAT_INCLUDE_DIRS_ABS ${PRAAT_INCLUDE_DIRS_ABS} ${X})
endforeach()

set(PRAAT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${PRAAT_INCLUDE_DIRS_ABS} PARENT_SCOPE)
set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} PARENT_SCOPE)
set(PRAAT_C_FLAGS ${PRAAT_C_FLAGS} PARENT_SCOPE)
set(PRAAT_CXX_FLAGS ${PRAAT_CXX_FLAGS} PARENT_SCOPE)

