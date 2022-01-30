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

pybind11_add_module(parselmouth parselmouth.cpp)
add_subdirectory(parselmouth)

target_include_directories(parselmouth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (UNIX)
	target_compile_options(parselmouth PRIVATE
		-Wall
		-Wextra
		-Wno-unknown-pragmas  # Praat's `#pragma mark`s
	)
endif ()

set_target_properties(parselmouth PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE
	CXX_EXTENSIONS FALSE
)
set_target_properties(praat fmt PROPERTIES
	POSITION_INDEPENDENT_CODE TRUE
	# Fixing "ld: warning: direct access in function '...' from file '...' to global weak symbol '...' from file '...' means the weak symbol cannot
	# be overridden at runtime. This was likely caused by different translation units being compiled with different visibility settings."
	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
)
target_include_directories(praat SYSTEM INTERFACE ${CMAKE_SOURCE_DIR})
target_link_libraries(parselmouth PRIVATE praat)
