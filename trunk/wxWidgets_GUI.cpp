//*********************************************************************************************************
//*********************************************************************************************************
//file wxWidgets_GUI.cpp
//Author: Todor Milev
//GUI for using the Polyhedra.cpp and polyhedra.h files.
//Uses wxWidgets. http://www.wxwidgets.org/
//wxWidgets is an open source (completely free and modifiable) toolkit.
//Many thanks to the wxWidgets team!
//*********************************************************************************************************
//*********************************************************************************************************

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

#include "polyhedra.h"

//#include "to_be_merged_in_main.cpp"
extern void main_test_function(std::string& output, GlobalVariables& theGlobalVariables);


extern DrawingVariables TDV;

class guiMainWindow;
class wxDialogOutput;
class guiApp : public wxApp
{
public:
  bool OnInit();
};

class wxGridExtra : public wxGrid
{
public:
	int maxNumCols;
	int maxNumRows;
	void SetNumRowsAndCols(int r, int c);
	wxGridExtra( wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString& name = wxT("") );
};

class WorkThreadClass
{
public:
#ifdef WIN32
	HANDLE ComputationalThread;
#else
	pthread_t ComputationalThreadLinux;
#endif
  bool isRunning;
  bool CriticalSectionWorkThreadEntered;
  bool CriticalSectionPauseButtonEntered;
  void run();
};

