cmake_minimum_required(VERSION 2.8.12)
project(Parselmouth)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/cmake ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/installed CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif()

add_subdirectory(pybind11)

add_subdirectory(praat)
add_subdirectory(src)

