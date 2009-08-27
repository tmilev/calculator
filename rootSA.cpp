//*********************************************************************************************************
//*********************************************************************************************************
//Vector partition function - computes an algebraic expression 
//                            for the vector partition function
//CopyRight (C) 2009: Todor Milev 
//Contributors: Thomas Bliem, Todor Milev
//Todor Milev would like to thank http://www.cplusplus.com/forum/ for the valuable
//advice and help with C++. Special thanks to helios, Disch, Grey Wolf, jsmith, 
//Hammurabi and Duoas for the helpful comments and advice on C++!
//
//
//email: t.milev@jacobs-university.de
//To Thomas: add your email here if you wish it displayed
//This is open source software (i.e. "public" software or "free" software).
//The code is licensed under the Library General Public License version 3.0.
//See the file License_LGPL-3.0.txt for the license information.
//You are free to use, modify and redistribute this code and the resulting program 
//under the terms of Library General Public License version 3.0.
//You should have received a copy of the GNU Library General Public License 
//along with this program. 
//If not, see <http://www.gnu.org/licenses/>.
//*********************************************************************************************************
//*********************************************************************************************************
//The (miniature for the moment) documentation of this project is contained in 
//
//./tex_docs/polyhedra.tex     /    ./tex_docs/polyhedra.pdf
//
//For the time being, not all functions available here are described in the documentation.
//That will hopefully be remedied in future versions of the code.
//For a baby version of an algorithm description see partial_fraction.pdf.
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

#define dont_define_Min_and_Max_in_polyhedra_h
#include "polyhedra.h"
#include <windows.h>
#include <Commdlg.h>
 
#define RadioButtonDashesIndex									156
#define RadioButtonInvisibleChambersIndex				158
#define EditBoxOutputIndex											157
#define RBDisplayingGraphicsIndex								1155
#define RBDisplayChamberIndicesIndex						1156
#define RBComputingPolysIndex										1157
#define IndicatorWindowPartialFractionsLabel1		1158
#define IndicatorWindowPartialFractionsLabel2		1159
#define IndicatorWindowPauseButton							1160
#define IndicatorWindowPartialFractionsLabel3		1161
#define IndicatorWindowPartialFractionsLabel4		1162
#define IndicatorWindowPartialFractionsLabel5		1163
#define IndicatorWindowEdit1										1164
#define IndicatorWindowEdit2										1165
#define IndicatorWindowhButton1									1166
#define IndicatorWindowhButton2									1167
#define IndicatorRButton1												1168
#define IndicatorRButton2												1169
#define IndicatorCombo1													1170
#define IndicatorEditWeights										1171
#define IndicatorWindowEdit3										1183
#define IndicatorWindowhButton3									1184
#define IndicatorWindowhButton4									1185
#define IndicatorWindowPartialFractionsLabel6		1186
#define IndicatorWindowPartialFractionsLabel7		1187
#define IndicatorWindowPartialFractionsLabel8		1188
#define IndicatorWindowPartialFractionsLabel9		1189
#define IndicatorWindowDrawGraphics							1190
#define IndicatorRButton3												1191
#define IndicatorRButton4												1192
#define IndicatorPauseComputationalThread				1193
#define IndicatorWindowEdit4theOutput						1194
#define IndicatorWindowPartialFractionsLabel10	1195
#define IndicatorWindowEdit5NumericValue				1196
#define IndicatorWindowButton5EvaluateVPF				1197

HINSTANCE hInst;
LRESULT CALLBACK	WndProcIndicatorWindow(HWND, UINT, WPARAM, LPARAM);
void draw(void);
int APIENTRY IndicatorWindow(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow);
extern IndicatorWindowVariables IndicatorWindowGlobalVariables;
extern PolynomialOutputFormat PolyFormatLocal;
extern int AltMain(void);
extern int FillInRoots(intRoots& ToBeSplit, root& IndicatorRoot);
void RunAlgorithm1();

int __stdcall WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{	hInst=hInstance;
	::RunAlgorithm1();
	return 0;
}

class IndicatorWindowClass
{	
public:
	QuasiPolynomial thePoly;
	int X,Y;
	int height,width;
	short rootRank;
	int SlicingIndex;
	char TypeWeylGroup;
	int oldCombo1State;
	int ComputationInitializedAndReadyToGo;
	bool ComputationMustDie;	
	bool ComputationalThreadIsDead;
	bool ComputationInProgress;
	int hEditBoxOutputHeight;	int EditBoxOutputBottomTolerance;
	bool usePFFile;
	bool useVPFile;
	bool initDLLneeded;
	bool RecalculationChambersNeeded;
	bool noGraphics;
	bool RequireOutputFile;
	intRoot rootEvaluateWeights;
	roots InputRoots;
	HDC theDC;
	HANDLE ComputationalThread;
	HWND hMain;
	HWND hLabel1;	HWND hLabel2;	HWND hLabel3;	HWND hLabel4;
	HWND hLabel5;	HWND hLabel6;	HWND hLabel7;	HWND hLabel8;
	HWND hLabel9;	HWND hLabel10;
	HWND hEdit1; HWND hEdit2;	HWND hEdit3;
	HWND hEdit4theOutput; HWND hEdit5NumericValue;
	HWND hButton1; HWND hButton2;	HWND hButton3; HWND hButton4;
	HWND hButton5EvaluateVPF;
	HWND hRadioButton1; HWND hRadioButton2;
	HWND hRadioButton3; HWND hRadioButton4;
	HWND hComboBox1;
	static const int numEdits=10;
	static const char* className;
	HWND hEditWeightsIndicator[numEdits];
	HWND hEditWeights[numEdits];
	//HWND hEditWeights
	HWND hButtonPause;
	HFONT theFont;
	HBRUSH LabelBrush1;
	PAINTSTRUCT thePainstruct;
	std::string stringEdit5NumericValue;
	std::string stringTitle;
	std::string stringEditWeights[numEdits];
	std::string stringFile1;
	std::string stringFile2;
	std::string stringFile3;
	std::string stringFile4;
	std::string stringNumFractions;
	std::string stringLabel9;
	std::string stringComputedContributions;
	std::string stringMonomials;
	std::string stringButtonPause;
	std::string stringLabel5;
	std::string stringCombo1;
	std::string stringTheOutput;
	std::fstream file3Output;
	void CreationProcedures(HWND hWnd);
	void clickIndicatorWindowPauseButton();
	void clickButton1();
	void clickCombo1();
	void clickButton5EvaluateVPF();
	void RefreshRadioButtonStates();
	void UpdateEditWeights();
	void GetRootFromEditWeights(root& output);
	void EnlargeWindowForPFComputation();
	void EnlargeWindowForGraphics();
	void clickCanvas(HWND hWnd, int x, int y);
	bool initFiles();	
	void clickButton3();
	void UpdateLabels5to9fromStrings();
	void PauseComputationalThread();
	void UpdateEdit4output();
	void FillInOPENFILENAME
				(std::string& outputString, HWND hAssociatedEditBox);
	IndicatorWindowClass()
	{	this->theFont=0;this->LabelBrush1=0;this->ComputationalThread=0;oldCombo1State=1;
		this->ComputationInitializedAndReadyToGo=1; this->usePFFile=false; this->useVPFile=false;
		this->rootRank=3; this->TypeWeylGroup='B'; this->stringTitle="I will eat your RAM";
		this->SlicingIndex=0; this->stringFile3="C:\\ouput.txt"; this->initDLLneeded=true;
		this->RecalculationChambersNeeded=true; this->ComputationMustDie=false;
		this->ComputationalThreadIsDead=false; this->noGraphics=true; this->RequireOutputFile=false;
		this->LabelBrush1 = CreateSolidBrush(RGB(192,192,192));
	};
	void SetIndicator(intRoot& input);
	~IndicatorWindowClass()
	{ DeleteObject(this->theFont);
		DeleteObject(this->LabelBrush1);
	};   
};