class wxComboBoxWheel : public wxComboBox
{
public:
  wxComboBoxWheel( wxWindow *parent, wxWindowID id, const wxString& value= wxT(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const int n=0, const wxString choices[]=0, const long style = wxWANTS_CHARS, const wxValidator& validator=wxDefaultValidator, const wxString& name = wxT(""));
  void OnMouseWheel(wxMouseEvent& event);
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxComboBoxWheel, wxComboBox)
    EVT_MOUSEWHEEL(wxComboBoxWheel::OnMouseWheel)
END_EVENT_TABLE()

wxComboBoxWheel::wxComboBoxWheel( wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const int n ,const wxString choices[], const long style, const wxValidator& validator, const wxString& name ):wxComboBox(parent, id, value, pos, size, n, choices, style, validator, name)
{
}

DECLARE_EVENT_TYPE(wxEVT_ProgressReport, -1)
DEFINE_EVENT_TYPE(wxEVT_ProgressReport)
DECLARE_EVENT_TYPE(wxEVT_ComputationFinished, -1)
DEFINE_EVENT_TYPE(wxEVT_ComputationFinished)

class drawCanvas : public ::wxPanel
{
public:
  void OnPaint(wxPaintEvent& ev);
  int ClickToleranceX;
  int ClickToleranceY;
  drawCanvas
    ( wxWindow *parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER, const wxString& name = wxPanelNameStr)
  {	this->Create(parent,winid,pos,size,style,name);
  };
  void onMouseDownOnCanvas(wxMouseEvent &ev);
  void onSizing(::wxSizeEvent&ev);
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( drawCanvas, wxPanel )
    EVT_PAINT(drawCanvas::OnPaint)
    EVT_LEFT_DOWN( drawCanvas::onMouseDownOnCanvas)
END_EVENT_TABLE()

class guiMainWindow : public wxFrame
{
public:
  std::fstream fileSettings;
  std::string bufferString1;
  ComputationSetup theComputationSetup;
  wxGridExtra *Table1Input;
  wxGridExtra *Table2Indicator;
  wxGridExtra *Table3Values;
  int NumVectors;
  int MaxAllowedVectors;
  wxCommandEvent wxProgressReportEvent;
  wxCommandEvent wxComputationOver;
  WorkThreadClass WorkThread1;
  wxBoxSizer* BoxSizer1HorizontalBackground;
  wxBoxSizer* BoxSizer2VerticalInputs;
  wxBoxSizer* BoxSizer3HorizontalInputButtons;
  wxBoxSizer* BoxSizer4VerticalToggleButton1;
  wxBoxSizer* BoxSizer5VerticalCanvasAndProgressReport;
  wxBoxSizer* BoxSizer6HorizontalInputsBlock;
  wxBoxSizer* BoxSizer7VerticalListBox1;
  wxBoxSizer* BoxSizer8HorizontalEval;
  wxBoxSizer* BoxSizer9HorizontalCombo1AndMainButton;
  wxBoxSizer* BoxSizer10HorizontalProgressReportsAndOptions;
  wxBoxSizer* BoxSizer11VerticalOptions;
  wxBoxSizer* BoxSizer12VerticalProgressReports;
  wxBoxSizer* BoxSizer13VerticalPartFracOutput;
  wxBoxSizer* BoxSizer14HorizontalSaveButtons;
  wxBoxSizer* BoxSizer15HorizontalSlicingButtons;
  wxBoxSizer* BoxSizer16VerticalStatusString;
  wxBoxSizer* BoxSizer17HorizontalProverButtons;
 // wxRadioButton* RB1OneSlice;
 // wxRadioButton* RB2OneIncrement;
 // wxRadioButton* RB3FullChop;
  wxRadioBox* RBGroup1SlicingOptions;
  wxTextCtrl* Text1Output;
  wxTextCtrl* Text2Values;
  wxTextCtrl* Text3PartialFractions;
  wxTextCtrl* Text4StatusString1;
  wxDialogOutput* Dialog1OutputPF;
  wxDialogOutput* Dialog2StatusString1;
  wxFont* theFont;
  ::wxStaticText* Label1ProgressReport;
  ::wxStaticText* Label2ProgressReport;
  ::wxStaticText* Label3ProgressReport;
  ::wxStaticText* Label4ProgressReport;
  ::wxStaticText* Label5ProgressReport;
  ::wxStaticText* Label6ProgressReport;
  ::wxStaticText* Label7Dim;
  ::wxStaticText* Label8NumVect;
  ::wxStaticText* Label9OutputVPF;
  ::wxComboBoxWheel* ListBox1WeylGroup;
  ::wxButton* Button1Go;
  ::wxButton* Button2Eval;
  ::wxButton* Button3Custom;
  ::wxButton* Button4SaveReadable;
  ::wxButton* Button5SaveComputer;
  ::wxButton* Button6OneSlice;
  ::wxButton* Button7OneDirectionIncrement;
  ::wxButton* Button8FullChopping;
	::wxButton* Button9CustomNilradical;
	::wxButton* Button10ProverOneStep;
	::wxButton* Button11ProverFullComputation;
	::wxButton* Button12ProverOneStepFixedK;
	::wxButton* Button13ProverFullComputationFixedK;
	::wxButton* Button14ProverFixedKSave;
	::wxButton* Button15ProverFixedKOpen;
	::wxButton* Button16Custom2;
	::wxButton* Button17Custom2PauseSaveResume;
	::wxButton* Button18Custom2Load;
  ::wxSpinCtrl* Spin1Dim;
  ::wxSpinCtrl* Spin2NumVect;
  ::wxCheckBox* CheckBox1ComputePFs;
  ::wxCheckBox* CheckBox2CheckSums;
  ::wxCheckBox* CheckBox3ComputeChambers;
  ::wxCheckBox* CheckBox4ChamberLabels;
  ::wxCheckBox* CheckBox5InvisibleChambers;
  ::wxCheckBox* CheckBox6Dashes;
  ::wxCheckBox* CheckBox7UseIndicatorForPFDecomposition;
  ::wxCheckBox* CheckBox8DoTheWeylGroup;
  ::drawCanvas* Canvas1;
  ::wxToggleButton* ToggleButton1UsingCustom;
  ::wxToggleButton* ToggleButton2ViewCombinatorialChambers;
  ::IndicatorWindowVariables progressReportVariables;
  //wxEVT_ProgressReport ProgressReportEvent;
  void OpenFile(std::fstream& output);
  void onToggleButton1UsingCustom( wxCommandEvent& ev);
  void onListBox1Change(wxCommandEvent& ev);
  void onButton2Eval(wxCommandEvent& ev);
  void onButton1Go(wxCommandEvent& ev);
  void onButton3Custom (wxCommandEvent& ev);
  void onButton16Custom2 (wxCommandEvent& ev);
  void onButton17Custom2PauseSaveResume(wxCommandEvent& ev);
  void onButton18Custom2Load(wxCommandEvent& ev);
  void onButton6OneSlice (wxCommandEvent& ev);
  void onButton7SliceIncrement (wxCommandEvent& ev);
  void onButton8FullChop(wxCommandEvent& ev);
  void onButton9CustomNilradical(wxCommandEvent& ev);
  void onButton10ProverOneStep(wxCommandEvent& ev);
  void onButton11ProverFullComputation(wxCommandEvent& ev);
  void onButton12ProverOneStepFixedK(wxCommandEvent& ev);
  void onButton13ProverFullComputationFixedK(wxCommandEvent& ev);
  void onButton14ProverProverFixedKSave(wxCommandEvent& ev);
  void onButton15ProverFixedKOpen(wxCommandEvent& ev);
  void onRBGroup1SlicingOptions(wxCommandEvent& ev);
  void onSpinner1and2 (wxSpinEvent & ev);
  void onComputationOver(wxCommandEvent& ev);
  void onRePaint(wxPaintEvent& ev);
  void onMouseDownOnCanvas(wxMouseEvent& ev);
  void onCheckBoxesGraphics(wxCommandEvent& ev);
  void ReadVPVectorsAndOptions();
  void WriteIndicatorWeight(root& tempRoot);
  void onProgressReport(wxCommandEvent & ev);
  void TurnOnAllDangerousButtons();
  void TurnOffAllDangerousButtons();
  void updatePartialFractionAndCombinatorialChamberTextData();
  void updateInputButtons();
  void ReadRBData();
  void initWeylGroupInfo();
  static void RunTheComputation();
  void initTableFromVPVectors();
  void initTableFromRowsAndColumns(int r, int c);
  void ReadSettingsIfAvailable();
  void WriteSettingsIfAvailable();
  void ArrangeWindows();
  guiMainWindow();
  ~guiMainWindow();
  void OnExit(wxCloseEvent& event);
  static const int DefaultButtonHeight=30;
  static const int DefaultButtonWidth=100;
  enum
  { ID_MainWindow= 1001,
    ID_Dialog1,
    ID_Dialog2,
    ID_ToggleButton1UsingCustom,
    ID_ToggleButton2ViewCombinatorialChambers,
    ID_ListBox1,
    ID_Button1Go,
    ID_Buton2Eval,
    ID_Button4SaveReadable,
    ID_Button5SaveComputer,
    ID_Button6OneSlice,
    ID_Button7Increment,
    ID_Button8FullChop,
		ID_Button9CustomNilradical,
		ID_Button10ProverOneStep,
		ID_Button11ProverFullComputation,
		ID_Button12ProverOneStepFixedK,
		ID_Button13ProverFullComputationFixedK,
		ID_Button14ProverFixedKSave,
		ID_Button15ProverFixedKOpen,
		ID_RBGroup1SliceOptions,
    ID_Spin1Dim,
    ID_Spin2NumVect,
    ID_Canvas1,
    ID_ComputationUpdate,
    ID_CheckBox1,
    ID_CheckBoxesGraphics,
    ID_Button3Custom,
    ID_Button16Custom2,
    ID_Button17Custom2PauseSaveResume,
    ID_Button18Custom2Load,
    ID_Paint,
  };
  DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP( guiApp )

guiMainWindow *MainWindow1;
//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{	::wxWindowDC dc(MainWindow1->Canvas1); wxPen tempPen;
  switch (thePenStyle)
  {	case 0: tempPen.SetStyle(::wxSOLID); break;
		case 1:	tempPen.SetStyle(::wxSHORT_DASH); break;
		case 2:	tempPen.SetStyle(::wxDOT); break;
		case 5: return;
  }
  tempPen.SetColour(ColorIndex); dc.SetPen(tempPen);
  dc.DrawLine((int)X1, (int)Y1,(int) X2,(int) Y2);
//	dc.setForeground(FXRGB(0,0,0));
//  dc.fillRectangle(0,0,MainWindow1->Canvas1DrawCanvas->getWidth(),MainWindow1->Canvas1DrawCanvas->getHeight());
}

void drawtext(double X1, double Y1, const char* text, int length, int color)
{	::wxWindowDC dc(MainWindow1->Canvas1);
  dc.SetFont(*MainWindow1->theFont);
  dc.SetTextForeground(color);
  //dc.setcolo(color);
  //dc.setBackground(MainWindow1->Canvas1DrawCanvas->getBackColor());
  //dc(FILL_STIPPLED);
  wxString temptext(text,wxConvUTF8 ,length);
  dc.DrawText(temptext,(int)X1, (int)Y1);
}

std::string MainWindow1GlobalPath;


bool guiApp::OnInit()
{	char directoryCharacter;
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
  {	wxPathList pathlist;
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
  {	if (::MainWindow1GlobalPath[i]==directoryCharacter)
    {	pathCutOffSize=i+1;
      break;
		}
  }
  ::MainWindow1GlobalPath.resize(pathCutOffSize);
  MainWindow1 = new guiMainWindow;
  MainWindow1->Show(true);
  return true;
}

BEGIN_EVENT_TABLE( guiMainWindow, wxFrame )
    EVT_TOGGLEBUTTON(guiMainWindow::ID_ToggleButton1UsingCustom, guiMainWindow::onToggleButton1UsingCustom)
    EVT_BUTTON(guiMainWindow::ID_Buton2Eval, guiMainWindow::onButton2Eval)
    EVT_BUTTON(guiMainWindow::ID_Button1Go, guiMainWindow::onButton1Go)
    EVT_BUTTON(guiMainWindow::ID_Button3Custom, guiMainWindow::onButton3Custom)
    EVT_BUTTON(guiMainWindow::ID_Button16Custom2, guiMainWindow::onButton16Custom2)
    EVT_BUTTON(guiMainWindow::ID_Button17Custom2PauseSaveResume, guiMainWindow::onButton17Custom2PauseSaveResume)
    EVT_BUTTON(guiMainWindow::ID_Button18Custom2Load, guiMainWindow::onButton18Custom2Load)
    EVT_BUTTON(guiMainWindow::ID_Button6OneSlice, guiMainWindow::onButton6OneSlice)
    EVT_BUTTON(guiMainWindow::ID_Button7Increment, guiMainWindow::onButton7SliceIncrement)
    EVT_BUTTON(guiMainWindow::ID_Button8FullChop, guiMainWindow::onButton8FullChop)
    EVT_BUTTON(guiMainWindow::ID_Button9CustomNilradical, guiMainWindow::onButton9CustomNilradical)
		EVT_RADIOBOX(guiMainWindow::ID_RBGroup1SliceOptions,guiMainWindow::onRBGroup1SlicingOptions)
    EVT_COMBOBOX(guiMainWindow::ID_ListBox1, guiMainWindow::onListBox1Change)
    EVT_SPINCTRL(guiMainWindow::ID_Spin1Dim, guiMainWindow::onSpinner1and2)
    EVT_SPINCTRL(guiMainWindow::ID_Spin2NumVect, guiMainWindow::onSpinner1and2)
    EVT_SIZING(drawCanvas::onSizing)
    EVT_SIZE(drawCanvas::onSizing)
    EVT_CHECKBOX(guiMainWindow::ID_CheckBoxesGraphics, guiMainWindow::onCheckBoxesGraphics)
    EVT_COMMAND (guiMainWindow::ID_MainWindow,::wxEVT_ProgressReport,guiMainWindow::onProgressReport )
    EVT_COMMAND (guiMainWindow::ID_MainWindow,::wxEVT_ComputationFinished,guiMainWindow::onComputationOver )
    EVT_CLOSE(guiMainWindow::OnExit)
    //EVT_PAINT(guiMainWindow::onPaint)
END_EVENT_TABLE()

class wxDialogOutput : public wxDialog
{
public:
    wxDialogOutput ( wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
    void onToggleButton2ViewCombinatorialChambers( wxCommandEvent& ev);
    void onButton4SaveReadable(wxCommandEvent& ev);
    void onButton5SaveComputer(wxCommandEvent& ev);
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( wxDialogOutput, wxDialog )
    EVT_TOGGLEBUTTON(	guiMainWindow::ID_ToggleButton2ViewCombinatorialChambers, wxDialogOutput::onToggleButton2ViewCombinatorialChambers)
    EVT_BUTTON(guiMainWindow::ID_Button4SaveReadable, wxDialogOutput::onButton4SaveReadable)
    EVT_BUTTON(guiMainWindow::ID_Button5SaveComputer, wxDialogOutput::onButton5SaveComputer)
    EVT_BUTTON(guiMainWindow::ID_Button10ProverOneStep, guiMainWindow::onButton10ProverOneStep)
    EVT_BUTTON(guiMainWindow::ID_Button11ProverFullComputation, guiMainWindow::onButton11ProverFullComputation)
    EVT_BUTTON(guiMainWindow::ID_Button12ProverOneStepFixedK, guiMainWindow::onButton12ProverOneStepFixedK)
    EVT_BUTTON(guiMainWindow::ID_Button13ProverFullComputationFixedK, guiMainWindow::onButton13ProverFullComputationFixedK)
    EVT_BUTTON(guiMainWindow::ID_Button14ProverFixedKSave, guiMainWindow::onButton14ProverProverFixedKSave)
    EVT_BUTTON(guiMainWindow::ID_Button15ProverFixedKOpen, guiMainWindow::onButton15ProverFixedKOpen)
END_EVENT_TABLE()

wxDialogOutput::wxDialogOutput ( wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & position, const wxSize & size, long style )
        : wxDialog( parent, id, title, position, size, style)
{
}


#ifdef WIN32
void RunComputationalThread()
#else
void* RunComputationalThread(void*ptr)
#endif
{	MainWindow1->theComputationSetup.Run();
  wxPostEvent(MainWindow1->GetEventHandler(),MainWindow1->wxComputationOver);
#ifndef WIN32
    pthread_exit(NULL);
#endif
}

void wxComboBoxWheel::OnMouseWheel(wxMouseEvent& event)
{ // scroll in drop down list using mouse wheel
  int rot = event.GetWheelRotation()/event.GetWheelDelta();
  int lines = rot*1;// event.GetLinesPerAction();
  unsigned int newSelection = (unsigned)(this->GetSelection()-lines);
  if (newSelection>=(unsigned)this->GetCount()|| newSelection<0)
    return;
  this->SetSelection(newSelection);
  wxCommandEvent ev;
  MainWindow1->onListBox1Change(ev);
}


void drawCanvas::onMouseDownOnCanvas(wxMouseEvent &ev)
{	int Realx=ev.GetX();//-this->Canvas1->GetRect().GetTop();
  int Realy=ev.GetY();//-this->GetRect().GetLeft();
  if (TDV.Selected==-2)
  {	double tempX, tempY;
    int tempXi, tempYi;
    int tempXi2, tempYi2;
    for (int i=0;i<MaxRank;i++)
    {	tempXi=(int)TDV.Projections[i][0];
      tempYi=(int)TDV.Projections[i][1];
      tempXi2=Realx-((int)TDV.centerX);
      tempYi2=((int) TDV.centerY)-Realy;
      if (((tempXi2-ClickToleranceX)<=tempXi) && (tempXi<=(tempXi2+ClickToleranceX)) &&
					((tempYi2-ClickToleranceY)<=tempYi) && (tempYi<=(tempYi2+ClickToleranceY)))
			{	TDV.Selected=i;
        break;
      }
    }
    tempX=TDV.centerX;
    tempY=TDV.centerY;
    tempXi=(int)tempX;
    tempYi=(int)tempY;
    if (((Realx-ClickToleranceX)<=tempXi) && (tempXi<=(Realx+ClickToleranceX)) &&
        ((Realy-ClickToleranceY)<=tempYi) && (tempYi<=(Realy+ClickToleranceY)))
			TDV.Selected=-1;
  }	else
	{	if (TDV.Selected==-1)
    {	TDV.centerX = (double) Realx;
      TDV.centerY = (double) Realy;
		} else
    {	double tempx, tempy;
      tempx=Realx;
      tempy=Realy;
      TDV.Projections[TDV.Selected][0]=tempx-TDV.centerX;
      TDV.Projections[TDV.Selected][1]=TDV.centerY-tempy;
    }
    TDV.Selected=-2;
    //wxPaintEvent tempev;
    this->Refresh();//tempev);
	}
}

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output);

guiMainWindow::guiMainWindow(): wxFrame(	(wxFrame *)NULL, guiMainWindow::ID_MainWindow, wxT("Vector partition function v0.101 (eating RAM for breakfast)"), wxPoint(100,100), wxSize(800,600), wxRESIZE_BORDER| wxCAPTION | wxSYSTEM_MENU| wxCLOSE_BOX | wxMINIMIZE_BOX)
{	//this->theComputationSetup.flagDoCustomComputation=true;
  this->theComputationSetup.flagHavingNotationExplanation=false;
  this->theComputationSetup.flagComputingVectorPartitions=true;
  this->theComputationSetup.flagHavingStartingExpression=true;
  this->theComputationSetup.flagHavingDocumentClassForLaTeX=true;
  this->BoxSizer1HorizontalBackground = new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer2VerticalInputs = new ::wxBoxSizer(::wxVERTICAL);
  this->BoxSizer3HorizontalInputButtons = new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer4VerticalToggleButton1 = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer5VerticalCanvasAndProgressReport = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer6HorizontalInputsBlock = new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer7VerticalListBox1 = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer8HorizontalEval =new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer9HorizontalCombo1AndMainButton= new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer10HorizontalProgressReportsAndOptions = new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer11VerticalOptions = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer12VerticalProgressReports = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer13VerticalPartFracOutput = new ::wxBoxSizer(wxVERTICAL);
  this->BoxSizer14HorizontalSaveButtons = new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer15HorizontalSlicingButtons= new ::wxBoxSizer(wxHORIZONTAL);
  this->BoxSizer16VerticalStatusString= new wxBoxSizer(wxVERTICAL);
  this->BoxSizer17HorizontalProverButtons=  new ::wxBoxSizer(wxHORIZONTAL);
  this->ToggleButton1UsingCustom= new ::wxToggleButton(this, guiMainWindow::ID_ToggleButton1UsingCustom,wxT("Switch to custom"));
  this->Table1Input = new ::wxGridExtra( this,wxID_ANY);
  this->Table2Indicator = new wxGridExtra( this,::wxID_ANY);
  this->Table3Values = new wxGridExtra( this,::wxID_ANY);
  this->Label1ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Label2ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Label3ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Label4ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Label5ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Label9OutputVPF= new ::wxStaticText(this,wxID_ANY, wxT("Vector partition function LaTex format:"));
  this->Spin1Dim = new wxSpinCtrl(this,this->ID_Spin1Dim);
  this->Spin2NumVect= new wxSpinCtrl(this, this->ID_Spin2NumVect);
  this->CheckBox1ComputePFs= new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Don't compute PF"));
  this->CheckBox2CheckSums= new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Checksums"));
  wxString tempS[3];
  tempS[0].assign(wxT("Full chop"));
  tempS[1].assign(wxT("One increment"));
  tempS[2].assign(wxT("Single slice"));
  this->RBGroup1SlicingOptions= new wxRadioBox(this, this->ID_RBGroup1SliceOptions, wxT("Slicing options"), wxDefaultPosition, wxDefaultSize, 3, tempS);
  this->CheckBox3ComputeChambers= new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Compute chambers"));
  this->CheckBox4ChamberLabels=new ::wxCheckBox(this,this->ID_CheckBoxesGraphics,wxT("Chamber labels"));
  this->CheckBox5InvisibleChambers=new ::wxCheckBox(this,this->ID_CheckBoxesGraphics,wxT("Invisibles"));
  this->CheckBox6Dashes=new ::wxCheckBox(this,this->ID_CheckBoxesGraphics,wxT("Dashes"));
  this->CheckBox7UseIndicatorForPFDecomposition=new ::wxCheckBox(this,this->ID_CheckBoxesGraphics,wxT("Make complete pf decomposition"));
	this->CheckBox8DoTheWeylGroup=new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Act by Weyl group"));
  //this->Spin2NumVect->SetSize(this->DefaultButtonWidth,this->DefaultButtonHeight);
  //this->Spin1Dim->SetSize(this->DefaultButtonWidth,this->DefaultButtonHeight);
  this->Canvas1 = new ::drawCanvas(this,::wxID_ANY,::wxDefaultPosition,::wxDefaultSize,::wxEXPAND|wxALL);
  //this->Label5ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Table1Input->CreateGrid( 0, 0 );
  this->Table2Indicator->CreateGrid( 0, 0 );
  this->Table3Values->CreateGrid(0,0);
  this->theFont= new ::wxFont(10,wxDEFAULT, wxNORMAL,wxNORMAL);
  this->ListBox1WeylGroup= new ::wxComboBoxWheel(this, this->ID_ListBox1,wxT("A3"), wxPoint(0,0),::wxDefaultSize,0,0,wxCB_DROPDOWN  );
  this->Button2Eval= new ::wxButton(this,this->ID_Buton2Eval, wxT("Evaluate"));
  this->Button2Eval->SetSize(this->DefaultButtonWidth, this->DefaultButtonHeight);
  this->Button1Go= new ::wxButton(this, this->ID_Button1Go, wxT("Go"));
  this->Button1Go->SetSize(this->DefaultButtonWidth, this->DefaultButtonHeight);
  this->Button3Custom= new wxButton(this, this->ID_Button3Custom, wxT("Experiments"));
  this->Button16Custom2= new wxButton(this, this->ID_Button16Custom2, wxT("Experiments 2"));
  this->Button17Custom2PauseSaveResume= new wxButton(this, this->ID_Button17Custom2PauseSaveResume, wxT("Experiments 2 Pause+Save"));
  this->Button18Custom2Load= new wxButton(this, this->ID_Button18Custom2Load, wxT("Experiments 2 Load+Resume"));
  //this->Button6OneSlice= new wxButton(this,this->ID_Button6OneSlice,wxT("One slice"));
  //this->Button7OneDirectionIncrement= new wxButton(this,this->ID_Button7Increment,wxT("Increment"));
  //this->Button8FullChopping= new wxButton(this,this->ID_Button8FullChop,wxT("Full chop"));
	//this->Button9CustomNilradical= new wxButton(this,this->ID_Button9CustomNilradical,wxT("Custom nilradical"));
  this->Text1Output= new ::wxTextCtrl(this,::wxID_ANY,wxT(""),::wxDefaultPosition, ::wxDefaultSize,wxTE_MULTILINE);
  this->Text2Values= new ::wxTextCtrl(this,::wxID_ANY);
  this->Dialog1OutputPF= new ::wxDialogOutput ( this,guiMainWindow::ID_Dialog1, wxT("Partial fractions"), ::wxDefaultPosition, ::wxDefaultSize, wxRESIZE_BORDER| wxCAPTION);
  this->Dialog2StatusString1=  new ::wxDialogOutput( this, guiMainWindow::ID_Dialog2, wxT("Status"), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER| wxCAPTION);
  this->Text3PartialFractions= new ::wxTextCtrl (	this->Dialog1OutputPF,::wxID_ANY,wxT(""), wxDefaultPosition, ::wxDefaultSize,wxTE_MULTILINE);
  this->Text4StatusString1 = new wxTextCtrl ( this->Dialog2StatusString1,::wxID_ANY,wxT(""), wxDefaultPosition, ::wxDefaultSize, wxTE_MULTILINE);
  this->ToggleButton2ViewCombinatorialChambers= new ::wxToggleButton(	this->Dialog1OutputPF,guiMainWindow::ID_ToggleButton2ViewCombinatorialChambers, wxT("Switch to chamber data"));
  this->Button4SaveReadable = new ::wxButton (this->Dialog1OutputPF,this->ID_Button4SaveReadable,wxT("Save"));
  this->Button5SaveComputer = new ::wxButton (this->Dialog1OutputPF,this->ID_Button5SaveComputer,wxT("Save computer format"));
	this->Button10ProverOneStep = new ::wxButton(this->Dialog2StatusString1,this->ID_Button10ProverOneStep,wxT("Prover one step "));
	this->Button11ProverFullComputation = new ::wxButton(this->Dialog2StatusString1,this->ID_Button11ProverFullComputation,wxT("Prover full go"));
	this->Button12ProverOneStepFixedK = new ::wxButton(this->Dialog2StatusString1,this->ID_Button12ProverOneStepFixedK,wxT("Prover one step fixed K"));
	this->Button13ProverFullComputationFixedK = new ::wxButton(this->Dialog2StatusString1,this->ID_Button13ProverFullComputationFixedK,wxT("Prover full go Fixed K"));
	this->Button14ProverFixedKSave= new ::wxButton(this->Dialog2StatusString1,this->ID_Button14ProverFixedKSave,wxT("Save provers"));
	this->Button15ProverFixedKOpen= new ::wxButton(this->Dialog2StatusString1,this->ID_Button15ProverFixedKOpen,wxT("Open provers"));
	this->Button5SaveComputer->Disable();
  //this->BoxSizer1HorizontalBackground->Fit(this);
  this->BoxSizer1HorizontalBackground->Add(this->BoxSizer2VerticalInputs,0,wxEXPAND|::wxBOTTOM);
  this->BoxSizer2VerticalInputs->Add(this->BoxSizer6HorizontalInputsBlock,0, wxEXPAND| wxALL,0);
  this->BoxSizer6HorizontalInputsBlock->Add(this->BoxSizer4VerticalToggleButton1,0,wxEXPAND);
  this->BoxSizer4VerticalToggleButton1->Add(this->ToggleButton1UsingCustom,0,wxALIGN_TOP);
  this->BoxSizer4VerticalToggleButton1->Add(new ::wxBoxSizer(::wxVERTICAL),1,::wxEXPAND);
  this->BoxSizer4VerticalToggleButton1->Add(this->Spin1Dim,0,::wxALIGN_BOTTOM,0);
  this->BoxSizer6HorizontalInputsBlock->Add(this->BoxSizer7VerticalListBox1,0, wxEXPAND| wxALL,0);
  this->BoxSizer7VerticalListBox1->Add(this->BoxSizer9HorizontalCombo1AndMainButton, wxEXPAND| wxALL);
  this->BoxSizer9HorizontalCombo1AndMainButton->Add(this->ListBox1WeylGroup);
  this->BoxSizer9HorizontalCombo1AndMainButton->Add(this->Button1Go);
  this->BoxSizer7VerticalListBox1->Add(this->Spin2NumVect);
  //this->BoxSizer7VerticalListBox1->Add(this->Spin2NumVect);
  this->BoxSizer2VerticalInputs->Add(new wxStaticText(this,wxID_ANY,wxT("Chamber indicator")));
  this->BoxSizer2VerticalInputs->Add(this->Table2Indicator);//,0,wxEXPAND|wxALL);
  this->BoxSizer2VerticalInputs->Add(this->Table1Input);//,0,wxEXPAND|wxALL);
  this->BoxSizer2VerticalInputs->Add(new wxStaticText(this,wxID_ANY,wxT("Evaluation")));
  this->BoxSizer2VerticalInputs->Add(this->BoxSizer8HorizontalEval);
  this->BoxSizer8HorizontalEval->Add(this->Table3Values);
  this->BoxSizer8HorizontalEval->Add(this->Button2Eval);
  this->BoxSizer8HorizontalEval->Add(this->Text2Values,0);
  this->BoxSizer2VerticalInputs->Add(this->Label9OutputVPF);
  this->BoxSizer2VerticalInputs->Add(this->Text1Output,1,::wxEXPAND| ::wxALL);
  this->BoxSizer1HorizontalBackground->Add(this->BoxSizer5VerticalCanvasAndProgressReport,1,wxEXPAND|wxALL);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->BoxSizer10HorizontalProgressReportsAndOptions);
  this->BoxSizer10HorizontalProgressReportsAndOptions->Add(this->BoxSizer11VerticalOptions);
		this->BoxSizer11VerticalOptions->Add(this->CheckBox1ComputePFs);
		this->BoxSizer11VerticalOptions->Add(this->CheckBox2CheckSums);
		this->BoxSizer11VerticalOptions->Add(this->CheckBox3ComputeChambers);
		this->BoxSizer11VerticalOptions->Add(this->CheckBox7UseIndicatorForPFDecomposition);
		this->BoxSizer11VerticalOptions->Add(this->Button3Custom);
		this->BoxSizer11VerticalOptions->Add(this->Button16Custom2);
		this->BoxSizer11VerticalOptions->Add(this->Button17Custom2PauseSaveResume);
		this->BoxSizer11VerticalOptions->Add(this->Button18Custom2Load);
  this->BoxSizer10HorizontalProgressReportsAndOptions->Add(this->BoxSizer12VerticalProgressReports);
  this->BoxSizer12VerticalProgressReports->Add(this->Label1ProgressReport);
  this->BoxSizer12VerticalProgressReports->Add(this->Label2ProgressReport);
  this->BoxSizer12VerticalProgressReports->Add(this->Label3ProgressReport);
  this->BoxSizer12VerticalProgressReports->Add(this->Label4ProgressReport);
  this->BoxSizer12VerticalProgressReports->Add(this->Label5ProgressReport);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->CheckBox4ChamberLabels);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->CheckBox5InvisibleChambers);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->CheckBox6Dashes);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->CheckBox8DoTheWeylGroup);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->BoxSizer15HorizontalSlicingButtons);
			this->BoxSizer15HorizontalSlicingButtons->Add(this->RBGroup1SlicingOptions);
			//this->BoxSizer15HorizontalSlicingButtons->Add(this->Button6OneSlice);
			//this->BoxSizer15HorizontalSlicingButtons->Add(this->Button7OneDirectionIncrement);
			//this->BoxSizer15HorizontalSlicingButtons->Add(this->Button8FullChopping);
			//this->BoxSizer15HorizontalSlicingButtons->Add(this->Button9CustomNilradical);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Canvas1,1,wxEXPAND|wxALL);
  this->BoxSizer13VerticalPartFracOutput->Add(this->ToggleButton2ViewCombinatorialChambers);
  this->BoxSizer13VerticalPartFracOutput->Add(this->Text3PartialFractions, 1, wxEXPAND|wxALL);
  this->BoxSizer13VerticalPartFracOutput->Add(this->BoxSizer14HorizontalSaveButtons);
    this->BoxSizer14HorizontalSaveButtons->Add(this->Button4SaveReadable);
    this->BoxSizer14HorizontalSaveButtons->Add(this->Button5SaveComputer);
  this->BoxSizer16VerticalStatusString->Add(this->BoxSizer17HorizontalProverButtons);
  this->BoxSizer16VerticalStatusString->Add(this->Text4StatusString1,1,wxEXPAND| wxALL);
		this->BoxSizer17HorizontalProverButtons->Add(this->Button10ProverOneStep);
		this->BoxSizer17HorizontalProverButtons->Add(this->Button11ProverFullComputation);
		this->BoxSizer17HorizontalProverButtons->Add(this->Button12ProverOneStepFixedK);
		this->BoxSizer17HorizontalProverButtons->Add(this->Button13ProverFullComputationFixedK);
		this->BoxSizer17HorizontalProverButtons->Add(this->Button14ProverFixedKSave);
		this->BoxSizer17HorizontalProverButtons->Add(this->Button15ProverFixedKOpen);
	this->Dialog1OutputPF->SetSizer(this->BoxSizer13VerticalPartFracOutput);
  this->Dialog2StatusString1->SetSizer(this->BoxSizer16VerticalStatusString);

