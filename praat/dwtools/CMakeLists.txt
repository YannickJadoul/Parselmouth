target_sources(praat PRIVATE
	ActivationList.cpp AffineTransform.cpp
	Categories.cpp CategoriesEditor.cpp
	Categories_and_Strings.cpp CCA.cpp CCA_and_Correlation.cpp
	Correlation.cpp Covariance.cpp CC.cpp CCs_to_DTW.cpp
	ClassificationTable.cpp Confusion.cpp
	ComplexSpectrogram.cpp Configuration.cpp ContingencyTable.cpp
	Configuration_AffineTransform.cpp
	Configuration_and_Procrustes.cpp DataModeler.cpp Distance.cpp
	DTW.cpp DTW_and_TextGrid.cpp
	Discriminant.cpp Discriminant_PatternList_Categories.cpp
	Electroglottogram.cpp EditDistanceTable.cpp EEG_extensions.cpp
	Eigen_and_Matrix.cpp Eigen_and_Procrustes.cpp
	Eigen_and_TableOfReal.cpp
	Eigen_and_SSCP.cpp Excitations.cpp
	espeakdata_FileInMemory.cpp
	FilterBank.cpp FormantGrid_extensions.cpp
	GaussianMixture.cpp
	HMM.cpp
	ICA.cpp Intensity_extensions.cpp IntervalTierNavigator.cpp
	LFCC.cpp LongSound_extensions.cpp
	KlattGrid.cpp KlattGridEditors.cpp KlattTable.cpp
	Ltas_extensions.cpp
	MDSVec.cpp MFCC.cpp
	manual_DataModeler.cpp manual_dwtools.cpp manual_BSS.cpp manual_HMM.cpp
	manual_KlattGrid.cpp manual_MDS.cpp manual_Permutation.cpp
	Minimizers.cpp MixingMatrix.cpp
	Matrix_and_NMF.cpp Matrix_extensions.cpp
	Matrix_Categories.cpp MDS.cpp
	NavigationContext.cpp
	OptimalCeilingTier.cpp OptimalCeilingTierEditor.cpp
	PatternList.cpp PCA.cpp
	Pitch_extensions.cpp Polynomial_to_Spectrum.cpp
	Polygon_extensions.cpp Procrustes.cpp
	Proximity.cpp Proximity_and_Distance.cpp
	Resonator.cpp Roots_to_Spectrum.cpp
	Sound_and_MixingMatrix.cpp
	Sound_and_Spectrogram_extensions.cpp Sound_and_PCA.cpp Sound_extensions.cpp
	Sound_to_MFCC.cpp Sounds_to_DTW.cpp
	Sound_to_Pitch2.cpp Sound_to_SPINET.cpp SPINET.cpp SPINET_to_Pitch.cpp
	Spectrogram_extensions.cpp Spectrum_extensions.cpp SSCP.cpp Strings_extensions.cpp
	SpeechSynthesizer.cpp SpeechSynthesizer_and_TextGrid.cpp
	Table_and_Strings.cpp Table_extensions.cpp TableOfReal_and_SVD.cpp
	TableOfReal_extensions.cpp
	TableOfReal_and_Discriminant.cpp TableOfReal_and_Permutation.cpp
	TextGrid_and_DurationTier.cpp TextGrid_and_PitchTier.cpp TextGrid_extensions.cpp
	TextGridNavigator.cpp TextGridView.cpp VowelEditor.cpp
	praat_MDS_init.cpp praat_BSS_init.cpp praat_HMM_init.cpp
	praat_KlattGrid_init.cpp praat_DataModeler_init.cpp praat_David_init.cpp
)
