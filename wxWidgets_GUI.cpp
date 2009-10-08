
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
#include "polyhedra.h"
#include "wx/textctrl.h"


class guiApp : public wxApp
{
public:
  bool OnInit();
};

class WorkThreadClass
{
public:
	#ifdef WIN32
		HANDLE ComputationalThread;
	#endif
	bool isRunning;
	bool CriticalSectionWorkThreadEntered;
	bool CriticalSectionPauseButtonEntered;
	void run();
};

class guiMainWindow : public wxFrame
{
public:
  ComputationSetup theComputationSetup;
  wxGrid *Table1Input;
  wxGrid *Table2Indicator;
  wxGrid *Table3Values;
  wxBoxSizer* BoxSizer1HorizontalBackground;
  wxBoxSizer* BoxSizer2VerticalInputs;
  wxBoxSizer* BoxSizer3HorizontalInputButtons;
  wxBoxSizer* BoxSizer4VerticalToggleButton1;
  wxBoxSizer* BoxSizer5VerticalCanvasAndProgressReport;
  wxBoxSizer* BoxSizer6HorizontalInputsBlock;
  wxBoxSizer* BoxSizer7VerticalListBox1;
  wxBoxSizer* BoxSizer8HorizontalEval;
  wxBoxSizer* BoxSizer9HorizontalCombo1AndMainButton;
  wxTextCtrl* Text1Output;
	::wxStaticText* Label1ProgressReport;
	::wxStaticText* Label2ProgressReport;
	::wxStaticText* Label3ProgressReport;
	::wxStaticText* Label4ProgressReport;
	::wxStaticText* Label5ProgressReport;
	::wxStaticText* Label6ProgressReport;
	::wxStaticText* Label7Dim;
	::wxStaticText* Label8NumVect;
  ::wxComboBox* ListBox1WeylGroup;
  ::wxButton* Button1Go;
  ::wxButton* Button2Eval;
  ::wxSpinCtrl* Spin1Dim;
  ::wxSpinCtrl* Spin2NumVect;
  ::wxPanel* Canvas1;
  ::wxToggleButton* ToggleButton1UsingCustom;
  void onToggleButton1UsingCustom( wxCommandEvent& ev);
  void onListBox1Change( wxCommandEvent& ev);
  void onButton2Eval( wxCommandEvent& ev);
  void onButton1Go( wxCommandEvent& ev);
	void onSpinner1and2(wxSpinEvent& ev);
	void onPaint(wxPaintEvent& ev);
	void ReadVPVectors();
	void TurnOnAllDangerousButtons();
	void updateListBox1();
  guiMainWindow();
  ~guiMainWindow();
  void OnExit(wxCloseEvent& event);
  static const int DefaultButtonHeight=30;
  static const int DefaultButtonWidth=100;
  enum
  { ID_ToggleButton1UsingCustom = 100,
		ID_ListBox1,
		ID_Button1Go,
		ID_Buton2Eval,
		ID_Spin1Dim,
		ID_Spin2NumVect,
		ID_Canvas1,
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
	EVT_BUTTON(guiMainWindow::ID_ToggleButton1UsingCustom, guiMainWindow::onToggleButton1UsingCustom)
	EVT_BUTTON(guiMainWindow::ID_Buton2Eval, guiMainWindow::onButton2Eval)
	EVT_BUTTON(guiMainWindow::ID_Button1Go, guiMainWindow::onButton1Go)
	EVT_LISTBOX(guiMainWindow::ID_ListBox1, guiMainWindow::onListBox1Change)
	//EVT_SPIN(guiMainWindow::ID_Spin1Dim, guiMainWindow::onSpinner1and2)
	//EVT_SPIN(guiMainWindow::ID_Spin2NumVect, guiMainWindow::onSpinner1and2)
	EVT_CLOSE(guiMainWindow::OnExit)
	//EVT_PAINT(guiMainWindow::onPaint)

END_EVENT_TABLE()


void RunComputationalThread()
{ MainWindow1->ReadVPVectors();
	MainWindow1->theComputationSetup.Run();
	wxString tempWS(MainWindow1->theComputationSetup.theOutput.DebugString.c_str(), wxConvUTF8);
	MainWindow1->Text1Output->WriteText(tempWS);
	MainWindow1->TurnOnAllDangerousButtons();
	tempWS= wxT("G");
	MainWindow1->Button1Go->SetTitle(tempWS);
}
/*
long MainWindow::onMouseDownOnCanvas(FXObject*, FXSelector, void* ptr)
{ FXEvent *theEvent=(FXEvent*)ptr;
	int Realx=theEvent->click_x; int Realy=theEvent->click_y;
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
		this->onRepaint(0,0,0);
	}
  return 1;
}


*/
guiMainWindow::guiMainWindow()
        : wxFrame( (wxFrame *)NULL, wxID_ANY, wxT("I will eat your RAM"),
                   wxPoint(100,100),
                   wxSize(600,600))
{	this->BoxSizer1HorizontalBackground = new ::wxBoxSizer(wxHORIZONTAL);
	this->BoxSizer2VerticalInputs = new ::wxBoxSizer(::wxVERTICAL);
	this->BoxSizer3HorizontalInputButtons = new ::wxBoxSizer(wxHORIZONTAL);
	this->BoxSizer4VerticalToggleButton1 = new ::wxBoxSizer(wxVERTICAL);
	this->BoxSizer5VerticalCanvasAndProgressReport = new ::wxBoxSizer(wxVERTICAL);
	this->BoxSizer6HorizontalInputsBlock = new ::wxBoxSizer(wxHORIZONTAL);
	this->BoxSizer7VerticalListBox1 = new ::wxBoxSizer(wxVERTICAL);
	this->BoxSizer8HorizontalEval =new ::wxBoxSizer(wxHORIZONTAL);
	this->BoxSizer9HorizontalCombo1AndMainButton= new ::wxBoxSizer(wxHORIZONTAL);
	this->ToggleButton1UsingCustom= new ::wxToggleButton(this,guiMainWindow::ID_ToggleButton1UsingCustom,wxT("Switch to custom"));
  this->Table1Input = new ::wxGrid( this,0,0,0,0,wxEXPAND| wxALL);
  this->Table2Indicator = new wxGrid( this,::wxID_ANY);
  this->Table3Values = new wxGrid( this,::wxID_ANY);
	this->Label1ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label2ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label3ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label4ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Spin1Dim = new wxSpinCtrl(this,this->ID_Spin1Dim);
	this->Spin2NumVect= new wxSpinCtrl(this, this->ID_Spin1Dim);
	//this->Spin2NumVect->SetSize(this->DefaultButtonWidth,this->DefaultButtonHeight);
	//this->Spin1Dim->SetSize(this->DefaultButtonWidth,this->DefaultButtonHeight);
	this->Canvas1 = new ::wxPanel(this,0,0,0,0,::wxEXPAND|wxALL);
	//this->Label5ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->ListBox1WeylGroup= new ::wxComboBox(this,this->ID_ListBox1,wxT("A2"),
                                            wxPoint(0,0),wxSize(this->DefaultButtonWidth, this->DefaultButtonHeight),0,0,wxCB_DROPDOWN);
	this->Button2Eval= new ::wxButton(this,this->ID_Buton2Eval, wxT("Evaluate"));
	this->Button2Eval->SetSize(this->DefaultButtonWidth, this->DefaultButtonHeight);
	this->Button1Go= new ::wxButton(this,this->ID_Button1Go,wxT("Go"));
	this->Button1Go->SetSize(this->DefaultButtonWidth, this->DefaultButtonHeight);
	this->BoxSizer1HorizontalBackground->Fit(this);
	this->BoxSizer1HorizontalBackground->Add(this->BoxSizer2VerticalInputs,1);
		this->BoxSizer2VerticalInputs->Add(this->BoxSizer6HorizontalInputsBlock, wxEXPAND| wxALL);
			this->BoxSizer6HorizontalInputsBlock->Add(this->BoxSizer4VerticalToggleButton1, wxEXPAND| wxALL);
				this->BoxSizer4VerticalToggleButton1->Add(this->ToggleButton1UsingCustom,0,0,0);
				this->BoxSizer4VerticalToggleButton1->Add(this->Spin1Dim,0,0,0);
			this->BoxSizer6HorizontalInputsBlock->Add(this->BoxSizer7VerticalListBox1, wxEXPAND| wxALL);
        this->BoxSizer7VerticalListBox1->Add(this->BoxSizer9HorizontalCombo1AndMainButton, wxEXPAND| wxALL);
          this->BoxSizer9HorizontalCombo1AndMainButton->Add(this->ListBox1WeylGroup,0,0,0);
          this->BoxSizer9HorizontalCombo1AndMainButton->Add(this->Button1Go, 0,0,0);
			this->BoxSizer7VerticalListBox1->Add(this->Spin2NumVect);
			//this->BoxSizer7VerticalListBox1->Add(this->Spin2NumVect);
		this->BoxSizer2VerticalInputs->Add(this->Table2Indicator);
		this->BoxSizer2VerticalInputs->Add(this->Table1Input);
		this->BoxSizer2VerticalInputs->Add(this->BoxSizer8HorizontalEval);
			this->BoxSizer8HorizontalEval->Add(this->Table3Values);
			this->BoxSizer8HorizontalEval->Add(this->Button2Eval);
	this->BoxSizer1HorizontalBackground->Add(this->BoxSizer5VerticalCanvasAndProgressReport,1,wxEXPAND|wxALL);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Label1ProgressReport);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Label2ProgressReport);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Label3ProgressReport);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Label4ProgressReport);
		this->BoxSizer5VerticalCanvasAndProgressReport->Add(this->Canvas1,1,wxEXPAND|wxALL);
  this->ListBox1WeylGroup->Append(wxT("A2"));
  this->ListBox1WeylGroup->Append(wxT("A3"));
  this->ListBox1WeylGroup->Append(wxT("A4"));
  this->ListBox1WeylGroup->Append(wxT("A5"));

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
  this->ListBox1WeylGroup->SetSelection(0,0);
  this->Table1Input->CreateGrid( 0, 0 );
  this->Table2Indicator->CreateGrid( 0, 0 );
  this->Table3Values->CreateGrid(0,0);
  SetSizer(this->BoxSizer1HorizontalBackground);
  	this->maxDim=5;
	this->maxNumVect=15;
	this->theComputationSetup.ComputationInProgress=false;
	this->theComputationSetup.AllowRepaint=true;
	//this->Canvas1DrawCanvas->::FXCanvas::setBackColor(this->getBackColor());
	this->ClickToleranceX=10;
	this->ClickToleranceY=10;
	this->WorkThread1.CriticalSectionPauseButtonEntered=false;
	this->WorkThread1.CriticalSectionWorkThreadEntered=false;
	TDV.centerX=150;
	TDV.centerY=200;
	this->Canvas1->Fit();
	Centre();
}


