#ifndef WXINDICATORWINDOW_H
#define WXINDICATORWINDOW_H

//(*Headers(wxIndicatorWindow)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>
//*)

class wxIndicatorWindow: public wxDialog
{
	public:

		wxIndicatorWindow(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxIndicatorWindow();

		//(*Declarations(wxIndicatorWindow)
		wxTextCtrl* TextCtrl2;
		wxTextCtrl* TextCtrl1;
		//*)

	protected:

		//(*Identifiers(wxIndicatorWindow)
		static const long ID_TEXTCTRL2;
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(wxIndicatorWindow)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
