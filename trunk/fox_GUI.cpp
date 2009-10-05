//*********************************************************************************************************
//*********************************************************************************************************
//Vector partition function - computes an algebraic expression 
//                            for the vector partition function
//CopyRight (C) 2009: Todor Milev 
//email: t.milev@jacobs-university.de
//
//Contributors: Thomas Bliem, Todor Milev
//
//This file is based on the cross-platform GUI toolkit FOX, which is also free software (LGPL license).
//Its author is Jeroen van der Zijp. You are kindly directed to http://www.fox-toolkit.org/ 
//or your favourite search engine for more detail on the FOX GUI project and latest downloads.
//
//This is free software. You are warmly welcome to use, modify and redistribute this code 
//and the resulting program any way you wish, as long as you provide the same rights 
//as those given to you to any future recipients of your modifications (in case you 
//decide you want to pass on those modifications). 
//The code is licensed under the Library General Public License version 3.0 
//(summarized in the preceding sentence). 
//You should have received a copy of the GNU Library General Public License 
//along with this program. 
//If not, see <http://www.gnu.org/licenses/>.
//
//Todor Milev would like to thank http://www.cplusplus.com/forum/ for the valuable
//advice and help with C++. Special thanks to helios, Disch, Grey Wolf, jsmith, 
//Hammurabi and Duoas for the helpful comments and advice on C++!
//*********************************************************************************************************
//*********************************************************************************************************
//DISCLAIMER
//
//THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. 
//EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES 
//PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, 
//EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
//THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. 
//SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, 
//REPAIR OR CORRECTION.
//*********************************************************************************************************
//*********************************************************************************************************


#include ".\fox-1.6.36\include\fx.h"
#include ".\fox-1.6.36\include\FXExpression.h"
#ifdef WIN32
#include <windows.h>
#include <Commdlg.h>
#endif
#include "polyhedra.h"

extern CombinatorialChamberPointers TheBigOutput;
extern FacetPointers TheBigFacetOutput;
extern int NextDirectionIndex;
extern roots InputRoots;
extern DrawingVariables TDV;

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


class MainWindow : public FXMainWindow 
{ FXDECLARE(MainWindow)
protected:
  MainWindow(){};
private:
public:
	::FXCanvas* Canvas1DrawCanvas;
	::FXTable* Table1Input;
	::FXTable* Table2Indicator;
	::FXTable* Table3Values;
	::FXHorizontalFrame* HorizontalFrame1;
	::FXHorizontalFrame* HorizontalFrame2;
	::FXHorizontalFrame* HorizontalFrame3Background;
	::FXHorizontalFrame* HorizontalFrame4SpinnerDim;
	::FXHorizontalFrame* HorizontalFrame5SpinnerNumVect;
	::FXHorizontalFrame* HorizontalFrame6ListBox1;
	::FXHorizontalFrame* HorizontalFrame7Table3Values;
	::FXVerticalFrame* VerticalFrame1ProgressReports;
	::FXVerticalFrame* VerticalFrame2InputData;
	::FXVerticalFrame* VerticalFrame3OutputQP;
	::FXVerticalFrame* VerticalFrame4ToggleButton1;
	::FXVerticalFrame* VerticalFrame5ListBox1;
	::FXVerticalFrame* VerticalFrame6Table2Input;
	::FXListBox* ListBox1WeylGroup;
	::FXToggleButton* ToggleButton1Custom;
	//::FXCanvas* 
	::FXLabel* Label1ProgressReport;
	::FXLabel* Label2ProgressReport;
	::FXLabel* Label3ProgressReport;
	::FXLabel* Label4ProgressReport;
	::FXLabel* Label5ProgressReport;
	::FXLabel* Label6ProgressReport;
	::FXButton* Button1MainButton;
	::FXSpinner* Spinner1Dimension;
	::FXSpinner* Spinner2NumVectors;
	::FXFont* FontDefaultDrawFont;
	::FXText* Text1OutputMain;
	ComputationSetup theComputationSetup;
	WorkThreadClass WorkThread1;
  long onRepaint(FXObject* sender,FXSelector sel,void*ptr);
  long onButton1MainButtonCommand(FXObject* sender,FXSelector sel,void*ptr);
  long onToggleButton1Custom(FXObject* sender,FXSelector sel,void*ptr);
  long onButton2EvaluateCommand(FXObject* sender,FXSelector sel,void*ptr);
  long onListBox1WeylGroupCommand(FXObject* sender,FXSelector sel,void*ptr);
  long onMouseDownOnCanvas(FXObject* sender,FXSelector sel,void*ptr);
  long onSpinner1and2Change(FXObject* sender,FXSelector sel,void*ptr);
  void initTableFromRowsAndColumns(int r, int c);
  void TurnOffAllDangerousButtons();
  void TurnOnAllDangerousButtons();
  void updateListBox1();
  void initWeylGroupInfo();
  void ReadVPVectors();
	int NumVectors;
	int ClickToleranceX;
	int ClickToleranceY; 
	int maxDim;
	int maxNumVect;
  enum
  {	ID_Button1MainButtonCommand=FXMainWindow::ID_LAST,
		ID_Table1InputCommand,
		ID_Table2IndicatorCommand,
		ID_Table3ValuesCommand,
		ID_ListBox1WeylGroupCommand,
		ID_Canvas1,
		ID_ToggleButton1Custom,	
		ID_Button2EvaluateCommand,	
		ID_Text1Edit,
		ID_Spinner1Dimension,
		ID_Spinner2NumVectors,
    ID_LAST
  };
  MainWindow(FXApp* a);
  virtual void create();
  virtual ~MainWindow();
};