//this->Panel1OutputPF->Create(this,::wxID_ANY, ::wxDefaultPosition, ::wxDefaultSize);
	//this->RBGroup1SlicingOptions->set

  this->ListBox1WeylGroup->Append(wxT("A2"));
  this->ListBox1WeylGroup->Append(wxT("A3"));
  this->ListBox1WeylGroup->Append(wxT("A4"));
  this->ListBox1WeylGroup->Append(wxT("A5"));
  this->ListBox1WeylGroup->Append(wxT("A6"));
  this->ListBox1WeylGroup->Append(wxT("B2"));
  this->ListBox1WeylGroup->Append(wxT("B3"));
  this->ListBox1WeylGroup->Append(wxT("B4"));
  this->ListBox1WeylGroup->Append(wxT("B5"));
  this->ListBox1WeylGroup->Append(wxT("C2"));
  this->ListBox1WeylGroup->Append(wxT("C3"));
  this->ListBox1WeylGroup->Append(wxT("C4"));
  this->ListBox1WeylGroup->Append(wxT("D4"));
  this->ListBox1WeylGroup->Append(wxT("D5"));
  this->ListBox1WeylGroup->Append(wxT("G2"));
  this->ListBox1WeylGroup->Append(wxT("E6"));
  this->ListBox1WeylGroup->Append(wxT("E7"));
  this->ListBox1WeylGroup->Append(wxT("E8"));
  this->ListBox1WeylGroup->Append(wxT("F4"));
  this->ListBox1WeylGroup->Append(wxT("Custom nilradical in A3"));
  this->ListBox1WeylGroup->SetSelection(1);
  this->CheckBox1ComputePFs->SetValue(false);
  this->CheckBox2CheckSums->SetValue(true);
  this->CheckBox3ComputeChambers->SetValue(true);
  this->CheckBox4ChamberLabels->SetValue(true);
  this->CheckBox5InvisibleChambers->SetValue(false);
  this->CheckBox7UseIndicatorForPFDecomposition->SetValue(true);
  this->CheckBox6Dashes->SetValue(true);
  this->CheckBox8DoTheWeylGroup->SetValue(false);
  this->theComputationSetup.flagComputationInProgress=false;
  this->theComputationSetup.flagAllowRepaint=true;
  this->theComputationSetup.flagUsingCustomVectors=false;
  //this->Button7OneDirectionIncrement->Disable();
  //this->Button8FullChopping->Disable();
  //////////////////////////////////////////
  /*this->theComputationSetup.UsingCustomVectors=true;
  root::AmbientDimension=8;
  this->theComputationSetup.VPVectors.SetSizeExpandOnTopNoObjectInit(10);
  this->theComputationSetup.VPVectors.TheObjects[0].InitFromIntegers(0	,	 0,	0,	0	,	 0,	1	,	1	,0);
  this->theComputationSetup.VPVectors.TheObjects[1].InitFromIntegers(2	,	 0,	0,	0	,	1	,-2,	0	,1);
  this->theComputationSetup.VPVectors.TheObjects[2].InitFromIntegers(-1	,	 1,	0,	1	,	-1,	2	,	1	,0);
  this->theComputationSetup.VPVectors.TheObjects[3].InitFromIntegers(0	,	-2,	1,	-2,	2,	-2,	0	,1);
  this->theComputationSetup.VPVectors.TheObjects[4].InitFromIntegers(-1	,	 0,	0,	0	,	0 ,	0	,	0	,0);
  this->theComputationSetup.VPVectors.TheObjects[5].InitFromIntegers(0	,	-1,	0,	0	,	0	,	0	,	0	,0);
  this->theComputationSetup.VPVectors.TheObjects[6].InitFromIntegers(0	,	 0,	1,	0	,	0	,	0	,	0	,0);
  this->theComputationSetup.VPVectors.TheObjects[7].InitFromIntegers(0	,	 0,	0,	1	,	0	,	0	,	0	,0);
  this->theComputationSetup.VPVectors.TheObjects[8].InitFromIntegers(0	,	 0,	0,	0	,	1	,	0	,	0	,0);
  this->theComputationSetup.VPVectors.TheObjects[9].InitFromIntegers(0	,	 0,	0,	0	,	0	,	1	,	0	,0);
  this->theComputationSetup.WeylGroupIndex= root::AmbientDimension;
  this->updateInputButtons();*/
  //////////////////////////////////////////
  //this->Canvas1DrawCanvas->::FXCanvas::setBackColor(this->getBackColor());
  this->Canvas1->ClickToleranceX=10;
  this->Canvas1->ClickToleranceY=10;
  this->WorkThread1.CriticalSectionPauseButtonEntered=false;
  this->WorkThread1.CriticalSectionWorkThreadEntered=false;
  TDV.initDrawingVariables(250,250);
  //this->Button3Custom->Disable();
  this->wxProgressReportEvent.SetId(this->GetId());
  this->wxProgressReportEvent.SetEventObject(this);
  this->wxProgressReportEvent.SetEventType(::wxEVT_ProgressReport);
  this->wxComputationOver.SetId(this->GetId());
  this->wxComputationOver.SetEventObject(this);
  this->wxComputationOver.SetEventType(wxEVT_ComputationFinished);
  this->MaxAllowedVectors= 16;
  //WeylGroup::flagAnErrorHasOcurredTimeToPanic=true;
  this->initWeylGroupInfo();
  this->updateInputButtons();
  this->SetSizer(this->BoxSizer1HorizontalBackground);
  this->theComputationSetup.theGlobalVariablesContainer->Default()->FeedDataToIndicatorWindowDefault=&FeedDataToIndicatorWindowWX;
