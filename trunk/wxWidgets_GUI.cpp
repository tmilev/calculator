
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
  wxBoxSizer* BoxSizer1HorizontalBackground;
  wxBoxSizer* BoxSizer2VerticalInputs;
  wxBoxSizer* BoxSizer3HorizontalInputButtons;
  wxBoxSizer* BoxSizer4VerticalToggleButton1;
  ::wxToggleButton* ToggleButton1UsingCustom;
public:
  void onToggleButton1UsingCustom( wxCommandEvent& ev);
  void onQuit( wxCommandEvent& ev);

  guiMainWindow();
  ~guiMainWindow();
  void OnQuit( wxCommandEvent& );
  enum
  { ID_ToggleButton1UsingCustom = 100,

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
END_EVENT_TABLE()

guiMainWindow::guiMainWindow()
        : wxFrame( (wxFrame *)NULL, wxID_ANY, _T("I will eat your RAM"),
                   wxPoint(100,100),
                   wxSize(600,600))
{	this->BoxSizer1HorizontalBackground = new ::wxBoxSizer(wxHORIZONTAL);
	this->BoxSizer2VerticalInputs = new ::wxBoxSizer(::wxVERTICAL);
	this->BoxSizer4VerticalToggleButton1= new ::wxBoxSizer(wxVERTICAL);
	this->ToggleButton1UsingCustom= new ::wxToggleButton(this,guiMainWindow::ID_ToggleButton1UsingCustom,wxT("Switch to custom"));
  this->Table1Input = new wxGrid( this,::wxID_ANY);
	this->BoxSizer1HorizontalBackground->Add(this->BoxSizer2VerticalInputs);
	this->BoxSizer2VerticalInputs->Add(this->BoxSizer4VerticalToggleButton1);
	this->BoxSizer4VerticalToggleButton1->Add(this->ToggleButton1UsingCustom);
	this->BoxSizer2VerticalInputs->Add(this->Table1Input);
	SetSizer(this->BoxSizer1HorizontalBackground);

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

void guiMainWindow::OnQuit( wxCommandEvent& WXUNUSED(ev) )
{ this->Close( true );
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
