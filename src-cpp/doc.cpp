/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.cpp
// Purpose:     Implements document functionality
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/wfstream.h"

#include "doc.h"
#include "view.h"
#include "Crc32.h"

#include "parameters.h"

#define wxTrue true

IMPLEMENT_DYNAMIC_CLASS(ToneDocument, wxDocument)

ToneDocument::ToneDocument(void) : wxDocument()
{
    clear();


    // A "default" data array to use as a starting point. It's a sine-wave sound
    unsigned char CALSINE [] =  \
      "CT-X3000\x00\x00\x00\x00\x00\x00\x00\x00TONH\x00\x00\x00\x00\xfe:R\x1a\xc8\x01\x00\x00"                              \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x80\x80\x80\x80\x7f\x00\x01\x00\x7f\x7f\x7f\x00"                    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00"    \
      "\x00\x02\x80\x00\x00\x02\x80\x00\x00\x02\x80\x00\x80\x80\x80\x80\x7f\x00\x01\x00\x7f\x7f\x7f\x00"                    \
      "\x00\x00\x00@@@@HH@@@@@@@@@@@\xff\x00                \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"   \
      "\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00"  \
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"   \
      "d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x01\x00"       \
      "\xc0" "0\x08\x00\xc0" "0\x08\x00\x85\x08" "CalSine         d\x7f\x02\x02\x02\x7f\x02\x7f\x02\x7f\x00\x00\x7f"        \
      "\x02\x02\x00\x00\x00" "EODA";

    int i;
    
    for (i = 0; i < sizeof(CALSINE)-1; i ++)  // -1 for the null termination
    {
        push_back(CALSINE[i]);
    }

    /* Random seed, for each document */
    srand( time(NULL) );



}


void ToneDocument::DoUpdate()
{
    // Calculate the CRC. Don't have the __setitem__() method to do that now.
    
    
    unsigned int crc = crc32_fast(&this->data()[0x20], 0x1C8);
    
    this->at(0x18) = ((unsigned char *)&crc)[0];
    this->at(0x19) = ((unsigned char *)&crc)[1];
    this->at(0x1A) = ((unsigned char *)&crc)[2];
    this->at(0x1B) = ((unsigned char *)&crc)[3];
    
    Modify(true);
    UpdateAllViews();
}

void ToneDocument::Modify(bool x)
{
    wxDocument::Modify(x);
}
 
extern void midi_comms_set_param(PP_ID P, int p_val);
 
bool ToneDocument::DoOpenDocument(const wxString& file)
{
    return true;//m_image.LoadFile(file);
}

bool ToneDocument::OnOpenDocument(const wxString& filename)
{
    if ( !wxDocument::OnOpenDocument(filename) )
        return false;

    // Do something

    return true;
}

extern void midi_comms_set_param(PP_ID P, int p_val);

void ToneDocument::InformByteChanged(int offset, unsigned char new_val, unsigned char old_val)
{
    PP_ID PP;
    
    for (PP = 0; PP < sizeof(Parameters)/sizeof(Parameters[0]); PP ++)
    {
        if ((Parameters[PP].byteOffset + 0x20 <= offset) && (Parameters[PP].byteOffset + 0x20 + Parameters[PP].byteCount > offset))
        {
            std::cout << "U" << std::endl;
            if (Parameters[PP].bitOffset + Parameters[PP].bitCount <= 8)
            {
                // Fits into a byte

                std::cout << "V" << std::endl;

                unsigned char a = (new_val << Parameters[PP].bitOffset) & ((1U >> Parameters[PP].bitCount) - 1);
                unsigned char b = (old_val << Parameters[PP].bitOffset) & ((1U >> Parameters[PP].bitCount) - 1);
                std::cout << static_cast<int>(a) << "," << static_cast<int>(b) << std::endl;
                if (a != b)
                {
                    midi_comms_set_param(PP, a);
                }
            }
            else
            {
                // Needs full 32 bits

                std::cout << "W" << std::endl;


                unsigned long int X = *((unsigned long int *) &this->data()[Parameters[PP].byteOffset + 0x20]);
                unsigned long int MASK = ((1ULL >> Parameters[PP].bitCount) - 1) >> Parameters[PP].bitOffset;

                midi_comms_set_param(PP, (X & MASK) << Parameters[PP].bitOffset);
            }
        }
        
    }
    
    
}


