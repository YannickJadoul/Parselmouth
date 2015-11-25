set(SRC Network.cpp
        OTGrammar.cpp OTGrammarEditor.cpp manual_gram.cpp praat_gram.cpp OTMulti.cpp OTMultiEditor.cpp
        OTGrammar_ex_metrics.cpp OTGrammar_ex_NoCoda.cpp OTGrammar_ex_NPA.cpp OTGrammar_ex_tongueRoot.cpp
        OTMulti_ex_metrics.cpp)

add_library(gram ${SRC})
target_link_libraries(gram num kar sys dwsys stat dwtools fon)

