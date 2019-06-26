Installation
============

Basics
------

Parselmouth can be installed like any other Python library, using (a recent version of) the Python package manager ``pip``, on Linux, macOS, and Windows::

    pip install praat-parselmouth

or, to update your installed version to the latest release::

    pip install -U praat-parselmouth

.. warning::

    While the Python module itself is called ``parselmouth``, the Parselmouth package on the Python Package Index has the name ``praat-parselmouth``.

.. note::

    To figure out if you can or should update, the version number of your current Parselmouth installation, can be found in the ``parselmouth.__version__`` variable.


Python distributions
--------------------

Anaconda
    If you use the Anaconda distribution of Python, you can use the same ``pip`` command in a terminal of the appropriate Anaconda environment, either activated through the `Anaconda Navigator <https://docs.continuum.io/anaconda/navigator/tutorials/manage-environments/#using-an-environment>`_ or `conda tool <https://conda.io/projects/continuumio-conda/en/latest/user-guide/tasks/manage-environments.html#activating-an-environment>`_.

Homebrew & MacPorts
    We currently do not have Homebrew or MacPorts packages to install Parselmouth. As far as we know however, Parselmouth can just be installed with the accompanying ``pip`` of these distributions.

PyPy
    In priciple, recent versions of PyPy are supported by the `pybind11 project <https://github.com/pybind/pybind11>`_ and should thus also be supported by Parselmouth. However, we currently have not figured out how to provide precompiled packages, so you will have to still compile the wheel yourself (or contribute an automated way of doing this to the project!).

Other
    For other distributions of Python, we are expecting that our package is compatible with the Python versions that are out there and that ``pip`` can handle the installation. If you are using yet another Python distribution, we are definitely interested in hearing about it, so that we can add it to this list!



PsychoPy
--------

As a Python library Parselmouth is perfect to be used within a PsychoPy experiment. There two different ways in which `PsychoPy can be installed <https://www.psychopy.org/installation.html>`_: it can just be manually installed as a standard Python library, in which case Parselmouth can just be installed next to it with ``pip``. For Windows and Mac OS X, however, *standalone* versions of PsychoPy exist, and the software does currently not allow for external libraries to be installed with ``pip``. These steps can be followed to install Parselmouth in a standalone PsychoPy:

1. Go to https://pypi.org/project/praat-parselmouth/.
2. Download the file ``praat_parselmouth-x.y.z-cp27-cp27m-win32.whl`` *(for Windows)* or ``praat_parselmouth-x.y.z-cp27-cp27m-macosx_10_6_intel.whl`` *(for Mac OS X)* - where x.y.z will be the latest released version of Parselmouth. Be sure to find the right file in the list, containing both ``cp27``, and ``win32`` *(Windows)* or ``macos`` *(Mac OS X)*  in its name!
3. Rename the downloaded file by replacing the ``.whl`` extension by ``.zip``.
4. Extract this zip archive somewhere on your computer, in your directory of choice. Remember the name and location of the extracted folder that contains the file ``parselmouth.pyd`` *(Windows)* or ``parselmouth.so`` *(Mac OS X)*.
5. Open PsychoPy, open the *Preferences* window, go to the *General* tab.
6. In the *General* tab of the PsychoPy *Preferences*, in the *paths* field, add the folder where you just extracted the Parselmouth library to the list, enclosing the path in quotemarks. (On *Windows*, also replace all ``\`` charachters by ``/``.)

    * For example, if the list was empty (``[]``), you could make it look like ``['C:/Users/Yannick/Parselmouth-0.1.1/']`` or ``['/Users/yannick/Parselmouth-0.1.1/']``.
    * On *Windows*, to find the right location to enter in the PsychoPy settings, right click ``parselmouth.pyd``, choose *Properties*, and look at the *Location* field.
    * On *Mac OS X*, to find the right location to enter in the PsychoPy settings, right click ``parselmouth.so``, choose *Get info*, and look at the *where* field.
    * On *Mac OS X*, dragging the folder into a terminal window will also give you the full path, with slashes.

