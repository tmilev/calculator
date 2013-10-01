#ifndef WXPNGDISPLAY_H
#define WXPNGDISPLAY_H

//(*Headers(wxPNGdisplay)
#include <wx/sizer.h>
#include <wx/frame.h>
//*)

class wxPNGdisplay: public wxFrame
{
	public:

		wxPNGdisplay(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxPNGdisplay();

		//(*Declarations(wxPNGdisplay)
		//*)

	protected:

		//(*Identifiers(wxPNGdisplay)
		//*)

	private:

		//(*Handlers(wxPNGdisplay)
		void OnPaint(wxPaintEvent& event);
		void OnMouseWheel(wxMouseEvent& event);
		//*)
		DECLARE_EVENT_TABLE()
};

#endif
