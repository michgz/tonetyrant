#!/usr/bin/env python
import wx
import wx.adv
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

__version__ = "1.0.0"

def _(X):
    return X


EDIT_SETTODEFAULT_ID = wx.NewIdRef()
EDIT_SETTORANDOMISE_ID = wx.NewIdRef()

MIDI_SETUP_ID = wx.NewIdRef()
MIDI_DOWNLOAD_ID = wx.NewIdRef()
MIDI_UPLOAD_ID = wx.NewIdRef()



P110_ID = wx.NewIdRef()
P111_ID = wx.NewIdRef()
P112_ID = wx.NewIdRef()


P1_ID = wx.NewIdRef()
P2_ID = wx.NewIdRef()

P3_ID = wx.NewIdRef()
P4_ID = wx.NewIdRef()
P5_ID = wx.NewIdRef()


P6B0_ID = wx.NewIdRef()
P6B1_ID = wx.NewIdRef()
P6B2_ID = wx.NewIdRef()
P7B0_ID = wx.NewIdRef()
P7B1_ID = wx.NewIdRef()
P7B2_ID = wx.NewIdRef()


class P1B6_Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        
        ctl_1 = wx.SpinCtrl(self, pos=wx.Point(5, 5), min=0,max=127,initial=100)
        wx.StaticText(self, pos=wx.Point(105,5+5), label="Volume")
    
    def Update(self, offset):
        pass
    
class P110_Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        
        ctl_1 = wx.SpinCtrl(self, pos=wx.Point(5, 5), min=0,max=127,initial=40)
        ctl_2 = wx.SpinCtrl(self, pos=wx.Point(5, 45), min=0,max=127,initial=0)
        ctl_3 = wx.SpinCtrl(self, pos=wx.Point(5, 85), min=0,max=127,initial=0)
        
        wx.StaticText(self, id=P110_ID, pos=wx.Point(105,5+5), label="Reverb send")
        wx.StaticText(self, id=P111_ID, pos=wx.Point(105,45+5), label="Chorus send")
        wx.StaticText(self, id=P112_ID, pos=wx.Point(105,85+5), label="Delay send")

    def Update(self, offset):
      
        ALL = [(0x110, P110_ID, _("Reverb send")), (0x111, P111_ID, _("Chorus send")), (0x112, P112_ID, _("Delay send"))]
        
        for aa in ALL:
            if offset == aa[0]:
                self.FindWindowById(aa[1]).SetLabelMarkup("<b>" + aa[2] + "</b>")
            else:
                self.FindWindowById(aa[1]).SetLabelMarkup(aa[2])

class P000_Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        
        ctl_1 = wx.SpinCtrl(self, pos=wx.Point(5, 5), min=0,max=255,initial=128)
        ctl_2 = wx.SpinCtrl(self, pos=wx.Point(5, 45), min=0,max=255,initial=128)
        ctl_3 = wx.SpinCtrl(self, pos=wx.Point(5, 85), min=0,max=255,initial=128)

        ctl_4 = wx.SpinCtrl(self, pos=wx.Point(5, 135), min=0,max=1023,initial=256)
        ctl_5 = wx.SpinCtrl(self, pos=wx.Point(5, 175), min=0,max=1023,initial=256)
        ctl_6 = wx.SpinCtrl(self, pos=wx.Point(5, 215), min=0,max=1023,initial=256)

        
        wx.StaticText(self, id=P6B0_ID, pos=wx.Point(105,5+5), label="")
        wx.StaticText(self, id=P6B1_ID, pos=wx.Point(105,45+5), label="")
        wx.StaticText(self, id=P6B2_ID, pos=wx.Point(105,85+5), label="")

        wx.StaticText(self, id=P7B0_ID, pos=wx.Point(105,135+5), label="")
        wx.StaticText(self, id=P7B1_ID, pos=wx.Point(105,175+5), label="")
        wx.StaticText(self, id=P7B2_ID, pos=wx.Point(105,215+5), label="")


    def Update(self, offset):
      
        ALL = [(0x002, 0x003, P6B0_ID, _("Attack level")),
               (0x006, 0x007, P6B1_ID, _("Hold level")),
               (0x00A, 0x00B, P6B2_ID, _("Release level")),
               (0x000, 0x001, P7B0_ID, _("unused time")),
               (0x004, 0x005, P7B1_ID, _("Attack time")),
               (0x008, 0x009, P7B2_ID, _("Release time")),
              ]

        
        for aa in ALL:
            if offset == aa[0] or offset == aa[1]:
                self.FindWindowById(aa[2]).SetLabelMarkup("<b>" + aa[3] + "</b>")
            else:
                self.FindWindowById(aa[2]).SetLabelMarkup(aa[3])

