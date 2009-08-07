//This is a sample file using the polyhedra.h and polyhedra.cpp

#define dont_define_Min_and_Max_in_polyhedra_h
#include "polyhedra.h"

//***************
//The below functions are required to use polyhedra.h. 
//They are defined in rootSA.cpp. 
//If you are not running Windows or aren't happy with the rootSA.cpp file,
//you need the below lines.
//***************
#ifndef WINDOWS
#ifndef _WINDOWS
void outputText(std::string theOutput)
{
} 
void FeedDataToIndicatorWindow(IndicatorWindowVariables& output)
{
  std::cout <<"Num Reduced Fractions:"<< output.NumReducedPartFractions;
  std::cout <<"Num Total Fractions:"<< output.NumPartFractionsLoaded;
} 
void drawline(double X1, double Y1, double X2, double Y2, 
								unsigned long thePenStyle, int ColorIndex)
{
}
void drawtext(double X1, double Y1, const char* text, int length, int color)
{
}
#endif
#endif
//*******************************************
//*******************************************
//*******************************************
//*******************************************



class ExampleComputation
{	
public: 
	void RunThomasExample();
};

int main(void)
{ //The below lines compute a partial fraction decomposition for the 
	//generating function of the Kostant partition function for G2
	//printout tempPF.DebugString to get the final result
	partFractions tempPF;
	intRoots theBorel,theVPbasis;
	intRoot tempWeight;
	roots tempRoots;
	WeylGroup tempW;
//	tempW.MakeBn(2);
//	tempW.MakeG2();
//	tempW.MakeBn(3);
	tempW.MakeAn(2);
//	tempW.MakeAn(3);
	tempPF.ClearTheObjects();
	tempW.ComputeRootsOfBorel(tempRoots);
	theBorel.AssignRoots(tempRoots);
	theVPbasis.CopyFromBase(theBorel);
	tempPF.initFromRootSystem(theVPbasis,theVPbasis,0);
	std::stringstream outputStream;
	tempPF.ComputeDebugString();
	outputStream<<tempPF.DebugString;
	tempPF.split();
	tempPF.ComputeDebugString();
	outputStream<<"\n=\n"<<tempPF.DebugString;
	std::string LatexString;
	LatexString=outputStream.str();
	//you can copy the content of LatexString to a latex compiler
	//it is supposed to display a partial fraction identity 
	ExampleComputation ThomasExample;
	ThomasExample.RunThomasExample();
}

void ExampleComputation::RunThomasExample()
{ ::initDLL(12);
	PolyFormatLocal.MakeAlphabetxi();
	intRoots exampleRoots;
	exampleRoots.SetSizeExpandOnTopNoObjectInit(16);
	exampleRoots.TheObjects[0].initFromInt(0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	1	,	0);
	exampleRoots.TheObjects[1].initFromInt(3	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,-3	,	0	,	1);
	exampleRoots.TheObjects[2].initFromInt(-1,	2	,	0	,	0	,	0	,	0	,	0	,	1	,-1	,	2	,	1	,	0);
	exampleRoots.TheObjects[3].initFromInt(0	,-3	,	3	,	0	,	0	,	0	,	1	,-3	,	2	,-3	,	0	,	1);
	exampleRoots.TheObjects[4].initFromInt(0	,	0	,-2 ,	1	,	0	,	1	,-1	,	2	,-1	,	2	,	1	,	0);
	exampleRoots.TheObjects[5].initFromInt(0	,	0	,	0	,	-3,	1	,-3	,	2	,-3	,	2	,-3	,	0	,	1);
	exampleRoots.TheObjects[6].initFromInt(-1,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[7].initFromInt(0	,-1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[8].initFromInt(0	,	0	,-1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[9].initFromInt(0	,	0	,	0	,-1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[10].initFromInt(0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[11].initFromInt(0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[12].initFromInt(0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[13].initFromInt(0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[14].initFromInt(0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0);
	exampleRoots.TheObjects[15].initFromInt(0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0);
	partFractions tempPF;
	IntegerPoly::ListBasicObjectsActualSizeIncrement=5;
	exampleRoots.ComputeDebugString();
	tempPF.initFromRootSystem(exampleRoots,exampleRoots,0);
	tempPF.split();
	tempPF.ComputeDebugString();
}