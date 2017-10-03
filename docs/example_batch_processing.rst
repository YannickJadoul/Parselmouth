Batch processing of files
-------------------------

Using the Python standard libraries, we can also quickly code up batch operations e.g. over all files with a certain extension in a directory:

.. code:: Python
    
    # Find all .wav files in a directory, pre-emphasize and save as new .wav and .aiff file
    import parselmouth
    import glob
    import os.path

    for wave_file in glob.glob('/home/yannick/*.wav'):
        s = parselmouth.Sound(wave_file)
        s_pre = s.pre_emphasize()
        s.save(os.path.splitext(wave_file)[0] + '_pre.wav', "WAV") # or parselmouth.SoundFileFormat.WAV instead of "WAV"
        s.save(os.path.splitext(wave_file)[0] + '_pre.aiff', "AIFF")

After running this, the original home directory now contains all of the original ``.wav`` files pre-emphazised and written again as ``.wav`` and ``.aiff`` files. The reading, pre-emphasis, and writing are all done by Praat, while looping over all ``.wav`` files is done by standard Python code.

Similarly, we can use the `pandas <http://pandas.pydata.org/>`_ library to read a CSV file with data collected in an experiment, and loop over that data to e.g. extract the mean harmonics-to-noise ratio. The results CSV file could look like this:

========= === =====
condition ... pp_id
========= === =====
0         ... 1877
1         ... 801
1         ... 2456
0         ... 3126
========= === =====

The following code would read and this table, loop over it, use Praat through Parselmouth to calculate the analysis of each row, and then write an augmented CSV file to disk:

.. code:: Python

    import parselmouth
    import pandas as pd

    def analyse_sound(row):
	    condition, pp_id = row[['condition', 'pp_id']].values
	    filepath = 'audio/{}/{}.wav'.format(condition, pp_id)
	    sound = parselmouth.Sound(filepath)
	    harmonicity = sound.to_harmonicity()
	    return harmonicity.values.mean()

    # Read in the experimental results file
    dataframe = pd.read_csv('results.csv')

    # Apply parselmouth wrapper function row-wise
    dataframe['harmonics_to_noise'] = dataframe.apply(analyse_sound, axis='rows') 

    # Write out the updated dataframe
    dataframe.to_csv('processed_results.csv')

