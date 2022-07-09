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

    r3_val = 16

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
        r3 = rr.find("step")
        if r3 is not None:
            r3 = r3.text
            if r3.startswith("0x") or r3.startswith("0X"):
                r3_val = int(r3, 16)
            else:
                r3_val = int(r3)




    mb = i.find("midiBytes")
    if mb is None:
        raise Exception
    else:
        mb = mb.text
        if mb.startswith("0x") or mb.startswith("0X"):
            mb_val = int(mb, 16)
        else:
            mb_val = int(mb)




    hlp = i.find("help")
    hlp_val = ""
    if hlp is not None:
        hlp_val = hlp.text



    dv_val = 0
    dd = i.find("defaultValue")
    if dd is not None:
        dv = dd.text
        if dv.startswith("0x") or dv.startswith("0X"):
            dv_val = int(dv, 16)
        else:
            dv_val = int(dv)
    


    nm_val = ""
    nn = i.find("name")
    if nn is not None:
        nm_val = nn.text

    cst_val = ""
    ct = i.find("cluster")
    if ct is not None:
        cst_val = ct.text


    A.append((     i.get("number"),   # non-unique number!
                   i.get("block"),
                   of_val,            # offset into file
                   cn_val,
                   cf_val,
                   cm_val,
                   dv_val,            # default value
                   r1_val,
                   r2_val,
                   r3_val,
                   nm_val,
                   cst_val,
                   mb_val,            # MIDI bytes count
                   hlp_val,
             ))
    

with open(p.parent.parent.resolve().joinpath("python", "parameters.py"), "w") as f_py:

    f_py.write("## Automatically generated file. Time of processing: {0}\n\n".format( datetime.datetime.now().isoformat() ))
    f_py.write(textwrap.dedent("""
                  from dataclasses import dataclass
                  
                  @dataclass
                  class Param:
                      number: int
                      block0: int
                      name: str
                      cluster: str
                      byteOffset: int
                      byteCount: int
                      bitOffset: int
                      bitCount: int
                      recommendedLimits: tuple
                      recommendedStep: int
                      defaultValue: int
                      midiBytes: int
                      helpStr: str
                  
                  
                  """))

    f_py.write("Params = [\n")

    for AA in A:
        f_py.write('    Param({0}, {1}, byteOffset={2}, byteCount={3}, bitOffset={4}, bitCount={5}, defaultValue={6}, recommendedLimits=({7}, {8}), recommendedStep={9}, name="{10}", cluster="{11}", midiBytes={12}, helpStr="{13}"),\n'.format(AA[0], AA[1], AA[2], AA[3], AA[4], AA[5], AA[6], AA[7], AA[8], AA[9], AA[10], AA[11], AA[12], AA[13]))
    
    f_py.write("]\n\n\n")

    
    
    
    
with open(p.parent.parent.resolve().joinpath("src-cpp", "parameters.h"), "w") as f_cpp:
    
    f_cpp.write("/* Automatically generated file. Time of processing: {0}   */\n\n".format( datetime.datetime.now().isoformat() ))
    f_cpp.write(textwrap.dedent("""
                  #include <wx/wxprec.h>


                  class Parameter
                  {
                      public:
                      
                      
                      int number;
                      int block0;
                      wxString name;
                      wxString cluster;
                      int byteOffset;
                      int byteCount;
                      int bitOffset;
                      int bitCount;
                      int recommendedLimits[2];
                      int recommendedStep;
                      int defaultValue;
                      int midiBytes;
                      wxString helpStr;
                      
                      
                  };
                  
                  """))



    f_cpp.write("const Parameter   Parameters[{0}] =\n{{\n".format(len(A)))

    for i, AA in enumerate(A):
        if i > 0:
            f_cpp.write(",")
        f_cpp.write('\n    {{{0}, {1}, "{10}", "{11}", {2}, {3}, {4}, {5}, {{{7}, {8}}}, {9}, {6}, {12}, "{13}"}}'.format(AA[0], AA[1], AA[2], AA[3], AA[4], AA[5], AA[6], AA[7], AA[8], AA[9], AA[10], AA[11], AA[12], AA[13]))
    
    f_cpp.write("\n\n};\n\n\n")
    
    
print("...done")