const char* IndicatorWindowClass::className="VectorPartitionWindowClass\0";
IndicatorWindowClass IndicatorWindow1;
extern CombinatorialChamberPointers TheBigOutput;
extern FacetPointers TheBigFacetOutput;
extern int NextDirectionIndex;
extern int RankGlobal;
extern roots InputRoots;
const int ClickToleranceX=10;
const int ClickToleranceY=10; 
Polynomials<Rational> DiscreteIntegrationPolynomials;
extern DrawingVariables TDV;

void FeedDataToIndicatorWindow(IndicatorWindowVariables& output)
{	std::stringstream out1, out3, out4,out5;
	out1<<output.NumReducedPartFractions<<" reduced fractions out of "<<output.NumPartFractionsLoaded;
	IndicatorWindow1.stringNumFractions= out1.str();
	out3<< output.NumComputedContributions<<" contributions out of "
			<< output.NumProcessedFractions <<" processed fractions";
	IndicatorWindow1.stringComputedContributions= out3.str();
	out4<<"Processed "<< output.NumProcessedMonomials<<" out of "<<output.TotalNumMonomials<<" monomials";
	IndicatorWindow1.stringMonomials= out4.str();
	IndicatorWindow1.stringLabel9.assign(output.StatusString);
	IndicatorWindow1.UpdateLabels5to9fromStrings();
	if (output.rootIsModified)
	{ output.rootIsModified=false;
		IndicatorWindow1.SetIndicator(output.modifiedRoot);
	}
}

void GUIthread(void* pParams)
{ IndicatorWindow(hInst,0,(LPTSTR)"",1);
}

