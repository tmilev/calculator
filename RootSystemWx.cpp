#ifdef USE_GUI

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/tglbtn.h"
#include <sys/time.h>

#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
#include "wx/sizer.h"
#include <wx/string.h>
#include "wx/spinbutt.h"
#include "wx/menu.h"
#include "wx/spinctrl.h"
#include "wx/textctrl.h"
#include "wx/filename.h"
#ifndef WIN32
#include <unistd.h>
#endif
//#include <math.h>
#include "polyhedra.h"

//#include "to_be_merged_in_main.cpp"
extern void main_test_function(std::string& output, GlobalVariables& theGlobalVariables);

class guiMainWindow;
class wxDialogOutput;
class guiApp : public wxApp
{
public:
  bool OnInit();
};


DECLARE_EVENT_TYPE(wxEVT_ProgressReport, -1)
DEFINE_EVENT_TYPE(wxEVT_ProgressReport)
DECLARE_EVENT_TYPE(wxEVT_ComputationFinished, -1)
DEFINE_EVENT_TYPE(wxEVT_ComputationFinished)

class drawCanvas : public ::wxPanel
{
public:
  void OnPaint(wxPaintEvent& ev);
  drawCanvas(wxWindow *parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER, const wxString& name = wxPanelNameStr)
  {	this->Create(parent, winid, pos, size, style, name);
  };
  void onMouseDownOnCanvas(wxMouseEvent &ev);
  void onMouseUpOnCanvas(wxMouseEvent &ev);
  void onMouseMove(wxMouseEvent& ev);
  void OnMouseWheel(wxMouseEvent& event);
  void onSizing(::wxSizeEvent& ev);
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(drawCanvas, wxPanel)
    EVT_PAINT(drawCanvas::OnPaint)
    EVT_LEFT_DOWN(drawCanvas::onMouseDownOnCanvas)
    EVT_MOTION(drawCanvas::onMouseMove)
    EVT_MOUSEWHEEL(drawCanvas::OnMouseWheel)
    EVT_LEFT_UP(drawCanvas::onMouseUpOnCanvas)
END_EVENT_TABLE()

class rootDouble: public List<double>
{
  public:
  std::string DebugString;
  void ComputeDebugString(){this->DebugString=this->ElementToString();};
  void AssignString(std::string& input)
  { unsigned int startIndex=0;
    for (; startIndex<input.size(); startIndex++)
      if (input[startIndex]=='(')
        break;
    startIndex++;
    this->SetSizeExpandOnTopNoObjectInit(0);
    std::string tempS;
    tempS.resize(input.size());
    tempS="";
    for (; startIndex<input.size(); startIndex++)
    { if (input[startIndex]==')' || input[startIndex]==',')
      { std::stringstream tempstream;
        tempstream << tempS;
        tempstream.seekg(0);
        double coordinate;
        tempstream >> coordinate;
        tempS="";
        this->AddObjectOnTop(coordinate);
      } else
      { tempS.resize(tempS.size()+1);
        tempS[tempS.size()-1]=input[startIndex];
      }
      if (input[startIndex]==')')
        break;
    }
  };
  std::string ElementToString()
  { std::stringstream out;
    out << "(";
    for (int i=0; i<this->size; i++)
    { out << this->TheObjects[i];
      if (i!=this->size-1)
        out << ",";
    }
    out << ")";
    return out.str();
  };
  void MakeEi(int theDim, int theIndex)
  { this->SetSizeExpandOnTopNoObjectInit(theDim);
    for (int i=0; i<theDim; i++)
      this->TheObjects[i]=0;
    this->TheObjects[theIndex]=1;
  };
  rootDouble(){};
  void operator=(const root& other)
  { this->SetSizeExpandOnTopNoObjectInit(other.size);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i]=other.TheObjects[i].DoubleValue();
  };
  rootDouble(const rootDouble& other)
  { this->operator=(other);
  };
  rootDouble operator*(double coeff)
  { rootDouble result; result.SetSizeExpandOnTopNoObjectInit(this->size);
    for (int i=0; i<this->size; i++)
      result.TheObjects[i]= this->TheObjects[i]*coeff;
    return result;
  };
  rootDouble operator+(const rootDouble& other)
  { rootDouble result; result.SetSizeExpandOnTopNoObjectInit(this->size);
    for (int i=0; i<this->size; i++)
      result.TheObjects[i]= this->TheObjects[i]+ other.TheObjects[i];
    return result;
  };
  void operator*=(double coeff)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]*= coeff;
  };
  void operator+=(const rootDouble& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]+=other.TheObjects[i];
  };
  void operator/=(double coeff)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]/=coeff;
  };
  void operator=(const rootDouble& other)
  { this->List<double>::operator=(other);
  };
  void operator-=(const rootDouble& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]-=other.TheObjects[i];
  };
};

std::string ConvertWXstring(const wxString& input)
{ char* buf;
  int wantedSize=input.size();
  buf= new char[wantedSize*3+5];
  strcpy(buf, (const char*)input.mb_str(wxConvUTF8) );
  std::string tempS=buf;
  delete [] buf;
  return tempS;
}

