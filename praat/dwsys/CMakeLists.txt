add_praat_subdir(SOURCES
	ChebyshevSeries.cpp Collection_extensions.cpp Command.cpp
	DoublyLinkedList.cpp Eigen.cpp ExtendedReal.cpp
	FileInMemory.cpp
	FunctionSeries.cpp
	Graphics_extensions.cpp Index.cpp
	LegendreSeries.cpp
	MAT_numerics.cpp
	NMF.cpp NUM2.cpp NUMhuber.cpp NUMmachar.cpp
	NUMcomplex.cpp NUMFourier.cpp NUMsorting.cpp
	NUMmathlib.cpp NUMstring.cpp NUMselect.cpp
	Permutation.cpp PermutationInversionCounter.cpp Permutation_and_Index.cpp
	Polynomial.cpp
	Roots.cpp
	SlopeSelector.cpp Spline.cpp SVD.cpp WorkvectorPool.cpp
INCLUDE_DIRS
	melder stat sys fon external/gsl external/num external/clapack kar dwtools
)
