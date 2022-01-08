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
    cn = bb.find("count").text
    if cn.startswith("0x") or cn.startswith("0X"):
        cn_val = int(cn, 16)
    else:
        cn_val = int(cn)


    cc = i.find("bits")
    if cc is None:
        cf_val = -1
        cm_val = -1
    else:
        cf = cc.find("offset").text
        if cf.startswith("0x") or cf.startswith("0X"):
            cf_val = int(cf, 16)
        else:
            cf_val = int(cf)
        cm = cc.find("count").text
        if cm.startswith("0x") or cm.startswith("0X"):
            cm_val = int(cm, 16)
        else:
            cm_val = int(cm)

    rr = i.find("recommendedLimits")
    if rr is None:
        # Try absoluteLimits before giving up.
        
        rr = i.find("absoluteLimits")
        if rr is None:
            # Just use the bit numbers as source of min/max
            r1_val = 0
            if cm_val > 0:
                r2_val = (1<<cm_val)-1
            else:
                r2_val = -1
                r1_val = -1
        else:
            r1 = rr.find("min").text
            if r1.startswith("0x") or r1.startswith("0X"):
                r1_val = int(r1, 16)
            else:
                r1_val = int(r1)
            r2 = rr.find("max").text
            if r2.startswith("0x") or r2.startswith("0X"):
                r2_val = int(r2, 16)
            else:
                r2_val = int(r2)
    else:
        r1 = rr.find("min").text
        if r1.startswith("0x") or r1.startswith("0X"):
            r1_val = int(r1, 16)
        else:
            r1_val = int(r1)
        r2 = rr.find("max").text
        if r2.startswith("0x") or r2.startswith("0X"):
            r2_val = int(r2, 16)
        else:
            r2_val = int(r2)





    dv_val = 0
    dd = i.find("defaultValue")
    if dd is not None:
        dv = dd.text
        if dv.startswith("0x") or dv.startswith("0X"):
            dv_val = int(dv, 16)
        else:
            dv_val = int(dv)
    

    A.append((     i.get("number"),   # non-unique number!
                   i.get("block"),
                   of_val,            # offset into file
                   cn_val,
                   cf_val,
                   cm_val,
                   dv_val,            # default value
                   r1_val,
                   r2_val
             ))
    

with open(p.parent.parent.resolve().joinpath("src", "parameters.py"), "w") as f_py:

    f_py.write("## Automatically generated file. Time of processing: {0}\n\n".format( datetime.datetime.now().isoformat() ))
    f_py.write(textwrap.dedent("""
                  from dataclasses import dataclass
                  
                  @dataclass
                  class Param:
                      number: int
                      block0: int
                      byteOffset: int
                      byteCount: int
                      bitOffset: int
                      bitCount: int
                      recommendedLimits: tuple
                      defaultValue: int
                  
                  
                  """))

    f_py.write("Params = [\n")

    for AA in A:
        f_py.write("    Param({0}, {1}, byteOffset={2}, byteCount={3}, bitOffset={4}, bitCount={5}, defaultValue={6}, recommendedLimits=({7}, {8})),\n".format(AA[0], AA[1], AA[2], AA[3], AA[4], AA[5], AA[6], AA[7], AA[8]))
    
    f_py.write("]\n\n\n")

    
print("...done")
