/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/view.cpp
// Purpose:     View classes implementation
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "doc.h"
#include "view.h"

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(ToneView, wxView)

wxBEGIN_EVENT_TABLE(ToneView, wxView)
    EVT_MENU(wxID_CUT, ToneView::OnCut)
    EVT_CHAR( ToneView::OnChar)
    EVT_LEFT_DOWN( ToneView::OnMouse)
wxEND_EVENT_TABLE()



#define wxTrue true



// What to do when a view is created. Creates actual
// windows for displaying the view.
bool ToneView::OnCreate(wxDocument *doc, long flags)
{
    if ( !wxView::OnCreate(doc, flags) )
        return false;

    MyApp& app = wxGetApp();
    if ( app.GetMode() != MyApp::Mode_Single )
    {
        // create a new window and canvas inside it
        //wxFrame* frame = app.CreateChildFrame(this, true);
        //wxASSERT(frame == GetFrame());
        //m_canvas = new MyCanvas(this);
        //frame->Show();
    }
    else // single document mode
    {
        // reuse the existing window and canvas
        m_canvas = app.GetMainWindowCanvas();
        m_canvas->SetView(this);

        // Initialize the edit menu Undo and Redo items
        doc->GetCommandProcessor()->SetEditMenu(app.GetMainWindowEditMenu());
        doc->GetCommandProcessor()->Initialize();


        _callback_window = app._hintsDlg;
        if (_callback_window != NULL)
        {
            _callback_window->SetView(this);
        }

    }

    __config();

    return true;
}

void ToneView::__config(void)
{
  
  _caret_pos = 0xAA;

    _font1 = wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFalse);
    _font2 = wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, wxFalse);

    _font_colour1 = wxColourDatabase().Find("BLACK");
    _font_colour2 = wxColourDatabase().Find("RED");


    
    _brush1 = wxBrush( wxColourDatabase().Find("DARK GREY") );
    _brush2 = wxBrush( wxColourDatabase().Find("LIGHT GREY") );
    _brush3 = wxBrush( wxColourDatabase().Find("GREY") );

    _brush4 = wxBrush( wxColour(220, 205, 205) );   // A pale pink.


    
    _x1 = 40;
    _x2 = 450;
    
    _y1 = 5;
    _y2 = 16;
    
    _x3 = 8;
    _x4 = 9;
    
//     self._buffer = None
    
    _header_offset = 32;
    
  
}



unsigned char ToneView::Hex2Nibble(unsigned char h)
{
    if (h >= 0x30 && h <= 0x39)
        return h - 0x30;
    if (h >= 0x41 && h <= 0x46)
        return 10 + h - 0x41;
    if (h >= 0x61 && h <= 0x66)
        return 10 + h - 0x61;
    //    raise Exception

};