guiMainWindow::~guiMainWindow()
{
}

void guiMainWindow::onToggleButton1UsingCustom( wxCommandEvent& ev)
{ FXbool tempB= this->ToggleButton1Custom->getState();
	if (tempB==0)
	{	this->theComputationSetup.UsingCustomVectors=false;
	}else
	{ this->theComputationSetup.UsingCustomVectors=true;
	}
	this->updateListBox1();
	return 1;
}

void guiMainWindow::onPaint(wxPaintEvent &ev)
{	/*if (this->theComputationSetup.AllowRepaint)
	{	root::AmbientDimension= this->theComputationSetup.RankEuclideanSpaceGraphics;
		root tempRoot;
		tempRoot.MakeZero();
		::FXDCWindow dc(this->Canvas1DrawCanvas);
		dc.setForeground(this->Canvas1DrawCanvas->getBackColor());
		dc.fillRectangle(0,0,this->Canvas1DrawCanvas->getWidth(),this->Canvas1DrawCanvas->getHeight());
		if (this->ListBox1WeylGroup->getCurrentItem()==2)
		{ std::string tempS;
			tempS.clear();
		}
		::drawOutput(::TDV,::TheBigOutput,::InputRoots,::NextDirectionIndex,tempRoot);
	}
	return 1;*/
}

