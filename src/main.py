#!/usr/bin/env python
import wx
import sys
import logging
import os

sys.path.append("src")

import hexeditview
import docview

import dataclasses
import parameters
import random
import struct
import binascii
import copy


__version__ = "1.0.3"
__author__ = "michgz"


# A text-format signifier for the origin of the executable. Displayed in the
# about box after the version. Can leave blank, for example if there's no
# executable.
_build_signifier = ""

from midi_comms import MidiComms



# Set up the language translation system. Currently, none is used and this function
# does nothing
_ = lambda X : X


# A separator to place between numbers and letters in a ComboBox. Tab works well
# for linux, but looks wierd on Windows. Go with this as a compromise.
SEP = "  "





EDIT_SETTODEFAULT_ID = wx.NewIdRef()
EDIT_SETTORANDOMISE_ID = wx.NewIdRef()

MIDI_SETUP_ID = wx.NewIdRef()
MIDI_DOWNLOAD_ID = wx.NewIdRef()
MIDI_UPLOAD_ID = wx.NewIdRef()


ICON_LOCATION = ""


class ViewType:
    SPIN = 0      # Handles most numeric fields
    CHECK = 1     # Handles most binary fields
    
    # Now some custom types for fields that need specific handling: ...
    CUSTOM_FILTERTYPE = 2
    CUSTOM_WAVETABLETIMBRE = 3
    CUSTOM_TONENAME = 4
    CUSTOM_DSPTYPE = 5
    CUSTOM_LFOTYPE = 6
    CUSTOM_PORTAMENTO = 7
    CUSTOM_OCTAVESHIFT = 8
    CUSTOM_DSPNAME = 9
    CUSTOM_DSPPARAMS = 10
    CUSTOM_NOTEOFFVELOCITY = 11


@dataclasses.dataclass
class ParamView:
    type_: ViewType
    param: parameters.Param
    offsets: list
    id_: int
    help_: str






class CustomListBox_FilterType(wx.ComboBox):
  
    def __init__(self, parent, id=wx.ID_ANY, value="", pos=wx.DefaultPosition, name=wx.ComboBoxNameStr):
        wx.ComboBox.__init__(self, parent, id, value=value, pos=pos, style=wx.CB_DROPDOWN, name=name, choices=[
            "0" + SEP + _("All-pass"),
            "1" + SEP + _("Low-pass"),
            "2" + SEP + _("High-pass"),
            "3" + SEP + _("Wide band-pass"),
            "4" + SEP + _("Bass shelf"),
            "5" + SEP + _("Treble shelf"),
            "6" + SEP + _("Band boost/cut"),
            "7" + SEP + _("Notch"),
            "8" + SEP + _("All-pass")  ]  )



class CustomListBox_WavetableTimbre(wx.ComboBox):
  
    def __init__(self, parent, id=wx.ID_ANY, value="", pos=wx.DefaultPosition, name=wx.ComboBoxNameStr):
        wx.ComboBox.__init__(self, parent, id, value=value, pos=pos, style=wx.CB_DROPDOWN, name=name, choices=[
            "0" + SEP + _("Melody"),
            "2" + SEP + _("Drum"),
            "4" + SEP + _("Piano"),
            "6" + SEP + _("Versatile")  ]  )


class CustomText_ToneName(wx.TextCtrl):

    def __init__(self, parent, id=wx.ID_ANY, value="", size=wx.Size(192, 30), pos=wx.DefaultPosition, name=wx.TextCtrlNameStr):
        wx.TextCtrl.__init__(self, parent, id, value=value, size=size, pos=pos, style=0, name=name)



class CustomListBox_DSPType(wx.ComboBox):
  
    def __init__(self, parent, id=wx.ID_ANY, value="", pos=wx.DefaultPosition, name=wx.ComboBoxNameStr):
        wx.ComboBox.__init__(self, parent, id, value=value, pos=pos, style=wx.CB_DROPDOWN, name=name, choices=[
            "0" + SEP + _(""),
            "1" + SEP + _("Stereo 3-band EQ"),
            "2" + SEP + _("Compress"),
            "3" + SEP + _("Limiter"),
            "4" + SEP + _("Enhancer"),
            "5" + SEP + _("Reflection"),
            "6" + SEP + _("Phaser"),
            "7" + SEP + _("Chorus"),
            "8" + SEP + _("Flange"),
            "9" + SEP + _("Tremolo"),
            "10" + SEP + _("Auto pan"),
            "11" + SEP + _("Rotary"),
            "12" + SEP + _("Drive rotary"),
            "13" + SEP + _("LFO wah"),
            "14" + SEP + _("Auto wah"),
            "15" + SEP + _("Distortion"),
            "16" + SEP + _("Pitch shift"),
            "17" + SEP + _(""),
            "18" + SEP + _("Ring modulation"),
            "19" + SEP + _("Delay"),
            "20" + SEP + _("Piano"),
            "21" + SEP + _("Stereo 1-band EQ"),
            "22" + SEP + _("Stereo 2-band EQ"),
            "23" + SEP + _("Drive"),
            "24" + SEP + _("Amp cabinet"),
            "25" + SEP + _(""),
            "26" + SEP + _(""),
            "27" + SEP + _("Mono 1-band EQ"),
            "28" + SEP + _("Mono 2-band EQ"),
            "29" + SEP + _("Mono 3-band EQ"),
            "30" + SEP + _("Model wah"),
            "31" + SEP + _("Tone control")   ] )



class CustomListBox_LFOType(wx.ComboBox):
  
    def __init__(self, parent, id=wx.ID_ANY, value="", pos=wx.DefaultPosition, name=wx.ComboBoxNameStr):
        wx.ComboBox.__init__(self, parent, id, value=value, pos=pos, style=wx.CB_DROPDOWN, name=name, choices=[
            "0" + SEP + _("Sine"),
            "1" + SEP + _("Triangle"),
            "2" + SEP + _("Saw up"),
            "3" + SEP + _("Saw down"),
            "4" + SEP + _("1:3 rectangle"),
            "5" + SEP + _("Square"),
            "6" + SEP + _("3:1 rectangle")  ] )



class CustomListBox_Portamento(wx.ComboBox):
  
    def __init__(self, parent, id=wx.ID_ANY, value="", pos=wx.DefaultPosition, name=wx.ComboBoxNameStr):
        wx.ComboBox.__init__(self, parent, id, value=value, pos=pos, style=wx.CB_DROPDOWN, name=name, choices=[
            "0" + SEP + _("Off"),
            "1" + SEP + _("On"),
            "2" + SEP + _("Legato")  ] )



class CustomListBox_NoteOffVelocity(wx.ComboBox):
  
    def __init__(self, parent, id=wx.ID_ANY, value="", pos=wx.DefaultPosition, name=wx.ComboBoxNameStr):
        wx.ComboBox.__init__(self, parent, id, value=value, pos=pos, style=wx.CB_DROPDOWN, name=name, choices=[
            "0" + SEP + _("Normal"),
            "1" + SEP + _("Note-on"),
            "2" + SEP + _("Minimum")  ] )




