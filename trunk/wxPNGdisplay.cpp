#include "wxPNGdisplay.h"

//(*InternalHeaders(wxPNGdisplay)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxPNGdisplay)
//*)

BEGIN_EVENT_TABLE(wxPNGdisplay,wxFrame)
	//(*EventTable(wxPNGdisplay)
	//*)
END_EVENT_TABLE()

wxPNGdisplay::wxPNGdisplay(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxPNGdisplay)
	wxBoxSizer* BoxSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxPNGdisplay::OnPaint);
	//*)
}

wxPNGdisplay::~wxPNGdisplay()
{
	//(*Destroy(wxPNGdisplay)
	//*)
}

