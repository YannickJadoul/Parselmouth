set(SRC mp3.cpp
        mad_bit.c
        mad_decoder.c
        mad_fixed.c
        mad_frame.c
        mad_huffman.c
        mad_layer12.c
        mad_layer3.c
        mad_stream.c
        mad_synth.c
        mad_timer.c
        mad_version.c)

add_library(mp3 ${SRC})
target_link_libraries(mp3 sys)

