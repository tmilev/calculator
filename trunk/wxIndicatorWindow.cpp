#include "wxIndicatorWindow.h"

//(*InternalHeaders(wxIndicatorWindow)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxIndicatorWindow)
const long wxIndicatorWindow::ID_TEXTCTRL2 = wxNewId();
const long wxIndicatorWindow::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxIndicatorWindow,wxDialog)
	//(*EventTable(wxIndicatorWindow)
	//*)
END_EVENT_TABLE()

wxIndicatorWindow::wxIndicatorWindow(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxIndicatorWindow)
	wxBoxSizer* BoxSizer1;
	
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer1->Add(TextCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(TextCtrl1, 3, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

wxIndicatorWindow::~wxIndicatorWindow()
{
	//(*Destroy(wxIndicatorWindow)
	//*)
}

