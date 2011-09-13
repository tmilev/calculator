#ifndef WXVPDRAWCANVAS_H
#define WXVPDRAWCANVAS_H

//(*Headers(wxVPDrawCanvas)
#include <wx/sizer.h>
#include <wx/dialog.h>
//*)

class wxVPDrawCanvas: public wxDialog
{
	public:

		wxVPDrawCanvas(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxVPDrawCanvas();

		//(*Declarations(wxVPDrawCanvas)
		//*)

	protected:

		//(*Identifiers(wxVPDrawCanvas)
		//*)

	private:

		//(*Handlers(wxVPDrawCanvas)
		void OnInit(wxInitDialogEvent& event);
		void OnPanel1Paint1(wxPaintEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
