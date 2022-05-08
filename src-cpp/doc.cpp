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
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff?\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00d"  \
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x01"            \
      "\x00\xc00\x08\x00\xc00\x08\x00\x85\x08CalSine         d\x7f\x02\x02\x02\x7f\x02\x7f\x02\x7f\x00\x00\x7f\x02\x02\x00\x00\x00EODA";

    int i;
    
    for (i = 0; i < sizeof(CALSINE); i ++)
    {
        push_back(CALSINE[i]);
    }




}


void ToneDocument::DoUpdate()
{
    // Calculate the CRC. Don't have the __setitem__() method to do that now.
    
    
    unsigned int c = crc32_fast(this->data(), 0x1C4);
    
    this->at(0x1C) = ((unsigned char *)&c)[0];
    this->at(0x1D) = ((unsigned char *)&c)[1];
    this->at(0x1E) = ((unsigned char *)&c)[2];
    this->at(0x1F) = ((unsigned char *)&c)[3];
    
    Modify(true);
    UpdateAllViews();
}


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


int ToneDocument::GetParamFrom(PP_ID PP)
{
    if (Parameters[PP].number == 0)
    {
        /*offset_ = P.byteOffset + 0x20;
        STR_ = self._data[offset_:offset_+16].decode('ascii');*/
        //return STR_;
        return 0;
    }
  
    if (Parameters[PP].number == 84)
    {
        /*offset_ = P.byteOffset + 0x20
        STR_ = self._data[offset_:offset_+16].decode('ascii')
        return STR_*/
    }
  
    if (Parameters[PP].number == 87)
    {
        /*offset_ = P.byteOffset + 0x20
        STR_ = self._data[offset_:offset_+14].hex(" ").upper()
        return STR_*/
        
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
        
    if (offset ^ 1 == 0)
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
    res->_old_vals = old_vals;
    res->_new_vals = new_vals;

    return res;
}

bool HexEditCommand::Do(void)
{
    
    
    /*
    if (_type == 1)  // Nibble
    {
        unsigned char x;
        if (_offset ^ 1 == 0)
        {
            x = _document->at(_offset/2) & 0x0F;
            x = x + (_new_nibble << 4);
            _document->at(_offset/2) = x;
        }
        else
        {
            x = _document->at(_offset/2) & 0xF0;
            x = x + (_new_nibble << 0);
            _document->at(_offset/2) = x;
        }
        _document->DoUpdate();

        return wxTrue;
    }
    else if (_type == 2)   // Byte
    {
        _document->at(_offset/2) = _new_byte;
        _document->DoUpdate();
        return wxTrue;
    }
    else if (_type == 3)   // Complete buffer overwrite
    {
        int i;
        for (i = 0x20; i < _document->size()-4; i ++)
        {
            _document->at(i) = _new_vals.at(i - 0x20);
        }
        _document->DoUpdate();

        return wxTrue;
    }
    return wxFalse;
    * */return true;
}