void ToneView::OnDraw(wxDC *_dc)
{
    ToneDocument * _buffer = GetDocument();

    if (!_buffer)
        return;
        
        
    bool _doRegion1 = wxFalse;   // x-values 0..X1
    bool _doRegion2 = wxFalse;   // x-values X1..X2
    bool _doRegion3 = wxFalse;   // x-values X2..


    // We use a fairly simple re-draw conditioning. If any part of any region requires
    // redraw, we just do the whole region

    // get the update rect list

/*
    wxRegionIterator *upd = new wxRegionIterator(GetFrame()->GetUpdateRegion());

    while (upd)
    {

        wxRect rect = upd->GetRect();

        //if (rect.Intersects( wxRect(0, 0, _x1, GetFrame()->GetSize().y) ))
         //   _doRegion1 = wxTrue;
        //if (rect.Intersects( wxRect(_x1, 0, _x2 - _x1, GetFrame()->GetSize().y) ))
        //    _doRegion2 = wxTrue;
        //if (rect.Intersects( wxRect(_x2, 0, GetFrame()->GetSize().x - _x2, GetFrame()->GetSize().y) ))
        //    _doRegion3 = wxTrue;

        upd ++;
    }
*/

    wxRegion rgn = m_canvas->GetUpdateRegion();
    {

        wxRect rect = rgn.GetBox();

        if (rect.Intersects( wxRect(0, 0, _x1, m_canvas->GetSize().y) ))
            _doRegion1 = wxTrue;
        if (rect.Intersects( wxRect(_x1, 0, _x2 - _x1, m_canvas->GetSize().y) ))
            _doRegion2 = wxTrue;
        if (rect.Intersects( wxRect(_x2, 0, m_canvas->GetSize().x - _x2, m_canvas->GetSize().y) ))
            _doRegion3 = wxTrue;

        //upd ++;
    }




  //  wxPaintDC *_dc = new wxPaintDC(this);
    _dc->SetPen(wxNullPen);
    if (_doRegion1)
    {
        _dc->SetBrush(_brush1);
        _dc->DrawRectangle(0, 0, _x1, m_canvas->GetSize().y);
    }
    if (_doRegion2)
    {
        _dc->SetBrush(_brush2);
        _dc->DrawRectangle(_x1, 0, _x2, m_canvas->GetSize().y);
    }
    if (_doRegion3)
    {
        _dc->SetBrush(_brush3);
        _dc->DrawRectangle(_x2, 0, m_canvas->GetSize().x, m_canvas->GetSize().y);
    }
    
    int i;

    if (_doRegion1)
    {
        _dc->SetFont(_font1);

        // First frame
        i = 0;
        int y = _y1;
        while (i < _buffer->size())
        {
            int j = 16;
            if (i + j > _buffer->size())
            {
                j = _buffer->size() - i;
            }
            if (i >= _header_offset)
            {
                wxString s = wxString::Format("%03X", i - _header_offset);
                _dc->DrawText(s, 9, y);
            }
            i += j;
            y += _y2;
        }


    if (_doRegion2)
    {
        _dc->SetFont(_font2);
        _dc->SetTextForeground(_font_colour1 );

        // Second frame
        
        i = 0;
        int y = _y1;
        _dc->SetBrush(_brush4);
        while (i < _buffer->size())
        {
            int k = 0;
            while (i + k < _buffer->size() && k < 16)
            {
                if (false)//(i + k) in self._highlighted_offsets
                {
                    int q = 0;
                    if (k >= 8)
                        q = 1;
                    _dc->DrawRectangle(_x1 + 0 + _x3*(3*k + (q)), y-1, _x3*2+10  , _y2 + 2);
                }
                k += 1;
            }

            i += k;
            y += _y2;
        }
        
        
    }
     //   list_ = _buffer.GetChangeList()
      std::list<int> list_;
        
        i = 0;
        y = _y1;
        while (i < _buffer->size())
        {
            int k = 0;
            while (i + k < _buffer->size() && k < 16)
            {
                wxString ss = wxString::Format("%02X", _buffer->at(i + k));
                
               
                if (false)//(i + k) in list_:
                    _dc->SetTextForeground(_font_colour2 );
                else
                    _dc->SetTextForeground(_font_colour1 );
                
                int q = 0;
                if (k >= 8)
                    q = 1;
                _dc->DrawText(ss, _x1 + 5 + _x3*(3*k + (q)), y);
                k += 1;
            }

            i += k;
            y += _y2;
        }
        
        int m = (int)_caret_pos / 32;
        int n = (int)(_caret_pos & 0x001F) / 2;
        int p = 0;
        if ((_caret_pos & 0x0001) != 0)
            p += 1;
        wxString s("");
        while (p > 0)
        {
            s += " ";
            p --;
        }

        s += wxString::FromUTF8("\xE2\x80\x97");
                 // in UTF8 the character U+2017 is encoded as 0xE2 0x80 0x97


        if (false)//(_caret_pos / 2) in list_:
            _dc->SetTextForeground(_font_colour2 );
        else
            _dc->SetTextForeground(_font_colour1 );
        
        int q = 0;
        if (n >= 8)
            q = 1;
        _dc->DrawText(s, _x1 + 5 + _x3*(3*n + (q)), _y1 + m*_y2);
    }

    if (_doRegion3)
    {
        _dc->SetFont(_font2);
        _dc->SetTextForeground(_font_colour1 );

        // Third frame
        int i = 0;
        int y = _y1;
        while (i < _buffer->size())
        {
            int k = 0;
            while (i + k < _buffer->size() && k < 16)
            {
                unsigned char c = _buffer->at(i + k);
                std::string s;
                if (c >= 0x20 && c <= 0x7E)
                    s = char(c);
                else
                    s = ".";
                wxString ss(s);
                _dc->DrawText(ss, _x2 + 5 + _x4*k, y);
                k += 1;
            }

            i += k;
            y += _y2;
        }
        
        int m = (int)_caret_pos / 32;
        int n = ((int)_caret_pos & 0x001F) / 2;
        wxString s = wxString::FromUTF8("\xE2\x80\x97");
                 // in UTF8 the character U+2017 is encoded as 0xE2 0x80 0x97
        _dc->DrawText(s, _x2 + 5 + _x4*n, _y1 + m*_y2);
    }

}