class HintsPanelGeneric(wx.Panel):
  
  
    def __init__(self, parent, PVList):
        wx.Panel.__init__(self, parent)
        
        self.NAMES = []
        self.PARAMS = PVList
        self.TYPES = []
        
        _sizer = wx.FlexGridSizer(cols=3, gap=wx.Size(5,5))
        _sizer.SetFlexibleDirection(wx.HORIZONTAL)
        
        for i, PV in enumerate(PVList):
          
            if PV.type_ == 1:
                w_ = wx.CheckBox(self, name="C_P{0}".format(PV.id_))
            elif PV.type_ == 2:
                w_ = CustomListBox_FilterType(self, name="C_P{0}".format(PV.id_))
            elif PV.type_ == 3:
                w_ = CustomListBox_WavetableTimbre(self, name="C_P{0}".format(PV.id_))
            elif PV.type_ in [4,9,10]:
                w_ = CustomText_ToneName(self, name="C_P{0}".format(PV.id_))
                # Bind the EVT_TEXT to this specific control ... ComboBoxs also raise this event,
                # and we need to ignore for them.
                self.Bind(wx.EVT_TEXT, self.OnTextChanged, id=w_.Id)
            elif PV.type_ == 5:
                w_ = CustomListBox_DSPType(self, name="C_P{0}".format(PV.id_))
            elif PV.type_ == 6:
                w_ = CustomListBox_LFOType(self, name="C_P{0}".format(PV.id_))
            elif PV.type_ == 7:
                w_ = CustomListBox_Portamento(self, name="C_P{0}".format(PV.id_))
            elif PV.type_ == 8:
                w_ = wx.SpinCtrl(self, min=-4,max=3,initial=0, name="C_P{0}".format(PV.id_))
            elif PV.type_ == 11:
                w_ = CustomListBox_NoteOffVelocity(self, pos=wx.Point(5, 5+i*40), name="C_P{0}".format(PV.id_))
            else:
                if PV.param.bitCount > 16:
                    # Put some arbitrary limit on the maximum field
                    w_ = wx.SpinCtrl(self, min=0,max=1023,initial=0, name="C_P{0}".format(PV.id_))
                else:
                    w_ = wx.SpinCtrl(self, min=0,max=(1 << PV.param.bitCount)-1,initial=0, name="C_P{0}".format(PV.id_))
          
            _sizer.Add(w_, 0, wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT, 5)
          
            name_ = PV.param.name
            u_ = wx.StaticText(self, label=name_, name="L_P{0}".format(PV.id_))
            
            _sizer.Add(u_, 0, wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT, 5)
            
            
            v_ = wx.StaticBitmap(self)
            if PV.help_:
                v_.SetIcon(wx.ArtProvider.GetIcon(wx.ART_HELP))
                v_.SetToolTip(PV.help_)
            _sizer.Add(v_, 0, wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT, 5)
            
            self.NAMES.append(name_)
            self.TYPES.append(PV.type_)
          

        self.SetSizer(_sizer)
        _sizer.Fit(self)

        self.Bind(wx.EVT_SPINCTRL, self.OnValueChanged)
        self.Bind(wx.EVT_CHECKBOX, self.OnCheckChanged)
        self.Bind(wx.EVT_COMBOBOX, self.OnComboBoxSelected)


    def Update(self, sel_id_):
      
        for k, PP in enumerate(self.PARAMS):
            W_ = self.FindWindowByName("L_P{0}".format(PP.id_))
            N_ = self.NAMES[k]
            # Use different methods for emphasis based on the OS. For linux, markup
            # works but bolding messes up the sizing of the statictext.
            # Windows doesn't support markup, but works okay with bolding.
            if sys.platform.startswith("win"):
                if sel_id_ == PP.id_:
                    W_.SetFont(self.Parent._font2)
                else:
                    W_.SetFont(self.Parent._font1)
                W_.SetLabelMarkup(N_)
            else:
                # Linux
                if sel_id_ == PP.id_:
                    W_.SetLabelMarkup("<b>" + N_ + "</b>")
                else:
                    W_.SetLabelMarkup(N_)

    def ReadValues(self, doc_):

        for k, PV in enumerate(self.PARAMS):
            W_ = self.FindWindowByName("C_P{0}".format(PV.id_))
            V_ = self.Parent._buffer.GetParamFrom(PV.param)
            
            if PV.type_ in [4,9,10]:
                W_.SetValue(V_)
            else:
                
                if PV.type_ == 3:
                    V_ = V_ // 2
                elif PV.type_ == 8:
                    V_ = V_ - 4
                
                if PV.type_ in [2,3,5,6,7,11]:
                    W_.SetSelection(V_)
                else:
                    W_.SetValue(V_)
        
        
    def OnValueChanged(self, event):
        w = self.FindWindowById(event.Id)
        if w.Name[0:3] == "C_P":
            p_idx = int(w.Name[3:])
            for PV in self.PARAMS:
                if PV.id_ == p_idx:
                    V_ = event.GetPosition()
                    if PV.type_ == 8:
                        V_ += 4
                    self.Parent._buffer.SetParamTo(PV.param, V_)
                    self.Parent._view.Update()
                    break
        else:
            raise Exception


    def OnCheckChanged(self, event):
        w = self.FindWindowById(event.Id)
        if w.Name[0:3] == "C_P":
            p_idx = int(w.Name[3:])
            for PV in self.PARAMS:
                if PV.id_ == p_idx:
                    self.Parent._buffer.SetParamTo(PV.param, event.GetInt())
                    self.Parent._view.Update()
                    break
        else:
            raise Exception

    def OnTextChanged(self, event):
        w = self.FindWindowById(event.Id)
        if w.Name[0:3] == "C_P":
            p_idx = int(w.Name[3:])
            for PV in self.PARAMS:
                if PV.id_ == p_idx:
                    self.Parent._buffer.SetParamTo(PV.param, event.GetString())
                    self.Parent._view.Update()
                    break
        else:
            raise Exception

    def OnComboBoxSelected(self, event):
        w = self.FindWindowById(event.Id)
        if w.Name[0:3] == "C_P":
            p_idx = int(w.Name[3:])
            for PV in self.PARAMS:
                if PV.id_ == p_idx:
                    V_ = event.GetSelection()
                    if PV.type_ == 3:
                        V_ = 2*V_
                    self.Parent._buffer.SetParamTo(PV.param, V_)
                    self.Parent._view.Update()
                    break
        else:
            raise Exception

    def SetNewVal(self, PV : ParamView, val):
        W_ = self.FindWindowByName("C_P{0}".format(PV.id_))
        if PV.type_ == 1:
            W_.SetValue(bool(val))
        elif PV.type_ in [2, 5, 6, 7, 11]:
            W_.SetSelection(val)
        elif PV.type_ == 3:
            W_.SetSelection(val // 2)
        elif PV.type_ == 8:
            W_.SetValue(val - 4)
        else:
            W_.SetValue(val)
        self.Parent._view.Update()







class ExtParam(parameters.Param):
    def __init__(self, init_vals):
        parameters.Param.__init__(self, **dataclasses.asdict(init_vals))
        
    def SetRandom(self, dd):
        if self.bitCount > 0 and self.bitOffset >= 0:
            X, = struct.unpack_from("<I", dd, self.byteOffset+0x20)
            MASK = ((1 << self.bitCount) - 1) << self.bitOffset
            if self.number in [1, 21]:
                Y = 0
            else:
                Y = random.randint(*self.recommendedLimits)
            X = (X & ~MASK) + (Y << self.bitOffset)
            struct.pack_into("<I", dd, self.byteOffset+0x20, X)

    def SetDefault(self, dd):
        if self.bitCount > 0 and self.bitOffset >= 0:
            X, = struct.unpack_from("<I", dd, self.byteOffset+0x20)
            MASK = ((1 << self.bitCount) - 1) << self.bitOffset
            Y = self.defaultValue
            X = (X & ~MASK) + (Y << self.bitOffset)
            struct.pack_into("<I", dd, self.byteOffset+0x20, X)





class HintsView(wx.EvtHandler):
  
    def __init__(self):
        wx.EvtHandler.__init__(self)
        self._buffer = None
        
        # Process the parameters list to draw out the clusters.
        CLUSTERS_ = {}
        
        for PP in parameters.Params:
            if PP.cluster != "":
                if PP.cluster in CLUSTERS_:
                    CLUSTERS_[PP.cluster].append( (PP.number, PP.block0)  )
                else:
                    CLUSTERS_.update({PP.cluster: [ (PP.number, PP.block0) ]})
            
        self.CLUSTERS = CLUSTERS_
        self._current_cluster = None
        self._current_offset = None
        
        
        # Process offsets only once clusters are done (so the ordering is fixed)
        OFFSETS_ = {}
        
        for PP in parameters.Params:
            if PP.cluster != "":
                if PP.byteOffset not in OFFSETS_:
                    for k, CC in enumerate(self.CLUSTERS):
                        if CC == PP.cluster:
                            for XX in range(PP.byteOffset, PP.byteOffset+PP.byteCount):
                                OFFSETS_.update({XX: k})
                            
        self.OFFSETS = OFFSETS_
        self._paramviewlist = None

    def SetDocument(self, buff):
      
        self._buffer = buff
        self._paramviewlist = None
        if buff is None:
            self.ClearSelected()
        self._current_cluster = None
        self._current_offset = None
        

    @staticmethod
    def MakeParamViewList(numlist):
      
        LIST_ = []
        OFFSETS_ = set() # want to ensure uniqueness
      
        for (num, block0) in numlist:
          
            for k, PP in enumerate(parameters.Params):
                if PP.number == num and PP.block0 == block0:
                    type_ = 0
                    if PP.bitCount == 1:
                        type_ = 1
                    if PP.number == 117:
                        type_ = 2
                    elif PP.number == 1:
                        type_ = 3
                    elif PP.number == 21:
                        type_ = 3
                    elif PP.number == 0:
                        type_ = 4
                    elif PP.number == 85:
                        type_ = 5
                    elif PP.number in [59, 66]:
                        type_ = 6
                    elif PP.number == 116:
                        type_ = 7
                    elif PP.number in [43, 108]:
                        type_ = 8
                    elif PP.number == 84:
                        type_ = 9
                    elif PP.number == 87:
                        type_ = 10
                    elif PP.number == 42:
                        type_ = 11
                    offsets_ = list(range(PP.byteOffset, PP.byteOffset + PP.byteCount))
                    for offset_ in offsets_:
                        if offset_ in OFFSETS_:
                            offset_ = None
                        else:
                            OFFSETS_.add(offset_)
                    LIST_.append( ParamView(type_= type_ , param=PP, offsets=offsets_, id_=k, help_=PP.helpStr) )
        return LIST_


    def ClearSelected(self):
        
        wx.Frame.SetTitle(self, _("Hints"))
            
        _enter = None
        _exit = None
        

        if self._current_cluster is not None:
            _exit = self._current_cluster
            
        
        self._current_offset = None

        if _exit is not None:
            self._sizer.Remove(0)
            self._panel.Destroy()
            self._panel = None
            self._paramviewlist = None
            self._current_cluster = None
            self._sizer.GetStaticBox().SetLabelText("")
                
        if _exit is not None or _enter is not None:
            self._sizer.Fit(self)
            self.Layout()
                
    def SetSelected(self, offset_in_file):
        if offset_in_file >= 0x20 and offset_in_file < 0x1E8 or True:
            offset = offset_in_file - 0x20
            
            if offset >= 0:
                wx.Frame.SetTitle(self, _("Hints") + ": {0:03X}h".format(offset))
            else:
                wx.Frame.SetTitle(self, _("Hints"))
            
            _enter = None
            _exit = None
            
            
            _enter = self.OFFSETS.get(offset, None)
            if self._current_cluster is not None:
                if _enter == self._current_cluster:
                    _enter = None   # Not actually entering
                else:
                    _exit = self._current_cluster
                
            
            self._current_offset = offset

            
            if _exit is not None:
                self._sizer.Remove(0)
                self._panel.Destroy()
                self._panel = None
                self._paramviewlist = None
                self._current_cluster = None
                self._sizer.GetStaticBox().SetLabelText("")
            
            if _enter is not None:
                
                self._paramviewlist = self.MakeParamViewList(list(self.CLUSTERS.items())[_enter][1])

                
                self._panel = HintsPanelGeneric(self, self._paramviewlist)
                self._sizer.Add(self._panel)
                self._sizer.GetStaticBox().SetLabelText(list(self.CLUSTERS.keys())[_enter])
                self._current_cluster = _enter

            
            if self._paramviewlist is not None:
                sel_id = None
                for PV in self._paramviewlist:
                    if offset in PV.offsets:
                        sel_id = PV.id_
                        break
                if self._panel is not None:
                    self._panel.Update(sel_id)

            if _exit is not None or _enter is not None:
                self._sizer.Fit(self)
                self.Layout()


    def GetHighlightList(self):
        if self._paramviewlist is None:
            return []
        else:
            offs_ = set()
            for PV in self._paramviewlist:
                offs_ |= set(list(range(PV.param.byteOffset+0x20, PV.param.byteOffset+0x20 + PV.param.byteCount))  )
            return list(offs_)

    @staticmethod
    def SuggestStep(PV : ParamView):
        if PV.type_ == 1:
            return 1
        elif PV.type_ in [4,9,10]:
            # Non-numeric
            return None
        else:
            if PV.param.recommendedStep >= 1:
                return PV.param.recommendedStep
            return 10



    def UpdateValues(self, doc_):
        raise NotImplemented

    def PositionChanged(self):
        """
        Called by the view controller to indicate that a new value has been typed
        in (*not* changed via any parameter input). Go through each parameter in
        the paramviewlist and see if it's value may have changed.
        """
        # No action in version 1.x.x
        pass


    def UpDown(self, ctrl_val: hexeditview.CtrlVals):
        """
        Process a "increase" or "decrease" key stroke.
        """
        
        if self._paramviewlist is not None and self._current_offset is not None:
            for PV in self._paramviewlist:
                if self._current_offset in PV.offsets:
                    
                    CURR = self._buffer.GetParamFrom(PV.param)
                    STEP = self.SuggestStep(PV)
                    
                    if STEP is None:
                        return   # No action possible
                    
                    X = None
                    
                    if ctrl_val == hexeditview.CtrlVals.MINIMUM:
                        X = PV.param.recommendedLimits[0]
                    elif ctrl_val == hexeditview.CtrlVals.MAXIMUM:
                        X = PV.param.recommendedLimits[1]
                    elif ctrl_val == hexeditview.CtrlVals.INCREASE:
                        X = CURR + STEP
                        if X > PV.param.recommendedLimits[1]:
                            X = PV.param.recommendedLimits[1]
                    elif ctrl_val == hexeditview.CtrlVals.DECREASE:
                        X = CURR - STEP
                        if X < PV.param.recommendedLimits[0]:
                            X = PV.param.recommendedLimits[0]
                    
                    if X is not None and X != CURR:
                        if self._panel is not None:
                            self._panel.SetNewVal(PV, X)
                        self._buffer.SetParamTo(PV.param, X)
                        self._buffer.Modify(True)
                        
                    # Leave the loop after the first parameter has been processed.
                    # That prevents multiple parameters being changed when they
                    # share a byte offset.
                    #
                    # For shared parameters, only one at a time will be bolded within
                    # hints window. Is it the same one that is change here?? At the
                    # moment it's left undefined, and may not necessarily behave as
                    # expected.
                    break


class HintsDialog(wx.Frame, HintsView):
    
    
    def __init__(self, parent):
        HintsView.__init__(self)
        self._parent = parent
        wx.Frame.__init__(self, parent, wx.ID_ANY, _("Hints"), style = wx.FRAME_TOOL_WINDOW | wx.FRAME_FLOAT_ON_PARENT | wx.CAPTION | wx.CLIP_CHILDREN)
        
        _sizer = wx.StaticBoxSizer(wx.VERTICAL, self, "")
        
        _sizer.SetMinSize( wx.Size(100, 100) )
        self.SetSizer(_sizer)
        _sizer.Fit(self)
        self.Bind(wx.EVT_IDLE, self.OnIdle) # Want to adjust the starting position relative to the parent
        
        self._is_1B6 = False
        self._panel = None
        self._sizer = _sizer
        self._view = None
        
        self._font1 = wx.Font(wx.DEFAULT, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self._font2 = wx.Font(wx.DEFAULT, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD)
        

    def OnIdle(self, event):
        self.SetPosition( wx.Point( self._parent.GetPosition().x + self._parent.GetSize().x + 5, self._parent.GetPosition().y + 100) )  # Move the window out of the way.
        self.Unbind(wx.EVT_IDLE) # Only do this function once, right at the start. After that, can unbind
        event.Skip()


    def UpdateValues(self, doc_):
        if self._panel is not None:
            self._panel.ReadValues(doc_)



class DefaultDialog(wx.Dialog):
    """
    A dialog for the user to request setting all parameters to defaults
    """
    
    include_wavetable_saved_value = False
    
    def __init__(self, parent):
        self._includeWavetable = False
        wx.Dialog.__init__(self, parent, -1, _("Set to defaults"), style = wx.DEFAULT_DIALOG_STYLE)

        sizer = wx.BoxSizer(wx.VERTICAL)

        lbl_1 = wx.StaticText(self, wx.ID_ANY, _("Set all values in the tone to default values. This will overwrite all current data."))
        sizer.Add(lbl_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        cbox_1 = wx.CheckBox(self, wx.ID_ANY, _("Include wavetable"), name="IncludeWavetable")
        cbox_1.SetValue(DefaultDialog.include_wavetable_saved_value)
        sizer.Add(cbox_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        btn_1 = wx.Button(self, wx.ID_OK)
        sizer.Add(btn_1, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        btn_2 = wx.Button(self, wx.ID_CANCEL)
        sizer.Add(btn_2, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)

    def GetResult(self):
        w = self.FindWindowByName("IncludeWavetable")
        res = w.GetValue()
        DefaultDialog.include_wavetable_saved_value = res
        return res


class RandomiseDialog(wx.Dialog):
    """
    A dialog for the user to request setting all parameters to random values
    """
    
    include_wavetable_saved_value = False
    
    def __init__(self, parent):
        self._includeWavetable = False
        wx.Dialog.__init__(self, parent, -1, _("Set to random"), style = wx.DEFAULT_DIALOG_STYLE)

        sizer = wx.BoxSizer(wx.VERTICAL)

        lbl_1 = wx.StaticText(self, wx.ID_ANY, _("Set all values in the tone to random values. This will overwrite all current data."))
        sizer.Add(lbl_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)
        cbox_1 = wx.CheckBox(self, wx.ID_ANY, _("Include wavetable"), name="IncludeWavetable")
        cbox_1.SetValue(RandomiseDialog.include_wavetable_saved_value)
        
        sizer.Add(cbox_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        btn_1 = wx.Button(self, wx.ID_OK)
        sizer.Add(btn_1, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        btn_2 = wx.Button(self, wx.ID_CANCEL)
        sizer.Add(btn_2, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)

    def GetResult(self):
        w = self.FindWindowByName("IncludeWavetable")
        res = w.GetValue()
        RandomiseDialog.include_wavetable_saved_value = res
        return res


class ToneDocument(docview.Document):
    """
    The main document class
    """
    
    
    # A "default" data array to use as a starting point.
    CALSINE =  \
      b"CT-X3000\x00\x00\x00\x00\x00\x00\x00\x00TONH\x00\x00\x00\x00\xfe:R\x1a\xc8\x01\x00\x00"                              \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x80\x80\x80\x80\x7f\x00\x01\x00\x7f\x7f\x7f\x00"                    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      b"\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x80\x80\x80\x80\x7f\x00\x01\x00\x7f\x7f\x7f\x00"                    \
      b"\x00\x00\x00@@@@HH@@@@@@@@@@@\xff\x00                \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"   \
      b"\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00"  \
      b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00d"  \
      b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x01"            \
      b"\x00\xc00\x08\x00\xc00\x08\x00\x85\x08CalSine         d\x7f\x02\x02\x02\x7f\x02\x7f\x02\x7f\x00\x00\x7f\x02\x02\x00\x00\x00EODA"

    
    
    
    def __init__(self):
        docview.Document.__init__(self)
        self._data = bytearray( self.CALSINE )
        self._current_changelist = []

    """
    Three functions to make the class behave a lot like a byte-string/byte array:
    """
    def __len__(self):
        return len(self._data)
        
    def __getitem__(self, slice_):
        return self._data[slice_]
        
    def __setitem__(self, slice_, data):
        old_data = copy.copy(self._data)
        self._data[slice_] = data
        CHANGELIST_ = []
        if isinstance(slice_, int):
            if slice_>=0x20 and slice_<0x1E8:
                self._data[0x18:0x1C] = struct.pack("<I", binascii.crc32(self._data[0x20:0x1E8]))
                CHANGELIST_ = [slice_] + list(range(0x18, 0x1C))
            else:
                CHANGELIST_ = [slice_]
        elif isinstance(slice_, slice):
            if slice_.stop>0x20 or slice_.start<0x1E8:
                self._data[0x18:0x1C] = struct.pack("<I", binascii.crc32(self._data[0x20:0x1E8]))
                CHANGELIST_ = list(range(slice_.start,slice_.stop)) + list(range(0x18, 0x1C))
            else:
                CHANGELIST_ = list(range(slice_.start,slice_.stop))
        if self._data != old_data:
            self._current_changelist = [X for X in CHANGELIST_ if old_data[X] != self._data[X]]
            self.Modify(True)


    def GetChangeList(self):
        return self._current_changelist

    def LoadObject(self, file):
        """
        Override this function and call it from your own ``LoadObject`` before
        loading your own data. ``LoadObject`` is called by the framework
        automatically when the document contents need to be loaded.

        Note that the wxPython version simply sends you a Python file object,
        so you can use pickle.
        """
        x = file.read()
        if len(x) == 0x1EC:
            self._data = bytearray(x)
        return True

    def SaveObject(self, file):
        """
        Override this function and call it from your own ``SaveObject`` before
        saving your own data. ``SaveObject`` is called by the framework
        automatically when the document contents need to be saved.

        Note that the wxPython version simply sends you a Python file object,
        so you can use pickle.
        """
        file.write( bytes(self._data) )
        return True

    """
    Two application-specific member functions:
    """
    def SetDocumentDefault(self, include_wavetable=False):

        for p in parameters.Params:
            if p.number not in [0, 84] \
                        and (include_wavetable or p.number not in [115, 41, 1, 2, 21, 22]):
                ExtParam(p).SetDefault(self._data)
        self.Modify(True)


    def SetDocumentRandomise(self, include_wavetable=False):

        for p in parameters.Params:
            """
            Exclude some parameters:
                 - Volumes are excluded, since their behaviour is known and not interesting
                 - String parameters are excluded
                 - Param 109 seems to have a permanent effect, so exclude it to avoid
                     that happening.
                 - Filters. It's possible for these to take on values that result in no sound.
                 - Gain-related key follows. These can cause apparent silence.
            """
            if p.number not in [0, 84, 45, 46, 109, 200] \
                        and (include_wavetable or p.number not in [115, 41, 1, 2, 21, 22])   \
                        and p.number not in [117, 118, 119, 120, 121, 122]   \
                        and p.number not in [47, 48, 201, 202]:
                ExtParam(p).SetRandom(self._data)
        self.Modify(True)


    def GetWriteable(self):
        """
        I'm not sure what this one was supposed to do. Just return True
        """
        return True


    def SetParamTo(self, P : parameters.Param, p_val):
      
      
        if P.number == 0:
            STR_ = p_val.ljust(16, " ")[:16]
            offset_ = P.byteOffset + 0x20
            self[offset_:offset_+16] = STR_.encode('ascii')
            return
            
        if P.number == 84:
            STR_ = p_val.ljust(16, " ")[:16]
            offset_ = P.byteOffset + 0x20
            self[offset_:offset_+16] = STR_.encode('ascii')
            return
            
        if P.number == 87:
            try:
                BYTES_ = bytes.fromhex(p_val).ljust(14, b'\x00')[:14]
            except ValueError:
                # Failed to decode. Assume it's just being changed, and ignore for now.
                return
            offset_ = P.byteOffset + 0x20
            self[offset_:offset_+14] = BYTES_
            return
            
            
      
        if p_val >= (1 << P.bitCount):
            raise Exception("Trying to set value {0} to a field with only {1} bits".format(p_val, P.bitCount))
            
        X, = struct.unpack_from("<I", self._data, P.byteOffset + 0x20)
        
        MASK = ((1 << P.bitCount) - 1) << P.bitOffset
        
        X = X & ~MASK
        X = X | (p_val << P.bitOffset)
        
        
        self[P.byteOffset + 0x20:P.byteOffset + 0x20 + 4] = struct.pack("<I", X)
        
        self._docManager.SetParamTo(P, p_val)
        
        



    def GetParamFrom(self, P : parameters.Param):
      
        if P.number == 0:
            offset_ = P.byteOffset + 0x20
            STR_ = self._data[offset_:offset_+16].decode('ascii')
            return STR_
      
        if P.number == 84:
            offset_ = P.byteOffset + 0x20
            STR_ = self._data[offset_:offset_+16].decode('ascii')
            return STR_
      
        if P.number == 87:
            offset_ = P.byteOffset + 0x20
            STR_ = self._data[offset_:offset_+14].hex(" ").upper()
            return STR_
          
      
      
        X, = struct.unpack_from("<I", self._data, P.byteOffset + 0x20)
        
        X = X >> (P.bitOffset)
        X = X & ((1 << P.bitCount) - 1)
        
        return X


    def SetFilename(self, filename, notifyViews = False):
        """
        Sets the filename for this document. Usually called by the framework.
        If notifyViews is true, :meth:`View.OnChangeFilename` is called for all
        views.
        """
        self._documentFile = filename
        if notifyViews:
            self._documentTemplate.GetDocumentManager()._view.OnChangeFilename()
            #for view in self._documentViews:
            #    view.OnChangeFilename()


    def UpdateAllViews(self, hint=None):
        self._documentTemplate.GetDocumentManager()._view.OnUpdate(None, ("modify", ))



"""
A very simple implementation of the template class. So simple we don't even inherit
from docview.py.
"""
class ToneDocumentTemplate:
  
    def __init__(self, documentManager):
        self._documentManager = documentManager
  
    def GetDocumentManager(self):
        return self._documentManager
  
    @staticmethod
    def GetDescription():
        return _("Casio Tone file")

    @staticmethod
    def GetFileFilter():
        return "*.TON"

    @staticmethod
    def GetDirectory():
        return os.getcwd()

    @staticmethod
    def GetDefaultExtension():
        return "TON"

    @staticmethod
    def FileMatchesTemplate(filename):
        # TODO:
        return True

    @staticmethod
    def IsVisible():
        return True


"""
A document manager for TON files. Again, this is not inherited from docview.py
in order to get a much simplified system.
"""
class ToneDocumentManager(wx.EvtHandler):
    def __init__(self):
        wx.EvtHandler.__init__(self)
        self._docs = []
        self._fileHistory = None
        self._lastDirectory = ""
        self._view = None
        self._view2 = None
        self._template = ToneDocumentTemplate(self)
        
       
    def Close(self):
        self._frame._midi.Close()

    def SetParamTo(self, P : parameters.Param, p_val):
        # No action in version 1.x.x
        pass

    def GetCurrentDocument(self):
        """
        Returns the document associated with the currently active view (if any).
        """
        if len(self._docs) >= 1:
            return self._docs[0]
        else:
            return None

    def OnUpdateUndo(self, event):
        """
        Updates the user interface for the Undo command.
        """
        doc = self.GetCurrentDocument()
        event.Enable(doc is not None and doc.GetCommandProcessor() is not None and doc.GetCommandProcessor().CanUndo())

    def OnUpdateRedo(self, event):
        """
        Updates the user interface for the Redo command.
        """
        doc = self.GetCurrentDocument()
        event.Enable(doc is not None and doc.GetCommandProcessor() is not None and doc.GetCommandProcessor().CanRedo())


    def MakeDefaultName(self):
        return "Untitled.TON"
        
    def SetView(self, view):
        self._view = view

    def SetView2(self, view):
        self._view2 = view

    def OnNew(self):
        _new_doc = ToneDocument()
        self._docs = [ _new_doc ]
        _new_doc.SetDocumentTemplate( self._template )
        _new_doc.OnNewDocument()
        _new_doc.SetCommandProcessor(_new_doc.OnCreateCommandProcessor())
        _new_doc._docManager = self
        self._view.SetDocument(_new_doc)
        self._view2.SetDocument(_new_doc)
        self._view.OnChangeFilename()
        self._view.Update()
        
    def OnSave(self):
        if len(self._docs) >= 1:
            self._docs[0].Save()

    def OnSaveAs(self):
        if len(self._docs) >= 1:
            self._docs[0].SaveAs()

    def OnOpen(self):
        _, path = self.SelectDocumentPath( [ self._template ] , 0, 0)
        if path:
            if len(self._docs) >= 1:
                self._docs[0].OnOpenDocument(path)
            else:
                self.OnNew()
                self._docs[0].OnOpenDocument(path)
            self._view.OnChangeFilename()
            self._view.Update()
    
    def OnSetToDefault(self, include_wavetable=False):
        self._view.DoDefaults(include_wavetable)
      
    def OnSetToRandomise(self, include_wavetable=False):
        self._view.DoRandomise(include_wavetable)
   
    def OnUndo(self):
        """
        Issues an Undo command to the current document's command processor.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        if doc.GetCommandProcessor():
            doc.GetCommandProcessor().Undo()
            self._view.Update()

    def OnRedo(self):
        """
        Issues a Redo command to the current document's command processor.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        if doc.GetCommandProcessor():
            doc.GetCommandProcessor().Redo()
            self._view.Update()

    def OnMenu(self, event):
        Id = event.GetId()
        if Id == wx.ID_NEW:
            self.OnNew()
        elif Id == wx.ID_OPEN:
            self.OnOpen()
        elif Id == wx.ID_SAVE:
            self.OnSave()
        elif Id == wx.ID_SAVEAS:
            self.OnSaveAs()
        elif Id == wx.ID_UNDO:
            self.OnUndo()
        elif Id == wx.ID_REDO:
            self.OnRedo()
            
    
    def DoBindings(self, src):
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_NEW)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_OPEN)
        self.Bind(wx.EVT_MENU, self.OnFileClose, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_SAVE)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_SAVEAS)
    
        self.Bind(wx.EVT_MENU, self.OnMenu, id=EDIT_SETTODEFAULT_ID)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=EDIT_SETTORANDOMISE_ID)

        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateUndo, id=wx.ID_UNDO)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateRedo, id=wx.ID_REDO)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_UNDO)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_REDO)

        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateFileOpen, id=wx.ID_OPEN)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateFileClose, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateFileNew, id=wx.ID_NEW)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateFileSave, id=wx.ID_SAVE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateFileSaveAs, id=wx.ID_SAVEAS)

    def OnFileClose(self, event):
        """
        Closes and deletes the currently active document.
        """
        doc = self.GetCurrentDocument()
        if doc:
            #doc.DeleteAllViews()
            if doc in self._docs:
                self._docs.remove(doc)
            self._view.SetDocument(None)
            self._view2.SetDocument(None)
            self._view.OnChangeFilename()
            self._view.Update()

    def OnUpdateFileOpen(self, event):
        """
        Updates the user interface for the File Open command.
        """
        event.Enable(True)


    def OnUpdateFileClose(self, event):
        """
        Updates the user interface for the File Close command.
        """
        event.Enable(self.GetCurrentDocument() is not None)


    def OnUpdateFileNew(self, event):
        """
        Updates the user interface for the File New command.
        """
        return True


    def OnUpdateFileSave(self, event):
        """
        Updates the user interface for the File Save command.
        """
        doc = self.GetCurrentDocument()
        event.Enable(doc is not None and doc.IsModified())


    def OnUpdateFileSaveAs(self, event):
        """
        Updates the user interface for the File Save As command.
        """
        event.Enable(self.GetCurrentDocument() is not None and self.GetCurrentDocument().GetWriteable())


    def AddFileToHistory(self, filename):
        # TODO:
        pass

    def SelectDocumentPath(self, templates, flags, save):
        """
        Under Windows, pops up a file selector with a list of filters
        corresponding to document templates. The wxDocTemplate corresponding
        to the selected file's extension is returned.

        On other platforms, if there is more than one document template a
        choice list is popped up, followed by a file selector.

        This function is used in :meth:`DocManager.CreateDocument`.
        """
        if wx.Platform == "__WXMSW__" or wx.Platform == "__WXGTK__" or wx.Platform == "__WXMAC__":
            descr = ''
            for temp in templates:
                if temp.IsVisible():
                    if len(descr) > 0:
                        descr = descr + _('|')
                    descr = descr + temp.GetDescription() + _(" (") + temp.GetFileFilter() + _(") |") + temp.GetFileFilter()  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
            descr = _("All|*.*|%s") % descr  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
        else:
            descr = _("*.*")

        dlg = wx.FileDialog(self.FindSuitableParent(),
                               _("Select a File"),
                               wildcard=descr,
                               style=wx.FD_OPEN|wx.FD_FILE_MUST_EXIST|wx.FD_CHANGE_DIR)
        # dlg.CenterOnParent()  # wxBug: caused crash with wx.FileDialog
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
        else:
            path = None
        dlg.Destroy()

        if path:
            theTemplate = self.FindTemplateForPath(path)
            return (theTemplate, path)

        return (None, None)

    def FindSuitableParent(self):
        """
        Returns a parent frame or dialog, either the frame with the current
        focus or if there is no current focus the application's top frame.
        """
        parent = wx.GetApp().GetTopWindow()
        focusWindow = wx.Window.FindFocus()
        if focusWindow:
            while focusWindow and not isinstance(focusWindow, wx.Dialog) and not isinstance(focusWindow, wx.Frame):
                focusWindow = focusWindow.GetParent()
            if focusWindow:
                parent = focusWindow
        return parent

    def FindTemplateForPath(self, path):
        return self._template

