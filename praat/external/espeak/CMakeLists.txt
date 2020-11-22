target_sources(praat PRIVATE
	categories.cpp compiledata.cpp compiledict.cpp
	dictionary.cpp
	encoding.cpp error.cpp espeak_api.cpp
	intonation.cpp klatt.cpp
	numbers.cpp phonemelist.cpp proplist.cpp readclause.cpp setlengths.cpp speech.cpp
	synthdata.cpp synthesize.cpp
	synth_mbrola.cpp tr_languages.cpp mnemonics.cpp translate.cpp voices.cpp wavegen.cpp
	create_espeak_ng_FileInMemoryManager.cpp create_espeak_ng_FileInMemorySet.cpp
	espeak_io.cpp
)