#ifndef WIN32
  pthread_mutex_init(&ParallelComputing::mutex1, NULL);
  pthread_cond_init (&ParallelComputing::continueCondition, NULL);
#endif
  this->ReadSettingsIfAvailable();
  this->Dialog1OutputPF->Show();
  this->Dialog2StatusString1->Show();
  //Centre();
}

void drawCanvas::onSizing(wxSizeEvent& ev)
{	if (MainWindow1==0)
		return;
	if (MainWindow1->Table1Input->GetNumberRows()>20)
  {	MainWindow1->Table1Input->SetAutoLayout(false);
    MainWindow1->Table1Input->SetSize(0,70,220,500);
    MainWindow1->Table1Input->SetMaxSize(wxSize(220,500));
	}
  MainWindow1->Layout();
  this->Refresh();//true,&tempRect);
}

guiMainWindow::~guiMainWindow()
{	//this->theFont
  this->theComputationSetup.flagAllowRepaint=false;
  this->WriteSettingsIfAvailable();
  this->Canvas1->Destroy();
  this->Dialog1OutputPF->Destroy();
  this->Dialog2StatusString1->Destroy();
  this->fileSettings.close();
#ifndef WIN32
  pthread_mutex_destroy(&ParallelComputing::mutex1);
  pthread_cond_destroy(&ParallelComputing::continueCondition);
#endif
}

