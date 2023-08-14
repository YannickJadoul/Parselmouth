/*
* Copyright (C) 2023  Yannick Jadoul
*
* This file is part of Parselmouth.
*
* Parselmouth is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Parselmouth is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>
*/

#pragma once
#ifndef INC_PARSELMOUTH_PRAAT_DOCSTRINGS_H
#define INC_PARSELMOUTH_PRAAT_DOCSTRINGS_H

namespace parselmouth {

auto constexpr PRAAT_MODULE_DOCSTRING =
R"(Submodule with functions to call Praat commands and run Praat scripts.

In addition to the Python interface provided in the top-level `parselmouth`
module, this module provides a Python interface to access any Praat
command or action through the `parselmouth.praat.call` function from
Python. Moreover, existing Praat scripts can be run through the
`parselmouth.praat.run` and `parselmouth.praat.run_file` functions.

.. note::
    Any command or script accessing the graphical user interface or other
    interactive functionality of Praat will not work, since the internal
    Praat source is in Parselmouth is run in *batch mode*. This is similar
    to running a Praat script from the command line (see
    :praat:`Scripting 6.9. Calling from the command line`).
)";

auto constexpr PRAAT_CALL_DOCSTRING =
R"(Call a Praat command.

This function provides a Python interface to call available Praat commands
based on the label in the Praat user interface and documentation, similar
to the Praat scripting language.

Calling a Praat command through this function roughly corresponds to the
following scenario in the Praat user interface or scripting language:

1. Zero, one, or multiple `parselmouth.Data` objects are put into Praat's
   global object list and are 'selected'.
2. The Python argument values are converted into Praat values; see below.
3. The Praat command is executed on the selected objects with the converted
   values as arguments.
4. The result of the command is returned. The type of the result depends on
   the result of the Praat command; see below.
5. Praat's object list is emptied again, such that a future execution of
   this function is independent from the current call.

The use of `call` is demonstrated in the `Pitch manipulation and Praat
commands <../examples/pitch_manipulation.ipynb>`_ example.

Parameters
----------
object : parselmouth.Data
    A single object to add to the Praat object list, which will be selected
    when the Praat command is called.
objects : List[parselmouth.Data]
    Multiple objects to be added to the Praat object list, which will be
    selected when the Praat command is called.
command : str
    The Praat action to call. This is the same command name as one would
    use in a Praat script and corresponds to the label on the button in
    the Praat user interface.
*args
    The list of values to be passed as arguments to the Praat command.
    Allowed types for these arguments are:

    - `int` or `float`: passed as a Praat numeric value
    - `bool`: converted into ``"yes"``/``"no"``
    - `str`: passed as Praat string value
    - `numpy.ndarray`: passed as Praat vector or matrix, if the array
      contains numeric values and is 1D or 2D, respectively.

Keyword Arguments
-----------------
extra_objects : List[parselmouth.Data]
    Extra objects added to the Praat object list that will not be selected
    when the command is called (default value: ``[]``).
return_string : bool
    Return the raw string written in the Praat info window instead of the
    converted Python object (default value: ``False``).

Returns
-------
object
    The result of the Praat command. The actual value returned depends on
    what the Praat command does. The following types can be returned:

    - If ``return_string=True`` was passed, a `str` value is returned,
      which contains the text that would have been written to the Praat
      info window.
    - A `float`, `int`, `bool`, or `complex` value is returned when the
      Praat command would write such a value to the Praat info window.
    - A `numpy.ndarray` value is returned if the command returns a Praat
      vector or matrix.
    - A `parselmouth.Data` object is returned if the command always creates
      exactly one object. If the actual type of the Praat object is
      available in Parselmouth, an object of a subtype of
      `parselmouth.Data` is returned.
    - A list of `parselmouth.Data` objects is returned if the command can
      create multiple new objects (even if this particular execution of the
      command only added one object to the Praat object list).
    - A `str` is returned when a string or info text would be written to
      the Praat info window.

See Also
--------
parselmouth.praat.run, parselmouth.praat.run_file
:praat:`Scripting`
)";

auto constexpr PRAAT_RUN_DOCSTRING =
R"(Run a Praat script.

Given a string with the contents of a Praat script, run this script as if
it was run inside Praat itself. Similarly to `parselmouth.praat.call`,
Parselmouth objects and Python argument values can be passed into the
script.

