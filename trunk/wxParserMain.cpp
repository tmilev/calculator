/***************************************************************
 * Name:      wxParserMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-13
 * Copyright: todor milev ()
 * License:
 **************************************************************/

#include "wxParserMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(wxParserFrame)
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

//(*IdInit(wxParserFrame)
const long wxParserFrame::ID_TEXTCTRL1 = wxNewId();
const long wxParserFrame::ID_BUTTON1 = wxNewId();
const long wxParserFrame::ID_BUTTON2 = wxNewId();
const long wxParserFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxParserFrame,wxFrame)
    //(*EventTable(wxParserFrame)
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

wxParserFrame* theMainWindow;
WorkThreadClass theComputationalThread;
Parser theParser;
GlobalVariables theGlobalVariables;
extern GeneralizedVermaModuleCharacters tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis;

void drawCircle(double X1, double Y1, double radius, unsigned long thePenStyle, int ColorIndex)
{ //wxWindowDC dc(theMainWindow->theDrawPanel);//->Panel1);
  wxMemoryDC dc;
  dc.SelectObject(*theMainWindow->theBitmapList[theMainWindow->currentBitmap]);
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
  tempPen.SetColour((ColorIndex/(256*256))%256, (ColorIndex/256)%256, ColorIndex%256);
  dc.SetPen(tempPen);
  dc.DrawCircle((int)X1, (int)Y1, radius);
  dc.SelectObject(wxNullBitmap);
}

void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ //wxWindowDC dc(theMainWindow->theDrawPanel);//->Panel1);
  wxMemoryDC dc;
  dc.SelectObject(*theMainWindow->theBitmapList[theMainWindow->currentBitmap]);
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
  tempPen.SetColour((ColorIndex/(256*256))%256, (ColorIndex/256)%256, ColorIndex%256);
  dc.SetPen(tempPen);
  dc.DrawLine((int)X1, (int)Y1, (int) X2, (int) Y2);
  dc.SelectObject(wxNullBitmap);
}

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output)
{ theMainWindow->mutexRuN.LockMe();
  std::stringstream out, out2;
  for (int i=0; i<output.ProgressReportStrings.size; i++)
    out << output.ProgressReportStrings[i] << "\n";
  theMainWindow->ProgressReportString=out.str();
  theMainWindow->StatusString=output.StatusString1;
  wxPostEvent(theMainWindow->GetEventHandler(), theMainWindow->eventProgressReport);
  theMainWindow->mutexRuN.UnlockMe();
}

