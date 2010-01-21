//This is a sample file using the polyhedra.h and polyhedra.cpp

#include "polyhedra.h"

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
{	int Dimension=2, NumVectors=4;/*
	root::AmbientDimension= (unsigned char) Dimension;
	ToBeSplit.SetSizeExpandOnTopNoObjectInit(NumVectors);
	IndicatorRoot.InitFromIntegers(405,603,0,0,0);
	ToBeSplit.TheObjects[0].initFromInt(2,3,0,0,0);
	ToBeSplit.TheObjects[1].initFromInt(4,1,0,0,0);
	ToBeSplit.TheObjects[2].initFromInt(5,3,0,0,0);
	ToBeSplit.TheObjects[3].initFromInt(2,5,0,0,0);
*/
	return Dimension;
}
void ExampleComputation::RunSomeTests()
{ intRoots toBeSplit;
	root indicator;
	::FillInRoots(toBeSplit,indicator);
	partFractions tempPF;
	GlobalVariables tempGV;
	tempPF.flagUsingIndicatorRoot=false;
	tempPF.initFromRootSystem(toBeSplit,toBeSplit,0,&tempGV);
	tempPF.flagUsingCheckSum=false;
	tempPF.flagMakingProgressReport=true;
	tempPF.split(&tempGV);
	tempPF.ComputeDebugString(&tempGV);
	QuasiPolynomial tempQP;
	tempPF.partFractionsToPartitionFunctionAdaptedToRoot
		(tempQP,indicator,false,false,&tempGV);
	intRoot tempR;
	tempR.initFromInt(5,0,0,0,0,0);
	Rational tempRat;
	tempQP.ComputeDebugString();
	tempQP.Evaluate(tempR,tempRat);
	std::string tempS;
	tempRat.ElementToString(tempS);
}

#ifndef USE_GUI
int main(void)
{ //The below lines compute a partial fraction decomposition for the
	//generating function of the Kostant partition function for G2
	//printout tempPF.DebugString to get the final result
	//ExampleComputation t1;
	//t1.RunSomeTests();

	partFractions tempPF;
	intRoots theBorel,theVPbasis;
	intRoot tempWeight;
	roots tempRoots;
	WeylGroup tempW;
	tempW.MakeG2();
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
	tempPF.flagUsingIndicatorRoot=false;
	tempPF.split();
	tempPF.ComputeDebugString();
	outputStream<<"\n=\n"<<tempPF.DebugString;
	std::string LatexString;
	LatexString=outputStream.str();
	std::cout << LatexString;
	//you can copy the content of LatexString to a latex compiler
	//it is supposed to display a partial fraction identity
	return 0;
}
#endif


void ExampleComputation::RunThomasExample()
{ PolyFormatLocal.MakeAlphabetxi();
	intRoots exampleRoots;
	GlobalVariables tempGV;
	exampleRoots.SetSizeExpandOnTopNoObjectInit(16);
	exampleRoots.TheObjects[0].initFromInt(12, 0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	1	,	0);
	exampleRoots.TheObjects[1].initFromInt(12, 3	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,-3	,	0	,	1);
	exampleRoots.TheObjects[2].initFromInt(12, -1,	2	,	0	,	0	,	0	,	0	,	0	,	1	,-1	,	2	,	1	,	0);
	exampleRoots.TheObjects[3].initFromInt(12, 0	,-3	,	3	,	0	,	0	,	0	,	1	,-3	,	2	,-3	,	0	,	1);
	exampleRoots.TheObjects[4].initFromInt(12, 0	,	0	,-2 ,	1	,	0	,	1	,-1	,	2	,-1	,	2	,	1	,	0);
	exampleRoots.TheObjects[5].initFromInt(12, 0	,	0	,	0	,	-3,	1	,-3	,	2	,-3	,	2	,-3	,	0	,	1);
	exampleRoots.TheObjects[6].initFromInt(12, -1,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[7].initFromInt(12, 0	,-1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[8].initFromInt(12, 0	,	0	,-1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[9].initFromInt(12, 0	,	0	,	0	,-1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[10].initFromInt(12,0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[11].initFromInt(12,0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[12].initFromInt(12,0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[13].initFromInt(12,0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0	,	0);
	exampleRoots.TheObjects[14].initFromInt(12,0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0	,	0);
	exampleRoots.TheObjects[15].initFromInt(12,0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	1	,	0	,	0);
	partFractions tempPF;
	partFractions::ListBasicObjectsActualSizeIncrement=2000;
	IntegerPoly::ListBasicObjectsActualSizeIncrement=5;
	exampleRoots.ComputeDebugString();
	tempPF.initFromRootSystem(exampleRoots,exampleRoots,0,&tempGV);
	std::fstream tempF;
	tempF.open("C:/ThomasOutput.txt",std::fstream::out | std::fstream::trunc);
	assert(tempF.is_open());
	tempPF.split(&tempGV);
//	tempPF.ComputeDebugString();
	tempPF.ElementToStringOutputToFile(tempF,true,false,true, &tempGV);
	tempF.close();
	#ifndef USE_GUI
		std::cout<<tempPF.DebugString;
	#endif
}
