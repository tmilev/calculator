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
const long wxParserFrame::ID_CHOICE1 = wxNewId();
const long wxParserFrame::ID_SPINCTRL1 = wxNewId();
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

void EnsureBitmapsSuffice()
{ DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  int oldSize=theMainWindow->theBitmapList.size;
  if (oldSize<=theOps.SelectedPlane)
  { theMainWindow->theBitmapList.SetSize(theOps.SelectedPlane+50);
    for (int i=oldSize; i<theMainWindow->theBitmapList.size; i++)
      theMainWindow->theBitmapList[i]=new wxBitmap(theMainWindow->bitmapW, theMainWindow->bitmapW);
  }
}

void drawCircle(double X1, double Y1, double radius, unsigned long thePenStyle, int ColorIndex)
{ //wxWindowDC dc(theMainWindow->theDrawPanel);//->Panel1);
  DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  EnsureBitmapsSuffice();
  wxMemoryDC dc;
  dc.SelectObject(*theMainWindow->theBitmapList[theOps.SelectedPlane]);
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
  DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  EnsureBitmapsSuffice();
  wxMemoryDC dc;
  dc.SelectObject(*theMainWindow->theBitmapList[theOps.SelectedPlane]);
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

void drawClearScreen()
{ DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  EnsureBitmapsSuffice();
  wxMemoryDC tempDC;
  tempDC.SetBackground(wxBrush (wxColour(255,255,255),wxSOLID));
  tempDC.SelectObject(*theMainWindow->theBitmapList[theOps.SelectedPlane]);
  tempDC.Clear();
  tempDC.SelectObject(wxNullBitmap);
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
    wxBoxSizer* BoxSizer3;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Choice1 = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    Choice1->Append(_("A"));
    Choice1->Append(_("B"));
    Choice1->Append(_("C"));
    Choice1->Append(_("D"));
    Choice1->Append(_("E"));
    Choice1->SetSelection( Choice1->Append(_("F")) );
    Choice1->Append(_("G"));
    BoxSizer3->Add(Choice1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    SpinCtrl1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("4"), wxDefaultPosition, wxSize(36,27), 0, 2, 8, 4, _T("ID_SPINCTRL1"));
    SpinCtrl1->SetValue(_T("4"));
    BoxSizer3->Add(SpinCtrl1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("animateRootSystemDefault(50)"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
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

    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&wxParserFrame::OnChoice1Select);
    Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxParserFrame::OnSpinCtrl1Change);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserFrame::OnButton1Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxParserFrame::OnButton2Click);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&wxParserFrame::OnTimer1Trigger);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&wxParserFrame::OnClose);
    //*)
    this->theDrawPanel= new wxDrawPanel(this);
    this->theParserOutput= new wxParserOutput(this);
    this->theStatus= new wxStatus(this);
    this->thePNGdisplay= new wxPNGdispaly(this);
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
    theGlobalVariables.theDrawingVariables.SetDrawClearFunction(&drawClearScreen);
    theParser.DefaultWeylRank=4;
    theParser.DefaultWeylLetter='F';

    this->thePath=std::string(wxStandardPaths::Get().GetExecutablePath().mb_str());
    for (unsigned i=this->thePath.size()-1; i>=0; i--)
    { if (this->thePath[i]=='/' || this->thePath[i]=='\\')
        break;
      thePath.resize(thePath.size()-1);
    }
    this->theSettingsFileName=this->thePath+"Settings.txt";
    this->ReadSettings();

    theMainWindow=this;
    this->theStatus->Show();
    this->theParserOutput->Show();
    this->theDrawPanel->Show();
    this->thePNGdisplay->Show();
