import pytest
import numpy as np

import parselmouth
from parselmouth.praat import call


def test_create_empty_process():
    assert parselmouth.PointProcess(0, 1) == parselmouth.PointProcess(0.0, 1.0)
    assert parselmouth.PointProcess(0, 1) != parselmouth.PointProcess(0, 2)
    with pytest.raises(parselmouth.PraatError, match=r'Your end time \(0\) should not be less than your start time \(1\).'):
        parselmouth.PointProcess(1.0, 0)
    assert parselmouth.PointProcess(end_time=1, start_time=0) == parselmouth.PointProcess(0, 1)


def test_create_process_from_array():
    # create a new process with prefilled values
    t = [0.1, 0.4, 1.2, 2.4, 0.5]
    process = parselmouth.PointProcess(t)
    assert process.xmin == min(t)
    assert process.xmax == max(t)
    assert np.array_equal(process, np.sort(t))

    # create another process but explicitly set the time domain
    process = parselmouth.PointProcess(t, start_time=0.0, end_time=1.0)
    assert process.xmin == 0.0
    assert process.xmax == 1.0
    assert np.array_equal(process, np.sort(t))


def test_create_poisson_process():
    poisson_process = parselmouth.PointProcess.create_poisson_process(0, 1, 100)
    assert isinstance(poisson_process, parselmouth.PointProcess)
    assert poisson_process != parselmouth.PointProcess.create_poisson_process(0, 1, 100)


def test_from_sound(sound):
    # tests both constructor and static from_pitch()
    sound.to_point_process_extrema("LEFT", True, False, "SINC70")
    sound.to_point_process_periodic(75.0, 600.0)
    sound.to_point_process_periodic_peaks(75.0, 600.0, True, False)


def test_from_pitch(pitch, sound):
    # tests both constructor and static from_pitch()
    pitch.to_point_process()
    pitch.to_point_process(sound)
    pitch.to_point_process(sound, method="cc")
    pitch.to_point_process(sound, method="peaks")
    with pytest.raises(ValueError):
        pitch.to_point_process(sound, method="invalid")

    pitch.to_point_process_cc(sound)
    pitch.to_point_process_peaks(sound, False, True)


def test_points(point_process):
    n = call(point_process, "Get number of points")
    assert point_process.get_number_of_points() == n
    assert point_process.n_points == n


def test_periods(point_process):
    # TODO PointProcess.get_periods() ?
    default_argument_values = (0.0, 0.0, 0.0001, 0.02, 1.3)
    assert point_process.get_number_of_periods() == call(point_process, "Get number of periods", *default_argument_values)
    assert point_process.get_mean_period() == call(point_process, "Get mean period", *default_argument_values)
    assert point_process.get_stdev_period() == call(point_process, "Get stdev period", *default_argument_values)


def test_sequence():
    sequence = sorted(np.random.random(10))
    point_process = parselmouth.PointProcess(sequence)
    assert len(point_process) == len(sequence)
    for i in range(len(sequence)):
        assert point_process[i] == sequence[i]
    assert set(point_process) == set(sequence)


def test_get_time_from_index(point_process):
    x = np.array(point_process)  # TODO PointProcess.values
    index = x.size // 3
    xi = point_process.get_time_from_index(index + 1)
    assert x[index] == xi


def test_get_jitters(point_process):
    def call_jitter(which):
        return call(point_process, f"Get jitter ({which})", 0, 0, 0.0001, 0.02, 1.3)

    assert point_process.get_jitter_local() == call_jitter("local")
    assert point_process.get_jitter_local_absolute() == call_jitter("local, absolute")
    assert point_process.get_jitter_rap() == call_jitter("rap")
    assert point_process.get_jitter_ppq5() == call_jitter("ppq5")
    assert point_process.get_jitter_ddp() == call_jitter("ddp")


