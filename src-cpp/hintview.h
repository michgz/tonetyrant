
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


typedef int PP_ID;   // This represents an offset into the Parameters table


class HintsPanelGeneric : public wxPanel
{
    public:
    HintsPanelGeneric(wxWindow *parent,  std::list<std::pair<int, int>> params);
    void OnTextChanged(wxCommandEvent& event);
    
    
    
    void ReadValues(ToneDocument * doc_);
    
    //std::list<std::pair<int, int>> PARAMS;
    std::list<PP_ID> PARAMS;
    
};

class CtrlVals
{
    public:
    const static int INCREASE = 0;
    const static int DECREASE = 1;
    const static int MAXIMUM = 2;
    const static int MINIMUM = 3;
};

class HintsDialog : public wxFrame
{
    public:

    //HintsDialog(){}
    HintsDialog(wxWindow *parent);
    ~HintsDialog();
    void OnIdle(wxIdleEvent& event);

    void UpdateValues(wxDocument* doc_);

    wxStaticBoxSizer    _sizer;

    void SetSelected(int);
    bool    _is_1B6;
    HintsPanelGeneric *   _panel ;
    wxView *  _view ;
    wxFont   _font1 ;
    wxFont   _font2 ;
    
    
    std::map<std::string, std::list<std::pair<int, int>>> CLUSTERS_;
    
    std::map<int, int> OFFSETS_;
    
    int _current_cluster;
    int _current_offset;


    void UpDown(int ctrl_val);
    void OnChar(wxKeyEvent& event);



  //  wxDECLARE_EVENT_TABLE();
  //  wxDECLARE_DYNAMIC_CLASS(HintsDialog);
};

#endif // _HINTVIEW_DOCVIEW_H_
