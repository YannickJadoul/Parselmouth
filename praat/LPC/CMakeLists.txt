add_praat_subdir(SOURCES
	Cepstrum.cpp Cepstrumc.cpp Cepstrum_and_Spectrum.cpp
	Cepstrogram.cpp
	Formant_extensions.cpp
	FormantModeler.cpp FormantModelerList.cpp
	FormantPath.cpp FormantPath_to_IntervalTier.cpp FormantPathArea.cpp FormantPathEditor.cpp
	LineSpectralFrequencies.cpp
	SoundToLPCWorkspace.cpp
	LPCToSampledWorkspace.cpp LPCToFormantWorkspace.cpp
	LPC.cpp LPC_and_Cepstrumc.cpp LPC_and_Formant.cpp LPC_and_LFCC.cpp
	LPC_and_LineSpectralFrequencies.cpp LPC_and_Polynomial.cpp
	LPC_to_Spectrum.cpp LPC_to_Spectrogram.cpp
	LPC_and_Tube.cpp PowerCepstrum.cpp PowerCepstrogram.cpp
	Sound_and_LPC.cpp Sound_and_LPC_robust.cpp
	Sound_to_Formant_mt.cpp Roots_and_Formant.cpp SoundToFormantWorkspace.cpp
	Sound_and_Cepstrum.cpp Tube.cpp
	VocalTractTier.cpp
	praat_LPC_init.cpp manual_LPC.cpp
INCLUDE_DIRS
	LPC kar melder fon foned dwtools sys dwsys stat
)
