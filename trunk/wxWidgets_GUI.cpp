
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
#include "polyhedra.h"



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
{ wxGrid *Table1Input;
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
  
	::wxStaticText* Label1ProgressReport;
	::wxStaticText* Label2ProgressReport;
	::wxStaticText* Label3ProgressReport;
	::wxStaticText* Label4ProgressReport;
	::wxStaticText* Label5ProgressReport;
	::wxStaticText* Label6ProgressReport;	
	::wxStaticText* Label7Dim;
	::wxStaticText* Label8NumVect;
  ::wxComboBox* ComboBox1WeylGroup;
  ::wxButton* Button1Go;
  ::wxButton* Button2Eval;
  ::wxSpinCtrl* Spin1Dim;
  ::wxSpinCtrl* Spin2NumVect;
  ::wxPanel* Canvas1;
  
  
  
  ::wxToggleButton* ToggleButton1UsingCustom;
public:
  void onToggleButton1UsingCustom( wxCommandEvent& ev);
  void onListBox1Change( wxCommandEvent& ev);
  void onButton2Eval( wxCommandEvent& ev);
  void onButton1Go( wxCommandEvent& ev);
	void onSpinner1and2(wxSpinEvent& ev);
	void onPaint(wxPaintEvent& ev);
  guiMainWindow();
  ~guiMainWindow();
  void OnExit(wxCloseEvent& event);
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

bool guiApp::OnInit()
{ guiMainWindow *frame = new guiMainWindow;
  frame->Show(true);
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
	this->ToggleButton1UsingCustom= new ::wxToggleButton(this,guiMainWindow::ID_ToggleButton1UsingCustom,wxT("Switch to custom"));
  this->Table1Input = new wxGrid( this,::wxID_ANY);
  this->Table2Indicator = new wxGrid( this,::wxID_ANY);
  this->Table3Values = new wxGrid( this,::wxID_ANY);
	this->Label1ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label2ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label3ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Label4ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->Canvas1 = new ::wxPanel(this,0,0,0,0,::wxEXPAND|wxALL);
	//this->Label5ProgressReport = new ::wxStaticText(this,wxID_ANY,wxT(""));
	this->ComboBox1WeylGroup= new ::wxComboBox(this,this->ID_ListBox1);
	this->Button2Eval= new ::wxButton(this,this->ID_Buton2Eval, wxT("Evaluate"));
	this->BoxSizer1HorizontalBackground->Add(this->BoxSizer2VerticalInputs);	
		this->BoxSizer2VerticalInputs->Add(this->BoxSizer6HorizontalInputsBlock);
			this->BoxSizer6HorizontalInputsBlock->Add(this->BoxSizer4VerticalToggleButton1);
				this->BoxSizer4VerticalToggleButton1->Add(this->ToggleButton1UsingCustom);
				//this->BoxSizer4VerticalToggleButton1->Add(this->Spin1Dim->getSizer());
			this->BoxSizer6HorizontalInputsBlock->Add(this->BoxSizer7VerticalListBox1);
			this->BoxSizer7VerticalListBox1->Add(this->ComboBox1WeylGroup);
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
	SetSizer(this->BoxSizer1HorizontalBackground);
	this->Canvas1->Fit();

  this->Table1Input->CreateGrid( 0, 0 );
  this->Table1Input->AppendRows(5);
  this->Table1Input->AppendCols(5);
  Centre();
}


guiMainWindow::~guiMainWindow()
{
}

void guiMainWindow::onToggleButton1UsingCustom( wxCommandEvent& ev)
{
}

void guiMainWindow::onPaint(wxPaintEvent &ev)
{
}

void guiMainWindow::onButton1Go(wxCommandEvent &ev)
{
} 

void guiMainWindow::onButton2Eval(wxCommandEvent &ev)
{
}

void guiMainWindow::onSpinner1and2(wxSpinEvent& ev)
{
}

void guiMainWindow::onListBox1Change(wxCommandEvent &ev)
{
}

void guiMainWindow::OnExit(wxCloseEvent &event)
{ this->Destroy();
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
