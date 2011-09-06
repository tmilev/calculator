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
const long wxParserDialog::ID_BUTTON2 = wxNewId();
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
{ wxWindowDC dc(theMainWindow->frameDrawCanvas);
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
{ theMainWindow->mutexRuN.LockMe();
  std::stringstream out, out2;
  out << output.ProgressReportString1 << "\n";
  out << output.ProgressReportString2 << "\n";
  out << output.ProgressReportString3 << "\n";
  out << output.ProgressReportString4 << "\n";
  out << output.ProgressReportString5 << "\n";
  theMainWindow->ProgressReportString=out.str();
  theMainWindow->StatusString=output.StatusString1;
  wxPostEvent(theMainWindow->GetEventHandler(), theMainWindow->eventProgressReport);
  theMainWindow->mutexRuN.UnlockMe();
}

wxParserDialog::wxParserDialog(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxParserDialog)
    wxBoxSizer* BoxSizer3;

    Create(parent, wxID_ANY, _("VP calculator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("RunGtwoInBthree"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer2->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("Go"), wxDefaultPosition, wxSize(62,29), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(Button1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 4);
    Button2 = new wxButton(this, ID_BUTTON2, _("Pause+Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer3->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
    SetSizer(BoxSizer1);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserDialog::OnAbout);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserDialog::OnButton2Click);
    Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&wxParserDialog::OnInit);
    //*)
    Connect(id, wxEVT_ComputationOverMakeReport, (wxObjectEventFunction)&wxParserDialog::OnComputationOver);
    Connect(id, wxEVT_ComputationProgressReport, (wxObjectEventFunction)&wxParserDialog::OnProgressReport);
    theGlobalVariables.theDrawingVariables.SetDrawLineFunction(&drawline);
    this->frameParserOutput=new wxParserOutput(this, id);
    this->frameIndicatorWindow= new wxIndicatorWindow(this, id);
    this->frameDrawCanvas = new wxVPDrawCanvas(this, id);
    this->eventComputationOver.SetId(this->GetId());
    this->eventComputationOver.SetEventObject(this);
    this->eventComputationOver.SetEventType(wxEVT_ComputationOverMakeReport);
    this->eventProgressReport.SetId(this->GetId());
    this->eventProgressReport.SetEventObject(this);
    this->eventProgressReport.SetEventType(wxEVT_ComputationProgressReport);
    theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&FeedDataToIndicatorWindowWX);
    theMainWindow=this;
}

wxParserDialog::~wxParserDialog()
{ this->WriteSettings();
  delete this->frameDrawCanvas;
  delete this->frameIndicatorWindow;
  delete this->frameParserOutput;
  this->frameDrawCanvas=0;
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
{ this->mutexRuN.LockMe();
  this->frameIndicatorWindow->TextCtrl2->SetValue(wxString(theMainWindow->ProgressReportString.c_str(), wxConvUTF8));
  this->frameIndicatorWindow->TextCtrl1->SetValue(wxString(theMainWindow->StatusString.c_str(), wxConvUTF8));
  this->mutexRuN.UnlockMe();
}

void wxParserDialog::OnComputationOver(wxCommandEvent& ev)
{ this->mutexRuN.LockMe();
  this->frameParserOutput->TextCtrl1->SetValue(wxString(theParser.DebugString.c_str(), wxConvUTF8));
  this->GetButton1()->SetLabel(wxT("Go"));
  theGlobalVariables.DrawBuffer();
  this->mutexRuN.UnlockMe();
}

void wxParserDialog::WriteSettings()
{ std::fstream fileSettings;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileSettings, this->theSettingsFileName, false, true, false);
  fileSettings << "mainDialogX_Y_Width_Height: " <<  this->GetRect().x << " " << this->GetRect().y << " "
  << this->GetRect().width << " " << this->GetRect().height << "\n";
  fileSettings << "drawDialogX_Y_Width_Height: " <<  this->frameDrawCanvas->GetRect().x << " " << this->frameDrawCanvas->GetRect().y << " "
  << this->frameDrawCanvas->GetRect().width << " " << this->frameDrawCanvas->GetRect().height << "\n";
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
  this->frameDrawCanvas->SetSize(tempRect);
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
  this->frameDrawCanvas->Show();
}

#ifdef WIN32
void RunComputationalThread()
#else
void* RunComputationalThread(void*ptr)
#endif
{ theGlobalVariables.theLocalPauseController.InitComputation();
  theParser.DebugString= theParser.ParseEvaluateAndSimplify(theParser.StringBeingParsed, false, theGlobalVariables);
  theGlobalVariables.theLocalPauseController.ExitComputation();
  wxPostEvent(theMainWindow->GetEventHandler(),
             theMainWindow->eventComputationOver
              );
  //theMainWindow->frameDrawCanvas->Refresh();
#ifndef WIN32
  pthread_exit(NULL);
#endif
}

void wxParserDialog::RunTheComputation()
{ theGlobalVariables.theLocalPauseController.mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
  if (!theGlobalVariables.theLocalPauseController.GetFlagIsRunningUnsafeUseWithMutexHoldMe())
  { theGlobalVariables.theLocalPauseController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
    this->Button1->SetLabel(wxT("Pause"));
    theParser.StringBeingParsed= std::string(this->TextCtrl1->GetValue().mb_str());
#ifdef WIN32
    theComputationalThread.ComputationalThread=CreateThread(0,0, (LPTHREAD_START_ROUTINE)RunComputationalThread,0,0,0);
#else
    pthread_create(&theComputationalThread.ComputationalThreadLinux, NULL, RunComputationalThread, 0);
#endif
  } else
  { if (theGlobalVariables.theLocalPauseController.GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe())
    { this->Button1->SetLabel(wxT("Pause"));
      theGlobalVariables.theLocalPauseController.UnlockSafePoint();
      theGlobalVariables.theLocalPauseController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
    }
    else
    { theGlobalVariables.theLocalPauseController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
      theGlobalVariables.theLocalPauseController.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
      theGlobalVariables.theLocalPauseController.GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe()=true;
      this->Button1->SetLabel(wxT("Continue"));
    }
  }
}

void wxVPDrawCanvas::OnPanel1Paint1(wxPaintEvent& event)
{ wxPaintDC  dc(this);
  //if (theMainWindow->flagAllowRepaint)
  {	dc.SetBackground(theMainWindow->GetBackgroundColour());
    dc.DrawRectangle(wxPoint(0,0), this->GetSize());
    theGlobalVariables.DrawBuffer();
//    CombinatorialChamberContainer::drawOutput(MainWindow1->theComputationSetup.theGlobalVariablesContainer->Default()->theDrawingVariables, MainWindow1->theComputationSetup.theChambers, MainWindow1->theComputationSetup.theChambers.theDirections, MainWindow1->theComputationSetup.theChambers.theCurrentIndex, MainWindow1->theComputationSetup.theChambers.IndicatorRoot,0, &drawline, &drawtext);
  }
}

void wxParserDialog::OnButton2Click(wxCommandEvent& event)
{ Controller& theController=  tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.thePauseControlleR;
  theController.mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
  if (theController.GetFlagIsRunningUnsafeUseWithMutexHoldMe())
  { if (theController.GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe())
    { theController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
      theController.UnlockSafePoint();
    } else
    { theController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
      theController.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
      tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.WriteToDefaultFile(theGlobalVariables);
    }
  }
  theController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
}
