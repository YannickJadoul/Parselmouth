include(add_sources)

include_directories(${PRAAT_INCLUDE_DIRS})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(${PRAAT_DEFINITIONS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PRAAT_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unknown-pragmas -std=c++14 ${PRAAT_CXX_FLAGS}")

add_sources(parselmouth.cpp)
add_subdirectory(parselmouth)

pybind11_add_module(parselmouth ${SRC})
target_link_libraries(parselmouth PRIVATE praat)
install(TARGETS parselmouth DESTINATION ${CMAKE_INSTALL_PREFIX})
