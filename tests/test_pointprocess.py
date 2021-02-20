import pytest
import numpy as np

import parselmouth


@pytest.fixture
def voice_path(resources):
    yield resources["eee.wav"]


@pytest.fixture
def voice(voice_path):
    yield parselmouth.read(voice_path)


@pytest.fixture
def voice_pitch(voice):
    return voice.to_pitch()


@pytest.fixture
def voice_process(voice_pitch):
    return parselmouth.PointProcess(voice_pitch)


@pytest.fixture
def empty_process():
    return parselmouth.PointProcess()


@pytest.fixture
def poisson_process():
    return parselmouth.PointProcess.create_poisson_process()


@pytest.fixture
def seqA():
    return [0.5, 0.6]


@pytest.fixture
def processA(seqA):
    """Returns PointProcess A=[0.5,0.6]"""
    processA = parselmouth.PointProcess()
    for t in seqA:
        processA.add_point(t)
    return processA


@pytest.fixture
def seqB():
    return [0.4, 0.6]


@pytest.fixture
def processB(seqB):
    """Returns PointProcesses B=[0.4,0.6]"""
    processB = parselmouth.PointProcess()
    processB.add_points(seqB)
    return processB


@pytest.fixture
def process_set(processA, processB):
    """Returns PointProcesses A=[0.5,0.6] and B=[0.4,0.6]"""
    return processA, processB


# def(py::init([](Pitch pitch) {}), "pitch"_a);

# def(py::init([](double startTime, double endTime) { }), "start_time"_a = 0.0, "end_time"_a = 1.0);
# def(py::init([](Sound sound, Pitch pitch, py::kwargs kwargs) { }), "sound"_a, "pitch"_a);


def test_create_empty_process(empty_process):
    # empty constructor test
    assert type(empty_process) == parselmouth.PointProcess
    with pytest.raises(parselmouth.PraatError):
        parselmouth.PointProcess(1.2)
    parselmouth.PointProcess(0.1, end_time=1.2)


def test_create_process():
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


def test_create_poisson_process(poisson_process):
    assert type(poisson_process) == parselmouth.PointProcess


def test_from_pitch(voice_pitch):
    # tests both constructor and static from_pitch()
    parselmouth.PointProcess(voice_pitch)
    parselmouth.PointProcess.from_pitch(voice_pitch)


def test_from_sound_pitch(voice, voice_pitch):
    # constructor from sound & pitch
    parselmouth.PointProcess(voice, voice_pitch)
    parselmouth.PointProcess(voice, voice_pitch, method="cc")
    parselmouth.PointProcess(voice, voice_pitch, method="peaks")
    with pytest.raises(ValueError):
        parselmouth.PointProcess(voice, voice_pitch, method="invalid")


def test_from_sound_pitch_cc(voice, voice_pitch):
    parselmouth.PointProcess.from_sound_pitch_cc(voice, voice_pitch)


def test_from_sound_pitch_peaks(voice, voice_pitch):
    parselmouth.PointProcess.from_sound_pitch_peaks(voice, voice_pitch)


def test_get_number_of_periods(voice_process):
    voice_process.get_number_of_periods()


def test_sequence(seqA, processA):
    """test compatibility with various sequence operations"""

    N = len(processA)
    assert N == 2
    for i in range(N):
        assert processA[i] == seqA[i]
    assert np.intersect1d([t for t in processA], seqA).size == N


def test_get_time_from_index(voice_process):
    x = np.array(voice_process)
    index = x.size // 3
    xi = voice_process.get_time_from_index(index + 1)
    assert x[index] == xi


def test_get_jitter_local(voice_process):
    voice_process.get_jitter_local()


def test_get_jitter_local_absolute(voice_process):
    voice_process.get_jitter_local_absolute()


def test_get_jitter_rap(voice_process):
    voice_process.get_jitter_rap()


def test_get_jitter_ppq5(voice_process):
    voice_process.get_jitter_ppq5()


def test_get_jitter_ddp(voice_process):
    voice_process.get_jitter_ddp()


def test_get_shimmer_local(voice, voice_process):
    voice_process.get_shimmer_local(voice)


def test_get_shimmer_local_dB(voice, voice_process):
    voice_process.get_shimmer_local_dB(voice)


def test_get_shimmer_local_apq3(voice, voice_process):
    voice_process.get_shimmer_local_apq3(voice)


def test_get_shimmer_local_apq5(voice, voice_process):
    voice_process.get_shimmer_local_apq5(voice)


def test_get_shimmer_local_apq11(voice, voice_process):
    voice_process.get_shimmer_local_apq11(voice)


def test_get_shimmer_local_dda(voice, voice_process):
    voice_process.get_shimmer_local_dda(voice)


def test_get_count_and_fraction_of_voice_breaks(voice_process):
    assert len(voice_process.get_count_and_fraction_of_voice_breaks()) == 4


def test_get_window_points(voice_process):
    t = np.array(voice_process)
    N = t.size
    i0 = N // 3
    i1 = N * 2 // 3
    t0 = (t[i0] + t[i0 - 1]) / 2.0
    t1 = (t[i1] + t[i1 + 1]) / 2.0
    x0, x1 = voice_process.get_window_points(t0, t1)
    assert i0 + 1 == x0 and i1 + 1 == x1


def test_get_low_index(voice_process):
    voice_process.get_low_index(0.5)


def test_get_high_index(voice_process):
    voice_process.get_high_index(0.5)


def test_get_nearest_index(voice_process):
    voice_process.get_nearest_index(0.5)


def test_get_interval(voice_process):
    voice_process.get_interval(0.5)


def test_get_mean_period(voice_process):
    voice_process.get_mean_period()


def test_get_stdev_period(voice_process):
    voice_process.get_stdev_period()


def test_add_point(processA):
    print(processA)  # fixture uses add_point in computing A


def test_add_points(processB):
    print(processB)  # fixture uses add_points in computing B


def test_remove_point(processA):
    processA.remove_point(1)
    assert np.all(np.isin(processA, [0.6]))


def test_remove_point_near(processA):
    processA.remove_point_near(0.49)
    assert np.all(np.isin(processA, [0.6]))


def test_remove_points(voice_process):
    x = np.array(voice_process)
    N = x.size
    range = np.array([N // 3, N * 2 // 3])
    mask = np.ones(N, bool)
    mask[range[0] : range[1]] = False
    y = x[mask]
    voice_process.remove_points(range[0] + 1, range[1])
    assert np.intersect1d(voice_process, y).size == y.size


def test_remove_points_between(voice_process):
    x = np.array(voice_process)
    N = x.size
    range = np.array([N // 3, N * 2 // 3])
    mask = np.ones(N, bool)
    mask[range[0] : range[1]] = False
    y = x[mask]
    voice_process.remove_points_between(x[range[0]], x[range[1] - 1])
    assert np.intersect1d(voice_process, y).size == y.size


def test_union(process_set):
    A, B = process_set
    C = A.union(B)
    assert np.all(np.isin(C, [0.4, 0.5, 0.6]))


def test_intersection(process_set):
    A, B = process_set
    C = A.intersection(B)
    assert np.all(np.isin(C, [0.6]))


def test_difference(process_set):
    A, B = process_set
    C = A.difference(B)
    assert np.all(np.isin(C, [0.5]))


def test_fill(empty_process):
    empty_process.fill(0.0, 1.0)


def test_voice(empty_process):
    empty_process.voice()


def test_get_number_of_points(processA):
    assert processA.get_number_of_points() == 2