void ComputationalThread()
{ while(!IndicatorWindow1.ComputationMustDie)
	{	while(IndicatorWindow1.ComputationInitializedAndReadyToGo==0)
		{ if(IndicatorWindow1.ComputationMustDie) 
			{	IndicatorWindow1.ComputationalThreadIsDead =true;
				return;
			}
		}
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo!=1)
		{	IndicatorWindow1.RecalculationChambersNeeded=false;
		}
		IndicatorWindow1.ComputationInProgress=true;
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo==2)
		{ rootFKFTcomputation r1;
			r1.useOutputFileForFinalAnswer=false;
			r1.RunA2A1A1inD5beta12221(true,true);
		}
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo==3)
		{ thePFcomputation tempPFComp;
			tempPFComp.Run();
		}
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo==5)
		{ AltMain();
		}
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo==4)
		{ rootSubalgebra tempSA;
			//tempSA.RunE7_A2plusA5();
			tempSA.RunF4_B4();
			tempSA.RunF4_A3_A1();
			tempSA.RunF4_A2_A2();
			tempSA.RunF4_C3_A1();
			tempSA.RunF4_D4();
			tempSA.RunF4_B2_2A1();
			tempSA.RunF4_4A1();
//			tempSA.RunE8_2A4();
//			tempSA.RunE8_A8();
//			tempSA.RunE8_D8();
//			tempSA.RunE8_A1_A7();
//			tempSA.RunE8_A5_A1_A2();
//			tempSA.RunE8_4A2();
//			tempSA.RunE8_A2_E6();
//			tempSA.RunE8_A1_E7();
//			tempSA.RunE8_D6_2A1();
//			tempSA.RunE8_D5_A3();
//			tempSA.RunE8_2D4();
//			tempSA.RunE8_D4_4A1();
//			tempSA.RunE8_2A3_2A1();
//			tempSA.RunE8_8A1();
		}
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo==1)
		{ PolyFormatLocal.MakeAlphabetxi();
			partFractions tempPF;
			root tempRoot;
			if (IndicatorWindow1.initDLLneeded)
			{	initDLL(IndicatorWindow1.rootRank);		
			}
			IndicatorWindow1.initDLLneeded=false;
			IndicatorWindow1.noGraphics=true;
			IndicatorWindow1.GetRootFromEditWeights(tempRoot);
			partFraction::MakingConsistencyCheck=false;
			partFraction::theVectorToBePartitioned=IndicatorWindow1.rootEvaluateWeights;
			//partFractions::AnErrorHasOccurredTimeToPanic=true;
			//partFraction::theVectorToBePartitioned.initFromInt(5,10,12,200,200);
			//tempRoot.InitFromIntegers(1,1,1,1,1);
			tempPF.ComputeKostantFunctionFromWeylGroup
				(	IndicatorWindow1.TypeWeylGroup,(unsigned char)IndicatorWindow1.rootRank,
					IndicatorWindow1.thePoly,&tempRoot,IndicatorWindow1.usePFFile,
					IndicatorWindow1.useVPFile);
			PolynomialOutputFormat::LatexMaxLineLength=125;
			tempRoot.ComputeDebugString();
			PolyFormatLocal.MakeRegularAlphabet();
			IndicatorWindow1.thePoly.ComputeDebugString();
			tempPF.ComputeDebugString();
			IndicatorWindow1.file3Output	<<"Start of LaTeX readable portion:\n"
																		<<"Chamber Indicator: "<<tempRoot.DebugString<<"\n"
																		<<"VP function:\n\\begin{eqnarray*}" 
																		<<IndicatorWindow1.thePoly.DebugString
																		<< "\\end{eqnarray*}";
			IndicatorWindow1.file3Output<< "\nPartial Fraction Decomposition\n"<<tempPF.DebugString
																	<< "\nEnd of LaTeX readable portion\nMore details:\n";	
			tempPF.WriteToFile(IndicatorWindow1.file3Output);
			IndicatorWindow1.file3Output.flush();
			PolynomialOutputFormat::LatexMaxLineLength=27;
			IndicatorWindow1.thePoly.ComputeDebugString();
			IndicatorWindow1.stringTheOutput.clear();
			IndicatorWindow1.stringTheOutput.append("\\begin{eqnarray*}&&\r\n");
			IndicatorWindow1.stringTheOutput.append(IndicatorWindow1.thePoly.DebugString);
			IndicatorWindow1.stringTheOutput.append("\\end{eqnarray*}");
		}
		if (IndicatorWindow1.ComputationInitializedAndReadyToGo==6)
		{ partFractions tempPF;
			::initDLL(2);
			intRoots exampleRoots;
			root indicatorRoot;
			IndicatorWindow1.rootRank=(short)FillInRoots(exampleRoots, indicatorRoot);
			partFractions::ListBasicObjectsActualSizeIncrement=2000;
			IntegerPoly::ListBasicObjectsActualSizeIncrement=5;
			exampleRoots.ComputeDebugString();
			tempPF.initFromRootSystem(exampleRoots,exampleRoots,0);
			tempPF.split();
			tempPF.ComputeDebugString();
			tempPF.partFractionsToPartitionFunctionAdaptedToRoot
			(IndicatorWindow1.thePoly,indicatorRoot,false,false,false,false);
			IndicatorWindow1.initDLLneeded=false;
			IndicatorWindow1.noGraphics=true;
			partFraction::MakingConsistencyCheck=false;
			partFraction::theVectorToBePartitioned=IndicatorWindow1.rootEvaluateWeights;
			PolynomialOutputFormat::LatexMaxLineLength=125;
			PolyFormatLocal.MakeRegularAlphabet();
			IndicatorWindow1.thePoly.ComputeDebugString();
			tempPF.ComputeDebugString();
			IndicatorWindow1.file3Output	<<"Start of LaTeX readable portion:\n"
																		<<"Chamber Indicator: "<<indicatorRoot.DebugString<<"\n"
																		<<"VP function:\n\\begin{eqnarray*}" 
																		<<IndicatorWindow1.thePoly.DebugString
																		<< "\\end{eqnarray*}";
			IndicatorWindow1.file3Output<< "\nPartial Fraction Decomposition\n"<<tempPF.DebugString
																	<< "\nEnd of LaTeX readable portion\nMore details:\n";	
			tempPF.WriteToFile(IndicatorWindow1.file3Output);
			IndicatorWindow1.file3Output.flush();
			PolynomialOutputFormat::LatexMaxLineLength=27;
			IndicatorWindow1.thePoly.ComputeDebugString();
			IndicatorWindow1.stringTheOutput.clear();
			IndicatorWindow1.stringTheOutput.append("\\begin{eqnarray*}\r\n");
			IndicatorWindow1.stringTheOutput.append(IndicatorWindow1.thePoly.DebugString);
			IndicatorWindow1.stringTheOutput.append("\\end{eqnarray*}");
		}
		::IndicatorWindow1.stringLabel9="Output file written";
		if (IndicatorWindowGlobalVariables.PerturbationHasOccurred)
		{  ::IndicatorWindow1.stringLabel9.append(". Indicator perturbed to regular.");
		}
		IndicatorWindow1.UpdateLabels5to9fromStrings();
		IndicatorWindow1.ComputationInitializedAndReadyToGo=0;
		IndicatorWindow1.EnlargeWindowForGraphics();
		WeylGroup tempW;
		if (IndicatorWindow1.TypeWeylGroup=='A')
		{ tempW.MakeAn((unsigned char)IndicatorWindow1.rootRank);}
		if (IndicatorWindow1.TypeWeylGroup=='B')
		{ tempW.MakeBn((unsigned char)IndicatorWindow1.rootRank);}
		if (IndicatorWindow1.TypeWeylGroup=='C')
		{ tempW.MakeCn((unsigned char)IndicatorWindow1.rootRank);}
		if (IndicatorWindow1.TypeWeylGroup=='D')
		{ tempW.MakeDn((unsigned char)IndicatorWindow1.rootRank);}
		if (IndicatorWindow1.TypeWeylGroup=='G')
		{ tempW.MakeG2();}
		tempW.ComputeRootsOfBorel(InputRoots);
		InputRoots.ComputeDebugString();
		::NextDirectionIndex=IndicatorWindow1.rootRank-1;		
		IndicatorWindow1.UpdateEdit4output();
		if (IndicatorWindow1.RecalculationChambersNeeded)
		{ ::SendMessage(IndicatorWindow1.hMain,WM_COMMAND,IndicatorWindowDrawGraphics,0);
		}
		::ShowWindow(IndicatorWindow1.hLabel10,SW_SHOW);
		IndicatorWindow1.noGraphics=false;
		IndicatorWindow1.ComputationInProgress=false;
		RECT tempRect;
		tempRect.top=200; tempRect.bottom=600; tempRect.left=0; tempRect.right=800;
		::InvalidateRect(IndicatorWindow1.hMain,&tempRect,1);
	//	::SendMessage(IndicatorWindow1.hMain,WM_PAINT,0,0);
		::SendMessage(IndicatorWindow1.hMain,WM_COMMAND,IndicatorPauseComputationalThread,0);
	}
	IndicatorWindow1.ComputationalThreadIsDead =true;
	return;
}

