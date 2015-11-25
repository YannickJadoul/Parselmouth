set(SRC Speaker.cpp Articulation.cpp Artword.cpp
        Art_Speaker.cpp Art_Speaker_to_VocalTract.cpp Artword_Speaker.cpp Artword_Speaker_Sound.cpp
        Artword_Speaker_to_Sound.cpp Artword_to_Art.cpp
        Delta.cpp Speaker_to_Delta.cpp Art_Speaker_Delta.cpp
        ArtwordEditor.cpp praat_Artsynth.cpp manual_Artsynth.cpp)

add_library(artsynth ${SRC})
target_link_libraries(artsynth num sys fon stat)

