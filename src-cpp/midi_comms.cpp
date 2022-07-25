

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "RtMidi.h"
#include <vector>


#include "midi_comms.h"
#include "parameters.h"

#define wxTrue true
#ifndef wxFalse
#define wxFalse false
#endif


#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "inih/INIReader.h"
#include "Crc32.h"



/* Define this macro to turn on command-line logging of sent and
 * received SYSEX messages. Generally should be undefined.
 */
#undef MIDI_SYSEX_LOG_TO_CONSOLE



/*

    def __init__(self):
        cfg = configparser.ConfigParser()
        cfg.read('tyrant.cfg')
        self._input_name = cfg.get('Midi', 'InPort', fallback="")
        self._output_name = cfg.get('Midi', 'OutPort', fallback="")
        self._realtime_channel = int(cfg.get('Midi Real-Time', 'Channel', fallback="0"))
        self._realtime_enable = cfg.get('Midi Real-Time', 'Enable', fallback="False").lower() in ['true', '1', 't', 'y', 'yes']
        self._logging_level = 0
        if cfg.get("Logging", "Level", fallback="").lower() in ['on', '1']:
            self._logging_level = 1
        elif cfg.get("Logging", "Level", fallback="").lower() in ['2']:
            self._logging_level = 2
        # Translate the logging levels to Python logging levels as follows:
        #   0 = no logging of interest to user. (WARNING)
        #   1 = logging of each SysEx message, for use by user. (INFO)
        #   2 = logging of each SysEx message, as well as other extraneous stuff. (DEBUG)
        if self._logging_level == 1:
            logging.getLogger().setLevel(logging.INFO)
        elif self._logging_level == 2:
            logging.getLogger().setLevel(logging.DEBUG)
        
        self._queue = queue.Queue()
        self._thread = MidiComms.MidiThread(self)
        self._thread.start()

        
 
    def QueueParamVal(self, P : parameters.Param, p_val):
        if self._realtime_enable:
            self._queue.put( (P.number, P.block0, P.midiBytes, self._realtime_channel, p_val), block=False )
 
    def Close(self):
        self._queue.put( MidiComms.MidiThread.StopSignal(), block=False)
        self._thread.join(1.0)  # Time out after 1 second
 
    def SetParamTo(self, P : parameters.Param, p_val):
        self.set_single_parameter(P.number, p_val, midi_bytes=P.midiBytes, category=3, memory=3, parameter_set=self._realtime_channel, block0=P.block0)
        return True

*/




void MidiComms::init(void)
{

    INIReader reader("tyrant.cfg");

    if (reader.ParseError() != 0) {
        // Could not load config. Put in some default values and return
        
        _input_name = "";
        _output_name = "";
        _realtime_channel = 0;
        _realtime_enable = false;
        _logging_level = 0;

        return;
    }

    _input_name = reader.Get("Midi", "InPort", "");
    _output_name = reader.Get("Midi", "OutPort", "");
    _realtime_channel = reader.GetInteger("Midi Real-Time", "Channel", 0);
    _realtime_enable = reader.GetBoolean("Midi Real-Time", "Enable", false);
    
    if (reader.Get("Logging", "Level", "") == "2")
    {
        _logging_level = 2;
    }
    else
    {
        _logging_level = (int) reader.GetBoolean("Logging", "Level", false);
    }

    /* Translate the logging levels to Python logging levels as follows:
     *   0 = no logging of interest to user. (WARNING)
     *   1 = logging of each SysEx message, for use by user. (INFO)
     *   2 = logging of each SysEx message, as well as other extraneous stuff. (DEBUG)   */

}

MidiComms::MidiComms(void)
{
    init();
}


int MidiComms::WriteToConfig(void)
{
    std::ofstream cfgfile;
    cfgfile.open ("tyrant.cfg");
    cfgfile << "[Midi]\nInPort = " << _input_name << "\nOutPort = " << _output_name << "\n\n";
    cfgfile << "[Midi Real-Time]\nChannel = " << _realtime_channel << "\nEnable = ";
    if (_realtime_enable)
    {
        cfgfile << "on\n\n";
    }
    else
    {
        cfgfile << "off\n\n";
    }

    cfgfile << "[Logging]\nLevel = " << _logging_level << "\n";
    cfgfile.close();

    return 0;
    
}




static MidiComms _midiComms;