ToneDocument* ToneView::GetDocument()
{
    return wxStaticCast(wxView::GetDocument(), ToneDocument);
}

void ToneView::Refresh(void)
{
    if ( m_canvas )
        m_canvas->Refresh();
}

void ToneView::OnUpdate(wxView* sender, wxObject* hint)
{
    wxView::OnUpdate(sender, hint);
    if ( m_canvas )
        m_canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool ToneView::OnClose(bool deleteWindow)
{
    if ( !wxView::OnClose(deleteWindow) )
        return false;

    Activate(false);

    // Clear the canvas in single-window mode in which it stays alive
    if ( wxGetApp().GetMode() == MyApp::Mode_Single )
    {
        
                GetDocument()->DeleteContents();
        
        m_canvas->ClearBackground();
        m_canvas->ResetView();
        m_canvas = NULL;
        
        if (_callback_window != NULL)
        {
            _callback_window->ResetView();
        }

        if (GetFrame())
            wxStaticCast(GetFrame(), wxFrame)->SetTitle(wxTheApp->GetAppDisplayName());
    }
    else // not single window mode
    {
        if ( deleteWindow )
        {
            GetFrame()->Destroy();
            SetFrame(NULL);
        }
    }
    return true;
}

void ToneView::OnCut(wxCommandEvent& WXUNUSED(event) )
{
    ToneDocument * const doc = GetDocument();

    //doc->GetCommandProcessor()->Submit(new DrawingRemoveSegmentCommand(doc));
}


void ToneView::OnChar(wxKeyEvent& event )
{

    bool _redraw = wxFalse;
    
    int key_ = event.GetKeyCode();



    if ( key_ ==  WXK_LEFT || key_ ==  WXK_NUMPAD_LEFT)
    {
        if (_edit_region == 3)
        {
            if (UpdateCaretPos(_caret_pos - 2))
            {
                _redraw = wxTrue;
            }
        }
        else
        {
            if (UpdateCaretPos(_caret_pos - 1))
            {
                _redraw = wxTrue;
            }
        }
    }
    else if (key_ == WXK_RIGHT || key_ ==  WXK_NUMPAD_RIGHT || (key_ == WXK_SPACE && _edit_region != 3))
    {
        if (_edit_region == 3)
        {
            if (UpdateCaretPos(_caret_pos + 2))
            {
               _redraw = wxTrue;
            }
        }
        else
        {
            if (UpdateCaretPos(_caret_pos + 1))
            {
                _redraw = wxTrue;
            }
        }
    }
    else if (key_ ==  WXK_UP || key_ ==  WXK_NUMPAD_UP)
    {
        if (UpdateCaretPos(_caret_pos - 32))
        {
           _redraw = wxTrue;
        }
    }
    else if (key_ ==  WXK_DOWN || key_ ==  WXK_NUMPAD_DOWN)
    {
        if (UpdateCaretPos(_caret_pos + 32))
        {
           _redraw = wxTrue;
        }
    }
    // For Up/Down controls, call the callback window directly. It should be
    // possible to do this with WX's event handling, I just haven't seen how.
    else if (key_ ==  WXK_PAGEUP || key_ ==  WXK_NUMPAD_PAGEUP)
    {
        _callback_window->UpDown(CtrlVals::INCREASE);
    }
    else if (key_ ==  WXK_PAGEDOWN || key_ ==  WXK_NUMPAD_PAGEDOWN)
    {
        _callback_window->UpDown(CtrlVals::DECREASE);
    }
    else if (key_ == WXK_END)  // numpad??
    {
        _callback_window->UpDown(CtrlVals::MINIMUM);
    }
    else if (key_ ==  WXK_HOME || key_ ==  WXK_NUMPAD_BEGIN)
    {
        _callback_window->UpDown(CtrlVals::MAXIMUM);
    }

    else if ( ((   key_ >= '0'  && key_ <= '9') || (   key_ >= 'a'  && key_ <= 'f')||(   key_ >= 'A'  && key_ <= 'F'))  && _edit_region != 3)
    {
        HexEditCommand * cmd = HexEditCommand::ChangeNibble(GetDocument(), _caret_pos, Hex2Nibble(event.GetKeyCode()));
        GetDocument()->GetCommandProcessor()->Submit(cmd);
      //  _callback_window->PositionChanged();
        UpdateCaretPos(_caret_pos + 1);
        _redraw = wxTrue;
        
        //event.Skip(true);
    }

    else if ( (   key_ >= 0x20  && key_ <= 0x7E   )   && _edit_region == 3)
    {
        HexEditCommand * cmd = HexEditCommand::ChangeByte(GetDocument(), _caret_pos, key_);
        GetDocument()->GetCommandProcessor()->Submit(cmd);
      //  _callback_window->PositionChanged();
        UpdateCaretPos(_caret_pos + 2);
        _redraw = wxTrue;
    
    }

    else
    {
        event.Skip(true);
    }


    if (_redraw)
    {
        m_canvas->Refresh();
    }

}



int ToneView::PosToBuff(int x, int y)
{
      
        if (x < _x1)
            return -1;
        if (x >= _x2)
            return -1;
        
        int yy = y / _y2;
        int xx = int((x - _x1) * 32 / (_x2 - _x1));
        
        return 32*yy+xx;
}

int ToneView::PosToBuffReg3(int x, int y)
{
        if (x < _x2 + 5)
            return -1;
        
        int yy = y / _y2;
        int xx = int((x - (_x2 + 5)) / _x4);
        
        return 32*yy+2*xx;
}

void ToneView::OnMouse(wxMouseEvent& event)
{
    if (event.LeftDown())
    {

        int offs = PosToBuff(event.GetX(), event.GetY());
        if (offs >= 0)
        {
            if (UpdateCaretPos(offs))
            {
                _edit_region = 2;
                Refresh();
            }
        }
        else
        {
            offs = PosToBuffReg3(event.GetX(), event.GetY());
            if (offs >= 0)
            {
                if (UpdateCaretPos(offs))
                {
                    _edit_region = 3;
                    Refresh();
                }
            }
        }
    }

    event.Skip(true);

}


bool ToneView::UpdateCaretPos(signed int x) {
    if (x < 0)
        return false;
    else if (x > 8*0x1C8)
        return false;
        
    _caret_pos = x;
    
    if (_callback_window != NULL)
    {
        _callback_window->SetSelected(_caret_pos / 2);
        _callback_window->UpdateValues(GetDocument());
        //self._highlighted_offsets = self._callback_window.GetHighlightList()
    }
    
    
    return true;
}





// ----------------------------------------------------------------------------
// MyCanvas implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyCanvas, wxWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_CHAR(MyCanvas::OnChar)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxView *view, wxWindow *parent)
    : wxWindow(parent ? parent : view->GetFrame(), wxID_ANY)
{
    m_view = view;

  //  m_currentSegment = NULL;
    m_lastMousePos = wxDefaultPosition;

    //SetCursor(wxCursor(wxCURSOR_PENCIL));

    // this is completely arbitrary and is done just for illustration purposes
    SetVirtualSize(1000, 1000);

    SetBackgroundColour(*wxWHITE);
}

MyCanvas::~MyCanvas()
{
   // delete m_currentSegment;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    if ( m_view )
    {
        m_view->OnDraw(& dc);
    }
}

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    OnDraw(dc);
}