void RunA_voidFunctionFromFunctionAddress
(//grrrrrrrrrrrrrrrrrrrrrrrrrrrr
#ifdef WIN32
    void (*RunAThread)(void), HANDLE& ComputationalThreadEntry
#else
    void* (*RunAThread)(void*ptr), pthread_t* ComputationalThreadLinux
#endif
)
{
#ifdef WIN32
    ComputationalThreadEntry=CreateThread(0,0, (LPTHREAD_START_ROUTINE)(*RunAThread),0,0,0);
#else
    //RunComputationalThread(0);
    pthread_create(ComputationalThreadLinux, NULL,*RunAThread, 0);
#endif
}

rootFKFTcomputation rootFKFTComputationLocal;
#ifdef WIN32
void RunrootFKFTComputationLocal()
#else
void* RunrootFKFTComputationLocal(void*)
#endif
{	rootFKFTComputationLocal.RunA2A1A1inD5beta12221();
#ifndef WIN32
    return 0;
#endif
}

void guiMainWindow::onToggleButton1UsingCustom( wxCommandEvent& ev)
{	if (!this->ToggleButton1UsingCustom->GetValue())
  {	this->theComputationSetup.flagUsingCustomVectors=false;
    this->ToggleButton1UsingCustom->SetLabel(wxT("Switch to custom"));
  } else
	{	this->theComputationSetup.flagUsingCustomVectors=true;
    this->ToggleButton1UsingCustom->SetLabel(wxT("Switch to roots"));
  }
  this->updateInputButtons();
}

void wxDialogOutput::onButton4SaveReadable(wxCommandEvent &ev)
{	if (MainWindow1==0)
		return;
	std::fstream tempFile;
  MainWindow1->OpenFile(tempFile);
  if (tempFile.is_open())
	{	MainWindow1->theComputationSetup.WriteReportToFile(::TDV,tempFile,*MainWindow1->theComputationSetup.theGlobalVariablesContainer->Default());
	}
	tempFile.close();
}

void wxDialogOutput::onButton5SaveComputer(wxCommandEvent &ev)
{
}

void wxDialogOutput::onToggleButton2ViewCombinatorialChambers(wxCommandEvent &ev)
{	if (MainWindow1==0)
		return;
	if (!MainWindow1->ToggleButton2ViewCombinatorialChambers->GetValue())
  {	MainWindow1->theComputationSetup.flagDisplayingCombinatorialChambersTextData=false;
    MainWindow1->theComputationSetup.flagDisplayingPartialFractions=true;
    MainWindow1->ToggleButton2ViewCombinatorialChambers->SetLabel(wxT("Switch to chamber data"));
    MainWindow1->Button4SaveReadable->Enable();
    MainWindow1->Button5SaveComputer->Disable();
  }	else
	{	MainWindow1->theComputationSetup.flagDisplayingCombinatorialChambersTextData=true;
    MainWindow1->theComputationSetup.flagDisplayingPartialFractions=false;
    MainWindow1->ToggleButton2ViewCombinatorialChambers->SetLabel(wxT("Switch to partial fractions"));
    MainWindow1->Button4SaveReadable->Disable();
    MainWindow1->Button5SaveComputer->Disable();
  }
  MainWindow1->updatePartialFractionAndCombinatorialChamberTextData();
}

void guiMainWindow::onRePaint(wxPaintEvent& ev)
{ ::wxPaintDC dc;
  this->Refresh();
}

void drawCanvas::OnPaint(::wxPaintEvent& ev)
{	::wxPaintDC  dc(this);
	if (MainWindow1==0)
		return;
  if (MainWindow1->theComputationSetup.flagAllowRepaint)
  {	dc.SetBackground(MainWindow1->GetBackgroundColour());
    dc.DrawRectangle(wxPoint(0,0),this->GetSize());
    ::CombinatorialChamberContainer::drawOutput
			(	::TDV,MainWindow1->theComputationSetup.theChambers, MainWindow1->theComputationSetup.InputRoots, MainWindow1->theComputationSetup.NextDirectionIndex,
				MainWindow1->theComputationSetup.theChambers.IndicatorRoot,0, &drawline, &drawtext);
  }
}

void guiMainWindow::RunTheComputation()
{//::RandomCodeIDontWantToDelete::SomeRandomTests2();
  MainWindow1->TurnOffAllDangerousButtons();
//#ifdef WIN32
	if (!MainWindow1->theComputationSetup.flagComputationInProgress)
  {	MainWindow1->theComputationSetup.flagComputationInProgress=true;
    MainWindow1->Button1Go->SetLabel(wxT("Pause"));
#ifdef WIN32
    MainWindow1->WorkThread1.ComputationalThread=CreateThread(0,0, (LPTHREAD_START_ROUTINE)RunComputationalThread,0,0,0);
#else
    //RunComputationalThread(0);
    pthread_create(&MainWindow1->WorkThread1.ComputationalThreadLinux, NULL,RunComputationalThread, 0);
#endif
  }	else
  { if (MainWindow1->WorkThread1.isRunning)
    {	//pthread_mutex_lock(&ParallelComputing::mutex1);
      MainWindow1->WorkThread1.CriticalSectionPauseButtonEntered=true;
      //return;
      while (MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered)
      {}
      ParallelComputing::ReachSafePointASAP=true;
      while (!ParallelComputing::SafePointReached)
      {}
      ParallelComputing::ReachSafePointASAP=false;
#ifdef WIN32
      ::SuspendThread(MainWindow1->WorkThread1.ComputationalThread);
#endif
      MainWindow1->WorkThread1.isRunning=false;
      MainWindow1->Button1Go->SetLabel(wxT("Go"));
      MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
      MainWindow1->WorkThread1.CriticalSectionPauseButtonEntered=false;
      //pthread_mutex_unlock(&ParallelComputing::mutex1);
    }	else
    {	//pthread_mutex_lock(&ParallelComputing::mutex1);
      MainWindow1->Button1Go->SetLabel(wxT("Pause"));
      MainWindow1->WorkThread1.isRunning=true;
#ifdef WIN32
      ResumeThread(MainWindow1->WorkThread1.ComputationalThread);
#else
      pthread_cond_signal(&ParallelComputing::continueCondition);
#endif
      //pthread_mutex_unlock(&ParallelComputing::mutex1);
    }
  }
//#else
  //this->theComputationSetup.ComputationInProgress=true;
//	this->WorkThread1.run();
//#endif
}

void guiMainWindow::onButton3Custom(wxCommandEvent& ev)
{ this->theComputationSetup.flagDoCustomComputation=true;
  this->theComputationSetup.flagUsingProverDoNotCallOthers=false;
  this->theComputationSetup.theRootSubalgebras.flagComputingLprohibitingWeights=true;
  this->theComputationSetup.theRootSubalgebras.flagComputeConeCondition=true;
  this->theComputationSetup.theRootSubalgebras.flagUsingONLYActionsNormalizerCentralizerNilradical=false;
  this->theComputationSetup.WeylGroupIndex=6;
  this->theComputationSetup.WeylGroupLetter='E';
  this->RunTheComputation();
}

void guiMainWindow::onButton16Custom2(wxCommandEvent& ev)
{ this->theComputationSetup.flagUsingProverDoNotCallOthers=false;
  this->theComputationSetup.flagDoCustomComputation=false;
  this->theComputationSetup.flagRunningExperiments2=true;
  this->RunTheComputation();
}

