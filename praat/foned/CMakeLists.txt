add_praat_subdir(SOURCES
	SoundRecorder.cpp
	FunctionArea.cpp SoundArea.cpp SoundAnalysisArea.cpp TextGridArea.cpp
	SpectrumArea.cpp PointArea.cpp PitchArea.cpp SpectrogramArea.cpp
	RealTierArea.cpp PitchTierArea.cpp IntensityTierArea.cpp DurationTierArea.cpp AmplitudeTierArea.cpp
	FormantGridArea.cpp MovieArea.cpp
	FunctionEditor.cpp
	PitchEditor.cpp SoundEditor.cpp SpectrumEditor.cpp SpectrogramEditor.cpp PointEditor.cpp
	RealTierEditor.cpp PitchTierEditor.cpp IntensityTierEditor.cpp
	DurationTierEditor.cpp AmplitudeTierEditor.cpp
	ManipulationEditor.cpp TextGridEditor.cpp FormantGridEditor.cpp
	RunnerMFC.cpp MovieWindow.cpp
INCLUDE_DIRS
	kar melder sys dwsys stat dwtools LPC fon external/portaudio external/flac external/mp3 external/espeak
)
