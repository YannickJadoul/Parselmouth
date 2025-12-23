# stereoAnalysis.praat
# Paul Boersma, 21 October 2025
#
# This script checks that analyses on stereo sounds make sense
# as compared to each other and as compared to analyses on mono sounds.
#
# This script uses pre-2014 syntax, because we like to measure the history of Praat
# since multi-channel sounds were introduced in 2007.
#
# We need Praat version 5 (from 2009) or up, though,
# because version 4 didn't have objectsAreIdentical() yet.
# Requiring version 5 or up also means that we can use praatVersion and object []
# and that we can be certain that stereo sounds were supported.

#
# CREATING STEREO SOUNDS
#
# Stereo sounds were supported since 2007, and the oldest version we are testing
# is 5.1.12 from 2009. Therefore, the following code works from version 5.1.12 on.
#

#
# Create a stereo sound with only the left channel filled.
#
chan1.Sound = Create Sound from formula... chan1 stereo 0.0 1.0 44100
... if row = 1 then 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1) else 0.0 fi

#
# Create the mirror image of that sound.
#
chan2.Sound = Create Sound from formula... chan2 stereo 0.0 1.0 44100
... object [chan1.Sound, 3 - row, col]

#
# Create the sum sound.
#
sum.Sound = Create Sound from formula... sum stereo 0.0 1.0 44100
... object [chan1.Sound] + object [chan2.Sound]

#
# Create the difference sound.
#
difference.Sound = Create Sound from formula... difference stereo 0.0 1.0 44100
... object [chan1.Sound] - object [chan2.Sound]

#
# Create the average sound.
#
average.Sound = Create Sound from formula... average stereo 0.0 1.0 44100
... 0.5 * object [sum.Sound]

#
# PITCH ANALYSIS
#
# The pitch analysis should give the exact same result on all five sounds.
# This is because channel averaging occurs late, namely at the autocorrelation level.
#
# The following code works from Praat version 5.1.12 on.
#

select Sound chan1
chan1.Pitch = To Pitch... 0.01 75 600

select Sound chan2
chan2.Pitch = To Pitch... 0.01 75 600
assert objectsAreIdentical (chan2.Pitch, chan1.Pitch)

select Sound sum
sum.Pitch = To Pitch... 0.01 75 600
assert objectsAreIdentical (sum.Pitch, chan1.Pitch)

select Sound difference
difference.Pitch = To Pitch... 0.01 75 600
assert objectsAreIdentical (difference.Pitch, chan1.Pitch)

select Sound average
average.Pitch = To Pitch... 0.01 75 600
assert objectsAreIdentical (average.Pitch, chan1.Pitch)

select Pitch chan1
plus Pitch chan2
plus Pitch sum
plus Pitch difference
plus Pitch average
Remove

#
# SPECTROGRAM ANALYSIS
#
# The spectrogram analysis should give the exact same result for the two channels,
# because averaging is done at the level of the power spectral density.
#
# Also, the sum and difference sounds should give the exact same results as each other,
# because averaging is done after squaring.
#

select Sound chan1
chan1.Spectrogram = To Spectrogram... 0.005 5000 0.002 20 Gaussian

select Sound chan2
chan2.Spectrogram = To Spectrogram... 0.005 5000 0.002 20 Gaussian
assert objectsAreIdentical (chan2.Spectrogram, chan1.Spectrogram)

select Sound sum
sum.Spectrogram = To Spectrogram... 0.005 5000 0.002 20 Gaussian
assert not objectsAreIdentical (sum.Spectrogram, chan1.Spectrogram)   ; more power

select Sound difference
difference.Spectrogram = To Spectrogram... 0.005 5000 0.002 20 Gaussian
assert objectsAreIdentical (difference.Spectrogram, sum.Spectrogram)

select Spectrogram sum
Formula... 0.5 * self
assert objectsAreIdentical (sum.Spectrogram, chan1.Spectrogram)   ; halved square

select Sound average
average.Spectrogram = To Spectrogram... 0.005 5000 0.002 20 Gaussian
assert not objectsAreIdentical (average.Spectrogram, chan1.Spectrogram)
Formula... 2.0 * self
assert objectsAreIdentical (average.Spectrogram, chan1.Spectrogram)   ; doubled square

select Spectrogram chan1
plus Spectrogram chan2
plus Spectrogram sum
plus Spectrogram difference
plus Spectrogram average
Remove

