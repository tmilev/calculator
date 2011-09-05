/***************************************************************
 * Name:      wxParserMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-05
 * Copyright: todor milev ()
 * License:
 **************************************************************/

#include "wxParserMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(wxParserDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

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

//(*IdInit(wxParserDialog)
const long wxParserDialog::ID_TEXTCTRL1 = wxNewId();
const long wxParserDialog::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxParserDialog,wxDialog)
    //(*EventTable(wxParserDialog)
    //*)
END_EVENT_TABLE()

class WorkThreadClass
{
public:
#ifdef WIN32
	HANDLE ComputationalThread;
#else
	pthread_t ComputationalThreadLinux;
#endif
};

wxParserDialog* theMainWindow;
WorkThreadClass theComputationalThread;
Parser theParser;
GlobalVariables theGlobalVariables;

void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ wxWindowDC dc(theMainWindow->frameDrawPanel);
  wxPen tempPen;
  switch (thePenStyle)
  { case DrawingVariables::PenStyleNormal:
      tempPen.SetStyle(::wxSOLID);
      break;
		case DrawingVariables::PenStyleDashed:
      tempPen.SetStyle(::wxSHORT_DASH);
      break;
		case DrawingVariables::PenStyleDotted:
      tempPen.SetStyle(::wxDOT);
      break;
		default:
      return;
  }
  tempPen.SetColour(ColorIndex); dc.SetPen(tempPen);
  dc.DrawLine((int)X1, (int)Y1, (int) X2, (int) Y2);
}

extern GeneralizedVermaModuleCharacters tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis;

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output)
{ theMainWindow->mutexRun.LockMe();
  std::stringstream out;
  out << output.ProgressReportString1 << "\n";
  out << output.ProgressReportString2 << "\n";
  out << output.ProgressReportString3 << "\n";
  out << output.ProgressReportString4 << "\n";
  out << output.ProgressReportString5 << "\n";
  out << output.StatusString1;
  theMainWindow->ProgressReportString=out.str();
  wxPostEvent(theMainWindow->GetEventHandler(), theMainWindow->eventProgressReport);
  theMainWindow->mutexRun.UnlockMe();
}

wxParserDialog::wxParserDialog(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxParserDialog)
    Create(parent, wxID_ANY, _("VP calculator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer2->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(this, ID_BUTTON1, _("Go"), wxDefaultPosition, wxSize(62,29), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer2->Add(Button1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 4);
    BoxSizer1->Add(BoxSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
    SetSizer(BoxSizer1);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserDialog::OnAbout);
    Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&wxParserDialog::OnInit);
    //*)
    Connect(id, wxEVT_ComputationOverMakeReport, (wxObjectEventFunction)&wxParserDialog::OnComputationOver);
    Connect(id, wxEVT_ComputationProgressReport, (wxObjectEventFunction)&wxParserDialog::OnProgressReport);
    theGlobalVariables.theDrawingVariables.SetDrawLineFunction(&drawline);
    this->frameParserOutput=new wxParserOutput(this, id);
    this->frameIndicatorWindow= new wxIndicatorWindow(this, id);
    this->frameDrawPanel = new wxVPDrawPanel(this, id);
    this->eventComputationOver.SetId(this->GetId());
    this->eventComputationOver.SetEventObject(this);
    this->eventComputationOver.SetEventType(wxEVT_ComputationOverMakeReport);
    this->eventProgressReport.SetId(this->GetId());
    this->eventProgressReport.SetEventObject(this);
    this->eventProgressReport.SetEventType(wxEVT_ComputationProgressReport);
    theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&FeedDataToIndicatorWindowWX);
    theMainWindow=this;
    this->flagComputationInProgress=false;
}

wxParserDialog::~wxParserDialog()
{ this->WriteSettings();
  delete this->frameDrawPanel;
  delete this->frameIndicatorWindow;
  delete this->frameParserOutput;
  this->frameDrawPanel=0;
  this->frameIndicatorWindow=0;
  this->frameParserOutput=0;
    //(*Destroy(wxParserDialog)
    //*)
}

void wxParserDialog::OnQuit(wxCommandEvent& event)
{
  Close();
}

void wxParserDialog::OnAbout(wxCommandEvent& event)
{ this->RunTheComputation();
}

