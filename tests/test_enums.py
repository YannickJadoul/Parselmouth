import numpy as np
import parselmouth as pm

def test_case(sound):
    pitch = sound.to_pitch("CC")
    pitch = sound.to_pitch("cc")
    