void guiMainWindow::onButton1Go(wxCommandEvent &ev)
{	this->TurnOffAllDangerousButtons();
#ifdef WIN32
	if (!this->theComputationSetup.ComputationInProgress)
	{ this->theComputationSetup.ComputationInProgress=true;
		this->Button1MainButton->setText("Pause");
		this->WorkThread1.ComputationalThread=CreateThread(0,0, (LPTHREAD_START_ROUTINE)RunComputationalThread,0,0,0);
	}
	else
	{ if(this->WorkThread1.isRunning)
		{ this->WorkThread1.CriticalSectionPauseButtonEntered=true;
			while(this->WorkThread1.CriticalSectionWorkThreadEntered)
			{}
			ParallelComputing::ReachSafePointASAP=true;
			while(!ParallelComputing::SafePointReached)
			{}
			ParallelComputing::ReachSafePointASAP=false;
			::SuspendThread(this->WorkThread1.ComputationalThread);
			this->WorkThread1.isRunning=false;
			this->Button1MainButton->setText("Go");
			this->WorkThread1.CriticalSectionWorkThreadEntered=false;
			this->WorkThread1.CriticalSectionPauseButtonEntered=false;
		}
		else
		{	this->Button1MainButton->setText("Pause");
			this->WorkThread1.isRunning=true;
			::ResumeThread(this->WorkThread1.ComputationalThread);
		}
	}
	return 1;
#else
	this->WorkThread1.run();
	return 1;
#endif
}

