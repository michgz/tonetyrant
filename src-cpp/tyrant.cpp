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
#include "wx/progdlg.h"

#ifdef __WXMAC__
    #include "wx/filename.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "tyrant-64x64.xpm"
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
    
    
    randomise_include_wavetable_saved_value = wxFalse;
    default_include_wavetable_saved_value = wxFalse;
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

const bool AllowMidi(void) {return wxTrue;}

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
    new wxDocTemplate(docManager, _("Casio Tone"), "*.TON", "", "TON",
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


    _hintsDlg = new HintsDialog(frame);
    _hintsDlg->Show(); // Non-modal


    m_canvas = new MyCanvas(NULL, frame);
    m_menuEdit = CreateDrawingEditMenu();
    docManager->CreateNewDocument();

    SETUP_MIDI_ID = wxWindow::NewControlId();
    wxMenu * const m_menuMidi = new wxMenu;
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, SETUP_MIDI_ID, _("&Setup...\tCtrl+M"), _("Sets up the MIDI communications")));
    
    m_menuMidi->AppendSeparator();

    RANDOMISE_ID = wxWindow::NewControlId();
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, RANDOMISE_ID, _("&Randomise...\tCtrl+R"), _("Sets to randomised values")));
    DEFAULT_ID = wxWindow::NewControlId();
    m_menuMidi->Append(new wxMenuItem(m_menuMidi, DEFAULT_ID, _("&Default...\tCtrl+D"), _("Sets to default values")));

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
    Bind(wxEVT_MENU, &MyApp::OnDefault, this, DEFAULT_ID);
    Bind(wxEVT_MENU, &MyApp::OnMidiDownload, this, MIDI_DOWNLOAD_ID);
    Bind(wxEVT_MENU, &MyApp::OnMidiUpload, this, MIDI_UPLOAD_ID);


#ifndef wxHAS_IMAGES_IN_RESOURCES
    frame->SetIcon(wxICON(tyrant));
#endif


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

    wxStaticBitmap *_icon = new wxStaticBitmap(dlg, wxID_ANY, wxNullBitmap);
#ifdef wxHAS_IMAGES_IN_RESOURCES
    // Windows: load from resources
    _icon->SetIcon(wxIcon("tyrant", wxICON_DEFAULT_TYPE, 64, 64));
#else
    // Linux: use XPM
    _icon->SetIcon(wxICON(tyrant));
#endif
    sizer->Add(_icon, wxALIGN_LEFT);

    sizer->Add(new wxStaticText(dlg, wxID_ANY, wxString(GetAppName()) + " v" + /*str(__version__)*/"2.2.1"), 0, wxALIGN_CENTRE|wxALL, 5);
#if defined(__WXGTK210__)
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++ for GTK 3.0"), 0, wxALIGN_CENTRE|wxALL, 5);
#elif defined(__WXGTK__)
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++ for GTK 2.0"), 0, wxALIGN_CENTRE|wxALL, 5);
#elif defined(__WIN64__)
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++ for Windows 64-bit"), 0, wxALIGN_CENTRE|wxALL, 5);
#elif defined(__WIN32__)
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++ for Windows 32-bit"), 0, wxALIGN_CENTRE|wxALL, 5);
#elif defined(__WXMSW__)
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++ for Windows"), 0, wxALIGN_CENTRE|wxALL, 5);
#else
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "Built in C++"), 0, wxALIGN_CENTRE|wxALL, 5);
#endif
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "\u00A9 2022"), 0, wxALIGN_CENTRE|wxALL, 5);
    sizer->Add(new wxStaticText(dlg, wxID_ANY, "https://github.com/michgz/ToneTyrant"), 0, wxALIGN_CENTRE|wxALL, 5);

    wxButton *btn = new wxButton(dlg, wxID_OK);

    sizer->Add(btn, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(sizer);
    sizer->Fit(dlg);

    dlg->CenterOnParent();
    dlg->ShowModal();
    dlg->Destroy();
}



void MyApp::OnMidiSetup(wxCommandEvent& WXUNUSED(event))
{
    wxDialog *dlg = new MidiSetupDialog(GetTopWindow());
    int resCode = dlg->ShowModal();
    
    if (resCode == wxID_OK)
    {
        static_cast<MidiSetupDialog *>(dlg)->DoOk();
    }
    
    dlg->Destroy();
    
    
}

void MyApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    ShowAbout();
}

