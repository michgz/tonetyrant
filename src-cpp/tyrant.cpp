/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.cpp
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    This sample show document/view support in wxWidgets.

    It can be run in several ways:
        * With "--mdi" command line option to use multiple MDI child frames
          for the multiple documents (this is the default).
        * With "--sdi" command line option to use multiple top level windows
          for the multiple documents
        * With "--single" command line option to support opening a single
          document only

    Notice that doing it like this somewhat complicates the code, you could
    make things much simpler in your own programs by using either
    wxDocParentFrame or wxDocMDIParentFrame unconditionally (and never using
    the single mode) instead of supporting all of them as this sample does.
 */

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/stockitem.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/spinctrl.h"

#include "tyrant.h"
#include "doc.h"
#include "view.h"
#include "midi_comms.h"

#include "wx/cmdline.h"
#include "wx/config.h"

#ifdef __WXMAC__
    #include "wx/filename.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "doc.xpm"
    #include "chart.xpm"
    #include "notepad.xpm"
#endif

#include "parameters.h"


#include "RtMidi.h"

#define wxTrue true

// ----------------------------------------------------------------------------
// MyApp implementation
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

wxBEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_MENU(wxID_ABOUT, MyApp::OnAbout)
wxEND_EVENT_TABLE()

MyApp::MyApp()
{
    m_mode = Mode_Single;

    m_canvas = NULL;
    m_menuEdit = NULL;
}


static wxWindowID SETUP_MIDI_ID = wxID_ANY;
static wxWindowID DEFAULT_ID = wxID_ANY;
static wxWindowID RANDOMISE_ID = wxID_ANY;

static wxWindowID MIDI_UPLOAD_ID = wxID_ANY;
static wxWindowID MIDI_DOWNLOAD_ID = wxID_ANY;


void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    return wxApp::OnCmdLineParsed(parser);
}

const bool AllowMidi(void) {return wxFalse;}

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

 //   ::wxInitAllImageHandlers();

    // Fill in the application information fields before creating wxConfig.
    SetVendorName("https://github.com/michgz");
    SetAppName(_("ToneTyrant"));
    SetAppDisplayName(_("Tone Tyrant for Casio"));

    //// Create a document manager
    wxDocManager *docManager = new wxDocManager;

    //// Create a template relating tone documents to their views
    new wxDocTemplate(docManager, _("Casio Tone"), "*.ton", "", "ton",
                      "Tone Doc", "Tone View",
                      CLASSINFO(ToneDocument), CLASSINFO(ToneView));


    // THIS PROGRAM ONLY ALLOWS A SINGLE FILE AT A TIME

    // If we've only got one window, we only get to edit one document at a
    // time. Therefore no text editing, just doodling.
    docManager->SetMaxDocsOpen(1);

//        // Create a template relating text documents to their views
//        new wxDocTemplate(docManager, "Text", "*.txt;*.text", "", "txt;text",
//                          "Text Doc", "Text View",
//                          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));
//#if defined( __WXMAC__ ) && wxOSX_USE_CARBON
//        wxFileName::MacRegisterDefaultTypeAndCreator("txt" , 'TEXT' , 'WXMA');
//#endif
//        // Create a template relating image documents to their views
//        new wxDocTemplate(docManager, "Image", "*.png;*.jpg", "", "png;jpg",
//                          "Image Doc", "Image View",
//                          CLASSINFO(ImageDocument), CLASSINFO(ImageView));


    // create the main frame window
    wxFrame *frame;

    frame = new wxDocParentFrame(docManager, NULL, wxID_ANY,
                                 GetAppDisplayName(),
                                 wxDefaultPosition,
                                 wxSize(620, 580));

    // and its menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(wxID_NEW);
    menuFile->Append(wxID_OPEN);

    AppendDocumentFileCommands(menuFile);

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    // A nice touch: a history of files visited. Use this menu.
    docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
    docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG


    m_canvas = new MyCanvas(NULL, frame);
    m_menuEdit = CreateDrawingEditMenu();
    docManager->CreateNewDocument();

    SETUP_MIDI_ID = wxWindow::NewControlId();
    wxMenu * const m_menuMidi = new wxMenu;
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, SETUP_MIDI_ID, _("&Setup...\tCtrl+M"), _("Sets up the MIDI communications")));
    
    m_menuMidi->AppendSeparator();

    RANDOMISE_ID = wxWindow::NewControlId();
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, RANDOMISE_ID, _("Randomise")));

    m_menuMidi->AppendSeparator();

    MIDI_UPLOAD_ID = wxWindow::NewControlId();
    MIDI_DOWNLOAD_ID = wxWindow::NewControlId();
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, MIDI_DOWNLOAD_ID, _("&Download...\tF2"), _("Downloads a tone from the keyboard")));
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, MIDI_UPLOAD_ID, _("&Upload...\tF3"), _("Uploads a tone from the keyboard")));

    m_menuMidi->Enable(MIDI_DOWNLOAD_ID, AllowMidi());
    m_menuMidi->Enable(MIDI_UPLOAD_ID, AllowMidi());

    CreateMenuBarForFrame(frame, menuFile, m_menuEdit, m_menuMidi);
    
    Bind(wxEVT_MENU, &MyApp::OnMidiSetup, this, SETUP_MIDI_ID);
    Bind(wxEVT_MENU, &MyApp::OnRandomise, this, RANDOMISE_ID);