void MidiSetupDialog::DoOk(void)
{
    /* Called if the "OK" button is pressed on the dialog. That means we read the selected
     *  values and store to a config file.
     * */
    
    bool b = (bool) static_cast<wxCheckBox *>(FindWindow("MidiRealTimeEnable"))->GetValue();
    _midiComms._realtime_enable = b;
    
    
    int k = static_cast<wxComboBox *>(FindWindow("MidiRealTimeChannel"))->GetSelection();
    if (k == wxNOT_FOUND)
    {
        /* When does this happen?   */
        _midiComms._realtime_channel = 0;
    }
    else if (k == 1)
    {
        _midiComms._realtime_channel = 32;
    }
    else
    {
        _midiComms._realtime_channel = 0;
    }
    
    
    k = static_cast<wxListBox *>(FindWindow("MidiInputPortSel"))->GetSelection();
    if (k == wxNOT_FOUND)
    {
        _midiComms._input_name = "";
    }
    else
    {
        _midiComms._input_name = static_cast<wxListBox *>(FindWindow("MidiInputPortSel"))->GetString(k);
    }

    k = static_cast<wxListBox *>(FindWindow("MidiOutputPortSel"))->GetSelection();
    if (k == wxNOT_FOUND)
    {
        _midiComms._output_name = "";
    }
    else
    {
        _midiComms._output_name = static_cast<wxListBox *>(FindWindow("MidiOutputPortSel"))->GetString(k);
    }
    
    _midiComms.WriteToConfig();
}




const wxString SEP("  ");

MidiSetupDialog::MidiSetupDialog(wxWindow * parent) :
            wxDialog(parent, wxID_ANY, _("MIDI Setup"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{

    wxWindowID INPUT_PORT_SEL_ID = wxWindow::NewControlId();
    wxWindowID OUTPUT_PORT_SEL_ID = wxWindow::NewControlId();
    wxWindowID RT_CHANNEL_ID = wxWindow::NewControlId();
    wxWindowID RT_ENABLE_ID = wxWindow::NewControlId();






    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, _("Input ports:"), wxDefaultPosition, wxSize(200, 22), 0));


    RtMidiIn * midi_in = new RtMidiIn();

    wxListBox * lst_1 = new wxListBox(this, INPUT_PORT_SEL_ID, wxDefaultPosition, wxSize(400,100), 0, NULL, wxLB_SINGLE, wxDefaultValidator, "MidiInputPortSel");
 
   
   
    int i;
    int j = midi_in->getPortCount();
    int k = -1;

    for (i = 0; i < j; i ++)
    {
        wxString s(midi_in->getPortName(i));
        lst_1->InsertItems(1, &s, i);
        if (s.compare(_midiComms._input_name) == 0)
        {
            k = i;
        }
    }
    if (k >= 0)
    {
        lst_1->SetSelection(k);
    }
    else
    {
        lst_1->SetSelection(wxNOT_FOUND);  // Clear selection
    }


    delete midi_in;



    sizer->Add(lst_1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);

    sizer->Add(new wxStaticText(this, wxID_ANY, _("Output ports:"), wxDefaultPosition, wxSize(200, 22), 0)     /* ?? options */);
    
    RtMidiOut * midi_out = new RtMidiOut();
            
            
            
    wxListBox * lst_2 = new wxListBox(this, OUTPUT_PORT_SEL_ID, wxDefaultPosition, wxSize(400,100), 0, NULL, wxLB_SINGLE, wxDefaultValidator, "MidiOutputPortSel");
 
   
   
    j = midi_out->getPortCount();

    for (i = 0; i < j; i ++)
    {
        wxString s(midi_out->getPortName(i));
        lst_2->InsertItems(1, &s, i);
        if (s.compare(_midiComms._output_name) == 0)
        {
            k = i;
        }
    }
    if (k >= 0)
    {
        lst_2->SetSelection(k);
    }
    else
    {
        lst_2->SetSelection(wxNOT_FOUND);  // Clear selection
    }


    delete midi_out;
            
    sizer->Add(lst_2, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5);



    wxStaticBoxSizer * _pnl = new wxStaticBoxSizer(wxHORIZONTAL, this, "");
            
            
    wxCheckBox       *  w_1 = new wxCheckBox(this, RT_ENABLE_ID, _("Enable"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE, wxDefaultValidator, "MidiRealTimeEnable");
   
   
   
    const bool realtime_enable = _midiComms._realtime_enable;
    const int realtime_channel = _midiComms._realtime_channel;
   
    _pnl->Add(w_1, 0, wxALIGN_CENTRE, 5);
            
    w_1->SetValue(realtime_enable);
            

            
    wxString choices[2]= {wxString("0") + SEP + "Upper keyboard 1", wxString("32") + SEP + "MIDI In 1"};
            
            
    wxComboBox * w_2 = new wxComboBox(this, RT_CHANNEL_ID, "", wxDefaultPosition, wxDefaultSize, 2, choices, 0, wxDefaultValidator, "MidiRealTimeChannel");
            

      
            
    _pnl->Add(w_2, 0, wxBOTTOM, 5);
            
    if (realtime_channel == 32)
        w_2->SetSelection(1);
    else
        w_2->SetSelection(0);
            
            
    wxStaticText*  w_3 = new wxStaticText(this, wxID_ANY, _("Channel"));
    _pnl->Add(w_3, 0, wxLEFT, 5);
            
           
    wxStaticText*     w_4 = new wxStaticText(this, wxID_ANY, _("Real-Time Control:"));
    sizer->Add(w_4, 0, wxTOP | wxLEFT, 15);
            
    sizer->Add(_pnl, 0, wxEXPAND, 5);




            
    sizer->Add(new wxButton(this, wxID_OK), 0, wxALIGN_CENTRE|wxALL, 5);
    sizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALIGN_CENTRE|wxALL, 5);

    SetSizer(sizer);
    sizer->Fit(this);
}
/*
    def AllowMidi(self):
        """
        Return True if MIDI operations are possible
        """
        return True


    def ShowMidiSetup(self, parent):
        """
        Show the MidiSetupDialog.
        """
        dlg = self.MidiSetupDialog(parent, self._input_name, self._output_name, self._realtime_channel, self._realtime_enable)
        dlg.CenterOnParent()
        ok_result = dlg.ShowModal()
        
        
        if ok_result == wx.ID_OK:

            cfg = configparser.ConfigParser()
            cfg.read('tyrant.cfg')


            _lst = dlg.FindWindowById(INPUT_PORT_SEL_ID)
            _n = _lst.GetSelection()
            if _n != wx.NOT_FOUND:
                self._input_name = _lst.GetString(_n)
                if not cfg.has_section("Midi"):
                    cfg.add_section("Midi")
                cfg.set('Midi', 'InPort', self._input_name)

            _lst = dlg.FindWindowById(OUTPUT_PORT_SEL_ID)
            _n = _lst.GetSelection()
            if _n != wx.NOT_FOUND:
                self._output_name = _lst.GetString(_n)
                if not cfg.has_section("Midi"):
                    cfg.add_section("Midi")
                cfg.set('Midi', 'OutPort', self._output_name)
        
            _w1 = dlg.FindWindowById(RT_ENABLE_ID)
            _n1 = bool(_w1.GetValue())
            self._realtime_enable = _n1
            if not cfg.has_section("Midi Real-Time"):
                cfg.add_section("Midi Real-Time")
            cfg.set("Midi Real-Time", "Enable", str(_n1))
        
            _w2 = dlg.FindWindowById(RT_CHANNEL_ID)
            _n2 = 32 if _w2.GetSelection() == 1 else 0
            self._realtime_channel = _n2
            if not cfg.has_section("Midi Real-Time"):
                cfg.add_section("Midi Real-Time")
            cfg.set("Midi Real-Time", "Channel", str(_n2))
        
            if not cfg.has_section("Logging"):
                cfg.add_section("Logging")
            cfg.set("Logging", "Level", str(self._logging_level))
        
        
            with open('tyrant.cfg', 'w') as cfg_file:
                cfg.write(cfg_file)
        
        dlg.Destroy()




*/


























