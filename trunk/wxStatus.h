#ifndef WXSTATUS_H
#define WXSTATUS_H

//(*Headers(wxStatus)
#include <wx/frame.h>
#include <wx/textctrl.h>
//*)

class wxStatus: public wxFrame
{
	public:

		wxStatus(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxStatus();

		//(*Declarations(wxStatus)
		wxTextCtrl* TextCtrlProgressString;
		//*)

	protected:

		//(*Identifiers(wxStatus)
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(wxStatus)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