void guiMainWindow::onButton2Eval(wxCommandEvent &ev)
{	if (this->theComputationSetup.theOutput.NumVars!=this->Table3Values->getNumColumns())
	{ return 1;
	}
	intRoot tempRoot;
	for (int i=0;i<this->theComputationSetup.theOutput.NumVars;i++)
	{ this->theComputationSetup.ValueRoot.elements[i]= FXIntVal(this->Table3Values->getItemText(0,i));
	}
	this->theComputationSetup.EvaluatePoly();
	this->Text2Value->setText(this->theComputationSetup.ValueString.c_str(), this->theComputationSetup.ValueString.length());
	return 1;
}

void guiMainWindow::onSpinner1and2(wxSpinEvent& ev)
{ int candidateDim= this->Spinner1Dimension->getValue();
	int candidateNumVectors= this->Spinner2NumVectors->getValue();
	if (candidateDim!= this->theComputationSetup.WeylGroupIndex ||
			candidateNumVectors!=this->NumVectors)
	{ if (candidateDim<1)
		{ candidateDim=1;
			this->Spinner1Dimension->setValue(1);
		}
		if (candidateNumVectors<1)
		{ candidateNumVectors=1;
			this->Spinner2NumVectors->setValue(1);
		}
		if (candidateDim>this->maxDim)
		{ candidateDim=this->maxDim;
			this->Spinner1Dimension->setValue(this->maxDim);
		}
		if (candidateNumVectors>this->maxNumVect)
		{ candidateNumVectors=this->maxNumVect;
			this->Spinner2NumVectors->setValue(this->maxNumVect);
		}
		this->initTableFromRowsAndColumns(candidateNumVectors,candidateDim);
	}
	return 1;
}

