//*********************************************************************************************************
//*********************************************************************************************************
//Vector partition function - computes an algebraic expression
//                            for the vector partition function
//CopyRight (C) 2009-2011: Todor Milev
//email: todor.milev@gmail.com
//
//Contributors: Thomas Bliem, Todor Milev
//
//This is free software. You are welcome to use, modify and redistribute this code
//and the resulting program any way you wish, as long as you provide the same rights
//as those given to you, to any future recipients of your modifications (in case you
//decide to pass on those modifications).
//The code is licensed under the Library General Public License version 3.0 or a later version of the License at your choice
//(the license terms were briefly summarized in the preceding sentence).
//You should have received a copy of the GNU Library General Public License
//along with this program.
//If not, see <http://www.gnu.org/licenses/>.
//
//Todor Milev would like to thank http://www.cplusplus.com/forum/ for the valuable
//advice and help with C++. Many thanks to helios, Disch, Grey Wolf, jsmith,
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

//the following option turns on a RAM memory limit safeguard, as well as
// very limited memory allocation statistics. See
//ParallelComputing::CheckPointerCounters. The 3 main files should compile and link just fine with the following 3 lines commented out.
//If not, it's a bug.
#ifndef CGIversionLimitRAMuse
#define CGIversionLimitRAMuse
#endif

#include <assert.h>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>

#ifndef WIN32
#include <pthread.h>
#else
 #include<windows.h>
// #include <unistd.h>
// #include <Pthread.h>
#endif

#ifdef WIN32
//have to disable C4100 in VS because it warns me on passing non-used parameters to my functions.
//Those of course are passed to facilitate future extensions of functionality.
#pragma warning(disable:4100)//warning C4100: non-referenced formal parameter
//The below causes problems in VS with my debugging code (which I comment/uncomment often).
#pragma warning(disable:4189)//warning 4189: variable initialized but never used
#endif

const int SomeRandomPrimesSize= 25;
//used for hashing various things.
const int SomeRandomPrimes[SomeRandomPrimesSize]={ 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911};
class CompositeComplexQNSub;
class affineCone;
class affineHyperplane;
class QuasiNumber;
class BasicQN;
class CombinatorialChamber;
class PolynomialsRationalCoeff;
class PrecomputedQuasiPolynomialIntegrals;
class PrecomputedTauknPointersKillOnExit;
class QuasiPolynomialOld;
class VertexSelectionPointers;
class CombinatorialChamberContainer;
class CompositeComplexQN;
template <class Element>
class Polynomial;
template <class Element>
class Monomial;
template <class Element>
class PolynomialLight;
class Selection;
class IntegerPoly;
class Rational;
class LargeInt;
class LargeIntUnsigned;
class rootSubalgebra;
class intRoot;
class PolyPartFractionNumerator;
template <class CoefficientType>
class Vectors;
template <class CoefficientType>
class Vector;
class root;
class roots;
class rootsCollection;
class coneRelation;
template <class Object>
class List;
template <class Object>
class Matrix;
template <class Object>
class ListPointers;
template <class Object>
class HashedList;
class PrecomputedTauknPointers;
struct PolynomialOutputFormat;
class Rational;
class QPSub;
class partFractions;
class SubsetWithMultiplicities;
class WeylGroup;
class intRoots;
class SemisimpleLieAlgebra;
class GlobalVariables;
class MathRoutines;
class GlobalVariablesContainer;
class MatrixIntTightMemoryFit;
class QuasiPolynomials;
class permutation;
template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
class MonomialInCommutativeAlgebra;
class affineCones;
struct IndicatorWindowVariables;
class rootSubalgebras;
class slTwo;
class SltwoSubalgebras;
class DrawingVariables;
class DrawOperations;
class ElementSimpleLieAlgebra;
template<class CoefficientType>
class ElementUniversalEnveloping;
template<class CoefficientType>
class MonomialUniversalEnveloping;
class rootPoly;
class PolynomialRationalCoeff;
class Cone;
class ConeComplex;
class XMLRoutines;
template<class Base>
class CompleX;
class RationalFunction;
struct CGI;
class charSSAlgMod;
class ReflectionSubgroupWeylGroup;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The documentation of pthreads.h can be found at:
// https://computing.llnl.gov/tutorials/pthreads/#MutexOverview
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The below class is a wrapper for mutexes. All system dependent machinery for mutexes should be put here.
//MutexWrapper specification:
//The mutex has two states: locked and unlocked.
//When the caller calls UnlockMe() this unlocks the mutex if it were locked, otherwise does nothing, and immediately returns.
//When the caller calls LockMe() there are two cases.
//1) First, If the mutex is unlocked, the mutex state changes to locked and execution of the caller continues.
//The preceding two operations are atomic: if the mutex happens to be unlocked, no other processor instruction
//can be executed before the mutex's state is changed to locked.
//2) Second, if the mutex is locked, the calling thread must pause execution, without consuming computational/processor power.
// As soon as the mutex is unlocked (by another thread or by the system), the calling thread is allowed to wake up and perform the sequence described in 1).
// The wake-up time is unspecified/not guaranteed to be immediate: another thread might "jump in" and overtake, again locking the calling thread.
// In order to have guaranteed wake-up when coordinating two threads only, use the controller object (which uses two mutexes to achieve guaranteed wake-up).
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is not guaranteed to work on Windows. Might cause crash. Must be fixed to a proper set of Windows routines.
//This is not possible at the moment since none of my legally owned (but outdated) versions of Windows support the multitasking routines
//that are officially documented at Microsoft's network.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MutexWrapper
{
private:
#ifndef WIN32
  pthread_mutex_t theMutex;
#endif
  bool locked;
public:
  bool isLockedUnsafeUseForWINguiOnly()
  { return this->locked;
  }
  //locks the mutex if the mutex is free. If not it suspends calling thread until mutex becomes free and then locks it.
  inline void LockMe()
  {
#ifndef WIN32
    pthread_mutex_lock(&this->theMutex);
    this->locked=true;
#else
    while(this->locked)
    {}
    this->locked=true;
#endif
  }
  //unlocks the mutex.
  inline void UnlockMe()
  {
#ifndef WIN32
    pthread_mutex_unlock(&this->theMutex);
#endif
    this->locked=false;
  }
  MutexWrapper()
  {
#ifndef WIN32
    pthread_mutex_init(&this->theMutex, NULL);
#endif
    this->locked=false;
  }
  ~MutexWrapper()
  {
#ifndef WIN32
    pthread_mutex_destroy(&this->theMutex);
#else
#endif
  }
};

class Controller
{
  MutexWrapper mutexLockMeToPauseCallersOfSafePoint;
  MutexWrapper mutexSignalMeWhenReachingSafePoint;
  bool flagIsRunning;
  bool flagIsPausedWhileRunning;
  inline bool IsPausedWhileRunning(){ return this->flagIsPausedWhileRunning;}
public:
  MutexWrapper mutexHoldMeWhenReadingOrWritingInternalFlags;
  inline void SafePointDontCallMeFromDestructors()
  { this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
    this->mutexLockMeToPauseCallersOfSafePoint.LockMe();
    this->mutexSignalMeWhenReachingSafePoint.LockMe();
    this->mutexLockMeToPauseCallersOfSafePoint.UnlockMe();
  }
  inline void SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint()
  { this->mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
    if (this->flagIsPausedWhileRunning)
    { this->mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
      return;
    }
    this->mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
    this->mutexLockMeToPauseCallersOfSafePoint.LockMe();
    this->mutexSignalMeWhenReachingSafePoint.LockMe();
    this->flagIsPausedWhileRunning=true;
    this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
  }
  inline void UnlockSafePoint()
  { this->flagIsPausedWhileRunning=false;
    this->mutexLockMeToPauseCallersOfSafePoint.UnlockMe();
  }
  inline void InitComputation()
  { this->mutexSignalMeWhenReachingSafePoint.LockMe();
    this->flagIsRunning=true;
  }
  inline void ExitComputation()
  { this->flagIsRunning=false;
    this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
  }
  bool& GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe()
  { return this->flagIsPausedWhileRunning;
  }
  bool& GetFlagIsRunningUnsafeUseWithMutexHoldMe()
  { return this->flagIsRunning;
  }
  Controller()
  { this->flagIsRunning=false;
    this->flagIsPausedWhileRunning=false;
  }
};

class ControllerStartsRunning: public Controller
{
  public:
    ControllerStartsRunning()
    { this->InitComputation();
    }
};

class ParallelComputing
{
public:
  static int GlobalPointerCounter;
  static int PointerCounterPeakRamUse;
  static ControllerStartsRunning controllerSignalPauseUseForNonGraciousExitOnly;
#ifdef CGIversionLimitRAMuse
  static int cgiLimitRAMuseNumPointersInList;
  static bool flagUngracefulExitInitiated;
  inline static void CheckPointerCounters()
  { if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList)
    { static MutexWrapper tempMutex;
      tempMutex.LockMe();
      if (ParallelComputing::flagUngracefulExitInitiated)
      { tempMutex.UnlockMe();
        return;
      }
      flagUngracefulExitInitiated=true;
      tempMutex.UnlockMe();
      std::cout << "<b>Error:</b> Number of pointers allocated exceeded allowed <b>limit of " <<
      ParallelComputing::cgiLimitRAMuseNumPointersInList << ".</b>\n<br><b>Signalling ungraceful exit...</b>";
      std::cout.flush();
      ParallelComputing::SafePointDontCallMeFromDestructors();
      ParallelComputing::controllerSignalPauseUseForNonGraciousExitOnly.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
      std::exit(0);
    }
    if (ParallelComputing::PointerCounterPeakRamUse<ParallelComputing::GlobalPointerCounter)
      ParallelComputing::PointerCounterPeakRamUse=ParallelComputing::GlobalPointerCounter;
  }
#endif
  inline static void SafePointDontCallMeFromDestructors()
  { ParallelComputing::controllerSignalPauseUseForNonGraciousExitOnly.SafePointDontCallMeFromDestructors();
  }
};

typedef void (*drawLineFunction)(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
typedef void (*drawTextFunction)(double X1, double Y1, const char* theText, int length, int ColorIndex, int fontSize);
typedef void (*drawCircleFunction)(double X1, double Y1, double radius, unsigned long thePenStyle, int ColorIndex);
typedef void (*FeedDataToIndicatorWindow)(IndicatorWindowVariables& input);
typedef void (*drawClearScreenFunction)();

class MathRoutines
{
public:
  static int lcm(int a, int b);
  static int TwoToTheNth(int n);
  static int GetIntFromDigit(char DigitHumanReadable)
  { switch (DigitHumanReadable)
    { case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      default:
        return-1;
    }
  }
  static int NChooseK(int n, int k)
  { int result=1;
    for (int i =0; i<k; i++)
    { result*=(n-i);
      if (result <0)
        return -1;
      result/=(i+1);
      if (result <0)
        return -1;
    }
    return result;
  }
  static inline double Pi(){return 3.141592653589793238462643383279;}
// the MS compiler refuses to compile the following (WTF?), hence the above line.
//  static const double Pi=(double)3.141592653589793238462643383279;
  static int KToTheNth(int k, int n);
  inline static int parity(int n){if (n%2==0) return 1; else return -1; }
  static int BinomialCoefficientMultivariate(int N, List<int>& theChoices);
  template <class Element>
  static void RaiseToPower(Element& theElement, int thePower, const Element& theRingUnit);
  inline static int Maximum(int a, int b)
  { if (a>b)
      return a;
    else
      return b;
  }
  template <typename T>
  inline static void swap(T& a, T& b) { T temp; temp=a; a=b; b=temp; }
  template <class Element>
  inline static Element Minimum(const Element& a, const Element& b)
  { if (a>b)
      return b;
    else
      return a;
  }
  template<class Element>
  static inline std::string ElementToStringBrackets(const Element& input)
  { if (!input.ElementToStringNeedsBracketsForMultiplication())
      return input.ElementToString();
    std::string result;
    result.append("(");
    result.append(input.ElementToString());
    result.append(")");
    return result;
  }
  template<class Element>
  static inline std::string ElementToStringBrackets(const Element& input, const PolynomialOutputFormat& theFormat)
  { if (!input.ElementToStringNeedsBracketsForMultiplication())
      return input.ElementToString(theFormat);
    std::string result;
    result.append("(");
    result.append(input.ElementToString(theFormat));
    result.append(")");
    return result;
  }
};

class XMLRoutines
{
private:
  static std::string GetOpenTagNoInputCheck(const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string result="<";
    result.append(tagNameNoSpacesNoForbiddenCharacters);
    result.append(">");
    return result;
  }
  static std::string GetCloseTagNoInputCheck(const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string result="</";
    result.append(tagNameNoSpacesNoForbiddenCharacters);
    result.append(">");
    return result;
  }
  public:
  static std::string GetOpenTagNoInputCheckAppendSpacE(const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string result=" <";
    result.append(tagNameNoSpacesNoForbiddenCharacters);
    result.append("> ");
    return result;
  }
  static std::string GetCloseTagNoInputCheckAppendSpacE(const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string result=" </";
    result.append(tagNameNoSpacesNoForbiddenCharacters);
    result.append("> ");
    return result;
  }
  inline static bool ReadThroughFirstOpenTag
  (std::istream& streamToMoveIn, const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { int tempInt;
    return XMLRoutines::ReadThroughFirstOpenTag(streamToMoveIn, tempInt, tagNameNoSpacesNoForbiddenCharacters);
  }
  inline static bool ReadEverythingPassedTagOpenUntilTagClose
  (std::istream& streamToMoveIn, const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { int tempInt;
    return XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(streamToMoveIn, tempInt, tagNameNoSpacesNoForbiddenCharacters);
  }
  static bool ReadThroughFirstOpenTag
  (std::istream& streamToMoveIn, int& NumReadWordsExcludingTag,
   const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string tagOpen=XMLRoutines::GetOpenTagNoInputCheck(tagNameNoSpacesNoForbiddenCharacters);
    std::string tempS;
    NumReadWordsExcludingTag=0;
    while (!streamToMoveIn.eof())
    { streamToMoveIn >> tempS;
      if (tempS==tagOpen)
        return true;
      NumReadWordsExcludingTag++;
    }
    return false;
  }
  static bool ReadEverythingPassedTagOpenUntilTagClose
  (std::istream& streamToMoveIn, int& NumReadWordsExcludingTag,
   const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string tagClose=XMLRoutines::GetCloseTagNoInputCheck(tagNameNoSpacesNoForbiddenCharacters);
    std::string tagOpen=XMLRoutines::GetOpenTagNoInputCheck(tagNameNoSpacesNoForbiddenCharacters);
    int TagDepth=1;
    std::string tempS;
    NumReadWordsExcludingTag=0;
    while (!streamToMoveIn.eof())
    { streamToMoveIn >> tempS;
      if (tempS==tagClose)
        TagDepth--;
      if (tempS==tagOpen)
        TagDepth++;
      if (TagDepth==0)
        return true;
      NumReadWordsExcludingTag++;
    }
    return false;
  }
};

class DrawElementInputOutput
{
 public:
  int TopLeftCornerX;
  int TopLeftCornerY;
  int outputWidth;
  int outputHeight;
};

template <class Object>
class MemorySaving
{
private:
  Object* theValue;
  MemorySaving(const MemorySaving<Object>& other){assert(false);}
public:
  void operator=(const MemorySaving<Object>& other)
  { if (!other.IsZeroPointer())
      (this->GetElement()).operator=(other.GetElementConst());
    else this->FreeMemory();
  }
  const Object& GetElementConst()const{ assert(this->theValue!=0); return *this->theValue;}
  Object& GetElement()
  { if (this->theValue==0)
    { this->theValue=new Object;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
    }
    return *(this->theValue);
  }
  bool IsZeroPointer()const{return this->theValue==0;}
  void FreeMemory()
  { delete this->theValue;
    this->theValue=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter--;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  MemorySaving(){this->theValue=0;}
  ~MemorySaving(){this->FreeMemory();}
};

//The below class is to be used together with List.
//The purpose of the class is to save up RAM memory use.
//This is the "light" version it is to be used for storage purposes only.
//To use it as a normal List simply copy it to a buffer List and there
//use the full functionality of List.
//Then copy the buffer List back to the light version to store to RAM.
template <class Object>
class ListLight
{
private:
  ListLight(const ListLight<Object>& right);
public:
  int size;
  Object* TheObjects;
  void AddObjectOnTopLight(const Object& o);
  void CopyFromHeavy(const List<Object>& from);
  void CopyFromLight(const ListLight<Object>& from);
  void PopIndexSwapWithLastLight(int index);
  int SizeWithoutObjects();
  int IndexInList(const Object& o)
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]==o)
        return i;
    return -1;
  }
  inline bool ContainsObject(const Object& o)
  { return this->IndexInList(o)!=-1;
  }
  void SetSize(int theSize);
  inline void initFillInObject(int theSize, const Object& o)
  { this->SetSize(theSize);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i]=o;
  }
  inline Object& operator[](int i)const{return this->TheObjects[i];}
  void operator = (const ListLight<Object>& right);
  void operator = (const List<Object>& right)
  { this->SetSize(right.size);
    for (int i=0; i<right.size; i++)
      this->TheObjects[i]=right[i];
  }
  inline bool operator == (const ListLight<Object>& right) const
  { if (this->size!=right.size)
      return false;
    for (int i=0; i<this->size; i++)
      if (!(this->TheObjects[i]==right[i]))
        return false;
    return true;
  }
  inline Object* LastObject()const{return &this->TheObjects[this->size-1]; }
  ListLight();
  ~ListLight();
};

template <class Object>
void ListLight<Object>::AddObjectOnTopLight(const Object& o)
{ this->SetSize(this->size+1);
  this->TheObjects[this->size-1]=o;
}

template <class Object>
void ListLight<Object>::PopIndexSwapWithLastLight(int index)
{ this->TheObjects[index]=this->TheObjects[this->size-1];
  this->SetSize(this->size-1);
}

template <class Object>
inline void ListLight<Object>::operator =(const ListLight<Object>& right)
{ this->CopyFromLight(right);
}

template <class Object>
inline void ListLight<Object>::SetSize(int theSize)
{ if (theSize== this->size)
    return;
  if (theSize<0)
    theSize=-1;
  if (theSize==0)
  {
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter-=this->size;
  ParallelComputing::CheckPointerCounters();
#endif
    this->size=0;
    delete [] this->TheObjects;
    this->TheObjects=0;
    return;
  }
  Object* newArray= new Object[theSize];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=theSize;
  ParallelComputing::CheckPointerCounters();
#endif
  int CopyUpTo= this->size;
  if (this->size>theSize)
    CopyUpTo= theSize;
  for (int i=0; i<CopyUpTo; i++)
    newArray[i]=this->TheObjects[i];
  delete [] this->TheObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->size;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheObjects=newArray;
  this->size= theSize;
}

template <class Object>
ListLight<Object>::ListLight()
{ this->size =0;
  this->TheObjects=0;
}

template <class Object>
ListLight<Object>::~ListLight()
{ delete [] this->TheObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->size;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheObjects=0;
}

template <class Object>
void ListLight<Object>::CopyFromHeavy(const List<Object>& from)
{ this->SetSize(from.size);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i]= from.TheObjects[i];
}

template <class Object>
void ListLight<Object>::CopyFromLight(const ListLight<Object>& from)
{ this->SetSize(from.size);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i]= from.TheObjects[i];
}

template <class Object>
int ListLight<Object>::SizeWithoutObjects()
{ return sizeof(Object*)*this->size+sizeof(int);
}

template <class Object>
std::iostream& operator<<(std::iostream& output, const List<Object>& theList);

template <class Object>
std::iostream& operator>>(std::iostream& input, List<Object>& theList);

//List kills the objects it contains when it expires
template <class Object>
class List
{ friend std::iostream& operator<< <Object>(std::iostream& output, const List<Object>& theList);
  friend std::iostream& operator>> <Object>(std::iostream& input, List<Object>& theList);
private:
  friend class PolynomialRationalCoeff;
  friend class IntegerPoly;
  friend class partFractions;
  friend class partFraction;
  int ActualSize;
  int IndexOfVirtualZero;
  Object* TheActualObjects;
  void ExpandArrayOnTop(int increase);
  void ExpandArrayOnBottom(int increase);
  void QuickSortAscending(int BottomIndex, int TopIndex);
  void QuickSortDescending(int BottomIndex, int TopIndex);
  inline void initConstructorCallOnly()
  { this->ActualSize=0;
    this->IndexOfVirtualZero=0;
    this->size=0;
    this->TheObjects=0;
    this->TheActualObjects=0;
  }
public:
  List(const List<Object>& other)
  { this->initConstructorCallOnly();
    this->CopyFromBase(other);
  }
  List(const ListLight<Object>& other)
  { this->initConstructorCallOnly();
    this->AssignLight(other);
  }
  inline static std::string GetXMLClassName()
  { std::string result="List_";
    result.append(Object::GetXMLClassName());
    return result;
  }
  static int ListActualSizeIncrement;
  Object* TheObjects;
  int size;
//  void AddOnTop(Object o);
  void AssignLight(const ListLight<Object>& from);
  void ExpandOnTop(int theIncrease){ int newSize=this->size+theIncrease; if(newSize<0) newSize=0; this->SetSize(newSize);}
  void SetSize(int theSize);
  void SetSizeMakeMatrix(int numRows, int numCols)
  { this->SetSize(numRows);
    for (int i=0; i<numRows; i++)
      this->TheObjects[i].SetSize(numCols);
  }
  void initFillInObject(int theSize, const Object& o);
  inline void AddObjectOnTopCreateNew();
  void MakeActualSizeAtLeastExpandOnTop(int theSize);
  void AddObjectOnBottom(const Object& o);
  void AddOnBottomNoRepetition(const Object& o) {if (!this->ContainsObject(o)) this->AddObjectOnBottom(o);}
  void AddOnTop(const Object& o);
  void AddListOnTop(const List<Object>& theList);
  bool AddOnTopNoRepetition(const Object& o);
  void PopIndexShiftUp(int index);
  void PopIndexShiftDown(int index)
  { for (int i=index; i<this->size-1; i++)
      this->TheObjects[i]=this->TheObjects[i+1];
    this->size--;
  }
  void PopIndexSwapWithLast(int index);
  void PopLastObject();
  void QuickSortAscending(){ this->QuickSortAscending(0, this->size-1);}
  void QuickSortDescending(){this->QuickSortAscending(); this->ReverseOrderElements();}
  // the below function is named a bit awkwardly because otherwise there is a risk of confusion
  // with the PopIndexSwapWithLast when selecting from autocomplete list. This cost me already 2 hours of lost time,
  // so the awkward name is necessary.
  void RemoveFirstOccurenceSwapWithLast(const Object& o);
  bool HasACommonElementWith(List<Object>& right);
  void SwapTwoIndices(int index1, int index2);
  std::string ElementToStringGeneric()const{ std::string tempS; this->ElementToStringGeneric(tempS); return tempS;}
  void ElementToStringGeneric(std::string& output)const;
  void ElementToStringGeneric(std::string& output, int NumElementsToPrint)const;
  int IndexOfObject(const Object& o) const;
  bool ContainsObject(const Object& o){ return this->IndexOfObject(o)!=-1; }
  bool ReadFromFile(std::fstream& input){ return this->ReadFromFile(input, 0, -1);}
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){return this->ReadFromFile(input, theGlobalVariables, -1);}
  void WriteToFile(std::fstream& output){this->WriteToFile(output, 0, -1);}
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output, theGlobalVariables, -1);}
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes);
//  inline bool ContainsObject(Object& o){return this->ContainsObject(o)!=-1; };
  int SizeWithoutObjects();
  inline Object* LastObject();
  void ReleaseMemory();
  int HashFunction()const
  { int numCycles=MathRoutines::Minimum(SomeRandomPrimesSize, this->size);
    int result=0;
    for (int i=0; i<numCycles; i++)
      result+=SomeRandomPrimes[i]*this->TheObjects[i].HashFunction();
    return result;
  }
  void operator=(const List<Object>& right){ this->CopyFromBase(right); }
  static void swap(List<Object>& l1, List<Object>& l2);
  void ReverseOrderElements();
  void CopyFromBase (const List<Object>& From);
  bool IsEqualTo(const List<Object>& Other)const
  { if (this->size!=Other.size)
      return false;
    for (int i=0; i<Other.size; i++)
      if (!(this->TheObjects[i]==Other.TheObjects[i]))
        return false;
    return true;
  }
  inline Object& operator[](int i)const{ assert(i<this->size); return this->TheObjects[i];}
  inline bool operator!=(const List<Object>& other)const{ return !this->IsEqualTo(other);}
  inline bool operator==(const List<Object>& other)const{ return this->IsEqualTo(other);}
  bool operator>(const List<Object>& other)const;
  void ShiftUpExpandOnTop(int StartingIndex);
  List(int StartingSize);
  List(int StartingSize, const Object& fillInValue);
  List();
  ~List();
};

template <class Object>
std::iostream& operator<< (std::iostream& output, const List<Object>& theList)
{ output << "size: " << theList.size << "\n";
  for (int i=0; i<theList.size; i++)
    output << theList.TheObjects[i] << " ";
  return output;
}

template <class Object>
std::iostream& operator >> (std::iostream& input, List<Object>& theList)
{ std::string tempS; int tempI;
  input >> tempS >> tempI;
  assert(tempS=="size:");
  theList.SetSize(tempI);
  for (int i=0; i<theList.size; i++)
    input >> theList.TheObjects[i];
  return input;
}

template <class Object>
class ListPointers: public List<Object*>
{
private:
  void QuickSortAscending(int BottomIndex, int TopIndex);
public:
  //ListPointers();
  void KillAllElements();
  void KillElementIndex(int i);
  bool AddObjectNoRepetitionOfPointer(Object* o);
  void PopAllOccurrencesSwapWithLast(Object* o);
  int ObjectPointerToIndex(Object* o);
  void resizeToLargerCreateNewObjects(int increase);
  void IncreaseSizeWithZeroPointers(int increase);
  void initAndCreateNewObjects(int d);
  bool IsAnElementOf(Object* o);
  void QuickSortAscending();
};

template <class Object>
class HashedList : public List<Object>
{
private:
  void AddObjectOnBottom(const Object& o);
  void AddOnTop(const Object& o);
  void AddListOnTop(List<Object>& theList);
  bool AddOnTopNoRepetition(const Object& o);
  void PopIndexShiftUp(int index);
  void PopIndexShiftDown(int index);
  void PopIndexSwapWithLast(int index);
  void RemoveFirstOccurenceSwapWithLast(Object& o);
  void CopyFromBase(const List<Object>& From);
  void SwapTwoIndices(int index1, int index2);
  void operator=(const List<Object>& right);
  void AssignLight(const ListLight<Object>& from);
  void SetSize(int theSize);
  int IndexOfObject(const Object& o);
  bool ContainsObject(const Object& o);
  void ReverseOrderElements();
  void ShiftUpExpandOnTop(int StartingIndex);
  void PopLastObject();
protected:
  void ClearHashes();
  List<int>* TheHashedArrays;
public:
  inline static std::string GetXMLClassName()
  { std::string result="HashedList_";
    result.append(Object::GetXMLClassName());
    return result;
  }
  static int PreferredHashSize;
  int HashSize;
  void initHash();
  inline int FitHashSize( int i){i%=this->HashSize; if (i<0) i+=this->HashSize; return i; }
  void ClearTheObjects();
  void AddOnTopHash(const Object& o);
  void AddOnTopHash(const List<Object>& theList)
  { this->MakeActualSizeAtLeastExpandOnTop(this->size+theList.size);
    for (int i=0; i<theList.size; i++)
      this->AddOnTopHash(theList[i]);
  }
  bool AddOnTopNoRepetitionHash(const Object& o);
  inline void AdjustHashes()
  { if (this->size/this->HashSize<5)
      return;
    this->SetHashSizE(this->size);
    this->MakeActualSizeAtLeastExpandOnTop(this->size*5);
//    static int counter=0;
//    std::cout << "<br> Num times adjust hashes is called: " << ++counter;
  }
  void AddOnTopNoRepetitionHash(const List<Object>& theList);
  void PopIndexSwapWithLastHash(int index);
  //the below returns -1 if it doesn't contain the object,
  //else returns the object's index
  void SwapTwoIndicesHash(int i1, int i2);
  inline bool ContainsObjectHash(const Object& o) {return this->IndexOfObjectHash(o)!=-1; }
  int IndexOfObjectHash(const Object& o) const;
  void SetExpectedSize(int expectedSize)
  { if (expectedSize<1)
      return;
    this->MakeActualSizeAtLeastExpandOnTop(expectedSize);
    this->SetHashSizE(MathRoutines::Maximum(this->HashSize, expectedSize*5));
  }
  void SetHashSizE(int HS);
  int SizeWithoutObjects();
  void AssignList(const List<Object>& other)
  { this->ClearTheObjects();
    this->MakeActualSizeAtLeastExpandOnTop(other.size);
    for (int i=0; i<other.size; i++)
      this->AddOnTopHash(other.TheObjects[i]);
  }
  void QuickSortAscending(){ List<Object> theList; theList=*this; theList.QuickSortAscending(); this->AssignList(theList);}
  void QuickSortDescending(){ List<Object> theList; theList=*this; theList.QuickSortDescending(); this->AssignList(theList);}
  HashedList();
  ~HashedList();
  void CopyFromHash(const HashedList<Object>& From);
  inline void operator=(const HashedList<Object>& From){this->CopyFromHash(From);}
};

class Integer
{
public:
  int value;
  static Integer TheRingUnit;
  static Integer TheRingMUnit;
  static Integer TheRingZero;
  inline void ComputeDebugString(){}
  inline void Add(const Integer& y){ this->value+=y.value; }
  inline void operator+=(const Integer& y){this->value+=y.value; }
  inline void MultiplyBy(const Integer& y){this->value*=y.value; }
  inline void operator=(const Integer& y){this->value=y.value; }
  inline bool operator==(Integer& y){return this->value==y.value; }
  inline void Assign(const Integer& y){this->value=y.value; }
  inline bool IsEqualTo(const Integer&y)const {return this->value==y.value; }
  inline bool IsEqualToZero()const {return this->value==0; }
  inline void DivideBy(const Integer& y){this->value/=y.value; }
  inline void operator/=(const Integer& y){this->value/=y.value; }
  inline void WriteToFile(std::fstream& output){output<<this->value; }
  inline void ReadFromFile(std::fstream& input){input>>this->value; }
  inline void ElementToString(std::string& output)const{ std::stringstream out; out << this->value; output= out.str(); }
  Integer(int x){this->value=x; }
  Integer(){}
};

template <typename Element>
class MatrixElementaryLooseMemoryFit
{
  MatrixElementaryLooseMemoryFit(const MatrixElementaryLooseMemoryFit<Element>& other);
public:
  int NumRows; int ActualNumRows;
  int NumCols; int ActualNumCols;
  Element** elements;
  void init(int r, int c);
  void ReleaseMemory();
  bool IsEqualTo(MatrixElementaryLooseMemoryFit<Element>& right);
  void Resize(int r, int c, bool PreserveValues) {this->Resize(r, c, PreserveValues, 0);}
  void Resize
  (int r, int c, bool PreserveValues, const Element* TheRingZero)
  ;
  void Assign(const MatrixElementaryLooseMemoryFit<Element>& m);
  inline void operator=(const MatrixElementaryLooseMemoryFit<Element>& other){this->Assign(other); }
  MatrixElementaryLooseMemoryFit();
  ~MatrixElementaryLooseMemoryFit();
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
{ this->ReleaseMemory();
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::init(int r, int c)
{ this->Resize(r, c, false);
}

template <typename Element>
bool MatrixElementaryLooseMemoryFit<Element>::IsEqualTo(MatrixElementaryLooseMemoryFit<Element>& right)
{ if (this->NumCols!=right.NumCols || this->NumRows!=right.NumRows)
    return false;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      if(!(this->elements[i][j]==right.elements[i][j]))
        return false;
  return true;
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::Resize(int r, int c, bool PreserveValues, const Element* const TheRingZero)
{ if (r<0)
    r=0;
  if (c<0)
    c=0;
  if (r==this->NumRows && c== this->NumCols)
    return;
  if (r==0 || c==0)
  { this->NumRows=r;
    this->NumCols=c;
    return;
  }
  Element** newElements=0;
  int newActualNumCols= MathRoutines::Maximum(this->ActualNumCols, c);
  int newActualNumRows= MathRoutines::Maximum(this->ActualNumRows, r);
  if (r>this->ActualNumRows || c>this->ActualNumCols)
  { newElements  = new Element*[newActualNumRows];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=newActualNumRows;
  ParallelComputing::CheckPointerCounters();
#endif
    for (int i=0; i<newActualNumRows; i++)
    { newElements[i]= new Element[newActualNumCols];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=newActualNumCols;
  ParallelComputing::CheckPointerCounters();
#endif
    }
  }
  int firstInvalidRow=MathRoutines::Minimum(this->NumRows, r);
  int firstInvalidCol=MathRoutines::Minimum(this->NumCols, c);
  if (PreserveValues && newElements!=0)
    for (int j=0; j<firstInvalidRow; j++)
      for (int i=0; i<firstInvalidCol; i++)
        newElements[j][i]= this->elements[j][i];
  if (TheRingZero!=0)
  { if (!PreserveValues)
    { firstInvalidRow=0;
      firstInvalidCol=0;
    }
    for (int i=0; i<r; i++)
    { int colStart= (i<firstInvalidRow) ? firstInvalidCol : 0;
      for (int j=colStart; j<c; j++)
        newElements[i][j]=*TheRingZero;
    }
  }
  if (newElements!=0)
  { this->ReleaseMemory();
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
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j]=m.elements[i][j];
}

template <typename Element>
inline void MatrixElementaryLooseMemoryFit<Element>::ReleaseMemory()
{ for (int i=0; i<this->ActualNumRows; i++)
    delete [] this->elements[i];
  delete [] this->elements;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualNumRows*this->ActualNumCols+this->ActualNumRows;
  ParallelComputing::CheckPointerCounters();
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
  void init(int r, int c);
  void Free();
  void Resize(int r, int c, bool PreserveValues);
  void Assign(const MatrixElementaryTightMemoryFit<Element>& m);
  inline void operator=(const MatrixElementaryTightMemoryFit<Element>& m){this->Assign(m);};
  MatrixElementaryTightMemoryFit<Element>();
  ~MatrixElementaryTightMemoryFit<Element>();
};

template<class Base>
std::iostream& operator<<(std::iostream& output, const CompleX<Base>& input);

template <typename Element>
std::iostream& operator<< (std::iostream& output, const Matrix<Element>& theMat)
{ output << "<table>";
  for (int i=0; i<theMat.NumRows; i++)
  { output << "<tr>";
    for (int j=0; j<theMat.NumCols; j++)
    { output << "<td>";
      output << (theMat.elements[i][j]);
      output << "</td> ";
    }
    output << "</tr>\n";
  }
  output << "</table>";
  return output;
}

template <typename Element>
class Matrix: public MatrixElementaryLooseMemoryFit<Element>
{ friend std::iostream& operator<< <Element>(std::iostream& output, const Matrix<Element>& theMat);
//  friend std::iostream& operator>> <Element>(std::iostream& input, Matrix<Element>& theMat);
public:
  inline static std::string GetXMLClassName()
  { std::string result="Matrix_";
    result.append(Element::GetXMLClassName());
    return result;
  }
  Matrix(const Matrix<Element>& other)
  { this->Assign(other);
  }
  inline void operator=(const Matrix<Element>& other)
  { this->Assign(other);
  }
  Matrix(){}
  std::string DebugString;
  static bool flagComputingDebugInfo;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void Transpose()
  { if (this->NumCols==this->NumRows)
    { for (int i=0; i<this->NumRows; i++)
        for (int j=i+1; j<this->NumCols; j++)
          MathRoutines::swap<Element>(this->elements[j][i], this->elements[i][j]);
      return;
    }
    Matrix<Element> tempMat;
    tempMat.init(this->NumCols, this->NumRows);
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        tempMat.elements[j][i].Assign(this->elements[i][j]);
    this->Assign(tempMat);
  }
  void ComputeDebugString();
  void ComputeDebugString(bool useHtml, bool useLatex){this->ElementToString(this->DebugString, useHtml, useLatex);}
  void ElementToString(std::string& output)const;
  void ActMultiplyVectorRowOnTheRight(Vector<Element>& theRoot,  const Element& TheRingZero)const{ Vector<Element> output; this->ActMultiplyVectorRowOnTheRight(theRoot, output, TheRingZero); theRoot=output; }
  void ActMultiplyVectorRowOnTheRight(const Vector<Element>& input, Vector<Element>& output, const Element& TheRingZero)const
  { assert(&input!=&output);
    assert(this->NumRows==input.size);
    output.MakeZero(this->NumCols, TheRingZero);
    Element tempElt;
    for (int i=0; i<this->NumCols; i++)
      for (int j=0; j<this->NumRows; j++)
      { tempElt=this->elements[j][i];
        tempElt*=input.TheObjects[j];
        output.TheObjects[i]+=tempElt;
      }
  }
  void GetNSquaredVectorForm(Vector<Element>& output)
  { output.SetSize(this->NumRows*this->NumCols);
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        output.TheObjects[i*this->NumRows+j]=this->elements[i][j];
  }
  void ActOnVectorColumn(const Vector<Element>& input, Vector<Element>& output, const Element& TheRingZero)const
  { assert(&input!=&output);
    assert(this->NumCols==input.size);
    output.MakeZero(this->NumRows, TheRingZero);
    Element tempElt;
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
      { tempElt=this->elements[i][j];
        tempElt*=input.TheObjects[j];
        output.TheObjects[i]+=tempElt;
      }
  }
  void ActOnVectorsColumn(const Vectors<Element>& input, Vectors<Element>& output, const Element& TheRingZero)const
  { assert(&input!=&output);
    assert(this->NumCols==input.size);
    output.SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->ActOnVectorColumn(input.TheObjects[i], output.TheObjects[i], TheRingZero);
  }
  void ActOnVectorColumn(Vector<Element>& inputOutput, const Element& TheRingZero=(Element) 0)const{ Vector<Element> buffer; this->ActOnVectorColumn(inputOutput, buffer, TheRingZero); inputOutput=buffer;}
  void ActOnVectorsColumn(Vectors<Element>& inputOutput, const Element& TheRingZero=(Element) 0)const{ Vectors<Element> buffer; this->ActOnVectorsColumn(inputOutput, buffer, TheRingZero); inputOutput=buffer;}
  void ElementToString(std::string& output, bool useHtml, bool useLatex)const ;
  std::string ElementToStringWithBlocks(List<int>& theBlocks);
  std::string ElementToString(bool useHtml, bool useLatex)const{std::string tempS; this->ElementToString(tempS, useHtml, useLatex); return tempS;}
  void MakeIdMatrix(int theDimension, const Element& theRingUnit, const Element& theRingZero)
  { this->init(theDimension, theDimension);
    for (int i=0; i<theDimension; i++)
      for (int j=0; j<theDimension; j++)
        if (j!=i)
          this->elements[i][j]=theRingZero;
        else
          this->elements[i][j]=theRingUnit;
  }
  void SwitchTwoRows(int row1, int row2);
  inline void SwitchTwoRowsWithCarbonCopy(int row1, int row2, Matrix<Element>* theCarbonCopy)
  { this->SwitchTwoRows(row1, row2);
    if (theCarbonCopy!=0)
      theCarbonCopy->SwitchTwoRows(row1, row2);
  }
  inline Element ScalarProduct(const Vector<Element>& left, const Vector<Element>& right){ return this->ScalarProduct(left, right, (Element) 0);}
  Element ScalarProduct(const Vector<Element>& left, const Vector<Element>& right, const Element& theRingZero)
  { assert(left.size==this->NumCols && right.size==this->NumRows);
    Element Result, tempElt;
    Result=theRingZero;
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
      { tempElt=left.TheObjects[i];
        tempElt*=this->elements[i][j];
        tempElt*=right.TheObjects[j];
        Result+=tempElt;
      }
    return Result;
  }
  void ColToRoot(int colIndex, Vector<Element>& output)const;
  void RowToRoot(int rowIndex, Vector<Element>& output)const;
  int FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex);
  bool FindFirstNonZeroElementSearchEntireRow(Element& output)
  { for (int i=0; i<this->NumCols; i++)
      for(int j=0; j<this->NumRows; j++)
        if (!this->elements[i][j].IsEqualToZero())
        { output=this->elements[i][j];
          return true;
        }
    return false;
  }
  void RowTimesScalar(int rowIndex, const Element& scalar);
  inline void RowTimesScalarWithCarbonCopy(int rowIndex, const Element& scalar, Matrix<Element>* theCarbonCopy)
  { this->RowTimesScalar(rowIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->RowTimesScalar(rowIndex, scalar);
  }
  void AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar);
  inline void AddTwoRowsWithCarbonCopy(int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar, Matrix<Element>* theCarbonCopy)
  { this->AddTwoRows(fromRowIndex, ToRowIndex, StartColIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->AddTwoRows(fromRowIndex, ToRowIndex, StartColIndex, scalar);
  }
  void SubtractRows(int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const Element& scalar);
  inline void SubtractRowsWithCarbonCopy(int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const Element& scalar, Matrix<Element>* theCarbonCopy)
  { this->SubtractRows(indexRowWeSubtractFrom, indexSubtracted, StartColIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->SubtractRows(indexRowWeSubtractFrom, indexSubtracted, StartColIndex, scalar);
  }
  void MultiplyOnTheLeft(const Matrix<Element>& input, Matrix<Element>& output);
  void MultiplyOnTheLeft(const Matrix<Element>& input);
  void NonPivotPointsToEigenVectorMatrixForm
  (Selection& TheNonPivotPoints, Matrix<Element>& output, const Element& theRingUnit, const Element& theRingZero)
;
void NonPivotPointsToEigenVector
(Selection& TheNonPivotPoints, Vector<Element>& output, const Element& theRingUnit, const Element& theRingZero)
;
  //returns true if successful, false otherwise
//  bool ExpressColumnAsALinearCombinationOfColumnsModifyMyself
//    (Matrix<Element>& inputColumn, Matrix<Element>* outputTheGaussianTransformations Matrix<Element>& outputColumn);
  bool Invert(GlobalVariables& theGlobalVariables){return this->Invert();}
  bool Invert();
  void NullifyAll();
  void NullifyAll(const Element& theRingZero);
  //if m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below function
  //corresponds to the linear operator from V1+W1 to V2+W2 (direct sum)
  //this means you write the matrix m1 in the upper left corner m2 in the lower right
  // and everything else you fill with zeros
  void AssignDirectSum(Matrix<Element>& m1,  Matrix<Element>& m2);
  void AssignVectorsToRows(Vectors<Element>& input)
  { int numCols=-1;
    if (input.size>0)
      numCols=input[0].size;
    this->init(input.size, numCols);
    for (int i=0; i<input.size; i++)
      for (int j=0; j<numCols; j++)
        this->elements[i][j]=input[i][j];
  }
  void AssignVectorColumn(const Vector<Element>& input)
  { this->init(input.size, 1);
    for (int i=0; i<input.size; i++)
      this->elements[i][0]=input.TheObjects[i];
  }
  void AssignVectorToRowKeepOtherRowsIntactNoInit(int rowIndex, const Vector<Element>& input)
  { assert(input.size==this->NumCols && rowIndex<this->NumRows && rowIndex>=0);
    for (int i=0; i<this->NumCols; i++)
      this->elements[rowIndex][i]=input[i];
  }
  void AssignVectorToColumnKeepOtherColsIntactNoInit(int colIndex, const Vector<Element>& input)
  { assert(input.size==this->NumRows && colIndex<this->NumCols && colIndex>=0);
    for (int i=0; i<this->NumRows; i++)
      this->elements[i][colIndex]=input[i];
  }
  void AssignVectorRow(const Vector<Element>& input)
  { this->init(1, input.size);
    for (int i=0; i<input.size; i++)
      this->elements[0][i]=input.TheObjects[i];
  }
  void FindZeroEigenSpacE
  (List<List<Element> >& output, const Element& theRingUnit, const Element& theRingMinusUnit, const Element& theRingZero, GlobalVariables& theGlobalVariables)
;
  void DirectSumWith(const Matrix<Element>& m2, const Element& theRingZero);
  inline bool operator==(const Matrix<Element>& other)const
  { if (this->NumRows!=other.NumRows || this->NumCols!=other.NumCols)
      return false;
    for(int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        if (!(this->elements[i][j]==other.elements[i][j]))
          return false;
    return true;
  }
  bool IsEqualToZero()const
  { for(int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        if (this->elements[i][j]!=0)
          return false;
    return true;
  }
  bool IsProportionalTo(const Matrix<Element>& input, Element& outputTimesMeEqualsInput)
  { if (input.NumCols!=this->NumCols || input.NumRows!=this->NumRows)
      return false;
    bool found=false;
    for (int i=0; i<this->NumRows &&!found; i++)
      for (int j=0; j<this->NumCols; j++)
        if (!this->elements[i][j].IsEqualToZero())
        { found=true;
          outputTimesMeEqualsInput=input.elements[i][j];
          outputTimesMeEqualsInput/=this->elements[i][j];
          break;
        }
    if (!found)
      return input.IsEqualToZero();
    Matrix<Element> tempMat;
    tempMat=*this;
    tempMat*=outputTimesMeEqualsInput;
    tempMat.Subtract(input);
    return tempMat.IsEqualToZero();
  }
  //returns true if the system has a solution, false otherwise
  bool RowEchelonFormToLinearSystemSolution(Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide, Matrix<Element>& outputSolution);
  void Add(const Matrix<Element>& right)
  { assert(this->NumRows==right.NumRows && this->NumCols==right.NumCols);
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j]+=(right.elements[i][j]);
  }
  void Subtract(const Matrix<Element>& right)
  { assert(this->NumRows==right.NumRows && this->NumCols==right.NumCols);
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j].Subtract(right.elements[i][j]);
  }
  inline void operator-=(const Matrix<Element>& other)
  { this->Subtract(other);
  }
  void WriteToFile(std::fstream& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output);}
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){return this->ReadFromFile(input);}
  bool ReadFromFile(std::fstream& input);
  void operator/=(const Element& input)
  { for (int j=0; j<this->NumRows; j++)
      for (int i=0; i<this->NumCols; i++)
        this->elements[j][i]/=input;
  }
  void operator*=(const Element& input)
  { for (int j=0; j<this->NumRows; j++)
      for (int i=0; i<this->NumCols; i++)
        this->elements[j][i]*=input;
  }
  static void LieBracket(const Matrix<Element>& left, const Matrix<Element>& right, Matrix<Element>& output)
  { assert(left.NumCols==left.NumRows && right.NumCols==right.NumRows && left.NumCols==right.NumCols);
    Matrix<Element> tempPlus, tempMinus;
    tempPlus.Assign(right);
    tempPlus.MultiplyOnTheLeft(left);
    tempMinus.Assign(left);
    tempMinus.MultiplyOnTheLeft(right);
    tempPlus.Subtract(tempMinus);
    output=tempPlus;
  }
  //The Gaussian elimination below brings the matrix to a row-echelon form, that is, makes the matrix be something like (example is 4x5):
  //1 0 3 0 0
  //0 1 0 0 0
  //0 0 0 1 0
  //0 0 0 0 0
  //In the Gaussian elimination below, we define non-pivot points to be indices of the columns
  // that do not have a pivot 1 in them.
  // In the above example, the third (index 2) and fifth (index 4) columns do not have a pivot 1 in them.
  inline static void GaussianEliminationByRows(Matrix<Element>& theMatrix, Matrix<Element>& otherMatrix, Selection& outputNonPivotPoints){ Matrix<Element>::GaussianEliminationByRows(theMatrix, otherMatrix, outputNonPivotPoints, true);  }
  static void GaussianEliminationByRows(Matrix<Element>& mat, Matrix<Element>& output, Selection& outputSelection, bool returnNonPivotPoints);
  void GaussianEliminationByRows(Matrix<Element>& otherMatrix, Selection& outputNonPivotColumns)
  { Matrix<Element>::GaussianEliminationByRows(*this, otherMatrix, outputNonPivotColumns);
  }
  void GaussianEliminationByRowsNoRowSwapPivotPointsByRows
(int firstNonProcessedRow, Matrix<Element>& output, List<int>& outputPivotPointCols, Selection* outputNonPivotPoints__WarningSelectionNotInitialized)
;
  void GaussianEliminationEuclideanDomain
(Matrix<Element>* otherMatrix, const Element& theRingMinusUnit, const Element& theRingUnit)
;
inline  void GaussianEliminationEuclideanDomain
(const Element& theRingMinusUnit, const Element& theRingUnit) {this->GaussianEliminationEuclideanDomain(0, theRingMinusUnit, theRingUnit);}
  static bool Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output);
};

template <class Element>
void Matrix<Element>::GaussianEliminationEuclideanDomain
(Matrix<Element>* otherMatrix, const Element& theRingMinusUnit, const Element& theRingUnit)
{ int col=0;
  Element tempElt;
  int row=0;
  while(row<this->NumRows && col<this->NumCols)
  { //std::cout << "<br>****************row: " << row << " status: " << this->ElementToString(true, false);
    int findPivotRow=-1;
    for (int i=row; i<this->NumRows; i++)
      if(!this->elements[i][col].IsEqualToZero())
      { findPivotRow=i;
        break;
      }
    if (findPivotRow!=-1)
    { this->SwitchTwoRowsWithCarbonCopy(row, findPivotRow, otherMatrix);
      if (this->elements[row][col]<0)
        this->RowTimesScalarWithCarbonCopy(row, theRingMinusUnit, otherMatrix);
      int ExploringRow= row+1;
//      std::cout << "<br>before second while: " << this->ElementToString(true, false);
      while (ExploringRow<this->NumRows)
      { Element& PivotElt=this->elements[row][col];
        Element& otherElt=this->elements[ExploringRow][col];
        if (otherElt<0)
          this->RowTimesScalarWithCarbonCopy(ExploringRow, theRingMinusUnit, otherMatrix);
        if (PivotElt<=otherElt)
        { tempElt=otherElt/PivotElt;
          tempElt.AssignFloor();
          this->SubtractRowsWithCarbonCopy(ExploringRow, row, 0, tempElt, otherMatrix);
        }
        if (this->elements[ExploringRow][col].IsEqualToZero())
          ExploringRow++;
        else
          this->SwitchTwoRowsWithCarbonCopy(ExploringRow, row, otherMatrix);
//        std::cout << "<br>second while cycle end: " << this->ElementToString(true, false);
      }
      Element& PivotElt = this->elements[row][col];
      for (int i=0; i<row; i++)
      { tempElt =this->elements[i][col]/PivotElt;
//        std::cout << " the floor of " << tempElt.ElementToString();
        tempElt.AssignFloor();
//        std::cout << " is " << tempElt.ElementToString();
        this->SubtractRowsWithCarbonCopy(i, row, 0, tempElt, otherMatrix);
        if (this->elements[i][col].IsNegative())
          this->AddTwoRowsWithCarbonCopy(row, i, 0, theRingUnit, otherMatrix);
      }
      row++;
    }
    col++;
//    std::cout << "end of cycle status: " << this->ElementToString(true, false) << "<br>****************";
  }
}

template <class Element>
bool Matrix<Element>::flagAnErrorHasOccurredTimeToPanic=false;

class MatrixLargeRational: public Matrix<Rational>
{
public:
  static bool flagAnErrorHasOccurredTimeToPanic;
  void ComputeDeterminantOverwriteMatrix(Rational& output);
  Rational GetDeterminant();
  void NonPivotPointsToRoot(Selection& TheNonPivotPoints, root& output);
  void Transpose(GlobalVariables& theGlobalVariables){this->Transpose();}
  void Transpose();
  void MultiplyByInt(int x);
  void AssignMatrixIntWithDen(const MatrixIntTightMemoryFit& theMat, int Den);
  void AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, LargeIntUnsigned& Den);
  void AssignRootsToRowsOfMatrix(const roots& input);
  void ScaleToIntegralForMinRationalHeightNoSignChange();
  void MultiplyByLargeRational(Rational& x);
  void MakeLinearOperatorFromDomainAndRange(roots& domain, roots& range, GlobalVariables& theGlobalVariables);
  void ActOnAroot(root& theRoot)const{ this->ActOnAroot(theRoot, theRoot); }
  void ActOnAroot(const root& input, root& output)const;
  void ActOnRoots(const roots& input, roots& output)const;
  void ActOnRoots(roots& theRoots)const;
  void ActOnMonomialAsDifferentialOperator(Monomial<Rational>& input, PolynomialRationalCoeff& output);
  void GetMatrixIntWithDen(MatrixIntTightMemoryFit& outputMat, int& outputDen);
  void GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen);
  void MakeIdMatrix(int theDim);
  void DivideByRational(Rational& x);
  void LieBracketWith(MatrixLargeRational& right);
  void ApproximateLargestEigenSpace
  (roots& outputBasis, const Rational& DesiredError, int SuggestedOrder, int numIterations)
  ;
  void MatrixToRoot(root& output);
//  MatrixLargeRational LieBracketWith(MatrixLargeRational& right)
//  { MatrixLargeRational tempMat;
//    tempMat.Assign(*this);
 //   tempMat.LieBracketWith(right);
//    return tempMat;
//  }
  inline void FindZeroEigenSpace(roots& output, GlobalVariables& theGlobalVariables){this->FindZeroEigenSpaceOneOneForEachNonPivot(output);}
  inline void FindZeroEigenSpace(roots& output){this->FindZeroEigenSpaceOneOneForEachNonPivot(output);}
  void FindZeroEigenSpaceOneOneForEachNonPivot(roots& output);
  static bool SystemLinearInequalitiesHasSolution(MatrixLargeRational& matA, MatrixLargeRational& matb, MatrixLargeRational& outputPoint);
  static bool SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(MatrixLargeRational& matA, MatrixLargeRational& matb, MatrixLargeRational& outputSolution, GlobalVariables& theGlobalVariables);
  static void ComputePotentialChangeGradient(MatrixLargeRational& matA, Selection& BaseVariables, int NumTrueVariables, int ColumnIndex, Rational& outputChangeGradient, bool& hasAPotentialLeavingVariable);
  static void GetMaxMovementAndLeavingVariableRow(Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables, MatrixLargeRational& tempMatA, MatrixLargeRational& matX, Selection& BaseVariables);
  int FindPositiveLCMCoefficientDenominatorsTruncated();
  int FindPositiveGCDCoefficientNumeratorsTruncated();
  bool IsIdMatrix()const;
  MatrixLargeRational(const MatrixLargeRational& right){this->Assign(right);}
  MatrixLargeRational(){}
  MatrixLargeRational operator-(const MatrixLargeRational& right)const
  { MatrixLargeRational tempMat;
    tempMat.Assign(*this);
    tempMat.Subtract(right);
    return tempMat;
  }
  MatrixLargeRational operator*(const MatrixLargeRational& right)const
  { MatrixLargeRational tempMat;
    tempMat.Assign(right);
    tempMat.ComputeDebugString();
    tempMat.MultiplyOnTheLeft(*this);
    tempMat.ComputeDebugString();
    return tempMat;
  }
};

class Selection
{
public:
  inline static std::string GetXMLClassName(){ return "Selection";}
  int MaxSize;
  int* elements;
//  int* elementsInverseSelection;
  bool* selected;
  int CardinalitySelection;
  void AddSelectionAppendNewIndex(int index);
  void RemoveLastSelection();
  void RemoveSelection(int index)
  { this->selected[index]=false;
    this->ComputeIndicesFromSelection();
  }
  void MakeFullSelection()
  { for (int i=0; i<this->MaxSize; i++)
    { this->elements[i]=i;
      this->selected[i]=true;
    }
  }
  void init(int maxNumElements);
  void ComputeIndicesFromSelection();
  void initNoMemoryAllocation();
  int HashFunction() const;
  std::string DebugString;
  void ComputeDebugString();
  std::string ElementToString() {std::string tempS; this->ElementToString(tempS); return tempS;}
  void ElementToString(std::string& output);
  void incrementSelection();
  int SelectionToIndex();
  void ExpandMaxSize();
  int GetNumCombinationsFixedCardinality(int theCardinality){return MathRoutines::NChooseK(this->MaxSize, theCardinality);}
  void ShrinkMaxSize();
  void MakeSubSelection(Selection& theSelection, Selection& theSubSelection);
  inline void incrementSelectionFixedCardinality(int card){int IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore; this->incrementSelectionFixedCardinality(card, IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore);}
  void incrementSelectionFixedCardinality(int card, int& IndexLastZeroWithOneBefore, int& NumOnesAfterLastZeroWithOneBefore);
  void Assign(const Selection& right);
  void WriteToFile(std::fstream& output);
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output);}
  void ReadFromFile(std::fstream& input);
  inline void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){ this->ReadFromFile(input);}
  inline void operator=(const Selection& right){this->Assign(right); }
  void operator=(const root& other);
//  void operator=(const std::string& other);
  //warning: to call the comparison operator sucessfully, cardinalitySelection must
  //be properly computed!
  inline bool operator==(const Selection& right) const
  { if (this->MaxSize!=right.MaxSize || this->CardinalitySelection!=right.CardinalitySelection)
      return false;
    for (int i=0; i<this->CardinalitySelection; i++)
      if (this->selected[this->elements[i]]!=right.selected[this->elements[i]])
        return false;
    return true;
  }
  Selection();
  Selection(int m);
  Selection(const Selection& other){this->Assign(other);}
  ~Selection();
};

class SelectionWithMultiplicities
{
public:
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  void ElementToString(std::string& output);
  std::string ElementToString(){std::string tempS; this->ElementToString(tempS); return tempS;}
  List<int> elements;
  List<int> Multiplicities;
  int CardinalitySelectionWithoutMultiplicities();
  void initWithMultiplicities(int NumElements);
  void ComputeElements();
};

class SelectionWithMaxMultiplicity: public SelectionWithMultiplicities
{ void init(int NumElements);
  void InitMe(int NumElements);
  void initWithMultiplicities(int NumElements);
public:
  int MaxMultiplicity;
  void initMaxMultiplicity(int NumElements, int MaxMult);
  int NumCombinationsOfCardinality(int cardinality);
  int NumSelectionsTotal(){return MathRoutines::KToTheNth(MaxMultiplicity, this->Multiplicities.size);}
  void IncrementSubset();
  void IncrementSubsetFixedCardinality(int Cardinality);
  bool HasMultiplicitiesZeroAndOneOnly();
  int MaxCardinalityWithMultiplicities(){return this->MaxMultiplicity*this->Multiplicities.size; }
  int CardinalitySelectionWithMultiplicities();
};

class SelectionWithDifferentMaxMultiplicities : public SelectionWithMultiplicities
{
public:
  List<int> MaxMultiplicities;
  void initIncomplete(int NumElements){ this->MaxMultiplicities.SetSize(NumElements); this->initWithMultiplicities(NumElements); }
  void clearNoMaxMultiplicitiesChange();
  void IncrementSubset();
  int getTotalNumSubsets();
  int TotalMultiplicity();
  int MaxTotalMultiplicity();
  void initFromInts(int* theMaxMults, int NumberMaxMults);
  void initFromInts(const List<int>& theMaxMults);
  bool HasSameMaxMultiplicities(SelectionWithDifferentMaxMultiplicities& other){ return this->MaxMultiplicities.IsEqualTo(other.MaxMultiplicities); }
  void operator=(const SelectionWithDifferentMaxMultiplicities& right)
  { this->Multiplicities.CopyFromBase(right.Multiplicities);
    this->MaxMultiplicities.CopyFromBase(right.MaxMultiplicities);
    this->elements.CopyFromBase(right.elements);
  }
};

class MatrixIntTightMemoryFit : public MatrixElementaryTightMemoryFit<int>
{
public:
  void NullifyAll()
  { for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j]=0;
  }
};

template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::Assign(const MatrixElementaryTightMemoryFit<Element>& m)
{ if (this==&m) return;
  this->init(m.NumRows, m.NumCols);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
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
{ for (int i=0; i<this->NumRows; i++)
    delete [] this->elements[i];
  delete [] this->elements;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->NumRows*this->NumCols+this->NumRows;
  ParallelComputing::CheckPointerCounters();
#endif
  this->elements=0;
  this->NumRows=0;
  this->NumCols=0;
}

template <typename Element>
void Matrix<Element>::WriteToFile(std::fstream& output)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "r: " << this->NumRows << " c: " << this->NumCols << "\n";
  for (int i=0; i<this->NumRows; i++)
  { for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j].WriteToFile(output);
      output << " ";
    }
    output << "\n";
  }
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

template <typename Element>
bool Matrix<Element>::ReadFromFile(std::fstream& input)
{ int r, c; std::string tempS;
  int NumReadWords;
  XMLRoutines::ReadThroughFirstOpenTag(input, NumReadWords, this->GetXMLClassName());
  assert(NumReadWords==0);
  input >> tempS >> r >> tempS >> c;
  if (tempS!="c:")
  { assert(false);
    return false;
  }
  this->init(r, c);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j].ReadFromFile(input);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumReadWords, this->GetXMLClassName());
  assert(NumReadWords==0);
  return true;
}

template <typename Element>
bool Matrix<Element>::Invert()
{ assert(this->NumCols==this->NumRows);
  if (this->flagComputingDebugInfo)
    this->ComputeDebugString();
  Matrix tempMatrix;
  Selection NonPivotPts;
  tempMatrix.init(this->NumRows, this->NumCols);
  tempMatrix.NullifyAll();
  for (int i=0; i<this->NumCols; i++)
    tempMatrix.elements[i][i].MakeOne();
  this->GaussianEliminationByRows(*this, tempMatrix, NonPivotPts);
  if(NonPivotPts.CardinalitySelection!=0)
    return false;
  else
  { this->Assign(tempMatrix);
    if (this->flagComputingDebugInfo)
      this->ComputeDebugString();
    return true;
  }
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(const Matrix<Element>& input)
{ Matrix<Element> tempMat;
  this->MultiplyOnTheLeft(input, tempMat);
  this->Assign(tempMat);
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(const Matrix<Element>& input, Matrix<Element>& output)
{ assert(&output!=this && &output!=&input);
  assert(this->NumRows==input.NumCols);
  Element tempEl;
  output.init(input.NumRows, this->NumCols);
  for (int i=0; i< input.NumRows; i++)
    for( int j=0; j< this->NumCols; j++)
    { output.elements[i][j]=0;
      for (int k=0; k<this->NumRows; k++)
      { tempEl.Assign(input.elements[i][k]);
        tempEl.MultiplyBy(this->elements[k][j]);
        output.elements[i][j]+=(tempEl);
      }
    }
}

template <typename Element>
inline void Matrix<Element>::ElementToString(std::string& output)const
{ this->ElementToString(output, false, false);
}

template <typename Element>
std::string Matrix<Element>::ElementToStringWithBlocks(List<int>& theBlocks)
{ std::stringstream out;
  std::string tempS;
  out << "\\left(\\begin{array}{";
  int offset=0; int blockIndex=0;
  for (int j=0; j<this->NumCols; j++)
  { out << "c";
    offset++;
    if (offset==theBlocks.TheObjects[blockIndex])
    { offset=0;
      blockIndex++;
      if (j!=this->NumCols-1)
        out << "|";
    }
  }
  out << "}";
  offset=0; blockIndex=0;
  for (int i=0; i<this->NumRows; i++)
  { for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j].ElementToString(tempS);
      out << tempS;
      if (j!=this->NumCols-1)
        out << " & ";
    }
    out << "\\\\\n";
    offset++;
    if (offset==theBlocks.TheObjects[blockIndex])
    { offset=0;
      blockIndex++;
      if (i!=this->NumCols-1)
        out << "\\hline";
    }
  }
  out << "\\end{array}\\right)";
  return out.str();
}

template <typename Element>
inline void Matrix<Element>::ElementToString(std::string& output, bool useHtml, bool useLatex)const
{ std::stringstream out;
  std::string tempS;
  if (useHtml)
    out << "<table>";
  if (useLatex)
  { out << "\\left(\\begin{array}{";
    for (int j=0; j<this->NumCols; j++)
      out << "c";
    out << "}";
  }
  for (int i=0; i<this->NumRows; i++)
  { if (useHtml)
      out << "<tr>";
    for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j].ElementToString(tempS);
      if (useHtml)
        out << "<td>";
      out << tempS;
      if (useLatex)
        if (j!=this->NumCols-1)
          out << " & ";
      if (useHtml)
        out << "</td>";
      if (!useHtml && !useLatex)
        out << ", \t ";
    }
    if (useHtml)
      out << "</tr>";
    if (useLatex)
      out << "\\\\";
    out << "\n";
  }
  if (useHtml)
    out << "</table>";
  if (useLatex)
    out << "\\end{array}\\right)";
  output = out.str();
}

template<typename Element>
void Matrix<Element>::FindZeroEigenSpacE(List<List<Element> >& output, const Element& theRingUnit, const Element& theRingMinusUnit, const Element& theRingZero, GlobalVariables& theGlobalVariables)
{ Matrix<Element> tempMat;
  Matrix<Element> emptyMat;
  tempMat.Assign(*this);
  Selection nonPivotPts;
  tempMat.GaussianEliminationByRows(tempMat, emptyMat, nonPivotPts);
  output.SetSize(nonPivotPts.CardinalitySelection);
  Element tempElt;
  for (int i=0; i<nonPivotPts.CardinalitySelection; i++)
  { List<Element>& current= output.TheObjects[i];
    current.initFillInObject(this->NumCols, theRingZero);
    int currentPivotIndex = nonPivotPts.elements[i];
    current.TheObjects[currentPivotIndex]=theRingUnit;
    int rowCounter=0;
    for (int j=0; j<this->NumCols; j++)
      if (!nonPivotPts.selected[j])
      { tempElt=tempMat.elements[rowCounter][currentPivotIndex];
        tempElt.operator*=(theRingMinusUnit);
        current.TheObjects[j]=tempElt;
        rowCounter++;
      }
  }
}

template<typename Element>
void Matrix<Element>::AssignDirectSum(Matrix<Element>& m1, Matrix<Element>& m2)
{ assert(this!=&m1 && this!=&m2);
  this->Resize(m1.NumRows+m2.NumRows, m1.NumCols+m2.NumCols, false);
  this->NullifyAll();
  for(int i=0; i<m1.NumRows; i++)
    for(int j=0; j<m1.NumCols; j++)
      this->elements[i][j]=m1.elements[i][j];
  for(int i=0; i<m2.NumRows; i++)
    for(int j=0; j<m2.NumCols; j++)
      this->elements[i+m1.NumRows][j+m1.NumCols]=m2.elements[i][j];
}

template<typename Element>
void Matrix<Element>::DirectSumWith(const Matrix<Element>& m2, const Element& theRingZero)
{ int oldNumRows=this->NumRows; int oldNumCols=this->NumCols;
  this->Resize(this->NumRows+m2.NumRows, this->NumCols+m2.NumCols, true);
  for(int i=0; i<m2.NumRows; i++)
  { for(int j=0; j<m2.NumCols; j++)
      this->elements[i+oldNumRows][j+oldNumCols]=m2.elements[i][j];
    for(int j=0; j<oldNumCols; j++)
      this->elements[i+oldNumRows][j]=theRingZero;
  }
  for(int j=0; j<oldNumRows; j++)
    for (int i=oldNumCols; i<this->NumCols; i++)
      this->elements[j][i]=theRingZero;
}

template <typename Element>
inline void Matrix<Element>::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

template <typename Element>
inline int Matrix<Element>::FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex)
{ for(int i = RowStartIndex; i<= RowEndIndex; i++)
    if (!this->elements[i][columnIndex].IsEqualToZero())
      return i;
  return -1;
}

template <typename Element>
inline void Matrix<Element>::AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar)
{ Element tempElement;
  for (int i = StartColIndex; i< this->NumCols; i++)
  { tempElement=this->elements[fromRowIndex][i];
    tempElement*=scalar;
    this->elements[ToRowIndex][i]+=tempElement;
  }
}

template <typename Element>
inline void Matrix<Element>::SubtractRows(int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const Element& scalar)
{ Element tempElement;
  for (int i = StartColIndex; i< this->NumCols; i++)
  { tempElement=this->elements[indexSubtracted][i];
    tempElement*=scalar;
    this->elements[indexRowWeSubtractFrom][i]-=tempElement;
  }
}

template <typename Element>
inline void Matrix<Element>::RowTimesScalar(int rowIndex, const Element& scalar)
{ for (int i=0; i<this->NumCols; i++)
    this->elements[rowIndex][i]*=(scalar);
}

template <typename Element>
inline void Matrix<Element>::SwitchTwoRows(int row1, int row2)
{ if (row1==row2)
    return;
  Element tempElement;
  for(int i = 0; i<this->NumCols; i++)
  { tempElement=(this->elements[row1][i]);
    this->elements[row1][i]=(this->elements[row2][i]);
    this->elements[row2][i]=(tempElement);
  }
}

template <typename Element>
bool Matrix<Element>::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output)
{ assert(A.NumRows== b.NumRows);
  Selection thePivotPoints;
  Matrix<Element>::GaussianEliminationByRows(A, b, thePivotPoints, false);
  return A.RowEchelonFormToLinearSystemSolution( thePivotPoints, b, output);
}

template <typename Element>
bool Matrix<Element>::RowEchelonFormToLinearSystemSolution(Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide, Matrix<Element>& outputSolution)
{ assert(inputPivotPoints.MaxSize==this->NumCols && inputRightHandSide.NumCols==1 && inputRightHandSide.NumRows==this->NumRows);
  //this->ComputeDebugString();
  //inputRightHandSide.ComputeDebugString();
  //inputPivotPoints.ComputeDebugString();
  outputSolution.init(this->NumCols, 1);
  int NumPivots=0;
  for (int i=0; i<this->NumCols; i++)
    if (inputPivotPoints.selected[i])
    { outputSolution.elements[i][0].Assign(inputRightHandSide.elements[NumPivots][0]);
      NumPivots++;
    }
    else
      outputSolution.elements[i][0].MakeZero();
  for (int i=NumPivots; i<this->NumRows; i++)
    if (!inputRightHandSide.elements[i][0].IsEqualToZero())
      return false;
  return true;
}

template <typename Element>
void Matrix<Element>::GaussianEliminationByRowsNoRowSwapPivotPointsByRows
(int firstNonProcessedRow, Matrix<Element>& output, List<int>& outputPivotPointCols, Selection* outputNonPivotPoints__WarningSelectionNotInitialized)
{ outputPivotPointCols.SetSize(this->NumRows);
  Element tempElement;
  for (int i=firstNonProcessedRow; i<this->numRows; i++)
  { int currentPivotCol=-1;
    for (int j=0; j<this->NumCols; j++)
      if (!this->elements[i][j].IsEqualToZero())
      { currentPivotCol=j;
        break;
      }
    outputPivotPointCols.TheObjects[i]=currentPivotCol;
    if (currentPivotCol!=-1)
    { tempElement=this->elements[i][currentPivotCol];
      tempElement.Invert();
      this->RowTimesScalar(i, tempElement);
      for (int j=0; j<this->NumRows; j++)
        if (i!=j)
          if (!this->elements[j][i].IsEqualToZero())
          { tempElement.Assign(this->elements[j][i]);
            this->SubtractRows(j, i, 0, tempElement);
          }
    }
  }
  if (outputNonPivotPoints__WarningSelectionNotInitialized!=0)
  { for (int i=0; i<this->NumCols; i++)
      outputNonPivotPoints__WarningSelectionNotInitialized->selected[i]=true;
    for (int i=0; i<this->NumRows; i++)
      if (outputPivotPointCols.TheObjects[i]!=-1)
        outputNonPivotPoints__WarningSelectionNotInitialized->selected[outputPivotPointCols.TheObjects[i]]=false;
    outputNonPivotPoints__WarningSelectionNotInitialized->ComputeIndicesFromSelection();
  }
}

template <typename Element>
void Matrix<Element>::GaussianEliminationByRows(Matrix<Element>& mat, Matrix<Element>& output, Selection& outputSelection, bool returnNonPivotPoints)
{ int tempI;
  int NumFoundPivots = 0;
  int MaxRankMat = MathRoutines::Minimum(mat.NumRows, mat.NumCols);
  Element tempElement;
  outputSelection.init(mat.NumCols);
  //mat.ComputeDebugString();
  for (int i=0; i<mat.NumCols; i++)
  { if (NumFoundPivots == MaxRankMat)
    { if (returnNonPivotPoints)
        for (int j =i; j<mat.NumCols; j++)
          outputSelection.AddSelectionAppendNewIndex(j);
      return;
    }
    tempI = mat.FindPivot(i, NumFoundPivots, mat.NumRows - 1);
    if (tempI!=-1)
    { if (tempI!=NumFoundPivots)
      { mat.SwitchTwoRows(NumFoundPivots, tempI);
        output.SwitchTwoRows (NumFoundPivots, tempI);
      }
      tempElement=mat.elements[NumFoundPivots][i];
//      std::string tempS;
//      if (i==9 && Matrix<Element>::flagAnErrorHasOccurredTimeToPanic)
//      { std::cout << tempS;
//      }
//      tempS=tempElement.ElementToString();
      tempElement.Invert();
//      assert(tempElement.checkConsistency());
//      tempS=tempElement.ElementToString();
      mat.RowTimesScalar(NumFoundPivots, tempElement);
      output.RowTimesScalar(NumFoundPivots, tempElement);
//      assert(tempElement.checkConsistency());
      for (int j = 0; j<mat.NumRows; j++)
        if (j!=NumFoundPivots)
          if (!mat.elements[j][i].IsEqualToZero())
          { //if (i==9 && j==8 && Matrix<Element>::flagAnErrorHasOccurredTimeToPanic==true)
            //{ tempS=tempElement.ElementToString();
              //mat.ComputeDebugString();
            //}
            tempElement=(mat.elements[j][i]);
            tempElement.Minus();
            //if (i==9 && j==8 && Matrix<Element>::flagAnErrorHasOccurredTimeToPanic==true)
              //tempS=tempElement.ElementToString();
            mat.AddTwoRows(NumFoundPivots, j, i, tempElement);
            output.AddTwoRows(NumFoundPivots, j, 0, tempElement);
            //assert(tempElement.checkConsistency());
            //mat.ComputeDebugString();
          }
      NumFoundPivots++;
      if (!returnNonPivotPoints)
        outputSelection.AddSelectionAppendNewIndex(i);
    }
    else
      if (returnNonPivotPoints)
        outputSelection.AddSelectionAppendNewIndex(i);
    //mat.ComputeDebugString();
  }
}

template <typename Element>
MatrixElementaryTightMemoryFit<Element>::~MatrixElementaryTightMemoryFit()
{ this->Free();
}

template <typename Element>
inline void Matrix<Element>::NullifyAll()
{ for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j].MakeZero();
}

template <typename Element>
inline void Matrix<Element>::NullifyAll(const Element& theRingZero)
{ for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j]=theRingZero;
}

template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::init(int r, int c)
{ this->Resize(r, c, false);
}

template <typename Element>
inline void MatrixElementaryTightMemoryFit<Element>::Resize(int r, int c, bool PreserveValues)
{ if (r==this->NumRows && c== this->NumCols)
    return;
  if (r<=0)
  { this->Free();
    return;
  }
  Element** newElements= new Element*[r];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=r;
  ParallelComputing::CheckPointerCounters();
#endif
  for (int i=0; i<r; i++)
  { newElements[i]= new Element[c];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=c;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  if (PreserveValues)
    for (int j=MathRoutines::Minimum(this->NumRows, r)-1; j>=0; j--)
      for (int i=MathRoutines::Minimum(this->NumCols, c)-1; i>=0; i--)
        newElements[j][i]= this->elements[j][i];
  this->Free();
  this->NumCols= c;
  this->NumRows=r;
  this->elements = newElements;
}

class LargeIntUnsigned: public List<unsigned int>
{ void AddNoFitSize(const LargeIntUnsigned& x);
public:
  //The zero element is assumed to have length one array with a zero entry.
  //
  //CarryOverBound is the "base" over which we work.
  //Requirements on the CarryOverBound:
  //1.  CarryOverBound*2-1 must fit inside an unsigned (int)
  //    on the system
  //2. (CarryOverBound*2)^2-1 must fit inside (long long)
  //    on the system.
  ////////////////////////////////////////////////////////
  //On a 32 bit machine any number smaller than or equal to 2^31 will work.
  //If you got no clue what to put just leave CarryOverBound= 2^31.
  //static const unsigned int CarryOverBound=37;
  void SubtractSmallerPositive(const LargeIntUnsigned& x);
//  static const unsigned int CarryOverBound=2147483648UL; //=2^31
  static const unsigned int CarryOverBound=1000000000UL;
  //The following must be less than or equal to the square root of CarryOverBound.
  //It is used for quick multiplication of Rational-s.
//  static const int SquareRootOfCarryOverBound=32768; //=2^15
  static const int SquareRootOfCarryOverBound=31000; //=2^15
  void ElementToString(std::string& output)const;
  void ElementToStringLargeElementDecimal(std::string& output)const;
  inline std::string ElementToString()const {std::string tempS; this->ElementToString(tempS); return tempS;}
  void DivPositive(const LargeIntUnsigned& x, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const;
  void MakeOne();
  void Add(const LargeIntUnsigned& x);
  void AddUInt(unsigned int x){static LargeIntUnsigned tempI; tempI.AssignShiftedUInt(x, 0); this->Add(tempI); }
  void MakeZero();
  bool IsEqualToZero()const{return this->size==1 && this->TheObjects[0]==0; }
  bool IsEqualTo(const LargeIntUnsigned& right)const;
  bool IsPositive()const{ return this->size>1 || this->TheObjects[0]>0; }
  bool IsEqualToOne()const{ return this->size==1 && this->TheObjects[0]==1; }
  bool IsGEQ(const LargeIntUnsigned& x)const;
  static void GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(unsigned int n, List<unsigned int>& output)
  { List<int> theSieve;
    theSieve.initFillInObject(n+1,1);
    output.MakeActualSizeAtLeastExpandOnTop(n/2);
    output.size=0;
    for (unsigned int i=2; i<=n; i++)
      if (theSieve.TheObjects[i]!=0)
      { output.AddOnTop(i);
//        std::cout << i << ",";
        for (unsigned int j=i; j<=n; j+=i)
          theSieve.TheObjects[j]=0;
      }
  }
  static void gcd(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output);
  static void lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output)
  { LargeIntUnsigned tempUI, tempUI2;
    LargeIntUnsigned::gcd(a, b, tempUI);
    a.MultiplyBy(b, tempUI2);
    output.Assign(tempUI2);
    output.DivPositive(tempUI, output, tempUI2);
    assert(!output.IsEqualToZero());
  }
  int HashFunction()
  { int numCycles=MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
    int result=0;
    for (int i=0; i<numCycles; i++)
      result+=this->TheObjects[i]*SomeRandomPrimes[i];
    return result;
  }
  void MultiplyBy(const LargeIntUnsigned& right);
  inline void operator*=(const LargeIntUnsigned& right){this->MultiplyBy(right);}
  inline void operator*=(unsigned int x){this->MultiplyByUInt(x);}
  inline void operator+=(unsigned int x){this->AddUInt(x);}
  void AssignFactorial(unsigned int x){ this->AssignFactorial(x, 0);}
  void AssignFactorial(unsigned int x, GlobalVariables* theGlobalVariables);

  void MultiplyBy(const LargeIntUnsigned& x, LargeIntUnsigned& output)const;
  void MultiplyByUInt(unsigned int x);
  void AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift);
  void AssignShiftedUInt(unsigned int x, int shift);
  inline void Assign(const LargeIntUnsigned& x){this->CopyFromBase(x); }
  void AssignString(const std::string& input);
  int GetUnsignedIntValueTruncated();
  int operator%(unsigned int x);
  inline void operator=(const LargeIntUnsigned& x){ this->Assign(x); }
  inline void operator=(unsigned int x){ this->AssignShiftedUInt(x,0); }
  inline void operator+=(const LargeIntUnsigned& other){this->Add(other);}
  LargeIntUnsigned operator/(unsigned int x)const;
  LargeIntUnsigned operator/(const LargeIntUnsigned& x)const;
  LargeIntUnsigned(const LargeIntUnsigned& x){ this->Assign(x); ParallelComputing::SafePointDontCallMeFromDestructors();}
  LargeIntUnsigned(){this->SetSize(1); this->TheObjects[0]=0; ParallelComputing::SafePointDontCallMeFromDestructors(); }
//  LargeIntUnsigned(unsigned int value){this->operator=(value); }
//  LargeIntUnsigned(unsigned int x) {this->AssignShiftedUInt(x,0);}
  static inline LargeIntUnsigned GetOne()
  { LargeIntUnsigned tempI;
    tempI.MakeOne();
    return tempI;
  }
  inline bool operator<(const LargeIntUnsigned& other)const{return !this->IsGEQ(other);}
  //must be rewritten:
  double GetDoubleValue();
  void FitSize();
};

class LargeInt
{ friend class Rational;
  friend LargeInt operator*(const LargeInt& left, const LargeInt& right)
  { LargeInt tempI; tempI=left; tempI*=right; return tempI;
  }
  friend LargeInt operator*(const LargeInt& left, const LargeIntUnsigned& right)
  { LargeInt tempI; tempI=right; return left*tempI;
  }
public:
  inline static const std::string GetXMLClassName(){ return "LInt";}
  signed char sign;
  LargeIntUnsigned value;
  void MultiplyBy(const LargeInt& x){ this->sign*=x.sign; this->value.MultiplyBy(x.value); }
  void MultiplyByInt(int x);
  void ElementToString(std::string& output)const;
  inline std::string ElementToString()const{std::string tempS; this->ElementToString(tempS); return tempS;}
  bool IsPositive()const{return this->sign==1 && (this->value.IsPositive()); }
  bool IsNegative()const{return this->sign==-1&& (this->value.IsPositive()); }
  inline bool BeginsWithMinus(){return this->IsNegative();}
  inline bool IsNonNegative()const{return !this->IsNegative(); }
  inline bool IsNonPositive()const{return !this->IsPositive(); }
  bool IsEqualTo(const LargeInt& x)const;
  bool IsEqualToZero()const{ return this->value.IsEqualToZero(); }
  bool IsEqualToOne()const{ return this->value.IsEqualToOne() && this->sign==1; }
  void Assign(const LargeInt& x);
  void AssignLargeIntUnsigned(const LargeIntUnsigned& x){this->value.Assign(x); this->sign=1;}
  void AssignInt(int x);
  void Add(const LargeInt& x);
  void AddLargeIntUnsigned(LargeIntUnsigned& x);
  inline void AddInt(int x) {LargeInt tempInt; tempInt.AssignInt(x); this->Add(tempInt);}
  //bool IsGEQ(LargeInt& x);
  bool CheckForConsistensy();
  void WriteToFile(std::fstream& output);
  void AssignString(const std::string& input);
  void ReadFromFile(std::fstream& input);
  void MakeZero();
  void MakeOne(){this->value.MakeOne(); this->sign=1; }
  void MakeMOne(){this->value.MakeOne(); this->sign=-1;}
  int HashFunction()
  { return this->value.HashFunction()+this->sign+3;
  }
  int GetIntValueTruncated(){return this->sign* this->value.GetUnsignedIntValueTruncated(); }
  double GetDoubleValue();
  int operator %(int x);
  inline void AssignFloor(){}
  inline void operator=(const LargeInt& x){ this->Assign(x);}
  inline void operator=(const LargeIntUnsigned& other) {this->value=other; this->sign=1;}
  inline void operator*=(const LargeInt& other){this->MultiplyBy(other);}
  inline void operator*=(int x){ this->MultiplyByInt(x);}
  inline void operator+=(int x){this->AddInt(x);}
  inline void Minus(){if (!this->IsEqualToZero()) this->sign*=-1;}
  inline void operator+=(const LargeInt& other){ this->Add(other);}
  bool operator==(const LargeInt& other)const{return this->IsEqualTo(other);}
  inline void operator-=(const LargeInt& other)
  { this->Minus();
    this->Add(other);
    this->Minus();
  }
  inline bool operator<=(const LargeInt& other) const{ return ! (other<*this);}
  inline bool operator<(const LargeInt& other)const
  { if (other.IsNonNegative())
    { if (this->IsNonNegative())
        return this->value<other.value;
      return true;
    }
    if (this->IsNegative())
      return other.value< this->value;
    return false;
  }
  inline LargeInt operator+(const LargeInt& other)const{LargeInt tempInt; tempInt.Assign(*this); tempInt.Add(other); return tempInt;}
  inline LargeInt operator+(int x)const{LargeInt tempInt; tempInt.Assign(*this); tempInt.AddInt(x); return tempInt;}
  LargeInt operator*(int x){ LargeInt tempInt; tempInt.Assign(*this); tempInt.MultiplyByInt(x); return tempInt;}
  LargeInt operator/(int x)const;
  LargeInt operator/(LargeInt& x)const;
  void operator/=(const LargeInt& other)
  { if (this->IsEqualToZero())
      return;
    this->sign*=other.sign;
    LargeIntUnsigned quotient, remainder;
    this->value.DivPositive(other.value, quotient, remainder);
    this->value=quotient;
  }
  LargeInt(const LargeInt& x){this->Assign(x);}
  LargeInt(int x){this->AssignInt(x); }
  LargeInt(){this->sign=1; }
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

Rational operator-(const Rational& argument);
Rational operator/(int left, const Rational& right);
class Rational
{ friend Rational operator-(const Rational& argument);
  friend Rational operator/(int left, const Rational& right);
  friend std::iostream& operator<< (std::iostream& output, const Rational& theRat)
  { output << theRat.ElementToString();
    return output;
  }
  inline bool TryToAddQuickly(int OtherNum, int OtherDen)
  { register int OtherNumAbs, thisNumAbs;
    assert(this->DenShort>0 && OtherDen>0);
    if (OtherNum<0)
      OtherNumAbs=-OtherNum;
    else
      OtherNumAbs=OtherNum;
    if (this->NumShort<0)
      thisNumAbs=-this->NumShort;
    else
      thisNumAbs=this->NumShort;
    if (!this->flagMinorRoutinesOnDontUseFullPrecision && (this->Extended!=0 || thisNumAbs>= LargeIntUnsigned::SquareRootOfCarryOverBound  || this->DenShort>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherNumAbs>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherDen>=LargeIntUnsigned::SquareRootOfCarryOverBound)  )
      return false;
    register int N= this->NumShort*OtherDen+this->DenShort*OtherNum;
    register int D= this->DenShort*OtherDen;
    if (N==0)
    { this->NumShort=0;
      this->DenShort=1;
      return true;
    }
    register int tempGCD;
    if (N>0)
      tempGCD= Rational::gcd(N, D);
    else
      tempGCD= Rational::gcd(-N, D);
    this->NumShort=(N/tempGCD);
    this->DenShort= (D/tempGCD);
    return true;
  }
  inline bool TryToMultiplyQuickly(int OtherNum, int OtherDen)
  { register int OtherNumAbs, thisNumAbs;
    assert(this->DenShort>0 && OtherDen>0);
    if (OtherNum<0)
      OtherNumAbs=-OtherNum;
    else
      OtherNumAbs=OtherNum;
    if (this->NumShort<0)
      thisNumAbs=-this->NumShort;
    else
      thisNumAbs=this->NumShort;
    if (!this->flagMinorRoutinesOnDontUseFullPrecision &&(this->Extended!=0 || thisNumAbs>= LargeIntUnsigned::SquareRootOfCarryOverBound || this->DenShort>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherNumAbs>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherDen   >=LargeIntUnsigned::SquareRootOfCarryOverBound)  )
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
        tempGCD= Rational::gcd(N, D);
      else
        tempGCD= Rational::gcd(-N, D);
      this->NumShort= (N/((signed int)tempGCD));
      this->DenShort= (D/tempGCD);
    }
    return true;
  }
  void AllocateExtended()
  { if (this->Extended!=0)
      return;
    this->Extended= new LargeRationalExtended;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  bool InitExtendedFromShortIfNeeded()
  { if (this->Extended!=0)
      return false;
    this->Extended= new LargeRationalExtended;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
    this->Extended->den.AssignShiftedUInt((unsigned int)this->DenShort, 0);
    this->Extended->num.AssignInt(this->NumShort);
    return true;
  }
  inline void FreeExtended()
  { if (this->Extended==0)
      return;
    delete this->Extended; this->Extended=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  bool ShrinkExtendedPartIfPossible()
  { if (this->Extended==0)
      return true;
    if (this->Extended->num.value.size>1 || this->Extended->den.size>1 || this->Extended->num.value.TheObjects[0]>=(unsigned int) LargeIntUnsigned::SquareRootOfCarryOverBound || this->Extended->den.TheObjects[0]>= (unsigned int)  LargeIntUnsigned::SquareRootOfCarryOverBound)
      return false;
    this->NumShort= this->Extended->num.GetIntValueTruncated();
    this->DenShort= this->Extended->den.GetUnsignedIntValueTruncated();
    this->FreeExtended();
    return true;
  }
  //Rational(const Rational& right);
  //grrr the below function is not needed for the gcc but needed for the MS compiler
  //that is bull(on MS's part), one shouldn't need to call an extra copy constructor.
public:
  inline static std::string GetXMLClassName(){ return "Rational";}
  int NumShort;
  //the requirement that the below be unsigned caused a huge problem, so I
  //changed it back to int. Grrrrr.
  int DenShort;
  LargeRationalExtended *Extended;
  void GetDen(Rational& output)
  { LargeIntUnsigned tempInt;
    this->GetDen(tempInt);
    output.AssignLargeIntUnsigned(tempInt);
  }
  bool ElementToStringNeedsBracketsForMultiplication()const
  { return false;
    //return this->IsNegative();
  }
  inline void GetDen(LargeIntUnsigned& output)
  { if (this->Extended==0)
    { unsigned int tempI= (unsigned int) this->DenShort;
      output.AssignShiftedUInt(tempI, 0);
    }
    else
      output.Assign(this->Extended->den);
  }
  bool BeginsWithMinus(){ return this->IsNegative();}
  inline void GetNum(LargeInt& output)
  { this->GetNumUnsigned(output.value);
    output.sign=1;
    if (this->IsNegative())
      output.sign=-1;
  }
  inline void GetNumUnsigned(LargeIntUnsigned& output)
  { if (this->Extended==0)
    { if (this->NumShort<0)
        output.AssignShiftedUInt((unsigned int)(-this->NumShort), 0);
      else
        output.AssignShiftedUInt((unsigned int) this->NumShort, 0);
    }
    else
      output.Assign(this->Extended->num.value);
  }
//  inline int GetNumValueTruncated(){return this->NumShort; };
//  inline int GetDenValueTruncated(){return this->denShort; };
  static bool flagMinorRoutinesOnDontUseFullPrecision;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void Subtract(const Rational& r);
  void AdD(const Rational& r){this->operator+=(r);}
  void AddInteger(int x);
  void AssignLargeIntUnsigned(const LargeIntUnsigned& other)
  { LargeInt tempInt;
    tempInt.AssignLargeIntUnsigned(other);
    this->AssignLargeInteger(tempInt);
  }
  void AssignLargeInteger(const LargeInt& other)
  { if (this->Extended==0)
    { this->Extended= new LargeRationalExtended;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
    }
    this->Extended->num.Assign(other);
    this->Extended->den.MakeOne();
    this->ShrinkExtendedPartIfPossible();
  }
  void AssignString(const std::string& input);
  void AssignFracValue();
  void AssignFloor()
  { Rational tempRat=*this;
    tempRat.AssignFracValue();
    *this-=tempRat;
  }
  void MultiplyBy(const Rational& r);
  int HashFunction() const
  { if (this->Extended==0)
      return this->NumShort*SomeRandomPrimes[0]+this->DenShort*::SomeRandomPrimes[1];
    return this->Extended->num.HashFunction()*SomeRandomPrimes[0]+this->Extended->den.HashFunction()*SomeRandomPrimes[1];
  }
  inline void ComputeDebugString(){}
  //void MultiplyByLargeRational(int num, int den);
  void MultiplyByInt(int x);
  void MultiplyByLargeInt(LargeInt& x);
  void MultiplyByLargeIntUnsigned(LargeIntUnsigned& x);
  void Assign(const Rational& r);
  void AssignInteger(int x);
  bool IsInteger()const;
  bool IsSmallInteger()const{return (this->Extended==0)&& this->DenShort==1; }
  bool IsSmallInteger(int& whichInteger)const
  { if (this->Extended!=0)
      return false;
    if (this->DenShort!=1)
      return false;
    whichInteger=this->NumShort;
    return true;
  }
  bool IsGreaterThan(const Rational& r) const;
  inline void AssignNumeratorAndDenominator(int n, int d)
  { if (d<0)
    { d=-d;
      n=-n;
    }
    this->NumShort=n; this->DenShort=d; this->FreeExtended(); this->Simplify();
  }
  void DivideBy(const Rational& r);
  void DivideByInteger(int x)
  { int tempDen; signed char tempSign;
    if (x<0) {tempDen=(-x); tempSign=-1; } else {tempDen=x; tempSign=1; }
    if (this->TryToMultiplyQuickly(tempSign, tempDen))
      return;
    this->InitExtendedFromShortIfNeeded(); this->Extended->den.MultiplyByUInt(((unsigned int)tempDen));
    this->Extended->num.sign*=tempSign; this->Simplify();
  }
  void DivideByLargeInteger(LargeInt& x)
  { this->InitExtendedFromShortIfNeeded(); this->Extended->den.MultiplyBy(x.value);
    this->Extended->num.sign*=x.sign; this->Simplify();
  }
  void DivideByLargeIntegerUnsigned(LargeIntUnsigned& x){ this->InitExtendedFromShortIfNeeded(); this->Extended->den.MultiplyBy(x); this->Simplify(); }
  void ElementToString(std::string& output)const;
  std::string ElementToString()const{ std::string tempS; this->ElementToString(tempS); return tempS;}
  std::string ElementToString(const PolynomialOutputFormat& notUsed)const
  { std::string tempS;
    this->ElementToString(tempS);
    return tempS;
  }
  bool IsEqualTo(const Rational& r) const;
  bool IsGreaterThanOrEqualTo(const Rational& right)const;
  inline bool IsEqualToOne()const
  { if (this->Extended==0)
      return (this->NumShort==1 && this->DenShort==1);
    else
      return (this->Extended->num.IsEqualToOne() && this->Extended->den.IsEqualToOne());
  }
  inline bool IsEqualToZero()const
  { if (this->Extended==0)
      return this->NumShort==0;
    else
      return this->Extended->num.IsEqualToZero();
  }
  inline bool operator<=(const Rational& other)const{ return !(other<*this);}
  inline bool IsNonNegative()
  { if (this->Extended==0)
      return this->NumShort>=0;
    else
      return this->Extended->num.IsNonNegative();
  }
  bool IsNegative()const
  { if (this->Extended==0)
      return this->NumShort<0;
    else
      return this->Extended->num.IsNegative();
  }
  bool IsNonPositive()
  { if (this->Extended==0)
      return this->NumShort<=0;
    else
      return this->Extended->num.IsNonPositive();
  }
  bool IsPositive()const
  { if (this->Extended==0)
      return this->NumShort>0;
    else
      return this->Extended->num.IsPositive();
  }
  void Simplify();
  void Invert();
  void Minus()
  { if (this->Extended==0)
      this->NumShort*=-1;
    else
      this->Extended->num.sign*=-1;
  }
  double DoubleValue()const;
  int floorIfSmall()
  { if (this->Extended==0)
    { if (NumShort<0)
      { if (DenShort!=1)
          return (this->NumShort/ this->DenShort)-1;
        else
          return this->NumShort/ this->DenShort;
      } else
        return this->NumShort/this->DenShort;
    }
    assert(false);
    return -1;
  }
  void MakeZero(){this->NumShort=0;  this->DenShort=1; this->FreeExtended(); }
  void MakeOne(){this->NumShort=1;  this->DenShort=1; this->FreeExtended(); }
  void MakeMOne(){this->NumShort=-1; this->DenShort=1; this->FreeExtended(); }
  void WriteToFile(std::fstream& output);
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output);}
  void ReadFromFile(std::fstream& input);
  inline void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){this->ReadFromFile(input);}
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData);
  inline void AssignAbsoluteValue(){if(this->IsNegative())this->Minus(); }
  static Rational NChooseK(int n, int k);
  static Rational Factorial(int n, GlobalVariables* theGlobalVariables);
  static Rational Factorial(int n){return Rational::Factorial(n,0);}
  static Rational TwoToTheNth(int n);
  static Rational NtoTheKth(int n, int k);
  void RaiseToPower(int x);
  static Rational TheRingUnit;
  static Rational TheRingZero;
  static Rational TheRingMUnit;
  //don't ever call the below manually or you can get memory leak (extended must be nullified here by
  //default!
  Rational(int n, int d){this->Extended=0; this->AssignNumeratorAndDenominator(n, d); ParallelComputing::SafePointDontCallMeFromDestructors();}
  Rational(){this->Extended=0; ParallelComputing::SafePointDontCallMeFromDestructors();}
  Rational(int n){this->Extended=0; this->AssignNumeratorAndDenominator(n, 1); ParallelComputing::SafePointDontCallMeFromDestructors(); }
  Rational(const Rational& right){this->Extended=0; this->Assign(right); ParallelComputing::SafePointDontCallMeFromDestructors();}
//  Rational(int x){this->Extended=0; this->AssignInteger(x); };
  ~Rational(){this->FreeExtended();}
  //the below must be called only with positive arguments!
  static int gcd(int a, int b)
  { int temp;
    while(b>0)
    { temp= a % b;
      a=b;
      b=temp;
    }
    return a;
  }
  static int gcdSigned(int a, int b){if (a<0) {a*=-1; } if (b<0){b*=-1; } return Rational::gcd(a, b); }
  inline bool CheckForElementSanity(){ return this->Extended==0;}
  inline bool checkConsistency(){ return this->ConsistencyCheck();}
  inline bool ConsistencyCheck()
  { if (this->Extended==0)
      return this->DenShort>0;
    return true;
  }
  inline void operator=(const LargeIntUnsigned& right){LargeInt tempI; tempI=right; this->operator=(tempI); }
  inline void operator=(const Rational& right){this->Assign(right); }
  inline bool operator==(const Rational& right)const{return this->IsEqualTo(right); }
  inline void operator+=(const Rational& r)
  { //static std::string tempS1, tempS2, tempS3, tempS4, tempS5, tempS6, tempS7;
    if (r.Extended==0 && this->Extended==0)
      if (this->TryToAddQuickly(r.NumShort, r.DenShort))
        return;
    if (this==&r)
    { this->MultiplyByInt(2);
      return;
    }
    this->InitExtendedFromShortIfNeeded();
    Rational tempRat;
    tempRat.Assign(r);
    tempRat.InitExtendedFromShortIfNeeded();
    LargeInt tempI;
    tempI.Assign(tempRat.Extended->num);
    tempI.value.MultiplyBy(this->Extended->den);
    this->Extended->num.value.MultiplyBy(tempRat.Extended->den);
    this->Extended->num.Add(tempI);
    this->Extended->den.MultiplyBy(tempRat.Extended->den);
    this->Simplify();
  }
  inline void operator-=(const Rational& right){this->Subtract(right); }
  inline void operator*=(const Rational& right){this->MultiplyBy(right); }
  inline void operator/=(const Rational& right){this->DivideBy(right);}
  inline void operator/=(const LargeIntUnsigned& right){Rational tempRat; tempRat=right; this->DivideBy(tempRat);}
  inline void operator+=(int right){this->AddInteger(right); }
  inline void operator-=(int right){Rational tempRat=right; this->Subtract(tempRat);}
//  inline bool operator==(int right){Rational tempRat; tempRat.AssignInteger(right); return this->IsEqualTo(tempRat); }
  inline void operator=(int right){this->AssignInteger(right); }
  inline void operator=(const LargeInt& other){this->AssignLargeInteger(other);}
  Rational operator*(const Rational& right)const;
  Rational operator*(int right)const{ Rational tempRat; tempRat.Assign(*this); tempRat.MultiplyByInt(right); return tempRat;}
  Rational operator/(int right)const{ Rational tempRat; tempRat.Assign(*this); tempRat.DivideByInteger(right); return tempRat;}
  root operator*(const root& right)const;
  Rational operator+(const Rational& right)const;
  Rational operator-(const Rational& right)const;
  Rational operator/(const Rational& right)const;
  bool operator!=(const Rational& right)const{ return !this->IsEqualTo(right);}
  bool operator!=(const int& right)const{ return !((*this)==right);}
  inline bool operator>(const Rational& right)const{return this->IsGreaterThan(right); }
  inline bool operator<(const Rational& right)const{return right.IsGreaterThan(*this); }
  inline bool operator>(const int right)const{Rational tempRat; tempRat.AssignInteger(right); return this->IsGreaterThan(tempRat); }
  inline bool operator>=(const Rational& right)const{return this->IsGreaterThanOrEqualTo(right); }
  inline bool operator<(const int right)const{Rational tempRat; tempRat.AssignInteger(right); return tempRat.IsGreaterThan(*this); }
};

template<class CoefficientType>
Vector<CoefficientType> operator-(const Vector<CoefficientType>& left, const Vector<CoefficientType>& right)
{ Vector<CoefficientType> result;
  result.SetSize(left.size);
  for (int i=0; i<left.size; i++)
  { result.TheObjects[i]=left.TheObjects[i];
    result.TheObjects[i]-=right.TheObjects[i];
  }
  return result;
}

template <class CoefficientType>
class Vector: public ListLight<CoefficientType>
{
//To M$: that YOU can't inline template functions is YOUR compiler's problem, not a problem of
//my code!!!!!
//#pragma warning(disable:4396)
  friend Vector<CoefficientType> operator-<CoefficientType>(const Vector<CoefficientType>& left, const Vector<CoefficientType>& right);
//#pragma warning(default:4396)
public:
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(this->DebugString);}
  Vector(){}
  Vector(const Vector<CoefficientType>& other){*this=other;}
  void ElementToString(std::string& output)
  { std::stringstream out;
    out.precision(5);
    out << "(";
    for(int i=0; i<this->size; i++)
    { out << this->TheObjects[i];
      if (i!=this->size-1)
        out << ", ";
    }
    out << ")";
    output=out.str();
  }
  void ElementToString(std::string& output, PolynomialOutputFormat& theFormat)
  { std::stringstream out;
    out.precision(5);
    out << "(";
    for(int i=0; i<this->size; i++)
    { out << this->TheObjects[i].ElementToString(theFormat);
      if (i!=this->size-1)
        out << ", ";
    }
    out << ")";
    output=out.str();
  }
  std::string ElementToString(){ std::string tempS; this->ElementToString(tempS); return tempS; }
  std::string ElementToString
  (PolynomialOutputFormat& theFormat){ std::string tempS; this->ElementToString(tempS, theFormat); return tempS; }

  std::string ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex, bool DontIncludeLastVar);

  void ElementToStringEpsilonForm(std::string& output, bool useLatex, bool useHtml);
  CoefficientType ScalarEuclidean(const Vector<CoefficientType>& other, const CoefficientType& theRingZero) const
  { CoefficientType result, tempElt;
    assert(this->size==other.size);
    result=theRingZero;
    for (int i=0; i<this->size; i++)
    { tempElt=other.TheObjects[i];
      tempElt*=this->TheObjects[i];
      result+=tempElt;
    }
    return result;
  }
  CoefficientType ScalarEuclidean(const Vector<CoefficientType>& other) const
  { return this->ScalarEuclidean(other, (CoefficientType) 0);
  }
  void MakeEi
  (int DesiredDimension, int NonZeroIndex,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  { this->MakeZero(DesiredDimension, theRingZero);
    this->TheObjects[NonZeroIndex]=theRingUnit;
  }
  int HashFunction() const
  { int result=0;
    int theSize= MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
    for (int i=0; i<theSize; i++)
      result+=  this->TheObjects[i].HashFunction()*  ::SomeRandomPrimes[i];
    return result;
  }
  void MakeAffineUsingLastCoordinate()
  { CoefficientType theElt;
    theElt=*this->LastObject();
    this->size--;
    this->operator/=(theElt);
  }
  bool AssignMatDetectRowOrColumn(const Matrix<CoefficientType>& input)
  { if (input.NumCols==1)
    { this->SetSize(input.NumRows);
      for (int i=0; i<this->size; i++)
        this->TheObjects[i]=input.elements[i][0];
      return true;
    }
    if (input.NumRows==1)
    { this->SetSize(input.NumCols);
      for (int i=0; i<this->size; i++)
        this->TheObjects[i]=input.elements[0][i];
      return true;
    }
    return false;
  }
  void ElementToString(std::string& output, bool useLaTeX)
  { output.clear();
    std::string tempStr;
    output.append("(");
    for(int i=0; i<this->size; i++)
    { this->TheObjects[i].ElementToString(tempStr);
      output.append(tempStr);
      if (i!=this->size-1)
        output.append(", ");
    }
    output.append(")");
  }
  void MakeZero(int theDim, const CoefficientType& theRingZero=0)
  { this->SetSize(theDim);
    for (int i=0; i<theDim; i++)
      this->TheObjects[i]=theRingZero;
  }
  int GetNumNonZeroCoords()const
  { int result=0;
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
        result++;
    return result;
  }
  inline Vector<CoefficientType> GetShiftToTheLeftOnePosition(){return this->GetShiftToTheLeft(1);}
  Vector<CoefficientType> GetShiftToTheLeft(int numPositions)
  { Vector<CoefficientType> result;
    assert(numPositions<=this->size);
    result.SetSize(this->size-numPositions);
    for (int i=0; i<result.size; i++)
      result[i]=this->TheObjects[i+numPositions];
    return result;
  }
  inline void ShiftToTheLeftOnePos(){ this->ShiftToTheLeft(1); }
  void ShiftToTheLeft(int numPositions)
  { assert(numPositions<=this->size);
    for (int i=0; i<this->size-numPositions; i++)
      this->TheObjects[i]=this->TheObjects[i+numPositions];
    this->size-=numPositions;
  }
  void ShiftToTheRightInsertZeroes(int numPositions, const CoefficientType& theRingZero)
  { assert(numPositions>=0);
    this->SetSize(this->size+numPositions);
    for (int i=this->size-1; i>=numPositions; i--)
      this->TheObjects[i]=this->TheObjects[i-numPositions];
    for(int i=0; i<numPositions; i++)
      this->TheObjects[i]=theRingZero;
  }
  bool GetIntegralCoordsInBasisIfTheyExist
(const Vectors<CoefficientType>& inputBasis, Vector<CoefficientType>& output,
  Matrix<CoefficientType>& bufferMatGaussianEliminationCC,
  Matrix<CoefficientType>& bufferMatGaussianElimination,
  const CoefficientType& theRingUnit,
  const CoefficientType& theRingMinusUnit,
  const CoefficientType& theRingZero)
  ;
  template<class OtherType>
  void operator=(const Vector<OtherType>& other)
  { this->SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->TheObjects[i]=other.TheObjects[i];
  }
  void GetVectorsPerpendicularTo(Vectors<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
  { int Pivot=-1;
    if (!this->FindIndexFirstNonZeroCoordinateFromTheLeft(Pivot))
    { output.MakeEiBasis(this->size, theRingUnit, theRingZero);
      return;
    }
    output.SetSize(this->size-1);
    for (int i=0; i<this->size; i++)
      if (i!=Pivot)
      { Vector<CoefficientType>& current=output.TheObjects[i];
        current.MakeEi(this->size, i, theRingUnit, theRingZero);
        current.TheObjects[Pivot]-=this->TheObjects[i];
      }
  }
  bool FindIndexFirstNonZeroCoordinateFromTheLeft(int& theIndex)
  { theIndex=-1;
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
      { theIndex=i;
        return true;
      }
    return false;
  }
  bool GetCoordsInBasiS
  (const Vectors<CoefficientType>& inputBasis, Vector<CoefficientType>& output,
  Vectors<CoefficientType>& bufferVectors, Matrix<CoefficientType>& bufferMat,
  const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
   ;
  bool GetCoordsInBasiS
(const Vectors<CoefficientType>& inputBasis, Vector<CoefficientType>& output,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  { Vectors<CoefficientType> buffer;
    Matrix<CoefficientType> matBuffer;
    return this->GetCoordsInBasiS(inputBasis, output, buffer, matBuffer, theRingUnit, theRingZero);
  }
  Vector<CoefficientType> GetProjectivizedNormal(Vector<CoefficientType>& affinePoint);
  Vector<CoefficientType> operator*(const CoefficientType& other)
  { Vector<CoefficientType> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
    { result.TheObjects[i]=this->TheObjects[i];
      result.TheObjects[i]*=other;
    }
    return result;
  }
  void operator*=(const CoefficientType& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]*=other;
  }
  void operator/=(const CoefficientType& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]/=other;
  }
  void operator+=(const Vector<CoefficientType>& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]+=other.TheObjects[i];
  }
  inline bool operator>(const Vector<CoefficientType>& other)const
  { assert(this->size==other.size);
    CoefficientType c1=0, c2=0;
    for (int i=0; i<this->size; i++)
    { c1+=this->TheObjects[i];
      c2+=other.TheObjects[i];
    }
    if (c1>c2)
      return true;
    if (c2>c1)
      return false;
    for (int i=this->size-1; i>=0; i--)
    { if (this->TheObjects[i]>other.TheObjects[i])
        return true;
      if (other.TheObjects[i]>this->TheObjects[i])
        return false;
    }
    return false;
  }
  void operator-=(const Vector<CoefficientType>& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]-=other.TheObjects[i];
  }
  Vector<CoefficientType> operator+(const Vector<CoefficientType>& right)
  { Vector<CoefficientType> result=*this;
    result+=right;
    return result;
  }
  void operator=(const List<CoefficientType>& other)
  { this->SetSize(other.size);
    for(int i=0; i<other.size; i++)
      this->TheObjects[i]=other.TheObjects[i];
  }
  void operator=(const Selection& other)
  { this->SetSize(other.MaxSize);
    for(int i=0; i<other.MaxSize; i++)
      if (other.selected[i])
        this->TheObjects[i]=1;
      else
        this->TheObjects[i]=0;
  }
  bool IsEqualToZero()const
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
};

template <class CoefficientType>
class Vectors: public List<Vector<CoefficientType> >
{
  public:
  std::string DebugString;
  std::string ElementToString()const{return this->ElementToString(false, false, false);}
  std::string ElementToString(bool useLaTeX, bool useHtml, bool makeTable)const
  { std::stringstream out;
    std::string tempS;
    if (! useLaTeX && ! useHtml)
      out << "Num roots: " << this->size << "\n";
    if (useLaTeX && makeTable)
      out << "\\begin{tabular}{c}";
    if (useHtml && makeTable)
      out << "<table>";
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].ElementToString(tempS, useLaTeX);
      if (useHtml&& makeTable)
        out << "<tr><td>";
      out << tempS;
      if (!makeTable && i!=this->size-1)
        out << ", ";
      if (useLaTeX && makeTable)
        out << "\\\\\n";
      if (useHtml && makeTable)
        out << "</td></tr>";
    }
    if (useHtml&& makeTable)
      out << "</table>";
    if (useLaTeX && makeTable)
      out << "\\end{tabular}";
    return out.str();
  }
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  bool LinearAlgebraForVertexComputation(Selection& theSelection, Vector<CoefficientType>& output, Matrix<CoefficientType>& buffer, Selection& NonPivotPointsBuffer)
  { if (this->size==0)
      return false;
    int theDimension=this->TheObjects[0].size;
    output.SetSize(theDimension);
    assert(theDimension -1== theSelection.CardinalitySelection);
    Matrix<CoefficientType> matOutputEmpty;
    buffer.init((int)(theDimension-1), (int)theDimension);
    matOutputEmpty.init(-1, -1);
    for (int i =0; i<theDimension-1; i++)
      for (int j=0; j<theDimension; j++)
        buffer.elements[i][j].Assign(this->Externalwalls.TheObjects[theSelection.elements[i]].normal.TheObjects[j]);
    Matrix<CoefficientType>::GaussianEliminationByRows(buffer, matOutputEmpty, NonPivotPointsBuffer);
    if (NonPivotPointsBuffer.CardinalitySelection==1)
    { buffer.NonPivotPointsToRoot(NonPivotPointsBuffer, output);
      return true;
    }
    return false;
  }
  bool LinSpanContainsRoot
  (const Vector<CoefficientType>& input, Matrix<CoefficientType>& bufferMatrix, Selection& bufferSelection)const
  ;
  void MakeEiBasis(int theDimension, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
  { this->SetSize(theDimension);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i].MakeEi(theDimension, i, theRingUnit, theRingZero);
  }
  bool LinSpanContainsRoot(const Vector<CoefficientType>& input)const
  { Matrix<CoefficientType> buffer;
    Selection bufferSelection;
    return this->LinSpanContainsRoot(input, buffer, bufferSelection);
  }
  void SelectABasis
  (Vectors<CoefficientType>& output, const CoefficientType& theRingZero, Selection& outputSelectedIndices, GlobalVariables& theGlobalVariables)const
  ;
  int GetRankOfSpanOfElements
  (Matrix<CoefficientType>& buffer, Selection& bufferSelection)const
;

  void GetCoordsInBasis
  (const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& outputCoords,
   Vectors<CoefficientType>& bufferVectors, Matrix<CoefficientType>& bufferMat, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
  ;
  bool GetIntegralCoordsInBasisIfTheyExist
  (const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& output,
  const CoefficientType& theRingUnit,
  const CoefficientType& theRingMinusUnit,
  const CoefficientType& theRingZero)
  { Matrix<CoefficientType> bufferMatGaussianEliminationCC, bufferMatGaussianElimination;
    bool result=true;
    output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].GetIntegralCoordsInBasisIfTheyExist(inputBasis, output.TheObjects[i], bufferMatGaussianEliminationCC, bufferMatGaussianElimination, theRingUnit, theRingMinusUnit, theRingZero))
        result=false;
    return result;
  }
  bool GetIntegralCoordsInBasisIfTheyExist
  (const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& output,
  Matrix<CoefficientType>& bufferMatGaussianEliminationCC,
  Matrix<CoefficientType>& bufferMatGaussianElimination,
  const CoefficientType& theRingUnit,
  const CoefficientType& theRingMinusUnit,
  const CoefficientType& theRingZero)
  { bool result=true;
    output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].GetIntegralCoordsInBasisIfTheyExist(inputBasis, output.TheObjects[i], bufferMatGaussianEliminationCC, bufferMatGaussianElimination, theRingUnit, theRingMinusUnit, theRingZero))
        result=false;
    return result;
  }
  void GaussianEliminationForNormalComputation(Matrix<CoefficientType>& inputMatrix, Selection& outputNonPivotPoints, int theDimension)const;
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool GetLinearDependence
  (Matrix<CoefficientType>& outputTheLinearCombination, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
  ;
  void GetLinearDependenceRunTheLinearAlgebra
  (Matrix<CoefficientType>& outputTheLinearCombination, Matrix<CoefficientType>& outputTheSystem, Selection& outputNonPivotPoints)
  ;
  void AssignMatrixRows(const Matrix<CoefficientType>& mat)
  { this->size=0;
    this->SetSize(mat.NumRows);
    for (int i=0; i<mat.NumRows; i++)
    { this->TheObjects[i].SetSize(mat.NumCols);
      for (int j=0; j<mat.NumCols; j++)
        this->TheObjects[i].TheObjects[j]=mat.elements[i][j];
    }
  }
  int GetDim()
  { if (this->size>0)
      return this->TheObjects[0].size;
    return -1;
  }
  void BeefUpWithEiToLinearlyIndependentBasis
  (int theDim, const CoefficientType& ringUnit =(CoefficientType) 1, const CoefficientType& ringZero=(CoefficientType) 0)
  { Selection BufferSel;
    Matrix<CoefficientType> Buffer;
    assert(this->size==0 || theDim==this->GetDim());
    int currentRank=this->GetRankOfSpanOfElements(Buffer, BufferSel);
    if (currentRank==theDim)
      return;
    Vector<CoefficientType> theVect;
    for (int i=0; i<theDim && currentRank<theDim; i++)
    { theVect.MakeEi(theDim, i, ringUnit, ringZero);
      this->AddOnTop(theVect);
      int candidateRank=this->GetRankOfSpanOfElements(Buffer, BufferSel);
      if (candidateRank>currentRank)
        currentRank=candidateRank;
      else
        this->size--;
    }
    assert(currentRank==theDim);
  }
  static void IntersectTwoLinSpaces
  (const Vectors<CoefficientType>& firstSpace, const Vectors<CoefficientType>& secondSpace,
   Vectors<CoefficientType>& output, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
     ;
  void operator=(const List<List<CoefficientType> >& other)
  { this->SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->TheObjects[i]=other.TheObjects[i];
  }
};

class root: public Vector<Rational>
{
public:
  inline static std::string GetXMLClassName(){ return "root";}
//the below is to facilitate operator overloading
  root(const root& right){this->Assign(right); };
  root(const Vector<Rational>& other){this->CopyFromLight(other);}
  void MultiplyByLargeRational(const Rational& a);
  void MakeZero(int DesiredDimension);
  void MakeEi(int DesiredDimension, int NonZeroIndex);
  void Add(const root& r);
  int getIndexFirstNonZeroCoordinate();
  int getIndexLastNonZeroCoordinate();
  void GetHeight(Rational& output);
  void DivByInteger(int a);
  void DivByLargeInt(LargeInt& a);
  void DivByLargeIntUnsigned(LargeIntUnsigned& a);
  bool GetCoordsInBasis(roots& inputBasis, root& outputCoords, GlobalVariables& theGlobalVariables);
  inline void MakeNormalInProjectivizationFromAffineHyperplane(affineHyperplane& input);
  void MakeNormalInProjectivizationFromPointAndNormal(root& point, root& normal);
  bool MakeAffineProjectionFromNormal(affineHyperplane& output);
  //the below returns false
  bool ProjectToAffineSpace(root& output);
  bool HasStronglyPerpendicularDecompositionWRT(int UpperBoundNumBetas, roots& theSet, WeylGroup& theWeylGroup, roots& output, List<Rational>& outputCoeffs, bool IntegralCoefficientsOnly);
  void DivByLargeRational(const Rational& a);
  Rational GetHeight();
  Rational SumCoordinates()
  { Rational result=0;
    for (int i=0; i<this->size; i++)
      result+=this->TheObjects[i];
    return result;
  }
  std::string DebugString;
  void ComputeDebugString();
  bool IsNonNegative()
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i].IsNegative())
        return false;
    return true;
  }
  void ElementToString(std::string& output)const;
  std::string ElementToStringLetterFormat(const PolynomialOutputFormat& theFormat, bool useLatex, bool DontIncludeLastVar);
  std::string ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex, bool DontIncludeLastVar);
  std::string ElementToStringLetterFormat(const std::string& inputLetter){return this->ElementToStringLetterFormat(inputLetter, false, false);}
  std::string ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex){ return this->ElementToStringLetterFormat(inputLetter, useLatex, false);}
  std::string ElementToString()const{ std::string tempS; this->ElementToString(tempS); return tempS; };
  void ElementToStringEpsilonForm(std::string& output, bool useLatex, bool useHtml);
  std::string ElementToStringEpsilonForm(){std::string result; this->ElementToStringEpsilonForm(result, true, false); return result;}
  void ElementToString(std::string& output, bool useLaTeX)const;
  bool CheckForElementSanity();
  //void RootToLinPolyToString(std::string& output, PolynomialOutputFormat& PolyOutput);
  void ScaleToFirstNonZeroCoordinatePositive();
  void ScaleByPositiveRationalToIntegralMinHeight();
  void ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  void FindLCMDenominators(LargeIntUnsigned& output);
//  root(const char* input){std::string tempS=input; this->operator=(tempS);}
  std::string* ElementToStringDebuggerCallOnly();
  int FindLCMDenominatorsTruncateToInt();
  void InitFromIntegers(int Dimension, int x1, int x2, int x3, int x4, int x5);
  void InitFromIntegers(int Dimension, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8);
  void InitFromIntegers(int Dimension, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, int x10, int x11, int x12);
  void MultiplyByInteger(int a);
  void MultiplyByLargeInt(LargeInt& right);
  void MultiplyByLargeIntUnsigned(LargeIntUnsigned& right);
  bool OurScalarProductIsPositive(const root& right);
  bool OurScalarProductIsNonNegative(const root& right){return !this->OurScalarProductIsNegative(right); }
  bool OurScalarProductIsNegative(const root& right);
  bool OurScalarProductIsZero(const root& right);
  void MinusRoot();
  Rational ScalarEuclidean(const root& other)const{return this->Vector<Rational>::ScalarEuclidean(other, (Rational) 0); }
  Rational ScalarEuclidean(const root& other, const Rational& theRingZero)const{return this->ScalarEuclidean(other); }
  void Subtract(const root& r);
  void AssignWithoutLastCoordinate(root& right);
  void ReadFromFile(std::fstream& input);
  inline void ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
  { this->ReadFromFile(input);
  }
  inline void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { this->ReadFromFile(input);
  }
  void WriteToFile(std::fstream& output);
  inline void WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
  { this->WriteToFile(output);
  }
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->WriteToFile(output);
  }
  inline void Assign(const Vector<Rational>& right)
  { if (this->size!=right.size)
      this->SetSize(right.size);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i].Assign(right.TheObjects[i]);
  }
  void AssignIntRoot(intRoot& right);
  bool IsProportionalTo(root& r);
  bool IsProportionalTo(const root& input, Rational& outputTimesMeEqualsInput)const;
  bool IsPositiveOrZero() const;
  bool IsNegativeOrZero();
  bool IsIntegral()const
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsInteger())
        return false;
    return true;
  }
  bool IsEqualToZero() const
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
  bool IsGreaterThanOrEqualTo(root& r);
  bool IsEqualTo(const root& right) const;
  bool IsStronglyPerpendicularTo(root& right, WeylGroup& theWeyl);
  static void RootScalarEuclideanRoot(const root& r1, const root& r2, Rational& output);
  static inline Rational RootScalarEuclideanRoot(const root& r1, const root& r2)
  { Rational tempRat;
    root::RootScalarEuclideanRoot(r1, r2, tempRat);
    return tempRat;
  }
  static void RootScalarRoot(root& r1, root& r2, MatrixLargeRational& KillingForm, Rational& output);
//  static void RootScalarRoot(root& r1, root& r2, MatrixIntTightMemoryFit& KillingForm, Rational& output);
  static void RootPlusRootTimesScalar(root& r1, root& r2, Rational& rat, root& output);
  root(){}
  root(const std::string& input){this->AssignString(input);};
  root(const char* input){std::string tempS; tempS=input; this->AssignString(tempS);}
  void operator=(const std::string& input){this->AssignString(input);}
  void operator=(const char* input){std::string tempS; tempS=input; this->AssignString(input);}
  void operator=(const SelectionWithMultiplicities& other);
  inline void operator=(const Vector<Rational>& right){this->Assign(right); }
  inline void operator=(const Selection& other){this->::Vector<Rational>::operator=(other); }
  inline Vector<double> GetVectorDouble();
  bool AssignString(const std::string& input)
  { unsigned int startIndex=0;
    for (; startIndex<input.size(); startIndex++)
      if (input[startIndex]=='(')
        break;
    startIndex++;
    this->SetSize(0);
    Rational tempRat;
    std::string tempS;
    tempS.resize(input.size());
    tempS="";
    for (; startIndex<input.size(); startIndex++)
    { if (input[startIndex]==')' || input[startIndex]==',')
      { tempRat.AssignString(tempS);
        tempS="";
        this->AddObjectOnTopLight(tempRat);
      } else
      { //tempS.resize(tempS.size()+1);
        //tempS[tempS.size()-1]=input[startIndex];
        tempS.push_back(input[startIndex]);
      }
      if (input[startIndex]==')')
        break;
    }
    return true;
  }
  inline void operator*=(const Rational& other){this->MultiplyByLargeRational(other);}
  inline bool operator==(const root& right){return IsEqualTo(right); }
  inline root operator+(const root& right)const{ root result; result.Assign(*this); result.Add(right); return result;}
  inline root operator+(const Vector<Rational>& right)const{ root result; result.Assign(right); result.Add(*this); return result;}
  inline root operator-(const root& right)const{ root result; result.Assign(*this); result.Subtract(right); return result;}
  inline void operator+=(const root& right){ this->Add(right); }
  inline void operator-=(const root& right){ this->Subtract(right); }
  inline bool operator!=(const root& right) const{ return !this->IsEqualTo(right); }
  root operator*(int right)const{ root tempRoot; tempRoot.Assign(*this); tempRoot.MultiplyByInteger(right); return tempRoot;}
  root operator*(const Rational& right)const{ root tempRoot; tempRoot.Assign(*this); tempRoot.MultiplyByLargeRational(right); return tempRoot;}
  Rational operator*(const root& right)const{ Rational tempRat; this->RootScalarEuclideanRoot(*this, right, tempRat); return tempRat; }
  root operator/(const Rational& right)const{ root tempRoot; tempRoot.Assign(*this); tempRoot.DivByLargeRational(right); return tempRoot;}
  inline bool operator<(const root& other)const {return other.operator>(*this);}
  bool operator>(const root& other)const
  { assert(this->size==other.size);
    Rational tempRat1=0, tempRat2=0;
    for (int i=0; i<this->size; i++)
    { tempRat1+=this->TheObjects[i];
      tempRat2+=other.TheObjects[i];
    }
    if (tempRat1>tempRat2)
      return true;
    if (tempRat2>tempRat1)
      return false;
    for (int i=this->size-1; i>=0; i--)
    { if (this->TheObjects[i]>other.TheObjects[i])
        return true;
      if (other.TheObjects[i]>this->TheObjects[i])
        return false;
    }
    return false;
  }
};

inline root operator-(const root& right)
{ root tempRoot;
  tempRoot.Assign(right);
  tempRoot.MinusRoot();
  return tempRoot;
}

template<typename Element>
void Matrix<Element>::RowToRoot(int rowIndex, Vector<Element>& output)const
{ output.SetSize(this->NumCols);
  for (int i=0; i<this->NumCols; i++)
    output.TheObjects[i]=this->elements[rowIndex][i];
}

template<typename Element>
void Matrix<Element>::ColToRoot(int colIndex, Vector<Element>& output)const
{ output.SetSize(this->NumRows);
  for (int i=0; i<this->NumRows; i++)
    output[i]=this->elements[i][colIndex];
}

class roots : public List<root>
{
public:
  std::string DebugString;
  bool CheckForElementSanity();
  void AssignIntRoots(intRoots& r);
  void AssignHashedIntRoots(HashedList<intRoot>& r);
  void AssignMatrixRows(const MatrixLargeRational& mat);
  void AssignMatrixColumns(MatrixLargeRational& mat);
  void ComputeDebugString();
  int GetRankOfSpanOfElements(GlobalVariables& theGlobalVariables)const;
  void AddRoot(root& r);
  void AddIntRoot(intRoot& r);
  void GetVectorsDouble
  (Vectors<double>& output)const
  { output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
    { Vector<double>& currentOutput=output[i];
      root& current=this->TheObjects[i];
      int Dim=current.size;
      currentOutput.SetSize(Dim);
      for (int j=0; j<Dim; j++)
        currentOutput[j]=current[j].DoubleValue();
    }
  }
  void GetVectorsRational
  (Vectors<Rational>& output)const
  { output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      output.TheObjects[i].CopyFromLight(this->TheObjects[i]);
  }
  void AddRootSnoRepetition(roots& r);
  bool AddRootNoRepetition(root& r);
  bool ElementsHaveNonNegativeScalarProduct(const root& theRoot)const;
  bool ElementsHavePositiveScalarProduct(const root& theRoot)const;
  bool ElementsHaveNonPositiveScalarProduct(const root& theRoot)const;
  std::string ElementsToInequalitiesString(bool useLatex, bool useHtml, PolynomialOutputFormat& theFormat){ return this->ElementsToInequalitiesString(useLatex, useHtml, false, theFormat);}
  std::string ElementsToInequalitiesString(bool useLatex, bool useHtml, bool LastVarIsConstant, PolynomialOutputFormat& theFormat);
  bool ContainsOppositeRoots();
  bool PerturbVectorToRegular(root&output, GlobalVariables& theGlobalVariables, int theDimension);
  void GetCoordsInBasis(roots& inputBasis, roots& outputCoords, GlobalVariables& theGlobalVariables);
  void ChooseABasis(GlobalVariables& theGlobalVariables);
  void average(root& output, int theDimension);
  void sum(root& output, int theDimension);
//  void sum(root& output);
  void Pop(int index);
  void intersectWith(roots& right, roots& output);
  std::string ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex)
  { std::stringstream out;
    for (int i=0; i<this->size; i++)
    { out << this->TheObjects[i].ElementToStringLetterFormat(inputLetter, useLatex);
      if (i!=this->size-1)
        out << ",";
    }
    return out.str();
  }
  bool HasAnElementPerpendicularTo(const root& input)const
  { for (int i=0; i<this->size; i++)
      if (root::RootScalarEuclideanRoot(this->TheObjects[i], input).IsEqualToZero())
        return true;
    return false;
  }
  bool HasAnElementWithNegativeScalarProduct(const root& input)const
  { for (int i=0; i<this->size; i++)
      if (root::RootScalarEuclideanRoot(this->TheObjects[i], input).IsNegative())
        return true;
    return false;
  }
  bool HasAnElementWithPositiveScalarProduct(const root& input)const
  { for (int i=0; i<this->size; i++)
      if (root::RootScalarEuclideanRoot(this->TheObjects[i], input).IsPositive())
        return true;
    return false;
  }
  bool ContainsARootNonPerpendicularTo(root& input, WeylGroup& theWeyl);
  bool ContainsARootNonStronglyPerpendicularTo(root& input, WeylGroup& theWeyl);
  int NumRootsConnectedTo(root& input, WeylGroup& theWeyl);
  bool LinSpanContainsRoot(root& input, GlobalVariables& theGlobalVariables);
  bool IsRegular(root& r, GlobalVariables& theGlobalVariables, int theDimension);
  bool IsRegular(root& r, root& outputFailingNormal, GlobalVariables& theGlobalVariables, int theDimension);
  bool GetMinLinearDependenceWithNonZeroCoefficientForFixedIndex(MatrixLargeRational& outputTheLinearCombination, int theIndex);
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool GetLinearDependence(MatrixLargeRational& outputTheLinearCombination);
  void GetLinearDependenceRunTheLinearAlgebra(MatrixLargeRational& outputTheLinearCombination, MatrixLargeRational& outputTheSystem, Selection& outputNonPivotPoints);
  void ReadCivilizedHumanReadableFormat(std::stringstream& input);
  void AssignVectorsRational(const Vectors<Rational>& input)
  { this->SetSize(input.size);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i].CopyFromLight(input.TheObjects[i]);
  }
  static void IntersectTwoLinSpaces
  (const roots& firstSpace, const roots& secondSpace,
   roots& output,
   GlobalVariables& theGlobalVariables)
  { Vectors<Rational> firstSpaceVectors, secondSpaceVectors, outputVectors;
    firstSpace.GetVectorsRational(firstSpaceVectors);
    secondSpace.GetVectorsRational(secondSpaceVectors);
    Vectors<Rational>::IntersectTwoLinSpaces(firstSpaceVectors, secondSpaceVectors, outputVectors, (Rational) 0, theGlobalVariables);
    output.AssignVectorsRational(outputVectors);
  }

  int GetDimensionOfElements();
  //Strict cone: The zero linear combination is not allowed.
  //Non-Strict cone: the zero linear combination is allowed.
  //the below function returns true if there exists positive numbers a_i b_j and vectors v_i in the nonstrict cone and
  //w_j in the strict cone such that a_1 v_1 +... +a_k v_k= b_1 w_1+...+b_lw_l
  // The left hand side is allowed to have zero number of summands (hence "non-strict" cone - contains 0)
  //The right hand side is NOT allowed to have zero number of summands.
  //If the strict cone has zero elements, the function returns false.
  static bool ConesIntersect(GlobalVariables& theGlobalVariables,  List<root>& StrictCone, List<root>& NonStrictCone, int theDimension);
  static bool GetNormalSeparatingCones(GlobalVariables& theGlobalVariables, int theDimension, List<root>& coneStrictlyPositiveCoeffs, List<root>& coneNonNegativeCoeffs, root& outputNormal);
  void GetGramMatrix(MatrixLargeRational& output, WeylGroup& theWeyl)const;
  //the following two functions assume the first dimension vectors are the images of the
  // vectors (1, 0, ..., 0), ..., (0, ..., 0, 1)
  void MakeBasisChange(root& input, root& output)const;
  void MakeBasisChange(roots& input, roots& output)const;
  void MakeEiBasis(int theDimension);
  void GaussianEliminationForNormalComputation (MatrixLargeRational& inputMatrix, Selection& outputNonPivotPoints, int theDimension)const;
  // the below function is slow
  int ArrangeFirstVectorsBeOfMaxPossibleRank(GlobalVariables& theGlobalVariables);
  void GetMatRootsToRows(MatrixLargeRational& output)const;
  void rootsToMatrixRationalTruncate(MatrixLargeRational& output);
  void ComputeDebugStringEpsilonForm(){this->ElementToStringEpsilonForm(this->DebugString, false, false, false); }
  void ElementToLinearCombinationString(std::string& output);
  void ElementToString(std::string& output)const;
  std::string ElementToString()const{std::string tempS; this->ElementToString(tempS); return tempS;}
  std::string ElementToString(bool useLatex, bool useHtml, bool makeTable)const{std::string tempS; this->ElementToString(tempS, useLatex, useHtml, makeTable); return tempS;};
  void ElementToStringEpsilonForm(std::string& output, bool useLatex, bool useHtml, bool makeTable);
  std::string ElementToStringEpsilonForm(){std::string tempS; this->ElementToStringEpsilonForm(tempS, false, false, false); return tempS;}
  void ElementToString(std::string& output, bool useLaTeX, bool useHtml, bool makeTable)const;
  std::string* ElementToStringDebuggerCallOnly(){ this->ComputeDebugString(); return &this->DebugString;}
  //The below function is required to preserve the order of elements given by theSelection.elements.
  void SubSelection(Selection& theSelection, roots& output);
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, MatrixLargeRational& output);
  void SelectionToMatrixAppend(Selection& theSelection, int OutputDimension, MatrixLargeRational& output, int StartRowIndex);
  bool ComputeNormal(root& output, int inputDim);
  bool ComputeNormalExcludingIndex(root& output, int index, GlobalVariables& theGlobalVariables);
  bool ComputeNormalFromSelection(root& output, Selection& theSelection, GlobalVariables& theGlobalVariables, int theDimension);
  bool ComputeNormalFromSelectionAndExtraRoot(root& output, root& ExtraRoot, Selection& theSelection, GlobalVariables& theGlobalVariables);
  bool ComputeNormalFromSelectionAndTwoExtraRoots(root& output, root& ExtraRoot1, root& ExtraRoot2, Selection& theSelection, GlobalVariables& theGlobalVariables);
  bool operator==(const roots& right)const;
  void operator = (const roots& right){this->CopyFromBase(right); }
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
;
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){ return this->ReadFromFile(input, theGlobalVariables, -1);}
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output);}
  void WriteToFile(std::fstream& output);
  roots(){}
  roots(int StartingSize){ this->SetSize(StartingSize);}
};

class WallData
{
// we consider a wall that has no neighbors to be an external wall of the chamber complex
// a wall that has a 0 pointer neighbor is a bogus wall, and must be deleted.
// it is allowed since deleting bogus neighbors upon discovery means one must rewire all neighbors of the wall, which is a
// lot of operations
public:
  std::string DebugString;
  int indexInOwnerChamber;
  bool flagIsClosed;
  root normal;
  List<CombinatorialChamber*> NeighborsAlongWall;
  List<int> IndicesMirrorWalls;
  static bool flagDisplayWallDetails;
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  void ElementToString(std::string& output);
  void RemoveNeighborhoodBothSidesNoRepetitionNeighbors(CombinatorialChamber* owner, CombinatorialChamber* NeighborPointer);
  int RemoveNeighborhoodBothSidesAllowRepetitionsReturnNeighborsDecrease(CombinatorialChamber* owner, CombinatorialChamber* NeighborPointer);
  void RemoveNeighborOneSideAllowRepetitions(CombinatorialChamber* NeighborPointer);
  void RemoveNeighborOneSideNoRepetitions(CombinatorialChamber* NeighborPointer);
  void AddNeighbor(CombinatorialChamber* newNeighbor, int IndexNewNeighborWall);
  void operator=(const WallData& right);
  bool HasExactlyOneTrueNeighbor(int& outputIndex)
  { outputIndex=-1;
    for (int i=0; i<this->NeighborsAlongWall.size; i++)
      if (this->NeighborsAlongWall.TheObjects[i]!=0)
      { if (outputIndex!=-1)
          return false;
        else
          outputIndex=i;
      }
    return outputIndex!=-1;
  }
  bool IsInFacetNoBoundaries(root& point);
  bool FacetContainsChamberOnlyOnce(CombinatorialChamber* owner);
  void SubstituteNeighborOneOccurenceNeighborOnly(CombinatorialChamber* oldNeighbor, CombinatorialChamber* newNeighbor, int IndexNewNeighborWall);
  void SubstituteNeighborOneAllowNeighborAppearingNotOnce(CombinatorialChamber* oldNeighbor, CombinatorialChamber* newNeighbor, int IndexNewNeighborWall);
  bool IsExternalWithRespectToDirection(root& direction);
  int GetIndexWallToNeighbor(CombinatorialChamber* neighbor);
  inline bool ContainsPoint(root& point){return this->normal.OurScalarProductIsZero(point); }
  bool ContainsNeighborAtMostOnce(CombinatorialChamber* neighbor);
  bool ContainsNeighborExactlyOnce(CombinatorialChamber* neighbor);
  void RemoveNeighborExtraOcurrences(CombinatorialChamber* neighbor);
  bool SplitWall(int indexInOwner, List<int>& possibleBogusWallsThisSide, CombinatorialChamber* BossChamber, CombinatorialChamber* NewPlusChamber, CombinatorialChamber* NewMinusChamber, CombinatorialChamberContainer& ownerComplex, roots& ThePlusVertices, roots& TheMinusVertices, root& TheKillerFacet, root& direction, List<CombinatorialChamber*>& PossibleBogusNeighbors, List<int>& PossibleBogusWalls, GlobalVariables& theGlobalVariables);
  bool ConsistencyCheck(CombinatorialChamber& owner, CombinatorialChamberContainer& ownerComplex);
  bool EveryNeigborIsExplored(bool& allNeighborsHaveZeroPoly);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input, CombinatorialChamberContainer& owner);
  void TransformToWeylProjective
  (GlobalVariables& theGlobalVariables)
  ;
  WallData(){this->flagIsClosed=true;}
};

class affineHyperplane
{
public:
  std::string DebugString;
  root affinePoint;
  root normal;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  //void InduceFromFacet(Facet& input);
  //the below returns false if the projection is not of full dimension
  int HashFunction()const;
//  bool ProjectFromFacet(Facet& input);
  bool ProjectFromFacetNormal(root& input);
  bool ContainsPoint(root& thePoint);
  void MakeFromNormalAndPoint(root& inputPoint, root&inputNormal);
  bool HasACommonPointWithPositiveTwoToTheNth_ant();
  void Assign(const affineHyperplane& right){ this->affinePoint.Assign(right.affinePoint); this->normal.Assign(right.normal); }
  inline void operator=(const affineHyperplane& right){ this->Assign(right); }
  bool operator==(const affineHyperplane& right);
};

class affineHyperplanes: public List<affineHyperplane>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
};

class affineCone
{
public:
  affineHyperplanes theWalls;
  int HashFunction() const;
  inline int GetDimension();
  void SuperimposeAffineCones(affineCones& theOtherComplex);
  void ProjectFromCombinatorialChamber(CombinatorialChamber& input);
  //void induceFromCombinatorialChamber(CombinatorialChamber& input);
  bool WallIsInternalInCone(affineHyperplane& theKillerCandidate);
  //The below function returns true if the system of homogeneous linear inequalities Ax<=b
  //has a solution, false otherwise, where A is a matrix and x and b are column vectors.
//  bool SystemLinearInequalitiesHasSolution
//    (MatrixLargeRational& matA, MatrixLargeRational& matb, MatrixLargeRational& outputPoint);
  bool SplitByAffineHyperplane(affineHyperplane& theKillerPlane, affineCones& output);
  void Assign(const affineCone& right){this->theWalls.CopyFromBase(right.theWalls); }
  inline void operator=(const affineCone& right){this->Assign(right); }
};

class affineCones: public HashedList<affineCone>
{
public:
  void SuperimposeAffineCones(affineCones& theOtherComplex);
  void ProjectFromCombinatorialChambers(CombinatorialChamberContainer& input);
};

class CombinatorialChamber
{
public:
  std::string DebugString;
  bool flagHasZeroPolynomiaL;
  bool flagExplored;
  bool flagPermanentlyZero;
  bool flagNormalsAreSorted;
  //QuasiPolynomialOld* ThePolynomial;
  int CreationNumber;
  int DisplayNumber;
  int IndexInOwnerComplex;
  List<WallData> Externalwalls;
  roots ExternalwallsNormalsSorted;
  List<int> IndicesCorrespondingNonSortedWalls;
  roots InternalWalls;
  roots AllVertices;
  roots affineVertices;
  affineHyperplanes affineExternalWalls;
  root InternalPoint;
  int IndexStartingCrossSectionNormal;
  int NumTrueAffineVertices;
  static bool flagIncludeVerticesInDebugString;
  static bool flagAnErrorHasOccurredTimeToPanic;
  static bool flagPrintWallDetails;
//  static bool flagMakingASingleHyperplaneSlice;
  bool PointLiesInMoreThanOneWall(root& point);
  bool PointIsAVertex(const root& point, GlobalVariables& theGlobalVariables);
  //bool InduceFromAffineCone(affineCone& input);
  void SortNormals();
  bool GetSplittingFacet(root& output, CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  bool ComputeDebugString(CombinatorialChamberContainer& owner);
  bool ElementToString(std::string& output, CombinatorialChamberContainer& owner, PolynomialOutputFormat& PolyFormatLocal);
  bool ElementToString(std::string& output, CombinatorialChamberContainer& owner, bool useLatex, bool useHtml, PolynomialOutputFormat& PolyFormatLocal);
  void ElementToInequalitiesString(std::string& output, CombinatorialChamberContainer& owner, bool useLatex, bool useHtml, PolynomialOutputFormat& PolyFormatLocal);
  void ChamberNumberToString(std::string& output, CombinatorialChamberContainer& owner);
  bool ConsistencyCheck(int theDimension, bool checkVertices, CombinatorialChamberContainer& ownerComplex, GlobalVariables& theGlobalVariables);
  //bool FacetIsInternal(Facet* f);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, CombinatorialChamberContainer& owner);
  void LabelWallIndicesProperly();
  void GetWallNormals(roots& output);
  int GetHashFromSortedNormals();
  int getIndexInfiniteHyperplane(CombinatorialChamberContainer& owner);
  int getIndexVertexIncidentWithSelection(Selection& theSel);
  int GetIndexWallWithNormal(root& theNormal);
  bool ChambersHaveNonZeroMeasureIntersection(CombinatorialChamber& other, GlobalVariables& theGlobalVariables);
  bool ChambersHaveNonZeroMeasureIntersection(roots& WallsOther, GlobalVariables& theGlobalVariables);
  bool VertexIsIncidentWithSelection(root& VertexCandidate, Selection& theSel);
  bool IsSeparatedByStartingConesFrom(CombinatorialChamberContainer& owner, CombinatorialChamber& Neighbor, GlobalVariables& theGlobalVariables);
  bool GetNonSeparableChamberIndicesOrReturnFalseIfUnionNotConvex(CombinatorialChamberContainer& owner, List<int>& outputIndicesChambersToGlue, roots& outputRedundantNormals, GlobalVariables& theGlobalVariables);
  bool GetNonSeparableChamberIndicesAppendList(CombinatorialChamberContainer& owner, List<int>& outputIndicesChambersToGlue, GlobalVariables& theGlobalVariables);
  bool UnionAlongWallIsConvex(CombinatorialChamber& other, int indexWall, GlobalVariables& theGlobalVariables);
  void ReplaceMeByAddExtraWallsToNewChamber(CombinatorialChamberContainer& owner, CombinatorialChamber* newChamber, List<int>& IndicesGluedChambers, roots& redundantNormals);
  void FindAllNeighbors(ListPointers<CombinatorialChamber>& TheNeighbors);
  bool SplitChamber(root& theKillerPlaneNormal, CombinatorialChamberContainer& output, root& direction, GlobalVariables& theGlobalVariables);
  bool IsABogusNeighbor(WallData& NeighborWall, CombinatorialChamber* Neighbor, CombinatorialChamberContainer& ownerComplex, GlobalVariables& theGlobalVariables);
  bool IsABogusNeighborUseStoredVertices(roots& relevantVerticesCurrentWall, WallData& NeighborWall, CombinatorialChamber* Neighbor, CombinatorialChamberContainer& ownerComplex, GlobalVariables& theGlobalVariables);
  void ComputeVerticesFromNormals(CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  bool ComputeVertexFromSelection(GlobalVariables& theGlobalVariables, root& output, Selection& theSel, int theDimension);
  void ComputeHyperplanesCurrentDirection(GlobalVariables& theGlobalVariables);
  void Triangulate(CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  void TransformToWeylProjective
  (CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables)
  ;
  //the below function returns false if the cross-section affine walls have been modified
  //and aborts its execution
  bool ProjectToDefaultAffineSpace(CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables);
  bool PointIsInChamber(const root& point);
  bool PointIsStrictlyInsideChamber(const root& point);
  bool PointIsOnChamberBorder(const root& point);
  bool BordersViaExternalWRTDirectionNonZeroNeighbor(const root& theDirection);
  void findWallsIncidentWithVertexExcludeWallAtInfinity(root& theVertex, Selection& output, CombinatorialChamberContainer* owner);
//  bool ScaledVertexIsInWallSelection(root &point, Selection& theSelection);
  bool ScaleVertexToFitCrossSection(root& point, CombinatorialChamberContainer& owner);
  void ComputeAffineInfinityPointApproximation(Selection& selectedExternalWalls, CombinatorialChamberContainer* owner, GlobalVariables& theGlobalVariables);
  bool PointIsInWallSelection(root& point, Selection& theSelection);
  bool PlusMinusPointIsInChamberModifyInput(root& point);
  bool ExtraPointRemovesDoubtForBogusWall(MatrixLargeRational& theMatrix, MatrixLargeRational& emptyMat, Selection& bufferSel, root& theRoot);
  void PurgeInternalWalls();
  bool HasNoNeighborsThatPointToThis();
  bool HasAsNeighbor(CombinatorialChamber* candidateNeighbor);
  bool LinearAlgebraForVertexComputation(Selection& theSelection, root& output, GlobalVariables& theGlobalVariables, int theDimension);
  bool LinearAlgebraForVertexComputationOneAffinePlane(Selection& theSelection, root& output, GlobalVariables& theGlobalVariables, CombinatorialChamberContainer* owner);
  //returns false if the vectors were linearly dependent
  bool SliceInDirection(root& direction, roots& directions, int CurrentIndex, CombinatorialChamberContainer& output, HashedList<root>& FacetOutput, GlobalVariables& theGlobalVariables);
  void PropagateSlicingWallThroughNonExploredNeighbors(root& theKillerNormal, rootsCollection& CuttingPlaneVertices, CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  //the below function will automatically add the candidate to the
  //list of used hyperplanes if the candidate is an allowed one
  bool IsAValidCandidateForNormalOfAKillerFacet(root& normalCandidate, roots& directions, int CurrentIndex, CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  bool HasHPositiveAndHNegativeVertex(root& h);
  bool CheckSplittingPointCandidate(Selection& SelectionTargetSimplex, Selection& SelectionStartSimplex, MatrixLargeRational& outputColumn, int Dimension);
  void AddInternalWall(root& TheKillerFacetNormal, root& TheFacetBeingKilledNormal, root& direction, CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
//  void InduceFromAffineConeAddExtraDimension(affineCone& input);
  void InduceFromCombinatorialChamberLowerDimensionNoAdjacencyInfo(CombinatorialChamber& input, CombinatorialChamberContainer& owner);
  void ComputeInternalPoint(root& InternalPoint, int theDimension);
  void ComputeAffineInternalPoint(root& outputPoint, int theDimension);
  bool OwnsAWall(WallData* theWall);
  void CheckForAndRemoveBogusNeighbors(CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  void MakeNewMutualNeighbors(CombinatorialChamber* NewPlusChamber, CombinatorialChamber* NewMinusChamber, root& normal);
  bool TestPossibilityToSlice(root& direction, CombinatorialChamberContainer& owner);
  bool MakeFacetFromEdgeAndDirection(WallData& Wall1, WallData& Wall2, CombinatorialChamberContainer& owner, root& direction, roots & directions, int CurrentIndex, root& outputNormal, GlobalVariables& theGlobalVariables);
  void drawOutputAffine(DrawingVariables& TDV, CombinatorialChamberContainer& owner, std::fstream* LaTeXoutput);
  void WireChamberAndWallAdjacencyData(CombinatorialChamberContainer& owner, CombinatorialChamber* input);
  void Assign(const  CombinatorialChamber& right);
//  void GetStylesAndColors(int& theTextStyle, int& theTextColor, int& thePenStyle, int& thePenStyleLinkToOrigin, int& thePenColor);
  inline void operator=(const CombinatorialChamber& right){this->Assign(right); }
  bool operator==(const CombinatorialChamber& right)const;
  bool operator<(CombinatorialChamber& right);
  bool operator>(CombinatorialChamber& right);
  CombinatorialChamber();
//  ~CombinatorialChamber(){this->IndexInOwnerComplex=-2; };
};

template <class Object>
void List<Object>::QuickSortAscending(int BottomIndex, int TopIndex)
{ if (TopIndex<=BottomIndex)
    return;
  int HighIndex = TopIndex;
  for (int LowIndex = BottomIndex+1; LowIndex<=HighIndex; LowIndex++)
    if (this->TheObjects[LowIndex]>(this->TheObjects[BottomIndex]))
    { this->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex--;
      HighIndex--;
    }
  if (this->TheObjects[HighIndex]>this->TheObjects[BottomIndex])
    HighIndex--;
  this->SwapTwoIndices(BottomIndex, HighIndex);
  this->QuickSortAscending(BottomIndex, HighIndex-1);
  this->QuickSortAscending(HighIndex+1, TopIndex);
}

template <class Object>
void ListPointers<Object>::QuickSortAscending()
{ this->QuickSortAscending(0, this->size-1);
}

template <class Object>
void ListPointers<Object>::QuickSortAscending(int BottomIndex, int TopIndex)
{ if (TopIndex<=BottomIndex)
    return;
  int LowIndex = BottomIndex+1;
  int HighIndex = TopIndex;
  for (; LowIndex<=HighIndex; LowIndex++)
    if (this->TheObjects[LowIndex]->operator>(*this->TheObjects[BottomIndex]))
    { this->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex--;
      HighIndex--;
    }
  if (this->TheObjects[HighIndex]->operator>(*this->TheObjects[BottomIndex]))
    HighIndex--;
  this->SwapTwoIndices(BottomIndex, HighIndex);
  this->QuickSortAscending(BottomIndex, HighIndex-1);
  this->QuickSortAscending(HighIndex+1, TopIndex);
}

template <class Object>
void List<Object>::AddListOnTop(const List<Object>& theList)
{ int oldsize= this->size;
  int otherSize=theList.size;
  this->SetSize(oldsize+otherSize);
  for (int i=0; i<otherSize; i++)
    this->TheObjects[i+oldsize]= theList.TheObjects[i];
}

template<class Object>
void List<Object>::SwapTwoIndices(int index1, int index2)
{ if (index1==index2)
    return;
  Object tempO;
  tempO= this->TheObjects[index1];
  this->TheObjects[index1]=this->TheObjects[index2];
  this->TheObjects[index2]=tempO;
}

template<class Object>
int List<Object>::IndexOfObject(const Object& o) const
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]==o)
      return i;
  return -1;
}

template <class Object>
void List<Object>::swap(List<Object>& l1, List<Object>& l2)
{ List<Object>* bigL;
  List<Object>* smallL;
  int smallSize;
  if (l1.size<l2.size)
  { smallL=&l1;
    bigL=&l2;
    smallSize=l1.size;
  } else
  { bigL=&l1;
    smallL=&l2;
    smallSize=l2.size;
  }
  smallL->SetSize(bigL->size);
  Object tempO;
  for(int i=0; i<smallSize; i++)
  { tempO=smallL->TheObjects[i];
    smallL->TheObjects[i]=bigL->TheObjects[i];
    bigL->TheObjects[i]=tempO;
  }
  for(int i=smallSize; i<bigL->size; i++)
    smallL->TheObjects[i]=bigL->TheObjects[i];
  bigL->size=smallSize;
}

template <class Object>
int List<Object>::SizeWithoutObjects()
{ return  sizeof(this->ActualSize)+ sizeof(this->IndexOfVirtualZero)+ sizeof(this->size)+ sizeof(this->TheActualObjects)+ sizeof(this->TheObjects);
}

template <class Object>
bool List<Object>::operator>(const List<Object>& other)const
{ if (this->size>other.size)
    return true;
  if (other.size>this->size)
    return false;
  for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i]>other.TheObjects[i])
      return true;
    if (other.TheObjects[i]>this->TheObjects[i])
      return false;
  }
  return false;
}

template <class Object>
void List<Object>::ShiftUpExpandOnTop(int StartingIndex)
{ this->SetSize(this->size+1);
  for (int i=this->size-1; i>StartingIndex; i--)
    this->TheObjects[i]= this->TheObjects[i-1];
}

template <class Object>
void List<Object>::initFillInObject(int theSize, const Object& o)
{ this->SetSize(theSize);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i]=o;
}

template <class Object>
bool  List<Object>::AddOnTopNoRepetition(const Object& o)
{ if (this->IndexOfObject(o)!=-1)
    return false;
  this->AddOnTop(o);
  return true;
}

template <class Object>
inline Object* List<Object>::LastObject()
{ return &this->TheObjects[this->size-1];
}

template <class Object>
bool List<Object>::HasACommonElementWith(List<Object>& right)
{ for(int i=0; i<this->size; i++)
    for (int j=0; j<right.size; j++)
      if (this->TheObjects[i]==right.TheObjects[j])
        return true;
  return false;
}

template <class Object>
void List<Object>::AssignLight(const ListLight<Object>& From)
{ this->SetSize(From.size);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i]= From.TheObjects[i];
}

template <class Object>
void List<Object>::CopyFromBase(const List<Object>& From)
{ if (this==&From)
    return;
  this->SetSize(From.size);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i]= From.TheObjects[i];
}

template <class Object>
void List<Object>::MakeActualSizeAtLeastExpandOnTop(int theSize)
{ if (!(this->ActualSize>= this->IndexOfVirtualZero+theSize))
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    this->ExpandArrayOnTop(this->IndexOfVirtualZero+theSize- this->ActualSize);
  }
}

template <class Object>
void List<Object>::RemoveFirstOccurenceSwapWithLast(const Object& o)
{ for (int i=0; i<this->size; i++)
    if (o==this->TheObjects[i])
    { this->PopIndexSwapWithLast(i);
      return;
    }
}

template <class Object>
void List<Object>::SetSize(int theSize)
{ if (theSize<0)
    theSize=0;
  this->MakeActualSizeAtLeastExpandOnTop(theSize);
  this->size=theSize;
}

template <class Object>
inline void List<Object>::ElementToStringGeneric(std::string& output)const
{ this->ElementToStringGeneric(output, this->size);
}

template <class Object>
inline void List<Object>::ElementToStringGeneric(std::string& output, int NumElementsToPrint)const
{ std::stringstream out; std::string tempS;
  int Upper=MathRoutines::Minimum(NumElementsToPrint, this->size);
  for (int i=0; i<Upper; i++)
  { this->TheObjects[i].ElementToString(tempS);
    out << tempS << "\n";
  }
  output= out.str();
}

template <class Object>
inline void List<Object>::AddObjectOnTopCreateNew()
{ this->SetSize(this->size+1);
}

template <class Object>
void List<Object>::PopIndexShiftUp(int index)
{ if (size==0)
    return;
  this->size--;
  for (int i=index; i>=1; i--)
    this->TheObjects[i]=this->TheObjects[i-1];
  this->TheObjects++;
  IndexOfVirtualZero++;
}

template <class Object>
void List<Object>::PopIndexSwapWithLast(int index)
{ if (this->size==0)
    return;
  this->size--;
  this->TheObjects[index]=this->TheObjects[this->size];
}

template <class Object>
void List<Object>::PopLastObject()
{ if (this->size==0)
    return;
  this->size--;
}

template <class Object>
List<Object>::List()
{ this->initConstructorCallOnly();
}

template <class Object>
List<Object>::List(int StartingSize)
{ this->initConstructorCallOnly();
  this->SetSize(StartingSize);
}

template <class Object>
List<Object>::List(int StartingSize, const Object& fillInObject)
{ this->initConstructorCallOnly();
  this->initFillInObject(StartingSize, fillInObject);
}

template <class Object>
void List<Object>::ReleaseMemory()
{ delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->ActualSize=0;
  this->IndexOfVirtualZero=0;
  this->size=0;
  this->TheObjects=0;
  this->TheActualObjects=0;
}

template <class Object>
List<Object>::~List()
{ delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  this->ActualSize=0;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheActualObjects=0;
  this->TheObjects=0;
}

template <class Object>
void List<Object>::ExpandArrayOnBottom(int increase)
{ if (increase<=0)
    return;
  Object* newArray = new Object[this->ActualSize+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->ActualSize+increase;
  ParallelComputing::CheckPointerCounters();
#endif
  for (int i=0; i<this->size; i++)
    newArray[i+increase+this->IndexOfVirtualZero]=this->TheObjects[i];
  delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheActualObjects= newArray;
  this->ActualSize+=increase;
  this->IndexOfVirtualZero+=increase;
  this->TheObjects = this->TheActualObjects+this->IndexOfVirtualZero;
}

template <class Object>
void List<Object>::ExpandArrayOnTop(int increase)
{ if (increase<=0)
    return;
  Object* newArray = new Object[this->ActualSize+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->ActualSize+increase;
  ParallelComputing::CheckPointerCounters();
#endif
  for (int i=0; i<this->size; i++)
    newArray[i+this->IndexOfVirtualZero]=this->TheObjects[i];
  delete [] this->TheActualObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheActualObjects= newArray;
  this->ActualSize+=increase;
  this->TheObjects = this->TheActualObjects+this->IndexOfVirtualZero;
}

template <class Object>
void List<Object>::ReverseOrderElements()
{ int tempI= this->size/2;
  for (int i=0; i<tempI; i++)
    this->SwapTwoIndices(i, this->size-i-1);
}

template <class Object>
void List<Object>::AddObjectOnBottom(const Object& o)
{ if (this->IndexOfVirtualZero==0)
    this->ExpandArrayOnBottom(List<Object>::ListActualSizeIncrement);
  this->IndexOfVirtualZero--;
  this->TheObjects--;
  this->TheObjects[0]=o;
  this->size++;
}

template <class Object>
void List<Object>::AddOnTop(const Object& o)
{ if (this->IndexOfVirtualZero+this->size>=this->ActualSize)
    this->ExpandArrayOnTop(List<Object>::ListActualSizeIncrement);
  this->TheObjects[size]=o;
  this->size++;
}

template <class Object>
void HashedList<Object>::SwapTwoIndicesHash(int i1, int i2)
{ Object tempO;
  int i1Hash = this->TheObjects[i1].HashFunction();
  int i2Hash = this->TheObjects[i2].HashFunction();
  this->FitHashSize(i1Hash); this->FitHashSize(i2Hash);
  this->TheHashedArrays[i1Hash].RemoveFirstOccurenceSwapWithLast(i1);
  this->TheHashedArrays[i2Hash].RemoveFirstOccurenceSwapWithLast(i2);
  tempO= this->TheObjects[i1];
  this->TheObjects[i1]=this->TheObjects[i2];
  this->TheObjects[i2]=tempO;
  this->TheHashedArrays[i1Hash].AddOnTop(i2);
  this->TheHashedArrays[i2Hash].AddOnTop(i1);
}

template <class Object>
int HashedList<Object>::SizeWithoutObjects()
{ int Accum=0;
  Accum+=this->List<Object>::SizeWithoutObjects();
  Accum+=sizeof(this->TheHashedArrays)*this->HashSize;
  Accum+=sizeof(this->HashSize);
  for (int i=0; i<this->HashSize; i++)
    Accum+=this->TheHashedArrays[i].SizeWithoutObjects();
  return Accum;
}

template <class Object>
void HashedList<Object>::CopyFromHash(const HashedList<Object>& From)
{ if (&From==this){return; }
  this->ClearHashes();
  this->SetHashSizE(From.HashSize);
  this->::List<Object>::CopyFromBase(From);
  if (this->size<this->HashSize)
    for (int i=0; i<this->size; i++)
    { int hashIndex= this->TheObjects[i].HashFunction()% this->HashSize;
      if (hashIndex<0)
        hashIndex+=this->HashSize;
      this->TheHashedArrays[hashIndex].CopyFromBase(From.TheHashedArrays[hashIndex]);
    }
  else
    for (int i=0; i<this->HashSize; i++)
      this->TheHashedArrays[i].CopyFromBase(From.TheHashedArrays[i]);
}

template <class Object>
void HashedList<Object>::ClearHashes()
{ if (this->size<this->HashSize)
    for (int i=0; i<this->size; i++)
    { int hashIndex=this->TheObjects[i].HashFunction()%this->HashSize;
      if (hashIndex<0)
        hashIndex+=this->HashSize;
      this->TheHashedArrays[hashIndex].size=0;
    }
  else
    for (int i=0; i<this->HashSize; i++)
      this->TheHashedArrays[i].size=0;
}

template <class Object>
void HashedList<Object>::ClearTheObjects()
{ this->ClearHashes();
  this->size=0;
}

template <class Object>
void HashedList<Object>::initHash()
{ this->size=0;
  for (int i=0; i<this->HashSize; i++)
    this->TheHashedArrays[i].size=0;
}

template <class Object>
int HashedList<Object>::IndexOfObjectHash(const Object& o)const
{ int hashIndex = o.HashFunction()%this->HashSize;
  if (hashIndex<0)
    hashIndex+=this->HashSize;
  for (int i=0; i<this->TheHashedArrays[hashIndex].size; i++)
  { int j=this->TheHashedArrays[hashIndex].TheObjects[i];
    if(this->TheObjects[j]==o)
      return j;
  }
  return -1;
}

template <class Object>
void HashedList<Object>::AddOnTopHash(const Object& o)
{ int hashIndex = o.HashFunction()% this->HashSize;
  if (hashIndex<0)
    hashIndex+=this->HashSize;
  this->TheHashedArrays[hashIndex].AddOnTop(this->size);
  this->::List<Object>::AddOnTop(o);
}

template <class Object>
void HashedList<Object>::AddOnTopNoRepetitionHash(const List<Object>& theList)
{ this->MakeActualSizeAtLeastExpandOnTop(this->size+theList.size);
  for (int i=0; i<theList.size; i++)
    this->AddOnTopNoRepetitionHash(theList.TheObjects[i]);
}

template <class Object>
bool HashedList<Object>::AddOnTopNoRepetitionHash(const Object& o)
{ if (this->IndexOfObjectHash(o)!=-1)
    return false;
  this->AddOnTopHash(o);
  return true;
}

template <class Object>
void HashedList<Object>::PopIndexSwapWithLastHash(int index)
{ Object* oPop= &this->TheObjects[index];
  int hashIndexPop = oPop->HashFunction()% this->HashSize;
  if (hashIndexPop<0)
    hashIndexPop+=this->HashSize;
  this->TheHashedArrays[hashIndexPop].RemoveFirstOccurenceSwapWithLast(index);
  if (index==this->size-1)
  { this->size--;
    return;
  }
  int tempI=this->size-1;
  Object* oTop= &this->TheObjects[tempI];
  int hashIndexTop= oTop->HashFunction()% this->HashSize;
  if (hashIndexTop<0)
    hashIndexTop+=this->HashSize;
  this->TheHashedArrays[hashIndexTop].RemoveFirstOccurenceSwapWithLast(tempI);
  this->TheHashedArrays[hashIndexTop].AddOnTop(index);
  this->List<Object>::PopIndexSwapWithLast(index);
}

template <class Object>
void HashedList<Object>::SetHashSizE(int HS)
{ if (HS!=this->HashSize)
  { delete [] this->TheHashedArrays;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=HS-this->HashSize;
  ParallelComputing::CheckPointerCounters();
#endif
    this->TheHashedArrays= new  List<int>[HS];
    this->HashSize=HS;
    if (this->size>0)
      for (int i=0; i<this->size; i++)
      { int theIndex=this->FitHashSize(this->TheObjects[i].HashFunction());
        this->TheHashedArrays[theIndex].AddOnTop(i);
      }
  }
}

template <class Object>
HashedList<Object>::HashedList()
{ this->HashSize=0;
  this->TheHashedArrays=0;
  this->SetHashSizE(HashedList<Object>::PreferredHashSize);
  this->initHash();
}

template <class Object>
HashedList<Object>::~HashedList()
{ delete [] this->TheHashedArrays;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->HashSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->HashSize=0;
  this->TheHashedArrays=0;
}

template <class Object>
class ListPointersKillOnExit: public ListPointers<Object>
{
public:
  ~ListPointersKillOnExit(){this->KillAllElements(); };
};

template<class Object>
void ListPointers<Object>::IncreaseSizeWithZeroPointers(int increase)
{ if (increase<=0)
    return;
  if (this->ActualSize<this->size+increase)
  { Object** newArray= new Object*[this->size+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=increase;
  ParallelComputing::CheckPointerCounters();
#endif
    for (int i=0; i<this->size; i++)
      newArray[i]=this->TheObjects[i];
    delete [] this->TheObjects;
    this->TheObjects= newArray;
    this->ActualSize+=increase;
  }
  for(int i=this->size; i<this->ActualSize; i++)
    this->TheObjects[i]=0;
  this->size+=increase;
}

template<class Object>
void ListPointers<Object>::initAndCreateNewObjects(int d)
{ this->KillAllElements();
  this->SetSize(d);
  for (int i=0; i<d; i++)
    this->TheObjects[i]=new Object;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=d;
  ParallelComputing::CheckPointerCounters();
#endif
}

template<class Object>
void ListPointers<Object>::KillElementIndex(int i)
{ delete this->TheObjects[i];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter--;
  ParallelComputing::CheckPointerCounters();
#endif
  this->size--;
  this->TheObjects[i]=this->TheObjects[this->size];
}

template<class Object>
int ListPointers<Object>::ObjectPointerToIndex(Object* o)
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]==o)
      return i;
  return -1;
}

template<class Object>
void ListPointers<Object>::resizeToLargerCreateNewObjects(int increase)
{ if (increase<=0)
    return;
  int oldsize= this->size;
  this->SetSize(this->size+increase);
  for (int i=oldsize; i<this->size; i++)
    this->TheObjects[i]=new Object;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->size-oldsize;
  ParallelComputing::CheckPointerCounters();
#endif
}

template<class Object>
void ListPointers<Object>::KillAllElements()
{ for (int i =0; i<this->size; i++)
  { delete this->TheObjects[i];
#ifdef CGIversionLimitRAMuse
    if (this->TheObjects[i]!=0)ParallelComputing::GlobalPointerCounter--;
    ParallelComputing::CheckPointerCounters();
#endif
    this->TheObjects[i]=0;
  }
  this->size=0;
}

template<class Object>
bool ListPointers<Object>::AddObjectNoRepetitionOfPointer(Object* o)
{ if (this->ContainsObject(o)==-1)
  { this->AddOnTop(o);
    return true;
  }
  return false;
}

template<class Object>
bool ListPointers<Object>::IsAnElementOf(Object* o)
{ for(int i=0; i<this->size; i++)
    if (this->TheObjects[i]==o)
      return true;
  return false;
}

template <class Object>
void ListPointers<Object>::PopAllOccurrencesSwapWithLast(Object* o)
{ for (int i =0; i<this->size; i++)
    if (o==this->TheObjects[i])
    { this->TheObjects[i]=this->TheObjects[this->size-1];
      this->size--;
      i--;
    }
}

class rootsCollection: public List<roots>
{
public:
  std::string DebugString;
  void ComputeDebugString();
  void Average(root& output, int Number, int theDimension);
  void ResetCounters();
  void CollectionToRoots(roots& output);
  ~rootsCollection(){}
};

//class pertains to the Q^+span of a set of roots.
class ConeGlobal : public roots
{ //The roots are the normals to the walls of the cone
public:
  void ComputeFromDirections(roots& directions, GlobalVariables& theGlobalVariables, int theDimension);
  bool IsSurelyOutsideCone(rootsCollection& TheVertices);
  // the below returns false is we have a point strictly inside the cone
  // else it fills in the Chamber test array
  bool FillInChamberTestArray(roots& TheVertices, bool initChamberTestArray);
  bool IsSurelyOutsideCone(roots& TheVertices);
  bool IsSurelyOutsideConeAccordingToChamberTestArray();
  bool IsInCone(const roots& theRoots);
  bool IsInCone(const root& r);
  bool IsOnConeBorder(const root& r);
  bool IsStrictlyInsideCone(const root& r);
  void TransformToWeylProjective(CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  bool SeparatesPoints(root& point1, root& point2);
  //returns 0 if root is on cone border, +1 if it is strictly inside the cone, -1 in the remaining case.
  int GetSignWRTCone(const root& r);
  //bool SeparatesPoints()
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  static bool PointIsAVertex(const roots& coneNormals, root& thePoint, GlobalVariables& theGlobalVariables);
  bool PointIsAVertex(root& thePoint, GlobalVariables& theGlobalVariables){ return this->PointIsAVertex(*this, thePoint, theGlobalVariables);}
//  int HashFunction() const;
  List<int> ChamberTestArray;
  void operator=(const ConeGlobal& right);
};

class simplicialCones : public List<ConeGlobal>
{
public:
  HashedList<root> theFacets;
  List<List<int> > ConesHavingFixedNormal;
  std::string DebugString;
  void ComputeDebugString();
  void ElementToString(std::string& output);
  void TransformToWeylProjective(CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables);
  std::string ElementToString(){std::string tempS; this->ElementToString(tempS); return tempS;}
  void initFromDirections(roots& directions, GlobalVariables& theGlobalVariables, CombinatorialChamberContainer& owner);
  bool SeparatePoints(root& point1, root& point2, root* PreferredNormal);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
};

class CombinatorialChamberContainer: public ListPointers<CombinatorialChamber>
{ bool flagStoringVertices;
  bool flagUsingVerticesToDetermineBogusNeighborsIfPossible;
  friend class CombinatorialChamber;
  friend class WallData;
public:
  int FirstNonExploredIndex;
  int AmbientDimension;
  std::stringstream log;
  std::string DebugString;
  HashedList<root> theHyperplanes;
  roots HyperplanesComingFromCurrentDirectionAndSmallerIndices;
  ConeGlobal TheGlobalConeNormals;
  ConeGlobal WeylChamber;
  simplicialCones startingCones;
  roots NewHyperplanesToSliceWith;
  HashedList<affineHyperplane> AffineWallsOfWeylChambers;
  affineHyperplanes theWeylGroupAffineHyperplaneImages;
  root IndicatorRoot;
  MemorySaving<WeylGroup> AmbientWeyl;
  List<int> PreferredNextChambers;
  List<int> IndicesInActualNonConvexChamber;
  List<List<int> > NonConvexActualChambers;
//  List<int> ChamberTestArrayBuffer;
  std::fstream FileOutput;
  int indexNextChamberToSlice;
  int NumAffineHyperplanesProcessed;
  int NumAffineHyperplanesBeforeWeylChamberWalls;
  int NumProjectiveHyperplanesBeforeWeylChamberWalls;
  int indexLowestNonCheckedForGlueing;
  int NumExplored;
  int NumTotalGlued;
  affineHyperplanes StartingCrossSections;
  //needed to run the algorithm independently of input variables
  int theCurrentIndex;
  roots theDirections;
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//flags to facilitate mutlitasking. Those are the internal ones.
// the actual mutexes used to syncronize the communation properly are system dependent and
// follow.
  bool flagReachSafePointASAP;
  bool flagIsRunning;
  bool flagMustStop;
// the implementation of the following two has #ifdef's and is system dependent
// Unfortunately there is no system independent way in C++ to do this (as far as I know)
// Let's hope that will be fixed with the new C++ standard!
  void PauseSlicing();
  void ResumeSlicing();
  Controller thePauseController;
/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
  bool flagMakingASingleHyperplaneSlice;
  bool flagSliceWithAWallInitDone;
  bool flagSliceWithAWallIgnorePermanentlyZero;
  bool flagDrawingProjective;
  bool flagMakingReports;
  bool flagSpanTheEntireSpace;
  bool flagMakeGrandMasterConsistencyCheck;
  bool flagUsingStartingConesSeparation;
  static const int MaxNumHeaps=5000;
  static const int GraphicsMaxNumChambers = 10000;
  static int NumTotalCreatedCombinatorialChambersAtLastDefrag;
  static int DefragSpacing;
  static int LastReportedMemoryUse;
  static std::fstream TheBigDump;
  static root PositiveLinearFunctional;
  static bool PrintLastChamberOnly;
  static bool flagAnErrorHasOcurredTimeToPanic;
  static bool flagMakingConsistencyCheck;
  static int flagMaxNumCharsAllowedInStringOutput;
  bool isAValidVertexInGeneral(const root& candidate, roots& theNormalsInvolved, Selection& theSelectedNormals);
  void ConvertHasZeroPolyToPermanentlyZero();
  void AddWeylChamberWallsToHyperplanes(GlobalVariables& theGlobalVariables, WeylGroup& theWeylGroup);
  bool IsSurelyOutsideGlobalCone(rootsCollection& TheVertices);
  void GetAffineWallImage
  (int indexWeylElement, WallData& input, root& output, GlobalVariables& theGlobalVariables)
  ;
  int FindVisibleChamberWithDisplayNumber(int inputDisplayNumber);
  void SliceTheEuclideanSpace(root* theIndicatorRoot, GlobalVariables& theGlobalVariables, bool SpanTheEntireSpace);
  void SliceTheEuclideanSpace(GlobalVariables& theGlobalVariables, bool SpanTheEntireSpace);
  void SliceOneDirection(root* theIndicatorRoot, GlobalVariables& theGlobalVariables);
  void OneSlice(root* theIndicatorRoot, GlobalVariables& theGlobalVariables);
  //when projectivizing the newly added dimension corresponds to the last coordinate
  void InduceFromLowerDimensionalAndProjectivize(CombinatorialChamberContainer& input, GlobalVariables& theGlobalVariables);
  void TransformToWeylProjective
  (GlobalVariables& theGlobalVariables)
  ;
  void GetWeylChamberWallsForCharacterComputation(roots& output);
  void MakeExtraProjectivePlane();
  int GetFirstChamberIndexContainingPoint(const root& input)
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]!=0)
        if (this->TheObjects[i]->PointIsInChamber(input))
          return i;
    return -1;
  }
  int GetNumVisibleChambersNoLabeling();
  int GetNumNonZeroPointers();
  void WireChamberAdjacencyInfoAsIn(CombinatorialChamberContainer& input);
  void LabelChamberIndicesProperly();
  void SetupRootsOfBorel(char WeylLetter, int Dimension, bool reverseOrderElementsForTest);
  void SliceAndComputeDebugString(GlobalVariables& theGlobalVariables, bool SpanTheEntireSpace);
  void SetupBorelAndSlice(char WeylLetter, int Dimension, bool reverseOrderElementsForTest, GlobalVariables& theGlobalVariables, bool SpanTheEntireSpace)
  { this->SetupRootsOfBorel(WeylLetter, Dimension, reverseOrderElementsForTest);
    this->SliceAndComputeDebugString(theGlobalVariables, SpanTheEntireSpace);
  }
  int LabelChambersAndGetNumChambersInWeylChamber(ConeGlobal& theWeylChamber);
  int LabelChambersForDisplayAndGetNumVisibleChambers();
  void ElementToString(std::string& output, bool useLatex, bool useHtml);
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  void ComputeDebugString(bool LatexFormat) {this->ElementToString(this->DebugString, LatexFormat, false); }
  void ComputeDebugString(bool useLatex, bool useHtml){this->ElementToString(this->DebugString, useLatex, useHtml); }
  void init();
  void initNextIndex();
  void Free();
  void WriteReportToFile(DrawingVariables& TDV, roots& directions, std::fstream& output);
  void WriteReportToFile(std::fstream& output, bool DoPurgeZeroPointers);
  void WriteReportToFile(const std::string& FileNameOutput, bool DoPurgeZeroPointers);
  void WriteToDefaultFile(GlobalVariables* theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool WriteToFile(const std::string& FileName, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(const std::string& FileName, GlobalVariables* theGlobalVariables);
  bool ReadFromDefaultFile(GlobalVariables* theGlobalVariables);
  bool ConsistencyCheckNextIndicesToSlice();
  int RootBelongsToChamberIndex(root& input, std::string* outputString);
  void MakeStartingChambers(roots& directions, root* theIndicatorRoot, GlobalVariables& theGlobalVariables);
  void MakeStartingChambersSpanEntireSpace(roots& directions, root* theIndicatorRoot, GlobalVariables& theGlobalVariables);
  void MakeStartingChambersDontSpanEntireSpace(roots& directions, root* theIndicatorRoot, GlobalVariables& theGlobalVariables);
  void ComputeNextIndexToSlice(root& direction);
  void ComputeVerticesFromNormals(GlobalVariables& theGlobalVariables);
  bool oneStepChamberSlice
  (GlobalVariables& theGlobalVariables)
  ;
  void SliceWithAWall(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
  void SliceWithAWallInit(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
  void SliceWithAWallInitSimple(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
  void SliceWithAWallOneIncrement(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables);
  void AddChamberPointerSetUpPreferredIndices(CombinatorialChamber* theChamber, GlobalVariables& theGlobalVariables);
  void GlueOverSubdividedChambersCheckLowestIndex(GlobalVariables& theGlobalVariables);
  void Glue(List<int>& IndicesToGlue, roots& normalsToBeKilled, GlobalVariables& theGlobalVariables);
  void LabelAllUnexplored();
  void DumpAll();
  bool GrandMasterConsistencyCheck(GlobalVariables& theGlobalVariables);
  bool ConsistencyCheck(bool CheckForConvexityChambers, GlobalVariables& theGlobalVariables);
  void PurgeZeroPointers();
  void PurgeInternalWalls();
  void PurgeZeroPolyChambers(GlobalVariables& theGlobalVariables);
  void MakeReportOneSlice(GlobalVariables& theGlobalVariables, int currentIndex, int totalRoots, root& theCurrentDirection);
  void MakeReportGlueing(GlobalVariables& theGlobalVariables, int currentIndex, int TotalNumGlueingsSoFar);
  void ProjectToDefaultAffineSpace(GlobalVariables& theGlobalVariables);
  bool ProjectToDefaultAffineSpaceModifyCrossSections(GlobalVariables& theGlobalVariables);
  void PrintThePolys(std::string& output);
  void CheckForAndRemoveBogusNeighbors(GlobalVariables& theGlobalVariables);
  void ComputeGlobalCone(roots& directions, GlobalVariables& theGlobalVariables);
  void ProjectOntoHyperPlane(root& input, root& normal, root& ProjectionDirection, root& output);
  void drawOutput(DrawingVariables& TDV, root& ChamberIndicator, std::fstream* LaTeXOutput);
  void DrawOutputProjective(DrawingVariables& TDV, root& ChamberIndicator, std::fstream* outputLatex);
  void drawOutputAffine(DrawingVariables& TDV, std::fstream* LaTeXoutput);
  void drawFacetVerticesMethod2(DrawingVariables& TDV, roots& r, roots& directions, int ChamberIndex, WallData& TheFacet, int DrawingStyle, int DrawingStyleDashes, std::fstream* outputLatex);
  bool TestPossibleIndexToSlice(root& direction, int index);
  void ComputeNonConvexActualChambers(GlobalVariables& theGlobalVariables);
  void initComplexPullbackWithAffineTranslates
  (WeylGroup& inputWeyl, roots& inputWeights, GlobalVariables& theGlobalVariables)
  ;
  void initCharacterComputation
  (WeylGroup& inputWeyl, roots& inputWeights, GlobalVariables& theGlobalVariables)
  ;
  void incrementCharacterComputation
  (GlobalVariables& theGlobalVariables)
  ;
  CombinatorialChamberContainer();
  ~CombinatorialChamberContainer();
};

struct PolynomialOutputFormat
{ List<std::string> alphabet;
  void operator=(const PolynomialOutputFormat& other);
public:
  //alphabetBases must contain at least two elements
  List<std::string> alphabetBases;
  std::string GetLetterIndex(int index)const;
  void SetLetterIndex(const std::string& theLetter, int index);
  PolynomialOutputFormat();
  PolynomialOutputFormat(const std::string& AlphabetBase1, const std::string& AlphabetBase2);
  void MakeNumPartFrac();
  void MakeAlphabetArbitraryWithIndex(const std::string& AlphabetBase1, const std::string& AlphabetBase2);
  void MakeAlphabetxi();
  void MakeAlphabetyi();
  int ExtraLinesCounterLatex;
  static int LatexCutOffLine;
  static int LatexMaxLineLength;
  static bool UsingLatexFormat;
  static bool CarriageReturnRegular;
  bool flagUseCalculatorFormatForUEOrdered;
  bool cutOffString;
  unsigned cutOffSize;
};

template <class Element>
class Monomial
{
private:
  Monomial(Monomial<Element>&){assert(false); }
//  int DegreesToIndexRecursive(int MaxDeg, int index);
public:
  inline static std::string GetXMLClassName(){ std::string result ="Monomial_"; result.append(Element::GetXMLClassName()); return result; }
  int NumVariables;
  int* degrees;
  static bool InitWithZero;
  Element Coefficient;
  int HashFunction() const;
//  int DegreesToIndex(int MaxDeg);
  std::string DebugString;
  bool ComputeDebugString(PolynomialOutputFormat& PolyFormat);
  bool ComputeDebugString(){PolynomialOutputFormat PolyFormatLocal; return this->ComputeDebugString(PolyFormatLocal); }
  void StringStreamPrintOutAppend(std::stringstream& out, const PolynomialOutputFormat& PolyFormat)const;
  std::string ElementToString()
  { PolynomialOutputFormat tempFormat;
    return this->ElementToString(tempFormat);
  }
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat);
  std::string ElementToString(PolynomialOutputFormat& PolyFormat){std::string tempS; this->ElementToString(tempS, PolyFormat); return tempS;}
  //void ElementToString(std::string& output);
  //std::string ElementToString(){std::string tempS; this->ElementToString(tempS); return tempS;}
  void MakeConstantMonomial(int Nvar, const Element& coeff);
  void MakeNVarFirstDegree(int LetterIndex, int NumVars, const Element& coeff);
  void init(int nv);
  void initNoDegreesInit(int nv);
  void NullifyDegrees();
  bool operator>(const Monomial<Element>& other)const
  { if (this->operator==(other))
      return false;
    return this->IsGEQLexicographicLastVariableStrongest(other);
  }
  bool IsDivisibleBy(const Monomial<Element>& other)const
  { for (int i=0; i<this->NumVariables; i++)
      if (this->degrees[i]< other.degrees[i])
        return false;
    return true;
  }
  int TotalDegree()const;
  void DivideBy(Monomial<Element>& input, Monomial<Element>& output);
  void DivideByExponentOnly(intRoot& input);
  void MonomialExponentToRoot(root& output);
  void MonomialExponentToRoot(intRoot& output);
  void MakeFromRoot(const Element& coeff, intRoot& input);
  void MakeFromRoot(const Element& coeff, root& input);
  void MonomialExponentToColumnMatrix(MatrixLargeRational& output);
  bool IsLinear()const
  { return this->IsAConstant() || this->IsLinearNoConstantTerm();
  }
  bool IsLinearNoConstantTerm()const
  { int tempInt;
    return this->IsOneLetterFirstDegree(tempInt);
  }
  bool IsOneLetterFirstDegree(int& whichLetter)const
  { whichLetter=-1;
    for (int i=0; i<this->NumVariables; i++)
      if (this->degrees[i]==1)
      { if (whichLetter==-1)
          whichLetter=i;
        else
          return false;
      } else
        if (this->degrees[i]!=0)
          return false;
    return whichLetter!=-1;
  }
  void GetMonomialWithCoeffOne(Monomial<Element>& output);
  void MultiplyBy(Monomial<Element>& m, Monomial<Element>& output);
  bool HasSameExponent(Monomial<Element>& m);
  void Assign(const Monomial<Element>& m);
  void Substitution(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, int NumVarTarget, const Element& theRingUnit);
  void MakeMonomialOneLetter(int NumVars, int LetterIndex, int Power, const Element& Coeff);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { for (int i=this->NumVariables-1; i>=0; i--)
      if (this->degrees[i]!=0)
        return i;
    return -1;
  }
  void IncreaseNumVariables(int increase);
  bool IsGEQpartialOrder(Monomial<Element>& m);
  static bool LeftIsGEQLexicographicLastVariableStrongest
  (const Monomial<Element>& left, const Monomial<Element>& right)
  { return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftIsGEQLexicographicLastVariableWeakest
  (const Monomial<Element>& left, const Monomial<Element>& right)
  { return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  bool IsGEQLexicographicLastVariableStrongest(const Monomial<Element>& m)const;
  bool IsGEQLexicographicLastVariableWeakest(const Monomial<Element>& m)const;
  bool IsGEQTotalDegThenLexicographic(const Monomial<Element>& m)const
  { if (this->TotalDegree()>m.TotalDegree())
      return true;
    if (this->TotalDegree()<m.TotalDegree())
      return false;
    return this->IsGEQLexicographicLastVariableStrongest(m);
  }
  bool IsEqualToZero() const;
  bool IsPositive();
  void DecreaseNumVariables(int increment);
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  bool IsAConstant()const;
  void InvertDegrees();
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData);
  bool operator==(const Monomial<Element>& m)const;
  void operator=(const Monomial<Element>& m);
  int SizeWithoutCoefficient();
  Monomial();
  ~Monomial();
};


template <class Element>
bool Monomial<Element>::InitWithZero=true;

//The class below could as well be called "Associative commutative algebra"
//The ring over which the algebra is defined is called by using TemplateMonomial.Coefficient
//The Template polynomial is assumed to be having coefficients in the ring given by Element

template <class TemplateMonomial, class Element>
class TemplatePolynomial: public HashedList<TemplateMonomial>
{
private:
  TemplatePolynomial(TemplatePolynomial<TemplateMonomial, Element>&){assert(false); };
public:
  int NumVars;
  void MultiplyByMonomial(TemplateMonomial& m);
  void MultiplyByMonomial(TemplateMonomial& m, TemplatePolynomial<TemplateMonomial, Element >& output);
  void AddMonomial(const TemplateMonomial& m);
  bool ElementToStringNeedsBracketsForMultiplication()const{return this->size>1;}
  void SubtractMonomial(const TemplateMonomial& m);
  void CopyFromPoly(const TemplatePolynomial<TemplateMonomial, Element>& p);
  void Assign(const TemplatePolynomial<TemplateMonomial, Element>& p);
  std::string DebugString;
  // returns the number of lines used
  int StringPrintOutAppend(std::string& output, const PolynomialOutputFormat& PolyFormat, bool breakLinesLatex)const;
  std::string ElementToString()const{ PolynomialOutputFormat LocalFormat; return this->ElementToString(false, LocalFormat);}
  std::string ElementToString(bool breakLinesLatex, const PolynomialOutputFormat& PolyFormatLocal)const
  { std::string output; output.clear();
    this->StringPrintOutAppend(output, PolyFormatLocal, breakLinesLatex); return output;
  }
  void ElementToString(std::string& output, PolynomialOutputFormat& PolyFormatLocal)const{ output.clear(); this->StringPrintOutAppend(output, PolyFormatLocal, true);};
  std::string ElementToString(const PolynomialOutputFormat& PolyFormatLocal)const{ std::string output; output.clear(); this->StringPrintOutAppend(output, PolyFormatLocal, true); return output;};
  bool ComputeDebugString();
  int TotalDegree();
  bool IsEqualTo(TemplatePolynomial<TemplateMonomial, Element>& p);
  bool IsEqualToZero()const;
  TemplatePolynomial();
  int FindMaxPowerOfVariableIndex(int VariableIndex);
  void Nullify(int NVar);
  inline void Nullify(int NVar, GlobalVariables* theContext){this->Nullify(NVar);}
  void MakeNVarConst(int nVar, const Element& coeff);
  inline void MakeNVarConst(int nVar, const Element& coeff, GlobalVariables* theContext){this->MakeNVarConst(nVar, coeff);}
  bool HasGEQMonomial(TemplateMonomial& m, int& WhichIndex);
  void MultiplyBy(const TemplatePolynomial<TemplateMonomial, Element>& p);
  inline void operator*=(const TemplatePolynomial<TemplateMonomial, Element>& p){ this->MultiplyBy(p);}
  void MultiplyBy(const TemplatePolynomial<TemplateMonomial, Element>& p, TemplatePolynomial<TemplateMonomial, Element>& output, TemplatePolynomial<TemplateMonomial, Element>& bufferPoly, TemplateMonomial& bufferMon)const;
  void RaiseToPower(int d, TemplatePolynomial<TemplateMonomial, Element>& output, const Element& theRingUniT);
  void RaiseToPower(int d,  const Element& theRingUniT);
  void AddPolynomial(const TemplatePolynomial<TemplateMonomial, Element>& p);
  void WriteToFile(std::fstream& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){return this->WriteToFile(output);}
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  int HasSameExponentMonomial(TemplateMonomial& m);
  void operator= (const TemplatePolynomial<TemplateMonomial, Element>& right);
  bool operator== (const TemplatePolynomial<TemplateMonomial, Element>& right);
  inline void operator+=(const Polynomial<Element>& other){this->AddPolynomial(other);}
  inline void operator+=(const Element& other)
  { TemplateMonomial tempM;
    tempM.MakeConstantMonomial(this->NumVars, other);
    this->AddMonomial(tempM);
  }
  inline void operator-=(const Element& other)
  { TemplateMonomial tempM;
    tempM.MakeConstantMonomial(this->NumVars, other);
    this->SubtractMonomial(tempM);
  }
};


template <class Element>
std::iostream& operator <<(std::iostream& output, const Polynomial<Element>& input)
{ output << input.ElementToString();
  return output;
}
//**********************************************************
/*To use Polynomial<Element> you MUST define:
//Absolutely necessary:
void Element::Add(Element&);
void Element::MultiplyBy(Element&);
void Element::Assign(Element&);
void Element::ElementToString(std::string& output);
      //the above is for printout& debug purposes:
      //how you want your element to be displayed in string format.
Element::Element TheRingUnit;
Element::Element TheRingMUnit;
Element::Element TheRingZero;
int HashedList<Monomial<Element>>::PreferredHashSize;
      //the above is the size of the hash table your polynomial will have. The larger the hash table -
      //the more RAM consumed - the faster the computation. Set 1 if you want min RAM usage. Set 10
      //if you got no good idea what this means.
int List<Monomial<Element>>::ListActualSizeIncrement;
      //the above is the size of the chunk of memory the polynomial class will allocate
      //whenever it needs more memory. For example, if you believe your polynomials
      //will have 1000+ monomials average, then set the above to 1000. Note: abuse of the above
      //might raise your memory use unexpectedly high!
//Optional (required for some methods):
//void Element::DivideBy(&Element);
//void Element::WriteToFile(std::fstream& output);
//void Element::ReadFromFile(std::fstream& input);
//void Element::AssignRational(Rational& r);
*/

template <class Element>
class Polynomial: public TemplatePolynomial<Monomial<Element>, Element>
{
  friend std::iostream& operator << <Element>(std::iostream& output, const Polynomial<Element>& input);
public:
  static bool flagAnErrorHasOccuredTimeToPanic;
  void GetConstantTerm(Element& output, const Element& theRingZero);
  void GetCoeffInFrontOfLinearTermVariableIndex(int index, Element& output, const Element& theRingZero);
  void AssignPolynomialLight(const PolynomialLight<Element>& from);
  void MakeMonomialOneLetter(int NumVars, int LetterIndex, int Power, const Element& Coeff);
  Polynomial(){}
  Polynomial(int degree, Element& coeff);
  void MakeNVarDegOnePoly(int NVar, int NonZeroIndex, const Element& coeff);
  void MakeNVarDegOnePoly(int NVar, int NonZeroIndex1, int NonZeroIndex2, const Element& coeff1, const Element& coeff2);
  void MakeNVarDegOnePoly(int NVar, int NonZeroIndex, const Element& coeff1, const Element& ConstantTerm);
  void MakeLinPolyFromRootNoConstantTerm(const root& r);
  void MakeLinPolyFromRootLastCoordConst(const root& input);
  void MakeOne(int NumVars)
  { this->MakeNVarConst(NumVars, 1);
  }
  void TimesInteger(int a);
  void DivideBy(const Polynomial<Element>& inputDivisor, Polynomial<Element>& outputQuotient, Polynomial<Element>& outputRemainder)const;
  void TimesConstant(const Element& r);
  void DivideByConstant(const Element& r);
  void AddConstant(const Element& theConst);
  void IncreaseNumVariables(int increase){this->IncreaseNumVariablesWithShiftToTheRight(0, increase);};
  void IncreaseNumVariablesShiftVarIndicesToTheRight(int theShift){this->IncreaseNumVariablesWithShiftToTheRight(theShift, theShift);};
  void IncreaseNumVariablesWithShiftToTheRight(int theShift, int theIncrease);
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { int result=-1;
    for (int i=0; i<this->size; i++)
      result=MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    return result;
  }
  void ScaleToPositiveMonomials(Monomial<Element>& outputScale);
  void DecreaseNumVariables(int increment, Polynomial<Element>& output);
  void Substitution(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, int NumVarTarget, const Element& theRingUnit);
  void Substitution(const List<Polynomial<Element> >& TheSubstitution, int NumVarTarget, const Element& theRingUnit= (Element)1);
  int TotalDegree()const;
  bool IsAConstant(Element& whichConstant)const
  { if (this->size>1)
      return false;
    if (this->size==0)
    { whichConstant=0;
      return true;
    }
    Monomial<Element>& theMon=this->TheObjects[0];
    whichConstant=theMon.Coefficient;
    return theMon.IsAConstant();
  }
  bool IsAConstant()const
  { if (this->size>1)
      return false;
    if (this->size==0)
      return true;
    Monomial<Element>& theMon=this->TheObjects[0];
    return theMon.IsAConstant();
  }
  bool IsLinearNoConstantTerm()
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsLinearNoConstantTerm())
        return false;
    return true;
  }
  bool IsLinear()
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsLinear())
        return false;
    return true;
  }
  bool IsLinearGetRootConstantTermLastCoordinate(Vector<Element>& outputRoot, const Element& theZero)
  { outputRoot.MakeZero(this->NumVars+1, theZero);
    int index;
    for (int i=0; i<this->size; i++)
      if(this->TheObjects[i].IsAConstant())
        *outputRoot.LastObject()=this->TheObjects[i].Coefficient;
      else
        if (this->TheObjects[i].IsOneLetterFirstDegree(index))
          outputRoot.TheObjects[index]=this->TheObjects[i].Coefficient;
        else
          return false;
    return true;
  }
  inline bool GetRootFromLinPolyConstTermLastVariable(Vector<Element>& outputRoot, const Element& theZero= (Element) 0){return this->IsLinearGetRootConstantTermLastCoordinate(outputRoot, theZero);}
  void Evaluate
(const Vector<Element>& input, Element& output, const Element& theRingZero)
  ;
  bool IsProportionalTo(const Polynomial<Element>& other, Element& TimesMeEqualsOther, const Element& theRingUnit)const;
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData, PolynomialOutputFormat& PolyFormatLocal);
  int GetIndexMaxMonomial( bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const Monomial<Element>& left, const Monomial<Element>& right))
  { if (this->size==0)
      return -1;
    int result=0;
    for (int i=1; i<this->size; i++)
    { if ( MonomialOrderLeftIsGreaterThanOrEqualToRight(this->TheObjects[i], this->TheObjects[result]))
        result=i;
    }
    return result;
  }
  int GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int GetIndexMaxMonomialTotalDegThenLexicographic();
  void ComponentInFrontOfVariableToPower(int VariableIndex, ListPointers<Polynomial<Element> >& output, int UpToPower);
  int GetMaxPowerOfVariableIndex(int VariableIndex);
  //has to be rewritten please don't use!
  bool IsGreaterThanZeroLexicographicOrder();
  bool IsEqualTo(const Polynomial<Element>& p)const;
  void Subtract(const Polynomial<Element>& other)
  { for (int i=0; i<other.size; i++)
      this->SubtractMonomial(other.TheObjects[i]);
  }
  inline void operator=(const Polynomial<Element>& other){this->Assign(other);}
  inline void operator/=(const Element& theConst){this->DivideByConstant(theConst);}
  inline void operator*=(const Polynomial<Element>& other){this->MultiplyBy(other);}
};

template <class Element>
class Polynomials: public List<Polynomial<Element> >
{
  public:
  //One of the main purposes of this class is to be used for carrying out substitutions.
  //the general format of the substitution is:
  // the i^th element denotes the image of x_i,
  // For example, if polynomials is the array
  // x_1+x_2 (poly in 3 variables)
  // x_1x_3+2 (poly in 3 variables)
  // this mens that it can be applied to polynomials in two variables
  // like this: x_1-> x_1+x_2
  // x_2-> (x_1x_3+2)
  // to produce a polynomial in three variables
  void MakeIdSubstitution(int numVars, const Element& theRingUnit=1);
  void MakeIdLikeInjectionSub(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const Element& theRingUnit);
  //In the following function we have that:
  //the format of the linear substitution is:
  //theSub is a  whose number of rows minus 1 must equal the # number of
  //target variables and whose number of columns must equal the number of variables in
  //the current polynomial (this->NumVariables).
  //The first row denotes the constant term in the substitution of the respective variable!
  //An element in the x-th row and y-th column
  //is defined as Element[x][y] !
  void MakeExponentSubstitution(MatrixIntTightMemoryFit& theSub);
  void PrintPolys(std::string& output, Element& TheRingUnit, Element& TheRingZero);
  void MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<Element>& EndPoint);
  void AddConstant(Element& theConst);
  void TimesConstant(Element& r);
  void DivideByConstant(Element& r);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { int result=-1;
    for (int i=0; i<this->size; i++)
      result=MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    return result;
  }
  void NullifyAll(int NumVars)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i].Nullify((int)NumVars);
  }
};

template <class Element>
class PolynomialLight: public ListLight<Monomial<Element> >
{
public:
  int NumVars;
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString();
  void AssignPolynomial(Polynomial<Element>& from);
  void AssignPolynomialLight(const PolynomialLight<Element>& from);
  void Nullify(int numberVariables);
  void MakeConst(const Element& theConstant, int theNumVars);
  bool IsEqualToZero();
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
};

template <class Element>
inline void PolynomialLight<Element>::ElementToString(std::string& output)
{ Polynomial<Element> tempP;
  tempP.AssignPolynomialLight(*this);
  tempP.ComputeDebugString();
  output= tempP.DebugString;
}

template <class Element>
inline void PolynomialLight<Element>::ComputeDebugString()
{ this->ElementToString(this->DebugString);
}

template <class Element>
inline void PolynomialLight<Element>::AssignPolynomial(Polynomial<Element>& from)
{ this->NumVars=from.NumVars;
  this->CopyFromHeavy(from);
}

template <class Element>
inline void PolynomialLight<Element>::AssignPolynomialLight(const PolynomialLight<Element>& from)
{ this->NumVars= from.NumVars;
  this->CopyFromLight(from);
}

template <class Element>
inline void PolynomialLight<Element>::Nullify(int numberVariables)
{ this->NumVars= numberVariables;
  this->SetSize(0);
}

template <class Element>
inline void PolynomialLight<Element>::MakeConst(const Element& theConstant, int theNumVars)
{ this->NumVars= theNumVars;
  this->SetSize(1);
  this->TheObjects[0].MakeConstantMonomial(theNumVars, theConstant);
}

template <class Element>
void PolynomialLight<Element>::WriteToFile(std::fstream& output)
{ Polynomial<Element> ComputationalBuffer;
  ComputationalBuffer.AssignPolynomialLight(*this);
  ComputationalBuffer.WriteToFile(output);
}

template <class Element>
void PolynomialLight<Element>::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ Polynomial<Element> ComputationalBuffer;
  ComputationalBuffer.ReadFromFile(input, theGlobalVariables);
  this->AssignPolynomial(ComputationalBuffer);
}

template <class Element>
inline bool PolynomialLight<Element>::IsEqualToZero()
{ return this->size==0;
}

class intRoot :public List<int>
{
private:
public:
  void AssignRoot(root& r);
  int HashFunction() const;
  void ElementToString(std::string& output);
  bool IsHigherThanWRTWeight(intRoot& r, intRoot& theWeights);
  bool IsGEQNoWeight(intRoot& r);
  void MakeZero(int  theDimension);
  void MultiplyByInteger(int x)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]*=x;
  }
  void initFromInt(int theDimension, int x1, int x2, int x3, int x4, int x5);
  void initFromInt(int theDimension, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, int x10, int x11, int x12);
  //remark: zero is considered to be a positive vector!
  bool IsPositive();
  void AddRoot(intRoot& theRoot);
  intRoot(){}
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
  Polynomial<LargeInt> Value;
  bool ValueIsComputed;
  GeneratorPFAlgebraRecord(){this->ValueIsComputed=false;}
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat)const;
  void GetValue(Polynomial<LargeInt>& output, int theDimension);
  void operator=(const GeneratorPFAlgebraRecord& right);
  int HashFunction()const{ return this->Elongation+ this->GeneratorRoot.HashFunction();}
  bool operator==(const GeneratorPFAlgebraRecord& right){ return (this->GeneratorRoot == right.GeneratorRoot) && (this->Elongation == right.Elongation);}
};

class GeneratorsPartialFractionAlgebra
{
public:
  static HashedList<GeneratorPFAlgebraRecord> theGenerators;
  //elongation zero means we are looking for x^exponent
  //other elongations means we are looking for p_elongation(x^exponent), where p is
  // the numerator in the geometric series sum corresponding to elongation
  static void GetMonomialFromExponentAndElongation(intRoot& exponent, int elongation, MonomialInCommutativeAlgebra<LargeInt, GeneratorsPartialFractionAlgebra, GeneratorPFAlgebraRecord>& output);
  std::string DebugString;
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat, int theDimension)const;
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat)const;
  int HashFunction() const;
  void operator=(const GeneratorsPartialFractionAlgebra& right);
  void ConvertToIntegerPoly(Polynomial<LargeInt>& output, int theDimension);
  //IMPORTANT two generators are declared to be equal if the generator indices coincide. The power doesn't count.
  //This might have to be rewritten.
  bool operator==(const GeneratorsPartialFractionAlgebra& right);
  bool operator>(const GeneratorsPartialFractionAlgebra& right)const
  { return this->GeneratorIndex>right.GeneratorIndex ||
      (this->GeneratorIndex==right.GeneratorIndex && this->GeneratorPower>right.GeneratorPower);
  }
  int GeneratorIndex;
  int GeneratorPower;
};

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
class MonomialInCommutativeAlgebra: public HashedList<GeneratorsOfAlgebra>
{
private:
  MonomialInCommutativeAlgebra(MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>&){assert(false); };
//  int DegreesToIndexRecursive(int MaxDeg, int index);
public:
  Element Coefficient;
  std::string DebugString;
  void ComputeDebugString(PolynomialOutputFormat &PolyFormat);
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat);
  void StringStreamPrintOutAppend(std::stringstream& out, const PolynomialOutputFormat& PolyFormat);
  int HashFunction()const;
  void MakeConstantMonomial(int Nvar, const Element& coeff);
  void MultiplyBy(MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m, MonomialInCommutativeAlgebra<  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& output);
  void MultiplyBy(MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m);
  //the below functions return 1 if no reduction has occured
  //i.e. the generator is not in zeroth power after the multiplication
  //If the generator is in zero-th power after the multiplication and reduction has occured,
  //the below return 0
  int MultiplyByGenerator(GeneratorsOfAlgebra& g);
  int MultiplyByGenerator(GeneratorsOfAlgebraRecord& g, int Power);
  int MultiplyByGenerator(int GeneratorIndex, int GeneratorPower);
  void Assign(const MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m);
  bool IsEqualToZero()const;
  MonomialInCommutativeAlgebra(){}
  //IMPORTANT: the coefficients of two monomials are not compared, that is,
  //two monomials are equal if they have the same
  //generators at same powers
  bool operator==(const MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m);
  void operator=(const MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m);
};

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
bool MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::IsEqualToZero()const
{ return this->Coefficient.IsEqualTo(Element::TheRingZero);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::StringStreamPrintOutAppend(std::stringstream& out, const PolynomialOutputFormat& PolyFormat)
{ std::stringstream out1;
  std::string tempS;
  for(int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS, PolyFormat);
    if (tempS[0]=='-' && this->size>1)
      out1 << "(" << tempS << ")";
    else
      out1 << tempS;
  }
  tempS= out1.str();
  std::string tempS2;
  this->Coefficient.ElementToString(tempS2);
  if (tempS=="")
    out << tempS2;
  else
  { if (tempS2=="1")
    { out << tempS;
      return;
    }
    if (tempS2=="-1")
    { if (tempS=="-")
      { tempS.erase(0, 1);
        out << tempS;
        return;
      }
      else
      { out << "-" << tempS;
        return;
      }
    }
    out << tempS2 << "(" << tempS << ")";
  }
}

class PolyPartFractionNumerator: public TemplatePolynomial<MonomialInCommutativeAlgebra<LargeInt, GeneratorsPartialFractionAlgebra, GeneratorPFAlgebraRecord>, LargeInt>
{
public:
  void ConvertToIntegerPoly(Polynomial<LargeInt>& output, int theDimension);
};

class PolyPartFractionNumeratorLight: public ListLight<ListLight<GeneratorsPartialFractionAlgebra> >
{
public:
  ListLight<LargeInt> Coefficients;
  void AssignPolyPartFractionNumerator(PolyPartFractionNumerator& from);
  void ComputePolyPartFractionNumerator(PolyPartFractionNumerator& output, int theDimension) const;
  void AssignPolyPartFractionNumeratorLight(const PolyPartFractionNumeratorLight& right);
  int NumGeneratorsUsed();
};

template<class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra<  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::MakeConstantMonomial(int Nvar, const Element& coeff)
{ this->ClearTheObjects();
  this->Coefficient.Assign(coeff);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra<  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::MultiplyBy( MonomialInCommutativeAlgebra  <  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m, MonomialInCommutativeAlgebra  <  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& output)
{ assert(&m!=&output);
  if (&output!= this)
    output.Assign(*this);
  output.MultiplyBy(m);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::MultiplyByGenerator(GeneratorsOfAlgebra& g)
{ int x = this->IndexOfObjectHash(g);
  if (x==-1)
    this->AddOnTopHash(g);
  else
  { this->TheObjects[x].GeneratorPower+=g.GeneratorPower;
    if (this->TheObjects[x].GeneratorPower==0)
    { this->PopIndexSwapWithLastHash(x);
      return 0;
    }
  }
  return 1;
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::MultiplyByGenerator(int GeneratorIndex, int GeneratorPower)
{ GeneratorsPartialFractionAlgebra tempG;
  tempG.GeneratorIndex= GeneratorIndex;
  tempG.GeneratorPower= GeneratorPower;
  return this->MultiplyByGenerator(tempG);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::MultiplyByGenerator(GeneratorsOfAlgebraRecord& g, int Power)
{ GeneratorsPartialFractionAlgebra tempG;
  tempG.GeneratorIndex=GeneratorsOfAlgebra::theGenerators.IndexOfObjectHash(g);
  if (tempG.GeneratorIndex==-1)
  { GeneratorsOfAlgebra::theGenerators.AddOnTopHash(g);
    tempG.GeneratorIndex= GeneratorsOfAlgebra::theGenerators.size-1;
  }
  tempG.GeneratorPower=Power;
  return this->MultiplyByGenerator(tempG);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
int MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::HashFunction() const
{ int result=0;
  for (int i=0; i<this->size; i++)
    result+=this->TheObjects[i].HashFunction()*this->TheObjects[i].GeneratorPower;
  return result;
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat)
{ std::stringstream out;
  this->StringStreamPrintOutAppend(out, PolyFormat);
  output=out.str();
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::ComputeDebugString(PolynomialOutputFormat& PolyFormat)
{ this->ElementToString(this->DebugString, PolyFormat);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
inline void MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::Assign(const MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& right)
{ this->CopyFromHash(right);
  this->Coefficient.Assign(right.Coefficient);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
inline void MonomialInCommutativeAlgebra <  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::operator =(const MonomialInCommutativeAlgebra <  Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& right)
{ this->Assign(right);
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
inline bool MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::operator ==(const MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& right)
{ MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord> tempM;
  tempM.Assign(*this);
  for (int i=0; i<right.size; i++)
  { GeneratorsOfAlgebra tempG;
    tempG= right.TheObjects[i];
    tempG.GeneratorPower=- tempG.GeneratorPower;
    if (tempM.MultiplyByGenerator(right.TheObjects[i].GeneratorIndex, -right.TheObjects[i].GeneratorPower)==1)
      return false;
  }
  if (tempM.size>0)
    return false;
  return true;
}

template <class Element, class GeneratorsOfAlgebra, class GeneratorsOfAlgebraRecord>
void MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>::MultiplyBy(MonomialInCommutativeAlgebra<Element, GeneratorsOfAlgebra, GeneratorsOfAlgebraRecord>& m)
{ this->MakeActualSizeAtLeastExpandOnTop(this->size+m.size);
  for (int i=0; i<m.size; i++)
    this->MultiplyByGenerator(m.TheObjects[i]);
  this->Coefficient.MultiplyBy(m.Coefficient);
}

class PolynomialsRationalCoeff: public Polynomials<Rational>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output, int numDisplayedEltsMinus1ForAll=-1);
  std::string ElementToString(int numDisplayedEltsMinus1ForAll=-1){std::string tempS; this->ElementToString(tempS, numDisplayedEltsMinus1ForAll); return tempS;}
  void ComputeDebugString();
  void operator=(const PolynomialsRationalCoeff& right);
  bool operator==(const PolynomialsRationalCoeff& right);
  void ComputeB(PolynomialRationalCoeff& output, int cutOffIndex, int theDimension);
  void MakeOneParameterSubFromDirection(root& direction);
  void MakeOneParameterSubFromDirectionInts(int x1, int x2, int x3, int x4, int x5);
  void MakeOneParameterSubFromDirectionIntsAndConstants(int x1, int x2, int x3, int x4, int x5, int c1, int c2, int c3, int c4, int c5);
  void MakeSubFromMatrixIntAndDen(MatrixIntTightMemoryFit& theMat, int Den);
  void MakeLinearSubConstTermsLastRow(MatrixLargeRational& theMat);
  void MakeConstantShiftCoordinatesAreAdded(root& shiftPerVariable);
  void MakeLinearSubbedVarsCorrespondToMatRows(MatrixLargeRational& theMat, root& theConstants);
  void ComputeDiscreteIntegrationUpTo(int d);
  void MakeLinearSubOnLastVariable(int NumVars, PolynomialRationalCoeff& LastVarSub);
  void MakeSubNVarForOtherChamber(root& direction, root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
  void MakeSubAddExtraVarForIntegration(root& direction);
  void Substitution(PolynomialsRationalCoeff& theSub, int NumVarsTarget);
};

class PolynomialsRationalCoeffCollection: public List<PolynomialsRationalCoeff>
{
public:
  roots ChamberIndicators;
  std::string DebugString;
  void ElementToString(std::string& output, int theDimension);
  void ElementToStringComputeFunctionB(std::string& output, bool computingB, int theDimension);
  void ComputeDebugString(int theDimension);
  void Substitution(PolynomialsRationalCoeff& theSub, int NumVarsTarget);
};

class IntegerPoly: public Polynomial<Integer>
{
public:
  void MakePolyExponentFromIntRoot(intRoot& r);
  IntegerPoly(){}
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
  PolynomialRationalCoeff(){}
  PolynomialRationalCoeff(const PolynomialRationalCoeff& other){this->Assign(other);}
  //the below is a temporary function to facilitate debugging should be removed
  bool checkConsistency(){return true;}
  //the below constructor is very slow use for testing purposes only
  //Parsing stuff is inherently slow and I use the mighty parser
  //PolynomialRationalCoeff(const char* input){ std::string tempS=input; this->operator=(tempS);}
  //bool GetRootFromLinPolyConstTermLastVariable(root& output) {return this->GetRootFromLinPolyConstTermLastVariable(output, (Rational) 0);}
  void AssignIntegerPoly(const Polynomial<LargeInt>& p);
  void Evaluate(intRoot& values, Rational& output);
  void Evaluate(const root& values, Rational& output){ this->::Polynomial<Rational>::Evaluate(values, output, (Rational) 0);}
  void MakePolyFromDirectionAndNormal(root& direction, root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
  void MakePolyExponentFromIntRoot(intRoot& r, GlobalVariables& theGlobalVariables);
  void MakeLinPolyFromInt(int theDimension, int x1, int x2, int x3, int x4, int x5);
  bool GetIntegerPoly(Polynomial<LargeInt>& output)const;
  int FindGCMCoefficientDenominators();
  Rational FindGCDCoefficientDenominators();
  Rational FindGCDCoefficientNumerators();
  void MakeLinearPoly(int NumVars);
  int SizeWithoutDebugString();
  //works at the moment for linear polynomials only!!!!!!
  void DivByInteger(int x);
  bool IsInteger()const
  { if (this->size>1)
      return false;
    if (this->size==0)
      return true;
    if (this->IsAConstant())
      if (this->TheObjects[0].Coefficient.IsInteger())
        return true;
    return false;
  }
  bool IsSmallInteger()const
  { int theInt;
    return this->IsSmallInteger(theInt);
  }
  bool IsSmallInteger(int& theInteger)const
  { if (this->size>1)
      return false;
    if (this->size==0)
    { theInteger=0;
      return true;
    }
    if (this->IsAConstant())
      if (this->TheObjects[0].Coefficient.IsInteger() && this->TheObjects[0].Coefficient.Extended==0)
      { theInteger=this->TheObjects[0].Coefficient.NumShort;
        return true;
      }
    return false;
  }
  bool IsEqualToOne()const
  { if (this->size!=1)
      return false;
    if (!this->IsAConstant())
      return false;
    if (this->TheObjects[0].Coefficient!=1)
      return false;
    return true;
  }
  void DivByRational(const Rational& other){Rational tempRat=other; tempRat.Invert(); this->TimesConstant(tempRat);}
  void TimesInteger(int x);
  void operator+=(const PolynomialRationalCoeff& other){ this->AddPolynomial(other);}
  void operator+=(const Rational& theConst){ Monomial<Rational> tempMon; tempMon.MakeConstantMonomial(this->NumVars,theConst); this->AddMonomial(tempMon);}
  inline void operator*=(const PolynomialRationalCoeff& other){this->MultiplyBy(other);}
  void operator*=(const Rational& theConst){ this->TimesConstant(theConst);}
  bool operator==(const PolynomialRationalCoeff& right){ PolynomialRationalCoeff tempP; tempP.Assign(right); tempP.Subtract(*this); return tempP.IsEqualToZero();};
  void operator=(const PolynomialRationalCoeff& right);
  void operator=(const Polynomial<Rational>& right){this->Assign(right);}
  //the below is very slow use for testing purposes only
  //Parsing stuff is inherently slow and I use the mighty parser
  void operator=(const std::string& input);
  void operator++(){this->operator+=((Rational)1);}
  void operator--(){this->operator-=((Rational)1);}
  void operator-=(const Rational& theConst){ Monomial<Rational> tempMon; tempMon.MakeConstantMonomial(this->NumVars, -theConst); this->AddMonomial(tempMon);};
  PolynomialRationalCoeff operator-(const PolynomialRationalCoeff& other)
  { PolynomialRationalCoeff tempP;
    tempP.Assign(*this);
    tempP.Subtract(other);
    return tempP;
  }
  void ClearDenominators(Rational& output)
  { output.MakeOne();
    Rational tempRat;
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].Coefficient.IsInteger())
      { this->TheObjects[i].Coefficient.GetDen(tempRat);
        *this*=tempRat;
        output*=tempRat;
      }
  }
  void MakePChooseK(const PolynomialRationalCoeff& P, int k, PolynomialRationalCoeff& output);
  void ScaleToIntegralNoGCDCoeffs();
  void Subtract(const PolynomialRationalCoeff& other)
  { PolynomialRationalCoeff tempP;
    tempP.Assign(other);
    Rational tempRat; tempRat.MakeMOne();
    tempP.TimesConstant(tempRat);
    this->AddPolynomial(tempP);
  }
};

std::iostream& operator<<(std::iostream& output, const RationalFunction& theRF);
std::iostream& operator>>(std::iostream& input, RationalFunction& theRF);

class RationalFunction
{
private:
  friend std::iostream& operator<< (std::iostream& output, const RationalFunction& theRF)
  { output << theRF.ElementToString();
    return output;
  }
  friend std::iostream& operator>> (std::iostream& input, RationalFunction& theRF);
  void AddSameTypes(const RationalFunction& other)
  { switch (this->expressionType)
    { case RationalFunction::typeRational: this->ratValue+=other.ratValue; break;
      case RationalFunction::typePoly: this->Numerator.GetElement()+=other.Numerator.GetElementConst(); break;
      case RationalFunction::typeRationalFunction: this->AddHonestRF(other); break;
    }
    this->ReduceMemory();
    assert(this->checkConsistency());
  }
  void AddHonestRF(const RationalFunction& other);
  void ReduceRFToPoly()
  { if (this->expressionType==this->typeRationalFunction)
    { if (this->Denominator.GetElement().IsAConstant())
      { this->Numerator.GetElement()/=this->Denominator.GetElement().TheObjects[0].Coefficient;
        this->Denominator.FreeMemory();
        this->expressionType=this->typePoly;
      }
      if (this->Numerator.GetElement().IsEqualToZero())
        this->Nullify(this->NumVars, this->context);
    }
  }
  void ReducePolyToRational()
  { if (this->expressionType==this->typePoly)
      if(this->Numerator.GetElement().IsAConstant())
      { this->expressionType=this->typeRational;
        if (this->Numerator.GetElement().IsEqualToZero())
          this->ratValue.MakeZero();
        else
          this->ratValue=this->Numerator.GetElement().TheObjects[0].Coefficient;
        this->Numerator.FreeMemory();
      }
  }
  bool ConvertToType(int theType)
  { if (theType<this->expressionType)
      return false;
    if (theType==this->expressionType)
      return true;
    if (this->expressionType==this->typeRational && this->expressionType<theType)
    { this->expressionType=this->typePoly;
      this->Numerator.GetElement().MakeNVarConst(this->NumVars, this->ratValue);
    }
    if (this->expressionType==this->typePoly && this->expressionType<theType)
    { this->expressionType=this->typeRationalFunction;
      this->Denominator.GetElement().MakeNVarConst(this->NumVars, (Rational) 1);
    }
    return true;
  }
  RationalFunction(const RationalFunction& other){this->Assign(other);}
public:
  MemorySaving<PolynomialRationalCoeff> Numerator;
  MemorySaving<PolynomialRationalCoeff> Denominator;
  GlobalVariables* context;
  Rational ratValue;
  int NumVars;
  int expressionType;
  enum typeExpression{ typeRational=0, typePoly=1, typeRationalFunction=2, typeError=3};
  std::string DebugString;
  void ComputeDebugString(){ this->DebugString=this->ElementToString();}
  std::string ElementToString()const {return this->ElementToString(true, false);}
  std::string ElementToString(bool useLatex, bool breakLinesLatex)const;
  bool ElementToStringNeedsBracketsForMultiplication()const
  { switch(this->expressionType)
    { case RationalFunction::typeRational: return false;
      case RationalFunction::typePoly: return this->Numerator.GetElementConst().ElementToStringNeedsBracketsForMultiplication();
      case RationalFunction::typeRationalFunction: return false;
    }
    return false;
  }
  void Substitution(PolynomialsRationalCoeff& theSub);
  std::string ElementToString(const PolynomialOutputFormat& theFormat)const{return this->ElementToString(true, true);}
  void ElementToString(std::string& output)const{output=this->ElementToString(true, false);}
  RationalFunction(){this->NumVars=0; this->expressionType=this->typeRational; this->ratValue.MakeZero(); this->context=0;}
  void RaiseToPower(int thePower);
//  RationalFunction(const RationalFunction& other){this->NumVars=0; this->expressionType=this->typeRational; this->ratValue.MakeZero(); this->operator=(other);}
  void ReduceMemory()
  { this->ReduceRFToPoly();
    this->ReducePolyToRational();
    assert(this->checkConsistency());
  }
  void operator=(const PolynomialRationalCoeff& other)
  { this->expressionType=this->typePoly;
    this->NumVars=other.NumVars;
    this->Numerator.GetElement()=other;
    this->ReduceMemory();
  }
  inline void operator=(const RationalFunction& other){this->Assign(other);}
  void Assign(const RationalFunction& other)
  { this->expressionType=other.expressionType;
    this->NumVars=other.NumVars;
    if (other.context!=0)
      this->context=other.context;
    switch (this->expressionType)
    { case RationalFunction::typeRational:
        this->ratValue=other.ratValue;
        //The below is for testing purposes. I think it is generally better to comment those lines!
        //this->Numerator.FreeMemory();
        //this->Denominator.FreeMemory();
        break;
      case RationalFunction::typePoly:
        assert(!other.Numerator.IsZeroPointer() );
        this->Numerator.GetElement()=other.Numerator.GetElementConst();
        //The below is for testing purposes. I think it is generally better to comment those lines!
        //this->Denominator.FreeMemory();
        break;
      case RationalFunction::typeRationalFunction:
        assert(!other.Numerator.IsZeroPointer() && !other.Denominator.IsZeroPointer());
        this->Numerator.GetElement()=other.Numerator.GetElementConst();
        this->Denominator.GetElement()=other.Denominator.GetElementConst();
        break;
      default: break;
    }
  }
  bool checkConsistency()const
  { if (this->expressionType==this->typePoly)
    { if (this->Numerator.IsZeroPointer())
      { assert(false);
        return false;
      }
      if (this->Numerator.GetElementConst().IsAConstant())
      { assert(false);
        return false;
      }
    }
    if (this->expressionType==this->typeRationalFunction)
    { if (this->Numerator.IsZeroPointer() || this->Denominator.IsZeroPointer())
      { assert(false);
        return false;
      }
      if (this->Denominator.GetElementConst().IsAConstant())
      { assert(false);
        return false;
      }
    }
    return true;
  }
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars)
  { if (this->NumVars== newNumVars)
      return;
    this->Numerator.GetElement().SetNumVariablesSubDeletedVarsByOne(newNumVars);
    this->Denominator.GetElement().SetNumVariablesSubDeletedVarsByOne(newNumVars);
    bool mustSimplify= (newNumVars<this->NumVars);
    this->NumVars=newNumVars;
    if (mustSimplify)
      this->Simplify();
    assert(this->checkConsistency());
  }
  void GetNumerator(PolynomialRationalCoeff& output)
  { switch(this->expressionType)
    { case RationalFunction::typeRational: output.MakeNVarConst(this->NumVars, this->ratValue); return;
      default: output=this->Numerator.GetElementConst(); return;
    }
  }
  void GetDenominator(PolynomialRationalCoeff& output)
  { switch(this->expressionType)
    { case RationalFunction::typeRationalFunction: output=this->Denominator.GetElement(); return;
      default: output.MakeNVarConst(this->NumVars, (Rational) 1); return;
    }
  }
  void ClearDenominators
  (RationalFunction& outputWasMultipliedBy)
  ;
  void MultiplyBy(const RationalFunction& other){this->operator*=(other);}
  void Add(const RationalFunction& other)
  { assert(this->NumVars==other.NumVars);
    assert(this->checkConsistency());
    assert(other.checkConsistency());
    assert(this!=&other);
    if (this->context==0)
      this->context=other.context;
    if (other.expressionType< this->expressionType)
    { RationalFunction tempRF;
      tempRF=other;
      tempRF.ConvertToType(this->expressionType);
      this->AddSameTypes(tempRF);
      assert(this->checkConsistency());
      return;
    }
    if (this->expressionType==other.expressionType)
    { std::string tempS;
      tempS=other.ElementToString();
      this->AddSameTypes(other);
      assert(this->checkConsistency());
      return;
    }
    if (this->expressionType<other.expressionType)
    { this->ConvertToType(other.expressionType);
      this->AddSameTypes(other);
      assert(this->checkConsistency());
    }
    assert(this->checkConsistency());
  }
  inline bool operator==(const RationalFunction& other){return this->IsEqualTo(other);}
  void Simplify();
  void SimplifyLeadingCoefficientOnly();
  void operator+=(const RationalFunction& other){this->Add(other);}
  void operator+=(int theConstant){RationalFunction tempRF; tempRF.MakeNVarConst(this->NumVars, (Rational) theConstant, this->context); (*this)+=tempRF;}
  void operator*=(const RationalFunction& other);
  void operator*=(const PolynomialRationalCoeff& other);
  void operator*=(const Rational& other);
  void operator-=(int theConstant){RationalFunction tempRF; tempRF.MakeNVarConst(this->NumVars, (Rational) -theConstant, this->context); (*this)+=tempRF;}
  inline void TimesConstant(const Rational& theConst){ this->operator*=(theConst);}
  void Invert()
  { assert(this->checkConsistency());
    if (this->expressionType==this->typeRational)
    { assert(!this->ratValue.IsEqualToZero());
      this->ratValue.Invert();
      return;
    }
    if (this->expressionType==this->typePoly)
      this->ConvertToType(this->typeRationalFunction);
    assert(!this->Numerator.GetElement().IsEqualToZero());
    MathRoutines::swap(this->Numerator.GetElement(), this->Denominator.GetElement());
    this->ReduceMemory();
    this->SimplifyLeadingCoefficientOnly();
    assert(this->checkConsistency());
  }
  void Nullify(int theNumVars, GlobalVariables* theContext)
  { this->NumVars=theNumVars;
    this->expressionType=this->typeRational;
    this->ratValue.MakeZero();
    if (theContext!=0)
      this->context=theContext;
    this->Numerator.FreeMemory();
    this->Denominator.FreeMemory();
    assert(this->checkConsistency());
  }
  void MakeNVarConst(int theNumVars, const Rational& theCoeff, GlobalVariables* theContext)
  { this->Nullify((int)theNumVars, theContext);
    this->ratValue=theCoeff;
  }
  bool IsInteger()const {return this->expressionType==this->typeRational && this->ratValue.IsInteger();}
  bool IsSmallInteger(int& theInteger)const {theInteger=this->ratValue.NumShort; return this->expressionType==this->typeRational && this->ratValue.IsSmallInteger();}
  bool IsEqualToZero()const{return this->expressionType==this->typeRational && this->ratValue.IsEqualToZero();}
  bool IsEqualToOne()const{return this->expressionType==this->typeRational && this->ratValue.IsEqualToOne();}
  bool IsEqualTo(const RationalFunction& other)const
  { if (this->expressionType!=other.expressionType)
      return false;
    switch(this->expressionType)
    { case RationalFunction::typeRationalFunction:  return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst()) && this->Denominator.GetElementConst().IsEqualTo(other.Denominator.GetElementConst());
      case RationalFunction::typePoly: return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst());
      case RationalFunction::typeRational: return this->ratValue==other.ratValue;
    }
    assert(false);
    return false;
  }
  static void ReduceGroebnerBasis
  (List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& buffer1,
 bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const Monomial<Rational>& left, const Monomial<Rational>& right)
   )
  ;
  static void GetRelations
  ( List<PolynomialRationalCoeff>& theGenerators, GlobalVariables& theGlobalVariables
   )
   ;
  static void TransformToReducedGroebnerBasis
  (List<PolynomialRationalCoeff>& theBasis,
   PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3,
   PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, GlobalVariables* theGlobalVariables)
  { RationalFunction::TransformToReducedGroebnerBasis
    (
     theBasis, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2, &Monomial<Rational>::LeftIsGEQLexicographicLastVariableStrongest, theGlobalVariables
     );
  }
  static void TransformToReducedGroebnerBasis
    (List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2,
  bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const Monomial<Rational>& left, const Monomial<Rational>& right), GlobalVariables* theGlobalVariables
 )
 ;
  static void RemainderDivisionWithRespectToBasis
(PolynomialRationalCoeff& input, List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& outputRemainder, PolynomialRationalCoeff& buffer1,
 PolynomialRationalCoeff& buffer2, Monomial<Rational>& bufferMon1,
 bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const Monomial<Rational>& left, const Monomial<Rational>& right)
 )
 ;
  static void RemainderDivision
(PolynomialRationalCoeff& input, PolynomialRationalCoeff& divisor, PolynomialRationalCoeff& outputRemainder,
 PolynomialRationalCoeff& buffer, Monomial<Rational>& bufferMon1,
   bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const Monomial<Rational>& left, const Monomial<Rational>& right)
 )
   ;
  static bool gcdQuicK
  (const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output)
  ;
  static void gcd
  (const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, GlobalVariables* theContext)
  ;
  static void ScaleClearDenominator
  (List<RationalFunction>& input, rootPoly& output)
  ;
  static void gcd(const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, PolynomialRationalCoeff& buffer5, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList);
  static void lcm(const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList);
  static inline void lcm(PolynomialRationalCoeff& left, PolynomialRationalCoeff& right, PolynomialRationalCoeff& output)
  { PolynomialRationalCoeff buffer1, buffer2, buffer3, buffer4; List<PolynomialRationalCoeff> bufferList; Monomial<Rational> tempMon1, tempMon2;
    RationalFunction::lcm(left, right, output, buffer1, buffer2, buffer3, buffer4, tempMon1, tempMon2, bufferList);
  }
  inline void operator-=(const RationalFunction& other)
  { assert(this->checkConsistency());
    assert(other.checkConsistency());
    if (other.context!=0)
      this->context=other.context;
    RationalFunction tempRF;
    tempRF=other;
    tempRF.Minus();
    this->operator+=(tempRF);
    assert(this->checkConsistency());
  }
  inline void operator/=(const Rational& input)
  { Rational tempRat=input;
    tempRat.Invert();
    this->operator*=(tempRat);
    assert(this->checkConsistency());
  }
  inline void operator/=(const RationalFunction& other)
  { RationalFunction tempRF;
    if (other.context!=0)
      this->context=other.context;
    tempRF=other;
    tempRF.Invert();
    this->MultiplyBy(tempRF);
    assert(this->checkConsistency());
  }
  void Minus(){this->operator*=((Rational) -1); assert(this->checkConsistency());}
};

class rootPoly: public List<PolynomialRationalCoeff>
{
public:
  std::string DebugString;
  std::string ElementToString();
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
};

class rootRationalFunction: public List<RationalFunction>
{
public:
  std::string DebugString;
  std::string ElementToString();
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  void ScaleToPolynomial();
};

class QuasiPolynomials:public Polynomials<QuasiNumber>
{
public:
  void IntegrateDiscreteInDirectionFromZeroTo(root& direction, PolynomialRationalCoeff& EndPoint, QuasiPolynomialOld& output, QuasiPolynomialOld& input, PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
};

template <class Element>
int Monomial<Element>::TotalDegree()const
{ int result=0;
  for (int i=0; i<this->NumVariables; i++)
    result+=this->degrees[i];
  return result;
}

template <class Element>
void Monomial<Element>::Substitution(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, int NumVarTarget, const Element& theRingUnit)
{ if (this->IsAConstant())
  { output.MakeNVarConst(NumVarTarget, this->Coefficient);
    return;
  }
  Polynomial<Element> tempPoly;
  output.MakeNVarConst(NumVarTarget, this->Coefficient);
//  std::cout << "<hr>subbing in monomial " << this->ElementToString();
//  output.ComputeDebugString();
  assert(TheSubstitution.size==this->NumVariables);
  for (int i=0; i<this->NumVariables; i++)
    if (this->degrees[i]!=0)
    {  //TheSubstitution.TheObjects[i]->ComputeDebugString();
      TheSubstitution.TheObjects[i].RaiseToPower(this->degrees[i], tempPoly, theRingUnit);
//      tempPoly.ComputeDebugString();
      output.MultiplyBy(tempPoly);
//      output.ComputeDebugString();
    }
//  std::cout << " to get: " << output.ElementToString();
}

template <class Element>
Monomial<Element>::Monomial()
{ this->NumVariables=0;
  this->degrees=0;
/*  if(Monomial<Element>::InitWithZero)
    this->Coefficient.Assign(Element::TheRingZero);
  else
    this->Coefficient.Assign(Element::TheRingUnit);*/
}

template <class Element>
void Monomial<Element>::InvertDegrees()
{ for (int i=0; i<this->NumVariables; i++)
    this->degrees[i]= - this->degrees[i];
}

template <class Element>
void Monomial<Element>::init(int nv)
{ assert(nv>=0);
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=nv-this->NumVariables;
  ParallelComputing::CheckPointerCounters();
#endif
  if(this->NumVariables!=nv)
  { this->NumVariables=nv;
    delete [] this->degrees;
    this->degrees= new int[this->NumVariables];
  }
  for (int i=0; i<this->NumVariables; i++)
    this->degrees[i]=0;
}

template <class Element>
void Monomial<Element>::initNoDegreesInit(int nv)
{
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=nv-this->NumVariables;
  ParallelComputing::CheckPointerCounters();
#endif
  if(this->NumVariables!=nv)
  { this->NumVariables=nv;
    delete [] this->degrees;
    this->degrees= new int[this->NumVariables];
  }
}

template <class Element>
Monomial<Element>::~Monomial()
{ delete [] this->degrees;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->NumVariables;
  ParallelComputing::CheckPointerCounters();
#endif
}

template <class Element>
void Monomial<Element>::StringStreamPrintOutAppend(std::stringstream& out, const PolynomialOutputFormat& PolyFormat)const
{ if (this->Coefficient.IsEqualToZero())
  { out << "0";
    return;
  }
  std::string tempS;
  this->Coefficient.ElementToString(tempS);
  if (this->IsAConstant())
  { out << tempS;
    return;
  }
  if (tempS=="-1")
    tempS="-";
  if (tempS=="1")
    tempS.clear();
  out << tempS;
  for (int i=0; i<this->NumVariables; i++)
    if (this->degrees[i]!=0)
    { out << PolyFormat.GetLetterIndex(i);
      if (!PolynomialOutputFormat::UsingLatexFormat)
      { if (this->degrees[i]!=1)
          out << "^" << this->degrees[i];
      }
      else
        if (this->degrees[i]!=1)
          out << "^{" << this->degrees[i] << "}";
    }
}

template <class Element>
bool Monomial<Element>::IsGEQpartialOrder(Monomial<Element>& m)
{ assert(this->NumVariables == m.NumVariables);
  for (int i=0; i<m.NumVariables; i++)
    if (this->degrees[i]<m.degrees[i])
      return false;
  return true;
}

template <class Element>
bool Monomial<Element>::IsGEQLexicographicLastVariableWeakest(const Monomial<Element>& m)const
{ assert(this->NumVariables==m.NumVariables);
  for (int i=0; i<this->NumVariables; i++)
  { if (this->degrees[i]>m.degrees[i])
      return true;
    if (this->degrees[i]<m.degrees[i])
      return false;
  }
  return true;
}

template <class Element>
bool Monomial<Element>::IsGEQLexicographicLastVariableStrongest(const Monomial<Element>& m)const
{ assert(this->NumVariables==m.NumVariables);
  for (int i=this->NumVariables-1; i>=0; i--)
  { if (this->degrees[i]>m.degrees[i])
      return true;
    if (this->degrees[i]<m.degrees[i])
      return false;
  }
  return true;
}

template <class Element>
void Monomial<Element>::ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormatLocal)
{ std::stringstream out;
  this->StringStreamPrintOutAppend(out, PolyFormatLocal);
  output=out.str();
}

template <class Element>
bool Monomial<Element>::ComputeDebugString(PolynomialOutputFormat& PolyFormat)
{ this->ElementToString(this->DebugString, PolyFormat);
  return true;
}

template <class Element>
void Monomial<Element>::MultiplyBy(Monomial<Element>& m, Monomial<Element>& output)
{ assert(m.NumVariables == this->NumVariables);
  output.init(this->NumVariables);
  output.Coefficient.Assign(m.Coefficient);
  output.Coefficient.MultiplyBy(this->Coefficient);
  for(int i=0; i<m.NumVariables; i++)
    output.degrees[i]=this->degrees[i]+m.degrees[i];
}

template <class Element>
bool Monomial<Element>::HasSameExponent(Monomial& m)
{ assert(m.NumVariables == this->NumVariables);
  for(int i=0; i<m.NumVariables; i++)
    if (this->degrees[i]!=m.degrees[i])
      return false;
  return true;
}

template <class Element>
void Monomial<Element>::IncreaseNumVariables(int increase)
{
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=increase;
  ParallelComputing::CheckPointerCounters();
#endif
  int* newDegrees= new int[NumVariables+increase];
  for(int i=0; i<this->NumVariables; i++)
    newDegrees[i]=degrees[i];
  for(int i=NumVariables; i<this->NumVariables+increase; i++)
    newDegrees[i]=0;
  this->NumVariables+=increase;
  delete [] degrees;
  degrees= newDegrees;
}

template <class Element>
void Monomial<Element>::GetMonomialWithCoeffOne(Monomial<Element>& output)
{ output.Coefficient.Assign(Element::TheRingUnit);
  output.initNoDegreesInit(this->NumVariables);
  for (int i=0; i< this->NumVariables; i++)
    output.degrees[i]=this->degrees[i];
}

template <class Element>
bool Monomial<Element>::operator ==(const Monomial<Element>& m)const
{ for(int i=0; i<this->NumVariables; i++)
    if (this->degrees[i]!=m.degrees[i])
      return false;
  return true;
}

template <class Element>
void Monomial<Element>::operator=(const Monomial<Element>& m)
{ this->initNoDegreesInit(m.NumVariables);
  for (int i=0; i<NumVariables; i++)
    this->degrees[i]=m.degrees[i];
  this->Coefficient.Assign (m.Coefficient);
}

template <class Element>
bool Monomial<Element>::IsAConstant()const
{ for (int i=0; i<this->NumVariables; i++)
    if (this->degrees[i]!=0) return false;
  return true;
}

template <class Element>
void Monomial<Element>::MakeFromRoot(const Element& coeff, intRoot& input)
{ this->init((int)input.size);
  this->Coefficient.Assign(coeff);
  for (int i=0; i<this->NumVariables; i++)
    this->degrees[i]=(int) input.TheObjects[i];
}

template <class Element>
void Monomial<Element>::MakeFromRoot
(const Element& coeff, root& input)
{ this->init((int)input.size);
  this->Coefficient.Assign(coeff);
  for (int i=0; i<this->NumVariables; i++)
    this->degrees[i]=(int) input[i].NumShort;
}

template <class Element>
void Monomial<Element>::MonomialExponentToColumnMatrix(MatrixLargeRational& output)
{ output.init(this->NumVariables, 1);
  for (int i=0; i<this->NumVariables; i++)
    output.elements[i][0].AssignInteger(this->degrees[i]);
}

template <class Element>
void Monomial<Element>::MonomialExponentToRoot(root& output)
{ output.SetSize(this->NumVariables);
  for (int i=0; i<this->NumVariables; i++)
    output.TheObjects[i].AssignInteger(this->degrees[i]);
}

template <class Element>
void Monomial<Element>::MonomialExponentToRoot(intRoot& output)
{ output.SetSize(this->NumVariables);
  for (int i=0; i<this->NumVariables; i++)
    output.TheObjects[i]=this->degrees[i];
}

template <class Element>
void Monomial<Element>::DivideByExponentOnly(intRoot& input)
{ assert(input.size==this->NumVariables);
  for (int i=0; i<this->NumVariables; i++)
    this->degrees[i]-=(int)input.TheObjects[i];
}

template <class Element>
void Monomial<Element>::DivideBy(Monomial<Element>& input, Monomial<Element>& output)
{ output.init(this->NumVariables);
  output.Coefficient.Assign(this->Coefficient);
  output.Coefficient/=input.Coefficient;
  for (int i=0; i<this->NumVariables; i++)
    output.degrees[i]=this->degrees[i]-input.degrees[i];
}

template <class Element>
void Monomial<Element>::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ if (newNumVars<0)
    return;
  int minNumVars=MathRoutines::Minimum((int) this->NumVariables, (int) newNumVars);
  Monomial<Element> tempM;
  tempM.init(newNumVars);
  tempM.Coefficient=this->Coefficient;
  for (int j=0; j<minNumVars; j++)
    tempM.degrees[j]=this->degrees[j];
  this->Assign(tempM);
}

template <class Element>
void Monomial<Element>::DecreaseNumVariables(int increment)
{
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=increment;
  if(ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList)std::exit(0);
#endif
  int* newDegrees= new int[NumVariables-increment];
  for(int i=0; i<this->NumVariables-increment; i++)
    newDegrees[i]=this->degrees[i];
  this->NumVariables-=increment;
  delete [] this->degrees;
  this->degrees= newDegrees;
}

template <class Element>
bool Monomial<Element>::IsPositive()
{ if (this->Coefficient.IsEqualToZero())
    return true;
  for (int i=0; i<this->NumVariables; i++)
    if (this->degrees[i]<0)
      return false;
  return true;
}

template <class Element>
bool Monomial<Element>::IsEqualToZero() const
{ return this->Coefficient.IsEqualToZero();
}

template <class Element>
int Monomial<Element>::HashFunction() const
{ int result=0;
  for (int i=0; i<this->NumVariables; i++)
    result+=this->degrees[i]*SomeRandomPrimes[i];
  return result;
}

template <class Element>
void Monomial<Element>::MakeConstantMonomial(int Nvar, const Element& coeff)
{ this->init(Nvar);
  this->Coefficient.Assign(coeff);
}

template <class Element>
void Monomial<Element>::MakeNVarFirstDegree(int LetterIndex, int NumVars, const Element& coeff)
{ this->MakeConstantMonomial(NumVars, coeff);
  this->degrees[LetterIndex]=1;
}

template <class Element>
void Monomial<Element>::MakeMonomialOneLetter(int NumVars, int LetterIndex, int Power, const Element& coeff)
{ this->init(NumVars);
  this->degrees[LetterIndex]=Power;
  this->Coefficient.Assign(coeff);
}

template <class Element>
int Monomial<Element>::SizeWithoutCoefficient()
{ int Accum =(sizeof(int) * this->NumVariables+sizeof(degrees));
  return Accum;
}

template <class Element>
void Monomial<Element>::Assign(const Monomial<Element>& m)
{ this->initNoDegreesInit(m.NumVariables);
  for (int i=0; i<NumVariables; i++)
    this->degrees[i]=m.degrees[i];
  this->Coefficient.Assign (m.Coefficient);
}

template <class Element>
int Polynomial<Element>::GetMaxPowerOfVariableIndex(int VariableIndex)
{ int result=0;
  for (int i=0; i<this->size; i++)
    result= MathRoutines::Maximum(result, this->TheObjects[i].degrees[VariableIndex]);
  return result;
}

template <class Element>
void Polynomial<Element>::GetConstantTerm(Element& output, const Element& theRingZero)
{ Monomial<Element> tempM;
  tempM.MakeConstantMonomial(this->NumVars, theRingZero);
  int i=this->IndexOfObjectHash(tempM);
  if (i==-1)
    output=theRingZero;
  else
    output=this->TheObjects[i].Coefficient;
}

template <class Element>
void Polynomial<Element>::GetCoeffInFrontOfLinearTermVariableIndex(int index, Element& output, const Element& theRingZero)
{ Monomial<Element> tempM;
  tempM.MakeNVarFirstDegree(index, this->NumVars, theRingZero);
  int i=this->IndexOfObjectHash(tempM);
  if (i==-1)
    output=theRingZero;
  else
    output=this->TheObjects[i].Coefficient;
}

template <class Element>
void Polynomial<Element>::AddConstant(const Element& theConst)
{ Monomial<Element> tempMon;
  tempMon.MakeConstantMonomial(this->NumVars, theConst);
  this->AddMonomial(tempMon);
}

template <class TemplateMonomial, class Element>
inline void TemplatePolynomial<TemplateMonomial, Element>::operator =(const TemplatePolynomial<TemplateMonomial, Element>& right)
{ this->CopyFromPoly(right);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::WriteToFile
(std::fstream& output)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "nv: " << this->NumVars << " numMons: " << this->size;
  for (int i=0; i<this->size; i++)
  { output << " ";
    if (i>0)
      if (! this->TheObjects[i].Coefficient.BeginsWithMinus())
        output << "+ ";
    this->TheObjects[i].Coefficient.WriteToFile(output);
    for (int j=0; j<this->NumVars; j++)
      if(this->TheObjects[i].degrees[j]!=0)
        output << " x_ " << j+1 << " ^ " << this->TheObjects[i].degrees[j];
  }
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class TemplateMonomial, class Element>
inline bool TemplatePolynomial<TemplateMonomial, Element>::ReadFromFile
(std::fstream& input, GlobalVariables* theGlobalVariables)
{ int numReadWords, candidateVars, varIndex, targetSize;
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string ReaderString;
  bool result=true;
  input >> ReaderString >> candidateVars >> ReaderString >> targetSize;
  if (ReaderString!="numMons:" || candidateVars<0)
  { assert(false);
    return false;
  }
  this->Nullify(candidateVars);
  TemplateMonomial tempM;
  tempM.init(candidateVars);
  this->MakeActualSizeAtLeastExpandOnTop(targetSize);
  input.ignore();
  for (int i=0; i<targetSize; i++)
  { if (input.peek()=='+')
      input >> ReaderString;
    tempM.init(candidateVars);
    tempM.Coefficient.ReadFromFile(input);
    input.ignore();
    for (int j=0; j<candidateVars; j++)
    { if (input.peek()!='x')
        break;
      input >> ReaderString;
      assert(ReaderString=="x_");
      input >> varIndex;
      varIndex--;
      if (varIndex>=candidateVars)
      { result=false;
        break;
      }
      input >> ReaderString >> tempM.degrees[varIndex];
      input.ignore();
    }
    if (!result)
      break;
    this->AddMonomial(tempM);
  }
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  assert(numReadWords==0);
  return result;
}

template <class Element>
void Polynomial<Element>::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ if (newNumVars==this->NumVars)
    return;
  Polynomial<Element> Accum;
  Accum.Nullify(newNumVars);
  Accum.MakeActualSizeAtLeastExpandOnTop(this->size);
  Monomial<Element> tempM;
  tempM.init(Accum.NumVars);
  int minNumVars=MathRoutines::Minimum(this->NumVars, Accum.NumVars);
  for (int i=0; i<this->size; i++)
  { for (int j=0; j<minNumVars; j++)
      tempM.degrees[j]=this->TheObjects[i].degrees[j];
    tempM.Coefficient=this->TheObjects[i].Coefficient;
    Accum.AddMonomial(tempM);
  }
  this->CopyFromPoly(Accum);
}

template <class Element>
void Polynomial<Element>::IncreaseNumVariablesWithShiftToTheRight(int theShift, int theIncrease)
{ Polynomial<Element> Accum;
  Accum.Nullify(this->NumVars+theIncrease);
  Accum.MakeActualSizeAtLeastExpandOnTop(this->size);
  Monomial<Element> tempM;
  tempM.init(Accum.NumVars);
  int minNumVars=MathRoutines::Minimum(this->NumVars, Accum.NumVars);
  for (int i=0; i<this->size; i++)
  { for (int j=0; j<minNumVars; j++)
      tempM.degrees[j+theShift]=this->TheObjects[i].degrees[j];
    tempM.Coefficient=this->TheObjects[i].Coefficient;
    Accum.AddMonomial(tempM);
  }
  this->CopyFromPoly(Accum);
}

template <class Element>
void Polynomial<Element>::Evaluate
(const Vector<Element>& input, Element& output, const Element& theRingZero)
{ output=theRingZero;
  Element tempElt;
  for (int i=0; i<this->size; i++)
  { tempElt=this->TheObjects[i].Coefficient;
    for (int j=0; j<this->NumVars; j++)
      for (int k=0; k<this->TheObjects[i].degrees[j]; k++)
      { tempElt*=input.TheObjects[j];
        ParallelComputing::SafePointDontCallMeFromDestructors();
      }
    output+=(tempElt);
  }
}


template <class Element>
void Polynomial<Element>::DecreaseNumVariables(int increment, Polynomial<Element>& output)
{ output.ClearTheObjects();
  Monomial<Element> tempM;
  for (int i=0; i<this->size; i++)
  { tempM.CopyFrom(this->TheObjects[i]);
    tempM.DecreaseNumVariables(increment);
    output.AddMonomial(tempM);
  }
}

template <class TemplateMonomial, class Element>
inline void TemplatePolynomial<TemplateMonomial, Element>::MultiplyBy
(const TemplatePolynomial<TemplateMonomial, Element>& p,
 TemplatePolynomial<TemplateMonomial, Element>& output,
 TemplatePolynomial<TemplateMonomial, Element>& bufferPoly, TemplateMonomial& bufferMon)const
{ if (p.size==0)
  { output.ClearTheObjects();
    return;
  }
  int maxNumMonsFinal=this->size*p.size;
  bufferPoly.MakeActualSizeAtLeastExpandOnTop(maxNumMonsFinal);
  if (maxNumMonsFinal/this->HashSize>3)
    bufferPoly.SetHashSizE(maxNumMonsFinal);
  bufferPoly.Nullify(p.NumVars);
  for (int i=0; i<p.size; i++)
    for (int j=0; j<this->size; j++)
    { this->TheObjects[j].MultiplyBy(p.TheObjects[i], bufferMon);
//      tempM.ComputeDebugString(PolyFormatLocal);
//      Accum.ComputeDebugString();
      bufferPoly.AddMonomial(bufferMon);
      ParallelComputing::SafePointDontCallMeFromDestructors();
//      Accum.ComputeDebugString();
    }
  output.CopyFromPoly(bufferPoly);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::MultiplyBy(const TemplatePolynomial<TemplateMonomial, Element> &p)
{ TemplatePolynomial<TemplateMonomial, Element> bufferPoly;
  TemplateMonomial bufferMon;
  this->MultiplyBy(p, *this, bufferPoly, bufferMon);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::MakeNVarConst(int nVar, const Element& coeff)
{ TemplateMonomial tempM;
  this->ClearTheObjects();
  this->NumVars=nVar;
  tempM.MakeConstantMonomial(nVar, coeff);
  this->AddMonomial(tempM);
}

template <class Element>
bool Polynomial<Element>::IsEqualTo(const Polynomial<Element>& p)const
{ Polynomial<Element> tempPoly;
  tempPoly.CopyFromPoly(p);
  tempPoly.TimesConstant(Element::TheRingMUnit);
  tempPoly.AddPolynomial(*this);
  return tempPoly.IsEqualToZero();
}

template <class TemplateMonomial, class Element>
bool TemplatePolynomial<TemplateMonomial, Element>::IsEqualToZero()const
{ if(this->size==0)
    return true;
  for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].IsEqualToZero())
      return false;
  return true;
}

template <class Element>
void Polynomial<Element>::MakeLinPolyFromRootLastCoordConst(const root& input)
{ this->Nullify((int)input.size);
  Monomial<Element> tempM;
  for (int i=0; i<input.size-1; i++)
  { tempM.MakeNVarFirstDegree(i, input.size, Element::TheRingUnit);
    tempM.Coefficient.Assign(input.TheObjects[i]);
    this->AddMonomial(tempM);
  }
  tempM.init(this->NumVars);
  tempM.Coefficient=input.TheObjects[input.size-1];
  this->AddMonomial(tempM);
}

template <class Element>
void Polynomial<Element>::MakeLinPolyFromRootNoConstantTerm(const root& r)
{ this->Nullify((int)r.size);
  Monomial<Element> tempM;
  for (int i=0; i<r.size; i++)
  { tempM.MakeNVarFirstDegree(i, (int)r.size, Element::TheRingUnit);
    tempM.Coefficient.Assign(r.TheObjects[i]);
    this->AddMonomial(tempM);
  }
}

template <class Element>
void Polynomial<Element>::MakeMonomialOneLetter(int NumVars, int LetterIndex, int Power, const Element& Coeff)
{ this->ClearTheObjects();
  this->NumVars= NumVars;
  Monomial<Element> tempM;
  tempM.MakeMonomialOneLetter(NumVars, LetterIndex, Power, Coeff);
  this->AddOnTopHash(tempM);
}

template <class TemplateMonomial, class Element>
inline int TemplatePolynomial<TemplateMonomial, Element>::HasSameExponentMonomial(TemplateMonomial& m)
{ return this->IndexOfObjectHash(m);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::AddPolynomial(const TemplatePolynomial<TemplateMonomial, Element>& p)
{ this->MakeActualSizeAtLeastExpandOnTop(p.size+this->size);
  //std::string tempS1;
  /*if (QuasiPolynomialOld::flagAnErrorHasOccurredTimeToPanic)
  {  std::string tempS;
    RandomCodeIDontWantToDelete::EvilList1.AddOnTop(tempS);
    tempS1=& RandomCodeIDontWantToDelete::EvilList1.TheObjects
                [RandomCodeIDontWantToDelete::EvilList1.size-1];
  }*/
/*  Rational tempRat, tempRat2;
  IntegerPoly* tempP;
  if (IntegerPoly::flagAnErrorHasOccurredTimeToPanic)
  { std::string tempS;
    tempP=(IntegerPoly*)this;
    tempP->Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot, tempRat);
    tempRat.ElementToString(tempS);
    //  currentList->AddOnTop(tempS);
  }*/
  for (int i=0; i<p.size; i++)
  {  /*if (QuasiPolynomialOld::flagAnErrorHasOccurredTimeToPanic)
    {  std::string tempS;
      Rational tempRat;
      QuasiPolynomialOld* tempP;
      tempP=(QuasiPolynomialOld*)this;
      tempP->Evaluate(partFraction::theVectorToBePartitioned, tempRat);
      tempRat.ElementToString(tempS);
    //  currentList->AddOnTop(tempS);
      tempS1->append(tempS);
      tempS1->append("\n");
    }*/
    ParallelComputing::SafePointDontCallMeFromDestructors();
    this->AddMonomial(p.TheObjects[i]);
  }
  /*if (IntegerPoly::flagAnErrorHasOccurredTimeToPanic)
  { std::string tempS1, tempS2;
    tempP->Evaluate(oneFracWithMultiplicitiesAndElongations::CheckSumRoot, tempRat2);
    tempRat.ElementToString(tempS1);
    tempRat2.ElementToString(tempS2);
    this->ComputeDebugString();
    //assert(tempRat.IsEqualTo(tempRat2));
  }*/
}

template <class TemplateMonomial, class Element>
bool TemplatePolynomial<TemplateMonomial, Element>::HasGEQMonomial(TemplateMonomial& m, int& WhichIndex)
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IsGEQpartialOrder(m))
    { WhichIndex=i;
      return true;
    }
  WhichIndex=-1;
  return false;
}

template <class TemplateMonomial, class Element>
int TemplatePolynomial<TemplateMonomial, Element>::StringPrintOutAppend(std::string& output, const PolynomialOutputFormat& PolyFormat, bool breakLinesLatex)const
{ std::stringstream out;
  int NumChars=0;
  int TotalNumLines=0;
  std::string tempS;
  const std::string LatexBeginString=""; // "\\begin{eqnarray*}\n&&";
//  if (PolyFormat.UsingLatexFormat)
//  { out <<LatexBeginString;
//  }
  if (this->size==0)
  { output.append("0");
    return 0;
  }
  List<TemplateMonomial> sortedMons;
  sortedMons.CopyFromBase(*this);
  sortedMons.QuickSortDescending();
  for (int i=0; i<sortedMons.size; i++)
  { sortedMons[i].ElementToString(tempS, PolyFormat);
    if (tempS[0]=='0')
      tempS.erase(0, 1);
    if (tempS.size()!=0)
    { if (tempS[0]!='-'){out << "+"; }
      out << tempS;
      if (PolyFormat.UsingLatexFormat)
      { if (((signed)tempS.size())> PolyFormat.LatexMaxLineLength)
        { bool found=false; int extraChars=0;
          for (int j=(signed)tempS.size(); j>=1; j--)
            if (tempS[j]=='\\' && tempS[j-1]=='\\')
            { found=true; break; }
            else
              extraChars++;
          if (found) NumChars=extraChars; else  NumChars+=extraChars;
        }
        else
          NumChars+= tempS.size();
        if (breakLinesLatex && NumChars>PolyFormat.LatexMaxLineLength)
        { NumChars=0;
          out << "\\\\&&\n ";
          TotalNumLines++;
        }
      }
      if (PolyFormat.cutOffString && out.str().size()> PolyFormat.cutOffSize)
      { out << "...";
        break;
      }
    }
  }
//  std::string tempS;
  tempS= out.str();
  if (tempS.size()!=0)
    if (tempS[0]=='+')
      tempS.erase(0, 1);
  if (PolyFormat.UsingLatexFormat)
    if (tempS[LatexBeginString.size()]=='+')
      tempS.erase(LatexBeginString.size(), 1);
  output.append(tempS);
  return TotalNumLines;
//  if (PolyFormat.UsingLatexFormat)
//  { output.append("\\end{eqnarray*}");
//  }
}

template <class Element>
void Polynomial<Element>::TimesConstant(const Element& r)
{ if(r.IsEqualToZero())
    this->ClearTheObjects();
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.MultiplyBy(r);
}

template <class Element>
int Polynomial<Element>::GetIndexMaxMonomialLexicographicLastVariableStrongest()
{ if (this->size==0)
    return -1;
  int result;
  result = 0;
  for (int i=1; i<this->size; i++)
    if (this->TheObjects[i].IsGEQLexicographicLastVariableStrongest(this->TheObjects[result]))
      result=i;
  return result;
}

template <class Element>
int Polynomial<Element>::GetIndexMaxMonomialTotalDegThenLexicographic()
{ if (this->size==0)
    return -1;
  int result;
  result = 0;
  for (int i=1; i<this->size; i++)
    if (this->TheObjects[i].IsGEQTotalDegThenLexicographic(this->TheObjects[result]))
      result=i;
  return result;
}

template <class Element>
void Polynomial<Element>::ScaleToPositiveMonomials(Monomial<Element>& outputScale)
{ outputScale.init(this->NumVars);
  outputScale.Coefficient.Assign(Element::TheRingUnit);
  for (int i=0; i<this->NumVars; i++)
  { outputScale.degrees[i]= 0;
    for (int j=0; j<this->size; j++)
      outputScale.degrees[i]= ::MathRoutines::Minimum(outputScale.degrees[i], this->TheObjects[j].degrees[i]);
  }
  Monomial<Element> tempMon;
  tempMon.Assign(outputScale);
  tempMon.InvertDegrees();
  this->MultiplyByMonomial(tempMon);
}

template <class Element>
bool Polynomial<Element>::IsProportionalTo(const Polynomial<Element>& other, Element& TimesMeEqualsOther, const Element& theRingUnit)const
{ if (this->size!=other.size)
    return false;
  if (other.size==0)
  { TimesMeEqualsOther=theRingUnit;
    return true;
  }
  Monomial<Element>& firstMon= this->TheObjects[0];
  int indexInOther=other.IndexOfObjectHash(firstMon);
  if (indexInOther==-1)
    return false;
  TimesMeEqualsOther=other.TheObjects[indexInOther].Coefficient;
  TimesMeEqualsOther/=firstMon.Coefficient;
  Polynomial<Element> tempP;
  tempP.Assign(*this);
  tempP.TimesConstant(TimesMeEqualsOther);
  tempP.Subtract(other);
  return tempP.IsEqualToZero();
}

template <class Element>
void Polynomial<Element>::DivideBy(const Polynomial<Element>& inputDivisor, Polynomial<Element>& outputQuotient, Polynomial<Element>& outputRemainder)const
{ assert(&outputQuotient!=this && &outputRemainder!=this && &outputQuotient!=&outputRemainder);
  outputRemainder.Assign(*this);
  Monomial<Element> scaleRemainder;
  Monomial<Element> scaleInput;
  Polynomial<Element> tempInput;
  tempInput.Assign(inputDivisor);
  outputRemainder.ScaleToPositiveMonomials(scaleRemainder);
  tempInput.ScaleToPositiveMonomials(scaleInput);
  int remainderMaxMonomial=outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int inputMaxMonomial= tempInput.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  outputQuotient.Nullify(this->NumVars);
  if (remainderMaxMonomial==-1)
    return;
  outputQuotient.MakeActualSizeAtLeastExpandOnTop(this->size);
  Monomial<Element> tempMon;
  tempMon.init(this->NumVars);
  Polynomial<Element> tempP;
  tempP.MakeActualSizeAtLeastExpandOnTop(this->size);
  //if (this->flagAnErrorHasOccuredTimeToPanic)
  //{ this->ComputeDebugString();
   // tempInput.ComputeDebugString();
  //}
  assert(remainderMaxMonomial<outputRemainder.size);
  assert(inputMaxMonomial<tempInput.size);
  while (outputRemainder.TheObjects[remainderMaxMonomial].IsGEQLexicographicLastVariableStrongest(tempInput.TheObjects[inputMaxMonomial]))
  { assert(remainderMaxMonomial<outputRemainder.size);
    outputRemainder.TheObjects[remainderMaxMonomial].DivideBy(tempInput.TheObjects[inputMaxMonomial], tempMon);
    if (!tempMon.IsPositive())
      break;
    if (this->flagAnErrorHasOccuredTimeToPanic)
      tempMon.ComputeDebugString();
    outputQuotient.AddMonomial(tempMon);
    tempP.Assign(tempInput);
    tempP.MultiplyByMonomial(tempMon);
    tempP.TimesConstant(Element::TheRingMUnit);
    outputRemainder.AddPolynomial(tempP);
    remainderMaxMonomial= outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
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

template <class Element>
void Polynomial<Element>::DivideByConstant(const Element& r)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient/=r;
}

template <class Element>
void Polynomial<Element>::TimesInteger(int a)
{ Rational r;
  r.AssignInteger(a);
  this->TimesRational(r);
}

template <class TemplateMonomial, class Element>
inline void TemplatePolynomial<TemplateMonomial, Element>::CopyFromPoly(const TemplatePolynomial<TemplateMonomial, Element>& p)
{ this->Assign(p);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::Assign (const TemplatePolynomial<TemplateMonomial, Element>& p)
{ this->CopyFromHash(p);
  this->NumVars= p.NumVars;
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::SubtractMonomial(const TemplateMonomial& m)
{ if (m.Coefficient.IsEqualToZero())
    return;
  int j= this->IndexOfObjectHash(m);
  if (j==-1)
  { if (!m.IsEqualToZero())
      this->AddOnTopHash(m);
  } else
  { this->TheObjects[j].Coefficient-=m.Coefficient;
    if (this->TheObjects[j].IsEqualToZero())
      this->PopIndexSwapWithLastHash(j);
  }
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::AddMonomial(const TemplateMonomial& m)
{ if (m.Coefficient.IsEqualToZero())
    return;
  int j= this->IndexOfObjectHash(m);
  if (j==-1)
  { if (!m.IsEqualToZero())
      this->AddOnTopHash(m);
  } else
  { this->TheObjects[j].Coefficient+=m.Coefficient;
    if (this->TheObjects[j].IsEqualToZero())
      this->PopIndexSwapWithLastHash(j);
  }
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::MultiplyByMonomial(TemplateMonomial& m)
{ this->MultiplyByMonomial(m, *this);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::MultiplyByMonomial(TemplateMonomial& m, TemplatePolynomial<TemplateMonomial, Element>& output)
{ if (m.IsEqualToZero())
  { output.ClearTheObjects();
    return;
  }
  TemplateMonomial tempM;
  TemplatePolynomial<TemplateMonomial, Element> Accum;
  Accum.MakeActualSizeAtLeastExpandOnTop(this->size);
  Accum.ClearTheObjects();
  Accum.NumVars= this->NumVars;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].MultiplyBy(m, tempM);
    Accum.AddMonomial(tempM);
  }
  output.CopyFromPoly(Accum);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::RaiseToPower(int d, TemplatePolynomial<TemplateMonomial, Element>& output, const Element& theRingUniT)
{ TemplatePolynomial<TemplateMonomial, Element> tempOne;
  tempOne.MakeNVarConst(this->NumVars, theRingUniT);
  if (&output!=this)
    output=*this;
  MathRoutines::RaiseToPower(output, d, tempOne);
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::RaiseToPower(int d, const Element& theRingUniT)
{ if (d!=1)
    this->RaiseToPower(d, *this, theRingUniT);
}

template <class Element>
int Polynomial<Element>::TotalDegree()const
{ int result=0;
  for (int i=0; i<this->size; i++)
    result=MathRoutines::Maximum(this->TheObjects[i].TotalDegree(), result);
  return result;
}

template <class TemplateMonomial, class Element>
void TemplatePolynomial<TemplateMonomial, Element>::Nullify(int NVar)
{ this->ClearTheObjects();
  this->NumVars= NVar;
}

template <class Element>
bool Polynomial<Element>::IsGreaterThanZeroLexicographicOrder()
{ Element tCoeff, sCoeff, Coeff;
  tCoeff.Assign(Element::TheRingZero);
  sCoeff.Assign(Element::TheRingZero);
  Coeff.Assign(Element::TheRingZero);
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].degrees[0]==1)
      tCoeff+=(this->TheObjects[i].Coefficient);
    else
    { if (this->TheObjects[i].degrees[1]==1)
        sCoeff+=(this->TheObjects[i].Coefficient);
      else
        Coeff+=(this->TheObjects[i].Coefficient);
    }
  if (Element::TheRingZero.IsGreaterThan(tCoeff))
    return false;
  if (tCoeff.IsEqualTo(Element::TheRingZero))
  { if (Element::TheRingZero.IsGreaterThan(sCoeff))
      return false;
    if (sCoeff.IsEqualTo(Element::TheRingZero))
      if (Element::TheRingZero.IsGreaterThan(Coeff))
        return false;
  }
  return true;
}

template <class Element>
void Polynomial<Element>::AssignPolynomialLight(const PolynomialLight<Element>& from)
{ this->Nullify(from.NumVars);
  this->MakeActualSizeAtLeastExpandOnTop(from.size);
  for (int i=0; i<from.size; i++)
    this->AddMonomial(from.TheObjects[i]);
}

template <class Element>
void Polynomial<Element>::ComponentInFrontOfVariableToPower(int VariableIndex, ListPointers<Polynomial<Element> >& output, int UpToPower)
{ Monomial<Element> tempM;
  for (int i=0; i<UpToPower; i++)
    output.TheObjects[i]->initHash();
  if (this->size ==0){return; }
  tempM.initNoDegreesInit(this->NumVars-1);
  for (int i=0; i<this->size; i++)
  { int Power = this->TheObjects[i].degrees[VariableIndex];
    if (Power<=UpToPower)
    { int k=0;
      for (int j=0; j<this->NumVars; j++)
        if (j!=VariableIndex)
        { tempM.degrees[k]=this->TheObjects[i].degrees[j];
          k++;
        }
      tempM.Coefficient.Assign(this->TheObjects[i].Coefficient);
      output.TheObjects[Power]->AddMonomial(tempM);
    }
  }
}

template <class Element>
void Polynomial<Element>::Substitution(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, int NumVarTarget, const Element& theRingUnit)
{ //std::cout << "<hr><hr><hr>Making a substitution ";
  //PolynomialOutputFormat theFormat;
  //std::cout << "into this piece of crap:<br> " << this->ElementToString(theFormat);
  Polynomial<Element> Accum, TempPoly;
  Accum.ClearTheObjects();
  Accum.NumVars=NumVarTarget;
  for(int i=0; i<this->size; i++)
  { this->TheObjects[i].Substitution(TheSubstitution, TempPoly, NumVarTarget, theRingUnit);
    Accum.AddPolynomial(TempPoly);
    //std::cout << "<br>So far accum is :<br> " << Accum.ElementToString(theFormat);
  }
  output.CopyFromPoly(Accum);
  //std::cout << "<hr>to finally get<br>" << output.ElementToString(theFormat);
}

template <class Element>
void Polynomial<Element>::Substitution(const List<Polynomial<Element> >& TheSubstitution, int NumVarTarget, const Element& theRingUnit)
{ this->Substitution(TheSubstitution, *this, NumVarTarget, theRingUnit);
}

template <class Element>
void Polynomial<Element>::MakeNVarDegOnePoly(int NVar, int NonZeroIndex, const Element& coeff)
{ this->ClearTheObjects();
  this->NumVars=NVar;
  Monomial<Element> tempM;
  tempM.MakeNVarFirstDegree(NonZeroIndex, NVar, coeff);
  this->AddMonomial(tempM);
}

template <class Element>
void Polynomial<Element>::MakeNVarDegOnePoly(int NVar, int NonZeroIndex1, int NonZeroIndex2, const Element& coeff1, const Element& coeff2)
{ this->ClearTheObjects();
  this->NumVars=NVar;
  Monomial<Element> tempM;
  tempM.MakeNVarFirstDegree(NonZeroIndex1, NVar, coeff1);
  this->AddMonomial(tempM);
  tempM.MakeNVarFirstDegree(NonZeroIndex2, NVar, coeff2);
  this->AddMonomial(tempM);
}

template <class Element>
void Polynomial<Element>::MakeNVarDegOnePoly(int NVar, int NonZeroIndex, const Element& coeff1, const Element& ConstantTerm)
{ this->ClearTheObjects();
  this->NumVars =NVar;
  Monomial<Element> tempM;
  tempM.MakeConstantMonomial(NVar, ConstantTerm);
  this->AddMonomial(tempM);
  tempM.MakeNVarFirstDegree(NonZeroIndex, NVar, coeff1);
  this->AddMonomial(tempM);
}

template <class TemplateMonomial, class Element>
bool TemplatePolynomial<TemplateMonomial, Element>::ComputeDebugString()
{ this->DebugString.clear();
  PolynomialOutputFormat PolyFormatLocal;
  this->StringPrintOutAppend(this->DebugString, PolyFormatLocal, true);
  return true;
}

template <class TemplateMonomial, class Element>
TemplatePolynomial<TemplateMonomial, Element>::TemplatePolynomial()
{ this->NumVars=0;
}

template <class Element>
Polynomial<Element>::Polynomial(int degree, Element& coeff)
{ this->MakeOneVarMonomial(degree, coeff);
}

template <class Element>
void Polynomials<Element>::PrintPolys(std::string &output, Element& TheRingUnit, Element& TheRingZero)
{ std::stringstream out;
  PolynomialOutputFormat PolyFormatLocal;
  for (int i=0; i<this->size; i++)
  { std::string tempS;
    out << i << ". ";
    this->TheObjects[i]->StringPrintOutAppend(tempS, PolyFormatLocal);
    out << tempS << "\r\n";
  }
  output=out.str();
}

template <class Element>
void Polynomials<Element>::MakeIdSubstitution(int numVars, const Element& theRingUnit)
{ this->MakeIdLikeInjectionSub(numVars, numVars, theRingUnit);
}

template <class Element>
void Polynomials<Element>::
MakeIdLikeInjectionSub
(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const Element& theRingUnit)
{ assert(numStartingVars<=numTargetVarsMustBeLargerOrEqual);
  this->SetSize(numStartingVars);
  for (int i=0; i<this->size; i++)
  { Polynomial<Element>& currentPoly=this->TheObjects[i];
    currentPoly.MakeNVarDegOnePoly(numTargetVarsMustBeLargerOrEqual, i, theRingUnit);
  }
}

template <class Element>
void Polynomials<Element>::MakeExponentSubstitution(MatrixIntTightMemoryFit& theSub)
{ Polynomial<Element> tempP;
  Monomial<Element> tempM;
  tempM.init((int)theSub.NumRows);
  tempM.Coefficient.Assign(Element::TheRingUnit);
  this->size=0;
  this->SetSize(theSub.NumCols);
  for (int i=0; i<theSub.NumCols; i++)
  { for (int j=0; j<theSub.NumRows; j++)
      tempM.degrees[j]=(int) theSub.elements[j][i];
    tempP.Nullify((int)theSub.NumRows);
    tempP.AddMonomial(tempM);
//    tempP->ComputeDebugString();
    this->TheObjects[i].CopyFromPoly(tempP);
  }
}

template <class Element>
void Polynomials<Element>::MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<Element>& EndPoint)
{ this->SetSize(numVars);
  for (int i=0; i<numVars-1; i++)
    this->TheObjects[i].MakeNVarDegOnePoly(numVars, i, Element::TheRingUnit);
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
  void ComputeCyclotomic(int n);
  void GetSumPrimitiveRoots(int n, Rational& output);
  void DivOneVarByOneVarPoly(PolynomialRationalCoeff& p, PolynomialRationalCoeff& q, PolynomialRationalCoeff& quotient, PolynomialRationalCoeff& remainder);
  int EulerPhi(int n);
};

class CompositeComplex: public List<BasicComplexNumber>
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
  bool SimplifyWRT(int n);
  bool IsEqualTo(CompositeComplex&c);
  bool IsEqualToZero();
};

class BasicQN
{
private:
  int GaussianEliminationByRowsCol(int Col, bool MoveToRight);
public:
  inline static const std::string GetXMLClassName(){ return "BasicQN";}
  static int NumTotalCreated;
  static ListPointers<BasicQN> GlobalCollectorsBasicQuasiNumbers;
  int CreationNumber;
  std::string DebugString;
  BasicQN(int NumVars)
  { BasicQN::NumTotalCreated++; this->NumVars= NumVars; CreationNumber=NumTotalCreated;
    BasicQN::GlobalCollectorsBasicQuasiNumbers.AddOnTop(this);
  }
  BasicQN()
  { BasicQN::NumTotalCreated++; CreationNumber=NumTotalCreated;
    BasicQN::GlobalCollectorsBasicQuasiNumbers.AddOnTop(this);
  }
  void MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column);
  Rational Coefficient;
  MatrixIntTightMemoryFit Exp;
  MatrixIntTightMemoryFit Nums;
  int NumVars;
  int Den;
  int HashFunction() const;
  void ScaleBy(int DenIncrease);
//  Selection PivotPoints;
  void ExpToDebugString();
  int GaussianEliminationByRows();
//  void AddBasicComplexNumber(BasicComplexNumber& b);
//  void MultiplyByTauSameDenMethod1(int* tau, int k);
  void Assign(const BasicQN& p);
  int InvertModN(int d, int p);
  void SwitchTwoRows(int rowI1, int rowI2, int StartCol);
  void RowPlusScalarTimesRow(int rowInd, int scalar, int otherRowInd, int StartCol);
  void MultiplyRowBy(int rowInd, int scalar, int StartColInd);
  void SetPivotRow(int index, int PivotRowIndex, int Col);
  void MultiplyBySameDen(BasicQN& q);
  void MultiplyBy(BasicQN& q);
  void MultiplyByLargeRational(Rational& r);
  void ComputeDebugString();
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat)const;
  bool HasSameExponent(BasicQN& q);
  bool ExponentIsEqualToZero();
  void DecreaseNumVars(int decrease);
  bool IsEqualToZero();
  void MakeConst(const Rational& Coeff, int NumV);
  void MakeFromNormalAndDirection(root& normal, root& direction, int theMod, Rational& coeff);
  void MakePureQN(int NumVariables, int NonZeroIndex, Rational& coeff, int theExp, int Num, int theDen);
  void BasicQNToComplexQN(CompositeComplexQN& output);
  //for format of the substitution see class qQPSub
  void LinearSubstitution(QPSub& theSub);
  void operator =(const BasicQN& q);
  bool operator ==(BasicQN& q);
  void Simplify();
  void GetCoeffInFrontOfLast(Rational& output);
  void Evaluate(List<int>& theVars, Rational& output);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input, int NumV);
//  void MakeQN(PolynomialRationalCoeff& exp, Rational& coeff);
};

class QuasiNumber :public HashedList<BasicQN>
{
public:
  std::string DebugString;
  int NumVariables;
  bool ComputeDebugString();
  bool IsEqualTo(const QuasiNumber& q)const;
  bool IsEqualToZero()const;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void AddBasicQuasiNumber(BasicQN& q);
  void Add(const QuasiNumber &q){this->operator+=(q);}
  void operator+=(const QuasiNumber &q);
  void MultiplyByBasicQuasiNumber(BasicQN& q);
  void MultiplyBy(const QuasiNumber& q);
  void Assign(const QuasiNumber& q);
  void AssignLargeRational(int NumVars, const Rational& coeff);
  void AssignInteger(int NumVars, int x);
  void ElementToString(std::string& output)const{PolynomialOutputFormat PolyFormatLocal; this->ElementToString(output, PolyFormatLocal);}
  void ElementToString(std::string& output, const PolynomialOutputFormat& PolyFormat)const;
  void MakeConst(Rational& Coeff, int NumV);
  void DivideByRational(Rational& r);
//  void DecreaseNumVars(int decrease);
  void MultiplyByLargeRational(Rational& r);
  void MakePureQN(int NumVar, int NonZeroIndex, Rational& coeff, int theExp, int Num, int theDen);
  void MakeFromNormalAndDirection(root& normal, root& direction, int theMod, Rational& coeff);
  void LinearSubstitution(QPSub& TheSub);
  static QuasiNumber TheRingUnit;
  static QuasiNumber TheRingZero;
  static QuasiNumber TheRingMUnit;
  void MakeZero(int NumVars);
  void Simplify();
  void Evaluate(List<int>& theVars, Rational& output);
  int FindGCMDens();
  void QNtoComplex(CompositeComplexQN& output);
  void MakeQNFromMatrixAndColumn(MatrixLargeRational& theMat, root& column);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  bool BeginsWithMinus()
  { std::string tempS;
    this->ElementToString(tempS);
    if (tempS=="")
      return false;
    return tempS[0]=='-';
  }
//  int NumNonZeroElements();
  QuasiNumber(int numVars){this->NumVariables=numVars; }
  QuasiNumber(){this->NumVariables=0; }
};

class ComplexQN
{
private:
  void CheckCoefficient(){}
public:
  static int NumTotalCreated;
  static ListPointers<ComplexQN> GlobalCollectorsComplexQNs;
  int CreationNumber;
  std::string DebugString;
  ComplexQN(int NumVars)
  { ComplexQN::NumTotalCreated++; this->NumVars= NumVars; CreationNumber=NumTotalCreated;
    ComplexQN::GlobalCollectorsComplexQNs.AddOnTop(this);
  }
  ComplexQN()
  { this->NumVars=NumVars; ComplexQN::NumTotalCreated++; CreationNumber=NumTotalCreated;
    ComplexQN::GlobalCollectorsComplexQNs.AddOnTop(this);
  }
  CompositeComplex Coefficient;
  List<Rational> Exponent;
  int NumVars;
//  void AddBasicComplexNumber(BasicComplexNumber& b);
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
//  void MakePureQN(int NumVars, int NonZeroIndex, Rational&coeff, Rational&ConstExp);
  void MakePureQN(int NumVars, int NonZeroIndex, Rational& coeff, Rational& ConstExp, Rational& Exp);
  //see PolynomialPointersKillOnExit::MakeLinearSubstitution
  //for the substitution format!
  void LinearSubstitution(MatrixLargeRational& TheSub);
//  void RootSubsti
  void operator=(const ComplexQN& q);
  bool operator==(ComplexQN& q);
  void Simplify();
//  void MakeQN(PolynomialRationalCoeff& exp, Rational& coeff);
};

class CompositeComplexQN: public List<ComplexQN>
{
public:
  std::string DebugString;
  int NumVariables;
  bool ComputeDebugString();
  bool IsEqualTo(const CompositeComplexQN& q)const;
  bool IsEqualToZero()const;
  void AddComplexQN(ComplexQN& q);
  inline void Add(const CompositeComplexQN &q){this->operator+=(q);}
  void operator+=(const CompositeComplexQN &q);
  void MultiplyByComplexQN(ComplexQN& q);
  void MultiplyBy(CompositeComplexQN& q);
  void Assign(const CompositeComplexQN& q);
  void ElementToString(std::string& output)const;
  void MakeConst(Rational& Coeff, int numVars);
  void DivideByRational(Rational& r);
  void Simplify();
  void MultiplyByLargeRational(Rational& r);
  void MakePureQN(Rational* expArg, int numVars);
  void MakePureQN(Rational* expArg, int numVars, Rational& coeff);
  void MakePureQN(Rational& constExp, int numVars, Rational& coeff);
  void MakePureQN(int numVars, int NonZeroIndex, Rational&coeff, Rational& Exp);
  void MakePureQN(int numVars, int NonZeroIndex, Rational&coeffExp, Rational& ConstExp, Rational& Coeff);
  //see PolynomialPointersKillOnExit::MakeLinearSubstitution
  //for the substitution format!
  void LinearSubstitution(MatrixLargeRational& TheSub);
  static CompositeComplexQN TheRingUnit;
  static CompositeComplexQN TheRingZero;
  static CompositeComplexQN TheRingMUnit;
  bool ElementHasPlusInFront();
  int NumNonZeroElements()const;
  CompositeComplexQN(int numVars){this->size=0; this->NumVariables=numVars; }
  CompositeComplexQN(){this->size=0; this->NumVariables=0; }
  CompositeComplexQN(Rational* expArg, int numVars, Rational& coeff){this->MakePureQN(expArg, numVars, coeff); }
};

class CompositeComplexQNPoly: public Polynomial<CompositeComplexQN>
{
public:
void AssignQP(QuasiPolynomialOld& q);
};

class QuasiPolynomialOld: public Polynomial<QuasiNumber>
{
public:
  static ListPointers<QuasiPolynomialOld> GlobalCollectorsPolys;
  static int TotalCreatedPolys;
  int CreationNumber; //for debug purposes
  static PrecomputedTauknPointersKillOnExit* PrecomputedTaus;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void AssignPolynomialRationalCoeff(PolynomialRationalCoeff& p);
  void MakeTauknp(int k, int n);
  void MakePureQuasiPolynomial(PolynomialRationalCoeff& p, int NumVars);
  void RationalLinearSubstitution(QPSub& TheSub, QuasiPolynomialOld& output);
  void IntegrateDiscreteFromZeroTo(QPSub& EndPointSub, QuasiPolynomialOld &output, PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
  void IntegrateDiscreteInDirectionFromZeroTo(QPSub& EndPointSub, QPSub& DirectionSub, QuasiPolynomialOld &output, PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
  void IntegrateDiscreteInDirectionFromOldChamber(root& direction, root& normal, QuasiPolynomialOld& OldChamberPoly, QuasiPolynomialOld& output, PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
  void WriteComplexFormToDebugString();
  void TimesInteger(int x);
  void operator*=(const Rational& x);
  void DivByInteger(int x);
  void Evaluate(intRoot& values, Rational& output);
  int SizeWithoutDebugString();
  void operator=(const QuasiPolynomialOld& right);
  void Simplify();
  QuasiPolynomialOld();
  void Nullify(int numVars);
};

class CompositeComplexQNSub
{
public:
  MatrixLargeRational MatrixForCoeffs;
  PolynomialsRationalCoeff RationalPolyForm;
  void MakeLinearSubIntegrand(root& normal, root&direction, Rational& Correction, GlobalVariables& theGlobalVariables);
  void MakeSubNVarForOtherChamber(root& direction, root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
  void MakeSubAddExtraVarForIntegration(root& direction);
};

class QuasiMonomial: public Monomial<QuasiNumber>
{
public:
  void IntegrateDiscreteInDirectionFromZeroTo(QPSub& EndPointSub, QPSub& DirectionSub, QuasiPolynomialOld& output, PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
  void IntegrateDiscreteFromZeroTo(QPSub& EndPointSub, QuasiPolynomialOld& output, PrecomputedQuasiPolynomialIntegrals& PrecomputedDiscreteIntegrals);
  void RationalLinearSubstitution(QPSub& TheSub, QuasiPolynomialOld& output);
};

class PrecomputedTaukn
{
public:
  int k, n;
  CompositeComplexQN Taukn;
  PrecomputedTaukn(int theK, int theN){k=theK; n=theN; }
  PrecomputedTaukn(){}
};

class PrecomputedTauknPointersKillOnExit: public ListPointers<PrecomputedTaukn>
{
private:
  void ComputeTaukn(int k, int n, CompositeComplexQN& output);
public:
  void GetTaukn(int k, int n, CompositeComplexQN& output);
  ~PrecomputedTauknPointersKillOnExit(){this->KillAllElements(); }
};

class PrecomputedQuasiPolynomialIntegral
{
public:
  int Num;
  int Den;
  int degree;
  QuasiPolynomialOld Value;
  void ComputeValue(PolynomialsRationalCoeff& PreComputedBernoulli);
  void operator=(PrecomputedQuasiPolynomialIntegral& right);
};

class PrecomputedQuasiPolynomialIntegrals: public List<PrecomputedQuasiPolynomialIntegral>
{
public:
  static PolynomialsRationalCoeff* PreComputedBernoulli;
  void GetQuasiPolynomialIntegral(int Num, int Den, int degree, QuasiPolynomialOld& output);
};

class QPSub
{
public:
  MatrixIntTightMemoryFit TheQNSub;
  int QNSubDen;
  std::string ElementToString();
  PolynomialsRationalCoeff RationalPolyForm;
  //PolynomialsRationalCoeff RationalPolyForm;
  //format of the substitution:
  //TheQNSub has rows for each new variable
  //plus an extra row for the constant terms of the linear expressions and
  //columns for each of the substituted variables.
  //in the substitution
  //x_1 \mapsto x_1+2x_2+4x_3+3
  //x_2 \mapsto x_1-x_2-2x_3+1
  // QPSub should look like (2 columns, 4 rows):
  // 1   1
  // 2  -1
  // 4  -2
  // 3   1
  void MakeSubFromMatrixInt(MatrixIntTightMemoryFit& theMat);
  void MakeSubFromMatrixIntAndDen(MatrixIntTightMemoryFit& theMat, int Den);
  void MakeSubFromMatrixRational(MatrixLargeRational& theMat);
  void MakeLinearSubIntegrand(root& normal, root& direction, Rational& Correction, GlobalVariables& theGlobalVariables);
  void MakeSubNVarForOtherChamber(root& direction, root& normal, Rational& Correction, GlobalVariables& theGlobalVariables);
  void MakeSubAddExtraVarForIntegration(root& direction);
  void MakeSubFromPolynomialsRationalCoeff(PolynomialsRationalCoeff& input);
  void operator=(const QPSub& other){this->TheQNSub=other.TheQNSub; this->QNSubDen=other.QNSubDen; this->RationalPolyForm=other.RationalPolyForm;}
};

template <class Object>
class ListPointersKillOnExitFakeSize: public ListPointersKillOnExit<Object>
{
public:
  int FakeSize;
  ListPointersKillOnExitFakeSize(){this->FakeSize=0; }
  void setFakeSize(int theFakeSize, int param);
  void Nullify(int param);
};

template <class Object>
void ListPointersKillOnExitFakeSize<Object>::Nullify(int param)
{ this->FakeSize=0;
}

template <class Object>
void ListPointersKillOnExitFakeSize<Object>::setFakeSize(int theFakeSize, int param)
{ if (theFakeSize>this->size)
    this->resizeToLargerCreateNewObjects(theFakeSize-this->size);
  for (int i=this->FakeSize; i<theFakeSize; i++)
    this->TheObjects[i]->Nullify(param);
  this->FakeSize =theFakeSize;
}

class SortedQPs : public ListPointersKillOnExitFakeSize<ListPointersKillOnExitFakeSize<ListPointersKillOnExitFakeSize<QuasiPolynomialOld> > >
{
public:
  void AddToEntry(int x, int y, int z, QuasiMonomial& QM);
};

class oneFracWithMultiplicitiesAndElongations
{
public:
  std::string DebugString;
  ListLight<int> Multiplicities;
  ListLight<int> Elongations;
  void ComputeDebugString();
  void ComputeDebugStringBasisChange(MatrixIntTightMemoryFit& VarChange);
  void AddMultiplicity(int MultiplicityIncrement, int Elongation);
  int IndexLargestElongation();
  int GetLargestElongation();
  void GetPolyDenominator(Polynomial<LargeInt>& output, int MultiplicityIndex, intRoot& theExponent);
  int GetMultiplicityLargestElongation();
  int GetLCMElongations();
  int GetTotalMultiplicity() const;
  void invert();
  void init();
  static root GetCheckSumRoot(int NumVars);
  int HashFunction() const;
  void ComputeOneCheckSum(Rational& output, intRoot& theExp, int theDimension);
  bool IsHigherThan(oneFracWithMultiplicitiesAndElongations& f);
  void operator=(oneFracWithMultiplicitiesAndElongations& right);
  bool operator==(oneFracWithMultiplicitiesAndElongations& right);
  void ElementToString(std::string& output, int index, bool LatexFormat);
  void ElementToStringBasisChange(partFractions& owner, MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, int index, int theDimension, PolynomialOutputFormat& PolyFormatLocal);
  void OneFracToStringBasisChange(partFractions& owner, int indexElongation, MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, int indexInFraction, int theDimension, PolynomialOutputFormat& PolyFormatLocal);
};

class rootWithMultiplicity: public root
{
public:
  int multiplicity;
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  void operator=(const rootWithMultiplicity& right) { this->multiplicity= right.multiplicity; this->root::operator= (right); }
  bool operator==(const rootWithMultiplicity& right) { return this->root::operator ==(right) && this->multiplicity== right.multiplicity; }
  void GetSum(root& output){ output.Assign(*this); output.MultiplyByInteger(this->multiplicity); }
};

class rootsWithMultiplicity: public List<rootWithMultiplicity>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  void BubbleSort()
  { for (int i=0; this->size; i++)
      for (int j=i+1; j<this->size; j++)
        if (this->TheObjects[j].IsGreaterThanOrEqualTo(this->TheObjects[i]))
        { rootWithMultiplicity tempRoot;
          tempRoot= this->TheObjects[i];
          this->TheObjects[i]=this->TheObjects[j];
          this->TheObjects[j]=tempRoot;
        }
  }
  void GetSum(root& output, int theDimension)
  { output.MakeZero(theDimension);
    root tempRoot;
    for(int i=0; i<this->size; i++)
    { this->TheObjects[i].GetSum(tempRoot);
      output.Add(tempRoot);
    }
  }
};

class rootsWithMultiplicitiesContainer: public List<rootsWithMultiplicity>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
};

class intRoots: public List<intRoot>
{
public:
  void AssignRoots(roots& r);
  void ElementToString(std::string& output);
  void BubbleSort(intRoot* weights);
  void ComputeDebugString();
  std::string DebugString;
};

class RootToIndexTable : public HashedList<intRoot>
{
  RootToIndexTable(const RootToIndexTable& other);
public:
  MatrixIntTightMemoryFit TableAllowedAminusB;
  MatrixIntTightMemoryFit TableAllowedAminus2B;
  Selection IndicesRedundantShortRoots;
  List<int> IndicesDoublesOfRedundantShortRoots;
  int NumNonRedundantShortRoots;
  intRoot weights;
  void initFromRoots(intRoots& theAlgorithmBasis, intRoot* theWeights);
  int AddRootAndSort(intRoot& theRoot);
  int AddRootPreserveOrder(intRoot& theRoot);
  int getIndex(intRoot& TheRoot);
  int getIndexDoubleOfARoot(intRoot& TheRoot);
  void ComputeTable(int theDimension);
  void operator=(const RootToIndexTable& other)
  { this->CopyFromHash(other);
    this->TableAllowedAminus2B=other.TableAllowedAminus2B;
    this->IndicesRedundantShortRoots=other.IndicesRedundantShortRoots;
    this->IndicesDoublesOfRedundantShortRoots=other.IndicesDoublesOfRedundantShortRoots;
    this->NumNonRedundantShortRoots=other.NumNonRedundantShortRoots;
    this->weights=other.weights;
  }
  RootToIndexTable(){}
};

class partFractionPolynomials: public List<PolynomialRationalCoeff>
{
public:
  roots LatticeIndicators;
  MatrixLargeRational theNormals;
  void CheckConsistency(root& RootLatticeIndicator, PolynomialRationalCoeff& input);
  void initLatticeIndicatorsFromPartFraction(partFractions& ownerExpression, partFraction& owner, GlobalVariables& theGlobalVariables, int theDimension);
  void AddPolynomialLargeRational(root& rootLatticeIndicator, PolynomialRationalCoeff& input);
  void ComputeQuasiPolynomial(QuasiPolynomialOld& output, bool RecordNumMonomials, int theDimension, GlobalVariables& theGlobalVariables);
};

class Lattice
{
  void TestGaussianEliminationEuclideanDomainRationals(MatrixLargeRational& output);
public:
  inline static const std::string GetXMLClassName(){ return "Lattice";}
  MatrixLargeRational basisRationalForm;
  Matrix<LargeInt> basis;
  LargeIntUnsigned Den;
  int GetDim()const{return this->basis.NumCols;}
  int GetRank()const{return this->basis.NumRows;}
  void IntersectWith(const Lattice& other);
  bool FindOnePreimageInLatticeOf
    (const MatrixLargeRational& theLinearMap, const roots& input, roots& output, GlobalVariables& theGlobalVariables)
;
  void IntersectWithPreimageOfLattice
  (const MatrixLargeRational& theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
;
  void IntersectWithLineGivenBy(root& inputLine, root& outputGenerator);
  static bool GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection
  (root& startingPoint, root& theAffineHyperplane, root& theDirection, root& outputPoint)
  ;
  void GetDefaultFundamentalDomainInternalPoint(root& output);
  bool GetInternalPointInConeForSomeFundamentalDomain
(root& output, Cone& coneContainingOutputPoint, GlobalVariables& theGlobalVariables)
  ;
  void GetRootOnLatticeSmallestPositiveProportionalTo
(root& input, root& output, GlobalVariables& theGlobalVariables)
  ;
  void GetRougherLatticeFromAffineHyperplaneDirectionAndLattice
  (const root& theDirection, root& outputDirectionMultipleOnLattice, root& theShift, root& theAffineHyperplane,
   roots& outputRepresentatives,
   roots& movementInDirectionPerRepresentative,
   Lattice& outputRougherLattice,
   GlobalVariables& theGlobalVariables)
     ;
  void ApplyLinearMap
  (MatrixLargeRational& theMap, Lattice& output)
  ;
  void IntersectWithBothOfMaxRank(const Lattice& other);
  void GetDualLattice(Lattice& output)const;
  bool IsInLattice(const Vector<Rational>& theVector)const
  { Vector<Rational> tempVect=theVector;
    if (!this->ReduceVector(tempVect))
      return false;
    return tempVect.IsEqualToZero();
  }
  //returns false if the vector is not in the vector space spanned by the lattice
  bool ReduceVector(Vector<Rational>& theVector) const;
  //In the following two functions, the format of the matrix theSub of the substitution is as follows.
  //Let the ambient dimension be n, and the coordinates be x_1,..., x_n.
  //Let the new vector space be of dimension m, with coordinates y_1,..., y_m.
  //Then theSub is an n by m matrix, where the i^th row of the matrix gives the expression of x_i via the y_j's.
  //In addition, we require that n>=m (otherwise, in general, we do not expect to get a lattice).
  //For example, if we want to carry out the substitution
  //x_1=y_1+y_2, x_2=y_1-y_2, x_3=y_1, then
  //theSub should be initialized as:
  //1  1
  //1 -1
  //1  0
  bool SubstitutionHomogeneous
    (const MatrixLargeRational& theSub, GlobalVariables& theGlobalVariables)
;
  bool SubstitutionHomogeneous
    (const PolynomialsRationalCoeff& theSub, GlobalVariables& theGlobalVariables)
;
//the following function follows the same convention as the preceding except that we allow n<m. However,
// in order to assure that the preimage of the lattice is a lattice,
//we provide as input an ambient lattice in the new vector space of dimension m
  bool SubstitutionHomogeneous
    (const MatrixLargeRational& theSub, Lattice& resultIsSubsetOf, GlobalVariables& theGlobalVariables)
;
  void Reduce
  ()
  ;
  void IntersectWithLinearSubspaceSpannedBy(const roots& theSubspaceBasis);
  void IntersectWithLinearSubspaceGivenByNormals(const roots& theSubspaceNormals);
  void IntersectWithLinearSubspaceGivenByNormal(const root& theNormal);
static bool GetHomogeneousSubMatFromSubIgnoreConstantTerms
(const PolynomialsRationalCoeff& theSub, MatrixLargeRational& output, GlobalVariables& theGlobalVariables)
;
  //returning false means that the lattice given as rougher is not actually rougher than the current lattice
  //or that there are too many representatives
  bool GetAllRepresentatives
  (const Lattice& rougherLattice, roots& output)const
  ;
  bool GetAllRepresentativesProjectingDownTo
  (const Lattice& rougherLattice, roots& startingShifts, roots& output)const
  ;
  inline std::string ElementToString()const{return this->ElementToString(true, false);}
  std::string ElementToString(bool useHtml, bool useLatex)const;
  bool operator==(const Lattice& other){return this->basisRationalForm==other.basisRationalForm;}
  void operator=(const Lattice& other)
  { this->basis=other.basis;
    this->Den=other.Den;
    this->basisRationalForm=other.basisRationalForm;
  }
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
  ;
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables)
  ;
  void MakeZn(int theDim);
  void RefineByOtherLattice(const Lattice& other);
  void MakeFromRoots
  (const roots& input)
  ;
  Lattice(){}
  Lattice(const Lattice& other){this->operator=(other);}
  void MakeFromMat
  (const MatrixLargeRational& input)
  ;
};

class QuasiPolynomial
{
public:
  inline static std::string GetXMLClassName(){ return "Quasipolynomial";}
  int GetNumVars()const{return this->AmbientLatticeReduced.basis.NumRows;}
  GlobalVariables* buffers;
  Lattice AmbientLatticeReduced;
  roots LatticeShifts;
  std::string DebugString;
  List<PolynomialRationalCoeff> valueOnEachLatticeShift;
  std::string ElementToString(bool useHtml, bool useLatex){PolynomialOutputFormat tempFormat; return this->ElementToString(useHtml, useLatex, tempFormat);}
  std::string ElementToString(bool useHtml, bool useLatex, const PolynomialOutputFormat& thePolyFormat);
  void ComputeDebugString(){this->DebugString=this->ElementToString(false, false);}
  Rational Evaluate(const root& input);
  void AddLatticeShift(const PolynomialRationalCoeff& input, const root& inputShift);
  void AddAssumingLatticeIsSame(const QuasiPolynomial& other);
  void MakeRougherLattice(const Lattice& latticeToRoughenBy);
  void MakeFromPolyShiftAndLattice(const PolynomialRationalCoeff& inputPoly, const root& theShift, const Lattice& theLattice, GlobalVariables& theGlobalVariables);
  void MakeZeroLatTiceZn(int theDim);
  void MakeZeroOverLattice(Lattice& theLattice);
//  bool ExtractLinearMapAndTranslationFromSub
//  ()
 // ;
  bool IsEqualToZero()const {return this->valueOnEachLatticeShift.size==0;}
  void Substitution
  (const MatrixLargeRational& mapFromNewSpaceToOldSpace, const root& inputTranslationSubtractedFromArgument,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  void Substitution
  (const MatrixLargeRational& mapFromNewSpaceToOldSpace,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  void Substitution
  (const root& inputTranslationSubtractedFromArgument, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  bool SubstitutionLessVariables
  (const PolynomialsRationalCoeff& theSub, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
  ;
  void operator+=(const QuasiPolynomial& other);
  QuasiPolynomial(){this->buffers=0;}
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  QuasiPolynomial(const QuasiPolynomial& other){this->operator=(other);}
  void operator*=(const Rational& theConst);
  void operator=(const QuasiPolynomial& other)
  { this->AmbientLatticeReduced=other.AmbientLatticeReduced;
    this->LatticeShifts=other.LatticeShifts;
    this->valueOnEachLatticeShift=other.valueOnEachLatticeShift;
    this->buffers=other.buffers;
  }

};

class partFraction: ListLight<oneFracWithMultiplicitiesAndElongations>
{
private:
  void findPivot();
  void findInitialPivot();
  //void intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent);
  bool rootIsInFractionCone (partFractions& owner, root* theRoot, GlobalVariables& theGlobalVariables);
  friend class partFractions;
  friend class partFractionPolynomials;
public:
  std::string DebugString;
  int LastDistinguishedIndex;
  int FileStoragePosition;
  bool PowerSeriesCoefficientIsComputed;
  bool IsIrrelevant;
  bool RelevanceIsComputed;
  List<int> IndicesNonZeroMults;
  PolynomialLight<LargeInt> Coefficient;
  PolyPartFractionNumeratorLight CoefficientNonExpanded;
  bool RemoveRedundantShortRootsClassicalRootSystem(partFractions& owner, root* Indicator, GlobalVariables& theGlobalVariables, int theDimension);
  bool RemoveRedundantShortRoots(partFractions& owner, root* Indicator, GlobalVariables& theGlobalVariables, int theDimension);
  bool AlreadyAccountedForInGUIDisplay;
  static bool flagAnErrorHasOccurredTimeToPanic;
//  static  bool flagUsingPrecomputedOrlikSolomonBases;
  static bool UncoveringBrackets;
  static std::fstream TheBigDump;
  static bool UseGlobalCollector;
  static bool MakingConsistencyCheck;
  static Rational CheckSum, CheckSum2;
  static intRoot theVectorToBePartitioned;
  static ListPointers<partFraction> GlobalCollectorPartFraction;
  void ComputePolyCorrespondingToOneMonomial(PolynomialRationalCoeff& output, int index, roots& normals, partFractionPolynomials* SplitPowerSeriesCoefficient, int theDimension);
  void ComputePolyCorrespondingToOneMonomial
  (partFractions& owner, QuasiPolynomial& outputQP, int monomialIndex, roots& normals, Lattice& theLattice, GlobalVariables& theGlobalVariables)
  ;
  static void EvaluateIntPoly
  (const Polynomial<LargeInt>& input, const root& values, Rational& output)
  ;
  static void MakePolynomialFromOneNormal(root& normal, root& shiftRational, int theMult,  PolynomialRationalCoeff& output);
  void ComputeNormals(partFractions& owner, roots& output, int theDimension, GlobalVariables& theGlobalVariables);
  int ComputeGainingMultiplicityIndexInLinearRelation(partFractions& owner,  MatrixLargeRational& theLinearRelation);
  void UncoverBracketsNumerator(GlobalVariables& theGlobalVariables, int theDimension);
  void GetRootsFromDenominator
  (partFractions& owner, roots& output)
;
  void GetVectorPartitionFunction
  (partFractions& owner, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  LargeInt EvaluateIntPolyAtOne(Polynomial<LargeInt>& input);
  void partFractionToPartitionFunctionSplit(partFractions& owner, QuasiPolynomialOld& output, bool RecordNumMonomials, bool StoreToFile, GlobalVariables& theGlobalVariables, int theDimension);
  //void partFractionToPartitionFunctionStoreAnswer
  //      (  QuasiPolynomialOld& output, bool RecordSplitPowerSeriesCoefficient,
  //        bool StoreToFile);
  bool IsEqualToZero();
  void MakePolyExponentFromIntRoot
  (Polynomial<LargeInt>& output, const intRoot& input)
  ;
  void ComputeDebugString(partFractions& owner, GlobalVariables& theGlobalVariables);
  void ComputeDebugStringBasisChange(MatrixIntTightMemoryFit& VarChange);
  //void InsertNewRootIndex(int index);
  //void MultiplyMinusRootShiftBy (int* theRoot, int Multiplicity);
  void MultiplyCoeffBy(Rational& r);
  void decomposeAMinusNB(int indexA, int indexB, int n, int indexAminusNB, partFractions& Accum, GlobalVariables& theGlobalVariables, root* Indicator);
  bool DecomposeFromLinRelation(MatrixLargeRational& theLinearRelation, partFractions& Accum, GlobalVariables& theGlobalVariables, root* Indicator);
  void ComputeOneCheckSum(partFractions& owner, Rational& output, int theDimension, GlobalVariables& theGlobalVariables);
  void AttemptReduction(partFractions& owner, int myIndex, GlobalVariables& theGlobalVariables, root* Indicator);
  void ReduceMonomialByMonomial(partFractions& owner, int myIndex, GlobalVariables& theGlobalVariables, root* Indicator);
  void ApplySzenesVergneFormula(List<int> &theSelectedIndices, List<int>& theElongations, int GainingMultiplicityIndex, int ElongationGainingMultiplicityIndex, partFractions& Accum, GlobalVariables& theGlobalVariables, root* Indicator);
  void ApplyGeneralizedSzenesVergneFormula(List<int> &theSelectedIndices, List<int>& theGreatestElongations, List<int>& theCoefficients, int GainingMultiplicityIndex, int ElongationGainingMultiplicityIndex, partFractions& Accum, GlobalVariables& theGlobalVariables, root* Indicator);
  bool CheckForOrlikSolomonAdmissibility(List<int>& theSelectedIndices);
  bool reduceOnceTotalOrderMethod(partFractions&Accum, GlobalVariables& theGlobalVariables, root* Indicator);
//  void reduceOnceOrlikSolomonBasis(partFractions&Accum);
  bool reduceOnceGeneralMethodNoOSBasis(partFractions& Accum, GlobalVariables& theGlobalVariables, root* Indicator);
  bool reduceOnceGeneralMethod(partFractions& Accum, GlobalVariables& theGlobalVariables, root* Indicator);
  bool AreEqual(partFraction& p);
  bool IsReduced();
  //the other fraction must have the same denominator, of course
  //owner is passed only in order to read the flags, not used in any other way
  bool AddReturnShouldAttemptReduction(const partFraction& other, const partFractions& owner, GlobalVariables& theGlobalVariables);
  int HashFunction() const;
  int MultiplyByOneFrac(oneFracWithMultiplicitiesAndElongations& f);
  void init(int numRoots);
  //int Elongate(int indexElongatedFraction, int theElongation);
  void ComputeIndicesNonZeroMults();
  bool DecreasePowerOneFrac(int index, int increment);
  //void GetNumerator(PolynomialRationalCoeff& output);
  //void SetNumerator(PolynomialRationalCoeff& input);
  void PrepareFraction(int indexA, int indexB,  int AminusNBindex, bool indexAisNullified, partFraction& output, Polynomial<LargeInt>& AminusNbetaPoly);
  void Assign(const partFraction& p);
  void AssignDenominatorOnly(const partFraction& p);
  void AssignNoIndicesNonZeroMults(partFraction& p);
  int getSmallestNonZeroIndexGreaterThanOrEqualTo(partFractions& owner, int minIndex);
  int ControlLineSizeFracs(std::string& output, PolynomialOutputFormat& PolyFormatLocal);
  int ControlLineSizeStringPolys(std::string& output, PolynomialOutputFormat& PolyFormatLocal);
  //void swap(int indexA, int indexB);
  partFraction();
  ~partFraction();
  void GetPolyReduceMonomialByMonomial(partFractions& owner, GlobalVariables& theGlobalVariables, intRoot& theExponent, int StartMonomialPower, int DenPowerReduction, int startDenominatorPower, Polynomial<LargeInt>& output);
  void ReduceMonomialByMonomialModifyOneMonomial(partFractions& Accum, GlobalVariables& theGlobalVariables, SelectionWithDifferentMaxMultiplicities& thePowers, List<int>&thePowersSigned, Monomial<LargeInt>& input);
  void GetAlphaMinusNBetaPoly(partFractions& owner, int indexA, int indexB, int n, Polynomial<LargeInt>& output, int theDimension);
  void GetNElongationPolyWithMonomialContribution(partFractions& owner, List<int>& theSelectedIndices, List<int>& theCoefficients, List<int>& theGreatestElongations, int theIndex, Polynomial<LargeInt>& output, int theDimension);
  void GetNElongationPoly
  (partFractions& owner, int index, int baseElongation, int LengthOfGeometricSeries, Polynomial<LargeInt>& output, int theDimension);
  static void GetNElongationPoly(intRoot& exponent, int n, Polynomial<LargeInt>& output, int theDimension);
  void GetNElongationPoly(partFractions& owner, int index, int baseElongation, int LengthOfGeometricSeries, PolyPartFractionNumerator& output, int theDimension);
  int GetNumProportionalVectorsClassicalRootSystems(partFractions& owner);
  bool operator==(const partFraction& right);
  void operator=(const partFraction& right);
  void initFromRootSystem(partFractions& owner, intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights);
  bool initFromRoots(partFractions& owner, roots& input);
  int ElementToString
  (partFractions& owner, std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator,
   GlobalVariables& theGlobalVariables,
   PolynomialOutputFormat& PolyFormatLocal)
   ;
  int ElementToStringBasisChange(partFractions& owner, MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, PolynomialOutputFormat& PolyFormatLocal, GlobalVariables& theGlobalVariables);
  void ReadFromFile(partFractions& owner, std::fstream& input, GlobalVariables* theGlobalVariables, int theDimension);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  int GetNumMonomialsInNumerator();
  int SizeWithoutDebugString();
};

class Cone
{
  void ComputeVerticesFromNormalsNoFakeVertices(GlobalVariables& theGlobalVariables);
  bool EliminateFakeNormalsUsingVertices(int theDiM, int numAddedFakeWalls, GlobalVariables& theGlobalVariables);
public:
  inline static const std::string GetXMLClassName(){ return "Cone";}
  bool flagIsTheZeroCone;
  roots Vertices;
  roots Normals;
//  bool flagHasSufficientlyManyVertices;
  int LowestIndexNotCheckedForChopping;
  int LowestIndexNotCheckedForSlicingInDirection;
  //ignore: the following is the information carrying variable. Write in it anything you want, say an index to an array
  //containing large data for the chamber, such as quasipolynomials, etc.
  //int Data;
  std::string ElementToString(PolynomialOutputFormat& theFormat){return this->ElementToString(false, false, theFormat);}
  std::string ElementToString(bool useLatex, bool useHtml, PolynomialOutputFormat& theFormat){return this->ElementToString(useLatex, useHtml, false, false, theFormat);}
  std::string ElementToString(bool useLatex, bool useHtml, bool PrepareMathReport, bool lastVarIsConstant, PolynomialOutputFormat& theFormat);
  std::string DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat);
  std::string DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat);
  void TranslateMeMyLastCoordinateAffinization(root& theTranslationVector);
  bool IsAnHonest1DEdgeAffine(root& vertex1, root& vertex2)
  { int numCommonWalls=0;
    for (int i=0; i<this->Normals.size; i++)
      if(vertex1.ScalarEuclidean(this->Normals[i]).IsEqualToZero() && vertex2.ScalarEuclidean(this->Normals[i]).IsEqualToZero())
      { numCommonWalls++;
        if (numCommonWalls==this->GetDim()-2)
          return true;
      }
    return false;
  }
  bool IsTheEntireSpace()
  { return this->Normals.size==0 && this->flagIsTheZeroCone;
  }
  bool IsAnHonest1DEdgeAffine(int vertexIndex1, int vertexIndex2)
  { root& vertex1=this->Vertices[vertexIndex1];
    root& vertex2=this->Vertices[vertexIndex2];
    return this->IsAnHonest1DEdgeAffine(vertex1, vertex2);
  }
  bool DrawMeLastCoordAffine
  (bool InitDrawVars, DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat,
   int ChamberWallColor=0);
  bool DrawMeProjective
(root* coordCenterTranslation, bool initTheDrawVars, DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat)
  ;
  bool IsInCone(const roots& vertices)
  { for (int i=0; i<vertices.size; i++)
      if (!this->IsInCone(vertices[i]))
        return false;
    return true;
  }
  bool GetLatticePointsInCone
  (Lattice& theLattice, root& theShift, int upperBoundPointsInEachDim, bool lastCoordinateIsOne,
   roots& outputPoints, root* shiftAllPointsBy)
  ;
  bool MakeConvexHullOfMeAnd(const Cone& other, GlobalVariables& theGlobalVariables);
  void ChangeBasis
  (MatrixLargeRational& theLinearMap, GlobalVariables& theGlobalVariables)
    ;
  std::string DebugString;
  int GetDim()
  { if (this->Normals.size==0)
      return 0;
    return this->Normals.TheObjects[0].size;
  }
  void ComputeDebugString(){ PolynomialOutputFormat theFormat; this->DebugString=this->ElementToString(theFormat);}
  void SliceInDirection
  (root& theDirection, ConeComplex& output, GlobalVariables& theGlobalVariables )
;
  bool CreateFromNormalS
  (roots& inputNormals, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
  ;
  //returns false if the cone is non-proper, i.e. when either
  //1) the cone is empty or is of smaller dimension than it should be
  //2) the resulting cone is the entire space
  bool CreateFromNormals
  (roots& inputNormals, GlobalVariables& theGlobalVariables)
  { return this->CreateFromNormalS(inputNormals, false, theGlobalVariables);
  }
  bool CreateFromVertices
  (roots& inputVertices, GlobalVariables& theGlobalVariables)
  ;
  void GetInternalPoint(root& output)
  { if (this->Vertices.size<=0)
      return;
    this->Vertices.sum(output, this->Vertices[0].size);
  }
  root GetInternalPoint(){root result; this->GetInternalPoint(result); return result;}
  int HashFunction()const
  { return this->Vertices.HashFunction();
  }
  bool ProduceNormalFromTwoNormalsAndSlicingDirection
  (root& SlicingDirection, root& normal1, root& normal2, root& output)
  ;
  bool IsInCone(const root& point)const
  { if (this->flagIsTheZeroCone)
      return point.IsEqualToZero();
    Rational tempRat;
    for (int i=0; i<this->Normals.size; i++)
    { root::RootScalarEuclideanRoot(this->Normals.TheObjects[i], point, tempRat);
      if (tempRat.IsNegative())
        return false;
    }
    return true;
  }
  bool ReadFromFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
;
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
;
  bool ReadFromFile
  (std::fstream& input, roots& buffer, GlobalVariables* theGlobalVariables)
;
  void operator=(const Cone& other)
  { //this->flagHasSufficientlyManyVertices=other.flagHasSufficientlyManyVertices;
    this->flagIsTheZeroCone=other.flagIsTheZeroCone;
    this->Vertices=other.Vertices;
    this->Normals=other.Normals;
    this->LowestIndexNotCheckedForSlicingInDirection=other.LowestIndexNotCheckedForSlicingInDirection;
    this->LowestIndexNotCheckedForChopping=other.LowestIndexNotCheckedForChopping;
  }
  Cone(const Cone& other){ this->operator=(other);}
  Cone()
  { this->LowestIndexNotCheckedForSlicingInDirection=0;
    this->LowestIndexNotCheckedForChopping=0;
    this->flagIsTheZeroCone=true;
    //this->flagHasSufficientlyManyVertices=true;
  }
  void IntersectAHyperplane
  (root& theNormal, Cone& outputConeLowerDim, GlobalVariables& theGlobalVariables)
  ;
  bool GetRootFromLPolyConstantTermGoesToLastVariable
  (PolynomialRationalCoeff& inputLPoly, root& output)
  ;
  bool SolveLPolyEqualsZeroIAmProjective
  ( PolynomialRationalCoeff& inputLPoly,
   Cone& outputCone, GlobalVariables& theGlobalVariables
   )
  ;
  bool SolveLQuasiPolyEqualsZeroIAmProjective
  (QuasiPolynomial& inputLQP,
   List<Cone>& outputConesOverEachLatticeShift, GlobalVariables& theGlobalVariables
   )
  ;
  bool operator>(const Cone& other)const
  { return this->Normals>other.Normals;
  }
  bool operator==(const Cone& other)const
  { return this->flagIsTheZeroCone==other.flagIsTheZeroCone && this->Normals==other.Normals;
  }
};

class ConeLatticeAndShift
{
  public:
  inline static std::string GetXMLClassName(){ return "ConeLatticeShift";}
  Cone theProjectivizedCone;
  Lattice theLattice;
  root theShift;
  void FindExtremaInDirectionOverLatticeOneNonParam
(root& theLPToMaximizeAffine, roots& outputAppendLPToMaximizeAffine,
 List<ConeLatticeAndShift>& outputAppend,
 GlobalVariables& theGlobalVariables)
       ;
  void operator=(const ConeLatticeAndShift& other)
  { this->theProjectivizedCone=other.theProjectivizedCone;
    this->theLattice=other.theLattice;
    this->theShift=other.theShift;
  }
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
  ;
  void FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase
(root& theLPToMaximizeAffine, roots& outputAppendLPToMaximizeAffine,
 List<ConeLatticeAndShift>& outputAppend, MatrixLargeRational& theProjectionLatticeLevel,
 GlobalVariables& theGlobalVariables)
 ;
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables)
  ;
  std::string ElementToString(PolynomialOutputFormat& theFormat);
  int GetDimProjectivized(){return this->theProjectivizedCone.GetDim();}
  int GetDimAffine(){return this->theProjectivizedCone.GetDim()-1;}
};

class ConeComplex : public HashedList<Cone>
{
public:
  bool flagChambersHaveTooFewVertices;
  bool flagIsRefined;
//  List<int> NonRefinedChambers;
  int indexLowestNonRefinedChamber;
  HashedList<root> splittingNormals;
  roots slicingDirections;
  Cone ConvexHull;
  std::string DebugString;
  void AssignCombinatorialChamberComplex(CombinatorialChamberContainer& other, GlobalVariables& theGlobalVariables);
  void RefineOneStep(GlobalVariables& theGlobalVariables);
  void Refine(GlobalVariables& theGlobalVariables);
  void RefineMakeCommonRefinement(const ConeComplex& other, GlobalVariables& theGlobalVariables);
  void Sort(GlobalVariables& theGlobalVariables);
  void RefineAndSort(GlobalVariables& theGlobalVariables);
  void FindMaxmumOverNonDisjointChambers
    (roots& theMaximaOverEachChamber, roots& outputMaxima, GlobalVariables& theGlobalVariables)
    ;
  void MakeAffineAndTransformToProjectiveDimPlusOne
  (root& affinePoint, ConeComplex& output, GlobalVariables& theGlobalVariables)
  ;
  int GetDim(){if (this->size<=0) return -1; return this->TheObjects[0].GetDim();}
  bool AddNonRefinedChamberOnTopNoRepetition(Cone& newCone, GlobalVariables& theGlobalVariables);
  void PopChamberSwapWithLast(int index);
  void GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals
    (roots& output)const
  ;
  bool DrawMeLastCoordAffine(bool InitDrawVars, DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat);
  void TranslateMeMyLastCoordinateAffinization(root& theTranslationVector, GlobalVariables& theGlobalVariables);
  void InitFromDirectionsAndRefine
  (roots& inputVectors, GlobalVariables& theGlobalVariables)
  ;
  void InitFromAffineDirectionsAndRefine
  (roots& inputDirections, roots& inputAffinePoints, GlobalVariables& theGlobalVariables)
  ;
  std::string DrawMeToHtmlLastCoordAffine
(DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat)
;
  bool DrawMeProjective
(root* coordCenterTranslation, bool InitDrawVars, DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat)
  ;
  std::string DrawMeToHtmlProjective
(DrawingVariables& theDrawingVariables, PolynomialOutputFormat& theFormat)
;
  std::string ElementToString(){return this->ElementToString(false, false);}
  std::string ElementToString(bool useLatex, bool useHtml);
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  int GetLowestIndexchamberContaining(const root& theRoot)const
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i].IsInCone(theRoot))
        return i;
    return -1;
  }
  bool findMaxLFOverConeProjective
  (Cone& input, List<PolynomialRationalCoeff>& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables)
  ;
  bool findMaxLFOverConeProjective
  (Cone& input, roots& inputLFsLastCoordConst,
   List<int>& outputMaximumOverEeachSubChamber,
   GlobalVariables& theGlobalVariables)
  ;
  void initFromCones
(List<roots>& NormalsOfCones, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
  ;
  void initFromCones
(List<Cone>& NormalsOfCones, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
  ;
  bool SplitChamber
(int indexChamberBeingRefined, bool weAreSlicingInDirection, bool weAreChopping, root& killerNormal, GlobalVariables& theGlobalVariables)
  ;
  void GetNewVerticesAppend
  (Cone& myDyingCone, root& killerNormal, HashedList<root>& outputVertices, GlobalVariables& theGlobalVariables)
  ;
  void init()
  { this->splittingNormals.ClearTheObjects();
    this->slicingDirections.size=0;
    this->ClearTheObjects();
    this->indexLowestNonRefinedChamber=0;
    this->ConvexHull.Normals.size=0;
    this->ConvexHull.Vertices.size=0;
    this->ConvexHull.flagIsTheZeroCone=true;
  }
  ConeComplex(const ConeComplex& other)
  { this->operator=(other);
  }
  ConeComplex()
  { this->flagChambersHaveTooFewVertices=false;
    this->flagIsRefined=false;
  }
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output, theGlobalVariables, -1);}
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimit)
  ;
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables)
  { return this->ReadFromFile(input, theGlobalVariables, -1);
  }
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
  ;
  void operator=(const ConeComplex& other)
  { this->:: HashedList<Cone>::operator=(other);
    this->splittingNormals=other.splittingNormals;
    this->slicingDirections=other.slicingDirections;
    this->indexLowestNonRefinedChamber=other.indexLowestNonRefinedChamber;
    this->flagIsRefined=other.flagIsRefined;
    this->flagChambersHaveTooFewVertices=other.flagChambersHaveTooFewVertices;
  }
};

class partFractions: public HashedList<partFraction>
{ bool ShouldIgnore(GlobalVariables& theGlobalVariables, root* Indicator);
  bool splitPartial(GlobalVariables& theGlobalVariables, root* Indicator);
  void initCommon();
  partFractions(const partFractions& other);
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
  Rational StartCheckSum;
  Rational EndCheckSum;
  static Rational CheckSum;
  bool flagDiscardingFractions;
  bool flagUsingOrlikSolomonBasis;
  bool flagInitialized;
  int LimitSplittingSteps;
  int SplitStepsCounter;
  ConeComplex theChambers;
  CombinatorialChamberContainer theChambersOld;
  static  bool flagSplitTestModeNoNumerators;
  static  bool flagAnErrorHasOccurredTimeToPanic;
  static  bool flagMakingProgressReport;
  static  bool flagUsingCheckSum;
  static int flagMaxNumStringOutputLines;
  void PrepareCheckSums(GlobalVariables& theGlobalVariables);
  std::string DoTheFullComputationReturnLatexFileString
  (GlobalVariables& theGlobalVariables, roots& toBePartitioned, PolynomialOutputFormat& theFormat, std::string* outputHtml)
  ;
  bool ArgumentsAllowed
  (roots& theArguments, std::string& outputWhatWentWrong, GlobalVariables& theGlobalVariables)
  ;
  bool AssureIndicatorRegularity(GlobalVariables& theGlobalVariables, root& theIndicator);
  void CompareCheckSums(GlobalVariables& theGlobalVariables);
  void ComputeDebugString(GlobalVariables& theGlobalVariables);
  void ComputeDebugStringNoNumerator(GlobalVariables& theGlobalVariables);
  void ComputeDebugStringWithVPfunction(GlobalVariables& theGlobalVariables);
  void ComputeDebugStringBasisChange(MatrixIntTightMemoryFit& VarChange, GlobalVariables& theGlobalVariables);
  void initFromRootSystem(intRoots& theFraction, intRoots& theAlgorithmBasis, intRoot* weights, GlobalVariables& theGlobalVariables);
  bool initFromRoots(roots& input, GlobalVariables& theGlobalVariables);
  void initAndSplit(roots& input, GlobalVariables& theGlobalVariables);
  void Run(roots& input, GlobalVariables& theGlobalVariables);
  //row index is the index of the root; column(second) index is the coordinate index
  void RemoveRedundantShortRootsClassicalRootSystem(GlobalVariables& theGlobalVariables, root* Indicator);
  void RemoveRedundantShortRoots(GlobalVariables& theGlobalVariables, root* Indicator);
  bool RemoveRedundantShortRootsIndex(GlobalVariables& theGlobalVariables, root* Indicator, int theIndex);
  bool splitClassicalRootSystem(bool ShouldElongate, GlobalVariables& theGlobalVariables, root* Indicator);
  bool split(GlobalVariables& theGlobalVariables, root* Indicator);
  void ComputeSupport(List<roots>& output, std::stringstream& outputString);
  void ComputeOneCheckSum(Rational& output, GlobalVariables& theGlobalVariables);
  void AccountPartFractionInternals(int sign, int index, root* Indicator, GlobalVariables& theGlobalVariables);
  void AddAndReduce(partFraction& f, GlobalVariables& theGlobalVariables, root* Indicator);
  void AddAlreadyReduced(partFraction& f, GlobalVariables& theGlobalVariables, root* Indicator);
  void PopIndexHashChooseSwapByLowestNonProcessedAndAccount(int index, GlobalVariables& theGlobalVariables, root* Indicator);
  void PopIndexSwapLastHashAndAccount(int index, GlobalVariables& theGlobalVariables, root* Indicator);
  void PrepareIndicatorVariables();
  void initFromOtherPartFractions(partFractions& input, GlobalVariables& theGlobalVariables);
  void IncreaseHighestIndex(int increment);
  std::string ElementToString(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat){std::string tempS; this->ElementToString(tempS, theGlobalVariables, theFormat);  return tempS;}
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat);
  int ElementToString
  (std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator,
   GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
   ;
  int ElementToStringBasisChange
  (MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat,
 bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& PolyFormatLocal)
;
  int ElementToStringOutputToFile(std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables);
  int ElementToStringBasisChangeOutputToFile(MatrixIntTightMemoryFit& VarChange, bool UsingVarChange, std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables);
  bool GetVectorPartitionFunction
  (QuasiPolynomial& output, root& newIndicator, GlobalVariables& theGlobalVariables)
  ;
  bool partFractionsToPartitionFunctionAdaptedToRoot(QuasiPolynomialOld& output, root& newIndicator, bool StoreToFile, bool UseOldData, GlobalVariables& theGlobalVariables, bool ResetRelevance );
  bool VerifyFileComputedContributions(GlobalVariables& theGlobalVariables);
  void WriteToFileComputedContributions(std::fstream& output, GlobalVariables&  theGlobalVariables);
  int ReadFromFileComputedContributions(std::fstream& input, GlobalVariables&  theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void UncoverBracketsNumerators(GlobalVariables& theGlobalVariables, root* Indicator);
  void ResetRelevanceIsComputed(){for (int i=0; i<this->size; i++){this->TheObjects[i].RelevanceIsComputed=false; }; }
  partFractions();
  int SizeWithoutDebugString();
  bool CheckForMinimalityDecompositionWithRespectToRoot(root *theRoot, GlobalVariables& theGlobalVariables);
  void MakeProgressReportSplittingMainPart(GlobalVariables& theGlobalVariables);
  void MakeProgressReportRemovingRedundantRoots(GlobalVariables& theGlobalVariables);
  void MakeProgressReportUncoveringBrackets(GlobalVariables& theGlobalVariables);
  void MakeProgressVPFcomputation(GlobalVariables& theGlobalVariables);
  void ComputeKostantFunctionFromWeylGroup(char WeylGroupLetter, int WeylGroupNumber, QuasiPolynomialOld& output, root* ChamberIndicator, bool UseOldData, bool StoreToFile, GlobalVariables& theGlobalVariables);
  void operator=(const partFractions& other);
};

class ElementWeylGroup: public List<int>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output){output=this->ElementToString();}
  std::string ElementToString() {return this->ElementToString(false, true, "\\eta", 0);}
  std::string ElementToString
  (bool useLatex, bool useHtml, const std::string& simpleRootLetter, List<int>* DisplayIndicesOfSimpleRoots)
  { return this->ElementToString(-1, useLatex, useHtml, simpleRootLetter, "a", DisplayIndicesOfSimpleRoots);
  }
  std::string ElementToString
  (int NumSimpleGens, bool useLatex, bool useHtml, const std::string& simpleRootLetter, const std::string& outerAutoLetter, List<int>* DisplayIndicesOfSimpleRoots)
  ;
  void ComputeDebugString();
  int HashFunction() const;
  void operator=(const ElementWeylGroup& right);
  bool operator==(const ElementWeylGroup& right);
};

class OneVarPolynomials: public List<Polynomial<LargeInt> >
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
//The OneVarIntPolynomial has 2 times the basic size of List of "service"
//RAM usage. On a 32 bit machine that means it has 2*28=56 bytes of extra RAM besides the
//coefficient storage.
class OneVarIntPolynomial
{
public:
  List<int> PolynomialPart;
  List<int> RationalPart;
  std::string DebugString;
  void MakeConst(int c);
  void MakeMonomial(int coeff, int power);
  void AddMonomial(int coeff, int power);
  void MakeQuadratic(int x2Term, int x1Term, int constTerm);
  void Nullify();
  void ComputeDebugString();
  void ElementToString(std::string& output);
  void SubstitutionOneOverX();
  int Substitution(int x);
  void MultiplyBy(OneVarIntPolynomial& p);
  void AddPolynomial(OneVarIntPolynomial& p);
  void Assign(OneVarIntPolynomial& p)
  { this->PolynomialPart.CopyFromBase(p.PolynomialPart);
    this->RationalPart.CopyFromBase(p.RationalPart);
  }
  void ReleaseMemory();
  void operator=(OneVarIntPolynomial& p){this->Assign(p); }
  void FitSize();
  static void SetSizeAtLeastInitProperly(List<int>& theArray, int desiredSize);
};

class OneVarIntPolynomials: public List<OneVarIntPolynomial>
{
public:
  std::string DebugString;
  int theWeightIndex;
  void ComputeDebugString();
  void ElementToString(std::string& output);
  void ElementToString(std::string& output, int KLindex);
};

class VermaModulesWithMultiplicities: public HashedList<root>
{
public:
  WeylGroup* TheWeylGroup;
  List<int> TheMultiplicities;
  List<bool> Explored;
  int NextToExplore;
  int LowestNonExplored;
  List<List<int> > BruhatOrder;
  List<List<int> > SimpleReflectionsActionList;
  List<List<int> > InverseBruhatOrder;
  std::string DebugString;
  //important: in both the R- and KL-polynomials, if a polynomial Rxy is non-zero,
  //then x is bigger than y. This is the opposite to the usually accepted convention!
  //The reason for that is the following: if you want to compute
  //once you are done with computing with a given highest weight,
  //you want to be able to release the used memory; that is why the higher weight must
  //be the first, not the second index!
  List<OneVarIntPolynomials> KLPolys;
  List<OneVarIntPolynomials> RPolys;
  void KLcoeffsToString(List<int>& theKLCoeffs, std::string& output);
  void FindNextToExplore();
  int FindLowestBruhatNonExplored();
  int FindHighestBruhatNonExplored(List<bool>& theExplored);
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
  void ComputeKLcoefficientsFromIndex(int ChamberIndex, List<int>& output);
  void ComputeKLcoefficientsFromChamberIndicator(root& ChamberIndicator, List<int>& output);
  int ChamberIndicatorToIndex(root& ChamberIndicator);
  void RPolysToString(std::string& output);
  void ComputeKLPolys(WeylGroup* theWeylGroup, int TopChamberIndex);
  void ComputeRPolys();
  int ComputeProductfromSimpleReflectionsActionList(int x, int y);
  void WriteKLCoeffsToFile(std::fstream& output, List<int>& KLcoeff, int TopIndex);
  //returns the TopIndex of the KL coefficients
  int ReadKLCoeffsFromFile(std::fstream& input, List<int>& output);
  VermaModulesWithMultiplicities(){this->TheWeylGroup=0; }
  void GeneratePartialBruhatOrder();
  void ExtendOrder();
  void ComputeFullBruhatOrder();
  void initFromWeyl(WeylGroup* theWeylGroup);
};

class WeylGroup: public HashedList<ElementWeylGroup>
{
  Matrix<int> CartanSymmetricIntBuffer;
  void UpdateIntBuffer()
  { this->CartanSymmetricIntBuffer.init(this->CartanSymmetric.NumRows, this->CartanSymmetric.NumCols);
    for (int i=0; i<this->CartanSymmetric.NumRows; i++)
      for (int j=0; j<this->CartanSymmetric.NumCols; j++)
        this->CartanSymmetricIntBuffer.elements[i][j]=this->CartanSymmetric.elements[i][j].NumShort;
  }
  MatrixLargeRational FundamentalToSimpleCoords;
  MatrixLargeRational SimpleToFundamentalCoords;
  bool flagFundamentalToSimpleMatricesAreComputed;
  inline void ComputeFundamentalToSimpleMatrices()
  { if (flagFundamentalToSimpleMatricesAreComputed)
      return;
    roots fundamentalBasis;
    this->GetFundamentalWeightsInSimpleCoordinates(fundamentalBasis);
    this->FundamentalToSimpleCoords.AssignRootsToRowsOfMatrix(fundamentalBasis);
    this->FundamentalToSimpleCoords.Transpose();
    this->SimpleToFundamentalCoords=this->FundamentalToSimpleCoords;
    this->SimpleToFundamentalCoords.Invert();
    this->flagFundamentalToSimpleMatricesAreComputed=true;
  }
public:
  std::string DebugString;
  MatrixLargeRational CartanSymmetric;
  root rho;
  char WeylLetter;
  Rational LongRootLength;
  //Rational LongLongScalarProdPositive;
  //Rational ShortShortScalarProdPositive;
  //Rational ShortLongScalarProdPositive;
  //Rational ShortRootLength;
  HashedList<root> RootSystem;
  roots RootsOfBorel;
  static bool flagAnErrorHasOcurredTimeToPanic;
//  void MakeFromParSel(root& parSel, WeylGroup& input);
  void Assign(const WeylGroup& right);
  void ComputeRho(bool Recompute);
  void ComputeDebugString();
  void ElementToString(std::string& output);
  std::string ElementToString(){ std::string tempS; this->ElementToString(tempS); return tempS;}
  void MakeArbitrary(char WeylGroupLetter, int n);
  void GenerateAdditivelyClosedSubset(roots& input, roots& output);
  Rational GetKillingDivTraceRatio();
  Rational EstimateNumDominantWeightsBelow(root& inputHWsimpleCoords, GlobalVariables& theGlobalVariables);
  void MakeAn(int n);
  void MakeEn(int n);
  void MakeBn(int n);
  void MakeCn(int n);
  void MakeDn(int n);
  void MakeF4();
  void MakeG2();
  WeylGroup(){this->flagFundamentalToSimpleMatricesAreComputed=false;}
  MatrixLargeRational* GetMatrixFundamentalToSimpleCoords()
  { this->ComputeFundamentalToSimpleMatrices();
    return &this->FundamentalToSimpleCoords;
  }
  MatrixLargeRational* GetMatrixSimpleToFundamentalCoords()
  { this->ComputeFundamentalToSimpleMatrices();
    return &this->SimpleToFundamentalCoords;
  }
  root GetSimpleCoordinatesFromFundamental
  (const Vector<Rational>& inputInFundamentalCoords)
  ;
  Vector<Rational> GetFundamentalCoordinatesFromSimple
  (const Vector<Rational>& inputInSimpleCoords)
  ;
  root GetDualCoordinatesFromFundamental
  (const root& inputInFundamentalCoords)
  { root result=inputInFundamentalCoords;
    assert(result.size==this->GetDim());
    for (int i=0; i<result.size; i++)
      result[i]*=this->CartanSymmetric.elements[i][i]/2;
    return result;
  }
  inline Rational WeylDimFormula(root& theWeightInFundamentalCoords, GlobalVariables& theGlobalVariables)
  { return this->WeylDimFormula(theWeightInFundamentalCoords);
  }
  Rational GetScalarProdSimpleRoot(root& input, int indexSimpleRoot)
  { Rational result, tempRat;
    result.MakeZero();
    Rational* currentRow;
    currentRow=this->CartanSymmetric.elements[indexSimpleRoot];
    for (int i=0; i<input.size; i++)
    { tempRat=input.TheObjects[i];
      tempRat*=currentRow[i];
      result+=tempRat;
    }
    return result;
  }
  Rational WeylDimFormula(Vector<Rational>& theWeightInFundamentalCoords);
  Rational WeylDimFormulaFromSimpleCoords(Vector<Rational>& theWeightInSimpleCoords);
  void RaiseToDominantWeight
  (root& theWeight, int* sign=0, bool* stabilizerFound=0)
  ;
  void GetCoxeterPlane
  (Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables)
;
  void DrawRootSystem
(DrawingVariables& outputDV, bool wipeCanvas, GlobalVariables& theGlobalVariables,
 bool drawWeylChamber, root* bluePoint=0, bool LabelDynkinDiagramVertices=false,
 roots* predefinedProjectionPlane=0)
  ;
  void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities);
  void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks){ this->MakeFromDynkinType(theLetters, theRanks, 0); }
  //void GetLongRootLength(Rational& output);
  static bool IsAddmisibleDynkinType(char candidateLetter, int n);
  //the below will not do anything if the inputLetter is not a valid Dynkin letter
  static void TransformToAdmissibleDynkinType(char inputLetter, int& outputRank);
  void GetEpsilonCoords(char WeylLetter, int WeylRank, roots& simpleBasis, root& input, root& output, GlobalVariables& theGlobalVariables);
  void GetEpsilonCoords(const root& input, root& output, GlobalVariables& theGlobalVariables);
  void GetEpsilonCoords(List<root>& input, roots& output, GlobalVariables& theGlobalVariables);
  root GetEpsilonCoords(const root& input, GlobalVariables& theGlobalVariables){root tempRoot; this->GetEpsilonCoords(input, tempRoot, theGlobalVariables); return tempRoot;}
  void GetEpsilonCoordsWRTsubalgebra(roots& generators, List<root>& input, roots& output, GlobalVariables& theGlobalVariables);
  void GetEpsilonMatrix(char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables, MatrixLargeRational& output);
  void ComputeWeylGroup();
  bool LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup& left, ElementWeylGroup& right);
  void GetMatrixReflection(root& reflectionRoot, MatrixLargeRational& output);
  bool GetAlLDominantWeightsHWFDIM
(root& highestWeightSimpleCoords, HashedList<root>& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
  ;
  bool FreudenthalEval
  (Vector<Rational>& inputHWfundamentalCoords, HashedList<root>& outputDominantWeightsSimpleCoords,
   List<Rational>& outputMultsSimpleCoords, std::string& outputDetails,
   GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
  ;
  void GetWeylChamber
  (Cone& output, GlobalVariables& theGlobalVariables)
  ;
  std::string GenerateWeightSupportMethoD1
  (root& highestWeightSimpleCoords, roots& outputWeights, int upperBoundWeights, GlobalVariables& theGlobalVariables)
;
  void GetIntegralLatticeInSimpleCoordinates(Lattice& output);
  void GetFundamentalWeightsInSimpleCoordinates(roots& output);
  inline int GetDim()const{return this->CartanSymmetric.NumRows;}
  void ComputeWeylGroup(int UpperLimitNumElements);
  void ComputeWeylGroupAndRootsOfBorel(roots& output);
  void ComputeRootsOfBorel(roots& output);
  static Rational GetSizeWeylByFormula(char weylLetter, int theDim);
  bool IsARoot(const root& input){ return this->RootSystem.ContainsObjectHash(input); }
  void GenerateRootSubsystem(roots& theRoots);
  void GenerateOrbitAlg(root& ChamberIndicator, PolynomialsRationalCoeff& input, PolynomialsRationalCoeffCollection& output, bool RhoAction, bool PositiveWeightsOnly, ConeGlobal* LimitingCone, bool onlyLowerWeights);
  bool GenerateOrbit(roots& theRoots, bool RhoAction, HashedList<root>& output, bool UseMinusRho, int UpperLimitNumElements=0);
  bool GenerateOrbit(roots& theRoots, bool RhoAction, HashedList<root>& output, bool ComputingAnOrbitGeneratingSubsetOfTheGroup, WeylGroup& outputSubset, bool UseMinusRho, int UpperLimitNumElements);
  void GenerateRootSystemFromKillingFormMatrix();
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  void ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane& output, bool RhoAction, bool UseMinusRho);
  void ProjectOnTwoPlane(root& orthonormalBasisVector1, root& orthonormalBasisVector2, GlobalVariables& theGlobalVariables);
  void GetLowestElementInOrbit
  (root & inputOutput, ElementWeylGroup* outputWeylElt,
   roots& bufferEiBAsis,
   bool RhoAction,
   bool UseMinusRho, int* sign=0, bool* stabilizerFound=0)
  { this->GetExtremeElementInOrbit
    (inputOutput, outputWeylElt, bufferEiBAsis, true, RhoAction, UseMinusRho, sign, stabilizerFound);
  }
  void GetHighestElementInOrbit
  (root & inputOutput, ElementWeylGroup* outputWeylElt,
   roots& bufferEiBAsis,
   bool RhoAction, bool UseMinusRho,
   int* sign, bool* stabilizerFound)
  { this->GetExtremeElementInOrbit
    (inputOutput, outputWeylElt, bufferEiBAsis, false, RhoAction, UseMinusRho, sign, stabilizerFound);
  }
  void GetExtremeElementInOrbit
  (root& inputOutput, ElementWeylGroup* outputWeylElt,
   roots& bufferEiBAsis,
   bool findLowest, bool RhoAction, bool UseMinusRho, int* sign,
   bool* stabilizerFound)
     ;
  void GetLongestWeylElt(ElementWeylGroup& outputWeylElt) ;
  bool IsEigenSpaceGeneratorCoxeterElement(root& input);
  void GetCoxeterElement(ElementWeylGroup& outputWeylElt)
  { outputWeylElt.SetSize(this->GetDim());
    for (int i=0; i<outputWeylElt.size; i++)
      outputWeylElt[i]=i;
  }
  template <class Element>
  void ActOn(ElementWeylGroup& theGroupElement, Vector<Element>& theVector, bool RhoAction, bool UseMinusRho, const Element& theRingZero)
  { for (int i=0; i<theGroupElement.size; i++)
      this->SimpleReflection(theGroupElement[i], theVector, RhoAction, UseMinusRho, theRingZero);
  }
  template <class Element>
  void ActOn(int indexOfWeylElement, Vector<Element>& theVector, bool RhoAction, bool UseMinusRho, const Element& theRingZero=0)
  { this->ActOn(this->TheObjects[indexOfWeylElement], theVector, RhoAction, UseMinusRho, theRingZero);
  }
  template <class Element>
  void ActOnDual(int index,Vector<Element>& theVector, bool RhoAction, const Element& theRingZero);
  //theRoot is a list of the simple coordinates of the root
  //theRoot serves as both input and output
  void ActOnRootAlgByGroupElement(int index, PolynomialsRationalCoeff& theRoot, bool RhoAction);
  void ActOnRootByGroupElement(int index, root& theRoot, bool RhoAction, bool UseMinusRho);
  void ActOnDualSpaceElementByGroupElement(int index, root& theDualSpaceElement, bool RhoAction);
  void SimpleReflectionRoot(int index, root& theRoot, bool RhoAction, bool UseMinusRho);
  template <class Element>
  void SimpleReflection
  (int index, Vector<Element>& theVector, bool RhoAction, bool UseMinusRho, const Element& theRingZero=0)
  ;
  void GetMatrixOfElement(int theIndex, MatrixLargeRational& outputMatrix);
  void GetMatrixOfElement(ElementWeylGroup& input, MatrixLargeRational& outputMatrix);
  void SimpleReflectionDualSpace(int index, root& DualSpaceElement);
  void SimpleReflectionRootAlg(int index, PolynomialsRationalCoeff& theRoot, bool RhoAction);
  bool IsPositiveOrPerpWRTh(const root& input, const root& theH){ return this->RootScalarCartanRoot(input, theH).IsNonNegative(); }
  void ReflectBetaWRTAlpha(root& alpha, root& Beta, bool RhoAction, root& Output);
  bool IsRegular(root& input, int* indexFirstPerpendicularRoot);
  void RootScalarCartanRoot(const root& r1, const root& r2, Rational& output)const;
  double RootScalarCartanRoot(const Vector<double>& r1, const Vector<double>& r2)const
  { assert(r1.size==r2.size && r1.size==this->GetDim());
    double result=0;
    for (int i=0; i<this->GetDim(); i++)
      for (int j=0; j<this->GetDim(); j++)
        result+=this->CartanSymmetric.elements[i][j].DoubleValue()*r1.TheObjects[i]*r2.TheObjects[j];
    return result;
  }
  Rational RootScalarCartanRoot(const root& r1, const root& r2)const{ Rational tempRat; this->RootScalarCartanRoot(r1, r2, tempRat); return tempRat; }
  //the below functions perturbs input so that inputH has non-zero scalar product with roots of the root system,
  //without changing the inputH-sign of any root that had a non-zero scalar product to begin with
  void PerturbWeightToRegularWRTrootSystem(const root& inputH, root& output);
  bool IsDominantWeight(root& theWeight);
  void TransformToSimpleBasisGenerators(roots& theGens);
  void TransformToSimpleBasisGeneratorsWRTh(roots& theGens, root& theH);
  int length(int index);
  void operator=(const WeylGroup& other){this->Assign(other);}
};

template <class Element>
void WeylGroup::SimpleReflection
  (int index, Vector<Element>& theVector, bool RhoAction, bool UseMinusRho, const Element& theRingZero)
{ Element alphaShift, tempRat;
  alphaShift=theRingZero;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat.Assign(theVector.TheObjects[i]);
    tempRat*=(this->CartanSymmetric.elements[index][i]*(-2));
    alphaShift+=(tempRat);
  }
  if (this->flagAnErrorHasOcurredTimeToPanic)
  { std::string tempS;
    tempS=alphaShift.ElementToString();
  }
  alphaShift/=(this->CartanSymmetric.elements[index][index]);
  if (RhoAction)
  { if(UseMinusRho)
      alphaShift+=(1);
    else
      alphaShift+=(-1);
  }
  theVector.TheObjects[index]+=(alphaShift);
}

class ReflectionSubgroupWeylGroup: public HashedList<ElementWeylGroup>
{
public:
  bool truncated;
  WeylGroup AmbientWeyl;
  WeylGroup Elements;
  List<ElementWeylGroup> RepresentativesQuotientAmbientOrder;
  roots simpleGenerators;
  //format: each element of of the group is a list of generators, reflections with respect to the simple generators, and outer
  //automorphisms.
  //format of the externalAutomorphisms:
  // For a fixed external automorphism (of type roots) the i^th entry gives the image of the simple root with 1 on the i^th coordinate
  rootsCollection ExternalAutomorphisms;
  HashedList<root> RootSubsystem;
  roots RootsOfBorel;
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(this->DebugString, true); }
  void ElementToString(std::string& output, bool displayElements);
  std::string ElementToStringBruhatGraph();
  std::string ElementToStringCosetGraph();
  std::string ElementToStringFromLayersAndArrows
  (List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices)
  ;
  std::string ElementToString(bool displayElements=true){std::string tempS; this->ElementToString(tempS, displayElements); return tempS;}
  root GetRho();
  void RaiseToDominantWeight
  (root& theWeight, int* sign=0, bool* stabilizerFound=0)
  ;
  bool FreudenthalEvalIrrepIsWRTLeviPart
(Vector<Rational>& inputHWfundamentalCoords, HashedList<root>& outputDominantWeightsSimpleCoords,
 List<Rational>& outputMultsSimpleCoords, std::string& outputDetails,
 GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
  ;
  void MakeParabolicFromSelectionSimpleRoots
(WeylGroup& inputWeyl, Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements)
  ;
  void MakeParabolicFromSelectionSimpleRoots
(WeylGroup& inputWeyl, root& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements)
  { Selection tempSel;
    tempSel=ZeroesMeanSimpleRootSpaceIsInParabolic;
    this->MakeParabolicFromSelectionSimpleRoots(inputWeyl, tempSel, theGlobalVariables, UpperLimitNumElements);
  }
  bool GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra
  (root& highestWeightSimpleCoords, HashedList<root>& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
 ;
  bool GetAlLDominantWeightsHWFDIM
  (root& highestWeightSimpleCoords, HashedList<root>& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
 ;
  bool DrawContour
(const root& highestWeightSimpleCoord, DrawingVariables& theDV, GlobalVariables& theGlobalVariables, int theColor,
 int UpperBoundVertices)
    ;
  bool IsDominantWeight(root& theWeight);
  void FindQuotientRepresentatives(int UpperLimit);
  void GetMatrixOfElement(ElementWeylGroup& input, MatrixLargeRational& outputMatrix);
  bool GenerateOrbitReturnFalseIfTruncated(const root& input, roots& outputOrbit, int UpperLimitNumElements);
  void ComputeSubGroupFromGeneratingReflections(roots& inputGenerators, rootsCollection& inputExternalAutos, GlobalVariables& theGlobalVariables, int UpperLimitNumElements, bool recomputeAmbientRho);
  void ComputeRootSubsystem();
  void ActByElement(int index, root& theRoot);
  void ActByElement(int index, root& input, root& output){this->ActByElement(this->TheObjects[index], input, output);}
  void ActByElement(ElementWeylGroup& theElement, root& input, root& output);
  void ActByElement(int index, roots& input, roots& output) {this->ActByElement(this->TheObjects[index], input, output);}
  void ActByElement(ElementWeylGroup& theElement, roots& input, roots& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void Assign(const ReflectionSubgroupWeylGroup& other);
  void operator=(const ReflectionSubgroupWeylGroup& other){ this->Assign(other); }
};

class LaTeXProcedures
{
public:
  static const int ScaleFactor=40;
  static const int FigureSizeX= 10; //in centimeters
  static const int FigureSizeY=10; //in centimeters
  static const int FigureCenterCoordSystemX= 4; //in centimeters
  static const int FigureCenterCoordSystemY=8; //in centimeters
  static const int TextPrintCenteringAdjustmentX=3;
  static const int TextPrintCenteringAdjustmentY=3;
  static void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex, std::fstream& output, DrawingVariables& drawInput);
  static void drawTextDirectly(double X1, double Y1, const std::string& theText, int ColorIndex, std::fstream& output);
  static void beginDocument(std::fstream& output);
  static void endLatexDocument(std::fstream& output);
  static void beginPSTricks(std::fstream& output);
  static void endPSTricks(std::fstream& output);
  static void GetStringFromColorIndex(int ColorIndex, std::string &output, DrawingVariables& drawInput);
};

class thePFcomputation
{
public:
  int LCMdeterminants;
  int TotalNumEnumerated;
  List<Selection> tableForbidden;
  List<Selection> theForbiddenSelections;
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
//  bool CheckAvailabilityIndex(int depth, int index);
  WeylGroup theWeylGroup;
  MatrixLargeRational theKillingForm;
  void ComputeNewRestrictionsFromOld(int depth, int newIndex);
  void Run();
};

class multTableKmods : public List<List <List<int> > >
{
public:
  std::string DebugString;
  void ElementToString(std::string& output, rootSubalgebra& owner);
  void ElementToString(std::string& output, bool useLaTeX, bool useHtml, rootSubalgebra& owner);
  void ComputeDebugString(rootSubalgebra& owner){ this->ElementToString(this->DebugString, owner); }
};

class DynkinDiagramRootSubalgebra
{
public:
  std::string DynkinStrinG;
  void ElementToStrinG
  (std::string& output, bool useDollarSigns, bool IncludeAlgebraNames)
  ;
  std::string ElementToStrinG
  (bool useDollarSigns, bool IncludeAlgebraNames)
  { std::string result;
    this->ElementToStrinG(result, useDollarSigns, IncludeAlgebraNames);
    return result;
  }
  void ComputeDynkinStrinG()
  { this->ElementToStrinG(this->DynkinStrinG, false, false);
  }
  std::string SetComponent
  (const std::string& WeylLetterWithLength, int WeylRank, int componentIndex)
  ;
  static std::string GetNameFrom
  (const std::string& WeylLetterWithLength, int WeylRank, bool IncludeAlgebraNames)
  ;
  static std::string GetDiagramAndAlgebraName
  (const std::string& WeylLetterWithLength, int WeylRank)
  {return DynkinDiagramRootSubalgebra::GetNameFrom(WeylLetterWithLength, WeylRank, true);}
  rootsCollection SimpleBasesConnectedComponents;
  //to each connected component of the simple bases corresponds
  //its dynkin string with the same index
  List<std::string> DynkinTypeStrings;
  List<int> ComponentRanks;
  List<std::string> ComponentLetters;
  List<int> indicesThreeNodes;
  List<List<int> > indicesEnds;
  List<List<int> > sameTypeComponents;
  List<int> indexUniComponent;
  List<int> indexInUniComponent;
  int RankTotal();
  int NumRootsGeneratedByDiagram();
  void Sort();
  void SwapDynkinStrings(int i, int j);
  bool LetterIsDynkinGreaterThanLetter(char letter1, char letter2);
  //the below function takes as an input a set of roots and computes the corredponding Dynkin diagram of the
  //root subsystem. Note: the simleBasisInput is required to be a set of simple roots. The procedure calls a
  //transformation to simple basis on the simpleBasisInput, so your input will get changed if it wasn't
  //simple as required!
  inline void ComputeDiagramTypeModifyInput(roots& simpleBasisInput, WeylGroup& theWeyl)
  { theWeyl.TransformToSimpleBasisGenerators(simpleBasisInput);
    this->ComputeDiagramTypeKeepInput(simpleBasisInput, theWeyl);
  }
  //the below function is just as the above but doesn't modify simpleBasisInput
  void ComputeDiagramTypeKeepInput
  (const roots& simpleBasisInput, WeylGroup& theWeyl)
  ;
  void ComputeDynkinStrings
  (WeylGroup& theWeyl)
  ;
  void ComputeDynkinString
(int indexComponent, WeylGroup& theWeyl)
  ;
  void GetKillingFormMatrixUseBourbakiOrder(MatrixLargeRational& output, WeylGroup& theWeyl);
  int numberOfThreeValencyNodes(int indexComponent, WeylGroup& theWeyl);
  void Assign(const DynkinDiagramRootSubalgebra& right);
  inline void operator=(const DynkinDiagramRootSubalgebra& right){this->Assign(right); }
  bool operator==(const DynkinDiagramRootSubalgebra& right) const;
  bool IsGreaterThan(DynkinDiagramRootSubalgebra& right);
  Rational GetSizeCorrespondingWeylGroupByFormula();
  void GetSimpleBasisInBourbakiOrder(roots& output);
  void GetSimpleBasisInBourbakiOrderOneComponentAppend(roots& outputAppend, int index);
  void GetAutomorphism(List<List<int> >& output, int index);
  void GetAutomorphisms(List<List<List<int> > >& output);
  void GetMapFromPermutation(roots& domain, roots& range, List<int>& thePerm, List<List<List<int> > >& theAutos, SelectionWithDifferentMaxMultiplicities& theAutosPerm, DynkinDiagramRootSubalgebra& right);
};

class coneRelation
{
public:
  roots Alphas;
  roots Betas;
  List<Rational> AlphaCoeffs;
  List<Rational> BetaCoeffs;
  List<List<int> > AlphaKComponents;
  List<List<int> > BetaKComponents;
  int IndexOwnerRootSubalgebra;
  bool GenerateAutomorphisms(coneRelation& right, rootSubalgebras& owners);
  DynkinDiagramRootSubalgebra theDiagram;
  DynkinDiagramRootSubalgebra theDiagramRelAndK;
  std::string DebugString;
  std::string stringConnectedComponents;
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ComputeTheDiagramAndDiagramRelAndK(rootSubalgebra& owner);
  void ComputeDiagramRelAndK(rootSubalgebra& owner);
  void FixRepeatingRoots(roots& theRoots, List<Rational>& coeffs);
  void RelationOneSideToString(std::string& output, const std::string& letterType, List<Rational>& coeffs, List<List<int> >& kComponents, roots& theRoots, bool useLatex, rootSubalgebra& owner);
  void GetEpsilonCoords(roots& input, roots& output, WeylGroup& theWeyl, GlobalVariables& theGlobalVariables);
  int ElementToString(std::string& output, rootSubalgebras& owners, bool useLatex, bool includeScalarsProductsEachSide, bool includeMixedScalarProducts);
  int RootsToScalarProductString(roots& inputLeft, roots& inputRight, const std::string& letterTypeLeft, const std::string& letterTypeRight, std::string& output, bool useLatex, rootSubalgebra& owner);
  void ComputeConnectedComponents(roots& input, rootSubalgebra& owner, List<List<int> >& output);
  void ComputeDebugString(rootSubalgebras& owner, bool includeScalarsProducts, bool includeMixedScalarProducts){ this->ElementToString(this->DebugString, owner, true, includeScalarsProducts, includeMixedScalarProducts);  }
  void MakeLookCivilized(rootSubalgebra& owner, roots& NilradicalRoots);
  bool IsStrictlyWeaklyProhibiting(rootSubalgebra& owner, roots& NilradicalRoots, GlobalVariables& theGlobalVariables, rootSubalgebras& owners, int indexInOwner);
  void FixRightHandSide(rootSubalgebra& owner, roots& NilradicalRoots);
  bool leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right);
  void ComputeKComponents(roots& input, List<List<int> >& output, rootSubalgebra& owner);
  void RelationOneSideToStringCoordForm(std::string& output,  List<Rational>& coeffs, roots& theRoots, bool EpsilonForm);
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
  }
  bool operator==(const coneRelation& right){ return this->DebugString==right.DebugString; }
  int HashFunction() const
  { int tempI= ::MathRoutines::Minimum((int)this->DebugString.length(), ::SomeRandomPrimesSize);
    int result=0;
    for (int i=0; i<tempI; i++)
      result+= this->DebugString[i]*::SomeRandomPrimes[i];
    return result;
  }
  coneRelation(){this->IndexOwnerRootSubalgebra=-1; }
};

class coneRelations: public HashedList<coneRelation>
{
public:
  int NumAllowedLatexLines;
  bool flagIncludeSmallerRelations;
  bool flagIncludeCoordinateRepresentation;
  bool flagIncludeSubalgebraDataInDebugString;
  std::string DebugString;
  List<std::string> CoordinateReps;
  void GetLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter);
  void ElementToString
(std::string& output, rootSubalgebras& owners, bool useLatex, bool useHtml, std::string* htmlPathPhysical,
 std::string* htmlPathServer, GlobalVariables& theGlobalVariables, const std::string& DisplayNameCalculator)
  ;
  void ComputeDebugString(rootSubalgebras& owners, std::string* htmlPathPhysical, std::string* htmlPathServer, GlobalVariables& theGlobalVariables)
  { this->ElementToString (this->DebugString, owners, true, false, htmlPathPhysical, htmlPathServer, theGlobalVariables, "");
  }
  void ComputeDebugString(rootSubalgebras& owners, GlobalVariables& theGlobalVariables){ this->ComputeDebugString(owners, 0, 0, theGlobalVariables); };
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner);
  void AddRelationNoRepetition(coneRelation& input, rootSubalgebras& owners, int indexInRootSubalgebras);
  coneRelations()
  { this->NumAllowedLatexLines=40;
    this->flagIncludeSmallerRelations=true;
    this->flagIncludeCoordinateRepresentation=false;
    this->flagIncludeSubalgebraDataInDebugString=false;
  }
};

class permutation: public SelectionWithDifferentMaxMultiplicities
{
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int TotalNumElements);
  void incrementAndGetPermutation(List<int>& output);
  void GetPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int GetNumPermutations() {return this->getTotalNumSubsets();}
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
  List<int> indicesSubalgebrasContainingK;
  multTableKmods theMultTable;
  List<int> theOppositeKmods;
  DynkinDiagramRootSubalgebra theDynkinDiagram;
  DynkinDiagramRootSubalgebra theCentralizerDiagram;
  List<List<int> > coneRelationsBuffer;
  List<int> coneRelationsNumSameTypeComponentsTaken;
  List<DynkinDiagramRootSubalgebra> relationsDiagrams;
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
  roots SimpleBasisKEpsCoords;
  roots SimpleBasisgEpsCoords;
  rootsCollection kModulesKepsCoords;
  rootsCollection kModulesgEpsCoords;
  List<roots> kModules;
  List<roots> PosRootsKConnectedComponents;
  List<Selection> theKEnumerations;
  List<int> theKComponentRanks;
  std::string DebugString;
  rootSubalgebra();
  //returns -1 if the weight/root is not in g/k
  int GetIndexKmoduleContainingRoot(root& input);
  void GetCoxeterPlane
  (Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables)
  ;
  void GetCoxeterElement(MatrixLargeRational& output);
  bool IsGeneratingSingularVectors(int indexKmod, roots& NilradicalRoots);
  bool rootIsInCentralizer(root& input);
  bool IsBKhighest(root& input);
  bool rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, root& input);
  void ComputeEpsCoordsWRTk(GlobalVariables& theGlobalVariables);
  bool AttemptTheTripleTrick(coneRelation& theRel, roots& NilradicalRoots, GlobalVariables& theGlobalVariables);
  bool AttemptTheTripleTrickWRTSubalgebra(coneRelation& theRel, roots& highestWeightsAllowed, roots& NilradicalRoots, GlobalVariables& theGlobalVariables);
  void ExtractRelations(MatrixLargeRational& matA, MatrixLargeRational& matX, roots& NilradicalRoots, rootSubalgebras& owner, int indexInOwner, GlobalVariables& theGlobalVariables, roots& Ksingular);
  bool GenerateIsomorphismsPreservingBorel(rootSubalgebra& right, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly);
  void GenerateAutomorphismsPreservingBorel(GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup& outputAutomorphisms);
  void MakeGeneratingSingularVectors(coneRelation& theRelation, roots& nilradicalRoots);
  bool attemptExtensionToIsomorphismNoCentralizer(roots& Domain, roots& Range, GlobalVariables& theGlobalVariables, int RecursionDepth, ReflectionSubgroupWeylGroup* outputAutomorphisms, bool GenerateAllpossibleExtensions, bool* abortKmodule, roots* additionalDomain, roots* additionalRange);
  static bool attemptExtensionToIsomorphism(roots& Domain, roots& Range, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly, WeylGroup& theWeyl, bool *DomainAndRangeGenerateNonIsoSAs);
  bool CheckForSmallRelations(coneRelation& theRel, roots& nilradicalRoots);
  int NumRootsInNilradical();
  void MakeSureAlphasDontSumToRoot(coneRelation& theRel, roots& NilradicalRoots);
  bool IsARoot(const root& input);
  bool IsARootOrZero(root& input);
  void KEnumerationsToLinComb(GlobalVariables& theGlobalVariables);
  void DoKRootsEnumeration(GlobalVariables& theGlobalVariables);
  void ComputeCentralizerFromKModulesAndSortKModules();
  void MatrixToRelation(coneRelation& output, MatrixLargeRational& matA, MatrixLargeRational& matX, int theDimension, roots& NilradicalRoots);
  void DoKRootsEnumerationRecursively(int indexEnumeration, GlobalVariables& theGlobalVariables);
  void MakeProgressReportPossibleNilradicalComputation(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner);
  void MakeProgressReportGenAutos(int progress, int outOf, int found, GlobalVariables& theGlobalVariables);
  void ComputeDebugString(GlobalVariables& theGlobalVariables);
  void ComputeDebugString(bool useLatex, bool useHtml, bool includeKEpsCoords, GlobalVariables& theGlobalVariables){ this->ElementToString(this->DebugString, useLatex, useHtml, includeKEpsCoords, theGlobalVariables); };
  bool IndexIsCompatibleWithPrevious(int startIndex, int RecursionDepth,  multTableKmods& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner, GlobalVariables& theGlobalVariables);
  bool IsAnIsomorphism(roots& domain, roots& range, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms, roots* additionalDomain, roots* additionalRange);
  bool ListHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel);
  void GeneratePossibleNilradicalsRecursive(Controller& PauseMutex, GlobalVariables& theGlobalVariables, multTableKmods& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner, int indexInOwner);
  void GeneratePossibleNilradicals(Controller& PauseMutex, List<Selection>& impliedSelections, int& parabolicsCounter, GlobalVariables& theGlobalVariables, bool useParabolicsInNilradical, rootSubalgebras& owner, int indexInOwner);
  void GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter);
  void WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables* theGlobalVariables, rootSubalgebras& owner);
  void ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner);
  bool ConeConditionHolds(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner, bool doExtractRelations);
  bool ConeConditionHolds(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner, roots& NilradicalRoots, roots& Ksingular, bool doExtractRelations);
  void PossibleNilradicalComputation(GlobalVariables& theGlobalVariables, Selection& selKmods, rootSubalgebras& owner, int indexInOwner);
  void ElementToStringHeaderFooter(std::string& outputHeader, std::string& outputFooter, bool useLatex, bool useHtml, bool includeKEpsCoords);
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables){this->ElementToString(output, false, false, false, theGlobalVariables); };
  void ElementToHtml(int index, std::string& path, SltwoSubalgebras* sl2s, GlobalVariables& theGlobalVariables);
  void ElementToString(std::string& output, bool useLatex, bool useHtml, bool includeKEpsCoords, GlobalVariables& theGlobalVariables){this->ElementToString(output, 0, 0, useLatex, useHtml, includeKEpsCoords, theGlobalVariables); };
  void ElementToString(std::string& output, SltwoSubalgebras* sl2s, int indexInOwner, bool useLatex, bool useHtml, bool includeKEpsCoords, GlobalVariables& theGlobalVariables);
  bool RootsDefineASubalgebra(roots& theRoots);
  void GenerateKmodMultTable(List<List<List<int> > >& output, List<int>& oppositeKmods, GlobalVariables& theGlobalVariables);
  void MakeProgressReportMultTable(int index, int outOf, GlobalVariables& theGlobalVariables);
  void KmodTimesKmod(int index1, int index2, List<int>& oppositeKmods, List<int>& output);
  void initFromAmbientWeyl();
  void GetSsl2SubalgebrasAppendListNoRepetition(SltwoSubalgebras& output, int indexInContainer, GlobalVariables& theGlobalVariables, SemisimpleLieAlgebra& theLieAlgebra);
  void ComputeAllButAmbientWeyl();
  void ComputeDynkinDiagramKandCentralizer();
  void ComputeAll();
  void ComputeRootsOfK();
  void ComputeKModules();
  void ComputeHighestWeightInTheSameKMod(root& input, root& outputHW);
  void ComputeExtremeWeightInTheSameKMod(root& input, root& outputW, bool lookingForHighest);
  inline void operator=(const rootSubalgebra& right){this->Assign(right); }
  void Assign(const rootSubalgebra& right);
  void ComputeLowestWeightInTheSameKMod(root& input, root& outputLW);
  void GetLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration, GlobalVariables& theGlobalVariables);
//  void commonCodeForGetLinearCombinationFromMaxRankRootsAndExtraRoot();
  void initForNilradicalGeneration();
  void GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2(GlobalVariables& theGlobalVariables);
  bool LinCombToString(root& alphaRoot, int coeff, root& linComb, std::string& output);
  bool LinCombToStringDistinguishedIndex(int distinguished, root& alphaRoot, int coeff, root& linComb, std::string& output);
  void WriteMultTableAndOppositeKmodsToFile(std::fstream& output, List<List<List<int> > >& inMultTable, List<int>& inOpposites);
  void ReadMultTableAndOppositeKmodsFromFile(std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites);
};

class rootSubalgebras: public List<rootSubalgebra>
{
public:
  std::string DebugString;
  coneRelations theBadRelations;
  coneRelations theGoodRelations;
  coneRelations theMinRels;
  List<List<int> > ActionsNormalizerCentralizerNilradical;
  List<ReflectionSubgroupWeylGroup> CentralizerOuterIsomorphisms;
  List<ReflectionSubgroupWeylGroup> CentralizerIsomorphisms;
  //Code used in nilradical generation:
  List<Selection> ImpiedSelectionsNilradical;
  List<List<List<int> > > storedNilradicals;
  int parabolicsCounterNilradicalGeneration;
  List<int> numNilradicalsBySA;
  int IndexCurrentSANilradicalsGeneration;
  int NumReductiveRootSAsToBeProcessedNilradicalsGeneration;
  List<int> CountersNilradicalsGeneration;
  List<int> NumConeConditionHoldsBySSpart;
  int RecursionDepthNilradicalsGeneration;
  Controller controllerLProhibitingRelations;
  int NumSubalgebrasProcessed;
  int NumConeConditionFailures;
  int NumSubalgebrasCounted;
  int NumLinesPerTableLatex;
  int NumColsPerTableLatex;
  int UpperLimitNumElementsWeyl;
  static int ProblemCounter;
  WeylGroup AmbientWeyl;
  std::string ReportStringNonNilradicalParabolic;
  bool flagComputingLprohibitingWeights;
  bool flagUseDynkinClassificationForIsomorphismComputation;
  bool flagUsingParabolicsInCentralizers;
  bool flagUsingActionsNormalizerCentralizerNilradical;
  bool flagNilradicalComputationInitialized;
  bool flagCountingNilradicalsOnlyNoComputation;
  bool flagComputeConeCondition;
  bool flagLookingForMinimalRels;
  bool flagStoringNilradicals;
  void ComputeKmodMultTables(GlobalVariables& theGlobalVariables);
  bool ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel, GlobalVariables& theGlobalVariables);
  bool ApproveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel, GlobalVariables& theGlobalVariables);
  void RaiseSelectionUntilApproval(Selection& targetSel, GlobalVariables& theGlobalVariables);
  void ApplyOneGenerator(List<int>& generator, Selection& targetSel, GlobalVariables& theGlobalVariables);
  void GenerateActionKintersectBIsos(rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables);
  void GenerateKintersectBOuterIsos(rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables);
  void ComputeActionNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables);
  void ComputeNormalizerOfCentralizerIntersectNilradical(ReflectionSubgroupWeylGroup& outputSubgroup, Selection& SelectedBasisRoots, rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables);
  void GenerateAllReductiveRootSubalgebrasUpToIsomorphism(GlobalVariables& theGlobalVariables, char WeylLetter, int WeylRank, bool sort, bool computeEpsCoords);
  void GenerateAllReductiveRootSubalgebrasUpToIsomorphism(GlobalVariables& theGlobalVariables, bool sort, bool computeEpsCoords);
  bool IsANewSubalgebra(rootSubalgebra& input, GlobalVariables& theGlobalVariables);
  int IndexSubalgebra(rootSubalgebra& input, GlobalVariables& theGlobalVariables);
  void GenerateAllReductiveRootSubalgebrasContainingInputUpToIsomorphism(rootSubalgebras& bufferSAs, int RecursionDepth, GlobalVariables& theGlobalVariables);
  void ElementToStringDynkinTable(bool useLatex, bool useHtml, std::string* htmlPathPhysical, std::string* htmlPathServer, std::string& output);
  void GetTableHeaderAndFooter(std::string& outputHeader, std::string& outputFooter, bool useLatex, bool useHtml);
  void SortDescendingOrderBySSRank();
  void pathToHtmlFileNameElements(int index, std::string* htmlPathServer, std::string& output, bool includeDotHtml);
  void pathToHtmlReference(int index, std::string& DisplayString, std::string* htmlPathServer, std::string& output);
  void WriteToDefaultFileNilradicalGeneration(GlobalVariables* theGlobalVariables);
  bool ReadFromDefaultFileNilradicalGeneration(GlobalVariables* theGlobalVariables);
  void WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables* theGlobalVariables);
  void ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, roots& input, GlobalVariables& theGlobalVariables);
  void ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm, GlobalVariables& theGlobalVariables);
  void ElementToHtml(std::string& header, std::string& pathPhysical, std::string& htmlPathServer, SltwoSubalgebras* Sl2s, GlobalVariables& theGlobalVariables);
  void ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess, GlobalVariables& theGlobalVariables);
  void ElementToString
(std::string& output, SltwoSubalgebras* sl2s, bool useLatex, bool useHtml, bool includeKEpsCoords,
 std::string* htmlPathPhysical, std::string* htmlPathServer, GlobalVariables& theGlobalVariables, const std::string& DisplayNameCalculator)
  ;
  void ComputeLProhibitingRelations(GlobalVariables& theGlobalVariables);
  void ComputeAllRootSubalgebrasUpToIso(GlobalVariables& theGlobalVariables, int StartingIndex, int NumToBeProcessed);
  void ComputeDebugString
  (bool useLatex, bool useHtml, bool includeKEpsCoords, std::string* htmlPathPhysical,
   std::string* htmlPathServer, GlobalVariables& theGlobalVariables)
  { this->ElementToString(this->DebugString, 0, useLatex, useHtml, includeKEpsCoords, htmlPathPhysical, htmlPathServer, theGlobalVariables, "");
  }
  void MakeProgressReportGenerationSubalgebras(rootSubalgebras& bufferSAs, int RecursionDepth, GlobalVariables& theGlobalVariables, int currentIndex, int TotalIndex);
  void MakeProgressReportAutomorphisms(ReflectionSubgroupWeylGroup& theSubgroup, rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables);
  void initForNilradicalGeneration()
  { this->NumSubalgebrasProcessed=0;
    this->NumConeConditionFailures=0;
    this->NumSubalgebrasCounted=0;
    this->IndexCurrentSANilradicalsGeneration=0;
    this->ReportStringNonNilradicalParabolic="";
    this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration=this->size-1;
    if (this->size>0)
    { this->TheObjects[0].GeneratePossibleNilradicalsInit(this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration);
      this->NumConeConditionHoldsBySSpart.initFillInObject(this->size, 0);
    }
  }
  rootSubalgebras()
  { this->flagNilradicalComputationInitialized=false;
    this->flagStoringNilradicals=false;
    this->flagUsingParabolicsInCentralizers=true;
    this->flagUseDynkinClassificationForIsomorphismComputation=true;
    this->flagCountingNilradicalsOnlyNoComputation = false;
    this->flagComputingLprohibitingWeights=false;
    this->flagComputeConeCondition=false;
    this->flagUsingActionsNormalizerCentralizerNilradical=true;
    this->flagLookingForMinimalRels=false;
    this->NumLinesPerTableLatex=20;
    this->NumColsPerTableLatex=4;
    this->UpperLimitNumElementsWeyl=0;
    this->initForNilradicalGeneration();
  }
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
  IndicatorWindowVariables(){this->Nullify(); }
  bool ProgressReportStringsNeedRefresh;
  bool StatusString1NeedsRefresh;
  List<std::string> ProgressReportStrings;
  std::string StatusString1;
  void Assign(IndicatorWindowVariables& right);
  void Nullify()
  { this->Busy=false;
    this->Pause=true;
    this->StatusString1NeedsRefresh=false;
    this->ProgressReportStringsNeedRefresh=false;
    this->ProgressReportStrings.SetSize(5);
    this->NumProcessedMonomialsCurrentFraction=0;
    this->NumProcessedMonomialsTotal=0;
    this->modifiedRoot.MakeZero(1);
  }
};

class ElementSimpleLieAlgebra
{
public:
  std::string DebugString;
  void ElementToString(std::string& output){this->ElementToString(output, false, false);}
  std::string ElementToString()const { std::string output; this->ElementToString(output, false, false); return output;}
  bool ElementToStringNeedsBracketsForMultiplication()const;
  void ElementToString(std::string& output, bool useHtml, bool useLatex, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)const;
  void ElementToString(std::string& output, bool useHtml, bool useLatex)const{ this->ElementToString(output, useHtml, useLatex, false, 0, 0);}
  std::string ElementToStringNegativeRootSpacesFirst
  (bool useRootNotation, bool useEpsilonNotation, SemisimpleLieAlgebra& owner, const PolynomialOutputFormat& thePolyFormat,
   GlobalVariables& theGlobalVariables)
;
  void ComputeDebugString(bool useHtml, bool useLatex){ this->ElementToString(this->DebugString, useHtml, useLatex, false, 0, 0);  }
  Selection NonZeroElements;
  // the index in i^th position in the below array gives the coefficient in front of the i^th root in the ambient root system, i.e. the root owner.theWeyl.RootSystem.TheObjects[i].
  List<Rational> coeffsRootSpaces;
  root Hcomponent;
  void AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(int theIndex, const SemisimpleLieAlgebra& owner);
  void ElementToVectorRootSpacesFirstThenCartan(root& output);
  void ElementToVectorNegativeRootSpacesFirst(Vector<Rational>& output)const;
  void AssignVectorRootSpacesFirstThenCartan(const root& input, SemisimpleLieAlgebra& owner);
  void AssignVectorNegRootSpacesCartanPosRootSpaces(const root& input, const SemisimpleLieAlgebra& owner);
  void AssignVectorNegRootSpacesCartanPosRootSpaces
  (const root& input, int numRoots, int theAlgebraRank)
  ;
  bool GetCoordsInBasis
  (const List<ElementSimpleLieAlgebra>& theBasis, Vector<Rational>& output, GlobalVariables& theGlobalVariables)const
;
  bool GetCoordsInBasis
  (const List<ElementSimpleLieAlgebra>& theBasis, Vector<RationalFunction>& output, GlobalVariables& theGlobalVariables)const
  { Vector<Rational> tempVect;
    if (! this->GetCoordsInBasis(theBasis, tempVect, theGlobalVariables))
      return false;
    output.SetSize(tempVect.size);
    for (int i=0; i<tempVect.size; i++)
      output.TheObjects[i].MakeNVarConst(this->Hcomponent.size, tempVect.TheObjects[i], &theGlobalVariables);
    return true;
  }
  void MultiplyByRational(SemisimpleLieAlgebra& owner, const Rational& theNumber);
  void ComputeNonZeroElements();
  static void GetBasisFromSpanOfElements
  (List<ElementSimpleLieAlgebra>& theElements, Vectors<RationalFunction>& outputCoords, List<ElementSimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables)
    ;
  void ActOnMe
  (const ElementSimpleLieAlgebra& theElt, ElementSimpleLieAlgebra& output, SemisimpleLieAlgebra& owner)
  ;
  void ActOnMe
    (const ElementSimpleLieAlgebra& theElt, ElementSimpleLieAlgebra& output, SemisimpleLieAlgebra& owner,
   const RationalFunction& theRingUnit, const RationalFunction& theRingZero, GlobalVariables* theGlobalVariables)
   ;
  bool IsACoeffOneChevalleyGenerator(int& outputGenerator, SemisimpleLieAlgebra& owner);
  bool IsProportionalTo(const ElementSimpleLieAlgebra& other)const
  { root tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.IsProportionalTo(tempRoot2);
  }
  bool IsProportionalTo(const ElementSimpleLieAlgebra& other, Rational& outputTimesMeEqualsInput)const
  { root tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.IsProportionalTo(tempRoot2, outputTimesMeEqualsInput);
  }
  bool MustUseBracketsWhenDisplayingMeRaisedToPower();
  void SetCoefficient(const root& indexingRoot, Rational& theCoeff, const SemisimpleLieAlgebra& owner);
  void SetCoefficient(const root& indexingRoot, int theCoeff, const  SemisimpleLieAlgebra& owner);
  //range is the image of the vectors e_i
  bool IsEqualToZero()const;
  ElementSimpleLieAlgebra(const ElementSimpleLieAlgebra& other){ this->operator=(other);}
  ElementSimpleLieAlgebra(){}
  void operator=(const ElementSimpleLieAlgebra& other)
  { this->coeffsRootSpaces.CopyFromBase(other.coeffsRootSpaces);
    this->Hcomponent.Assign(other.Hcomponent);
    this->NonZeroElements.Assign(other.NonZeroElements);
  }
  void init
  (const SemisimpleLieAlgebra& owner)
  ;
  void init
  (int numRoots, int theAlgebraRank)
  ;
  void Nullify(const SemisimpleLieAlgebra& owner);
  void Nullify
  (int numRoots, int theAlgebraRank)
;
  ElementSimpleLieAlgebra operator*(const Rational& other)
  { ElementSimpleLieAlgebra result;
    result=*this;
    result.TimesConstant(other);
    return result;
  }
  int HashFunction()
  { int numCycles=MathRoutines::Minimum(SomeRandomPrimesSize, this->NonZeroElements.CardinalitySelection);
    int result=0;
    for (int i=0; i< numCycles; i++)
      result+= SomeRandomPrimes[i]* this->coeffsRootSpaces[this->NonZeroElements.elements[i]].HashFunction()
      *(1+this->NonZeroElements.elements[i]);
    result+=this->Hcomponent.HashFunction();
    return result;
  }
  void TimesConstant(const Rational& input);
  bool operator==(const ElementSimpleLieAlgebra& other)const{ return this->coeffsRootSpaces.IsEqualTo(other.coeffsRootSpaces) && this->Hcomponent.IsEqualTo(other.Hcomponent);}
  void operator+=(const ElementSimpleLieAlgebra& other);
};

class slTwo
{
public:
  std::string DebugString;
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer, PolynomialOutputFormat& PolyFormatLocal);
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, bool useLatex, bool useHtml, PolynomialOutputFormat& PolyFormatLocal){ this->ElementToString(output, theGlobalVariables, container, 0, useLatex, useHtml, false, 0, 0, PolyFormatLocal);}
  void ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output);
  void ComputeDebugString(bool useHtml, bool useLatex, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container){ PolynomialOutputFormat PolyFormatLocal; this->ElementToString(this->DebugString, theGlobalVariables, container, useLatex, useHtml, PolyFormatLocal); }
  List<int> highestWeights;
  List<int> multiplicitiesHighestWeights;
  List<int> weightSpaceDimensions;
  ElementSimpleLieAlgebra theH, theE, theF;
  ElementSimpleLieAlgebra bufferHbracketE, bufferHbracketF, bufferEbracketF;
  SemisimpleLieAlgebra* owner;
  List<int> IndicesContainingRootSAs;
  List<int> IndicesMinimalContainingRootSA;
  List<List<int> > HighestWeightsDecompositionMinimalContainingRootSA;
  List<List<int> > MultiplicitiesDecompositionMinimalContainingRootSA;
  roots preferredAmbientSimpleBasis;
  root hCharacteristic;
  root hElementCorrespondingToCharacteristic;
  roots hCommutingRootSpaces;
  roots RootsHavingScalarProduct2WithH;
  DynkinDiagramRootSubalgebra DiagramM;
  DynkinDiagramRootSubalgebra CentralizerDiagram;
  PolynomialsRationalCoeff theSystemToBeSolved;
  MatrixLargeRational theSystemMatrixForm;
  MatrixLargeRational theSystemColumnVector;
  bool DifferenceTwoHsimpleRootsIsARoot;
  int DynkinsEpsilon;
  void ComputeModuleDecomposition(roots& positiveRootsContainingRegularSA, int dimensionContainingRegularSA, List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights, List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables);
  bool ModuleDecompositionFitsInto(const slTwo& other);
  static bool ModuleDecompositionFitsInto(const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft, const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight);
  void MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra);
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void ComputeDynkinsEpsilon(WeylGroup& theWeyl);
  void ElementToHtml(std::string& filePath);
  void ElementToHtmlCreateFormulaOutputReference(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer);
  void operator=(const slTwo& right)
  { this->highestWeights.CopyFromBase(right.highestWeights);
    this->multiplicitiesHighestWeights.CopyFromBase(right.multiplicitiesHighestWeights);
    this->weightSpaceDimensions.CopyFromBase(right.weightSpaceDimensions);
    this->HighestWeightsDecompositionMinimalContainingRootSA.CopyFromBase(right.HighestWeightsDecompositionMinimalContainingRootSA);
    this->MultiplicitiesDecompositionMinimalContainingRootSA.CopyFromBase(right.MultiplicitiesDecompositionMinimalContainingRootSA);
    this->hCommutingRootSpaces.CopyFromBase(right.hCommutingRootSpaces);
    this->CentralizerDiagram.Assign(right.CentralizerDiagram);
    this->DiagramM.Assign(right.DiagramM);
    this->hCommutingRootSpaces.CopyFromBase(right.hCommutingRootSpaces);
    this->DifferenceTwoHsimpleRootsIsARoot=right.DifferenceTwoHsimpleRootsIsARoot;
    this->RootsHavingScalarProduct2WithH=right.RootsHavingScalarProduct2WithH;
    this->DynkinsEpsilon=right.DynkinsEpsilon;
    this->hCharacteristic.Assign(right.hCharacteristic);
    this->hElementCorrespondingToCharacteristic.Assign(right.hElementCorrespondingToCharacteristic);
    this->owner = right.owner;
    this->theE= right.theE;
    this->theH= right.theH;
    this->theF= right.theF;
    this->bufferEbracketF= right.bufferEbracketF;
    this->bufferHbracketE=right.bufferHbracketE;
    this->bufferHbracketF=right.bufferHbracketF;
    this->theSystemToBeSolved=right.theSystemToBeSolved;
    this->theSystemMatrixForm=right.theSystemMatrixForm;
    this->theSystemColumnVector= right.theSystemColumnVector;
    this->IndicesContainingRootSAs.CopyFromBase(right.IndicesContainingRootSAs);
    this->preferredAmbientSimpleBasis= right.preferredAmbientSimpleBasis;
  }
  bool  operator==(const slTwo& right)
  {// See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, chapter 7-10
     return this->hCharacteristic.IsEqualTo(right.hCharacteristic);
  }
  int HashFunction() const
  { int tempI=MathRoutines::Minimum(SomeRandomPrimesSize, this->hCharacteristic.size);
    int result=0;
    for (int i=0; i<tempI; i++)
      result+= this->hCharacteristic.TheObjects[i].NumShort*SomeRandomPrimes[i];
    return result;
  }
};

class VectorPartition
{
public:
  roots PartitioningRoots;
  root theRoot;
  //format: each element of thePartitions gives an array whose entries give
  // the multiplicity of the weights. I.e. if PartitioningRoots has 2 elements, then thePartitions.TheObjects[0]
  // would have 2 elements: the first giving the multiplicity of PartitioningRoots.TheObjects[0] and the second - the multiplicity of
  // PartitioningRoots.TheObjects[1]
  List<List<int> > thePartitions;
  std::string DebugString;
  std::string ElementToString(bool useHtml);
  void ComputeDebugString(bool useHtml){this->DebugString=this->ElementToString(useHtml);}
  int ComputeVectorPartitionFunctionSmall(root& theRoot, roots& theRoots);
  void ComputeAllPartitions();
  void ComputeAllPartitionsRecursive(int currentIndex, List<int>& CurrentPartition, int UpperBoundEachIndex, root& toBePartitioned);
};

class SemisimpleLieAlgebra: public HashedList<ElementSimpleLieAlgebra>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output, bool useHtml, bool useLatex, GlobalVariables& theGlobalVariables){ this->ElementToString(output, useHtml, useLatex, false, theGlobalVariables, 0, 0, 0, 0); }
  void ElementToString(std::string& output, bool useHtml, bool useLatex, bool usePNG, GlobalVariables& theGlobalVariables, std::string* physicalPath, std::string* htmlServerPath, List<std::string>* outputPNGFileNames, List<std::string>* outputLatexToPNGstrings);
  void ElementToStringNegativeRootSpacesFirst
  (std::string& output, bool useRootNotation, bool useEpsilonNotation, bool useHtml, bool useLatex, bool usePNG, PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
  ;
  std::string ElementToStringNegativeRootSpacesFirst
  (bool useRootNotation, bool useEpsilonNotation, bool useHtml, bool useLatex, bool usePNG, PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
  { std::string tempS;
    this->ElementToStringNegativeRootSpacesFirst(tempS, useRootNotation, useEpsilonNotation, useHtml, useLatex, usePNG, theFormat, theGlobalVariables);
    return tempS;
  }
  std::string ElementToStringLieBracketPairing();
  void ComputeDebugString(bool useHtml, bool useLatex, GlobalVariables& theGlobalVariables){ this->ElementToString(this->DebugString, useHtml, useLatex, theGlobalVariables); }
  std::string ElementToStringRootIndexToEpsForm(int theIndex, GlobalVariables& theGlobalVariables) {return this->theWeyl.GetEpsilonCoords(this->theWeyl.RootSystem[theIndex], theGlobalVariables).ElementToStringEpsilonForm();}
  std::string ElementToStringRootIndexToSimpleBasis
  (int theIndex, GlobalVariables& theGlobalVariables)
  { return this->theWeyl.RootSystem[theIndex].ElementToStringLetterFormat("\\alpha", true);
  }
  bool flagAnErrorHasOccurredTimeToPanic;
  WeylGroup theWeyl;
  //List<VermaModuleWord> VermaWords;
/*  List<VermaModuleMonomial> VermaMonomials;
  List<MatrixLargeRational> EmbeddingsRootSpaces;
  List<MatrixLargeRational> EmbeddingsCartan;*/
  //format:
  //the Chevalley constants are listed in the same order as the root system of the Weyl group
  // i.e. if \alpha is the root at the i^th position in this->theWyl.RootSystem and \beta -
  //the root at the j^th position, then
  //the chevalley constant N_{\alpha\beta} given by [g^\alpha, g^\beta]=N_{\alpha\beta}g^{\alpha+\beta}
  //will be located at the ij^{th} entry in the below matrix.
  //Let $\alpha$ be a root. Then our choice of the elements of the Cartan subalgebra is such that
  //1.   [g^{\alpha}, g^{-\alpha}]=h_\alpha * (2/ \langle\alpha,\alpha\rangle)
  //2.   [h_{\alpha},g^\beta] :=\langle\alpha,\beta\rangle g^\beta
  //Reference: Samelson, Notes on Lie algebras, pages 46-51
  void ComputeMultTable(GlobalVariables& theGlobalVariables);
  MatrixLargeRational ChevalleyConstants;
  Matrix<bool> Computed;
  //The below gives a total ordering to all generators, including the elements of the Cartan
  //the order is:  We put first the generators corresponding to positive roots,
  //we put second the elements of the Cartan
  //we put last the negative roots
  Matrix<int> theLiebracketPairingIndices;
  MatrixLargeRational theLiebracketPairingCoefficients;
  List<int> OppositeRootSpaces;
  List<int> UEGeneratorOrderIncludingCartanElts;

  bool CommutatorIsNonZero(int leftIndex, int rightIndex)
  { if (this->OppositeRootSpaces.TheObjects[leftIndex]==rightIndex)
      return true;
    if (theLiebracketPairingIndices.elements[leftIndex][rightIndex]!=-1)
      return true;
    return false;
  }
  static std::string GetLieAlgebraTypeAndName(char WeylLetter, int WeylDim)
  { std::stringstream out;
    out << WeylLetter << "_" << WeylDim;
    if (WeylLetter!='E' && WeylLetter!='F' && WeylLetter!='G')
      out << " (" << SemisimpleLieAlgebra::GetLieAlgebraName(WeylLetter, WeylDim) << ")";
    return out.str();
  }
  static std::string GetLieAlgebraName(char WeylLetter, int WeylDim)
  { std::stringstream out;
    switch (WeylLetter)
    { case 'A':  out << "sl(" << WeylDim+1 << ")"; break;
      case 'B':  out << "so(" << 2*WeylDim+1 << ")"; break;
      case 'C':  out << "sp(" << 2*WeylDim << ")"; break;
      case 'D':  out << "so(" << 2*WeylDim << ")"; break;
      default: out << WeylLetter << "_" << WeylDim; break;
    }
    return out.str();
  }
  void GetMinusTransposeAuto(const ElementSimpleLieAlgebra& input, ElementSimpleLieAlgebra& output);
  void GenerateWeightSupportMethod2(root& theHighestWeight, roots& output, GlobalVariables& theGlobalVariables);
  inline int GetNumGenerators()const{ return this->theWeyl.CartanSymmetric.NumRows+this->theWeyl.RootSystem.size;}
  inline int GetNumPosRoots()const{ return this->theWeyl.RootsOfBorel.size;}
  inline int GetRank()const{ return this->theWeyl.CartanSymmetric.NumRows;}
  int CartanIndexToChevalleyGeneratorIndex(int theIndex){ return this->theWeyl.RootsOfBorel.size+theIndex;}
  int RootToIndexInUE(const root& input){ return this->RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(this->theWeyl.RootSystem.IndexOfObjectHash(input));}
  int DisplayIndexToRootIndex(int theIndex);
  int DisplayIndexToChevalleyGeneratorIndex(int theIndex)
  { if (theIndex<0)
      return theIndex+this->GetNumPosRoots();
    return theIndex+this->GetNumPosRoots()+this->GetRank()-1;
  }
  int RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(int theIndex)const;
  int RootIndexToDisplayIndexNegativeSpacesFirstThenCartan(int theIndex)const;
  //the below function returns an negative number if the chevalley generator is an element of the Cartan subalgebra
  int ChevalleyGeneratorIndexToRootIndex(int theIndex)const;
  int ChevalleyGeneratorIndexToElementCartanIndex(int theIndex){ return theIndex-this->theWeyl.RootsOfBorel.size;}
  int ChevalleyGeneratorIndexToDisplayIndex(int theIndex)const{ return this->RootIndexToDisplayIndexNegativeSpacesFirstThenCartan(this->ChevalleyGeneratorIndexToRootIndex(theIndex));}
  bool AreOrderedProperly(int leftIndex, int rightIndex);
  std::string GetLetterFromGeneratorIndex
  (int theIndex, bool useRootIndices, bool useEpsCoords, PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
;
  void GenerateVermaMonomials(root& highestWeight, GlobalVariables& theGlobalVariables);
  void ComputeChevalleyConstants(WeylGroup& input, GlobalVariables& theGlobalVariables);
  void ComputeChevalleyConstants(char WeylLetter, int WeylIndex, GlobalVariables& theGlobalVariables);
  //Setup: \gamma+\delta=\epsilon+\zeta=\eta is a root.
  //then the below function computes n_{-\epsilon, -\zeta}
  void LieBracket(const ElementSimpleLieAlgebra& g1, const ElementSimpleLieAlgebra& g2, ElementSimpleLieAlgebra& output)const;
  void ComputeOneChevalleyConstant(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta);
  void ExploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ);
  void ExploitSymmetryChevalleyConstants(int indexI, int indexJ);
  void ExploitTheCyclicTrick(int i, int j, int k);
  int GetMaxQForWhichBetaMinusQAlphaIsARoot(root& alpha, root& beta);
  Rational GetConstant(const root& root1, const root& root2);
  void ComputeCommonAdEigenVectors
(int theDegree, List<ElementUniversalEnveloping<PolynomialRationalCoeff> >& theGenerators,
 List<ElementUniversalEnveloping<PolynomialRationalCoeff> >& generatorsBeingActedOn,
 List<ElementUniversalEnveloping<PolynomialRationalCoeff> >& output, std::stringstream& out,
 GlobalVariables& theGlobalVariables)
  ;
  void ComputeCommonAdEigenVectorsFixedWeight
(root& theWeight, roots& theHs, List<ElementUniversalEnveloping<PolynomialRationalCoeff> >& theGenerators,
 List<ElementUniversalEnveloping<PolynomialRationalCoeff> >& output, std::stringstream& out,
 GlobalVariables& theGlobalVariables)
;
  bool CheckClosedness(std::string& output, GlobalVariables& theGlobalVariables);
  void ElementToStringVermaMonomials(std::string& output);
  void ElementToStringEmbedding(std::string& output);
  //returns true if returning constant, false if returning element of h
  bool GetConstantOrHElement(const root& root1, const root& root2, Rational& outputRat, root& outputH);
  bool TestForConsistency(GlobalVariables& theGlobalVariables);
  bool FindComplementaryNilpotent(ElementSimpleLieAlgebra& e, ElementSimpleLieAlgebra& output, GlobalVariables& theGlobalVariables);
  bool AttemptExtendingHEtoHEF(root& h, ElementSimpleLieAlgebra& e, ElementSimpleLieAlgebra& output, GlobalVariables& theGlobalVariables);
  bool AttemptExtendingHEtoHEFWRTSubalgebra(roots& RootsWithCharacteristic2, roots& relativeRootSystem, Selection& theZeroCharacteristics, roots& simpleBasisSA, root& h, ElementSimpleLieAlgebra& outputE, ElementSimpleLieAlgebra& outputF, MatrixLargeRational& outputMatrixSystemToBeSolved, PolynomialsRationalCoeff& outputSystemToBeSolved, MatrixLargeRational& outputSystemColumnVector, GlobalVariables& theGlobalVariables);
  void FindSl2Subalgebras(SltwoSubalgebras& output, char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables);
  void FindSl2Subalgebras(SltwoSubalgebras& output, GlobalVariables& theGlobalVariables);
  void GetSl2SubalgebraFromRootSA(GlobalVariables& theGlobalVariables);
  void GetAdNilpotentElement(MatrixLargeRational& output, ElementSimpleLieAlgebra& e);
  void initHEFSystemFromECoeffs(int theRelativeDimension, Selection& theZeroCharacteristics, roots& rootsInPlay, roots& simpleBasisSA,  roots& SelectedExtraPositiveRoots, int numberVariables, int numRootsChar2, int halfNumberVariables, root& targetH, MatrixLargeRational& inputFCoeffs, MatrixLargeRational& outputMatrixSystemToBeSolved, MatrixLargeRational& outputSystemColumnVector, PolynomialsRationalCoeff& outputSystemToBeSolved);
  void MakeChevalleyTestReport(int i, int j, int k, int Total, GlobalVariables& theGlobalVariables);
  void MakeSl2ProgressReport(int progress, int found, int foundGood, int DifferentHs, int outOf, GlobalVariables& theGlobalVariables);
  void MakeSl2ProgressReportNumCycles(int progress, int outOf, GlobalVariables& theGlobalVariables);
  bool IsInTheWeightSupport(root& theWeight, root& highestWeight, GlobalVariables& theGlobalVariables);
  void GenerateOneMonomialPerWeightInTheWeightSupport(root& theHighestWeight, GlobalVariables& theGlobalVariables);
  void CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(root& theHighestWeight, GlobalVariables& theGlobalVariables);
  int GetLengthStringAlongAlphaThroughBeta(root& alpha, root& beta, int& distanceToHighestWeight, roots& weightSupport);
  void ComputeOneAutomorphism(GlobalVariables& theGlobalVariables, MatrixLargeRational& outputAuto,  bool useNegativeRootsFirst);
  void operator=(const SemisimpleLieAlgebra& other){ this->Assign(other);}
  void Assign(const SemisimpleLieAlgebra& other)
  { this->theWeyl.Assign(other.theWeyl);
    this->ChevalleyConstants.Assign(other.ChevalleyConstants);
    this->OppositeRootSpaces.CopyFromBase(other.OppositeRootSpaces);
    this->theLiebracketPairingCoefficients.Assign(other.theLiebracketPairingCoefficients);
    this->theLiebracketPairingIndices.Assign(other.theLiebracketPairingIndices);
    this->Computed.Assign(other.Computed);
    this->UEGeneratorOrderIncludingCartanElts=other.UEGeneratorOrderIncludingCartanElts;
  }
};

class Parser;
class HomomorphismSemisimpleLieAlgebra
{
public:
  SemisimpleLieAlgebra theDomain;
  SemisimpleLieAlgebra theRange;
  //Let rk:=Rank(Domain)
  //format of ImagesSimpleChevalleyGenerators: the first rk elements give the images of the Chevalley generators corresponding to simple positive roots
  //the second rk elements give the images of the Chevalley generators corresponding to simple negative roots
  List<ElementSimpleLieAlgebra> imagesSimpleChevalleyGenerators;
  //format of ImagesAllChevalleyGenerators: the Generators are given in the same order as the one used in MonomialUniversalEnveloping
  List<ElementSimpleLieAlgebra> imagesAllChevalleyGenerators;
  List<ElementSimpleLieAlgebra> domainAllChevalleyGenerators;
  List<ElementSimpleLieAlgebra> GmodK;
  roots RestrictedRootSystem;
  std::string DebugString;
  void GetWeightsGmodKInSimpleCoordsK
  (roots& outputWeights, GlobalVariables& theGlobalVariables)
  { this->GetWeightsWrtKInSimpleCoordsK(outputWeights, this->GmodK, theGlobalVariables);}
  void GetWeightsKInSimpleCoordsK
  (roots& outputWeights, GlobalVariables& theGlobalVariables)
  { this->GetWeightsWrtKInSimpleCoordsK(outputWeights, this->imagesAllChevalleyGenerators, theGlobalVariables); }
  void GetWeightsWrtKInSimpleCoordsK
  (roots& outputWeights, List<ElementSimpleLieAlgebra>& inputElts, GlobalVariables& theGlobalVariables)
  ;
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables) {this->ElementToString(output, false, theGlobalVariables);};
  void ElementToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables);
  void MakeG2InB3(Parser& owner, GlobalVariables& theGlobalVariables);
  void MakeGinGWithId(char theWeylLetter, int theWeylDim, GlobalVariables& theGlobalVariables);
  void ProjectOntoSmallCartan(root& input, root& output, GlobalVariables& theGlobalVariables);
  void ProjectOntoSmallCartan(roots& input, roots& output, GlobalVariables& theGlobalVariables);
  void GetMapSmallCartanDualToLargeCartanDual(MatrixLargeRational& output);
  void ComputeDebugString(GlobalVariables& theGlobalVariables){this->ElementToString(this->DebugString, theGlobalVariables);}
  void ComputeDebugString(bool useHtml, GlobalVariables& theGlobalVariables){this->ElementToString(this->DebugString, useHtml, theGlobalVariables);}
  std::string ElementToString(GlobalVariables& theGlobalVariables){ std::string tempS; this->ElementToString(tempS, theGlobalVariables); return tempS; }
  void GetRestrictionAmbientRootSystemToTheSmallerCartanSA(roots& output, GlobalVariables& theGlobalVariables);
  bool ComputeHomomorphismFromImagesSimpleChevalleyGenerators(GlobalVariables& theGlobalVariables);
  bool CheckClosednessLieBracket(GlobalVariables& theGlobalVariables);
  void ApplyHomomorphism
  (ElementSimpleLieAlgebra& input, ElementSimpleLieAlgebra& output)
  ;
  bool ApplyHomomorphism
  (ElementUniversalEnveloping<PolynomialRationalCoeff>& input,
   ElementUniversalEnveloping<PolynomialRationalCoeff>& output, GlobalVariables& theGlobalVariables)
   ;
  bool ApplyHomomorphism
  (MonomialUniversalEnveloping<PolynomialRationalCoeff>& input,
   ElementUniversalEnveloping<PolynomialRationalCoeff>& output, GlobalVariables& theGlobalVariables)
   ;
};

class SemisimpleLieAlgebraOrdered
{
public:
  SemisimpleLieAlgebra theOwner;
  //the format of the order is arbitrary except for the following requirements:
  //-All elements of the order must be either 1) nilpotent or 2) elements of the Cartan
  //-Let the number of positive roots be N and the rank be K. Then the indices N,..., N+K-1 must
  //   correspond to the elements of the Cartan.
  List<ElementSimpleLieAlgebra> theOrder;
  //The order of chevalley generators is as follows. First come negative roots, then elements of cartan, then positive roots
  //The weights are in increasing order
  //The i^th column of the matrix gives the coordinates of the i^th Chevalley generator in the current coordinates
  MatrixLargeRational ChevalleyGeneratorsInCurrentCoords;
  std::string GetGeneratorString
  (int theIndex, bool formatCalculator, bool& mustUsebracketsWhenRaisingToPower,
   const PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
     ;
  void AssignGeneratorCoeffOne(int theIndex, ElementSimpleLieAlgebra& output){output.operator=(this->theOrder.TheObjects[theIndex]);}
  int GetDisplayIndexFromGeneratorIndex(int GeneratorIndex);
  void GetLinearCombinationFrom
  (ElementSimpleLieAlgebra& input, root& theCoeffs)
  ;
  void init
  (List<ElementSimpleLieAlgebra>& inputOrder, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
;
 void initDefaultOrder
  (SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
;
};

template<class CoefficientType>
class ElementUniversalEnvelopingOrdered;

template<class CoefficientType>
class MonomialUniversalEnvelopingOrdered
{
  void SimplifyAccumulateInOutputNoOutputInit
(ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0
 )
  ;
  MonomialUniversalEnvelopingOrdered(const MonomialUniversalEnvelopingOrdered& other);
public:
  SemisimpleLieAlgebraOrdered* owner;
  std::string DebugString;
  std::string ElementToString
(bool useLatex, bool useCalculatorFormat,
 const PolynomialOutputFormat& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
;
  void ComputeDebugString()
  { GlobalVariables theGlobalVariables;
    PolynomialOutputFormat PolyFormatLocal;
    this->DebugString=this->ElementToString(false, true, PolyFormatLocal, theGlobalVariables);
  }
  // SelectedIndices gives the non-zero powers of the generators participating in the monomial
  // Powers gives the powers of the generators in the order specified in the owner
  List<int> generatorsIndices;
  List<CoefficientType> Powers;
  CoefficientType Coefficient;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void MultiplyBy
  (const MonomialUniversalEnveloping<CoefficientType>& other,
   ElementUniversalEnvelopingOrdered<CoefficientType>& output)
  ;
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const CoefficientType& thePower);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower);
  void MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other);
  void Nullify(int numVars, SemisimpleLieAlgebraOrdered& theOwner, GlobalVariables* theContext);
  void Nullify(const CoefficientType& theRingZero, SemisimpleLieAlgebraOrdered& theOwner);
  bool ModOutFDRelationsExperimental
    (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
;
  void ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement=0,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
     ;
  void ModOutVermaRelationSOld(bool SubHighestWeightWithZeroes);
  void ModOutVermaRelationSOld
  (bool SubHighestWeightWithZeroes, const PolynomialsRationalCoeff& highestWeightSub,
   GlobalVariables* theContext, const CoefficientType& theRingUnit)
  ;
  void SetNumVariables(int newNumVars);
  int HashFunction() const;
  void GetDegree(PolynomialRationalCoeff& output)
  { output.Nullify(this->Coefficient.NumVars);
    for (int i=0; i<this->generatorsIndices.size; i++)
      output.AddPolynomial(this->Powers.TheObjects[i]);
  }
  bool GetElementUniversalEnveloping
  (ElementUniversalEnveloping<CoefficientType>& output, SemisimpleLieAlgebra& owner)
  ;
  bool IsEqualToZero()const{return this->Coefficient.IsEqualToZero();}
  bool CommutingLeftIndexAroundRightIndexAllowed(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex);
  bool CommutingRightIndexAroundLeftIndexAllowed(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex);
  bool SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex, MonomialUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingZero=0)
   ;
  void MakeConst(const CoefficientType& theConst, SemisimpleLieAlgebraOrdered& theOwner){this->generatorsIndices.size=0; this->Powers.size=0; this->Coefficient=theConst; this->owner=&theOwner;}
  void Simplify
  (ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
  void CommuteConsecutiveIndicesLeftIndexAroundRight
  (int theIndeX, ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
    const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0
    )
  ;
  void CommuteConsecutiveIndicesRightIndexAroundLeft
(int theIndeX, ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0
 )
  ;
  MonomialUniversalEnvelopingOrdered(){this->owner=0;}
  void SubstitutionCoefficients
(PolynomialsRationalCoeff& theSub)
;
  bool operator==(const MonomialUniversalEnvelopingOrdered& other)const
  { assert(this->owner==other.owner);
    return this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;
  }
  void operator*=(const MonomialUniversalEnvelopingOrdered& other);
  template<class OtherCoefficientType>
  void AssignChangeCoefficientType(const MonomialUniversalEnvelopingOrdered<OtherCoefficientType>& other)
  { this->Coefficient=other.Coefficient;
    this->generatorsIndices=other.generatorsIndices.size;
    this->Powers.SetSize(other.Powers.size);
    for(int i=0; i<this->Powers.size; i++)
      this->Powers[i]=other.Powers[i];
  }
  inline void operator=(const MonomialUniversalEnvelopingOrdered& other)
  { this->generatorsIndices.CopyFromBase(other.generatorsIndices);
    this->Powers.CopyFromBase(other.Powers);
    this->Coefficient.Assign(other.Coefficient);
    this->owner=other.owner;
  }
};

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::flagAnErrorHasOccurredTimeToPanic=false;

template <class CoefficientType>
class ElementUniversalEnvelopingOrdered : public HashedList<MonomialUniversalEnvelopingOrdered<CoefficientType> >
{
private:
  void AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnvelopingOrdered<CoefficientType>& input);
  void CleanUpZeroCoeff();
  friend class MonomialUniversalEnvelopingOrdered<CoefficientType>;
public:
  std::string DebugString;
  GlobalVariables* context;
  void ElementToString
(std::string& output, bool useLatex,
 bool useCalculatorFormat,
 const PolynomialOutputFormat& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
  ;
  std::string ElementToString
  (bool useLatex, bool useCalculatorFormat, const PolynomialOutputFormat& PolyFormatLocal,
   GlobalVariables& theGlobalVariables)const
  { std::string tempS;
    this->ElementToString(tempS, useLatex, useCalculatorFormat, PolyFormatLocal, theGlobalVariables);
    return tempS;
  }
  std::string ElementToString
  (const PolynomialOutputFormat& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
  { std::string tempS;
    this->ElementToString(tempS, true, true, PolyFormatLocal, theGlobalVariables);
    return tempS;
  }
  std::string ElementToString(const PolynomialOutputFormat& PolyFormatLocal)const
  { GlobalVariables theGlobalVariables;
    return this->ElementToString(PolyFormatLocal, theGlobalVariables);
  }
  bool ElementToStringNeedsBracketsForMultiplication()const{return this->size>1;}
  void ComputeDebugString()
  { PolynomialOutputFormat tempFormat;
    GlobalVariables theGlobalVariables;
    this->DebugString=this->ElementToString(tempFormat, theGlobalVariables);
  }
  SemisimpleLieAlgebraOrdered* owner;
  void AddMonomial(const MonomialUniversalEnvelopingOrdered<CoefficientType>& input);
  void AssignElementCartan(const root& input, int numVars, SemisimpleLieAlgebraOrdered& theOwner);
  void AssignElementLieAlgebra
(const ElementSimpleLieAlgebra& input, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
;
  void MakeOneGenerator
  (int theIndex, const CoefficientType& theCoeff, SemisimpleLieAlgebraOrdered& owner,
   GlobalVariables* theContext);
//  void MakeOneGeneratorCoeffOne(root& rootSpace, int numVars, SemisimpleLieAlgebraOrdered& theOwner){this->MakeOneGeneratorCoeffOne(theOwner.RootToIndexInUE(rootSpace), numVars, theOwner);};
  void Nullify(SemisimpleLieAlgebraOrdered& theOwner);
  bool AssignElementUniversalEnveloping
  (ElementUniversalEnveloping<CoefficientType>& input, SemisimpleLieAlgebraOrdered& owner,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
  ;
  bool AssignMonomialUniversalEnveloping
  (MonomialUniversalEnveloping<CoefficientType>& input, SemisimpleLieAlgebraOrdered& owner,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
  ;
  bool ModOutFDRelationsExperimental
  (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
  bool IsEqualToZero()const {return this->size==0;}
  bool GetElementUniversalEnveloping
  (ElementUniversalEnveloping<CoefficientType>& output, SemisimpleLieAlgebra& owner);
  bool ConvertToLieAlgebraElementIfPossible(ElementSimpleLieAlgebra& output)const;
  void SubstitutionCoefficients
(PolynomialsRationalCoeff& theSub, GlobalVariables* theContext)
  ;
  void MakeConst(const CoefficientType& coeff, SemisimpleLieAlgebraOrdered& theOwner)
  { this->Nullify(theOwner);
    MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
    tempMon.MakeConst(coeff, theOwner);
    this->AddMonomial(tempMon);
  }
  void Simplify
  (GlobalVariables* theContext,
    const CoefficientType& theRingUnit=1,  const CoefficientType& theRingZero=0);
  int GetNumVariables()const
  { if (this->size==0)
      return 0;
    else
      return this->TheObjects[0].Coefficient.NumVars;
  }
  inline void MultiplyBy(const ElementUniversalEnvelopingOrdered& other){this->operator*=(other);}
  void ModOutVermaRelationSOld(GlobalVariables* theContext, const CoefficientType& theRingUnit, int NumVars){ this->ModOutVermaRelationSOld(false, theContext, theRingUnit, NumVars);}
  void ModOutVermaRelationSOld(bool SubHighestWeightWithZeroes, GlobalVariables* theContext, const CoefficientType& theRingUnit, int NumVars)
  { PolynomialsRationalCoeff theSub;
    theSub.MakeIdSubstitution(NumVars, (Rational) 1);
    this->ModOutVermaRelationSOld(SubHighestWeightWithZeroes, theSub, theContext, theRingUnit);
  }
  void ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement=0,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;

  void ModOutVermaRelationSOld
  (bool SubHighestWeightWithZeroes, const PolynomialsRationalCoeff& highestWeightSub, GlobalVariables* theContext, const CoefficientType& theRingUnit)
  ;
  template<class CoefficientTypeQuotientField>
  static void GetBasisFromSpanOfElements
  (List<ElementUniversalEnvelopingOrdered<CoefficientType> >& theElements,
   Vectors<CoefficientTypeQuotientField>& outputCoords,
   List<ElementUniversalEnvelopingOrdered<CoefficientType> >& outputTheBasis,
   const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero,
   GlobalVariables& theGlobalVariables)
   ;
  static void GetBasisFromSpanOfElements
  (List<ElementUniversalEnvelopingOrdered>& theElements, List<rootPoly>& outputCoordinates,
   List<ElementUniversalEnvelopingOrdered>& outputTheBasis, GlobalVariables& theGlobalVariables)
;
  bool GetCoordsInBasis
  (List<ElementUniversalEnvelopingOrdered<CoefficientType> >& theBasis, Vector<CoefficientType>& output,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)const
  ;
  static void GetCoordinateFormOfSpanOfElements
  (int numVars, List<ElementUniversalEnvelopingOrdered>& theElements, List<rootPoly>& outputCoordinates, ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
;
//  static void GetCoordinateFormOfSpanOfElements
//  (List<ElementUniversalEnvelopingOrdered>& theElements, roots& outputCoordinates, ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
//;
  void AssignFromCoordinateFormWRTBasis
  (List<ElementUniversalEnveloping<CoefficientType> >& theBasis, rootPoly& input,
   SemisimpleLieAlgebraOrdered& owner)
  ;
  void SetNumVariables(int newNumVars);
  void RaiseToPower(int thePower, const CoefficientType& theRingUnit);
  bool IsAPowerOfASingleGenerator()
  { if (this->size!=1)
      return false;
    MonomialUniversalEnvelopingOrdered<CoefficientType>& tempMon=this->TheObjects[0];
    if (!tempMon.Coefficient.IsEqualToOne())
      return false;
    if (tempMon.generatorsIndices.size!=1)
      return false;
    return true;
  }
  void MakeCasimir(SemisimpleLieAlgebraOrdered& theOwner, int numVars, GlobalVariables& theGlobalVariables);
  void ActOnMe(const ElementSimpleLieAlgebra& theElt, ElementUniversalEnvelopingOrdered& output);
  void LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output);
  void LieBracketOnTheRight
  (const ElementSimpleLieAlgebra& right, const CoefficientType& ringUnit, const CoefficientType& ringZero)
  ;
  void operator=(const ElementUniversalEnvelopingOrdered& other)
  { this->CopyFromHash(other);
    this->owner=other.owner;
  }
  template<class OtherCoefficientType>
  void AssignChangeCoefficientType (const ElementUniversalEnvelopingOrdered<OtherCoefficientType>& other)
  { this->Nullify(*other.owner);
    MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
    this->MakeActualSizeAtLeastExpandOnTop(other.size);
    for (int i=0; i<other.size; i++)
    { tempMon.AssignChangeCoefficientType<OtherCoefficientType>(other[i]);
      this->AddMonomial(tempMon);
    }
  }
  void operator+=(const ElementUniversalEnvelopingOrdered& other);
  void operator+=(int other);
  void operator+=(const Rational& other);
  void operator-=(const ElementUniversalEnvelopingOrdered& other);
  void operator*=(const ElementUniversalEnvelopingOrdered& other);
  template <class SecondType>
  void operator/=(const SecondType& other);
  template<class otherType>
  void operator*=(const otherType& other);
  ElementUniversalEnvelopingOrdered(){this->owner=0;}
  bool IsProportionalTo
 (const ElementUniversalEnvelopingOrdered<CoefficientType>& other,
  CoefficientType& outputTimesMeEqualsOther,
  const CoefficientType& theRingZero)const
  ;
  ElementUniversalEnvelopingOrdered(const ElementUniversalEnvelopingOrdered& other)
  { this->operator=(other);
  }
  void ClearDenominators(CoefficientType& outputWasMultipliedBy, const CoefficientType& theRingUnit)
  { outputWasMultipliedBy=theRingUnit;
    CoefficientType currentCoeff;
    for (int i=0; i<this->size; i++)
    { MonomialUniversalEnvelopingOrdered<CoefficientType>& currentMon=this->TheObjects[i];
      currentMon.Coefficient.ClearDenominators(currentCoeff);
      for (int j=0; j<this->size; j++)
        if (j!=i)
          this->TheObjects[j].Coefficient*=currentCoeff;
      outputWasMultipliedBy*=currentCoeff;
    }
  }
};

template <class CoefficientType>
class MonomialUniversalEnveloping
{
private:
  void SimplifyAccumulateInOutputNoOutputInit
  (ElementUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
public:
  std::string DebugString;
  std::string ElementToString
  (bool useRootIndices, bool useEpsilonCoords, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
  ;
  void ComputeDebugString(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
  { this->DebugString=this->ElementToString(false, false, theGlobalVariables, theFormat);
  }
  SemisimpleLieAlgebra* owner;
  // SelectedIndices gives the non-zero powers of the chevalley generators participating in the monomial
  // Powers gives the powers of the Chevalley generators in the order they appear in generatorsIndices
  List<int> generatorsIndices;
  List<CoefficientType> Powers;
  CoefficientType Coefficient;
  bool AdjointRepresentationAction
  (const ElementUniversalEnveloping<CoefficientType>& input, ElementUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables)
  ;
  void MultiplyBy(const MonomialUniversalEnveloping& other, ElementUniversalEnveloping<CoefficientType>& output);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const CoefficientType& thePower);
  void MultiplyByNoSimplify(const MonomialUniversalEnveloping& other);
  void Nullify(int numVars, SemisimpleLieAlgebra& theOwner);
  void Nullify(const CoefficientType& theRingZero, SemisimpleLieAlgebra& theOwner)
  { this->init(theOwner);
    this->Coefficient=theRingZero;
  }
  bool HWTAAbilinearForm
  (const MonomialUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream=0)
  ;
  void ModOutVermaRelationS(bool SubHighestWeightWithZeroes, GlobalVariables& theGlobalVariables);
  void ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement=0,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
;
  void SetNumVariables(int newNumVars);
  int HashFunction() const;
  void GetDegree(PolynomialRationalCoeff& output)
  { output.Nullify(this->Coefficient.NumVars);
    for (int i=0; i<this->generatorsIndices.size; i++)
      output.AddPolynomial(this->Powers.TheObjects[i]);
  }
  void init(SemisimpleLieAlgebra& theOwner)
  { this->owner=&theOwner;
    this->Powers.size=0;
    this->generatorsIndices.size=0;
  }
  bool CommutingLeftIndexAroundRightIndexAllowed
(CoefficientType& theLeftPower, int leftGeneratorIndex,
 CoefficientType& theRightPower, int rightGeneratorIndex)
   ;
  bool CommutingRightIndexAroundLeftIndexAllowed
(CoefficientType& theLeftPower, int leftGeneratorIndex,
 CoefficientType& theRightPower, int rightGeneratorIndex)
   ;
  bool SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex, MonomialUniversalEnveloping& output, GlobalVariables& theGlobalVariables)
  ;
    bool ModOutFDRelationsExperimental
    (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
;
  void SubstitutionCoefficient
  (PolynomialsRationalCoeff& theSub)
  ;
  void MakeConst(const CoefficientType& theConst, SemisimpleLieAlgebra& theOwner)
  { this->generatorsIndices.size=0;
    this->Powers.size=0;
    this->Coefficient=theConst;
    this->owner=&theOwner;
  }
  //we assume the standard order for being simplified to be Ascending.
  //this way the positive roots will end up being in the end, which is very convenient for computing with Verma modules
  //format of the order of the generators:
  // first come the negative roots, in increasing height, then the elements of the Cartan subalgebra, then the positive roots, in increasing height
  //The order of the positive roots is the same as the order in which roots are kept in WeylGroup::RootSystem
  // The order of the negative roots is reverse to the order in which they are kept in WeylGroup::RootSystem
  // with the "zero level roots" - i.e. the elements of the Cartan subalgebra - put in between.
  void Simplify
  (ElementUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0 )
  ;
  void CommuteConsecutiveIndicesLeftIndexAroundRight
(int theIndeX, ElementUniversalEnveloping<CoefficientType>& output,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
  void CommuteConsecutiveIndicesRightIndexAroundLeft
(int theIndeX, ElementUniversalEnveloping<CoefficientType>& output,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
  MonomialUniversalEnveloping(){this->owner=0;}
  bool operator>(const MonomialUniversalEnveloping& other)
  { if (other.generatorsIndices.size>this->generatorsIndices.size)
      return false;
    if (other.generatorsIndices.size< this->generatorsIndices.size)
      return true;
    for (int i=0; i<this->generatorsIndices.size; i++)
    { if (other.generatorsIndices[i]>this->generatorsIndices[i])
        return false;
      if (other.generatorsIndices[i]<this->generatorsIndices[i])
        return true;
      if (other.Powers[i]>this->Powers[i])
        return false;
      if (other.Powers[i]<this->Powers[i])
        return true;
    }
    return false;
  }
  bool operator==(const MonomialUniversalEnveloping& other)const{ return this->owner==other.owner && this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;}
  inline void operator=(const MonomialUniversalEnveloping& other)
  { this->generatorsIndices.CopyFromBase(other.generatorsIndices);
    this->Powers.CopyFromBase(other.Powers);
    this->Coefficient.Assign(other.Coefficient);
    this->owner=other.owner;
  }
};

template <class CoefficientType>
class ElementUniversalEnveloping: public HashedList<MonomialUniversalEnveloping<CoefficientType> >
{
private:
  void AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnveloping<CoefficientType>& input);
  void CleanUpZeroCoeff();
  friend class MonomialUniversalEnveloping<CoefficientType>;
public:
  std::string DebugString;
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)const
  { this->ElementToString(output, false, false, theGlobalVariables, theFormat);
  }
  void ElementToString
  (std::string& output, bool useRootIndices, bool includeEpsilonCoords,
   GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)const
  ;
  std::string ElementToString(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)const
  { std::string tempS;
    this->ElementToString(tempS, theGlobalVariables, theFormat);
    return tempS;
  }
  std::string ElementToString(bool useLatex, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)const
  { std::string tempS;
    this->ElementToString(tempS, useLatex, false, theGlobalVariables, theFormat);
    return tempS;
  }
  std::string ElementToStringCalculatorFormat(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)const
  { std::string tempS;
    this->ElementToString(tempS, false, false, theGlobalVariables, theFormat);
    return tempS;
  }
  std::string ElementToString
  (bool useLatex, bool includeEpsilonCoords, GlobalVariables& theGlobalVariables,
   PolynomialOutputFormat& theFormat)const
  { std::string tempS;
    this->ElementToString(tempS, useLatex, includeEpsilonCoords, theGlobalVariables, theFormat);
    return tempS;
  }
  void ComputeDebugString(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
  { this->ElementToString(this->DebugString, theGlobalVariables, theFormat);
  }
  SemisimpleLieAlgebra* owner;
  bool AdjointRepresentationAction
  (const ElementUniversalEnveloping<CoefficientType>& input, ElementUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables)
  ;
  bool IsEqualToZero()
  { return this->size==0;
  }
  bool HWMTAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream=0)
  ;
  void PrepareOrderSSalgebraForHWbfComputation()
  { int numPosRoots=this->owner->GetNumPosRoots();
    for (int i=0; i<numPosRoots; i++)
      this->owner->UEGeneratorOrderIncludingCartanElts[i]=-1;
  }
  void RestoreOrderSSLieAlgebra()
  { int numGens=this->owner->GetNumGenerators();
    for (int i=0; i<numGens; i++)
      this->owner->UEGeneratorOrderIncludingCartanElts[i]=i;
  }
  std::string IsInProperSubmodule
  (const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
   ;
  bool HWTAAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream=0)
  ;
  bool HWTAAbilinearForm
  (const MonomialUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream=0)
  { ElementUniversalEnveloping<CoefficientType> tempElt;
    tempElt.Nullify(*this->owner);
    tempElt+=right;
    return this->HWTAAbilinearForm
    (tempElt, output, subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, 0);
  }
  bool ApplyMinusTransposeAutoOnMe();
  bool ApplyTransposeAntiAutoOnMe();
  void AddMonomial(const MonomialUniversalEnveloping<CoefficientType>& input);
  void AssignElementCartan(const root& input, int numVars, SemisimpleLieAlgebra& theOwner);
  void AssignElementLieAlgebra(const ElementSimpleLieAlgebra& input, int numVars, SemisimpleLieAlgebra& theOwner);
  void MakeOneGenerator
  (int theIndex, const CoefficientType& theCoeff, SemisimpleLieAlgebra& owner)
  ;
  bool ModOutFDRelationsExperimental
  (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
   ;
  void MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebra& theOwner);
  void MakeOneGeneratorCoeffOne(const root& rootSpace, int numVars, SemisimpleLieAlgebra& theOwner)
  { this->MakeOneGeneratorCoeffOne(theOwner.RootToIndexInUE(rootSpace), numVars, theOwner);
  }
  void Nullify(SemisimpleLieAlgebra& theOwner);
  bool ConvertToLieAlgebraElementIfPossible(ElementSimpleLieAlgebra& output)const;
  void MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebra& theOwner);
  void MakeConst(const CoefficientType& coeff, SemisimpleLieAlgebra& theOwner)
  { this->Nullify(theOwner);
    MonomialUniversalEnveloping<CoefficientType> tempMon;
    tempMon.MakeConst(coeff, theOwner);
    this->AddMonomial(tempMon);
  }
  void Simplify
  (GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero)
   ;
  int GetNumVariables()
  { if (this->size==0)
      return 0;
    else
      return this->TheObjects[0].Coefficient.NumVars;
  }
  inline void MultiplyBy(const ElementUniversalEnveloping<CoefficientType>& standsOnTheRight)
  { this->operator*=(standsOnTheRight);
  }
  void ModToMinDegreeFormFDRels
  (const root& theHWinSimpleCoords,
   GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero);
  void ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
   ;
  void ModOutVermaRelationS(GlobalVariables& theGlobalVariables){ this->ModOutVermaRelationS(false, theGlobalVariables);}
  void ModOutVermaRelationS(bool SubHighestWeightWithZeroes, GlobalVariables& theGlobalVariables);
  static void GetCoordinateFormOfSpanOfElements
  (int numVars, List<ElementUniversalEnveloping<CoefficientType> >& theElements,
   List<rootPoly>& outputCoordinates, ElementUniversalEnveloping<CoefficientType>& outputCorrespondingMonomials,
   GlobalVariables& theGlobalVariables)
;
  bool GetCoordsInBasis
  (List<ElementUniversalEnveloping<CoefficientType> >& theBasis, Vector<CoefficientType>& output,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)const
;

template<class CoefficientTypeQuotientField>
static bool GetBasisFromSpanOfElements
  (List<ElementUniversalEnveloping<CoefficientType> >& theElements,
   Vectors<CoefficientTypeQuotientField>& outputCoords,
   List<ElementUniversalEnveloping<CoefficientType> >& outputTheBasis,
   const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero,
   GlobalVariables& theGlobalVariables)
   ;
  static void GetCoordinateFormOfSpanOfElements
  (List<ElementUniversalEnveloping<CoefficientType> >& theElements, roots& outputCoordinates,
   ElementUniversalEnveloping<CoefficientType>& outputCorrespondingMonomials,
   GlobalVariables& theGlobalVariables)
   ;
  void AssignFromCoordinateFormWRTBasis
  (List<ElementUniversalEnveloping<CoefficientType> >& theBasis, rootPoly& input, SemisimpleLieAlgebra& owner)
  ;
  void SetNumVariables(int newNumVars);
  void RaiseToPower(int thePower);
  bool IsAPowerOfASingleGenerator()
  { if (this->size!=1)
      return false;
    MonomialUniversalEnveloping<CoefficientType>& tempMon=this->TheObjects[0];
    if (!tempMon.Coefficient.IsEqualToOne())
      return false;
    if (tempMon.generatorsIndices.size!=1)
      return false;
    return true;
  }
  void SubstitutionCoefficients
(PolynomialsRationalCoeff& theSub, GlobalVariables* theContext,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
  ;
  void MakeCasimir(SemisimpleLieAlgebra& theOwner, int numVars, GlobalVariables& theGlobalVariables);
  void LieBracketOnTheRight(const ElementUniversalEnveloping<CoefficientType>& right, ElementUniversalEnveloping<CoefficientType>& output);
  void LieBracketOnTheLeft(const ElementSimpleLieAlgebra& left);
  void AssignInt(int coeff, int numVars, SemisimpleLieAlgebra& theOwner){ Rational tempRat=coeff; this->MakeConst(tempRat, numVars, theOwner);};
  void operator=(const ElementUniversalEnveloping<CoefficientType>& other)
  { this->CopyFromHash(other);
    this->owner=other.owner;
  }
  void operator=(const MonomialUniversalEnveloping<CoefficientType>& other)
  { this->Nullify(*other.owner);
    this->AddMonomial(other);
  }
  void operator+=(const ElementUniversalEnveloping<CoefficientType>& other);
  void operator+=(const MonomialUniversalEnveloping<CoefficientType>& other){this->AddMonomial(other);}
  void operator+=(int other);
  void operator+=(const Rational& other);
  void operator-=(const ElementUniversalEnveloping<CoefficientType>& other);
  void operator*=(const ElementUniversalEnveloping<CoefficientType>& standsOnTheRight);
  void operator*=(const MonomialUniversalEnveloping<CoefficientType>& standsOnTheRight);
  void operator/=(const Rational& other);
  void operator*=(const Rational& other);
  void operator*=(const PolynomialRationalCoeff& other);
  ElementUniversalEnveloping<CoefficientType>(){this->owner=0;}
  ElementUniversalEnveloping<CoefficientType>(const ElementUniversalEnveloping<CoefficientType>& other){this->operator=(other);}
};

class SltwoSubalgebras : public HashedList<slTwo>
{
public:
  std::string DebugString;
//  List< int > hSingularWeights;
  List<int> MultiplicitiesFixedHweight;
  List<List<int> > IndicesSl2sContainedInRootSA;
  List<int> IndicesSl2decompositionFlas;
  roots BadHCharacteristics;
  int IndexZeroWeight;
  SemisimpleLieAlgebra owner;
  rootSubalgebras theRootSAs;
  void ComputeDebugString(GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml){ this->ElementToString(this->DebugString, theGlobalVariables, theWeyl, useLatex, useHtml, false, 0, 0);};
  List<std::string> texFileNamesForPNG;
  List<std::string> texStringsEachFile;
  List<std::string> listSystemCommandsLatex;
  List<std::string> listSystemCommandsDVIPNG;
  void ComputeModuleDecompositionsOfAmbientLieAlgebra(GlobalVariables& theGlobalVariables)
  { for(int i=0; i<this->size; i++)
      this->TheObjects[i].ComputeModuleDecompositionAmbientLieAlgebra(theGlobalVariables);
  }
  void ComputeModuleDecompositionsOfMinimalContainingRegularSAs(GlobalVariables& theGlobalVariables)
  { for(int i=0; i<this->size; i++)
      this->TheObjects[i].ComputeModuleDecompositionOfMinimalContainingRegularSAs(*this, i, theGlobalVariables);
  }
  void getZuckermansArrayE8(roots& output);
  void MakeProgressReport(int index, int outOf, GlobalVariables& theGlobalVariables);
  void ComputeDebugStringCurrent();
  bool ContainsSl2WithGivenH(root& theH, int* outputIndex);
  bool ContainsSl2WithGivenHCharacteristic(root& theHCharacteristic, int* outputIndex);
  void ElementToHtml
(GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool usePNG, std::string& physicalPath, std::string& htmlPathServer,
 std::string& DisplayNameCalculator)
   ;
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml);
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer);
  void ElementToStringNoGenerators(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer);
};

class reductiveSubalgebra
{
  public:
  int indexInOwner;
};

class reductiveSubalgebras
{
public:
  SltwoSubalgebras theSl2s;
  SltwoSubalgebras CandidatesPrincipalSl2ofSubalgebra;
  Selection RemainingCandidates;
//  List<DynkinDiagramRootSubalgebra> thePossibleDynkinDiagrams;
  List<List<int> > theRanks;
  List<List<int> > theMultiplicities;
  List<List<char> > theLetters;
  List<List<int> > thePartitionValues;
  List<List<int> > thePartitionMultiplicities;
  List<SltwoSubalgebras> theCandidateSubAlgebras;
  List<List<int> > IndicesMatchingSl2s;
  List<List<int> > IndicesMatchingPartitionSl2s;
  List<List<int> > IndicesMatchingActualSl2s;
  void MatchRealSl2sToPartitionSl2s();
  void MakeSelectionBasedOnPrincipalSl2s(GlobalVariables& theGlobalVariables);
  void MatchActualSl2sFixedRootSAToPartitionSl2s(GlobalVariables& theGlobalVariables);
  void GenerateModuleDecompositionsPrincipalSl2s(int theRank, GlobalVariables& theGlobalVariables);
  void ElementToStringCandidatePrincipalSl2s(bool useLatex, bool useHtml, std::string& output, GlobalVariables& theGlobalVariables);
  void ElementToStringDynkinType(int theIndex, bool useLatex, bool useHtml, std::string& output);
  void FindTheReductiveSubalgebras(char WeylLetter, int WeylIndex, GlobalVariables& theGlobalVariables);
  void EnumerateAllPossibleDynkinDiagramsOfRankUpTo(int theRank);
  void GenerateAllPartitionsUpTo(int theRank);
  void GenerateAllPartitionsDontInit(int theRank);
  void GenerateAllDiagramsForPartitionRecursive(int indexPartition, int indexInPartition, List<int>& ranksBuffer, List<int>& multiplicitiesBuffer, List<char>& lettersBuffer);
  void GenerateAllPartitionsRecursive(int remainingToBePartitioned, int CurrentValue, List<int>& Multiplicities, List<int>& Values);
};

class minimalRelationsProverStatesFixedK;

class minimalRelationsProverStateFixedK
{
public:
  std::string DebugString;
  static int ProblemCounter;
  bool flagAnErrorHasOccurredTimeToPanic;
  void ElementToString( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool displayEpsilons);
  void ComputeDebugString(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables){ this->ElementToString(this->DebugString, theWeyl, TheGlobalVariables, true); };
  bool flagNeedsAdditionOfPositiveKroots;
  Selection theGmodLmodules;
  Selection theNilradicalModules;
  root SeparatingNormalUsed;
  roots nonAlphas;
  roots nonBetas;
  List<int> indicesIsosRespectingInitialNilradicalChoice;
  root currentSeparatingNormalEpsilonForm;
  roots theChoicesWeMake;
  coneRelation PartialRelation;
  MatrixLargeRational theScalarProducts;
  bool StateIsPossible;
  bool StateIsComplete;
  //bool StateIsDubious;
  List<int> PossibleChildStates;
  List<int> ImpossibleChildStates;
  List<int> CompleteChildStates;
  int activeChild;
  minimalRelationsProverStatesFixedK* owner;
  void initFromParentState(minimalRelationsProverStateFixedK& parent);
  bool StateAllowsPositiveKChoice(root& theCandidate, root& theNonSingularRoot, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void SortAlphasAndBetas(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  bool IsSeparatingCones(root& input, bool& oneBetaIsPositive, WeylGroup& theWeyl);
  static bool IsSeparatingCones(root& input, roots& theAlphas, roots& theBetas, bool& oneBetaIsPositive, WeylGroup& theWeyl);
  void GetNumberScalarProductsData(root& input, roots& theRoots, bool& isLong, int& NumLongValue, int& NumMixedValue,  int& NumShortValue, int& NumMinusLongValue, int& NumMinusMixedValue,  int& NumMinusShortValue, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, roots& theAlphas, roots& theBetas, MatrixLargeRational& output);
  bool ComputeStateReturnFalseIfDubious(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS);
  bool CanBeShortened(coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas, SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS);
  bool SumWithNoPosRootIsARoot(root& input, WeylGroup& theWeyl);
  void Assign(const minimalRelationsProverStateFixedK& right);
  bool RootIsGoodForPreferredSimpleRoot(root& input, int preferredIndex, bool& GoodForAlpha, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& AlphasMinusBetas);
  void GetPossibleAlphasAndBetas(roots& outputAlphas, roots& outputBetas, WeylGroup& theWeyl);
  void GetCertainGmodLhighestAndNilradicalRoots(roots& outputAGmodLhighest, roots& outputNilradicalRoots, WeylGroup& theWeyl);
  bool RootIsGoodForProblematicIndex(root& input, int problemIndex, bool AddingAlphas, bool NeedPositiveContribution, roots& theDualBasis, WeylGroup& theWeyl);
  bool FindBetaWithoutTwoAlphas(root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl);
  bool ComputeCommonSenseImplicationsReturnFalseIfContradictionFixedK(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  bool IsAGoodPosRootsKChoice(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void MakeAlphaBetaMatrix(MatrixLargeRational& output);
  void operator=(const minimalRelationsProverStateFixedK& right){this->Assign(right); }
  minimalRelationsProverStateFixedK();
  void MakeProgressReportCanBeShortened(int checked, int outOf, GlobalVariables& theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
};

class minimalRelationsProverStatesFixedK: public List<minimalRelationsProverStateFixedK>
{
public:
  std::string DebugString;
  static int ProblemCounter;
  int MinNumDifferentBetas;
  roots PreferredDualBasis;
  roots PreferredDualBasisEpsilonCoords;
  //int RecursionDepth;
  List<int> theIndexStack;
  List<int> IndexKmoduleByRoots;
  rootSubalgebra isomorphismComputer;
  MatrixLargeRational invertedCartan;
  WeylGroup theWeylGroup;
  rootSubalgebra theK;
  ReflectionSubgroupWeylGroup theIsos;
  rootsCollection PrecomputedIsoDomains;
  rootsCollection PrecomputedIsoRanges;
  std::fstream theFile;
  std::string ProverFileName;
  bool flagAssumeGlobalMinimalityRHS;
  bool flagComputationIsInitialized;
  bool flagSearchForOptimalSeparatingRoot;
  bool ExtendToIsomorphismRootSystemFixedK(minimalRelationsProverStateFixedK& theState, int indexOther, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl);
  bool flagAnErrorHasOccurredTimeToPanic;
  void PrepareKIsos();
  int GetModuleIndex(root& input);
  void ElementToString(std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void initShared(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void GetIsoTypicComponents(roots& theRoots, roots& theOtherTypeRoots, permutation& outputComponents, minimalRelationsProverStateFixedK& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ComputeDebugString(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)  { this->ElementToString(this->DebugString, theWeyl, TheGlobalVariables); };
  bool GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive( int index, root& outputNormal, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  bool AddObjectOnTopNoRepetitionOfObjectFixedK( int ParentIndex, minimalRelationsProverStateFixedK& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ComputeLastStackIndexFixedK(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void InvokeExtensionOfState(int index, int UpperLimitChildren, bool oneBetaIsPositive, root& NormalSeparatingCones, bool addFirstAlpha, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void GenerateStartingStatesFixedK(Parser& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension);
  void RecursionStepFixedK(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void TheFullRecursionFixedK (WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ExtensionStepFixedK(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, minimalRelationsProverStateFixedK& newState);
  void TestAddingExtraRootFixedK(int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot, bool AddAlpha, int indexAddedRoot, root& normalSeparatingConesOneBetaPositive, bool oneBetaIsPositive);
  bool GetNormalSeparatingConesFromPreferredBasis(int theIndex, List<root>& inputPreferredBasis, root& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool& oneBetaIsPositive );
  void RemoveDoubt(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  bool StateIsEqualTo(minimalRelationsProverStateFixedK& theState, int IndexOther, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void MakeProgressReportStack(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void MakeProgressReportIsos(int progress, int numSearchedWithinState, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void MakeProgressReportCurrentState(int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void MakeProgressReportChildStates(int numSearched, int outOf, int NewFound, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  minimalRelationsProverStatesFixedK()
  { this->flagComputationIsInitialized=false;
    MinNumDifferentBetas=-1;
    this->flagSearchForOptimalSeparatingRoot=true;
    this->theWeylGroup.CartanSymmetric.NumRows=-1;
  }
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void WriteToFile(std::string& fileName, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::string& fileName, GlobalVariables* theGlobalVariables);
};

class minimalRelationsProverStates;
class minimalRelationsProverState
{
public:
  std::string DebugString;
  static int ProblemCounter;
  bool flagAnErrorHasOccurredTimeToPanic;
  void ElementToString( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool displayEpsilons);
  void ComputeDebugString(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables){ this->ElementToString(this->DebugString, theWeyl, TheGlobalVariables, true);  }
  bool flagNeedsAdditionOfPositiveKroots;
  root currentSeparatingNormalEpsilonForm;
  roots theChoicesWeMake;
  roots NilradicalRoots;
  roots nonNilradicalRoots;
  roots ChosenPositiveKroots;
  roots nonLNonSingularsAleviatedByChosenPosKRoots;
  roots PositiveKroots;
  roots nonPositiveKRoots;
  roots nonKRoots;
  roots BKSingularGmodLRoots;
  roots nonBKSingularGmodLRoots;
  roots nonAlphas;
  roots nonBetas;
  roots nonLRoots;
  roots nonLNonSingularRoots;
  roots nonLNonSingularRootsInNeedOfPosKroots;
//  roots nonBKsingularRoots;
  //roots UndecidedRoots;
  coneRelation PartialRelation;
  MatrixLargeRational theScalarProducts;
  bool StateIsInternallyPossible;
  bool StateIsPossible;
  bool StateIsComplete;
  bool InternalStateIsComputed;
  //bool StateIsDubious;
  List<int> PossibleChildStates;
  List<int> CompleteChildStates;
  int NumImpossibleChildren;
  int activeChild;
  minimalRelationsProverStates* owner;
  void ReduceMemoryUse();
  bool IsSeparatingCones(root& input, bool& oneBetaIsPositive, WeylGroup& theWeyl);
  bool StateAllowsPositiveKChoice(root& theCandidate, root& theNonSingularRoot, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void SortAlphasAndBetas(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void ComputeIsPossible(minimalRelationsProverStates& theOwner);
  static void GetNumberScalarProductsData(root& input, roots& theRoots, bool& isLong, int& NumLongValue, int& NumMixedValue,  int& NumShortValue, int& NumMinusLongValue, int& NumMinusMixedValue,  int& NumMinusShortValue, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  static bool Root1IsGreaterThanRoot2(int index1, int index2, roots& setWeBelongTo, roots& setOtherSet, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, roots& theAlphas, roots& theBetas, MatrixLargeRational& output);
  bool ComputeStateReturnFalseIfDubious(minimalRelationsProverStates& owner, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS);
  bool CanBeShortened(coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas, SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS);
  bool SumWithNoPosRootIsARoot(root& input, WeylGroup& theWeyl);
  bool IsBKSingularImplied(root& input, WeylGroup& theWeyl);
  void Assign(const minimalRelationsProverState& right);
  void initFromParent(const minimalRelationsProverState& right);
  bool RootIsGoodForPreferredSimpleRoot(root& input, int preferredIndex, bool& GoodForAlpha, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& AlphasMinusBetas);
  void GetPossibleAlphasAndBetas(roots& outputAlphas, roots& outputBetas, WeylGroup& theWeyl);
  bool RootIsGoodForProblematicIndex(root& input, int problemIndex, bool AddingAlphas, bool NeedPositiveContribution, roots& theDualBasis, WeylGroup& theWeyl);
  bool FindBetaWithoutTwoAlphas(root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl);
  bool ComputeCommonSenseImplicationsReturnFalseIfContradiction(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  bool SatisfyNonLnonBKSingularRoots(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  bool IsAGoodPosRootsKChoice(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void MakeAlphaBetaMatrix(MatrixLargeRational& output);
  void operator=(const minimalRelationsProverState& right){this->Assign(right); }
  minimalRelationsProverState();
  void MakeProgressReportCanBeShortened(int checked, int outOf, GlobalVariables& theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
};

class minimalRelationsProverStates: public List<minimalRelationsProverState>
{
public:
  std::string DebugString;
  static int ProblemCounter;
  int MinNumDifferentBetas;
  roots PreferredDualBasis;
  roots PreferredDualBasisEpsilonCoords;
  //int RecursionDepth;
  List<int> theIndexStack;
  rootSubalgebra isomorphismComputer;
  MatrixLargeRational invertedCartan;
  WeylGroup theWeylGroup;
  rootSubalgebra theK;
  ReflectionSubgroupWeylGroup theIsos;
  rootsCollection PrecomputedIsoDomains;
  rootsCollection PrecomputedIsoRanges;
  int sizeByLastSave;
  int sizeByLastPurge;
  int NumFullyComputed;
  std::fstream theFileHeader;
  std::fstream theFileBody;
  std::string FileHeaderString;
  std::string FileBodyString;
  bool flagAssumeGlobalMinimalityRHS;
  bool flagComputationIsInitialized;
  bool flagSearchForOptimalSeparatingRoot;
  bool ExtendToIsomorphismRootSystem(minimalRelationsProverState& theState, int indexOther, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl);
  bool flagAnErrorHasOccurredTimeToPanic;
  void PrepareKIsos();
  void initShared(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ElementToString(std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void GetIsoTypicComponents(roots& theRoots, roots& theOtherTypeRoots, permutation& outputComponents, minimalRelationsProverState& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ComputeDebugString(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables){ this->ElementToString(this->DebugString, theWeyl, TheGlobalVariables); };
  bool GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive(int index, root& outputNormal, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ComputePreferredDualBasis(char WeylLetter, int theDimension, GlobalVariables& TheGlobalVariables);
  bool AddOnTopNoRepetition(int ParentIndex, minimalRelationsProverState& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ComputeLastStackIndex(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void GenerateStartingState(Parser& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension);
  void RecursionStep(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void TheFullRecursion(WeylGroup& theWeyl, GlobalVariables& theGlobalVariables);
  void ExtensionStep(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,  minimalRelationsProverState& newState);
  void BranchByAddingKRoots(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  static int CountNumSeparatingNormals(roots& theAlphas, roots& theBetas, WeylGroup& theWeyl);
  void ReduceMemoryUse(GlobalVariables& theGlobalVariables);
  void TestAddingExtraRoot(int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot, bool AddAlpha, int indexAddedRoot, root& normalSeparatingConesOneBetaPositive, bool oneBetaIsPositive);
  bool GetNormalSeparatingConesFromPreferredBasis(int theIndex, List<root>& inputPreferredBasis, root& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool& oneBetaIsPositive );
  static bool GetSeparatingRootIfExistsFromSet(roots* choicePreferrence, int* choiceIndex, roots& ConeOneStrictlyPositive, roots& ConeNonNegative, root& output, WeylGroup& TheWeyl, GlobalVariables& TheGlobalVariables, List<root>& theNormalCandidates);
  void InvokeExtensionOfState(int index, int UpperLimitChildren, bool oneBetaIsPositive, root& NormalSeparatingCones, bool addFirstAlpha, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void RemoveDoubt(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  bool StateIsEqualTo(minimalRelationsProverState& theState, int IndexOther, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void MakeProgressReportStack(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void MakeProgressReportIsos(int progress, int numSearchedWithinState, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void MakeProgressReportCurrentState(int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void MakeProgressReportChildStates(int numSearched, int outOf, int NewFound, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  bool CheckConsistencyOfTree();
  minimalRelationsProverStates()
  { this->flagComputationIsInitialized=false;
    this->MinNumDifferentBetas=-1;
    this->flagSearchForOptimalSeparatingRoot=true;
    this->sizeByLastSave=0;
    this->sizeByLastPurge=0;
    this->NumFullyComputed=0;
  }
  void ReadFromFile(std::fstream &inputHeader, std::fstream& inputBody, GlobalVariables* theGlobalVariables);
  void WriteToFileAppend(std::fstream& outputHeader, std::fstream& outputBody, GlobalVariables* theGlobalVariables);
  void WriteToFileAppend(GlobalVariables* theGlobalVariables);
  void ReadFromFile (GlobalVariables* theGlobalVariables);
  void PurgeImpossibleStates();
};

class DyckPaths;
class DyckPath
{
public:
  std::string DebugString;
  void ComputeDebugString(DyckPaths& owner, bool useHtml, bool WithDetail){this->ElementToString(DebugString, owner, useHtml, WithDetail); };
  void ElementToString(std::string& output, DyckPaths& owner, bool useHtml, bool WithDetail);
  ListLight<int> thePathNodes;
  ListLight<int> thePathEdgesTaken;
  void Assign(const DyckPath& other);
  bool IamComplete(DyckPaths& owner);
  void operator=(const DyckPath& other){this->Assign(other); }
};

class DyckPaths: public List<DyckPath>
{
public:
  std::string DebugString;
  void ComputeDebugString(bool useHtml){this->ElementToString(DebugString, useHtml); }
  void ElementToString(std::string& output, bool useHtml);
  int NumCompletePaths;
  WeylGroup AmbientWeyl;
  Selection AllowedEndNodes;
  List<List<int> > pathGraphPairsOfNodes;
  List<List<int> > pathGraphEdgeLabels;
  roots startingRoots;
  HashedList<root> PositiveRoots;
  int LastNonExploredIndex;
  List<int> GoodPaths;
  void ComputeGoodPathsExcludeTrivialPath();
  void GenerateAllDyckPathsTypesABC();
  void initPathGraphTypesABC();
  void GenerateAllDyckPathsTypesABCRecursive();
  bool SimpleRootAllowedToBeAddedTypesABC(int simpleRootIndex, root& output);
  bool SimpleRootAllowedToBeSubtractedTypesABC(int simpleRootIndex, root& output);
};

class ElementWeylAlgebra
{
private:
  //the standard order is as follows. First come the variables, then the differential operators, i.e. for 2 variables the order is x_1 x_2 \partial_{x_1}\partial_{x_2}
  PolynomialRationalCoeff StandardOrder;
public:
  std::string DebugString;
  void ComputeDebugString(bool useBeginEqnArray, bool useXYs){this->ElementToString(this->DebugString, useXYs, true, useBeginEqnArray); }
  void ElementToString(std::string& output, List<std::string>& alphabet, bool useLatex, bool useBeginEqnArray);
  void ElementToString(std::string& output, bool useXYs, bool useLatex, bool useBeginEqnArray);
  std::string ElementToString(bool useLatex){std::string tempS; this->ElementToString(tempS, false, useLatex, false); return tempS;}
  //NumVariables must equal 2*StandardOrder.NumVars
  int NumVariables;
  void MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars);
  void MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void Makexixj(int i, int j, int NumVars);
  void Makexi(int i, int NumVars);
  void Makedi(int i, int NumVars);
  void Makedidj(int i, int j, int NumVars);
  void Makexidj(int i, int j, int NumVars);
  void Nullify(int NumVars);
  void ActOnPolynomial(PolynomialRationalCoeff& thePoly);
  void GetStandardOrder(PolynomialRationalCoeff& output){output.Assign(this->StandardOrder);}
  void SetNumVariablesPreserveExistingOnes(int newNumVars);
  void TimesConstant(Rational& theConstant){ this->StandardOrder.TimesConstant(theConstant);}
  void MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
  void MultiplyOnTheRight(const ElementWeylAlgebra& standsOnTheRight);
  void LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
  void LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report);
  void LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report);
  void LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables);
  void Assign(const ElementWeylAlgebra& other){ this->StandardOrder.Assign(other.StandardOrder); this->NumVariables=other.NumVariables;}
  void AssignPolynomial(const Polynomial<Rational>& thePoly){ this->StandardOrder.Assign(thePoly); this->StandardOrder.IncreaseNumVariables(thePoly.NumVars); this->NumVariables=thePoly.NumVars;}
  void Subtract(const ElementWeylAlgebra& other){ this->StandardOrder.Subtract(other.StandardOrder);}
  void MakeConst(int NumVars, const Rational& theConst);
  void SubstitutionTreatPartialsAndVarsAsIndependent
  (PolynomialsRationalCoeff& theSub)
  ;
  void RaiseToPower(int thePower);
  void Add(const ElementWeylAlgebra& other){ this->StandardOrder.AddPolynomial(other.StandardOrder);}
  void MultiplyTwoMonomials(Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput);
  ElementWeylAlgebra(){ this->NumVariables=0; }
  void operator=(const std::string& input);
  void operator+=(const ElementWeylAlgebra& other){this->Add(other);}
  inline void operator*=(const ElementWeylAlgebra& other){ this->MultiplyOnTheRight(other);}
  void operator-=(const ElementWeylAlgebra& other){this->Subtract(other);}
  bool IsLetter(char theLetter);
  bool IsIndex(char theIndex);
  bool IsNumber(char theNumber);
};

class IrreducibleFiniteDimensionalModule
{
  public:
  DyckPaths thePaths; //the Weyl group inside the Dyck paths is assumed to parametrize the module.
  MatrixLargeRational thePathMatrix;
  MatrixLargeRational theMatrix;
  roots theColumnsOfTheMatrix;
  void InitAndPrepareTheChambersForComputation(int IndexWeyl, CombinatorialChamberContainer& theChambers, GlobalVariables& theGlobalVariables);
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(this->DebugString); }
  void ElementToString(std::string& output);
};

class DrawTextOperation
{
public:
  double X1; double Y1; std::string theText; int ColorIndex; int fontSize; int TextStyle;
  void init(double x1, double y1, const std::string& inputText, int color, int theFontSize, int theTextStyle){ this->X1=x1; this->Y1=y1; this->theText=inputText; this->ColorIndex=color; this->fontSize=theFontSize; this->TextStyle=theTextStyle;}
  void operator=(const DrawTextOperation& other){ this->X1=other.X1; this->Y1=other.Y1; this->theText=other.theText; this->ColorIndex=other.ColorIndex; this->fontSize=other.fontSize; this->TextStyle=other.TextStyle; }
};

class DrawLineOperation
{
public:
  double X1;
  double Y1;
  double X2;
  double Y2;
  int thePenStyle;
  int ColorIndex;
  inline void init(double x1, double y1, double x2, double y2, unsigned long PenStyle, int colorIndex){ this->X1=x1; this->Y1= y1; this->X2=x2; this->Y2=y2; this->thePenStyle=PenStyle; this->ColorIndex= colorIndex;}
  void operator=(const DrawLineOperation& other){ this->X1=other.X1; this->Y1=other.Y1; this->X2=other.X2; this->Y2=other.Y2; this->thePenStyle=other.thePenStyle; this->ColorIndex=other.ColorIndex;}
};

class DrawLineBetweenTwoRootsOperation
{
public:
  Vector<double> v1;
  Vector<double> v2;
  int thePenStyle;
  int ColorIndex;
  double precomputedX1, precomputedY1, precomputedX2, precomputedY2;
  void init(const root& input1, const root& input2, unsigned long PenStyle, int colorIndex)
  { assert(input1.size==input2.size);
    int theDimension=input1.size;
    this->v1.SetSize(theDimension);
    this->v2.SetSize(theDimension);
    for (int i=0; i<theDimension; i++)
    { this->v1[i]=input1.TheObjects[i].DoubleValue();
      this->v2[i]=input2.TheObjects[i].DoubleValue();
    }
    this->thePenStyle=PenStyle;
    this->ColorIndex=colorIndex;
  }
  void operator=(const DrawLineBetweenTwoRootsOperation& other)
  { this->v1=other.v1;
    this->v2=other.v2;
    this->thePenStyle=other.thePenStyle;
    this->ColorIndex=other.ColorIndex;
    this->precomputedX1=other.precomputedX1;
    this->precomputedY1=other.precomputedY1;
    this->precomputedX2=other.precomputedX2;
    this->precomputedY2=other.precomputedY2;
  }
};

class DrawTextAtVectorOperation
{
public:
  Vector<double> theVector;
  std::string theText;
  int ColorIndex;
  int fontSize;
  int TextStyle;
  double precomputedX, precomputedY;
  void init(const root& input, const std::string& inputText, int colorIndex, int theFontSize, int theTextStyle)
  { this->theVector.SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->theVector[i]=input.TheObjects[i].DoubleValue();
    this->ColorIndex=colorIndex;
    this->theText=inputText;
    this->fontSize=theFontSize;
    this->TextStyle=theTextStyle;
  }
  void operator=(const DrawTextAtVectorOperation& other)
  { this->theVector=other.theVector;
    this->ColorIndex=other.ColorIndex;
    this->theText=other.theText;
    this->fontSize=other.fontSize;
    this->TextStyle=other.TextStyle;
    this->precomputedX=other.precomputedX;
    this->precomputedY=other.precomputedY;
  }
};

class DrawCircleAtVectorOperation
{
public:
  Vector<double> theVector;
  int theMultiplicity;
  double radius;
  int ColorIndex;
  int thePenStyle;
  double precomputedX, precomputedY;
  void init(const root& input, double theRadius, unsigned long thePenStylE, int colorIndex)
  { this->theVector.SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->theVector[i]=input.TheObjects[i].DoubleValue();
    this->ColorIndex=colorIndex;
    this->thePenStyle=thePenStylE;
    this->radius=theRadius;
    this->theMultiplicity=1;
  }
  void operator=(const DrawCircleAtVectorOperation& other)
  { this->theVector=other.theVector;
    this->ColorIndex=other.ColorIndex;
    this->thePenStyle=other.thePenStyle;
    this->radius=other.radius;
    this->precomputedX=other.precomputedX;
    this->precomputedY=other.precomputedY;
    this->theMultiplicity=other.theMultiplicity;
  }
};

class DrawOperations
{
private:
  void changeBasisPreserveAngles(double newX, double newY, GlobalVariables& theGlobalVariables);
public:
  List<int> IndexNthDrawOperation;
  List<int> TypeNthDrawOperation;
  List<DrawTextOperation> theDrawTextOperations;
  List<DrawLineOperation> theDrawLineOperations;
  List<DrawLineBetweenTwoRootsOperation> theDrawLineBetweenTwoRootsOperations;
  List<DrawTextAtVectorOperation> theDrawTextAtVectorOperations;
  List<DrawCircleAtVectorOperation> theDrawCircleAtVectorOperations;
  List<List<double> > ProjectionsEiVectors;
  List<Vectors<double> > BasisProjectionPlane;
  static const int GraphicsUnitDefault=150;
  int SelectedCircleMinus2noneMinus1Center; //-2= none, -1=center of coordinate system, nonnegative integers= selectedindex
  Vectors<double> BasisToDrawCirclesAt;
  Matrix<double> theBilinearForm;
  double ClickToleranceX;
  double ClickToleranceY;
  List<double> centerX;
  List<double> centerY;
  List<double> GraphicsUnit;
  bool flagRotatingPreservingAngles;
  bool flagAnimatingMovingCoordSystem;
  bool flagIsPausedWhileAnimating;
  int SelectedPlane;
  std::string DebugString;
  int indexStartingModifiableTextCommands;
  void (*specialOperationsOnBasisChange)(DrawOperations& theOps, GlobalVariables& theGlobalVariables);
  static void projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps, GlobalVariables& theGlobalVariables);
  void operator=(const DrawOperations& other)
  { this->indexStartingModifiableTextCommands=other.indexStartingModifiableTextCommands;
    this->specialOperationsOnBasisChange=other.specialOperationsOnBasisChange;
    this->IndexNthDrawOperation=other.IndexNthDrawOperation;
    this->TypeNthDrawOperation=other.TypeNthDrawOperation;
    this->theDrawTextOperations=other.theDrawTextOperations;
    this->theDrawLineOperations=other.theDrawLineOperations;
    this->theDrawLineBetweenTwoRootsOperations=other.theDrawLineBetweenTwoRootsOperations;
    this->theDrawTextAtVectorOperations=other.theDrawTextAtVectorOperations;
    this->theDrawCircleAtVectorOperations=other.theDrawCircleAtVectorOperations;
    this->ProjectionsEiVectors=other.ProjectionsEiVectors;
    this->BasisProjectionPlane=other.BasisProjectionPlane;
    this->SelectedCircleMinus2noneMinus1Center=other.SelectedCircleMinus2noneMinus1Center;
    this->BasisToDrawCirclesAt=other.BasisToDrawCirclesAt;
    this->theBilinearForm=other.theBilinearForm;
    this->ClickToleranceX=other.ClickToleranceX;
    this->ClickToleranceY=other.ClickToleranceY;
    this->centerX=other.centerX;
    this->centerY=other.centerY;
    this->GraphicsUnit=other.GraphicsUnit;
    this->flagRotatingPreservingAngles=other.flagRotatingPreservingAngles;
    this->flagAnimatingMovingCoordSystem=other.flagAnimatingMovingCoordSystem;
    this->flagIsPausedWhileAnimating=other.flagIsPausedWhileAnimating;
    this->SelectedPlane=other.SelectedPlane;
    this->DebugString=other.DebugString;
  }
  void MakeMeAStandardBasis(int theDim);
  void operator+=(const DrawOperations& other);
  void initDimensions
  (Matrix<double>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
  { this->theBilinearForm=bilinearForm;
    this->BasisToDrawCirclesAt=draggableBasis;
    this->BasisProjectionPlane.initFillInObject(NumAnimationFrames, startingPlane);
    this->centerX.initFillInObject(NumAnimationFrames, 300);
    this->centerY.initFillInObject(NumAnimationFrames, 300);
    this->GraphicsUnit.initFillInObject(NumAnimationFrames, DrawOperations::GraphicsUnitDefault);
    this->ComputeProjectionsEiVectors();
  }
  void initDimensions
  (MatrixLargeRational& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
  { Matrix<double> tempMat;
    tempMat.init(bilinearForm.NumRows, bilinearForm.NumCols);
    for (int i=0; i<bilinearForm.NumRows; i++)
      for (int j=0; j<bilinearForm.NumCols; j++)
        tempMat.elements[i][j]=bilinearForm.elements[i][j].DoubleValue();
    this->initDimensions(tempMat, draggableBasis, startingPlane, NumAnimationFrames);
  }
  void initDimensions(int theDim, int numAnimationFrames);
  int GetDimFirstDimensionDependentOperation();
  int GetDimFromBilinearForm();
  inline void GetCoordsDrawingComputeAll(Vector<double>& input, double& X1, double& Y1)
  { X1=this->theBilinearForm.ScalarProduct(input, this->BasisProjectionPlane[this->SelectedPlane][0]);
    Y1=this->theBilinearForm.ScalarProduct(input, this->BasisProjectionPlane[this->SelectedPlane][1]);
    X1=X1*this->GraphicsUnit[this->SelectedPlane] +this->centerX[this->SelectedPlane];
    Y1=Y1*this->GraphicsUnit[this->SelectedPlane] +this->centerY[this->SelectedPlane] ;
  }
  inline void GetCoordsForDrawingProjectionsComputed(Vector<double>& input, double& X1, double& Y1)
  { X1=0; Y1=0;
    for (int j=0; j<input.size; j++)
    { X1+=this->ProjectionsEiVectors[j][0]*input[j];
      Y1+=this->ProjectionsEiVectors[j][1]*input[j];
    }
    X1=X1*this->GraphicsUnit[this->SelectedPlane] +this->centerX[this->SelectedPlane];
    Y1=Y1*this->GraphicsUnit[this->SelectedPlane]+this->centerY[this->SelectedPlane];
  }
  inline void GetCoordsForDrawingProjectionsComputed(Vector<double>& input1, Vector<double>& input2, double& X1, double& Y1, double& X2, double& Y2)
  { X1=0; X2=0; Y1=0; Y2=0;
    for (int j=0; j<input1.size; j++)
    { X1+=this->ProjectionsEiVectors[j][0]*input1[j];
      Y1+=this->ProjectionsEiVectors[j][1]*input1[j];
      X2+=this->ProjectionsEiVectors[j][0]*input2[j];
      Y2+=this->ProjectionsEiVectors[j][1]*input2[j];
    }
    X1=X1*this->GraphicsUnit[this->SelectedPlane]+this->centerX[this->SelectedPlane];
    X2=X2*this->GraphicsUnit[this->SelectedPlane]+this->centerX[this->SelectedPlane];
    Y1=Y1*this->GraphicsUnit[this->SelectedPlane]+this->centerY[this->SelectedPlane];
    Y2=Y2*this->GraphicsUnit[this->SelectedPlane]+this->centerY[this->SelectedPlane];
  }
  void EnsureProperInitialization();
  bool AreWithinClickTolerance(double x1, double y1, double x2, double y2)
  { x1-=x2;
    y1-=y2;
    if (x1<0)
      x1=-x1;
    if (y1<0)
      y1=-y1;
    return x1<=this->ClickToleranceX && y1<=this->ClickToleranceY;
  };
  bool mouseMoveRedraw(int X, int Y, GlobalVariables& theGlobalVariables)
  { if (this->SelectedCircleMinus2noneMinus1Center==-2)
      return false;
    if (this->SelectedCircleMinus2noneMinus1Center==-1)
    { this->centerX[this->SelectedPlane]=X;
      this->centerY[this->SelectedPlane]=Y;
      return true;
    }
    if (this->SelectedCircleMinus2noneMinus1Center>=0)
    { if (this->flagRotatingPreservingAngles)
      { this->changeBasisPreserveAngles((double) X , (double) Y, theGlobalVariables);
        return true;
      }
    }
    return false;
  //  this->draw();
  }
  void click(double x, double y);
  void drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawLineBetweenTwoVectorsBuffer(const root& vector1, const root& vector2, unsigned long thePenStyle, int ColorIndex);
  void drawTextAtVectorBuffer(const root& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawCircleAtVectorBuffer(const root& input, double radius, unsigned long thePenStyle, int theColor);
  double getAngleFromXandY(double x, double y, double neighborX, double neighborY);
  void ScaleToUnitLength(Vector<double>& theRoot)
  { double theLength=this->theBilinearForm.ScalarProduct(theRoot, theRoot);
    theLength=sqrt(theLength);
    theRoot/=theLength;
  }
  void RotateOutOfPlane
  (std::stringstream& logger, Vector<double>& input, Vector<double>& output,
   Vector<double>& orthoBasis1, Vector<double>& orthoBasis2, double oldTanSquared, double newTanSquared)
   ;
  void ModifyToOrthonormalNoShiftSecond
(Vector<double>& root1, Vector<double>& root2)
;
  void ComputeXYsFromProjectionsEisAndGraphicsUnit()
  ;
  void ComputeProjectionsEiVectors();
  DrawOperations()
  { this->SelectedPlane=0;
    this->initDimensions(2, 2);
    this->flagAnimatingMovingCoordSystem=false;
    this->specialOperationsOnBasisChange=0;
    this->indexStartingModifiableTextCommands=0;
  }
  void init()
  { this->IndexNthDrawOperation.MakeActualSizeAtLeastExpandOnTop(10000);
    this->TypeNthDrawOperation.MakeActualSizeAtLeastExpandOnTop(10000);
    this->theDrawLineBetweenTwoRootsOperations.MakeActualSizeAtLeastExpandOnTop(10000);
    this->theDrawTextAtVectorOperations.MakeActualSizeAtLeastExpandOnTop(15);
    this->theDrawCircleAtVectorOperations.MakeActualSizeAtLeastExpandOnTop(280);
    this->IndexNthDrawOperation.size=0;
    this->TypeNthDrawOperation.size=0;
    this->theDrawTextOperations.size=0;
    this->theDrawLineOperations.size=0;
    this->theDrawLineBetweenTwoRootsOperations.size=0;
    this->theDrawTextAtVectorOperations.size=0;
    this->theDrawCircleAtVectorOperations.size=0;
    this->centerX.initFillInObject(this->BasisProjectionPlane.size, 300);
    this->centerY.initFillInObject(this->BasisProjectionPlane.size, 300);
    this->GraphicsUnit.initFillInObject(this->BasisProjectionPlane.size, DrawOperations::GraphicsUnitDefault);
    this->ClickToleranceX=5;
    this->ClickToleranceY=5;
    this->SelectedCircleMinus2noneMinus1Center=-2;
    this->SelectedPlane=0;
    this->flagRotatingPreservingAngles=true;
    this->flagAnimatingMovingCoordSystem=false;
    this->flagIsPausedWhileAnimating=false;
  }
  enum DrawOperationType{ typeDrawLine, typeDrawText, typeDrawLineBetweenTwoVectors, typeDrawTextAtVector, typeDrawCircleAtVector,};
};

class VirtualDrawOp
{
public:
  int theVirtualOp;
  int indexPhysicalFrame;
  int indexPhysicalDrawOp;
  int selectedPlaneInPhysicalDrawOp;
  std::string ElementToString();
  void operator=(const VirtualDrawOp& other)
  { this->theVirtualOp=other.theVirtualOp;
    this->indexPhysicalFrame=other.indexPhysicalFrame;
    this->indexPhysicalDrawOp=other.indexPhysicalDrawOp;
    this->selectedPlaneInPhysicalDrawOp=other.selectedPlaneInPhysicalDrawOp;
  }
};

class AnimationBuffer
{
public:
  DrawOperations stillFrame;
  List<DrawOperations> thePhysicalDrawOps;
  List<VirtualDrawOp> theVirtualOpS;
  bool flagAnimating;
  bool flagIsPausedWhileAnimating;
  int indexVirtualOp;
  std::string ElementToString();
  bool IncrementOpReturnNeedsRedraw();
  int GetIndexCurrentPhysicalFrame();
  void DrawNoInit(DrawingVariables& theDrawingVariables, GlobalVariables& theGlobalVariables);
  AnimationBuffer();
  DrawOperations& GetCurrentDrawOps();
  DrawOperations& GetLastDrawOps();
  int GetIndexCurrentDrawOps();
  int GetNumPhysicalFramesNoStillFrame();
  void MakeZero();
  void operator+=(const AnimationBuffer& other);
  void operator+=(const DrawOperations& other);
  void operator=(const AnimationBuffer& other);
  void AddPause(int numFrames);
  void AddCloneLastFrameAppendOperations(const DrawOperations& other);
  void AddFrameShift(int numFrames);
  enum{typeDrawOps, typePause, typeClearScreen, typeCloneLastFrameAddOps,};
};

class DrawingVariables
{
private:
  drawLineFunction theDrawLineFunction;
  drawTextFunction theDrawTextFunction;
  drawCircleFunction theDrawCircleFunction;
  drawClearScreenFunction theDrawClearScreenFunction;
public:
  enum PenStyles
  { PenStyleInvisible, PenStyleDashed, PenStyleDotted, PenStyleNormal, PenStyleZeroChamber, PenStylePermanentlyZeroChamber, PenStyleLinkToOriginZeroChamber, PenStyleLinkToOrigin, PenStyleLinkToOriginPermanentlyZeroChamber };
  enum TextStyles
  { TextStyleNormal, TextStyleInvisible, TextStyleChamber, TextStyleZeroChamber, TextStylePermanentlyZeroChamber };
  MutexWrapper LockedWhileDrawing;
  int ColorDashes;
  bool flagLaTeXDraw;
  bool flag2DprojectionDraw;
  bool flagDisplayingCreationNumbersInsteadOfDisplayNumbers;
  bool flagDrawChamberIndices;
  bool flagDrawingInvisibles;
  bool flagDrawingLinkToOrigin;
  int Selected;
  int NumHtmlGraphics;
  int textX;
  int textY;
  int fontSizeNormal;
  int fontSizeSubscript;
  int ColorTextDefault;
  int ColorTextZeroChamber;
  int ColorTextPermanentlyZeroChamber;
  int ColorChamberIndicator;
  int ColorWeylChamberWalls;
  int DefaultHtmlWidth;
  int DefaultHtmlHeight;
  void initDrawingVariables(int cX1, int cY1);
  DrawingVariables(int cx, int cy){this->initDrawingVariables(cx, cy); };
  DrawingVariables(){this->initDrawingVariables(100, 100); };
  void SetDrawLineFunction(drawLineFunction theFunction){ this->theDrawLineFunction=theFunction; }
  void SetDrawTextFunction(drawTextFunction theFunction){ this->theDrawTextFunction=theFunction; }
  void SetDrawCircleFunction(drawCircleFunction theFunction){ this->theDrawCircleFunction=theFunction; }
  void SetDrawClearFunction(drawClearScreenFunction theFunction){ this->theDrawClearScreenFunction=theFunction; }
  int GetColorFromChamberIndex(int index, std::fstream* LaTexOutput);
  static void GetCoordsForDrawing(DrawingVariables& TDV, root& r, double& x, double& y);
  static void ProjectOnToHyperPlaneGraphics(root& input, root& output, roots& directions);
  std::string GetColorHtmlFromColorIndex(int colorIndex);
  DrawOperations theBuffer;
  inline int GetActualPenStyleFromFlagsAnd(int inputPenStyle);
  inline int GetActualTextStyleFromFlagsAnd(int inputTextStyle);
  std::string GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension);
  void drawString(DrawElementInputOutput& theDrawData, const std::string& input, int theFontSize, int theTextStyle);
  void drawCoordSystemDirectlly(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile);
  void drawCoordSystemBuffer(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile);
  void drawLineDirectly(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
  void drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
  void drawTextDirectly(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile);
  void drawBufferNoIniT
(DrawOperations& theOps)
;
  void drawBufferNoIniT(){this->drawBufferNoIniT(this->theBuffer);}
  //if the LatexOutFile is zero then the procedure defaults to the screen
  void drawLineBufferOld(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex, std::fstream* LatexOutFile);
  inline void drawLineBetweenTwoVectorsBuffer(const root& r1, const root& r2, int PenStyle, int PenColor){this->theBuffer.drawLineBetweenTwoVectorsBuffer(r1, r2, PenStyle, PenColor);}
  void drawTextAtVectorBuffer(const root& point, const std::string& inputText, int textColor, int theTextStyle, std::fstream* LatexOutFile);
  void drawCircleAtVectorBuffer
  (const root& point, double radius, unsigned long thePenStyle, int theColor)
;
  void operator=(const DrawingVariables& other)
  { this->theDrawLineFunction=other.theDrawLineFunction;
    this->theDrawTextFunction=other.theDrawTextFunction;
  }
};

struct CGI
{
public:
  static std::stringstream outputStream;
  static int GlobalFormulaIdentifier;
  static int numLinesAll;
  static int numRegularLines;
  static int numDashedLines;
  static int numDottedLines;
  static int shiftX;
  static int shiftY;
  static int scale;
  static void outputLineJavaScriptSpecific(const std::string& lineTypeName, int theDimension, std::string& stringColor, int& lineCounter);
  static void PrepareOutputLineJavaScriptSpecific(const std::string& lineTypeName, int numberLines);
  static void CivilizedStringTranslationFromVPold(std::string& input, std::string& output);
  static void CivilizedStringTranslationFromCGI(std::string& input, std::string& output);
  static std::string UnCivilizeStringCGI(const std::string& input);
  static void ReplaceEqualitiesAndAmpersantsBySpaces(std::string& inputOutput);
  static bool AttemptToCivilize(std::string& readAhead, std::stringstream& out);
  static void MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank);
  static void rootSubalgebrasToHtml(rootSubalgebras& input, std::fstream& output);
  static void WeylGroupToHtml(WeylGroup&input, std::string& path);
  static bool GetHtmlStringSafeishReturnFalseIfIdentical(const std::string& input, std::string& output);
  static void TransormStringToHtmlSafeish(std::string& theString){std::string tempS; CGI::GetHtmlStringSafeishReturnFalseIfIdentical(theString, tempS); theString=tempS; }
  static std::string GetHtmlMathDivFromLatexFormulA(const std::string& input)
  { return CGI::GetHtmlMathFromLatexFormulA(input, "", "", true, false);
  }
  static std::string GetHtmlMathDivFromLatexAddBeginARCL(const std::string& input)
  { return  CGI:: GetHtmlMathFromLatexFormulA(input, "", "", true, true);
  }
  static std::string GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(const std::string& input)
  { return  CGI:: GetHtmlMathFromLatexFormulA(input, "", "", false, true);
  }
  static std::string GetHtmlMathSpanFromLatexFormula(const std::string& input)
  { return  CGI:: GetHtmlMathFromLatexFormulA(input, "", "", false, false);
  }
  static std::string GetHtmlMathFromLatexFormulA
  (const std::string& input, const std::string& prependString, const std::string& appendStringBeforeButton, bool useDiv, bool useBeginArrayRCL)
;
  static std::string GetStyleButtonLikeHtml(){return " style=\"background:none; border:0; text-decoration:underline; color:blue; cursor:pointer\" ";}
  static std::string GetHtmlButton
  (const std::string& buttonID, const std::string& theScript, const std::string& buttonText)
;
  static void rootSubalgebrasToHtml(GlobalVariables& theGlobalVariables, rootSubalgebras& input, std::string& path);
  static bool FileExists(const std::string& theFileName);
  static bool OpenFileCreateIfNotPresent(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary);
  static void clearDollarSigns(std::string& theString, std::string& output);
  static void subEqualitiesWithSimeq(std::string& theString, std::string& output);
  static void ChopCGIInputStringToMultipleStrings(const std::string& input, List<std::string>& output);
  static void ElementToStringTooltip(const std::string& input, const std::string& inputTooltip, std::string& output, bool useHtml);
  static std::string ElementToStringTooltip(const std::string& input, const std::string& inputTooltip, bool useHtml){ std::string result; CGI::ElementToStringTooltip(input, inputTooltip, result, useHtml); return result; };
  static std::string ElementToStringTooltip(const std::string& input, const std::string& inputTooltip){ return CGI::ElementToStringTooltip(input, inputTooltip, true); };
  static inline int RedGreenBlue(int r, int g, int b)
  { r=r%256;
    g=g%256;
    b=b%256;
    return r*65536+g*256+b;
  }
  static void FormatCPPSourceCode(const std::string& FileName);
  static void(*functionCGIServerIgnoreUserAbort)(void);
  static void SetCGIServerIgnoreUserAbort()
  { if (CGI::functionCGIServerIgnoreUserAbort!=0)
      CGI::functionCGIServerIgnoreUserAbort();
  }
  enum TheChoicesWeMake
  { choiceDefaultNeedComputation, choiceInitAndDisplayMainPage, choiceGenerateDynkinTables, choiceDisplayRootSApage, choiceGosl2, choiceExperiments
  };
};

class LittelmannPath
{
public:
  WeylGroup* owner;
  roots Waypoints;
  void MakeFromWeightInSimpleCoords
  (root& weightInSimpleCoords, WeylGroup& theOwner)
  ;
  void MakeFromWaypoints
  (roots& weightsInSimpleCoords, WeylGroup& theOwner)
  { this->owner=& theOwner;
    this->Waypoints=weightsInSimpleCoords;
    this->Simplify();
  }
  void ActByFalpha(int indexAlpha);
  void ActByEalpha(int indexAlpha);
//   List<Rational> Speeds;
  void operator+=(const LittelmannPath& other)
  { this->Waypoints.MakeActualSizeAtLeastExpandOnTop(this->Waypoints.size+other.Waypoints.size);
    root endPoint=*this->Waypoints.LastObject();
    for (int i=0; i<other.Waypoints.size; i++)
      this->Waypoints.AddOnTop(other.Waypoints[i]+endPoint);
  }
  std::string ElementToStringIndicesToCalculatorOutput
(LittelmannPath& inputStartingPath, List<int> & input)
  ;
  std::string ElementToStringOperatorSequenceStartingOnMe
(List<int> & input)
  ;
  bool GenerateOrbit
(List<LittelmannPath>& output, List<List<int> >& outputOperators, GlobalVariables& theGlobalVariables, int UpperBoundNumElts)
;
  bool MinimaAreIntegral();
  std::string ElementToString(bool useSimpleCoords=true)
  { if (this->Waypoints.size==0)
      return "0";
    std::stringstream out;
    for (int i=0; i<this->Waypoints.size; i++)
    { if (useSimpleCoords)
        out << this->Waypoints[i].ElementToString();
      else
        out << this->owner->GetFundamentalCoordinatesFromSimple(this->Waypoints[i]).ElementToString();
      if (i!=this->Waypoints.size-1)
        out << "->";
    }
    return out.str();
  }
  void Simplify();
  int HashFunction()const
  { return this->Waypoints.HashFunction();
  }
  bool IsEqualToZero()const
  { return this->Waypoints.size==0;
  }
  void operator=(const LittelmannPath& other)
  { this->Waypoints=other.Waypoints;
    this->owner=other.owner;
  }
  bool operator==(const LittelmannPath& other)const
  { return this->Waypoints==other.Waypoints;
  }
};

class ConeLatticeAndShiftMaxComputation
{ public:
  inline static const std::string GetXMLClassName(){ return "ConeLatticeAndShiftMaxComputation";}
  int numNonParaM;
  int numProcessedNonParam;
  List<ConeComplex> complexStartingPerRepresentative;
  List<ConeComplex> complexRefinedPerRepresentative;
  List<List<roots> > theMaximaCandidates;
  List<roots> startingLPtoMaximize;
  List<roots> finalMaxima;
  Lattice theStartingLattice;
  Lattice theFinalRougherLattice;
  root theStartingRepresentative;
  roots theFinalRepresentatives;
  List<ConeLatticeAndShift> theConesLargerDim;
  List<ConeLatticeAndShift> theConesSmallerDim;
  List<List<ConeComplex> > finalMaximaChambers;
  List<List<List<int> > > finalMaximaChambersIndicesMaxFunctions;
  List<bool> IsInfinity;
  roots LPtoMaximizeLargerDim;
  roots LPtoMaximizeSmallerDim;

  std::string ElementToString(PolynomialOutputFormat& theFormat);
  void init
  (root& theNEq, Cone& startingCone, Lattice& startingLattice, root& startingShift)
  ;
  void FindExtremaParametricStep1
    (Controller& thePauseController, bool assumeNewConesHaveSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
       ;
  void FindExtremaParametricStep2TrimChamberForMultOne
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
;
  void FindExtremaParametricStep3
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
;
  void FindExtremaParametricStep4
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
;
  void FindExtremaParametricStep5
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
;

  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
  ;
  bool ReadFromFile
(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
  ;
};

class PiecewiseQuasipolynomial
{
  public:
  //Note: PiecewiseQuasipolynomial assumes that its variable GlobalVariables* theBuffers
  // is always initialized with a non-zero object.
  //This is an engineering decision that yet has to be proven good: please follow the assumption,
  //but keep in mind that a better solution might exist, and be ready to switch.
  //Let the piecewise quasipolynomial be in n variables.
  //Then the theProjectivizedComplex is an n+1-dimensional complex,
  //which is the projectivization of the n-dim affine complex representing the
  //domain of the piecewise quasipoly.
  ConeComplex theProjectivizedComplex;
  List<QuasiPolynomial> theQPs;
  GlobalVariables* theBuffers;
  int NumVariables;
  void operator=(PiecewiseQuasipolynomial& other)
  { this->theBuffers=other.theBuffers;
    this->NumVariables=other.NumVariables;
    this->theQPs=other.theQPs;
    this->theProjectivizedComplex=other.theProjectivizedComplex;
  }
  std::string ElementToString(bool useLatex, bool useHtml);
  PiecewiseQuasipolynomial(){this->theBuffers=0;}
  PiecewiseQuasipolynomial(GlobalVariables& PermanentGlobalVariables){this->theBuffers=& PermanentGlobalVariables;}
  void DrawMe
 (DrawingVariables& theDrawingVars, int numLatticePointsPerDim, Cone* RestrictingChamber=0,
 root* distinguishedPoint=0)
  ;
  int GetNumVars(){return this->NumVariables;}
  inline void MakeCommonRefinement(const PiecewiseQuasipolynomial& other){ this->MakeCommonRefinement(other.theProjectivizedComplex);  }
  void MakeCommonRefinement(const ConeComplex& other);
  void TranslateArgument(root& translateToBeAddedToArgument, GlobalVariables& theGlobalVariables);
  bool MakeVPF
  (roots& theRoots, std::string& outputstring, GlobalVariables& theGlobalVariables)
  ;
  Rational Evaluate(const root& thePoint);
  Rational EvaluateInputProjectivized(const root& thePoint);
  void Nullify(int numVars, GlobalVariables& theGlobalVariables)
  { this->NumVariables=numVars;
    this->theProjectivizedComplex.init();
    this->theBuffers=& theGlobalVariables;
    this->theQPs.size=0;
  }
  void operator+=(const PiecewiseQuasipolynomial& other);
  void operator*=(const Rational& other);
  void operator=(const PiecewiseQuasipolynomial& other);
};

class charSSAlgMod;

template<class CoefficientType>
class MonomialChar
{
public:
  CoefficientType Coefficient;
  Vector<CoefficientType> weightFundamentalCoords;
  void AccountSingleWeight
(root& currentWeightSimpleCoords, root& otherHighestWeightSimpleCoords, WeylGroup& theWeyl,
 Rational& theMult, charSSAlgMod& outputAccum, Rational& finalCoeff
 )
  ;
  std::string TensorAndDecompose
(MonomialChar<CoefficientType>& other, SemisimpleLieAlgebra& owner, charSSAlgMod& output, GlobalVariables& theGlobalVariables)
  ;
  std::string ElementToString
  (const std::string& theVectorSpaceLetter, const std::string& theWeightLetter, bool useBrackets=false)
  ;
  inline int HashFunction()const
  { return weightFundamentalCoords.HashFunction();
  }
  inline bool operator==(const MonomialChar<CoefficientType>& other) const
  { return this->weightFundamentalCoords==other.weightFundamentalCoords;
  }
  inline bool operator>(const MonomialChar<CoefficientType>& other) const
  { return this->weightFundamentalCoords>other.weightFundamentalCoords;
  }
};

class charSSAlgMod : public HashedList<MonomialChar<Rational> >
{
  public:
  SemisimpleLieAlgebra* theBoss;
  void Nullify(SemisimpleLieAlgebra* owner)
  { this->ClearTheObjects();
    this->theBoss=owner;
  }
  bool IsEqualToZero()
  { return this->size==0;
  }
  charSSAlgMod()
  { this->theBoss=0;
  }
  std::string ElementToString
  (const std::string& theVectorSpaceLetter="V", const std::string& theWeightLetter="\\omega", bool useBrackets=false)
  ;
  std::string ElementToStringCharacter(List<root>& theWeights, List<Rational>& theMults);
  void MakeFromWeight
  (const Vector<Rational>& inputWeightSimpleCoords, SemisimpleLieAlgebra* owner)
  ;
  bool GetDominantCharacterWRTsubalgebra
 (charSSAlgMod& outputCharOwnerSetToZero, std::string& outputDetails,
  GlobalVariables& theGlobalVariables, int upperBoundNumDominantWeights)
;
  bool FreudenthalEvalMe
 (charSSAlgMod& outputCharOwnerSetToZero, std::string& outputDetails,
  GlobalVariables& theGlobalVariables, int upperBoundNumDominantWeights)
;
  bool DrawMeNoMults
(std::string& outputDetails, GlobalVariables& theGlobalVariables,
 DrawingVariables& theDrawingVars, int upperBoundWeights)
  ;
  bool DrawMeWithMults
(std::string& outputDetails, GlobalVariables& theGlobalVariables,
 DrawingVariables& theDrawingVars, int upperBoundWeights)
  ;
  bool SplitCharOverLevi
(std::string* Report, charSSAlgMod& output, root& parabolicSel, ReflectionSubgroupWeylGroup& outputWeylSub,
 GlobalVariables& theGlobalVariables)
     ;
  void MakeTrivial(SemisimpleLieAlgebra* owner);
  void operator+=(const charSSAlgMod& other);
  void operator+=(const MonomialChar<Rational>& other);
  void operator-=(const charSSAlgMod& other);
  void operator-=(const MonomialChar<Rational>& other);
  std::string MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables);
  std::string operator*=(const charSSAlgMod& other);
  std::string operator*=(const MonomialChar<Rational>& other);
};

class Parser;
class ParserNode
{
private:
  bool ConvertToNextType
(int GoalType, int GoalNumVariables, bool& ErrorHasOccured, GlobalVariables& theGlobalVariables)
;
public:
  std::string DebugString;
  std::string outputString;
  void ComputeDebugString(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat){ this->ElementToString(this->DebugString, theGlobalVariables, theFormat); }
  void ElementToString(std::string& output, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat);
  Parser* owner;
  int indexParentNode;
  int indexInOwner;
  int Operation; //using tokenTypes from class Parser
  int ExpressionType;
  int ErrorType;
  SemisimpleLieAlgebra* ContextLieAlgebra;
  int impliedNumVars;
  bool Evaluated;
  MemorySaving<PolynomialRationalCoeff> polyValue;
  MemorySaving<ElementWeylAlgebra> WeylAlgebraElement;
  MemorySaving<ElementUniversalEnveloping<PolynomialRationalCoeff> > UEElement;
  MemorySaving<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> > UEElementOrdered;
  MemorySaving<PolynomialRationalCoeff> polyBeingMappedTo;
  MemorySaving<ElementWeylAlgebra> weylEltBeingMappedTo;
  MemorySaving<RationalFunction> ratFunction;
  MemorySaving<Cone> theCone;
  MemorySaving<Lattice> theLattice;
  MemorySaving<QuasiPolynomial> theQP;
  MemorySaving<charSSAlgMod> theChar;
  MemorySaving<partFractions> thePFs;
  MemorySaving<PiecewiseQuasipolynomial> thePiecewiseQP;
  MemorySaving<AnimationBuffer> theAnimation;
  MemorySaving<LittelmannPath> theLittelmann;
  List<int> children;
  int intValue;
  Rational rationalValue;
  void operator=(const ParserNode& other);
  void Clear();
  int GetMaxImpliedNumVarsChildren();
  int GetStrongestExpressionChildrenConvertChildrenIfNeeded(GlobalVariables& theGlobalVariables);
  void ConvertChildrenAndMyselfToStrongestExpressionChildren(GlobalVariables& theGlobalVariables);
  void CopyValue(const ParserNode& other);
  bool ConvertToType
(int theType, int GoalNumVars, GlobalVariables& theGlobalVariables)
  ;
  bool ConvertChildrenToType(int theType, int GoalNumVars, GlobalVariables& theGlobalVariables);
  //the order of the types matters, they WILL be compared by numerical value!
  enum typeExpression{typeUndefined=0, typeIntegerOrIndex, typeRational, typeLieAlgebraElement, typePoly, typeRationalFunction, typeUEElementOrdered, //=6
  typeUEelement, typeWeylAlgebraElement, typeMapPolY, typeMapWeylAlgebra, typeString, typePDF, typeLattice, typeCone, //=14
  typeArray, typeQuasiPolynomial, typePartialFractions, //=17
  typeCharSSFDMod,
  typePiecewiseQP,
  typeAnimation,
  typeLittelman,
  typeFile, typeDots,
  typeError //typeError must ALWAYS have the highest numerical value!!!!!
  };
  enum typesErrors{errorNoError=0, errorDivisionByZero, errorDivisionByNonAllowedType, errorMultiplicationByNonAllowedTypes,
  errorUnknownOperation, errorOperationByUndefinedOrErrorType, errorProgramming, errorBadIndex, errorDunnoHowToDoOperation,
  errorWrongNumberOfArguments,//=9
  errorBadOrNoArgument, errorBadSyntax, errorBadSubstitution, errorConversionError, errorDimensionProblem,//=14
  errorImplicitRequirementNotSatisfied, errorBadFileFormat };
  void InitForAddition(GlobalVariables* theContext);
  void InitForMultiplication(GlobalVariables* theContext);
  std::string ElementToStringValueAndType
  (bool useHtml, GlobalVariables& theGlobalVariables, bool displayOutputString, PolynomialOutputFormat& theFormat)
  { return this->ElementToStringValueAndType(useHtml, 0, 3, theGlobalVariables, displayOutputString, theFormat);
  }
  std::string ElementToStringValueAndType
(bool useHtml, int RecursionDepth, int maxRecursionDepth, GlobalVariables& theGlobalVariables, bool displayOutputString, PolynomialOutputFormat& theFormat)
;
  std::string ElementToStringValueOnlY(bool useHtml, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
  { return this->ElementToStringValueOnlY(useHtml, 0, 2, theGlobalVariables, theFormat);
  }
  std::string ElementToStringValueOnlY
  (bool useHtml, int RecursionDepth, int maxRecursionDepth, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
;
  std::string ElementToStringErrorCode(bool useHtml);
  void TrimSubToMinNumVars(PolynomialsRationalCoeff& theSub, int theDimension);
  template <class CoefficientType>
bool GetRootRationalDontUseForFunctionArguments
(Vector<CoefficientType>& output, GlobalVariables& theGlobalVariables)
;
  template <class CoefficientType>
CoefficientType& GetElement();
  template <class CoefficientType>
bool GetListDontUseForFunctionArguments
(List<CoefficientType>& output, GlobalVariables& theGlobalVariables, int ExpressionType, int impliedNumVars)
;
bool GetRootRationalFromFunctionArguments
(//List<int>& argumentList,
 root& output, GlobalVariables& theGlobalVariables)
;
bool GetRootSRationalDontUseForFunctionArguments
(
 roots& output, int& outputDim, GlobalVariables& theGlobalVariables)
;
  bool GetRootInt(Vector<int>& output, GlobalVariables& theGlobalVariables);
  void CopyError(ParserNode& other) {this->ExpressionType=other.ExpressionType; this->ErrorType=other.ErrorType;}
  int SetError(int theError){this->ExpressionType=this->typeError; this->ErrorType=theError; return theError;}
  int CarryOutSubstitutionInMe(PolynomialsRationalCoeff& theSub, GlobalVariables& theGlobalVariables);
  void ReduceRatFunction();
  void EvaluateLieBracket(GlobalVariables& theGlobalVariables);
  void Evaluate(GlobalVariables& theGlobalVariables);
  int EvaluateTimes(GlobalVariables& theGlobalVariables);
  void EvaluateDivide(GlobalVariables& theGlobalVariables);
  void EvaluateOrder(GlobalVariables& theGlobalVariables);
  void CreateDefaultLatexAndPDFfromString
  (const std::string& theLatexFileString)
  ;
  void EvaluateInteger(GlobalVariables& theGlobalVariables);
  bool GetRootsEqualDimNoConversionNoEmptyArgument
(List<int>& theArgumentList, roots& output, int& outputDim)
;
  bool GetListRootsEqualSizeEqualDimNoConversionNoEmptyArgument
(List<int>& theArgumentList, List<roots>& output, int& outputRootsSize, int& outputDim,
 GlobalVariables& theGlobalVariables)
;
  static int EvaluateInvariantsExteriorPowerFundamentalRepsPlusTrivialReps
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateRepresentationFromHWFundCoords
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateLattice
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDrawWeightSupportWithMults
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateHWMTABilinearForm
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateHWTAABilinearForm
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateFreudenthal
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateHWV
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateIsInProperSubmoduleVermaModule
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateLittelmannPaths
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateSplitIrrepOverLeviParabolic
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateSplitCharOverLeviParabolic
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateMakeWeylFromParSel
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateLittelmannPathFromWayPoints
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAllLittelmannPaths
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateLittelmannEAlpha
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;

  static int EvaluateGetCoxeterBasis
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAnimationPause
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAnimationClearScreen
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateChar
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDrawRootSystemCoxeterPlaneRootSA
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAnimationDots
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAnimateRootSAs
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateMinusTransposeAuto
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateTransposeAntiAuto
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAnimationClonePreviousFrameDrawExtraLine
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateG2ParabolicSupport
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;

  static int EvaluateGroebner
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateG2InB3MultsParabolic
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateRelations
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateModOutRelsFromGmodKtimesVerma
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateCreateFromDirectionsAndSalamiSlice
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateParabolicWeylGroups
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateActByWeylAlgebraElement
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateParabolicWeylGroupsBruhatGraph
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateVPF
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;

  static int EvaluateLatticeImprecise
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateMakeCasimir
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDrawWeightSupport
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAdjointAction
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateWeylAction
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluateWeylAction(theNode, theArgumentList, theGlobalVariables, false, false, false);}
  static int EvaluateWeylRhoAction
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluateWeylAction(theNode, theArgumentList, theGlobalVariables, false, true, false);}
  static int EvaluateWeylMinusRhoAction
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluateWeylAction(theNode, theArgumentList,  theGlobalVariables, false, true, true);}
  static int EvaluateSolveLPolyEqualsZeroOverCone
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateSliceCone
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateAnimateRootSystemBluePoint
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateAnimateRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return ParserNode::EvaluateAnimateRootSystem(theNode, theArgumentList, theGlobalVariables, 0);
  }
  static int EvaluateAnimateRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, root* bluePoint)
  ;
  static int EvaluateCone(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables);
  static int EvaluateConeFromVertices(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables);
  static int EvaluateSlTwoInSlN(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables);
  static int EvaluateVectorPFIndicator
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDrawRootSystem
  (ParserNode& theNode, char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables,
   root* bluePoint, bool wipeCanvas=true, bool LabelDynkin=false, bool DrawWeylChamber=false, roots* projectionPlane=0)
  ;
  static int EvaluateDrawRootSystemLabelDynkin
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateDrawRootSystemOld
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, root* bluePoint)
;
  static int EvaluateDrawRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, root* bluePoint)
;
  static int EvaluateDrawRootSystemFixedProjectionPlane
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDrawRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluateDrawRootSystem(theNode, theArgumentList, theGlobalVariables, 0);}
  static int EvaluateDrawRootSystemOld
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables){return EvaluateDrawRootSystemOld(theNode, theArgumentList, theGlobalVariables,0);}
  static int EvaluatePrintRootSAsAndSlTwos
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, bool redirectToSlTwos, bool forceRecompute)
;
  static int EvaluateDrawConeAffine
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDrawConeProjective
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluatePrintSlTwos
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluatePrintRootSAsAndSlTwos(theNode, theArgumentList, theGlobalVariables, true, false);
  }
  static int EvaluatePrintRootSAsForceRecompute
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluatePrintRootSAsAndSlTwos(theNode, theArgumentList, theGlobalVariables, false, true);
  }
  static int EvaluateG2InB3Computation
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateClosestPointToHyperplaneAlongTheNormal
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateDecomposeOverSubalgebra
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluatePrintRootSAs
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  { return theNode.EvaluatePrintRootSAsAndSlTwos(theNode, theArgumentList, theGlobalVariables, false, false);
  }
  int EvaluateWriteToFile(GlobalVariables& theGlobalVariables);
  void EvaluateOuterAutos(GlobalVariables& theGlobalVariables);
  void EvaluateMinus(GlobalVariables& theGlobalVariables);
  void EvaluateDereferenceArray(GlobalVariables& theGlobalVariables);
  void EvaluateMinusUnary(GlobalVariables& theGlobalVariables);
  void EvaluatePrintWeyl(GlobalVariables& theGlobalVariables);
  void EvaluateGCDorLCM(GlobalVariables& theGlobalVariables);
  static int EvaluateWeylAction
  (ParserNode& theNode,
   List<int>& theArgumentList, GlobalVariables& theGlobalVariables,
   bool DualAction, bool useRho, bool useMinusRho)
  ;
  int EvaluatePlus(GlobalVariables& theGlobalVariables);
  static int EvaluatePartialFractions
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateSplit
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateFindExtremaInDirectionOverLattice
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int  EvaluateWeylDimFormula
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateIntersectLatticeWithPreimageLattice
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  void EvaluatePrintEmbedding(GlobalVariables& theGlobalVariables);
  static int EvaluatePrintDecomposition
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluatePrintRootSystem
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateFactorial
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluatePrintAllPrimesEratosthenes
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateInvariantsSl2DegreeM
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateEigen
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  static int EvaluateEigenOrdered
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  void EvaluateSecretSauce(GlobalVariables& theGlobalVariables);
  static int EvaluateSecretSauceOrdered
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
  ;
  void EvaluateThePower(GlobalVariables& theGlobalVariables);
  void EvaluateUnderscore(GlobalVariables& theGlobalVariables);
  int EvaluateUnderscoreLeftArgumentIsArray(GlobalVariables& theGlobalVariables);
  void EvaluateEmbedding(GlobalVariables& theGlobalVariables);
  int EvaluateSubstitution(GlobalVariables& theGlobalVariables);
  int EvaluateApplySubstitution(GlobalVariables& theGlobalVariables);
  static int EvaluateModVermaRelations
   (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  static int EvaluateModVermaRelationsOrdered
   (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
;
  void EvaluateFunction(GlobalVariables& theGlobalVariables);
  void ExtractAndEvalWeylSubFromMap(GlobalVariables& theGlobalVariables);
  bool AllChildrenAreOfDefinedNonErrorType();
  bool OneChildrenOrMoreAreOfType(int theType);
  ParserNode();
};

class ParserFunctionArgumentTree
{ public:
  //format: the if the function argument is composite, then it corresponds to the first non-used element of nestedArgumentsOfArguments
  // for example, the function argument (A, (B,C), D, (E,F) )
  // would have functionArguments: typeA, typeComposite, typeD, typeComposite
  // and nestedArgumentsOfArguments would have two elements, the parametrizing respectively (B ,C) and (E,F)
  List<int> functionArguments;
  List<ParserFunctionArgumentTree> nestedArgumentsOfArguments;
  void operator=(const ParserFunctionArgumentTree& other)
  { this->functionArguments=other.functionArguments;
    this->nestedArgumentsOfArguments=other.nestedArgumentsOfArguments;
  }
  std::string ElementToString(bool useHtml, bool useLatex)const;
  void Nullify(){this->functionArguments.size=0; this->nestedArgumentsOfArguments.size=0;}

bool ConvertOneArgumentIndex
  (ParserNode& theNode, int theIndex, int& lowestNestedIndexNonExplored, GlobalVariables& theGlobalVariables)
  ;
  bool ConvertArguments
  (ParserNode& theNode, List<int>& outputArgumentIndices, GlobalVariables& theGlobalVariables)
    ;
  bool MakeFunctionArgumentFromString(const std::string& theArgumentList);
  bool MakeFromString(unsigned int& currentChar, const std::string& theArgumentList);
  bool AddArgumentNonNestedChangeInput(std::string& theArgument);
  static int GetTokenFromArgumentStringChangeInput(std::string& theArgument)
  { unsigned int finalSize=0;
    for (unsigned int i=0; i< theArgument.size(); i++)
      if (theArgument[i]!=' ')
      { theArgument[finalSize]=theArgument[i];
        finalSize++;
      }
    theArgument.resize(finalSize);
    return ParserFunctionArgumentTree::GetTokenFromArgumentStringNoSpaces(theArgument);
  }
  static int GetTokenFromArgumentStringNoSpaces(const std::string& theArgument);
  static std::string GetArgumentStringFromToken(int theArgument);

};

class ParserFunction
{
public:
  ParserFunctionArgumentTree theArguments;
  int (*theActualFunction)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables);
  char exampleAmbientWeylLetter;
  int exampleAmbientWeylRank;
  bool flagVisible;
  std::string functionDescription;
  std::string functionName;
  std::string example;
  void operator=(const ParserFunction& other)
  { this->theArguments=other.theArguments;
    this->theActualFunction=other.theActualFunction;
    this->functionDescription=other.functionDescription;
    this->functionName=other.functionName;
    this->example=other.example;
    this->flagVisible=other.flagVisible;
    this->exampleAmbientWeylRank=other.exampleAmbientWeylRank;
    this->exampleAmbientWeylLetter=other.exampleAmbientWeylLetter;
  }
  std::string ElementToString
(bool useHtml, bool useLatex, Parser& owner)const
  ;
  int CallMe
  (ParserNode& theNode, GlobalVariables& theGlobalVariables)
  ;
  bool MakeMe
  (const std::string& theFunctionName, const std::string& theFunctionArguments, const std::string& theFunctionDescription, const std::string& theExample, int (*inputFunctionAddress)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables))
  ;
  int HashFunction()const
  { return this->GetHashFromString(this->functionName);
  }
  int GetHashFromString(const std::string& input)const
  { int numCycles=MathRoutines::Minimum(SomeRandomPrimesSize, (int) input.size());
    int result=0;
    for(int i=0; i<numCycles; i++)
      result+=SomeRandomPrimes[i]*input[i];
    return result;
  }
  bool operator>(const ParserFunction& other) const
  { return this->functionName>other.functionName;
  }
  bool operator==(const ParserFunction& other)const
  { return this->functionName==other.functionName;
  }
  ParserFunction(){}
  void MakeFunctionNoArguments(){this->theArguments.Nullify();}
};

//the below class was written and implemented by an idea of helios from the forum of www.cplusplus.com
class Parser: public List<ParserNode>
{
public:
  std::string DebugString;
  std::string StringBeingParsed;
  List<std::string> SystemCommands;

  std::string DisplayNameCalculator;

  std::string DisplayPathServerBase;
  std::string PhysicalPathServerBase;

  std::string DisplayPathOutputFolder;
  std::string PhysicalPathOutputFolder;

  std::string DisplayNameDefaultOutput;
  std::string DisplayNameDefaultOutputNoPath;
  std::string PhysicalNameDefaultOutput;

  std::string indicatorFileName;
  std::string indicatorFileNameDisplay;
  std::string indicatorReportFileName;
  std::string indicatorReportFileNameDisplay;
  std::string userLabel;
  char DefaultWeylLetter;
  int DefaultWeylRank;
  int MaxFoundVars;
  bool flagDisplayIndicator;
  ParserNode theValue;
  HomomorphismSemisimpleLieAlgebra theHmm;
  SemisimpleLieAlgebraOrdered testAlgebra;
  SemisimpleLieAlgebraOrdered testSubAlgebra;
  std::string javaScriptDisplayingIndicator;
  std::string afterSystemCommands;
//  SemisimpleLieAlgebra theLieAlgebra;
  void initDefaultFolderAndFileNames
  (const std::string& inputPathBinaryBaseIsFolderBelow, const std::string& inputDisplayPathBase, const std::string& scrambledIP)
  ;
  void InitJavaScriptDisplayIndicator();
  void ComputeDebugString(bool includeLastNode, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
  { this->ElementToString(includeLastNode, DebugString, true, theGlobalVariables, theFormat);
  }
  void ElementToString(bool includeLastNode, std::string& output, bool useHtml, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat);
  enum tokenTypes
  { tokenExpression, tokenEmpty, tokenEnd, tokenDigit, tokenInteger, tokenPlus, tokenMinus, tokenMinusUnary, tokenUnderscore,  //=8
    tokenTimes, tokenDivide, tokenPower, tokenOpenBracket, tokenCloseBracket,//=13
    tokenOpenLieBracket, tokenCloseLieBracket, tokenOpenCurlyBracket, tokenCloseCurlyBracket, tokenX, tokenF, //=19
    tokenPartialDerivative, tokenComma, tokenLieBracket, tokenG, //=23
    tokenH, tokenC, tokenEmbedding, tokenVariable, //=27
    tokenArraY, tokenMapsTo, tokenColon, tokenDereferenceArray,
    tokenDot,
    tokenEndStatement, tokenFunction, tokenFunctionNoArgument,
  };
  HashedList<ParserFunction> theFunctionList;
  enum functionList
  { functionEigen, functionEigenOrdered, functionLCM, functionGCD, functionSecretSauce, functionSecretSauceOrdered, //=5
    functionWeylDimFormula, functionOuterAutos, functionMod, functionInvariants, functionOrder, functionEmbedding, //=11
    functionPrintDecomposition, functionPrintRootSystem, functionSlTwoInSlN, functionActByWeyl, functionActByAffineWeyl, //=16
    functionPrintWeylGroup, functionChamberParam, functionMaximumLFoverCone, functionCone, functionLattice, //21
    functionGetAllRepresentatives, functionInvertLattice, functionQuasiPolynomial, functionPartialFractions, functionSplit, //=26
    functionGetVPIndicator, functionMaximumQPoverCone, functionWriteToFile, functionReadFromFile, //=30
    functionIntersectWithSubspace, functionIntersectLatticeWithLatticePreimage, functionSubstitutionInQuasipolynomial, //=33
    functionIntersectHyperplaneByACone,
  };
  List<int> TokenBuffer;
  List<int> ValueBuffer;
  List<int> TokenStack;
  List<int> ValueStack;
  List<int> NodeIndexStack;
  int numEmptyTokensAtBeginning;
  bool flagFunctionListInitialized;
//  ElementSimpleLieAlgebra LieAlgebraValue;
//  ElementWeylAlgebra WeylAlgebraValue;
  //Transform operations create no new nodes.
  //Replace operations create new nodes.
  void ExpandOnTop(int numNew)
  { this->SetSize(this->size+numNew);
    for (int i=0; i<numNew; i++)
    { this->TheObjects[this->size-1-i].owner=this;
      this->TheObjects[this->size-1-i].Clear();
      this->TheObjects[this->size-1-i].indexInOwner=this->size-1-i;
    }
  }
  bool ReplaceObyE()
  { this->ExpandOnTopUseOperationOffset(0, this->tokenExpression);
    this->TransformRepeatXAtoA(1);
    return true;
  }
  bool ReplaceYOZbyE()
  { return this->ReplaceTwoChildrenOperationOffset(-2, 0, -1, 3, this->tokenExpression);
  }
  bool ReplaceOXAXbyE()
  { return this->ReplaceOneChildOperationOffset(-1, -3, 4, this->tokenExpression);
  }
  bool ReplaceOYbyE()
  { return this->ReplaceOneChildOperationOffset(0, -1, 2, this->tokenExpression);
  }
  bool ReplaceZYbyE(int theOperationToken)
  { return this->ReplaceTwoChildrenOperationToken(-1,0,2, theOperationToken, this->tokenExpression);
  }
  bool ReplaceXYbyE(int theOperationToken)
  { return this->ReplaceOneChildOperationToken(0, 2, theOperationToken, this->tokenExpression);
  }
  bool ReplaceXYCZXbyE(int theOperationToken)
  { return this->ReplaceTwoChildrenOperationToken(-3, -1, 5, theOperationToken, this->tokenExpression);
  }
  bool ReplaceXYXbyA()
  { return this->ReplaceOneChildOperationToken(-1, 3, this->tokenArraY, this->tokenArraY);
  }
  bool ReplaceEXEXbyE(int theOperation)
  { return this->ReplaceTwoChildrenOperationToken(-3, -1, 4, theOperation, this->tokenExpression);
  }
  bool ReplaceDdotsDbyEdoTheArithmetic();
  bool ReplaceXECdotsCEXbyE(int theDimension, int theNewToken, int theOperation);
  bool ReplaceXECdotsCEXEXbyE(int theDimension, int theNewToken, int theOperation);
  void FatherByLastNodeChildrenWithIndexInNodeIndex(int IndexInNodeIndex)
  { int childIndex=this->NodeIndexStack.TheObjects[IndexInNodeIndex];
    this->LastObject()->children.AddOnTop(childIndex);
    this->TheObjects[childIndex].indexParentNode=this->size-1;
  }
  bool ReplaceOneChildOperationToken
  (int child1Offset, int StackDecreaseNotCountingNewExpression, int theOperationToken, int theToken)
  { int lastIndexInNodeIndex=this->TokenStack.size-1;
    this->ExpandOnTopIncreaseStacks(theOperationToken, theToken, 0);
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(lastIndexInNodeIndex+child1Offset);
    this->TransformRepeatXAtoA(StackDecreaseNotCountingNewExpression);
    return true;
  }
  bool ReplaceTwoChildrenOperationToken
  (int child1Offset, int child2Offset, int StackDecreaseNotCountingNewExpression, int theOperationToken, int theToken)
  { int lastIndexInNodeIndex=this->TokenStack.size-1;
    this->ExpandOnTopIncreaseStacks(theOperationToken, theToken, 0);
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(lastIndexInNodeIndex+child1Offset);
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(lastIndexInNodeIndex+child2Offset);
    this->TransformRepeatXAtoA(StackDecreaseNotCountingNewExpression);
    return true;
  }
  bool ExpandOnTopIncreaseStacks(int theOperationToken, int theToken, int theIntValue)
  { this->ExpandOnTop(1);
    this->LastObject()->Operation=theOperationToken;
    this->LastObject()->intValue=theIntValue;
    this->NodeIndexStack.AddOnTop(this->size-1);
    this->ValueStack.AddOnTop(theIntValue);
    this->TokenStack.AddOnTop(theToken);
    return true;
  }
  bool ExpandOnTopUseOperationOffset(int OperationOffset, int theToken)
  { return this->ExpandOnTopIncreaseStacks(this->TokenStack.TheObjects[this->TokenStack.size-1+OperationOffset], theToken, this->ValueStack.TheObjects[this->TokenStack.size-1+OperationOffset]);
  }
  bool ReplaceTwoChildrenOperationOffset
  (int child1Offset, int child2Offset, int OperationOffset, int StackDecreaseNotCountingNewExpression, int theToken)
  { int lastIndexInNodeIndex=this->TokenStack.size-1;
    this->ExpandOnTopUseOperationOffset(OperationOffset, theToken);
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(lastIndexInNodeIndex+child1Offset);
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(lastIndexInNodeIndex+child2Offset);
    this->TransformRepeatXAtoA(StackDecreaseNotCountingNewExpression);
    return true;
  }
  std::string CreateBasicStructureConstantInfoIfItDoesntExist
  (GlobalVariables& theGlobalVariables)
  ;
  bool ReplaceOneChildOperationOffset
  (int child1Offset, int OperationOffset, int StackDecreaseNotCountingNewExpression, int theToken)
  { int lastIndexInNodeIndex=this->TokenStack.size-1;
    this->ExpandOnTopUseOperationOffset(OperationOffset, theToken);
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(lastIndexInNodeIndex+child1Offset);
    this->TransformRepeatXAtoA(StackDecreaseNotCountingNewExpression);
    return true;
  }
  bool TransformXtoE()
  { *this->TokenStack.LastObject()=this->tokenExpression;
    *this->ValueStack.LastObject()=0;
    return true;
  }
  bool TransformRepeatXAtoA(int repeat)
  { int startIndex=this->ValueStack.size-1;
    int targetIndex=startIndex-repeat;
    this->ValueStack.SwapTwoIndices(startIndex, targetIndex);
    this->NodeIndexStack.SwapTwoIndices(startIndex, targetIndex);
    this->TokenStack.SwapTwoIndices(startIndex, targetIndex);
    this->ValueStack.size-=repeat;
    this->NodeIndexStack.size-=repeat;
    this->TokenStack.size-=repeat;
    return true;
  }
  bool TransformXtoNothing()
  { this->ValueStack.PopLastObject();
    this->TokenStack.PopLastObject();
    this->NodeIndexStack.PopLastObject();
    return true;
  }
  bool TransformRepeatXtoNothing(int repeat)
  { for (int i=0; i<repeat; i++)
      this->TransformXtoNothing();
    return true;
  }
  bool TransformXYXtoY()
  { this->TransformXtoNothing();
    this->TransformRepeatXAtoA(1);
    return true;
  }
  void InitAndTokenize(const std::string& input);
  void Evaluate(GlobalVariables& theGlobalVariables);
  std::string ParseEvaluateAndSimplify
  (const std::string& input, bool useHtml, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
;
  void ParseEvaluateAndSimplifyPart1(const std::string& input, GlobalVariables& theGlobalVariables);
  std::string ParseEvaluateAndSimplifyPart2
  (const std::string& input, bool useHtml, GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
;
  ElementUniversalEnveloping<PolynomialRationalCoeff> ParseAndCompute
  (const std::string& input, GlobalVariables& theGlobalVariables)
  ;
  void Parse(const std::string& input);
  void ParseNoInit(int indexFrom, int indexTo);
  void ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables);
  bool ApplyRules(int lookAheadToken);
  bool lookAheadTokenProhibitsPlus(int theToken);
  bool lookAheadTokenProhibitsTimes(int theToken);
  bool lookAheadTokenAllowsMapsTo(int theToken);
  bool TokenProhibitsUnaryMinus(int theToken);
  void initTestAlgebraNeedsToBeRewritten(GlobalVariables& theGlobalVariables);
  void initFunctionList(char defaultExampleWeylLetter, int defaultExampleWeylRank);
  void AddOneFunctionToDictionaryNoFail
  (const std::string& theFunctionName, const std::string& theFunctionArguments, const std::string& theFunctionDescription,
   const std::string& theExample, int (*inputFunctionAddress)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables)
   )
  { bool tempBool=this->AddOneFunctionToDictionary
      (theFunctionName, theFunctionArguments, theFunctionDescription, theExample, 'B', 3, true, inputFunctionAddress);
    assert(tempBool);
  }
  void AddOneFunctionToDictionaryNoFail
  (const std::string& theFunctionName, const std::string& theFunctionArguments, const std::string& theFunctionDescription, const std::string& theExample, char ExampleWeylLetter, int ExampleWeylRank, bool isVisible, int (*inputFunctionAddress)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables)
   )
  { bool tempBool=this->AddOneFunctionToDictionary(theFunctionName, theFunctionArguments, theFunctionDescription, theExample, ExampleWeylLetter, ExampleWeylRank, isVisible, inputFunctionAddress);
    assert(tempBool);
  }
  void AddOneFunctionToDictionaryNoFail
  (const std::string& theFunctionName, const std::string& theFunctionArguments,
   const std::string& theFunctionDescription, const std::string& theExample, char ExampleWeylLetter,
   int ExampleWeylRank, int (*inputFunctionAddress)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables)
   )
  { bool tempBool=this->AddOneFunctionToDictionary(theFunctionName, theFunctionArguments, theFunctionDescription, theExample, ExampleWeylLetter, ExampleWeylRank, true, inputFunctionAddress);
    assert(tempBool);
  }
  bool AddOneFunctionToDictionary
  (const std::string& theFunctionName, const std::string& theFunctionArguments, const std::string& theFunctionDescription,
   const std::string& theExample, char ExampleWeylLetter, int ExampleWeylRank, bool isVisible,
   int (*inputFunctionAddress)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables)
)
  { ParserFunction newFunction;
    bool result=newFunction.MakeMe(theFunctionName, theFunctionArguments, theFunctionDescription, theExample, inputFunctionAddress);
    if (!this->theFunctionList.AddOnTopNoRepetitionHash(newFunction))
      return false;
    this->theFunctionList.LastObject()->exampleAmbientWeylLetter=ExampleWeylLetter;
    this->theFunctionList.LastObject()->exampleAmbientWeylRank=ExampleWeylRank;
    this->theFunctionList.LastObject()->flagVisible=isVisible;
    return result;
  }
  void SubAby(int newToken);
  std::string GetFunctionDescription();
  bool StackTopIsASub(int& outputNumEntries);
  bool StackTopIsAVector(int& outputDimension);
  bool StackTopIsDelimiter1ECdotsCEDelimiter2(int& outputDimension, int LeftDelimiter, int RightDelimiter);
  bool StackTopIsDelimiter1ECdotsCEDelimiter2EDelimiter3(int& outputDimension, int LeftDelimiter, int middleDelimiter, int rightDelimiter);
  bool IsAWordSeparatingCharacter(char c);
  bool LookUpInDictionaryAndAdd(std::string& input);
  void TokenToStringStream(std::stringstream& out, int theToken);
  void Clear();
  Parser(){ this->flagFunctionListInitialized=false; this->flagDisplayIndicator=true;}
};

class GlobalVariables
{
private:
  FeedDataToIndicatorWindow FeedDataToIndicatorWindowDefault;
  double (*getElapsedTimePrivate)();
  void (*callSystem)(const std::string& theSystemCommand);
public:
  int ProgressReportDepth;
  double MaxAllowedComputationTimeInSeconds;
  PolynomialOutputFormat theDefaultFormat;
  roots rootsGeneralPurposeBuffer1;
  roots rootsGeneralPurposeBuffer2;
  roots rootsWallBasis;
  roots rootsIsABogusNeighbor1;
  roots rootsIsABogusNeighbor2;
  roots rootsSplitChamber1;
  roots rootsSplitChamber2;
  roots rootsSplitChamber3;
  roots rootsNilradicalRoots;
  roots rootsConeConditionHolds2;
  roots rootsRootSAIso;
  roots rootsGetCoordsInBasis;
  roots rootsGetEpsilonCoords;
  roots rootsAttemptTheTripleTrick;
  roots rootsGetEpsCoords2;
  roots rootsGetEpsCoords3;
  roots rootsComputeEpsCoordsWRTk;
  roots rootsComputeEpsCoordsWRTk2;
  roots rootsProverStateComputation1;
  roots rootsProverStateComputation2;
  roots rootsProverStateComputation3;
  roots rootsProverStateComputation4;
  roots rootsProverStateComputation5;
  roots rootsAttepmtTheTripleTrick;
  roots rootsAttepmtTheTripleTrickWRTSA;
  roots rootsreduceOnceGeneralMethod;
  roots rootsGlueingGetNonSeparableChambers;
  roots rootsConeWriteToFileBuffer;

  rootsCollection rootsCollectionSplitChamber1;
  rootsCollection rootsCollectionSplitChamber2;
  rootsCollection rootsStronglyPerpendicular;
  rootsCollection rootsAttemptExtensionIso1;
  rootsCollection rootsAttemptExtensionIso2;
  rootsCollection rootsAttemptExtensionIso3;
  rootsCollection rootsAttemptExtensionIso4;
  rootsCollection rootsExtendToIsomorphismRootSystem1;
  rootsCollection rootsExtendToIsomorphismRootSystem2;

  rootSubalgebra rootSAProverIsos;
  rootSubalgebra rootSAAttemptExtensionToIso1;
  rootSubalgebra rootSAAttemptExtensionToIso2;

  rootSubalgebras rootSAAttemptExtensionIso1;
  rootSubalgebras rootSAAttemptExtensionIso2;
  rootSubalgebras rootSAsGenerateAll;

  HashedList<root> hashedRootsComputeSubGroupFromGeneratingReflections;
  HashedList<root> hashedRootsNewChamberSplit;

  List<CombinatorialChamber*> listCombinatorialChamberPtSplitChamber;
  List<int> listWallDataPtSplitChamber;
  List<int> listWallDataPtSplitChamber2;
  List<int> listPivotColsNewSplit;

  Monomial<Rational> monMakePolyExponentFromIntRoot;
  Monomial<Rational> monMakePolyFromDirectionAndNormal;

  MatrixLargeRational matTransposeBuffer;
  MatrixLargeRational matComputationBufferLinAlgOneAffinePlane;
  MatrixLargeRational matComputationBufferLinAlgAffinePart;
  MatrixLargeRational matComputeNormalFromSelection;
  MatrixLargeRational matOutputEmpty;
  MatrixLargeRational matIdMatrix;
  MatrixLargeRational matComputeNormalExcludingIndex;
  MatrixLargeRational matLinearAlgebraForVertexComputation;
  MatrixLargeRational matComputeNormalFromSelectionAndExtraRoot;
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
  MatrixLargeRational matGetEpsilonCoords2;
  MatrixLargeRational matGetEpsilonCoords3;
  MatrixLargeRational matComputeEpsCoordsWRTk;
  MatrixLargeRational matreduceOnceGeneralMethod;
  MatrixLargeRational matBogusNeighbors;
  MatrixLargeRational matBogusNeighborsEmpty;

  partFraction fracReduceMonomialByMonomial;
  partFraction fracRemoveRedundantRootsBuffer1;
 // partFraction fracRemoveRedundantRootsBuffer2;
  partFraction fracSplit1;
  QuasiPolynomialOld QPComputeQuasiPolynomial;
  QuasiNumber QNComputeQuasiPolynomial;

  Polynomial<LargeInt> PolyLargeIntPartFracBuffer1;
  Polynomial<LargeInt> PolyLargeIntPartFracBuffer2;
  Polynomial<LargeInt> PolyLargeIntPartFracBuffer3;
  Polynomial<LargeInt> PolyLargeIntPartFracBuffer4;
  Polynomial<LargeInt> PolyLargeIntPartFracBuffer5;
  Polynomial<LargeInt> PolyLargeIntPartFracBuffer6;

  IntegerPoly IPRemoveRedundantShortRootsClassicalRootSystem;
  IntegerPoly IPElementToStringBasisChange;
  IntegerPoly IPComputationalBufferAddPartFraction1;
  IntegerPoly IPComputationalBufferAddPartFraction2;
  IntegerPoly IPReduceMonomialByMonomialModifyOneMonomial1;
  IntegerPoly IPReduceMonomialByMonomialModifyOneMonomial2;

  PolyPartFractionNumerator PPFNElementToStringBasisChange;
  PolyPartFractionNumerator PPFNAddPartFraction1;
  PolyPartFractionNumerator PPFNAddPartFraction2;

  Selection selGeneralPurposeBuffer1;
  Selection selNonPivotPointsNewSplit;
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
  Selection selBogusNeighbor;
  Cone coneBuffer1NewSplit;
  Cone coneBuffer2NewSplit;

  PolynomialRationalCoeff RFgcdBuffer1, RFgcdBuffer2, RFgcdBuffer3, RFgcdBuffer4, RFgcdBuffer5;
  PolynomialRationalCoeff polyGeneralPurposeBuffer1;
  PolynomialRationalCoeff polyGeneralPurposeBuffer2;
  PolynomialRationalCoeff polyGeneralPurposeBuffer3;
  Monomial<Rational> RFgcdBuferMon1, RFgcdBuferMon2;
  List<PolynomialRationalCoeff> RFgcdBufferList1;

  SelectionWithDifferentMaxMultiplicities selWeylAlgebra1;

  ReflectionSubgroupWeylGroup subGroupActionNormalizerCentralizer;

  HashedList<Selection> hashedSelSimplexAlg;

  DynkinDiagramRootSubalgebra dynGetEpsCoords;
  DynkinDiagramRootSubalgebra dynAttemptTheTripleTrick;

  IndicatorWindowVariables theIndicatorVariables;
  DrawingVariables theDrawingVariables;
  PolynomialOutputFormat thePolyFormat;
  Controller theLocalPauseController;

  GlobalVariables();
  void SetTimerFunction(double (*timerFunction)())
  { this->getElapsedTimePrivate=timerFunction;
  }
  double GetElapsedSeconds()
  { if (this->getElapsedTimePrivate!=0)
      return this->getElapsedTimePrivate();
    return -1;
  }
  inline void IncrementReadWriteDepth()
  { this->ProgressReportDepth++;
    this->theIndicatorVariables.ProgressReportStrings.SetSize(MathRoutines::Maximum(5,  this->ProgressReportDepth+1));
  }
  void operator=(const GlobalVariables& other)
  { this->FeedDataToIndicatorWindowDefault=other.FeedDataToIndicatorWindowDefault;
    this->theDrawingVariables=other.theDrawingVariables;
  }
  inline void DrawBufferNoIniT()
  { this->theDrawingVariables.drawBufferNoIniT();
  }
  inline void SetFeedDataToIndicatorWindowDefault(FeedDataToIndicatorWindow input)
  { this->FeedDataToIndicatorWindowDefault=input;
  }
  inline FeedDataToIndicatorWindow GetFeedDataToIndicatorWindowDefault()
  { return this->FeedDataToIndicatorWindowDefault;
  }
  inline void FeedIndicatorWindow(IndicatorWindowVariables& input)
  { if (this->FeedDataToIndicatorWindowDefault!=0)
      this->FeedDataToIndicatorWindowDefault(input);
  }
  void SetCallSystem(void (*theSystemCall)(const std::string&))
  { this->callSystem=theSystemCall;
  }
  void System(const std::string& systemCommand)
  { if (this->callSystem!=0)
      this->callSystem(systemCommand);
  }
  void ClearIndicatorVars()
  { this->ProgressReportDepth=0;
    this->theIndicatorVariables.ProgressReportStrings.SetSize(5);
    for (int i=0; i<this->theIndicatorVariables.ProgressReportStrings.size; i++)
      this->theIndicatorVariables.ProgressReportStrings[i]="";
    this->theIndicatorVariables.StatusString1="";
  }
  inline void MakeProgressReport(const std::string& input, int stringIndex=0)
  { if (stringIndex>100)
      return;
    this->theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
    if (this->theIndicatorVariables.ProgressReportStrings.size<=stringIndex)
      this->theIndicatorVariables.ProgressReportStrings.SetSize(stringIndex+1);
    this->theIndicatorVariables.ProgressReportStrings[stringIndex]=input;
    this->MakeReport();
  }
  inline void MakeStatusReport(const std::string& input)
  { this->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->theIndicatorVariables.StatusString1=input;
    this->MakeReport();
  }
  inline void MakeReport()
  { if (this->FeedDataToIndicatorWindowDefault!=0)
      this->FeedDataToIndicatorWindowDefault(this->theIndicatorVariables);
  }
};

template <class Element>
void Monomial<Element>::DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData)
{ theDrawData.outputHeight=0;
  theDrawData.outputWidth=0;
  std::string tempS, tempS3;
  this->Coefficient.ElementToString(tempS);
  if (tempS=="1" && !this->IsAConstant())
    tempS="";
  else
    if (tempS=="-1" && !this->IsAConstant())
      tempS="-";
  theGlobalVariables.theDrawingVariables.drawString(theDrawData, tempS, theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
  for (int i=0; i<this->NumVariables; i++)
  { if (this->degrees[i]!=0)
    { theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer(theDrawData.outputWidth+theDrawData.TopLeftCornerX, theDrawData.outputHeight+theDrawData.TopLeftCornerY, "x", 0, theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
      theDrawData.outputWidth+=10;
      std::stringstream out1, out2; std::string tempS1, tempS2;
      out1 << i+1;
      tempS1=out1.str();
      theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer(theDrawData.outputWidth+theDrawData.TopLeftCornerX-3, theDrawData.outputHeight+theDrawData.TopLeftCornerY+9, tempS1, 0, theGlobalVariables.theDrawingVariables.fontSizeSubscript, theGlobalVariables.theDrawingVariables.TextStyleNormal);
      out2 << this->degrees[i];
      tempS2 = out2.str();
      theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer(theDrawData.outputWidth+theDrawData.TopLeftCornerX-3, theDrawData.outputHeight+theDrawData.TopLeftCornerY-1, tempS2, 0, theGlobalVariables.theDrawingVariables.fontSizeSubscript, theGlobalVariables.theDrawingVariables.TextStyleNormal);
      theDrawData.outputWidth+=5*MathRoutines::Maximum(tempS1.size(), tempS2.size());
    }
  }
}

template <class Element>
void Polynomial<Element>::DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData, PolynomialOutputFormat& PolyFormatLocal)
{ DrawElementInputOutput changeData;
  changeData.TopLeftCornerX = theDrawData.TopLeftCornerX;
  changeData.TopLeftCornerY = theDrawData.TopLeftCornerY;
  std::string tempS;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].DrawElement(theGlobalVariables, changeData);
    changeData.TopLeftCornerX+=changeData.outputWidth;
    changeData.outputWidth=0;
    if( i!=this->size-1)
    { this->TheObjects[i+1].ElementToString(tempS, PolyFormatLocal);
      if (tempS.at(0)!='-')
        theGlobalVariables.theDrawingVariables.drawString(changeData, "+", theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
      changeData.TopLeftCornerX+=changeData.outputWidth+3;
      changeData.outputWidth=0;
    }
  }
}

template <class Element>
void MathRoutines::RaiseToPower(Element& theElement, int thePower, const Element& theRingUnit)
{ if (thePower<0)
    return;
  if (thePower==1)
    return;
  if (thePower==0)
  { theElement=theRingUnit;
    return;
  }
  Element squares;
	squares=theElement;
	if (thePower<4)
  { for (int i=1; i<thePower; i++)
      theElement*=squares;
    return;
  }
  theElement=theRingUnit;
	while (thePower>0)
	{	if (thePower%2==1)
			theElement*=squares;
		squares*=squares;
		thePower/=2;
	}
}

class GlobalVariablesContainer :public List<GlobalVariables>
{
public:
  GlobalVariables* Default(){return &this->TheObjects[0]; };
};

/*class NonExpandedOnTheRightRationalPoly
{
public:
  std::string ElementToString();
  List<PolynomialRationalCoeff> theRightCoeffs;
  ListPointers<NonExpandedOnTheRightRationalPoly> theLeftBrackets;
};*/

class GeneralizedMonomialRational
{
  public:
  PolynomialRationalCoeff Coefficient;
  List<PolynomialRationalCoeff> degrees;
  void MakeOne(int NumVars);
  void Nullify
  (int numVars)
  ;
  std::string ElementToString(PolynomialOutputFormat& PolyFormatLocal);
  void MakeGenericMon
(int numVars,  int startingIndexFormalExponent)
  ;
  bool IsConstant()const
  { for(int i=0; i<degrees.size; i++)
      if (!this->degrees.TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
  int HashFunction()const
  { int result=0;
    int upperBound=MathRoutines::Minimum(degrees.size, SomeRandomPrimesSize);
    for (int i=0; i<upperBound; i++)
      for (int j=0; j<degrees.TheObjects[i].size; j++)
        result+=SomeRandomPrimes[i]*degrees.TheObjects[i].TheObjects[j].HashFunction();
    return result;
  }
  bool IsEqualToZero()const{ return this->Coefficient.IsEqualToZero();}
  bool operator==(const GeneralizedMonomialRational& other)const{ return this->degrees.operator==(other.degrees); }
  void operator=(const GeneralizedMonomialRational& other) {this->Coefficient.operator=(other.Coefficient); this->degrees.operator=(other.degrees);}
};

class GeneralizedPolynomialRational: public HashedList<GeneralizedMonomialRational>
{
  public:
  int NumCoeffVars;
  int NumMainVars;

  std::string ElementToString(PolynomialOutputFormat& PolyFormatLocal);
  void Nullify
(int numVarsCoeff, int numberMainVars)
  ;
  void MakeGenericMonomial(int numberMainVars, int numVarsCoeff);
  void MakeConst(int NumVars, const Rational& theCoeff);
  void MakeConst(int NumVars, const PolynomialRationalCoeff& theCoeff);
  void operator+=(const GeneralizedMonomialRational& theMon);
};

class EigenVectorComputation
{
public:
  rootsCollection theExponentShiftsTargetPerSimpleGenerator;
  roots theExponentShifts;
  PolynomialRationalCoeff coefficientInFrontOfMon;
  roots theModuleWeightsShifted;
//  List<Matrix<RationalFunction> > theSystemsPerGenerator;
  Matrix<RationalFunction> theSystem;
  List<ElementWeylAlgebra> theOperators;
  //the last variable corresponds to the generating function
  List<PolynomialRationalCoeff> theGeneratingSystemRows;
//  List<List<int> > theExponentShifts;
  std::string ComputeAndReturnStringNonOrdered(GlobalVariables& theGlobalVariables, Parser& theParser);
  void PrepareCartanSub
(SemisimpleLieAlgebraOrdered& theOwner, PolynomialsRationalCoeff& outputSub, GlobalVariables& theGlobalVariables)
  ;
  std::string ComputeAndReturnStringOrdered
(GlobalVariables& theGlobalVariables, Parser& theParser, int NodeIndex)
  ;

  std::string CrunchTheOperatorsOneMon
  (Monomial<Rational>& input, PolynomialRationalCoeff& output, GlobalVariables& theGlobalVariables, Parser& theParser)
  ;
  std::string CrunchTheOperators
  (GlobalVariables& theGlobalVariables, Parser& theParser)
  ;

  std::string ComputeEigenVectorsOfWeight
  (HomomorphismSemisimpleLieAlgebra& inputHmm, SemisimpleLieAlgebraOrdered& theOwner,
   List<ElementUniversalEnveloping<PolynomialRationalCoeff> >& output,
   root& theWeight, GlobalVariables& theGlobalVariables)
  ;
  std::string ComputeEigenVectorsOfWeightConventionOrdered
  (HomomorphismSemisimpleLieAlgebra& inputHmm, SemisimpleLieAlgebraOrdered& theOwner, List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> >& output, root& theWeight, GlobalVariables& theGlobalVariables)
  ;

  //the first rank-of-theOwner variables correspond to the coordinates of the highest weight; the next #positive roots
  //variables correspond to the exponents: the rank-of-theOwner+1st variable corresponds to the
  //exponent of the 1st negative root, (the root with index -1).
  //The rank-of-theOwner+2nd variable corresponds to the exponent of the 2nd negative root (the root with index -2)
  //Note that in the accepted order of monomials, the 1st negative root comes last (i.e. on the right hand side)
  void MakeGenericVermaElement
  (ElementUniversalEnveloping<PolynomialRationalCoeff>& theElt, SemisimpleLieAlgebra& theOwner)
  ;
  void WeylElementActsOnGeneralizedMonomial
  (ElementWeylAlgebra& inputWeylElement, GeneralizedMonomialRational& inputMonomial, GeneralizedPolynomialRational& output)
  ;
  void WeylMonomialActsOnGeneralizedMonomial
  (Monomial<Rational>& inputWeylMon, GeneralizedMonomialRational& inputMonomial, GeneralizedMonomialRational& output)
  ;
  bool AreUnimodular
(roots& input, Selection& outputBasisUnimodularity, GlobalVariables& theGlobalVariables)
  ;
 /* void TakeOutOfTheBracketFromTheRight
  (PolynomialRationalCoeff& input, NonExpandedOnTheRightRationalPoly& output)
  ;*/
  void MakeGenericVermaElementOrdered
  (ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theElt, SemisimpleLieAlgebraOrdered& theOwner)
  ;
  void GetDiffOperatorFromShiftAndCoefficient
  (PolynomialRationalCoeff& theCoeff, root& theShift, ElementWeylAlgebra& output, GlobalVariables& theGlobalVariables)
  ;
  void MakeGenericMonomialBranchingCandidate
  (HomomorphismSemisimpleLieAlgebra& theHmm,
   ElementUniversalEnveloping<PolynomialRationalCoeff>& theElt, GlobalVariables& theGlobalVariables);
  void DetermineEquationsFromResultLieBracketEquationsPerTarget
  (Parser& theParser, ElementUniversalEnveloping<PolynomialRationalCoeff>& theStartingGeneric,
   ElementUniversalEnveloping<PolynomialRationalCoeff>& theElt, std::stringstream& out,
   GlobalVariables& theGlobalVariables)
  ;
  void DetermineEquationsFromResultLieBracketEquationsPerTargetOrdered
  (Parser& theParser, int nodeIndex, ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theStartingGeneric, ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theElt, std::stringstream& out, GlobalVariables& theGlobalVariables)
  ;
  void RootIndexToPoly(int theIndex, SemisimpleLieAlgebra& theAlgebra, PolynomialRationalCoeff& output);
};

class PolynomialOverModule;

class SSalgebraModuleOld
{
public:
  SemisimpleLieAlgebra theAlgebra;
  SemisimpleLieAlgebra ambientAlgebrA;
  PolynomialsRationalCoeff invariantsFound;
  List<MatrixLargeRational> actionsNegativeRootSpacesCartanPositiveRootspaces;
  List<ElementSimpleLieAlgebra> moduleElementsEmbedded;
  List<ElementUniversalEnveloping<PolynomialRationalCoeff> > invariantsMappedToEmbedding;

  //Index ordering of sipleNegGenerators:
  //if simplePosGenerators.TheObjects[i] is a positive root space then its opposite root space should be
  //simpleNegGenerators.TheObjects[i]
  void GenerateSubmoduleFromRootNegativeRootSpacesFirst
  (root& input, roots& output, List<MatrixLargeRational>& allSimpleGenerators, GlobalVariables& theGlobalVariables)
  ;
  void ExtractHighestWeightVectorsFromVector
  (root& input, roots& outputVectors, List<MatrixLargeRational>& simplePosGenerators, List<MatrixLargeRational>& simpleNegGenerators)
  ;
  void ClimbDownFromHighestWeightAlongSl2String
  (root& input, root& output, Rational& outputCoeff, MatrixLargeRational& posGenerator, MatrixLargeRational& negGenerator, int generatorPower)
  ;
  void ClimbDownFromVectorAccordingToSequence
  (root& input, root& output, Rational& outputCoeff, List<MatrixLargeRational>& simplePosGenerators, List<MatrixLargeRational>& SimpleNegGenerators,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers)
  ;
  void ClimbUpFromVector
  (root& input, root& outputLastNonZero, List<MatrixLargeRational>& SimplePositiveGenerators, List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers)
  ;
  void ComputeAdMatrixFromModule
  (ElementSimpleLieAlgebra& theElt, roots& theModule, const SemisimpleLieAlgebra& ambientLieAlgebra, MatrixLargeRational& output,
   GlobalVariables& theGlobalVariables)
     ;
  void ConvertElementsToAdMatrixFormNegativeRootSpacesFirst
  (const List<ElementSimpleLieAlgebra>& input, const SemisimpleLieAlgebra& ambientLieAlgebra, List<MatrixLargeRational>& output)
  ;
  void ConvertElementsToAdMatrixFormNegativeRootSpacesFirst
  (const ElementSimpleLieAlgebra& input, const SemisimpleLieAlgebra& ambientLieAlgebra, MatrixLargeRational& output)
  ;
  void ComputeInvariantsOfDegree
  (int degree, std::stringstream& out, GlobalVariables& theGlobalVariables)
  ;
  void mapInvariantsToEmbedding
  (std::stringstream& out, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables,
   PolynomialRationalCoeff& theRingUnit, PolynomialRationalCoeff& theRingZero)
     ;
  void mapMonomialToEmbedding
  (Monomial<Rational>& input, ElementUniversalEnveloping<PolynomialRationalCoeff>& Accum,
   SemisimpleLieAlgebra& owner, std::stringstream& out, GlobalVariables& theGlobalVariables,
   PolynomialRationalCoeff& theRingUnit, PolynomialRationalCoeff& theRingZero)
  ;
  void InduceFromEmbedding
  (std::stringstream& out, HomomorphismSemisimpleLieAlgebra& theHmm, GlobalVariables& theGlobalVariables);
  void ActOnPolynomialOverModule
  (const ElementSimpleLieAlgebra& theActingElement, const PolynomialOverModule& theArgument,
   PolynomialOverModule& output)
  ;
  void ActOnMonomialOverModule
  (int indexChevalleyGeneratorNegativeSpacesFirst, const Monomial<Rational>& theArgument, PolynomialRationalCoeff& output)
  ;
  int GetDim()const {if (this->actionsNegativeRootSpacesCartanPositiveRootspaces.size==0) return -1; return this->actionsNegativeRootSpacesCartanPositiveRootspaces.TheObjects[0].NumRows;};
};

template <class CoefficientType>
class ElementVermaModuleOrdered
{
public:
  ElementUniversalEnvelopingOrdered<CoefficientType> theElT;
  std::string DebugString;
  PolynomialsRationalCoeff theSubNthElementIsImageNthCoordSimpleBasis;
  void ComputeDebugString(){PolynomialOutputFormat tempFormat; this->DebugString=this->ElementToString(tempFormat);}
  std::string ElementToString(const PolynomialOutputFormat& theFormat)const;
  void ElementToString(std::string& output)const{output=this->ElementToString();}
  bool IsEqualToZero()const {return this->theElT.IsEqualToZero();}
  bool ElementToStringNeedsBracketsForMultiplication()const;
  void AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
  (ElementUniversalEnvelopingOrdered<CoefficientType>& input, const ElementVermaModuleOrdered<CoefficientType>& theRingZero,
   GlobalVariables* theContext, const CoefficientType& theRingUnit)
  ;
  void ActOnMe
  (const ElementSimpleLieAlgebra& actingElt, ElementVermaModuleOrdered<CoefficientType>& output, SemisimpleLieAlgebra& owner, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)const
  ;
  static void GetBasisFromSpanOfElements
  (List<ElementVermaModuleOrdered<CoefficientType> >& theElements, Vectors<RationalFunction>& outputCoordinates, List<ElementVermaModuleOrdered>& outputTheBasis,
    const RationalFunction& RFOne, const RationalFunction& RFZero, GlobalVariables& theGlobalVariables)
    ;
  bool GetCoordsInBasis
  (const List<ElementVermaModuleOrdered<CoefficientType> >& theBasis, Vector<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables& theGlobalVariables)const
  ;
  bool IsProportionalTo(const ElementVermaModuleOrdered<CoefficientType>& other, CoefficientType& outputTimesMeEqualsOther, const CoefficientType& theRingZero)const{ return this->theElT.IsProportionalTo(other.theElT, outputTimesMeEqualsOther, theRingZero); }
  void Nullify(SemisimpleLieAlgebraOrdered& owner, PolynomialsRationalCoeff& incomingSub)
  { this->theElT.Nullify(owner);
    this->theSubNthElementIsImageNthCoordSimpleBasis=incomingSub;
  }
  template <class CoefficientTypeOther>
  void operator*=(const CoefficientTypeOther& theConst)
  { this->theElT.operator*=<CoefficientTypeOther>(theConst);
  }
  void MultiplyOnTheLeft
(const ElementSimpleLieAlgebra& other, ElementVermaModuleOrdered<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
;
  void ClearDenominators(CoefficientType& outputWasMultipliedBy, const CoefficientType& theRingUnit){ this->theElT.ClearDenominators(outputWasMultipliedBy, theRingUnit); }
  void operator/=(const CoefficientType& theConst){this->theElT.operator/=(theConst);}
  void operator-=(const ElementVermaModuleOrdered& other){ this->theElT-=other.theElT;}
  void operator+=(const ElementVermaModuleOrdered& other){ this->theElT+=other.theElT;}
  void operator=(const ElementVermaModuleOrdered& other)
  { this->theElT=other.theElT;
    this->theSubNthElementIsImageNthCoordSimpleBasis=other.theSubNthElementIsImageNthCoordSimpleBasis;
  }
};

class PolynomialOverModule : PolynomialRationalCoeff
{
  friend class SSalgebraModuleOld;
public:
  SSalgebraModuleOld* owner;
  std::string ElementToString(){ PolynomialOutputFormat PolyFormatLocal; return this->::PolynomialRationalCoeff::ElementToString(PolyFormatLocal); }
  void Assign(const PolynomialOverModule& other){ this->::PolynomialRationalCoeff::Assign(other); this->owner=other.owner;}
  void operator=(const PolynomialOverModule& other){ this->Assign(other); }
  int GetNumVars(){return this->NumVars;};
  void Nullify(SSalgebraModuleOld& theOwner){this->owner=&theOwner; this->::PolynomialRationalCoeff::Nullify(theOwner.GetDim());}
};

class slTwoInSlN
{
  int GetModuleIndexFromHighestWeightVector(const MatrixLargeRational& input)
  { Rational tempRat;
    for (int i=0; i<this->theHighestWeightVectors.size; i++)
      if (this->theHighestWeightVectors.TheObjects[i].IsProportionalTo(input, tempRat))
        return i;
    return -1;
  }
public:
  int theDimension;
  MatrixLargeRational theH;
  MatrixLargeRational theE;
  MatrixLargeRational theF;

  List<int> thePartition;
  List<MatrixLargeRational> theProjectors;
  List<MatrixLargeRational> theHighestWeightVectors;
  List<List<MatrixLargeRational> > theGmodKModules;
  List<List<List<int> > > PairingTable;
  void GetIsPlusKIndexingFrom(int input, int& s, int& k);
  std::string ElementMatrixToTensorString(const MatrixLargeRational& input, bool useHtml);
  std::string initFromModuleDecomposition(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable);
  std::string initPairingTable(bool useHtml);
  std::string ElementModuleIndexToString(int input, bool useHtml);
  std::string GetNotationString(bool useHtml);
  bool ComputeInvariantsOfDegree
  (List<int>& decompositionDimensions, int theDegree, List<PolynomialRationalCoeff>& output, std::string& outputError, GlobalVariables& theGlobalVariables)
  ;
  std::string PairTwoIndices
  (List<int>& output, int leftIndex, int rightIndex, bool useHtml)
  ;
  void ExtractHighestWeightVectorsFromVector
  (MatrixLargeRational& input, List<MatrixLargeRational>& outputDecompositionOfInput, List<MatrixLargeRational>& outputTheHWVectors)
  ;
  void ClimbDownFromHighestWeightAlongSl2String
  (MatrixLargeRational& input, MatrixLargeRational& output, Rational& outputCoeff, int generatorPower)
  ;
  void ClimbUpFromVector
  (MatrixLargeRational& input, MatrixLargeRational& outputLastNonZero, int& largestPowerNotKillingInput)
  ;
};

class GeneralizedVermaModuleCharacters
{
public:
  inline static const std::string GetXMLClassName(){ return "GeneralizedVermaCharacters";}
  Controller thePauseControlleR;
  List<MatrixLargeRational> theLinearOperators;
  //the first k variables correspond to the Cartan of the smaller Lie algebra
  //the next l variables correspond to the Cartan of the larger Lie algebra
  //the last variable is the projectivization
  List<MatrixLargeRational> theLinearOperatorsExtended;
  root NonIntegralOriginModificationBasisChanged;
  std::fstream theMultiplicitiesMaxOutput;
  std::fstream theMultiplicitiesMaxOutputReport2;
  roots GmodKnegativeWeightS;
  roots GmodKNegWeightsBasisChanged;
  MatrixLargeRational preferredBasisChangE;
  MatrixLargeRational preferredBasisChangeInversE;
  roots preferredBasiS;
  Cone PreimageWeylChamberLargerAlgebra;
  Cone WeylChamberSmallerAlgebra;
  Cone PreimageWeylChamberSmallerAlgebra;
  Lattice theExtendedIntegralLatticeMatForM;
  List<QuasiPolynomial> theQPsNonSubstituted;
  List<List<QuasiPolynomial> > theQPsSubstituted;
  List<QuasiPolynomial> theMultiplicities;
//  List<QuasiPolynomial> theMultiplicitiesExtremaCandidates;
  int UpperLimitChambersForDebugPurposes;
  int numNonZeroMults;
  Selection ParabolicLeviPartRootSpacesZeroStandsForSelected;
  Selection ParabolicSelectionSmallerAlgebra;
  List<Rational> theCoeffs;
  roots theTranslationS;
  roots theTranslationsProjectedBasisChanged;
  partFractions thePfs;
//  List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
  ConeComplex projectivizedParamComplex;
  ConeLatticeAndShiftMaxComputation theMaxComputation;
  ConeComplex smallerAlgebraChamber;
  ConeComplex projectivizedChambeR;
  std::stringstream log;
  Parser theParser;
  WeylGroup WeylSmaller;
  WeylGroup WeylLarger;
  int computationPhase;
  int NumProcessedConesParam;
  int NumProcessedExtremaEqualOne;
  std::string ComputeMultsLargerAlgebraHighestWeight
  ( root& highestWeightLargerAlgebraFundamentalCoords, root& parabolicSel, Parser& theParser, GlobalVariables& theGlobalVariables
   )
  ;
  std::string CheckMultiplicitiesVsOrbits
  (GlobalVariables& theGlobalVariables)
  ;
  std::string ElementToStringMultiplicitiesReport
  (GlobalVariables& theGlobalVariables)
  ;
  void IncrementComputation
  (root& parabolicSel, GlobalVariables& theGlobalVariables)
  ;
  std::string PrepareReport(GlobalVariables& theGlobalVariables);
  GeneralizedVermaModuleCharacters()
  { this->UpperLimitChambersForDebugPurposes=-1;
    this->computationPhase=0;
    this->NumProcessedConesParam=0;
    this->NumProcessedExtremaEqualOne=0;
    this->numNonZeroMults=0;
  }
  void ReadFromDefaultFile(GlobalVariables* theGlobalVariables);
  void WriteToDefaultFile(GlobalVariables* theGlobalVariables);
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
  ;
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables)
  { std::string tempS;
    int numReadWords;
    root parSel; parSel=this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
    if (theGlobalVariables!=0)
    { this->theParser.theHmm.MakeG2InB3(this->theParser, *theGlobalVariables);
      this->initFromHomomorphism(parSel, this->theParser.theHmm, *theGlobalVariables);
    } else
    { GlobalVariables tempGlobalVars;
      this->theParser.theHmm.MakeG2InB3(this->theParser, tempGlobalVars);
      this->initFromHomomorphism(parSel, this->theParser.theHmm, tempGlobalVars);
    }
    XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());

    input >> tempS >> this->computationPhase;
    assert(tempS=="ComputationPhase:");
    bool result=true;
    if (this->computationPhase!=0)
      result= this->ReadFromFileNoComputationPhase(input, theGlobalVariables);
    XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
    return result;
  }
  bool ReadFromFileNoComputationPhase
  (std::fstream& input, GlobalVariables* theGlobalVariables)
  ;
  std::string PrepareReportOneCone
  (PolynomialOutputFormat& theFormat, Cone& theCone, GlobalVariables& theGlobalVariables)
  ;
  void GetProjection(int indexOperator, const root& input, root& output);
  void SplitByMultiplicityFreeWall(Cone& theCone, ConeComplex& output);
  void InitTheMaxComputation
  (GlobalVariables& theGlobalVariables)
  ;
  void ComputeQPsFromChamberComplex
  (GlobalVariables& theGlobalVariables)
  ;
  void GetSubFromIndex(QPSub& output, int theIndex);
  void SortMultiplicities(GlobalVariables& theGlobalVariables);
  void GetSubFromNonParamArray
(MatrixLargeRational& output, root& outputTranslation, roots& NonParams, int numParams)
  ;
  void initQPs
  (GlobalVariables& theGlobalVariables)
  ;
  void initFromHomomorphism
  (root& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjectiveStep1
  (GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjectiveStep2
  (GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (int indexOperator, root& startingNormal, root& outputNormal)
  ;
};

class SemisimpleSubalgebras
{
public:
  std::string DebugString;
  std::string ElementToString();
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  SltwoSubalgebras theSl2s;
  SemisimpleLieAlgebra theAlgebra;
  rootsCollection theHcandidates;
  int indexLowestUnexplored;
  void FindHCandidates
    (char WeylLetter, int WeylDim, GlobalVariables& theGlobalVariables)
  ;
  void FindHCandidatesWithOneExtraHContaining
  (roots& inpuT, GlobalVariables& theGlobalVariables)
  ;
};

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::MakeOneGenerator
(int theIndex, const CoefficientType& theCoeff, SemisimpleLieAlgebraOrdered& owner, GlobalVariables* theContext)
{ this->Nullify(owner);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
//  tempMon.Nullify(theCoeff.NumVars, owner, theContext);
  tempMon.Coefficient=theCoeff;
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddOnTopHash(tempMon);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The implementation of the big templating-in effort for the root class starts here.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class CoefficientType>
bool Vectors<CoefficientType>::LinSpanContainsRoot(const Vector<CoefficientType>& input, Matrix<CoefficientType>& bufferMatrix, Selection& bufferSelection)const
{ Vectors<CoefficientType> tempVectors;
  tempVectors.CopyFromBase(*this);
  tempVectors.AddOnTop(input);
//  this->ComputeDebugString();
//  tempRoots.ComputeDebugString();
//  input.ComputeDebugString();
  return this->GetRankOfSpanOfElements(bufferMatrix, bufferSelection)==tempVectors.GetRankOfSpanOfElements(bufferMatrix, bufferSelection);
}

template <class CoefficientType>
int Vectors<CoefficientType>::GetRankOfSpanOfElements(Matrix<CoefficientType>& buffer, Selection& bufferSelection)const
{ if (this->size==0)
    return 0;
  int theDimension= this->TheObjects[0].size;
  this->GaussianEliminationForNormalComputation(buffer, bufferSelection, theDimension);
  return (theDimension-bufferSelection.CardinalitySelection);
}

template <class CoefficientType>
void Vectors<CoefficientType>::GaussianEliminationForNormalComputation(Matrix<CoefficientType>& inputMatrix, Selection& outputNonPivotPoints, int theDimension) const
{ inputMatrix.init((int)this->size, (int)theDimension);
  Matrix<CoefficientType> matOutputEmpty;
  matOutputEmpty.init(-1, -1);
  outputNonPivotPoints.init(theDimension);
  for(int i=0; i<this->size; i++)
    for(int j=0; j<theDimension; j++)
      inputMatrix.elements[i][j].Assign(this->TheObjects[i].TheObjects[j]);
  Matrix<CoefficientType>::GaussianEliminationByRows(inputMatrix, matOutputEmpty, outputNonPivotPoints);
}

template <class CoefficientType>
void Vectors<CoefficientType>::GetCoordsInBasis
  (const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& outputCoords,
   Vectors<CoefficientType>& bufferVectors, Matrix<CoefficientType>& bufferMat, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ assert(this!=&outputCoords);
  outputCoords.SetSize(this->size);
  for(int i=0; i<this->size; i++)
    this->TheObjects[i].GetCoordsInBasiS(inputBasis, outputCoords.TheObjects[i], bufferVectors, bufferMat, theRingUnit, theRingZero);
}

template <class CoefficientType>
bool Vector<CoefficientType>::GetIntegralCoordsInBasisIfTheyExist
(const Vectors<CoefficientType>& inputBasis, Vector<CoefficientType>& output,
  Matrix<CoefficientType>& bufferMatGaussianEliminationCC,
  Matrix<CoefficientType>& bufferMatGaussianElimination,
  const CoefficientType& theRingUnit,
  const CoefficientType& theRingMinusUnit,
  const CoefficientType& theRingZero)
{ int theDim=this->size;
  bufferMatGaussianElimination.init(inputBasis.size, theDim);
  for (int i=0; i<inputBasis.size; i++)
    for (int j=0; j<theDim; j++)
      bufferMatGaussianElimination.elements[i][j]=inputBasis.TheObjects[i].TheObjects[j];
  bufferMatGaussianEliminationCC.MakeIdMatrix(bufferMatGaussianElimination.NumRows, theRingUnit, theRingZero);
  //std::cout << "<br> the matrix before integral gaussian elimination: " << bufferMatGaussianElimination.ElementToString(true, false) << " and the other matrix: " << bufferMatGaussianEliminationCC.ElementToString(true, false);
  bufferMatGaussianElimination.GaussianEliminationEuclideanDomain(&bufferMatGaussianEliminationCC, theRingMinusUnit, theRingUnit);
  //std::cout << "<br> the matrix after integral gaussian elimination: " << bufferMatGaussianElimination.ElementToString(true, false) << " and the other matrix: " << bufferMatGaussianEliminationCC.ElementToString(true, false);
  Vector<CoefficientType> tempRoot, theCombination;
  assert(this!=&output);
  output.MakeZero(inputBasis.size, theRingZero);
  theCombination=*this;
  int col=0;
//  std::cout << "<br>vector whose coords we wanna find: " << this->ElementToString();
  for (int i=0; i<inputBasis.size; i++)
  { for (; col<theDim; col++)
      if (!bufferMatGaussianElimination.elements[i][col].IsEqualToZero())
        break;
    if (col>=theDim)
       break;
    bufferMatGaussianElimination.RowToRoot(i, tempRoot);
    output.TheObjects[i]=this->TheObjects[col];
    output.TheObjects[i]/=bufferMatGaussianElimination.elements[i][col];
    tempRoot*=output.TheObjects[i];
    theCombination-=tempRoot;
  }
  if (!theCombination.IsEqualToZero())
    return false;
//  std::cout << "<br>" << bufferMatGaussianEliminationCC.ElementToString(true, false) << " acting on " << output.ElementToString();
  bufferMatGaussianEliminationCC.ActMultiplyVectorRowOnTheRight(output, theRingZero);
//  std::cout << " gives " << output.ElementToString();
  return true;
}

template <class CoefficientType>
bool Vector<CoefficientType>::GetCoordsInBasiS
(const Vectors<CoefficientType>& inputBasis, Vector<CoefficientType>& output,
  Vectors<CoefficientType>& bufferVectors, Matrix<CoefficientType>& bufferMat,
  const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ bufferVectors.size=0;
  bufferVectors.AddListOnTop(inputBasis);
  bufferVectors.AddOnTop(*this);
//  bufferVectors.ComputeDebugString();
  if(!bufferVectors.GetLinearDependence(bufferMat, theRingUnit, theRingZero))
    return false;
//  tempRoots.ComputeDebugString();
//  tempMat.ComputeDebugString();
  bufferMat/=bufferMat.elements[bufferMat.NumRows-1][0];
  output.SetSize(bufferMat.NumRows-1);
  for (int i=0; i<bufferMat.NumRows-1; i++)
  { bufferMat.elements[i][0].Minus();
    output.TheObjects[i].Assign(bufferMat.elements[i][0]);
  }
  return true;
}

template <class CoefficientType>
void Vectors<CoefficientType>::GetLinearDependenceRunTheLinearAlgebra
  (Matrix<CoefficientType>& outputTheLinearCombination, Matrix<CoefficientType>& outputTheSystem, Selection& outputNonPivotPoints)
{ if (this->size==0)
    return;
  int Dimension=(int) this->TheObjects[0].size;
  outputTheSystem.init(Dimension, (int)this->size);
  Matrix<CoefficientType> matOutputEmpty;
  matOutputEmpty.init(-1, -1);
  for(int i=0; i<this->size; i++)
    for(int j=0; j<Dimension; j++)
      outputTheSystem.elements[j][i].Assign(this->TheObjects[i].TheObjects[j]);
  //outputTheSystem.ComputeDebugString();
  Matrix<CoefficientType>::GaussianEliminationByRows(outputTheSystem, matOutputEmpty, outputNonPivotPoints);
  //outputTheSystem.ComputeDebugString();
}

template <class CoefficientType>
bool Vectors<CoefficientType>::GetLinearDependence
  (Matrix<CoefficientType>& outputTheLinearCombination, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ Matrix<CoefficientType> tempMat;
  Selection nonPivotPoints;
  this->GetLinearDependenceRunTheLinearAlgebra(outputTheLinearCombination, tempMat, nonPivotPoints);
  //std::cout << tempMat.ElementToString(true, false);
  if (nonPivotPoints.CardinalitySelection==0)
    return false;
//  outputTheLinearCombination.ComputeDebugString();
  tempMat.NonPivotPointsToEigenVectorMatrixForm(nonPivotPoints, outputTheLinearCombination, theRingUnit, theRingZero);
  //outputTheLinearCombination.ComputeDebugString();
  return true;
}

template<typename Element>
void Matrix<Element>::NonPivotPointsToEigenVectorMatrixForm
(Selection& TheNonPivotPoints, Matrix<Element>& output, const Element& theRingUnit, const Element& theRingZero)
{ int RowCounter=0;
  output.init(this->NumCols, 1);
  for (int i=0; i<this->NumCols; i++)
  { if (!TheNonPivotPoints.selected[i])
    { output.elements[i][0]=theRingZero;
      for (int j=0; j<TheNonPivotPoints.CardinalitySelection; j++)
        output.elements[i][0]-=(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
      RowCounter++;
    }
    else
      output.elements[i][0]=theRingUnit;
  }
}

template<typename Element>
void Matrix<Element>::NonPivotPointsToEigenVector
(Selection& TheNonPivotPoints, Vector<Element>& output, const Element& theRingUnit, const Element& theRingZero)
{ int RowCounter=0;
  output.SetSize(this->NumCols);
  for (int i=0; i<this->NumCols; i++)
  { if (!TheNonPivotPoints.selected[i])
    { output.TheObjects[i]=theRingZero;
      for (int j=0; j<TheNonPivotPoints.CardinalitySelection; j++)
        output.TheObjects[i]-=(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
      RowCounter++;
    }
    else
      output.TheObjects[i]=theRingUnit;
  }
}

template <class CoefficientType>
void Vectors<CoefficientType>::SelectABasis
  (Vectors<CoefficientType>& output, const CoefficientType& theRingZero, Selection& outputSelectedIndices, GlobalVariables& theGlobalVariables)const
{ assert(this!=&output);
  if (this->size==0)
    return;
  int theDim=this->TheObjects[0].size;
  output.MakeActualSizeAtLeastExpandOnTop(this->size);
  output.size=0;
  Matrix<CoefficientType> theMat, matEmpty;
  theMat.init(1, theDim);
  int currentRow=0;
  Selection theSel;
  outputSelectedIndices.init(this->size);
  for (int i=0; i<this->size; i++)
  { if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Selecting a basis in dimension " << theDim << ", processed " << i << " out of " << this->size;
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=out.str();
      theGlobalVariables.theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
      theGlobalVariables.MakeReport();
    }
    for (int j=0; j<theDim; j++)
      theMat.elements[currentRow][j]=this->TheObjects[i].TheObjects[j];
    theMat.ComputeDebugString(false, false);
    theMat.GaussianEliminationByRows(theMat, matEmpty, theSel);
    theMat.ComputeDebugString(false, false);
    if (currentRow<theDim-theSel.CardinalitySelection)
    { output.AddOnTop(this->TheObjects[i]);
      outputSelectedIndices.AddSelectionAppendNewIndex(i);
      theMat.Resize(currentRow+2, theDim, true, &theRingZero);
    }
    currentRow=theDim-theSel.CardinalitySelection;
    if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Selecting a basis in dimension " << theDim << ", processed " << i << " out of " << this->size;
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=out.str();
      theGlobalVariables.theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
      theGlobalVariables.MakeReport();
    }
    if (currentRow==theDim)
      return;
  }
}

template <class Object>
void List<Object>::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes)
{ int NumWritten=this->size;
  if (UpperLimitForDebugPurposes>0 && UpperLimitForDebugPurposes<NumWritten)
    NumWritten=UpperLimitForDebugPurposes;
  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "size: " << NumWritten << "\n";
  for (int i=0; i<NumWritten; i++)
  { this->TheObjects[i].WriteToFile(output, theGlobalVariables);
    output << "\n";
    if (theGlobalVariables!=0)
      theGlobalVariables->theLocalPauseController.SafePointDontCallMeFromDestructors();
  }
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class Object>
bool List<Object>::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes)
{ std::string tempS;
  int ActualListSize; int NumWordsBeforeTag;
  XMLRoutines::ReadThroughFirstOpenTag(input, NumWordsBeforeTag, this->GetXMLClassName());
  assert(NumWordsBeforeTag==0);
  input >> tempS >> ActualListSize;
  assert(tempS=="size:");
  if(tempS!="size:")
    return false;
  if (theGlobalVariables!=0)
    theGlobalVariables->IncrementReadWriteDepth();
  int CappedListSize = ActualListSize;
  if (UpperLimitForDebugPurposes>0 && UpperLimitForDebugPurposes<CappedListSize)
    CappedListSize=UpperLimitForDebugPurposes;
  this->SetSize(CappedListSize);
  for (int i=0; i<CappedListSize; i++)
  { this->TheObjects[i].ReadFromFile(input, theGlobalVariables);
    //<reporting_and_safepoint_duties>
    if (theGlobalVariables!=0)
    { if (ActualListSize>30)
      { std::stringstream report;
        report << "Reading object number " << i+1 << " out of " << ActualListSize;
        if (CappedListSize<ActualListSize)
          report << " capped at " << CappedListSize;
        theGlobalVariables->theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
        theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]=report.str();
        theGlobalVariables->MakeReport();
      }
      theGlobalVariables->theLocalPauseController.SafePointDontCallMeFromDestructors();
    }
    //</reporting_and_safepoint_duties>
  }
  bool tempBool= XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsBeforeTag, this->GetXMLClassName());
  assert(tempBool);
  if (theGlobalVariables!=0)
    theGlobalVariables->ProgressReportDepth--;
  return true;
}

template<class Base>
class CompleX
{
  static bool flagEqualityIsApproximate;
  static double EqualityPrecision;
  public:
  Base Im;
  Base Re;
  std::string ElementToString()
  { std::stringstream tempStream;
    tempStream << *this;
    return tempStream.str();
  }
  void ElementToString(std::string& output){ output=this->ElementToString(); }
  friend std::iostream& operator<< <Base>(std::iostream& output, const CompleX<Base>& input);
  void operator*=(const CompleX<Base>& other)
  { CompleX Accum;
    Accum.Re=this->Re*other.Re-this->Im*other.Im;
    Accum.Im=this->Re*other.Im+ this->Im*other.Re;
    this->operator=(Accum);
  }
  void operator=(const CompleX<Base>& other)
  { this->Re=other.Re;
    this->Im=other.Im;
  }
  void operator+=(const CompleX<Base>& other)
  { this->Re+=other.Re;
    this->Im+=other.Im;
  }
  void operator-=(const CompleX<Base>& other)
  { this->Re-=other.Re;
    this->Im-=other.Im;
  }
  void operator=(int other)
  { this->Re=other;
    this->Im=0;
  }
  void operator=(double other)
  { this->Re=other;
    this->Im=0;
  }
  void Invert()
  { Base numerator;
    numerator=this->Re*this->Re+this->Im* this->Im;
    this->Re/=numerator;
    numerator*=-1;
    this->Im/=numerator;
  }
  bool IsEqualToZero()const
  { if(!CompleX<Base>::flagEqualityIsApproximate)
      return this->Im==0 && this->Re==0;
    else
      return
      this->Im<CompleX<Base>::EqualityPrecision && -this->Im<CompleX<Base>::EqualityPrecision
      &&
      this->Re<CompleX<Base>::EqualityPrecision && -this->Re<CompleX<Base>::EqualityPrecision
      ;
  }
  inline void Minus(){this->Im=-this->Im; this->Re=-this->Re;}
  CompleX(){}
  CompleX(int other){this->operator=(other);}
  CompleX(double other){this->operator=(other);}
};

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductElement;
template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductSpaceAndElements;

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductSpace
{
public:
  List<ElementLeft> leftSpaceBasis;
  List<ElementRight> rightSpaceBasis;
  roots leftWeights;
  roots rightWeights;
  CoefficientType theRingZerO;
  CoefficientType theRingUniT;
  std::string ElementToString
  (const PolynomialOutputFormat& theFormat)
  ;
  bool GetInternalRepresentationFromVectorLeft
  (const ElementLeft& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
  ;
  void GetInternalRepresentationFromVectorRight
  (const ElementRight& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
  ;
  void GetInternalRepresentationFromLeftAndRight
  (const ElementLeft& leftInput, const ElementRight& rightInput, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
  { Polynomial<CoefficientType> tempP;
    this->GetInternalRepresentationFromVectorLeft(leftInput, tempP, theGlobalVariables);
    this->GetInternalRepresentationFromVectorRight(rightInput, output, theGlobalVariables);
    output.MultiplyBy(tempP);
  }
  void GetEltFromIndex(int theIndex, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output, GlobalVariables& theGlobalVariables)
  { int leftIndex=theIndex/this->rightSpaceBasis.size;
    int rightIndex=theIndex% this->rightSpaceBasis.size;
    output.SetTensorProduct(this->leftSpaceBasis.TheObjects[leftIndex], this->rightSpaceBasis.TheObjects[rightIndex], *this, theGlobalVariables);
  }
  void GetLeftAndRightIndexFromMonomial(const Monomial<CoefficientType>& input, int& leftIndex, int& rightIndex)
  { for (int i=0; i<this->leftSpaceBasis.size; i++)
      if (input.degrees[i]==1)
      { leftIndex=i;
        break;
      }
    for (int i=0; i<this->rightSpaceBasis.size; i++)
      if (input.degrees[i+this->leftSpaceBasis.size]==1)
      { rightIndex=i;
        return;
      }
  }
  int GetIndexFromMonomial(const Monomial<CoefficientType>& input)
  { int leftIndex=-1; int rightIndex=-1;
    this->GetLeftAndRightIndexFromMonomial(input, leftIndex, rightIndex);
    assert(rightIndex!=-1 && leftIndex!=-1);
    return leftIndex*this->rightSpaceBasis.size+rightIndex;
  }
  void GetComponentsFromInternalRepresentation
  (const Monomial<CoefficientType>& input, ElementLeft& leftOutput, ElementRight& rightOutput)
  ;
};

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductElement
{
  //Format.
  //Let the owner TensorProductSpace be of the form V_1 \otimes V_2.
  //Then the internalRepresentation is a second degree homogeneous polynomial of dim V_1 +dim V_2 variables.
  //A monomial from the internalRepresentation must be of the form (coefficient)*x_i*x_j, where i<dim V_1 and j>=dim V_1.
  //Then the monomial (coefficient)*x_i*x_j, corresponds to the tensor product (coefficient)v_i\otimes v_k, where k=j-dim V_1,
  // v_i corresponds to the i^th member of TensorProductSpace::leftSpaceBasis and v_k correponds to the k^th member of TensorProductSpace::rightSpaceBasis
  friend class TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>;
  Polynomial<CoefficientType> internalRepresentation;
public:
  std::string DebugString;
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  std::string ElementToString();
  std::string ElementToString
  (TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner)
  ;
  void SetTensorProduct
  (const  ElementLeft& leftVector, const  ElementRight& rightVector, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
  ;
  void SetTensorProductBasisElement
  (int leftIndex, int rightIndex, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
  ;
  void ActOnMe
  (const ElementSimpleLieAlgebra& theElt, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theStartingSpace,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theTargetSpace,
   SemisimpleLieAlgebra& theAlgebra, GlobalVariables& theGlobalVariables)
  ;
  void ActOnMe
  (Polynomials<CoefficientType>& theSub, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output, const CoefficientType& theRingUnit)
  { this->internalRepresentation.Substitution(theSub, output.internalRepresentation, this->internalRepresentation.NumVars, theRingUnit);
  }
  void ActOnMe
  (Polynomials<CoefficientType>& theSub, const CoefficientType& theRingUnit)
  { this->internalRepresentation.Substitution(theSub, this->internalRepresentation.NumVars, theRingUnit);
  }
  void ElementToVector
  (Vector<CoefficientType>& output, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)const
  ;
  void AssignVector
  (Vector<CoefficientType>& input, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
  ;
  void Nullify(TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner){ this->internalRepresentation.Nullify(owner.leftSpaceBasis.size+owner.rightSpaceBasis.size);}
  void operator+=(const TensorProductElement& other){ this->internalRepresentation+=other.internalRepresentation; }
  void operator/=(const CoefficientType& theCoeff){ this->internalRepresentation/=theCoeff; }
  void operator-=(const TensorProductElement& other){ this->internalRepresentation.Subtract(other.internalRepresentation); }
  bool IsEqualToZero()const{return this->internalRepresentation.IsEqualToZero();}
};

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductSpaceAndElements: public List<TensorProductElement<ElementLeft, ElementRight, CoefficientType> >
{
  public:
  TensorProductSpace<ElementLeft, ElementRight, CoefficientType> theTargetSpace, theStartingSpace;
  List<Polynomials<CoefficientType> > simplePositiveGeneratorsActions;
  List<Polynomials<CoefficientType> > simpleNegativeGeneratorsActions;
  std::string DebugString;
  std::string ElementToString();
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> theSeedVector;
  root theRightLowestWeight;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  void initTheSpacesForAdjointAction
    (List<ElementSimpleLieAlgebra>& theElementsActing,
     SemisimpleLieAlgebra& theAlgebra,
     List<ElementLeft>& theLeftElts,
     List<ElementRight>& theRightElts,
     const ElementLeft& leftZero,
     const ElementRight& rightZero,
     const CoefficientType& theRingZero,
     const CoefficientType& theRingUnit,
     GlobalVariables& theGlobalVariables)
     ;
  std::string ExtractHighestWeightsCorrespondingToMaxVermaSummands
    (root& input, roots& outputVectors, List<MatrixLargeRational>& simplePosGenerators, List<MatrixLargeRational>& simpleNegGenerators,
     GlobalVariables& theGlobalVariables)
;
  std::string ExtractHighestWeightVectorsFromVector
( TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input, List<TensorProductElement<ElementLeft, ElementRight, CoefficientType> >& outputVectors,
  GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
  ;
  void ClimbDownFromHighestWeightAlongSl2String
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
    CoefficientType& outputCoeff,
    Polynomials<CoefficientType>& posGenerator,
    Polynomials<CoefficientType>& negGenerator, int generatorPower, const CoefficientType& theRingUnit)
  ;
  void ClimbDownFromVectorAccordingToSequence
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& outputLastNonZero, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
  ;
  void ClimbUpFromVector
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
  ;

  void initTheSpacesSl2StringVersion
  ( List<ElementSimpleLieAlgebra>& theSimplePosElts,  List<ElementSimpleLieAlgebra>& theSimpleNegElts, SemisimpleLieAlgebra& theAlgebra,
    const ElementLeft& leftZero, const ElementRight& rightZero, const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
    GlobalVariables& theGlobalVariables, const PolynomialOutputFormat& theFormat)
     ;
  void GetRootForm(roots& output, GlobalVariables& theGlobalVariables);
  bool FindEigenVectorsWithRespectTo
  (List<ElementSimpleLieAlgebra>& theElementsActing,  List<ElementLeft>& theLeftElts, List<ElementRight>& theRightElts,
   SemisimpleLieAlgebra& theAlgebra, TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>& output,
   const ElementLeft& leftZero, const ElementRight& rightZero,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
  ;
  bool FindEigenVectorsWithRespectToSl2StringVersion
  (List<ElementSimpleLieAlgebra>& theSimplePosElts,  List<ElementSimpleLieAlgebra>& theSimpleNegElts,
   SemisimpleLieAlgebra& theAlgebra, TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>& output,
   const ElementLeft& leftZero, const ElementRight& rightZero,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
  ;
  void PrepareTheMatrix
  (List<ElementSimpleLieAlgebra>& theElementsActing, SemisimpleLieAlgebra& theAlgebra, Matrix<CoefficientType>& output, GlobalVariables& theGlobalVariables)
  ;
};

class TranslationFunctorGmodKVermaModule
{
public:
  List<TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> > theElements;
  List<TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> > theEigenVectors;
  TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> theSpace;
  List<TensorProductElement<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> >  theOutput;
  roots theModuleWeightsShifted;
  roots theModuleWeightsNonShifted;
  List<ElementSimpleLieAlgebra> ElementsOfKofNegativeWeights;
  List<ElementVermaModuleOrdered<RationalFunction> > leftComponents;
  List<ElementSimpleLieAlgebra> rightComponents;
  List<List<ElementVermaModuleOrdered<RationalFunction> > > theLeftComponentsByWeight;
  List<List<ElementSimpleLieAlgebra> > theRightComponentsByWeight;
  VectorPartition theVectorPartition;
  PolynomialsRationalCoeff HighestWeightSub;
  std::string RunTheComputationSl2StringVersion
  (Parser& theParser, GlobalVariables& theGlobalVariables, EigenVectorComputation& owner)
  ;

  std::string RunTheComputation
  (Parser& theParser, GlobalVariables& theGlobalVariables, EigenVectorComputation& owner)
  ;
  std::string initTheWeights
    (Parser& theParser, GlobalVariables& theGlobalVariables)
  ;

  std::string initTheCandidateMonomials
    (Parser& theParser, GlobalVariables& theGlobalVariables)
  ;
  std::string initTheCandidateMonomialsSl2StringVersion
    (Parser& theParser, GlobalVariables& theGlobalVariables)
  ;
  std::string GetLeftAndRightFromVP
  (int indexTotalWeight, int indexRightWeight, VectorPartition& theVP, Parser& theParser, GlobalVariables& theGlobalVariables)
 ;
  std::string GetLeftFromVP
  ( VectorPartition& theVP, Parser& theParser, GlobalVariables& theGlobalVariables)
 ;
};

template<class CoefficientType>
class GeneralizedVermaModuleData
{
  public:
  List<ElementSimpleLieAlgebra> theFDspace;
  Vectors<Rational> smallerCartanEmbedding;

  SemisimpleLieAlgebraOrdered *theOwner;
  PolynomialsRationalCoeff VermaHWSubNthElementImageNthCoordSimpleBasis;
  roots theFDspaceWeights;
  //The order of the below is chosen so that the opposite generator of posGenerators.TheObjects[i] is negGenerators.TheObjects[i]
  List<ElementSimpleLieAlgebra> posGenerators, negGenerators;
  CoefficientType theRingUnit;
  CoefficientType theRingZero;
  GeneralizedVermaModuleData()
  { this->theOwner=0;
  }
  void init(Parser& input, GlobalVariables* theContext, const CoefficientType& ringUnit, const CoefficientType& ringZero)
  { this->theRingUnit=ringUnit;
    this->theRingZero=ringZero;
    this->posGenerators.size=0; this->negGenerators.size=0;
    int domainRank=input.theHmm.theDomain.GetRank();
    int numDomainPosRoots=input.theHmm.theDomain.GetNumPosRoots();
    this->smallerCartanEmbedding.SetSize(domainRank);
    for (int i=0; i<domainRank; i++)
    { this->posGenerators.AddOnTop(input.theHmm.imagesAllChevalleyGenerators.TheObjects
        [domainRank+numDomainPosRoots+i]);
      this->negGenerators.AddOnTop(input.theHmm.imagesAllChevalleyGenerators.TheObjects
        [numDomainPosRoots-i-1]);
      this->smallerCartanEmbedding[i]=input.theHmm.imagesAllChevalleyGenerators[numDomainPosRoots+i].Hcomponent;
    }
    this->theOwner=&input.testAlgebra;
    this->theFDspace=input.theHmm.GmodK;
    input.theHmm.GetWeightsGmodKInSimpleCoordsK(this->theFDspaceWeights, *theContext);
    this->VermaHWSubNthElementImageNthCoordSimpleBasis.MakeIdSubstitution(input.theHmm.theRange.GetRank(), (Rational) 1);
  }
};

template<class CoefficientType>
class ElementGeneralizedVerma
{
  public:
  std::string DebugString;
  std::string ElementToString(PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
  { return this->ElementToString(true, false, true, theFormat, theGlobalVariables);
  }
  std::string ElementToString
(bool displayLeftComponenetsOnTheRight, bool useOtimes, bool useFs,
 PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables
)
;
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  GeneralizedVermaModuleData<CoefficientType>* theOwner;
  List<ElementVermaModuleOrdered<CoefficientType> > leftComponents;
  ElementGeneralizedVerma(){this->theOwner=0;}
  std::string Decompose
  (Parser& theParser, List<ElementGeneralizedVerma<CoefficientType> >& outputVectors,
    GlobalVariables& theGlobalVariables)
;
  void Nullify
  (GeneralizedVermaModuleData<CoefficientType>& owner)
    ;
  bool AssignElementUE
  (ElementUniversalEnvelopingOrdered<CoefficientType>& theElt,
   GeneralizedVermaModuleData<CoefficientType>& owner,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
      ;
  void AssignDefaultGeneratorIndex
  (int theIndex, GeneralizedVermaModuleData<CoefficientType>& owner, GlobalVariables* theContext)
  { this->Nullify(owner);
    ElementVermaModuleOrdered<CoefficientType>& currentElt=this->leftComponents.TheObjects[theIndex];
    ElementVermaModuleOrdered<CoefficientType> zeroVermaElt;
    assert(this->theOwner->theOwner!=0);
    zeroVermaElt.Nullify(*this->theOwner->theOwner, this->theOwner->VermaHWSubNthElementImageNthCoordSimpleBasis);
    ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
    tempElt.MakeConst(this->theOwner->theRingUnit, *this->theOwner->theOwner);
    currentElt.AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
    (tempElt, zeroVermaElt, theContext, this->theOwner->theRingUnit)
    ;
    currentElt.theSubNthElementIsImageNthCoordSimpleBasis=
    this->theOwner->VermaHWSubNthElementImageNthCoordSimpleBasis;
  }
  std::string GetStringFormGmodKindex
(int theIndex, PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
  ;
  void ActOnMe
  (const ElementSimpleLieAlgebra& theElt, ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
;
template<class CoefficientActingElement>
  bool ActOnMe
  (const ElementUniversalEnvelopingOrdered<CoefficientActingElement>& theElt,
   ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
;
  void ActOnMe(const ElementSimpleLieAlgebra& theElt, GlobalVariables* theContext) {ElementGeneralizedVerma<CoefficientType> output; this->ActOnMe(theElt, output, theContext); this->operator=(output);};
  void ClimbDownFromHighestWeightAlongSl2String
(ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& output,
    CoefficientType& outputCoeff,
   const ElementSimpleLieAlgebra& posGenerator, const ElementSimpleLieAlgebra& negGenerator,
     int generatorPower, GlobalVariables& theGlobalVariables)
  ;
  void ClimbDownFromVectorAccordingToSequence
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& outputLastNonZero, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables)
  ;
  void ClimbUpFromVector
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& outputLastNonZero,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
  ;
  void operator=(const ElementGeneralizedVerma<CoefficientType>& other)
  { this->leftComponents=other.leftComponents;
    this->theOwner=other.theOwner;
  }
  bool IsEqualToZero()const
  { for (int i=0; i<this->leftComponents.size; i++)
      if (!this->leftComponents.TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
  void operator/=(const CoefficientType& theConst)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]/=theConst;
  }
  template<class CoefficientOther>
  void operator*=(const CoefficientOther& theConst)
  { if (theConst.IsEqualToZero())
    { this->Nullify(*this->theOwner);
      return;
    }
    for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]*=theConst;
  }
  void operator-=(const ElementGeneralizedVerma<CoefficientType>& other)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]-=other.leftComponents.TheObjects[i];
  }
  void operator+=(const ElementGeneralizedVerma<CoefficientType>& other)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]+=other.leftComponents.TheObjects[i];
  }
  bool IsProportionalTo(const ElementGeneralizedVerma<CoefficientType>& other, CoefficientType& outputTimesMeEqualsInput)const
  { assert(this->leftComponents.size==other.leftComponents.size);
    if (this->IsEqualToZero())
    { if (other.IsEqualToZero())
      { outputTimesMeEqualsInput=this->theOwner->theRingZero;
        return true;
      }
      return false;
    }
    for (int i=0; i<this->leftComponents.size; i++)
      if (!this->leftComponents.TheObjects[i].IsEqualToZero())
      { if (!this->leftComponents.TheObjects[i].IsProportionalTo(other.leftComponents.TheObjects[i], outputTimesMeEqualsInput, this->theOwner->theRingZero))
          return false;
        else
          break;
      }
    ElementGeneralizedVerma<CoefficientType> tempElt;
    tempElt=*this;
    tempElt*=outputTimesMeEqualsInput;
    tempElt-=other;
    return tempElt.IsEqualToZero();
  }
  void ClearDenominators
  (CoefficientType& outputWasMultipliedByThis)
  ;
};

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ClimbDownFromHighestWeightAlongSl2String
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
    CoefficientType& outputCoeff,
    Polynomials<CoefficientType>& posGenerator,
    Polynomials<CoefficientType>& negGenerator, int generatorPower, const CoefficientType& theRingUnit)
{ assert(&input!=&output);
  CoefficientType currentWeight;
  input.ActOnMe(negGenerator, output, theRingUnit);
  output.ActOnMe(posGenerator, theRingUnit);
  input.internalRepresentation.IsProportionalTo(output.internalRepresentation, currentWeight, theRingUnit);
  CoefficientType RaiseCoeff;
  RaiseCoeff=this->theTargetSpace.theRingZerO;
  outputCoeff=theRingUnit;
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    output.ActOnMe(negGenerator, output, theRingUnit);
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ClimbDownFromVectorAccordingToSequence
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
{ assert(&input!=&output);
  output=input;
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> tempElt;
  outputCoeff=theRingUnit;
  CoefficientType tempCoeff;
  for (int i=inputGeneratorSequence.size-1; i>=0; i--)
  { Polynomials<CoefficientType>& currentPosGen= this->simplePositiveGeneratorsActions.TheObjects[inputGeneratorSequence.TheObjects[i]];
    Polynomials<CoefficientType>& currentNegGen=this->simpleNegativeGeneratorsActions.TheObjects[inputGeneratorSequence.TheObjects[i]];
    this->ClimbDownFromHighestWeightAlongSl2String
    (output, tempElt, tempCoeff, currentPosGen, currentNegGen, inputGeneratorPowers.TheObjects[i], theRingUnit);
    outputCoeff*=(tempCoeff);
    output=tempElt;
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ClimbUpFromVector
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& outputLastNonZero,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
{ TensorProductElement<ElementLeft, ElementRight, CoefficientType> tempElt;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  outputGeneratorPowers.size=0;
  outputGeneratorSequence.size=0;
  bool found=true;
  while (found)
  { found=false;
    for (int i=0; i<this->simplePositiveGeneratorsActions.size; i++)
    { Polynomials<CoefficientType>& currentAction=this->simplePositiveGeneratorsActions.TheObjects[i];
      int counter=0;
      for(outputLastNonZero.ActOnMe(currentAction, tempElt, this->theTargetSpace.theRingUniT); !tempElt.IsEqualToZero(); tempElt.ActOnMe(currentAction, this->theTargetSpace.theRingUniT))
      { counter++;
        found=true;
        outputLastNonZero=tempElt;
//        std::cout << "<br>" << outputLastNonZero.ElementToString();
//        std::cout.flush();
      }
      if (found)
      { outputGeneratorSequence.AddOnTop(i);
        outputGeneratorPowers.AddOnTop(counter);
      }
    }
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ExtractHighestWeightVectorsFromVector
( TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input, List<TensorProductElement<ElementLeft, ElementRight, CoefficientType> >& outputVectors,
  GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
{ //Index ordering of simpleNegGenerators:
  //if simplePosGenerators.TheObjects[i] is a positive root space then its opposite root space should be
  //simpleNegGenerators.TheObjects[i]
  std::stringstream out;
  outputVectors.size=0;
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> remainderElement, componentElement, currentHighestWeightElement;
  remainderElement=input;
  CoefficientType theCoeff;
  List<int> GeneratorSequence, GeneratorPowers;
  while(!remainderElement.IsEqualToZero())
  { this->ClimbUpFromVector
      (remainderElement, currentHighestWeightElement,
       GeneratorSequence, GeneratorPowers, theGlobalVariables);
    this->ClimbDownFromVectorAccordingToSequence
      (currentHighestWeightElement, componentElement, theCoeff, GeneratorSequence, GeneratorPowers, theGlobalVariables, theRingUnit);
    assert(!theCoeff.IsEqualToZero());
    componentElement/=theCoeff;
    outputVectors.AddOnTop(componentElement);
    remainderElement-=*outputVectors.LastObject();
    std::cout << "<br>found element: " << componentElement.ElementToString();
  }
  return out.str();
}

template<class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::Nullify
  (GeneralizedVermaModuleData<CoefficientType>& owner)
{ this->theOwner=&owner;
  this->leftComponents.SetSize(owner.theFDspace.size);
  for (int i=0; i<this->leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentElt=this->leftComponents.TheObjects[i];
    currentElt.Nullify(*this->theOwner->theOwner, this->theOwner->VermaHWSubNthElementImageNthCoordSimpleBasis);
  }
}

template <class CoefficientType>
std::string ElementGeneralizedVerma<CoefficientType>::GetStringFormGmodKindex
(int theIndex, PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  bool tempbool;
  if (this->theOwner->theOwner->theOwner.theWeyl.WeylLetter=='B' && this->theOwner->theOwner->theOwner.theWeyl.GetDim()==3)
  { if (theIndex<3)
      theIndex+=6;
    else if (theIndex==3)
      theIndex=11;
    else
      theIndex+=8;
  }
  return this->theOwner->theOwner->GetGeneratorString
    (theIndex, false, tempbool, theFormat, theGlobalVariables)
    ;
}

template <class CoefficientType>
std::string ElementGeneralizedVerma<CoefficientType>::ElementToString
(bool displayLeftComponenetsOnTheRight, bool useOtimes, bool useFs,
 PolynomialOutputFormat& theFormat, GlobalVariables& theGlobalVariables
)
{ if (this->IsEqualToZero())
    return "0";
  std::stringstream out; std::string leftString, rightString;
  bool foundFirstNonZero=false;
  for (int i=0; i<this->leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentElt= this->leftComponents.TheObjects[i];
    if (!currentElt.IsEqualToZero())
    { if (foundFirstNonZero)
        out << "+";
      foundFirstNonZero=true;
      leftString=MathRoutines::ElementToStringBrackets(currentElt, theFormat);
      if (!useFs)
        rightString=MathRoutines::ElementToStringBrackets(this->theOwner->theFDspace.TheObjects[i]);
      else
        rightString= this->GetStringFormGmodKindex(i, theFormat, theGlobalVariables);
      if (displayLeftComponenetsOnTheRight)
        MathRoutines::swap(leftString, rightString);
      out << leftString;
      if (useOtimes)
        out << "\\otimes";
      out << rightString;
    }
  }
  return out.str();
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClimbDownFromHighestWeightAlongSl2String
(ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& output,
    CoefficientType& outputCoeff,
   const ElementSimpleLieAlgebra& posGenerator, const ElementSimpleLieAlgebra& negGenerator,
     int generatorPower, GlobalVariables& theGlobalVariables)
{ assert(&input!=&output);
  CoefficientType currentWeight;
  ElementGeneralizedVerma<CoefficientType> debugElt;
  input.ActOnMe(posGenerator, debugElt, &theGlobalVariables);
  std::cout << "<br>climbing down with (" << negGenerator.ElementToString() << " )^" << generatorPower << ": ";
  std::cout << input.ElementToString() << "->";
  //if (!debugElt.IsEqualToZero())
  //{ std::cout << "<br>positive generator acting on the supposedly highest weight vector equals: " << debugElt.ElementToString();
  //  std::cout.flush();
  //  assert(false);
  //}
  input.ActOnMe(negGenerator, output, &theGlobalVariables);
  std::cout << output.ElementToString() << "->";
  output.ActOnMe(posGenerator, &theGlobalVariables);
  std::cout << output.ElementToString() << "->";
  std::cout.flush();
  bool tempBool=input.IsProportionalTo(output, currentWeight);
  assert(tempBool);
  CoefficientType RaiseCoeff;
  RaiseCoeff=this->theOwner->theRingZero;
  outputCoeff=this->theOwner->theRingUnit;
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    output.ActOnMe(negGenerator, &theGlobalVariables);

  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClimbUpFromVector
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& outputLastNonZero,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
{ ElementGeneralizedVerma<CoefficientType> tempElt;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  outputGeneratorPowers.size=0;
  outputGeneratorSequence.size=0;
  bool found=true;
  std::stringstream foundGenerators;
  foundGenerators << "Non-zero acting generators sequence: ";
  while (found)
  { for (int i=0; i<this->theOwner->posGenerators.size; i++)
    { ElementSimpleLieAlgebra& currentGen=this->theOwner->posGenerators.TheObjects[i];
      int counter=0;
      found=false;
      std::stringstream reportStream;
      reportStream << " Acting by " << " g_" << i+1;
      theGlobalVariables.MakeProgressReport(reportStream.str(),1);
      for(outputLastNonZero.ActOnMe(currentGen, tempElt, &theGlobalVariables); !tempElt.IsEqualToZero(); tempElt.ActOnMe(currentGen, &theGlobalVariables))
      { counter++;
        found=true;
        outputLastNonZero=tempElt;
  //      std::cout << "<br> Current state of the element: " << outputLastNonZero.ElementToString();
    //    std::cout.flush();
      }
      if (found)
      { outputGeneratorSequence.AddOnTop(i);
        outputGeneratorPowers.AddOnTop(counter);
        foundGenerators << "g_" <<  i+1 << "^" << counter;
        theGlobalVariables.MakeProgressReport(foundGenerators.str(), 0);
      }
    }
  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ActOnMe
(const ElementSimpleLieAlgebra& theElt, ElementGeneralizedVerma<CoefficientType>& output,
 GlobalVariables* theContext)
{ assert(this!=&output);
  ElementGeneralizedVerma<CoefficientType> actingOnRight;
  output.Nullify(*this->theOwner);
  actingOnRight.Nullify(*this->theOwner);
  ElementSimpleLieAlgebra tempLieElt;
  Vector<RationalFunction> coordFormRightElt;
  for (int i=0; i<output.leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentLeft= this->leftComponents.TheObjects[i];
    currentLeft.ActOnMe(theElt, output.leftComponents.TheObjects[i], this->theOwner->theOwner->theOwner, this->theOwner->theRingUnit, this->theOwner->theRingZero, theContext);
    currentLeft.MultiplyOnTheLeft(theElt, output.leftComponents.TheObjects[i], this->theOwner->theRingUnit, this->theOwner->theRingZero, theContext);
  }
//  std::cout << "<br><br>acting on the Universal enveloping part we get:" << output.ElementToString();
  ElementVermaModuleOrdered<CoefficientType> tempElt;
  for (int i=0; i<output.leftComponents.size; i++)
  { ElementSimpleLieAlgebra& currentAlgebraElt=this->theOwner->theFDspace.TheObjects[i];
    this->theOwner->theOwner->theOwner.LieBracket(theElt, currentAlgebraElt, tempLieElt);
    //std::cout << "<br><br>" << theElt.ElementToString() << " acts on " << currentAlgebraElt.ElementToString() << " to obtain: " << tempLieElt.ElementToString();

    bool tempBool=tempLieElt.GetCoordsInBasis(this->theOwner->theFDspace, coordFormRightElt, *theContext);
    //std::cout << "<br>translated in coordinate form this is: " << coordFormRightElt.ElementToString();
    assert(tempBool);
    for (int j=0; j<coordFormRightElt.size; j++)
    { tempElt=this->leftComponents.TheObjects[i];
      tempElt*=(coordFormRightElt.TheObjects[j]);
      output.leftComponents.TheObjects[j]+=tempElt;
    }
  }
//  std::cout<< "<br><br>finally we have: " << output.ElementToString() << " <br>";

}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClimbDownFromVectorAccordingToSequence
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& output, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables)
 { assert(&input!=&output);
  output=input;
  ElementGeneralizedVerma<CoefficientType> tempElt;
  outputCoeff=this->theOwner->theRingUnit;
  CoefficientType tempCoeff;
  for (int i=inputGeneratorSequence.size-1; i>=0; i--)
  { ElementSimpleLieAlgebra& currentPosGen= this->theOwner->posGenerators.TheObjects[inputGeneratorSequence.TheObjects[i]];
    ElementSimpleLieAlgebra& currentNegGen=this->theOwner->negGenerators.TheObjects[inputGeneratorSequence.TheObjects[i]];
    this->ClimbDownFromHighestWeightAlongSl2String
    (output, tempElt, tempCoeff, currentPosGen, currentNegGen, inputGeneratorPowers.TheObjects[i], theGlobalVariables);
    outputCoeff*=(tempCoeff);
//    std::cout << "<br><br><br>... and the coefficient currently is: " << outputCoeff.ElementToString();
    output=tempElt;
  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClearDenominators
  (CoefficientType& outputWasMultipliedByThis)
{ outputWasMultipliedByThis=this->theOwner->theRingUnit;
  CoefficientType CurrentCoeff;
  for (int i=0; i<this->leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentElt=this->leftComponents.TheObjects[i];
    currentElt.ClearDenominators(CurrentCoeff, this->theOwner->theRingUnit);
    for (int j=0; j<this->leftComponents.size; j++)
      if (i!=j)
        this->leftComponents.TheObjects[j]*=CurrentCoeff;
    outputWasMultipliedByThis*=CurrentCoeff;
  }
}

template <class CoefficientType>
template<class CoefficientActingElement>
bool ElementGeneralizedVerma<CoefficientType>::ActOnMe
  (const ElementUniversalEnvelopingOrdered<CoefficientActingElement>& theElt,
   ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
{ ElementGeneralizedVerma<CoefficientType> Accum, tempElt;
  Accum.Nullify(*this->theOwner);
  for (int i=0; i<theElt.size; i++)
  { MonomialUniversalEnvelopingOrdered<CoefficientActingElement>& currentMon=theElt.TheObjects[i];
    tempElt=*this;
    for (int j=currentMon.generatorsIndices.size-1; j>=0 ; j--)
    { int thePower;
      if (!currentMon.Powers.TheObjects[j].IsSmallInteger(thePower))
        return false;
      if (theContext!=0)
      { std::stringstream reportStream;
        reportStream << "acting by monomial " << i+1 << " out of " << theElt.size << " by the letter of index " << j
        << " out of  " << currentMon.generatorsIndices.size << " letters in the monomial";
        theContext->MakeProgressReport(reportStream.str(), 1);
      }
      ElementSimpleLieAlgebra& currentLieElt=
      this->theOwner->theOwner->theOrder.TheObjects[currentMon.generatorsIndices.TheObjects[j]];
      for (int k=0; k<thePower; k++)
        tempElt.ActOnMe(currentLieElt, theContext);
    }
    tempElt.operator*=<CoefficientActingElement>(currentMon.Coefficient);
    Accum+=tempElt;
  }
  output=Accum;
  return true;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::GetInternalRepresentationFromVectorRight
  (const ElementRight& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
{ output.Nullify(this->leftSpaceBasis.size+this->rightSpaceBasis.size);
  Monomial<CoefficientType> tempMon;
  Vector<CoefficientType> vectorFormInput;
  input.GetCoordsInBasis(this->rightSpaceBasis, vectorFormInput, theGlobalVariables);
  for(int i=0; i<vectorFormInput.size; i++)
    if (!vectorFormInput.TheObjects[i].IsEqualToZero())
    { tempMon.MakeMonomialOneLetter(output.NumVars, i+this->leftSpaceBasis.size, 1, vectorFormInput.TheObjects[i]);
      output.AddMonomial(tempMon);
    }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
bool TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::GetInternalRepresentationFromVectorLeft
  (const ElementLeft& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
{ output.Nullify(this->leftSpaceBasis.size+this->rightSpaceBasis.size);
  Monomial<CoefficientType> tempMon;
  Vector<CoefficientType> vectorFormInput;
//  std::string tempS=input.ElementToString();
//  std::string tempS2=this->leftSpaceBasis.ElementToStringGeneric();
  if (! input.GetCoordsInBasis(this->leftSpaceBasis, vectorFormInput, this->theRingUniT, this->theRingZerO, theGlobalVariables))
    return false;
//  vectorFormInput.ComputeDebugString();
  for(int i=0; i<vectorFormInput.size; i++)
    if (!vectorFormInput.TheObjects[i].IsEqualToZero())
    { tempMon.MakeMonomialOneLetter(output.NumVars, i, 1, vectorFormInput.TheObjects[i]);
      output.AddMonomial(tempMon);
    }
  return true;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::
GetComponentsFromInternalRepresentation
  (const Monomial<CoefficientType>& input, ElementLeft& leftOutput, ElementRight& rightOutput)
{ for (int i=0; i<this->leftSpaceBasis.size; i++)
    if (input.degrees[i]==1)
    { leftOutput.operator=(this->leftSpaceBasis.TheObjects[i]);
      break;
    }
  for (int i=0; i<this->rightSpaceBasis.size; i++)
    if (input.degrees[i+leftSpaceBasis.size]==1)
    { rightOutput.operator=(this->rightSpaceBasis.TheObjects[i]);
      break;
    }
  leftOutput.operator*=(input.Coefficient);

}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::ElementToString
(const PolynomialOutputFormat& theFormat)
{ std::stringstream out;
  out << "<br> Basis of the left components (" << this->leftSpaceBasis.size << " elements): <br>";
  for (int i=0; i<this->leftSpaceBasis.size; i++)
    out << this->leftSpaceBasis.TheObjects[i].ElementToString(theFormat) << "<br> ";
  out << "<br> Basis of the right components (" << this->rightSpaceBasis.size << " elements): <br>";
  for (int i=0; i<this->rightSpaceBasis.size; i++)
    out << this->rightSpaceBasis.TheObjects[i].ElementToString() << "<br> ";
  return out.str();
}

template<class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::GetBasisFromSpanOfElements
  (List<ElementVermaModuleOrdered>& theElements, Vectors<RationalFunction>& outputCoordinates, List<ElementVermaModuleOrdered>& outputTheBasis,
    const RationalFunction& RFOne, const RationalFunction& RFZero, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnvelopingOrdered<CoefficientType> > theEltsUEform;
  theEltsUEform.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
    theEltsUEform.TheObjects[i]=theElements.TheObjects[i].theElT;
  List<ElementUniversalEnvelopingOrdered<CoefficientType> > theBasisUEform;
  ElementUniversalEnvelopingOrdered<CoefficientType>::GetBasisFromSpanOfElements(theEltsUEform, outputCoordinates, theBasisUEform, RFOne, RFZero, theGlobalVariables);
  outputTheBasis.SetSize(theBasisUEform.size);
  for (int i=0; i<theBasisUEform.size; i++)
  { outputTheBasis.TheObjects[i].theElT=theBasisUEform.TheObjects[i];
    outputTheBasis.TheObjects[i].theSubNthElementIsImageNthCoordSimpleBasis=theElements.TheObjects[0].theSubNthElementIsImageNthCoordSimpleBasis;
  }
}

template<class CoefficientType>
bool ElementVermaModuleOrdered<CoefficientType>::GetCoordsInBasis
  (const List<ElementVermaModuleOrdered<CoefficientType> >& theBasis, Vector<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables& theGlobalVariables)const
{ List<ElementUniversalEnvelopingOrdered<CoefficientType> > theEltsUEform;
  theEltsUEform.SetSize(theBasis.size);
  for (int i=0; i<theBasis.size; i++)
  { theEltsUEform.TheObjects[i]=theBasis.TheObjects[i].theElT;
  }
  return this->theElT.GetCoordsInBasis(theEltsUEform, output, theRingUnit, theRingZero, theGlobalVariables);
}

template<class CoefficientType>
bool ElementVermaModuleOrdered<CoefficientType>::ElementToStringNeedsBracketsForMultiplication()const
{ return this->theElT.ElementToStringNeedsBracketsForMultiplication();
}

template <class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
  (ElementUniversalEnvelopingOrdered<CoefficientType>& input, const ElementVermaModuleOrdered<CoefficientType>& theRingZero, GlobalVariables* theContext, const CoefficientType& theRingUnit)
{ this->theElT.operator=(input);
  assert(theRingZero.theSubNthElementIsImageNthCoordSimpleBasis.size==3);
  this->theSubNthElementIsImageNthCoordSimpleBasis=theRingZero.theSubNthElementIsImageNthCoordSimpleBasis;
  this->theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theContext, theRingUnit);
}

template <class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::MultiplyOnTheLeft
(const ElementSimpleLieAlgebra& other, ElementVermaModuleOrdered<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  tempElt.AssignElementLieAlgebra(other, theRingUnit, theRingZero, *this->theElT.owner);
  //std::cout << "<br>multiplying " << tempElt.ElementToString() << " times " << this->ElementToString();
  tempElt*=this->theElT;
  output.theElT=tempElt;
  //std::cout << "<br> ... and the result before simplifying is: " << output.theElT.ElementToString();
  output.theElT.Simplify(theContext, theRingUnit, theRingZero);
  //std::cout << "<br> before modding out we get: " << output.theElT.ElementToString();
  output.theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theContext, theRingUnit);
  //std::cout << "<br> finally we get: " << output.theElT.ElementToString();
}

template <class CoefficientType>
std::string ElementVermaModuleOrdered<CoefficientType>::ElementToString(const PolynomialOutputFormat& theFormat)const
{ std::stringstream out;
  std::string tempS=MathRoutines::ElementToStringBrackets(this->theElT, theFormat);
  if (tempS.size()>1)
    out << "(";
  if (tempS!="1")
    out << tempS;
  if (tempS.size()>1)
    out << ")";
  if (tempS!="0")
    out << " v";
  return out.str();
}

template<class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::ActOnMe
  (const ElementSimpleLieAlgebra& actingElt, ElementVermaModuleOrdered<CoefficientType>& output, SemisimpleLieAlgebra& owner,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)const
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  tempElt.AssignElementLieAlgebra(actingElt, theRingUnit, theRingZero, *this->theElT.owner);
//  std::cout << "<br>" << actingElt.ElementToString() << " acts on " << this->ElementToString();
  tempElt.LieBracketOnTheRight(this->theElT, output.theElT);
  output.theElT.Simplify(theContext, theRingUnit, theRingZero);
  output.theSubNthElementIsImageNthCoordSimpleBasis=this->theSubNthElementIsImageNthCoordSimpleBasis;
//  std::cout << "<br>and the result before modding out is: " << output.ElementToString();
//  int numVars=output.theElt.GetNumVariables();
  output.theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theContext, theRingUnit);
//  std::cout << "<br>and after modding out we get: " << output.ElementToString();
}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ElementToString()
{ std::stringstream out;
  PolynomialOutputFormat tempFormat;
  out << "The elements (" << this->size << ") are: ";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString() << "<br>";
  }
  out << "starting space: " << this->theStartingSpace.ElementToString(tempFormat) << "<br>target space: "
  << this->theTargetSpace.ElementToString(tempFormat);
  return out.str();
}

template <class ElementLeft, class ElementRight, class CoefficientType>
bool TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::FindEigenVectorsWithRespectTo
  (List<ElementSimpleLieAlgebra>& theElementsActing,  List<ElementLeft>& theLeftElts, List<ElementRight>& theRightElts,
   SemisimpleLieAlgebra& theAlgebra, TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>& output,
   const ElementLeft& leftZero, const ElementRight& rightZero,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
{ Matrix<RationalFunction> theSystem;
  this->initTheSpacesForAdjointAction(theElementsActing, theAlgebra, theLeftElts, theRightElts, leftZero, rightZero, theRingUnit, theRingZero, theGlobalVariables);
//  std::cout << "<br><br>the starting space: " << this->theStartingSpace.ElementToString();
//  std::cout << "<br><br>the target space: " << this->theTargetSpace.ElementToString();
  this->PrepareTheMatrix(theElementsActing, theAlgebra, theSystem, theGlobalVariables);
  return false;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::initTheSpacesForAdjointAction
    (List<ElementSimpleLieAlgebra>& theElementsActing,
     SemisimpleLieAlgebra& theAlgebra,
     List<ElementLeft>& theLeftElts,
     List<ElementRight>& theRightElts,
     const ElementLeft& leftZero,
     const ElementRight& rightZero,
     const CoefficientType& theRingUnit,
     const CoefficientType& theRingZero,
     GlobalVariables& theGlobalVariables)
{ List<ElementLeft> tempLeftElts;
  List<ElementRight> tempRightElts;
  Vectors<RationalFunction> tempCoords;
  ElementLeft tempLeft;
  ElementRight tempRight;
  this->SetSize(theLeftElts.size);
  for (int i=0; i<theLeftElts.size; i++)
    for (int j=0; j<theElementsActing.size; j++)
    { assert(theLeftElts.TheObjects[i].theSubNthElementIsImageNthCoordSimpleBasis.size==3);
      theLeftElts.TheObjects[i].ActOnMe(theElementsActing.TheObjects[j], tempLeft, theAlgebra, theRingUnit, theRingZero, &theGlobalVariables);
      tempLeftElts.AddOnTop(tempLeft);
      theRightElts.TheObjects[i].ActOnMe(theElementsActing.TheObjects[j], tempRight, theAlgebra);
      tempRightElts.AddOnTop(tempRight);
      tempRight.ComputeDebugString( false, false);
      tempLeft.ComputeDebugString();
    }
  this->theStartingSpace.theRingUniT=theRingUnit;
  this->theTargetSpace.theRingUniT=theRingUnit;
  this->theStartingSpace.theRingZerO=theRingZero;
  this->theTargetSpace.theRingZerO=theRingZero;
  tempLeftElts.AddListOnTop(theLeftElts);
  tempRightElts.AddListOnTop(theRightElts);
//  std::cout << "<br><br><br> before extracting a basis the new left elements are:<br>";
//  for (int i=0; i<tempLeftElts.size; i++)
//    std::cout << tempLeftElts.TheObjects[i].ElementToString() << "<br>";
//  std::cout << "<br> before extracting a basis the new right elements are:<br>";
  for (int i=0; i<tempRightElts.size; i++)
    std::cout << tempRightElts.TheObjects[i].ElementToString() << "<br>";
  std::stringstream out;
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
    out << "Obtaining basis on the *target* left vector space...";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementLeft::GetBasisFromSpanOfElements(tempLeftElts, tempCoords, this->theTargetSpace.leftSpaceBasis, theRingUnit, theRingZero, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\nObtaining basis on the *target* right vector space...";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementRight::GetBasisFromSpanOfElements(tempRightElts, tempCoords, this->theTargetSpace.rightSpaceBasis, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\nObtaining basis on the *starting* left vector space";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementLeft::GetBasisFromSpanOfElements(theLeftElts, tempCoords, this->theStartingSpace.leftSpaceBasis, theRingUnit, theRingZero, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\nObtaining basis on the *starting* right vector space";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementRight::GetBasisFromSpanOfElements(theRightElts, tempCoords, this->theStartingSpace.rightSpaceBasis, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\n";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
//  std::cout << "<br> starting space LOOK here: " << this->theStartingSpace.ElementToString();//
//  std::cout << "<br> target space LOOK here: " << this->theTargetSpace.ElementToString();
//  std::cout << "<br> the elements we are acting upon are (" << theLeftElts.size << "): ";
  for (int i=0; i<theLeftElts.size; i++)
  { this->TheObjects[i].SetTensorProduct(theLeftElts.TheObjects[i], theRightElts.TheObjects[i], this->theStartingSpace, theGlobalVariables);
    std::cout << this->TheObjects[i].ElementToString() << ", ";
  }

}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::initTheSpacesSl2StringVersion
  ( List<ElementSimpleLieAlgebra>& theSimplePosElts,  List<ElementSimpleLieAlgebra>& theSimpleNegElts, SemisimpleLieAlgebra& theAlgebra,
    const ElementLeft& leftZero, const ElementRight& rightZero, const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
    GlobalVariables& theGlobalVariables, const PolynomialOutputFormat& theFormat)
{ this->theTargetSpace.theRingUniT=theRingUnit;
  this->theTargetSpace.theRingZerO=theRingZero;
  this->MakeActualSizeAtLeastExpandOnTop(this->theTargetSpace.leftSpaceBasis.size*this->theTargetSpace.rightSpaceBasis.size);
  this->size=0;
  std::stringstream de_Bugger;
  bool foundSeed=false;
  for (int i=0; i< this->theTargetSpace.leftSpaceBasis.size; i++)
    for (int j=0; j< this->theTargetSpace.rightSpaceBasis.size; j++)
    { this->ExpandOnTop(1);
      this->LastObject()->SetTensorProductBasisElement(i, j, this->theTargetSpace, theGlobalVariables);
      de_Bugger << this->TheObjects[i].ElementToString() << "  <br>\n";
      std::cout << "Element of index " << i << ", " << j << ": " << this->TheObjects[i].ElementToString() << "<br>";
      if(!foundSeed)
        if (this->theRightLowestWeight==this->theTargetSpace.rightWeights.TheObjects[j] && i==this->theTargetSpace.leftSpaceBasis.size-1)
          this->theSeedVector=*this->LastObject();
    }
  std::cout << this->theTargetSpace.ElementToString(theFormat);
  //return;

  ElementLeft resultLeft; ElementRight  resultRight;
  Vector<CoefficientType> theResultVector;
  this->simplePositiveGeneratorsActions.SetSize(theSimplePosElts.size);
  this->simpleNegativeGeneratorsActions.SetSize(theSimpleNegElts.size);
  PolynomialOutputFormat tempFormat;
  List<ElementSimpleLieAlgebra>* curentGeneratorCollection=&theSimplePosElts;
  for (int j=0; j<2; j++, curentGeneratorCollection=& theSimpleNegElts)
    for (int i=0; i<curentGeneratorCollection->size; i++)
    { ElementSimpleLieAlgebra& currentGenerator=curentGeneratorCollection->TheObjects[i];
      Polynomials<CoefficientType>* currentAction= (j==0)? (&this->simplePositiveGeneratorsActions.TheObjects[i]) : (&this->simpleNegativeGeneratorsActions.TheObjects[i]);
      currentAction->SetSize(this->theTargetSpace.leftSpaceBasis.size+this->theTargetSpace.rightSpaceBasis.size);
      for (int k=0; k<this->theTargetSpace.leftSpaceBasis.size; k++)
      { ElementLeft& currentLeftElt=this->theTargetSpace.leftSpaceBasis.TheObjects[k];
        Polynomial<CoefficientType>& currentPoly=currentAction->TheObjects[k];
        currentLeftElt.ActOnMe(currentGenerator, resultLeft, theAlgebra, this->theTargetSpace.theRingUniT, this->theTargetSpace.theRingZerO, &theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << " maps " <<
        currentLeftElt.ElementToString(tempFormat) << " to "
        << resultLeft.ElementToString(tempFormat);
        this->theTargetSpace.GetInternalRepresentationFromVectorLeft(resultLeft, currentPoly, theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << "maps x_{" << k << "} to: " << currentPoly.ElementToString() << "<br><br>";
      }
      for (int k=0; k<this->theTargetSpace.rightSpaceBasis.size; k++)
      { ElementRight& currentRightElt=this->theTargetSpace.rightSpaceBasis.TheObjects[k];
        Polynomial<CoefficientType>& currentPoly=currentAction->TheObjects[k];
        currentRightElt.ActOnMe(currentGenerator, resultRight, theAlgebra, this->theTargetSpace.theRingUniT, this->theTargetSpace.theRingZerO, &theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << " maps " << currentRightElt.ElementToString() << " to " << resultRight.ElementToString();
        this->theTargetSpace.GetInternalRepresentationFromVectorRight(resultRight, currentPoly, theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << "maps x_{" << k << "} to: " << currentPoly.ElementToString() << "<br><br>";
      }
    }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::PrepareTheMatrix
  (List<ElementSimpleLieAlgebra>& theElementsActing, SemisimpleLieAlgebra& theAlgebra, Matrix<CoefficientType>& output, GlobalVariables& theGlobalVariables)
{ TensorProductElement<ElementLeft, ElementRight, CoefficientType> resultElt;
  output.init(this->theTargetSpace.leftSpaceBasis.size*this->theTargetSpace.rightSpaceBasis.size*theElementsActing.size, this->size);
  output.NullifyAll(this->theStartingSpace.theRingZerO);
  this->ComputeDebugString();
  Vector<CoefficientType> theVector;
  for (int i=0; i<theElementsActing.size; i++)
  { ElementSimpleLieAlgebra& currentGenerator= theElementsActing.TheObjects[i];
    //if (i<1)
    //  i=1;
    for (int j=0; j<this->size; j++)
    { //if (j<6 && i==0)
        //j=6;
      TensorProductElement<ElementLeft, ElementRight, CoefficientType>& currentElt= this->TheObjects[j];
      assert(currentElt.internalRepresentation.NumVars==this->theStartingSpace.leftSpaceBasis.size+this->theStartingSpace.rightSpaceBasis.size);
      currentElt.ActOnMe(currentGenerator, resultElt, this->theStartingSpace, this->theTargetSpace, theAlgebra, theGlobalVariables);
      /*resultElt.ComputeDebugString();
      if (i==1 && j==2)
      { resultElt.ComputeDebugString();
        this->flagAnErrorHasOccurredTimeToPanic=true;
      }*/
      resultElt.ElementToVector(theVector, this->theTargetSpace, theGlobalVariables);
      theVector.ComputeDebugString();
      std::cout << "<br>" << currentGenerator.ElementToString() << " acting on ";
      std::cout << currentElt.ElementToString(this->theStartingSpace) << " gives ";
      std::cout << resultElt.ElementToString(this->theTargetSpace);
      for (int k=0; k<theVector.size; k++)
        output.elements[i*theVector.size+k][j]=theVector.TheObjects[k];
    }
  }
  std::cout << "<br><br><br><br>... and the matrix is: <div class=\"math\">" << output.ElementToString(false, true) <<"</div>";

}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ElementToString
  (TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner)
{ int theLeftIndex=-1, theRightIndex=-1;
  std::stringstream out;
  PolynomialOutputFormat tempFormat;
  for (int i=0; i<this->internalRepresentation.size; i++)
  { Monomial<CoefficientType>& currentMon=this->internalRepresentation.TheObjects[i];
    owner.GetLeftAndRightIndexFromMonomial(currentMon, theLeftIndex, theRightIndex);
    std::string tempS= currentMon.Coefficient.ElementToString();
    if (tempS=="1")
      out << "(" << tempS << ")";
    out << "(" << owner.leftSpaceBasis.TheObjects[theLeftIndex].ElementToString(tempFormat) << ")\\otimes("
    << owner.rightSpaceBasis.TheObjects[theRightIndex].ElementToString() << ")";
  }
  return out.str();
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ElementToVector
  (Vector<CoefficientType>& output, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)const
{ output.MakeZero(owner.leftSpaceBasis.size*owner.rightSpaceBasis.size, owner.theRingZerO);
  for (int i=0; i<this->internalRepresentation.size; i++)
  { int relevantIndex=owner.GetIndexFromMonomial(this->internalRepresentation.TheObjects[i]);
    if (TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::flagAnErrorHasOccurredTimeToPanic)
    { output.ComputeDebugString();
      this->internalRepresentation.TheObjects[i].Coefficient.ComputeDebugString();
    }
    output.TheObjects[relevantIndex]=this->internalRepresentation.TheObjects[i].Coefficient;
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::AssignVector
  (Vector<CoefficientType>& input, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
{ this->Nullify(owner);
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> tempElt;
  for (int i=0; i<input.size; i++)
  { owner.GetEltFromIndex(i, tempElt, theGlobalVariables);
    this->operator+=(tempElt);
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ActOnMe
  (const ElementSimpleLieAlgebra& theElt, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theStartingSpace,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theTargetSpace,
   SemisimpleLieAlgebra& theAlgebra, GlobalVariables& theGlobalVariables)
{ assert(this!=&output);
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> newSummand;
  output.Nullify(theTargetSpace);
  ElementLeft leftComponent, newLeftComponent;
  ElementRight rightComponent, newRightComponent;
  Polynomial<CoefficientType> tempP;
  for (int i=0; i<this->internalRepresentation.size; i++)
  { theStartingSpace.GetComponentsFromInternalRepresentation(this->internalRepresentation.TheObjects[i], leftComponent, rightComponent);
    //leftComponent.ComputeDebugString();
    //rightComponent.ComputeDebugString(false, false);
    leftComponent.ActOnMe(theElt, newLeftComponent, theAlgebra, theStartingSpace.theRingUniT, theStartingSpace.theRingZerO, &theGlobalVariables);
//    std::cout << "<br>" << theElt.ElementToString() << " acting on " << leftComponent.ElementToString() << " gives " << newLeftComponent.ElementToString();
    rightComponent.ActOnMe(theElt, newRightComponent, theAlgebra);
//    leftComponent.ComputeDebugString();
//    rightComponent.ComputeDebugString(false, false);
//    newRightComponent.ComputeDebugString(false, false);
//    newLeftComponent.ComputeDebugString();
//    std::cout << "<br>" << theElt.ElementToString() << " acting on " << rightComponent.ElementToString() << " gives " << newRightComponent.ElementToString();

    theTargetSpace.GetInternalRepresentationFromLeftAndRight(newLeftComponent, rightComponent, tempP, theGlobalVariables);
    tempP.ComputeDebugString();
    output.internalRepresentation.AddPolynomial(tempP);
    theTargetSpace.GetInternalRepresentationFromLeftAndRight(leftComponent, newRightComponent, tempP, theGlobalVariables);
    tempP.ComputeDebugString();
    output.internalRepresentation.AddPolynomial(tempP);
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::SetTensorProduct
  (const  ElementLeft& leftVector, const  ElementRight& rightVector, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
{ owner.GetInternalRepresentationFromLeftAndRight(leftVector, rightVector, this->internalRepresentation, theGlobalVariables);
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::SetTensorProductBasisElement
  (int leftIndex, int rightIndex, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
{ Polynomial<CoefficientType> tempP;
  tempP.MakeMonomialOneLetter(owner.leftSpaceBasis.size+owner.rightSpaceBasis.size, leftIndex, 1 , owner.theRingUniT);
  this->internalRepresentation.MakeMonomialOneLetter(owner.leftSpaceBasis.size+owner.rightSpaceBasis.size, rightIndex+owner.leftSpaceBasis.size, 1 , owner.theRingUniT);
  this->internalRepresentation*=tempP;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ElementToString()
{ std::stringstream out;
  std::stringstream tempS;
  out << this->internalRepresentation.ElementToString();
  return out.str();
}

template<class CoefficientType>
template<class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<CoefficientType>::GetBasisFromSpanOfElements
  (List<ElementUniversalEnvelopingOrdered<CoefficientType> >& theElements,
   Vectors<CoefficientTypeQuotientField>& outputCoords,
   List<ElementUniversalEnvelopingOrdered<CoefficientType> >& outputTheBasis, const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero,
   GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  ElementUniversalEnvelopingOrdered<CoefficientType> outputCorrespondingMonomials;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddOnTopNoRepetitionHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordsBeforeReduction.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
  { Vector<CoefficientTypeQuotientField>& currentList=outputCoordsBeforeReduction.TheObjects[i];
    currentList.MakeZero(outputCorrespondingMonomials.size, theFieldZero);
    ElementUniversalEnvelopingOrdered<CoefficientType>& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnvelopingOrdered<CoefficientType>& currentMon=currentElt.TheObjects[j];
      currentList.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
  outputTheBasis.size=0;
  outputTheBasis.MakeActualSizeAtLeastExpandOnTop(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.MakeActualSizeAtLeastExpandOnTop(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis, theGlobalVariables);
  for (int i=0; i<selectedBasis.CardinalitySelection; i++)
    outputTheBasis.AddOnTop(theElements.TheObjects[selectedBasis.elements[i]]);
  Matrix<CoefficientType> bufferMat;
  Vectors<CoefficientType> bufferVectors;
  outputCoordsBeforeReduction.GetCoordsInBasis(basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::Simplify
(GlobalVariables* theContext,
 const CoefficientType& theRingUnit,  const CoefficientType& theRingZero)
{ ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  //this->ComputeDebugString();
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Simplify(buffer, theContext, theRingUnit, theRingZero);
    output+=buffer;
    //output.ComputeDebugString();
  }
  *this=output;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::Nullify(SemisimpleLieAlgebraOrdered& theOwner)
{ this->ClearTheObjects();
  this->owner=&theOwner;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::Simplify
(ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingUnit,  const CoefficientType& theRingZero
 )
{ output.Nullify(*this->owner);
  output.AddOnTopHash(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theContext, theRingUnit, theRingZero);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexSwapWithLastHash(i);
      i--;
    }
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator+=(const ElementUniversalEnvelopingOrdered<CoefficientType>& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  for (int i=0; i<other.size; i++)
    this->AddMonomialNoCleanUpZeroCoeff(other.TheObjects[i]);
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator-=(const ElementUniversalEnvelopingOrdered<CoefficientType>& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  for (int i=0; i<other.size; i++)
  { tempMon=other.TheObjects[i];
    tempMon.Coefficient*=-1;
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
template <class otherType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator*=(const otherType& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.operator*=(other);
}

template <class CoefficientType>
template <class SecondType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator/=
(const SecondType& other)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.operator/=(other);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other)
{ assert(this!=&other);
  this->generatorsIndices.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Powers.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(other.Coefficient);
  if (other.generatorsIndices.size==0)
    return;
  int firstIndex=other.generatorsIndices.TheObjects[0];
  int i=0;
//  std::string tempS;
//  tempS=other.ElementToString();
  if (this->generatorsIndices.size>0)
    if (firstIndex==(*this->generatorsIndices.LastObject()))
    { //this->ComputeDebugString();
      *this->Powers.LastObject()+=other.Powers.TheObjects[0];
      i=1;
    }
  for (; i<other.generatorsIndices.size; i++)
  { this->Powers.AddOnTop(other.Powers.TheObjects[i]);
    this->generatorsIndices.AddOnTop(other.generatorsIndices.TheObjects[i]);
  }
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::IsProportionalTo
 (const ElementUniversalEnvelopingOrdered<CoefficientType>& other, CoefficientType& outputTimesMeEqualsOther,
  const CoefficientType& theRingZero)const
{ if (this->IsEqualToZero())
  { if (other.IsEqualToZero())
      return true;
    return false;
  }
  if (other.IsEqualToZero())
  { outputTimesMeEqualsOther=theRingZero;
    return true;
  }
  if (other.size!=this->size)
    return false;
  MonomialUniversalEnvelopingOrdered<CoefficientType>& theMon= this->TheObjects[0];
  int theIndex=other.IndexOfObjectHash(theMon);
  if (theIndex==-1)
    return false;
  MonomialUniversalEnvelopingOrdered<CoefficientType>& otherMon= other.TheObjects[theIndex];
  theMon.ComputeDebugString();
  otherMon.ComputeDebugString();
  outputTimesMeEqualsOther=otherMon.Coefficient;
  outputTimesMeEqualsOther/=theMon.Coefficient;
  ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;

  tempElt=*this;
  tempElt*=outputTimesMeEqualsOther;
  tempElt-=other;
  return tempElt.IsEqualToZero();
}

template<class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::operator*=
(const MonomialUniversalEnvelopingOrdered& other)
{ assert(this!=&other);
  this->Coefficient*=other.Coefficient;
  for (int i=0; i<other.generatorsIndices.size; i++)
    this->MultiplyByGeneratorPowerOnTheRight(other.generatorsIndices.TheObjects[i], other.Powers.TheObjects[i]);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator*=(const ElementUniversalEnvelopingOrdered<CoefficientType>& other)
{ ElementUniversalEnvelopingOrdered output;
  output.Nullify(*this->owner);
  output.MakeActualSizeAtLeastExpandOnTop(this->size*other.size);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  for (int i=0; i<this->size; i++)
    for(int j=0; j<other.size; j++)
    { tempMon=this->TheObjects[i];
      tempMon*=other.TheObjects[j];
      output.AddMonomial(tempMon);
    }
  *this=output;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::AddMonomialNoCleanUpZeroCoeff
(const MonomialUniversalEnvelopingOrdered<CoefficientType>& input)
{ //MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  //tempMon=input;
  //tempMon.ComputeDebugString();
  //this->ComputeDebugString();
  int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddOnTopHash(input);
  else
    this->TheObjects[theIndex].Coefficient+=input.Coefficient;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::SimplifyAccumulateInOutputNoOutputInit
(ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero
 )
{ int IndexlowestNonSimplified=0;
  ElementUniversalEnvelopingOrdered<CoefficientType> buffer2;
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified<output.size)
  { bool reductionOccurred=false;
    if (output.TheObjects[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred=true;
    else
      for (int i=0; i<output.TheObjects[IndexlowestNonSimplified].generatorsIndices.size-1; i++)
        if (output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i]>output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1])
        { if (output.TheObjects[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute
              (i, tempMon, theContext, theRingZero))
          { output.AddMonomialNoCleanUpZeroCoeff(tempMon);
//            tempMon.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft(i, buffer2, theContext, theRingUnit, theRingZero);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
//            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight
            (i, buffer2, theContext, theRingUnit, theRingZero);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
//            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
        }
    if (reductionOccurred)
      output.PopIndexSwapWithLastHash(IndexlowestNonSimplified);
    else
      IndexlowestNonSimplified++;
//    output.ComputeDebugString();
  }
  output.CleanUpZeroCoeff();
}

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex, MonomialUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingZero)
{ if (theLeftIndex>=this->generatorsIndices.size-1)
    return false;
  int theLeftGeneratorIndex=this->generatorsIndices.TheObjects[theLeftIndex];
  int theRightGeneratorIndex=this->generatorsIndices.TheObjects[theLeftIndex+1];
  ElementSimpleLieAlgebra tempElt;
  this->owner->theOwner.LieBracket(this->owner->theOrder.TheObjects[theLeftGeneratorIndex], this->owner->theOrder.TheObjects[theRightGeneratorIndex], tempElt);
  if (tempElt.IsEqualToZero())
  { output.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Nullify(theRingZero, *this->owner);
    output.Coefficient=this->Coefficient;
    //output.ComputeDebugString();
    for (int i=0; i<theLeftIndex; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex+1], this->Powers.TheObjects[theLeftIndex+1]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex], this->Powers.TheObjects[theLeftIndex]);
    for (int i=theLeftIndex+2; i<this->generatorsIndices.size; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    return true;
  }
  return false;
}

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::CommutingRightIndexAroundLeftIndexAllowed(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::CommuteConsecutiveIndicesRightIndexAroundLeft
(int theIndeX, ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero
 )
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.Nullify(theRingZero, *this->owner);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndeX= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndeX=this->generatorsIndices.TheObjects[theIndeX];
  CoefficientType theRightPoweR, theLeftPoweR;
  theRightPoweR= this->Powers.TheObjects[theIndeX+1];
  theLeftPoweR= this->Powers.TheObjects[theIndeX];
  theRightPoweR-=1;
  int powerDroP=0;
//  if (this->flagAnErrorHasOccurredTimeToPanic)
//  if (this->ElementToString()=="2f_{5}f_{-5}f_{-4}" || this->ElementToString()=="2f_{11}f_{-4}")
//  { std::cout << "here we are!";
//    this->flagAnErrorHasOccurredTimeToPanic=true;
//  }

  CoefficientType acquiredCoefficienT;
  acquiredCoefficienT=this->Coefficient;
  tempMon.Coefficient=this->Coefficient;
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  MonomialUniversalEnvelopingOrdered startMon;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResulT, tempElT, theLeftElt;
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndeX, adResulT);
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndeX, theLeftElt);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  root theCoeffs;
  do
  { if(this->flagAnErrorHasOccurredTimeToPanic)
    { acquiredCoefficienT.ComputeDebugString();
      theRightPoweR.ComputeDebugString();
      theLeftPoweR.ComputeDebugString();
      adResulT.ComputeDebugString( false, false);
      tempMon.ComputeDebugString();
    }
    this->owner->GetLinearCombinationFrom(adResulT, theCoeffs);
    for (int i=0; i<theCoeffs.size; i++)
      if (theCoeffs.TheObjects[i]!=0)
      { int theNewGeneratorIndex=i;
        tempMon=startMon;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient=acquiredCoefficienT;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient*=(theCoeffs.TheObjects[i]);
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
        tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
        for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        output.AddOnTopHash(tempMon);
      }
    acquiredCoefficienT.MultiplyBy(theLeftPoweR);
    theLeftPoweR-=1;
    this->owner->theOwner.LieBracket(theLeftElt, adResulT, tempElT);
    adResulT=tempElT;
    powerDroP++;
    acquiredCoefficienT/=powerDroP;
    if (this->flagAnErrorHasOccurredTimeToPanic)
      adResulT.ComputeDebugString(false, false);
  }while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::CommuteConsecutiveIndicesLeftIndexAroundRight
(int theIndeX, ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero
 )
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.Nullify(theRingZero, *this->owner);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndex= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndex=this->generatorsIndices.TheObjects[theIndeX];
  CoefficientType theRightPower, theLeftPower;
  theRightPower= this->Powers.TheObjects[theIndeX+1];
  theLeftPower= this->Powers.TheObjects[theIndeX];
  theLeftPower-=1;
  int powerDrop=0;
 /* if (this->ElementToString()=="2f_{5}f_{-5}f_{-4}" || this->ElementToString()=="2f_{11}f_{-4}")
  { std::cout << "here we are!";
    this->flagAnErrorHasOccurredTimeToPanic=true;
  }*/

  CoefficientType acquiredCoefficient;
  acquiredCoefficient.Assign(this->Coefficient);
  tempMon.Coefficient=this->Coefficient;
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theIndeX], theLeftPower);
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResult, tempElt, tempRightElt;
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndex, adResult);
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndex, tempRightElt);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  root theCoeffs;
  do
  { //acquiredCoefficient.ComputeDebugString();
    //theRightPower.ComputeDebugString();
    //adResult.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    //tempMon.ComputeDebugString();
    this->owner->GetLinearCombinationFrom(adResult, theCoeffs);
    for (int i=0; i<theCoeffs.size; i++)
      if(theCoeffs.TheObjects[i]!=0)
      { int theNewGeneratorIndex= i;
        tempMon=startMon;
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
        tempMon.Coefficient=acquiredCoefficient;
        tempMon.Coefficient*=(theCoeffs.TheObjects[i]);
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
        for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
        output.AddOnTopHash(tempMon);
      }
    acquiredCoefficient.MultiplyBy(theRightPower);
    theRightPower-=1;
    this->owner->theOwner.LieBracket(adResult, tempRightElt, tempElt);
    adResult=tempElt;
    powerDrop++;
    acquiredCoefficient/=powerDrop;
    //adResult.ComputeDebugString(*this->owner, false, false);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::CommutingLeftIndexAroundRightIndexAllowed(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex)
{ int tempInt;
  if (theLeftPower.IsSmallInteger(tempInt))
  { if(theRightPower.IsSmallInteger(tempInt))
      return true;
    int numPosRoots=this->owner->theOwner.theWeyl.RootsOfBorel.size;
    int theDimension= this->owner->theOwner.theWeyl.CartanSymmetric.NumRows;
    if(rightGeneratorIndex>= numPosRoots && rightGeneratorIndex<numPosRoots+theDimension)
    { ElementSimpleLieAlgebra tempElt;
      this->owner->theOwner.LieBracket(this->owner->theOrder.TheObjects[leftGeneratorIndex], this->owner->theOrder.TheObjects[rightGeneratorIndex], tempElt);
      if (tempElt.IsEqualToZero())
        return true;
      else
        return false;
    } else
      return true;
  }
  return false;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::Nullify(int numVars, SemisimpleLieAlgebraOrdered& theOwner, GlobalVariables* theContext)
{ this->Coefficient.Nullify((int)numVars, theContext);
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::Nullify(const CoefficientType& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Coefficient=theRingZero;
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

template <class CoefficientType>
int MonomialUniversalEnvelopingOrdered<CoefficientType>::HashFunction() const
{ int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result=0;
  for (int i=0; i<top; i++)
    result+=SomeRandomPrimes[i]*this->generatorsIndices.TheObjects[i];
  return result;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower)
{ if (thePower==0)
    return;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst(this->Coefficient.NumVars, thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const CoefficientType& thePower)
{ if (thePower.IsEqualToZero())
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { (*this->Powers.LastObject())+=thePower;
      return;
    }
  this->Powers.AddOnTop(thePower);
  this->generatorsIndices.AddOnTop(theGeneratorIndex);
}


template <class CoefficientType>
std::string MonomialUniversalEnvelopingOrdered<CoefficientType>::ElementToString
(bool useLatex, bool useCalculatorFormat,
 const PolynomialOutputFormat& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
{ if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  if (this->generatorsIndices.size>0)
  { tempS= MathRoutines::ElementToStringBrackets(this->Coefficient, PolyFormatLocal);
    if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
  } else
    tempS= this->Coefficient.ElementToString(PolyFormatLocal);
  out << tempS;
  for (int i=0; i<this->generatorsIndices.size; i++)
  { CoefficientType& thePower=this->Powers[i];
    int theIndex=this->generatorsIndices[i];
    bool usebrackets=false;
    tempS=this->owner->GetGeneratorString
    (theIndex, useCalculatorFormat, usebrackets, PolyFormatLocal, theGlobalVariables)
    ;
    if (usebrackets)
      out << "(";
    out << tempS;
    if (usebrackets)
      out << ")";
    tempS=thePower.ElementToString(PolyFormatLocal);
    if (tempS!="1")
    { out << "^";
     // if (useLatex)
      out << "{";
      out << tempS;
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::ElementToString
(std::string& output, bool useLatex,
 bool useCalculatorFormat,
 const PolynomialOutputFormat& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
{ std::stringstream out;
  std::string tempS;
  if (this->size==0)
    out << "0";
  int IndexCharAtLastLineBreak=0;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnvelopingOrdered<CoefficientType>& current=this->TheObjects[i];
    tempS=current.ElementToString(false, useCalculatorFormat, PolyFormatLocal, theGlobalVariables);
    if (i!=0)
      if (tempS.size()>0)
        if (tempS[0]!='-')
          out << '+';
    out << tempS;
    if (((int)out.tellp())- IndexCharAtLastLineBreak>150)
    { IndexCharAtLastLineBreak=out.tellp();
      out << "\\\\&&";
    }
  }
  output=out.str();
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::AssignElementUniversalEnveloping
  (ElementUniversalEnveloping<CoefficientType>& input, SemisimpleLieAlgebraOrdered& owner,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  this->Nullify(owner);
  for (int i=0; i<input.size; i++)
  { if(!tempElt.AssignMonomialUniversalEnveloping(input.TheObjects[i], owner, theRingUnit, theRingZero, theContext))
      return false;
    this->operator+=(tempElt);
  }
  this->Simplify(theContext, theRingUnit, theRingZero);
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::AssignMonomialUniversalEnveloping
  (MonomialUniversalEnveloping<CoefficientType>& input, SemisimpleLieAlgebraOrdered& owner,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered theMon;
  ElementSimpleLieAlgebra tempElt;
  CoefficientType theCoeff;
  theCoeff=theRingUnit;
  theCoeff=input.Coefficient;
  this->MakeConst(theCoeff, owner);
  //std::cout << "<br>after initialization with constant I am " << this->ElementToString();
  for (int i=0; i<input.generatorsIndices.size; i++)
  { int thePower;
    bool isASmallInt=input.Powers.TheObjects[i].IsSmallInteger(thePower);
    if (isASmallInt)
    { tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(input.generatorsIndices.TheObjects[i], owner.theOwner);
      theMon.AssignElementLieAlgebra(tempElt, theRingUnit, theRingZero, owner);
      //std::cout << "<br>raising " << theMon.ElementToString() << " to power " << thePower;
      theMon.RaiseToPower(thePower, theRingUnit);
      //std::cout << " to obtain " << theMon.ElementToString();
    }
    else
      return false;
    this->MultiplyBy(theMon);

  }
  //this->Simplify(theContext);
  return true;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::AssignElementLieAlgebra
(const ElementSimpleLieAlgebra& input, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Nullify(theOwner);
  root ElementRootForm;
  input.ElementToVectorNegativeRootSpacesFirst(ElementRootForm);
  theOwner.ChevalleyGeneratorsInCurrentCoords.ActOnAroot(ElementRootForm);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  tempMon.Nullify(theRingZero, theOwner);
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  tempMon.Powers.TheObjects[0]=theRingUnit;
  for (int theIndex=0; theIndex<ElementRootForm.size; theIndex++)
    if ( ElementRootForm.TheObjects[theIndex]!=0)
    { tempMon.Coefficient=theRingUnit;
      tempMon.Coefficient*=ElementRootForm.TheObjects[theIndex];
      tempMon.generatorsIndices.TheObjects[0]=theIndex;
      this->AddOnTopHash(tempMon);
    }
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::RaiseToPower(int thePower, const CoefficientType& theRingUnit)
{ if (this->size==0)
    return;
  ElementUniversalEnvelopingOrdered<CoefficientType> buffer;
  buffer.operator=(*this);
  this->MakeConst(theRingUnit, *this->owner);
  //std::cout << "<br>raising " <<buffer.ElementToString() << " to power " << thePower;
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
  //std::cout << "<br> and the result is " << this->ElementToString();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::LieBracketOnTheRight
(const ElementSimpleLieAlgebra& right, const CoefficientType& ringUnit, const CoefficientType& ringZero)
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  tempElt.AssignElementLieAlgebra(right, ringUnit, ringZero, *this->owner);
  this->LieBracketOnTheRight(tempElt, *this);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output)
{ ElementUniversalEnvelopingOrdered tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

template<class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::AddMonomial(const MonomialUniversalEnvelopingOrdered<CoefficientType>& input)
{ if (input.IsEqualToZero())
    return;
  int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddOnTopHash(input);
  else
  { this->TheObjects[theIndex].Coefficient+=input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelationSOld
  (bool SubHighestWeightWithZeroes, const PolynomialsRationalCoeff& highestWeightSub,
   GlobalVariables* theContext, const CoefficientType& theRingUnit)
{ MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  ElementUniversalEnvelopingOrdered<CoefficientType> output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
//    tempMon.ComputeDebugString();
    tempMon.ModOutVermaRelationSOld(SubHighestWeightWithZeroes, highestWeightSub, theContext, theRingUnit);
//    tempMon.ComputeDebugString();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelationSOld
  (bool SubHighestWeightWithZeroes, const PolynomialsRationalCoeff& highestWeightSub, GlobalVariables* theContext, const CoefficientType& theRingUnit)
{ int numPosRoots=this->owner->theOwner.GetNumPosRoots();
  int theDimension=this->owner->theOwner.GetRank();
  int theNumVars=this->Coefficient.NumVars;
  //this->ComputeDebugString();
  if (theNumVars<theDimension && !SubHighestWeightWithZeroes)
  { this->SetNumVariables(theDimension);
    theNumVars=theDimension;
  }
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices.TheObjects[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->Nullify(theNumVars, *this->owner, theContext);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { int theDegree;
      if (!this->Powers.TheObjects[i].IsSmallInteger(theDegree))
        return;
      if (SubHighestWeightWithZeroes)
      { this->Nullify(theNumVars, *this->owner, theContext);
        return;
      }
      PolynomialRationalCoeff tempP;
      root tempRoot;
      tempRoot.MakeZero(theNumVars);
      for (int i=0; i<this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.size; i++)
        tempRoot.TheObjects[i]=this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.TheObjects[i];
      tempP.MakeLinPolyFromRootNoConstantTerm(tempRoot);
      tempP.Substitution(highestWeightSub, theNumVars, (Rational) 1);
      tempP.RaiseToPower(theDegree, (Rational) 1);
//      tempP.ComputeDebugString();
      this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.ComputeDebugString();
      assert(this->Coefficient.checkConsistency());
      this->Coefficient.operator*=(tempP);
      assert(this->Coefficient.checkConsistency());
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::GetElementUniversalEnveloping
  (ElementUniversalEnveloping<CoefficientType>& output, SemisimpleLieAlgebra& owner)
{ ElementUniversalEnveloping<CoefficientType> Accum, tempElt;
  Accum.Nullify(owner);
  for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].GetElementUniversalEnveloping(tempElt, owner))
      return false;
    else
      Accum+=tempElt;
  output=Accum;
  return true;
}

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::GetElementUniversalEnveloping
  (ElementUniversalEnveloping<CoefficientType>& output, SemisimpleLieAlgebra& owner)
{ ElementUniversalEnveloping<CoefficientType> Accum;
  ElementUniversalEnveloping<CoefficientType> tempMon;
  int theIndex;
  Accum.MakeConst(this->Coefficient, owner);
  for (int i=0; i<this->generatorsIndices.size; i++)
    if (this->Powers.TheObjects[i].IsSmallInteger())
    { tempMon.AssignElementLieAlgebra(this->owner->theOrder.TheObjects[this->generatorsIndices.TheObjects[i]], this->Coefficient.NumVars, owner);
      tempMon.RaiseToPower(this->Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort);
      Accum.MultiplyBy(tempMon);
    }
    else
      if (this->owner->theOrder.TheObjects[this->generatorsIndices.TheObjects[i]].IsACoeffOneChevalleyGenerator(theIndex, owner))
      { tempMon.MakeOneGeneratorCoeffOne(theIndex, this->Coefficient.NumVars, owner);
        tempMon.TheObjects[0].Powers.TheObjects[0]=this->Powers.TheObjects[i];
        Accum.MultiplyBy(tempMon);
      } else
        return false;
  output.operator=(Accum);
  return true;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::SetNumVariables(int newNumVars)
{ //this->ComputeDebugString();
  if (this->size==0)
    return;
  int currentNumVars=this->TheObjects[0].Coefficient.NumVars;
  if (currentNumVars==newNumVars)
    return;
  ElementUniversalEnvelopingOrdered Accum;
  Accum.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon.SetNumVariables(newNumVars);
    Accum.AddMonomial(tempMon);
  }
//  Accum.ComputeDebugString();
  this->operator=(Accum);
 // this->ComputeDebugString();
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::SetNumVariables(int newNumVars)
{ //the below commented out code causes problems in substitution code!
  //if (this->Coefficient.NumVars==newNumVars)
  //  return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::GetCoordsInBasis
  (List<ElementUniversalEnvelopingOrdered<CoefficientType> >& theBasis, Vector<CoefficientType>& output,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables& theGlobalVariables)const
{ List<ElementUniversalEnvelopingOrdered<CoefficientType> > tempBasis, tempElts;
  tempBasis=theBasis;
  tempBasis.AddOnTop(*this);
  Vectors<CoefficientType> tempCoords;
  this->GetBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero, theGlobalVariables);
  Vector<CoefficientType> tempRoot;
  tempRoot=*tempCoords.LastObject();
  tempCoords.SetSize(theBasis.size);
  return tempRoot.GetCoordsInBasiS(tempCoords, output, theRingUnit, theRingZero);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::GetCoordinateFormOfSpanOfElements
  (int numVars, List<ElementUniversalEnvelopingOrdered>& theElements, List<rootPoly>& outputCoordinates,
   ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddOnTopNoRepetitionHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSize(theElements.size);
  PolynomialRationalCoeff ZeroPoly;
  ZeroPoly.Nullify((int)numVars);
  for (int i=0; i<theElements.size; i++)
  { rootPoly& current=outputCoordinates.TheObjects[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnvelopingOrdered& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnvelopingOrdered<CoefficientType>& currentMon=currentElt.TheObjects[j];
      current.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::SubstitutionCoefficients
(PolynomialsRationalCoeff& theSub)
{ if (theSub.size<1)
    return;
  this->Coefficient.Substitution(theSub, theSub[0].NumVars);
  this->SetNumVariables(theSub[0].NumVars);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::SubstitutionCoefficients
(PolynomialsRationalCoeff& theSub, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<CoefficientType> endResult;
  MonomialUniversalEnvelopingOrdered<CoefficientType> currentMon;
  endResult.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { currentMon=this->TheObjects[i];
    currentMon.SubstitutionCoefficients(theSub);
    endResult.AddMonomial(currentMon);
  }
//  endResult.Simplify(theContext);
  this->operator=(endResult);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::MakeConst
(const Rational& coeff, int numVars, SemisimpleLieAlgebra& theOwner)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  this->Nullify(theOwner);
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((int)numVars, coeff);
  tempMon.MakeConst(tempP, theOwner);
  this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::Nullify(SemisimpleLieAlgebra& theOwner)
{ this->ClearTheObjects();
  this->owner=&theOwner;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator+=(const ElementUniversalEnveloping<CoefficientType>& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  for (int i=0; i<other.size; i++)
    this->AddMonomialNoCleanUpZeroCoeff(other.TheObjects[i]);
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator-=
(const ElementUniversalEnveloping<CoefficientType>& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  for (int i=0; i<other.size; i++)
  { tempMon=other.TheObjects[i];
    tempMon.Coefficient*=(-1);
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator/=(const Rational& other)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.DivByRational(other);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator*=(const PolynomialRationalCoeff& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.MultiplyBy(other);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator*=(const Rational& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient*=(other);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator*=
(const MonomialUniversalEnveloping<CoefficientType>& standsOnTheRight)
{ ElementUniversalEnveloping<CoefficientType> output;
  output.MakeActualSizeAtLeastExpandOnTop(this->size);
  output.Nullify(*this->owner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  int sizeOriginal=0;
  CoefficientType powerOriginal, CoeffOriginal;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    sizeOriginal=tempMon.generatorsIndices.size;
    if (sizeOriginal>0)
      powerOriginal=*tempMon.Powers.LastObject();
    CoeffOriginal.Assign(tempMon.Coefficient);
    tempMon.generatorsIndices.size=sizeOriginal;
    tempMon.Powers.size=sizeOriginal;
    if (sizeOriginal>0)
      *tempMon.Powers.LastObject()=powerOriginal;
    tempMon.Coefficient.Assign(CoeffOriginal);
    tempMon.MultiplyByNoSimplify(standsOnTheRight);
      //tempMon.ComputeDebugString();
    output.AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  (*this)=output;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::operator*=(const ElementUniversalEnveloping& standsOnTheRight)
{ ElementUniversalEnveloping output;
  output.Nullify(*this->owner);
  output.SetExpectedSize(standsOnTheRight.size*this->size);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  int sizeOriginal=0;
  CoefficientType powerOriginal, CoeffOriginal;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    sizeOriginal=tempMon.generatorsIndices.size;
    if (sizeOriginal>0)
      powerOriginal=*tempMon.Powers.LastObject();
    CoeffOriginal.Assign(tempMon.Coefficient);
    for(int j=0; j<standsOnTheRight.size; j++)
    { tempMon.generatorsIndices.size=sizeOriginal;
      tempMon.Powers.size=sizeOriginal;
      if (sizeOriginal>0)
        *tempMon.Powers.LastObject()=powerOriginal;
      tempMon.Coefficient.Assign(CoeffOriginal);
      tempMon.MultiplyByNoSimplify(standsOnTheRight.TheObjects[j]);
      //tempMon.ComputeDebugString();
      output.AddMonomialNoCleanUpZeroCoeff(tempMon);
    }
  }
  *this=output;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::SetNumVariables(int newNumVars)
{ //this->ComputeDebugString();
  if (this->size==0)
    return;
  int currentNumVars=this->TheObjects[0].Coefficient.NumVars;
  if (currentNumVars==newNumVars)
    return;
  ElementUniversalEnveloping<CoefficientType> Accum;
  Accum.Nullify(*this->owner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon.SetNumVariables(newNumVars);
    Accum.AddMonomial(tempMon);
  }
//  Accum.ComputeDebugString();
  this->operator=(Accum);
 // this->ComputeDebugString();
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::RaiseToPower(int thePower)
{ ElementUniversalEnveloping<CoefficientType> buffer;
  buffer=*this;
  if (this->size==0)
    return;
  this->MakeConst(1, this->TheObjects[0].Coefficient.NumVars, *this->owner);
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::MakeCasimir
(SemisimpleLieAlgebra& theOwner, int numVars, GlobalVariables& theGlobalVariables)
{ //std::stringstream out;
  this->Nullify(theOwner);
  WeylGroup& theWeyl= this->owner->theWeyl;
  int theDimension=theWeyl.CartanSymmetric.NumRows;
  root tempRoot1, tempRoot2;
/*  MatrixLargeRational killingForm;
  killingForm.init(theDimension, theDimension);
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
    for (int j=0; j<theDimension; j++)
    { killingForm.elements[i][j]=0;
      tempRoot2.MakeEi(theDimension, j);
      for (int k=0; k<theWeyl.RootSystem.size; k++)
        killingForm.elements[i][j]+= theWeyl.RootScalarCartanRoot(tempRoot1, theWeyl.RootSystem.TheObjects[k])* theWeyl.RootScalarCartanRoot(tempRoot2, theWeyl.RootSystem.TheObjects[k]);
    }
  }
  std::cout << killingForm.ElementToString(true, false);
  killingForm.Invert(theGlobalVariables);
  killingForm.ComputeDebugString();
  out << killingForm.ElementToString(true, false);*/
//  std::cout << killingForm.ElementToString(true, false);
  ElementUniversalEnveloping<CoefficientType> tempElt1, tempElt2;
//this code is to check a math formula:
//  ElementUniversalEnveloping checkElement;
//  checkElement.Nullify(theOwner);
  MatrixLargeRational invertedSymCartan;
  invertedSymCartan=theWeyl.CartanSymmetric;
  invertedSymCartan.Invert();
////////////////////////////////////////////////////////////////////////
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
  //implementation without the ninja formula:
/*    killingForm.ActOnAroot(tempRoot1, tempRoot2);
    tempElt1.AssignElementCartan(tempRoot1, numVars, theOwner);
    tempElt2.AssignElementCartan(tempRoot2, numVars, theOwner);
    tempElt1*=tempElt2;
    *this+=tempElt1;*/
// Alternative implementation using a ninja formula I cooked up after looking at the printouts:
    invertedSymCartan.ActOnAroot(tempRoot1, tempRoot2);
    tempElt1.AssignElementCartan(tempRoot1, numVars, theOwner);
    tempElt2.AssignElementCartan(tempRoot2, numVars, theOwner);
    tempElt1*=tempElt2;
    *this+=tempElt1;
  }
  Rational tempRat;
  root theSum;
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { /*Implementation without the ninja formula:
    tempRat=0;
    root& theRoot=theWeyl.RootSystem.TheObjects[i];
    int indexOfOpposite=theWeyl.RootSystem.IndexOfObjectHash(-theRoot);
    root& theOpposite= theWeyl.RootSystem.TheObjects[indexOfOpposite];
    for (int j=0; j<theWeyl.RootSystem.size; j++)
    { root& current=theWeyl.RootSystem.TheObjects[j];
      if (current==theOpposite)
        tempRat+=2;
      else
      { int indexOfSum=theWeyl.RootSystem.IndexOfObjectHash(current+theRoot);
        if (indexOfSum!=-1)
          tempRat+=(theOwner.ChevalleyConstants.elements[i][j]*theOwner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
      }
    }
    tempRat+=2;
    tempRat= 1/tempRat;
    tempElt2.MakeOneGeneratorCoeffOne(theOpposite, numVars, theOwner);
    tempElt1.MakeOneGeneratorCoeffOne(theRoot, numVars, theOwner);
    tempElt2*=tempElt1;

    tempElt2*=tempRat;
    *this+=tempElt2;*/
 //The ninja formula alternative implementation:
    root& theRoot=theWeyl.RootSystem.TheObjects[i];
    tempElt2.MakeOneGeneratorCoeffOne(-theRoot, numVars, theOwner);
    tempElt1.MakeOneGeneratorCoeffOne(theRoot, numVars, theOwner);
    tempElt2*=tempElt1;
    tempElt2*=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[i])/2;
    *this+=tempElt2;
  }
  *this/=theWeyl.GetKillingDivTraceRatio();
// Check that the ninja formula is correct:
/*  PolynomialOutputFormat tempPolyFormat;
  tempPolyFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  std::cout << "Killing divided by trace ratio:" << theWeyl.GetKillingDivTraceRatio().ElementToString();
  std::cout << "<br>casimir: " << this->ElementToString(false, false, theGlobalVariables, tempPolyFormat);
  std::cout << "<br>check element: " << checkElement.ElementToString(false, false, theGlobalVariables, tempPolyFormat);
*/
//  std::cout << "<br> check element minus casimir=" << checkElement.ElementToString(false, false, theGlobalVariables, tempPolyFormat);
  //this->DebugString=out.str();
/*  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (!length1Explored)
    { length1= theWeyl.RootScalarCartanRoot(tempRoot, tempRoot);
      length1Explored=true;
      coefficient1=0;
      for (int j=0; j<theWeyl.RootsOfBorel.size; j++)
      { coefficient1+=theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j])*theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j]);
        coef
      }
    }
  }*/
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::SubstitutionCoefficient
(PolynomialsRationalCoeff& theSub)
{ if (theSub.size<1)
    return;
  this->Coefficient.Substitution(theSub, theSub[0].NumVars);
  this->SetNumVariables(theSub[0].NumVars);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::SubstitutionCoefficients
(PolynomialsRationalCoeff& theSub, GlobalVariables* theContext,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ ElementUniversalEnveloping<CoefficientType> endResult;
  MonomialUniversalEnveloping<CoefficientType> currentMon;
  endResult.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { currentMon=this->TheObjects[i];
    currentMon.SubstitutionCoefficient(theSub);
    endResult.AddMonomial(currentMon);
  }
  if (theContext!=0)
    endResult.Simplify(*theContext, theRingUnit, theRingZero);
  else
  { GlobalVariables theGlobalVariables;
    endResult.Simplify(theGlobalVariables, theRingUnit, theRingZero);
  }
  this->operator=(endResult);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::Simplify
(GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ ElementUniversalEnveloping<CoefficientType> buffer;
  ElementUniversalEnveloping<CoefficientType> output;

  //this->ComputeDebugString();
  //return;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Simplify(buffer, theGlobalVariables, theRingUnit, theRingZero);
    output+=buffer;
    //output.ComputeDebugString();
  }
  *this=output;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnveloping<CoefficientType>& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddOnTopHash(input);
  else
    this->TheObjects[theIndex].Coefficient+=input.Coefficient;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::LieBracketOnTheRight
(const ElementUniversalEnveloping<CoefficientType>& right, ElementUniversalEnveloping<CoefficientType>& output)
{ ElementUniversalEnveloping<CoefficientType> tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::AddMonomial
(const MonomialUniversalEnveloping<CoefficientType>& input)
{ if (input.Coefficient.IsEqualToZero())
    return;
  int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddOnTopHash(input);
  else
  { this->TheObjects[theIndex].Coefficient+=input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexSwapWithLastHash(i);
      i--;
    }
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::MultiplyByGeneratorPowerOnTheRight
(int theGeneratorIndex, const CoefficientType& thePower)
{ if (thePower.IsEqualToZero())
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { this->Powers.LastObject()->operator+=(thePower);
      return;
    }
  this->Powers.AddOnTop(thePower);
  this->generatorsIndices.AddOnTop(theGeneratorIndex);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::MakeOneGenerator
(int theIndex, const CoefficientType& theCoeff, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(theOwner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.init(theOwner);
  tempMon.Coefficient=theCoeff;
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddOnTopHash(tempMon);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::MakeOneGeneratorCoeffOne
(int theIndex, int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(theOwner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.Coefficient.MakeNVarConst((int)numVars, (Rational) 1);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddOnTopHash(tempMon);
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ConvertToLieAlgebraElementIfPossible
(ElementSimpleLieAlgebra& output)const
{ output.Nullify(*this->owner);
  int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  Rational tempRat=0;
  PolynomialRationalCoeff tempP;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& tempMon= this->TheObjects[i];
    tempMon.GetDegree(tempP);
    if (!tempP.IsEqualToOne())
      return false;
    if (tempMon.Coefficient.TotalDegree()>0)
      return false;
    if (tempMon.Coefficient.size>0)
      tempRat=tempMon.Coefficient.TheObjects[0].Coefficient;
    else
      tempRat=0;
    int theGeneratorIndex=tempMon.generatorsIndices.TheObjects[0];
    int correspondingRootIndex=this->owner->ChevalleyGeneratorIndexToRootIndex(theGeneratorIndex);
    if(correspondingRootIndex<0)
    { theGeneratorIndex-=numPosRoots;
      output.Hcomponent.TheObjects[theGeneratorIndex]=tempRat;
    } else
      output.SetCoefficient(this->owner->theWeyl.RootSystem.TheObjects[correspondingRootIndex], tempRat, *this->owner);
  }
  return true;
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::AssignElementLieAlgebra
(const ElementSimpleLieAlgebra& input, int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(theOwner);
  this->AssignElementCartan(input.Hcomponent, numVars, theOwner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  tempMon.Powers.TheObjects[0].MakeNVarConst((int)numVars, 1);
  for (int i=0; i<input.NonZeroElements.CardinalitySelection; i++)
  { int theIndex=input.NonZeroElements.elements[i];
    int theGeneratorIndex=theOwner.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(theIndex);
    tempMon.Coefficient.MakeNVarConst((int)numVars, input.coeffsRootSpaces.TheObjects[theIndex]);
    tempMon.generatorsIndices.TheObjects[0]=theGeneratorIndex;
    this->AddOnTopHash(tempMon);
  }
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelations(theContext, subHiGoesToIthElement, theRingUnit, theRingZero);
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::ModOutVermaRelationS(bool SubHighestWeightWithZeroes, GlobalVariables& theGlobalVariables)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelationS(SubHighestWeightWithZeroes, theGlobalVariables);
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::GetCoordinateFormOfSpanOfElements
  (int numVars, List<ElementUniversalEnveloping<CoefficientType> >& theElements,
   List<rootPoly>& outputCoordinates, ElementUniversalEnveloping<CoefficientType>& outputCorrespondingMonomials,
   GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddOnTopNoRepetitionHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSize(theElements.size);
  PolynomialRationalCoeff ZeroPoly;
  ZeroPoly.Nullify((int)numVars);
  for (int i=0; i<theElements.size; i++)
  { rootPoly& current=outputCoordinates.TheObjects[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnveloping& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping<CoefficientType>& currentMon=currentElt.TheObjects[j];
      current.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::AssignElementCartan
(const root& input, int numVars, SemisimpleLieAlgebra& theOwner)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  this->Nullify(theOwner);
  tempMon.Nullify(numVars, theOwner);
  int theDimension= theOwner.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theOwner.theWeyl.RootsOfBorel.size;
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  for (int i=0; i<theDimension; i++)
    if (!input.TheObjects[i].IsEqualToZero())
    { (*tempMon.generatorsIndices.LastObject())=i+numPosRoots;
      tempMon.Powers.LastObject()->MakeNVarConst((int)numVars, 1);
      tempMon.Coefficient.MakeNVarConst((int)numVars, input.TheObjects[i]);
      this->AddOnTopHash(tempMon);
    }
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::ElementToString
(std::string& output, bool useRootIndices, bool includeEpsilonCoords, GlobalVariables& theGlobalVariables,
 PolynomialOutputFormat& theFormat)const
{ std::stringstream out;
  std::string tempS;
  if (this->size==0)
    out << "0";
  int IndexCharAtLastLineBreak=0;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& current=this->TheObjects[i];
    tempS=current.ElementToString
    (useRootIndices, includeEpsilonCoords, theGlobalVariables, theFormat);
    if (i!=0)
      if (tempS.size()>0)
        if (tempS[0]!='-')
          out << '+';
    out << tempS;
    if (((int)out.tellp())- IndexCharAtLastLineBreak> 150)
    { IndexCharAtLastLineBreak=out.tellp();
      out << "\\\\&&";
    }
  }
  output=out.str();
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::SimplifyAccumulateInOutputNoOutputInit
(ElementUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ int IndexlowestNonSimplified=0;
  ElementUniversalEnveloping<CoefficientType> buffer2;
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified<output.size)
  { bool reductionOccurred=false;
    if (output[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred=true;
    else
      for (int i=0; i<output[IndexlowestNonSimplified].generatorsIndices.size-1; i++)
        if (!this->owner->AreOrderedProperly
            (output[IndexlowestNonSimplified].generatorsIndices[i],
             output[IndexlowestNonSimplified].generatorsIndices[i+1]))
        { if (output[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute(i, tempMon, theGlobalVariables))
          { output.AddMonomialNoCleanUpZeroCoeff(tempMon);
            //tempMon.ComputeDebugString(theGlobalVariables);
            reductionOccurred=true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed
              (output[IndexlowestNonSimplified].Powers[i], output[IndexlowestNonSimplified].generatorsIndices[i],
               output[IndexlowestNonSimplified].Powers[i+1],
               output[IndexlowestNonSimplified].generatorsIndices[i+1]))
          { output[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft
            (i, buffer2, theRingUnit, theRingZero);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2[j]);
//            output.ComputeDebugString(theGlobalVariables);
            reductionOccurred=true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed
              (output[IndexlowestNonSimplified].Powers[i],
               output[IndexlowestNonSimplified].generatorsIndices[i],
               output[IndexlowestNonSimplified].Powers[i+1],
               output[IndexlowestNonSimplified].generatorsIndices[i+1]))
          { output[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight
            (i, buffer2, theRingUnit, theRingZero);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2[j]);
//            output.ComputeDebugString(theGlobalVariables);
            reductionOccurred=true;
            break;
          }
        }
    if (reductionOccurred)
      output.PopIndexSwapWithLastHash(IndexlowestNonSimplified);
    else
      IndexlowestNonSimplified++;
//    output.ComputeDebugString(theGlobalVariables);
  }
  output.CleanUpZeroCoeff();
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::CommutingLeftIndexAroundRightIndexAllowed
(CoefficientType& theLeftPower, int leftGeneratorIndex,
 CoefficientType& theRightPower, int rightGeneratorIndex)
{ if (!theLeftPower.IsSmallInteger())
    return false;
  if(theRightPower.IsSmallInteger())
    return true;
  int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  int theDimension= this->owner->theWeyl.CartanSymmetric.NumRows;
  if(rightGeneratorIndex>= numPosRoots && rightGeneratorIndex<numPosRoots+theDimension)
  { if (this->owner->theLiebracketPairingCoefficients.elements[leftGeneratorIndex][rightGeneratorIndex].IsEqualToZero())
      return true;
    else
      return false;
  } else
    return true;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex, MonomialUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables)
{ if (theLeftIndex>= this->generatorsIndices.size-1)
    return false;
  int leftGenerator=this->generatorsIndices.TheObjects[theLeftIndex];
  int rightGenerator=this->generatorsIndices.TheObjects[theLeftIndex+1];
  if (this->owner->OppositeRootSpaces.TheObjects[leftGenerator]==rightGenerator)
    return false;
  if (this->owner->theLiebracketPairingCoefficients.elements[leftGenerator][rightGenerator].IsEqualToZero())
  { //this->ComputeDebugString(theGlobalVariables);
    output.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.init(*this->owner);
    output.Coefficient=this->Coefficient;
    //output.ComputeDebugString(theGlobalVariables);
    for (int i=0; i<theLeftIndex; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex+1], this->Powers.TheObjects[theLeftIndex+1]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex], this->Powers.TheObjects[theLeftIndex]);
    for (int i=theLeftIndex+2; i<this->generatorsIndices.size; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    return true;
  }
  return false;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::CommutingRightIndexAroundLeftIndexAllowed
(CoefficientType& theLeftPower, int leftGeneratorIndex,
 CoefficientType& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed
(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::Simplify
(ElementUniversalEnveloping<CoefficientType>& output, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero )
{ output.Nullify(*this->owner);
  output.AddOnTopHash(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theGlobalVariables, theRingUnit, theRingZero);
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::MultiplyByNoSimplify
(const MonomialUniversalEnveloping<CoefficientType>& standsOnTheRight)
{ this->generatorsIndices.MakeActualSizeAtLeastExpandOnTop(standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
  this->Powers.MakeActualSizeAtLeastExpandOnTop(standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(standsOnTheRight.Coefficient);
  if (standsOnTheRight.generatorsIndices.size==0)
    return;
  int firstIndex=standsOnTheRight.generatorsIndices.TheObjects[0];
  int i=0;
  if (this->generatorsIndices.size>0)
    if (firstIndex==(*this->generatorsIndices.LastObject()))
    { *this->Powers.LastObject()+=standsOnTheRight.Powers.TheObjects[0];
      i=1;
    }
  for (; i<standsOnTheRight.generatorsIndices.size; i++)
  { this->Powers.AddOnTop(standsOnTheRight.Powers.TheObjects[i]);
    this->generatorsIndices.AddOnTop(standsOnTheRight.generatorsIndices.TheObjects[i]);
  }
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::CommuteConsecutiveIndicesRightIndexAroundLeft
(int theIndeX, ElementUniversalEnveloping<CoefficientType>& output,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.init(*this->owner);
  tempMon.Coefficient=theRingZero;
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndeX= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndeX=this->generatorsIndices.TheObjects[theIndeX];
  CoefficientType theRightPoweR, theLeftPoweR;
  theRightPoweR= this->Powers.TheObjects[theIndeX+1];
  theLeftPoweR= this->Powers.TheObjects[theIndeX];
  theRightPoweR-=1;
  int powerDroP=0;
  CoefficientType acquiredCoefficienT;
  acquiredCoefficienT.Assign(this->Coefficient);
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  MonomialUniversalEnveloping<CoefficientType> startMon;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResulT, tempElT, tempLefttElt;
  adResulT.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(rightGeneratorIndeX, *this->owner);
  tempLefttElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(leftGeneratorIndeX, *this->owner);
  do
  { //acquiredCoefficienT.ComputeDebugString();
    //theRightPoweR.ComputeDebugString();
    //theLeftPoweR.ComputeDebugString();
    //adResulT.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    if (adResulT.NonZeroElements.CardinalitySelection>0)
    { int theNewGeneratorIndex= this->owner->RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(adResulT.NonZeroElements.elements[0]);
      tempMon.Coefficient=acquiredCoefficienT;
      tempMon.Coefficient*=(adResulT.coeffsRootSpaces[adResulT.NonZeroElements.elements[0]]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
      //tempMon.ComputeDebugString();
      output.AddOnTopHash(tempMon);
    } else
    { int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
      int numPosRoots= this->owner->theWeyl.RootsOfBorel.size;
      for (int i=0; i<theDimension; i++)
        if (!adResulT.Hcomponent[i].IsEqualToZero())
        { tempMon=startMon;
          tempMon.Coefficient=acquiredCoefficienT;
          tempMon.Coefficient*=(adResulT.Hcomponent[i]);
          tempMon.MultiplyByGeneratorPowerOnTheRight(i+numPosRoots, theRingUnit);
          tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
          tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
          for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
            tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
          //tempMon.ComputeDebugString();
          output.AddOnTopHash(tempMon);
        }
    }
    acquiredCoefficienT.MultiplyBy(theLeftPoweR);
    theLeftPoweR-=1;
    tempMon=startMon;
    this->owner->LieBracket(tempLefttElt, adResulT, tempElT);
    adResulT=tempElT;
    powerDroP++;
    acquiredCoefficienT/=(powerDroP);
    //adResulT.ComputeDebugString(*this->owner, false, false);
  }while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::CommuteConsecutiveIndicesLeftIndexAroundRight
(int theIndeX, ElementUniversalEnveloping<CoefficientType>& output,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.init(*this->owner);
  tempMon.Coefficient=theRingZero;
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndex= this->generatorsIndices[theIndeX+1];
  int leftGeneratorIndex=this->generatorsIndices[theIndeX];
  CoefficientType theRightPower, theLeftPower;
  theRightPower= this->Powers[theIndeX+1];
  theLeftPower= this->Powers[theIndeX];
  theLeftPower-=1;
  int powerDrop=0;
  CoefficientType acquiredCoefficient;
  acquiredCoefficient.Assign(this->Coefficient);
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theIndeX], theLeftPower);
  MonomialUniversalEnveloping<CoefficientType> startMon, tempMon2;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResult, tempElt, tempRightElt;
  adResult.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (leftGeneratorIndex, *this->owner);
  tempRightElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (rightGeneratorIndex, *this->owner);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  do
  { //acquiredCoefficient.ComputeDebugString();
    //theRightPower.ComputeDebugString();
    //adResult.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
    //tempMon.ComputeDebugString();
    if (adResult.NonZeroElements.CardinalitySelection>0)
    { int theNewGeneratorIndex= this->owner->RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(adResult.NonZeroElements.elements[0]);
      tempMon.Coefficient=acquiredCoefficient;
      tempMon.Coefficient*=(adResult.coeffsRootSpaces[adResult.NonZeroElements.elements[0]]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      //tempMon.ComputeDebugString();
      output.AddOnTopHash(tempMon);
    } else
    { int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
      int numPosRoots= this->owner->theWeyl.RootsOfBorel.size;
      tempMon.Coefficient=acquiredCoefficient;
      tempMon2=tempMon;
      for (int i=0; i<theDimension; i++)
        if (!adResult.Hcomponent[i].IsEqualToZero())
        { tempMon=tempMon2;
          tempMon.Coefficient*=(adResult.Hcomponent[i]);
          tempMon.MultiplyByGeneratorPowerOnTheRight(i+numPosRoots, theRingUnit);
          for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
            tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
          //tempMon.ComputeDebugString();
          output.AddOnTopHash(tempMon);
        }
    }
    acquiredCoefficient.MultiplyBy(theRightPower);
    theRightPower-=1;
    tempMon=startMon;
    this->owner->LieBracket(adResult, tempRightElt, tempElt);
    adResult=tempElt;
    powerDrop++;
    acquiredCoefficient/=(powerDrop);
    //adResult.ComputeDebugString(*this->owner, false, false);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::SetNumVariables(int newNumVars)
{//the below code is wrong messed up with substitutions!
  // if (this->Coefficient.NumVars==newNumVars)
  //  return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
}

template <class CoefficientType>
std::string MonomialUniversalEnveloping<CoefficientType>::ElementToString
(bool useRootIndices, bool useEpsilonCoords, GlobalVariables& theGlobalVariables,
 PolynomialOutputFormat& theFormat)
{ std::stringstream out;
  std::string tempS;
  if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->Coefficient.IsEqualToZero())
    tempS="0";
  else
    tempS = this->Coefficient.ElementToString(theFormat);
  if (this->generatorsIndices.size>0)
  { if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
  }
  if (this->Coefficient.ElementToStringNeedsBracketsForMultiplication())
    out << "(" << tempS << ")";
  else
    out << tempS;
  for (int i=0; i<this->generatorsIndices.size; i++)
  { CoefficientType& thePower=this->Powers.TheObjects[i];
    int theIndex=this->generatorsIndices.TheObjects[i];
    tempS=this->owner->GetLetterFromGeneratorIndex
    (theIndex, useRootIndices, useEpsilonCoords, theFormat, theGlobalVariables);
    //if (thePower>1)
    //  out << "(";
    out << tempS;
    if (!thePower.IsEqualToOne())
    { out << "^";
     // if (useRootIndices)
      out << "{";
      out << thePower.ElementToString(theFormat);
      //if (useRootIndices)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::ModOutVermaRelationS
(bool SubHighestWeightWithZeroes, GlobalVariables& theGlobalVariables)
{ int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  int theNumVars=this->Coefficient.NumVars;
//  this->ComputeDebugString(theGlobalVariables);
  if (theNumVars<theDimension && !SubHighestWeightWithZeroes)
  { this->SetNumVariables(theDimension);
    theNumVars=theDimension;
  }
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentRoot=this->owner->ChevalleyGeneratorIndexToRootIndex(generatorsIndices.TheObjects[i]);
    //ChevalleyGeneratorToRootIndex returns a negative value if the generator is an element of the Cartan
    //the order of roots is: first negative, then positive, in ascending grlex order.
    if (IndexCurrentRoot>=numPosRoots)
    { this->Nullify(theNumVars, *this->owner);
      return;
    }
    if (IndexCurrentRoot<numPosRoots && IndexCurrentRoot>=0)
      return;
    if (IndexCurrentRoot<0)//the generator is an element of the Cartan
    { if (this->Powers.TheObjects[i].TotalDegree()!=0)
        return;
      if (SubHighestWeightWithZeroes)
      { this->Nullify(theNumVars, *this->owner);
        return;
      }
      int theDegree = this->Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort;
      PolynomialRationalCoeff tempP;
      root tempRoot;
      tempRoot.MakeEi(theNumVars, this->owner->ChevalleyGeneratorIndexToElementCartanIndex(this->generatorsIndices.TheObjects[i]));
      tempP.MakeLinPolyFromRootNoConstantTerm(tempRoot);
      if (theDegree!=1)
        tempP.RaiseToPower(theDegree, (Rational) 1);
      this->Coefficient.MultiplyBy(tempP);
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::Nullify(int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Coefficient.Nullify((int)numVars);
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

template <class CoefficientType>
int MonomialUniversalEnveloping<CoefficientType>::HashFunction() const
{ int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result=0;
  for (int i=0; i<top; i++)
    result+=SomeRandomPrimes[i]*this->generatorsIndices.TheObjects[i];
  return result;
}

template <class CoefficientType>
bool ParserNode::GetRootRationalDontUseForFunctionArguments
(Vector<CoefficientType>& output, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType!=this->typeArray)
  { output.SetSize(1);
    if (!this->ConvertToType(this->typeRational, 0, theGlobalVariables))
      return false;
    output.TheObjects[0]=this->rationalValue;
    return true;
  }
  output.SetSize(this->children.size);
  for (int i=0; i<output.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeRational, this->impliedNumVars, theGlobalVariables))
      return false;
    output.TheObjects[i]=currentNode.rationalValue;
  }
  return true;
}

template <class CoefficientType>
bool ParserNode::GetListDontUseForFunctionArguments
(List<CoefficientType>& output, GlobalVariables& theGlobalVariables, int goalExpressionType, int impliedNumVars)
{ if (this->ExpressionType!=this->typeArray)
  { output.SetSize(1);
    if (!this->ConvertToType(goalExpressionType, impliedNumVars, theGlobalVariables))
      return false;
    output.TheObjects[0]=this->rationalValue;
    return true;
  }
  output.SetSize(this->children.size);
  for (int i=0; i<output.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children.TheObjects[i]];
    if (!currentNode.ConvertToType(goalExpressionType, this->impliedNumVars, theGlobalVariables))
      return false;
    output.TheObjects[i]=currentNode.GetElement<CoefficientType>();
  }
  return true;
}
#endif

