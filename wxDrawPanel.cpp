#include "wxDrawPanel.h"


//(*InternalHeaders(wxDrawPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxDrawPanel)
//*)

BEGIN_EVENT_TABLE(wxDrawPanel,wxFrame)
	//(*EventTable(wxDrawPanel)
	//*)
END_EVENT_TABLE()


wxDrawPanel::wxDrawPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxDrawPanel)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(255,255,255));

	Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxDrawPanel::OnPaint);
	Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&wxDrawPanel::OnPanel1LeftDown);
	Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&wxDrawPanel::OnPanel1LeftUp);
	Connect(wxEVT_MOTION,(wxObjectEventFunction)&wxDrawPanel::OnPanel1MouseMove);
	Connect(wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&wxDrawPanel::OnPanel1MouseWheel);
	//*)
}

wxDrawPanel::~wxDrawPanel()
{
	//(*Destroy(wxDrawPanel)
	//*)
}
