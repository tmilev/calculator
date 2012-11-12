#include "wxStatus.h"

//(*InternalHeaders(wxStatus)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(wxStatus)
const long wxStatus::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxStatus,wxFrame)
	//(*EventTable(wxStatus)
	//*)
END_EVENT_TABLE()

wxStatus::wxStatus(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxStatus)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	TextCtrlProgressString = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(152,312), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	//*)
}

wxStatus::~wxStatus()
{
	//(*Destroy(wxStatus)
	//*)
}

