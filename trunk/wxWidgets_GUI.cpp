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
#include "wx/spinbutt.h"
#include "wx/menu.h"
#include "wx/spinctrl.h"
#include "wx/textctrl.h"
#ifndef WIN32
  #include <unistd.h>
#endif

#include "polyhedra.h"

extern DrawingVariables TDV;
extern int NextDirectionIndex;
extern int RankGlobal;
extern roots InputRoots;
extern CombinatorialChamberPointers TheBigOutput;
extern FacetPointers TheBigFacetOutput;

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
	wxGridExtra( wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxWANTS_CHARS,
            const wxString& name = wxT("") );
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
  wxComboBoxWheel( wxWindow *parent,
                    wxWindowID id,
                    const wxString& value= wxT(""),
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    const int n=0 ,const wxString choices[]=0,
                    const long style = wxWANTS_CHARS,
                    const wxValidator& validator=wxDefaultValidator,
                    const wxString& name = wxT(""));
  void OnMouseWheel(wxMouseEvent& event);
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxComboBoxWheel, wxComboBox)
   EVT_MOUSEWHEEL(wxComboBoxWheel::OnMouseWheel)
END_EVENT_TABLE()

wxComboBoxWheel::wxComboBoxWheel( wxWindow *parent,
                    wxWindowID id,
                    const wxString& value,
                    const wxPoint& pos ,
                    const wxSize& size ,
                    const int n ,const wxString choices[],
                    const long style ,
                    const wxValidator& validator,
                    const wxString& name )