void RunAlgorithm1( )
{	IndicatorWindow1.ComputationalThread=CreateThread
			(0,0, (LPTHREAD_START_ROUTINE)ComputationalThread,0,0,0);
	::SuspendThread(IndicatorWindow1.ComputationalThread);
	::IndicatorWindowGlobalVariables.Pause=true;
	GUIthread(0);
}

bool IndicatorWindowClass::initFiles()
{ if (this->file3Output.is_open())
	{ return true;}
	this->file3Output.open(this->stringFile3.c_str(),std::fstream::out | std::fstream::trunc);
	return this->file3Output.is_open();
}

void IndicatorWindowClass::clickButton5EvaluateVPF()
{ root tempR;
	this->GetRootFromEditWeights(tempR);
	LargeRational tempRat;
	this->thePoly.Evaluate(this->rootEvaluateWeights,tempRat);
	tempRat.ElementToString(this->stringEdit5NumericValue);
	::SetWindowText(this->hEdit5NumericValue,this->stringEdit5NumericValue.c_str());
}

void IndicatorWindowClass::clickCanvas(HWND hWnd, int x, int y)
{	char buff[100];
	wsprintf( buff,  "X:%i",x);
	wsprintf( &buff[5], "Y:%i",y);
	int Realx=x; int Realy=y;
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
		::InvalidateRect(hWnd, NULL, TRUE); 
		SendMessage(hWnd, WM_PAINT, 0, 0);	
	}
}

void draw(void)
{	IndicatorWindow1.theDC = BeginPaint(IndicatorWindow1.hMain, &IndicatorWindow1.thePainstruct);	
	if (!IndicatorWindow1.noGraphics)
	{	root tempRoot;
		IndicatorWindow1.GetRootFromEditWeights(tempRoot);
		drawOutput(TDV,TheBigOutput,InputRoots,NextDirectionIndex,tempRoot);
		EndPaint(IndicatorWindow1.hMain, &IndicatorWindow1.thePainstruct);
	}
}

void drawtext(double X1, double Y1,const char* text, int length, int color)
{	int x1,y1;
	x1=int(X1);
	y1=int(Y1);
	::SetBkMode(IndicatorWindow1.theDC, TRANSPARENT);
	::SetTextColor(IndicatorWindow1.theDC,color); 
	TextOut(IndicatorWindow1.theDC, x1, y1, text,length);
//	DrawText(device1,text, x1, y1, text, length);
}

void drawline(double X1, double Y1, double X2, double Y2, DWORD thePenStyle, int ColorIndex)
{	HPEN p, oldPen;
	::SetBkMode(IndicatorWindow1.theDC, TRANSPARENT);
	p = CreatePen (thePenStyle, 0, ColorIndex);
	oldPen=(HPEN)::SelectObject(IndicatorWindow1.theDC,p);
	int x1,y1,x2,y2;
	x1=int(X1);	x2=int(X2);	y1=int(Y1);	y2=int(Y2);
	::MoveToEx (IndicatorWindow1.theDC, x1,y1, 0);
	::LineTo (IndicatorWindow1.theDC, x2,y2);
	SelectObject(IndicatorWindow1.theDC,oldPen);
	::DeleteObject(p);
//	::EndPaint(hCurrentWindow,&paintStr);
//	::ReleaseDC(hCurrentWindow,device1);

}

std::string outputS;

void outputText(std::string theOutput)
{	outputS.assign(theOutput); 
}

