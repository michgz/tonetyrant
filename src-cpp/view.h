#ifndef _TONETYRANT_VIEW_H_
#define _TONETYRANT_VIEW_H_

#include "wx/docview.h"
#include "doc.h"
#include "tyrant.h"

#include <list>
#include <vector>

class HintsDialog;

// ----------------------------------------------------------------------------
// Drawing view classes
// ----------------------------------------------------------------------------

// The window showing the drawing itself
class MyCanvas : public wxWindow
{
public:
    // view may be NULL if we're not associated with one yet, but parent must
    // be a valid pointer
    MyCanvas(wxView *view, wxWindow *parent = NULL);
    virtual ~MyCanvas();

    virtual void OnDraw(wxDC& dc);
    virtual void OnPaint(wxPaintEvent&);
    virtual void OnChar(wxKeyEvent& event);

    // in a normal multiple document application a canvas is associated with
    // one view from the beginning until the end, but to support the single
    // document mode in which all documents reuse the same MyApp::GetCanvas()
    // we need to allow switching the canvas from one view to another one

    void SetView(wxView *view)
    {
        wxASSERT_MSG( !m_view, "shouldn't be already associated with a view" );

        m_view = view;
    }

    void ResetView()
    {
        wxASSERT_MSG( m_view, "should be associated with a view" );

        m_view = NULL;
    }

private:
    void OnMouseEvent(wxMouseEvent& event);

    wxView *m_view;

    // the segment being currently drawn or NULL if none
    //DoodleSegment *m_currentSegment;

    // the last mouse press position
    wxPoint m_lastMousePos;

    wxDECLARE_EVENT_TABLE();
};





// The view using MyCanvas to show its contents
class ToneView : public wxView
{
public:
    ToneView() : wxView(), m_canvas(NULL) {}

    virtual bool OnCreate(wxDocument *doc, long flags);
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL);
    virtual bool OnClose(bool deleteWindow = true);

    ToneDocument* GetDocument();

    void OnChar(wxKeyEvent& event );
    void OnMouse(wxMouseEvent& event);

    HintsDialog* _callback_window;

    static unsigned char Hex2Nibble(unsigned char h);
    
    void Update(void){Refresh();}

private:
    void OnCut(wxCommandEvent& event);


int PosToBuff(int x, int y);
int PosToBuffReg3(int x, int y);
void Refresh(void);
void __config(void);


    std::list<unsigned int> _highlighted_offsets;
    wxFont _font1, _font2;
    wxColour _font_colour1, _font_colour2;
    wxBrush _brush1, _brush2, _brush3, _brush4;
    unsigned int _x1, _x2, _y1, _y2, _x3, _x4, _header_offset;
    signed int _caret_pos;
    unsigned int _edit_region;
    
    bool UpdateCaretPos(signed int x);

    MyCanvas *m_canvas;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(ToneView);


};




// ----------------------------------------------------------------------------
// HintsView
// ----------------------------------------------------------------------------

class HintsView : public wxView
{
public:
 //   HintsView() {}
    HintsView(ToneDocument *doc);

    virtual void OnDraw(wxDC *dc);
    virtual bool OnClose(bool deleteWindow);


private:
    wxFrame *m_frame;

    wxDECLARE_NO_COPY_CLASS(HintsView);
};




#endif // _TONETYRANT_VIEW_H_
