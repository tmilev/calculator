
#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/app.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


class VectorPartitionApp : public wxApp
{
public:
  virtual bool OnInit();
};

class VectorPartitionFrame: public wxFrame
{
public:
  VectorPartitionFrame(wxFrame *frame, const wxString& title);
  ~VectorPartitionFrame();
private:
  enum
  { idMenuQuit = 1000,
    idMenuAbout
  };
  void OnClose(wxCloseEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(VectorPartitionApp);

bool VectorPartitionApp::OnInit()
{
    VectorPartitionFrame* frame = new VectorPartitionFrame(0L, _("wxWidgets Application Template"));

    frame->Show();

    return true;
}

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(VectorPartitionFrame, wxFrame)
    EVT_CLOSE(VectorPartitionFrame::OnClose)
    EVT_MENU(idMenuQuit, VectorPartitionFrame::OnQuit)
    EVT_MENU(idMenuAbout, VectorPartitionFrame::OnAbout)
END_EVENT_TABLE()

VectorPartitionFrame::VectorPartitionFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

}


VectorPartitionFrame::~VectorPartitionFrame()
{
}

void VectorPartitionFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void VectorPartitionFrame::OnQuit(wxCommandEvent &event)
{
}

void VectorPartitionFrame::OnAbout(wxCommandEvent &event)
{
}