void ToneDocument::SetParamTo(PP_ID PP, unsigned int p_val)
{
    
    // "1ULL" to allow for 32-bit parameters.
    if (p_val >= (1ULL << Parameters[PP].bitCount))
    {
        wxLogError("Trying to set value %d to a field with only %d bits", p_val, Parameters[PP].bitCount);
        return;
    }
            
    unsigned long int X = *((unsigned long int *) &this->data()[Parameters[PP].byteOffset + 0x20]);
    unsigned long int MASK = ((1ULL >> Parameters[PP].bitCount) - 1) >> Parameters[PP].bitOffset;
        
    X = X & ~MASK;
    X = X | (p_val >> Parameters[PP].bitOffset);

    
    std::vector<unsigned char> altered_ = std::vector<unsigned char>(*this);
    
    
    
    //*((unsigned long int *) &this->data()[Parameters[PP].byteOffset + 0x20]) = X;
    altered_.at(Parameters[PP].byteOffset + 0x20) = ((unsigned char *) &X)[0];
    altered_.at(Parameters[PP].byteOffset + 0x21) = ((unsigned char *) &X)[1];
    altered_.at(Parameters[PP].byteOffset + 0x22) = ((unsigned char *) &X)[2];
    altered_.at(Parameters[PP].byteOffset + 0x23) = ((unsigned char *) &X)[3];
        
    
    HexEditCommand * cmd_ = HexEditCommand::CompletelyChange(this, *this, altered_);
    this->GetCommandProcessor()->Submit(cmd_);
    //self._docManager.SetParamTo(P, p_val)


std::cout << "A" << std::endl;
midi_comms_set_param(PP, p_val);

}

void ToneDocument::OnSetToRandomise(bool include_wavetable)
{
    std::vector<unsigned char> altered_ = std::vector<unsigned char>(*this);
    
    PP_ID PP;
    for (PP = 0; PP <  sizeof(Parameters)/sizeof(Parameters[0]); PP ++)
    {
        int number_ = Parameters[PP].number;

            /*
            Exclude some parameters:
                 - Volumes are excluded, since their behaviour is known and not interesting
                 - String parameters are excluded
                 - Param 109 seems to have a permanent effect, so exclude it to avoid
                     that happening.
                 - Filters. It's possible for these to take on values that result in no sound.
                 - Gain-related key follows. These can cause apparent silence.
            */
        if (number_ == 0
                || number_ == 84
                || number_ == 87
                
                || number_ == 45
                || number_ == 46
                || number_ == 109
                || number_ == 200
                || number_ == 115
                || number_ == 41
                
                || number_ == 1
                || number_ == 21
                
                || number_ == 117
                || number_ == 118
                || number_ == 119
                || number_ == 120
                || number_ == 121
                || number_ == 122

                || number_ == 47
                || number_ == 48
                || number_ == 201
                || number_ == 202
                )
        {
            // No action
        }
        else if ((number_ == 2 || number_ == 22) && !include_wavetable)
        {
            // No action
        }
        else
        {
            unsigned long int Y = (rand() % (Parameters[PP].recommendedLimits[1] - Parameters[PP].recommendedLimits[0] + 1)) + Parameters[PP].recommendedLimits[0];
            
            unsigned long int X = *((unsigned long int *) &altered_.data()[Parameters[PP].byteOffset + 0x20]);
            unsigned long int MASK = ((1ULL << Parameters[PP].bitCount) - 1) << Parameters[PP].bitOffset;
                
            X = X & ~MASK;
            X = X | (Y << Parameters[PP].bitOffset);
            
            //*((unsigned long int *) &this->data()[Parameters[PP].byteOffset + 0x20]) = X;
            altered_.at(Parameters[PP].byteOffset + 0x20) = ((unsigned char *) &X)[0];
            altered_.at(Parameters[PP].byteOffset + 0x21) = ((unsigned char *) &X)[1];
            altered_.at(Parameters[PP].byteOffset + 0x22) = ((unsigned char *) &X)[2];
            altered_.at(Parameters[PP].byteOffset + 0x23) = ((unsigned char *) &X)[3];
        }
    }
    
    HexEditCommand * cmd_ = HexEditCommand::CompletelyChange(this, *this, altered_);
    this->GetCommandProcessor()->Submit(cmd_);
}

