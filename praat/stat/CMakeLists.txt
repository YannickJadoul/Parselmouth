set(SRC Table.cpp TableEditor.cpp Regression.cpp manual_statistics.cpp praat_Stat.cpp
        TableOfReal.cpp Distributions.cpp Distributions_and_Strings.cpp PairDistribution.cpp
        LogisticRegression.cpp)

add_library(stat ${SRC})
target_link_libraries(stat num kar sys dwsys dwtools fon)