void guiMainWindow::onButton17Custom2PauseSaveResume(wxCommandEvent& ev)
{ this->theComputationSetup.theChambers.PauseSlicing();
  this->theComputationSetup.theChambers.WriteToDefaultFile();
  int tempI= wxMessageBox(wxT("Saved! Press OK to continue with the computation, Cancel to quit."), wxT("Saved"), wxOK | wxCANCEL);
  if (tempI==wxCANCEL)
    this->theComputationSetup.theChambers.flagMustStop=true;
  this->theComputationSetup.theChambers.ResumeSlicing();
}

void guiMainWindow::onButton18Custom2Load(wxCommandEvent& ev)
{ this->theComputationSetup.theChambers.ReadFromDefaultFile();
  this->theComputationSetup.flagRunningExperiments2=true;
  this->theComputationSetup.flagExperiment2ChambersAlreadyLoaded=true;
  this->theComputationSetup.WeylGroupIndex=this->theComputationSetup.theChambers.AmbientDimension;
  this->RunTheComputation();
}


void guiMainWindow::onButton1Go(wxCommandEvent& ev)
{ this->theComputationSetup.flagUsingProverDoNotCallOthers=false;
  this->theComputationSetup.flagDoCustomComputation=false;
  this->ReadVPVectorsAndOptions();
  this->RunTheComputation();
}

void guiMainWindow::OpenFile(std::fstream& output)
{ wxFileDialog* OpenDialog = new wxFileDialog ( this, wxT("Choose a file to open"), wxEmptyString, wxEmptyString, wxT("Text files (*.tex)|*.tex"),wxSAVE, wxDefaultPosition);
  output.close();
  if (OpenDialog->ShowModal() == wxID_OK)
  { wxString CurrentDocPath = OpenDialog->GetPath();
    std::string tempS(CurrentDocPath.mb_str());
    output.open(tempS.c_str(),std::fstream::out|std::fstream::trunc);
    output.clear();
  }
  OpenDialog->Destroy();
}

void guiMainWindow::onButton2Eval(wxCommandEvent& ev)
{ if (this->theComputationSetup.theOutput.NumVars!=this->Table3Values->GetNumberCols())
    return;
  intRoot tempRoot;
  for (int i=0;i<this->theComputationSetup.theOutput.NumVars;i++)
    this->theComputationSetup.ValueRoot.elements[i]= wxAtoi(this->Table3Values->GetCellValue(0,i));
  this->theComputationSetup.EvaluatePoly();
  this->Text2Values->SetValue(wxString(this->theComputationSetup.ValueString.c_str(),wxConvUTF8));
  return;
}

void guiMainWindow::onButton6OneSlice(wxCommandEvent& ev)
{ this->ReadVPVectorsAndOptions();
	this->theComputationSetup.flagDoCustomNilradical=false;
	this->theComputationSetup.oneStepChamberSlice(*this->theComputationSetup.theGlobalVariablesContainer->Default());
  this->Dialog1OutputPF->onToggleButton2ViewCombinatorialChambers(ev);
  this->Refresh();
}

void guiMainWindow::onButton7SliceIncrement(wxCommandEvent& ev)
{ this->ReadVPVectorsAndOptions();
	this->theComputationSetup.flagDoCustomNilradical=false;
	this->theComputationSetup.oneIncrement(*this->theComputationSetup.theGlobalVariablesContainer->Default());
	this->Dialog1OutputPF->onToggleButton2ViewCombinatorialChambers(ev);
  this->Refresh();
}

void guiMainWindow::onButton10ProverOneStep(wxCommandEvent& ev)
{ MainWindow1->theComputationSetup.flagUsingProverDoNotCallOthers=true;
	MainWindow1->theComputationSetup.flagProverDoingFullRecursion=false;
  MainWindow1->theComputationSetup.flagProverUseFixedK=false;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName.append( "theProverFixedK.txt");
  MainWindow1->theComputationSetup.theProver.FileHeaderString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileBodyString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileHeaderString.append("theProverHeader.txt");
  MainWindow1->theComputationSetup.theProver.FileBodyString.append("theProverBody.txt");
  MainWindow1->RunTheComputation();
}

void guiMainWindow::onButton11ProverFullComputation(wxCommandEvent& ev)
{ MainWindow1->theComputationSetup.flagProverDoingFullRecursion=true;
  MainWindow1->theComputationSetup.flagUsingProverDoNotCallOthers=true;
  MainWindow1->theComputationSetup.flagProverUseFixedK=false;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName.append( "theProverFixedK.txt");
  MainWindow1->theComputationSetup.theProver.FileHeaderString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileBodyString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileHeaderString.append("theProverHeader.txt");
  MainWindow1->theComputationSetup.theProver.FileBodyString.append("theProverBody.txt");
  MainWindow1->RunTheComputation();
}

void guiMainWindow::onButton12ProverOneStepFixedK(wxCommandEvent& ev)
{ MainWindow1->theComputationSetup.flagUsingProverDoNotCallOthers=true;
	MainWindow1->theComputationSetup.flagProverDoingFullRecursion=false;
  MainWindow1->theComputationSetup.flagProverUseFixedK=true;
  MainWindow1->theComputationSetup.flagOpenProverData=false;
  MainWindow1->theComputationSetup.flagSavingProverData=false;
  MainWindow1->RunTheComputation();
}

void guiMainWindow::onButton13ProverFullComputationFixedK(wxCommandEvent& ev)
{ MainWindow1->theComputationSetup.flagProverDoingFullRecursion=true;
  MainWindow1->theComputationSetup.flagUsingProverDoNotCallOthers=true;
  MainWindow1->theComputationSetup.flagProverUseFixedK=true;
  MainWindow1->theComputationSetup.flagOpenProverData=false;
  MainWindow1->theComputationSetup.flagSavingProverData=false;
  MainWindow1->RunTheComputation();
}

void guiMainWindow::onButton14ProverProverFixedKSave(wxCommandEvent& ev)
{ MainWindow1->theComputationSetup.flagUsingProverDoNotCallOthers=true;
  MainWindow1->theComputationSetup.flagSavingProverData=true;
  MainWindow1->theComputationSetup.flagOpenProverData=false;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName.append( "theProverFixedK.txt");
  MainWindow1->theComputationSetup.theProver.FileHeaderString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileBodyString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileHeaderString.append("theProverHeader.txt");
  MainWindow1->theComputationSetup.theProver.FileBodyString.append("theProverBody.txt");
  MainWindow1->RunTheComputation();
}

void guiMainWindow::onButton15ProverFixedKOpen(wxCommandEvent& ev)
{ MainWindow1->theComputationSetup.flagProverDoingFullRecursion=true;
  MainWindow1->theComputationSetup.flagUsingProverDoNotCallOthers=true;
  MainWindow1->theComputationSetup.flagOpenProverData=true;
  MainWindow1->theComputationSetup.flagSavingProverData=false;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProverFixedK.ProverFileName.append( "theProverFixedK.txt");
  MainWindow1->theComputationSetup.theProver.FileHeaderString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileBodyString=MainWindow1GlobalPath;
  MainWindow1->theComputationSetup.theProver.FileHeaderString.append("theProverHeader.txt");
  MainWindow1->theComputationSetup.theProver.FileBodyString.append("theProverBody.txt");
  MainWindow1->RunTheComputation();
}

void guiMainWindow::onButton8FullChop(wxCommandEvent &ev)
{ this->ReadVPVectorsAndOptions();
	this->theComputationSetup.flagDoCustomNilradical=false;
	this->theComputationSetup.FullChop(*this->theComputationSetup.theGlobalVariablesContainer->Default());
  this->Dialog1OutputPF->onToggleButton2ViewCombinatorialChambers(ev);
  this->Refresh();
}

void guiMainWindow::onButton9CustomNilradical(wxCommandEvent& ev)
{	this->theComputationSetup.flagDoCustomNilradical=true;
	this->theComputationSetup.SetupCustomNilradicalInVPVectors(*this->theComputationSetup.theGlobalVariablesContainer->Default());
	this->theComputationSetup.FullChop(*this->theComputationSetup.theGlobalVariablesContainer->Default());
  this->Dialog1OutputPF->onToggleButton2ViewCombinatorialChambers(ev);
  this->Refresh();
}

void guiMainWindow::onSpinner1and2(wxSpinEvent & ev)
{	int candidateDim= this->Spin1Dim->GetValue();
  int candidateNumVectors= this->Spin2NumVect->GetValue();
  if (candidateDim!= this->theComputationSetup.WeylGroupIndex || candidateNumVectors!=this->NumVectors)
  { if (candidateDim<1)
    { candidateDim=1;
      this->Spin1Dim->SetValue(1);
    }
    if (candidateNumVectors<1)
    { candidateNumVectors=1;
      this->Spin2NumVect->SetValue(1);
    }
    if (candidateDim>this->Table1Input->maxNumCols)
    { candidateDim=this->Table1Input->maxNumCols;
      this->Spin1Dim->SetValue(this->Table1Input->maxNumCols);
    }
    if (candidateNumVectors>this->Table1Input->maxNumRows)
    { candidateNumVectors=this->Table1Input->maxNumRows;
      this->Spin2NumVect->SetValue(this->Table1Input->maxNumRows);
    }
    this->initTableFromRowsAndColumns(candidateNumVectors,candidateDim);
  }
}

void guiMainWindow::updateInputButtons()
{ if (this->theComputationSetup.flagUsingCustomVectors)
  {	this->ListBox1WeylGroup->Disable();
    this->Spin1Dim->Enable();
    this->Spin2NumVect->Enable();
    this->Spin1Dim->SetValue(this->theComputationSetup.WeylGroupIndex);
    this->Spin2NumVect->SetValue(this->theComputationSetup.VPVectors.size);
    wxCommandEvent ev;
    this->onListBox1Change(ev);
  }
  else
  {	this->Spin1Dim->Disable();
    this->Spin2NumVect->Disable();
    this->ListBox1WeylGroup->Enable();
    wxCommandEvent ev;
    this->onListBox1Change(ev);
    this->initWeylGroupInfo();
  }
}

