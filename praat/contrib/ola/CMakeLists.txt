set(SRC KNN.cpp
        KNN_threads.cpp Pattern_to_Categories_cluster.cpp KNN_prune.cpp FeatureWeights.cpp praat_contrib_Ola_KNN.cpp manual_KNN.cpp)

add_library(ola ${SRC})
target_link_libraries(ola dwtools fon sys dwsys stat num gsl)

