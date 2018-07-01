#!/usr/bin/env python3
from __future__ import print_function
# obtain path to "english.wav" in the same folder as this script
from os import path
def translateAudio():
	import speech_recognition as sr
	AUDIO_FILE = path.join(path.dirname(path.realpath(__file__)), "file.wav")

# use the audio file as the audio source
	r = sr.Recognizer()
	with sr.AudioFile(AUDIO_FILE) as source:
		audio = r.record(source)  # read the entire audio file

	return r.recognize_google(audio)

f1 = open('./result.txt', 'w+')

f1.write(translateAudio())
f1.close()