void guiMainWindow::onListBox1Change(wxCommandEvent &ev)
{ if (this->theComputationSetup.flagUsingCustomVectors)
		return;
  std::string tempS;
  unsigned char newWeylGroupIndex=0;
  char newWeylGroupLetter=0;
  int tempI=this->ListBox1WeylGroup->GetCurrentSelection();
	bool DoingCustomNilradical=false;
  switch (tempI)
  { case 0: newWeylGroupIndex=2;  newWeylGroupLetter='A'; break;
		case 1: newWeylGroupIndex=3;  newWeylGroupLetter='A'; break;
		case 2: newWeylGroupIndex=4;  newWeylGroupLetter='A'; break;
		case 3: newWeylGroupIndex=5;  newWeylGroupLetter='A'; break;
		case 4: newWeylGroupIndex=6;  newWeylGroupLetter='A'; break;
		case 5: newWeylGroupIndex=2;  newWeylGroupLetter='B'; break;
		case 6: newWeylGroupIndex=3;  newWeylGroupLetter='B'; break;
		case 7: newWeylGroupIndex=4;  newWeylGroupLetter='B'; break;
		case 8: newWeylGroupIndex=5;  newWeylGroupLetter='B'; break;//CombinatorialChamber::DisplayingGraphics=false; break;
		case 9: newWeylGroupIndex=2;  newWeylGroupLetter='C'; break;
		case 10: newWeylGroupIndex=3; newWeylGroupLetter='C'; break;
		case 11: newWeylGroupIndex=4; newWeylGroupLetter='C'; break;
		case 12: newWeylGroupIndex=4; newWeylGroupLetter='D'; break;
		case 13: newWeylGroupIndex=5; newWeylGroupLetter='D'; break;
		case 14: newWeylGroupIndex=2; newWeylGroupLetter='G'; break;
		case 15: newWeylGroupIndex=6; newWeylGroupLetter='E'; break;
		case 16: newWeylGroupIndex=7; newWeylGroupLetter='E'; break;
		case 17: newWeylGroupIndex=8; newWeylGroupLetter='E'; break;
		case 18: newWeylGroupIndex=4; newWeylGroupLetter='F'; break;
		case 19: newWeylGroupIndex=3; this->theComputationSetup.NumColsNilradical=2;
      this->theComputationSetup.NumRowsNilradical=2;
      newWeylGroupLetter='A';
      DoingCustomNilradical= true;
      break;
  }
	if(!DoingCustomNilradical)
	{	if (!(newWeylGroupLetter==this->theComputationSetup.WeylGroupLetter && newWeylGroupIndex==this->theComputationSetup.WeylGroupIndex))
		{ this->theComputationSetup.WeylGroupLetter=newWeylGroupLetter;
			this->theComputationSetup.WeylGroupIndex=newWeylGroupIndex;
			this->initWeylGroupInfo();
			this->theComputationSetup.Reset();
		}
	} else
	{ this->theComputationSetup.flagDoCustomNilradical=true;
		if (newWeylGroupLetter=='A')
			this->theComputationSetup.SetupCustomNilradicalInVPVectors(*this->theComputationSetup.theGlobalVariablesContainer->Default());
		this->initTableFromVPVectors();
		this->theComputationSetup.Reset();
	}
  if (this->theComputationSetup.WeylGroupIndex>5&& (newWeylGroupLetter!='A' && newWeylGroupLetter!='B' ))
		this->Button1Go->Disable();
  else
    this->Button1Go->Enable();
}

void guiMainWindow::WriteIndicatorWeight(root& tempRoot)
{	for (int i=0; i<tempRoot.size; i++)
	{	std::string tempS;
    tempRoot.TheObjects[i].ElementToString(tempS);
    this->Table2Indicator->SetCellValue(0,i,wxString(tempS.c_str(),wxConvUTF8));
    this->Table2Indicator->SetCellAlignment(0,i,wxALIGN_CENTER);
  }
}


void guiMainWindow::initWeylGroupInfo()
{ if (!this->theComputationSetup.flagUsingCustomVectors)
  { if (!this->theComputationSetup.flagDoCustomNilradical)
		{ WeylGroup tempW;
			tempW.MakeArbitrary(this->theComputationSetup.WeylGroupLetter,this->theComputationSetup.WeylGroupIndex);
			tempW.ComputeRho(true);
			this->theComputationSetup.VPVectors.CopyFromBase(tempW.RootsOfBorel);
			this->NumVectors=this->theComputationSetup.VPVectors.size;
			if (this->theComputationSetup.flagUsingIndicatorRoot)
				this->theComputationSetup.IndicatorRoot.Assign(tempW.rho);
    } else
		{
		}
  }
  this->initTableFromVPVectors();
  this->Table1Input->SetColLabelSize(0);
  this->Table1Input->SetRowLabelSize(0);
  this->Table2Indicator->SetColLabelSize(0);
  this->Table2Indicator->SetRowLabelSize(0);
  this->Table3Values->SetColLabelSize(0);
  this->Table3Values->SetRowLabelSize(0);
  //this->Table1Input->recalc();
}

void guiMainWindow::initTableFromVPVectors()
{	int theDimension=	this->theComputationSetup.WeylGroupIndex;
  this->initTableFromRowsAndColumns(this->theComputationSetup.VPVectors.size,theDimension);
	root tempRoot;
  tempRoot.MakeZero(theDimension);
  for (int i=0;i<this->theComputationSetup.VPVectors.size;i++)
  {	for (int j=0;j<theDimension;j++)
    {	std::string tempS;
      this->theComputationSetup.VPVectors.TheObjects[i].TheObjects[j].ElementToString(tempS);
      this->Table1Input->SetCellValue(i,j,wxString(tempS.c_str(),wxConvUTF8));
      this->Table1Input->SetCellAlignment(i,j,wxALIGN_CENTRE);
    }
    tempRoot.Add(this->theComputationSetup.VPVectors.TheObjects[i]);
  }
//	tempRoot.MultiplyByInteger(2);
  this->WriteIndicatorWeight(tempRoot);
}

void guiMainWindow::OnExit(wxCloseEvent &event)
{	this->Destroy();
}

void guiMainWindow::ReadVPVectorsAndOptions()
{	if (!this->theComputationSetup.flagComputationDone)
		return;
	this->theComputationSetup.flagComputingPartialFractions=! this->CheckBox1ComputePFs->GetValue();
  this->theComputationSetup.thePartialFraction.flagUsingCheckSum=this->CheckBox2CheckSums->GetValue();
  this->theComputationSetup.flagComputingChambers= this->CheckBox3ComputeChambers->GetValue();
  bool tempBool = this->CheckBox8DoTheWeylGroup->GetValue();
  if(	!(this->theComputationSetup.flagDoingWeylGroupAction== tempBool))
	{ this->theComputationSetup.Reset();
	}
  this->theComputationSetup.flagDoingWeylGroupAction= tempBool;
  this->theComputationSetup.flagUsingIndicatorRoot=!this->CheckBox7UseIndicatorForPFDecomposition->GetValue();
  this->ReadRBData();
  TDV.DrawChamberIndices= this->CheckBox4ChamberLabels->GetValue();
  TDV.DrawingInvisibles= this->CheckBox5InvisibleChambers->GetValue();
  TDV.DrawDashes = this->CheckBox6Dashes->GetValue();
  if (this->theComputationSetup.flagUsingCustomVectors)
  {	int theDimension=this->Spin1Dim->GetValue();
    this->theComputationSetup.WeylGroupIndex= theDimension;
    this->theComputationSetup.VPVectors.size=0;
    for (int i=0;i<this->Spin2NumVect->GetValue();i++)
    {	root tempRoot; tempRoot.SetSizeExpandOnTopLight(theDimension);
      for (int j=0;j<theDimension;j++)
      { int tempI=wxAtoi(this->Table1Input->GetCellValue(i,j));
        tempRoot.TheObjects[j].AssignInteger(tempI);
      }
			this->theComputationSetup.VPVectors.AddRoot(tempRoot);
    }
    this->theComputationSetup.VPVectors.ComputeDebugString();
  }
  this->theComputationSetup.IndicatorRoot.SetSizeExpandOnTopLight(this->theComputationSetup.WeylGroupIndex);
  for (int j=0;j<this->theComputationSetup.WeylGroupIndex;j++)
  {	int tempI=wxAtoi(this->Table2Indicator->GetCellValue(0,j));
    this->theComputationSetup.IndicatorRoot.TheObjects[j].AssignInteger(tempI);
  }
  this->theComputationSetup.IndicatorRoot.size=this->theComputationSetup.WeylGroupIndex;
}


void guiMainWindow::TurnOnAllDangerousButtons()
{	if (this->theComputationSetup.flagUsingCustomVectors)
  {	this->Spin1Dim->Enable();
    this->Spin2NumVect->Enable();
  }
  else
  {	this->ListBox1WeylGroup->Enable();
  }
  this->ToggleButton1UsingCustom->Enable();
}

void guiMainWindow::initTableFromRowsAndColumns(int r, int c)
{	this->NumVectors=r;
  this->theComputationSetup.WeylGroupIndex= c;
  this->Table2Indicator->SetNumRowsAndCols(1,c);
  this->Table3Values->SetNumRowsAndCols(1,c);
  this->Table1Input->SetNumRowsAndCols(this->NumVectors,this->theComputationSetup.WeylGroupIndex);
  if (r==0)
		return;
	for (int j=0;j<c;j++)
  {	this->Table1Input->SetColumnWidth(j,25);
    this->Table2Indicator->SetColumnWidth(j,25);
    this->Table3Values->SetColumnWidth(j,25);
  }
  this->Table2Indicator->SetRowHeight(0,25);
  this->Table3Values->SetRowHeight(0,25);
  for (int i=0;i<r;i++)
  	this->Table1Input->SetRowHeight(i,25);
  if (this->Table1Input->GetNumberRows()>20)
  {	this->Table1Input->SetSize(0,70,220,500);
    this->Table1Input->SetMaxSize(wxSize(220,500));
    this->SetAutoLayout(false);
  }	else
	{	this->BoxSizer2VerticalInputs->Layout();
    this->BoxSizer8HorizontalEval->Layout();
    this->BoxSizer1HorizontalBackground->Layout();
  }
  if (r>this->MaxAllowedVectors)
  {//this->Button1Go->Disable();
  }
  else
  	this->Button1Go->Enable();
#ifndef WIN32
//    this->Layout();
  this->BoxSizer8HorizontalEval->Fit(this->Table3Values);
  this->BoxSizer4VerticalToggleButton1->Fit(this->Table2Indicator);
  this->BoxSizer2VerticalInputs->Fit(this->Table1Input);
#endif
}

