#include "wx/wxprec.h"

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/spinctrl.h"
#include <wx/artprov.h>

#include "hintview.h"
#include "tyrant.h"
#include "doc.h"
#include "view.h"




#include "parameters.h"



#define wxTrue true





//IMPLEMENT_DYNAMIC_CLASS(HintsPanelGeneric, wxPanel)


int PVtype(PP_ID PP)
{

    int type_ = 0;
    
    if (Parameters[PP].bitCount == 1)
        type_ = 1;
    
    if (Parameters[PP].number == 117)
        type_ = 2;
    else if (Parameters[PP].number == 1)
        type_ = 3;
    else if (Parameters[PP].number == 21)
        type_ = 3;
    else if (Parameters[PP].number == 0)
        type_ = 4;
    else if (Parameters[PP].number == 85)
        type_ = 5;
    else if (Parameters[PP].number == 59 || Parameters[PP].number == 66)
        type_ = 6;
    else if (Parameters[PP].number == 116)
        type_ = 7;
    else if (Parameters[PP].number == 43  || Parameters[PP].number == 108)
        type_ = 8;
    else if (Parameters[PP].number == 84)
        type_ = 9;
    else if (Parameters[PP].number == 87)
        type_ = 10;
    else if (Parameters[PP].number == 42)
        type_ = 11;

    return type_;
}

int HintsDialog::SuggestStep(PP_ID PP)
{
    int type_ = PVtype(PP);
    
    
    if (type_ == 1)
        return 1;
    else if (type_ == 4 || type_ == 9 || type_ == 10)
        return -1;  // Non-numeric
    else
    {
        if (Parameters[PP].recommendedStep >= 1)
            return Parameters[PP].recommendedStep;
        return 10;
    }
}

bool ParamInOffsets(int offset_, PP_ID PP)
{
    if ((offset_ >= Parameters[PP].byteOffset) && (offset_ < Parameters[PP].byteOffset + Parameters[PP].byteCount))
    {
        return wxTrue;
    }
    return wxFalse;
}



void HintsDialog::UpDown(CtrlVals ctrl_val)
{
    /*
    Process a "increase" or "decrease" key stroke.
    */
    if (_current_offset >= 0 && _current_cluster >= 0 && _panel != NULL)
    {
        for (auto iter = _panel->PARAMS.begin(); iter != _panel->PARAMS.end(); iter ++)
        {
            PP_ID PP = *iter;
            if (ParamInOffsets(_current_offset, PP))
            {

                int CURR = 0;
                if (_view != NULL)
                {
                    if (_view->GetDocument() != NULL)
                    {
                        CURR = static_cast<ToneDocument *>(_view->GetDocument())->GetParamFrom(PP);
                    }
                }
                int STEP = SuggestStep(PP);
                    
                if (STEP <= 0)
                {
                    return;   // No action possible
                }
                    
                int X = -999;
                    
                if (ctrl_val == CtrlVals::MINIMUM)
                {
                    X = Parameters[PP].recommendedLimits[0];
                }
                else if (ctrl_val == CtrlVals::MAXIMUM)
                {
                    X = Parameters[PP].recommendedLimits[1];
                }
                else if (ctrl_val == CtrlVals::INCREASE)
                {
                    X = CURR + STEP;
                    if (X > Parameters[PP].recommendedLimits[1])
                    {
                        X = Parameters[PP].recommendedLimits[1];
                    }
                }
                else if (ctrl_val == CtrlVals::DECREASE)
                {
                    X = CURR - STEP;
                    if (X < Parameters[PP].recommendedLimits[0])
                    {
                        X = Parameters[PP].recommendedLimits[0];
                    }
                }
                if ((X != -999) && (X != CURR))
                {
                    if (_panel != NULL)
                    {
                        _panel->SetNewVal(PP, X);
                    }
                    if (_view != NULL)
                    {
                        if (_view->GetDocument() != NULL)
                        {
                            static_cast<ToneDocument *>(_view->GetDocument())->SetParamTo(PP, X);
                            _view->GetDocument()->Modify(wxTrue);
                        }
                    }
                }
                    
                /*# Leave the loop after the first parameter has been processed.
                # That prevents multiple parameters being changed when they
                # share a byte offset.
                #
                # For shared parameters, only one at a time will be bolded within
                # hints window. Is it the same one that is change here?? At the
                # moment it's left undefined, and may not necessarily behave as
                # expected.*/
                break;
            }
        }
    }
}