//    this->theDrawPanel->GetSize(&this->bitmapW, &this->bitmapH);
    this->bitmapH=600;
    this->bitmapW=600;
    theBitmapList.SetSize(1);
    this->theBitmapList[0]= new wxBitmap(this->bitmapW, this->bitmapH);
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
  EnsureBitmapsSuffice();
  DrawOperations& theOps= theParser.theValue.theAnimation.GetElement();
  wxBitmap& theBM=*theMainWindow->theBitmapList[theOps.SelectedPlane];
  assert(theOps.SelectedPlane>=0 && theOps.SelectedPlane<theMainWindow->theBitmapList.size);
  if (!theOps.flagAnimatingMovingCoordSystem || theOps.flagIsPausedWhileAnimating)
    theGlobalVariables.theDrawingVariables.drawBufferNoIniT(theOps);
  dc.DrawBitmap(theBM, 0, 0);
  theMainWindow->mutexRuN.UnlockMe();
}

void wxDrawPanel::OnPanel1LeftDown(wxMouseEvent& event)
{ DrawOperations& theOps= theParser.theValue.theAnimation.GetElement();
  theOps.click(event.GetX(), event.GetY());
}

void wxDrawPanel::OnPanel1MouseWheel(wxMouseEvent& event)
{ // scroll in drop down list using mouse wheel
  if (theMainWindow==0)
    return;
  int rot = event.GetWheelRotation()/event.GetWheelDelta();
//  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.LockMe();
  DrawOperations& theOps= theParser.theValue.theAnimation.GetElement();
  theOps.GraphicsUnit[theOps.SelectedPlane]+=rot*5;
//  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.UnlockMe();
  //wxPaintDC dc(this);
  //dc.SetBackground(theMainWindow->GetBackgroundColour());
  //dc.DrawRectangle(wxPoint(0,0), this->GetSize());
  //theGlobalVariables.DrawBufferNoInit();
  this->Refresh();
}

void wxDrawPanel::OnPanel1MouseMove(wxMouseEvent& event)
{ DrawOperations& theOps= theParser.theValue.theAnimation.GetElement();
  if (theOps.SelectedCircleMinus2noneMinus1Center==-2)
    return;
  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.LockMe();
  theOps.mouseMoveRedraw(event.GetX(), event.GetY());
  theGlobalVariables.theDrawingVariables.LockedWhileDrawing.UnlockMe();
  theMainWindow->theStatus->TextCtrlStatusString->SetValue
  (wxString(theOps.DebugString.c_str(), wxConvUTF8));
  this->Refresh();
}

void wxDrawPanel::OnPanel1LeftUp(wxMouseEvent& event)
{ DrawOperations& theOps= theParser.theValue.theAnimation.GetElement();
  theOps.SelectedCircleMinus2noneMinus1Center=-2;
}

void wxParserFrame::OnProgressReport(wxCommandEvent& ev)
{ this->mutexRuN.LockMe();
  this->theStatus->TextCtrlProgressString->SetValue(wxString(theMainWindow->ProgressReportString.c_str(), wxConvUTF8));
  this->theStatus->TextCtrlStatusString->SetValue(wxString(theMainWindow->StatusString.c_str(), wxConvUTF8));
  this->mutexRuN.UnlockMe();
}

void Animate()
{ DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  for (theOps.SelectedPlane=0;  theOps.SelectedPlane<theOps.BasisProjectionPlane.size; theOps.SelectedPlane++)
  { theGlobalVariables.theDrawingVariables.drawBufferNoIniT(theOps);
    std::stringstream tempStream;
    tempStream << "Computing frame " << theOps.SelectedPlane+1 << " out of " << theOps.BasisProjectionPlane.size;
    std::string tempS=tempStream.str();
    theMainWindow->theStatus->TextCtrlProgressString->SetValue(wxString(tempS.c_str(), wxConvUTF8));
    theMainWindow->theStatus->TextCtrlProgressString->Update();
  }
  theMainWindow->StartTimer();
}

