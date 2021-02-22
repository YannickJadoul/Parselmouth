import pytest

def test_get_mean_strength(pitch):
    print(f"Mean autocorrelation={pitch.get_mean_strength(type='ac')}")
    print(f"Mean noise-to-harmonics ratio={pitch.get_mean_strength(type='nhr')}")
    print(f"Mean harmonics-to-noise ratio={pitch.get_mean_strength(type='hnr_db')} dB")
    print(f"default={pitch.get_mean_strength()}")

def test_get_mean(pitch):
    print(f"Mean frequency={pitch.get_mean()}")

def test_get_standard_deviation(pitch):
    print(f"Standard_deviation={pitch.get_standard_deviation()}")

def test_get_minimum(pitch):
    print(f"Minimum={pitch.get_minimum()}")

def test_get_maximum(pitch):
    print(f"Maximum={pitch.get_maximum()}")

def test_get_quantile(pitch):
    print(f"10% Quantile={pitch.get_quantile(0.1)}")

def test_get_fraction_of_locally_unvoiced_frames(pitch):
    print(pitch.get_fraction_of_locally_unvoiced_frames())