wxParserFrame::wxParserFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxParserFrame)
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("animateRootSystem"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("Go"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer2->Add(Button1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Pause"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer2->Add(Button2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(100, false);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserFrame::OnButton1Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserFrame::OnButton2Click);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&wxParserFrame::OnTimer1Trigger);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&wxParserFrame::OnClose);
    //*)
    this->theDrawPanel= new wxDrawPanel(this);
    this->theParserOutput= new wxParserOutput(this);
    this->theStatus= new wxStatus(this);
    this->Timer1.Stop();

    this->Connect(id, wxEVT_ComputationOverMakeReport, (wxObjectEventFunction)&wxParserFrame::OnComputationOver);
    this->Connect(id, wxEVT_ComputationProgressReport, (wxObjectEventFunction)&wxParserFrame::OnProgressReport);
    this->eventComputationOver.SetId(this->GetId());
    this->eventComputationOver.SetEventObject(this);
    this->eventComputationOver.SetEventType(wxEVT_ComputationOverMakeReport);
    this->eventProgressReport.SetId(this->GetId());
    this->eventProgressReport.SetEventObject(this);
    this->eventProgressReport.SetEventType(wxEVT_ComputationProgressReport);
    theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&FeedDataToIndicatorWindowWX);
    theGlobalVariables.theDrawingVariables.SetDrawLineFunction(&drawline);
    theGlobalVariables.theDrawingVariables.SetDrawCircleFunction(&drawCircle);
    theParser.DefaultWeylRank=4;
    theParser.DefaultWeylLetter='F';

    this->thePath=std::string(wxStandardPaths::Get().GetExecutablePath().mb_str());
    this->theSettingsFileName=this->thePath;
    this->theSettingsFileName.append("Settings.txt");
    this->ReadSettings();

    theMainWindow=this;
    this->theStatus->Show();
    this->theParserOutput->Show();
    this->theDrawPanel->Show();
    theBitmapList.SetSize(500);
//    this->theDrawPanel->GetSize(&this->bitmapW, &this->bitmapH);
    this->bitmapH=600;
    this->bitmapW=600;
    for (int i=0; i<theBitmapList.size; i++)
      this->theBitmapList[i]= new wxBitmap(this->bitmapW, this->bitmapH);
    this->currentBitmap=0;
    this->Quitting=false;
}

wxParserFrame::~wxParserFrame()
{ this->Quitting=true;
  if (this->theStatus!=0)
    this->WriteSettings();
  //std::exit(0);
  this->theStatus=0;
  this->theParserOutput=0;
  this->theDrawPanel=0;
}

void wxParserFrame::OnQuit(wxCommandEvent& event)
{ this->Close();
}

void wxParserFrame::OnClose(wxCloseEvent& event)
{ this->Destroy();
}

void wxDrawPanel::OnPaint(wxPaintEvent& event)
{ wxPaintDC dc(this->Panel1);
  if (theMainWindow->Quitting)
    return;
  theMainWindow->mutexRuN.LockMe();
  wxBitmap& theBM=*theMainWindow->theBitmapList[theMainWindow->currentBitmap];
  assert(theMainWindow->currentBitmap>=0 && theMainWindow->currentBitmap< theMainWindow->theBitmapList.size);
  if (!theGlobalVariables.theDrawingVariables.theBuffer.flagAnimatingMovingCoordSystem)
  { wxMemoryDC tempDC;
    tempDC.SelectObject(theBM);
    tempDC.Clear();
    tempDC.SelectObject(wxNullBitmap);
    theGlobalVariables.DrawBufferNoInit();
  }
  dc.DrawBitmap(theBM,0,0);
  theMainWindow->mutexRuN.UnlockMe();
}

void wxDrawPanel::OnPanel1LeftDown(wxMouseEvent& event)
{ theGlobalVariables.theDrawingVariables.theBuffer.click(event.GetX(), event.GetY());
}

void wxDrawPanel::OnPanel1MouseWheel(wxMouseEvent& event)
{ // scroll in drop down list using mouse wheel
  if (theMainWindow==0)
    return;
  int rot = event.GetWheelRotation()/event.GetWheelDelta();
//  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.LockMe();
  theGlobalVariables.theDrawingVariables.theBuffer.GraphicsUnit+=rot*5;
//  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.UnlockMe();
  //wxPaintDC dc(this);
  //dc.SetBackground(theMainWindow->GetBackgroundColour());
  //dc.DrawRectangle(wxPoint(0,0), this->GetSize());
  //theGlobalVariables.DrawBufferNoInit();
  this->Refresh();
}

void wxDrawPanel::OnPanel1MouseMove(wxMouseEvent& event)
{ if (theGlobalVariables.theDrawingVariables.theBuffer.SelectedIndex==-2)
    return;
  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.LockMe();
  theGlobalVariables.theDrawingVariables.theBuffer.mouseMoveRedraw(event.GetX(), event.GetY());
  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.UnlockMe();
  theMainWindow->theStatus->TextCtrlStatusString->SetValue
  (wxString(theGlobalVariables.theDrawingVariables.theBuffer.DebugString.c_str(), wxConvUTF8));
  this->Refresh();
}

void wxDrawPanel::OnPanel1LeftUp(wxMouseEvent& event)
{ theGlobalVariables.theDrawingVariables.theBuffer.SelectedIndex=-2;
}

void wxParserFrame::OnProgressReport(wxCommandEvent& ev)
{ this->mutexRuN.LockMe();
  this->theStatus->TextCtrlProgressString->SetValue(wxString(theMainWindow->ProgressReportString.c_str(), wxConvUTF8));
  this->theStatus->TextCtrlStatusString->SetValue(wxString(theMainWindow->StatusString.c_str(), wxConvUTF8));
  this->mutexRuN.UnlockMe();
}

void Animate()
{ Vector<double> target1, target2, start1, start2;
  int theDim=theParser.theHmm.theRange.GetRank();
  DrawOperations& theOps=theGlobalVariables.theDrawingVariables.theBuffer;
  target1.MakeEi(theDim, 0, 1, 0);
  target2.MakeEi(theDim, 1, 1, 0);
  start1=theOps.BasisProjectionPlane[0];
  start2=theOps.BasisProjectionPlane[1];
  theOps.ModifyToOrthonormalNoShiftSecond(target1, target2);
  for (theMainWindow->currentBitmap=0; theMainWindow->currentBitmap<theMainWindow->theBitmapList.size; theMainWindow->currentBitmap++)
  { double fraction=((double) theMainWindow->currentBitmap)/((double)(theMainWindow->theBitmapList.size-1));
    wxMemoryDC tempDC;
    tempDC.SelectObject(*theMainWindow->theBitmapList[theMainWindow->currentBitmap]);
    tempDC.Clear();
    tempDC.SelectObject(wxNullBitmap);
    theOps.BasisProjectionPlane[0]=start1*(1-fraction);
    theOps.BasisProjectionPlane[1]=start2*(1-fraction);
    theOps.BasisProjectionPlane[0]+=target1*fraction;
    theOps.BasisProjectionPlane[1]+=target2*fraction;
    theOps.ModifyToOrthonormalNoShiftSecond(theOps.BasisProjectionPlane[0], theOps.BasisProjectionPlane[1]);
    theGlobalVariables.DrawBufferNoInit();
    std::stringstream tempStream;
    tempStream << "Computing frame " << theMainWindow->currentBitmap+1 << " out of " << theMainWindow->theBitmapList.size << " desired frames.\n Please wait!";
    wxString reportString( std::string(tempStream.str()).c_str(), wxConvUTF8);
    theMainWindow->theStatus->TextCtrlProgressString->SetValue(reportString);
    theMainWindow->theStatus->TextCtrlProgressString->Update();
  }
  theMainWindow->currentBitmap=0;
  theMainWindow->StartTimer();
}

void wxParserFrame::OnComputationOver(wxCommandEvent& ev)
{ this->mutexRuN.LockMe();
  this->theParserOutput->TextCtrl1->SetValue(wxString(theParser.DebugString.c_str(), wxConvUTF8));
  this->GetButton1()->SetLabel(wxT("Go"));
  if (theGlobalVariables.theDrawingVariables.theBuffer.flagAnimatingMovingCoordSystem)
  { Animate();
  }
  theMainWindow->theDrawPanel->Panel1->Refresh();
  this->mutexRuN.UnlockMe();
}

void wxParserFrame::WriteSettings()
{ std::fstream fileSettings;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileSettings, this->theSettingsFileName, false, true, false);
  fileSettings << "mainDialogX_Y_Width_Height: " <<  this->GetRect().x << " " << this->GetRect().y << " "
  << this->GetRect().width << " " << this->GetRect().height << "\n";
  fileSettings << "drawDialogX_Y_Width_Height: " <<  this->theDrawPanel->GetRect().x << " " << this->theDrawPanel->GetRect().y << " "
  << this->theDrawPanel->GetRect().width << " " << this->theDrawPanel->GetRect().height << "\n";
  fileSettings << "indicatorDialogX_Y_Width_Height: " <<  this->theStatus->GetRect().x << " " << this->theStatus->GetRect().y << " "
  << this->theStatus->GetRect().width << " " << this->theStatus->GetRect().height << "\n";
  fileSettings << "outputDialogX_Y_Width_Height: " <<  this->theParserOutput->GetRect().x << " " << this->theParserOutput->GetRect().y << " "
  << this->theParserOutput->GetRect().width << " " << this->theParserOutput->GetRect().height << "\n";
}

