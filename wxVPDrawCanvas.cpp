#include "wxVPDrawCanvas.h"

//(*InternalHeaders(wxVPDrawCanvas)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include "wx/dcclient.h"

//(*IdInit(wxVPDrawCanvas)
const long wxVPDrawCanvas::ID_TEXTCTRL1 = wxNewId();
const long wxVPDrawCanvas::ID_BUTTON1 = wxNewId();
const long wxVPDrawCanvas::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxVPDrawCanvas,wxDialog)
	//(*EventTable(wxVPDrawCanvas)
	//*)

END_EVENT_TABLE()

wxVPDrawCanvas::wxVPDrawCanvas(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxVPDrawCanvas)
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("drawRootSystem"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this, ID_BUTTON1, _("Go"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("Pause"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer2->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&wxVPDrawCanvas::OnInit);
	Connect(wxEVT_PAINT,(wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1);
	Connect(wxEVT_MOTION,(wxObjectEventFunction)&wxVPDrawCanvas::OnMouseMove);
	//*)

  this->Connect(this->GetId(), wxEVT_PAINT, (wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);
  /*this->Connect(ID_PANEL1, wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove));
  this->Connect(this->GetId(), wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove));
  this->CaptureMouse();.*/
//  this->Connect(ID_PANEL1, wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove), 0, this);
/*	Panel1->Connect(ID_PANEL1, wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove));
	Panel1->Connect(ID_PANEL1, wxEVT_KEY_DOWN, wxMouseEventHandler(wxVPDrawCanvas::onMouseDownOnCanvas));
	Panel1->Connect(ID_PANEL1, wxEVT_LEFT_UP, wxMouseEventHandler(wxVPDrawCanvas::onMouseUpOnCanvas));
  Panel1->Connect(ID_PANEL1, wxEVT_PAINT, (wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);
	Panel1->Connect(wxID_ANY, wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove));
	Panel1->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxMouseEventHandler(wxVPDrawCanvas::onMouseDownOnCanvas));
	Panel1->Connect(wxID_ANY, wxEVT_LEFT_UP, wxMouseEventHandler(wxVPDrawCanvas::onMouseUpOnCanvas));
  Panel1->Connect(wxID_ANY, wxEVT_PAINT, (wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);

	this->Connect(this->GetId(), wxEVT_KEY_DOWN, wxMouseEventHandler(wxVPDrawCanvas::onMouseDownOnCanvas));
	this->Connect(this->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler(wxVPDrawCanvas::onMouseUpOnCanvas));

	this->Connect(ID_PANEL1, wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove));
	this->Connect(ID_PANEL1, wxEVT_KEY_DOWN, wxMouseEventHandler(wxVPDrawCanvas::onMouseDownOnCanvas));
	this->Connect(ID_PANEL1, wxEVT_LEFT_UP, wxMouseEventHandler(wxVPDrawCanvas::onMouseUpOnCanvas));
  this->Connect(ID_PANEL1, wxEVT_PAINT, (wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);
	this->Connect(wxID_ANY, wxEVT_MOTION, wxMouseEventHandler(wxVPDrawCanvas::onMouseMove));
	this->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxMouseEventHandler(wxVPDrawCanvas::onMouseDownOnCanvas));
	this->Connect(wxID_ANY, wxEVT_LEFT_UP, wxMouseEventHandler(wxVPDrawCanvas::onMouseUpOnCanvas));
  this->Connect(wxID_ANY, wxEVT_PAINT, (wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);
  this->Connect(wxEVT_PAINT, (wxObjectEventFunction)&wxVPDrawCanvas::OnPanel1Paint1,0,this);*/

}

wxVPDrawCanvas::~wxVPDrawCanvas()
{
	//(*Destroy(wxVPDrawCanvas)
	//*)
}

void wxVPDrawCanvas::OnInit(wxInitDialogEvent& event)
{
}