void guiMainWindow::TurnOffAllDangerousButtons()
{	MainWindow1->ListBox1WeylGroup->Disable();
  MainWindow1->Spin1Dim->Disable();
  MainWindow1->Spin2NumVect->Disable();
  MainWindow1->ToggleButton1UsingCustom->Disable();
}

wxGridExtra::wxGridExtra(	wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{	this->maxNumCols=8;
  this->maxNumRows=120;
  this->Create(parent,id,pos,size,style,name);
}

void wxGridExtra::SetNumRowsAndCols(int r, int c)
{	if (r<0 || r>this->maxNumRows|| c<0 ||c>this->maxNumCols)
		return;
  int oldNumRows=	this->GetNumberRows();
  int oldNumCols= this->GetNumberCols();
  if (oldNumRows>r)
  {	this->DeleteRows(r,oldNumRows-r,true);
    oldNumRows=r;
  }
  if (oldNumRows<r)
  {	this->InsertRows(oldNumRows-1,r-oldNumRows,true);
    oldNumRows=r;
  }
  if (oldNumCols>c)
  {	this->DeleteCols(c,oldNumCols-c,true);
    oldNumCols=c;
  }
  if (oldNumCols<c)
  {	this->InsertCols(oldNumCols-1,c-oldNumCols,true);
    oldNumCols=c;
  }
}


void WorkThreadClass::run()
{
#ifdef WIN32
    ::RunComputationalThread();
#else
    RunComputationalThread(0);
#endif
}



void outputText(std::string& theOutput)
{
}

void guiMainWindow::WriteSettingsIfAvailable()
{	if (this->fileSettings.is_open())
  {	this->fileSettings.clear();
    this->fileSettings.seekp(0);
    int x,y;
    this->GetPosition(&x,&y);
    this->fileSettings  <<"  Main_window_top_left_corner_x " <<x<<"   \nMain_window_top_left_corner_y " <<y<<"    ";
    this->GetSize(& x, & y);
    this->fileSettings  <<"  \nMain_window_width " <<x<<"   \nMain_window_height "  <<y<<"    ";
    this->Dialog1OutputPF->GetPosition(&x,&y);
    this->fileSettings  <<"  \nPartial_fraction_window_top_left_corner_x " <<x<<"   \nPartial_fraction_window_top_left_corner_y "  <<y<<"    ";
    this->Dialog1OutputPF->GetSize(&x,&y);
    this->fileSettings  <<"  \nPartial_fraction_window_width " <<x<<"   \nPartial_fraction_window_height "  <<y <<"    ";
    this->Dialog2StatusString1->GetPosition(&x,&y);
    this->fileSettings  <<"  \nStatus_string1_window_top_left_corner_x " <<x <<"   \nStatus_string1_top_left_corner_y "  <<y<<"    ";
    this->Dialog2StatusString1->GetSize(&x,&y);
    this->fileSettings <<"\nStatus_string1_window_width " <<x <<"   \nStatus_string1_window_height "  <<y <<"    ";
    this->fileSettings.flush();
  }
}

void guiMainWindow::ReadSettingsIfAvailable()
{	if (::MainWindow1GlobalPath=="")
		return;
  std::stringstream out;
  out << ::MainWindow1GlobalPath<<"vector_partition_settings.txt";
  std::string tempS;
  tempS= out.str();
  if (rootFKFTcomputation::OpenDataFileOrCreateIfNotPresent2(this->fileSettings,tempS,false,false))
  {	wxPoint tempPt, tempPt2, tempPt3;
    wxSize tempSize, tempSize2, tempSize3;
    this->fileSettings.seekg(0);
    this->fileSettings>>tempS >> tempPt.x;
    this->fileSettings>>tempS >> tempPt.y;
    this->fileSettings>>tempS >> tempSize.x;
    this->fileSettings>>tempS >> tempSize.y;
    this->fileSettings>>tempS >> tempPt2.x;
    this->fileSettings>>tempS >> tempPt2.y;
    this->fileSettings>>tempS >> tempSize2.x;
    this->fileSettings>>tempS >> tempSize2.y;
    this->fileSettings>>tempS >> tempPt3.x;
    this->fileSettings>>tempS >> tempPt3.y;
    this->fileSettings>>tempS >> tempSize3.x;
    this->fileSettings>>tempS >> tempSize3.y;
    this->SetPosition(tempPt);
    this->SetSize(tempSize);
    this->Dialog1OutputPF->SetPosition(tempPt2);
    this->Dialog1OutputPF->SetSize(tempSize2);
    this->Dialog2StatusString1->SetPosition(tempPt3);
    this->Dialog2StatusString1->SetSize(tempSize3);
  }
}

void guiMainWindow::ArrangeWindows()
{ this->ReadSettingsIfAvailable();
}

void guiMainWindow::updatePartialFractionAndCombinatorialChamberTextData()
{ std::stringstream out;
  wxString tempWS(MainWindow1->theComputationSetup.theOutput.DebugString.c_str(), wxConvUTF8);
  MainWindow1->Text1Output->SetValue(tempWS);
  int old= this->Text3PartialFractions->GetLastPosition();
  if (this->theComputationSetup.flagDisplayingPartialFractions)
  { wxString tempWS(MainWindow1->theComputationSetup.thePartialFraction.DebugString.c_str(),wxConvUTF8);
    MainWindow1->Text3PartialFractions->SetValue(tempWS);
  } else
    if (this->theComputationSetup.flagDisplayingCombinatorialChambersTextData)
    { out <<"\\documentclass{article}\\begin{document}"<<this->theComputationSetup.theChambers.DebugString<<"\\end{document}";
      MainWindow1->bufferString1=out.str();
      wxString tempWS(MainWindow1->bufferString1.c_str(),wxConvUTF8);
      MainWindow1->Text3PartialFractions->SetValue(tempWS);
    }
  this->Text3PartialFractions->ShowPosition(old);
}

void guiMainWindow::onCheckBoxesGraphics(wxCommandEvent& ev)
{	TDV.DrawChamberIndices= this->CheckBox4ChamberLabels->GetValue();
  TDV.DrawingInvisibles= this->CheckBox5InvisibleChambers->GetValue();
  TDV.DrawDashes = this->CheckBox6Dashes->GetValue();
  this->Refresh();
}

void guiMainWindow::ReadRBData()
{	switch(this->RBGroup1SlicingOptions->GetSelection())
	{ case 0: this->theComputationSetup.flagFullChop=true; break;
		case 1: this->theComputationSetup.flagFullChop=false; this->theComputationSetup.flagOneIncrementOnly=true;	break;
		case 2: this->theComputationSetup.flagFullChop=false; this->theComputationSetup.flagOneIncrementOnly=false; break;
	}
}

void guiMainWindow::onRBGroup1SlicingOptions(wxCommandEvent& ev)
{ this->ReadRBData();
}


void guiMainWindow::onComputationOver(wxCommandEvent& ev)
{ this->updatePartialFractionAndCombinatorialChamberTextData();
  MainWindow1->TurnOnAllDangerousButtons();
  MainWindow1->Button1Go->SetLabel(wxT("Go"));
  MainWindow1->Refresh();
}

void guiMainWindow::onProgressReport(::wxCommandEvent& ev)
{	IndicatorWindowVariables& output= MainWindow1->progressReportVariables;
  //if (output.Busy)
	//	return;
	output.Busy=true;
	if (output.String1NeedsRefresh)
  {	wxString tempS1(output.ProgressReportString1.c_str(),wxConvUTF8);
		MainWindow1->Label1ProgressReport->SetLabel(tempS1);
  }
  if (output.String2NeedsRefresh)
  {	wxString tempS2(output.ProgressReportString2.c_str(),wxConvUTF8);
		MainWindow1->Label2ProgressReport->SetLabel(tempS2);
  }
  if (output.String3NeedsRefresh)
  {	wxString tempS3(output.ProgressReportString3.c_str(),wxConvUTF8);
		MainWindow1->Label3ProgressReport->SetLabel(tempS3);
  }
  if (output.String4NeedsRefresh)
  {	wxString tempS4(output.ProgressReportString4.c_str(),wxConvUTF8);
		MainWindow1->Label4ProgressReport->SetLabel(tempS4);
  }
  if (output.String5NeedsRefresh)
  {	wxString tempS5(output.ProgressReportString5.c_str(),wxConvUTF8);
		MainWindow1->Label5ProgressReport->SetLabel(tempS5);
  }
  if (output.StatusString1NeedsRefresh)
  {	wxString tempS6(output.StatusString1.c_str(),wxConvUTF8);
		MainWindow1->Text4StatusString1->SetValue(tempS6);
		MainWindow1->Text4StatusString1->Refresh();
  }
  if (output.flagRootIsModified)
  {	root tempRoot;
    tempRoot.AssignIntRoot(output.modifiedRoot);
    MainWindow1->WriteIndicatorWeight(tempRoot);
  }
  output.Busy=false;
}

void FeedDataToIndicatorWindowWX(IndicatorWindowVariables& output)
{	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=true;
  if (MainWindow1->WorkThread1.CriticalSectionPauseButtonEntered)
  {	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
    return;
  }
  if (MainWindow1->progressReportVariables.Busy)
		return;
  MainWindow1->progressReportVariables.Busy=true;
  MainWindow1->progressReportVariables.Assign(output);
  wxPostEvent(MainWindow1->GetEventHandler(), MainWindow1->wxProgressReportEvent);
  MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
  MainWindow1->progressReportVariables.Busy=false;
}


#endif

