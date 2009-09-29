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


#ifndef polyhedra_h_already_included
#define polyhedra_h_already_included
#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>


#pragma warning(disable:4100)//warning C4100: non-referenced formal parameter
#pragma warning(disable:4189)//warning 4189: variable initialized but never used

const int MaxRank=12;
const int MaxNumberOfRoots= 100;
const int SomeRandomPrimesSize= 25;
//used for hashing various things.
const int SomeRandomPrimes[SomeRandomPrimesSize]=
																		{ 743, 751, 757, 761, 769,
																			773, 787, 797, 809, 811,
																			821, 823, 827, 829, 839,
																			853, 857, 859, 863, 877, 
																			881, 883, 887, 907, 911};
																		 
class CompositeComplexQNSub;
class QuasiNumber;
class BasicQN;
class CombinatorialChamber;
class PolynomialsRationalCoeff;
class PrecomputedQuasiPolynomialIntegrals;
class Facet;
class hashedRoots;
class PrecomputedTauknPointersKillOnExit;
class QuasiPolynomial;
class VertexSelectionPointers;
class CombinatorialChamberPointers;
class FacetPointers;
class CompositeComplexQN;
template <class ElementOfCommutativeRingWithIdentity>
class Polynomial;
class Selection;
class intRoot;
class root;
template <class Object>
class ListObjectPointers;
template <class Object>
class HashedListBasicObjects;
class PrecomputedTauknPointers;
struct PolynomialOutputFormat;
class LargeRational;
class QPSub;
class partFractions;
class SubsetWithMultiplicities;
class hashedRoots;
class WeylGroup;
class intRoots;
class MatrixInt;
class QuasiPolynomials;

extern ::PolynomialOutputFormat PolyFormatLocal; //a global variable in 
//polyhedra.cpp used to format the polynomial printouts.



//#ifndef dont_define_Min_and_Max_in_polyhedra_h
//grrrrrrrr: names conflict
inline int Minimum(int a, int b)
{	if (a>b) {return b;} else {return a;}
}

inline int Maximum(int a, int b)
{	if (a>b) {return a;} else {return b;}
}
//#endif

struct DrawingVariables
{
public:
	static const int NumColors=8;
	bool DrawDashes;
	bool DrawChamberIndices;
	bool DrawingInvisibles;
	bool DisplayingChamberCreationNumbers;
	int Selected;
	double centerX;
	double centerY;
	double Projections[MaxRank][2];
	double scale;
	int TextColor;
	int ZeroChamberTextColor;
	int DeadChamberTextColor;
	int ColorChamberIndicator;
	int TextOutStyle;
	int TextOutStyleInvisibles;
	int DrawStyle;
	int DrawStyleDashes;
	int DrawStyleInvisibles;
	int DrawStyleDashesInvisibles;
	void initDrawingVariables(int cX1, int cY1);
	int ColorsR[DrawingVariables::NumColors];
	int ColorsG[DrawingVariables::NumColors];
	int ColorsB[DrawingVariables::NumColors];
	int Colors[DrawingVariables::NumColors];
	DrawingVariables(int cx, int cy){this->initDrawingVariables(cx,cy);};
};

//ListBasicObjects kills the objects it contains when it expires
template <class Object>
class ListBasicObjects
{
private:
	friend class PolynomialRationalCoeff;
	friend class IntegerPoly;
	friend class partFractions;
	friend class partFraction;
	int ActualSize;
	int IndexOfVirtualZero;
	Object* TheActualObjects;
	ListBasicObjects(ListBasicObjects<Object>&);
	void ExpandArrayOnTop(int increase);
	void ExpandArrayOnBottom(int increase);
public:
	static int ListBasicObjectsActualSizeIncrement;
	Object* TheObjects;
	int size;
//	void AddObjectOnTop(Object o);
	void SetSizeExpandOnTopNoObjectInit(int theSize);
	void SetActualSizeAtLeastExpandOnTop(int theSize);
	void AddObjectOnBottom(Object& o);
	void AddObjectOnTop(Object& o);
	void AddListOnTop(ListBasicObjects<Object>& theList);
	void PopIndexShiftUp(int index);
	void PopIndexShiftDown(int index);
	void PopIndexSwapWithLast(int index);
	void PopFirstOccurenceObjectSwapWithLast(Object& o);
	void CopyFromBase (const ListBasicObjects<Object>& From);
	void ShiftUpExpandOnTop(int StartingIndex);
	//careful output is not bool but int!!!!
	int ContainsObject(Object& o);
//	inline bool ContainsObject(Object& o){return this->ContainsObject(o)!=-1;};
	int SizeWithoutObjects();
	void ReleaseMemory();
	void operator=(ListBasicObjects<Object>& right){this->CopyFromBase(right);};
	static void swap(ListBasicObjects<Object>&l1, ListBasicObjects<Object>&l2);
	ListBasicObjects();
	~ListBasicObjects();
};

template <class Object>
class ListObjectPointers
{
public:
	static int MemoryAllocationIncrement;
	int ActualSize;
	int size;
	Object** TheObjects;
	ListObjectPointers();
	void KillAllElements();
	void KillElementIndex(int i);
	void CopyOntoObject(ListObjectPointers<Object>* FromList);
	void AddObject(Object* o);
	bool AddObjectNoRepetitionOfPointer(Object* o);
	void Pop(Object*o);
	void PopIndex(int i);
	void init(int d);
	int ObjectPointerToIndex(Object*o);
	void resizeToLargerCreateNewObjects(int increase);
	void IncreaseSizeWithZeroPointers(int increase);
	void initAndCreateNewObjects(int d);
	bool IsAnElementOf(Object* o);
	~ListObjectPointers();
};

class Integer
{
public:
	int value;
	static Integer TheRingUnit;
	static Integer TheRingMUnit;
	static Integer TheRingZero;
	inline void ComputeDebugString(){};
	inline void Add(Integer& y){this->value+=y.value;};
	inline void MultiplyBy(Integer& y){this->value*=y.value;};
	inline void operator=(Integer& y){this->value=y.value;};
	inline bool operator==(Integer& y){return this->value==y.value;};
	inline void Assign(Integer&y){this->value=y.value;};
	inline bool IsEqualTo(Integer&y){return this->value==y.value;};
	inline void DivideBy(Integer&y){this->value/=y.value;};
	inline void WriteToFile(std::fstream& output){output<<this->value;};
	inline void ReadFromFile(std::fstream& input){input>>this->value;};
	inline void ElementToString(std::string& output)
	{ std::stringstream out; out<<this->value; output= out.str();
	};
	Integer(int x){this->value=x;};
	Integer(){};
};

class Rational
{ bool operator==(Rational&);
	Rational(const Rational&);
public:
//for debug purposes
//	double x;
////////////////////
	int den;
	int num;
//	int sign;
	int sign();
	double DoubleValue();
	Rational(){};
	Rational(int Num,int Den){init(Num,Den);};
//	Rational(Rational& r){this->Assign(r);};
	void init(int Num,int Den);
	void MultiplyByInteger(int a);
	void Simplify();
	void MinusRational();
	bool IsInteger();
	void AddInteger(int x);
	void AssignFracValue();
	bool IsEqualTo(Rational& b);
	bool IsGreaterThan(Rational&b);
	bool IsGreaterThanOrEqualTo(Rational&b);
	void MultiplyByRational(Rational& r);
	bool IsEqualToZero();
	void AssignRational(Rational& r);
	static bool MinorRoutinesOnIgnoreErrors;
	static int LargestRationalHeight;
	static Rational TheRingUnit;
	static Rational TheRingZero;
	static Rational TheRingMUnit;
	void MakeZero();
	void AssignInteger(int i);
	void Invert();
	void Assign(Rational& r);
	inline void operator=(Rational& r){this->Assign(r);};
	void MultiplyBy(Rational& r);
	void DivideBy(Rational& r);
	void DivideByInteger(int x);
	void Add(Rational& r);
	void Subtract(Rational &r);
	void ElementToString(std::string& output);
	bool ElementHasPlusInFront();
	void WriteToFile(std::fstream& output);
	void ReadFromFile(std::fstream& input);
};

class MatrixRational
{
public:
	void Assign(MatrixRational& m);
	short NumRows;
	short NumCols;
	Rational** elements;
	std::string DebugString;
	void ComputeDebugString();
	void ComputeDeterminantOverwriteMatrix( Rational& output);
	void ElementToSting(std::string& output);
	void NonPivotPointsToRoot(Selection& TheNonPivotPoints, root& output);
	void NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, MatrixRational& output);
	void init(short r,short c);
	void Transpose();
	int FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex );
	void AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, Rational& scalar);
	void RowTimesScalar(int rowIndex, Rational& scalar);
	void SwitchTwoRows( int row1, int row2);
	void NullifyAll();
	bool Invert();
	void MultiplyByInt(int x);
	void MultiplyByRational(Rational& x);
	void AssignMatrixIntWithDen(MatrixInt& theMat, int Den);
	void ScaleToIntegralForMinRationalHeight();
	int FindLCMCoefficientDenominators();
	//int FindGCMCoefficientDenominatorsInRow(int index);
	int FindGCDCoefficientNumerators();
	//int FindGCMCoefficientNumeratorsInRow(int index);
	void Free();
	MatrixRational();
	~MatrixRational();
};

template <typename Element>
class Matrix
{	
public:
	short NumRows;
	short NumCols;
	Element** elements;
	void Assign(Matrix<Element>& m);
	void init(short r,short c);
	void Resize(short r, short c, bool PreserveValues);
	void Free();
	void NullifyAll();
	Matrix<Element>();
	~Matrix<Element>();
};


class MatrixInt : public Matrix<int>
{
public:
};


template <typename Element>
inline void Matrix<Element>::Assign(Matrix<Element>& m)
{ if (this==&m) return;
	this->init(m.NumRows, m.NumCols);
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			this->elements[i][j]=m.elements[i][j];  
}

template <typename Element>
Matrix<Element>::Matrix()
{ this->elements=0;
	this->NumCols=0;
	this->NumRows=0;  
} 

template <typename Element>
inline void Matrix<Element>::Free()
{ for (int i=0;i<this->NumRows;i++) 
	{	delete [] this->elements[i];
	}
	delete [] this->elements;
	this->elements=0;
	this->NumRows=0;
	this->NumCols=0;
}

template <typename Element>
Matrix<Element>::~Matrix() 
{ this->Free();
}

template <typename Element>
inline void Matrix<Element>::NullifyAll()
{ for (int i=0;i<this->NumRows;i++)
		for (int j=0; j<this->NumCols;j++)
			this->elements[i][j]=0;
}

template <typename Element>
inline void Matrix<Element>::init(short r, short c) 
{ this->Resize(r,c,false);
}

template <typename Element>
inline void Matrix<Element>::Resize(short r, short c, bool PreserveValues) 
{ if (r==this->NumRows && c== this->NumCols)
		return;
	if (r<=0)
	{	Free();
		return;
	}
	int** newElements= new Element*[r];
	for (int i=0;i<r;i++)
	{ newElements[i]= new int[c];
	}
	if (PreserveValues)
	{ for (int j=Minimum(this->NumRows,r)-1;j>=0;j--)
		{	for (int i=Minimum(this->NumCols,c)-1;i>=0;i--)
			{ newElements[j][i]= this->elements[j][i];
			}
		}
	}
	this->Free();
	this->NumCols= c;
	this->NumRows=r;  
	this->elements = newElements;
}

class root
{ 
private:
public:
	static unsigned char AmbientDimension;
	unsigned char dimension;
	Rational coordinates[MaxRank];
	std::string DebugString;
	void ComputeDebugString();
	void MakeZero();
	void Add(root& r);
	void DivByInteger(int a);
	void DivByRational(Rational& a);
	void ElementToString(std::string& output);
	//void RootToLinPolyToString(std::string& output,PolynomialOutputFormat& PolyOutput);
	void MultiplyByRational(Rational& a);
	void ScaleForMinHeight();
	void ScaleToIngegral();
	int FindLCMDenominators();
	void InitFromIntegers(int x1,int x2, int x3,int x4, int x5);
	void InitFromIntegers(int x1,int x2, int x3,int x4, int x5,int x6, int x7, int x8);
	void InitFromIntegers(int x1,int x2, int x3,int x4, int x5,int x6, int x7, int x8, int x9, int x10, int x11, int x12);
	void MultiplyByInteger(int a);
	void MinusRoot();
	void Subtract(root& r);
	void Assign(root& right);
	void AssignIntRoot(intRoot& right);
	bool IsProportianalTo(root& r);
	bool IsPositiveOrZero();	
	bool IsEqualToZero()
	{	for (int i=0;i<this->dimension;i++)
		{	if (!this->coordinates[i].IsEqualToZero()) 
			return false;
		}
		return true;
	};
	bool IsGreaterThanOrEqualTo(root& r);
	bool IsEqualTo(root& right);
	static void RootScalarEuclideanRoot(root& r1, root& r2, Rational& output);
	static void RootScalarRoot(root& r1, root& r2, MatrixRational& KillingForm, Rational& output);	
//	static void RootScalarRoot(root& r1, root& r2, MatrixInt& KillingForm, Rational& output);	
	static void RootPlusRootTimesScalar(root& r1, root& r2, Rational& rat, root& output);
	int HashFunction();
	root(){this->dimension=root::AmbientDimension;};
	inline void operator=(root& right){this->Assign(right);};
	inline bool operator==(root& right){return IsEqualTo(right);};
};

class roots : public ListBasicObjects<root>
{
public:
	std::string DebugString;
	void AssignIntRoots(intRoots& r);
	void AssignHashedIntRoots(HashedListBasicObjects<intRoot>& r);
	void AssignMatrixRows(MatrixRational& mat);
	void ComputeDebugString();
	int GetRankOfSpanOfElements();
	void AddRoot(root& r);
	void AddIntRoot(intRoot& r);
	void AddRootS(roots& r);
	void AddRootSnoRepetition(roots& r);
	bool AddRootNoRepetition(root& r);
	void PerturbVectorToRegular(root&output);
	void Average(root& output);
	void Pop(int index);
	bool IsRegular(root& r);
	bool IsRegular(root& r, root& outputFailingNormal);
	bool GetLinearDependence(MatrixRational& outputTheLinearCombination);
	void GaussianEliminationForNormalComputation(MatrixRational& inputMatrix, 
																							 Selection& outputNonPivotPoints);
	void rootsToMatrix(MatrixRational& output);
	void ElementToString(std::string& output);
	void SubSelection(Selection& theSelection, roots& output);
	void SelectionToMatrix(Selection& theSelection, MatrixRational& output);
	void SelectionToMatrixAppend
				(Selection& theSelection, MatrixRational& output, int StartRowIndex);
	void ComputeNormal(root& output);
	bool ComputeNormalExcludingIndex(root& output, int index);
	bool ComputeNormalFromSelection(root& output, Selection& theSelection);
	bool ComputeNormalFromSelectionAndExtraRoot(root& output,root& ExtraRoot, Selection& theSelection);
	bool ComputeNormalFromSelectionAndTwoExtraRoots(root& output,root& ExtraRoot1,
																									root& ExtraRoot2, Selection& theSelection);
};

class Selection
{
public:
	int MaxSize;
	int* elements;
//	int* elementsInverseSelection;
	bool* selected;
	int CardinalitySelection;
	void AddSelection(int index);
	void RemoveLastSelection();
	void init(int maxNumElements);
	void ComputeIndicesFromSelection();
	void initNoMemoryAllocation();
	std::string DebugString;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void incrementSelection();
	int SelectionToIndex();
	void ExpandMaxSize();
	void ShrinkMaxSize();
	void incrementSelectionFixedCardinality(int card);
	void Assign(Selection& right);
	inline void operator=(Selection& right){this->Assign(right);};
	Selection();
	Selection(int m);
	~Selection();
};