class P084_Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        
        ctl_1 = wx.SpinCtrl(self, pos=wx.Point(5, 5), min=-64,max=63,initial=0)
        ctl_2 = wx.SpinCtrl(self, pos=wx.Point(5, 45), min=-64,max=63,initial=0)
        ctl_3 = wx.SpinCtrl(self, pos=wx.Point(5, 85), min=-64,max=63,initial=63)


        
        wx.StaticText(self, id=P3_ID, pos=wx.Point(105,5+5), label="")
        wx.StaticText(self, id=P4_ID, pos=wx.Point(105,45+5), label="")
        wx.StaticText(self, id=P5_ID, pos=wx.Point(105,85+5), label="")



    def Update(self, offset):
      
        ALL = [(0x084, P3_ID, _("")),
               (0x085, P4_ID, _("Velocity to filter")),
               (0x086, P5_ID, _("Velocity sense")),
              ]

        
        for aa in ALL:
            if offset == aa[0]:
                self.FindWindowById(aa[1]).SetLabelMarkup("<b>" + aa[2] + "</b>")
            else:
                self.FindWindowById(aa[1]).SetLabelMarkup(aa[2])


class P082_Panel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        
        ctl_1 = wx.ComboBox(self, pos=wx.Point(5, 5), choices=["0\tMelody","2\tDrum","4\tPiano","6\tVersatile"], style=wx.CB_READONLY)
        ctl_2 = wx.SpinCtrl(self, pos=wx.Point(5, 45), min=0,max=899,initial=1)



        
        wx.StaticText(self, id=P1_ID, pos=wx.Point(125,5+5), label="")
        wx.StaticText(self, id=P2_ID, pos=wx.Point(105,45+5), label="")



    def Update(self, offset):
      
        ALL = [(0x087, -1, P1_ID, _("Type")),
               (0x082, 0x083, P2_ID, _("Wavetable")),
              ]

        
        for aa in ALL:
            if offset == aa[0] or offset == aa[1]:
                self.FindWindowById(aa[2]).SetLabelMarkup("<b>" + aa[3] + "</b>")
            else:
                self.FindWindowById(aa[2]).SetLabelMarkup(aa[3])






class Cluster:
    def __init__(self, name, offset_range, class_panel):
        self._name = name
        self._offset_range = offset_range
        self._class_panel = class_panel

clusters = [
    Cluster(_("Pitch envelope (Sound A)"), range(0x000,0x00C), P000_Panel),
    Cluster(_("Wavetable (Sound A)"), [0x82, 0x83, 0x87], P082_Panel),
    Cluster(_("Velocity (Sound A)"), range(0x084,0x087), P084_Panel),
    Cluster(_("Volume"), [0x1B6], P1B6_Panel),
    Cluster(_("Effect Sends"), [0x110, 0x111, 0x112], P110_Panel),
    ]


class ExtParam(parameters.Param):
    def __init__(self, init_vals):
        parameters.Param.__init__(self, **dataclasses.asdict(init_vals))
        
    def SetRandom(self, dd):
        if self.bitCount > 0 and self.bitOffset >= 0:
            X, = struct.unpack_from("<I", dd, self.byteOffset+0x20)
            MASK = ((1 << self.bitCount) - 1) << self.bitOffset
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





class HintsDialog(wx.Dialog):
    
    
    def __init__(self, parent):
        self._parent = parent
        wx.Dialog.__init__(self, parent, wx.ID_ANY, _("Hints"), style = wx.DEFAULT_DIALOG_STYLE)
        
        _sizer = wx.StaticBoxSizer(wx.VERTICAL, self, "")
        
        _sizer.SetMinSize( wx.Size(100, 100) )
        self.SetSizer(_sizer)
        _sizer.Fit(self)
        self.Bind(wx.EVT_IDLE, self.OnIdle) # Want to adjust the starting position relative to the parent
        
        self._is_1B6 = False
        self._current_cluster = ""
        self._panel = None
        self._sizer = _sizer
        

    def OnIdle(self, event):
        self.SetPosition( wx.Point( self._parent.GetPosition().x + self._parent.GetSize().x + 5, self.GetPosition().y) )  # Move the window out of the way.
        self.Unbind(wx.EVT_IDLE) # Only do this function once, right at the start. After that, can unbind
        event.Skip()


    def SetSelected(self, offset_in_file):
        
        if offset_in_file >= 0x20 and offset_in_file < 0x1E8 or True:
            offset = offset_in_file - 0x20
            
            _enter = None
            _exit = None
            
            for cc in clusters:
                if offset in cc._offset_range and self._current_cluster != cc._name:
                    # Entering this cluster
                    _enter = cc
                elif offset not in cc._offset_range and self._current_cluster == cc._name:
                    # Exiting this cluster
                    _exit = cc
            
            if _exit is not None:
                self._sizer.Remove(0)
                self._panel.Destroy()
                self._panel = None
                self._current_cluster = ""
                self._sizer.GetStaticBox().SetLabelText("")
            
            if _enter is not None:
                self._panel = _enter._class_panel(self)
                self._sizer.Add(self._panel)
                self._sizer.GetStaticBox().SetLabelText(_enter._name)
                self._current_cluster = _enter._name
            
            if self._panel is not None:
                self._panel.Update(offset)

            if _exit is not None or _enter is not None:
                self._sizer.Fit(self)
                self.Layout()


