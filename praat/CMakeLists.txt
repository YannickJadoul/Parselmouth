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

set(PRAAT_DEFINITIONS -DNO_GUI -DNO_AUDIO -DNO_GRAPHICS -DNO_NETWORK)

# Trying to convert the flags from the Praat makefiles...
if (UNIX)
	set(PRAAT_FLAGS ${PRAAT_FLAGS} -pthread)
	set(PRAAT_FLAGS_PRIVATE -Wno-trigraphs -Wno-logical-op-parentheses)
	
	set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} -DUNIX)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} -DPLATFORM_POSIX -DHAVE_MKSTEMP) # eSpeak
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(PRAAT_DEFINITIONS ${PRAAT_DEFINITIONS} -Dlinux)
	endif()
elseif (WIN32)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} -D_CRT_SECURE_NO_WARNINGS -DUNICODE) # Windows no "safe alternatives" warning, and Windows unicode API by default
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} -DFLAC__NO_DLL) # flac
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} -DPLATFORM_WINDOWS) # eSpeak
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /wd4068) # "warning C4068: unknown pragma": #pragma mark
	set(PRAAT_FLAGS ${PRAAT_FLAGS} /EHs) # Some 'extern "C"' functions throw exceptions: make sure they're caught
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} -D_FILE_OFFSET_BITS=64)
else ()
	set(PRAAT_DEFINITIONS_PRIVATE ${PRAAT_DEFINITIONS_PRIVATE} -D_FILE_OFFSET_BITS=32)
endif ()

foreach(SUBDIR ${PRAAT_SUBDIRECTORIES})
  add_subdirectory(${SUBDIR})
endforeach()

add_library(praat ${SRC})
target_include_directories(praat PUBLIC ${PRAAT_INCLUDE_DIRS})
target_compile_definitions(praat PUBLIC ${PRAAT_DEFINITIONS} PRIVATE ${PRAAT_DEFINITIONS_PRIVATE})
target_compile_options(praat PUBLIC ${PRAAT_FLAGS} PRIVATE ${PRAAT_FLAGS_PRIVATE})

set_property(TARGET praat PROPERTY POSITION_INDEPENDENT_CODE TRUE)
set_property(TARGET praat PROPERTY C_STANDARD 99)
set_property(TARGET praat PROPERTY CXX_STANDARD 11)
set_property(TARGET praat PROPERTY CXX_STANDARD_REQUIRED TRUE)
