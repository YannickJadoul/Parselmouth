# Copyright (C) 2016-2017  Yannick Jadoul
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

include_directories(${PRAAT_INCLUDE_DIRS})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_definitions(${PRAAT_DEFINITIONS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PRAAT_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PRAAT_CXX_FLAGS}")
if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unknown-pragmas")
elseif (WIN32)
	set(PYBIND11_CPP_STANDARD "/std:c++latest")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif ()

add_sources(parselmouth.cpp)
add_subdirectory(parselmouth)

pybind11_add_module(parselmouth ${SRC})
target_link_libraries(parselmouth PRIVATE praat)
install(TARGETS parselmouth DESTINATION ${CMAKE_INSTALL_PREFIX})
