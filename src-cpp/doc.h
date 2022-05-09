/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_DOCVIEW_DOC_H_
#define _WX_SAMPLES_DOCVIEW_DOC_H_

#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/vector.h"
#include "wx/image.h"

#include <list>
#include <vector>

// This sample is written to build both with wxUSE_STD_IOSTREAM==0 and 1, which
// somewhat complicates its code but is necessary in order to support building
// it under all platforms and in all build configurations
//
// In your own code you would normally use std::stream classes only and so
// wouldn't need these typedefs
#if wxUSE_STD_IOSTREAM
    typedef wxSTD istream DocumentIstream;
    typedef wxSTD ostream DocumentOstream;
#else // !wxUSE_STD_IOSTREAM
    typedef wxInputStream DocumentIstream;
    typedef wxOutputStream DocumentOstream;
#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM

typedef int PP_ID;

// ----------------------------------------------------------------------------
// The document class and its helpers
// ----------------------------------------------------------------------------

class ToneDocument : public wxDocument, public std::vector<unsigned char> {

public:
 //  std::list<unsigned char> _data;
 //   std::list<void *> _current_changelist;
 //   
 //   ToneDocumentManager *_docManager;
    
    ToneDocument(void);


    virtual bool OnOpenDocument(const wxString& file);
    
    
    int GetParamFrom(PP_ID PP);
    wxString GetParamFromStr(PP_ID PP);
    void SetParamTo(PP_ID PP, unsigned int p_val);
    void SetParamTo(PP_ID PP, wxString p_val);

virtual void Modify(bool );

protected:
    virtual bool DoOpenDocument(const wxString& file);

private:
public:
    void DoUpdate();
    
    
    wxDECLARE_NO_COPY_CLASS(ToneDocument);
    wxDECLARE_DYNAMIC_CLASS(ToneDocument);

};


class HexEditCommand : public wxCommand
{
public:
    HexEditCommand(ToneDocument *doc,
                   const wxString& name,
                   const int info,
                   bool canUndo)
        : wxCommand(true, name)
    {
    }
    
private:
    int _type;
    int _new_nibble, _old_nibble;
    int _new_byte, _old_byte;
    ToneDocument * _document;
    int _offset;
    std::vector<unsigned char> _old_vals;
    std::vector<unsigned char> _new_vals;

public:

    static HexEditCommand * ChangeNibble(ToneDocument * document, int offset, int new_nibble);
    static HexEditCommand * ChangeByte(ToneDocument * document, int offset, int new_byte);
    static HexEditCommand * CompletelyChange(ToneDocument * document, std::vector<unsigned char> old_vals, std::vector<unsigned char> new_vals);

    virtual bool Do();
    virtual bool Undo();
};

#endif // _WX_SAMPLES_DOCVIEW_DOC_H_
