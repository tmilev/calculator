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
  void onSizing(::wxSizeEvent&ev);
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
  rootDouble(){};
  rootDouble(const rootDouble& other)
  { this->operator=(other);
  };
  rootDouble operator*(double coeff)
  { rootDouble result; result.SetSizeExpandOnTopNoObjectInit(this->size);
    for (int i=0; i<this->size; i++)
      result.TheObjects[i]= this->TheObjects[i]*coeff;
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
  int ClickToleranceX;
  int ClickToleranceY;
  List<rootDouble> theRootSystem;
  List<rootDouble> theRootSystemProjectionsScaled;
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
  wxComboBox* ListBox1WeylGroup;
  RootSystemGraphics theGraphics;
  wxCommandEvent wxProgressReportEvent;
  wxCommandEvent wxComputationOver;
  wxBoxSizer* BoxSizer1VerticalBackground;
  wxBoxSizer* BoxSizer2HorizontalButtons;
  wxTextCtrl* Text1Output;
 ::wxSpinCtrl* Spin1Dim;
  wxFont* theFont;
  ::drawCanvas* Canvas1;
  ::wxToggleButton* ToggleButton1UsingCustom;
  ::wxToggleButton* ToggleButton2ViewCombinatorialChambers;
  ::IndicatorWindowVariables progressReportVariables;
  //wxEVT_ProgressReport ProgressReportEvent;
  wxFont* GetFont(int theSize);
  void onRePaint(wxPaintEvent& ev);
  void onMouseDownOnCanvas(wxMouseEvent& ev);
  void onMouseUpOnCanvas(wxMouseEvent& ev);
  void onSpinner(wxSpinEvent & ev);
  void onListBox1Change(wxCommandEvent& ev);
  void ReadSettingsIfAvailable();
  void WriteSettingsIfAvailable();
  void ArrangeWindows();
  void Recompute();
  guiMainWindow();
  ~guiMainWindow();
  void OnExit(wxCloseEvent& event);
  enum
  { ID_MainWindow= 1001,
    ID_ListBox1,
    ID_Canvas1,
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
  int rot = event.GetWheelRotation()/event.GetWheelDelta();
  MainWindow1->theGraphics.graphicsUnit+=rot*5;
  MainWindow1->theGraphics.ComputeProjections();
  wxPaintEvent temp;
  MainWindow1->onRePaint(temp);
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
    //EVT_PAINT(guiMainWindow::onPaint)
END_EVENT_TABLE()

void drawCanvas::onMouseDownOnCanvas(wxMouseEvent& ev)
{ if (MainWindow1==0)
    return;
  int Realx=ev.GetX();//-this->Canvas1->GetRect().GetTop();
  int Realy=ev.GetY();//-this->GetRect().GetLeft();
  MainWindow1->theGraphics.click(ev.GetX(), ev.GetY());
}

void drawCanvas::onMouseUpOnCanvas(wxMouseEvent& ev)
{ if (MainWindow1==0)
    return;
  MainWindow1->theGraphics.SelectedIndex=-2;
}

void drawCanvas::onMouseMove(wxMouseEvent& ev)
{ if (MainWindow1==0)
    return;
  MainWindow1->theGraphics.mouseMove(ev.GetX(), ev.GetY());
  wxPaintEvent temp;
  MainWindow1->onRePaint(temp);
}

void guiMainWindow::onListBox1Change(wxCommandEvent& ev)
{ int tempI=this->ListBox1WeylGroup->GetCurrentSelection();
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
  this->theGraphics.theWeyl.MakeArbitrary(WeylLetter, this->theGraphics.theWeyl.CartanSymmetric.NumCols);
  this->Recompute();
}

void guiMainWindow::onSpinner(wxSpinEvent & ev)
{ int candidateDim= this->Spin1Dim->GetValue();
  if (candidateDim>8)
  { candidateDim=8;
    this->Spin1Dim->SetValue(8);
  }
  if (candidateDim<1)
  { candidateDim=1;
     this->Spin1Dim->SetValue(1);
  }
  this->theGraphics.theWeyl.MakeArbitrary(this->theGraphics.theWeyl.WeylLetter, candidateDim);
  this->Recompute();
}

void RootSystemGraphics::ScaleToUnitLength(rootDouble& theRoot)
{ double theLength=this->getScalarProduct(theRoot, theRoot);
  theLength=sqrt(theLength);
  theRoot/=theLength;
}

void RootSystemGraphics::ModifyToOrthonormalNoShiftSecond(rootDouble& root1, rootDouble& root2)
{ double theScalar= this->getScalarProduct(root1, root2)/this->getScalarProduct(root2, root2);
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
{ for (int i=0; i<this->theWeyl.RootSystem.size; i++)
    drawline(shiftX, shiftY, this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0]+this->shiftX, this->shiftY-this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1], DrawingVariables::PenStyleNormal, 0);
  int theDim=this->theWeyl.CartanSymmetric.NumRows;
  for (int i=0; i<theDim; i++)
  { drawCircle(shiftX+this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0], shiftY-this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1], 3, CGIspecificRoutines::RedGreenBlue(200, 50, 50) );
    std::stringstream tempChar;
    tempChar << (i+1);
    std::string tempS=tempChar.str();
    drawtext(shiftX+this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[0]+5, shiftY-this->theRootSystemProjectionsScaled.TheObjects[i].TheObjects[1]+5, tempS.c_str(), 1, CGIspecificRoutines::RedGreenBlue(50, 50, 50), 9);
  }
}