void wxParserFrame::ReadSettings()
{ if (!CGIspecificRoutines::FileExists(this->theSettingsFileName))
    return;
  std::fstream fileSettings;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileSettings, this->theSettingsFileName, false, false, false);
  std::string tempS;
  wxRect tempRect;
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->SetSize(tempRect);
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->theDrawPanel->SetSize(tempRect);
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->theStatus->SetSize(tempRect);
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->theParserOutput->SetSize(tempRect);
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
  //theMainWindow->frameInput->Refresh();
#ifndef WIN32
  pthread_exit(NULL);
#endif
}

void wxParserFrame::RunTheComputation()
{ theGlobalVariables.theLocalPauseController.mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
  if (!theGlobalVariables.theLocalPauseController.GetFlagIsRunningUnsafeUseWithMutexHoldMe())
  { theGlobalVariables.theLocalPauseController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
    this->GetButton1()->SetLabel(wxT("Pause"));
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

void wxParserFrame::OnButton2Click(wxCommandEvent& event)
{ if (theMainWindow->GetTimer().IsRunning())
  { theMainWindow->GetTimer().Stop();
    theMainWindow->Button2->SetLabel(wxT("Continue"));
  } else
  { theMainWindow->GetTimer().Start();
    theMainWindow->Button2->SetLabel(wxT("Pause"));
  }

  return;
  Controller& theController=  tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.thePauseControlleR;
  theController.mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
  if (theController.GetFlagIsRunningUnsafeUseWithMutexHoldMe())
  { if (theController.GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe())
    { theController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
      theController.UnlockSafePoint();
    } else
    { theController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
      theController.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
      tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.WriteToDefaultFile(&theGlobalVariables);
    }
  }
  theController.mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
}

void wxParserFrame::OnButton1Click(wxCommandEvent& event)
{ this->RunTheComputation();
}

void wxParserFrame::OnTimer1Trigger(wxTimerEvent& event)
{ if (!theGlobalVariables.theDrawingVariables.theBuffer.flagAnimatingMovingCoordSystem)
    return;
  this->currentBitmap=(this->currentBitmap+1)% this->theBitmapList.size;
  this->theDrawPanel->Refresh();
}