// A separator to place between numbers and letters in a ComboBox. Tab works well
// for linux, but looks wierd on Windows. Go with this as a compromise.
#define SEP "  "

class CustomListBox_FilterType : public wxComboBox
{
    public:
    CustomListBox_FilterType(wxWindow *parent) :
        wxComboBox()
    {
        wxString c_[9];
        c_[0] = wxString("0") + SEP + _("All-pass");
        c_[1] = wxString("1") + SEP + _("Low-pass");
        c_[2] = wxString("2") + SEP + _("High-pass");
        c_[3] = wxString("3") + SEP + _("Wide band-pass");
        c_[4] = wxString("4") + SEP + _("Bass shelf");
        c_[5] = wxString("5") + SEP + _("Treble shelf");
        c_[6] = wxString("6") + SEP + _("Band boost/cut");
        c_[7] = wxString("7") + SEP + _("Notch");
        c_[8] = wxString("8") + SEP + _("All-pass");

        wxComboBox::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 9, c_);
    }
  
};

class CustomListBox_WavetableTimbre : public wxComboBox
{
    public:
    CustomListBox_WavetableTimbre(wxWindow *parent) :
        wxComboBox()
    {
        wxString c_[4];
        c_[0] = wxString("0") + SEP + _("Melody");
        c_[1] = wxString("2") + SEP + _("Drum");
        c_[2] = wxString("4") + SEP + _("Piano");
        c_[3] = wxString("6") + SEP + _("Versatile");

        wxComboBox::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 4, c_);
    }
  
};

class CustomText_ToneName : public wxTextCtrl
{
    public:
    CustomText_ToneName(wxWindow *parent) :
        wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(192, 30))
    {
    }
  
};

class CustomListBox_DSPType : public wxComboBox
{
    public:
    CustomListBox_DSPType(wxWindow *parent) :
        wxComboBox()
    {
        wxString c_[32];
        c_[0]  = wxString("0")  + SEP + _("");
        c_[1]  = wxString("1")  + SEP + _("Stereo 3-band EQ");
        c_[2]  = wxString("2")  + SEP + _("Compress");
        c_[3]  = wxString("3")  + SEP + _("Limiter");
        c_[4]  = wxString("4")  + SEP + _("Enhancer");
        c_[5]  = wxString("5")  + SEP + _("Reflection");
        c_[6]   = wxString("6")  + SEP + _("Phaser");
        c_[7]  = wxString("7")  + SEP + _("Chorus");
        c_[8]  = wxString("8")  + SEP + _("Flange");
        c_[9]  = wxString("9")  + SEP + _("Tremolo");
        c_[10] = wxString("10") + SEP + _("Auto pan");
        c_[11] = wxString("11") + SEP + _("Rotary");
        c_[12] = wxString("12") + SEP + _("Drive rotary");
        c_[13] = wxString("13") + SEP + _("LFO wah");
        c_[14] = wxString("14") + SEP + _("Auto wah");
        c_[15] = wxString("15") + SEP + _("Distortion");
        c_[16] = wxString("16") + SEP + _("Pitch shift");
        c_[17] = wxString("17") + SEP + _("");
        c_[18] = wxString("18") + SEP + _("Ring modulation");
        c_[19] = wxString("19") + SEP + _("Delay");
        c_[20] = wxString("20") + SEP + _("Piano");
        c_[21] = wxString("21") + SEP + _("Stereo 1-band EQ");
        c_[22] = wxString("22") + SEP + _("Stereo 2-band EQ");
        c_[23] = wxString("23") + SEP + _("Drive");
        c_[24] = wxString("24") + SEP + _("Amp cabinet");
        c_[25] = wxString("25") + SEP + _("");
        c_[26] = wxString("26") + SEP + _("");
        c_[27] = wxString("27") + SEP + _("Mono 1-band EQ");
        c_[28] = wxString("28") + SEP + _("Mono 2-band EQ");
        c_[29] = wxString("29") + SEP + _("Mono 3-band EQ");
        c_[30] = wxString("30") + SEP + _("Model wah");
        c_[31] = wxString("31") + SEP + _("Tone control");
        wxComboBox::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 32, c_);
    }

};

