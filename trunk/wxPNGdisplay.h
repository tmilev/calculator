#ifndef WXPNGDISPALY_H
#define WXPNGDISPALY_H

//(*Headers(wxPNGdispaly)
#include <wx/sizer.h>
#include <wx/frame.h>
//*)

class wxPNGdispaly: public wxFrame
{
	public:

		wxPNGdispaly(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxPNGdispaly();

		//(*Declarations(wxPNGdispaly)
		//*)

	protected:

		//(*Identifiers(wxPNGdispaly)
		//*)

	private:

		//(*Handlers(wxPNGdispaly)
		void OnPaint(wxPaintEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
