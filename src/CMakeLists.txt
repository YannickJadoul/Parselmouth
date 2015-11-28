find_package(PythonLibs)
find_package(Boost COMPONENTS python)

include_directories(${PRAAT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_definitions(${PRAAT_DEFINITIONS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PRAAT_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ${PRAAT_CXX_FLAGS}")

set(SRC parselmouth.cpp)

python_add_module(parselmouth ${SRC})
target_link_libraries(parselmouth praat ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
install(TARGETS parselmouth DESTINATION lib)

