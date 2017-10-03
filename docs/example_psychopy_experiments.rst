PsychoPy experiments
--------------------

Parselmouth also allows Praat functionality to be included in an interactive PsychoPy experiment (refer to the subsection on :ref:`installation-psychopy` for instructions on installing Parselmouth to be used with PsychoPy). The following example shows how easily Python code that uses Parselmouth can be injected in such an experiment; following a staircase experimental design [#staircase_design]_, at each trial of the experiment a new stimulus is generated from the audio fragments *"bat"* or *"bet"* [#bat_bet_audio]_ with a specific signal-to-noise ratio (using of course Parselmouth to do so). Depending on whether the participant correctly identifies whether the noisy stimulus was *"bat"* or *"bet"*, the noise level is either increased or decreased.

These are the code fragments that are inserted into the experiment with a ``Code`` component:

.. code:: Python

    # -- Begin experiment --

    import parselmouth
    import numpy as np
    import random

    conditions = ['a', 'e']
    stimulus_files = {'a': 'audio/bat.wav', 'e': 'audio/bet.wav'}

    STANDARD_INTENSITY = 70.
    stimuli = {}
    for condition in conditions:
        stimulus = parselmouth.Sound(stimulus_files[condition])
        stimulus.scale_intensity(STANDARD_INTENSITY)
        stimuli[condition] = stimulus

.. code:: Python

    # -- Begin Routine --

    random_condition = random.choice(conditions)
    random_stimulus = stimuli[random_condition]

    noise_samples = np.random.normal(size=random_stimulus.num_samples)
    noise = parselmouth.Sound(noise_samples, random_stimulus.sampling_frequency)
    noise.scale_intensity(STANDARD_INTENSITY - level)

    noisy_stimulus = random_stimulus.copy()
    noisy_stimulus_values = noisy_stimulus.values
    noisy_stimulus_values += noise.values
    noisy_stimulus.scale_intensity(STANDARD_INTENSITY)

    # 'filename' variable is set by PsychoPy and contains base file name
    # of saved log/output files, so we'll use that to save our custom stimuli
    stimulus_file_name = filename + '_stimulus_' + str(trials.thisTrialN) + '.wav'
    noisy_stimulus.resample(44100).save(stimulus_file_name, "WAV")
    sound_1.setSound(stimulus_file_name)

.. code:: Python

    # -- End routine --

    trials.addResponse(key_resp_2.keys == random_condition)


The full PsychoPy experiment which can be opened in the *Builder* then looks like this: :download:`adaptive_listening.psyexp <other/adaptive_listening.psyexp>`

.. [#staircase_design] See e.g. Kaernbach, C. (2001). Adaptive threshold estimation with unforced-choice tasks. *Attention, Perception, & Psychophysics*, *63*, 1377–1388., or the PsychoPy tutorial at http://www.psychopy.org/coder/tutorial2.html.
.. [#bat_bet_audio] :download:`bat.wav <audio/bat.wav>` and :download:`bet.wav <audio/bet.wav>`