std::vector<unsigned char> midi_7bit_to_8bit(std::vector<unsigned char> b)
{
    /*
    Decode from MIDI "7-bit format", which requires the MSb of each byte to be
    zero.
    */

    unsigned int r = 0;  // remainder
    int n = 0;  // position of split
    int i = 0;  // pointer to input
    std::vector<unsigned char> c; // output
  
  
    while (i < b.size())
    {
        unsigned char x = b[i];
  
        if (x >= 128)
        {
            //raise Exception("Not valid 7-bit data at position {0} : {1:02X}!".format(i, x));
        }

        if (n == 0 || n == 8)
        {
            r = x;
            n = 0;
        }
        else if (n == 1)
        {
            c.push_back(((x&0x01)<<7) + r);
            r = x/2;
        }
        else if (n == 2)
        {
            c.push_back(((x&0x03)<<6) + r);
            r = x/4;
        }
        else if (n == 3)
        {
            c.push_back(((x&0x07)<<5) + r);
            r = x/8;
        }
        else if (n == 4)
        {
            c.push_back(((x&0x0f)<<4) + r);
            r = x/16;
        }
        else if (n == 5)
        {
            c.push_back(((x&0x1f)<<3) + r);
            r = x/32;
        }
        else if (n == 6)
        {
            c.push_back(((x&0x3f)<<2) + r);
            r = x/64;
        }
        else if (n == 7)
        {
            c.push_back(((x&0x7f)<<1) + r);
            r = 0;
        }
        i += 1;
        n += 1;
    }
    if (r != 0)
    {
        //raise Exception("Left over data! Probably an error");
    }
  
    return c;
  
}