void MyCanvas::OnChar(wxKeyEvent& event)
{
    if ( m_view )
    {
        m_view->ProcessEvent(event);
        
    }
    
    event.Skip(true);
}

// This implements a tiny doodling program. Drag the mouse using the left
// button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    if ( !m_view )
        return;

   m_view->ProcessEvent(event);
   event.Skip(true);

/*
    // is this the end of the current segment?
    if ( m_currentSegment && event.LeftUp() )
    {
        if ( !m_currentSegment->IsEmpty() )
        {
            // We've got a valid segment on mouse left up, so store it.
            DrawingDocument * const
                doc = wxStaticCast(m_view->GetDocument(), DrawingDocument);

            doc->GetCommandProcessor()->Submit(
                new DrawingAddSegmentCommand(doc, *m_currentSegment));

            doc->Modify(true);
        }

        wxDELETE(m_currentSegment);
    }

    // is this the start of a new segment?
    if ( m_lastMousePos != wxDefaultPosition && event.Dragging() )
    {
        if ( !m_currentSegment )
            m_currentSegment = new DoodleSegment;

        m_currentSegment->AddLine(m_lastMousePos, pt);

        dc.DrawLine(m_lastMousePos, pt);
    }
*/

}


// ----------------------------------------------------------------------------
// HintsView implementation
// ----------------------------------------------------------------------------


