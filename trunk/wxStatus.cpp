#include "wxStatus.h"

//(*InternalHeaders(wxStatus)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxStatus)
const long wxStatus::ID_TEXTCTRL1 = wxNewId();
const long wxStatus::ID_TEXTCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxStatus,wxFrame)
	//(*EventTable(wxStatus)
	//*)
END_EVENT_TABLE()

wxStatus::wxStatus(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxStatus)
	wxBoxSizer* BoxSizer1;
	
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(TextCtrl1, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer1->Add(TextCtrl2, 5, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

wxStatus::~wxStatus()
{
	//(*Destroy(wxStatus)
	//*)
}