std::vector<unsigned char> midi_8bit_to_7bit(std::vector<unsigned char> b)
{
    /*
    Encode to MIDI "7-bit format", which requires the MSb of each byte to be
    zero.
    */

    unsigned int r = 0;  // remainder
    int n = 0;  // position of split
    int i = 0;  // pointer to input
    std::vector<unsigned char> c; // output
      
    while (i < b.size())
    {
        if (n == 0 || n == 7)
        {
            n = 0;
            c.push_back(0x1*(b[i] & 0x7f));
            r = (b[i] & 0x80)/0x80;
        }
        else if (n == 1)
        {
            c.push_back(r + 0x2*(b[i] & 0x3f));
            r = (b[i] & 0xc0)/0x40;
        }
        else if (n == 2)
        {
            c.push_back(r + 0x4*(b[i] & 0x1f));
            r = (b[i] & 0xe0)/0x20;
        }
        else if (n == 3)
        {
            c.push_back(r + 0x8*(b[i] & 0x0f));
            r = (b[i] & 0xf0)/0x10;
        }
        else if (n == 4)
        {
            c.push_back(r + 0x10*(b[i] & 0x07));
            r = (b[i] & 0xf8)/0x8;
        }
        else if (n == 5)
        {
            c.push_back(r + 0x20*(b[i] & 0x03));
            r = (b[i] & 0xfc)/0x4;
        }
        else if (n == 6)
        {
            c.push_back(r + 0x40*(b[i] & 0x01));
            c.push_back((b[i] & 0xfe)/0x2);
            r = 0;
        }
        n += 1;
        i += 1;
    }
    if (n < 7)
    {
        c.push_back(r);
    }
    return c;
}



   /* Define the device ID. Constructed as follows:
    #    0x44       Manufacturer ID ( = Casio)
    #    0x19 0x01  Model ID ( = CT-X3000, CT-X5000, CT-X700)
    #    0x7F       Device. This is a "don't care" value
    #*/
const std::vector<unsigned char> DEVICE_ID = {0x44, 0x19, 0x01, 0x7F};
static const unsigned short int EMPTY_BLOCKS[4] = {0,0,0,0};
    
std::vector<unsigned char> make_packet(bool tx,
                                        std::vector<unsigned char> data,
                                        unsigned short int category=30,
                                        unsigned short int memory=1,
                                        unsigned short int parameter_set=0,
                                        const unsigned short int block[4] = EMPTY_BLOCKS,
                                        unsigned short int parameter=0,
                                        unsigned char      index=0,
                                        int                length=1,
                                        int                command=-1,
                                        int                sub_command=3)
{
    /*
    Construct a packet in Casio SYSEX format, for sending over the MIDI port
    */
      
    std::vector<unsigned char> w;

    w.push_back(0xF0);
    w.push_back(DEVICE_ID[0]);
    w.push_back(DEVICE_ID[1]);
    w.push_back(DEVICE_ID[2]);
    w.push_back(DEVICE_ID[3]);


    if (command < 0)
    {
        if (tx)
        {
            command = 1;
        }
        else
        {
            command = 0;
        }
    }
    w.push_back((unsigned char) command);

    if (command == 0x8)
    {
        w.push_back((unsigned char) sub_command);
        w.push_back(0xF7);
        return w;
    }

    w.push_back((unsigned char) category);
    w.push_back((unsigned char) memory);
    w.push_back((unsigned char) (parameter_set & 0x007F));
    w.push_back((unsigned char) (parameter_set/128));

    if (command == 0xA)
    {
        w.push_back(0xF7);
        return w;
    }
    else if (command == 5)
    {
        w.push_back((unsigned char) (length & 0x007F));
        w.push_back((unsigned char) (length/128));
        int k;
        std::vector<unsigned char> to_append = midi_8bit_to_7bit(data);
        for (k = 0; k < to_append.size(); k ++)
        {
            w.push_back(to_append[k]);
        }
        uint32_t crc_val = crc32_fast(&(*(w.begin() + 1)), w.size() - 1);
        std::vector<unsigned char> crc_vec = std::vector<unsigned char>();
        crc_vec.push_back(((unsigned char *) &crc_val)[0]);
        crc_vec.push_back(((unsigned char *) &crc_val)[1]);
        crc_vec.push_back(((unsigned char *) &crc_val)[2]);
        crc_vec.push_back(((unsigned char *) &crc_val)[3]);
        to_append = midi_8bit_to_7bit(crc_vec);
        for (k = 0; k < to_append.size(); k ++)
        {
            w.push_back(to_append[k]);
        }
        w.push_back(0xF7);
        return w;
    }
    else if ((command >= 2 && command < 8) || command == 0xD || command == 0xE) // OBR/HBR doesn't have the following stuff
    {
    }
    else
    {
        // Blocks are done in reverse order. This means that block[0] is block0, etc.
        w.push_back((unsigned char) (block[3] & 0x007F));
        w.push_back((unsigned char) (block[3]/128));
        w.push_back((unsigned char) (block[2] & 0x007F));
        w.push_back((unsigned char) (block[2]/128));
        w.push_back((unsigned char) (block[1] & 0x007F));
        w.push_back((unsigned char) (block[1]/128));
        w.push_back((unsigned char) (block[0] & 0x007F));
        w.push_back((unsigned char) (block[0]/128));
        w.push_back((unsigned char) (parameter & 0x007F));
        w.push_back((unsigned char) (parameter/128));
        w.push_back((unsigned char) (index));
        w.push_back((unsigned char) (0));
        w.push_back((unsigned char) (length-1));
        w.push_back((unsigned char) (0));
    }
    if (tx)
    {
        for (auto D = data.begin(); D != data.end(); D++)
        {
            w.push_back(*D);
        }
    }
    w.push_back(0xF7);
    return w;

}



