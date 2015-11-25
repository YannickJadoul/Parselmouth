set(SRC EEG.cpp EEGWindow.cpp ERPTier.cpp ERP.cpp ERPWindow.cpp
        praat_EEG.cpp manual_EEG.cpp)

add_library(EEG ${SRC})
target_link_libraries(EEG num kar sys dwsys stat dwtools fon)

