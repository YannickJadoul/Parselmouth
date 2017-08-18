# Parselmouth - Praat in Python, the Pythonic way

[![PyPI](https://img.shields.io/pypi/v/praat-parselmouth.svg)](https://pypi.python.org/pypi/praat-parselmouth)
[![Gitter chat](https://img.shields.io/gitter/room/PraatParselmouth/Lobby.svg)](https://gitter.im/PraatParselmouth/Lobby)
[![Travis CI status](https://img.shields.io/travis/YannickJadoul/Parselmouth.svg)](https://travis-ci.org/YannickJadoul/Parselmouth/)
[![AppVeyor status](https://img.shields.io/appveyor/ci/YannickJadoul/Parselmouth.svg)](https://ci.appveyor.com/project/YannickJadoul/parselmouth)
[![License](https://img.shields.io/pypi/l/praat-parselmouth.svg)](https://github.com/YannickJadoul/Parselmouth/blob/master/LICENSE)

**Parselmouth** is a Python library for the [Praat](http://www.praat.org) software.

Though other attempts have been made at porting functionality from Praat to Python, Parselmouth is unique in its aim to provide a complete and Pythonic interface to the internal Praat code. While other projects either wrap Praat's scripting language or reimplementing parts of Praat's functionality in Python, Parselmouth directly accesses Praat's C/C++ code (which means the algorithms and their output are exactly the same as in Praat) and provides efficient access to the program's data, but *also* provides an interface that looks no different from any other Python library.

Please note that Parselmouth is currently in premature state and in active development. While the amount of functionality that is currently present is not huge, more will be added over the next few months.

Drop by our [Gitter chat room](https://gitter.im/PraatParselmouth/Lobby) if you have any question, remarks, or requests!

## Installation
Parselmouth can be installed like any other Python library, using (a recent version of) the Python package manager `pip`:
```
pip install praat-parselmouth
```
or, to update your installed version to the latest release:
```
pip install -U praat-parselmouth
```
*The version number of your current Parselmouth installation, can be found in the `parselmouth.__version__` variable.*

If you use the Anaconda distribution of Python, you can use the same `pip` command in a terminal of the appropriate Anaconda environment, either activated through the [Anaconda Navigator](https://docs.continuum.io/anaconda/navigator/tutorials/manage-environments#using-an-environment) or [conda tool](https://docs.continuum.io/docs_oss/conda/using/envs#change-environments-activate-deactivate).

In case you have multiple installations of Python and don't know which `pip` belongs to which Python version *(looking at you, OS X)*:
```Python
import pip
pip.main(['install', 'praat-parselmouth'])
```
*The latter approach for some unknown reason sometimes takes quite a lot of time. If this happens, you can either be patient, or you can try figuring out how to call pip immediately from the command line.*

- On Windows and Mac OS X, the installation should finish fairly quickly.
- On a Linux system, we do not have a binary distribution yet, but `pip install praat-parselmouth` should automatically build your project in 5 to 10 minutes if you have `cmake` installed.


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
seaborn.set() # Use seaborn's default style to make graphs more pretty

# Plot nice figures using Python's "standard" matplotlib library
snd = parselmouth.Sound("~/z6a.WAVE")
plt.figure()
plt.plot(snd.x1 + snd.dx * np.arange(snd.nx), snd.values)
plt.xlim([snd.xmin, snd.xmax])
plt.xlabel("time [s]")
plt.ylabel("amplitude")
plt.show() # or plt.savefig("sound.pdf")
```
![example_sound.png](res/images/example_sound.png)
```Python
def draw_spectrogram(spectrogram, dynamic_range=70):
    X = spectrogram.x1 + spectrogram.dx * (np.arange(spectrogram.nx + 1) - 0.5)
    Y = spectrogram.y1 + spectrogram.dy * (np.arange(spectrogram.ny + 1) - 0.5)
    sg_db = 10 * np.log10(spectrogram.values.T)
    plt.pcolormesh(X, Y, sg_db, vmin=sg_db.max() - dynamic_range, cmap='afmhot')
    plt.ylim([spectrogram.ymin, spectrogram.ymax])
    plt.xlabel("time [s]")
    plt.ylabel("frequency [Hz]")

def draw_intensity(intensity):
    plt.plot(intensity.x1 + intensity.dx * np.arange(intensity.nx), intensity.values, linewidth=3, color='w')
    plt.plot(intensity.x1 + intensity.dx * np.arange(intensity.nx), intensity.values, linewidth=1)
    plt.grid(False)
    plt.ylim(0)
    plt.ylabel("intensity [dB]")

intensity = snd.to_intensity()
spectrogram = snd.to_spectrogram()
plt.figure()
draw_spectrogram(spectrogram)
plt.twinx()
draw_intensity(intensity)
plt.xlim([snd.xmin, snd.xmax])
plt.show() # or plt.savefig("spectrogram.pdf")
```
![example_spectrogram.png](res/images/example_spectrogram.png)
```Python
spectrogram = snd.to_spectrogram(window_length=0.05)
plt.figure()
draw_spectrogram(spectrogram)
plt.xlim([snd.xmin, snd.xmax])
plt.show() # or plt.savefig("spectrogram_0.05.pdf")
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
- Special thanks go to [Bill Thompson](https://billdthompson.github.io/) and [Robin Jadoul](https://github.com/RobinJadoul/) for their non-visible-in-history but very valuable contributions.

## License
* Parselmouth is released under the GNU General Public License, version 3 or later. See [the `LICENSE` file](LICENSE) for details.

* [Praat](https://github.com/praat/praat) is released under [the GNU General Public License, version 2 or later](praat/main/GNU_General_Public_License.txt). Small changes to this code base, made in the context of Parselmouth, can be found within the `git` history.

  Parselmouth only exposes Praat's existing functionality and implementation of algorithms. If you use Parselmouth in your research and plan to cite it in a scientific publication, please do not forget to [*cite Praat*](http://www.fon.hum.uva.nl/praat/manual/FAQ__How_to_cite_Praat.html).

  > Boersma, Paul & Weenink, David (2017). Praat: doing phonetics by computer [Computer program]. Version 6.0.28, retrieved 23 March 2017 from [http://www.praat.org/](http://www.praat.org/)  

* [pybind11](https://github.com/pybind/pybind11) is released under [a BSD-style license](pybind11/LICENSE).
