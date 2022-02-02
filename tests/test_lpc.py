import parselmouth

import numpy as np


def test_lpc(sound):
    lpc = sound.to_lpc_autocorrelation()
    lpc = sound.to_lpc_covariance()
    lpc = sound.to_lpc_burg()
    lpc = sound.to_lpc_marple()
    print(lpc)

    print(f"time_step={lpc.dt} s")
    print(f"time_frame_at={lpc.t1} s")
    print(f"num_frames={lpc.nt}")
    print(f"sampling_rate={1/lpc.sampling_period} samples/second")
    print(f"max_n_coefficients={lpc.max_n_coefficients}")
    lpc_frame = next(((frame) for frame in lpc))

    print(f"frame.n_coefficients={lpc_frame.n_coefficients}")
    print(f"frame.gain={lpc_frame.gain}")
    print(f"frame.a({lpc_frame.a.size}, {lpc_frame.a.dtype})={lpc_frame.a}")

    print(np.array([np.pad(frame.a, (0, lpc.max_n_coefficients-frame.n_coefficients), 'constant') for frame in lpc]).shape)

    # assert False