int set_single_parameter(int parameter, 
                        unsigned long int data, 
                        int midi_bytes=1, 
                        int category=3, 
                        int memory=3, 
                        int parameter_set=0, 
                        int block0=0, 
                        int block1=0, 
                        bool _debug=wxFalse)
{
    /*
    Send a single parameter value to the keyboard
    */

    //_logger = logging.getLogger()
    //_logger.info(f" parameter {parameter}, block {block0} <- {str(data)}:")




    // Open the device (if needed)
    /*   midiin = rtmidi.MidiIn()
    midiout = rtmidi.MidiOut()
    for i in range(midiout.get_port_count()):
      if self._output_name == midiout.get_port_name(i):
          midiout.open_port(port=i)
    for i in range(midiin.get_port_count()):
      if self._input_name == midiin.get_port_name(i):
          midiin.open_port(port=i)
    if not midiout.is_port_open() or not midiin.is_port_open():
      raise Exception("Could not find the named port")

    midiin.ignore_types(sysex=False)*/



    RtMidiOut * midi_out = new RtMidiOut();

    int i;
    int j = midi_out->getPortCount();

    for (i = 0; i < j; i ++)
    {
        wxString s(midi_out->getPortName(i));
        if (s.compare(_midiComms._output_name) == 0)
        {
            break;
        }
    }
    
    if (i >= j)
    {
        // Did not find the port
        delete midi_out;
        return -1;
    }
    
    
    midi_out->openPort(i);
    


    // Flush the input queue
    /* time.sleep(0.01)
    midiin.get_message()*/

    // Prepare the input
    std::vector<unsigned char> d;
    int l = 1;

    //if isinstance(data, type(0)):
    if (true)
    {
        /* The input is an integer. The "length" parameter passed to make_packet must be
        # 1, but we don't know how many bytes of bit-stuffed data the keyboard is actually
        # expecting. Use the "midi_bytes" parameter for that.*/

        int key_len = midi_bytes;
        
        // Now do the bit-stuffing
        for (int i = 0; i < key_len; i ++)
        {
            d.push_back((unsigned char) data&0x7F);
            data = data/0x80;
        }
        l = 1;   // length is always 1 for numeric inputs
    }
    else
    {
        // Assume the input is a byte array
      //  d = data
      //  l = len(d)
    }
      
    // Write the parameter
    unsigned short int blocks[4] = {block0,block1, 0, 0};
    auto pkt = make_packet(wxTrue, d, category, memory, parameter_set, blocks, parameter, 0, 1);
    midi_out->sendMessage(&pkt);

#if 0
    _logger.info("    " + pkt.hex(" ").upper())
    time.sleep(0.1)
    // Handle any response -- don't expect one
    midiin.get_message()
    time.sleep(0.01)

    // Close the device
    midiin.close_port()
    midiout.close_port()

    /* Also delete the instances. See notes in rtmidi-python documentation. This is
    # needed to get around delays in the python garbage-collector. A better
    # solution might be to keep the ports open and close only when exiting the
    # program.*/
    midiin.delete()
    midiout.delete()
#endif

    // A delay here seems to reduce the incidence of MIDI port errors, especially
    // in Windows. May refine this a bit more.
    wxMilliSleep(50);


    midi_out->closePort();
    delete midi_out;
    
    
    if (_midiComms._logging_level >= 1)
    {
        std::ofstream logfile;

      logfile.open("tyrant.log", std::ios_base::app); // append
      logfile << "<   ";
      int j1;
      for (j1 = 0; j1 < pkt.size(); j1 ++)
      {
          logfile << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned short int>(pkt[j1]) << " ";
      }
      logfile << std::endl;
    }
    
    
    return 0;

}