wxString ConvertToWxString(const std::string& input)
{ wxString theString(input.c_str(), wxConvUTF8);
  return theString;
}

class RootSystemGraphics
{
  public:
  WeylGroup theWeyl;
  double shiftX, shiftY;
  double graphicsUnit;
  void Recompute();
  int SelectedIndex; //-2= none, -1=center of coordinate system, nonnegative integers= selectedindex
  rootDouble e1;
  rootDouble e2;
//  MutexWrapper onAnimatingAccess;
  bool Animating;
  rootDouble animationTargetE1;
  rootDouble animationTargetE2;
  rootDouble animationStartE1;
  rootDouble animationStartE2;
  Controller CantModifyMe;
  int ClickToleranceX;
  int ClickToleranceY;
  List<rootDouble> theRootSystem;
  List<rootDouble> theRootSystemProjectionsScaled;
  char DisplayBuffer[10000];
  std::string DebugString;
  std::string ElementToString()
  { std::stringstream out;
    out << "e1: " << this->e1.ElementToString() << "\n" << "e2: " << this->e2.ElementToString();
    return out.str();
  };
  void ComputeDebugString(){this->DebugString=this->ElementToString();};
//  List<List<double>> theBasis;
  void RotateOutOfPlane(std::stringstream& logger, rootDouble& input, rootDouble& output, rootDouble& orthoBasis1, rootDouble& orthoBasis2, double oldTanSquared, double newTanSquared);
  void click(int x, int y);
  double getScalarProduct(rootDouble& root1, rootDouble& root2)
  { int theDim= this->theWeyl.CartanSymmetric.NumRows;
    double result=0;
    for (int i=0; i<theDim; i++)
      for (int j=0; j<theDim; j++)
        result+=root1.TheObjects[i]*root2.TheObjects[j]*this->theWeyl.CartanSymmetric.elements[i][j].DoubleValue();
    return result;
  };
  void ScaleToUnitLength(rootDouble& theRoot);
  void ModifyToOrthonormalNoShiftSecond(rootDouble& root1, rootDouble& root2);
  void ComputeProjections()
  { for (int i=0; i<this->theRootSystemProjectionsScaled.size; i++)
    { this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0]=this->getScalarProduct(this->theRootSystem.TheObjects[i], this->e1)*this->graphicsUnit;
      this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1]=this->getScalarProduct(this->theRootSystem.TheObjects[i], this->e2)*this->graphicsUnit;
    }
  };
  void draw();
  bool AreWithinClickTolerance(int x1, int y1, int x2, int y2)
  { x1-=x2;
    y1-=y2;
    if (x1<0)
      x1=-x1;
    if (y1<0)
      y1=-y1;
    return x1<=this->ClickToleranceX && y1<=this->ClickToleranceY;
  };
  double  getAngleFromXandY(double x, double y, double neighborX, double neighborY);
  void changeBasis(double newX, double newY);
  void  mouseMove(int x, int y);
  RootSystemGraphics()
  { this->graphicsUnit=100;
    this->SelectedIndex=-2;
    this->Animating=false;
//    this->ComputingAnimation=false;
    this->shiftX=400;
    this->shiftY=200;
    this->ClickToleranceX=5;
    this->ClickToleranceY=5;
  };
};

class guiMainWindow : public wxFrame
{
  ListPointers<wxFont> theFonts;
public:
  std::fstream fileSettings;
  wxPaintEvent paintEvent;
  wxComboBox* ListBox1WeylGroup;
  RootSystemGraphics theGraphics;
  wxCommandEvent wxProgressReportEvent;
  wxCommandEvent wxComputationOver;
  wxBoxSizer* BoxSizer1VerticalBackground;
  wxBoxSizer* BoxSizer2HorizontalButtons;
  wxTextCtrl* Text1Output;
  wxStaticText* Static1E1;
  wxStaticText* Static2E2;
  //wxTextCtrl* Text4mutex;
  wxTextCtrl* Text2e1;
  wxTextCtrl* Text3e2;
 ::wxSpinCtrl* Spin1Dim;
  wxFont* theFont;
  ::drawCanvas* Canvas1;
  ::wxButton* Button1Animation;
  pthread_t AnimationThread;
  ::IndicatorWindowVariables progressReportVariables;
  //wxEVT_ProgressReport ProgressReportEvent;
  wxFont* GetFont(int theSize);
  //void DisplayUnlockedStatus() {this->Text4mutex->SetValue(wxT("Unlocked"));};
  //void DisplayLockedStatus(){this->Text4mutex->SetValue(wxT("Locked"));};

