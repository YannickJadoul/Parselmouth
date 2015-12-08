find_package(PythonLibs 2.7 REQUIRED)
find_package(NumPy 1.8 REQUIRED)
find_package(Boost 1.55 REQUIRED COMPONENTS python)

set(BOOST_NUMPY_INCLUDE_DIRS include
                             ${PYTHON_INCLUDE_DIRS} 
                             ${NUMPY_INCLUDE_DIRS}
                             ${Boost_INCLUDE_DIRS})

include_directories(${BOOST_NUMPY_INCLUDE_DIRS})

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)


set(SRC src/dtype.cpp
        src/scalars.cpp
        src/ndarray.cpp
        src/matrix.cpp
        src/ufunc.cpp
        src/numpy.cpp)

add_library(boost_numpy ${SRC})
target_link_libraries(boost_numpy ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

foreach(X ${BOOST_NUMPY_INCLUDE_DIRS})
	get_filename_component(X ${X} ABSOLUTE)
	set(BOOST_NUMPY_INCLUDE_DIRS_ABS ${BOOST_NUMPY_INCLUDE_DIRS_ABS} ${X})
endforeach()

set(BOOST_NUMPY_INCLUDE_DIRS ${BOOST_NUMPY_INCLUDE_DIRS_ABS} PARENT_SCOPE)

