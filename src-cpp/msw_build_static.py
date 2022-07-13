
# A script to change *all* project files within the WxWidgets codebase, to build them
# for static run-time. This seems to be the only way to do it with more
# recent versions of Visual Studio.

# First command-line argument points to the build/msw directory.


import sys
import pathlib
import re



if len(sys.argv) < 2:
    sys.exit(-1)
    

P = pathlib.Path(sys.argv[1]).resolve()
for F in P.glob("*.vcxproj"):
    if F.is_file():
        print("msw_build_static.py is processing file: {0}".format(str(F)))
        T = F.read_text()
        T = re.sub(r'\bMultiThreadedDLL\b', 'MultiThreaded', T)              # Replaces whole word only
        T = re.sub(r'\bMultiThreadedDebugDLL\b', 'MultiThreadedDebug', T)    # Replaces whole word only
        F.write_text(T)

sys.exit(0)