  void onRePaint(wxPaintEvent& ev);
  void onMouseDownOnCanvas(wxMouseEvent& ev);
  void onMouseUpOnCanvas(wxMouseEvent& ev);
  void onSpinner(wxSpinEvent & ev);
  void onListBox1Change(wxCommandEvent& ev);
  void onAnimation(wxCommandEvent& ev);
  void ReadSettingsIfAvailable();
  void WriteSettingsIfAvailable();
  void ArrangeWindows();
  void readSettingsWeylInfo();
  void Recompute();
  static void* DoTheAnimation(void* ptr);
  guiMainWindow();
  ~guiMainWindow();
  void OnExit(wxCloseEvent& event);
  enum
  { ID_MainWindow= 1001,
    ID_ListBox1,
    ID_Canvas1,
    ID_Button1,
    ID_ComputationUpdate,
    ID_Spin1Dim,
    ID_Paint,
  };
  DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(guiApp)

guiMainWindow *MainWindow1=0;
void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ if (MainWindow1!=0)
  { wxWindowDC dc(MainWindow1->Canvas1); wxPen tempPen;
    tempPen.SetWidth(2);
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
//	dc.setForeground(FXRGB(0,0,0));
//  dc.fillRectangle(0,0,MainWindow1->Canvas1DrawCanvas->getWidth(),MainWindow1->Canvas1DrawCanvas->getHeight());
}

void drawCircle(double X1, double Y1, double radius, int ColorIndex)
{ if (MainWindow1!=0)
  { wxWindowDC dc(MainWindow1->Canvas1); wxPen tempPen;
    tempPen.SetStyle(::wxSOLID);
    tempPen.SetColour(ColorIndex); dc.SetPen(tempPen);
    dc.DrawCircle((int)X1, (int)Y1, radius);
  }
//	dc.setForeground(FXRGB(0,0,0));
//  dc.fillRectangle(0,0,MainWindow1->Canvas1DrawCanvas->getWidth(),MainWindow1->Canvas1DrawCanvas->getHeight());
}

wxFont* guiMainWindow::GetFont(int theSize)
{ if (this->theFonts.size<theSize)
  { int oldsize=this->theFonts.size;
    this->theFonts.SetSizeExpandOnTopNoObjectInit(theSize);
    for (int i=oldsize; i<theSize; i++)
      this->theFonts.TheObjects[i]=0;
  }
  if (this->theFonts.TheObjects[theSize-1]==0)
    this->theFonts.TheObjects[theSize-1]= new wxFont(theSize,  wxDEFAULT, wxNORMAL, wxNORMAL);
  return this->theFonts.TheObjects[theSize-1];
}

void drawtext(double X1, double Y1, const char* text, int length, int color, int fontSize)
{ wxWindowDC dc(MainWindow1->Canvas1);
  dc.SetFont(*MainWindow1->GetFont(fontSize));
  dc.SetTextForeground(color);
  //dc.setcolo(color);
  //dc.setBackground(MainWindow1->Canvas1DrawCanvas->getBackColor());
  //dc(FILL_STIPPLED);
  wxString temptext(text, wxConvUTF8 ,length);
  dc.DrawText(temptext, (int)X1, (int)Y1);
}


void drawCanvas::OnMouseWheel(wxMouseEvent& event)
{ // scroll in drop down list using mouse wheel
  if (MainWindow1==0)
    return;
 	if (MainWindow1->theGraphics.Animating)
      return;
  int rot = event.GetWheelRotation()/event.GetWheelDelta();
  MainWindow1->theGraphics.graphicsUnit+=rot*5;
  //MainWindow1->Text4mutex->SetValue(wxT("locked"));
  MainWindow1->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  MainWindow1->theGraphics.ComputeProjections();
  MainWindow1->theGraphics.CantModifyMe.UnlockSafePoint();
  //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
  //wxPaintEvent temp;

  MainWindow1->Refresh();
}

std::string MainWindow1GlobalPath;

bool guiApp::OnInit()
{ char directoryCharacter;
#ifdef WIN32
  char path[500];
  ::GetModuleFileName(NULL,path,499);
  std::stringstream out;
  out<<path;
  ::MainWindow1GlobalPath= out.str();
  directoryCharacter='\\';
#else
  wxString path =this->argv[0];
  if (!wxIsAbsolutePath(path))
  { wxPathList pathlist;
    pathlist.AddEnvList(wxT("PATH"));
    path = pathlist.FindAbsoluteValidPath(path);
  }
  wxFileName filename(path);
  filename.Normalize();
  path = filename.GetFullPath();
  MainWindow1GlobalPath=path.mb_str();
  directoryCharacter='/';
#endif
  int pathCutOffSize=0;
  for (int i=MainWindow1GlobalPath.size();i>=0;i--)
  { if (::MainWindow1GlobalPath[i]==directoryCharacter)
    { pathCutOffSize=i+1;
      break;
		}
  }
  ::MainWindow1GlobalPath.resize(pathCutOffSize);
  MainWindow1 = new guiMainWindow;
  MainWindow1->Show(true);
  return true;
}

BEGIN_EVENT_TABLE(guiMainWindow, wxFrame)
    EVT_SIZING(drawCanvas::onSizing)
    EVT_SIZE(drawCanvas::onSizing)
    EVT_COMBOBOX(guiMainWindow::ID_ListBox1, guiMainWindow::onListBox1Change)
    EVT_CLOSE(guiMainWindow::OnExit)
    EVT_SPINCTRL(guiMainWindow::ID_Spin1Dim, guiMainWindow::onSpinner)
    EVT_BUTTON(guiMainWindow::ID_Button1, guiMainWindow::onAnimation)
    //EVT_PAINT(guiMainWindow::onPaint)
END_EVENT_TABLE()

void drawCanvas::onMouseDownOnCanvas(wxMouseEvent& ev)
{ if (MainWindow1==0)
    return;
 	if (MainWindow1->theGraphics.Animating)
    return;
  //MainWindow1->Text4mutex->SetValue(wxT("locked"));
  MainWindow1->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  MainWindow1->theGraphics.click(ev.GetX(), ev.GetY());
  MainWindow1->theGraphics.CantModifyMe.UnlockSafePoint();
  //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
}

void drawCanvas::onMouseUpOnCanvas(wxMouseEvent& ev)
{ if (MainWindow1==0)
    return;
  MainWindow1->theGraphics.SelectedIndex=-2;
}

void drawCanvas::onMouseMove(wxMouseEvent& ev)
{ if (MainWindow1==0)
    return;
 	if (MainWindow1->theGraphics.Animating)
      return;
  //MainWindow1->Text4mutex->SetValue(wxT("locked"));
  MainWindow1->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  MainWindow1->theGraphics.mouseMove(ev.GetX(), ev.GetY());
  MainWindow1->theGraphics.CantModifyMe.UnlockSafePoint();
  //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
  MainWindow1->Refresh();
}

void guiMainWindow::readSettingsWeylInfo()
{ if (MainWindow1->theGraphics.Animating)
    return;
  int tempI=this->ListBox1WeylGroup->GetCurrentSelection();
  char WeylLetter;
  switch(tempI)
  { case 0: WeylLetter='A'; break;
    case 1: WeylLetter='B'; break;
    case 2: WeylLetter='C'; break;
    case 3: WeylLetter='D'; break;
    case 4: WeylLetter='E'; break;
    case 5: WeylLetter='F'; break;
    case 6: WeylLetter='G'; break;
  }
//  MainWindow1->Text4mutex->SetValue(wxT("locked"));
  int candidateDim= this->Spin1Dim->GetValue();
  if (candidateDim>8)
  { candidateDim=8;
    this->Spin1Dim->SetValue(8);
  }
  if (candidateDim<1)
  { candidateDim=1;
    this->Spin1Dim->SetValue(1);
  }
//  MainWindow1->Text4mutex->SetValue(wxT("locked"));
  this->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  this->theGraphics.theWeyl.MakeArbitrary(WeylLetter, candidateDim);
  this->Recompute();
  this->theGraphics.CantModifyMe.UnlockSafePoint();
//  MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
  this->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  this->theGraphics.theWeyl.MakeArbitrary(WeylLetter, this->theGraphics.theWeyl.CartanSymmetric.NumCols);
  this->Recompute();
  this->theGraphics.CantModifyMe.UnlockSafePoint();
//  MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
}

void guiMainWindow::onListBox1Change(wxCommandEvent& ev)
{ this->readSettingsWeylInfo();
}

void guiMainWindow::onSpinner(wxSpinEvent & ev)
{ this->readSettingsWeylInfo();
}

void RootSystemGraphics::ScaleToUnitLength(rootDouble& theRoot)
{ double theLength=this->getScalarProduct(theRoot, theRoot);
  theLength=sqrt(theLength);
  theRoot/=theLength;
}

void RootSystemGraphics::ModifyToOrthonormalNoShiftSecond(rootDouble& root1, rootDouble& root2)
{ //if  (this->getScalarProduct(root2, root2)==0)
  //  root2.MakeEi(this->theWeyl.CartanSymmetric.NumRows,1);
  double theScalar= this->getScalarProduct(root1, root2)/this->getScalarProduct(root2, root2);
  root1-=root2*theScalar;
  this->ScaleToUnitLength(root1);
  this->ScaleToUnitLength(root2);
}

void RootSystemGraphics::click(int x, int y)
{ x-=this->shiftX;
  y=this->shiftY-y;
  this->SelectedIndex=-2;
  if (AreWithinClickTolerance(x,y,0,0))
    this->SelectedIndex=-1;
  int theDim=this->theWeyl.CartanSymmetric.NumRows;
  for (int i=0; i<theDim; i++)
    if (this->AreWithinClickTolerance(x, y, (int) this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0], (int) this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1] ))
    { this->SelectedIndex=i;
      return;
    }
}