:wxComboBox(parent,id,value,pos,size,n,choices,style,validator,name)
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
	int ClickToleranceX; int ClickToleranceY;
	drawCanvas(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
	{ this->Create(parent,winid,pos,size,style,name);
	}
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
  wxTextCtrl* Text1Output;
  wxTextCtrl* Text2Values;
	wxFont* theFont;
	::wxStaticText* Label1ProgressReport;
	::wxStaticText* Label2ProgressReport;
	::wxStaticText* Label3ProgressReport;
	::wxStaticText* Label4ProgressReport;
	::wxStaticText* Label5ProgressReport;
	::wxStaticText* Label6ProgressReport;
	::wxStaticText* Label7Dim;
	::wxStaticText* Label8NumVect;
  ::wxComboBoxWheel* ListBox1WeylGroup;
  ::wxButton* Button1Go;
  ::wxButton* Button2Eval;
  ::wxButton* Button3Custom;
  ::wxSpinCtrl* Spin1Dim;
  ::wxSpinCtrl* Spin2NumVect;
	::wxCheckBox* CheckBox1ComputePFs;
	::wxCheckBox* CheckBox2CheckSums;
	::wxCheckBox* CheckBox3ComputeChambers;
  ::drawCanvas* Canvas1;
  ::wxToggleButton* ToggleButton1UsingCustom;
	::IndicatorWindowVariables progressReportVariables;
	//wxEVT_ProgressReport ProgressReportEvent;
  void onToggleButton1UsingCustom( wxCommandEvent& ev);
  void onListBox1Change(wxCommandEvent& ev);
  void onButton2Eval(wxCommandEvent& ev);
  void onButton1Go(wxCommandEvent& ev);
  void onButton3Custom (wxCommandEvent& ev);
	void onSpinner1and2 (wxSpinEvent & ev);
	void onComputationOver(wxCommandEvent& ev);
	void onRePaint(wxPaintEvent& ev);
	void onMouseDownOnCanvas(wxMouseEvent& ev);
	void ReadVPVectorsAndOptions();
	void WriteIndicatorWeight(root& tempRoot);
	void onProgressReport(wxCommandEvent & ev);
	void TurnOnAllDangerousButtons();
	void TurnOffAllDangerousButtons();
	void updateInputButtons();
	void initWeylGroupInfo();
	void initTableFromVPVectors();
	void initTableFromRowsAndColumns(int r, int c);
  guiMainWindow();
  ~guiMainWindow();
  void OnExit(wxCloseEvent& event);
  static const int DefaultButtonHeight=30;
  static const int DefaultButtonWidth=100;
  enum
  { ID_MainWindow,
		ID_ToggleButton1UsingCustom = 100,
		ID_ListBox1,
		ID_Button1Go,
		ID_Buton2Eval,
		ID_Spin1Dim,
		ID_Spin2NumVect,
		ID_Canvas1,
		ID_ComputationUpdate,
		ID_CheckBox1,
		ID_Button3Custom,
		ID_Paint,
  };
	DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP( guiApp )

guiMainWindow *MainWindow1;
bool guiApp::OnInit()
{ MainWindow1 = new guiMainWindow;
  MainWindow1->Show(true);
  return true;
}

BEGIN_EVENT_TABLE( guiMainWindow, wxFrame )
	EVT_TOGGLEBUTTON(guiMainWindow::ID_ToggleButton1UsingCustom, guiMainWindow::onToggleButton1UsingCustom)
	EVT_BUTTON(guiMainWindow::ID_Buton2Eval, guiMainWindow::onButton2Eval)
	EVT_BUTTON(guiMainWindow::ID_Button1Go, guiMainWindow::onButton1Go)
	EVT_BUTTON(guiMainWindow::ID_Button3Custom, guiMainWindow::onButton3Custom)
	EVT_COMBOBOX(guiMainWindow::ID_ListBox1, guiMainWindow::onListBox1Change)
	EVT_SPINCTRL(guiMainWindow::ID_Spin1Dim, guiMainWindow::onSpinner1and2)
	EVT_SPINCTRL(guiMainWindow::ID_Spin2NumVect, guiMainWindow::onSpinner1and2)
	EVT_SIZING(drawCanvas::onSizing)
	EVT_SIZE(drawCanvas::onSizing)
	EVT_COMMAND  (guiMainWindow::ID_MainWindow,::wxEVT_ProgressReport,guiMainWindow::onProgressReport )
	EVT_COMMAND  (guiMainWindow::ID_MainWindow,::wxEVT_ComputationFinished,guiMainWindow::onComputationOver )
	EVT_CLOSE(guiMainWindow::OnExit)
	//EVT_PAINT(guiMainWindow::onPaint)

END_EVENT_TABLE()

#ifdef WIN32
  void RunComputationalThread()
#else
  void* RunComputationalThread(void*ptr)
#endif
{ MainWindow1->ReadVPVectorsAndOptions();
	MainWindow1->theComputationSetup.Run();
	::wxPostEvent(MainWindow1->GetEventHandler(),MainWindow1->wxComputationOver);
#ifndef WIN32
	pthread_exit(NULL);
#endif
}


void wxComboBoxWheel::OnMouseWheel(wxMouseEvent& event)
{	// scroll in drop down list using mouse wheel
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
{ int Realx=ev.GetX();//-this->Canvas1->GetRect().GetTop();
	int Realy=ev.GetY();//-this->GetRect().GetLeft();
	if (TDV.Selected==-2)
	{	double tempX,tempY;
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
		{	TDV.Selected=-1;
		}
	}
	else
	{	if (TDV.Selected==-1)
		{	TDV.centerX = (double) Realx;
			TDV.centerY = (double) Realy;
		}
		else
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

guiMainWindow::guiMainWindow()
				: wxFrame( (wxFrame *)NULL, guiMainWindow::ID_MainWindow, wxT("I will eat your RAM (but not as much as in the past)"),
                   wxPoint(100,100),
                   wxSize(800,600))
{	this->BoxSizer1HorizontalBackground = new ::wxBoxSizer(wxHORIZONTAL);
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
	this->ToggleButton1UsingCustom= new ::wxToggleButton(this,guiMainWindow::ID_ToggleButton1UsingCustom,wxT("Switch to custom"));
  this->Table1Input = new ::wxGridExtra( this,wxID_ANY);
  this->Table2Indicator = new wxGridExtra( this,::wxID_ANY);
  this->Table3Values = new wxGridExtra( this,::wxID_ANY);
	this->Label1ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label2ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label3ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label4ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label5ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Spin1Dim = new wxSpinCtrl(this,this->ID_Spin1Dim);
	this->Spin2NumVect= new wxSpinCtrl(this, this->ID_Spin2NumVect);
	this->CheckBox1ComputePFs= new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Don't compute PF"));
	this->CheckBox2CheckSums= new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Checksums"));
	this->CheckBox3ComputeChambers= new ::wxCheckBox(this,this->ID_CheckBox1,wxT("Compute chambers"));
	//this->Spin2NumVect->SetSize(this->DefaultButtonWidth,this->DefaultButtonHeight);
	//this->Spin1Dim->SetSize(this->DefaultButtonWidth,this->DefaultButtonHeight);
	this->Canvas1 = new ::drawCanvas(this,::wxID_ANY,::wxDefaultPosition,::wxDefaultSize,::wxEXPAND|wxALL);
	//this->Label5ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
  this->Table1Input->CreateGrid( 0, 0 );
  this->Table2Indicator->CreateGrid( 0, 0 );
  this->Table3Values->CreateGrid(0,0);
	this->theFont= new ::wxFont(10,wxDEFAULT, wxNORMAL,wxNORMAL);
	this->ListBox1WeylGroup= new ::wxComboBoxWheel(this,this->ID_ListBox1,wxT("A3"),
																								wxPoint(0,0),::wxDefaultSize// wxSize(this->DefaultButtonWidth, this->DefaultButtonHeight)
																								,0,0,wxCB_DROPDOWN  );
	this->Button2Eval= new ::wxButton(this,this->ID_Buton2Eval, wxT("Evaluate"));
	this->Button2Eval->SetSize(this->DefaultButtonWidth, this->DefaultButtonHeight);
	this->Button1Go= new ::wxButton(this,this->ID_Button1Go,wxT("Go"));
	this->Button1Go->SetSize(this->DefaultButtonWidth, this->DefaultButtonHeight);
	this->Button3Custom= new wxButton(this,this->ID_Button3Custom,wxT("Experiments"));
	this->Text1Output= new ::wxTextCtrl(this,::wxID_ANY,wxT(""),::wxDefaultPosition, ::wxDefaultSize,wxTE_MULTILINE);
	this->Text2Values= new ::wxTextCtrl(this,::wxID_ANY);
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
			this->BoxSizer2VerticalInputs->Add(this->Text1Output,1,::wxEXPAND| ::wxALL);
	this->BoxSizer1HorizontalBackground->Add(this->BoxSizer5VerticalCanvasAndProgressReport,1,wxEXPAND|wxALL);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->BoxSizer10HorizontalProgressReportsAndOptions);
			this->BoxSizer10HorizontalProgressReportsAndOptions->Add(this->BoxSizer11VerticalOptions);
				this->BoxSizer11VerticalOptions->Add(this->CheckBox1ComputePFs);
				this->BoxSizer11VerticalOptions->Add(this->CheckBox2CheckSums);
				this->BoxSizer11VerticalOptions->Add(this->CheckBox3ComputeChambers);
				this->BoxSizer11VerticalOptions->Add(this->Button3Custom);
			this->BoxSizer10HorizontalProgressReportsAndOptions->Add(this->BoxSizer12VerticalProgressReports);
				this->BoxSizer12VerticalProgressReports->Add(this->Label1ProgressReport);
				this->BoxSizer12VerticalProgressReports->Add(this->Label2ProgressReport);
				this->BoxSizer12VerticalProgressReports->Add(this->Label3ProgressReport);
				this->BoxSizer12VerticalProgressReports->Add(this->Label4ProgressReport);
				this->BoxSizer12VerticalProgressReports->Add(this->Label5ProgressReport);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Canvas1,1,wxEXPAND|wxALL);
  this->ListBox1WeylGroup->Append(wxT("A2"));
  this->ListBox1WeylGroup->Append(wxT("A3"));
  this->ListBox1WeylGroup->Append(wxT("A4"));
  this->ListBox1WeylGroup->Append(wxT("A5"));
	this->ListBox1WeylGroup->Append(wxT("B2"));
  this->ListBox1WeylGroup->Append(wxT("B3"));
  this->ListBox1WeylGroup->Append(wxT("B4"));
  this->ListBox1WeylGroup->Append(wxT("C2"));
  this->ListBox1WeylGroup->Append(wxT("C3"));
  this->ListBox1WeylGroup->Append(wxT("C4"));
  this->ListBox1WeylGroup->Append(wxT("D4"));
	this->ListBox1WeylGroup->Append(wxT("G2"));
	this->ListBox1WeylGroup->Append(wxT("E6"));
	this->ListBox1WeylGroup->Append(wxT("E7"));
	this->ListBox1WeylGroup->Append(wxT("E8"));
	this->ListBox1WeylGroup->Append(wxT("F4"));
	this->ListBox1WeylGroup->SetSelection(1);
	this->CheckBox1ComputePFs->SetValue(false);
	this->CheckBox2CheckSums->SetValue(false);
	this->CheckBox3ComputeChambers->SetValue(true);
	this->theComputationSetup.ComputationInProgress=false;
	this->theComputationSetup.AllowRepaint=true;
	this->theComputationSetup.UsingCustomVectors=false;
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
	TDV.centerX=150;
	TDV.centerY=200;
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
#ifndef WIN32
  pthread_mutex_init(&ParallelComputing::mutex1, NULL);
  pthread_cond_init (&ParallelComputing::continueCondition, NULL);
#endif
	Centre();
}
void drawCanvas::onSizing(wxSizeEvent& ev)
{ if (MainWindow1->Table1Input->GetNumberRows()>20)
	{	MainWindow1->Table1Input->SetAutoLayout(false);
		MainWindow1->Table1Input->SetSize(0,70,220,500);
		MainWindow1->Table1Input->SetMaxSize(wxSize(220,500));
	}
  MainWindow1->Layout();
  this->Refresh();//true,&tempRect);
}

guiMainWindow::~guiMainWindow()
{ //this->theFont
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
}


void guiMainWindow::onButton3Custom(wxCommandEvent& ev)
{ rootFKFTComputationLocal.useOutputFileForFinalAnswer=false;
	this->theComputationSetup.AllowRepaint=false;
#ifndef WIN32
  RandomCodeIDontWantToDelete::SomeRandomTests2();
  rootFKFTComputationLocal.KLCoeffFileString="/home/todor/math/KLcoeff.txt";
  rootFKFTComputationLocal.PartialFractionsFileString="/home/todor/math/partialFractions.txt";
  rootFKFTComputationLocal.VPEntriesFileString="/home/todor/math/VPdata.txt";
  rootFKFTComputationLocal.VPIndexFileString="/home/todor/math/VPIndex.txt";
  ::RunA_voidFunctionFromFunctionAddress(&RunrootFKFTComputationLocal,&this->WorkThread1.ComputationalThreadLinux);
#else
  rootFKFTComputationLocal.KLCoeffFileString="C:/math/rootFKFT/cpp/KLcoeff.txt";
  rootFKFTComputationLocal.PartialFractionsFileString="C:/math/rootFKFT/cpp/partialFractions.txt";
  rootFKFTComputationLocal.VPEntriesFileString="C:/math/rootFKFT/cpp/VPdata.txt";
  rootFKFTComputationLocal.VPIndexFileString="C:/math/rootFKFT/cpp/VPIndex.txt";
	::RunA_voidFunctionFromFunctionAddress
		(&RunrootFKFTComputationLocal,this->WorkThread1.ComputationalThread);
#endif
}

void guiMainWindow::onToggleButton1UsingCustom( wxCommandEvent& ev)
{ if (!this->ToggleButton1UsingCustom->GetValue())
	{	this->theComputationSetup.UsingCustomVectors=false;
		this->ToggleButton1UsingCustom->SetLabel(wxT("Switch to custom"));
	}else
	{ this->theComputationSetup.UsingCustomVectors=true;
		this->ToggleButton1UsingCustom->SetLabel(wxT("Switch to roots"));
	}
	this->updateInputButtons();
}

void guiMainWindow::onRePaint(wxPaintEvent& ev)
{	::wxPaintDC dc;
	this->Refresh();
}

void drawCanvas::OnPaint(::wxPaintEvent& ev)
{	::wxPaintDC  dc(this);
	if (MainWindow1->theComputationSetup.AllowRepaint)
	{	root::AmbientDimension= TheBigOutput.AmbientDimension;
		dc.SetBackground(MainWindow1->GetBackgroundColour());
		dc.DrawRectangle(wxPoint(0,0),this->GetSize());
		::drawOutput(::TDV,::TheBigOutput,::InputRoots,::NextDirectionIndex,TheBigOutput.IndicatorRoot);
	}
}

void guiMainWindow::onButton1Go(wxCommandEvent &ev)
{	::RandomCodeIDontWantToDelete::SomeRandomTests2();
	this->TurnOffAllDangerousButtons();
//#ifdef WIN32
	if (!this->theComputationSetup.ComputationInProgress)
	{ this->theComputationSetup.ComputationInProgress=true;
		this->Button1Go->SetLabel(wxT("Pause"));
		#ifdef WIN32
      this->WorkThread1.ComputationalThread=CreateThread(0,0, (LPTHREAD_START_ROUTINE)RunComputationalThread,0,0,0);
    #else
      //RunComputationalThread(0);
      pthread_create(&this->WorkThread1.ComputationalThreadLinux, NULL,RunComputationalThread, 0);
    #endif
	}
	else
	{ if(this->WorkThread1.isRunning)
		{ //pthread_mutex_lock(&ParallelComputing::mutex1);
      this->WorkThread1.CriticalSectionPauseButtonEntered=true;
			//return;
			while(this->WorkThread1.CriticalSectionWorkThreadEntered)
			{}
			ParallelComputing::ReachSafePointASAP=true;
			while(!ParallelComputing::SafePointReached)
			{}
			ParallelComputing::ReachSafePointASAP=false;
			#ifdef WIN32
        ::SuspendThread(this->WorkThread1.ComputationalThread);
      #endif
			this->WorkThread1.isRunning=false;
			this->Button1Go->SetLabel(wxT("Go"));
			this->WorkThread1.CriticalSectionWorkThreadEntered=false;
			this->WorkThread1.CriticalSectionPauseButtonEntered=false;
			//pthread_mutex_unlock(&ParallelComputing::mutex1);
		}
		else
		{	//pthread_mutex_lock(&ParallelComputing::mutex1);
		  this->Button1Go->SetLabel(wxT("Pause"));
			this->WorkThread1.isRunning=true;
			#ifdef WIN32
        ::ResumeThread(this->WorkThread1.ComputationalThread);
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

void guiMainWindow::onButton2Eval(wxCommandEvent &ev)
{	if (this->theComputationSetup.theOutput.NumVars!=this->Table3Values->GetNumberCols())
	{ return;
	}
	intRoot tempRoot;
	for (int i=0;i<this->theComputationSetup.theOutput.NumVars;i++)
	{ this->theComputationSetup.ValueRoot.elements[i]= wxAtoi(this->Table3Values->GetCellValue(0,i));
	}
	this->theComputationSetup.EvaluatePoly();
	this->Text2Values->SetValue(wxString(this->theComputationSetup.ValueString.c_str(),wxConvUTF8));
	return;
}

void guiMainWindow::onSpinner1and2(wxSpinEvent & ev)
{ int candidateDim= this->Spin1Dim->GetValue();
	int candidateNumVectors= this->Spin2NumVect->GetValue();
	if (candidateDim!= this->theComputationSetup.WeylGroupIndex ||
			candidateNumVectors!=this->NumVectors)
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
{ if (this->theComputationSetup.UsingCustomVectors)
	{ this->ListBox1WeylGroup->Disable();
		this->Spin1Dim->Enable();
		this->Spin2NumVect->Enable();
		this->Spin1Dim->SetValue(this->theComputationSetup.WeylGroupIndex);
		this->Spin2NumVect->SetValue(this->theComputationSetup.VPVectors.size);
		wxCommandEvent ev;
		this->onListBox1Change(ev);
	}
	else
	{ this->Spin1Dim->Disable();
		this->Spin2NumVect->Disable();
		this->ListBox1WeylGroup->Enable();
		wxCommandEvent ev;
		this->onListBox1Change(ev);
		this->initWeylGroupInfo();
	}
}

void guiMainWindow::onListBox1Change(wxCommandEvent &ev)
{	if (this->theComputationSetup.UsingCustomVectors)
		return;
	std::string tempS;
	unsigned char newWeylGroupIndex=0;
	char newWeylGroupLetter=0;
	int tempI=this->ListBox1WeylGroup->GetCurrentSelection();
	switch(tempI)
	{ case 0:	newWeylGroupIndex=2;  newWeylGroupLetter='A'; break;
		case 1:	newWeylGroupIndex=3;  newWeylGroupLetter='A'; break;
		case 2:	newWeylGroupIndex=4;  newWeylGroupLetter='A'; break;
		case 3:	newWeylGroupIndex=5;  newWeylGroupLetter='A'; break;
		case 4:	newWeylGroupIndex=2;  newWeylGroupLetter='B'; break;
		case 5:	newWeylGroupIndex=3;  newWeylGroupLetter='B'; break;
		case 6:	newWeylGroupIndex=4;  newWeylGroupLetter='B'; break;
		case 7:	newWeylGroupIndex=2;  newWeylGroupLetter='C'; break;
		case 8:	newWeylGroupIndex=3;  newWeylGroupLetter='C'; break;
		case 9:	newWeylGroupIndex=4;  newWeylGroupLetter='C'; break;
		case 10: newWeylGroupIndex=4; newWeylGroupLetter='D'; break;
		case 11: newWeylGroupIndex=2; newWeylGroupLetter='G'; break;
		case 12: newWeylGroupIndex=6; newWeylGroupLetter='E'; break;
		case 13: newWeylGroupIndex=7; newWeylGroupLetter='E'; break;
		case 14: newWeylGroupIndex=8; newWeylGroupLetter='E'; break;
		case 15: newWeylGroupIndex=4; newWeylGroupLetter='F'; break;
	//	case 12: this->UsingCustomVectors=true;
	}
	if (!(newWeylGroupLetter==this->theComputationSetup.WeylGroupLetter &&
				newWeylGroupIndex==this->theComputationSetup.WeylGroupIndex))
	{ this->theComputationSetup.WeylGroupLetter=newWeylGroupLetter;
		this->theComputationSetup.WeylGroupIndex=newWeylGroupIndex;
		this->initWeylGroupInfo();
	}
	if (this->theComputationSetup.WeylGroupIndex>5)
	{ this->Button1Go->Disable();
	}
	else
	{ this->Button1Go->Enable();
	}
}

void guiMainWindow::WriteIndicatorWeight(root& tempRoot)
{ for (int i=0;i<tempRoot.size;i++)
	{ std::string tempS;
		tempRoot.TheObjects[i].ElementToString(tempS);
		this->Table2Indicator->SetCellValue(0,i,wxString(tempS.c_str(),wxConvUTF8));
		this->Table2Indicator->SetCellAlignment(0,i,wxALIGN_CENTER);
	}
}


void guiMainWindow::initWeylGroupInfo()
{ if (!this->theComputationSetup.UsingCustomVectors)
	{	WeylGroup tempW;
		tempW.MakeArbitrary(this->theComputationSetup.WeylGroupLetter,
												this->theComputationSetup.WeylGroupIndex);
		tempW.ComputeRho();
		this->theComputationSetup.VPVectors.CopyFromBase(tempW.RootsOfBorel);
		this->NumVectors=this->theComputationSetup.VPVectors.size;
		if (this->theComputationSetup.thePartialFraction.flagUsingIndicatorRoot)
		{	this->theComputationSetup.thePartialFraction.IndicatorRoot.Assign(tempW.rho);}
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
{ 	this->initTableFromRowsAndColumns(this->theComputationSetup.VPVectors.size,
																			root::AmbientDimension);
		root tempRoot; tempRoot.MakeZero();
		for (int i=0;i<this->theComputationSetup.VPVectors.size;i++)
		{ for (int j=0;j<root::AmbientDimension;j++)
			{ std::string tempS;
				this->theComputationSetup.VPVectors.TheObjects[i].TheObjects[j].ElementToString(tempS);
				this->Table1Input->SetCellValue(i,j,wxString(tempS.c_str(),wxConvUTF8));
				this->Table1Input->SetCellAlignment(i,j,wxALIGN_CENTRE);
			}
			tempRoot.Add(this->theComputationSetup.VPVectors.TheObjects[i]);
		}
//		tempRoot.MultiplyByInteger(2);
		this->WriteIndicatorWeight(tempRoot);
}


void guiMainWindow::OnExit(wxCloseEvent &event)
{ this->Destroy();
}


void guiMainWindow::ReadVPVectorsAndOptions()
{ this->theComputationSetup.ComputingPartialFractions=! this->CheckBox1ComputePFs->GetValue();
	this->theComputationSetup.MakingCheckSumPFsplit=this->CheckBox2CheckSums->GetValue();
	this->theComputationSetup.ComputingChambers= this->CheckBox3ComputeChambers->GetValue();
	if (this->theComputationSetup.UsingCustomVectors)
	{	root::AmbientDimension= this->Spin1Dim->GetValue();
		this->theComputationSetup.VPVectors.size=0;
		for (int i=0;i<this->Spin2NumVect->GetValue();i++)
		{ root tempRoot;
			for (int j=0;j<root::AmbientDimension;j++)
			{	int tempI=wxAtoi(this->Table1Input->GetCellValue(i,j));
				tempRoot.TheObjects[j].AssignInteger(tempI);
			}
			this->theComputationSetup.VPVectors.AddRoot(tempRoot);
		}
		this->theComputationSetup.VPVectors.ComputeDebugString();
	}
	for (int j=0;j<this->theComputationSetup.WeylGroupIndex;j++)
	{	int tempI=wxAtoi(this->Table2Indicator->GetCellValue(0,j));
		this->theComputationSetup.thePartialFraction.IndicatorRoot.TheObjects[j].AssignInteger(tempI);
	}
	this->theComputationSetup.thePartialFraction.IndicatorRoot.size=this->theComputationSetup.WeylGroupIndex;
}


void guiMainWindow::TurnOnAllDangerousButtons()
{ if (this->theComputationSetup.UsingCustomVectors)
	{ this->Spin1Dim->Enable();
		this->Spin2NumVect->Enable();
	}
	else
	{ this->ListBox1WeylGroup->Enable();
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
	{ this->Table1Input->SetColumnWidth(j,25);
		this->Table2Indicator->SetColumnWidth(j,25);
		this->Table3Values->SetColumnWidth(j,25);
	}
	this->Table2Indicator->SetRowHeight(0,25);
	this->Table3Values->SetRowHeight(0,25);
	for (int i=0;i<r;i++)
	{ this->Table1Input->SetRowHeight(i,25);
  }
	if (this->Table1Input->GetNumberRows()>20)
	{	this->Table1Input->SetSize(0,70,220,500);
		this->Table1Input->SetMaxSize(wxSize(220,500));
		this->SetAutoLayout(false);
	}
	else
	{	this->BoxSizer2VerticalInputs->Layout();
		this->BoxSizer8HorizontalEval->Layout();
		this->BoxSizer1HorizontalBackground->Layout();
	}
	if (r>this->MaxAllowedVectors)
	{ //this->Button1Go->Disable();
  } else
  { this->Button1Go->Enable();
  }
   #ifndef WIN32
//    this->Layout();
    this->BoxSizer8HorizontalEval->Fit(this->Table3Values);
    this->BoxSizer4VerticalToggleButton1->Fit(this->Table2Indicator);
    this->BoxSizer2VerticalInputs->Fit(this->Table1Input);
   #endif
}

void guiMainWindow::TurnOffAllDangerousButtons()
{ this->ListBox1WeylGroup->Disable();
	this->Spin1Dim->Disable();
	this->Spin2NumVect->Disable();
	this->ToggleButton1UsingCustom->Disable();
}

wxGridExtra::wxGridExtra(	wxWindow *parent, wxWindowID id,
													const wxPoint &pos,
													const wxSize &size,
													long style, const wxString &name)
{ this->maxNumCols=8;
	this->maxNumRows=120;
	this->Create(parent,id,pos,size,style,name);
}

void wxGridExtra::SetNumRowsAndCols(int r, int c)
{ if (r<0 || r>this->maxNumRows|| c<0 ||c>this->maxNumCols)
		return;
	int oldNumRows=	this->GetNumberRows();
	int oldNumCols= this->GetNumberCols();
	if (oldNumRows>r)
	{ this->DeleteRows(r,oldNumRows-r,true);
		oldNumRows=r;
	}
	if(oldNumRows<r)
	{ this->InsertRows(oldNumRows-1,r-oldNumRows,true);
		oldNumRows=r;
	}
	if (oldNumCols>c)
	{ this->DeleteCols(c,oldNumCols-c,true);
		oldNumCols=c;
	}
	if(oldNumCols<c)
	{ this->InsertCols(oldNumCols-1,c-oldNumCols,true);
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

void guiMainWindow::onComputationOver(wxCommandEvent& ev)
{ wxString tempWS(MainWindow1->theComputationSetup.theOutput.DebugString.c_str(), wxConvUTF8);
	MainWindow1->Text1Output->SetValue(tempWS);
	MainWindow1->TurnOnAllDangerousButtons();
	MainWindow1->Button1Go->SetLabel(wxT("Go"));
	MainWindow1->Refresh();
}

void guiMainWindow::onProgressReport(::wxCommandEvent& ev)
{	IndicatorWindowVariables& output= MainWindow1->progressReportVariables;
	wxString tempS1(output.ProgressReportString1.c_str(),wxConvUTF8);
	MainWindow1->Label1ProgressReport->SetLabel(tempS1);
	wxString tempS2(output.ProgressReportString2.c_str(),wxConvUTF8);
	MainWindow1->Label2ProgressReport->SetLabel(tempS2);
	wxString tempS3(output.ProgressReportString3.c_str(),wxConvUTF8);
	MainWindow1->Label3ProgressReport->SetLabel(tempS3);
	wxString tempS4(output.ProgressReportString4.c_str(),wxConvUTF8);
	MainWindow1->Label4ProgressReport->SetLabel(tempS4);
	wxString tempS5(output.ProgressReportString5.c_str(),wxConvUTF8);
	MainWindow1->Label5ProgressReport->SetLabel(tempS5);
	if (output.PerturbationHasOccurred)
	{ root tempRoot; tempRoot.AssignIntRoot(output.modifiedRoot);
		MainWindow1->WriteIndicatorWeight(tempRoot);
	}
}

void FeedDataToIndicatorWindow(IndicatorWindowVariables& output)
{	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=true;
	if (MainWindow1->WorkThread1.CriticalSectionPauseButtonEntered)
	{	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
		return;
	}
	MainWindow1->progressReportVariables.Assign(output);
	::wxPostEvent(MainWindow1->GetEventHandler(),MainWindow1->wxProgressReportEvent);
	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
}
//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
void drawline(double X1, double Y1, double X2, double Y2,
								unsigned long thePenStyle, int ColorIndex)
{ ::wxWindowDC dc(MainWindow1->Canvas1);
	wxPen tempPen;
	switch (thePenStyle)
	{ case 0: tempPen.SetStyle(::wxSOLID); break;
		case 1: tempPen.SetStyle(::wxSHORT_DASH); break;
		case 2: tempPen.SetStyle(::wxDOT);break;
		case 5: return;
	}
	tempPen.SetColour(ColorIndex);
	dc.SetPen(tempPen);
	dc.DrawLine((int)X1, (int)Y1,(int) X2,(int) Y2);
//	dc.setForeground(FXRGB(0,0,0));
//  dc.fillRectangle(0,0,MainWindow1->Canvas1DrawCanvas->getWidth(),MainWindow1->Canvas1DrawCanvas->getHeight());
}
void drawtext(double X1, double Y1, const char* text, int length, int color)
{	::wxWindowDC dc(MainWindow1->Canvas1);
	dc.SetFont(*MainWindow1->theFont);
	//dc.setcolo(color);
	//dc.setBackground(MainWindow1->Canvas1DrawCanvas->getBackColor());
	//dc(FILL_STIPPLED);
	wxString temptext(text,wxConvUTF8 ,length);
	dc.DrawText(temptext,(int)X1, (int)Y1);
}

#endif

