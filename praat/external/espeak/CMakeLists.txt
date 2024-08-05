target_sources(praat PRIVATE
	case.cpp categories.cpp common.cpp compiledata.cpp compiledict.cpp
	dictionary.cpp
	encoding.cpp error.cpp espeak_api.cpp
	intonation.cpp klatt.cpp langopts.cpp mnemonics.cpp
	numbers.cpp phonemelist.cpp proplist.cpp readclause.cpp
	setlengths.cpp soundicon.cpp speech.cpp
	ssml.cpp synthdata.cpp synthesize.cpp
	synth_mbrola.cpp tr_languages.cpp translate.cpp translateword.cpp voices.cpp wavegen.cpp
	create_espeak_ng_FileInMemoryManager.cpp
	create_espeak_ng_FileInMemorySet__ru.cpp create_espeak_ng_FileInMemorySet.cpp
	espeak_io.cpp
)
