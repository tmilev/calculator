#include "wxVPDrawCanvas.h"

//(*InternalHeaders(wxVPDrawCanvas)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include "wx/dcclient.h"

//(*IdInit(wxVPDrawCanvas)
//*)

BEGIN_EVENT_TABLE(wxVPDrawCanvas,wxDialog)
	//(*EventTable(wxVPDrawCanvas)
	//*)
END_EVENT_TABLE()

wxVPDrawCanvas::wxVPDrawCanvas(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxVPDrawCanvas)
	wxBoxSizer* BoxSizer1;
	
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&wxVPDrawCanvas::OnInit);
	//*)
	this->Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);
}

wxVPDrawCanvas::~wxVPDrawCanvas()
{
	//(*Destroy(wxVPDrawCanvas)
	//*)
}
void wxVPDrawCanvas::OnInit(wxInitDialogEvent& event)
{
}

