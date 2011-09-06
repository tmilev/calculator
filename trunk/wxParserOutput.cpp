#include "wxParserOutput.h"

//(*InternalHeaders(wxParserOutput)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxParserOutput)
const long wxParserOutput::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxParserOutput,wxFrame)
	//(*EventTable(wxParserOutput)
	//*)
END_EVENT_TABLE()

wxParserOutput::wxParserOutput(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(wxParserOutput)
	wxBoxSizer* BoxSizer1;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

wxParserOutput::~wxParserOutput()
{
	//(*Destroy(wxParserOutput)
	//*)
}