#ifndef wxHAS_IMAGES_IN_RESOURCES
    frame->SetIcon(wxICON(doc));
#endif

    _hintsDlg = new HintsDialog(frame);
    _hintsDlg->Show(); // Non-modal

    frame->Centre();
    frame->Show();

    return true;
}

int MyApp::OnExit()
{
    wxDocManager * const manager = wxDocManager::GetDocumentManager();
#if wxUSE_CONFIG
    manager->FileHistorySave(*wxConfig::Get());
#endif // wxUSE_CONFIG
    delete manager;

    // Make sure RTMidi is linking, don't do anything more with it at this stage.
    RtMidiOut * midi = new RtMidiOut();

    (void) midi->getPortCount();


    return wxApp::OnExit();
}

// /////////////////////////////////////////////////////////////////////////////




// /////////////////////////////////////////////////////////////////////////////

void MyApp::AppendDocumentFileCommands(wxMenu *menu)
{
    menu->Append(wxID_CLOSE);
    menu->Append(wxID_SAVE);
    menu->Append(wxID_SAVEAS);
    menu->Append(wxID_REVERT, _("Re&vert..."));
}

wxMenu *MyApp::CreateDrawingEditMenu()
{
    wxMenu * const menu = new wxMenu;
    menu->Append(wxID_UNDO);
    menu->Append(wxID_REDO);
    menu->AppendSeparator();
    menu->Append(wxID_CUT, "&Cut last segment");

    return menu;
}

void MyApp::CreateMenuBarForFrame(wxFrame *frame, wxMenu *file, wxMenu *edit, wxMenu *midi)
{
    wxMenuBar *menubar = new wxMenuBar;

    menubar->Append(file, wxGetStockLabel(wxID_FILE));

    if ( edit )
        menubar->Append(edit, wxGetStockLabel(wxID_EDIT));

    if ( midi )
        menubar->Append(midi, _("&MIDI"));

    wxMenu *help= new wxMenu;
    help->Append(wxID_ABOUT);
    menubar->Append(help, wxGetStockLabel(wxID_HELP));

    frame->SetMenuBar(menubar);
}

const char  ICON_LOCATION[] = "../tyrant-64x64.ico";


void MyApp::ShowAbout(void)
{
    wxDialog *dlg = new wxDialog(GetTopWindow(), wxID_ANY, wxString(_("About ")) + GetAppName(), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, "");

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBitmap _icon = wxStaticBitmap(dlg, wxID_ANY, wxNullBitmap);
    _icon.SetIcon(wxIcon(ICON_LOCATION));
    sizer->Add(&_icon, wxALIGN_LEFT);

    sizer->Add(new wxStaticText(dlg, wxID_ANY, wxString(GetAppName()) + " v" + /*str(__version__)*/"2.0.0"), 0, wxALIGN_CENTRE|wxALL, 5);
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++"), 0, wxALIGN_CENTRE|wxALL, 5);
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "\u00A9 2022"), 0, wxALIGN_CENTRE|wxALL, 5);
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "https://github.com/michgz/ToneTyrant"), 0, wxALIGN_CENTRE|wxALL, 5);

    wxButton btn = wxButton(dlg, wxID_OK);

    sizer->Add(&btn, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(sizer);
    sizer->Fit(dlg);

    dlg->CenterOnParent();
    dlg->ShowModal();
    dlg->Destroy();
}



void MyApp::OnMidiSetup(wxCommandEvent& WXUNUSED(event))
{
    wxDialog *dlg = new MidiSetupDialog(GetTopWindow());
    dlg->ShowModal();
    
}

void MyApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    ShowAbout();
}

bool MyApp::ShowRandomise(void)
{
    bool include_wavetable_saved_value = wxFalse;
    
    bool _includeWavetable = wxFalse;

    wxDialog *dlg = new wxDialog(GetTopWindow(), wxID_ANY, _("Set to random"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, "");
    wxBoxSizer sizer = wxBoxSizer(wxVERTICAL);

    wxStaticText lbl_1 = wxStaticText(dlg, wxID_ANY, _("Set all values in the tone to random values. This will overwrite all current data."));
    sizer.Add(&lbl_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    wxCheckBox cbox_1 = wxCheckBox(dlg, wxID_ANY, _("Include wavetable"), 
            wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator,
            "IncludeWavetable");
    cbox_1.SetValue(include_wavetable_saved_value);

    sizer.Add(&cbox_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);

    wxButton btn_1 = wxButton(dlg, wxID_OK);
    sizer.Add(&btn_1, 0, wxALIGN_CENTRE|wxALL, 5);
    wxButton btn_2 = wxButton(dlg, wxID_CANCEL);
    sizer.Add(&btn_2, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(&sizer);
    sizer.Fit(dlg);
    dlg->ShowModal();

    wxCheckBox * w = reinterpret_cast<wxCheckBox*>(dlg->FindWindowByName("IncludeWavetable"));
    bool res = w->GetValue();
    include_wavetable_saved_value = res;  // TODO: make this a class global

    dlg->SetSizer(NULL, false);
    return res;
}

void MyApp::OnRandomise(wxCommandEvent& WXUNUSED(event))
{
    ShowRandomise();
}
