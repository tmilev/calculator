#ifndef WXVPDRAWPANEL_H
#define WXVPDRAWPANEL_H

//(*Headers(wxVPDrawPanel)
#include <wx/panel.h>
#include <wx/dialog.h>
//*)

class wxVPDrawPanel: public wxDialog
{
	public:

		wxVPDrawPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxVPDrawPanel();

		//(*Declarations(wxVPDrawPanel)
		wxPanel* Panel1;
		//*)

	protected:

		//(*Identifiers(wxVPDrawPanel)
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(wxVPDrawPanel)

		//*)
    void OnPaint(wxPaintEvent& ev);
		DECLARE_EVENT_TABLE()
};

#endif
