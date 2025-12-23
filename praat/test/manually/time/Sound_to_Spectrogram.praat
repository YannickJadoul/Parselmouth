# test/manually/Sound_to_Spectrogram.praat
# Paul Boersma, 12 August 2025

timeStep = 0.001 ; seconds
sound1000 = Create Sound from formula: "sineWithNoise", 1, 0, 1000, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
stopwatch
spectrogram1000 = noprogress To Spectrogram: 0.005, 5000.0, timeStep, 20.0, "Gaussian"
writeInfoLine: stopwatch
sound100 = Create Sound from formula: "sineWithNoise", 1, 0, 100, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
stopwatch
spectrogram100 = noprogress To Spectrogram: 0.005, 5000.0, timeStep, 20.0, "Gaussian"
appendInfoLine: stopwatch
sound10 = Create Sound from formula: "sineWithNoise", 1, 0, 10, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
stopwatch
spectrogram10 = noprogress To Spectrogram: 0.005, 5000.0, timeStep, 20.0, "Gaussian"
appendInfoLine: stopwatch
sound1 = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
stopwatch
spectrogram1 = noprogress To Spectrogram: 0.005, 5000.0, timeStep, 20.0, "Gaussian"
appendInfoLine: stopwatch
sound01 = Create Sound from formula: "sineWithNoise", 1, 0, 0.1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
stopwatch
spectrogram01 = noprogress To Spectrogram: 0.005, 5000.0, timeStep, 20.0, "Gaussian"
appendInfoLine: stopwatch

removeObject: sound01, sound1, sound10, sound100, sound1000, spectrogram01, spectrogram1, spectrogram10, spectrogram100, spectrogram1000

margin = 0.0095 ; seconds
numberOfReplications = 100
maximumNumberOfFrames = 100
numberOfTimings = numberOfReplications * maximumNumberOfFrames
time# = zero# (maximumNumberOfFrames)
numbersOfFrames# = shuffle#: repeat# (to# (maximumNumberOfFrames), numberOfReplications)
for timing to numberOfTimings 
	desiredNumberOfFrames = numbersOfFrames# [timing]
	durationOfFrames = desiredNumberOfFrames * timeStep
	durationOfSound = durationOfFrames + margin
	sound = Create Sound from formula: "sineWithNoise", 1, 0, durationOfSound, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
	stopwatch
	spectrogram = noprogress To Spectrogram: 0.005, 5000.0, timeStep, 20.0, "Gaussian"
	time# [desiredNumberOfFrames] += stopwatch
	resultingNumberOfFrames = Get number of frames
	assert resultingNumberOfFrames = desiredNumberOfFrames   ; 'desiredNumberOfFrames' 'resultingNumberOfFrames'
	removeObject: sound, spectrogram
endfor

for numberOfFrames from 1 to maximumNumberOfFrames
	appendInfoLine: numberOfFrames, " ", round (time# [numberOfFrames] / numberOfReplications * 1e6), "μ"
endfor

appendInfoLine: "Mean: ", round (mean (time#) / numberOfReplications * 1e6), "μ"