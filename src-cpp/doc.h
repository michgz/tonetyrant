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


protected:
    virtual bool DoOpenDocument(const wxString& file);

private:
    void DoUpdate();
    
    
    wxDECLARE_NO_COPY_CLASS(ToneDocument);
    wxDECLARE_DYNAMIC_CLASS(ToneDocument);

};


class HexEditCommand : public wxCommand
{
public:
    HexEditCommand(ToneDocument *doc,
                   const wxString& name,
                   const int info)
        : wxCommand(true, name)
    {
    }


public:


    virtual bool Do() { return true; }
    virtual bool Undo() { return true; }
};

#endif // _WX_SAMPLES_DOCVIEW_DOC_H_