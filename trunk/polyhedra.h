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

#ifndef polyhedra_h_already_included
#define polyhedra_h_already_included
#include <assert.h>
#include <sstream>
//#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#ifndef WIN32
#include <pthread.h>
#endif

#ifdef WIN32
//have to disable C4100 in VS because it warns me on passing non-used parameters to my functions.
//Those of course are passed to facilitate future extensions of functionality.
#pragma warning(disable:4100)//warning C4100: non-referenced formal parameter
//The below causes problems in VS with my debugging code (which I comment/uncomment often).
#pragma warning(disable:4189)//warning 4189: variable initialized but never used
#endif

#ifdef CGIversionLimitRAMuse
static int cgiLimitRAMuseNumPointersInListBasicObjects=9000000;
#endif

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
class affineCone;
class affineHyperplane;
class QuasiNumber;
class BasicQN;
class CombinatorialChamber;
class PolynomialsRationalCoeff;
class PrecomputedQuasiPolynomialIntegrals;
class hashedRoots;
class PrecomputedTauknPointersKillOnExit;
class QuasiPolynomial;
class VertexSelectionPointers;
class CombinatorialChamberContainer;
class CompositeComplexQN;
template <class ElementOfCommutativeRingWithIdentity>
class Polynomial;
template <class ElementOfCommutativeRingWithIdentity>
class PolynomialLight;
class Selection;
class IntegerPoly;
class Rational;
class rootSubalgebra;
class intRoot;
class PolyPartFractionNumerator;
class root;
class roots;
class rootsCollection;
template <class Object>
class ListBasicObjects;
template <class Object>
class ListObjectPointers;
template <class Object>
class HashedListBasicObjects;
class PrecomputedTauknPointers;
struct PolynomialOutputFormat;
class Rational;
class QPSub;
class partFractions;
class SubsetWithMultiplicities;
class hashedRoots;
class WeylGroup;
class intRoots;
class GlobalVariables;
class MathRoutines;
class GlobalVariablesContainer;
class MatrixIntTightMemoryFit;
class QuasiPolynomials;
class permutation;
template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
class MonomialInCommutativeAlgebra;
class affineCones;
struct IndicatorWindowVariables;
class rootSubalgebras;

extern ::PolynomialOutputFormat PolyFormatLocal; //a global variable in
//polyhedra.cpp used to format the polynomial printouts.


class ParallelComputing
{
public:
	static bool ReachSafePointASAP;
	static bool SafePointReached;
	static int GlobalPointerCounter;
#ifndef WIN32
	static pthread_mutex_t mutex1;
  static pthread_cond_t continueCondition;
#endif
	inline static void SafePoint()
	{ while(ParallelComputing::ReachSafePointASAP)
		{ //pthread_mutex_lock(&ParallelComputing::mutex1);
      ParallelComputing::SafePointReached=true;
#ifndef WIN32
      pthread_cond_wait(&ParallelComputing::continueCondition, &ParallelComputing::mutex1);
#endif
      //pthread_mutex_unlock(&ParallelComputing::mutex1);
		}
		ParallelComputing::SafePointReached=false;
	}
};

//#ifndef dont_define_Min_and_Max_in_polyhedra_h
//grrrrrrrr: names conflict
inline int Minimum(int a, int b)
{	if (a>b) {return b;} else {return a;}
}

inline int Maximum(int a, int b)
{	if (a>b) {return a;} else {return b;}
}
//#endif

typedef void (*drawLineFunction)
	(	double X1, double Y1, double X2, double Y2,
		unsigned long thePenStyle, int ColorIndex);
typedef void (*drawTextFunction)
	(	double X1, double Y1, const char* theText, int length, int ColorIndex);
typedef void (*FeedDataToIndicatorWindow)
	(	IndicatorWindowVariables& input);

struct DrawingVariables
{
public:
	static const int NumColors=8;
	//color styles (taken from windows.h and substituted for independence of the .h file):
	// 0 = normal line
	// 1 = dashed line
	// 2 = dotted line
	// 5 = invisible line (no line)
	static const int PenStyleInvisible= 5;
	static const int PenStyleDashed= 1;
	static const int PenStyleDotted= 2;
	static const int PenStyleNormal= 0;
	int ColorDashes;
	bool flagLaTeXDraw;
	bool flag2DprojectionDraw;
	bool DrawDashes;
	bool DrawChamberIndices;
	bool DrawingInvisibles;
	bool DisplayingChamberCreationNumbers;
	int Selected;
	double centerX;
	double centerY;
	int textX;
	int textY;
	double Projections[MaxRank][2];
	double scale;
	int TextColor;
	int ZeroChamberTextColor;
	int DeadChamberTextColor;
	int ColorChamberIndicator;
	int ColorWeylChamberWalls;
	int TextOutStyle;
	int TextOutStyleInvisibles;
	int DrawStyle;
	int DrawStyleDashes;
	int DrawStyleInvisibles;
	int DrawStyleDashesInvisibles;
	void drawCoordSystem
		(	DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile,
			drawTextFunction drawTextIn);
	void initDrawingVariables(int cX1, int cY1);
	int ColorsR[DrawingVariables::NumColors];
	int ColorsG[DrawingVariables::NumColors];
	int ColorsB[DrawingVariables::NumColors];
	int Colors[DrawingVariables::NumColors];
	DrawingVariables(int cx, int cy){this->initDrawingVariables(cx,cy);};
	int GetColorFromChamberIndex(int index, std::fstream* LaTexOutput);
	static void GetCoordsForDrawing(DrawingVariables& TDV, root& r,double& x, double& y);
	static void ProjectOnToHyperPlaneGraphics(root& input, root& output, roots& directions);
	void ApplyScale(double inputScale);
	void SetCoordsForG2();
	void SetCoordsForB2();
	void SetCoordsForA2();
	void SetCoordsForC2();
	void drawText
		(	double X1, double Y1, std::string& inputText,
			int color, std::fstream* LatexOutFile,
			drawTextFunction drawTextIn);
	//if the LatexOutFile is zero then the procedure defaults to the screen
	void drawLine
		(	double X1, double Y1, double X2, double Y2,
			unsigned long thePenStyle, int ColorIndex,
			std::fstream* LatexOutFile, drawLineFunction theDrawFunction);
	void drawlineBetweenTwoVectors
		(	root& r1, root& r2, int PenStyle, int PenColor,
			std::fstream* LatexOutFile, drawLineFunction theDrawFunction);
//	void drawlineBetweenTwoVectorsColorIndex(root& r1, root& r2, int PenStyle, int ColorIndex, std::fstream* LatexOutFile);
	void drawTextAtVector
		(	root& point, std::string& inputText, int textColor, std::fstream* LatexOutFile,
			drawTextFunction drawTextIn);
};

class MathRoutines
{
public:
	static int lcm(int a, int b);
	static int TwoToTheNth(int n);
	static int NChooseK(int n, int k){	int result=1;
																			for (int i =0; i<k;i++){
																				result*=(n-i);
																				result/=(i+1); }
																			return result; };
	static int KToTheNth(int k, int n);
	inline static int parity(int n){if (n%2==0) return 1; else return -1;};
	static int BinomialCoefficientMultivariate(int N, ListBasicObjects<int>& theChoices);
	inline static int Maximum(int a, int b){	if (a>b) return a;
																						else return b;};
	template <typename T>
	inline static void swap(T& a, T& b)
	{ T temp;
    temp=a;
    a=b;
    b=temp;
  };
	inline static int Minimum(int a, int b){	if (a>b) return b;
																						else return a;};
	inline static short Minimum(short a, short b){if (a>b) return b; else return a;};
};

//The below class is to be used together with ListBasicObjects.
//The purpose of the class is to save up RAM memory use.
//This is the "light" version it is to be used for storage purposes only.
//To use it as a normal ListBasicObjects simply copy it to a buffer ListBasicObjects and there
//use the full functionality of ListBasicObjects.
//Then copy the buffer ListBasicObjects back to the light version to store to RAM.
template <class Object>
class ListBasicObjectsLight
{
private:
	ListBasicObjectsLight(const ListBasicObjectsLight<Object>& right);
public:
	int size;
	Object* TheObjects;
	void AddObjectOnTopLight(const Object& o);
	void CopyFromHeavy(const ListBasicObjects<Object>& from);
	void CopyFromLight(const ListBasicObjectsLight<Object>& from);
	void PopIndexSwapWithLastLight(int index);
	int SizeWithoutObjects();
	void SetSizeExpandOnTopLight(int theSize);
	void operator = (const ListBasicObjectsLight<Object>& right);
	void operator == (const ListBasicObjectsLight<Object>& right);
	inline Object* LastObject(){return &this->TheObjects[this->size-1];};
	ListBasicObjectsLight();
	~ListBasicObjectsLight();
};

template <class Object>
void ListBasicObjectsLight<Object>::AddObjectOnTopLight(const Object& o)
{ this->SetSizeExpandOnTopLight(this->size+1);
	this->TheObjects[this->size-1]=o;
}

template <class Object>
void ListBasicObjectsLight<Object>::PopIndexSwapWithLastLight(int index)
{ this->TheObjects[index]=this->TheObjects[this->size-1];
	this->SetSizeExpandOnTopLight(this->size-1);
}

template <class Object>
inline void ListBasicObjectsLight<Object>::operator =(const ListBasicObjectsLight<Object>& right)
{ this->CopyFromLight(right);
}

template <class Object>
inline void ListBasicObjectsLight<Object>::SetSizeExpandOnTopLight(int theSize)
{ if (theSize== this->size)
		return;
	if (theSize<0)
		theSize=-1;
	if (theSize==0)
	{
#ifdef CGIversionLimitRAMuse
	ParallelComputing::GlobalPointerCounter-=this->size;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
		this->size=0;
		delete [] this->TheObjects;
		this->TheObjects=0;
		return;
	}
	Object* newArray= new Object[theSize];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=theSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	int CopyUpTo= this->size;
	if (this->size>theSize)
		CopyUpTo= theSize;
	for (int i=0;i<CopyUpTo;i++)
		newArray[i]=this->TheObjects[i];
	delete [] this->TheObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->size;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->TheObjects=newArray;
	this->size= theSize;
}

template <class Object>
ListBasicObjectsLight<Object>::ListBasicObjectsLight()
{ this->size =0;
	this->TheObjects=0;
}

template <class Object>
ListBasicObjectsLight<Object>::~ListBasicObjectsLight()
{ delete [] this->TheObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->size;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->TheObjects=0;
}

template <class Object>
void ListBasicObjectsLight<Object>::CopyFromHeavy(const ListBasicObjects<Object>& from)
{ this->SetSizeExpandOnTopLight(from.size);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i]= from.TheObjects[i];
}

template <class Object>
void ListBasicObjectsLight<Object>::CopyFromLight(const ListBasicObjectsLight<Object>& from)
{ this->SetSizeExpandOnTopLight(from.size);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i]= from.TheObjects[i];
}

template <class Object>
int ListBasicObjectsLight<Object>::SizeWithoutObjects()
{ return sizeof(Object*)*this->size+sizeof(int);
}


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
	void AssignLight(const ListBasicObjectsLight<Object>& from);
	void SetSizeExpandOnTopNoObjectInit(int theSize);
	void initFillInObject(int theSize, const Object& o);
	inline void AddObjectOnTopCreateNew();
	void MakeActualSizeAtLeastExpandOnTop(int theSize);
	void AddObjectOnBottom(const Object& o);
	void AddObjectOnTop(const Object& o);
	void AddListOnTop(ListBasicObjects<Object>& theList);
	bool AddObjectOnTopNoRepetitionOfObject(const Object& o);
	void PopIndexShiftUp(int index);
	void PopIndexShiftDown(int index);
	void PopIndexSwapWithLast(int index);
	void PopLastObject();
	void PopFirstOccurenceObjectSwapWithLast(Object& o);
	void SwapTwoIndices(int index1, int index2);
	void ElementToStringGeneric(std::string& output);
	void ElementToStringGeneric(std::string& output, int NumElementsToPrint);
	//careful output is not bool but int!!!!
	int IndexOfObject(const Object& o);
	bool ContainsObject(const Object& o){return this->IndexOfObject(o)!=-1;};
//	inline bool ContainsObject(Object& o){return this->ContainsObject(o)!=-1;};
	int SizeWithoutObjects();
	inline Object* LastObject();
	void ReleaseMemory();
	void operator=(const ListBasicObjects<Object>& right){this->CopyFromBase(right);};
	static void swap(ListBasicObjects<Object>&l1, ListBasicObjects<Object>&l2);
	void ReverseOrderElements();
	void CopyFromBase (const ListBasicObjects<Object>& From);
	void ShiftUpExpandOnTop(int StartingIndex);
	ListBasicObjects();
	~ListBasicObjects();
};

template <class Object>
class ListObjectPointers: public ListBasicObjects<Object*>
{
public:
	//ListObjectPointers();
	void KillAllElements();
	void KillElementIndex(int i);
	bool AddObjectNoRepetitionOfPointer(Object* o);
	void PopAllOccurrencesSwapWithLast(Object*o);
	int ObjectPointerToIndex(Object*o);
	void resizeToLargerCreateNewObjects(int increase);
	void IncreaseSizeWithZeroPointers(int increase);
	void initAndCreateNewObjects(int d);
	bool IsAnElementOf(Object* o);
};

template <class Object>
class HashedListBasicObjects : public ListBasicObjects<Object>
{
private:
	void AddObjectOnBottom(const Object& o);
	void AddObjectOnTop(const Object& o);
	void AddListOnTop(ListBasicObjects<Object>& theList);
	bool AddObjectOnTopNoRepetitionOfObject(const Object& o);
	void PopIndexShiftUp(int index);
	void PopIndexShiftDown(int index);
	void PopIndexSwapWithLast(int index);
	void PopFirstOccurenceObjectSwapWithLast(Object& o);
	void CopyFromBase (const ListBasicObjects<Object>& From);
	void SwapTwoIndices(int index1, int index2);
	void operator=(const ListBasicObjects<Object>& right){this->CopyFromBase(right);};
	void AssignLight(const ListBasicObjectsLight<Object>& from);
	void SetSizeExpandOnTopNoObjectInit(int theSize);
	int IndexOfObject(const Object& o);
	bool ContainsObject(const Object& o);
	void ReverseOrderElements();
	void ShiftUpExpandOnTop(int StartingIndex);
	void PopLastObject();
protected:
	friend class partFractions;
	friend class QuasiMonomial;
	friend class CombinatorialChamber;
	friend class QuasiPolynomial;
	void ClearHashes();
	ListBasicObjects<int>* TheHashedArrays;
public:
	static int PreferredHashSize;
	int HashSize;
	void initHash();
	inline int FitHashSize( int i){i%=this->HashSize; if (i<0) i+=this->HashSize; return i;};
	void ClearTheObjects();
	void AddObjectOnTopHash(Object& o);
	void AddObjectOnTopNoRepetitionOfObjectHash(Object& o);
	void PopIndexSwapWithLastHash(int index);
	//the below returns -1 if it doesn't contain the object,
	//else returns the object's index
	void SwapTwoIndicesHash(int i1, int i2);
	inline bool ContainsObjectHash(Object& o) {return this->IndexOfObjectHash(o)!=-1;};
	int IndexOfObjectHash(Object& o);
	void SetHashSize(int HS);
	int SizeWithoutObjects();
	HashedListBasicObjects();
	~HashedListBasicObjects();
	void CopyFromHash(const HashedListBasicObjects<Object>& From);
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
	inline void operator=(const Integer& y){this->value=y.value;};
	inline bool operator==(Integer& y){return this->value==y.value;};
	inline void Assign(const Integer&y){this->value=y.value;};
	inline bool IsEqualTo(Integer&y){return this->value==y.value;};
	inline bool IsEqualToZero(){return this->value==0;}
	inline void DivideBy(Integer&y){this->value/=y.value;};
	inline void WriteToFile(std::fstream& output){output<<this->value;};
	inline void ReadFromFile(std::fstream& input){input>>this->value;};
	inline void ElementToString(std::string& output)
	{ std::stringstream out; out<<this->value; output= out.str();
	};
	Integer(int x){this->value=x;};
	Integer(){};
};

template <typename Element>
class MatrixElementaryLooseMemoryFit
{
public:
	int NumRows; int ActualNumRows;
	int NumCols; int ActualNumCols;
	Element** elements;
	void init(int r,int c);
	void Free();
	bool IsEqualTo(MatrixElementaryLooseMemoryFit<Element>& right);
	void Resize(int r, int c, bool PreserveValues);
	void Assign(const MatrixElementaryLooseMemoryFit<Element>& m);
	void MakeIdMatrix(int theDimension);
	inline void operator=(const MatrixElementaryLooseMemoryFit<Element>& right){this->Assign(right);};
	MatrixElementaryLooseMemoryFit<Element>();
	~MatrixElementaryLooseMemoryFit<Element>();
};

template <typename Element>
MatrixElementaryLooseMemoryFit<Element>::MatrixElementaryLooseMemoryFit()
{ this->elements=0;
	this->NumCols=0;
	this->NumRows=0;
	this->ActualNumRows=0;
	this->ActualNumCols=0;
}

