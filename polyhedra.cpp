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
//decide to pass on those modifications).
//The code is licensed under the Library General Public License version 3.0
//(summarized in the preceding sentence).
//You should have received a copy of the GNU Library General Public License
//along with this program.
//If not, see <http://www.gnu.org/licenses/>.
//
//Todor Milev would like to thank http://www.cplusplus.com/forum/ for the valuable
//advice and help with C++. Special thanks to helios, Disch, Grey Wolf, jsmith,
//Hammurabi and Duoas!
//*********************************************************************************************************
//*********************************************************************************************************
//DISCLAIMER
//
//THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.
//EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
//PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND,
//EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.
//SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
//REPAIR OR CORRECTION.
//*********************************************************************************************************
//*********************************************************************************************************

#include "polyhedra.h"

void outputTextDummy(std::string theOutput)
{}

void FeedDataToIndicatorWindowDummy(IndicatorWindowVariables& output)
{}

void drawlineDummy(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{}

void drawtextDummy(double X1, double Y1, const char* text, int length, int color)
{}
//taken from windows.h
#ifndef RGB
unsigned int RGB(int r, int g, int b)
{ return r | (g<<8) | b<<16;
}
#endif
//end of windows.h portion

GlobalVariables::GlobalVariables()
{
}

GlobalVariables::~GlobalVariables()
{
}


void GlobalVariables::operator =(const GlobalVariables &G_V)
{ if(this==&G_V)
		return;
	this->rootsWallBasis.CopyFromBase(G_V.rootsWallBasis);
	this->matTransposeBuffer.Assign(G_V.matTransposeBuffer);
}

int ParallelComputing::GlobalPointerCounter=0;
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

Rational& LROne=Rational::TheRingUnit;
Rational& LRMOne=Rational::TheRingMUnit;
Rational& LRZero=Rational::TheRingZero;

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

int CombinatorialChamber::MethodUsed;
simplicialCones CombinatorialChamberContainer::startingCones;
bool CombinatorialChamber::flagPrintWallDetails=true;
bool CombinatorialChamber::DisplayingGraphics=true;
bool CombinatorialChamberContainer::PrintLastChamberOnly=true;
bool CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic=false;

bool CombinatorialChamber::flagIncludeVerticesInDebugString=true;
bool CombinatorialChamber::flagAnErrorHasOccurredTimeToPanic=false;
bool CombinatorialChamberContainer::flagMakingConsistencyCheck=true;
int CombinatorialChamberContainer::NumTotalCreatedCombinatorialChambersAtLastDefrag;
int CombinatorialChamberContainer::LastReportedMemoryUse;
int CombinatorialChamberContainer::DefragSpacing;
int CombinatorialChamberContainer::flagMaxNumCharsAllowedInStringOutput=100000;//16777216;
std::fstream CombinatorialChamberContainer::TheBigDump;

ListObjectPointers<QuasiPolynomial> QuasiPolynomial::GlobalCollectorsPolys;
ListObjectPointers<ComplexQN> ComplexQN::GlobalCollectorsComplexQNs;
//CombinatorialChamberContainer GlobalCollectorChambers;
//FacetPointers GlobalCollectorFacets;

intRoot partFraction::theVectorToBePartitioned;
bool partFraction::MakingConsistencyCheck=false;
bool partFraction::UncoveringBrackets=true;
Rational partFractions::CheckSum;
Rational partFraction::CheckSum;
Rational partFraction::CheckSum2;

PolynomialsRationalCoeff PrecomputedPolys;
PrecomputedQuasiPolynomialIntegrals PrecomputedQuasiPolys;
PolynomialOutputFormat PolyFormatLocal;
int PolynomialOutputFormat::LatexMaxLineLength=125;
int PolynomialOutputFormat::LatexCutOffLine=15;
bool PolynomialOutputFormat::UsingLatexFormat=true;
bool PolynomialOutputFormat::CarriageReturnRegular=true;
PolynomialOutputFormat PolyFormatNumFrac;
IndicatorWindowVariables IndicatorWindowGlobalVariables;
int QuasiPolynomial::TotalCreatedPolys=0;
int ComplexQN::NumTotalCreated=0;

CyclotomicList CompositeComplex::PrecomputedCyclotomic;
ListObjectPointers<BasicQN> BasicQN::GlobalCollectorsBasicQuasiNumbers;
int BasicQN::NumTotalCreated=0;
template < > bool Matrix<Rational>::flagComputingDebugInfo=true;
template < > bool Polynomial<Integer>::flagAnErrorHasOccuredTimeToPanic=true;
template < > bool Polynomial<Rational>::flagAnErrorHasOccuredTimeToPanic=true;

template < > int HashedListBasicObjects<Monomial<CompositeComplexQN> >::PreferredHashSize=1000;
template < > int HashedListBasicObjects<BasicQN>::PreferredHashSize= 1;
template < > int HashedListBasicObjects<partFraction>::PreferredHashSize=10000;
template < > int HashedListBasicObjects<intRoot>::PreferredHashSize = 10000;
template < > int HashedListBasicObjects<oneFracWithMultiplicitiesAndElongations>::PreferredHashSize= 10;
template < > int HashedListBasicObjects<ElementWeylGroup>::PreferredHashSize=10000;
template < > int HashedListBasicObjects<root>::PreferredHashSize=1000;
template < > int HashedListBasicObjects<Monomial<LargeInt> >::PreferredHashSize=1;
template < > int HashedListBasicObjects<Monomial<Rational> >::PreferredHashSize=10000;
template < > int HashedListBasicObjects<Monomial<Integer> >::PreferredHashSize=100;
template < > int HashedListBasicObjects<Monomial<QuasiNumber> >::PreferredHashSize=1000;
template < > int HashedListBasicObjects
		<MonomialInCommutativeAlgebra<Integer, GeneratorsPartialFractionAlgebra, GeneratorPFAlgebraRecord> >
  ::PreferredHashSize=100;
template < > int HashedListBasicObjects<GeneratorPFAlgebraRecord>::PreferredHashSize = 1000;
template < > int HashedListBasicObjects<GeneratorsPartialFractionAlgebra>::PreferredHashSize=100;
template < > int HashedListBasicObjects<Selection>::PreferredHashSize=20;
template < > int HashedListBasicObjects<affineHyperplane>::PreferredHashSize=100;

template < > int ListBasicObjects<affineCone>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<CombinatorialChamber*>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<ComplexQN>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<int>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<CompositeComplexQN> >::ListBasicObjectsActualSizeIncrement=10;
template < > int ListBasicObjects<BasicQN *>::ListBasicObjectsActualSizeIncrement=1000;
template < > int ListBasicObjects<ComplexQN *>::ListBasicObjectsActualSizeIncrement=10000;
template < > int ListBasicObjects<QuasiPolynomial *>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<PrecomputedTaukn *>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<BasicComplexNumber>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<MonomialInCommutativeAlgebra
																	<	Integer,GeneratorsPartialFractionAlgebra,
																		GeneratorPFAlgebraRecord> >
	::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<GeneratorsPartialFractionAlgebra>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<GeneratorPFAlgebraRecord>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<partFraction>::ListBasicObjectsActualSizeIncrement=1000;
template < > int ListBasicObjects<BasicQN>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<QuasiNumber> >::ListBasicObjectsActualSizeIncrement=1000;
template < > int ListBasicObjects<intRoot>::ListBasicObjectsActualSizeIncrement= 20;
template < > int ListBasicObjects<oneFracWithMultiplicitiesAndElongations>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<ElementWeylGroup>::ListBasicObjectsActualSizeIncrement= 1000;
template < > int ListBasicObjects<PolynomialsRationalCoeff>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<root>::ListBasicObjectsActualSizeIncrement=10;
template < > int ListBasicObjects<QuasiPolynomial>::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<PolynomialRationalCoeff>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<LargeInt> >::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Polynomial<Rational> >::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<Monomial<Integer> >::ListBasicObjectsActualSizeIncrement=10;
template < > int ListBasicObjects<Monomial<Rational> >::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<rootWithMultiplicity>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<std::string>::ListBasicObjectsActualSizeIncrement=100;
template < > int ListBasicObjects<unsigned int>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<roots>::ListBasicObjectsActualSizeIncrement=5;
template < > int ListBasicObjects<Selection>::ListBasicObjectsActualSizeIncrement=5;
template < > int ListBasicObjects<affineHyperplane>::ListBasicObjectsActualSizeIncrement=1000;
template < > int ListBasicObjects<WallData*>::ListBasicObjectsActualSizeIncrement=1;
template < > int ListBasicObjects<WallData>::ListBasicObjectsActualSizeIncrement=6;
template < > int ListBasicObjects<rootsWithMultiplicity>::ListBasicObjectsActualSizeIncrement=10;
template < > int ListBasicObjects<Rational>::ListBasicObjectsActualSizeIncrement=10;
template < > int ListBasicObjects<coneRelation>::ListBasicObjectsActualSizeIncrement=1000;
template < > int ListBasicObjects<DynkinDiagramRootSubalgebra>::ListBasicObjectsActualSizeIncrement=100;


ListBasicObjects<std::string> RandomCodeIDontWantToDelete::EvilList1;
ListBasicObjects<std::string> RandomCodeIDontWantToDelete::EvilList2;
bool RandomCodeIDontWantToDelete::UsingEvilList1=true;
std::string RandomCodeIDontWantToDelete::theFirstEvilString;
std::string RandomCodeIDontWantToDelete::theSecondEvilString;

std::fstream partFraction::TheBigDump;
std::fstream partFractions::ComputedContributionsList;
template < > std::string Matrix<Rational>::MatrixElementSeparator= ",";

//template < > int ListObjectPointers<partFraction>::MemoryAllocationIncrement=100;
ListObjectPointers<partFraction> partFraction::GlobalCollectorPartFraction;
bool partFraction::UseGlobalCollector=true;
bool partFraction::flagAnErrorHasOccurredTimeToPanic=false;
bool partFractions::flagSplitTestModeNoNumerators=false;
bool partFractions::flagAnErrorHasOccurredTimeToPanic=false;
bool partFractions::flagUsingCheckSum=true;
int partFractions::NumMonomialsInNumeratorsRelevantFractions=0;
int partFractions::NumProcessedForVPFMonomialsTotal=0;
int partFractions::flagMaxNumStringOutputLines=500;
//int partFraction::lastApplicationOfSVformulaNumNewGenerators=0;
//int partFraction::lastApplicationOfSVformulaNumNewMonomials=0;
bool QuasiPolynomial::flagAnErrorHasOccurredTimeToPanic=false;
bool Rational::flagAnErrorHasOccurredTimeToPanic=false;
bool Rational::flagMinorRoutinesOnDontUseFullPrecision=false;
bool partFractions::flagMakingProgressReport=true;
bool partFractions::flagUsingIndicatorRoot=true;
HashedListBasicObjects<GeneratorPFAlgebraRecord> GeneratorsPartialFractionAlgebra::theGenerators;
bool WeylGroup::flagAnErrorHasOcurredTimeToPanic=false;
bool WallData::flagDisplayWallDetails=true;
int RankGlobal;
//FacetPointers TheBigFacetOutput;
DrawingVariables TDV(200,400);
bool QuasiNumber::flagAnErrorHasOccurredTimeToPanic=false;
bool IntegerPoly::flagAnErrorHasOccurredTimeToPanic=false;
root oneFracWithMultiplicitiesAndElongations::CheckSumRoot;
bool ParallelComputing::SafePointReached=false;
bool ParallelComputing::ReachSafePointASAP=false;
FeedDataToIndicatorWindow GlobalVariables::FeedDataToIndicatorWindowDefault=0;
bool MatrixLargeRational::flagAnErrorHasOccurredTimeToPanic=false;

#ifndef WIN32
pthread_mutex_t ParallelComputing::mutex1;
pthread_cond_t ParallelComputing::continueCondition;
#endif

bool Stop()
{	return true;
}

void CombinatorialChamberContainer::OneSlice
		(	roots& directions, int& index, int rank, root& IndicatorRoot,
			GlobalVariables& theGlobalVariables)
{	if (index==rank-1)
	{	this->MakeStartingChambers(directions, IndicatorRoot, theGlobalVariables);
		index++;
		this->ComputeNextIndexToSlice(directions.TheObjects[index]);
	}
	else
	{	if (index<directions.size)
		{	if(this->indexNextChamberToSlice!=-1)
			{	if (this->TheObjects[this->indexNextChamberToSlice]->SliceInDirection
							(	directions.TheObjects[index],directions,index,*this,
								this->theHyperplanes, theGlobalVariables))
				{	delete this->TheObjects[this->indexNextChamberToSlice];
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
					this->TheObjects[this->indexNextChamberToSlice]=0;
					//if (this->flagAnErrorHasOcurredTimeToPanic)
					//{	this->ComputeDebugString();
					//	this->ConsistencyCheck();
					//}
				}
			}
			else
			{	this->PreferredNextChambers.size=0;
				this->PurgeZeroPointers();
				index++;
				this->LabelAllUnexplored();
			}
			if (index<directions.size)
				this->ComputeNextIndexToSlice(directions.TheObjects[index]);
		}
	}
}

void CombinatorialChamberContainer::SortIndicesByDisplayNumber
	(ListBasicObjects<int>& outputSortedIndices)
{	outputSortedIndices.MakeActualSizeAtLeastExpandOnTop(this->size);
	for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]!=0)
			if (!this->TheObjects[i]->flagHasZeroPolynomial)
				outputSortedIndices.AddObjectOnTop(i);
	this->QuickSortIndicesByDisplayNumber(outputSortedIndices,0,outputSortedIndices.size-1);
}

void CombinatorialChamberContainer::QuickSortIndicesByDisplayNumber
	(ListBasicObjects<int>& outputSortedIndices, int BottomIndex, int TopIndex)
{	if (TopIndex<=BottomIndex)
		return;
	int HighIndex=TopIndex;
	for (int i=BottomIndex+1;i<=HighIndex;i++)
	{ if (	this->TheObjects[outputSortedIndices.TheObjects[i]]->DisplayNumber>
					this->TheObjects[outputSortedIndices.TheObjects[BottomIndex]]->DisplayNumber)
		{	for (;HighIndex>i &&
						this->TheObjects[outputSortedIndices.TheObjects[HighIndex]]->DisplayNumber>
						this->TheObjects[outputSortedIndices.TheObjects[BottomIndex]]->DisplayNumber;
						HighIndex--)
			{}
			int tempI= outputSortedIndices.TheObjects[i];
			outputSortedIndices.TheObjects[i]= outputSortedIndices.TheObjects[HighIndex];
			outputSortedIndices.TheObjects[HighIndex]=tempI;
		}
	}
	if (this->TheObjects[outputSortedIndices.TheObjects[HighIndex]]->DisplayNumber>
			this->TheObjects[outputSortedIndices.TheObjects[BottomIndex]]->DisplayNumber)
		HighIndex--;
	//swap HighIndex and Bottom Index:
	int tempI= outputSortedIndices.TheObjects[BottomIndex];
	outputSortedIndices.TheObjects[BottomIndex]= outputSortedIndices.TheObjects[HighIndex];
	outputSortedIndices.TheObjects[HighIndex]=tempI;
	this->QuickSortIndicesByDisplayNumber(outputSortedIndices, BottomIndex,HighIndex-1);
	this->QuickSortIndicesByDisplayNumber(outputSortedIndices, HighIndex+1,TopIndex);
}

void CombinatorialChamberContainer::PurgeZeroPointers()
{ int ActualIndex=0;
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[ActualIndex]= this->TheObjects[i];
		if(this->TheObjects[i]!=0)
		{	this->TheObjects[ActualIndex]->IndexInOwnerComplex=ActualIndex;
			ActualIndex++;
		}
	}
	this->size= ActualIndex;
}

void CombinatorialChamberContainer::SliceOneDirection
	(roots& directions, int& index, int rank, root& IndicatorRoot, GlobalVariables& theGlobalVariables)
{	if (index<directions.size)
	{	int oldindex= index;
//		int counter=0;
		while(oldindex==index)
		{ //counter++;
//			if (counter==8)
//			{CombinatorialChamberContainer::AnErrorHasOcurredTimeToPanic=true;
//			}
			this->OneSlice(directions,index,rank,IndicatorRoot, theGlobalVariables);
		}
	}
}

void CombinatorialChamberContainer::SliceTheEuclideanSpace
	(roots& directions,int &index, int rank, root& IndicatorRoot, GlobalVariables& theGlobalVariables)
{ if (directions.size==0)
		return;
	if (directions.TheObjects[0].size==1)
		return;
	while(index<directions.size)
		SliceOneDirection(directions,index,rank,IndicatorRoot, theGlobalVariables);
	this->ConsistencyCheck();
}

int DrawingVariables::GetColorFromChamberIndex(int index, std::fstream* LaTexOutput)
{ int tempI=index%this->NumColors;
	if (tempI<0) tempI+=this->NumColors;
	if (LaTexOutput==0)
		return this->Colors[tempI];
	return tempI;
}

void DrawingVariables::initDrawingVariables(int cX1, int cY1)
{ this->flagLaTeXDraw= false;
	this->ColorDashes=RGB(200,200,200);
	this->flag2DprojectionDraw=true;
	this->ColorChamberIndicator=RGB(220,220,0);
	this->ColorWeylChamberWalls=RGB(220,220,0);
	this->DeadChamberTextColor= RGB(200,100,100);
	this->ZeroChamberTextColor= RGB(220,120,120);
	this->TextColor= RGB(0,0,0);
	this->DrawStyleDashes= 1;
	this->DrawStyleDashesInvisibles= 1;
	this->DrawChamberIndices= true;
	this->DisplayingChamberCreationNumbers =false;
	this->DrawDashes=true;
	this->DrawStyleInvisibles=2;
	this->DrawStyle= 0;
	this->TextOutStyle= 0;
	this->Selected=-2;
	this->textX=0;
	this->textY=15;
	this->centerX= (double) cX1;//(double(X2)-double(X1));
	this->centerY= (double) cY1;
	this->scale=1;
//	Projections[0][0]=(0+50)*tempI;
//	Projections[0][1]=(0-50)*tempI;
//	Projections[1][0]=(50+50)*tempI;
//	Projections[1][1]=(87-50)*tempI;
//	Projections[2][0]=(100+50)*tempI;
//	Projections[2][1]=(0-50)*tempI;
	Projections[0][0]=100;
	Projections[0][1]=0;
	Projections[1][0]=-50;
	Projections[1][1]=86.66;
//	this->Projections[0][0]=200;
//	this->Projections[0][1]=-200;
//	this->Projections[1][0]=200;
//	this->Projections[1][1]=200;
	this->Projections[2][0]=0;
	this->Projections[2][1]=100;
	this->Projections[3][0]=92;
	this->Projections[3][1]=-29;
	this->Projections[4][0]=88;
	this->Projections[4][1]=-49;
	this->Projections[5][0]=50;
	this->Projections[5][1]=-88;
	this->Projections[6][0]=28;
	this->Projections[6][1]=-92;
	this->ApplyScale(1);
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

void DrawingVariables::SetCoordsForA2()
{	this->scale=1;
	Projections[0][0]=100;
	Projections[0][1]=0;
	Projections[1][0]=-50;
	Projections[1][1]=86.66;
}

void DrawingVariables::SetCoordsForB2()
{	this->scale=1;
	Projections[0][0]=120;
	Projections[0][1]=0;
	Projections[1][0]=-60;
	Projections[1][1]=60;
}

void DrawingVariables::SetCoordsForC2()
{	this->scale=1;
	Projections[0][0]=100;
	Projections[0][1]=0;
	Projections[1][0]=-100;
	Projections[1][1]=100;
}

void DrawingVariables::SetCoordsForG2()
{	this->scale=1;
	Projections[0][0]=120;
	Projections[0][1]=0;
	Projections[1][0]=-60;
	Projections[1][1]=34.64;
}

void DrawingVariables::ApplyScale(double inputScale)
{	if (inputScale==0)
		return;
	double ScaleChange= inputScale/this->scale;
	this->scale= inputScale;
	for (int i =0; i<7;i++)
	{	this->Projections[i][0]*=ScaleChange;
		this->Projections[i][1]*=ScaleChange;
	}
}

void ComputationSetup::WriteReportToFile(DrawingVariables& TDV, std::fstream &theFile, GlobalVariables &theGlobalVariables)
{	theFile.clear();
	LaTeXProcedures::beginDocument(theFile);
	if (this->thePartialFraction.size>0)
		this->WriteToFilePFdecomposition(theFile);
	std::string tempS;
	this->VPVectors.ElementToString(tempS);
	theFile<<"\\title{"<<this->WeylGroupLetter<<(int)(this->WeylGroupIndex )<<"}" <<"\\maketitle ";
	this->theChambers.WriteToFile(TDV,this->VPVectors,theFile);
	LaTeXProcedures::endLatexDocument(theFile);
}

void ComputationSetup::SetupCustomNilradicalInVPVectors(GlobalVariables& theGlobalVariables)
{ this->VPVectors.size=0;
	this->WeylGroupIndex= (this->NumColsNilradical+this->NumRowsNilradical-1);
	this->WeylGroupLetter='A';
	for (int i=0;i<this->NumRowsNilradical;i++)
	{ for(int j=0;j<this->NumColsNilradical;j++)
		{ root tempRoot; tempRoot.MakeZero(this->WeylGroupIndex);
			for (int k=i; k<this->NumRowsNilradical+j;k++)
				tempRoot.TheObjects[k].MakeOne();
			this->VPVectors.AddRoot(tempRoot);
		}
	}
	this->VPVectors.ComputeDebugString();
}

::std::stringstream  CGIspecificRoutines::outputStream;
int CGIspecificRoutines::numLines;
int CGIspecificRoutines::shiftX=0;
int CGIspecificRoutines::numDashedLines=0;
int CGIspecificRoutines::numRegularLines=0;
int CGIspecificRoutines::numDottedLines=0;
int CGIspecificRoutines::shiftY=-200;
int CGIspecificRoutines::scale=100;

void CGIspecificRoutines::drawlineInOutputStreamBetweenTwoRoots
	(	root& r1, root& r2,	unsigned long thePenStyle, int r, int g, int b)
{	if (thePenStyle!=5)
	{	CGIspecificRoutines::outputStream << thePenStyle<<" ["<<r<<","<<g<<","<<b<<"] ";
		for (int i=0;i<r1.size;i++)
			CGIspecificRoutines::outputStream << (int)(CGIspecificRoutines::scale* r1.TheObjects[i].DoubleValue()) << " "
																				<< (int)(CGIspecificRoutines::scale* r2.TheObjects[i].DoubleValue()) << " ";
		CGIspecificRoutines::numLines++;
		switch(thePenStyle)
		{ case 0: CGIspecificRoutines::numRegularLines++; break;
			case 1: CGIspecificRoutines::numDashedLines++; break;
			case 2: CGIspecificRoutines::numDottedLines++; break;
		}
	}
}

void CGIspecificRoutines::PrepareOutputLineJavaScriptSpecific(const std::string& lineTypeName, int numberLines)
{	std::cout	<< "\n\tvar num"<< lineTypeName <<"Lines=" 					<< numberLines <<";";
	std::cout	<< "\n\tvar "		<< lineTypeName <<"1= new Array("		<< numberLines <<");"
						<< "  \tvar "		<< lineTypeName <<"2= new Array("		<< numberLines <<");"
						<< "  \tvar clr"	<< lineTypeName <<"= new Array("	<< numberLines <<");";
}
void CGIspecificRoutines::outputLineJavaScriptSpecific
	(const std::string& lineTypeName, int theDimension, std::string& stringColor, int& lineCounter )
{	std::string tempS;
	std::cout	<< "\n\t"		<< lineTypeName <<"1["	<< lineCounter <<"]= new Array(" << theDimension << ");"
							<< "\t"		<< lineTypeName <<"2["	<< lineCounter <<"]= new Array(" << theDimension << ");"
							<< "\tclr"<< lineTypeName <<"["  	<< lineCounter <<"]= new Array(" << 3 << ");\n";
	for (int j=0;j< theDimension;j++)
	{ CGIspecificRoutines::outputStream>> tempS;
		std::cout << "\t"<< lineTypeName <<"1[" <<lineCounter<< "][" << j<<"]=" <<tempS <<";";
		CGIspecificRoutines::outputStream>> tempS;
		std::cout << "\t"<< lineTypeName <<"2[" <<lineCounter<< "][" << j<<"]=" <<tempS <<";";
	}
	std::cout << "\tclr"<< lineTypeName <<"[" <<lineCounter<< "]=" <<stringColor <<";";
	lineCounter++;
}

void CGIspecificRoutines::MakeReportFromComputationSetup(ComputationSetup& input)
{ CGIspecificRoutines::numLines=0;
	CGIspecificRoutines::numRegularLines=0;
	CGIspecificRoutines::numDottedLines=0;
	TDV.flag2DprojectionDraw=false;
	input.theChambers.drawOutput
		(	TDV, input.theChambers, input.VPVectors, -1,
			input.theChambers.IndicatorRoot,0,&::drawlineDummy,
			&::drawtextDummy);
	std::string tempS;
	std::string stringColor;
	CGIspecificRoutines::outputStream.seekg(0);
	std::cout << "\n<script>";
	std::cout	<< "\n\tvar numDrawLines=" << CGIspecificRoutines::numLines<<";";
	CGIspecificRoutines::PrepareOutputLineJavaScriptSpecific("l", CGIspecificRoutines::numRegularLines);
	CGIspecificRoutines::PrepareOutputLineJavaScriptSpecific("da",CGIspecificRoutines::numDashedLines);
	CGIspecificRoutines::PrepareOutputLineJavaScriptSpecific("do",CGIspecificRoutines::numDottedLines);
	int theDimension=input.theChambers.AmbientDimension;
	int numLlines=0;	int numDalines=0;	int numDolines=0;
	for (int i=0;i<CGIspecificRoutines::numLines;i++)
	{	CGIspecificRoutines::outputStream>> tempS>> stringColor;
		if (tempS=="0")
			CGIspecificRoutines::outputLineJavaScriptSpecific
				("l",theDimension,stringColor, numLlines);
		if (tempS=="1")
			CGIspecificRoutines::outputLineJavaScriptSpecific
				("da",theDimension,stringColor, numDalines);
		if (tempS=="2")
			CGIspecificRoutines::outputLineJavaScriptSpecific
				("do",theDimension,stringColor, numDolines);
	}
	std::cout <<"</script>";
#ifdef CGIversionLimitRAMuse
	std::cout <<"Total number of pointers allocated: "<< ::ParallelComputing::GlobalPointerCounter <<"<br>\n";
#endif
	std::cout << "Number of partial fractions: " << input.thePartialFraction.size <<"<br>\n";
	std::cout << "Number of monomials in the numerators: "<<input.thePartialFraction.NumMonomialsInTheNumerators
						<<"<br>\n";
	std::cout << "<textarea name=\"vp_output\" cols=\"120\" rows=\"150\">"
						<< input.thePartialFraction.DebugString
						<< "</textarea>";
}

struct bmpfile_magic {
  unsigned char magic[2];
};

struct bmpfile_header {
	unsigned int filesz;
	unsigned short creator1;
	unsigned short creator2;
	unsigned int bmp_offset;
};

struct bmp_dib_v3_header_t
{ unsigned int header_sz;
	unsigned int width;
	unsigned int height;
	unsigned short nplanes;
	unsigned short bitspp;
	unsigned int compress_type;
	unsigned int bmp_bytesz;
	unsigned int hres;
	unsigned int vres;
	unsigned int ncolors;
	unsigned int nimpcolors;
};


void CGIspecificRoutines::MakeABitmap(std::string& fileName, std::fstream& outputFileOpenWithPreparedHeader)
{	outputFileOpenWithPreparedHeader.open(fileName.c_str(),std::fstream::trunc|std::fstream::out| std::fstream::binary);
	outputFileOpenWithPreparedHeader.clear(std::ios::goodbit);// false);
	::bmpfile_magic h1;
	::bmpfile_header h2;
	::bmp_dib_v3_header_t h3;
	int size=10000;
	h1.magic[0]='B'; h1.magic[1]='M';
	h2.bmp_offset=14;
	h2.creator1=0;
	h2.creator2=0;
	h2.filesz=size*3+54;
	h3.width=100;
	h3.height=100;
	h3.header_sz=40;
	h3.nplanes=1;
	h3.bitspp=24;
	h3.bmp_bytesz=size*3;
	h3.compress_type=0;
	h3.hres=2835;
	h3.vres=2835;
	h3.nimpcolors=0;
	h3.ncolors=0;
	outputFileOpenWithPreparedHeader.write((char*)(&h1),2);
	outputFileOpenWithPreparedHeader.write((char*)(&h2),12);
	outputFileOpenWithPreparedHeader.write((char*)(&h3),40);
	for (int i=0;i<size;i++)
	{	char tempUI=100;
		outputFileOpenWithPreparedHeader.write(&tempUI,1);
		outputFileOpenWithPreparedHeader.write(&tempUI,1);
		outputFileOpenWithPreparedHeader.write(&tempUI,1);
	}
	outputFileOpenWithPreparedHeader.close();
}

void CGIspecificRoutines::CivilizedStringTranslation(std::string& input)
{	for (int i=0;i<(signed)input.length();i++)
	{	if (input[i]=='&')
			input[i]=' ';
		if (input[i]=='=')
		{	input.insert(i+1," ");
			input.insert(i," ");
			i+=2;
		}
	}
}

bool CGIspecificRoutines::ReadDataFromCGIinput(std::string& input, ComputationSetup& output)
{	if (input.length()<2)
		return false;
	std::string tempS3;
	tempS3=input;
	tempS3.resize(7);
	if (tempS3!="textDim")
		return false;
	CGIspecificRoutines::CivilizedStringTranslation(input);
	//std::cout<<input;
	std::stringstream tempStream;
	tempStream << input;
	std::string tempS; int tempI;	tempStream.seekg(0);
	tempStream >> tempS>>tempS>> output.theChambers.AmbientDimension;
	if (output.theChambers.AmbientDimension>20)
		return false;
	tempStream >> tempS>>tempS>> tempI;
	output.VPVectors.SetSizeExpandOnTopNoObjectInit(tempI);
	//std::cout<<"Dim: "<<output.theChambers.AmbientDimension<<"Num: "<<tempI;
	for (int i=0;i<output.VPVectors.size;i++)
	{ output.VPVectors.TheObjects[i].SetSizeExpandOnTopLight(output.theChambers.AmbientDimension);
		for(int j=0;j<(signed int)output.theChambers.AmbientDimension;j++)
		{ tempStream>> tempS>>tempS>>tempI;
			output.VPVectors.TheObjects[i].TheObjects[j].AssignInteger(tempI);
		}
	}
	//output.VPVectors.ComputeDebugString();
	//std::cout<<output.VPVectors.size<<output.theChambers.AmbientDimension<< output.VPVectors.DebugString;
	output.flagComputingPartialFractions=true;
	output.thePartialFraction.flagUsingOrlikSolomonBasis=false;
	output.flagComputingVectorPartitions=false;
	output.flagUsingCustomVectors=true;
	output.thePartialFraction.flagMaxNumStringOutputLines=200000;
//	std::cout<<"\n<br><br>"<<output.VPVectors.DebugString;
//	std::cout<<"\n<br><br>"<<output.VPVectors.TheObjects[1].DebugString;
//	std::cout<<"\n<br><br>"<<output.VPVectors.TheObjects[2].DebugString;
//	std::cout<<"\n<br><br>"<<output.VPVectors.TheObjects[3].DebugString;
	return true;
}

ComputationSetup::ComputationSetup()
{	this->AllowRepaint=true;
	this->flagUsingCustomVectors=false;
//	this->flagComputingPartialFractions=true;
	this->flagComputingPartialFractions=false;
	this->flagComputingVectorPartitions=true;
	this->ComputingChambers=true;
	this->flagComputationInProgress=false;
	this->flagComputationDone=true;
	this->flagComputationInitialized=false;
	this->MakingCheckSumPFsplit=false;
	this->flagOneIncrementOnly=false;
	this->flagFullChop=true;
	this->flagHavingBeginEqnForLaTeXinStrings=true;
	this->flagSliceTheEuclideanSpaceInitialized=false;
	this->thePartialFraction.flagUsingIndicatorRoot=true;
	this->flagDisplayingPartialFractions=true;
	this->flagDisplayingCombinatorialChambersTextData=false;
	this->flagHavingDocumentClassForLaTeX=true;
	this->flagHavingStartingExpression=true;
	this->flagComputationIsDoneStepwise=true;
	//this->flagAffineComputationDone=false;
	this->flagSuperimposingComplexes=true;
	this->flagCustomNilradicalInitted=false;
	this->flagDoCustomNilradical=false;
	this->flagOneSteChamberSliceInitialized=false;
	this->NextDirectionIndex=0;
	this->WeylGroupLetter='A';
	this->WeylGroupIndex=3;
	this->NumRowsNilradical=2;
	this->NumColsNilradical=2;
	this->theGlobalVariablesContainer= new GlobalVariablesContainer;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter++;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
	this->theGlobalVariablesContainer->SetSizeExpandOnTopNoObjectInit(1);
//	this->RankEuclideanSpaceGraphics=3;
}

ComputationSetup::~ComputationSetup()
{	delete this->theGlobalVariablesContainer;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
}


void ComputationSetup::WriteToFilePFdecomposition(std::fstream& output)
{ std::string tempS;
  this->thePartialFraction.ElementToString
		(	tempS,true,false,true,
			*this->theGlobalVariablesContainer->Default());
  if (this->flagHavingDocumentClassForLaTeX)
  { output << "\\documentclass{article}\n\\begin{document}"
        <<tempS<< "\n\\end{document}";
  }
}

void ComputationSetup::oneIncrement(GlobalVariables& theGlobalVariables)
{	this->AllowRepaint=false;
	for ( this->oneStepChamberSlice(theGlobalVariables);
        this->theChambers.PreferredNextChambers.size!=0;
        this->oneStepChamberSlice(theGlobalVariables))
	{}
	this->AllowRepaint=true;
}

void ComputationSetup::FullChop(GlobalVariables& theGlobalVariables)
{	this->AllowRepaint=false;
	this->theChambers.flagDrawingProjective=true;
	while (this->theChambers.flagDrawingProjective)
		this->oneIncrement(theGlobalVariables);
	this->AllowRepaint=true;
}

void ComputationSetup::initWeylActionSpecifics(GlobalVariables& theGlobalVariables)
{ this->theChambers.flagDrawingProjective=true;
	CombinatorialChamberContainer tempComplex;
	this->theChambers.NumAffineHyperplanesProcessed=-1;
	this->theChambers.AmbientDimension=this->WeylGroupIndex;
	this->InputRoots.CopyFromBase(this->VPVectors);
	this->NextDirectionIndex=this->theChambers.AmbientDimension-1;
	tempComplex.SliceTheEuclideanSpace
		(	this->InputRoots,this->NextDirectionIndex,this->WeylGroupIndex,
				this->thePartialFraction.IndicatorRoot,
					*this->theGlobalVariablesContainer->Default());
	this->initGenerateWeylAndHyperplanesToSliceWith(theGlobalVariables,tempComplex);
	this->flagComputationInitialized=true;
	this->theChambers.flagSliceWithAWallInitDone=false;
}

void ComputationSetup::initGenerateWeylAndHyperplanesToSliceWith
	(GlobalVariables& theGlobalVariables, CombinatorialChamberContainer& inputComplex)
{ assert(&inputComplex!=&this->theChambers);
	this->theChambers.flagDrawingProjective=true;
	this->AllowRepaint=false;
	this->theChambers.InduceFromLowerDimensionalAndProjectivize
		(inputComplex,*this->theGlobalVariablesContainer->Default());
	this->theChambers.ConvertHasZeroPolyToPermanentlyZero();
	WeylGroup tempWeyl;
	tempWeyl.MakeArbitrary(this->WeylGroupLetter, this->WeylGroupIndex);
	tempWeyl.ComputeWeylGroup();
	tempWeyl.ComputeDebugString();
	this->theChambers.theWeylGroupAffineHyperplaneImages
		.SetSizeExpandOnTopNoObjectInit(0);
	this->theChambers.AffineWallsOfWeylChambers.ClearTheObjects();
	inputComplex.AddWeylChamberWallsToHyperplanes(theGlobalVariables, tempWeyl);
	this->theChambers.WeylChamber.CopyFromBase(inputComplex.WeylChamber);
	for (int i=0;i<inputComplex.theHyperplanes.size;i++)
	{ affineHyperplane tempH;
		root tempRoot; tempRoot.MakeZero(inputComplex.AmbientDimension);
		int start = 1;
		if (i>=inputComplex.NumProjectiveHyperplanesBeforeWeylChamberWalls)
		{	this->theChambers.NumAffineHyperplanesBeforeWeylChamberWalls =
				this->theChambers.theWeylGroupAffineHyperplaneImages.size;
			start= 0;
		}
		inputComplex.theHyperplanes.ComputeDebugString();
		for (int j=start;j<tempWeyl.size;j++)
		{	tempH.MakeFromNormalAndPoint
				(tempRoot,inputComplex.theHyperplanes.TheObjects[i]);
			tempH.ComputeDebugString();
//			tempWeyl.ActOnAffineHyperplaneByGroupElement(j,tempH,true,true);
			tempWeyl.ActOnRootByGroupElement(j,tempH.affinePoint,true, true);
			if (tempH.HasACommonPointWithPositiveTwoToTheNth_ant())
        if (this->theChambers.theWeylGroupAffineHyperplaneImages
							.AddObjectOnTopNoRepetitionOfObject(tempH))
					if (start==0)
						this->theChambers.AffineWallsOfWeylChambers
							.AddObjectOnTopNoRepetitionOfObjectHash(tempH);
			//tempH.ComputeDebugString();
		}
		std::string tempS;
		this->theChambers.theWeylGroupAffineHyperplaneImages.ComputeDebugString();
		this->theChambers.AffineWallsOfWeylChambers.ElementToStringGeneric(tempS);
		this->theChambers.ComputeDebugString();
	}
	this->theChambers.NewHyperplanesToSliceWith.size=0;
	this->theChambers.NewHyperplanesToSliceWith.MakeActualSizeAtLeastExpandOnTop
		(this->theChambers.theWeylGroupAffineHyperplaneImages.size);
	this->theChambers.theWeylGroupAffineHyperplaneImages.ComputeDebugString();
	for (int i=0;i<this->theChambers.theWeylGroupAffineHyperplaneImages.size;i++)
	{ root tempRoot;
		tempRoot.MakeNormalInProjectivizationFromAffineHyperplane
			(theChambers.theWeylGroupAffineHyperplaneImages.TheObjects[i]);
		//tempRoot.ComputeDebugString();
		tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
		this->theChambers.NewHyperplanesToSliceWith.AddRootNoRepetition(tempRoot);
	}
	this->theChambers.flagSliceWithAWallInitDone=false;
}

void ComputationSetup::oneStepChamberSlice(GlobalVariables& theGlobalVariables)
{	this->AllowRepaint=false;
	this->flagComputationInProgress=true;
	this->theChambers.flagDrawingProjective=true;
	if (this->theChambers.PreferredNextChambers.size==0 &&
			this->theChambers.NumAffineHyperplanesProcessed	<
			this->theChambers.NewHyperplanesToSliceWith.size)
		this->theChambers.NumAffineHyperplanesProcessed++;
	if (	this->theChambers.NumAffineHyperplanesProcessed	<
				this->theChambers.NewHyperplanesToSliceWith.size)
	{	if (this->theChambers.PreferredNextChambers.size==0)
			this->theChambers.SliceWithAWallInit(this->theChambers.NewHyperplanesToSliceWith
				.TheObjects[this->theChambers.NumAffineHyperplanesProcessed],
					*this->theGlobalVariablesContainer->Default());
		else
			this->theChambers.SliceWithAWallOneIncrement(this->theChambers.NewHyperplanesToSliceWith
				.TheObjects[this->theChambers.NumAffineHyperplanesProcessed],theGlobalVariables);
	} else
	{ this->theChambers.flagDrawingProjective=false;
    this->theChambers.ProjectToDefaultAffineSpace(theGlobalVariables);
		this->flagComputationDone=true;
    this->theChambers.ComputeDebugString();
  }
 // this->theChambers.ComputeDebugString();
	this->AllowRepaint=true;
}

void ComputationSetup::AdjustGraphicsForTwoDimensionalLieAlgebras(DrawingVariables& theDV)
{ if (this->WeylGroupIndex!=2)
		return;
	if (this->WeylGroupLetter=='A')
		TDV.SetCoordsForA2();
	if (this->WeylGroupLetter=='B')
		TDV.SetCoordsForB2();
	if (this->WeylGroupLetter=='C')
		TDV.SetCoordsForC2();
	if (this->WeylGroupLetter=='G')
		TDV.SetCoordsForG2();
}

void ComputationSetup::InitComputationSetup()
{ if(!this->flagComputationInitialized)
	{	this->theChambers.flagDrawingProjective=!this->flagDoingWeylGroupAction;
		if (this->flagDoingWeylGroupAction)
			this->initWeylActionSpecifics(*this->theGlobalVariablesContainer->Default());
		else
		{	this->theChambers.AmbientDimension=this->WeylGroupIndex;
			this->InputRoots.CopyFromBase(this->VPVectors);
			this->NextDirectionIndex=this->WeylGroupIndex-1;
		}
		this->AdjustGraphicsForTwoDimensionalLieAlgebras(TDV);
		this->flagComputationInitialized=true;
		this->flagComputationDone=false;
		this->flagComputationInProgress=true;
		this->theChambers.NumAffineHyperplanesProcessed=0;
		PolyFormatLocal.MakeAlphabetxi();
		::IndicatorWindowGlobalVariables.Nullify();
		PolynomialOutputFormat::LatexMaxLineLength=95;
		PolynomialOutputFormat::UsingLatexFormat=true;
		RandomCodeIDontWantToDelete::SomeRandomTests2();
		partFractions::flagUsingCheckSum=this->MakingCheckSumPFsplit;
		this->flagDoneComputingPartialFractions=false;
	//		this->theChambers.SliceTheEuclideanSpace
	//													(	::InputRoots,::NextDirectionIndex,root::AmbientDimension,
	//														this->thePartialFraction.IndicatorRoot);
	//		this->theChambers.ComputeDebugString();
	}
}

void ComputationSetup::DoTheRootSAComputation()
{	//rootSubalgebra theRootSA;

	this->theRootSubalgebra.NumRelationsgreaterLengthThan2=0;
	//theRootSA.flagAnErrorHasOccuredTimeToPanic=true;

/*	theRootSA.SetupE6_3A2(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString();
	theRootSA.theGoodRelations.ComputeDebugString();

	theRootSA.SetupE6_2A2plusA1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString();
	theRootSA.theGoodRelations.ComputeDebugString();

	theRootSA.SetupE6_A5(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString();
	theRootSA.theGoodRelations.ComputeDebugString();

	theRootSA.SetupE6_A4plusA1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString();
	theRootSA.theGoodRelations.ComputeDebugString();

	theRootSA.SetupE6_D5(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString();
	theRootSA.theGoodRelations.ComputeDebugString();

	theRootSA.SetupE6_A3plus2A1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);
*/
	this->theRootSubalgebra.flagAnErrorHasOccuredTimeToPanic=true;
	this->theRootSubalgebra.SetupE6_A4(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.theBadRelations.ComputeDebugString(this->theRootSubalgebra);
	this->theRootSubalgebra.theGoodRelations.ComputeDebugString(this->theRootSubalgebra);
/*


	theRootSA.SetupE6_A3plusA1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);

	theRootSA.SetupE6_2A2(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);

	theRootSA.SetupE6_A2plus2A1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);

	theRootSA.SetupE6_4A1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);

	theRootSA.SetupE6_D4(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);

	this->theRootSubalgebra.SetupE6_A3(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.theBadRelations.ComputeDebugString(this->theRootSubalgebra);
	this->theRootSubalgebra.theGoodRelations.ComputeDebugString(this->theRootSubalgebra);

	theRootSA.SetupE6_A2plusA1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);

	this->theRootSubalgebra.SetupE6_3A1(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.theBadRelations.ComputeDebugString(this->theRootSubalgebra);
	this->theRootSubalgebra.theGoodRelations.ComputeDebugString(this->theRootSubalgebra);

	theRootSA.SetupE6_A2(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);
*//*
	this->theRootSubalgebra.SetupE6_2A1(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebra.theBadRelations.ComputeDebugString(this->theRootSubalgebra);
	this->theRootSubalgebra.theGoodRelations.ComputeDebugString(this->theRootSubalgebra);

	*//*theRootSA.SetupE6_A1(*this->theGlobalVariablesContainer->Default());
	theRootSA.GenerateParabolicsInCentralizerAndPossibleNilradicals
		(*this->theGlobalVariablesContainer->Default());
	theRootSA.theBadRelations.ComputeDebugString(theRootSA);
	theRootSA.theGoodRelations.ComputeDebugString(theRootSA);*/
}

void ComputationSetup::Run()
{ this->AllowRepaint=false;
	this->InitComputationSetup();
	std::string BeginString;
	//this->thePartialFraction.flagAnErrorHasOccurredTimeToPanic=true;
	//partFraction::flagAnErrorHasOccurredTimeToPanic=true;
	this->thePartialFraction.flagUsingOrlikSolomonBasis=false;
	//MatrixLargeRational::flagAnErrorHasOccurredTimeToPanic=true;
/*	this->DoTheRootSAComputation();
    this->theRootSubalgebra.ComputeDebugString(true);
    this->theOutput.DebugString.append(	"\\documentclass{article}\n\\usepackage{amssymb}\n\\begin{document}");
		this->theOutput.DebugString.append(this->theRootSubalgebra.DebugString);
    this->theOutput.DebugString.append("\\end{document}");
    this->ExitComputationSetup();
    this->AllowRepaint=true;
    this->flagComputationInProgress=false;
		return;
	*///partFraction::flagAnErrorHasOccurredTimeToPanic=true;
	//this->thePartialFraction.IndicatorRoot.InitFromIntegers(6,10,0,0,0);
	//this->VPVectors.ComputeDebugString();
	if (this->flagComputingPartialFractions && ! this->flagDoneComputingPartialFractions)
	{	if (!this->flagUsingCustomVectors)
		{	this->thePartialFraction.ComputeKostantFunctionFromWeylGroup
				(	this->WeylGroupLetter,this->WeylGroupIndex,this->theOutput,
					&this->thePartialFraction.IndicatorRoot,false,false,
					*this->theGlobalVariablesContainer->Default());
			this->theOutput.ComputeDebugString();
		}
		else
		{ intRoots tempRoots;
			tempRoots.AssignRoots(this->VPVectors);
			this->thePartialFraction.initFromRootSystem
				(tempRoots,tempRoots,0,*this->theGlobalVariablesContainer->Default());
			if (this->flagHavingStartingExpression)
				this->thePartialFraction.ElementToString(BeginString, *this->theGlobalVariablesContainer->Default());
			this->thePartialFraction.split(*this->theGlobalVariablesContainer->Default());
			if (this->flagComputingVectorPartitions)
			{	this->thePartialFraction.partFractionsToPartitionFunctionAdaptedToRoot
					(	this->theOutput,this->thePartialFraction.IndicatorRoot,
						false,false,*this->theGlobalVariablesContainer->Default());
				this->theOutput.ComputeDebugString();
			}
		}
		if (this->flagHavingBeginEqnForLaTeXinStrings)
		{ std::stringstream out;
			if (this->flagHavingDocumentClassForLaTeX)
        out<< "\\documentclass{article}\n\\begin{document}";
			out <<"\\begin{eqnarray*}&&"<<this->theOutput.DebugString<< "\\end{eqnarray*}";
			if (this->flagHavingDocumentClassForLaTeX)
        out<< "\n\\end{document}";
			this->theOutput.DebugString= out.str();
		}
		if (this->flagDisplayingPartialFractions)
		{ std::stringstream out2;
			if (this->flagHavingBeginEqnForLaTeXinStrings)
				out2<<"\\documentclass{article}\n\\begin{document}";
			if (this->flagHavingStartingExpression)
				out2<< BeginString<<"=";
			this->thePartialFraction.ComputeDebugString
				(*this->theGlobalVariablesContainer->Default());
			out2<<this->thePartialFraction.DebugString;
			if (this->flagHavingBeginEqnForLaTeXinStrings)
	       out2<< "\n\\end{document}";
			this->thePartialFraction.DebugString= out2.str();
		}
		this->flagDoneComputingPartialFractions=true;
	}
	if (this->ComputingChambers)
	{	if (!this->flagDoingWeylGroupAction)
		{	if (this->flagFullChop)
			{	this->theChambers.SliceTheEuclideanSpace
					(	this->InputRoots,this->NextDirectionIndex,this->theChambers.AmbientDimension,
						this->thePartialFraction.IndicatorRoot, *this->theGlobalVariablesContainer->Default());
			} else
			{ if (this->flagOneIncrementOnly)
					this->theChambers.SliceOneDirection
						(	this->InputRoots, this->NextDirectionIndex,this->theChambers.AmbientDimension,
							this->thePartialFraction.IndicatorRoot, *this->theGlobalVariablesContainer->Default());
				else
					this->theChambers.OneSlice
						(	this->InputRoots, this->NextDirectionIndex,this->theChambers.AmbientDimension,
							this->thePartialFraction.IndicatorRoot, *this->theGlobalVariablesContainer->Default());
			}
			this->theChambers.ComputeDebugString();
		} else
		{ if (this->flagFullChop)
				this->FullChop(*this->theGlobalVariablesContainer->Default());
			else
			{	if (this->flagOneIncrementOnly)
					this->oneIncrement(*this->theGlobalVariablesContainer->Default());
				else
					this->oneStepChamberSlice(*this->theGlobalVariablesContainer->Default());
			}
		}
	}
	//TheBigOutput.InduceFromLowerDimensionalAndProjectivize(this->theChambers);
	this->theChambers.ComputeDebugString();
	this->ExitComputationSetup();
	//std::stringstream out;
	//ListBasicObjects<roots> tempRoots;
	//this->thePartialFraction.ComputeSupport(tempRoots, out);
	//std::string tempS;
//	tempS=out.str();
	this->AllowRepaint=true;
	this->flagComputationInProgress=false;
}

void ComputationSetup::ExitComputationSetup()
{ if (!this->flagDoingWeylGroupAction)
	{	if(this->NextDirectionIndex>=this->InputRoots.size)
		{ //Computation is done and we must reset
			this->flagComputationDone=true;
		//	this->flagComputationInitialized=false;
		}
	}
	ParallelComputing::SafePointReached=true;
}

void ComputationSetup::Reset()
{ this->flagComputationInProgress=false;
	this->flagComputationDone=true;
	this->flagComputationInitialized=false;
}

void ComputationSetup::EvaluatePoly()
{ //Rational::flagAnErrorHasOccurredTimeToPanic=true;
	this->theOutput.Evaluate(this->ValueRoot,this->Value);
	this->Value.ElementToString(this->ValueString);
}

void DrawingVariables::GetCoordsForDrawing(DrawingVariables& TDV, root& r,double& x, double& y)
{	x=TDV.centerX;
	y=TDV.centerY;
	for (int k=0; k<r.size; k++)
	{	x+=(r.TheObjects[k].DoubleValue())*(TDV.Projections[k][0]);
		y-=(r.TheObjects[k].DoubleValue())*(TDV.Projections[k][1]);
	}
}

void DrawingVariables::drawlineBetweenTwoVectors
	(	root& r1, root& r2, int PenStyle, int PenColor, std::fstream* LatexOutFile,
		drawLineFunction theDrawFunction)
{ if (this->flag2DprojectionDraw)
	{	double x1,x2,y1,y2;
		this->GetCoordsForDrawing(*this,r1,x1,y1);
		this->GetCoordsForDrawing(*this,r2,x2,y2);
		this->drawLine
			(x1,y1,x2,y2,PenStyle,PenColor, LatexOutFile, theDrawFunction);
	}
	else
	{ int r,g,b;
		r=PenColor/65536;
		g=(PenColor/256)%256;
		b=PenColor%256;
		::CGIspecificRoutines::drawlineInOutputStreamBetweenTwoRoots(r1,r2,PenStyle,r,g,b);
	}
}

void DrawingVariables::drawCoordSystem
	(	DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile,
		drawTextFunction drawTextIn)
{	Rational Epsilon; Epsilon.AssignNumeratorAndDenominator(-1,5);
	for (int i=0;i<theDimension;i++)
	{ root tempRoot; tempRoot.MakeZero(theDimension);
		root tempRoot2; tempRoot2.MakeZero(theDimension);
		for(int j=0;j<theDimension;j++)
			tempRoot.TheObjects[j].Assign(Epsilon);
		tempRoot.TheObjects[i].MakeOne();
		tempRoot2.TheObjects[i].MakeOne();
		std::string tempS; std::stringstream out;
		tempRoot2.ElementToString(tempS);
		out <<"("<<tempS<<")";
		tempS=out.str();
		TDV.drawTextAtVector(tempRoot,tempS, 0,LatexOutFile, drawTextIn);
	}
}

void DrawingVariables::drawLine
	(	double X1, double Y1, double X2, double Y2,
		unsigned long thePenStyle, int ColorIndex, std::fstream* LatexOutFile,
		drawLineFunction theDrawFunction)
{ if (LatexOutFile==0)
		(*theDrawFunction)(X1,Y1,X2,Y2,thePenStyle, ColorIndex);
	else
		LaTeXProcedures::drawline(X1,Y1, X2, Y2, thePenStyle, ColorIndex,*LatexOutFile);
}

void DrawingVariables::drawTextAtVector
	(	root& point, std::string& inputText, int textColor, std::fstream* LatexOutFile,
		drawTextFunction drawTextIn)
{ double x,y; this->GetCoordsForDrawing(*this,point,x,y);
	this->drawText(x,y,inputText,textColor,LatexOutFile,drawTextIn);
}

void DrawingVariables::drawText
	(	double X1, double Y1, std::string& inputText, int color, std::fstream* LatexOutFile,
		drawTextFunction drawTextIn)
{	if (LatexOutFile==0)
		drawTextIn(X1-7,Y1-7,inputText.c_str(),inputText.length(),color);
  else
    ::LaTeXProcedures::drawText(X1, Y1,inputText,color,*LatexOutFile);
}

void CombinatorialChamberContainer::drawFacetVerticesMethod2
	(	DrawingVariables& TDV,roots& r, roots& directions, int ChamberIndex,
		WallData& TheFacet, int DrawingStyle, int DrawingStyleDashes,
		drawLineFunction theDrawFunction)
{	root tempRoot;
	root Projection1;
	root tempRoot2;
	root zeroRoot; zeroRoot.MakeZero(r.TheObjects[0].size);
	root Projection2;
	for (int i = 0; i<r.size; i++)
	{	if (TheFacet.IsInFacetNoBoundaries(r.TheObjects[i]))
		{	tempRoot.Assign(r.TheObjects[i]);
			TDV.ProjectOnToHyperPlaneGraphics(tempRoot, Projection1,directions);
			if (TDV.DrawDashes)
				TDV.drawlineBetweenTwoVectors
					(zeroRoot,Projection1, DrawingStyleDashes,TDV.ColorDashes,0,theDrawFunction);
			for (int j=i+1;j<r.size; j++)
			{ if (TheFacet.IsInFacetNoBoundaries(r.TheObjects[j]))
				{	tempRoot2.Assign(r.TheObjects[j]);
					TDV.ProjectOnToHyperPlaneGraphics(tempRoot2, Projection2, directions);
					TDV.drawlineBetweenTwoVectors
						(	Projection1, Projection2,	DrawingStyle,TDV.Colors[ChamberIndex%TDV.NumColors],0,
							theDrawFunction);
				}
			}
		}
	}
}

void CombinatorialChamberContainer::drawOutput
	(	DrawingVariables& TDV, CombinatorialChamberContainer& output,
		roots& directions, int directionIndex, root& ChamberIndicator,
		std::fstream* LaTeXOutput,drawLineFunction theDrawFunction, drawTextFunction drawTextIn)
{	if (LaTeXOutput!=0)
		LaTeXProcedures::beginPSTricks(*LaTeXOutput);
	for(int i=0;i<output.size;i++)
		if (output.TheObjects[i]!=0)
			output.TheObjects[i]->DisplayNumber=i;
	if (output.flagDrawingProjective)
		CombinatorialChamberContainer::drawOutputProjective
			(	TDV,output,directions, directionIndex,ChamberIndicator,
				theDrawFunction,drawTextIn);
	else
		CombinatorialChamberContainer::drawOutputAffine
			(TDV,output, LaTeXOutput,theDrawFunction,drawTextIn);
	if (LaTeXOutput!=0)
		LaTeXProcedures::endPSTricks(*LaTeXOutput);
}

//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
void CombinatorialChamberContainer::drawOutputAffine
	(	DrawingVariables &TDV, CombinatorialChamberContainer &output,
		std::fstream* LaTeXoutput,drawLineFunction theDrawFunction,
		drawTextFunction drawTextIn)
{ if(output.AffineWallsOfWeylChambers.size>0)
		output.GetNumChambersInWeylChamberAndLabelChambers(output.WeylChamber);
	else
		output.GetNumVisibleChambersAndLabelChambersForDisplay();
	for(int i=0;i<output.size;i++)
		if (output.TheObjects[i]!=0)
			output.TheObjects[i]->drawOutputAffine
				(TDV,output, LaTeXoutput, theDrawFunction,drawTextIn);
	TDV.drawCoordSystem(TDV, output.AmbientDimension-1,LaTeXoutput,drawTextIn);
}

//color styles (taken from windows.h and substituted for independence of the .h file):
// 0 = normal line
// 1 = dashed line
// 2 = dotted line
// 5 = invisible line (no line)
void CombinatorialChamberContainer::drawOutputProjective
	(	DrawingVariables& TDV, CombinatorialChamberContainer& output,
		roots& directions, int directionIndex, root& ChamberIndicator,
		drawLineFunction theDrawFunction,drawTextFunction drawTextIn)
{ int color=0;
	Rational::flagMinorRoutinesOnDontUseFullPrecision=true;
	int NumTrueChambers=0;
	//int NumTrueChambers2=0;
	int NumZeroChambers=0;
	for (int j=0; j<output.size;j++)
	{	if (output.TheObjects[j]!=0)
		{	if (!output.TheObjects[j]->flagHasZeroPolynomial)
			{	NumTrueChambers++;
				output.TheObjects[j]->DisplayNumber=NumTrueChambers;
			}
			else
			{	NumZeroChambers++;
				output.TheObjects[j]->DisplayNumber=NumZeroChambers;
			}
		}
	}
	if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
		output.ComputeDebugString();
	std::string tempS;
	std::stringstream out,out1,out3, out2;
	out<<"#Drawn chambers: "<<NumTrueChambers;
	tempS=out.str();
	drawTextIn(TDV.textX,TDV.textY, tempS.c_str(), tempS.length(),TDV.TextColor);
	out1<<"#Zero chambers: "<< NumZeroChambers;
	tempS=out1.str();
	drawTextIn(TDV.textX,TDV.textY+30, tempS.c_str(), tempS.length(),TDV.TextColor);
	out2<<"#Next chamber: ";
	if (output.indexNextChamberToSlice!=-1)
	{	if (output.TheObjects[output.indexNextChamberToSlice]!=0)
		{	if (output.TheObjects[output.indexNextChamberToSlice]->flagHasZeroPolynomial)
				out2 << "i"; else out2 <<"c";
			out2 << output.TheObjects[output.indexNextChamberToSlice]->DisplayNumber;
		}
	}
	if (output.flagMakingASingleHyperplaneSlice)
	{ out2	<< "; "<< "Plane: " << output.NumAffineHyperplanesProcessed+1 << " out of "
					<< output.theWeylGroupAffineHyperplaneImages.size;
	}
	tempS=out2.str();
	drawTextIn(TDV.textX,TDV.textY+15, tempS.c_str(), tempS.length(),TDV.TextColor);
	//int NumTrueChambers=0;
	//int NumTrueChambers2=0;
	//int NumZeroChambers=0;
	if (output.size>1000)
		return;
	if (CombinatorialChamber::DisplayingGraphics)
	{	for (int j=0; j<output.size;j++)
		{	if (output.TheObjects[j]!=0)
			{	bool hasZeroPoly= output.TheObjects[j]->flagHasZeroPolynomial;
				int DrawingStyle;
				int DrawingStyleDashes;
				if (!hasZeroPoly)
				{	DrawingStyle = TDV.DrawStyle;
					DrawingStyleDashes= TDV.DrawStyleDashes;
					color = TDV.TextColor;
					//NumTrueChambers2++;
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
				if (output.TheObjects[j]->flagPermanentlyZero){color = TDV.DeadChamberTextColor;}
				if (TDV.DrawChamberIndices)
				{	if ((!hasZeroPoly)||(TDV.DrawingInvisibles))
					{	root tempRoot;
						output.TheObjects[j]->ComputeInternalPointMethod2(tempRoot,output.AmbientDimension);
						root Proj;
						TDV.ProjectOnToHyperPlaneGraphics(tempRoot,Proj,directions);
						std::stringstream out; std::string tempS;
						if (TDV.DisplayingChamberCreationNumbers)
							out<<output.TheObjects[j]->CreationNumber;
						else
							out<<output.TheObjects[j]->DisplayNumber;
						tempS=out.str();
						TDV.drawTextAtVector(Proj,tempS,color,0,drawTextIn);
					}
				}
				for (int i =0;i< output.TheObjects[j]->Externalwalls.size;i++)
				{	CombinatorialChamberContainer::drawFacetVerticesMethod2
						(	TDV,output.TheObjects[j]->AllVertices,directions,j,
							output.TheObjects[j]->Externalwalls.TheObjects[i],
							DrawingStyle,DrawingStyleDashes,theDrawFunction);
				}
			}
		}
		if (output.size>0 && ChamberIndicator.size==output.AmbientDimension &&
				TDV.flag2DprojectionDraw)
		{	root tempRootX;
			TDV.ProjectOnToHyperPlaneGraphics(ChamberIndicator,tempRootX,directions);
			double tmpX,tmpY;
			TDV.GetCoordsForDrawing(TDV,tempRootX,tmpX,tmpY);
			TDV.drawLine
				(	tmpX-2,tmpY-2,tmpX+2,tmpY+2,TDV.DrawStyle,TDV.ColorChamberIndicator,0,
					theDrawFunction);
			TDV.drawLine
				(	tmpX-2,tmpY+2,tmpX+2,tmpY-2,TDV.DrawStyle,TDV.ColorChamberIndicator,0,
					theDrawFunction);
			tmpX=(tmpX-TDV.centerX)*1.5+TDV.centerX;
			tmpY=(tmpY-TDV.centerY)*1.5+TDV.centerY;
			TDV.drawLine
				(	TDV.centerX,TDV.centerY,tmpX,tmpY,TDV.DrawStyle,
					TDV.ColorChamberIndicator,0,theDrawFunction);
			std::string tempS="Indicator";
			drawTextIn(tmpX,tmpY,tempS.c_str(),tempS.size(),TDV.TextColor);
		}
	}
	Rational::flagMinorRoutinesOnDontUseFullPrecision=false;
}

void DrawingVariables::ProjectOnToHyperPlaneGraphics(root& input, root& output, roots& directions)
{	output.Assign(input);
	root normal; root basepoint;
	Rational tempRat2, tempRat, tempRat3;
	normal.MakeZero(input.size);
	for (int i=0;i<input.size;i++)
	{	if (input.TheObjects[i].IsNonNegative())
		{ normal.TheObjects[i].Add(ROne);}
		else
		{normal.TheObjects[i].Add(RMOne);}
	}
	basepoint.MakeZero(input.size);
	basepoint.TheObjects[0].AssignInteger(1);
	if (input.TheObjects[0].IsNegative())
	{basepoint.MinusRoot(); }
//////////////////////////////////////////////////
	root::RootScalarEuclideanRoot(output,normal,tempRat2);
	root::RootScalarEuclideanRoot(basepoint,normal,tempRat);
	if (!tempRat2.IsEqualToZero())
	{	tempRat3.Assign(tempRat);
		tempRat3.DivideBy(tempRat2);
		output.MultiplyByLargeRational(tempRat3);
	}
	else
	{	output.MakeZero(input.size);
	}
}

void root::ElementToString(std::string& output)
{ this->ElementToString(output,false);
}

void root::ElementToString(std::string& output, bool useLaTeX)
{	output.clear();
	std::string tempStr;
	if (useLaTeX)
    output.append("(");
  for(int i=0;i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempStr);
		output.append(tempStr);
		if (i!=this->size-1)
			output.append(",");
	}
	if (useLaTeX)
    output.append(")");
}

void root::MinusRoot()
{	MultiplyByInteger(-1);
}

void root::MultiplyByInteger(int a)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByInt(a);
	}
}

void root::MultiplyByLargeInt(LargeInt& a)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByLargeInt(a);
	}
}

void root::MultiplyByLargeIntUnsigned(LargeIntUnsigned& a)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByLargeIntUnsigned(a);
	}
}

void root::ScaleForMinHeightHeavy()
{	LargeIntUnsigned d;
	this->FindLCMDenominatorsHeavy(d);
	this->MultiplyByLargeIntUnsigned(d);
	d.MakeZero();
	for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i].IsEqualToZero())
		{	if (d.IsEqualToZero())
			{ this->TheObjects[i].GetNumExtendedUnsigned(d);
			}
			else
			{	LargeIntUnsigned tempI;
				this->TheObjects[i].GetNumExtendedUnsigned(tempI);
				LargeIntUnsigned::gcd(d,tempI,d);
			}
		}
	}
	this->DivByLargeIntUnsigned(d);
}

void root::ScaleForMinHeightLight()
{	int d= this->FindLCMDenominatorsTruncateToInt();
	this->MultiplyByInteger(d);
	d=0;
	for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i].IsEqualToZero())
		{	assert(this->TheObjects[i].Extended==0);
			if (d==0)
				d=this->TheObjects[i].NumShort;
			else
			{	int tempI=this->TheObjects[i].NumShort;
				if (tempI<0) tempI=-tempI;
				d= Rational::gcd(tempI,d);
			}
		}
	}
	this->DivByInteger(d);
}


bool root::IsEqualTo(const root& right)
{	if (this->size!=right.size)
		return false;
	for (int i=0;i<this->size;i++)
		if (!this->TheObjects[i].IsEqualTo(right.TheObjects[i]))
			return false;
	return true;
}

bool root::IsProportianalTo(root& r)
{	if (this->size!=r.size)
		return false;
	//r.ComputeDebugString();
	int IndexFirstNonZero=-1;
	for(int i=0;i<this->size;i++)
		if (!this->TheObjects[i].IsEqualToZero())
		{	IndexFirstNonZero=i;
			break;
		}
	if (IndexFirstNonZero==-1)
		return false;
	root tempRoot;
	tempRoot.Assign(*this);
	tempRoot.DivByLargeRational(this->TheObjects[IndexFirstNonZero]);
	tempRoot.MultiplyByLargeRational( r.TheObjects [IndexFirstNonZero]);
	return tempRoot.IsEqualTo(r);
}

void root::FindLCMDenominatorsHeavy(LargeIntUnsigned& output)
{ output.MakeOne();
	for (int i=0;i<this->size;i++)
	{	LargeIntUnsigned tempI,tempI2;
		this->TheObjects[i].GetDenExtended(tempI2);
		LargeIntUnsigned::gcd(output,tempI2,tempI);
		output.MultiplyBy(tempI2);
		output.DivPositive(tempI,output,tempI2);
	}
}

int root::FindLCMDenominatorsTruncateToInt()
{ int result=1;
	for (int i=0;i<this->size;i++)
	{	result = MathRoutines::lcm(result,this->TheObjects[i].DenShort);
		assert(this->TheObjects[i].Extended==0);
	}
	return result;
}

void root::ScaleToIntegralHeavy()
{ LargeIntUnsigned x;
	this->FindLCMDenominatorsHeavy(x);
	this->MultiplyByLargeIntUnsigned(x);
}

void root::ScaleToIntegralLight()
{ this->MultiplyByInteger(this->FindLCMDenominatorsTruncateToInt());
}

void root::ScaleToIntegralMinHeightHeavy()
{ this->ScaleToIntegralHeavy();
	this->ScaleForMinHeightHeavy();
}

void root::ScaleToIntegralMinHeightLight()
{ this->ScaleToIntegralLight();
	this->ScaleForMinHeightLight();
}

void root::ScaleToFirstNonZeroCoordinatePositive()
{ for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].IsPositive())
		{ return;
		}
		if (this->TheObjects[i].IsNegative())
		{ this->MultiplyByInteger(-1);
			return;
		}
	}
}

void root::ScaleToIntegralMinHeightFirstNonZeroCoordinatePositiveHeavy()
{ this->ScaleToIntegralMinHeightHeavy();
	this->ScaleToFirstNonZeroCoordinatePositive();
}

void root::ScaleToIntegralMinHeightFirstNonZeroCoordinatePositiveLight()
{ this->ScaleToIntegralMinHeightLight();
	this->ScaleToFirstNonZeroCoordinatePositive();
}

bool root::HasSmallCoordinates()
{	for (int i=0;i<this->size;i++)
		if (this->TheObjects[i].Extended!=0)
			return false;
	return true;
}

void root::ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive()
{	if (this->HasSmallCoordinates())
		this->ScaleToIntegralMinHeightFirstNonZeroCoordinatePositiveLight();
	else
		this->ScaleToIntegralMinHeightFirstNonZeroCoordinatePositiveHeavy();
}

void root::ScaleToIntegralMinHeight()
{	if (this->HasSmallCoordinates())
		this->ScaleToIntegralMinHeightLight();
	else
		this->ScaleToIntegralMinHeightHeavy();
}


void root::DivByLargeIntUnsigned(LargeIntUnsigned& a)
{	for (int i=0;i<this->size;i++)
		this->TheObjects[i].DivideByLargeIntegerUnsigned(a);
}

void root::MultiplyByLargeRational(Rational& a)
{	for (int i=0;i<this->size;i++)
		this->TheObjects[i].MultiplyBy(a);
}

void root::Add(root&r)
{	assert(r.size==this->size);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i].Add(r.TheObjects[i]);
}

bool root::OurScalarProductIsPositive(root& right)
{ Rational tempRat;
	root::RootScalarEuclideanRoot(*this,right,tempRat);
	return tempRat.IsPositive();
}

bool root::OurScalarProductIsNegative(root& right)
{ Rational tempRat;
	root::RootScalarEuclideanRoot(*this,right,tempRat);
	return tempRat.IsNegative();
}

bool root::OurScalarProductIsZero(root& right)
{ Rational tempRat;
	root::RootScalarEuclideanRoot(*this,right,tempRat);
	return tempRat.IsEqualToZero();
}

void root::Subtract(root&r)
{ assert(r.size==this->size);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i].Subtract(r.TheObjects[i]);
}

void root::MakeZero(int DesiredDimension)
{	this->SetSizeExpandOnTopLight(DesiredDimension);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i].MakeZero();
}

void root::AssignIntRoot(intRoot& r)
{	this->SetSizeExpandOnTopLight(r.dimension);
	for (int i=0;i<r.dimension;i++)
		this->TheObjects[i].AssignInteger(r.elements[i]);
}

void root::AssignWithoutLastCoordinate(root& right)
{	this->SetSizeExpandOnTopLight(right.size-1);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i].Assign(right.TheObjects[i]);
}

void root::InitFromIntegers(int Dimension,int x1,int x2, int x3,int x4, int x5)
{	this->SetSizeExpandOnTopLight(5);
	this->TheObjects[0].AssignInteger(x1);
	this->TheObjects[1].AssignInteger(x2);
	this->TheObjects[2].AssignInteger(x3);
	this->TheObjects[3].AssignInteger(x4);
	this->TheObjects[4].AssignInteger(x5);
	this->SetSizeExpandOnTopLight(Dimension);
}

void root::InitFromIntegers(int Dimension,int x1,int x2, int x3,int x4, int x5,int x6, int x7, int x8)
{ this->SetSizeExpandOnTopLight(8);
	this->TheObjects[0].AssignInteger(x1);
	this->TheObjects[1].AssignInteger(x2);
	this->TheObjects[2].AssignInteger(x3);
	this->TheObjects[3].AssignInteger(x4);
	this->TheObjects[4].AssignInteger(x5);
	this->TheObjects[5].AssignInteger(x6);
	this->TheObjects[6].AssignInteger(x7);
	this->TheObjects[7].AssignInteger(x8);
	this->SetSizeExpandOnTopLight(Dimension);
}

void root::InitFromIntegers(int Dimension,int x1,int x2, int x3,int x4, int x5,int x6, int x7, int x8, int x9, int x10, int x11, int x12)
{ this->SetSizeExpandOnTopLight(12);
	this->TheObjects[0].AssignInteger(x1);
	this->TheObjects[1].AssignInteger(x2);
	this->TheObjects[2].AssignInteger(x3);
	this->TheObjects[3].AssignInteger(x4);
	this->TheObjects[4].AssignInteger(x5);
	this->TheObjects[5].AssignInteger(x6);
	this->TheObjects[6].AssignInteger(x7);
	this->TheObjects[7].AssignInteger(x8);
	this->TheObjects[8].AssignInteger(x9);
	this->TheObjects[9].AssignInteger(x10);
	this->TheObjects[10].AssignInteger(x11);
	this->TheObjects[11].AssignInteger(x12);
	this->SetSizeExpandOnTopLight(Dimension);
}

int root::HashFunction()
{ int result=0;
	for (int i=0;i<this->size;i++)
		result+=	this->TheObjects[i].HashFunction()*	::SomeRandomPrimes[i];
	return result;
}

inline bool root::IsPositiveOrZero()
{ for (int i=0;i<this->size;i++)
		if (this->TheObjects[i].IsNegative())
			return false;
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

inline void root::MakeNormalInProjectivizationFromAffineHyperplane(affineHyperplane& input)
{ this->MakeNormalInProjectivizationFromPointAndNormal(input.affinePoint, input.normal);
}

void root::MakeNormalInProjectivizationFromPointAndNormal(root& point, root& normal)
{ //the extra dimension is going to be the last dimension
	int newDimension= normal.size+1;
	this->SetSizeExpandOnTopLight(newDimension);
	root::RootScalarEuclideanRoot
			(	normal,point,this->TheObjects[newDimension-1]);
	this->TheObjects[newDimension-1].Minus();
	for (int j=0;j<newDimension-1;j++)
		this->TheObjects[j].Assign(normal.TheObjects[j]);
}

bool root::ProjectToAffineSpace(root &output)
{	if (this->TheObjects[this->size-1].IsEqualToZero())
		return false;
	output.SetSizeExpandOnTopLight(this->size-1);
	for (int i=0;i<this->size-1;i++)
		output.TheObjects[i].Assign(this->TheObjects[i]);
	output.DivByLargeRational(this->TheObjects[this->size-1]);
	return true;
}

bool root::MakeAffineProjectionFromNormal(affineHyperplane& output)
{ int tempI= this->getIndexFirstNonZeroCoordinate();
	if (tempI==this->size-1)
		return false;
	output.affinePoint.MakeZero(this->size-1);
	output.normal.SetSizeExpandOnTopLight(this->size-1);
	output.affinePoint.TheObjects[tempI].Assign(*this->LastObject());
	output.affinePoint.TheObjects[tempI].Minus();
	output.affinePoint.TheObjects[tempI].DivideBy(this->TheObjects[tempI]);
	for (int i=0;i<this->size-1;i++)
		output.normal.TheObjects[i].Assign(this->TheObjects[i]);
	return true;
}

void root::DivByInteger(int a)
{	for (int i =0; i<this->size;i++)
		this->TheObjects[i].DivideByInteger(a);
}

int root::getIndexFirstNonZeroCoordinate()
{	for (int i =0; i<this->size;i++)
		if (!this->TheObjects[i].IsEqualToZero())
			return i;
	return -1;
}

void root::DivByLargeInt(LargeInt& a)
{	for (int i =0; i<this->size;i++)
		this->TheObjects[i].DivideByLargeInteger(a);
}

void root::DivByLargeRational(Rational&a)
{	for (int i =0; i<this->size;i++)
		this->TheObjects[i].DivideBy(a);
}

bool root::HasStronglyPerpendicularDecompositionWRT
	(	roots& theSet, WeylGroup& theWeylGroup, roots& output,
		ListBasicObjects<Rational>& outputCoeffs)
{ if (this->IsEqualToZero())
		return true;
	if (theSet.size==0)
		return false;
	if (output.size==0)
	{	output.MakeActualSizeAtLeastExpandOnTop(theSet.size);
		outputCoeffs.MakeActualSizeAtLeastExpandOnTop(theSet.size);
	}
//	this->ComputeDebugString();
//	theSet.ComputeDebugString();
	roots theNewSet;
	theNewSet.MakeActualSizeAtLeastExpandOnTop(theSet.size);
	root tempRoot;
	Rational tempRat, tempRat2;
	for (int indexFirstNonZeroRoot=0; indexFirstNonZeroRoot<theSet.size; indexFirstNonZeroRoot++)
	{	root& currentRoot = theSet.TheObjects[indexFirstNonZeroRoot];
		theWeylGroup.RootScalarKillingFormMatrixRoot
			(*this,currentRoot,tempRat);
		if (tempRat.IsPositive())
		{	theNewSet.size=0;
			for (int i=indexFirstNonZeroRoot;i<theSet.size;i++)
			{ theWeylGroup.RootScalarKillingFormMatrixRoot
					(currentRoot,theSet.TheObjects[i],tempRat2);
				if (tempRat2.IsEqualToZero())
					theNewSet.AddRoot(theSet.TheObjects[i]);
			}
			tempRoot.Assign(currentRoot);
			tempRoot.MinusRoot();
			tempRoot.MultiplyByLargeRational(tempRat);
			outputCoeffs.AddObjectOnTop(tempRat);
			theWeylGroup.RootScalarKillingFormMatrixRoot
				(currentRoot,currentRoot,tempRat);
			tempRoot.DivByLargeRational(tempRat);
			tempRoot.Add(*this);
			outputCoeffs.LastObject()->DivideBy(tempRat);
			output.AddRoot(currentRoot);
			if (	tempRoot.HasStronglyPerpendicularDecompositionWRT
							(theNewSet,theWeylGroup,output,outputCoeffs))
				return true;
			output.size--;
			outputCoeffs.size--;
		}
	}
	return false;
}

int MatrixLargeRational::FindPositiveLCMCoefficientDenominatorsTruncated()
{ int result=1;
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			result*=(this->elements[i][j].DenShort/Rational::gcdSigned(result,this->elements[i][j].DenShort));
	return result;
}

int MatrixLargeRational::FindPositiveGCDCoefficientNumeratorsTruncated()
{ int result=1;
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			if (this->elements[i][j].NumShort!=0)
				result=Rational::gcdSigned(result,this->elements[i][j].NumShort);
	assert(result!=0);
	if (result<0)
		result=-result;
	return result;
}

void MatrixLargeRational::ScaleToIntegralForMinRationalHeightNoSignChange()
{ Rational tempRat;
	tempRat.AssignNumeratorAndDenominator
		(	this->FindPositiveLCMCoefficientDenominatorsTruncated(),
      this->FindPositiveGCDCoefficientNumeratorsTruncated());
	this->MultiplyByLargeRational(tempRat);
}

void MatrixLargeRational::ComputeDeterminantOverwriteMatrix(Rational &output)
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
		if(tempI!=i){output.Minus();}
		tempRat.Assign(this->elements[i][i]);
		output.MultiplyBy(tempRat);
		tempRat.Invert();
		this->RowTimesScalar(i, tempRat);
		//this->ComputeDebugString();
		for (int j = i+1; j<dim; j++)
		{	if (!this->elements[j][i].IsEqualToZero())
			{ tempRat.Assign(this->elements[j][i]);
				tempRat.Minus();
				this->AddTwoRows (i, j, i, tempRat);
			}
		}
		//this->ComputeDebugString();
	}
}

void MatrixLargeRational::Transpose(GlobalVariables& theGlobalVariables)
{ MatrixLargeRational& tempMat=theGlobalVariables.matTransposeBuffer;
	tempMat.init(this->NumCols,this->NumRows);
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			tempMat.elements[j][i].Assign(this->elements[i][j]);
	this->Assign(tempMat);
}

void MatrixLargeRational::MultiplyByInt(int x)
{ for (int i=0;i<this->NumCols;i++)
		for (int j=0;j<this->NumRows;j++)
			this->elements[j][i].MultiplyByInt(x);
}

void MatrixLargeRational::MultiplyByLargeRational(Rational& x)
{ for (int i=0;i<this->NumCols;i++)
		for (int j=0;j<this->NumRows;j++)
			this->elements[j][i].MultiplyBy(x);
}

void MatrixLargeRational::AssignMatrixIntWithDen(MatrixIntTightMemoryFit &theMat, int Den)
{ this->init(theMat.NumRows,theMat.NumCols);
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			this->elements[i][j].AssignNumeratorAndDenominator(theMat.elements[i][j],Den);
}

void MatrixLargeRational::NonPivotPointsToRoot(Selection& TheNonPivotPoints, int OutputDimension, root& output)
{	int RowCounter=0;
	output.SetSizeExpandOnTopLight(OutputDimension);
	for (int i=0;i<OutputDimension;i++)
	{	if (!TheNonPivotPoints.selected[i])
		{	output.TheObjects[i].MakeZero();
			for (int j=0;j<TheNonPivotPoints.CardinalitySelection;j++ )
				output.TheObjects[i].Subtract(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
			RowCounter++;
		}
		else
			output.TheObjects[i].MakeOne();
	}
}

void MatrixLargeRational::NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, MatrixLargeRational& output)
{	int RowCounter=0;
	output.init(this->NumCols,1);
	//output.ComputeDebugString();
	for (int i=0;i<this->NumCols;i++)
	{	if (!TheNonPivotPoints.selected[i])
		{	output.elements[i][0].MakeZero();
			for (int j=0;j<TheNonPivotPoints.CardinalitySelection;j++ )
				output.elements[i][0].Subtract(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
			RowCounter++;
			//output.ComputeDebugString();
		}
		else
			output.elements[i][0].Assign(ROne);
		//output.ComputeDebugString();
	}
}

void MatrixLargeRational::ComputeDebugString()
{ this->ElementToSting(this->DebugString);
}

int MathRoutines::lcm(int a, int b)
{ if (a<0){a=-a;}
	if (b<0){b=-b;}
	return ((a*b)/Rational::gcdSigned(a,b));
}

int MathRoutines::TwoToTheNth(int n)
{	int result=1;
	for(int i=0;i<n;i++){result*=2;}
	return result;
}

int MathRoutines::KToTheNth(int k, int n)
{	int result=1;
	for(int i=0;i<n;i++){result*=k;}
	return result;
}

int MathRoutines::BinomialCoefficientMultivariate(int N, ListBasicObjects<int> &theChoices)
{ int ChoiceIndex=0;
	int denominator=1;
	int result=0;
	for( int i=N;i>0;i--)
	{ result*=i;
		result/=denominator;
		denominator++;
		if (denominator>theChoices.TheObjects[ChoiceIndex])
		{ denominator=1;
			ChoiceIndex++;
		}
	}
	return result;
}

void ProjectOntoHyperPlane(root& input, root& normal, root& ProjectionDirection, root&output)
{	Rational t;
	Rational tempRat;
	root::RootScalarEuclideanRoot(input,normal,t);
	root::RootScalarEuclideanRoot(ProjectionDirection,normal, tempRat);
	t.DivideBy(tempRat);
	assert(t.IsNonNegative());
	t.Minus();
	root::RootPlusRootTimesScalar(input,ProjectionDirection,t,output);
}

void Selection::init(int maxNumElements)
{	if (maxNumElements>0 && maxNumElements!=this->MaxSize)
	{	delete [] selected;
		delete [] elements;
		//delete [] elementsInverseSelection;
		this->selected = new bool[maxNumElements];
		this->elements = new int[maxNumElements];
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter+=(maxNumElements-this->MaxSize)*2;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
		MaxSize =maxNumElements;
	}
	for (int i=0; i<this->MaxSize;i++)
		this->selected[i]=false;
	this->CardinalitySelection=0;
}

void roots::SubSelection(Selection& theSelection, roots& output)
{ assert(&output!=this);
	output.SetSizeExpandOnTopNoObjectInit(theSelection.CardinalitySelection);
	for(int i=0;i<theSelection.CardinalitySelection;i++)
		output.TheObjects[i].Assign(this->TheObjects[theSelection.elements[i]]);
}

void roots::SelectionToMatrix(Selection& theSelection, int OutputDimension, MatrixLargeRational& output)
{	output.init((short)OutputDimension,(short)theSelection.CardinalitySelection);
	this->SelectionToMatrixAppend(theSelection,OutputDimension,output,0);
}

void roots::SelectionToMatrixAppend
				(Selection& theSelection, int OutputDimension, MatrixLargeRational& output, int StartRowIndex)
{ for(int i=0;i<theSelection.CardinalitySelection;i++)
	{	root& tempRoot=this->TheObjects[theSelection.elements[i]];
		for (int j=0;j<OutputDimension;j++)
			output.elements[StartRowIndex+i][j].Assign(tempRoot.TheObjects[j]);
	}
}


void roots::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void roots::ElementToString(std::string& output)
{ this->ElementToString(output,false);
}

void roots::ElementToString(std::string& output, bool useLaTeX)
{	std::stringstream out;
	std::string tempS;
	if (! useLaTeX)
    out<<"Num roots: "<<this->size<<"\n";
	for (int i=0; i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempS,useLaTeX);
		out<<tempS;
		if (i!=this->size-1)
		{ if (useLaTeX)
        out<<", ";
      else
        out<<"\n";
		}
	}
	output = out.str();
}

void roots::rootsToMatrix(MatrixLargeRational& output)
{ short tempNumCols= 0;
	if (this->size!=0)
		tempNumCols=(short)this->TheObjects[0].size;
	output.init((short) this->size,tempNumCols);
	for (int i=0;i<this->size;i++)
		for (int j=0;j<tempNumCols;j++)
			output.elements[i][j].Assign(this->TheObjects[i].TheObjects[j]);
}


void Selection::AddSelectionAppendNewIndex(int index)
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
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter-=this->MaxSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
	//	delete [] elementsInverseSelection;
	this->MaxSize=0;
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
{	MaxSize--;
	if (selected[MaxSize])
		CardinalitySelection--;
}

void Selection::ExpandMaxSize()
{	elements[CardinalitySelection]=MaxSize;
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
		out << this->elements[i]<<", ";
	output=out.str();
}

void Selection::incrementSelection()
{	for (int i=MaxSize-1;i>=0; i--)
	{	selected[i]=!selected[i];
		if (selected[i])
		{	ComputeIndicesFromSelection();
			return;
		}
	}
	ComputeIndicesFromSelection();
}

void Selection::incrementSelectionFixedCardinality(int card)
{	if (card>MaxSize)
		return;
	if (CardinalitySelection!=card)
	{	initNoMemoryAllocation();
		for (int i=0;i<card;i++)
		{	this->selected[i]=true;
			this->elements[i]=i;
		}
		CardinalitySelection=card;
		return;
	}
	if(card==MaxSize|| card==0){return;}
	int IndexLastZeroWithOneBefore=-1;
	int NumOnesAfterLastZeroWithOneBefore=0;
	for(int i=MaxSize-1;i>=0;i--)
	{	if (selected[i])
		{	if (IndexLastZeroWithOneBefore==-1)
				NumOnesAfterLastZeroWithOneBefore++;
			else
				break;
		}
		else
			IndexLastZeroWithOneBefore=i;
	}
	if (IndexLastZeroWithOneBefore==0){return;}
	for(int i=0;i<NumOnesAfterLastZeroWithOneBefore+1;i++)
		selected[elements[CardinalitySelection-i-1]]=false;
	for(int i=0;i<NumOnesAfterLastZeroWithOneBefore+1;i++)
	{	selected[i+ IndexLastZeroWithOneBefore]=true;
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

void Selection::initNoMemoryAllocation()
{	for (int i = 0; i<this->CardinalitySelection; i++)
		this->selected[this->elements[i]]=false;
	CardinalitySelection = 0;
}

void Selection::MakeSubSelection(Selection &theSelection, Selection &theSubSelection)
{ this->init(theSelection.MaxSize);
	for(int i=0; i<theSubSelection.CardinalitySelection;i++)
		this->AddSelectionAppendNewIndex(theSelection.elements[theSubSelection.elements[i]]);
}



void Selection::Assign(const Selection& right)
{ if (this==&right)
		return;
	if (this->MaxSize!=right.MaxSize)
		this->init(right.MaxSize);
	else
		this->initNoMemoryAllocation();
	for (int i=0; i<right.CardinalitySelection;i++)
	{ this->elements[i]=right.elements[i];
		this->selected[this->elements[i]]=true;
	}
	this->CardinalitySelection=right.CardinalitySelection;
}

int Selection::HashFunction()
{ int tempMin=Minimum(SomeRandomPrimesSize, this->MaxSize);
  int result=0;
  for (int i=0;i<tempMin;i++)
    if (this->selected[i])
      result+=i*SomeRandomPrimes[i];
  return result;
}

bool Selection::operator==(Selection& right)
{ if (this->MaxSize!=right.MaxSize || this->CardinalitySelection!=right.CardinalitySelection)
    return false;
  for (int i=0;i<this->CardinalitySelection;i++)
  { if (this->selected[this->elements[i]]!=right.selected[this->elements[i]])
      return false;
  }
  return true;
}

void root::RootPlusRootTimesScalar(root& r1, root& r2, Rational& rat, root& output)
{	static Rational tempRat;
	assert(r1.size==r2.size);
	output.Assign(r1);
	for (int i=0;i<r1.size;i++)
	{	tempRat.Assign(r2.TheObjects[i]);
		tempRat.MultiplyBy(rat);
		output.TheObjects[i].Add(tempRat);
		//r1.ComputeDebugString();
	}
}

void root::RootScalarRoot(root& r1, root& r2, MatrixLargeRational& KillingForm, Rational& output)
{	static Rational tempRat;
	assert(r1.size==KillingForm.NumRows && r1.size==r2.size && r1.size==KillingForm.NumCols);
	output.MakeZero();
	for (int i=0; i<KillingForm.NumRows; i++)
	{	for(int j =0; j<KillingForm.NumCols; j++)
		{	tempRat.Assign(r1.TheObjects[i]);
			tempRat.MultiplyBy(KillingForm.elements[i][j]);
			tempRat.MultiplyBy(r2.TheObjects[j]);
			output.Add(tempRat);
		}
	}
}

void root::RootScalarEuclideanRoot(const root& r1, const  root& r2, Rational& output)
{	static Rational tempRat;
	assert(r1.size==r2.size);
	output.MakeZero();
	for(int i=0;i<r1.size;i++)
	{	tempRat.Assign(r1.TheObjects[i]);
		tempRat.MultiplyBy(r2.TheObjects[i]);
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

void roots::PerturbVectorToRegular
	(	root& output, GlobalVariables& theGlobalVariables, int theDimension,
		FeedDataToIndicatorWindow reportFunction)
{ root normal;
	while (!this->IsRegular(output,normal, theGlobalVariables,theDimension))
	{ IndicatorWindowGlobalVariables.flagRootIsModified=true;
		normal.DivByInteger(10);
		output.Add(normal);
	}
	if (IndicatorWindowGlobalVariables.flagRootIsModified)
	{ output.ScaleToIntegralMinHeight();
		IndicatorWindowGlobalVariables.modifiedRoot.AssignRoot(output);
		IndicatorWindowGlobalVariables.ProgressReportString5="Indicator modified to regular";
		if (reportFunction!=0)
			reportFunction(IndicatorWindowGlobalVariables);
	}
}

int roots::GetRankOfSpanOfElements(GlobalVariables& theGlobalVariables)
{ Selection& NonPivotPoints= theGlobalVariables.selGetRankOfSpanOfElements;
	MatrixLargeRational& tempMatrix=theGlobalVariables.matGetRankOfSpanOfElements;
	if (this->size==0)
		return 0;
	int theDimension= this->TheObjects[0].size;
	this->GaussianEliminationForNormalComputation(tempMatrix,NonPivotPoints,theDimension);
	return (theDimension-NonPivotPoints.CardinalitySelection);
}

bool roots::GetMinLinearDependenceWithNonZeroCoefficientForFixedIndex
	(MatrixLargeRational &outputTheLinearCombination, int theIndex)
{
	return false;
}


void roots::GetLinearDependenceRunTheLinearAlgebra
	(	MatrixLargeRational &outputTheLinearCombination,
		MatrixLargeRational &outputTheSystem,
		Selection &outputNonPivotPoints)
{ if (this->size==0)
		return;
	short Dimension=(short) this->TheObjects[0].size;
	outputTheSystem.init(Dimension,(short)this->size);
	MatrixLargeRational matOutputEmpty;
	matOutputEmpty.init(-1,-1);
	for(int i=0;i<this->size;i++)
		for(int j=0;j<Dimension;j++)
			outputTheSystem.elements[j][i].Assign(this->TheObjects[i].TheObjects[j]);
	//tempMat.ComputeDebugString();
	MatrixLargeRational::GaussianEliminationByRows(outputTheSystem,matOutputEmpty,outputNonPivotPoints);
	//tempMat.ComputeDebugString();
}

bool roots::GetLinearDependence(MatrixLargeRational& outputTheLinearCombination)
{	MatrixLargeRational tempMat;
	Selection nonPivotPoints;
	this->GetLinearDependenceRunTheLinearAlgebra(outputTheLinearCombination,tempMat,nonPivotPoints);
	if (nonPivotPoints.CardinalitySelection==0)
		return false;
	tempMat.NonPivotPointsToEigenVector(nonPivotPoints,outputTheLinearCombination);
	//outputTheLinearCombination.ComputeDebugString();
	return true;
}

void roots::GaussianEliminationForNormalComputation(MatrixLargeRational& inputMatrix,
																									 Selection& outputNonPivotPoints, int theDimension)
{	inputMatrix.init((short)this->size,(short)theDimension);
	MatrixLargeRational matOutputEmpty;
	matOutputEmpty.init(-1,-1);
	outputNonPivotPoints.init(theDimension);
	for(int i=0;i<this->size;i++)
		for(int j=0;j<theDimension;j++)
			inputMatrix.elements[i][j].Assign(this->TheObjects[i].TheObjects[j]);
	MatrixLargeRational::GaussianEliminationByRows(inputMatrix,matOutputEmpty,outputNonPivotPoints);
}

void roots::ComputeNormal(root& output)
{	if (this->size==0)
		return;
	int theDimension= this->TheObjects[0].size;
	MatrixLargeRational tempMatrix;
	Selection NonPivotPoints;
	NonPivotPoints.init(theDimension);
	output.SetSizeExpandOnTopLight(theDimension);
	this->GaussianEliminationForNormalComputation(tempMatrix,NonPivotPoints,theDimension);
	if (NonPivotPoints.CardinalitySelection!=1) {return;}
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,theDimension,output);
}

bool roots::IsRegular(root &r, GlobalVariables& theGlobalVariables, int theDimension)
{ root tempRoot;
	return this->IsRegular(r,tempRoot,theGlobalVariables,theDimension);
}

bool roots::IsRegular
	(	root& r, root& outputFailingNormal, GlobalVariables& theGlobalVariables,
		int theDimension)
{ Selection& WallSelection=theGlobalVariables.selWallSelection;
	WallSelection.init(this->size);
	int x= MathRoutines::NChooseK(this->size, theDimension-1);
	for (int i=0;i<x;i++)
	{ WallSelection.incrementSelectionFixedCardinality(theDimension-1);
		root tempRoot;
		if (this->ComputeNormalFromSelection
					(tempRoot,WallSelection, theGlobalVariables, theDimension))
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

bool roots::ComputeNormalExcludingIndex(root& output, int index, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational& tempMatrix=theGlobalVariables.matComputeNormalExcludingIndex;
	MatrixLargeRational matOutputEmpty;
	Selection& NonPivotPoints=theGlobalVariables.selComputeNormalExcludingIndex;
	if (this->size==0) return false; int theDimension= this->TheObjects[0].size;
	output.SetSizeExpandOnTopLight(theDimension);
	tempMatrix.init((short)this->size-1,(short)theDimension);
	matOutputEmpty.init(-1,-1);
	int k=-1;
	for(int i=0;i<this->size;i++)
		if (i!=index)
		{	k++;
			for(int j=0;j<theDimension;j++)
				tempMatrix.elements[k][j].Assign(this->TheObjects[i].TheObjects[j]);
		}
	tempMatrix.ComputeDebugString();
	MatrixLargeRational::GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,theDimension,output);
	return true;
}

bool roots::ComputeNormalFromSelection
	(root& output, Selection& theSelection, GlobalVariables& theGlobalVariables, int theDimension)
{	MatrixLargeRational& tempMatrix= theGlobalVariables.matComputeNormalFromSelection;
	MatrixLargeRational& matOutputEmpty=theGlobalVariables.matOutputEmpty;
	Selection& NonPivotPoints=theGlobalVariables.selComputeNormalFromSelection;
	matOutputEmpty.init(-1,-1);
	output.SetSizeExpandOnTopLight(theDimension);
	tempMatrix.init((short)theSelection.CardinalitySelection,(short)theDimension);
	for(int i=0;i<theSelection.CardinalitySelection;i++)
	{	for(int j=0;j<theDimension;j++)
			tempMatrix.elements[i][j].Assign
				(this->TheObjects[theSelection.elements[i]].TheObjects[j]);

	}
	MatrixLargeRational::GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,theDimension,output);
	return true;
}

bool roots::ComputeNormalFromSelectionAndExtraRoot
	(root& output,root& ExtraRoot,Selection& theSelection, GlobalVariables& theGlobalVariables)
{	if (this->size==0)
		return false;
	int theDimension= this->TheObjects[0].size;
	output.SetSizeExpandOnTopLight(theDimension);
	MatrixLargeRational& tempMatrix=
		theGlobalVariables.matComputeNormalFromSelectionAndExtraRoot;
	MatrixLargeRational	matOutputEmpty;
	Selection& NonPivotPoints=theGlobalVariables.selComputeNormalFromSelectionAndExtraRoot;
	tempMatrix.init((short)theSelection.CardinalitySelection+1,(short)theDimension);
	matOutputEmpty.init(-1,-1);
	for(int j=0;j<theDimension;j++)
	{	for(int i=0;i<theSelection.CardinalitySelection;i++)
			tempMatrix.elements[i][j].Assign
				(this->TheObjects[theSelection.elements[i]].TheObjects[j]);
		tempMatrix.elements[theSelection.CardinalitySelection][j].Assign
			(ExtraRoot.TheObjects[j]);
	}
	MatrixLargeRational::GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,theDimension,output);
	return true;
}


bool roots::ComputeNormalFromSelectionAndTwoExtraRoots
			(root& output,root& ExtraRoot1, root& ExtraRoot2, Selection& theSelection, GlobalVariables& theGlobalVariables)
{	MatrixLargeRational& tempMatrix=
		theGlobalVariables.matComputeNormalFromSelectionAndTwoExtraRoots;
	MatrixLargeRational matOutputEmpty;
	Selection& NonPivotPoints=
		theGlobalVariables.selComputeNormalFromSelectionAndTwoExtraRoots;
	if (this->size==0) return false; int theDimension= this->TheObjects[0].size;
	output.SetSizeExpandOnTopLight(theDimension);
	matOutputEmpty.init(-1,-1);
	tempMatrix.init((short)theSelection.CardinalitySelection+2,(short)theDimension);
	for(int j=0;j<theDimension;j++)
	{	for(int i=0;i<theSelection.CardinalitySelection;i++)
			tempMatrix.elements[i][j].Assign
				(this->TheObjects[theSelection.elements[i]].TheObjects[j]);
		tempMatrix.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot1.TheObjects[j]);
		tempMatrix.elements[theSelection.CardinalitySelection+1][j].Assign(ExtraRoot2.TheObjects[j]);
	}
	MatrixLargeRational::GaussianEliminationByRows(tempMatrix,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection!=1) return false;
	tempMatrix.NonPivotPointsToRoot(NonPivotPoints,theDimension,output);
	return true;
}

void roots::AssignMatrixRows(MatrixLargeRational& mat)
{ this->size=0;
	root tempRoot;
	tempRoot.SetSizeExpandOnTopLight(mat.NumCols);
	for (int i=0;i<mat.NumRows;i++)
	{ for (int j=0;j<mat.NumCols;j++)
			tempRoot.TheObjects[j].Assign(mat.elements[i][j]);
		this->AddRoot(tempRoot);
	}
}

void roots::Average(root& output, int theDimension)
{	output.MakeZero(theDimension);
	if (this->size==0){return;}
	for (int i=0;i<this->size;i++)
		output.Add(this->TheObjects[i]);
	output.DivByInteger(this->size);
}

void roots::AddRootSnoRepetition(roots &r)
{	for (int i=0;i<r.size;i++)
		this->AddRootNoRepetition(r.TheObjects[i]);
}

bool roots::operator ==(const roots& right)
{ if (this->size!= right.size)
		return false;
	for (int i=0;i<right.size;i++)
	{ if (!this->TheObjects[i].IsEqualTo(right.TheObjects[i]))
			return false;
	}
	return true;
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
{	if (this->IndexOfObject(r)==-1)
	{	this->AddObjectOnTop(r);
		return true;
	}
	return false;
}

int roots::GetDimensionOfElements()
{ if (this->size==0)
		return -1;
	int result= this->TheObjects[0].size;
	for(int i=1;i<this->size;i++)
		assert(this->TheObjects[i].size==result);
	return result;
}

bool roots::ContainsARootConnectedTo(root& input, WeylGroup& theWeyl)
{ Rational tempRat;
	for (int i=0;i<this->size;i++)
	{	theWeyl.RootScalarKillingFormMatrixRoot(this->TheObjects[i],input,tempRat);
		if(!tempRat.IsEqualToZero())
			return true;
	}
	return false;
}

int roots::ArrangeFirstVectorsBeOfMaxPossibleRank(GlobalVariables& theGlobalVariables)
{	if (this->size==0)
		return 0;
	int theDimension= this->GetDimensionOfElements();
	Selection NonPivotPoints;
	MatrixLargeRational tempMatrix;
	roots tempRoots;
	int oldRank=0;
	for (int i=0;i<this->size;i++)
	{ tempRoots.AddObjectOnTop(this->TheObjects[i]);
		int newRank= tempRoots.GetRankOfSpanOfElements(theGlobalVariables);
		if (newRank==oldRank)
			tempRoots.PopIndexSwapWithLast(tempRoots.size-1);
		else
		{	this->SwapTwoIndices(oldRank, i);
			assert(oldRank+1==newRank);
			oldRank=newRank;
		}
		if (oldRank== theDimension)
			return theDimension;
	}
	return (oldRank);
}

inline bool Rational::IsEqualTo(const Rational& b)
{	if (this->Extended==0 && b.Extended==0)
    return (this->NumShort*b.DenShort==b.NumShort*this->DenShort);
  static Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(b);
  return tempRat.IsEqualToZero();
}

inline bool Rational::IsGreaterThanOrEqualTo(Rational &right)
{	if (this->Extended==0 && right.Extended==0)
    return (this->NumShort*right.DenShort>=right.NumShort*this->DenShort);
  static Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(right);
	return tempRat.IsNonNegative();
}

inline void Rational::ElementToString(std::string& output)
{	std::stringstream out;
  if (this->Extended==0)
	{ out <<this->NumShort;
    if (this->DenShort!=1)
    {	out <<"/"<<this->DenShort;
    }
	} else
	{ std::string tempS;
	  this->Extended->num.ElementToString(tempS);
	  out << tempS;
	  this->Extended->den.ElementToString(tempS);
	  if (tempS!="1")
      out<<"/"<<tempS;
  }
  output = out.str();
}

void CombinatorialChamber::AddInternalWall
	(	root& TheKillerFacetNormal, root& TheFacetBeingKilledNormal,
		root &direction, CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables )
{	static Rational tempRat;
	root::RootScalarEuclideanRoot(direction, TheFacetBeingKilledNormal,tempRat);
	root tempRoot; tempRoot.Assign(TheKillerFacetNormal);
	tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
	if ( !owner->flagMakingASingleHyperplaneSlice &&tempRat.IsPositive())
		this->InternalWalls.AddRootNoRepetition(tempRoot);
	if (owner->flagMakingASingleHyperplaneSlice && !this->flagExplored )
		if (!owner->flagSliceWithAWallIgnorePermanentlyZero || !this->flagPermanentlyZero)
			owner->PreferredNextChambers.AddObjectOnTopNoRepetitionOfObject(this->IndexInOwnerComplex);
}

bool CombinatorialChamber::CheckSplittingPointCandidate
	(Selection &SelectionTargetSimplex, Selection &SelectionStartSimplex, MatrixLargeRational& outputColumn, int Dimension)
{	for (int i=0; i<SelectionStartSimplex.CardinalitySelection;i++)
	{	if (outputColumn.elements[i][0].IsNonPositive())
			return false;
	}
	if (outputColumn.elements[Dimension-1 ][0].IsNonPositive())
		return false;
	return true;
}

bool CombinatorialChamber::ComputeDebugString( CombinatorialChamberContainer* owner)
{ this->ElementToString(this->DebugString,owner);
	return true;
}

void CombinatorialChamber::ChamberNumberToStringStream(std::stringstream& out, CombinatorialChamberContainer& owner)
{ if (this->flagHasZeroPolynomial)
	{ out<< "Invisible";
	} else
	{	out <<"c";
	}
	if (this->DisplayNumber!=-1)
		out<<this->DisplayNumber;
	else
		out<<this->CreationNumber;
}

bool CombinatorialChamber::ElementToString
	(std::string& output, CombinatorialChamberContainer* owner)
{ return this->ElementToString(output,owner,false);
}

bool CombinatorialChamber::ElementToString
	(std::string& output, CombinatorialChamberContainer* owner, bool LatexFormat)
{	std::stringstream out;
	//assert(this->ExternalWalls->size== this->ExternalWallsNormals.size);
	std::string endOfLine;
	if (LatexFormat)
		endOfLine.assign("\\\\\n");
	else
		endOfLine.assign("\n");
	this->ChamberNumberToStringStream(out,*owner);
	if (LatexFormat)
		out <<endOfLine<< "Projective representation";
	out <<endOfLine<<"External Walls:"<<endOfLine;
	root tempNormal;
	std::string tempS;
	for (int i=0;i<this->Externalwalls.size;i++)
	{	this->Externalwalls.TheObjects[i].ElementToString(tempS);
		if (!LatexFormat)
			out <<"f"<<i<<": "<< tempS<<endOfLine;
		else
			out << tempS <<endOfLine;
	}
	if (!LatexFormat)
	{	out <<"Internal Walls:"<<endOfLine;
		for (int i=0;i<this->InternalWalls.size;i++)
		{	this->InternalWalls.TheObjects[i].ElementToString(tempS);
			out <<"f"<<i<<": "<< tempS<<endOfLine;
		}
	}
	ListObjectPointers<CombinatorialChamber> outputChambers;
	this->FindAllNeighbors(outputChambers);
	out<<"Neighbors: ";
	for (int i=0;i<outputChambers.size;i++)
	{	if (outputChambers.TheObjects[i]!=0)
		{	outputChambers.TheObjects[i]->ChamberNumberToStringStream(out,*owner);
			out <<", ";
		}
	}
	PolyFormatLocal.cutOffString=false;
	if (this->flagIncludeVerticesInDebugString)
	{ out << endOfLine<<"Vertices: ";
		for (int i=0;i<this->AllVertices.size;i++)
		{ this->AllVertices.TheObjects[i].ElementToString(tempS);
			out <<"("<< tempS <<"), ";
		}
	}
	out<<endOfLine;
	if (!owner->flagDrawingProjective)
	{	if (LatexFormat)
		out << "Affine data"<< endOfLine;
		out<<"Affine walls: "<<endOfLine;
		for (int i=0;i<this->affineExternalWalls.size;i++)
		{ this->affineExternalWalls.TheObjects[i].ElementToString(tempS);
			out << tempS <<endOfLine;
		}
		out << "Affine vertices (true): "<<endOfLine;
		for (int i=0;i<affineVertices.size;i++)
		{	if (i==this->NumTrueAffineVertices)
				out << "Affine vertices representing pt(s) at infty: "<<endOfLine;
			this->affineVertices.TheObjects[i].ElementToString(tempS);
			out << tempS<<endOfLine;
		}
	}
	output=out.str();
	return true;
}

bool CombinatorialChamber::ConsistencyCheck(int theDimension)
{ for (int i=0;i<this->Externalwalls.size;i++)
	{ if (!this->Externalwalls.TheObjects[i].ConsistencyCheck(this))
			return false;
	}
	if (this->flagPermanentlyZero)
		return true;
	if (CombinatorialChamber::DisplayingGraphics)
	{ for (int i=0;i<this->Externalwalls.size;i++)
		{ Rational tempRat;
			this->ComputeInternalPointMethod2(this->InternalPoint,theDimension);
			root::RootScalarEuclideanRoot(this->InternalPoint,this->Externalwalls.TheObjects[i].normal,tempRat);
			if (tempRat.IsNonPositive())
			{ assert(false);
				return false;
			}
		}
	}
	return true;
}

CombinatorialChamber::CombinatorialChamber()
{	this->flagHasZeroPolynomial=true;
  this->IndexInOwnerComplex=-1;
  this->IndexStartingCrossSectionNormal=-1;
	this->flagExplored=false;
	this->DisplayNumber=-1;
	this->flagPermanentlyZero=false;
	this->NumTrueAffineVertices=-1;
}

bool CombinatorialChamber::ProjectToDefaultAffineSpace
	(CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables)
{ this->affineExternalWalls.MakeActualSizeAtLeastExpandOnTop(this->Externalwalls.size);
	this->affineVertices.MakeActualSizeAtLeastExpandOnTop(this->AllVertices.size);
	this->affineExternalWalls.size=0;
	this->affineVertices.size=0;
	root tempCrossSectionNormal, tempAffinePoint;
	tempCrossSectionNormal.AssignWithoutLastCoordinate
		(owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].normal);
	tempAffinePoint.AssignWithoutLastCoordinate
		(owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].affinePoint);
	for (int i=0;i<this->Externalwalls.size;i++)
	{ affineHyperplane tempAH;
		if (this->Externalwalls.TheObjects[i].normal.MakeAffineProjectionFromNormal(tempAH))
			this->affineExternalWalls.AddObjectOnTop(tempAH);
	}
	Rational tempScalarProd1, tempRat;
	root::RootScalarEuclideanRoot
		(tempCrossSectionNormal,tempAffinePoint,tempScalarProd1);
	owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].normal.ComputeDebugString();
	owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].affinePoint.ComputeDebugString();
	//this->ComputeDebugString(owner);
	root tempRoot;
	for (int i=0;i<this->AllVertices.size;i++)
	{ if (this->AllVertices.TheObjects[i].ProjectToAffineSpace(tempRoot))
		{ root::RootScalarEuclideanRoot
				(	tempCrossSectionNormal,tempRoot, tempRat);
			if (tempRat.IsEqualTo(tempScalarProd1))
			{	//for (int i=0;i<owner->StartingCrossSections.size;i++)
				{	owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].affinePoint.MultiplyByInteger(2);
//					owner->StartingCrossSections.TheObjects[i].affinePoint.DivByInteger(3);
				}
				return false;
			}
			this->affineVertices.AddObjectOnTop(tempRoot);
		}
	}
	this->NumTrueAffineVertices= this->affineVertices.size;
	for (int i=0;i<this->AllVertices.size;i++)
	{ if (!this->AllVertices.TheObjects[i].ProjectToAffineSpace(tempRoot))
		{ Selection tempSel;
			this->findWallsIncidentWithVertexExcludeWallAtInfinity(this->AllVertices.TheObjects[i],tempSel, owner);
			assert(	tempSel.CardinalitySelection>=owner->AmbientDimension-2);
			this->ComputeAffineInfinityPointApproximation(tempSel,owner,theGlobalVariables);
		}
	}
	//this->ComputeDebugString(owner);
	return true;
}

void CombinatorialChamber::PurgeInternalWalls()
{ this->InternalWalls.size=0;
}

void CombinatorialChamber::ComputeAffineInfinityPointApproximation
	(Selection& selectedExternalWalls,CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables)
{ root candidateVertex;
	Selection& subSelection = theGlobalVariables.selComputeAffineInfinityPointApproximation1;
	Selection& tempSel = theGlobalVariables.selComputeAffineInfinityPointApproximation2;
	tempSel.init(selectedExternalWalls.CardinalitySelection);
	int tempI = MathRoutines::NChooseK
		(selectedExternalWalls.CardinalitySelection, owner->AmbientDimension-2);
	for (int i=0;i<tempI;i++)
	{ tempSel.incrementSelectionFixedCardinality(owner->AmbientDimension-2);
		tempSel.ComputeDebugString();
		subSelection.MakeSubSelection(selectedExternalWalls,tempSel);
		if (this->LinearAlgebraForVertexComputationOneAffinePlane(subSelection,candidateVertex,theGlobalVariables,owner))
			this->affineVertices.AddObjectOnTop(candidateVertex);
	}
}

void CombinatorialChamber::findWallsIncidentWithVertexExcludeWallAtInfinity
	(root& theVertex, Selection &output, CombinatorialChamberContainer* owner)
{ output.init(this->Externalwalls.size);
	root tempRoot;
	tempRoot.MakeZero(owner->AmbientDimension);
	tempRoot.LastObject()->MakeOne();
	for (int i=0;i<this->Externalwalls.size;i++)
	{ if (	theVertex.OurScalarProductIsZero(this->Externalwalls.TheObjects[i].normal)
				&&!this->Externalwalls.TheObjects[i].normal.IsEqualTo(tempRoot))
			output.AddSelectionAppendNewIndex(i);
	}
}


int CombinatorialChamber::getIndexInfiniteHyperplane(CombinatorialChamberContainer* owner)
{ root tempRoot; tempRoot.MakeZero(owner->AmbientDimension);
	tempRoot.LastObject()->MakeOne();
	for(int i=0;i<this->Externalwalls.size;i++)
		if (this->Externalwalls.TheObjects[i].normal.IsEqualTo(tempRoot))
			return i;
	return -1;
}

int CombinatorialChamber::getIndexVertexIncidentWithSelection(Selection&theSel)
{ for (int i=0;i<this->AllVertices.size;i++)
	{ if (this->VertexIsIncidentWithSelection(this->AllVertices.TheObjects[i],theSel))
			return i;
	}
	return -1;
}

bool CombinatorialChamber::VertexIsIncidentWithSelection(root& VertexCandidate, Selection& theSel)
{ for(int i=0; i<theSel.CardinalitySelection;i++)
	{ if (!this->Externalwalls.TheObjects[theSel.elements[i]]
					.normal.OurScalarProductIsZero(VertexCandidate))
			return false;
	}
	return true;
}

bool CombinatorialChamber::OwnsAWall(WallData* theWall)
{ for (int i=0;i<this->Externalwalls.size;i++)
	{ if (&this->Externalwalls.TheObjects[i]==theWall)
			return true;
	}
	return false;
}


void CombinatorialChamber::ComputeInternalPointMethod2(root &InternalPoint, int theDimension)
{	InternalPoint.MakeZero(theDimension);
	for (int i=0;i<this->AllVertices.size;i++)
		InternalPoint.Add(this->AllVertices.TheObjects[i]);
	InternalPoint.DivByInteger(this->AllVertices.size);
}

void CombinatorialChamber::ComputeAffineInternalPoint(root& outputPoint, int theDimension)
{ this->affineVertices.Average(outputPoint,theDimension);
}

void CombinatorialChamber::LabelWallIndicesProperly()
{ for (int i=0;i<this->Externalwalls.size;i++)
	{ this->Externalwalls.TheObjects[i].indexInOwnerChamber=i;
	}
}

void CombinatorialChamber::drawOutputAffine
	(	DrawingVariables& TDV,CombinatorialChamberContainer& owner,
		std::fstream* LaTeXoutput,drawLineFunction theDrawFunction,drawTextFunction drawTextIn)
{ if (!TDV.DrawingInvisibles && this->flagHasZeroPolynomial)
		return;
	TDV.ApplyScale(0.3);
	for (int i=0;i<this->affineVertices.size;i++)
  { for(int j=0;j<this->affineVertices.size;j++)
    { affineHyperplane* AreInAWall=0;
      for (int k=0;k<this->affineExternalWalls.size;k++)
      { if (this->affineExternalWalls.TheObjects[k].ContainsPoint(this->affineVertices.TheObjects[i])
            &&
            this->affineExternalWalls.TheObjects[k].ContainsPoint(this->affineVertices.TheObjects[j]))
        { AreInAWall=&this->affineExternalWalls.TheObjects[k];
          break;
        }
      }
      if (AreInAWall!=0)
			{ int color = TDV.GetColorFromChamberIndex(this->IndexInOwnerComplex,LaTeXoutput);
				int penStyle = TDV.DrawStyle;
				if (this->flagHasZeroPolynomial || this->flagPermanentlyZero)
					penStyle= TDV.DrawStyleInvisibles;
				if (owner.AffineWallsOfWeylChambers.ContainsObjectHash(*AreInAWall)!=-1)
					color= TDV.ColorWeylChamberWalls;
				TDV.drawlineBetweenTwoVectors
					(	this->affineVertices.TheObjects[i],
						this->affineVertices.TheObjects[j],penStyle,color,LaTeXoutput,
						theDrawFunction);
				root tempRoot; this->ComputeAffineInternalPoint(tempRoot,owner.AmbientDimension-1);
				std::stringstream out;
				out << this->DisplayNumber;
				std::string tempS;
				tempS=out.str();
				if (! this->flagHasZeroPolynomial)
					color=TDV.TextColor;
				else
					color= TDV.ZeroChamberTextColor;
        if(TDV.DrawChamberIndices)
          TDV.drawTextAtVector(tempRoot,tempS, color, LaTeXoutput, drawTextIn);
      }
    }
  }
}

void CombinatorialChamber::ComputeVerticesFromNormals
	(CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables)
{	for (int i =0;i<this->Externalwalls.size;i++)
	{	this->AllVertices.size=0;
	}
	Selection theSelection;
	theSelection.init(this->Externalwalls.size);
	root VertexCandidate;
	int NumCandidates = MathRoutines::NChooseK(this->Externalwalls.size, owner.AmbientDimension-1);
	for (int i=0;i<NumCandidates; i++)
	{	theSelection.incrementSelectionFixedCardinality(owner.AmbientDimension-1);
		if (this->LinearAlgebraForVertexComputation
					(theSelection,VertexCandidate,theGlobalVariables,owner.AmbientDimension))
		{	if (this->PlusMinusPointIsInChamber(VertexCandidate))
			{	for (int j=0;j<theSelection.CardinalitySelection;j++ )
				{	this->ScaleVertexToFitCrossSection(VertexCandidate,owner);
					this->AllVertices.AddRootNoRepetition(VertexCandidate);
				}
			}
		}
	}
}

bool CombinatorialChamber::ComputeVertexFromSelection
	( GlobalVariables& theGlobalVariables, root& output, Selection& theSelection, int theDimension)
{ if (this->LinearAlgebraForVertexComputation
					(theSelection,output,theGlobalVariables,theDimension))
	{ if (this->PlusMinusPointIsInChamber(output))
			return true;
	}
	return false;
}

bool CombinatorialChamber::PlusMinusPointIsInChamber(root&point)
{	if (PointIsInChamber(point))
	{ return true;
	}
	point.MinusRoot();
	if (PointIsInChamber(point))
	{	return true;
	}
	return false;
}

bool CombinatorialChamber::ScaleVertexToFitCrossSection
	(root& point, CombinatorialChamberContainer& owner)
{	static Rational tempRat;
	if (this->IndexStartingCrossSectionNormal==-1)
		return false;
	root::RootScalarEuclideanRoot
		(point,	owner.StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].normal,tempRat);
	if (tempRat.IsEqualToZero()){return false;}
	point.DivByLargeRational(tempRat);
	root::RootScalarEuclideanRoot
		(	owner.StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].normal,
			owner.StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].affinePoint,
			tempRat);
	point.MultiplyByLargeRational(tempRat);
	return true;
}

bool CombinatorialChamber::PointIsInChamber(root&point)
{	for (int i=0;i<this->Externalwalls.size;i++)
	{	Rational tempRat;
		root::RootScalarEuclideanRoot(this->Externalwalls.TheObjects[i].normal,point,tempRat);
		if (tempRat.IsNegative())
		{	return false;
		}
	}
	return true;
}

bool CombinatorialChamber::IsABogusNeighbor
	(WallData& NeighborWall, CombinatorialChamber* Neighbor, CombinatorialChamberContainer& ownerComplex,
		GlobalVariables& theGlobalVariables)
{	if (NeighborWall.NeighborsAlongWall.size==1)
		return false;
	roots& ExternalWallsConglomerate= theGlobalVariables.rootsIsABogusNeighbor1;
	roots& FoundVertices = theGlobalVariables.rootsIsABogusNeighbor2;
	ExternalWallsConglomerate.size=0;
	for (int i=0;i<this->Externalwalls.size;i++)
	{	if (&this->Externalwalls.TheObjects[i]!=&NeighborWall)
		{ExternalWallsConglomerate.AddRoot(this->Externalwalls.TheObjects[i].normal);}
	}
	root tempRoot; tempRoot.Assign(NeighborWall.normal); tempRoot.MinusRoot();
	for (int i=0;i<Neighbor->Externalwalls.size;i++)
	{	if (!(Neighbor->Externalwalls.TheObjects[i].normal.IsEqualTo(tempRoot)))
		{ExternalWallsConglomerate.AddRootNoRepetition
			(Neighbor->Externalwalls.TheObjects[i].normal);}
	}
	Selection theSelection;
	theSelection.init(ExternalWallsConglomerate.size);
	int NumPossibilities;
	NumPossibilities =
		MathRoutines::NChooseK(ExternalWallsConglomerate.size,ownerComplex.AmbientDimension-2);
	root VertexCandidate; VertexCandidate.SetSizeExpandOnTopLight(ownerComplex.AmbientDimension);
	int OldRank; OldRank= 0;
	FoundVertices.size=0;
	if (this->flagAnErrorHasOccurredTimeToPanic)
	{ ExternalWallsConglomerate.ComputeDebugString();
		NeighborWall.ComputeDebugString();
	}
	for (int i=0;i<NumPossibilities;i++)
	{	theSelection.incrementSelectionFixedCardinality(ownerComplex.AmbientDimension-2);
		if(	ExternalWallsConglomerate.ComputeNormalFromSelectionAndExtraRoot
					(VertexCandidate,NeighborWall.normal,theSelection,theGlobalVariables))
		{	if (this->PlusMinusPointIsInChamber(VertexCandidate))
			{	if (Neighbor->PointIsInChamber(VertexCandidate))
				{	if(FoundVertices.AddRootNoRepetition(VertexCandidate))
					{	int NewRank;
						if (this->flagAnErrorHasOccurredTimeToPanic)
							FoundVertices.ComputeDebugString();
						NewRank= FoundVertices.GetRankOfSpanOfElements(theGlobalVariables);
						if (NewRank>OldRank)
						{OldRank= NewRank;}
						else
						{FoundVertices.size--;}
						if (OldRank==ownerComplex.AmbientDimension-1)
						{return false;}
					}
				}
			}
		}
	}
	return true;
}


bool CombinatorialChamber::LinearAlgebraForVertexComputation
			(Selection& theSelection,  root& output, GlobalVariables& theGlobalVariables, int theDimension)
{ output.SetSizeExpandOnTopLight(theDimension);
	assert(theDimension -1== theSelection.CardinalitySelection);
	MatrixLargeRational& RMinus1ByR=
		theGlobalVariables.matLinearAlgebraForVertexComputation;
	MatrixLargeRational	matOutputEmpty;
	RMinus1ByR.init((short)(theDimension-1),(short)theDimension);
	matOutputEmpty.init(-1,-1);
	Selection& NonPivotPoints= theGlobalVariables.selLinearAlgebraForVertexComputation;
	for (int i =0;i<theDimension-1;i++)
	{	for (int j=0; j<theDimension;j++)
		{	RMinus1ByR.elements[i][j].Assign
				(this->Externalwalls.TheObjects[theSelection.elements[i]].normal.TheObjects[j]);
		}
	}
	MatrixLargeRational::GaussianEliminationByRows(RMinus1ByR,matOutputEmpty,NonPivotPoints);
	if (NonPivotPoints.CardinalitySelection==1)
	{	RMinus1ByR.NonPivotPointsToRoot(NonPivotPoints,theDimension,output);
		return true;
	}
	return false;
}

bool CombinatorialChamber::LinearAlgebraForVertexComputationOneAffinePlane
	(	Selection& theSelection, root& output,
		GlobalVariables& theGlobalVariables, CombinatorialChamberContainer* owner)
{ assert(theSelection.CardinalitySelection==owner->AmbientDimension-2);
	MatrixLargeRational& tempMat=
		theGlobalVariables.matComputationBufferLinAlgOneAffinePlane;
	MatrixLargeRational& tempColumn=theGlobalVariables.matComputationBufferLinAlgAffinePart;
	tempMat.init(owner->AmbientDimension, owner->AmbientDimension);
	tempColumn.init(owner->AmbientDimension,1);
	for (int i=0;i<theSelection.CardinalitySelection;i++)
	{ for (int j=0;j<owner->AmbientDimension;j++)
		{ tempMat.elements[i][j].Assign
				(	this->Externalwalls.TheObjects[theSelection.elements[i]]
						.normal.TheObjects[j]);
		}
		tempColumn.elements[i][0].MakeZero();
	}
	tempColumn.elements[owner->AmbientDimension-2][0].MakeOne();
	Rational tempRat; Rational tempOne; tempOne.MakeOne();
	root::RootScalarEuclideanRoot
		(	owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].normal,
			owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal].affinePoint,tempRat);
	tempRat.Subtract(tempOne);
	tempColumn.elements[owner->AmbientDimension-1][0].Assign(tempRat);
	for (int i=0;i<owner->AmbientDimension-1;i++)
	{ tempMat.elements[owner->AmbientDimension-2][i].MakeZero();
		tempMat.elements[owner->AmbientDimension-1][i].Assign
			(	owner->StartingCrossSections.TheObjects[this->IndexStartingCrossSectionNormal]
					.normal.TheObjects[i]);
	}
	tempMat.elements[owner->AmbientDimension-2][owner->AmbientDimension-1].MakeOne();
	tempMat.elements[owner->AmbientDimension-1][owner->AmbientDimension-1].MakeZero();
	Selection tempSel;
	tempMat.ComputeDebugString();
	tempColumn.ComputeDebugString();
	Matrix<Rational>::GaussianEliminationByRows(tempMat,tempColumn, tempSel);
	if(tempSel.CardinalitySelection!=0)
		return false;
	output.SetSizeExpandOnTopLight(owner->AmbientDimension-1);
	tempMat.ComputeDebugString();
	tempColumn.ComputeDebugString();
	for (int i=0;i<owner->AmbientDimension-1;i++)
		output.TheObjects[i].Assign(tempColumn.elements[i][0]);
	assert(tempColumn.elements[owner->AmbientDimension-1][0].IsEqualTo(ROne));
	return true;
}

bool CombinatorialChamber::SliceInDirection(root& direction,roots& directions,
																						int CurrentIndex,CombinatorialChamberContainer& output,
																						hashedRoots& FacetOutput,
																						GlobalVariables& theGlobalVariables)
{	if (!(this->Externalwalls.size>=output.AmbientDimension))
	{	this->ComputeDebugString(&output);
//		CombinatorialChamberContainer::TheBigDump<<this->DebugString;
	}
	assert(this->Externalwalls.size>=output.AmbientDimension);
	this->flagExplored=false;
	bool canHaveZeroPoly=true;
	if (this->InternalWalls.size!=0)
	{	if (this->SplitChamber(this->InternalWalls.TheObjects[0],output,direction,theGlobalVariables))
		{	return true;
		}
		else
		{this->InternalWalls.PopIndexSwapWithLast(0);}
		return false;
	}
	else
	{	//if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
		//	this->ComputeDebugString();
		for (int i=0;i<this->Externalwalls.size;i++)
		{	if(this->Externalwalls.TheObjects[i].IsExternalWithRespectToDirection(direction))
			{	if (this->flagHasZeroPolynomial)
				{	bool tempBool;
					this->Externalwalls.TheObjects[i].EveryNeigborIsExplored(tempBool);
					canHaveZeroPoly= canHaveZeroPoly|| tempBool;
				}
				for (int j=i+1; j<this->Externalwalls.size;j++)
				{	if(this->Externalwalls.TheObjects[j].IsExternalWithRespectToDirection(direction))
					{	bool tempBool;
						this->Externalwalls.TheObjects[j].EveryNeigborIsExplored(tempBool);
						canHaveZeroPoly= canHaveZeroPoly|| tempBool;
						root candidateNormal;
						tempBool=this->MakeFacetFromEdgeAndDirection
							(	this->Externalwalls.TheObjects[i],this->Externalwalls.TheObjects[j],output,
								direction,directions,CurrentIndex,candidateNormal, theGlobalVariables);
						if (tempBool)
						{	if (this->SplitChamber(candidateNormal,output,direction, theGlobalVariables))
								return true;
							else
							{ this->flagExplored=true;
								return false;
							}
						}
					}
				}
			}
		}
	}
	if (this->flagHasZeroPolynomial)
		this->flagHasZeroPolynomial= canHaveZeroPoly;
	this->flagExplored=true;
	return false;
}

void CombinatorialChamber::FindAllNeighbors(
	     ListObjectPointers<CombinatorialChamber>& TheNeighbors)
{	for (int i=0;i<this->Externalwalls.size;i++)
	{	for (int j=0; j<this->Externalwalls.TheObjects[i].NeighborsAlongWall.size;j++)
			TheNeighbors.AddObjectOnTop(this->Externalwalls.TheObjects[i].NeighborsAlongWall.TheObjects[j]);
	}
}

bool CombinatorialChamber::TestPossibilityToSlice(root& direction)
{	bool tempBool;
	if (this->flagExplored) {return false;}
	if (this->flagPermanentlyZero)
	{	this->flagExplored=true;
		return false;
	}
	for (int j=0;j<this->Externalwalls.size;j++)
	{ if(	this->Externalwalls.TheObjects[j].IsExternalWithRespectToDirection(direction) &&
	      (!this->Externalwalls.TheObjects[j].EveryNeigborIsExplored(tempBool)))
			return false;
	}
	return true;
}

bool CombinatorialChamber::SplitChamber
	(	root& theKillerPlaneNormal,CombinatorialChamberContainer& output,
		root& direction, GlobalVariables& theGlobalVariables)
{	//	 roots PositiveVertices, NegativeVertices
	if (output.flagSliceWithAWallIgnorePermanentlyZero && this->flagPermanentlyZero)
		return false;
	bool AnErrorHasOcurred;
	bool PlusChamberIsPermanentZero;
	bool MinusChamberIsPermanentZero;
	rootsCollection& LocalContainerPlusVertices=theGlobalVariables.rootsCollectionSplitChamber1;
	rootsCollection& LocalContainerMinusVertices=theGlobalVariables.rootsCollectionSplitChamber2;
	roots& LocalLinearAlgebra= theGlobalVariables.rootsSplitChamber1;
	ListBasicObjects<CombinatorialChamber*>& PossibleBogusNeighbors=
		theGlobalVariables.listCombinatorialChamberPtSplitChamber;
	ListBasicObjects<WallData*>& PossibleBogusWalls=
		theGlobalVariables.listWallDataPtSplitChamber;
	CombinatorialChamber* NewPlusChamber;
	CombinatorialChamber* NewMinusChamber;
	PossibleBogusNeighbors.size=0;
	PossibleBogusWalls.size=0;
	Selection theSelection;
	PlusChamberIsPermanentZero=false;
	AnErrorHasOcurred=false;
	MinusChamberIsPermanentZero=false;
	LocalLinearAlgebra.size=0;
//	if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
//	{	output.ComputeDebugString();
//		this->ConsistencyCheck();
//	}
	for (int i=0;i<this->Externalwalls.size;i++)
	{	bool tempBool;
		tempBool=LocalLinearAlgebra.AddRootNoRepetition(Externalwalls.TheObjects[i].normal);
		if (!tempBool){this->ComputeDebugString(&output);}
		assert(tempBool);
	}
 	LocalLinearAlgebra.AddRoot(theKillerPlaneNormal);
	theSelection.init(LocalLinearAlgebra.size);
	int NumCandidates =MathRoutines::NChooseK(LocalLinearAlgebra.size, output.AmbientDimension-1);
	LocalContainerPlusVertices.SetSizeExpandOnTopNoObjectInit(LocalLinearAlgebra.size);
	LocalContainerMinusVertices.SetSizeExpandOnTopNoObjectInit(LocalLinearAlgebra.size);
	LocalContainerMinusVertices.ResetCounters();
	LocalContainerPlusVertices.ResetCounters();
	bool hasPositive, hasNegative;
	hasPositive=false; hasNegative=false;
	root PositiveChamberInternalPoint, NegativeChamberInternalPoint;
	PositiveChamberInternalPoint.MakeZero(output.AmbientDimension);
	NegativeChamberInternalPoint.MakeZero(output.AmbientDimension);
	for (int i=0;i<NumCandidates;i++)
	{	root VertexCandidate; VertexCandidate.SetSizeExpandOnTopLight(output.AmbientDimension);
		theSelection.incrementSelectionFixedCardinality(output.AmbientDimension-1);
		if (	LocalLinearAlgebra.ComputeNormalFromSelection
						(VertexCandidate,theSelection, theGlobalVariables,output.AmbientDimension))
		{	if (this->PlusMinusPointIsInChamber(VertexCandidate))
			{	Rational tempRat; bool IsPositive; bool IsNegative;
				root::RootScalarEuclideanRoot(VertexCandidate,theKillerPlaneNormal,tempRat);
				IsPositive = tempRat.IsPositive();
				IsNegative = tempRat.IsNegative();
				hasPositive = (hasPositive || IsPositive);
				hasNegative = (hasNegative || IsNegative);
				for (int j=0;j<output.AmbientDimension-1;j++)
				{	if (!IsNegative)
					{	LocalContainerPlusVertices.TheObjects[theSelection.elements[j]]
							.AddRootNoRepetition(VertexCandidate);
						PositiveChamberInternalPoint.Add(VertexCandidate);
					}
					if(!IsPositive)
					{	LocalContainerMinusVertices.TheObjects[theSelection.elements[j]]
							.AddRootNoRepetition(VertexCandidate);
						NegativeChamberInternalPoint.Add(VertexCandidate);
					}
				}
			}
		}
	}
	if (!output.flagMakingASingleHyperplaneSlice)
	{	PlusChamberIsPermanentZero= output.IsSurelyOutsideGlobalCone( LocalContainerPlusVertices);
		MinusChamberIsPermanentZero=output.IsSurelyOutsideGlobalCone( LocalContainerMinusVertices);
	}
	if (!(hasPositive && hasNegative))
	{	if (output.flagMakingASingleHyperplaneSlice && output.flagSliceWithAWallInitDone)
		{ if (hasPositive)
				this->PropagateSlicingWallThroughNonExploredNeighbors
					(theKillerPlaneNormal,LocalContainerMinusVertices,output,theGlobalVariables);
			else
				this->PropagateSlicingWallThroughNonExploredNeighbors
					(theKillerPlaneNormal,LocalContainerPlusVertices,output,theGlobalVariables);
			this->flagExplored=true;
		}
		return false;
	}
	if (!output.flagMakingASingleHyperplaneSlice)
	{	if (!CombinatorialChamberContainer::startingCones.SeparatePoints
					( PositiveChamberInternalPoint, NegativeChamberInternalPoint,&theKillerPlaneNormal))
		{	return false;}
	}
//	if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
//	{ LocalContainerPlusVertices.ComputeDebugString();
//		LocalContainerMinusVertices.ComputeDebugString();
//	}
	NewPlusChamber= new CombinatorialChamber;
	NewMinusChamber= new CombinatorialChamber;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter+=2;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
	NewPlusChamber->flagPermanentlyZero= PlusChamberIsPermanentZero;
	NewMinusChamber->flagPermanentlyZero= MinusChamberIsPermanentZero;
	NewPlusChamber->Externalwalls.MakeActualSizeAtLeastExpandOnTop(this->Externalwalls.size+1);
	NewMinusChamber->Externalwalls.MakeActualSizeAtLeastExpandOnTop(this->Externalwalls.size+1);
	NewPlusChamber->flagHasZeroPolynomial= this->flagHasZeroPolynomial;
	NewMinusChamber->flagHasZeroPolynomial= this->flagHasZeroPolynomial;
	NewPlusChamber->IndexStartingCrossSectionNormal= this->IndexStartingCrossSectionNormal;
	NewMinusChamber->IndexStartingCrossSectionNormal= this->IndexStartingCrossSectionNormal;
	output.AddChamberPointerSetUpPreferredIndices(NewPlusChamber, theGlobalVariables);
	output.AddChamberPointerSetUpPreferredIndices(NewMinusChamber,theGlobalVariables);
	if (CombinatorialChamber::DisplayingGraphics)
	{	LocalContainerPlusVertices.Average(NewPlusChamber->InternalPoint,LocalLinearAlgebra.size,output.AmbientDimension);
		LocalContainerMinusVertices.Average(NewMinusChamber->InternalPoint,LocalLinearAlgebra.size,output.AmbientDimension);
		for (int i=0;i<LocalLinearAlgebra.size;i++)
		{	NewPlusChamber->AllVertices.AddRootSnoRepetition
				(LocalContainerPlusVertices.TheObjects[i]);
			NewMinusChamber->AllVertices.AddRootSnoRepetition
				(LocalContainerMinusVertices.TheObjects[i]);
		}
	}
	PossibleBogusNeighbors.size=0;
	PossibleBogusWalls.size=0;
	if (output.flagAnErrorHasOcurredTimeToPanic)
		this->ComputeDebugString(&output);
	for (int i=0;i<this->Externalwalls.size;i++)
	{	Externalwalls.TheObjects[i].SplitWall(this,	NewPlusChamber,NewMinusChamber,&output,
																		LocalContainerPlusVertices.TheObjects[i],
																		LocalContainerMinusVertices.TheObjects[i],
																		theKillerPlaneNormal,direction,
																		PossibleBogusNeighbors,
																		&PossibleBogusWalls,theGlobalVariables);
		if (output.flagAnErrorHasOcurredTimeToPanic)
		{ NewPlusChamber->ComputeDebugString(&output);
			NewMinusChamber->ComputeDebugString(&output);
			this->ComputeDebugString(&output);
			NewPlusChamber->ConsistencyCheck(output.AmbientDimension);
			NewMinusChamber->ConsistencyCheck(output.AmbientDimension);
			output.ComputeDebugString();
		}
	}
//	if (CombinatorialChamberContainer::AnErrorHasOcurredTimeToPanic)
//	{ this->ComputeDebugString();
//	}
	for (int i=1;i<this->InternalWalls.size;i++)
	{	NewPlusChamber->InternalWalls.AddRootNoRepetition(this->InternalWalls.TheObjects[i]);
		NewMinusChamber->InternalWalls.AddRootNoRepetition(this->InternalWalls.TheObjects[i]);
	}
	this->MakeNewMutualNeighbors(NewPlusChamber, NewMinusChamber,theKillerPlaneNormal);
	if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
	{	assert(NewPlusChamber->ConsistencyCheck(output.AmbientDimension));
		assert(NewMinusChamber->ConsistencyCheck(output.AmbientDimension));
		output.ComputeDebugString();
	}
	for (int i=0;i<PossibleBogusNeighbors.size;i++)
	{	//if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
		//{	PossibleBogusWalls.TheObjects[i]->ComputeDebugString();
		//	PossibleBogusNeighbors.TheObjects[i]->ComputeDebugString();
		//}
		if (NewPlusChamber->IsABogusNeighbor(*PossibleBogusWalls.TheObjects[i],
			                                   PossibleBogusNeighbors.TheObjects[i],
																				 output,theGlobalVariables))
		{	PossibleBogusWalls.TheObjects[i]->RemoveNeighborhoodBothSides
				(PossibleBogusNeighbors.TheObjects[i],NewPlusChamber);
		}
		if (NewMinusChamber->IsABogusNeighbor(*PossibleBogusWalls.TheObjects[i],
			                                    PossibleBogusNeighbors.TheObjects[i],
																					output,theGlobalVariables ) )
		{	PossibleBogusWalls.TheObjects[i]->RemoveNeighborhoodBothSides
				(PossibleBogusNeighbors.TheObjects[i],NewMinusChamber);
		}
	}
	assert(NewPlusChamber->Externalwalls.size>=output.AmbientDimension);
	assert(NewMinusChamber->Externalwalls.size>=output.AmbientDimension);
	//if (output.flagAnErrorHasOcurredTimeToPanic)
	//{	output.ComputeDebugString();
		//assert(NewPlusChamber->ConsistencyCheck());
		//assert(NewMinusChamber->ConsistencyCheck());
	//}
	return true;
}

void CombinatorialChamber::PropagateSlicingWallThroughNonExploredNeighbors
	(	root& theKillerNormal,rootsCollection &CuttingPlaneVertices,
		::CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables)
{ for (int i=0;i<this->Externalwalls.size;i++)
	{ bool mustPropagate=false;
		for (int j=0;j<CuttingPlaneVertices.TheObjects[i].size;j++)
		{	if (this->Externalwalls.TheObjects[i].ContainsPoint(CuttingPlaneVertices.TheObjects[i].TheObjects[j]))
			{ mustPropagate=true;
				break;
			}
		}
		if (mustPropagate)
		{ for (int j=0;j<this->Externalwalls.TheObjects[i].NeighborsAlongWall.size;j++)
			{ if (this->Externalwalls.TheObjects[i].NeighborsAlongWall.TheObjects[j]!=0)
				{	if(!this->Externalwalls.TheObjects[i].NeighborsAlongWall.TheObjects[j]->flagExplored)
					{	root tempRoot; tempRoot.MakeZero(owner.AmbientDimension);
						this->Externalwalls.TheObjects[i].NeighborsAlongWall.TheObjects[j]
							->AddInternalWall(theKillerNormal,this->Externalwalls.TheObjects[i]
								.MirrorWall.TheObjects[j]->normal,tempRoot,&owner,theGlobalVariables);
					}
				}
			}
		}
	}
}

void CombinatorialChamberContainer::ConvertHasZeroPolyToPermanentlyZero()
{ for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]!=0)
			if (this->TheObjects[i]->flagHasZeroPolynomial)
				this->TheObjects[i]->flagPermanentlyZero=true;
}

void CombinatorialChamberContainer::AddWeylChamberWallsToHyperplanes
	(GlobalVariables& theGlobalVariables, WeylGroup& theWeylGroup)
{ MatrixLargeRational tempMat;
	tempMat.AssignMatrixIntWithDen(theWeylGroup.KillingFormMatrix,1);
	tempMat.Invert(theGlobalVariables);
	tempMat.ComputeDebugString();
	this->NumProjectiveHyperplanesBeforeWeylChamberWalls=this->theHyperplanes.size;
	roots tempRoots;
	this->WeylChamber.ReleaseMemory();
	for (int i=0;i<tempMat.NumCols;i++)
	{ root tempRoot; tempRoot.SetSizeExpandOnTopLight(tempMat.NumRows);
		for (int j=0; j<tempMat.NumRows;j++)
			tempRoot.TheObjects[j].Assign(tempMat.elements[j][i]);
		tempRoots.AddRoot(tempRoot);
	}
	this->WeylChamber.ComputeFromDirections(tempRoots, theGlobalVariables, tempMat.NumCols);
	this->WeylChamber.ComputeDebugString();
	for (int i=0;i< this->WeylChamber.size;i++)
		this->theHyperplanes.AddObjectOnTopNoRepetitionOfObjectHash(this->WeylChamber.TheObjects[i]);
}


void CombinatorialChamberContainer::AddChamberPointerSetUpPreferredIndices
	(CombinatorialChamber* theChamber, GlobalVariables& theGlobalVariables)
{	theChamber->IndexInOwnerComplex=this->size;
	this->AddObjectOnTop(theChamber);
	if (!theChamber->flagPermanentlyZero)
		this->PreferredNextChambers.AddObjectOnTop(this->size-1);
	else
		theChamber->flagExplored=true;
}

void CombinatorialChamber::MakeNewMutualNeighbors
	(CombinatorialChamber* NewPlusChamber, CombinatorialChamber* NewMinusChamber, root& normal)
{	WallData tempWall; tempWall.normal.Assign(normal);
	tempWall.NeighborsAlongWall.AddObjectOnTop(NewMinusChamber);
	NewPlusChamber->Externalwalls.AddObjectOnTop(tempWall);
	tempWall.normal.MinusRoot(); tempWall.NeighborsAlongWall.TheObjects[0]=NewPlusChamber;
	NewMinusChamber->Externalwalls.AddObjectOnTop(tempWall);
	NewPlusChamber->Externalwalls.LastObject()->MirrorWall.AddObjectOnTop
		(	NewMinusChamber->Externalwalls.LastObject());
	NewMinusChamber->Externalwalls.LastObject()->MirrorWall.AddObjectOnTop
		(	NewPlusChamber->Externalwalls.LastObject());
//	if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
//	{ NewPlusChamber->ComputeDebugString();
//		NewMinusChamber->ComputeDebugString();
//	}
}

void CombinatorialChamberContainer::SliceWithAWall
	(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{ this->SliceWithAWallInit(TheKillerFacetNormal,theGlobalVariables);
	while (this->PreferredNextChambers.size>0)
	{ this->SliceWithAWallOneIncrement(TheKillerFacetNormal,theGlobalVariables);
	}
	this->PurgeZeroPointers();
	this->PurgeInternalWalls();
}

void CombinatorialChamberContainer::PurgeInternalWalls()
{ for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]!=0)
			this->TheObjects[i]->PurgeInternalWalls();
}

void CombinatorialChamberContainer::SliceWithAWallInit
	(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{ this->flagMakingASingleHyperplaneSlice=true;
	this->flagSliceWithAWallInitDone=false;
	this->PurgeZeroPointers();
	this->PreferredNextChambers.ReleaseMemory();
	this->LabelAllUnexplored();
	root tempRoot; tempRoot.MakeZero(this->AmbientDimension);
	TheKillerFacetNormal.ComputeDebugString();
	for (int i=0;i<this->size;i++)
	{ if (this->flagAnErrorHasOcurredTimeToPanic)
		{ this->ComputeDebugString();
		}
		if (this->TheObjects[i]!=0)
		{	if (this->TheObjects[i]->SplitChamber(TheKillerFacetNormal,*this,tempRoot,theGlobalVariables))
			{	delete this->TheObjects[i];
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
				this->TheObjects[i]=0;
				break;
			}
		}
	}
	if (this->PreferredNextChambers.size>0)
	{	this->indexNextChamberToSlice= this->PreferredNextChambers.TheObjects[0];
	} else
	{	this->indexNextChamberToSlice=-1;
	}
	this->flagSliceWithAWallInitDone=true;
}

void CombinatorialChamberContainer::SliceWithAWallOneIncrement
	(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{	root tempRoot; tempRoot.MakeZero(this->AmbientDimension);
	if (!this->flagSliceWithAWallInitDone)
	{ this->SliceWithAWallInit(TheKillerFacetNormal,theGlobalVariables);
	}
	else
	{	if(this->PreferredNextChambers.size>0)
		{ if (this->TheObjects[this->PreferredNextChambers.TheObjects[0]]!=0)
			{	if (this->TheObjects[this->PreferredNextChambers.TheObjects[0]]->
							SplitChamber(TheKillerFacetNormal,*this,tempRoot,theGlobalVariables))
				{ delete this->TheObjects[this->PreferredNextChambers.TheObjects[0]];
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
					this->TheObjects[this->PreferredNextChambers.TheObjects[0]]=0;
				}
			}
			this->PreferredNextChambers.PopIndexShiftUp(0);
			if (this->flagAnErrorHasOcurredTimeToPanic)
			{ this->ComputeDebugString();
			}
		}
	}
	if (this->PreferredNextChambers.size>0)
	{	this->indexNextChamberToSlice= this->PreferredNextChambers.TheObjects[0];
	} else
	{	this->indexNextChamberToSlice=-1;
	}
}

bool CombinatorialChamberContainer::IsSurelyOutsideGlobalCone
					(rootsCollection& TheVertices)
{ return this->TheGlobalConeNormals.IsSurelyOutsideCone(TheVertices);
}

void CombinatorialChamberContainer::ComputeGlobalCone(roots& directions, GlobalVariables& theGlobalVariables)
{	this->TheGlobalConeNormals.ComputeFromDirections(directions, theGlobalVariables,this->AmbientDimension);
}

void CombinatorialChamberContainer::MakeExtraProjectivePlane()
{	/*root extraWallNormal;
	extraWallNormal.MakeZero(root::AmbientDimension+1);
	extraWallNormal.TheObjects[root::AmbientDimension].MakeOne();
	Facet* tempFacet2= this->theHyperplanes.
		NormalizeRootAndGetFacetCreateNewIfNeeded(extraWallNormal);
	for (int i=0;i<this->size;i++)
	{ tempFacet2->Owners.AddCouple(this->TheObjects[i],0);
	}*/
}

void CombinatorialChamberContainer::ComputeVerticesFromNormals(GlobalVariables& theGlobalVariables)
{ if (this->flagAnErrorHasOcurredTimeToPanic)
	{ for (int i=0;i<this->size;i++)
			this->TheObjects[i]->ComputeDebugString(this);
	}
	for (int i=0;i<this->size;i++)
		this->TheObjects[i]->ComputeVerticesFromNormals(*this, theGlobalVariables);
}

bool CombinatorialChamberContainer::ProjectToDefaultAffineSpaceModifyCrossSections(GlobalVariables& theGlobalVariables)
{ for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]!=0)
			if (!this->TheObjects[i]->ProjectToDefaultAffineSpace(this,theGlobalVariables))
				return false;
	return true;
}

void CombinatorialChamberContainer::ProjectToDefaultAffineSpace(GlobalVariables& theGlobalVariables)
{ while (!this->ProjectToDefaultAffineSpaceModifyCrossSections(theGlobalVariables))
	{ //this->ComputeDebugString();
	}
}

void CombinatorialChamberContainer::ComputeNextIndexToSlice(root &direction)
{//	if (this->flagAnErrorHasOcurredTimeToPanic)
//		this->ComputeDebugString();
	while (this->PreferredNextChambers.size>0)
	{ this->indexNextChamberToSlice=this->PreferredNextChambers.TheObjects[0];
		this->PreferredNextChambers.PopIndexShiftUp(0);
		if (this->TheObjects[this->indexNextChamberToSlice]->TestPossibilityToSlice(direction))
			return;
	}
	this->indexNextChamberToSlice=-1;
	bool foundUnexplored=false;
	for (int i=this->FirstNonExploredIndex;i<size;i++)
	{	if (this->TheObjects[i]!=0)
		{	if(!foundUnexplored && !this->TheObjects[i]->flagExplored )
			{ this->FirstNonExploredIndex=i;
				foundUnexplored=true;
			}
			if (this->TheObjects[i]->TestPossibilityToSlice (direction))
			{	this->indexNextChamberToSlice= i;
				return;
			}
		}
	}
	this->indexNextChamberToSlice=-1;
}

bool CombinatorialChamberContainer::ConsistencyCheck()
{	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i]!=0)
			if (!this->TheObjects[i]->ConsistencyCheck(this->AmbientDimension))
				return false;
	}
	return true;
}

void CombinatorialChamberContainer::WriteToFile(DrawingVariables& TDV, roots& directions, std::fstream& output)
{ if (!output.is_open())
		return;
	this->drawOutput
		(TDV,*this,directions,0,this->IndicatorRoot,&output,&drawlineDummy, &drawtextDummy);
	std::string tempS;
	output	<< "Nilradical simple coords:\\\\\n ";
	for (int i=0;i<directions.size;i++)
	{ directions.TheObjects[i].ElementToString(tempS);
		output << tempS <<"\\\\\n";
	}
	this->ElementToString(tempS,true);
	output<< tempS;
}

void CombinatorialChamberContainer::ElementToString(std::string& output)
{ this->ElementToString(output,false);
}

void CombinatorialChamberContainer::ElementToString(std::string& output, bool LatexFormat)
{ std::stringstream out;
	std::string tempS;
	std::string endOfLine;
	if (LatexFormat)
		endOfLine= "\\\\\n";
	else
		endOfLine= "\n";
	if (LatexFormat)
	{ this->PurgeZeroPointers();
		out << "Total visible chambers: "
				<< this->GetNumVisibleChambersAndLabelChambersForDisplay()
				<< endOfLine;
	}
	if (this->AffineWallsOfWeylChambers.size>0)
	{	int tempI=this->GetNumChambersInWeylChamberAndLabelChambers(this->WeylChamber);
		out << "Number of chambers with internal point in Weyl chamber: "
				<< tempI<< endOfLine;
		out <<"Weyl chamber walls and their images: \\\\";
		for (int i=0;i<this->AffineWallsOfWeylChambers.size;i++)
		{	this->AffineWallsOfWeylChambers.TheObjects[i].ElementToString(tempS);
			out<< tempS <<endOfLine;
		}
	} else
	{ out << "Number of visible chambers: " << this->GetNumVisibleChambersAndLabelChambersForDisplay() << endOfLine;
	}
	ListBasicObjects<int> sortedIndices;
	this->SortIndicesByDisplayNumber(sortedIndices);
	if (this->size>this->GraphicsMaxNumChambers)
	{	out<<"Detailed chamber data too large for display";
		output=out.str();
		return;
	}
	if (!LatexFormat)
	{	for (int i=0;i<this->size;i++)
		{ if (this->TheObjects[i]!=0)
			{	this->TheObjects[i]->ElementToString(tempS,this,LatexFormat);
					out <<tempS <<endOfLine;
					if (out.gcount()>this->flagMaxNumCharsAllowedInStringOutput)
						break;
			}
		}
	} else
	{ for (int i=0;i<sortedIndices.size;i++)
		{ this->TheObjects[sortedIndices.TheObjects[i]]->ElementToString
				(tempS, this,LatexFormat);
			out <<tempS <<endOfLine;
		}
	}
	output= out.str();
}

void CombinatorialChamberContainer::WireChamberAdjacencyInfoAsIn
	(CombinatorialChamberContainer& input)
{ assert(this->size==input.size);
	input.LabelChamberIndicesProperly();
	this->LabelChamberIndicesProperly();
	if (this->flagAnErrorHasOcurredTimeToPanic)
	{ this->ComputeDebugString();
	}
	for (int i=0;i<input.size;i++)
	{ this->TheObjects[i]->WireChamberAndWallAdjacencyData(*this, input.TheObjects[i]);
	}
}

void CombinatorialChamberContainer::InduceFromLowerDimensionalAndProjectivize
	(CombinatorialChamberContainer& input, GlobalVariables& theGlobalVariables)
{ this->init();
	this->flagMakingASingleHyperplaneSlice=true;
	this->AmbientDimension=input.AmbientDimension+1;
//	input.ComputeDebugString();
  this->initAndCreateNewObjects(input.size);
	this->StartingCrossSections.SetSizeExpandOnTopNoObjectInit(input.StartingCrossSections.size);
	for (int i=0;i<input.StartingCrossSections.size;i++)
	{ this->StartingCrossSections.TheObjects[i].affinePoint.SetSizeExpandOnTopLight(input.AmbientDimension+1);
		this->StartingCrossSections.TheObjects[i].normal.SetSizeExpandOnTopLight(input.AmbientDimension+1);
		for (int j=0;j<input.AmbientDimension;j++)
		{ this->StartingCrossSections.TheObjects[i].affinePoint.TheObjects[j].Assign(
				input.StartingCrossSections.TheObjects[i].affinePoint.TheObjects[j]);
			this->StartingCrossSections.TheObjects[i].normal.TheObjects[j].Assign(
				input.StartingCrossSections.TheObjects[i].normal.TheObjects[j]);
		}
		this->StartingCrossSections.TheObjects[i].normal.TheObjects[input.AmbientDimension].MakeOne();
		this->StartingCrossSections.TheObjects[i].affinePoint.TheObjects[input.AmbientDimension].MakeOne();
	}
	input.LabelChamberIndicesProperly();
  this->LabelChamberIndicesProperly();
	if (this->flagAnErrorHasOcurredTimeToPanic)
	{ this->ComputeDebugString();
		input.ComputeDebugString();
	}
	for (int i=0;i<this->size;i++)
  { this->TheObjects[i]->
			InduceFromCombinatorialChamberLowerDimensionNoAdjacencyInfo
				(*input.TheObjects[i],*this);
  }
  this->theHyperplanes.ClearTheObjects();
  //this->ComputeDebugString();
  for (int i=0;i<input.theHyperplanes.size;i++)
	{ root tempRoot, theZeroRoot; theZeroRoot.MakeZero(this->AmbientDimension-1);
		tempRoot.Assign(input.theHyperplanes.TheObjects[i]);
		tempRoot.MakeNormalInProjectivizationFromPointAndNormal(theZeroRoot,input.theHyperplanes.TheObjects[i]);
		tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
		this->theHyperplanes.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
  }
  //this->ComputeDebugString();
  this->ComputeVerticesFromNormals(theGlobalVariables);
  //this->ComputeDebugString();
  this->WireChamberAdjacencyInfoAsIn(input);
  //this->ComputeDebugString();
	if (!this->ConsistencyCheck())
	{ this->flagAnErrorHasOcurredTimeToPanic=true;
	}
	assert(this->ConsistencyCheck());
}

void CombinatorialChamberContainer::LabelChamberIndicesProperly()
{ for(int i=0;i<this->size;i++)
	{ if (this->TheObjects[i]!=0)
		{	this->TheObjects[i]->IndexInOwnerComplex=i;
			this->TheObjects[i]->LabelWallIndicesProperly();
		}
  }
}

CombinatorialChamberContainer::CombinatorialChamberContainer()
{	this->init();
}

void CombinatorialChamberContainer::init()
{ this->KillAllElements();
	this->ReleaseMemory();
	this->FirstNonExploredIndex=0;
	this->indexNextChamberToSlice=-1;
	this->NewHyperplanesToSliceWith.size=0;
	this->theHyperplanes.ClearTheObjects();
	this->PreferredNextChambers.ReleaseMemory();
	this->startingCones.ReleaseMemory();
	this->theWeylGroupAffineHyperplaneImages.size=0;
	this->flagMakingASingleHyperplaneSlice=false;
	this->flagDrawingProjective=true;
	this->flagSliceWithAWallIgnorePermanentlyZero=true;
}


CombinatorialChamberContainer::~CombinatorialChamberContainer()
{	Free();
}

void CombinatorialChamberContainer::DumpAll()
{	CombinatorialChamberContainer::TheBigDump.clear();
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i]->ComputeDebugString(this);
//		CombinatorialChamberContainer::TheBigDump <<this->TheObjects[i]->DebugString;
	}
}

int CombinatorialChamberContainer::GetNumVisibleChambersAndLabelChambersForDisplay()
{ int NumZeroChambers=0;
	int NumChambersNonZero=0;
	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i]!=0)
		{ if (this->TheObjects[i]->flagHasZeroPolynomial)
			{ NumZeroChambers++;
				this->TheObjects[i]->DisplayNumber=NumZeroChambers;
			} else
			{	NumChambersNonZero++;
				this->TheObjects[i]->DisplayNumber=NumChambersNonZero;
			}
		}
	}
	return NumChambersNonZero;
}

int CombinatorialChamberContainer::GetNumVisibleChambersNoLabeling()
{ int NumChambersNonZero=0;
	for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]!=0)
			if (!this->TheObjects[i]->flagHasZeroPolynomial)
				NumChambersNonZero++;
	return NumChambersNonZero;
}

int CombinatorialChamberContainer::GetNumChambersInWeylChamberAndLabelChambers(Cone& theWeylChamber)
{ int NumChambersInWeyl=0;
	int NumZeroChambers=0;
	int NumChambersNonZeroNotInWeyl=0;
//	theWeylChamber.ComputeDebugString();
	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i]!=0)
		{ if (this->TheObjects[i]->flagHasZeroPolynomial)
			{ NumZeroChambers++;
				this->TheObjects[i]->DisplayNumber=NumZeroChambers;
			} else
			{	if (this->TheObjects[i]->affineVertices.size!=0)
				{	root tempRoot;
					this->TheObjects[i]->ComputeAffineInternalPoint(tempRoot, this->AmbientDimension-1);
					if (theWeylChamber.IsInCone(tempRoot))
					{ NumChambersInWeyl++;
						this->TheObjects[i]->DisplayNumber=NumChambersInWeyl;
					} else
						NumChambersNonZeroNotInWeyl++;
				}
			}
		}
	}
	NumChambersNonZeroNotInWeyl=0;
	for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i]!=0)
		{	if (!this->TheObjects[i]->flagHasZeroPolynomial)
			{	if (this->TheObjects[i]->affineVertices.size!=0)
				{	root tempRoot;
					this->TheObjects[i]->ComputeAffineInternalPoint(tempRoot, this->AmbientDimension-1);
					if (!theWeylChamber.IsInCone(tempRoot))
					{	NumChambersNonZeroNotInWeyl++;
						this->TheObjects[i]->DisplayNumber=
							NumChambersInWeyl+NumChambersNonZeroNotInWeyl;
					}
				}
			}
		}
	}
	return NumChambersInWeyl;
}

void CombinatorialChamberContainer::Free()
{	this->KillAllElements();
}

void CombinatorialChamberContainer::MakeStartingChambers
	(roots& directions, root& IndicatorRoot, GlobalVariables& theGlobalVariables)
{	this->flagMakingASingleHyperplaneSlice=false;
	if (directions.size==0)
		return;
	this->AmbientDimension= directions.TheObjects[0].size;
	if (this->AmbientDimension==1)
		return;
	int tempI= directions.ArrangeFirstVectorsBeOfMaxPossibleRank(theGlobalVariables);
	assert(tempI== this->AmbientDimension);
	this->IndicatorRoot.Assign(IndicatorRoot);
	if (this->IndicatorRoot.size!=this->AmbientDimension)
		this->IndicatorRoot.MakeZero(this->AmbientDimension);
	this->startingCones.initFromDirections(directions,theGlobalVariables);
//	this->startingCones.ComputeDebugString();
	this->PreferredNextChambers.ReleaseMemory();
	this->KillAllElements();
	CombinatorialChamberContainer::ComputeGlobalCone(directions, theGlobalVariables);
	Selection theSelection;
	theSelection.init(this->AmbientDimension);
	this->theHyperplanes.ClearTheObjects();
	this->theHyperplanes.MakeActualSizeAtLeastExpandOnTop
		(MathRoutines::NChooseK(directions.size,this->AmbientDimension-1));
	for(int i=0;i<this->AmbientDimension;i++)
	{	roots TempRoots;
		root TempRoot;
		Rational tempRat;
		theSelection.incrementSelectionFixedCardinality(this->AmbientDimension-1);
		directions.SubSelection(theSelection,TempRoots);
		TempRoots.ComputeNormal(TempRoot);
		TempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
		this->theHyperplanes.AddObjectOnTopHash(TempRoot);
	}
	theSelection.initNoMemoryAllocation();
	int NumStartingChambers=MathRoutines::TwoToTheNth(this->AmbientDimension);
	this->initAndCreateNewObjects(NumStartingChambers);
	this->StartingCrossSections
		.SetSizeExpandOnTopNoObjectInit(NumStartingChambers);
	for(int i=0;i<NumStartingChambers;i++)
	{	int tempI= theSelection.SelectionToIndex();
		this->TheObjects[tempI]->Externalwalls.MakeActualSizeAtLeastExpandOnTop(this->AmbientDimension);
		for (int j=0;j<this->AmbientDimension;j++)
		{	this->TheObjects[tempI]->AllVertices.AddRoot(directions.TheObjects[j]);
			if (theSelection.selected[j])
			{	int tempI2;
				theSelection.selected[j]=false;
				tempI2=theSelection.SelectionToIndex();
				theSelection.selected[j]=true;
				CombinatorialChamber* plusOwner;CombinatorialChamber* minusOwner;
				if(this->theHyperplanes.TheObjects[this->AmbientDimension-j-1]
						.OurScalarProductIsPositive(directions.TheObjects[j]))
				{ plusOwner= this->TheObjects[tempI]; minusOwner= this->TheObjects[tempI2];
				} else
				{ minusOwner= this->TheObjects[tempI]; plusOwner= this->TheObjects[tempI2];
				}
				this->TheObjects[tempI]->MakeNewMutualNeighbors
					(	plusOwner, minusOwner,this->theHyperplanes.TheObjects[this->AmbientDimension-j-1]);
			} else
			{ this->TheObjects[tempI]->AllVertices.LastObject()->MinusRoot();
			}
//			if (this->flagAnErrorHasOcurredTimeToPanic)
	//			this->ComputeDebugString();
		}
		root tempRoot;
		tempRoot.Assign(directions.TheObjects[0]);
		if (!theSelection.selected[0]) {tempRoot.MinusRoot(); }
		this->StartingCrossSections.TheObjects[tempI].affinePoint.Assign(tempRoot);
		theSelection.incrementSelection();
//		if (this->flagAnErrorHasOcurredTimeToPanic)
//			this->ComputeDebugString();
	}
	for(int i=0;i<NumStartingChambers;i++)
	{	this->TheObjects[i]->IndexStartingCrossSectionNormal=i;
		root Accum;	Accum.MakeZero(this->AmbientDimension);
		for (int j=0;j<this->TheObjects[i]->Externalwalls.size;j++)
			Accum.Add(this->TheObjects[i]->Externalwalls.TheObjects[j].normal);
		this->StartingCrossSections.TheObjects[i].normal.Assign(Accum);
//		if (this->flagAnErrorHasOcurredTimeToPanic)
//			Accum.ComputeDebugString();
		assert(this->TheObjects[i]->ConsistencyCheck(this->AmbientDimension));
		if (this->TheGlobalConeNormals.IsSurelyOutsideCone(this->TheObjects[i]->AllVertices))
			this->TheObjects[i]->flagPermanentlyZero=true;
	}
	this->TheObjects[NumStartingChambers-1]->flagHasZeroPolynomial=false;
//	if (this->flagAnErrorHasOcurredTimeToPanic)
//		this->ComputeDebugString();
}

void Cone::ComputeFromDirections(roots& directions, GlobalVariables& theGlobalVariables, int theDimension)
{	this->size=0;
	int NumCandidates = MathRoutines::NChooseK(directions.size,theDimension-1);
	Selection theSelection;
	theSelection.init(directions.size);
	root normalCandidate;
	for (int i=0;i<NumCandidates;i++)
	{	theSelection.incrementSelectionFixedCardinality(theDimension-1);
		if (directions.ComputeNormalFromSelection(normalCandidate,theSelection, theGlobalVariables,theDimension))
		{	bool hasPositive; bool hasNegative;
			hasPositive=false; hasNegative=false;
			for (int j=0;j<directions.size;j++)
			{	Rational tempRat;
				root::RootScalarEuclideanRoot(directions.TheObjects[j],normalCandidate,tempRat);
				if (tempRat.IsNegative()){hasNegative=true;}
				if (tempRat.IsPositive()){hasPositive=true;}
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

bool Cone::IsSurelyOutsideCone(roots& TheVertices)
{ if (!this->FillInChamberTestArray(TheVertices,true))
		return false;
	return this->IsSurelyOutsideConeAccordingToChamberTestArray();
}

bool Cone::FillInChamberTestArray(roots &TheVertices, bool initChamberTestArray)
{ //init first the array we will use for check
  //values in the array: -1 undertermined, 0 - outside normal, +1 inside normal
	if (initChamberTestArray)
		for (int i=0;i<this->size;i++)
			this->ChamberTestArray.TheObjects[i]=-1;
	for (int j=0;j<TheVertices.size;j++)
	{	bool TestedVertexIsStrictlyInsideGlobalCone;
		TestedVertexIsStrictlyInsideGlobalCone= true;
		for (int k=0;k<this->size;k++)
		{	Rational tempRat;
			root::RootScalarEuclideanRoot
				(TheVertices.TheObjects[j],this->TheObjects[k],tempRat);
			int state;	state=-1;
			if (tempRat.IsPositive())
				state =1;
			else
			{	TestedVertexIsStrictlyInsideGlobalCone=false;
				if (tempRat.IsNegative())
					state=0;
			}
			if (state!=-1)
			{	if (this->ChamberTestArray.TheObjects[k]==-1)
					this->ChamberTestArray.TheObjects[k]=state;
				else
					if (this->ChamberTestArray.TheObjects[k]!=state)
						return false;
			}
		}
		if (TestedVertexIsStrictlyInsideGlobalCone){return false;}
	}
	return true;
}

bool Cone::IsSurelyOutsideConeAccordingToChamberTestArray()
{	for (int i=0;i<this->size;i++)
		if (this->ChamberTestArray.TheObjects[i]==0){return true;}
	return false;
}

bool Cone::IsSurelyOutsideCone(::rootsCollection& TheVertices)
{ bool firstRun=true;
	for (int i=0;i<TheVertices.size;i++)
	{	if (!this->FillInChamberTestArray(TheVertices.TheObjects[i],firstRun))
			return false;
		firstRun=true;
	}
	return this->IsSurelyOutsideConeAccordingToChamberTestArray();
}

/*void CombinatorialChamber::InduceFromAffineConeAddExtraDimension(affineCone& input)
{ this->ExternalWallsNormals.SetSizeExpandOnTopNoObjectInit(input.theWalls.size);
	//the extra dimension is going to be the last dimension
	for (int i=0;i<this->ExternalWallsNormals.size;i++)
	{ this->ExternalWallsNormals.TheObjects[i].
			MakeNormalInProjectivizationFromAffineHyperplane(input.theWalls.TheObjects[i]);
	}
}*/

void CombinatorialChamber::
	InduceFromCombinatorialChamberLowerDimensionNoAdjacencyInfo
		(CombinatorialChamber& input, CombinatorialChamberContainer& owner)
{ root ZeroRoot; ZeroRoot.MakeZero(owner.AmbientDimension-1);
	this->Externalwalls.SetSizeExpandOnTopNoObjectInit(input.Externalwalls.size+1);
	if (owner.flagAnErrorHasOcurredTimeToPanic)
		input.ComputeDebugString(&owner);
	assert(input.IndexInOwnerComplex==this->IndexInOwnerComplex);
	//the extra dimension is going to be the last dimension
	for (int i=0;i<input.Externalwalls.size;i++)
	{ this->Externalwalls.TheObjects[i].normal
			.MakeNormalInProjectivizationFromPointAndNormal
				(ZeroRoot,input.Externalwalls.TheObjects[i].normal);
		this->Externalwalls.TheObjects[i].NeighborsAlongWall.size=0;
		this->Externalwalls.TheObjects[i].MirrorWall.size=0;
	}
	//this->ComputeDebugString(&owner);
	this->Externalwalls.LastObject()->normal.MakeZero(owner.AmbientDimension);
	this->Externalwalls.LastObject()->normal.TheObjects[owner.AmbientDimension-1].MakeOne();
	this->Externalwalls.LastObject()->MirrorWall.size=0;
	this->Externalwalls.LastObject()->MirrorWall.AddObjectOnTop(0);
	this->Externalwalls.LastObject()->NeighborsAlongWall.size=0;
	this->Externalwalls.LastObject()->NeighborsAlongWall.AddObjectOnTop(0);
	this->flagHasZeroPolynomial= input.flagHasZeroPolynomial;
	this->flagPermanentlyZero= input.flagPermanentlyZero;
	this->IndexStartingCrossSectionNormal= input.IndexStartingCrossSectionNormal;
}

void CombinatorialChamber::WireChamberAndWallAdjacencyData
	(	CombinatorialChamberContainer &owner,
		CombinatorialChamber *input)
{	this->LabelWallIndicesProperly();
	input->LabelWallIndicesProperly();
	assert(	this->Externalwalls.size==input->Externalwalls.size+1);
	for (int i=0;i<input->Externalwalls.size;i++)
	{ WallData& currentWall= this->Externalwalls.TheObjects[i];
		WallData& originalWall= input->Externalwalls.TheObjects[i];
		currentWall.NeighborsAlongWall.SetSizeExpandOnTopNoObjectInit
				(originalWall.NeighborsAlongWall.size);
		currentWall.MirrorWall.SetSizeExpandOnTopNoObjectInit
				(originalWall.NeighborsAlongWall.size);
		assert(originalWall.MirrorWall.size==originalWall.NeighborsAlongWall.size);
		for (int j=0;j<currentWall.NeighborsAlongWall.size;j++)
		{ if (originalWall.NeighborsAlongWall.TheObjects[j]==0)
			{ currentWall.NeighborsAlongWall.TheObjects[j]=0;
				currentWall.MirrorWall.TheObjects[j]=0;
			}else
			{ currentWall.NeighborsAlongWall.TheObjects[j]=
					owner.TheObjects[originalWall.NeighborsAlongWall.TheObjects[j]->IndexInOwnerComplex];
				assert(currentWall.NeighborsAlongWall.TheObjects[j]->Externalwalls.size
								==
								originalWall.NeighborsAlongWall.TheObjects[j]->Externalwalls.size+1);
				currentWall.NeighborsAlongWall.TheObjects[j]->LabelWallIndicesProperly();
				currentWall.MirrorWall.TheObjects[j]=&
					currentWall.NeighborsAlongWall.TheObjects[j]->Externalwalls.TheObjects
						[originalWall.MirrorWall.TheObjects[j]->indexInOwnerChamber];
			}
		}
	}
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

void CombinatorialChamberContainer::LabelAllUnexplored()
{	for (int i =0; i<size; i++)
	{	if (!this->TheObjects[i]->flagPermanentlyZero || this->flagMakingASingleHyperplaneSlice)
			this->TheObjects[i]->flagExplored =false;
	}
	FirstNonExploredIndex=0;
}

bool WallData::IsExternalWithRespectToDirection(root &direction)
{	return this->normal.OurScalarProductIsPositive(direction);
}

bool WallData::EveryNeigborIsExplored(bool& aNeighborHasNonZeroPoly)
{ aNeighborHasNonZeroPoly=false;
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]!=0)
		{	if ((!this->NeighborsAlongWall.TheObjects[i]->flagExplored)&&
					(!this->NeighborsAlongWall.TheObjects[i]->flagPermanentlyZero))
				return false;
			else
				aNeighborHasNonZeroPoly=
					aNeighborHasNonZeroPoly	||
					this->NeighborsAlongWall.TheObjects[i]->flagHasZeroPolynomial;
		}
	}
	return true;
}

/*void Facet::InduceFromFacetLowerDimension(Facet& input, CombinatorialChamberContainer& ownerComplex)
{ this->normal.MakeNormalInProjectivizationFromPointAndNormal(ZeroRoot,input.normal);
	for(int i=0;i<input.Owners.size;i++)
	{ CombinatorialChamber* tempPlus;
		CombinatorialChamber* tempMinus;
		tempPlus=ownerComplex.TheObjects[input.Owners.TheObjects[i].PlusOwner->IndexInOwnerComplex];
		tempMinus=ownerComplex.TheObjects[input.Owners.TheObjects[i].MinusOwner->IndexInOwnerComplex];
		this->Owners.AddCouple(tempPlus,tempMinus);
	}
	this->ComputeDebugString();
}*/

bool WallData::FacetContainsChamberOnlyOnce(CombinatorialChamber *owner)
{	bool FoundOnce= false;
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{	if (this->NeighborsAlongWall.TheObjects[i]==owner)
		{	FoundOnce= !FoundOnce;
			if (!FoundOnce){return false;}
		}
	}
	return FoundOnce;
}

void WallData::ElementToString(std::string &output)
{ std::stringstream out;
	std::string tempS; this->normal.ElementToString(tempS);
	out<< "normal: "<< tempS << " Neighbors: ";
	if (this->flagDisplayWallDetails)
	{ for (int i=0;i<this->NeighborsAlongWall.size;i++)
		{ if (this->NeighborsAlongWall.TheObjects[i]!=0)
			{ if (this->NeighborsAlongWall.TheObjects[i]->flagHasZeroPolynomial)
					out <<"Invisible";
				else
					out <<"c";
				out << this->NeighborsAlongWall.TheObjects[i]->DisplayNumber <<", ";
			}
		}
	}
	output= out.str();
}

void WallData::RemoveNeighborhoodBothSides(CombinatorialChamber* owner, CombinatorialChamber *NeighborPointer)
{ //if (CombinatorialChamberContainer::flagAnErrorHasOcurredTimeToPanic)
	//{	assert(this->ContainsNeighborAtMostOnce(NeighborPointer));
	//	assert(owner->OwnsAWall(this));
	//}
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]==NeighborPointer)
		{ this->NeighborsAlongWall.PopIndexSwapWithLast(i);
			this->MirrorWall.TheObjects[i]->RemoveNeighborOneSide(owner);
			this->MirrorWall.PopIndexSwapWithLast(i);
			return;
		}
	}
	assert(false);
}

void WallData::RemoveNeighborOneSide(CombinatorialChamber *NeighborPointer)
{ assert(this->ContainsNeighborAtMostOnce(NeighborPointer));
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]==NeighborPointer)
		{ this->NeighborsAlongWall.PopIndexSwapWithLast(i);
			this->MirrorWall.PopIndexSwapWithLast(i);
			return;
		}
	}
	assert(false);
}

bool WallData::ConsistencyCheck(CombinatorialChamber* owner)
{ for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]!=0)
		{	if (!(	this->MirrorWall.TheObjects[i]->ContainsNeighborExactlyOnce(owner)
						||this->MirrorWall.TheObjects[i]->ContainsMirrorWallExactlyOnce(this)))
				return false;
			root tempRoot; tempRoot.Assign(this->normal); tempRoot.MinusRoot();
			if(!tempRoot.IsEqualTo(this->MirrorWall.TheObjects[i]->normal))
			{	assert(false);
				return false;
			}
		}
	}
	return true;
}

bool WallData::ContainsNeighborAtMostOnce(CombinatorialChamber *neighbor)
{ int NumNeighbors=0;
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]==neighbor)
			NumNeighbors++;
		if (NumNeighbors>1)
		{	assert(false);
			return false;
		}
	}
	return true;
}

bool WallData::ContainsMirrorWallExactlyOnce(WallData* theWall)
{ bool result=false;
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->MirrorWall.TheObjects[i]==theWall)
		{	result=!result;
			if (!result)
			{	assert(false);
				return false;
			}
		}
	}
	return result;
}

bool WallData::ContainsNeighborExactlyOnce(CombinatorialChamber *neighbor)
{int NumNeighbors=0;
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]==neighbor)
			NumNeighbors++;
		if (NumNeighbors>1)
			return false;
	}
	return NumNeighbors==1;
}

void WallData::operator =(const WallData& right)
{ if (this==&right)
		return;
	this->normal.Assign(right.normal);
	this->NeighborsAlongWall.CopyFromBase(right.NeighborsAlongWall);
	this->MirrorWall.CopyFromBase(right.MirrorWall);
}

void WallData::SubstituteNeighbor
	(CombinatorialChamber* oldNeighbor, CombinatorialChamber* newNeighbor,WallData* newNeighborWall)
{	//assert(this->ContainsNeighborAtMostOnce(oldNeighbor));
	for (int i=0;i<this->NeighborsAlongWall.size;i++)
	{ if (this->NeighborsAlongWall.TheObjects[i]==oldNeighbor)
		{ this->NeighborsAlongWall.TheObjects[i]=newNeighbor;
			this->MirrorWall.TheObjects[i]= newNeighborWall;
			return;
		}
	}
	assert(false);
}

void WallData::AddNeighbor(CombinatorialChamber* newNeighbor, WallData* newNeighborWall)
{ this->NeighborsAlongWall.AddObjectOnTop(newNeighbor);
	this->MirrorWall.AddObjectOnTop(newNeighborWall);
}

bool WallData::SplitWall(CombinatorialChamber *BossChamber,
											CombinatorialChamber *NewPlusChamber,
											CombinatorialChamber *NewMinusChamber,
											CombinatorialChamberContainer* ownerComplex,
											roots& ThePlusVertices, roots& TheMinusVertices,
											root& TheKillerFacet, root& direction,
											ListBasicObjects<CombinatorialChamber*>& PossibleBogusNeighbors,
											ListBasicObjects<WallData*>* PossibleBogusWalls,
											GlobalVariables& theGlobalVariables)
{	bool IsPositive, IsNegative; IsPositive = false;	IsNegative = false;
	Rational tempRat;
	for (int j=0;j<ThePlusVertices.size;j++)
	{	if (TheKillerFacet.OurScalarProductIsPositive(ThePlusVertices.TheObjects[j]))
		{	IsPositive=true;
			break;
		}
	}
	for (int j=0;j<TheMinusVertices.size;j++)
	{	if(TheKillerFacet.OurScalarProductIsNegative(TheMinusVertices.TheObjects[j]))
		{	IsNegative=true; break;
			break;
		}
	}
	if (!(IsPositive || IsNegative))
	{	this->ComputeDebugString();
		TheKillerFacet.ComputeDebugString();
		BossChamber->ComputeDebugString(ownerComplex);
		std::string tempS;
		ThePlusVertices.ElementToString(tempS);
		CombinatorialChamberContainer::TheBigDump<<"\nThePlusVertices:\n"<<tempS;
		TheMinusVertices.ElementToString(tempS);
		CombinatorialChamberContainer::TheBigDump<<"\nTheMinusVertices:\n"<<tempS;
		CombinatorialChamberContainer::TheBigDump<<"\n this:\n" <<this->DebugString;
		CombinatorialChamberContainer::TheBigDump<<"\n TheKillerFacet:\n" <<TheKillerFacet.DebugString;
	}
	assert(IsPositive || IsNegative);
	if (ownerComplex->flagAnErrorHasOcurredTimeToPanic)
	{ this->ComputeDebugString();
	}
	if (IsPositive && IsNegative)
	{//we must split the face
		NewPlusChamber->Externalwalls.AddObjectOnTopCreateNew();
		NewMinusChamber->Externalwalls.AddObjectOnTopCreateNew();
		WallData* NewPlusWall;
		WallData* NewMinusWall;
		NewPlusWall  = NewPlusChamber->Externalwalls.LastObject();
		NewMinusWall = NewMinusChamber->Externalwalls.LastObject();
		NewPlusWall->normal.Assign(this->normal);
		NewMinusWall->normal.Assign(this->normal);
		for (int i=0; i<this->NeighborsAlongWall.size;i++)
		{ if (this->NeighborsAlongWall.TheObjects[i]!=0)
			{	this->MirrorWall.TheObjects[i]->SubstituteNeighbor(BossChamber,NewPlusChamber,NewPlusWall);
				this->MirrorWall.TheObjects[i]->AddNeighbor(NewMinusChamber,NewMinusWall);
				PossibleBogusNeighbors.AddObjectOnTopNoRepetitionOfObject(this->NeighborsAlongWall.TheObjects[i]);
				PossibleBogusWalls->AddObjectOnTopNoRepetitionOfObject(this->MirrorWall.TheObjects[i]);
			}
			NewPlusWall->AddNeighbor(this->NeighborsAlongWall.TheObjects[i],this->MirrorWall.TheObjects[i]);
			NewMinusWall->AddNeighbor(this->NeighborsAlongWall.TheObjects[i],this->MirrorWall.TheObjects[i]);
			assert(PossibleBogusNeighbors.size==PossibleBogusWalls->size);
			if (this->NeighborsAlongWall.TheObjects[i]!=0)
			{	if (!this->NeighborsAlongWall.TheObjects[i]->flagPermanentlyZero)
				{	this->NeighborsAlongWall.TheObjects[i]->AddInternalWall
						(TheKillerFacet,this->MirrorWall.TheObjects[i]->normal,direction,ownerComplex, theGlobalVariables);
				}
			}
		}
		return true;
	}
	else
	{	root tempRoot; tempRoot.SetSizeExpandOnTopLight(ownerComplex->AmbientDimension);
		CombinatorialChamber* tempC;
		root FacetInternalPoint; FacetInternalPoint.SetSizeExpandOnTopLight(ownerComplex->AmbientDimension);
		if (IsPositive)
		{tempC= NewPlusChamber;}
		else
		{tempC= NewMinusChamber;}
		tempC->Externalwalls.AddObjectOnTop(*this);
		for (int i=0;i<this->NeighborsAlongWall.size;i++)
		{ if (this->NeighborsAlongWall.TheObjects[i]!=0)
			{	tempC->Externalwalls.LastObject()->MirrorWall.TheObjects[i]->SubstituteNeighbor
					(BossChamber,tempC,tempC->Externalwalls.LastObject());
			}
		}
		return false;
	}
}

bool CombinatorialChamber::MakeFacetFromEdgeAndDirection
	(	WallData& Wall1, WallData& Wall2,CombinatorialChamberContainer& owner,
		root& direction,
		roots & directions, int CurrentIndex,
		root& outputNormal,GlobalVariables& theGlobalVariables)
{	root NewFacetNormal;
//	tempNormal1.SetSizeExpandOnTopLight(root::AmbientDimension);
//	tempNormal2.SetSizeExpandOnTopLight(root::AmbientDimension);
	NewFacetNormal.SetSizeExpandOnTopLight(owner.AmbientDimension);
	Rational a1,a2, b;
	root::RootScalarEuclideanRoot(direction,Wall1.normal,a1);
	root::RootScalarEuclideanRoot(direction,Wall2.normal,a2);
	assert(!a1.IsEqualToZero());
	assert(!a2.IsEqualToZero());
	b.Assign(a1);
	b.DivideBy(a2);
	b.Minus();
	root::RootPlusRootTimesScalar(Wall1.normal,Wall2.normal,b,NewFacetNormal);
	NewFacetNormal.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
	if (!this->IsAValidCandidateForNormalOfAKillerFacet
				(NewFacetNormal,directions,CurrentIndex,owner,theGlobalVariables))
		return false;
	outputNormal.Assign(NewFacetNormal);
	return true;
}

bool WallData::IsInFacetNoBoundaries(root &point)
{	Rational tempRat;
	root::RootScalarEuclideanRoot(point,this->normal,tempRat);
	return (tempRat.IsEqualToZero());
}

bool CombinatorialChamber::IsAValidCandidateForNormalOfAKillerFacet
	(	root& normalCandidate, roots &directions, int CurrentIndex, CombinatorialChamberContainer& owner,
		GlobalVariables& theGlobalVariables)
{	if (owner.theHyperplanes.ContainsObjectHash(normalCandidate)!=-1)
		return true;
	static roots WallBasis;
	WallBasis.size=0;
	for (int i=0;i<=CurrentIndex;i++)
	{	static Rational tempRat;
		root::RootScalarEuclideanRoot(directions.TheObjects[i],normalCandidate,tempRat);
		if (tempRat.IsEqualToZero())
		{	WallBasis.AddRoot(directions.TheObjects[i]);
		}
	}
	if (WallBasis.size<owner.AmbientDimension-1)
		return false;
	int tempI=WallBasis.GetRankOfSpanOfElements(theGlobalVariables);
	if (tempI<owner.AmbientDimension-1)
		return false;
	assert(tempI==owner.AmbientDimension-1);
	owner.theHyperplanes.AddObjectOnTopHash(normalCandidate);
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

void ComplexQN::MultiplyByLargeRational(Rational& r)
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

void CompositeComplexQN::LinearSubstitution(MatrixLargeRational& TheSub)
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

void CompositeComplexQN::MultiplyByLargeRational(Rational &r)
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
	tempQN.MultiplyByLargeRational(RMOne);
	tempQN.Add(*this);
	return tempQN.IsEqualToZero();
}

void CompositeComplexQN::DivideByRational(Rational& r)
{
	Rational tempRat;
	tempRat.Assign(r);
	tempRat.Invert();
	this->MultiplyByLargeRational(tempRat);
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

void ComplexQN::LinearSubstitution(MatrixLargeRational& TheSub)
{	static Rational tempExponent[MaxRank+1];
	static Rational tempRat;
	static BasicComplexNumber tempBC;
	tempBC.Coeff.MakeOne();
	tempBC.Exp.Assign(RZero);
	assert(TheSub.NumCols==this->NumVars);
	for (int i=0;i<TheSub.NumRows-1;i++)
	{ tempExponent[i].MakeZero();
	}
	for (int i=0; i<this->NumVars;i++)
	{ tempRat.Assign(TheSub.elements[0][i]);
    tempRat.MultiplyBy(this->Exponent[i]);
		tempBC.Exp.Add(tempRat);
		for (int j=1;j<TheSub.NumRows;j++)
		{	tempRat.Assign(TheSub.elements[j][i]);
      tempRat.MultiplyBy(this->Exponent[i]);
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


void PolynomialRationalCoeff::MakeLinPolyFromInt(int theDimension,int x1,int x2, int x3,int x4, int x5)
{ root tempRoot;
	tempRoot.InitFromIntegers(theDimension,x1,x2,x3,x4,x5);
	this->MakeLinPolyFromRoot(tempRoot);
}


void PolynomialRationalCoeff::operator=(const PolynomialRationalCoeff& right)
{ this->CopyFromPoly(right);
}

void PolynomialRationalCoeff::MakePolyExponentFromIntRoot
	(intRoot& r, GlobalVariables& theGlobalVariables)
{	this->ClearTheObjects();
	this->NumVars=(short)r.dimension;
	Monomial<Rational>& tempM=theGlobalVariables.monMakePolyExponentFromIntRoot;
	tempM.init((short)r.dimension);
	tempM.Coefficient.Assign(ROne);
	for (int i=0;i<r.dimension;i++)
		tempM.degrees[i]=(short) r.elements[i];
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
	{ this->TheObjects[i].Coefficient.MultiplyByInt(x);
	}
}

void PolynomialRationalCoeff::DivByInteger(int x)
{	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Coefficient.DivideByInteger(x);
	}
}

void PolynomialRationalCoeff::AssignIntegerPoly(IntegerPoly& p)
{	this->Nullify(p.NumVars);
	this->MakeActualSizeAtLeastExpandOnTop(p.size);
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
				(root& direction, root& normal, Rational& Correction, GlobalVariables& theGlobalVariables)
{	Rational tempRat2;
	root::RootScalarEuclideanRoot(direction,normal,tempRat2);
	this->ClearTheObjects();
	this->NumVars=(short)direction.size;
	Monomial<Rational>& tempM=theGlobalVariables.monMakePolyFromDirectionAndNormal;
	for (int i=0;i<direction.size;i++)
	{ tempM.MakeNVarFirstDegree(i,(short)direction.size, normal.TheObjects[i]);
		tempM.Coefficient.DivideBy(tempRat2);
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
			int tempI=MathRoutines::NChooseK(i+1,j);
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
{	this->SetSizeExpandOnTopNoObjectInit(direction.size);
	for (int i=0;i<direction.size;i++)
	{	Rational tempRat; tempRat.Assign(direction.TheObjects[i]); tempRat.Minus();
		this->TheObjects[i].MakeNVarDegOnePoly
						((short)(direction.size+1),i,direction.size,ROne,tempRat);
	}
}

void PolynomialsRationalCoeff::MakeSubNVarForOtherChamber
         (root& direction,root& normal, Rational& Correction, GlobalVariables& theGlobalVariables)
{	this->SetSizeExpandOnTopNoObjectInit(direction.size);
	static PolynomialRationalCoeff TempPoly;
	TempPoly.MakePolyFromDirectionAndNormal(direction,normal, Correction,theGlobalVariables);
	for (int i=0;i<direction.size;i++)
	{	static PolynomialRationalCoeff TempPoly2;
		TempPoly2.CopyFromPoly(TempPoly);
		TempPoly2.TimesConstant(direction.TheObjects[i]);
		TempPoly2.TimesConstant(RMOne);
		this->TheObjects[i].MakeNVarDegOnePoly((short)direction.size,i,ROne);
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

/*int PolynomialRationalCoeff::FindGCMCoefficientDenominators()
{	int result=1;
	for (int i=0;i<this->size;i++)
	{	int tempI=Rational::gcdSigned(result,this->TheObjects[i].Coefficient.den);
		result= result*this->TheObjects[i].Coefficient.den/tempI;
	}
	return result;
}*/
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
		output.NumVars= (short)TheSub.TheQNSub.NumRows;
		return;
	}
	static PolynomialRationalCoeff tempP;
	static Monomial<Rational> tempM;
	tempM.initNoDegreesInit(this->NumVariables);
	tempM.Coefficient.Assign(ROne);
	for (int i=0;i<this->NumVariables;i++)
	{ tempM.degrees[i]= this->degrees[i];
	}
	tempM.Substitution(TheSub.RationalPolyForm,tempP,(short)(TheSub.TheQNSub.NumRows-1));
	static QuasiNumber tempQ((short)TheSub.TheQNSub.NumRows);
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
{/*	static QuasiPolynomial SubstitutedQP;
	this->RationalLinearSubstitution(DirectionSub,SubstitutedQP);
	SubstitutedQP.IntegrateDiscreteFromZeroTo(EndPointSub,output,PrecomputedDiscreteIntegrals);*/
}

//We are integrating with respect to the last variable always!
/*void QuasiMonomial::IntegrateDiscreteFromZeroTo
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
}*/

/*void PrecomputedQuasiPolynomialIntegral::ComputeValue
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
}*/

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



/*void QuasiPolynomial::IntegrateDiscreteInDirectionFromOldChamber
		              (root &direction,root& normal,QuasiPolynomial& OldChamberPoly,
									 QuasiPolynomial &output,
									 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{ static QPSub EndPointSub,DirectionSub, OtherChamberSub;
	static Rational tempRat;
	static root tempRoot; tempRoot.SetSizeExpandOnTopLight(root::AmbientDimension);
	static QuasiPolynomial Accum;
//	static bool tempB=false;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	tempRoot.Assign(normal);
	tempRoot.DivByLargeRational(tempRat);
	LargeIntUnsigned Denominator; tempRoot.FindLCMDenominators(Denominator);
	int Den=Denominator.GetUnsignedIntValueTruncated();
	Accum.ClearTheObjects();
	Accum.NumVars=root::AmbientDimension;
	DirectionSub.MakeSubAddExtraVarForIntegration(direction);
	for (int i=0;i<Den;i++)
	{ Rational tempSmallRat;
		tempSmallRat.init(i,Den);
		EndPointSub.MakeLinearSubIntegrand(normal,direction,tempSmallRat);
		static	QuasiPolynomial tempQP, tempQP1, tempQP2, tempQP3, tempQP4;
		this->IntegrateDiscreteInDirectionFromZeroTo(EndPointSub,DirectionSub,
		                                             tempQP, PrecomputedDiscreteIntegrals);
//		tempQP.ComputeDebugString();
		if (tempQP.IsEqualToZero())
		{ tempSmallRat.init(i,Den);
		}
		else
		{ tempSmallRat.init(Den+i,Den);
		}
		OtherChamberSub.MakeSubNVarForOtherChamber(direction,normal,tempSmallRat);
		OldChamberPoly.RationalLinearSubstitution(OtherChamberSub,tempQP1);
		tempQP.AddPolynomial(tempQP1);
		static QuasiNumber tempQ;
		tempQ.MakeFromNormalAndDirection(normal,direction,i, ROne);
		tempQP.TimesConstant(tempQ);
		Accum.AddPolynomial(tempQP);
	}
	output.CopyFromPoly(Accum);
}*/

/*void QuasiPolynomial::IntegrateDiscreteFromZeroTo
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
}*/

void QuasiPolynomial::Nullify(short numVars)
{ this->ClearTheObjects();
	this->NumVars=numVars;
}

void QuasiPolynomial::IntegrateDiscreteInDirectionFromZeroTo
					(QPSub& EndPointSub, QPSub& DirectionSub,
					 QuasiPolynomial &output,
					 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals)
{	/*static QuasiPolynomial tempQP, Accum;
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
*/}

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

void QuasiPolynomial::Evaluate(intRoot& values, Rational& output)
{ output.MakeZero();
	std::string tempS;
	/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
	{	output.ElementToString(tempS);
	}*/
	Rational tempLRat, tempLRat2;
	for (int i=0;i<this->size;i++)
	{ tempLRat.MakeOne();
		/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{	this->TheObjects[i].ComputeDebugString();
		}*/
		for (int j=0; j<this->NumVars;j++)
		{ for (int k=0;k<this->TheObjects[i].degrees[j];k++)
			{	/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
				{ if (this->TheObjects[i].DebugString=="-1/2ac^{2}")
					{ tempLRat.ElementToString(tempS);
					}
				}*/
				tempLRat.MultiplyByInt(values.elements[j]);
			/*	if (Rational::flagAnErrorHasOccurredTimeToPanic)
				{ if (this->TheObjects[i].DebugString=="-1/2ac^{2}")
					{ tempLRat.ElementToString(tempS);
					}
				}*/
			}
		}
		this->TheObjects[i].Coefficient.Evaluate(values.elements,tempLRat2);
/*		if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{	this->TheObjects[i].ComputeDebugString();
			if (this->TheObjects[i].DebugString=="-1/2ac^{2}")
			{ Stop();
				tempLRat2.ElementToString(tempS);
			}
			tempLRat.ElementToString(tempS);
		}*/
		tempLRat.MultiplyBy(tempLRat2);
		/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{	this->TheObjects[i].ComputeDebugString();
			if (this->TheObjects[i].DebugString=="-1/2ac^{2}")
			{ Stop();
				tempLRat2.ElementToString(tempS);
			}
			tempLRat.ElementToString(tempS);
		}*/
		output.Add(tempLRat);
		/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{	output.ElementToString(tempS);
		}*/
	}
}

void QuasiPolynomial::DivByInteger(int x)
{ Rational tempRat2;
	tempRat2.AssignNumeratorAndDenominator(1,x);
	QuasiNumber tempQN;
	tempQN.AssignLargeRational(this->NumVars,tempRat2);
	this->TimesConstant(tempQN);
}

void QuasiPolynomial::RationalLinearSubstitution
												(QPSub& TheSub,QuasiPolynomial& output)
{	static QuasiPolynomial tempQP;
	static QuasiPolynomial Accum;
	Accum.ClearTheObjects();
	Accum.NumVars= (short)TheSub.TheQNSub.NumRows;
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
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter++;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
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
		{	tempRat.AssignNumeratorAndDenominator(i,n);
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

void rootsCollection::ResetCounters()
{	for (int i=0;i<this->size;i++)
		this->TheObjects[i].size=0;
}

void rootsCollection::ComputeDebugString()
{	std::string tempS;
	this->DebugString.clear();
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempS);
		this->DebugString.append(tempS);
		this->DebugString.append("\n");
	}
}

void rootsCollection::Average(root& output, int Number, int theDimension)
{	root tempRoot; output.MakeZero(theDimension);
	if (this->size==0) {return;}
	for(int i=0;i<Number;i++)
	{	this->TheObjects[i].Average(tempRoot,theDimension);
		output.Add(tempRoot);
	}
	output.DivByInteger(this->size);
}

void CompositeComplexQNSub::MakeLinearSubIntegrand
	(root &normal,root &direction,Rational& Correction,GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff EndPoint;
	EndPoint.MakePolyFromDirectionAndNormal(direction,normal,Correction,theGlobalVariables);
	this->RationalPolyForm.MakeSubstitutionLastVariableToEndPoint(1,EndPoint);
	this->MatrixForCoeffs.init((short)(direction.size+1),1);
	Rational tempLargeRat;
	root::RootScalarEuclideanRoot(direction,normal,tempLargeRat);
	MatrixForCoeffs.elements[0][0].Assign(Correction);
	for (int i=1;i<this->MatrixForCoeffs.NumRows;i++)
	{ MatrixForCoeffs.elements[i][0].Assign(normal.TheObjects[i-1]);
		MatrixForCoeffs.elements[i][0].DivideBy(tempLargeRat);
	}
}

void CompositeComplexQNSub::MakeSubAddExtraVarForIntegration(root &direction)
{ this->RationalPolyForm.MakeSubAddExtraVarForIntegration(direction);
	this->MatrixForCoeffs.init((short)direction.size+2,(short)direction.size);
	this->MatrixForCoeffs.NullifyAll();
	for (int i=0;i<this->MatrixForCoeffs.NumCols;i++)
	{ this->MatrixForCoeffs.elements[i][i].Assign(ROne);
		this->MatrixForCoeffs.elements[direction.size+1][i].Assign(RMOne);
		this->MatrixForCoeffs.elements[direction.size+1][i].MultiplyBy(direction.TheObjects[i]);
	}
}

void CompositeComplexQNSub::MakeSubNVarForOtherChamber(root &direction, root &normal, Rational &Correction,GlobalVariables& theGlobalVariables)
{ this->RationalPolyForm.MakeSubNVarForOtherChamber
		(direction,normal,Correction, theGlobalVariables);
	this->MatrixForCoeffs.init((short)direction.size+1,(short)direction.size);
	Rational tempLargeRat;
	root::RootScalarEuclideanRoot(direction,normal,tempLargeRat);
	tempLargeRat.Minus();
	for (int i=0;i<direction.size;i++)
	{ this->MatrixForCoeffs.elements[0][i].Assign(Correction);
		this->MatrixForCoeffs.elements[0][i].MultiplyBy(direction.TheObjects[i]);
		this->MatrixForCoeffs.elements[0][i].Minus();
		for(int j=0;j<direction.size;j++)
		{ this->MatrixForCoeffs.elements[j+1][i].Assign(normal.TheObjects[j]);
		  this->MatrixForCoeffs.elements[j+1][i].DivideBy(tempLargeRat);
			this->MatrixForCoeffs.elements[j+1][i].MultiplyBy(direction.TheObjects[i]);
			if (i==j) this->MatrixForCoeffs.elements[j+1][i].Add(ROne);
		}
	}
}

void BasicComplexNumber::Assign(const BasicComplexNumber & c)
{ this->Coeff.Assign(c.Coeff);
	this->Exp.Assign(c.Exp);
}

void BasicComplexNumber::init(Rational& coeff, Rational & exp)
{ this->Coeff.Assign(coeff);
	this->Exp.Assign(exp);
}

void BasicComplexNumber::MultiplyBy(BasicComplexNumber &c)
{ this->Coeff.MultiplyBy(c.Coeff);
  this->Exp.Add(c.Exp);
}

void BasicComplexNumber::MultiplyByLargeRational(Rational &c)
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
{ this->Coeff.Assign(r);
	this->Exp.MakeZero();
}

void BasicComplexNumber::AssignLargeRational(const Rational& r)
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

void CompositeComplex::AddLargeRational(Rational& r)
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

void CompositeComplex::MultiplyByLargeRational(Rational &r)
{ if(r.IsEqualTo(RZero))
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
	{ result = Maximum(result,this->TheObjects[i].Exp.DenShort);}
	return result;
}

bool CompositeComplex::SimplifyWRT(short n)
{ if (n==1) return false;
	Rational CoeffFirst;
	bool FoundFirst=false;
	CoeffFirst.MakeZero();
	int NumFound=0;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Exp.DenShort==n)
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
		{ if (this->TheObjects[i].Exp.DenShort== n)
			{	this->PopIndexSwapWithLast(i);
				i--;
			}
		}
		Rational tempRat;
		Rational tempRat2;
		CompositeComplex::PrecomputedCyclotomic.GetSumPrimitiveRoots(n,tempRat);
		tempRat2.Assign(tempRat);
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
		{ this->TheObjects[i].Coeff.Add(r);
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
		tempM.Coefficient.Minus();
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
			output.Minus();
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

void BasicQN::MultiplyByLargeRational(Rational& coeff)
{ this->Coefficient.MultiplyBy(coeff);
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
		int thelcm= MathRoutines::lcm(this->Den,q.Den);
		tempQ.ScaleBy((thelcm/q.Den));
		this->ScaleBy((thelcm/this->Den));
		this->MultiplyBySameDen(tempQ);
	}
}

void BasicQN::GetCoeffInFrontOfLast( Rational &output)
{	if (this->Exp.NumRows>0)
    output.AssignInteger(Exp.elements[0][this->NumVars-1]);
	else
    output.MakeZero();
}

short BasicQN::GaussianEliminationByRows()
{ return this->GaussianEliminationByRowsCol(this->NumVars-1,false);
}

short BasicQN::GaussianEliminationByRowsCol(int Col, bool MoveToRight)
{	short Row=0;
	while(Row<this->Exp.NumRows && Col<this->NumVars && Col>=0)
	{	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ this->ComputeDebugString();
		}
		int PivotRow=-1;
		for (int i=Row;i<this->Exp.NumRows;i++)
		{	if(this->Exp.elements[i][Col]!=0)
			{ PivotRow=i; break;}
		}
		if (PivotRow!=-1)
		{	this->SetPivotRow(PivotRow,Row,Col);
			int ExploringRow= PivotRow+1;
			while (ExploringRow<this->Exp.NumRows)
			{	//this->ComputeDebugString();
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
	this->Exp.init(tempI,NumV);
	this->Nums.init(tempI, NumV);
	this->NumVars=NumV;
	for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0;j<NumV;j++)
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

inline void BasicQN::Evaluate(int*theVars, Rational& output)
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
/*
void BasicQN::MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column)
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
		tempLCM= lcm(tempLCM,column.TheObjects[i].GetDenValueTruncated());
	}
	this->Den=tempLCM;
	for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0; j<root::AmbientDimension;j++)
		{ this->Exp.elements[i][j]=(theMat.elements[i][j].num*tempLCM)/theMat.elements[i][j].den;
			this->Exp.elements[i][j]%=tempLCM;
			if (this->Exp.elements[i][j]<0) {this->Exp.elements[i][j]+=tempLCM;}
		}
		this->Nums.elements[i][0]= (column.TheObjects[i].GetNumValueTruncated()*tempLCM)/
											column.TheObjects[i].GetDenValueTruncated();
		this->Nums.elements[i][0]%=tempLCM;
		if (this->Nums.elements[i][0]<0){this->Nums.elements[i][0]+=tempLCM;}
	}
	//this->ComputeDebugString();
	this->Simplify();
	//this->ComputeDebugString();
}*/

void BasicQN::MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column)
{ this->Coefficient.MakeOne();
	int theDimension = column.size;
	this->NumVars=(short) theDimension;
	this->Exp.init(theMat.NumRows,(short)theDimension);
	this->Nums.init(theMat.NumRows,1);
	int tempLCM= 1;
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{	theMat.ComputeDebugString();
	}
	for (int i=0;i<theMat.NumRows;i++)
	{	for (int j=0;j<theMat.NumCols;j++)
		{ tempLCM= MathRoutines::lcm(tempLCM,theMat.elements[i][j].DenShort);
		}
		tempLCM= MathRoutines::lcm(tempLCM,column.TheObjects[i].DenShort);
	}
	this->Den=tempLCM;
	for (int i=0;i<this->Exp.NumRows;i++)
	{ for (int j=0; j<theDimension;j++)
		{ this->Exp.elements[i][j]=(theMat.elements[i][j].NumShort*tempLCM)/
																theMat.elements[i][j].DenShort;
			this->Exp.elements[i][j]%=tempLCM;
			if (this->Exp.elements[i][j]<0) {this->Exp.elements[i][j]+=tempLCM;}
		}
		this->Nums.elements[i][0]= (column.TheObjects[i].NumShort*tempLCM)/
											column.TheObjects[i].DenShort;
		this->Nums.elements[i][0]%=tempLCM;
		if (this->Nums.elements[i][0]<0){this->Nums.elements[i][0]+=tempLCM;}
	}
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{	this->ComputeDebugString();
		if (this->DebugString=="\\tau_{6}[(3a+5b=1)(4b=2)]")
		{ Stop();
		}
	}
	this->Simplify();
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString();
	}
}


void BasicQN::BasicQNToComplexQN(CompositeComplexQN& output)
{ static CompositeComplexQN tempC;
	MatrixLargeRational TheSub;
	TheSub.init(this->NumVars+1,1);
	output.MakeConst(ROne,this->NumVars);
//	tempC.MakeConst(RZero,this->NumVars);
	for (int i=0;i<this->Exp.NumRows;i++)
	{ PrecomputedTausLocal.GetTaukn(this->Nums.elements[i][0],this->Den,tempC);
		for (int j=0;j<this->NumVars;j++)
		{ TheSub.elements[j+1][0].AssignNumeratorAndDenominator(this->Exp.elements[i][j],1);
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
	if (partFractions::flagAnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString();
	}
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
	int LastRowIndex=theSub.TheQNSub.NumRows-1;
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
	this->NumVars=(short)theSub.TheQNSub.NumRows-1;
	this->ComputeDebugString();
	this->Simplify();
}

void BasicQN::MakeConst(Rational& Coeff, short NumV)
{ this->Exp.Free();
	this->Nums.Free();
	this->Coefficient.Assign(Coeff);
	this->Den=1;
	this->NumVars=NumV;
}

void BasicQN::MakeFromNormalAndDirection(root& normal, root& direction,
																				 int theMod, Rational& coeff)
{ root tempRoot;
	Rational tempLargeRat;
	int theDimension = direction.size;
	root::RootScalarEuclideanRoot(normal,direction,tempLargeRat);
	tempRoot.Assign(normal);
	tempRoot.DivByLargeRational(tempLargeRat);
	this->Exp.init(1,(short)theDimension);
	this->Nums.init(1,1);
	this->Den= tempRoot.FindLCMDenominatorsTruncateToInt();
	tempRoot.MultiplyByInteger(this->Den);
	this->Coefficient.Assign(coeff);
	this->NumVars= (short)theDimension;
	this->Nums.elements[0][0]=theMod;
	for (int i=0;i<theDimension;i++)
	{ this->Exp.elements[0][i]=tempRoot.TheObjects[i].NumShort;
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
	this->Coefficient.Assign(coeff);
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
	this->MakeActualSizeAtLeastExpandOnTop(NeededSize);
	for(int i=0;i<NeededSize;i++)
	{ tempQN.ReadFromFile(input,this->NumVariables);
		this->AddBasicQuasiNumber(tempQN);
	}
}

void QuasiNumber::AddBasicQuasiNumber(BasicQN &q)
{ if (QuasiNumber::flagAnErrorHasOccurredTimeToPanic)
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
{ Rational tempRat;
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

void QuasiNumber::Evaluate(int *theVars, Rational& output)
{ output.MakeZero();
	static Rational tempRat;
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Evaluate(theVars,tempRat);
		output.Add(tempRat);
	}
}

void QuasiNumber::DivideByRational(Rational& r)
{	Rational tempRat;
	tempRat.Assign(r);
	tempRat.Invert();
	this->MultiplyByLargeRational(tempRat);
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
	tempQN.MultiplyByLargeRational(RMOne);
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
	Accum.MakeZero((short)(TheSub.TheQNSub.NumRows-1));
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
	{ theLCM=MathRoutines::lcm(theLCM,this->TheObjects[i].Den);
	}
	if (theLCM>4) {return;}
	int NumCycles = MathRoutines::KToTheNth(theLCM,this->NumVariables);
	SelectionWithMaxMultiplicity theSubset;
	theSubset.initMe2(this->NumVariables, theLCM-1);
//	bool oneValue=true;
	Rational theValue, tempRat;
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
	this->MakeZero(this->NumVariables);
	this->AssignLargeRational(this->NumVariables, theValue);
}

void QuasiNumber::AssignLargeRational(short NumVars, Rational& coeff)
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
{ this->MakeZero((short)normal.size);
	BasicQN tempQ;
	tempQ.MakeFromNormalAndDirection(normal,direction,theMod,coeff);
	this->AddBasicQuasiNumber(tempQ);
}

void QuasiNumber::MakeConst(Rational& Coeff, short NumV)
{	static BasicQN q;
	this->MakeZero(NumV);
	q.MakeConst(Coeff,NumV);
	this->AddBasicQuasiNumber(q);
}

/*void QuasiNumber::MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column)
{ this->ClearTheObjects();
	this->NumVariables= root::AmbientDimension;
	static BasicQN tempBQN;
	tempBQN.MakeQNFromMatrixAndColumn(theMat,column);
	this->AddBasicQuasiNumber(tempBQN);
}*/

void QuasiNumber::MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column)
{ this->ClearTheObjects();
	this->NumVariables= (short)column.size;
	BasicQN tempBQN;
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

void QuasiNumber::MultiplyByLargeRational(Rational &r)
{	if (r.IsEqualTo(RZero))
	{ this->MakeZero(this->NumVariables);
		return;
	}
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyByLargeRational(r);
	}
}

void QPSub::MakeLinearSubIntegrand
	(root &normal,root &direction,Rational& Correction, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff EndPoint;
	EndPoint.MakePolyFromDirectionAndNormal
		(direction,normal,Correction, theGlobalVariables);
	this->RationalPolyForm.MakeSubstitutionLastVariableToEndPoint(1,EndPoint);
	this->TheQNSub.init((short)(normal.size+1),1);
	Rational tempRat;
	root tempRoot; tempRoot.SetSizeExpandOnTopLight(normal.size);
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	tempRoot.Assign(normal);
	tempRoot.DivByLargeRational(tempRat);
	int tempDen=tempRoot.FindLCMDenominatorsTruncateToInt();
	tempDen= MathRoutines::lcm(Correction.DenShort,tempDen);
	tempRoot.MultiplyByInteger(tempDen);
	this->QNSubDen= tempDen;
	this->TheQNSub.elements [normal.size][0]=-(Correction.NumShort*tempDen)/Correction.DenShort;
	this->TheQNSub.elements [normal.size][0]%=tempDen;
	if (this->TheQNSub.elements [normal.size][0]<0)
	{	this->TheQNSub.elements[normal.size][0]+=tempDen;
	}
	for (int i=0;i<this->TheQNSub.NumRows;i++)
	{ this->TheQNSub.elements[i][0]=(normal.TheObjects[i].NumShort)%this->QNSubDen;
		if (this->TheQNSub.elements[i][0]<0){this->TheQNSub.elements[i][0]+=tempDen;}
	}
}

void QPSub::MakeSubAddExtraVarForIntegration(root &direction)
{ this->RationalPolyForm.MakeSubAddExtraVarForIntegration(direction);
	this->TheQNSub.init((short)direction.size+2,(short)direction.size);
	this->TheQNSub.NullifyAll();
	this->QNSubDen=1;
	for (int i=0;i<this->TheQNSub.NumCols;i++)
	{ this->TheQNSub.elements[i][i]=1;
		this->TheQNSub.elements[direction.size][i]= -direction.TheObjects[i].NumShort;
	}
}

void QPSub::MakeSubFromMatrixIntAndDen(MatrixIntTightMemoryFit& theMat, int Den)
{	this->RationalPolyForm.MakeSubFromMatrixIntAndDen(theMat,Den);
	this->QNSubDen=Den;
	this->TheQNSub.Assign(theMat);
}

void QPSub::MakeSubFromPolynomialsRationalCoeff(PolynomialsRationalCoeff& input)
{	this->RationalPolyForm.SetSizeExpandOnTopNoObjectInit(input.size);
	if (input.size<1)
		return;
	int theDimension= input.TheObjects[0].NumVars;
	for(int i=0;i<input.size;i++)
		this->RationalPolyForm.TheObjects[i].CopyFromPoly(input.TheObjects[i]);
	this->TheQNSub.init((short)theDimension+1,(short)theDimension);
	this->TheQNSub.NullifyAll();
	for (int i=0;i<input.size;i++)
	{ for (int k=0;k<input.TheObjects[i].size;k++)
		{	for (int j=0;j<theDimension;j++)
			{ bool IsAConstantTerm=true;
				assert(	input.TheObjects[i].TheObjects[k].degrees[j]==1 ||
								input.TheObjects[i].TheObjects[k].degrees[j]==0);
				if (input.TheObjects[i].TheObjects[k].degrees[j]==1)
				{ this->TheQNSub.elements[i][j]=input.TheObjects[i].TheObjects[k].Coefficient.NumShort;
					assert(IsAConstantTerm);
					IsAConstantTerm=false;
				}
				if (IsAConstantTerm)
				{ this->TheQNSub.elements[theDimension][i]=input.TheObjects[i].TheObjects[k].Coefficient.NumShort;  }
			}
		}
	}
}

void QPSub::MakeSubNVarForOtherChamber(root &direction, root &normal, Rational &Correction, GlobalVariables& theGlobalVariables)
{ this->RationalPolyForm.MakeSubNVarForOtherChamber
		(direction,normal,Correction, theGlobalVariables);
	int theDimension= direction.size;
	this->TheQNSub.init((short)theDimension+1,(short)theDimension);
	Rational tempRat;
	root::RootScalarEuclideanRoot(direction,normal,tempRat);
	root tempRoot; tempRoot.SetSizeExpandOnTopLight(theDimension);
	tempRoot.Assign(normal);
	tempRat.Minus();
	tempRoot.DivByLargeRational(tempRat);
	int tempDen = tempRoot.FindLCMDenominatorsTruncateToInt();
	tempDen= MathRoutines::lcm(tempDen,Correction.DenShort);
	tempRoot.MultiplyByInteger(tempDen);
	int Corr= -(Correction.NumShort*((signed int)Correction.DenShort))/tempDen;
	this->TheQNSub.NullifyAll();
	this->QNSubDen=tempDen;
	for (int i=0;i<theDimension;i++)
	{ this->TheQNSub.elements[theDimension][i]=
			(direction.TheObjects[i].NumShort*Corr)%tempDen;
		if (this->TheQNSub.elements[theDimension][i]<0)
			this->TheQNSub.elements[theDimension][i]+=tempDen;
		for(int j=0;j<theDimension;j++)
		{ this->TheQNSub.elements[i][j]=
				tempRoot.TheObjects[j].NumShort*direction.TheObjects[j].NumShort;
			if (i==j)
				this->TheQNSub.elements[i][j]+=1;
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

/*inline void Rational::MultiplyByLargeRational(int num, unsigned int den)
{ if(this->TryToMultiplyQuickly(sign,den,num))
		return;
	this->theSign*=sign;
	this->InitExtendedFromShortIfNeeded();
	this->NumeratorExtended->MultiplyByUInt(num);
	this->DenominatorExtended->MultiplyByUInt(den);
	this->Simplify();
}*/

void Rational::WriteToFile(std::fstream& output)
{ std::string tempS;
	this->ElementToString(tempS);
	output<<tempS<<" ";
}

inline void Rational::RaiseToPower(int x)
{ if (x==0)
	{	assert(!this->IsEqualToZero());
		this->MakeOne();
		return;
	}
	if (x<0)
	{ x=-x;
		this->Invert();
	}
	Rational tempRat;
	tempRat.Assign(*this);
	for (int i=1;i<x;i++)
	{//if (Rational::flagAnErrorHasOccurredTimeToPanic && i==8)
		//{ std::string tempS;
		//	this->ElementToString(tempS);
		//}
		this->MultiplyBy(tempRat);
	}
}

inline void Rational::Invert()
{ if (this->Extended==0)
	{ int tempI= this->DenShort;
		assert(tempI>0);
		if (this->NumShort<0)
		{	this->DenShort=-this->NumShort;
      this->NumShort=-tempI;
		}
		else
		{	this->DenShort= this->NumShort;
		  this->NumShort= tempI;
		}
		return;
	}
	LargeIntUnsigned tempI;
	tempI.Assign(this->Extended->den);
	this->Extended->den.Assign( this->Extended->num.value);
	this->Extended->num.value.Assign(tempI);
}

void Rational::ReadFromFile(std::fstream& input)
{ std::string tempS;
	input>> tempS;
	int positionInTempS=0;
	this->MakeZero();
  if (tempS=="0")
	{ return;
	}
	if (tempS[0]=='-')
	{ positionInTempS++;
	}
	static LargeIntUnsigned tempNum, tempDen;
	tempNum.MakeZero();
	tempDen.MakeOne();
	bool readingNumerator=true;
	for (unsigned i=positionInTempS;i<tempS.length();i++)
	{ char a= tempS[i];
		if (a=='/')
		{ readingNumerator=false;
			tempDen.MakeZero();
		}else
		{	if (readingNumerator)
			{ tempNum.MultiplyByUInt(10);
				unsigned int x=std::atoi(&a);
				tempNum.AddUInt(x);
			} else
			{	tempDen.MultiplyByUInt(10);
				unsigned int x= std::atoi(&a);
				tempDen.AddUInt(x);
			}
		}
	}
	if (tempS[0]=='-')
	{ this->Minus();
	}
}

inline void Rational::MultiplyByInt(int x)
{ static Rational tempRat;
	tempRat.AssignInteger(x);
	this->MultiplyBy(tempRat);
}

inline void Rational::MultiplyBy(Rational &r)
{ if (r.Extended==0)
	{ if (this->TryToMultiplyQuickly(r.NumShort,r.DenShort))
		{	return;}
	}
	this->InitExtendedFromShortIfNeeded();
	if (r.Extended!=0)
	{	this->Extended->num.MultiplyBy(r.Extended->num);
		this->Extended->den.MultiplyBy(r.Extended->den);
	}
	else
	{ this->Extended->num.MultiplyByInt(r.NumShort);
		this->Extended->den.MultiplyByUInt((unsigned int)r.DenShort);
	}
	this->Simplify();
}

inline void Rational::MultiplyByLargeInt(LargeInt& x)
{ this->InitExtendedFromShortIfNeeded();
	this->Extended->num.MultiplyBy(x);
	this->Simplify();
}

void Rational::MultiplyByLargeIntUnsigned(LargeIntUnsigned& x)
{ this->InitExtendedFromShortIfNeeded();
	this->Extended->num.value.MultiplyBy(x);
	this->Simplify();
}

inline void Rational::DivideBy(Rational &r)
{ if (r.Extended==0)
	{ int tempNum; int tempDen;
    if(r.NumShort<0)
    { tempNum= -(r.DenShort);
      tempDen= -r.NumShort;
    }
    else
    { tempNum=r.DenShort;
      tempDen= r.NumShort;
    }
	  if (this->TryToMultiplyQuickly(tempNum, tempDen))
		{	return;}
	}
	if (this==&r)
	{ this->MakeOne(); return;
	}
	this->InitExtendedFromShortIfNeeded();
	if (r.Extended!=0)
	{	this->Extended->num.value.MultiplyBy(r.Extended->den);
		this->Extended->den.MultiplyBy(r.Extended->num.value);
		this->Extended->num.sign*=r.Extended->num.sign;
	}
	else
	{ int tempNum; int tempDen;
    if(r.NumShort<0)
    { tempNum= -(r.DenShort);
      tempDen= -r.NumShort;
    }
    else
    { tempNum= r.DenShort;
      tempDen= r.NumShort;
    }
	  this->Extended->num.MultiplyByInt(tempNum);
		this->Extended->den.MultiplyByUInt(tempDen);
	}
	this->Simplify();
}

void Rational::GetDenExtended(LargeIntUnsigned& output)
{ if (this->Extended==0)
  {	unsigned int tempI= (unsigned int) this->DenShort;
		output.AssignShiftedUInt(tempI,0);
  }
  else
  { output.Assign(this->Extended->den);
  }
}

void Rational::GetNumExtendedUnsigned(LargeIntUnsigned& output)
{ if (this->Extended==0)
  { if (this->NumShort<0)
			output.AssignShiftedUInt((unsigned int)(-this->NumShort),0);
    else
      output.AssignShiftedUInt((unsigned int) this->NumShort,0);
  }
  else
  { output.Assign(this->Extended->num.value);
  }
}

void Rational::Assign(const Rational &r)
{ this->NumShort= r.NumShort;
	this->DenShort= r.DenShort;
	if (r.Extended==0)
	{ if (this->Extended==0) return;
		this->FreeExtended();
		return;
	}
	this->InitExtendedFromShortIfNeeded();
	this->Extended->num.Assign(r.Extended->num);
	this->Extended->den.Assign(r.Extended->den);
}

inline void Rational::AssignFracValue()
{ if (this->Extended==0)
  { if (this->NumShort==0) return;
    if (this->DenShort==1)
    {this->NumShort=0; this->DenShort=1; return;}
    this->NumShort=this->NumShort% this->DenShort;
    if (this->NumShort<0)
    { this->NumShort+=this->DenShort;
    }
    return;
  }
  if (this->IsEqualToZero()) return;
	if (this->Extended->den.IsEqualToOne())
	{this->MakeZero(); return;}
	LargeIntUnsigned newNum, tempI;
	this->Extended->num.value.DivPositive(this->Extended->den,tempI,newNum);
	this->Extended->num.value.Assign(newNum);
	if (this->Extended->num.IsNegative())
		this->Extended->num.AddLargeIntUnsigned(this->Extended->den);
	assert(this->Extended->num.IsNonNegative());
	this->Simplify();
}

void Rational::AddInteger( int x)
{ Rational tempRat;
	tempRat.AssignNumeratorAndDenominator(x,1);
	this->Add(tempRat);
}

bool Rational::IsGreaterThan(Rational& r)
{ static Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(r);
  return tempRat.IsPositive();
}

void Rational::Subtract(const Rational& r)
{ static Rational temp;
	temp.Assign(r);
	temp.Minus();
	this->Add(temp);
}

void Rational::AssignInteger(int x)
{	this->FreeExtended();
  this->DenShort=1;
  this->NumShort=x;
}

double Rational::DoubleValue()
{ if (this->Extended==0)
    return (double)this->NumShort/(double)this->DenShort;
  else
    return this->Extended->num.GetDoubleValue()/this->Extended->den.GetDoubleValue();
}

void Rational::Add(Rational &r)
{ //static std::string tempS1,tempS2, tempS3, tempS4, tempS5, tempS6, tempS7;
	if (r.Extended==0)
	{	if (this->TryToAddQuickly(r.NumShort,
															r.DenShort))
		{	return;}
	}
	if (this==&r)
	{ this->MultiplyByInt(2); return;
	}
	this->InitExtendedFromShortIfNeeded();
	static Rational tempRat;
	tempRat.Assign(r);
	tempRat.InitExtendedFromShortIfNeeded();
	static LargeInt tempI;
	tempI.Assign(tempRat.Extended->num);
	tempI.value.MultiplyBy(this->Extended->den);
	this->Extended->num.value.MultiplyBy(tempRat.Extended->den);
	this->Extended->num.Add(tempI);
	this->Extended->den.MultiplyBy(tempRat.Extended->den);
	this->Simplify();
}

void Rational::Simplify()
{ if (this->Extended==0)
	{ if (this->NumShort==0)
    { this->DenShort=1;
    }
    else
    { int tempGCD;
      if (this->NumShort>0)
      {  tempGCD= this->gcd(this->NumShort,this->DenShort);
      }
      else
      {  tempGCD= this->gcd(-this->NumShort, this->DenShort);
      }
      this->NumShort/=tempGCD;
      this->DenShort/=tempGCD;
    }
    return;
  }
	if (this->Extended->num.IsEqualToZero())
	{ this->MakeZero();
    return;
  }
	static LargeIntUnsigned tempI;
	LargeIntUnsigned::gcd(this->Extended->den, this->Extended->num.value,tempI);
	/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
	{ std::string tempS1,tempS2,tempS3;
		tempI.ElementToString(tempS1);
		this->ElementToString(tempS2);
	}*/
	static LargeIntUnsigned tempI2;
	this->Extended->den.DivPositive(tempI,this->Extended->den,tempI2);
	this->Extended->num.value.DivPositive(tempI,this->Extended->num.value,tempI2);
	this->ShrinkExtendedPartIfPossible();
}

inline void LargeIntUnsigned::AssignShiftedUInt(unsigned int x, int shift)
{	if (x==0)
	{ this->MakeZero(); return;
	}
	this->SetSizeExpandOnTopNoObjectInit(shift+1);
	for (int i=0;i<shift;i++)
		this->TheObjects[i]=0;
	unsigned int tempX= x%LargeIntUnsigned::CarryOverBound;
	this->TheObjects[shift]=tempX;
  x= x/LargeIntUnsigned::CarryOverBound;
  while (x!=0)
	{ tempX= x%LargeIntUnsigned::CarryOverBound;
	  this->AddObjectOnTop(tempX);
    x= x/LargeIntUnsigned::CarryOverBound;
  }
}

inline void LargeIntUnsigned::AddNoFitSize(LargeIntUnsigned &x)
{ int oldsize= this->size;
	this->SetSizeExpandOnTopNoObjectInit(Maximum(this->size,x.size)+1);
	for (int i=oldsize;i<this->size;i++)
	{ this->TheObjects[i]=0;
	}
	unsigned int CarryOver=0;
	for(int i=0;i<x.size;i++)
	{ this->TheObjects[i]+=x.TheObjects[i]+CarryOver;
		if (this->TheObjects[i]>=LargeIntUnsigned::CarryOverBound)
		{ this->TheObjects[i]-=LargeIntUnsigned::CarryOverBound;
			CarryOver=1;
		}
		else
			CarryOver=0;
	}
	if (CarryOver!=0)
	{	for(int i=x.size;i<this->size;i++)
		{ this->TheObjects[i]+=1;
			if (this->TheObjects[i]>=LargeIntUnsigned::CarryOverBound)
			{ this->TheObjects[i]-=LargeIntUnsigned::CarryOverBound;
			}
			else
				break;
		}
	}
}

void LargeIntUnsigned::Add(LargeIntUnsigned &x)
{ this->AddNoFitSize(x);
	this->FitSize();
}

void LargeIntUnsigned::SubtractSmallerPositive(LargeIntUnsigned& x)
{	unsigned int CarryOver=0;
	assert(this->IsGEQ(x));
	for (int i=0;i<x.size;i++)
	{ if (this->TheObjects[i]<x.TheObjects[i]+CarryOver)
		{ this->TheObjects[i]+=LargeIntUnsigned::CarryOverBound;
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
			{ this->TheObjects[i]=LargeIntUnsigned::CarryOverBound-1;
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

void LargeIntUnsigned::MultiplyBy(LargeIntUnsigned &x, LargeIntUnsigned &output)
{ assert(this!=&output);
	output.SetSizeExpandOnTopNoObjectInit(x.size+output.size);
	for(int i=0;i<output.size;i++)
	{ output.TheObjects[i]=0;
	}
	for (int i=0;i<this->size;i++)
	{ for(int j=0;j<x.size;j++)
		{ unsigned long long tempLong= this->TheObjects[i];
			unsigned long long tempLong2= x.TheObjects[j];
			tempLong= tempLong*tempLong2;
			unsigned long long lowPart= tempLong%LargeIntUnsigned::CarryOverBound;
			unsigned long long highPart= tempLong/LargeIntUnsigned::CarryOverBound;
			static LargeIntUnsigned tempI;
			tempI.AssignShiftedUInt((unsigned int) lowPart,i+j);
			output.AddNoFitSize(tempI);
			tempI.AssignShiftedUInt((unsigned int) highPart,i+j+1);
			output.AddNoFitSize(tempI);
		}
	}
	output.FitSize();
//	assert(this->CheckForConsistensy());
}

void LargeIntUnsigned::FitSize()
{	int newSize=this->size;
	for (int i=this->size-1;i>=1;i--)
	{ if (this->TheObjects[i]==0)
		{	newSize--;
		}
		else
		{ break;
		}
	}
	this->SetSizeExpandOnTopNoObjectInit(newSize);
//	assert(this->CheckForConsistensy());
}

void LargeIntUnsigned::MultiplyByUInt(unsigned int x)
{ static LargeIntUnsigned tempLI;
	tempLI.AssignShiftedUInt(x,0);
	this->MultiplyBy(tempLI);
}

void LargeIntUnsigned::MultiplyBy(LargeIntUnsigned &x)
{ static LargeIntUnsigned tempInt;
	this->MultiplyBy(x,tempInt);
	this->Assign(tempInt);
//	assert(this->CheckForConsistensy());
}

void LargeInt::MultiplyByInt(int x)
{//	if (this->value.size==0) return;
	LargeInt tempI;
	tempI.AssignInt(x);
	this->MultiplyBy(tempI);
}

void LargeIntUnsigned::ElementToString(std::string &output)
{	int base=10;
	int tempI;
	if (this->IsEqualToZero())
	{ output="0";
		return;
	}
	static LargeIntUnsigned tempInt;
	tempInt.Assign(*this);
	std::string tempS;
	std::stringstream out;
	while(!tempInt.IsEqualToZero() )
	{ tempI= tempInt%base;
		out<<tempI;
		tempInt= tempInt/base;
	}
	tempS= out.str();
	output.resize(tempS.size());
	for (unsigned int i=0; i<tempS.size();i++)
	{ output[i]=tempS[tempS.size()-1-i];
	}
//	assert(this->CheckForConsistensy());
}

bool LargeInt::IsEqualTo(LargeInt& x)
{	if (x.sign!=this->sign)
	{ if (x.IsEqualToZero() && this->IsEqualToZero())
		return true; else	return false;
	}
	return this->value.IsEqualTo(x.value);
}

bool LargeInt::CheckForConsistensy()
{ if (this->sign!=-1 && this->sign!=1)
		return false;
	for (int i=0;i<this->value.size;i++)
	{ if (this->value.TheObjects[i]>=LargeIntUnsigned::CarryOverBound)
			return false;
	}
	return true;
}

double LargeInt::GetDoubleValue()
{ return this->GetIntValueTruncated();
}

void LargeInt::ElementToString(std::string& output)
{ std::stringstream out;
	if (this->IsEqualToZero())
	{	output.assign("0");
		return;
	}
	if (this->sign==-1)
		out <<"-";
	std::string tempS;
	this->value.ElementToString(tempS);
	out<< tempS;
	output=out.str();
}

void LargeInt::AssignInt(int x)
{ if (x==0)
	{ this->MakeZero(); return;
	}
	this->sign=1;
	if (x<0)
	{ this->sign=-1; x=-x;
	}
	this->value.AssignShiftedUInt( (unsigned int)x,0);
//	assert(this->CheckForConsistensy());
}

inline void LargeInt::AddInt(int x)
{ static LargeInt tempInt;
	tempInt.AssignInt(x);
	this->Add(tempInt);
}

void LargeInt::AddLargeIntUnsigned(LargeIntUnsigned& x)
{ if (this->sign==1)
  { this->value.Add(x);
    return;
  }
  if (this->value.IsGEQ(x))
  { this->value.SubtractSmallerPositive(x);
  }
  else
  { LargeIntUnsigned tempI;
    tempI.Assign(x);
    tempI.SubtractSmallerPositive(this->value);
    this->value.Assign(tempI);
  }
}

void LargeInt::Add(LargeInt &x)
{ if (this->sign==x.sign)
	{ this->value.Add(x.value);
	}
	else
	{ if (this->value.IsGEQ(x.value))
		{	this->value.SubtractSmallerPositive(x.value);
		}
		else
		{ LargeIntUnsigned tempI;
			tempI.Assign(this->value);
			this->value.Assign(x.value);
			this->value.SubtractSmallerPositive(tempI);
			this->sign= x.sign;
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
void LargeIntUnsigned::DivPositive
			(	LargeIntUnsigned &x,
				LargeIntUnsigned& quotientOutput,
				LargeIntUnsigned& remainderOutput) const
{	static LargeIntUnsigned remainder,quotient,divisor;
	remainder.Assign(*this);
	divisor.Assign(x);
	quotient.MakeZero();
	while (remainder.IsGEQ(divisor))
	{	unsigned int q;
		static LargeIntUnsigned current, Total;
		if (	 remainder.TheObjects[remainder.size-1]> divisor.TheObjects[divisor.size-1])
		{	q=remainder.TheObjects[remainder.size-1]/(divisor.TheObjects[divisor.size-1]+1);
			current.AssignShiftedUInt(q,remainder.size-divisor.size);
		}
		else
		{ if (remainder.size==divisor.size)
			{ current.AssignShiftedUInt(1,0);
			}
			else
			{	q=this->CarryOverBound/(divisor.TheObjects[divisor.size-1]+1);
				current.AssignShiftedUInt(q, remainder.size- divisor.size-1);
				current.MultiplyByUInt(remainder.TheObjects[remainder.size-1]);
			}
		}
		Total.Assign(divisor);
		Total.MultiplyBy(current);
		remainder.SubtractSmallerPositive(Total);
		quotient.Add(current);
	}
	remainderOutput.Assign(remainder);
	quotientOutput.Assign(quotient);
//	assert(remainderOut.CheckForConsistensy());
}
#ifdef WIN32
#pragma warning(default:4244)//warning 4244: data loss from conversion
#endif

int LargeIntUnsigned::GetUnsignedIntValueTruncated()
{	return  (int) this->TheObjects[0];
}

bool LargeIntUnsigned::IsEqualTo(LargeIntUnsigned &right)
{ if (this->size!=right.size) return false;
	for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]!=right.TheObjects[i])
			return false;
	return true;
}

double LargeIntUnsigned::GetDoubleValue()
{ //must be rewritten
	return this->GetUnsignedIntValueTruncated();
}

void LargeIntUnsigned::gcd(LargeIntUnsigned &a, LargeIntUnsigned &b, LargeIntUnsigned &output)
{ static LargeIntUnsigned p,q,r,temp;
	static std::string tempSP, tempSQ, tempSR, tempS;
	p.Assign(a);
	q.Assign(b);
	/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
	{ p.ElementToString(tempSP);
		q.ElementToString(tempSQ);
		r.ElementToString(tempSR);
		temp.ElementToString(tempS);
	}*/
	while(!q.IsEqualToZero() )
	{ /*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}*/
		p.DivPositive(q,temp,r);
		/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}*/
		p.Assign(q);
		/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}*/
		q.Assign(r);
		/*if (Rational::flagAnErrorHasOccurredTimeToPanic)
		{ p.ElementToString(tempSP);
			q.ElementToString(tempSQ);
			r.ElementToString(tempSR);
			temp.ElementToString(tempS);
		}*/
	}
	output.Assign(p);
//	assert(output.CheckForConsistensy());
}

void LargeInt::MakeZero()
{ this->value.MakeZero();
	this->sign=1;
}

void LargeInt::Assign(const LargeInt &x)
{ this->sign=x.sign;
	this->value.CopyFromBase(x.value);
//	assert(this->CheckForConsistensy());
}

LargeInt LargeInt::operator/(int x)const
{ LargeInt result;
	LargeIntUnsigned remainder;
	LargeIntUnsigned tempX;
	int absX=x; signed char signX=1; if (x<0){ signX=-1; absX=-absX;}
	tempX.AssignShiftedUInt(absX,0);
	this->value.DivPositive(tempX,result.value,remainder);
//	assert(result.CheckForConsistensy());
	result.sign=this->sign* signX;
	return result;
}

LargeInt LargeInt::operator/(LargeInt& x)const
{ LargeInt result;
	LargeInt remainder;
	this->value.DivPositive(x.value,result.value,remainder.value);
	result.sign= this->sign* x.sign;
	assert(result.CheckForConsistensy());
	return result;
}

int LargeInt::operator%(int x)
{ assert(x>0);
	LargeIntUnsigned result;
	LargeIntUnsigned remainder;
	LargeIntUnsigned tempX;
	if (x<0) {x=-x;}
	tempX.AssignShiftedUInt(x,0);
	this->value.DivPositive(tempX,result,remainder);
	if (remainder.size ==0)
	{return 0;
	}else
	{ return remainder.TheObjects[0];
	}
}

LargeIntUnsigned LargeIntUnsigned::operator/(unsigned int x)const
{ static LargeIntUnsigned result;
	static LargeIntUnsigned remainder;
	static LargeIntUnsigned tempX;
	tempX.AssignShiftedUInt(x,0);
	this->DivPositive(tempX,result,remainder);
//	assert(result.CheckForConsistensy());
	return result;
}

LargeIntUnsigned LargeIntUnsigned::operator/(LargeIntUnsigned& x)const
{ static LargeIntUnsigned result;
	static LargeIntUnsigned remainder;
	this->DivPositive(x,result,remainder);
	return result;
}

int LargeIntUnsigned::operator%(unsigned int x)
{ static LargeIntUnsigned result;
	static LargeIntUnsigned remainder;
	static LargeIntUnsigned tempX;
	tempX.AssignShiftedUInt(x,0);
	this->DivPositive(tempX,result,remainder);
	return remainder.TheObjects[0];
}

LargeInt::LargeInt(const LargeInt& x)
{ this->Assign(x);
//	this->CheckForConsistensy();
}

void LargeIntUnsigned::MakeOne()
{ this->SetSizeExpandOnTopNoObjectInit(1);
	this->TheObjects[0]=1;
}

void LargeIntUnsigned::MakeZero()
{ this->SetSizeExpandOnTopNoObjectInit(1);
	this->TheObjects[0]=0;
}

bool LargeIntUnsigned::IsGEQ(LargeIntUnsigned& x)
{ if (this->size>x.size)
		return true;
	if (this->size<x.size)
		return false;
	for (int i=this->size-1;i>=0;i--)
	{ if (x.TheObjects[i]>this->TheObjects[i])
			return false;
		if (x.TheObjects[i]<this->TheObjects[i])
			return true;
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
			tempM.degrees[j]=q.TheObjects[i].degrees[j];
		q.TheObjects[i].Coefficient.QNtoComplex(tempM.Coefficient);
		this->AddMonomial(tempM);
	}
}

void partFractionPolynomials::ComputeQuasiPolynomial
			(QuasiPolynomial &output, bool RecordNumMonomials, int theDimension, GlobalVariables& theGlobalVariables)
{	output.Nullify((short)theDimension);
	for (int i=0;i<this->size;i++)
	{	if (RecordNumMonomials)
		{	std::stringstream out;
			out<<i<<" out of "<<this->size<<" accounted for";
			::IndicatorWindowGlobalVariables.ProgressReportString4= out.str();
		}
		QuasiPolynomial& tempQP	= theGlobalVariables.QPComputeQuasiPolynomial;
		QuasiNumber& tempQN			= theGlobalVariables.QNComputeQuasiPolynomial;
		tempQP.AssignPolynomialRationalCoeff(this->TheObjects[i]);
		//tempQP.ComputeDebugString();
		tempQN.MakeQNFromMatrixAndColumn
			(	this->theNormals,
				this->LatticeIndicators.TheObjects[i]);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
			tempQN.ComputeDebugString();
		//tempQN.ComputeDebugString();
		tempQP.TimesConstant(tempQN);
		//tempQP.ComputeDebugString();
		if (partFraction::MakingConsistencyCheck)
		{ std::string tempS;
			Rational tempLR;
			tempQP.Evaluate(partFraction::theVectorToBePartitioned,tempLR);
			//assert (tempLR.den.IsEqualTo(LIOne));
			partFractions::CheckSum.Add(tempLR);
			//partFraction::CheckSum.Add(tempLR);
//			assert(partFractions::CheckSum.den.IsEqualTo(LIOne));
		}
		output.AddPolynomial(tempQP);
		//output.ComputeDebugString();
	}
}

void partFractionPolynomials::initLatticeIndicatorsFromPartFraction
	(partFractions& ownerExpression,partFraction &owner,GlobalVariables& theGlobalVariables, int theDimension)
{	this->LatticeIndicators.size=0;
	this->theNormals.init((short)theDimension,(short)theDimension);
	for (int i=0;i<theDimension;i++)
	{ int tempI=owner.IndicesNonZeroMults.TheObjects[i];
		assert(owner.TheObjects[tempI].Elongations.size==1);
		for(int j=0;j<theDimension;j++)
			this->theNormals.elements[j][i].AssignInteger
					(ownerExpression.RootsToIndices.TheObjects
						[tempI].elements[j]*
						owner.TheObjects[tempI].Elongations.TheObjects[0]);
	}
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		theNormals.ComputeDebugString();
	this->theNormals.Invert(theGlobalVariables);
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		theNormals.ComputeDebugString();
	this->size=0;
}

void partFractionPolynomials::AddPolynomialLargeRational
				(root& rootLatticeIndicator,PolynomialRationalCoeff& input)
{	root tempRoot, tempRoot2;
	int theDimension= rootLatticeIndicator.size;
	tempRoot.SetSizeExpandOnTopLight(theDimension);
	tempRoot2.SetSizeExpandOnTopLight(theDimension);
	static Rational tempRat, tempRat2;
	//if (partFraction::MakingConsistencyCheck)
	//{ this->CheckConsistency(rootLatticeIndicator,input);
	//}
	//tempRoot.MakeZero();
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ this->theNormals.ComputeDebugString();
	}
	for(int i=0;i<this->theNormals.NumRows;i++)
	{ tempRoot.TheObjects[i].MakeZero();
		if (partFraction::MakingConsistencyCheck)
		{ tempRoot2.TheObjects[i].MakeZero();
		}
		for(int j=0;j<theDimension;j++)
		{ tempRat.Assign(this->theNormals.elements[i][j]);
			tempRat.MultiplyBy(rootLatticeIndicator.TheObjects[j]);
			tempRoot.TheObjects[i].Add(tempRat);
			if (partFraction::MakingConsistencyCheck)
			{	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
				{ Stop();
				}
				tempRat2.AssignInteger(partFraction::theVectorToBePartitioned.elements[j]);
				tempRat2.MultiplyBy(this->theNormals.elements[i][j]);
				tempRoot2.TheObjects[i].Add(tempRat2);
			}
		}
		tempRoot.TheObjects[i].AssignFracValue();
		if (partFraction::MakingConsistencyCheck)
		{ tempRoot2.TheObjects[i].AssignFracValue();
		}
	}
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ tempRoot.ComputeDebugString();
		tempRoot2.ComputeDebugString();
	}
	if (partFraction::MakingConsistencyCheck)
	{	if (tempRoot2.IsEqualTo(tempRoot))
		{ Rational tempLRat;
			std::string tempS1, tempS2;
			if (partFraction::flagAnErrorHasOccurredTimeToPanic)
			{ input.ComputeDebugString();
			}
			input.Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
			tempLRat.ElementToString(tempS1);
	//		tempLRat.ElementToString(tempS1);
			partFraction::CheckSum.Add(tempLRat);
			if (partFraction::flagAnErrorHasOccurredTimeToPanic)
			{ partFraction::CheckSum.ElementToString(tempS1);
				RandomCodeIDontWantToDelete::theFirstEvilString.append(tempS1);
				RandomCodeIDontWantToDelete::theFirstEvilString.append("\n");
			}
		}
	}
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ rootLatticeIndicator.ComputeDebugString();
	}

	int x= this->LatticeIndicators.IndexOfObject(tempRoot);
	if (x==-1)
	{ this->LatticeIndicators.AddRoot(tempRoot);
		this->AddObjectOnTop(input);
	}
	else
	{ this->TheObjects[x].AddPolynomial(input);
		if (partFraction::MakingConsistencyCheck)
		{	if (tempRoot2.IsEqualTo(tempRoot))
			{ Rational tempLRat;
				this->TheObjects[x].Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
				assert(tempLRat.IsEqualTo(partFraction::CheckSum));
			}
		}
	}
}

void partFractionPolynomials::CheckConsistency
				(root& RootLatticeIndicator,PolynomialRationalCoeff& input)
{ root tempRoot2;
	int theDimension= RootLatticeIndicator.size;
	tempRoot2.AssignIntRoot(partFraction::theVectorToBePartitioned);
	tempRoot2.Subtract(RootLatticeIndicator);
	intRoot tempIntRoot;
	tempIntRoot.AssignRoot(tempRoot2);
	root tempRoot; tempRoot.SetSizeExpandOnTopLight(theDimension);
	Rational tempRat;
	for(int i=0;i<this->theNormals.NumRows;i++)
	{ tempRoot.TheObjects[i].MakeZero();
		for(int j=0;j<theDimension;j++)
		{ tempRat.Assign(this->theNormals.elements[i][j]);
			tempRat.MultiplyBy(tempRoot2.TheObjects[j]);
			tempRoot.TheObjects[i].Add(tempRat);
		}
		tempRoot.TheObjects[i].AssignFracValue();
	}
	Rational tempLRat;
	if (tempRoot.IsEqualToZero())
	{ input.Evaluate( tempIntRoot,tempLRat);
		std::string tempS;
		tempLRat.ElementToString(tempS);
		assert(tempS=="1");
	}
}

bool partFraction::RemoveRedundantShortRootsClassicalRootSystem
	(partFractions& owner, root* Indicator, GlobalVariables& theGlobalVariables, int theDimension)
{ bool result=false;
	if (Indicator!=0)
		if (!this->rootIsInFractionCone(owner,*Indicator, theGlobalVariables))
			return false;
	for (int i=0;i<owner.RootsToIndices.
										IndicesRedundantShortRoots.CardinalitySelection;i++  )
	{ int tempI1, tempI2;
		tempI1= owner.RootsToIndices.IndicesRedundantShortRoots.elements[i];
		tempI2= owner.RootsToIndices.IndicesDoublesOfRedundantShortRoots[tempI1];
		if(tempI2!=-1)
		{	if (	this->TheObjects[tempI1].Multiplicities.size>0 &&
						this->TheObjects[tempI2].Multiplicities.size>0)
			{ IntegerPoly& tempP=theGlobalVariables.IPRemoveRedundantShortRootsClassicalRootSystem;
				assert(this->TheObjects[tempI1].Multiplicities.size==1 && this->TheObjects[tempI2].Multiplicities.size==1);
				tempP.ClearTheObjects();
				tempP.NumVars= (short)theDimension;
				tempP.MakePolyExponentFromIntRoot(owner.RootsToIndices.TheObjects[tempI1]);
				tempP.AddConstant(IOne);
				//tempP.ComputeDebugString();
				static IntegerPoly ComputationalBuffer;
				ComputationalBuffer.AssignPolynomialLight(this->Coefficient);
				for (int j=0;j<this->TheObjects[tempI1].Multiplicities.TheObjects[0];j++)
					ComputationalBuffer.MultiplyBy(tempP);
				this->Coefficient.AssignPolynomial(ComputationalBuffer);
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

bool partFraction::RemoveRedundantShortRoots
	(partFractions& owner, root* Indicator, GlobalVariables& theGlobalVariables, int theDimension)
{ bool result=false;
	Rational StartCheckSum,LocalCheckSum, LocalCheckSum2;
	//if (partFraction::MakingConsistencyCheck)
	//{	this->ComputeDebugString();
	//	this->ComputeOneCheckSum(StartCheckSum);
	//}
	if (Indicator!=0)
		if (!this->rootIsInFractionCone(owner,*Indicator,theGlobalVariables))
			return false;
	static IntegerPoly ComputationalBufferCoefficient;
	static ::PolyPartFractionNumerator ComputationalBufferCoefficientNonExpanded;
	ComputationalBufferCoefficient.AssignPolynomialLight(this->Coefficient);
	this->CoefficientNonExpanded.ComputePolyPartFractionNumerator
		(ComputationalBufferCoefficientNonExpanded, theDimension);

	for (int k=0;k<this->IndicesNonZeroMults.size;k++  )
	{ int currentIndex=this->IndicesNonZeroMults.TheObjects[k];
		::oneFracWithMultiplicitiesAndElongations& currentFrac =
				 this->TheObjects[currentIndex];
		int LCMElongations = currentFrac.GetLCMElongations();
		std::string tempS, tempS1, tempS2;
		owner.RootsToIndices.TheObjects[currentIndex].ElementToString(tempS);
		while (currentFrac.Elongations.size>1)
		{ for (int i=0;i<currentFrac.Elongations.size;i++)
			{	int ElongationValue=currentFrac.Elongations.TheObjects[i];
				if (ElongationValue!=LCMElongations)
				{ int numSummands=LCMElongations/ElongationValue;
					if (this->UncoveringBrackets)
					{	static IntegerPoly tempP;
						this->GetNElongationPoly(owner,currentIndex,ElongationValue,numSummands,tempP,theDimension);
						//tempP.ComputeDebugString();
						tempP.RaiseToPower(currentFrac.Multiplicities.TheObjects[i]);
						//tempP.ComputeDebugString();
						ComputationalBufferCoefficient.MultiplyBy(tempP);
					}
					else
					{ static ::PolyPartFractionNumerator tempP;
						this->GetNElongationPoly(owner,currentIndex, ElongationValue, numSummands, tempP,theDimension);
						tempP.RaiseToPower(currentFrac.Multiplicities.TheObjects[i]);
//						this->CoefficientNonExpanded.ComputeDebugString();
						ComputationalBufferCoefficientNonExpanded.MultiplyBy(tempP);
//						this->CoefficientNonExpanded.ComputeDebugString();
					}
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
	this->CoefficientNonExpanded.AssignPolyPartFractionNumerator(ComputationalBufferCoefficientNonExpanded);
	this->Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
	return result;
}

bool partFraction::reduceOnceTotalOrderMethod(partFractions &Accum, GlobalVariables& theGlobalVariables)
{ for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ for (int j=0;j<this->IndicesNonZeroMults.size;j++)
		{ //assert (this->IndicesNonZeroMults[i]<this->IndicesNonZeroMults[j]);
			int AminusBindex = Accum.RootsToIndices.
												 TableAllowedAminusB[this->IndicesNonZeroMults.TheObjects[i]]
												 [this->IndicesNonZeroMults.TheObjects[j]];
			int Aminus2Bindex = Accum.RootsToIndices.
												  TableAllowedAminus2B[this->IndicesNonZeroMults.TheObjects[i]]
												  [this->IndicesNonZeroMults.TheObjects[j]];
			if (AminusBindex!=-1 &&	AminusBindex>this->IndicesNonZeroMults.TheObjects[j])
			{ this->decomposeAMinusNB( this->IndicesNonZeroMults.TheObjects[i],
																this->IndicesNonZeroMults.TheObjects[j],1,
																AminusBindex, Accum,theGlobalVariables);
				return true;
			}
			if (Aminus2Bindex!=-1 &&	Aminus2Bindex>this->IndicesNonZeroMults.TheObjects[j])
			{ this->decomposeAMinusNB( this->IndicesNonZeroMults.TheObjects[i],
																this->IndicesNonZeroMults.TheObjects[j],2,
																Aminus2Bindex, Accum,theGlobalVariables);
				return true;
			}
		}
	}
//	Accum.Add(*this);
	return false;
}

/*bool partFraction::reduceOnceOrlikSolomonBasis(partFractions& Accum)
{ static MatrixLargeRational tempMat;
	if (!this->flagUsingPrecomputedOrlikSolomonBases)
	{ static roots tempRoots;
		tempRoots.size=0;
		for(int i=this->IndicesNonZeroMults.size-1;i>=0;i++)
		{ int tempIndex= this->IndicesNonZeroMults.TheObjects[i];
			tempRoots.AddIntRoot(this->RootsToIndices.TheObjects[tempIndex]);
			tempRoots.TheObjects[tempRoots.size-1].MultiplyByInteger
				(this->TheObjects[tempIndex].GetLargestElongation());
			bool ShouldDecompose = tempRoots.GetLinearDependence(tempMat);
			if (ShouldDecompose)
		}
		return true;
	}
	else
	{ return true;
	}
}*/

bool partFraction::reduceOnceGeneralMethodNoOSBasis
	(partFractions &Accum, GlobalVariables& theGlobalVariables)
{ static roots tempRoots;
	static MatrixLargeRational tempMat;
	tempRoots.size=0;
	int IndexInLinRelationOfLastGainingMultiplicityIndex=-1;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ intRoot tempRoot; tempRoot.dimension= Accum.AmbientDimension;
		int currentIndex= this->IndicesNonZeroMults.TheObjects[i];
		if (currentIndex== this->LastDistinguishedIndex)
		{ IndexInLinRelationOfLastGainingMultiplicityIndex=i;
		}
		tempRoot= Accum.RootsToIndices.TheObjects[currentIndex];
		tempRoot.MultiplyByInteger
			(this->TheObjects[currentIndex].GetLargestElongation());
		tempRoots.AddIntRoot(tempRoot);
		bool ShouldDecompose;
		ShouldDecompose = tempRoots.GetLinearDependence(tempMat);
	//	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	//	{ tempMat.ComputeDebugString();
	//	}
	//	tempMat.ComputeDebugString();
		if (ShouldDecompose && this->LastDistinguishedIndex!=-1)
		{ if (IndexInLinRelationOfLastGainingMultiplicityIndex==-1)
				ShouldDecompose=false;
			else
				ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
		}
		if (ShouldDecompose)
		{	if (this->flagAnErrorHasOccurredTimeToPanic)
				this->ComputeDebugString(Accum,theGlobalVariables);
			this->DecomposeFromLinRelation(tempMat,Accum, theGlobalVariables);
			return true;
		}
	}
	this->LastDistinguishedIndex=-1;
	return false;
	//tempRoots.r
}

bool partFraction::reduceOnceGeneralMethod(partFractions &Accum, GlobalVariables& theGlobalVariables)
{ static roots tempRoots;
	static MatrixLargeRational tempMat;
	tempRoots.size=0;
	if (this->flagAnErrorHasOccurredTimeToPanic)
		this->ComputeDebugString(Accum,theGlobalVariables);
	this->LastDistinguishedIndex=
		this->getSmallestNonZeroIndexGreaterThanOrEqualTo(Accum,this->LastDistinguishedIndex);
	int IndexInLinRelationOfLastGainingMultiplicityIndex=-1;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ intRoot tempRoot; tempRoot.dimension= Accum.AmbientDimension;
		int currentIndex= this->IndicesNonZeroMults.TheObjects[i];
		if (currentIndex== this->LastDistinguishedIndex)
			IndexInLinRelationOfLastGainingMultiplicityIndex=i;
		tempRoot= Accum.RootsToIndices.TheObjects[currentIndex];
		tempRoot.MultiplyByInteger
			(this->TheObjects[currentIndex].GetLargestElongation());
		tempRoots.AddIntRoot(tempRoot);
		bool ShouldDecompose;
		ShouldDecompose = tempRoots.GetLinearDependence(tempMat);
	//	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	//	{ tempMat.ComputeDebugString();
	//	}
	//	tempMat.ComputeDebugString();
		if ( ShouldDecompose &&
				 (this->LastDistinguishedIndex!=-1 ||
					this->LastDistinguishedIndex==Accum.RootsToIndices.size))
		{ if (IndexInLinRelationOfLastGainingMultiplicityIndex==-1)
				ShouldDecompose=false;
			else
				ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
		}

		if (ShouldDecompose)
		{	if (this->flagAnErrorHasOccurredTimeToPanic)
			{ this->ComputeDebugString(Accum,theGlobalVariables);
				tempMat.ComputeDebugString();
			}
			this->DecomposeFromLinRelation(tempMat,Accum, theGlobalVariables);
			if (this->flagAnErrorHasOccurredTimeToPanic)
			{ this->ComputeDebugString(Accum,theGlobalVariables);
				Accum.ComputeDebugString(theGlobalVariables);
			}
			//this->ComputeDebugString();
			return true;
		}
	}
	return false;
	//tempRoots.r
}



int partFraction::SizeWithoutDebugString()
{	int Accum =0;
	Accum+=this->Coefficient.SizeWithoutObjects();
	Accum+=this->::ListBasicObjectsLight<oneFracWithMultiplicitiesAndElongations>::SizeWithoutObjects();
	Accum+=this->size*sizeof(oneFracWithMultiplicitiesAndElongations);
	Accum+=	this->IndicesNonZeroMults.SizeWithoutObjects();
	return Accum;
}

void partFraction::AssignDenominatorOnly(const partFraction& p)
{ this->CopyFromLight(p);
	this->Coefficient.NumVars=p.Coefficient.NumVars;
	this->ComputeIndicesNonZeroMults();
}

void partFraction::Assign(const partFraction& p)
{ this->CopyFromLight(p);
	this->Coefficient.AssignPolynomialLight(p.Coefficient);
	this->CoefficientNonExpanded.AssignPolyPartFractionNumeratorLight(p.CoefficientNonExpanded);
	this->IndicesNonZeroMults.CopyFromBase(p.IndicesNonZeroMults);
	this->IsIrrelevant= p.IsIrrelevant;
	this->RelevanceIsComputed= p.RelevanceIsComputed;
	this->LastDistinguishedIndex=p.LastDistinguishedIndex;
}

void partFraction::AssignNoIndicesNonZeroMults(partFraction& p)
{ this->CopyFromLight(p);
	this->Coefficient.AssignPolynomialLight(p.Coefficient);
	this->CoefficientNonExpanded.AssignPolyPartFractionNumeratorLight(p.CoefficientNonExpanded);
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

int partFraction::ElementToString
	(	partFractions& owner, std::string &output, bool LatexFormat,
		bool includeVPsummand, bool includeNumerator,
		GlobalVariables& theGlobalVariables)
{ static MatrixIntTightMemoryFit tempMat;
	return this->ElementToStringBasisChange
		(owner,tempMat,false,output,LatexFormat, includeVPsummand, includeNumerator, theGlobalVariables);
}

int partFraction::GetNumMonomialsInNumerator()
{ if (this->UncoveringBrackets)
		return this->Coefficient.size;
	else
		return this->CoefficientNonExpanded.size;
}


void partFraction::WriteToFile(std::fstream &output, GlobalVariables&  theGlobalVariables)
{ output<<"Fraction_start: " <<this->size<<"\n";
	for (int j=0;j<this->size;j++)
	{ output <<this->TheObjects[j].Multiplicities.size <<" ";
	  for (int i=0;i<this->TheObjects[j].Multiplicities.size;i++)
	  { output << this->TheObjects[j].Multiplicities.TheObjects[i] <<" "
             << this->TheObjects[j].Elongations.TheObjects[i] <<" ";
	  }
	}
	output<<"\nCoefficient:\n";
	this->Coefficient.WriteToFile(output);
	output<<"Fraction_end\n";
}

void partFraction::ReadFromFile
	(partFractions& owner,std::fstream &input, GlobalVariables&  theGlobalVariables, int theDimension)
{ std::string tempS;
  int tempI;
  input>>tempS >>tempI;
  assert(tempI==owner.RootsToIndices.size);
  this->init(tempI);
	for (int j=0;j<this->size;j++)
	{ input >>tempI;
    this->TheObjects[j].Multiplicities.SetSizeExpandOnTopLight(tempI);
    this->TheObjects[j].Elongations.SetSizeExpandOnTopLight(tempI);
	  for (int i=0;i<this->TheObjects[j].Multiplicities.size;i++)
	  { input >> this->TheObjects[j].Multiplicities.TheObjects[i]
             >> this->TheObjects[j].Elongations.TheObjects[i];
	  }
	}
	input >>tempS;
	this->Coefficient.ReadFromFile(input,(short)theDimension);
	input>>tempS;
	this->ComputeIndicesNonZeroMults();
}

void partFraction::UncoverBracketsNumerator(GlobalVariables&  theGlobalVariables, int theDimension)
{ if (this->UncoveringBrackets)
		return;
//	this->CoefficientNonExpanded.ComputeDebugString();
	static ::IntegerPoly ComputationalBufferCoefficient;
	static ::PolyPartFractionNumerator ComputationalBufferCoefficientNonExpanded;
	this->CoefficientNonExpanded.ComputePolyPartFractionNumerator
		(ComputationalBufferCoefficientNonExpanded,theDimension);
	//ComputationalBufferCoefficientNonExpanded.ComputeDebugString();
	ComputationalBufferCoefficientNonExpanded.ConvertToIntegerPoly(ComputationalBufferCoefficient, theDimension);
	//ComputationalBufferCoefficient.ComputeDebugString();
	this->Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
	//ComputationalBufferCoefficient.AssignPolynomialLight(this->Coefficient);
	//ComputationalBufferCoefficient.ComputeDebugString();
//	this->Coefficient.ComputeDebugString();
}

void partFraction::ComputeOneCheckSum
	(	partFractions& owner,Rational &output, int theDimension,
		GlobalVariables& theGlobalVariables)
{ if (this->flagAnErrorHasOccurredTimeToPanic)
	{ this->Coefficient.ComputeDebugString();
		this->ComputeDebugString(owner,theGlobalVariables);
	}
	static IntegerPoly ComputationalBufferCoefficient;
	ComputationalBufferCoefficient.AssignPolynomialLight(this->Coefficient);
	ComputationalBufferCoefficient.Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot,output);
	std::string tempS;
	if (this->flagAnErrorHasOccurredTimeToPanic && Rational::flagAnErrorHasOccurredTimeToPanic)
	{ output.ElementToString(tempS);
	}
	//output.ElementToString(tempS);
	Rational tempRat;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]]
			.ComputeOneCheckSum(tempRat,owner.RootsToIndices.TheObjects
				[this->IndicesNonZeroMults.TheObjects[i]],theDimension);
		//tempRat.ElementToString(tempS);
		output.MultiplyBy(tempRat);
		//output.ElementToString(tempS);
	}
}

#ifdef WIN32
#pragma warning(disable:4018)//grrrrr
#endif
int partFraction::ElementToStringBasisChange
	(	partFractions& owner,MatrixIntTightMemoryFit& VarChange,
		bool UsingVarChange, std::string& output,
		bool LatexFormat, bool includeVPsummand,
		bool includeNumerator, GlobalVariables& theGlobalVariables)
{ std::stringstream out, out2;
	std::string tempS, stringPoly;
	if (this->Coefficient.size==0)
	{output.clear(); return 0;}
	int NumLinesUsed=0;
//	int OldCutOff=0;
	int theDimension= owner.RootsToIndices.TheObjects[0].dimension;
	IntegerPoly& ComputationalBufferCoefficient=
		theGlobalVariables.IPElementToStringBasisChange;
	PolyPartFractionNumerator& ComputationalBufferCoefficientNonExpanded=
		theGlobalVariables.PPFNElementToStringBasisChange;
	ComputationalBufferCoefficient.AssignPolynomialLight(this->Coefficient);
	this->CoefficientNonExpanded.ComputePolyPartFractionNumerator
		(ComputationalBufferCoefficientNonExpanded,theDimension);
	if (!UsingVarChange)
	{	if (includeNumerator)
			{	if (this->UncoveringBrackets)
					NumLinesUsed+=ComputationalBufferCoefficient
						.StringPrintOutAppend (stringPoly,PolyFormatLocal);
				else
					NumLinesUsed+=ComputationalBufferCoefficientNonExpanded
						.StringPrintOutAppend (stringPoly,PolyFormatLocal);
			}
		else
			stringPoly="(...)";
	}
	else
	{ static PolynomialRationalCoeff tempP,tempP2;
		static Polynomials<Rational> tempSub;
		tempP.Nullify((short)VarChange.NumRows);
		tempSub.MakeExponentSubstitution(VarChange);
		tempP2.AssignIntegerPoly(ComputationalBufferCoefficient);
		tempP2.Substitution(tempSub,tempP,(short)VarChange.NumRows);
		NumLinesUsed+=tempP.StringPrintOutAppend(stringPoly, PolyFormatLocal);
	}
	if (stringPoly=="1"){stringPoly="";}
	if (stringPoly=="-1"){stringPoly="-";}
	if ((this->Coefficient.size>1 && this->UncoveringBrackets) ||
			(this->CoefficientNonExpanded.size>1 && !this->UncoveringBrackets))
	{out2<<"(";}
	out2<<stringPoly;
	if ((this->Coefficient.size>1 && this->UncoveringBrackets) ||
			(this->CoefficientNonExpanded.size>1 && !this->UncoveringBrackets))
	{out2<<")";}
	stringPoly= out2.str();
//	this->intRootToString(out,this->RootShift,false);
	out<<" ";
	for (int i =0;i<this->size;i++)
	{ this->TheObjects[i].ElementToStringBasisChange
			(owner,VarChange,UsingVarChange,tempS,LatexFormat,i,theDimension);
		out<<tempS;
	}
	if (includeVPsummand && LatexFormat)
	{	std::string tempS2;
		QuasiPolynomial tempQP;
		tempQP.Nullify((short)theDimension);
		this->partFractionToPartitionFunctionSplit
			(	owner,tempQP,false,false, theGlobalVariables, theDimension);
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

/*void partFraction::MultiplyMinusRootShiftBy(int *theRoot, int Multiplicity)
{ //for (int i=0;i<root::AmbientDimension;i++)
	//{ this->RootShift[i]+=theRoot[i]*Multiplicity;
	//}
	static Monomial<Integer> tempM;
	tempM.MakeConstantMonomial(root::AmbientDimension,IOne);
	for (int i=0;i<root::AmbientDimension;i++)
	{ tempM.degrees[i]=(short)(theRoot[i]*Multiplicity);
	}
	this->Coefficient.MultiplyByMonomial(tempM);
}*/

/*void partFraction::intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent)
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
}*/

bool partFraction::rootIsInFractionCone(partFractions& owner, root&r, GlobalVariables& theGlobalVariables)
{ //assert(this->IndicesNonZeroMults.size==root::AmbientDimension);
	if (!partFractions::flagUsingIndicatorRoot|| owner.IndicatorRoot.IsEqualToZero())
		return true;
	if (this->RelevanceIsComputed)
		return !this->IsIrrelevant;
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString(owner,theGlobalVariables);
		if (this->DebugString=="a^{3}b^{2} \\frac{1}{(1-a^{2}b^{3})^5} \\frac{1}{(1-ab)}")
		{ Stop();
		}
	}
	static MatrixLargeRational tempMat, MatOneCol;
	static Selection NonPivotPoints;
	Cone tempCone; roots tempRoots;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ int tempI= this->IndicesNonZeroMults.TheObjects[i];
		tempRoots.AddIntRoot(owner.RootsToIndices.TheObjects[tempI]);
	}
	tempCone.ComputeFromDirections(tempRoots, theGlobalVariables,r.size);
	tempCone.ComputeDebugString();
	this->IsIrrelevant=! tempCone.IsInCone(r);
	this->RelevanceIsComputed=true;
	return !this->IsIrrelevant;
}

void partFraction::PrepareFraction( int indexA, int indexB,  int AminusNBindex,
																		bool indexAisNullified, partFraction &output,
																		IntegerPoly& AminusNbetaPoly )
{	output.AssignNoIndicesNonZeroMults(*this);
	static IntegerPoly ComputationalBufferCoefficient;
	ComputationalBufferCoefficient.AssignPolynomialLight(output.Coefficient);
	int powerDropA = this->TheObjects[indexA].Multiplicities.TheObjects[0];
	int powerDropB = this->TheObjects[indexB].Multiplicities.TheObjects[0];
	if (indexAisNullified)
		powerDropB=0;
	else
		powerDropA=0;
	for (int i=0;i<powerDropB;i++)
		ComputationalBufferCoefficient.MultiplyBy(AminusNbetaPoly);
	output.DecreasePowerOneFrac(indexA,powerDropA);
	output.DecreasePowerOneFrac(indexB,powerDropB);
	output.DecreasePowerOneFrac(AminusNBindex,-powerDropA-powerDropB);
	output.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
}

int partFraction::GetNumProportionalVectorsClassicalRootSystems(partFractions& owner)
{ int result=0;
	for (int i=0;i<owner.RootsToIndices.IndicesRedundantShortRoots.CardinalitySelection;i++)
	{ int tempI = owner.RootsToIndices.IndicesRedundantShortRoots.elements[i];
		int tempI2= owner.RootsToIndices.getIndexDoubleOfARoot
			(owner.RootsToIndices.TheObjects[tempI]);
		if (tempI2!=-1)
			if (this->TheObjects[tempI2].Multiplicities.size>0)
				result++;
	}
	return result;
}

int partFraction::getSmallestNonZeroIndexGreaterThanOrEqualTo(partFractions& owner,int minIndex)
{ int result=owner.RootsToIndices.size;
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
	{ int tempI=this->IndicesNonZeroMults.TheObjects[i];
		if (this->TheObjects[tempI].Multiplicities.size>0)
			if (tempI>=minIndex && tempI<result)
				result=tempI;
	}
	return result;
}

int partFraction::ComputeGainingMultiplicityIndexInLinearRelation
				(partFractions& owner,	MatrixLargeRational& theLinearRelation)
{	int DesireToSelectAsGainingMultiplicity=-1;
	int result=-1;
	for( int i=0;i<theLinearRelation.NumRows;i++)
	{ if(! theLinearRelation.elements[i][0].IsEqualToZero())
		{	int currentIndex= this->IndicesNonZeroMults.TheObjects[i];
			int candidateDesire;
			if (!owner.flagUsingOrlikSolomonBasis)
				candidateDesire=this->TheObjects[currentIndex].GetTotalMultiplicity();
			else
				candidateDesire= currentIndex;
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

bool partFraction::CheckForOrlikSolomonAdmissibility
	(ListBasicObjects<int> &theSelectedIndices)
{ return true;
	/*if (!this->flagUsingPrecomputedOrlikSolomonBases)
		return true;
	else
		return true;*/
}

bool partFraction::DecomposeFromLinRelation
	(	MatrixLargeRational& theLinearRelation, partFractions& Accum,
		GlobalVariables& theGlobalVariables)
{//	theLinearRelation.ComputeDebugString();
	//theLinearRelation.ComputeDebugString();
	int GainingMultiplicityIndexInLinRelation=-1;
	int GainingMultiplicityIndex=-1;
	int ElongationGainingMultiplicityIndex=-1;
	static ListBasicObjects<int> ParticipatingIndices;
	static ListBasicObjects<int> theGreatestElongations;
	static ListBasicObjects<int> theCoefficients;
	Rational oldCheckSum;
	ParticipatingIndices.size=0;
	theCoefficients.size=0;
	theGreatestElongations.size=0;
	GainingMultiplicityIndexInLinRelation =
		this->ComputeGainingMultiplicityIndexInLinearRelation
			(Accum,theLinearRelation);
	GainingMultiplicityIndex= this->IndicesNonZeroMults
																.TheObjects[GainingMultiplicityIndexInLinRelation];
	int tempI=this->TheObjects[GainingMultiplicityIndex].GetLargestElongation();
	theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].MultiplyByInt(tempI);
	//theLinearRelation.ComputeDebugString();
	theLinearRelation.ScaleToIntegralForMinRationalHeightNoSignChange();
	//theLinearRelation.ComputeDebugString();
	ElongationGainingMultiplicityIndex =
		theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].NumShort;
	if (ElongationGainingMultiplicityIndex<0)
		ElongationGainingMultiplicityIndex=-ElongationGainingMultiplicityIndex;
	else
		theLinearRelation.MultiplyByInt(-1);
	//theLinearRelation.ComputeDebugString();
	for (int i=0;i<theLinearRelation.NumRows;i++)
	{ if (i!=GainingMultiplicityIndexInLinRelation && theLinearRelation.elements[i][0].NumShort!=0)
		{	int tempI=this->IndicesNonZeroMults.TheObjects[i];
			ParticipatingIndices.AddObjectOnTop(tempI);
			theGreatestElongations.AddObjectOnTop(this->TheObjects[tempI].GetLargestElongation());
			theCoefficients.AddObjectOnTop(theLinearRelation.elements[i][0].NumShort);
		}
	}
	if (!Accum.flagUsingOrlikSolomonBasis)
		this->LastDistinguishedIndex=GainingMultiplicityIndex;
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		this->ComputeDebugString(Accum,theGlobalVariables);
	//if (this->MakingConsistencyCheck)
	//{ this->ComputeOneCheckSum(this->CheckSum2);
	//}
	//if (!this->CheckForOrlikSolomonAdmissibility(ParticipatingIndices))
	//	return false;
	this->ApplyGeneralizedSzenesVergneFormula
		(	ParticipatingIndices,theGreatestElongations,theCoefficients,GainingMultiplicityIndex,
			ElongationGainingMultiplicityIndex,Accum, theGlobalVariables);
	//if (this->MakingConsistencyCheck)
	//{ assert(this->CheckSum2.IsEqualTo(this->CheckSum));
	//}
	/*if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ Rational tempRat2, tempRat;
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

void partFraction::AttemptReduction(partFractions& owner, int myIndex, GlobalVariables& theGlobalVariables)
{ bool hasImprovement=true;
	bool improvedAtLeastOnce=false;
	if (this->flagAnErrorHasOccurredTimeToPanic)
		this->ComputeDebugString(owner, theGlobalVariables);
	partFraction tempFrac;
	IntegerPoly numerator;
	IntegerPoly tempP1, tempP2, tempP3;
	numerator.AssignPolynomialLight(this->Coefficient);
	tempFrac.Assign(*this);
	while (hasImprovement)
	{ hasImprovement=false;
		numerator.ComputeDebugString();
		for (int i=0;i<tempFrac.IndicesNonZeroMults.size;i++)
			for (int j=0;j<tempFrac.TheObjects[IndicesNonZeroMults.TheObjects[i]]
												.Multiplicities.size;j++)
			{	tempFrac.TheObjects[IndicesNonZeroMults.TheObjects[i]].GetPolyDenominator
					(tempP1,j,owner.RootsToIndices.TheObjects[IndicesNonZeroMults.TheObjects[i]]);
				tempP1.ComputeDebugString();
				numerator.DivideBy(tempP1,tempP2, tempP3);
				tempP2.ComputeDebugString();
				tempP3.ComputeDebugString();
				if (tempP3.IsEqualToZero())
				{	tempFrac.DecreasePowerOneFrac(IndicesNonZeroMults.TheObjects[i],1);
					numerator.Assign(tempP2);
					hasImprovement=true;
					improvedAtLeastOnce=true;
				}
			}
	}
	if (improvedAtLeastOnce)
	{	if (this->flagAnErrorHasOccurredTimeToPanic)
		{//	owner.CompareCheckSums();
			owner.ComputeDebugString(theGlobalVariables);
		}
		owner.PopIndexHashAndAccount(myIndex,theGlobalVariables);
		tempFrac.Coefficient.AssignPolynomial(numerator);
		owner.AddAndReduce(tempFrac,theGlobalVariables);
		if (this->flagAnErrorHasOccurredTimeToPanic)
		{	//owner.CompareCheckSums();
			owner.ComputeDebugString(theGlobalVariables);
		}
	}
}

void partFraction::GetNElongationPolyWithMonomialContribution
			(	partFractions& owner,
				ListBasicObjects<int>& theSelectedIndices,
				ListBasicObjects<int>& theCoefficients,
				ListBasicObjects<int>& theGreatestElongations,
				int theIndex,// int theIndexBaseElongation, int lengthGeometricSeries,
				IntegerPoly& output, int theDimension)
{ static Monomial<Integer> tempM;
	static IntegerPoly tempP;
	tempM.init((short)theDimension);
	tempM.Coefficient.value=1;
	for (int i=0;i<theIndex;i++)
	{ int tempI= theSelectedIndices.TheObjects[i];
		for (int j=0;j<theDimension;j++)
		{ tempM.degrees[j]+=(short)(theCoefficients.TheObjects[i] *theGreatestElongations.TheObjects[i]*
																owner.RootsToIndices.TheObjects[tempI].elements[j]);
		}
	}
	this->GetNElongationPoly
		(	owner,theSelectedIndices.TheObjects[theIndex],
			theGreatestElongations.TheObjects[theIndex],
			theCoefficients.TheObjects[theIndex],
			output,theDimension);
	output.MultiplyByMonomial(tempM);
}

void partFraction::ApplyGeneralizedSzenesVergneFormula
			(	ListBasicObjects<int> &theSelectedIndices,
				ListBasicObjects<int> &theGreatestElongations,
				ListBasicObjects<int> &theCoefficients, int GainingMultiplicityIndex,
				int ElongationGainingMultiplicityIndex, partFractions &Accum,
				GlobalVariables& theGlobalVariables)
{ static partFraction tempFrac; tempFrac.RelevanceIsComputed=false;
	static IntegerPoly tempP;
  static PolyPartFractionNumerator tempNum;
  static IntegerPoly ComputationalBufferCoefficient;
  static PolyPartFractionNumerator ComputationalBufferCoefficientNonExpanded;
  //this->lastApplicationOfSVformulaNumNewGenerators=0;
  //this->lastApplicationOfSVformulaNumNewMonomials=0;
  Rational StartCheckSum, theDiff;
  if (this->flagAnErrorHasOccurredTimeToPanic)
	{ Accum.ComputeOneCheckSum(StartCheckSum,theGlobalVariables);
		this->ComputeOneCheckSum(Accum,theDiff,Accum.AmbientDimension,theGlobalVariables);
	}
  SelectionWithDifferentMaxMultiplicities TheBigBadIndexingSet;
  TheBigBadIndexingSet.initIncomplete(theSelectedIndices.size);
  static int TotalMultiplicity;
  TotalMultiplicity=0;
  for (int i=0;i<theSelectedIndices.size;i++)
  { int tempI= this->TheObjects[theSelectedIndices.TheObjects[i]]
				.GetMultiplicityLargestElongation()-1;
		TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i]=tempI;
		TotalMultiplicity+=tempI;
  }
  for (int i=0;i<theSelectedIndices.size;i++)
  {	TheBigBadIndexingSet.clearNoMaxMultiplicitiesChange();
		int oldMaxMultiplicity= TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i];
		TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i]=0;
		int NumSubsets=TheBigBadIndexingSet.getTotalNumSubsets();
		for (int j=0;j<NumSubsets;j++)
		{	tempFrac.Assign(*this);
			tempFrac.RelevanceIsComputed=false;
			if (this->UncoveringBrackets)
				ComputationalBufferCoefficient.AssignPolynomialLight(tempFrac.Coefficient);
			else
				tempFrac.CoefficientNonExpanded.ComputePolyPartFractionNumerator
					(ComputationalBufferCoefficientNonExpanded,Accum.AmbientDimension);
			int tempN= TheBigBadIndexingSet.TotalMultiplicity()+oldMaxMultiplicity;
			if (this->flagAnErrorHasOccurredTimeToPanic)
				TheBigBadIndexingSet.ComputeDebugString();
			for (int k=0;k<theSelectedIndices.size;k++)
			{	int multiplicityChange;
				if (k!=i)
					multiplicityChange= TheBigBadIndexingSet.Multiplicities.TheObjects[k];
				else
					multiplicityChange= oldMaxMultiplicity+1;
				tempFrac.TheObjects[theSelectedIndices.TheObjects[k]].AddMultiplicity
					(-multiplicityChange ,theGreatestElongations.TheObjects[k]);
				if (this->UncoveringBrackets)
				{ this->GetNElongationPolyWithMonomialContribution
						(	Accum,theSelectedIndices,theCoefficients,theGreatestElongations,
							k, tempP,Accum.AmbientDimension);
					if (this->flagAnErrorHasOccurredTimeToPanic)
						tempP.ComputeDebugString();
					tempP.RaiseToPower(multiplicityChange);
					if (this->flagAnErrorHasOccurredTimeToPanic)
						tempP.ComputeDebugString();
					ComputationalBufferCoefficient.MultiplyBy(tempP);
					static Integer tempInt;
					int tempI;
					if (k==i) tempI = oldMaxMultiplicity; else tempI=multiplicityChange;
					tempInt.value=MathRoutines::NChooseK(tempN,tempI);
					ComputationalBufferCoefficient.TimesConstant(tempInt);
					tempN-=tempI;
					if (this->flagAnErrorHasOccurredTimeToPanic)
						tempFrac.ComputeDebugString(Accum,theGlobalVariables);
				}else
				{}
			}
			if (this->UncoveringBrackets)
				tempFrac.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
			else
				{}
			if (this->flagAnErrorHasOccurredTimeToPanic)
				tempFrac.ComputeDebugString(Accum,theGlobalVariables);
			tempFrac.TheObjects[GainingMultiplicityIndex].AddMultiplicity
				(	TheBigBadIndexingSet.TotalMultiplicity()+oldMaxMultiplicity+1,
					ElongationGainingMultiplicityIndex);
			if (this->flagAnErrorHasOccurredTimeToPanic)
				tempFrac.ComputeDebugString(Accum,theGlobalVariables);
			tempFrac.ComputeIndicesNonZeroMults();
			Accum.AddAndReduce(tempFrac,theGlobalVariables);
			TheBigBadIndexingSet.IncrementSubset();
		}
		TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i]= oldMaxMultiplicity;
  }
  if (this->flagAnErrorHasOccurredTimeToPanic)
	{ Rational tempRat;
		Accum.ComputeOneCheckSum(tempRat,theGlobalVariables);
		tempRat.Subtract(theDiff);
		assert(tempRat.IsEqualTo(StartCheckSum));
	}
}

void partFraction::ApplySzenesVergneFormula
			(	ListBasicObjects<int> &theSelectedIndices, ListBasicObjects<int>& theElongations,
				int GainingMultiplicityIndex,int ElongationGainingMultiplicityIndex,
				partFractions& Accum, GlobalVariables& theGlobalVariables)
{	static partFraction tempFrac; tempFrac.RelevanceIsComputed=false;
	static IntegerPoly tempP;
  static PolyPartFractionNumerator tempNum;
  static IntegerPoly ComputationalBufferCoefficient;
  static PolyPartFractionNumerator ComputationalBufferCoefficientNonExpanded;
  //this->lastApplicationOfSVformulaNumNewGenerators=0;
  //this->lastApplicationOfSVformulaNumNewMonomials=0;
  Rational StartCheckSum, theDiff;
  if (this->flagAnErrorHasOccurredTimeToPanic)
	{ Accum.ComputeOneCheckSum(StartCheckSum,theGlobalVariables);
		this->ComputeOneCheckSum(Accum,theDiff,Accum.AmbientDimension,theGlobalVariables);
	}
	for(int i=0;i<theSelectedIndices.size;i++)
	{	tempFrac.Assign(*this);
		tempFrac.RelevanceIsComputed=false;
		ComputationalBufferCoefficient.AssignPolynomialLight(tempFrac.Coefficient);
		tempFrac.CoefficientNonExpanded.ComputePolyPartFractionNumerator
			(ComputationalBufferCoefficientNonExpanded,Accum.AmbientDimension);
		tempFrac.TheObjects[GainingMultiplicityIndex].AddMultiplicity
			(1,ElongationGainingMultiplicityIndex);
		oneFracWithMultiplicitiesAndElongations& currentFrac=
			tempFrac.TheObjects[theSelectedIndices.TheObjects[i]];
		int LargestElongation= currentFrac.GetLargestElongation();
		currentFrac.AddMultiplicity
			(-1,LargestElongation);
		static Monomial<Integer> tempM;
		if (this->UncoveringBrackets)
		{ tempM.init((short)Accum.AmbientDimension);
      tempM.Coefficient.Assign(IOne);
      for (int j=0;j<i;j++)
      {	short tempElongation=(short) this->TheObjects[theSelectedIndices.TheObjects[j]].GetLargestElongation();
        for (int k=0;k<Accum.AmbientDimension;k++)
        { tempM.degrees[k]+=(short)theElongations.TheObjects[j]*tempElongation*
                            (short)Accum.RootsToIndices.
                              TheObjects[theSelectedIndices.TheObjects[j]].elements[k];
        }
      }
      ParallelComputing::SafePoint();
      ComputationalBufferCoefficient.MultiplyByMonomial(tempM);
      this->GetNElongationPoly
	      (	Accum,theSelectedIndices.TheObjects[i],LargestElongation,
					theElongations.TheObjects[i],tempP,Accum.AmbientDimension);
      ComputationalBufferCoefficient.MultiplyBy(tempP);
      tempFrac.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
		}
		else
		{ static ::MonomialInCommutativeAlgebra<Integer,
																						GeneratorsPartialFractionAlgebra,
																						GeneratorPFAlgebraRecord> tempM;
//			if (this->flagAnErrorHasOccurredTimeToPanic)
//			{	this->ComputeDebugString();
//			}
			tempM.Coefficient.Assign(IOne);
			tempM.ClearTheObjects();
			for (int j=0;j<i;j++)
			{	short tempElongation=(short) this->TheObjects[theSelectedIndices.TheObjects[j]].GetLargestElongation();
				intRoot tempRoot;
				tempRoot= Accum.RootsToIndices.TheObjects[theSelectedIndices.TheObjects[j]];
				tempRoot.MultiplyByInteger(theElongations.TheObjects[j]*tempElongation);
				static ::MonomialInCommutativeAlgebra
									<	Integer,
										GeneratorsPartialFractionAlgebra,
										GeneratorPFAlgebraRecord> tempM2;
				GeneratorsPartialFractionAlgebra::GetMonomialFromExponentAndElongation(tempRoot,0,tempM2);
				tempM.MultiplyBy(tempM2);
			}
			ParallelComputing::SafePoint();
			if (theElongations.TheObjects[i]!=1)
			{ static intRoot tempRoot;
				tempRoot=Accum.RootsToIndices.TheObjects[theSelectedIndices.TheObjects[i]];
				tempRoot.MultiplyByInteger( LargestElongation);
				static ::MonomialInCommutativeAlgebra
					<Integer, GeneratorsPartialFractionAlgebra,GeneratorPFAlgebraRecord> tempM2;
				GeneratorsPartialFractionAlgebra::GetMonomialFromExponentAndElongation
					(tempRoot,theElongations.TheObjects[i],tempM2);
				//tempM2.ComputeDebugString(PolyFormatLocal);
				//tempM.ComputeDebugString(PolyFormatLocal);
				tempM.MultiplyBy(tempM2);
				//tempM.ComputeDebugString(PolyFormatLocal);
			}
			//tempFrac.CoefficientNonExpanded.ComputeDebugString();
			ComputationalBufferCoefficientNonExpanded.MultiplyByMonomial(tempM);
			tempFrac.CoefficientNonExpanded.AssignPolyPartFractionNumerator
				(ComputationalBufferCoefficientNonExpanded);
			//tempFrac.CoefficientNonExpanded.ComputeDebugString();
		}
		tempFrac.ComputeIndicesNonZeroMults();
    Accum.AddAndReduce(tempFrac,theGlobalVariables);
//    this->lastApplicationOfSVformulaNumNewMonomials+=tempFrac.GetNumMonomialsInNumerator();
//    this->lastApplicationOfSVformulaNumNewGenerators+=tempFrac.CoefficientNonExpanded.NumGeneratorsUsed();
	}
	//if (this->flagAnErrorHasOccurredTimeToPanic)
	//{ assert(this->CheckSum.IsEqualTo(this->CheckSum2));
	//}
  if (this->flagAnErrorHasOccurredTimeToPanic)
	{ Rational tempRat;
		Accum.ComputeOneCheckSum(tempRat,theGlobalVariables);
		tempRat.Subtract(theDiff);
		assert(tempRat.IsEqualTo(StartCheckSum));
	}
	//this->Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
}

//void partFraction::GetNElongationPoly

void partFraction::decomposeAMinusNB(int indexA, int indexB, int n,
																	   int indexAminusNB, partFractions& Accum,
																	   GlobalVariables& theGlobalVariables)
{	static partFraction tempFrac; tempFrac.RelevanceIsComputed=false;
	static IntegerPoly AminusNbetaPoly;
	static IntegerPoly ComputationalBufferCoefficient;
	this->GetAlphaMinusNBetaPoly(Accum,indexA,indexB,n,AminusNbetaPoly, Accum.AmbientDimension);
	int powerA= this->TheObjects[indexA].Multiplicities.TheObjects[0];
	int powerB= this->TheObjects[indexB].Multiplicities.TheObjects[0];
	this->PrepareFraction(indexA,indexB,indexAminusNB,true,tempFrac,
												AminusNbetaPoly);
	ComputationalBufferCoefficient.AssignPolynomialLight(tempFrac.Coefficient);
	for (int i=powerB;i>=1;i--)
	{	Integer tempInt(MathRoutines::NChooseK(powerA+powerB-i-1,powerA-1));
		ComputationalBufferCoefficient.TimesConstant(tempInt);
		tempFrac.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
		Accum.AddAlreadyReduced(tempFrac,theGlobalVariables);
		ComputationalBufferCoefficient.DivideByConstant(tempInt);
//		Accum.ComputeDebugString();
		if (i>1)
		{	tempFrac.TheObjects[indexAminusNB].AddMultiplicity(1,1);
			tempFrac.TheObjects[indexB].AddMultiplicity(-1,1);
			ComputationalBufferCoefficient.MultiplyBy(AminusNbetaPoly);
	//		tempFrac.ComputeDebugString();
		}
	}
	this->PrepareFraction(indexA,indexB,indexAminusNB,false,tempFrac,AminusNbetaPoly);
//	tempFrac.ComputeDebugString();
	ComputationalBufferCoefficient.AssignPolynomialLight(tempFrac.Coefficient);
	for (int i=powerA;i>=1;i--)
	{	Integer tempInt(MathRoutines::NChooseK(powerA+powerB-i-1,powerB-1));
		ComputationalBufferCoefficient.TimesConstant(tempInt);
		tempFrac.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
		Accum.AddAlreadyReduced(tempFrac,theGlobalVariables);
		ComputationalBufferCoefficient.DivideByConstant(tempInt);
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

void partFraction::GetAlphaMinusNBetaPoly
	(partFractions& owner, int indexA, int indexB, int n, IntegerPoly& output, int theDimension)
{	output.Nullify((short)theDimension);
	Monomial<Integer> tempM;
	tempM.init((short)theDimension);
	tempM.Coefficient.Assign(IMOne);
	for (int i=0;i<n;i++)
	{ for (int j=0;j<theDimension;j++)
			tempM.degrees[j]= (short)(owner.RootsToIndices.TheObjects[indexA].elements[j]
												- (i+1)*owner.RootsToIndices.TheObjects[indexB].elements[j]);
		output.AddMonomial(tempM);
	}
}

void partFraction::GetNElongationPoly
			(	partFractions& owner,int index,int baseElongation,
				int LengthOfGeometricSeries, IntegerPoly& output,int theDimension)
{	output.Nullify((short)theDimension);
	Monomial<Integer> tempM;
	tempM.init((short)theDimension);
	if (LengthOfGeometricSeries>0)
	{	tempM.Coefficient.Assign(IOne);
		for (int i=0;i<LengthOfGeometricSeries;i++)
		{ for (int j=0;j<theDimension;j++)
			{ tempM.degrees[j]=(short)
					(baseElongation*i*owner.RootsToIndices.TheObjects[index].elements[j]);
			}
			output.AddMonomial(tempM);
		}
	}
	else
	{ assert(LengthOfGeometricSeries<0);
		tempM.Coefficient.Assign(IMOne);
		for (int i=-1;i>=LengthOfGeometricSeries;i--)
		{ for (int j=0;j<theDimension;j++)
			{ tempM.degrees[j]=short(baseElongation*i*owner.RootsToIndices.TheObjects[index].elements[j]);
			}
			output.AddMonomial(tempM);
		}
	}
	//output.ComputeDebugString();
}

void partFraction::GetNElongationPoly
	(	partFractions& owner, int index,int baseElongation,int LengthOfGeometricSeries,
		PolyPartFractionNumerator& output, int theDimension)
{	::MonomialInCommutativeAlgebra
		<Integer,::GeneratorsPartialFractionAlgebra,GeneratorPFAlgebraRecord> tempM;
	assert (LengthOfGeometricSeries!=0);
	intRoot tempRoot;
	tempRoot= owner.RootsToIndices.TheObjects[index];
	tempRoot.MultiplyByInteger(baseElongation);
	::GeneratorsPartialFractionAlgebra::GetMonomialFromExponentAndElongation
		(tempRoot,LengthOfGeometricSeries, tempM);
	output.Nullify((short)theDimension);
	output.AddMonomial(tempM);
}

/*void partFraction::partFractionToPartitionFunctionStoreAnswer
				(QuasiPolynomial& output, bool RecordSplitPowerSeriesCoefficient, bool StoreToFile)
{	//if (!this->PowerSeriesCoefficientIsComputed)
	{ this->partFractionToPartitionFunctionSplit
			(output,true,RecordSplitPowerSeriesCoefficient, StoreToFile);
//		this->PowerSeriesCoefficient.CopyFromPoly(output);
		this->PowerSeriesCoefficientIsComputed=true;
		IndicatorWindowGlobalVariables.NumComputedContributions++;
		FeedDataToIndicatorWindow(IndicatorWindowGlobalVariables);
	}
	else
	{ output.CopyFromPoly(this->PowerSeriesCoefficient);
	}
}*/

void partFraction::partFractionToPartitionFunctionSplit
			(partFractions& owner,QuasiPolynomial& output, bool RecordNumMonomials,
			 //bool RecordSplitPowerSeriesCoefficient,
			 bool StoreToFile, GlobalVariables& theGlobalVariables, int theDimension)
{	static PolynomialRationalCoeff shiftedPoly, tempP;
	static roots normals;
	static partFractionPolynomials tempSplitPowerSeriesCoefficient;
	static partFractionPolynomials* SplitPowerSeriesCoefficient;
	if (partFraction::MakingConsistencyCheck)
	{	partFraction::CheckSum.MakeZero();
	}
	if (this->FileStoragePosition!=-1)
	{ partFraction::TheBigDump.seekg(this->FileStoragePosition);
		output.ReadFromFile(partFraction::TheBigDump,(short)theDimension);
		if (RecordNumMonomials && ! this->AlreadyAccountedForInGUIDisplay)
		{ this->AlreadyAccountedForInGUIDisplay=true;
			partFractions::NumProcessedForVPFMonomialsTotal+=this->Coefficient.size;
			//IndicatorWindowGlobalVariables.NumProcessedMonomials+=this->Coefficient.size;
		}
//		output.ComputeDebugString();
		return;
	}
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		this->ComputeDebugString(owner,theGlobalVariables);
	/*if(RecordSplitPowerSeriesCoefficient)
	{ SplitPowerSeriesCoefficient= &this->SplitPowerSeriesCoefficients;
	}
	else*/
	{ SplitPowerSeriesCoefficient= &tempSplitPowerSeriesCoefficient;
	}
	SplitPowerSeriesCoefficient->initLatticeIndicatorsFromPartFraction(owner,*this,theGlobalVariables,theDimension);
//	SplitPowerSeriesCoefficient->InvertedMatrixRoots.ComputeDebugString();
	normals.AssignMatrixRows(SplitPowerSeriesCoefficient->theNormals);
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ this->ComputeDebugString(owner, theGlobalVariables);
		normals.ComputeDebugString();
		Stop();
	}
	for (int i=0;i<this->Coefficient.size;i++)
	{	this->ComputePolyCorrespondingToOneMonomial
			(shiftedPoly,i,normals,SplitPowerSeriesCoefficient,theDimension);

		if (RecordNumMonomials)
		{ std::stringstream out4,out3;
			out4 <<"Current fraction: "<<i+1<<" out of "<<this->Coefficient.size
					<<" processed";
			partFractions::NumProcessedForVPFMonomialsTotal++;
			out3	<<" Processed " << partFractions::NumProcessedForVPFMonomialsTotal <<" out of "
						<<partFractions::NumMonomialsInNumeratorsRelevantFractions << " relevant monomials";
			::IndicatorWindowGlobalVariables.ProgressReportString4= out4.str();
			::IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
			if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
				theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
	}
	SplitPowerSeriesCoefficient->ComputeQuasiPolynomial(output, RecordNumMonomials,theDimension,theGlobalVariables);
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ Stop();
	}
	if (partFraction::MakingConsistencyCheck)
	{ Rational tempLRat;
		output.Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
		assert(tempLRat.DenShort==1);
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
			(	PolynomialRationalCoeff &output, int index, roots& normals,
				partFractionPolynomials* SplitPowerSeriesCoefficient, int theDimension)
{ root shiftRational; shiftRational.SetSizeExpandOnTopLight(theDimension);
	static PolynomialRationalCoeff tempP;
	for (int j=0;j<theDimension;j++)
	{ shiftRational.TheObjects[j].AssignInteger
			(this->Coefficient.TheObjects[index].degrees[j]);
	}
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{	shiftRational.ComputeDebugString();
		this->Coefficient.ComputeDebugString();
		normals.ComputeDebugString();
	}
	Rational tempRat;
	tempRat.AssignInteger(this->Coefficient.TheObjects[index].Coefficient.value);
	output.MakeNVarConst((short)theDimension, tempRat);
	for (int i=0;i<theDimension;i++)
	{ static root beta; beta.SetSizeExpandOnTopLight(theDimension);
		static Rational tempRat,tempRat2;
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ normals.ComputeDebugString();
		}
		this->MakePolynomialFromOneNormal
			(	normals.TheObjects[i],shiftRational,
				this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]].Multiplicities.TheObjects[0],
				tempP);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ normals.ComputeDebugString();
		}
		output.MultiplyBy(tempP);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{	tempP.ComputeDebugString();
			output.ComputeDebugString();
		}
	}
	//	output.ComputeDebugString();
	SplitPowerSeriesCoefficient->AddPolynomialLargeRational(shiftRational,output);
	this->AlreadyAccountedForInGUIDisplay=true;
}

/*int partFraction::Elongate(int indexElongatedFraction, int theElongation)
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
}*/

void partFraction::MakePolynomialFromOneNormal
			(	root& normal, root& shiftRational, int theMult,	PolynomialRationalCoeff& output)
{	Rational tempRat,tempRat2;
	static PolynomialRationalCoeff tempP;
	int theDimension= normal.size;
	output.MakeNVarConst((short)theDimension,LROne);
	if (theMult==1) return;
	root::RootScalarEuclideanRoot(normal,shiftRational,tempRat);
	for (int j=0;j<theMult-1;j++)
	{ tempP.MakeLinPolyFromRoot(normal);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ tempP.ComputeDebugString();
		}
		tempRat2.AssignNumeratorAndDenominator(-1,j+1);
		tempRat2.MultiplyBy(tempRat);
		tempRat2.Add(LROne);
		Rational tempRat3;
		tempRat3.AssignNumeratorAndDenominator(1,j+1);
		tempP.TimesConstant(tempRat3);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ tempP.ComputeDebugString(); }
		tempP.AddConstant(tempRat2);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ tempP.ComputeDebugString(); }
		output.MultiplyBy(tempP);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ output.ComputeDebugString(); }
	}
}

void partFraction::ComputeNormals(partFractions& owner, roots& output, int theDimension, GlobalVariables& theGlobalVariables)
{	static roots dens;
	root tempRoot; tempRoot.SetSizeExpandOnTopLight(theDimension);
	dens.size=0;
	output.size=0;
	for (int i=0;i<theDimension;i++)
	{	tempRoot.AssignIntRoot
			(owner.RootsToIndices.TheObjects[this->IndicesNonZeroMults.TheObjects[i]]);
		dens.AddRoot(tempRoot);
	}
	for (int i=0;i<theDimension;i++)
	{ static Rational tempRat, tempRat2;
		dens.ComputeNormalExcludingIndex(tempRoot, i,theGlobalVariables);
		root::RootScalarEuclideanRoot(tempRoot, dens.TheObjects[i],tempRat);
		assert(!tempRat.IsEqualToZero());
		tempRoot.DivByLargeRational(tempRat);
		output.AddRoot(tempRoot);
//		tempRoot.ComputeDebugString();
	}
}

partFraction::partFraction()
{//	this->init(partFraction::RootsToIndices.size);
	this->PowerSeriesCoefficientIsComputed=false;
	this->AlreadyAccountedForInGUIDisplay=false;
	this->FileStoragePosition=-1;
	this->LastDistinguishedIndex=-1;
	this->RelevanceIsComputed=false;
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
{	this->IndicesNonZeroMults.MakeActualSizeAtLeastExpandOnTop(numRoots);
	this->IndicesNonZeroMults.size=0;
	this->SetSizeExpandOnTopLight(numRoots);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Elongations.SetSizeExpandOnTopLight(0);
		this->TheObjects[i].Multiplicities.SetSizeExpandOnTopLight(0);
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

bool partFraction::IsEqualToZero()
{ if (this->UncoveringBrackets)
		return this->Coefficient.IsEqualToZero();
	else
		return this->CoefficientNonExpanded.size==0;
}

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
		Accum+=this->TheActualObjects[i].SizeWithoutDebugString();
	Accum+=	sizeof(this->HighestIndex)+
					sizeof(this->IndexLowestNonProcessed);
	return Accum;
}

void partFractions::AssureIndicatorRegularity(GlobalVariables& theGlobalVariables)
{	if (this->flagUsingIndicatorRoot)
	{	roots tempRoots;
		tempRoots.AssignHashedIntRoots(this->RootsToIndices);
		tempRoots.PerturbVectorToRegular
			(	this->IndicatorRoot, theGlobalVariables,this->IndicatorRoot.size,
				theGlobalVariables.FeedDataToIndicatorWindowDefault);
	}
}

void partFractions::UncoverBracketsNumerators( GlobalVariables& theGlobalVariables)
{ if (partFraction::UncoveringBrackets)
		return;
	int changeOfNumMonomials=0;
	for (int i=0;i<this->size;i++)
	{	if (this->flagMakingProgressReport)
		{	std::stringstream out;
			out <<"Uncovering brackets"<< i+1 <<" out of " << this->size;
			IndicatorWindowGlobalVariables.ProgressReportString4=out.str();
			changeOfNumMonomials-=this->TheObjects[i].Coefficient.size;
			if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
				theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
		this->AccountPartFractionInternals(-1, i,theGlobalVariables);
		this->TheObjects[i].UncoverBracketsNumerator(theGlobalVariables, this->AmbientDimension);
		this->AccountPartFractionInternals(1, i,theGlobalVariables);
		if (this->flagMakingProgressReport)
		{	changeOfNumMonomials+=this->TheObjects[i].Coefficient.size;
			std::stringstream out;
			out <<"Number actual monomials"<< changeOfNumMonomials;
			IndicatorWindowGlobalVariables.ProgressReportString5=out.str();
			if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
				theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
	}
}

bool partFractions::ShouldIgnore(GlobalVariables& theGlobalVariables)
{ bool shouldIgnore=false;
	if (this->flagUsingIndicatorRoot)
		shouldIgnore=!this->TheObjects[this->IndexLowestNonProcessed].rootIsInFractionCone
			(*this,this->IndicatorRoot,theGlobalVariables);
	if (shouldIgnore)
	{ if (this->flagDiscardingFractions)
			this->PopIndexSwapWithLastHash(this->IndexLowestNonProcessed);
		else
			this->IndexLowestNonProcessed++;
	}
	return shouldIgnore;
}

void partFractions::PrepareCheckSums(GlobalVariables& theGlobalVariables)
{//	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.InitFromIntegers(1,1,1,1,1,1,1,1,1,1,1,1);
//	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.DivByInteger(4);
//	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.MultiplyByInteger(3);
	if (!this->flagUsingCheckSum)
		return;
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.SetSizeExpandOnTopLight(12);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[0].AssignNumeratorAndDenominator(1,2);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[1].AssignNumeratorAndDenominator(2,3);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[2].AssignNumeratorAndDenominator(3,4);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[3].AssignNumeratorAndDenominator(4,5);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[4].AssignNumeratorAndDenominator(11,13);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[5].AssignNumeratorAndDenominator(13,17);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[6].AssignNumeratorAndDenominator(17,19);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[7].AssignNumeratorAndDenominator(19,23);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[8].AssignNumeratorAndDenominator(23,29);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[9].AssignNumeratorAndDenominator(31,37);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[10].AssignNumeratorAndDenominator(37,41);
	::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[11].AssignNumeratorAndDenominator(41,43);
	this->ComputeOneCheckSum(this->StartCheckSum,theGlobalVariables);
}

void partFractions::initFromOtherPartFractions(partFractions& input, GlobalVariables& theGlobalVariables)
{ this->RootsToIndices.CopyFromHash(input.RootsToIndices);
	this->IndicatorRoot.Assign(input.IndicatorRoot);
	this->IndexLowestNonProcessed=0;
	this->IndexCurrentlyProcessed=0;
	this->SetSizeExpandOnTopNoObjectInit(0);
	this->AmbientDimension= input.AmbientDimension;
}

void partFractions::CompareCheckSums(GlobalVariables& theGlobalVariables)
{	if (!this->flagUsingCheckSum)
		return;
	if (!this->flagDiscardingFractions)
	{	this->ComputeOneCheckSum(this->EndCheckSum,theGlobalVariables);
	//partFraction::MakingConsistencyCheck=true;
	/*if (partFraction::MakingConsistencyCheck)
	{	this->ComputeDebugString();
		tempRat2.ElementToString(tempS2);
		tempRat.ElementToString(tempS1);
	}*/
		if (this->flagAnErrorHasOccurredTimeToPanic)
		{ std::string tempS1, tempS2;
			this->StartCheckSum.ElementToString(tempS1);
			this->EndCheckSum.ElementToString(tempS2);
		}
		assert(this->StartCheckSum.IsEqualTo(this->EndCheckSum));
	}

}
void partFractions::PrepareIndicatorVariables()
{ this->NumberIrrelevantFractions=0;
	this->NumberRelevantReducedFractions=0;
	this->NumGeneratorsInTheNumerators=0;
	this->NumGeneratorsIrrelevantFractions=0;
	this->NumGeneratorsRelevenatFractions=0;
	this->NumMonomialsInNumeratorsIrrelevantFractions=0;
	this->NumMonomialsInNumeratorsRelevantFractions=0;
	this->NumMonomialsInTheNumerators=1;
	this->NumTotalReduced=0;
	this->NumRelevantNonReducedFractions=0;
	this->NumProcessedForVPFMonomialsTotal=0;
	this->NumProcessedForVPFfractions=0;
	this->NumRunsReduceMonomialByMonomial=0;
	::IndicatorWindowGlobalVariables.Nullify();
}

bool partFractions::split(GlobalVariables& theGlobalVariables)
{ //partFraction::flagAnErrorHasOccurredTimeToPanic=true;
	//this->flagAnErrorHasOccurredTimeToPanic=true;
	partFraction& tempFrac= theGlobalVariables.fracSplit1;
	this->IndexLowestNonProcessed=0;
	partFraction tempF;
	std::string OldDebugString;
	int ProblemCounter=0;
	//Checksum code follows:
	//std::string tempS1, tempS2;
//	this->ComputeDebugString();
//	partFraction::MakingConsistencyCheck=true;
	this->PrepareIndicatorVariables();
	this->PrepareCheckSums(theGlobalVariables);
	this->AssureIndicatorRegularity(theGlobalVariables);
	//this->IndicatorRoot.MakeZero();
	//if (partFraction::MakingConsistencyCheck)
	//{	oneFracWithMultiplicitiesAndElongations::CheckSumRoot.ComputeDebugString();
	//	this->CheckSum.ElementToString(tempS1);
	//	this->ComputeDebugString();
	//}
	std::stringstream out; std::string tempS;
	while (this->IndexLowestNonProcessed<this->size)
	{ //this->ComputeDebugString();
//		bool ShouldIgnore=false;
		this->IndexCurrentlyProcessed= this->IndexLowestNonProcessed;
		if (!this->ShouldIgnore(theGlobalVariables))
		{	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
			{	this->ElementToString(OldDebugString, theGlobalVariables);
			//	ProblemCounter++;
			//	if (ProblemCounter==14)
			//		Stop();
			}
			//tempF.Assign(this->TheObjects[this->IndexLowestNonProcessed]);
				tempFrac.Assign(this->TheObjects[this->IndexLowestNonProcessed]);
//			this->ComputeDebugString();
//			tempF.ComputeDebugString();
//			if (this->IndexLowestNonReduced==5 && this->size==9)
//			{ partFraction::flagAnErrorHasOccurredTimeToPanic=true;
//				this->ComputeDebugString();
//			}
			bool tempBool;
			if (this->flagUsingOrlikSolomonBasis)
				tempBool=tempFrac.reduceOnceGeneralMethod(*this, theGlobalVariables);
			else
				tempBool = tempFrac.reduceOnceGeneralMethodNoOSBasis(*this,theGlobalVariables);
			if (! tempBool)
			{ if (tempFrac.IndicesNonZeroMults.size<=this->AmbientDimension)
					this->IndexLowestNonProcessed++;
				else
					this->TheObjects[this->IndexLowestNonProcessed].LastDistinguishedIndex=
						tempFrac.LastDistinguishedIndex;
			}
			else
			{//	if (ProblemCounter==17)
				//	this->ComputeDebugString(theGlobalVariables);//				if (partFraction::flagAnErrorHasOccurredTimeToPanic)
				this->PopIndexHashAndAccount( this->IndexCurrentlyProcessed,theGlobalVariables);
//				{ this->ComputeDebugString();
//				}
			}
//			if (partFraction::MakingConsistencyCheck)
//			{	Rational tempRat2;
//				this->UncoverBracketsNumerators();
//				this->ComputeOneCheckSum(tempRat2);
//				if (!tempRat2.IsEqualTo(this->StartCheckSum))
//				{	this->ComputeDebugString();
//					tempRat2.ElementToString(tempS2);
//					assert(false);
//				}
//			}
		}
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{	ProblemCounter++;
			this->ComputeDebugString(theGlobalVariables);
			out <<this->DebugString<<"\\\\ = \\\\";
			tempS= out.str();
			this->CompareCheckSums(theGlobalVariables);
		}
		this->MakeProgressReportSplittingMainPart(theGlobalVariables.FeedDataToIndicatorWindowDefault);
	//	this->ComputeDebugString();
//		x= this->SizeWithoutDebugString();
	}
//	this->RemoveRedundantShortRootsClassicalRootSystem();
//	PolyFormatLocal.MakeAlphabetxi();
//	this->ComputeDebugString();
	//this->flagAnErrorHasOccurredTimeToPanic=true;
	if (this->flagAnErrorHasOccurredTimeToPanic)
	{	this->ComputeDebugString(theGlobalVariables);
		this->CompareCheckSums(theGlobalVariables);
	}
	Rational tempRat2;
	/*if (partFraction::MakingConsistencyCheck)
	{	this->ComputeDebugString();
		this->ComputeOneCheckSum(tempRat2);
		tempRat2.ElementToString(tempS2);
		tempRat.ElementToString(tempS1);
		assert(tempRat2.IsEqualTo(tempRat));
	}	*/
	//this->ComputeDebugString();
	this->RemoveRedundantShortRoots(theGlobalVariables);
	//this->ComputeDebugString();
	this->UncoverBracketsNumerators(theGlobalVariables);
	//partFraction::UncoveringBrackets=true;
	//this->ComputeDebugString();
	this->CompareCheckSums(theGlobalVariables);
	this->IndexLowestNonProcessed= this->size;
	this->MakeProgressReportSplittingMainPart(theGlobalVariables.FeedDataToIndicatorWindowDefault);
	return false;
}

bool partFractions::splitClassicalRootSystem(bool ShouldElongate, GlobalVariables& theGlobalVariables)
{ this->IndexLowestNonProcessed=0;
	this->PrepareIndicatorVariables();
	//partFraction::flagAnErrorHasOccurredTimeToPanic= true;
	//partFractions::flagAnErrorHasOccurredTimeToPanic= true;
	Rational::flagAnErrorHasOccurredTimeToPanic=true;
	this->PrepareCheckSums(theGlobalVariables);
	std::string tempS;
	this->CheckSum.ElementToString(tempS);
	// if IndicatorRoot is zero then the caller has forgotten
	// to set the flagUsingIndicatorRoot to false
	if ( this->flagUsingIndicatorRoot&& !this->IndicatorRoot.IsEqualToZero())
		this->AssureIndicatorRegularity(theGlobalVariables);
	partFraction tempF;
	while (this->IndexLowestNonProcessed<this->size)
	{ //this->ComputeDebugString();
		if (!this->ShouldIgnore(theGlobalVariables))
		{	tempF.Assign(this->TheObjects[this->IndexLowestNonProcessed]);
			//this->ComputeDebugString();
			//tempF.ComputeDebugString();
			if (! (tempF.reduceOnceTotalOrderMethod(*this,theGlobalVariables)))
			{ if (this->TheObjects[this->IndexLowestNonProcessed].IndicesNonZeroMults.size
						-this->TheObjects[this->IndexLowestNonProcessed].GetNumProportionalVectorsClassicalRootSystems(*this)
						>this->AmbientDimension)
				{ this->TheObjects[this->IndexLowestNonProcessed].ComputeDebugString(*this,theGlobalVariables);
					this->NumRelevantNonReducedFractions++;
				}
				this->IndexLowestNonProcessed++;
			}
			else
				this->PopIndexHashAndAccount( this->IndexLowestNonProcessed,theGlobalVariables);
			this->MakeProgressReportSplittingMainPart(theGlobalVariables.FeedDataToIndicatorWindowDefault);
		}
//		this->ComputeDebugString();
//		x= this->SizeWithoutDebugString();
	}
	//this->ComputeDebugString();
//	this->CompareCheckSums();
	if (ShouldElongate)
	{	this->RemoveRedundantShortRootsClassicalRootSystem(theGlobalVariables);
//	this->ComputeDebugString();
	}
	this->CompareCheckSums(theGlobalVariables);
	this->IndexLowestNonProcessed= this->size;
	this->MakeProgressReportSplittingMainPart(theGlobalVariables.FeedDataToIndicatorWindowDefault);
	return this->CheckForMinimalityDecompositionWithRespectToRoot(this->IndicatorRoot, theGlobalVariables);
}

bool partFractions::CheckForMinimalityDecompositionWithRespectToRoot(root & r, GlobalVariables& theGlobalVariables)
{	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].IndicesNonZeroMults.size>this->AmbientDimension)
		{	if (this->TheObjects[i].rootIsInFractionCone(*this,r,theGlobalVariables))
			{	this->TheObjects[i].ComputeDebugString(*this,theGlobalVariables);
				return false;
			}
		}
	}
	return true;
}

void partFraction::initFromRootSystem
	(partFractions& owner, intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights)
{	static IntegerPoly ComputationalBufferCoefficient;
	static PolyPartFractionNumerator ComputationalBufferCoefficientNonExpanded;
	if (theFraction.size==0)
		return;
	int theDimension = theFraction.TheObjects[0].dimension;
	ComputationalBufferCoefficient.MakeNVarConst((short)theDimension,IOne);
	this->Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
	ComputationalBufferCoefficientNonExpanded.MakeNVarConst((short)theDimension, IOne);
	this->CoefficientNonExpanded.AssignPolyPartFractionNumerator(ComputationalBufferCoefficientNonExpanded);
	owner.RootsToIndices.initFromRoots(theAlgorithmBasis,weights);
	this->init(owner.RootsToIndices.size);
	for(int i=0;i<owner.RootsToIndices.size;i++)
		this->TheObjects[i].init();
	for (int i=0;i<theFraction.size;i++)
	{ int index = owner.RootsToIndices.getIndex(theFraction.TheObjects[i]);
		this->TheObjects[index].AddMultiplicity(1,1);
	}
	this->ComputeIndicesNonZeroMults();
}

int partFractions::ReadFromFileComputedContributions(std::fstream& input, GlobalVariables&  theGlobalVariables)
{ std::string tempS;
	input.seekg(0);
	input>>tempS;
	int lastNonZero=-1;
	for (int i=0;i<this->size;i++)
	{ int x;
		input>>x>>this->TheObjects[i].FileStoragePosition;
		if (this->TheObjects[i].FileStoragePosition>lastNonZero)
			lastNonZero=this->TheObjects[i].FileStoragePosition;
		assert(x==i);
	}
	return lastNonZero;
}

void partFractions::WriteToFileComputedContributions(std::fstream& output, GlobalVariables&  theGlobalVariables)
{ output.seekp(0);
	output<< "Partial_fraction_index/file_storage_position\n";
	for (int i=0;i<this->size;i++)
		output<< i<<" "<<this->TheObjects[i].FileStoragePosition<<"\n";
}


partFractions::partFractions()
{ this->HighestIndex=-1;
	this->IndexLowestNonProcessed=-2;
	this->flagSplitTestModeNoNumerators=false;
	this->flagUsingIndicatorRoot=true;
	this->flagDiscardingFractions=false;
	this->flagUsingCheckSum=false;
	this->flagUsingOrlikSolomonBasis=true;
	this->IndicatorRoot.MakeZero(0);
}

void partFraction::ComputeDebugString(partFractions& owner, GlobalVariables& theGlobalVariables)
{ this->ElementToString
		(owner,this->DebugString, PolynomialOutputFormat::UsingLatexFormat,false,true, theGlobalVariables);
}

/*void partFraction::InsertNewRootIndex(int index)
{ this->ShiftUpExpandOnTop(index);
	this->TheObjects[index].Multiplicities.TheObjects[0]=0;
}*/

void partFractions::PopIndexHashAndAccount(int index, GlobalVariables& theGlobalVariables)
{ this->AccountPartFractionInternals(-1, index, theGlobalVariables);
	if (index>=this->IndexLowestNonProcessed )
		this->PopIndexSwapWithLastHash(index);
	else
	{	assert(this->IndexLowestNonProcessed!=0);
		if (this->IndexLowestNonProcessed-1== this->IndexCurrentlyProcessed)
			this->IndexCurrentlyProcessed=index;
		this->SwapTwoIndicesHash(index, this->IndexLowestNonProcessed-1);
		this->PopIndexSwapWithLastHash(this->IndexLowestNonProcessed-1);
		this->IndexLowestNonProcessed--;
	}
}

void partFractions::AccountPartFractionInternals(int sign, int index, GlobalVariables& theGlobalVariables)
{	partFraction& tempFrac= this->TheObjects[index];
	if (this->flagUsingIndicatorRoot)
	{	if (tempFrac.rootIsInFractionCone(*this,this->IndicatorRoot, theGlobalVariables ))
		{	if (tempFrac.IndicesNonZeroMults.size<=this->AmbientDimension)
			{	this->NumGeneratorsRelevenatFractions+=sign*(tempFrac.CoefficientNonExpanded.NumGeneratorsUsed());
				if (partFraction::UncoveringBrackets)
					this->NumMonomialsInNumeratorsRelevantFractions+=sign*(tempFrac.Coefficient.size);
				else
					this->NumMonomialsInNumeratorsRelevantFractions+=sign*tempFrac.CoefficientNonExpanded.size;
				this->NumTotalReduced+=sign;
				this->NumberRelevantReducedFractions+=sign;
			}
		}
		else
		{	this->NumGeneratorsIrrelevantFractions+=sign*tempFrac.CoefficientNonExpanded.NumGeneratorsUsed();
			if (partFraction::UncoveringBrackets)
				this->NumMonomialsInNumeratorsIrrelevantFractions+=sign*tempFrac.Coefficient.size;
			else
				this->NumMonomialsInNumeratorsIrrelevantFractions+=sign* tempFrac.CoefficientNonExpanded.size;
			this->NumTotalReduced+=sign;
			this->NumberIrrelevantFractions+=sign;
		}
	}
	else
	{	if (tempFrac.IndicesNonZeroMults.size<=this->AmbientDimension)
		{	this->NumGeneratorsRelevenatFractions+=sign*(tempFrac.CoefficientNonExpanded.NumGeneratorsUsed());
			if (partFraction::UncoveringBrackets)
				this->NumMonomialsInNumeratorsRelevantFractions+=sign*(tempFrac.Coefficient.size);
			else
				this->NumMonomialsInNumeratorsRelevantFractions+=sign*tempFrac.CoefficientNonExpanded.size;
			this->NumTotalReduced+=sign;
			this->NumberRelevantReducedFractions+=sign;
		}
	}
	this->NumGeneratorsInTheNumerators+=sign*tempFrac.CoefficientNonExpanded.NumGeneratorsUsed();
	if (partFraction::UncoveringBrackets)
		this->NumMonomialsInTheNumerators+=sign*tempFrac.Coefficient.size;
	else
		this->NumMonomialsInTheNumerators+=sign*tempFrac.CoefficientNonExpanded.size;
}

void partFraction::ReduceMonomialByMonomial
	(partFractions& owner, int myIndex, GlobalVariables& theGlobalVariables)
{	partFraction tempFrac;
	//tempFrac.Assign(*this);
	Rational StartCheckSum, theDiff;
	int ProblemCounter=-1;
	if (this->flagAnErrorHasOccurredTimeToPanic)
	{ owner.ComputeOneCheckSum(StartCheckSum,theGlobalVariables);
		this->ComputeOneCheckSum(owner, theDiff,owner.AmbientDimension,theGlobalVariables);
		this->ComputeDebugString(owner, theGlobalVariables);
		owner.NumRunsReduceMonomialByMonomial++;
		ProblemCounter=owner.NumRunsReduceMonomialByMonomial;
		if (ProblemCounter==8)
		{	Stop();	owner.ComputeDebugString(theGlobalVariables);
		}
	}
	MatrixLargeRational& tempMat= theGlobalVariables.matReduceMonomialByMonomial;
	MatrixLargeRational& startAsIdMat = theGlobalVariables.matIdMatrix;
	MatrixLargeRational& matColumn = theGlobalVariables.matOneColumn;
	MatrixLargeRational& matLinComb = theGlobalVariables.matReduceMonomialByMonomial2;
	Selection& tempSel= theGlobalVariables.selReduceMonomialByMonomial;
	Monomial<Integer> tempMon; intRoot tempRoot;
	tempMat.init(owner.AmbientDimension,(short) this->IndicesNonZeroMults.size);
	for (int i=0;i<this->IndicesNonZeroMults.size;i++)
		for (int j=0;j<owner.AmbientDimension;j++)
			tempMat.elements[j][i].AssignInteger(owner.RootsToIndices
				.TheObjects[this->IndicesNonZeroMults.TheObjects[i]].elements[j]*
					this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]].GetLargestElongation());
	if (this->flagAnErrorHasOccurredTimeToPanic)
	{	this->ComputeDebugString(owner,theGlobalVariables);
		tempMat.ComputeDebugString();
	}
	startAsIdMat.MakeIdMatrix(owner.AmbientDimension);
	MatrixLargeRational::GaussianEliminationByRows(tempMat,startAsIdMat,tempSel,false);
	if (this->flagAnErrorHasOccurredTimeToPanic)
	{ startAsIdMat.ComputeDebugString();
		tempMat.ComputeDebugString();
	}
	SelectionWithDifferentMaxMultiplicities thePowers;
	ListBasicObjects<int> thePowersSigned;
	thePowersSigned.SetSizeExpandOnTopNoObjectInit(this->IndicesNonZeroMults.size);
	thePowers.initIncomplete(this->IndicesNonZeroMults.size);
	for (int k=0;k<this->Coefficient.size;k++)
	{ this->Coefficient.TheObjects[k].MonomialExponentToColumnMatrix(matColumn);
		if (this->flagAnErrorHasOccurredTimeToPanic)
			matColumn.ComputeDebugString();
		matColumn.MultiplyOnTheLeft(startAsIdMat);
		if (this->flagAnErrorHasOccurredTimeToPanic)
			matColumn.ComputeDebugString();
		tempFrac.AssignDenominatorOnly(*this);
		tempFrac.Coefficient.SetSizeExpandOnTopLight(1);
		tempFrac.Coefficient.TheObjects[0].Assign(this->Coefficient.TheObjects[k]);
		if (	tempMat.RowEchelonFormToLinearSystemSolution
						(tempSel,matColumn,matLinComb))
		{ tempMon.Assign(this->Coefficient.TheObjects[k]);
			if (this->flagAnErrorHasOccurredTimeToPanic)
				matLinComb.ComputeDebugString();
			for (int i=0;i<matLinComb.NumRows;i++)
			{	thePowers.MaxMultiplicities.TheObjects[i]=0;
				if (	matLinComb.elements[i][0].IsGreaterThanOrEqualTo(ROne) ||
							matLinComb.elements[i][0].IsNegative() )
        { int tempI=matLinComb.elements[i][0].floor();
          thePowersSigned.TheObjects[i]=tempI;
          if (tempI<0)
						thePowers.MaxMultiplicities.TheObjects[i]=
							this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]]
								.GetMultiplicityLargestElongation();
					else
						thePowers.MaxMultiplicities.TheObjects[i]=MathRoutines::Minimum
							(	tempI,this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]]
									.GetMultiplicityLargestElongation());
					tempRoot=owner.RootsToIndices.TheObjects
						[this->IndicesNonZeroMults.TheObjects[i]];
					tempRoot.MultiplyByInteger
						(	thePowersSigned.TheObjects[i]*
							this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]]
								.GetLargestElongation());
					tempMon.DivideByExponentOnly(tempRoot);
					if (this->flagAnErrorHasOccurredTimeToPanic)
						tempMon.ComputeDebugString();
        } else
        {	thePowers.Multiplicities.TheObjects[i]=0;
					thePowersSigned.TheObjects[i]=0;
        }
			}
			thePowers.ComputeElements();
			int numSummands=thePowers.getTotalNumSubsets();
			if (numSummands==1)
				owner.AddAlreadyReduced(tempFrac,theGlobalVariables);
			else
			{	partFractions tempFracs;
				Rational tempDiff;
				if (this->flagAnErrorHasOccurredTimeToPanic)
				{	tempFracs.initFromOtherPartFractions(owner, theGlobalVariables);
					tempFrac.AssignDenominatorOnly(*this);
					tempFrac.Coefficient.Nullify((short)owner.AmbientDimension);
					tempFrac.Coefficient.AddObjectOnTopLight(this->Coefficient.TheObjects[k]);
					tempFrac.ComputeOneCheckSum(owner,tempDiff,owner.AmbientDimension,theGlobalVariables);
				}
				for (int l=0;l<numSummands;l++)
				{	if (ProblemCounter==8 )
						Stop();
					tempFrac.AssignDenominatorOnly(*this);
					if (this->flagAnErrorHasOccurredTimeToPanic)
						thePowers.ComputeDebugString();
					tempFrac.ReduceMonomialByMonomialModifyOneMonomial
						(owner, theGlobalVariables,thePowers,thePowersSigned,tempMon);
					if (this->flagAnErrorHasOccurredTimeToPanic)
						tempFrac.ComputeDebugString(owner,theGlobalVariables);
					tempFrac.ReduceMonomialByMonomial(owner,-1,theGlobalVariables);
					if (this->flagAnErrorHasOccurredTimeToPanic)
						tempFrac.ReduceMonomialByMonomial(tempFracs,-1,theGlobalVariables);
					thePowers.IncrementSubset();
				}
				if (this->flagAnErrorHasOccurredTimeToPanic)
				{ Rational tempFracsCheckSum;
					tempFracs.ComputeOneCheckSum(tempFracsCheckSum,theGlobalVariables);
					tempFracs.ComputeDebugString(theGlobalVariables);
					assert(tempFracsCheckSum.IsEqualTo(tempDiff));
				}
			}
		}
		else
			owner.AddAlreadyReduced(tempFrac,theGlobalVariables);
	}
	if (this->flagAnErrorHasOccurredTimeToPanic)
	{ Rational tempRat;
		owner.ComputeOneCheckSum(tempRat,theGlobalVariables);
		tempRat.Subtract(theDiff);
		assert(tempRat.IsEqualTo(StartCheckSum));
	}
}

void partFraction::ReduceMonomialByMonomialModifyOneMonomial
	(	partFractions &Accum, GlobalVariables &theGlobalVariables,
		SelectionWithDifferentMaxMultiplicities &thePowers,
		ListBasicObjects<int> &thePowersSigned, Monomial<Integer> &input)
{	IntegerPoly& theNumerator=
		theGlobalVariables.IPReduceMonomialByMonomialModifyOneMonomial1;
	IntegerPoly& tempP=
		theGlobalVariables.IPReduceMonomialByMonomialModifyOneMonomial2;
	theNumerator.Nullify((short)Accum.AmbientDimension);
	theNumerator.AddMonomial(input);
	assert(thePowersSigned.size== thePowers.Multiplicities.size);
	if (this->flagAnErrorHasOccurredTimeToPanic)
		theNumerator.ComputeDebugString();
	for (int j=0;j<thePowers.Multiplicities.size;j++)
	{ int currentIndexInFraction = this->IndicesNonZeroMults.TheObjects[j];
		int currentElongation=
			this->TheObjects[currentIndexInFraction].GetLargestElongation();
		int MultChange= thePowers.Multiplicities.TheObjects[j];
		int MaxMultchange=
			this->TheObjects[currentIndexInFraction].GetMultiplicityLargestElongation();
		intRoot tempRoot;
		tempRoot= Accum.RootsToIndices.TheObjects[currentIndexInFraction];
		tempRoot.MultiplyByInteger(currentElongation);
		this->GetPolyReduceMonomialByMonomial
			(	Accum, theGlobalVariables,tempRoot,thePowersSigned.TheObjects[j],MultChange,
				MaxMultchange,tempP);
		if (this->flagAnErrorHasOccurredTimeToPanic)
			tempP.ComputeDebugString();
		theNumerator.MultiplyBy(tempP);
		if (this->flagAnErrorHasOccurredTimeToPanic)
			theNumerator.ComputeDebugString();
		this->TheObjects[currentIndexInFraction].AddMultiplicity
			(-MultChange,currentElongation);
	}
	this->Coefficient.AssignPolynomial(theNumerator);
	this->ComputeIndicesNonZeroMults();
	if (this->flagAnErrorHasOccurredTimeToPanic)
		this->ComputeDebugString(Accum, theGlobalVariables);
}

void partFraction::GetPolyReduceMonomialByMonomial
	(	partFractions& owner, GlobalVariables& theGlobalVariables,
		intRoot& theExponent, int StartMonomialPower, int DenPowerReduction,
		int startDenominatorPower,IntegerPoly& output)
{ if (StartMonomialPower==0)
	{	output.MakeNVarConst((short)owner.AmbientDimension,IOne);
		return;
	}
	Monomial<Integer> tempMon;	tempMon.init((short)owner.AmbientDimension);
	output.Nullify((short)owner.AmbientDimension);
	if (StartMonomialPower>0)
	{	if (DenPowerReduction!=startDenominatorPower)
		{	tempMon.Coefficient.value= MathRoutines::NChooseK
				(StartMonomialPower,DenPowerReduction);
			tempMon.Coefficient.value*=MathRoutines::parity(DenPowerReduction);
			output.AddMonomial(tempMon);
		} else
		{	intRoot tempRoot;
			assert(StartMonomialPower>=startDenominatorPower);
			for (int k=0;k<=StartMonomialPower-startDenominatorPower;k++)
			{ tempRoot= theExponent; tempRoot.MultiplyByInteger(k);
				tempMon.MakeFromRoot(IOne,tempRoot);
				tempMon.Coefficient.value= MathRoutines::parity(startDenominatorPower)*
					MathRoutines::NChooseK(StartMonomialPower-1-k,startDenominatorPower-1);
				output.AddMonomial(tempMon);
			}
		}
	}
	if (StartMonomialPower<0 )
	{ if (DenPowerReduction!=startDenominatorPower)
		{	tempMon.Coefficient.value= MathRoutines::NChooseK
				(-StartMonomialPower-1+DenPowerReduction,DenPowerReduction);
			output.AddMonomial(tempMon);
		} else
		{ intRoot tempRoot;
			for (int k=1;k<=-StartMonomialPower;k++)
			{ tempRoot= theExponent; tempRoot.MultiplyByInteger(-k);
				tempMon.MakeFromRoot(IOne,tempRoot);
				tempMon.Coefficient.value= MathRoutines::NChooseK
					(startDenominatorPower-StartMonomialPower-1-k,startDenominatorPower-1);
				output.AddMonomial(tempMon);
			}
		}
	}
	output.ComputeDebugString();
}

void partFractions::AddAndReduce(partFraction &f, GlobalVariables& theGlobalVariables)
{	if (this->flagUsingOrlikSolomonBasis)
		f.ReduceMonomialByMonomial(*this,-1,theGlobalVariables);
	else
		this->AddAlreadyReduced(f,theGlobalVariables);
}

void partFractions::AddAlreadyReduced(partFraction &f, GlobalVariables& theGlobalVariables)
{ bool shouldAttemptReduction=false;
	int tempI=this->ContainsObjectHash(f);
	if (tempI==-1)
	{ this->AddObjectOnTopHash(f);
		tempI= this->size-1;
		this->TheObjects[tempI].RelevanceIsComputed=false;
		this->AccountPartFractionInternals(1,tempI,theGlobalVariables);
	}
	else
	{ IntegerPoly& ComputationalBufferCoefficient1 =
			theGlobalVariables.IPComputationalBufferAddPartFraction1;
		IntegerPoly& ComputationalBufferCoefficient2 =
			theGlobalVariables.IPComputationalBufferAddPartFraction2;
		PolyPartFractionNumerator& ComputationalBufferCoefficientNonExpanded1
			=theGlobalVariables.PPFNAddPartFraction1;
		PolyPartFractionNumerator& ComputationalBufferCoefficientNonExpanded2
			=theGlobalVariables.PPFNAddPartFraction2;
		//Accounting
		this->AccountPartFractionInternals(-1,tempI,theGlobalVariables);
		//end of accounting
		ComputationalBufferCoefficient1.AssignPolynomialLight(this->TheObjects[tempI].Coefficient);
		ComputationalBufferCoefficient2.AssignPolynomialLight(f.Coefficient);
		this->TheObjects[tempI].CoefficientNonExpanded.ComputePolyPartFractionNumerator
			(ComputationalBufferCoefficientNonExpanded1,this->AmbientDimension);
		f.CoefficientNonExpanded.ComputePolyPartFractionNumerator
			(ComputationalBufferCoefficientNonExpanded2,this->AmbientDimension);
		ComputationalBufferCoefficient1.AddPolynomial
			(ComputationalBufferCoefficient2);
		if (ComputationalBufferCoefficient1.EvaluateAtOne()==0)
		{	if (this->flagAnErrorHasOccurredTimeToPanic)
				ComputationalBufferCoefficient1.ComputeDebugString();
			shouldAttemptReduction=true;
		}
		ComputationalBufferCoefficientNonExpanded1.AddPolynomial
			(ComputationalBufferCoefficientNonExpanded2);
		this->TheObjects[tempI].Coefficient.AssignPolynomial
			(ComputationalBufferCoefficient1);
		this->TheObjects[tempI].CoefficientNonExpanded.AssignPolyPartFractionNumerator
			(ComputationalBufferCoefficientNonExpanded1);
		this->AccountPartFractionInternals(1,tempI,theGlobalVariables);
	}
	if (this->flagSplitTestModeNoNumerators)
		this->TheObjects[tempI].Coefficient.MakeConst(IOne,(short)this->IndicatorRoot.size);
	if ( this->TheObjects[tempI].IsEqualToZero())
	{ this->PopIndexHashAndAccount(tempI,theGlobalVariables);
		shouldAttemptReduction=false;
	}
	if (shouldAttemptReduction&& this->flagUsingOrlikSolomonBasis)
	{	if (this->flagAnErrorHasOccurredTimeToPanic)
			this->TheObjects[tempI].ComputeDebugString(*this,theGlobalVariables);
		this->TheObjects[tempI].AttemptReduction(*this,tempI,theGlobalVariables);
		if (this->flagAnErrorHasOccurredTimeToPanic)
			this->TheObjects[tempI].ComputeDebugString(*this,theGlobalVariables);
	}
}

int partFractions::ElementToString(std::string& output, bool LatexFormat,
																		bool includeVPsummand, bool includeNumerator,
																		GlobalVariables& theGlobalVariables)
{ static MatrixIntTightMemoryFit tempMat;
	return this->ElementToStringBasisChange
		(tempMat,false,output,LatexFormat,includeVPsummand,includeNumerator, theGlobalVariables);
}

int partFractions::ElementToStringOutputToFile
	(	std::fstream& output, bool LatexFormat, bool includeVPsummand,
		bool includeNumerator, GlobalVariables& theGlobalVariables)
{ static MatrixIntTightMemoryFit tempMat;
	return this->ElementToStringBasisChangeOutputToFile
		(tempMat,false,output,LatexFormat,includeVPsummand,includeNumerator,theGlobalVariables);
}

int partFractions::ElementToStringBasisChange
	(	MatrixIntTightMemoryFit& VarChange,
		bool UsingVarChange, std::string& output,
		bool LatexFormat,bool includeVPsummand,
		bool includeNumerator, GlobalVariables& theGlobalVariables )
{ std::stringstream out;
	std::string tempS;
	int TotalLines=0;
	PolyFormatLocal.ExtraLinesCounterLatex=0;
	if (LatexFormat)
	{ out << "\\begin{eqnarray*}\n";}
	int LastCutOff=0;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.size>0 )
		{ TotalLines+=this->TheObjects[i].ElementToStringBasisChange
				(	*this, VarChange, UsingVarChange,tempS, LatexFormat,
					includeVPsummand,includeNumerator,theGlobalVariables);
			if (LatexFormat)
				out <<"&&";
			if (tempS[0]!='-'){out<<"+";}
			out<<tempS;
			if (LatexFormat)
			{ out<<"\\\\ \n";
				TotalLines++;
			}
			else
			{out <<"\n";}
			if	( LatexFormat &&
						(TotalLines-LastCutOff)> PolynomialOutputFormat::LatexCutOffLine)
			{ out<<"\\end{eqnarray*}\\begin{eqnarray*}\n";
				LastCutOff=TotalLines;
			}
		}
		if (TotalLines>this->flagMaxNumStringOutputLines)
		{ out<< "\n Number of lines exceeded " << this->flagMaxNumStringOutputLines
				 << "; The rest of the output was suppressed.";
			break;
		}
	}
	if (!LatexFormat)
	{	output= out.str();
		if (output.size()>0)
			if (output[0]=='+')	output.erase(0,1);
	}
	else
	{ out << "\\end{eqnarray*}";
		output= out.str();
	}
	return TotalLines;
}

int partFractions::ElementToStringBasisChangeOutputToFile
	(	MatrixIntTightMemoryFit& VarChange, bool UsingVarChange,
		std::fstream& output, bool LatexFormat,bool includeVPsummand,
		bool includeNumerator,GlobalVariables& theGlobalVariables)
{ std::string tempS;
	int TotalLines=0;
	PolyFormatLocal.ExtraLinesCounterLatex=0;
	if (LatexFormat)
	{ output << "\\begin{eqnarray*}\n";}
	int LastCutOff=0;
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.size>0 )
		{ TotalLines+=this->TheObjects[i].ElementToStringBasisChange
				(	*this,VarChange, UsingVarChange,tempS, LatexFormat,
					includeVPsummand,includeNumerator, theGlobalVariables);
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
void partFractions::MakeProgressReportSplittingMainPart(FeedDataToIndicatorWindow reportFunction)
{ std::stringstream out1,out2,out3;
	out1<< this->NumberRelevantReducedFractions << " relevant reduced + "
			<< this->NumberIrrelevantFractions << " disjoint = " << this->NumTotalReduced;
	if (this->NumRelevantNonReducedFractions!=0)
	{ out1<<" + " << this->NumRelevantNonReducedFractions <<" relevant unreduced ";
	}
	out1<<" out of "<<this->size <<" total fractions";
	IndicatorWindowGlobalVariables.ProgressReportString1= out1.str();
	out2<<this->NumMonomialsInNumeratorsRelevantFractions << " relevant reduced + "
			<< this->NumMonomialsInNumeratorsIrrelevantFractions << " disjoint = "
			<< this->NumMonomialsInNumeratorsRelevantFractions +this->NumMonomialsInNumeratorsIrrelevantFractions
			<<" out of "<<this->NumMonomialsInTheNumerators
			<<" total monomials in the numerators";
	IndicatorWindowGlobalVariables.ProgressReportString2= out2.str();
	if (this->NumGeneratorsInTheNumerators!=0)
	{	out3<<this->NumGeneratorsRelevenatFractions << " relevant reduced + "
			<< this->NumGeneratorsIrrelevantFractions << " disjoint = "
			<< this->NumGeneratorsIrrelevantFractions +this->NumGeneratorsRelevenatFractions
			<<" out of "<<this->NumGeneratorsInTheNumerators
			<<" total generators in the numerators";
		IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
	} else
	{ IndicatorWindowGlobalVariables.ProgressReportString3.clear();
	}
	if (reportFunction!=0)
		reportFunction(IndicatorWindowGlobalVariables);
}

void partFractions::MakeProgressVPFcomputation(FeedDataToIndicatorWindow reportFunction)
{ this->NumProcessedForVPFfractions++;
	std::stringstream out2, out3;
	out2	<< "Processed " << this->NumProcessedForVPFfractions<<" out of " <<this->NumberRelevantReducedFractions
				<< " relevant fractions";
//	out3	<< "Processed " <<" out of " <<this->NumMonomialsInNumeratorsRelevantFractions
//				<< " relevant fractions";
	::IndicatorWindowGlobalVariables.ProgressReportString2= out2.str();
	//::IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
	if (reportFunction!=0)
		reportFunction(::IndicatorWindowGlobalVariables);
}

void partFractions::ComputeOneCheckSum
	(Rational &output,GlobalVariables& theGlobalVariables)
{ output.MakeZero();
	for(int i=0;i<this->size;i++)
	{ Rational tempRat;
		this->TheObjects[i].ComputeOneCheckSum
			(*this,tempRat, this->AmbientDimension,theGlobalVariables);
		if (this->flagAnErrorHasOccurredTimeToPanic)
		{ std::string tempS;
			tempRat.ElementToString(tempS);
		}
		output.Add(tempRat);
		if (this->flagMakingProgressReport)
		{ std::stringstream out;
			out <<"Checksum "<<i+1<<" out of "<<this->size;
			IndicatorWindowGlobalVariables.ProgressReportString4= out.str();
			if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
				theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
	}
}

void partFractions::initFromRootSystem
	(	intRoots& theFraction, intRoots& theAlgorithmBasis,
		intRoot* weights, GlobalVariables& theGlobalVariables)
{ this->ClearTheObjects();
	this->RootsToIndices.ClearTheObjects();
	partFraction f;
	this->AmbientDimension= theFraction.TheObjects[0].dimension;
	f.initFromRootSystem(*this,theFraction,theAlgorithmBasis, weights);
	this->AddAlreadyReduced(f,theGlobalVariables);
}

void partFractions::RemoveRedundantShortRoots(GlobalVariables& theGlobalVariables)
{	partFraction tempFrac;
	Rational startCheckSum,tempCheckSum, tempCheckSum2,tempCheckSum3;
	if (partFraction::MakingConsistencyCheck)
		this->ComputeOneCheckSum(startCheckSum,theGlobalVariables);
	for (int i=0;i<this->size;i++)
	{ tempFrac.Assign(this->TheObjects[i]);
		root* tempPointerRoot=0;
		if (this->flagUsingIndicatorRoot){tempPointerRoot=&this->IndicatorRoot;}
		if(tempFrac.RemoveRedundantShortRoots(*this,tempPointerRoot,theGlobalVariables,this->AmbientDimension))
		{	this->AccountPartFractionInternals(-1,i,theGlobalVariables);
			this->TheObjects[i].CoefficientNonExpanded.SetSizeExpandOnTopLight(0);
			this->TheObjects[i].Coefficient.Nullify((short)this->AmbientDimension);
			int oldsize= this->size;
			this->AddAlreadyReduced(tempFrac,theGlobalVariables);
			assert(oldsize<=this->size);
			if (this->flagMakingProgressReport)
			{ std::stringstream out;
				out<<"Elongating denominator "<<i+1<<" out of "<<this->size;
				IndicatorWindowGlobalVariables.ProgressReportString3=out.str();
				if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
					theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
			}
 		}
	}
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.IsEqualToZero())
		{ this->PopIndexSwapWithLastHash(i);
			i--;
		}
	}
}

void partFractions::RemoveRedundantShortRootsClassicalRootSystem
	(GlobalVariables& theGlobalVariables)
{	partFraction tempFrac;
	for (int i=0;i<this->size;i++)
	{ tempFrac.Assign(this->TheObjects[i]);
		root* tempRoot=0;
		if (this->flagUsingIndicatorRoot)
		{ tempRoot=&this->IndicatorRoot;
		}
		if(tempFrac.RemoveRedundantShortRootsClassicalRootSystem
			(*this,tempRoot,theGlobalVariables,this->AmbientDimension))
		{	this->TheObjects[i].Coefficient.Nullify((short)this->AmbientDimension);
			this->AddAlreadyReduced(tempFrac,theGlobalVariables);
		}
		if (this->flagMakingProgressReport)
		{ std::stringstream out;
			out<<"Elongating denominator "<<i+1<<" out of "<<this->size;
			IndicatorWindowGlobalVariables.ProgressReportString4=out.str();
			if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
				theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
	}
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].Coefficient.IsEqualToZero())
		{ this->PopIndexHashAndAccount(i,theGlobalVariables);
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

bool partFractions::VerifyFileComputedContributions(GlobalVariables&  theGlobalVariables)
{	int tempI= this->ReadFromFileComputedContributions(partFractions::ComputedContributionsList,theGlobalVariables);
	std::filebuf* pbuf = partFraction::TheBigDump.rdbuf();
	int tempSize = pbuf->pubseekoff(0,std::fstream::end);
	partFraction::TheBigDump.seekp(tempSize);
	return(tempSize>=tempI);
}

bool partFractions::partFractionsToPartitionFunctionAdaptedToRoot
				(	QuasiPolynomial& output, root& r, //bool storeComputations,
					//bool RecordSplitPowerSeriesCoefficient,
					bool StoreToFile,
					bool UseOldData, GlobalVariables& theGlobalVariables)
{	if (!this->CheckForMinimalityDecompositionWithRespectToRoot(r,theGlobalVariables)){return false;}
	this->NumProcessedForVPFfractions=0;
	Rational oldCheckSum;
	QuasiPolynomial oldOutput;
	if (!this->flagUsingIndicatorRoot)
	{ this->flagUsingIndicatorRoot=true;
		this->AssureIndicatorRegularity(theGlobalVariables);
		this->ResetRelevanceIsComputed();
	}
	if (partFraction::MakingConsistencyCheck)
		partFractions::CheckSum.MakeZero();
	if (StoreToFile&& UseOldData)
	{ bool tempBool = this->VerifyFileComputedContributions(theGlobalVariables);
		assert(tempBool);
	}
	IndicatorWindowGlobalVariables.NumProcessedMonomialsCurrentFraction=0;
	output.Nullify((short)this->AmbientDimension);
	///////////////////////////////////////////////
	//this->flagAnErrorHasOccurredTimeToPanic=true;
	//partFraction::flagAnErrorHasOccurredTimeToPanic=true;
	//this->ComputeDebugString();
	///////////////////////////////////////////////
	static QuasiPolynomial tempQP;
	for (int i=0;i<this->size;i++)
	{ assert(partFraction::TheBigDump.is_open()||!StoreToFile);
		assert(partFractions::ComputedContributionsList.is_open()||!StoreToFile);
		//if (this->flagAnErrorHasOccurredTimeToPanic)
		//{ this->TheObjects[i].ComputeDebugString();
		//}
		if (this->TheObjects[i].rootIsInFractionCone(*this,r,theGlobalVariables))
		{ this->TheObjects[i].partFractionToPartitionFunctionSplit
				(*this,tempQP,true,StoreToFile,theGlobalVariables,this->AmbientDimension);
			if (StoreToFile)
			{ this->WriteToFileComputedContributions(partFractions::ComputedContributionsList,theGlobalVariables);
				FileSetPutPointerToEnd
					(partFractions::ComputedContributionsList, StoreToFile);
				FileSetPutPointerToEnd
					(partFraction::TheBigDump, StoreToFile);
			}
/*			if(partFraction::MakingConsistencyCheck)
			{	Rational tempLRat2, tempLRat3, tempLRat4;
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
					QuasiPolynomial::flagAnErrorHasOccurredTimeToPanic=true;
					::RandomCodeIDontWantToDelete theEvilBug;
					theEvilBug.EvilPoly1.Assign(output);
					theEvilBug.EvilPoly2.Assign(tempQP);
					theEvilBug.RevealTheEvilConspiracy();
					RandomCodeIDontWantToDelete::UsingEvilList1=false;
				}
			}*/
			output.AddPolynomial(tempQP);
	/*		if (partFraction::MakingConsistencyCheck)
			{	Rational tempLRat;
				output.Evaluate(partFraction::theVectorToBePartitioned,tempLRat);
				if ((! tempLRat.IsEqualTo(partFractions::CheckSum)) || (! tempLRat.den.IsEqualTo(LIOne)))
				{ std::string tempS,tempS2,tempS3,tempS4;
					Rational tempLRat2;
					tempQP.Evaluate(partFraction::theVectorToBePartitioned,tempLRat2);
					partFractions::CheckSum.ElementToString(tempS);
					tempLRat.ElementToString(tempS2);
					oldCheckSum.ElementToString(tempS3);
					tempLRat2.ElementToString(tempS4);
					partFraction::flagAnErrorHasOccurredTimeToPanic=true;
	//				assert(false);
				}
				oldCheckSum.Assign(partFractions::CheckSum);
				oldOutput.Assign(output);
			}*/
			this->MakeProgressVPFcomputation(theGlobalVariables.FeedDataToIndicatorWindowDefault);
		}
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

void partFractions::ComputeDebugString(GlobalVariables& theGlobalVariables)
{ this->ElementToString
		(	this->DebugString,PolynomialOutputFormat::UsingLatexFormat,
			false,true,theGlobalVariables);
}

void partFractions::ElementToString(std::string& output, GlobalVariables& theGlobalVariables)
{ this->ElementToString
		(	output,PolynomialOutputFormat::UsingLatexFormat,
			false,true,theGlobalVariables);
}


void partFractions::ComputeDebugStringNoNumerator(GlobalVariables& theGlobalVariables)
{ this->ElementToString
		(this->DebugString,PolynomialOutputFormat::UsingLatexFormat,false,false,theGlobalVariables);
}

void partFractions::ComputeDebugStringWithVPfunction(GlobalVariables& theGlobalVariables)
{ this->ElementToString(this->DebugString,PolynomialOutputFormat::UsingLatexFormat,true,true,theGlobalVariables);
}

void partFractions::WriteToFile(std::fstream& output, GlobalVariables&  theGlobalVariables)
{ std::string tempS;
	output<<"Dimension: ";
	output<<this->AmbientDimension<<"\n";
	output<<"Indices_of_roots:\n";
	for (int i=0;i<this->RootsToIndices.size;i++)
	{ this->RootsToIndices.TheObjects[i].ElementToString(tempS);
		output<<"| "<<i<<"    "<<tempS <<"\n";
	}
	output<<"Alphabet_used:\n";
	for (int i=0;i<this->AmbientDimension;i++)
		output<<PolyFormatLocal.alphabet[i]<<" ";
	output<<"\n"<<"Number_of_fractions: " <<this->size<<"\n";
	for (int i=0;i<this->size;i++)
		this->TheObjects[i].WriteToFile(output, theGlobalVariables);
}

void partFractions::ReadFromFile(std::fstream& input, GlobalVariables&  theGlobalVariables)
{ intRoots tempRoots;
	this->PrepareIndicatorVariables();
	//input.seekg(0,std::ios::end);
	//int bufferSize= input.tellg();
	//bufferSize = ::Minimum(bufferSize,this->MaxReadFileBufferSize)+1;
	//char* TempBuffer= new char[bufferSize];
	//input.rdbuf()->pubsetbuf(TempBuffer,bufferSize);
	//input.rdbuf()->sgetn(TempBuffer, bufferSize);
//	input.read(TempBuffer,bufferSize+1);
//	std::filebuf* oldInputBuffer;
//	oldInputBuffer=input.rdbuf();
//	input.rdbuf(TempBuffer);
  assert(input.is_open());
	input.seekg(0);
	std::string tempS;
	input>>tempS;
	input>>this->AmbientDimension;
	input>>tempS;
	intRoot tempRoot;
	for (input>>tempS; tempS!="Alphabet_used:";input>>tempS)
	{ int tempI;
		input>>tempI;
		input>>tempS;
		for (int i=0;i<this->AmbientDimension;i++)
		{	input>>tempRoot.elements[i];
			input>>tempS;
		}
		tempRoots.AddObjectOnTop(tempRoot);
	}
	tempRoots.ComputeDebugString();
	this->RootsToIndices.initFromRoots(tempRoots,0);
	for (int i=0;i<this->AmbientDimension;i++)
	{ input>>tempS;
		PolyFormatLocal.alphabet[i]=tempS;
	}
	int tempI;
	input>>tempS;
	assert(tempS=="Number_of_fractions:");
	input >>tempI;
	partFraction tempFrac;
	this->MakeActualSizeAtLeastExpandOnTop(tempI);
	for(int i=0;i<tempI;i++)
	{ tempFrac.ReadFromFile(*this,input, theGlobalVariables,this->AmbientDimension);
		this->AddAlreadyReduced(tempFrac, theGlobalVariables);
		this->MakeProgressVPFcomputation(theGlobalVariables.FeedDataToIndicatorWindowDefault);
	}
}

void partFractions::ComputeSupport( ListBasicObjects<roots>& output, std::stringstream& outputString)
{ output.size=0;
	output.MakeActualSizeAtLeastExpandOnTop(this->size);
	for (int i=0;i<this->size;i++)
	{ roots tempRoots;
		tempRoots.ComputeDebugString();
		for (int j=0;j<this->TheObjects[i].IndicesNonZeroMults.size;j++)
		{ root tempRoot, tempRoot3;
			tempRoot.AssignIntRoot(this->RootsToIndices.
					TheObjects[this->TheObjects[i].IndicesNonZeroMults.TheObjects[j]]);
			intRoot tempRoot2;
			tempRoot.DivByInteger(2);
			tempRoot2.AssignRoot(tempRoot);
			tempRoot3.AssignIntRoot(tempRoot2);
			if (!tempRoot3.IsEqualTo(tempRoot))
				tempRoot.MultiplyByInteger(2);
			tempRoots.AddObjectOnTopNoRepetitionOfObject(tempRoot);
			tempRoots.ComputeDebugString();
		}
		if (output.AddObjectOnTopNoRepetitionOfObject(tempRoots))
		{ tempRoots.ComputeDebugString();
			outputString<< tempRoots.DebugString << "\n\n\n";
		}
	}
}

void partFractions::ComputeDebugStringBasisChange(MatrixIntTightMemoryFit& VarChange, GlobalVariables&  theGlobalVariables)
{ this->ElementToStringBasisChange( VarChange,true,
																		this->DebugString,
																		PolynomialOutputFormat::UsingLatexFormat,false,true, theGlobalVariables);
}

void partFractions::ComputeKostantFunctionFromWeylGroup
				(	char WeylGroupLetter, int WeylGroupNumber,
					QuasiPolynomial& output, root* ChamberIndicator, bool UseOldData,
					bool StoreToFile, GlobalVariables&  theGlobalVariables)
{	intRoots theBorel,theVPbasis;
	intRoot tempWeight;
	roots tempRoots;
	WeylGroup tempW;
	this->ClearTheObjects();
	this->AmbientDimension=(short) WeylGroupNumber;
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
		partFractions::ListBasicObjectsActualSizeIncrement=35000;
	}
	if (WeylGroupLetter=='A'||
			WeylGroupLetter=='B'||
			WeylGroupLetter=='C'||
			WeylGroupLetter=='D'||
			WeylGroupLetter=='F')
	{	for (int i=0;i<this->AmbientDimension;i++)
		{ tempWeight.elements[i]=MathRoutines::KToTheNth(8,this->AmbientDimension-i-1);
		}
	}
	else
	{	for (int i=0;i<this->AmbientDimension;i++)
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
			if (tempRat.IsEqualTo(LROne))
			{ intRoot tempIntRoot;
				tempIntRoot=theBorel.TheObjects[i];
				tempIntRoot.MultiplyByInteger(2);
				theVPbasis.AddObjectOnTop(tempIntRoot);
			}
		}
	}
	if (WeylGroupLetter=='D')
	{	intRoot tempRoot;
		tempRoot.MakeZero(this->AmbientDimension);
		tempRoot.elements[this->AmbientDimension-1]=1;
		tempRoot.elements[this->AmbientDimension-2]=-1;
		theVPbasis.AddObjectOnTop(tempRoot);
		tempRoot.elements[this->AmbientDimension-1]=1;
		tempRoot.elements[this->AmbientDimension-2]=1;
		theVPbasis.AddObjectOnTop(tempRoot);
		for(int i=this->AmbientDimension-3;i>=0;i--)
		{ tempRoot.elements[i]=2;
			theVPbasis.AddObjectOnBottom(tempRoot);
		}
		tempWeight.elements[this->AmbientDimension-2]=7;
		tempWeight.elements[this->AmbientDimension-1]=8;
	}
	theVPbasis.BubbleSort(&tempWeight);
	theVPbasis.ComputeDebugString();
	tempW.ComputeDebugString();
	theBorel.ComputeDebugString();
	this->initFromRootSystem(theBorel,theVPbasis,0, theGlobalVariables);
	if (ChamberIndicator==0)
	{ this->IndicatorRoot.Assign(tempW.rho);
	}
	else
	{ this->IndicatorRoot.Assign(*ChamberIndicator);
	}
	//this->flagSplitTestModeNoNumerators=true;
	this->split(theGlobalVariables);
	/*if (WeylGroupLetter=='A'||
			WeylGroupLetter=='B'||
			WeylGroupLetter=='C'||
			WeylGroupLetter=='D')
	{	this->splitClassicalRootSystem(true);
	}
	else
	{ if (WeylGroupLetter=='F')
		{ this->splitClassicalRootSystem(false);
			this->split();
		}
		else
			this->split();
	}*/
//	this->ComputeDebugString();
	assert(this->CheckForMinimalityDecompositionWithRespectToRoot(this->IndicatorRoot,theGlobalVariables));
	//return;
	if(!this->partFractionsToPartitionFunctionAdaptedToRoot
		(output,this->IndicatorRoot,StoreToFile,UseOldData,theGlobalVariables))
	{ this->ComputeDebugStringNoNumerator(theGlobalVariables);
		assert(false);
	}
	//output.ComputeDebugString();
}

void oneFracWithMultiplicitiesAndElongations::operator =(oneFracWithMultiplicitiesAndElongations& right)
{ this->Multiplicities.CopyFromLight(right.Multiplicities);
	this->Elongations.CopyFromLight(right.Elongations);
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

void oneFracWithMultiplicitiesAndElongations::GetPolyDenominator
	(IntegerPoly& output, int MultiplicityIndex, intRoot& theExponent)
{ assert(MultiplicityIndex<this->Multiplicities.size);
	Monomial<Integer> tempM;
	output.MakeNVarConst((short)theExponent.dimension,IOne);
	tempM.init((short)theExponent.dimension);
	tempM.Coefficient.Assign(IMOne);
	for (int i=0;i<theExponent.dimension;i++)
		tempM.degrees[i]=(short)(theExponent.elements[i]*this->Elongations.TheObjects[MultiplicityIndex]);
	output.AddMonomial(tempM);
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
		result=MathRoutines::lcm(this->Elongations.TheObjects[i],result);
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
{ this->Elongations.SetSizeExpandOnTopLight(0);
	this->Multiplicities.SetSizeExpandOnTopLight(0);
}

void oneFracWithMultiplicitiesAndElongations::ComputeOneCheckSum(Rational& output, intRoot& theExp, int theDimension)
{ output.Assign(LROne);
	std::string tempS;
	for (int i=0;i<this->Elongations.size;i++)
	{ Rational tempRat, tempRat2, tempRat3;
		tempRat.Assign(LROne);
		tempRat2.Assign(LROne);
		for (int j=0;j<theDimension;j++)
		{	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
			{ theExp.ElementToString(tempS);
			}
			tempRat3.Assign
				(oneFracWithMultiplicitiesAndElongations::CheckSumRoot.TheObjects[j]);
			if (!tempRat3.IsEqualToZero())
				tempRat3.RaiseToPower(theExp.elements[j]*this->Elongations.TheObjects[i]);
			tempRat2.MultiplyBy(tempRat3);
			if (partFraction::flagAnErrorHasOccurredTimeToPanic)
			{ tempRat2.ElementToString(tempS);
			}
		}
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ tempRat.ElementToString(tempS);
		}
		tempRat.Subtract(tempRat2);
		tempRat.RaiseToPower(this->Multiplicities.TheObjects[i]);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ tempRat.ElementToString(tempS);
		}
		output.MultiplyBy(tempRat);
		if (partFraction::flagAnErrorHasOccurredTimeToPanic)
		{ output.ElementToString(tempS);
		}
	}
	if (output.IsEqualToZero())
	{ std::string tempS;
		Stop();
	}
	output.Invert();
	if (partFraction::flagAnErrorHasOccurredTimeToPanic)
	{ output.ElementToString(tempS);
	}
}

int oneFracWithMultiplicitiesAndElongations::GetMultiplicityLargestElongation()
{ int result=0;
	int LargestElongationFound=0;
	for (int i=0;i<this->Elongations.size;i++)
	{ if (LargestElongationFound<this->Elongations.TheObjects[i])
		{ LargestElongationFound= this->Elongations.TheObjects[i];
			result= this->Multiplicities.TheObjects[i];
		}
	}
	return result;
}

void oneFracWithMultiplicitiesAndElongations::AddMultiplicity
	(int MultiplicityIncrement, int Elongation)
{ if (MultiplicityIncrement==0) return;
	int ElongationIndex=-1;
	for (int i=0;i<this->Elongations.size;i++)
	{ if (this->Elongations.TheObjects[i]==Elongation)
		{ ElongationIndex=i;
			break;
		}
	}
	if (ElongationIndex==-1)
	{ this->Elongations.AddObjectOnTopLight(Elongation);
		int tempI=0;
		this->Multiplicities.AddObjectOnTopLight(tempI);
		ElongationIndex= this->Multiplicities.size-1;
	}
	this->Multiplicities.TheObjects[ElongationIndex]+=MultiplicityIncrement;
	assert(this->Multiplicities.TheObjects[ElongationIndex]>=0);
	if (this->Multiplicities.TheObjects[ElongationIndex]==0)
	{ this->Multiplicities.PopIndexSwapWithLastLight(ElongationIndex);
		this->Elongations.PopIndexSwapWithLastLight(ElongationIndex);
	}
}

void oneFracWithMultiplicitiesAndElongations::OneFracToStringBasisChange
	(	partFractions& owner, int indexElongation, MatrixIntTightMemoryFit& VarChange, bool UsingVarChange,
		std::string& output, bool LatexFormat, int indexInFraction, int theDimension)
{ std::stringstream  out;
	std::string tempS;
	intRoot tempRoot2, tempRoot;
	int NumCoords;
	if (UsingVarChange)
	{	NumCoords= VarChange.NumRows;
		tempRoot2=owner.RootsToIndices.TheObjects[indexInFraction];
		for (int i=0;i<NumCoords;i++)
		{ tempRoot.elements[i]=0;
			for (int j=0;j<theDimension;j++)
			{ tempRoot.elements[i]+=VarChange.elements[i][j]*tempRoot2.elements[j];
			}
		}
	}
	else
	{ NumCoords=theDimension;
		tempRoot = owner.RootsToIndices.TheObjects[indexInFraction];
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
			( partFractions& owner, MatrixIntTightMemoryFit& VarChange,
				bool UsingVarChange, std::string& output,
				bool LatexFormat, int index, int theDimension)
{	if (this->Multiplicities.size==0)
	{	output.clear(); return;
	}
	std::string tempS;
	std::stringstream out;
	for (int k=0;k<this->Multiplicities.size;k++)
	{ this->OneFracToStringBasisChange(owner,k,VarChange,UsingVarChange,tempS,LatexFormat,index,theDimension);
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
			if (this->Elongations.TheObjects[i]==right.Elongations.TheObjects[j])
			{ if(this->Multiplicities.TheObjects[i]!=right.Multiplicities.TheObjects[j])
					return false;
				else
				{ Found=true; break;
				}
			}
		if (!Found)
			return false;
	}
	return true;
}

void intRoot::MakeZero(int theDimension)
{ this->dimension=theDimension;
	for (int i=0;i<this->dimension;i++)
		this->elements[i]=0;
}

void intRoot::AddRoot(intRoot& theRoot)
{ for (int i=0;i<this->dimension;i++)
		this->elements[i]+=theRoot.elements[i];
}

int intRoot::HashFunction()
{	int result=0;
	for (int i=0;i<this->dimension;i++)
		result+=::SomeRandomPrimes[i]*this->elements[i];
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
{ this->dimension= r.size;
	for (int i=0;i<this->dimension;i++)
		this->elements[i]= r.TheObjects[i].NumShort;
}

bool intRoot::IsPositive()
{ for (int i=0;i<this->dimension;i++)
		if (this->elements[i]<0)
			return false;
	return true;
}

void intRoot::initFromInt(int theDimension, int x1, int x2, int x3, int x4, int x5)
{ this->dimension= theDimension;
	this->elements[0]=x1;
	this->elements[1]=x2;
	this->elements[2]=x3;
	this->elements[3]=x4;
	this->elements[4]=x5;
}

void intRoot::initFromInt(int theDimension, int x1,int x2,int x3, int x4, int x5, int x6,
														int x7,int x8,int x9, int x10, int x11, int x12)
{ this->dimension=theDimension;
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
		if (r.elements[i]>this->elements[i])
			return false;
	return true;
}

inline bool intRoot::IsHigherThanWRTWeight(intRoot& r, intRoot& theWeights)
{	assert(this->dimension == r.dimension);
	int accum=0;
	for (int i=0;i<this->dimension;i++)
		accum+=(this->elements[i]-r.elements[i])*theWeights.elements[i];
	return (accum>0);
}

void intRoot::operator =(const intRoot &right)
{ this->dimension=right.dimension;
	for (int i=0;i<this->dimension;i++)
		this->elements[i]=right.elements[i];
}

bool intRoot::operator ==(intRoot &right)
{	if (this->dimension!=right.dimension)
		return false;
	for (int i=0;i<this->dimension;i++)
		if (this->elements[i]!=right.elements[i])
			return false;
	return true;
}

/*bool intRoot::operator ==(int* right)
{	for (int i=0;i<this->dimension;i++)
	{ if (this->elements[i]!=right[i]){return false;}
	}
	return true;
}*/

void RootToIndexTable::initFromRoots(intRoots& theAlgorithmBasis, intRoot* theWeights)
{ if (theAlgorithmBasis.size==0)
		return;
	int theDimension= theAlgorithmBasis.TheObjects[0].dimension;
	if (theWeights!=0) {this->weights=*theWeights;}
	for (int i=0;i<theAlgorithmBasis.size;i++)
	{ int x= this->ContainsObjectHash(theAlgorithmBasis.TheObjects[i]);
		if (x==-1)
		{	if (theWeights!=0)
				this->AddRootAndSort(theAlgorithmBasis.TheObjects[i]);
			else
				this->AddRootPreserveOrder(theAlgorithmBasis.TheObjects[i]);
		}
	}
	this->NumNonRedundantShortRoots= this->size;
	this->ComputeTable(theDimension);
}

void RootToIndexTable::ComputeTable(int theDimension)
{ intRoot tempR, tempR2, tempR3;
	tempR.dimension=theDimension; tempR2.dimension= theDimension;
	tempR3.dimension= theDimension;
	this->IndicesRedundantShortRoots.init(this->size);
	for (int i=0;i<this->size;i++)
	{ for (int j=0;j<this->size;j++)
		{ for (int k=0;k<theDimension;k++)
			{ tempR.elements[k] = this->TheObjects[i].elements[k]-
														this->TheObjects[j].elements[k];
				tempR2.elements[k] = this->TheObjects[i].elements[k]-
														 2*this->TheObjects[j].elements[k];
			}
			this->TableAllowedAminusB[i][j]  = this->getIndex(tempR);
			this->TableAllowedAminus2B[i][j] = this->getIndex(tempR2);
		}
		tempR3=this->TheObjects[i];
		tempR3.MultiplyByInteger(2);
		this->IndicesDoublesOfRedundantShortRoots[i] = this->getIndex(tempR3);
		if (IndicesDoublesOfRedundantShortRoots[i]!=-1)
			this->IndicesRedundantShortRoots.AddSelectionAppendNewIndex(i);
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
		if (theRoot.IsHigherThanWRTWeight(tempList.TheObjects[index],this->weights))
			break;
	tempList.ShiftUpExpandOnTop(index);
	tempList.TheObjects[index]= theRoot;
	this->ClearTheObjects();
	for (int i=0;i<tempList.size;i++)
		this->AddObjectOnTopHash(tempList.TheObjects[i]);
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

int RootToIndexTable::getIndexDoubleOfARoot(intRoot& TheRoot)
{ intRoot tempRoot;
	tempRoot=TheRoot;
	tempRoot.MultiplyByInteger(2);
	return this->getIndex(tempRoot);
}

void ::SelectionWithMultiplicities::initMe(int NumElements)
{	this->Multiplicities.SetSizeExpandOnTopNoObjectInit(NumElements);
	for (int i=0;i<this->Multiplicities.size;i++)
		this->Multiplicities.TheObjects[i]=0;
	this->elements.MakeActualSizeAtLeastExpandOnTop(NumElements);
	this->elements.size=0;
}

void SelectionWithMultiplicities::ElementToString(std::string& output)
{ std::stringstream out;
	for (int i=0;i<this->elements.size;i++)
		out << "Index: " << this->elements.TheObjects[i] << "\nMultiplicity: "
				<<this->Multiplicities.TheObjects[this->elements.TheObjects[i]];
	output= out.str();
}

void SelectionWithMaxMultiplicity::initMe2(int NumElements, int MaxMult)
{ this->initMe(NumElements);
	this->MaxMultiplicity=MaxMult;
}

void SelectionWithMaxMultiplicity::IncrementSubset()
{ for (int i=this->Multiplicities.size-1;i>=0;i--)
	{ if (this->Multiplicities.TheObjects[i]<this->MaxMultiplicity)
		{ if (this->Multiplicities.TheObjects[i]==0)
				this->elements.AddObjectOnTop(i);
			this->Multiplicities.TheObjects[i]++;
			return;
		}
		else
		{ this->Multiplicities.TheObjects[i]=0;
			this->elements.PopFirstOccurenceObjectSwapWithLast(i);
		}
	}
}

void SelectionWithMultiplicities::ComputeElements()
{ this->elements.size=0;
	for (int i=0;i<this->Multiplicities.size;i++)
		if (this->Multiplicities.TheObjects[i]>0)
			this->elements.AddObjectOnTop(i);
}

int SelectionWithMultiplicities::CardinalitySelectionWithoutMultiplicities()
{ return this->Multiplicities.size;
}

int ::SelectionWithDifferentMaxMultiplicities::getTotalNumSubsets()
{ int result=1;
	for (int i=0;i<this->MaxMultiplicities.size;i++)
		result*=(this->MaxMultiplicities.TheObjects[i]+1);
	assert(result>=0);
	return result;
}

int ::SelectionWithDifferentMaxMultiplicities::TotalMultiplicity()
{ int result=0;
	for (int i=0;i<this->Multiplicities.size;i++)
		result+=this->Multiplicities.TheObjects[i];
	return result;
}

int ::SelectionWithDifferentMaxMultiplicities::MaxTotalMultiplicity()
{ int result=0;
	for (int i=0;i<this->Multiplicities.size;i++)
		result+=this->MaxMultiplicities.TheObjects[i];
	return result;
}

void ::SelectionWithDifferentMaxMultiplicities::clearNoMaxMultiplicitiesChange()
{ for (int i=0;i<this->Multiplicities.size;i++)
		this->Multiplicities.TheObjects[i]=0;
}

void SelectionWithDifferentMaxMultiplicities::IncrementSubset()
{	for (int i=this->Multiplicities.size-1;i>=0;i--)
	{ if (this->Multiplicities.TheObjects[i]<this->MaxMultiplicities.TheObjects[i])
		{ if (this->Multiplicities.TheObjects[i]==0)
				this->elements.AddObjectOnTop(i);
			this->Multiplicities.TheObjects[i]++;
			return;
		}
		else
		{ this->Multiplicities.TheObjects[i]=0;
			this->elements.PopFirstOccurenceObjectSwapWithLast(i);
		}
	}
}

void WeylGroup::ReflectBetaWRTAlpha(root& alpha, root &beta, bool RhoAction, root& output)
{ static Rational alphaShift, tempRat;
	output.SetSizeExpandOnTopLight(this->KillingFormMatrix.NumCols);
	alphaShift.MakeZero();
	Rational lengthA;
	lengthA.MakeZero();
	if (RhoAction)
		beta.Add(this->rho);
	for (int i=0;i<this->KillingFormMatrix.NumRows;i++)
		for (int j=0;j<this->KillingFormMatrix.NumCols;j++)
		{ tempRat.Assign(beta.TheObjects[j]);
			tempRat.MultiplyBy(alpha.TheObjects[i]);
			tempRat.MultiplyByInt(-this->KillingFormMatrix.elements[i][j]*2);
			alphaShift.Add(tempRat);
			tempRat.Assign(alpha.TheObjects[i]);
			tempRat.MultiplyBy(alpha.TheObjects[j]);
			tempRat.MultiplyByInt(this->KillingFormMatrix.elements[i][j]);
			lengthA.Add(tempRat);
		}
	alphaShift.DivideBy(lengthA);
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ tempRat.Assign(alphaShift);
		tempRat.MultiplyBy(alpha.TheObjects[i]);
		tempRat.Add(beta.TheObjects[i]);
		output.TheObjects[i].Assign(tempRat);
	}
	if (RhoAction)
	{ beta.Subtract(this->rho);
		output.Subtract(this->rho);
	}
}

void WeylGroup::SimpleReflectionDualSpace(int index, root& DualSpaceElement)
{	static Rational coefficient, tempRat;
	coefficient.Assign(DualSpaceElement.TheObjects[index]);
	coefficient.DivideByInteger(this->KillingFormMatrix.elements[index][index]);
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ tempRat.Assign(coefficient);
		tempRat.MultiplyByInt(-this->KillingFormMatrix.elements[index][i]*2);
		DualSpaceElement.TheObjects[i].Add(tempRat);
	}
}

void WeylGroup::SimpleReflectionRoot
	(int index, root& theRoot,bool RhoAction, bool UseMinusRho)
{ static Rational alphaShift, tempRat;
	alphaShift.MakeZero();
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ tempRat.Assign(theRoot.TheObjects[i]);
		tempRat.MultiplyByInt(-this->KillingFormMatrix.elements[index][i]*2);
		alphaShift.Add(tempRat);
	}
	if (this->flagAnErrorHasOcurredTimeToPanic)
	{ std::string tempS;
		alphaShift.ElementToString(tempS);
	}
	alphaShift.DivideByInteger(this->KillingFormMatrix.elements[index][index]);
	if (RhoAction)
	{	if(UseMinusRho)
			alphaShift.AddInteger(1);
		else
			alphaShift.AddInteger(-1);
	}
	theRoot.TheObjects[index].Add(alphaShift);
}

void WeylGroup::SimpleReflectionRootAlg
					(	int index, PolynomialsRationalCoeff& theRoot,
						bool RhoAction)
{	int lengthA=this->KillingFormMatrix.elements[index][index];
	static PolynomialRationalCoeff AscalarB, tempP;
	AscalarB.Nullify((short)this->KillingFormMatrix.NumRows);
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ tempP.Nullify((short)this->KillingFormMatrix.NumRows);
		tempP.CopyFromPoly(theRoot.TheObjects[i]);
		tempP.TimesInteger(KillingFormMatrix.elements[index][i]);
		AscalarB.AddPolynomial(tempP);
	}
	AscalarB.TimesInteger(-2);
	AscalarB.DivByInteger(lengthA);
	theRoot.TheObjects[index].AddPolynomial(AscalarB);
	if (RhoAction){theRoot.TheObjects[index].AddConstant(RMOne);}
}

void WeylGroup::ActOnAffineHyperplaneByGroupElement
	(int index, affineHyperplane& output, bool RhoAction, bool UseMinusRho)
{ int tempI= this->TheObjects[index].size;
	for (int i=0; i<tempI;i++)
	{	this->SimpleReflectionRoot
			(this->TheObjects[index].TheObjects[i],output.affinePoint,RhoAction, UseMinusRho);
//		output.affinePoint.ComputeDebugString();
		this->SimpleReflectionDualSpace
			(this->TheObjects[index].TheObjects[tempI-i-1],output.normal);
	}
}

void WeylGroup::ActOnRootByGroupElement(	int index,
																						root &theRoot, bool RhoAction, bool UseMinusRho)
{ for (int i=0;i<this->TheObjects[index].size;i++)
		this->SimpleReflectionRoot(this->TheObjects[index].TheObjects[i],theRoot,RhoAction, UseMinusRho);
}

void WeylGroup::GenerateRootSystemFromKillingFormMatrix()
{ root tempRoot;
	roots startRoots;
	this->RootSystem.ClearTheObjects();
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{	tempRoot.MakeZero(this->KillingFormMatrix.NumRows);
		tempRoot.TheObjects[i].AssignInteger(1);
		startRoots.AddRoot(tempRoot);
	}
	this->GenerateOrbit(startRoots,false,this->RootSystem,false);
	this->RootsOfBorel.size=0;
	for (int i=0;i<this->RootSystem.size;i++)
	{ if (this->RootSystem.TheObjects[i].IsPositiveOrZero())
		{ this->RootsOfBorel.AddRoot(this->RootSystem.TheObjects[i]);
		}
	}
}

void WeylGroup::GenerateOrbit(roots& theRoots, bool RhoAction,
															hashedRoots& output, bool UseMinusRho)
{ static WeylGroup tempW;
	this->GenerateOrbit(theRoots,RhoAction,output,false,tempW, UseMinusRho);
}

void WeylGroup::GenerateOrbit( roots& theRoots, bool RhoAction,
															 hashedRoots& output,
															 bool ComputingAnOrbitGeneratingSubsetOfTheGroup,
															 WeylGroup& outputSubset, bool UseMinusRho)
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
			if (this->flagAnErrorHasOcurredTimeToPanic)
			{ currentRoot.ComputeDebugString();
			}
			this->SimpleReflectionRoot(j,currentRoot,RhoAction, UseMinusRho);
			if (this->flagAnErrorHasOcurredTimeToPanic)
			{ currentRoot.ComputeDebugString();
			}
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
			tempRat.Assign(r1.TheObjects[i]);
			tempRat.MultiplyBy(r2.TheObjects[j]);
			tempRat.MultiplyByInt(this->KillingFormMatrix.elements[i][j]);
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
											true,OrbitGeneratingSubset,false);
	TheIndicatorsOrbit.ComputeDebugString();
	roots TempTest;
	root tempRoot;
	for (int i=0;i<OrbitGeneratingSubset.size;i++)
	{ tempRoot= ChamberIndicator;
		OrbitGeneratingSubset.ActOnRootByGroupElement(i,tempRoot,RhoAction,false);
		TempTest.AddRoot(tempRoot);
	}
	TempTest.ComputeDebugString();
	static PolynomialsRationalCoeff theRoot;
	PolyFormatLocal.MakeAlphabetxi();
	output.size=0;
	output.ChamberIndicators.size=0;
	output.MakeActualSizeAtLeastExpandOnTop(OrbitGeneratingSubset.size);
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
	output.ComputeDubugString(this->KillingFormMatrix.NumRows);
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

void WeylGroup::MakeArbitrary(char WeylGroupLetter,int n)
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

void WeylGroup::MakeDn(int n)
{ this->MakeAn(n);
	this->KillingFormMatrix.elements[n-1][n-2]=0;
	this->KillingFormMatrix.elements[n-2][n-1]=0;
	this->KillingFormMatrix.elements[n-3][n-1]=-1;
	this->KillingFormMatrix.elements[n-1][n-3]=-1;
}

void WeylGroup::MakeAn(int n)
{	this->rho.SetSizeExpandOnTopLight(n);
	this->KillingFormMatrix.init(n,n);
	this->KillingFormMatrix.NullifyAll();
	for (int i=0;i<n-1;i++)
	{ this->KillingFormMatrix.elements[i][i]=2;
		this->KillingFormMatrix.elements[i+1][i]=-1;
		this->KillingFormMatrix.elements[i][i+1]=-1;
	}
	this->KillingFormMatrix.elements[n-1][n-1]=2;
}

void WeylGroup::MakeEn(int n)
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
{	this->rho.SetSizeExpandOnTopLight(4);
	this->KillingFormMatrix.init(4,4);
	this->KillingFormMatrix.elements[0][0]=2 ;this->KillingFormMatrix.elements[0][1]=-1;this->KillingFormMatrix.elements[0][2]=0 ;this->KillingFormMatrix.elements[0][3]=0 ;
	this->KillingFormMatrix.elements[1][0]=-1;this->KillingFormMatrix.elements[1][1]=2 ;this->KillingFormMatrix.elements[1][2]=-2;this->KillingFormMatrix.elements[1][3]=0 ;
	this->KillingFormMatrix.elements[2][0]=0 ;this->KillingFormMatrix.elements[2][1]=-2;this->KillingFormMatrix.elements[2][2]=4 ;this->KillingFormMatrix.elements[2][3]=-2;
	this->KillingFormMatrix.elements[3][0]=0 ;this->KillingFormMatrix.elements[3][1]=0 ;this->KillingFormMatrix.elements[3][2]=-2;this->KillingFormMatrix.elements[3][3]=4 ;
}

void WeylGroup::MakeG2()
{	this->rho.SetSizeExpandOnTopLight(2);
	this->KillingFormMatrix.init(2,2);
	this->KillingFormMatrix.elements[0][0]=6;
	this->KillingFormMatrix.elements[1][1]=2;
	this->KillingFormMatrix.elements[1][0]=-3;
	this->KillingFormMatrix.elements[0][1]=-3;
}

void WeylGroup::MakeBn(int n)
{ this->MakeAn(n);
	this->KillingFormMatrix.elements[n-1][n-1]=1;
}

void WeylGroup::MakeCn(int n)
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
	this->GenerateOrbit(tempRoots,false,tempRoots2,true,*this,false);
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
	this->rho.MakeZero(this->KillingFormMatrix.NumRows);
	for (int i=0;i<this->RootSystem.size;i++)
	{	if (RootSystem.TheObjects[i].IsPositiveOrZero() )
		{ this->rho.Add(RootSystem.TheObjects[i]);
		}
	}
	for (int i=0;i<this->KillingFormMatrix.NumCols;i++)
	{ this->rho.TheObjects[i].DivideByInteger(2);
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

void PolynomialsRationalCoeff::MakeUsualParametricRoot(int theDimension)
{ PolynomialRationalCoeff tempP;
	for (int i=0;i<theDimension;i++)
	{ tempP.MakeNVarDegOnePoly((short)theDimension,i,ROne);
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
	this->SetSizeExpandOnTopNoObjectInit(direction.size);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].Nullify(1);
		tempM.Coefficient.Assign(direction.TheObjects[i]);
		this->TheObjects[i].AddMonomial(tempM);
	}
}

void PolynomialsRationalCoeff::MakeOneParameterSubFromDirectionInts
				(int x1,int x2,int x3, int x4, int x5)
{	root tempRoot;
	tempRoot.InitFromIntegers(5,x1,x2,x3,x4,x5);
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

void PolynomialsRationalCoeff::MakeSubFromMatrixIntAndDen(MatrixIntTightMemoryFit &theMat, int Den)
{ MatrixLargeRational tempMat;
	tempMat.AssignMatrixIntWithDen(theMat,Den);
	this->MakeSubFromMatrixRational(tempMat);
}

void PolynomialsRationalCoeff::MakeSubFromMatrixRational(MatrixLargeRational &theMat)
{ this->SetSizeExpandOnTopNoObjectInit(theMat.NumCols);
	for (int i=0;i<this->size;i++)
	{ static Monomial<Rational> tempM;
		this->TheObjects[i].Nullify((short)theMat.NumRows-1);
		for (int j=0;j<theMat.NumRows-1;j++)
		{	tempM.init((short)theMat.NumRows-1);
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

void PolynomialsRationalCoeff::ComputeB(PolynomialRationalCoeff& output, int cutOffIndex, int theDimension)
{ static PolynomialsRationalCoeff EpsForm;
	PolynomialRationalCoeff tempP;
	EpsForm.SetSizeExpandOnTopNoObjectInit(theDimension);
	EpsForm.TheObjects[0].CopyFromPoly(this->TheObjects[0]);
	for (int i=1;i<theDimension;i++)
	{ EpsForm.TheObjects[i].CopyFromPoly(this->TheObjects[i]);
		tempP.CopyFromPoly(this->TheObjects[i-1]);
		tempP.TimesConstant(RMOne);
		EpsForm.TheObjects[i].AddPolynomial(tempP);
	}
	EpsForm.TheObjects[3].AddPolynomial(this->TheObjects[4]);
	for (int j=cutOffIndex+1;j<theDimension;j++)
	{	if (!EpsForm.TheObjects[j].IsGreaterThanZeroLexicographicOrder()
				)
		{ EpsForm.TheObjects[j].TimesConstant(RMOne);
		}
	}
	output.Nullify(2);
	for (int i=0;i<theDimension;i++)
	{ output.AddPolynomial(EpsForm.TheObjects[i]);
	}
}

void PolynomialsRationalCoeffCollection::ElementToString(std::string &output, int theDimension)
{ this->ElementToStringComputeFunctionB(output,false, theDimension);
}

void PolynomialsRationalCoeffCollection::ElementToStringComputeFunctionB
					(std::string &output, bool computingB, int theDimension)
{ std::string tempS;
	std::stringstream  out;
	out<<"Number of elements: "<<this->size<<"\n";
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		out<< tempS;
		if (computingB)
		{ static PolynomialRationalCoeff tempP;
			this->TheObjects[i].ComputeB(tempP, 2, theDimension);
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

void PolynomialsRationalCoeffCollection::ComputeDubugString(int theDimension)
{ this->ElementToString(this->DebugString, theDimension);
}

void VermaModulesWithMultiplicities::WriteKLCoeffsToFile(std::fstream& output, ListBasicObjects<int>& KLcoeff,int TopIndex)
{ output.clear();
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
{ int theDimension= this->TheWeylGroup->KillingFormMatrix.NumRows;
	root ZeroRoot; ZeroRoot.MakeZero(theDimension);
	this->BruhatOrder.SetSizeExpandOnTopNoObjectInit(this->size);
	this->InverseBruhatOrder.SetSizeExpandOnTopNoObjectInit(this->size);
	this->SimpleReflectionsActionList.SetSizeExpandOnTopNoObjectInit(this->size);
	this->ComputeDebugString();
	for (int i=0;i<this->size;i++)
	{ this->SimpleReflectionsActionList.TheObjects[i].
			MakeActualSizeAtLeastExpandOnTop(theDimension);
		for (int j=0;j<theDimension;j++)
		{ root tempRoot,tempRoot2;
			tempRoot.Assign(this->TheObjects[i]);
			tempRoot2.Assign(this->TheObjects[i]);
			this->TheWeylGroup->SimpleReflectionRoot(j,tempRoot,true,false);
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
{	int theDimension= this->TheWeylGroup->KillingFormMatrix.NumRows;
	root ChamberIndicatorPlusRho;
	ChamberIndicatorPlusRho.Assign(ChamberIndicator);
	ChamberIndicatorPlusRho.Add(this->TheWeylGroup->rho);
	for (int i=0;i<this->size;i++)
	{	Rational tempRat1, tempRat2;
		bool tempBool1, tempBool2;
		bool haveSameSigns=true;
		for (int j=0;j<this->TheWeylGroup->RootSystem.size;j++)
		{ this->TheWeylGroup->RootScalarKillingFormMatrixRoot
				(ChamberIndicatorPlusRho, this->TheWeylGroup->RootSystem.TheObjects[j],tempRat1);
			static root tempRoot; tempRoot.SetSizeExpandOnTopLight(theDimension);
			tempRoot.Assign(this->TheObjects[i]);
			tempRoot.Add(this->TheWeylGroup->rho);
			this->TheWeylGroup->RootScalarKillingFormMatrixRoot
				(tempRoot, this->TheWeylGroup->RootSystem.TheObjects[j],tempRat2);
			tempBool1=tempRat1.IsPositive();
			tempBool2=tempRat2.IsPositive();
			assert(!tempRat1.IsEqualToZero());
			assert(!tempRat2.IsEqualToZero());
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
			output.TheObjects[i]*=-1;
	}
}

void VermaModulesWithMultiplicities::initFromWeyl(WeylGroup* theWeylGroup)
{ this->TheWeylGroup= theWeylGroup;
	roots tempRoots;
	tempRoots.AddRoot(this->TheWeylGroup->rho);
	this->TheWeylGroup->GenerateOrbit(tempRoots,true,*this,false);
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
{	int theDimension= this->TheWeylGroup->KillingFormMatrix.NumRows;
	this->RPolys.SetSizeExpandOnTopNoObjectInit(this->size);
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
			for (int j=0;j<theDimension;j++)
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
	MatrixLargeRational tempMat;
	tempMat.AssignMatrixIntWithDen(tempW.KillingFormMatrix,1);
	tempMat.ComputeDebugString();
	Rational tempRat;
	tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
	tempMat.ComputeDebugString();
}

void RandomCodeIDontWantToDelete::SomeRandomTests5()
{
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
{
}

void RandomCodeIDontWantToDelete::RevealTheEvilConspiracy()
{
}

void RandomCodeIDontWantToDelete::SomeRandomTests2()
{ Rational tempRat;
	std::string tempS;
	tempRat.AssignNumeratorAndDenominator(10,2);
	tempRat.AssignFracValue();
	tempRat.ElementToString(tempS);
	tempRat.AssignNumeratorAndDenominator(-10,2);
	tempRat.AssignFracValue();
	tempRat.ElementToString(tempS);
	tempRat.AssignNumeratorAndDenominator(-12,7);
	tempRat.AssignFracValue();
	tempRat.ElementToString(tempS);
	tempRat.AssignNumeratorAndDenominator(1,2);
	tempRat.AssignFracValue();
	tempRat.ElementToString(tempS);
	tempRat.AssignNumeratorAndDenominator(0,2);
	tempRat.AssignFracValue();
	tempRat.ElementToString(tempS);
	tempRat.AssignNumeratorAndDenominator(-9,14);
	//Rational::flagAnErrorHasOccurredTimeToPanic=true;
	/*tempRat.RaiseToPower(100);
	tempRat.ElementToString(tempS);
	tempRat.MakeOne();
	for (int i=0;i<200;i++)
	{ Rational tempR;
		tempR.AssignNumeratorAndDenominator(1,i+1);
		tempRat.MultiplyBy(tempR);
	}
	tempRat.ElementToString(tempS);
	Rational tempRat2;
	tempRat2.MakeOne();
	for (int i=0;i<201;i++)
	{ Rational tempR;
		tempR.AssignNumeratorAndDenominator(1,i+1);
		tempRat2.MultiplyBy(tempR);
	}
	tempRat2.ElementToString(tempS);
	tempRat2.Subtract(tempRat);
	tempRat2.ElementToString(tempS);
	Stop();*/
	affineCone tempCone;
	MatrixLargeRational tempA, tempb, output;
	tempA.init(5,2);
	tempb.init(5,1);
	MatrixLargeRational::MatrixElementSeparator=",\t";
	tempA.elements[0][0].AssignInteger( 0); tempA.elements[0][1].AssignInteger(-1); tempb.elements[0][0].AssignInteger(-2);
	tempA.elements[1][0].AssignInteger(1);	tempA.elements[1][1].AssignInteger(-1); tempb.elements[1][0].AssignInteger(-1);
	tempA.elements[2][0].AssignInteger(1);	tempA.elements[2][1].AssignInteger( 0); tempb.elements[2][0].AssignInteger(2);
	tempA.elements[3][0].AssignInteger(-1); tempA.elements[3][1].AssignInteger( 0); tempb.elements[3][0].AssignInteger(0);
	tempA.elements[4][0].AssignInteger(1);	tempA.elements[4][1].AssignInteger( 1); tempb.elements[4][0].AssignInteger(2);
	tempA.ComputeDebugString();
	tempb.ComputeDebugString();
	output.ComputeDebugString();
//	tempCone.SystemLinearInequalitiesHasSolution(tempA,tempb,output);
	PolynomialRationalCoeff tempP1,tempP2,tempP3,tempP4;
	tempP1.DivideBy(tempP2, tempP3, tempP4);
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
{	//	int j=0;
	intRoot tempRoot;
	tempRoot.initFromInt(5,0,0,1,0,0);//eps_3-\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,1,1,0,0);//\eps_2-\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,1,1,0);//eps_3-\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,1,0,1);//eps_3+\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,1,1,1,0,0);//eps_1-\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,1,1,1,0);//eps_2-\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,1,1,0,1);//eps_2+\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,1,1,1);//eps_3+\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,1,1,1,1,0);//eps_1-\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,1,1,1,0,1);//eps_1+\eps_5
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,1,1,1,1);//eps_2+\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,1,1,1,1,1);//eps_1+\eps_4
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,1,2,1,1);//eps_2+\eps_3
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,1,1,2,1,1);//eps_1+\eps_3
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,1,2,2,1,1);//eps_1+\eps_2
	this->nilradicalA2A1A1inD5.AddObjectOnTop(tempRoot);
	this->AlgorithmBasisA2A1A1inD5.CopyFromBase(this->nilradicalA2A1A1inD5);
	tempRoot.initFromInt(5,0,1,0, 0,0);//\eps_2-\eps_3
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,0, 1,0);//\eps_4-\eps_5
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,0, 0,1);//\eps_4+\eps_5
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,2,2, 1,1);//2\eps_2
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,2, 1,1);//2\eps_3
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,0, 1,1);//2\eps_4
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,0,0,0,-1,1);//2\eps_5
	this->AlgorithmBasisA2A1A1inD5.AddObjectOnTop(tempRoot);
	tempRoot.initFromInt(5,10000,1000,100,8,10);
	this->AlgorithmBasisA2A1A1inD5.BubbleSort(&tempRoot);
	this->AlgorithmBasisA2A1A1inD5.ComputeDebugString();
}

rootFKFTcomputation::rootFKFTcomputation()
{ this->OutputFile = "C:/math/output.txt";
	this->TheGlobalVariables= new GlobalVariables;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter++;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
}

rootFKFTcomputation::~rootFKFTcomputation()
{ delete this->TheGlobalVariables;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
}

void rootFKFTcomputation::RunA2A1A1inD5beta12221()
{	this->initA2A1A1inD5();
//	RandomCodeIDontWantToDelete::SomeRandomTests2();
	if (this->useOutputFileForFinalAnswer)
	{ RandomCodeIDontWantToDelete::SomeRandomTestsIWroteMonthsAgo();
		std::fstream tempFile;
		tempFile.open(this->OutputFile.c_str(),std::fstream::in | std::fstream::out | std::fstream::app);
		QuasiPolynomial tempQP;
		tempQP.ReadFromFile(tempFile,5);
		tempFile.close();
		this->processA2A1A1inD5beta12221Answer(tempQP);
		return;
	}
	std::fstream KLDump;
	std::fstream PartialFractionsFile;
	bool KLDumpIsPresent =this->OpenDataFileOrCreateIfNotPresent(KLDump,this->KLCoeffFileString, false,false);
  bool PFfileIsPresent=this->OpenDataFileOrCreateIfNotPresent
		(PartialFractionsFile,this->PartialFractionsFileString,false,false);
  bool VPIndexIsPresent= this->OpenDataFileOrCreateIfNotPresent
		(partFractions::ComputedContributionsList,this->VPIndexFileString,false,false);
	this->OpenDataFileOrCreateIfNotPresent(partFraction::TheBigDump,this->VPEntriesFileString,true,false);
	assert(partFraction::TheBigDump.is_open());
	assert(KLDump.is_open());
	assert(PartialFractionsFile.is_open());
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
	beta.InitFromIntegers(5,10,20,20,20,10);
	ListBasicObjects<int> KLcoeff;
//	int TopIndex=
  tempV.ChamberIndicatorToIndex(beta);
	if (KLDumpIsPresent)
	{ tempV.ReadKLCoeffsFromFile(KLDump,KLcoeff);
	}
	else
	{ tempV.ComputeKLcoefficientsFromChamberIndicator(beta,KLcoeff);
		tempV.KLPolys.ReleaseMemory();
		tempV.RPolys.ReleaseMemory();
		tempV.WriteKLCoeffsToFile(KLDump,KLcoeff,tempV.ChamberIndicatorToIndex(beta));
	}
	KLDump.flush();
	KLDump.close();
//	tempV.ComputeDebugString();
	std::string tempS;
	//tempV.KLcoeffsToString(KLcoeff,tempS);
//	tempV.ComputeDebugString();

//	theVPfunction.initFromRootSystem(tempRoots1,tempRoots1,0);
 /* partFractions tempTest;
  QuasiPolynomial tempQPtest;
  //tempTest.ComputeKostantFunctionFromWeylGroup('A',3,tempQPtest,0,false,false);
  this->OpenDataFileOrCreateIfNotPresent(PartialFractionsFile,this->PartialFractionsFileString,false);
	//tempTest.WriteToFile(PartialFractionsFile);
  tempTest.ReadFromFile(PartialFractionsFile);
  PartialFractionsFile.flush();
  PartialFractionsFile.close();
  tempTest.ComputeDebugString();
*/
	if(!PFfileIsPresent )
	{	theVPfunction.ComputeDebugString(*this->TheGlobalVariables);
		theVPfunction.IndicatorRoot.MakeZero(5);
		theVPfunction.initFromRootSystem(	this->nilradicalA2A1A1inD5,
																		this->AlgorithmBasisA2A1A1inD5, 0,*this->TheGlobalVariables);
		theVPfunction.splitClassicalRootSystem(true,*this->TheGlobalVariables);
		theVPfunction.WriteToFile(PartialFractionsFile,*this->TheGlobalVariables);
	}
	else
		theVPfunction.ReadFromFile(PartialFractionsFile,*this->TheGlobalVariables);
	PartialFractionsFile.flush();
	PartialFractionsFile.close();
//	theVPfunction.ComputeDebugString();
	for (int i=0;i<theVPfunction.size;i++)
		assert(theVPfunction.TheObjects[i].IndicesNonZeroMults.size==5);
	QuasiPolynomial tempQP;
	beta.MultiplyByInteger(100);
	beta.Add(D5.rho);
	roots tempRoots;
	tempRoots.AssignIntRoots(this->nilradicalA2A1A1inD5);
	if (!VPIndexIsPresent)
		theVPfunction.WriteToFileComputedContributions
			(theVPfunction.ComputedContributionsList,*this->TheGlobalVariables);
	this->MakeTheRootFKFTSum(beta, theVPfunction,KLcoeff,	tempQP,
													 tempV,tempRoots);
	std::fstream tempFile;
	tempFile.open(this->OutputFile.c_str(),std::fstream::out | std::fstream::trunc);
	tempQP.WriteToFile(tempFile);
	tempFile.close();
	this->processA2A1A1inD5beta12221Answer(tempQP);
	PartialFractionsFile.close();
}

void rootFKFTcomputation::processA2A1A1inD5beta12221Answer(QuasiPolynomial& theAnswer)
{	theAnswer.ComputeDebugString();
	root beta;
	beta.InitFromIntegers(5,1,2,2,2,1);
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

bool rootFKFTcomputation::OpenDataFile
	(std::fstream& theFile, std::string& theFileName)
{ theFile.open(theFileName.c_str(),std::fstream::in|std::fstream::out);
  if(theFile.is_open())
  { theFile.clear(std::ios::goodbit);// false);
  	theFile.seekp(0,std::ios_base::end);
  	theFile.seekg(0);
		int tempI=theFile.tellp();
		if (tempI>=1)
			return true;
  }
	return false;
}

bool rootFKFTcomputation::OpenDataFileOrCreateIfNotPresent
	(std::fstream& theFile, std::string& theFileName, bool OpenInAppendMode, bool openAsBinary)
{ if (OpenInAppendMode)
	{	if (openAsBinary)
			theFile.open(theFileName.c_str(),std::fstream::in|std::fstream::out|std::fstream::app| std::fstream::binary);
		else
			theFile.open(theFileName.c_str(),std::fstream::in|std::fstream::out|std::fstream::app);
  } else
  { if (openAsBinary)
			theFile.open(theFileName.c_str(),std::fstream::in|std::fstream::out| std::fstream::binary);
		else
			theFile.open(theFileName.c_str(),std::fstream::in|std::fstream::out);
  }
  if(theFile.is_open())
  { theFile.clear(std::ios::goodbit);// false);
  	theFile.seekp(0,std::ios_base::end);
		int tempI=theFile.tellp();
		if (tempI>=1)
		{	return true;
		}
  }
	theFile.close();
	theFile.open(theFileName.c_str(),std::fstream::out |std::fstream::in| std::fstream::trunc);
	theFile.clear();
	return false;
}

void rootFKFTcomputation::MakeRootFKFTsub(root& direction, QPSub& theSub)
{	MatrixIntTightMemoryFit tempMat;
	tempMat.init(5+2,5);
	tempMat.NullifyAll();
	int tempLCM=direction.FindLCMDenominatorsTruncateToInt();
	for (int i=0;i<5;i++)
	{ tempMat.elements[i][i]=1;
		Rational tempRat;
		tempRat.Assign(direction.TheObjects[i]);
		tempRat.MultiplyByInt(tempLCM);
		tempMat.elements[5][i]=tempRat.NumShort;
		tempMat.elements[5+1][i]=0;
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
	//::IndicatorWindowGlobalVariables.TotalNumMonomials = theBVdecomposition.NumMonomialsInTheNumerators();

	Cone TheNilradicalCone;
	TheNilradicalCone.ComputeFromDirections(theNilradical,*this->TheGlobalVariables,5);
	TheNilradicalCone.ComputeDebugString();
	StartingRoot.MakeUsualParametricRoot(5);
	theHighestWeights.TheWeylGroup->GenerateOrbitAlg
		(	ChamberIndicator,StartingRoot,TheChambersInTheGame,true,false,&TheNilradicalCone,true);
	static QuasiPolynomial tempQP1, Accum;
	Accum.Nullify(5);
	for (int i=0;i<TheChambersInTheGame.size;i++)
	{	if (theKLCoeffs.TheObjects[i]!=0)
		{	tempQP1.Nullify(5);
			theBVdecomposition.partFractionsToPartitionFunctionAdaptedToRoot
				(tempQP1,TheChambersInTheGame.ChamberIndicators.TheObjects[i],true,true,
					*this->TheGlobalVariables);
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
				out2<<"-";
			else
			{ if (this->PolynomialPart.TheObjects[i]!=1)
					out2<<this->PolynomialPart.TheObjects[i];
			}
			if (i!=1)
			{	out2<<"q^{"<<i<<"}"; }else{out2<<"q";}
			tempS=out2.str();
			if (tempS[0]!='-')
				tempS.insert(0,"+");
			out<< tempS;
		}
	}
	output= out.str();
	if (output.size()>0)
	{ if (output[0]=='+')
			output.erase(0,1);
	}
}

int OneVarIntPolynomial::Substitution(int x)
{ assert(this->RationalPart.size==0);
	int result=0;
	for (int i=0;i<this->PolynomialPart.size;i++)
	{ int accum=1;
		for (int j=0;j<i;j++)
			accum*=x;
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
			sizeDecrease++;
		else
			break;
	}
	this->PolynomialPart.size-=sizeDecrease;
	sizeDecrease=0;
	for (int i=this->RationalPart.size-1;i>=0;i--)
	{ if (this->RationalPart.TheObjects[i]==0)
			sizeDecrease++;
		else
			break;
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
			out <<i<<". "<<tempS<<"; ";
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
			out<<KLindex<<"||"<<i<<" "<<tempS<<"; ";
	}
	output= out.str();
}

int IntegerPoly::EvaluateAtOne()
{ int result=0;
	for(int i=0;i<this->size;i++)
		result+=this->TheObjects[i].Coefficient.value;
	return result;
}

void IntegerPoly::Evaluate(root& values, Rational& output)
{ std::string tempS1, tempS2;
	output.MakeZero();
	//if(this->flagAnErrorHasOccurredTimeToPanic)
	//{ output.ElementToString(tempS1);
	//}
	for (int i=0;i<this->size;i++)
	{ Rational tempRat1,tempRat2;
		tempRat1.AssignInteger(this->TheObjects[i].Coefficient.value);
		for (int j=0;j<this->NumVars;j++)
		{ tempRat2.Assign(values.TheObjects[j]);
			if (tempRat2.IsEqualToZero())
			{	tempRat1.MakeZero();
				break;
			}
			tempRat2.RaiseToPower(this->TheObjects[i].degrees[j]);
			tempRat1.MultiplyBy(tempRat2);
			ParallelComputing::SafePoint();
		}
//		if(this->flagAnErrorHasOccurredTimeToPanic)
//		{ output.ElementToString(tempS2);
//			tempRat1.ElementToString(tempS1);
//			if (i==5)
//			{ //Rational::flagAnErrorHasOccurredTimeToPanic=true;
//			}
//		}
		output.Add(tempRat1);
//		if(this->flagAnErrorHasOccurredTimeToPanic)
//		{ output.ElementToString(tempS2);
//		}
	}
}

void IntegerPoly::MakePolyExponentFromIntRoot(intRoot&r)
{	this->ClearTheObjects();
	this->NumVars=(short)r.dimension;
	static Monomial<Integer> tempM;
	tempM.init((short)r.dimension);
	tempM.Coefficient.Assign(IOne);
	for (int i=0;i<r.dimension;i++)
		tempM.degrees[i]=(short) r.elements[i];
	this->AddMonomial(tempM);
}

int IntegerPoly::SizeWithoutDebugString()
{ int Accum=0;
	Accum+=this->HashedListBasicObjects<Monomial<Integer> >::SizeWithoutObjects();
	Accum+=	sizeof(this->NumVars);
	for (int i=0;i<this->ActualSize;i++)
		Accum+=this->TheActualObjects[i].SizeWithoutCoefficient()+sizeof(Integer);
	return Accum;
}

void LaTeXProcedures::beginPSTricks(std::fstream& output)
{ output<<"\\begin{pspicture}(8,8)";
}

void LaTeXProcedures::endPSTricks(std::fstream& output)
{ output<<"\\end{pspicture}\\medskip \\\\ \\noindent ";
}

void LaTeXProcedures::beginDocument(std::fstream& output)
{ output<< "\\documentclass{article}\n \\usepackage{pstricks} \\begin{document}";
}

void LaTeXProcedures::endLatexDocument(std::fstream& output)
{ output <<"\\end{document}";
}

void LaTeXProcedures::GetStringFromColorIndex(int ColorIndex, std::string &output)
{ switch(ColorIndex)
	{ case 0: output.assign("black"); break;
		case 1: output.assign("blue"); break;
		case 2: output.assign("purple"); break;
		case 3: output.assign("green"); break;
		case 4: output.assign("cyan"); break;
		case 5: output.assign("red"); break;
		case 6: output.assign("purple"); break;
		case 7: output.assign("cyan"); break;
		case 56540: output.assign("yellow"); break;
		default:
			output.assign("black");
			break;
	}
}

void LaTeXProcedures::drawText(	double X1, double Y1, std::string& theText, int ColorIndex, std::fstream& output)
{ output.precision(4);
	X1-=theText.length()* LaTeXProcedures::TextPrintCenteringAdjustmentX;
	Y1+=LaTeXProcedures::TextPrintCenteringAdjustmentY;
	X1/=LaTeXProcedures::ScaleFactor; Y1/=LaTeXProcedures::ScaleFactor;
	output	<< "\\put("<<X1-LaTeXProcedures::FigureCenterCoordSystemX
					<<","<<LaTeXProcedures::FigureCenterCoordSystemY-Y1<<"){\\tiny{"<<theText<<"}}";
}

void LaTeXProcedures::drawline
			(	double X1, double Y1, double X2, double Y2,
				unsigned long thePenStyle, int ColorIndex, std::fstream& output)
{ if ((int)thePenStyle== DrawingVariables::PenStyleInvisible)
		return;
	output.precision(4);
	X1/=LaTeXProcedures::ScaleFactor; X2/=LaTeXProcedures::ScaleFactor;
	Y1/=LaTeXProcedures::ScaleFactor; Y2/=LaTeXProcedures::ScaleFactor;
	std::string tempS;
	LaTeXProcedures::GetStringFromColorIndex(ColorIndex, tempS);
	output	<<"\\psline[linewidth=0.3pt, linecolor="<<tempS <<"]"
		<<"("<<X1-LaTeXProcedures::FigureCenterCoordSystemX <<","<<LaTeXProcedures::FigureCenterCoordSystemY-Y1<<")"
		<<"("<<X2-LaTeXProcedures::FigureCenterCoordSystemX <<","<<LaTeXProcedures::FigureCenterCoordSystemY-Y2<<")\n";
}

void thePFcomputation::Run()
{ RandomCodeIDontWantToDelete::SomeRandomTests6();
	this->theWeylGroup.MakeEn(6);
	this->theWeylGroup.GenerateRootSystemFromKillingFormMatrix();
	this->theWeylGroup.RootSystem.ComputeDebugString();
	this->ComputeTableAllowed();
	this->ComputeDebugString();
	this->EnumerateRecursively(0,0,6);
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
			out<<this->tableForbidden.TheObjects[i].elements[j]<<",";
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
			if (tempRat.IsPositive() || tempRat.NumShort==-2)
				tableForbidden.TheObjects[i].AddSelectionAppendNewIndex(j);
		}
		this->theForbiddenSelections.TheObjects[i].init
			(this->theWeylGroup.RootSystem.size);
	}
	this->theSelection.init(this->theWeylGroup.RootSystem.size);
	this->LCMdeterminants=1;
	this->TotalNumEnumerated=0;
}

void thePFcomputation::EnumerateRecursively(int depth, int startingIndex, int theDimension)
{ if (depth==theDimension)
	{	//this->theSelection.ComputeDebugString();
		this->TotalNumEnumerated++;
		this->ComputeDeterminantSelection(theDimension);
		return;
	}
	for (int i=startingIndex;i<this->theWeylGroup.RootSystem.size;i++)
		if (!this->theForbiddenSelections.TheObjects[depth].selected[i])
		{ this->theSelection.AddSelectionAppendNewIndex(i);
	//		this->theSelection.ComputeDebugString();
			if (depth+1!=theDimension)
				this->ComputeNewRestrictionsFromOld(depth+1,i);
			this->EnumerateRecursively(depth+1,i+1,theDimension);
			this->theSelection.RemoveLastSelection();
		}
}

void thePFcomputation::ComputeNewRestrictionsFromOld(int depth, int newIndex)
{ this->theForbiddenSelections.TheObjects[depth].Assign(this->theForbiddenSelections.TheObjects[depth-1]);
//	this->theForbiddenSelections.TheObjects[depth].ComputeDebugString();
	for(int i=0;i<this->tableForbidden.TheObjects[newIndex].CardinalitySelection;i++)
	{	this->theForbiddenSelections.TheObjects[depth].AddSelectionAppendNewIndex
			(this->tableForbidden.TheObjects[newIndex].elements[i]);
	}
}

void thePFcomputation::ComputeDeterminantSelection(int theDimension)
{	static MatrixLargeRational tempMat;
	this->SelectionToMatrixRational(tempMat, theDimension);
	//tempMat.ComputeDebugString();
	Rational tempRat;
	tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
	if (tempRat.NumShort==0) return;
	if (tempRat.NumShort<0){tempRat.Minus();}
	this->LCMdeterminants= MathRoutines::lcm(tempRat.NumShort, this->LCMdeterminants);
	//tempMat.ComputeDebugString();
}

void thePFcomputation::SelectionToString(std::string &output, int theDimension)
{ MatrixLargeRational tempMat;
	this->SelectionToMatrixRational(tempMat,theDimension);
	tempMat.ElementToSting(output);
}

void thePFcomputation::SelectionToMatrixRational(MatrixLargeRational &output, int theDimension)
{	output.init((short)theDimension,(short)theDimension);
	for (int i=0;i<theDimension;i++)
		for (int j=0;j<theDimension;j++)
			output.elements[i][j].Assign
				(this->theWeylGroup.RootSystem.TheObjects[this->theSelection.elements[i]].TheObjects[j]);
}

void rootSubalgebra::ComputeAll()
{ this->initFromAmbientWeyl();
	this->SimpleBasisK.CopyFromBase(this->genK);
	this->TransformToSimpleBasisGenerators(this->SimpleBasisK);
	this->ComputeKModules();
	this->ComputeCentralizerFromKModulesAndSortKModules();
	this->NilradicalKmods.init(this->kModules.size);
	this->theDynkinDiagram.ComputeDiagramType(this->SimpleBasisK,this->AmbientWeyl);
	this->theCentralizerDiagram.ComputeDiagramType(this->SimpleBasisCentralizerRoots,this->AmbientWeyl);
	this->ComputeDebugString();
}

void rootSubalgebra::ComputeCentralizerFromKModulesAndSortKModules()
{ this->CentralizerKmods.init(this->kModules.size);
	this->CentralizerRoots.size=0;
	this->SimpleBasisCentralizerRoots.size=0;
	int counter=0;
	for (int i=0;i<this->kModules.size;i++)
		if (this->kModules.TheObjects[i].size==1)
		{	this->kModules.SwapTwoIndices(counter,i);
			this->HighestWeightsGmodK.SwapTwoIndices(counter,i);
			this->LowestWeightsGmodK.SwapTwoIndices(counter,i);
			this->CentralizerKmods.AddSelectionAppendNewIndex(counter);
			this->CentralizerRoots.AddRoot
				(this->kModules.TheObjects[counter].TheObjects[0]);
			this->SimpleBasisCentralizerRoots.AddRoot
				(this->kModules.TheObjects[counter].TheObjects[0]);
			counter++;
		}
	this->TransformToSimpleBasisGenerators(this->SimpleBasisCentralizerRoots);
}

void rootSubalgebra::initFromAmbientWeyl()
{ MatrixLargeRational tempMat;
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
{ for (int i=0;i<theGens.size;i++)
		if (!theGens.TheObjects[i].IsPositiveOrZero())
			theGens.TheObjects[i].MinusRoot();
	bool reductionOccured=true;
	root tempRoot;
	while (reductionOccured)
	{ reductionOccured= false;
		for (int i=0;i<theGens.size;i++)
			for (int j=i+1; j<theGens.size;j++)
			{ if (this->flagAnErrorHasOccuredTimeToPanic)
					theGens.ComputeDebugString();
				tempRoot.Assign(theGens.TheObjects[i]);
				tempRoot.Subtract(theGens.TheObjects[j]);
				if (this->flagAnErrorHasOccuredTimeToPanic)
					tempRoot.ComputeDebugString();
				if (tempRoot.IsEqualToZero())
				{	theGens.PopIndexSwapWithLast(j);
					reductionOccured=true;
				}
				if (this->AllRoots.IndexOfObject(tempRoot)!=-1)
				{ if (!tempRoot.IsPositiveOrZero())
					{	tempRoot.MinusRoot();
						theGens.TheObjects[j].Assign(tempRoot);
					}
					else
						theGens.TheObjects[i].Assign(tempRoot);
					reductionOccured=true;
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
				tempRoot.Add(this->SimpleBasisK.TheObjects[i]);
			else
				tempRoot.Subtract(this->SimpleBasisK.TheObjects[i]);
			if (this->AllRoots.IndexOfObject(tempRoot)!=-1)
			{ outputW.Assign(tempRoot);
				FoundHigher=true;
			}
			if (tempRoot.IsEqualToZero()) {outputW.MakeZero(this->AmbientWeyl.KillingFormMatrix.NumRows); return;}
		}
	}
}

void rootSubalgebra::ComputeHighestWeightInTheSameKMod(root& input, root& outputHW)
{ this->ComputeExtremeWeightInTheSameKMod(input,outputHW,true);
}

void rootSubalgebra::ComputeLowestWeightInTheSameKMod(root& input, root& outputLW)
{ this->ComputeExtremeWeightInTheSameKMod(input,outputLW,false);
}


void rootSubalgebra::GenerateParabolicsInCentralizerAndPossibleNilradicals
	(GlobalVariables& theGlobalVariables)
{	this->GenerateKmodMultTable(this->theMultTable,this->theOppositeKmods,theGlobalVariables);
	if (this->flagAnErrorHasOccuredTimeToPanic)
		this->theMultTable.ComputeDebugString(*this);
	this->NumNilradicalsAllowed=0;
	this->NumConeConditionFailures=0;
	this->NumRelationsWithStronglyPerpendicularDecomposition=0;
	ListBasicObjects<Selection> impliedSelections;
	impliedSelections.SetSizeExpandOnTopNoObjectInit(this->kModules.size+1);
	Selection tempSel;
	tempSel.init(this->SimpleBasisCentralizerRoots.size);
	int numCycles= MathRoutines::TwoToTheNth(this->SimpleBasisCentralizerRoots.size);
	if (this->flagAnErrorHasOccuredTimeToPanic)
		this->ComputeDebugString();
	for (int i=0;i<numCycles;i++)
	{	impliedSelections.TheObjects[0].init(this->kModules.size);
		for (int j=0;j<this->CentralizerRoots.size;j++)
			for (int k=0;k<tempSel.CardinalitySelection;k++)
			{ root tempRoot;
				if (!this->CentralizerRoots.TheObjects[j].IsPositiveOrZero())
					break;
				else
					tempRoot.Assign(this->CentralizerRoots.TheObjects[j]);
				tempRoot.Subtract
					(	this->SimpleBasisCentralizerRoots.TheObjects
							[tempSel.elements[k]]);
				if (this->IsARootOrZero(tempRoot))
				{ impliedSelections.TheObjects[0].AddSelectionAppendNewIndex(j);
					break;
				}
			}
		if (this->flagAnErrorHasOccuredTimeToPanic)
			tempSel.ComputeDebugString();
		//impliedSelections.TheObjects[0].ComputeDebugString();
		this->GeneratePossibleNilradicalsRecursive
			(	theGlobalVariables, this->theMultTable,this->CentralizerRoots.size,
				impliedSelections, this->theOppositeKmods);
		tempSel.incrementSelection();
	}
}

void rootSubalgebra::GeneratePossibleNilradicalsRecursive
	(	GlobalVariables &theGlobalVariables,
		multTableKmods &multTable, int StartIndex,
		ListBasicObjects<Selection>& impliedSelections,
		ListBasicObjects<int> &oppositeKmods)
{ int RecursionDepth=0;
	std::string tempSsel, tempSopposite;
	ListBasicObjects<Selection> tempSels;
	if (this->flagAnErrorHasOccuredTimeToPanic)
	{	multTable.ComputeDebugString(*this);
		std::stringstream out; out <<"\n\t";
		for (int i=0;i<oppositeKmods.size;i++)
			out <<i <<" / " << oppositeKmods.TheObjects[i]<< "\t";
		tempSopposite=out.str();
		multTable.DebugString.append(tempSopposite);
	}
	ListBasicObjects<int> counters;
	counters.SetSizeExpandOnTopNoObjectInit(this->kModules.size+1);
	counters.TheObjects[0]=StartIndex;
	while (RecursionDepth>-1)
	{	while(counters.TheObjects[RecursionDepth]<this->kModules.size)
		{	if (! impliedSelections.TheObjects[RecursionDepth]
							.selected[counters.TheObjects[RecursionDepth]])
			{	if ( this->IndexIsCompatibleWithPrevious
							(	counters.TheObjects[RecursionDepth],RecursionDepth,multTable,
								impliedSelections,oppositeKmods))
				{	RecursionDepth++;
					counters.TheObjects[RecursionDepth]=counters.TheObjects[RecursionDepth-1];
				}
			}
			if (this->flagAnErrorHasOccuredTimeToPanic)
			{	impliedSelections.ElementToStringGeneric(tempSsel,RecursionDepth+1);
				impliedSelections.TheObjects[RecursionDepth].ComputeDebugString();
			}
			counters.TheObjects[RecursionDepth]++;
		}
		this->PossibleNilradicalComputation
			(theGlobalVariables,impliedSelections.TheObjects[RecursionDepth]);
		if (this->flagAnErrorHasOccuredTimeToPanic)
			tempSels.AddObjectOnTop(impliedSelections.TheObjects[RecursionDepth]);
		RecursionDepth--;
		if (RecursionDepth>-1)
			counters.TheObjects[RecursionDepth]++;
	}
}

bool rootSubalgebra::ListHasNonSelectedIndexLowerThanGiven
	(int index, ListBasicObjects<int>& tempList, Selection& tempSel)
{	for (int j=0;j<tempList.size;j++)
		if (tempList.TheObjects[j]<index)
			if (!tempSel.selected[tempList.TheObjects[j]])
				return false;
	return true;
}

bool rootSubalgebra::IndexIsCompatibleWithPrevious
	(	int startIndex,int RecursionDepth,	multTableKmods & multTable,
		ListBasicObjects<Selection>& impliedSelections,
		ListBasicObjects<int> &oppositeKmods)
{ Selection& targetSel= impliedSelections.TheObjects[RecursionDepth+1];
	Selection& originalSel=impliedSelections.TheObjects[RecursionDepth];
	targetSel.Assign(originalSel);
	targetSel.AddSelectionAppendNewIndex(startIndex);
	for (int k=targetSel.CardinalitySelection-1;k<targetSel.CardinalitySelection;k++)
	{	int tempI=targetSel.elements[k];
		for (int i=0; i<targetSel.CardinalitySelection;i++ )
		{	if (targetSel.selected[oppositeKmods.TheObjects[targetSel.elements[i]]])
				return false;
			ListBasicObjects<int>& tempList=
				multTable.TheObjects[tempI].TheObjects[targetSel.elements[i]];
			for (int j=0;j<tempList.size;j++)
			{	if (tempList.TheObjects[j]<startIndex && !originalSel.selected[tempList.TheObjects[j]])
					return false;
				else
					targetSel.AddSelectionAppendNewIndex(tempList.TheObjects[j]);
			}
		}
	}
	return true;
}

void rootSubalgebra::PossibleNilradicalComputation
	(GlobalVariables& theGlobalVariables, Selection& selKmods)
{ this->NumNilradicalsAllowed++;
	this->NilradicalKmods.Assign(selKmods);
	this->ComputeDebugString();
	if(!this->ConeConditionHolds(theGlobalVariables))
		this->NumConeConditionFailures++;
	Stop();
}

void rootSubalgebra::GenerateKmodMultTable
	(	ListBasicObjects<ListBasicObjects< ListBasicObjects<int> > >& output,
		ListBasicObjects<int>& oppositeKmods,
		GlobalVariables& theGlobalVariables)
{ output.SetSizeExpandOnTopNoObjectInit(this->kModules.size);
	oppositeKmods.SetSizeExpandOnTopNoObjectInit(this->kModules.size);
	for (int i=0;i<this->kModules.size;i++)
	{ output.TheObjects[i].SetSizeExpandOnTopNoObjectInit(this->kModules.size);
		for (int j=0;j<this->kModules.size;j++)
			this->KmodTimesKmod(i,j,oppositeKmods,output.TheObjects[i].TheObjects[j]);
	}
}

bool rootSubalgebra::IsARoot(root& input)
{ if (input.size!=this->AmbientWeyl.KillingFormMatrix.NumRows)
		return false;
	return !(this->AmbientWeyl.RootSystem.ContainsObjectHash(input)==-1);
}

bool rootSubalgebra::IsARootOrZero(root& input)
{ return input.IsEqualToZero() || this->IsARoot(input);
}

void rootSubalgebra::KmodTimesKmod
	(int index1, int index2,ListBasicObjects<int>& oppositeKmods,
	 ListBasicObjects<int>& output)
{ root tempRoot;
	output.size=0;
	for (int i=0;i<this->kModules.TheObjects[index1].size;i++)
		for (int j=0;j<this->kModules.TheObjects[index2].size;j++)
		{ tempRoot.Assign(this->kModules.TheObjects[index1].TheObjects[i]);
			tempRoot.Add(this->kModules.TheObjects[index2].TheObjects[j]);
			tempRoot.ComputeDebugString();
			if (tempRoot.IsEqualToZero())
			{ oppositeKmods.TheObjects[index1]=index2;
				oppositeKmods.TheObjects[index2]=index1;
			}
			else
				if (this->IsARoot(tempRoot))
					for (int k=0;k<this->kModules.size;k++)
						if (this->kModules.TheObjects[k].IndexOfObject(tempRoot)!=-1)
						{	output.AddObjectOnTopNoRepetitionOfObject(k);
							break;
						}
		}
}

void rootSubalgebra::ComputeKModules()
{	this->HighestRootsK.size =0;
	this->LowestWeightsGmodK.size=0;
	this->HighestWeightsGmodK.size=0;
	this->kModules.size=0;
	this->ComputeDebugString();
	this->ComputeRootsOfK();
	for (int i=0;i<this->AllRoots.size;i++)
	{ if (this->AllRootsK.IndexOfObject(this->AllRoots.TheObjects[i])==-1)
		{	root tempLW, tempHW;
			this->ComputeLowestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempLW);
			this->ComputeHighestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempHW);
			int x=this->LowestWeightsGmodK.IndexOfObject(tempLW);
			if (x==-1)
			{ this->LowestWeightsGmodK.AddRoot(tempLW);
				this->HighestWeightsGmodK.AddRoot(tempHW);
				x=this->LowestWeightsGmodK.size -1;
				this->kModules.SetSizeExpandOnTopNoObjectInit(this->LowestWeightsGmodK.size);
				this->kModules.TheObjects[x].size=0;
			}
			this->kModules.TheObjects[x].AddRoot(this->AllRoots.TheObjects[i]);
		}
		else
		{	if (this->AllRoots.TheObjects[i].IsPositiveOrZero())
			{	root tempHW;
				this->ComputeHighestWeightInTheSameKMod(this->AllRoots.TheObjects[i],tempHW);
				int x=this->HighestRootsK.IndexOfObject(tempHW);
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

int rootSubalgebra::NumRootsInNilradical()
{ int result=0;
	for (int i=0;i<this->NilradicalKmods.CardinalitySelection;i++)
		result+=this->kModules.TheObjects[this->NilradicalKmods.elements[i]].size;
	return result;
}

bool rootSubalgebra::ConeConditionHolds(GlobalVariables& theGlobalVariables)
{ MatrixLargeRational& matA= theGlobalVariables.matConeCondition1;
	MatrixLargeRational& matb= theGlobalVariables.matConeCondition2;
	MatrixLargeRational& matX= theGlobalVariables.matConeCondition3;
	roots& NilradicalRoots= theGlobalVariables.rootsNilradicalRoots;
	int numNilradRoots=this->NumRootsInNilradical();
	int theDimension= this->AmbientWeyl.KillingFormMatrix.NumRows;
	int numCols= numNilradRoots+this->kModules.size-
		this->NilradicalKmods.CardinalitySelection;
	matA.init((short)theDimension+1, (short)numCols);
	matb.init((short)theDimension+1,1);
	NilradicalRoots.size=0;
	matb.NullifyAll(); matb.elements[theDimension][0].MakeOne();
	int counter=0;
	for (int i=0;i<this->NilradicalKmods.CardinalitySelection;i++)
	{	roots& tempKmod= this->kModules.TheObjects[this->NilradicalKmods.elements[i]];
		for (int j=0;j<tempKmod.size;j++)
		{	for (int k=0;k<theDimension;k++)
				matA.elements[k][counter].Assign(tempKmod.TheObjects[j].TheObjects[k]);
			NilradicalRoots.AddRoot(tempKmod.TheObjects[j]);
			matA.elements[theDimension][counter].MakeOne();
			counter++;
		}
	}
	for (int i=0;i<this->kModules.size;i++)
	{ if (!this->NilradicalKmods.selected[i])
		{ for (int k=0;k<theDimension;k++)
			{	matA.elements[k][counter].Assign
					(this->HighestWeightsGmodK.TheObjects[i].TheObjects[k]);
				matA.elements[k][counter].Minus();
			}
			//this->HighestWeightsGmodK.TheObjects[i].ComputeDebugString();
			matA.elements[theDimension][counter].MakeZero();
			counter++;
		}
	}
	//matA.ComputeDebugString();
	//matb.ComputeDebugString();
	//matX.ComputeDebugString();
	if (	MatrixLargeRational
					::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
						(matA,matb,matX,theGlobalVariables))
	{	this->ExtractRelations(matA, matX,NilradicalRoots);
		return false;
	}
	else
		return true;
}

bool rootSubalgebra::CheckForSmallRelations
	(coneRelation& theRel, roots& nilradicalRoots)
{ //return false;
	root tempRoot;
	for (int i=0;i<this->kModules.size;i++)
		if (!this->NilradicalKmods.selected[i])
			for (int j=i+1;j<this->kModules.size;j++)
			{ if (!this->NilradicalKmods.selected[j])
				{	tempRoot.Assign(this->HighestWeightsGmodK.TheObjects[i]);
					tempRoot.Add(this->HighestWeightsGmodK.TheObjects[j]);
					if (!tempRoot.IsEqualToZero())
					{ theRel.BetaCoeffs.SetSizeExpandOnTopNoObjectInit(0);
						theRel.Betas.SetSizeExpandOnTopNoObjectInit(0);
						if (tempRoot.HasStronglyPerpendicularDecompositionWRT
									(nilradicalRoots,this->AmbientWeyl,theRel.Betas, theRel.BetaCoeffs))
						{	theRel.Alphas.size=0;
							theRel.AlphaCoeffs.size=0;
							theRel.Alphas.AddObjectOnTop(this->HighestWeightsGmodK.TheObjects[i]);
							theRel.Alphas.AddObjectOnTop(this->HighestWeightsGmodK.TheObjects[j]);
							theRel.AlphaCoeffs.AddObjectOnTop(ROne);
							theRel.AlphaCoeffs.AddObjectOnTop(ROne);
							theRel.ComputeDebugString(*this);
							return true;
						}
					}
				}
			}
	return false;
}

void rootSubalgebra::ExtractRelations
	(MatrixLargeRational& matA, MatrixLargeRational& matX, roots& NilradicalRoots)
{	int theDimension= this->AmbientWeyl.KillingFormMatrix.NumRows;
	if (this->flagAnErrorHasOccuredTimeToPanic)
	{	this->NilradicalKmods.ComputeDebugString();
		NilradicalRoots.ComputeDebugString();
		matX.ComputeDebugString();
	}
	root tempRoot; tempRoot.MakeZero(theDimension);
	coneRelation theRel;
	if (!this->CheckForSmallRelations(theRel,NilradicalRoots))
	{	this->NumRelationsgreaterLengthThan2++;
		matX.ScaleToIntegralForMinRationalHeightNoSignChange();
		for (int i=0;i<matA.NumCols;i++)
			if (!matX.elements[i][0].IsEqualToZero())
			{	for (int j=0;j<theDimension;j++)
					tempRoot.TheObjects[j].Assign(matA.elements[j][i]);
				assert(matX.elements[i][0].DenShort==1);
				if (i<NilradicalRoots.size)
				{ theRel.Betas.AddObjectOnTop(tempRoot);
					theRel.BetaCoeffs.AddObjectOnTop(matX.elements[i][0]);
				} else
				{	tempRoot.MinusRoot();
					theRel.Alphas.AddObjectOnTop(tempRoot);
					theRel.AlphaCoeffs.AddObjectOnTop(matX.elements[i][0]);
				}
			}
	}
//	theRel.ComputeDebugString();
//	this->MakeSureAlphasDontSumToRoot(theRel,NilradicalRoots);
	theRel.ComputeDebugString(*this);
//	this->MakeGeneratingSingularVectors(theRel,NilradicalRoots);
//	this->MakeSureAlphasDontSumToRoot(theRel,NilradicalRoots);
	this->MakeGeneratingSingularVectors(theRel,NilradicalRoots);

	theRel.ComputeDebugString(*this);
	root AccumRoot;
	theRel.GetSumAlphas(AccumRoot,this->AmbientWeyl.KillingFormMatrix.NumRows);
	if (this->flagAnErrorHasOccuredTimeToPanic)
	{	std::string tempS;
		theRel.ComputeDebugString(*this);
		AccumRoot.ComputeDebugString();
	}
	theRel.BetaCoeffs.size=0; theRel.Betas.size=0;
	if (	AccumRoot.HasStronglyPerpendicularDecompositionWRT
					(NilradicalRoots,this->AmbientWeyl,theRel.Betas, theRel.BetaCoeffs))
	{	this->NumRelationsWithStronglyPerpendicularDecomposition++;
		roots tempRoots;
		if (this->flagAnErrorHasOccuredTimeToPanic)
			theRel.ComputeDebugString(*this);
		tempRoots.CopyFromBase(theRel.Alphas);
		tempRoots.AddListOnTop(theRel.Betas);
		if (this->flagAnErrorHasOccuredTimeToPanic)
			tempRoots.ComputeDebugString();
		this->TransformToSimpleBasisGenerators(tempRoots);
		if (this->flagAnErrorHasOccuredTimeToPanic)
			tempRoots.ComputeDebugString();
		theRel.theDiagram.ComputeDiagramType(tempRoots, this->AmbientWeyl);
		if (theRel.theDiagram.DynkinTypeStrings.TheObjects[0]=="A1")
			Stop();
		theRel.SortRelation(*this);
		this->theGoodRelations.AddRelationNoRepetition(theRel,*this);
	}
	else
	{ theRel.ComputeDebugString(*this);
		//this->MakeSureAlphasDontSumToRoot(theRel,NilradicalRoots);
		this->theBadRelations.AddObjectOnTop(theRel);
	}
}

void rootSubalgebra::MakeSureAlphasDontSumToRoot(coneRelation& theRel, roots& NilradicalRoots)
{ root alpha1, alpha2,beta1, tempRoot;
	bool madeChange=true;
	while(madeChange)
	{	madeChange=false;
		for (int i=0;i<theRel.Alphas.size;i++)
			for(int j=i+1;j<theRel.Alphas.size;j++)
			{ theRel.ComputeDebugString(*this);
				beta1.Assign(theRel.Alphas.TheObjects[i]);
				beta1.Add(theRel.Alphas.TheObjects[j]);
				if (this->IsARootOrZero(beta1))
				{	this->ComputeHighestWeightInTheSameKMod(beta1,tempRoot);
					assert(tempRoot.IsEqualTo(tempRoot));
					if (NilradicalRoots.ContainsObject(beta1))
					{ alpha1.Assign(theRel.Alphas.TheObjects[i]);
						alpha2.Assign(theRel.Alphas.TheObjects[j]);
						theRel.Alphas.SetSizeExpandOnTopNoObjectInit(2);
						theRel.AlphaCoeffs.SetSizeExpandOnTopNoObjectInit(2);
						theRel.Alphas.TheObjects[0].Assign(alpha1);
						theRel.Alphas.TheObjects[1].Assign(alpha2);
						theRel.AlphaCoeffs.TheObjects[0].Assign(ROne);
						theRel.AlphaCoeffs.TheObjects[1].Assign(ROne);
						theRel.Betas.SetSizeExpandOnTopNoObjectInit(1);
						theRel.BetaCoeffs.SetSizeExpandOnTopNoObjectInit(1);
						theRel.BetaCoeffs.TheObjects[0].Assign(ROne);
						theRel.Betas.TheObjects[0].Assign(beta1);
						madeChange=false;
						break;
					}
					else
					{ int changedIndex=i, otherIndex=j;
						Rational alpha1coeff, alpha2coeff;
						if (alpha1coeff.IsGreaterThanOrEqualTo
									(alpha2coeff))
						{ changedIndex=j;
							otherIndex=i;
						}
						alpha1coeff.Assign(theRel.AlphaCoeffs.TheObjects[changedIndex]);
						alpha2coeff.Assign(theRel.AlphaCoeffs.TheObjects[otherIndex]);
						alpha2.Assign(theRel.Alphas.TheObjects[otherIndex]);
						alpha2coeff.Subtract(alpha1coeff);
						madeChange=true;
						theRel.Alphas.TheObjects[changedIndex].Assign(beta1);
						theRel.AlphaCoeffs.TheObjects[changedIndex].Assign(alpha1coeff);
						if (alpha2coeff.IsEqualToZero())
						{	theRel.Alphas.PopIndexSwapWithLast(otherIndex);
							theRel.AlphaCoeffs.PopIndexSwapWithLast(otherIndex);
						} else
						{ theRel.Alphas.TheObjects[otherIndex].Assign(alpha2);
							theRel.AlphaCoeffs.TheObjects[otherIndex].Assign(alpha2coeff);
						}
					}
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
				this->PositiveRootsK.AddRoot(this->AllRoots.TheObjects[i]);
		}
	}
}

void rootSubalgebra::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

void rootSubalgebra::ElementToStringLaTeXHeaderModTable(std::string& outputHeader,std::string&  outputFooter)
{ outputHeader.clear();
  outputHeader.append
    ("\n\n\\noindent\\begin{tabular}{|ccccc|} \n \\multicolumn{5}{c}{");
  outputHeader.append("$\\mathfrak{g}/\\mathfrak{k}$ $\\mathfrak{k}$-submodules} \\\\");
  outputHeader.append ("id & size & highest weight& lowest weight& elements\\\\");
  outputFooter.clear();
  outputFooter.append("\\hline \\end{tabular}");
}

bool rootSubalgebra::IsIsomorphicTo(rootSubalgebra& right)
{ if (this->theDynkinDiagram.DebugString!= right.theDynkinDiagram.DebugString)
    return false;
  if (this->theCentralizerDiagram.DebugString!= right.theCentralizerDiagram.DebugString)
    return false;
  roots isoDomain, isoRange;
  permutation permComponents, permComponentsCentralizer;
  ListBasicObjects<int> UniTypicComponents, tempPermutation1, tempPermutation2;
  UniTypicComponents.SetSizeExpandOnTopNoObjectInit
    (this->theDynkinDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0;i<this->theDynkinDiagram.sameTypeComponents.size;i++)
  { UniTypicComponents.TheObjects[i]= this->theDynkinDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=UniTypicComponents.TheObjects[i];
  }
  permComponents.initPermutation(UniTypicComponents,tempSize);
  UniTypicComponents.SetSizeExpandOnTopNoObjectInit
    (this->theCentralizerDiagram.sameTypeComponents.size);
  tempSize=0;
  for (int i=0;i<this->theCentralizerDiagram.sameTypeComponents.size;i++)
  { UniTypicComponents.TheObjects[i]= this->theCentralizerDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=UniTypicComponents.TheObjects[i];
  }
  permComponentsCentralizer.initPermutation(UniTypicComponents,tempSize);
  int tempI1= permComponents.getTotalNumSubsets();
  int tempI2= permComponentsCentralizer.getTotalNumSubsets();
  permComponents.GetPermutation(tempPermutation1);
  permComponentsCentralizer.GetPermutation(tempPermutation2);
  for (int i=0;i<tempI1;i++)
  { for(int j=0;j<tempI2;j++)
    { isoDomain.size=0; isoRange.size=0;
      this->theDynkinDiagram.GetMapFromPermutation(isoDomain,isoRange,tempPermutation1);
      this->theCentralizerDiagram.GetMapFromPermutation(isoDomain,isoRange,tempPermutation2);
     // if (this->attemptExtensionToIsomorphism(isoDomain,isoRange))
		//		return true;
      permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
    }
    permComponents.incrementAndGetPermutation(tempPermutation1);
  }
  return false;
}

bool rootSubalgebra::attemptExtensionToIsomorphism
	(roots& domain, roots& range, GlobalVariables& theGlobalVariables)
{	int CurrentRank=domain.GetRankOfSpanOfElements(theGlobalVariables);
	assert(CurrentRank==range.GetRankOfSpanOfElements(theGlobalVariables));
	rootSubalgebra leftSA, rightSA;
	leftSA.genK.size=0; rightSA.genK.size=0;
	leftSA.AmbientWeyl.KillingFormMatrix.Assign(this->AmbientWeyl.KillingFormMatrix);
	rightSA.AmbientWeyl.KillingFormMatrix.Assign(this->AmbientWeyl.KillingFormMatrix);
	leftSA.genK.AddListOnTop(domain); rightSA.genK.AddListOnTop(range);
	leftSA.ComputeAll(); rightSA.ComputeAll();
	for (int i=CurrentRank;i<this->AmbientWeyl.KillingFormMatrix.NumRows;i++)
	{	
	}
	return false;
}



void rootSubalgebra::ElementToString(std::string &output, bool makeALaTeXReport)
{ std::stringstream out;
	std::string tempS;
	std::string latexFooter, latexHeader;
	int LatexLineCounter=0;
	this->ElementToStringLaTeXHeaderModTable(latexHeader, latexFooter);
	this->theDynkinDiagram.ElementToString(tempS);
  out <<"\\noindent Semisimple type of $\\mathfrak{k}:$ "
      << tempS;
  if (!makeALaTeXReport)
    out<<"\nRank k: " <<this->SimpleBasisK.size;
	this->SimpleBasisK.ElementToString(tempS,makeALaTeXReport);
	out <<"\n\\noindent Simple basis: "<<tempS;
	this->theCentralizerDiagram.ElementToString(tempS);
	out<<"\n\n\\noindent  Type of centralizer of $\\mathfrak{k}$: " <<tempS<<"; simple basis centralizer: ";
	this->SimpleBasisCentralizerRoots.ElementToString(tempS,makeALaTeXReport);
	out <<tempS;
	out << "\n\n\\noindent Number $\\mathfrak{g}/\\mathfrak{k}$ $\\mathfrak{k}$-submodules: "<<this->LowestWeightsGmodK.size;
	if (makeALaTeXReport)
    out << latexHeader;
	for (int i=0;i<this->kModules.size;i++)
	{ this->LowestWeightsGmodK.TheObjects[i].ElementToString(tempS,makeALaTeXReport);
    if (!makeALaTeXReport)
      out << "\n\n Module ";
    else
      out <<"\\hline";
    out<<i;
    if (!makeALaTeXReport)
      out << ": size: ";
    else
      out <<" & ";
    out << this->kModules.TheObjects[i].size;
    if (!makeALaTeXReport)
      out << "; lowest weight: ";
    else
      out <<" & ";
		out << tempS;
		this->HighestWeightsGmodK.TheObjects[i].ElementToString(tempS,makeALaTeXReport);
    if (!makeALaTeXReport)
      out << "; highest weight: ";
    else
      out <<" & ";
		out	<< tempS;
		if (!makeALaTeXReport)
      out	<< "; elements: ";
    else
      out <<" & \n\\begin{tabular}{c}\n";
		for (int j=0;j<this->kModules.TheObjects[i].size;j++)
		{ this->kModules.TheObjects[i].TheObjects[j].ElementToString(tempS,makeALaTeXReport);
			out	<<tempS;
			if (j!=this->kModules.TheObjects[i].size)
        out<<", ";
      if (makeALaTeXReport)
        out<<"\\\\";
    }
    if (makeALaTeXReport)
      out <<"\\end{tabular}\\\\\n";
    if (LatexLineCounter>this->NumGmodKtableRowsAllowedLatex)
    { LatexLineCounter=0;
      out <<latexFooter<<latexHeader;
    }
    if (i!=this->kModules.size-1)
    { LatexLineCounter+=this->kModules.TheObjects[i].size;
      if (  (LatexLineCounter>this->NumGmodKtableRowsAllowedLatex) &&
            (LatexLineCounter!=this->kModules.TheObjects[i].size))
      { out <<latexFooter<<latexHeader;
        LatexLineCounter=this->kModules.TheObjects[i].size;
      }
    }
	}
	if (makeALaTeXReport)
    out<<latexFooter;
	out <<"\n\n\\noindent Pairing table:\n\n\\noindent";
	this->theMultTable.ElementToString(tempS,makeALaTeXReport,*this);
	out << tempS <<"\n";
	output=out.str();
}

void rootSubalgebra::MakeGeneratingSingularVectors
	( coneRelation &theRelation, roots& nilradicalRoots)
{ bool isMaximal=false;
	root beta,tempRoot;
	theRelation.ComputeDebugString(*this);
	while (!isMaximal)
	{ isMaximal=true;
		for (int i=0;i<theRelation.AlphaCoeffs.size;i++)
		{ for (int j=0;j<nilradicalRoots.size;j++)
			{ tempRoot.Assign(theRelation.Alphas.TheObjects[i]);
				theRelation.Alphas.TheObjects[i].ComputeDebugString();
				nilradicalRoots.TheObjects[j].ComputeDebugString();
				tempRoot.Add(nilradicalRoots.TheObjects[j]);
				tempRoot.ComputeDebugString();
				theRelation.ComputeDebugString(*this);
				if (	(this->IsARoot(tempRoot) || tempRoot.IsEqualToZero())
						&&(!nilradicalRoots.ContainsObject(tempRoot)))
				{ this->ComputeHighestWeightInTheSameKMod(tempRoot,tempRoot);
					tempRoot.ComputeDebugString();
					tempRoot.Subtract(theRelation.Alphas.TheObjects[i]);
					tempRoot.ComputeDebugString();
					theRelation.Alphas.TheObjects[i].Add(tempRoot);
					int tempI=theRelation.Betas.IndexOfObject(tempRoot);
					if (tempI==-1)
					{ theRelation.Betas.AddObjectOnTop(tempRoot);
						theRelation.BetaCoeffs.AddObjectOnTop
							(theRelation.AlphaCoeffs.TheObjects[i]);
					}
					else
						theRelation.BetaCoeffs.TheObjects[tempI].Add
							(theRelation.AlphaCoeffs.TheObjects[i]);
					isMaximal=false;
					break;
				}
				if (theRelation.Alphas.TheObjects[i].IsEqualToZero())
				{	theRelation.Alphas.PopIndexSwapWithLast(i);
					theRelation.AlphaCoeffs.PopIndexSwapWithLast(i);
					i--;
					isMaximal=false;
					break;
				}
			}
		}
	}
}

void ::DynkinDiagramRootSubalgebra::Sort()
{ //doing bubble sort
	for (int i=0;i<this->SimpleBasesConnectedComponents.size;i++)
	{ for (int j=i+1;j<this->SimpleBasesConnectedComponents.size;j++)
		{	bool tempBool=false;
			if (	this->SimpleBasesConnectedComponents.TheObjects[i].size<
						this->SimpleBasesConnectedComponents.TheObjects[j].size)
				tempBool=true;
			if 	(	this->SimpleBasesConnectedComponents.TheObjects[i].size==
						this->SimpleBasesConnectedComponents.TheObjects[j].size)
				tempBool=
					(	(this->DynkinTypeStrings.TheObjects[i])[0]<
						(this->DynkinTypeStrings.TheObjects[j])[0]);
			if (tempBool)
			{	this->DynkinTypeStrings.SwapTwoIndices(i,j);
				this->SimpleBasesConnectedComponents.SwapTwoIndices(i,j);
			}
		}
	}
	this->sameTypeComponents.size=0;
	this->indexInUniComponent.SetSizeExpandOnTopNoObjectInit(this->SimpleBasesConnectedComponents.size);
	this->indexUniComponent.SetSizeExpandOnTopNoObjectInit(this->SimpleBasesConnectedComponents.size);
	this->sameTypeComponents.MakeActualSizeAtLeastExpandOnTop(this->SimpleBasesConnectedComponents.size);
	std::string LastString="";
	for (int i=0; i<this->SimpleBasesConnectedComponents.size;i++)
	{ if (!(this->DynkinTypeStrings.TheObjects[i]==LastString))
		{	this->sameTypeComponents.SetSizeExpandOnTopNoObjectInit(this->sameTypeComponents.size+1);
			this->sameTypeComponents.LastObject()->size=0;
			LastString=this->DynkinTypeStrings.TheObjects[i];
		}
		this->sameTypeComponents.LastObject()->AddObjectOnTop(i);
		this->indexUniComponent.TheObjects[i]=this->sameTypeComponents.size-1;
		this->indexInUniComponent.TheObjects[i]=this->sameTypeComponents.LastObject()->size-1;
	}
}

void ::DynkinDiagramRootSubalgebra::ComputeDiagramType
	(roots& simpleBasisInput, WeylGroup& theWeyl)
{ this->SimpleBasesConnectedComponents.size=0;
	for (int i=0;i<simpleBasisInput.size;i++)
	{	int indexFirstComponentConnectedToRoot=-1;
		for (int j=0;j<this->SimpleBasesConnectedComponents.size;j++)
			if (this->SimpleBasesConnectedComponents.TheObjects[j]
						.ContainsARootConnectedTo(simpleBasisInput.TheObjects[i],theWeyl))
			{	if (indexFirstComponentConnectedToRoot==-1)
				{ indexFirstComponentConnectedToRoot=j;
					this->SimpleBasesConnectedComponents.TheObjects[j].AddObjectOnTop
						(simpleBasisInput.TheObjects[i]);
				}
				else
				{	this->SimpleBasesConnectedComponents
						.TheObjects[indexFirstComponentConnectedToRoot]
							.AddListOnTop(this->SimpleBasesConnectedComponents.TheObjects[j]);
					this->SimpleBasesConnectedComponents.PopIndexSwapWithLast(j);
					j--;
				}
			}
		if (indexFirstComponentConnectedToRoot==-1)
		{	this->SimpleBasesConnectedComponents.SetSizeExpandOnTopNoObjectInit
				(this->SimpleBasesConnectedComponents.size+1);
			this->SimpleBasesConnectedComponents.LastObject()->size=0;
			this->SimpleBasesConnectedComponents.LastObject()->AddObjectOnTop
				(simpleBasisInput.TheObjects[i]);
		}
	}
	this->indicesThreeNodes.SetSizeExpandOnTopNoObjectInit
		(this->SimpleBasesConnectedComponents.size);
	this->DynkinTypeStrings.SetSizeExpandOnTopNoObjectInit
		(this->SimpleBasesConnectedComponents.size);
  this->indicesEnds.SetSizeExpandOnTopNoObjectInit(this->SimpleBasesConnectedComponents.size);
	this->ComputeDynkinStrings(theWeyl);
	this->Sort();
	this->ComputeDebugString();
}

void DynkinDiagramRootSubalgebra::GetMapFromPermutation
  (roots& domain, roots& range, ListBasicObjects< int >& thePerm)
{ for(int i=0;i<this->SimpleBasesConnectedComponents.size;i++)
    for (int j=0;j<this->SimpleBasesConnectedComponents.TheObjects[i].size;j++)
    { domain.AddObjectOnTop( this->SimpleBasesConnectedComponents.TheObjects[i].TheObjects[j]);
      range.AddObjectOnTop
        ( this->SimpleBasesConnectedComponents.TheObjects
            [thePerm.TheObjects[i]].TheObjects[j]);
    }
}

void ::DynkinDiagramRootSubalgebra::ComputeDynkinStrings(WeylGroup& theWeyl)
{ for (int i=0;i<this->SimpleBasesConnectedComponents.size;i++)
		this->ComputeDynkinString(i,theWeyl);
}

void ::DynkinDiagramRootSubalgebra::ElementToString(std::string& output)
{ std::stringstream out;
	for (int i=0;i<this->SimpleBasesConnectedComponents.size;i++)
		out <<this->DynkinTypeStrings.TheObjects[i]<<"\n";
	output=out.str();
}

void ::DynkinDiagramRootSubalgebra::ComputeDynkinString
	(int indexComponent, WeylGroup& theWeyl)
{ assert(indexComponent<this->SimpleBasesConnectedComponents.size);
	std::stringstream out;
	roots& currentComponent=
		this->SimpleBasesConnectedComponents.TheObjects[indexComponent];
  ListBasicObjects<int>& currentEnds=this->indicesEnds.TheObjects[indexComponent];
	if (this->numberOfThreeValencyNodes(indexComponent,theWeyl)==1)
	{//type D or E
	  root tripleNode;
	  int tripleNodeindex=this->indicesThreeNodes.TheObjects[indexComponent];
	  tripleNode.Assign( currentComponent.TheObjects[tripleNodeindex]);
		roots tempRoots;
		tempRoots.CopyFromBase(currentComponent);
		tempRoots.PopIndexSwapWithLast(tripleNodeindex);
		DynkinDiagramRootSubalgebra	tempDiagram;
		tempDiagram.ComputeDiagramType(tempRoots,theWeyl);
		assert(tempDiagram.SimpleBasesConnectedComponents.size==3);
		ListBasicObjects<int> indicesLongComponents;
		indicesLongComponents.size=0;
		Rational tempRat;
		for (int i=0;i<3;i++)
		{	if(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].size>1)
				indicesLongComponents.AddObjectOnTop(i);
      theWeyl.RootScalarKillingFormMatrixRoot
        ( tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].TheObjects[0],
          currentComponent.TheObjects[tripleNodeindex],
          tempRat);
      if (tempRat.IsEqualToZero())
        tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].ReverseOrderElements();
		}
    for(int i=0;i<3;i++)
      for(int j=0;j<3;j++)
      { if (tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].size<
            tempDiagram.SimpleBasesConnectedComponents.TheObjects[j].size)
        { tempRoots.CopyFromBase(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i]);
          tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].CopyFromBase
            (tempDiagram.SimpleBasesConnectedComponents.TheObjects[j]);
          tempDiagram.SimpleBasesConnectedComponents.TheObjects[j].CopyFromBase(tempRoots);
        }
      }
    currentComponent.size=0;
    currentComponent.AddObjectOnTop(tripleNode);
    for (int i=0;i<3;i++)
      currentComponent.AddListOnTop(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i]);
		if ( indicesLongComponents.size==1 || indicesLongComponents.size==0)
			out<<"D" <<currentComponent.size;
		else
		{//type E
			assert(indicesLongComponents.size==2);
			out <<"E" <<currentComponent.size;
		}
	}else
	{ Rational length1, length2, tempRat;
		theWeyl.RootScalarKillingFormMatrixRoot
			(currentComponent.TheObjects[0],currentComponent.TheObjects[0],	length1);
		int numLength1=1; int numLength2=0;
		for(int i=1;i<currentComponent.size;i++)
		{ theWeyl.RootScalarKillingFormMatrixRoot
				(currentComponent.TheObjects[i],currentComponent.TheObjects[i],	tempRat);
			if (tempRat.IsEqualTo(length1))
				numLength1++;
			else
			{ numLength2++;
				length2.Assign(tempRat);
			}
		}
		if (numLength2==0 )
		{//type A
			out<< "A"<<numLength1;
		}
		else
		{ Rational greaterlength, tempRat;
		  greaterlength.Assign(length2);
		  if (length1.IsGreaterThan(length2))
        greaterlength.Assign(length1);
      theWeyl.RootScalarKillingFormMatrixRoot
        ( currentComponent.TheObjects[currentEnds.TheObjects[0]],
          currentComponent.TheObjects[currentEnds.TheObjects[0]],
          tempRat);
      if (greaterlength.IsGreaterThan(tempRat))
        currentEnds.SwapTwoIndices(0,1);
		  if (numLength1==numLength2)
			{//B2, C2, F4 or G2
				if (numLength1!=1)
				{	out <<"F4";
					assert(numLength1==2);
				}
				else
				{ if (length1.NumShort==6 || length2.NumShort==6)
						out<<"G2";
					else
						out<<"B2=C2";
				}
			}
		}
		currentComponent.SwapTwoIndices(0,currentEnds.TheObjects[0]);
		for (int i=0;i<currentComponent.size;i++)
      for (int j=i+1;j<currentComponent.size;j++)
      { theWeyl.RootScalarKillingFormMatrixRoot
          (currentComponent.TheObjects[i],currentComponent.TheObjects[j],tempRat);
        if (!tempRat.IsEqualToZero())
          currentComponent.SwapTwoIndices(i+1,j);
        break;
      }
	}
	this->DynkinTypeStrings.TheObjects[indexComponent]=out.str();
}

bool ::DynkinDiagramRootSubalgebra::operator==(const DynkinDiagramRootSubalgebra& right)
{ if (right.SimpleBasesConnectedComponents.size!=
			this->SimpleBasesConnectedComponents.size)
		return false;
	for (int i=0;i<this->SimpleBasesConnectedComponents.size;i++)
	{ bool tempBool=
			(	(	this->SimpleBasesConnectedComponents.TheObjects[i].size==
					right.SimpleBasesConnectedComponents.TheObjects[i].size)
				&&
				(this->DynkinTypeStrings.TheObjects[i]==right.DynkinTypeStrings.TheObjects[i]));
		if (!tempBool)
			return false;
	}
	return true;
}

void DynkinDiagramRootSubalgebra::operator =(const DynkinDiagramRootSubalgebra& right)
{ this->DebugString.assign(right.DebugString);
	this->DynkinTypeStrings.CopyFromBase(right.DynkinTypeStrings);
	this->indicesThreeNodes.CopyFromBase(right.indicesThreeNodes);
	this->SimpleBasesConnectedComponents.CopyFromBase(right.SimpleBasesConnectedComponents);
}

int ::DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes
	(int indexComponent, WeylGroup& theWeyl)
{ roots& currentComponent =
		this->SimpleBasesConnectedComponents.TheObjects[indexComponent];
	int numEnds=0; int result=0;
	this->indicesThreeNodes.TheObjects[indexComponent] =-1;
	this->indicesEnds.TheObjects[indexComponent].size=0;
	for (int i=0;i<currentComponent.size;i++)
	{ int counter=0;
		for (int j=0; j<currentComponent.size;j++)
		{	Rational tempRat;
			theWeyl.RootScalarKillingFormMatrixRoot
				(currentComponent.TheObjects[i],currentComponent.TheObjects[j],tempRat);
			if (tempRat.IsNegative())
				counter++;
		}
		assert(counter<=3);
		if (counter==3)
		{	result++;
			this->indicesThreeNodes.TheObjects[indexComponent]=i;
		}
		if (counter<=1)
		{	numEnds++;
      this->indicesEnds.TheObjects[indexComponent].AddObjectOnTop(i);
		}
	}
	assert(result<=1);
	if (result==1)
		assert(numEnds==3);
	else
		assert(numEnds<=2);
	return result;
}

void rootSubalgebra::SetupE6_3A2(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(6);
	this->genK.TheObjects[0].InitFromIntegers(6, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(6, 1,1,1,1,1,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(6, 1,0,1,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(6, 0,1,1,2,2,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(6, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(6, 0,1,0,1,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_2A2plusA1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(5);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	1,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[4].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A5(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(5);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[4].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A4plusA1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(5);
	this->genK.TheObjects[0].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->genK.TheObjects[4].InitFromIntegers(6, -1, -2, -2, -3, -2, -1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_D5(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(5);
	this->genK.TheObjects[0].InitFromIntegers(6,	0,	1,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[4].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A3plus2A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(5);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6, -1, -2, -2, -3, -2, -1, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[4].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A4(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A3plusA1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_2A2(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A2plus2A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	1,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_4A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6, -1, -2, -2, -3, -2, -1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_D4(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(6,	0,	1,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->genK.TheObjects[3].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A3(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(3);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	1,	0,	0, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A2plusA1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(3);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	1,	0, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_3A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(3);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	1,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[2].InitFromIntegers(6,	0,	0,	0,	0,	0,	1, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A2(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(2);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_2A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(2);
	this->genK.TheObjects[0].InitFromIntegers(6,	0,	0,	1,	0,	0,	0, 0, 0 );
	this->genK.TheObjects[1].InitFromIntegers(6,	0,	1,	0,	0,	0,	0, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

void rootSubalgebra::SetupE6_A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(6);
	this->genK.SetSizeExpandOnTopNoObjectInit(1);
	this->genK.TheObjects[0].InitFromIntegers(6,	1,	0,	0,	0,	0,	0, 0, 0 );
	this->ComputeAll();
	this->ComputeDebugString();
}

/////////////////////////////////////////////////////

void rootSubalgebra::RunE6_3A2(GlobalVariables& theGlobalVariables)
{ this->SetupE6_3A2(theGlobalVariables);
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE7_D6plusA1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(7, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(7, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(7, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(7, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(7, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(7, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(7, 1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE7_A2plusA5(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(7, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(7, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(7, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(7, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(7, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[5].InitFromIntegers(7, 1,1,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(7, 0,1,0,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE7_2A3plusA1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(7, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(7, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(7, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(7, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[4].InitFromIntegers(7, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[5].InitFromIntegers(7, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[6].InitFromIntegers(7, 1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE7_A7(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(7, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(7, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(7, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(7, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(7, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[5].InitFromIntegers(7, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[6].InitFromIntegers(7, 1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE7_D4plus3A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(7, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(7, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(7, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(7, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[4].InitFromIntegers(7, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[5].InitFromIntegers(7, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(7, 1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE7_7A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(7);
	this->genK.SetSizeExpandOnTopNoObjectInit(7);
	this->genK.TheObjects[0].InitFromIntegers(7, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(7, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(7, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[3].InitFromIntegers(7, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[4].InitFromIntegers(7, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[5].InitFromIntegers(7, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(7, 1,1,2,2,1,1,1,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_A8(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 1,3,3,5,4,3,2,1);
	this->genK.TheObjects[1].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(true,theGlobalVariables);
}

void rootSubalgebra::RunE8_D8(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_A1_A7(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 2,3,4,6,5,4,3,2);
	this->genK.TheObjects[1].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_A5_A1_A2(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 2,3,4,6,5,4,3,1);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_2A4(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 1,1,2,3,3,3,2,1);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,0,0,0,0,1,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(true,theGlobalVariables);
}

void rootSubalgebra::RunE8_4A2(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 1,1,2,3,2,1,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,1,0,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 2,3,4,6,5,4,3,1);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_A2_E6(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 2,3,4,6,5,4,3,1);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->genK.TheObjects[2].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,1,0,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_A1_E7(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 2,3,4,6,5,4,3,1);
	this->genK.TheObjects[1].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,1,0,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,0,1,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_D6_2A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 2,3,4,6,5,4,3,2);
	this->genK.TheObjects[7].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_D5_A3(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,1,2,3,3,3,2,1);
	this->genK.TheObjects[6].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,0,1,1);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_2D4(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,0,0,0,0,1,1);
	this->genK.TheObjects[7].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_D4_4A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->genK.TheObjects[3].InitFromIntegers(8, 1,1,2,2,1,1,1,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_2A3_2A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 1,2,2,3,2,2,2,1);
	this->genK.TheObjects[1].InitFromIntegers(8, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 0,0,0,0,0,0,0,1);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunE8_8A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeEn(8);
	this->genK.SetSizeExpandOnTopNoObjectInit(8);
	this->genK.TheObjects[0].InitFromIntegers(8, 0,1,0,1,1,1,1,0);
	this->genK.TheObjects[1].InitFromIntegers(8, 1,1,2,3,3,2,1,0);
	this->genK.TheObjects[2].InitFromIntegers(8, 1,1,2,2,1,1,1,0);
	this->genK.TheObjects[3].InitFromIntegers(8, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[4].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[5].InitFromIntegers(8, 1,2,2,3,2,1,0,0);
	this->genK.TheObjects[6].InitFromIntegers(8, 0,0,0,0,0,1,0,0);
	this->genK.TheObjects[7].InitFromIntegers(8, 2,3,4,6,5,4,3,2);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_B4(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 0,1,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,2,1,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_A3_A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,4,2,1,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_A2_A2(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,1,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,4,3,1,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_C3_A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,1,0,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,4,3,2,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_D4(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,0,0,1,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,2,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,2,1,0,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_B2_2A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 1,0,0,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,2,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,4,3,2,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::RunF4_4A1(GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.MakeF4();
	this->genK.SetSizeExpandOnTopNoObjectInit(4);
	this->genK.TheObjects[0].InitFromIntegers(4, 2,2,1,0,0,0,0,0);
	this->genK.TheObjects[1].InitFromIntegers(4, 0,2,1,0,0,0,0,0);
	this->genK.TheObjects[2].InitFromIntegers(4, 0,0,1,0,0,0,0,0);
	this->genK.TheObjects[3].InitFromIntegers(4, 2,4,3,2,0,0,0,0);
	this->ComputeAll();
	this->ComputeDebugString();
	this->GetLinearCombinationFromMaxRankRootsAndExtraRoot(false,theGlobalVariables);
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRoot
	(bool DoEnumeration, GlobalVariables& theGlobalVariables)
{ int theDimension = this->AmbientWeyl.KillingFormMatrix.NumRows;
	std::stringstream out2;
	std::stringstream out;
	this->ComputeDebugString();
	out2<<this->DebugString<<"\n";
	MatrixLargeRational tempMat;
	this->SimpleBasisK.rootsToMatrix(tempMat);
	tempMat.Invert(theGlobalVariables);
	int counter=0;
	for(int i=0;i<this->AllRoots.size;i++)
	{ root linComb;
		if (this->AllRootsK.IndexOfObject(this->AllRoots.TheObjects[i])==-1)
		{ for (int j=0;j<theDimension;j++)
			{	linComb.TheObjects[j].MakeZero();
				for(int k=0;k<theDimension;k++)
				{ Rational tempRat;
					tempRat.Assign(tempMat.elements[k][j]);
					tempRat.MultiplyBy(this->AllRoots.TheObjects[i].TheObjects[k]);
					linComb.TheObjects[j].Add(tempRat);
				}
			}
			int x= linComb.FindLCMDenominatorsTruncateToInt();
			linComb.MultiplyByInteger(-x);
			std::string tempS;
			if (this->LinCombToString(this->AllRoots.TheObjects[i], x,linComb,tempS))
			{ out<<tempS<<"\n";
				counter++;
				if (this->LowestWeightsGmodK.IndexOfObject(this->AllRoots.TheObjects[i]) !=-1)
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
		this->DoKRootsEnumeration(theGlobalVariables);
	}
//	this->GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2(GlobalVariables& theGlobalVariables)
{	int theDimension = this->AmbientWeyl.KillingFormMatrix.NumRows;
	std::stringstream out;
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
			MatrixLargeRational tempMat;
			tempRoots.rootsToMatrix(tempMat);
			tempMat.Invert(theGlobalVariables);
			for(int i=0;i<this->AllRoots.size;i++)
			{ root linComb;
				if (this->AllRootsK.IndexOfObject(this->AllRoots.TheObjects[i])==-1)
				{ for (int j=0;j<theDimension;j++)
					{	linComb.TheObjects[j].MakeZero();
						for(int k=0;k<theDimension;k++)
						{ Rational tempRat;
							tempRat.Assign(tempMat.elements[k][j]);
							tempRat.MultiplyBy(this->AllRoots.TheObjects[i].TheObjects[k]);
							linComb.TheObjects[j].Add(tempRat);
						}
					}
					int x= linComb.FindLCMDenominatorsTruncateToInt();
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
{	int theDimension = this->AmbientWeyl.KillingFormMatrix.NumRows;
	if (coeff==1)
		return false;
	std::stringstream out;
	std::string tempS;
	alphaRoot.ElementToString(tempS);
	out<<"("<<tempS<<")&$";
	out<<coeff<<"\\alpha_"<<theDimension+1;
	for (int i=0;i<theDimension;i++)
	{	//if (linComb.coordinates[i].IsEqualToZero())
		//	return false;
		linComb.TheObjects[i].ElementToString(tempS);
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
{	int theDimension = this->AmbientWeyl.KillingFormMatrix.NumRows;
	if (coeff==1)
		return false;
	std::stringstream out;
	std::string tempS;
	alphaRoot.ElementToString(tempS);
	out<<"("<<tempS<<")&$";
	out<<coeff<<"\\alpha_"<<theDimension+1;
	for (int i=0;i<theDimension;i++)
	{	//if (linComb.coordinates[i].IsEqualToZero())
		//	return false;
		linComb.TheObjects[i].ElementToString(tempS);
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

void rootSubalgebra::DoKRootsEnumeration(GlobalVariables& theGlobalVariables)
{	this->theKEnumerations.SetSizeExpandOnTopNoObjectInit
		(this->PosRootsKConnectedComponents.size);
	this->theKComponentRanks.SetSizeExpandOnTopNoObjectInit
		(this->PosRootsKConnectedComponents.size);
	for (int i=0;i<this->PosRootsKConnectedComponents.size;i++)
	{ this->theKEnumerations.TheObjects[i].init(this->PosRootsKConnectedComponents.TheObjects[i].size);
		this->PosRootsKConnectedComponents.TheObjects[i].ComputeDebugString();
		this->theKComponentRanks.TheObjects[i]=
			this->PosRootsKConnectedComponents.TheObjects[i].GetRankOfSpanOfElements
				(theGlobalVariables);
	}
	this->DoKRootsEnumerationRecursively(0,theGlobalVariables);
}

void rootSubalgebra::DoKRootsEnumerationRecursively(int indexEnumeration, GlobalVariables& theGlobalVariables)
{ int theRank=this->theKComponentRanks.TheObjects[indexEnumeration];
	int numRuns=MathRoutines::NChooseK
								(	this->PosRootsKConnectedComponents.TheObjects[indexEnumeration].size,
									theRank);
	this->theKEnumerations.TheObjects[indexEnumeration].initNoMemoryAllocation();
	for (int i=0;i<numRuns;i++)
	{ this->theKEnumerations.TheObjects[indexEnumeration]
			.incrementSelectionFixedCardinality(theRank);
		if (indexEnumeration<this->PosRootsKConnectedComponents.size-1)
		{ this->DoKRootsEnumerationRecursively(indexEnumeration+1,theGlobalVariables);
		}
		else
		{ this->KEnumerationsToLinComb(theGlobalVariables);
		}
	}
}

void rootSubalgebra::KEnumerationsToLinComb(GlobalVariables& theGlobalVariables)
{ int theDimension = this->AmbientWeyl.KillingFormMatrix.NumRows;
	static MatrixLargeRational tempMat;
	static Selection tempSelection;
	tempMat.init((short)theDimension,(short)theDimension);
	int counter=0;
	for (int i=0;i<this->PosRootsKConnectedComponents.size;i++)
	{ this->PosRootsKConnectedComponents.TheObjects[i].SelectionToMatrixAppend
			(this->theKEnumerations.TheObjects[i],theDimension,tempMat,counter);
		counter+=this->theKComponentRanks.TheObjects[i];
	}
	//tempMat.ComputeDebugString();
	if (tempMat.Invert(theGlobalVariables))
	{ //tempMat.ComputeDebugString();
		for (int l=0;l<this->TestedRootsAlpha.size;l++)
		{	root linComb;
			root& TestedRootAlpha= this->TestedRootsAlpha.TheObjects[l];
			for (int j=0;j<theDimension;j++)
			{	linComb.TheObjects[j].MakeZero();
				for(int k=0;k<theDimension;k++)
				{ Rational tempRat;
					tempRat.Assign(tempMat.elements[k][j]);
					tempRat.MultiplyBy(TestedRootAlpha.TheObjects[k]);
					linComb.TheObjects[j].Add(tempRat);
				}
			}
			int x= linComb.FindLCMDenominatorsTruncateToInt();
			linComb.MultiplyByInteger(-x);
			bool foundBadCombination=true;
			for (int i=0;i<theDimension;i++)
			{ if (linComb.TheObjects[i].NumShort==-1 ||
						linComb.TheObjects[i].NumShort== 1)
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


void PolynomialRationalCoeff::Evaluate(intRoot &values, Rational &output)
{ output.MakeZero();
	std::string tempS;
	Rational tempLRat, tempLRat2;
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

void IndicatorWindowVariables::Assign(IndicatorWindowVariables& right)
{ this->ProgressReportString1=right.ProgressReportString1;
	this->ProgressReportString2= right.ProgressReportString2;
	this->ProgressReportString3= right.ProgressReportString3;
	this->ProgressReportString4= right.ProgressReportString4;
	this->ProgressReportString5= right.ProgressReportString5;
	this->flagRootIsModified= right.flagRootIsModified;
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

void simplicialCones::initFromDirections(roots &directions,GlobalVariables& theGlobalVariables)
{ Selection tempSel;
	assert(directions.size>0);
	tempSel.init(directions.size);
	roots tempRoots;
	int theDimension;
	if (directions.size==0)
		return;
	else
		theDimension=directions.TheObjects[0].size;
	this->size=0;
	this->ConesHavingFixedNormal.size=0;
	this->theFacets.ClearTheObjects();
	int maxSize=MathRoutines::NChooseK(directions.size,theDimension);
	int maxNumFacets= 2*MathRoutines::NChooseK(directions.size, theDimension-1);
	this->MakeActualSizeAtLeastExpandOnTop(maxSize);
	this->theFacets.MakeActualSizeAtLeastExpandOnTop(maxNumFacets);
	this->ConesHavingFixedNormal.MakeActualSizeAtLeastExpandOnTop(maxNumFacets);
	for (int i=0;i<maxSize;i++)
	{ tempSel.incrementSelectionFixedCardinality(theDimension);
		tempRoots.size=0;
		for (int j=0;j<theDimension;j++)
		{ tempRoots.AddRoot(directions.TheObjects[tempSel.elements[j]]);
		}
		if (		tempRoots.GetRankOfSpanOfElements(theGlobalVariables)==theDimension
				&&	theDimension!=1)
		{	this->SetSizeExpandOnTopNoObjectInit(this->size+1);
			this->TheObjects[this->size-1].ComputeFromDirections(tempRoots,theGlobalVariables,theDimension);
			for (int j=0;j<theDimension;j++)
			{	root tempRoot;
				int tempI= this->size-1;
				tempRoot.Assign(this->TheObjects[tempI].TheObjects[j]);
				tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
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

void  GeneratorsPartialFractionAlgebra::GetMonomialFromExponentAndElongation
				(	intRoot &exponent, int elongation,
					MonomialInCommutativeAlgebra
						<	Integer,GeneratorsPartialFractionAlgebra,
							GeneratorPFAlgebraRecord> &output)
{ output.ClearTheObjects();
	output.Coefficient.Assign(IOne);
	int theDimension= exponent.dimension;
	if (elongation==0 || elongation==-1)
	{ for (int i=0;i<theDimension;i++)
		{ if (exponent.elements[i]!=0)
			{ GeneratorPFAlgebraRecord tempGen;
				tempGen.GeneratorRoot.MakeZero(theDimension); tempGen.GeneratorRoot.elements[i]=1;
				tempGen.Elongation=0;
				int tempI= exponent.elements[i];
				if (elongation==-1) tempI=-tempI;
				output.MultiplyByGenerator(tempGen, tempI);
			}
		}
		if (elongation==-1)
		{ output.Coefficient.MultiplyBy(IMOne);
		}
		return;
	}
	if (elongation==1)
		return;
	GeneratorPFAlgebraRecord tempGen;
	tempGen.GeneratorRoot= exponent;
	tempGen.Elongation= elongation;
	output.MultiplyByGenerator(tempGen,1);
}

GeneratorPFAlgebraRecord::GeneratorPFAlgebraRecord()
{	this->ValueIsComputed=false;
	this->Value = new IntegerPoly;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter++;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
}

GeneratorPFAlgebraRecord::~GeneratorPFAlgebraRecord()
{	delete this->Value; this->Value=0;
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter--;
	if(ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
}

void GeneratorPFAlgebraRecord::operator =(const GeneratorPFAlgebraRecord &right)
{ this->GeneratorRoot = right.GeneratorRoot;
  this->Elongation = right.Elongation;
	this->ValueIsComputed= right.ValueIsComputed;
	this->Value->Assign(*right.Value);
}

int GeneratorsPartialFractionAlgebra::HashFunction()
{ return this->GeneratorIndex;
}

void GeneratorsPartialFractionAlgebra::ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat)
{ this->ElementToString(output,PolyFormat,-1);
}

void GeneratorsPartialFractionAlgebra::ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat, int theDimension)
{	std::string tempS;
	::GeneratorPFAlgebraRecord& currentGen= this->theGenerators.TheObjects[this->GeneratorIndex];
	currentGen.ElementToString(tempS, PolyFormat);
	if(this->GeneratorPower==1)
	{ output.assign(tempS);
		return;
	}
	std::stringstream out;
	out<<"("<<tempS<<")^{"<<this->GeneratorPower<<"}";
	output=out.str();
}

void GeneratorsPartialFractionAlgebra::ConvertToIntegerPoly(IntegerPoly& output, int theDimension)
{ this->theGenerators.TheObjects[this->GeneratorIndex].GetValue(output,theDimension);
	if (this->GeneratorPower<0)
	{ assert(output.size==1);
		assert(output.TheObjects[0].Coefficient.IsEqualTo(IOne));
		static Monomial<Integer> tempM;
		tempM.Coefficient.Assign(IOne);
		tempM.init((short)theDimension);
		for (int i=0;i<theDimension;i++)
		{ tempM.degrees[i]=(short)(output.TheObjects[0].degrees[i]*this->GeneratorPower);
		}
		output.ClearTheObjects();
		output.AddMonomial(tempM);
	}
	if (this->GeneratorPower==0)
	{ output.MakeNVarConst((short)theDimension,IOne);
		assert(false);
	}
	if (this->GeneratorPower>1)
	{ output.RaiseToPower(this->GeneratorPower);
	}
}


bool GeneratorsPartialFractionAlgebra::operator ==(GeneratorsPartialFractionAlgebra &right)
{ return this->GeneratorIndex== right.GeneratorIndex;
}

void GeneratorsPartialFractionAlgebra::operator =(const GeneratorsPartialFractionAlgebra &right)
{ this->GeneratorIndex= right.GeneratorIndex;
	this->GeneratorPower= right.GeneratorPower;
}

void ::PolyPartFractionNumerator::ConvertToIntegerPoly(IntegerPoly &output, int theDimension)
{ output.Nullify((short)theDimension);
	static IntegerPoly tempP;
	static IntegerPoly tempP2;
	for (int i=0;i<this->size;i++)
	{ tempP.MakeNVarConst((short)theDimension,this->TheObjects[i].Coefficient);
		tempP.ComputeDebugString();
		for(int j=0;j<this->TheObjects[i].size;j++)
		{ this->TheObjects[i].TheObjects[j].ConvertToIntegerPoly(tempP2,theDimension);
			tempP2.ComputeDebugString();
			tempP.MultiplyBy(tempP2);
			tempP.ComputeDebugString();
		}
		output.AddPolynomial(tempP);
		output.ComputeDebugString();
	}
}

void GeneratorPFAlgebraRecord::ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat)
{	std::stringstream out1, out2;
	int theDimension= this->GeneratorRoot.dimension;
	for (int i=0;i<theDimension;i++)
	{	if (this->GeneratorRoot.elements[i]!=0)
		{	if (this->GeneratorRoot.elements[i]==1)
				out1<< PolyFormat.alphabet[i];
			else
				out1<< PolyFormat.alphabet[i]<<"^{"<<this->GeneratorRoot.elements[i]<<"}";
		}
	}
	if (this->Elongation==0)
	{ output= out1.str();
		return;
	}
	out2<< "p_{"<< this->Elongation<<"}("<<out1.str()<<")";
	output= out2.str();
}

void GeneratorPFAlgebraRecord::GetValue(IntegerPoly &output, int theDimension)
{ if (this->ValueIsComputed)
	{ output.Assign(*this->Value);
		return;
	}
	output.Nullify((short)theDimension);
	this->ValueIsComputed=true;
	static Monomial<Integer> tempM;
	tempM.init((short)theDimension);
	if (this->Elongation>=0)
		tempM.Coefficient.Assign(IOne);
	else
		tempM.Coefficient.Assign(IMOne);
	int tempI= this->Elongation;
	if (tempI<0) {tempI=-tempI;}
	if (tempI==0){tempI=1;}
	for (int i=0; i<tempI;i++)
	{ for (int j=0;j<theDimension;j++)
		{ if ( this->Elongation<0)
			{ tempM.degrees[j]=(short) (-this->GeneratorRoot.elements[j]*(i+1));
			}
			if( this->Elongation>0)
			{ tempM.degrees[j]=(short) (this->GeneratorRoot.elements[j]*i);
			}
			if( this->Elongation==0)
			{ tempM.degrees[j]=(short) (this->GeneratorRoot.elements[j]);
			}
		}
		output.AddMonomial(tempM);
	}
	this->Value->Assign(output);
}

void PolyPartFractionNumeratorLight::AssignPolyPartFractionNumerator(PolyPartFractionNumerator &from)
{ this->SetSizeExpandOnTopLight(from.size);
	this->Coefficients.SetSizeExpandOnTopLight(from.size);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].CopyFromHeavy(from.TheObjects[i]);
		this->Coefficients.TheObjects[i]=from.TheObjects[i].Coefficient.value;
	}
}

void PolyPartFractionNumeratorLight::ComputePolyPartFractionNumerator(PolyPartFractionNumerator &output, int theDimension)
{ output.Nullify((short)theDimension);
	output.MakeActualSizeAtLeastExpandOnTop(this->size);
	for (int i=0;i<this->size;i++)
	{ ::MonomialInCommutativeAlgebra<Integer, GeneratorsPartialFractionAlgebra,GeneratorPFAlgebraRecord>
				tempM;
		tempM.Coefficient.value= this->Coefficients.TheObjects[i];
		for (int j=0;j<this->TheObjects[i].size;j++)
			tempM.MultiplyByGenerator(this->TheObjects[i].TheObjects[j]);
		output.AddMonomial(tempM);
	}
}

int PolyPartFractionNumeratorLight::NumGeneratorsUsed()
{ int result=0;
	for (int i=0;i<this->size;i++)
	{ result+=this->TheObjects[i].size;
	}
	return result;
}

void PolyPartFractionNumeratorLight::AssignPolyPartFractionNumeratorLight
	(const PolyPartFractionNumeratorLight &right)
{ this->CopyFromLight(right);
	this->Coefficients.CopyFromLight(right.Coefficients);
}

bool affineCone::SplitByAffineHyperplane(affineHyperplane& theKillerPlane, affineCones &output)
{
	return true;
}

bool affineCone::WallIsInternalInCone(affineHyperplane &theKillerCandidate)
{  return true;
}

inline int affineCone::GetDimension()
{ if (this->theWalls.size==0)
		return 0;
	return this->theWalls.TheObjects[0].affinePoint.size;
}

inline int affineCone::HashFunction()
{ int tempMin = ::Minimum(this->theWalls.size, ::SomeRandomPrimesSize);
	int result=0;
	for (int i=0;i<tempMin;i++)
	{ result+= this->theWalls.TheObjects[i].HashFunction()*::SomeRandomPrimes[i];
	}
	return result;
}

void MatrixLargeRational::ComputePotentialChangeGradient
	(	MatrixLargeRational& matA, Selection& BaseVariables, int NumTrueVariables,
		int ColumnIndex, Rational &outputChangeGradient, bool &hasAPotentialLeavingVariable)
{ hasAPotentialLeavingVariable = false;
	outputChangeGradient.MakeZero();
	for (int j=0;j<matA.NumRows;j++)
	{	if (BaseVariables.elements[j]>=NumTrueVariables)
			outputChangeGradient.Add(matA.elements[j][ColumnIndex]);
		hasAPotentialLeavingVariable =
			hasAPotentialLeavingVariable ||
				matA.elements[j][ColumnIndex].IsPositive();
	}
	if (ColumnIndex>=NumTrueVariables)
		outputChangeGradient.Subtract(ROne);
}

void MatrixLargeRational::GetMaxMovementAndLeavingVariableRow
	(	Rational &maxMovement, int& LeavingVariableRow, int EnteringVariable,
		int NumTrueVariables, MatrixLargeRational& tempMatA, MatrixLargeRational& matX,
		Selection& BaseVariables)
{	LeavingVariableRow=-1;
	maxMovement.MakeZero();
	for(int i=0; i<tempMatA.NumRows;i++)
	{ Rational tempRat;
		tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
		if (tempRat.IsPositive())
		{ tempRat.Invert();
			tempRat.MultiplyBy(matX.elements[BaseVariables.elements[i]][0]);
			if (maxMovement.IsGreaterThan(tempRat)|| (LeavingVariableRow==-1 ))
			{ maxMovement.Assign(tempRat);
				LeavingVariableRow=i;
			}
		}
	}
}

bool MatrixLargeRational
	::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
	(	MatrixLargeRational& matA, MatrixLargeRational& matb,
		MatrixLargeRational& outputPoint,
		GlobalVariables& theGlobalVariables)
//this function return true if Ax=b>=0 has a solution with x>=0
//and records a solution x at outputPoint
//else returns false,
//where b is a given nonnegative column vector, A is an n by m matrix
//and x is a column vector with m entries
{	MatrixLargeRational& tempMatA=theGlobalVariables.matSimplexAlgorithm1;
	MatrixLargeRational& matX=theGlobalVariables.matSimplexAlgorithm2;
	MatrixLargeRational& tempDebugMat=theGlobalVariables.matSimplexAlgorithm3;
	//MatrixLargeRational& tempMatb=theGlobalVariables.matSimplexAlgorithm3;
	Selection& BaseVariables = theGlobalVariables.selSimplexAlg2;
	Rational GlobalGoal; GlobalGoal.MakeZero();
	assert (matA.NumRows== matb.NumRows);
	for (int j=0;j<matb.NumRows;j++)
  { GlobalGoal.Add(matb.elements[j][0]);
		assert(!matb.elements[j][0].IsNegative());
	}
	if (GlobalGoal.IsEqualToZero())
		return false;
	int NumTrueVariables=matA.NumCols;
	//tempMatb.Assign(matb);
	tempMatA.init(matA.NumRows, NumTrueVariables+matA.NumRows);
	matX.init(tempMatA.NumCols,1);
	HashedListBasicObjects<Selection>& VisitedVertices=
		theGlobalVariables.hashedSelSimplexAlg;
	VisitedVertices.ClearTheObjects();
	BaseVariables.init(tempMatA.NumCols);
	tempMatA.NullifyAll();	matX.NullifyAll();
	for (int j=0;j<matA.NumCols;j++)
		for (int i=0;i<matA.NumRows;i++)
			tempMatA.elements[i][j].Assign(matA.elements[i][j]);
	for (int j=0;j<matA.NumRows;j++)
	{ tempMatA.elements[j][j+NumTrueVariables].MakeOne();
		matX.elements[j+NumTrueVariables][0].Assign(matb.elements[j][0]);
		BaseVariables.AddSelectionAppendNewIndex(j+NumTrueVariables);
	}
	if (::MatrixLargeRational::flagAnErrorHasOccurredTimeToPanic)
	{	tempDebugMat.Assign(tempMatA);
		tempMatA.ComputeDebugString(); matX.ComputeDebugString();
	}
	Rational	PotentialChangeGradient, ChangeGradient;//Change, PotentialChange;
	int EnteringVariable=0;
	bool WeHaveNotEnteredACycle=true;
//	int ProblemCounter=0;
	while (EnteringVariable!=-1 && WeHaveNotEnteredACycle && GlobalGoal.IsPositive())
	{//	ProblemCounter++;
	//	if (ProblemCounter==8)
		//{ BaseVariables.ComputeDebugString();
		//}
		//tempMatA.ComputeDebugString(); matX.ComputeDebugString();
		EnteringVariable=-1; ChangeGradient.MakeZero();
		for (int i=0;i<tempMatA.NumCols;i++)
		{ if (!BaseVariables.selected[i])
			{ Rational PotentialChangeGradient; bool hasAPotentialLeavingVariable;
				MatrixLargeRational::ComputePotentialChangeGradient
					(	tempMatA,BaseVariables,NumTrueVariables,i,
						PotentialChangeGradient,hasAPotentialLeavingVariable);
				if (PotentialChangeGradient.IsGreaterThanOrEqualTo(ChangeGradient) &&
						hasAPotentialLeavingVariable)
				{ EnteringVariable= i;
					ChangeGradient.Assign(PotentialChangeGradient);
				}
			}
		}
		if (EnteringVariable!=-1)
		{	int LeavingVariableRow;	Rational MaxMovement;
			MatrixLargeRational::GetMaxMovementAndLeavingVariableRow
				(	MaxMovement,LeavingVariableRow,EnteringVariable, NumTrueVariables,
					tempMatA, matX, BaseVariables);
			Rational tempRat, tempTotalChange;
			assert(!tempMatA.elements[LeavingVariableRow][EnteringVariable].IsEqualToZero());
			tempRat.Assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
			tempRat.Invert();
  //    if (BaseVariables.elements[LeavingVariableRow]==34)
	//			tempMatA.ComputeDebugString();
			for (int i=0;i<tempMatA.NumRows;i++)
				assert(tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(ROne));
			tempMatA.RowTimesScalar(LeavingVariableRow,tempRat);
      //if (BaseVariables.elements[LeavingVariableRow]==34)
			//	tempMatA.ComputeDebugString();
			tempTotalChange.Assign(MaxMovement);
			tempTotalChange.MultiplyBy(ChangeGradient);
			matX.elements[EnteringVariable][0].Add(MaxMovement);
			if (!tempTotalChange.IsEqualToZero())
			{	VisitedVertices.ClearTheObjects();
				GlobalGoal.Subtract(tempTotalChange);
			}
			else
			{ //BaseVariables.ComputeDebugString();
				int tempI= VisitedVertices.ContainsObjectHash(BaseVariables);
        if (tempI==-1)
					VisitedVertices.AddObjectOnTopHash(BaseVariables);
        else
					WeHaveNotEnteredACycle=false;
      }
      //if (BaseVariables.elements[LeavingVariableRow]==34)
			//	tempMatA.ComputeDebugString();
			for (int i=0;i<tempMatA.NumRows;i++)
			{	if (!tempMatA.elements[i][EnteringVariable].IsEqualToZero()&& i!=LeavingVariableRow)
				{	tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
					tempRat.MultiplyBy(MaxMovement);
					matX.elements[BaseVariables.elements[i]][0].Subtract(tempRat);
					tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
					tempRat.Minus();
					tempMatA.AddTwoRows(LeavingVariableRow,i,0,tempRat);
				}
				if (i==LeavingVariableRow)
					matX.elements[BaseVariables.elements[i]][0].MakeZero();
				//tempMatA.ComputeDebugString();
				//matX.ComputeDebugString();
			}
			assert
				(	matX.elements[BaseVariables.elements[LeavingVariableRow]][0]
						.IsEqualToZero());
			BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]]=false;
			BaseVariables.elements[LeavingVariableRow]= EnteringVariable;
			BaseVariables.selected[EnteringVariable]= true;
			//BaseVariables.ComputeDebugString();
			for (int i=0;i<tempMatA.NumRows;i++)
				assert(tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(ROne));
		}
//		if (::MatrixLargeRational::flagAnErrorHasOccurredTimeToPanic)
//		{ MatrixLargeRational tempMat;
//			tempMat.Assign(matX);
//			tempMat.ComputeDebugString();
//			tempDebugMat.ComputeDebugString();
//			tempMat.MultiplyOnTheLeft(tempDebugMat);
//			tempMat.ComputeDebugString();
//			assert(tempMat.IsEqualTo(matb));
//		}
	}
//	std::string tempS;
//	std::stringstream out;
//	for (int i=0;i<BaseVariables.CardinalitySelection;i++)
//	{ int tempI=BaseVariables.elements[i];
//		matX.elements[tempI][0].ElementToString(tempS);
//		out << tempS <<"(";
//		if (tempI<matA.NumCols)
//		{	for (int j=0;j<matA.NumRows;j++)
//			{ matA.elements[j][tempI].ElementToString(tempS);
//				out << tempS;
//				if (j!=matA.NumRows-1)
//					out <<",";
//			}
//		} else
//			out<<"dummy column " << i <<" ";
//		out <<")";
//		if (i!=BaseVariables.CardinalitySelection-1)
//			out <<"+";
//	}
//	tempS=out.str();
	for(int i=NumTrueVariables;i<matX.NumRows;i++)
		if (matX.elements[i][0].IsPositive())
			return false;
	outputPoint.Resize(NumTrueVariables,1,false);
	for (int i=0;i<NumTrueVariables;i++)
		outputPoint.elements[i][0].Assign(matX.elements[i][0]);
	return true;
}

//this function return true if Ax<=b has a solution
//and records a point at outputPoint
//else returns false
bool MatrixLargeRational::SystemLinearInequalitiesHasSolution
	(MatrixLargeRational& matA, MatrixLargeRational& matb, MatrixLargeRational& outputPoint)
{/* static MatrixLargeRational tempMatA;
	static MatrixLargeRational matX;
	static short NumTrueVariables;
	short numExtraColumns=0;
	static Rational GlobalGoal;
	GlobalGoal.MakeZero();
	assert (matA.NumRows== matb.NumRows);
	for (int j=0;j<matb.NumRows;j++)
    if (matb.elements[j][0].IsNegative())
      numExtraColumns++;
	NumTrueVariables= matA.NumCols;
	tempMatA.init(matA.NumRows, NumTrueVariables*2+matA.NumRows+numExtraColumns);
	matX.init(tempMatA.NumCols,1);
	static HashedListBasicObjects<Selection> VisitedVertices;
	VisitedVertices.ClearTheObjects();
	static Selection NonZeroSlackVariables;
	static Selection BaseVariables;
	BaseVariables.init(tempMatA.NumCols);
	tempMatA.NullifyAll();
	matX.NullifyAll();
	for (int j=0;j<matA.NumCols;j++)
	{	for (int i=0;i<matA.NumRows;i++)
		{	tempMatA.elements[i][j].Assign(matA.elements[i][j]);
			tempMatA.elements[i][j+NumTrueVariables].Assign(matA.elements[i][j]);
			tempMatA.elements[i][j+NumTrueVariables].Minus();
		}
	}
	int LowestBadIndex= tempMatA.NumCols- numExtraColumns;
	short tempCounter=0;
	for (int j=0;j<matA.NumRows;j++)
	{ tempMatA.elements[j][j+NumTrueVariables*2].MakeOne();
		if (matb.elements[j][0].IsNonNegative())
		{	matX.elements[j+NumTrueVariables*2][0].Assign(matb.elements[j][0]);
			BaseVariables.AddSelectionAppendNewIndex(j+NumTrueVariables*2);
		}
		else
		{	int tempI=NumTrueVariables*2+matA.NumRows+tempCounter;
			matX.elements[tempI][0].Assign(matb.elements[j][0]);
			matX.elements[tempI][0].Minus();
			GlobalGoal.Add(matX.elements[tempI][0]);
			tempMatA.elements[j][tempI].Assign(RMOne);
			tempMatA.RowTimesScalar(j,RMOne);
			BaseVariables.AddSelectionAppendNewIndex(tempI);
			tempCounter++;
		}
	}
	tempMatA.ComputeDebugString(); matX.ComputeDebugString();
	static Rational	PotentialChangeGradient, ChangeGradient;//Change, PotentialChange;
	int EnteringVariable=0;
	bool WeHaveNotEnteredACycle=true;
	while (EnteringVariable!=-1 && WeHaveNotEnteredACycle && GlobalGoal.IsPositive())
	{ EnteringVariable=-1;
		ChangeGradient.MakeZero();
		for (int i=0;i<tempMatA.NumCols;i++)
		{ BaseVariables.ComputeDebugString();
			if (!BaseVariables.selected[i])
			{ PotentialChangeGradient.MakeZero();
				bool hasAPotentialLeavingVariable=false;
				for (int j=0;j<tempMatA.NumRows;j++)
				{	if (BaseVariables.elements[j]>=LowestBadIndex)
					{	static Rational tempRat;
						tempRat.Assign(tempMatA.elements[j][i]);
						PotentialChangeGradient.Add(tempRat);
					}
					hasAPotentialLeavingVariable=hasAPotentialLeavingVariable|| tempMatA.elements[j][i].IsPositive();
				}
				if (i>=LowestBadIndex)
					PotentialChangeGradient.Subtract(ROne);
				if (PotentialChangeGradient.IsGreaterThanOrEqualTo(ChangeGradient) && hasAPotentialLeavingVariable)
				{ EnteringVariable= i;
					ChangeGradient.Assign(PotentialChangeGradient);
				}
			}
		}
		if (EnteringVariable!=-1)
		{	int LeavingVariableRow=-1;
			static Rational	MaxMovement;
			MaxMovement.MakeZero();
			for(int i=0; i<tempMatA.NumRows;i++)
			{ static Rational tempRat;
				tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
				if (tempRat.IsPositive() && BaseVariables.elements[i]>=NumTrueVariables)
				{ tempRat.Invert();
					tempRat.MultiplyBy(matX.elements[BaseVariables.elements[i]][0]);
					if (MaxMovement.IsGreaterThan(tempRat)|| (LeavingVariableRow==-1 ))
					{ MaxMovement.Assign(tempRat);
						LeavingVariableRow=i;
					}
				}
			}
			static Rational tempRat, tempTotalChange;
			assert(!tempMatA.elements[LeavingVariableRow][EnteringVariable].IsEqualToZero());
			tempRat.Assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
			tempRat.Invert();
			tempMatA.RowTimesScalar(LeavingVariableRow,tempRat);
			tempTotalChange.Assign(MaxMovement);
			tempTotalChange.MultiplyBy(ChangeGradient);
			matX.elements[EnteringVariable][0].Add(MaxMovement);
			if (!tempTotalChange.IsEqualToZero())
			{	VisitedVertices.ClearTheObjects();
				GlobalGoal.Subtract(tempTotalChange);
			}
			else
			{ int tempI= VisitedVertices.ContainsObjectHash(BaseVariables);
        if (tempI==-1)
					VisitedVertices.AddObjectOnTopHash(BaseVariables);
        else
					WeHaveNotEnteredACycle=false;
      }
			for (int i=0;i<tempMatA.NumRows;i++)
			{	tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
				tempRat.MultiplyBy(MaxMovement);
				matX.elements[BaseVariables.elements[i]][0].Subtract(tempRat);
				if (!tempMatA.elements[i][EnteringVariable].IsEqualToZero()&& i!=LeavingVariableRow)
				{ tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
					tempRat.Minus();
					tempMatA.AddTwoRows(LeavingVariableRow,i,0,tempRat);
				}
				tempMatA.ComputeDebugString();
				matX.ComputeDebugString();
			}
			BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]]=false;
			BaseVariables.elements[LeavingVariableRow]= EnteringVariable;
			BaseVariables.selected[EnteringVariable]= true;
			BaseVariables.ComputeDebugString();
		}
	}
	for(int i=LowestBadIndex;i<matX.NumRows;i++)
	{ if (matX.elements[i][0].IsPositive())
			return false;
	}
	outputPoint.Resize(NumTrueVariables,1,false);
	for (int i=0;i<NumTrueVariables;i++)
	{	outputPoint.elements[i][0].Assign(matX.elements[i][0]);
		outputPoint.elements[i][0].Subtract(matX.elements[i+NumTrueVariables][0]);
	}*/
	return true;
}

void affineCone::ProjectFromCombinatorialChamber(CombinatorialChamber& input)
{/* this->theWalls.SetSizeExpandOnTopNoObjectInit(input.ExternalWallsNormals.size);
	for(int i=0;i<this->theWalls.size;i++)
	{ if (!this->theWalls.TheObjects[i].
					ProjectFromFacetNormal
						(input.ExternalWallsNormals.TheObjects[i]))
		{ this->theWalls.PopIndexSwapWithLast(i);
			i--;
		}
	}*/
}

inline bool affineHyperplane::operator ==(const affineHyperplane& right)
{ root tempRoot1, tempRoot2;
	tempRoot1.Assign(this->normal);
	tempRoot2.Assign(right.normal);
	tempRoot1.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
	tempRoot2.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
	if (!tempRoot1.IsEqualTo(tempRoot2))
		return false;
	Rational tempRat1, tempRat2;
	root::RootScalarEuclideanRoot(tempRoot1, this->affinePoint, tempRat1);
	root::RootScalarEuclideanRoot(tempRoot1, right.affinePoint, tempRat2);
	return tempRat1.IsEqualTo(tempRat2);
}


bool affineHyperplane::ProjectFromFacetNormal(root& input)
{ Rational tempRat;
	int tempI=input.getIndexFirstNonZeroCoordinate();
	assert(tempI!=-1);
	if (tempI==input.size-1)
		return false;
	this->affinePoint.MakeZero(input.size);
	this->affinePoint.SetSizeExpandOnTopLight(input.size-1);
	this->affinePoint.TheObjects[tempI].Assign(input.TheObjects[input.size-1]);
	this->affinePoint.TheObjects[tempI].Minus();
	this->affinePoint.TheObjects[tempI].DivideBy(input.TheObjects[tempI]);
	this->normal.Assign(input);
	this->normal.SetSizeExpandOnTopLight(input.size-1);
	return true;
}

bool affineHyperplane::ContainsPoint(root& thePoint)
{ Rational tempRat1, tempRat2;
  root::RootScalarEuclideanRoot(this->normal,thePoint,tempRat1);
  root::RootScalarEuclideanRoot(this->normal,this->affinePoint,tempRat2);
  return tempRat2.IsEqualTo(tempRat1);
}

bool affineHyperplane::HasACommonPointWithPositiveTwoToTheNth_ant()
{ Rational tempRat;
	root::RootScalarEuclideanRoot(this->normal,this->affinePoint, tempRat);
	if (tempRat.IsEqualToZero())
		return true;
	for(int i=0;i<this->normal.size;i++)
	{ Rational& tempRat2= this->normal.TheObjects[i];
		if (tempRat.IsNegative() && tempRat2.IsNegative())
			return true;
		if (tempRat.IsPositive() && tempRat2.IsPositive())
			return true;
	}
	return false;
}

void affineHyperplane::MakeFromNormalAndPoint(root& inputPoint, root& inputNormal)
{ this->affinePoint.Assign(inputPoint);
	this->normal.Assign(inputNormal);
}

void affineHyperplane::ElementToString(std::string& output)
{ std::stringstream out;
	std::string tempS;
	this->affinePoint.ElementToString(tempS);
	out<< "point: " << tempS;
	this->normal.ElementToString(tempS);
	out<<" normal: "<< tempS;
	output= out.str();
}

int affineHyperplane::HashFunction()
{ //warning: if normal gets streched, the hashfunction will change!
	this->normal.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
	Rational tempRat;
	root::RootScalarEuclideanRoot(this->normal, this->affinePoint, tempRat);
	return this->normal.HashFunction()+ tempRat.HashFunction();
}

void affineCones::ProjectFromCombinatorialChambers(CombinatorialChamberContainer &input)
{ this->MakeActualSizeAtLeastExpandOnTop(input.size);
	affineCone tempCone;
	for(int i=0;i<input.size;i++)
	{ tempCone.ProjectFromCombinatorialChamber(*input.TheObjects[i]);
		this->AddObjectOnTopHash(tempCone);
	}
}

//void FacetPointers::ConvertToAffineAndProjectivize
//	(CombinatorialChamberContainer& input, CombinatorialChamberContainer& ownerComplex)
/*{ assert(this!=&input.theHyperplanes);
	this->initAndCreateNewObjects(input.theHyperplanes.size);
	input.ComputeDebugString();
	input.theHyperplanes.ComputeDebugString();
	for (int i=0;i<input.theHyperplanes.size;i++)
	{ this->TheObjects[i]->InduceFromFacetLowerDimension
			(*input.theHyperplanes.TheObjects[i],input);
	}
	root extraWallNormal;
	extraWallNormal.MakeZero(root::AmbientDimension+1);
	extraWallNormal.TheObjects[root::AmbientDimension].MakeOne();
	this->NormalizeRootAndGetFacetCreateNewIfNeeded(extraWallNormal);*/
//}

void affineHyperplanes::ElementToString(std::string& output)
{ std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ std::string tempS;
		this->TheObjects[i].ElementToString(tempS);
		out<<"index: "<<i<<" "<< tempS<<"\n";
	}
	output= out.str();
}

void multTableKmods::ElementToString(std::string& output, rootSubalgebra& owner)
{ this->ElementToString(output,false,owner);
}

void multTableKmods::ElementToString(std::string& output, bool useLaTeX, rootSubalgebra& owner)
{	std::stringstream out;
	out <<"\t";
	if (!useLaTeX)
    for (int i=0; i<this->size;i++)
      out << i<<"\t";
  else
  { out <<"\\begin{tabular}{c|";
    for (int i=0; i<this->size;i++)
      out << "c";
    out << "|} & ";
    for (int i=0; i<this->size;i++)
    { out << i;
      if (i!=this->size-1)
        out <<"&";
    }
    out<<"\\\\\\hline";
  }
	for (int i=0; i<this->size;i++)
	{ out <<"\n"<<i;
    if (useLaTeX)
      out<<" & ";
    else
      out <<"\t";
		for (int j=0;j<this->TheObjects[i].size;j++)
		{	if ((j==owner.CentralizerRoots.size-1) && (i<=owner.CentralizerRoots.size-1))
        out<<"\\multicolumn{1}{c|}{";
      for (int k=0;k<this->TheObjects[i].TheObjects[j].size;k++)
				out << this->TheObjects[i].TheObjects[j].TheObjects[k] << ", ";
			if ((j==owner.CentralizerRoots.size-1) && (i<=owner.CentralizerRoots.size-1))
        out<<"}";
      if (useLaTeX )
      { if (j!=this->TheObjects[i].size-1)
          out <<" & ";
      }
			else
        out << "\t";
		}
		if (useLaTeX)
    { out <<"\\\\";
      if (i==owner.CentralizerRoots.size-1)
        out<<"\\cline{2-" << owner.CentralizerRoots.size+1<<"}";
    }
	}
  if (useLaTeX)
    out << "\n\\hline opposite & ";
  else
    out << "\t";
  for (int i=0;i<owner.theOppositeKmods.size;i++)
  { out << i<<"/"<< owner.theOppositeKmods.TheObjects[i];
    if (i!=owner.theOppositeKmods.size-1)
    { if (useLaTeX)
        out<<" & ";
      else
        out <<"\t";
    }
  }
	if (useLaTeX)
    out <<"\n\\end{tabular}";
	output=out.str();
}

rootSubalgebra::rootSubalgebra()
{ this->flagAnErrorHasOccuredTimeToPanic=false;
  this->NumGmodKtableRowsAllowedLatex=35;
}

/*void SelectionList::ElementToString(std::string &output)
{ for ()
}*/
void rootsWithMultiplicitiesContainer::ElementToString(std::string &output)
{ std::stringstream out;
	std::string tempS;
	out << "Num Elements: "<< this->size<<"    \n";
	for (int i=0; i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS);
		out<< tempS<<"\n";
	}
	output=out.str();
}

void rootWithMultiplicity::ElementToString(std::string& output)
{ std::stringstream out;
	std::string tempS;
	out<< this->multiplicity <<" (";
	this->::root::ElementToString(tempS);
	out << tempS<<")";
	output=out.str();
}

void rootsWithMultiplicity::ElementToString(std::string &output)
{ std::string tempS;
	std::stringstream out;
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempS);
		out <<tempS;
		if (i!=this->size-1)
			out <<" + ";
	}
	output=out.str();
}

void coneRelation::ElementToString(std::string &output, rootSubalgebra& owner)
{ std::string tempS;
	std::stringstream out;
	assert(this->AlphaCoeffs.size==this->Alphas.size);
	assert(this->BetaCoeffs.size==this->Betas.size);
	for (int i=0;i<this->Alphas.size;i++)
	{	this->AlphaCoeffs.TheObjects[i].ElementToString(tempS);
		if (tempS=="1")	tempS="";
		if (tempS=="-1") tempS="-";
		assert(!(tempS=="0"));
		out<< tempS;
		this->Alphas.TheObjects[i].ElementToString(tempS);
		out <<"("<<tempS<<")";
		if (i!=this->Alphas.size-1)
			out <<" + ";
	}
	out << "=";
	for (int i=0;i<this->Betas.size;i++)
	{	this->BetaCoeffs.TheObjects[i].ElementToString(tempS);
		if (tempS=="1")	tempS="";
		if (tempS=="-1") tempS="-";
		assert(!(tempS=="0"));
		out<< tempS;
		this->Betas.TheObjects[i].ElementToString(tempS);
		out <<"("<<tempS<<")";
		if (i!=this->Betas.size-1)
			out <<" + ";
	}
	this->ComputeStringConnectedComponents(owner,this->stringConnectedComponents);
	out <<this->stringConnectedComponents<<"\n";
	this->theDiagram.ElementToString(tempS);
	out <<"\n"<<tempS;
	output=out.str();
}

void coneRelation::ComputeStringConnectedComponents
	(rootSubalgebra& owner, std::string& output)
{ std::string tempS;
	std::stringstream out;
		owner.coneRelationsBuffer.SetSizeExpandOnTopNoObjectInit
		(owner.theDynkinDiagram.sameTypeComponents.size);
	owner.coneRelationsNumSameTypeComponentsTaken.SetSizeExpandOnTopNoObjectInit
		(owner.theDynkinDiagram.sameTypeComponents.size);
	for (int i=0;i<owner.theDynkinDiagram.sameTypeComponents.size;i++)
	{ owner.coneRelationsNumSameTypeComponentsTaken.TheObjects[i]=-1;
		owner.coneRelationsBuffer.TheObjects[i].SetSizeExpandOnTopNoObjectInit
			(owner.theDynkinDiagram.sameTypeComponents.TheObjects[i].size);
		for (int j=0; j<owner.coneRelationsBuffer.TheObjects[i].size;j++)
			owner.coneRelationsBuffer.TheObjects[i].TheObjects[j]=-1;
	}
	this->stringByConnectedComponents(this->Alphas,tempS,owner);
	out <<tempS;
	this->stringByConnectedComponents(this->Betas,tempS,owner);
	out << tempS;
	output=out.str();
}

void coneRelation::stringByConnectedComponents(roots& input, std::string& output, rootSubalgebra& owner)
{	std::stringstream out;
	for (int i=0;i<input.size;i++)
	{ bool foundAnotherComponent=false;
		for (int j=0; j<owner.theDynkinDiagram.SimpleBasesConnectedComponents.size;j++)
			if (owner.theDynkinDiagram.SimpleBasesConnectedComponents.TheObjects[j]
						.ContainsARootConnectedTo(input.TheObjects[i],owner.AmbientWeyl))
			{	if (foundAnotherComponent)
					out<<"+";
				foundAnotherComponent=true;
				out << owner.theDynkinDiagram.DynkinTypeStrings.TheObjects[j];
				int& index =
					owner.coneRelationsBuffer.TheObjects
						[owner.theDynkinDiagram.indexUniComponent.TheObjects[j]].TheObjects
							[owner.theDynkinDiagram.indexInUniComponent.TheObjects[j]];
				int& numPrimes=owner.coneRelationsNumSameTypeComponentsTaken.TheObjects
					[owner.theDynkinDiagram.indexUniComponent.TheObjects[j]];
				if (index==-1)
				{	numPrimes++;
					index=numPrimes;
				}
				for (int k=0;k<numPrimes;k++)
					out<<"'";
			}
		out <<",\t";
	}
	output=out.str();
}


void coneRelation::GetSumAlphas(root &output, int theDimension)
{ assert(this->AlphaCoeffs.size==this->Alphas.size);
	output.MakeZero(theDimension);
	root tempRoot;
	for(int i=0;i<this->Alphas.size;i++)
	{ tempRoot.Assign(this->Alphas.TheObjects[i]);
		tempRoot.MultiplyByLargeRational(this->AlphaCoeffs.TheObjects[i]);
		output.Add(tempRoot);
	}
}

void coneRelation::SortRelation(rootSubalgebra& owner)
{ this->ComputeKComponents(this->Alphas,this->AlphaKComponents,owner);
	this->ComputeKComponents(this->Betas, this->BetaKComponents, owner);
	//bubble sort
	for (int i=0;i<this->Alphas.size;i++)
		for (int j=i+1; j<this->Alphas.size;j++)
			if (this->leftSortedBiggerThanOrEqualToRight
				(this->AlphaKComponents.TheObjects[j],this->AlphaKComponents.TheObjects[i]))
			{	this->AlphaCoeffs.SwapTwoIndices(i,j);
				this->Alphas.SwapTwoIndices(i,j);
				this->AlphaKComponents.SwapTwoIndices(i,j);
			}
	for (int i=0;i<this->Betas.size;i++)
		for (int j=i+1; j<this->Betas.size;j++)
			if (this->leftSortedBiggerThanOrEqualToRight
				(this->BetaKComponents.TheObjects[j],this->BetaKComponents.TheObjects[i]))
			{	this->BetaCoeffs.SwapTwoIndices(i,j);
				this->Betas.SwapTwoIndices(i,j);
				this->BetaKComponents.SwapTwoIndices(i,j);
			}
}

void coneRelation::ComputeKComponents
	(roots& input, ListBasicObjects<ListBasicObjects<int> >& output, rootSubalgebra& owner)
{	output.SetSizeExpandOnTopNoObjectInit(input.size);
	for(int i=0; i<input.size;i++)
	{ output.TheObjects[i].size=0;
		for(int j=0;j<owner.theDynkinDiagram.SimpleBasesConnectedComponents.size;j++)
			if (	owner.theDynkinDiagram.SimpleBasesConnectedComponents.TheObjects[j].
							ContainsARootConnectedTo( input.TheObjects[i],owner.AmbientWeyl))
				output.TheObjects[i].AddObjectOnTop(j);
	}
}

bool coneRelation::leftSortedBiggerThanOrEqualToRight
	(ListBasicObjects<int>& left,ListBasicObjects<int>& right)
{ if (left.size>right.size)
		return true;
	if (right.size>left.size)
		return false;
	for(int i=0;i<right.size;i++)
	{	if (right.TheObjects[i]>left.TheObjects[i])
			return false;
		if (left.TheObjects[i]>right.TheObjects[i])
			return true;
	}
	return true;
}

bool coneRelation::isIsomorphicTo(coneRelation& right, rootSubalgebra& owner)
{ this->ComputeDebugString(owner);
	right.ComputeDebugString(owner);
	return this->stringConnectedComponents==right.stringConnectedComponents;
}

void coneRelations::AddRelationNoRepetition(coneRelation& input,rootSubalgebra& owner)
{ for (int i=0;i<this->size;i++)
		if(	this->TheObjects[i].isIsomorphicTo(input,owner))
			return;
	this->AddObjectOnTop(input);
}

void permutation::initPermutation(int n)
{ this->initIncomplete(n);
  for(int i=0;i<n;i++)
  { this->MaxMultiplicities.TheObjects[i]=n-i-1;
    this->Multiplicities.TheObjects[i]=0;
  }
}

void permutation::initPermutation(ListBasicObjects<int>& disjointSubsets, int TotalNumElements)
{ this->initIncomplete(TotalNumElements);
  for(int i=0;i<disjointSubsets.size;i++)
  { for (int j=0;j<disjointSubsets.TheObjects[i];j++)
    { this->MaxMultiplicities.TheObjects[i]=disjointSubsets.TheObjects[i]-j-1;
      this->Multiplicities.TheObjects[i]=0;
    }
    TotalNumElements-=disjointSubsets.TheObjects[i];
  }
  assert(TotalNumElements==0);
}

void permutation::incrementAndGetPermutation(ListBasicObjects<int>& output)
{ this->IncrementSubset();
  this->GetPermutation(output);
}

void permutation::GetPermutation(ListBasicObjects<int>& output)
{ int numElements=this->Multiplicities.size;
  output.SetSizeExpandOnTopNoObjectInit(numElements);
  for (int i=0; i<numElements;i++)
    output.TheObjects[i]=i;
  for (int i=0;i<numElements;i++)
    MathRoutines::swap(output.TheObjects[i],output.TheObjects[i+this->Multiplicities.TheObjects[i]]);
}