void ToneDocument::OnSetToDefault(bool include_wavetable)
{
    std::vector<unsigned char> altered_ = std::vector<unsigned char>(*this);
    
    PP_ID PP;
    for (PP = 0; PP <  sizeof(Parameters)/sizeof(Parameters[0]); PP ++)
    {
        int number_ = Parameters[PP].number;

        if (number_ == 0 || number_ == 84 || number_ == 87 || number_ == 41 || number_ == 115)
        {
            // No action
        }
        else if ((number_ == 1 || number_ == 2 || number_ == 21 || number_ == 22) && !include_wavetable)
        {
            // No action
        }
        else
        {
            unsigned long int Y = Parameters[PP].defaultValue;
            
            unsigned long int X = *((unsigned long int *) &altered_.data()[Parameters[PP].byteOffset + 0x20]);
            unsigned long int MASK = ((1ULL << Parameters[PP].bitCount) - 1) << Parameters[PP].bitOffset;
                
            X = X & ~MASK;
            X = X | (Y << Parameters[PP].bitOffset);
            
            //*((unsigned long int *) &this->data()[Parameters[PP].byteOffset + 0x20]) = X;
            altered_.at(Parameters[PP].byteOffset + 0x20) = ((unsigned char *) &X)[0];
            altered_.at(Parameters[PP].byteOffset + 0x21) = ((unsigned char *) &X)[1];
            altered_.at(Parameters[PP].byteOffset + 0x22) = ((unsigned char *) &X)[2];
            altered_.at(Parameters[PP].byteOffset + 0x23) = ((unsigned char *) &X)[3];
        }
    }
    
    HexEditCommand * cmd_ = HexEditCommand::CompletelyChange(this, *this, altered_);
    this->GetCommandProcessor()->Submit(cmd_);
}



void ToneDocument::SetParamTo(PP_ID PP, wxString p_val)
{
    int number_ = Parameters[PP].number;

    std::vector<unsigned char> altered_ = std::vector<unsigned char>(*this);

    if (number_ == 0 || number_ == 84)
    {
        int offset_ = Parameters[PP].byteOffset + 0x20;
        int i;
        for (i = 0; i < 16 && i < p_val.Length(); i ++)
        {
            if (p_val[i] == '\0')
            {
                altered_.at(offset_ + i) = ' ';
            }
            else
            {
                altered_.at(offset_ + i) = p_val[i];
            }
        }
        for (; i < 16; i ++)
        {
            altered_.at(offset_ + i) = ' ';
        }
    }
    else if (number_ == 87)
    {
        // DSP parameters. For now regard as text, although that doesn't make
        // a lot of sense given the nature of the data.
        int offset_ = Parameters[PP].byteOffset + 0x20;
        int i;
        for (i = 0; i < 14 && i < p_val.Length(); i ++)
        {
            if (p_val[i] == '\0')
            {
                altered_.at(offset_ + i) = ' ';
            }
            else
            {
                altered_.at(offset_ + i) = p_val[i];
            }
        }
        for (; i < 14; i ++)
        {
            altered_.at(offset_ + i) = ' ';
        }
    }
    else
    {
        wxLogError("Not a string parameter: parameter %d", number_);
        return;
    }

    HexEditCommand * cmd_ = HexEditCommand::CompletelyChange(this, *this, altered_);
    this->GetCommandProcessor()->Submit(cmd_);
    //self._docManager.SetParamTo(P, p_val)


    
    
}


wxString ToneDocument::GetParamFromStr(PP_ID PP)
{
    if (Parameters[PP].number == 0)
    {
        return wxString::FromAscii(&this->data()[Parameters[PP].byteOffset + 0x20], 16);
    }
  
    if (Parameters[PP].number == 84)
    {
        return wxString::FromAscii(&this->data()[Parameters[PP].byteOffset + 0x20], 16);
    }
  
    if (Parameters[PP].number == 87)
    {
        return wxString::FromAscii(&this->data()[Parameters[PP].byteOffset + 0x20], 14);
    }
    
    return ""; // Error
    
    
}


