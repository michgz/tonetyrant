# Tone Tyrant for Casio

![tyrant-64x64](https://user-images.githubusercontent.com/42281265/149575450-a4bce95c-79d1-4836-a40a-6082bbaf9f68.png)

A hex editor and tone editing tool for Casio keyboards. "Real-time" parameter control works on all Casio CT-X keyboards, and upload/download of User Tones is possible with keyboards which have User Tone memory (e.g. CT-X3000 or X5000).

Features include:
* Automatically calculates the Casio CRC value
* Provides "Hints" when editing bytes that are associated with a known parameter. The parameter can be quickly increased/decreased with PAGE UP/PAGE DN buttons.
* Can generate a random tone
* Upload and download to User Tone memory locations in a CT-X3000/5000 keyboard
* **New with V2.0**: "real-time" control of synthesis parameters of the currently selected keyboard tone, using SysEx commands over MIDI USB to the keyboard

### Windows

A pre-built executable for Windows 10 & 11 may be downloaded from [here](https://github.com/michgz/tonetyrant/releases/download/v2.0.0/windows-10-11-executable-v2.0.0.zip). Extract from the zip file. The first time you run it, Windows security may ask if you really want to run it: click "More Info" and "Run Anyway".

### Linux

A pre-built executable may be downloaded from [here](https://github.com/michgz/tonetyrant/releases/download/v2.0.0/linux-executable-v2.0.0.zip) for Linux kernel versions from 4.15 or later (Ubuntu 18.04 or later). Extract from the zip file and enable execution rights with ```sudo chmod +x tyrant```.

### From source

Running from source code will give the best possible compatibility with your operating system. Clone or download this Github repository and run it by calling ```python run.py```. There are some prerequisites to install which are listed [here](Prerequisites.md).

This method works for all versions of Windows and Linux, and maybe even Mac OS (if you try it on Mac OS, please give some feedback with your experiences using a Github "Issue").

## Quick start guide

To try out the main features of the software, go through the following steps:

1. "Ctrl+N" to create a new TON file. It will be pre-filled with a sine-wave tone.
2. Use arrow keys &#8592;&#8593;&#8594;&#8595; to navigate around bytes in the file
3. Overwrite values by typing in numbers. The numbers are in "hexadecimal", so digits "0"&#8211;"9" and "A"&#8211;"F" are all accepted.
4. When you have navigated to certain parameters, boxes will pop up in the "Hints" window. When that happens, try increasing/decreasing the parameter values by pressing PG.UP/PG.DN keys.
5. Press "Ctrl+R" and "OK" to change all parameters in the tone to random values.
6. Attach a CT-X3000/X5000 keyboard to the computer by MIDI, switched on, and press "Ctrl+M" to set up the MIDI communications. Make sure you change both the "Input port" and the "Output port" to be the Casio USB MIDI, and press "OK".
7. Press "F3" and "OK" to upload the new randomised file to User Tone 801 on the keyboard. Change the selected tone in the keyboard to something else (e.g. tone 802), then back to 801 and play some notes. It will sound with the new randomised tone.

## History

* v2.0.0  Real-time parameter control over MIDI SysEx
* v1.0.3
   * Improved sizing layout of Hints window
   * Info bubbles for some parameters
   * Special combo box for parameter 42, "Note Off Velocity"
   * Configuration parameters stored persistently in a .CFG file
   * **Linux**: built for GTK 2.0. The previous GTK 3.0 was causing crashing with some windows managers, e.g. Plasma
* v1.0.2  Try a new build method which might give better compatibility with older Windows versions (this version built for Windows only)
* v1.0.1  Improvements to the appearance of the Hints window under MS Windows
* v1.0.0  First release