int APIENTRY IndicatorWindow( HINSTANCE hInstance,
															HINSTANCE hPrevInstance,
															LPTSTR    lpCmdLine,
															int       nCmdShow)
{	MSG msg;
	WNDCLASSEX wcex={sizeof(wcex)};// set first member, zero the rest
	wcex.style         = CS_HREDRAW|CS_VREDRAW;
  wcex.lpfnWndProc   = WndProcIndicatorWindow;
  wcex.hInstance     = hInstance;
  wcex.hCursor       = LoadCursor( NULL, IDC_ARROW );
  wcex.hbrBackground =IndicatorWindow1.LabelBrush1;
  wcex.lpszClassName = IndicatorWindowClass::className;
  RegisterClassEx(&wcex);
  hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern
	IndicatorWindow1.X= 50; IndicatorWindow1.Y=50; IndicatorWindow1.height=170; IndicatorWindow1.width=290;
  IndicatorWindow1.hMain = 
		CreateWindow( IndicatorWindowClass::className, IndicatorWindow1.stringTitle.c_str(),
									WS_OVERLAPPED | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU | WS_THICKFRAME | 
									WS_MINIMIZEBOX,	IndicatorWindow1.X, IndicatorWindow1.Y, 
									IndicatorWindow1.width, IndicatorWindow1.height, NULL, NULL, hInstance, NULL);
  if (!IndicatorWindow1.hMain)
  { return FALSE;
  }
  ShowWindow(IndicatorWindow1.hMain, nCmdShow);
  UpdateWindow(IndicatorWindow1.hMain);   
	while (GetMessage(&msg, NULL, 0, 0))
	{	TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

void IndicatorWindowClass::UpdateEditWeights()
{ for (int i=0;i<this->numEdits;i++)		
	{ if (i<rootRank)
		{ ::ShowWindow(this->hEditWeightsIndicator[i],SW_SHOW);
			::ShowWindow(this->hEditWeights[i],SW_SHOW);
			::SendMessage(this->hEditWeightsIndicator[i],WM_SETTEXT,0,
										(LPARAM)this->stringEditWeights[i].c_str());
			int x = ((i+1)*(100- rootRank*20))/(rootRank+1)+i*20;
			int y = ((i+1)*(130- rootRank*20))/(rootRank+1)+i*20;
			::MoveWindow(this->hEditWeightsIndicator[i],x,85,20,20,1);
			::MoveWindow(this->hEditWeights[i],420+y,150,20,20,1);
		}
		else
		{ ::ShowWindow(this->hEditWeightsIndicator[i],SW_HIDE);
			::ShowWindow(this->hEditWeights[i],SW_HIDE);
		}
	}
}

void IndicatorWindowClass::CreationProcedures(HWND hWnd)
{	this->theFont=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,"Arial");
	this->hLabel1 = 
		::CreateWindow(	"Static","Part. fraction file",WS_CHILD | WS_VISIBLE,100,
										0+2,120,20,hWnd,(HMENU)IndicatorWindowPartialFractionsLabel1, hInst, NULL);
	this->hLabel2 = 
		::CreateWindow(	"Static","Computations file",WS_CHILD | WS_VISIBLE,100,
										40+2,120,20,hWnd,(HMENU)IndicatorWindowPartialFractionsLabel2,hInst, NULL);
	this->hLabel3 = 
		::CreateWindow(	"Static","Chamber indicator (2rho default)",WS_CHILD| WS_VISIBLE,0,25,95,60,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel3,hInst, NULL);
	this->hLabel4 = 
		::CreateWindow(	"Static","Output",WS_CHILD| WS_VISIBLE,100,
										80+2,285,20,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel4,hInst, NULL);
	this->hLabel5 = 
		::CreateWindow(	"Static","The drawn chambers are finer than the actual. Single click basis vectors to pick/drop for better view."
										,WS_CHILD| WS_VISIBLE,120,
										150,285,60,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel4,hInst, NULL);
	this->hLabel6 = 
		::CreateWindow(	"Static"," ",WS_CHILD ,295,
										0+2,285,20,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel6,hInst, NULL);
	this->hLabel7 = 
		::CreateWindow(	"Static"," ",WS_CHILD ,295,
										20+2,285,20,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel7,hInst, NULL);
	this->hLabel8 = 
		::CreateWindow(	"Static"," ",WS_CHILD ,295,
										40+2,285,20,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel8,hInst, NULL);
	this->hLabel9 = 
		::CreateWindow(	"Static"," ",WS_CHILD ,295,
										60+2,285,20,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel9,hInst, NULL);
	this->hLabel10 = 
		::CreateWindow(	"Static","In LaTeX format:",WS_CHILD,
										420,190+2,130,20,hWnd,
										(HMENU)IndicatorWindowPartialFractionsLabel10,hInst, NULL);
	this->hButtonPause = 
		::CreateWindow(	"button","Go",WS_CHILD | WS_VISIBLE| WS_TABSTOP ,15,110,70,20,hWnd,
										(HMENU)IndicatorWindowPauseButton,hInst, NULL);
	this->hButton1 = 
		::CreateWindow(	"button","Select",WS_CHILD| WS_VISIBLE| WS_TABSTOP ,205,20,70,20,hWnd,
										(HMENU)IndicatorWindowhButton1,hInst, NULL);
	this->hButton2 = 
		::CreateWindow(	"button","Select",WS_CHILD| WS_VISIBLE| WS_TABSTOP ,205,60,70,20,hWnd,
										(HMENU)IndicatorWindowhButton2,hInst, NULL);
	this->hButton3 = 
		::CreateWindow(	"button","Select",WS_CHILD| WS_VISIBLE| WS_TABSTOP ,205,100,70,20,hWnd,
										(HMENU)IndicatorWindowhButton3,hInst, NULL);
	this->hButton5EvaluateVPF = 
		::CreateWindow(	"button","Evaluate",WS_CHILD| WS_VISIBLE| WS_TABSTOP ,420+130,150,70,20,hWnd,
										(HMENU)IndicatorWindowButton5EvaluateVPF,hInst, NULL);
	this->hEdit1 = 
		::CreateWindow(	"EDIT","C:\\pf.txt", WS_CHILD  | ES_WANTRETURN | WS_TABSTOP
										| WS_BORDER | ES_AUTOHSCROLL| WS_VISIBLE,
										100,20,100,20,hWnd,(HMENU)IndicatorWindowEdit1,hInst, NULL);
	this->hEdit2 = 
		::CreateWindow(	"EDIT","C:\\pfVP.txt", WS_CHILD  | ES_WANTRETURN | WS_TABSTOP
										| WS_BORDER | ES_AUTOHSCROLL| WS_VISIBLE,
										100,60,100,20,hWnd,(HMENU)IndicatorWindowEdit2,hInst, NULL);
	this->hEdit3 = 
		::CreateWindow(	"EDIT",IndicatorWindow1.stringFile3.c_str(), 
										WS_CHILD  | ES_WANTRETURN | WS_TABSTOP
										| WS_BORDER | ES_AUTOHSCROLL| WS_VISIBLE,
										100,100,100,20,hWnd,(HMENU)IndicatorWindowEdit3,hInst, NULL);
	this->hEdit4theOutput = 
		::CreateWindow(	"EDIT",IndicatorWindow1.stringTheOutput.c_str(), 
										WS_CHILD | WS_TABSTOP | ES_READONLY | ES_MULTILINE |
										WS_HSCROLL | WS_VSCROLL |
										WS_BORDER | ES_AUTOHSCROLL |ES_AUTOVSCROLL| WS_VISIBLE,
										420,210,200,210,hWnd,(HMENU)IndicatorWindowEdit4theOutput,hInst, NULL);
	this->hEdit5NumericValue=
		::CreateWindow(	"EDIT","", 
										WS_CHILD | WS_TABSTOP | ES_READONLY |
										WS_BORDER | ES_AUTOHSCROLL |WS_VISIBLE,
										420,170,200,20,hWnd,(HMENU)IndicatorWindowEdit5NumericValue,hInst, NULL);
	this->hRadioButton1= 
		::CreateWindow(	"button","Store PF",WS_CHILD | WS_VISIBLE 
										| BS_AUTOCHECKBOX| WS_TABSTOP ,
										205,0,70,20,hWnd,(HMENU)IndicatorRButton1,
										hInst, NULL);
	this->hRadioButton2= ::CreateWindow(	"button","Store VP",WS_CHILD | WS_VISIBLE 
										| BS_AUTOCHECKBOX| WS_TABSTOP ,
										205,40,70,20,hWnd,(HMENU)IndicatorRButton2,
										hInst, NULL);
	this->hRadioButton3= ::CreateWindow(	"button","Chamber labels",WS_CHILD | WS_VISIBLE 
										| BS_AUTOCHECKBOX| WS_TABSTOP ,
										0,150,120,20,hWnd,(HMENU)IndicatorRButton3,
										hInst, NULL);
	this->hRadioButton4= ::CreateWindow(	"button","Dashes",WS_CHILD | WS_VISIBLE 
										| BS_AUTOCHECKBOX| WS_TABSTOP ,
										0,170,90,20,hWnd,(HMENU)IndicatorRButton4,
										hInst, NULL);
	this->hComboBox1= ::CreateWindow(	"COMBOBOX","",WS_CHILD | WS_VISIBLE 
										| CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL,
										0,0,90,80,hWnd,(HMENU)IndicatorCombo1,
										hInst, NULL);
	this->stringEditWeights[0]="5";
	this->stringEditWeights[1]="8";
	this->stringEditWeights[2]="9";
	this->rootRank=3;
	for(int i=0;i<this->numEdits;i++)
	{ this->hEditWeightsIndicator[i]=::CreateWindow(	"EDIT","", WS_CHILD  | ES_WANTRETURN 
										| WS_TABSTOP 
										| WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
										0,85,20,20,hWnd,(HMENU)(IndicatorEditWeights+i),hInst, NULL);
		this->hEditWeights[i]=::CreateWindow(	"EDIT","", WS_CHILD  | ES_WANTRETURN 
										| WS_TABSTOP 
										| WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
										420,150,20,20,hWnd,(HMENU)(IndicatorEditWeights+i),hInst, NULL);
		::SendMessage(this->hEditWeightsIndicator[i],WM_SETFONT,(WPARAM)this->theFont,0);
	}
	this->UpdateEditWeights();
  ::SendMessage(this->hLabel1 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel2 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel3 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel4 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel5 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel6 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel7 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel8 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel9 ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hLabel10,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hEdit1  ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hEdit2  ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hEdit3  ,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hButtonPause,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hButton1,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hButton2,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hButton3,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hButton5EvaluateVPF,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hRadioButton1,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hRadioButton2,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hRadioButton3,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hRadioButton4,WM_SETFONT,(WPARAM)this->theFont,0);
  ::SendMessage(this->hComboBox1,WM_SETFONT,(WPARAM)this->theFont,0);
	::SendMessage(this->hRadioButton1,BM_SETCHECK,0,0);
	::SendMessage(this->hRadioButton2,BM_SETCHECK,0,0);
	::SendMessage(this->hRadioButton3,BM_SETCHECK,0,0);
	::SendMessage(this->hRadioButton4,BM_SETCHECK,1,0);
	::EnableWindow(this->hRadioButton1,0);
	::EnableWindow(this->hRadioButton2,0);
	this->stringCombo1="A2";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="A3";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="A4";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="A5";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="B2";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="B3";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="B4";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="C2";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="C3";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="C4";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="D4";
//	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
//	this->stringCombo1="F4";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	this->stringCombo1="G2";
	SendMessage(this->hComboBox1,CB_ADDSTRING,0,(LPARAM)this->stringCombo1.c_str());
	SendMessage(this->hComboBox1,CB_SETCURSEL,5,0);
  //SendMessage(this->hComboBox1,,1,0);
  IndicatorWindow1.theDC=::GetDC(this->hMain);
  ::SetBkColor(IndicatorWindow1.theDC,RGB(192,192,192));
  IndicatorWindow1.thePainstruct.hdc=IndicatorWindow1.theDC;
  IndicatorWindow1.thePainstruct.rcPaint.top=300;
  IndicatorWindow1.thePainstruct.rcPaint.bottom=800;
  IndicatorWindow1.thePainstruct.rcPaint.left=0;
  IndicatorWindow1.thePainstruct.rcPaint.right=600;
	this->RefreshRadioButtonStates();
	ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=100;
}

void IndicatorWindowClass::UpdateLabels5to9fromStrings()
{	::SendMessage(IndicatorWindow1.hLabel6,WM_SETTEXT,0,
								(LPARAM)IndicatorWindow1.stringNumFractions.c_str());	
	::SendMessage(IndicatorWindow1.hLabel9 ,WM_SETTEXT,0,
								(LPARAM)IndicatorWindow1.stringLabel9.c_str());	
	::SendMessage(IndicatorWindow1.hLabel7 ,WM_SETTEXT,0,
								(LPARAM)IndicatorWindow1.stringComputedContributions.c_str());	
	::SendMessage(IndicatorWindow1.hLabel8 ,WM_SETTEXT,0,
								(LPARAM)IndicatorWindow1.stringMonomials.c_str());	
	::RedrawWindow(IndicatorWindow1.hLabel6,0,0,0);
	::RedrawWindow(IndicatorWindow1.hLabel7,0,0,0);
	::RedrawWindow(IndicatorWindow1.hLabel8,0,0,0);
	::RedrawWindow(IndicatorWindow1.hLabel9,0,0,0);
}

void IndicatorWindowClass::GetRootFromEditWeights(root& output)
{ std::string tempS;
	tempS.resize(100);
	output.dimension=(unsigned char)this->rootRank;
	output.MakeZero();
	for (int i=0;i<min(this->rootRank,5);i++)
	{ ::GetWindowText(this->hEditWeightsIndicator[i],(LPSTR)tempS.c_str(),90);
		int x=std::atoi(tempS.c_str());
		output.coordinates[i].AssignInteger(x);
		::GetWindowText(this->hEditWeights[i],(LPSTR)tempS.c_str(),90);
		x=std::atoi(tempS.c_str());
		this->rootEvaluateWeights.elements[i]=x;
	}
	output.dimension=(unsigned char)this->rootRank;
}

void IndicatorWindowClass::EnlargeWindowForPFComputation()
{	RECT rect1;
	::GetWindowRect(this->hMain,&rect1);
	this->X= rect1.left;
	this->Y= rect1.top;
	this->width= rect1.right-rect1.left;
	this->height= rect1.bottom-rect1.top;
	this->width=670;
	::ShowWindow(this->hLabel6,SW_SHOW); ::ShowWindow(this->hLabel7,SW_SHOW);
	::ShowWindow(this->hLabel8,SW_SHOW); ::ShowWindow(this->hLabel9,SW_SHOW);
	::MoveWindow(this->hMain,this->X,this->Y,this->width,this->height,1);	
}

void IndicatorWindowClass::EnlargeWindowForGraphics()
{	RECT rect1;
	::GetWindowRect(this->hMain,&rect1);
	this->X= rect1.left;
	this->Y= rect1.top;
	this->height= 470;
	this->width=670;
	::MoveWindow(this->hMain,this->X,this->Y,this->width,this->height,1);	
}

void IndicatorWindowClass::PauseComputationalThread()
{ while (::SuspendThread(IndicatorWindow1.ComputationalThread)==-1){};
	this->ComputationInitializedAndReadyToGo=1;
	IndicatorWindowGlobalVariables.Nullify();
	this->ComputationInProgress=false;
	this->initDLLneeded=false;
	this->RecalculationChambersNeeded=false;
	IndicatorWindow1.stringButtonPause="Go";
	SendMessage(IndicatorWindow1.hButtonPause,WM_SETTEXT,0,
							(LPARAM)IndicatorWindow1.stringButtonPause.c_str());
}

void IndicatorWindowClass::UpdateEdit4output()
{ ::SetWindowText(this->hEdit4theOutput,this->stringTheOutput.c_str());
}

void IndicatorWindowClass::clickIndicatorWindowPauseButton()
{	::IndicatorWindowGlobalVariables.Pause=!IndicatorWindowGlobalVariables.Pause;
	if (!this->ComputationInProgress && !IndicatorWindowGlobalVariables.Pause)
	{ if (!this->initFiles()&& this->RequireOutputFile)
		{ std::string tempS= "Error opening file(s)!";
			::MessageBox(this->hMain,tempS.c_str(),tempS.c_str(),MB_OK);
			IndicatorWindowGlobalVariables.Pause=true;
			IndicatorWindow1.stringButtonPause="Go";
			SendMessage(IndicatorWindow1.hButtonPause,WM_SETTEXT,0,
							(LPARAM)IndicatorWindow1.stringButtonPause.c_str());
			return;
		}
	}
	if (IndicatorWindowGlobalVariables.Pause)
	{	while (::SuspendThread(IndicatorWindow1.ComputationalThread)==-1){};
		if (this->ComputationInProgress)
		{	IndicatorWindow1.stringButtonPause= "Resume";
		}
		else
		{	IndicatorWindow1.stringButtonPause="Go";
		}
	}else
	{	this->EnlargeWindowForPFComputation();
		::ResumeThread(IndicatorWindow1.ComputationalThread);
		IndicatorWindow1.stringButtonPause= "Pause";
	}
	SendMessage(IndicatorWindow1.hButtonPause,WM_SETTEXT,0,
							(LPARAM)IndicatorWindow1.stringButtonPause.c_str());
}

void IndicatorWindowClass::FillInOPENFILENAME
				(std::string& outputString, HWND hAssociatedEditBox) 
{	char buffOutputFileString[290];
	OPENFILENAME ofn;
	::ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->hMain;
	ofn.lpstrFile = buffOutputFileString;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(buffOutputFileString);
	ofn.lpstrFilter = "All *.*\0*.*\0Text *.txt\0*.txt\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if(GetOpenFileName(&ofn))
	{ outputString=buffOutputFileString;
		if (hAssociatedEditBox!=0)
		{::SendMessage(hAssociatedEditBox,WM_SETTEXT,0,(LPARAM)outputString.c_str());
		}
	}

}

void IndicatorWindowClass::clickButton1()
{ this->FillInOPENFILENAME(this->stringFile1,this->hEdit1);
}

void IndicatorWindowClass::clickButton3()
{ this->FillInOPENFILENAME(this->stringFile3,this->hEdit3);
}

void IndicatorWindowClass::RefreshRadioButtonStates()
{	int state=SendMessage(this->hRadioButton1,BM_GETSTATE,0,0);
	if(state%2==1) this->usePFFile= true;
	if(state%2==0) this->usePFFile= false;
	state=SendMessage(this->hRadioButton2,BM_GETSTATE,0,0);
	if(state%2==1) this->useVPFile= true;
	if(state%2==0) this->useVPFile= false;
	if (this->usePFFile)
	{ EnableWindow(this->hEdit1,1);
		EnableWindow(this->hButton1,1);
	}else
	{ EnableWindow(this->hEdit1,0);
		EnableWindow(this->hButton1,0);
	}
	if (this->useVPFile)
	{ EnableWindow(this->hEdit2,1);
		EnableWindow(this->hButton2,1);
	}else
	{ EnableWindow(this->hEdit2,0);
		EnableWindow(this->hButton2,0);
	}
	state=SendMessage(this->hRadioButton3,BM_GETSTATE,0,0);
	if (state%2==0) 
	{	TDV.DrawChamberIndices=false;}
	else
	{ TDV.DrawChamberIndices=true;}
	state=SendMessage(this->hRadioButton4,BM_GETSTATE,0,0);
	if (state%2==0) 
	{	TDV.DrawDashes=false;}
	else
	{ TDV.DrawDashes=true;}	
}

void IndicatorWindowClass::SetIndicator(intRoot &input)
{ for (int i=0;i<root::AmbientDimension;i++)
	{ std::stringstream out;
		out<<input.elements[i];
		this->stringEditWeights[i]=out.str();
	}
	this->UpdateEditWeights();
}

void IndicatorWindowClass::clickCombo1()
{ int newCombo1state=::SendMessage(this->hComboBox1,CB_GETCURSEL,0,0);
	if (newCombo1state!=this->oldCombo1State)
	{ switch(newCombo1state)
		{ case 0:
				this->TypeWeylGroup='A'; this->rootRank=2;
				this->stringEditWeights[0]="2";this->stringEditWeights[1]="2";
				break;
			case 1:
				this->TypeWeylGroup='A'; this->rootRank=3;
				this->stringEditWeights[0]="3";this->stringEditWeights[1]="4";this->stringEditWeights[2]="3";
				break;
			case 2:
				this->TypeWeylGroup='A'; this->rootRank=4;
				this->stringEditWeights[0]="4";this->stringEditWeights[1]="6";
				this->stringEditWeights[2]="6";this->stringEditWeights[3]="4";
				break;
			case 3:
				this->TypeWeylGroup='A'; this->rootRank=5;
				this->stringEditWeights[0]="5";this->stringEditWeights[1]="8";
				this->stringEditWeights[2]="9";this->stringEditWeights[3]="8";
				this->stringEditWeights[4]="5";
			break;
			case 4:
				this->TypeWeylGroup='B'; this->rootRank=2;
				this->stringEditWeights[0]="3";this->stringEditWeights[1]="4";
			break;
			case 5:
				this->TypeWeylGroup='B'; this->rootRank=3;
				this->stringEditWeights[0]="5";this->stringEditWeights[1]="8";
				this->stringEditWeights[2]="9";
				break;
			case 6:
				this->TypeWeylGroup='B'; this->rootRank=4;
				this->stringEditWeights[0]="7";this->stringEditWeights[1]="12";
				this->stringEditWeights[2]="15";this->stringEditWeights[3]="16";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
			case 7:
				this->TypeWeylGroup='C'; this->rootRank=2;
				this->stringEditWeights[0]="5";this->stringEditWeights[1]="8";
				this->stringEditWeights[2]="9";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
			case 8:
				this->TypeWeylGroup='C'; this->rootRank=3;
				this->stringEditWeights[0]="7";this->stringEditWeights[1]="12";
				this->stringEditWeights[2]="15";this->stringEditWeights[3]="16";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
			case 9:
				this->TypeWeylGroup='C'; this->rootRank=4;
				this->stringEditWeights[0]="7";this->stringEditWeights[1]="12";
				this->stringEditWeights[2]="15";this->stringEditWeights[3]="16";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
			case 10:
				this->TypeWeylGroup='D'; this->rootRank=4;
				this->stringEditWeights[0]="7";this->stringEditWeights[1]="12";
				this->stringEditWeights[2]="15";this->stringEditWeights[3]="16";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
/*			case 11:
				this->TypeWeylGroup='F'; this->rootRank=4;
				this->stringEditWeights[0]="7";this->stringEditWeights[1]="12";
				this->stringEditWeights[2]="15";this->stringEditWeights[3]="16";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
	*/	case 11:
				this->TypeWeylGroup='G'; this->rootRank=2;
				this->stringEditWeights[0]="4";this->stringEditWeights[1]="4";
				ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=3000;
				break;
			}
		this->noGraphics=true;
		this->RecalculationChambersNeeded=true;
		this->UpdateEditWeights();
	}
	this->oldCombo1State= newCombo1state;
}

LRESULT CALLBACK WndProcIndicatorWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	int wmId, wmEvent;
	static HDC tempHDC;
	static RECT tempRect;
	//int state;
	switch (message)
	{
	case WM_CREATE:
		IndicatorWindow1.CreationProcedures(hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:
		tempHDC = (HDC)wParam;
		SetBkColor(tempHDC,RGB(192,192,192));
		SetBkMode(tempHDC,TRANSPARENT);
		return (LRESULT) IndicatorWindow1.LabelBrush1;
		break;
	case WM_LBUTTONDOWN:
		int xPos,yPos;
		xPos = LOWORD(lParam); 
		yPos = HIWORD(lParam); 
		IndicatorWindow1.clickCanvas(IndicatorWindow1.hMain,xPos,yPos);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Menüauswahl bearbeiten:
		switch (wmId)
		{
		case IndicatorWindowPauseButton:	
			IndicatorWindow1.clickIndicatorWindowPauseButton();
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case IndicatorWindowhButton1:
			IndicatorWindow1.clickButton1();
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case IndicatorWindowhButton3:
			IndicatorWindow1.clickButton3();
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case IndicatorRButton1:
			IndicatorWindow1.RefreshRadioButtonStates();
			break;
		case IndicatorRButton2:
			IndicatorWindow1.RefreshRadioButtonStates();
			break;
		case IndicatorRButton3:
			IndicatorWindow1.RefreshRadioButtonStates();
			tempRect.top=200; tempRect.bottom=600; tempRect.left=0; tempRect.right=800;
			::InvalidateRect(IndicatorWindow1.hMain,&tempRect,1);
//			::RedrawWindow(IndicatorWindow1.hMain,0,0,0);
			break;
		case IndicatorRButton4:
			IndicatorWindow1.RefreshRadioButtonStates();
			tempRect.top=200; tempRect.bottom=600; tempRect.left=0; tempRect.right=800;
			::InvalidateRect(IndicatorWindow1.hMain,&tempRect,1);
//			::RedrawWindow(IndicatorWindow1.hMain,0,0,0);
			break;
		case IndicatorWindowButton5EvaluateVPF:
			IndicatorWindow1.clickButton5EvaluateVPF();
			break;
		case IndicatorCombo1:
			IndicatorWindow1.clickCombo1();
			break;
		case IndicatorPauseComputationalThread:
			IndicatorWindow1.PauseComputationalThread();
			break;
		case IndicatorWindowDrawGraphics:
			SliceTheEuclideanSpace(	::InputRoots,::NextDirectionIndex,IndicatorWindow1.rootRank,
															TheBigOutput,TheBigFacetOutput);
			tempRect.top=200; tempRect.bottom=600; tempRect.left=0; tempRect.right=800;
			::InvalidateRect(IndicatorWindow1.hMain,&tempRect,1);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT: 
		draw();
		break;
	case WM_DESTROY:
		::TerminateThread(IndicatorWindow1.ComputationalThread,0);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
