//*********************************************************************************************************
//*********************************************************************************************************
//Vector partition function - computes an algebraic expression
//                            for the vector partition function
//CopyRight (C) 2009: Todor Milev
//email: t.milev@jacobs-university.de
//
//Contributors: Thomas Bliem, Todor Milev
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

#include "polyhedra.h"

extern void outputText(std::string theOutput);
extern void FeedDataToIndicatorWindow(IndicatorWindowVariables& output);
//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
extern void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
extern void drawtext(double X1, double Y1, const char* text, int length, int color);
//taken from windows.h
#ifndef RGB
unsigned int RGB(int r, int g, int b)
{ return r | (g<<8) | b<<16;
}
#endif
//end of windows.h portion

unsigned char root::AmbientDimension;

Integer Integer::TheRingUnit  (1) ;
Integer Integer::TheRingMUnit (-1);
Integer Integer::TheRingZero  (0) ;
Integer& IOne =Integer::TheRingUnit ;
Integer& IMOne=Integer::TheRingMUnit;
Integer& IZero=Integer::TheRingZero ;

LargeInt LargeInt::TheRingUnit (1 );
LargeInt LargeInt::TheRingMUnit(-1);
LargeInt LargeInt::TheRingZero (0 );
LargeInt& LIOne =LargeInt::TheRingUnit ;
LargeInt& LIMOne=LargeInt::TheRingMUnit;
LargeInt& LIZero=LargeInt::TheRingZero ;

Rational Rational::TheRingUnit(1,1);
Rational Rational::TheRingMUnit(-1,1);
Rational Rational::TheRingZero(0,1);
Rational& ROne=Rational::TheRingUnit ;
Rational& RMOne=Rational::TheRingMUnit;
Rational& RZero =Rational::TheRingZero ;
Rational RHalf(1,2);
Rational RMHalf(-1,2);

LargeRational LargeRational::TheRingUnit(1);
LargeRational LargeRational::TheRingMUnit(-1);
LargeRational LargeRational::TheRingZero(0);
LargeRational& LROne=LargeRational::TheRingUnit;
LargeRational& LRMOne=LargeRational::TheRingMUnit;
LargeRational& LRZero=LargeRational::TheRingZero;

QuasiNumber QuasiNumber::TheRingUnit ;
QuasiNumber QuasiNumber::TheRingMUnit;
QuasiNumber QuasiNumber::TheRingZero ;
QuasiNumber& QNOne =QuasiNumber::TheRingUnit ;
QuasiNumber& QNMOne=QuasiNumber::TheRingMUnit;
QuasiNumber& QNZero=QuasiNumber::TheRingZero ;

CompositeComplexQN CompositeComplexQN::TheRingUnit (1);
CompositeComplexQN CompositeComplexQN::TheRingZero (1);
CompositeComplexQN CompositeComplexQN::TheRingMUnit(1);
CompositeComplexQN& CQNOne =CompositeComplexQN::TheRingUnit ;
CompositeComplexQN& CQNMOne=CompositeComplexQN::TheRingMUnit;
CompositeComplexQN& CQNZero=CompositeComplexQN::TheRingZero ;

PrecomputedTauknPointersKillOnExit* QuasiPolynomial::PrecomputedTaus;
PolynomialsRationalCoeff*
PrecomputedQuasiPolynomialIntegrals::PreComputedBernoulli;
PolynomialsRationalCoeff PreComputedBernoulliLocal;
PrecomputedTauknPointersKillOnExit PrecomputedTausLocal;

roots CombinatorialChamber::StartingCrossSectionNormals;
roots CombinatorialChamber::StartingCrossSectionAffinePoints;

int CombinatorialChamber::MethodUsed;
simplicialCones CombinatorialChamberPointers::startingCones;
bool CombinatorialChamber::PrintWallDetails;
bool CombinatorialChamber::DisplayingGraphics=true;
bool CombinatorialChamberPointers::PrintLastChamberOnly=true;
bool CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic=false;
bool CombinatorialChamber::ComputingPolys=false;
Cone CombinatorialChamberPointers::TheGlobalConeNormals;
int CombinatorialChamberPointers::NumTotalCreatedCombinatorialChambersAtLastDefrag;
int CombinatorialChamberPointers::LastReportedMemoryUse;
int CombinatorialChamberPointers::DefragSpacing;
std::fstream CombinatorialChamberPointers::TheBigDump;

ListObjectPointers<QuasiPolynomial> QuasiPolynomial::GlobalCollectorsPolys;
ListObjectPointers<ComplexQN> ComplexQN::GlobalCollectorsComplexQNs;
CombinatorialChamberPointers GlobalCollectorChambers;
FacetPointers GlobalCollectorFacets;

root ZeroRoot;
intRoot partFraction::theVectorToBePartitioned;
bool partFraction::MakingConsistencyCheck=false;
LargeRational partFractions::CheckSum;
LargeRational partFraction::CheckSum;
LargeRational partFraction::CheckSum2;
int Rational::LargestRationalHeight;
bool Rational::MinorRoutinesOnIgnoreErrors;

PolynomialsRationalCoeff PrecomputedPolys;
PrecomputedQuasiPolynomialIntegrals PrecomputedQuasiPolys;
PolynomialOutputFormat PolyFormatLocal;
int PolynomialOutputFormat::LatexMaxLineLength=125;
int PolynomialOutputFormat::LatexCutOffLine=15;
bool PolynomialOutputFormat::UsingLatexFormat=true;
bool PolynomialOutputFormat::CarriageReturnRegular=true;
PolynomialOutputFormat PolyFormatNumFrac;
IndicatorWindowVariables IndicatorWindowGlobalVariables;
template < > int ListBasicObjects<CombinatorialChamber*>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<ComplexQN>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<CompositeComplexQN> >::ListBasicObjectsActualSizeIncrement=10;
template < > int ListBasicObjects<int>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<Rational> >::ListBasicObjectsActualSizeIncrement=1;
template < > int HashedListBasicObjects<Monomial<Rational> >::PreferredHashSize=10;
template < > int HashedListBasicObjects<Monomial<CompositeComplexQN> >::PreferredHashSize=1000;
int QuasiPolynomial::TotalCreatedPolys=0;
int ComplexQN::NumTotalCreated=0;
template < > int ListBasicObjects<Facet *>::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<BasicQN *>::ListBasicObjectsActualSizeIncrement=1000;
template < > int ListBasicObjects<ComplexQN *>::ListBasicObjectsActualSizeIncrement=10000;
template < > int ListBasicObjects<QuasiPolynomial *>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<PrecomputedTaukn *>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<BasicComplexNumber>::ListBasicObjectsActualSizeIncrement=1;
CyclotomicList CompositeComplex::PrecomputedCyclotomic;
ListObjectPointers<BasicQN> BasicQN::GlobalCollectorsBasicQuasiNumbers;
int BasicQN::NumTotalCreated=0;
template < > int ListBasicObjects<BasicQN>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<QuasiNumber> >::ListBasicObjectsActualSizeIncrement=1000;
template < > int HashedListBasicObjects<Monomial<QuasiNumber> >::PreferredHashSize=1000;
template < > int HashedListBasicObjects<BasicQN>::PreferredHashSize= 1;
template < > int ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=1000;
template < > int HashedListBasicObjects<partFraction>::PreferredHashSize=10000;
template < > int ListBasicObjects<intRoot>::ListBasicObjectsActualSizeIncrement= 20;
template < > int HashedListBasicObjects<intRoot>::PreferredHashSize = 10000;
template < > int HashedListBasicObjects<oneFracWithMultiplicitiesAndElongations>::PreferredHashSize= 10;
template < > int ListBasicObjects<oneFracWithMultiplicitiesAndElongations>::ListBasicObjectsActualSizeIncrement=1;
template < > int HashedListBasicObjects<ElementWeylGroup>::PreferredHashSize=10000;
template < > int ListBasicObjects<ElementWeylGroup>::ListBasicObjectsActualSizeIncrement= 1000;
template < > int ListBasicObjects<PolynomialsRationalCoeff>::ListBasicObjectsActualSizeIncrement=1;
template < > int HashedListBasicObjects<root>::PreferredHashSize=1000;
template < > int ListBasicObjects<root>::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<QuasiPolynomial>::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<PolynomialRationalCoeff>::ListBasicObjectsActualSizeIncrement=1;
template < > int HashedListBasicObjects<Monomial<LargeInt> >::PreferredHashSize=1;
template < > int ListBasicObjects<Monomial<LargeInt> >::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Polynomial<Rational> >::ListBasicObjectsActualSizeIncrement=1;
template < > int HashedListBasicObjects<Monomial<Integer> >::PreferredHashSize=25;
template < > int ListBasicObjects<Monomial<Integer> >::ListBasicObjectsActualSizeIncrement=10;
template < > int HashedListBasicObjects<Monomial<LargeRational> >::PreferredHashSize=10000;
template < > int ListBasicObjects<Monomial<LargeRational> >::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<PolynomialLargeRational>::ListBasicObjectsActualSizeIncrement=32;
template < > int ListBasicObjects<rootWithMultiplicity>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<std::string>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<unsigned int>::ListBasicObjectsActualSizeIncrement=1;
ListBasicObjects<std::string> RandomCodeIDontWantToDelete::EvilList1;
ListBasicObjects<std::string> RandomCodeIDontWantToDelete::EvilList2;
bool RandomCodeIDontWantToDelete::UsingEvilList1=true;
std::string RandomCodeIDontWantToDelete::theFirstEvilString;
std::string RandomCodeIDontWantToDelete::theSecondEvilString;

std::fstream partFraction::TheBigDump;
std::fstream partFractions::ComputedContributionsList;

RootToIndexTable partFraction::RootsToIndices;

//template < > int ListObjectPointers<partFraction>::MemoryAllocationIncrement=100;
ListObjectPointers<partFraction> partFraction::GlobalCollectorPartFraction;
bool partFraction::UseGlobalCollector=true;
bool partFraction::AnErrorHasOccurredTimeToPanic=false;
bool partFractions::SplitTestMode=false;
bool partFractions::AnErrorHasOccurredTimeToPanic=false;
bool partFractions::UsingCheckSum=true;
bool QuasiPolynomial::AnErrorHasOccurredTimeToPanic=false;
bool LargeRational::AnErrorHasOccurredTimeToPanic=false;
bool partFractions::MakingProgressReport=true;

int NextDirectionIndex;
int RankGlobal;
roots InputRoots;
CombinatorialChamberPointers TheBigOutput;
FacetPointers TheBigFacetOutput;
DrawingVariables TDV(140,400);
bool QuasiNumber::AnErrorHasOccurredTimeToPanic=false;
bool IntegerPoly::AnErrorHasOccurredTimeToPanic=false;
root oneFracWithMultiplicitiesAndElongations::CheckSumRoot;
bool ParallelComputing::SafePointReached=false;
bool ParallelComputing::ReachSafePointASAP=false;
#ifndef WIN32
pthread_mutex_t ParallelComputing::mutex1;
pthread_cond_t ParallelComputing::continueCondition;
#endif

bool Stop()
{	return true;
}

void AttemptMemoryCompactification()
{
/*	HANDLE HeapHandles[CombinatorialChamberPointers::MaxNumHeaps];
	int NumUsedHeaps= GetProcessHeaps(CombinatorialChamberPointers::MaxNumHeaps,HeapHandles);
	if(NumUsedHeaps>CombinatorialChamberPointers::MaxNumHeaps)
	{
		Stop();
	}
	for (int i=0;i<NumUsedHeaps;i++)
	{
		HeapCompact(HeapHandles[i],0);
	}
*/
}

void PolynomialMemoryLeakTest()
{ PolynomialRationalCoeff tempP, Accum;
	tempP.MakeNVarDegOnePoly(5,0,ROne,ROne);
	Accum.AddPolynomial(tempP);
	Accum.MultiplyBy(tempP);
	Accum.MultiplyBy(Accum);
	tempP.MakeNVarDegOnePoly(5,1,ROne,RZero);
	Accum.AddPolynomial(tempP);
	Accum.MultiplyBy(tempP);
	Accum.MultiplyBy(Accum);
	tempP.MakeNVarDegOnePoly(5,2,ROne,RZero);
	Accum.AddPolynomial(tempP);
	Accum.MultiplyBy(tempP);
	Accum.MultiplyBy(Accum);
	tempP.MakeNVarDegOnePoly(5,3,ROne,RZero);
	Accum.AddPolynomial(tempP);
	Accum.MultiplyBy(tempP);
	Accum.MultiplyBy(Accum);
	tempP.MakeNVarDegOnePoly(5,4,ROne,RZero);
	Accum.AddPolynomial(tempP);
	Accum.MultiplyBy(tempP);
	Accum.MultiplyBy(Accum);
	Accum.ComputeDebugString();
}

void OneSlice(roots& directions, int& index, int rank,
							CombinatorialChamberPointers& output, FacetPointers& FacetOutput)
{	if (index==rank-1)
	{	output.MakeStartingChambers(directions,FacetOutput);
		index++;
		output.ComputeNextIndexToSlice(directions.TheObjects[index]);
/*		SimplexMaxDimension* StartingSimplex;
		CombinatorialChamber* StartingChamber;
		StartingSimplex=MakeSimplexFromRoots(directions,rank, TempStrictSelection);
		StartingChamber = new CombinatorialChamber;
		output.AddObject(StartingChamber);
		StartingChamber->Explored=false;
		for (int i=0;i<rank;i++)
		{
			Facet*NewFacet= new Facet;
			CopyRoot(StartingSimplex->normalVectors->TheObjects[i],NewFacet->normal);
			StartingChamber->ExternalWalls->AddObject(NewFacet);
			NewFacet->PlusOwner=StartingChamber;
			NewFacet->MinusOwner=0;
		}
		delete StartingSimplex;
		StartingChamber->ComputeEdgesAndVerticesFromNormals(true,true);
		StartingChamber->ThePolynomial->MakeNVarConst(root::AmbientDimension, ROne);
		index++;
		output.ComputeNextIndexToSlice(directions.TheObjects[index]);*/
	}
	else
	{	if (index<directions.size)
		{	if(output.NextChamberToSlice!=0)
			{//	if (CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic)
				//{ Stop();
				//}
				if (output.NextChamberToSlice->SliceInDirection(directions.TheObjects[index],directions,index,output,FacetOutput))
				{	delete output.NextChamberToSlice;
				}
			}
			else
			{	output.PreferredNextChambers.size=0;
//				assert(output.PreferredNextChambers.size==0);
				index++;
				output.LabelAllUnexplored();
			}
			if (index<directions.size)
			{	output.ComputeNextIndexToSlice(directions.TheObjects[index]);
			}
		}
	}
	output.initThePolys();
	if(		CombinatorialChamberPointers::NumTotalCreatedCombinatorialChambersAtLastDefrag
				<GlobalCollectorChambers.size-CombinatorialChamberPointers::DefragSpacing
		)
	{	CombinatorialChamberPointers::NumTotalCreatedCombinatorialChambersAtLastDefrag=
			GlobalCollectorChambers.size;
		AttemptMemoryCompactification();
	}
}

void SliceOneDirection(roots& directions, int& index, int rank,
											 CombinatorialChamberPointers& output,
											 FacetPointers& FacetOutput)
{	if (index<directions.size)
	{	int oldindex= index;
//		int counter=0;
		while(oldindex==index)
		{ //counter++;
//			if (counter==8)
//			{CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic=true;
//			}
			OneSlice(directions,index,rank,output,FacetOutput);
		}
	}
}

void SliceTheEuclideanSpace(roots& directions,int &index,
														int rank, CombinatorialChamberPointers& output,
														FacetPointers& FacetOutput)
{ //	int index=rank-1;
	while(index<directions.size)
	{	SliceOneDirection(directions,index,rank,output,FacetOutput);
	}
	Stop();
}

int TwoToTheNth(int n)
{	int result=1;
	for(int i=0;i<n;i++){result*=2;}
	return result;
}

int KToTheNth(int k, int n)
{	int result=1;
	for(int i=0;i<n;i++){result*=k;}
	return result;
}

int NChooseK(int n, int k)
{	int result=1;
	for (int i =0; i<k;i++)
	{	result*=(n-i);
		result/=(i+1);
	}
	return result;
}

void DrawingVariables::initDrawingVariables(int cX1, int cY1)
{ this->ColorChamberIndicator=RGB(220,220,0);
	this->DeadChamberTextColor= RGB(200,100,100);
	this->ZeroChamberTextColor= RGB(100,100,0);
	this->TextColor= RGB(0,0,0);
	this->DrawStyleDashes= 1;
	this->DrawStyleDashesInvisibles= 1;
	this->DrawChamberIndices= false;
	this->DisplayingChamberCreationNumbers =false;
	this->DrawDashes=true;
	this->DrawStyleInvisibles=2;
	this->DrawStyle= 0;
	this->TextOutStyle= 0;
	this->Selected=-2;
	this->textX=0;
	this->textY=15;
	if(cX1!=-1 && cY1!=-1)
	{	this->centerX= (double) cX1;//(double(X2)-double(X1));
		this->centerY= (double) cY1;
	}
	this->scale= 1.9;
//	Projections[0][0]=(0+50)*tempI;
//	Projections[0][1]=(0-50)*tempI;
//	Projections[1][0]=(50+50)*tempI;
//	Projections[1][1]=(87-50)*tempI;
//	Projections[2][0]=(100+50)*tempI;
//	Projections[2][1]=(0-50)*tempI;
	Projections[0][0]=100;
	Projections[0][1]=0;
  Projections[1][0]=0;
	Projections[1][1]=100;
	Projections[2][0]=88;
	Projections[2][1]=49;
	Projections[3][0]=92;
	Projections[3][1]=-29;
	Projections[4][0]=88;
	Projections[4][1]=-49;
//	Projections[5][0]=50;
//	Projections[5][1]=-88;
//	Projections[6][0]=28;
//	Projections[6][1]=-92;
	for (int i =0; i<root::AmbientDimension;i++)
	{	Projections[i][0]*=scale;
		Projections[i][1]*=scale;
	}
	int i=0;
	this->ColorsR[i]=0;
	this->ColorsG[i]=0;
	this->ColorsB[i]=0;
	i++;
////////////////////
	this->ColorsR[i]=50;
	this->ColorsG[i]=50;
	this->ColorsB[i]=200;
	i++;
////////////////////
	this->ColorsR[i]=200;
	this->ColorsG[i]=50;
	this->ColorsB[i]=200;
	i++;
////////////////////
	this->ColorsR[i]=50;
	this->ColorsG[i]=200;
	this->ColorsB[i]=50;
	i++;
////////////////////
	this->ColorsR[i]=50;
	this->ColorsG[i]=200;
	this->ColorsB[i]=200;
	i++;
////////////////////
	this->ColorsR[i]=200;
	this->ColorsG[i]=50;
	this->ColorsB[i]=50;
	i++;
////////////////////
	this->ColorsR[i]=200;
	this->ColorsG[i]=0;
	this->ColorsB[i]=200;
	i++;
////////////////////
	this->ColorsR[i]=0;
	this->ColorsG[i]=200;
	this->ColorsB[i]=200;
	i++;
	for (int i=0;i<DrawingVariables::NumColors;i++)
	{ this->Colors[i]=RGB(this->ColorsR[i],this->ColorsG[i],this->ColorsB[i]);
	}
////////////////////
}

ComputationSetup::ComputationSetup()
{ this->AllowRepaint=true;
	this->UsingCustomVectors=false;
	this->ComputingPartialFractions=true;
	this->ComputingVectorPartitions=true;
	this->ComputingChambers=true;
	this->ComputationInProgress=false;
	this->MakingCheckSumPFsplit=false;
	this->havingBeginEqnForLaTeXinStrings=true;
	this->WeylGroupLetter='A';
	this->WeylGroupIndex=3;
	this->RankEuclideanSpaceGraphics=3;
}

void ComputationSetup::Run()
{ PolyFormatLocal.MakeRegularAlphabet();
	::IndicatorWindowGlobalVariables.Nullify();
	PolynomialOutputFormat::LatexMaxLineLength=35;
	PolynomialOutputFormat::UsingLatexFormat=true;
	this->AllowRepaint=false;
	partFractions::UsingCheckSum=this->MakingCheckSumPFsplit;
	::initDLL(this->WeylGroupIndex);
	if (this->ComputingPartialFractions)
	{	if (!this->UsingCustomVectors)
		{	this->thePartialFraction.ComputeKostantFunctionFromWeylGroup
				(	this->WeylGroupLetter,this->WeylGroupIndex,this->theOutput,
					&this->IndicatorRoot,false,false);
			this->theOutput.ComputeDebugString();
		}
		else
		{ intRoots tempRoots;
			tempRoots.AssignRoots(this->VPVectors);
			this->thePartialFraction.initFromRootSystem(tempRoots,tempRoots,0);
			this->thePartialFraction.IndicatorRoot.Assign(this->IndicatorRoot);
			this->thePartialFraction.split();
			this->thePartialFraction.partFractionsToPartitionFunctionAdaptedToRoot
				(this->theOutput,this->thePartialFraction.IndicatorRoot,false,false,false,false);
			this->theOutput.ComputeDebugString();
		}
		if (this->havingBeginEqnForLaTeXinStrings)
		{ std::stringstream out;
			out <<"\\begin{eqnarray*}&&"<<this->theOutput.DebugString<< "\\end{eqnarray*}";
			this->theOutput.DebugString= out.str();
		}
	}

	if (this->ComputingChambers)
	{	::InputRoots.CopyFromBase(this->VPVectors);
		::NextDirectionIndex=this->WeylGroupIndex-1;
		this->RankEuclideanSpaceGraphics=this->WeylGroupIndex;
		SliceTheEuclideanSpace(	::InputRoots,::NextDirectionIndex,this->WeylGroupIndex,
													TheBigOutput,TheBigFacetOutput);
	}
	this->AllowRepaint=true;
	this->ComputationInProgress=false;
}

void ComputationSetup::EvaluatePoly()
{ this->theOutput.Evaluate(this->ValueRoot,this->Value);
	this->Value.ElementToString(this->ValueString);
}

void GetCoordsForDrawing(DrawingVariables& TDV, root& r,double& x, double& y)
{	x=TDV.centerX;
	y=TDV.centerY;
	for (int k=0; k<root::AmbientDimension; k++)
	{	x+=(r.coordinates[k].DoubleValue())*(TDV.Projections[k][0]);
		y-=(r.coordinates[k].DoubleValue())*(TDV.Projections[k][1]);
	}
}

void ProjectOnToHyperPlaneGraphics(root& input, root& output, roots& directions);
void drawFacetVertices(DrawingVariables& TDV,
											 roots& r, roots& directions, int ChamberIndex)
{	root tempRoot;
	root Projection1;
	root tempRoot2;
	root Projection2;
	for (int i = 0; i<r.size; i++)
	{ tempRoot.Assign(r.TheObjects[i]);
		ProjectOnToHyperPlaneGraphics(tempRoot, Projection1,directions);
		double tempX, tempY;
		GetCoordsForDrawing(TDV,Projection1,tempX,tempY);
		if (TDV.DrawDashes)
		{	drawline(TDV.centerX,TDV.centerY,tempX,tempY, 1,TDV.Colors[0]);
		}
		for (int j=i+1;j<r.size; j++)
		{
			double tempX2;
			double tempY2;
			double tempX1;
			double tempY1;
			tempRoot2.Assign(r.TheObjects[j]);
			ProjectOnToHyperPlaneGraphics(tempRoot2, Projection2, directions);
			GetCoordsForDrawing(TDV,Projection2,tempX2,tempY2);
			GetCoordsForDrawing(TDV,Projection1,tempX1,tempY1);
			drawline(tempX1,tempY1,tempX2,tempY2, 0,TDV.Colors[ChamberIndex%TDV.NumColors]);
		}
	}
}

void drawFacetVerticesMethod2(DrawingVariables& TDV,
														  roots& r, roots& directions, int ChamberIndex,
															Facet* TheFacet, int DrawingStyle, int DrawingStyleDashes)
{	root tempRoot;
	root Projection1;
	root tempRoot2;
	root Projection2;
	for (int i = 0; i<r.size; i++)
	{	if (TheFacet->IsInFacetNoBoundaries(r.TheObjects[i]))
		{	tempRoot.Assign(r.TheObjects[i]);
			ProjectOnToHyperPlaneGraphics(tempRoot, Projection1,directions);
			double tempX, tempY;
			GetCoordsForDrawing(TDV,Projection1,tempX,tempY);
			if (TDV.DrawDashes)
			{	drawline(TDV.centerX,TDV.centerY,tempX,tempY, DrawingStyleDashes,TDV.Colors[0]);
			}
			for (int j=i+1;j<r.size; j++)
			{ if (TheFacet->IsInFacetNoBoundaries(r.TheObjects[j]))
				{	double tempX2;
					double tempY2;
					double tempX1;
					double tempY1;
					tempRoot2.Assign(r.TheObjects[j]);
					ProjectOnToHyperPlaneGraphics(tempRoot2, Projection2, directions);
					GetCoordsForDrawing(TDV,Projection2,tempX2,tempY2);
					GetCoordsForDrawing(TDV,Projection1,tempX1,tempY1);
					drawline(tempX1,tempY1,tempX2,tempY2, DrawingStyle,TDV.Colors[ChamberIndex%TDV.NumColors]);
				}
			}
		}
	}
}

//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
void drawOutput(DrawingVariables& TDV,
								CombinatorialChamberPointers& output,
								roots& directions, int directionIndex, root& ChamberIndicator)
{ int color=0;
	Rational::MinorRoutinesOnIgnoreErrors=true;
	int NumTrueChambers=0;
	int NumTrueChambers2=0;
	int NumZeroChambers=0;
	for (int j=0; j<output.size;j++)
	{ if (!output.TheObjects[j]->HasZeroPoly())
	  {NumTrueChambers++;}else{NumZeroChambers++;}
	}
	if (CombinatorialChamber::DisplayingGraphics)
	{	for (int j=0; j<output.size;j++)
		{	bool hasZeroPoly= output.TheObjects[j]->HasZeroPoly();
			int DrawingStyle;
			int DrawingStyleDashes;
			if (!hasZeroPoly)
			{	DrawingStyle = TDV.DrawStyle;
				DrawingStyleDashes= TDV.DrawStyleDashes;
				color = TDV.TextColor;
				NumTrueChambers2++;
			}
			else
			{	color= TDV.ZeroChamberTextColor;
				if (TDV.DrawingInvisibles)
				{	DrawingStyle = TDV.DrawStyleInvisibles;
					DrawingStyleDashes= TDV.DrawStyleDashesInvisibles;
				}else
				{	DrawingStyle = 5;
					DrawingStyleDashes= 5;
				}
			}
			if (output.TheObjects[j]->PermanentlyZero){color = TDV.DeadChamberTextColor;}
			if (TDV.DrawChamberIndices)
			{	if ((!hasZeroPoly)||(TDV.DrawingInvisibles))
				{	root tempRoot;
					if (CombinatorialChamber::MethodUsed==1)
						output.TheObjects[j]->ComputeInternalPointMethod1(tempRoot);
					if (CombinatorialChamber::MethodUsed==2)
						output.TheObjects[j]->ComputeInternalPointMethod2(tempRoot);
					root Proj;
					ProjectOnToHyperPlaneGraphics(tempRoot,Proj,directions);
					double tempX, tempY;
					GetCoordsForDrawing(TDV,Proj,tempX,tempY);
					std::stringstream out; std::string tempS;
					if (TDV.DisplayingChamberCreationNumbers)
					{	out<<output.TheObjects[j]->CreationNumber;
					}
					else
					{ out<<NumTrueChambers2;
					}
					tempS=out.str();
					drawtext(tempX-7,tempY-7, tempS.c_str(), tempS.length(),color);
				}
			}
			if(CombinatorialChamber::MethodUsed==1)
			{ for (int i =0;i< output.TheObjects[j]->ExternalWalls->size;i++)
				{	Facet* tempFacet= output.TheObjects[j]->ExternalWalls->TheObjects[i];
					drawFacetVertices(TDV,tempFacet->AllVertices,directions,j);
	//				drawFacetVertices(TDV,tempFacet->AllVertices,
	//					                directions,output.TheObjects[j]->CreationNumber);
				}
			}
			if(CombinatorialChamber::MethodUsed==2)
			{ for (int i =0;i< output.TheObjects[j]->ExternalWalls->size;i++)
				{	Facet* tempFacet= output.TheObjects[j]->ExternalWalls->TheObjects[i];
					drawFacetVerticesMethod2(TDV,output.TheObjects[j]->AllVertices,directions,j,
																	 tempFacet, DrawingStyle,DrawingStyleDashes);
		//			output.TheObjects[j]->ComputeDebugString();
				}
			}
		}
		if (output.size>0)
		{	root tempRootX;
			ProjectOnToHyperPlaneGraphics(ChamberIndicator,tempRootX,directions);
			double tmpX,tmpY;
			GetCoordsForDrawing(TDV,tempRootX,tmpX,tmpY);
			drawline(tmpX-2,tmpY-2,tmpX+2,tmpY+2,TDV.DrawStyle,TDV.ColorChamberIndicator);
			drawline(tmpX-2,tmpY+2,tmpX+2,tmpY-2,TDV.DrawStyle,TDV.ColorChamberIndicator);
			tmpX=(tmpX-TDV.centerX)*1.5+TDV.centerX;
			tmpY=(tmpY-TDV.centerY)*1.5+TDV.centerY;
			drawline(TDV.centerX,TDV.centerY,tmpX,tmpY,TDV.DrawStyle,TDV.ColorChamberIndicator);
			std::string tempS="Indicator";
			drawtext(tmpX,tmpY,tempS.c_str(),tempS.size(),TDV.TextColor);
		}
	}

	Rational::MinorRoutinesOnIgnoreErrors=false;
	std::string tempS;
	std::stringstream out,out1,out3;
	out<<"#Drawn chambers: "<<NumTrueChambers;
	tempS=out.str();
	drawtext(TDV.textX,TDV.textY, tempS.c_str(), tempS.length(),TDV.TextColor);
	out1<<"#Zero chambers: "<< NumZeroChambers;
	tempS=out1.str();
	drawtext(TDV.textX,TDV.textY+15, tempS.c_str(), tempS.length(),TDV.TextColor);
	out3 <<"MaxRationalHeight:"<< Rational::LargestRationalHeight;
	tempS=out3.str();
	drawtext(TDV.textX,TDV.textY+30, tempS.c_str(), tempS.length(),TDV.TextColor);
}

void ProjectOnToHyperPlaneGraphics(root& input, root& output, roots& directions)
{	output.Assign(input);
	root normal;
	root basepoint;
	Rational tempRat2;
	Rational tempRat;
	Rational tempRat3;
	normal.MakeZero();
	for (int i=0;i<root::AmbientDimension;i++)
	{	if (input.coordinates[i].IsGreaterThanOrEqualTo(RZero))
		{RationalPlusRational(normal.coordinates[i], ROne, normal.coordinates[i]);}
		else
		{RationalPlusRational(normal.coordinates[i], RMOne, normal.coordinates[i]);}
	}
	for (int i=0;i<directions.size;i++)
	{
//		RootPlusRoot(normal,directions.TheObjects[i],normal);
	}
	basepoint.MakeZero();
	basepoint.coordinates[0].AssignInteger(1);
	if (RZero.IsGreaterThan (input.coordinates[0]))
	{basepoint.MinusRoot(); }
//////////////////////////////////////////////////
	root::RootScalarEuclideanRoot(output,normal,tempRat2);
	root::RootScalarEuclideanRoot(basepoint,normal,tempRat);
	if (!RationalEqualsRational (tempRat2,RZero))
	{	RationalDivRational(tempRat,tempRat2,tempRat3);
		output.MultiplyByRational(tempRat3);
	}
	else
	{	output.MakeZero();
	}
}

void root::ElementToString(std::string& output)
{	output.clear();
	for(int i=0;i<this->dimension;i++)
	{	std::string tempStr;
		this->coordinates[i].ElementToString(tempStr);
		output.append(tempStr);
		if (i!=this->dimension-1)
		{	output.append(",");
		}
	}
}

void root::MinusRoot()
{	MultiplyByInteger(-1);
}

void root::MultiplyByInteger(int a)
{	for (int i=0;i<this->dimension;i++)
	{	this->coordinates[i].MultiplyByInteger(a);
	}
}

void root::ScaleForMinHeight()
{	int d=0;
	for (int i=0;i<this->dimension;i++)
	{	this->MultiplyByInteger(coordinates[i].den);
		if (coordinates[i].num!=0)
		{	if (d==0)
			{	d=coordinates[i].num*coordinates[i].sign();
			}
			else
			{	d= gcd(d,coordinates[i].num*coordinates[i].sign());
			}
		}
	}
	this->DivByInteger(d);
}

bool root::IsEqualTo(root& right)
{	if (this->dimension!=right.dimension)
		return false;
	for (int i=0;i<this->dimension;i++)
	{ if (!this->coordinates[i].IsEqualTo(right.coordinates[i]))
			return false;
	}
	return true;
}

bool root::IsProportianalTo(root& r)
{	if (this->dimension!=r.dimension)
		return false;
	int IndexFirstNonZero=-1;
	for(int i=0;i<this->dimension;i++)
	{	if (!RationalEqualsRational(coordinates[i],RZero))
		{	IndexFirstNonZero=i;
			break;
		}
	}
	if (IndexFirstNonZero==-1)
	{	return false;
	}
	root tempRoot;
	tempRoot.Assign(*this);
	tempRoot.DivByRational(coordinates[IndexFirstNonZero]);
	tempRoot.MultiplyByRational( r.coordinates [IndexFirstNonZero]);
	return tempRoot.IsEqualTo(r);
}

int root::FindLCMDenominators()
{ int result=1;
	for (int i=0;i<this->dimension;i++)
	{	int tempI=gcd(result,this->coordinates[i].den);
		result= result*(this->coordinates[i].den/tempI);
	}
	return result;
}

void root::ScaleToIntegral()
{ int x= this->FindLCMDenominators();
	this->MultiplyByInteger(x);
}

void root::ScaleToIntegralMinHeight()
{ this->ScaleToIntegral();
	this->ScaleForMinHeight();
}

void root::ScaleToFirstNonZeroCoordinatePositive()
{ for (int i=0;i<this->dimension;i++)
	{ if (this->coordinates[i].IsPositive())
		{ return;
		}
		if (this->coordinates[i].IsNegative())
		{ this->MultiplyByInteger(-1);
			return;
		}
	}
}

void root::ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive()
{ this->ScaleToIntegralMinHeight();
	this->ScaleToFirstNonZeroCoordinatePositive();
}

void root::DivByRational(Rational& a)
{	for (int i=0;i<this->dimension;i++)
	{ this->coordinates[i].DivideBy(a);
	}
}

void root::MultiplyByRational(Rational& a)
{	for (int i=0;i<this->dimension;i++)
	{	RationalTimesRational(coordinates[i],a,coordinates[i]);
	}
}

void root::Add(root&r)
{	assert(r.dimension==this->dimension);
	for (int i=0;i<this->dimension;i++)
	{ this->coordinates[i].Add(r.coordinates[i]);
	}
}

void root::Subtract(root&r)
{ assert(r.dimension==this->dimension);
	for (int i=0;i<this->dimension;i++)
	{ this->coordinates[i].Subtract(r.coordinates[i]);
	}
}

void root::MakeZero()
{	this->dimension=root::AmbientDimension;
	for (int i=0;i<this->dimension;i++)
	{	this->coordinates[i].MakeZero();
	}
}

void root::Assign(const root& right)
{	this->dimension=right.dimension;
	for (int i=0;i<this->dimension;i++)
	{ this->coordinates[i].Assign(right.coordinates[i]);
	}
}

void root::AssignIntRoot(intRoot& r)
{	this->dimension=r.dimension;
	for (int i=0;i<r.dimension;i++)
	{ this->coordinates[i].AssignInteger(r.elements[i]);
	}
}

void root::InitFromIntegers(int x1,int x2, int x3,int x4, int x5)
{ this->dimension=root::AmbientDimension;
	this->coordinates[0].init(x1,1);
	this->coordinates[1].init(x2,1);
	this->coordinates[2].init(x3,1);
	this->coordinates[3].init(x4,1);
	this->coordinates[4].init(x5,1);
}

void root::InitFromIntegers(int x1,int x2, int x3,int x4, int x5,int x6, int x7, int x8)
{ this->dimension=root::AmbientDimension;
	this->coordinates[0].init(x1,1);
	this->coordinates[1].init(x2,1);
	this->coordinates[2].init(x3,1);
	this->coordinates[3].init(x4,1);
	this->coordinates[4].init(x5,1);
	this->coordinates[5].init(x6,1);
	this->coordinates[6].init(x7,1);
	this->coordinates[7].init(x8,1);
}

void root::InitFromIntegers(int x1,int x2, int x3,int x4, int x5,int x6, int x7, int x8, int x9, int x10, int x11, int x12)
{ this->dimension=root::AmbientDimension;
	this->coordinates[0].init(x1,1);
	this->coordinates[1].init(x2,1);
	this->coordinates[2].init(x3,1);
	this->coordinates[3].init(x4,1);
	this->coordinates[4].init(x5,1);
	this->coordinates[5].init(x6,1);
	this->coordinates[6].init(x7,1);
	this->coordinates[7].init(x8,1);
	this->coordinates[8].init(x9,1);
	this->coordinates[9].init(x10,1);
	this->coordinates[10].init(x11,1);
	this->coordinates[11].init(x12,1);
}

int root::HashFunction()
{ int result=0;
	for (int i=0;i<this->dimension;i++)
	{ result+=this->coordinates[i].num*this->coordinates[i].den*::SomeRandomPrimes[i];
	}
	return result;
}

inline bool root::IsPositiveOrZero()
{ for (int i=0;i<this->dimension;i++)
	{ if (RZero.IsGreaterThan(this->coordinates[i]))
			return false;
	}
	return true;
}

inline bool root::IsGreaterThanOrEqualTo(root& r)
{ root tempRoot;
	tempRoot.Assign(*this);
	tempRoot.Subtract(r);
	return tempRoot.IsPositiveOrZero();
}

void root::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void root::DivByInteger(int a)
{	for (int i =0; i<this->dimension;i++)
	{ this->coordinates[i].DivideByInteger(a);
	}
}

MatrixRational::~MatrixRational()
{ Free();
}

int MatrixRational::FindLCMCoefficientDenominators()
{ int result=1;
	for (int i=0;i<this->NumRows;i++)
	{ for (int j=0;j<this->NumCols;j++)
		{ result*=(this->elements[i][j].den/gcd(result,this->elements[i][j].den));
		}
	}
	return result;
}

int MatrixRational::FindGCDCoefficientNumerators()
{ int result=1;
	for (int i=0;i<this->NumRows;i++)
	{ for (int j=0;j<this->NumCols;j++)
		{ if (this->elements[i][j].num!=0)
				result=gcd(result,this->elements[i][j].num);
		}
	}
	assert(result!=0);
	return result;
}

void MatrixRational::ScaleToIntegralForMinRationalHeight()
{ Rational tempRat;
	tempRat.init(this->FindLCMCoefficientDenominators(),this->FindGCDCoefficientNumerators());
	this->MultiplyByRational(tempRat);
}

void MatrixRational::ComputeDeterminantOverwriteMatrix(Rational &output)
{ int tempI;
	output.Assign(ROne);
	Rational tempRat;
	assert(this->NumCols==this->NumRows);
	int dim =this->NumCols;
	for (int i=0; i<dim; i++)
	{	//this->ComputeDebugString();
		tempI = this->FindPivot(i,i,dim - 1);
		if (tempI==-1)
		{ output.MakeZero();
			return;
		}
		this->SwitchTwoRows(i, tempI);
		if(tempI!=i){output.MinusRational();}
		tempRat.Assign(this->elements[i][i]);
		output.MultiplyBy(tempRat);
		tempRat.Invert();
		this->RowTimesScalar(i, tempRat);
		//this->ComputeDebugString();
		for (int j = i+1; j<dim; j++)
		{	if (this->elements[j][i].num!= 0)
			{ tempRat.Assign(this->elements[j][i]);
				tempRat.MinusRational();
				this->AddTwoRows (i, j, i, tempRat);
			}
		}
		//this->ComputeDebugString();
	}
}

void MatrixRational::Free()
{	for (int i = 0; i<NumRows; i++)
	{	delete [] elements[i];
	}
	delete [] elements;
	elements=0;
}

void MatrixRational::Transpose()
{ static MatrixRational tempMat;
	tempMat.init(this->NumCols,this->NumRows);
	for (int i=0;i<this->NumRows;i++)
	{	for (int j=0;j<this->NumCols;j++)
		{	tempMat.elements[j][i].Assign(this->elements[i][j]);
		}
	}
	this->Assign(tempMat);
}

MatrixRational::MatrixRational()
{	NumCols=0;
	NumRows=0;
	elements=0;
}

void MatrixRational::MultiplyByInt(int x)
{ for (int i=0;i<this->NumCols;i++)
		for (int j=0;j<this->NumRows;j++)
			this->elements[j][i].MultiplyByInteger(x);
}

void MatrixRational::MultiplyByRational(Rational& x)
{ for (int i=0;i<this->NumCols;i++)
		for (int j=0;j<this->NumRows;j++)
			this->elements[j][i].MultiplyBy(x);
}

void MatrixRational::AssignMatrixIntWithDen(MatrixInt &theMat, int Den)
{ this->init(theMat.NumRows,theMat.NumCols);
	for (int i=0;i<this->NumRows;i++)
	{ for (int j=0;j<this->NumCols;j++)
		{ this->elements[i][j].init(theMat.elements[i][j],Den);
		}
	}
}

void MatrixRational::NullifyAll()
{ for(int i=0;i<this->NumRows; i++)
		for(int j=0;j<this->NumCols;j++)
			this->elements[i][j].MakeZero();
}

void MatrixRational::NonPivotPointsToRoot(Selection& TheNonPivotPoints, root& output)
{	int RowCounter=0;
	output.dimension=root::AmbientDimension;
	for (int i=0;i<root::AmbientDimension;i++)
	{	if (!TheNonPivotPoints.selected[i])
		{	output.coordinates[i].MakeZero();
			for (int j=0;j<TheNonPivotPoints.CardinalitySelection;j++ )
			{	RationalMinusRational(output.coordinates[i],
															this->elements[RowCounter][TheNonPivotPoints.elements[j]],
															output.coordinates[i]);
			}
			RowCounter++;
		}
		else
		{	output.coordinates[i].AssignInteger(1);
		}
	}
}

void MatrixRational::NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, MatrixRational& output)
{	int RowCounter=0;
	output.init(this->NumCols,1);
	//output.ComputeDebugString();
	for (int i=0;i<this->NumCols;i++)
	{	if (!TheNonPivotPoints.selected[i])
		{	output.elements[i][0].MakeZero();
			for (int j=0;j<TheNonPivotPoints.CardinalitySelection;j++ )
			{	output.elements[i][0].Subtract(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
			}
			RowCounter++;
			//output.ComputeDebugString();
		}
		else
		{	output.elements[i][0].Assign(ROne);
		}
		//output.ComputeDebugString();
	}
}

void MatrixRational::Assign(MatrixRational& m)
{ this->init(m.NumRows, m.NumCols);
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			this->elements[i][j].Assign(m.elements[i][j]);
}

void MatrixRational::ComputeDebugString()
{ this->ElementToSting(this->DebugString);
}

void MatrixRational::ElementToSting(std::string& output)
{ std::stringstream out;
	std::string tempS;
	for (int i=0;i<this->NumRows;i++)
	{ for (int j=0;j<this->NumCols;j++)
		{ this->elements[i][j].ElementToString(tempS);
			out<< tempS<<",";
		}
		out <<"\n";
	}
	output= out.str();
}


bool MatrixRational::Invert()
{ assert(this->NumCols==this->NumRows);
	static MatrixRational tempMatrix;
	static Selection NonPivotPts;
	tempMatrix.init(this->NumRows, this->NumCols);
	tempMatrix.NullifyAll();
	for (int i=0;i<this->NumCols;i++)
	{ tempMatrix.elements[i][i].Assign(ROne);
	}
	GaussianEliminationByRows(*this,tempMatrix,NonPivotPts);
	if(NonPivotPts.CardinalitySelection!=0)
	{	return false;
	}
	else
	{	this->Assign(tempMatrix);
		return true;
	}
}

void MatrixRational::init(short r, short c)
{	if (NumRows!=r || NumCols!=c)
	{	this->Free();
		NumRows=r;
		NumCols=c;
		if (NumRows>0)
		{	elements= new Rational*[NumRows];
		}
		for (int i = 0; i<NumRows; i++)
		{	elements[i]= new Rational[NumCols];
		}
	}
}

void initDLL(int rank)
{ TheBigOutput.PreferredNextChambers.size=0;
	TheBigOutput.NextChamberToSliceIndexInPreferredNextChambers=-1;
	TheBigOutput.NextChamberToSlice=0;
	TheBigOutput.LastComputedChamber=0;
	TheBigOutput.size=0;
	TheBigFacetOutput.size=0;
	GlobalCollectorChambers.KillAllElements();
	GlobalCollectorFacets.KillAllElements();

	root::AmbientDimension =(unsigned char)rank;
	CQNOne.MakeConst(ROne,root::AmbientDimension);
	CQNZero.MakeConst(RZero,root::AmbientDimension);
	CQNMOne.MakeConst(RMOne,root::AmbientDimension);

	QNZero.MakeConst( RZero,root::AmbientDimension);
	QNOne.MakeConst ( ROne, root::AmbientDimension);
	QNMOne.MakeConst( RMOne,root::AmbientDimension);

	ZeroRoot.MakeZero();

	CombinatorialChamber::PrintWallDetails= false;
	ListBasicObjects<CombinatorialChamber*>::ListBasicObjectsActualSizeIncrement=1000;
	CombinatorialChamberPointers::DefragSpacing=500;
	CombinatorialChamberPointers::NumTotalCreatedCombinatorialChambersAtLastDefrag=0;
	CombinatorialChamberPointers::LastReportedMemoryUse=0;

	NextDirectionIndex=rank-1;
	Rational::MinorRoutinesOnIgnoreErrors=false;
	CombinatorialChamber::MethodUsed=2;
	CombinatorialChamberPointers::TheBigDump.open
		("C:/BigDump.txt",	std::fstream::in | std::fstream::out);
	//NonPivotPoints.init(rank);
	Rational::LargestRationalHeight=0;
	RankGlobal=rank;
	//matrices memory allocation
	//RMinus1ByR.init(root::AmbientDimension-1,root::AmbientDimension);
	//matOutputEmpty.init(-1,-1);

	QuasiPolynomial::PrecomputedTaus = &PrecomputedTausLocal;
	PrecomputedQuasiPolynomialIntegrals::PreComputedBernoulli = &PreComputedBernoulliLocal;
	//initIntegersFastAccessMemoryAllocation(TempNonPivotPoints,rank);
	TDV.initDrawingVariables(-1,-1);
}

void exitDLL()
{
	GlobalCollectorChambers.KillAllElements();
	GlobalCollectorFacets.KillAllElements();
	QuasiPolynomial::PrecomputedTaus=0;
	CombinatorialChamberPointers::TheBigDump.close();
}

void MatrixRational::RowTimesScalar(int rowIndex, Rational& scalar)
{	for (int i=0; i<this->NumCols; i++)
	{ this->elements[rowIndex][i].MultiplyBy(scalar);
	}
}

void SimplifyRational(Rational& r)
{
	r.Simplify();
}

void RationalMinusRational(Rational& a, Rational& b, Rational& result)
{	Rational temp;
	temp.Assign(b);
	temp.num*=-1;
	RationalPlusRational (a, temp, result);
}

void RationalPlusRational(Rational& a, Rational& b, Rational& output)
{
	int temp, temp2;
	temp = a.num * b.den  + b.num * a.den;
	temp2 = a.den * b.den;
	output.num=temp;
	output.den = temp2;
  SimplifyRational(output);
}

void RationalTimesRational( Rational& a, Rational& b, Rational& result)
{
  result.den = a.den * b.den;
  result.num = a.num * b.num;
  SimplifyRational(result);
}

int RationalToInteger (Rational& r)
{
	return r.num /r.den;
}

void RationalDivRational(Rational& a, Rational& b, Rational& result)
{
	int aden, bden, anum, bnum;
  aden = a.den;
  bden = b.den;
  anum = a.num;
  bnum = b.num;
	result.den = aden * bnum;
	result.num = anum * bden;
  SimplifyRational(result);
}

int lcm(int a, int b)
{ if (a<0){a=-a;}
	if (b<0){b=-b;}
	return ((a*b)/gcd(a,b));
}

int gcd(int a, int b)
{	int temp;
	if (a<0){a=-a;}
	if (b<0){b=-b;}
	while(b>0)
	{ temp= a % b;
		a=b;
		b=temp;
	}
  return a;
}

int MatrixRational::FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex )
{	for(int i = RowStartIndex; i<= RowEndIndex; i++)
	{	if (!RationalEqualsRational(RZero,this->elements[i][columnIndex]))
		{	return i;
		}
	}
	return -1;
}

void MatrixRational::AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, Rational& scalar)
{	Rational tempRat;
	for (int i = StartColIndex; i< this->NumCols; i++)
	{	RationalTimesRational (this->elements[fromRowIndex][i], scalar, tempRat);
		RationalPlusRational (tempRat, this->elements[ToRowIndex][i], this->elements[ToRowIndex][i]);
	}
}

void MatrixRational::SwitchTwoRows( int row1, int row2)
{	Rational tempRat;
	for(int i = 0; i<this->NumCols; i++)
	{	tempRat.Assign(this->elements[row1][i]);
		this->elements[row1][i].Assign(this->elements[row2][i]);
		this->elements[row2][i].Assign(tempRat);
	}
}

void GaussianEliminationByRows(MatrixRational& mat, MatrixRational& output, Selection& outputNonPivotPoints)
{	int tempI;
	int NumFoundPivots = 0;
	int MaxRankMat = Minimum(mat.NumRows, mat.NumCols);
	Rational tempRat;
	outputNonPivotPoints.init(mat.NumCols);
	for (int i=0; i<mat.NumCols; i++)
	{	if (NumFoundPivots == MaxRankMat)
		{	for (int j =i; j<mat.NumCols; j++)
			{	AddToIntegersFastAccess(outputNonPivotPoints,j);
			}
			return;
		}
		tempI = mat.FindPivot(i, NumFoundPivots, mat.NumRows - 1);
		if (tempI!=-1)
		{	if (tempI!=NumFoundPivots)
			{	mat.SwitchTwoRows(NumFoundPivots, tempI);
				output.SwitchTwoRows (NumFoundPivots, tempI);
			}
			tempRat.Assign(mat.elements[NumFoundPivots][i]);
			tempRat.Invert();
			mat.RowTimesScalar(NumFoundPivots, tempRat);
			output.RowTimesScalar(NumFoundPivots, tempRat);
			for (int j = 0; j<mat.NumRows; j++)
			{	if (j!=NumFoundPivots)
				{	if (mat.elements[j][i].num!= 0)
					{ tempRat.Assign(mat.elements[j][i]);
						tempRat.MinusRational();
						mat.AddTwoRows (NumFoundPivots, j, i, tempRat);
	          output.AddTwoRows (NumFoundPivots, j, 0, tempRat);
	          //mat.ComputeDebugString();
					}
				}
			}
			NumFoundPivots++;
		}
		else
		{	AddToIntegersFastAccess(outputNonPivotPoints,i);
		}
	}
}

void ProjectOntoHyperPlane(root& input, root& normal, root& ProjectionDirection, root&output)
{
	Rational t;
	Rational tempRat;
	root::RootScalarEuclideanRoot(input,normal,t);
	root::RootScalarEuclideanRoot(ProjectionDirection,normal, tempRat);
	t.DivideBy(tempRat);
	assert(!RZero.IsGreaterThan(t));
	t.MinusRational();
	root::RootPlusRootTimesScalar(input,ProjectionDirection,t,output);
}

bool RationalGreaterThanRational(Rational& a, Rational& b)
{	if ((a.num==0) && (b.num==0)) return false;
	if ((a.num==0) && (b.num>0)) return false;
	if ((a.num==0) && (b.num<0)) return true;
	if (b.num==0)
	{	return (a.num>0);
	}
	if (a.num>0 && b.num<0) return true;
	if (a.num<0 && b.num>0) return false;
	if (a.num>=0 && b.num>=0)
	{	return (a.num*b.den> a.den*b.num);
	}
	if (a.num<0 && b.num<0)
	{	return (a.num*b.den< a.den*b.num);
	}
	assert(false);
	return false;
}

inline bool RationalEqualsRational (Rational& r1, Rational& r2)
{	return r1.IsEqualTo (r2);
}

inline double RealValue(Rational& r)
{	return r.num / r.den;
}

void initIntegersFastAccessMemoryAllocation(Selection& x, int s)
{	x.init(s);
}

void Selection::init(int maxNumElements)
{	if (maxNumElements>0 && maxNumElements!=this->MaxSize)
	{	delete [] selected;
		delete[] elements;
		//delete [] elementsInverseSelection;
		this->selected = new bool[maxNumElements];
		this->elements = new int[maxNumElements];
	//	this->elementsInverseSelection= new int[maxNumElements];
		MaxSize =maxNumElements;
	}
	for (int i=0; i<this->MaxSize;i++)
	{ this->selected[i]=false;
	//	this->elementsInverseSelection[i] =i;
	}
	this->CardinalitySelection=0;
}

void roots::SubSelection(Selection& theSelection, roots& output)
{ assert(&output!=this);
	output.SetSizeExpandOnTopNoObjectInit(theSelection.CardinalitySelection);
	for(int i=0;i<theSelection.CardinalitySelection;i++)
	{	output.TheObjects[i].Assign(this->TheObjects[theSelection.elements[i]]);
	}
}

void roots::SelectionToMatrix(Selection& theSelection, MatrixRational& output)
{	output.init(root::AmbientDimension,(short)theSelection.CardinalitySelection);
	this->SelectionToMatrixAppend(theSelection,output,0);
}

void roots::SelectionToMatrixAppend
				(Selection& theSelection, MatrixRational& output, int StartRowIndex)
{ for(int i=0;i<theSelection.CardinalitySelection;i++)
	{	root& tempRoot=this->TheObjects[theSelection.elements[i]];
		for (int j=0;j<root::AmbientDimension;j++)
		{ output.elements[StartRowIndex+i][j].Assign(tempRoot.coordinates[j]);
		}
	}
}


void roots::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void roots::ElementToString(std::string& output)
{	std::stringstream out;
	std::string tempS;
	out<<"Num roots: "<<this->size<<"\n";
	for (int i=0; i<this->size;i++)
	{	tempS.clear();
		this->TheObjects[i].ElementToString(tempS);
		out<<tempS<<"\n";
	}
	output = out.str();
}

void roots::rootsToMatrix(MatrixRational& output)
{ output.init((short) this->size,root::AmbientDimension);
	for (int i=0;i<this->size;i++)
		for (int j=0;j<root::AmbientDimension;j++)
			output.elements[i][j].Assign(this->TheObjects[i].coordinates[j]);
}


void Selection::AddSelection(int index)
{	if (!(index>=this->MaxSize))
	{	if (!this->selected[index])
		{	this->selected[index]=true;
			this->elements[this->CardinalitySelection]=index;
			this->CardinalitySelection++;
		}
	}
}

Selection::Selection()
{ this->MaxSize=-1;
//	this->elementsInverseSelection=0;
	this->selected=0;
	this->elements=0;
	this->CardinalitySelection=0;
}

Selection::Selection(int m)
{	//this->elementsInverseSelection=0;
	this->selected=0;
	this->elements=0;
	this->CardinalitySelection=0;
	this->init(m);
}

void Selection::RemoveLastSelection()
{ this->selected[this->elements[this->CardinalitySelection-1]]=false;
	this->CardinalitySelection--;
}

Selection::~Selection()
{	delete [] this->selected;
	delete [] this->elements;
//	delete [] elementsInverseSelection;
	this->selected=0;
	this->elements=0;
}

int Selection::SelectionToIndex()
{	int result=0;
	for (int i=0; i<MaxSize;i++)
	{	result*=2;
		if (this->selected[i]){result+=1;}
	}
	return result;
}

void Selection::ShrinkMaxSize()
{
	MaxSize--;
	if (selected[MaxSize])
	{
		CardinalitySelection--;
	}
}

void Selection::ExpandMaxSize()
{
	elements[CardinalitySelection]=MaxSize;
	selected[MaxSize]=true;
	MaxSize++;
	CardinalitySelection++;
}

void Selection::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void Selection::ElementToString(std::string& output)
{ std::stringstream out;
	out<<"Cardinality: "<<this->CardinalitySelection<<"\n";
	for (int i=0;i<this->CardinalitySelection;i++)
	{ out << this->elements[i]<<", ";
	}
	output=out.str();
}

void Selection::incrementSelection()
{
	for (int i=MaxSize-1;i>=0; i--)
	{
		selected[i]=!selected[i];
		if (selected[i])
		{
			ComputeIndicesFromSelection();
			return;
		}
	}
	ComputeIndicesFromSelection();
}

void Selection::incrementSelectionFixedCardinality(int card)
{
	if (card>MaxSize)
	{
		return;
	}
	if (CardinalitySelection!=card)
	{
		initNoMemoryAllocation();
		for (int i=0;i<card;i++)
		{
			this->selected[i]=true;
			this->elements[i]=i;
		}
		CardinalitySelection=card;
		return;
	}
	if(card==MaxSize|| card==0){return;}
	int IndexLastZeroWithOneBefore=-1;
	int NumOnesAfterLastZeroWithOneBefore=0;
	for(int i=MaxSize-1;i>=0;i--)
	{
		if (selected[i])
		{
			if (IndexLastZeroWithOneBefore==-1)
			{
				NumOnesAfterLastZeroWithOneBefore++;
			}
			else
			{
				break;
			}
		}
		else
		{
			IndexLastZeroWithOneBefore=i;
		}
	}
	if (IndexLastZeroWithOneBefore==0){return;}
	for(int i=0;i<NumOnesAfterLastZeroWithOneBefore+1;i++)
	{
		selected[elements[CardinalitySelection-i-1]]=false;
	}
	for(int i=0;i<NumOnesAfterLastZeroWithOneBefore+1;i++)
	{
		selected[i+ IndexLastZeroWithOneBefore]=true;
		elements[CardinalitySelection+i-NumOnesAfterLastZeroWithOneBefore-1]
			= i+ IndexLastZeroWithOneBefore;
	}
}

void Selection::ComputeIndicesFromSelection()
{	CardinalitySelection=0;
	for (int i=0;i<MaxSize;i++)
	{	if (selected[i])
		{	elements[CardinalitySelection]=i;
			CardinalitySelection++;
		}
//		else
//		{	elementsInverseSelection[i-CardinalitySelection]=i;
//		}
	}
}

void AddToIntegersFastAccess(Selection& x,int a)
{
	x.AddSelection(a);
}

void initIntegersFastAccessNoMemoryAllocation(Selection& x)
{	x.initNoMemoryAllocation();
}

void Selection::initNoMemoryAllocation()
{	for (int i = 0; i<this->CardinalitySelection; i++)
	{	this->selected[this->elements[i]]=false;
	}
	CardinalitySelection = 0;
}

void Selection::Assign(Selection& right)
{ if (this==&right)
	{ return;
	}
	if (this->MaxSize!=right.MaxSize)
	{	this->init(right.MaxSize);
	}
	else
	{ this->initNoMemoryAllocation();
	}
	for (int i=0; i<right.CardinalitySelection;i++)
	{ this->elements[i]=right.elements[i];
		this->selected[this->elements[i]]=true;
	}
	this->CardinalitySelection=right.CardinalitySelection;
}

void root::RootPlusRootTimesScalar(root& r1, root& r2, Rational& rat, root& output)
{	static Rational tempRat;
	assert(r1.dimension==r2.dimension);
	output.dimension=r1.dimension;
	for (int i=0;i<r1.dimension;i++)
	{	RationalTimesRational(r2.coordinates[i],rat,tempRat);
		RationalPlusRational(r1.coordinates[i],tempRat,output.coordinates[i]);
	}
}

void root::RootScalarRoot(root& r1, root& r2, MatrixRational& KillingForm, Rational& output)
{	static Rational tempRat;
	assert(r1.dimension==KillingForm.NumRows && r1.dimension==r2.dimension &&
				 r1.dimension==root::AmbientDimension && r1.dimension==KillingForm.NumCols);
	output.MakeZero();
	for (int i=0; i<KillingForm.NumRows; i++)
	{	for(int j =0; j<KillingForm.NumCols; j++)
		{	RationalTimesRational(r1.coordinates[i], KillingForm.elements[i][j],tempRat);
			tempRat.MultiplyBy(r2.coordinates[j]);
			output.Add(tempRat);
		}
	}
}

void root::RootScalarEuclideanRoot(root& r1, root& r2, Rational& output)
{	static Rational tempRat;
	assert(r1.dimension==r2.dimension);
	output.MakeZero();
	for(int i=0;i<r1.dimension;i++)
	{	RationalTimesRational(r1.coordinates[i],r2.coordinates[i],tempRat);
		output.Add(tempRat);
	}
}

inline void roots::Pop(int index)
{ this->PopIndexSwapWithLast(index);
}

inline void roots::AddRoot(root &r)
{	this->AddObjectOnTop(r);
}

inline void roots::AddIntRoot(intRoot &r)
{	this->SetSizeExpandOnTopNoObjectInit(this->size+1);
	this->TheObjects[this->size-1].AssignIntRoot(r);
}

void roots::PerturbVectorToRegular(root& output)
{ root normal;
	while (!this->IsRegular(output,normal))
	{ IndicatorWindowGlobalVariables.rootIsModified=true;
		IndicatorWindowGlobalVariables.PerturbationHasOccurred=true;
		normal.DivByInteger(10);
		output.Add(normal);
	}
	if (IndicatorWindowGlobalVariables.rootIsModified)
	{ output.ScaleToIntegral();
		IndicatorWindowGlobalVariables.modifiedRoot.AssignRoot(output);
		IndicatorWindowGlobalVariables.StatusString="Indicator modified to regular";
		::FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
	}
}

int roots::GetRankOfSpanOfElements()
{ static Selection NonPivotPoints;
	static MatrixRational tempMatrix;
	this->GaussianEliminationForNormalComputation(tempMatrix,NonPivotPoints);
	return (root::AmbientDimension-NonPivotPoints.CardinalitySelection);
}

bool roots::GetLinearDependence(MatrixRational& outputTheLinearCombination)
{ MatrixRational tempMat;
	tempMat.init(root::AmbientDimension,(short)this->size);
	static MatrixRational matOutputEmpty;
	matOutputEmpty.init(-1,-1);
	for(int i=0;i<this->size;i++)
	{	for(int j=0;j<root::AmbientDimension;j++)
		{	tempMat.elements[j][i].Assign(this->TheObjects[i].coordinates[j]);
		}
	}
	//tempMat.ComputeDebugString();
	Selection nonPivotPoints;
	GaussianEliminationByRows(tempMat,matOutputEmpty,nonPivotPoints);
	//tempMat.ComputeDebugString();
	if (nonPivotPoints.CardinalitySelection==0)
		return false;
	tempMat.NonPivotPointsToEigenVector(nonPivotPoints,outputTheLinearCombination);
	//outputTheLinearCombination.ComputeDebugString();
	return true;
}

void roots::GaussianEliminationForNormalComputation(MatrixRational& inputMatrix,
																									 Selection& outputNonPivotPoints)
{	inputMatrix.init((short)this->size,root::AmbientDimension);
	static MatrixRational matOutputEmpty;
	matOutputEmpty.init(-1,-1);
	outputNonPivotPoints.init(root::AmbientDimension);
	for(int i=0;i<this->size;i++)
	{	for(int j=0;j<root::AmbientDimension;j++)
		{	inputMatrix.elements[i][j].Assign(this->TheObjects[i].coordinates[j]);
		}
	}
	GaussianEliminationByRows(inputMatrix,matOutputEmpty,outputNonPivotPoints);
}

void roots::ComputeNormal(root& output)
{	static MatrixRational tempMatrix;
	static Selection NonPivotPoints;
	NonPivotPoints.init(root::AmbientDimension);
	output.dimension=root::AmbientDimension;
	this->GaussianEliminationForNormalComputation(tempMatrix,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) {return;}
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,output);
}

bool roots::IsRegular(root &r)
{ root tempRoot;
	return this->IsRegular(r,tempRoot);
}

bool roots::IsRegular(root& r, root& outputFailingNormal)
{ static Selection WallSelection;
	WallSelection.init(this->size);
	int x= NChooseK(this->size, root::AmbientDimension-1);
	for (int i=0;i<x;i++)
	{ WallSelection.incrementSelectionFixedCardinality(root::AmbientDimension-1);
		root tempRoot;
		if (this->ComputeNormalFromSelection(tempRoot,WallSelection))
		{ Rational tempRat;
			root::RootScalarEuclideanRoot(tempRoot, r, tempRat);
			if (tempRat.IsEqualToZero())
			{ outputFailingNormal.Assign(tempRoot);
				return false;
			}
		}
	}
	return true;
}

bool roots::ComputeNormalExcludingIndex(root& output, int index)
{ static MatrixRational tempMatrix;
	static MatrixRational matOutputEmpty;
	static Selection NonPivotPoints;
	output.dimension=root::AmbientDimension;
	tempMatrix.init((short)this->size-1,root::AmbientDimension);
	matOutputEmpty.init(-1,-1);
	int k=-1;
	for(int i=0;i<this->size;i++)
	{	if (i!=index)
		{	k++;
			for(int j=0;j<root::AmbientDimension;j++)
			{	tempMatrix.elements[k][j].Assign(this->TheObjects[i].coordinates[j]);
			}
		}
	}
	tempMatrix.ComputeDebugString();
	GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,output);
	return true;
}

bool roots::ComputeNormalFromSelection(root& output, Selection& theSelection)
{	static MatrixRational tempMatrix,matOutputEmpty;
	static Selection NonPivotPoints;
	matOutputEmpty.init(-1,-1);
	output.dimension=root::AmbientDimension;
	tempMatrix.init((short)theSelection.CardinalitySelection,root::AmbientDimension);
	for(int i=0;i<theSelection.CardinalitySelection;i++)
	{	for(int j=0;j<root::AmbientDimension;j++)
		{	tempMatrix.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].coordinates[j]);
		}
	}
	GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,output);
	return true;
}

bool roots::ComputeNormalFromSelectionAndExtraRoot(root& output,root& ExtraRoot,
																									 Selection& theSelection)
{	output.dimension=root::AmbientDimension;
	static MatrixRational tempMatrix,matOutputEmpty;
	static Selection NonPivotPoints;
	tempMatrix.init((short)theSelection.CardinalitySelection+1,root::AmbientDimension);
	matOutputEmpty.init(-1,-1);
	for(int j=0;j<root::AmbientDimension;j++)
	{	for(int i=0;i<theSelection.CardinalitySelection;i++)
		{	tempMatrix.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].coordinates[j]);
		}
		tempMatrix.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot.coordinates[j]);
	}
	GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,output);
	return true;
}


bool roots::ComputeNormalFromSelectionAndTwoExtraRoots
			(root& output,root& ExtraRoot1, root& ExtraRoot2, Selection& theSelection)
{	static MatrixRational tempMatrix,matOutputEmpty;
	static Selection NonPivotPoints;
	output.dimension=root::AmbientDimension;
	matOutputEmpty.init(-1,-1);
	tempMatrix.init((short)theSelection.CardinalitySelection+2,root::AmbientDimension);
	for(int j=0;j<root::AmbientDimension;j++)
	{	for(int i=0;i<theSelection.CardinalitySelection;i++)
		{	tempMatrix.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].coordinates[j]);
		}
		tempMatrix.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot1.coordinates[j]);
		tempMatrix.elements[theSelection.CardinalitySelection+1][j].Assign(ExtraRoot2.coordinates[j]);
	}
	GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,output);
	return true;
}

void roots::AssignMatrixRows(MatrixRational& mat)
{ this->size=0;
	root tempRoot;
	assert(mat.NumCols==root::AmbientDimension);
	for (int i=0;i<mat.NumRows;i++)
	{ for (int j=0;j<root::AmbientDimension;j++)
		{ tempRoot.coordinates[j].Assign(mat.elements[i][j]);
		}
		this->AddRoot(tempRoot);
	}
}

void roots::Average(root& output)
{	output.MakeZero();
	if (this->size==0){return;}
	for (int i=0;i<this->size;i++)
	{	output.Add(this->TheObjects[i]);
	}
	output.DivByInteger(this->size);
}

void roots::AddRootSnoRepetition(roots &r)
{
	for (int i=0;i<r.size;i++)
	{
		this->AddRootNoRepetition(r.TheObjects[i]);
	}
}

void roots::AssignIntRoots(intRoots& r)
{ this->size=0;
	root tempRoot;
	for (int i=0;i<r.size;i++)
	{ tempRoot.AssignIntRoot(r.TheObjects[i]);
		this->AddRoot(tempRoot);
	}
}

void roots::AssignHashedIntRoots(HashedListBasicObjects<intRoot>& r)
{ this->size=0;
	root tempRoot;
	for (int i=0;i<r.size;i++)
	{ tempRoot.AssignIntRoot(r.TheObjects[i]);
		this->AddRoot(tempRoot);
	}
}

bool roots::AddRootNoRepetition(root& r)
{	if (this->ContainsObject(r)==-1)
	{	this->AddObjectOnTop(r);
		return true;
	}
	return false;
}

void MinusRoot(root& r)
{
	for(int i=0;i<root::AmbientDimension;i++)
	{
		r.coordinates[i].num*=-1;
	}
}

void RationalToDouble(Rational& r, double& output)
{	double x1,x2;
	x1=double(r.num);
	x2=double(r.den);
	output=x1/x2;
}

inline bool Rational::IsEqualTo(const Rational& b)
{	return(this->num*b.den==b.num*this->den);
}

inline void Rational::MinusRational()
{	this->num*=-1;
}

inline void Rational::DivideBy(Rational& r)
{	RationalDivRational(*this,r,*this);
}

inline void Rational::MultiplyBy(Rational& r)
{	RationalTimesRational(*this,r,*this);
}

inline void Rational::ElementToString(std::string& output)
{	std::stringstream out;
	out <<this->num;
	if (this->den!=1)
	{	out <<"/"<<den;
	}
	if (this->den==0)
	{ Stop();
	}
	output = out.str();
}

inline void Rational::Add(Rational& r)
{	this->num= this->num*r.den+r.num*this->den;
	this->den*= r.den;
	this->Simplify();
}

inline void Rational::AddInteger(int x)
{	this->num+= x*this->den;
	this->Simplify();
}

inline void Rational::WriteToFile(std::fstream& output)
{ output<< this->num<<" / "<<this->den;
}

inline void Rational::ReadFromFile(std::fstream& input)
{ std::string tempS;
	input>> this->num>>tempS>>this->den;
}

inline void Rational::Subtract(Rational& r)
{ int tempDen= this->den*r.den;
	int tempNum= this->num*r.den-this->den*r.num;
	this->num= tempNum;
	this->den = tempDen;
	this->Simplify();
}

inline int Rational::sign()
{	if (this->num>=0)
	{	return 1;
	}
	else
	{	return -1;
	}
}

inline void Rational::MultiplyByInteger(int a)
{	num*=a;
	Simplify();
}

inline void Rational::DivideByInteger(int x)
{	this->den*=x;
	Simplify();
}

inline void Rational::AssignFracValue()
{
	if (this->num==0) return;
	int sign=1;
	if (this->num<0){sign=-1; this->num*=-1;}
	this->num%=this->den;
	if (sign==-1 && this->num!=0)
	{this->num=this->den-this->num;}
}

inline bool Rational::IsEqualToZero()
{ return (this->num==0);
}

inline void Rational::AssignRational(Rational& r)
{ this->Assign(r);
}

inline void Rational::MultiplyByRational(Rational &r)
{ this->MultiplyBy(r);
}

inline void Rational::AssignInteger(int i)
{	this->num=i; this->den=1;
}

inline bool Rational::IsGreaterThanOrEqualTo(Rational& b)
{	if (this->IsEqualTo(b)) return true;
	return RationalGreaterThanRational(*this,b);
}

inline bool Rational::IsGreaterThan(Rational& b)
{	return RationalGreaterThanRational(*this,b);
}

inline void Rational::MakeZero()
{	this->den=1;
	this->num=0;
}

void Rational::Simplify()
{	int temp;
	if (num == 0)
	{	den = 1;
	}
	else
	{	if (den < 0)
		{	den*=-1;
			num*=-1;
		}
		temp = gcd(den, num);
		den = den / temp;
		num = num / temp;
	}
	if (!MinorRoutinesOnIgnoreErrors)
	{	LargestRationalHeight=Maximum(LargestRationalHeight,::Maximum(num,-num));
		LargestRationalHeight=Maximum(LargestRationalHeight,den);
	}
}

inline void Rational::Invert()
{	int tempI=this->den;
	this->den=this->num;
	this->num=tempI;
	this->Simplify();
}

/*bool Rational::IsEqualToElement( ElementOfCommutativeRingWithIdentity& e)
{
	return this->IsEqualTo(*((Rational*)(&e)));
}

void Rational::AssignElement(ElementOfCommutativeRingWithIdentity& e)
{
	this->Assign(*((Rational*)(&e)));
}

void Rational::AddElement(ElementOfCommutativeRingWithIdentity& e)
{
	this->PlusRational(*((Rational*)(&e)));
}

void Rational::MultyplyByElement(ElementOfCommutativeRingWithIdentity& e)
{
	this->MultiplyByRational(*((Rational*)(&e)));
}
*/

inline void Rational::Assign(const Rational& r)
{	this->num= r.num;
	this->den= r.den;
}

inline bool Rational::ElementHasPlusInFront()
{	return (this->num>=0);
}

inline bool Rational::IsPositive()
{ return (this->num>0);
}

inline bool Rational::IsNegative()
{ return (this->num<0);
}

inline double Rational::DoubleValue()
{	double tempd, tempn;
	tempd= this->den;
	tempn= this->num;
	return tempn/tempd;
}

inline void Rational::init(int Num, int Den)
{	this->num=Num; this->den=Den; this->Simplify();
}

void CombinatorialChamber::GetNormalFromFacet(root& output, Facet* theFacet)
{	output.dimension=root::AmbientDimension;
	for (int i=0;i<this->ExternalWalls->size;i++)
	{	if (this->ExternalWalls->TheObjects[i]==theFacet)
		{	output.Assign(this->ExternalWallsNormals.TheObjects[i]);
			return;
		}
	}
}

bool CombinatorialChamber::HasHSignVertex(root & h, int sign)
{ root tempH;
	tempH.Assign(h);
	tempH.MultiplyByInteger(sign);
	for (int i =0; i<ExternalWalls->size;i++)
	{	Facet* tempFacet=ExternalWalls->TheObjects[i];
		if (tempFacet->HasHPositiveVertex(tempH))
		{	return true;
		}
	}
	return false;
}

void CombinatorialChamber::AddInternalWall(Facet *TheKillerFacet, Facet *TheFacetBeingKilled, root &direction)
{	static root tempNormal; tempNormal.dimension=root::AmbientDimension;
	this->GetNormalFromFacet(tempNormal,TheFacetBeingKilled);
	if (TheFacetBeingKilled->IsExternalWithRespectToDirection(direction,this,tempNormal))
	{	this->InternalWalls->AddObjectNoRepetitionOfPointer( TheKillerFacet);
	}
}

bool CombinatorialChamber::FacetIsInternal(Facet* f)
{ for (int i=0;i<this->ExternalWalls->size;i++)
	{	if (f==this->ExternalWalls->TheObjects[i])
		{ return false;
		}
	}
	for(int i=0;i<this->InternalWalls->size;i++)
	{	if (f==this->InternalWalls->TheObjects[i])
		{	return true;
		}
	}
	return false;
}

bool CombinatorialChamber::CheckSplittingPointCandidate(Selection &SelectionTargetSimplex,
																	Selection &SelectionStartSimplex,
																	MatrixRational& outputColumn)
{	for (int i=0; i<SelectionStartSimplex.CardinalitySelection;i++)
	{	if (!RationalGreaterThanRational
					(outputColumn.elements[i][0], RZero)
				)
		{	return false;
		}
	}
	if (!RationalGreaterThanRational
				(outputColumn.elements[root::AmbientDimension-1 ][0], RZero)
			)
	{	return false;
	}
	return true;
}

bool CombinatorialChamber::ComputeDebugString()
{
	std::stringstream out;
	out <<"c"<<this->CreationNumber;
	out <<"\n External Walls:\n";
	root tempNormal;
	std::string tempS;
	for (int i=0;i<this->ExternalWalls->size;i++)
	{	this->ExternalWallsNormals.TheObjects[i].ElementToString(tempS);
		out <<"f"<<i<<": "<<"n"<<this->ExternalWalls->TheObjects[i]->CreationNumber<<": "<< tempS<<"\n";
	}
	out <<"\n Internal Walls:\n";
	for (int i=0;i<this->InternalWalls->size;i++)
	{	this->InternalWalls->TheObjects[i]->normal.ElementToString(tempS);
		out <<"f"<<i<<": "<<"n"<<this->InternalWalls->TheObjects[i]->CreationNumber<<": "<< tempS<<"\n";
	}
	ListObjectPointers<CombinatorialChamber> output;
	this->FindAllNeighbors(output);
	out<<"Neighbors: ";
	for (int i=0;i<output.size;i++)
	{
		out<< "c"<<output.TheObjects[i]->CreationNumber<<", ";
	}
	PolyFormatLocal.cutOffString=false;
	if (this->ComputingPolys)
	{	this->ThePolynomial->ComputeDebugString();
		out<<"\n The Polynomial:"<< this->ThePolynomial->DebugString <<"\n\n";
		this->ThePolynomial->WriteComplexFormToDebugString();
		out<<"\n Complex Form:"<< this->ThePolynomial->DebugString <<"\n\n";
	}
	if (CombinatorialChamber::PrintWallDetails)
	{	out <<"\n Wall Details:\n";
		for (int i=0;i<this->ExternalWalls->size;i++)
		{	this->ExternalWalls->TheObjects[i]->ComputeDebugString(this);
			out<<	this->ExternalWalls->TheObjects[i]->DebugString<<"\n";
		}
		for (int i=0;i<this->InternalWalls->size;i++)
		{	this->InternalWalls->TheObjects[i]->ComputeDebugString(this);
			out << this->InternalWalls->TheObjects[i]->DebugString<<"\n";
		}
	}
	out<<"\n XXX";
	this->DebugString=out.str();
	return true;
}

bool CombinatorialChamber::CheckVertices()
{
	for (int i=0;i<this->ExternalWalls->size;i++ )
	{
		assert (this->ExternalWalls->TheObjects[i]->CheckVertices(this));
	}
	return true;
}

CombinatorialChamber::CombinatorialChamber()
{	this->ThePolynomial=0;
	if (CombinatorialChamber::ComputingPolys){this->ThePolynomial=new QuasiPolynomial;}
	this->ExternalWalls =new FacetPointers;
	this->InternalWalls =new FacetPointers;
	this->Explored=false;
	this->PermanentlyZero=false;
	GlobalCollectorChambers.AddObjectOnTop(this);
	this->CreationNumber=GlobalCollectorChambers.size;
}

CombinatorialChamber::~CombinatorialChamber()
{	Free();
	GlobalCollectorChambers.TheObjects[this->CreationNumber-1]=0;
}

void CombinatorialChamber::Free()
{
	if (CombinatorialChamber::ComputingPolys)
	{delete this->ThePolynomial;}
	this->ThePolynomial =0;
	delete this->ExternalWalls;
	delete this->InternalWalls;
	this->ThePolynomial=0;
	this->ExternalWalls=0;
	this->InternalWalls=0;
}

bool CombinatorialChamber::HasZeroPoly()
{
	if (CombinatorialChamber::ComputingPolys)
	{	return this->ThePolynomial->IsEqualToZero();
	}
	else
	{	return (this->ThePolynomial ==0);
	}
}

void CombinatorialChamber::ComputeInternalPointMethod1(root &InternalPoint)
{
	InternalPoint.MakeZero();
	int totalWeight=0;
	for (int i=0;i<this->ExternalWalls->size;i++)
	{
		Facet* tempFacet=this->ExternalWalls->TheObjects[i];
		root tempRoot;
		tempFacet->ComputeInternalPoint(tempRoot);
		tempRoot.MultiplyByInteger(tempFacet->AllVertices.size);
		totalWeight+=tempFacet->AllVertices.size;
		InternalPoint.Add(tempRoot);
	}
	InternalPoint.DivByInteger(totalWeight);
}

void CombinatorialChamber::ComputeInternalPointMethod2(root &InternalPoint)
{
	InternalPoint.MakeZero();
	for (int i=0;i<this->AllVertices.size;i++)
	{
		InternalPoint.Add(this->AllVertices.TheObjects[i]);
	}
	InternalPoint.DivByInteger(this->AllVertices.size);
}

void CombinatorialChamber::ComputeEdgesAndVerticesFromNormals(bool ComputingVertices, bool ComputingEdges)
{	if (ComputingVertices)
	{	for (int i =0;i<this->ExternalWalls->size;i++)
		{	this->ExternalWalls->TheObjects[i]->AllVertices.size=0;
		}
	}
	Selection theSelection;
	theSelection.init(this->ExternalWalls->size);
	root VertexCandidate;
	int NumCandidates = NChooseK(this->ExternalWalls->size, root::AmbientDimension-1);
	for (int i=0;i<NumCandidates; i++)
	{	theSelection.incrementSelectionFixedCardinality(root::AmbientDimension-1);
		LinearAlgebraForVertexComputation(theSelection,VertexCandidate);
		if (this->PlusMinusPointIsInChamber(VertexCandidate))
		{
			for (int j=0;j<theSelection.CardinalitySelection;j++ )
			{//generate vertices
				if (ComputingVertices)
				{
					this->ScaleVertexToFitCrossSection(VertexCandidate);
					ExternalWalls->TheObjects[theSelection.elements[j]]->AllVertices.AddRootNoRepetition(VertexCandidate);
				}
				if (ComputingEdges)
				{
					Facet* tempFacet=ExternalWalls->TheObjects[theSelection.elements[j]];
					for (int k=0;k<theSelection.CardinalitySelection;k++)
					{//generate edges
						if(k!=j)
						{
							tempFacet->Boundaries.AddRootNoRepetition(this->ExternalWallsNormals.TheObjects[theSelection.elements[k]]);
						}
					}
				}
			}
		}
	}
}

bool CombinatorialChamber::PlusMinusPointIsInChamber(root&point)
{
	if (PointIsInChamber(point))
	{ return true;
	}
	point.MinusRoot();
	if (PointIsInChamber(point))
	{	return true;
	}
	return false;
}

bool CombinatorialChamber::ScaleVertexToFitCrossSection(root& point)
{
	static Rational tempRat;
	root::RootScalarEuclideanRoot(point,
		CombinatorialChamber::StartingCrossSectionNormals
		 .TheObjects[this->IndexStartingCrossSectionNormal],tempRat);
	if (tempRat.IsEqualTo(RZero)){return false;}
	point.DivByRational(tempRat);
	root::RootScalarEuclideanRoot(CombinatorialChamber::StartingCrossSectionNormals
		.TheObjects[this->IndexStartingCrossSectionNormal],
			CombinatorialChamber::StartingCrossSectionAffinePoints
				.TheObjects[this->IndexStartingCrossSectionNormal],tempRat);
	point.MultiplyByRational(tempRat);
	return true;
}

bool CombinatorialChamber::PointIsInChamber(root&point)
{
	for (int i=0;i<this->ExternalWalls->size;i++)
	{	static Rational tempRat;
		root::RootScalarEuclideanRoot(this->ExternalWallsNormals.TheObjects[i],point,tempRat);
		if (RationalGreaterThanRational(RZero,tempRat))
		{	return false;
		}
	}
	return true;
}

bool CombinatorialChamber::PlusMinusPointIsInWallSelection(root &point,
																													 Selection& theSelection)
{
	for (int k=0;k<2;k++)
	{
		bool result=true;
		for (int i=0;i<theSelection.CardinalitySelection;i++)
		{
			Facet* tempFacet= this->ExternalWalls->TheObjects[theSelection.elements[i]];
			if (!tempFacet->IsInFacetWithBoundaries(point))
			{
				result=false;
				break;
			}
		}
		if (result){return true;}
		point.MinusRoot();
	}
	return false;
}

bool CombinatorialChamber::IsABogusNeighbor(Facet* NeighborWall,CombinatorialChamber* Neighbor)
{	if (NeighborWall->FacetContainsChamberOnlyOnce(this))
		return false;
	if (NeighborWall->FacetContainsChamberOnlyOnce(Neighbor))
		return false;
	static roots ExternalWallsConglomerate;
	static roots FoundVertices;
	ExternalWallsConglomerate.size=0;
	for (int i=0;i<this->ExternalWalls->size;i++)
	{	if (this->ExternalWalls->TheObjects[i]!=NeighborWall)
		{ExternalWallsConglomerate.AddRoot(this->ExternalWalls->TheObjects[i]->normal);}
	}
	for (int i=0;i<Neighbor->ExternalWalls->size;i++)
	{	if (Neighbor->ExternalWalls->TheObjects[i]!=NeighborWall)
		{ExternalWallsConglomerate.AddRootNoRepetition(Neighbor->ExternalWalls->TheObjects[i]->normal);}
	}
	Selection theSelection;
	theSelection.init(ExternalWallsConglomerate.size);
	static int NumPossibilities;
	NumPossibilities = NChooseK(ExternalWallsConglomerate.size,root::AmbientDimension-2);
	static root VertexCandidate; VertexCandidate.dimension=root::AmbientDimension;
	static int OldRank;
	OldRank= 0;
	FoundVertices.size=0;
	for (int i=0;i<NumPossibilities;i++)
	{	theSelection.incrementSelectionFixedCardinality(root::AmbientDimension-2);
		if(ExternalWallsConglomerate.ComputeNormalFromSelectionAndExtraRoot(
			VertexCandidate,NeighborWall->normal,theSelection))
		{	if (this->PlusMinusPointIsInChamber(VertexCandidate))
			{	if (Neighbor->PointIsInChamber(VertexCandidate))
				{	if(FoundVertices.AddRootNoRepetition(VertexCandidate))
					{	static int NewRank;
						NewRank= FoundVertices.GetRankOfSpanOfElements();
						if (NewRank>OldRank)
						{OldRank= NewRank;}
						else
						{FoundVertices.size--;}
						if (OldRank==root::AmbientDimension-1)
						{return false;}
					}
				}
			}
		}
	}
	return true;
}

bool CombinatorialChamber::PointIsInWallSelection(root &point, Selection& theSelection)
{
	for (int i=0;i<theSelection.CardinalitySelection;i++)
	{
		Facet* tempFacet= this->ExternalWalls->TheObjects[theSelection.elements[i]];
		if (!tempFacet->IsInFacetWithBoundaries(point))
		{
			return false;
		}
	}
	return true;
}

void CombinatorialChamber::PurgeAllSentencedFacets()
{
	for (int i =0;i<this->ExternalWalls->size; i++)
	{
		Facet* tempFacet=this->ExternalWalls->TheObjects[i];
		if (tempFacet->SentencedToDeath)
		{
			this->ExternalWalls->Pop(tempFacet);
			delete tempFacet;
			i--;
		}
	}
}

bool CombinatorialChamber::LinearAlgebraForVertexComputation
			(Selection& theSelection,  root& output)
{ output.dimension=root::AmbientDimension;
	assert(root::AmbientDimension -1== theSelection.CardinalitySelection);
	static MatrixRational RMinus1ByR,matOutputEmpty;
	RMinus1ByR.init(root::AmbientDimension-1,root::AmbientDimension);
	matOutputEmpty.init(-1,-1);
	static Selection NonPivotPoints;
	for (int i =0;i<root::AmbientDimension-1;i++)
	{	for (int j=0; j<root::AmbientDimension;j++)
		{	RMinus1ByR.elements[i][j].Assign
				(ExternalWalls->TheObjects[theSelection.elements[i]]->normal.coordinates[j]);
		}
	}
	GaussianEliminationByRows(RMinus1ByR,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection==1)
	{	RMinus1ByR.NonPivotPointsToRoot(NonPivotPoints,output);
		return true;
	}
	return false;
}

bool CombinatorialChamber::SliceInDirection(root& direction,roots& directions,
																						int CurrentIndex,CombinatorialChamberPointers& output,
																						FacetPointers& FacetOutput)
{	if (!(this->ExternalWalls->size>=root::AmbientDimension))
	{	this->ComputeDebugString();
		CombinatorialChamberPointers::TheBigDump<<this->DebugString;
	}
	assert(this->ExternalWalls->size>=root::AmbientDimension);
	if (!this->IsAnOwnerOfAllItsWalls())
	{	this->ComputeDebugString();
		CombinatorialChamberPointers::TheBigDump<<this->DebugString;
	}
//	static bool tempB=false;
	assert(this->IsAnOwnerOfAllItsWalls());
	this->Explored=false;
	Facet* LastExternalWithRespectToDirection=0;
	switch(CombinatorialChamber::MethodUsed)
	{
	case 1:/*
		for (int i=0;i<this->ExternalWalls->size;i++)
		{
			Facet* tempFacet= this->ExternalWalls->TheObjects[i];
			if(tempFacet->IsExternalWithRespectToDirection(direction, this))
			{
				LastExternalWithRespectToDirection=tempFacet;
				assert (tempFacet->Boundaries.size>=2);
				for (int j=0; j<tempFacet->Boundaries.size;j++)
				{
					if(this->SlashChamber(j,tempFacet,direction,directions,CurrentIndex,output))
					{
						output.Pop(this);
						GarbageCollectorChambers.AddObject(this);
						return;
					}
				}
			}
		}
		break;*/
	case 2:
		if (InternalWalls->size!=0)
		{	Facet* theKillerFacet= this->InternalWalls->TheObjects[0];
			if (this->SplitChamber(theKillerFacet,output,direction))
			{	output.KillNextChamberToSlice();
			}
			else
			{this->RemoveInternalWall(0);}
			return false;
		}
		else
		{	for (int i=0;i<this->ExternalWalls->size;i++)
			{	Facet* tempFacet1= this->ExternalWalls->TheObjects[i];
				if(tempFacet1->IsExternalWithRespectToDirection(direction, this, this->ExternalWallsNormals.TheObjects[i] ))
				{	LastExternalWithRespectToDirection=tempFacet1;
					for (int j=i+1; j<this->ExternalWalls->size;j++)
					{	Facet* tempFacet2= this->ExternalWalls->TheObjects[j];
						if(tempFacet2->IsExternalWithRespectToDirection(direction, this, this->ExternalWallsNormals.TheObjects[j]))
						{	Facet* theKillerFacet= tempFacet1->MakeFacetFromEdgeAndDirection
								        (this,tempFacet2,direction,directions,CurrentIndex,FacetOutput);
							if (theKillerFacet!=0)
							{	if (this->SplitChamberMethod2(theKillerFacet,output,direction))
								{	output.KillNextChamberToSlice();
									return true;
								}
								else
								{ this->Explored=true;
									return false;
								}

							}
						}
					}
				}
			}
		}
	}
	if(LastExternalWithRespectToDirection!=0)
	{ output.LastComputedChamber= this;
		CombinatorialChamber* TheOtherChamber=
				LastExternalWithRespectToDirection->TheFirstOtherChamber(this);
		if (CombinatorialChamber::ComputingPolys)
		{	static QuasiPolynomial TheOtherChambersPoly;
			root tempNormal;
			this->GetNormalFromFacet(tempNormal,LastExternalWithRespectToDirection);
			if (TheOtherChamber!=0)
			{	TheOtherChambersPoly.CopyFromPoly(*TheOtherChamber->ThePolynomial);
			}
			this->ThePolynomial->IntegrateDiscreteInDirectionFromOldChamber
					(direction,tempNormal,TheOtherChambersPoly, *this->ThePolynomial, PrecomputedQuasiPolys);
		}
		else
		{	if (this->ThePolynomial==0)
			{	this->ThePolynomial= TheOtherChamber->ThePolynomial;
			}
		}
	}
	this->Explored=true;
	if (output.NextChamberToSliceIndexInPreferredNextChambers!=-1)
	{	output.PreferredNextChambers.PopIndexShiftUp(output.NextChamberToSliceIndexInPreferredNextChambers);
		output.NextChamberToSliceIndexInPreferredNextChambers=-1;
		output.NextChamberToSlice=0;
	}
	return false;
}

bool CombinatorialChamber::SlashChamber(int theKillerEdgeIndex,Facet* TheFacet, root& direction,
																				roots& directions, int CurrentIndex,
																				CombinatorialChamberPointers& output)
{
	Facet* newFacet= TheFacet->RayTrace(theKillerEdgeIndex,direction,directions,CurrentIndex,this);
	if (newFacet== 0)
	{
		return false;
	}
	if (this->SplitChamber(newFacet,output, direction))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CombinatorialChamber::FindAllNeighbors(
	     ListObjectPointers<CombinatorialChamber>& TheNeighbors)
{
	for (int i=0;i<this->ExternalWalls->size;i++)
	{
		this->ExternalWalls->TheObjects[i]->FindAllNeighborsTo
			(this,TheNeighbors);
	}
}

bool CombinatorialChamber::IsAnOwnerOfAllItsWalls()
{
	CombinatorialChamber* tempC;
	bool tempB;
	for (int i=0;i<this->ExternalWalls->size;i++)
	{
		if (!this->ExternalWalls->TheObjects[i]->IsAnOwner(this,tempC,tempB))
			return false;
	}
/*	for (int i=0;i<this->InternalWalls->size;i++)
	{
		if (!this->InternalWalls->TheObjects[i]->IsAnOwner(this,tempC,tempB))
			return false;
	}*/
	return true;
}

void CombinatorialChamber::RemoveInternalWall(int index)
{
//	Facet* tempF= this->InternalWalls->TheObjects[index];
//	tempF->Owners.RemoveCouple(this,this);
	this->InternalWalls->PopIndexSwapWithLast(index);
}

bool CombinatorialChamber::TestPossibilityToSlice(root& direction)
{
	if (this->Explored) {return false;}
	if (this->PermanentlyZero)
	{	this->Explored=true;
		return false;
	}
	for (int j=0;j<this->ExternalWalls->size;j++)
	{ if (this->ExternalWalls->TheObjects[j]->
	       IsExternalWithRespectToDirection(direction,this, this->ExternalWallsNormals.TheObjects[j]))
		{	if (this->ExternalWalls->TheObjects[j]->OneOfTheOtherChambersIsNotExplored(this))
			{ return false;
			}
		}
	}
	return true;
}

bool CombinatorialChamber::SplitChamberMethod2(Facet* theKillerFacet,
																							 CombinatorialChamberPointers& output,
																							 root& direction)
{
//	static roots PositiveVertices, NegativeVertices;
	static bool AnErrorHasOcurred;
	static bool PlusChamberIsPermanentZero;
	static bool MinusChamberIsPermanentZero;
	static rootsPointersKillOnExit LocalContainerPlusVertices;
	static rootsPointersKillOnExit LocalContainerMinusVertices;
	static roots LocalLinearAlgebra;
	static CombinatorialChamber* NewPlusChamber;
	static CombinatorialChamber* NewMinusChamber;
	static CombinatorialChamberPointers PossibleBogusNeighbors;
	static FacetPointers PossibleBogusWalls;
	PossibleBogusNeighbors.size=0;
	PossibleBogusWalls.size=0;
	Selection theSelection;
	PlusChamberIsPermanentZero=false;
	AnErrorHasOcurred=false;
	MinusChamberIsPermanentZero=false;
	LocalLinearAlgebra.size=0;
//	if (CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic)
//	{ this->ComputeDebugString();
//	}
	for (int i=0;i<this->ExternalWalls->size;i++)
	{	static bool tempBool;
		tempBool=LocalLinearAlgebra.AddRootNoRepetition(ExternalWalls->TheObjects[i]->normal);
		if (!tempBool){this->ComputeDebugString();}
		assert(tempBool);
	}
 	LocalLinearAlgebra.AddRoot(theKillerFacet->normal);
	theSelection.init(LocalLinearAlgebra.size);
	int NumCandidates =NChooseK(LocalLinearAlgebra.size, root::AmbientDimension-1);
	if (LocalLinearAlgebra.size>LocalContainerPlusVertices.size)
	{	LocalContainerPlusVertices.KillAllElements();
		LocalContainerMinusVertices.KillAllElements();
		LocalContainerPlusVertices.initAndCreateNewObjects(LocalLinearAlgebra.size);
		LocalContainerMinusVertices.initAndCreateNewObjects(LocalLinearAlgebra.size);
	}
	LocalContainerMinusVertices.ResetCounters(LocalLinearAlgebra.size);
	LocalContainerPlusVertices.ResetCounters(LocalLinearAlgebra.size);
	static bool hasPositive, hasNegative;
	hasPositive=false; hasNegative=false;
	static root PositiveChamberInternalPoint, NegativeChamberInternalPoint;
	PositiveChamberInternalPoint.MakeZero(); NegativeChamberInternalPoint.MakeZero();
	for (int i=0;i<NumCandidates;i++)
	{	static root VertexCandidate; VertexCandidate.dimension=root::AmbientDimension;
		theSelection.incrementSelectionFixedCardinality(root::AmbientDimension-1);
		if (LocalLinearAlgebra.ComputeNormalFromSelection(VertexCandidate,theSelection))
		{	if (this->PlusMinusPointIsInChamber(VertexCandidate))
			{	static Rational tempRat; static bool IsPositive; static bool IsNegative;
				root::RootScalarEuclideanRoot(VertexCandidate,theKillerFacet->normal,tempRat);
				IsPositive = tempRat.IsGreaterThan(RZero);
				IsNegative = RZero.IsGreaterThan(tempRat);
				hasPositive = (hasPositive || IsPositive);
				hasNegative = (hasNegative || IsNegative);
				for (int j=0;j<root::AmbientDimension-1;j++)
				{	if (!IsNegative)
					{	LocalContainerPlusVertices.TheObjects[theSelection.elements[j]]
						->AddRootNoRepetition(VertexCandidate);
						PositiveChamberInternalPoint.Add(VertexCandidate);
					}
					if(!IsPositive)
					{	LocalContainerMinusVertices.TheObjects[theSelection.elements[j]]
						->AddRootNoRepetition(VertexCandidate);
						NegativeChamberInternalPoint.Add(VertexCandidate);
					}
				}
			}
		}
	}
	PlusChamberIsPermanentZero=
		CombinatorialChamberPointers::IsSurelyOutsideGlobalCone( LocalContainerPlusVertices,
																											       LocalLinearAlgebra.size);
	MinusChamberIsPermanentZero=
		CombinatorialChamberPointers::IsSurelyOutsideGlobalCone( LocalContainerMinusVertices,
		                                                         LocalLinearAlgebra.size);
	if (!(hasPositive && hasNegative))
	{	return false;
	}
	//this->ComputeDebugString();
	if (!CombinatorialChamberPointers::startingCones.SeparatePoints
				( PositiveChamberInternalPoint, NegativeChamberInternalPoint,&theKillerFacet->normal))
	{ return false;
	}
	NewPlusChamber= new CombinatorialChamber;
	NewMinusChamber= new CombinatorialChamber;
	NewPlusChamber->PermanentlyZero= PlusChamberIsPermanentZero;
	NewMinusChamber->PermanentlyZero= MinusChamberIsPermanentZero;
	if (CombinatorialChamber::DisplayingGraphics)
	{	LocalContainerPlusVertices.Average(NewPlusChamber->InternalPoint,LocalLinearAlgebra.size);
		LocalContainerMinusVertices.Average(NewMinusChamber->InternalPoint,LocalLinearAlgebra.size);
		for (int i=0;i<LocalLinearAlgebra.size;i++)
		{	NewPlusChamber->AllVertices.AddRootSnoRepetition(
				*LocalContainerPlusVertices.TheObjects[i]);
			NewMinusChamber->AllVertices.AddRootSnoRepetition(
				*LocalContainerMinusVertices.TheObjects[i]);
		}
	}
	PossibleBogusNeighbors.size=0;
	PossibleBogusWalls.size=0;
	for (int i=0;i<this->ExternalWalls->size;i++)
	{	ExternalWalls->TheObjects[i]->SplitFacetMethod2(this,	NewPlusChamber,NewMinusChamber,
																		*LocalContainerPlusVertices.TheObjects[i],
																		*LocalContainerMinusVertices.TheObjects[i],
																		theKillerFacet,direction,
																		&PossibleBogusNeighbors,
																		&PossibleBogusWalls);
	}
//	if (CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic)
//	{ this->ComputeDebugString();
//	}
	for (int i=1;i<this->InternalWalls->size;i++)
	{	Facet* tempF= this->InternalWalls->TheObjects[i];
		NewPlusChamber->InternalWalls->AddObjectNoRepetitionOfPointer(tempF);
		NewMinusChamber->InternalWalls->AddObjectNoRepetitionOfPointer(tempF);
	}
	if (CombinatorialChamber::ComputingPolys)
	{	NewPlusChamber->ThePolynomial->CopyFromPoly(*this->ThePolynomial);
		NewMinusChamber->ThePolynomial->CopyFromPoly(*this->ThePolynomial);
	}
	else
	{	NewPlusChamber->ThePolynomial= this->ThePolynomial;
		NewMinusChamber->ThePolynomial= this->ThePolynomial;
	}
	theKillerFacet->Owners.AddCouple(NewPlusChamber,NewMinusChamber);
	NewPlusChamber->ExternalWalls->AddObjectOnTop(theKillerFacet);
	NewPlusChamber->ExternalWallsNormals.AddRoot(theKillerFacet->normal);
	NewMinusChamber->ExternalWalls->AddObjectOnTop(theKillerFacet);
	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	tempRoot.Assign(theKillerFacet->normal);
	tempRoot.MinusRoot();
	NewMinusChamber->ExternalWallsNormals.AddRoot(tempRoot);
	for (int i=0;i<PossibleBogusNeighbors.size;i++)
	{	if (NewPlusChamber->IsABogusNeighbor(PossibleBogusWalls.TheObjects[i],
			                                   PossibleBogusNeighbors.TheObjects[i]))
		{	PossibleBogusWalls.TheObjects[i]->Owners.RemoveCoupleBothOrders(NewPlusChamber,
				                                            PossibleBogusNeighbors.TheObjects[i]);
		}
		if (NewMinusChamber->IsABogusNeighbor(PossibleBogusWalls.TheObjects[i],
			                                    PossibleBogusNeighbors.TheObjects[i] ) )
		{	PossibleBogusWalls.TheObjects[i]->Owners.RemoveCoupleBothOrders(NewMinusChamber,
				                                            PossibleBogusNeighbors.TheObjects[i]);
		}
	}
	NewPlusChamber->IndexStartingCrossSectionNormal= this->IndexStartingCrossSectionNormal;
	NewMinusChamber->IndexStartingCrossSectionNormal= this->IndexStartingCrossSectionNormal;
	output.AddObjectOnTop(NewPlusChamber);
	output.AddObjectOnTop(NewMinusChamber);
	if (!NewPlusChamber->PermanentlyZero )
	{	output.PreferredNextChambers.AddObjectOnBottom (NewPlusChamber);
		output.NextChamberToSliceIndexInPreferredNextChambers++;
	}
	else
	{	NewPlusChamber->Explored=true;
	}
	if (!NewMinusChamber->PermanentlyZero )
	{	output.PreferredNextChambers.AddObjectOnBottom(NewMinusChamber);
		output.NextChamberToSliceIndexInPreferredNextChambers++;
	}
	else
	{	NewMinusChamber->Explored =true;
	}
	if (!(NewPlusChamber->ExternalWalls->size>=root::AmbientDimension)){AnErrorHasOcurred=true;}
	if (!(NewMinusChamber->ExternalWalls->size>=root::AmbientDimension)){AnErrorHasOcurred=true;}
//	if (CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic)
//	{ NewPlusChamber->ComputeDebugString();
//	}
	assert(NewPlusChamber->ExternalWalls->size>=root::AmbientDimension);
	assert(NewMinusChamber->ExternalWalls->size>=root::AmbientDimension);
	if (AnErrorHasOcurred)
	{	LocalContainerPlusVertices.ComputeDebugString();
		LocalContainerMinusVertices.ComputeDebugString();
		CombinatorialChamberPointers::TheBigDump << LocalContainerPlusVertices.DebugString<<"\n"
																						 << LocalContainerMinusVertices.DebugString;
		NewPlusChamber->ComputeDebugString();
		NewMinusChamber->ComputeDebugString();
		this->ComputeDebugString();
		CombinatorialChamberPointers::TheBigDump << this->DebugString;
		CombinatorialChamberPointers::TheBigDump << NewPlusChamber->DebugString;
		CombinatorialChamberPointers::TheBigDump << NewMinusChamber->DebugString;
	}
	return true;
}


bool CombinatorialChamber::SplitChamber(Facet* theKillerFacet,CombinatorialChamberPointers& output, root& direction)
{	if (CombinatorialChamber::MethodUsed==2)
	{
		return this->SplitChamberMethod2(theKillerFacet,output,direction);
	}
	if (CombinatorialChamber::MethodUsed==1)
	{
		return this->SplitChamberMethod1(theKillerFacet,output);
	}
	return false;
}

bool CombinatorialChamber::SplitChamberMethod1(Facet* theKillerFacet,CombinatorialChamberPointers& output)
{
/*	CombinatorialChamber* NewPlusChamber  = new CombinatorialChamber;
	CombinatorialChamber* NewMinusChamber = new CombinatorialChamber;
	NewPlusChamber->ThePolynomial->CopyFrom(*this->ThePolynomial);
	NewMinusChamber->ThePolynomial->CopyFrom(*this->ThePolynomial);
	NewPlusChamber->ExternalWalls->AddObject(theKillerFacet);
	NewMinusChamber->ExternalWalls->AddObject(theKillerFacet);
	NewPlusChamber->IndexStartingCrossSectionNormal= this->IndexStartingCrossSectionNormal;
	NewMinusChamber->IndexStartingCrossSectionNormal= this->IndexStartingCrossSectionNormal;
	theKillerFacet->MinusOwner=NewMinusChamber;
	theKillerFacet->PlusOwner=NewPlusChamber;
//	theKillerFacet->PositiveDescendant=theKillerFacet;
//	theKillerFacet->NegativeDescendant=theKillerFacet;
//	assert(this->ComputeDebugString());
	for(int i=0; i<this->ExternalWalls->size;i++)
	{
		static root TempMNormal; TempMNormal.dimension=root::AmbientDimension;
		TempMNormal.Assign( theKillerFacet->normal);
		TempMNormal.MinusRoot();
		if (this->ExternalWalls->TheObjects[i]->HasHNonPositiveVertex( theKillerFacet->normal)
				&&
				this->ExternalWalls->TheObjects[i]->HasHNonPositiveVertex(TempMNormal))
		{
			this->ExternalWalls->TheObjects[i]->SplitFacetFindVertices(theKillerFacet,this);
			static root tempRoot; tempRoot.dimension=root::AmbientDimension;
			this->ExternalWalls->TheObjects[i]->GetNormal(this,tempRoot);
			theKillerFacet->Boundaries.AddRootNoRepetition(tempRoot);
//			theKillerFacet->ComputeDebugString(this) ;
		}
//		ExternalWalls->TheObjects[i]->ComputeDebugString(this);
	}
//	assert(this->ComputeDebugString());
	this->ExternalWalls->AddObject(theKillerFacet);
	assert(theKillerFacet->AllVertices.size>0);
	for (int i =0;i<this->ExternalWalls->size-1; i++)
	{
		Facet* tempFacet=this->ExternalWalls->TheObjects[i];
		tempFacet->SplitVerticesAndCreateNewFacets(NewPlusChamber,NewMinusChamber,theKillerFacet,this);
	}
	this->PurgeAllSentencedFacets();
	theKillerFacet->PurgeRedundantBoundaries();
//	NewPlusChamber->ComputeDebugString();
//	NewMinusChamber->ComputeDebugString();
	output.AddObject(NewPlusChamber);
	output.AddObject(NewMinusChamber);*/
	return true;
}

bool CombinatorialChamberPointers::IsSurelyOutsideGlobalCone
					(ListObjectPointers<roots>& TheVertices,int NumrootsLists)
{ return CombinatorialChamberPointers::TheGlobalConeNormals.IsSurelyOutsideCone
			(TheVertices,NumrootsLists);
}

void CombinatorialChamberPointers::KillNextChamberToSlice()
{
	this->Pop(this->NextChamberToSlice);
	if (this->NextChamberToSliceIndexInPreferredNextChambers!=-1)
	{
		this->PreferredNextChambers.PopIndexShiftUp
			(this->NextChamberToSliceIndexInPreferredNextChambers);
	}
}

void CombinatorialChamberPointers::ComputeGlobalCone(roots& directions)
{ CombinatorialChamberPointers::TheGlobalConeNormals.ComputeFromDirections(directions);
}

void CombinatorialChamberPointers::ComputeNextIndexToSlice(root &direction)
{
	for (int i=0;i<this->PreferredNextChambers.size;i++)
	{
		if (this->PreferredNextChambers.TheObjects[i]->TestPossibilityToSlice(direction))
		{
			this->NextChamberToSlice=this->PreferredNextChambers.TheObjects[i];
			this->NextChamberToSliceIndexInPreferredNextChambers=i;
			return;
		}
	}
	this->NextChamberToSliceIndexInPreferredNextChambers=-1;
	bool foundUnexplored=false;
	for (int i=this->FirstNonExploredIndex;i<size;i++)
	{	if(!foundUnexplored && !this->TheObjects[i]->Explored )
		{ this->FirstNonExploredIndex=i;
			foundUnexplored=true;
		}
		if (this->TheObjects[i]->TestPossibilityToSlice (direction))
		{	this->NextChamberToSlice= this->TheObjects[i];
			return;
		}
	}
	this->NextChamberToSlice=0;
}

CombinatorialChamberPointers::CombinatorialChamberPointers()
{	this->FirstNonExploredIndex=0;
	this->NextChamberToSliceIndexInPreferredNextChambers=-1;
	this->NextChamberToSlice=0;
	this->ThePolys= 0;
	this->ThePolys= new QuasiPolynomials;
}

CombinatorialChamberPointers::~CombinatorialChamberPointers()
{	Free();
}

void CombinatorialChamberPointers::DumpAll()
{	CombinatorialChamberPointers::TheBigDump.clear();
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i]->ComputeDebugString();
		CombinatorialChamberPointers::TheBigDump <<this->TheObjects[i]->DebugString;
	}
}

void CombinatorialChamberPointers::Free()
{	delete this->ThePolys;
	this->ThePolys=0;
}

void CombinatorialChamberPointers::MakeStartingChambers(roots& directions, FacetPointers& FacetOutput)
{	this->startingCones.initFromDirections(directions);
	this->startingCones.ComputeDebugString();
	if (CombinatorialChamber::ComputingPolys)
	{	PrecomputedQuasiPolynomialIntegrals::PreComputedBernoulli
			->ComputeDiscreteIntegrationUpTo(15);
	}
	CombinatorialChamberPointers::ComputeGlobalCone(directions);
	Selection theSelection;
	theSelection.init(root::AmbientDimension);
	FacetOutput.initAndCreateNewObjects(root::AmbientDimension);
	for(int i=0;i<root::AmbientDimension;i++)
	{	roots TempRoots;
		root TempRoot;
		Rational tempRat;
		theSelection.incrementSelectionFixedCardinality(root::AmbientDimension-1);
		directions.SubSelection(theSelection,TempRoots);
		TempRoots.ComputeNormal(TempRoot);
		root::RootScalarEuclideanRoot(TempRoot,directions.TheObjects[root::AmbientDimension-i-1],tempRat);
		assert(!tempRat.IsEqualTo(RZero));
		TempRoot.DivByRational(tempRat);
		FacetOutput.TheObjects[i]->normal.Assign(TempRoot);
	}
	theSelection.initNoMemoryAllocation();
	int NumStartingChambers=TwoToTheNth(root::AmbientDimension);
	this->initAndCreateNewObjects(NumStartingChambers);
	CombinatorialChamber::StartingCrossSectionAffinePoints.SetSizeExpandOnTopNoObjectInit(NumStartingChambers);
	for(int i=0;i<NumStartingChambers;i++)
	{	int tempI= theSelection.SelectionToIndex();
		this->TheObjects[tempI]->ExternalWallsNormals.SetSizeExpandOnTopNoObjectInit(root::AmbientDimension);
		for (int j=0;j<root::AmbientDimension;j++)
		{	this->TheObjects[tempI]->ExternalWalls->AddObjectOnTop
				(FacetOutput.TheObjects[root::AmbientDimension-j-1]);
			this->TheObjects[tempI]->ExternalWallsNormals.TheObjects[j].Assign
				(FacetOutput.TheObjects[root::AmbientDimension-j-1]->normal);
			this->TheObjects[tempI]->AllVertices.AddRoot(directions.TheObjects[j]);
			if (theSelection.selected[j])
			{	int tempI2;
				theSelection.selected[j]=false;
				tempI2=theSelection.SelectionToIndex();
				theSelection.selected[j]=true;
				FacetOutput.TheObjects[root::AmbientDimension-j-1]->
					Owners.AddCouple(this->TheObjects[tempI],this->TheObjects[tempI2]);
			}
			else
			{	this->TheObjects[tempI]->AllVertices.TheObjects[j].MinusRoot();
				this->TheObjects[tempI]->ExternalWallsNormals.TheObjects[j].MinusRoot();
			}
		}
		root tempRoot;
		tempRoot.Assign(directions.TheObjects[0]);
		if (!theSelection.selected[0]) {tempRoot.MinusRoot(); }
		CombinatorialChamber::StartingCrossSectionAffinePoints.TheObjects[tempI].Assign(tempRoot);
		theSelection.incrementSelection();
	}
	for(int i=0;i<NumStartingChambers;i++)
	{	this->TheObjects[i]->IndexStartingCrossSectionNormal=i;
		root Accum;
		Accum.MakeZero();
		for (int j=0;j<root::AmbientDimension;j++)
		{	root tempNormal;
			this->TheObjects[i]->ExternalWalls->TheObjects[j]
					->GetNormal(this->TheObjects[i],tempNormal);
			this->TheObjects[i]->ExternalWallsNormals.AddRoot(tempNormal);
			Accum.Add(tempNormal);
		}
		CombinatorialChamber::StartingCrossSectionNormals.AddRoot(Accum);
//		this->TheObjects[i]->ComputeEdgesAndVerticesFromNormals(true,true);
		assert(this->TheObjects[i]->CheckVertices());
//		this->TheObjects[i]->ComputeDebugString();
	}
	if (CombinatorialChamber::ComputingPolys)
	{	this->TheObjects[NumStartingChambers-1]->ThePolynomial
			->MakeNVarConst(root::AmbientDimension, QNOne);
//		this->TheObjects[NumStartingChambers-1]->ThePolynomial->ComputeDebugString();
		for (int i=0;i<root::AmbientDimension;i++)
		{ root tempNormal;
			tempNormal.Assign (this->TheObjects[NumStartingChambers-1]->ExternalWalls
														 ->TheObjects[i]->normal);
			QPSub tempSub;
			tempSub.MakeLinearSubIntegrand(tempNormal,directions.TheObjects[i],RZero);
			if (tempSub.QNSubDen!=1)
			{	QuasiPolynomial tempQP;
				QuasiNumber tempQN;
				tempQN.MakePureQN(1,0,ROne,1,0,1);
				tempQN.LinearSubstitution(tempSub);
				tempQP.MakeNVarConst(root::AmbientDimension,tempQN);
				this->TheObjects[NumStartingChambers-1]->ThePolynomial->MultiplyBy(tempQP);
			}
		}
	}
	else
	{	this->TheObjects[NumStartingChambers-1]->ThePolynomial+=1;
	}
}

void Cone::ComputeFromDirections(roots& directions)
{	this->size=0;
	int NumCandidates = NChooseK(directions.size,root::AmbientDimension-1);
	Selection theSelection;
	theSelection.init(directions.size);
	root normalCandidate;
	for (int i=0;i<NumCandidates;i++)
	{	theSelection.incrementSelectionFixedCardinality(root::AmbientDimension-1);
		if (directions.ComputeNormalFromSelection(normalCandidate,theSelection))
		{	bool hasPositive; bool hasNegative;
			hasPositive=false; hasNegative=false;
			for (int j=0;j<directions.size;j++)
			{	Rational tempRat;
				root::RootScalarEuclideanRoot(directions.TheObjects[j],normalCandidate,tempRat);
				if (RZero.IsGreaterThan(tempRat)){hasNegative=true;}
				if (tempRat.IsGreaterThan(RZero)){hasPositive=true;}
			}
			if ((hasNegative && !hasPositive))
			{normalCandidate.MinusRoot();}
			normalCandidate.ScaleToIntegralMinHeight();
			if (!(hasNegative && hasPositive))
			{	bool IsValid;
				IsValid=true;
				for (int j=0;j<this->size;j++)
				{	if (normalCandidate.IsProportianalTo( this->TheObjects[j]))
						IsValid=false;
				}
				if (IsValid)
				{	this->AddRootNoRepetition(normalCandidate);
				}
			}
		}
	}
	this->ChamberTestArray.SetSizeExpandOnTopNoObjectInit(this->size);
}

bool Cone::IsSurelyOutsideCone(ListObjectPointers<roots>& TheVertices, int NumrootsLists)
{ //init first the array we will use for check
  //values in the array: -1 undertermined, 0 - outside normal, +1 inside normal
	for (int i=0;i<this->size;i++)
	{	this->ChamberTestArray.TheObjects[i]=-1;
	}
	for (int i=0;i<NumrootsLists;i++)
	{	for (int j=0;j<TheVertices.TheObjects[i]->size;j++)
		{	static bool TestedVertexIsStrictlyInsideGlobalCone;
			TestedVertexIsStrictlyInsideGlobalCone= true;
			for (int k=0;k<this->size;k++)
			{	static Rational tempRat;
				root::RootScalarEuclideanRoot(TheVertices.TheObjects[i]->TheObjects[j],
																this->TheObjects[k],tempRat);
				static int state;
				state=-1;
				if (tempRat.IsGreaterThan(RZero))
				{	state =1;
				}
				else
				{	TestedVertexIsStrictlyInsideGlobalCone=false;
					if (RZero.IsGreaterThan(tempRat))
					{	state=0;
					}
				}
				if (state!=-1)
				{	if (this->ChamberTestArray.TheObjects[k]==-1)
					{	this->ChamberTestArray.TheObjects[k]=state;
					}
					else
					{	if (this->ChamberTestArray.TheObjects[k]!=state)
						{return false;}
					}
				}
			}
			if (TestedVertexIsStrictlyInsideGlobalCone){return false;}
		}
	}
	for (int i=0;i<this->size;i++)
	{ if (this->ChamberTestArray.TheObjects[i]==0){return true;}
	}
	return false;
}

bool Cone::IsInCone(root& r)
{ for (int i=0;i<this->size;i++)
	{	Rational tempRat;
		root::RootScalarEuclideanRoot(r,this->TheObjects[i],tempRat);
		if (tempRat.IsNegative()){return false;}
	}
	return true;
}

bool Cone::SeparatesPoints(root& point1, root& point2)
{ bool tempB1= this->IsInCone(point1);
	bool tempB2= this->IsInCone(point2);
	return !(tempB1==tempB2);
}

void Cone::operator=(Cone& right)
{ this->CopyFromBase(right);
	this->ChamberTestArray.CopyFromBase(right.ChamberTestArray);
}

void CombinatorialChamberPointers::PrintThePolys(std::string& output)
{	std::stringstream out;
	if (CombinatorialChamberPointers::PrintLastChamberOnly)
	{ if (this->LastComputedChamber!=0)
		{ std::string tempS;
			out << this->LastComputedChamber->CreationNumber<<". ";
			this->LastComputedChamber->ThePolynomial->StringPrintOutAppend(tempS,PolyFormatLocal);
			out << tempS;
		}
	}
	else
	{ for (int i=0;i<this->size;i++)
		{	if (!this->TheObjects[i]->HasZeroPoly())
			{	std::string tempS;
				out << this->TheObjects[i]->CreationNumber<<". ";
				this->ThePolys->TheObjects[i].StringPrintOutAppend(tempS,PolyFormatLocal);
				out << tempS<<"\r\n";
			}
		}
	}
	output=out.str();
}

void CombinatorialChamberPointers::initThePolys()
{	if (CombinatorialChamber::ComputingPolys)
	{	this->ThePolys->SetSizeExpandOnTopNoObjectInit(this->size);
		for (int i =0; i<size; i++)
		{	this->ThePolys->TheObjects[i].CopyFromPoly(*this->TheObjects[i]->ThePolynomial);
		}
	}
}

void CombinatorialChamberPointers::LabelAllUnexplored()
{
	for (int i =0; i<size; i++)
	{	if (!this->TheObjects[i]->PermanentlyZero)
		{	this->TheObjects[i]->Explored =false;
		}
	}
	FirstNonExploredIndex=0;
}

void Facet::ComputeInternalPoint(root& output)
{	output.MakeZero();
	for (int i =0; i<this->AllVertices.size;i++ )
	{ output.Add( this->AllVertices.TheObjects[i]);
	}
	output.DivByInteger(AllVertices.size);
}


bool Facet::IsExternalWithRespectToDirection(root &direction, CombinatorialChamber* owner, root& TheNormal)
{	static Rational tempRat;
	root::RootScalarEuclideanRoot(direction,TheNormal,tempRat);
	return tempRat.IsGreaterThan(RZero);
}

bool Facet::IsInternalWRT(CombinatorialChamber* owner)
{ for (int i=0;i<this->Owners.size;i++)
	{ if (this->Owners.TheObjects[i].MinusOwner==owner &&
			  this->Owners.TheObjects[i].PlusOwner ==owner)
		{	return true;
		}
	}
	return false;
}

bool Facet::IsInFacetWithBoundaries(root &projection)
{	for (int i =0;i<this->Boundaries.size;i++)
	{	Rational tempRat;
		root::RootScalarEuclideanRoot(projection,Boundaries.TheObjects[i],tempRat);
		if (RZero.IsGreaterThan(tempRat))
		{	return false;
		}
	}
	return true;
}

CombinatorialChamber* Facet::TheFirstOtherChamber(CombinatorialChamber *owner)
{	for (int i=0;i<this->Owners.size;i++)
	{	if (this->Owners.TheObjects[i].PlusOwner==owner)
		{	return this->Owners.TheObjects[i].MinusOwner;
		}
		if (this->Owners.TheObjects[i].MinusOwner==owner)
		{	return this->Owners.TheObjects[i].PlusOwner;
		}
	}
	return 0;
}

void Facet::GetNormal(CombinatorialChamber* owner,root& output)
{	output.dimension=root::AmbientDimension;
	for (int i=0;i<this->Owners.size;i++)
	{	if (this->Owners.TheObjects[i].PlusOwner ==owner)
		{	output.Assign(normal);
			return;
		}
		if (this->Owners.TheObjects[i].MinusOwner ==owner)
		{	output.Assign(normal);
			output.MinusRoot();
			return;
		}
	}
	this->ComputeDebugString(owner);
	Stop();
}

bool Facet::HasHNonPositiveVertex(root &h)
{	for (int i=0;i<AllVertices.size;i++)
	{	Rational tempRat;
		root::RootScalarEuclideanRoot(h,AllVertices.TheObjects[i],tempRat);
		if(RZero.IsGreaterThanOrEqualTo(tempRat))
		{	return true;
		}
	}
	return false;
}

bool Facet::HasHPositiveVertex(root& h)
{	for (int i=0;i<AllVertices.size;i++)
	{	Rational tempRat;
		root::RootScalarEuclideanRoot(h,AllVertices.TheObjects[i],tempRat);
		if(tempRat.IsGreaterThan(RZero))
		{	return true;
		}
	}
	return false;
}

void Facet::ComputeDebugString(CombinatorialChamber* owner)
{
	std::stringstream out;
	this->DebugString.clear();
	out <<"n" <<this->CreationNumber<<": ";
	std::string tempStr;
	this->normal.ElementToString(tempStr);
	out << tempStr;
	for (int i=0;i<this->Owners.size;i++)
	{
		out<<"||"<<"+c"<<this->Owners.TheObjects[i].PlusOwner->CreationNumber
			 <<     " -c"<<this->Owners.TheObjects[i].MinusOwner->CreationNumber <<" ||";
	}
	if(CombinatorialChamber::MethodUsed==1)
	{
		for (int i =0;i<this->Boundaries.size;i++)
		{
			out <<"b"<<i<<": ";
			this->Boundaries.TheObjects[i].ElementToString(tempStr);
			out<< tempStr;
			out<<"||";
		}
		for (int i =0;i<this->AllVertices.size;i++)
		{
			out <<"v"<<i<<": ";
			this->AllVertices.TheObjects[i].ElementToString(tempStr);
			out<< tempStr;
			out<<"||";
		}
	}
	DebugString= out.str();
}

Facet::Facet()
{	this->SentencedToDeath=false;
	GlobalCollectorFacets.AddObjectOnTop(this);
	CreationNumber= GlobalCollectorFacets.size;
	init();
}

bool Facet::OneOfTheOtherChambersIsNotExplored(CombinatorialChamber* owner)
{	static	CombinatorialChamber* theOtherChamber;
	for(int i=0;i<this->Owners.size;i++)
	{	theOtherChamber=0;
		if (this->Owners.TheObjects[i].MinusOwner==owner)
		{theOtherChamber=this->Owners.TheObjects[i].PlusOwner;}
		if (this->Owners.TheObjects[i].PlusOwner==owner)
		{theOtherChamber=this->Owners.TheObjects[i].MinusOwner;}
		if (theOtherChamber!=0)
		{	if (!theOtherChamber->Explored)
			{return true;}
		}
	}
	return false;
}

void Facet::init()
{
}

Facet::~Facet()
{	GlobalCollectorFacets.TheObjects[this->CreationNumber-1]=0;
}

void Facet::FindAllNeighborsTo(CombinatorialChamber* TheChamber,
					ListObjectPointers<CombinatorialChamber>& output)
{ for (int i=0;i<this->Owners.size;i++)
	{ if (Owners.TheObjects[i].MinusOwner==TheChamber &&
				Owners.TheObjects[i].PlusOwner!=TheChamber)
				output.AddObjectNoRepetitionOfPointer ( Owners.TheObjects[i].PlusOwner);
		if (Owners.TheObjects[i].PlusOwner==TheChamber &&
				Owners.TheObjects[i].MinusOwner!=TheChamber)
				output.AddObjectNoRepetitionOfPointer( Owners.TheObjects[i].MinusOwner);
	}
}

bool Facet::FacetContainsChamberOnlyOnce(CombinatorialChamber *owner)
{	bool FoundOnce= false;
	for (int i=0;i<this->Owners.size;i++)
	{	if (this->Owners.TheObjects[i].MinusOwner==owner)
		{	FoundOnce= !FoundOnce;
			if (!FoundOnce){return false;}
		}
		if (this->Owners.TheObjects[i].PlusOwner==owner)
		{	FoundOnce= !FoundOnce;
			if (!FoundOnce){return false;}
		}
	}
	return FoundOnce;
}

void Facet::Free()
{	for (int i=0;i<this->Owners.size;i++)
	{	if (this->Owners.TheObjects[i].PlusOwner!=0)
		{	this->Owners.TheObjects[i].PlusOwner->InternalWalls->Pop(this);
			this->Owners.TheObjects[i].PlusOwner->ExternalWalls->Pop(this);
			this->Owners.TheObjects[i].PlusOwner=0;
		}
		if (this->Owners.TheObjects[i].MinusOwner!=0)
		{	this->Owners.TheObjects[i].MinusOwner->InternalWalls->Pop(this);
			this->Owners.TheObjects[i].MinusOwner->ExternalWalls->Pop(this);
			this->Owners.TheObjects[i].MinusOwner=0;
		}
	}
}

bool Facet::SplitFacetMethod2(CombinatorialChamber *BossChamber,
															CombinatorialChamber *NewPlusChamber,
															CombinatorialChamber *NewMinusChamber,
															roots& ThePlusVertices, roots& TheMinusVertices,
															Facet* TheKillerFacet, root& direction,
															CombinatorialChamberPointers* PossibleBogusNeighbors,
															FacetPointers* PossibleBogusWalls)
{	static bool IsPositive, IsNegative;
	IsPositive = false;	IsNegative = false;
	static Rational tempRat;
//	if (CombinatorialChamberPointers::AnErrorHasOcurredTimeToPanic)
//	{ this->ComputeDebugString(BossChamber);
//	}
	for (int j=0;j<ThePlusVertices.size;j++)
	{	root::RootScalarEuclideanRoot(TheKillerFacet->normal,ThePlusVertices.TheObjects[j],tempRat);
		if (tempRat.IsGreaterThan(RZero)){IsPositive=true;}
	}
	for (int j=0;j<TheMinusVertices.size;j++)
	{	root::RootScalarEuclideanRoot(TheKillerFacet->normal,TheMinusVertices.TheObjects[j],tempRat);
		if (RZero.IsGreaterThan(tempRat)){IsNegative=true;}
		if (IsPositive && IsNegative){break;}
	}
	if (!(IsPositive || IsNegative))
	{	this->ComputeDebugString(BossChamber);
		TheKillerFacet->ComputeDebugString(BossChamber);
		BossChamber->ComputeDebugString();
		std::string tempS;
		ThePlusVertices.ElementToString(tempS);
		CombinatorialChamberPointers::TheBigDump<<"\nThePlusVertices:\n"<<tempS;
		TheMinusVertices.ElementToString(tempS);
		CombinatorialChamberPointers::TheBigDump<<"\nTheMinusVertices:\n"<<tempS;
		CombinatorialChamberPointers::TheBigDump<<"\n this:\n" <<this->DebugString;
		CombinatorialChamberPointers::TheBigDump<<"\n TheKillerFacet:\n" <<TheKillerFacet->DebugString;
		CombinatorialChamberPointers::TheBigDump<<"\n BossChamber:\n" <<BossChamber->DebugString;
	}
	assert(IsPositive || IsNegative);
	if (IsPositive && IsNegative)
	{//we must split the face
		static CombinatorialChamber* TheOtherChamber;
		static bool IsAPlusOwner;
		while(this->IsAnOwner(BossChamber,TheOtherChamber,IsAPlusOwner))
		{	if(IsAPlusOwner)
			{
				this->Owners.AddCouple(NewPlusChamber,TheOtherChamber);
				this->Owners.AddCouple(NewMinusChamber,TheOtherChamber);
				this->Owners.RemoveCouple(BossChamber,TheOtherChamber);
			}
			else
			{	this->Owners.AddCouple(TheOtherChamber,NewPlusChamber);
				this->Owners.AddCouple(TheOtherChamber,NewMinusChamber);
				this->Owners.RemoveCouple(TheOtherChamber,BossChamber);
			}
			if (PossibleBogusNeighbors!=0)
			{	PossibleBogusNeighbors->AddObjectOnTop(TheOtherChamber);
				PossibleBogusWalls->AddObjectOnTop(this);
			}
			if (TheOtherChamber!=0)
			{	if (!TheOtherChamber->PermanentlyZero)
					TheOtherChamber->AddInternalWall(TheKillerFacet,this,direction);
			}
		}
		NewPlusChamber->ExternalWalls->AddObjectOnTop(this);
		NewMinusChamber->ExternalWalls->AddObjectOnTop(this);
		if (IsAPlusOwner)
		{	NewPlusChamber->ExternalWallsNormals.AddRoot(this->normal);
			NewMinusChamber->ExternalWallsNormals.AddRoot(this->normal);
		}
		else
		{	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
			tempRoot.Assign(this->normal);
			tempRoot.MinusRoot();
			NewPlusChamber->ExternalWallsNormals.AddRoot(tempRoot);
			NewMinusChamber->ExternalWallsNormals.AddRoot(tempRoot);
		}
		return true;
	}
	else
	{	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
		BossChamber->GetNormalFromFacet(tempRoot,this);
		CombinatorialChamber* tempC;
		static root FacetInternalPoint; FacetInternalPoint.dimension=root::AmbientDimension;
		if (IsPositive)
		{tempC= NewPlusChamber;}
		else
		{tempC= NewMinusChamber;}
		this->Owners.Substitute(this,BossChamber,tempC);
		if(tempC->ExternalWalls->AddObjectNoRepetitionOfPointer(this))
		{	tempC->ExternalWallsNormals.AddRoot(tempRoot);
		}
		return false;
	}
}

bool Facet::SplitVerticesAndCreateNewFacets(CombinatorialChamber* NewPlusChamber,
																						CombinatorialChamber* NewMinusChamber, Facet* TheKillerFacet,
																						CombinatorialChamber* BossChamber)
{/*
	static roots PositiveVertices;
	static roots NegativeVertices;
	PositiveVertices.size=0;
	NegativeVertices.size=0;
	int StrictlyPositive=0;
	int StrictlyNegative=0;
	for (int j=0;j<this->AllVertices.size;j++)
	{
		Rational tempRat;
		RootScalarEuclideanRoot(this->AllVertices.TheObjects[j],TheKillerFacet->normal,tempRat);
		if (RationalGreaterThanRational(RZero,tempRat))
		{
			StrictlyNegative++;
		}
		else
		{
			PositiveVertices.AddRoot(this->AllVertices.TheObjects[j]);
		}
		if (RationalGreaterThanRational(tempRat,RZero))
		{
			StrictlyPositive++;
		}
		else
		{
			NegativeVertices.AddRoot(this->AllVertices.TheObjects[j]);
		}
	}
	if (PositiveVertices.size<root::AmbientDimension-1 &&  NegativeVertices.size<root::AmbientDimension-1)
	{
		this->DumpSplitVerticesAndCreateNewFacets(TheKillerFacet,BossChamber);
	}
	if (!(StrictlyNegative>0 || StrictlyPositive>0))
	{
		this->DumpSplitVerticesAndCreateNewFacets(TheKillerFacet,BossChamber);
	}
	assert(!(PositiveVertices.size<root::AmbientDimension-1 &&  NegativeVertices.size<root::AmbientDimension-1));
	assert(StrictlyNegative>0 || StrictlyPositive>0);
	if (StrictlyPositive>0 && StrictlyNegative>0)
	{//we must split the face
		this->SentencedToDeath =true;
		Facet* PositiveDescendant= new Facet;
		Facet* NegativeDescendant= new Facet;
		CopyRoots(NegativeVertices,NegativeDescendant->AllVertices);
		CopyRoots(PositiveVertices,PositiveDescendant->AllVertices);
		CopyRoots(this->Boundaries,NegativeDescendant->Boundaries);
		CopyRoots(this->Boundaries,PositiveDescendant->Boundaries);
		CopyRoot(this->normal, PositiveDescendant->normal);
		CopyRoot(this->normal, NegativeDescendant->normal);
		NegativeDescendant->Boundaries.AddRoot(TheKillerFacet->normal);
		NegativeDescendant->Boundaries
			.TheObjects[NegativeDescendant->Boundaries.size-1].MinusRoot();
		PositiveDescendant->Boundaries.AddRoot(TheKillerFacet->normal);
		assert (BossChamber == this->PlusOwner || BossChamber ==this->MinusOwner);
		CombinatorialChamber* TheOtherOwner;
		if (BossChamber==this->PlusOwner)
		{
			PositiveDescendant->PlusOwner= NewPlusChamber;
			NegativeDescendant->PlusOwner= NewMinusChamber;
			PositiveDescendant->MinusOwner= this->MinusOwner;
			NegativeDescendant->MinusOwner= this->MinusOwner;
			TheOtherOwner=this->MinusOwner;
		}
		else
		{
			PositiveDescendant->PlusOwner  = this->PlusOwner;
			NegativeDescendant->PlusOwner  = this->PlusOwner;
			PositiveDescendant->MinusOwner = NewPlusChamber;
			NegativeDescendant->MinusOwner = NewMinusChamber;
			TheOtherOwner=this->PlusOwner;
		}
		NewPlusChamber->ExternalWalls->AddObject(PositiveDescendant);
		NewMinusChamber->ExternalWalls->AddObject(NegativeDescendant);
		if (TheOtherOwner!=0)
		{
			TheOtherOwner->ExternalWalls->AddObject(PositiveDescendant);
			TheOtherOwner->ExternalWalls->AddObject(NegativeDescendant);
			TheOtherOwner->ExternalWalls->Pop(this);
		}
		NegativeDescendant->PurgeRedundantBoundaries();
		PositiveDescendant->PurgeRedundantBoundaries();
		return true;
	}
	else
	{
		CombinatorialChamber* tempOwnerChamber;
		if (StrictlyPositive>0)
		{
			tempOwnerChamber=NewPlusChamber;
		}
		else
		{
			tempOwnerChamber=NewMinusChamber;
		}
		tempOwnerChamber->ExternalWalls->AddObject(this);
		if (this->MinusOwner ==BossChamber)
		{this->MinusOwner=tempOwnerChamber;}
		else
		{this->PlusOwner=tempOwnerChamber;}
	}*/
	return false;
}

bool Facet::CheckVertices(CombinatorialChamber* owner)
{
	for(int i=0;i<this->AllVertices.size;i++)
	{
		root tempRoot;
		tempRoot.Assign(this->AllVertices.TheObjects[i]);
		owner->ScaleVertexToFitCrossSection(tempRoot);
		assert(tempRoot.IsEqualTo(this->AllVertices.TheObjects[i]));
	}
	return true;
}

bool Facet::DumpSplitVerticesAndCreateNewFacets(
					Facet* TheKillerFacet,CombinatorialChamber* BossChamber)
{
	/*	TheKillerFacet->ComputeDebugString(BossChamber);
		this->PlusOwner->ComputeDebugString();
		this->MinusOwner->ComputeDebugString();
		this->ComputeDebugString(BossChamber);
		CombinatorialChamberPointers::TheBigDump<<TheKillerFacet->DebugString <<"\n\n"<<this->DebugString<<"\n\n"
			<< BossChamber->DebugString<<"\n\n\n\n\n"
			<< this->PlusOwner->DebugString<<"\n\n"
			<< this->MinusOwner->DebugString;*/
		return true;
}

/*int Facet::GetChamberSign(CombinatorialChamber* owner)
{
	assert(owner == this->PlusOwner || owner== this->MinusOwner);
	if (owner == this->PlusOwner) {return +1;}
	if (owner ==this->MinusOwner) {return -1;}
	return -100000;
}*/

bool Facet::SplitFacetFindVertices(Facet* TheKillerFacet, CombinatorialChamber* owner)
{ Selection theSelection;
	static bool result;
	result=false;
	theSelection.init(this->Boundaries.size);
	int NumCandidates= NChooseK(this->Boundaries.size, root::AmbientDimension-3);
	for (int i=0;i<NumCandidates;i++)
	{	static root VertexCandidate; VertexCandidate.dimension=root::AmbientDimension;
		theSelection.incrementSelectionFixedCardinality(root::AmbientDimension-3);
		if(this->Boundaries.ComputeNormalFromSelectionAndTwoExtraRoots
			(VertexCandidate,TheKillerFacet->normal,this->normal,theSelection))
		{	owner->ScaleVertexToFitCrossSection(VertexCandidate);
			if (this->IsInFacetWithBoundaries(VertexCandidate))
			{ this->AllVertices.AddRootNoRepetition(VertexCandidate);
				if(TheKillerFacet->AllVertices.AddRootNoRepetition(VertexCandidate))
				{	result=true;
				}
			}
		}
	}
	return result;
}

void Facet::PurgeRedundantBoundaries()
{	for (int i=0; i<this->Boundaries.size;i++)
	{	bool IsRedundant= true;
		for (int j=0;j<this->AllVertices.size;j++)
		{	Rational tempRat;
			root::RootScalarEuclideanRoot(Boundaries.TheObjects[i],AllVertices.TheObjects[j],tempRat);
			if (RZero.IsEqualTo(tempRat))
			{	IsRedundant=false;
				break;
			}
			assert(!RationalGreaterThanRational(RZero, tempRat));
		}
		if (IsRedundant)
		{	Boundaries.Pop(i);
			i--;
		}
	}
}

void Facet::PurgeAllEdgesAndVertices()
{	this->AllVertices.size = 0;
	this->Boundaries.size  = 0;
}

Facet* Facet::MakeFacetFromEdgeAndDirection(CombinatorialChamber* owner,
																			Facet* theOtherFacet, root& direction,
																			roots & directions, int CurrentIndex,
																			FacetPointers& FacetOutput)
{	static root tempNormal1, tempNormal2, NewFacetNormal;
	tempNormal1.dimension=root::AmbientDimension;
	tempNormal2.dimension=root::AmbientDimension;
	NewFacetNormal.dimension=root::AmbientDimension;
	static Rational a1,a2, b;
	owner->GetNormalFromFacet(tempNormal1,this);
	owner->GetNormalFromFacet(tempNormal2, theOtherFacet);
	root::RootScalarEuclideanRoot(direction,tempNormal1,a1);
	root::RootScalarEuclideanRoot(direction,tempNormal2,a2);
	assert(!RationalEqualsRational(a1,RZero));
	assert(!RationalEqualsRational(a2,RZero));
	RationalDivRational(a1,a2,b);
	b.MinusRational();
	root::RootPlusRootTimesScalar(tempNormal1,tempNormal2,b,NewFacetNormal);
	for (int i=0;i<FacetOutput.size;i++)
	{	if (NewFacetNormal.IsProportianalTo( FacetOutput.TheObjects[i]->normal))
		{	return FacetOutput.TheObjects[i];
		}
	}
	if (!IsAValidCandidateForNormalOfAKillerFacet(NewFacetNormal,directions,CurrentIndex)){return 0;}
	Facet* newFacet= new Facet;
	newFacet->normal.Assign(NewFacetNormal);
	FacetOutput.AddObjectOnTop(newFacet);
	return newFacet;
}


Facet* Facet::RayTrace(int EdgeIndex, root& direction,roots& directions,
											 int CurrentIndex, CombinatorialChamber* owner)
{
	static root NewFacetNormal, tempNormal; NewFacetNormal.dimension=root::AmbientDimension;
	tempNormal.dimension=root::AmbientDimension;
	static Rational a1,a2, b;
	this->GetNormal(owner,tempNormal);
	root::RootScalarEuclideanRoot(direction,tempNormal,a1);
	NewFacetNormal.Assign(Boundaries.TheObjects[EdgeIndex]);
	root::RootScalarEuclideanRoot(direction,Boundaries.TheObjects[EdgeIndex],a2);
	assert(!(a1.IsEqualToZero() && a2.IsEqualToZero()));
	if (RationalEqualsRational(a1,RZero))
	{return 0;}
	RationalDivRational(a2,a1,b);
	b.MinusRational();
	root::RootPlusRootTimesScalar(NewFacetNormal,tempNormal,b,NewFacetNormal);
/*	for (int i=0;i<owner->ExternalWalls->size;i++)
	{
		if (NewFacetNormal.IsProportianalTo(owner->ExternalWalls->TheObjects[i]->normal))
		{
			return 0;
		}
	}*/
	if (!owner->HasHSignVertex(NewFacetNormal,1)){return 0;}
	if (!owner->HasHSignVertex(NewFacetNormal,-1)){return 0;}
	if (!IsAValidCandidateForNormalOfAKillerFacet(NewFacetNormal,directions,CurrentIndex)){return 0;}
	Facet* newFacet= new Facet;
	newFacet->normal.Assign(NewFacetNormal);
	return newFacet;
}

bool Facet::IsAnOwner(CombinatorialChamber *owner,
											CombinatorialChamber *&TheFirstOtherOwner, bool& IsAPlusOwner)
{	for (int i=0;i<this->Owners.size;i++)
	{	if (this->Owners.TheObjects[i].PlusOwner==owner)
		{	IsAPlusOwner=true;
			TheFirstOtherOwner= this->Owners.TheObjects[i].MinusOwner;
			return true;
		}
		if (this->Owners.TheObjects[i].MinusOwner==owner)
		{	IsAPlusOwner=false;
			TheFirstOtherOwner= this->Owners.TheObjects[i].PlusOwner;
			return true;
		}
	}
	return false;
}

bool Facet::IsInFacetNoBoundaries(root &point)
{	Rational tempRat;
	root::RootScalarEuclideanRoot(point,this->normal,tempRat);
	return (tempRat.IsEqualTo(RZero));
}

bool Facet::IsAValidCandidateForNormalOfAKillerFacet(root& normalCandidate,
																										 roots &directions, int CurrentIndex)
{	static roots WallBasis;
	WallBasis.size=0;
	for (int i=0;i<=CurrentIndex;i++)
	{	Rational tempRat;
		root::RootScalarEuclideanRoot(directions.TheObjects[i],normalCandidate,tempRat);
		if (tempRat.IsEqualTo(RZero))
		{	WallBasis.AddRoot(directions.TheObjects[i]);
		}
	}
	if (WallBasis.size<root::AmbientDimension-1)
	{return false;}
	int tempI=WallBasis.GetRankOfSpanOfElements();
	if (WallBasis.GetRankOfSpanOfElements()<root::AmbientDimension-1)
	{return false;}
	assert(tempI==root::AmbientDimension-1);
	return true;
}

void ComplexQN::MultiplyByBasicComplex(BasicComplexNumber & b)
{
}

void ComplexQN::CopyFrom(const ComplexQN& q)
{	this->NumVars = q.NumVars;
	for (int i=0;i<this->NumVars;i++)
	{	this->Exponent[i].Assign(q.Exponent[i]);
	}
	this->Coefficient.Assign(q.Coefficient);
}

void ComplexQN::MakePureQN(Rational* expArg, int NumVars)
{	this->NumVars=NumVars;
	this->MakeConst(ROne,NumVars);
	for(int i=0;i<this->NumVars;i++)
	{	this->Exponent[i].Assign(expArg[i]);
	}
	this->Simplify();
	this->CheckCoefficient();
}

void ComplexQN::MakePureQN(int NumVars,int NonZeroIndex, Rational&coeff, Rational&ConstExp, Rational& Exp)
{ this->NumVars =NumVars;
	for(int i=0;i<this->NumVars;i++)
	{ this->Exponent[i].MakeZero();
	}
	this->Coefficient.MakeBasicComplex(coeff,ConstExp) ;
	this->Exponent[NonZeroIndex].Assign(Exp);
	this->Simplify();
}

bool ComplexQN::ExponentIsEqualToZero()
{	this->CheckCoefficient();
	for (int i=0;i<this->NumVars;i++)
	{ if (!this->Exponent[i].IsEqualTo(RZero))
			return false;
	}
	return true;
}

void ComplexQN::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

bool ComplexQN::HasSameExponent(ComplexQN& q)
{	for (int i=0;i<this->NumVars;i++)
	{ if (!this->Exponent[i].IsEqualTo(q.Exponent[i]))
		{ return false;
		}
	}
	return true;
}

void ComplexQN::MultiplyByRational(Rational& r)
{	this->Coefficient.MultiplyByRational(r);
}

void ComplexQN::MultiplyByLargeRational(LargeRational& r)
{	this->Coefficient.MultiplyByLargeRational(r);
}

void ComplexQN::operator =(const ComplexQN& q)
{ this->CopyFrom(q);
	this->CheckCoefficient();
}

bool ComplexQN::operator ==(ComplexQN& q)
{ this->CheckCoefficient();
	for (int i=0;i<this->NumVars+1;i++)
	{ if (!(this->Exponent[i].IsEqualTo(q.Exponent[i]))){return false;}
	}
	return true;
}

void ComplexQN::MultiplyBy(ComplexQN &q)
{	for (int i=0;i<this->NumVars;i++)
	{	this->Exponent[i].Add(q.Exponent[i]);
	}
	this->Coefficient.MultiplyBy(q.Coefficient);
	this->Simplify();
}

void ComplexQN::MakeConst(Rational& Coeff, int NumVars)
{this->Coefficient.AssignRational(Coeff);
	this->NumVars =NumVars;
	for (int i=0;i<this->NumVars;i++)
	{	this->Exponent[i].MakeZero();
	}
}

void CompositeComplexQN::MakeConst(Rational& Coeff, int numVars)
{	ComplexQN q(numVars);
	this->size =0;
	this->NumVariables=numVars;
	q.MakeConst(Coeff,numVars);
	this->AddObjectOnTop(q);
}

/*bool CompositeComplexQN::ElementHasPlusInFront()
{	if (this->NumNonZeroElements()>1)
	{return true;}
	else
	{	for (int i=0;i<this->size;i++)
		{	if (!this->TheObjects[i].Coefficient.IsEqualToZero())
			{	return this->TheObjects[i].Coefficient.ElementHasPlusInFront();
			}
		}
	}
	return true;
}
*/
void CompositeComplexQN::ElementToString(std::string& output)
{	std::stringstream out;
	int NumNonZero = this->NumNonZeroElements();
	std::string tempS;
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		if (tempS[0]!='0')
		{ if (tempS[0]!='-')
			{ out<<"+";
			}
			out<<tempS;
		}
	}
	output=out.str();
	if (output[0]=='+')
	{ output.erase(0,1);
	}
	if (NumNonZero >1)
	{ output.append(")");
		output.insert(0,"(");
	}
}

void CompositeComplexQN::AddComplexQN(ComplexQN& q)
{	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i].HasSameExponent(q))
		{	this->TheObjects[i].Coefficient.Add(q.Coefficient);
			if (this->TheObjects[i].Coefficient.IsEqualToZero())
			{	this->PopIndexSwapWithLast(i);
			}
			return;
		}
	}
	this->AddObjectOnTop(q);
}

void CompositeComplexQN::Add(CompositeComplexQN &q)
{ for (int i=0;i<q.size;i++)
	{	this->AddComplexQN(q.TheObjects[i]);
	}
}

bool CompositeComplexQN::ComputeDebugString()
{
	this->DebugString.clear();
	this->ElementToString(this->DebugString);
	return true;
}

void CompositeComplexQN::MultiplyByComplexQN(ComplexQN &q)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyBy(q);
	}
}

void CompositeComplexQN::LinearSubstitution(MatrixRational& TheSub)
{	static CompositeComplexQN Accum(TheSub.NumRows-1);
	static ComplexQN tempQ(TheSub.NumRows-1);
	Accum.size=0;
	Accum.NumVariables= TheSub.NumRows-1;
	for(int i=0;i<this->size;i++)
	{	tempQ.CopyFrom(this->TheObjects[i]);
		tempQ.LinearSubstitution(TheSub);
		Accum.AddComplexQN(tempQ);
	}
	this->Assign(Accum);
}

void CompositeComplexQN::MakePureQN(Rational* expArg, int numVars)
{	this->NumVariables=numVars;
	this->size=0;
	static ComplexQN tempQ(numVars);
	tempQ.MakePureQN(expArg,numVars);
	this->AddObjectOnTop(tempQ);
}


void CompositeComplexQN::MakePureQN(Rational* expArg,int numVars, Rational& coeff)
{	this->NumVariables=numVars;
	this->MakePureQN(expArg,numVars);
	this->TheObjects[0].Coefficient.AssignRational(coeff);
}

void CompositeComplexQN::MakePureQN(int numVars,int NonZeroIndex, Rational&coeff, Rational&Exp)
{ this->MakePureQN(numVars,NonZeroIndex,Exp,RZero,coeff);
}

void CompositeComplexQN::MakePureQN(int numVars,int NonZeroIndex, Rational&coeffExp, Rational&ConstExp,
														 Rational& Coeff)
{ this->size=0;
	this->NumVariables=numVars;
	static ComplexQN tempQ(numVars);
	tempQ.MakePureQN(numVars,NonZeroIndex,Coeff,ConstExp,coeffExp);
	this->AddComplexQN(tempQ);
}

void CompositeComplexQN::MakePureQN(Rational& constExp,int numVars, Rational&coeff)
{ this->MakePureQN(numVars,0,RZero,constExp,coeff);
}


void CompositeComplexQN::MultiplyByRational(Rational &r)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByRational(r);
	}
}

void CompositeComplexQN::MultiplyByLargeRational(LargeRational &r)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByLargeRational(r);
	}
}

bool CompositeComplexQN::IsEqualToZero()
{ for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i].IsEqualToZero())
		{	return false;
		}
	}
	return true;
}

void CompositeComplexQN::Simplify()
{ for( int i=0;i<this->size;i++)
	{ this->TheObjects[i].Simplify();
	}
}

int CompositeComplexQN::NumNonZeroElements()
{	int result=0;
	for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i].IsEqualToZero())
		{	result++;
		}
	}
	return result;
}

bool CompositeComplexQN::IsEqualTo(CompositeComplexQN&q)
{	CompositeComplexQN tempQN(q.NumVariables) ;
	tempQN.Assign(q);
	tempQN.MultiplyByRational(RMOne);
	tempQN.Add(*this);
	return tempQN.IsEqualToZero();
}

void CompositeComplexQN::DivideByRational(Rational& r)
{
	Rational tempRat;
	tempRat.Assign(r);
	tempRat.Invert();
	this->MultiplyByRational(tempRat);
}

void CompositeComplexQN::Assign(const CompositeComplexQN &q)
{	this->size=0;
	this->NumVariables= q.NumVariables;
	for (int i=0;i<q.size;i++)
	{	this->AddObjectOnTop(q.TheObjects[i]);
	}
}

void CompositeComplexQN::MultiplyBy(CompositeComplexQN& q)
{	static CompositeComplexQN tempQ(q.NumVariables);
	static CompositeComplexQN Accum(q.NumVariables);
	Accum.size=0;
	for (int i=0;i<q.size;i++)
	{	tempQ.Assign(*this);
		tempQ.MultiplyByComplexQN(q.TheObjects[i]);
		Accum.Add(tempQ);
	}
	this->Assign(Accum);
}

/*void ComplexQN::DecreaseNumVariables(int increment, ComplexQN& output)
{	output.CopyFrom(*this);
	output.size-=increment;
}

void ComplexQN::DecreaseNumVariables(int increment, CompositeComplexQN& output)
{ ComplexQN tempQ(this->size-2);
	this->DecreaseNumVariables(increment,tempQ);
	output.size=0;
	output.AddComplexQN(tempQ);
}

void ComplexQN::DecreaseNumVariables(int increment)
{ this->size-=increment;
}*/

/*void ComplexQN::AddBasicComplexNumber(BasicComplexNumber& b)
{ for(int i=0;i<this->Coefficients.size;i++)
	{ if (this->Coefficients.TheObjects[i].Exp.IsEqualTo(b.Exp))
		{ this->Coefficients.TheObjects[i].Coeff.Add(b.Coeff);
			this->Simplify();
			return;
		}
	}
	this->Coefficients.AddObjectOnTop(b);
	this->Simplify();
} */

void ComplexQN::ElementToString(std::string& output)
{	if (this->IsEqualToZero()){output.clear(); output.append("0"); return;}
	std::stringstream out;
	std::string tempS;
	this->Coefficient.ElementToString(tempS);
	if (!this->ExponentIsEqualToZero())
	{	if (tempS=="1")
		{ tempS.clear();
		}
		if (tempS=="-1")
		{ tempS="-";
		}
		out<< tempS<<"e^{2i \\pi(";
		tempS.clear();
		this->LinPartToString(tempS);
		out<< tempS<<")}";
	}
	else
	{ out<<tempS;
	}
	output= out.str();
}

void ComplexQN::LinPartToString(std::string& output)
{	output.clear();
	std::stringstream out;
	std::string tempS;
	for (int i=0;i<this->NumVars;i++)
	{	if (!this->Exponent[i].IsEqualTo(RZero))
		{	if (!this->Exponent[i].IsEqualTo(ROne))
			{ tempS.clear();
				if (!this->Exponent[i].IsEqualTo(RMOne))
				{	this->Exponent[i].ElementToString(tempS);
				}
				else
				{ tempS.assign("-");
				}
				if (tempS[0]!='-')
				{out<<"+";}
				out<<tempS;
			}
			else
			{	out<<"+";
			}
			out<<PolyFormatLocal.alphabet[i];
		}
	}
	output= out.str();
	if (output.size()!=0)
	{ if( output[0]=='+')
		{ output.erase(0,1);
		}
	}
}

void ComplexQN::LinearSubstitution(MatrixRational& TheSub)
{	static Rational tempExponent[MaxRank+1];
	static Rational tempRat;
	static BasicComplexNumber tempBC;
	tempBC.Coeff.AssignRational(ROne);
	tempBC.Exp.Assign(RZero);
	assert(TheSub.NumCols==this->NumVars);
	for (int i=0;i<TheSub.NumRows-1;i++)
	{ tempExponent[i].MakeZero();
	}
	for (int i=0; i<this->NumVars;i++)
	{	RationalTimesRational(TheSub.elements[0][i], this->Exponent[i],tempRat);
		tempBC.Exp.Add(tempRat);
		for (int j=1;j<TheSub.NumRows;j++)
		{	RationalTimesRational(TheSub.elements[j][i],this->Exponent[i],tempRat);
			tempExponent[j-1].Add(tempRat);
		}
	}
	this->NumVars= TheSub.NumRows-1;
	for (int i=0;i<this->NumVars;i++)
	{	this->Exponent[i].Assign(tempExponent[i]);
	}
	this->Coefficient.MultiplyByBasicComplex(tempBC);
	this->Simplify();
}

void ComplexQN::Simplify()
{	this->Coefficient.Simplify();
	for (int i=0;i<this->NumVars;i++)
	{ this->Exponent[i].AssignFracValue();
	}
}

bool ComplexQN::IsBasicComplex()
{	if (this->Coefficient.size==0){return true;}
	return (this->ExponentIsEqualToZero());
}

/*void ComplexQN::MakePureQN(Rational* expArg, int Size)
{	this->Coefficient.Assign(ROne);
	this->size=Size;
	for (int i=0;i<this->size;i++)
	{ this->Exponent[i].Assign(expArg[i]);
	}
	this->Simplify();
}*/

bool ComplexQN::IsEqualToZero()
{	return (this->Coefficient.size==0);
}


void PolynomialRationalCoeff::MakeLinPolyFromInt(int x1,int x2, int x3,int x4, int x5)
{ root tempRoot;
	tempRoot.InitFromIntegers(x1,x2,x3,x4,x5);
	this->MakeLinPolyFromRoot(tempRoot);
}


void PolynomialRationalCoeff::operator=(PolynomialRationalCoeff& right)
{ this->CopyFromPoly(right);
}

void PolynomialRationalCoeff::MakePolyExponentFromIntRoot(intRoot&r)
{	this->ClearTheObjects();
	this->NumVars=root::AmbientDimension;
	static Monomial<Rational> tempM;
	tempM.init(root::AmbientDimension);
	tempM.Coefficient.Assign(ROne);
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempM.degrees[i]=(short) r.elements[i];
	}
	this->AddMonomial(tempM);
}

int PolynomialRationalCoeff::SizeWithoutDebugString()
{ int Accum=0;
	Accum+=this->HashedListBasicObjects<Monomial<Rational> >::SizeWithoutObjects();
	Accum+=	sizeof(this->NumVars);
	for (int i=0;i<this->ActualSize;i++)
	{ Accum+=this->TheActualObjects[i].SizeWithoutCoefficient()+sizeof(Rational);
	}
	return Accum;
}

void PolynomialRationalCoeff::TimesInteger(int x)
{ if (x==0)
	{ this->Nullify(this->NumVars);
		return;
	}
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Coefficient.MultiplyByInteger(x);
	}
}

void PolynomialRationalCoeff::DivByInteger(int x)
{	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Coefficient.DivideByInteger(x);
	}
}

void PolynomialRationalCoeff::AssignIntegerPoly(IntegerPoly& p)
{	this->Nullify(p.NumVars);
	this->SetActualSizeAtLeastExpandOnTop(p.size);
	static Monomial<Rational> tempM;
	tempM.initNoDegreesInit(this->NumVars);
	for (int i=0;i<p.size;i++)
	{	for (int j=0;j<this->NumVars;j++)
		{ tempM.degrees[j]=p.TheObjects[i].degrees[j];
		}
		tempM.Coefficient.AssignInteger(p.TheObjects[i].Coefficient.value);
		this->AddObjectOnTopHash(tempM);
	}
}

void PolynomialRationalCoeff::MakePolyFromDirectionAndNormal
				(root& direction, root& normal, Rational& Correction)
{	Rational tempRat;
	assert(direction.dimension==root::AmbientDimension);
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	this->ClearTheObjects();
	this->NumVars=root::AmbientDimension;
	static Monomial<Rational> tempM;
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempM.MakeNVarFirstDegree(i,root::AmbientDimension, normal.coordinates[i]);
		tempM.Coefficient.DivideBy(tempRat);
		this->AddMonomial(tempM);
	}
	this->AddConstant(Correction);
}
/*
void QuasiPolynomialPointers::IntegrateDiscreteInDirectionFromZeroTo
		 												 (root& direction, PolynomialRationalCoeff& EndPoint,
															QuasiPolynomial& output,
															QuasiPolynomial& input,
															PrecomputedQuasiPolynomialIntegralPointersKillOnExit&
															                                    PrecomputedDiscreteIntegrals)
{
	if (input.TheMonomials.size==0) {return;}
	int NumVars= input.TheMonomials.TheObjects[0]->NumVariables;
	PolynomialPointersKillOnExit<CompositeComplexQN> TSubstitution;
	TSubstitution.initAndCreateNewObjects(NumVars+1);
	for (int i=0; i<NumVars;i++)
	{
		Monomial<CompositeComplexQN> TempMonomial;
		TempMonomial.MakeOneLetterMonomialFirstDegree(i,NumVars+1,CQNOne);
		TSubstitution.TheObjects[i]->AddMonomial(TempMonomial);
		direction.coordinates[i].MinusRational();
		CompositeComplexQN tempQN(root::AmbientDimension,direction.coordinates[i]);
		TempMonomial.MakeOneLetterMonomialFirstDegree(NumVars,NumVars+1,tempQN);
		TSubstitution.TheObjects[i]->AddMonomial(TempMonomial);
		TSubstitution.TheObjects[i]->ComputeDebugString();
		direction.coordinates[i].MinusRational();
	}
	TSubstitution.TheObjects[NumVars]->TheMonomials.initAndCreateNewObjects(1);
	TSubstitution.TheObjects[NumVars]->TheMonomials.TheObjects[0]
	             ->MakeOneLetterMonomialFirstDegree(NumVars,NumVars+1,CQNOne);
	TSubstitution.TheObjects[NumVars]->ComputeDebugString();
	Polynomial<CompositeComplexQN> Accum;
	Accum.CopyFrom(input);
	Accum.IncreaseNumVariables(1);
	Accum.ComputeDebugString();
	Accum.Substitution(TSubstitution,Accum);
	Accum.ComputeDebugString();
	output.TheMonomials.KillAllElements();
	PolynomialPointersKillOnExit<CompositeComplexQN> UpperBoundSubstitution;
	UpperBoundSubstitution.initAndCreateNewObjects(NumVars);
	UpperBoundSubstitution.TheObjects[0]->CopyFrom(EndPoint);
	for (int i=0;i<Accum.TotalDegree()+1;i++)
	{
		Polynomial<CompositeComplexQN> TempPoly;
		Polynomial<CompositeComplexQN> TempPoly2;
		Polynomial<CompositeComplexQN> TempPoly3;
		Accum.ComponentInFrontOfVariableToPower(NumVars,i,TempPoly);
		TempPoly.ComputeDebugString();
		TempPoly2.CopyFrom(*PrecomputedDiscreteIntegrals.TheObjects[i]);
		TempPoly2.IncreaseNumVariables(NumVars-1);
		TempPoly2.ComputeDebugString();
		TempPoly2.Substitution(UpperBoundSubstitution,TempPoly3);
		TempPoly3.ComputeDebugString();
		TempPoly.MultiplyBy(TempPoly3);
		TempPoly.ComputeDebugString();
		output.ComputeDebugString();
		output.AddPolynomial(TempPoly);
		output.ComputeDebugString();
	}
}*/

void PolynomialsRationalCoeff::ComputeDiscreteIntegrationUpTo(int d)
{	this->SetSizeExpandOnTopNoObjectInit(d);
	this->TheObjects[0].MakeNVarDegOnePoly(1,0,ROne,ROne);
	PolynomialRationalCoeff Accum,tempPoly;
	for (int i=1;i<d;i++)
	{	Accum.ClearTheObjects();
		Accum.NumVars=1;
		for (int j=0;j<i;j++)
		{	tempPoly.CopyFromPoly(this->TheObjects[j]);
			int tempI=NChooseK(i+1,j);
			Rational tempNum(-tempI,1);
			tempPoly.TimesConstant(tempNum);
			Accum.AddPolynomial(tempPoly);
		}
		tempPoly.MakeNVarDegOnePoly(1,0,ROne,ROne);
		tempPoly.RaiseToPower(i+1);
		Accum.AddPolynomial(tempPoly);
		Rational tempRat(1,i+1);
		Accum.TimesConstant(tempRat);
		this->TheObjects[i].CopyFromPoly(Accum);
	}
}

void PolynomialsRationalCoeff::MakeSubAddExtraVarForIntegration(root& direction)
{	this->SetSizeExpandOnTopNoObjectInit(root::AmbientDimension);
	for (int i=0;i<root::AmbientDimension;i++)
	{	Rational tempRat;tempRat.Assign(direction.coordinates[i]); tempRat.MinusRational();
		this->TheObjects[i].MakeNVarDegOnePoly
						(root::AmbientDimension+1,i,root::AmbientDimension,ROne,tempRat);
	}
}

void PolynomialsRationalCoeff::MakeSubNVarForOtherChamber
         (root& direction,root& normal, Rational& Correction)
{	this->SetSizeExpandOnTopNoObjectInit(root::AmbientDimension);
	static PolynomialRationalCoeff TempPoly;
	TempPoly.MakePolyFromDirectionAndNormal(direction,normal, Correction);
	for (int i=0;i<root::AmbientDimension;i++)
	{	static PolynomialRationalCoeff TempPoly2;
		TempPoly2.CopyFromPoly(TempPoly);
		TempPoly2.TimesConstant(direction.coordinates[i]);
		TempPoly2.TimesConstant(RMOne);
		this->TheObjects[i].MakeNVarDegOnePoly(root::AmbientDimension,i,ROne);
		this->TheObjects[i].AddPolynomial(TempPoly2);
	}
}

void PolynomialsRationalCoeff::MakeLinearSubOnLastVariable
            (short NumVars,PolynomialRationalCoeff& LastVarSub)
{ this->SetSizeExpandOnTopNoObjectInit(NumVars);
	for (int i=0;i<NumVars-1;i++)
	{ this->TheObjects[i].MakeNVarDegOnePoly(NumVars,i,ROne);
	}
	this->TheObjects[NumVars-1].CopyFromPoly(LastVarSub);
}

PolynomialOutputFormat::PolynomialOutputFormat()
{ this->MakeRegularAlphabet();
	this->ExtraLinesCounterLatex=0;
}

void PolynomialOutputFormat::MakeRegularAlphabet()
{	this->alphabet[0]='a';
	this->alphabet[1]='b';
	this->alphabet[2]='c';
	this->alphabet[3]='d';
	this->alphabet[4]='e';
	this->alphabet[5]='f';
	this->alphabet[6]='g';
	this->alphabet[7]='h';
	this->alphabet[8]='i';
	this->alphabet[9]='j';
	this->alphabet[10]='k';
	this->alphabet[11]='l';
	this->alphabet[12]='m';
	this->alphabet[13]='n';
	this->alphabet[14]='o';
	this->alphabet[15]='p';
	this->alphabet[16]='q';
	this->alphabet[17]='r';
	this->alphabet[18]='s';
	this->alphabet[19]='t';
	this->alphabet[20]='u';
	this->alphabet[21]='v';
	this->alphabet[22]='w';
	this->alphabet[23]='x';
	this->alphabet[24]='y';
	this->alphabet[25]='z';
	this->cutOffString=false;
	this->cutOffSize=500;
}

void PolynomialOutputFormat::MakeAlphabetxi()
{	this->alphabet[0]="x_1";
	this->alphabet[1]="x_2";
	this->alphabet[2]="x_3";
	this->alphabet[3]="x_4";
	this->alphabet[4]="x_5";
	this->alphabet[5]="x_6";
	this->alphabet[6]="x_7";
	this->alphabet[7]="x_8";
	this->alphabet[8]="x_9";
	this->alphabet[9]="x_{10}";
	this->alphabet[10]="x_{11}";
	this->alphabet[11]="x_{12}";
	this->alphabet[12]="x_{13}";
	this->alphabet[13]="x_{14}";
	this->alphabet[14]="x_{15}";
	this->alphabet[15]="x_{16}";
	this->alphabet[16]="x_{17}";
	this->alphabet[17]="x_{18}";
	this->alphabet[18]="x_{19}";
	this->alphabet[19]="x_{20}";
	this->alphabet[20]="x_{21}";
	this->alphabet[21]="x_{22}";
	this->alphabet[22]="x_{23}";
	this->alphabet[23]="x_{24}";
	this->alphabet[24]="x_{25}";
	this->alphabet[25]="x_{26}";
	this->cutOffString=false;
	this->cutOffSize=500;
}

void PolynomialOutputFormat::MakeAlphabetyi()
{	this->alphabet[0]="y_1";
	this->alphabet[1]="y_2";
	this->alphabet[2]="y_3";
	this->alphabet[3]="y_4";
	this->alphabet[4]="y_5";
	this->alphabet[5]="y_6";
	this->alphabet[6]="y_7";
	this->alphabet[7]="y_8";
	this->alphabet[8]="y_9";
	this->alphabet[9]="y_{10}";
	this->alphabet[10]="y_{11}";
	this->alphabet[11]="y_{12}";
	this->alphabet[12]="y_{13}";
	this->alphabet[13]="y_{14}";
	this->alphabet[14]="y_{15}";
	this->alphabet[15]="y_{16}";
	this->alphabet[16]="y_{17}";
	this->alphabet[17]="y_{18}";
	this->alphabet[18]="y_{19}";
	this->alphabet[19]="y_{20}";
	this->alphabet[20]="y_{21}";
	this->alphabet[21]="y_{22}";
	this->alphabet[22]="y_{23}";
	this->alphabet[23]="y_{24}";
	this->alphabet[24]="y_{25}";
	this->alphabet[25]="y_{26}";
	this->cutOffString=false;
	this->cutOffSize=500;
}

int PolynomialRationalCoeff::FindGCMCoefficientDenominators()
{	int result=1;
	for (int i=0;i<this->size;i++)
	{	int tempI=gcd(result,this->TheObjects[i].Coefficient.den);
		result= result*this->TheObjects[i].Coefficient.den/tempI;
	}
	return result;
}
/*
void PolynomialRationalCoeff::IntegrateDiscreteFromZeroTo
							(int IntegrationVariableIndex, PolynomialRationalCoeff &EndPoint,
							 PolynomialRationalCoeffPointers& PrecomputedDiscreteIntegration)
{
	PolynomialRationalCoeff Accum, tempP1;
	int tempI=this->FindMaxPowerOfVariableIndex( IntegrationVariableIndex);
	for (int i=0;i<tempI;i++)
	{
		this->ComponentInFrontOfVariableToPower(IntegrationVariableIndex,i,tempP1);


	}
}*/

void QuasiMonomial::RationalLinearSubstitution(QPSub& TheSub, QuasiPolynomial& output)
{	if (this->Coefficient.IsEqualToZero())
	{	output.ClearTheObjects();
		output.NumVars= TheSub.TheQNSub.NumRows;
		return;
	}
	static PolynomialRationalCoeff tempP;
	static Monomial<Rational> tempM;
	tempM.initNoDegreesInit(this->NumVariables);
	tempM.Coefficient.Assign(ROne);
	for (int i=0;i<this->NumVariables;i++)
	{ tempM.degrees[i]= this->degrees[i];
	}
	tempM.Substitution(TheSub.RationalPolyForm,tempP,TheSub.TheQNSub.NumRows-1);
	static QuasiNumber tempQ(TheSub.TheQNSub.NumRows);
	tempQ.Assign(this->Coefficient);
	tempQ.LinearSubstitution(TheSub);
	output.AssignPolynomialRationalCoeff(tempP);
	output.TimesConstant(tempQ);
}

//We are integrating with respect to the last variable always!

void QuasiMonomial::IntegrateDiscreteInDirectionFromZeroTo
					(QPSub& EndPointSub, QPSub& DirectionSub,
					QuasiPolynomial &output,
					PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{	static QuasiPolynomial SubstitutedQP;
	this->RationalLinearSubstitution(DirectionSub,SubstitutedQP);
	SubstitutedQP.IntegrateDiscreteFromZeroTo(EndPointSub,output,PrecomputedDiscreteIntegrals);
}

//We are integrating with respect to the last variable always!
void QuasiMonomial::IntegrateDiscreteFromZeroTo
										(QPSub& EndPointSub, QuasiPolynomial &output,
										 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{	output.ClearTheObjects(); output.NumVars=root::AmbientDimension;
	for (int i=0; i<this->Coefficient.size;i++)
	{	static Rational coeffInFrontOft;
		this->Coefficient.TheObjects[i].GetCoeffInFrontOfLast(coeffInFrontOft);
		static QuasiPolynomial tempQP, tempQP2;
		PrecomputedDiscreteIntegrals.GetQuasiPolynomialIntegral
			(coeffInFrontOft.num,coeffInFrontOft.den,
			 this->degrees[root::AmbientDimension],tempQP);
		tempQP.RationalLinearSubstitution(EndPointSub,tempQP);
		static QuasiNumber tempQN(EndPointSub.TheQNSub.NumRows);
		tempQN.size=0;
		tempQN.NumVariables= root::AmbientDimension+1;
		tempQN.AddBasicQuasiNumber(this->Coefficient.TheObjects[i]);
	//	tempQN.DecreaseNumVars(1);
		tempQP.TimesConstant(tempQN);
//		bool tempBool = ("-1/4c-1/4c^2"==tempQP.DebugString);
		output.AddPolynomial(tempQP);
	}
	static QuasiMonomial tempQM;
	tempQM.MakeConstantMonomial(root::AmbientDimension,QNOne);
	for (int i=0;i<root::AmbientDimension;i++ )
	{ tempQM.degrees[i]=this->degrees[i];}
	output.MultiplyByMonomial(tempQM);
}

void PrecomputedQuasiPolynomialIntegral::ComputeValue
				(PolynomialsRationalCoeff& PreComputedBernoulli)
{	this->Value.ClearTheObjects();
	this->Value.NumVars=1;
	QuasiPolynomial Accum, tempQP;
	PolynomialRationalCoeff tempP1,tempP2;
	PolynomialsRationalCoeff EndPtSub;
	PolynomialRationalCoeff EndPoint;
	QuasiNumber tempQN(1); tempQN.MakeConst(RZero,1);
	Accum.MakeNVarConst(1,tempQN);
	Rational tempRat(this->Den,1), tempRat2(this->Num,1);
	tempP1.MakeNVarDegOnePoly(1,0, tempRat,tempRat2);
	tempP1.RaiseToPower(this->degree);
	for (int i=0;i<this->Den;i++)
	{	QuasiPolynomial Accum2;
		tempRat.init(1,this->Den);
		if (i<this->Num)
		{	tempRat2.init(-i-this->Den,this->Den);
		}else
		{ tempRat2.init(-i,this->Den);
		}
		EndPoint.MakeNVarDegOnePoly(1,0,tempRat,tempRat2);
		EndPtSub.MakeLinearSubOnLastVariable(1,EndPoint);
		for (int k=0;k<tempP1.size;k++)
		{ PreComputedBernoulli.TheObjects[tempP1.TheObjects[k].degrees[0]]
													.Substitution(EndPtSub,tempP2,1);
			tempP2.TimesConstant(tempP1.TheObjects[k].Coefficient);
			tempQP.AssignPolynomialRationalCoeff(tempP2);
			Accum2.AddPolynomial(tempQP);
		}
		tempQP.MakeTauknp(i,this->Den);
		Accum2.MultiplyBy(tempQP);
		Accum.AddPolynomial(Accum2);
	}
	this->Value.CopyFromPoly(Accum);
}

void PrecomputedQuasiPolynomialIntegral::operator=
				(PrecomputedQuasiPolynomialIntegral& right)
{ this->degree= right.degree;
	this->Den= right.Den;
	this->Num= right.Num;
	this->Value.Assign(right.Value);
}

void QuasiPolynomial::operator=(QuasiPolynomial& right)
{ this->CopyFromPoly(right);
}


void QuasiPolynomial::AssignPolynomialRationalCoeff(PolynomialRationalCoeff& p)
{	this->Nullify(p.NumVars);
	static QuasiMonomial tempM;
	tempM.initNoDegreesInit(this->NumVars);
	for (int i=0;i<p.size;i++)
	{	for (int j=0;j<this->NumVars;j++)
		{ tempM.degrees[j]=p.TheObjects[i].degrees[j];
		}
		tempM.Coefficient.MakeConst(p.TheObjects[i].Coefficient, this->NumVars);
		this->AddObjectOnTopHash(tempM);
	}
}

void QuasiPolynomial::AssignPolynomialLargeRational(PolynomialLargeRational& p)
{	this->Nullify(p.NumVars);
	static QuasiMonomial tempM;
	tempM.initNoDegreesInit(this->NumVars);
	for (int i=0;i<p.size;i++)
	{	for (int j=0;j<this->NumVars;j++)
		{ tempM.degrees[j]=p.TheObjects[i].degrees[j];
		}
		tempM.Coefficient.MakeConst(p.TheObjects[i].Coefficient, this->NumVars);
		this->AddObjectOnTopHash(tempM);
	}
}


void QuasiPolynomial::IntegrateDiscreteInDirectionFromOldChamber
		              (root &direction,root& normal,QuasiPolynomial& OldChamberPoly,
									 QuasiPolynomial &output,
									 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{ static QPSub EndPointSub,DirectionSub, OtherChamberSub;
	static Rational tempRat;
	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	static QuasiPolynomial Accum;
//	static bool tempB=false;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	tempRoot.Assign(normal);
	tempRoot.DivByRational(tempRat);
	int Den= tempRoot.FindLCMDenominators();
	Accum.ClearTheObjects();
	Accum.NumVars=root::AmbientDimension;
	DirectionSub.MakeSubAddExtraVarForIntegration(direction);
	for (int i=0;i<Den;i++)
	{ tempRat.init(i,Den);
		EndPointSub.MakeLinearSubIntegrand(normal,direction,tempRat);
		static	QuasiPolynomial tempQP, tempQP1, tempQP2, tempQP3, tempQP4;
		this->IntegrateDiscreteInDirectionFromZeroTo(EndPointSub,DirectionSub,
		                                             tempQP, PrecomputedDiscreteIntegrals);
//		tempQP.ComputeDebugString();
		if (tempQP.IsEqualToZero())
		{ tempRat.init(i,Den);
		}
		else
		{ tempRat.init(Den+i,Den);
		}
		OtherChamberSub.MakeSubNVarForOtherChamber(direction,normal,tempRat);
		OldChamberPoly.RationalLinearSubstitution(OtherChamberSub,tempQP1);
		tempQP.AddPolynomial(tempQP1);
		static QuasiNumber tempQ;
		tempQ.MakeFromNormalAndDirection(normal,direction,i, ROne);
		tempQP.TimesConstant(tempQ);
		Accum.AddPolynomial(tempQP);
	}
	output.CopyFromPoly(Accum);
}

void QuasiPolynomial::IntegrateDiscreteFromZeroTo
										(QPSub& EndPointSub, QuasiPolynomial &output,
										 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{	static QuasiPolynomial tempQP;
	output.ClearTheObjects();
	output.NumVars=root::AmbientDimension;
	static SortedQPs QPs;
	static QuasiMonomial tempQM;
	QPs.Nullify(root::AmbientDimension);
	tempQM.init(root::AmbientDimension);
	for (int i=0;i<this->size;i++)
	{	static int x;
		static Rational yz;
		for (int j=0;j<root::AmbientDimension;j++)
		{ tempQM.degrees[j]=this->TheObjects[i].degrees[j];
		}
		x= this->TheObjects[i].degrees[root::AmbientDimension];
		for (int j=0;j<this->TheObjects[i].Coefficient.size;j++)
		{	tempQM.Coefficient.MakeZero(root::AmbientDimension);
			this->TheObjects[i].Coefficient.TheObjects[j].GetCoeffInFrontOfLast(yz);
			static BasicQN tempQ;
			tempQ.Assign(this->TheObjects[i].Coefficient.TheObjects[j]);
//			tempQ.ComputeDebugString();
			tempQ.DecreaseNumVars(1);
//			tempQ.ComputeDebugString();
			tempQM.Coefficient.AddBasicQuasiNumber(tempQ);
//			tempQM.ComputeDebugString(PolyFormatLocal);
			QPs.AddToEntry(x,yz.den-1,yz.num,tempQM);
		}
	}
	output.ClearTheObjects(); output.NumVars=root::AmbientDimension;
	for (int i=0;i<QPs.FakeSize;i++)
	{ for (int j=0;j<QPs.TheObjects[i]->FakeSize;j++)
		{ for(int k=0;k<QPs.TheObjects[i]->TheObjects[j]->FakeSize;k++)
			{ PrecomputedDiscreteIntegrals.GetQuasiPolynomialIntegral(k,j+1,i,tempQP);
//				tempQP.ComputeDebugString();
				tempQP.RationalLinearSubstitution(EndPointSub,tempQP);
//				tempQP.ComputeDebugString();
//				QPs.TheObjects[i]->TheObjects[j]->TheObjects[k]->ComputeDebugString();
				tempQP.MultiplyBy(*QPs.TheObjects[i]->TheObjects[j]->TheObjects[k]);
//				tempQP.ComputeDebugString();
				output.AddPolynomial(tempQP);
//				output.ComputeDebugString();
			}
		}
	}
}

void QuasiPolynomial::Nullify(short numVars)
{ this->ClearTheObjects();
	this->NumVars=numVars;
}

void QuasiPolynomial::IntegrateDiscreteInDirectionFromZeroTo
					(QPSub& EndPointSub, QPSub& DirectionSub,
					 QuasiPolynomial &output,
					 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{	static QuasiPolynomial tempQP, Accum;
	Accum.ClearTheObjects();
	Accum.NumVars=root::AmbientDimension;
	for (int i=0;i<this->size;i++)
	{	static QuasiMonomial tempQM;
		tempQM.Assign(this->TheObjects[i]);
//		tempQM.ComputeDebugString(PolyFormatLocal);
		tempQM.RationalLinearSubstitution(DirectionSub,tempQP);
//		tempQM.ComputeDebugString(PolyFormatLocal);
		Accum.AddPolynomial(tempQP);
	}
//	Accum.ComputeDebugString();
	Accum.IntegrateDiscreteFromZeroTo(EndPointSub,output,PrecomputedDiscreteIntegrals);
}

/*void QuasiPolynomial::MakePureQuasiPolynomial( PolynomialRationalCoeff& p, int NumVars)
{	if (p.IsEqualToZero())
	{
		this->MakeNVarConst(NumVars,CQNOne);
		return;
	}
	CompositeComplexQN tempQN;
	tempQN.Mak(p,ROne,NumVars);
	this->MakeNVarConst(NumVars,tempQN);
}*/

void QuasiPolynomial::WriteComplexFormToDebugString()
{ static CompositeComplexQNPoly p;
	p.AssignQP(*this);
	p.ComputeDebugString();
	this->DebugString.assign(p.DebugString);
}

void QuasiPolynomial::Simplify()
{for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Coefficient.Simplify();
	}
}

void QuasiPolynomial::TimesInteger(int x)
{ QuasiNumber tempQN;
	tempQN.AssignInteger(this->NumVars,x);
	this->TimesConstant(tempQN);
}

void QuasiPolynomial::Evaluate(intRoot& values, LargeRational& output)
{ output.MakeZero();
	std::string tempS;
	LargeRational tempLRat, tempLRat2;
	for (int i=0;i<this->size;i++)
	{ tempLRat.AssignRational(ROne);
		for (int j=0; j<this->NumVars;j++)
		{ for (int k=0;k<this->TheObjects[i].degrees[j];k++)
			{ tempLRat.MultiplyByInt(values.elements[j]);
			}
		}
		this->TheObjects[i].Coefficient.Evaluate(values.elements,tempLRat2);
		//tempLRat2.ElementToString(tempS);
		//tempLRat.ElementToString(tempS);
		tempLRat.MultiplyBy(tempLRat2);
		output.Add(tempLRat);
		//output.ElementToString(tempS);
	}
}

void QuasiPolynomial::DivByInteger(int x)
{ Rational tempRat;
	tempRat.init(1,x);
	LargeRational tempRat2;
	tempRat2.AssignRational(tempRat);
	QuasiNumber tempQN;
	tempQN.AssignLargeRational(this->NumVars,tempRat2);
	this->TimesConstant(tempQN);
}

void QuasiPolynomial::RationalLinearSubstitution
												(QPSub& TheSub,QuasiPolynomial& output)
{	static QuasiPolynomial tempQP;
	static QuasiPolynomial Accum;
	Accum.ClearTheObjects();
	Accum.NumVars= TheSub.TheQNSub.NumRows;
	for (int i=0;i<this->size;i++)
	{	static QuasiMonomial tempQM;
		tempQM.Assign(this->TheObjects[i]);
		tempQM.RationalLinearSubstitution(TheSub,tempQP);
		Accum.AddPolynomial(tempQP);
	}
	output.CopyFromPoly(Accum);
}

QuasiPolynomial::QuasiPolynomial()
{	QuasiPolynomial::TotalCreatedPolys++;
	this->CreationNumber=QuasiPolynomial::TotalCreatedPolys;
	GlobalCollectorsPolys.AddObjectOnTop(this);
};

void QuasiPolynomial::MakeTauknp(int k, int n)
{ static QuasiNumber tempQN;
	tempQN.MakePureQN(1,0,ROne,1,k,n);
	this->MakeNVarConst(1,tempQN);
}

void PrecomputedTauknPointersKillOnExit::GetTaukn(int k, int n, CompositeComplexQN& output)
{	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i]->k==k && this->TheObjects[i]->n==n)
		{	output.Assign(this->TheObjects[i]->Taukn);
			return;
		}
	}
	this->ComputeTaukn(k,n,output);
	PrecomputedTaukn* NewMember = new PrecomputedTaukn;
	this->AddObjectOnTop(NewMember);
	NewMember->k=k;
	NewMember->n=n;
	NewMember->Taukn.Assign(output);
}

void PrecomputedTauknPointersKillOnExit::ComputeTaukn(int k, int n, CompositeComplexQN& output)
{	CompositeComplexQN NonConstQN(1), Accum(1);
	Rational tempRat(1,n);
	NonConstQN.MakePureQN(1,0,ROne,tempRat);
	NonConstQN.ComputeDebugString();
	Rational tempRat2(k*(1-n),n);
	Accum.MakePureQN(1,0,RZero,tempRat2,tempRat);
	Accum.ComputeDebugString();
	for (int i=0;i<n;i++)
	{	if (i!=k)
		{	tempRat.init(i,n);
			CompositeComplexQN tempQN(1), tempQN2(1);
			tempQN.MakePureQN(tempRat,1,RMOne);
			tempQN.ComputeDebugString();
			tempQN2.Assign(NonConstQN);
			tempQN2.Add(tempQN);
			tempQN2.ComputeDebugString();
			assert(tempQN2.ComputeDebugString());
			Accum.MultiplyBy(tempQN2);
			assert(Accum.ComputeDebugString());
		}
	}
	output.Assign(Accum);
	assert(output.ComputeDebugString());
}


void PrecomputedQuasiPolynomialIntegrals::
			GetQuasiPolynomialIntegral(int Num, int Den, int degree, QuasiPolynomial& output)
{	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i].Num == Num &&
			  this->TheObjects[i].Den == Den &&
				this->TheObjects[i].degree == degree)
		{	output.CopyFromPoly(this->TheObjects[i].Value);
			return;
		}
	}
	this->SetSizeExpandOnTopNoObjectInit(this->size+1);
	this->TheObjects[this->size].degree=degree;
	this->TheObjects[this->size].Den =Den;
	this->TheObjects[this->size].Num=Num;
	this->TheObjects[this->size].ComputeValue(*this->PreComputedBernoulli);
	output.CopyFromPoly(this->TheObjects[this->size].Value);
}

void CombinatorialChamberCouplePointers::AddCouple(CombinatorialChamber* PlusOwner,
																									 CombinatorialChamber* MinusOwner)
{	this->SetSizeExpandOnTopNoObjectInit(this->size+1);
	this->TheObjects[this->size-1].MinusOwner = MinusOwner;
	this->TheObjects[this->size-1].PlusOwner = PlusOwner;
}

void CombinatorialChamberCouplePointers::Substitute
				(Facet*owner,CombinatorialChamber *Leaves, CombinatorialChamber *Enters)
{	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i].MinusOwner==Leaves)
		{	this->TheObjects[i].MinusOwner=Enters;}
		if (this->TheObjects[i].PlusOwner==Leaves)
		{ this->TheObjects[i].PlusOwner=Enters;}
	}
}

void CombinatorialChamberCouplePointers::RemoveCoupleBothOrders
        (CombinatorialChamber* Owner1,CombinatorialChamber* Owner2)
{	bool KilledOnce=false;
	for (int i=0;i<this->size;i++)
	{	if((this->TheObjects[i].MinusOwner==Owner1
			 &&
			 this->TheObjects[i].PlusOwner==Owner2)||
			 (this->TheObjects[i].MinusOwner==Owner2
			 &&
			 this->TheObjects[i].PlusOwner==Owner1))
		{	this->PopIndexSwapWithLast(i);
			i--;
			assert(!KilledOnce);
			KilledOnce=true;
		}
	}
}

void CombinatorialChamberCouplePointers::RemoveCouple(CombinatorialChamber *PlusOwner, CombinatorialChamber *MinusOwner)
{ bool KilledOnce=false;
	for (int i=0;i<this->size;i++)
	{	if(this->TheObjects[i].MinusOwner==MinusOwner
			 &&
			 this->TheObjects[i].PlusOwner==PlusOwner)
		{	this->PopIndexSwapWithLast(i);
			i--;
			assert(!KilledOnce);
			KilledOnce=true;
		}
	}
}

void rootsPointersKillOnExit::ResetCounters(int Number)
{
	for (int i=0;i<Number;i++)
	{
		this->TheObjects[i]->size=0;
	}
}

void rootsPointersKillOnExit::ComputeDebugString()
{
	std::string tempS;
	this->DebugString.clear();
	for (int i=0;i<this->size;i++)
	{
		this->TheObjects[i]->ElementToString(tempS);
		this->DebugString.append(tempS);
		this->DebugString.append("\n");
	}
}
void rootsPointersKillOnExit::Average(root& output, int Number)
{	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	output.MakeZero();
	if (this->size==0) {return;}
	for(int i=0;i<Number;i++)
	{	this->TheObjects[i]->Average(tempRoot);
		output.Add(tempRoot);
	}
	output.DivByInteger(this->size);
}

void CompositeComplexQNSub::MakeLinearSubIntegrand(root &normal,
																					root &direction,
																					Rational& Correction)
{ static PolynomialRationalCoeff EndPoint;
	EndPoint.MakePolyFromDirectionAndNormal(direction,normal,Correction);
	this->RationalPolyForm.MakeSubstitutionLastVariableToEndPoint(1,EndPoint);
	this->MatrixForCoeffs.init(root::AmbientDimension+1,1);
	static Rational tempRat;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	MatrixForCoeffs.elements[0][0].Assign(Correction);
	for (int i=1;i<this->MatrixForCoeffs.NumRows;i++)
	{ MatrixForCoeffs.elements[i][0].Assign(normal.coordinates[i-1]);
		MatrixForCoeffs.elements[i][0].DivideBy(tempRat);
	}
}

void CompositeComplexQNSub::MakeSubAddExtraVarForIntegration(root &direction)
{ this->RationalPolyForm.MakeSubAddExtraVarForIntegration(direction);
	this->MatrixForCoeffs.init(root::AmbientDimension+2,root::AmbientDimension);
	this->MatrixForCoeffs.NullifyAll();
	for (int i=0;i<this->MatrixForCoeffs.NumCols;i++)
	{ this->MatrixForCoeffs.elements[i][i].Assign(ROne);
		this->MatrixForCoeffs.elements[root::AmbientDimension+1][i].Assign(RMOne);
		this->MatrixForCoeffs.elements[root::AmbientDimension+1][i].MultiplyBy(direction.coordinates[i]);
	}
}

void CompositeComplexQNSub::MakeSubNVarForOtherChamber(root &direction, root &normal, Rational &Correction)
{ this->RationalPolyForm.MakeSubNVarForOtherChamber(direction,normal,Correction);
	this->MatrixForCoeffs.init(root::AmbientDimension+1,root::AmbientDimension);
	static Rational tempRat;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	tempRat.MinusRational();
	for (int i=0;i<root::AmbientDimension;i++)
	{ this->MatrixForCoeffs.elements[0][i].Assign(Correction);
		this->MatrixForCoeffs.elements[0][i].MultiplyBy(direction.coordinates[i]);
		this->MatrixForCoeffs.elements[0][i].MinusRational();
		for(int j=0;j<root::AmbientDimension;j++)
		{ this->MatrixForCoeffs.elements[j+1][i].Assign(normal.coordinates[j]);
		  this->MatrixForCoeffs.elements[j+1][i].DivideBy(tempRat);
			this->MatrixForCoeffs.elements[j+1][i].MultiplyBy(direction.coordinates[i]);
			if (i==j) this->MatrixForCoeffs.elements[j+1][i].Add(ROne);
		}
	}
}

void BasicComplexNumber::Assign(const BasicComplexNumber & c)
{ this->Coeff.Assign(c.Coeff);
	this->Exp.Assign(c.Exp);
}

void BasicComplexNumber::init(Rational& coeff, Rational & exp)
{ this->Coeff.AssignRational(coeff);
	this->Exp.Assign(exp);
}

void BasicComplexNumber::MultiplyBy(BasicComplexNumber &c)
{ this->Coeff.MultiplyBy(c.Coeff);
  this->Exp.Add(c.Exp);
}

void BasicComplexNumber::MultiplyByRational(Rational &c)
{ this->Coeff.MultiplyByRational(c);
}

void BasicComplexNumber::MultiplyByLargeRational(LargeRational &c)
{ this->Coeff.MultiplyBy(c);
}

void BasicComplexNumber::Simplify()
{ this->Exp.AssignFracValue();
}

bool BasicComplexNumber::operator ==(const BasicComplexNumber& c)
{ return (this->Coeff.IsEqualTo(c.Coeff) && this->Exp.IsEqualTo(c.Exp));
}

void BasicComplexNumber::operator = (const BasicComplexNumber& c)
{ this->Assign(c);
}


void BasicComplexNumber::AssignRational(Rational& r)
{ this->Coeff.AssignRational(r);
	this->Exp.MakeZero();
}

void BasicComplexNumber::AssignLargeRational(const LargeRational& r)
{ this->Coeff.Assign(r);
	this->Exp.MakeZero();
}

void BasicComplexNumber::ElementToString(std::string& output)
{ std::stringstream out;
  std::string tempS;
  if (this->Coeff.IsEqualToZero())
  { output.assign("0");
    return;
  }
  if (this->Exp.IsEqualTo(RZero))
  { this->Coeff.ElementToString(output);
    return;
  }
  this->Coeff.ElementToString(tempS);
  if (tempS=="1")
  { tempS.clear();
  }
  if (tempS=="-1")
  { tempS="-";
  }
	out<< tempS;
  this->Exp.ElementToString(tempS);
  out<<"e^{2i\\pi("<<tempS<<")}";
  output= out.str();
}

void CompositeComplex::Add(CompositeComplex &c)
{ for (int i=0;i<c.size;i++)
	{ this->AddBasicComplex(c.TheObjects[i]);
	}
	this->Simplify();
}

void CompositeComplex::MakeBasicComplex(Rational& coeff, Rational & exp)
{ static BasicComplexNumber tempB;
	tempB.init(coeff, exp);
	this->size=0;
	this->AddObjectOnTop(tempB);
}

void CompositeComplex::MultiplyBy(CompositeComplex& c)
{ static CompositeComplex Accum;
	static BasicComplexNumber tempB;
	Accum.size=0;
	for (int i=0;i<c.size;i++)
	{ for (int j=0;j<this->size;j++)
		{ tempB.Assign(this->TheObjects[j]);
			tempB.MultiplyBy(c.TheObjects[i]);
			Accum.AddBasicComplex(tempB);
		}
	}
	Accum.Simplify();
	this->CopyFromBase(Accum);
}

void CompositeComplex::AddLargeRational(LargeRational& r)
{ for (int i=0;i<this->size;i++)
	{ if(this->TheObjects[i].Exp.IsEqualTo(RZero))
		{ this->TheObjects[i].Coeff.Add(r);
			if (this->TheObjects[i].Coeff.IsEqualToZero())
			{ this->PopIndexSwapWithLast(i);
			}
			return;
		}
	}
	static BasicComplexNumber tempB;
	tempB.AssignLargeRational(r);
	this->AddObjectOnTop(tempB);
}

void CompositeComplex::ElementToString(std::string& output)
{ std::stringstream out;
	std::string tempS;
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempS);
		if (!(tempS[0]=='0'))
		{ if(!(tempS[0]=='-'))
			{ out <<"+";
			}
			out<<tempS;
		}
	}
	if (this->size>1)
	{ out<<")";
	}
	output= out.str();
	if (output[0]=='+')
	{ output.erase(0,1); }
	if (this->size>1)
	{ output.insert(0,"(");}
}

void CompositeComplex::MultiplyByRational(Rational &r)
{ if(r.IsEqualTo(RZero))
	{this->size=0; return;}
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].MultiplyByRational(r);
	}
}

void CompositeComplex::MultiplyByLargeRational(LargeRational &r)
{ if(r.IsEqualToZero())
	{this->size=0; return;}
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].MultiplyByLargeRational(r);
	}
}

void CompositeComplex::Simplify()
{ while (this->SimplifyTrue()){}
}

bool CompositeComplex::SimplifyTrue()
{ bool result = false;
	if (this->size==0) return result;
/*	static Rational CommonFactor;*/
	this->TheObjects[0].Simplify();
/*	CommonFactor.Assign(this->TheObjects[0].Exp);     */
	for (int i=1;i<this->size;i++)
	{ this->TheObjects[i].Simplify();
/*		if (CommonFactor.IsGreaterThan(this->TheObjects[i].Exp))
		{ CommonFactor.Assign(this->TheObjects[i].Exp);
		}*/
	}
/*	if (CommonFactor.IsGreaterThan(RZero))
	{ for (int i=0;i<this->size;i++)
		{this->TheObjects[i].Exp.Subtract(CommonFactor);}
	} */
	for (short i=1; i<=this->FindMaxDenExp();i++)
	{ result= result || this->SimplifyWRT(i);
	}
/*	if (CommonFactor.IsGreaterThan(RZero))
	{ for (int i=0;i<this->size;i++)
		{this->TheObjects[i].Exp.Add(CommonFactor);}
	}*/
	return result;
}

int CompositeComplex::FindMaxDenExp()
{ int result=0;
	for (int i=0;i<this->size;i++)
	{ result = Maximum(result,this->TheObjects[i].Exp.den);}
	return result;
}

bool CompositeComplex::SimplifyWRT(short n)
{ if (n==1) return false;
	LargeRational CoeffFirst;
	bool FoundFirst=false;
	CoeffFirst.MakeZero();
	int NumFound=0;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Exp.den==n)
		{ if (!FoundFirst)
			{ CoeffFirst.Assign(this->TheObjects[i].Coeff);
				FoundFirst=true;
			}
			if (!this->TheObjects[i].Coeff.IsEqualTo(CoeffFirst))
			{ return false;}
			NumFound++;
		}
	}
	if (NumFound<CompositeComplex::PrecomputedCyclotomic.EulerPhi(n))
		return false;
	if (!CoeffFirst.IsEqualToZero())
	{ for (int i=0;i<this->size;i++)
		{ if (this->TheObjects[i].Exp.den==n)
			{	this->PopIndexSwapWithLast(i);
				i--;
			}
		}
		Rational tempRat;
		LargeRational tempRat2;
		CompositeComplex::PrecomputedCyclotomic.GetSumPrimitiveRoots(n,tempRat);
		tempRat2.AssignRational(tempRat);
		tempRat2.MultiplyBy(CoeffFirst);
		this->AddLargeRational(tempRat2);
	}
	return true;
}

bool CompositeComplex::IsEqualToZero()
{ return (this->size==0);
}

void CompositeComplex::Assign(const CompositeComplex& c)
{ this->CopyFromBase(c);
}

void CompositeComplex::AddBasicComplex(BasicComplexNumber& b)
{ for(int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Exp.IsEqualTo(b.Exp))
		{ this->TheObjects[i].Coeff.Add(b.Coeff);
			if (this->TheObjects[i].Coeff.IsEqualToZero())
			{ this->PopIndexSwapWithLast(i);
			}
			return;
		}
	}
	this->AddObjectOnTop(b);
}

void CompositeComplex::AddRational(Rational& r)
{ for (int i=0;i<this->size;i++)
	{ if(this->TheObjects[i].Exp.IsEqualTo(RZero))
		{ this->TheObjects[i].Coeff.AddRational(r);
			if (this->TheObjects[i].Coeff.IsEqualToZero())
			{ this->PopIndexSwapWithLast(i);
			}
			return;
		}
	}
	static BasicComplexNumber tempB;
	tempB.AssignRational(r);
	this->AddObjectOnTop(tempB);
}

void CompositeComplex::AssignRational(Rational &r)
{ this->size=0;
	static BasicComplexNumber tempB;
	tempB.AssignRational(r);
	this->AddObjectOnTop(tempB);
}

void CompositeComplex::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void CompositeComplex::MultiplyByBasicComplex(BasicComplexNumber &b)
{	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].MultiplyBy(b);
	}
}

void CyclotomicList::DivOneVarByOneVarPoly(PolynomialRationalCoeff& p, PolynomialRationalCoeff& q,
																					 PolynomialRationalCoeff& quotient, PolynomialRationalCoeff& remainder)
{ remainder.CopyFromPoly(p);
	Monomial<Rational> HighestM;
	HighestM.Assign(q.TheObjects[0]);
	for (int i=1;i<q.size;i++)
	{ if (HighestM.degrees[0]<q.TheObjects[i].degrees[0])
		{	HighestM.Assign(q.TheObjects[i]);}
	}
	int index;
	quotient.ClearTheObjects();
	while(remainder.HasGEQMonomial(HighestM,index))
	{	Monomial<Rational> tempM;
		tempM.init(1);
		tempM.Coefficient.Assign(remainder.TheObjects[index].Coefficient);
		tempM.Coefficient.DivideBy(HighestM.Coefficient);
		tempM.degrees[0]= remainder.TheObjects[index].degrees[0]-HighestM.degrees[0];
		quotient.AddMonomial(tempM);
		tempM.Coefficient.MinusRational();
		PolynomialRationalCoeff tempQ2;
		tempQ2.CopyFromPoly(q);
		tempQ2.MultiplyByMonomial(tempM);
		remainder.AddPolynomial(tempQ2);
	}
}

void CyclotomicList::GetSumPrimitiveRoots(short n, Rational& output)
{ this->ComputeCyclotomic(n);
	int phi= this->EulerPhi(n);
	for (int i=0;i<this->TheObjects[n-1].size;i++)
	{ if 	(this->TheObjects[n-1].TheObjects[i].degrees[0]==phi-1)
		{	output.Assign(this->TheObjects[n-1].TheObjects[i].Coefficient);
			output.MinusRational();
			return;
		}
	}
	output.MakeZero();
}

void CyclotomicList::ComputeCyclotomic(short n)
{	if (this->size<n)
	{ this->SetSizeExpandOnTopNoObjectInit(n);
	}
	if(this->TheObjects[n-1].size==0)
	{	PolynomialRationalCoeff tempP;
		tempP.MakeMonomialOneLetter(1,0,n,ROne);
		tempP.AddConstant(RMOne);
		for (short i=1;i<n;i++)
		{ if (((n/i)*i)==n)
			{	this->ComputeCyclotomic(i);
				PolynomialRationalCoeff tempD, tempR, tempQ;
				tempD.CopyFromPoly(this->TheObjects[i-1]);
				this->DivOneVarByOneVarPoly(tempP,tempD,tempQ,tempR);
				tempP.CopyFromPoly(tempQ);
			}
		}
		this->TheObjects[n-1].CopyFromPoly(tempP);
	}
}

int CyclotomicList::EulerPhi(short n)
{ this->ComputeCyclotomic(n);
	int result=0;
	for(int i=0;i<this->TheObjects[n-1].size;i++)
	{ result = Maximum(result,this->TheObjects[n-1].TheObjects[i].degrees[0]);
	}
	return result;
}

void BasicQN::ScaleBy(int DenIncrease)
{ for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0;j<this->NumVars;j++)
		{ this->Exp.elements[i][j]*= DenIncrease;
		}
		this->Nums.elements[i][0]*=DenIncrease;
	}
	this->Den*=DenIncrease;
}

void BasicQN::MultiplyByRational(Rational& coeff)
{ this->Coefficient.MultiplyByRational(coeff);
	if (this->Coefficient.IsEqualToZero())
	{ this->Exp.Free();
		this->Nums.Free();
	}
}

void BasicQN::MultiplyBy(BasicQN& q)
{ if (q.Den==this->Den)
	{this->MultiplyBySameDen(q);
	}
	else
	{ static BasicQN tempQ;
		tempQ.Assign(q);
		int thelcm= lcm(this->Den,q.Den);
		tempQ.ScaleBy((thelcm/q.Den));
		this->ScaleBy((thelcm/this->Den));
		this->MultiplyBySameDen(tempQ);
	}
}

void BasicQN::GetCoeffInFrontOfLast( Rational &output)
{	if (this->Exp.NumRows>0)
	{ output.den= this->Den;
		output.num= this->Exp.elements[0][this->NumVars-1];
		output.Simplify();
	}
	else
	{ output.MakeZero();
	}
}

short BasicQN::GaussianEliminationByRows()
{ return this->GaussianEliminationByRowsCol(this->NumVars-1,false);
}

short BasicQN::GaussianEliminationByRowsCol(int Col, bool MoveToRight)
{	short Row=0;
	while(Row<this->Exp.NumRows && Col<this->NumVars && Col>=0)
	{	int PivotRow=-1;
		for (int i=Row;i<this->Exp.NumRows;i++)
		{	if(this->Exp.elements[i][Col]!=0)
			{ PivotRow=i; break;}
		}
		if (PivotRow!=-1)
		{	this->SetPivotRow(PivotRow,Row,Col);
			int ExploringRow= PivotRow+1;
			while (ExploringRow<this->Exp.NumRows)
			{	this->ComputeDebugString();
				int PivotElt= this->Exp.elements[Row][Col];
				if (PivotElt<= this->Exp.elements[ExploringRow][Col])
				{ int coeff = -(this->Exp.elements[ExploringRow][Col]/PivotElt);
					this->RowPlusScalarTimesRow(ExploringRow, coeff,Row,0);
				}
				if (this->Exp.elements[ExploringRow][Col]==0)
				{ ExploringRow++;
				}
				else
				{	SetPivotRow(ExploringRow,Row, Col);
				}
			}
			int PivotElt = this->Exp.elements[Row][Col];
			for (int i=0;i<Row;i++)
			{ if (PivotElt<=this->Exp.elements[i][Col])
				{ int coeff =-( this->Exp.elements[i][Col]/PivotElt);
					coeff%=this->Den;
					if (coeff<0){coeff+=this->Den;}
					this->RowPlusScalarTimesRow(i,coeff,Row,0);
				}
			}
			Row++;
		}
		if(MoveToRight)
		{	Col++;
		}
		else
		{ Col--;
		}
	}
	return Row;
}

void BasicQN::RowPlusScalarTimesRow(int rowInd, int scalar, int otherRowInd, int StartCol)
{ for (int i=StartCol;i<this->NumVars;i++)
	{ this->Exp.elements[rowInd][i]= (this->Exp.elements[rowInd][i]+
																			scalar*this->Exp.elements[otherRowInd][i])%(this->Den);
		if (this->Exp.elements[rowInd][i]<0) {this->Exp.elements[rowInd][i]+=this->Den;}
	}
	this->Nums.elements[rowInd][0]=(	this->Nums.elements[rowInd][0]+
																scalar*this->Nums.elements[otherRowInd][0])%(this->Den);
	if (this->Nums.elements[rowInd][0]<0){this->Nums.elements[rowInd][0]+=this->Den;}
}

void BasicQN::WriteToFile(std::fstream& output)
{ this->Coefficient.WriteToFile(output);
	output<<" | ";
	output<<this->Den<<" "<<this->Exp.NumRows<<" | ";
	for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0;j<this->Exp.NumCols;j++)
		{ output<<this->Exp.elements[i][j]<<" ";
		}
		output<<"~ "<< this->Nums.elements[i][0]<<" | ";
	}
}

void BasicQN::ReadFromFile(std::fstream& input, short NumV)
{	std::string tempS;
	this->Coefficient.ReadFromFile(input);
	short tempI;
	input>>tempS>>this->Den>>tempI>>tempS;
	this->Exp.init(tempI,root::AmbientDimension);
	this->Nums.init(tempI, root::AmbientDimension);
	this->NumVars=NumV;
	for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0;j<root::AmbientDimension;j++)
		{ input>>this->Exp.elements[i][j];
		}
		input>>tempS>>this->Nums.elements[i][0]>>tempS;
	}
}

void BasicQN::SetPivotRow(int index, int PivotRowIndex, int Col)
{ if (index!=PivotRowIndex)
	{ this->SwitchTwoRows(index,PivotRowIndex,0);
	}
//	int PivotElt = this->Exp.elements[PivotRowIndex][Col];
//	int Scale= PivotElt/(gcd(PivotElt,this->Den));
//	if (Scale!=1)
//	{	Scale = this->InvertModN(Scale,this->Den);
//		this->MultiplyRowBy(PivotRowIndex,Scale,0);
//	}
}

void BasicQN::MultiplyRowBy(int rowInd, int scalar, int StartColInd)
{ for (int i=StartColInd;i<this->NumVars;i++)
	{ this->Exp.elements[rowInd][i]=(this->Exp.elements[rowInd][i]*scalar)%this->Den;
		if (this->Exp.elements[rowInd][i]<0){this->Exp.elements[rowInd][i]+=this->Den;}
	}
	this->Nums.elements[rowInd][0]= (this->Nums.elements[rowInd][0]*scalar)%this->Den;
	if (this->Nums.elements[rowInd][0]<0){this->Nums.elements[rowInd][0]+=this->Den;}
}

void BasicQN::MultiplyBySameDen(BasicQN& q)
{ int newRows= q.Exp.NumRows;
	this->Exp.Resize(this->Exp.NumRows+q.Exp.NumRows,this->NumVars,true);
	this->Nums.Resize(this->Exp.NumRows,1,true);
	for (int i =0;i<newRows;i++ )
	{ for (int j=0;j<q.NumVars;j++)
		{ this->Exp.elements[this->Exp.NumRows-newRows+i][j]= q.Exp.elements[i][j];
		}
		this->Nums.elements[this->Exp.NumRows-newRows+i][0]= q.Nums.elements[i][0];
	}
	this->Coefficient.MultiplyBy(q.Coefficient);
	this->Simplify();
}

int BasicQN::InvertModN( int X, int N)
{ static int q,r,p,d; // d - divisor, q - quotient, r - remainder,
											// p is the number to be divided
	static int vD[2],vP[2], temp;
	vP[0]=1; vP[1]=0;// at any given moment, p=vP[0]*N+vP[1]*X
	vD[0]=0; vD[1]=1;	 // at any given moment, d=vD[0]*N+vD[1]*X
	p=N; d=X; d%=N; if (d<0){d+=N;}
	while (d>0)
	{ q=p/d;
		r=p%d;
		p=d;
		d=r;
		for(int i=0;i<2;i++)
		{	temp=vP[i];
			vP[i]= vD[i];
			vD[i]= temp-q*vD[i];
		}
	}
	assert(p==1);//if d and p were relatively prime this should be so.
							 //Otherwise the function was not called properly.
	p=vP[1]%N;
	if (p<0){p+=N;}
	return p;
}

inline void BasicQN::Assign(const BasicQN &q)
{ this->Exp.Assign(q.Exp);
	this->Nums.Assign(q.Nums);
	this->NumVars= q.NumVars;
	this->Den= q.Den;
	this->Coefficient.Assign(q.Coefficient);
}

void BasicQN::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void BasicQN::ElementToString(std::string &output)
{ this->ElementToString(output,PolyFormatLocal);
}

void BasicQN::ElementToString(std::string &output,PolynomialOutputFormat& PolyFormat)
{ std::stringstream out;
	std::string tempS;
	if (this->Exp.NumRows==0)
	{ this->Coefficient.ElementToString(output);
		return;
	}
	this->Coefficient.ElementToString(tempS);
	if (tempS=="1") tempS.clear();
	if (tempS=="-1") tempS="-";
	if (!PolyFormat.UsingLatexFormat )
	{ out<<tempS<<"t_"<<this->Den<< "["; }
	else
	{ out <<tempS<<"\\tau_{"<<this->Den<<"}[";
	}
	for (int i=0;i<this->Exp.NumRows;i++)
	{	std::stringstream out2;
		out2<<"(";
		for (int j=0;j<this->NumVars;j++)
		{ if (this->Exp.elements[i][j]!=0)
			{	if (this->Exp.elements[i][j]>0)
				{ out2<<"+";
				}
				if (this->Exp.elements[i][j]!=1)
				{ if (this->Exp.elements[i][j]==-1)
					{ out2<<"-";
					}
					else
					{ out2<<this->Exp.elements[i][j];
					}
				}
				out2<< PolyFormatLocal.alphabet[j];
			}
		}
		out2<<"="<<this->Nums.elements[i][0]<<")";
		tempS= out2.str();
		if (tempS[1]=='+')
		{ tempS.erase(1,1);
		}
		out<<tempS;
	}
	out<<"]";
	output=out.str();
}

inline bool BasicQN::ExponentIsEqualToZero()
{ return (this->Exp.NumRows==0);
}

inline bool BasicQN::HasSameExponent(BasicQN &q)
{ if (this->Exp.NumRows!=q.Exp.NumRows)
	{ return false;
	}
	if (this->Den!=q.Den)
	{ return false;
	}
	for (int i=0;i<this->Exp.NumRows;i++)
	{ if (this->Nums.elements[i][0]!=q.Nums.elements[i][0])
			return false;
		for (int j=0;j<this->NumVars;j++)
		{ if (this->Exp.elements[i][j]!=q.Exp.elements[i][j])
				return false;
		}
	}
	return true;
}

inline void BasicQN::Evaluate(int*theVars, LargeRational& output)
{ output.MakeZero();
	for (int i=0;i<this->Exp.NumRows;i++)
	{ int Accum=0;
		for (int j=0;j<this->NumVars;j++)
		{ Accum += this->Exp.elements[i][j]*theVars[j];
		}
		Accum-=this->Nums.elements[i][0];
		Accum%=this->Den;
		if (Accum<0){Accum+=this->Den;}
		if (Accum!=0)
			return;
	}
	output.Assign(this->Coefficient);
}

void BasicQN::MakeQNFromMatrixAndColumn(MatrixRational& theMat, root& column)
{ this->Coefficient.AssignRational(ROne);
	this->NumVars=root::AmbientDimension;
	this->Exp.init(theMat.NumRows,root::AmbientDimension);
	this->Nums.init(theMat.NumRows,1);
	int tempLCM= 1;
	theMat.ComputeDebugString();
	for (int i=0;i<theMat.NumRows;i++)
	{	for (int j=0;j<theMat.NumCols;j++)
		{ tempLCM= lcm(tempLCM,theMat.elements[i][j].den);
		}
		tempLCM= lcm(tempLCM,column.coordinates[i].den);
	}
	this->Den=tempLCM;
	for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0; j<root::AmbientDimension;j++)
		{ this->Exp.elements[i][j]=(theMat.elements[i][j].num*tempLCM)/theMat.elements[i][j].den;
			this->Exp.elements[i][j]%=tempLCM;
			if (this->Exp.elements[i][j]<0) {this->Exp.elements[i][j]+=tempLCM;}
		}
		this->Nums.elements[i][0]= (column.coordinates[i].num*tempLCM)/
											column.coordinates[i].den;
		this->Nums.elements[i][0]%=tempLCM;
		if (this->Nums.elements[i][0]<0){this->Nums.elements[i][0]+=tempLCM;}
	}
	this->ComputeDebugString();
	this->Simplify();
	this->ComputeDebugString();
}

void BasicQN::BasicQNToComplexQN(CompositeComplexQN& output)
{ static CompositeComplexQN tempC;
	MatrixRational TheSub;
	TheSub.init(this->NumVars+1,1);
	output.MakeConst(ROne,this->NumVars);
//	tempC.MakeConst(RZero,this->NumVars);
	for (int i=0;i<this->Exp.NumRows;i++)
	{ PrecomputedTausLocal.GetTaukn(this->Nums.elements[i][0],this->Den,tempC);
		for (int j=0;j<this->NumVars;j++)
		{ TheSub.elements[j+1][0].init(this->Exp.elements[i][j],1);
		}
		TheSub.elements[0][0].MakeZero();
		tempC.LinearSubstitution(TheSub);
		output.MultiplyBy(tempC);
	}
	output.MultiplyByLargeRational(this->Coefficient);
}

inline void BasicQN::DecreaseNumVars(short decrease)
{ this->Exp.Resize(this->Exp.NumRows,this->Exp.NumCols-decrease,true);
	this->NumVars-=decrease;
}

bool BasicQN::IsEqualToZero()
{ return this->Coefficient.IsEqualToZero();
}

inline void BasicQN::operator =(const BasicQN& q)
{ this->Assign(q);
}

bool BasicQN::operator ==(BasicQN &q)
{ return this->HasSameExponent(q);
}

inline void BasicQN::SwitchTwoRows(int rowI1, int rowI2, int StartCol)
{ int temp;
	for (int i=StartCol;i<this->NumVars;i++)
	{	temp=this->Exp.elements[rowI1][i];
		this->Exp.elements[rowI1][i]= this->Exp.elements[rowI2][i] ;
		this->Exp.elements[rowI2][i]= temp;
	}
	temp= this->Nums.elements[rowI1][0];
	this->Nums.elements[rowI1][0]=this->Nums.elements[rowI2][0];
	this->Nums.elements[rowI2][0]=temp;
}

void BasicQN::Simplify()
{	short FirstZeroRow=this->GaussianEliminationByRows();
	if (Den==1){FirstZeroRow=0;}
	for (int i=FirstZeroRow;i<this->Exp.NumRows;i++)
	{ if (this->Nums.elements[i][0]!=0)
		{ this->MakeConst(RZero,this->NumVars);
			return;
		}
	}
	this->Exp.Resize(FirstZeroRow,this->NumVars,true);
	this->Nums.Resize(FirstZeroRow,1,true);
}

void BasicQN::LinearSubstitution(QPSub& theSub)
{	assert(this->NumVars== theSub.TheQNSub.NumCols);
	static BasicQN tempOutput;
	short LastRowIndex=theSub.TheQNSub.NumRows-1;
	tempOutput.Exp.init(this->Exp.NumRows,LastRowIndex);
	tempOutput.Nums.init(LastRowIndex,1);
	this->ScaleBy(theSub.QNSubDen);
	for(int i=0;i<this->Exp.NumRows;i++)
	{ for (int k=0;k<LastRowIndex;k++)
		{	tempOutput.Exp.elements[i][k]=0;
			for (int j=0;j<this->NumVars;j++)
			{ tempOutput.Exp.elements[i][k]+=(this->Exp.elements[i][j]/theSub.QNSubDen)*
										theSub.TheQNSub.elements[k][j];
			}
			tempOutput.Exp.elements[i][k]%=this->Den;
			if (tempOutput.Exp.elements[i][k]<0){tempOutput.Exp.elements[i][k]+=this->Den;}
		}
		for (int j=0;j<this->NumVars;j++)
			tempOutput.Nums.elements[i][0]-=(this->Exp.elements[i][j]/theSub.QNSubDen)*
											theSub.TheQNSub.elements[LastRowIndex][j];
		tempOutput.Nums.elements[i][0]%= this->Den;
		if (tempOutput.Nums.elements[i][0]<0){tempOutput.Nums.elements[i][0]+=this->Den;}
	}
	this->Exp.Assign(tempOutput.Exp);
	this->Nums.Assign(tempOutput.Nums);
	this->NumVars=theSub.TheQNSub.NumRows-1;
	this->ComputeDebugString();
	this->Simplify();
}

void BasicQN::MakeConst(Rational& Coeff, short NumV)
{ this->Exp.Free();
	this->Nums.Free();
	this->Coefficient.AssignRational(Coeff);
	this->Den=1;
	this->NumVars=NumV;
}

void BasicQN::MakeConst(LargeRational& Coeff, short NumV)
{ this->Exp.Free();
	this->Coefficient.Assign(Coeff);
	this->Den=1;
	this->NumVars=NumV;
}

void BasicQN::MakeFromNormalAndDirection(root& normal, root& direction,
																				 int theMod, Rational& coeff)
{ root tempRoot;
	Rational tempRat;
	root::RootScalarEuclideanRoot(normal,direction,tempRat);
	tempRoot.Assign(normal);
	tempRoot.DivByRational(tempRat);
	this->Exp.init(1,root::AmbientDimension);
	this->Nums.init(1,1);
	this->Den= tempRoot.FindLCMDenominators();
	tempRoot.MultiplyByInteger(this->Den);
	this->Coefficient.AssignRational(coeff);
	this->NumVars= root::AmbientDimension;
	this->Nums.elements[0][0]=theMod;
	for (int i=0;i<root::AmbientDimension;i++)
	{ this->Exp.elements[0][i]=tempRoot.coordinates[i].num;
	}
	this->Simplify();
}

int BasicQN::HashFunction()
{ int result=0;
	for (int i=0;i<this->Exp.NumRows;i++)
	{ result+=SomeRandomPrimes[i]*this->Exp.elements[i][this->NumVars-i-1];
	}
	return result;
}

void BasicQN::MakePureQN(	short NumVariables,int NonZeroIndex,
													Rational&coeff, int theExp, int Num, int theDen)
{ this->NumVars= NumVariables;
	this->Exp.init(1,NumVariables);
	this->Nums.init(1,1);
	this->Nums.elements[0][0]=Num;
	this->Coefficient.AssignRational(coeff);
	this->Exp.NullifyAll();
	this->Den=theDen;
	this->Exp.elements[0][NonZeroIndex] = theExp% this->Den;
}

void BasicQN::ExpToDebugString()
{ std::stringstream out;
	for (int i=0;i<this->Exp.NumRows;i++)
	{	for (int j=0;j<this->NumVars;j++)
			out<<"|"<<this->Exp.elements[i][j];
		out<<"|";
	}
	this->DebugString=out.str();
	this->ComputeDebugString();
}

void QuasiNumber::ElementToString(std::string &output)
{ this->ElementToString(output,PolyFormatLocal);
}

void QuasiNumber::ElementToString(std::string &output, PolynomialOutputFormat& PolyFormat)
{ std::stringstream out;
	std::string tempS;
	if (this->size>1)
	{ out<<"(";}
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempS, PolyFormat);
		if (!(tempS[0]=='0'))
		{ if(!(tempS[0]=='-'))
			{ out <<"+";
			}
			out<<tempS;
		}
	}
	if (this->size>1)
	{ out<<")";
	}
	output= out.str();
	if (output.size()>1)
	{ if (output[1]=='+')
	  { output.erase(1,1); }
	}
	if (output.size()>0)
	{ if (output[0]=='+')
	  { output.erase(0,1); }
	}
}

void QuasiNumber::Add(QuasiNumber &q)
{ for (int i=0;i<q.size;i++)
	{	this->AddBasicQuasiNumber(q.TheObjects[i]);
	}
}

void QuasiNumber::WriteToFile(std::fstream& output)
{ output<< this->size<<" "<< this->NumVariables<<" | ";
	for(int i=0;i<this->size;i++)
	{ this->TheObjects[i].WriteToFile(output);
	}
}

void QuasiNumber::ReadFromFile(std::fstream& input)
{	int NeededSize;
	std::string tempS;
	input>> NeededSize >> this->NumVariables>>tempS;
	static BasicQN tempQN;
	this->MakeConst(RZero,this->NumVariables);
	this->SetActualSizeAtLeastExpandOnTop(NeededSize);
	for(int i=0;i<NeededSize;i++)
	{ tempQN.ReadFromFile(input,this->NumVariables);
		this->AddBasicQuasiNumber(tempQN);
	}
}

void QuasiNumber::AddBasicQuasiNumber(BasicQN &q)
{ if (QuasiNumber::AnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString();
		q.ComputeDebugString();
	}
	int tempH= q.HashFunction()%this->HashSize;
	if (tempH<0){tempH+=this->HashSize;}
	if (q.Coefficient.IsEqualToZero())
	{	return;
	}
	for (int i=0;i<this->TheHashedArrays[tempH].size;i++)
	{	int tempI = this->TheHashedArrays[tempH].TheObjects[i];
		if (this->TheObjects[tempI].HasSameExponent(q))
		{	this->TheObjects[tempI].Coefficient.Add(q.Coefficient);
			if (this->TheObjects[tempI].Coefficient.IsEqualToZero())
			{	this->PopIndexSwapWithLastHash(tempI);
			}
			return;
		}
	}
	this->AddObjectOnTopHash(q);
}

void QuasiNumber::AssignInteger(short NumVars,int x)
{ LargeRational tempRat;
	tempRat.AssignInteger(x);
	this->AssignLargeRational(NumVars,tempRat);
}


void QuasiNumber::Assign(const QuasiNumber &q)
{	if (this==&q) return;
	this->MakeZero(q.NumVariables);
	this->CopyFromHash(q);
}

bool QuasiNumber::ComputeDebugString()
{
	this->DebugString.clear();
	this->ElementToString(this->DebugString);
	return true;
}

void QuasiNumber::Evaluate(int *theVars, LargeRational& output)
{ output.MakeZero();
	static LargeRational tempRat;
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Evaluate(theVars,tempRat);
		output.Add(tempRat);
	}
}

void QuasiNumber::DivideByRational(Rational& r)
{	Rational tempRat;
	tempRat.Assign(r);
	tempRat.Invert();
	this->MultiplyByRational(tempRat);
}

void QuasiNumber::MultiplyBy(QuasiNumber& q)
{	static QuasiNumber Accum;
	static BasicQN tempQ;
	Accum.MakeZero(this->NumVariables);
	for (int i=0;i<q.size;i++)
	{ for (int j=0;j<this->size;j++)
		{ tempQ.Assign(this->TheObjects[j]);
			tempQ.MultiplyBy(q.TheObjects[i]);
			Accum.AddBasicQuasiNumber(tempQ);
		}
	}
	this->Assign(Accum);
}

void QuasiNumber::QNtoComplex(CompositeComplexQN& output)
{ output.MakeConst(RZero,this->NumVariables);
	CompositeComplexQN tempC;
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].BasicQNToComplexQN(tempC);
		output.Add(tempC);
	}
}

bool QuasiNumber::IsEqualTo(QuasiNumber&q)
{	//the below line is just for speed
	if (&q==&QuasiNumber::TheRingZero)
	{ return this->IsEqualToZero();
	}
	QuasiNumber tempQN(q.NumVariables) ;
	tempQN.Assign(q);
	tempQN.MultiplyByRational(RMOne);
	tempQN.Add(*this);
	return tempQN.IsEqualToZero();
}


bool QuasiNumber::IsEqualToZero()
{ return (this->size==0);
	/*for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i].IsEqualToZero())
		{	return false;
		}
	}*/
//	return true;
}

void QuasiNumber::LinearSubstitution(QPSub& TheSub)
{	static QuasiNumber Accum;
	static BasicQN tempQ;
	Accum.MakeZero(TheSub.TheQNSub.NumRows-1);
	for(int i=0;i<this->size;i++)
	{	tempQ.Assign(this->TheObjects[i]);
		tempQ.LinearSubstitution(TheSub);
		Accum.AddBasicQuasiNumber(tempQ);
	}
	this->Assign(Accum);
}

void QuasiNumber::MakeZero(short NumVars)
{ this->ClearTheObjects();
	this->NumVariables= NumVars;
}

/*void QuasiNumber::DecreaseNumVars(int decrease)
{ for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].DecreaseNumVars(decrease);
	}
	this->NumVariables-=decrease;
	this->Simplify();
}*/

void QuasiNumber::Simplify()
{ this->ComputeDebugString();
	if (this->size==0){return;}
	int theLCM=1;
	for (int i=0;i<this->size;i++)
	{ theLCM=lcm(theLCM,this->TheObjects[i].Den);
	}
	if (theLCM>4) {return;}
	int NumCycles = KToTheNth(theLCM,root::AmbientDimension);
	SubsetWithMultiplicities theSubset;
	theSubset.init(root::AmbientDimension, theLCM-1);
//	bool oneValue=true;
	LargeRational theValue, tempRat;
	std::string tempS, tempS2;
	for (int i=0;i<NumCycles;i++)
	{ theValue.ElementToString(tempS);
		tempRat.ElementToString(tempS2);
		if (i==0)
		{ this->Evaluate(theSubset.Multiplicities.TheObjects,theValue);}
		else
		{ this->Evaluate(theSubset.Multiplicities.TheObjects,tempRat);
			if (!tempRat.IsEqualTo(theValue)) {return;}
			theSubset.IncrementSubset();
		}
	}
	this->MakeZero(root::AmbientDimension);
	this->AssignLargeRational(root::AmbientDimension, theValue);
}

void QuasiNumber::AssignLargeRational(short NumVars, LargeRational& coeff)
{	static BasicQN q;
	this->MakeZero(NumVars);
	q.MakeConst(coeff,NumVars);
	this->AddBasicQuasiNumber(q);
}

void QuasiNumber::MakePureQN(short NumVar,int NonZeroIndex, Rational&coeff, int theExp, int Num, int theDen)
{ this->MakeZero(NumVar);
	static BasicQN tempQ;
	tempQ.MakePureQN(NumVar,NonZeroIndex,coeff,theExp,Num,theDen);
	this->AddBasicQuasiNumber(tempQ);
}

void QuasiNumber::MakeFromNormalAndDirection(root& normal, root& direction,
																						 int theMod, Rational& coeff)
{ this->MakeZero(root::AmbientDimension);
	static BasicQN tempQ;
	tempQ.MakeFromNormalAndDirection(normal,direction,theMod,coeff);
	this->AddBasicQuasiNumber(tempQ);
}

void QuasiNumber::MakeConst(Rational& Coeff, short NumV)
{	static BasicQN q;
	this->MakeZero(NumV);
	q.MakeConst(Coeff,NumV);
	this->AddBasicQuasiNumber(q);
}

void QuasiNumber::MakeConst(LargeRational& Coeff, short NumV)
{	static BasicQN q;
	this->MakeZero(NumV);
	q.MakeConst(Coeff,NumV);
	this->AddBasicQuasiNumber(q);
}

void QuasiNumber::MakeQNFromMatrixAndColumn(MatrixRational& theMat, root& column)
{ this->ClearTheObjects();
	this->NumVariables= root::AmbientDimension;
	static BasicQN tempBQN;
	tempBQN.MakeQNFromMatrixAndColumn(theMat,column);
	this->AddBasicQuasiNumber(tempBQN);
}

void QuasiNumber::MultiplyByBasicQuasiNumber(BasicQN&q)
{ static QuasiNumber Accum;
	static BasicQN tempQ;
	Accum.MakeZero(this->NumVariables);
	for (int i=0;i<this->size;i++)
	{	tempQ.Assign(this->TheObjects[i]);
		tempQ.MultiplyBy(q);
		Accum.AddBasicQuasiNumber(tempQ);
	}
	this->Assign(Accum);
}

void QuasiNumber::MultiplyByRational(Rational &r)
{	if (r.IsEqualTo(RZero))
	{ this->MakeZero(this->NumVariables);
		return;
	}
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByRational(r);
	}
}

void QPSub::MakeLinearSubIntegrand(root &normal,
																	 root &direction,
																	 Rational& Correction)
{ static PolynomialRationalCoeff EndPoint;
	EndPoint.MakePolyFromDirectionAndNormal(direction,normal,Correction);
	this->RationalPolyForm.MakeSubstitutionLastVariableToEndPoint(1,EndPoint);
	this->TheQNSub.init(root::AmbientDimension+1,1);
	static Rational tempRat;
	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	tempRoot.Assign(normal);
	tempRoot.DivByRational(tempRat);
	int tempDen=tempRoot.FindLCMDenominators();
	tempDen= lcm(Correction.den,tempDen);
	tempRoot.MultiplyByInteger(tempDen);
	this->QNSubDen= tempDen;
	this->TheQNSub.elements [root::AmbientDimension][0]=-(Correction.num*tempDen)/Correction.den;
	this->TheQNSub.elements [root::AmbientDimension][0]%=tempDen;
	if (this->TheQNSub.elements [root::AmbientDimension][0]<0)
	{	this->TheQNSub.elements[root::AmbientDimension][0]+=tempDen;
	}
	for (int i=0;i<this->TheQNSub.NumRows;i++)
	{ this->TheQNSub.elements[i][0]=(normal.coordinates[i].num)%this->QNSubDen;
		if (this->TheQNSub.elements[i][0]<0){this->TheQNSub.elements[i][0]+=tempDen;}
	}
}

void QPSub::MakeSubAddExtraVarForIntegration(root &direction)
{ this->RationalPolyForm.MakeSubAddExtraVarForIntegration(direction);
	this->TheQNSub.init(root::AmbientDimension+2,root::AmbientDimension);
	this->TheQNSub.NullifyAll();
	this->QNSubDen=1;
	for (int i=0;i<this->TheQNSub.NumCols;i++)
	{ this->TheQNSub.elements[i][i]=1;
		this->TheQNSub.elements[root::AmbientDimension][i]= -direction.coordinates[i].num;
	}
}

void QPSub::MakeSubFromMatrixIntAndDen(MatrixInt& theMat, int Den)
{	this->RationalPolyForm.MakeSubFromMatrixIntAndDen(theMat,Den);
	this->QNSubDen=Den;
	this->TheQNSub.Assign(theMat);
}

void QPSub::MakeSubFromPolynomialsRationalCoeff(PolynomialsRationalCoeff& input)
{	this->RationalPolyForm.SetSizeExpandOnTopNoObjectInit(input.size);
	for(int i=0;i<input.size;i++)
	{ this->RationalPolyForm.TheObjects[i].CopyFromPoly(input.TheObjects[i]);
	}
	this->TheQNSub.init(root::AmbientDimension+1,root::AmbientDimension);
	this->TheQNSub.NullifyAll();
	for (int i=0;i<input.size;i++)
	{ for (int k=0;k<input.TheObjects[i].size;k++)
		{	for (int j=0;j<root::AmbientDimension;j++)
			{ bool IsAConstantTerm=true;
				assert(	input.TheObjects[i].TheObjects[k].degrees[j]==1 ||
								input.TheObjects[i].TheObjects[k].degrees[j]==0);
				if (input.TheObjects[i].TheObjects[k].degrees[j]==1)
				{ this->TheQNSub.elements[i][j]=input.TheObjects[i].TheObjects[k].Coefficient.num;
					assert(IsAConstantTerm);
					IsAConstantTerm=false;
				}
				if (IsAConstantTerm)
				{ this->TheQNSub.elements[root::AmbientDimension][i]=input.TheObjects[i].TheObjects[k].Coefficient.num;  }
			}
		}
	}
}

void QPSub::MakeSubNVarForOtherChamber(root &direction, root &normal, Rational &Correction)
{ this->RationalPolyForm.MakeSubNVarForOtherChamber(direction,normal,Correction);
	this->TheQNSub.init(root::AmbientDimension+1,root::AmbientDimension);
	static Rational tempRat;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	tempRoot.Assign(normal);
	tempRat.MinusRational();
	tempRoot.DivByRational(tempRat);
	int tempDen = tempRoot.FindLCMDenominators();
	tempDen= lcm(tempDen,Correction.den);
	tempRoot.MultiplyByInteger(tempDen);
	int Corr= -(Correction.num*Correction.den)/tempDen;
	this->TheQNSub.NullifyAll();
	this->QNSubDen=tempDen;
	for (int i=0;i<root::AmbientDimension;i++)
	{ this->TheQNSub.elements[root::AmbientDimension][i]=(direction.coordinates[i].num*Corr)%tempDen;
		if (this->TheQNSub.elements[root::AmbientDimension][i]<0)
		{	this->TheQNSub.elements[root::AmbientDimension][i]+=tempDen;
		}
		for(int j=0;j<root::AmbientDimension;j++)
		{ this->TheQNSub.elements[i][j]= tempRoot.coordinates[j].num*direction.coordinates[j].num;
			if (i==j)
			{ this->TheQNSub.elements[i][j]+=1;
			}
			TheQNSub.elements[i][j]%=tempDen;
			if (TheQNSub.elements[i][j]<0){TheQNSub.elements[i][j]+=tempDen;}
		}
	}
}

void SortedQPs::AddToEntry(int x, int y, int z, QuasiMonomial &QM)
{ if (x>this->FakeSize-1)
	{ this->setFakeSize(x+1,QM.NumVariables);
	}
	if (y>this->TheObjects[x]->FakeSize-1)
	{ this->TheObjects[x]->setFakeSize(y+1,QM.NumVariables);
	}
	if(z>this->TheObjects[x]->TheObjects[y]->FakeSize-1)
	{ this->TheObjects[x]->TheObjects[y]->setFakeSize(z+1,QM.NumVariables);
	}
	this->TheObjects[x]->TheObjects[y]->TheObjects[z]->AddMonomial(QM);
}

void LargeRational::MultiplyByRational(Rational &r)
{	assert(r.den>0);
	int tempNum;
	if (r.num<0)
	{ tempNum=-r.num; this->num.sign*=-1;
	}
	else
	{ tempNum=r.num;
	}
	this->num.MultiplyByInt(tempNum);
	this->den.MultiplyByInt(r.den);
	this->Simplify();
}

bool LargeRational::IsEqualTo(const LargeRational& r)
{ static LargeRational x;
	x.Assign(r);
	x.Subtract(*this);
	return x.IsEqualToZero();
}

void LargeRational::WriteToFile(std::fstream& output)
{ std::string tempS;
	this->ElementToString(tempS);
	output<<tempS<<" ";
}

void LargeRational::RaiseToPower(int x)
{ if (x==0)
	{	assert(!this->IsEqualToZero());
		this->Assign(LROne);
		return;
	}
	if (x<0)
	{ x=-x;
		this->Invert();
	}
	LargeRational tempRat;
	tempRat.Assign(*this);
	for (int i=1;i<x;i++)
	{ this->MultiplyBy(tempRat);
	}
}

void LargeRational::Invert()
{ assert(!this->num.IsEqualToZero());
	LargeInt tempI;
	tempI.Assign(this->den);
	this->den.Assign(this->num);
	if (this->den.sign==-1)
	{	this->den.sign=1;
		tempI.sign=-tempI.sign;
	}
	this->num.Assign(tempI);
}

void LargeRational::ReadFromFile(std::fstream& input)
{ std::string tempS;
	input>> tempS;
	int positionInTempS=0;
	if (tempS=="0")
	{ this->MakeZero();
		return;
	}
	this->num.sign=1;
	if (tempS[0]=='-')
	{ positionInTempS++;
	}
	this->num.AssignInt(0);
	this->den.AssignInt(1);
	LargeInt* CurrentNumberRead;
	CurrentNumberRead= &this->num;
	for (unsigned i=positionInTempS;i<tempS.length();i++)
	{ char a= tempS[i];
		if (a=='/')
		{ assert(CurrentNumberRead!=&this->den);
			CurrentNumberRead= &this->den;
			CurrentNumberRead->AssignInt(0);
		}else
		{	CurrentNumberRead->MultiplyByInt(10);
			int x=std::atoi(&a);
			CurrentNumberRead->AddInt(x);
		}
	}
	if (tempS[0]=='-')
	{ this->num.sign=-1;
	}
}

void LargeRational::MultiplyByInt(int x)
{ static Rational tempRat;
	tempRat.AssignInteger(x);
	this->MultiplyByRational(tempRat);
}

void LargeRational::MultiplyBy(LargeRational &r)
{ this->num.MultiplyBy(r.num);
	this->den.MultiplyBy(r.den);
	this->Simplify();
}

void LargeRational::Assign(const LargeRational &r)
{ this->num.Assign(r.num);
	assert(r.den.size!=0);
	this->den.Assign(r.den);
}

void LargeRational::ElementToString(std::string &output)
{ if (this->IsEqualToZero())
	{ output="0";
		return;
	}
	std::stringstream  out;
	std::string tempS;
	this->num.ElementToString(tempS);
	out<<tempS;
	if (tempS=="0")
	{ output=out.str();
		return;
	}
	this->den.ElementToString(tempS);
	if (tempS!="1")
		out<<"/"<<tempS;
	output= out.str();
}

bool LargeRational::IsEqualToZero()
{ return this->num.IsEqualToZero();
}

void LargeRational::AddRational(Rational& r)
{ assert(r.den>0);
	static LargeRational temp;
	temp.AssignRational(r);
	this->Add(temp);
}

void LargeRational::Subtract(LargeRational& r)
{ static LargeRational temp;
	temp.Assign(r);
	temp.num.sign*=-1;
	this->Add(temp);
}

void LargeRational::MakeZero()
{ this->num.MakeZero();
	this->den.AssignInt(1);
}

void LargeRational::AssignInteger(int x)
{	Rational tempRat;
	tempRat.AssignInteger(x);
	this->AssignRational(tempRat);
}

void LargeRational::AssignRational(Rational &r)
{	assert(r.den>0);
	this->num.AssignInt(r.num);
	this->den.AssignInt(r.den);
}

void LargeRational::Add(LargeRational &r)
{ static std::string tempS1,tempS2, tempS3, tempS4, tempS5, tempS6, tempS7;
	assert(this->den.sign!=-1);
	assert(r.den.sign!=-1);
	assert(r.den.size!=0);
	if (r.num.size==0) return;
	static LargeInt tempI;
	if(this->AnErrorHasOccurredTimeToPanic)
	{ this->den.ElementToString(tempS1);
		r.num.ElementToString(tempS2);
	}
	this->den.MultiplyBy(r.num,tempI);
	if(this->AnErrorHasOccurredTimeToPanic)
	{ tempI.ElementToString(tempS3);
		this->num.ElementToString(tempS6);
		r.den.ElementToString(tempS7);
	}
	this->num.MultiplyBy(r.den);
	if(this->AnErrorHasOccurredTimeToPanic)
	{ this->num.ElementToString(tempS4);
	}
	this->num.Add(tempI);
	if(this->AnErrorHasOccurredTimeToPanic)
	{ this->num.ElementToString(tempS5);
	}
	assert(this->den.sign!=-1);
	this->den.MultiplyBy(r.den);
	assert(this->den.sign!=-1);
	if(this->AnErrorHasOccurredTimeToPanic)
	{ this->ElementToString(tempS1);
	}
	this->Simplify();
	if(this->AnErrorHasOccurredTimeToPanic)
	{ this->ElementToString(tempS2);
	}
	assert(this->den.sign!=-1);
}

void LargeRational::Simplify()
{ if (this->num.IsEqualToZero())
	{ this->MakeZero(); return;}
	if (this->den.sign==-1)
	{ this->den.sign=1;
		this->num.sign*=-1;
	}
	static LargeInt tempI;
	LargeInt::gcd(this->den, this->num,tempI);
	if (LargeRational::AnErrorHasOccurredTimeToPanic)
	{ std::string tempS1,tempS2,tempS3;
		tempI.ElementToString(tempS1);
		this->ElementToString(tempS2);
	}
	assert(tempI.sign!=-1);
	static LargeInt tempI2;
	this->den.DivPositive(tempI,this->den,tempI2);
	this->num.DivPositive(tempI,this->num,tempI2);
}

inline void LargeInt::AssignShiftedUInt(unsigned int x, int shift)
{ if (x==0){this->MakeZero(); return;}
	this->sign=1;
	this->SetActualSizeAtLeastExpandOnTop(shift+1);
  this->SetSizeExpandOnTopNoObjectInit(shift);
	for (int i=0;i<shift;i++)
		this->TheObjects[i]=0;
	while (x!=0)
	{ unsigned int tempX= x%LargeInt::CarryOverBound;
	  this->AddObjectOnTop(tempX);
    x= x/LargeInt::CarryOverBound;
  }
}

void LargeInt::AddPositive(LargeInt &x)
{ int oldsize= this->size;
	this->SetSizeExpandOnTopNoObjectInit(Maximum(this->size,x.size)+1);
	for (int i=oldsize;i<this->size;i++)
	{ this->TheObjects[i]=0;
	}
	unsigned int CarryOver=0;
	for(int i=0;i<x.size;i++)
	{ this->TheObjects[i]+=x.TheObjects[i]+CarryOver;
		if (this->TheObjects[i]>=LargeInt::CarryOverBound)
		{ this->TheObjects[i]-=LargeInt::CarryOverBound;
			CarryOver=1;
		}
		else
			CarryOver=0;
	}
	if (CarryOver!=0)
	{	for(int i=x.size;i<this->size;i++)
		{ this->TheObjects[i]+=1;
			if (this->TheObjects[i]>=LargeInt::CarryOverBound)
			{ this->TheObjects[i]-=LargeInt::CarryOverBound;
			}
			else
				break;
		}
	}
	this->FitSize();
}

void LargeInt::SubtractSmallerPositive(LargeInt& x)
{	unsigned int CarryOver=0;
	for (int i=0;i<x.size;i++)
	{ if (this->TheObjects[i]<x.TheObjects[i]+CarryOver)
		{ this->TheObjects[i]+=LargeInt::CarryOverBound;
			this->TheObjects[i]-=(x.TheObjects[i]+CarryOver);
			CarryOver=1;
		}
		else
		{	this->TheObjects[i]-=(x.TheObjects[i]+CarryOver);
			CarryOver=0;
		}
	}
	if (CarryOver!=0)
	{	for (int i=x.size;i<this->size;i++)
		{	if ( this->TheObjects[i]>0)
			{	this->TheObjects[i]--;
				break;
			}
			else
			{ this->TheObjects[i]=LargeInt::CarryOverBound-1;
			}
		}
	}
	this->FitSize();
//	assert(this->CheckForConsistensy());
}

/*void LargeInt::SubtractToIndex(int index, unsigned int x)
{ if (x>this->TheObjects[index])
	{ this->TheObjects[index]+=LargeInt::CarryOverBound;
		this->TheObjects[index]-=x;
		this->SubtractToIndex(index+1,1);
	}
	else
	{ this->TheObjects[index]-=x;
	}
//	assert(this->CheckForConsistensy());
}*/

void LargeInt::MultiplyBy(LargeInt &x, LargeInt &output)
{ assert(this!=&output);
	output.MakeZero();
	output.SetActualSizeAtLeastExpandOnTop(x.size+this->size);
	for (int i=0;i<this->size;i++)
	{ for(int j=0;j<x.size;j++)
		{ unsigned long long tempLong= this->TheObjects[i];
			unsigned long long tempLong2= x.TheObjects[j];
			tempLong= tempLong*tempLong2;
			unsigned long long lowPart= tempLong%LargeInt::CarryOverBound;
			unsigned long long highPart= tempLong/LargeInt::CarryOverBound;
			static LargeInt tempI;
			tempI.AssignShiftedUInt((unsigned int) lowPart,i+j);
			output.AddPositive(tempI);
			tempI.AssignShiftedUInt((unsigned int) highPart,i+j+1);
			output.AddPositive(tempI);
		}
	}
	output.sign= x.sign*this->sign;
	output.FitSize();
//	assert(this->CheckForConsistensy());
}

void LargeInt::FitSize()
{	int sizeDecrease=0;
	for (int i=this->size-1;i>=0;i--)
	{ if (this->TheObjects[i]==0)
		{	sizeDecrease++;
		}
		else
		{ break;
		}
	}
	this->size-=sizeDecrease;
	if (this->size==0)
		this->sign=1;
//	assert(this->CheckForConsistensy());
}

void LargeInt::MultiplyBy(LargeInt &x)
{ static LargeInt tempInt;
	this->MultiplyBy(x,tempInt);
	this->Assign(tempInt);
//	assert(this->CheckForConsistensy());
}

void LargeInt::MultiplyByInt(int x)
{	if (this->size==0) return;
	LargeInt tempI;
	tempI.AssignInt(x);
	this->MultiplyBy(tempI);
}

void LargeInt::ElementToString(std::string &output)
{	int base=10;
	int tempI;
	if (this->IsEqualToZero())
	{ output="0";
		return;
	}
	LargeInt tempInt;
	tempInt.Assign(*this);
	std::string tempS;
	std::stringstream out;
	while(!tempInt.IsEqualToZero() )
	{ tempI= tempInt%base;
		out<<tempI;
		tempInt= tempInt/base;
	}
	tempS= out.str();
	int startI;
	if (this->sign==-1)
	{ output.resize(tempS.size()+1);
		output[0]='-';
		startI=1;
	}else
	{ output.resize(tempS.size());
		startI=0;
	}
	for (unsigned int i=0; i<tempS.size();i++)
	{ output[startI+i]=tempS[tempS.size()-1-i];
	}
//	assert(this->CheckForConsistensy());
}

bool LargeInt::IsEqualToZero()
{	if (this->size==1)
	{	return this->TheObjects[0]==0;
	}
	return this->size==0;
}

bool LargeInt::IsEqualTo(LargeInt& x)
{ if (x.size!=this->size) return false;
	if (x.sign!=this->sign) return false;
	for (int i=0;i<this->size;i++)
	{ if(this->TheObjects[i]!=x.TheObjects[i]) return false;
	}
	return true;
}

bool LargeInt::CheckForConsistensy()
{ if (this->sign!=-1 && this->sign!=1)
		return false;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i]>=LargeInt::CarryOverBound)
			return false;
	}
	return true;
}

void LargeInt::AssignInt(int x)
{ if (x==0)
	{ this->MakeZero(); return;
	}
	this->SetSizeExpandOnTopNoObjectInit(1);
	if (x<0)
	{ this->TheObjects[0]= -x;
		this->sign=-1;
	}
	else
	{ this->TheObjects[0]=x;
		this->sign=1;
	}
	this->FitSize();
//	assert(this->CheckForConsistensy());
}

inline void LargeInt::AddInt(int x)
{ static LargeInt tempInt;
	tempInt.AssignInt(x);
	this->Add(tempInt);
}

void LargeInt::Add(LargeInt &x)
{ if (this->sign==x.sign)
	{ this->AddPositive(x);
	}
	else
	{ if (this->IsGEQByAbs(x))
		{	this->SubtractSmallerPositive(x);
		}
		else
		{ LargeInt tempI;
			tempI.Assign(*this);
			this->Assign(x);
			this->SubtractSmallerPositive(tempI);
		}
	}
//	assert(this->CheckForConsistensy());
}

/*void LargeInt::Minus_qDivisor(LargeInt& QuotientAccum,  LargeInt& q,LargeInt& divisor)
{ LargeInt tempI;
	q.MultiplyBy(divisor,tempI);
	this->SubtractSmallerPositive(tempI);
	QuotientAccum.Add(q);
}

void LargeInt::Minus_qDivisor
			(LargeInt& QuotientAccum, unsigned int q, int shift,LargeInt& divisor)
{ LargeInt tempI;
	tempI.Assign(LIOne);
	tempI.MultiplyByShifted(q,shift);
	QuotientAccum.Add(tempI);
	tempI.MultiplyBy(divisor);
	this->SubtractSmallerPositive(tempI);
}*/
#ifdef WIN32
#pragma warning(disable:4244)//warning 4244: data loss from conversion
#endif
void LargeInt::DivPositive(LargeInt &x, LargeInt& quotientOutput, LargeInt& remainderOutput) const
{	static LargeInt remainder,quotient,divisor;
	remainder.Assign(*this);	remainder.sign=1;
	divisor.Assign(x);	divisor.sign=1;
	quotient.MakeZero();
	while (remainder.IsGEQByAbs(divisor))
	{	unsigned int q =
			remainder.TheObjects[remainder.size-1]/(divisor.TheObjects[divisor.size-1]+1);
		int shift= remainder.size-divisor.size;
		assert(shift>=0);
		LargeInt current, oldTotal,Total;
		Total.Assign(divisor);
		if (q==0 && shift>=2){q=1; shift--;}
		if (q!=0)
		{ current.AssignShiftedUInt(q,shift);
			Total.MultiplyBy(current);
		}
		else
		{	current.Assign(LIOne);
		}
		oldTotal.Assign(Total);
		assert(remainder.IsGEQByAbs(Total));
		for (;;)
		{	Total.MultiplyByInt(2);
			if (!remainder.IsGEQByAbs(Total))
				break;
			current.MultiplyByInt(2);
			oldTotal.Assign(Total);
		}
		remainder.SubtractSmallerPositive(oldTotal);
		quotient.Add(current);
	}
	quotient.sign= this->sign*x.sign;
	if (remainder.size==0)
		remainder.sign=1;
	else
		remainder.sign= this->sign;
	remainderOutput.Assign(remainder);
	quotientOutput.Assign(quotient);
//	assert(remainderOut.CheckForConsistensy());
}
#ifdef WIN32
#pragma warning(default:4244)//warning 4244: data loss from conversion
#endif

void LargeInt::gcd(LargeInt &a, LargeInt &b, LargeInt &output)
{ LargeInt p,q,r,temp;
	static std::string tempSP, tempSQ, tempSR, tempS;
	p.Assign(a);
	q.Assign(b);
	p.sign=1;
	q.sign=1;
	if (LargeRational::AnErrorHasOccurredTimeToPanic)
	{ p.ElementToString(tempSP);
		q.ElementToString(tempSQ);
		r.ElementToString(tempSR);
		temp.ElementToString(tempS);
	}
	while(!q.IsEqualToZero() )
	{ if (LargeRational::AnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}
		p.DivPositive(q,temp,r);
		if (LargeRational::AnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}
		p.Assign(q);
		if (LargeRational::AnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}
		q.Assign(r);
		if (LargeRational::AnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}
	}
	output.Assign(p);
//	assert(output.CheckForConsistensy());
}

void LargeInt::MakeZero()
{ this->size=0;
	this->sign=1;
}

void LargeInt::Assign(const LargeInt &x)
{ this->sign=x.sign;
	this->CopyFromBase(x);
//	assert(this->CheckForConsistensy());
}

LargeInt LargeInt::operator/(int x)const
{ LargeInt result;
	LargeInt remainder;
	LargeInt tempX;
	tempX.AssignInt(x);
	this->DivPositive(tempX,result,remainder);
//	assert(result.CheckForConsistensy());
	return result;
}

LargeInt LargeInt::operator/(LargeInt& x)const
{ LargeInt result;
	LargeInt remainder;
	this->DivPositive(x,result,remainder);
	result.sign= this->sign* x.sign;
	assert(result.CheckForConsistensy());
	return result;
}

int LargeInt::operator%(int x)
{ assert(x>0);
	LargeInt result;
	LargeInt remainder;
	LargeInt tempX;
	tempX.AssignInt(x);
	this->DivPositive(tempX,result,remainder);
	if (remainder.size ==0)
	{return 0;
	}else
	{ return remainder.TheObjects[0];
	}
}

LargeInt::LargeInt(const LargeInt& x)
{ this->Assign(x);
//	this->CheckForConsistensy();
}

bool LargeInt::IsGEQByAbs(LargeInt& x)
{ if (this->size>x.size)
	{ return true;
	}
	if (this->size<x.size)
	{ return false;
	}
	for (int i=this->size-1;i>=0;i--)
	{ if (x.TheObjects[i]>this->TheObjects[i])
		{ return false;}
		if (x.TheObjects[i]<this->TheObjects[i])
		{ return true;}
	}
	return true;
}

void CompositeComplexQNPoly::AssignQP(QuasiPolynomial &q)
{ static Monomial<CompositeComplexQN> tempM;
	tempM.init(q.NumVars);
	this->ClearTheObjects();
	this->NumVars= q.NumVars;
	for (int i=0;i<q.size;i++)
	{ for (int j=0;j<this->NumVars;j++)
		{ tempM.degrees[j]=q.TheObjects[i].degrees[j];
		}
		q.TheObjects[i].Coefficient.QNtoComplex(tempM.Coefficient);
		this->AddMonomial(tempM);
	}
}

void partFractionPolynomials::ComputeQuasiPolynomial
			(QuasiPolynomial &output, bool RecordNumMonomials)
{	output.Nullify(root::AmbientDimension);
	for (int i=0;i<this->size;i++)
	{	if (RecordNumMonomials)
		{	std::stringstream out;
			out<<i<<" out of "<<this->size<<" accounted for";
			::IndicatorWindowGlobalVariables.StatusString= out.str();
		}
		static QuasiPolynomial tempQP;
		static QuasiNumber tempQN;
		tempQP.AssignPolynomialLargeRational(this->TheObjects[i]);
		//tempQP.ComputeDebugString();
		tempQN.MakeQNFromMatrixAndColumn
			(	this->theNormals,
				this->LatticeIndicators.TheObjects[i]);
		//tempQN.ComputeDebugString();
		tempQP.TimesConstant(tempQN);
		//tempQP.ComputeDebugString();
		if (partFraction::MakingConsistencyCheck)
		{ std::string tempS;
			LargeRational tempLR;
			tempQP.Evaluate(partFraction::theVectorToBePartitioned,tempLR);
			assert (tempLR.den.IsEqualTo(LIOne));
			partFractions::CheckSum.Add(tempLR);
			//partFraction::CheckSum.Add(tempLR);
			assert(partFractions::CheckSum.den.IsEqualTo(LIOne));
		}
		output.AddPolynomial(tempQP);
		//output.ComputeDebugString();
	}
}

void partFractionPolynomials::initLatticeIndicatorsFromPartFraction(partFraction &owner)
{	this->LatticeIndicators.size=0;
	this->theNormals.init(root::AmbientDimension,root::AmbientDimension);
	for (int i=0;i<root::AmbientDimension;i++)
	{ int tempI=owner.IndicesNonZeroMults.TheObjects[i];
		assert(owner.TheObjects[tempI].Elongations.size==1);
		for(int j=0;j<root::AmbientDimension;j++)
		{	this->theNormals.elements[j][i].AssignInteger
					(partFraction::RootsToIndices.TheObjects
						[tempI].elements[j]*
						owner.TheObjects[tempI].Elongations.TheObjects[0]
						);
		}
	}
	this->theNormals.Invert();
	//this->theNormals.ComputeDebugString();
	this->size=0;
}

void partFractionPolynomials::AddPolynomialLargeRational
				(root& rootLatticeIndicator,PolynomialLargeRational& input)
{ static root tempRoot, tempRoot2;
	tempRoot.dimension=root::AmbientDimension; tempRoot2.dimension=root::AmbientDimension;
	static Rational tempRat, tempRat2;
	//if (partFraction::MakingConsistencyCheck)
	//{ this->CheckConsistency(rootLatticeIndicator,input);
	//}
	//tempRoot.MakeZero();
	if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ this->theNormals.ComputeDebugString();
	}
	for(int i=0;i<this->theNormals.NumRows;i++)
	{ tempRoot.coordinates[i].MakeZero();
		if (partFraction::MakingConsistencyCheck)
		{ tempRoot2.coordinates[i].MakeZero();
		}
		for(int j=0;j<root::AmbientDimension;j++)
		{ tempRat.Assign(this->theNormals.elements[i][j]);
			tempRat.MultiplyBy(rootLatticeIndicator.coordinates[j]);
			tempRoot.coordinates[i].Add(tempRat);
			if (partFraction::MakingConsistencyCheck)
			{	if (partFraction::AnErrorHasOccurredTimeToPanic)
				{ Stop();
				}
				tempRat2.AssignInteger(partFraction::theVectorToBePartitioned.elements[j]);
				tempRat2.MultiplyBy(this->theNormals.elements[i][j]);
				tempRoot2.coordinates[i].Add(tempRat2);
			}
		}
		tempRoot.coordinates[i].AssignFracValue();
		if (partFraction::MakingConsistencyCheck)
		{ tempRoot2.coordinates[i].AssignFracValue();
		}
	}
	if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ tempRoot.ComputeDebugString();
		tempRoot2.ComputeDebugString();
	}
	if (partFraction::MakingConsistencyCheck)
	{	if (tempRoot2.IsEqualTo(tempRoot))
		{ LargeRational tempLRat;
			std::string tempS1, tempS2;
			if (partFraction::AnErrorHasOccurredTimeToPanic)
			{ input.ComputeDebugString();
			}
			input.Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
			tempLRat.ElementToString(tempS1);
			assert(tempLRat.den.IsEqualTo(LIOne));
	//		tempLRat.ElementToString(tempS1);
			partFraction::CheckSum.Add(tempLRat);
			if (partFraction::AnErrorHasOccurredTimeToPanic)
			{ partFraction::CheckSum.ElementToString(tempS1);
				RandomCodeIDontWantToDelete::theFirstEvilString.append(tempS1);
				RandomCodeIDontWantToDelete::theFirstEvilString.append("\n");
			}
		}
	}

	int x= this->LatticeIndicators.ContainsObject(tempRoot);
	if (x==-1)
	{ this->LatticeIndicators.AddRoot(tempRoot);
		this->AddObjectOnTop(input);
	}
	else
	{ this->TheObjects[x].AddPolynomial(input);
		if (partFraction::MakingConsistencyCheck)
		{	if (tempRoot2.IsEqualTo(tempRoot))
			{ LargeRational tempLRat;
				this->TheObjects[x].Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
				assert(tempLRat.IsEqualTo(partFraction::CheckSum));
			}
		}
	}
}

void partFractionPolynomials::CheckConsistency
				(root& RootLatticeIndicator,PolynomialLargeRational& input)
{ root tempRoot2;
	tempRoot2.AssignIntRoot(partFraction::theVectorToBePartitioned);
	tempRoot2.Subtract(RootLatticeIndicator);
	intRoot tempIntRoot;
	tempIntRoot.AssignRoot(tempRoot2);
	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	Rational tempRat;
	for(int i=0;i<this->theNormals.NumRows;i++)
	{ tempRoot.coordinates[i].MakeZero();
		for(int j=0;j<root::AmbientDimension;j++)
		{ tempRat.Assign(this->theNormals.elements[i][j]);
			tempRat.MultiplyBy(tempRoot2.coordinates[j]);
			tempRoot.coordinates[i].Add(tempRat);
		}
		tempRoot.coordinates[i].AssignFracValue();
	}
	LargeRational tempLRat;
	if (tempRoot.IsEqualToZero())
	{ input.Evaluate( tempIntRoot,tempLRat);
		std::string tempS;
		tempLRat.ElementToString(tempS);
		assert(tempLRat.den.IsEqualTo(LIOne));
	}
}

bool partFraction::RemoveRedundantShortRootsClassicalRootSystem(root*Indicator)
{ bool result=false;
	if (Indicator!=0)
	{ if (!this->rootIsInFractionCone(*Indicator))
			return false;
	}
	for (int i=0;i<partFraction::RootsToIndices.
										IndicesRedundantShortRoots.CardinalitySelection;i++  )
	{ int tempI1, tempI2;
		tempI1= partFraction::RootsToIndices.IndicesRedundantShortRoots.elements[i];
		tempI2= partFraction::RootsToIndices.IndicesDoublesOfRedundantShortRoots[tempI1];
		if(tempI2!=-1)
		{	if (	this->TheObjects[tempI1].Multiplicities.size>0 &&
						this->TheObjects[tempI2].Multiplicities.size>0)
			{ static IntegerPoly tempP;
				assert(this->TheObjects[tempI1].Multiplicities.size==1 && this->TheObjects[tempI2].Multiplicities.size==1);
				tempP.ClearTheObjects();
				tempP.NumVars= root::AmbientDimension;
				tempP.MakePolyExponentFromIntRoot(partFraction::RootsToIndices.TheObjects[tempI1]);
				tempP.AddConstant(IOne);
				//tempP.ComputeDebugString();
				for (int j=0;j<this->TheObjects[tempI1].Multiplicities.TheObjects[0];j++)
				{ this->Coefficient.MultiplyBy(tempP);
				}
				//this->Coefficient.ComputeDebugString();
				this->TheObjects[tempI2].AddMultiplicity(this->TheObjects[tempI1].Multiplicities.TheObjects[0],1);
				this->TheObjects[tempI1].AddMultiplicity(-this->TheObjects[tempI1].Multiplicities.TheObjects[0],1);
				assert(this->TheObjects[tempI1].Multiplicities.size==0);
				result=true;
			}
		}
	}
	this->ComputeIndicesNonZeroMults();
	return result;
}

bool partFraction::RemoveRedundantShortRoots(root* Indicator)
{ bool result=false;
	LargeRational StartCheckSum,LocalCheckSum, LocalCheckSum2;
	//if (partFraction::MakingConsistencyCheck)
	//{	this->ComputeDebugString();
	//	this->ComputeOneCheckSum(StartCheckSum);
	//}
	if (Indicator!=0)
	{ if (!this->rootIsInFractionCone(*Indicator))
			return false;
	}
	for (int k=0;k<this->IndicesNonZeroMults.size;k++  )
	{ int currentIndex=this->IndicesNonZeroMults.TheObjects[k];
		::oneFracWithMultiplicitiesAndElongations& currentFrac =
				 this->TheObjects[currentIndex];
		int LCMElongations = currentFrac.GetLCMElongations();
		std::string tempS, tempS1, tempS2;
		partFraction::RootsToIndices.TheObjects[currentIndex].ElementToString(tempS);
		while (currentFrac.Elongations.size>1)
		{ for (int i=0;i<currentFrac.Elongations.size;i++)
			{	int ElongationValue=currentFrac.Elongations.TheObjects[i];
				if (ElongationValue!=LCMElongations)
				{ static IntegerPoly tempP;
					int numSummands=LCMElongations/ElongationValue;
					this->GetNElongationPoly(currentIndex,ElongationValue,numSummands,tempP);
					//tempP.ComputeDebugString();
					tempP.RaiseToPower(currentFrac.Multiplicities.TheObjects[i]);
					//tempP.ComputeDebugString();
					this->Coefficient.MultiplyBy(tempP);
					//this->Coefficient.ComputeDebugString();
					currentFrac.AddMultiplicity(currentFrac.Multiplicities.TheObjects[i],LCMElongations);
					currentFrac.AddMultiplicity(-currentFrac.Multiplicities.TheObjects[i],ElongationValue);
					result=true;
				}
			}
		}
	/*	if (partFraction::MakingConsistencyCheck)
		{ this->ComputeOneCheckSum(LocalCheckSum2);
			LocalCheckSum2.ElementToString(tempS1);
			StartCheckSum.ElementToString(tempS2);
			assert(LocalCheckSum2.IsEqualTo(StartCheckSum));
		}*/
	}
	return result;
}

bool partFraction::reduceOnceTotalOrderMethod(partFractions &Accum)
{ for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ for (int j=0;j<this->IndicesNonZeroMults.size;j++)
		{ //assert (this->IndicesNonZeroMults[i]<this->IndicesNonZeroMults[j]);
			int AminusBindex = partFraction::RootsToIndices.
												 TableAllowedAminusB[this->IndicesNonZeroMults.TheObjects[i]]
												 [this->IndicesNonZeroMults.TheObjects[j]];
			int Aminus2Bindex = partFraction::RootsToIndices.
												  TableAllowedAminus2B[this->IndicesNonZeroMults.TheObjects[i]]
												  [this->IndicesNonZeroMults.TheObjects[j]];
			if (AminusBindex!=-1 &&	AminusBindex>this->IndicesNonZeroMults.TheObjects[j])
			{ this->decomposeAMinusNB( this->IndicesNonZeroMults.TheObjects[i],
																this->IndicesNonZeroMults.TheObjects[j],1,AminusBindex, Accum);
				return true;
			}
			if (Aminus2Bindex!=-1 &&	Aminus2Bindex>this->IndicesNonZeroMults.TheObjects[j])
			{ this->decomposeAMinusNB( this->IndicesNonZeroMults.TheObjects[i],
																this->IndicesNonZeroMults.TheObjects[j],2,Aminus2Bindex, Accum);
				return true;
			}
		}
	}
//	Accum.Add(*this);
	return false;
}

bool partFraction::reduceOnceGeneralMethod(partFractions &Accum)
{ static roots tempRoots;
	static MatrixRational tempMat;
	tempRoots.size=0;
	int IndexInLinRelationOfLastGainingMultiplicityIndex=-1;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ static intRoot tempRoot; tempRoot.dimension=root::AmbientDimension;
		int currentIndex= this->IndicesNonZeroMults.TheObjects[i];
		if (currentIndex== this->LastGainingMultiplicityIndex)
		{ IndexInLinRelationOfLastGainingMultiplicityIndex=i;
		}
		tempRoot= partFraction::RootsToIndices.TheObjects[currentIndex];
		tempRoot.MultiplyByInteger
			(this->TheObjects[currentIndex].GetLargestElongation());
		tempRoots.AddIntRoot(tempRoot);
		bool ShouldDecompose;
		ShouldDecompose = tempRoots.GetLinearDependence(tempMat);
	//	if (partFraction::AnErrorHasOccurredTimeToPanic)
	//	{ tempMat.ComputeDebugString();
	//	}
	//	tempMat.ComputeDebugString();
		if (ShouldDecompose && this->LastGainingMultiplicityIndex!=-1)
		{ if (IndexInLinRelationOfLastGainingMultiplicityIndex==-1)
			{	ShouldDecompose=false;}
			else
			{	ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
			}
		}
		if (ShouldDecompose)
		{	if (this->AnErrorHasOccurredTimeToPanic)
			{ this->ComputeDebugString();
			}
			this->DecomposeFromLinRelation(tempMat,Accum);
			//this->ComputeDebugString();
			return true;
		}
	}
	this->LastGainingMultiplicityIndex=-1;
	return false;
	//tempRoots.r
}

int partFraction::SizeWithoutDebugString()
{	int Accum =0;
	Accum+=this->Coefficient.SizeWithoutDebugString();
	Accum+=this->ListBasicObjects<oneFracWithMultiplicitiesAndElongations>::SizeWithoutObjects();
	Accum+=this->ActualSize*sizeof(oneFracWithMultiplicitiesAndElongations);
	Accum+=	this->IndicesNonZeroMults.SizeWithoutObjects();
	return Accum;
}

void partFraction::Assign(const partFraction& p)
{ this->CopyFromBase(p);
	this->Coefficient.CopyFromPoly(p.Coefficient);
	this->IndicesNonZeroMults.CopyFromBase(p.IndicesNonZeroMults);
}

void partFraction::AssignNoIndicesNonZeroMults(partFraction& p)
{ this->CopyFromBase(p);
	this->Coefficient.CopyFromPoly(p.Coefficient);
}

/*void partFraction::swap(int indexA, int indexB)
{ for (int i=0;i<root::AmbientDimension;i++)
	{ int tempI = this->TheObjects[indexA].den[i];
		this->TheObjects[indexA].den[i]= this->TheObjects[indexB].den[i];
		this->TheObjects[indexB].den[i]= tempI;
	}
	int tempI= this->TheObjects[indexA].Mult;
	this->TheObjects[indexA].Mult= this->TheObjects[indexB].Mult;
	this->TheObjects[indexB].Mult= tempI;
}*/

int partFraction::ElementToString(	std::string &output, bool LatexFormat,
																		bool includeVPsummand, bool includeNumerator)
{ static MatrixInt tempMat;
	return this->ElementToStringBasisChange
		(tempMat,false,output,LatexFormat, includeVPsummand, includeNumerator);
}

void partFraction::WriteToFile(std::fstream &output)
{/* output<<"Fraction_start\n";
	for (int j=0;j<partFraction::RootsToIndices.size;j++)
	{ output << this->TheObjects[j].Mult <<" ";
	}
	output<<"\nCoefficient:\n";
	this->Coefficient.WriteToFile(output);
	output<<"Fraction_end\n";*/
}

void partFraction::ReadFromFile(std::fstream &input)
{/* std::string tempS;
	input >>tempS;
	assert(tempS=="Fraction_start");
	for (int i=0;i<partFraction::RootsToIndices.size;i++)
	{ input >>this->TheObjects[i].Mult;
	}
	input >>tempS;
	this->Coefficient.ReadFromFile(input,root::AmbientDimension);
	input>>tempS;
	this->ComputeIndicesNonZeroMults();*/
}

void partFraction::ComputeOneCheckSum(LargeRational &output)
{ if (this->AnErrorHasOccurredTimeToPanic)
	{ this->Coefficient.ComputeDebugString();
	}
	this->Coefficient.Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot,output);
	std::string tempS;
	if (this->AnErrorHasOccurredTimeToPanic && LargeRational::AnErrorHasOccurredTimeToPanic)
	{ output.ElementToString(tempS);
	}
	//output.ElementToString(tempS);
	LargeRational tempRat;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]]
			.ComputeOneCheckSum(tempRat,partFraction::RootsToIndices.TheObjects
				[this->IndicesNonZeroMults.TheObjects[i]]);
		//tempRat.ElementToString(tempS);
		output.MultiplyBy(tempRat);
		//output.ElementToString(tempS);
	}
}

#ifdef WIN32
#pragma warning(disable:4018)//grrrrr
#endif
int partFraction::ElementToStringBasisChange(MatrixInt& VarChange,
																	bool UsingVarChange, std::string& output,
																	bool LatexFormat, bool includeVPsummand, bool includeNumerator)
{ std::stringstream out, out2;
	std::string tempS, stringPoly;
	if (this->Coefficient.size==0)
	{output.clear(); return 0;}
	int NumLinesUsed=0;
//	int OldCutOff=0;
	if (!UsingVarChange)
	{	if (includeNumerator)
			NumLinesUsed+=this->Coefficient.StringPrintOutAppend (stringPoly,PolyFormatLocal);
		else
			stringPoly="(...)";
	}
	else
	{ static PolynomialRationalCoeff tempP,tempP2;
		static Polynomials<Rational> tempSub;
		tempP.Nullify(VarChange.NumRows);
		tempSub.MakeExponentSubstitution(VarChange);
		tempP2.AssignIntegerPoly(this->Coefficient);
		tempP2.Substitution(tempSub,tempP,VarChange.NumRows);
		NumLinesUsed+=tempP.StringPrintOutAppend(stringPoly, PolyFormatLocal);
	}
	if (stringPoly=="1"){stringPoly="";}
	if (stringPoly=="-1"){stringPoly="-";}
	if (this->Coefficient.size>1)
	{out2<<"(";}
	out2<<stringPoly;
	if (this->Coefficient.size>1)
	{out2<<")";}
	stringPoly= out2.str();
//	this->intRootToString(out,this->RootShift,false);
	out<<" ";
	for (int i =0;i<this->size;i++)
	{ this->TheObjects[i].ElementToStringBasisChange(VarChange,UsingVarChange,
																									 tempS,LatexFormat,i);
		out<<tempS;
	}
	if (includeVPsummand && LatexFormat)
	{	std::string tempS2;
		QuasiPolynomial tempQP;
		tempQP.Nullify(root::AmbientDimension);
		this->partFractionToPartitionFunctionSplit(tempQP,false,true,false);
		PolyFormatLocal.MakeAlphabetyi();
		PolyFormatLocal.cutOffString=false;
		tempQP.StringPrintOutAppend(tempS2, PolyFormatLocal);
		PolyFormatLocal.MakeAlphabetxi();
		PolyFormatLocal.cutOffString=false;
		out <<"\\\\\n&&[";
		NumLinesUsed++;
		out <<tempS2;
		out<<"]";
	}
	tempS= out.str();
	if (LatexFormat)
	{ //NumLinesUsed+=this->ControlLineSizeStringPolys(stringPoly);
		//NumLinesUsed+=this->ControlLineSizeFracs(tempS);
		output.clear();
		output.append(stringPoly);
		if (stringPoly.size()>(unsigned) PolyFormatLocal.LatexMaxLineLength)
		{	output.append("\\\\\n&&");
			NumLinesUsed++;
		}
		output.append(tempS);
	}
	else
	{ output.clear();
		output.append(stringPoly);
		output.append(tempS);
	}
	return NumLinesUsed;
}
#ifdef WIN32
#pragma warning(default:4018)//grrrrr
#endif

void partFraction::MultiplyMinusRootShiftBy(int *theRoot, int Multiplicity)
{ /*for (int i=0;i<root::AmbientDimension;i++)
	{ this->RootShift[i]+=theRoot[i]*Multiplicity;
	}*/
	static Monomial<Integer> tempM;
	tempM.MakeConstantMonomial(root::AmbientDimension,IOne);
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempM.degrees[i]=(short)(theRoot[i]*Multiplicity);
	}
	this->Coefficient.MultiplyByMonomial(tempM);
}

void partFraction::intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent)
{	for (int i=0;i<root::AmbientDimension;i++)
	{ int tempI = TheRoot[i];
		if (MinusInExponent) {tempI=-tempI;}
		if (tempI!=0)
		{ out<<PolyFormatLocal.alphabet[i];
			if(tempI<0)
			{ out<<"^{"<<tempI<<"}";
			}
			if(tempI>1)
			{ out<< "^"<<tempI;
			}
		}
	}
}

bool partFraction::rootIsInFractionCone(root&r)
{ //assert(this->IndicesNonZeroMults.size==root::AmbientDimension);
	static MatrixRational tempMat, MatOneCol;
	static Selection NonPivotPoints;
	Cone tempCone; roots tempRoots;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ int tempI= this->IndicesNonZeroMults.TheObjects[i];
		tempRoots.AddIntRoot(partFraction::RootsToIndices.TheObjects[tempI]);
	}
	tempCone.ComputeFromDirections(tempRoots);
	tempCone.ComputeDebugString();
	return tempCone.IsInCone(r);
}

void partFraction::PrepareFraction( int indexA, int indexB,  int AminusNBindex,
																		bool indexAisNullified, partFraction &output,
																		IntegerPoly& AminusNbetaPoly)
{	output.AssignNoIndicesNonZeroMults(*this);
	int powerDropA = this->TheObjects[indexA].Multiplicities.TheObjects[0];
	int powerDropB = this->TheObjects[indexB].Multiplicities.TheObjects[0];
	if (indexAisNullified)
	{ powerDropB=0;
	}else
	{ powerDropA=0;
	}
	for (int i=0;i<powerDropB;i++)
	{ output.Coefficient.MultiplyBy(AminusNbetaPoly);
	}
	output.DecreasePowerOneFrac(indexA,powerDropA);
	output.DecreasePowerOneFrac(indexB,powerDropB);
	output.DecreasePowerOneFrac(AminusNBindex,-powerDropA-powerDropB);
}

int partFraction::ComputeGainingMultiplicityIndexInLinearRelation
				(	MatrixRational& theLinearRelation)
{ int DesireToSelectAsGainingMultiplicity=-1;
	int result=-1;
	for( int i=0;i<theLinearRelation.NumRows;i++)
	{ if(! theLinearRelation.elements[i][0].IsEqualToZero())
		{	int currentIndex= this->IndicesNonZeroMults.TheObjects[i];
			int candidateDesire=this->TheObjects[currentIndex].GetTotalMultiplicity();
			if (candidateDesire<0)
				candidateDesire=-candidateDesire;
			if (result==-1 || DesireToSelectAsGainingMultiplicity<candidateDesire)
			{	result=i;
				DesireToSelectAsGainingMultiplicity = candidateDesire;
			}
		}
	}
	return result;
}

bool partFraction::DecomposeFromLinRelation(MatrixRational& theLinearRelation, partFractions& Accum)
{//	theLinearRelation.ComputeDebugString();
	//theLinearRelation.ComputeDebugString();
	int GainingMultiplicityIndexInLinRelation=-1;
	int GainingMultiplicityIndex=-1;
	int ElongationGainingMultiplicityIndex=-1;
	static ListBasicObjects<int> ParticipatingIndices;
	static ListBasicObjects<int> theElongations;
	LargeRational oldCheckSum;
/*	if(this->AnErrorHasOccurredTimeToPanic)
	{ Accum.ComputeOneCheckSum(oldCheckSum);
	} */
	ParticipatingIndices.size=0;
	theElongations.size=0;
	//this->ComputeDebugString();
	//Accum.ComputeDebugString();
	GainingMultiplicityIndexInLinRelation =
		this->ComputeGainingMultiplicityIndexInLinearRelation(theLinearRelation);
	GainingMultiplicityIndex= this->IndicesNonZeroMults
																.TheObjects[GainingMultiplicityIndexInLinRelation];
	int tempI=this->TheObjects[GainingMultiplicityIndex].GetLargestElongation();
	theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].MultiplyByInteger(tempI);
	//theLinearRelation.ComputeDebugString();
	theLinearRelation.ScaleToIntegralForMinRationalHeight();
	//theLinearRelation.ComputeDebugString();
	ElongationGainingMultiplicityIndex= theLinearRelation
																				.elements[GainingMultiplicityIndexInLinRelation][0].num;
	if (ElongationGainingMultiplicityIndex<0)
	{	ElongationGainingMultiplicityIndex=-ElongationGainingMultiplicityIndex;
	}
	else
	{ theLinearRelation.MultiplyByInt(-1);
	}
	//theLinearRelation.ComputeDebugString();
	for (int i=0;i<theLinearRelation.NumRows;i++)
	{ if (i!=GainingMultiplicityIndexInLinRelation && theLinearRelation.elements[i][0].num!=0)
		{	ParticipatingIndices.AddObjectOnTop(this->IndicesNonZeroMults.TheObjects[i]);
			theElongations.AddObjectOnTop(theLinearRelation.elements[i][0].num);
		}
	}
	this->LastGainingMultiplicityIndex=GainingMultiplicityIndex;
	/*if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString();
	}*/
	//if (this->MakingConsistencyCheck)
	//{ this->ComputeOneCheckSum(this->CheckSum2);
	//}
	this->ApplySzenesVergneFormula
		(	ParticipatingIndices,theElongations,GainingMultiplicityIndex,
			ElongationGainingMultiplicityIndex,Accum);
	//if (this->MakingConsistencyCheck)
	//{ assert(this->CheckSum2.IsEqualTo(this->CheckSum));
	//}
	/*if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ LargeRational tempRat2, tempRat;
		std::string tempS1,tempS2;
		Accum.ComputeOneCheckSum(tempRat2);
		this->ComputeOneCheckSum(tempRat);
		tempRat.ElementToString(tempS1);
		this->CheckSum2.ElementToString(tempS2);
		tempRat2.Subtract(tempRat);
		assert(oldCheckSum.IsEqualTo(tempRat2));
	}*/
	//Accum.ComputeDebugString();
	return true;
}

void partFraction::ApplySzenesVergneFormula
			(	ListBasicObjects<int> &theSelectedIndices, ListBasicObjects<int>& theElongations,
				int GainingMultiplicityIndex,int ElongationGainingMultiplicityIndex,
				partFractions& Accum)
{	static partFraction tempFrac;
	static IntegerPoly tempP;
	std::string tempSX;
/*	if (this->AnErrorHasOccurredTimeToPanic)
	{ this->CheckSum2.MakeZero();
		this->ComputeOneCheckSum(this->CheckSum);
		std::string tempS;
		this->CheckSum.ElementToString(tempS);
		this->ComputeDebugString();
		this->DebugString.append("-> ");
		this->DebugString.append(tempS);
	}*/
//if (this->MakingConsistencyCheck)
	//{ this->ComputeOneCheckSum(this->CheckSum);
	//}
	for(int i=0;i<theSelectedIndices.size;i++)
	{	tempFrac.Assign(*this);
		tempFrac.TheObjects[GainingMultiplicityIndex].AddMultiplicity
			(1,ElongationGainingMultiplicityIndex);
		oneFracWithMultiplicitiesAndElongations& currentFrac=
			tempFrac.TheObjects[theSelectedIndices.TheObjects[i]];
		int LargestElongation= currentFrac.GetLargestElongation();
		currentFrac.AddMultiplicity
			(-1,LargestElongation);
		static Monomial<Integer> tempM;
		tempM.init(root::AmbientDimension);
		tempM.Coefficient.Assign(IOne);
		for (int j=0;j<i;j++)
		{	short tempElongation=(short) this->TheObjects[theSelectedIndices.TheObjects[j]].GetLargestElongation();
			for (int k=0;k<root::AmbientDimension;k++)
			{ tempM.degrees[k]+=(short)theElongations.TheObjects[j]*tempElongation*
													(short)partFraction::RootsToIndices.
														TheObjects[theSelectedIndices.TheObjects[j]].elements[k];
			}
		}
		ParallelComputing::SafePoint();
		tempFrac.Coefficient.MultiplyByMonomial(tempM);
		this->GetNElongationPoly
				(theSelectedIndices.TheObjects[i],LargestElongation,theElongations.TheObjects[i],tempP);
		tempFrac.Coefficient.MultiplyBy(tempP);
		tempFrac.ComputeIndicesNonZeroMults();
	/*	if (this->AnErrorHasOccurredTimeToPanic)
		{ //tempFrac.ComputeDebugString();
			LargeRational tempRat;
			tempFrac.ComputeOneCheckSum(tempRat);
			std::string tempS1, tempS2;
			tempRat.ElementToString(tempS1);
			this->CheckSum2.Add(tempRat);
			tempFrac.ComputeDebugString();
			tempSX.append(tempFrac.DebugString);
			tempSX.append("-> ");
			tempSX.append(tempS1);
			tempSX.append("  + \\\\&&");
		}*/
		Accum.Add(tempFrac);
		/*if (this->AnErrorHasOccurredTimeToPanic)
		{	Accum.ComputeDebugString();
		}*/
	}
	//if (this->AnErrorHasOccurredTimeToPanic)
	//{ assert(this->CheckSum.IsEqualTo(this->CheckSum2));
	//}
}

void partFraction::decomposeAMinusNB(int indexA, int indexB, int n,
																	   int indexAminusNB, partFractions& Accum)
{	static partFraction tempFrac;
	static IntegerPoly AminusNbetaPoly;
	this->GetAlphaMinusNBetaPoly(indexA,indexB,n,AminusNbetaPoly);
	int powerA= this->TheObjects[indexA].Multiplicities.TheObjects[0];
	int powerB= this->TheObjects[indexB].Multiplicities.TheObjects[0];
	this->PrepareFraction(indexA,indexB,indexAminusNB,true,tempFrac,AminusNbetaPoly);
//	tempFrac.ComputeDebugString();
	if (partFractions::SplitTestMode)
	{ tempFrac.Coefficient.MakeNVarConst(root::AmbientDimension,IOne);
		tempFrac.TheObjects[indexAminusNB].Multiplicities.TheObjects[0]=1;
		tempFrac.TheObjects[indexB].Multiplicities.TheObjects[0]=1;
		tempFrac.TheObjects[indexA].Multiplicities.TheObjects[0]=0;
		tempFrac.ComputeIndicesNonZeroMults();
		Accum.Add(tempFrac);
		tempFrac.TheObjects[indexB].Multiplicities.TheObjects[0]=0;
		tempFrac.TheObjects[indexA].Multiplicities.TheObjects[0]=1;
		tempFrac.ComputeIndicesNonZeroMults();
		Accum.Add(tempFrac);
		return;
	}
	for (int i=powerB;i>=1;i--)
	{	Integer tempInt(NChooseK(powerA+powerB-i-1,powerA-1));
		tempFrac.Coefficient.TimesConstant(tempInt);
		Accum.Add(tempFrac);
		tempFrac.Coefficient.DivideByConstant(tempInt);
//		Accum.ComputeDebugString();
		if (i>1)
		{	tempFrac.TheObjects[indexAminusNB].AddMultiplicity(1,1);
			tempFrac.TheObjects[indexB].AddMultiplicity(-1,1);
			tempFrac.Coefficient.MultiplyBy(AminusNbetaPoly);
	//		tempFrac.ComputeDebugString();
		}
	}
	this->PrepareFraction(indexA,indexB,indexAminusNB,false,tempFrac,AminusNbetaPoly);
//	tempFrac.ComputeDebugString();
	for (int i=powerA;i>=1;i--)
	{	Integer tempInt(NChooseK(powerA+powerB-i-1,powerB-1));
		tempFrac.Coefficient.TimesConstant(tempInt);
		Accum.Add(tempFrac);
		tempFrac.Coefficient.DivideByConstant(tempInt);
//		Accum.ComputeDebugString();
		if (i>1)
		{	tempFrac.TheObjects[indexAminusNB].AddMultiplicity(1,1);;
			tempFrac.TheObjects[indexA].AddMultiplicity(-1,1);;
//			tempFrac.ComputeDebugString();
		}
	}
//	Accum.ComputeDebugString();
}

bool partFraction::DecreasePowerOneFrac(int index, int increment)
{ this->TheObjects[index].AddMultiplicity(-increment,1);
	this->ComputeIndicesNonZeroMults();
	return true;
}

void partFraction::ComputeIndicesNonZeroMults()
{ this->IndicesNonZeroMults.size=0;
	for (int i=0;i<this->size;i++)
	{ if(this->TheObjects[i].Multiplicities.size>0)
		{ this->IndicesNonZeroMults.AddObjectOnTop(i);
		}
	}
}

void partFraction::GetAlphaMinusNBetaPoly(int indexA, int indexB,
																					int n, IntegerPoly& output)
{	output.Nullify(root::AmbientDimension);
	Monomial<Integer> tempM;
	tempM.init(root::AmbientDimension);
	tempM.Coefficient.Assign(IMOne);
	for (int i=0;i<n;i++)
	{ for (int j=0;j<root::AmbientDimension;j++)
		{ tempM.degrees[j]= (short)(partFraction::RootsToIndices.TheObjects[indexA].elements[j]
												- (i+1)*partFraction::RootsToIndices.TheObjects[indexB].elements[j]);
		}
		output.AddMonomial(tempM);
	}
//	output.ComputeDebugString();
}

void partFraction::GetNElongationPoly(int index,int Elongation,int n, IntegerPoly& output)
{	output.Nullify(root::AmbientDimension);
	Monomial<Integer> tempM;
	tempM.init(root::AmbientDimension);
	if (n>0)
	{	tempM.Coefficient.Assign(IOne);
		for (int i=0;i<n;i++)
		{ for (int j=0;j<root::AmbientDimension;j++)
			{ tempM.degrees[j]=(short)
					(Elongation*i*partFraction::RootsToIndices.TheObjects[index].elements[j]);
			}
			output.AddMonomial(tempM);
		}
	}
	else
	{ assert(n<0);
		tempM.Coefficient.Assign(IMOne);
		for (int i=-1;i>=n;i--)
		{ for (int j=0;j<root::AmbientDimension;j++)
			{ tempM.degrees[j]=short(Elongation*i*partFraction::RootsToIndices.TheObjects[index].elements[j]);
			}
			output.AddMonomial(tempM);
		}
	}
	//output.ComputeDebugString();
}

void partFraction::partFractionToPartitionFunctionStoreAnswer
				(QuasiPolynomial& output, bool RecordSplitPowerSeriesCoefficient, bool StoreToFile)
{	if (!this->PowerSeriesCoefficientIsComputed)
	{ this->partFractionToPartitionFunctionSplit
			(output,true,RecordSplitPowerSeriesCoefficient, StoreToFile);
		this->PowerSeriesCoefficient.CopyFromPoly(output);
		this->PowerSeriesCoefficientIsComputed=true;
		IndicatorWindowGlobalVariables.NumComputedContributions++;
		FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
	}
	else
	{ output.CopyFromPoly(this->PowerSeriesCoefficient);
	}
}

void partFraction::partFractionToPartitionFunctionSplit
			(QuasiPolynomial& output, bool RecordNumMonomials,
			 bool RecordSplitPowerSeriesCoefficient, bool StoreToFile)
{	static PolynomialLargeRational shiftedPoly, tempP;
	static roots normals;
	static partFractionPolynomials tempSplitPowerSeriesCoefficient;
	static partFractionPolynomials* SplitPowerSeriesCoefficient;
	if (partFraction::MakingConsistencyCheck)
	{	partFraction::CheckSum.MakeZero();
	}
	if (this->FileStoragePosition!=-1)
	{ partFraction::TheBigDump.seekg(this->FileStoragePosition);
		output.ReadFromFile(partFraction::TheBigDump,root::AmbientDimension);
		if (RecordNumMonomials && ! this->AlreadyAccountedForInGUIDisplay)
		{ this->AlreadyAccountedForInGUIDisplay=true;
			IndicatorWindowGlobalVariables.NumProcessedMonomials+=this->Coefficient.size;
			::IndicatorWindowGlobalVariables.StatusString.clear();
			::FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
		}
//		output.ComputeDebugString();
		return;
	}
	if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString();
	}
	if(RecordSplitPowerSeriesCoefficient)
	{ SplitPowerSeriesCoefficient= &this->SplitPowerSeriesCoefficients;
	}
	else
	{ SplitPowerSeriesCoefficient= &tempSplitPowerSeriesCoefficient;
	}
	SplitPowerSeriesCoefficient->initLatticeIndicatorsFromPartFraction(*this);
//	SplitPowerSeriesCoefficient->InvertedMatrixRoots.ComputeDebugString();
	normals.AssignMatrixRows(SplitPowerSeriesCoefficient->theNormals);
	if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString();
		normals.ComputeDebugString();
		Stop();
	}
	for (int i=0;i<this->Coefficient.size;i++)
	{	this->ComputePolyCorrespondingToOneMonomial
			(shiftedPoly,i,normals,SplitPowerSeriesCoefficient);

		if (RecordNumMonomials)
		{ ::IndicatorWindowGlobalVariables.NumProcessedMonomials++;
			std::stringstream out;
			out <<"Current fraction: "<<i+1<<" out of "<<this->Coefficient.size
					<<" processed";
			::IndicatorWindowGlobalVariables.ProgressReportString4= out.str();
			::FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
		}
	}
	SplitPowerSeriesCoefficient->ComputeQuasiPolynomial(output, RecordNumMonomials);
	if (partFraction::AnErrorHasOccurredTimeToPanic)
	{ Stop();
	}
	if (partFraction::MakingConsistencyCheck)
	{ LargeRational tempLRat;
		output.Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
		assert(tempLRat.den.IsEqualTo(LIOne));
		assert(tempLRat.IsEqualTo(partFraction::CheckSum));
	}
	if (StoreToFile)
	{ this->FileStoragePosition= partFraction::TheBigDump.tellp();
		output.WriteToFile(partFraction::TheBigDump);
		partFractions::ComputedContributionsList.flush();
	}

//	Accum.ComputeDebugString();
}

void partFraction::ComputePolyCorrespondingToOneMonomial
			(	PolynomialLargeRational &output, int index, roots& normals,
				partFractionPolynomials* SplitPowerSeriesCoefficient)
{ static root shiftRational; shiftRational.dimension=root::AmbientDimension;
	static PolynomialLargeRational tempP;
	for (int j=0;j<root::AmbientDimension;j++)
	{ shiftRational.coordinates[j].AssignInteger
			(this->Coefficient.TheObjects[index].degrees[j]);
	}
	if (partFraction::AnErrorHasOccurredTimeToPanic)
	{	shiftRational.ComputeDebugString();
		this->Coefficient.ComputeDebugString();
		normals.ComputeDebugString();
	}
	LargeRational tempRat;
	tempRat.AssignInteger(this->Coefficient.TheObjects[index].Coefficient.value);
	output.MakeNVarConst(root::AmbientDimension, tempRat);
	for (int i=0;i<root::AmbientDimension;i++)
	{ static root beta; beta.dimension=root::AmbientDimension;
		static Rational tempRat,tempRat2;
		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{ normals.ComputeDebugString();
		}
		this->MakePolynomialFromOneNormal
			(	normals.TheObjects[i],shiftRational,
				this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]].Multiplicities.TheObjects[0],
				tempP);
		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{ normals.ComputeDebugString();
		}
		output.MultiplyBy(tempP);
		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{	tempP.ComputeDebugString();
			output.ComputeDebugString();
		}
	}
	//	output.ComputeDebugString();
	SplitPowerSeriesCoefficient->AddPolynomialLargeRational(shiftRational,output);
	this->AlreadyAccountedForInGUIDisplay=true;
}

int partFraction::Elongate(int indexElongatedFraction, int theElongation)
{ if (this->TheObjects[indexElongatedFraction].Multiplicities.TheObjects[0]==0) return indexElongatedFraction;
	intRoot tempIntRoot;
	tempIntRoot=partFraction::RootsToIndices.TheObjects[indexElongatedFraction];
	tempIntRoot.MultiplyByInteger(theElongation);
	int IndexLongVector=partFraction::RootsToIndices.ContainsObjectHash(tempIntRoot);
	if (IndexLongVector==-1)
	{	IndexLongVector=partFraction::RootsToIndices.size;
		partFraction::RootsToIndices.AddRootPreserveOrder(tempIntRoot);
	}
	if (IndexLongVector>=this->size )
	{	int oldsize=this->size;
		this->SetSizeExpandOnTopNoObjectInit(partFraction::RootsToIndices.size);
		for(int i= oldsize;i<this->size;i++)
			this->TheObjects[i].Multiplicities.TheObjects[0]=0;
	}
	assert(theElongation>0);
	IntegerPoly tempP;
	this->GetNElongationPoly(indexElongatedFraction,1,theElongation,tempP);
	tempP.RaiseToPower(this->TheObjects[indexElongatedFraction].Multiplicities.TheObjects[0]);
	this->TheObjects[IndexLongVector].Multiplicities.TheObjects[0]+=
		this->TheObjects[indexElongatedFraction].Multiplicities.TheObjects[0];
	this->TheObjects[indexElongatedFraction].Multiplicities.TheObjects[0]=0;
	this->Coefficient.MultiplyBy(tempP);
	this->ComputeIndicesNonZeroMults();
	return IndexLongVector;
}

void partFraction::MakePolynomialFromOneNormal
			(	root& normal, root& shiftRational, int theMult,	PolynomialLargeRational& output)
{	static Rational tempRat,tempRat2;
	static PolynomialLargeRational tempP;
	output.MakeNVarConst(root::AmbientDimension,LROne);
	if (theMult==1) return;
	root::RootScalarEuclideanRoot(normal,shiftRational,tempRat);
	for (int j=0;j<theMult-1;j++)
	{ tempP.MakeLinPolyFromRoot(normal);
		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{ tempP.ComputeDebugString();
		}
		tempRat2.init(-1,j+1);
		tempRat2.MultiplyBy(tempRat);
		tempRat2.Add(ROne);
		static Rational tempRat3;
		tempRat3.init(1,j+1);
		LargeRational tempLR1, tempLR2;
		tempLR1.AssignRational(tempRat2);
		tempLR2.AssignRational(tempRat3);
		tempP.TimesConstant(tempLR2);
		tempP.AddConstant(tempLR1);
		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{ tempP.ComputeDebugString(); }
		output.MultiplyBy(tempP);
		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{ output.ComputeDebugString(); }
	}
}

void partFraction::ComputeNormals(roots& output)
{	static roots dens;
	static root tempRoot; tempRoot.dimension=root::AmbientDimension;
	dens.size=0;
	output.size=0;
	for (int i=0;i<root::AmbientDimension;i++)
	{	tempRoot.AssignIntRoot
			(partFraction::RootsToIndices.TheObjects[this->IndicesNonZeroMults.TheObjects[i]]);
		dens.AddRoot(tempRoot);
	}
	for (int i=0;i<root::AmbientDimension;i++)
	{ static Rational tempRat, tempRat2;
		dens.ComputeNormalExcludingIndex(tempRoot, i);
		root::RootScalarEuclideanRoot(tempRoot, dens.TheObjects[i],tempRat);
		assert(!tempRat.IsEqualTo(RZero));
		tempRoot.DivByRational(tempRat);
		output.AddRoot(tempRoot);
//		tempRoot.ComputeDebugString();
	}
}

partFraction::partFraction()
{	this->init(partFraction::RootsToIndices.size);
	this->PowerSeriesCoefficientIsComputed=false;
	this->AlreadyAccountedForInGUIDisplay=false;
	this->FileStoragePosition=-1;
	this->LastGainingMultiplicityIndex=-1;
/*	if (partFraction::UseGlobalCollector)
	{ partFraction::GlobalCollectorPartFraction.AddObject(this);
		this->indexInGlobalCollectorPartFraction=
			partFraction::GlobalCollectorPartFraction.size-1;
		if (this->indexInGlobalCollectorPartFraction==102)
		{ Stop();
		}
	}*/
}

void partFraction::init(int numRoots)
{// delete [] this->IndicesNonZeroMults;
	//this->IndicesNonZeroMults=0;
	this->IndicesNonZeroMults.SetActualSizeAtLeastExpandOnTop(numRoots);
	this->IndicesNonZeroMults.size=0;
	this->SetSizeExpandOnTopNoObjectInit(partFraction::RootsToIndices.size);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Elongations.size=0;
		this->TheObjects[i].Multiplicities.size=0;
	}
}

partFraction::~partFraction()
{ /*if (this->indexInGlobalCollectorPartFraction==102)
	{ Stop();
	}*/
/*partFraction::GlobalCollectorPartFraction.
		TheObjects[this->indexInGlobalCollectorPartFraction]=0;*/
}
/*void partFraction::findInitialPivot()
{ if (this->currentPivot!=-1) return;
	this->currentPivot=0;
	this->findPivot();
}*/

int partFraction::HashFunction()
{ int result=0;
	for (int i=0;i<this->size;i++)
		result+=SomeRandomPrimes[i]*this->TheObjects[i].HashFunction();
	return result;
}

bool partFraction::operator==(partFraction& right)
{	if (this->size!= right.size) return false;
	for (int i=0;i<this->size;i++)
	{ if (! (this->TheObjects[i]==right.TheObjects[i]))
			return false;
	}
/*	for(int i=0;i<root::AmbientDimension;i++)
	{ if (this->RootShift[i]!=right.RootShift[i])
			return false;
	}*/
	return true;
}

void partFraction::operator =(const partFraction &right)
{ this->Assign(right);
}

/*void partFraction::findPivot()
{ int theRow=-1;
	for (int i=this->currentPivot+1; i<this->size; i++)
	{	if ( this->TheObjects[i].den[this->currentPivot]>0 &&
			   ( this->TheObjects[this->currentPivot].den[this->currentPivot]==0
					 ||
					 ( this->TheObjects[this->currentPivot].den[this->currentPivot]!=0
						 &&
						 this->TheObjects[this->currentPivot].IsHigherThan(this->TheObjects[i])
					 )
				 )
			 )
		{ this->swap(i,this->currentPivot);
		}
	}
	this->EliminationStart= this->currentPivot+1;
}*/

int partFractions::SizeWithoutDebugString()
{ int Accum=0;
	Accum+=	this->HashedListBasicObjects<partFraction>::SizeWithoutObjects();
	for (int i=0;i<this->ActualSize;i++)
	{ Accum+=this->TheActualObjects[i].SizeWithoutDebugString();
	}
	Accum+=	sizeof(this->HighestIndex)+
					sizeof(this->IndexLowestNonReduced);
	return Accum;
}

void partFractions::AssureIndicatorRegularity()
{	if (this->UsingIndicatorRoot)
	{	roots tempRoots;
		tempRoots.AssignHashedIntRoots(partFraction::RootsToIndices);
		tempRoots.PerturbVectorToRegular(this->IndicatorRoot);
	}
}

bool partFractions::ShouldIgnore()
{ bool shouldIgnore=false;
	if (this->UsingIndicatorRoot)
	{	shouldIgnore=!this->TheObjects[this->IndexLowestNonReduced].rootIsInFractionCone(this->IndicatorRoot);
	}
	if (shouldIgnore)
	{ if (this->DiscardingFractions)
		{	this->PopIndexSwapWithLastHash(this->IndexLowestNonReduced);}
		else
		{	this->IndexLowestNonReduced++;}
	}
	return shouldIgnore;
}

void partFractions::PrepareCheckSums()
{//	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.InitFromIntegers(1,1,1,1,1,1,1,1,1,1,1,1);
//	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.DivByInteger(4);
//	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.MultiplyByInteger(3);
	if (!this->UsingCheckSum)
		return;
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[0].init(2,3);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[1].init(2,3);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[2].init(2,3);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[3].init(1,5);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[4].init(11,13);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[5].init(13,17);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[6].init(17,19);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[7].init(19,23);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[8].init(23,29);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[9].init(31,37);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[10].init(37,41);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[11].init(41,43);
	this->ComputeOneCheckSum(this->StartCheckSum);
}

void partFractions::CompareCheckSums()
{	if (!this->UsingCheckSum)
		return;
	if (!this->DiscardingFractions)
	{	this->ComputeOneCheckSum(this->EndCheckSum);
	//partFraction::MakingConsistencyCheck=true;
	/*if (partFraction::MakingConsistencyCheck)
	{	this->ComputeDebugString();
		tempRat2.ElementToString(tempS2);
		tempRat.ElementToString(tempS1);
	}*/
		assert(this->StartCheckSum.IsEqualTo(this->EndCheckSum));
	}

}

bool partFractions::split()
{ this->IndexLowestNonReduced=0;
	partFraction tempF;
	//Checksum code follows:
//	std::string tempS1, tempS2;
//	this->ComputeDebugString();
	this->PrepareCheckSums();
	this->AssureIndicatorRegularity();
/*	if (partFraction::MakingConsistencyCheck)
	{	oneFracWithMultiplicitiesAndElongations::CheckSumRoot.ComputeDebugString();
		tempRat.ElementToString(tempS1);
		this->ComputeDebugString();
	}*/
//	partFraction::AnErrorHasOccurredTimeToPanic=true;
	while (this->IndexLowestNonReduced<this->size)
	{ //this->ComputeDebugString();
//		bool ShouldIgnore=false;
		if (!this->ShouldIgnore())
		{	tempF.Assign(this->TheObjects[this->IndexLowestNonReduced]);
			//this->ComputeDebugString();
			//tempF.ComputeDebugString();
			//if (this->IndexLowestNonReduced==22 && this->size==70)
			//{ partFraction::AnErrorHasOccurredTimeToPanic=true;
			//}
			if (! (tempF.reduceOnceGeneralMethod(*this)))
			{ if (tempF.IndicesNonZeroMults.size<=root::AmbientDimension)
					this->IndexLowestNonReduced++;
			}
			else
			{	this->PopIndexSwapWithLastHash( this->IndexLowestNonReduced);
				if (partFraction::AnErrorHasOccurredTimeToPanic)
				{ this->ComputeDebugString();
				}
			}
	/*		if (partFraction::MakingConsistencyCheck)
			{	LargeRational tempRat2;
				this->ComputeOneCheckSum(tempRat2);
				if (!tempRat2.IsEqualTo(tempRat))
				{	this->ComputeDebugString();
					tempRat2.ElementToString(tempS2);
					tempRat.ElementToString(tempS1);
					assert(false);
				}
			}*/
		}
		this->MakeProgressReport();
	//	this->ComputeDebugString();
//		x= this->SizeWithoutDebugString();
	}
//	this->RemoveRedundantShortRootsClassicalRootSystem();
//	PolyFormatLocal.MakeAlphabetxi();
//	this->ComputeDebugString();
	LargeRational tempRat2;
	/*if (partFraction::MakingConsistencyCheck)
	{	this->ComputeDebugString();
		this->ComputeOneCheckSum(tempRat2);
		tempRat2.ElementToString(tempS2);
		tempRat.ElementToString(tempS1);
		assert(tempRat2.IsEqualTo(tempRat));
	}	*/
	this->RemoveRedundantShortRoots();
	this->CompareCheckSums();
	this->IndexLowestNonReduced= this->size;
	this->MakeProgressReport();
	return false;
}

bool partFractions::splitClassicalRootSystem()
{ this->IndexLowestNonReduced=0;
	if (this->SplitTestMode)
	{ ListBasicObjects<Monomial<Integer> >::ListBasicObjectsActualSizeIncrement=1;
		HashedListBasicObjects<Monomial<Integer> >::PreferredHashSize=1;
	}
	this->PrepareCheckSums();
	this->AssureIndicatorRegularity();
	partFraction tempF;
	while (this->IndexLowestNonReduced<this->size)
	{ //this->ComputeDebugString();
		if (!this->ShouldIgnore())
		{	tempF.Assign(this->TheObjects[this->IndexLowestNonReduced]);
			//this->ComputeDebugString();
			//tempF.ComputeDebugString();
			if (! (tempF.reduceOnceTotalOrderMethod(*this)))
			{ this->IndexLowestNonReduced++;
			}
			else
			{	this->PopIndexSwapWithLastHash( this->IndexLowestNonReduced);
			}
			this->MakeProgressReport();
		}
//		this->ComputeDebugString();
//		x= this->SizeWithoutDebugString();
	}
	//this->ComputeDebugString();
	this->RemoveRedundantShortRootsClassicalRootSystem();
//	this->ComputeDebugString();
	this->CompareCheckSums();
	this->IndexLowestNonReduced= this->size;
	this->MakeProgressReport();
	return this->CheckForMinimalityDecompositionWithRespectToRoot(this->IndicatorRoot);
}

bool partFractions::CheckForMinimalityDecompositionWithRespectToRoot(root & r)
{	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].IndicesNonZeroMults.size>root::AmbientDimension)
		{	if (this->TheObjects[i].rootIsInFractionCone(r))
				return false;
		}
	}
	return true;
}

void partFraction::initFromRootSystem(intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights)
{	this->Coefficient.MakeNVarConst(root::AmbientDimension,IOne);
	partFraction::RootsToIndices.initFromRoots(theAlgorithmBasis,weights);
	this->init(partFraction::RootsToIndices.size);
	for(int i=0;i<partFraction::RootsToIndices.size;i++)
	{ this->TheObjects[i].init();
	}
	for (int i=0;i<theFraction.size;i++)
	{ int index = partFraction::RootsToIndices.getIndex(theFraction.TheObjects[i]);
		this->TheObjects[index].AddMultiplicity(1,1);
	}
	this->ComputeIndicesNonZeroMults();
}

int partFractions::ReadFromFileComputedContributions(std::fstream& input)
{ std::string tempS;
	input.seekg(0);
	input>>tempS;
	int lastNonZero=-1;
	for (int i=0;i<this->size;i++)
	{ int x;
		input>>x>>this->TheObjects[i].FileStoragePosition;
		if (this->TheObjects[i].FileStoragePosition>lastNonZero)
		{ lastNonZero=this->TheObjects[i].FileStoragePosition;
		}
		assert(x==i);
	}
	return lastNonZero;
}

void partFractions::WriteToFileComputedContributions(std::fstream& output)
{ output.seekp(0);
	output<< "Partial_fraction_index/file_storage_position\n";
	for (int i=0;i<this->size;i++)
	{ output<< i<<" "<<this->TheObjects[i].FileStoragePosition<<"\n";
	}
}


int partFractions::NumMonomialsInTheNumerators()
{ int result=0;
	for (int i=0;i<this->size;i++)
		result+=this->TheObjects[i].Coefficient.size;
	return result;
}

partFractions::partFractions()
{ this->HighestIndex=-1;
	this->IndexLowestNonReduced=-2;
	this->SplitTestMode=false;
	this->UsingIndicatorRoot=true;
	this->DiscardingFractions=false;
	this->IndicatorRoot.MakeZero();
}

void partFraction::ComputeDebugString()
{ this->ElementToString
		(this->DebugString, PolynomialOutputFormat::UsingLatexFormat,false,true);
}

void partFraction::InsertNewRootIndex(int index)
{ this->ShiftUpExpandOnTop(index);
	this->TheObjects[index].Multiplicities.TheObjects[0]=0;
}

void partFractions::Add(partFraction &f)
{ int tempI=this->ContainsObjectHash(f);
	if (tempI==-1)
	{ this->AddObjectOnTopHash(f);
		tempI= this->size-1;
	}
	else
	{ this->TheObjects[tempI].Coefficient.AddPolynomial(f.Coefficient);
	}
}

int partFractions::ElementToString(std::string& output, bool LatexFormat,
																		bool includeVPsummand, bool includeNumerator)
{ static MatrixInt tempMat;
	return this->ElementToStringBasisChange
		(tempMat,false,output,LatexFormat,includeVPsummand,includeNumerator);
}

int partFractions::ElementToStringOutputToFile(std::fstream& output, bool LatexFormat,
																		bool includeVPsummand, bool includeNumerator)
{ static MatrixInt tempMat;
	return this->ElementToStringBasisChangeOutputToFile
		(tempMat,false,output,LatexFormat,includeVPsummand,includeNumerator);
}

int partFractions::ElementToStringBasisChange(MatrixInt& VarChange,
																	bool UsingVarChange, std::string& output,
																	bool LatexFormat,bool includeVPsummand, bool includeNumerator)
{ std::stringstream out;
	std::string tempS;
	int TotalLines=0;
	PolyFormatLocal.ExtraLinesCounterLatex=0;
	if (LatexFormat)
	{ out << "\\begin{eqnarray*}\n";}
	int LastCutOff=0;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.size>0 )
		{ TotalLines+=this->TheObjects[i].ElementToStringBasisChange(VarChange, UsingVarChange,
																									 tempS, LatexFormat,includeVPsummand,includeNumerator);
			if (LatexFormat)
			{ out <<"&&"; }
			if (tempS[0]!='-'){out<<"+";}
			out<<tempS;
			if (LatexFormat)
			{ out<<"\\\\ \n";
				TotalLines++;
			}
			else
			{out <<"\n";}
			if	( LatexFormat &&
						(TotalLines-LastCutOff)> PolynomialOutputFormat::LatexCutOffLine
					)
			{ out<<"\\end{eqnarray*}\\begin{eqnarray*}\n";
				LastCutOff=TotalLines;
			}
		}
	}
	if (!LatexFormat)
	{	output= out.str();
		if (output.size()>0)
		{ if (output[0]=='+')	output.erase(0,1);
		}
	}
	else
	{ out << "\\end{eqnarray*}";
		output= out.str();
	}
	return TotalLines;
}

int partFractions::ElementToStringBasisChangeOutputToFile(MatrixInt& VarChange,
																	bool UsingVarChange, std::fstream& output,
																	bool LatexFormat,bool includeVPsummand, bool includeNumerator)
{ std::string tempS;
	int TotalLines=0;
	PolyFormatLocal.ExtraLinesCounterLatex=0;
	if (LatexFormat)
	{ output << "\\begin{eqnarray*}\n";}
	int LastCutOff=0;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.size>0 )
		{ TotalLines+=this->TheObjects[i].ElementToStringBasisChange(VarChange, UsingVarChange,
																									 tempS, LatexFormat,includeVPsummand,includeNumerator);
			if (LatexFormat)
			{ output <<"&&"; }
			if (tempS[0]!='-'){output<<"+";}
			output<<tempS;
			if (LatexFormat)
			{ output<<"\\\\ \n";
				TotalLines++;
			}
			else
			{output <<"\n";}
			if	( LatexFormat &&
						(TotalLines-LastCutOff)> PolynomialOutputFormat::LatexCutOffLine
					)
			{ output<<"\\end{eqnarray*}\\begin{eqnarray*}\n";
				LastCutOff=TotalLines;
			}
		}
	}
	if (LatexFormat)
	{ output << "\\end{eqnarray*}";
	}
	return TotalLines;
}

#ifdef WIN32
#pragma warning(disable:4018)//grrrrr
#endif
int partFraction::ControlLineSizeFracs(std::string& output)
{ int numCutOffs= output.size()% PolyFormatLocal.LatexMaxLineLength;
	int LastCutOffIndex=0;
	int NumLinesAdded=0;
	for (int i=0;i<numCutOffs;i++)
	{ for ( int j=LastCutOffIndex+PolyFormatLocal.LatexMaxLineLength;
					j< ((int) output.size())-1;j++)
		{ if (output[j]=='\\'&&output[j+1]=='f')
			{ output.insert(j,"\\\\\n&&");
				NumLinesAdded++;
				LastCutOffIndex=j+5;
				break;
			}
		}
	}
	return NumLinesAdded;
}

int partFraction::ControlLineSizeStringPolys(std::string& output)
{ int numCutOffs= output.size()% PolyFormatLocal.LatexMaxLineLength;
	int LastCutOffIndex=0;
	int NumLinesAdded=0;
	for (int i=0;i<numCutOffs;i++)
	{ for ( int j=LastCutOffIndex+PolyFormatLocal.LatexMaxLineLength;
					j<(int)(output.size())-1;j++)
		{ if ((output[j]=='+'||output[j]=='-')&&output[j-1]!='{')
			{ output.insert(j,"\\\\\n&&");
				NumLinesAdded++;
				LastCutOffIndex=j+5;
				break;
			}
		}
	}
	return NumLinesAdded;
}
#ifdef WIN32
#pragma warning(default:4018)//grrrrr
#endif
void partFractions::MakeProgressReport()
{ IndicatorWindowGlobalVariables.NumPartFractionsLoaded= this->size;
	IndicatorWindowGlobalVariables.NumReducedPartFractions= this->IndexLowestNonReduced;
	IndicatorWindowGlobalVariables.PrepareStrings();
	FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
	//while(IndicatorWindowGlobalVariables.Pause)
	//{
	//}
}

void partFractions::ComputeOneCheckSum(LargeRational &output)
{ output.MakeZero();
	for(int i=0;i<this->size;i++)
	{ LargeRational tempRat;
		this->TheObjects[i].ComputeOneCheckSum(tempRat);
		output.Add(tempRat);
		if (this->MakingProgressReport)
		{ IndicatorWindowGlobalVariables.StatusString.clear();
			std::stringstream out;
			out <<"Checksum "<<i+1<<" out of "<<this->size;
			IndicatorWindowGlobalVariables.StatusString= out.str();
			this->MakeProgressReport();
		}
	}
}

void partFractions::initFromRootSystem(intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights)
{ this->ClearTheObjects();
	partFraction::RootsToIndices.ClearTheObjects();
	partFraction f;
	f.initFromRootSystem(theFraction,theAlgorithmBasis, weights);
	this->Add(f);
}

void partFractions::RemoveRedundantShortRoots()
{	partFraction tempFrac;
	LargeRational startCheckSum,tempCheckSum, tempCheckSum2,tempCheckSum3;
	//partFraction::MakingConsistencyCheck=true;
	if (partFraction::MakingConsistencyCheck)
	{	this->ComputeOneCheckSum(startCheckSum);
	}
	//partFraction::AnErrorHasOccurredTimeToPanic=true;
//	partFraction::MakingConsistencyCheck=false;
	for (int i=0;i<this->size;i++)
	{ //if (i==12)
//			partFraction::MakingConsistencyCheck=true;
		if (partFraction::MakingConsistencyCheck)
		{ /*if (i==12)
			{ partFraction::AnErrorHasOccurredTimeToPanic=true;
				this->TheObjects[i].ComputeDebugString();
				this->ComputeOneCheckSum(tempCheckSum);
				assert(tempCheckSum.IsEqualTo(startCheckSum));
			}*/
		}
		tempFrac.Assign(this->TheObjects[i]);
		root* tempPointerRoot=0;
		if (this->UsingIndicatorRoot){tempPointerRoot=&this->IndicatorRoot;}
		if(tempFrac.RemoveRedundantShortRoots(tempPointerRoot))
		{/*	if (partFraction::AnErrorHasOccurredTimeToPanic)
			{ this->TheObjects[i].ComputeOneCheckSum(tempCheckSum3);
				tempFrac.ComputeOneCheckSum(tempCheckSum2);
				std::string tempS1, tempS2;
				tempCheckSum3.ElementToString(tempS1);
				tempCheckSum2.ElementToString(tempS2);
				assert(tempCheckSum3.IsEqualTo(tempCheckSum2));
				tempFrac.ComputeDebugString();
				this->TheObjects[i].ComputeDebugString();
				tempCheckSum.Subtract(tempCheckSum2);
//				tempCheckSum.Add(tempCheckSum2);
		//		assert(tempCheckSum.IsEqualTo(startCheckSum));
			}*/
			this->TheObjects[i].Coefficient.Nullify(root::AmbientDimension);
			this->TheObjects[i].Coefficient.ReleaseMemory();
	/*		if (partFraction::AnErrorHasOccurredTimeToPanic)
			{ this->ComputeOneCheckSum(tempCheckSum3);
				assert(tempCheckSum3.IsEqualTo(tempCheckSum));
				//IntegerPoly::AnErrorHasOccurredTimeToPanic=true;
			}*/
			int oldsize= this->size;
			this->Add(tempFrac);
			if (oldsize>this->size)
			{ Stop();
			}
			if (this->MakingProgressReport)
			{ std::stringstream out;
				out<<"Elongating denominator "<<i+1<<" out of "<<this->size;
				IndicatorWindowGlobalVariables.ProgressReportString3=out.str();
				::FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
			}
/*			if (partFraction::AnErrorHasOccurredTimeToPanic)
			{ this->ComputeOneCheckSum(tempCheckSum);
				std::string tempS1, tempS2;
				tempCheckSum.ElementToString(tempS1);
				startCheckSum.ElementToString(tempS2);
				assert(startCheckSum.IsEqualTo(tempCheckSum));
				//IntegerPoly::AnErrorHasOccurredTimeToPanic=true;
 			}*/
 		}
/*		if (partFraction::AnErrorHasOccurredTimeToPanic)
		{	this->ComputeOneCheckSum(tempCheckSum);
			std::string tempS1,tempS2;
			tempCheckSum.ElementToString(tempS1);
			startCheckSum.ElementToString(tempS2);
			assert(startCheckSum.IsEqualTo(tempCheckSum));
		}*/
	}
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.IsEqualToZero())
		{ this->PopIndexSwapWithLastHash(i);
			i--;
		}
	}
/*	if (partFraction::MakingConsistencyCheck)
	{	this->ComputeOneCheckSum(tempCheckSum);
		assert(startCheckSum.IsEqualTo(tempCheckSum));
	}*/
}

void partFractions::RemoveRedundantShortRootsClassicalRootSystem()
{	partFraction tempFrac;
	for (int i=0;i<this->size;i++)
	{ tempFrac.Assign(this->TheObjects[i]);
		root* tempRoot=0;
		if (this->UsingIndicatorRoot)
		{ tempRoot=&this->IndicatorRoot;
		}
		if(tempFrac.RemoveRedundantShortRootsClassicalRootSystem(tempRoot))
		{	this->TheObjects[i].Coefficient.Nullify(root::AmbientDimension);
			this->TheObjects[i].Coefficient.ReleaseMemory();
			this->Add(tempFrac);
		}
		if (this->MakingProgressReport)
		{ std::stringstream out;
			out<<"Elongating denominator "<<i+1<<" out of "<<this->size;
			IndicatorWindowGlobalVariables.StatusString=out.str();
			::FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
		}
	}
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.IsEqualToZero())
		{ this->PopIndexSwapWithLastHash(i);
			i--;
		}
	}
}

void FileSetPutPointerToEnd(std::fstream& theFile, bool StoreToFile)
{ //theFile.close();
	//theFile.open(path);
	assert(theFile.is_open()||!StoreToFile);
	std::filebuf* pbuf = theFile.rdbuf();
	int tempSize = pbuf->pubseekoff(0,std::fstream::end);
	theFile.seekp(tempSize);
}

bool partFractions::VerifyFileComputedContributions()
{	int tempI= this->ReadFromFileComputedContributions(partFractions::ComputedContributionsList);
	std::filebuf* pbuf = partFraction::TheBigDump.rdbuf();
	int tempSize = pbuf->pubseekoff(0,std::fstream::end);
	partFraction::TheBigDump.seekp(tempSize);
	return(tempSize>=tempI);
}

bool partFractions::partFractionsToPartitionFunctionAdaptedToRoot
				(	QuasiPolynomial& output, root& r, bool storeComputations,
					bool RecordSplitPowerSeriesCoefficient, bool StoreToFile,
					bool UseOldData)
{	if (!this->CheckForMinimalityDecompositionWithRespectToRoot(r)){return false;}
	LargeRational oldCheckSum;
	QuasiPolynomial oldOutput;
	if (partFraction::MakingConsistencyCheck)
	{ partFractions::CheckSum.MakeZero();
	}
	if (StoreToFile&& UseOldData)
	{ bool tempBool = this->VerifyFileComputedContributions();
		assert(tempBool);
	}
	IndicatorWindowGlobalVariables.rootIsModified=false;
	IndicatorWindowGlobalVariables.PerturbationHasOccurred=false;
	output.Nullify(root::AmbientDimension);
	static QuasiPolynomial tempQP;
	for (int i=0;i<this->size;i++)
	{ assert(partFraction::TheBigDump.is_open()||!StoreToFile);
		assert(partFractions::ComputedContributionsList.is_open()||!StoreToFile);
		if (this->TheObjects[i].rootIsInFractionCone(r))
		{ if (storeComputations)
			{this->TheObjects[i].partFractionToPartitionFunctionStoreAnswer
				(tempQP,RecordSplitPowerSeriesCoefficient,StoreToFile); }
			else
			{ this->TheObjects[i].partFractionToPartitionFunctionSplit
					(tempQP,true,RecordSplitPowerSeriesCoefficient,StoreToFile);
			}
			if (StoreToFile)
			{ this->WriteToFileComputedContributions(partFractions::ComputedContributionsList);
				FileSetPutPointerToEnd
					(partFractions::ComputedContributionsList, StoreToFile);
				FileSetPutPointerToEnd
					(partFraction::TheBigDump, StoreToFile);
			}
/*			if(partFraction::MakingConsistencyCheck)
			{	LargeRational tempLRat2, tempLRat3, tempLRat4;
				std::string tempS1, tempS2, tempS3, tempS4;
				tempQP.Evaluate(partFraction::theVectorToBePartitioned,tempLRat2);
				output.Evaluate(partFraction::theVectorToBePartitioned,tempLRat3);
				tempLRat2.ElementToString(tempS1);
				tempLRat3.ElementToString(tempS2);
				tempLRat4.Assign(tempLRat2);
				tempLRat4.Add(tempLRat3);
				partFraction::CheckSum.ElementToString(tempS3);
				partFractions::CheckSum.ElementToString(tempS4);
				assert(tempLRat2.den.IsEqualTo(LIOne));
				assert(tempLRat3.den.IsEqualTo(LIOne));
				assert(tempLRat2.IsEqualTo(partFraction::CheckSum));
				assert(tempLRat4.IsEqualTo(partFractions::CheckSum));
				if (i==4)
				{ Stop();
					QuasiPolynomial::AnErrorHasOccurredTimeToPanic=true;
					::RandomCodeIDontWantToDelete theEvilBug;
					theEvilBug.EvilPoly1.Assign(output);
					theEvilBug.EvilPoly2.Assign(tempQP);
					theEvilBug.RevealTheEvilConspiracy();
					RandomCodeIDontWantToDelete::UsingEvilList1=false;
				}
			}*/
			output.AddPolynomial(tempQP);
	/*		if (partFraction::MakingConsistencyCheck)
			{	LargeRational tempLRat;
				output.Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
				if ((! tempLRat.IsEqualTo(partFractions::CheckSum)) || (! tempLRat.den.IsEqualTo(LIOne)))
				{ std::string tempS,tempS2,tempS3,tempS4;
					LargeRational tempLRat2;
					tempQP.Evaluate(partFraction::theVectorToBePartitioned,tempLRat2);
					partFractions::CheckSum.ElementToString(tempS);
					tempLRat.ElementToString(tempS2);
					oldCheckSum.ElementToString(tempS3);
					tempLRat2.ElementToString(tempS4);
					partFraction::AnErrorHasOccurredTimeToPanic=true;
	//				assert(false);
				}
				oldCheckSum.Assign(partFractions::CheckSum);
				oldOutput.Assign(output);
			}*/
			::IndicatorWindowGlobalVariables.NumComputedContributions++;
		}
		::IndicatorWindowGlobalVariables.NumProcessedFractions=i+1;
		this->MakeProgressReport();
	}
/*	if (partFraction::MakingConsistencyCheck)
	{ std::string tempS;
		partFractions::CheckSum.ElementToString(tempS);
		Stop();
	}*/
	if (StoreToFile)
	{ partFraction::TheBigDump.flush();

	}
	return true;
}

void partFractions::ComputeDebugString()
{ this->ElementToString(this->DebugString,PolynomialOutputFormat::UsingLatexFormat,false,true);
}

void partFractions::ComputeDebugStringNoNumerator()
{ this->ElementToString(this->DebugString,PolynomialOutputFormat::UsingLatexFormat,false,false);
}

void partFractions::ComputeDebugStringWithVPfunction()
{ this->ElementToString(this->DebugString,PolynomialOutputFormat::UsingLatexFormat,true,true);
}

void partFractions::WriteToFile(std::fstream& output)
{ std::string tempS;
	output<<"Dimension:\n";
	output<<root::AmbientDimension<<"\n";
	output<<"Indices_of_roots:\n";
	for (int i=0;i<partFraction::RootsToIndices.size;i++)
	{ partFraction::RootsToIndices.TheObjects[i].ElementToString(tempS);
		output<<"| "<<i<<"    "<<tempS <<"\n";
	}
	output<<"Alphabet_used:\n";
	for (int i=0;i<root::AmbientDimension;i++)
	{ output<<PolyFormatLocal.alphabet[i]<<" ";
	}
	output<<"\n"<<"Number_of_fractions: " <<this->size<<"\n";
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].WriteToFile(output);
	}
}

void partFractions::ReadFromFile(std::fstream& input)
{ intRoots tempRoots;
	std::string tempS;
	intRoot tempRoot;
	input>>tempS;
	input>>root::AmbientDimension;
	input>>tempS;
	for (input>>tempS; tempS!="Alphabet_used:";input>>tempS)
	{ int tempI;
		input>>tempI;
		input>>tempS;
		for (int i=0;i<root::AmbientDimension;i++)
		{	input>>tempRoot.elements[i];
			input>>tempS;
		}
		tempRoots.AddObjectOnTop(tempRoot);
	}
	tempRoots.ComputeDebugString();
	partFraction::RootsToIndices.initFromRoots(tempRoots,0);
	for (int i=0;i<root::AmbientDimension;i++)
	{ input>>tempS;
		PolyFormatLocal.alphabet[i]=tempS;
	}
	int tempI;
	input>>tempS;
	assert(tempS=="Number_of_fractions:");
	input >>tempI;
	partFraction tempFrac;
	this->SetActualSizeAtLeastExpandOnTop(tempI);
	for(int i=0;i<tempI;i++)
	{ tempFrac.ReadFromFile(input);
		this->Add(tempFrac);
		this->MakeProgressReport();
	}
}

void partFractions::ComputeDebugStringBasisChange(MatrixInt& VarChange)
{ this->ElementToStringBasisChange( VarChange,true,
																		this->DebugString,
																		PolynomialOutputFormat::UsingLatexFormat,false,true);
}

void partFractions::ComputeKostantFunctionFromWeylGroup
				(	char WeylGroupLetter, unsigned char WeylGroupNumber,
					QuasiPolynomial& output, root* ChamberIndicator, bool UseOldData,
					bool StoreToFile)
{	intRoots theBorel,theVPbasis;
	intRoot tempWeight;
	roots tempRoots;
	WeylGroup tempW;
	this->ClearTheObjects();
	if (WeylGroupLetter=='A')
	{ tempW.MakeAn(WeylGroupNumber);
	}
	if (WeylGroupLetter=='B')
	{ tempW.MakeBn(WeylGroupNumber);
	}
	if (WeylGroupLetter=='C')
	{ tempW.MakeCn(WeylGroupNumber);
	}
	if (WeylGroupLetter=='D')
	{ tempW.MakeDn(WeylGroupNumber);
	}
	if (WeylGroupLetter=='G')
	{ tempW.MakeG2();
	}
	if (WeylGroupLetter=='F')
	{ tempW.MakeF4();
		partFractions::ListBasicObjectsActualSizeIncrement=10000;
	}
	if (WeylGroupLetter=='A'||
			WeylGroupLetter=='B'||
			WeylGroupLetter=='C'||
			WeylGroupLetter=='D')
	{	for (int i=0;i<root::AmbientDimension;i++)
		{ tempWeight.elements[i]=KToTheNth(8,root::AmbientDimension-i-1);
		}
	}
	else
	{	for (int i=0;i<root::AmbientDimension;i++)
		{ tempWeight.elements[i]=1;//KToTheNth(8,root::AmbientDimension-i-1);
		}
	}
	//tempW.ComputeWeylGroup();
	tempW.ComputeRootsOfBorel(tempRoots);
	theBorel.AssignRoots(tempRoots);
	theVPbasis.CopyFromBase(theBorel);
	if (WeylGroupLetter=='B')
	{ for (int i=0;i<theVPbasis.size;i++)
		{ Rational tempRat;
			root tempRoot; tempRoot.AssignIntRoot(theVPbasis.TheObjects[i]);
			tempW.RootScalarKillingFormMatrixRoot(tempRoot,tempRoot,tempRat);
			if (tempRat.IsEqualTo(ROne))
			{ intRoot tempIntRoot;
				tempIntRoot=theBorel.TheObjects[i];
				tempIntRoot.MultiplyByInteger(2);
				theVPbasis.AddObjectOnTop(tempIntRoot);
			}
		}
	}
	if (WeylGroupLetter=='D')
	{	intRoot tempRoot;
		tempRoot.MakeZero();
		tempRoot.elements[root::AmbientDimension-1]=1;
		tempRoot.elements[root::AmbientDimension-2]=-1;
		theVPbasis.AddObjectOnTop(tempRoot);
		tempRoot.elements[root::AmbientDimension-1]=1;
		tempRoot.elements[root::AmbientDimension-2]=1;
		theVPbasis.AddObjectOnTop(tempRoot);
		for(int i=root::AmbientDimension-3;i>=0;i--)
		{ tempRoot.elements[i]=2;
			theVPbasis.AddObjectOnBottom(tempRoot);
		}
		tempWeight.elements[root::AmbientDimension-2]=7;
		tempWeight.elements[root::AmbientDimension-1]=8;
	}
	theVPbasis.BubbleSort(&tempWeight);
	theVPbasis.ComputeDebugString();
	tempW.ComputeDebugString();
	theBorel.ComputeDebugString();
	this->initFromRootSystem(theBorel,theVPbasis,0);
	if (ChamberIndicator==0)
	{ this->IndicatorRoot.Assign(tempW.rho);
		this->UsingIndicatorRoot=false;
	}
	else
	{ this->IndicatorRoot.Assign(*ChamberIndicator);
		this->UsingIndicatorRoot=true;
	}
	//this->SplitTestMode=true;
	if (WeylGroupLetter=='A'||
			WeylGroupLetter=='B'||
			WeylGroupLetter=='C'||
			WeylGroupLetter=='D')
	{	this->splitClassicalRootSystem();
	}
	else
	{ this->split();
	}
//	this->ComputeDebugString();
	assert(this->CheckForMinimalityDecompositionWithRespectToRoot(this->IndicatorRoot));
	//return;

	::IndicatorWindowGlobalVariables.TotalNumMonomials= this->NumMonomialsInTheNumerators();
	if(!this->partFractionsToPartitionFunctionAdaptedToRoot
		(output,this->IndicatorRoot,false,false,StoreToFile,UseOldData))
	{ this->ComputeDebugStringNoNumerator();
		assert(false);
	}
	//output.ComputeDebugString();
}

void oneFracWithMultiplicitiesAndElongations::operator =(oneFracWithMultiplicitiesAndElongations& right)
{ this->Multiplicities.CopyFromBase(right.Multiplicities);
	this->Elongations.CopyFromBase(right.Elongations);
}

/*bool oneFracWithMultiplicitiesAndElongations::IsHigherThan(oneFracWithMultiplicitiesAndElongations& f)
{ for (int i=0;i<root::AmbientDimension;i++)
	{ if (this->den[i]<f.den[i]) return false;
		if (this->den[i]>f.den[i]) return true;
	}
	return false;
} */

/*int oneFracWithMultiplicitiesAndElongations::height()
{ int result=0;
	for (int i=0;i<root::AmbientDimension;i++)
	{ result+=this->den[i]*oneFracWithMultiplicitiesAndElongations::heights[i];
	}
	return result;
}*/

int oneFracWithMultiplicitiesAndElongations::HashFunction()
{ return this->GetTotalMultiplicity();
}

int oneFracWithMultiplicitiesAndElongations::GetLargestElongation()
{ int result=this->Elongations.TheObjects[0];
	for (int i=1;i<this->Elongations.size;i++)
	{	assert(this->Elongations.TheObjects[i]!=result);
		if (this->Elongations.TheObjects[i]>result)
			result= this->Elongations.TheObjects[i];
	}
	return result;
}

int oneFracWithMultiplicitiesAndElongations::GetLCMElongations()
{ int result =1;
	for (int i=0;i<this->Elongations.size;i++)
	{ assert(this->Elongations.TheObjects[i]!=0);
		result=lcm(this->Elongations.TheObjects[i],result);
	}
	return result;
}

int oneFracWithMultiplicitiesAndElongations::GetTotalMultiplicity()
{ int result=0;
	for (int i=0;i<this->Elongations.size;i++)
	{ result+= this->Multiplicities.TheObjects[i];
	}
	return result;
}

int oneFracWithMultiplicitiesAndElongations::IndexLargestElongation()
{ int result=0;
	for (int i=1;i<this->Elongations.size;i++)
	{ if (this->Elongations.TheObjects[i]>this->Elongations.TheObjects[result])
			result=i;
	}
	return result;
}

void oneFracWithMultiplicitiesAndElongations::init()
{ this->Elongations.size=0;
	this->Multiplicities.size=0;
}

void oneFracWithMultiplicitiesAndElongations::ComputeOneCheckSum(LargeRational& output, intRoot& theExp)
{ output.Assign(LROne);
	std::string tempS;
	for (int i=0;i<this->Elongations.size;i++)
	{ LargeRational tempRat, tempRat2, tempRat3;
		tempRat.Assign(LROne);
		tempRat2.Assign(LROne);
		for (int j=0;j<root::AmbientDimension;j++)
		{ tempRat3.AssignRational
				(oneFracWithMultiplicitiesAndElongations::CheckSumRoot.coordinates[j]);
			tempRat3.RaiseToPower(theExp.elements[j]*this->Elongations.TheObjects[i]);
			tempRat2.MultiplyBy(tempRat3);
		}
		//tempRat.ElementToString(tempS);
		tempRat.Subtract(tempRat2);
		tempRat.RaiseToPower(this->Multiplicities.TheObjects[i]);
		output.MultiplyBy(tempRat);
	}
	if (output.IsEqualToZero())
	{ std::string tempS;
		Stop();
	}
	output.Invert();

}

void oneFracWithMultiplicitiesAndElongations::AddMultiplicity(int MultiplicityIncrement, int Elongation)
{ int ElongationIndex=-1;
	for (int i=0;i<this->Elongations.size;i++)
	{ if (this->Elongations.TheObjects[i]==Elongation)
		{ ElongationIndex=i;
			break;
		}
	}
	if (ElongationIndex==-1)
	{ this->Elongations.AddObjectOnTop(Elongation);
		int tempI=0;
		this->Multiplicities.AddObjectOnTop(tempI);
		ElongationIndex= this->Multiplicities.size-1;
	}
	this->Multiplicities.TheObjects[ElongationIndex]+=MultiplicityIncrement;
	assert(this->Multiplicities.TheObjects[ElongationIndex]>=0);
	if (this->Multiplicities.TheObjects[ElongationIndex]==0)
	{ this->Multiplicities.PopIndexSwapWithLast(ElongationIndex);
		this->Elongations.PopIndexSwapWithLast(ElongationIndex);
	}
}

void oneFracWithMultiplicitiesAndElongations::OneFracToStringBasisChange
			(	int indexElongation, MatrixInt& VarChange, bool UsingVarChange, std::string& output,
				bool LatexFormat, int indexInFraction)
{ std::stringstream  out;
	std::string tempS;
	intRoot tempRoot2, tempRoot;
	int NumCoords;
	if (UsingVarChange)
	{	NumCoords= VarChange.NumRows;
		tempRoot2=partFraction::RootsToIndices.TheObjects[indexInFraction];
		for (int i=0;i<NumCoords;i++)
		{ tempRoot.elements[i]=0;
			for (int j=0;j<root::AmbientDimension;j++)
			{ tempRoot.elements[i]+=VarChange.elements[i][j]*tempRoot2.elements[j];
			}
		}
	}
	else
	{ NumCoords=root::AmbientDimension;
		tempRoot = partFraction::RootsToIndices.TheObjects[indexInFraction];
	}
	tempRoot.MultiplyByInteger(this->Elongations.TheObjects[indexElongation]);
	if (!LatexFormat)
	{	out << "1/(1-";}
	else
	{ out <<"\\frac{1}{(1-";}
	for(int i=0;i<NumCoords;i++)
	{ if (tempRoot.elements[i]!=0)
		{ out <<PolyFormatLocal.alphabet[i];
			if (tempRoot.elements[i]!=1)
			{ out<<"^{"<<tempRoot.elements[i]<<"}";
			}
		}
	}
	out <<")";
	if (this->Multiplicities.TheObjects[indexElongation]>1)
		out <<"^"	<<this->Multiplicities.TheObjects[indexElongation];
	if (LatexFormat){ out<<"}";}
	output= out.str();
}

void oneFracWithMultiplicitiesAndElongations::ElementToStringBasisChange
			( MatrixInt& VarChange,
				bool UsingVarChange, std::string& output,
				bool LatexFormat, int index)
{	if (this->Multiplicities.size==0)
	{	output.clear(); return;
	}
	std::string tempS;
	std::stringstream out;
	for (int k=0;k<this->Multiplicities.size;k++)
	{ this->OneFracToStringBasisChange(k,VarChange,UsingVarChange,tempS,LatexFormat,index);
		out<<tempS;
	}
	out<<" ";
	output= out.str();
}

bool oneFracWithMultiplicitiesAndElongations::operator ==
			(oneFracWithMultiplicitiesAndElongations &right)
{	if (this->Elongations.size!=right.Elongations.size)
		return false;
	for (int i=0;i<this->Elongations.size;i++)
	{ bool Found=false;
		for (int j=0; j<right.Elongations.size;j++)
		{ if (this->Elongations.TheObjects[i]==right.Elongations.TheObjects[j])
			{ if(this->Multiplicities.TheObjects[i]!=right.Multiplicities.TheObjects[j])
					return false;
				else
				{ Found=true; break;
				}
			}
		}
		if (!Found)
			return false;
	}
	return true;
}

void intRoot::MakeZero()
{ this->dimension=root::AmbientDimension;
	for (int i=0;i<this->dimension;i++)
	{ this->elements[i]=0;
	}
}

void intRoot::AddRoot(intRoot& theRoot)
{ for (int i=0;i<this->dimension;i++)
	{this->elements[i]+=theRoot.elements[i];}
}

int intRoot::HashFunction()
{	int result=0;
	for (int i=0;i<this->dimension;i++)
	{ result+=::SomeRandomPrimes[i]*this->elements[i];
	}
	return result;
}

void intRoot::ElementToString(std::string& output)
{ std::stringstream out;
	out<<"( ";
	for (int i=0;i<this->dimension;i++)
	{ out<<this->elements[i];
		if (i!=this->dimension-1)
			out <<" , ";
	}
	out<<" )";
	output= out.str();
}

void intRoot::AssignRoot(root& r)
{ this->dimension= r.dimension;
	for (int i=0;i<this->dimension;i++)
	{ this->elements[i]= r.coordinates[i].num;
	}
}

bool intRoot::IsPositive()
{ for (int i=0;i<this->dimension;i++)
	{ if (this->elements[i]<0)
			return false;
	}
	return true;
}

void intRoot::initFromInt(int x1, int x2, int x3, int x4, int x5)
{ this->dimension=root::AmbientDimension;
	this->elements[0]=x1;
	this->elements[1]=x2;
	this->elements[2]=x3;
	this->elements[3]=x4;
	this->elements[4]=x5;
}

void intRoot::initFromInt(	int x1,int x2,int x3, int x4, int x5, int x6,
														int x7,int x8,int x9, int x10, int x11, int x12)
{ this->dimension=root::AmbientDimension;
	this->elements[0]=x1;
	this->elements[1]=x2;
	this->elements[2]=x3;
	this->elements[3]=x4;
	this->elements[4]=x5;
	this->elements[5]=x6;
	this->elements[6]=x7;
	this->elements[7]=x8;
	this->elements[8]=x9;
	this->elements[9]=x10;
	this->elements[10]=x11;
	this->elements[11]=x12;
}

inline bool intRoot::IsGEQNoWeight(intRoot& r)
{ assert(this->dimension==r.dimension);
	for (int i=0;i<this->dimension;i++)
	{ if (r.elements[i]>this->elements[i])
			return false;
	}
	return true;
}

inline bool intRoot::IsHigherThanWRTWeight(intRoot& r, intRoot& theWeights)
{	assert(this->dimension == r.dimension);
	int accum=0;
	for (int i=0;i<this->dimension;i++)
	{ accum+=(this->elements[i]-r.elements[i])*theWeights.elements[i];
	}
	return (accum>0);
}

void intRoot::operator =(const intRoot &right)
{ this->dimension=right.dimension;
	for (int i=0;i<this->dimension;i++)
	{ this->elements[i]=right.elements[i];
	}
}

void intRoot::operator =(int* right)
{ this->dimension=root::AmbientDimension;
	for (int i=0;i<this->dimension;i++)
	{ this->elements[i]=right[i];
	}
}

bool intRoot::operator ==(intRoot &right)
{ assert(this->dimension==right.dimension);
	if (this->dimension!=right.dimension)
		return false;
	for (int i=0;i<this->dimension;i++)
	{ if (this->elements[i]!=right.elements[i]){return false;}
	}
	return true;
}

/*bool intRoot::operator ==(int* right)
{	for (int i=0;i<this->dimension;i++)
	{ if (this->elements[i]!=right[i]){return false;}
	}
	return true;
}*/

void RootToIndexTable::initFromRoots(intRoots& theAlgorithmBasis, intRoot* theWeights)
{ if (theWeights!=0) {this->weights=*theWeights;}
	for (int i=0;i<theAlgorithmBasis.size;i++)
	{ int x= this->ContainsObjectHash(theAlgorithmBasis.TheObjects[i]);
		if (x==-1)
		{	if (theWeights!=0)
			{ this->AddRootAndSort(theAlgorithmBasis.TheObjects[i]);
			}else
			{ this->AddRootPreserveOrder(theAlgorithmBasis.TheObjects[i]);
			}
		}
	}
	this->NumNonRedundantShortRoots= this->size;
	this->ComputeTable();
}

void RootToIndexTable::ComputeTable()
{ intRoot tempR, tempR2, tempR3;
	this->IndicesRedundantShortRoots.init(this->size);
	for (int i=0;i<this->size;i++)
	{ for (int j=0;j<this->size;j++)
		{ for (int k=0;k<root::AmbientDimension;k++)
			{ tempR.elements[k] = this->TheObjects[i].elements[k]-
														this->TheObjects[j].elements[k];
				tempR2.elements[k] = this->TheObjects[i].elements[k]-
														 2*this->TheObjects[j].elements[k];
			}
			this->TableAllowedAminusB[i][j]  = this->getIndex(tempR);
			this->TableAllowedAminus2B[i][j] = this->getIndex(tempR2);
		}
		for (int j=0;j<root::AmbientDimension;j++)
		{	tempR3.elements[j]= this->TheObjects[i].elements[j]*2;
		}
		this->IndicesDoublesOfRedundantShortRoots[i] = this->getIndex(tempR3);
		if (IndicesDoublesOfRedundantShortRoots[i]!=-1)
		{ this->IndicesRedundantShortRoots.AddSelection(i);
		}
	}
}

int RootToIndexTable::AddRootPreserveOrder(intRoot& theRoot)
{ this->AddObjectOnTopHash(theRoot);
	return this->size-1;
}

int RootToIndexTable::AddRootAndSort(intRoot& theRoot)
{ ListBasicObjects<intRoot> tempList;
	tempList.CopyFromBase(*this);
	int index=0;
	for (index=0;index<tempList.size;index++)
	{ if (theRoot.IsHigherThanWRTWeight(tempList.TheObjects[index],this->weights))
			break;
	}
	tempList.ShiftUpExpandOnTop(index);
	tempList.TheObjects[index]= theRoot;
	this->ClearTheObjects();
	for (int i=0;i<tempList.size;i++)
	{ this->AddObjectOnTopHash(tempList.TheObjects[i]);
	}
	return index;
}

int RootToIndexTable::getIndex(intRoot& TheRoot)
{ return this->ContainsObjectHash(TheRoot);
/*	int index= TheRoot.HashFunction();
	index%=this->HashSize;
	if (index<0){index+=this->HashSize;}
	for (int i=0;i<this->TheHashedArrays[index].size;i++)
	{ int tempI= this->TheHashedArrays[index].TheObjects[i];
		if (this->TheObjects[tempI]==TheRoot)
		{return tempI;}
	}
	return -1;*/
}

void SubsetWithMultiplicities::init(int NumElements, int MaxMult)
{ this->Multiplicities.SetSizeExpandOnTopNoObjectInit(NumElements);
	for (int i=0;i<this->Multiplicities.size;i++)
	{ this->Multiplicities.TheObjects[i]=0;
	}
	this->elements.SetActualSizeAtLeastExpandOnTop(NumElements);
	this->elements.size=0;
	this->MaxMultiplicity=MaxMult;
}

void SubsetWithMultiplicities::IncrementSubset()
{ for (int i=this->Multiplicities.size-1;i>=0;i--)
	{ if (this->Multiplicities.TheObjects[i]<this->MaxMultiplicity)
		{ if (this->Multiplicities.TheObjects[i]==0)
			{	this->elements.AddObjectOnTop(i);}
			this->Multiplicities.TheObjects[i]++;
			return;
		}
		else
		{ this->Multiplicities.TheObjects[i]=0;
			this->elements.size--;
		}
	}
}

void SubsetWithMultiplicities::ComputeElements()
{ this->elements.size=0;
	for (int i=0;i<this->Multiplicities.size;i++)
	{ if (this->Multiplicities.TheObjects[i]>0)
			this->elements.AddObjectOnTop(i);
	}
}

int SubsetWithMultiplicities::CardinalitySelection()
{ return this->Multiplicities.size;
}

void WeylGroup::ReflectBetaWRTAlpha(root& alpha, root &beta, bool RhoAction, root& output)
{ static Rational alphaShift, tempRat;
	output.dimension=root::AmbientDimension;
	alphaShift.MakeZero();
	Rational lengthA;
	lengthA.MakeZero();
	if (RhoAction)
	{ beta.Add(this->rho);
	}
	for (int i=0;i<this->KillingFormMatrix.NumRows;i++)
	{ for (int j=0;j<this->KillingFormMatrix.NumCols;j++)
		{ tempRat.Assign(beta.coordinates[j]);
			tempRat.MultiplyBy(alpha.coordinates[i]);
			tempRat.MultiplyByInteger(-this->KillingFormMatrix.elements[i][j]*2);
			alphaShift.Add(tempRat);
			RationalTimesRational(alpha.coordinates[i],alpha.coordinates[j],tempRat);
			tempRat.MultiplyByInteger(this->KillingFormMatrix.elements[i][j]);
			lengthA.Add(tempRat);
		}
	}
	alphaShift.DivideBy(lengthA);
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempRat.Assign(alphaShift);
		tempRat.MultiplyBy(alpha.coordinates[i]);
		tempRat.Add(beta.coordinates[i]);
		output.coordinates[i].Assign(tempRat);
	}
	if (RhoAction)
	{ beta.Subtract(this->rho);
		output.Subtract(this->rho);
	}
}

void WeylGroup::SimpleReflectionIntRoot(int index, root& theRoot,
																				bool RhoAction)
{ static Rational alphaShift, tempRat;
	alphaShift.MakeZero();
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ tempRat.Assign(theRoot.coordinates[i]);
		tempRat.MultiplyByInteger(-KillingFormMatrix.elements[index][i]*2);
		alphaShift.Add(tempRat);
	}
	alphaShift.DivideByInteger(this->KillingFormMatrix.elements[index][index]);
	if (RhoAction){alphaShift.AddInteger(-1);}
	theRoot.coordinates[index].Add(alphaShift);
}

void WeylGroup::SimpleReflectionRootAlg
					(	int index, PolynomialsRationalCoeff& theRoot,
						bool RhoAction)
{	int lengthA=this->KillingFormMatrix.elements[index][index];
	static PolynomialRationalCoeff AscalarB, tempP;
	AscalarB.Nullify(root::AmbientDimension);
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ tempP.Nullify(root::AmbientDimension);
		tempP.CopyFromPoly(theRoot.TheObjects[i]);
		tempP.TimesInteger(KillingFormMatrix.elements[index][i]);
		AscalarB.AddPolynomial(tempP);
	}
	AscalarB.TimesInteger(-2);
	AscalarB.DivByInteger(lengthA);
	theRoot.TheObjects[index].AddPolynomial(AscalarB);
	if (RhoAction){theRoot.TheObjects[index].AddConstant(RMOne);}
}

void WeylGroup::ActOnRootByGroupElement(	int index,
																						root &theRoot, bool RhoAction)
{ for (int i=0;i<this->TheObjects[index].size;i++)
	{ this->SimpleReflectionIntRoot(this->TheObjects[index].TheObjects[i],theRoot,RhoAction);
	}
}

void WeylGroup::GenerateRootSystemFromKillingFormMatrix()
{ root tempRoot;
	roots startRoots;
	this->RootSystem.ClearTheObjects();
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{	tempRoot.MakeZero();
		tempRoot.coordinates[i].AssignInteger(1);
		startRoots.AddRoot(tempRoot);
	}
	this->GenerateOrbit(startRoots,false,this->RootSystem);
	this->RootsOfBorel.size=0;
	for (int i=0;i<this->RootSystem.size;i++)
	{ if (this->RootSystem.TheObjects[i].IsPositiveOrZero())
		{ this->RootsOfBorel.AddRoot(this->RootSystem.TheObjects[i]);
		}
	}
}

void WeylGroup::GenerateOrbit(roots& theRoots, bool RhoAction,
															hashedRoots& output)
{ static WeylGroup tempW;
	this->GenerateOrbit(theRoots,RhoAction,output,false,tempW);
}

void WeylGroup::GenerateOrbit( roots& theRoots, bool RhoAction,
															 hashedRoots& output,
															 bool ComputingAnOrbitGeneratingSubsetOfTheGroup,
															 WeylGroup& outputSubset)
{	for (int i=0;i<theRoots.size;i++)
	{ output.AddObjectOnTopHash(theRoots.TheObjects[i]);
	}
	root currentRoot;
	ElementWeylGroup tempEW;
	if (ComputingAnOrbitGeneratingSubsetOfTheGroup)
	{ tempEW.size=0;
		outputSubset.KillingFormMatrix.Assign(this->KillingFormMatrix);
		outputSubset.size=0;
		outputSubset.AddObjectOnTopHash(tempEW);
	}
	for (int i=0;i<output.size;i++)
	{	if (ComputingAnOrbitGeneratingSubsetOfTheGroup)
		{ tempEW=outputSubset.TheObjects[i];}
		for (int j=0; j<this->KillingFormMatrix.NumRows;j++)
		{	currentRoot=output.TheObjects[i];
			this->SimpleReflectionIntRoot(j,currentRoot,RhoAction);
			if (output.ContainsObjectHash(currentRoot)==-1)
			{	output.AddObjectOnTopHash(currentRoot);
				if (ComputingAnOrbitGeneratingSubsetOfTheGroup)
				{ tempEW.AddObjectOnTop(j);
					outputSubset.AddObjectOnTop(tempEW);
					tempEW.PopIndexSwapWithLast(tempEW.size-1);
				}
			}
		}
	}
}

void WeylGroup::RootScalarKillingFormMatrixRoot(root& r1, root& r2, Rational& output)
{ output.MakeZero();
	for (int i=0;i<this->KillingFormMatrix.NumRows;i++)
	{	for (int j=0;j<this->KillingFormMatrix.NumCols;j++)
		{ static Rational tempRat;
			tempRat.Assign(r1.coordinates[i]);
			tempRat.MultiplyBy(r2.coordinates[j]);
			tempRat.MultiplyByInteger(this->KillingFormMatrix.elements[i][j]);
			output.Add(tempRat);
		}
	}
}

void WeylGroup::GenerateOrbitAlg(root& ChamberIndicator,
						PolynomialsRationalCoeff& input,
						PolynomialsRationalCoeffCollection& output,
						bool RhoAction,
						bool PositiveWeightsOnly, Cone* LimitingCone,
						bool onlyLowerWeights)
{ hashedRoots TheIndicatorsOrbit;
	WeylGroup OrbitGeneratingSubset;
	static roots tempRoots;
	tempRoots.size=0;
	tempRoots.AddRoot(ChamberIndicator);
	this->GenerateOrbit(tempRoots,RhoAction,TheIndicatorsOrbit,
											true,OrbitGeneratingSubset);
	TheIndicatorsOrbit.ComputeDebugString();
	roots TempTest;
	root tempRoot;
	for (int i=0;i<OrbitGeneratingSubset.size;i++)
	{ tempRoot= ChamberIndicator;
		OrbitGeneratingSubset.ActOnRootByGroupElement(i,tempRoot,RhoAction);
		TempTest.AddRoot(tempRoot);
	}
	TempTest.ComputeDebugString();
	static PolynomialsRationalCoeff theRoot;
	PolyFormatLocal.MakeAlphabetxi();
	output.size=0;
	output.ChamberIndicators.size=0;
	output.SetActualSizeAtLeastExpandOnTop(OrbitGeneratingSubset.size);
	input.ComputeDubugString();
	for (int i=0;i<OrbitGeneratingSubset.size;i++)
	{	bool tempBool= TheIndicatorsOrbit.TheObjects[i].IsPositiveOrZero()||(!PositiveWeightsOnly);
		if (LimitingCone!=0)
		{ tempBool = tempBool && LimitingCone->IsInCone(TheIndicatorsOrbit.TheObjects[i]);
		}
		if (onlyLowerWeights)
		{ root tempRoot2;
			tempRoot2.Assign(ChamberIndicator);
			tempRoot2.Subtract(TheIndicatorsOrbit.TheObjects[i]);
			tempBool = tempBool && tempRoot2.IsPositiveOrZero();
		}
		if (tempBool)
		{	theRoot= input;
			OrbitGeneratingSubset.ActOnRootAlgByGroupElement(i,theRoot,RhoAction);
			output.AddObjectOnTop(theRoot);
			output.ChamberIndicators.AddRoot(TheIndicatorsOrbit.TheObjects[i]);
		}
	}
	output.ComputeDubugString();
}

void WeylGroup::ActOnRootAlgByGroupElement
					(	int index, PolynomialsRationalCoeff& theRoot,
						bool RhoAction)
{	for (int i=0;i<this->TheObjects[index].size;i++)
	{ this->SimpleReflectionRootAlg(this->TheObjects[index].TheObjects[i],theRoot,RhoAction);
	}
}

void WeylGroup::ComputeWeylGroupAndRootsOfBorel(roots& output)
{	this->ComputeWeylGroup();
	output.size=0;
	for (int i=0;i<this->RootSystem.size;i++)
	{ if (this->RootSystem.TheObjects[i].IsPositiveOrZero())
		{ output.AddRoot(this->RootSystem.TheObjects[i]);
		}
	}
}

void WeylGroup::ComputeRootsOfBorel(roots& output)
{ output.size=0;
	this->RootSystem.ClearTheObjects();
	this->GenerateRootSystemFromKillingFormMatrix();
	output.CopyFromBase(this->RootsOfBorel);
}

void WeylGroup::ElementToString(std::string& output)
{ std::string tempS;
	std::stringstream out;
	out <<"Size: "<<this->size<<"\n";
//	out <<"Number of roots: "<<this->RootSystem.size<<"\n";
	this->rho.ElementToString(tempS);
	out <<"rho:"<<tempS<<"\n";
	this->RootSystem.ElementToString(tempS);
	out<<"Root system:\n"<< tempS<<"\n";
	out <<"Elements of the group:\n";
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		out << i<<". "<<tempS<<"\n";
	}
	output= out.str();
}

void WeylGroup::MakeArbitrary(char WeylGroupLetter,unsigned char n)
{ switch(WeylGroupLetter)
	{ case 'A': this->MakeAn(n);
		break;
		case 'B': this->MakeBn(n);
		break;
		case 'C': this->MakeCn(n);
		break;
		case 'D': this->MakeDn(n);
		break;
		case 'E': this->MakeEn(n);
		break;
		case 'F': this->MakeF4();
		break;
		case 'G': this->MakeG2();
		break;
	}
}

void WeylGroup::MakeDn(unsigned char n)
{ this->MakeAn(n);
	this->KillingFormMatrix.elements[n-1][n-2]=0;
	this->KillingFormMatrix.elements[n-2][n-1]=0;
	this->KillingFormMatrix.elements[n-3][n-1]=-1;
	this->KillingFormMatrix.elements[n-1][n-3]=-1;
}

void WeylGroup::MakeAn(unsigned char n)
{	root::AmbientDimension=n;
	this->rho.dimension=n;
	this->KillingFormMatrix.init(n,n);
	this->KillingFormMatrix.NullifyAll();
	for (int i=0;i<n-1;i++)
	{ this->KillingFormMatrix.elements[i][i]=2;
		this->KillingFormMatrix.elements[i+1][i]=-1;
		this->KillingFormMatrix.elements[i][i+1]=-1;
	}
	this->KillingFormMatrix.elements[n-1][n-1]=2;
}

void WeylGroup::MakeEn(unsigned char n)
{	this->MakeAn(n);
	this->KillingFormMatrix.elements[0][1]=0;
	this->KillingFormMatrix.elements[1][0]=0;
	this->KillingFormMatrix.elements[1][2]=0;
	this->KillingFormMatrix.elements[2][1]=0;
	this->KillingFormMatrix.elements[0][2]=-1;
	this->KillingFormMatrix.elements[1][3]=-1;
	this->KillingFormMatrix.elements[2][0]=-1;
	this->KillingFormMatrix.elements[3][1]=-1;
}

void WeylGroup::MakeF4()
{	root::AmbientDimension=4;
	this->rho.dimension=4;
	this->KillingFormMatrix.init(4,4);
	this->KillingFormMatrix.elements[0][0]=2 ;this->KillingFormMatrix.elements[0][1]=-1;this->KillingFormMatrix.elements[0][2]=0 ;this->KillingFormMatrix.elements[0][3]=0 ;
	this->KillingFormMatrix.elements[1][0]=-1;this->KillingFormMatrix.elements[1][1]=2 ;this->KillingFormMatrix.elements[1][2]=-2;this->KillingFormMatrix.elements[1][3]=0 ;
	this->KillingFormMatrix.elements[2][0]=0 ;this->KillingFormMatrix.elements[2][1]=-2;this->KillingFormMatrix.elements[2][2]=4 ;this->KillingFormMatrix.elements[2][3]=-2;
	this->KillingFormMatrix.elements[3][0]=0 ;this->KillingFormMatrix.elements[3][1]=0 ;this->KillingFormMatrix.elements[3][2]=-2;this->KillingFormMatrix.elements[3][3]=4 ;
}

void WeylGroup::MakeG2()
{ root::AmbientDimension=2;
	this->rho.dimension=2;
	this->KillingFormMatrix.init(2,2);
	this->KillingFormMatrix.elements[0][0]=6;
	this->KillingFormMatrix.elements[1][1]=2;
	this->KillingFormMatrix.elements[1][0]=-3;
	this->KillingFormMatrix.elements[0][1]=-3;
}

void WeylGroup::MakeBn(unsigned char n)
{ this->MakeAn(n);
	this->KillingFormMatrix.elements[n-1][n-1]=1;
}

void WeylGroup::MakeCn(unsigned char n)
{ this->MakeAn(n);
	this->KillingFormMatrix.elements[n-1][n-1]=4;
	this->KillingFormMatrix.elements[n-2][n-1]=-2;
	this->KillingFormMatrix.elements[n-1][n-2]=-2;
}

void WeylGroup::ComputeWeylGroup()
{ this->ComputeRho();
	this->ComputeDebugString();
	roots tempRoots;
	tempRoots.AddRoot(this->rho);
	this->ClearTheObjects();
	static hashedRoots tempRoots2;
	tempRoots2.ClearTheObjects();
	this->GenerateOrbit(tempRoots,false,tempRoots2,true,*this);
}

void WeylGroup::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

int WeylGroup::length(int index)
{ return this->TheObjects[this->size-1].size-this->TheObjects[index].size;
}

void WeylGroup::ComputeRho()
{ this->GenerateRootSystemFromKillingFormMatrix();
	//this->ComputeDebugString();
	this->rho.MakeZero();
	for (int i=0;i<this->RootSystem.size;i++)
	{	if (RootSystem.TheObjects[i].IsPositiveOrZero() )
		{ this->rho.Add(RootSystem.TheObjects[i]);
		}
	}
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ this->rho.coordinates[i].DivideByInteger(2);
	}
}

void ElementWeylGroup::operator =(const ElementWeylGroup &right)
{ this->CopyFromBase(right);
}

bool ElementWeylGroup::operator ==(const ElementWeylGroup &right)
{ if (this->size!=right.size) return false;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i]!=right.TheObjects[i])
			return false;
	}
	return true;
}

void ElementWeylGroup::ElementToString(std::string& output)
{ std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ out <<this->TheObjects[i]<<",";
	}
	output= out.str();
}

int ElementWeylGroup::HashFunction()
{	int top = Minimum(this->size,::SomeRandomPrimesSize);
	int result =0;
	for (int i=0;i<top;i++)
	{ result+=this->TheObjects[i]*::SomeRandomPrimes[i];
	}
	return result;
}

void PolynomialsRationalCoeff::operator=(const PolynomialsRationalCoeff& right)
{ this->CopyFromBase(right);
}

void PolynomialsRationalCoeff::MakeUsualParametricRoot()
{ PolynomialRationalCoeff tempP;
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempP.MakeNVarDegOnePoly(root::AmbientDimension,i,ROne);
		this->AddObjectOnTop(tempP);
	}
}

void PolynomialsRationalCoeff::Substitution
				(PolynomialsRationalCoeff& theSub, short NumVarsTarget)
{ static PolynomialRationalCoeff tempP;
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].Substitution(theSub, tempP,NumVarsTarget);
		this->TheObjects[i].CopyFromPoly(tempP);
	}
}

void PolynomialsRationalCoeff::MakeOneParameterSubFromDirection(root& direction)
{ Monomial<Rational> tempM;
	tempM.init(1);
	tempM.degrees[0]=1;
	this->SetSizeExpandOnTopNoObjectInit(direction.dimension);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Nullify(1);
		tempM.Coefficient.Assign(direction.coordinates[i]);
		this->TheObjects[i].AddMonomial(tempM);
	}
}

void PolynomialsRationalCoeff::MakeOneParameterSubFromDirectionInts
				(int x1,int x2,int x3, int x4, int x5)
{	root tempRoot;
	tempRoot.InitFromIntegers(x1,x2,x3,x4,x5);
	this->MakeOneParameterSubFromDirection(tempRoot);
}


void PolynomialsRationalCoeff::MakeOneParameterSubFromDirectionIntsAndConstants
				(int x1, int x2, int x3, int x4, int x5,
				 int c1, int c2, int c3, int c4, int c5)
{	this->MakeOneParameterSubFromDirectionInts(x1,x2,x3,x4,x5);
	Rational rc1, rc2, rc3, rc4, rc5;
	rc1.AssignInteger(c1); rc2.AssignInteger(c2);	rc3.AssignInteger(c3);
	rc4.AssignInteger(c4); rc5.AssignInteger(c5);
	this->TheObjects[0].AddConstant(rc1);
	this->TheObjects[1].AddConstant(rc2);
	this->TheObjects[2].AddConstant(rc3);
	this->TheObjects[3].AddConstant(rc4);
	this->TheObjects[4].AddConstant(rc5);
}

void PolynomialsRationalCoeff::MakeSubFromMatrixIntAndDen(MatrixInt &theMat, int Den)
{ MatrixRational tempMat;
	tempMat.AssignMatrixIntWithDen(theMat,Den);
	this->MakeSubFromMatrixRational(tempMat);
}

void PolynomialsRationalCoeff::MakeSubFromMatrixRational(MatrixRational &theMat)
{ this->SetSizeExpandOnTopNoObjectInit(theMat.NumCols);
	for (int i=0;i<this->size;i++)
	{ static Monomial<Rational> tempM;
		this->TheObjects[i].Nullify(theMat.NumRows-1);
		for (int j=0;j<theMat.NumRows-1;j++)
		{	tempM.init(theMat.NumRows-1);
			tempM.degrees[j]=1;
			tempM.Coefficient.Assign(theMat.elements[j][i]);
			this->TheObjects[i].AddMonomial(tempM);
		}
		this->TheObjects[i].AddConstant(theMat.elements[theMat.NumRows-1][i]);
	}
}

bool PolynomialsRationalCoeff::operator==(const PolynomialsRationalCoeff& right)
{ assert(false);
	return false;
}

void intRoots::ElementToString(std::string& output)
{ std::string tempS;
	output.clear();
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		output.append(tempS);
		output.append("\n");
	}
}

void intRoots::AssignRoots(roots&r)
{ this->SetSizeExpandOnTopNoObjectInit(r.size);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].AssignRoot(r.TheObjects[i]);
	}
}

void intRoots::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void intRoots::BubbleSort(intRoot* weights)
{ intRoot tempRoot;
	for (int i=0;i<this->size;i++)
	{ for (int j=i+1;j<this->size;j++)
		{ bool tempBool;
			if (weights==0)
			{ tempBool = this->TheObjects[j].IsGEQNoWeight(this->TheObjects[i]);
			}
			else
			{ tempBool = this->TheObjects[j].IsHigherThanWRTWeight(this->TheObjects[i],*weights);
			}
			if (tempBool)
			{ tempRoot= this->TheObjects[i];
				this->TheObjects[i]=this->TheObjects[j];
				this->TheObjects[j]=tempRoot;
			}
		}
	}
}

void hashedRoots::ElementToString(std::string &output)
{ std::stringstream  out;
	std::string tempS;
	out <<"Number of vectors: "<<this->size<<"\n";
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		out<<tempS<<"\n";
	}
	output= out.str();
}

void hashedRoots::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void PolynomialsRationalCoeff::ElementToString(std::string &output)
{ output.clear();
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].StringPrintOutAppend(output,PolyFormatLocal);
		output.append(", ");
	}
}

void PolynomialsRationalCoeff::ComputeDubugString()
{ this->ElementToString(this->DebugString);
}

void PolynomialsRationalCoeff::ComputeB(PolynomialRationalCoeff& output, int cutOffIndex)
{ static PolynomialsRationalCoeff EpsForm;
	PolynomialRationalCoeff tempP;
	EpsForm.SetSizeExpandOnTopNoObjectInit(root::AmbientDimension);
	EpsForm.TheObjects[0].CopyFromPoly(this->TheObjects[0]);
	for (int i=1;i<root::AmbientDimension;i++)
	{ EpsForm.TheObjects[i].CopyFromPoly(this->TheObjects[i]);
		tempP.CopyFromPoly(this->TheObjects[i-1]);
		tempP.TimesConstant(RMOne);
		EpsForm.TheObjects[i].AddPolynomial(tempP);
	}
	EpsForm.TheObjects[3].AddPolynomial(this->TheObjects[4]);
	for (int j=cutOffIndex+1;j<root::AmbientDimension;j++)
	{	if (!EpsForm.TheObjects[j].IsGreaterThanZeroLexicographicOrder()
				)
		{ EpsForm.TheObjects[j].TimesConstant(RMOne);
		}
	}
	output.Nullify(2);
	for (int i=0;i<root::AmbientDimension;i++)
	{ output.AddPolynomial(EpsForm.TheObjects[i]);
	}
}

void PolynomialsRationalCoeffCollection::ElementToString(std::string &output)
{ this->ElementToStringComputeFunctionB(output,false);
}

void PolynomialsRationalCoeffCollection::ElementToStringComputeFunctionB
					(std::string &output, bool computingB)
{ std::string tempS;
	std::stringstream  out;
	out<<"Number of elements: "<<this->size<<"\n";
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		out<< tempS;
		if (computingB)
		{ static PolynomialRationalCoeff tempP;
			this->TheObjects[i].ComputeB(tempP, 2);
			tempP.ComputeDebugString();
			out <<"  function B: "<<tempP.DebugString;
		}
		out<<"\n";
	}
	output=out.str();
}

void PolynomialsRationalCoeffCollection::
				Substitution(PolynomialsRationalCoeff& theSub, short NumVarsTarget)
{ for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Substitution(theSub,NumVarsTarget);
	}
}

void PolynomialsRationalCoeffCollection::ComputeDubugString()
{ this->ElementToString(this->DebugString);
}

void VermaModulesWithMultiplicities::WriteKLCoeffsToFile(std::fstream& output, ListBasicObjects<int>& KLcoeff,int TopIndex)
{ output.flush();
	output<< "Top_index: "<<TopIndex<<"\n";
	std::string tempS;
	this->KLcoeffsToString(KLcoeff,	tempS);
	output<<tempS;
}

int VermaModulesWithMultiplicities::ReadKLCoeffsFromFile(std::fstream& input, ListBasicObjects<int>& output)
{	std::string tempS;
	int TopIndex;
	input>>tempS>>TopIndex;
	output.SetSizeExpandOnTopNoObjectInit(this->size);
	for (int i=0;i<this->size;i++)
	{ input>>tempS>> output.TheObjects[i];
	}
	return TopIndex;
}


void VermaModulesWithMultiplicities::KLcoeffsToString
				(ListBasicObjects<int>& theKLCoeffs, std::string& output)
{ std::stringstream out;
	for (int i=0;i<theKLCoeffs.size;i++)
	{  out<< i<<".  "<<theKLCoeffs.TheObjects[i]<<"\n";
	}
	output=out.str();
}

void VermaModulesWithMultiplicities::initTheMults()
{ this->TheMultiplicities.SetSizeExpandOnTopNoObjectInit(this->size);
	this->Explored.SetSizeExpandOnTopNoObjectInit(this->size);
	for (int i=0;i<this->size;i++)
	{ this->TheMultiplicities.TheObjects[i]=0;
		this->Explored.TheObjects[i]=false;
	}
	this->NextToExplore=0;
	this->LowestNonExplored=0;
}

void VermaModulesWithMultiplicities::Check()
{ for (int i=0;i<this->size;i++)
	{ this->Compute(i);
//		bool found=false;
		for (int j=0;j<this->size;j++)
		{ if (this->TheMultiplicities.TheObjects[i]!=0 &&
					this->TheMultiplicities.TheObjects[i]!=1 &&
					this->TheMultiplicities.TheObjects[i]!=-1)
			{ this->ComputeDebugString();
				Stop();
			}
		}
	}
}

void VermaModulesWithMultiplicities::Compute(int x)
{ this->initTheMults();
	this->TheMultiplicities.TheObjects[x]=1;
	this->ComputeDebugString();
	while (this->NextToExplore!=-1)
	{	for (int i=0;i<this->BruhatOrder.TheObjects[this->NextToExplore].size;i++)
		{ int a=this->BruhatOrder.TheObjects[this->NextToExplore].TheObjects[i];
			this->TheMultiplicities.TheObjects[a]-=
						this->TheMultiplicities.TheObjects[this->NextToExplore];
		}
		this->Explored.TheObjects[this->NextToExplore]=true;
		this->FindNextToExplore();
	//	this->ComputeDebugString();
	}
}

void VermaModulesWithMultiplicities::FindNextToExplore()
{ bool foundNonExplored=false;
	for (int i=this->LowestNonExplored;i<this->size;i++)
	{ if (!this->Explored.TheObjects[i])
		{ if (!foundNonExplored)
			{ this->LowestNonExplored=i;
				foundNonExplored=true;
			}
			if (this->IsMaxNonEplored(i))
			{ this->NextToExplore=i;
				return;
			}
		}
	}
	this->NextToExplore=-1;
}

bool VermaModulesWithMultiplicities::IsMaxNonEplored(int index)
{ for (int i=this->LowestNonExplored;i<this->size;i++)
	{ if (!this->Explored.TheObjects[i]&& i!=index)
		{ root tempRoot;
			tempRoot.Assign(this->TheObjects[i]);
			tempRoot.Subtract(this->TheObjects[index]);
			if (tempRoot.IsPositiveOrZero())
				return false;
		}
	}
	return true;
}

void VermaModulesWithMultiplicities::ElementToString(std::string& output)
{ std::string tempS;
	std::stringstream out;
	out<<"Next to explore: "<<this->NextToExplore<<"\n Orbit of rho:\n";
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		out << tempS<<"   :  "<<this->TheMultiplicities.TheObjects[i];
		if (this->Explored.TheObjects[i])
		{ out<<" Explored\n";
		}
		else
		{ out<<" not Explored\n";
		}
	}
	out<<"Bruhat order:\n";
	for (int i=0;i<this->size;i++)
	{ out <<i<<".   ";
		for(int j=0;j<this->BruhatOrder.TheObjects[i].size;j++)
		{	out <<this->BruhatOrder.TheObjects[i].TheObjects[j]<<",";
		}
		out<<"\n";
	}
	this->RPolysToString(tempS);
	out<<"R Polynomials:\n"<<tempS;
	this->KLPolysToString(tempS);
	out<<"Kazhdan-Lusztig Polynomials:\n"<<tempS;
	output= out.str();
}

void VermaModulesWithMultiplicities::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void VermaModulesWithMultiplicities::GeneratePartialBruhatOrder()
{ this->BruhatOrder.SetSizeExpandOnTopNoObjectInit(this->size);
	this->InverseBruhatOrder.SetSizeExpandOnTopNoObjectInit(this->size);
	this->SimpleReflectionsActionList.SetSizeExpandOnTopNoObjectInit(this->size);
	this->ComputeDebugString();
	for (int i=0;i<this->size;i++)
	{ this->SimpleReflectionsActionList.TheObjects[i].
			SetActualSizeAtLeastExpandOnTop(root::AmbientDimension);
		for (int j=0;j<root::AmbientDimension;j++)
		{ root tempRoot,tempRoot2;
			tempRoot.Assign(this->TheObjects[i]);
			tempRoot2.Assign(this->TheObjects[i]);
			this->TheWeylGroup->SimpleReflectionIntRoot(j,tempRoot,true);
			int x= this->ContainsObjectHash(tempRoot);
			this->SimpleReflectionsActionList.TheObjects[i].AddObjectOnTop(x);
			tempRoot2.Subtract(tempRoot);
			if (tempRoot2.IsPositiveOrZero() && !tempRoot2.IsEqualTo(ZeroRoot) )
			{ this->BruhatOrder.TheObjects[i].AddObjectOnTop(x);
				this->InverseBruhatOrder.TheObjects[x].AddObjectOnTop(i);
			}
		}
	}
	this->ComputeDebugString();
}

void VermaModulesWithMultiplicities::ExtendOrder()
{ this->initTheMults();
	int x=this->FindLowestBruhatNonExplored();
	while (x!=-1)
	{	for (int j=0;j<this->BruhatOrder.TheObjects[x].size;j++)
		{ int a= this->BruhatOrder.TheObjects[x].TheObjects[j];
			this->MergeBruhatLists(a,x);
		}
		this->Explored.TheObjects[x]=true;
		x=this->FindLowestBruhatNonExplored();
		this->ComputeDebugString();
	}
}

int VermaModulesWithMultiplicities::FindLowestBruhatNonExplored()
{ for (int i=this->size-1;i>=0;i--)
	{ if (!this->Explored.TheObjects[i])
		{ bool isGood=true;
			for (int j=0;j<this->BruhatOrder.TheObjects[i].size;j++)
			{ int a= this->BruhatOrder.TheObjects[i].TheObjects[j];
				if (!this->Explored.TheObjects[a])
				{ isGood=false;
					break;
				}
			}
			if (isGood ) {return i;}
		}
	}
	return -1;
}

int VermaModulesWithMultiplicities::FindHighestBruhatNonExplored
				(ListBasicObjects<bool>& theExplored)
{ for (int i=0;i<this->size;i++)
	{ if (!theExplored.TheObjects[i])
		{ bool isGood=true;
			for (int j=0;j<this->InverseBruhatOrder.TheObjects[i].size-1;j++)
			{ int a= this->InverseBruhatOrder.TheObjects[i].TheObjects[j];
				if (!theExplored.TheObjects[a])
				{ isGood=false;
					break;
				}
			}
			if (isGood ) {return i;}
		}
	}
	return -1;
}

void VermaModulesWithMultiplicities::MergeBruhatLists(int fromList, int toList)
{ for (int i=0;i<this->BruhatOrder.TheObjects[fromList].size;i++)
	{ bool found=false;
		for (int j=0;j<this->BruhatOrder.TheObjects[toList].size;j++)
		{ if (this->BruhatOrder.TheObjects[toList].TheObjects[j]==
					this->BruhatOrder.TheObjects[fromList].TheObjects[i])
			{ found = true;
				break;
			}
		}
		if (!found)
		{	this->BruhatOrder.TheObjects[toList].AddObjectOnTop
				(this->BruhatOrder.TheObjects[fromList].TheObjects[i]);
		}
	}
}

void VermaModulesWithMultiplicities::ComputeFullBruhatOrder()
{ this->initTheMults();
	this->GeneratePartialBruhatOrder();
	this->ExtendOrder();
}

void VermaModulesWithMultiplicities::ComputeKLcoefficientsFromChamberIndicator
					(root& ChamberIndicator, ListBasicObjects<int>& output)
{ this->ComputeKLcoefficientsFromIndex
		(this->ChamberIndicatorToIndex(ChamberIndicator),output);
}


int VermaModulesWithMultiplicities::ChamberIndicatorToIndex(root &ChamberIndicator)
{	root ChamberIndicatorPlusRho;
	ChamberIndicatorPlusRho.Assign(ChamberIndicator);
	ChamberIndicatorPlusRho.Add(this->TheWeylGroup->rho);
	for (int i=0;i<this->size;i++)
	{	Rational tempRat1, tempRat2;
		bool tempBool1, tempBool2;
		bool haveSameSigns=true;
		for (int j=0;j<this->TheWeylGroup->RootSystem.size;j++)
		{ this->TheWeylGroup->RootScalarKillingFormMatrixRoot
				(ChamberIndicatorPlusRho, this->TheWeylGroup->RootSystem.TheObjects[j],tempRat1);
			static root tempRoot; tempRoot.dimension=root::AmbientDimension;
			tempRoot.Assign(this->TheObjects[i]);
			tempRoot.Add(this->TheWeylGroup->rho);
			this->TheWeylGroup->RootScalarKillingFormMatrixRoot
				(tempRoot, this->TheWeylGroup->RootSystem.TheObjects[j],tempRat2);
			tempBool1=tempRat1.IsGreaterThan(RZero);
			tempBool2=tempRat2.IsGreaterThan(RZero);
			assert(!tempRat1.IsEqualTo(RZero));
			assert(!tempRat2.IsEqualTo(RZero));
			if (tempBool1!=tempBool2)
			{ haveSameSigns=false; break;
			}
		}
		if (haveSameSigns)
		{return i;
		}
	}
	return -1;
}

void VermaModulesWithMultiplicities::ComputeKLcoefficientsFromIndex
				(int ChamberIndex, ListBasicObjects<int>& output)
{ output.SetSizeExpandOnTopNoObjectInit(this->size);
	this->ComputeKLPolys(this->TheWeylGroup,ChamberIndex);
	for (int i=0;i<this->KLPolys.TheObjects[ChamberIndex].size;i++ )
	{ output.TheObjects[i]=this->KLPolys.TheObjects[ChamberIndex].TheObjects[i].
															Substitution(1);
		if((this->TheWeylGroup->length(i)+this->TheWeylGroup->length(ChamberIndex))%2!=0)
		{	output.TheObjects[i]*=-1;
		}
	}
}

void VermaModulesWithMultiplicities::initFromWeyl(WeylGroup* theWeylGroup)
{ this->TheWeylGroup= theWeylGroup;
	roots tempRoots;
	tempRoots.AddRoot(this->TheWeylGroup->rho);
	this->TheWeylGroup->GenerateOrbit(tempRoots,true,*this);
	this->initTheMults();
}

void VermaModulesWithMultiplicities::ComputeKLPolys
				(WeylGroup* theWeylGroup, int TopChamberIndex)
{	this->GeneratePartialBruhatOrder();
	PolyFormatLocal.alphabet[0]="q";
	this->ComputeRPolys();
//	this->ComputeDebugString();
	this->KLPolys.SetSizeExpandOnTopNoObjectInit(this->size);
	for (int i=0;i<this->size;i++)
	{ this->Explored.TheObjects[i]=false;
	}
	this->KLPolys.TheObjects[TopChamberIndex].SetSizeExpandOnTopNoObjectInit(this->size);
	int highestNonExplored = this->FindHighestBruhatNonExplored(this->Explored);
	while(highestNonExplored!=-1)
	{	this->ComputeKLxy(TopChamberIndex,highestNonExplored);
		this->Explored.TheObjects[highestNonExplored]=true;
		highestNonExplored= this->FindHighestBruhatNonExplored(this->Explored);
	}
}

void VermaModulesWithMultiplicities::ComputeRPolys()
{	this->RPolys.SetSizeExpandOnTopNoObjectInit(this->size);
	for (int i=0;i<this->size;i++)
	{ this->Explored.TheObjects[i]=false;
		this->RPolys.TheObjects[i].SetSizeExpandOnTopNoObjectInit(this->size);
	}
	this->LowestNonExplored=this->FindLowestBruhatNonExplored();
	ListBasicObjects<bool> ExploredFromTop;
	ExploredFromTop.SetSizeExpandOnTopNoObjectInit(this->size);
	while(this->LowestNonExplored!=-1)
	{ for (int i=0;i<this->size;i++)
		{ ExploredFromTop.TheObjects[i]=false;
		}
		int a= this->FindHighestBruhatNonExplored(ExploredFromTop);
		while (a!=-1)
		{ bool tempBool = false;
			for (int j=0;j<root::AmbientDimension;j++)
			{ if (this->ComputeRxy(this->LowestNonExplored,a,j))
				{	tempBool =true;
					break;
				}
			}
			assert (tempBool);
			ExploredFromTop.TheObjects[a]=true;
			a= this->FindHighestBruhatNonExplored(ExploredFromTop);
		}
		this->Explored.TheObjects[this->LowestNonExplored]=true;
		this->LowestNonExplored= this->FindLowestBruhatNonExplored();
//		std::string tempS;
//		this->RPolysToString(tempS);
	}
	//this->ComputeDebugString();
}

bool VermaModulesWithMultiplicities::IndexGEQIndex(int a, int b)
{ root tempRoot;
	tempRoot.Assign(this->TheObjects[a]);
	tempRoot.Subtract(this->TheObjects[b]);
	return tempRoot.IsPositiveOrZero();
}

bool VermaModulesWithMultiplicities::IndexGreaterThanIndex(int a, int b)
{ if (a==b) {return false;}
	return this->IndexGEQIndex(a,b);
}

int VermaModulesWithMultiplicities::ComputeProductfromSimpleReflectionsActionList
			(int x,int y)
{ int start = y;
	for (int i=0;i<this->TheWeylGroup->TheObjects[x].size;i++)
	{ start=this->SimpleReflectionsActionList.TheObjects[start].TheObjects
						[this->TheWeylGroup->TheObjects[x].TheObjects[i]];
	}
	return start;
}

void VermaModulesWithMultiplicities::ComputeKLxy(int w, int x)
{ static OneVarIntPolynomial Accum, tempP1,tempP2;
	if (x==w)
	{	this->KLPolys.TheObjects[w].TheObjects[x].MakeConst(1);
		return;
	}
	if (!this->IndexGEQIndex(w,x))
	{ this->KLPolys.TheObjects[w].TheObjects[x].Nullify();
	  return;
	}
	Accum.Nullify();
	this->KLPolys.TheObjects[w].TheObjects[x].Nullify();
	for (int i=0;i<this->size;i++)
	{ if (this->IndexGreaterThanIndex(i,x) &&
				this->IndexGEQIndex(w,i))
		{ int tempI;
			if ((this->TheWeylGroup->length(x)+this->TheWeylGroup->length(i))%2==0)
			{	tempI=1;
			}
			else
			{ tempI=-1;
			}
			int powerQ= -this->TheWeylGroup->length(x)+2*this->TheWeylGroup->length(i)
									-this->TheWeylGroup->length(w);
//			this->RPolys.TheObjects[i].TheObjects[x].ComputeDebugString();
			tempP1.Assign(this->RPolys.TheObjects[i].TheObjects[x]);
//			tempP1.ComputeDebugString();
			tempP1.SubstitutionOneOverX();
//			tempP1.ComputeDebugString();
			tempP2.MakeMonomial(tempI,powerQ);
			tempP1.MultiplyBy(tempP2);
			tempP1.MultiplyBy(this->KLPolys.TheObjects[w].TheObjects[i]);
//			tempP1.ComputeDebugString();
			Accum.AddPolynomial(tempP1);
//			Accum.ComputeDebugString();
		}
	}
//	Accum.ComputeDebugString();
	int lengthDiff= this->TheWeylGroup->length(w)-this->TheWeylGroup->length(x);
	for (int i=0;i<Accum.RationalPart.size;i++)
	{	this->KLPolys.TheObjects[w].TheObjects[x].
				AddMonomial(-Accum.RationalPart.TheObjects[i],-i-1+lengthDiff);
	}
//	this->KLPolys.TheObjects[x].TheObjects[w].ComputeDebugString();
}

bool VermaModulesWithMultiplicities::ComputeRxy(int x, int y, int SimpleReflectionIndex)
{ if (this->IndexGreaterThanIndex(y,x))
	{ this->RPolys.TheObjects[x].TheObjects[y].Nullify();
		return true;
	}
	if (x==y)
	{ this->RPolys.TheObjects[x].TheObjects[y].MakeMonomial(1,0);
		return true;
	}
	int sx= this->SimpleReflectionsActionList.TheObjects[x].TheObjects[SimpleReflectionIndex];
	int sy= this->SimpleReflectionsActionList.TheObjects[y].TheObjects[SimpleReflectionIndex];
	bool boolX, boolY;
	boolX=this->IndexGreaterThanIndex(x,sx);
	boolY=this->IndexGreaterThanIndex(y,sy);
	if (boolX && boolY)
	{ this->RPolys.TheObjects[x].TheObjects[y].Assign
			(this->RPolys.TheObjects[sx].TheObjects[sy]);
		return true;
	}
	if (boolX && !boolY)
	{ static OneVarIntPolynomial q2Minus1, Accum;
		q2Minus1.MakeQuadratic(1,0,-1);
		Accum.MakeQuadratic(1,0,0);
//		this->RPolys.TheObjects[sx].TheObjects[y].ComputeDebugString();
		q2Minus1.MultiplyBy(this->RPolys.TheObjects[x].TheObjects[sy]);
//		this->RPolys.TheObjects[sx].TheObjects[sy].ComputeDebugString();
		Accum.MultiplyBy(this->RPolys.TheObjects[sx].TheObjects[sy]);
		Accum.AddPolynomial(q2Minus1);
		this->RPolys.TheObjects[x].TheObjects[y].Assign(Accum);
		return true;
	}
	return false;
}

void VermaModulesWithMultiplicities::KLPolysToString(std::string &output)
{ std::stringstream out;
	std::string tempS;
	for (int i=0;i<this->KLPolys.size;i++)
	{ this->KLPolys.TheObjects[i].ElementToString(tempS,i);
		out <<tempS<<"\n";
	}
	output = out.str();
}

void VermaModulesWithMultiplicities::RPolysToString(std::string &output)
{ std::stringstream out;
	std::string tempS;
	for (int i=0;i<this->RPolys.size;i++)
	{ this->RPolys.TheObjects[i].ElementToString(tempS,i);
		out <<tempS<<"\n";
	}
	output = out.str();
}

void OneVarPolynomials::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void OneVarPolynomials::ElementToString(std::string& output)
{ std::string tempS;
	std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ tempS.clear();
		this->TheObjects[i].StringPrintOutAppend(tempS,PolyFormatLocal);
		if (tempS!="")
		{	out <<i<<". "<<tempS<<"; ";
		}
	}
	output= out.str();
}

void OneVarPolynomials::ElementToString(std::string& output,int KLindex)
{ std::string tempS;
	std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ tempS.clear();
		this->TheObjects[i].StringPrintOutAppend(tempS,PolyFormatLocal);
		if (tempS!="")
		{	out<<KLindex<<"||"<<i<<" "<<tempS<<"; ";
		}
	}
	output= out.str();
}

void RandomCodeIDontWantToDelete::WeylChamberRandomCode()
{ WeylGroup tempW;
	std::string tempS;
	PolyFormatLocal.MakeAlphabetxi();
	tempW.MakeDn(5);
	tempW.ComputeWeylGroup();
	tempW.ComputeDebugString();
	root tempRoot;
	tempRoot.MakeZero();
	tempRoot.InitFromIntegers(1,2,2,1,2);
	tempRoot.MultiplyByInteger(100);
	tempRoot.Add(tempW.rho);
	PolynomialsRationalCoeff tempP;
	tempP.MakeUsualParametricRoot();
	PolynomialsRationalCoeffCollection output;
	tempW.GenerateOrbitAlg(tempRoot,tempP,output,false,false,0,true);
	output.ComputeDubugString();
	PolynomialsRationalCoeff tempP2;
	tempP2.MakeOneParameterSubFromDirectionIntsAndConstants(1,2,2,1,2,
																													2,3,3,2,2);
	PolyFormatLocal.alphabet[0]="t";
	PolyFormatLocal.alphabet[1]="s";
	output.Substitution(tempP2,1);
	output.ComputeDubugString();
	PolynomialRationalCoeff tempPR;
	ListBasicObjects<Polynomial<Rational> > tempSub1;
	tempSub1.SetSizeExpandOnTopNoObjectInit(1);
	tempSub1.TheObjects[0].MakeNVarDegOnePoly(2,1,RMOne);
	for (int j=0;j<root::AmbientDimension;j++)
	{ tempP2.TheObjects[j].Substitution(tempSub1,2);
	}
	for (int i=0;i<output.size;i++)
	{ for (int j=0;j<root::AmbientDimension;j++)
		{ output.TheObjects[i].TheObjects[j].AddPolynomial(tempP2.TheObjects[j]);
		}
	}
	output.ElementToStringComputeFunctionB(output.DebugString,true);
}

void RandomCodeIDontWantToDelete::KLPolysRandomCode()
{ std::string tempS;
	WeylGroup tempW;
	tempW.MakeAn(3);
	tempW.ComputeWeylGroup();
	tempW.ComputeDebugString();
	VermaModulesWithMultiplicities M;
	M.ComputeKLPolys(& tempW, false);
	M.RPolysToString(tempS);
}

void RandomCodeIDontWantToDelete::SomeRandomTests6()
{ WeylGroup tempW;
	tempW.MakeF4();
	MatrixRational tempMat;
	tempMat.AssignMatrixIntWithDen(tempW.KillingFormMatrix,1);
	tempMat.ComputeDebugString();
	Rational tempRat;
	tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
	tempMat.ComputeDebugString();
}

void RandomCodeIDontWantToDelete::SomeRandomTests5()
{ QuasiNumber tempQN1,tempQN2,tempQN3;
	Rational tempRat(1,2);
	tempQN1.MakePureQN(3,0,ROne,1,1,2);
	tempQN1.ComputeDebugString();
	tempQN2.Assign(tempQN1);
	tempQN2.MultiplyBy(tempQN1);
	tempQN2.ComputeDebugString();
	tempQN3.MakePureQN(3,1,ROne,1,1,2);
	tempQN2.Add(tempQN3);
	tempQN2.ComputeDebugString();
	tempQN2.MultiplyBy(tempQN3);
	tempQN2.ComputeDebugString();
	tempQN2.MultiplyBy(tempQN2);
	tempQN2.ComputeDebugString();

	tempQN2.Add(tempQN1);
	tempQN2.ComputeDebugString();
	tempQN2.MultiplyBy(tempQN2);
	tempQN2.ComputeDebugString();
	MatrixRational theMat;
	intRoot theRoot;
	theMat.init(root::AmbientDimension,2);
	tempQN2.MakeConst(ROne,root::AmbientDimension);
	/*for (int i=0;i<1000;i++)
	{ for (int j=0;j<root::AmbientDimension;j++)
		{ for (int k=0;k<root::AmbientDimension;k++)
			{ theMat.elements[j][k].init(::rand(),2);
			}
			theRoot.elements[j]= ::rand();
		}
		tempQN1.MakeQNFromMatrixAndColumn(theMat,theRoot);
		tempQN2.Add(tempQN1);
//		tempQN2.ComputeDebugString();
		tempQN2.MultiplyBy(tempQN2);
//		tempQN2.ComputeDebugString();
	}
	tempQN2.ComputeDebugString();*/
}

void RandomCodeIDontWantToDelete::SomeRandomTests4()
{ OneVarIntPolynomial tempP1,tempP2;
	tempP1.MakeMonomial(1,-1);
	tempP1.ComputeDebugString();
	tempP2.MakeMonomial(1,1);
	tempP2.ComputeDebugString();
	tempP1.AddPolynomial(tempP2);
	tempP1.ComputeDebugString();
	tempP2.Assign(tempP1);
	for (int i=0;i<20;i++)
	{ tempP1.MultiplyBy(tempP2);
		tempP1.ComputeDebugString();
	}
	tempP1.SubstitutionOneOverX();
	tempP1.ComputeDebugString();
	tempP1.MakeMonomial(1,1);
	tempP1.ComputeDebugString();
	tempP1.SubstitutionOneOverX();
	tempP1.ComputeDebugString();
	tempP1.MakeMonomial(1,0);
	tempP1.ComputeDebugString();
	tempP1.SubstitutionOneOverX();
	tempP1.ComputeDebugString();
	tempP1.MakeMonomial(1,-1);
	tempP1.ComputeDebugString();
	tempP1.SubstitutionOneOverX();
	tempP1.ComputeDebugString();
	tempP2.MakeConst(4);
	tempP2.ComputeDebugString();
	tempP1.MakeQuadratic(1,2,3);
	tempP1.ComputeDebugString();
	tempP2.SubstitutionOneOverX();
	tempP2.ComputeDebugString();
	tempP1.SubstitutionOneOverX();
	tempP1.ComputeDebugString();
	tempP2.MultiplyBy(tempP1);
	tempP2.ComputeDebugString();
	tempP2.SubstitutionOneOverX();
	tempP2.ComputeDebugString();
	tempP1.AddPolynomial(tempP2);
	tempP1.ComputeDebugString();
	tempP1.SubstitutionOneOverX();
////////////////////////////////////////
}

void RandomCodeIDontWantToDelete::SomeRandomTests3()
{	rootFKFTcomputation FKFT;
	root::AmbientDimension=3;
	intRoots theBorel;
	intRoots theNilradical;
	intRoot tempWeight;
	tempWeight.initFromInt(100,10,1,0,0);
	WeylGroup A2;
	A2.MakeAn(2);
	A2.ComputeWeylGroup();
	A2.ComputeDebugString();
	roots tempRoots;
	A2.ComputeRootsOfBorel(tempRoots);
	theBorel.AssignRoots(tempRoots);
	ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=100;
	partFractions theVPfunction;
	std::fstream theDump;
	theDump.open("C:/todor/math/VP.txt",std::fstream::in | std::fstream::out);
	assert(theDump.is_open());
	theBorel.BubbleSort(&tempWeight);
	theVPfunction.initFromRootSystem(	theBorel, theBorel,0);
	theVPfunction.splitClassicalRootSystem();
	theVPfunction.WriteToFile(theDump);
	//theVPfunction.ReadFromFile(theDump);
	theDump.close();
	theVPfunction.ComputeDebugString();
}

void RandomCodeIDontWantToDelete::RevealTheEvilConspiracy()
{ LargeRational EvilValue1, EvilValue2, theEvilSum, theEvilSumInAction;
	this->EvilPoly1.Evaluate(partFraction::theVectorToBePartitioned,EvilValue1);
	this->EvilPoly2.Evaluate(partFraction::theVectorToBePartitioned,EvilValue2);
	std::string EvilString1, EvilString2, EvilString3, EvilString4;
	EvilValue1.ElementToString(EvilString1);
	EvilValue2.ElementToString(EvilString2);
	theEvilSum.Assign(EvilValue1);
	theEvilSum.Add(EvilValue2);
	theEvilSum.ElementToString(EvilString3);
	QuasiPolynomial theEvilPolySum, tempEvilPoly;
	LargeRational TheEvilRational;
	LargeRational theCheckSum;
	//ListBasicObjects<LargeRational> listRat;
	TheEvilRational.MakeZero();
	theCheckSum.MakeZero();
	for (int i=0;i<this->EvilPoly1.size;i++)
	{ tempEvilPoly.Nullify(root::AmbientDimension);
		tempEvilPoly.AddMonomial(this->EvilPoly1.TheObjects[i]);
		tempEvilPoly.Evaluate(partFraction::theVectorToBePartitioned,TheEvilRational);
		TheEvilRational.ElementToString(EvilString4);
		this->EvilList2.AddObjectOnBottom(EvilString4);
		this->theFirstEvilString.append(EvilString4);
		this->theFirstEvilString.append("\n");
		theCheckSum.Add(TheEvilRational);
		//listRat.AddObjectOnTop(theCheckSum);
		theCheckSum.ElementToString(EvilString4);
		this->theSecondEvilString.append(EvilString4);
		this->theSecondEvilString.append("\n");
	}
	this->theFirstEvilString.append("\n\n");
	this->theSecondEvilString.append("\n");
	for (int i=0;i<this->EvilPoly2.size;i++)
	{ tempEvilPoly.Nullify(root::AmbientDimension);
		tempEvilPoly.AddMonomial(this->EvilPoly2.TheObjects[i]);
		tempEvilPoly.Evaluate(partFraction::theVectorToBePartitioned,TheEvilRational);
		TheEvilRational.ElementToString(EvilString4);
		this->EvilList2.AddObjectOnBottom(EvilString4);
		this->theFirstEvilString.append(EvilString4);
		this->theFirstEvilString.append("\n");
		theCheckSum.Add(TheEvilRational);
		//listRat.AddObjectOnTop(theCheckSum);
		theCheckSum.ElementToString(EvilString4);
		this->theSecondEvilString.append(EvilString4);
		this->theSecondEvilString.append("\n");
	}
	tempEvilPoly.Nullify(root::AmbientDimension);
	std::string CheckSumList;
	for (int i=0;i<this->EvilPoly1.size;i++)
	{ tempEvilPoly.AddMonomial(this->EvilPoly1.TheObjects[i]);
		tempEvilPoly.Evaluate(partFraction::theVectorToBePartitioned,TheEvilRational);
		TheEvilRational.ElementToString(EvilString4);
		CheckSumList.append(EvilString4);
		CheckSumList.append("\n");
	}
	CheckSumList.append("\n");
	for (int i=0;i<this->EvilPoly2.size;i++)
	{ if (i==13)
		{	Stop();
			this->EvilPoly2.TheObjects[i].ComputeDebugString(PolyFormatLocal);
			tempEvilPoly.ComputeDebugString();
			QuasiNumber::AnErrorHasOccurredTimeToPanic=true;
		}
		tempEvilPoly.AddMonomial(this->EvilPoly2.TheObjects[i]);
		if (i==13)
		{ tempEvilPoly.ComputeDebugString();
		}
		tempEvilPoly.Evaluate(partFraction::theVectorToBePartitioned,TheEvilRational);
		TheEvilRational.ElementToString(EvilString4);
		CheckSumList.append(EvilString4);
		CheckSumList.append("\n");
	}
	theEvilPolySum.Assign(this->EvilPoly1);
	theEvilPolySum.AddPolynomial(this->EvilPoly2);
	theEvilPolySum.Evaluate(partFraction::theVectorToBePartitioned, theEvilSumInAction);
	theEvilPolySum.Assign(this->EvilPoly2);
	theEvilPolySum.AddPolynomial(this->EvilPoly1);
	theEvilPolySum.Evaluate(partFraction::theVectorToBePartitioned, theEvilSumInAction);
	assert(theEvilSumInAction.IsEqualTo(partFractions::CheckSum));
	assert(theEvilSumInAction.IsEqualTo(theEvilSum));
}

void RandomCodeIDontWantToDelete::SomeRandomTests2()
{	partFraction::TheBigDump.open("C:/pfVP.txt",std::fstream::in | std::fstream::trunc | std::fstream::out);
	assert(partFraction::TheBigDump.is_open());
	partFractions VPtemp;
	QuasiPolynomial tempQP2;
	partFractions::ComputedContributionsList.open
		("C:/File_positions.txt",std::fstream::out | std::fstream::trunc| std::fstream::out);
	VPtemp.ComputeKostantFunctionFromWeylGroup('B',3,tempQP2,0,false,false);
	tempQP2.ComputeDebugString();
	partFractions::ComputedContributionsList.close();
	partFraction::TheBigDump.flush();
	partFractions::ComputedContributionsList.open
		("C:/File_positions.txt",std::fstream::in);
	partFraction::TheBigDump.open("C:/pfVP.txt",std::fstream::in | std::fstream::trunc| std::fstream::out);
	assert(partFraction::TheBigDump.is_open());
	VPtemp.ComputeKostantFunctionFromWeylGroup('B',3,tempQP2,0,true,false);
	tempQP2.ComputeDebugString();
	partFraction::TheBigDump.flush();
	partFraction::TheBigDump.close();
	partFraction::TheBigDump.open("C:/pfVP.txt",std::fstream::in | std::fstream::out);
	std::fstream tempFile;
	tempFile.open("C:/outTemp.txt",std::fstream::in | std::fstream::trunc| std::fstream::out);
	tempQP2.WriteToFile(tempFile);
	tempQP2.ComputeDebugString();
	tempFile.flush();
	tempFile.close();
	tempFile.open("C:/outTemp.txt",std::fstream::in );
	tempQP2.ReadFromFile(tempFile,3);
	tempQP2.ComputeDebugString();
//	RandomCodeIDontWantToDelete::SomeRandomTests5();
	rootFKFTcomputation FKFT;
	root::AmbientDimension=3;
	intRoots theBorel;
	intRoots theNilradical;
	theBorel.SetSizeExpandOnTopNoObjectInit(12);
	theBorel.TheObjects[0].initFromInt(1,0,0,0,0);
	theBorel.TheObjects[1].initFromInt(0,1,0,0,0);
	theBorel.TheObjects[2].initFromInt(0,0,1,0,0);
	theBorel.TheObjects[3].initFromInt(1,1,0,0,0);
	theBorel.TheObjects[4].initFromInt(0,1,2,0,0);
	theBorel.TheObjects[5].initFromInt(0,1,1,0,0);
	theBorel.TheObjects[6].initFromInt(1,1,2,0,0);
	theBorel.TheObjects[7].initFromInt(1,1,1,0,0);
	theBorel.TheObjects[8].initFromInt(1,2,2,0,0);
	theBorel.TheObjects[9].initFromInt(0,0,2,0,0);
	theBorel.TheObjects[10].initFromInt(0,2,2,0,0);
	theBorel.TheObjects[11].initFromInt(2,2,2,0,0);
	theNilradical.CopyFromBase(theBorel);
/*	theNilradical.SetSizeExpandOnTopNoObjectInit(8);
	theNilradical.TheObjects[0].initFromInt(0,1,0,0,0);
	theNilradical.TheObjects[1].initFromInt(0,0,1,0,0);
	theNilradical.TheObjects[2].initFromInt(1,1,0,0,0);
	theNilradical.TheObjects[3].initFromInt(0,1,2,0,0);
	theNilradical.TheObjects[4].initFromInt(0,1,1,0,0);
	theNilradical.TheObjects[5].initFromInt(1,1,2,0,0);
	theNilradical.TheObjects[6].initFromInt(1,1,1,0,0);
	theNilradical.TheObjects[7].initFromInt(1,2,2,0,0);
*/

	intRoot tempWeight;
	tempWeight.initFromInt(100,10,1,0,0);
	WeylGroup B3;
	B3.MakeBn(3);
	B3.ComputeWeylGroup();
	B3.ComputeDebugString();
	VermaModulesWithMultiplicities tempV;
	tempV.initFromWeyl(&B3);
	root beta;
	beta.InitFromIntegers(5,8,9,0,0);
	ListBasicObjects<int> KLcoeff;
	std::fstream KLDump;
	KLDump.open("M:/math/KL.txt",std::fstream::in | std::fstream::out);
	assert(KLDump.is_open());
  tempV.ChamberIndicatorToIndex(beta);
	//tempV.ReadKLCoeffsFromFile(KLDump,KLcoeff);
	tempV.ComputeKLcoefficientsFromChamberIndicator(beta,KLcoeff);
	tempV.KLPolys.ReleaseMemory();
	tempV.RPolys.ReleaseMemory();
	tempV.WriteKLCoeffsToFile(KLDump,KLcoeff,tempV.ChamberIndicatorToIndex(beta));
	KLDump.close();
	ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=100;
	partFractions theVPfunction;
	std::fstream theDump;
	theDump.open("M:/math/VP.txt",std::fstream::in | std::fstream::out);
	assert(theDump.is_open());
	theBorel.BubbleSort(&tempWeight);
	theVPfunction.initFromRootSystem(	theNilradical, theBorel,0);
	theVPfunction.splitClassicalRootSystem();
	theVPfunction.WriteToFile(theDump);
	//theVPfunction.ReadFromFile(theDump);
	theDump.close();
	theVPfunction.ComputeDebugString();

	QuasiPolynomial tempQP;
	beta.MultiplyByInteger(100);
	beta.Add(B3.rho);
	roots tempRoots;
	tempRoots.AssignIntRoots(theNilradical);
	FKFT.MakeTheRootFKFTSum(beta, theVPfunction,KLcoeff,	tempQP,
													 tempV,tempRoots);
	tempQP.ComputeDebugString();
}

void RandomCodeIDontWantToDelete::SomeRandomTestsIWroteMonthsAgo()
{	QuasiNumber p,r,q,s,t,u;
	q.MakeConst(ROne,3);
	p.MakePureQN(3,0,ROne,2,3,3);
	p.ComputeDebugString();
	r.MakePureQN(3,1,ROne,1,1,3);
	r.ComputeDebugString();
	s.MakePureQN(3,2,RMOne,2,1,3);
	s.ComputeDebugString();
	s.Add(p);
	s.ComputeDebugString();
	s.MultiplyBy(r);
	s.ComputeDebugString();
	s.Add(r);
	s.ComputeDebugString();
	r.MakePureQN(1,0,ROne,1,2,3);
	r.ComputeDebugString();
	s.MakePureQN(1,0,ROne,2,2,3);
	s.ComputeDebugString();
	QPSub TheSub;
	TheSub.TheQNSub.init(4,1);
	TheSub.QNSubDen=3;
	TheSub.TheQNSub.elements[0][0]=1;
	TheSub.TheQNSub.elements[1][0]=2;
	TheSub.TheQNSub.elements[2][0]=1;
	TheSub.TheQNSub.elements[3][0]=1;
	TheSub.TheQNSub.elements[0][0]=1;
	r.LinearSubstitution(TheSub);
	r.ComputeDebugString();
	r.MultiplyBy(r);
	r.ComputeDebugString();
	TheSub.TheQNSub.elements[0][0]=1;
	TheSub.TheQNSub.elements[1][0]=0;
	TheSub.TheQNSub.elements[2][0]=2;
	s.LinearSubstitution(TheSub);
	s.ComputeDebugString();
	r.MultiplyBy(s);
	r.ComputeDebugString();
	CompositeComplexQN tempC;
	r.QNtoComplex(tempC);
	tempC.ComputeDebugString();

}

void rootFKFTcomputation::initA2A1A1inD5()
{	initDLL(5);
//	int j=0;
	intRoot tempRoot;
	tempRoot.initFromInt(0,0,1,0,0);//eps_3-\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,1,1,0,0);//\eps_2-\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,1,1,0);//eps_3-\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,1,0,1);//eps_3+\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(1,1,1,0,0);//eps_1-\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,1,1,1,0);//eps_2-\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,1,1,0,1);//eps_2+\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,1,1,1);//eps_3+\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(1,1,1,1,0);//eps_1-\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(1,1,1,0,1);//eps_1+\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,1,1,1,1);//eps_2+\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(1,1,1,1,1);//eps_1+\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,1,2,1,1);//eps_2+\eps_3
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(1,1,2,1,1);//eps_1+\eps_3
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(1,2,2,1,1);//eps_1+\eps_2
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	this->AlgorithmBasisA2A1A1inD5.CopyFromBase(this->nilradicalA2A1A1inD5);
	tempRoot.initFromInt(0,1,0, 0,0);//\eps_2-\eps_3
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,0, 1,0);//\eps_4-\eps_5
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,0, 0,1);//\eps_4+\eps_5
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,2,2, 1,1);//2\eps_2
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,2, 1,1);//2\eps_3
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,0, 1,1);//2\eps_4
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(0,0,0,-1,1);//2\eps_5
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(10000,1000,100,8,10);
	this->AlgorithmBasisA2A1A1inD5.BubbleSort(&tempRoot);
	this->AlgorithmBasisA2A1A1inD5.ComputeDebugString();
}

void rootFKFTcomputation::RunA2A1A1inD5beta12221
        (bool precomputedPartition, bool precomputedKLcoeff,
          std::string& KLCoeffFile, std::string& PartialFractionsFile,
          std::string& VPEntriesFile, std::string& VPIndexFile)
{	this->initA2A1A1inD5();
//	RandomCodeIDontWantToDelete::SomeRandomTests2();
	if (this->useOutputFileForFinalAnswer)
	{ RandomCodeIDontWantToDelete::SomeRandomTestsIWroteMonthsAgo();
		std::fstream tempFile;
		tempFile.open("C:/math/output.txt",std::fstream::in );
		QuasiPolynomial tempQP;
		tempQP.ReadFromFile(tempFile,root::AmbientDimension);
		tempFile.close();
		this->processA2A1A1inD5beta12221Answer(tempQP);
		return;
	}
	std::fstream KLDump;
	std::fstream theDump;
	KLDump.open("C:/math/KL.txt",std::fstream::in | std::fstream::out);
	theDump.open("C:/math/VP.txt",std::fstream::in | std::fstream::out);
	partFraction::TheBigDump.open("C:/math/pfVP.txt",std::fstream::in | std::fstream::out);
	partFractions::ComputedContributionsList.open("C:/math/pfLocations.txt",std::fstream::in | std::fstream::out);
	assert(partFraction::TheBigDump.is_open());
	assert(KLDump.is_open());
	assert(theDump.is_open());
	assert(partFractions::ComputedContributionsList.is_open());
	ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=13000;
	partFractions theVPfunction;
	//theVPfunction.SetSizeExpandOnTopNoObjectInit(8723);
	//assert(theVPfunction.VerifyFileComputedContributions());

	WeylGroup D5;
	D5.MakeDn(5);
//	D5.MakeAn(3);
	intRoots tempRoots1; roots tempRootsRat;
	D5.ComputeWeylGroup();
	D5.ComputeDebugString();
	VermaModulesWithMultiplicities tempV;
	tempV.initFromWeyl(&D5);
	root beta;
	beta.InitFromIntegers(100,200,200,200,100);
	ListBasicObjects<int> KLcoeff;
//	int TopIndex=
  tempV.ChamberIndicatorToIndex(beta);
	if (precomputedKLcoeff)
	{ tempV.ReadKLCoeffsFromFile(KLDump,KLcoeff);
	}
	else
	{ tempV.ComputeKLcoefficientsFromChamberIndicator(beta,KLcoeff);
		tempV.KLPolys.ReleaseMemory();
		tempV.RPolys.ReleaseMemory();
		tempV.WriteKLCoeffsToFile(KLDump,KLcoeff,tempV.ChamberIndicatorToIndex(beta));
	}
	KLDump.close();
//	tempV.ComputeDebugString();
	std::string tempS;
	//tempV.KLcoeffsToString(KLcoeff,tempS);
//	tempV.ComputeDebugString();

//	theVPfunction.initFromRootSystem(tempRoots1,tempRoots1,0);
	if(! precomputedPartition)
	{	theVPfunction.initFromRootSystem(	this->nilradicalA2A1A1inD5,
																		this->AlgorithmBasisA2A1A1inD5, 0);
		theVPfunction.splitClassicalRootSystem();
		theVPfunction.WriteToFile(theDump);
	}
	else
	{	theVPfunction.ReadFromFile(theDump);
	}
	theDump.close();
//	theVPfunction.ComputeDebugString();
	for (int i=0;i<theVPfunction.size;i++)
	{ assert(theVPfunction.TheObjects[i].IndicesNonZeroMults.size==root::AmbientDimension);
	}
	QuasiPolynomial tempQP;
	beta.MultiplyByInteger(100);
	beta.Add(D5.rho);
	roots tempRoots;
	tempRoots.AssignIntRoots(this->nilradicalA2A1A1inD5);
	this->MakeTheRootFKFTSum(beta, theVPfunction,KLcoeff,	tempQP,
													 tempV,tempRoots);
	std::fstream tempFile;
	tempFile.open("C:/math/output.txt",std::fstream::out | std::fstream::trunc);
	tempQP.WriteToFile(tempFile);
	tempFile.close();
	this->processA2A1A1inD5beta12221Answer(tempQP);
}

void rootFKFTcomputation::processA2A1A1inD5beta12221Answer(QuasiPolynomial& theAnswer)
{	theAnswer.ComputeDebugString();
	root beta;
	beta.InitFromIntegers(1,2,2,2,1);
	QPSub theSub;
	this->MakeRootFKFTsub(beta,theSub);
	QuasiPolynomial tempQP2;
	theAnswer.RationalLinearSubstitution(theSub,tempQP2);
	std::fstream tempFile;
	tempFile.open("C:/math/outSub.txt",std::fstream::out | std::fstream::trunc);
	tempQP2.WriteToFile(tempFile);
	tempFile.close();
	tempQP2.ComputeDebugString();
}

void rootFKFTcomputation::MakeRootFKFTsub(root& direction, QPSub& theSub)
{	MatrixInt tempMat;
	tempMat.init(root::AmbientDimension+2,root::AmbientDimension);
	tempMat.NullifyAll();
	int tempLCM=direction.FindLCMDenominators();
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempMat.elements[i][i]=1;
		Rational tempRat;
		tempRat.Assign(direction.coordinates[i]);
		tempRat.MultiplyByInteger(tempLCM);
		tempMat.elements[root::AmbientDimension][i]=tempRat.num;
		tempMat.elements[root::AmbientDimension+1][i]=0;
	}
	theSub.MakeSubFromMatrixIntAndDen(tempMat,tempLCM);
}


void rootFKFTcomputation::MakeTheRootFKFTSum
				(	root& ChamberIndicator, partFractions& theBVdecomposition,
					ListBasicObjects<int>& theKLCoeffs,	QuasiPolynomial& output,
					VermaModulesWithMultiplicities& theHighestWeights,
					roots& theNilradical)
{ PolynomialsRationalCoeffCollection TheChambersInTheGame;
	PolynomialsRationalCoeff StartingRoot;
	::IndicatorWindowGlobalVariables.TotalNumMonomials = theBVdecomposition.NumMonomialsInTheNumerators();
	Cone TheNilradicalCone;
	TheNilradicalCone.ComputeFromDirections(theNilradical);
	TheNilradicalCone.ComputeDebugString();
	StartingRoot.MakeUsualParametricRoot();
	theHighestWeights.TheWeylGroup->GenerateOrbitAlg
			(	ChamberIndicator,StartingRoot,TheChambersInTheGame,true,false,&TheNilradicalCone,true);
	static QuasiPolynomial tempQP1, Accum;
	Accum.Nullify(root::AmbientDimension);
	for (int i=0;i<TheChambersInTheGame.size;i++)
	{	if (theKLCoeffs.TheObjects[i]!=0)
		{	tempQP1.Nullify(root::AmbientDimension);
			theBVdecomposition.partFractionsToPartitionFunctionAdaptedToRoot
				(tempQP1,TheChambersInTheGame.ChamberIndicators.TheObjects[i],false,false,true,true);
//			tempQP1.ComputeDebugString();
			tempQP1.TimesInteger(theKLCoeffs.TheObjects[i]);
			Accum.AddPolynomial(tempQP1);
//			Accum.ComputeDebugString();
		}
	}
	output.CopyFromPoly(Accum);
	output.Simplify();
}

void OneVarIntPolynomial::AddMonomial(int coeff, int power)
{ if (power<0)
	{ power=-power;
		this->SetSizeAtLeastInitProperly(this->RationalPart,power);
		this->RationalPart.TheObjects[power-1]+=coeff;
	}
	else
	{ this->SetSizeAtLeastInitProperly(this->PolynomialPart,power+1);
		this->PolynomialPart.TheObjects[power]+=coeff;
	}
	this->FitSize();
}

void OneVarIntPolynomial::SetSizeAtLeastInitProperly(ListBasicObjects<int> &theArray, int desiredSize)
{ if (theArray.size<desiredSize)
	{ int oldSize=theArray.size;
		theArray.SetSizeExpandOnTopNoObjectInit(desiredSize);
		for (int i= oldSize;i<theArray.size;i++)
		{ theArray.TheObjects[i]=0;
		}
	}
}

void OneVarIntPolynomial::AddPolynomial(OneVarIntPolynomial &p)
{ OneVarIntPolynomial::SetSizeAtLeastInitProperly(this->RationalPart,p.RationalPart.size);
	OneVarIntPolynomial::SetSizeAtLeastInitProperly(this->PolynomialPart,p.PolynomialPart.size);
	for (int i=0;i<p.PolynomialPart.size;i++)
	{ this->PolynomialPart.TheObjects[i]+=p.PolynomialPart.TheObjects[i];
	}
	for (int i=0;i<p.RationalPart.size;i++)
	{ this->RationalPart.TheObjects[i]+=p.RationalPart.TheObjects[i];
	}
	this->FitSize();
}

void OneVarIntPolynomial::MultiplyBy(OneVarIntPolynomial &p)
{ static OneVarIntPolynomial Accum;
	Accum.RationalPart.size=0;
	Accum.PolynomialPart.size=0;
	OneVarIntPolynomial::SetSizeAtLeastInitProperly
		(	Accum.PolynomialPart,this->PolynomialPart.size+p.PolynomialPart.size -1);
	OneVarIntPolynomial::SetSizeAtLeastInitProperly
		(	Accum.RationalPart,this->RationalPart.size+p.RationalPart.size);
	for (int i=0;i<this->PolynomialPart.size;i++)
	{ for(int j=0;j<p.PolynomialPart.size;j++)
		{ Accum.PolynomialPart.TheObjects[i+j]+=this->PolynomialPart.TheObjects[i]*
																						p.PolynomialPart.TheObjects[j];
		}
		for(int j=0;j<p.RationalPart.size;j++)
		{ if (i-j-1<0)
			{	Accum.RationalPart.TheObjects[j-i]+=this->PolynomialPart.TheObjects[i]*
																							p.RationalPart.TheObjects[j];
			}
			else
			{	Accum.PolynomialPart.TheObjects[i-j-1]+=this->PolynomialPart.TheObjects[i]*
																						p.RationalPart.TheObjects[j];
			}
		}
	}
	for (int i=0;i<this->RationalPart.size;i++)
	{ for(int j=0;j<p.RationalPart.size;j++)
		{ Accum.RationalPart.TheObjects[i+j+1]+=this->RationalPart.TheObjects[i]*
																						p.RationalPart.TheObjects[j];
		}
		for(int j=0;j<p.PolynomialPart.size;j++)
		{ if (j-i-1<0)
			{	Accum.RationalPart.TheObjects[i-j]+=this->RationalPart.TheObjects[i]*
																							p.PolynomialPart.TheObjects[j];
			}
			else
			{	Accum.PolynomialPart.TheObjects[j-i-1]+=this->RationalPart.TheObjects[i]*
																						p.PolynomialPart.TheObjects[j];
			}
		}
	}
	Accum.FitSize();
	this->Assign(Accum);
}

void OneVarIntPolynomial::SubstitutionOneOverX()
{ if (this->PolynomialPart.size-1>this->RationalPart.size)
	{	int tempI;
		for(int i=0;i<this->RationalPart.size;i++)
		{	tempI=this->RationalPart.TheObjects[i];
			this->RationalPart.TheObjects[i]=this->PolynomialPart.TheObjects[i+1];
			this->PolynomialPart.TheObjects[i+1]=tempI;
		}
		int oldSize= this->RationalPart.size;
		this->RationalPart.SetSizeExpandOnTopNoObjectInit(this->PolynomialPart.size-1);
		for(int i=oldSize;i<this->PolynomialPart.size-1;i++)
		{ this->RationalPart.TheObjects[i]=this->PolynomialPart.TheObjects[i+1];
		}
		this->PolynomialPart.size=oldSize+1;
	}
	else
	{	int tempI;
		for(int i=1;i<this->PolynomialPart.size;i++)
		{	tempI=this->RationalPart.TheObjects[i-1];
			this->RationalPart.TheObjects[i-1]=this->PolynomialPart.TheObjects[i];
			this->PolynomialPart.TheObjects[i]=tempI;
		}
		int oldSize= this->PolynomialPart.size;
		int start= oldSize;
		this->SetSizeAtLeastInitProperly(this->PolynomialPart,this->RationalPart.size+1);
		if (start==0)
		{	start=1;
			if (this->RationalPart.size!=0)
			{	this->PolynomialPart.TheObjects[0]=0;
			}
		}
		for(int i=start;i<this->RationalPart.size+1;i++)
		{ this->PolynomialPart.TheObjects[i]=this->RationalPart.TheObjects[i-1];
		}
		this->RationalPart.size=oldSize-1;
		if (this->RationalPart.size<0){this->RationalPart.size=0;}
	}
}

void OneVarIntPolynomial::MakeMonomial(int coeff, int power)
{ this->PolynomialPart.size=0;
	this->RationalPart.size=0;
	if (power<0)
	{ OneVarIntPolynomial::SetSizeAtLeastInitProperly(this->RationalPart,-power);
		this->RationalPart.TheObjects[-power-1]=coeff;
	}
	else
	{	OneVarIntPolynomial::SetSizeAtLeastInitProperly(this->PolynomialPart,power+1);
		this->PolynomialPart.TheObjects[power]=coeff;
	}
}

void OneVarIntPolynomial::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void OneVarIntPolynomial::ElementToString(std::string& output)
{ std::stringstream out;
	std::string tempS;
	for (int i=this->RationalPart.size-1;i>=0;i--)
	{	if (this->RationalPart.TheObjects[i]!=0)
		{	std::stringstream out2;
			if (this->RationalPart.TheObjects[i]==-1)
			{ out2<<"-";
			}
			else
			{ if (this->RationalPart.TheObjects[i]!=1)
					out2<<this->RationalPart.TheObjects[i];
			}
			out2<<"q^{"<<-i-1<<"}";
			tempS=out2.str();
			if (tempS[0]!='-')
			{	tempS.insert(0,"+");
			}
			out<< tempS;
		}
	}
	if (this->PolynomialPart.size>0)
	{ if (this->PolynomialPart.TheObjects[0]>0)
		{ out <<"+"<<this->PolynomialPart.TheObjects[0];
		}
		if (this->PolynomialPart.TheObjects[0]<0)
		{ out<<this->PolynomialPart.TheObjects[0];
		}
	}
	for (int i=1;i<this->PolynomialPart.size;i++)
	{	if (this->PolynomialPart.TheObjects[i]!=0)
		{	std::stringstream out2;
			if (this->PolynomialPart.TheObjects[i]==-1)
			{ out2<<"-";
			}
			else
			{ if (this->PolynomialPart.TheObjects[i]!=1)
					out2<<this->PolynomialPart.TheObjects[i];
			}
			if (i!=1)
			{	out2<<"q^{"<<i<<"}"; }else{out2<<"q";}
			tempS=out2.str();
			if (tempS[0]!='-')
			{	tempS.insert(0,"+");
			}
			out<< tempS;
		}
	}
	output= out.str();
	if (output.size()>0)
	{ if (output[0]=='+')
		{ output.erase(0,1);
		}
	}
}

int OneVarIntPolynomial::Substitution(int x)
{ assert(this->RationalPart.size==0);
	int result=0;
	for (int i=0;i<this->PolynomialPart.size;i++)
	{ int accum=1;
		for (int j=0;j<i;j++)
		{ accum*=x;
		}
		result+=accum*PolynomialPart.TheObjects[i];
	}
	return result;
}

void OneVarIntPolynomial::Nullify()
{ this->RationalPart.size=0;
	this->PolynomialPart.size=0;
}

void OneVarIntPolynomial::MakeConst(int c)
{ this->RationalPart.size=0;
	this->PolynomialPart.SetSizeExpandOnTopNoObjectInit(1);
	this->PolynomialPart.TheObjects[0]=c;
}

void OneVarIntPolynomial::MakeQuadratic(int x2Term, int x1Term, int constTerm)
{ this->RationalPart.size=0;
	this->PolynomialPart.SetSizeExpandOnTopNoObjectInit(3);
	this->PolynomialPart.TheObjects[0]= constTerm;
	this->PolynomialPart.TheObjects[1]= x1Term;
	this->PolynomialPart.TheObjects[2]= x2Term;
}

void OneVarIntPolynomial::ReleaseMemory()
{ this->RationalPart.ReleaseMemory();
  this->PolynomialPart.ReleaseMemory();
}

void OneVarIntPolynomial::FitSize()
{ int sizeDecrease=0;
	for (int i=this->PolynomialPart.size-1;i>=0;i--)
	{ if (this->PolynomialPart.TheObjects[i]==0)
		{ sizeDecrease++;
		}
		else
		{ break;
		}
	}
	this->PolynomialPart.size-=sizeDecrease;
	sizeDecrease=0;
	for (int i=this->RationalPart.size-1;i>=0;i--)
	{ if (this->RationalPart.TheObjects[i]==0)
		{ sizeDecrease++;
		}
		else
		{ break;
		}
	}
	this->RationalPart.size-=sizeDecrease;
}

void OneVarIntPolynomials::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void OneVarIntPolynomials::ElementToString(std::string& output)
{ std::string tempS;
	std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ tempS.clear();
		this->TheObjects[i].ElementToString(tempS);
		if (tempS!="")
		{	out <<i<<". "<<tempS<<"; ";
		}
	}
	output= out.str();
}

void OneVarIntPolynomials::ElementToString(std::string& output,int KLindex)
{ std::string tempS;
	std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ tempS.clear();
		this->TheObjects[i].ElementToString(tempS);
		if (tempS!="")
		{	out<<KLindex<<"||"<<i<<" "<<tempS<<"; ";
		}
	}
	output= out.str();
}

void IntegerPoly::Evaluate(root& values, LargeRational& output)
{ std::string tempS1, tempS2;
	output.MakeZero();
	//if(this->AnErrorHasOccurredTimeToPanic)
	//{ output.ElementToString(tempS1);
	//}
	for (int i=0;i<this->size;i++)
	{ LargeRational tempRat1,tempRat2;
		tempRat1.AssignInteger(this->TheObjects[i].Coefficient.value);
		for (int j=0;j<this->NumVars;j++)
		{ tempRat2.AssignRational(values.coordinates[j]);
			tempRat2.RaiseToPower(this->TheObjects[i].degrees[j]);
			tempRat1.MultiplyBy(tempRat2);
			ParallelComputing::SafePoint();
		}
//		if(this->AnErrorHasOccurredTimeToPanic)
//		{ output.ElementToString(tempS2);
//			tempRat1.ElementToString(tempS1);
//			if (i==5)
//			{ //LargeRational::AnErrorHasOccurredTimeToPanic=true;
//			}
//		}
		output.Add(tempRat1);
//		if(this->AnErrorHasOccurredTimeToPanic)
//		{ output.ElementToString(tempS2);
//		}
	}
}

void IntegerPoly::MakePolyExponentFromIntRoot(intRoot&r)
{	this->ClearTheObjects();
	this->NumVars=root::AmbientDimension;
	static Monomial<Integer> tempM;
	tempM.init(root::AmbientDimension);
	tempM.Coefficient.Assign(IOne);
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempM.degrees[i]=(short) r.elements[i];
	}
	this->AddMonomial(tempM);
}

int IntegerPoly::SizeWithoutDebugString()
{ int Accum=0;
	Accum+=this->HashedListBasicObjects<Monomial<Integer> >::SizeWithoutObjects();
	Accum+=	sizeof(this->NumVars);
	for (int i=0;i<this->ActualSize;i++)
	{ Accum+=this->TheActualObjects[i].SizeWithoutCoefficient()+sizeof(Integer);
	}
	return Accum;
}

void LaTeXProcedures::drawline
			(	double X1, double Y1, double X2, double Y2,
				unsigned long thePenStyle, int ColorIndex, std::fstream& output)
{ output.precision(4);
	X1/=50; X2/=50;
	Y1/=50; Y2/=50;
	output	<<"\\psline[linewidth=0.1pt]"
					<<"("<<X1<<","<<Y1<<")"
					<<"("<<X2<<","<<Y2<<")\n";
}

void thePFcomputation::Run()
{ RandomCodeIDontWantToDelete::SomeRandomTests6();
	this->theWeylGroup.MakeEn(6);
	this->theWeylGroup.GenerateRootSystemFromKillingFormMatrix();
	this->theWeylGroup.RootSystem.ComputeDebugString();
	this->ComputeTableAllowed();
	this->ComputeDebugString();
	this->EnumerateRecursively(0,0);
	Stop();
}

void thePFcomputation::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void thePFcomputation::ElementToString(std::string& output)
{ std::stringstream out;
	out<<"tableForbidden:\n";
	for (int i=0;i<this->tableForbidden.size;i++)
	{ out <<i<<"\t\t\t";
		for (int j=0; j<this->tableForbidden.TheObjects[i].CardinalitySelection;j++)
		{ out<<this->tableForbidden.TheObjects[i].elements[j]<<",";
		}
		out<<"\n";
	}
	output= out.str();
}

void thePFcomputation::ComputeTableAllowed()
{ this->tableForbidden.SetSizeExpandOnTopNoObjectInit(this->theWeylGroup.RootSystem.size);
	this->theKillingForm.AssignMatrixIntWithDen(this->theWeylGroup.KillingFormMatrix,1);
	this->theKillingForm.ComputeDebugString();
	this->theForbiddenSelections.SetSizeExpandOnTopNoObjectInit(this->theWeylGroup.RootSystem.size);
	for (int i=0;i<this->theWeylGroup.RootSystem.size;i++)
	{ this->tableForbidden.TheObjects[i].init(this->theWeylGroup.RootSystem.size);
		for (int j=0; j<this->theWeylGroup.RootSystem.size;j++)
		{ Rational tempRat;
			root::RootScalarRoot
				(	this->theWeylGroup.RootSystem.TheObjects[i], this->theWeylGroup.RootSystem.TheObjects[j],
					this->theKillingForm,tempRat);
			if (tempRat.IsGreaterThan(RZero) || tempRat.num==-2)
			{ tableForbidden.TheObjects[i].AddSelection(j);
			}
		}
		this->theForbiddenSelections.TheObjects[i].init
			(this->theWeylGroup.RootSystem.size);
	}
	this->theSelection.init(this->theWeylGroup.RootSystem.size);
	this->LCMdeterminants=1;
	this->TotalNumEnumerated=0;
}

void thePFcomputation::EnumerateRecursively(int depth, int startingIndex)
{ if (depth==root::AmbientDimension)
	{	//this->theSelection.ComputeDebugString();
		this->TotalNumEnumerated++;
		this->ComputeDeterminantSelection();
		return;
	}
	for (int i=startingIndex;i<this->theWeylGroup.RootSystem.size;i++)
	{ if (!this->theForbiddenSelections.TheObjects[depth].selected[i])
		{ this->theSelection.AddSelection(i);
	//		this->theSelection.ComputeDebugString();
			if (depth+1!=root::AmbientDimension)
			{ this->ComputeNewRestrictionsFromOld(depth+1,i);
			}
			this->EnumerateRecursively(depth+1,i+1);
			this->theSelection.RemoveLastSelection();
		}
	}
}

void thePFcomputation::ComputeNewRestrictionsFromOld(int depth, int newIndex)
{ this->theForbiddenSelections.TheObjects[depth].Assign(this->theForbiddenSelections.TheObjects[depth-1]);
//	this->theForbiddenSelections.TheObjects[depth].ComputeDebugString();
	for(int i=0;i<this->tableForbidden.TheObjects[newIndex].CardinalitySelection;i++)
	{	this->theForbiddenSelections.TheObjects[depth].AddSelection
			(this->tableForbidden.TheObjects[newIndex].elements[i]);
	}
}

void thePFcomputation::ComputeDeterminantSelection()
{	static MatrixRational tempMat;
	this->SelectionToMatrixRational(tempMat);
	//tempMat.ComputeDebugString();
	Rational tempRat;
	tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
	if (tempRat.num==0) return;
	if (tempRat.num<0){tempRat.MinusRational();}
	this->LCMdeterminants= lcm(tempRat.num, this->LCMdeterminants);
	//tempMat.ComputeDebugString();
}

void thePFcomputation::SelectionToString(std::string &output)
{ MatrixRational tempMat;
	this->SelectionToMatrixRational(tempMat);
	tempMat.ElementToSting(output);
}

void thePFcomputation::SelectionToMatrixRational(MatrixRational &output)
{	output.init(root::AmbientDimension,root::AmbientDimension);
	for (int i=0;i<root::AmbientDimension;i++)
	{ for (int j=0;j<root::AmbientDimension;j++)
		{ output.elements[i][j].Assign
				(this->theWeylGroup.RootSystem.TheObjects[this->theSelection.elements[i]].coordinates[j]);
		}
	}
}

void rootSubalgebra::ComputeAll()
{ this->initFromAmbientWeyl();
	this->SimpleBasisK.CopyFromBase(this->genK);
	this->TransformToSimpleBasisGenerators(this->SimpleBasisK);
	this->ComputeDebugString();
	this->ComputeKModules();
}

void rootSubalgebra::initFromAmbientWeyl()
{ MatrixRational tempMat;
	tempMat.AssignMatrixIntWithDen(this->AmbientWeyl.KillingFormMatrix,1);
	tempMat.ComputeDebugString();
	this->PosRootsKConnectedComponents.ReleaseMemory();
	this->theKComponentRanks.ReleaseMemory();
	this->theKEnumerations.ReleaseMemory();
	this->AmbientWeyl.GenerateRootSystemFromKillingFormMatrix();
	this->AllRoots.CopyFromBase(this->AmbientWeyl.RootSystem);
	this->AmbientWeyl.ComputeRootsOfBorel(this->AllPositiveRoots);
}

void rootSubalgebra::TransformToSimpleBasisGenerators(roots& theGens)
{ for (int i=0;i>theGens.size;i++)
	{ if (!theGens.TheObjects[i].IsPositiveOrZero())
		{ theGens.TheObjects[i].MinusRoot();
		}
	}
	bool reductionOccured=true;
	while (reductionOccured)
	{ reductionOccured= false;
		for (int i=0;i<theGens.size;i++)
		{ for (int j=i+1; j<theGens.size;j++)
			{ root tempRoot;
				tempRoot.Assign(theGens.TheObjects[i]);
				tempRoot.Subtract(theGens.TheObjects[j]);
				if (tempRoot.IsEqualToZero())
				{	theGens.PopIndexSwapWithLast(j);
					reductionOccured=true;
				}
				if (this->AllRoots.ContainsObject(tempRoot)!=-1)
				{ if (!tempRoot.IsPositiveOrZero()){tempRoot.MinusRoot();}
					theGens.TheObjects[j].Assign(tempRoot);
					reductionOccured=true;
				}
			}
		}
	}
}

void rootSubalgebra::ComputeExtremeWeightInTheSameKMod
			(root& input, root& outputW, bool lookingForHighest)
{ outputW.Assign(input);
	for(bool FoundHigher=true;FoundHigher;)
	{ FoundHigher=false;
		for (int i=0;i<this->SimpleBasisK.size;i++)
		{	root tempRoot;
			tempRoot.Assign(outputW);
			if (lookingForHighest)
			{ tempRoot.Add(this->SimpleBasisK.TheObjects[i]);}
			else
			{ tempRoot.Subtract(this->SimpleBasisK.TheObjects[i]);}
			if (this->AllRoots.ContainsObject(tempRoot)!=-1)
			{ outputW.Assign(tempRoot);
				FoundHigher=true;
			}
			if (tempRoot.IsEqualToZero()) {outputW.MakeZero(); return;}
		}
	}
}

void rootSubalgebra::ComputeHighestWeightInTheSameKMod(root& input, root& outputHW)
{ this->ComputeExtremeWeightInTheSameKMod(input,outputHW,true);
}

void rootSubalgebra::ComputeLowestWeightInTheSameKMod(root& input, root& outputLW)
{ this->ComputeExtremeWeightInTheSameKMod(input,outputLW,false);
}

void rootSubalgebra::ComputeKModules()
{	this->HighestRootsK.size =0;
	this->ComputeDebugString();
	this->ComputeRootsOfK();
	for (int i=0;i<this->AllRoots.size;i++)
	{ if (this->AllRootsK.ContainsObject(this->AllRoots.TheObjects[i])==-1)
		{	root tempLW;
			this->ComputeLowestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempLW);
			int x=this->LowestWeightsGmodK.ContainsObject(tempLW);
			if (x==-1)
			{ this->LowestWeightsGmodK.AddRoot(tempLW);
				x=this->LowestWeightsGmodK.size -1;
				this->kModules.SetSizeExpandOnTopNoObjectInit(this->LowestWeightsGmodK.size);
			}
			this->kModules.TheObjects[x].AddRoot(this->AllRoots.TheObjects[i]);
		}
		else
		{	if (this->AllRoots.TheObjects[i].IsPositiveOrZero())
			{	root tempHW;
				this->ComputeHighestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempHW);
				int x=this->HighestRootsK.ContainsObject(tempHW);
				if (x==-1)
				{ this->HighestRootsK.AddRoot(tempHW);
					x=this->HighestRootsK.size -1;
					this->PosRootsKConnectedComponents.SetSizeExpandOnTopNoObjectInit
						(this->HighestRootsK.size);
				}
				this->PosRootsKConnectedComponents.TheObjects[x].AddRoot
					(this->AllRoots.TheObjects[i]);
			}
		}
	}
}

void rootSubalgebra::ComputeRootsOfK()
{ this->AllRootsK.size=0;
	this->PositiveRootsK.size=0;
	for (int i=0;i<this->AllRoots.size;i++)
	{ root tempHW, tempLW;
		this->ComputeHighestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempHW);
		this->ComputeLowestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempLW);
		if (tempHW.IsEqualToZero() || tempLW.IsEqualToZero())
		{ this->AllRootsK.AddRoot(this->AllRoots.TheObjects[i]);
			if (this->AllRoots.TheObjects[i].IsPositiveOrZero())
			{ this->PositiveRootsK.AddRoot(this->AllRoots.TheObjects[i]);
			}
		}
	}
}

void rootSubalgebra::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void rootSubalgebra::ElementToString(std::string &output)
{ std::stringstream out;
	out <<"Num roots k: "<<this->AllRootsK.size;
	out<<"\nRank k: " <<this->SimpleBasisK.size <<"\nSimple Basis:";
	this->SimpleBasisK.ComputeDebugString();
	out <<this->SimpleBasisK.DebugString;
	out << "Num g mod k modules: "<<this->LowestWeightsGmodK.size << "\nLowest weights: ";
	this->LowestWeightsGmodK.ComputeDebugString();
	out<<this->LowestWeightsGmodK.DebugString;
	output=out.str();
}

void rootSubalgebra::RunE6_3A2()
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(6);
	this->genK.TheObjects[0].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(1,1,1,1,1,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(1,0,1,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,1,1,2,2,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,1,0,1,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE7_D6plusA1()
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE7_A2plusA5()
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[5].InitFromIntegers(1,1,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE7_2A3plusA1()
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE7_A7()
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE7_D4plus3A1()
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[4].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE7_7A1()
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[3].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(1,1,2,2,1,1,1,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_A8()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(1,3,3,5,4,3,2,1);
	this->genK.TheObjects[1].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(true);
}

void rootSubalgebra::RunE8_D8()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[7].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_A1_A7()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(2,3,4,6,5,4,3,2);
	this->genK.TheObjects[1].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[7].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_A5_A1_A2()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(2,3,4,6,5,4,3,1);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_2A4()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(1,1,2,3,3,3,2,1);
	this->genK.TheObjects[5].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,0,0,0,0,1,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(true);
}

void rootSubalgebra::RunE8_4A2()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(1,1,2,3,2,1,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(2,3,4,6,5,4,3,1);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_A2_E6()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(2,3,4,6,5,4,3,1);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->genK.TheObjects[2].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_A1_E7()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(2,3,4,6,5,4,3,1);
	this->genK.TheObjects[1].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,0,1,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_D6_2A1()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(2,3,4,6,5,4,3,2);
	this->genK.TheObjects[7].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_D5_A3()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,1,2,3,3,3,2,1);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,0,1,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_2D4()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[5].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,0,0,0,0,1,1);
	this->genK.TheObjects[7].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_D4_4A1()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[1].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->genK.TheObjects[3].InitFromIntegers(1,1,2,2,1,1,1,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_2A3_2A1()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(1,2,2,3,2,2,2,1);
	this->genK.TheObjects[1].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,0,0,0,0,1);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunE8_8A1()
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(0,1,0,1,1,1,1,0);
	this->genK.TheObjects[1].InitFromIntegers(1,1,2,3,3,2,1,0);
	this->genK.TheObjects[2].InitFromIntegers(1,1,2,2,1,1,1,0);
	this->genK.TheObjects[3].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(0,0,0,0,0,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(2,3,4,6,5,4,3,2);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_B4()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,2,1,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_A3_A1()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,4,2,1,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_A2_A2()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,4,3,1,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_C3_A1()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,1,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,4,3,2,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_D4()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,2,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,2,1,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_B2_2A1()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,2,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,4,3,2,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::RunF4_4A1()
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(2,2,1,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(0,2,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(2,4,3,2,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false);
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration)
{ std::stringstream out2;
	std::stringstream out;
	this->ComputeDebugString();
	out2<<this->DebugString<<"\n";
	MatrixRational tempMat;
	this->SimpleBasisK.rootsToMatrix(tempMat);
	tempMat.Invert();
	int counter=0;
	for(int i=0;i<this->AllRoots.size;i++)
	{ root linComb;
		if (this->AllRootsK.ContainsObject(this->AllRoots.TheObjects[i])==-1)
		{ for (int j=0;j<root::AmbientDimension;j++)
			{	linComb.coordinates[j].MakeZero();
				for(int k=0;k<root::AmbientDimension;k++)
				{ Rational tempRat;
					tempRat.Assign(tempMat.elements[k][j]);
					tempRat.MultiplyBy(this->AllRoots.TheObjects[i].coordinates[k]);
					linComb.coordinates[j].Add(tempRat);
				}
			}
			int x= linComb.FindLCMDenominators();
			linComb.MultiplyByInteger(-x);
			std::string tempS;
			if (this->LinCombToString(this->AllRoots.TheObjects[i], x,linComb,tempS))
			{ out<<tempS<<"\n";
				counter++;
				if (this->LowestWeightsGmodK.ContainsObject(this->AllRoots.TheObjects[i]) !=-1)
				{ out2<< tempS<<"\n";
				}
			}
		}
	}
	out<<"\\multicolumn{2}{|c|}{Number of relations: "<<counter<<" }\\\\\\hline";
	std::string tempS=out.str();
	out2<<"\n\n"<<tempS;
	this->DebugString=out2.str();
	if (DoEnumeration)
	{ this->TestedRootsAlpha.CopyFromBase(this->LowestWeightsGmodK);
		this->DoKRootsEnumeration();
	}
//	this->GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2()
{ std::stringstream out;
	out<<this->DebugString<<"\n\n";
	root tempRoot;
	tempRoot.Assign(this->SimpleBasisK.TheObjects[0]);
	this->ComputeHighestWeightInTheSameKMod(tempRoot,tempRoot);
	for (int l=0;l<this->SimpleBasisK.size;l++)
	{ Rational tempRat;
		this->AmbientWeyl.RootScalarKillingFormMatrixRoot
			(tempRoot,this->SimpleBasisK.TheObjects[l],tempRat);
		if (!tempRat.IsEqualToZero())
		{	int counter=0;
			roots tempRoots;
			tempRoots.CopyFromBase(this->SimpleBasisK);
			tempRoots.TheObjects[l].Assign(tempRoot);
			MatrixRational tempMat;
			tempRoots.rootsToMatrix(tempMat);
			tempMat.Invert();
			for(int i=0;i<this->AllRoots.size;i++)
			{ root linComb;
				if (this->AllRootsK.ContainsObject(this->AllRoots.TheObjects[i])==-1)
				{ for (int j=0;j<root::AmbientDimension;j++)
					{	linComb.coordinates[j].MakeZero();
						for(int k=0;k<root::AmbientDimension;k++)
						{ Rational tempRat;
							tempRat.Assign(tempMat.elements[k][j]);
							tempRat.MultiplyBy(this->AllRoots.TheObjects[i].coordinates[k]);
							linComb.coordinates[j].Add(tempRat);
						}
					}
					int x= linComb.FindLCMDenominators();
					linComb.MultiplyByInteger(-x);
					std::string tempS;
					if (this->LinCombToStringDistinguishedIndex
						(l,this->AllRoots.TheObjects[i], x,linComb,tempS))
					{ out<<tempS<<"\n";
						counter++;
					}
				}
			}
			out<<"\\multicolumn{2}{|c|}{Number of relations: "<<counter<<" }\\\\\\hline";
		}
	}
	this->DebugString=out.str();
}

bool rootSubalgebra::LinCombToString
				(root& alphaRoot, int coeff, root &linComb, std::string &output)
{	if (coeff==1)
		return false;
	std::stringstream out;
	std::string tempS;
	alphaRoot.ElementToString(tempS);
	out<<"("<<tempS<<")&$";
	out<<coeff<<"\\alpha_"<<root::AmbientDimension+1;
	for (int i=0;i<root::AmbientDimension;i++)
	{	//if (linComb.coordinates[i].IsEqualToZero())
		//	return false;
		linComb.coordinates[i].ElementToString(tempS);
		if (tempS!="0")
		{ if (tempS=="-1") {tempS="-";}
			if (tempS=="1") {tempS="+";}
			if (!(tempS[0]=='+')&& !(tempS[0]=='-'))
			{ tempS.insert(0,"+");
			}
			out<<tempS<<"\\beta_"<<i+1;
		}
	}
	out<<"=0$\\\\";
	output=out.str();
	return true;
}

bool rootSubalgebra::LinCombToStringDistinguishedIndex
				(int distinguished,root& alphaRoot, int coeff, root &linComb, std::string &output)
{	if (coeff==1)
		return false;
	std::stringstream out;
	std::string tempS;
	alphaRoot.ElementToString(tempS);
	out<<"("<<tempS<<")&$";
	out<<coeff<<"\\alpha_"<<root::AmbientDimension+1;
	for (int i=0;i<root::AmbientDimension;i++)
	{	//if (linComb.coordinates[i].IsEqualToZero())
		//	return false;
		linComb.coordinates[i].ElementToString(tempS);
		if (tempS!="0")
		{ if (tempS=="-1") {tempS="-";}
			if (tempS=="1") {tempS="+";}
			if (!(tempS[0]=='+')&& !(tempS[0]=='-'))
			{ tempS.insert(0,"+");
			}
			out <<tempS;
			if (i!=distinguished)
			{ out<<"\\beta_"<<i+1;
			}
			else
			{ out<<"\\gamma";
			}
		}
	}
	out<<"=0$\\\\";
	output=out.str();
	return true;
}

void rootSubalgebra::DoKRootsEnumeration()
{	this->theKEnumerations.SetSizeExpandOnTopNoObjectInit
		(this->PosRootsKConnectedComponents.size);
	this->theKComponentRanks.SetSizeExpandOnTopNoObjectInit
		(this->PosRootsKConnectedComponents.size);
	for (int i=0;i<this->PosRootsKConnectedComponents.size;i++)
	{ this->theKEnumerations.TheObjects[i].init(this->PosRootsKConnectedComponents.TheObjects[i].size);
		this->PosRootsKConnectedComponents.TheObjects[i].ComputeDebugString();
		this->theKComponentRanks.TheObjects[i]=
			this->PosRootsKConnectedComponents.TheObjects[i].GetRankOfSpanOfElements();
	}
	this->DoKRootsEnumerationRecursively(0);
}

void rootSubalgebra::DoKRootsEnumerationRecursively(int indexEnumeration)
{ int theRank=this->theKComponentRanks.TheObjects[indexEnumeration];
	int numRuns=NChooseK
								(	this->PosRootsKConnectedComponents.TheObjects[indexEnumeration].size,
									theRank);
	this->theKEnumerations.TheObjects[indexEnumeration].initNoMemoryAllocation();
	for (int i=0;i<numRuns;i++)
	{ this->theKEnumerations.TheObjects[indexEnumeration]
			.incrementSelectionFixedCardinality(theRank);
		if (indexEnumeration<this->PosRootsKConnectedComponents.size-1)
		{ this->DoKRootsEnumerationRecursively(indexEnumeration+1);
		}
		else
		{ this->KEnumerationsToLinComb();
		}
	}
}

void rootSubalgebra::KEnumerationsToLinComb()
{ static MatrixRational tempMat;
	static Selection tempSelection;
	tempMat.init(root::AmbientDimension,root::AmbientDimension);
	int counter=0;
	for (int i=0;i<this->PosRootsKConnectedComponents.size;i++)
	{ this->PosRootsKConnectedComponents.TheObjects[i].SelectionToMatrixAppend
			(this->theKEnumerations.TheObjects[i],tempMat,counter);
		counter+=this->theKComponentRanks.TheObjects[i];
	}
	//tempMat.ComputeDebugString();
	if (tempMat.Invert())
	{ //tempMat.ComputeDebugString();
		for (int l=0;l<this->TestedRootsAlpha.size;l++)
		{	root linComb;
			root& TestedRootAlpha= this->TestedRootsAlpha.TheObjects[l];
			for (int j=0;j<root::AmbientDimension;j++)
			{	linComb.coordinates[j].MakeZero();
				for(int k=0;k<root::AmbientDimension;k++)
				{ Rational tempRat;
					tempRat.Assign(tempMat.elements[k][j]);
					tempRat.MultiplyBy(TestedRootAlpha.coordinates[k]);
					linComb.coordinates[j].Add(tempRat);
				}
			}
			int x= linComb.FindLCMDenominators();
			linComb.MultiplyByInteger(-x);
			bool foundBadCombination=true;
			for (int i=0;i<root::AmbientDimension;i++)
			{ if (linComb.coordinates[i].num==-1 || linComb.coordinates[i].num==1)
				{ foundBadCombination=false;
					break;
				}
			}
			if (foundBadCombination)
			{	std::string tempS;
				this->LinCombToString(TestedRootAlpha,x,linComb,tempS);
				Stop();
			}
		}
	}
}


void PolynomialLargeRational::Evaluate(intRoot &values, LargeRational &output)
{ output.MakeZero();
	std::string tempS;
	LargeRational tempLRat, tempLRat2;
	for (int i=0;i<this->size;i++)
	{ tempLRat.Assign(this->TheObjects[i].Coefficient);
		for (int j=0; j<this->NumVars;j++)
		{ for (int k=0;k<this->TheObjects[i].degrees[j];k++)
			{ tempLRat.MultiplyByInt(values.elements[j]);
				ParallelComputing::SafePoint();
			}
		}
		output.Add(tempLRat);
	}
}

void IndicatorWindowVariables::PrepareStrings()
{ std::stringstream out1,out2,out3;
	out1<<this->NumReducedPartFractions<<" reduced fractions out of "<<this->NumPartFractionsLoaded;
	this->ProgressReportString1= out1.str();
	out2<< this->NumComputedContributions<<" contributions out of "
			<< this->NumProcessedFractions <<" processed fractions";
	this->ProgressReportString2= out2.str();
	out3<<"Processed "<< this->NumProcessedMonomials<<" out of "<<this->TotalNumMonomials<<" monomials";
	this->ProgressReportString3= out3.str();
	this->ProgressReportString4.assign(this->StatusString);
}

void IndicatorWindowVariables::Assign(IndicatorWindowVariables& right)
{ this->ProgressReportString1=right.ProgressReportString1;
	this->ProgressReportString2= right.ProgressReportString2;
	this->ProgressReportString3= right.ProgressReportString3;
	this->ProgressReportString4= right.ProgressReportString4;
	this->StatusString= right.StatusString;
	this->rootIsModified= right.rootIsModified;
	this->PerturbationHasOccurred= right.PerturbationHasOccurred;
	this->modifiedRoot=right.modifiedRoot;
}

void simplicialCones::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void simplicialCones::ElementToString(std::string& output)
{ std::stringstream out;
	for (int i=0; i<this->size;i++)
	{ this->TheObjects[i].ComputeDebugString();
		out << this->TheObjects[i].DebugString <<"\n";
	}
	output= out.str();
}

void simplicialCones::initFromDirections(roots &directions)
{ Selection tempSel;
	assert(directions.size>0);
	tempSel.init(directions.size);
	roots tempRoots;
	this->size=0;
	this->ConesHavingFixedNormal.size=0;
	this->theFacets.ClearTheObjects();
	int maxSize=::NChooseK(directions.size,root::AmbientDimension);
	int maxNumFacets= 2*::NChooseK(directions.size, root::AmbientDimension-1);
	this->SetActualSizeAtLeastExpandOnTop(maxSize);
	this->theFacets.SetActualSizeAtLeastExpandOnTop(maxNumFacets);
	this->ConesHavingFixedNormal.SetActualSizeAtLeastExpandOnTop(maxNumFacets);
	for (int i=0;i<maxSize;i++)
	{ tempSel.incrementSelectionFixedCardinality(root::AmbientDimension);
		tempRoots.size=0;
		for (int j=0;j<root::AmbientDimension;j++)
		{ tempRoots.AddRoot(directions.TheObjects[tempSel.elements[j]]);
		}
		if (tempRoots.GetRankOfSpanOfElements()==root::AmbientDimension)
		{	this->SetSizeExpandOnTopNoObjectInit(this->size+1);
			this->TheObjects[this->size-1].ComputeFromDirections(tempRoots);
			for (int j=0;j<root::AmbientDimension;j++)
			{	root tempRoot;
				int tempI= this->size-1;
				tempRoot.Assign(this->TheObjects[tempI].TheObjects[j]);
				tempRoot.ScaleToFirstNonZeroCoordinatePositive();
				int x=this->theFacets.ContainsObjectHash(tempRoot);
				if (x!=-1)
				{ this->ConesHavingFixedNormal.TheObjects[x].AddObjectOnTop(tempI);
				}
				else
				{ this->theFacets.AddObjectOnTopHash(tempRoot);
					this->ConesHavingFixedNormal.SetSizeExpandOnTopNoObjectInit
						(this->ConesHavingFixedNormal.size+1);
					this->ConesHavingFixedNormal.TheObjects[this->ConesHavingFixedNormal.size-1].size=0;
					this->ConesHavingFixedNormal.TheObjects[this->ConesHavingFixedNormal.size-1]
						.AddObjectOnBottom(tempI);
				}
			}
		}
	}
}

bool simplicialCones::SeparatePoints(root &point1, root &point2, root *PreferredNormal)
{ if (PreferredNormal==0)
	{ for (int i=0;this->size;i++)
		{ if (this->TheObjects[i].SeparatesPoints(point1,point2))
				return true;
		}
	}
	else
	{ root tempRoot;
		tempRoot.Assign(*PreferredNormal);
		tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
		int x= this->theFacets.ContainsObjectHash(tempRoot);
		assert(x!=-1);
		int theSize=this->ConesHavingFixedNormal.TheObjects[x].size;
		for (int i=0;i<theSize;i++)
		{ int y= this->ConesHavingFixedNormal.TheObjects[x].TheObjects[i];
			if (this->TheObjects[y].SeparatesPoints(point1,point2))
				return true;
		}
	}
	return false;
}
