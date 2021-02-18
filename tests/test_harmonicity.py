import numpy as np

if __name__ == "__main__":
    from os import path
    import sys

    rootdir = path.dirname(path.dirname(__file__))
    sys.path.append(path.join(rootdir, "build", "src"))
    resources = {
        "the_north_wind_and_the_sun.wav": "tests/data/the_north_wind_and_the_sun.wav"
    }

import parselmouth


def test_pitch_harmonicity(resources):
    sound = parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"])
    vowel = sound.extract_part(1.086, 1.136)
    
    # get pitch data
    pitch = vowel.to_pitch_ac()
    print(f"Mean autocorrelation={pitch.get_mean_strength(type='ac')}")
    print(f"Mean noise-to-harmonics ratio={pitch.get_mean_strength(type='nhr')}")
    print(f"Mean harmonics-to-noise ratio={pitch.get_mean_strength(type='hnr_db')} dB")
    print(f"default={pitch.get_mean_strength()}")

def test_harmonicity(resources):
    sound = parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"])
    vowel = sound.extract_part(1.086, 1.136)
    
    # get harmonicity data
    harmonicity = vowel.to_harmonicity("CC") # "ac", "gne"
    print(f"Median={harmonicity.get_quantile(0.5)} dB")
    print(f"   10 %={harmonicity.get_quantile(0.1)} dB")
    print(f"   16 %={harmonicity.get_quantile(0.16)} dB")
    print(f"   25 %={harmonicity.get_quantile(0.25)} dB")
    print(f"   75 %={harmonicity.get_quantile(0.75)} dB")
    print(f"   84 %={harmonicity.get_quantile(0.84)} dB")
    print(f"   90 %={harmonicity.get_quantile(0.90)} dB")
    print(f"Minimum={harmonicity.get_minimum()} dB @ {harmonicity.get_time_of_minimum()}")
    print(f"Maximum={harmonicity.get_maximum()} dB @ {harmonicity.get_time_of_maximum()}")
    print(f"Average={harmonicity.get_mean()} dB")
    print(f"Maximum={harmonicity.get_standard_deviation()} dB")

if __name__ == "__main__":
    test_pitch_harmonicity(resources)
    test_harmonicity(resources)
