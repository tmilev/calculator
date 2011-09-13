#include "wxDrawPanel.h"


//(*InternalHeaders(wxDrawPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxDrawPanel)
const long wxDrawPanel::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxDrawPanel,wxFrame)
	//(*EventTable(wxDrawPanel)
	//*)
END_EVENT_TABLE()


wxDrawPanel::wxDrawPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxDrawPanel)
	wxBoxSizer* BoxSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	BoxSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Panel1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxDrawPanel::OnPaint,0,this);
	Panel1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&wxDrawPanel::OnPanel1LeftDown,0,this);
	Panel1->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&wxDrawPanel::OnPanel1LeftUp,0,this);
	Panel1->Connect(wxEVT_MOTION,(wxObjectEventFunction)&wxDrawPanel::OnPanel1MouseMove,0,this);
	Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxDrawPanel::OnPaint);
	//*)
}

wxDrawPanel::~wxDrawPanel()
{
	//(*Destroy(wxDrawPanel)
	//*)
}


