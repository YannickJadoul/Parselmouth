# Parselmouth - Praat in Python, the Pythonic way
**Parselmouth** is a Python library for the [Praat](http://www.praat.org) software.

Though other attempts have been made at porting functionality from Praat to Python, Parselmouth is unique in its aim to provide a complete and Pythonic interface to the internal Praat code. While other projects either wrap Praat's scripting language or reimplementing parts of Praat's functionality in Python, Parselmouth directly accesses Praat's C/C++ code (which means the algorithms and their output are exactly the same as in Praat) and provides efficient access to the program's data, but *also* provides an interface that looks no different from any other Python library.

Please note that Parselmouth is currently in premature state and in active development. While the amount of functionality that is currently present is not huge, more will be added over the next few months.

Drop by our [Gitter chat room](https://gitter.im/PraatParselmouth/Lobby), if you have any question, remarks, or requests!



## Installation
Parselmouth can be installed like any other Python library, using (a recent version of) the Python package manager `pip`:
```
pip install praat-parselmouth
```
or, in case you have multiple installations of Python and don't know which `pip` belongs to which Python version *(looking at you, OS X)*:
```python
import pip
pip.main(['install', 'praat-parselmouth'])
```

If this results in an error, try updating `pip` to the latest version by running
```
pip install -U pip
```
If you do not have `pip` installed, you follow these instructions to install pip: https://pip.pypa.io/en/stable/installing/

### Troubleshooting
Since the project is still in an early development phase, it is possible that you run into more problems when trying to install or use Parselmouth. If you would do so after trying this, please drop by the [Gitter chat room](https://gitter.im/PraatParselmouth/Lobby), log a GitHub issue, or write [me](mailto:Yannick.Jadoul@ai.vub.ac.be) a quick email. We are of course very grateful for you feedback!

## Example usage
```Python
import parselmouth

import numpy as np
import matplotlib.pyplot as plt
import seaborn

# Plot nice figures using Python’s “standard” matplotlib library
snd = parselmouth.Sound("~/z6a.WAVE")
max_t = snd.num_samples / snd.sampling_frequency
plt.plot(np.linspace(0, max_t, snd.num_samples), snd.values)
plt.xlim([0, max_t])
plt.xlabel("time [s]")
plt.ylabel("amplitude")
plt.show() # or plt.savefig("sound.pdf)
```
![example_sound.png](res/images/example_sound.png)
```Python
def draw_spectrogram(spectrogram, max_t, max_f=5000, dynamic_range=70):
    X = np.linspace(0, max_t, spectrogram.values.shape[0])
    Y = np.linspace(0, max_f, spectrogram.values.shape[1])
    spectrogram_db = 10 * np.log10(spectrogram.values.T)
    plt.pcolormesh(X, Y, sg_db, vmin=spectrogram_db.max() - dynamic_range, cmap='afmhot')
    plt.xlabel("time [s]")
    plt.ylabel("frequency [Hz]")

def draw_intensity(intensity, max_t):
    plt.plot(np.linspace(0, max_t, intensity.values.shape[0]), intensity.values, linewidth=3, color='w')
    plt.plot(np.linspace(0, max_t, intensity.values.shape[0]), intensity.values, linewidth=1)
    plt.grid(False)
    plt.xlim([0, max_t])
    plt.ylim(0)
    plt.ylabel("intensity [dB]")

intensity = snd.to_intensity()

spectrogram = snd.to_spectrogram()
draw_spectrogram(spectrogram, max_t)
plt.twinx()
draw_intensity(intensity, max_t)
plt.show() # or plt.savefig("spectrogram.pdf)
```
![example_spectrogram.png](res/images/example_spectrogram.png)
```Python
spectrogram = snd.to_spectrogram(window_length=0.05)
draw_spectrogram(spectrogram, max_t)
plt.show() # or plt.savefig("spectrogram_0.05.pdf)
```
![example_spectrogram_0.05.png](res/images/example_spectrogram_0.05.png)
```Python
# Find all .wav files in a directory, pre-emphasize and save as new .wav and .aiff file
import glob
import os.path
from parselmouth import SoundFileFormat

for wave_file in glob.glob('/home/yannick/*.wav'):
    s = parselmouth.Sound(wave_file)
    s_pre = s.pre_emphasize()
    s.save(os.path.splitext(wave_file)[0] + '_pre.wav', SoundFileFormat.WAV)
    s.save(os.path.splitext(wave_file)[0] + '_pre.aiff', SoundFileFormat.AIFF)
```

## Documentation
Though it is rather ugly and little for the moment, until more work will be done on this, the existing API documentation can be found [here](http://ai.vub.ac.be/~yajadoul/parselmouth.html).

## Development
Currently, the actual project and Parselmouth's code is not very well documented. Or well,  hardly documented at all. That is planned to still change in order to allow for easier contribution to this open source project.

Briefly summarized, Parselmouth is built using [`cmake`](https://cmake.org/). Next to that, to manually build Parselmouth, the only requirement is a modern C++ compiler supporting the C++14 standard.

## Acknowledgements
- Parselmouth builds on the extensive code base of [Praat](https://github.com/praat/praat) by Paul Boersma, which actually implements the huge variety of speech processing and phonetic algorithms that can now be accessed through Parselmouth.
- In order to do so, Parselmouth makes use of the amazing [pybind11](https://github.com/pybind/pybind11) library, allowing expose the C/C++ functionality of Praat as a Python interface.
- Special thanks go to [Bill Thompson](https://github.com/billdthompson) and [Robin Jadoul](https://github.com/RobinJadoul/) for their non-visible-in-history but very valuable contributions.

## License
Parselmouth is released under the GNU General Public License, version 3 or later. See [the `LICENSE` file](LICENSE) for details.

[Praat](https://github.com/praat/praat) is released under [the GNU General Public License, version 2 or later](praat/main/GNU_General_Public_License.txt). Small changes to this code base, made in the context of Parselmouth, can be found within the `git` history.

[pybind11](https://github.com/pybind/pybind11) is released under [a BSD-style license](pybind11/LICENSE).