def test_get_shimmers(sound, point_process):
    def call_shimmer(which):
        return call([point_process, sound], f"Get shimmer ({which})", 0, 0, 0.0001, 0.02, 1.3, 1.6)

    assert point_process.get_shimmer_local(sound) == call_shimmer("local")
    assert point_process.get_shimmer_local_dB(sound) == call_shimmer("local_dB")
    assert point_process.get_shimmer_apq3(sound) == call_shimmer("apq3")
    assert point_process.get_shimmer_apq5(sound) == call_shimmer("apq5")
    assert point_process.get_shimmer_apq11(sound) == call_shimmer("apq11")
    assert point_process.get_shimmer_dda(sound) == call_shimmer("dda")


def test_get_count_and_fraction_of_voice_breaks(point_process):
    assert len(point_process.get_count_and_fraction_of_voice_breaks()) == 4


def test_indexes(point_process):
    t, n = np.array(point_process), len(point_process)
    i0, i1 = n // 3, (2 * n) // 3
    t0, t1 = (t[i0] + t[i0 - 1]) / 2, (t[i1] + t[i1 + 1]) / 2
    j0, j1 = point_process.get_window_points(t0, t1)
    assert i0 + 1 == j0 and i1 + 1 == j1

    low, high = point_process.get_low_index(0.5), point_process.get_high_index(0.5)
    assert point_process.get_nearest_index(0.5) in [low, high]
    assert point_process.get_interval(0.5) == point_process[high-1] - point_process[low-1]


def test_modifications(point_process):
    n = len(point_process)
    point_process.add_point(np.random.uniform(point_process.tmin, point_process.tmax))
    assert len(point_process) == n + 1

    point_process.add_points(np.random.uniform(point_process.tmin, point_process.tmax, 9))
    assert len(point_process) == n + 10

    p = point_process[0]
    point_process.remove_point(1)
    assert len(point_process) == n + 9
    assert point_process[0] != p

    n = np.random.randint(1, len(point_process) - 1)
    p = point_process[n]
    point_process.remove_point_near((point_process[n-1] + 2 * p) / 3)
    assert not np.any(np.array(point_process) == p)

    n = len(point_process)
    i, j = n // 3, (2 * n) // 3
    remaining = np.delete(point_process, slice(i, j))
    point_process.remove_points(i + 1, j)
    assert set(point_process) == set(remaining)

    point_process.remove_points_between(point_process.centre_time, point_process.tmax)
    assert not np.any(np.array(point_process) > point_process.centre_time)


def test_combinations():
    a = np.arange(0.1, 0.9, 0.2)
    b = np.arange(0.1, 0.5, 0.1)
    point_process_a = parselmouth.PointProcess(a)
    point_process_b = parselmouth.PointProcess(b)

    union = point_process_a.union(point_process_b)
    assert np.array_equal(np.array(union), np.union1d(a, b))
    assert union.xmin == min(point_process_a.xmin, point_process_b.xmin)
    assert union.xmax == max(point_process_a.xmax, point_process_b.xmax)

    intersection = point_process_a.intersection(point_process_b)
    assert np.array_equal(np.array(intersection), np.intersect1d(a, b))
    assert intersection.xmin == max(point_process_a.xmin, point_process_b.xmin)
    assert intersection.xmax == min(point_process_a.xmax, point_process_b.xmax)

    difference = point_process_a.difference(point_process_b)
    assert np.array_equal(np.array(difference), np.setdiff1d(a, b))
    assert difference.xmin == point_process_a.xmin
    assert difference.xmax == point_process_a.xmax


def test_voice(point_process):
    new_point_process = parselmouth.PointProcess(0.0, 2.0)
    new_point_process.fill(0.5, 1.5, 0.01)
    assert len(new_point_process) == 100
    assert np.array_equal(np.array(new_point_process), np.arange(0.5, 1.5, 0.01))

    point_process.voice()
    _, voice_breaks_fraction, _, _ = point_process.get_count_and_fraction_of_voice_breaks()
    assert voice_breaks_fraction == 0
