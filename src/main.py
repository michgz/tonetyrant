#!/usr/bin/env python
import wx
import sys
import logging
import os

sys.path.append("src")

import hexeditview
import docview



def _(X):
    return X


EDIT_SETTODEFAULT_ID = wx.NewIdRef()
EDIT_SETTORANDOMISE_ID = wx.NewIdRef()


class DefaultDialog(wx.Dialog):
    """
    A dialog for the user to request setting all parameters to defaults
    """
    def __init__(self, parent):
        self._includeWavetable = False
        wx.Dialog.__init__(self, parent, -1, _("Set to defaults") + wx.GetApp().GetAppName(), style = wx.DEFAULT_DIALOG_STYLE)

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
        wx.Dialog.__init__(self, parent, -1, _("Set to random") + wx.GetApp().GetAppName(), style = wx.DEFAULT_DIALOG_STYLE)

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
    def __init__(self):
        docview.Document.__init__(self)
        self._data = bytearray( b'\x00' * 0x1E8 )

    """
    Three functions to make the class behave a lot like a byte-string/byte array:
    """
    def __len__(self):
        return len(self._data)
        
    def __getitem__(self, slice):
        return self._data[slice]
        
    def __setitem__(self, slice, data):
        self._data[slice] = data



    def Open(self):
        """
        A new function not supported by the parent class
        """
        _, path = self.SelectDocumentPath(None, 0, 0)
        if path:
            self.OnOpenDocument(path)


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

        for p in Parameters:
            p.SetDefault(self._data)
        self.Modify(True)


    def SetDocumentRandomise(self, include_wavetable=False):
        # ...
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
        if doc and doc.GetCommandProcessor():
            doc.GetCommandProcessor().SetMenuStrings()
        else:
            event.SetText(_("&Undo\tCtrl+Z"))

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
        if len(self._docs) >= 1:
            self._docs[0].Open()
        else:
            self.OnNew()
            self._docs[0].Open()
    
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
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_SAVE)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_SAVEAS)
    
        self.Bind(wx.EVT_MENU, self.OnMenu, id=EDIT_SETTODEFAULT_ID)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=EDIT_SETTORANDOMISE_ID)

        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateUndo, id=wx.ID_UNDO)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_UNDO)
        self.Bind(wx.EVT_MENU, self.OnMenu, id=wx.ID_REDO)

    def AddFileToHistory(self, filename):
        # TODO:
        pass

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
        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_SAVE, _("&Save\tCtrl+S"), _("Saves the active document"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_SAVE)
        fileMenu.Append(wx.ID_SAVEAS, _("Save &As..."), _("Saves the active document with a new name"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_SAVEAS)
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
        editMenu.Append(wx.ID_REDO, _("&Redo\tCtrl+Y"), _("Reverses the last undo"))
        self.Bind(wx.EVT_MENU, self.ProcessEvent2, id=wx.ID_REDO)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessEvent2, id=wx.ID_REDO)
        editMenu.AppendSeparator()
        editMenu.Append(EDIT_SETTODEFAULT_ID, _("&Default...\tCtrl+D"), _("Sets to default values"))
        self.Bind(wx.EVT_MENU, self.OnDefault, id=EDIT_SETTODEFAULT_ID)
        editMenu.Append(EDIT_SETTORANDOMISE_ID, _("&Randomise...\tCtrl+R"), _("Sets to randomised values"))
        self.Bind(wx.EVT_MENU, self.OnRandomise, id=EDIT_SETTORANDOMISE_ID)
        menuBar.Append(editMenu, _("&Edit"))
        
        helpMenu = wx.Menu()
        helpMenu.Append(wx.ID_ABOUT, _("&About" + " " + wx.GetApp().GetAppName()), _("Displays program information, version number, and copyright"))
        menuBar.Append(helpMenu, _("&Help"))

        self.Bind(wx.EVT_MENU, self.OnAbout, id=wx.ID_ABOUT)
        self.Bind(wx.EVT_UPDATE_UI, self.ProcessUpdateUIEvent, id=wx.ID_ABOUT)  # Using ID_ABOUT to update the window menu, the window menu items are not triggering

        self.Bind(wx.EVT_MENU, self.OnExit, id=wx.ID_EXIT)

        return menuBar

    def ProcessEvent2(self, event):
        if self._docManager is not None:
            wx.PostEvent(self._docManager, event)


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

    def OnAbout(self, event):
        self.ShowAbout()

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
        pass



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
    _frame.SetSize(wx.DefaultCoord, wx.DefaultCoord, 500, 600)
    _view = hexeditview.HexEditView(_frame)
    _docManager.SetView(_view)


    # Run the application
    _frame.Show(True)
    _app.MainLoop() 


if __name__=="__main__":
    main()


