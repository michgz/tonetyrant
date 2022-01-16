# Tone Tyrant for Casio

![tyrant-64x64](https://user-images.githubusercontent.com/42281265/149575450-a4bce95c-79d1-4836-a40a-6082bbaf9f68.png)

A hex editor and tone editing tool for Casio keyboards. Currently works for keyboards CT-X3000 & CT-X5000.

### Windows

A pre-built windows executable may be downloaded from here. Extract from the zip file. The first time you run it, Windows security may ask if you really want to run it: click "More Info" and "Run Anyway".

[https://github.com/michgz/tonetyrant/releases/download/v1.0.0/windows-executable-v1.0.0.zip]

### Linux

A pre-built executable may be downloaded from here for Linux kernel versions from 4.15 or later (Ubuntu 18.04 or later). Extract from the zip file and enable execution rights with ```sudo chmod +x tyrant```.

[https://github.com/michgz/tonetyrant/releases/download/v1.0.0/linux-executable-v1.0.0.zip]

### From source

Running from source code will give the best possible compatibility with your operating system. Clone or download this Github repository and run it by calling ```python run.py```. There are some prerequisites to install which are listed [here](Prerequisites.md).

This method works for Windows, all versions of Linux, and maybe even Mac OS (if you try it on Mac OS, please give some feedback with your experiences using a Github "Issue").

## Quick start guide

To try out the main features of the software, go through the following steps:

1. "Ctrl+N" to create a new TON file. It will be pre-filled with a sine-wave tone.
2. Use arrow keys &#8592;&#8593;&#8594;&#8595; to navigate around bytes in the file
3. Overwrite values by typing in numbers. The numbers are in "hexadecimal", so digits "0"&#8211;"9" and "A"&#8211;"F" are all accepted.
4. When you have navigated to certain parameters, boxes will pop up in the "Hints" window. When that happens, try increasing/decreasing the parameter values by pressing PG.UP/PG.DN keys.
5. Press "Ctrl+R" and "OK" to change all parameters in the tone to random values.
6. Attach a CT-X3000/X5000 keyboard to the computer by MIDI, switched on, and press "Ctrl+M" to set up the MIDI communications. Make sure you change both the "Input port" and the "Output port" to be the Casio USB MIDI, and press "OK".
7. Press "F3" and "OK" to upload the new randomised file to User Tone 801 on the keyboard. Change the selected tone in the keyboard to something else (e.g. tone 802), then back to 801 and play some notes. It will sound with the new randomised tone.