HintsView::HintsView(ToneDocument *doc)
                : wxView()
{
    SetDocument(doc);

    //m_frame = wxGetApp().CreateChildFrame(this, false);
    //m_frame->SetTitle("Image Details");

#if 0
    wxPanel * const panel = new wxPanel(m_frame);
    wxFlexGridSizer * const sizer = new wxFlexGridSizer(2, wxSize(5, 5));
    const wxSizerFlags
        flags = wxSizerFlags().Align(wxALIGN_CENTRE_VERTICAL).Border();

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Image &file:"), flags);
    sizer->Add(new wxStaticText(panel, wxID_ANY, doc->GetFilename()), flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Image &type:"), flags);
    wxString typeStr;
    /*switch ( doc->GetType() )
    {
        case wxBITMAP_TYPE_PNG:
            typeStr = "PNG";
            break;

        case wxBITMAP_TYPE_JPEG:
            typeStr = "JPEG";
            break;

        default:
            typeStr = "Unknown";
    }*/
    
    typeStr = "WHAT";
    
    sizer->Add(new wxStaticText(panel, wxID_ANY, typeStr), flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Image &size:"), flags);
    //wxSize size = doc->GetSize();
    //sizer->Add(new wxStaticText(panel, wxID_ANY,
    //                            wxString::Format("%d*%d", size.x, size.y)),
    //           flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Number of unique &colours:"),
               flags);
    //sizer->Add(new wxStaticText(panel, wxID_ANY,
    //                            wxString::Format("%lu", doc->GetNumColours())),
    //           flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Uses &alpha:"), flags);
    //sizer->Add(new wxStaticText(panel, wxID_ANY,
    //                            doc->HasAlpha() ? "Yes" : "No"), flags);

    panel->SetSizer(sizer);
    m_frame->SetClientSize(panel->GetBestSize());
    m_frame->Show(true);
#endif
}

void HintsView::OnDraw(wxDC * WXUNUSED(dc))
{
    // nothing to do here, we use controls to show our information
}

bool HintsView::OnClose(bool deleteWindow)
{
    if ( wxGetApp().GetMode() != MyApp::Mode_Single && deleteWindow )
    {
        delete m_frame;
        m_frame = NULL;
    }

    return true;
}