class DefaultDialog(wx.Dialog):
    """
    A dialog for the user to request setting all parameters to defaults
    """
    def __init__(self, parent):
        self._includeWavetable = False
        wx.Dialog.__init__(self, parent, -1, _("Set to defaults"), style = wx.DEFAULT_DIALOG_STYLE)

        sizer = wx.BoxSizer(wx.VERTICAL)

        lbl_1 = wx.StaticText(self, wx.ID_ANY, _("Set all values in the tone to default values. This will overwrite all current data."))
        sizer.Add(lbl_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        cbox_1 = wx.CheckBox(self, wx.ID_ANY, _("Include wavetable"), name="IncludeWavetable")
        sizer.Add(cbox_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        btn_1 = wx.Button(self, wx.ID_OK)
        sizer.Add(btn_1, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        btn_2 = wx.Button(self, wx.ID_CANCEL)
        sizer.Add(btn_2, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)

    def GetResult(self):
        w = self.FindWindowByName("IncludeWavetable")
        return w.GetValue()

class RandomiseDialog(wx.Dialog):
    """
    A dialog for the user to request setting all parameters to random values
    """
    def __init__(self, parent):
        self._includeWavetable = False
        wx.Dialog.__init__(self, parent, -1, _("Set to random"), style = wx.DEFAULT_DIALOG_STYLE)

        sizer = wx.BoxSizer(wx.VERTICAL)

        lbl_1 = wx.StaticText(self, wx.ID_ANY, _("Set all values in the tone to random values. This will overwrite all current data."))
        sizer.Add(lbl_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        cbox_1 = wx.CheckBox(self, wx.ID_ANY, _("Include wavetable"), name="IncludeWavetable")
        sizer.Add(cbox_1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT, 5)

        btn_1 = wx.Button(self, wx.ID_OK)
        sizer.Add(btn_1, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        btn_2 = wx.Button(self, wx.ID_CANCEL)
        sizer.Add(btn_2, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)

    def GetResult(self):
        w = self.FindWindowByName("IncludeWavetable")
        return w.GetValue()



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

    """
    Three functions to make the class behave a lot like a byte-string/byte array:
    """
    def __len__(self):
        return len(self._data)
        
    def __getitem__(self, slice):
        return self._data[slice]
        
    def __setitem__(self, slice, data):
        self._data[slice] = data



    def LoadObject(self, file):
        """
        Override this function and call it from your own ``LoadObject`` before
        loading your own data. ``LoadObject`` is called by the framework
        automatically when the document contents need to be loaded.

        Note that the wxPython version simply sends you a Python file object,
        so you can use pickle.
        """
        x = file.read()
        if len(x) == 0x1E8:
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
            if p.number not in [0, 84, 45, 46, 200] \
                        and (include_wavetable or p.number not in [115, 41, 1, 2, 21, 22]):
                ExtParam(p).SetRandom(self._data)
        self.Modify(True)



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
        self._template = ToneDocumentTemplate(self)

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

    def OnNew(self):
        _new_doc = ToneDocument()
        self._docs = [ _new_doc ]
        _new_doc.SetDocumentTemplate( self._template )
        _new_doc.OnNewDocument()
        _new_doc.SetCommandProcessor(_new_doc.OnCreateCommandProcessor())
        self._view.SetDocument(_new_doc)
        
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
    
    def OnSetToDefault(self, include_wavetable=False):
        if len(self._docs) >= 1:
            self._docs[0].SetDocumentDefault(include_wavetable)
      
    def OnSetToRandomise(self, include_wavetable=False):
        if len(self._docs) >= 1:
            self._docs[0].SetDocumentRandomise(include_wavetable)
   
    def OnUndo(self):
        """
        Issues an Undo command to the current document's command processor.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        if doc.GetCommandProcessor():
            doc.GetCommandProcessor().Undo()


    def OnRedo(self):
        """
        Issues a Redo command to the current document's command processor.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        if doc.GetCommandProcessor():
            doc.GetCommandProcessor().Redo()
   
   
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
        if image:
            imageItem = wx.StaticBitmap(self, -1, image.ConvertToBitmap(), (0,0), (image.GetWidth(), image.GetHeight()))
            sizer.Add(imageItem, 0, wx.ALIGN_CENTER|wx.ALL, 0)
        sizer.Add(wx.StaticText(self, -1, wx.GetApp().GetAppName()), 0, wx.ALIGN_CENTRE|wx.ALL, 5)

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

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(wx.StaticText(self, -1, HelpDialog.helpText), 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        btn = wx.Button(self, wx.ID_OK)
        sizer.Add(btn, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)


"""
A top-level frame for our application
"""
class ToneParentFrame(wx.Frame):
  
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title)
        self._fileMenu = None  
        self._image = None
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
        midiMenu.Enable(MIDI_SETUP_ID, False)
        midiMenu.Enable(MIDI_DOWNLOAD_ID, False)
        midiMenu.Enable(MIDI_UPLOAD_ID, False)
        menuBar.Append(midiMenu, _("&MIDI"))
        
        helpMenu = wx.Menu()
        helpMenu.Append(wx.ID_HELP, _("&Help"), _("Displays help information"))
        helpMenu.Append(wx.ID_ABOUT, _("&About" + " " + wx.GetApp().GetAppName()), _("Displays program information, version number, and copyright"))
        menuBar.Append(helpMenu, _("&Help"))

        self.Bind(wx.EVT_MENU, self.OnAbout, id=wx.ID_ABOUT)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=wx.ID_ABOUT)  # Using ID_ABOUT to update the window menu, the window menu items are not triggering

        self.Bind(wx.EVT_MENU, self.OnHelp, id=wx.ID_HELP)

        self.Bind(wx.EVT_MENU, self.OnExit, id=wx.ID_EXIT)

        return menuBar

    def ProcessEvent2(self, event):
        if self._docManager is not None:
            self._docManager.ProcessEvent(event)


    def SetDocManager(self, dm):
        self._docManager = dm
        dm.DoBindings(self)

    def ShowAbout(self):
        """
        Show the AboutDialog
        """
        #if self._image:
        #    dlg = AboutDialog(wx.GetApp().GetTopWindow(), self._image)
        #else:
        #    dlg = AboutDialog(wx.GetApp().GetTopWindow())
        #dlg.CenterOnParent()
        #dlg.ShowModal()
        #dlg.Destroy()
        
        aboutInfo = wx.adv.AboutDialogInfo()
        aboutInfo.SetName("Tone Tyrant for Casio")
        aboutInfo.SetVersion(__version__)
        aboutInfo.SetDescription("Tone editor for Casio CT-X keyboards and others.")
        aboutInfo.SetCopyright("(C) 2022")
        aboutInfo.SetWebSite("https://github.com/michgz/tonetyrant")
        aboutInfo.AddDeveloper("michgz")
        aboutInfo.SetIcon(wx.Icon("tyrant-64x64.ico"))
        
        wx.adv.AboutBox(aboutInfo)
        
        
        

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


"""
The main function
"""
def main():
  
    # Set up the root logger
    _logger = logging.getLogger()
    _logger.setLevel(logging.DEBUG)
    _fh = logging.FileHandler('log.txt')
    _fh.setLevel(logging.DEBUG)
    _logger.addHandler(_fh)

    # Start the app
    _app = wx.App(False)

    # Initialise the document-view system
    _docManager = ToneDocumentManager()
    _frame = ToneParentFrame(None,wx.ID_ANY,"Untitled.TON")
    _frame.SetDocManager(_docManager)
    _frame.SetSize(wx.DefaultCoord, wx.DefaultCoord, 620, 580)
    _view = hexeditview.HexEditView(_frame)
    _docManager.SetView(_view)


    _hintsDlg = HintsDialog(_frame)
    _hintsDlg.Show()  # Non-modal



    _view._callback_window = _hintsDlg

    # Run the application
    _frame.Show(True)
    _app.MainLoop() 


if __name__=="__main__":
    main()


