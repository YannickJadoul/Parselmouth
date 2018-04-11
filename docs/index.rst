Parselmouth -- Praat in Python, the Pythonic way
================================================

**Parselmouth** is a Python library for the `Praat <http://www.praat.org>`_ software.

Though other attempts have been made at porting functionality from Praat to Python, Parselmouth is unique in its aim to provide a complete and Pythonic interface to the internal Praat code. While other projects either wrap Praat's scripting language or reimplementing parts of Praat's functionality in Python, Parselmouth directly accesses Praat's C/C++ code (which means the algorithms and their output are exactly the same as in Praat) and provides efficient access to the program's data, but *also* provides an interface that looks no different from any other Python library.

Please note that Parselmouth is currently in premature state and in active development. While the amount of functionality that is currently present is not huge, more will be added over the next few months. As such, *feedback* and possibly *contributions* are highly appreciated.

Drop by our `Gitter chat room <https://gitter.im/PraatParselmouth/Lobby>`_ if you have any question, remarks, or requests!

.. note::
    Try out Parselmouth online, in interactive Jupyter notebooks on Binder: |binder_badge_examples|

.. toctree::
   :maxdepth: 2
   :caption: Getting Started

   installation
   examples

   api_reference


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
