import pytest

if __name__ == "__main__":
    from os import path
    import sys

    rootdir = path.dirname(path.dirname(__file__))
    sys.path.append(path.join(rootdir, "build", "src"))
    resources = {
        "the_north_wind_and_the_sun.wav": "tests/data/the_north_wind_and_the_sun.wav"
    }

import parselmouth


def test_pointprocess(resources):
    sound = parselmouth.Sound(resources["the_north_wind_and_the_sun.wav"])
    vowel = sound.extract_part(1.086, 1.136)
    print(vowel)

    # get pitch data
    pitch = vowel.to_pitch_ac()
    print(pitch)

    # get pulses from sound & pitch data
    pulses = parselmouth.PointProcess.from_sound_pitch_cc(vowel, pitch)
    print(pulses)

    # test sequence operations
    nt = pulses.get_number_of_points()
    assert len(pulses) == nt
    pulses[nt - 1]
    assert pulses[-1] == pulses[nt - 1]

    def iterate(P):
        for p in P:
            pass
        return True

    assert iterate(pulses)

    # various get functions
    print(f"get_number_of_points={pulses.get_number_of_points()}")
    print(f"get_number_of_periods={pulses.get_number_of_periods()}")

    i = max(nt // 2, 1)  # point number (1-based index of target time point)
    t = (pulses.get_time_from_index(i) + pulses.get_time_from_index(i + 1)) / 2
    print(f"get_time_from_index({i})={pulses.get_time_from_index(i)}")
    print(f"get_interval()={pulses.get_interval(t)}")
    print(f"get_low_index={pulses.get_low_index(t)}")
    print(f"get_high_index={pulses.get_high_index(t)}")
    print(f"get_nearest_index={pulses.get_nearest_index(t)}")

    # all these gets measurements over the entire sound
    print(f"get_jitter_local()={pulses.get_jitter_local()}")
    print(f"get_jitter_local_absolute={pulses.get_jitter_local_absolute()}")
    print(f"get_jitter_rap={pulses.get_jitter_rap()}")
    print(f"get_jitter_ppq5={pulses.get_jitter_ppq5()}")
    print(f"get_jitter_ddp={pulses.get_jitter_ddp()}")
    print(f"get_mean_period={pulses.get_mean_period()}")
    print(f"get_stdev_period={pulses.get_stdev_period()}")

    print(
        f"get_count_and_fraction_of_voice_breaks={pulses.get_count_and_fraction_of_voice_breaks()}"
    )
    print(f"get_shimmer_local={pulses.get_shimmer_local(vowel)}")
    print(f"get_shimmer_local_dB={pulses.get_shimmer_local_dB(vowel)}")
    print(f"get_shimmer_local_apq3={pulses.get_shimmer_local_apq3(vowel)}")
    print(f"get_shimmer_local_apq5={pulses.get_shimmer_local_apq5(vowel)}")
    print(f"get_shimmer_local_apq11={pulses.get_shimmer_local_apq11(vowel)}")
    print(f"get_shimmer_local_dda={pulses.get_shimmer_local_dda(vowel)}")


if __name__ == "__main__":
    test_pointprocess(resources)
