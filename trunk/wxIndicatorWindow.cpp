#include "wxIndicatorWindow.h"

//(*InternalHeaders(wxIndicatorWindow)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxIndicatorWindow)
const long wxIndicatorWindow::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxIndicatorWindow,wxDialog)
	//(*EventTable(wxIndicatorWindow)
	//*)
END_EVENT_TABLE()

wxIndicatorWindow::wxIndicatorWindow(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxIndicatorWindow)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(104,288), wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	//*)
}

wxIndicatorWindow::~wxIndicatorWindow()
{
	//(*Destroy(wxIndicatorWindow)
	//*)
}

