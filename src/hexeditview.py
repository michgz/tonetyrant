"""HexEditView class.

Implements a view class that can be used with the docview system to make a hex
editor.
"""

import wx

import keyword
import os
import sys
import time
import logging
import parameters

from dataclasses import dataclass

import docview


class CtrlVals:
    INCREASE = 1
    DECREASE = 2
    MINIMUM = 3
    MAXIMUM = 4
    

class HexEditView(wx.Control):



    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.CLIP_CHILDREN | wx.SUNKEN_BORDER):
        wx.Control.__init__(self, parent, id, pos, size, style)
        self._edit_region = -1
        self.__config()
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_CHAR, self.OnChar)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouse)
        
        self._callback_window = None


    def __config(self):
      

        self._font1 = wx.Font(9, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, False)
        self._font2 = wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False)
        
        self._font_colour1 = wx.ColourDatabase().FindColour("BLACK")
        self._font_colour2 = wx.ColourDatabase().FindColour("RED")


        
        self._brush1 = wx.Brush( wx.ColourDatabase().FindColour("DARK GREY") )
        self._brush2 = wx.Brush( wx.ColourDatabase().FindColour("LIGHT GREY") )
        self._brush3 = wx.Brush( wx.ColourDatabase().FindColour("GREY") )
        
        self._x1 = 40
        self._x2 = 450
        
        self._y1 = 5
        self._y2 = 16
        
        self._x3 = 8
        self._x4 = 9
        
        self._buffer = None
        
        self._header_offset = 32
        
      
        pass


    def SetDocument(self, buff):
      
        self._buffer = buff
        self._caret_pos = 0
        self._edit_region = 2   # 2 or 3


    def UpdateCaretPos(self, new_pos):
      
        if new_pos is None:
            return False
      
        if new_pos >= 2*len(self._buffer):
            return False
        
        if new_pos < 0:
            return False
            
        if new_pos == self._caret_pos:
            return False
            
        self._caret_pos = new_pos
        if self._callback_window is not None:
            self._callback_window.SetSelected(self._caret_pos // 2)
            self._callback_window.UpdateValues(self._buffer)
        return True


    def PosToBuff(self, x, y):
      
        if x < self._x1:
            return None
        if x >= self._x2:
            return None
        
        yy = y // self._y2
        xx = int((x - self._x1) / (self._x2 - self._x1) * 32)
        
        return 32*yy+xx

    def PosToBuffReg3(self, x, y):
      
        if x < self._x2 + 5:
            return None
        
        yy = y // self._y2
        xx = int((x - (self._x2 + 5)) / self._x4)
        
        return 32*yy+2*xx


    def OnMouse(self, event):
      
        if event.EventType == 10026:#wx.EVT_LEFT_DOWN:
            
            offs = self.PosToBuff(event.GetX(), event.GetY())
            if offs is not None:
                if self.UpdateCaretPos(offs):
                    self._edit_region = 2
                    self.Refresh()
            else:
                offs = self.PosToBuffReg3(event.GetX(), event.GetY())
                if offs is not None:
                    if self.UpdateCaretPos(offs):
                        self._edit_region = 3
                        self.Refresh()
            
        event.Skip()


    @staticmethod
    def Hex2Nibble(h):
        if h >= 0x30 and h <= 0x39:
            return h - 0x30
        if h >= 0x41 and h <= 0x46:
            return 10 + h - 0x41
        if h >= 0x61 and h <= 0x66:
            return 10 + h - 0x61
        raise Exception

    

    def OnChar(self, event):
        _redraw = False
        if event.KeyCode in [ wx.WXK_LEFT, wx.WXK_NUMPAD_LEFT ]:
            if self._edit_region == 3:
                if self.UpdateCaretPos(self._caret_pos - 2):
                    _redraw = True
            else:
                if self.UpdateCaretPos(self._caret_pos - 1):
                    _redraw = True
        elif event.KeyCode in [ wx.WXK_RIGHT, wx.WXK_NUMPAD_RIGHT ] or (event.KeyCode == wx.WXK_SPACE and self._edit_region != 3):
            if self._edit_region == 3:
                if self.UpdateCaretPos(self._caret_pos + 2):
                   _redraw = True
            else:
                if self.UpdateCaretPos(self._caret_pos + 1):
                   _redraw = True
        elif event.KeyCode in [ wx.WXK_UP, wx.WXK_NUMPAD_UP ]:
            if self.UpdateCaretPos(self._caret_pos - 32):
               _redraw = True
        elif event.KeyCode in [ wx.WXK_DOWN, wx.WXK_NUMPAD_DOWN ]:
            if self.UpdateCaretPos(self._caret_pos + 32):
               _redraw = True
        elif event.KeyCode in [ wx.WXK_PAGEUP, wx.WXK_PAGEDOWN, wx.WXK_END, wx.WXK_HOME, 
                 wx.WXK_NUMPAD_PAGEUP, wx.WXK_NUMPAD_PAGEDOWN, wx.WXK_NUMPAD_END, wx.WXK_NUMPAD_BEGIN ]:
            
            if event.KeyCode in [ wx.WXK_PAGEUP, wx.WXK_NUMPAD_PAGEUP ]:
                self._callback_window.UpDown(CtrlVals.INCREASE)
            elif event.KeyCode in [ wx.WXK_PAGEDOWN, wx.WXK_NUMPAD_PAGEDOWN ]:
                self._callback_window.UpDown(CtrlVals.DECREASE)
            elif event.KeyCode in [ wx.WXK_END, wx.WXK_NUMPAD_END ]:
                self._callback_window.UpDown(CtrlVals.MINIMUM)
            elif event.KeyCode in [ wx.WXK_HOME, wx.WXK_NUMPAD_BEGIN ]:
                self._callback_window.UpDown(CtrlVals.MAXIMUM)

        elif self._edit_region != 3 and event.KeyCode in [0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
                                                          0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
                                                          0x61, 0x62, 0x63, 0x64, 0x65, 0x66 ]:
                                 

            
            cmd = HexEditCommand.ChangeNibble(self._buffer, self._caret_pos, self.Hex2Nibble(event.KeyCode))
            self._buffer.GetCommandProcessor().Submit(cmd)
            self.UpdateCaretPos(self._caret_pos + 1)
            _redraw = True
        elif self._edit_region == 3 and event.KeyCode >= 0x20 and event.KeyCode <= 0x7E:

            cmd = HexEditCommand.ChangeByte(self._buffer, self._caret_pos, event.KeyCode)
            self._buffer.GetCommandProcessor().Submit(cmd)
            self.UpdateCaretPos(self._caret_pos + 2)
            _redraw = True

        
        else:
            event.Skip()
                
        if _redraw:
            self.Refresh()


    def SetParamTo(self, P : parameters.Param, p_val):
        self._buffer.SetParamTo(P, p_val)
        self.Refresh()
        
        
        
    def GetParamFrom(self, P : parameters.Param):
        return self._buffer.GetParamFrom(P)


    def DoRandomise(self, include_wavetable : bool):
        old_vals = self._buffer[0x20:-4]
        self._buffer.SetDocumentRandomise(include_wavetable)
        new_vals = self._buffer[0x20:-4]
        cmd = HexEditCommand.CompletelyChange(self._buffer, old_vals, new_vals)
        self._buffer.GetCommandProcessor().Submit(cmd)
        self.Refresh()

    def DoDefaults(self, include_wavetable:bool):
        old_vals = self._buffer[0x20:-4]
        self._buffer.SetDocumentDefault(include_wavetable)
        new_vals = self._buffer[0x20:-4]
        cmd = HexEditCommand.CompletelyChange(self._buffer, old_vals, new_vals)
        self._buffer.GetCommandProcessor().Submit(cmd)
        self.Refresh()


    def OnPaint(self, event):
      
        if not self._buffer:
            return
            
            
        _doRegion1 = False   # x-values 0..X1
        _doRegion2 = False   # x-values X1..X2
        _doRegion3 = False   # x-values X2..


        # We use a fairly simple re-draw conditioning. If any part of any region requires
        # redraw, we just do the whole region

        # get the update rect list
        upd = wx.RegionIterator(self.GetUpdateRegion())

        while upd.HaveRects():

            rect = upd.GetRect()

            if rect.Intersects( wx.Rect(0, 0, self._x1, self.GetSize().y) ):
                _doRegion1 = True
            if rect.Intersects( wx.Rect(self._x1, 0, self._x2 - self._x1, self.GetSize().y) ):
                _doRegion2 = True
            if rect.Intersects( wx.Rect(self._x2, 0, self.GetSize().x - self._x2, self.GetSize().y) ):
                _doRegion3 = True

            upd.Next()


        _dc = wx.PaintDC(self)
        _dc.SetPen(wx.NullPen)
        if _doRegion1:
            _dc.SetBrush(self._brush1)
            _dc.DrawRectangle(0, 0, self._x1, self.Size[1])
        if _doRegion2:
            _dc.SetBrush(self._brush2)
            _dc.DrawRectangle(self._x1, 0, self._x2, self.Size[1])
        if _doRegion3:
            _dc.SetBrush(self._brush3)
            _dc.DrawRectangle(self._x2, 0, *self.Size)
        

        if _doRegion1:
            _dc.SetFont(self._font1)

            # First frame
            i = 0
            y = self._y1
            while (i < len(self._buffer)):
                j = 16
                if i + j > len(self._buffer):
                    j = len(self._buffer) - i
                s = ""
                if i >= self._header_offset:
                    s = "{0:03X}".format(i - self._header_offset)
                    _dc.DrawText(s, 9, y)
                i += j
                y += self._y2


        if _doRegion2:
            _dc.SetFont(self._font2)
            _dc.SetTextForeground(self._font_colour1 )

            # Second frame
            i = 0
            y = self._y1
            while (i < len(self._buffer)):
                k = 0
                while i + k < len(self._buffer) and k < 16:
                    s = "{0:02X}".format(self._buffer[i + k])
                    _dc.DrawText(s, self._x1 + 5 + self._x3*(3*k + (1 if k >= 8 else 0)), y)
                    k += 1

                i += k
                y += self._y2
            
            m = self._caret_pos // 32
            n = (self._caret_pos % 32) // 2
            p = 0
            if (self._caret_pos % 2 != 0):
                p += 1
            s = " "*p + u"\u2017"
            _dc.DrawText(s, self._x1 + 5 + self._x3*(3*n + (1 if n >= 8 else 0)), self._y1 + m*self._y2)


        if _doRegion3:
            _dc.SetFont(self._font2)
            _dc.SetTextForeground(self._font_colour1 )

            # Third frame
            i = 0
            y = self._y1
            while (i < len(self._buffer)):
                k = 0
                while i + k < len(self._buffer) and k < 16:
                    c = self._buffer[i + k]
                    if c >= 0x20 and c <= 0x7E:
                        s = chr(c)
                    else:
                        s = "."
                    _dc.DrawText(s, self._x2 + 5 + self._x4*k, y)
                    k += 1

                i += k
                y += self._y2
            
            m = self._caret_pos // 32
            n = (self._caret_pos % 32) // 2
            s = u"\u2017"
            _dc.DrawText(s, self._x2 + 5 + self._x4*n, self._y1 + m*self._y2)




class HexEditCommand(docview.Command):
    def __init__(self, canUndo=True):
        docview.Command.__init__(self, canUndo=canUndo)
        self._type = -1
        

    @classmethod
    def ChangeNibble(cls, document, offset, new_nibble):
        self = cls(True)
        
        self._type = 1
        self._new_nibble = new_nibble
        self._document = document
        self._offset = offset
        
        if offset % 2 == 0:
            self._old_nibble = ((document[offset//2]) & 0xF0) >> 4
        else:
            self._old_nibble = document[offset//2] & 0x0F

        return self


    @classmethod
    def ChangeByte(cls, document, offset, new_byte):
        self = cls(True)
        
        self._type = 2
        self._new_byte = new_byte
        self._document = document
        self._offset = offset
       
        self._old_byte = document[offset//2]

        return self


    @classmethod
    def CompletelyChange(cls, document, old_vals, new_vals):
        self = cls(True)
        self._type = 3
        self._document = document
        self._old_vals = old_vals
        self._new_vals = new_vals
        
        return self


    def Do(self):
        if self._type == 1:  # Nibble
            if self._offset % 2 == 0:
                x = self._document[self._offset//2] & 0x0F
                x = x + (self._new_nibble << 4)
                self._document[self._offset//2] = x
            else:
                x = self._document[self._offset//2] & 0xF0
                x = x + (self._new_nibble << 0)
                self._document[self._offset//2] = x
            return True
        elif self._type == 2:   # Byte
            self._document[self._offset//2] = self._new_byte
            return True
        elif self._type == 3:   # Complete buffer overwrite
            self._document[0x20:-4] = self._new_vals
            return True
        return False
        
    def Undo(self):
        if self._type == 1:  # Nibble
            if self._offset % 2 == 0:
                x = self._document[self._offset//2] & 0x0F
                x = x + (self._old_nibble << 4)
                self._document[self._offset//2] = x
            else:
                x = self._document[self._offset//2] & 0xF0
                x = x + (self._old_nibble << 0)
                self._document[self._offset//2] = x
            return True
        elif self._type == 2:   # Byte
            self._document[self._offset//2] = self._old_byte
            return True
        elif self._type == 3:   # Complete buffer overwrite
            self._document[0x20:-4] = self._old_vals
            return True
        return False

