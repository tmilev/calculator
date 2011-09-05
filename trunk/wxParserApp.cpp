/***************************************************************
 * Name:      wxParserApp.cpp
 * Purpose:   Code for Application Class
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-05
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
    	wxParserDialog Dlg(0);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }
    //*)
    return wxsOK;

}