class CombinatorialChamber
{
public:
	std::string DebugString; 
	QuasiPolynomial* ThePolynomial;
	int CreationNumber;
	bool Explored;
	bool PermanentlyZero;
	FacetPointers* ExternalWalls;
	FacetPointers* InternalWalls;
	roots ExternalWallsNormals;
	roots AllVertices;
	root InternalPoint;
	int IndexStartingCrossSectionNormal;
	static bool ComputingPolys;
	static roots StartingCrossSectionNormals;
	static roots StartingCrossSectionAffinePoints;
	static bool DisplayingGraphics;
	static int MethodUsed;//1. normals vertices and boundaries
												//2. normals only
	static bool PrintWallDetails;
	bool HasZeroPoly();
	bool PointLiesInMoreThanOneWall(root& point);
	bool IsAnOwnerOfAllItsWalls();
	bool ComputeDebugString();
	bool CheckVertices();
	bool FacetIsInternal(Facet* f);
	void GetNormalFromFacet(root& output, Facet* theFacet);
	void FindAllNeighbors(ListObjectPointers<CombinatorialChamber>& TheNeighbors);
	bool SlashChamber(int theKillerEdgeIndex,Facet* TheFacet, root& direction,
		                roots& directions, int CurrentIndex,
										CombinatorialChamberPointers& output);
	bool SplitChamber(Facet* theKillerFacet,CombinatorialChamberPointers& output, root& direction);
	bool SplitChamberMethod1(Facet* theKillerFacet,CombinatorialChamberPointers& output);
	bool SplitChamberMethod2(Facet* theKillerFacet,CombinatorialChamberPointers& output, 
		                       root& direction);
	bool IsABogusNeighbor(Facet* NeighborWall, CombinatorialChamber* Neighbor);
	void ComputeEdgesAndVerticesFromNormals(bool ComputingVertices, bool ComputingEdges);//we assume that
	//the normals of the faces have been initialized properly
	bool PointIsInChamber(root&point);
//	bool ScaledVertexIsInWallSelection(root &point, Selection& theSelection);
	bool ScaleVertexToFitCrossSection(root&point);
	bool PlusMinusPointIsInWallSelection(root &point, Selection& theSelection);
	bool PointIsInWallSelection(root &point, Selection& theSelection);
	bool PlusMinusPointIsInChamber(root&point);
	bool LinearAlgebraForVertexComputation
				(Selection& theSelection, root& output);
	//returns false if the vectors were linearly dependent
	bool SliceInDirection(root& direction,roots& directions,
										    int CurrentIndex, CombinatorialChamberPointers& output,
												FacetPointers& FacetOutput);
//	void SliceInDirectionOld(root& direction,CombinatorialChamberPointers& output);
	bool HasHSignVertex(root& h,int sign);
	bool CheckSplittingPointCandidate(Selection &SelectionTargetSimplex, 
																	Selection &SelectionStartSimplex, 
																	MatrixRational& outputColumn);
	void AddInternalWall(Facet* TheKillerFacet, Facet* TheFacetBeingKilled, root& direction);
	void RemoveInternalWall(int index);
	void ComputeInternalPointMethod1(root& InternalPoint);
	void ComputeInternalPointMethod2(root& InternalPoint);
	bool TestPossibilityToSlice(root& direction);
	void Free();
	void PurgeAllSentencedFacets();
	CombinatorialChamber();
	~CombinatorialChamber();
};

template <class Object>
void ListBasicObjects<Object>::AddListOnTop(ListBasicObjects<Object>& theList)
{ int oldsize= this->size;
	this->SetSizeExpandOnTopNoObjectInit(this->size+theList.size);
	for (int i=oldsize;i<this->size;i++)
	{ this->TheObjects[i]= theList.TheObjects[i];
	}
}

template<class Object>
int ListBasicObjects<Object>::ContainsObject(Object &o)
{ for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i]==o)
		{ return i;
		}
	} 
	return -1;
}

template <class Object>
void ListBasicObjects<Object>::swap(ListBasicObjects<Object>&l1, ListBasicObjects<Object>&l2)
{ ListBasicObjects<Object>* bigL;
	ListBasicObjects<Object>* smallL;
	int smallSize;
	if (l1.size<l2.size)
	{smallL=&l1; bigL=&l2;smallSize=l1.size;}
	else
	{bigL=&l1;smallL=&l2;smallSize=l2.size;}
	static Object tempO;
	for(int i=0;i<smallL->size;i++)
	{ tempO=smallL->TheObjects[i];
		smallL->TheObjects[i]=bigL->TheObjects[i];
		bigL->TheObjects[i]=tempO;
	}
	smallL->SetSizeExpandOnTopNoObjectInit(bigL->size);
	for(int i=smallSize;i<bigL->size;i++)
	{smallL->TheObjects[i]=bigL->TheObjects[i];
	}
	bigL->size=smallSize;
}

template <class Object>
int ListBasicObjects<Object>::SizeWithoutObjects()
{ return	sizeof(this->ActualSize)+
					sizeof(this->IndexOfVirtualZero)+
					sizeof(this->size)+
					sizeof(this->TheActualObjects)+
					sizeof(this->TheObjects);
}

template <class Object>
void ListBasicObjects<Object>::ShiftUpExpandOnTop(int StartingIndex)
{ this->SetSizeExpandOnTopNoObjectInit(this->size+1);
	for (int i=this->size-1;i>StartingIndex;i--)
	{ this->TheObjects[i]= this->TheObjects[i-1];  
	} 
}

template <class Object>
void ListBasicObjects<Object>::CopyFromBase(const ListBasicObjects<Object>& From)
{ this->SetSizeExpandOnTopNoObjectInit(From.size);
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i]= From.TheObjects[i];  
	}
}

template <class Object>
void ListBasicObjects<Object>::SetActualSizeAtLeastExpandOnTop(int theSize)
{	if (!(this->ActualSize>= this->IndexOfVirtualZero+theSize))
	{	this->ExpandArrayOnTop( this->IndexOfVirtualZero+theSize- this->ActualSize);
	}
}

template <class Object>
void ListBasicObjects<Object>::PopFirstOccurenceObjectSwapWithLast(Object& o)
{ for (int i=0;i<this->size;i++)
	{ if (o==this->TheObjects[i])
		{ this->PopIndexSwapWithLast(i);
			return;
		}
	}
}

template <class Object>
void ListBasicObjects<Object>::SetSizeExpandOnTopNoObjectInit(int theSize)
{
	this->SetActualSizeAtLeastExpandOnTop(theSize); 
	size=theSize;
}

template <class Object>
void ListBasicObjects<Object>::PopIndexShiftUp(int index)
{
	if (size==0){return;}
	this->size--;
	for (int i=index;i>=1;i--)
	{
		this->TheObjects[i]=this->TheObjects[i-1];
	}
	this->TheObjects++;
	IndexOfVirtualZero++;
}

template <class Object>
void ListBasicObjects<Object>::PopIndexSwapWithLast(int index)
{	if (this->size==0){return;}
	this->size--;
	this->TheObjects[index]=this->TheObjects[this->size];
}

template <class Object>
ListBasicObjects<Object>::ListBasicObjects() 
{	this->ActualSize=0;
	this->IndexOfVirtualZero=0;
	this->size=0;
	this->TheObjects=0;
	this->TheActualObjects=0; 
}

template <class Object>
void ListBasicObjects<Object>::ReleaseMemory()
{	delete [] TheActualObjects;
	this->ActualSize=0;
	this->IndexOfVirtualZero=0;
	this->size=0;
	this->TheObjects=0;
	this->TheActualObjects=0; 
}

template <class Object>
ListBasicObjects<Object>::~ListBasicObjects()
{
	delete [] TheActualObjects;
	this->TheActualObjects=0;
	this->TheObjects=0;
}

template <class Object>
void ListBasicObjects<Object>::ExpandArrayOnBottom(int increase) 
{	if (increase<=0) return;
	Object* newArray = new Object[this->ActualSize+increase];
	for (int i=0;i<this->size;i++)
	{
		newArray[i+increase+this->IndexOfVirtualZero]=this->TheObjects[i];
	}
	delete [] this->TheActualObjects;
	this->TheActualObjects= newArray;
	this->ActualSize+=increase;
	this->IndexOfVirtualZero+=increase;
	this->TheObjects = this->TheActualObjects+this->IndexOfVirtualZero;
}

template <class Object>
void ListBasicObjects<Object>::ExpandArrayOnTop(int increase) 
{	if (increase<=0) return;
	Object* newArray = new Object[this->ActualSize+increase];
	for (int i=0;i<this->size;i++)
	{	newArray[i+this->IndexOfVirtualZero]=this->TheObjects[i];
	}
	delete [] this->TheActualObjects;
	this->TheActualObjects= newArray;
	this->ActualSize+=increase;
	this->TheObjects = this->TheActualObjects+this->IndexOfVirtualZero;
}

template <class Object>
void ListBasicObjects<Object>::AddObjectOnBottom(Object& o)
{
	if (this->IndexOfVirtualZero==0)
	{
		this->ExpandArrayOnBottom(ListBasicObjects<Object>::ListBasicObjectsActualSizeIncrement);
	}
	this->IndexOfVirtualZero--;
	this->TheObjects--;
	this->TheObjects[0]=o;
	this->size++;
}

template <class Object>
void ListBasicObjects<Object>::AddObjectOnTop(Object& o)
{	if (this->IndexOfVirtualZero+this->size>=this->ActualSize)
	{
		this->ExpandArrayOnTop(ListBasicObjects<Object>::ListBasicObjectsActualSizeIncrement);
	}
	this->TheObjects[size]=o;
	this->size++;
}

template <class Object>
class HashedListBasicObjects : public ListBasicObjects<Object>
{
protected:
	friend class partFractions;
	friend class QuasiMonomial;
	friend class CombinatorialChamber;
	friend class QuasiPolynomial;
	ListBasicObjects<int>* TheHashedArrays;
	void ClearHashes();
public:
	static int PreferredHashSize;
	int HashSize;
	void initHash();
	void ClearTheObjects();
	void AddObjectOnTopHash(Object& o);
	void PopIndexSwapWithLastHash(int index);
	int ContainsObjectHash(Object& o);
	void SetHashSize(int HS);
	int SizeWithoutObjects();
	HashedListBasicObjects();
	~HashedListBasicObjects();
	void CopyFromHash(HashedListBasicObjects<Object>& From);
};

template <class Object>
int HashedListBasicObjects<Object>::SizeWithoutObjects()
{ int Accum=0;
	Accum+=this->ListBasicObjects<Object>::SizeWithoutObjects();
	Accum+=sizeof(this->TheHashedArrays)*this->HashSize;
	Accum+=sizeof(this->HashSize);
	for (int i=0;i<this->HashSize;i++)
	{ Accum+=this->TheHashedArrays[i].SizeWithoutObjects(); 
	}
	return Accum;	
}


template <class Object>
void HashedListBasicObjects<Object>::CopyFromHash (HashedListBasicObjects<Object>& From)
{ if (&From==this){return;}
	this->ClearHashes();
	this->SetHashSize(From.HashSize); 
  this->CopyFromBase(From); 
  if (this->size<this->HashSize)
  {	for (int i=0;i<this->size;i++)
		{	int hashIndex= this->TheObjects[i].HashFunction()% this->HashSize;  
			if (hashIndex<0){hashIndex+=this->HashSize;}
			this->TheHashedArrays[hashIndex].CopyFromBase(From.TheHashedArrays[hashIndex]); 
		}
	}
	else
	{ for (int i=0;i<this->HashSize;i++)
		{ this->TheHashedArrays[i].CopyFromBase(From.TheHashedArrays[i]);
		}
	}
}
template <class Object>
void HashedListBasicObjects<Object>::ClearHashes()
{ if (this->size<this->HashSize)
	{	for (int i=0;i<this->size;i++)
		{ int hashIndex=this->TheObjects[i].HashFunction()%this->HashSize;
			if (hashIndex<0){hashIndex+=this->HashSize;}
			this->TheHashedArrays[hashIndex].size=0; 
		}
	}
	else
	{ for (int i=0;i<this->HashSize;i++)
		{ this->TheHashedArrays[i].size=0; 
		}
	}	
}

template <class Object>
void HashedListBasicObjects<Object>::ClearTheObjects()
{	this->ClearHashes();
	this->size=0;
}

template <class Object>
void HashedListBasicObjects<Object>::initHash()
{ this->size=0;
	for (int i=0;i<this->HashSize;i++)
	{ this->TheHashedArrays[i].size=0; 
	}
}

template <class Object>
int HashedListBasicObjects<Object>::ContainsObjectHash(Object &o) 
{	int hashIndex = o.HashFunction()%this->HashSize;
	if (hashIndex<0){hashIndex+=this->HashSize;}
	for (int i=0;i<this->TheHashedArrays[hashIndex].size;i++)
	{ int j=this->TheHashedArrays[hashIndex].TheObjects[i];
		if(this->TheObjects[j]==o)
			return j; 
	}
	return -1;
}

template <class Object>
void HashedListBasicObjects<Object>::AddObjectOnTopHash(Object &o)
{ int hashIndex = o.HashFunction()% this->HashSize;
	if (hashIndex<0) {hashIndex+=this->HashSize;}
	this->TheHashedArrays[hashIndex].AddObjectOnTop(this->size); 
	this->AddObjectOnTop(o);
}

template <class Object>
void HashedListBasicObjects<Object>::PopIndexSwapWithLastHash(int index) 
{	Object* oPop= &this->TheObjects[index];
	int hashIndexPop = oPop->HashFunction()% this->HashSize;
	if (hashIndexPop<0) {hashIndexPop+=this->HashSize;}
	TheHashedArrays[hashIndexPop].PopFirstOccurenceObjectSwapWithLast(index); 
	if (index==this->size-1){this->size--; return;}
	int tempI=this->size-1;
	Object* oTop= &this->TheObjects[tempI];
	int hashIndexTop= oTop->HashFunction()% this->HashSize;
	if (hashIndexTop<0){hashIndexTop+=this->HashSize;}
	TheHashedArrays[hashIndexTop].PopFirstOccurenceObjectSwapWithLast(tempI); 
	TheHashedArrays[hashIndexTop].AddObjectOnTop(index);
	this->PopIndexSwapWithLast(index);
}

template <class Object>
void HashedListBasicObjects<Object>::SetHashSize(int HS)
{ if (HS!=this->HashSize)
	{	delete [] this->TheHashedArrays;
		this->TheHashedArrays= new  ListBasicObjects<int>[HS];
		this->HashSize=HS; 
		this->size=0;
	}
}

template <class Object>
HashedListBasicObjects<Object>::HashedListBasicObjects()
{	this->HashSize=0;
	this->TheHashedArrays=0; 
	this->SetHashSize(HashedListBasicObjects<Object>::PreferredHashSize);
	this->initHash();
}

template <class Object>
HashedListBasicObjects<Object>::~HashedListBasicObjects() 
{	delete [] this->TheHashedArrays;
	this->TheHashedArrays=0; 
}

template <class Object>
class ListObjectPointersKillOnExit: public ListObjectPointers<Object>
{
public:
	~ListObjectPointersKillOnExit(){this->KillAllElements();};
};

template<class Object>
void ListObjectPointers<Object>::IncreaseSizeWithZeroPointers(int increase)
{ if (increase<=0){return;}
	if (ActualSize<this->size+increase)
	{ Object** newArray= new Object*[this->size+increase];
		for (int i=0;i<this->size;i++)
		{ newArray[i]=this->TheObjects[i];
		} 
		delete [] TheObjects;
		this->TheObjects= newArray;
		ActualSize+=increase;
	}
	for(int i=this->size;i<this->ActualSize;i++)
	{ this->TheObjects[i]=0;
	}
	this->size+=increase;
}

template<class Object>
void ListObjectPointers<Object>::initAndCreateNewObjects(int d)
{
	init(d);
	for (int i=0;i<d;i++)
	{
		TheObjects[i]=new Object;
	}
	this->size=d;
	this->ActualSize=d;
}

template<class Object>
void ListObjectPointers<Object>::KillElementIndex(int i)
{
	delete this->TheObjects[i];
	this->size--;
	this->TheObjects[i]=this->TheObjects[size]; 
}

template<class Object>
void ListObjectPointers<Object>::PopIndex(int i)
{
	this->size--;
	this->TheObjects[i]=this->TheObjects[size]; 
}

template<class Object>
int ListObjectPointers<Object>::ObjectPointerToIndex(Object* o)
{
	for (int i=0;i<this->size;i++)
	{
		if (this->TheObjects[i]==o){return i;}
	}
	return -1;
}


template<class Object>
void ListObjectPointers<Object>::resizeToLargerCreateNewObjects(int increase)
{	if (increase<=0){return;}
	size+=increase;
	if (size>ActualSize)
	{	ActualSize+=size-ActualSize;
		Object** newArray = new Object*[ActualSize];
		for (int i = 0; i< size-increase; i++)
		{	newArray[i]=TheObjects[i];
		}
		delete [] TheObjects;
		TheObjects=newArray;
	}
	for (int i=size-increase;i<size;i++)
	{	TheObjects[i]=new Object;
	}
}

template<class Object>
void ListObjectPointers<Object>::KillAllElements()
{	for (int i =0;i<size;i++)
	{	delete TheObjects[i];
		TheObjects[i]=0;
	}
	this->size=0;
}

template<class Object>
bool ListObjectPointers<Object>::AddObjectNoRepetitionOfPointer(Object* o)
{
	if (!IsAnElementOf(o))
	{
		AddObject(o);
		return true;
	}
	return false;
}

template<class Object>
bool ListObjectPointers<Object>::IsAnElementOf(Object* o)
{
	for(int i=0;i<size;i++)
	{
		if (TheObjects[i]==o)
		{
			return true;
		}
	}
	return false;
}
template<class Object>
void CopyOntoObject(ListObjectPointers<Object>* FromList)
{
	init(FromList->size);
	for(int i=0; i<FromList.size;i++)
	{
		AddObject(FromList->TheObjects[i]); 
	}
}

template <class Object>
ListObjectPointers<Object>::ListObjectPointers()
{
	ActualSize=0;
	size=0;
	TheObjects=0;
};

template <class Object>
void ListObjectPointers<Object>::Pop(Object*o)
{
	for (int i =0; i<size;i++)
	{
		if (o==TheObjects[i])
		{
			TheObjects[i]=TheObjects[size-1];
			size--;
			i--;		
		}
	}
};

template <class Object>
void ListObjectPointers<Object>::AddObject(Object* o)
{
	size++;
	if (size>ActualSize)
	{
		ActualSize+=ListObjectPointers<Object>::MemoryAllocationIncrement;
		Object** newArray = new Object*[ActualSize];
		for (int i = 0; i< size-1; i++)
		{
			newArray[i]=TheObjects[i];
		}
		delete [] TheObjects;
		TheObjects=newArray;
	}
	TheObjects[size-1]=o;
};

