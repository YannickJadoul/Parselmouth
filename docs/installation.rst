Installation
============

Basics
------

Parselmouth can be installed like any other Python library, using (a recent version of) the Python package manager ``pip``, on Linux, macOS, and Windows::

    pip install praat-parselmouth

To update your installed version to the latest release, add ``-U`` (or ``--upgrade``) to the command::

    pip install -U praat-parselmouth

.. warning::

    While the Python module itself is called ``parselmouth``, the Parselmouth package on the Python Package Index has the name ``praat-parselmouth``.

.. note::

    To figure out if you can or should update, the version number of your current Parselmouth installation can be found in the `parselmouth.VERSION` variables. The version of Praat on which this version of Parselmouth is based and the release date of that Praat version are available as `~parselmouth.PRAAT_VERSION` and `~parselmouth.PRAAT_VERSION_DATE`, respectively.


Python distributions
--------------------

Anaconda
    If you use the Anaconda distribution of Python, you can use the same ``pip`` command in a terminal of the appropriate Anaconda environment, either activated through the `Anaconda Navigator <https://docs.continuum.io/free/navigator/tutorials/manage-environments/#using-an-environment>`_ or `conda tool <https://conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html#activating-an-environment>`_.

Homebrew & MacPorts
    We currently do not have Homebrew or MacPorts packages to install Parselmouth. Normally, Parselmouth can just be installed with the accompanying ``pip`` of these distributions.

PyPy
    Binary wheels for recent versions of `PyPy <https://www.pypy.org/>` are available on the Python Package Index (PyPI), and can be installed with ``pip``.

Other
    For other distributions of Python, we are expecting that our package is compatible with the Python versions that are out there and that ``pip`` can handle the installation. If you are using yet another Python distribution, we are definitely interested in hearing about it, so that we can add it to this list!


PsychoPy
--------

As a Python library, Parselmouth can be used in a PsychoPy experiment. There are two different ways in which `PsychoPy can be installed <https://www.psychopy.org/download.html>`_: it can just be manually installed as a standard Python library, in which case Parselmouth can just be installed next to it with ``pip``. For Windows and Mac OS X, however, *standalone* versions of PsychoPy exist, and the software does currently not allow for external libraries to be installed with ``pip``.

To install Parselmouth in a standalone version of PsychoPy, the following script can be opened and run from within the PsychoPy Coder interface: :download:`psychopy_installation.py`

.. note::

    If running the script results in an error mentioning ``TLSV1_ALERT_PROTOCOL_VERSION``, the version of PsychoPy/Python is too old and you will need to follow the manual instructions underneath.

Alternatively, you can follow these steps to manually install Parselmouth into a standalone version of PsychoPy:

0. Find out which version of Python PsychoPy is running.

    * To do so, you can run ``import sys; print(sys.version_info)`` in the *Shell* tab of the PsychoPy Coder interface. Remember the first two numbers of the version (major and minor; e.g., 3.6).
    * On *Windows*, also run ``import platform; print(platform.architecture()[0])`` and remember whether the Python executable's architecture is ``32bit`` or ``64bit``.

1. Go to https://pypi.org/project/praat-parselmouth/.
2. Download the file ``praat_parselmouth-x.y.z-cpVV-cpVVm-AA.whl`` *(for Windows)* or ``praat_parselmouth-x.y.z-cpVV-cpVVm-macosx_10_6_intel.whl`` *(for Mac OS X)* - where:

    * *x.y.z* will be the version of Parselmouth you want to install
    * *VV* are the first two numbers of the Python version
    * For *Windows*, *AA* is ``win32`` if you have a ``32bit`` architecture, and ``win_amd64`` for ``64bit``

   Be sure to find the right file in the list, containing both the correct Python version, and ``win32``/``win_amd64`` *(Windows)* or ``macosx`` *(Mac OS X)*  in its name!
3. Rename the downloaded file by replacing the ``.whl`` extension by ``.zip``.
4. Extract this zip archive somewhere on your computer, in your directory of choice. Remember the name and location of the extracted folder that contains the file ``parselmouth.pyd`` *(Windows)* or ``parselmouth.so`` *(Mac OS X)*.
5. Open PsychoPy, open the *Preferences* window, go to the *General* tab.
6. In the *General* tab of the PsychoPy *Preferences*, in the *paths* field, add the folder where you just extracted the Parselmouth library to the list, enclosing the path in quotemarks. (On *Windows*, also replace all ``\`` charachters by ``/``.)

    * For example, if the list was empty (``[]``), you could make it look like ``['C:/Users/Yannick/parselmouth-psychopy/']`` or ``['/Users/yannick/parselmouth-psychopy/']``.
    * On *Windows*, to find the right location to enter in the PsychoPy settings, right click ``parselmouth.pyd``, choose *Properties*, and look at the *Location* field.
    * On *Mac OS X*, to find the right location to enter in the PsychoPy settings, right click ``parselmouth.so``, choose *Get info*, and look at the *where* field.
    * On *Mac OS X*, dragging the folder into a terminal window will also give you the full path with slashes.

7. Click *Ok* to save the PsychoPy settings, close the *Preferences* window, and restart PsychoPy.
8. *Optional*: if you want to check if Parselmouth was installed correctly, open the PsychoPy Coder interface, open the *Shell* tab, and type ``import parselmouth``.

    * If this results in an error message, please let us know, and we'll try to help you fix what went wrong!
    * If this does not give you an error, congratulations, you can now use Parselmouth in your PsychoPy Builder!

.. note::

    These instructions were tested with the standalone versions `3.1.3 <https://github.com/psychopy/psychopy/releases/tag/3.1.3>`_ and `1.85.2 <https://github.com/psychopy/psychopy/releases/tag/1.85.2>`_ of PsychoPy. Things might have changed since then, so if running the script or following the manual steps results in an error, please do not hesitate to get in touch.


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

The "Microsoft Visual C++ Redistributable for Visual Studio 2019" installer can be downloaded from `Microsoft's website <https://visualstudio.microsoft.com/downloads/>`_, listed under the "Other Tools and Frameworks" section. These are the direct download links to the relevant files:

- For a 64-bit Python installation: https://aka.ms/vs/16/release/VC_redist.x64.exe
- For a 32-bit Python installation: https://aka.ms/vs/16/release/VC_redist.x86.exe

To check which Python version you are using, you can look at the first line of output when starting a Python shell. The version information should contain ``[MSC v.xxxx 64 bit (AMD64)]`` in a 64-bit installation, or ``[MSC v.xxxx 32 bit (Intel)]`` in a 32-bit installation.
