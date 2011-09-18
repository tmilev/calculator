#ifndef WXDRAWPANEL_H
#define WXDRAWPANEL_H

//(*Headers(wxDrawPanel)
#include <wx/frame.h>
//*)

class wxDrawPanel: public wxFrame
{
	public:

		wxDrawPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxDrawPanel();

		//(*Declarations(wxDrawPanel)
		//*)

	protected:

		//(*Identifiers(wxDrawPanel)
		//*)

	private:

		//(*Handlers(wxDrawPanel)
		void OnPaint(wxPaintEvent& event);
		void OnPanel1MouseMove(wxMouseEvent& event);
		void OnPanel1LeftDown(wxMouseEvent& event);
		void OnPanel1LeftUp(wxMouseEvent& event);
		void OnPanel1MouseWheel(wxMouseEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
