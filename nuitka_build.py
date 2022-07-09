"""
A script to automate building ToneTyrant with Nuitka. Only for Windows
"""

import sys
import os
import os.path
import nuitka.__main__


def read_version():
    # Pull the version out of our main.py file
    import python.main
    return python.main.__version__
    
    
def build():
  
    VERSION_STR = read_version()
  
    # Manipulate the command line to have the options we want
    sys.argv = sys.argv[:1]
    
    sys.argv.extend(["--onefile"])
    sys.argv.extend(["--assume-yes-for-downloads"])  # This is needed to get dependency walker on a fresh system
    sys.argv.extend(["--include-data-file=tyrant-64x64.ico=tyrant-64x64.ico"])
    
    sys.argv.extend(["--windows-icon-from-ico", "tyrant-64x64.ico"])
    
    sys.argv.extend(["--windows-company-name", "https://github.com/michgz"])
    sys.argv.extend(["--windows-product-name", "ToneTyrant"])
    sys.argv.extend(["--windows-file-version", VERSION_STR])
    sys.argv.extend(["--windows-product-version", VERSION_STR])
    sys.argv.extend(["--windows-file-description", "Tone editor for Casio keyboards"])
    sys.argv.extend(["--windows-disable-console"])
    
    
    sys.argv.extend(["--show-scons"])   # These two produce lots of output!!
    sys.argv.extend(["--verbose"])      #
    sys.argv.extend(["-o", "tyrant.exe"])
    sys.argv.extend(["python/main.py"])
    
    # Now run!
    nuitka.__main__.main()
    
    
    
  
  
if __name__ == "__main__":
    build()