bool MyApp::ShowRandomise(void)
{

    wxDialog *dlg = new wxDialog(GetTopWindow(), wxID_ANY, _("Set to random"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, "");
    wxBoxSizer sizer = wxBoxSizer(wxVERTICAL);

    wxStaticText *lbl_1 = new wxStaticText(dlg, wxID_ANY, _("Set all values in the tone to random values. This will overwrite all current data."));
    sizer.Add(lbl_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    wxCheckBox *cbox_1 = new wxCheckBox(dlg, wxID_ANY, _("Include wavetable"), 
            wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator,
            "IncludeWavetable");
    cbox_1->SetValue(randomise_include_wavetable_saved_value);

    sizer.Add(cbox_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);

    wxButton *btn_1 = new wxButton(dlg, wxID_OK);
    sizer.Add(btn_1, 0, wxALIGN_CENTRE|wxALL, 5);
    wxButton *btn_2 = new wxButton(dlg, wxID_CANCEL);
    sizer.Add(btn_2, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(&sizer);
    sizer.Fit(dlg);
    int retCode = dlg->ShowModal();

    wxCheckBox * w = reinterpret_cast<wxCheckBox*>(dlg->FindWindowByName("IncludeWavetable"));
    bool res = w->GetValue();
    randomise_include_wavetable_saved_value = res;  // TODO: make this a class global

    dlg->SetSizer(NULL, false);
    dlg->Destroy();
    res = (retCode == wxID_OK);
    return res;
}

void MyApp::OnRandomise(wxCommandEvent& WXUNUSED(event))
{
    if (ShowRandomise())
    {
        // TODO: Unsafe!!!!! Need to test every pointer in this sequence!!
        static_cast<ToneDocument *>(this->m_canvas->m_view->GetDocument())->OnSetToRandomise(randomise_include_wavetable_saved_value);
    }
}

bool MyApp::ShowDefault(void)
{

    wxDialog *dlg = new wxDialog(GetTopWindow(), wxID_ANY, _("Set to default"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, "");
    wxBoxSizer sizer = wxBoxSizer(wxVERTICAL);

    wxStaticText *lbl_1 = new wxStaticText(dlg, wxID_ANY, _("Set all values in the tone to default values. This will overwrite all current data."));
    sizer.Add(lbl_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    wxCheckBox *cbox_1 = new wxCheckBox(dlg, wxID_ANY, _("Include wavetable"), 
            wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator,
            "IncludeWavetable");
    cbox_1->SetValue(default_include_wavetable_saved_value);

    sizer.Add(cbox_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);

    wxButton *btn_1 = new wxButton(dlg, wxID_OK);
    sizer.Add(btn_1, 0, wxALIGN_CENTRE|wxALL, 5);
    wxButton *btn_2 = new wxButton(dlg, wxID_CANCEL);
    sizer.Add(btn_2, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(&sizer);
    sizer.Fit(dlg);
    int retCode = dlg->ShowModal();

    wxCheckBox * w = reinterpret_cast<wxCheckBox*>(dlg->FindWindowByName("IncludeWavetable"));
    bool res = w->GetValue();
    default_include_wavetable_saved_value = res;  // TODO: make this a class global

    dlg->SetSizer(NULL, false);
    dlg->Destroy();
    res = (retCode == wxID_OK);
    return res;
}

void MyApp::OnDefault(wxCommandEvent& WXUNUSED(event))
{
    if (ShowDefault())
    {
        // TODO: Unsafe!!!!! Need to test every pointer in this sequence!!
        static_cast<ToneDocument *>(this->m_canvas->m_view->GetDocument())->OnSetToDefault(default_include_wavetable_saved_value);
    }
}


void MyApp::OnMidiDownload(wxCommandEvent& WXUNUSED(event))
{
    /*
    Show the MidiDowloadDialog, and perform any operations which were requested.
    */


    int target_saved_value = 801;

    wxDialog *dlg = new wxDialog(GetTopWindow(), wxID_ANY, _("MIDI Dowload"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, "");
    wxBoxSizer sizer = wxBoxSizer(wxVERTICAL);

    wxStaticText *lbl_1 = new wxStaticText(dlg, wxID_ANY, _("Read from User Tone:"), wxDefaultPosition, wxSize(140, 22));
    sizer.Add(lbl_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    
    wxString s_1 = wxString::Format("%d", target_saved_value);
    wxSpinCtrl *spn_1 = new wxSpinCtrl(dlg, wxID_ANY, s_1, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 801, 900, target_saved_value, "TargetValue");
    sizer.Add(spn_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    
    
    wxButton *btn_1 = new wxButton(dlg, wxID_OK);
    sizer.Add(btn_1, 0, wxALIGN_CENTRE|wxALL, 5);
    wxButton *btn_2 = new wxButton(dlg, wxID_CANCEL);
    sizer.Add(btn_2, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(&sizer);
    sizer.Fit(dlg);
    dlg->CenterOnParent();
    int retCode = dlg->ShowModal();

    wxSpinCtrl * w = reinterpret_cast<wxSpinCtrl*>(dlg->FindWindowByName("TargetValue"));
    int res = w->GetValue();
    target_saved_value = res;  // TODO: make this a class global

    dlg->SetSizer(NULL, false);
    dlg->Destroy();
    if (retCode == wxID_OK)
    {
        
        if (target_saved_value >= 801 && target_saved_value <= 900)
        {
        
            wxProgressDialog *dlg_2 = new wxProgressDialog (_("Downloading..."), "Progress:", 100, GetTopWindow());
            dlg_2->Show();
            
            
            // TODO: Unsafe!!!!! Need to test every pointer in this sequence!!
            ToneDocument * src_doc = static_cast<ToneDocument *>(this->m_canvas->m_view->GetDocument());
            
            std::vector<unsigned char> dest_doc = download_ac7_internal(target_saved_value - 801, 1, 3, true);
            
            if (dest_doc.size() == 0x1C8)
            {
                
                HexEditCommand * cmd = HexEditCommand::CompletelyChange(src_doc, src_doc->GetSubsetData(), dest_doc);
                src_doc->GetCommandProcessor()->Submit(cmd);
                this->m_canvas->Refresh();
                
                
            }
            
            

            // Now a "fake" progress indication
            //     ** For now, the progress is commented out. It causes double free
            //        error in Linux
            wxMilliSleep(300);
            //dlg_2->Update(44);
            wxMilliSleep(300);
            //dlg_2->Update(96);
            wxMilliSleep(300);
           
            
            
            dlg_2->Destroy();
            
            
            // We have (probably) updated the document.
            src_doc->DoUpdate();
            
            
        }
            
         

    }

}

void MyApp::OnMidiUpload(wxCommandEvent& WXUNUSED(event))
{
        /*
        Show the MidiUploadDialog, and perform any operations which were requested.
        */


    int target_saved_value = 801;

    wxDialog *dlg = new wxDialog(GetTopWindow(), wxID_ANY, _("MIDI Upload"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, "");
    wxBoxSizer sizer = wxBoxSizer(wxVERTICAL);

    wxStaticText *lbl_1 = new wxStaticText(dlg, wxID_ANY, _("Write to User Tone:"), wxDefaultPosition, wxSize(140, 22));
    sizer.Add(lbl_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    
    wxString s_1 = wxString::Format("%d", target_saved_value);
    wxSpinCtrl *spn_1 = new wxSpinCtrl(dlg, wxID_ANY, s_1, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 801, 900, target_saved_value, "TargetValue");
    sizer.Add(spn_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);
    
    
    wxButton *btn_1 = new wxButton(dlg, wxID_OK);
    sizer.Add(btn_1, 0, wxALIGN_CENTRE|wxALL, 5);
    wxButton *btn_2 = new wxButton(dlg, wxID_CANCEL);
    sizer.Add(btn_2, 0, wxALIGN_CENTRE|wxALL, 5);

    dlg->SetSizer(&sizer);
    sizer.Fit(dlg);
    dlg->CenterOnParent();
    int retCode = dlg->ShowModal();

    wxSpinCtrl * w = reinterpret_cast<wxSpinCtrl*>(dlg->FindWindowByName("TargetValue"));
    int res = w->GetValue();
    target_saved_value = res;  // TODO: make this a class global

    dlg->SetSizer(NULL, false);
    dlg->Destroy();
    if (retCode == wxID_OK)
    {
        
        if (target_saved_value >= 801 && target_saved_value <= 900)
        {
        
            wxProgressDialog *dlg_2 = new wxProgressDialog (_("Uploading..."), "Progress:", 100, GetTopWindow());
            dlg_2->Show();
            
            
            // TODO: Unsafe!!!!! Need to test every pointer in this sequence!!
            std::vector<unsigned char> * src_doc = static_cast<std::vector<unsigned char> *>(static_cast<ToneDocument *>(this->m_canvas->m_view->GetDocument()));
            
            // Copy a subset. Should be able to do this better!
            std::vector<unsigned char> dest_doc = std::vector<unsigned char>();
            int j;
            for (j = 0x20; j < 0x1E8; j ++)
            {
                dest_doc.push_back(src_doc->at(j));
            }
            
            upload_ac7_internal(dest_doc, target_saved_value - 801, 1, 3, true);
            

            // Now a "fake" progress indication
            //     ** For now, the progress is commented out. It causes double free
            //        error in Linux
            wxMilliSleep(300);
            //dlg_2->Update(44);
            wxMilliSleep(300);
            //dlg_2->Update(96);
            wxMilliSleep(300);
           
            
            
            dlg_2->Destroy();
        }
            
         

    }

}
