#include "wxPNGdispaly.h"

//(*InternalHeaders(wxPNGdispaly)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxPNGdispaly)
//*)

BEGIN_EVENT_TABLE(wxPNGdispaly,wxFrame)
	//(*EventTable(wxPNGdispaly)
	//*)
END_EVENT_TABLE()

wxPNGdispaly::wxPNGdispaly(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxPNGdispaly)
	wxBoxSizer* BoxSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxPNGdispaly::OnPaint);
	//*)
}

wxPNGdispaly::~wxPNGdispaly()
{
	//(*Destroy(wxPNGdispaly)
	//*)
}