// Message Map for the Scribble Window class
FXDEFMAP(MainWindow) ScribbleWindowMap[]=
{ //________Message_Type,  ID,  Message_Handler
  FXMAPFUNC(SEL_COMMAND, MainWindow::ID_Button1MainButtonCommand, 
						MainWindow::onButton1MainButtonCommand),
  FXMAPFUNC(SEL_COMMAND, MainWindow::ID_ListBox1WeylGroupCommand, 
						MainWindow::onListBox1WeylGroupCommand),
  FXMAPFUNC(SEL_COMMAND, MainWindow::ID_ToggleButton1Custom, 
						MainWindow::onToggleButton1Custom),
  FXMAPFUNC(SEL_COMMAND, MainWindow::ID_Spinner1Dimension, 
						MainWindow::onSpinner1and2Change),
  FXMAPFUNC(SEL_COMMAND, MainWindow::ID_Spinner2NumVectors, 
						MainWindow::onSpinner1and2Change),
  FXMAPFUNC(SEL_PAINT, MainWindow::ID_Canvas1, 
						MainWindow::onRepaint),
  FXMAPFUNC(SEL_COMMAND, MainWindow::ID_Button2EvaluateCommand, 
						MainWindow::onButton2EvaluateCommand),
	FXMAPFUNC(SEL_LEFTBUTTONPRESS,MainWindow::ID_Canvas1, 
						MainWindow::onMouseDownOnCanvas),
};

// Macro for the ScribbleApp class hierarchy implementation
FXIMPLEMENT(MainWindow,FXMainWindow,ScribbleWindowMap,ARRAYNUMBER(ScribbleWindowMap))

///////
///////
///////
///////
///////
///////


MainWindow* MainWindow1=0;


void RunComputationalThread()
{ MainWindow1->ReadVPVectors();
	MainWindow1->theComputationSetup.Run();
	MainWindow1->Text1OutputMain->setText(
		MainWindow1->theComputationSetup.theOutput.DebugString.c_str(),
		MainWindow1->theComputationSetup.theOutput.DebugString.length());
	MainWindow1->TurnOnAllDangerousButtons();
	MainWindow1->Button1MainButton->setText("Go");
	MainWindow1->onRepaint(0,0,0);
}

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

long MainWindow::onRepaint(FXObject*,FXSelector sel,void*)
{	if (this->theComputationSetup.AllowRepaint)
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
	return 1;
}