void MidiComms::SetParamTo(PP_ID P, unsigned int p_val)
{
    if (_realtime_enable)
    {
        set_single_parameter(Parameters[P].number, p_val, Parameters[P].midiBytes, 3, 3, _realtime_channel, Parameters[P].block0, 0);
    }
}

void midi_comms_set_param_to(PP_ID PP, unsigned int p_val)
{
    _midiComms.SetParamTo(PP, p_val);
}



static bool have_got_ack;
static bool have_got_error;
static std::vector<unsigned char> so_far;
static bool is_busy;
static bool must_send_ack;
static bool have_got_ess;
static std::vector<unsigned char> total_rxed;


static void handle_pkt(std::vector<unsigned char> p)
{
    /*
    Handle a complete packet as received from the MIDI port. It is assumed that
    each packet will be in Casio SYSEX format.
    */
    unsigned char type_of_pkt;
      
    //print(p.hex(" "))
    if (p.size() < 7)
    {
        //print("BAD PACKET!!")
        return;
    }
    if ((p[0] != 0xF0) || (p[1] != 0x44) || (p[4] != 0x7F) || (p[p.size()-1] != 0xF7))
    {
        //print("BAD PACKET!!")
        return;
    }
    type_of_pkt = p[5];
    if (type_of_pkt == 0xB)
    {
        is_busy = wxTrue;
    }
    else
    {
        is_busy = wxFalse;
        if (type_of_pkt == 0xA)
        {
            have_got_ack = wxTrue;
        }
        if (type_of_pkt == 0xD)
        {
            have_got_ack = wxTrue;
            have_got_ess = wxTrue;
        }
        if (type_of_pkt == 0xF)
        {
            have_got_error = wxTrue;
        }
    }
          
    if ((type_of_pkt == 3) || (type_of_pkt == 5))   // This takes a CRC
    {
        //c = struct.unpack('<5B', p[-6:-1])
        uint32_t crc_compare =    (uint32_t) p[p.size()-6]
                                + (((uint32_t) p[p.size()-5])<<7)
                                + (((uint32_t) p[p.size()-4])<<14)
                                + (((uint32_t) p[p.size()-3])<<21)
                                + (((uint32_t) p[p.size()-2])<<28);
        if (wxTrue)   //binascii.crc32(p[1:-6]) == crc_compare:
        {
            must_send_ack = wxTrue;
            if (type_of_pkt == 5)
            {
                have_got_ack = wxTrue; // This one must look like an ACK
                std::vector<unsigned char> temp;
                int i;
                for (i = 12; i < p.size()-6; i ++)
                {
                    temp.push_back(p[i]);
                }
                auto mm = midi_7bit_to_8bit(temp);
                for (i = 0; i < mm.size(); i ++)
                {
                    total_rxed.push_back(mm[i]);
                }
            }
        }
        else
        {
            //print("BAD CRC!!!")
        }
          
    }


    if (type_of_pkt == 1)
    {
        //v = p[24:-1]
        //type_1_rxed = v;
    }
}


void parse_response(std::vector<unsigned char> b, bool _debug=wxFalse)
{
    /*
    Parse bytes received from the MIDI port, collating them into SYSEX packets.
    */
      
    bool in_pkt = wxTrue;
    if (so_far.size() == 0)
    {
        in_pkt = wxFalse;
    }
    
    int i;
    for (i = 0; i < b.size(); i ++)
    {
        unsigned char x = b[i];
        if (in_pkt)
        {
            if (x == 0xF7)
            {
                so_far.push_back(0xf7);
                // Have completed. Do something!
                if (_debug)
                {
                    //print(so_far.hex(" ").upper())
                }
                handle_pkt(so_far);
                in_pkt = wxFalse;
                so_far.clear();
            }
            else if (x == 0xF0)
            {
                // Error! but start a new packet
                so_far.clear();
                so_far.push_back(0xf0);
            }
            else if (x >= 0x80)
            {
                // Error!
                in_pkt = wxFalse;
                so_far.clear();
            }
            else
            {
                so_far.push_back(x);
            }
        }
        else
        {
            if (x == 0xF0)
            {
                so_far.clear();
                so_far.push_back(0xf0);
                in_pkt = wxTrue;
            }
        }
    }

}


static const std::vector<unsigned char> EMPTY_VEC = std::vector<unsigned char>();