template <class Object>
ListObjectPointers<Object>::~ListObjectPointers()
{
	delete [] TheObjects;
	TheObjects=0;
}

template <class Object>
void ListObjectPointers<Object>::init(int d) 
{
	size=0;
	ActualSize=d;
	delete[] TheObjects;
	if (d==0) {return;}
	TheObjects= new Object*[ActualSize];
	for (int i =0;i<ActualSize; i++)
	{
		TheObjects[i]=0;
	}
}


class rootsPointersKillOnExit: public ListObjectPointersKillOnExit<roots>
{
public:
	std::string DebugString;
	void ComputeDebugString();
	void Average(root& output, int Number);
	void ResetCounters(int Number);
};

class Cone : public roots
{ //The roots are the normals to the walls of the cone
public:	
	void ComputeFromDirections(roots& directions);
	bool IsSurelyOutsideCone(ListObjectPointers<roots>& TheVertices, int NumrootsLists);
	bool IsInCone(root& r);
	int* ChamberTestArray;
	Cone(){ChamberTestArray=0;};
	~Cone(){delete [] this->ChamberTestArray;this->ChamberTestArray=0; };
};

class CombinatorialChamberPointers: public ListObjectPointers<CombinatorialChamber>
{
public:
	static const int MaxNumHeaps=5000;
	static int NumTotalCreatedCombinatorialChambersAtLastDefrag;
	static int DefragSpacing;
	static int LastReportedMemoryUse;
	static Cone TheGlobalConeNormals;
	static bool IsSurelyOutsideGlobalCone(ListObjectPointers<roots>& TheVertices, int NumrootsLists);
	static std::fstream TheBigDump;
	static root PositiveLinearFunctional;
	static bool PrintLastChamberOnly;
	QuasiPolynomials* ThePolys;
	ListBasicObjects<CombinatorialChamber*> PreferredNextChambers;
	CombinatorialChamber* NextChamberToSlice;
	CombinatorialChamber* LastComputedChamber;
	int NextChamberToSliceIndexInPreferredNextChambers;
	int FirstNonExploredIndex;
	void Free();
	void MakeStartingChambers(roots& directions, FacetPointers& FacetOutput);
	void ComputeNextIndexToSlice(root& direction);
	void LabelAllUnexplored();
	void KillNextChamberToSlice();
	void initThePolys();
	void DumpAll();
	void PrintThePolys(std::string& output);
	void ComputeGlobalCone(roots& directions);
	bool TestPossibleIndexToSlice(root&direction, int index);
	CombinatorialChamberPointers();
	~CombinatorialChamberPointers();
};

class CombinatorialChamberCouple
{
public:
	CombinatorialChamber* PlusOwner;
	CombinatorialChamber* MinusOwner;
};
class CombinatorialChamberCouplePointers: public ListObjectPointers<CombinatorialChamberCouple>
{
public:
	void AddCouple(CombinatorialChamber* PlusOwner,CombinatorialChamber* MinusOwner);
	void RemoveCouple(CombinatorialChamber* PlusOwner,CombinatorialChamber* MinusOwner);
	void RemoveCoupleBothOrders(CombinatorialChamber* Owner1,CombinatorialChamber* Owner2);
	void Substitute(Facet*owner,CombinatorialChamber *Leaves, 
		              CombinatorialChamber *Enters);
	~CombinatorialChamberCouplePointers(){this->KillAllElements();}
};
class Facet
{
public:
	std::string DebugString;
	int CreationNumber;
	root normal;
	bool SentencedToDeath;
	CombinatorialChamberCouplePointers Owners;
	roots AllVertices;
	roots Boundaries;
	bool IsInternalWRT(CombinatorialChamber* owner);
	bool IsAPlusOwner(CombinatorialChamber* owner);
	bool IsAnOwner(CombinatorialChamber* owner, 
		             CombinatorialChamber*&TheFirstOtherOwner, bool& IsAPlusOwner);
	void FindAllNeighborsTo(CombinatorialChamber* TheChamber,
					ListObjectPointers<CombinatorialChamber>& output);
	Facet();
	~Facet();
	//to be used with Method 1 only:
	Facet* RayTrace(int EdgeIndex, root& direction,roots& directions, 
									int CurrentIndex, CombinatorialChamber* owner);
	//to be used with Method 2 only:
	Facet*MakeFacetFromEdgeAndDirection(CombinatorialChamber* owner, 
																			Facet* theOtherFacet, root& direction,
																			roots & directions, int CurrentIndex,
																			FacetPointers& FacetOutput);
	bool CheckVertices(CombinatorialChamber* owner);
	void ComputeDebugString(CombinatorialChamber* owner);
	bool IsAValidCandidateForNormalOfAKillerFacet(root& normalCandidate,
																								roots& directions,int CurrentIndex);
	bool FacetContainsChamberOnlyOnce(CombinatorialChamber* owner);
	void PurgeRedundantBoundaries();
//	int OwnerToIndex(CombinatorialChamber* owner);
//	int GetChamberSign(CombinatorialChamber* owner);
	CombinatorialChamber* TheFirstOtherChamber(CombinatorialChamber* owner);
	bool OneOfTheOtherChambersIsNotExplored(CombinatorialChamber* owner);
	bool DumpSplitVerticesAndCreateNewFacets( Facet* TheKillerFacet, 
																			 CombinatorialChamber* BossChamber);
	bool HasHPositiveVertex(root& h);
	bool HasHNonPositiveVertex(root& h);
	void ComputeInternalPoint(root&output);
	void init();
	void Free();
	void PurgeAllEdgesAndVertices();
	bool IsExternalWithRespectToDirection
				(root &direction, CombinatorialChamber* owner,root& TheNormal);
	bool IsInFacetWithBoundaries(root& projection);
	bool IsInFacetNoBoundaries(root& point);
	void GetNormal(CombinatorialChamber* owner, root& output);
	bool SplitFacetMethod2(CombinatorialChamber *BossChamber,
												 CombinatorialChamber *NewPlusChamber, 
												 CombinatorialChamber *NewMinusChamber,
												 roots& ThePlusVertices, roots& TheMinusVertices,
												 Facet* TheKillerFacet, root& direction,
												 CombinatorialChamberPointers* PossibleBogusNeighbors,
												 FacetPointers* PossibleBogusWalls); 
	bool SplitFacetFindVertices(Facet* TheKillerFacet, CombinatorialChamber* owner);
	bool SplitVerticesAndCreateNewFacets(CombinatorialChamber* NewPlusChamber, 
																			 CombinatorialChamber* NewMinusChamber, Facet* TheKillerFacet, 
																			 CombinatorialChamber* BossChamber);
	void ProjectOnto(root& point, root& output);
};


class FacetPointers: public ListObjectPointers<Facet>
{
public:
	~FacetPointers()
	{	
	//	_CrtDumpMemoryLeaks();
	};
};

class FacetPointersKillOnExit: public FacetPointers
{
public:
	~FacetPointersKillOnExit(){this->KillAllElements();};
};


class RationalPointers: public ListObjectPointers<Rational>
{
public:
};

struct PolynomialOutputFormat
{
public:
	std::string alphabet[26];
	PolynomialOutputFormat();
	void MakeNumPartFrac();
	void MakeAlphabetxi();
	void MakeAlphabetyi();
	void MakeRegularAlphabet();
	int ExtraLinesCounterLatex;
	static int LatexCutOffLine;
	static int LatexMaxLineLength;
	static bool UsingLatexFormat;
	static bool CarriageReturnRegular;
	bool cutOffString;
	unsigned cutOffSize;
};