void wxParserFrame::OnComputationOver(wxCommandEvent& ev)
{ this->mutexRuN.LockMe();
  this->theParserOutput->TextCtrl1->SetValue(wxString(theParser.DebugString.c_str(), wxConvUTF8));
  this->GetButton1()->SetLabel(wxT("Go"));
  DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  if (theOps.flagAnimatingMovingCoordSystem)
    Animate();
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
  fileSettings << "outputDialogX_Y_Width_Height: " <<  this->thePNGdisplay->GetRect().x << " " << this->thePNGdisplay->GetRect().y << " "
  << this->thePNGdisplay->GetRect().width << " " << this->thePNGdisplay->GetRect().height << "\n";
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
  fileSettings >> tempS >> tempRect.x >> tempRect.y >> tempRect.width >> tempRect.height;
  this->thePNGdisplay->SetSize(tempRect);
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
{ DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  if (theOps.flagAnimatingMovingCoordSystem)
  { if (theMainWindow->GetTimer().IsRunning())
    { theMainWindow->GetTimer().Stop();
      theMainWindow->Button2->SetLabel(wxT("Continue"));
      theOps.flagIsPausedWhileAnimating=true;
    } else
    { theOps.flagIsPausedWhileAnimating=false;
      theMainWindow->GetTimer().Start();
      theMainWindow->Button2->SetLabel(wxT("Pause"));
    }
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
{ DrawOperations& theOps=theParser.theValue.theAnimation.GetElement();
  if (!theOps.flagAnimatingMovingCoordSystem)
    return;
  int& frameIndex=theParser.theValue.theAnimation.GetElement().SelectedPlane;
  assert(theOps.BasisProjectionPlane.size<=this->theBitmapList.size);
  if (frameIndex==theOps.BasisProjectionPlane.size-2)
  { theMainWindow->Timer1.Stop();
    theOps.flagIsPausedWhileAnimating=true;
    theMainWindow->Button2->SetLabel(wxT("Continue"));
  }
  frameIndex=(frameIndex+1)% theOps.BasisProjectionPlane.size;
  this->theDrawPanel->Refresh();
}

void wxParserFrame::UpdateChoices()
{ theMainWindow->SpinCtrl1->SetValue(theParser.DefaultWeylRank);
  theMainWindow->Choice1->SetSelection(theParser.DefaultWeylLetter- 'A');
}

void wxParserFrame::OnChoice1Select(wxCommandEvent& event)
{ switch(this->Choice1->GetSelection())
  { case 0: theParser.DefaultWeylLetter='A'; break;
    case 1: theParser.DefaultWeylLetter='B'; break;
    case 2: theParser.DefaultWeylLetter='C'; break;
    case 3: theParser.DefaultWeylLetter='D'; break;
    case 4: theParser.DefaultWeylLetter='E'; break;
    case 5: theParser.DefaultWeylLetter='F'; break;
    case 6: theParser.DefaultWeylLetter='G'; break;
    default: break;
  }
  char oldLetter=theParser.DefaultWeylLetter;
  int oldRank = theParser.DefaultWeylRank;
  CGIspecificRoutines::MakeSureWeylGroupIsSane(theParser.DefaultWeylLetter, theParser.DefaultWeylRank);
  if (oldLetter!=theParser.DefaultWeylLetter || oldRank != theParser.DefaultWeylRank)
    this->UpdateChoices();
}

void wxParserFrame::OnSpinCtrl1Change(wxSpinEvent& event)
{ theParser.DefaultWeylRank=this->SpinCtrl1->GetValue();
  char oldLetter=theParser.DefaultWeylLetter;
  int oldRank = theParser.DefaultWeylRank;
  CGIspecificRoutines::MakeSureWeylGroupIsSane(theParser.DefaultWeylLetter, theParser.DefaultWeylRank);
  if (oldLetter!=theParser.DefaultWeylLetter || oldRank != theParser.DefaultWeylRank)
    this->UpdateChoices();
}

void wxPNGdispaly::OnPaint(wxPaintEvent& event)
{ wxImage theImage;

  theImage.LoadFile(wxString((theMainWindow->thePath+"jacobs_logo.png").c_str(), wxConvUTF8), wxBITMAP_TYPE_PNG);
  wxPaintDC theDC(this);
  theDC.DrawBitmap(wxBitmap(theImage), 0, 0);
}