void RootSystemGraphics::Recompute()
{ this->theWeyl.ComputeRho(true);
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
  { this->e1.TheObjects[i]=(i+1.3)*(i+1.3)+1;
    this->e2.TheObjects[i]=i*3+1.7;
  }
  rootDouble tempRoot;
  tempRoot.SetSizeExpandOnTopNoObjectInit(2);
  this->theRootSystemProjectionsScaled.initFillInObject(numRoots, tempRoot);
  this->ModifyToOrthonormalNoShiftSecond(this->e1, this->e2);
  this->ComputeProjections();
}

void guiMainWindow::Recompute()
{ this->theGraphics.Recompute();
  wxPaintEvent temp;
  this->onRePaint(temp);
}

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output);

guiMainWindow::guiMainWindow(): wxFrame((wxFrame *)NULL, guiMainWindow::ID_MainWindow, wxT("Root systems"), wxPoint(100,100), wxSize(800,600), wxRESIZE_BORDER| wxCAPTION | wxSYSTEM_MENU| wxCLOSE_BOX | wxMINIMIZE_BOX)
{ //this->theComputationSetup.flagDoCustomComputation=true;
  this->BoxSizer1VerticalBackground = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer2HorizontalButtons= new ::wxBoxSizer(wxHORIZONTAL);
  this->Canvas1 = new ::drawCanvas(this,::wxID_ANY,::wxDefaultPosition,::wxDefaultSize,::wxEXPAND|wxALL);
  this->theFont= new ::wxFont(10, wxDEFAULT, wxNORMAL,wxNORMAL);
  this->Spin1Dim = new wxSpinCtrl(this,this->ID_Spin1Dim);
  this->Text1Output= new ::wxTextCtrl(this,::wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  this->ListBox1WeylGroup= new ::wxComboBox(this, this->ID_ListBox1, wxT("B"), wxPoint(0, 0), wxDefaultSize, 0, 0, wxCB_DROPDOWN);
  this->BoxSizer2HorizontalButtons->Add(this->ListBox1WeylGroup);
  this->BoxSizer2HorizontalButtons->Add(this->Spin1Dim);
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
  MainWindow1->Layout();
  MainWindow1->theGraphics.draw();
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
  dc.SetBackground(MainWindow1->GetBackgroundColour());
  dc.DrawRectangle(wxPoint(0,0), this->GetSize());
  MainWindow1->theGraphics.draw();
  wxString temptext(MainWindow1->theGraphics.DebugString.c_str(), wxConvUTF8);
  MainWindow1->Text1Output->SetValue(temptext);
}

void guiMainWindow::OnExit(wxCloseEvent &event)
{	this->Destroy();
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