class CustomListBox_LFOType : public wxComboBox
{
    public:
    CustomListBox_LFOType(wxWindow *parent) :
        wxComboBox()
    {
        wxString c_[7];
        c_[0]  = wxString("0")  + SEP + _("Sine");
        c_[1]  = wxString("1")  + SEP + _("Triangle");
        c_[2]  = wxString("2")  + SEP + _("Saw up");
        c_[3]  = wxString("3")  + SEP + _("Saw down");
        c_[4]  = wxString("4")  + SEP + _("1:3 rectangle");
        c_[5]  = wxString("5")  + SEP + _("Square");
        c_[6]  = wxString("6")  + SEP + _("3:1 rectangle");
        wxComboBox::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 7, c_);
    }

};

class CustomListBox_Portamento : public wxComboBox
{
    public:
    CustomListBox_Portamento(wxWindow *parent) :
        wxComboBox()
    {
        wxString c_[3];
        c_[0]  = wxString("0")  + SEP + _("Off");
        c_[1]  = wxString("1")  + SEP + _("On");
        c_[2]  = wxString("2")  + SEP + _("Legato");
        wxComboBox::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, c_);
    }

};

class CustomListBox_NoteOffVelocity : public wxComboBox
{
    public:
    CustomListBox_NoteOffVelocity(wxWindow *parent) :
        wxComboBox()
    {
        wxString c_[3];
        c_[0]  = wxString("0")  + SEP + _("Normal");
        c_[1]  = wxString("1")  + SEP + _("Note-on");
        c_[2]  = wxString("2")  + SEP + _("Minimum");
        wxComboBox::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, c_);
    }

};



void HintsPanelGeneric::OnTextChanged(wxCommandEvent& event)
{
    /*
    auto w = FindWindowById(event.Int);
    if (strcmp(w.Name[0:3], "C_P", 3))
    {
        p_idx = int(w.Name[3:])
            for PV in self.PARAMS:
                if PV.id_ == p_idx:
                    V_ = event.GetSelection()
                    if PV.type_ == 3:
                        V_ = 2*V_
                    self.Parent._buffer.SetParamTo(PV.param, V_)
                    self.Parent._view.Update()
                    break;
    }
    else
    {
        //raise Exception
    }
    * */
}

