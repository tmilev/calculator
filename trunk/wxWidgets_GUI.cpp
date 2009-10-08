
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

#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
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
public:
  void ToggleRowLabels( wxCommandEvent& );
  void onQuit( wxCommandEvent& );
  guiMainWindow();
  ~guiMainWindow();
  void OnQuit( wxCommandEvent& );
  enum
  { ID_TOGGLEROWLABELS = 100,
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
    EVT_MENU( ID_TOGGLEROWLABELS,  guiMainWindow::ToggleRowLabels )
END_EVENT_TABLE()

guiMainWindow::guiMainWindow()
        : wxFrame( (wxFrame *)NULL, wxID_ANY, _T("wxWidgets grid class demo"),
                   wxDefaultPosition,
                   wxDefaultSize )
{ this->BoxSizer1HorizontalBackground = new wxBoxSizer(wxHORIZONTAL);
   this->Table1Input = new wxGrid( this, wxID_ANY,wxPoint( 0, 0 ), wxSize( 400, 300 ) );
  this->Table1Input->CreateGrid( 0, 0 );
  this->Table1Input->AppendRows(10);
  this->Table1Input->AppendCols(10);
  Centre();
}


guiMainWindow::~guiMainWindow()
{
}

void guiMainWindow::ToggleRowLabels( wxCommandEvent& WXUNUSED(ev) )
{ if ( GetMenuBar()->IsChecked( ID_TOGGLEROWLABELS ) )
  { this->Table1Input->SetRowLabelSize( this->Table1Input->GetDefaultRowLabelSize() );
  }
  else
  { this->Table1Input->SetRowLabelSize( 0 );
  }
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