template <typename Element>
MatrixElementaryLooseMemoryFit<Element>::~MatrixElementaryLooseMemoryFit()
{ this->Free();
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::init(int r, int c)
{ this->Resize(r,c,false);
}

template <typename Element>
bool MatrixElementaryLooseMemoryFit<Element>::IsEqualTo(MatrixElementaryLooseMemoryFit<Element> &right)
{ if (this->NumCols!=right.NumCols || this->NumRows!=right.NumRows)
		return false;
	for (int i=0;i<this->NumRows;i++)
		for (int j=0; j<this->NumCols;j++)
			if(!(this->elements[i][j]==right.elements[i][j]))
				return false;
	return true;
}

template <typename Element>
void MatrixElementaryLooseMemoryFit<Element>::MakeIdMatrix(int theDimension)
{ this->init(theDimension, theDimension);
	for (int i=0;i<theDimension;i++)
		for (int j=0;j<theDimension;j++)
			if (j!=i)
				this->elements[i][j].Assign(Element::TheRingZero);
			else
				this->elements[i][j].Assign(Element::TheRingUnit);
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::Resize
	(int r, int c, bool PreserveValues)
{ if (r<0) r=0;
	if (c<0) c=0;
	if (r==this->NumRows && c== this->NumCols)
		return;
	if (r==0 || c==0)
	{	this->NumRows=0;
		this->NumCols=0;
		return;
	}
	Element** newElements=0;
	int newActualNumCols= MathRoutines::Maximum(this->ActualNumCols,c);
	int newActualNumRows= MathRoutines::Maximum(this->ActualNumRows,r);
	if (r>this->ActualNumRows || c>this->ActualNumCols)
	{ newElements	= new Element*[newActualNumRows];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=newActualNumRows;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
		for (int i=0;i<newActualNumRows;i++)
		{	newElements[i]= new Element[newActualNumCols];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=newActualNumCols;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
		}
	}
	if (PreserveValues && newElements!=0)
		for (int j=::Minimum(this->NumRows,r)-1;j>=0;j--)
			for (int i=::Minimum(this->NumCols,c)-1;i>=0;i--)
				newElements[j][i]= this->elements[j][i];
	if (newElements!=0)
	{	this->Free();
		this->elements = newElements;
		this->ActualNumCols=newActualNumCols;
		this->ActualNumRows=newActualNumRows;
	}
	this->NumCols=c;
	this->NumRows=r;
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::Assign(const MatrixElementaryLooseMemoryFit<Element>& m)
{ if (this==&m) return;
	this->Resize(m.NumRows, m.NumCols, false);
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			this->elements[i][j]=m.elements[i][j];
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::Free()
{ for (int i=0;i<this->ActualNumRows;i++)
		delete [] this->elements[i];
	delete [] this->elements;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualNumRows*this->ActualNumCols+this->ActualNumRows;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->elements=0;
	this->NumCols=0;
	this->NumRows=0;
	this->ActualNumRows=0;
	this->ActualNumCols=0;
}

template <typename Element>
class MatrixElementaryTightMemoryFit
{
public:
	int NumRows;
	int NumCols;
	Element** elements;
	void init(int r,int c);
	void Free();
	void Resize(int r, int c, bool PreserveValues);
	void Assign(const MatrixElementaryTightMemoryFit<Element>& m);
	MatrixElementaryTightMemoryFit<Element>();
	~MatrixElementaryTightMemoryFit<Element>();
};

template <typename Element>
class Matrix: public MatrixElementaryLooseMemoryFit<Element>
{
public:
	std::string DebugString;
	static bool flagComputingDebugInfo;
	static std::string MatrixElementSeparator;
	void ComputeDebugString();
	void ElementToSting(std::string& output);
	void SwitchTwoRows( int row1, int row2);
	int FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex );
	void RowTimesScalar(int rowIndex, Element& scalar);
	void AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, Element& scalar);
	void MultiplyOnTheLeft(Matrix<Element>& input,Matrix<Element>& output);
	void MultiplyOnTheLeft(Matrix<Element>& input);
	//returns true if successful, false otherwise
//	bool ExpressColumnAsALinearCombinationOfColumnsModifyMyself
//		(Matrix<Element>& inputColumn,Matrix<Element>* outputTheGaussianTransformations Matrix<Element>& outputColumn);
	bool Invert(GlobalVariables& theGlobalVariables);
	void NullifyAll();
	//returns true if the system has a solution, false otherwise
	bool RowEchelonFormToLinearSystemSolution
		( Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide,
			Matrix<Element>& outputSolution);
	inline static void GaussianEliminationByRows
		(	Matrix<Element>& theMatrix, Matrix<Element>& otherMatrix,
			Selection& outputNonPivotPoints)
				{Matrix<Element>::GaussianEliminationByRows(theMatrix,otherMatrix,outputNonPivotPoints,true);};
	static void GaussianEliminationByRows
		(	Matrix<Element>& mat, Matrix<Element>& output,
			Selection& outputSelection, bool returnNonPivotPoints);
};

class MatrixLargeRational: public Matrix<Rational>
{
public:
	static bool flagAnErrorHasOccurredTimeToPanic;
	void ComputeDeterminantOverwriteMatrix( Rational& output);
	void NonPivotPointsToRoot(Selection& TheNonPivotPoints, int OutputDimension, root& output);
	void NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, MatrixLargeRational& output);
	void Transpose(GlobalVariables& theGlobalVariables);
	void MultiplyByInt(int x);
	void AssignMatrixIntWithDen(MatrixIntTightMemoryFit& theMat, int Den);
	void ScaleToIntegralForMinRationalHeightNoSignChange();
	void ComputeDebugString();
	void MultiplyByLargeRational(Rational& x);
	void DivideByRational(Rational& x);
	static bool SystemLinearInequalitiesHasSolution
		(	MatrixLargeRational& matA, MatrixLargeRational& matb,
			MatrixLargeRational& outputPoint);
	static bool SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
		(	MatrixLargeRational& matA, MatrixLargeRational& matb,
			MatrixLargeRational& outputSolution,GlobalVariables& theGlobalVariables);
	static void ComputePotentialChangeGradient
	(	MatrixLargeRational& matA, Selection& BaseVariables, int NumTrueVariables,
		int ColumnIndex, Rational &outputChangeGradient,
		bool &hasAPotentialLeavingVariable);
	static void GetMaxMovementAndLeavingVariableRow
	(	Rational &maxMovement, int& LeavingVariableRow, int EnteringVariable,
		int NumTrueVariables, MatrixLargeRational& tempMatA, MatrixLargeRational& matX,
		Selection& BaseVariables);
	int FindPositiveLCMCoefficientDenominatorsTruncated();
	int FindPositiveGCDCoefficientNumeratorsTruncated();
};

class Selection
{
public:
	int MaxSize;
	int* elements;
//	int* elementsInverseSelection;
	bool* selected;
	int CardinalitySelection;
	void AddSelectionAppendNewIndex(int index);
	void RemoveLastSelection();
	void init(int maxNumElements);
	void ComputeIndicesFromSelection();
	void initNoMemoryAllocation();
	int HashFunction();
	std::string DebugString;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void incrementSelection();
	int SelectionToIndex();
	void ExpandMaxSize();
	void ShrinkMaxSize();
	void MakeSubSelection(Selection& theSelection, Selection& theSubSelection);
	void incrementSelectionFixedCardinality(int card);
	void Assign(const Selection& right);
	inline void operator=(const Selection& right){this->Assign(right);};
	//warning: to call the comparison operator sucessfully, cardinalitySelection must
	//be properly computed!
	inline bool operator==(Selection& right);
	Selection();
	Selection(int m);
	~Selection();
};


class SelectionWithMultiplicities
{
public:
	std::string DebugString;
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
	void ElementToString(std::string& output);
	ListBasicObjects<int> elements;
	ListBasicObjects<int> Multiplicities;
	int CardinalitySelectionWithoutMultiplicities();
	void initMe(int NumElements);
	void ComputeElements();
};

class SelectionWithMaxMultiplicity: public SelectionWithMultiplicities
{
public:
	int MaxMultiplicity;
	void initMe2(int NumElements, int MaxMult);
	int NumCombinationsOfCardinality(int cardinality);
	void IncrementSubset();
	void IncrementSubsetFixedCardinality(int Cardinality);
	int CardinalitySelectionWithMultiplicities();
};

class SelectionWithDifferentMaxMultiplicities : public SelectionWithMultiplicities
{
public:
	ListBasicObjects<int> MaxMultiplicities;
	void initIncomplete(int NumElements){	this->MaxMultiplicities.SetSizeExpandOnTopNoObjectInit(NumElements);
                                        this->initMe(NumElements);};
	void clearNoMaxMultiplicitiesChange();
	void IncrementSubset();
	int getTotalNumSubsets();
	int TotalMultiplicity();
	int MaxTotalMultiplicity();
	void operator=(const SelectionWithDifferentMaxMultiplicities& right)
	{ this->Multiplicities.CopyFromBase(right.Multiplicities);
		this->MaxMultiplicities.CopyFromBase(right.MaxMultiplicities);
		this->elements.CopyFromBase(right.elements);
	};
};

class MatrixIntTightMemoryFit : public ::MatrixElementaryTightMemoryFit<int>
{
public:
	void NullifyAll()
	{ for (int i=0;i<this->NumRows;i++)
			for (int j=0; j<this->NumCols;j++)
				this->elements[i][j]=0;
	};
};


template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::Assign(const MatrixElementaryTightMemoryFit<Element>& m)
{ if (this==&m) return;
	this->init(m.NumRows, m.NumCols);
	for (int i=0;i<this->NumRows;i++)
		for (int j=0;j<this->NumCols;j++)
			this->elements[i][j]=m.elements[i][j];
}

template <typename Element>
MatrixElementaryTightMemoryFit<Element>::MatrixElementaryTightMemoryFit()
{ this->elements=0;
	this->NumCols=0;
	this->NumRows=0;
}

template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::Free()
{ for (int i=0;i<this->NumRows;i++)
		delete [] this->elements[i];
	delete [] this->elements;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->NumRows*this->NumCols+this->NumRows;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->elements=0;
	this->NumRows=0;
	this->NumCols=0;
}

template <typename Element>
bool Matrix<Element>::Invert(GlobalVariables& theGlobalVariables)
{ assert(this->NumCols==this->NumRows);
	if (this->flagComputingDebugInfo)
		this->ComputeDebugString();
	static Matrix tempMatrix;
	static Selection NonPivotPts;
	tempMatrix.init(this->NumRows, this->NumCols);
	tempMatrix.NullifyAll();
	for (int i=0;i<this->NumCols;i++)
		tempMatrix.elements[i][i].MakeOne();
	this->GaussianEliminationByRows(*this,tempMatrix,NonPivotPts);
	if(NonPivotPts.CardinalitySelection!=0)
		return false;
	else
	{	this->Assign(tempMatrix);
		if (this->flagComputingDebugInfo)
			this->ComputeDebugString();
		return true;
	}
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(Matrix<Element>& input)
{ Matrix<Element> tempMat;
	this->MultiplyOnTheLeft(input,tempMat);
	this->Assign(tempMat);
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft
	(Matrix<Element>& input, Matrix<Element>& output)
{ assert(&output!=this && &output!=&input);
	assert(this->NumRows==input.NumCols);
	Element tempEl;
	output.init(input.NumRows, this->NumCols);
	for (int i=0; i< input.NumRows;i++)
		for( int j=0;j< this->NumCols;j++)
		{ output.elements[i][j].Assign(Element::TheRingZero);
			for (int k=0;k<this->NumRows;k++)
			{ tempEl.Assign(input.elements[i][k]);
				tempEl.MultiplyBy(this->elements[k][j]);
				output.elements[i][j].Add(tempEl);
			}
		}
}

template <typename Element>
inline void Matrix<Element>::ElementToSting(std::string& output)
{ std::stringstream out;
	std::string tempS;
	for (int i=0;i<this->NumRows;i++)
	{ for (int j=0;j<this->NumCols;j++)
		{ this->elements[i][j].ElementToString(tempS);
			out<< tempS;
			out<< this->MatrixElementSeparator;
		}
		out <<"\n";
	}
	output= out.str();
}

template <typename Element>
inline void Matrix<Element>::ComputeDebugString()
{ this->ElementToSting(this->DebugString);
}

template <typename Element>
inline int Matrix<Element>::FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex )
{	for(int i = RowStartIndex;i<= RowEndIndex; i++)
	{	if (!this->elements[i][columnIndex].IsEqualToZero())
			return i;
	}
	return -1;
};


template <typename Element>
inline void Matrix<Element>::AddTwoRows
	(int fromRowIndex, int ToRowIndex, int StartColIndex, Element& scalar)
{	Element tempElement;
	for (int i = StartColIndex; i< this->NumCols; i++)
	{	tempElement.Assign(this->elements[fromRowIndex][i]);
		tempElement.MultiplyBy(scalar);
		this->elements[ToRowIndex][i].Add(tempElement);
	}
}

template <typename Element>
inline void Matrix<Element>::RowTimesScalar(int rowIndex, Element& scalar)
{	for (int i=0; i<this->NumCols; i++)
		this->elements[rowIndex][i].MultiplyBy(scalar);
}

template <typename Element>
inline void Matrix<Element>::SwitchTwoRows( int row1, int row2)
{	Element tempElement;
	for(int i = 0; i<this->NumCols; i++)
	{	tempElement.Assign(this->elements[row1][i]);
		this->elements[row1][i].Assign(this->elements[row2][i]);
		this->elements[row2][i].Assign(tempElement);
	}
}

template <typename Element>
bool Matrix<Element>::RowEchelonFormToLinearSystemSolution
	( Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide,
			Matrix<Element>& outputSolution)
{ assert(	inputPivotPoints.MaxSize==this->NumCols && inputRightHandSide.NumCols==1
					&& inputRightHandSide.NumRows==this->NumRows);
	outputSolution.init(this->NumCols,1);
	int NumPivots=0;
	for (int i=0;i<this->NumCols;i++)
		if (inputPivotPoints.selected[i])
		{	outputSolution.elements[i][0].Assign(inputRightHandSide.elements[NumPivots][0]);
			NumPivots++;
		}
		else
			outputSolution.elements[i][0].MakeZero();
	for (int i=NumPivots;i<this->NumRows;i++)
		if (!inputRightHandSide.elements[i][0].IsEqualToZero())
			return false;
	return true;
}

template <typename Element>
void Matrix<Element>::GaussianEliminationByRows
	(	Matrix<Element>& mat, Matrix<Element>& output,
		Selection& outputSelection, bool returnNonPivotPoints)
{	int tempI;
	int NumFoundPivots = 0;
	int MaxRankMat = Minimum(mat.NumRows, mat.NumCols);
	Element tempElement;
	outputSelection.init(mat.NumCols);
	for (int i=0; i<mat.NumCols; i++)
	{	if (NumFoundPivots == MaxRankMat)
		{	if (returnNonPivotPoints)
				for (int j =i; j<mat.NumCols; j++)
					outputSelection.AddSelectionAppendNewIndex(j);
			return;
		}
		tempI = mat.FindPivot(i, NumFoundPivots, mat.NumRows - 1);
		if (tempI!=-1)
		{	if (tempI!=NumFoundPivots)
			{	mat.SwitchTwoRows(NumFoundPivots, tempI);
				output.SwitchTwoRows (NumFoundPivots, tempI);
			}
			tempElement.Assign(mat.elements[NumFoundPivots][i]);
			tempElement.Invert();
			mat.RowTimesScalar(NumFoundPivots, tempElement);
			output.RowTimesScalar(NumFoundPivots, tempElement);
			for (int j = 0; j<mat.NumRows; j++)
			{	if (j!=NumFoundPivots)
				{	if (!mat.elements[j][i].IsEqualToZero())
					{ tempElement.Assign(mat.elements[j][i]);
						tempElement.Minus();
						mat.AddTwoRows (NumFoundPivots, j, i, tempElement);
	          output.AddTwoRows (NumFoundPivots, j, 0, tempElement);
	          //mat.ComputeDebugString();
					}
				}
			}
			NumFoundPivots++;
			if (!returnNonPivotPoints)
				outputSelection.AddSelectionAppendNewIndex(i);
		}
		else
			if (returnNonPivotPoints)
				outputSelection.AddSelectionAppendNewIndex(i);
	}
}

template <typename Element>
MatrixElementaryTightMemoryFit<Element>::~MatrixElementaryTightMemoryFit()
{ this->Free();
}

template <typename Element>
inline void Matrix<Element>::NullifyAll()
{ for (int i=0;i<this->NumRows;i++)
		for (int j=0; j<this->NumCols;j++)
			this->elements[i][j].MakeZero();
}

template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::init(int r, int c)
{ this->Resize(r,c,false);
}

template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::Resize
	(int r, int c, bool PreserveValues)
{ if (r==this->NumRows && c== this->NumCols)
		return;
	if (r<=0)
	{	Free();
		return;
	}
	Element** newElements= new Element*[r];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=r;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	for (int i=0;i<r;i++)
	{ newElements[i]= new Element[c];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=c;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
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

class LargeIntUnsigned: public ListBasicObjects<unsigned int>
{	void AddNoFitSize(LargeIntUnsigned& x);
public:
	// Carry over bound is the "base" over which we work
	//Requirements on the CarryOverBound:
	//1.	CarryOverBound*2-1 must fit inside an unsigned (int)
	//		on the system
	//2. (CarryOverBound*2)^2-1 must fit inside (long long)
	//		on the system.
	////////////////////////////////////////////////////////
	//On a 32 bit machine any number smaller than 2^31 will work.
	//If you got no clue what to put just leave CarryOverBound= 2^31
	//static const unsigned int CarryOverBound=37;
	void SubtractSmallerPositive(LargeIntUnsigned& x);
	static const unsigned int CarryOverBound=2147483648UL;//=2^31
	//the below must be less than or equal to the square root of CarryOverBound.
	//it is used for quick multiplication of LargeRationals.
	static const int SquareRootOfCarryOverBound=32768;//=2^15
	void ElementToString(std::string& output);
	void DivPositive(LargeIntUnsigned &x, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const;
	void MakeOne();
	void Add(LargeIntUnsigned& x);
	void AddUInt(unsigned int x){static LargeIntUnsigned tempI; tempI.AssignShiftedUInt(x,0); this->Add(tempI);};
	void MakeZero();
	bool IsEqualToZero(){return this->size==1 && this->TheObjects[0]==0;};
	bool IsEqualTo(LargeIntUnsigned& right);
	bool IsPositive(){ return this->size>1 || this->TheObjects[0]>0;};
	bool IsEqualToOne(){ return this->size==1 && this->TheObjects[0]==1;};
	bool IsGEQ(LargeIntUnsigned& x);
	static void gcd(LargeIntUnsigned&a,LargeIntUnsigned&b,LargeIntUnsigned& output);
	void MultiplyBy(LargeIntUnsigned& right);
	void MultiplyBy(LargeIntUnsigned& x, LargeIntUnsigned& output);
	void MultiplyByUInt(unsigned int x);
	void AssignShiftedUInt(unsigned int x, int shift);
	void Assign(const LargeIntUnsigned& x){this->CopyFromBase(x);};
	int GetUnsignedIntValueTruncated();
	int operator %(unsigned int x);
	inline void operator = (const LargeIntUnsigned& x)
	{ this->Assign(x);
	};
	LargeIntUnsigned operator/(unsigned int x)const;
	LargeIntUnsigned operator/(LargeIntUnsigned& x)const;
	LargeIntUnsigned(const LargeIntUnsigned& x){ this->Assign(x);};
	LargeIntUnsigned(){this->SetSizeExpandOnTopNoObjectInit(1); this->TheObjects[0]=0;};
	//must be rewritten:
	double GetDoubleValue();
	void FitSize();
};

class LargeInt
{	friend class Rational;
public:
	signed char sign;
	LargeIntUnsigned value;
	void MultiplyBy(LargeInt& x){	this->sign*=x.sign;
																this->value.MultiplyBy(x.value);
															};
	void MultiplyByInt(int x);
	void ElementToString(std::string& output);
	bool IsPositive(){return this->sign==1 && (this->value.IsPositive());};
	bool IsNegative(){return this->sign==-1&& (this->value.IsPositive());};
	bool IsNonNegative(){return !this->IsNegative();};
	bool IsNonPositive(){return !this->IsPositive();};
	bool IsEqualTo(LargeInt& x);
	bool IsEqualToZero(){ return this->value.IsEqualToZero();}
	void Assign(const LargeInt& x);
	void AssignInt(int x);
	void Add(LargeInt& x);
	void AddLargeIntUnsigned(LargeIntUnsigned& x);
	void AddInt(int x);
	//bool IsGEQ(LargeInt& x);
	bool CheckForConsistensy();
	void MakeZero();
	void MakeOne(){this->value.MakeOne(); this->sign=1;};
	void MakeMOne();
	int GetIntValueTruncated(){return this->sign* this->value.GetUnsignedIntValueTruncated();};
	double GetDoubleValue();
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

class LargeRationalExtended
{
public:
  LargeInt num;
  LargeIntUnsigned den;
};

class Rational
{	inline bool TryToAddQuickly ( int OtherNum,
																int OtherDen)
	{ register int OtherNumAbs, thisNumAbs;
		assert(this->DenShort>0 && OtherDen>0);
	  if (OtherNum<0)
	  { OtherNumAbs=-OtherNum;
	  } else
	  { OtherNumAbs=OtherNum;
    }
	  if (this->NumShort<0)
	  { thisNumAbs=-this->NumShort;
	  } else
	  { thisNumAbs=this->NumShort;
    }
		if (!this->flagMinorRoutinesOnDontUseFullPrecision &&
				(	this->Extended!=0
					||thisNumAbs>= LargeIntUnsigned::SquareRootOfCarryOverBound
					||this->DenShort>=LargeIntUnsigned::SquareRootOfCarryOverBound
					||OtherNumAbs>=LargeIntUnsigned::SquareRootOfCarryOverBound
					||OtherDen   >=LargeIntUnsigned::SquareRootOfCarryOverBound)	)
      return false;
    register int N= this->NumShort*OtherDen+this->DenShort*OtherNum;
		register int D= this->DenShort*OtherDen;
    if (N==0)
    { this->NumShort=0;
      this->DenShort=1;
    }
    else
    {	register int tempGCD;
      if (N>0)
      { tempGCD= Rational::gcd(N,D);
      }
      else
      { tempGCD= Rational::gcd(-N,D);
      }
      this->NumShort= (N/tempGCD);
      this->DenShort= (D/tempGCD);
    }
		return true;
	};
	inline bool TryToMultiplyQuickly(	int OtherNum,
																		int OtherDen)
	{ register int OtherNumAbs, thisNumAbs;
		assert(this->DenShort>0 && OtherDen>0);
	  if (OtherNum<0)
	  { OtherNumAbs=-OtherNum;
	  } else
	  { OtherNumAbs=OtherNum;
    }
	  if (this->NumShort<0)
	  { thisNumAbs=-this->NumShort;
	  } else
	  { thisNumAbs=this->NumShort;
    }
		if (!this->flagMinorRoutinesOnDontUseFullPrecision &&
				(	this->Extended!=0
					||thisNumAbs>= LargeIntUnsigned::SquareRootOfCarryOverBound
					||this->DenShort>=LargeIntUnsigned::SquareRootOfCarryOverBound
					||OtherNumAbs>=LargeIntUnsigned::SquareRootOfCarryOverBound
					||OtherDen   >=LargeIntUnsigned::SquareRootOfCarryOverBound)	)
      return false;
    register int N = this->NumShort*OtherNum;
		register int D = this->DenShort*OtherDen;
		if (N==0)
		{ this->NumShort=0;
      this->DenShort=1;
    }
    else
    { register int tempGCD;
      if (N>0)
      { tempGCD= Rational::gcd(N,D);
      }
      else
      { tempGCD= Rational::gcd(-N,D);
      }
      this->NumShort= (N/((signed int)tempGCD));
      this->DenShort= (D/tempGCD);
    }
    return true;
	};
	bool InitExtendedFromShortIfNeeded()
	{ if (this->Extended!=0)
			return false;
		this->Extended= new LargeRationalExtended;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
		this->Extended->den.AssignShiftedUInt((unsigned int)this->DenShort,0);
		this->Extended->num.AssignInt(this->NumShort);
		return true;
	};
	inline void FreeExtended()
	{	if (this->Extended==0)
      return;
		delete this->Extended; this->Extended=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	}
	bool ShrinkExtendedPartIfPossible()
	{ if (this->Extended==0)
			return true;
		if (		this->Extended->num.value.size>1
				||	this->Extended->den.size>1
				||	this->Extended->num.value.TheObjects[0]>=
						(unsigned int)	LargeIntUnsigned::SquareRootOfCarryOverBound
				||	this->Extended->den.TheObjects[0]>=
						(unsigned int)	LargeIntUnsigned::SquareRootOfCarryOverBound)
			return false;
		this->NumShort= this->Extended->num.GetIntValueTruncated();
		this->DenShort= this->Extended->den.GetUnsignedIntValueTruncated();
		this->FreeExtended();
		return true;
	};
public:
	int NumShort;
	//the requirement that the below be unsigned caused a huge problem, so I
	//changed it back to int. Grrrrr.
	int DenShort;
	LargeRationalExtended *Extended;
	void GetDenExtended(LargeIntUnsigned & output);
	void GetNumExtendedUnsigned(LargeIntUnsigned & output);
//	inline int GetNumValueTruncated(){return this->NumShort;};
//	inline int GetDenValueTruncated(){return this->denShort;};
	static bool flagMinorRoutinesOnDontUseFullPrecision;
	static bool flagAnErrorHasOccurredTimeToPanic;
	void Subtract(const Rational& r);
	void Add(Rational& r);
	void AddInteger(int x);
	void AssignFracValue();
	void MultiplyBy(Rational& r);
	int HashFunction(){return this->NumShort*::SomeRandomPrimes[0]+
														this->DenShort*::SomeRandomPrimes[1];};
	//void MultiplyByLargeRational(int num, int den);
	void MultiplyByInt(int x);
	void MultiplyByLargeInt(LargeInt& x);
	void MultiplyByLargeIntUnsigned(LargeIntUnsigned& x);
	void Assign(const Rational& r);
	void AssignInteger(int x);
	bool IsGreaterThan(Rational& r);
	inline void AssignNumeratorAndDenominator( int n, int d){ if (d<0)
																														{ d=-d; n=-n;}
																														this->NumShort=n;
																														this->DenShort=d;
																														this->FreeExtended();
																														this->Simplify();
																													};
	void DivideBy(Rational& r);
	void DivideByInteger(int x)	{ int tempDen; signed char tempSign;
																if (x<0) {tempDen=(-x); tempSign=-1;} else {tempDen=x; tempSign=1;}
																if (this->TryToMultiplyQuickly(tempSign,tempDen))
																	return;
																this->InitExtendedFromShortIfNeeded();
																this->Extended->den.MultiplyByUInt(((unsigned int)tempDen));
																this->Extended->num.sign*=tempSign;
																this->Simplify();
															};
	void DivideByLargeInteger(LargeInt& x){ this->InitExtendedFromShortIfNeeded();
																					this->Extended->den.MultiplyBy(x.value);
																					this->Extended->num.sign*=x.sign;
																					this->Simplify();};
	void DivideByLargeIntegerUnsigned(LargeIntUnsigned& x){	this->InitExtendedFromShortIfNeeded();
																													this->Extended->den.MultiplyBy(x);
																													this->Simplify();};
	void ElementToString(std::string& output);
	bool IsEqualTo(const Rational& r);
	bool IsGreaterThanOrEqualTo(Rational& right);
	inline bool IsEqualToZero(){	if (this->Extended==0)
																	return this->NumShort==0;
                                else
																	return this->Extended->num.IsEqualToZero();
															};
	inline bool IsNonNegative(){	if (this->Extended==0)
																	return this->NumShort>=0;
                                else
																	return this->Extended->num.IsNonNegative();
															};
	bool IsNegative(){  if (this->Extended==0)
                        return this->NumShort<0;
                      else
                        return this->Extended->num.IsNegative();
                   };
	bool IsNonPositive(){ if (this->Extended==0)
                          return this->NumShort<=0;
                        else
                          return this->Extended->num.IsNonPositive();
                      };
	bool IsPositive(){ if (this->Extended==0)
                       return this->NumShort>0;
                     else
                       return this->Extended->num.IsPositive();
                   };
	void Simplify();
	void Invert();
	void Minus(){ if (this->Extended==0)
                  this->NumShort*=-1;
                else
                  this->Extended->num.sign*=-1;
              };
	double DoubleValue();
	int floor(){	if (NumShort<0)
								{	if (DenShort!=1)
										return (this->NumShort/ this->DenShort)-1;
									else
										return this->NumShort/ this->DenShort;
								}
								else
									return this->NumShort/this->DenShort;};
	void MakeZero()	{this->NumShort=0;	this->DenShort=1; this->FreeExtended(); };
	void MakeOne()	{this->NumShort=1;	this->DenShort=1; this->FreeExtended(); };
	void MakeMOne()	{this->NumShort=-1; this->DenShort=1; this->FreeExtended(); };
	void WriteToFile (std::fstream& output);
	void ReadFromFile(std::fstream&  input);
	inline void AssignAbsoluteValue(){if(this->IsNegative())this->Minus();};
	void RaiseToPower(int x);
	static Rational TheRingUnit;
	static Rational TheRingZero;
	static Rational TheRingMUnit;
	//don't ever call the below manually or you can get memory leak (extended must be nullified here by
	//default!
	Rational(int n, int d){this->Extended=0; this->AssignNumeratorAndDenominator(n,d);};
	Rational(){this->Extended=0;};
//	Rational(int x){this->Extended=0; this->AssignInteger(x);};
	~Rational(){this->FreeExtended();};
	//the below must be called only with positive arguments!
	static int gcd(int a, int b){	int temp;
																while(b>0)
																{ temp= a % b; a=b; b=temp; }
																return a;};
	static int gcdSigned(int a, int b){if (a<0) {a*=-1;} if (b<0){b*=-1;} return Rational::gcd(a,b);};
	inline bool CheckForElementSanity(){ if (this->Extended==0) return true;
                                       if (this->DenShort<0) return false;
                                       if (((unsigned)this->DenShort)>LargeIntUnsigned::CarryOverBound) return false;
                                       return true; };
	inline void operator =(const Rational& right){this->Assign(right);};
	inline bool operator ==(const Rational& right){return this->IsEqualTo(right);};
	inline bool operator ==(int right){Rational tempRat; tempRat.AssignInteger(right); return this->IsEqualTo(tempRat);};
	inline void operator = (int right){this->AssignInteger(right);};
};

class root :public ListBasicObjectsLight<Rational>
{
private:
	void ScaleForMinHeightHeavy();
	void ScaleToIntegralHeavy();
	void ScaleToIntegralMinHeightHeavy();
	void ScaleToFirstNonZeroCoordinatePositive();
	void ScaleToIntegralMinHeightFirstNonZeroCoordinatePositiveHeavy();
	void FindLCMDenominatorsHeavy(LargeIntUnsigned& output);

	void ScaleForMinHeightLight();
	void ScaleToIntegralLight();
	void ScaleToIntegralMinHeightLight();
	void ScaleToIntegralMinHeightFirstNonZeroCoordinatePositiveLight();
	void FindLCMDenominatorsLight(LargeIntUnsigned& output);
	bool HasSmallCoordinates();
public:
	std::string DebugString;
	void MultiplyByLargeRational(Rational& a);
	void ComputeDebugString();
	void MakeZero(int DesiredDimension);
	void Add(root& r);
	int getIndexFirstNonZeroCoordinate();
	void DivByInteger(int a);
	void DivByLargeInt(LargeInt& a);
	void DivByLargeIntUnsigned(LargeIntUnsigned& a);
	void GetCoordsInBasis(roots& inputBasis, root& outputCoords, GlobalVariables& theGlobalVariables);
	inline void MakeNormalInProjectivizationFromAffineHyperplane(affineHyperplane& input);
	void MakeNormalInProjectivizationFromPointAndNormal(root& point, root& normal);
	bool MakeAffineProjectionFromNormal(affineHyperplane& output);
	//the below returns false
	bool ProjectToAffineSpace(root& output);
	bool HasStronglyPerpendicularDecompositionWRT
	(	roots& theSet, WeylGroup& theWeylGroup, roots& output,
		ListBasicObjects<Rational>& outputCoeffs);
	void DivByLargeRational(Rational& a);
	void ElementToString(std::string& output);
	void ElementToStringEpsilonForm(std::string& output);
	void ElementToString(std::string& output, bool useLaTeX);
	bool CheckForElementSanity();
	//void RootToLinPolyToString(std::string& output,PolynomialOutputFormat& PolyOutput);
	void ScaleToIntegralMinHeight();
	void ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
	void FindLCMDenominators(LargeIntUnsigned& output);
	int FindLCMDenominatorsTruncateToInt();
	void InitFromIntegers(int Dimension, int x1,int x2, int x3,int x4, int x5);
	void InitFromIntegers
		(	int Dimension, int x1, int x2, int x3,
			int x4, int x5,int x6, int x7, int x8);
	void InitFromIntegers
		(	int Dimension, int x1, int x2, int x3, int x4,
			int x5,int x6, int x7, int x8, int x9, int x10, int x11, int x12);
	void MultiplyByInteger(int a);
	void MultiplyByLargeInt(LargeInt& right);
	void MultiplyByLargeIntUnsigned(LargeIntUnsigned& right);
	bool OurScalarProductIsPositive(root& right);
	bool OurScalarProductIsNegative(root& right);
	bool OurScalarProductIsZero(root& right);
	void MinusRoot();
	void Subtract(root& r);
	void AssignWithoutLastCoordinate(root& right);
	inline void Assign(const root& right)
	{ if (this->size!=right.size)
			this->SetSizeExpandOnTopLight(right.size);
		for (int i=0;i<this->size;i++)
			this->TheObjects[i].Assign(right.TheObjects[i]);
	};
	void AssignIntRoot(intRoot& right);
	bool IsProportianalTo(root& r);
	bool IsPositiveOrZero();
	bool IsNegativeZero();
	bool IsEqualToZero()
	{	for (int i=0;i<this->size;i++)
		{	if (!this->TheObjects[i].IsEqualToZero())
			return false;
		}
		return true;
	};
	bool IsGreaterThanOrEqualTo(root& r);
	bool IsEqualTo(const root& right);
	bool IsStronglyPerpendicularTo(root& right, WeylGroup& theWeyl);
	static void RootScalarEuclideanRoot(const root& r1, const root& r2, Rational& output);
	static void RootScalarRoot(root& r1, root& r2, MatrixLargeRational& KillingForm, Rational& output);
//	static void RootScalarRoot(root& r1, root& r2, MatrixIntTightMemoryFit& KillingForm, Rational& output);
	static void RootPlusRootTimesScalar(root& r1, root& r2, Rational& rat, root& output);
	int HashFunction();
	root(){};
	inline void operator=(const root& right){this->Assign(right);};
	inline bool operator==(const root& right){return IsEqualTo(right);};
};

class roots : public ListBasicObjects<root>
{
public:
	std::string DebugString;
	bool CheckForElementSanity();
	void AssignIntRoots(intRoots& r);
	void AssignHashedIntRoots(HashedListBasicObjects<intRoot>& r);
	void AssignMatrixRows(MatrixLargeRational& mat);
	void ComputeDebugString();
	int GetRankOfSpanOfElements(GlobalVariables& theGlobalVariables);
	void AddRoot(root& r);
	void AddIntRoot(intRoot& r);
	void AddRootS(roots& r);
	void AddRootSnoRepetition(roots& r);
	bool AddRootNoRepetition(root& r);
	void PerturbVectorToRegular
		(	root&output, GlobalVariables& theGlobalVariables,
			int theDimension,FeedDataToIndicatorWindow reportFunction);
	void Average(root& output, int theDimension);
	void Pop(int index);
	void intersectWith(roots& right, roots& output);
	bool ContainsARootConnectedTo(root& input, WeylGroup& theWeyl);
	bool IsRegular(root& r, GlobalVariables& theGlobalVariables, int theDimension);
	bool IsRegular(root& r, root& outputFailingNormal, GlobalVariables& theGlobalVariables, int theDimension);
	bool GetMinLinearDependenceWithNonZeroCoefficientForFixedIndex
		(MatrixLargeRational& outputTheLinearCombination, int theIndex);
	void GetLinearDependenceRunTheLinearAlgebra
		(MatrixLargeRational& outputTheLinearCombination, MatrixLargeRational& outputTheSystem,
		 Selection& outputNonPivotPoints);
	int GetDimensionOfElements();
	//the following function assumes the first dimension vectors are the images of the
	// vectors (1,0,...,0),..., (0,...,0,1)
	void MakeBasisChange(root& input, root& output);
	bool GetLinearDependence(MatrixLargeRational& outputTheLinearCombination);
	void GaussianEliminationForNormalComputation
		(MatrixLargeRational& inputMatrix, Selection& outputNonPivotPoints, int theDimension);
	// the below function is slow
	int ArrangeFirstVectorsBeOfMaxPossibleRank(GlobalVariables& theGlobalVariables);
	void rootsToMatrix(MatrixLargeRational& output);
	void rootsToMatrixRationalTruncate(MatrixLargeRational& output);
	void ElementToString(std::string& output);
	void ElementToString(std::string& output, bool useLaTeX);
	void SubSelection(Selection& theSelection, roots& output);
	void SelectionToMatrix(Selection& theSelection, int OutputDimension, MatrixLargeRational& output);
	void SelectionToMatrixAppend
				(Selection& theSelection, int OutputDimension, MatrixLargeRational& output, int StartRowIndex);
	void ComputeNormal(root& output);
	bool ComputeNormalExcludingIndex(root& output, int index, GlobalVariables& theGlobalVariables);
	bool ComputeNormalFromSelection
		(root& output, Selection& theSelection, GlobalVariables& theGlobalVariables, int theDimension);
	bool ComputeNormalFromSelectionAndExtraRoot
		(root& output,root& ExtraRoot, Selection& theSelection, GlobalVariables& theGlobalVariables);
	bool ComputeNormalFromSelectionAndTwoExtraRoots
		(root& output,root& ExtraRoot1, root& ExtraRoot2, Selection& theSelection, GlobalVariables& theGlobalVariables);
	bool operator==(const roots& right);
	void operator = (const roots& right){this->CopyFromBase(right);};
};

class WallData
{
public:
	std::string DebugString;
	int indexInOwnerChamber;
	root normal;
	ListBasicObjects<CombinatorialChamber*> NeighborsAlongWall;
	ListBasicObjects<WallData*> MirrorWall;
	static bool flagDisplayWallDetails;
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
	void ElementToString(std::string& output);
	void RemoveNeighborhoodBothSides(CombinatorialChamber* owner, CombinatorialChamber* NeighborPointer);
	void RemoveNeighborOneSide(CombinatorialChamber* NeighborPointer);
	void AddNeighbor(CombinatorialChamber* newNeighbor,WallData* newNeighborWall);
	void operator=(const WallData& right);
	bool IsInFacetNoBoundaries(root &point);
	bool FacetContainsChamberOnlyOnce(CombinatorialChamber* owner);
	void SubstituteNeighbor
		(CombinatorialChamber* oldNeighbor, CombinatorialChamber* newNeighbor,WallData* newNeighborWall);
	bool IsExternalWithRespectToDirection(root &direction);
	inline bool ContainsPoint(root& point){return this->normal.OurScalarProductIsZero(point);};
	bool ContainsNeighborAtMostOnce(CombinatorialChamber* neighbor);
	bool ContainsNeighborExactlyOnce(CombinatorialChamber *neighbor);
	bool ContainsMirrorWallExactlyOnce(WallData* theWall);
	bool SplitWall(CombinatorialChamber *BossChamber,
									CombinatorialChamber *NewPlusChamber,
									CombinatorialChamber *NewMinusChamber,
									CombinatorialChamberContainer* ownerComplex,
									roots& ThePlusVertices, roots& TheMinusVertices,
									root& TheKillerFacet, root& direction,
									ListBasicObjects<CombinatorialChamber*>& PossibleBogusNeighbors,
									ListBasicObjects<WallData*>* PossibleBogusWalls,
									GlobalVariables& theGlobalVariables);
	bool ConsistencyCheck(CombinatorialChamber* owner);
	bool EveryNeigborIsExplored(bool& aNeighborHasNonZeroPoly);
};

class affineHyperplane
{
public:
	std::string DebugString;
	root affinePoint;
	root normal;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
	//void InduceFromFacet(Facet& input);
	//the below returns false if the projection is not of full dimension
	int HashFunction();
//	bool ProjectFromFacet(Facet& input);
	bool ProjectFromFacetNormal(root& input);
	bool ContainsPoint(root& thePoint);
	void MakeFromNormalAndPoint(root& inputPoint, root&inputNormal);
	bool HasACommonPointWithPositiveTwoToTheNth_ant();
	void Assign(const affineHyperplane& right){ this->affinePoint.Assign(right.affinePoint); this->normal.Assign(right.normal);};
	inline void operator=(const affineHyperplane& right){this->Assign(right);};
	inline bool operator==(const affineHyperplane& right);
};

class affineHyperplanes: public ListBasicObjects<affineHyperplane>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
};

class affineCone
{
public:
	affineHyperplanes theWalls;
	int HashFunction();
	inline int GetDimension();
	void SuperimposeAffineCones(affineCones& theOtherComplex);
	void ProjectFromCombinatorialChamber(CombinatorialChamber& input);
	//void induceFromCombinatorialChamber(CombinatorialChamber& input);
	bool WallIsInternalInCone(affineHyperplane& theKillerCandidate);
	//The below function returns true if the system of homogeneous linear inequalities Ax<=b
	//has a solution, false otherwise, where A is a matrix and x and b are column vectors.
//	bool SystemLinearInequalitiesHasSolution
//		(MatrixLargeRational& matA, MatrixLargeRational& matb, MatrixLargeRational& outputPoint);
	bool SplitByAffineHyperplane(affineHyperplane& theKillerPlane, affineCones& output);
	void Assign(const affineCone& right){this->theWalls.CopyFromBase(right.theWalls);};
	inline void operator=(const affineCone& right){this->Assign(right);};
};

class affineCones: public HashedListBasicObjects<affineCone>
{
public:
	void SuperimposeAffineCones(affineCones& theOtherComplex);
	void ProjectFromCombinatorialChambers(CombinatorialChamberContainer& input);
};


class CombinatorialChamber
{
public:
	std::string DebugString;
	bool flagHasZeroPolynomial;
	bool flagExplored;
	bool flagPermanentlyZero;
	//QuasiPolynomial* ThePolynomial;
	int CreationNumber;
	int DisplayNumber;
	int IndexInOwnerComplex;
	ListBasicObjects<WallData> Externalwalls;
	roots InternalWalls;
	roots AllVertices;
	roots affineVertices;
	affineHyperplanes affineExternalWalls;
	root InternalPoint;
	int IndexStartingCrossSectionNormal;
	int NumTrueAffineVertices;
	static bool DisplayingGraphics;
	static bool flagIncludeVerticesInDebugString;
	static bool flagAnErrorHasOccurredTimeToPanic;
	static int MethodUsed;//1. normals vertices and boundaries
												//2. normals only
	static bool flagPrintWallDetails;
//	static bool flagMakingASingleHyperplaneSlice;
	bool PointLiesInMoreThanOneWall(root& point);
	//bool InduceFromAffineCone(affineCone& input);
	bool ComputeDebugString(CombinatorialChamberContainer* owner);
	bool ElementToString(std::string& output, CombinatorialChamberContainer* owner);
	bool ElementToString
		(std::string& output, CombinatorialChamberContainer* owner, bool useLatex, bool useHtml);
	void ElementToInequalitiesString
		(std::string& output,CombinatorialChamberContainer& owner, bool useLatex, bool useHtml);
	void ChamberNumberToString(std::string& out, CombinatorialChamberContainer& owner);
	bool ConsistencyCheck(int theDimension);
	//bool FacetIsInternal(Facet* f);
	void LabelWallIndicesProperly();
	int getIndexInfiniteHyperplane(CombinatorialChamberContainer* owner);
	int getIndexVertexIncidentWithSelection(Selection& theSel);
	bool VertexIsIncidentWithSelection(root& VertexCandidate,Selection& theSel);
	void FindAllNeighbors(ListObjectPointers<CombinatorialChamber>& TheNeighbors);
	bool SplitChamber
    ( root& theKillerPlaneNormal,CombinatorialChamberContainer& output,
      root& direction, GlobalVariables& theGlobalVariables);
	bool IsABogusNeighbor
		(	WallData& NeighborWall,CombinatorialChamber* Neighbor, CombinatorialChamberContainer& ownerComplex,
			GlobalVariables& theGlobalVariables);
	void ComputeVerticesFromNormals
		(	CombinatorialChamberContainer& owner,
			GlobalVariables& theGlobalVariables);
	bool ComputeVertexFromSelection
		(	GlobalVariables& theGlobalVariables, root& output, Selection& theSel, int theDimension);
	//the below function returns false if the cross-section affine walls have been modified
	//and aborts its execution
	bool ProjectToDefaultAffineSpace(CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables);
	bool PointIsInChamber(root&point);
	void findWallsIncidentWithVertexExcludeWallAtInfinity
		(root& theVertex, Selection& output, CombinatorialChamberContainer* owner);
//	bool ScaledVertexIsInWallSelection(root &point, Selection& theSelection);
	bool ScaleVertexToFitCrossSection(root&point, CombinatorialChamberContainer& owner);
	void ComputeAffineInfinityPointApproximation
		(Selection& selectedExternalWalls,CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables);
	bool PointIsInWallSelection(root &point, Selection& theSelection);
	bool PlusMinusPointIsInChamber(root&point);
	void PurgeInternalWalls();
	bool LinearAlgebraForVertexComputation
    (Selection& theSelection, root& output, GlobalVariables& theGlobalVariables, int theDimension);
	bool LinearAlgebraForVertexComputationOneAffinePlane
		(	Selection& theSelection, root& output,
			GlobalVariables& theGlobalVariables, CombinatorialChamberContainer* owner);
	//returns false if the vectors were linearly dependent
	bool SliceInDirection
    ( root& direction,roots& directions, int CurrentIndex, CombinatorialChamberContainer& output,
      hashedRoots& FacetOutput, GlobalVariables& theGlobalVariables);
	void PropagateSlicingWallThroughNonExploredNeighbors
		(	root& theKillerNormal,rootsCollection &CuttingPlaneVertices,
			::CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
	//the below function will automatically add the candidate to the
	//list of used hyperplanes if the candidate is an allowed one
	bool IsAValidCandidateForNormalOfAKillerFacet
		(	root& normalCandidate,roots &directions, int CurrentIndex, CombinatorialChamberContainer& owner,
			GlobalVariables& theGlobalVariables);
	bool HasHSignVertex(root& h,int sign);
	bool CheckSplittingPointCandidate
		(	Selection &SelectionTargetSimplex,Selection &SelectionStartSimplex,
			MatrixLargeRational& outputColumn, int Dimension);
	void AddInternalWall
		(	root& TheKillerFacetNormal, root& TheFacetBeingKilledNormal,
			root &direction, CombinatorialChamberContainer* owner,
			GlobalVariables& theGlobalVariables);
//	void InduceFromAffineConeAddExtraDimension(affineCone& input);
	void InduceFromCombinatorialChamberLowerDimensionNoAdjacencyInfo
		(CombinatorialChamber& input,CombinatorialChamberContainer& owner);
	void ComputeInternalPointMethod2(root& InternalPoint, int theDimension);
	void ComputeAffineInternalPoint(root& outputPoint, int theDimension);
	bool OwnsAWall(WallData* theWall);
	void MakeNewMutualNeighbors
		(CombinatorialChamber* NewPlusChamber, CombinatorialChamber* NewMinusChamber, root& normal);
	bool TestPossibilityToSlice(root& direction);
	bool MakeFacetFromEdgeAndDirection
    (	WallData& Wall1, WallData& Wall2,CombinatorialChamberContainer& owner,
			root& direction, roots & directions, int CurrentIndex,
			root& outputNormal,GlobalVariables& theGlobalVariables);
  void drawOutputAffine
		(	DrawingVariables& TDV, CombinatorialChamberContainer& owner,
			std::fstream* LaTeXoutput, drawLineFunction theDrawFunction,
			drawTextFunction drawTextIn);
	void WireChamberAndWallAdjacencyData
		(	CombinatorialChamberContainer &owner,
			CombinatorialChamber* input);
	void Assign(const  CombinatorialChamber& right);
	inline void operator=(const CombinatorialChamber& right){this->Assign(right);};
	CombinatorialChamber();
};

template <class Object>
void ListBasicObjects<Object>::AddListOnTop(ListBasicObjects<Object>& theList)
{ int oldsize= this->size;
	int otherSize=theList.size;
	this->SetSizeExpandOnTopNoObjectInit(oldsize+otherSize);
	for (int i=0;i<otherSize;i++)
		this->TheObjects[i+oldsize]= theList.TheObjects[i];
}

template<class Object>
void ListBasicObjects<Object>::SwapTwoIndices(int index1, int index2)
{ if (index1==index2)
		return;
	Object tempO;
	tempO= this->TheObjects[index1];
	this->TheObjects[index1]=this->TheObjects[index2];
	this->TheObjects[index2]=tempO;
}

template<class Object>
int ListBasicObjects<Object>::IndexOfObject(const Object &o)
{ for (int i=0;i<this->size;i++)
		if (this->TheObjects[i]==o)
			return i;
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
		this->TheObjects[i]= this->TheObjects[i-1];
}

template <class Object>
void ListBasicObjects<Object>::initFillInObject(int theSize, const Object& o)
{ this->SetSizeExpandOnTopNoObjectInit(theSize);
	for (int i=0; i<this->size;i++)
		this->TheObjects[i]=o;
}

template <class Object>
bool  ListBasicObjects<Object>::AddObjectOnTopNoRepetitionOfObject(const Object& o)
{ if (this->IndexOfObject(o)!=-1)
		return false;
	this->AddObjectOnTop(o);
	return true;
}

template <class Object>
inline Object* ListBasicObjects<Object>::LastObject()
{ return &this->TheObjects[this->size-1];
}

template <class Object>
void ListBasicObjects<Object>::AssignLight(const ListBasicObjectsLight<Object>& From)
{ this->SetSizeExpandOnTopNoObjectInit(From.size);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i]= From.TheObjects[i];
}

template <class Object>
void ListBasicObjects<Object>::CopyFromBase(const ListBasicObjects<Object>& From)
{ this->SetSizeExpandOnTopNoObjectInit(From.size);
	for (int i=0;i<this->size;i++)
		this->TheObjects[i]= From.TheObjects[i];
}

template <class Object>
void ListBasicObjects<Object>::MakeActualSizeAtLeastExpandOnTop(int theSize)
{	if (!(this->ActualSize>= this->IndexOfVirtualZero+theSize))
	{	ParallelComputing::SafePoint();
		this->ExpandArrayOnTop( this->IndexOfVirtualZero+theSize- this->ActualSize);
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
{	this->MakeActualSizeAtLeastExpandOnTop(theSize);
	size=theSize;
}

template <class Object>
inline void ListBasicObjects<Object>::ElementToStringGeneric(std::string& output)
{ this->ElementToStringGeneric(output,this->size);
}

template <class Object>
inline void ListBasicObjects<Object>::ElementToStringGeneric
	(std::string& output, int NumElementsToPrint)
{ std::stringstream out; std::string tempS;
	int Upper= ::MathRoutines::Minimum(NumElementsToPrint,this->size);
	for (int i=0;i<Upper;i++)
	{	this->TheObjects[i].ElementToString(tempS);
		out<< tempS<< "\n";
	}
	output= out.str();
}

template <class Object>
inline void ListBasicObjects<Object>::AddObjectOnTopCreateNew()
{	this->SetSizeExpandOnTopNoObjectInit(this->size+1);
}

template <class Object>
void ListBasicObjects<Object>::PopIndexShiftUp(int index)
{	if (size==0)
		return;
	this->size--;
	for (int i=index;i>=1;i--)
		this->TheObjects[i]=this->TheObjects[i-1];
	this->TheObjects++;
	IndexOfVirtualZero++;
}

template <class Object>
void ListBasicObjects<Object>::PopIndexSwapWithLast(int index)
{	if (this->size==0)
		return;
	this->size--;
	this->TheObjects[index]=this->TheObjects[this->size];
}

template <class Object>
void ListBasicObjects<Object>::PopLastObject()
{	if (this->size==0)
		return;
	this->size--;
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
{	delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->ActualSize=0;
	this->IndexOfVirtualZero=0;
	this->size=0;
	this->TheObjects=0;
	this->TheActualObjects=0;
}

template <class Object>
ListBasicObjects<Object>::~ListBasicObjects()
{	delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
	this->ActualSize=0;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->TheActualObjects=0;
	this->TheObjects=0;
}

template <class Object>
void ListBasicObjects<Object>::ExpandArrayOnBottom(int increase)
{	if (increase<=0) return;
	Object* newArray = new Object[this->ActualSize+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->ActualSize+increase;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	for (int i=0;i<this->size;i++)
		newArray[i+increase+this->IndexOfVirtualZero]=this->TheObjects[i];
	delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->TheActualObjects= newArray;
	this->ActualSize+=increase;
	this->IndexOfVirtualZero+=increase;
	this->TheObjects = this->TheActualObjects+this->IndexOfVirtualZero;
}

template <class Object>
void ListBasicObjects<Object>::ExpandArrayOnTop(int increase)
{	if (increase<=0)
		return;
	Object* newArray = new Object[this->ActualSize+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->ActualSize+increase;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	for (int i=0;i<this->size;i++)
		newArray[i+this->IndexOfVirtualZero]=this->TheObjects[i];
	delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->TheActualObjects= newArray;
	this->ActualSize+=increase;
	this->TheObjects = this->TheActualObjects+this->IndexOfVirtualZero;
}

template <class Object>
void ListBasicObjects<Object>::ReverseOrderElements()
{ int tempI= this->size/2;
  for (int i=0;i<tempI;i++)
    this->SwapTwoIndices(i,this->size-i-1);
}

template <class Object>
void ListBasicObjects<Object>::AddObjectOnBottom(const Object& o)
{	if (this->IndexOfVirtualZero==0)
		this->ExpandArrayOnBottom(ListBasicObjects<Object>::ListBasicObjectsActualSizeIncrement);
	this->IndexOfVirtualZero--;
	this->TheObjects--;
	this->TheObjects[0]=o;
	this->size++;
}

template <class Object>
void ListBasicObjects<Object>::AddObjectOnTop(const Object& o)
{	if (this->IndexOfVirtualZero+this->size>=this->ActualSize)
		this->ExpandArrayOnTop(ListBasicObjects<Object>::ListBasicObjectsActualSizeIncrement);
	this->TheObjects[size]=o;
	this->size++;
}

template <class Object>
void HashedListBasicObjects<Object>::SwapTwoIndicesHash(int i1, int i2)
{ Object tempO;
	int i1Hash = this->TheObjects[i1].HashFunction();
	int i2Hash = this->TheObjects[i2].HashFunction();
	this->FitHashSize(i1Hash); this->FitHashSize(i2Hash);
	this->TheHashedArrays[i1Hash].PopFirstOccurenceObjectSwapWithLast(i1);
	this->TheHashedArrays[i2Hash].PopFirstOccurenceObjectSwapWithLast(i2);
	tempO= this->TheObjects[i1];
	this->TheObjects[i1]=this->TheObjects[i2];
	this->TheObjects[i2]=tempO;
	this->TheHashedArrays[i1Hash].AddObjectOnTop(i2);
	this->TheHashedArrays[i2Hash].AddObjectOnTop(i1);
}

template <class Object>
int HashedListBasicObjects<Object>::SizeWithoutObjects()
{ int Accum=0;
	Accum+=this->ListBasicObjects<Object>::SizeWithoutObjects();
	Accum+=sizeof(this->TheHashedArrays)*this->HashSize;
	Accum+=sizeof(this->HashSize);
	for (int i=0;i<this->HashSize;i++)
		Accum+=this->TheHashedArrays[i].SizeWithoutObjects();
	return Accum;
}


template <class Object>
void HashedListBasicObjects<Object>::CopyFromHash (const HashedListBasicObjects<Object>& From)
{ if (&From==this){return;}
	this->ClearHashes();
	this->SetHashSize(From.HashSize);
  this->::ListBasicObjects<Object>::CopyFromBase(From);
  if (this->size<this->HashSize)
  	for (int i=0;i<this->size;i++)
		{	int hashIndex= this->TheObjects[i].HashFunction()% this->HashSize;
			if (hashIndex<0){hashIndex+=this->HashSize;}
			this->TheHashedArrays[hashIndex].
				CopyFromBase(From.TheHashedArrays[hashIndex]);
		}
	else
		for (int i=0;i<this->HashSize;i++)
			this->TheHashedArrays[i].CopyFromBase(From.TheHashedArrays[i]);
}
template <class Object>
void HashedListBasicObjects<Object>::ClearHashes()
{ if (this->size<this->HashSize)
		for (int i=0;i<this->size;i++)
		{ int hashIndex=this->TheObjects[i].HashFunction()%this->HashSize;
			if (hashIndex<0){hashIndex+=this->HashSize;}
			this->TheHashedArrays[hashIndex].size=0;
		}
	else
		for (int i=0;i<this->HashSize;i++)
			this->TheHashedArrays[i].size=0;
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
		this->TheHashedArrays[i].size=0;
}

template <class Object>
int HashedListBasicObjects<Object>::IndexOfObjectHash(Object &o)
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
	this->::ListBasicObjects<Object>::AddObjectOnTop(o);
}

template <class Object>
void HashedListBasicObjects<Object>::AddObjectOnTopNoRepetitionOfObjectHash(Object &o)
{ if (this->IndexOfObjectHash(o)!=-1)
		return;
	this->AddObjectOnTopHash(o);
}

template <class Object>
void HashedListBasicObjects<Object>::PopIndexSwapWithLastHash(int index)
{	Object* oPop= &this->TheObjects[index];
	int hashIndexPop = oPop->HashFunction()% this->HashSize;
	if (hashIndexPop<0) {hashIndexPop+=this->HashSize;}
	this->TheHashedArrays[hashIndexPop].PopFirstOccurenceObjectSwapWithLast(index);
	if (index==this->size-1){this->size--; return;}
	int tempI=this->size-1;
	Object* oTop= &this->TheObjects[tempI];
	int hashIndexTop= oTop->HashFunction()% this->HashSize;
	if (hashIndexTop<0){hashIndexTop+=this->HashSize;}
	this->TheHashedArrays[hashIndexTop].PopFirstOccurenceObjectSwapWithLast(tempI);
	this->TheHashedArrays[hashIndexTop].AddObjectOnTop(index);
	this->::ListBasicObjects<Object>::PopIndexSwapWithLast(index);
}

template <class Object>
void HashedListBasicObjects<Object>::SetHashSize(int HS)
{ if (HS!=this->HashSize)
	{	delete [] this->TheHashedArrays;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=HS-this->HashSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
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
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->HashSize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->HashSize=0;
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
	if (this->ActualSize<this->size+increase)
	{ Object** newArray= new Object*[this->size+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=increase;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
		for (int i=0;i<this->size;i++)
			newArray[i]=this->TheObjects[i];
		delete [] this->TheObjects;
		this->TheObjects= newArray;
		this->ActualSize+=increase;
	}
	for(int i=this->size;i<this->ActualSize;i++)
		this->TheObjects[i]=0;
	this->size+=increase;
}

template<class Object>
void ListObjectPointers<Object>::initAndCreateNewObjects(int d)
{ this->KillAllElements();
	this->SetSizeExpandOnTopNoObjectInit(d);
	for (int i=0;i<d;i++)
		this->TheObjects[i]=new Object;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=d;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
}

template<class Object>
void ListObjectPointers<Object>::KillElementIndex(int i)
{	delete this->TheObjects[i];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	this->size--;
	this->TheObjects[i]=this->TheObjects[this->size];
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
	int oldsize= this->size;
	this->SetSizeExpandOnTopNoObjectInit(this->size+increase);
	for (int i=oldsize;i<this->size;i++)
		this->TheObjects[i]=new Object;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->size-oldsize;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
}

template<class Object>
void ListObjectPointers<Object>::KillAllElements()
{	for (int i =0;i<this->size;i++)
	{	delete this->TheObjects[i];
#ifdef CGIversionLimitRAMuse
	if (this->TheObjects[i]!=0)ParallelComputing::GlobalPointerCounter--;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
		this->TheObjects[i]=0;
	}
	this->size=0;
}

template<class Object>
bool ListObjectPointers<Object>::AddObjectNoRepetitionOfPointer(Object* o)
{ if (this->ContainsObject(o)==-1)
	{	this->AddObjectOnTop(o);
		return true;
	}
	return false;
}

template<class Object>
bool ListObjectPointers<Object>::IsAnElementOf(Object* o)
{	for(int i=0;i<this->size;i++)
		if (this->TheObjects[i]==o)
			return true;
	return false;
}
template<class Object>
void CopyOntoObject(ListObjectPointers<Object>* FromList)
{	init(FromList->size);
	for(int i=0; i<FromList->size;i++)
		AddObject(FromList->TheObjects[i]);
}

template <class Object>
void ListObjectPointers<Object>::PopAllOccurrencesSwapWithLast(Object*o)
{	for (int i =0; i<this->size;i++)
	{	if (o==this->TheObjects[i])
		{	this->TheObjects[i]=this->TheObjects[this->size-1];
			this->size--;
			i--;
		}
	}
};

class rootsCollection: public ListBasicObjects<roots>
{
public:
	std::string DebugString;
	void ComputeDebugString();
	void Average(root& output, int Number, int theDimension);
	void ResetCounters();
	void CollectionToRoots(roots& output);
	~rootsCollection(){};
};

class hashedRoots: public HashedListBasicObjects<root>
{
public:
	std::string DebugString;
	void AddRootsOnTopHash(roots& input)
	{ for (int i=0;i<input.size;i++)
			this->AddObjectOnTopHash(input.TheObjects[i]);
	};
	void ComputeDebugString();
	void ElementToString(std::string& output);
};


//class pertains to the Q^+span of a set of roots.
class Cone : public roots
{ //The roots are the normals to the walls of the cone
public:
	void ComputeFromDirections(roots& directions, GlobalVariables& theGlobalVariables, int theDimension);
	bool IsSurelyOutsideCone(rootsCollection& TheVertices);
	// the below returns false is we have a point strictly inside the cone
	// else it fills in the Chamber test array
	bool FillInChamberTestArray(roots& TheVertices, bool initChamberTestArray);
	bool IsSurelyOutsideCone(roots& TheVertices);
	bool IsSurelyOutsideConeAccordingToChamberTestArray();
	bool IsInCone(root& r);
	bool SeparatesPoints(root& point1, root& point2);
//	int HashFunction();
	ListBasicObjects<int> ChamberTestArray;
	void operator =(Cone& right);
};


class simplicialCones : public ListBasicObjects<Cone>
{
public:
	hashedRoots theFacets;
	ListBasicObjects<ListBasicObjects<int> > ConesHavingFixedNormal;
	std::string DebugString;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void initFromDirections(roots& directions,GlobalVariables& theGlobalVariables);
	bool SeparatePoints(root& point1, root& point2, root* PreferredNormal);
};

class CombinatorialChamberContainer: public ListObjectPointers<CombinatorialChamber>
{
public:
	int FirstNonExploredIndex;
	int AmbientDimension;
	std::string DebugString;
	hashedRoots theHyperplanes;
	Cone TheGlobalConeNormals;
	Cone WeylChamber;
	roots NewHyperplanesToSliceWith;
	HashedListBasicObjects<affineHyperplane> AffineWallsOfWeylChambers;
	affineHyperplanes theWeylGroupAffineHyperplaneImages;
	root IndicatorRoot;
	ListBasicObjects<int> PreferredNextChambers;
	std::fstream FileOutput;
	int indexNextChamberToSlice;
	int NumAffineHyperplanesProcessed;
	int NumAffineHyperplanesBeforeWeylChamberWalls;
	int NumProjectiveHyperplanesBeforeWeylChamberWalls;
	affineHyperplanes StartingCrossSections;
	bool flagMakingASingleHyperplaneSlice;
	bool flagSliceWithAWallInitDone;
	bool flagSliceWithAWallIgnorePermanentlyZero;
	bool flagDrawingProjective;
	static const int MaxNumHeaps=5000;
	static const int GraphicsMaxNumChambers = 1000;
	static int NumTotalCreatedCombinatorialChambersAtLastDefrag;
	static int DefragSpacing;
	static int LastReportedMemoryUse;
	static simplicialCones startingCones;
	static std::fstream TheBigDump;
	static root PositiveLinearFunctional;
	static bool PrintLastChamberOnly;
	static bool flagAnErrorHasOcurredTimeToPanic;
	static bool flagMakingConsistencyCheck;
	static int flagMaxNumCharsAllowedInStringOutput;
	void ConvertHasZeroPolyToPermanentlyZero();
	void SortIndicesByDisplayNumber(ListBasicObjects<int>& outputSortedIndices);
	void QuickSortIndicesByDisplayNumber
		(ListBasicObjects<int>& outputSortedIndices, int BottomIndex, int TopIndex);
	void AddWeylChamberWallsToHyperplanes
		(GlobalVariables& theGlobalVariables, WeylGroup& theWeylGroup);
	void SliceTheEuclideanSpace
		(	roots& directions,int& index,
			int rank,root& IndicatorRoot,
			GlobalVariables& theGlobalVariables);
	bool IsSurelyOutsideGlobalCone(rootsCollection& TheVertices);
	int FindVisibleChamberWithDisplayNumber(int inputDisplayNumber);
	void SliceOneDirection
			(	roots& directions, int& index, int rank,
				root& IndicatorRoot, GlobalVariables& theGlobalVariables);
	void OneSlice(roots& directions, int& index,
								int rank, root& IndicatorRoot, GlobalVariables& theGlobalVariables);
  void InduceFromLowerDimensionalAndProjectivize
		(CombinatorialChamberContainer& input, GlobalVariables& theGlobalVariables);
  void MakeExtraProjectivePlane();
	int GetNumChambersInWeylChamberAndLabelChambers(Cone& theWeylChamber);
	int GetNumVisibleChambersAndLabelChambersForDisplay();
	int GetNumVisibleChambersNoLabeling();
  void WireChamberAdjacencyInfoAsIn(CombinatorialChamberContainer& input);
  void LabelChamberIndicesProperly();
	void ElementToString(std::string& output, bool useLatex, bool useHtml);
	void ElementToString(std::string& output);
	void WriteToFile(DrawingVariables& TDV, roots& directions, std::fstream& output);
	void ComputeDebugString()
		{this->ElementToString(this->DebugString);};
	void ComputeDebugString(bool LatexFormat)
		{this->ElementToString(this->DebugString,LatexFormat,false);};
	void ComputeDebugString(bool useLatex, bool useHtml)
		{this->ElementToString(this->DebugString,useLatex,useHtml);};
	void init();
	void Free();
	int RootBelongsToChamberIndex(root& input, std::string* outputString);
	void MakeStartingChambers
		(roots& directions, root& IndicatorRoot, GlobalVariables& theGlobalVariables);
	void ComputeNextIndexToSlice(root& direction);
	void ComputeVerticesFromNormals(GlobalVariables& theGlobalVariables);
	void SliceWithAWall(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
	void SliceWithAWallInit(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
	void SliceWithAWallOneIncrement(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
	void AddChamberPointerSetUpPreferredIndices
		(CombinatorialChamber* theChamber, GlobalVariables& theGlobalVariables);
	void LabelAllUnexplored();
	void DumpAll();
	bool ConsistencyCheck();
	void PurgeZeroPointers();
	void PurgeInternalWalls();
	void ProjectToDefaultAffineSpace(GlobalVariables& theGlobalVariables);
	bool ProjectToDefaultAffineSpaceModifyCrossSections(GlobalVariables& theGlobalVariables);
	void PrintThePolys(std::string& output);
	void ComputeGlobalCone(roots& directions, GlobalVariables& theGlobalVariables);
	static void drawOutput
		(	DrawingVariables& TDV, CombinatorialChamberContainer& output,
			roots& directions, int directionIndex,root& ChamberIndicator,
			std::fstream* LaTeXOutput, drawLineFunction theDrawFunction,
			drawTextFunction drawTextIn);
	static void drawOutputProjective
		(	DrawingVariables& TDV, CombinatorialChamberContainer& output,
			roots& directions, int directionIndex,root& ChamberIndicator,
			drawLineFunction theDrawFunction, drawTextFunction drawTextIn);
	static void drawOutputAffine
		(	DrawingVariables& TDV, CombinatorialChamberContainer& output,
			std::fstream* LaTeXoutput, drawLineFunction theDrawFunction,
			drawTextFunction drawTextIn);
	static void drawFacetVerticesMethod2
		(	DrawingVariables& TDV,roots& r, roots& directions, int ChamberIndex,
			WallData& TheFacet, int DrawingStyle, int DrawingStyleDashes,
			drawLineFunction theDrawFunction);
	bool TestPossibleIndexToSlice(root&direction, int index);
	CombinatorialChamberContainer();
	~CombinatorialChamberContainer();
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
	bool ComputeDebugString();
	ElementOfCommutativeRingWithIdentity Coefficient;
	int HashFunction();
//	int DegreesToIndex(int MaxDeg);
	void MakeConstantMonomial(short Nvar,ElementOfCommutativeRingWithIdentity&coeff);
	void MakeNVarFirstDegree(int LetterIndex,short NumVars,ElementOfCommutativeRingWithIdentity& coeff);
	void init(short nv);
	void initNoDegreesInit(short nv);
	void NullifyDegrees();
	void DivideBy
		(	Monomial<ElementOfCommutativeRingWithIdentity>& input,
			Monomial<ElementOfCommutativeRingWithIdentity>& output);
	void DivideByExponentOnly(	intRoot& input);
	void MonomialExponentToRoot(root& output);
	void MonomialExponentToRoot(intRoot& output);
	void MakeFromRoot(ElementOfCommutativeRingWithIdentity& coeff, intRoot& input);
	void MonomialExponentToColumnMatrix(MatrixLargeRational& output);
	void GetMonomialWithCoeffOne(Monomial<ElementOfCommutativeRingWithIdentity>& output);
	void MultiplyBy(Monomial<ElementOfCommutativeRingWithIdentity>& m,
									Monomial<ElementOfCommutativeRingWithIdentity>& output);
	bool HasSameExponent(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	void Assign(const Monomial<ElementOfCommutativeRingWithIdentity>& m);
	void Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution,
		                Polynomial<ElementOfCommutativeRingWithIdentity>& output,
										short NumVarTarget);
	void MakeMonomialOneLetter(short NumVars,int LetterIndex,
	                           short Power, ElementOfCommutativeRingWithIdentity& Coeff);
	void IncreaseNumVariables(short increase);
	bool IsGEQpartialOrder(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	bool IsGEQ(Monomial<ElementOfCommutativeRingWithIdentity>& m);
	bool IsEqualToZero();
	bool IsPositive();
	void DecreaseNumVariables(short increment);
	void StringStreamPrintOutAppend(std::stringstream& out,PolynomialOutputFormat& PolyFormat);
	void ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat);
	bool IsAConstant();
	void InvertDegrees();
	bool operator==(Monomial<ElementOfCommutativeRingWithIdentity>& m);
  void operator=(const Monomial<ElementOfCommutativeRingWithIdentity>& m);
  int SizeWithoutCoefficient();
	Monomial();
	~Monomial();
};

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::InitWithZero=true;

//The class below could as well be called "Associative commutative algebra"
//The ring over which the algebra is defined is called by using TemplateMonomial.Coefficient
//The Template polynomial is assumed to be having coefficients in the ring given by ElementOfCommutativeRingWithIdentity

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
class TemplatePolynomial: public HashedListBasicObjects<TemplateMonomial>
{
private:
	TemplatePolynomial(TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>&){assert(false);};
public:
		short NumVars;
	void MultiplyByMonomial(TemplateMonomial& m);
	void MultiplyByMonomial
         (TemplateMonomial& m, TemplatePolynomial<	TemplateMonomial,ElementOfCommutativeRingWithIdentity >& output);
	void AddMonomial(TemplateMonomial& m);
	void CopyFromPoly(const TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& p);
	void Assign(const TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& p);
	std::string DebugString;
	// returns the number of lines used
	int StringPrintOutAppend(std::string& output, PolynomialOutputFormat& PolyFormat);
	bool ComputeDebugString();
	int TotalDegree();
	bool IsEqualTo(TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& p);
	bool IsEqualToZero();
	TemplatePolynomial();
	int FindMaxPowerOfVariableIndex(int VariableIndex);
	void Nullify(short NVar);
	void MakeNVarConst(short nVar, ElementOfCommutativeRingWithIdentity& coeff);
	bool HasGEQMonomial(TemplateMonomial& m, int& WhichIndex);
	void MultiplyBy(TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& p);
	void MultiplyBy(TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& p,
									TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& output);
	void RaiseToPower(int d,TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& output);
	void RaiseToPower(int d);
	void AddPolynomial(TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& p);
	void WriteToFile(std::fstream& output);
	void ReadFromFile(std::fstream& input, short NumV);
	int HasSameExponentMonomial(TemplateMonomial& m);
	void operator= (const TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& right);
	bool operator== (const TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& right);
};


//**********************************************************
/*To use Polynomial<ElementOfCommutativeRingWithIdentity> you MUST define:
//Absolutely necessary:
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
      //if you got no good idea what this means.
int ListBasicObjects<Monomial<ElementOfCommutativeRingWithIdentity>>::ListBasicObjectsActualSizeIncrement;
      //the above is the size of the chunk of memory the polynomial class will allocate
      //whenever it needs more memory. For example, if you believe your polynomials
      //will have 1000+ monomials average, then set the above to 1000. Note: abuse of the above
      //might raise your memory use unexpectedly high!
//Optional (required for some methods):
//void ElementOfCommutativeRingWithIdentity::DivideBy(&ElementOfCommutativeRingWithIdentity);
//void ElementOfCommutativeRingWithIdentity::WriteToFile(std::fstream& output);
//void ElementOfCommutativeRingWithIdentity::ReadFromFile(std::fstream& input);
//void ElementOfCommutativeRingWithIdentity::AssignRational(Rational& r);
*/


template <class ElementOfCommutativeRingWithIdentity>
class Polynomial: public TemplatePolynomial<Monomial<ElementOfCommutativeRingWithIdentity>, ElementOfCommutativeRingWithIdentity >
{
public:
	static bool flagAnErrorHasOccuredTimeToPanic;
	void FindCoeffInFrontOfLinearTermVariableIndex(int index,
							ElementOfCommutativeRingWithIdentity& output );
	void AssignPolynomialLight(const PolynomialLight<ElementOfCommutativeRingWithIdentity>& from);
	void MakeMonomialOneLetter(short NumVars,int LetterIndex,
	                           short Power, ElementOfCommutativeRingWithIdentity& Coeff);
	Polynomial(){};
	Polynomial(int degree, ElementOfCommutativeRingWithIdentity& coeff);
	void MakeNVarDegOnePoly
					(short NVar, int NonZeroIndex,ElementOfCommutativeRingWithIdentity& coeff);
	void MakeNVarDegOnePoly
					(short NVar, int NonZeroIndex1, int NonZeroIndex2,
					 ElementOfCommutativeRingWithIdentity& coeff1,
					 ElementOfCommutativeRingWithIdentity& coeff2);
	void MakeNVarDegOnePoly
					(short NVar, int NonZeroIndex,ElementOfCommutativeRingWithIdentity& coeff1,
					 ElementOfCommutativeRingWithIdentity& ConstantTerm);
	void MakeLinPolyFromRoot(root& r);
	void TimesInteger(int a);
	void DivideBy
		(	Polynomial<ElementOfCommutativeRingWithIdentity>& inputDivisor,
			Polynomial<ElementOfCommutativeRingWithIdentity>& outputQuotient,
			Polynomial<ElementOfCommutativeRingWithIdentity>& outputRemainder);
	void TimesConstant(ElementOfCommutativeRingWithIdentity& r);
	void DivideByConstant(ElementOfCommutativeRingWithIdentity& r);
	void AddConstant(ElementOfCommutativeRingWithIdentity& theConst);
	void IncreaseNumVariables(short increase);
	void ScaleToPositiveMonomials(Monomial<ElementOfCommutativeRingWithIdentity>& outputScale);
	void DecreaseNumVariables(short increment,Polynomial<ElementOfCommutativeRingWithIdentity>& output);
	void Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution,
		                Polynomial<ElementOfCommutativeRingWithIdentity>& output,
										short NumVarTarget);
	void Substitution(ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >& TheSubstitution,
		                short NumVarTarget);
  int TotalDegree();
	int GetIndexMaxMonomial();
	void ComponentInFrontOfVariableToPower
							(int VariableIndex,
							 ListObjectPointers<Polynomial<ElementOfCommutativeRingWithIdentity> >& output,
							 int UpToPower);
  int FindMaxPowerOfVariableIndex(int VariableIndex);
	//has to be rewritten please don't use!
	bool IsGreaterThanZeroLexicographicOrder();
  bool IsEqualTo(Polynomial<ElementOfCommutativeRingWithIdentity>& p);
};


template <class ElementOfCommutativeRingWithIdentity>
class Polynomials: public ListBasicObjects<Polynomial<ElementOfCommutativeRingWithIdentity> >
{	public:
	//the format of the linear substitution is:
	//coeff is a MatrixLargeRational whose number of rows minus 1 must equal the # number of
	//target variables and whose number of columns must equal the number of variables in
	//the current polynomial (this->NumVariables).
	//The first row denotes the constant term in the substitution of the respective variable!
	//An element in the x-th row and y-th column
	//is defined as ElementOfCommutativeRingWithIdentity[x][y] !
	void MakeLinearSubstitution(ElementOfCommutativeRingWithIdentity** coeffs,
		                          short NumStartVar, short NumTargetVar);
	void MakeExponentSubstitution(MatrixIntTightMemoryFit& theSub);
	void PrintPolys(std::string& output,ElementOfCommutativeRingWithIdentity& TheRingUnit,
									ElementOfCommutativeRingWithIdentity& TheRingZero);
	void MakeSubstitutionLastVariableToEndPoint(short numVars, Polynomial<ElementOfCommutativeRingWithIdentity>& EndPoint);
	void AddConstant(ElementOfCommutativeRingWithIdentity& theConst);
	void TimesConstant(ElementOfCommutativeRingWithIdentity& r);
	void DivideByConstant(ElementOfCommutativeRingWithIdentity& r);
};

template <class ElementOfCommutativeRingWithIdentity>
class PolynomialLight: public ListBasicObjectsLight<Monomial<ElementOfCommutativeRingWithIdentity> >
{
public:
	short NumVars;
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString();
	void AssignPolynomial(Polynomial<ElementOfCommutativeRingWithIdentity>& from);
	void AssignPolynomialLight(const PolynomialLight<ElementOfCommutativeRingWithIdentity>& from);
	void Nullify(short numberVariables);
	void MakeConst(ElementOfCommutativeRingWithIdentity& theConstant, short theNumVars);
	bool IsEqualToZero();
	void WriteToFile(std::fstream& output);
	void ReadFromFile(std::fstream& input, short numV);
};

template <class ElementOfCommutativeRingWithIdentity>
inline void PolynomialLight<ElementOfCommutativeRingWithIdentity>::ElementToString(std::string& output)
{ static Polynomial<ElementOfCommutativeRingWithIdentity> tempP;
	tempP.AssignPolynomialLight(*this);
	tempP.ComputeDebugString();
	output= tempP.DebugString;
}

template <class ElementOfCommutativeRingWithIdentity>
inline void PolynomialLight<ElementOfCommutativeRingWithIdentity>::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

template <class ElementOfCommutativeRingWithIdentity>
inline void PolynomialLight<ElementOfCommutativeRingWithIdentity>::AssignPolynomial
	(Polynomial<ElementOfCommutativeRingWithIdentity>& from)
{ this->NumVars=from.NumVars;
	this->CopyFromHeavy(from);
}

template <class ElementOfCommutativeRingWithIdentity>
inline void PolynomialLight<ElementOfCommutativeRingWithIdentity>::AssignPolynomialLight
	(const PolynomialLight<ElementOfCommutativeRingWithIdentity>& from)
{ this->NumVars= from.NumVars;
	this->CopyFromLight(from);
}

template <class ElementOfCommutativeRingWithIdentity>
inline void PolynomialLight<ElementOfCommutativeRingWithIdentity>::Nullify(short numberVariables)
{ this->NumVars= numberVariables;
	this->SetSizeExpandOnTopLight(0);
}

template <class ElementOfCommutativeRingWithIdentity>
inline void PolynomialLight<ElementOfCommutativeRingWithIdentity>::
	MakeConst(ElementOfCommutativeRingWithIdentity& theConstant, short theNumVars)
{ this->NumVars= theNumVars;
	this->SetSizeExpandOnTopLight(1);
	this->TheObjects[0].MakeConstantMonomial(theNumVars,theConstant);
}

template <class ElementOfCommutativeRingWithIdentity>
void PolynomialLight<ElementOfCommutativeRingWithIdentity>::WriteToFile(std::fstream& output)
{ static Polynomial<ElementOfCommutativeRingWithIdentity> ComputationalBuffer;
  ComputationalBuffer.AssignPolynomialLight(*this);
  ComputationalBuffer.WriteToFile(output);
}

template <class ElementOfCommutativeRingWithIdentity>
void PolynomialLight<ElementOfCommutativeRingWithIdentity>::ReadFromFile(std::fstream& input, short numV)
{ static Polynomial<ElementOfCommutativeRingWithIdentity> ComputationalBuffer;
  ComputationalBuffer.ReadFromFile(input, numV);
  this->AssignPolynomial(ComputationalBuffer);
}

template <class ElementOfCommutativeRingWithIdentity>
inline bool PolynomialLight<ElementOfCommutativeRingWithIdentity>::IsEqualToZero()
{ return this->size==0;
}

class intRoot
{
private:
public:
	int dimension;
	int elements[MaxRank];
	void AssignRoot(root&r);
	int HashFunction();
	void ElementToString(std::string& output);
	bool IsHigherThanWRTWeight(intRoot& r, intRoot& theWeights);
	bool IsGEQNoWeight(intRoot& r);
	void MakeZero(int  theDimension);
	void MultiplyByInteger(int x)
	{	for (int i=0;i<this->dimension;i++)
		{ this->elements[i]*=x;
		}
	};
	void initFromInt(int theDimension, int x1,int x2,int x3, int x4, int x5);
	void initFromInt(int theDimension, int x1,int x2,int x3, int x4, int x5, int x6,
										int x7,int x8,int x9, int x10, int x11, int x12);
	//remark: zero is considered to be a positive vector!
	bool IsPositive();
	void AddRoot(intRoot& theRoot);
	void operator=(const intRoot& right);
	bool operator==(intRoot& right);
	intRoot(){};
};

class GeneratorPFAlgebraRecord
{
private:
  GeneratorPFAlgebraRecord(const GeneratorPFAlgebraRecord& right);
public:
	//the represented element is considered to be p_elongation(x^GeneratorRoot)
	//where p is the numerator in the geometric series with length elongation.
	//This is well defined only for elongation not equal to zero.
	//For elongation=0 we define the represented element to be x^GeneratorRoot.
	//This is a convenient programming convention for the time being, no mathematical
	//reasons behind it.
  intRoot GeneratorRoot;
  int Elongation;
	std::string DebugString;
	IntegerPoly* Value;
	bool ValueIsComputed;
	GeneratorPFAlgebraRecord();
	~GeneratorPFAlgebraRecord();
	void ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat);
	void GetValue(IntegerPoly &output, int theDimension);
  void operator = (const GeneratorPFAlgebraRecord& right);
  int HashFunction()
  { return this->Elongation+ this->GeneratorRoot.HashFunction();
  };
  bool operator == (GeneratorPFAlgebraRecord& right)
  { return (this->GeneratorRoot == right.GeneratorRoot)
				&& (this->Elongation == right.Elongation);
  }
};

class GeneratorsPartialFractionAlgebra
{
public:
  static HashedListBasicObjects<GeneratorPFAlgebraRecord> theGenerators;
  //elongation zero means we are looking for x^exponent
  //other elongations means we are looking for p_elongation(x^exponent), where p is
  // the numerator in the geometric series sum corresponding to elongation
  static void GetMonomialFromExponentAndElongation(intRoot& exponent, int elongation,
								::MonomialInCommutativeAlgebra
									<	Integer,
										::GeneratorsPartialFractionAlgebra,
										GeneratorPFAlgebraRecord>& output);
	std::string DebugString;
	void ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat, int theDimension);
	void ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat);
  int HashFunction();
  void operator = (const GeneratorsPartialFractionAlgebra& right);
 	void ConvertToIntegerPoly(IntegerPoly& output, int theDimension);
	//IMPORTANT two generators are declared to be equal if the generator indices coincide. The power doesn't count.
	//This might have to be rewritten.
  bool operator == (GeneratorsPartialFractionAlgebra& right);
  int GeneratorIndex;
  int GeneratorPower;
};

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
class MonomialInCommutativeAlgebra: public HashedListBasicObjects<GeneratorsOfAlgebra>
{
private:
	MonomialInCommutativeAlgebra
    (MonomialInCommutativeAlgebra<ElementOfCommutativeRingWithIdentity,
                                  GeneratorsOfAlgebra,GeneratorsOfAlgebraRecord>&)
  {assert(false);};
//	int DegreesToIndexRecursive(int MaxDeg, int index);
public:
	ElementOfCommutativeRingWithIdentity Coefficient;
	std::string DebugString;
	void ComputeDebugString(PolynomialOutputFormat &PolyFormat);
	void ElementToString(std::string& output,PolynomialOutputFormat& PolyFormat);
	void StringStreamPrintOutAppend(std::stringstream& out,PolynomialOutputFormat& PolyFormat);
	int HashFunction();
	void MakeConstantMonomial(short Nvar,ElementOfCommutativeRingWithIdentity&coeff);
	void MultiplyBy(MonomialInCommutativeAlgebra
										<	ElementOfCommutativeRingWithIdentity,
											GeneratorsOfAlgebra,
											GeneratorsOfAlgebraRecord>& m,
									MonomialInCommutativeAlgebra<	ElementOfCommutativeRingWithIdentity,
																								GeneratorsOfAlgebra,
																								GeneratorsOfAlgebraRecord>& output);
	void MultiplyBy(MonomialInCommutativeAlgebra<	ElementOfCommutativeRingWithIdentity,
																								GeneratorsOfAlgebra,
																								GeneratorsOfAlgebraRecord>& m);
	//the below functions return 1 if no reduction has occured
	//i.e. the generator is not in zeroth power after the multiplication
	//If the generator is in zero-th power after the multiplication and reduction has occured,
	//the below return 0
	int MultiplyByGenerator(GeneratorsOfAlgebra& g);
	int MultiplyByGenerator(GeneratorsOfAlgebraRecord& g, int Power);
	int MultiplyByGenerator(int GeneratorIndex, int GeneratorPower);
	void Assign(const MonomialInCommutativeAlgebra
									<	ElementOfCommutativeRingWithIdentity,
										GeneratorsOfAlgebra,
										GeneratorsOfAlgebraRecord>& m);
	bool IsEqualToZero();
	MonomialInCommutativeAlgebra(){};
	//IMPORTANT: the coefficients of two monomials are not compared, that is,
	//two monomials are equal if they have the same
	//generators at same powers
	bool operator==(MonomialInCommutativeAlgebra
										<	ElementOfCommutativeRingWithIdentity,
											GeneratorsOfAlgebra,
											GeneratorsOfAlgebraRecord>& m);
  void operator=(const MonomialInCommutativeAlgebra
									<	ElementOfCommutativeRingWithIdentity,
										GeneratorsOfAlgebra,
										GeneratorsOfAlgebraRecord>& m);
};

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
bool MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
	::IsEqualToZero()
{ return this->Coefficient.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero);
}


template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
	::StringStreamPrintOutAppend(std::stringstream& out,::PolynomialOutputFormat& PolyFormat)
{ std::stringstream out1;
	std::string tempS;
	for(int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS,PolyFormat);
		if (tempS[0]=='-' && this->size>1)
			out1<< "("<<tempS<<")";
		else
			out1 << tempS;
	}
	tempS= out1.str();
	std::string tempS2;
	this->Coefficient.ElementToString(tempS2);
	if (tempS=="")
	{ out<<tempS2;
	}
	else
	{ if (tempS2=="1")
		{ out<< tempS; return;
		}
		if (tempS2=="-1")
		{ if (tempS=="-")
			{	tempS.erase(0,1);
				out<<tempS;
				return;
			}
			else
			{ out<<"-"<<tempS;
				return;
			}
		}
		out <<tempS2<< "("<<tempS<<")";
	}
}

class PolyPartFractionNumerator: public TemplatePolynomial
    <	MonomialInCommutativeAlgebra<	Integer,GeneratorsPartialFractionAlgebra, GeneratorPFAlgebraRecord>,
			Integer >
{
public:
	void ConvertToIntegerPoly(IntegerPoly& output, int theDimension);
};

class PolyPartFractionNumeratorLight: public
	ListBasicObjectsLight<ListBasicObjectsLight<GeneratorsPartialFractionAlgebra> >
{
public:
	ListBasicObjectsLight<int> Coefficients;
	void AssignPolyPartFractionNumerator(PolyPartFractionNumerator& from);
	void ComputePolyPartFractionNumerator(PolyPartFractionNumerator& output, int theDimension);
	void AssignPolyPartFractionNumeratorLight(const PolyPartFractionNumeratorLight& right);
	int NumGeneratorsUsed();
};

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
				::MakeConstantMonomial(short Nvar, ElementOfCommutativeRingWithIdentity& coeff)
{ this->ClearTheObjects();
	this->Coefficient.Assign(coeff);
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
			::MultiplyBy
		( MonomialInCommutativeAlgebra	<	ElementOfCommutativeRingWithIdentity,
																			GeneratorsOfAlgebra,
																			GeneratorsOfAlgebraRecord>& m,
			MonomialInCommutativeAlgebra	<	ElementOfCommutativeRingWithIdentity,
																			GeneratorsOfAlgebra,
																			GeneratorsOfAlgebraRecord>& output)
{ assert(&m!=&output);
  if (&output!= this)
  { output.Assign(*this);
  }
  output.MultiplyBy(m);
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::MultiplyByGenerator
  (GeneratorsOfAlgebra& g)
{	int x = this->IndexOfObjectHash(g);
  if (x==-1)
  { this->AddObjectOnTopHash(g);
  }
  else
  { this->TheObjects[x].GeneratorPower+=g.GeneratorPower;
    if (this->TheObjects[x].GeneratorPower==0)
    { this->PopIndexSwapWithLastHash(x);
			return 0;
    }
  }
	return 1;
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::MultiplyByGenerator
  (int GeneratorIndex,int GeneratorPower)
{	static ::GeneratorsPartialFractionAlgebra tempG;
	tempG.GeneratorIndex= GeneratorIndex;
	tempG.GeneratorPower= GeneratorPower;
	return this->MultiplyByGenerator(tempG);
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::MultiplyByGenerator
			(GeneratorsOfAlgebraRecord& g, int Power)
{ static ::GeneratorsPartialFractionAlgebra tempG;
	tempG.GeneratorIndex=GeneratorsOfAlgebra::theGenerators.IndexOfObjectHash(g);
	if (tempG.GeneratorIndex==-1)
	{ GeneratorsOfAlgebra::theGenerators.AddObjectOnTopHash(g);
		tempG.GeneratorIndex= GeneratorsOfAlgebra::theGenerators.size-1;
	}
	tempG.GeneratorPower=Power;
	return this->MultiplyByGenerator(tempG);
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
		::HashFunction()
{ int result=0;
	for (int i=0;i<this->size;i++)
	{ result+=this->TheObjects[i].HashFunction()*this->TheObjects[i].GeneratorPower;
	}
	return result;
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
	::ElementToString(std::string &output, PolynomialOutputFormat &PolyFormat)
{ std::stringstream out;
	this->StringStreamPrintOutAppend(out,PolyFormat);
	output=out.str();
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>
	::ComputeDebugString(PolynomialOutputFormat &PolyFormat)
{ this->ElementToString(this->DebugString,PolyFormat);
}


template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
inline void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::Assign
	(const MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>& right)
{ this->CopyFromHash(right);
	this->Coefficient.Assign(right.Coefficient);
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
inline void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::operator =
	(const MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>& right)
{ this->Assign(right);
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
inline bool MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::operator ==
	(MonomialInCommutativeAlgebra	<	ElementOfCommutativeRingWithIdentity,
																	GeneratorsOfAlgebra,
																	GeneratorsOfAlgebraRecord>& right)
{ static MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord> tempM;
	tempM.Assign(*this);
	for (int i=0;i<right.size;i++)
	{ static GeneratorsOfAlgebra tempG;
		tempG= right.TheObjects[i];
		tempG.GeneratorPower=- tempG.GeneratorPower;
		if (tempM.MultiplyByGenerator
					(right.TheObjects[i].GeneratorIndex, -right.TheObjects[i].GeneratorPower)==1
				)
			return false;
	}
	if (tempM.size>0)
		return false;
	return true;
}

template <class ElementOfCommutativeRingWithIdentity,
					class GeneratorsOfAlgebra,
					class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>::MultiplyBy
  ( MonomialInCommutativeAlgebra
		<	ElementOfCommutativeRingWithIdentity,
			GeneratorsOfAlgebra,
			GeneratorsOfAlgebraRecord>& m)
{ this->MakeActualSizeAtLeastExpandOnTop(this->size+m.size);
	for (int i=0;i<m.size;i++)
	{ this->MultiplyByGenerator(m.TheObjects[i]);
  }
  this->Coefficient.MultiplyBy(m.Coefficient);
}



class PolynomialsRationalCoeff: public Polynomials<Rational>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDubugString();
	void operator=(const PolynomialsRationalCoeff& right);
	bool operator==(const PolynomialsRationalCoeff& right);
	void ComputeB(PolynomialRationalCoeff& output,int cutOffIndex, int theDimension);
	void MakeUsualParametricRoot(int theDimension);
	void MakeOneParameterSubFromDirection(root& direction);
	void MakeOneParameterSubFromDirectionInts(int x1, int x2, int x3, int x4, int x5);
	void MakeOneParameterSubFromDirectionIntsAndConstants(int x1, int x2, int x3, int x4, int x5,
						int c1, int c2, int c3, int c4, int c5);
	void MakeSubFromMatrixIntAndDen(MatrixIntTightMemoryFit& theMat, int Den);
	void MakeSubFromMatrixRational(MatrixLargeRational& theMat);
	void ComputeDiscreteIntegrationUpTo(int d);
	void MakeLinearSubOnLastVariable(short NumVars,PolynomialRationalCoeff& LastVarSub);
	void MakeSubNVarForOtherChamber(root& direction,root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
	void MakeSubAddExtraVarForIntegration(root& direction);
	void Substitution(PolynomialsRationalCoeff& theSub, short NumVarsTarget);
};

class PolynomialsRationalCoeffCollection: public ListBasicObjects<PolynomialsRationalCoeff>
{
public:
	roots ChamberIndicators;
	std::string DebugString;
	void ElementToString(std::string& output, int theDimension);
	void ElementToStringComputeFunctionB(std::string& output, bool computingB, int theDimension);
	void ComputeDubugString(int theDimension);
	void Substitution(PolynomialsRationalCoeff& theSub, short NumVarsTarget);
};

class IntegerPoly: public Polynomial<Integer>
{
public:
	void MakePolyExponentFromIntRoot(intRoot& r);
	IntegerPoly(){};
	int SizeWithoutDebugString();
	void Evaluate(root& values, Rational& output);
	int EvaluateAtOne();
	static bool flagAnErrorHasOccurredTimeToPanic;
};

class IntegerPolyLight: public PolynomialLight<Integer>
{
public:
};

class PolynomialRationalCoeff: public Polynomial<Rational>
{
public:
	void AssignIntegerPoly(IntegerPoly& p);
	void Evaluate(intRoot& values,Rational& output);
	void MakePolyFromDirectionAndNormal
		(root& direction, root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
	void MakePolyExponentFromIntRoot(intRoot& r, GlobalVariables& theGlobalVariables);
	void MakeLinPolyFromInt(int theDimension,int x1,int x2, int x3,int x4, int x5);
	int FindGCMCoefficientDenominators();
	void MakeLinearPoly(short NumVars);
	int SizeWithoutDebugString();
	//works at the moment for linear polynomials only!!!!!!
	void DivByInteger(int x);
	void TimesInteger(int x);
	void operator=(const PolynomialRationalCoeff& right);
	bool operator==(const PolynomialRationalCoeff& right);
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
void Monomial<ElementOfCommutativeRingWithIdentity>::InvertDegrees()
{ for (int i=0;i<this->NumVariables;i++)
		this->degrees[i]= - this->degrees[i];
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::init(short nv)
{	assert(nv>=0);
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=nv-this->NumVariables;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	if(this->NumVariables!=nv)
	{	NumVariables=nv;
		delete [] degrees;
		degrees= new short[NumVariables];
	}
	for (int i=0;i<NumVariables;i++)
	{degrees[i]=0;}
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::initNoDegreesInit(short nv)
{
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=nv-this->NumVariables;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	if(this->NumVariables!=nv)
	{	this->NumVariables=nv;
		delete [] this->degrees;
		this->degrees= new short[this->NumVariables];
	}
}

template <class ElementOfCommutativeRingWithIdentity>
Monomial<ElementOfCommutativeRingWithIdentity>::~Monomial()
{	delete [] this->degrees;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->NumVariables;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::StringStreamPrintOutAppend
	(std::stringstream& out,PolynomialOutputFormat& PolyFormat)
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
		tempS="-";
	if (tempS=="1")
		tempS.clear();
	out <<tempS;
	for (int i=0;i<this->NumVariables;i++)
		if (this->degrees[i]!=0)
		{	out<<	PolyFormat.alphabet[i];
			if (!PolynomialOutputFormat::UsingLatexFormat)
			{	if (this->degrees[i]!=1)
					out << "^"<<this->degrees[i];
			}
			else
			{	if (this->degrees[i]!=1)
					out << "^{"<<this->degrees[i]<<"}";
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
bool Monomial<ElementOfCommutativeRingWithIdentity>::IsGEQpartialOrder
	(Monomial<ElementOfCommutativeRingWithIdentity>&m)
{ assert(this->NumVariables == m.NumVariables);
	for (int i=0;i<m.NumVariables;i++)
		if (this->degrees[i]<m.degrees[i])
			return false;
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>
	::IsGEQ(Monomial<ElementOfCommutativeRingWithIdentity> &m)
{ assert(this->NumVariables==m.NumVariables);
	for (int i=0;i<this->NumVariables;i++)
	{	if (this->degrees[i]>m.degrees[i])
			return true;
		if (this->degrees[i]<m.degrees[i])
			return false;
	}
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::ElementToString
           (std::string &output, PolynomialOutputFormat &PolyFormat)
{ std::stringstream out;
	this->StringStreamPrintOutAppend(out,PolyFormat);
	output=out.str();
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::ComputeDebugString(PolynomialOutputFormat &PolyFormat)
{	this->ElementToString(this->DebugString,PolyFormat);
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::ComputeDebugString()
{	this->ElementToString(this->DebugString,PolyFormatLocal);
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
{
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=increase;
	if (ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::cgiLimitRAMuseNumPointersInListBasicObjects; std::exit(0);}
#endif
	short* newDegrees= new short[NumVariables+increase];
	for(int i=0;i<this->NumVariables;i++)
		newDegrees[i]=degrees[i];
	for(int i=NumVariables;i<this->NumVariables+increase;i++)
		newDegrees[i]=0;
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
																(const Monomial<ElementOfCommutativeRingWithIdentity>&  m)
{ this->initNoDegreesInit(m.NumVariables);
	for (int i=0;i<NumVariables;i++)
	{	this->degrees[i]=m.degrees[i];
	}
	this->Coefficient.Assign (m.Coefficient);
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::IsAConstant()
{ for (int i=0;i<this->NumVariables;i++)
		if (this->degrees[i]!=0) return false;
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MakeFromRoot
	(ElementOfCommutativeRingWithIdentity& coeff, intRoot& input)
{ this->init((short)input.dimension);
	this->Coefficient.Assign(coeff);
	for (int i=0;i<this->NumVariables;i++)
		this->degrees[i]=(short) input.elements[i];
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::
	MonomialExponentToColumnMatrix(MatrixLargeRational& output)
{ output.init(this->NumVariables,1);
	for (int i=0;i<this->NumVariables;i++)
		output.elements[i][0].AssignInteger(this->degrees[i]);
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MonomialExponentToRoot(root& output)
{ output.SetSizeExpandOnTopLight(this->NumVariables);
	for (int i=0;i<this->NumVariables;i++)
		output.TheObjects[i].AssignInteger(this->degrees[i]);
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::MonomialExponentToRoot(intRoot& output)
{ output.dimension=this->NumVariables;
	for (int i=0;i<this->NumVariables;i++)
		output.elements[i]=this->degrees[i];
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::DivideByExponentOnly
	(	intRoot& input)
{	assert(input.dimension==this->NumVariables);
	for (int i=0;i<this->NumVariables;i++)
		this->degrees[i]-=(short)input.elements[i];
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::DivideBy
	(	Monomial<ElementOfCommutativeRingWithIdentity> &input,
		Monomial<ElementOfCommutativeRingWithIdentity> &output)
{ output.init(this->NumVariables);
	output.Coefficient.Assign(this->Coefficient);
	output.Coefficient.DivideBy(input.Coefficient);
	for (int i=0;i<this->NumVariables;i++)
		output.degrees[i]=this->degrees[i]-input.degrees[i];
}

template <class ElementOfCommutativeRingWithIdentity>
void Monomial<ElementOfCommutativeRingWithIdentity>::DecreaseNumVariables(short increment)
{
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=increment;
	if(ParallelComputing::GlobalPointerCounter>::cgiLimitRAMuseNumPointersInListBasicObjects)std::exit(0);
#endif
	short* newDegrees= new short[NumVariables-increment];
	for(int i=0;i<this->NumVariables-increment;i++)
	{	newDegrees[i]=degrees[i];
	}
	this->NumVariables-=increment;
	delete [] degrees;
	degrees= newDegrees;
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::IsPositive()
{ if (this->Coefficient.IsEqualToZero())
		return true;
	for (int i=0;i<this->NumVariables;i++)
		if (this->degrees[i]<0)
			return false;
	return true;
}

template <class ElementOfCommutativeRingWithIdentity>
bool Monomial<ElementOfCommutativeRingWithIdentity>::IsEqualToZero()
{ return this->Coefficient.IsEqualTo(ElementOfCommutativeRingWithIdentity::TheRingZero);
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
void Monomial<ElementOfCommutativeRingWithIdentity>::Assign(const Monomial<ElementOfCommutativeRingWithIdentity>& m)
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

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
inline void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::operator =(const
				  TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>& right)
{ this->CopyFromPoly(right);
}


template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::
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

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
inline void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::
				ReadFromFile(std::fstream& input, short NumV)
{ std::string tempS;
	static TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity> Buffer;
	Buffer.Nullify(NumV);
	TemplateMonomial tempM;
	tempM.init(NumV);
	input>>tempS;
	assert (tempS=="|/--");
	int tempSize;
	input>>tempSize;
	Buffer.MakeActualSizeAtLeastExpandOnTop(tempSize);
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

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
inline void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>
					::MultiplyBy(TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity> &p,
									 TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity> &output)
{	if (p.size==0)
	{ output.ClearTheObjects();
		return;
	}
	static TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity> Accum;
	static TemplateMonomial tempM;
	Accum.MakeActualSizeAtLeastExpandOnTop(this->size*p.size);
	Accum.ClearTheObjects();
	Accum.NumVars= p.NumVars;
	for (int i=0;i<p.size;i++)
	{	for (int j=0;j<this->size;j++)
		{	this->TheObjects[j].MultiplyBy(p.TheObjects[i],tempM);
//			tempM.ComputeDebugString(PolyFormatLocal);
//			Accum.ComputeDebugString();
			Accum.AddMonomial(tempM);
			ParallelComputing::SafePoint();
//			Accum.ComputeDebugString();
		}
	}
	output.CopyFromPoly(Accum);
}

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>
			::MultiplyBy(TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity> &p)
{	this->MultiplyBy(p,*this);
}

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>::MakeNVarConst
				(short nVar, ElementOfCommutativeRingWithIdentity& coeff)
{	static TemplateMonomial tempM;
	this->ClearTheObjects();
	this->NumVars=nVar;
	tempM.MakeConstantMonomial(nVar,coeff);
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

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
bool TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::IsEqualToZero()
{	if(this->size==0)
	{return true;}
	for (int i=0;i<this->size;i++)
	{	if (!this->TheObjects[i].IsEqualToZero())
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
{ this->Nullify((short)r.size);
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	for (int i=0;i<r.size;i++)
	{ tempM.MakeNVarFirstDegree(i,(short)r.size,
															ElementOfCommutativeRingWithIdentity::TheRingUnit);
		tempM.Coefficient.Assign(r.TheObjects[i]);
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


template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
inline int TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::HasSameExponentMonomial(TemplateMonomial& m)
{ return this->IndexOfObjectHash(m);
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>
				::AddPolynomial(TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>& p)
{	this->MakeActualSizeAtLeastExpandOnTop(p.size+this->size);
	//std::string tempS1;
	/*if (QuasiPolynomial::flagAnErrorHasOccurredTimeToPanic)
	{	std::string tempS;
		RandomCodeIDontWantToDelete::EvilList1.AddObjectOnTop(tempS);
		tempS1=& RandomCodeIDontWantToDelete::EvilList1.TheObjects
								[RandomCodeIDontWantToDelete::EvilList1.size-1];
	}*/
/*	Rational tempRat,tempRat2;
	IntegerPoly* tempP;
	if (IntegerPoly::flagAnErrorHasOccurredTimeToPanic)
	{ std::string tempS;
		tempP=(IntegerPoly*)this;
		tempP->Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot,tempRat);
		tempRat.ElementToString(tempS);
		//	currentList->AddObjectOnTop(tempS);
	}*/
	for (int i=0;i<p.size;i++)
	{	/*if (QuasiPolynomial::flagAnErrorHasOccurredTimeToPanic)
		{	std::string tempS;
			Rational tempRat;
			QuasiPolynomial* tempP;
			tempP=(QuasiPolynomial*)this;
			tempP->Evaluate(partFraction::theVectorToBePartitioned,tempRat);
			tempRat.ElementToString(tempS);
		//	currentList->AddObjectOnTop(tempS);
			tempS1->append(tempS);
			tempS1->append("\n");
		}*/
		ParallelComputing::SafePoint();
		this->AddMonomial(p.TheObjects[i]);
	}
	/*if (IntegerPoly::flagAnErrorHasOccurredTimeToPanic)
	{ std::string tempS1,tempS2;
		tempP->Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot,tempRat2);
		tempRat.ElementToString(tempS1);
		tempRat2.ElementToString(tempS2);
		this->ComputeDebugString();
		//assert(tempRat.IsEqualTo(tempRat2));
	}*/
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
bool TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::
               HasGEQMonomial(TemplateMonomial& m, int& WhichIndex)
{ for (int i=0;i<this->size;i++)
	{ if (this->TheObjects[i].IsGEQpartialOrder(m))
		{ WhichIndex=i;
			return true;
		}
	}
	WhichIndex=-1;
	return false;
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
int TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::StringPrintOutAppend
															(std::string& output, PolynomialOutputFormat& PolyFormat)
{	std::stringstream out;
	int NumChars=0;
	int TotalNumLines=0;
	std::string tempS;
	const std::string LatexBeginString="";// "\\begin{eqnarray*}\n&&";
//	if (PolyFormat.UsingLatexFormat)
//	{ out <<LatexBeginString;
//	}
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS,PolyFormat);
		if (tempS[0]=='0'){tempS.erase(0,1);}
		if (tempS.size()!=0)
		{ if (tempS[0]!='-'){out <<"+";}
			out <<tempS;
			if (PolyFormat.UsingLatexFormat)
			{ if (((signed)tempS.size())> PolyFormat.LatexMaxLineLength)
				{ bool found=false; int extraChars=0;
					for (int j=(signed)tempS.size();j>=1;j--)
						if (tempS[j]=='\\' && tempS[j-1]=='\\')
						{ found=true;	break;}
						else
							extraChars++;
					if (found)	NumChars=extraChars;	else	NumChars+=extraChars;
				}
				else
					NumChars+= tempS.size();
				if (NumChars>PolyFormat.LatexMaxLineLength)
				{ NumChars=0;
					out <<"\\\\&&\n ";
					TotalNumLines++;
				}
			}
			if (PolyFormat.cutOffString && out.str().size()> PolyFormat.cutOffSize)
			{ out <<"...";
				break;
			}
		}
	}
//	std::string tempS;
	tempS= out.str();
	if (tempS.size()!=0)
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
int Polynomial<ElementOfCommutativeRingWithIdentity>::GetIndexMaxMonomial()
{ if (this->size==0)
		return -1;
	int result;
	result = 0;
	for (int i=1;i<this->size;i++)
		if (this->TheObjects[i].IsGEQ(this->TheObjects[result]))
			result=i;
	return result;
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::ScaleToPositiveMonomials
	(Monomial<ElementOfCommutativeRingWithIdentity> &outputScale)
{ outputScale.init(this->NumVars);
	outputScale.Coefficient.Assign(ElementOfCommutativeRingWithIdentity::TheRingUnit);
	for (int i=0;i<this->NumVars;i++)
	{ outputScale.degrees[i]= 0;
		for (int j=0;j<this->size;j++)
			outputScale.degrees[i]= ::MathRoutines::Minimum
				(outputScale.degrees[i], this->TheObjects[j].degrees[i]);
	}
	Monomial<ElementOfCommutativeRingWithIdentity> tempMon;
	tempMon.Assign(outputScale);
	tempMon.InvertDegrees();
	this->MultiplyByMonomial(tempMon);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::DivideBy
	(	Polynomial<ElementOfCommutativeRingWithIdentity> &inputDivisor,
		Polynomial<ElementOfCommutativeRingWithIdentity> &outputQuotient,
		Polynomial<ElementOfCommutativeRingWithIdentity> &outputRemainder)
{ assert(&outputQuotient!=this && &outputRemainder!=this && &outputQuotient!=&outputRemainder);
	outputRemainder.Assign(*this);
	Monomial<ElementOfCommutativeRingWithIdentity> scaleRemainder;
	Monomial<ElementOfCommutativeRingWithIdentity> scaleInput;
	Polynomial<ElementOfCommutativeRingWithIdentity> tempInput;
	tempInput.Assign(inputDivisor);
	outputRemainder.ScaleToPositiveMonomials(scaleRemainder);
	tempInput.ScaleToPositiveMonomials(scaleInput);
	int remainderMaxMonomial=outputRemainder.GetIndexMaxMonomial();
	int inputMaxMonomial= tempInput.GetIndexMaxMonomial();
	outputQuotient.Nullify(this->NumVars);
	if (remainderMaxMonomial==-1)
		return;
	outputQuotient.MakeActualSizeAtLeastExpandOnTop(this->size);
	Monomial<ElementOfCommutativeRingWithIdentity> tempMon;
	tempMon.init(this->NumVars);
	Polynomial<ElementOfCommutativeRingWithIdentity> tempP;
	tempP.MakeActualSizeAtLeastExpandOnTop(this->size);
	if (this->flagAnErrorHasOccuredTimeToPanic)
	{ this->ComputeDebugString();
		tempInput.ComputeDebugString();
	}
	assert(remainderMaxMonomial<outputRemainder.size);
	assert(inputMaxMonomial<tempInput.size);
	while (outputRemainder.TheObjects[remainderMaxMonomial].IsGEQ(tempInput.TheObjects[inputMaxMonomial]))
	{ assert(remainderMaxMonomial<outputRemainder.size);
		outputRemainder.TheObjects[remainderMaxMonomial].DivideBy(tempInput.TheObjects[inputMaxMonomial],tempMon);
		if (!tempMon.IsPositive())
			break;
		if (this->flagAnErrorHasOccuredTimeToPanic)
			tempMon.ComputeDebugString();
		outputQuotient.AddMonomial(tempMon);
		tempP.Assign(tempInput);
		tempP.MultiplyByMonomial(tempMon);
		tempP.TimesConstant(ElementOfCommutativeRingWithIdentity::TheRingMUnit);
		outputRemainder.AddPolynomial(tempP);
		remainderMaxMonomial= outputRemainder.GetIndexMaxMonomial();
		if (remainderMaxMonomial==-1)
			break;
		if (this->flagAnErrorHasOccuredTimeToPanic)
			outputRemainder.ComputeDebugString();
	}
	scaleInput.InvertDegrees();
	outputQuotient.MultiplyByMonomial(scaleInput);
	outputQuotient.MultiplyByMonomial(scaleRemainder);
	outputRemainder.MultiplyByMonomial(scaleRemainder);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::DivideByConstant
                 (ElementOfCommutativeRingWithIdentity& r)
{	for (int i=0;i<this->size;i++)
		this->TheObjects[i].Coefficient.DivideBy(r);
}

template <class ElementOfCommutativeRingWithIdentity>
void Polynomial<ElementOfCommutativeRingWithIdentity>::TimesInteger(int a)
{	Rational r;
	r.AssignInteger(a);
	this->TimesRational(r);
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
inline void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>
					::CopyFromPoly(const TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>& p)
{	this->Assign(p);
}

template <class TemplateMonomial, class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::Assign
				(const TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>& p)
{	this->CopyFromHash(p);
	this->NumVars= p.NumVars;
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<	TemplateMonomial, ElementOfCommutativeRingWithIdentity>::AddMonomial(TemplateMonomial& m)
{	int j= this->IndexOfObjectHash(m);
	if (j==-1)
	{	if (!m.IsEqualToZero())
			this->AddObjectOnTopHash(m);
	}
	else
	{	this->TheObjects[j].Coefficient.Add(m.Coefficient);
		if (this->TheObjects[j].IsEqualToZero())
			this->PopIndexSwapWithLastHash(j);
	}
}


template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>
					::MultiplyByMonomial(TemplateMonomial& m)
{ this->MultiplyByMonomial(m,*this);
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity >::MultiplyByMonomial
			(TemplateMonomial& m,	TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>& output)
{	if (m.IsEqualToZero())
	{ output.ClearTheObjects();
		return;
	}
	static TemplateMonomial tempM;
	static TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity> Accum;
	Accum.ClearTheObjects();
	Accum.NumVars= this->NumVars;
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].MultiplyBy(m,tempM);
		Accum.AddMonomial(tempM);
	}
	output.CopyFromPoly(Accum);
}


template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>::RaiseToPower
						(int d,TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>& output)
{	assert(d>=0);
	if (d==0)
	{	short nv=this->NumVars;
		this->MakeNVarConst(nv,ElementOfCommutativeRingWithIdentity::TheRingUnit);
		return;
	}
	static TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity> Accum;
	Accum.CopyFromPoly(*this);
	for (int i=0;i<d-1;i++)
		Accum.MultiplyBy(*this);
	output.CopyFromPoly(Accum);
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial,ElementOfCommutativeRingWithIdentity>::RaiseToPower(int d)
{	this->RaiseToPower(d,*this);
}

template <class ElementOfCommutativeRingWithIdentity>
int Polynomial<ElementOfCommutativeRingWithIdentity>::TotalDegree()
{	int result=0;
	for (int i=0;i<this->TheMonomials.size;i++)
		result=Maximum(this->TheMonomials.TheObjects[i]->TotalDegree(),result);
	return result;
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
void TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::Nullify(short NVar)
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
void Polynomial<ElementOfCommutativeRingWithIdentity>::AssignPolynomialLight
	(const PolynomialLight<ElementOfCommutativeRingWithIdentity>& from)
{	this->Nullify(from.NumVars);
	this->MakeActualSizeAtLeastExpandOnTop(from.size);
	for (int i=0;i<from.size;i++)
	{ this->AddMonomial(from.TheObjects[i]);
	}
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

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
bool TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::ComputeDebugString()
{	this->DebugString.clear();
	this->StringPrintOutAppend(this->DebugString,PolyFormatLocal);
	return true;
}

template <class TemplateMonomial,class ElementOfCommutativeRingWithIdentity>
TemplatePolynomial<TemplateMonomial, ElementOfCommutativeRingWithIdentity>::TemplatePolynomial()
{ this->NumVars=0;
}

template <class ElementOfCommutativeRingWithIdentity>
Polynomial<ElementOfCommutativeRingWithIdentity>::Polynomial(int degree, ElementOfCommutativeRingWithIdentity& coeff)
{	this->MakeOneVarMonomial(degree,coeff);
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
			 MakeExponentSubstitution(MatrixIntTightMemoryFit& theSub)
{ static Polynomial<ElementOfCommutativeRingWithIdentity> tempP;
	static Monomial<ElementOfCommutativeRingWithIdentity> tempM;
	tempM.init((short)theSub.NumRows);
	tempM.Coefficient.Assign(ElementOfCommutativeRingWithIdentity::TheRingUnit);
	this->size=0;
	this->SetSizeExpandOnTopNoObjectInit(theSub.NumCols);
	for (int i=0;i<theSub.NumCols;i++)
	{	for (int j=0;j<theSub.NumRows;j++)
			tempM.degrees[j]=(short) theSub.elements[j][i];
		tempP.Nullify((short)theSub.NumRows);
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


class BasicComplexNumber
{
public:
	Rational Coeff;
	Rational Exp;
	void operator=(const BasicComplexNumber& c);
	bool operator==(const BasicComplexNumber& c);
	void Assign(const BasicComplexNumber& c);
	void MultiplyBy(BasicComplexNumber& c);
	void MultiplyByLargeRational(Rational& c);
	void AssignRational(Rational& r);
	void AssignLargeRational(const Rational& r);
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
	void Assign(const CompositeComplex& c);
	void Add(CompositeComplex& c);
	void AddRational(Rational& r);
	void AddLargeRational(Rational& r);
	void MultiplyByLargeRational(Rational& r);
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
		BasicQN::GlobalCollectorsBasicQuasiNumbers.AddObjectOnTop(this);
	};
	BasicQN()
	{	BasicQN::NumTotalCreated++; CreationNumber=NumTotalCreated;
		BasicQN::GlobalCollectorsBasicQuasiNumbers.AddObjectOnTop(this);
	};
	void MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column);
	Rational Coefficient;
	MatrixIntTightMemoryFit Exp;
	MatrixIntTightMemoryFit Nums;
	short NumVars;
	int Den;
	int HashFunction();
	void ScaleBy(int DenIncrease);
//	Selection PivotPoints;
	void ExpToDebugString();
	short GaussianEliminationByRows();
//	void AddBasicComplexNumber(BasicComplexNumber& b);
//	void MultiplyByTauSameDenMethod1(int* tau, int k);
	void Assign(const BasicQN& p);
	int InvertModN( int d, int p);
	void SwitchTwoRows(int rowI1,int rowI2, int StartCol);
	void RowPlusScalarTimesRow(int rowInd, int scalar, int otherRowInd, int StartCol);
	void MultiplyRowBy(int rowInd, int scalar, int StartColInd);
	void SetPivotRow(int index, int PivotRowIndex, int Col);
	void MultiplyBySameDen(BasicQN& q);
	void MultiplyBy(BasicQN& q);
	void MultiplyByLargeRational(Rational& r);
	void ComputeDebugString();
 	void ElementToString(std::string& output, PolynomialOutputFormat& PolyFormat);
 	void ElementToString(std::string& output);
	bool HasSameExponent(BasicQN& q);
	bool ExponentIsEqualToZero();
	void DecreaseNumVars(short decrease);
	bool IsEqualToZero();
	void MakeConst(Rational& Coeff, short NumV);
	void MakeFromNormalAndDirection(root& normal, root& direction,
																	int theMod, Rational& coeff);
	void MakePureQN(short NumVariables,int NonZeroIndex, Rational&coeff,
									int theExp, int Num, int theDen);
	void BasicQNToComplexQN(CompositeComplexQN& output);
	//for format of the substitution see class qQPSub
	void LinearSubstitution(QPSub& theSub);
	void operator =(const BasicQN& q);
	bool operator ==(BasicQN& q);
	void Simplify();
	void GetCoeffInFrontOfLast(Rational& output);
	void Evaluate(int* theVars, Rational& output);
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
	static bool flagAnErrorHasOccurredTimeToPanic;
	void AddBasicQuasiNumber(BasicQN& q);
	void Add(QuasiNumber &q);
	void MultiplyByBasicQuasiNumber(BasicQN& q);
	void MultiplyBy(QuasiNumber& q);
	void Assign(const QuasiNumber& q);
	void AssignLargeRational(short NumVars, Rational& coeff);
	void AssignInteger(short NumVars, int x);
	void ElementToString(std::string& output, PolynomialOutputFormat& PolyFormat);
	void ElementToString(std::string& output);
	void MakeConst(Rational& Coeff, short NumV);
	void DivideByRational(Rational& r);
//	void DecreaseNumVars(int decrease);
	void MultiplyByLargeRational(Rational& r);
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
	void Evaluate(int* theVars, Rational& output);
	int FindGCMDens();
	void QNtoComplex(CompositeComplexQN& output);
	void MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column);
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
		ComplexQN::GlobalCollectorsComplexQNs.AddObjectOnTop(this);
	};
	ComplexQN()
	{	this->NumVars=NumVars;ComplexQN::NumTotalCreated++;CreationNumber=NumTotalCreated;
		ComplexQN::GlobalCollectorsComplexQNs.AddObjectOnTop(this);
	};
	CompositeComplex Coefficient;
	Rational Exponent[MaxRank+1];
	int NumVars;
//	void AddBasicComplexNumber(BasicComplexNumber& b);
	void MultiplyBy(ComplexQN& q);
	void MultiplyByBasicComplex(BasicComplexNumber& b);
	void MultiplyByLargeRational(Rational& r);
	void ComputeDebugString();
	void CopyFrom(const ComplexQN& q);
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
	void LinearSubstitution(MatrixLargeRational& TheSub);
//	void RootSubsti
	void operator =(const ComplexQN& q);
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
	void Assign(const CompositeComplexQN& q);
	void ElementToString(std::string& output);
	void MakeConst(Rational& Coeff, int numVars);
	void DivideByRational(Rational& r);
	void Simplify();
	void MultiplyByLargeRational(Rational& r);
	void MakePureQN(Rational* expArg,int numVars);
	void MakePureQN(Rational* expArg,int numVars, Rational& coeff);
	void MakePureQN(Rational& constExp,int numVars, Rational&coeff);
	void MakePureQN(int numVars,int NonZeroIndex, Rational&coeff, Rational&Exp);
	void MakePureQN(int numVars,int NonZeroIndex, Rational&coeffExp, Rational& ConstExp, Rational& Coeff);
	//see PolynomialPointersKillOnExit::MakeLinearSubstitution
	//for the substitution format!
	void LinearSubstitution(MatrixLargeRational& TheSub);
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
	static bool flagAnErrorHasOccurredTimeToPanic;
	void AssignPolynomialRationalCoeff(PolynomialRationalCoeff& p);
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
	void Evaluate(intRoot& values,Rational& output);
	int SizeWithoutDebugString();
	void operator=(QuasiPolynomial& right);
	void Simplify();
	QuasiPolynomial();
	void Nullify(short numVars);
};

class CompositeComplexQNSub
{
public:
	MatrixLargeRational MatrixForCoeffs;
	PolynomialsRationalCoeff RationalPolyForm;
	void MakeLinearSubIntegrand(root& normal, root&direction, Rational& Correction, GlobalVariables& theGlobalVariables);
	void MakeSubNVarForOtherChamber(root& direction,root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
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
	MatrixIntTightMemoryFit TheQNSub;
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
	void MakeSubFromMatrixInt(MatrixIntTightMemoryFit& theMat);
	void MakeSubFromMatrixIntAndDen(MatrixIntTightMemoryFit& theMat, int Den);
	void MakeSubFromMatrixRational(MatrixLargeRational& theMat);
	void MakeLinearSubIntegrand(root& normal, root&direction, Rational& Correction, GlobalVariables& theGlobalVariables);
	void MakeSubNVarForOtherChamber(root& direction,root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
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
	ListBasicObjectsLight<int> Multiplicities;
	ListBasicObjectsLight<int> Elongations;
	void ComputeDebugString();
	void ComputeDebugStringBasisChange(MatrixIntTightMemoryFit& VarChange);
	void AddMultiplicity(int MultiplicityIncrement, int Elongation);
	int IndexLargestElongation();
	int GetLargestElongation();
	void GetPolyDenominator
		(IntegerPoly& output, int MultiplicityIndex, intRoot& theExponent);
	int GetMultiplicityLargestElongation();
	int GetLCMElongations();
	int GetTotalMultiplicity();
	void invert();
	void init();
	static root CheckSumRoot;
	int HashFunction();
	void ComputeOneCheckSum(Rational &output, intRoot& theExp, int theDimension);
	bool IsHigherThan(oneFracWithMultiplicitiesAndElongations& f);
	void operator=(oneFracWithMultiplicitiesAndElongations& right);
	bool operator==(oneFracWithMultiplicitiesAndElongations& right);
	void ElementToString(std::string& output, int index, bool LatexFormat);
	void ElementToStringBasisChange
		(	partFractions& owner, MatrixIntTightMemoryFit& VarChange,
			bool UsingVarChange, std::string& output,
			bool LatexFormat, int index, int theDimension);
	void OneFracToStringBasisChange
		(	partFractions& owner,int indexElongation, MatrixIntTightMemoryFit& VarChange,
			bool UsingVarChange, std::string& output, bool LatexFormat,
			int indexInFraction, int theDimension);
};

class rootWithMultiplicity: public root
{
public:
	int multiplicity;
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
	void operator=(const rootWithMultiplicity& right)
	{	this->multiplicity= right.multiplicity;
		this->root::operator= (right);
	};
	bool operator==(const rootWithMultiplicity& right)
	{ return	this->root::operator ==(right)	&&
						this->multiplicity== right.multiplicity;
	};
	void GetSum(root& output)
	{ output.Assign(*this);
		output.MultiplyByInteger(this->multiplicity);
	};
};

class rootsWithMultiplicity: public ListBasicObjects<rootWithMultiplicity>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
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
	void GetSum(root& output, int theDimension)
	{ output.MakeZero(theDimension);
		root tempRoot;
		for(int i=0;i<this->size;i++)
		{	this->TheObjects[i].GetSum(tempRoot);
			output.Add(tempRoot);
		}
	};
};

class rootsWithMultiplicitiesContainer: public ListBasicObjects<rootsWithMultiplicity>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
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
	int getIndexDoubleOfARoot(intRoot& TheRoot);
	void ComputeTable(int theDimension);
};

class partFractionPolynomials: public ListBasicObjects<PolynomialRationalCoeff>
{
public:
	roots LatticeIndicators;
	MatrixLargeRational theNormals;
	void CheckConsistency(root& RootLatticeIndicator,PolynomialRationalCoeff& input);
	void initLatticeIndicatorsFromPartFraction
		(	partFractions& ownerExpression,partFraction &owner,
			GlobalVariables& theGlobalVariables, int theDimension);
	void AddPolynomialLargeRational
		(	root& rootLatticeIndicator, PolynomialRationalCoeff& input);
	void ComputeQuasiPolynomial
		(	QuasiPolynomial& output, bool RecordNumMonomials,
			int theDimension, GlobalVariables& theGlobalVariables);
};

class partFraction: ListBasicObjectsLight<oneFracWithMultiplicitiesAndElongations>
{
private:
	void findPivot();
	void findInitialPivot();
	//void intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent);
	bool rootIsInFractionCone
		(partFractions& owner, root& r, GlobalVariables& theGlobalVariables);
	friend class partFractions;
	friend class partFractionPolynomials;
public:
	std::string DebugString;
	int LastDistinguishedIndex;
	int FileStoragePosition;
	bool PowerSeriesCoefficientIsComputed;
	bool IsIrrelevant;
	bool RelevanceIsComputed;
	ListBasicObjects<int> IndicesNonZeroMults;
	IntegerPolyLight Coefficient;
	PolyPartFractionNumeratorLight CoefficientNonExpanded;
	bool RemoveRedundantShortRootsClassicalRootSystem
		(	partFractions& owner, root* Indicator, GlobalVariables& theGlobalVariables,
			int theDimension);
	bool RemoveRedundantShortRoots
		(	partFractions& owner, root* Indicator,
			GlobalVariables& theGlobalVariables, int theDimension);
	bool AlreadyAccountedForInGUIDisplay;
	static	bool flagAnErrorHasOccurredTimeToPanic;
//	static	bool flagUsingPrecomputedOrlikSolomonBases;
	static	bool UncoveringBrackets;
	static	std::fstream TheBigDump;
	static	bool UseGlobalCollector;
	static	bool MakingConsistencyCheck;
	static Rational CheckSum, CheckSum2;
	static intRoot theVectorToBePartitioned;
	static ListObjectPointers<partFraction> GlobalCollectorPartFraction;
	void ComputePolyCorrespondingToOneMonomial
		(	PolynomialRationalCoeff &output, int index, roots& normals,
			partFractionPolynomials* SplitPowerSeriesCoefficient, int theDimension);
	static void MakePolynomialFromOneNormal
		(	root& normal, root& shiftRational, int theMult,	PolynomialRationalCoeff& output);
	void ComputeNormals
		(	partFractions& owner, roots& output, int theDimension,
			GlobalVariables& theGlobalVariables);
	int ComputeGainingMultiplicityIndexInLinearRelation
		(	partFractions& owner,	MatrixLargeRational& theLinearRelation);
	void UncoverBracketsNumerator(GlobalVariables&  theGlobalVariables, int theDimension);
	void partFractionToPartitionFunctionSplit
		(	partFractions& owner, QuasiPolynomial& output, bool RecordNumMonomials,
			bool StoreToFile, GlobalVariables& theGlobalVariables, int theDimension);
	//void partFractionToPartitionFunctionStoreAnswer
	//			(	QuasiPolynomial& output, bool RecordSplitPowerSeriesCoefficient,
	//				bool StoreToFile);
	bool IsEqualToZero();
	void ComputeDebugString(partFractions& owner, GlobalVariables& theGlobalVariables);
	void ComputeDebugStringBasisChange(MatrixIntTightMemoryFit& VarChange);
	//void InsertNewRootIndex(int index);
	//void MultiplyMinusRootShiftBy (int* theRoot, int Multiplicity);
	void MultiplyCoeffBy(Rational& r);
	void decomposeAMinusNB
		(	int indexA, int indexB, int n,int indexAminusNB, partFractions& Accum,
			GlobalVariables& theGlobalVariables);
	bool DecomposeFromLinRelation
		(	MatrixLargeRational& theLinearRelation, partFractions& Accum,
			GlobalVariables& theGlobalVariables);
	void ComputeOneCheckSum
		(	partFractions& owner,Rational &output, int theDimension,
			GlobalVariables& theGlobalVariables);
	void AttemptReduction
		(partFractions& owner, int myIndex, GlobalVariables& theGlobalVariables);
	void ReduceMonomialByMonomial
		(partFractions& owner, int myIndex, GlobalVariables& theGlobalVariables);
	void ApplySzenesVergneFormula
		(	ListBasicObjects<int> &theSelectedIndices, ListBasicObjects<int>& theElongations,
			int GainingMultiplicityIndex,int ElongationGainingMultiplicityIndex,
			partFractions& Accum, GlobalVariables& theGlobalVariables);
	void ApplyGeneralizedSzenesVergneFormula
		(	ListBasicObjects<int> &theSelectedIndices,
			ListBasicObjects<int> &theGreatestElongations,
			ListBasicObjects<int> &theCoefficients, int GainingMultiplicityIndex,
			int ElongationGainingMultiplicityIndex, partFractions &Accum,
			GlobalVariables& theGlobalVariables);
	bool CheckForOrlikSolomonAdmissibility(ListBasicObjects<int>& theSelectedIndices);
	bool reduceOnceTotalOrderMethod
		(partFractions&Accum, GlobalVariables& theGlobalVariables);
//	void reduceOnceOrlikSolomonBasis(partFractions&Accum);
	bool reduceOnceGeneralMethodNoOSBasis
		(partFractions& Accum, GlobalVariables& theGlobalVariables);
	bool reduceOnceGeneralMethod
		(partFractions& Accum, GlobalVariables& theGlobalVariables);
	bool AreEqual(partFraction& p);
	bool IsReduced();
	int HashFunction();
	int MultiplyByOneFrac(oneFracWithMultiplicitiesAndElongations& f);
	void init(int numRoots);
	//int Elongate(int indexElongatedFraction, int theElongation);
	void ComputeIndicesNonZeroMults();
	bool DecreasePowerOneFrac(int index, int increment);
	//void GetNumerator(PolynomialRationalCoeff& output);
	//void SetNumerator(PolynomialRationalCoeff& input);
	void PrepareFraction
		(	int indexA, int indexB,  int AminusNBindex,
			bool indexAisNullified, partFraction &output,
			IntegerPoly& AminusNbetaPoly);
	void Assign(const partFraction&p);
	void AssignDenominatorOnly(const partFraction& p);
	void AssignNoIndicesNonZeroMults(partFraction&p);
	int getSmallestNonZeroIndexGreaterThanOrEqualTo(partFractions& owner,int minIndex);
	int ControlLineSizeFracs(std::string& output);
	int ControlLineSizeStringPolys(std::string& output);
	//void swap(int indexA,int indexB);
	partFraction();
	~partFraction();
	void GetPolyReduceMonomialByMonomial
		(	partFractions& owner, GlobalVariables& theGlobalVariables,
			intRoot& theExponent, int StartMonomialPower, int DenPowerReduction,
			int startDenominatorPower,IntegerPoly& output);
	void ReduceMonomialByMonomialModifyOneMonomial
		(	partFractions& Accum,GlobalVariables& theGlobalVariables,
			SelectionWithDifferentMaxMultiplicities& thePowers,
			ListBasicObjects<int>&thePowersSigned, Monomial<Integer>& input);
	void GetAlphaMinusNBetaPoly
		(	partFractions& owner,int indexA,
			int indexB, int n, IntegerPoly& output, int theDimension);
	void GetNElongationPolyWithMonomialContribution
		(	partFractions& owner, ListBasicObjects<int>& theSelectedIndices,
			ListBasicObjects<int>& theCoefficients,
			ListBasicObjects<int>& theGreatestElongations,
			int theIndex,// int theIndexBaseElongation, int lengthGeometricSeries,
			IntegerPoly& output, int theDimension);
	void GetNElongationPoly
		(	partFractions& owner, int index,int baseElongation,
			int LengthOfGeometricSeries, IntegerPoly& output, int theDimension);
	static void GetNElongationPoly
		(intRoot& exponent, int n, IntegerPoly& output, int theDimension);
	void GetNElongationPoly
		(	partFractions& owner, int index,int baseElongation,int LengthOfGeometricSeries,
			PolyPartFractionNumerator& output, int theDimension);
	int GetNumProportionalVectorsClassicalRootSystems(partFractions& owner);
	bool operator==(partFraction& right);
	void operator=(const partFraction& right);
	void initFromRootSystem
		(	partFractions& owner, intRoots& theFraction, intRoots& theAlgorithmBasis,
			intRoot* weights);
	int ElementToString
		(	partFractions& owner, std::string& output, bool LatexFormat,
			bool includeVPsummand, bool includeNumerator,
			GlobalVariables& theGlobalVariables);
	int ElementToStringBasisChange
		(	partFractions& owner, MatrixIntTightMemoryFit& VarChange,
			bool UsingVarChange, std::string& output,
			bool LatexFormat,bool includeVPsummand,bool includeNumerator,
			GlobalVariables& theGlobalVariables);
	void ReadFromFile
		(	partFractions& owner,std::fstream &input, GlobalVariables&  theGlobalVariables,
			int theDimension);
	void WriteToFile(std::fstream& output, GlobalVariables&  theGlobalVariables);
	int GetNumMonomialsInNumerator();
	int SizeWithoutDebugString();
};

class partFractions: public HashedListBasicObjects<partFraction>
{ bool ShouldIgnore(GlobalVariables& theGlobalVariables);
	void AssureIndicatorRegularity(GlobalVariables& theGlobalVariables);
public:
	int AmbientDimension;
	int IndexLowestNonProcessed;
	int IndexCurrentlyProcessed;
	int HighestIndex;
	int NumberIrrelevantFractions;
	int NumberRelevantReducedFractions;
	int NumMonomialsInTheNumerators;
	int NumGeneratorsInTheNumerators;
	int NumRelevantNonReducedFractions;
	//int NumRelevantMonomialsInNonReducedFractions;
	//int NumRelevantGeneratorsInNonReducedFractions;
	static int NumMonomialsInNumeratorsRelevantFractions;
	int NumGeneratorsRelevenatFractions;
	int NumMonomialsInNumeratorsIrrelevantFractions;
	int NumGeneratorsIrrelevantFractions;
	int NumTotalReduced;
	int NumProcessedForVPFfractions;
	int NumRunsReduceMonomialByMonomial;
	//GlobalVariables theGlobalVariables;
	RootToIndexTable RootsToIndices;
	static int NumProcessedForVPFMonomialsTotal;
	static std::fstream ComputedContributionsList;
	static const int MaxReadFileBufferSize= 33554432; //= 32 MB of read buffer size
	std::string DebugString;
	root IndicatorRoot;
	Rational StartCheckSum;
	Rational EndCheckSum;
	static Rational CheckSum;
	bool flagUsingIndicatorRoot;
	bool flagDiscardingFractions;
	static	bool flagSplitTestModeNoNumerators;
	static	bool flagAnErrorHasOccurredTimeToPanic;
	static	bool flagMakingProgressReport;
	static	bool flagUsingCheckSum;
	static int flagMaxNumStringOutputLines;
	bool flagUsingOrlikSolomonBasis;
	void PrepareCheckSums(GlobalVariables& theGlobalVariables);
	void CompareCheckSums(GlobalVariables& theGlobalVariables);
	void ComputeDebugString(GlobalVariables& theGlobalVariables);
	void ComputeDebugStringNoNumerator(GlobalVariables& theGlobalVariables);
	void ComputeDebugStringWithVPfunction(GlobalVariables& theGlobalVariables);
	void ComputeDebugStringBasisChange
		(MatrixIntTightMemoryFit& VarChange,GlobalVariables& theGlobalVariables);
	void initFromRootSystem
		(	intRoots& theFraction, intRoots& theAlgorithmBasis,
			intRoot* weights, GlobalVariables& theGlobalVariables);
	//row index is the index of the root; column(second) index is the coordinate index
	void RemoveRedundantShortRootsClassicalRootSystem(GlobalVariables& theGlobalVariables);
	void RemoveRedundantShortRoots(GlobalVariables& theGlobalVariables);
	bool splitClassicalRootSystem(bool ShouldElongate, GlobalVariables& theGlobalVariables);
	bool split(GlobalVariables& theGlobalVariables);
	void ComputeSupport(ListBasicObjects<roots>& output, std::stringstream& outputString);
	void ComputeOneCheckSum(Rational& output, GlobalVariables& theGlobalVariables);
	void AccountPartFractionInternals(int sign, int index, GlobalVariables& theGlobalVariables);
	void AddAndReduce(partFraction& f, GlobalVariables& theGlobalVariables);
	void AddAlreadyReduced(partFraction& f, GlobalVariables& theGlobalVariables);
	void PopIndexHashAndAccount(int index, GlobalVariables& theGlobalVariables);
	void PrepareIndicatorVariables();
	void initFromOtherPartFractions(partFractions& input, GlobalVariables& theGlobalVariables);
	void IncreaseHighestIndex(int increment);
	void ElementToString(std::string& output, GlobalVariables& theGlobalVariables);
	int ElementToString(std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator,
											GlobalVariables& theGlobalVariables);
	int ElementToStringBasisChange(	MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::string& output,
																	bool LatexFormat, bool includeVPsummand,
																	bool includeNumerator, GlobalVariables& theGlobalVariables);
	int ElementToStringOutputToFile
		(	std::fstream& output, bool LatexFormat, bool includeVPsummand,
			bool includeNumerator, GlobalVariables& theGlobalVariables);
	int ElementToStringBasisChangeOutputToFile
		(	MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::fstream& output,
			bool LatexFormat, bool includeVPsummand, bool includeNumerator,
			GlobalVariables& theGlobalVariables);
	bool partFractionsToPartitionFunctionAdaptedToRoot
		(	QuasiPolynomial& output, root& r,
			//bool storeComputations, bool RecordSplitPowerSeriesCoefficient,
			bool StoreToFile,bool UseOldData,
			GlobalVariables& theGlobalVariables);
	bool VerifyFileComputedContributions(GlobalVariables&  theGlobalVariables);
	void WriteToFileComputedContributions(std::fstream& output, GlobalVariables&  theGlobalVariables);
	int ReadFromFileComputedContributions(std::fstream& input, GlobalVariables&  theGlobalVariables);
	void WriteToFile(std::fstream& output, GlobalVariables&  theGlobalVariables);
	void ReadFromFile(std::fstream& input, GlobalVariables&  theGlobalVariables);
	void UncoverBracketsNumerators(GlobalVariables& theGlobalVariables);
	void ResetRelevanceIsComputed(){for (int i=0;i<this->size;i++){this->TheObjects[i].RelevanceIsComputed=false;};};
	partFractions();
	int SizeWithoutDebugString();
	bool CheckForMinimalityDecompositionWithRespectToRoot(root& r, GlobalVariables& theGlobalVariables);
	void MakeProgressReportSplittingMainPart(FeedDataToIndicatorWindow reportFunction);
	void MakeProgressReportRemovingRedundantRoots(FeedDataToIndicatorWindow reportFunction);
	void MakeProgressReportUncoveringBrackets(FeedDataToIndicatorWindow reportFunction);
	void MakeProgressVPFcomputation(FeedDataToIndicatorWindow reportFunction);
	void ComputeKostantFunctionFromWeylGroup
				(	char WeylGroupLetter, int WeylGroupNumber,
					QuasiPolynomial& output, root* ChamberIndicator,bool UseOldData,
					bool StoreToFile, GlobalVariables&  theGlobalVariables);
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
	MatrixIntTightMemoryFit KillingFormMatrix;
	root rho;
	hashedRoots RootSystem;
	roots RootsOfBorel;
	static bool flagAnErrorHasOcurredTimeToPanic;
	void Assign(const WeylGroup &right);
	void ComputeRho();
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void MakeArbitrary(char WeylGroupLetter,int n);
	void MakeAn(int n);
	void MakeEn(int n);
	void MakeBn(int n);
	void MakeCn(int n);
	void MakeDn(int n);
	void MakeF4();
	void MakeG2();
	void GetEpsilonCoords
		(	char WeylLetter, int WeylRank, roots& simpleBasis, root& input,
			root& output, GlobalVariables& theGlobalVariables);
	void GetEpsilonMatrix
		(	char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables,
			MatrixLargeRational& output);
	void ComputeWeylGroup();
	void ComputeWeylGroup(int UpperLimitNumElements);
	void ComputeWeylGroupAndRootsOfBorel(roots& output);
	void ComputeRootsOfBorel(roots& output);
	void GenerateOrbitAlg(root& ChamberIndicator,
												PolynomialsRationalCoeff& input,
												PolynomialsRationalCoeffCollection& output,
												bool RhoAction, bool PositiveWeightsOnly,
												Cone* LimitingCone, bool onlyLowerWeights);
	void GenerateOrbit(	roots& theRoots, bool RhoAction,
											hashedRoots& output, bool UseMinusRho);
	void GenerateOrbit
    (	roots& theRoots, bool RhoAction, hashedRoots& output,
      bool ComputingAnOrbitGeneratingSubsetOfTheGroup,
			WeylGroup& outputSubset, bool UseMinusRho, int UpperLimitNumElements);
	void GenerateRootSystemFromKillingFormMatrix();
	void ActOnAffineHyperplaneByGroupElement
		(	int index, affineHyperplane& output, bool RhoAction, bool UseMinusRho);
	//theRoot is a list of the simple coordinates of the root
	//theRoot serves as both input and output
	void ActOnRootAlgByGroupElement(
							int index, PolynomialsRationalCoeff& theRoot,
							bool RhoAction);
	void ActOnRootByGroupElement(int index, root& theRoot,
																	bool RhoAction, bool UseMinusRho);
	void ActOnDualSpaceElementByGroupElement(int index, root& theDualSpaceElement,
																	bool RhoAction);
	void SimpleReflectionRoot(int index, root& theRoot, bool RhoAction, bool UseMinusRho);
	void SimpleReflectionDualSpace(int index, root& DualSpaceElement);
	void SimpleReflectionRootAlg
						(	int index, PolynomialsRationalCoeff& theRoot,
							bool RhoAction);
	void ReflectBetaWRTAlpha(root& alpha, root &Beta, bool RhoAction, root& Output);
	void RootScalarKillingFormMatrixRoot(root&r1, root& r2, Rational& output);
	void TransformToSimpleBasisGenerators(roots& theGens);
	int length(int index);
};

class ReflectionSubgroupWeylGroup: public HashedListBasicObjects<ElementWeylGroup>
{
public:
	bool truncated;
	WeylGroup AmbientWeyl;
	WeylGroup Elements;
	roots simpleGenerators;
	rootsCollection ExternalAutomorphisms;
	hashedRoots RootSubsystem;
	std::string DebugString;
	void ComputeDebugString(){this->ElementToString(DebugString);};
	void ElementToString(std::string& output);
	void ComputeSubGroupFromGeneratingReflections
		(	roots& generators,rootsCollection& ExternalAutos,
			GlobalVariables& theGlobalVariables, int UpperLimitNumElements,
			bool recomputeAmbientRho);
	void ComputeRootSubsystem();
	void ActByElement(int index, root& theRoot);
};

class rootFKFTcomputation
{
public:
	intRoots nilradicalA2A1A1inD5;
	intRoots AlgorithmBasisA2A1A1inD5;
	intRoot weights;
	GlobalVariables* TheGlobalVariables;
	partFractions partitionA2A1A1inD5;
	std::string OutputFile;
	bool useOutputFileForFinalAnswer;
//	bool useOldKLData;
	bool useOldPartialFractionDecompositionData;
	bool useVPFstoredData;
	std::string KLCoeffFileString;
	std::string PartialFractionsFileString;
	std::string VPEntriesFileString;
	std::string VPIndexFileString;
	rootFKFTcomputation();
	~rootFKFTcomputation();
	static bool OpenDataFile
			(std::fstream& theFileOutput, std::string& theFileName);
	static bool OpenDataFileOrCreateIfNotPresent2
			(std::fstream& theFile, std::string& theFileName, bool OpenInAppendMode, bool openAsBinary);
	void MakeRootFKFTsub(root& direction, QPSub& theSub);
	void initA2A1A1inD5();
  void RunA2A1A1inD5beta12221();
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
	void MakeTheRootFKFTSum
		(	root& ChamberIndicator, partFractions& theBVdecomposition,
			ListBasicObjects<int>& theKLCoeffs,	QuasiPolynomial& output,
			VermaModulesWithMultiplicities& theHighestWeights,
			roots& theNilradical);
};

class LaTeXProcedures
{
public:
	static const int ScaleFactor=40;
	static const int FigureSizeX= 10;//in centimeters
	static const int FigureSizeY=10;//in centimeters
	static const int FigureCenterCoordSystemX= 4;//in centimeters
	static const int FigureCenterCoordSystemY=8;//in centimeters
	static const int TextPrintCenteringAdjustmentX=3;
	static const int TextPrintCenteringAdjustmentY=3;
	static void drawline(	double X1, double Y1, double X2, double Y2,
									unsigned long thePenStyle, int ColorIndex, std::fstream& output);
	static void drawText(	double X1, double Y1, std::string& theText, int ColorIndex, std::fstream& output);
	static void beginDocument(std::fstream& output);
	static void endLatexDocument(std::fstream& output);
	static void beginPSTricks(std::fstream& output);
	static void endPSTricks(std::fstream& output);
	static void GetStringFromColorIndex(int ColorIndex, std::string &output);
};

class thePFcomputation
{
public:
	int LCMdeterminants;
	int TotalNumEnumerated;
	ListBasicObjects<Selection> tableForbidden;
	ListBasicObjects<Selection> theForbiddenSelections;
	void ComputeDeterminantSelection(int theDimension);
	Selection theSelection;
	roots AllRoots;
	void EnumerateRecursively(int depth, int startingIndex, int theDimension);
	std::string DebugString;
	void ComputeDebugString();
	void ElementToString(std::string& output);
	void SelectionToMatrixRational(MatrixLargeRational& output, int theDimension);
	void SelectionToString(std::string& output, int theDimension);
	void ComputeTableAllowed();
//	bool CheckAvailabilityIndex(int depth, int index);
	WeylGroup theWeylGroup;
	MatrixLargeRational theKillingForm;
	void ComputeNewRestrictionsFromOld(int depth, int newIndex);
	void Run();
};

class multTableKmods : public ListBasicObjects<ListBasicObjects <ListBasicObjects<int> > >
{
public:
	std::string DebugString;
	void ElementToString(std::string& output, rootSubalgebra& owner);
	void ElementToString(std::string& output, bool useLaTeX, bool useHtml, rootSubalgebra& owner);
	void ComputeDebugString(rootSubalgebra& owner)
	{	this->ElementToString(this->DebugString,owner);
	};
};

class DynkinDiagramRootSubalgebra
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(this->DebugString);};
	rootsCollection SimpleBasesConnectedComponents;
	ListBasicObjects<std::string> DynkinTypeStrings;
	ListBasicObjects<int> indicesThreeNodes;
	ListBasicObjects<ListBasicObjects< int > > indicesEnds;

	ListBasicObjects<ListBasicObjects< int > > sameTypeComponents;
	ListBasicObjects<int> indexUniComponent;
	ListBasicObjects<int> indexInUniComponent;
	int RankTotal();
	void Sort();
	void ComputeDiagramType(roots& simpleBasisInput, WeylGroup& theWeyl);
	void ComputeDynkinStrings(WeylGroup& theWeyl);
	void ComputeDynkinString(int indexComponent, WeylGroup& theWeyl);
	int numberOfThreeValencyNodes(int indexComponent, WeylGroup& theWeyl);
	void Assign(const DynkinDiagramRootSubalgebra& right);
	inline void operator=(const DynkinDiagramRootSubalgebra& right){this->Assign(right);};
	bool operator==(const DynkinDiagramRootSubalgebra& right);
	bool IsGreaterThan(DynkinDiagramRootSubalgebra& right);
	void GetAutomorphism(ListBasicObjects<ListBasicObjects<int> > & output,int index);
	void GetAutomorphisms
		(ListBasicObjects<ListBasicObjects<ListBasicObjects<int> > > & output);
	void GetMapFromPermutation
		(	roots& domain, roots& range, ListBasicObjects< int >& thePerm,
			ListBasicObjects< ListBasicObjects< ListBasicObjects< int > > >& theAutos,
			SelectionWithDifferentMaxMultiplicities& theAutosPerm,
			DynkinDiagramRootSubalgebra& right);
};

class coneRelation
{
public:
	roots Alphas;
	roots Betas;
	ListBasicObjects<Rational> AlphaCoeffs;
	ListBasicObjects<Rational> BetaCoeffs;
	ListBasicObjects<ListBasicObjects<int> > AlphaKComponents;
	ListBasicObjects<ListBasicObjects<int> > BetaKComponents;
	int IndexOwnerRootSubalgebra;
	bool GenerateAutomorphisms(coneRelation& right,rootSubalgebras& owners);
	::DynkinDiagramRootSubalgebra theDiagram;
	::DynkinDiagramRootSubalgebra theDiagramRelAndK;
	std::string DebugString;
	std::string stringConnectedComponents;
	void ComputeDiagramRelAndK(rootSubalgebra& owner);
	void FixRepeatingRoots( roots& theRoots, ListBasicObjects<Rational>& coeffs);
	void RelationOneSideToString
    ( std::string& output, const std::string& letterType,
			ListBasicObjects<Rational>& coeffs,
      ListBasicObjects<ListBasicObjects<int> >& kComponents, roots& theRoots,
			bool useLatex, rootSubalgebra& owner);
	void GetEpsilonCoords
		(	roots& input, roots& output, WeylGroup& theWeyl,
			GlobalVariables& theGlobalVariables);
	int ElementToString
		(	std::string &output, rootSubalgebras& owners, bool useLatex,
			bool includeScalarsProductsEachSide, bool includeMixedScalarProducts );
	int RootsToScalarProductString
		(	roots& inputLeft, roots& inputRight,const std::string& letterTypeLeft,
			const std::string& letterTypeRight,
			std::string& output, bool useLatex,
			rootSubalgebra& owner);
	void ComputeConnectedComponents
		(roots& input, rootSubalgebra& owner, ListBasicObjects<ListBasicObjects<int> >& output);
	void ComputeDebugString
		(	rootSubalgebras& owner, bool includeScalarsProducts,
			bool includeMixedScalarProducts)
	{	this->ElementToString
			(this->DebugString,owner,true,includeScalarsProducts,includeMixedScalarProducts);
	};
	void MakeLookCivilized(rootSubalgebra& owner, roots& NilradicalRoots);
	bool IsStrictlyWeaklyProhibiting
		(	rootSubalgebra& owner, roots& NilradicalRoots, GlobalVariables& theGlobalVariables,
			rootSubalgebras& owners, int indexInOwner);
	void FixRightHandSide(rootSubalgebra& owner, roots& NilradicalRoots);
	bool leftSortedBiggerThanOrEqualToRight
		(ListBasicObjects<int>& left,ListBasicObjects<int>& right);
	void ComputeKComponents
		(	roots& input, ListBasicObjects<ListBasicObjects<int> >& output,
			rootSubalgebra& owner);
	void RelationOneSideToStringCoordForm
		(std::string& output,	ListBasicObjects<Rational>& coeffs,	roots& theRoots, bool EpsilonForm);
	void GetSumAlphas(root& output, int theDimension);
	bool CheckForBugs(rootSubalgebra& owner, roots& NilradicalRoots);
	void SortRelation(rootSubalgebra& owner);
	void operator=(const coneRelation& right)
	{ this->Alphas.CopyFromBase(right.Alphas);
		this->Betas.CopyFromBase(right.Betas);
		this->AlphaCoeffs.CopyFromBase(right.AlphaCoeffs);
		this->BetaCoeffs.CopyFromBase(right.BetaCoeffs);
		this->AlphaKComponents.CopyFromBase(right.AlphaKComponents);
		this->BetaKComponents.CopyFromBase(right.BetaKComponents);
		this->theDiagram=right.theDiagram;
		this->theDiagramRelAndK=right.theDiagramRelAndK;
		this->IndexOwnerRootSubalgebra=right.IndexOwnerRootSubalgebra;
		this->DebugString= right.DebugString;
	};
	bool operator==(const coneRelation& right)
	{ return this->DebugString==right.DebugString;
	};
	int HashFunction()
	{ int tempI= ::MathRoutines::Minimum(this->DebugString.length(),::SomeRandomPrimesSize);
		int result=0;
		for (int i=0;i<tempI;i++)
			result+= this->DebugString[i]*::SomeRandomPrimes[i];
		return result;
	};
	coneRelation(){this->IndexOwnerRootSubalgebra=-1;};
};

class coneRelations: public HashedListBasicObjects<coneRelation>
{
public:
	int NumAllowedLatexLines;
	bool flagIncludeSmallerRelations;
	bool flagIncludeCoordinateRepresentation;
	bool flagIncludeSubalgebraDataInDebugString;
	std::string DebugString;
	ListBasicObjects<std::string> CoordinateReps;
	void GetLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter);
	void ElementToString
		(	std::string& output, rootSubalgebras& owners, bool useLatex, bool useHtml,
			std::string* htmlPathPhysical, std::string* htmlPathServer, GlobalVariables& theGlobalVariables);
	void ComputeDebugString
		(	rootSubalgebras& owners,std::string* htmlPathPhysical,
			std::string* htmlPathServer, GlobalVariables& theGlobalVariables)
	{	this->ElementToString
			(this->DebugString,owners,true,false, htmlPathPhysical,htmlPathServer,theGlobalVariables);
	};
	void ComputeDebugString
    (	rootSubalgebras& owners,GlobalVariables& theGlobalVariables)
  { this->ComputeDebugString(owners,0,0,theGlobalVariables);
  };
	void AddRelationNoRepetition
		(coneRelation& input, rootSubalgebras& owners, int indexInRootSubalgebras);
	coneRelations()
	{	this->NumAllowedLatexLines=44;
		this->flagIncludeSmallerRelations=true;
		this->flagIncludeCoordinateRepresentation=false;
		this->flagIncludeSubalgebraDataInDebugString=false;
	};
};

class permutation: public SelectionWithDifferentMaxMultiplicities
{
public:
  void initPermutation(int n);
  void initPermutation(ListBasicObjects<int>& disjointSubsets, int TotalNumElements);
  void incrementAndGetPermutation(ListBasicObjects<int>& output);
  void GetPermutation(ListBasicObjects<int>& output);
};

class rootSubalgebra
{
public:
	int NumNilradicalsAllowed;
	int NumConeConditionFailures;
	int NumRelationsWithStronglyPerpendicularDecomposition;
	//int NumRelationsgreaterLengthThan2;
	int NumGmodKtableRowsAllowedLatex;
	int NumTotalSubalgebras;
	bool flagFirstRoundCounting;
	bool flagComputeConeCondition;
	bool flagMakingProgressReport;
	bool flagAnErrorHasOccuredTimeToPanic;
	static int ProblemCounter;
	static int ProblemCounter2;
	ListBasicObjects<int> indicesSubalgebrasContainingK;
	::multTableKmods theMultTable;
	ListBasicObjects<int> theOppositeKmods;
	DynkinDiagramRootSubalgebra theDynkinDiagram;
	DynkinDiagramRootSubalgebra theCentralizerDiagram;
	ListBasicObjects< ListBasicObjects<int> > coneRelationsBuffer;
	ListBasicObjects< int> coneRelationsNumSameTypeComponentsTaken;
	ListBasicObjects<DynkinDiagramRootSubalgebra> relationsDiagrams;
	WeylGroup AmbientWeyl;
	roots genK;
	roots SimpleBasisK;
	roots PositiveRootsK;
	roots AllRootsK;
	Selection NilradicalKmods;
	Selection CentralizerKmods;
	roots LowestWeightsGmodK;
	roots HighestWeightsGmodK;
	roots HighestRootsK;
	roots TestedRootsAlpha;
	roots CentralizerRoots;
	roots SimpleBasisCentralizerRoots;
	ListBasicObjects<roots> kModules;
	ListBasicObjects<roots> PosRootsKConnectedComponents;
	ListBasicObjects<Selection> theKEnumerations;
	ListBasicObjects<int> theKComponentRanks;
	std::string DebugString;
	rootSubalgebra();
	//returns -1 if the weight/root is not in g/k
	int GetIndexKmoduleContainingRoot(root& input);
	bool IsGeneratingSingularVectors(int indexKmod, roots& NilradicalRoots);
	bool rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, root& input);
	bool AttemptTheTripleTrick
		(coneRelation& theRel, roots& NilradicalRoots, GlobalVariables& theGlobalVariables);
	bool AttemptTheTripleTrickWRTSubalgebra
		(	coneRelation& theRel, roots& highestWeightsAllowed,
			roots& NilradicalRoots, GlobalVariables& theGlobalVariables);
	void ExtractRelations
		(	MatrixLargeRational& matA,MatrixLargeRational& matX,
			roots& NilradicalRoots, rootSubalgebras& owner, int indexInOwner,
			GlobalVariables& theGlobalVariables,roots& Ksingular);
  bool GenerateAutomorphisms
		(	rootSubalgebra& right, GlobalVariables& theGlobalVariables,
			ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly);
	void MakeGeneratingSingularVectors
		(coneRelation &theRelation, roots& nilradicalRoots);
  bool attemptExtensionToIsomorphism
		( roots& Domain, roots& Range, GlobalVariables& theGlobalVariables,
			int RecursionDepth, ReflectionSubgroupWeylGroup* outputAutomorphisms,
			bool GenerateAllpossibleExtensions);
	bool CheckForSmallRelations(coneRelation& theRel,roots& nilradicalRoots);
	int NumRootsInNilradical();
	void MakeSureAlphasDontSumToRoot(coneRelation& theRel, roots& NilradicalRoots);
	bool IsARoot(root& input);
	bool IsARootOrZero(root& input);
	void KEnumerationsToLinComb(GlobalVariables& theGlobalVariables);
	void DoKRootsEnumeration(GlobalVariables& theGlobalVariables);
	void ComputeCentralizerFromKModulesAndSortKModules();
	void MatrixToRelation
		(	coneRelation& output, MatrixLargeRational& matA, MatrixLargeRational& matX,
			int theDimension, roots& NilradicalRoots);
	void GenerateParabolicsInCentralizerAndPossibleNilradicals
		(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner);
	void DoKRootsEnumerationRecursively
		(int indexEnumeration, GlobalVariables& theGlobalVariables);
	void MakeProgressReportPossibleNilradicalComputation
		(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner);
	void MakeProgressReportGenAutos
		(int progress,int outOf,int found, GlobalVariables& theGlobalVariables);
	void ComputeDebugString(GlobalVariables& theGlobalVariables);
	void ComputeDebugString(bool makeALaTeXReport, bool useHtml,GlobalVariables& theGlobalVariables)
	{this->ElementToString(this->DebugString,makeALaTeXReport,useHtml,theGlobalVariables);};
	bool IndexIsCompatibleWithPrevious
		(	int startIndex, int RecursionDepth,	multTableKmods &multTable,
			ListBasicObjects<Selection>& impliedSelections,
			ListBasicObjects<int> &oppositeKmods, rootSubalgebras& owner, GlobalVariables& theGlobalVariables);
	bool IsAnIsomorphism
		(	roots& domain, roots& range, GlobalVariables& theGlobalVariables,
			ReflectionSubgroupWeylGroup* outputAutomorphisms);
//	void GeneratePossibleNilradicals(GlobalVariables& theGlobalVariables);
	bool ListHasNonSelectedIndexLowerThanGiven
		(int index,ListBasicObjects<int>& tempList, Selection& tempSel);
	void GeneratePossibleNilradicalsRecursive
		(	GlobalVariables& theGlobalVariables,
			multTableKmods & multTable,	int StartIndex,
			ListBasicObjects<Selection>& impliedSelections,
			ListBasicObjects<int>& oppositeKmods, rootSubalgebras& owner, int indexInOwner);
	bool ConeConditionHolds
		(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner);
	bool ConeConditionHolds
		(	GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner,
			roots& NilradicalRoots, roots& Ksingular, bool doExtractRelations);
	void PossibleNilradicalComputation
		(	GlobalVariables& theGlobalVariables,Selection& selKmods,
			rootSubalgebras& owner, int indexInOwner);
	void ElementToStringHeaderFooter
		(std::string& outputHeader,std::string&  outputFooter, bool useLatex, bool useHtml);
	void ElementToString(std::string& output,GlobalVariables& theGlobalVariables)
	{this->ElementToString(output,false,false,theGlobalVariables);};
	void ElementToHtml(int index, std::string& path, GlobalVariables& theGlobalVariables);
	void ElementToString
		(	std::string& output, bool makeALaTeXReport, bool useHtml,
			GlobalVariables& theGlobalVariables);
	bool RootsDefineASubalgebra(roots& theRoots);
	void GenerateKmodMultTable
		(	ListBasicObjects<ListBasicObjects< ListBasicObjects<int> > > & output,
			ListBasicObjects<int>& oppositeKmods,
			GlobalVariables& theGlobalVariables);
	void KmodTimesKmod
		(	int index1, int index2,ListBasicObjects<int>& oppositeKmods,
			ListBasicObjects<int> & output);
	void initFromAmbientWeyl();
	void ComputeAllButAmbientWeyl();
	void ComputeAll();
	void ComputeRootsOfK();
	void ComputeKModules();
	void ComputeHighestWeightInTheSameKMod(root& input, root& outputHW);
	void ComputeExtremeWeightInTheSameKMod
		(root& input, root& outputW, bool lookingForHighest);
	inline void operator=(const rootSubalgebra& right);
	void Assign(const rootSubalgebra& right);
	void ComputeLowestWeightInTheSameKMod(root& input, root& outputLW);
	void GetLinearCombinationFromMaxRankRootsAndExtraRoot
		(bool DoEnumeration, GlobalVariables& theGlobalVariables);
//	void commonCodeForGetLinearCombinationFromMaxRankRootsAndExtraRoot();
	void GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2
		(GlobalVariables& theGlobalVariables);
	bool LinCombToString(root& alphaRoot, int coeff, root& linComb,std::string& output);
	bool LinCombToStringDistinguishedIndex
		(	int distinguished,root& alphaRoot, int coeff, root &linComb,
			std::string &output);
};

class rootSubalgebras: public ListBasicObjects<rootSubalgebra>
{
public:
	std::string DebugString;
	coneRelations theBadRelations;
	coneRelations theGoodRelations;
	coneRelations theMinRels;
	ListBasicObjects< ListBasicObjects <int> > ActionsNormalizerCentralizerNilradical;
	//ListBasicObjects< ListBasicObjects <int> > OrbitsUnderNormalizerCentralizerNilradical;
	int NumSubalgebrasProcessed;
	int NumConeConditionFailures;
	int NumSubalgebrasCounted;
	int NumLinesPerTableLatex;
	int NumColsPerTableLatex;
	int UpperLimitNumElementsWeyl;
	ListBasicObjects<std::string> theBadDiagrams;
	ListBasicObjects<int> numFoundBadDiagrams;
	WeylGroup AmbientWeyl;
	bool flagUseDynkinClassificationForIsomorphismComputation;
	bool flagUsingActionsNormalizerCentralizerNilradical;
	bool flagComputeConeCondition;
	bool flagLookingForMinimalRels;
	void ComputeKmodMultTables(GlobalVariables& theGlobalVariables);
	bool ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical
		(	Selection& targetSel,
			GlobalVariables& theGlobalVariables);
	bool ApproveSelAgainstOneGenerator
		(	ListBasicObjects<int>& generator,
			Selection& targetSel, GlobalVariables& theGlobalVariables);
	void RaiseSelectionUntilApproval
		(	Selection& targetSel, GlobalVariables& theGlobalVariables);
	void ApplyOneGenerator
		(	ListBasicObjects<int>& generator, Selection& targetSel,
			GlobalVariables& theGlobalVariables);
	void ComputeActionNormalizerOfCentralizerIntersectNilradical
		(	Selection& SelectedBasisRoots, rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables);
	void GenerateAllRootSubalgebrasUpToIsomorphism
			(GlobalVariables& theGlobalVariables, char WeylLetter, int WeylRank);
	bool IsANewSubalgebra(rootSubalgebra& input, GlobalVariables& theGlobalVariables);
	int IndexSubalgebra(rootSubalgebra& input, GlobalVariables& theGlobalVariables);
	void GenerateAllRootSubalgebrasContainingInputUpToIsomorphism
		(rootSubalgebras& bufferSAs, int RecursionDepth, GlobalVariables &theGlobalVariables);
	void DynkinTableToString
		(	bool useLatex, bool useHtml,std::string* htmlPathPhysical,
			std::string* htmlPathServer,std::string& output);
	void GetTableHeaderAndFooter
		(	std::string& outputHeader,std::string& outputFooter, bool useLatex,
			bool useHtml);
	void SortDescendingOrderBySSRank();
	void initDynkinDiagramsNonDecided
		(WeylGroup& theWeylGroup, char WeylLetter, int WeylRank);
	void pathToHtmlFileNameElements
		(int index, std::string* htmlPathServer, std::string& output, bool includeDotHtml);
	void pathToHtmlReference
		(int index,std::string& DisplayString, std::string* htmlPathServer, std::string& output);
	void ElementToHtml
		( std::string& header,	std::string& pathPhysical,std::string& htmlPathServer,
			GlobalVariables& theGlobalVariables);
	void ElementToString
		(	std::string& output, bool useLatex, bool useHtml, std::string* htmlPathPhysical,
			std::string* htmlPathServer, GlobalVariables& theGlobalVariables);
	void ComputeLProhibitingRelations
		(GlobalVariables& theGlobalVariables, int StartingIndex, int NumToBeProcessed);
	void ComputeDebugString
		(	bool useLatex, bool useHtml, std::string* htmlPathPhysical,
			std::string* htmlPathServer, GlobalVariables& theGlobalVariables)
	{	this->ElementToString
			(this->DebugString,useLatex, useHtml,htmlPathPhysical,htmlPathServer,theGlobalVariables);
	};
	void MakeProgressReportGenerationSubalgebras
		(	rootSubalgebras& bufferSAs, int RecursionDepth,GlobalVariables &theGlobalVariables,
			int currentIndex, int TotalIndex);
	void MakeProgressReportAutomorphisms
		(	ReflectionSubgroupWeylGroup& theSubgroup, rootSubalgebra& theRootSA,
			GlobalVariables& theGlobalVariables);
	rootSubalgebras()
	{	this->flagUseDynkinClassificationForIsomorphismComputation=false;
		this->flagComputeConeCondition=true;
		this->flagUsingActionsNormalizerCentralizerNilradical=true;
		this->flagLookingForMinimalRels=false;
		this->NumLinesPerTableLatex=20;
		this->NumColsPerTableLatex=4;
		this->UpperLimitNumElementsWeyl=10000;
	};
};

struct IndicatorWindowVariables
{
public:
	bool Busy;
	int NumProcessedMonomialsCurrentFraction;
	int NumProcessedMonomialsTotal;

	intRoot modifiedRoot;
	bool flagRootIsModified;
	bool Pause;
	IndicatorWindowVariables(){this->Nullify();}
	std::string ProgressReportString1;
	std::string ProgressReportString2;
	std::string ProgressReportString3;
	std::string ProgressReportString4;
	std::string ProgressReportString5;
	void Assign(IndicatorWindowVariables& right);
	void Nullify()
	{ this->Busy=false;
		this->Pause=true;
		this->NumProcessedMonomialsCurrentFraction=0;
		this->NumProcessedMonomialsTotal=0;
	};
};

struct ComputationSetup
{
	roots InputRoots;
public:
	partFractions thePartialFraction;
	QuasiPolynomial theOutput;
	rootSubalgebras theRootSubalgebras;
	CombinatorialChamberContainer theChambers;
	Rational Value;
	std::string ValueString;
	std::string NotationExplanationLatex1;
	std::string NotationExplanationLatex2;
	std::string NotationExplanationLatex3;
	std::string NotationExplanationLatex4;
	intRoot ValueRoot;
	int NextDirectionIndex;
	roots VPVectors;
	GlobalVariablesContainer *theGlobalVariablesContainer;
	bool flagAllowRepaint;
	bool flagDoCustomComputation;
	bool flagComputationInitialized;
	bool flagComputationInProgress;
	bool flagComputationDone;
	bool flagOneStepOnly;
	bool flagUseHtml;
	bool flagOneIncrementOnly;
	bool flagFullChop;
	bool flagUsingCustomVectors;
	bool flagComputingPartialFractions;
	bool flagDoneComputingPartialFractions;
	bool flagComputingVectorPartitions;
	bool flagComputingChambers;
	bool flagDoingWeylGroupAction;
	bool flagHavingStartingExpression;
	bool flagDisplayingCombinatorialChambersTextData;
	bool flagHavingBeginEqnForLaTeXinStrings;
	bool flagHavingDocumentClassForLaTeX;
	bool flagDisplayingPartialFractions;
	bool flagComputationIsDoneStepwise;
	//bool flagAffineComputationDone;
	bool flagSuperimposingComplexes;
	bool flagCustomNilradicalInitted;
	bool flagDoCustomNilradical;
	bool flagSliceTheEuclideanSpaceInitialized;
	bool flagOneSteChamberSliceInitialized;
	bool flagPartialFractionSplitPrecomputed;
	char WeylGroupLetter;
	int NumRowsNilradical;
	int NumColsNilradical;
	int WeylGroupIndex;
	int DisplayNumberChamberOfInterest;
	void AdjustGraphicsForTwoDimensionalLieAlgebras(DrawingVariables& theDV);
	void EvaluatePoly();
	void Run();
	void RunCustom();
	int getNextEqualityIndex(std::string& input, int index);
	bool IsAnInteger(char a);
	int GetDigitFromChar(char a);
	int readNextIntData(std::string&input, int index, int& endIndex);
	void InitComputationSetup();
	void ExitComputationSetup();
	void WriteReportToFile(DrawingVariables& TDV, std::fstream &theFile, GlobalVariables &theGlobalVariables);
	void oneStepChamberSlice(GlobalVariables& theGlobalVariables);
	void oneIncrement(GlobalVariables& theGlobalVariables);
	void initWeylActionSpecifics(GlobalVariables& theGlobalVariables);
	void initGenerateWeylAndHyperplanesToSliceWith
		(GlobalVariables& theGlobalVariables,CombinatorialChamberContainer& inputComplex);
	void SetupCustomNilradicalInVPVectors(GlobalVariables& theGlobalVariables);
	void FullChop(GlobalVariables& theGlobalVariables);
	void WriteToFilePFdecomposition(std::fstream& output);
	void Reset();
	void DoTheRootSAComputation();
	void DoTheRootSAComputationCustom();
	ComputationSetup();
	~ComputationSetup();
};

/*class SelectionList: public ListBasicObjects<Selection>
{
public:
	std::string DebugString;
	void ElementToString(std::string& output);
	void ComputeDebugString(){this->ElementToString(DebugString);};
};*/

struct CGIspecificRoutines
{
public:
	static std::stringstream outputStream;
	static int numLines;
	static int numRegularLines;
	static int numDashedLines;
	static int numDottedLines;
	static int shiftX;
	static int shiftY;
	static int scale;
	static void outputLineJavaScriptSpecific
		(	const std::string& lineTypeName, int theDimension, std::string& stringColor,
			int& lineCounter );
  static void MakeVPReportFromComputationSetup(ComputationSetup& input);
	static void PrepareOutputLineJavaScriptSpecific(const std::string& lineTypeName, int numberLines);
	static int ReadDataFromCGIinput(std::string& inputBad, ComputationSetup& output, std::string& thePath);
	static void CivilizedStringTranslation(std::string& input, std::string& output);
	static void MakePFAndChamberReportFromComputationSetup(ComputationSetup& input);
	static void drawlineInOutputStreamBetweenTwoRoots
		(	root& r1, root& r2,	unsigned long thePenStyle,  int r, int g, int b);
  static void rootSubalgebrasToHtml(rootSubalgebras& input, std::fstream& output);
  static void WeylGroupToHtml(WeylGroup&input, std::string& path);
  static void rootSubalgebrasToHtml
    (GlobalVariables& theGlobalVariables,rootSubalgebras& input, std::string& path);
 	static bool OpenDataFileOrCreateIfNotPresent
		(std::fstream& theFile, std::string& theFileName, bool OpenInAppendMode, bool openAsBinary);
	static void clearDollarSigns(std::string& theString, std::string& output);
	static void subEqualitiesWithSimeq(std::string& theString, std::string& output);
	static bool CheckForInputSanity(ComputationSetup& input);

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

class GlobalVariables
{
public:
	roots rootsWallBasis;
	roots rootsIsABogusNeighbor1;
	roots rootsIsABogusNeighbor2;
	roots rootsSplitChamber1;
	roots rootsNilradicalRoots;
	roots rootsConeConditionHolds2;
	roots rootsRootSAIso;
	roots rootsGetCoordsInBasis;
	roots rootsGetEpsilonCoords;
	roots rootsAttemptTheTripleTrick;

	rootsCollection rootsCollectionSplitChamber1;
	rootsCollection rootsCollectionSplitChamber2;
	rootsCollection rootsStronglyPerpendicular;
	rootsCollection rootsAttemptExtensionIso1;
	rootsCollection rootsAttemptExtensionIso2;

	rootSubalgebras rootSAAttemptExtensionIso1;
	rootSubalgebras rootSAAttemptExtensionIso2;
	rootSubalgebras rootSAsGenerateAll;

	hashedRoots hashedRootsComputeSubGroupFromGeneratingReflections;

	ListBasicObjects<CombinatorialChamber*> listCombinatorialChamberPtSplitChamber;
	ListBasicObjects<WallData*> listWallDataPtSplitChamber;

	Monomial<Rational> monMakePolyExponentFromIntRoot;
	Monomial<Rational> monMakePolyFromDirectionAndNormal;

	MatrixLargeRational matTransposeBuffer;
	MatrixLargeRational matComputationBufferLinAlgOneAffinePlane;
	MatrixLargeRational matComputationBufferLinAlgAffinePart;
	MatrixLargeRational matComputeNormalFromSelection;
	MatrixLargeRational matOutputEmpty;
	MatrixLargeRational matIdMatrix;
	MatrixLargeRational	matComputeNormalExcludingIndex;
	MatrixLargeRational matLinearAlgebraForVertexComputation;
	MatrixLargeRational	matComputeNormalFromSelectionAndExtraRoot;
	MatrixLargeRational matComputeNormalFromSelectionAndTwoExtraRoots;
	MatrixLargeRational matGetRankOfSpanOfElements;
	MatrixLargeRational matReduceMonomialByMonomial;
	MatrixLargeRational matReduceMonomialByMonomial2;
	MatrixLargeRational matOneColumn;
	MatrixLargeRational matConeCondition1;
	MatrixLargeRational matConeCondition2;
	MatrixLargeRational matConeCondition3;
	MatrixLargeRational matSimplexAlgorithm1;
	MatrixLargeRational matSimplexAlgorithm2;
	MatrixLargeRational matSimplexAlgorithm3;
	MatrixLargeRational matRootSAIso;
	MatrixLargeRational matGetCoordsInBasis;
	MatrixLargeRational matGetEpsilonCoords;

	partFraction fracReduceMonomialByMonomial;
	partFraction fracSplit1;
	QuasiPolynomial QPComputeQuasiPolynomial;
	QuasiNumber QNComputeQuasiPolynomial;

	IntegerPoly IPRemoveRedundantShortRootsClassicalRootSystem;
	IntegerPoly IPElementToStringBasisChange;
	IntegerPoly IPComputationalBufferAddPartFraction1;
	IntegerPoly IPComputationalBufferAddPartFraction2;
	IntegerPoly IPReduceMonomialByMonomialModifyOneMonomial1;
	IntegerPoly IPReduceMonomialByMonomialModifyOneMonomial2;

	PolyPartFractionNumerator PPFNElementToStringBasisChange;
	PolyPartFractionNumerator PPFNAddPartFraction1;
	PolyPartFractionNumerator PPFNAddPartFraction2;

	Selection selLinearAlgebraForVertexComputation;
	Selection selComputeNormalFromSelection;
	Selection selComputeNormalExcludingIndex;
	Selection selWallSelection;
	Selection selComputeNormalFromSelectionAndExtraRoot;
	Selection selComputeNormalFromSelectionAndTwoExtraRoots;
	Selection selComputeAffineInfinityPointApproximation1;
	Selection selComputeAffineInfinityPointApproximation2;
	Selection selGetRankOfSpanOfElements;
	Selection selReduceMonomialByMonomial;
	Selection selSimplexAlg1;
	Selection selSimplexAlg2;
	Selection selApproveSelAgainstOneGenerator;
	ReflectionSubgroupWeylGroup subGroupActionNormalizerCentralizer;

	HashedListBasicObjects<Selection> hashedSelSimplexAlg;

	GlobalVariables();
	~GlobalVariables();
	static FeedDataToIndicatorWindow FeedDataToIndicatorWindowDefault;
	void operator=(const GlobalVariables& G_V);
};

class GlobalVariablesContainer :public ListBasicObjects<GlobalVariables>
{
public:
	GlobalVariables* Default(){return & this->TheObjects[0];};
};

void ProjectOntoHyperPlane(root& input, root& normal, root& ProjectionDirection, root&output);

//extern GlobalVariablesContainer StaticGlobalVariablesContainer;

#endif
