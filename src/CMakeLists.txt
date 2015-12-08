find_package(PythonLibs 2.7 REQUIRED)
find_package(Boost 1.55 REQUIRED COMPONENTS python)

include_directories(${PRAAT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${BOOST_NUMPY_INCLUDE_DIRS})

add_definitions(${PRAAT_DEFINITIONS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PRAAT_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ${PRAAT_CXX_FLAGS}")

set(SRC parselmouth.cpp)

python_add_module(parselmouth ${SRC})
target_link_libraries(parselmouth praat boost_numpy ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
install(TARGETS parselmouth DESTINATION ${CMAKE_INSTALL_PREFIX})

