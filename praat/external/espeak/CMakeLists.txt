add_praat_subdir(SOURCES
	case.cpp categories.cpp common.cpp compiledict.cpp
	dictionary.cpp
	encoding.cpp error.cpp espeak_api.cpp
	intonation.cpp klatt.cpp langopts.cpp mnemonics.cpp
	numbers.cpp phonemelist.cpp proplist.cpp readclause.cpp
	setlengths.cpp soundicon.cpp speech.cpp
	ssml.cpp synthdata.cpp synthesize.cpp
	tr_languages.cpp translate.cpp translateword.cpp voices.cpp wavegen.cpp
	espeak_praat_FileInMemorySet_addLanguages.cpp espeak_praat_FileInMemorySet_addVoices.cpp
	espeak_praat_FileInMemorySet_addRussianDict.cpp espeak_praat_FileInMemorySet_addFaroeseDict.cpp
	espeak_praat_FileInMemorySet_addOtherDicts.cpp espeak_praat_FileInMemorySet_addPhon.cpp
	espeak_praat.cpp
INCLUDE_DIRS
	kar melder sys dwsys stat
)