void guiMainWindow::updateListBox1()
{ if (this->theComputationSetup.UsingCustomVectors)
	{ this->ListBox1WeylGroup->disable();
		this->Spinner1Dimension->enable();
		this->Spinner2NumVectors->enable();
		this->Spinner1Dimension->setValue(this->theComputationSetup.WeylGroupIndex);
		this->Spinner2NumVectors->setValue(this->theComputationSetup.VPVectors.size);
		this->onListBox1WeylGroupCommand(this->ToggleButton1Custom,0,0);
	}
	else
	{ this->Spinner1Dimension->disable();
		this->Spinner2NumVectors->disable();
		this->ListBox1WeylGroup->enable();
		this->onListBox1WeylGroupCommand(0,0,0);
		this->initWeylGroupInfo();
	}
}

void guiMainWindow::onListBox1Change(wxCommandEvent &ev)
{	std::string tempS;
	unsigned char newWeylGroupIndex=0;
	char newWeylGroupLetter=0;
	switch(this->ListBox1WeylGroup->getCurrentItem())
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
	//	case 12: this->UsingCustomVectors=true;
	}
	if (!(newWeylGroupLetter==this->theComputationSetup.WeylGroupLetter &&
				newWeylGroupIndex==this->theComputationSetup.WeylGroupIndex))
	{ this->theComputationSetup.WeylGroupLetter=newWeylGroupLetter;
		this->theComputationSetup.WeylGroupIndex=newWeylGroupIndex;
		this->initWeylGroupInfo();
	}
	return 1;
}


void guiMainWindow::initWeylGroupInfo()
{ if (!this->theComputationSetup.UsingCustomVectors)
	{	WeylGroup tempW;
		tempW.MakeArbitrary(this->theComputationSetup.WeylGroupLetter,
												this->theComputationSetup.WeylGroupIndex);
		tempW.ComputeRho();
		this->Table1Input->setTableSize(tempW.RootsOfBorel.size,
																		this->theComputationSetup.WeylGroupIndex);
		this->Table2Indicator->setTableSize(1,this->theComputationSetup.WeylGroupIndex);
		this->Table3Values->setTableSize(1,this->theComputationSetup.WeylGroupIndex);
		for (int i=0;i<tempW.RootsOfBorel.size;i++)
		{ for (int j=0;j<root::AmbientDimension;j++)
			{ std::string tempS;
				tempW.RootsOfBorel.TheObjects[i].coordinates[j].ElementToString(tempS);
				this->Table1Input->setItemText(i,j,tempS.c_str());
				this->Table1Input->setItemJustify(i,j,JUSTIFY_CENTER_X);
			}
		}
//		this->Table1Input->getColumnHeader()->setWidth(20);
		this->Table1Input->setVisibleColumns(root::AmbientDimension);
		this->Table1Input->setVisibleRows(tempW.RootsOfBorel.size);
		this->Table2Indicator->setVisibleColumns(root::AmbientDimension);
		this->Table3Values->setVisibleColumns(root::AmbientDimension);
		this->Table2Indicator->setVisibleRows(1);
		this->Table3Values->setVisibleRows(1);
//		this->Table1Input->fitColumnsToContents(0,root::AmbientDimension);
		root tempRoot; tempRoot.Assign(tempW.rho); tempRoot.MultiplyByInteger(2);
		for (int i=0;i<root::AmbientDimension;i++)
		{ this->Table1Input->setColumnWidth(i,20);
			std::string tempS;
			tempRoot.coordinates[i].ElementToString(tempS);
			this->Table2Indicator->setItemText(0,i,tempS.c_str());
			this->Table2Indicator->setItemJustify(0,i,JUSTIFY_CENTER_X);
			this->Table2Indicator->setColumnWidth(i,20);
			this->Table3Values->setColumnWidth(i,20);
		}
		this->Table1Input->resize(root::AmbientDimension*20+5,tempW.RootsOfBorel.size*20+3);
		this->Table2Indicator->resize(root::AmbientDimension*20+5,23);
		this->Table3Values->resize(root::AmbientDimension*20+5,23);
		this->theComputationSetup.VPVectors.CopyFromBase(tempW.RootsOfBorel);
		this->NumVectors=this->theComputationSetup.VPVectors.size;
	}
	//this->Table1Input->recalc();
}

void guiMainWindow::OnExit(wxCloseEvent &event)
{ this->Destroy();
}


