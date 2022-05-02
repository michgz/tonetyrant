



#include "RtMidi.h"
#include <vector>


#define wxTrue true
#ifndef wxFalse
#define wxFalse false
#endif


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
    
    
std::vector<unsigned char> make_packet(bool tx,
                                        std::vector<unsigned char> data,
                                        unsigned short int category=30,
                                        unsigned short int memory=1,
                                        unsigned short int parameter_set=0,
                                        unsigned short int block[4] = (unsigned short int[]){0,0,0,0},
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
        //w += self.midi_8bit_to_7bit(data)
        //crc_val = binascii.crc32(w[1:])
        //w += self.midi_8bit_to_7bit(struct.pack('<I', crc_val))
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
        w.push_back((unsigned char) (length-1));
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



void set_single_parameter(int parameter, 
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
 //   midiout.send_message(bytearray(pkt))

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

}

