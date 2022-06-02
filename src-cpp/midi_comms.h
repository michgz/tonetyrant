
#ifndef _MIDI_COMMS_H_
#define _MIDI_COMMS_H_

#include <wx/wxprec.h>
#include "doc.h"  // PP_ID


class MidiSetupDialog : public wxDialog
{
    /*
    A dialog for the user to set MIDI ports
    */
    public:
    MidiSetupDialog(wxWindow * parent);

    void DoOk(void);


    
    
};


class MidiComms
{
    /* Encapsulates configuration of the MIDI   */


    public:

    std::string     _input_name;
    std::string     _output_name;
    int             _realtime_channel;
    bool            _realtime_enable;
    int             _logging_level;


    void init(void);
    MidiComms(void);
    int WriteToConfig(void);
    
    
    // A parameter has changed in a document
    void SetParamTo(PP_ID PP, unsigned int p_val);


};

extern void midi_comms_set_param_to(PP_ID PP, unsigned int p_val);


void upload_ac7_internal(std::vector<unsigned char> data, int param_set, int memory=1, int category=30, bool _debug=wxFalse);

#endif // _MIDI_COMMS_H_