HintsPanelGeneric::HintsPanelGeneric(wxWindow *parent, std::list<std::pair<int, int>> params) : wxPanel(parent, wxID_ANY)
{
    wxFlexGridSizer *   _sizer_2 = new wxFlexGridSizer(3, wxSize(5,5));
    _sizer_2->SetFlexibleDirection(wxHORIZONTAL);

    PARAMS.clear();

    PP_ID PP = 0;
    int i = 0;
    
    for (auto PV = params.begin(); PV != params.end(); PV ++)
    {
        
        for (PP = 0; PP < sizeof(Parameters)/sizeof(Parameters[0]); PP ++)
        {
            if (Parameters[PP].number == PV->first && Parameters[PP].block0 == PV->second)
            {
            
                int PVtype_ = PVtype(PP);
                PARAMS.push_back(PP);

                wxControl * w_;


                if (PVtype_ == 1)
                {
                    w_ = new wxCheckBox(this, wxID_ANY, "");
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 2)
                {
                    w_ = new CustomListBox_FilterType(this);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 3)
                {
                    w_ = new CustomListBox_WavetableTimbre(this);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 4 || PVtype_ == 9 || PVtype_ == 10)
                {
                    w_ = new CustomText_ToneName(this);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                    // Bind the EVT_TEXT to this specific control ... ComboBoxs also raise this event,
                    // and we need to ignore for them.
                    Bind(wxEVT_TEXT, &HintsPanelGeneric::OnTextChanged, this, w_->GetId());
                }
                else if (PVtype_ == 5)
                {
                    w_ = new CustomListBox_DSPType(this);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 6)
                {
                    w_ = new CustomListBox_LFOType(this);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 7)
                {
                    w_ = new CustomListBox_Portamento(this);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 8)
                {
                    w_ = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -4, 3, 0);
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else if (PVtype_ == 11)
                {
                    w_ = new CustomListBox_NoteOffVelocity(this);
                    w_->SetPosition(wxPoint(5, 5+i*40));
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }
                else
                {
                    if (Parameters[PP].bitCount > 16)
                    {
                        // Put some arbitrary limit on the maximum field
                        w_ = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1023, 0);
                    }
                    else
                    {
                        w_ = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, (1 << Parameters[PP].bitCount)-1, 0);
                    }
                    w_->SetName(wxString::Format("C_P%d", /*PV.id_*/(int) PP));
                }

                _sizer_2->Add(w_, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

                wxStaticText*   u_ = new wxStaticText(this, wxID_ANY, Parameters[PP].name, wxDefaultPosition, wxDefaultSize, 0, "name");
                _sizer_2->Add(u_, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);
            
                wxStaticBitmap *v_ = new wxStaticBitmap(this, wxID_ANY, wxBitmap());
                if (! Parameters[PP].helpStr.IsEmpty())
                {
                    v_->SetIcon(wxArtProvider::GetIcon(wxART_HELP));
                    v_->SetToolTip(Parameters[PP].helpStr);
                }
                _sizer_2->Add(v_, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

                i ++;
                break;
            }

        }
    }

    SetSizer(_sizer_2);
    _sizer_2->Fit(this);

}
  

static bool PARAM_IS_STR(int X) {return (X==0 || X==84 || X==87);}


void HintsPanelGeneric::ReadValues(ToneDocument * doc_)
{

    for (auto iter = PARAMS.begin(); iter != PARAMS.end(); iter ++)
    {
        int PP = *iter;
        
       
        wxWindow * W_ = FindWindowByName(wxString::Format("C_P%d", (int) PP));

        if (PARAM_IS_STR(Parameters[PP].number))
        {
            static_cast<wxTextCtrl *>(W_)->SetValue(doc_->GetParamFromStr(PP));
        }
        else
        {
            int V_ = doc_->GetParamFrom(PP);

            if (PVtype(PP) == 1)
            {
                static_cast<wxCheckBox *>(W_)->SetValue(V_);
            }
            else if (PVtype(PP) == 4 || PVtype(PP) == 9 || PVtype(PP) == 10)
            {
                static_cast<wxSpinCtrl *>(W_)->SetValue(V_);
                
            }
            else
            {
                if (PVtype(PP) == 3)
                {
                    V_ /= 2;
                }
                else if (PVtype(PP) == 8)
                {
                    V_ -= 4;
                }
                
                if (PVtype(PP) == 2 ||
                        PVtype(PP) == 3 ||
                        PVtype(PP) == 5 ||
                        PVtype(PP) == 6 ||
                        PVtype(PP) == 7 ||
                        PVtype(PP) == 11)
                {
                    static_cast<wxComboBox *>(W_)->SetSelection(V_);
                }
                else
                {
                    static_cast<wxSpinCtrl *>(W_)->SetValue(V_);
                }
            }
        }
    }


}