7. Click *Ok* to save the PsychoPy settings and close the *Preferences* window.
8. *Optional*: if you want to check if Parselmouth was installed correctly, open the PsychoPy Coder interface, open the *Shell* tab, and type ``import parselmouth``.

    * If this results in an error message, please let us know, and we'll try to help you fix what went wrong!
    * If this does not give you an error, congratulations, you can now use Parselmouth in your PsychoPy Builder!

.. note::

    These instructions were tested with the ``StandalonePsychoPy-1.85.2-win32.exe`` and ``StandalonePsychoPy-1.85.2-OSX_64bit.dmg`` version downloaded from https://www.psychopy.org/installation.html.


Troubleshooting
---------------

It is possible that you run into more problems when trying to install or use Parselmouth. Supporting all of the different Python versions out there is not an easy job, as there are plenty of different platforms and setups.

If you run into problems and these common solutions are not solving them, please drop by the `Gitter chat room <https://gitter.im/PraatParselmouth/Lobby>`_, write a message in the `Google discussion group <https://groups.google.com/d/forum/parselmouth>`_, create a `GitHub issue <https://github.com/YannickJadoul/Parselmouth/issues>`_, or write `me <mailto:Yannick.Jadoul@ai.vub.ac.be>`_ a quick email. We would be very happy to solve these problems, so that future users can avoid them!


Multiple Python versions
^^^^^^^^^^^^^^^^^^^^^^^^

In case you have multiple installations of Python and don't know which ``pip`` belongs to which Python version *(looking at you, OS X)*::

    python -m pip install praat-parselmouth

Finding out the exact location of the ``python`` executable (to call the previous command) for a certain Python installation can be done by typing the following lines in your Python interpreter::

    >>> import sys
    >>> print(sys.executable)

If executing this in your Python shell would for example print ``/usr/bin/python``, then you would run ``/usr/bin/python -m pip install praat-parselmouth`` in a terminal to install Parselmouth. (``-U`` can again be added to update an already installation to the latest version.)

Combining these two approaches, you can install Parselmouth from within Python itself without knowing where that version of Python is installed::

    >>> import sys, subprocess
    >>> subprocess.call([sys.executable, '-m', 'pip', 'install', 'praat-parselmouth'])

Extra arguments to ``pip`` can be added by inserting them as strings into the list of arguments passed to ``subprocess.call`` (e.g., to update an existing installation of Parselmouth: ``[..., 'install', '-U', 'praat-parselmouth']``).


Pip version
^^^^^^^^^^^

If the standard way to install Parselmouth results in an error or takes a long time, try updating ``pip`` to the latest version (as ``pip`` needs to be a reasonably recent version to install the binary, precompiled wheels) by running ::

    pip install -U pip

If you do not have ``pip`` installed, you follow these instructions to install pip: https://pip.pypa.io/en/stable/installing/


``ImportError: DLL load failed`` on Windows
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes on Windows, the installation works, but importing Parselmouth fails with an error message saying ``ImportError: DLL load failed: The specified module could not be found.``. This error is cause by some missing system files, but can luckily be solved quite easily by installing the "Microsoft Visual C++ Redistributable for Visual Studio 2017".

The "Microsoft Visual C++ Redistributable for Visual Studio 2017" installer can be downloaded from `Microsoft's website <https://visualstudio.microsoft.com/downloads/>`_, listed under the "Other Tools and Frameworks" section. These are the direct download links to the relevant files:

- For a 64-bit Python installation: https://aka.ms/vs/15/release/VC_redist.x64.exe
- For a 32-bit Python installation: https://aka.ms/vs/15/release/VC_redist.x86.exe

To check which Python version you are using, you can look at the first line of output when starting a Python shell. The version information should contain ``[MSC v.xxxx 64 bit (AMD64)]`` in a 64-bit installation, or ``[MSC v.xxxx 32 bit (Intel)]`` in a 32-bit installation.

