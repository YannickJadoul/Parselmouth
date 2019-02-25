include to_include.praat

form
    sentence File_name sound.wav
endform

@readSound: file_name$
sound = readSound.return
selectObject: sound