void HintsPanelGeneric::SetNewVal(PP_ID PP, wxString val_)
{
    wxWindow *W_ = FindWindowByName(wxString::Format("C_P%d", (int) PP));
    //int type_ = PVtype(PP);
    static_cast<wxTextCtrl *>(W_)->SetValue(val_);
}

void HintsPanelGeneric::SetNewVal(PP_ID PP, int val_)
{
    wxWindow *W_ = FindWindowByName(wxString::Format("C_P%d", (int) PP));
    int type_ = PVtype(PP);
    if (type_ == 1)
        static_cast<wxCheckBox *>(W_)->SetValue((bool)val_);
    else if (type_ == 2 || type_ == 5 || type_ == 6 || type_ == 7 || type_ == 11)
        static_cast<wxComboBox *>(W_)->SetSelection(val_);
    else if (type_ == 3)
        static_cast<wxComboBox *>(W_)->SetSelection(val_ / 2);
    else if (type_ == 8)
        static_cast<wxSpinCtrl *>(W_)->SetValue(val_ - 4);
    else
        static_cast<wxSpinCtrl *>(W_)->SetValue(val_);
   // Parent._view->Update();
}

#if 0
void HintsDialog::OnChar(wxKeyEvent& event)
{
    bool _redraw = wxFalse;
    
    
    switch (   event.GetKeyCode()  )
    {
        case WXK_PAGEUP:
        case WXK_NUMPAD_PAGEUP:
            UpDown(CtrlVals::INCREASE);
            break;
        case WXK_PAGEDOWN:
        case WXK_NUMPAD_PAGEDOWN:
            UpDown(CtrlVals::DECREASE);
            break;
        case WXK_END:
            UpDown(CtrlVals::MINIMUM);
            break;
        case WXK_HOME:
        case WXK_NUMPAD_BEGIN:
            UpDown(CtrlVals::MAXIMUM);
            break;
        default:
            break;
    }

}
#endif
  
  
  
  
  
/*
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

        //self.Bind(wx.EVT_SPINCTRL, self.OnValueChanged)
        //self.Bind(wx.EVT_CHECKBOX, self.OnCheckChanged)
        //self.Bind(wx.EVT_COMBOBOX, self.OnComboBoxSelected)















*/











//IMPLEMENT_DYNAMIC_CLASS(HintsDialog, wxFrame)

//wxBEGIN_EVENT_TABLE(HintsDialog, wxFrame)
//wxEND_EVENT_TABLE()