template <class ElementOfCommutativeRingWithIdentity>
class Monomial
{	
private:
	Monomial(Monomial<ElementOfCommutativeRingWithIdentity>&){assert(false);};
//	int DegreesToIndexRecursive(int MaxDeg, int index);
public:
	short NumVariables;
	int TotalDegree();
	short* degrees;
	static bool InitWithZero;
	std::string DebugString;
	bool ComputeDebugString(PolynomialOutputFormat &PolyFormat);
	ElementOfCommutativeRingWithIdentity Coefficient;
	int HashFunction();
//	int DegreesToIndex(int MaxDeg);
	void MakeConstantMonomial(short Nvar,ElementOfCommutativeRingWithIdentity&coeff);
	void MakeNVarFirstDegree(int LetterIndex,short NumVars,ElementOfCommutativeRingWithIdentity& coeff);
	void init(short nv);
	void initNoDegreesInit(short nv);
	void NullifyDegrees();
	void GetMonomialWithCoeffOne(Monomial<ElementOfCommutativeRingWithIdentity>& output);
	void MultiplyBy(Monomial<ElementOfCommutativeRingWithIdentity>& m, 
									Monomial<ElementOfCommutativeRingWithIdentity>& output);
	bool HasSameExponent(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	void CopyFrom(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	void Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution, 
		                Polynomial<ElementOfCommutativeRingWithIdentity>& output,
										short NumVarTarget);
	void MakeMonomialOneLetter(short NumVars,int LetterIndex,
	                           short Power, ElementOfCommutativeRingWithIdentity& Coeff);
	void IncreaseNumVariables(short increase);
	bool IsGEQ(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	void DecreaseNumVariables(short increment);
	void StringStreamPrintOutAppend(std::stringstream& out,PolynomialOutputFormat& PolyFormat);
	void MonomialToString(std::string& output,PolynomialOutputFormat& PolyFormat);
	bool IsAConstant();
	bool operator==(Monomial<ElementOfCommutativeRingWithIdentity>& m);
  void operator=(Monomial<ElementOfCommutativeRingWithIdentity>& m);
  int SizeWithoutCoefficient();
	Monomial();
	~Monomial();
};

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::InitWithZero=true; 

//The class below is to facilitate for different realizations of monomials in the future 
//Polynomial<ElementOfCommutativeRingWithIdentity> class.
//It should not be used outside of this file.
//The two current uses for the TemplatePolynomial
//would be to set TemplateMonomial to class Monomial
//or to class ...

template <class ElementOfCommutativeRingWithIdentity,
					template<typename RingElementMonomial> class TemplateMonomial
					>
class TemplatePolynomial: public HashedListBasicObjects
														<TemplateMonomial<ElementOfCommutativeRingWithIdentity> >
{	public:
	short NumVars;
	void MultiplyByMonomial(TemplateMonomial<ElementOfCommutativeRingWithIdentity>& m);
	void MultiplyByMonomial
         (TemplateMonomial<ElementOfCommutativeRingWithIdentity>& m, 
				  TemplatePolynomial
						<	ElementOfCommutativeRingWithIdentity,
							TemplateMonomial >& output
					);
	void AddMonomial(TemplateMonomial<ElementOfCommutativeRingWithIdentity>& m); 
	void CopyFromPoly(TemplatePolynomial
											<ElementOfCommutativeRingWithIdentity,TemplateMonomial>& p);
	void Assign(TemplatePolynomial
											<ElementOfCommutativeRingWithIdentity,TemplateMonomial>& p);
};

template <class ElementOfCommutativeRingWithIdentity,
					template<typename RingElementMonomial> class TemplateMonomial
					>
inline void TemplatePolynomial
			<ElementOfCommutativeRingWithIdentity, 
				TemplateMonomial>
			::CopyFromPoly(TemplatePolynomial
											<ElementOfCommutativeRingWithIdentity,TemplateMonomial>& p)
{	this->Assign(p);
}

template <class ElementOfCommutativeRingWithIdentity,
					template<typename RingElementMonomial> class TemplateMonomial
					>
void TemplatePolynomial
			<ElementOfCommutativeRingWithIdentity, 
				TemplateMonomial>
			::Assign(TemplatePolynomial
											<ElementOfCommutativeRingWithIdentity,TemplateMonomial>& p)
{	this->CopyFromHash(p);
	this->NumVars= p.NumVars;
}

template <class ElementOfCommutativeRingWithIdentity,
					template<typename RingElementMonomial> class TemplateMonomial
					>
void TemplatePolynomial
			<ElementOfCommutativeRingWithIdentity, 
				TemplateMonomial>
				::AddMonomial(TemplateMonomial<ElementOfCommutativeRingWithIdentity>& m) 
{	int j= this->ContainsObjectHash(m);
	if (j==-1)
	{	if (!m.Coefficient.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
			this->AddObjectOnTopHash(m);  
	}
	else
	{	this->TheObjects[j].Coefficient.Add(m.Coefficient);
		if (this->TheObjects[j].Coefficient.IsEqualTo
			   (ElementOfCommutativeRingWithIdentity::TheRingZero))
		{ this->PopIndexSwapWithLastHash(j); 
		}
	}
}


template <class ElementOfCommutativeRingWithIdentity,
					template<typename RingElementMonomial> class TemplateMonomial
					>
void TemplatePolynomial
			<ElementOfCommutativeRingWithIdentity, 
				TemplateMonomial>
					::MultiplyByMonomial(TemplateMonomial<ElementOfCommutativeRingWithIdentity>& m)
{ this->MultiplyByMonomial(m,*this); 
}

template <class ElementOfCommutativeRingWithIdentity,
					template<typename RingElementMonomial> class TemplateMonomial
					>
void TemplatePolynomial
			<ElementOfCommutativeRingWithIdentity, 
				TemplateMonomial >
					::MultiplyByMonomial
						(	TemplateMonomial<ElementOfCommutativeRingWithIdentity>& m, 
							TemplatePolynomial<ElementOfCommutativeRingWithIdentity,TemplateMonomial>& output)
{	if (m.Coefficient.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
	{ output.ClearTheObjects(); 
		return;
	}
	static TemplateMonomial<ElementOfCommutativeRingWithIdentity> tempM;
	static TemplatePolynomial<ElementOfCommutativeRingWithIdentity,
														TemplateMonomial> Accum;
	Accum.ClearTheObjects(); 
	Accum.NumVars= this->NumVars;
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyBy(m,tempM);
		Accum.AddMonomial(tempM); 
	}
	output.CopyFromPoly(Accum);
}
//**********************************************************
/*To use Polynomial<ElementOfCommutativeRingWithIdentity> you MUST define:

void ElementOfCommutativeRingWithIdentity::Add(ElementOfCommutativeRingWithIdentity&); 
void ElementOfCommutativeRingWithIdentity::MultiplyBy(ElementOfCommutativeRingWithIdentity&);
void ElementOfCommutativeRingWithIdentity::Assign(ElementOfCommutativeRingWithIdentity&);
void ElementOfCommutativeRingWithIdentity::ElementToString(std::string& output);
//the above is for printout& debug purposes:
//how you want your element to be displayed in string format.
ElementOfCommutativeRingWithIdentity::ElementOfCommutativeRingWithIdentity TheRingUnit;
ElementOfCommutativeRingWithIdentity::ElementOfCommutativeRingWithIdentity TheRingMUnit;
ElementOfCommutativeRingWithIdentity::ElementOfCommutativeRingWithIdentity TheRingZero;
int HashedListBasicObjects<Monomial<ElementOfCommutativeRingWithIdentity>>::PreferredHashSize;
//the above is the size of the hash table your polynomial will have. The larger the hash table - 
//the more RAM consumed - the faster the computation. Set 1 if you want min RAM usage. Set 10
//if you got no good idea of what this means.
int ListBasicObjects<Monomial<ElementOfCommutativeRingWithIdentity>>::ListBasicObjectsActualSizeIncrement;
//the above is the size of the chunk of memory the polynomial class will allocate
//whenever it needs more memory. For example, if you believe your polynomials
//will have 1000+ monomials average, then set the above to 1000. Note: abuse of the above 
//might raise your memory usage unexpectedly high! 
*/
//**********************************************************
//optional functions to use some of the methods of the class:
//void ElementOfCommutativeRingWithIdentity::DivideBy(&ElementOfCommutativeRingWithIdentity); 
//void ElementOfCommutativeRingWithIdentity::WriteToFile(std::fstream& output);
//void ElementOfCommutativeRingWithIdentity::ReadFromFile(std::fstream& input);
//void ElementOfCommutativeRingWithIdentity::AssignRational(Rational& r);

template <class ElementOfCommutativeRingWithIdentity>
class Polynomial: public TemplatePolynomial<ElementOfCommutativeRingWithIdentity,
																						Monomial>
{
private:
	Polynomial(Polynomial<ElementOfCommutativeRingWithIdentity>&){assert(false);};
public:
	std::string DebugString;
	// returns the number of lines used
	int StringPrintOutAppend(std::string& output, PolynomialOutputFormat& PolyFormat);
	bool ComputeDebugString();
	int TotalDegree();
	bool IsEqualTo(Polynomial<ElementOfCommutativeRingWithIdentity>& p);
	bool IsEqualToZero();
	void Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution, 
		                Polynomial<ElementOfCommutativeRingWithIdentity>& output,
										short NumVarTarget);
	void Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution, 
		                short NumVarTarget);
	void IncreaseNumVariables(short increase);
	void DecreaseNumVariables(short increment,Polynomial<ElementOfCommutativeRingWithIdentity>& output);
	void FindCoeffInFrontOfLinearTermVariableIndex(int index,
							ElementOfCommutativeRingWithIdentity& output );
	int FindMaxPowerOfVariableIndex(int VariableIndex);
	void Nullify(short NVar);
	void MakeMonomialOneLetter(short NumVars,int LetterIndex,
	                           short Power, ElementOfCommutativeRingWithIdentity& Coeff);
	void ComponentInFrontOfVariableToPower
							(int VariableIndex,
							 ListObjectPointers<Polynomial<ElementOfCommutativeRingWithIdentity> >& output,
							 int UpToPower);
	Polynomial(int degree, ElementOfCommutativeRingWithIdentity& coeff);
	bool HasGEQMonomial(Monomial<ElementOfCommutativeRingWithIdentity>&m, int& WhichIndex);
	Polynomial();
	~Polynomial();
	void MultiplyBy(Polynomial<ElementOfCommutativeRingWithIdentity>& p);
	void MultiplyBy(Polynomial<ElementOfCommutativeRingWithIdentity>& p,
									Polynomial<ElementOfCommutativeRingWithIdentity>& output);
	void MakeNVarDegOnePoly
					(short NVar, int NonZeroIndex,ElementOfCommutativeRingWithIdentity& coeff);
	void MakeNVarDegOnePoly
					(short NVar, int NonZeroIndex1, int NonZeroIndex2,
					 ElementOfCommutativeRingWithIdentity& coeff1,
					 ElementOfCommutativeRingWithIdentity& coeff2);
	void MakeNVarDegOnePoly
					(short NVar, int NonZeroIndex,ElementOfCommutativeRingWithIdentity& coeff1,
					 ElementOfCommutativeRingWithIdentity& ConstantTerm);
	void MakeNVarConst(short nVar, ElementOfCommutativeRingWithIdentity& coeff);
	void MakeLinPolyFromRoot(root& r);
//	void MultiplyByMonomial(Monomial<ElementOfCommutativeRingWithIdentity>& m);
//	void MultiplyByMonomial(Monomial<ElementOfCommutativeRingWithIdentity>& m,
//													Polynomial<ElementOfCommutativeRingWithIdentity>& output);
//	void AddMonomial(Monomial<ElementOfCommutativeRingWithIdentity>& m);
//	static bool AnErrorHasOccurredTimeToPanic;
	void AddPolynomial(Polynomial<ElementOfCommutativeRingWithIdentity>& p);
	void TimesInteger(int a);
	void AddConstant(ElementOfCommutativeRingWithIdentity& theConst);
	void TimesConstant(ElementOfCommutativeRingWithIdentity& r);
	void DivideByConstant(ElementOfCommutativeRingWithIdentity& r);
	void RaiseToPower(int d,Polynomial<ElementOfCommutativeRingWithIdentity>& output);
	void RaiseToPower(int d);
	void WriteToFile(std::fstream& output);
	void ReadFromFile(std::fstream& input, short NumV);
	int HasSameExponentMonomial(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	void operator= (Polynomial<ElementOfCommutativeRingWithIdentity>& right);
	bool operator== (const Polynomial<ElementOfCommutativeRingWithIdentity>& right);
	//If your name is Todor Milev, do not use if you don't remember what it does!
	//Otherwise don't use at all!
	bool IsGreaterThanZeroLexicographicOrder();
};

/*class MonomialMultiplicationTable: Matrix<short>
{
};

template <class ElementOfCommutativeRingWithIdentity>
class MonomialNthDegreeKVariables
{
public:

};

//This class is essentially a remake of the Polynomial<ElementOfCommutativeRingWithIdentity>
//The essential difference between this class and the Polynomial<ElementOfCommutativeRingWithIdentity>
//is that this class prepares a full table with all monomials of Nth degree in K variables
//and uses this table to index the monomials, rather than have the monomials themselves.
//This of course mea
template <class ElementOfCommutativeRingWithIdentity>
class PolynomialNthDegreeKVariables 
				:public TemplatePolynomial
									< ElementOfCommutativeRingWithIdentity,
									  MonomialNthDegreeKVariables>
{
public:
};*/

template <class ElementOfCommutativeRingWithIdentity>
class Polynomials: public ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >
{	public:
	//the format of the linear substitution is: 
	//coeff is a MatrixRational whose number of rows minus 1 must equal the # number of 
	//target variables and whose number of columns must equal the number of variables in
	//the current polynomial (this->NumVariables).
	//The first row denotes the constant term in the substitution of the respective variable!
	//An element in the x-th row and y-th column
	//is defined as ElementOfCommutativeRingWithIdentity[x][y] !
	void MakeLinearSubstitution(ElementOfCommutativeRingWithIdentity** coeffs, 
		                          short NumStartVar, short NumTargetVar);
	void MakeExponentSubstitution(MatrixInt& theSub);
	void PrintPolys(std::string& output,ElementOfCommutativeRingWithIdentity& TheRingUnit,
									ElementOfCommutativeRingWithIdentity& TheRingZero);
	void MakeSubstitutionLastVariableToEndPoint(short numVars, Polynomial<ElementOfCommutativeRingWithIdentity>& EndPoint);

};


class PolynomialsRationalCoeff: public Polynomials<Rational>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDubugString();
	void operator=(const PolynomialsRationalCoeff& right);
	bool operator==(const PolynomialsRationalCoeff& right);
	void ComputeB(PolynomialRationalCoeff& output,int cutOffIndex);
	void MakeUsualParametricRoot();
	void MakeOneParameterSubFromDirection(root& direction);
	void MakeOneParameterSubFromDirectionInts(int x1, int x2, int x3, int x4, int x5);
	void MakeOneParameterSubFromDirectionIntsAndConstants(int x1, int x2, int x3, int x4, int x5, 
						int c1, int c2, int c3, int c4, int c5);
	void MakeSubFromMatrixIntAndDen(MatrixInt& theMat, int Den);
	void MakeSubFromMatrixRational(MatrixRational& theMat);
	void ComputeDiscreteIntegrationUpTo(int d);
	void MakeLinearSubOnLastVariable(short NumVars,PolynomialRationalCoeff& LastVarSub);
	void MakeSubNVarForOtherChamber(root& direction,root& normal, Rational& Correction);
	void MakeSubAddExtraVarForIntegration(root& direction);
	void Substitution(PolynomialsRationalCoeff& theSub, short NumVarsTarget);
};

class PolynomialsRationalCoeffCollection: public ListBasicObjects<PolynomialsRationalCoeff>
{
public:
	roots ChamberIndicators;
	std::string DebugString;
	void ElementToString(std::string& output);
	void ElementToStringComputeFunctionB(std::string& output, bool computingB);
	void ComputeDubugString();	
	void Substitution(PolynomialsRationalCoeff& theSub, short NumVarsTarget);
};

class IntegerPoly: public Polynomial<Integer>
{ 
public:
	void MakePolyExponentFromIntRoot(intRoot& r);
	int SizeWithoutDebugString();
	void Evaluate(root& values, LargeRational& output);
	static bool AnErrorHasOccurredTimeToPanic;
};

class PolynomialLargeRational: public Polynomial<LargeRational>
{
public:
	void Evaluate(intRoot& values,LargeRational& output);
};

class PolynomialRationalCoeff: public Polynomial<Rational>
{
public:
	void AssignIntegerPoly(IntegerPoly& p);
	void MakePolyFromDirectionAndNormal(root& direction, root& normal, Rational& Correction);
	void MakePolyExponentFromIntRoot(intRoot& r);
	void MakeLinPolyFromInt(int x1,int x2, int x3,int x4, int x5);
	int FindGCMCoefficientDenominators();
	void MakeLinearPoly(short NumVars);
	int SizeWithoutDebugString();
	//works at the moment for linear polynomials only!!!!!!
	void DivByInteger(int x);
	void TimesInteger(int x);
	void operator=(PolynomialRationalCoeff& right);
	bool operator==(PolynomialRationalCoeff& right);
};

class QuasiPolynomials:public Polynomials<QuasiNumber>
{
public:
	void IntegrateDiscreteInDirectionFromZeroTo(
															root& direction, PolynomialRationalCoeff& EndPoint,
															QuasiPolynomial& output,
															QuasiPolynomial& input,
															PrecomputedQuasiPolynomialIntegrals& 
															                     PrecomputedDiscreteIntegrals);
};

template <class ElementOfCommutativeRingWithIdentity>
int Monomial<ElementOfCommutativeRingWithIdentity>::TotalDegree()
{	int result=0;
	for (int i=0;i<this->NumVariables;i++)
	{ result+=this->degrees[i];
	}
	return result;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>
				::Substitution(
				ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution, 
		    Polynomial<ElementOfCommutativeRingWithIdentity>& output,
				short NumVarTarget)
{	if (this->IsAConstant())
	{	output.MakeNVarConst(NumVarTarget,this->Coefficient); 
		return;
	}
	static Polynomial<ElementOfCommutativeRingWithIdentity> tempPoly;
	output.MakeNVarConst(NumVarTarget,this->Coefficient); 
//	output.ComputeDebugString(); 
	for (int i=0;i<this->NumVariables;i++)
	{	if (this->degrees[i]!=0)
		{	//TheSubstitution.TheObjects[i]->ComputeDebugString();
			TheSubstitution.TheObjects[i].RaiseToPower(degrees[i],tempPoly);
//			tempPoly.ComputeDebugString(); 
			output.MultiplyBy(tempPoly); 
//			output.ComputeDebugString(); 
		}
	}
}

template <class ElementOfCommutativeRingWithIdentity>
Monomial<ElementOfCommutativeRingWithIdentity>::Monomial()
{	this->NumVariables=0;
	this->degrees=0; 
	if(Monomial<ElementOfCommutativeRingWithIdentity>::InitWithZero)
	{ this->Coefficient.Assign(ElementOfCommutativeRingWithIdentity::TheRingZero);}
	else
	{ this->Coefficient.Assign(ElementOfCommutativeRingWithIdentity::TheRingUnit);}
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::init(short nv)
{	if(this->NumVariables!=nv)
	{	NumVariables=nv;
		delete [] degrees;
		degrees= new short[NumVariables];
	}
	for (int i=0;i<NumVariables;i++)
	{degrees[i]=0;}
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::initNoDegreesInit(short nv)
{ if(this->NumVariables!=nv)
	{	this->NumVariables=nv;
		delete [] degrees;
		degrees= new short[this->NumVariables];
	}
}

template <class ElementOfCommutativeRingWithIdentity>
Monomial<ElementOfCommutativeRingWithIdentity>::~Monomial() 
{	delete [] degrees;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::StringStreamPrintOutAppend(std::stringstream& out,
																					PolynomialOutputFormat& PolyFormat)
{	if (this->Coefficient.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
	{ out<< "0";
		return;
	}
	std::string tempS;
	this->Coefficient.ElementToString(tempS);
	if (this->IsAConstant()) 
	{	out<< tempS;
		return;
	}
	if (tempS=="-1")
	{ tempS="-";
	}
	if (tempS=="1")
	{ tempS.clear();
	}
	out <<tempS;
	for (int i=0;i<this->NumVariables;i++)
	{	if (this->degrees[i]!=0)
		{	out<<	PolyFormat.alphabet[i];
			if (!PolynomialOutputFormat::UsingLatexFormat)
			{	if (this->degrees[i]!=1)
				{	out << "^"<<this->degrees[i];
				}
			}
			else
			{	if (this->degrees[i]!=1)
				{	out << "^{"<<this->degrees[i]<<"}";
				}
			}
		}
	}
}

/*template <class ElementOfCommutativeRingWithIdentity>
int Monomial<ElementOfCommutativeRingWithIdentity>::DegreesToIndex(int MaxDeg)
{ return this->DegreesToIndexRecursive(MaxDeg,0); 
} 

template <class ElementOfCommutativeRingWithIdentity>
int Monomial<ElementOfCommutativeRingWithIdentity>::DegreesToIndexRecursive(int MaxDeg, int index)
{ int result=0;
	for (int i=0;i<this->degrees[index];i++)
	{ result+=NChooseK(MaxDeg-i+this->NumVariables-1-index,this->NumVariables-1-index); 
	}
	return result+this->DegreesToIndexRecursive(MaxDeg-this->degrees[index],index+1);  
} */

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::IsGEQ
																							(Monomial<ElementOfCommutativeRingWithIdentity>&m)
{ assert(this->NumVariables == m.NumVariables); 
	for (int i=0;i<m.NumVariables;i++)
	{ if (this->degrees[i]<m.degrees[i])
		{ return false;
		}
	}
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MonomialToString
           (std::string &output, PolynomialOutputFormat &PolyFormat)
{ std::stringstream out;
	this->StringStreamPrintOutAppend(out,PolyFormat); 
	output=out.str();
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::ComputeDebugString(PolynomialOutputFormat &PolyFormat)
{	this->MonomialToString(this->DebugString,PolyFormat);  
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MultiplyBy
			(Monomial<ElementOfCommutativeRingWithIdentity>& m, 
			 Monomial<ElementOfCommutativeRingWithIdentity>& output)
{	assert(m.NumVariables == this->NumVariables);
	output.init(this->NumVariables);
	output.Coefficient.Assign(m.Coefficient);
	output.Coefficient.MultiplyBy(this->Coefficient); 
	for(int i=0; i< m.NumVariables;i++)
	{	output.degrees[i]=this->degrees[i]+m.degrees[i]; 
	}
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::HasSameExponent(Monomial& m)
{	assert(m.NumVariables == this->NumVariables);
	for(int i=0; i< m.NumVariables;i++)
	{	if (this->degrees[i]!=m.degrees[i])
		{return false;}
	}
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::IncreaseNumVariables(short increase)
{	short* newDegrees= new short[NumVariables+increase];
	for(int i=0;i<this->NumVariables;i++)
	{	newDegrees[i]=degrees[i];	
	}
	for(int i=NumVariables;i<this->NumVariables+increase;i++)
	{	newDegrees[i]=0;	
	}
	this->NumVariables+=increase;
	delete [] degrees;
	degrees= newDegrees;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::GetMonomialWithCoeffOne(
														Monomial<ElementOfCommutativeRingWithIdentity>& output)
{ output.Coefficient.Assign(ElementOfCommutativeRingWithIdentity::TheRingUnit);
	output.initNoDegreesInit(this->NumVariables);  	
	for (int i=0; i< this->NumVariables;i++)
	{ output.degrees[i]=this->degrees[i];
	}
}
template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::operator ==
																(Monomial<ElementOfCommutativeRingWithIdentity>&  m)
{ for(int i=0;i<this->NumVariables;i++)
	{ if (this->degrees[i]!=m.degrees[i]) return false;
	}
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::operator =
																(Monomial<ElementOfCommutativeRingWithIdentity>&  m)
{ this->initNoDegreesInit(m.NumVariables); 
	for (int i=0;i<NumVariables;i++)
	{	this->degrees[i]=m.degrees[i]; 
	}
	this->Coefficient.Assign (m.Coefficient);
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::IsAConstant()
{ for (int i=0;i<this->NumVariables;i++)
	{ if (this->degrees[i]!=0) return false; 
	}
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::DecreaseNumVariables(short increment)
{	short* newDegrees= new short[NumVariables-increment];
	for(int i=0;i<this->NumVariables-increment;i++)
	{	newDegrees[i]=degrees[i];	
	}
	this->NumVariables-=increment;
	delete [] degrees;
	degrees= newDegrees;
}

template <class ElementOfCommutativeRingWithIdentity>
int Monomial<ElementOfCommutativeRingWithIdentity>::HashFunction()
{int result=0;	
	for (int i=0;i<this->NumVariables;i++)
	{ result+=this->degrees[i]*SomeRandomPrimes[i];}
	return result;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MakeConstantMonomial(short Nvar, ElementOfCommutativeRingWithIdentity& coeff)
{	this->init(Nvar);
	this->Coefficient.Assign (coeff);
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MakeNVarFirstDegree(int LetterIndex,
																					short NumVars, ElementOfCommutativeRingWithIdentity& coeff)
{	this->MakeConstantMonomial(NumVars,coeff); 
	this->degrees[LetterIndex]=1;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MakeMonomialOneLetter
								(short NumVars,int LetterIndex,short Power, ElementOfCommutativeRingWithIdentity& coeff)
{ this->init(NumVars);
	this->degrees[LetterIndex]=Power;
	this->Coefficient.Assign(coeff);
}

template <class ElementOfCommutativeRingWithIdentity>
int Monomial<ElementOfCommutativeRingWithIdentity>::SizeWithoutCoefficient()
{ int Accum =(sizeof(int) * this->NumVariables+sizeof(degrees));
	return Accum;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::CopyFrom(Monomial<ElementOfCommutativeRingWithIdentity>& m)
{	this->initNoDegreesInit(m.NumVariables); 
	for (int i=0;i<NumVariables;i++)
	{	this->degrees[i]=m.degrees[i]; 
	}
	this->Coefficient.Assign (m.Coefficient);
}


template <class ElementOfCommutativeRingWithIdentity>
int Polynomial<ElementOfCommutativeRingWithIdentity>
					::FindMaxPowerOfVariableIndex(int VariableIndex)
{ int result=0;
	for (int i=0;i<this->size;i++)
	{	result= Maximum(result,this->TheObjects[i].degrees[VariableIndex]); 
	}
	return result;
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::FindCoeffInFrontOfLinearTermVariableIndex
								(int index, ElementOfCommutativeRingWithIdentity& output) 
{ for (int i=0;i<this->size;i++)
	{	if (this->TheObjects[i].degrees[index]=1)
		{	bool FoundLinearTerm= true;
			for (int j=0;j<this->NumVars;j++)
			{	if (this->TheObjects[i].degrees[j]!=0 && j!=index)
				{	FoundLinearTerm=false;
					break;
				}
			}
			if (FoundLinearTerm)
			{	output.Assign(this->TheObjects[i].Coefficient);
				return;
			}
		}
	}
	output.Assign(*ElementOfCommutativeRingWithIdentity::TheRingZero);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
     ::AddConstant(ElementOfCommutativeRingWithIdentity& theConst)
{	static Monomial<ElementOfCommutativeRingWithIdentity> tempMon;
	tempMon.MakeConstantMonomial(this->NumVars,theConst);
	this->AddMonomial(tempMon); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::operator =(
				  Polynomial<ElementOfCommutativeRingWithIdentity>& right)
{ this->CopyFromPoly(right); 
}


template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::
				WriteToFile(std::fstream& output)
{ output<< "|/-- "<<this->size <<"\n";
	for (int i=0;i<this->size;i++)
	{ output <<"| ";
		this->TheObjects[i].Coefficient.WriteToFile(output);
		output <<" | ";
		for (int j=0;j<this->NumVars;j++)
		{ output<< this->TheObjects[i].degrees[j]<<" ";
		}
		output <<"\n";
	}
	output<<"|\\--"<<"\n";
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::
				ReadFromFile(std::fstream& input, short NumV)
{ std::string tempS;
	static Polynomial<ElementOfCommutativeRingWithIdentity> Buffer;
	Buffer.Nullify(NumV);
	Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.init(NumV);
	input>>tempS;
	assert (tempS=="|/--");
	int tempSize;
	input>>tempSize;
	Buffer.SetActualSizeAtLeastExpandOnTop(tempSize);
	for (int i=0;i<tempSize;i++)
	{ input >>tempS;
		tempM.Coefficient.ReadFromFile(input);
//		tempM.Coefficient.ComputeDebugString();
		input>>tempS;
		for (int j=0;j<NumV;j++)
		{ input >> tempM.degrees[j];
		}
		Buffer.AddMonomial(tempM);
	}
	input>>tempS;
	assert(tempS=="|\\--");
	this->CopyFromPoly(Buffer);
}


template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::IncreaseNumVariables
				(short increase)
{	static Polynomial<ElementOfCommutativeRingWithIdentity> Accum;
	Accum.Nullify(this->NumVars+increase);
	Accum.SetActualSizeAtLeastExpandOnTopHash(this->size);
  static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	for (int i=0;i<this->size;i++)
	{	tempM.CopyFrom(this->TheObjects[i]);
		tempM.IncreaseNumVariables(increase);
		Accum.AddMonomial(tempM);  
	}
	this->CopyFromPoly(Accum);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::DecreaseNumVariables
       (short increment,Polynomial<ElementOfCommutativeRingWithIdentity>& output)
{	output.ClearTheObjects();
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	for (int i=0;i<this->size;i++)
	{	tempM.CopyFrom(this->TheObjects[i]);  
		tempM.DecreaseNumVariables(increment);
		output.AddMonomial(tempM); 
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
			::MultiplyBy(Polynomial<ElementOfCommutativeRingWithIdentity> &p,
									 Polynomial<ElementOfCommutativeRingWithIdentity> &output) 
{	if (p.size==0)
	{ output.ClearTheObjects();
		return;
	}	
	static Polynomial<ElementOfCommutativeRingWithIdentity> Accum;
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	Accum.SetActualSizeAtLeastExpandOnTop(this->size*p.size);
	Accum.ClearTheObjects(); 
	Accum.NumVars= p.NumVars;
	for (int i=0;i<p.size;i++)
	{	for (int j=0;j<this->size;j++)
		{	this->TheObjects[j].MultiplyBy(p.TheObjects[i],tempM);
//			tempM.ComputeDebugString(PolyFormatLocal); 
//			Accum.ComputeDebugString();
			Accum.AddMonomial(tempM);
//			Accum.ComputeDebugString(); 
		}
	}
	output.CopyFromPoly(Accum); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
			::MultiplyBy(Polynomial<ElementOfCommutativeRingWithIdentity> &p) 
{	this->MultiplyBy(p,*this);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::MakeNVarConst
				(short nVar, ElementOfCommutativeRingWithIdentity& coeff)
{	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	this->ClearTheObjects();
	this->NumVars=nVar; 
	tempM.MakeConstantMonomial(nVar, coeff);
	this->AddMonomial(tempM); 
}

template <class ElementOfCommutativeRingWithIdentity>
bool Polynomial<ElementOfCommutativeRingWithIdentity>
			::IsEqualTo(Polynomial<ElementOfCommutativeRingWithIdentity>& p)
{	Polynomial<ElementOfCommutativeRingWithIdentity> tempPoly;
	tempPoly.CopyFromPoly(p);
	tempPoly.TimesConstant(ElementOfCommutativeRingWithIdentity::TheRingMOne);
	tempPoly.AddPolynomial(*this);
	return tempPoly.IsEqualToZero(); 
}

template <class ElementOfCommutativeRingWithIdentity>
bool Polynomial<ElementOfCommutativeRingWithIdentity>::IsEqualToZero()
{	if(this->size==0)
	{return true;}
	for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i]
	           .Coefficient.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
		{	return false;
		}
	}
	return true;
}


/*template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
			::AddMonomial(Monomial<ElementOfCommutativeRingWithIdentity>& m) 
{	int j= this->HasSameExponentMonomial(m);
	if (j==-1)
	{	if (!m.Coefficient.IsEqualTo(*ElementOfCommutativeRingWithIdentity::TheRingZero))
			this->AddObjectOnTopHash(m);  
	}
	else
	{	this->TheObjects[j].Coefficient.Add(m.Coefficient);
		if (this->TheObjects[j].Coefficient.IsEqualTo
			   (*ElementOfCommutativeRingWithIdentity::TheRingZero))
		{ this->PopIndexSwapWithLastHash(j); 
		}
	}
}*/


template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::MakeLinPolyFromRoot(root& r)
{ this->Nullify(r.dimension);
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	for (int i=0;i<r.dimension;i++)
	{ tempM.MakeNVarFirstDegree(i,r.dimension, 
															ElementOfCommutativeRingWithIdentity::TheRingUnit);
		tempM.Coefficient.AssignRational(r.coordinates[i]);  
		this->AddMonomial(tempM); 
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
				::MakeMonomialOneLetter(short NumVars, int LetterIndex, short Power, 
																ElementOfCommutativeRingWithIdentity& Coeff)
{ this->ClearTheObjects();
	this->NumVars= NumVars;
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.MakeMonomialOneLetter(NumVars,LetterIndex,Power,Coeff);
	this->AddObjectOnTopHash(tempM);  
}


template <class ElementOfCommutativeRingWithIdentity>
inline int Polynomial<ElementOfCommutativeRingWithIdentity>::HasSameExponentMonomial(Monomial<ElementOfCommutativeRingWithIdentity>& m)
{ return this->ContainsObjectHash(m); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::AddPolynomial(Polynomial<ElementOfCommutativeRingWithIdentity>& p)
{	this->SetActualSizeAtLeastExpandOnTop(p.size+this->size);
	std::string tempS1; 
	/*if (QuasiPolynomial::AnErrorHasOccurredTimeToPanic)
	{	std::string tempS;
		RandomCodeIDontWantToDelete::EvilList1.AddObjectOnTop(tempS);
		tempS1=& RandomCodeIDontWantToDelete::EvilList1.TheObjects
								[RandomCodeIDontWantToDelete::EvilList1.size-1];
	}*/
/*	LargeRational tempRat,tempRat2;
	IntegerPoly* tempP;
	if (IntegerPoly::AnErrorHasOccurredTimeToPanic)
	{ std::string tempS; 
		tempP=(IntegerPoly*)this; 
		tempP->Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot,tempRat);
		tempRat.ElementToString(tempS);
		//	currentList->AddObjectOnTop(tempS);
	}*/
	for (int i=0;i<p.size;i++)
	{	/*if (QuasiPolynomial::AnErrorHasOccurredTimeToPanic)
		{	std::string tempS; 
			LargeRational tempRat;
			QuasiPolynomial* tempP;
			tempP=(QuasiPolynomial*)this; 
			tempP->Evaluate(partFraction::theVectorToBePartitioned,tempRat);
			tempRat.ElementToString(tempS);
		//	currentList->AddObjectOnTop(tempS);
			tempS1->append(tempS);
			tempS1->append("\n");
		}*/
		this->AddMonomial(p.TheObjects[i]);   
	}
	/*if (IntegerPoly::AnErrorHasOccurredTimeToPanic)
	{ std::string tempS1,tempS2; 
		tempP->Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot,tempRat2);
		tempRat.ElementToString(tempS1);
		tempRat2.ElementToString(tempS2);
		this->ComputeDebugString();
		//assert(tempRat.IsEqualTo(tempRat2));
	}*/
}

template <class ElementOfCommutativeRingWithIdentity>
bool Polynomial<ElementOfCommutativeRingWithIdentity>::
               HasGEQMonomial(Monomial<ElementOfCommutativeRingWithIdentity>&m, int& WhichIndex)
{ for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].IsGEQ(m))
		{ WhichIndex=i;
			return true;
		}
	}
	WhichIndex=-1;
	return false;
}

template <class ElementOfCommutativeRingWithIdentity>
int Polynomial<ElementOfCommutativeRingWithIdentity>::StringPrintOutAppend
															(std::string& output, PolynomialOutputFormat& PolyFormat)
{	std::stringstream out;
	int NumLines=0;
	int TotalNumLines=0;
	const std::string LatexBeginString="";// "\\begin{eqnarray*}\n&&";
//	if (PolyFormat.UsingLatexFormat)
//	{ out <<LatexBeginString;
//	}
	for (int i=0;i<this->size;i++)
	{ std::string tempS;
		this->TheObjects[i].MonomialToString(tempS,PolyFormat);
		if (tempS[0]=='0'){tempS.erase(0,1);}
		if (tempS.size()!=0)
		{ if (tempS[0]!='-'){out <<"+";}
			out <<tempS;
			if (PolyFormat.UsingLatexFormat)
			{ NumLines+=tempS.size();
				if (NumLines>PolyFormat.LatexMaxLineLength)
				{ NumLines=0;
					out <<"\\\\\n&& ";
					if (!PolyFormat.CarriageReturnRegular){out<<"\r\n";}
					TotalNumLines++;
				}
			} 
			if (PolyFormat.cutOffString && out.str().size()> PolyFormat.cutOffSize)
			{ out <<"...";
				break;
			}			 
		}
	}
	std::string tempS;
	tempS= out.str();
	if (tempS[0]=='+'){tempS.erase(0,1); }
	if (PolyFormat.UsingLatexFormat)
	{ if (tempS[LatexBeginString.size()]=='+')
		{ tempS.erase(LatexBeginString.size(),1); 
		}
	}
	output.append(tempS);
	return TotalNumLines;
//	if (PolyFormat.UsingLatexFormat)
//	{ output.append("\\end{eqnarray*}");
//	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::TimesConstant
                 (ElementOfCommutativeRingWithIdentity& r)
{	if(r.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
	{	this->ClearTheObjects(); 
	}
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].Coefficient.MultiplyBy(r);  
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::DivideByConstant
                 (ElementOfCommutativeRingWithIdentity& r)
{	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].Coefficient.DivideBy(r);  
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::TimesInteger(int a)
{
	Rational r;
	r.AssignInteger(a);
	this->TimesRational(r); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::RaiseToPower
						(int d,Polynomial<ElementOfCommutativeRingWithIdentity>& output)
{	assert(d>=0);
	if (d==0)
	{	short nv=this->NumVars;
		this->MakeNVarConst(nv,ElementOfCommutativeRingWithIdentity::TheRingUnit);
		return;
	}	
	static Polynomial<ElementOfCommutativeRingWithIdentity> Accum;
	Accum.CopyFromPoly(*this);
	for (int i=0;i<d-1;i++)
	{	Accum.MultiplyBy(*this); 
	}
	output.CopyFromPoly(Accum); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::RaiseToPower(int d)
{	this->RaiseToPower(d,*this);
}

template <class ElementOfCommutativeRingWithIdentity>
int Polynomial<ElementOfCommutativeRingWithIdentity>::TotalDegree()
{
	int result=0;
	for (int i=0;i<this->TheMonomials.size;i++)
	{
		result=Maximum(this->TheMonomials.TheObjects[i]->TotalDegree(),result);
	}
	return result;
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::Nullify(short NVar)
{ this->ClearTheObjects();
	this->NumVars= NVar;  
}
	
template <class ElementOfCommutativeRingWithIdentity>
bool Polynomial<ElementOfCommutativeRingWithIdentity>
     ::IsGreaterThanZeroLexicographicOrder()
{	ElementOfCommutativeRingWithIdentity tCoeff, sCoeff, Coeff;
	tCoeff.Assign(ElementOfCommutativeRingWithIdentity::TheRingZero); 
	sCoeff.Assign(ElementOfCommutativeRingWithIdentity::TheRingZero); 
	Coeff.Assign(ElementOfCommutativeRingWithIdentity::TheRingZero);
	for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].degrees[0]==1)
		{ tCoeff.Add(this->TheObjects[i].Coefficient);
		}
		else
		{ if (this->TheObjects[i].degrees[1]==1)
			{ sCoeff.Add(this->TheObjects[i].Coefficient);
			}
			else
			{ Coeff.Add(this->TheObjects[i].Coefficient);
			}
		}
	}
 if (ElementOfCommutativeRingWithIdentity::TheRingZero.IsGreaterThan(tCoeff))
	{ return false;
	}
	if (tCoeff.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
	{ if (ElementOfCommutativeRingWithIdentity::TheRingZero.IsGreaterThan(sCoeff))
		{ return false;
		}
		if (sCoeff.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero))
		{ if (ElementOfCommutativeRingWithIdentity::TheRingZero.IsGreaterThan(Coeff))
			{ return false;
			}
		}
	}
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
     ::ComponentInFrontOfVariableToPower(int VariableIndex, 
																				 ListObjectPointers<Polynomial<ElementOfCommutativeRingWithIdentity> >& output,
																				 int UpToPower) 
{	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	for (int i=0;i<UpToPower;i++)
	{	output.TheObjects[i]->initHash();
	}
	if (this->size ==0){return;}	
	tempM.initNoDegreesInit(this->NumVars-1);
	for (int i=0;i<this->size;i++)
	{	int Power = this->TheObjects[i].degrees[VariableIndex];
		if (Power<=UpToPower)
		{ int k=0;
			for (int j=0;j<this->NumVars;j++)
			{	if (j!=VariableIndex)
				{	tempM.degrees[k]=this->TheObjects[i].degrees[j];
					k++;
				}
			}
			tempM.Coefficient.Assign(this->TheObjects[i].Coefficient);
			output.TheObjects[Power]->AddMonomial(tempM);
		}
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
     ::Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution, 
		                Polynomial<ElementOfCommutativeRingWithIdentity>& output,
										short NumVarTarget)
{	static Polynomial<ElementOfCommutativeRingWithIdentity> Accum, TempPoly; 
	Accum.ClearTheObjects();
	Accum.NumVars=NumVarTarget;
	for(int i=0;i<this->size;i++)
	{	this->TheObjects[i].Substitution(TheSubstitution, TempPoly, NumVarTarget);
		Accum.AddPolynomial(TempPoly); 
	}
	output.CopyFromPoly(Accum);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
     ::Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution, 
		                short NumVarTarget)
{	this->Substitution(TheSubstitution,*this,NumVarTarget);
}


template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::MakeNVarDegOnePoly(short NVar, int NonZeroIndex,
																																							ElementOfCommutativeRingWithIdentity& coeff)
{	this->ClearTheObjects();
	this->NumVars=NVar;
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.MakeNVarFirstDegree(NonZeroIndex,NVar,coeff); 
	this->AddMonomial(tempM); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
        ::MakeNVarDegOnePoly(short NVar, int NonZeroIndex1,int NonZeroIndex2,
																 ElementOfCommutativeRingWithIdentity& coeff1,
																 ElementOfCommutativeRingWithIdentity& coeff2)
{	this->ClearTheObjects();
	this->NumVars=NVar; 
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.MakeNVarFirstDegree( NonZeroIndex1,NVar,coeff1);
	this->AddMonomial(tempM);
	tempM.MakeNVarFirstDegree(NonZeroIndex2,NVar,coeff2); 
	this->AddMonomial(tempM); 
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>
        ::MakeNVarDegOnePoly(short NVar, int NonZeroIndex,
				                         ElementOfCommutativeRingWithIdentity& coeff1,
																 ElementOfCommutativeRingWithIdentity& ConstantTerm)
{	this->ClearTheObjects();
	this->NumVars =NVar;
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.MakeConstantMonomial(NVar,ConstantTerm);
	this->AddMonomial(tempM);
	tempM.MakeNVarFirstDegree(NonZeroIndex,NVar,coeff1);
	this->AddMonomial(tempM);
}

template <class ElementOfCommutativeRingWithIdentity>
bool Polynomial<ElementOfCommutativeRingWithIdentity>::ComputeDebugString()
{	this->DebugString.clear();
	this->StringPrintOutAppend(this->DebugString,PolyFormatLocal); 
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
Polynomial<ElementOfCommutativeRingWithIdentity>::Polynomial()
{ this->NumVars=0;
}

template <class ElementOfCommutativeRingWithIdentity>
Polynomial<ElementOfCommutativeRingWithIdentity>::Polynomial(int degree, ElementOfCommutativeRingWithIdentity& coeff)
{	this->MakeOneVarMonomial(degree,coeff); 
}

template <class ElementOfCommutativeRingWithIdentity>
Polynomial<ElementOfCommutativeRingWithIdentity>::~Polynomial()
{
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomials<ElementOfCommutativeRingWithIdentity>::PrintPolys(std::string &output, 
																			ElementOfCommutativeRingWithIdentity& TheRingUnit,
																			ElementOfCommutativeRingWithIdentity& TheRingZero) 
{	std::stringstream out;
	for (int i=0;i<this->size;i++)
	{	std::string tempS;
		out << i<<". ";
		this->TheObjects[i]->StringPrintOutAppend(tempS,PolyFormatLocal);
		out << tempS<<"\r\n";
	}
	output=out.str();
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomials<ElementOfCommutativeRingWithIdentity>::MakeLinearSubstitution
															(ElementOfCommutativeRingWithIdentity** coeffs,
															 short NumStartVar, short NumTargetVar)
{ if (this->size!=NumStartVar)
	{	this->KillAllElements();
		this->initAndCreateNewObjects(NumStartVar);
	}
	for (int i=0;i<this->size;i++)
	{ static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
		Polynomial<ElementOfCommutativeRingWithIdentity>* 
										tempP=this->TheObjects[i]->ClearTheObjects();
	  tempP->MakeNVarConst(NumTargetVar,coeffs[0][i]);
		for (int j=0;j<NumTargetVar;j++)
		{ tempM.MakeNVarFirstDegree(j,NumTargetVar,coeffs[i][j+1]);
			tempP->AddMonomial(tempM); 
		}
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomials<ElementOfCommutativeRingWithIdentity>::
			 MakeExponentSubstitution(MatrixInt& theSub) 
{ static Polynomial<ElementOfCommutativeRingWithIdentity> tempP;
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.init(theSub.NumRows);
	tempM.Coefficient.Assign(ElementOfCommutativeRingWithIdentity::TheRingUnit);
	this->size=0;
	this->SetSizeExpandOnTopNoObjectInit(theSub.NumCols);
	for (int i=0;i<theSub.NumCols;i++)
	{	for (int j=0;j<theSub.NumRows;j++)
		{ tempM.degrees[j]=(short) theSub.elements[j][i]; 
		}
		tempP.Nullify(theSub.NumRows);
		tempP.AddMonomial(tempM);  
//		tempP->ComputeDebugString(); 
		this->TheObjects[i].CopyFromPoly(tempP);
	}
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomials<ElementOfCommutativeRingWithIdentity>::
			MakeSubstitutionLastVariableToEndPoint
			 (short numVars,Polynomial<ElementOfCommutativeRingWithIdentity>& EndPoint)
{ this->SetSizeExpandOnTopNoObjectInit(numVars); 
	for (int i=0;i<numVars-1;i++)
	{	this->TheObjects[i].MakeNVarDegOnePoly(numVars,i,
	                                          ElementOfCommutativeRingWithIdentity::TheRingUnit); 
	}
	this->TheObjects[numVars-1].CopyFromPoly(EndPoint); 
}

class LargeInt:public ListBasicObjects<unsigned int>
{	friend class LargeRational;
	static const unsigned int CarryOverBound=2147483648;//=2^31
//	static const unsigned int CarryOverBound=37;
	void AddPositive(LargeInt& x);
	void SubtractSmallerPositive(LargeInt& x);
	void FitSize();
	void DivPositive(LargeInt &x, LargeInt& quotientOutput, LargeInt& remainderOutput) const; 
public:
	short sign;
	void MultiplyBy(LargeInt& x);
	void MultiplyBy(LargeInt& x, LargeInt& output);
	void MultiplyByInt(int x);
	void ElementToString(std::string& output);
	bool IsEqualToZero();
	bool IsEqualTo(LargeInt& x);
	void Assign(const LargeInt& x);
	void AssignShiftedUInt(unsigned int x, int shift);
	void AssignInt(int x);
	void Add(LargeInt& x);
	void AddInt(int x);
	//bool IsGEQ(LargeInt& x);
	bool IsGEQByAbs(LargeInt& x);
	bool CheckForConsistensy();
	static void gcd(LargeInt&a,LargeInt&b,LargeInt& output);
	void MakeZero();
	int operator %(int x);
	inline void operator = (const LargeInt& x)
	{ this->Assign(x);
	};
	LargeInt operator/(int x)const;
	LargeInt operator/(LargeInt& x)const;
	LargeInt(const LargeInt& x);
	LargeInt(int x){this->AssignInt(x);};
	LargeInt(){this->sign=1;};
	static LargeInt TheRingUnit;
	static LargeInt TheRingZero;
	static LargeInt TheRingMUnit;
};

class LargeRational
{
public:
	LargeInt den;
	LargeInt num;
	static bool AnErrorHasOccurredTimeToPanic;
	void Subtract(LargeRational& r);
	void Add(LargeRational& r);
	void AddRational(Rational& r);
	void MultiplyBy(LargeRational& r);	
	void MultiplyByRational(Rational&r);
	void MultiplyByInt(int x);
	void Assign(LargeRational& r);
	void AssignRational(Rational& r);
	void AssignInteger(int x);
	void ElementToString(std::string& output);
	bool IsEqualTo(LargeRational& r);
	bool IsEqualToZero();
	void Simplify();
	void Invert();
	void MakeZero();
	void WriteToFile (std::fstream& output);
	void ReadFromFile(std::fstream&  input);
	void RaiseToPower(int x);
	static LargeRational TheRingUnit;
	static LargeRational TheRingZero;
	static LargeRational TheRingMUnit;
	LargeRational(){};
	LargeRational(int x){this->AssignInteger(x);};
};

class BasicComplexNumber
{
public:
	LargeRational Coeff; 
	Rational Exp;
	void operator=(BasicComplexNumber& c);
	bool operator==(BasicComplexNumber& c);
	void Assign(BasicComplexNumber& c);
	void MultiplyBy(BasicComplexNumber& c);
	void MultiplyByRational(Rational& c);
	void MultiplyByLargeRational(LargeRational& c);
	void AssignRational(Rational& r);
	void AssignLargeRational(LargeRational& r);
	void ElementToString(std::string& output);
	void init(Rational& coeff, Rational& exp); 
	void Simplify();
};

class CyclotomicList : private PolynomialsRationalCoeff
{
public:
	void ComputeCyclotomic(short n);
	void GetSumPrimitiveRoots(short n, Rational& output);
	void DivOneVarByOneVarPoly(PolynomialRationalCoeff& p, PolynomialRationalCoeff& q, 
	                           PolynomialRationalCoeff& quotient, PolynomialRationalCoeff& remainder);
	int EulerPhi(short n);
};

class CompositeComplex: public ListBasicObjects<BasicComplexNumber>
{
private:
	void AddBasicComplex(BasicComplexNumber& b);
	bool SimplifyTrue();
public:
	std::string DebugString;
	void ComputeDebugString();
	static CyclotomicList PrecomputedCyclotomic;
	void MultiplyByBasicComplex(BasicComplexNumber& b);
	void AssignRational(Rational& r);
	void ElementToString(std::string& output);
	void Assign(CompositeComplex& c);
	void Add(CompositeComplex& c);
	void AddRational(Rational& r);
	void AddLargeRational(LargeRational& r);
	void MultiplyByRational(Rational& r);
	void MultiplyByLargeRational(LargeRational& r);
	void MultiplyBy(CompositeComplex& c);
	void Simplify();
	int FindMaxDenExp();
	void MakeBasicComplex(Rational& coeff, Rational& exp); 
	bool SimplifyWRT(short n);
	bool IsEqualTo(CompositeComplex&c);
	bool IsEqualToZero();
};

class BasicQN
{
private:
	short GaussianEliminationByRowsCol(int Col, bool MoveToRight);
public:
	static int NumTotalCreated;
	static ListObjectPointers<BasicQN> GlobalCollectorsBasicQuasiNumbers;
	int CreationNumber;
	std::string DebugString;
	BasicQN(short NumVars)
	{	BasicQN::NumTotalCreated++; this->NumVars= NumVars; CreationNumber=NumTotalCreated;
		BasicQN::GlobalCollectorsBasicQuasiNumbers.AddObject(this); 
	};
	BasicQN()
	{	BasicQN::NumTotalCreated++; CreationNumber=NumTotalCreated;
		BasicQN::GlobalCollectorsBasicQuasiNumbers.AddObject(this); 
	};
	void MakeQNFromMatrixAndColumn(MatrixRational& theMat, root& column);
	LargeRational Coefficient;
	MatrixInt Exp;
	MatrixInt Nums;
	short NumVars;
	int Den;
	int HashFunction();
	void ScaleBy(int DenIncrease);
//	Selection PivotPoints;
	void ExpToDebugString();
	short GaussianEliminationByRows();
//	void AddBasicComplexNumber(BasicComplexNumber& b);
//	void MultiplyByTauSameDenMethod1(int* tau, int k);
	void Assign(BasicQN& p);
	int InvertModN( int d, int p);
	void SwitchTwoRows(int rowI1,int rowI2, int StartCol);
	void RowPlusScalarTimesRow(int rowInd, int scalar, int otherRowInd, int StartCol);
	void MultiplyRowBy(int rowInd, int scalar, int StartColInd);
	void SetPivotRow(int index, int PivotRowIndex, int Col);
	void MultiplyBySameDen(BasicQN& q);
	void MultiplyBy(BasicQN& q);
	void MultiplyByRational(Rational& r);
	void ComputeDebugString();
 	void ElementToString(std::string& output, PolynomialOutputFormat& PolyFormat);
 	void ElementToString(std::string& output);
	bool HasSameExponent(BasicQN& q);
	bool ExponentIsEqualToZero();
	void DecreaseNumVars(short decrease);
	bool IsEqualToZero();
	void MakeConst(Rational& Coeff, short NumV);
	void MakeConst(LargeRational& Coeff, short NumV);
	void MakeFromNormalAndDirection(root& normal, root& direction, 
																	int theMod, Rational& coeff);
	void MakePureQN(short NumVariables,int NonZeroIndex, Rational&coeff, 
									int theExp, int Num, int theDen);
	void BasicQNToComplexQN(CompositeComplexQN& output);
	//for format of the substitution see class qQPSub
	void LinearSubstitution(QPSub& theSub);
	void operator =(BasicQN& q);
	bool operator ==(BasicQN& q);
	void Simplify();
	void GetCoeffInFrontOfLast(Rational& output);
	void Evaluate(int* theVars, LargeRational& output);
	void WriteToFile (std::fstream& output);
	void ReadFromFile(std::fstream&  input, short NumV);
//	void MakeQN(PolynomialRationalCoeff& exp, Rational& coeff);
};

class QuasiNumber :public HashedListBasicObjects<BasicQN>
{
public:
	std::string DebugString;
	short NumVariables;
	bool ComputeDebugString();
	bool IsEqualTo(QuasiNumber& q);
	bool IsEqualToZero();
	static bool AnErrorHasOccurredTimeToPanic;
	void AddBasicQuasiNumber(BasicQN& q);
	void Add(QuasiNumber &q);
	void MultiplyByBasicQuasiNumber(BasicQN& q);
	void MultiplyBy(QuasiNumber& q);
	void Assign(QuasiNumber& q);
	void AssignLargeRational(short NumVars, LargeRational& coeff);
	void AssignInteger(short NumVars, int x);
	void ElementToString(std::string& output, PolynomialOutputFormat& PolyFormat);
	void ElementToString(std::string& output);
	void MakeConst(Rational& Coeff, short NumV);
	void MakeConst(LargeRational& Coeff, short NumV);
	void DivideByRational(Rational& r);
//	void DecreaseNumVars(int decrease);
	void MultiplyByRational(Rational& r);
	void MakePureQN(short NumVar,int NonZeroIndex, Rational&coeff, 
									int theExp, int Num, int theDen);
	void MakeFromNormalAndDirection(root& normal, 
																	root& direction, int theMod, Rational& coeff);
	void LinearSubstitution(QPSub& TheSub);
	static QuasiNumber TheRingUnit;
	static QuasiNumber TheRingZero;
	static QuasiNumber TheRingMUnit;
	void MakeZero(short NumVars);
	void Simplify();
	void Evaluate(int* theVars, LargeRational& output);
	int FindGCMDens();
	void QNtoComplex(CompositeComplexQN& output);
	void MakeQNFromMatrixAndColumn(MatrixRational& theMat, root& column);
	void WriteToFile(std::fstream& output);
	void ReadFromFile(std::fstream& input);
//	int NumNonZeroElements();
	QuasiNumber(short numVars)
	{this->NumVariables=numVars; };
	QuasiNumber()
	{this->NumVariables=0;};
};

class ComplexQN
{
private:
	void CheckCoefficient(){};
public:
	static int NumTotalCreated;
	static ListObjectPointers<ComplexQN> GlobalCollectorsComplexQNs;
	int CreationNumber;
	std::string DebugString;
	ComplexQN(int NumVars)
	{	ComplexQN::NumTotalCreated++; this->NumVars= NumVars;CreationNumber=NumTotalCreated;
		ComplexQN::GlobalCollectorsComplexQNs.AddObject(this); 
	};
	ComplexQN()
	{	this->NumVars=NumVars;ComplexQN::NumTotalCreated++;CreationNumber=NumTotalCreated;
		ComplexQN::GlobalCollectorsComplexQNs.AddObject(this); 
	};
	CompositeComplex Coefficient;
	Rational Exponent[MaxRank+1];
	int NumVars;
//	void AddBasicComplexNumber(BasicComplexNumber& b);
	void MultiplyBy(ComplexQN& q);
	void MultiplyByBasicComplex(BasicComplexNumber& b);
	void MultiplyByRational(Rational& r);
	void MultiplyByLargeRational(LargeRational& r);
	void ComputeDebugString();
	void CopyFrom(ComplexQN& q);
 	void ElementToString(std::string& output);
	void LinPartToString(std::string& output);
	bool HasSameExponent(ComplexQN& q);
	void DecreaseNumVariables(int increment, ComplexQN& output);
	void DecreaseNumVariables(int increment, CompositeComplexQN& output);
	void DecreaseNumVariables(int increment);
	bool ExponentIsEqualToZero();
	bool IsBasicComplex();
	bool IsEqualToZero();
	void MakeConst(Rational& Coeff, int NumVars);
	void MakePureQN(Rational* expArg, int NumVars);
//	void MakePureQN(int NumVars,int NonZeroIndex, Rational&coeff, Rational&ConstExp);
	void MakePureQN(int NumVars,int NonZeroIndex, Rational&coeff, Rational&ConstExp, Rational& Exp);
	//see PolynomialPointersKillOnExit::MakeLinearSubstitution
	//for the substitution format!
	void LinearSubstitution(MatrixRational& TheSub);
//	void RootSubsti
	void operator =(ComplexQN& q);
	bool operator ==(ComplexQN& q);
	void Simplify();
//	void MakeQN(PolynomialRationalCoeff& exp, Rational& coeff);
};

class CompositeComplexQN: public ListBasicObjects<ComplexQN>
{
public:
	std::string DebugString;
	int NumVariables;
	bool ComputeDebugString();
	bool IsEqualTo(CompositeComplexQN& q);
	bool IsEqualToZero();
	void AddComplexQN(ComplexQN& q);
	void Add(CompositeComplexQN &q);
	void MultiplyByComplexQN(ComplexQN& q);
	void MultiplyBy(CompositeComplexQN& q);
	void Assign(CompositeComplexQN& q);
	void ElementToString(std::string& output);
	void MakeConst(Rational& Coeff, int numVars);
	void DivideByRational(Rational& r);
	void Simplify();
	void MultiplyByRational(Rational& r);
	void MultiplyByLargeRational(LargeRational& r);
	void MakePureQN(Rational* expArg,int numVars);
	void MakePureQN(Rational* expArg,int numVars, Rational& coeff);
	void MakePureQN(Rational& constExp,int numVars, Rational&coeff);
	void MakePureQN(int numVars,int NonZeroIndex, Rational&coeff, Rational&Exp);
	void MakePureQN(int numVars,int NonZeroIndex, Rational&coeffExp, Rational& ConstExp, Rational& Coeff);
	//see PolynomialPointersKillOnExit::MakeLinearSubstitution
	//for the substitution format!
	void LinearSubstitution(MatrixRational& TheSub);
	static CompositeComplexQN TheRingUnit;
	static CompositeComplexQN TheRingZero;
	static CompositeComplexQN TheRingMUnit;
	bool ElementHasPlusInFront();
	int NumNonZeroElements();
	CompositeComplexQN(int numVars)
	{this->size=0; this->NumVariables=numVars; };
	CompositeComplexQN()
	{this->size=0; this->NumVariables=0;};
	CompositeComplexQN(Rational* expArg, int numVars, Rational& coeff)
	{this->MakePureQN(expArg,numVars,coeff) ;};
};

class CompositeComplexQNPoly: public Polynomial<CompositeComplexQN>
{
public:
void AssignQP(QuasiPolynomial& q);
};

class QuasiPolynomial: public Polynomial<QuasiNumber>
{
public:
	static ListObjectPointers<QuasiPolynomial> GlobalCollectorsPolys;
	static int TotalCreatedPolys;
	int CreationNumber; //for debug purposes
	static PrecomputedTauknPointersKillOnExit* PrecomputedTaus;
	static bool AnErrorHasOccurredTimeToPanic;
	void AssignPolynomialRationalCoeff(PolynomialRationalCoeff& p);
	void AssignPolynomialLargeRational(PolynomialLargeRational& p);
	void MakeTauknp(int k, int n);
	void MakePureQuasiPolynomial(PolynomialRationalCoeff& p, int NumVars);
	void RationalLinearSubstitution(QPSub& TheSub, QuasiPolynomial& output);
	void IntegrateDiscreteFromZeroTo
									(QPSub& EndPointSub, QuasiPolynomial &output, 
									 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
	void IntegrateDiscreteInDirectionFromZeroTo
									(QPSub& EndPointSub, QPSub& DirectionSub, 
									 QuasiPolynomial &output, 
									 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
	void IntegrateDiscreteInDirectionFromOldChamber
		              (root &direction,root& normal,QuasiPolynomial& OldChamberPoly, 
									 QuasiPolynomial &output, 
									 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
	void WriteComplexFormToDebugString();
	void TimesInteger(int x);
	void DivByInteger(int x);
	void Evaluate(intRoot& values,LargeRational& output);
	int SizeWithoutDebugString();
	void operator=(QuasiPolynomial& right);
	void Simplify();
	QuasiPolynomial();
	void Nullify(short numVars);
};

class CompositeComplexQNSub
{
public:
	MatrixRational MatrixForCoeffs;
	PolynomialsRationalCoeff RationalPolyForm;
	void MakeLinearSubIntegrand(root& normal, root&direction, Rational& Correction);
	void MakeSubNVarForOtherChamber(root& direction,root& normal, Rational& Correction);
	void MakeSubAddExtraVarForIntegration(root& direction);
};

class QuasiMonomial: public Monomial<QuasiNumber>
{
public:
	void IntegrateDiscreteInDirectionFromZeroTo
					(QPSub& EndPointSub, QPSub& DirectionSub, 
					QuasiPolynomial &output, 
					PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals) ;
	void IntegrateDiscreteFromZeroTo
									(QPSub& EndPointSub, QuasiPolynomial &output, 
									 PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
	void RationalLinearSubstitution(QPSub& TheSub, QuasiPolynomial& output);
};

class PrecomputedTaukn
{
public:
	int k,n;
	CompositeComplexQN Taukn;
	PrecomputedTaukn(int theK, int theN){k=theK;n=theN;};
	PrecomputedTaukn(){};
};

class PrecomputedTauknPointersKillOnExit: public ListObjectPointers<PrecomputedTaukn>
{
private: 
	void ComputeTaukn(int k, int n, CompositeComplexQN& output);
public:
	void GetTaukn(int k, int n, CompositeComplexQN& output);
	~PrecomputedTauknPointersKillOnExit(){this->KillAllElements(); };
};

class PrecomputedQuasiPolynomialIntegral
{
public:
	int Num;
	int Den;
	int degree;
	QuasiPolynomial Value;
	void ComputeValue(PolynomialsRationalCoeff& PreComputedBernoulli);
	void operator=(PrecomputedQuasiPolynomialIntegral& right);
};

class PrecomputedQuasiPolynomialIntegrals: 
	      public ListBasicObjects<PrecomputedQuasiPolynomialIntegral>
{
public:
	static PolynomialsRationalCoeff* PreComputedBernoulli;
	void GetQuasiPolynomialIntegral(int Num, int Den, int degree, QuasiPolynomial& output);
};

class QPSub
{
public:
	MatrixInt TheQNSub;
	int QNSubDen;
	PolynomialsRationalCoeff RationalPolyForm;
	//PolynomialsRationalCoeff RationalPolyForm;
	//format of the substitution:
	//TheQNSub has rows for each new variable
	//plus an extra row for the constant terms of the linear expressions and
	//columns for each of the substituted variables.
	//in the substitution x_1\mapsto x_1+2x_2+4x_3+3 
	//                    x_2\mapsto x_1-x_2-2x_3+1 
	// QPSub should look like (2 columns, 4 rows):
	// 1   1 
	// 2  -1
	// 4  -2
	// 3   1
	void MakeSubFromMatrixInt(MatrixInt& theMat);
	void MakeSubFromMatrixIntAndDen(MatrixInt& theMat, int Den);
	void MakeSubFromMatrixRational(MatrixRational& theMat);
	void MakeLinearSubIntegrand(root& normal, root&direction, Rational& Correction);
	void MakeSubNVarForOtherChamber(root& direction,root& normal, Rational& Correction);
	void MakeSubAddExtraVarForIntegration(root& direction);
	void MakeSubFromPolynomialsRationalCoeff(PolynomialsRationalCoeff& input);
};

template <class Object>
class ListObjectPointersKillOnExitFakeSize: public ListObjectPointersKillOnExit<Object>
{
public: 
	int FakeSize;
	ListObjectPointersKillOnExitFakeSize(){this->FakeSize=0;} 
	void setFakeSize(int theFakeSize, short param);
	void Nullify(short param);
};

template <class Object>
void ListObjectPointersKillOnExitFakeSize<Object>::Nullify(short param)
{ this->FakeSize=0;
} 

template <class Object>
void ListObjectPointersKillOnExitFakeSize<Object>::setFakeSize(int theFakeSize, short param)
{ if (theFakeSize>this->size)
	{ this->resizeToLargerCreateNewObjects(theFakeSize-this->size);
	}
	for (int i=this->FakeSize;i<theFakeSize;i++)
	{ this->TheObjects[i]->Nullify(param); 
	}
	this->FakeSize =theFakeSize;
}

class SortedQPs : public ListObjectPointersKillOnExitFakeSize
												   <ListObjectPointersKillOnExitFakeSize
													   <ListObjectPointersKillOnExitFakeSize<QuasiPolynomial> > >
{
public: 
	void AddToEntry(int x, int y, int z, QuasiMonomial& QM);
};


class oneFracWithMultiplicitiesAndElongations
{
public:
	std::string DebugString;
	ListBasicObjects<int> Multiplicities;
	ListBasicObjects<int> Elongations;
	void ComputeDebugString();
	void ComputeDebugStringBasisChange(MatrixInt& VarChange);
	void AddMultiplicity(int MultiplicityIncrement, int Elongation);
	int IndexLargestElongation();
	int GetLargestElongation();
	int GetLCMElongations();
	int GetTotalMultiplicity();
	void invert();
	void init();
	static root CheckSumRoot;
	int HashFunction();
	void ComputeOneCheckSum(LargeRational &output, intRoot& theExp);
	bool IsHigherThan(oneFracWithMultiplicitiesAndElongations& f);
	void operator=(oneFracWithMultiplicitiesAndElongations& right);
	bool operator==(oneFracWithMultiplicitiesAndElongations& right);
	void ElementToString(std::string& output, int index, bool LatexFormat);
	void ElementToStringBasisChange(MatrixInt& VarChange, 
																	bool UsingVarChange, std::string& output,
																	bool LatexFormat, int index);
	void OneFracToStringBasisChange(int indexElongation, MatrixInt& VarChange, 
																	bool UsingVarChange, std::string& output,
																	bool LatexFormat, int indexInFraction);
};

class SubsetWithMultiplicities
{
private:
public:
	ListBasicObjects<int> elements;
	ListBasicObjects<int> Multiplicities;
	int MaxMultiplicity;
	int CardinalitySelection();
	void ComputeElements();
	void init(int NumElements, int MaxMult);
	void IncrementSubset();	
};

class intRoot
{ 
private:
public: 
	unsigned char dimension;
	int elements[MaxRank];
	void AssignRoot(root&r);
	int HashFunction();
	void ElementToString(std::string& output);
	bool IsHigherThanWRTWeight(intRoot& r, intRoot& theWeights);
	bool IsGEQNoWeight(intRoot& r);
	void MakeZero();
	void MultiplyByInteger(int x)
	{	for (int i=0;i<this->dimension;i++)
		{ this->elements[i]*=x;
		}
	};
	void initFromInt(int x1,int x2,int x3, int x4, int x5);
	void initFromInt(	int x1,int x2,int x3, int x4, int x5, int x6,
										int x7,int x8,int x9, int x10, int x11, int x12);
	//remark: zero is considered to be a positive vector!
	bool IsPositive();
	void AddRoot(intRoot& theRoot);
	void operator=(intRoot& right);
	bool operator==(intRoot& right);
	void operator=(int* right);
	intRoot(){this->dimension=root::AmbientDimension; for (unsigned char i=0;i<this->dimension;i++)elements[i]=0;}
};

class rootWithMultiplicity: public root
{
public:
	int multiplicity;
	void operator=(rootWithMultiplicity& right)
	{	this->multiplicity= right.multiplicity;
		this->root::operator= (right);
	};
	bool operator==(rootWithMultiplicity& right)
	{ return	this->root::operator ==(right)	&& 
						this->multiplicity== right.multiplicity;
	};
	bool IsGreaterThanOrEqualTo(rootWithMultiplicity& right)
	{ for (int i=0;i<this->dimension;i++)
		{ if (right.coordinates[i].IsGreaterThan(right.coordinates[i]))
				return false;
		}
		return true;
	};
};


class rootsWithMultiplicity: public ListBasicObjects<rootWithMultiplicity>
{ 
public:
	void BubbleSort()
	{ for (int i=0;this->size;i++)
		{ for (int j=i+1;j<this->size;j++)
			{ if (this->TheObjects[j].IsGreaterThanOrEqualTo(this->TheObjects[i]))
				{ rootWithMultiplicity tempRoot;
					tempRoot= this->TheObjects[i];
					this->TheObjects[i]=this->TheObjects[j];
					this->TheObjects[j]=tempRoot;
				}
			}
		}
	};
};

class intRoots: public ListBasicObjects<intRoot>
{
public:
	void AssignRoots(roots& r);
	void ElementToString(std::string& output);
	void BubbleSort(intRoot* weights);
	void ComputeDebugString();
	std::string DebugString;
};

class RootToIndexTable : public HashedListBasicObjects<intRoot>
{
public:
	int TableAllowedAminusB[MaxNumberOfRoots][MaxNumberOfRoots];
	int TableAllowedAminus2B[MaxNumberOfRoots][MaxNumberOfRoots];
	Selection IndicesRedundantShortRoots;
	int IndicesDoublesOfRedundantShortRoots[MaxNumberOfRoots];
	int NumNonRedundantShortRoots;
	intRoot weights;
	void initFromRoots(intRoots& theAlgorithmBasis,intRoot* theWeights);
	int AddRootAndSort(intRoot& theRoot);
	int AddRootPreserveOrder(intRoot& theRoot);
	int getIndex(intRoot& TheRoot);
	void ComputeTable();
};

class partFractionPolynomials: public ListBasicObjects<PolynomialLargeRational>
{
public:
	roots LatticeIndicators;
	MatrixRational theNormals;
	void CheckConsistency(root& RootLatticeIndicator,PolynomialLargeRational& input);
	void initLatticeIndicatorsFromPartFraction(partFraction& owner);
	void AddPolynomialLargeRational(root& rootLatticeIndicator,PolynomialLargeRational& input);
	void ComputeQuasiPolynomial(QuasiPolynomial& output, bool RecordNumMonomials);
};

class partFraction: ListBasicObjects<oneFracWithMultiplicitiesAndElongations>
{
private: 
	void findPivot();
	void findInitialPivot();
	void intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent);
	bool rootIsInFractionCone(root& r);
	friend class partFractions;
	friend class partFractionPolynomials;
public:
	int LastGainingMultiplicityIndex;
	IntegerPoly Coefficient;
	QuasiPolynomial PowerSeriesCoefficient;
	partFractionPolynomials SplitPowerSeriesCoefficients;
	ListBasicObjects<int> IndicesNonZeroMults;
	std::string DebugString;
	int indexInGlobalCollectorPartFraction;
	int FileStoragePosition;
	bool PowerSeriesCoefficientIsComputed;
	bool RemoveRedundantShortRootsClassicalRootSystem(root*Indicator);
	bool RemoveRedundantShortRoots(root* Indicator);
	bool AlreadyAccountedForInGUIDisplay;
	static bool AnErrorHasOccurredTimeToPanic;
	static std::fstream TheBigDump;
	static bool UseGlobalCollector;
	static bool MakingConsistencyCheck;
	static LargeRational CheckSum, CheckSum2;
	static intRoot theVectorToBePartitioned;
	static ListObjectPointers<partFraction> GlobalCollectorPartFraction;
	void ComputePolyCorrespondingToOneMonomial
			(	PolynomialLargeRational &output, int index, roots& normals, 
				partFractionPolynomials* SplitPowerSeriesCoefficient);
	static void MakePolynomialFromOneNormal
						(	root& normal, root& shiftRational, int theMult,	PolynomialLargeRational& output);
	void ComputeNormals(roots& output);
	int ComputeGainingMultiplicityIndexInLinearRelation
				(	MatrixRational& theLinearRelation);
	void partFractionToPartitionFunctionSplit
					(	QuasiPolynomial& output, bool RecordNumMonomials,
						bool RecordSplitPowerSeriesCoefficient, bool StoreToFile);
	
	void partFractionToPartitionFunctionStoreAnswer
				(	QuasiPolynomial& output, bool RecordSplitPowerSeriesCoefficient, 
					bool StoreToFile);
	static RootToIndexTable RootsToIndices;
	void ComputeDebugString();
	void ComputeDebugStringBasisChange(MatrixInt& VarChange);
	void InsertNewRootIndex(int index);
	void MultiplyMinusRootShiftBy (int* theRoot, int Multiplicity);
	void MultiplyCoeffBy(Rational& r);
	void decomposeAMinusNB(int indexA, int indexB, int n, 
												 int indexAminusNB, partFractions& Accum);
	bool DecomposeFromLinRelation(MatrixRational& theLinearRelation, partFractions& Accum);
	void ComputeOneCheckSum(LargeRational& output);
	void ApplySzenesVergneFormula			
			(	ListBasicObjects<int> &theSelectedIndices, ListBasicObjects<int>& theElongations, 
				int GainingMultiplicityIndex,int ElongationGainingMultiplicityIndex, 
				partFractions& Accum);
	bool reduceOnceTotalOrderMethod(partFractions&Accum);
	bool reduceOnceGeneralMethod(partFractions&Accum);
	bool AreEqual(partFraction& p);
	bool IsReduced();
	int HashFunction();
	int MultiplyByOneFrac(oneFracWithMultiplicitiesAndElongations& f);
	void init(int numRoots);
	int Elongate(int indexElongatedFraction, int theElongation);
	void ComputeIndicesNonZeroMults();
	bool DecreasePowerOneFrac(int index, int increment);
	void GetNumerator(PolynomialRationalCoeff& output);
	void SetNumerator(PolynomialRationalCoeff& input);
	void PrepareFraction(int indexA, int indexB,  int AminusNBindex,
											 bool indexAisNullified, partFraction &output,
											 IntegerPoly& AminusNbetaPoly); 
	void Assign(partFraction&p);
	void AssignNoIndicesNonZeroMults(partFraction&p);
	int ControlLineSizeFracs(std::string& output);
	int ControlLineSizeStringPolys(std::string& output);
	//void swap(int indexA,int indexB);
	partFraction();
	~partFraction();
	void GetAlphaMinusNBetaPoly(int indexA, int indexB, int n, IntegerPoly& output);
	void GetNElongationPoly(int index,int Elongation,int n, IntegerPoly& output);
	bool operator==(partFraction& right);
	void operator=(partFraction& right);
	void initFromRootSystem(intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights);
	int ElementToString(std::string& output, bool LatexFormat,
											bool includeVPsummand,bool includeNumerator);
	int ElementToStringBasisChange(MatrixInt& VarChange, 
																	bool UsingVarChange, std::string& output,
																	bool LatexFormat,bool includeVPsummand,bool includeNumerator);
	void ReadFromFile(std::fstream& input);
	void WriteToFile(std::fstream& output);
	int SizeWithoutDebugString();
};

class partFractions: public HashedListBasicObjects<partFraction>
{ bool ShouldIgnore();
	void AssureIndicatorRegularity();
public:
	int IndexLowestNonReduced;
	int HighestIndex;	
	std::string DebugString;
	bool UsingIndicatorRoot;
	bool DiscardingFractions;
	root IndicatorRoot;
	LargeRational StartCheckSum;
	LargeRational EndCheckSum;
	static bool SplitTestMode;
	static bool AnErrorHasOccurredTimeToPanic;
	static LargeRational CheckSum;
	static std::fstream ComputedContributionsList;
	static bool MakingProgressReport;
	static bool UsingCheckSum;
	void PrepareCheckSums();
	void CompareCheckSums();
	void ComputeDebugString();
	void ComputeDebugStringNoNumerator();
	void ComputeDebugStringWithVPfunction();
	void ComputeDebugStringBasisChange(MatrixInt& VarChange);
	void initFromRootSystem(intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights);
	//row index is the index of the root; column(second) index is the coordinate index
	void RemoveRedundantShortRootsClassicalRootSystem();
	void RemoveRedundantShortRoots();
	bool splitClassicalRootSystem();
	bool split();
	void ComputeOneCheckSum(LargeRational& output);
	void Add(partFraction& f);
	void IncreaseHighestIndex(int increment);
	int ElementToString(std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator);
	int ElementToStringBasisChange(MatrixInt& VarChange, bool UsingVarChange, std::string& output,
																	bool LatexFormat, bool includeVPsummand, bool includeNumerator);
	int ElementToStringOutputToFile(std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator);
	int ElementToStringBasisChangeOutputToFile(MatrixInt& VarChange, bool UsingVarChange, std::fstream& output,
																	bool LatexFormat, bool includeVPsummand, bool includeNumerator);
	bool partFractionsToPartitionFunctionAdaptedToRoot
					(	QuasiPolynomial& output, root& r,
						bool storeComputations, bool RecordSplitPowerSeriesCoefficient,bool StoreToFile,
						bool UseOldData);
	bool VerifyFileComputedContributions();
	void WriteToFileComputedContributions(std::fstream& output);
	int ReadFromFileComputedContributions(std::fstream& input);
	void WriteToFile(std::fstream& output);
	void ReadFromFile(std::fstream& input);
	partFractions();
	int SizeWithoutDebugString();
	bool CheckForMinimalityDecompositionWithRespectToRoot(root& r);
	void MakeProgressReport();
	int NumMonomialsInTheNumerators();
	void ComputeKostantFunctionFromWeylGroup
				(	char WeylGroupLetter, unsigned char WeylGroupNumber, 
					QuasiPolynomial& output, root* ChamberIndicator,bool UseOldData,bool StoreToFile);
};

class ElementWeylGroup: public ListBasicObjects<int>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString();
	int HashFunction();
	void operator=(const ElementWeylGroup& right);
	bool operator==(const ElementWeylGroup& right);
};

class hashedRoots: public HashedListBasicObjects<root>
{
public:
	std::string DebugString;
	void ComputeDebugString();
	void ElementToString(std::string& output);
};

class OneVarPolynomials: public ListBasicObjects<Polynomial<LargeInt> >
{
public:
	std::string DebugString;
	int theWeightIndex;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void ElementToString(std::string& output, int KLindex);
};


//this class is in effect a remake of Polynomial<int>; Use only to minimize RAM usage.
//The OneVarIntPolynomial assumes coefficients do not exceed int size.
//The OneVarIntPolynomial has 2 times the basic size of ListBasicObjects of "service"
//RAM usage. On a 32 bit machine that means it has 2*28=56 bytes of extra RAM besides the
//coefficient storage.
class OneVarIntPolynomial
{
public:
	ListBasicObjects<int> PolynomialPart;
	ListBasicObjects<int> RationalPart;
	std::string DebugString;
	void MakeConst(int c);
	void MakeMonomial (int coeff, int power);
	void AddMonomial  (int coeff, int power);
	void MakeQuadratic(int x2Term, int x1Term, int constTerm);
	void Nullify();
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void SubstitutionOneOverX();
	int Substitution(int x);
	void MultiplyBy(OneVarIntPolynomial& p);
	void AddPolynomial(OneVarIntPolynomial& p);
	void Assign(OneVarIntPolynomial& p)
	{	this->PolynomialPart.CopyFromBase(p.PolynomialPart);
		this->RationalPart.CopyFromBase(p.RationalPart);
	};
	void ReleaseMemory();
	void operator=(OneVarIntPolynomial& p){this->Assign(p);};
	void FitSize();
	static void SetSizeAtLeastInitProperly(ListBasicObjects<int>& theArray,int desiredSize);
};

class OneVarIntPolynomials: public ListBasicObjects<OneVarIntPolynomial>
{
public:
	std::string DebugString;
	int theWeightIndex;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void ElementToString(std::string& output, int KLindex);
};

class VermaModulesWithMultiplicities: public hashedRoots
{
public:
	WeylGroup* TheWeylGroup;
	ListBasicObjects<int> TheMultiplicities;
	ListBasicObjects<bool> Explored;
	int NextToExplore;
	int LowestNonExplored;
	ListBasicObjects<ListBasicObjects<int> > BruhatOrder;
	ListBasicObjects<ListBasicObjects<int> > SimpleReflectionsActionList;
	ListBasicObjects<ListBasicObjects<int> > InverseBruhatOrder;
	//important: in both the R- and KL-polynomials, if a polynomial Rxy is non-zero,
	//then x is bigger than y. This is the opposite to the usually accepted convention!
	//The reason for that is the following: if you want to compute 
	//once you are done with computing with a given highest weight,
	//you want to be able to release the used memory; that is why the higher weight must
	//be the first, not the second index!
	ListBasicObjects<OneVarIntPolynomials> KLPolys;
	ListBasicObjects<OneVarIntPolynomials> RPolys;
	void KLcoeffsToString(ListBasicObjects<int>& theKLCoeffs, std::string& output);
	void FindNextToExplore();
	int FindLowestBruhatNonExplored();
	int FindHighestBruhatNonExplored(ListBasicObjects<bool>& theExplored);
	void initTheMults();
	void Compute(int x);
	void Check();
	//returns true if reduction succeeded, false otherwise
	bool ComputeRxy(int x, int y, int SimpleReflectionIndex);
	void ComputeKLxy(int w, int x);
	bool IsMaxNonEplored(int index);
	bool IndexGEQIndex(int a, int b);
	bool IndexGreaterThanIndex(int a, int b);
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void MergeBruhatLists(int fromList, int toList); 
	void KLPolysToString(std::string& output);
	void ComputeKLcoefficientsFromIndex(int ChamberIndex, ListBasicObjects<int>& output);
	void ComputeKLcoefficientsFromChamberIndicator(root& ChamberIndicator, ListBasicObjects<int>& output);
	int ChamberIndicatorToIndex(root& ChamberIndicator);
	void RPolysToString(std::string& output);
	void ComputeKLPolys(WeylGroup* theWeylGroup,int TopChamberIndex);
	void ComputeRPolys();
	int ComputeProductfromSimpleReflectionsActionList(int x, int y);
	void WriteKLCoeffsToFile(std::fstream& output,ListBasicObjects<int>& KLcoeff, int TopIndex);
	//returns the TopIndex of the KL coefficients
	int ReadKLCoeffsFromFile(std::fstream& input, ListBasicObjects<int>& output);
	VermaModulesWithMultiplicities(){this->TheWeylGroup=0;};
	void GeneratePartialBruhatOrder();
	void ExtendOrder();
	void ComputeFullBruhatOrder();
	void initFromWeyl (WeylGroup* theWeylGroup);
};

class WeylGroup: public HashedListBasicObjects<ElementWeylGroup>
{
public:
	std::string DebugString;
	MatrixInt KillingFormMatrix;
	root rho;
	hashedRoots RootSystem;
	roots RootsOfBorel;
	void ComputeRho();
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void MakeArbitrary(char WeylGroupLetter,unsigned char n);
	void MakeAn(unsigned char n);
	void MakeEn(unsigned char n);
	void MakeBn(unsigned char n);
	void MakeCn(unsigned char n);
	void MakeDn(unsigned char n);
	void MakeF4();
	void MakeG2();
	void ComputeWeylGroup();
	void ComputeWeylGroupAndRootsOfBorel(roots& output);
	void ComputeRootsOfBorel(roots& output);
	void GenerateOrbitAlg(root& ChamberIndicator,
												PolynomialsRationalCoeff& input,
												PolynomialsRationalCoeffCollection& output,
												bool RhoAction, bool PositiveWeightsOnly, 
												Cone* LimitingCone, bool onlyLowerWeights);
	void GenerateOrbit(	roots& theRoots, bool RhoAction, 
											hashedRoots& output);
	void GenerateOrbit(	roots& theRoots, bool RhoAction, 
											hashedRoots& output,
											bool ComputingAnOrbitGeneratingSubsetOfTheGroup,
											WeylGroup& outputSubset);
	void GenerateRootSystemFromKillingFormMatrix();
	//theRoot is a list of the simple coordinates of the root
	void ActOnRootAlgByGroupElement(
							int index, PolynomialsRationalCoeff& theRoot,
							bool RhoAction);
	void ActOnRootByGroupElement(int index, root& theRoot, 
																	bool RhoAction);
	void SimpleReflectionIntRoot(int index, root& theRoot, bool RhoAction);
	void SimpleReflectionRootAlg
						(	int index, PolynomialsRationalCoeff& theRoot, 
							bool RhoAction);
	void ReflectBetaWRTAlpha(root& alpha, root &beta, bool RhoAction, root& output);
	void RootScalarKillingFormMatrixRoot(root&r1, root& r2, Rational& output);
	int length(int index);
};

class rootFKFTcomputation
{
public:
	intRoots nilradicalA2A1A1inD5; 
	intRoots AlgorithmBasisA2A1A1inD5; 
	intRoot weights;
	partFractions partitionA2A1A1inD5;
	bool useOutputFileForFinalAnswer;
	void MakeRootFKFTsub(root& direction, QPSub& theSub);
	void initA2A1A1inD5();
	void RunA2A1A1inD5();
	void RunA2A1A1inD5beta12221(bool precomputedPartition, bool precomputedKLcoeff);
	void processA2A1A1inD5beta12221Answer(QuasiPolynomial& theAnswer);
	//format: the polynomials must be root::AmbientDimension in count
	//these are the coordinates in simple basis of the vector. 
	//They are allowed to be arbitrary
	//polynomials. The chamberIndicator indicates which chamber 
	//(in the partition function sense) is the input vector located.	
	//setting PositiveWeightsOnly makes the program generate only elements 
	//of the orbit with Borel-positive weights
	//give a Limiting cone if you want only to observe weights lying in a
	//certain cone. Set 0 if there is no particular cone of interest.
	void MakeTheRootFKFTSum(root& ChamberIndicator, partFractions& theBVdecomposition,
													ListBasicObjects<int>& theKLCoeffs, QuasiPolynomial& output,
													VermaModulesWithMultiplicities& theHighestWeights,
													roots& theNilradical);
};

class LaTeXProcedures
{
public:
	void drawline(	double X1, double Y1, double X2, double Y2, 
									unsigned long thePenStyle, int ColorIndex, std::fstream& output);

};

class thePFcomputation
{
public:
	int LCMdeterminants;
	int TotalNumEnumerated;
	ListBasicObjects<Selection> tableForbidden;
	ListBasicObjects<Selection> theForbiddenSelections;
	void ComputeDeterminantSelection();
	Selection theSelection;
	roots AllRoots;
	void EnumerateRecursively(int depth, int startingIndex);
	std::string DebugString;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void SelectionToMatrixRational(MatrixRational& output);
	void SelectionToString(std::string& output);
	void ComputeTableAllowed();
//	bool CheckAvailabilityIndex(int depth, int index);
	WeylGroup theWeylGroup;
	MatrixRational theKillingForm;
	void ComputeNewRestrictionsFromOld(int depth, int newIndex);
	void Run();
};

class rootSubalgebra
{
public:
	roots AllRoots;
	roots AllPositiveRoots;
	WeylGroup AmbientWeyl;
	roots genK;
	roots SimpleBasisK;
	roots PositiveRootsK;
	roots AllRootsK;
	roots LowestWeightsGmodK;
	roots HighestRootsK;
	roots TestedRootsAlpha;
	ListBasicObjects<roots> kModules;
	ListBasicObjects<roots> PosRootsKConnectedComponents;
	ListBasicObjects<Selection> theKEnumerations;
	ListBasicObjects<int> theKComponentRanks;
	std::string DebugString;
	void KEnumerationsToLinComb();
	void DoKRootsEnumeration();
	void DoKRootsEnumerationRecursively(int indexEnumeration);
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void initFromAmbientWeyl();
	void ComputeAll();  
	void ComputeRootsOfK();  
	void TransformToSimpleBasisGenerators(roots& theGens);
	void ComputeKModules();
	void ComputeHighestWeightInTheSameKMod(root& input, root& outputHW);
	void ComputeExtremeWeightInTheSameKMod(root& input, root& outputW, bool lookingForHighest);
	void ComputeLowestWeightInTheSameKMod(root& input, root& outputLW);
	void RunE6_3A2();
	void RunE7_D6plusA1();
	void RunE7_A2plusA5();
	void RunE7_2A3plusA1();
	void RunE7_D4plus3A1();
	void RunE7_A7();
	void RunE7_7A1();	
	void RunE8_A8();
	void RunE8_D8();
	void RunE8_A1_A7();
	void RunE8_A5_A1_A2();
	void RunE8_2A4();
	void RunE8_4A2();
	void RunE8_A2_E6();
	void RunE8_A1_E7();
	void RunE8_D6_2A1();
	void RunE8_D5_A3();
	void RunE8_2D4();
	void RunE8_D4_4A1();
	void RunE8_2A3_2A1();
	void RunE8_8A1();
	void RunF4_B4();
	void RunF4_A3_A1();
	void RunF4_A2_A2();
	void RunF4_C3_A1();
	void RunF4_D4();
	void RunF4_B2_2A1();
	void RunF4_4A1();
	void GetLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration);
//	void commonCodeForGetLinearCombinationFromMaxRankRootsAndExtraRoot();
	void GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
	bool LinCombToString(root& alphaRoot, int coeff, root& linComb,std::string& output); 
	bool LinCombToStringDistinguishedIndex
				(int distinguished,root& alphaRoot, int coeff, root &linComb, std::string &output);
};

struct IndicatorWindowVariables
{ 
public:
	int NumPartFractionsLoaded;
	int NumReducedPartFractions;
	int NumComputedContributions;
	int TotalNumMonomials;
	int NumProcessedMonomials;
	int NumProcessedFractions;
	intRoot modifiedRoot;
	bool rootIsModified;
	bool PerturbationHasOccurred;
	bool Pause;
	std:: string StatusString;
	IndicatorWindowVariables(){this->Nullify();}
	std::string ProgressReportString1;
	std::string ProgressReportString2;
	std::string ProgressReportString3;
	std::string ProgressReportString4;
	void PrepareStrings();
	void Nullify()
	{ this->NumPartFractionsLoaded=0;
		this->Pause=true;
		this->NumComputedContributions=0;
		this->TotalNumMonomials=0;
		this->NumProcessedMonomials=0;
		this->NumProcessedFractions=0;
	};	
};

class RandomCodeIDontWantToDelete
{
public: 
	static void SomeRandomTests2();
	static void SomeRandomTests3();
	static ListBasicObjects<std::string> EvilList1,EvilList2;
	static bool UsingEvilList1;
	void SomeRandomTests4();
	static void SomeRandomTests5();
	static void SomeRandomTests6();
	void WeylChamberRandomCode();
	void KLPolysRandomCode();
	static void SomeRandomTestsIWroteMonthsAgo();
	static std::string theFirstEvilString, theSecondEvilString;
	QuasiPolynomial EvilPoly1, EvilPoly2;
	void RevealTheEvilConspiracy();
};

void delIntegersFastAccess(Selection& x);
void AddToIntegersFastAccess(Selection& x,int a);
void RationalPlusRational(Rational& a, Rational& b, Rational& output);
void RationalMinusRational(Rational& a, Rational& b, Rational& result);
void RationalDivRational(Rational& a, Rational& b, Rational& output);
void RationalTimesRational(Rational& a, Rational& b, Rational& output);
void RationalToDouble(Rational& r, double& output);
int RationalToInteger (Rational& r);
bool RationalEqualsRational(Rational &a, Rational &b);
void initIntegersFastAccessMemoryAllocation(Selection& x, int s);
void initIntegersFastAccessNoMemoryAllocation(Selection& x);
int gcd(int a, int b);
int lcm(int a, int b);
void SimplifyRational(Rational& r);
void initDLL(int rank);
void exitDLL();
void GaussianEliminationByRows(MatrixRational& mat, MatrixRational& output,Selection& outputNonPivotPoints);
void CopyMatrix(MatrixRational& FromMat, MatrixRational& ToMat); 
void MinusRoot(root& r);
int TwoToTheNth(int n);
int NChooseK(int n, int k);
int KToTheNth(int k, int n);
void SliceTheEuclideanSpace(roots& directions,int& index, int rank, 
														CombinatorialChamberPointers& output,
														FacetPointers& FacetOutput);
void SliceOneDirection(roots& directions, int& index, int rank, 
											 CombinatorialChamberPointers& output,
											 FacetPointers& FacetOutput);
void OneSlice(roots& directions, int& index, int rank, CombinatorialChamberPointers& output,
							FacetPointers& FacetOutput);
void drawOutput(DrawingVariables& TDV, CombinatorialChamberPointers& output, 
								roots& directions, int directionIndex,root& ChamberIndicator);
void ProjectOntoHyperPlane(root& input, root& normal, root& ProjectionDirection, root&output);


#endif
