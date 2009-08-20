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
  std::cout <<"\tNum Total Fractions:"<< output.NumPartFractionsLoaded << "\n";
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
	void RunSomeTests();
};

int FillInRoots(intRoots& ToBeSplit, root& IndicatorRoot)
{ int Dimension=3, NumVectors=6;
	::initDLL(Dimension);
	ToBeSplit.SetSizeExpandOnTopNoObjectInit(NumVectors);
	//ToBeSplit.TheObjects[0].initFromInt(1,0,0,0,0);
	//ToBeSplit.TheObjects[1].initFromInt(2,0,0,0,0);
	//ToBeSplit.TheObjects[2].initFromInt(3,0,0,0,0);
	IndicatorRoot.InitFromIntegers(15,16,17,0,0);
	//ToBeSplit.TheObjects[0].initFromInt(1,0,0,0,0);
	//ToBeSplit.TheObjects[1].initFromInt(0,1,0,0,0);
	//ToBeSplit.TheObjects[2].initFromInt(0,0,1,0,0);
	ToBeSplit.TheObjects[5].initFromInt(1,1,0,0,0);
	ToBeSplit.TheObjects[4].initFromInt(0,1,1,0,0);
	ToBeSplit.TheObjects[3].initFromInt(1,1,1,0,0);
	ToBeSplit.TheObjects[2].initFromInt(0,1,2,0,0);
	ToBeSplit.TheObjects[1].initFromInt(1,1,2,0,0);
	ToBeSplit.TheObjects[0].initFromInt(1,2,2,0,0);
	return Dimension;
}


int main(void)
{ //The below lines compute a partial fraction decomposition for the 
	//generating function of the Kostant partition function for G2
	//printout tempPF.DebugString to get the final result
	ExampleComputation t1;
	t1.RunSomeTests();
		
	partFractions tempPF;
	intRoots theBorel,theVPbasis;
	intRoot tempWeight;
	roots tempRoots;
	WeylGroup tempW;
//	tempW.MakeBn(2);
	tempW.MakeG2();
//	tempW.MakeBn(3);
//	tempW.MakeAn(2);
//	tempW.MakeAn(3);
	PolyFormatLocal.MakeAlphabetxi();
	tempPF.ClearTheObjects();
	tempW.ComputeRootsOfBorel(tempRoots);
	theBorel.AssignRoots(tempRoots);
	theVPbasis.CopyFromBase(theBorel);
	theVPbasis.ComputeDebugString();
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
	ThomasExample.RunSomeTests();
	//ThomasExample.RunThomasExample();
}

void ExampleComputation::RunSomeTests()
{ LargeRational tempRat1, tempRat2;
	LargeRational tempRat ;
	std::string tempS,tempS1, tempS2;
	Rational fuckingBug;
	fuckingBug.init(-3,4);

	for(int x=88;x<200;x++)
	{	tempRat.MakeZero();
		for (int i=0;i<x;i++)
		{	tempRat2.AssignRational(fuckingBug);
			tempRat2.RaiseToPower(i);
			tempRat.Add(tempRat2);
			//tempRat.ElementToString(tempS);
		}
		tempRat1.AssignRational( fuckingBug);
		tempRat1.RaiseToPower(x);
		tempRat1.MultiplyByInt(-1);
		tempRat1.AddRational(Rational::TheRingUnit);
		tempRat2.AssignRational(fuckingBug);
		tempRat2.MultiplyByRational(Rational::TheRingMUnit);
		tempRat2.AddRational(Rational::TheRingUnit);
		tempRat2.Invert();
		tempRat1.MultiplyBy(tempRat2);
		tempRat1.ElementToString(tempS2);
		tempRat.ElementToString(tempS1);
		assert(tempRat.IsEqualTo(tempRat1));
	}
	tempRat1.ElementToString(tempS2);
//	LargeRational::AnErrorHasOccurredTimeToPanic=true;
	tempRat.Add(tempRat1);
	tempRat.ElementToString(tempS);
	tempRat.RaiseToPower(15);
	tempRat.ElementToString(tempS);

	tempRat1.AssignInteger(3);
	tempRat2.AssignInteger(4);
	tempRat1.RaiseToPower(21);
	tempRat2.RaiseToPower(15);
	tempRat1.MultiplyBy(tempRat2);
	tempRat1.ElementToString(tempS1);
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
	partFractions::ListBasicObjectsActualSizeIncrement=2000;
	IntegerPoly::ListBasicObjectsActualSizeIncrement=5;
	exampleRoots.ComputeDebugString();
	tempPF.initFromRootSystem(exampleRoots,exampleRoots,0);
	std::fstream tempF;
	tempF.open("C:/ThomasOutput.txt",std::fstream::out | std::fstream::trunc);
	assert(tempF.is_open());
	tempPF.split();
//	tempPF.ComputeDebugString();
	tempPF.ElementToStringOutputToFile(tempF,true,false,true);
	tempF.close();
	#ifndef _WINDOWS
		std::cout<<tempPF.DebugString;
	#endif
}