double RootSystemGraphics::getAngleFromXandY(double x, double y, double neighborX, double neighborY)
{ double result;
  if (x!=0)
   result= atan(y/x);
  else
    if (y>0)
      result= M_PI/2;
    else
      result= M_PI/(-2);
  return result;
}

void RootSystemGraphics::RotateOutOfPlane(std::stringstream& logger, rootDouble& input, rootDouble& output, rootDouble& orthoBasis1, rootDouble& orthoBasis2, double oldTanSquared, double newTanSquared)
{ rootDouble projection= orthoBasis1;
  rootDouble vComponent= input;
  double scal1= this->getScalarProduct(orthoBasis1, input);
  double scal2= this->getScalarProduct(orthoBasis2, input);
  projection*=scal1;
  projection+= orthoBasis2*scal2;
  vComponent-=projection;
  logger << "\ngetScalarProd=" << this->getScalarProduct(projection, vComponent);
  if (oldTanSquared<0 || newTanSquared<0)
    return;
  double oldAngle=atan(sqrt(oldTanSquared));
  double newAngle=atan(sqrt(newTanSquared));
  double angleChange=-oldAngle+newAngle;
  projection=orthoBasis1;
  projection*=cos(angleChange)*scal1-sin(angleChange)*scal2;
  projection+=orthoBasis2*(sin(angleChange)*scal1+sin(angleChange)*scal2);
  output = vComponent;
  output+=projection;
}

