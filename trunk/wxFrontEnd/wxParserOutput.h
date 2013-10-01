#ifndef WXPARSEROUTPUT_H
#define WXPARSEROUTPUT_H

//(*Headers(wxParserOutput)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/frame.h>
//*)

class wxParserOutput: public wxFrame
{
	public:

		wxParserOutput(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~wxParserOutput();

		//(*Declarations(wxParserOutput)
		wxTextCtrl* TextCtrl1;
		//*)

	protected:

		//(*Identifiers(wxParserOutput)
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(wxParserOutput)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
