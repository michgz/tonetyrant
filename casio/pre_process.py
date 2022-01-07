"""
Pre-process the parameters.xml file into python source code.
"""

import pathlib
from xml.etree import ElementTree as ET
import datetime
import textwrap

A = []

print("Processing...")

p = pathlib.Path(__file__)
tree = ET.parse(p.parent.resolve().joinpath("parameters.xml"))
elem = tree.getroot()
for i in elem.find("parameters"):
    bb = i.find("bytes")
    of = bb.find("offset").text
    if of.startswith("0x") or of.startswith("0X"):
        of_val = int(of, 16)
    else:
        of_val = int(of)
  
    dv_val = 0
    dd = i.find("defaultValue")
    if dd is not None:
        dv = dd.text
        if dv.startswith("0x") or dv.startswith("0X"):
            dv_val = int(dv, 16)
        else:
            dv_val = int(dv)
    

    A.append((     i.get("number"),   # non-unique number!
                   of_val,            # offset into file
                   dv_val             # default value
             ))
    

with open(p.parent.parent.resolve().joinpath("src", "parameters.py"), "w") as f_py:

    f_py.write("## Automatically generated file. Time of processing: {0}\n\n".format( datetime.datetime.now().isoformat() ))
    f_py.write(textwrap.dedent("""
                  from dataclasses import dataclass
                  
                  @dataclass
                  class Param:
                      number: int
                      offset: int
                      defaultValue: int
                  
                  
                  """))

    f_py.write("Params = [\n")

    for AA in A:
        f_py.write("    Param({0}, {1}, {2}),\n".format(AA[0], AA[1], AA[2]))
    
    f_py.write("]\n\n\n")

    
print("...done")