class AboutDialog(wx.Dialog):
    """
    Opens an AboutDialog.
    """

    def __init__(self, parent, image=None):
        """
        Initializes the about dialog.
        """
        wx.Dialog.__init__(self, parent, -1, _("About ") + wx.GetApp().GetAppName(), style = wx.DEFAULT_DIALOG_STYLE)

        sizer = wx.BoxSizer(wx.VERTICAL)
        
        _icon = wx.StaticBitmap(self, wx.ID_ANY)
        _icon.SetIcon(wx.Icon(ICON_LOCATION))
        sizer.Add(_icon, wx.ALIGN_LEFT)

        sizer.Add(wx.StaticText(self, -1, wx.GetApp().GetAppName() + " v" + str(__version__)), 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        sizer.Add(wx.StaticText(self, -1, _build_signifier), 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        sizer.Add(wx.StaticText(self, -1, _(u"\u00A9 2022")), 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        sizer.Add(wx.StaticText(self, -1, "https://github.com/michgz/ToneTyrant"), 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        btn = wx.Button(self, wx.ID_OK)
        sizer.Add(btn, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)


class HelpDialog(wx.Dialog):
    """
    Opens a HelpDialog.
    """
    
    helpText = """Keyboard shortcuts:
    
    \u2190\u2191\u2192\u2193\t(Arrow keys) navigate around the tone file
    PG.UP/PG.DN\tIncrease/decrease the parameter by some amount
    HOME/END\tSet the parameter to its minimum/maximum recommended value. (Larger/
    \t\tsmaller values might be possible but will probably not sound very
    \t\tmusical.)"""
    

    def __init__(self, parent, image=None):
        """
        Initializes the help dialog.
        """
        wx.Dialog.__init__(self, parent, -1, _("Help"), style = wx.DEFAULT_DIALOG_STYLE)

        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        sizer_1.Add(wx.StaticText(self, -1, _("Keyboard shortcuts:")), 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        
        sizer_2 = wx.FlexGridSizer(cols=2, gap=wx.Size(5, 5))
        
        sizer_2.Add(wx.StaticText(self, -1, u"\u2190\u2191\u2192\u2193"))
        sizer_2.Add(wx.StaticText(self, -1, "(Arrow keys) navigate around the tone file"))
        sizer_2.Add(wx.StaticText(self, -1, "PG.UP/PG.DN"))
        sizer_2.Add(wx.StaticText(self, -1, "Increase/decrease the parameter by some amount"))
        sizer_2.Add(wx.StaticText(self, -1, "HOME/END"))
        stxt_1 = wx.StaticText(self, -1, "Set the parameter to its minimum/maximum recommended value. (Larger/smaller values might be possible but will probably not sound very musical")
        stxt_1.Wrap(350)
        sizer_2.Add(stxt_1)
        
        sizer_1.Add(sizer_2)

        btn = wx.Button(self, wx.ID_OK)
        sizer_1.Add(btn, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer_1)
        sizer_1.Fit(self)


"""
A top-level frame for our application
"""
class ToneParentFrame(wx.Frame):
  
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title)
        self._fileMenu = None  
        self._image = None
        self._midi = MidiComms()
        menuBar = self.CreateDefaultMenuBar(sdi=True)
        self.SetMenuBar(menuBar)  # wxBug: Need to do this in SDI to mimic MDI... because have to set the menubar at the very end or the automatic MDI "window" menu doesn't get put in the right place when the services add new menus to the menubar

    def CreateDefaultMenuBar(self, sdi=False):
        """
        Creates the default MenuBar.  Contains File, Edit, View, Tools, and Help menus.
        """
        menuBar = wx.MenuBar()

        fileMenu = wx.Menu()
        fileMenu.Append(wx.ID_NEW, _("&New...\tCtrl+N"), _("Creates a new document"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_NEW)
        fileMenu.Append(wx.ID_OPEN, _("&Open...\tCtrl+O"), _("Opens an existing document"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_OPEN)
        fileMenu.Append(wx.ID_CLOSE, _("&Close"), _("Closes the active document"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=wx.ID_CLOSE)
        fileMenu.Enable(wx.ID_CLOSE, False)
        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_SAVE, _("&Save\tCtrl+S"), _("Saves the active document"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_SAVE)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=wx.ID_SAVE)
        fileMenu.Enable(wx.ID_SAVE, False)
        fileMenu.Append(wx.ID_SAVEAS, _("Save &As..."), _("Saves the active document with a new name"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_SAVEAS)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=wx.ID_SAVEAS)
        fileMenu.Enable(wx.ID_SAVEAS, False)
        fileMenu.AppendSeparator()
        if wx.Platform == '__WXMAC__':
            fileMenu.Append(wx.ID_EXIT, _("&Quit"), _("Closes this program"))
        else:
            fileMenu.Append(wx.ID_EXIT, _("E&xit"), _("Closes this program"))
        #self._docManager.FileHistoryUseMenu(fileMenu)
        #self._docManager.FileHistoryAddFilesToMenu()
        menuBar.Append(fileMenu, _("&File"))
        
        editMenu = wx.Menu()
        editMenu.Append(wx.ID_UNDO, _("&Undo\tCtrl+Z"), _("Reverses the last action"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_UNDO)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessEvent2, id=wx.ID_UNDO)
        editMenu.Enable(wx.ID_UNDO, False)
        editMenu.Append(wx.ID_REDO, _("&Redo\tCtrl+Y"), _("Reverses the last undo"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_REDO)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessEvent2, id=wx.ID_REDO)
        editMenu.Enable(wx.ID_REDO, False)
        editMenu.AppendSeparator()
        editMenu.Append(EDIT_SETTODEFAULT_ID, _("&Default...\tCtrl+D"), _("Sets to default values"))
        self.Bind(wx.EVT_MENU, self.OnDefault, id=EDIT_SETTODEFAULT_ID)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=EDIT_SETTODEFAULT_ID)
        editMenu.Enable(EDIT_SETTODEFAULT_ID, False)
        editMenu.Append(EDIT_SETTORANDOMISE_ID, _("&Randomise...\tCtrl+R"), _("Sets to randomised values"))
        self.Bind(wx.EVT_MENU, self.OnRandomise, id=EDIT_SETTORANDOMISE_ID)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=EDIT_SETTORANDOMISE_ID)
        editMenu.Enable(EDIT_SETTORANDOMISE_ID, False)
        menuBar.Append(editMenu, _("&Edit"))
        
        midiMenu = wx.Menu()
        midiMenu.Append(MIDI_SETUP_ID, _("&Setup...\tCtrl+M"), _("Sets up the MIDI communications"))
        midiMenu.AppendSeparator()
        midiMenu.Append(MIDI_DOWNLOAD_ID, _("&Download...\tF2"), _("Downloads a tone from the keyboard"))
        midiMenu.Append(MIDI_UPLOAD_ID, _("&Upload...\tF3"), _("Uploads a tone from the keyboard"))
        # All items in this menu are disabled for now - functionality to be added in a future version
        midiMenu.Enable(MIDI_SETUP_ID, self._midi.AllowMidi())
        self.Bind(wx.EVT_MENU, self.OnMidiSetup, id=MIDI_SETUP_ID)
        midiMenu.Enable(MIDI_DOWNLOAD_ID, self._midi.AllowMidi())
        self.Bind(wx.EVT_MENU, self.OnMidiDownload, id=MIDI_DOWNLOAD_ID)
        midiMenu.Enable(MIDI_UPLOAD_ID, self._midi.AllowMidi())
        self.Bind(wx.EVT_MENU, self.OnMidiUpload, id=MIDI_UPLOAD_ID)
        menuBar.Append(midiMenu, _("&MIDI"))
        
        helpMenu = wx.Menu()
        helpMenu.Append(wx.ID_HELP, _("&Help"), _("Displays help information"))
        helpMenu.Append(wx.ID_ABOUT, _("&About"), _("Displays program information, version number, and copyright"))
        menuBar.Append(helpMenu, _("&Help"))

        self.Bind(wx.EVT_MENU, self.OnAbout, id=wx.ID_ABOUT)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=wx.ID_ABOUT)  # Using ID_ABOUT to update the window menu, the window menu items are not triggering

        self.Bind(wx.EVT_MENU, self.OnHelp, id=wx.ID_HELP)

        self.Bind(wx.EVT_MENU, self.OnExit, id=wx.ID_EXIT)

        return menuBar

    def ProcessEvent2(self, event):
        if self._docManager is not None:
            self._docManager.ProcessEvent(event)

    def OnMidiSetup(self, event):
        self._midi.ShowMidiSetup(self)

    def OnMidiUpload(self, event):
        self._midi.ShowMidiUpload(self, self._docManager._docs[0])

    def OnMidiDownload(self, event):
        self._midi.ShowMidiDownload(self, self._docManager._docs[0])

    def SetDocManager(self, dm):
        self._docManager = dm
        dm.DoBindings(self)

    def ShowAbout(self):
        """
        Show the AboutDialog
        """
        if self._image:
            dlg = AboutDialog(wx.GetApp().GetTopWindow(), self._image)
        else:
            dlg = AboutDialog(wx.GetApp().GetTopWindow())
        dlg.CenterOnParent()
        dlg.ShowModal()
        dlg.Destroy()

    def ShowHelp(self):
        """
        Show the HelpDialog
        """
        dlg = HelpDialog(wx.GetApp().GetTopWindow())
        dlg.CenterOnParent()
        dlg.ShowModal()
        dlg.Destroy()

    def OnAbout(self, event):
        self.ShowAbout()

    def OnHelp(self, event):
        self.ShowHelp()

    def OnDefault(self, event):
        dlg = DefaultDialog(self)
        dlg.CenterOnParent()
        id_result = dlg.ShowModal()
        req_result = dlg.GetResult()
        dlg.Destroy()
        
        if id_result == wx.ID_OK:  # Only process if the user selected "okay"
            self._docManager.OnSetToDefault(req_result)

    def OnRandomise(self, event):
        dlg = RandomiseDialog(self)
        dlg.CenterOnParent()
        id_result = dlg.ShowModal()
        req_result = dlg.GetResult()
        dlg.Destroy()
        
        if id_result == wx.ID_OK:  # Only process if the user selected "okay"
            self._docManager.OnSetToRandomise(req_result)

    def OnExit(self, event):
        """
        Called when the application is exitting.
        """
        #self._childView.GetDocumentManager().Clear(force = False)
        self.Close()

    def ProcessUpdateUIEvent(self, event):
        Id = event.GetId()
        if Id == EDIT_SETTODEFAULT_ID or Id == EDIT_SETTORANDOMISE_ID:
            event.Enable(self._docManager.GetCurrentDocument() is not None)
        elif Id == wx.ID_CLOSE or Id==wx.ID_SAVE or Id==wx.ID_SAVEAS:
            self._docManager.ProcessEvent(event)

    def GetDocument(self):
        if not self._docManager:
            return None
        return self._docManager.GetCurrentDocument()

    def OnTitleIsModified(self):
        """
        Add/remove to the frame's title an indication that the document is dirty.
        If the document is dirty, an '*' is appended to the title
        """
        title = self.GetTitle()
        if title:
            _doc = self.GetDocument()
            if _doc is not None:
                if _doc.IsModified():
                    if title.endswith("*"):
                        return
                    else:
                        title = title + "*"
                        self.SetTitle(title)
                else:
                    if title.endswith("*"):
                        title = title[:-1]
                        self.SetTitle(title)
                    else:
                        return


"""
The main function
"""
def main():
  
    global ICON_LOCATION
    global _build_signifier
  
    # Set up the root logger
    _logger = logging.getLogger()
    _logger.setLevel(logging.WARNING)
    _fh = logging.FileHandler('log.txt')
    _fh.setLevel(logging.DEBUG)
    _logger.addHandler(_fh)

    # Start the app
    _app = wx.App(False)
    _app.SetAppName(_("ToneTyrant"))

    # Initialise the document-view system
    _docManager = ToneDocumentManager()
    _frame = ToneParentFrame(None,wx.ID_ANY,"Untitled.TON")
    _frame.SetDocManager(_docManager)
    _frame.SetSize(wx.DefaultCoord, wx.DefaultCoord, 620, 580)

    
    # Find the location of the icon file.
    if getattr(sys, 'frozen', False):
        # This is a frozen build created by pyinstaller. See:
        #     https://stackoverflow.com/questions/45628653/add-ico-file-to-executable-in-pyinstaller
        application_path = sys._MEIPASS
        _build_signifier = "(Built by PyInstaller)"
    elif str(sys.argv[0]).lower().endswith(".py"):
        # Being run directly with python -- not a frozen build
        application_path = os.path.dirname(os.path.dirname(__file__))
    elif __file__:
        # This is probably a frozen build created by nuitka.
        application_path = os.path.dirname(__file__)
        _build_signifier = "(Built by Nuitka)"

    iconFile = 'tyrant-64x64.ico'
    
    ICON_LOCATION = os.path.join(application_path, iconFile)

    
    _frame.SetIcon(wx.Icon(ICON_LOCATION))
    
    _view = hexeditview.HexEditView(_frame)
    _docManager.SetView(_view)


    _hintsDlg = HintsDialog(_frame)
    _hintsDlg.Show()  # Non-modal



    _view._callback_window = _hintsDlg
    _hintsDlg._view = _view
    _docManager.SetView2(_hintsDlg)
    _docManager._frame = _frame

    # Run the application
    _frame.Show(True)
    _view.OnChangeFilename()
    _app.MainLoop() 
    
    
    # End the application
    _docManager.Close()
    


if __name__=="__main__":
    main()


