# Prerequisites

To run from source, the following needs to be installed. This doesn't apply to the pre-built executable binaries, they should run with no additional setup.

- Python 3.8 (ideally 3.8.10). Hopefully newer versions will be supported in future, but for now it _needs_ to be 3.8.
- It's strongly recommended to use a virtual environment. Not only does that help with running wxPython in a consistent way, but it also allows use of Python 3.8 (as above) without disturbing your usual Python installation. See [here|https://docs.python.org/3/tutorial/venv.html] for more on how to set up a virtual environment.
- [python-rtmidi|https://pypi.org/project/python-rtmidi/], version 1.4.9 or newer. Can be installed with pip:
  ```
  pip install python-rtmidi
  ```
- wxPython, version 4.1.1 or newer. Under Windows this is fairly easy to install with pip, but under Linux I found a more complex approach was required.
  Windows:
  ```pip install wxPython```
  Ubuntu 18.04:
  ```$ pip install -U -f https://extras.wxpython.org/wxPython4/extras/linux/gtk3/ubuntu-18.04 wxPython --no-cache-dir```
  Ubuntu 20.04:
  ```$ pip install -U -f https://extras.wxpython.org/wxPython4/extras/linux/gtk3/ubuntu-20.04 wxPython --no-cache-dir```
 