static void wait_for_ack(RtMidiIn * midi_in)
{
    /*
    Receive bytes from the MIDI port and parse them until an ACK packet has
    been seen. If more than 4 seconds passes then an exception will be raised.
    */
    //global have_got_ack
    int retry_count = 0;
    have_got_ack = wxFalse;
    have_got_error = wxFalse;
    //st = time.monotonic()
    while (wxTrue)
    {
        std::vector<unsigned char> msg;
        (void) midi_in->getMessage(&msg);
        if (msg.size() > 0)
        {
            // print("<    " + bytes(msg[0]).hex(" "))
            parse_response(msg);
            
#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
            char dbg_2[500];
            int i = 0;
            strcpy(dbg_2, "< ");
            i = 2;
            int j;
            for (j = 0; j < msg.size(); j ++)
            {
                sprintf(&dbg_2[i], "%02X ", msg[j]);
                i += 3;
            }
            wxLogDebug(dbg_2);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE
            
            
            
            
        }
        if (have_got_ack)
        {
            // Success!
            return;
        }
        wxMilliSleep(20);
        retry_count += 1;
        if (retry_count > 200)
        {
            // Timed out
            return;
        }
        //if time.monotonic() > st + 4.0:
        //  # Clean up. We're exiting with an exception, but just in case a higher-
        //  # level process catches the exception we should have the port closed.
        //  #os.close(f)
        //  # Timed out. Completely exit the program
        //  raise self.SysexTimeoutError("SYSEX communication timed out. Exiting ...")
    }

}



std::vector<unsigned char> download_ac7_internal(int param_set, int memory, int category, bool _debug)
{
    RtMidiIn * midi_in = new RtMidiIn();

    int i_in;
    int j = midi_in->getPortCount();

    for (i_in = 0; i_in < j; i_in ++)
    {
        wxString s(midi_in->getPortName(i_in));
        if (s.compare(_midiComms._input_name) == 0)
        {
            break;
        }
    }
    
    if (i_in >= j)
    {
        // Did not find the port
        delete midi_in;
        return EMPTY_VEC;
    }



    RtMidiOut * midi_out = new RtMidiOut();

    int i_out;
    j = midi_out->getPortCount();

    for (i_out = 0; i_out < j; i_out ++)
    {
        wxString s(midi_out->getPortName(i_out));
        if (s.compare(_midiComms._output_name) == 0)
        {
            break;
        }
    }
    
    if (i_out >= j)
    {
        // Did not find the port
        delete midi_in;
        delete midi_out;
        return EMPTY_VEC;
    }
    
    
    midi_in->openPort(i_in);
    midi_out->openPort(i_out);


    midi_in->ignoreTypes(false, true, true);
    
    
    // Flush the input
    wxMilliSleep(40);
    std::vector<unsigned char> unused_pkt = std::vector<unsigned char>();
    midi_in->getMessage(&unused_pkt);

    int retry_count = 0;
    
    
    while (true)   // retry loop
    {
        retry_count ++;
        if (retry_count > 3)
        {
            break;
        }
    
    
    
        total_rxed.clear();


        // Send the SBS command

        {
        auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 8, 2);
#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
        char dbg_1 [500];
        int i = 0;
        strcpy(dbg_1, "> ");
        i = 2;
        int j1;
        for (j1 = 0; j1 < pkt.size(); j1 ++)
        {
            sprintf(&dbg_1[i], "%02X ", pkt[j1]);
            i += 3;
        }
        wxLogDebug(dbg_1);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE
        midi_out->sendMessage(&pkt);  // SBS(HBR)
        }

        wait_for_ack(midi_in);
        if (have_got_error)
            continue;


        {
        auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 4);
#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
        char dbg_1 [500];
        int i = 0;
        strcpy(dbg_1, "> ");
        i = 2;
        int j1;
        for (j1 = 0; j1 < pkt.size(); j1 ++)
        {
            sprintf(&dbg_1[i], "%02X ", pkt[j1]);
            i += 3;
        }
        wxLogDebug(dbg_1);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE
        midi_out->sendMessage(&pkt);  // HBR
        }


        have_got_ess = wxFalse;


        while(wxTrue)
        {

            wait_for_ack(midi_in);

            if (have_got_ess)
                break;
            
            
            {
            auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 0xa);
            midi_out->sendMessage(&pkt);
            }

        }

        // Send EBS (no ACK expected)
        {
        auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 0xe);
#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
        char dbg_1 [500];
        int i = 0;
        strcpy(dbg_1, "> ");
        i = 2;
        int j1;
        for (j1 = 0; j1 < pkt.size(); j1 ++)
        {
            sprintf(&dbg_1[i], "%02X ", pkt[j1]);
            i += 3;
        }
        wxLogDebug(dbg_1);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE
        midi_out->sendMessage(&pkt);
        wxMilliSleep(300);
        }

    } // retry loop

    midi_out->closePort();
    midi_in->closePort();

    delete midi_in;
    delete midi_out;
    
    return total_rxed;

}