Calling this function roughly corresponds to the following sequence of
steps in Praat:

1. Zero, one, or multiple `parselmouth.Data` objects are put into Praat's
   global object list and are 'selected'.
2. The Python argument values are converted into Praat values; see `call`.
3. The Praat script is opened and run with the converted values as
   arguments; see :praat:`Scripting 6.1. Arguments to the script`.
4. The results of the execution of the script are returned; see below.
5. Praat's object list is emptied again, such that a future execution of
   this function is independent from the current call.

Note that the script will be run in Praat's so-called 'batch' mode; see
:praat:`Scripting 6.9. Calling from the command line`. Since the script is
run from inside a Python program, the Praat functionality is run without
graphical user interface and no windows (such as *"View & Edit"*) can be
opened by the Praat script. However, the functionality in these windows is
also available in different ways: for example, opening a *Sound* object in
a *"View & Edit"* window, making a selection, and choosing *"Extract
selected sound (windowed)..."* can also be achieved by directly using the
*"Extract part..."* command of the *Sound* object.

Arguments
---------
object : parselmouth.Data
    A single object to add to the Praat object list, which will be selected
    when the Praat script is run.
objects : List[parselmouth.Data]
    Multiple objects to be added to the Praat object list, which will be
    selected when the Praat script is run.
script : str
    The content of the Praat script to be run.
*args
    The list of values to be passed as arguments to the Praat script. For
    more details on the allowed types of these argument, see `call`.

Keyword arguments
-----------------
extra_objects : List[parselmouth.Data]
    Extra objects added to the Praat object list that will not be selected
    when the command is called (default value: ``[]``).
capture_output : bool
    Intercept and also return the output written to the Praat info window,
    instead of forwarding it to the Python standard output; see below
    (default value: ``False``).
return_variables : bool
    Also return a `dict` of the Praat variables and their values at the
    end of the script's execution; see below (default value: ``False``).

Returns
-------
object
    A list of `parselmouth.Data` objects selected at the end of the
    script's execution.

    Optionally, extra values are returned:

    - A `str` containing the intercepted output if ``capture_output=True``
      was passed.
    - A `dict` mapping variable names (`str`) to their values (`object`)
      if ``return_variables`` is ``True``. The values of Praat's variables
      get converted to Python values:

      - A Praat string variable, with a name ending in ``$``, is returned
        as `str` value.
      - A Praat vector or matrix variable, respectively ending in ``#`` or
        ``##``, is returned as `numpy.ndarray`.
      - A numeric variable, without variable name suffix, is converted to
        a Python `float`.

See Also
--------
parselmouth.praat.run_file, parselmouth.praat.call
:praat:`Scripting`
)";

auto constexpr PRAAT_RUN_FILE_DOCSTRING =
R"(Run a Praat script from file.

Given the filename of a Praat script, the script is read and run the same
way as a script string passed to `parselmouth.praat.run`. See `run` for
details on the manner in which the script gets executed.

One thing to note is that relative filenames in the Praat script
(including those in potential 'include' statements in the script; see
:praat:`Scripting 5.8. Including other scripts`) will be resolved relative
to the path of the script file, just like in Praat. Also note that Praat
accomplishes this by temporarily changing the current working during the
execution of the script.

Arguments
---------
object : parselmouth.Data
    A single object to add to the Praat object list, which will be selected
    when the Praat script is run.
objects : List[parselmouth.Data]
    Multiple objects to be added to the Praat object list, which will be
    selected when the Praat script is run.
path : str
    The filename of the Praat script to run.
*args
    The list of values to be passed as arguments to the Praat script. For
    more details on the allowed types of these argument, see `call`.

Keyword arguments
-----------------
keep_cwd : bool
    Keep the current working directory (see `os.getcwd`) when running the
    script, rather than changing it to the script's parent directory, as
    Praat does by default (default value: ``False``). Note that even when
    set to ``True``, the filenames in the Praat script's include statements
    will be resolved relatively to the directory containing the script.
**kwargs
    See `parselmouth.praat.run`.

Returns
-------
object
    See `parselmouth.praat.run`.

See Also
--------
parselmouth.praat.run, parselmouth.praat.call
:praat:`Scripting`
)";

} // namespace parselmouth

#endif // INC_PARSELMOUTH_PRAAT_DOCSTRINGS_H
