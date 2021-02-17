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
    print(vowel)

    # get pitch data
    pitch = vowel.to_pitch_ac()
    print(f"Mean autocorrelation={pitch.get_mean_strength(type='ac')}")
    print(f"Mean noise-to-harmonics ratio={pitch.get_mean_strength(type='nhr')}")
    print(f"Mean harmonics-to-noise ratio={pitch.get_mean_strength(type='hnr_db')} dB")
    print(f"default={pitch.get_mean_strength()}")


if __name__ == "__main__":
    test_pitch_harmonicity(resources)
