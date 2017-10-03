Plotting
--------

Using Parselmouth, it is possible to use the existing Python plotting libraries -- such as `Matplotlib <http://matplotlib.org/>`_ and `seaborn <http://seaborn.pydata.org/>`_ -- to make custom visualizations of the speech data and analysis results obtained by running Praat's algorithms.

The following examples visualize an audio recording of someone saying *"The north wind and the sun [...]"* [#nwas_audio]_.

We start out by simply reading in the audio file and plotting the raw waveform:

.. code:: python

    import parselmouth

    import numpy as np
    import matplotlib.pyplot as plt
    import seaborn as sns
    sns.set() # Use seaborn's default style to make graphs more pretty

    # Plot nice figures using Python's "standard" matplotlib library
    snd = parselmouth.Sound("the_north_wind_and_the_sun.wav")
    plt.figure()
    plt.plot(snd.xs(), snd.values)
    plt.xlim([snd.xmin, snd.xmax])
    plt.xlabel("time [s]")
    plt.ylabel("amplitude")
    plt.show() # or plt.savefig("sound.png"), or plt.savefig("sound.pdf")

.. image:: images/nwas_wave.*

It is also possible to extract part of the speech fragment and plot it separately. For example, let's extract the word *"sun"* and plot its waveform with a finer line:

.. code:: python

    snd_part = snd.extract_part(from_time=0.9, preserve_times=True)

    plt.figure()
    plt.plot(snd_part.xs(), snd_part.values, linewidth=0.5)
    plt.xlim([snd_part.xmin, snd_part.xmax])
    plt.xlabel("time [s]")
    plt.ylabel("amplitude")
    plt.show()

.. image:: images/nwas_part_wave.*

Next, we can write a couple of ordinary Python functions to plot a Parselmouth ``Spectrogram`` and ``Intensity``:

.. code:: Python

    def draw_spectrogram(spectrogram, dynamic_range=70):
        X, Y = spectrogram.x_grid(), spectrogram.y_grid()
        sg_db = 10 * np.log10(spectrogram.values.T)
        plt.pcolormesh(X, Y, sg_db, vmin=sg_db.max() - dynamic_range, cmap='afmhot')
        plt.ylim([spectrogram.ymin, spectrogram.ymax])
        plt.xlabel("time [s]")
        plt.ylabel("frequency [Hz]")

    def draw_intensity(intensity):
        plt.plot(intensity.xs(), intensity.values, linewidth=3, color='w')
        plt.plot(intensity.xs(), intensity.values, linewidth=1)
        plt.grid(False)
        plt.ylim(0)
        plt.ylabel("intensity [dB]")

After defining how to plot these, we use Praat (through Parselmouth) to calculate the spectrogram and intensity to actually plot the intensity curve overlaid on the spectrogram:

.. code:: Python

    intensity = snd.to_intensity()
    spectrogram = snd.to_spectrogram()
    plt.figure()
    draw_spectrogram(spectrogram)
    plt.twinx()
    draw_intensity(intensity)
    plt.xlim([snd.xmin, snd.xmax])
    plt.show()

.. image:: images/nwas_spectrogram_intensity.*

The Parselmouth fucntions and methods have the same arguments as the Praat commands, so we can for example also change the window size of the spectrogram analysis to get a narrow-band spectrogram. Next to that, let's now have Praat calculate the pitch of the fragment, so we can plot it instead of the intensity:

.. code:: Python

    def draw_pitch(pitch):
        # Extract selected pitch contour, and
        # replace unvoiced samples by NaN to not plot
        pitch_values = pitch.to_matrix().values
        pitch_values[pitch_values==0] = np.nan
        plt.plot(pitch.xs(), pitch_values, linewidth=3, color='w')
        plt.plot(pitch.xs(), pitch_values, linewidth=1)
        plt.grid(False)
        plt.ylim(0, pitch.ceiling)
        plt.ylabel("pitch [Hz]")

    pitch = snd.to_pitch()
    spectrogram = snd.to_spectrogram(window_length=0.03, maximum_frequency=8000)
    plt.figure()
    draw_spectrogram(spectrogram)
    plt.twinx()
    draw_pitch(pitch)
    plt.xlim([snd.xmin, snd.xmax])
    plt.show()

.. image:: images/nwas_nb_spectrogram_pitch.*

Using the ``FacetGrid`` functionality from *seaborn*, we can even plot plot multiple a structured grid of multiple custom spectrograms. For example, we will read a CSV file (using the `pandas <http://pandas.pydata.org/>`_ library) that contains the digit that was spoken, the ID of the speaker and the file name of the audio fragment [#digits_audio]_:

.. code:: Python

    import pandas as pd

    def facet_util(data, **kwargs):
        digit, speaker_id = data[['digit', 'speaker_id']].iloc[0]
        sound = parselmouth.Sound("{0}_{1}.wav".format(digit, speaker_id))
        draw_spectrogram(sound.to_spectrogram())
        plt.twinx()
        draw_pitch(sound.to_pitch())
        # If not the rightmost column, then clear the right side axis
        if digit != 5:
            plt.ylabel("")
            plt.yticks([])

    results = pd.read_csv("digit_list.csv")

    grid = sns.FacetGrid(results, row='speaker_id', col='digit')
    grid.map_dataframe(facet_util)
    grid.set_titles(col_template="{col_name}", row_template="{row_name}")
    grid.set_axis_labels("time [s]", "frequency [Hz]")
    grid.set(facecolor='white', xlim=(0, None))
    plt.show()

.. image:: images/digits_facetgrid.*

.. [#nwas_audio] :download:`the_north_wind_and_the_sun.wav <audio/the_north_wind_and_the_sun.wav>`, extracted from a `Wikipedia Commons audio file <https://commons.wikimedia.org/wiki/File:Recording_of_speaker_of_British_English_(Received_Pronunciation).ogg>`_.
.. [#digits_audio] :download:`digit_list.csv <other/digit_list.csv>`, :download:`1_b.wav <audio/1_b.wav>`, :download:`2_b.wav <audio/2_b.wav>`, :download:`3_b.wav <audio/3_b.wav>`, :download:`4_b.wav <audio/4_b.wav>`, :download:`5_b.wav <audio/5_b.wav>`, :download:`1_y.wav <audio/1_y.wav>`, :download:`2_y.wav <audio/2_y.wav>`, :download:`3_y.wav <audio/3_y.wav>`, :download:`4_y.wav <audio/4_y.wav>`, :download:`5_y.wav <audio/5_y.wav>`