/*
HintsDialog::HintsDialog() :
    wxFrame()//,
 //   _sizer(wxVERTICAL, this, "")
{
}
*/
HintsDialog::HintsDialog(wxWindow *parent) :
    wxFrame(parent, wxID_ANY, _("Hints"), wxDefaultPosition, wxDefaultSize, wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxCLIP_CHILDREN),
    _sizer(wxVERTICAL, this, ""),
    _font1(wxDEFAULT, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
    _font2(wxDEFAULT, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)
{

    _sizer.SetMinSize( wxSize(100, 100) );
    SetSizer(&_sizer);
    _sizer.Fit(this);
    
    _is_1B6 = wxFalse;
    _panel = NULL;
    _view = NULL;
    
    _current_cluster = -1;
    
    
    CLUSTERS_.clear();
    
    int PP;
    for (PP = 0; PP < sizeof(Parameters)/sizeof(Parameters[0]); PP ++)
    {
        if (strlen(Parameters[PP].cluster) > 0)
        {
             
            auto K = CLUSTERS_.find(std::string(Parameters[PP].cluster));
            if (K == CLUSTERS_.end())
            {
                CLUSTERS_[std::string(Parameters[PP].cluster)] = std::list<std::pair<int, int>>();
            }
            
            CLUSTERS_[std::string(Parameters[PP].cluster)].push_back(std::pair<int, int>(Parameters[PP].number, Parameters[PP].block0));
        }
    
    }
    
    
    
    
    // Process offsets only once clusters are done (so the ordering is fixed)
    OFFSETS_.clear();
    for (PP = 0; PP < sizeof(Parameters)/sizeof(Parameters[0]); PP ++)
    {
        if (strlen(Parameters[PP].cluster) > 0)
        {
            auto L = OFFSETS_.find(Parameters[PP].byteOffset);
            if (L == OFFSETS_.end())
            {
                int i = 0;
                for (auto CC = CLUSTERS_.begin(); CC != CLUSTERS_.end(); CC ++)
                {
                    if (CC->first.compare(Parameters[PP].cluster) == 0)
                    {
                        int XX;
                        for (XX = Parameters[PP].byteOffset; XX < Parameters[PP].byteOffset + Parameters[PP].byteCount; XX++)
                        {
                            OFFSETS_[XX] = i;
                        }
                        break;
                    }
                    
                    i ++;
                }
            }
        }
    }

        

    
    
    Bind(wxEVT_IDLE, &HintsDialog::OnIdle, this); // Want to adjust the starting position relative to the parent

    Bind(wxEVT_SPINCTRL, &HintsDialog::OnValueChanged, this);
    Bind(wxEVT_CHECKBOX, &HintsDialog::OnCheckChanged, this);
    Bind(wxEVT_COMBOBOX, &HintsDialog::OnComboBoxSelected, this);
}

HintsDialog::~HintsDialog()
{
    // Disassociate, but do not delete, the old sizer
    SetSizer(NULL, false);
}

void HintsDialog::OnIdle(wxIdleEvent& event)
{
    SetPosition( wxPoint( GetParent()->GetPosition().x + GetParent()->GetSize().x + 5, GetParent()->GetPosition().y + 100) );  // Move the window out of the way.
    Unbind(wxEVT_IDLE, &HintsDialog::OnIdle, this); // Only do this function once, right at the start. After that, can unbind
    event.Skip(true);
}

void HintsDialog::OnValueChanged(wxSpinEvent& event)
{
    if (_panel == NULL)
        return;   // Nothing we can do
    wxWindow * w_ = FindWindowById(event.GetId(), _panel);
    if (w_ == NULL)
    {
        wxLogError("Could not find window ", event.GetId());
        return;
    }
    
    std::list<PP_ID> PARAMS;
    
    for (auto iter = _panel->PARAMS.begin(); iter != _panel->PARAMS.end(); iter++)
    {
        if (wxString::Format("C_P%d", (int)*iter).IsSameAs(w_->GetName()))
        {
            int V_ = event.GetPosition();
            if (PVtype(*iter) == 8)
            {
                V_ += 4;
            }
            static_cast<ToneDocument *>(_view->GetDocument())->SetParamTo(*iter, V_);
            static_cast<ToneView *>(_view)->Update();
            break;
        }
    }
}

void HintsDialog::OnCheckChanged(wxCommandEvent& event)
{
    if (_panel == NULL)
        return;   // Nothing we can do
    wxWindow * w_ = FindWindowById(event.GetId(), _panel);
    if (w_ == NULL)
    {
        wxLogError("Could not find window ", event.GetId());
        return;
    }
    
    std::list<PP_ID> PARAMS;
    
    for (auto iter = _panel->PARAMS.begin(); iter != _panel->PARAMS.end(); iter++)
    {
        if (wxString::Format("C_P%d", (int)*iter).IsSameAs(w_->GetName()))
        {
            int V_ = event.IsChecked() ? 1 : 0;
            static_cast<ToneDocument *>(_view->GetDocument())->SetParamTo(*iter, V_);
            static_cast<ToneView *>(_view)->Update();
            break;
        }
    }
}

void HintsDialog::OnComboBoxSelected(wxCommandEvent& event)
{
    if (_panel == NULL)
        return;   // Nothing we can do
    wxWindow * w_ = FindWindowById(event.GetId(), _panel);
    if (w_ == NULL)
    {
        wxLogError("Could not find window ", event.GetId());
        return;
    }
    
    std::list<PP_ID> PARAMS;
    
    for (auto iter = _panel->PARAMS.begin(); iter != _panel->PARAMS.end(); iter++)
    {
        if (wxString::Format("C_P%d", (int)*iter).IsSameAs(w_->GetName()))
        {
            int V_ = event.GetSelection();
            if (PVtype(*iter) == 3)
                V_ = 2*V_;
            static_cast<ToneDocument *>(_view->GetDocument())->SetParamTo(*iter, V_);
            static_cast<ToneView *>(_view)->Update();
            break;
        }
    }
}

void HintsDialog::OnTextChanged(wxCommandEvent& event)
{
}


void HintsDialog::UpdateValues(wxDocument* doc_)
{
    if (_panel != NULL)
    {
        _panel->ReadValues(static_cast<ToneDocument *>(doc_));
    }
}



/*
void HintsDialog::MakeParamViewList(numlist)
{
      
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

}


*/














void HintsDialog::SetSelected(int offset_in_file)
{

    if ((offset_in_file >= 0x20 && offset_in_file < 0x1E8) || wxTrue)
    {
        int offset = offset_in_file - 0x20;
        
        if (offset >= 0)
            wxFrame::SetTitle(wxString(_("Hints")) + wxString::Format(": %03Xh", offset));
        else
            wxFrame::SetTitle(_("Hints"));


        int _enter = -1;
        int _exit = -1;

        try
        {
            _enter = OFFSETS_.at(offset);
        }
        catch (std::out_of_range)
        {
            _enter = -1;
        }


        if (_current_cluster >= 0)
        {
            if (_enter == _current_cluster)
            {
                _enter = -1;   // Not actually entering
            }
            else
            {
                _exit = _current_cluster;
            }
        }
        _current_offset = offset;




        if (_exit >= 0)
        {
            _sizer.Clear(false);
            _panel->Destroy();
            _panel = NULL;
            //self._paramviewlist = None
            _current_cluster = -1;
            _sizer.GetStaticBox()->SetLabelText("");
        }
            
        if (_enter >= 0)
        {
            
            
            int i = 0;
            
            
            //std::iterator<std::forward_iterator_tag, std::map<int, int>> CC;
            
            std::map<std::string, std::list<std::pair<int, int>>>::iterator CC;
            
            for (CC = CLUSTERS_.begin(); CC != CLUSTERS_.end(); CC ++)
            {
                i ++;
                if (i > _enter)
                {
                    break;
                }
            }
            
            if (CC == CLUSTERS_.end())
            {
                // Error! What to do?
                wxLogError("Reached end of clusters");
            }
            else
            {

                _panel = new HintsPanelGeneric(this, CC->second);
                
                
                //new wxPanel(this, wxID_ANY);
    
    /*
    
                wxFlexGridSizer *   _sizer_2 = new wxFlexGridSizer(2, wxSize(5,5));
                _sizer_2->SetFlexibleDirection(wxHORIZONTAL);
        

                wxSpinCtrl *w_ = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 127, 0);

                _sizer_2->Add(w_, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);
          

                wxStaticText*   u_ = new wxStaticText(_panel, wxID_ANY, "TheLabel", wxDefaultPosition, wxDefaultSize, 0, "name");
            
                _sizer_2->Add(u_, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);
            
            
          

                _panel->SetSizer(_sizer_2);
                _sizer_2->Fit(_panel);
*/

                _sizer.Add(_panel);
            
            
                _sizer.GetStaticBox()->SetLabelText(CC->first); 
            

                _current_cluster = _enter;

            /*
            if self._paramviewlist is not None:
                sel_id = None
                for PV in self._paramviewlist:
                    if offset in PV.offsets:
                        sel_id = PV.id_
                        break
                if self._panel is not None:
                    self._panel.Update(sel_id)*/
            }
        }
        
        
        if ((_exit >= 0) || (_enter >= 0))
        {
            _sizer.Fit(this);
            _sizer.Layout();
            Layout();
            Refresh();
        }

    }

}