void upload_ac7_internal(std::vector<unsigned char> data, int param_set, int memory, int category, bool _debug)
{
    RtMidiIn * midi_in = new RtMidiIn();

    int i_in;
    int j = midi_in->getPortCount();

    for (i_in = 0; i_in < j; i_in ++)
    {
        wxString s(midi_in->getPortName(i_in));
        if (s.compare(_midiComms._input_name) == 0)
        {
            break;
        }
    }
    
    if (i_in >= j)
    {
        // Did not find the port
        delete midi_in;
        return;
    }



    RtMidiOut * midi_out = new RtMidiOut();

    int i_out;
    j = midi_out->getPortCount();

    for (i_out = 0; i_out < j; i_out ++)
    {
        wxString s(midi_out->getPortName(i_out));
        if (s.compare(_midiComms._output_name) == 0)
        {
            break;
        }
    }
    
    if (i_out >= j)
    {
        // Did not find the port
        delete midi_in;
        delete midi_out;
        return;
    }
    
    
    midi_in->openPort(i_in);
    midi_out->openPort(i_out);


    midi_in->ignoreTypes(false, true, true);

    
    // Flush the input
    wxMilliSleep(40);
    std::vector<unsigned char> unused_pkt = std::vector<unsigned char>();
    midi_in->getMessage(&unused_pkt);


    {
    // Send the SBS command
    auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 8, 3);
    
#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
    char dbg_1 [500];
    int i = 0;
    strcpy(dbg_1, "> ");
    i = 2;
    int j1;
    for (j1 = 0; j1 < pkt.size(); j1 ++)
    {
        sprintf(&dbg_1[i], "%02X ", pkt[j1]);
        i += 3;
    }
    wxLogDebug(dbg_1);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE

    
    midi_out->sendMessage(&pkt);
    wait_for_ack(midi_in);
    }

    


    int i = 0;
    while (i < data.size())
    {
        // Send a HBS packet:
        // Category 30 = Rhythms
        // Parameter set: indicates the specific rhythm
        // Memory 1 = user rhythm space
        
        int len_remaining = data.size() - i;
        if (len_remaining > 0x80)
        {
            len_remaining = 0x80;
        }
        
        // Create a sub-vector. Should be a better way of doing it than this
        std::vector<unsigned char> sub_data = std::vector<unsigned char>();
        int j;
        for (j = 0; j < len_remaining; j ++)
        {
            sub_data.push_back(data[i + j]);
        }
        
        
        auto pkt = make_packet(wxFalse, sub_data, category, memory, param_set, EMPTY_BLOCKS, 0, 0, len_remaining, 5);

#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
        char dbg_2 [500];
        int i1 = 0;
        strcpy(dbg_2, "> ");
        i1 = 2;
        int j1;
        for (j1 = 0; j1 < pkt.size(); j1 ++)
        {
            sprintf(&dbg_2[i1], "%02X ", pkt[j1]);
            i1 += 3;
        }
        wxLogDebug(dbg_2);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE

        midi_out->sendMessage(&pkt);
        wait_for_ack(midi_in);
        i += len_remaining;
        
        
    }

    {
    // Sending ESS (no ACK expected)
    auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 0xD);

#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
    char dbg_3 [500];
    int i1 = 0;
    strcpy(dbg_3, "> ");
    i1 = 2;
    int j1;
    for (j1 = 0; j1 < pkt.size(); j1 ++)
    {
        sprintf(&dbg_3[i1], "%02X ", pkt[j1]);
        i1 += 3;
    }
    wxLogDebug(dbg_3);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE

    midi_out->sendMessage(&pkt);
    wxMilliSleep(300);
    }
    
    {
    // Sending EBS (no ACK expected)
    auto pkt = make_packet(wxFalse, EMPTY_VEC, category, memory, param_set, EMPTY_BLOCKS, 0, 0, 1, 0xE);

#ifdef MIDI_SYSEX_LOG_TO_CONSOLE
    char dbg_3 [500];
    int i1 = 0;
    strcpy(dbg_3, "> ");
    i1 = 2;
    int j1;
    for (j1 = 0; j1 < pkt.size(); j1 ++)
    {
        sprintf(&dbg_3[i1], "%02X ", pkt[j1]);
        i1 += 3;
    }
    wxLogDebug(dbg_3);
#endif // MIDI_SYSEX_LOG_TO_CONSOLE

    midi_out->sendMessage(&pkt);
    wxMilliSleep(300);
    }
    



    midi_out->closePort();
    midi_in->closePort();

    delete midi_in;
    delete midi_out;
    
    return;

}