#
# FORMANT ANALYSIS
#
# The formant analysis should give the exact same result for the two channels,
# because the two channels are averaged before the LPC analysis is performed.
#

select Sound chan1
chan1.Formant = To Formant (burg)... 0.01 5 5500 0.025 50.0

select Sound chan2
chan2.Formant = To Formant (burg)... 0.01 5 5500 0.025 50.0
assert objectsAreIdentical (chan2.Formant, chan1.Formant)

select Formant chan1
plus Formant chan2
Remove

#
# LPC ANALYSIS
#
# The LPC analysis should give the exact same result for the two channels,
# because the two channels are averaged before the LPC analysis is performed.
#
# Before some date in 2025 (?), the LPC algorithms took only channel 1 into account.
#

select Sound chan1
chan1.LPC = To LPC (burg)... 16 0.025 0.005 50.0

select Sound chan2
chan2.LPC = To LPC (burg)... 16 0.025 0.005 50.0
if praatVersion >= 6447
	assert objectsAreIdentical (chan2.LPC, chan1.LPC)   ; correct since October 2025
else
	assert not objectsAreIdentical (chan2.LPC, chan1.LPC)   ; bug from 2009 to 2025
endif

select LPC chan1
plus LPC chan2
Remove

select Sound chan1
chan1.LPC = To LPC (autocorrelation)... 16 0.025 0.005 50.0

select Sound chan2
chan2.LPC = To LPC (autocorrelation)... 16 0.025 0.005 50.0
if praatVersion >= 6447
	assert objectsAreIdentical (chan2.LPC, chan1.LPC)   ; correct since October 2025
else
	assert not objectsAreIdentical (chan2.LPC, chan1.LPC)   ; bug from 2009 to 2025
endif

select LPC chan1
plus LPC chan2
Remove

#
# CEPSTROGRAM ANALYSIS
#
# The cepstrogram analysis should give the exact same result for the two channels,
# because averaging is done at the level of the power spectral density.
#
# Also, the sum and difference sounds should give the exact same results as each other,
# because averaging is done after squaring.
#
# Before some date in 2025 (?), the algorithm took only channel 1 into account.
#

if praatVersion >= 5353   ; PowerCepstrogram available since July 2013
	select Sound chan1
	chan1.PowerCepstrogram = To PowerCepstrogram... 60.0 0.002 5000.0 50.0

	select Sound chan2
	chan2.PowerCepstrogram = To PowerCepstrogram... 60.0 0.002 5000.0 50.0
	if praatVersion >= 9999
		assert objectsAreIdentical (chan2.PowerCepstrogram, chan1.PowerCepstrogram)   ; when will this happen?
	else
		assert not objectsAreIdentical (chan2.PowerCepstrogram, chan1.PowerCepstrogram)   ; bug
	endif

	select Sound sum
	sum.PowerCepstrogram = To PowerCepstrogram... 60.0 0.002 5000.0 50.0
	if praatVersion >= 9999
		assert not objectsAreIdentical (sum.PowerCepstrogram, chan1.PowerCepstrogram)   ; more power
	else
		assert objectsAreIdentical (sum.PowerCepstrogram, chan1.PowerCepstrogram)   ; bug (only channel 1)
	endif

	select Sound difference
	difference.PowerCepstrogram = To PowerCepstrogram... 60.0 0.002 5000.0 50.0
	assert objectsAreIdentical (difference.PowerCepstrogram, sum.PowerCepstrogram)

	select PowerCepstrogram sum
	Formula... 0.5 * self
	assert not objectsAreIdentical (sum.PowerCepstrogram, chan1.PowerCepstrogram)

	select Sound average
	average.PowerCepstrogram = To PowerCepstrogram... 60.0 0.002 5000.0 50.0
	assert not objectsAreIdentical (average.PowerCepstrogram, chan1.PowerCepstrogram)
	Formula... 2.0 * self
	assert not objectsAreIdentical (average.PowerCepstrogram, chan1.PowerCepstrogram)

	select PowerCepstrogram chan1
	plus PowerCepstrogram chan2
	plus PowerCepstrogram sum
	plus PowerCepstrogram difference
	plus PowerCepstrogram average
	Remove
endif

#
# CLEAN UP
#

select Sound chan1
plus Sound chan2
plus Sound sum
plus Sound difference
plus Sound average
Remove