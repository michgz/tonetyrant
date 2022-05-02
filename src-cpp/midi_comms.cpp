



#include "RtMidi.h"
#include <vector>



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





