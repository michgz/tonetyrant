/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.h
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _TYRANT_DOCVIEW_H_
#define _TYRANT_DOCVIEW_H_

#include "wx/docview.h"
#include "view.h"
#include "hintview.h"

#include <map>
#include <utility>
#include <string>

class MyCanvas;
class HintsDialog;


// Define a new application
class MyApp : public wxApp
{
public:
    // the docview sample can be launched in several different ways. These don't
    // apply, but keep in case they are ever needed again.
    enum Mode
    {
#if wxUSE_MDI_ARCHITECTURE
        Mode_MDI,   // MDI mode: multiple documents, single top level window
#endif // wxUSE_MDI_ARCHITECTURE
        Mode_SDI,   // SDI mode: multiple documents, multiple top level windows
        Mode_Single // single document mode (and hence single top level window)
    };

    MyApp();

    // override some wxApp virtual methods
    virtual bool OnInit();
    virtual int OnExit();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    // our specific methods
    Mode GetMode() const { return m_mode; }

    // these accessors should only be called in single document mode, otherwise
    // the pointers are NULL and an assert is triggered
    MyCanvas *GetMainWindowCanvas() const
        { wxASSERT(m_canvas); return m_canvas; }
    wxMenu *GetMainWindowEditMenu() const
        { wxASSERT(m_menuEdit); return m_menuEdit; }

private:
    // append the standard document-oriented menu commands to this menu
    void AppendDocumentFileCommands(wxMenu *menu);

    Mode m_mode;

    // create the edit menu for drawing documents
    wxMenu *CreateDrawingEditMenu();

    // create and associate with the given frame the menu bar containing the
    // given file and edit (possibly NULL) menus as well as the standard help
    // one
    void CreateMenuBarForFrame(wxFrame *frame, wxMenu *file, wxMenu *edit, wxMenu*);


    // show the about box: as we can have different frames it's more
    // convenient, even if somewhat less usual, to handle this in the
    // application object itself
    void OnAbout(wxCommandEvent& event);

    // Show an about box
    void ShowAbout(void);

    bool ShowRandomise(void);
    void OnRandomise(wxCommandEvent& );
    void OnMidiSetup(wxCommandEvent& );


    MyCanvas *m_canvas;
    wxMenu *m_menuEdit;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MyApp);

public:

    HintsDialog *_hintsDlg;

};

DECLARE_APP(MyApp)

#endif // _WX_SAMPLES_DOCVIEW_DOCVIEW_H_