int ToneDocument::GetParamFrom(PP_ID PP)
{
    if (Parameters[PP].number == 0)
    {
        /*offset_ = P.byteOffset + 0x20;
        STR_ = self._data[offset_:offset_+16].decode('ascii');*/
        //return STR_;
        return 0;  // Error!
    }
  
    if (Parameters[PP].number == 84)
    {
        /*offset_ = P.byteOffset + 0x20
        STR_ = self._data[offset_:offset_+16].decode('ascii')
        return STR_*/
        return 0;  // Error!
    }
  
    if (Parameters[PP].number == 87)
    {
        /*offset_ = P.byteOffset + 0x20
        STR_ = self._data[offset_:offset_+14].hex(" ").upper()
        return STR_*/
        return 0;  // Error!
    }
      
  
  
    unsigned long int X = *((unsigned long int *) &this->data()[Parameters[PP].byteOffset + 0x20]);
    
    
    X = X >> (Parameters[PP].bitOffset);
    X = X & ((1 << Parameters[PP].bitCount) - 1);
    
    return X;
    
}


HexEditCommand * HexEditCommand::ChangeNibble(ToneDocument * document, int offset, int new_nibble)
{
    HexEditCommand * res = new HexEditCommand(document, "", 0, wxTrue);
    
    res->_type = 1;
    res->_new_nibble = new_nibble;
    res->_document = document;
    res->_offset = offset;
        
    if ((offset & 1) == 0)
        res->_old_nibble = ((document->at(offset/2)) & 0xF0) >> 4;
    else
        res->_old_nibble = document->at(offset/2) & 0x0F;

    return res;
}

HexEditCommand * HexEditCommand::ChangeByte(ToneDocument * document, int offset, int new_byte)
{
    HexEditCommand * res = new HexEditCommand(document, "", 0, wxTrue);
    
    res->_type = 2;
    res->_new_byte = new_byte;
    res->_document = document;
    res->_offset = offset;
        
    res->_old_byte = document->at(offset/2);

    return res;
}

HexEditCommand * HexEditCommand::CompletelyChange(ToneDocument * document, std::vector<unsigned char> old_vals, std::vector<unsigned char> new_vals)
{
    HexEditCommand * res = new HexEditCommand(document, "", 0, wxTrue);
    
    res->_type = 3;
    res->_document = document;
    std::vector<unsigned char>::const_iterator first = old_vals.begin() + 0x20;
    std::vector<unsigned char>::const_iterator last = old_vals.begin() + 0x1E8;
    //vector<T> newVec(first, last);
    res->_old_vals = std::vector<unsigned char>(first, last);
    res->_new_vals = new_vals;

    return res;
}

