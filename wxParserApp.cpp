/***************************************************************
 * Name:      wxParserApp.cpp
 * Purpose:   Code for Application Class
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-13
 * Copyright: todor milev ()
 * License:
 **************************************************************/

#include "wxParserApp.h"

//(*AppHeaders
#include "wxParserMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(wxParserApp);

bool wxParserApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	wxParserFrame* Frame = new wxParserFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
