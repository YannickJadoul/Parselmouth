cmake_minimum_required(VERSION 2.8)
project(Parselmouth)

if(NOT CMAKE_BUILD_TYPE)
  #set(CMAKE_BUILD_TYPE "RELEASE")
  set(CMAKE_BUILD_TYPE "DEBUG")
endif()

add_subdirectory(praat)
add_subdirectory(src)

