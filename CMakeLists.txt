# Copyright (C) 2016-2022  Yannick Jadoul
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

cmake_minimum_required(VERSION 3.18)
project(Parselmouth)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/cmake ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if (MSVC)
	add_compile_options(/permissive- /Zc:inline) # Please follow the standard more closely, MSVC (e.g. adds ciso646 alternative tokens for logical operators)
	add_compile_options(/utf-8) # Because Praat contains source files with unicode chars encoded as UTF-8, and because UTF-8 is not default enough for MSVC to read and compile these files correctly (also, because it's 2020 by now)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS UNICODE NOMINMAX) # Windows no "safe alternatives" warning, Windows unicode API by default, and no <windows.h> 'min' and 'max' macros
	if (MSVC_VERSION GREATER_EQUAL 1920)
		add_compile_options(/d2FH4-) # Because we don't want to link against VCRUNTIME140_1.dll (see https://cibuildwheel.readthedocs.io/en/stable/faq/#importerror-dll-load-failed-the-specific-module-could-not-be-found-error-on-windows)
	endif ()
endif ()

if (PREBUILT_DEPENDENCIES)
	file(TO_CMAKE_PATH ${PREBUILT_DEPENDENCIES} PREBUILT_DEPENDENCIES_DIR)
	include(${PREBUILT_DEPENDENCIES_DIR}/dependency_exports.cmake)
else ()
	add_subdirectory(extern)
	add_subdirectory(praat)
	export(TARGETS praat fmt FILE dependency_exports.cmake)
endif ()

# Currently, scikit-build does not support FindPython.
if(SKBUILD)
	set(Python_EXECUTABLE "${PYTHON_EXECUTABLE}")
	set(Python_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")
	unset(PYTHON_EXECUTABLE)
	unset(PYTHON_INCLUDE_DIR)
	unset(PYTHON_LIBRARY)
	unset(PYTHON_VERSION_STRING)
endif()

set(Python_FIND_IMPLEMENTATIONS CPython PyPy)
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
add_subdirectory(pybind11)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)

if(SKBUILD)
	install(TARGETS parselmouth LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