void guiMainWindow::ReadVPVectors()
{ if (this->theComputationSetup.UsingCustomVectors)
	{	root::AmbientDimension= this->Spin1Dim->GetValue();
		this->theComputationSetup.VPVectors.size=0;
		for (int i=0;i<this->Spin2NumVect->GetValue();i++)
		{ root tempRoot;
			for (int j=0;j<root::AmbientDimension;j++)
			{	int tempI=wxAtoi(this->Table1Input->GetCellValue(i,j));
				tempRoot.coordinates[j].AssignInteger(tempI);
			}
			this->theComputationSetup.VPVectors.AddRoot(tempRoot);
		}
		this->theComputationSetup.VPVectors.ComputeDebugString();
	}
	for (int j=0;j<this->theComputationSetup.WeylGroupIndex;j++)
	{	int tempI=wxAtoi(this->Table2Indicator->GetCellValue(0,j));
		this->theComputationSetup.IndicatorRoot.coordinates[j].AssignInteger(tempI);
	}
	this->theComputationSetup.IndicatorRoot.dimension=this->theComputationSetup.WeylGroupIndex;
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
	this->Table1Input->setTableSize(this->NumVectors,this->theComputationSetup.WeylGroupIndex);
	for (int j=0;j<c;j++)
	{ this->Table1Input->setColumnWidth(j,20);
	}
	this->Table1Input->setVisibleColumns(Minimum(c,10));
	this->Table1Input->setVisibleRows(Minimum(r,20));
}

void guiMainWindow::TurnOffAllDangerousButtons()
{ this->ListBox1WeylGroup->disable();
	this->Spinner1Dimension->disable();
	this->Spinner2NumVectors->disable();
	this->ToggleButton1Custom->disable();
}

void WorkThreadClass::run()
{	::RunComputationalThread();
}



void outputText(std::string theOutput)
{
}

void FeedDataToIndicatorWindow(IndicatorWindowVariables& output)
{/*	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=true;
	if (MainWindow1->WorkThread1.CriticalSectionPauseButtonEntered)
	{	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
		return;
	}
	MainWindow1->Label1ProgressReport->setText(output.ProgressReportString1.c_str());
	MainWindow1->Label2ProgressReport->setText(output.ProgressReportString2.c_str());
	MainWindow1->Label3ProgressReport->setText(output.ProgressReportString3.c_str());
	MainWindow1->Label4ProgressReport->setText(output.ProgressReportString4.c_str());
	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;*/
}
//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
void drawline(double X1, double Y1, double X2, double Y2,
								unsigned long thePenStyle, int ColorIndex)
{ /*::FXDCWindow dc(MainWindow1->Canvas1DrawCanvas);
	switch (thePenStyle)
	{ case 0: dc.::FXDCWindow::setLineStyle(::LINE_SOLID); break;
		case 1: dc.::FXDCWindow::setLineStyle(::LINE_ONOFF_DASH); break;
		case 2: dc.::FXDCWindow::setLineStyle(::LINE_DOUBLE_DASH); break;
		case 5: return;
	}
	dc.::FXDCWindow::setForeground(ColorIndex);
	dc.::FXDCWindow::drawLine((int)X1, (int)Y1,(int) X2,(int) Y2);*/
//	dc.setForeground(FXRGB(0,0,0));
//  dc.fillRectangle(0,0,MainWindow1->Canvas1DrawCanvas->getWidth(),MainWindow1->Canvas1DrawCanvas->getHeight());
}
void drawtext(double X1, double Y1, const char* text, int length, int color)
{	//::FXDCWindow dc(MainWindow1->Canvas1DrawCanvas);
	//dc.setFont(MainWindow1->FontDefaultDrawFont);
	//dc.setForeground(color);
	//dc.setBackground(MainWindow1->Canvas1DrawCanvas->getBackColor());
	//dc.setFillStyle(FILL_STIPPLED);
	//dc.drawImageText((int)X1, (int)Y1,text,length);
}
