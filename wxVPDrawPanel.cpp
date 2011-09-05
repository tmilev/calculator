#include "wxVPDrawPanel.h"

//(*InternalHeaders(wxVPDrawPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxVPDrawPanel)
const long wxVPDrawPanel::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxVPDrawPanel,wxDialog)
	//(*EventTable(wxVPDrawPanel)
	//*)
END_EVENT_TABLE()

wxVPDrawPanel::wxVPDrawPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxVPDrawPanel)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(120,240), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	//*)
}

wxVPDrawPanel::~wxVPDrawPanel()
{
	//(*Destroy(wxVPDrawPanel)
	//*)
}

