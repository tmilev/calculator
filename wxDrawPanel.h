#ifndef WXDRAWPANEL_H
#define WXDRAWPANEL_H

//(*Headers(wxDrawPanel)
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>
//*)

class wxDrawPanel: public wxFrame
{
	public:

		wxDrawPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxDrawPanel();

		//(*Declarations(wxDrawPanel)
		wxPanel* Panel1;
		//*)

	protected:

		//(*Identifiers(wxDrawPanel)
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(wxDrawPanel)
		void OnPaint(wxPaintEvent& event);
		void OnPanel1MouseMove(wxMouseEvent& event);
		void OnPanel1LeftDown(wxMouseEvent& event);
		void OnPanel1LeftUp(wxMouseEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
