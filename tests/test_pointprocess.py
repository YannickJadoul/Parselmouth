from os import path
import sys

rootdir = path.dirname(path.dirname(__file__))
sys.path.append(path.join(rootdir, "build", "src"))

import parselmouth as pm
import numpy as np

wavfile = "tests/data/the_north_wind_and_the_sun.wav"

# read speech data from test WAV file
sound_all = pm.Sound(wavfile)

# focus on a vowel
sound = sound_all.extract_part(1.086, 1.136)
print(sound)

# get pitch data
pitch = sound.to_pitch_ac()
print(pitch)

# get pulses from sound & pitch data
pulses = pm.PointProcess.from_sound_pitch_cc(sound, pitch)
print(pulses)

# various get functions
print(f"get_number_of_points={pulses.get_number_of_points()}")
print(f"get_number_of_periods={pulses.get_number_of_periods()}")

nt = pulses._nt
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

