set(SRC NUM.cpp NUMarrays.cpp NUMrandom.cpp NUMsort.cpp NUMear.cpp NUMlinprog.cpp)

add_library(num ${SRC})
target_link_libraries(num gsl glpk sys dwsys)