void wxParserDialog::OnProgressReport(wxCommandEvent& ev)
{ this->mutexRun.LockMe();
  this->frameIndicatorWindow->TextCtrl1->SetValue(wxString(theMainWindow->ProgressReportString.c_str(), wxConvUTF8));
  this->mutexRun.UnlockMe();
}

void wxParserDialog::OnComputationOver(wxCommandEvent& ev)
{ this->mutexRun.LockMe();
  this->frameParserOutput->TextCtrl1->SetValue(wxString(theParser.DebugString.c_str(), wxConvUTF8));
  this->GetButton1()->SetLabel(wxT("Go"));
  theGlobalVariables.theDrawingVariables.drawBuffer();
  this->mutexRun.UnlockMe();
}

void wxParserDialog::WriteSettings()
{ std::fstream fileSettings;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileSettings, this->theSettingsFileName, false, true, false);
  fileSettings << "mainDialogX_Y_Width_Height: " <<  this->GetRect().x << " " << this->GetRect().y << " "
  << this->GetRect().width << " " << this->GetRect().height << "\n";
  fileSettings << "drawDialogX_Y_Width_Height: " <<  this->frameDrawPanel->GetRect().x << " " << this->frameDrawPanel->GetRect().y << " "
  << this->frameDrawPanel->GetRect().width << " " << this->frameDrawPanel->GetRect().height << "\n";
  fileSettings << "indicatorDialogX_Y_Width_Height: " <<  this->frameIndicatorWindow->GetRect().x << " " << this->frameIndicatorWindow->GetRect().y << " "
  << this->frameIndicatorWindow->GetRect().width << " " << this->frameIndicatorWindow->GetRect().height << "\n";
  fileSettings << "outputDialogX_Y_Width_Height: " <<  this->frameParserOutput->GetRect().x << " " << this->frameParserOutput->GetRect().y << " "
  << this->frameParserOutput->GetRect().width << " " << this->frameParserOutput->GetRect().height << "\n";
}

void wxParserDialog::ReadSettings()
{ if (!CGIspecificRoutines::FileExists(this->theSettingsFileName))
    return;
  std::fstream fileSettings;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileSettings, this->theSettingsFileName, false, false, false);
  std::string tempS;
  wxRect tempRect;
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->SetSize(tempRect);
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->frameDrawPanel->SetSize(tempRect);
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->frameIndicatorWindow->SetSize(tempRect);
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->frameParserOutput->SetSize(tempRect);
}

void wxParserDialog::OnInit(wxInitDialogEvent& event)
{ this->thePath=std::string(wxStandardPaths::Get().GetExecutablePath().mb_str());
  this->theSettingsFileName=this->thePath;
  this->theSettingsFileName.append("Settings.txt");
  this->ReadSettings();

  this->frameParserOutput->Show();
  this->frameIndicatorWindow->Show();
  this->frameDrawPanel->Show();
}

#ifdef WIN32
void RunComputationalThread()
#else
void* RunComputationalThread(void*ptr)
#endif
{ theParser.DebugString= theParser.ParseEvaluateAndSimplify(theParser.StringBeingParsed, false, theGlobalVariables);
  theMainWindow->flagComputationInProgress=false;
  wxPostEvent(theMainWindow->GetEventHandler(),
             theMainWindow->eventComputationOver
              );
  //theMainWindow->frameDrawPanel->Refresh();
#ifndef WIN32
  pthread_exit(NULL);
#endif
}

void wxParserDialog::RunTheComputation()
{ this->mutexRun.LockMe();
  if (!this->flagComputationInProgress)
  { this->flagComputationInProgress=true;
    this->Button1->SetLabel(wxT("Pause"));
    theParser.StringBeingParsed= std::string(this->TextCtrl1->GetValue().mb_str());
#ifdef WIN32
    theComputationalThread.ComputationalThread=CreateThread(0,0, (LPTHREAD_START_ROUTINE)RunComputationalThread,0,0,0);
#else
    pthread_create(&theComputationalThread.ComputationalThreadLinux, NULL, RunComputationalThread, 0);
#endif
  } //else
    //ParallelComputing::controllerLockThisMutexToSignalPause.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  this->mutexRun.UnlockMe();
}

