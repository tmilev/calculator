#include ".\fox-1.6.36\include\fx.h"
#include ".\fox-1.6.36\include\FXExpression.h"
#include "polyhedra.h"

class MainWindow : public FXMainWindow 
{ FXDECLARE(MainWindow)
protected:
  MainWindow(){};
private:
	::FXTable* Table1Input;
	::FXComboBox* ComboBox1WeylGroup;
public:
  long onButton1MainButtonCommand(FXObject*,FXSelector,void*);
  long onComboBox1WeylGroupCommand(FXObject*,FXSelector,void*);
  enum
  {	ID_Button1MainButtonCommand=FXMainWindow::ID_LAST,
		ID_Table1InputCommand,
		ID_ComboBox1WeylGroupCommand,
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
  FXMAPFUNC(SEL_CHANGED, MainWindow::ID_ComboBox1WeylGroupCommand, 
						MainWindow::onComboBox1WeylGroupCommand),
};

// Macro for the ScribbleApp class hierarchy implementation
FXIMPLEMENT(MainWindow,FXMainWindow,ScribbleWindowMap,ARRAYNUMBER(ScribbleWindowMap))

MainWindow::MainWindow(FXApp *a):FXMainWindow(a,"I will eat your RAM",NULL,NULL,DECOR_ALL,0,0,800,600)
{ new FXButton(	this,"&Go",NULL,this,MainWindow::ID_Button1MainButtonCommand,
								FRAME_THICK|FRAME_RAISED,0,0,0,0,10,10,5,5);
	this->ComboBox1WeylGroup= 
		new FXComboBox(	this,2,this,MainWindow::ID_ComboBox1WeylGroupCommand,
										0,50,0,100,10);					
	this->Table1Input= new FXTable(this,this,MainWindow::ID_Table1InputCommand,0,0,30,200,200);
	this->Table1Input->setTableSize(5,5);
	this->Table1Input->setColumnHeaderHeight(0);
	this->Table1Input->setRowHeaderWidth(0);
	this->Table1Input->setVisibleRows(5);
	this->Table1Input->setVisibleColumns(5);
//  this->Table1Input->setCellColor(0,0,FXRGB(255,255,255));
//  this->Table1Input->setCellColor(0,1,FXRGB(255,240,240));
//  this->Table1Input->setCellColor(1,0,FXRGB(240,255,240));
//  this->Table1Input->setCellColor(1,1,FXRGB(240,240,255));	
  this->Table1Input->setItemText(1,1,"asdf");
  this->Table1Input->setItemText(2,1,"asdf");
}

MainWindow::~MainWindow()
{
}

void MainWindow::create()
{ FXMainWindow::create();
  show(PLACEMENT_SCREEN);
}

long MainWindow::onButton1MainButtonCommand(FXObject*,FXSelector,void*)
{  return 1;
}

long MainWindow::onComboBox1WeylGroupCommand(FXObject*,FXSelector,void*)
{  return 1;
}

int main(int argc,char *argv[])
{ FXApp application("Vector Partition","Vector Partition");
  application.init(argc,argv);
  new MainWindow(&application);
  application.create();
  return application.run();
}









void outputText(std::string theOutput)
{
} 
void FeedDataToIndicatorWindow(IndicatorWindowVariables& output)
{
  std::cout <<"Num Reduced Fractions:"<< output.NumReducedPartFractions;
  std::cout <<"\tNum Total Fractions:"<< output.NumPartFractionsLoaded << "\n";
} 
void drawline(double X1, double Y1, double X2, double Y2, 
								unsigned long thePenStyle, int ColorIndex)
{
}
void drawtext(double X1, double Y1, const char* text, int length, int color)
{
}

