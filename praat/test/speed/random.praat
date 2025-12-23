stopwatch

sound1000 = Create Sound from formula: "sineWithNoise", 1, 0, 1000, 44100, ~ randomGauss (0.0, 1.0)

writeInfoLine: stopwatch

a# = randomGauss# (44100 * 1000, 0.0, 1.0)

appendInfoLine: stopwatch

removeObject: sound1000

