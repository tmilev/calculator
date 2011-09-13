#ifndef WXVPDRAWCANVAS_H
#define WXVPDRAWCANVAS_H

//(*Headers(wxVPDrawCanvas)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class wxVPDrawCanvas: public wxDialog
{
	public:
		wxVPDrawCanvas(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxVPDrawCanvas();
    void onMouseMove(wxMouseEvent& ev);

		//(*Declarations(wxVPDrawCanvas)
		wxButton* Button1;
		wxButton* Button2;
		wxTextCtrl* TextCtrl1;
		//*)

	protected:

		//(*Identifiers(wxVPDrawCanvas)
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(wxVPDrawCanvas)
		void OnInit(wxInitDialogEvent& event);
		void OnPanel1Paint1(wxPaintEvent& event);
		void OnPanel1Paint(wxPaintEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		//*)
		DECLARE_EVENT_TABLE()
};

#endif
