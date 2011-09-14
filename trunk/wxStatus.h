#ifndef WXSTATUS_H
#define WXSTATUS_H

//(*Headers(wxStatus)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/frame.h>
//*)

class wxStatus: public wxFrame
{
	public:

		wxStatus(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxStatus();

		//(*Declarations(wxStatus)
		wxTextCtrl* TextCtrlStatusString;
		wxTextCtrl* TextCtrlProgressString;
		//*)

	protected:

		//(*Identifiers(wxStatus)
		static const long ID_TEXTCTRL1;
		static const long ID_TEXTCTRL2;
		//*)

	private:

		//(*Handlers(wxStatus)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
