
#ifndef _HINTVIEW_DOCVIEW_H_
#define _HINTVIEW_DOCVIEW_H_


#include <wx/wxprec.h>

#include "wx/docview.h"
#include "view.h"
#include "doc.h"
#include "tyrant.h"

#include <map>
#include <utility>
#include <string>
#include <list>
#include <set>


typedef int PP_ID;   // This represents an offset into the Parameters table


class HintsPanelGeneric : public wxPanel
{
    public:
    HintsPanelGeneric(wxWindow *parent,  std::list<std::pair<int, int>> params);
    
    void SetNewVal(PP_ID PP, wxString val_);
    void SetNewVal(PP_ID PP, int val_);
    
    void ReadValues(ToneDocument * doc_);
    
    //std::list<std::pair<int, int>> PARAMS;
    std::list<PP_ID> PARAMS;
    
};

enum CtrlVals
{
    INCREASE = 0,
    DECREASE = 1,
    MAXIMUM = 2,
    MINIMUM = 3,
};

class HintsDialog : public wxFrame
{
    public:

    //HintsDialog(){}
    HintsDialog(wxWindow *parent);
    ~HintsDialog();
    void OnIdle(wxIdleEvent& event);

    void OnValueChanged(wxSpinEvent& event);
    void OnCheckChanged(wxCommandEvent& event);
    void OnComboBoxSelected(wxCommandEvent& event);
    void OnTextChanged(wxCommandEvent& event);
    
    void UpdateValues(wxDocument* doc_);

    wxStaticBoxSizer    _sizer;

    void SetSelected(int);
    bool    _is_1B6;
    HintsPanelGeneric *   _panel ;
    wxView *  _view ;
    wxFont   _font1 ;
    wxFont   _font2 ;
    
    void SetView(wxView * __view) {_view = __view;}
    void ResetView(void) {_view = NULL;}
        
    std::map<std::string, std::list<std::pair<int, int>>> CLUSTERS_;
    
    std::map<int, int> OFFSETS_;
    std::set<unsigned short int> _highlight_list;
    std::set<unsigned short> GetHighlightList(void);
    
    int _current_cluster;
    int _current_offset;


    void UpDown(CtrlVals ctrl_val);
    void OnChar(wxKeyEvent& event);


    static int SuggestStep(PP_ID PP);

    private:
    void MakeHighlightList(std::list<std::pair<int, int>> params);
    
    
  //  wxDECLARE_EVENT_TABLE();
  //  wxDECLARE_DYNAMIC_CLASS(HintsDialog);
};

#endif // _HINTVIEW_DOCVIEW_H_