void RootSystemGraphics::changeBasis(double newX, double newY)
{ if (newX==0 && newY==0)
    return;
 	if (MainWindow1->theGraphics.Animating)
    return;
  std::stringstream out;
  rootDouble& selectedRoot=this->theRootSystem.TheObjects[this->SelectedIndex];
  double selectedRootLength=this->getScalarProduct(selectedRoot, selectedRoot);
  double oldX=this->theRootSystemProjectionsScaled.TheObjects[this->SelectedIndex].TheObjects[0]/this->graphicsUnit;
  double oldY=this->theRootSystemProjectionsScaled.TheObjects[this->SelectedIndex].TheObjects[1]/this->graphicsUnit;
  newX/=this->graphicsUnit;
  newY/=this->graphicsUnit;
  double oldAngle= getAngleFromXandY(oldX, oldY, newX, newY);
  double newAngle= getAngleFromXandY(newX, newY, oldX, oldY);
  double AngleChange= -newAngle+oldAngle;
  double epsilon=0.000000000001;
  while (AngleChange>M_PI/2+epsilon)
  { AngleChange-=M_PI;}
  while (AngleChange<=M_PI/(-2)-epsilon)
  { AngleChange+=M_PI;}
  out << "\nold angle:" << oldAngle;
  out << "\nnew angle: " << newAngle;
  rootDouble NewVectorE1, NewVectorE2;
  //NewVectorE1.ComputeDebugString();
  NewVectorE1= this->e1*cos(AngleChange);
  //NewVectorE1.ComputeDebugString();
  NewVectorE1+=this->e2*sin(AngleChange);
  //NewVectorE1.ComputeDebugString();
  //NewVectorE1.ComputeDebugString();
  //NewVectorE2.ComputeDebugString();
  NewVectorE2= this->e2*cos(AngleChange);
  //NewVectorE2.ComputeDebugString();
  NewVectorE2+=this->e1*(-sin(AngleChange));
  //NewVectorE2.ComputeDebugString();
  //NewVectorE2.ComputeDebugString();
  this->e1=NewVectorE1;
  this->e2=NewVectorE2;
  this->e1.ComputeDebugString();
  this->e2.ComputeDebugString();
  double RootTimesE1=getScalarProduct(selectedRoot, this->e1);
  double RootTimesE2=getScalarProduct(selectedRoot, this->e2);
  rootDouble vOrthogonal=selectedRoot;
  rootDouble vProjection=this->e1*RootTimesE1;
  vProjection+=this->e2*RootTimesE2;
  vOrthogonal-= vProjection;
  double oldRatioProjectionOverHeightSquared = (oldX*oldX+oldY*oldY)/ (selectedRootLength-oldX*oldX-oldY*oldY);
  double newRatioProjectionOverHeightSquared = (newX*newX+newY*newY)/ (selectedRootLength-newX*newX-newY*newY);
  out << "\noldRatio: " << oldRatioProjectionOverHeightSquared;
  out << "\nnewRatio: " << newRatioProjectionOverHeightSquared;
  if (this->getScalarProduct(vOrthogonal, vOrthogonal)>epsilon || this->getScalarProduct(vOrthogonal, vOrthogonal)<-epsilon)
  { this->ScaleToUnitLength(vProjection);
    this->ScaleToUnitLength(vOrthogonal);
    out << "\nscaled vOrthogonal=" << vOrthogonal.ElementToString() << "->" << this->getScalarProduct(vOrthogonal, vOrthogonal);
    out << "\nscaled vProjection=" << vProjection.ElementToString() << "->" <<this->getScalarProduct(vProjection, vProjection);
    out << "\ntheScalarProd: " << this->getScalarProduct(vOrthogonal, vProjection);
    this->RotateOutOfPlane(out, this->e1, this->e1, vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared);
    this->RotateOutOfPlane(out, this->e2, this->e2, vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared);
  }
//  this->e1.ComputeDebugString();
//  this->e2.ComputeDebugString();
  this->ModifyToOrthonormalNoShiftSecond(this->e1, this->e2);
//  this->e1.ComputeDebugString();
//  this->e2.ComputeDebugString();
  out << "\ne1=" << this->e1.ElementToString();
  out << "\ne2=" << this->e2.ElementToString();
  out << "\ne1*e2=" << this->getScalarProduct(this->e1, this->e2);
  this->ComputeProjections();
  this->DebugString= out.str();
}

