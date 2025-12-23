# test/manually/cacheBlock.praat
# Paul Boersma, 13 August 2025
# 21 September 2025: accommodate API change

timeStep = 0.001 ; seconds
samplingFrequency = 1e8   ; 100 MHz: several 10-MB buffers
writeInfoLine: "Trying to cause cache misses..."
margin = 0.0399 ; seconds

#
# Almost equally fast:
#
;Debug multi-threading: "yes", 16, 0, "no"
Debug multi-threading: "yes", 6, 0, "no"
;Debug multi-threading: "yes", 4, 0, "no"
# slower:
;Debug multi-threading: "yes", 1, 0, "no"

;sound100 = Create Sound from formula: "sineWithNoise", 1, 0, 100 + margin, samplingFrequency, "1/2 * sin (2*pi*377*x) + randomGauss (0, 0.1)"
sound10 = Create Sound from formula: "sineWithNoise", 1, 0, 10 + margin, samplingFrequency, "1/2 * sin (2*pi*377*x) + randomGauss (0, 0.1)"
sound1 = Create Sound from formula: "sineWithNoise", 1, 0, 1 + margin, samplingFrequency, "1/2 * sin (2*pi*377*x) + randomGauss (0, 0.1)"
sound01 = Create Sound from formula: "sineWithNoise", 1, 0, 0.1 + margin, samplingFrequency, "1/2 * sin (2*pi*377*x) + randomGauss (0, 0.1)"
appendInfoLine: "sounds created"

;selectObject: sound100
;stopwatch
;pitch100 = To Pitch (raw autocorrelation): timeStep, 75, 600, 15, "no", 0.03, 0.45, 0.01, 0.35, 0.14
;appendInfoLine: stopwatch

selectObject: sound10
stopwatch
pitch10 = To Pitch (raw autocorrelation): timeStep, 75, 600, 15, "no", 0.03, 0.45, 0.01, 0.35, 0.14
appendInfoLine: stopwatch

selectObject: sound1
stopwatch
pitch1 = To Pitch (raw autocorrelation): timeStep, 75, 600, 15, "no", 0.03, 0.45, 0.01, 0.35, 0.14
appendInfoLine: stopwatch

selectObject: sound01
stopwatch
pitch01 = To Pitch (raw autocorrelation): timeStep, 75, 600, 15, "no", 0.03, 0.45, 0.01, 0.35, 0.14
appendInfoLine: stopwatch

removeObject: sound01, sound1, sound10, pitch01, pitch1, pitch10

Debug multi-threading: "yes", 0, 0, "no"
