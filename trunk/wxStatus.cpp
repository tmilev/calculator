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
	
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(255,255,255));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	TextCtrlProgressString = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(TextCtrlProgressString, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlStatusString = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer1->Add(TextCtrlStatusString, 5, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