void RootSystemGraphics::mouseMove(int X, int Y)
{ if (this->SelectedIndex==-2)
    return;
  if (this->SelectedIndex==-1)
  { this->shiftX=X;
    this->shiftY=Y;
  }
  if (this->SelectedIndex>=0)
    this->changeBasis(double (this->shiftX-X), double (Y-this->shiftY));
//  this->draw();
}

void RootSystemGraphics::draw()
{ //this->CantModifyMe.LockMe();
  this->CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  Rational RootLength;
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
  { int tempColor= CGIspecificRoutines::RedGreenBlue(200, 200, 255);
    RootLength= this->theWeyl.RootScalarCartanRoot(this->theWeyl.RootSystem.TheObjects[i], this->theWeyl.RootSystem.TheObjects[i]);
    if (this->theWeyl.LongRootLength== RootLength)
      tempColor=CGIspecificRoutines::RedGreenBlue(50, 50, 255);
    drawline(shiftX, shiftY, this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0]+this->shiftX, this->shiftY-this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1], DrawingVariables::PenStyleNormal, tempColor);
  }
  int theDim=this->theWeyl.CartanSymmetric.NumRows;
  for (int i=0; i<theDim; i++)
  { drawCircle(shiftX+this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0], shiftY-this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1], 3, CGIspecificRoutines::RedGreenBlue(200, 50, 50) );
    std::stringstream tempChar;
    tempChar << (i+1);
    std::string tempS=tempChar.str();
    drawtext(shiftX+this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0]+5, shiftY-this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1]+5, tempS.c_str(), 1, CGIspecificRoutines::RedGreenBlue(50, 50, 50), 9);
  }
  //this->CantModifyMe.UnlockMe();
}

void RootSystemGraphics::Recompute()
{ if (this->Animating)
    return;
  this->theWeyl.ComputeRho(true);
  int theDim=this->theWeyl.CartanSymmetric.NumRows;
  this->e1.SetSizeExpandOnTopNoObjectInit(theDim);
  this->e2.SetSizeExpandOnTopNoObjectInit(theDim);
  int numRoots=this->theWeyl.RootSystem.size;
  this->theRootSystem.initFillInObject(numRoots, this->e1);
  for (int i=0; i<numRoots; i++)
    for (int j=0; j<theDim; j++)
      this->theRootSystem.TheObjects[i].TheObjects[j]=this->theWeyl.RootSystem.TheObjects[i].TheObjects[j].DoubleValue();
//  this->theBasis.SetSizeExpandOnTopNoObjectInit(theDim);
  for (int i=0; i<theDim; i++)
  { this->e1.TheObjects[i]=((i-1.3)*sqrt(i+1.3)-1);
    this->e2.TheObjects[i]=(i*3+1.7);
  }
  rootDouble tempRoot;
  tempRoot.SetSizeExpandOnTopNoObjectInit(2);
  this->theRootSystemProjectionsScaled.initFillInObject(numRoots, tempRoot);
  this->ModifyToOrthonormalNoShiftSecond(this->e1, this->e2);
  this->animationTargetE1.MakeEi(theDim, 0);
  this->animationTargetE2.MakeEi(theDim, 1);
  this->animationStartE1=this->e1;
  this->animationStartE2=this->e2;
  this->ComputeProjections();
}

void guiMainWindow::Recompute()
{ this->theGraphics.Recompute();
  this->Text2e1->SetValue(ConvertToWxString(this->theGraphics.animationTargetE1.ElementToString()));
  this->Text3e2->SetValue(ConvertToWxString(this->theGraphics.animationTargetE2.ElementToString()));
  wxPaintEvent temp;
  this->onRePaint(temp);
}

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output);