MainWindow::MainWindow(FXApp *a):FXMainWindow(a,"I will eat your RAM",NULL,NULL,DECOR_ALL,0,0,800,600)
{	this->HorizontalFrame3Background= new FXHorizontalFrame(this,::LAYOUT_FILL,0,0);
		this->VerticalFrame2InputData =new 
		::FXVerticalFrame(this->HorizontalFrame3Background,::LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
			this->HorizontalFrame1= new FXHorizontalFrame(this->VerticalFrame2InputData,LAYOUT_NORMAL,0,0,0,0,0,0,0,0);
			this->VerticalFrame4ToggleButton1= 
			new ::FXVerticalFrame(this->HorizontalFrame1,0,0,0,0,0,0,0,0,0);
			this->VerticalFrame5ListBox1= 
			new ::FXVerticalFrame(this->HorizontalFrame1,0,0,0,0,0,0,0,0,0);
			this->HorizontalFrame6ListBox1=
			new ::FXHorizontalFrame(this->VerticalFrame5ListBox1,0,0,0,0,0,0,0,0,0);
 			this->ToggleButton1Custom= 
			new ::FXToggleButton(	this->VerticalFrame4ToggleButton1,"Use custom vectors", "Use root system",0,0,
															this,MainWindow::ID_ToggleButton1Custom);
			this->ListBox1WeylGroup= 
			new FXListBox(	this->HorizontalFrame6ListBox1,this,MainWindow::ID_ListBox1WeylGroupCommand,
											LAYOUT_FIX_WIDTH,80,0,50,10);
			this->Button1MainButton= new FXButton(	this->HorizontalFrame6ListBox1,
											"&Go",NULL,this,MainWindow::ID_Button1MainButtonCommand,
											FRAME_THICK|FRAME_RAISED|LAYOUT_FIX_WIDTH | ::LAYOUT_RIGHT,
											0,0,50,10,1,1,1,1);
				this->HorizontalFrame4SpinnerDim=
				new ::FXHorizontalFrame(this->VerticalFrame4ToggleButton1,::LAYOUT_FILL,0,0,0,0,0,0,0,0);
					new ::FXLabel(this->HorizontalFrame4SpinnerDim,"Dimension:",0,::LAYOUT_LEFT);
					this->Spinner1Dimension= 
					new ::FXSpinner(this->HorizontalFrame4SpinnerDim,2,this,
													MainWindow::ID_Spinner1Dimension,
													::LAYOUT_RIGHT|::LAYOUT_FIX_WIDTH,0,0,30,0);
				this->HorizontalFrame5SpinnerNumVect=
				new ::FXHorizontalFrame(this->VerticalFrame5ListBox1,::LAYOUT_FILL,0,0,0,0,0,0,0,0);
					new ::FXLabel(this->HorizontalFrame5SpinnerNumVect,"Num Vectors:",0,::LAYOUT_LEFT);
					this->Spinner2NumVectors= 
					new ::FXSpinner(this->HorizontalFrame5SpinnerNumVect,3,
													this,MainWindow::ID_Spinner2NumVectors,
													::LAYOUT_FIX_WIDTH| ::LAYOUT_RIGHT,0,0,30,0);
			this->VerticalFrame6Table2Input=
			new ::FXVerticalFrame(this->VerticalFrame2InputData,0,0,0,0,0,0,0,0,0);
				new ::FXLabel(this->VerticalFrame6Table2Input,"Chamber Indicator");
				this->Table2Indicator= 
				new FXTable(this->VerticalFrame6Table2Input,this,
										MainWindow::ID_Table2IndicatorCommand, 
										::LAYOUT_FIX_HEIGHT|::LAYOUT_FIX_WIDTH);
			new ::FXSeparator(this->VerticalFrame2InputData);
			this->Table1Input= 
			new FXTable(this->VerticalFrame2InputData,this,MainWindow::ID_Table1InputCommand, 
									::LAYOUT_FIX_HEIGHT|::LAYOUT_FIX_WIDTH);
			new ::FXSeparator(this->VerticalFrame2InputData);
			this->HorizontalFrame7Table3Values= 
			new ::FXHorizontalFrame(this->VerticalFrame2InputData,0,0,0,0,0,0,0,0,0);
				this->Table3Values= 
				new FXTable(this->HorizontalFrame7Table3Values,this,MainWindow::ID_Table3ValuesCommand, 
										::LAYOUT_FIX_HEIGHT|::LAYOUT_FIX_WIDTH);
				new ::FXButton(	this->HorizontalFrame7Table3Values,"Evaluate",
												0,this,MainWindow::ID_Button2EvaluateCommand);
			this->Text1OutputMain= 
			new ::FXText(this->VerticalFrame2InputData,this,MainWindow::ID_Text1Edit,::LAYOUT_FILL);
		this->VerticalFrame1ProgressReports=				
		new ::FXVerticalFrame(this->HorizontalFrame3Background,::LAYOUT_FILL,0,0,0,0,0,0,0,0);
			this->Label1ProgressReport = 
			new ::FXLabel(this->VerticalFrame1ProgressReports,"");
			this->Label2ProgressReport = 
			new ::FXLabel(this->VerticalFrame1ProgressReports,"");
			this->Label3ProgressReport = 
			new ::FXLabel(this->VerticalFrame1ProgressReports,"");
			this->Label4ProgressReport = 
			new ::FXLabel(this->VerticalFrame1ProgressReports,"");
			this->Canvas1DrawCanvas =
			new ::FXCanvas(	this->VerticalFrame1ProgressReports,this,MainWindow::ID_Canvas1,::LAYOUT_FILL);	
	this->FontDefaultDrawFont=
	new FXFont(	a,"arial",10);
//		new FXFont(	a,"helvetica",20,FXFont::Normal,FXFont::Straight,
//								FONTENCODING_DEFAULT,FXFont::NonExpanded,
//								FXFont::Scalable|FXFont::Rotatable);
	this->ListBox1WeylGroup->appendItem("A2");
	this->ListBox1WeylGroup->appendItem("A3");
	this->ListBox1WeylGroup->appendItem("A4");
	this->ListBox1WeylGroup->appendItem("A5");
	this->ListBox1WeylGroup->appendItem("B2");
	this->ListBox1WeylGroup->appendItem("B3");
	this->ListBox1WeylGroup->appendItem("B4");
	this->ListBox1WeylGroup->appendItem("C2");
	this->ListBox1WeylGroup->appendItem("C3");
	this->ListBox1WeylGroup->appendItem("C4");
	this->ListBox1WeylGroup->appendItem("D4");
	this->ListBox1WeylGroup->appendItem("G2");
	//this->ListBox1WeylGroup->appendItem("Custom");
	this->ListBox1WeylGroup->setNumVisible(5);
	this->ListBox1WeylGroup->setCurrentItem(1);
	//this->Table1Input->setBorderColor(FXRGB(192,192,192));
	//this->HorizontalFrame1->setBackColor(FXRGB(192,222,22));
	this->Table1Input->setBackColor(this->getBackColor());
	this->Table2Indicator->setBackColor(this->getBackColor());
	this->Table3Values->setBackColor(this->getBackColor());
	this->Table1Input->setBaseColor( FXRGB(192,192,192));
	this->Table1Input->setColumnHeaderHeight(0);
	this->Table1Input->setRowHeaderWidth(0);
	this->Table2Indicator->setRowHeaderWidth(0);
	this->Table2Indicator->setColumnHeaderHeight(0);
	this->Table3Values->setRowHeaderWidth(0);
	this->Table3Values->setColumnHeaderHeight(0);
	this->updateListBox1();
//  this->Table1Input->setCellColor(0,0,FXRGB(255,255,255));
//  this->Table1Input->setCellColor(0,1,FXRGB(255,240,240));
//  this->Table1Input->setCellColor(1,0,FXRGB(240,255,240));
//  this->Table1Input->setCellColor(1,1,FXRGB(240,240,255));	
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
}

MainWindow::~MainWindow()
{ delete this->FontDefaultDrawFont;
}



void MainWindow::create()
{ FXMainWindow::create();
	this->FontDefaultDrawFont->create();
  show(PLACEMENT_SCREEN);
}
long MainWindow::onSpinner1and2Change(FX::FXObject *sender, FX::FXSelector sel, void *ptr)
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

long MainWindow::onButton1MainButtonCommand(FXObject*sender,FXSelector sel,void*ptr)
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

long MainWindow::onButton2EvaluateCommand(FXObject*sender,FXSelector sel,void*ptr)
{	return 1;
}

long MainWindow::onToggleButton1Custom(FXObject*,FXSelector sel,void*)
{ FXbool tempB= this->ToggleButton1Custom->getState();
	if (tempB==0)
	{	this->theComputationSetup.UsingCustomVectors=false;
	}else
	{ this->theComputationSetup.UsingCustomVectors=true;
	}
	this->updateListBox1();
	return 1;
}

void MainWindow::updateListBox1()
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

long MainWindow::onListBox1WeylGroupCommand(FXObject*sender,FXSelector sel,void* ptr)
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

void MainWindow::initWeylGroupInfo()
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

int main(int argc,char *argv[])
{ FXApp application("Vector Partition","Vector Partition");
  application.init(argc,argv);
  MainWindow1= new MainWindow(&application);
  application.create();
  //application.
  return application.run();
}

void WorkThreadClass::run()
{	::RunComputationalThread();
}

void MainWindow::ReadVPVectors()
{ if (this->theComputationSetup.UsingCustomVectors)
	{	root::AmbientDimension= this->Spinner1Dimension->getValue();
		this->theComputationSetup.VPVectors.size=0;
		for (int i=0;i<this->Spinner2NumVectors->getValue();i++)
		{ root tempRoot;
			for (int j=0;j<root::AmbientDimension;j++)
			{	int tempI=::FXIntVal(this->Table1Input->getItemText(i,j));
				tempRoot.coordinates[j].AssignInteger(tempI);
			}
			this->theComputationSetup.VPVectors.AddRoot(tempRoot);
		}
		this->theComputationSetup.VPVectors.ComputeDebugString();
	}
	for (int j=0;j<this->theComputationSetup.WeylGroupIndex;j++)
	{	int tempI=::FXIntVal(this->Table2Indicator->getItemText(0,j));
		this->theComputationSetup.IndicatorRoot.coordinates[j].AssignInteger(tempI);
	}
	this->theComputationSetup.IndicatorRoot.dimension=this->theComputationSetup.WeylGroupIndex;
}

void MainWindow::initTableFromRowsAndColumns(int r, int c)
{	this->NumVectors=r;
	this->theComputationSetup.WeylGroupIndex= c;	
	this->Table1Input->setTableSize(this->NumVectors,this->theComputationSetup.WeylGroupIndex);
	for (int j=0;j<c;j++)
	{ this->Table1Input->setColumnWidth(j,20);
	}
	this->Table1Input->setVisibleColumns(min(c,10));
	this->Table1Input->setVisibleRows(min(r,20));
}

void MainWindow::TurnOffAllDangerousButtons()
{ this->ListBox1WeylGroup->disable();
	this->Spinner1Dimension->disable();
	this->Spinner2NumVectors->disable();
	this->ToggleButton1Custom->disable();
}

void MainWindow::TurnOnAllDangerousButtons()
{ if (this->theComputationSetup.UsingCustomVectors)
	{ this->Spinner1Dimension->enable();
		this->Spinner2NumVectors->enable();
	}
	else
	{ this->ListBox1WeylGroup->enable();
	}
	this->ToggleButton1Custom->enable();
}


void outputText(std::string theOutput)
{
} 
void FeedDataToIndicatorWindow(IndicatorWindowVariables& output)
{	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=true;
	if (MainWindow1->WorkThread1.CriticalSectionPauseButtonEntered)
	{	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
		return;
	}
	MainWindow1->Label1ProgressReport->::FXLabel::setText(output.ProgressReportString1.c_str());
	MainWindow1->Label2ProgressReport->::FXLabel::setText(output.ProgressReportString2.c_str());
	MainWindow1->Label3ProgressReport->::FXLabel::setText(output.ProgressReportString3.c_str());
	MainWindow1->Label4ProgressReport->::FXLabel::setText(output.ProgressReportString4.c_str());
	MainWindow1->WorkThread1.CriticalSectionWorkThreadEntered=false;
} 
//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line 
// 5 = invisible line (no line)
void drawline(double X1, double Y1, double X2, double Y2, 
								unsigned long thePenStyle, int ColorIndex)
{ ::FXDCWindow dc(MainWindow1->Canvas1DrawCanvas);
	switch (thePenStyle)
	{ case 0: dc.::FXDCWindow::setLineStyle(::LINE_SOLID); break;
		case 1: dc.::FXDCWindow::setLineStyle(::LINE_ONOFF_DASH); break;
		case 2: dc.::FXDCWindow::setLineStyle(::LINE_DOUBLE_DASH); break;
		case 5: return;
	}
	dc.::FXDCWindow::setForeground(ColorIndex);
	dc.::FXDCWindow::drawLine((int)X1, (int)Y1,(int) X2,(int) Y2);
//	dc.setForeground(FXRGB(0,0,0));
//  dc.fillRectangle(0,0,MainWindow1->Canvas1DrawCanvas->getWidth(),MainWindow1->Canvas1DrawCanvas->getHeight());
}
void drawtext(double X1, double Y1, const char* text, int length, int color)
{	::FXDCWindow dc(MainWindow1->Canvas1DrawCanvas);
	dc.setFont(MainWindow1->FontDefaultDrawFont);
	dc.setForeground(color);
	dc.setBackground(MainWindow1->Canvas1DrawCanvas->getBackColor());
	dc.setFillStyle(FILL_STIPPLED);
	dc.drawImageText((int)X1, (int)Y1,text,length);
}

