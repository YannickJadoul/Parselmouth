set(SRC FFNet.cpp
        FFNet_Eigen.cpp FFNet_Matrix.cpp FFNet_Pattern.cpp
        FFNet_Activation_Categories.cpp FFNet_Pattern_Activation.cpp
        FFNet_Pattern_Categories.cpp
        praat_FFNet_init.cpp manual_FFNet.cpp)

add_library(FFNet ${SRC})
target_link_libraries(FFNet num dwtools fon sys dwsys stat)