guiMainWindow::guiMainWindow(): wxFrame((wxFrame *)NULL, guiMainWindow::ID_MainWindow, wxT("Root systems"), wxPoint(100,100), wxSize(800,600), wxRESIZE_BORDER| wxCAPTION | wxSYSTEM_MENU| wxCLOSE_BOX | wxMINIMIZE_BOX)
{ //this->theComputationSetup.flagDoCustomComputation=true;
  this->BoxSizer1VerticalBackground = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer2HorizontalButtons= new ::wxBoxSizer(wxHORIZONTAL);
  this->Button1Animation= new wxButton(this, guiMainWindow::ID_Button1, wxT("Animate"));
  this->Canvas1 = new ::drawCanvas(this,::wxID_ANY,::wxDefaultPosition,::wxDefaultSize,::wxEXPAND|wxALL);
  this->theFont= new ::wxFont(10, wxDEFAULT, wxNORMAL,wxNORMAL);
  this->Spin1Dim = new wxSpinCtrl(this,this->ID_Spin1Dim);
  this->Text1Output= new ::wxTextCtrl(this,::wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  this->ListBox1WeylGroup= new ::wxComboBox(this, this->ID_ListBox1, wxT("B"), wxPoint(0, 0), wxDefaultSize, 0, 0, wxCB_DROPDOWN);
  this->Text2e1 = new wxTextCtrl(this, wxID_ANY, wxT("(1,0,0)"));//, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  this->Text3e2 = new wxTextCtrl(this, wxID_ANY, wxT("(0,1,0)"));
//  this->Text4mutex = new wxTextCtrl(this, wxID_ANY);
  this->Static1E1 = new wxStaticText(this, wxID_ANY, wxT("Animation targets e1 | e2:"));
  this->Static2E2 = new wxStaticText(this, wxID_ANY, wxT("|"));
  this->BoxSizer2HorizontalButtons->Add(this->ListBox1WeylGroup);
  this->BoxSizer2HorizontalButtons->Add(this->Spin1Dim);
  this->BoxSizer2HorizontalButtons->Add(this->Button1Animation);
  this->BoxSizer2HorizontalButtons->Add(this->Static1E1);
  this->BoxSizer2HorizontalButtons->Add(this->Text2e1);
  this->BoxSizer2HorizontalButtons->Add(this->Static2E2);
  this->BoxSizer2HorizontalButtons->Add(this->Text3e2);
  //this->BoxSizer2HorizontalButtons->Add(this->Text4mutex);
  this->BoxSizer1VerticalBackground->Add(this->BoxSizer2HorizontalButtons, 0, wxEXPAND|::wxALL);
  this->BoxSizer1VerticalBackground->Add(this->Canvas1, 5, wxEXPAND|::wxALL);
  this->BoxSizer1VerticalBackground->Add(this->Text1Output, 2, wxEXPAND|wxALL);
  this->SetSizer(this->BoxSizer1VerticalBackground);
  this->ListBox1WeylGroup->Append(wxT("A"));
  this->ListBox1WeylGroup->Append(wxT("B"));
  this->ListBox1WeylGroup->Append(wxT("C"));
  this->ListBox1WeylGroup->Append(wxT("D"));
  this->ListBox1WeylGroup->Append(wxT("E"));
  this->ListBox1WeylGroup->Append(wxT("F4"));
  this->ListBox1WeylGroup->Append(wxT("G2"));
  this->ListBox1WeylGroup->SetSelection(1);
  this->Spin1Dim->SetValue(3);
  this->theGraphics.theWeyl.MakeArbitrary('B', 3);
  this->ReadSettingsIfAvailable();
  this->theGraphics.Recompute();
  this->Refresh();
  //Centre();
}

void drawCanvas::onSizing(wxSizeEvent& ev)
{ if (MainWindow1==0)
		return;
 	if (MainWindow1->theGraphics.Animating)
   return;
  //MainWindow1->Text4mutex->SetValue(wxT("locked"));
  MainWindow1->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  MainWindow1->Layout();
  MainWindow1->theGraphics.draw();
  MainWindow1->theGraphics.CantModifyMe.UnlockSafePoint();
  //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
  this->Refresh();
}

guiMainWindow::~guiMainWindow()
{ //this->theFont
  this->WriteSettingsIfAvailable();
  this->Canvas1->Destroy();
  this->theFonts.KillAllElements();
  this->fileSettings.close();
}


void guiMainWindow::onRePaint(wxPaintEvent& ev)
{ wxPaintDC dc;
  this->Refresh();
}

void drawCanvas::OnPaint(::wxPaintEvent& ev)
{ wxPaintDC  dc(this);
	if (MainWindow1==0)
		return;
  //MainWindow1->Text4mutex->SetValue(wxT("locked"));
  MainWindow1->theGraphics.CantModifyMe.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  dc.SetBackground(MainWindow1->GetBackgroundColour());
  dc.DrawRectangle(wxPoint(0,0), this->GetSize());
  MainWindow1->theGraphics.draw();
  MainWindow1->Text1Output->SetValue(ConvertToWxString(MainWindow1->theGraphics.DebugString));
  MainWindow1->theGraphics.CantModifyMe.UnlockSafePoint();
  //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
}

void guiMainWindow::OnExit(wxCloseEvent &event)
{	this->Destroy();
}

double GetElapsedTimeInSeconds(timeval& StartingTime)
{ timeval tempTime;
  gettimeofday(&tempTime, NULL);
  int miliSeconds =(tempTime.tv_sec- StartingTime.tv_sec)*1000+(tempTime.tv_usec- StartingTime.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

void* guiMainWindow::DoTheAnimation(void* ptr)
{ MainWindow1->theGraphics.Animating=true;
  timeval startingTime;
  gettimeofday(&startingTime, NULL);
  RootSystemGraphics& theGraphics=MainWindow1->theGraphics;
  //MainWindow1->Text4mutex->SetValue(wxT("locked"));
  theGraphics.CantModifyMe.InitComputation();
  std::string inputStringE1= ConvertWXstring(MainWindow1->Text2e1->GetValue());
  std::string inputStringE2= ConvertWXstring(MainWindow1->Text3e2->GetValue());
  theGraphics.animationTargetE1.AssignString(inputStringE1);
  theGraphics.animationTargetE2.AssignString(inputStringE2);
  rootDouble& startingE1= theGraphics.animationStartE1;
  rootDouble& startingE2= theGraphics.animationStartE2;
  startingE1=theGraphics.e1;
  startingE2=theGraphics.e2;
  startingE1.ComputeDebugString();
  startingE2.ComputeDebugString();
  MainWindow1->theGraphics.ModifyToOrthonormalNoShiftSecond(theGraphics.animationTargetE1, theGraphics.animationTargetE2);
  theGraphics.animationTargetE1.ComputeDebugString();
  theGraphics.animationTargetE2.ComputeDebugString();
  MainWindow1->Text2e1->SetValue(ConvertToWxString(theGraphics.animationTargetE1.DebugString));
  MainWindow1->Text3e2->SetValue(ConvertToWxString(theGraphics.animationTargetE2.DebugString));
  usleep(100000);
  MainWindow1->Layout();
  MainWindow1->Refresh();
  usleep(100000);
  MainWindow1->theGraphics.CantModifyMe.SafePoint();
  //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));
  int numRuns=100;
  for (int i=0; i<numRuns; i++)
  { usleep(100000);
    if (i==1)
      MainWindow1->Refresh();
    double a=((double) (i+1))/ ((double) numRuns);
    double b=1.0-a;
    theGraphics.e1= theGraphics.animationStartE1*b+theGraphics.animationTargetE1*a;
    theGraphics.e2= theGraphics.animationStartE2*b+theGraphics.animationTargetE2*a;
    theGraphics.e1.ComputeDebugString();
    theGraphics.e2.ComputeDebugString();
    theGraphics.ModifyToOrthonormalNoShiftSecond(MainWindow1->theGraphics.e1, MainWindow1->theGraphics.e2);
    theGraphics.e1.ComputeDebugString();
    theGraphics.e2.ComputeDebugString();
    theGraphics.ComputeProjections();
    theGraphics.ComputeDebugString();
    std::stringstream tempOutput;
    tempOutput << "\na: " << a << " b: " << b << "\n" << "starting e1: " << startingE1.DebugString << "\nstarting e2: " << startingE2.DebugString;
    tempOutput << "\ntarget e1: " << theGraphics.animationTargetE1.DebugString << "\ntarget e2: " << theGraphics.animationTargetE2.DebugString;
    MainWindow1->theGraphics.DebugString.append(tempOutput.str());
    usleep(10000);
//    MainWindow1->();
    MainWindow1->Refresh();
//    MainWindow1->onRePaint(MainWindow1->paintEvent);
    usleep(10000);
    //MainWindow1->GetEventHandler()->AddPendingEvent(MainWindow1->paintEvent);
    MainWindow1->theGraphics.CantModifyMe.SafePoint();
    //MainWindow1->Text4mutex->SetValue(wxT("unlocked"));

  }
  theGraphics.CantModifyMe.ExitComputation();
  MainWindow1->theGraphics.Animating=false;
  usleep(100000);
  pthread_exit(NULL);
}

void guiMainWindow::onAnimation(wxCommandEvent& ev)
{ pthread_create(&this->AnimationThread, NULL, &this->DoTheAnimation, 0);
}

void guiMainWindow::WriteSettingsIfAvailable()
{ if (this->fileSettings.is_open())
  { this->fileSettings.clear();
    this->fileSettings.seekp(0);
    int x,y;
    this->GetPosition(&x, &y);
    this->fileSettings  <<"  Main_window_top_left_corner_x " <<x<<"   \nMain_window_top_left_corner_y " <<y<<"    ";
    this->GetSize(& x, & y);
    this->fileSettings  << "  \nMain_window_width " <<x<<"   \nMain_window_height "  <<y<<"    ";
    this->fileSettings.flush();
  }
}

void guiMainWindow::ReadSettingsIfAvailable()
{ if (::MainWindow1GlobalPath=="")
		return;
  std::stringstream out;
  out << ::MainWindow1GlobalPath << "root-system-settings.txt";
  std::string tempS;
  tempS= out.str();
  if (rootFKFTcomputation::OpenDataFileOrCreateIfNotPresent2(this->fileSettings,tempS,false,false))
  { wxPoint tempPt, tempPt2, tempPt3;
    wxSize tempSize, tempSize2, tempSize3;
    this->fileSettings.seekg(0);
    this->fileSettings>>tempS >> tempPt.x;
    this->fileSettings>>tempS >> tempPt.y;
    this->fileSettings>>tempS >> tempSize.x;
    this->fileSettings>>tempS >> tempSize.y;
    this->SetPosition(tempPt);
    this->SetSize(tempSize);
  }
}

void guiMainWindow::ArrangeWindows()
{ this->ReadSettingsIfAvailable();
}

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output)
{
}


#endif