bool HexEditCommand::Do(void)
{
    if (_type == 1)  // Nibble
    {
        unsigned char x;
        unsigned char y;
        unsigned char old_nibble;

        _document->change_list.clear();

        if ((_offset & 1) == 0)
        {
            y = _document->at(_offset/2);
            old_nibble = (y & 0xF0) >> 4;
            x = y & 0x0F;
            x = x + (_new_nibble << 4);
            _document->at(_offset/2) = x;
        }
        else
        {
            y = _document->at(_offset/2);
            old_nibble = y & 0x0F;
            x = y & 0xF0;
            x = x + (_new_nibble << 0);
            _document->at(_offset/2) = x;
        }
        
        if (_new_nibble != old_nibble)
        {
            _document->change_list.push_back(_offset/2);
            if ((_offset/2) >= 0x20 && (_offset/2) < 0x1E8)
            {
                // Assume every byte of the CRC has changed
                _document->change_list.push_back(0x18);
                _document->change_list.push_back(0x19);
                _document->change_list.push_back(0x1A);
                _document->change_list.push_back(0x1B);
            }
            
            _document->InformByteChanged(_offset/2, x, y);
        }

        _document->DoUpdate();

        return wxTrue;
    }
    else if (_type == 2)   // Byte
    {
        unsigned char old_byte;
        _document->change_list.clear();
        old_byte = _document->at(_offset/2);
        _document->at(_offset/2) = _new_byte;
        if (_new_byte != old_byte)
        {
            _document->change_list.push_back(_offset/2);
            if ((_offset/2) >= 0x20 && (_offset/2) < 0x1E8)
            {
                // Assume every byte of the CRC has changed
                _document->change_list.push_back(0x18);
                _document->change_list.push_back(0x19);
                _document->change_list.push_back(0x1A);
                _document->change_list.push_back(0x1B);
            }
            _document->InformByteChanged(_offset/2, _new_byte, old_byte);
        }
        _document->DoUpdate();
        return wxTrue;
    }
    else if (_type == 3)   // Complete buffer overwrite
    {
        int i;
        bool has_changed = false;
        _document->change_list.clear();
        for (i = 0x20; i < _document->size()-4; i ++)
        {
            if (_document->at(i) != _new_vals.at(i))
            {
                _document->at(i) = _new_vals.at(i);
                has_changed = true;
                _document->change_list.push_back(i);
            }   
        }
        if (has_changed)
        {
            // Assume every byte of the CRC has changed
            _document->change_list.push_back(0x18);
            _document->change_list.push_back(0x19);
            _document->change_list.push_back(0x1A);
            _document->change_list.push_back(0x1B);
        }
        
        _document->DoUpdate();

        return wxTrue;
    }
    return wxFalse;
}

bool HexEditCommand::Undo(void)
{
    if (_type == 1)  // Nibble
    {
        unsigned char x;
        unsigned char y;
        unsigned char now_nibble;

        _document->change_list.clear();
        if ((_offset & 1) == 0)
        {
            y = _document->at(_offset/2);
            now_nibble = (y & 0xF0) >> 4;
            x = y & 0x0F;
            x = x + (_old_nibble << 4);
            _document->at(_offset/2) = x;
        }
        else
        {
            y = _document->at(_offset/2);
            now_nibble = y & 0x0F;
            x = y & 0xF0;
            x = x + (_old_nibble << 0);
            _document->at(_offset/2) = x;
        }

        if (_new_nibble != now_nibble)
        {
            _document->change_list.push_back(_offset/2);
            if ((_offset/2) >= 0x20 && (_offset/2) < 0x1E8)
            {
                // Assume every byte of the CRC has changed
                _document->change_list.push_back(0x18);
                _document->change_list.push_back(0x19);
                _document->change_list.push_back(0x1A);
                _document->change_list.push_back(0x1B);
            }
            _document->InformByteChanged(_offset/2, x, y);
        }

        _document->DoUpdate();

        return wxTrue;
    }
    else if (_type == 2)   // Byte
    {
        unsigned char now_byte;
        now_byte = _document->at(_offset/2);
        _document->at(_offset/2) = _old_byte;
        if (_new_byte != now_byte)
        {
            _document->change_list.push_back(_offset/2);
            if ((_offset/2) >= 0x20 && (_offset/2) < 0x1E8)
            {
                // Assume every byte of the CRC has changed
                _document->change_list.push_back(0x18);
                _document->change_list.push_back(0x19);
                _document->change_list.push_back(0x1A);
                _document->change_list.push_back(0x1B);
            }
            _document->InformByteChanged(_offset/2, _new_byte, now_byte);
        }
        
        _document->DoUpdate();
        return wxTrue;
    }
    else if (_type == 3)   // Complete buffer overwrite
    {
        bool has_changed = false;
        int i;
        _document->change_list.clear();
        for (i = 0x20; i < _document->size()-4; i ++)
        {
            if (_document->at(i) != _old_vals.at(i))
            {
                _document->at(i) = _old_vals.at(i);
                has_changed = true;
                _document->change_list.push_back(i);
            }   
        }
        if (has_changed)
        {
            // Assume every byte of the CRC has changed
            _document->change_list.push_back(0x18);
            _document->change_list.push_back(0x19);
            _document->change_list.push_back(0x1A);
            _document->change_list.push_back(0x1B);
        }

        _document->DoUpdate();

        return wxTrue;
    }
    return wxFalse;
}





