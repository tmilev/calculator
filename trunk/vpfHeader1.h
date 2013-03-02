//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_h_already_included
#define vpfHeader1_h_already_included

#include "vpfMacros.h"
//static ProjectInformationInstance vpfHeader1instance(__FILE__, "Main header. Math and general routines. ");

//IMPORTANT.
//Convention on Hash functions.
//1. C++ objects that represent mathematically equal objects
//   are allowed to have different bit representations in RAM memory.
//2. Mathematically equal objects must have their object::HashFunction return identical
//   values for mathematically identical objects, even if they are bitwise different.
//3. Mathematical objects representing 0 in an abelian group
//   must have their hash function return 0.
//
//In particular, zero rational numbers, zero polynomials/monomial collections/elements
//of semisimple Lie algebras, etc. must have their hash functions return 0.
//Motivation: for speed purposes, it is not desirable to fix unique
//bitwise representations in RAM memory for mathematically equal objects.
//For example, this means that monomials in a polynomial do not
//need to be ordered in a specific order. Yet polynomials can be quickly compared by
//computing their hash functions. Here, as an example we may give two 1-million monomial polynomials
//(computed, say, by adding two 0.5 million monomial polynomials in two different orders). The
//polynomials' monomials are ordered differently, yet the polynomials can quickly be compared.
//Note that sorting N monomials of an arbitrary polynomial is at best O(N log(N) ) operations, while
//computing the hash functions is only O(N) operations.

//used for hashing various things.
const int SomeRandomPrimesSize= 25;
const int SomeRandomPrimes[SomeRandomPrimesSize]=
{ 607,  1013, 2207, 3001, 4057, 5419, 5849, 6221,  7057, 7411,
  7417, 7681, 7883, 8011, 8209, 8369, 8447, 9539, 10267, 10657,
  11489, 12071, 12613, 13933, 14759
};

//the following class is for buffers, i/o function pointers, multitasking, general purpose.
class GlobalVariables;

//Simple math routines (min, max, etc.) that I have not been able to find a better placement for
class MathRoutines;

//Rationals and integers:
class LargeIntUnsigned;
class LargeInt;
class Rational;

//More involved mathematical types
template<class Base>
class CompleX;
class RationalFunctionOld;
class SemisimpleLieAlgebra;
class rootSubalgebra;
class rootSubalgebras;
class ChevalleyGenerator;
class DynkinDiagramRootSubalgebra;
template<class CoefficientType>
class ElementSemisimpleLieAlgebra;
template<class CoefficientType>
class ElementUniversalEnveloping;
template <class CoefficientType, unsigned int inputHashFunction(const CoefficientType&)>
class MonomialTensor;
template<class CoefficientType>
class MonomialUniversalEnveloping;
template<class CoefficientType>
class ElementSumGeneralizedVermas;
template <class CoefficientType>
class charSSAlgMod;
class ReflectionSubgroupWeylGroup;
template<class CoefficientType>
class ModuleSSalgebra;
class SltwoSubalgebras;
template<class CoefficientType>
class MonomialTensorGeneralizedVermas;
template<class CoefficientType>
class ElementTensorsGeneralizedVermas;
struct branchingData;
class quasiDiffMon;
template<class CoefficientType>
class quasiDiffOp;

//classes related to linear integral programming (polyhedra, lattices, quasipolynomials)
class Cone;
class ConeComplex;
class Lattice;
class affineHyperplane;
class affineCones;

//Hybrid classes that serve both memory-management and mathematical purposes
//(Matrices, Vectors, PolynomialSubstitution, etc.)
template <class ObjectType1, class ObjectType2,
unsigned int hashFunction1(const ObjectType1&)=ObjectType1::HashFunction,
unsigned int hashFunction2(const ObjectType2&)=ObjectType2::HashFunction
>
class Pair;
template <class Object>
class List;
template <class Object>
class Matrix;
template <class Object, unsigned int hashFunction(const Object&)>
class HashedList;
template <class CoefficientType>
class Vector;
template <class CoefficientType>
class Vectors;
class MonomialP;
template <class TemplateMonomial, class CoefficientType>
class MonomialCollection;
template <class CoefficientType>
class Polynomial;
template <class CoefficientType>
class PolynomialSubstitution;

//combinatorial classes
class Selection;
class SubsetWithMultiplicities;

//the following classes have to do with user interface/displaying/drawing information
class FormatExpressions;
struct IndicatorWindowVariables;
class DrawingVariables;
class DrawOperations;
class XMLRoutines;
struct CGI;

//The calculator parsing routines:
class CommandList;
class Function;
class Expression;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The documentation of pthreads.h can be found at:
// https://computing.llnl.gov/tutorials/pthreads/#MutexOverview
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The below class is a wrapper for mutexes. All system dependent machinery for
//mutexes should be put here.
//MutexWrapper specification:
//The mutex has two states: locked and unlocked.
//When the caller calls UnlockMe() this unlocks the mutex if it were locked,
//otherwise does nothing, and immediately returns.
//When the caller calls LockMe() there are two cases.
//1) First, If the mutex is unlocked, the mutex state changes to
//locked and execution of the caller continues.
//The preceding two operations are atomic: if the mutex happens to be unlocked,
//no other processor instruction
//can be executed before the mutex's state is changed to locked.
//2) Second, if the mutex is locked, the calling thread must pause execution,
//without consuming computational/processor power.
// As soon as the mutex is unlocked (by another thread or by the system),
//the calling thread is allowed to wake up and perform the sequence described in 1).
// The wake-up time is unspecified/not guaranteed to be immediate: another thread
//might "jump in" and overtake, again locking the calling thread.
// In order to have guaranteed wake-up when coordinating two threads only, use the
//controller object (which uses two mutexes to achieve guaranteed wake-up).
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is not guaranteed to work on Windows. Might cause crash.
//Must be fixed to a proper set of Windows routines.
//This is not possible at the moment since none of my legally owned (but outdated)
//versions of Windows support the multitasking routines
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
  //locks the mutex if the mutex is free. If not it suspends calling thread until
  //mutex becomes free and then locks it.
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

//this class is used as a custom completely portable
//stack trace log.
class RegisterFunctionCall
{ public:
  RegisterFunctionCall(const char* fileName, int line, const std::string& functionName="");
  ~RegisterFunctionCall();
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
      ParallelComputing::flagUngracefulExitInitiated=true;
      tempMutex.UnlockMe();
      std::cout << "<b>Error:</b> Number of pointers allocated exceeded allowed <b>limit of " <<
      ParallelComputing::cgiLimitRAMuseNumPointersInList << ".</b>\n<br><b>Signalling ungraceful exit...</b>";
      std::cout.flush();

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
  int InvertModN(int X, int N)
  { int q, r, p, d; // d - divisor, q - quotient, r - remainder, p is the number to be divided
    int vD[2], vP[2], temp;
    vP[0]=1; vP[1]=0; // at any given moment, p=vP[0]*N+vP[1]*X
    vD[0]=0; vD[1]=1;   // at any given moment, d=vD[0]*N+vD[1]*X
    p=N; d=X; d%=N; if (d<0){d+=N; }
    while (d>0)
    { q=p/d;
      r=p%d;
      p=d;
      d=r;
      for(int i=0; i<2; i++)
      { temp=vP[i];
        vP[i]= vD[i];
        vD[i]= temp-q*vD[i];
      }
    }
    assert(p==1); //if d and p were relatively prime this should be so. Otherwise the function was not called properly.
    p=vP[1]%N;
    if (p<0)
      p+=N;
    return p;
  }
  static int lcm(int a, int b);
  static int TwoToTheNth(int n);
  static bool isADigit(const std::string& input, int& whichDigit)
  { if (input.size()!=1)
      return false;
    return MathRoutines::isADigit(input[0], whichDigit);
  }
  static inline bool isADigit(char theChar, int& whichDigit)
  { whichDigit=theChar-'0';
    return whichDigit<10 && whichDigit>=0;
  }
//  static void NChooseK(int n, int k, LargeInt& output);//
  static void NChooseK(int n, int k, LargeInt& result);
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
  template <typename CoefficientType>
  inline static CoefficientType Maximum
  (const CoefficientType& a, const CoefficientType& b)
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
  { if (!input.NeedsBrackets())
      return input.ToString();
    std::string result;
    result.append("(");
    result.append(input.ToString());
    result.append(")");
    return result;
  }
  template<class Element>
  static inline std::string ElementToStringBrackets(const Element& input, FormatExpressions* theFormat)
  { if (!input.NeedsBrackets())
      return input.ToString(theFormat);
    std::string result;
    result.append("(");
    result.append(input.ToString(theFormat));
    result.append(")");
    return result;
  }
  inline static unsigned int HashDouble(const double& input)
  { return (unsigned) (input*10000);
  }
  inline static unsigned int IntUnsignIdentity(const int& input)
  { return (unsigned) input;
  }
  static unsigned int hashString(const std::string& x)
  { int numCycles=x.size();
    if (numCycles>SomeRandomPrimesSize)
      numCycles=SomeRandomPrimesSize;
    unsigned int result=0;
    for (int i=0; i<numCycles; i++)
      result+=x[i]*SomeRandomPrimes[i];
    return result;
  }
  template <class Element>
  static void LieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output);
  //this function just redirects to the class CGI function with the same name.
  //Reason: cannot use an incomplete CGI type.
  static std::string GetStackTraceEtcErrorMessage(const std::string& file, int line);
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
    else
      this->FreeMemory();
  }
  const Object& GetElementConst()const;
  Object& GetElement();
  bool operator==(const MemorySaving<Object>& other)const
  { if (this->IsZeroPointer()!=other.IsZeroPointer())
      return false;
    if (this->IsZeroPointer())
      return true;
    return this->GetElementConst()==other.GetElementConst();
  }
  int HashFunction()const
  { if (this->IsZeroPointer())
      return 0;
    return this->GetElementConst().HashFunction();
  }
  static inline int HashFunction(const MemorySaving<Object>& input)
  { return input.HashFunction();
  }
  bool IsZeroPointer()const{return this->theValue==0;}
  void FreeMemory();
  MemorySaving(){this->theValue=0;}
  ~MemorySaving();
};

class RecursionDepthCounter
{
public:
  int* theCounter;
  RecursionDepthCounter(int* inputCounter): theCounter(0)
  { if (inputCounter==0)
      return;
    this->theCounter=inputCounter;
    (*this->theCounter)++;
  }
  ~RecursionDepthCounter()
  { if (this->theCounter!=0)
      (*this->theCounter)--;
    this->theCounter=0;
  }
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
  inline bool Contains(const Object& o)
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
std::ostream& operator<<(std::ostream& output, const List<Object>& theList)
{ output << "size: " << theList.size << "\n";
  for (int i=0; i<theList.size; i++)
    output << theList[i] << " ";
  return output;
}

template <class Object>
std::iostream& operator>>(std::iostream& input, List<Object>& theList)
{ std::string tempS; int tempI;
  input >> tempS >> tempI;
  assert(tempS=="size:");
  theList.SetSize(tempI);
  for (int i=0; i<theList.size; i++)
    input >> theList[i];
  return input;
}

static unsigned int NumListsCreated=0;
static unsigned int NumListResizesTotal=0;
//List serves the same purpose as std::vector
template <class Object>
class List
{ friend std::ostream& operator<< <Object>(std::ostream& output, const List<Object>& theList);
  friend std::iostream& operator>> <Object>(std::iostream& input, List<Object>& theList);
public:
  typedef bool (*OrderLeftGreaterThanRight) (const Object& left, const Object& right)
  ;
private:
  friend class Polynomial<Rational> ;
  friend class IntegerPoly;
  friend class partFractions;
  friend class partFraction;
  void ExpandArrayOnTop(int increase);
  void QuickSortAscendingNoOrder(int BottomIndex, int TopIndex);
  //submitting zero comparison function is not allowed!
  //that is why the function is private.
  void QuickSortAscendingOrder
  (int BottomIndex, int TopIndex, List<Object>::OrderLeftGreaterThanRight theOrder);
  void QuickSortDescending(int BottomIndex, int TopIndex);
  inline void initConstructorCallOnly()
  { this->TheObjects=0;
    this->ActualSize=0;
    this->size=0;
    MacroIncrementCounter(NumListsCreated);
  }
  int ActualSize;
public:
  Object* TheObjects;
  int size;
  List(const List<Object>& other)
  { this->initConstructorCallOnly();
    *this=(other);
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
//  void AddOnTop(Object o);
  inline int GetNewSizeRelativeToExpectedSize(int expectedSize)const
  { if (expectedSize==1)
      return 1;
    if (expectedSize==2)
      return 2;
    return (expectedSize*4)/3+1;
  }
  void SetExpectedSize(int theSize)
  { if ((this->ActualSize)*5/6<theSize)
      this->ReservE(this->GetNewSizeRelativeToExpectedSize(theSize));
  }
  void AssignLight(const ListLight<Object>& from);
  void ExpandOnTop(int theIncrease)
  { int newSize=this->size+theIncrease;
    if(newSize<0)
      newSize=0;
    this->SetSize(newSize);
  }
  void SetSize(int theSize);// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void SetSizeMakeMatrix(int numRows, int numCols)
  { this->SetSize(numRows);
    for (int i=0; i<numRows; i++)
      this->TheObjects[i].SetSize(numCols);
  }
  void initFillInObject(int theSize, const Object& o);
  inline void AddObjectOnTopCreateNew();
  void ReservE(int theSize);// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void SortedInsert(const Object& o)
  { this->InsertAtIndexShiftElementsUp(o,this->BSExpectedIndex(o));
  }
  void InsertAtIndexShiftElementsUp(const Object& o, int desiredIndex)
  { this->AddOnTop(o);
    for(int i=this->size-1; i>desiredIndex; i--)
    { this->SwapTwoIndices(i,i-1);
    }
  }
  void AddOnTop(const Object& o);
  void AddListOnTop(const List<Object>& theList);
  bool AddOnTopNoRepetition(const Object& o);
  void AddOnTopNoRepetition(const List<Object>& theList)
  { this->SetExpectedSize(this->size+theList.size);
    for (int i=0; i<theList.size; i++)
      this->AddOnTopNoRepetition(theList[i]);
  }
  int AddNoRepetitionOrReturnIndexFirst(const Object& o)
  { int indexOfObject=this->GetIndex(o);
    if (indexOfObject==-1)
    { this->AddOnTop(o);
      return this->size-1;
    }
    return indexOfObject;
  }
  void RemoveIndexShiftDown(int index)
  { for (int i=index; i<this->size-1; i++)
      this->TheObjects[i]=this->TheObjects[i+1];
    this->size--;
  }
  void RemoveIndexSwapWithLast(int index);
  void RemoveLastObject();
  // the below function is named a bit awkwardly because otherwise there is a risk of confusion
  // with the RemoveIndexSwapWithLast when selecting from autocomplete list. This cost me already 2 hours of lost time,
  // so the awkward name is necessary.
  void RemoveFirstOccurenceSwapWithLast(const Object& o);
  Object PopLastObject();
  Object PopIndexSwapWithLast(int index)
  { Object result;
    result=(*this)[index];
    this->RemoveIndexSwapWithLast(index);
    return result;
  }
  Object PopIndexShiftDown(int index)
  { Object result;
    result=(*this)[index];
    this->RemoveIndexShiftDown(index);
    return result;
  }
  //If comparison function is not specified, QuickSortAscending usese operator>, else it uses the given
  //comparison function
  void QuickSortAscending(List<Object>::OrderLeftGreaterThanRight theOrder=0)
  { if (this->size==0)
      return;
    if (theOrder==0)
      this->QuickSortAscendingNoOrder(0, this->size-1);
    else
      this->QuickSortAscendingOrder(0, this->size-1, theOrder);
  }
  void QuickSortDescending(List<Object>::OrderLeftGreaterThanRight theOrder=0)
  { this->QuickSortAscending(theOrder);
    this->ReverseOrderElements();
  }
  bool HasACommonElementWith(List<Object>& right);
  void SwapTwoIndices(int index1, int index2);
  std::string ToString(FormatExpressions* theFormat)const;
  std::string ToString()const;
  void ToString(std::string& output, FormatExpressions* theFormat=0)const
  { output= this->ToString(theFormat);
  }
  int GetIndex(const Object& o) const;
  bool Contains(const Object& o)const
  { return this->GetIndex(o)!=-1;
  }
  // Perform a binary search, assuming the list is sorted
  bool BSContains(const Object& o) const
  { return this->BSGetIndex(o)!=-1;
  }
  int BSGetIndex(const Object& o) const
  { int n = this->BSExpectedIndex(o);
    return (this->TheObjects[n]==o) ? n : -1;
  }
  int BSExpectedIndex(const Object& o) const{
    int i = 0, j = this->size;
    while(true){
        int s=j-i;
        int n = i+s/2;
        if(s<0)
            return n;
        if(o==this->TheObjects[n])
            return n;
        if(o>this->TheObjects[n]){
            if(i<n){
                i=n;
                continue;
            }
            i = n+1;
        }
        if(o<this->TheObjects[n]){
            if(n<j){
                j=n;
                continue;
            }
            j = n-1;
        }
    }
  }
  bool ReadFromFile(std::fstream& input){ return this->ReadFromFile(input, 0, -1);}
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){return this->ReadFromFile(input, theGlobalVariables, -1);}
  void WriteToFile(std::fstream& output){this->WriteToFile(output, 0, -1);}
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output, theGlobalVariables, -1);}
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes);
//  inline bool Contains(Object& o){return this->Contains(o)!=-1; };
  int SizeWithoutObjects();
  inline Object* LastObject()const;// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void ReleaseMemory();
  int HashFunction()const
  { int numCycles=MathRoutines::Minimum(SomeRandomPrimesSize, this->size);
    int result=0;
    for (int i=0; i<numCycles; i++)
      result+=SomeRandomPrimes[i]*this->TheObjects[i].HashFunction();
    return result;
  }
  static inline int HashFunction(const List<Object>& input)
  { return input.HashFunction();
  }
  void IntersectWith(const List<Object>& other, List<Object>& output)const;
  void operator=(const List<Object>& right)
  { if (this==&right)
      return;
    this->SetSize(right.size);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i]= right.TheObjects[i];
  }
  template <class otherObjectType>
  void operator=(const List<otherObjectType>& right)
  { this->SetSize(right.size);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i]= right.TheObjects[i];
  }
  static void swap(List<Object>& l1, List<Object>& l2);
  void ReverseOrderElements();
  bool IsEqualTo(const List<Object>& Other)const
  { if (this->size!=Other.size)
      return false;
    for (int i=0; i<Other.size; i++)
      if (!(this->TheObjects[i]==Other.TheObjects[i]))
        return false;
    return true;
  }
  inline Object& operator[](int i)const
  { if (i>=this->size || i<0)
    { std::cout << "Programming error: attempting to access the entry of index " << i << " in an array of "
      << this->size << " elements. " << MathRoutines::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return this->TheObjects[i];
  }
  inline bool operator!=(const List<Object>& other)const{ return !this->IsEqualTo(other);}
  inline bool operator==(const List<Object>& other)const{ return this->IsEqualTo(other);}
  bool operator>(const List<Object>& other)const;
  void ShiftUpExpandOnTop(int StartingIndex);
  List(int StartingSize);
  List(int StartingSize, const Object& fillInValue);
  List();//<-newly constructed lists start with size=0; This default is used in critical places in HashedList and other classes, do not change!
  ~List();
  void AssignListList(const List<List<Object> >& input)
  { int count=0;
    for (int i=0; i<input.size; i++)
      count+=input[i].size;
    this->SetSize(0);
    this->Reserve(count);
    for (int i=0; i<input.size; i++)
      for (int j=0; j<input[i].size; j++)
        this->AddOnTop(input[i][j]);
  }
};

struct stackInfo
{
public:
  std::string fileName;
  int line;
  std::string functionName;
  void operator=(const stackInfo& other)
  { this->fileName=other.fileName;
    this->line=other.line;
    this->functionName=other.functionName;
  }
};

class ProjectInformation
{
  public:
  static ProjectInformation& GetMainProjectInfo()
  { //This is required to avoid the static initialization order fiasco.
    //For more information, google "static initialization order fiasco"
    //and go to the first link. The solution used here was proposed inside that link.
    static ProjectInformation MainProjectInfo;
    MainProjectInfo.CustomStackTrace.ReservE(30);
    MainProjectInfo.FileNames.ReservE(30);
    MainProjectInfo.FileDescriptions.ReservE(30);
    return MainProjectInfo;
  }
  List<stackInfo> CustomStackTrace;
  List<std::string> FileNames;
  List<std::string> FileDescriptions;
  std::string ToString();
  MutexWrapper infoIsInitialized;
  void AddProjectInfo(const std::string& fileName, const std::string& fileDescription);
  std::string GetStackTraceReport();
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
  static std::string RemovePathFromFileName(const std::string& fileName)
  { unsigned startNameWithoutFolderInfo=0;
    for (unsigned i=0; i<fileName.size(); i++)
      if (fileName[i]=='/' || fileName[i]=='\\')
        startNameWithoutFolderInfo=i+1;
    std::stringstream nameWithoutFolderInfo;
    for (unsigned i=startNameWithoutFolderInfo; i<fileName.size(); i++)
      nameWithoutFolderInfo << fileName[i];
    return nameWithoutFolderInfo.str();
  }
  inline static std::string GetHtmlLinkFromProjectFileName(const std::string& fileName)
  { return CGI::GetHtmlLinkFromProjectFileName(fileName, "");
  }
  static std::string GetCalculatorLink
  (const std::string& DisplayNameCalculator, const std::string& input)
  ;
  static std::string GetAnimateShowHideJavascriptMustBEPutInHTMLHead();
  static std::string GetSliderSpanStartsHidden(const std::string& content, const std::string& label="Expand/collapse", const std::string& desiredID="");
  static std::string GetHtmlLinkFromProjectFileName(const std::string& fileName, const std::string& fileDesc);
  static std::string GetStackTraceEtcErrorMessage(const std::string& file, int line);
  static std::string GetHtmlSwitchMenuDoNotEncloseInTags(const std::string& serverBase)
  { std::stringstream output;
    output << "<script src=\"" << serverBase << "jsmath/easy/load.js\"></script> ";
    output << " <script type=\"text/javascript\"> \n";
    output << " function switchMenu(obj)\n";
    output << " { var el = document.getElementById(obj);	\n";
    output << "   if ( el.style.display != \"none\" ) \n";
    output << "     el.style.display = 'none';\n";
    output << "   else \n";
    output << "     el.style.display = '';\n";
    output << " }\n";
    output << "</script>";
    return output.str();
  }
  static std::string GetLatexEmbeddableLinkFromCalculatorInput
(const std::string& address, const std::string& display)
  ;
  static bool GetHtmlStringSafeishReturnFalseIfIdentical(const std::string& input, std::string& output);
  static void TransormStringToHtmlSafeish(std::string& theString){std::string tempS; CGI::GetHtmlStringSafeishReturnFalseIfIdentical(theString, tempS); theString=tempS; }
  static std::string GetHtmlMathDivFromLatexFormulA(const std::string& input)
  { return CGI::GetHtmlMathFromLatexFormulA(input, "", "", true, false);
  }
  static std::string GetHtmlMathDivFromLatexAddBeginArrayL(const std::string& input)
  { return  CGI:: GetHtmlMathFromLatexFormulA(input, "", "", true, true);
  }
  static std::string GetHtmlMathSpanFromLatexFormulaAddBeginArrayL(const std::string& input)
  { return  CGI:: GetHtmlMathFromLatexFormulA(input, "", "", false, true);
  }
  static std::string GetHtmlMathSpanFromLatexFormula(const std::string& input)
  { return  CGI:: GetHtmlMathFromLatexFormulA(input, "", "", false, false);
  }
  static std::string GetHtmlMathSpanNoButtonAddBeginArrayL
  (const std::string& input)
;
  static std::string GetHtmlMathSpanPure
  (const std::string& input, int upperNumChars=700)
;
  static std::string GetHtmlMathFromLatexFormulA
  (const std::string& input, const std::string& prependString, const std::string& appendStringBeforeButton, bool useDiv, bool useBeginArrayRCL)
;
  static std::string GetStyleButtonLikeHtml(){return " style=\"background:none; border:0; text-decoration:underline; color:blue; cursor:pointer\" ";}
  static std::string GetHtmlButton
  (const std::string& buttonID, const std::string& theScript, const std::string& buttonText)
;
  static std::string GetHtmlSpanHidableStartsHiddeN
  (const std::string& input)
;
  static bool FileExists(const std::string& theFileName);
  static bool OpenFileCreateIfNotPresent(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary);
  static std::string clearSlashes(const std::string& theString);
  static void clearDollarSigns(std::string& theString, std::string& output);
  static void subEqualitiesWithSimeq(std::string& theString, std::string& output);
  static void ChopCGIInputStringToMultipleStrings(const std::string& input, List<std::string>& outputData, List<std::string>& outputFieldNames);
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
};

template <class ObjectType1, class ObjectType2,
unsigned int hashFunction1(const ObjectType1&),
unsigned int hashFunction2(const ObjectType2&)
>
class Pair
{
public:
  ObjectType1 Object1;
  ObjectType2 Object2;
  Pair(){}
  Pair(const ObjectType1& o1, const ObjectType2& o2): Object1(o1), Object2(o2) {}
  static unsigned int HashFunction
  (const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& input)
  { return SomeRandomPrimes[0]*hashFunction1(input.Object1)+
    SomeRandomPrimes[1]*hashFunction2(input.Object2);
  }
  unsigned int HashFunction()const
  { return Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>::HashFunction(*this);
  }
  void operator=(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other)
  { this->Object1=other.Object1;
    this->Object2=other.Object2;
  }
  bool operator==(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other)const
  { return this->Object1==other.Object1 && this->Object2==other.Object2;
  }
};
typedef Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> PairInts;

static unsigned int NumHashResizes=0;

template <class Object, class TemplateList, unsigned int hashFunction(const Object&)=Object::HashFunction>
class HashTemplate: public TemplateList
{
private:
  void AddObjectOnBottom(const Object& o);
  void AddListOnTop(List<Object>& theList);
  void RemoveIndexShiftDown(int index);
  void RemoveFirstOccurenceSwapWithLast(Object& o);
  Object PopIndexShiftDown(int index);
  void SwapTwoIndices(int index1, int index2);
  void AssignLight(const ListLight<Object>& from);
  void SetSize(int theSize);
  void ReverseOrderElements();
  void ShiftUpExpandOnTop(int StartingIndex);

protected:
  List<List<int> > TheHashedArrays;
public:
  inline static std::string GetXMLClassName()
  { std::string result="HashedList_";
    result.append(Object::GetXMLClassName());
    return result;
  }
  inline unsigned int GetHash(const Object& input)const
  { unsigned int result=hashFunction(input);
    result%=this->TheHashedArrays.size;
    if (result<0)
      result+=this->TheHashedArrays.size;
    return result;
  }
  void Clear()
  { //if the hashed list is somewhat sparse, and the index is somewhat large,
    //(above 20 entries), we clear the hash by finding the occupied hashes and
    //nullifying them one by one.
    //else, we simply go through the entire hash index and nullify everything.
    //Note: for better performance, 20 entries should probably be changed to 100+,
    //however the smaller number is a good consistency test (it would make it easier to
    //detect a faulty hash).
    //If this program ever gets to do some hard-core number crunching, the 20 entries
    //should be increased.
    if (this->IsSparse() && this->TheHashedArrays.size>20)
      for (int i=0; i<this->size; i++)
      { int hashIndex=this->GetHash((*this)[i]);
        this->TheHashedArrays[hashIndex].size=0;
      }
    else
      for (int i=0; i<(signed) this->TheHashedArrays.size; i++)
        this->TheHashedArrays[i].size=0;
    this->size=0;
  }
  std::string GetReport()
  { std::stringstream out;
    out << "<br>List size: " << this->size;
    out << "<br>Hash size: " << this->TheHashedArrays.size;
    int maxHashSize=0;
    int numNonZeroHashes=0;
    for (int i =0; i<this->TheHashedArrays.size; i++)
    { maxHashSize=MathRoutines::Maximum(maxHashSize, this->TheHashedArrays[i].size);
      if (this->TheHashedArrays[i].size>0)
        numNonZeroHashes++;
    }
    out << "<br>Max hash array size: " << maxHashSize;
    out << "<br>Average hash array size: " <<((double) this->size)/((double) numNonZeroHashes);
    return out.str();
  }
  void AddOnTop(const Object& o)
  { unsigned int hashIndex =this->GetHash(o);
    this->TheHashedArrays[hashIndex].AddOnTop(this->size);
    this->TemplateList::AddOnTop(o);
    if (this->size>1)
      this->AdjustHashes();
  }
  void AddOnTop(const List<Object>& theList)
  { this->SetExpectedSize(this->size+theList.size);
    for (int i=0; i<theList.size; i++)
      this->AddOnTop(theList[i]);
  }
  void GrandMasterConsistencyCheck()const
  { for (int i=0; i<this->TheHashedArrays.size; i++)
    { List<int>& current=this->TheHashedArrays[i];
      for (int j=0; j<current.size; j++)
      { int theIndex=current[j];
        if (theIndex>=this->size)
        { std::cout << "This is a programming error: "
          << " hash lookup array of index " << i << ", entry of index " << j << " reports index "
          << theIndex << " but I have only " << this->size << " entries. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
        if (this->GetHash(this->TheObjects[theIndex])!=(unsigned) i)
        { std::cout << "<hr>This is a programming error: the hashed element in position " << theIndex
          << " is recorded in hash array of index " << i << ", however its hash value is instead "
          << this->GetHash(this->TheObjects[theIndex]) << ". ";
          std::cout << " The hash size is " << this->TheHashedArrays.size;
          std::cout << "<br>hashes of objects: ";
          for (int l=0; l<this->size; l++)
            std::cout << this->GetHash(this->TheObjects[l])
            << "= " << this->GetHash(this->TheObjects[l])%this->TheHashedArrays.size << ", ";
          std::cout << "<br>hashes recorded: ";
          for (int l=0; l<this->TheHashedArrays.size; l++)
            for (int k=0; k<this->TheHashedArrays[l].size; k++)
              std::cout << this->TheHashedArrays[l][k] << ", ";
          std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
      }
    }
  }
  int AddNoRepetitionOrReturnIndexFirst(const Object& o)
  { int result= this->GetIndex(o);
    if (result!=-1)
      return result;
    this->AddOnTop(o);
    return this->size-1;
  }
  inline void AdjustHashes()
  { this->SetExpectedSize(this->size);
  }
  void AddOnTopNoRepetition(const List<Object>& theList)
  { this->SetExpectedSize(this->size+theList.size);
    for (int i=0; i<theList.size; i++)
      this->AddOnTopNoRepetition(theList.TheObjects[i]);
  }
  bool AddOnTopNoRepetition(const Object& o)
  { if (this->GetIndex(o)!=-1)
      return false;
    this->AddOnTop(o);
    return true;
  }
  Object PopLastObject()
  { Object result=*this->LastObject();
    this->RemoveIndexSwapWithLast(this->size-1);
    return result;
  }
  Object PopIndexSwapWithLast(int index)
  { Object result=(*this)[index];
    this->RemoveIndexSwapWithLast(index);
    return result;
  }
  void RemoveLastObject()
  { this->RemoveIndexSwapWithLast(this->size-1);
  }
  void RemoveIndexSwapWithLast(int index)
  { if (index<0 || index>=this->size)
    { std::cout << "This is a programming error. You are attempting to pop out index " << index << " out of hashed array "
      << " of size " << this->size << ". "<< CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    Object* oPop= &this->TheObjects[index];
    int hashIndexPop = this->GetHash(*oPop);
    this->TheHashedArrays[hashIndexPop].RemoveFirstOccurenceSwapWithLast(index);
    if (index==this->size-1)
    { this->size--;
      return;
    }
    int tempI=this->size-1;
    Object* oTop= &this->TheObjects[tempI];
    int hashIndexTop=this->GetHash(*oTop);
    this->TheHashedArrays[hashIndexTop].RemoveFirstOccurenceSwapWithLast(tempI);
    this->TheHashedArrays[hashIndexTop].AddOnTop(index);
    this->List<Object>::RemoveIndexSwapWithLast(index);
  }
  void SwapTwoIndicesHash(int i1, int i2)
  { Object tempO;
    int i1Hash = this->GetHash(this->TheObjects[i1]);
    int i2Hash = this->GetHash(this->TheObjects[i2]);
    this->TheHashedArrays[i1Hash].RemoveFirstOccurenceSwapWithLast(i1);
    this->TheHashedArrays[i2Hash].RemoveFirstOccurenceSwapWithLast(i2);
    tempO= this->TheObjects[i1];
    this->TheObjects[i1]=this->TheObjects[i2];
    this->TheObjects[i2]=tempO;
    this->TheHashedArrays[i1Hash].AddOnTop(i2);
    this->TheHashedArrays[i2Hash].AddOnTop(i1);
  }
  inline bool Contains(const Object& o)const
  { return this->GetIndex(o)!=-1;
  }
  inline bool Contains(const List<Object>& theList)const
  { for (int i=0; i<theList.size; i++)
      if (this->GetIndex(theList[i])==-1)
        return false;
    return true;
  }
  int GetIndex(const Object& o) const
  { unsigned int hashIndex = this->GetHash(o);
    for (int i=0; i<this->TheHashedArrays[hashIndex].size; i++)
    { int j=this->TheHashedArrays[hashIndex].TheObjects[i];
      if (j>=this->size)
      { std::cout << "This is a programming error: corrupt hash table: at hashindex= " << hashIndex
        << " I get instructed to look up index " << j << " but I have only " << this->size << "elements. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      if((*this)[j]==o)
        return j;
    }
    return -1;
  }
  inline int GetIndexIMustContainTheObject(const Object& o) const
  { int result=this->GetIndex(o);
    if (result==-1)
    { std::cout << "This is a programming error: the programmer has requested the index of object "
      << o << " with a function that does not allow failure. "
      << " However, the container array does not contain his object. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return result;
  }
  inline bool IsSparseRelativeToExpectedSize(int expectedSize)const
  { return expectedSize*3<this->TheHashedArrays.size;
  }
  inline bool IsSparse()const
  { return this->IsSparseRelativeToExpectedSize(this->size);
  }
  void SetExpectedSize(int expectedSize)
  { if (expectedSize<1)
      return;
    if (expectedSize==1 || expectedSize==2)
    { this->SetHashSizE(1);
      return;
    }
    this->TemplateList::SetExpectedSize(expectedSize);
    if (!this->IsSparseRelativeToExpectedSize(expectedSize))
      this->SetHashSizE(expectedSize*5);
  }
  void SetHashSizE(unsigned int HS)
  { if (HS==(unsigned) this->TheHashedArrays.size)
      return;
    MacroIncrementCounter(NumHashResizes);
    List<int> emptyList; //<-empty list has size 0
    this->TheHashedArrays.initFillInObject(HS, emptyList);
    if (this->size>0)
      for (int i=0; i<this->size; i++)
      { int theIndex=this->GetHash((*this)[i]);
        this->TheHashedArrays[theIndex].AddOnTop(i);
      }
  }
  void QuickSortAscending(typename List<Object>::OrderLeftGreaterThanRight theOrder=0)
  { List<Object> theList;
    theList=*this;
    theList.QuickSortAscending(theOrder);
    this->operator=(theList);
  }
  void QuickSortDescending(typename List<Object>::OrderLeftGreaterThanRight theOrder=0)
  { List<Object> theList;
    theList=*this;
    theList.QuickSortDescending(theOrder);
    this->operator=(theList);
  }
  void initHashesToOne()
  { this->TheHashedArrays.SetSize(1);
    this->TheHashedArrays[0].size=0;
  }
  HashTemplate(const HashTemplate& other)
  { this->initHashesToOne();
    this->operator=(other);
  }
  HashTemplate()
  { this->initHashesToOne();
  }
  std::string ToString(FormatExpressions* theFormat)const
  { return this->List<Object>::ToString(theFormat);
  }
  std::string ToString()const
  { return this->List<Object>::ToString();
  }
  void operator=(const HashedList<Object, hashFunction>& From)
  { if (&From==this)
      return;
    this->Clear();
    this->SetHashSizE(From.TheHashedArrays.size);
    this->::List<Object>::operator=(From);
    if (From.IsSparse())
    { for (int i=0; i<this->size; i++)
      { unsigned int hashIndex=this->GetHash(this->TheObjects[i]);
        this->TheHashedArrays[hashIndex].ReservE(From.TheHashedArrays[hashIndex].size);
        this->TheHashedArrays[hashIndex].AddOnTop(i);
      }
    } else
      this->TheHashedArrays=From.TheHashedArrays;
  }

  void operator=(const TemplateList& other)
  { if (this==&other)
      return;
    this->Clear();
    this->SetExpectedSize(other.size);
    for (int i=0; i<other.size; i++)
      this->AddOnTop(other.TheObjects[i]);
//    int commentmewhendone;
//    this->GrandMasterConsistencyCheck();
  }
};

template <class Object, unsigned int hashFunction(const Object&)=Object::HashFunction>
class HashedList: public HashTemplate<Object, List<Object>, hashFunction>
{
public:
  HashedList(const HashedList& other)
  { this->operator=(other);
  }
  HashedList(){}
  inline void operator=(const HashedList& other)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::operator=(other);
  }
  inline void operator=(const List<Object>& other)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::operator=(other);
  }
  //Note The following function specializations are declared entirely in order to
  //facilitate autocomplete in my current IDE. If I find a better autocompletion
  //IDE the following should be removed.
  inline void AddOnTopNoRepetition(const List<Object>& theList)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTopNoRepetition(theList);
  }
  inline bool AddOnTopNoRepetition(const Object& o)
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTopNoRepetition(o);
  }
  inline void AddOnTop(const Object& o)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTop(o);
  }
  inline void AddOnTop(const List<Object>& theList)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTop(theList);
  }
  inline bool Contains(const Object& o)const
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::Contains(o);
  }
  inline bool Contains(const List<Object>& theList)const
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::Contains(theList);
  }
  int GetIndex(const Object& o) const
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::GetIndex(o);
  }
  inline int GetIndexIMustContainTheObject(const Object& o) const
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::GetIndexIMustContainTheObject(o);
  }
  inline int AddNoRepetitionOrReturnIndexFirst(const Object& o)
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::AddNoRepetitionOrReturnIndexFirst(o);
  }
};

//class used to avoid a gcc compiler bug.
//This class should probably be removed as soon as the bug is resolved.
template <class Object>
class HashedListSpecialized: public HashedList<Object, Object::HashFunction>
{
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
class Matrix
{ // friend std::iostream& operator<< <Element>(std::iostream& output, const Matrix<Element>& theMat);
  friend std::ostream& operator<< (std::ostream& output, const Matrix<Element>& theMat){
    output << theMat.ToString();
    return output;
  }
//  friend std::iostream& operator>> <Element>(std::iostream& input, Matrix<Element>& theMat);
public:
  int NumRows; int ActualNumRows;
  int NumCols; int ActualNumCols;
  Element** elements;
  void init(int r, int c);
  void ReleaseMemory();
  void Resize(int r, int c, bool PreserveValues) {this->Resize(r, c, PreserveValues, 0);}
  bool IsPositiveDefinite();
  bool IsNonNegativeAllEntries()
  { for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        if (this->elements[i][j]<0)
          return false;
    return true;
  }
  void Resize
  (int r, int c, bool PreserveValues, const Element* TheRingZero)
  ;
  inline static std::string GetXMLClassName()
  { std::string result="Matrix_";
    result.append(Element::GetXMLClassName());
    return result;
  }
  Matrix():NumRows(0), ActualNumRows(0), NumCols(0), ActualNumCols(0), elements(0){}
  Matrix(const Matrix<Element>& other):NumRows(0), ActualNumRows(0), NumCols(0), ActualNumCols(0), elements(0)
  { *this=other;
  }
  ~Matrix()
  { this->ReleaseMemory();
  }
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
    *this=tempMat;
  }
  void AppendMatrixOnTheRight(const Matrix<Element>& standsOnTheRight)
  { if (standsOnTheRight.NumRows<this->NumRows)
    { Element theZero;
      theZero=0;
      Matrix<Element> standsOnTheRightNew=standsOnTheRight;
      standsOnTheRightNew.Resize(this->NumRows, standsOnTheRight.NumCols, true, &theZero);
      this->AppendMatrixOnTheRight(standsOnTheRightNew);
      return;
    }
    if (this->NumRows<standsOnTheRight.NumRows)
    { Element theZero;
      theZero=0;
      this->Resize(standsOnTheRight.NumRows, this->NumCols, true, &theZero);
    }
    int oldNumCols=this->NumCols;
    this->Resize(this->NumRows, standsOnTheRight.NumCols+oldNumCols, true);
    for (int i=0; i<this->NumRows; i++)
      for (int j=oldNumCols; j<this->NumCols; j++)
        this->elements[i][j]=standsOnTheRight.elements[i][j-oldNumCols];
  }
  void ComputeDeterminantOverwriteMatrix(Element& output, const Element& theRingOne=1, const Element& theRingZero=0);
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
  template <class otherType>
  void ActOnVectorColumn(const Vector<otherType>& input, Vector<otherType>& output, const otherType& TheRingZero=0)const
  { if (&input==&output)
    { Vector<otherType> inputNew=input;
      this->ActOnVectorColumn(inputNew, output, TheRingZero);
      return;
    }
    if (this->NumCols!=input.size)
    { std::cout << "This is a programming error: attempting to multply a matrix with " << this->NumCols << " columns with a vector(column) of "
      << " dimension " << input.size << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    output.MakeZero(this->NumRows, TheRingZero);
    otherType tempElt;
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
      { tempElt=this->elements[i][j];
        tempElt*=input[j];
        output[i]+=tempElt;
      }
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const Matrix<Element>& input)
  { unsigned int result=0;
    int counter=0;
    for (int i=0; i<input.NumRows; i++, counter++)
      for (int j=0; j<input.NumCols; j++, counter++)
        if (counter<SomeRandomPrimesSize)
          result+=input.elements[i][j].HashFunction()*SomeRandomPrimes[counter];
        else
          result+=input.elements[i][j].HashFunction()*(i+1)+j;
    return result;
  }
  template <class otherType>
  void ActOnVectorsColumn(const Vectors<otherType>& input, Vectors<otherType>& output, const otherType& TheRingZero=0)const
  { assert(&input!=&output);
    if (input.size==0)
      return;
    if (this->NumCols!=input.GetDim())
    { std::cout << "This is a programming error: attempting to act by "
      << this->ToString() << "(an " << this->NumRows << " x "
      << this->NumCols << " matrix) on a column vector " << input.ToString()
      << "(dimension " << input.size << ").";
      assert(false);
    }
    output.SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->ActOnVectorColumn(input[i], output[i], TheRingZero);
  }
  template <class otherType>
  void ActOnVectorColumn(Vector<otherType>& inputOutput, const otherType& TheRingZero=(otherType) 0)const
  { Vector<otherType> buffer;
    this->ActOnVectorColumn(inputOutput, buffer, TheRingZero);
    inputOutput=buffer;
  }
  template <class otherType>
  void ActOnVectorsColumn(Vectors<otherType>& inputOutput, const otherType& TheRingZero=(otherType) 0)const
  { for (int i=0; i<inputOutput.size; i++)
      this->ActOnVectorColumn(inputOutput[i], TheRingZero);
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  std::string ElementToStringWithBlocks(List<int>& theBlocks);
  void MakeIdMatrix(int theDimension, const Element& theRingUnit=1, const Element& theRingZero=0)
  { this->init(theDimension, theDimension);
    for (int i=0; i<theDimension; i++)
      for (int j=0; j<theDimension; j++)
        if (j!=i)
          this->elements[i][j]=theRingZero;
        else
          this->elements[i][j]=theRingUnit;
  }
  void MakeZeroMatrix(int theDimension, const Element& theRingZero=0)
  { this->init(theDimension, theDimension);
    for (int i=0; i<theDimension; i++)
      for (int j=0; j<theDimension; j++)
        this->elements[i][j] = theRingZero;
  }
  void ActOnMonomialAsDifferentialOperator(MonomialP& input, Polynomial<Rational>& output)
;
  void SwitchTwoRows(int row1, int row2);
  inline void SwitchTwoRowsWithCarbonCopy(int row1, int row2, Matrix<Element>* theCarbonCopy)
  { this->SwitchTwoRows(row1, row2);
    if (theCarbonCopy!=0)
      theCarbonCopy->SwitchTwoRows(row1, row2);
  }
  void SetNumVariables(int GoalNumVars)
  { for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j].SetNumVariables(GoalNumVars);
  }
  void Substitution(const PolynomialSubstitution<Rational>& theSub)
  ;
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
  inline Element& operator()(int i, int j)const
  { if (i<0  || i>=this->NumRows || j<0 || j>this->NumCols)
    { std::cout << "This is a programming error: requesting row, column indexed"
      << " by " << i+1 << " and " << j+1 << " but I am a matrix with "
      << this->NumRows << " rows and " << this->NumCols << " colums. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return this->elements[i][j];
  }
  bool IsIdMatrix()const
  { if (this->NumRows!=this->NumCols || this->NumRows<=0)
       return false;
     for (int i=0; i<this->NumRows; i++)
       for (int j=0; j<this->NumCols; j++)
         if (i==j)
         { if (!this->elements[i][j].IsEqualToOne())
             return false;
         } else
           if (!this->elements[i][j].IsEqualToZero())
             return false;
     return true;
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
  void MakeLinearOperatorFromDomainAndRange(Vectors<Element>& domain, Vectors<Element>& range)
  { Matrix<Element> A;
    Matrix<Element> B;
    A.AssignRootsToRowsOfMatrix(domain);
    B.AssignRootsToRowsOfMatrix(range);
    A.Invert();
    (*this)=A*B;
    this->Transpose();
  }
  void RowTimesScalar(int rowIndex, const Element& scalar);
  inline void RowTimesScalarWithCarbonCopy(int rowIndex, const Element& scalar, Matrix<Element>* theCarbonCopy)
  { this->RowTimesScalar(rowIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->RowTimesScalar(rowIndex, scalar);
  }
  void AddTwoRows
  (int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar,
   GlobalVariables* theGlobalVariables=0
   );
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
  void MultiplyOnTheLeft(const Matrix<Element>& standsOnTheLeft, Matrix<Element>& output, const Element& theRingZero=0);
  void MultiplyOnTheLeft(const Matrix<Element>& standsOnTheLeft, const Element& theRingZero=0);
  void MultiplyOnTheRight(const Matrix<Element>& standsOnTheRight, const Element& theRingZero=0)
  { Matrix<Element> temp=*this;
    *this=standsOnTheRight;
    this->MultiplyOnTheLeft(temp);
  }
  void NonPivotPointsToEigenVectorMatrixForm
  (Selection& TheNonPivotPoints, Matrix<Element>& output, const Element& theRingUnit, const Element& theRingZero)
;
  void GetListRowsToVectors(List<Vector<Element> >& output)
  { output.SetSize(this->NumRows);
    for (int i=0; i<this->NumRows; i++)
    { output[i].SetSize(this->NumCols);
      for (int j=0; j<this->NumCols; j++)
        output[i][j]=this->elements[i][j];
    }
  }
void NonPivotPointsToEigenVector
(Selection& TheNonPivotPoints, Vector<Element>& output, const Element& theRingUnit=1, const Element& theRingZero=0)
;
  //returns true if successful, false otherwise
//  bool ExpressColumnAsALinearCombinationOfColumnsModifyMyself
//    (Matrix<Element>& inputColumn, Matrix<Element>* outputTheGaussianTransformations Matrix<Element>& outputColumn);
  bool Invert(GlobalVariables& theGlobalVariables){return this->Invert();}
  bool Invert(const Element& theRingUnit=1, const Element& theRingZero=0);
  void NullifyAll(const Element& theRingZero=0);
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
  void DirectSumWith(const Matrix<Element>& m2, const Element& theRingZero=0);
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
    tempMat-=(input);
    return tempMat.IsEqualToZero();
  }
  //returns true if the system has a solution, false otherwise
  bool RowEchelonFormToLinearSystemSolution(Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide, Matrix<Element>& outputSolution);
  void operator+=(const Matrix<Element>& right)
  { if (this->NumRows!=right.NumRows || this->NumCols!=right.NumCols)
    { std::cout << "This is a programming error: attempting to add matrix with " << this->NumRows << " rows and "
      << this->NumCols << " columns " << " to a matrix with " << right.NumRows << " rows and " << right.NumCols
      << " columns. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j]+=(right.elements[i][j]);
  }
  LargeIntUnsigned FindPositiveLCMCoefficientDenominators();

  void operator-=(const Matrix<Element>& right)
  { if (this->NumRows!=right.NumRows || this->NumCols!=right.NumCols)
    { std::cout << "This is a programming error: attempting to subtract from matrix with "
      << this->NumRows << " rows and "
      << this->NumCols << " columns " << " a matrix with " << right.NumRows
      << " rows and " << right.NumCols
      << " columns. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j].Subtract(right.elements[i][j]);
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
  void LieBracketWith(Matrix<Element> & right)
  { Matrix<Element> tempMat;
    this->LieBracket(*this, right, tempMat);
    *this=tempMat;
  }
  static void LieBracket(const Matrix<Element>& left, const Matrix<Element>& right, Matrix<Element>& output)
  { assert(left.NumCols==left.NumRows && right.NumCols==right.NumRows && left.NumCols==right.NumCols);
    Matrix<Element> tempPlus, tempMinus;
    tempPlus=(right);
    tempPlus.MultiplyOnTheLeft(left);
    tempMinus=(left);
    tempMinus.MultiplyOnTheLeft(right);
    tempPlus-=(tempMinus);
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
  inline static void GaussianEliminationByRows
  (Matrix<Element>& theMatrix, Matrix<Element>& otherMatrix, Selection& outputNonPivotPoints,
   GlobalVariables* theGlobalVariables=0)
  { Matrix<Element>::GaussianEliminationByRows
    (theMatrix, otherMatrix, outputNonPivotPoints, true, theGlobalVariables);
  }
  static void GaussianEliminationByRows
  (Matrix<Element>& mat, Matrix<Element>& output, Selection& outputSelection,
   bool returnNonPivotPoints, GlobalVariables* theGlobalVariables=0)
  ;
  void GaussianEliminationByRows
  (Matrix<Element>& otherMatrix, Selection& outputNonPivotColumns,
   GlobalVariables* theGlobalVariables=0)
  { Matrix<Element>::GaussianEliminationByRows
    (*this, otherMatrix, outputNonPivotColumns, theGlobalVariables);
  }
  void GaussianEliminationByRowsNoRowSwapPivotPointsByRows
  (int firstNonProcessedRow, Matrix<Element>& output, List<int>& outputPivotPointCols, Selection* outputNonPivotPoints__WarningSelectionNotInitialized)
;
  void GaussianEliminationEuclideanDomain
  (Matrix<Element>* otherMatrix=0, const Element& theRingMinusUnit=-1,
   const Element& theRingUnit=1
  , bool (*comparisonGEQFunction) (const Element& left, const Element& right)=0,
   GlobalVariables* theGlobalVariables=0
   );
  static bool Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists
  (Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output);
  Element GetDeterminant();
  Element GetTrace();
  void Transpose(GlobalVariables& theGlobalVariables){this->Transpose();}
  void AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den);
  void AssignRootsToRowsOfMatrix(const Vectors<Rational>& input)
  ;
  void ScaleToIntegralForMinRationalHeightNoSignChange();
  void GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen);
  void LieBracketWith(const Matrix<Element>& right);
  void ApproximateLargestEigenSpace
  (Vectors<Rational>& outputBasis, const Rational& DesiredError, int SuggestedOrder, int numIterations)
  ;
  void MatrixToRoot(Vector<Rational> & output);
  inline void FindZeroEigenSpace(Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
  { this->FindZeroEigenSpaceOneOneForEachNonPivot(output);
  }
  inline void FindZeroEigenSpace(Vectors<Rational>& output)
  { this->FindZeroEigenSpaceOneOneForEachNonPivot(output);
  }
  void FindZeroEigenSpaceOneOneForEachNonPivot(Vectors<Element>& output);
  static bool SystemLinearInequalitiesHasSolution
  (Matrix<Element>& matA, Matrix<Element>& matb, Matrix<Element>& outputPoint);
  static bool SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
  (Matrix<Element>& matA, Matrix<Element>& matb, Matrix<Element>& outputSolution, GlobalVariables& theGlobalVariables)
  ;
  static void ComputePotentialChangeGradient(Matrix<Element>& matA, Selection& BaseVariables, int NumTrueVariables, int ColumnIndex, Rational& outputChangeGradient, bool& hasAPotentialLeavingVariable);
  static void GetMaxMovementAndLeavingVariableRow
  (Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables,
   Matrix<Element>& tempMatA, Matrix<Element>& matX, Selection& BaseVariables)
   ;
  int FindPositiveLCMCoefficientDenominatorsTruncated();
  int FindPositiveGCDCoefficientNumeratorsTruncated();
  Matrix<Element> operator-(const Matrix<Element>& right)const
  { Matrix<Element> tempMat;
    tempMat.Assign(*this);
    tempMat.Subtract(right);
    return tempMat;
  }
  Matrix<Element> operator*(const Matrix<Element>& right)const
  ;
  Vector<Element> operator*(const Vector<Element>& v) const;
  inline void operator=(const Matrix<Element>& other);
};

template <typename Element>
inline void Matrix<Element>::Resize(int r, int c, bool PreserveValues, const Element* const TheRingZero)
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
inline void Matrix<Element>::operator=(const Matrix<Element>& m)
{ if (this==&m)
    return;
  this->Resize(m.NumRows, m.NumCols, false);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j]=m.elements[i][j];
}

template <typename Element>
inline void Matrix<Element>::ReleaseMemory()
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

template <class Element>
bool Matrix<Element>::flagAnErrorHasOccurredTimeToPanic=false;

class Selection
{
public:
  inline static std::string GetXMLClassName(){ return "Selection";}
  List<int> elements;
  List<bool> selected;
  int MaxSize;
  int CardinalitySelection;
  void AddSelectionAppendNewIndex(int index);
  void RemoveLastSelection();
  void RemoveSelection(int index)
  { this->selected[index]=false;
    this->ComputeIndicesFromSelection();
  }
  void MakeFullSelection(int inputMaxSize)
  { this->init(inputMaxSize);
    this->MakeFullSelection();
  }
  void MakeFullSelection()
  { for (int i=0; i<this->MaxSize; i++)
    { this->elements[i]=i;
      this->selected[i]=true;
    }
    this->CardinalitySelection=this->MaxSize;
  }
  bool IsSubset(const Selection& other)const
  { if (this->MaxSize!=other.MaxSize)
      return false;
    for (int i=0; i<this->CardinalitySelection; i++)
      if (!other.selected[this->elements[i]])
        return false;
    return true;
  }
  void init(int maxNumElements);
  void ComputeIndicesFromSelection();
  void initNoMemoryAllocation();
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const Selection& input)
  { return input.HashFunction();
  }
  std::string ToString()const;
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
  void InvertSelection()
  { for (int i=0; i<this->MaxSize; i++)
      this->selected[i]=!this->selected[i];
    this->ComputeIndicesFromSelection();
  }
  inline void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){ this->ReadFromFile(input);}
  inline void operator=(const Selection& right){this->Assign(right); }
  void operator=(const Vector<Rational> & other);
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
  Selection(const Vector<Rational>& other) { this->operator=(other);}
  Selection(const Selection& other):MaxSize(0), CardinalitySelection(0)
  { this->Assign(other);
  }
};

class SelectionWithMultiplicities
{
public:
  std::string DebugString;
  void ComputeDebugString(){this->ToString(this->DebugString); }
  void ToString(std::string& output);
  std::string ToString(){std::string tempS; this->ToString(tempS); return tempS;}
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
  { this->Multiplicities=(right.Multiplicities);
    this->MaxMultiplicities=(right.MaxMultiplicities);
    this->elements=(right.elements);
  }
};

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
bool Matrix<Element>::Invert(const Element& theRingUnit, const Element& theRingZero)
{ assert(this->NumCols==this->NumRows);
//  if (this->flagComputingDebugInfo)
//    this->ComputeDebugString();
  Matrix tempMatrix;
  Selection NonPivotPts;
  tempMatrix.init(this->NumRows, this->NumCols);
  tempMatrix.NullifyAll(theRingZero);
  for (int i=0; i<this->NumCols; i++)
    tempMatrix.elements[i][i]=theRingUnit;
  this->GaussianEliminationByRows(*this, tempMatrix, NonPivotPts);
  if(NonPivotPts.CardinalitySelection!=0)
    return false;
  else
  { *this=tempMatrix;
//    if (this->flagComputingDebugInfo)
//      this->ComputeDebugString();
    return true;
  }
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(const Matrix<Element>& standsOnTheLeft, const Element& theRingZero)
{ Matrix<Element> tempMat;
  this->MultiplyOnTheLeft(standsOnTheLeft, tempMat, theRingZero);
  this->operator=(tempMat);
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(const Matrix<Element>& standsOnTheLeft, Matrix<Element>& output, const Element& theRingZero)
{ assert(&output!=this && &output!=&standsOnTheLeft);
  assert(this->NumRows==standsOnTheLeft.NumCols);
  Element tempEl;
  output.init(standsOnTheLeft.NumRows, this->NumCols);
  for (int i=0; i< standsOnTheLeft.NumRows; i++)
    for(int j=0; j< this->NumCols; j++)
    { output.elements[i][j]=theRingZero;
      for (int k=0; k<this->NumRows; k++)
      { tempEl.Assign(standsOnTheLeft.elements[i][k]);
        tempEl*=(this->elements[k][j]);
        output.elements[i][j]+=(tempEl);
      }
    }
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
    { tempS=this->elements[i][j].ToString();
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

template<typename Element>
void Matrix<Element>::FindZeroEigenSpacE(List<List<Element> >& output, const Element& theRingUnit, const Element& theRingMinusUnit, const Element& theRingZero, GlobalVariables& theGlobalVariables)
{ Matrix<Element> tempMat;
  Matrix<Element> emptyMat;
  tempMat=(*this);
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
inline int Matrix<Element>::FindPivot(int columnIndex, int RowStartIndex, int RowEndIndex)
{ for(int i = RowStartIndex; i<= RowEndIndex; i++)
    if (!this->elements[i][columnIndex].IsEqualToZero())
      return i;
  return -1;
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
inline void Matrix<Element>::NullifyAll(const Element& theRingZero)
{ for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j]=theRingZero;
}

template <typename Element>
inline void Matrix<Element>::init(int r, int c)
{ this->Resize(r, c, false);
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
  //If you got no clue what to put just leave CarryOverBound as it is below.
  static const unsigned int CarryOverBound=1000000000UL;
  //the above choice of CarryOverBound facilitates very quick conversions of Large integers into decimal, with
  //relatively small loss of speed and RAM memory.
//  static const unsigned int CarryOverBound=2147483648UL; //=2^31
  //The following must be less than or equal to the square root of CarryOverBound.
  //It is used for quick multiplication of Rational-s.
  static const int SquareRootOfCarryOverBound=31000; //31000*31000=961000000<1000000000
//  static const int SquareRootOfCarryOverBound=32768; //=2^15
  void SubtractSmallerPositive(const LargeIntUnsigned& x);
  void ToString(std::string& output)const;
  void ElementToStringLargeElementDecimal(std::string& output)const;
  inline std::string ToString()const {std::string tempS; this->ToString(tempS); return tempS;}
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
    output.ReservE(n/2);
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
  static LargeIntUnsigned lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b)
  { LargeIntUnsigned output;
    LargeIntUnsigned::lcm(a, b, output);
    return output;
  }
  static void lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output)
  { LargeIntUnsigned tempUI, tempUI2;
    LargeIntUnsigned::gcd(a, b, tempUI);
    a.MultiplyBy(b, tempUI2);
    output.Assign(tempUI2);
    output.DivPositive(tempUI, output, tempUI2);
    assert(!output.IsEqualToZero());
  }
  unsigned int HashFunction()const
  { int numCycles=MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
    unsigned int result=0;
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
  inline void AccountPrimeFactor
  (unsigned int theP, List<unsigned int>& outputPrimeFactors, List<int>& outputMults)
  { if (outputPrimeFactors.size==0)
    { outputPrimeFactors.AddOnTop(theP);
      outputMults.AddOnTop(1);
      return;
    }
    if (*outputPrimeFactors.LastObject()==theP)
      (*outputMults.LastObject())++;
    else
    { outputPrimeFactors.AddOnTop(theP);
      outputMults.AddOnTop(1);
    }
  }
  bool Factor(List<unsigned int>& outputPrimeFactors, List<int>& outputMultiplicites);
  inline void Assign(const LargeIntUnsigned& x)
  { this->::List<unsigned int>::operator =(x);
  }
  void AssignString(const std::string& input);
  int GetUnsignedIntValueTruncated();
  int operator%(unsigned int x);
  inline void operator=(const LargeIntUnsigned& x){ this->Assign(x); }
  inline void operator=(unsigned int x){ this->AssignShiftedUInt(x,0); }
  inline void operator+=(const LargeIntUnsigned& other){this->Add(other);}
  inline void operator--(int)
  { if (this->IsEqualToZero())
    { std::cout << "This is a programming error: attempting to subtract 1 from a large unsigned "
      << " integer with value 0. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->SubtractSmallerPositive(1);
  }
  inline void operator%=(const LargeIntUnsigned& other)
  { if (&other==this)
    { this->MakeZero();
      return;
    }
    LargeIntUnsigned copyMe=*this;
    LargeIntUnsigned temp1;
    copyMe.DivPositive(other, temp1, *this);
  }
  LargeIntUnsigned operator/(unsigned int x)const;
  LargeIntUnsigned operator/(const LargeIntUnsigned& x)const;
  LargeIntUnsigned(unsigned int x)
  { this->Assign(x);
  }
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
  void ToString(std::string& output)const;
  inline std::string ToString(FormatExpressions* theFormat=0)const{std::string tempS; this->ToString(tempS); return tempS;}
  bool IsPositive()const{return this->sign==1 && (this->value.IsPositive()); }
  bool IsNegative()const{return this->sign==-1&& (this->value.IsPositive()); }
  inline bool BeginsWithMinus(){return this->IsNegative();}
  inline bool IsPositiveOrZero()const{return !this->IsNegative(); }
  inline bool IsNonPositive()const{return !this->IsPositive(); }
  bool NeedsBrackets()const{return false;}
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
  void checkConsistency(){}
  void MakeZero();
  bool GetDivisors(List<int>& output, bool includeNegative)
  { if (this->value.size>1)
      return false;
    int val= this->value[0];
    if (val>50000)
      return false;
    output.SetSize(0);
    for (int i=1; i<= val; i++)
      if (val% i==0)
      { output.AddOnTop(i);
        if (includeNegative)
          output.AddOnTop(-i);
      }
    return true;
  }
  void MakeOne(){this->value.MakeOne(); this->sign=1; }
  void MakeMOne(){this->value.MakeOne(); this->sign=-1;}
  static unsigned int HashFunction (const LargeInt& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->value.HashFunction()+this->sign+3;
  }
  int GetIntValueTruncated(){return this->sign* this->value.GetUnsignedIntValueTruncated(); }
  double GetDoubleValue();
  int operator %(int x);
  inline void AssignFloor(){}
  inline void operator=(const LargeInt& x)
  { this->Assign(x);
  }
  inline void operator=(int x)
  { this->Assign(x);
  }
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
  { if (other.IsPositiveOrZero())
    { if (this->IsPositiveOrZero())
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
  friend std::ostream& operator << (std::ostream& output, const Rational& theRat)
  { output << theRat.ToString();
    return output;
  }
  friend std::istream& operator >> (std::istream& input, const Rational& output)
  { std::string tempS;
    assert(false);
//    input >> tempS;
    //output.
    return input;
  }
  inline bool TryToAddQuickly(int OtherNum, int OtherDen)
  { register int OtherNumAbs, thisNumAbs;
    if (this->DenShort<=0 || OtherDen<=0)
    { std::cout << "This is a programming error: corrupt rational number(s) with denominator "
      << this->DenShort << " and " << OtherDen << ". The cause of the error should be in some of "
      << " the calling functions. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (OtherNum<0)
      OtherNumAbs=-OtherNum;
    else
      OtherNumAbs=OtherNum;
    if (this->NumShort<0)
      thisNumAbs=-this->NumShort;
    else
      thisNumAbs=this->NumShort;
    if (////!this->flagMinorRoutinesOnDontUseFullPrecision &&
        (this->Extended!=0 || thisNumAbs>= LargeIntUnsigned::SquareRootOfCarryOverBound  || this->DenShort>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherNumAbs>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherDen>=LargeIntUnsigned::SquareRootOfCarryOverBound)  )
      return false;
    register int N= this->NumShort*OtherDen+this->DenShort*OtherNum;
    register int D= this->DenShort*OtherDen;
    if (N==0)
    { this->NumShort=0;
      this->DenShort=1;
      MacroIncrementCounter(Rational::TotalSmallAdditions);
      return true;
    }
    register int tempGCD;
    if (N>0)
      tempGCD= Rational::gcd(N, D);
    else
      tempGCD= Rational::gcd(-N, D);
    this->NumShort=(N/tempGCD);
    this->DenShort= (D/tempGCD);
    MacroIncrementCounter(Rational::TotalSmallAdditions);
    return true;
  }
  inline bool TryToMultiplyQuickly(int OtherNum, int OtherDen)
  { register int OtherNumAbs, thisNumAbs;
    if (this->DenShort<=0 || OtherDen<=0)
    { if (DenShort==0 || OtherDen==0)
        std::cout << "This is a programming error: division by zero. ";
      else
        std::cout << "This is a programming error: corrupt rational number denominator. ";
      std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (OtherNum<0)
      OtherNumAbs=-OtherNum;
    else
      OtherNumAbs=OtherNum;
    if (this->NumShort<0)
      thisNumAbs=-this->NumShort;
    else
      thisNumAbs=this->NumShort;
    if (////!this->flagMinorRoutinesOnDontUseFullPrecision &&
        (this->Extended!=0 || thisNumAbs>= LargeIntUnsigned::SquareRootOfCarryOverBound || this->DenShort>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherNumAbs>=LargeIntUnsigned::SquareRootOfCarryOverBound || OtherDen   >=LargeIntUnsigned::SquareRootOfCarryOverBound)  )
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
    MacroIncrementCounter(Rational::TotalSmallMultiplications);
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
public:
  inline static std::string GetXMLClassName(){ return "Rational";}
  int NumShort;
  //the requirement that the below be unsigned caused a huge problem, so I
  //changed it back to int. Grrrrr.
  int DenShort;
  LargeRationalExtended *Extended;
  static unsigned TotalSmallAdditions;
  static unsigned TotalLargeAdditions;
  static unsigned TotalSmallMultiplications;
  static unsigned TotalLargeMultiplications;
  static unsigned TotalSmallGCDcalls;
  static unsigned TotalLargeGCDcalls;
  void GetDenominator(Rational& output)
  { LargeIntUnsigned tempInt;
    this->GetDenominator(tempInt);
    output.AssignLargeIntUnsigned(tempInt);
  }
  bool NeedsBrackets()const
  { return false;
    //return this->IsNegative();
  }

  Rational GetLCMNumeratorRationalDenominators()
  { Rational result;
    this->GetDenominator(result);
    return result;
  }
  Rational GetGCDNumeratorsRationalCoeffs()
  { return this->GetNumerator();
  }
  inline LargeIntUnsigned GetDenominator()const
  { LargeIntUnsigned tempI;
    this->GetDenominator(tempI);
    return tempI;
  }
  inline void GetDenominator(LargeIntUnsigned& output)const
  { if (this->Extended==0)
    { unsigned int tempI= (unsigned int) this->DenShort;
      output.AssignShiftedUInt(tempI, 0);
    }
    else
      output.Assign(this->Extended->den);
  }
  bool BeginsWithMinus(){ return this->IsNegative();}

  inline LargeInt GetNumerator()const
  { LargeInt result;
    this->GetNumerator(result);
    return result;
  }
  inline void GetNumerator(LargeInt& output)const
  { this->GetNumerator(output.value);
    output.sign=1;
    if (this->IsNegative())
      output.sign=-1;
  }
  inline void SetDynamicSubtype(int dummyParameter){}
  inline void GetNumerator(LargeIntUnsigned& output)const
  { if (this->Extended==0)
    { if (this->NumShort<0)
        output.AssignShiftedUInt((unsigned int)(-this->NumShort), 0);
      else
        output.AssignShiftedUInt((unsigned int) this->NumShort, 0);
    }
    else
      output.Assign(this->Extended->num.value);
  }
  Rational RationalValue()
  { return *this;
  }
//  inline int GetNumValueTruncated(){return this->NumShort; };
//  inline int GetDenValueTruncated(){return this->denShort; };
//  static bool flagMinorRoutinesOnDontUseFullPrecision;
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
  Rational GetZero()
  { return 0;
  }
  Rational GetOne()
  { return 1;
  }
  void AssignFracValue();
  void AssignFloor()
  { Rational tempRat=*this;
    tempRat.AssignFracValue();
    *this-=tempRat;
  }
  void MultiplyBy(const Rational& r);
  //IMPORTANT NOTE ON HASHFUNCTIONS
  //The Hash function of zero MUST be equal to zero.
  //See Note on Hashes before the definition of SomeRandomPrimes;
  unsigned int HashFunction() const
  { if (this->Extended==0)
    { if (this->NumShort==0)
        return 0;
      return this->NumShort*SomeRandomPrimes[0]+this->DenShort*::SomeRandomPrimes[1];
    }
    return this->Extended->num.HashFunction()*SomeRandomPrimes[0]+this->Extended->den.HashFunction()*SomeRandomPrimes[1];
  }
  static inline unsigned int HashFunction(const Rational& input)
  { return input.HashFunction();
  }
  inline void ComputeDebugString(){}
  //void MultiplyByLargeRational(int num, int den);
  void MultiplyByInt(int x);
  void MultiplyByLargeInt(LargeInt& x);
  void MultiplyByLargeIntUnsigned(LargeIntUnsigned& x);
  void Assign(const Rational& r);
  void AssignInteger(int x);
  bool IsInteger(LargeInt* whichInteger=0)const;
  bool IsSmallInteger(int* whichInteger=0)const
  { if (this->Extended!=0)
      return false;
    if (this->DenShort!=1)
      return false;
    if(whichInteger!=0)
      *whichInteger=this->NumShort;
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
  std::string ToString(FormatExpressions* notUsed=0)const;
  bool IsEqualTo(const Rational& r) const;
  bool IsGreaterThanOrEqualTo(const Rational& right)const;
  bool IsEven()const
  { Rational tempRat=*this;
    tempRat/=2;
    return tempRat.IsInteger();
  }
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
  inline bool operator<=(const Rational& other)const
  { return !(other<*this);
  }
  inline bool IsPositiveOrZero()const
  { if (this->Extended==0)
      return this->NumShort>=0;
    else
      return this->Extended->num.IsPositiveOrZero();
  }
  bool IsNegative()const
  { if (this->Extended==0)
      return this->NumShort<0;
    else
      return this->Extended->num.IsNegative();
  }
  bool IsNonPositive()const
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
  void ReadFromFile(std::istream& input);
  inline void ReadFromFile(std::istream& input, GlobalVariables* theGlobalVariables){this->ReadFromFile(input);}
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData);
  inline void AssignAbsoluteValue(){if(this->IsNegative())this->Minus(); }
  static Rational NChooseK(int n, int k);
  static Rational Factorial(int n, GlobalVariables* theGlobalVariables);
  static Rational Factorial(int n){return Rational::Factorial(n,0);}
  static Rational TwoToTheNth(int n);
  static Rational NtoTheKth(int n, int k);
  void RaiseToPower(int x);
  //don't ever call the below manually or you can get memory leak (extended must be nullified here by
  //default!
  Rational(int n, int d){this->Extended=0; this->AssignNumeratorAndDenominator(n, d); ParallelComputing::SafePointDontCallMeFromDestructors();}
  Rational(const LargeInt& other){this->Extended=0; *this=other;}
  Rational(const LargeIntUnsigned& other){this->Extended=0; *this=other;}
  Rational(): NumShort(0), DenShort(0), Extended(0){ ParallelComputing::SafePointDontCallMeFromDestructors();}
  Rational(int n){this->Extended=0; this->AssignNumeratorAndDenominator(n, 1); ParallelComputing::SafePointDontCallMeFromDestructors(); }
  Rational(const Rational& right){this->Extended=0; this->Assign(right); ParallelComputing::SafePointDontCallMeFromDestructors();}
//  Rational(int x){this->Extended=0; this->AssignInteger(x); };
  ~Rational(){this->FreeExtended();}
  //the below must be called only with positive arguments!
  static int gcd(int a, int b)
  { MacroIncrementCounter(Rational::TotalSmallGCDcalls);
    int temp;
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
  inline bool operator==(const int other)
  { if (other==0)
      return this->IsEqualToZero();
    Rational tempRat=other;
    return this->IsEqualTo(other);
  }
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
    MacroIncrementCounter(Rational::TotalLargeAdditions);
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
//  inline void operator*=(const Polynomial<Rational>& right);
  inline void operator/=(const Rational& right){this->DivideBy(right);}
//  inline void operator/=(const LargeInt& right){Rational tempRat; tempRat=right; this->DivideBy(tempRat);}
//  inline void operator/=(const LargeIntUnsigned& right){Rational tempRat; tempRat=right; this->DivideBy(tempRat);}
  inline void operator+=(int right){this->AddInteger(right); }
  inline void operator-=(int right){Rational tempRat=right; this->Subtract(tempRat);}
//  inline bool operator==(int right){Rational tempRat; tempRat.AssignInteger(right); return this->IsEqualTo(tempRat); }
  inline void operator=(int right){this->AssignInteger(right); }
  inline void operator=(const LargeInt& other){this->AssignLargeInteger(other);}
  Rational operator*(const Rational& right)const
  ;
//  { Rational result=*this;
//    result*=right;
//    return result;
//  }
  Rational operator*(const LargeInt& right)const
  { Rational result;
    result=*this;
    result*= (Rational) right;
    return result;
  }
  Rational operator*(int right)const{ Rational tempRat; tempRat.Assign(*this); tempRat.MultiplyByInt(right); return tempRat;}
  Rational operator/(int right)const{ Rational tempRat; tempRat.Assign(*this); tempRat.DivideByInteger(right); return tempRat;}
  Vector<Rational>  operator*(const Vector<Rational> & right)const;
  Rational operator+(const Rational& right)const;
  Rational operator-(const Rational& right)const;
  Rational operator/(const Rational& right)const;
  inline bool operator!=(const Rational& right)const{ return !this->IsEqualTo(right);}
  inline bool operator!=(const int& right)const{ return !((*this)==right);}
  inline bool operator>(const Rational& right)const{return this->IsGreaterThan(right); }
  inline bool operator<(const Rational& right)const{return right.IsGreaterThan(*this); }
  inline bool operator>(const int right)const{Rational tempRat; tempRat.AssignInteger(right); return this->IsGreaterThan(tempRat); }
  inline bool operator>=(const Rational& right)const{return this->IsGreaterThanOrEqualTo(right); }
  inline bool operator<(const int right)const{Rational tempRat; tempRat.AssignInteger(right); return tempRat.IsGreaterThan(*this); }
};

template<class CoefficientType>
Vector<CoefficientType> operator-(const Vector<CoefficientType>& input)
{ Vector<CoefficientType> result;
  result.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    result[i]=-input[i];
  return result;
}

template <class CoefficientType>
std::ostream& operator<<
(std::ostream& out, const Vector<CoefficientType>& theVector)
{ out  << "(";
  for (int i=0; i<theVector.size; i++)
  { out << theVector[i];
    if (i!=theVector.size-1)
      out << ", ";
  }
  out << ")";
  return out;
}

template <class CoefficientType>
class Vector: public ListLight<CoefficientType>
{ friend Vector<CoefficientType> operator-<CoefficientType>(const Vector<CoefficientType>& input);
  friend std::ostream& operator<< <CoefficientType>(std::ostream& output, const Vector<CoefficientType>& theVector)
;
public:
  Vector(){}
  Vector(const Vector<CoefficientType>& other){*this=other;}
  Vector(const Selection& other) {*this=other;}
  inline static std::string GetXMLClassName()
  { std::string result="Vector_" + CoefficientType::GetXMLClassName();
    return result;
  }
  std::string ToString()const
  { std::stringstream out;
    out.precision(5);
    out << "(";
    for(int i=0; i<this->size; i++)
    { out << this->TheObjects[i];
      if (i!=this->size-1)
        out << ", ";
    }
    out << ")";
    return out.str();
  }
  std::string ToString(FormatExpressions* theFormat)const
  { std::stringstream out;
    out.precision(5);
    out << "(";
    for(int i=0; i<this->size; i++)
    { out << this->TheObjects[i].ToString(theFormat);
      if (i!=this->size-1)
        out << ", ";
    }
    out << ")";
    return out.str();
  }
  std::string ToStringLetterFormat
  (const std::string& inputLetter, FormatExpressions* theFormat=0, bool DontIncludeLastVar=false)const
  ;
  std::string ToStringEpsilonFormat()const
  { return this->ToStringLetterFormat("\\varepsilon");
  }
  inline CoefficientType ScalarEuclidean(const Vector<CoefficientType>& other, const CoefficientType& theRingZero=0)const
  { CoefficientType output;
    this->ScalarEuclidean(other, output, theRingZero);
    return output;
  }
  static void ScalarProduct
(const Vector<CoefficientType>& r1, const Vector<CoefficientType>& r2, const Matrix<CoefficientType>& TheBilinearForm, CoefficientType& result)
  { result=Vector<CoefficientType>::ScalarProduct(r1, r2, TheBilinearForm);
  }
  static CoefficientType ScalarProduct
  (const Vector<CoefficientType>& r1, const Vector<CoefficientType>& r2, const Matrix<CoefficientType>& TheBilinearForm)
  { if (r1.size!=TheBilinearForm.NumRows || r1.size!=r2.size || r1.size!=TheBilinearForm.NumCols)
    { std::cout << "This is a programming error: attempting to take a bilinear form represented by matrix with "
      << TheBilinearForm.NumRows << " rows and " << TheBilinearForm.NumCols << " columns "
      << " of vectors of dimension " << r1.size << " and " << r2.size << ". "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    CoefficientType result, tempRat, accumRow;
    result=0;
    for (int i=0; i<TheBilinearForm.NumRows; i++)
    { accumRow=0;
      for(int j =0; j<TheBilinearForm.NumCols; j++)
      { tempRat=r2[j];
        tempRat*=TheBilinearForm.elements[i][j];
        accumRow+=(tempRat);
      }
      accumRow*=r1[i];
      result+=accumRow;
    }
    return result;
  }
  bool IsPositive()
  { return this->IsPositiveOrZero() && !this->IsEqualToZero();
  }
  bool IsNegative()
  { return this->IsNegativeOrZero() && !this->IsEqualToZero();
  }
  Vector<Rational> GetVectorRational()
  { Vector<Rational> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      result[i]=this->TheObjects[i].RationalValue();
    return result;
  }
  Vector<double> GetVectorDouble()
  { Vector<double> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      result[i]=this->TheObjects[i].DoubleValue();
    return result;
  }
  bool IsIntegral()
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsInteger())
        return false;
    return true;
  }
  static Vector<CoefficientType> GetEi(int theDimension, int theIndex)
  { Vector<CoefficientType> output;
    output.MakeEi(theDimension, theIndex);
    return output;
  }
  bool IsNegativeOrZero()
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]>0)
        return false;
    return true;
  }
  bool IsPositiveOrZero()
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]<0)
        return false;
    return true;
  }
  CoefficientType SumCoords()const
  { CoefficientType result=0;
    for (int i=0; i<this->size; i++)
      result+=this->TheObjects[i];
    return result;
  }
  static CoefficientType ScalarEuclidean
  (const Vector<CoefficientType>&left, const Vector<CoefficientType>&right, const CoefficientType& theRingZero=0)
  { CoefficientType output;
    left.ScalarEuclidean(right, output, theRingZero);
    return output;
  }
  static void ScalarEuclidean
  (const Vector<CoefficientType>&left, const Vector<CoefficientType>&right, CoefficientType& output, const CoefficientType& theRingZero=0)
  { left.ScalarEuclidean(right, output, theRingZero);
  }
  void  ScalarEuclidean
  (const Vector<CoefficientType>& other, CoefficientType& output, const CoefficientType& theRingZero=0) const
  { CoefficientType tempElt;
    assert(this->size==other.size);
    output=theRingZero;
    for (int i=0; i<this->size; i++)
    { tempElt=other.TheObjects[i];
      tempElt*=this->TheObjects[i];
      output+=tempElt;
    }
  }

static void ProjectOntoHyperPlane
(Vector<CoefficientType>& input, Vector<CoefficientType>& normal,
 Vector<CoefficientType>& ProjectionDirection, Vector<CoefficientType>& output)
{ CoefficientType t;
  CoefficientType tempRat;
  Vector<CoefficientType>::ScalarEuclidean(input, normal, t);
  Vector<CoefficientType>::ScalarEuclidean(ProjectionDirection, normal, tempRat);
  t/=(tempRat);
  t.Minus();
  Vector<CoefficientType>::VectorPlusVectorTimesScalar(input, ProjectionDirection, t, output);
}
  static void VectorPlusVectorTimesScalar
  (const Vector<CoefficientType>& r1, const Vector<CoefficientType>& r2, const CoefficientType& theCoeff,
   Vector<CoefficientType>& output)
  { CoefficientType tempRat;
    assert(r1.size==r2.size);
    output=r1;
    for (int i=0; i<r1.size; i++)
    { tempRat=r2[i];
      tempRat*=theCoeff;
      output.TheObjects[i]+=(tempRat);
    }
  }

  void Minus()
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]*=-1;
  }
  void MakeEi
  (int DesiredDimension, int NonZeroIndex,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  { this->MakeZero(DesiredDimension, theRingZero);
    (*this)[NonZeroIndex]=theRingUnit;
  }
  inline static unsigned int HashFunction(const Vector<CoefficientType>& input){return input.HashFunction();}
  unsigned int HashFunction() const
  { unsigned int result=0;
    int theSize= MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
    for (int i=0; i<theSize; i++)
      result+=  this->TheObjects[i].HashFunction()*  ::SomeRandomPrimes[i];
    return result;
  }
  void ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive()
  { this->ScaleByPositiveRationalToIntegralMinHeight();
    this->ScaleToFirstNonZeroCoordinatePositive();
  }
  void ScaleToFirstNonZeroCoordinatePositive();
  void MakeAffineUsingLastCoordinate()
  { CoefficientType theElt;
    theElt=*this->LastObject();
    this->size--;
    this->operator/=(theElt);
  }
  void ScaleByPositiveRationalToIntegralMinHeight()
  ;
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
  bool IsProportionalTo(const Vector<CoefficientType>& other, CoefficientType& TimesMeEqualsOther)const;
  bool IsProportionalTo(const Vector<CoefficientType>& other)const
  { CoefficientType TimesMeEqualsOther;
    return this->IsProportionalTo(other, TimesMeEqualsOther);
  }
  int FindLCMDenominatorsTruncateToInt();
  void FindLCMDenominators(LargeIntUnsigned& output);
  inline Vector<CoefficientType> GetShiftToTheLeftOnePosition()
  { return this->GetShiftToTheLeft(1);
  }
  void MakeNormalInProjectivizationFromPointAndNormal(Vector<CoefficientType>& point, Vector<CoefficientType>& normal)
  { //the extra dimension is going to be the last dimension
    int newDimension= normal.size+1;
    this->SetSize(newDimension);
    this->RootScalarEuclideanRoot(normal, point, this->TheObjects[newDimension-1]);
    this->TheObjects[newDimension-1].Minus();
    for (int j=0; j<newDimension-1; j++)
      this->TheObjects[j]=normal[j];
  }
  bool ProjectToAffineSpace(Vector<CoefficientType> &output)
  { if (this->TheObjects[this->size-1].IsEqualToZero())
      return false;
    output.SetSize(this->size-1);
    for (int i=0; i<this->size-1; i++)
      output[i]=(this->TheObjects[i]);
    output/=(this->TheObjects[this->size-1]);
    return true;
  }
  bool MakeAffineProjectionFromNormal(affineHyperplane& output);
/*  { int tempI= this->GetIndexFirstNonZeroCoordinate();
    if (tempI==this->size-1)
      return false;
    output.affinePoint.MakeZero(this->size-1);
    output.normal.SetSize(this->size-1);
    output.affinePoint[tempI].Assign(*this->LastObject());
    output.affinePoint[tempI].Minus();
    output.affinePoint[tempI].DivideBy(this->TheObjects[tempI]);
    for (int i=0; i<this->size-1; i++)
      output.normal[i]=this->TheObjects[i];
    return true;
  }*/
  int GetIndexFirstNonZeroCoordinate()
  { for(int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
        return i;
    return -1;
  }
  int getIndexLastNonZeroCoordinate()
  { for(int i=this->size-1; i>=0; i--)
      if (!this->TheObjects[i].IsEqualToZero())
        return i;
    return -1;
  }
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
/*  template<class OtherType>
  void operator=(const Vector<OtherType>& other)
  { this->SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->TheObjects[i]=other.TheObjects[i];
  }*/
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
  const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
   ;
  bool GetCoordsInBasiS
(const Vectors<CoefficientType>& inputBasis, Vector<CoefficientType>& output,
 const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  { Vectors<CoefficientType> buffer;
    Matrix<CoefficientType> matBuffer;
    return this->GetCoordsInBasiS(inputBasis, output, buffer, matBuffer, theRingUnit, theRingZero);
  }
  Vector<CoefficientType> GetProjectivizedNormal(Vector<CoefficientType>& affinePoint);
  Vector<CoefficientType> operator*(const CoefficientType& other)const
  { Vector<CoefficientType> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
    { result[i]=this->TheObjects[i];
      result[i]*=other;
    }
    return result;
  }
  Vector<CoefficientType> operator/(const CoefficientType& other)const
  { if (other.IsEqualToZero())
    { std::cout << "This is a programming error: division by zero. "
      << " Division by zero error are supposed to be handled at an earlier level. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    Vector<CoefficientType> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
    { result[i]=this->TheObjects[i];
      result[i]/=other;
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
  template <class otherType>
  void operator+=(const Vector<otherType>& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]+=other.TheObjects[i];
  }
  inline bool operator<(const Vector<CoefficientType>& other)const
  { return other>*this;
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
  template <class otherType>
  void operator-=(const Vector<otherType>& other)
  { if (this->size!=other.size)
    { std::cout << "This is a programming error: subtracting vectors "
      << " with different dimensions. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    for (int i=0; i<this->size; i++)
      this->TheObjects[i]-=other[i];
  }
  Vector<CoefficientType> operator+(const Vector<CoefficientType>& right)const
  { Vector<CoefficientType> result=*this;
    result+=right;
    return result;
  }
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
      { //tempS.resize(tempS.size()+1);WeylDimFormula
        //tempS[tempS.size()-1]=input[startIndex];
        tempS.push_back(input[startIndex]);
      }
      if (input[startIndex]==')')
        break;
    }
    return true;
  }
  void operator=(const std::string& input)
  { this->AssignString(input);
  }
  void operator=(const Vector<CoefficientType>& other)
  { if (&other==this)
      return;
    this->SetSize(other.size);
    for(int i=0; i<other.size; i++)
      this->TheObjects[i]=other[i];
  }
  template <class otherType>
  void operator=(const Vector<otherType>& other)
  { this->SetSize(other.size);
    for(int i=0; i<other.size; i++)
      this->TheObjects[i]=other[i];
  }
  void operator=(const List<CoefficientType>& other)
  { this->SetSize(other.size);
    for(int i=0; i<other.size; i++)
      this->TheObjects[i]=other[i];
  }
  void operator=(const Selection& other)
  { if (other.MaxSize<0)
    { this->SetSize(0);
      return;
    }
    this->SetSize(other.MaxSize);
    for(int i=0; i<other.MaxSize; i++)
      if (other.selected[i])
        this->TheObjects[i]=1;
      else
        this->TheObjects[i]=0;
  }
  void operator=(const SelectionWithMultiplicities& other)
  { this->SetSize(other.Multiplicities.size);
    for(int i=0; i<other.Multiplicities.size; i++)
      this->TheObjects[i]=other.Multiplicities[i];
  }
  bool operator!=(const Vector<CoefficientType>& other)const
  { return ! (*this==other);
  }
  bool IsEqualToZero()const
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
  Vector<CoefficientType> operator-(const Vector<CoefficientType>& other)const
  { Vector<CoefficientType> result;
    result=*this;
    result-=other;
    return result;
  }
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){this->ReadFromFile(input);}
  void ReadFromFile(std::fstream& input)
  { std::string tempS;
    input >> tempS;
    assert(tempS=="root_dim:");
    int tempI;
    input >> tempI;
    this->SetSize(tempI);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i].ReadFromFile(input);
  }
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->WriteToFile(output);
  }
  void WriteToFile(std::fstream& output)
  { output << "root_dim: " << this->size << " ";
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].WriteToFile(output);
      output << " ";
    }
  }
};

template <class CoefficientType>
void Vector<CoefficientType>::ScaleByPositiveRationalToIntegralMinHeight()
{ LargeIntUnsigned numGCD, tempUI;
  bool foundNonZero=false;
  for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].IsEqualToZero())
    { if (foundNonZero)
      { if(!numGCD.IsEqualToOne())
        { this->TheObjects[i].GetNumerator(tempUI);
          LargeIntUnsigned::gcd(numGCD, tempUI, numGCD);
        }
      } else
      { this->TheObjects[i].GetNumerator(numGCD);
        foundNonZero=true;
      }
      this->TheObjects[i].GetDenominator(tempUI);
      if (!tempUI.IsEqualToOne())
        *this*= tempUI;
    }
  if (foundNonZero)
    if (!numGCD.IsEqualToOne())
      *this/=(numGCD);
}

template <class CoefficientType>
void Vector<CoefficientType>::ScaleToFirstNonZeroCoordinatePositive()
{ for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i].IsPositive())
      return;
    if (this->TheObjects[i].IsNegative())
    { this->Minus();
      return;
    }
  }
}

template <class CoefficientType>
bool Vector<CoefficientType>::IsProportionalTo
(const Vector<CoefficientType>& input, CoefficientType& outputTimesMeEqualsInput)const
{ if (this->size!= input.size)
    return false;
  int IndexFirstNonZero=-1;
  for(int i=0; i<this->size; i++)
    if (!this->TheObjects[i].IsEqualToZero())
    { IndexFirstNonZero=i;
      break;
    }
  if (IndexFirstNonZero==-1)
  { if (input.IsEqualToZero())
    { outputTimesMeEqualsInput.MakeZero();
      return true;
    }
    return false;
  }
  Vector<Rational> tempRoot=*this;
  outputTimesMeEqualsInput= input.TheObjects[IndexFirstNonZero];
  outputTimesMeEqualsInput/=this->TheObjects[IndexFirstNonZero];
  tempRoot*=outputTimesMeEqualsInput;
  return tempRoot==input;
}

template <class CoefficientType>
void Vector<CoefficientType>::FindLCMDenominators(LargeIntUnsigned& output)
{ LargeIntUnsigned tempI, tempI2;
  output.MakeOne();
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].GetDenominator(tempI2);
    LargeIntUnsigned::gcd(output, tempI2, tempI);
    output.MultiplyBy(tempI2);
    output.DivPositive(tempI, output, tempI2);
  }
}

template <class CoefficientType>
int Vector<CoefficientType>::FindLCMDenominatorsTruncateToInt()
{ int result=1;
  for (int i=0; i<this->size; i++)
  { result = MathRoutines::lcm(result, this->TheObjects[i].DenShort);
    assert(this->TheObjects[i].Extended==0);
  }
  return result;
}

template <class CoefficientType>
class Vectors: public List<Vector<CoefficientType> >
{
  public:
  std::string ElementToStringEpsilonForm(bool useLatex, bool useHtml, bool makeTable)
  { std::string tempS; std::stringstream out;
    if (useLatex)
      useHtml=false;
    if (useHtml && makeTable)
      out << "<table>";
    if (useLatex && makeTable)
      out << "\\begin{array}{l}";
    for (int i=0; i<this->size; i++)
    { tempS=this->TheObjects[i].ToStringEpsilonFormat();
      if (useHtml && makeTable)
        out << "<tr><td>";
      if (!useLatex && useHtml)
        out << "(";
      out << tempS;
      if (!useLatex && useHtml)
        out << ")";
      if (useLatex && makeTable)
        out << "\\\\";
      if (useHtml && makeTable)
        out << "</td></tr>";
      if (!makeTable)
      { if (i!=this->size-1)
          out << ", ";
      }
      else
        out << "\n";
    }
    if (useLatex && makeTable)
      out << "\\end{array}";
    if (useHtml && makeTable)
      out << "</table>";
    return out.str();
  }
  std::string ElementsToInequalitiesString
(bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat)
;
  std::string ToString(FormatExpressions* theFormat=0)const;
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
    { buffer.NonPivotPointsToEigenVector(NonPivotPointsBuffer, output);
      return true;
    }
    return false;
  }
  void GetVectorsDouble(Vectors<double>& output)
  { output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      output[i]=this->TheObjects[i].GetVectorDouble();
  }
  void AssignListListCoefficientType(const List<List<CoefficientType> >& input)
  { this->SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->TheObjects[i]=input[i];
  }
  void AssignListList(const List<Vectors<CoefficientType> >& input)
  { int count=0;
    for (int i=0; i<input.size; i++)
      count+=input[i].size;
    this->SetSize(0);
    this->ReservE(count);
    for (int i=0; i<input.size; i++)
      for (int j=0; j<input[i].size; j++)
        this->AddOnTop(input[i][j]);
  }
  std::string ElementToLinearCombinationString()
  { std::stringstream out;
    std::string tempS;
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].ToString(tempS);
      out << tempS;
       if (i!=this->size-1)
        out << " + ";
    }
    return out.str();
  }
  int ArrangeFirstVectorsBeOfMaxPossibleRank
  (Matrix<CoefficientType>& bufferMat, Selection& bufferSel)
;
  static unsigned int HashFunction(const Vectors<CoefficientType>& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->::List<Vector<CoefficientType> >::HashFunction();
  }
  //The below function is required to preserve the order of elements given by theSelection.elements.
  void SubSelection(Selection& theSelection, Vectors<CoefficientType>& output)
  { assert(&output!=this);
    output.SetSize(theSelection.CardinalitySelection);
    for(int i=0; i<theSelection.CardinalitySelection; i++)
      output[i].Assign(this->TheObjects[theSelection.elements[i]]);
  }
  bool HasAnElementWithPositiveScalarProduct(const Vector<CoefficientType>& input)const
  { for (int i=0; i<this->size; i++)
      if (Vector<CoefficientType>::ScalarEuclidean(this->TheObjects[i], input).IsPositive())
        return true;
    return false;
  }
  bool HasAnElementWithNegativeScalarProduct(const Vector<CoefficientType>& input)const
  { for (int i=0; i<this->size; i++)
      if (Vector<CoefficientType>::ScalarEuclidean(this->TheObjects[i], input).IsNegative())
        return true;
    return false;
  }
  bool HasAnElementPerpendicularTo(const Vector<CoefficientType>& input)const
  { for (int i=0; i<this->size; i++)
      if (Vector<CoefficientType>::ScalarEuclidean(this->TheObjects[i], input).IsEqualToZero())
        return true;
    return false;
  }
  bool PerturbVectorToRegular(Vector<CoefficientType>& inputOutput, GlobalVariables& theGlobalVariables);
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<CoefficientType>& output)
  { output.init((int)OutputDimension, (int)theSelection.CardinalitySelection);
    this->SelectionToMatrix(theSelection, OutputDimension, output, 0);
  }
  void SelectionToMatrixAppend(Selection& theSelection, int OutputDimension, Matrix<CoefficientType>& output, int StartRowIndex)
  { for(int i=0; i<theSelection.CardinalitySelection; i++)
    { Vector<CoefficientType>& tempRoot=this->TheObjects[theSelection.elements[i]];
      for (int j=0; j<OutputDimension; j++)
        output.elements[StartRowIndex+i][j].Assign(tempRoot.TheObjects[j]);
    }
  }
  void SelectionToMatrix
  (Selection& theSelection, int OutputDimension, Matrix<CoefficientType>& output, int StartRowIndex)
  { for(int i=0; i<theSelection.CardinalitySelection; i++)
    { Vector<Rational>& tempRoot=this->TheObjects[theSelection.elements[i]];
      for (int j=0; j<OutputDimension; j++)
        output.elements[StartRowIndex+i][j].Assign(tempRoot.TheObjects[j]);
    }
  }
  void GetGramMatrix
  (Matrix<CoefficientType> & output, const Matrix<CoefficientType>& theBilinearForm)const
  ;
  void GetMatrixRootsToRows(Matrix<Rational> & output)const
  { int tempNumCols= 0;
    if (this->size!=0)
      tempNumCols=(int)this->TheObjects[0].size;
    output.init((int) this->size, tempNumCols);
    for (int i=0; i<this->size; i++)
      for (int j=0; j<tempNumCols; j++)
        output.elements[i][j]=this->TheObjects[i][j];
  }
  bool LinSpanContainsRoot
  (const Vector<CoefficientType>& input, Matrix<CoefficientType>& bufferMatrix, Selection& bufferSelection)const
  ;
  void MakeEiBasis(int theDimension, const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
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
  (Matrix<CoefficientType>* buffer=0, Selection* bufferSelection=0)const
;
static bool ConesIntersect
(GlobalVariables& theGlobalVariables, List<Vector<Rational> >& StrictCone, List<Vector<Rational> >& NonStrictCone,
 int theDimension)
 ;
bool GetNormalSeparatingCones
(GlobalVariables& theGlobalVariables, int theDimension, List<Vector<CoefficientType> >& coneStrictlyPositiveCoeffs,
  List<Vector<CoefficientType> >& coneNonNegativeCoeffs, Vector<CoefficientType>& outputNormal)
;
  void average(Vector<CoefficientType>& output, int theDimension)
  { this->sum(output, theDimension);
    if (this->size==0)
      return;
    output/=this->size;
  }
  void sum (Vector<CoefficientType>& output, int resultDim)const
  { output.MakeZero(resultDim);
    for (int i=0; i<this->size; i++)
      output+=this->TheObjects[i];
  }
  bool GetCoordsInBasis
  (const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& outputCoords,
   Vectors<CoefficientType>& bufferVectors, Matrix<CoefficientType>& bufferMat,
   const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
  bool GetIntegralCoordsInBasisIfTheyExist
  (const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& output,
  const CoefficientType& theRingUnit,
  const CoefficientType& theRingMinusUnit,
  const CoefficientType& theRingZero)const
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
  const CoefficientType& theRingZero)const
  { bool result=true;
    output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].GetIntegralCoordsInBasisIfTheyExist(inputBasis, output.TheObjects[i], bufferMatGaussianEliminationCC, bufferMatGaussianElimination, theRingUnit, theRingMinusUnit, theRingZero))
        result=false;
    return result;
  }
  std::string ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex)
  { std::stringstream out;
    for (int i=0; i<this->size; i++)
    { out << this->TheObjects[i].ElementToStringLetterFormat(inputLetter, useLatex);
      if (i!=this->size-1)
        out << ",";
    }
    return out.str();
  }
  bool ComputeNormalExcludingIndex
(Vector<CoefficientType>& output, int index, Matrix<CoefficientType>& bufferMatrix)
;
bool ComputeNormalFromSelection
(Vector<CoefficientType>& output, Selection& theSelection, Matrix<CoefficientType>& bufferMatrix, int theDimension)
;
bool ComputeNormalFromSelectionAndExtraRoot
(Vector<CoefficientType>& output, Vector<CoefficientType>& ExtraRoot,
 Selection& theSelection, Matrix<CoefficientType>& bufferMatrix, Selection& bufferSel)
  ;
  bool ComputeNormalFromSelectionAndTwoExtraRoots
(Vector<CoefficientType>& output, Vector<CoefficientType>& ExtraRoot1, Vector<CoefficientType>& ExtraRoot2,
 Selection& theSelection, Matrix<CoefficientType>& bufferMat, Selection& bufferSel)
 ;
  bool ComputeNormal(Vector<CoefficientType>& output, int inputDim)
  { if (this->size==0)
    { if (inputDim==1)
      { output.MakeEi(1,0);
        return true;
      }
      return false;
    }
    int theDimension= this->TheObjects[0].size;
    Matrix<CoefficientType> tempMatrix;
    Selection NonPivotPoints;
    NonPivotPoints.init(theDimension);
    output.SetSize(theDimension);
    this->GaussianEliminationForNormalComputation(tempMatrix, NonPivotPoints, theDimension);
    if (NonPivotPoints.CardinalitySelection!=1)
      return false;
    tempMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
    return true;
  }
  void GaussianEliminationForNormalComputation(Matrix<CoefficientType>& inputMatrix, Selection& outputNonPivotPoints, int theDimension)const;
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool GetLinearDependence
  (Matrix<CoefficientType>& outputTheLinearCombination, const CoefficientType& theRingUnit=1, const CoefficientType& theRingZero=0)
  ;
  void GetLinearDependenceRunTheLinearAlgebra
  (Matrix<CoefficientType>& outputTheLinearCombination, Matrix<CoefficientType>& outputTheSystem, Selection& outputNonPivotPoints)
  ;
  bool ContainsARootNonPerpendicularTo
(const Vector<CoefficientType>& input, Matrix<CoefficientType>& theBilinearForm)
;
  bool ContainsOppositeRoots()
  { if (this->size<10)
    { Vector<Rational> tempRoot;
      for(int i=0; i<this->size; i++)
      { tempRoot=this->TheObjects[i];
        tempRoot.Minus();
        for (int j=i+1; j<this->size; j++)
          if (this->TheObjects[j].IsEqualTo(tempRoot))
            return true;
      }
      return false;
    }
    HashedList<Vector<CoefficientType> > tempList;
    tempList.SetExpectedSize(this->size);
    tempList=*this;
    for (int i=0; i <this->size; i++)
      if (tempList.Contains(-this->TheObjects[i]))
        return true;
    return false;
  }
  void AssignMatrixColumns(Matrix<CoefficientType> & mat)
  { Vector<CoefficientType> tempRoot;
    this->SetSize(mat.NumCols);
    tempRoot.SetSize(mat.NumRows);
    for (int i=0; i<mat.NumCols; i++)
    { for (int j=0; j<mat.NumRows; j++)
        tempRoot[j]=mat.elements[j][i];
      this->TheObjects[i]=tempRoot;
    }
  }
  void AssignMatrixRows(const Matrix<CoefficientType>& mat)
  { this->size=0;
    this->SetSize(mat.NumRows);
    for (int i=0; i<mat.NumRows; i++)
    { this->TheObjects[i].SetSize(mat.NumCols);
      for (int j=0; j<mat.NumCols; j++)
        this->TheObjects[i].TheObjects[j]=mat.elements[i][j];
    }
  }
  int GetDim()const
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
  void ChooseABasis(GlobalVariables& theGlobalVariables)
  { Vectors<Rational> output;
    Matrix<CoefficientType> tempMat;
    Selection tempSel;
    for (int i=0; i<this->size; i++)
    { output.AddOnTop(this->TheObjects[i]);
      if (output.GetRankOfSpanOfElements(&tempMat, &tempSel)< output.size)
        output.RemoveLastObject();
    }
    this->operator=(output);
  }
  static void IntersectTwoLinSpaces
  (const Vectors<CoefficientType>& firstSpace, const Vectors<CoefficientType>& secondSpace,
   Vectors<CoefficientType>& output, GlobalVariables& theGlobalVariables, const CoefficientType& theRingZero=0
   )
     ;
  bool IsRegular
  (Vector<CoefficientType>& r, Vector<CoefficientType>& outputFailingNormal,
    GlobalVariables& theGlobalVariables, int theDimension)
;
  void operator=(const List<List<CoefficientType> >& other)
  { this->SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->TheObjects[i]=other.TheObjects[i];
  }
  template <class otherType>
  void operator=(const List<Vector<otherType> >& other)
  { this->SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->TheObjects[i]=other[i];
  }
};

class ChevalleyGenerator
{
public:
  SemisimpleLieAlgebra* owneR;
  int theGeneratorIndex;
  ChevalleyGenerator(): owneR(0), theGeneratorIndex(-1){}
  friend std::ostream& operator << (std::ostream& output, const ChevalleyGenerator& theGen)
  { output << theGen.ToString();
    return output;
  }
  static const bool IsEqualToZero(){return false;}
  static inline unsigned int HashFunction(const ChevalleyGenerator& input)
  { return (unsigned) input.theGeneratorIndex;
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  void MakeGenerator(SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex)
  { this->owneR=&inputOwner;
    this->theGeneratorIndex=inputGeneratorIndex;
  }
  void operator=(const ChevalleyGenerator& other)
  { this->owneR=other.owneR;
    this->theGeneratorIndex=other.theGeneratorIndex;
  }
  bool operator>(const ChevalleyGenerator& other)const;
  std::string ToString(FormatExpressions* inputFormat=0)const;
  bool IsNilpotent()const;
  void CheckConsistencyWithOther(const ChevalleyGenerator& other)const;
  bool operator==(const ChevalleyGenerator& other)
  { this->CheckConsistencyWithOther(other);
    return this->theGeneratorIndex==other.theGeneratorIndex;
  }
};

template <class CoefficientType, unsigned int inputHashFunction(const CoefficientType&)= CoefficientType::HashFunction>
class MonomialTensor
{
  friend std::ostream& operator <<
  (std::ostream& output, const MonomialTensor<CoefficientType, inputHashFunction>& theMon)
  { return output << theMon.ToString();
  }
private:
public:
  List<int> generatorsIndices;
  List<CoefficientType> Powers;
  std::string ToString
  (FormatExpressions* theFormat=0)const;
  bool IsEqualToOne()const
  { return this->generatorsIndices.size==0;
  }
  void operator=(List<int>& other)
  { this->generatorsIndices.ReservE(other.size);
    this->Powers.ReservE(other.size);
    this->MakeConst();
    for (int i=0; i<other.size; i++)
      this->MultiplyByGeneratorPowerOnTheRight(other[i], 1);
  }
  void operator=(const MonomialTensor<CoefficientType, inputHashFunction>& other)
  { this->generatorsIndices=(other.generatorsIndices);
    this->Powers=other.Powers;
  }
  int GetMinNumVars()
  { int result=0;
    for (int i=0; i<this->Powers.size; i++)
      result=MathRoutines::Maximum(result, this->Powers[i].GetMinNumVars());
    return result;
  }
  template<class otherType>
  void operator=(const MonomialTensor<otherType>& other)
  { this->generatorsIndices=(other.generatorsIndices);
    this->Powers.SetSize(other.Powers.size);
    for (int i=0; i<other.Powers.size; i++)
      this->Powers[i]=other.Powers[i];
  }
  bool SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex);
  void MultiplyByGeneratorPowerOnTheRight
  (int theGeneratorIndex, const CoefficientType& thePower)
  ;
  void MultiplyByGeneratorPowerOnTheLeft
  (int theGeneratorIndexStandsToTheLeft, const CoefficientType& thePower)
  ;
  unsigned int HashFunction() const
  { int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
    unsigned int result=0;
    for (int i=0; i<top; i++)
      result+=
      SomeRandomPrimes[i]*this->generatorsIndices[i] +
      SomeRandomPrimes[top-1-i]* inputHashFunction(this->Powers[i]);
    return result;
  }
  static inline unsigned int HashFunction(const MonomialTensor<CoefficientType, inputHashFunction>& input)
  { return input.HashFunction();
  }
  void MakeConst()
  { this->generatorsIndices.size=0;
    this->Powers.size=0;
  }
  bool operator>(const MonomialTensor<CoefficientType, inputHashFunction>& other)const
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
      if (this->Powers[i]>other.Powers[i])
        return true;
    }
    return false;
  }
  bool operator==(const MonomialTensor<CoefficientType, inputHashFunction>& other)const
  { return this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;
  }
  inline void operator*=(const MonomialTensor<CoefficientType, inputHashFunction>& standsOnTheRight)
  { if (standsOnTheRight.generatorsIndices.size==0)
      return;
    if (this==&standsOnTheRight)
    { MonomialTensor<CoefficientType, inputHashFunction> tempMon;
      tempMon=standsOnTheRight;
      (*this)*=(tempMon);
      return;
    }
    this->generatorsIndices.SetExpectedSize
    (standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
    this->Powers.SetExpectedSize
    (standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
    int firstIndex=standsOnTheRight.generatorsIndices[0];
    int i=0;
    if (this->generatorsIndices.size>0)
      if (firstIndex==(*this->generatorsIndices.LastObject()))
      { *this->Powers.LastObject()+=standsOnTheRight.Powers[0];
        i=1;
      }
    for (; i<standsOnTheRight.generatorsIndices.size; i++)
    { this->Powers.AddOnTop(standsOnTheRight.Powers[i]);
      this->generatorsIndices.AddOnTop(standsOnTheRight.generatorsIndices[i]);
    }
  }
};

class MonomialP
{
private:
  //monbody contains the exponents of the variables.
  //IMPORTANT. The monBody of a monomial is NOT unique.
  //Two monomials whose non-zero entries coincide
  //(but otherwise one monomial might have more entries filled with zeroes)
  //are considered to be equal.
  //Therefore special attention must be paid when performing operations with
  //MonomialP's, especially with operator== and MonomialP::HashFunction!
  //Please modify this class in accordance with what was just explained.
  //Note that by the above token I decided to declare operator[] as non-const
  //function and operator() as a const function but returning a copy of the
  //underlying element, rather than a reference to the element.
  //
  //IMPORTANT. The default monomial order, implemented by operator>, is the graded lexicographic
  //last variable strongest order. Other monomial orders are not referred by operator>, but
  // by their explicit names.
  //Note that the MonomialCollection::ToString method uses the FormatExpressions::thePolyMonOrder
  //to sort monomials when displaying polynomials to the screen.
  List<Rational> monBody;
public:
  friend std::ostream& operator << (std::ostream& output, const MonomialP& theMon)
  { output << theMon.ToString();
    return output;
  }
  Rational& operator[](int i)
  { if (i<0)
    { std::cout << "This is a programming error: requested exponent of monomial variable "
      << " with index " << i << " which is negative. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (i>=this->monBody.size)
      this->SetNumVariablesSubDeletedVarsByOne(i+1);
    return this->monBody[i];
  }
  Rational operator()(int i)const
  { if (i<0)
    { std::cout << "This is a programming error: requested exponent of monomial variable "
      << " with index " << i << " which is negative. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (i>=this->monBody.size)
      return 0;
    return this->monBody[i];
  }
  inline static std::string GetXMLClassName(){ return "MonomialP"; }
  unsigned  int HashFunction() const
  { return this->monBody.HashFunction();
  }
  bool HasPositiveOrZeroExponents()const
  { for (int i=0; i<this->monBody.size; i++)
      if (this->monBody[i].IsNegative())
        return false;
    return true;
  }
  static const inline bool IsEqualToZero()
  { return false;
  }
  void ExponentMeBy(const Rational& theExp);
  //Warning: HashFunction must return the same result
  //for equal monomials represented by different monBodies.
  // Two such different representation may differ by extra entries filled in with zeroes.
  static inline unsigned int HashFunction(const MonomialP& input)
  { unsigned int result=0;
    int numCycles=MathRoutines::Minimum(input.monBody.size, SomeRandomPrimesSize);
    for (int i=0; i<numCycles; i++)
      result+=input.monBody[i].HashFunction();
    return result;
  }
  std::string ToString(FormatExpressions* PolyFormat=0)const;
  void MakeOne(int ExpectedNumVars=0)
  { this->monBody.initFillInObject(ExpectedNumVars, (Rational) 0);
  }
  bool operator>(const MonomialP& other)const;
  bool IsDivisibleBy(const MonomialP& other)const;
  Rational TotalDegree()const
  { Rational result=0;
    for (int i=0; i<this->monBody.size; i++)
      result+=this->monBody[i];
    return result;
  }
  inline void MultiplyBy(const MonomialP& other)
  { this->operator*=(other);
  }
  inline void DivideBy(const MonomialP& other)
  { this->operator/=(other);
  }
  bool IsLinear()const
  { return this->IsAConstant() || this->IsLinearNoConstantTerm();
  }
  bool IsLinearNoConstantTerm()const
  { int tempInt;
    return this->IsOneLetterFirstDegree(&tempInt);
  }
  bool IsOneLetterFirstDegree(int* whichLetter=0)const
  { Rational whichDegree;
    if (!this->IsOneLetterNthDegree(whichLetter, &whichDegree))
      return false;
    return whichDegree==1;
  }
  bool IsOneLetterNthDegree(int* whichLetter=0, Rational* whichDegree=0)const
  { int tempI1;
    if (whichLetter==0)
      whichLetter=&tempI1;
    *whichLetter=-1;
    for (int i=0; i<this->monBody.size; i++)
      if (this->monBody[i]!=0)
      { if (whichDegree!=0)
          *whichDegree=this->monBody[i];
        if ((*whichLetter)==-1)
          *whichLetter=i;
        else
          return false;
      }
    return (*whichLetter)!=-1;
  }
  template <class Element>
  bool SubstitutioN
(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, const Element& theRingUnit)
  ;
  void MakeEi(int LetterIndex, int Power=1, int ExpectedNumVars=0);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { for (int i=this->monBody.size-1; i>=0; i--)
      if (this->monBody[i]!=0)
        return i;
    return -1;
  }
  bool IsGEQpartialOrder(MonomialP& m);
  static bool LeftIsGEQLexicographicLastVariableStrongest
  (const MonomialP& left, const MonomialP& right)
  { return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableStrongest
  (const MonomialP& left, const MonomialP& right)
  { if (left==(right))
      return false;
    return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftIsGEQLexicographicLastVariableWeakest
  (const MonomialP& left, const MonomialP& right)
  { return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest
  (const MonomialP& left, const MonomialP& right)
  { if (left==right)
      return false;
    return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }
  static bool LeftIsGEQTotalDegThenLexicographicLastVariableStrongest
  (const MonomialP& left, const MonomialP& right)
  { return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }

  bool IsGEQLexicographicLastVariableStrongest(const MonomialP& other)const;
  bool IsGEQLexicographicLastVariableWeakest(const MonomialP& other)const;
  bool IsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& other)const;
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  bool IsAConstant()const
  { for (int i=0; i<this->monBody.size; i++)
      if (!this->monBody[i].IsEqualToZero())
        return false;
    return true;
  }
  inline int GetMinNumVars()const
  { return this->monBody.size;
  }
  void Invert()
  { for (int i=0; i<this->monBody.size; i++)
      this->monBody[i].Minus();
  }
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData);
  void RaiseToPower(const Rational& thePower)
  { for (int i=0; i<this->monBody.size; i++)
      this->monBody[i]*=thePower;
  }
  void operator*=(const MonomialP& other);
  void operator/=(const MonomialP& other);
  bool operator==(const MonomialP& other)const;
  template <class CoefficientType>
  void operator=(const Vector<CoefficientType>& other)
  { this->monBody=(other);
  }
  void operator=(const MonomialP& other)
  { this->monBody=(other.monBody);
  }
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { this->monBody.ReadFromFile(input);
  }
  void ReadFromFile(std::fstream& input)
  { this->monBody.ReadFromFile(input);
  }
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->monBody.WriteToFile(output);
  }
  void WriteToFile(std::fstream& output)
  { this->monBody.WriteToFile(output);
  }
};

class MonomialWeylAlgebra
{
  public:
  MonomialP polynomialPart;
  MonomialP differentialPart;
  friend std::ostream& operator << (std::ostream& output, const MonomialWeylAlgebra& theMon)
  { output << theMon.ToString();
    return output;
  }
  static const inline bool IsEqualToZero()
  { return false;
  }
  bool IsAConstant()const
  { return this->polynomialPart.IsAConstant() && this->differentialPart.IsAConstant();
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  void operator=(const MonomialWeylAlgebra& other)
  { this->polynomialPart=other.polynomialPart;
    this->differentialPart=other.differentialPart;
  }
  static unsigned int HashFunction(const MonomialWeylAlgebra& input)
  { return
    input.polynomialPart.HashFunction()+
    input.differentialPart.HashFunction()*SomeRandomPrimes[0];
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  int GetMinNumVars()const
  { return
    MathRoutines::Maximum
    (this->polynomialPart.GetMinNumVars(), this->differentialPart.GetMinNumVars());
  }
  bool operator==(const MonomialWeylAlgebra& other)const
  { return this->polynomialPart==other.polynomialPart &&
    this->differentialPart==other.differentialPart;
  }
  bool operator>(const MonomialWeylAlgebra& other)const
  { if (this->differentialPart>other.differentialPart)
      return true;
    if (other.differentialPart>this->differentialPart)
      return false;
    if (this->polynomialPart>other.polynomialPart)
      return true;
    if(other.polynomialPart>this->polynomialPart)
      return false;
    return false;
  }
  void MakeOne(int ExpectedNumVars=0)
  { this->polynomialPart.MakeOne(ExpectedNumVars);
    this->differentialPart.MakeOne(ExpectedNumVars);
  }
};


struct FormatExpressions
{
public:
  //alphabetBases must contain at least two elements
  std::string chevalleyGgeneratorLetter;
  std::string chevalleyHgeneratorLetter;
  std::string fundamentalWeightLetter;
  std::string polyDefaultLetter;
  std::string CustomPlusSign;
  std::string CustomCoeffMonSeparator;
  std::string FDrepLetter;
  std::string simpleRootLetter;
  std::string DisplayNameCalculator;
  std::string physicalPath;
  std::string htmlPathServer;
  List<std::string> polyAlphabeT;
  std::string GetPolyLetter(int index)const;
  std::string GetChevalleyHletter(int index)const;
  std::string GetChevalleyGletter(int index)const;
  int ExtraLinesCounterLatex;
  int NumAmpersandsPerNewLineForLaTeX;
  int MaxRecursionDepthPerExpression;
  int MaxLineLength;
  int MaxLinesPerPage;
  int MatrixColumnVerticalLineIndex;
  bool flagPassCustomCoeffMonSeparatorToCoeffs;
  bool flagMakingExpressionTableWithLatex;
  bool flagUseLatex;
  bool flagUsePNG;
  bool flagUseHTML;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsFinal;
  bool flagExpressionNewLineAllowed;
  bool flagIncludeLieAlgebraNonTechnicalNames;
  bool flagIncludeLieAlgebraTypes;

  List<MonomialP>::OrderLeftGreaterThanRight thePolyMonOrder;
  template <typename TemplateMonomial>
  typename List<TemplateMonomial>::OrderLeftGreaterThanRight GetMonOrder();
  void operator=(const FormatExpressions& other)
  { this->chevalleyGgeneratorLetter=other.chevalleyGgeneratorLetter;
    this->chevalleyHgeneratorLetter=other.chevalleyHgeneratorLetter ;
    this->fundamentalWeightLetter=other.fundamentalWeightLetter;
    this->polyDefaultLetter=other.polyDefaultLetter;
    this->CustomPlusSign=other.CustomPlusSign;
    this->CustomCoeffMonSeparator=other.CustomCoeffMonSeparator;
    this->FDrepLetter=other.FDrepLetter;
    this->simpleRootLetter=other.simpleRootLetter;
    this->DisplayNameCalculator=other.DisplayNameCalculator;
    this->physicalPath=other.physicalPath;
    this->htmlPathServer=other.htmlPathServer;
    this->polyAlphabeT=other.polyAlphabeT;
    this->ExtraLinesCounterLatex=other.ExtraLinesCounterLatex;
    this->NumAmpersandsPerNewLineForLaTeX=other.NumAmpersandsPerNewLineForLaTeX;
    this->MaxRecursionDepthPerExpression=other.MaxRecursionDepthPerExpression;
    this->MaxLineLength=other.MaxLineLength;
    this->MaxLinesPerPage=other.MaxLinesPerPage;
    this->MatrixColumnVerticalLineIndex=other.MatrixColumnVerticalLineIndex;
    this->flagPassCustomCoeffMonSeparatorToCoeffs=other.flagPassCustomCoeffMonSeparatorToCoeffs;
    this->flagMakingExpressionTableWithLatex=other.flagMakingExpressionTableWithLatex;
    this->flagUseLatex=other.flagUseLatex;
    this->flagUsePNG=other.flagUsePNG;
    this->flagUseHTML=other.flagUseHTML;
    this->flagUseCalculatorFormatForUEOrdered=other.flagUseCalculatorFormatForUEOrdered;
    this->flagQuasiDiffOpCombineWeylPart=other.flagQuasiDiffOpCombineWeylPart;
    this->thePolyMonOrder=other.thePolyMonOrder;
    this->flagExpressionIsFinal=other.flagExpressionIsFinal;
    this->flagExpressionNewLineAllowed=other.flagExpressionNewLineAllowed;
  }
  FormatExpressions();
};

template <class Element>
std::iostream& operator <<(std::iostream& output, const Polynomial<Element>& input)
{ output << input.ToString();
  return output;
}

template <class TemplateMonomial, class CoefficientType>
std::ostream& operator<<
(std::ostream& output, const MonomialCollection<TemplateMonomial, CoefficientType>& theCollection)
;

template <class TemplateMonomial, class CoefficientType>
class MonomialCollection : public HashedList<TemplateMonomial>
{
private:
  void AddOnTop(const MonomialP& tempP);//<-to guard the now unsafe base class method
  void Clear();//<-to guard the now unsafe base class method
  friend std::ostream& operator<< <TemplateMonomial, CoefficientType>
  (std::ostream& output,
   const MonomialCollection<TemplateMonomial, CoefficientType>& theCollection)
;
public:
  List<CoefficientType> theCoeffs;
  MonomialCollection(){};
  MonomialCollection(const MonomialCollection& other){this->operator=(other);}
  bool NeedsBrackets()const{return this->size>1;}
  std::string ToString(FormatExpressions* theFormat=0)const;
  static unsigned int HashFunction(const MonomialCollection<TemplateMonomial, CoefficientType>& input)
  { unsigned int result=0;
    for (int i=0; i<input.size; i++)
      result+= input.theCoeffs[i].HashFunction()*input[i].HashFunction();
    return result;
  }
  void PopMonomial(int index)
  { this->RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  void PopMonomial(int index, TemplateMonomial& outputMon, CoefficientType& outputCoeff)
  { outputMon=this->TheObjects[index];
    outputCoeff=this->theCoeffs[index];
    this->RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  inline int HashFunction()const
  { return this->HashFunction(*this);
  }
  void AddMonomial(const TemplateMonomial& inputMon, const CoefficientType& inputCoeff)
  { this->CleanupMonIndex
    (this->AddMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));

  }
  inline bool CleanupMonIndex(int theIndex)
  { if (theIndex!=-1)
      if (this->theCoeffs[theIndex]==0)
      { this->RemoveIndexSwapWithLast(theIndex);
        this->theCoeffs.RemoveIndexSwapWithLast(theIndex);
        return true;
      }
    return false;
  }
  int AddMonomialNoCoeffCleanUpReturnsCoeffIndex
  (const TemplateMonomial& inputMon, const CoefficientType& inputCoeff)
  ;

  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRows
  (List<MonomialCollectionTemplate>& theList,
   bool *IvemadeARowSwitch=0, HashedList<TemplateMonomial>* seedMonomials=0
   )
   ;
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRowsDeleteZeroRows
  (List<MonomialCollectionTemplate >& theList,
   bool *IvemadeARowSwitch=0, HashedList<TemplateMonomial>* seedMonomials=0
   )
  { MonomialCollectionTemplate::GaussianEliminationByRows
    (theList, IvemadeARowSwitch, seedMonomials);
    for (int j=theList.size-1; j>=0; j--)
      if (theList[j].IsEqualToZero())
        theList.size--;
  }
  int SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex
  (const TemplateMonomial& inputMon, const CoefficientType& inputCoeff);
  void CleanUpZeroCoeffs()
  { for (int i=0; i<this->size; i++)
      if (this->CleanupMonIndex(i))
        i--;
  }
  void SubtractMonomial(const TemplateMonomial& inputMon, const CoefficientType& inputCoeff)
  { this->CleanupMonIndex(this->SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  int TotalDegree();
  void checkConsistency()const
  { this->GrandMasterConsistencyCheck();
    this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
    for (int i =0; i<this->size; i++)
      this->theCoeffs[i].checkConsistency();
  }
  template <class baseRing>
  baseRing FindGCDCoefficientNumerators()
  { if (this->size==0)
      return 1;
    baseRing result, tempCF;
    this->theCoeffs[0].GetNumerator(result);
    for (int i=1; i<this->size; i++)
    { this->theCoeffs[i].GetNumerator(tempCF);
      CoefficientType::gcd(result, tempCF, result);
    }
    return result;
  }

  Rational FindGCDCoefficientNumeratorsOverRationals()
  { if (this->size==0)
      return 1;
    LargeIntUnsigned result, tempUI;
    this->theCoeffs[0].GetNumerator(result);
    for (int i=1; i<this->size; i++)
    { this->theCoeffs[i].GetNumerator(tempUI);
      LargeIntUnsigned::gcd(result, tempUI, result);
    }
    return result;
  }
  Rational FindLCMCoefficientDenominatorsOverRationals()
  { LargeIntUnsigned result, tempUI, tempRat;
    result=1;
    for (int i=0; i<this->size; i++)
    { this->theCoeffs[i].GetDenominator(tempUI);
      LargeIntUnsigned::lcm(result, tempUI, result);
    }
    return result;
  }
  Rational ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy()
  { if (this->size==0)
      return 1;
    Rational result=1;
    Rational tempRat;
    for (int i=0; i<this->size; i++)
    { tempRat=this->theCoeffs[i].GetLCMNumeratorRationalDenominators();
      *this*=tempRat;
      result*=tempRat;
    }
    LargeIntUnsigned theGCD, tempUI;
    this->theCoeffs[0].GetGCDNumeratorsRationalCoeffs().GetNumerator(theGCD);
    for (int i=0; i<this->size; i++)
    { this->theCoeffs[i].GetGCDNumeratorsRationalCoeffs().GetNumerator(tempUI);
      LargeIntUnsigned::gcd(theGCD, tempUI, theGCD);
    }
    tempRat=theGCD;
    *this/=tempRat;
    result/=tempRat;
    return result;
  }
  static std::string GetBlendCoeffAndMon
  (const TemplateMonomial& inputMon, CoefficientType& inputCoeff,
   bool addPlusToFront, FormatExpressions* theFormat=0)
  ;
  void CheckNumCoeffsConsistency(const char* fileName, int lineName)const
  { if (this->theCoeffs.size!=this->size)
    { std::cout << "This is a programming error: a monomial collection has "
      << this->size << " monomials but "
      << this->theCoeffs.size << " coefficients. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  bool IsEqualToZero()const;
  int FindMaxPowerOfVariableIndex(int VariableIndex);
  void MakeZero()
  { this->::HashedList<TemplateMonomial>::Clear();
    this->theCoeffs.size=0;
  }
  inline int GetMinNumVars()const
  { int result =0;
    for (int i=0; i<this->size; i++)
      result= MathRoutines::Maximum(result, (*this)[i].GetMinNumVars());
    return result;
  }
  bool IsSmallInteger(int* whichInteger=0)const;
  bool IsInteger(LargeInt* whichInteger=0)const;
  void SetExpectedSize(int theSize)
  { this->::HashedList<TemplateMonomial>::SetExpectedSize(theSize);
    this->theCoeffs.SetExpectedSize(theSize);
  }
  bool HasGEQMonomial(TemplateMonomial& m, int& WhichIndex);
  void WriteToFile(std::fstream& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { return this->WriteToFile(output);
  }
  void ClearDenominators(Rational& output)
  { output.MakeOne();
    Rational tempRat;
    for (int i=0; i<this->size; i++)
      if (!this->theCoeffs[i].IsInteger())
      { this->theCoeffs[i].GetDenominator(tempRat);
        *this*=tempRat;
        output*=tempRat;
      }
  }
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  bool operator== (const MonomialCollection<TemplateMonomial, CoefficientType>& other)const;
  bool operator== (int x)const;
  inline void operator+=(const MonomialCollection<TemplateMonomial, CoefficientType>& other);
  MonomialCollection<TemplateMonomial, CoefficientType> operator*(const CoefficientType& other)const
  { MonomialCollection<TemplateMonomial, CoefficientType> result=*this;
    result*=other;
    return result;
  }
//  inline void operator+=(const TemplateMonomial& m)
//  { this->AddMonomial(m, 1);
//  }
  MonomialCollection<TemplateMonomial, CoefficientType> operator+(const MonomialCollection<TemplateMonomial, CoefficientType>& other)const
  { MonomialCollection<TemplateMonomial, CoefficientType> output=*this;
    output+=other;
    return output;
  }
  void MakeConsT(const CoefficientType& coeff)
  { TemplateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, coeff);
  }
  void MakeOne()
  { this->MakeConst(1);
  }
  inline void operator+=(const CoefficientType& other)
  { TemplateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, other);
  }
  inline void operator-=(const CoefficientType& other)
  { TemplateMonomial tempM;
    tempM.MakeOne();
    this->SubtractMonomial(tempM, other);
  }
  template <class OtherType>
  void AssignOtherType(const MonomialCollection<TemplateMonomial, CoefficientType>& other)
  { this->::HashedList<TemplateMonomial>::operator=(other);
    this->theCoeffs.SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->theCoeffs[i]=other.theCoeffs[i];
  }
  inline void operator-=(const MonomialCollection<TemplateMonomial, CoefficientType>& other)
  { this->SubtractOtherTimesCoeff(other);
  }
  inline void SubtractOtherTimesCoeff
  (const MonomialCollection<TemplateMonomial, CoefficientType>& other, CoefficientType* inputcf=0);
  template <class otherType>
  inline void operator/=(const otherType& other)
  { if (other==0)
    { std::cout << "This is a programming error. A MonomialCollection division "
      << "by zero has been requested: division by zero error should"
      << " be handled before calling operator/=. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__,__LINE__);
      assert(false);
      return;
    }
    for (int i=0; i<this->theCoeffs.size; i++)
      this->theCoeffs[i]/=other;
  }
  template <class otherType>
  inline void operator*=(const otherType& other)
  { //int commentmewhendone;
    //this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
    for (int i=0; i<this->theCoeffs.size; i++)
    { //int commentmewhendone;
      //this->theCoeffs[i].checkConsistency();
      ////
      this->theCoeffs[i]*=other;
      ////
      //this->theCoeffs[i].checkConsistency();
      if (i==0)
        if (this->theCoeffs[i].IsEqualToZero())//<- to avoid implicit conversion problems, we make the zero check at the first cycle
        { this->MakeZero();
          return;
        }
    }
  }
  template<class otherType>
  void operator=(const MonomialCollection<TemplateMonomial, otherType>& other)
  { this->theCoeffs=other.theCoeffs;
    this->::HashedList<TemplateMonomial>::operator=(other);
//    int commentwhendone;
 //   this->checkConsistency();
  }
};

template <class TemplateMonomial, class CoefficientType>
class ElementMonomialAlgebra: public MonomialCollection<TemplateMonomial, CoefficientType>
{
  public:
  void MultiplyBy
  (const ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& other,
   ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& output,
   ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& bufferPoly, TemplateMonomial& bufferMon)const
  ;
  void MultiplyBy
  (const TemplateMonomial& other, const CoefficientType& theCoeff,
   ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& output,
   ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& bufferPoly,
   TemplateMonomial& bufferMon)const
  ;
  void operator*=(const ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& other)
  { ElementMonomialAlgebra<TemplateMonomial, CoefficientType> bufferPoly;
    TemplateMonomial bufferMon;
    this->MultiplyBy(other, *this, bufferPoly, bufferMon);
  }
  inline void operator*=(const CoefficientType& other)
  { this->::MonomialCollection<TemplateMonomial, CoefficientType>::operator*= (other);
  }
  void RaiseToPower
(int d, ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& output, const CoefficientType& theRingUniT)
;
  void RaiseToPower(int d)
  { if (d==1)
      return;
    ElementMonomialAlgebra<TemplateMonomial, CoefficientType> theOne;
    theOne.MakeConsT(1);
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
};

template<class CoefficientType>
class Polynomial: public ElementMonomialAlgebra<MonomialP, CoefficientType>
{
public:
  friend std::iostream& operator << <CoefficientType>(std::iostream& output, const Polynomial<CoefficientType>& input);
  Polynomial(int x)
  { this->MakeConsT(x, 0);
  }
  Polynomial(){}
  Polynomial(const Polynomial<CoefficientType>& other)
  { this->operator=(other);
  }
  static bool flagAnErrorHasOccuredTimeToPanic;
  //to avoid code::blocks parsing problems (remove when code::blocks learns to parse parent classes:
  void AddMonomial(const MonomialP& inputMon, const CoefficientType& inputCoeff)
  { this->::MonomialCollection<MonomialP, CoefficientType>::AddMonomial(inputMon, inputCoeff);
  }
  unsigned int HashFunction()const
  { return this->::MonomialCollection<MonomialP, CoefficientType>::HashFunction();
  }
  static inline unsigned int HashFunction(const Polynomial<CoefficientType>& input)
  { return input.HashFunction();
  }
  void MultiplyBy
  (const MonomialP& other, const CoefficientType& theCoeff)
  { Polynomial<CoefficientType> output;
    output.MakeZero();
    output.AddMonomial(other, theCoeff);
    *this*=output;
  }
  void MultiplyBy
  (const MonomialP& other, const CoefficientType& theCoeff, Polynomial<CoefficientType>& output)const
  { output=*this;
    output.MultiplyBy(other, theCoeff);
  }
  //////////////////////////////////////////////
  void AssignFloor()const
  {}
  void AssignMonomialWithExponent(Vector<Rational>& r, const CoefficientType& theCoeff=1)
  { MonomialP tempM=r;
    this->MakeZero(r.size);
    this->AddMonomial(r);
  }
  void GetConstantTerm(CoefficientType& output, const CoefficientType& theRingZero=0)const;
  CoefficientType GetConstantTerm(const CoefficientType& theRingZero=0)const
  { CoefficientType result;
    this->GetConstantTerm(result, theRingZero);
    return result;
  }
bool
FactorMeOutputIsSmallestDivisor(Polynomial<Rational>& output, std::stringstream* comments)
  ;
  void Interpolate(const Vector<CoefficientType>& thePoints, const Vector<CoefficientType>& ValuesAtThePoints);
  bool FindOneVarRatRoots(List<Rational>& output);
  void GetCoeffInFrontOfLinearTermVariableIndex(int index, CoefficientType& output, const CoefficientType& theRingZero);
  void MakeMonomiaL
  (int LetterIndex, const Rational& Power, const CoefficientType& Coeff=1,
   int ExpectedNumVars=0);
  void MakeDegreeOne(int NVar, int NonZeroIndex, const CoefficientType& coeff);
  void MakeDegreeOne(int NVar, int NonZeroIndex1, int NonZeroIndex2, const CoefficientType& coeff1, const CoefficientType& coeff2);
  void MakeDegreeOne(int NVar, int NonZeroIndex, const CoefficientType& coeff1, const CoefficientType& ConstantTerm);
  void MakeLinPolyFromRootNoConstantTerm(const Vector<Rational> & r);
  void MakeLinPolyFromRootLastCoordConst(const Vector<Rational> & input);
  void MakePolyFromDirectionAndNormal
(Vector<CoefficientType>& direction, Vector<CoefficientType>& normal, CoefficientType& Correction)
;
  Polynomial<CoefficientType> GetOne()const
  { Polynomial<CoefficientType> result;
    result.MakeOne();
    return result;
  }
  Polynomial<CoefficientType> GetZero()const
  { Polynomial<CoefficientType> result;
    result.MakeZero();
    return result;
  }
  Rational RationalValue()
  { Rational result;
    this->GetConstantTerm(result, 0);
    return result;
  }
  void MakeConsT(const CoefficientType& theConst, int ExpectedNumVars=0)
  {// int commentGrandMasterChecksWhenDone=-1;
    //this->GrandMasterConsistencyCheck();
    this->MakeZero();
    //this->GrandMasterConsistencyCheck();
    MonomialP theConstMon;
    theConstMon.MakeOne(ExpectedNumVars);
    this->AddMonomial(theConstMon, theConst);
    //this->GrandMasterConsistencyCheck();
  }
  void MakeOne(int ExpectedNumVars=0);
  Rational ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy()
  { if (this->IsEqualToZero())
      return 1;
    Rational result=this->ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    if (this->theCoeffs[this->GetIndexMaxMonomialLexicographicLastVariableStrongest()].IsNegative())
    { *this*=-1;
      result*=-1;
    }
    return result;
  }
  void ScaleToIntegralNoGCDCoeffs();
  void TimesInteger(int a);
  void DivideBy(const Polynomial<CoefficientType>& inputDivisor, Polynomial<CoefficientType>& outputQuotient, Polynomial<CoefficientType>& outputRemainder)const;
  void DivideByConstant(const CoefficientType& r);
  void AddConstant(const CoefficientType& theConst);
  void ShiftVariableIndicesToTheRight(int VarIndexShift);
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  inline void SetDynamicSubtype(int newNumVars)
  { this->SetNumVariablesSubDeletedVarsByOne(newNumVars);
  }
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { int result=-1;
    for (int i=0; i<this->size; i++)
      result=MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    return result;
  }
  void ScaleToPositiveMonomials(MonomialP& outputScale);
  void DecreaseNumVariables(int increment, Polynomial<CoefficientType>& output);
  bool SubstitutioN
  (const List<Polynomial<CoefficientType> >& TheSubstitution, const CoefficientType& theRingUnit=1,
   const CoefficientType& theRingZero=0)
   ;
  Rational TotalDegree()const;
  bool IsEqualToOne()const
  { CoefficientType tempC;
    if (this->IsAConstant(&tempC))
      return tempC.IsEqualToOne();
    return false;
  }
  bool IsAConstant(CoefficientType* whichConstant=0)const
  { if (this->size>1)
      return false;
    if (this->size==0)
    { if (whichConstant!=0)
        *whichConstant=0;
      return true;
    }
    if (whichConstant!=0)
      *whichConstant=this->theCoeffs[0];
    MonomialP& theMon=this->TheObjects[0];
    return theMon.IsAConstant();
  }
  bool IsNegative()const
  { CoefficientType tempC;
    if(!this->IsAConstant(&tempC))
      return false;
    return tempC.IsNegative();
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
  bool IsLinearGetRootConstantTermLastCoordinate(Vector<CoefficientType>& outputRoot, const CoefficientType& theZero)
  { outputRoot.MakeZero(this->GetMinNumVars()+1, theZero);
    int index;
    for (int i=0; i<this->size; i++)
      if(this->TheObjects[i].IsAConstant())
        *outputRoot.LastObject()=this->theCoeffs[i];
      else
        if (this->TheObjects[i].IsOneLetterFirstDegree(&index))
          outputRoot.TheObjects[index]=this->theCoeffs[i];
        else
          return false;
    return true;
  }
  void RaiseToPower(int d, const CoefficientType& theRingUniT=1)
  { if (d==1)
      return;
    Polynomial<CoefficientType> theOne;
    theOne.MakeOne(this->GetMinNumVars());
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
  inline bool GetRootFromLinPolyConstTermLastVariable(Vector<CoefficientType>& outputRoot, const CoefficientType& theZero= (CoefficientType) 0){return this->IsLinearGetRootConstantTermLastCoordinate(outputRoot, theZero);}
  void Evaluate
(const Vector<CoefficientType>& input, CoefficientType& output, const CoefficientType& theRingZero=0)
  ;
  bool IsProportionalTo(const Polynomial<CoefficientType>& other, CoefficientType& TimesMeEqualsOther, const CoefficientType& theRingUnit)const;
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData, FormatExpressions& PolyFormatLocal);
  const MonomialP& GetMaxMonomial(List<MonomialP>::OrderLeftGreaterThanRight theMonOrder)const
  { return (*this)[this->GetIndexMaxMonomial(theMonOrder)];
  }
  int GetIndexMaxMonomial(List<MonomialP>::OrderLeftGreaterThanRight theMonOrder)const
  { if (this->size==0)
      return -1;
    int result=0;
    for (int i=1; i<this->size; i++)
      if (theMonOrder(this->TheObjects[i], this->TheObjects[result]))
        result=i;
    return result;
  }
  int GetIndexMaxMonomialLexicographicLastVariableStrongest()const;
//  void ComponentInFrontOfVariableToPower(int VariableIndex, ListPointers<Polynomial<CoefficientType> >& output, int UpToPower);
  int GetMaxPowerOfVariableIndex(int VariableIndex);
  bool operator<=(const CoefficientType& other)const
  { CoefficientType constME;
    if (!this->IsAConstant(&constME))
    { std::cout << "This may or may not be a programming error: "
      << "attempting to compare a non-constant polynomial to "
      << " a constant. I cannot judge at the moment whether allowing "
      << " that is a good decision. In any case, crashing to let you know. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
    return constME<=other;
  }
  bool operator<(const CoefficientType& other)const
  { CoefficientType constME;
    if (!this->IsAConstant(&constME))
    { std::cout << "This may or may not be a programming error: "
      << "attempting to compare a non-constant polynomial to "
      << " a constant. I cannot judge at the moment whether allowing "
      << " that is a good decision. In any case, crashing to let you know. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
    return constME<other;
  }

  bool operator>(const Polynomial<CoefficientType>& other)const
  { if (other.size==0)
    { if (this->size==0)
        return false;
      else
        return true;
    }
    if (this->size==0)
      return false;
    if (this->TotalDegree()>other.TotalDegree())
      return true;
    if (this->TotalDegree()<other.TotalDegree())
      return false;
    int thisMaxMonIndex= this->GetIndexMaxMonomialLexicographicLastVariableStrongest();
    int otherMaxMonIndex= other.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    if( (*this)[thisMaxMonIndex]>other[otherMaxMonIndex])
      return true;
    if( other[otherMaxMonIndex]>(*this)[thisMaxMonIndex])
      return false;
    if(this->theCoeffs[thisMaxMonIndex]>other.theCoeffs[otherMaxMonIndex])
      return true;
    if(other.theCoeffs[otherMaxMonIndex]>this->theCoeffs[thisMaxMonIndex])
      return false;
    return false;
  }
  bool operator<=(const Polynomial<CoefficientType>& other)const
  { return ! (*this>other);
  }
  //has to be rewritten please don't use!
  bool IsGreaterThanZeroLexicographicOrder();
  static bool IsGEQcompareByTopMonomialTotalDegThenLexicographic
  (const Polynomial<CoefficientType>& left, const Polynomial<CoefficientType>& right)
  { if (left.IsEqualToZero())
      return right.IsEqualToZero();
    if (right.IsEqualToZero())
      return true;
    return left[left.GetIndexMaxMonomialTotalDegThenLexicographic()]
    .MonomialP::IsGEQTotalDegThenLexicographic
    (right[right.GetIndexMaxMonomialTotalDegThenLexicographic()]);
  }
  static bool IsGEQcompareByTopMonomialLexicographicLastVarStrongest
  (const Polynomial<CoefficientType>& left, const Polynomial<CoefficientType>& right)
  { if (left.IsEqualToZero())
      return right.IsEqualToZero();
    if (right.IsEqualToZero())
      return true;
    return left[left.GetIndexMaxMonomialLexicographicLastVariableStrongest()]
    .MonomialP::IsGEQLexicographicLastVariableStrongest
    (right[right.GetIndexMaxMonomialLexicographicLastVariableStrongest()]);
  }
  bool IsEqualTo(const Polynomial<CoefficientType>& p)const{return *this==p;}
  void operator-=(int x)
  { MonomialP tempMon;
    tempMon.MakeOne();
    this->SubtractMonomial(tempMon, x);
  }
  void operator-=(const CoefficientType& other)
  { MonomialP tempMon;
    tempMon.MakeOne();
    this->SubtractMonomial(tempMon, other);
  }
  void operator-=(const Polynomial<CoefficientType>& other)
  { this->::MonomialCollection<MonomialP, CoefficientType>::operator-=(other);
  }
  void operator*=(const MonomialP& other)
  { Polynomial<CoefficientType> otherP;
    otherP.MakeZero(this->NumVars);
    otherP.AddMonomial(other, 1);
    (*this)*=otherP;
  }
  void operator*=(const Polynomial<CoefficientType>& other)
  { this->::ElementMonomialAlgebra<MonomialP, CoefficientType>::operator*=(other);
  }
  void operator/=(const Polynomial<CoefficientType>& other)
  { Polynomial<CoefficientType> tempMe=*this;
    Polynomial<CoefficientType> tempRemainder;
    tempMe.DivideBy(other, *this, tempRemainder);
  }
  void operator/=(int other)
  { this->::MonomialCollection<MonomialP, CoefficientType>::operator/= (other);
  }
  void operator/=(const CoefficientType& other)
  { this->::MonomialCollection<MonomialP, CoefficientType>::operator/= (other);
  }
  template <class otherType>
  inline void operator*=(const otherType& other)
  { this->::MonomialCollection<MonomialP, CoefficientType>::operator*= (other);
  }
  void operator=(const Polynomial<CoefficientType>& other)
  { this->::MonomialCollection<MonomialP, CoefficientType>::operator=(other);
  }
  template<class otherType>
  void operator=(const Polynomial<otherType>& other)
  { this->::MonomialCollection<MonomialP, CoefficientType>::operator=(other);
  }
  void operator=(const CoefficientType& other)
  { this->MakeConsT(other);
  }
  void operator=(int other)
  { CoefficientType tempCF;
    tempCF=other;
    this->MakeConst(tempCF);
  }
  template <class otherType>
  void AssignOtherType(const Polynomial<otherType>& other)
  { this->NumVars=other.NumVars;
    this->::MonomialCollection<MonomialP, CoefficientType>::AssignOtherType(other);
  }
};

template <class Element>
class PolynomialSubstitution: public List<Polynomial<Element> >
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
  void MakeExponentSubstitution(Matrix<LargeInt>& theSub);
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
      this->TheObjects[i].MakeZero((int)NumVars);
  }
  std::string ToString(int numDisplayedEltsMinus1ForAll=-1)const
  { std::string tempS;
    this->ToString(tempS, numDisplayedEltsMinus1ForAll);
    return tempS;
  }
  void ComputeDebugString();
  bool operator==(const PolynomialSubstitution& right);
  void Substitution(PolynomialSubstitution<Element>& theSub, int NumVarsTarget)
  { Polynomial<Rational>  tempP;
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].Substitution(theSub, tempP, NumVarsTarget, (Rational) 1);
      this->TheObjects[i]=(tempP);
    }
  }
  void MakeOneParameterSubFromDirection(Vector<Rational>& direction)
  { MonomialP tempM;
    tempM.MakeOne(1);
    tempM.monBody[0]=1;
    this->SetSize(direction.size);
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].MakeZero(1);
      this->TheObjects[i].AddMonomial(tempM,direction.TheObjects[i]);
    }
  }
  void MakeSubFromMatrixIntAndDen(Matrix<LargeInt>& theMat, LargeIntUnsigned& Den)
  { Matrix<Rational>  tempMat;
    tempMat.AssignMatrixIntWithDen(theMat, Den);
    this->MakeLinearSubConstTermsLastRow(tempMat);
  }
  void ToString(std::string& output, int numDisplayedEltsMinus1ForAll)const
  { std::stringstream out;
    output.clear();
    FormatExpressions PolyFormatLocal;
    if (numDisplayedEltsMinus1ForAll==-1)
      numDisplayedEltsMinus1ForAll=this->size;
    for (int i=0; i<this->size; i++)
    { out << "x_{" << i+1 << "} \\mapsto " << this->TheObjects[i].ToString(&PolyFormatLocal);
      if (i!=this->size-1)
        out << ", ";
    }
    out << ":   ";
    output=out.str();
  }
  void ComputeB(Polynomial<Rational> & output, int cutOffIndex, int theDimension)
  { PolynomialSubstitution EpsForm;
    Polynomial<Rational>  tempP;
    EpsForm.SetSize(theDimension);
    EpsForm.TheObjects[0]=(this->TheObjects[0]);
    for (int i=1; i<theDimension; i++)
    { EpsForm.TheObjects[i]=(this->TheObjects[i]);
      tempP=(this->TheObjects[i-1]);
      tempP*=(-1);
      EpsForm.TheObjects[i]+=(tempP);
    }
    EpsForm.TheObjects[3]+=(this->TheObjects[4]);
    for (int j=cutOffIndex+1; j<theDimension; j++)
      if (!EpsForm.TheObjects[j].IsGreaterThanZeroLexicographicOrder())
        EpsForm.TheObjects[j]*=(-1);
    output.MakeZero();
    for (int i=0; i<theDimension; i++)
      output+=(EpsForm.TheObjects[i]);
  }
  void MakeLinearSubConstTermsLastRow(Matrix<Element> & theMat);
  void MakeConstantShiftCoordinatesAreAdded(Vector<Rational> & shiftPerVariable);
  void MakeLinearSubbedVarsCorrespondToMatRows(Matrix<Rational> & theMat, Vector<Rational> & theConstants);
  void ComputeDiscreteIntegrationUpTo(int d);
  void MakeLinearSubOnLastVariable(int NumVars, Polynomial<Rational> & LastVarSub)
  { this->SetSize(NumVars);
    for (int i=0; i<NumVars-1; i++)
      this->TheObjects[i].MakeDegreeOne(NumVars, i, 1);
    this->TheObjects[NumVars-1]=LastVarSub;
  }
  void MakeSubNVarForOtherChamber(Vector<Rational> & direction, Vector<Rational> & normal, Rational& Correction, GlobalVariables& theGlobalVariables)
  { this->SetSize(direction.size);
    Polynomial<Rational> TempPoly;
    TempPoly.MakePolyFromDirectionAndNormal(direction, normal, Correction);
    for (int i=0; i<direction.size; i++)
    { Polynomial<Rational> TempPoly2;
      TempPoly2=TempPoly;
      TempPoly2*=direction[i];
      TempPoly2*=-1;
      this->TheObjects[i].MakeDegreeOne(direction.size, i, 1);
      this->TheObjects[i]+=TempPoly2;
    }
  }
  void MakeSubAddExtraVarForIntegration(Vector<Rational> & direction)
  { this->SetSize(direction.size);
    for (int i=0; i<direction.size; i++)
    { Rational tempRat; tempRat.Assign(direction.TheObjects[i]); tempRat.Minus();
      this->TheObjects[i].MakeDegreeOne((int)(direction.size+1), i, direction.size, 1, tempRat);
    }
  }
  void MakeIdSubstitution(int numVars);
};

//std::iostream& operator<<(std::iostream& output, const RationalFunctionOld& theRF);
//std::iostream& operator>>(std::iostream& input, RationalFunctionOld& theRF);

class GroebnerBasisComputation
{
  public:
  List<MonomialP>::OrderLeftGreaterThanRight theMonOrdeR;
  Polynomial<Rational> SoPolyBuf;
  Polynomial<Rational> remainderDivision;
  Polynomial<Rational> bufPoly;
  Polynomial<Rational> bufPolyForGaussianElimination;
  MonomialP SoPolyLeftShift;
  MonomialP SoPolyRightShift;
  MonomialP bufferMoN1;
  List<Polynomial<Rational> > theBasiS;
  List<Polynomial<Rational> > basisCandidates;
  List<MonomialP> leadingMons;
  List<Rational> leadingCoeffs;
  int NumberOfComputations;
  int MaxNumComputations;
  bool flagBasisGuaranteedToGenerateIdeal;
  bool flagDoProgressReport;
  bool flagDoSortBasis;
  bool flagDoLogDivision;
  MemorySaving<List<Polynomial<Rational> > > intermediateRemainders;
  MemorySaving<List<MonomialP> > intermediateHighestMonDivHighestMon;
  MemorySaving<List<Rational> > intermediateCoeffs;
  MemorySaving<List<Polynomial<Rational> > > intermediateSubtractands;
  MemorySaving<List<int> > intermediateSelectedDivisors;
  MemorySaving<Polynomial<Rational> > startingPoly;
  std::string GetPolynomialStringSpacedMonomials
  (const Polynomial<Rational>& thePoly, const HashedList<MonomialP>& theMonomialOrder,
   const std::string& extraStyle, FormatExpressions* theFormat=0)
  ;
  std::string GetDivisionString(FormatExpressions* theFormat=0);

 bool AddPolyAndReduceBasis
 (GlobalVariables* theGlobalVariables)
 ;
  bool TransformToReducedGroebnerBasis
  (List<Polynomial<Rational> >& inputOutpuT,
   GlobalVariables* theGlobalVariables=0)
   ;
  bool TransformToReducedGroebnerBasisImprovedAlgorithm
  (List<Polynomial<Rational> >& inputOutpuT,
   GlobalVariables* theGlobalVariables=0, int upperComputationBound=-1);
  GroebnerBasisComputation();
  void MakeMinimalBasis
()
  ;
  void RemainderDivisionWithRespectToBasis
(Polynomial<Rational>& inputOutput,
 Polynomial<Rational>* outputRemainder=0, GlobalVariables* theGlobalVariables=0,
  int basisIndexToIgnore=-1
 );
 bool AddRemainderToBasis
 (GlobalVariables* theGlobalVariables)
 ;
   //Criterion from Cox, Little, O'Shea:
  static bool CriterionCLOsh
(HashedListSpecialized
 <Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&
 thePairs, List<MonomialP>& theLeadingMons, MonomialP& leadingTermLCM)
  ;
  void ConsistencyCheck();

  void initForGroebnerComputation(List<Polynomial<Rational> >& inputOutpuT, GlobalVariables* theGlobalVariables);
  void initForDivisionAlone(List<Polynomial<Rational> >& inputOutpuT, GlobalVariables* theGlobalVariables);
};

class RationalFunctionOld
{
private:
  void AddSameTypes(const RationalFunctionOld& other)
  { switch (this->expressionType)
    { case RationalFunctionOld::typeRational: this->ratValue+=other.ratValue; break;
      case RationalFunctionOld::typePoly: this->Numerator.GetElement()+=other.Numerator.GetElementConst(); break;
      case RationalFunctionOld::typeRationalFunction: this->AddHonestRF(other); break;
    }
    this->ReduceMemory();
    assert(this->checkConsistency());
  }
  void AddHonestRF(const RationalFunctionOld& other);
  void ReduceRFToPoly()
  { if (this->expressionType==this->typeRationalFunction)
    { if (this->Denominator.GetElement().IsAConstant())
      { this->Numerator.GetElement()/=this->Denominator.GetElement().theCoeffs[0];
        this->Denominator.FreeMemory();
        this->expressionType=this->typePoly;
      }
      if (this->Numerator.GetElement().IsEqualToZero())
        this->MakeZero(this->context);
    }
  }
  void ReducePolyToRational()
  { if (this->expressionType==this->typePoly)
      if(this->Numerator.GetElement().IsAConstant())
      { this->expressionType=this->typeRational;
        if (this->Numerator.GetElement().IsEqualToZero())
          this->ratValue.MakeZero();
        else
          this->ratValue=this->Numerator.GetElement().theCoeffs[0];
        this->Numerator.FreeMemory();
      }
//    int commentMEWhenDone;
//    assert(this->checkConsistency());
  }
  bool ConvertToType(int theType);
public:
  friend std::iostream& operator<< (std::iostream& output, const RationalFunctionOld& theRF)
  { output << theRF.ToString();
    return output;
  }
  friend std::iostream& operator>> (std::iostream& input, RationalFunctionOld& theRF);

  MemorySaving<Polynomial<Rational> > Numerator;
  MemorySaving<Polynomial<Rational> > Denominator;
  GlobalVariables* context;
  Rational ratValue;
  int expressionType;
  enum typeExpression{ typeRational=0, typePoly=1, typeRationalFunction=2, typeError=3};
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool NeedsBrackets()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational: return false;
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst().NeedsBrackets();
      case RationalFunctionOld::typeRationalFunction: return false;
    }
    return false;
  }
  bool FindOneVarRatRoots(List<Rational>& output)
  { if (this->expressionType==this->typeRational)
    { output.SetSize(0);
      return true;
    }
    Polynomial<Rational> tempP;
    this->GetNumerator(tempP);
    return tempP.FindOneVarRatRoots(output);
  }
  Rational GetLCMNumeratorRationalDenominators()
  { Polynomial<Rational> Num;
    this->GetNumerator(Num);
    return Num.FindLCMCoefficientDenominatorsOverRationals();
  }
  Rational GetGCDNumeratorsRationalCoeffs()
  { Polynomial<Rational> Num;
    this->GetNumerator(Num);
    return Num.FindGCDCoefficientNumeratorsOverRationals();
  }
  RationalFunctionOld GetOne()const
  { RationalFunctionOld tempRat;
    tempRat.MakeConsT(1, this->context);
    return tempRat;
  }
  RationalFunctionOld GetZero()const
  { RationalFunctionOld tempRat;
    tempRat.MakeZero(this->context);
    return tempRat;
  }
  int GetMinNumVars()const;
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  RationalFunctionOld(const RationalFunctionOld& other): context(0), expressionType(RationalFunctionOld::typeError)
  { this->operator=(other);
  }
  RationalFunctionOld()
  { this->expressionType=this->typeError;
    this->ratValue.MakeZero();
    this->context=0;
  }
  RationalFunctionOld(int other)
  { this->expressionType=this->typeRational;
    this->context=0;
    this->operator=(other);
  }
  RationalFunctionOld(const Rational& other)
  { this->expressionType=this->typeRational;
    this->context=0;
    this->operator=(other);
  }
  Rational RationalValue()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational:  return this->ratValue;
      case RationalFunctionOld::typeError: return 0;
      default:
        return this->Numerator.GetElementConst().GetConstantTerm();
    }
  }
  void RaiseToPower(int thePower);
//  RationalFunctionOld(const RationalFunctionOld& other){this->NumVars=0; this->expressionType=this->typeRational; this->ratValue.MakeZero(); this->operator=(other);}
  void ReduceMemory()
  { this->ReduceRFToPoly();
    this->ReducePolyToRational();
    assert(this->checkConsistency());
  }
  void operator=(const Polynomial<Rational> & other)
  { this->expressionType=this->typePoly;
    this->Numerator.GetElement()=other;
    this->ReduceMemory();
  }
  inline unsigned int HashFunction()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational:
        return this->ratValue.HashFunction();
      case RationalFunctionOld::typePoly:
        return this->Numerator.GetElementConst().HashFunction();
      case RationalFunctionOld::typeRationalFunction:
        return this->Numerator.GetElementConst().HashFunction()*SomeRandomPrimes[0]+
        this->Denominator.GetElementConst().HashFunction()*SomeRandomPrimes[1];
      default:
        return (unsigned)-1;
    }
  }
  static inline unsigned int HashFunction(const RationalFunctionOld& input)
  { return input.HashFunction();
  }
  inline void operator=(int other)
  { this->MakeConsT(other, 0);
  }
  inline void operator=(const Rational& other)
  { this->MakeConsT(other, 0);
  }
  void operator=(const RationalFunctionOld& other);
  bool checkConsistency()const;
  inline void SetNumVariables
  (int GoalNumVars)
  { this->SetNumVariablesSubDeletedVarsByOne(GoalNumVars);
  }
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  void MakeOneLetterMoN
  (int theIndex, const Rational& theCoeff, GlobalVariables& theGlobalVariables,
   int ExpectedNumVars=0)
;
  void GetNumerator(Polynomial<Rational>& output)const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational: output.MakeConsT(this->ratValue); return;
      default: output=this->Numerator.GetElementConst(); return;
    }
  }
  bool IsNegative()
  { if (this->expressionType==this->typeRational)
      return this->ratValue.IsNegative();
    return false;
  }
  void GetDenominator(Polynomial<Rational>& output)const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRationalFunction:
      if (this->Denominator.IsZeroPointer())
      { std::cout << "This is a programming error: the rational function is supposed to be honest, "
        << " but the denominator pointer is zero. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      output=this->Denominator.GetElementConst();
      return;
      default: output.MakeConsT((Rational) 1); return;
    }
  }
  void ClearDenominators
  (RationalFunctionOld& outputWasMultipliedBy)
  ;
  void operator+=(const Polynomial<Rational>& other)
  { RationalFunctionOld tempOther;
    tempOther=other;
    *this+=tempOther;
  }
  void operator+=(const RationalFunctionOld& other);
  inline bool operator==(int other)const
  { if (other==0)
      return this->IsEqualToZero();
    else
      return this->expressionType==this->typeRational && (this->ratValue==other);
  }
  inline bool operator!=(int other)
  { return !(this->operator==(other));
  }
  inline bool operator==(const RationalFunctionOld& other)const
  { return this->IsEqualTo(other);
  }
  void Simplify();
  void SimplifyLeadingCoefficientOnly();
  void operator+=(int theConstant)
  { RationalFunctionOld tempRF;
    tempRF.MakeConsT((Rational) theConstant, this->context);
    (*this)+=tempRF;
  }
  void operator*=(const RationalFunctionOld& other);
  void operator*=(const Polynomial<Rational>& other);
  void operator*=(const MonomialP& other);
  void operator*=(const Rational& other);
  void operator*=(int other){*this*=(Rational)other;}
//  void operator-=(const Rational& theConstant)
//  { RationalFunctionOld tempRF;
//    tempRF.MakeConst(this->NumVars, -theConstant, this->context);
//    (*this)+=tempRF;
//  }
  bool operator<(const RationalFunctionOld& other)const
  { return other>*this;
  }
  bool operator<=(const RationalFunctionOld& other)const
  { if (*this==other)
      return true;
    return other>*this;
  }
  bool operator>(const RationalFunctionOld& other)const
  { if (this->expressionType<other.expressionType)
      return false;
    if (this->expressionType>other.expressionType)
      return true;
    switch(this->expressionType)
    { case RationalFunctionOld::typeRational:  return this->ratValue>other.ratValue;
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst()>other.Numerator.GetElementConst();
      case RationalFunctionOld::typeRationalFunction:
        if (other.Denominator.GetElementConst()>this->Denominator.GetElementConst())
          return true;
        if (this->Denominator.GetElementConst()>other.Denominator.GetElementConst())
          return false;
        return this->Numerator.GetElementConst()>other.Numerator.GetElementConst();
      default: return false;
    }
  }
  inline void TimesConstant(const Rational& theConst){ this->operator*=(theConst);}
  void Invert();
  void MakeOne(GlobalVariables* theContext)
  { this->MakeConsT(1, theContext);
  }
  void MakeZero(GlobalVariables* theContext)
  { this->expressionType=this->typeRational;
    this->ratValue.MakeZero();
    if (theContext!=0)
      this->context=theContext;
    this->Numerator.FreeMemory();
    this->Denominator.FreeMemory();
    assert(this->checkConsistency());
  }
  void MakeConsT(const Rational& theCoeff, GlobalVariables* theContext)
  { this->MakeZero(theContext);
    this->ratValue=theCoeff;
  }
  bool IsInteger()const {return this->expressionType==this->typeRational && this->ratValue.IsInteger();}
  bool IsSmallInteger(int* whichInteger=0)const
  { return this->expressionType==this->typeRational &&
    this->ratValue.IsSmallInteger(whichInteger);
  }
  bool IsEqualToZero()const
  { return this->expressionType==this->typeRational && this->ratValue.IsEqualToZero();
  }
  bool IsEqualToOne()const{return this->expressionType==this->typeRational && this->ratValue.IsEqualToOne();}
  bool IsEqualTo(const RationalFunctionOld& other)const
  { if (this->expressionType!=other.expressionType)
      return false;
    switch(this->expressionType)
    { case RationalFunctionOld::typeRationalFunction:  return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst()) && this->Denominator.GetElementConst().IsEqualTo(other.Denominator.GetElementConst());
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst());
      case RationalFunctionOld::typeRational: return this->ratValue==other.ratValue;
    }
    assert(false);
    return false;
  }
  static void GetRelations
  (List<Polynomial<Rational> >& theGenerators, GlobalVariables& theGlobalVariables)
   ;
  static bool gcdQuicK
  (const Polynomial<Rational>& left, const Polynomial<Rational>& right,
   Polynomial<Rational>& output)
  ;
  static void ScaleClearDenominator
  (List<RationalFunctionOld>& input, Vector<Polynomial<Rational> >& output)
  ;
  static void gcd
  (const Polynomial<Rational>& left, const Polynomial<Rational>& right,
  Polynomial<Rational>& output, GlobalVariables* theGlobalVariables=0);
  static void lcm
  (const Polynomial<Rational>& left, const Polynomial<Rational>& right,
  Polynomial<Rational>& output, GlobalVariables* theGlobalVariables=0);
  inline void operator-=(int other)
  { *this-=(Rational) other;
  }
  inline void operator-=(const Rational& other)
  { assert(this->checkConsistency());
    RationalFunctionOld tempRF;
    tempRF.MakeConsT(other, this->context);
    tempRF.Minus();
    this->operator+=(tempRF);
    assert(this->checkConsistency());
  }
  inline void operator-=(const RationalFunctionOld& other)
  { assert(this->checkConsistency());
    assert(other.checkConsistency());
    if (other.context!=0)
      this->context=other.context;
    RationalFunctionOld tempRF;
    tempRF=other;
    tempRF.Minus();
    this->operator+=(tempRF);
    assert(this->checkConsistency());
  }
  inline void operator/=(int other)
  { RationalFunctionOld tempRF;
    tempRF.MakeConsT(other, this->context);
    *this/=tempRF;
  }
  inline void operator/=(const Polynomial<Rational>& other)
  { RationalFunctionOld tempRF;
    tempRF=other;
    tempRF.Invert();
    *this*=(tempRF);
    assert(this->checkConsistency());
  }
  void operator/=(const RationalFunctionOld& other);
  void Minus(){this->operator*=((Rational) -1); assert(this->checkConsistency());}
};

template <class Element>
bool MonomialP::SubstitutioN
(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, const Element& theRingUnit)
{ MacroRegisterFunctionWithName("MonomialP::Substitution");
  if (TheSubstitution.size<this->GetMinNumVars())
  { std::cout << "This is a programming error. Attempting to carry out "
    << "a substitution in a monomial with " << this->GetMinNumVars()
    << " variables but the substition has "
    << TheSubstitution.size << " variable images. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output.MakeConsT(1);
  if (this->IsAConstant())
    return true;
  Polynomial<Element> tempPoly;
//  std::cout << "<hr>subbing in monomial " << this->ToString();
//  output.ComputeDebugString();
  for (int i=0; i<this->monBody.size; i++)
    if (this->monBody[i]!=0)
    { int theExponent=0;
      if (!this->monBody[i].IsSmallInteger(&theExponent))
      { std::cout << " I cannot carry out a substitution in a monomial "
        << " that has exponent which is not a small integer: it is "
        << this->monBody[i] << " instead. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        return false;
      }
      //TheSubstitution.TheObjects[i]->ComputeDebugString();
      tempPoly=TheSubstitution[i];
      tempPoly.RaiseToPower(theExponent, 1);
//      tempPoly.ComputeDebugString();
      output*=(tempPoly);
//      output.ComputeDebugString();
    }
  return true;
//  std::cout << " to get: " << output.ToString();
}

template <class Element>
int Polynomial<Element>::GetMaxPowerOfVariableIndex(int VariableIndex)
{ int result=0;
  for (int i=0; i<this->size; i++)
  { result= MathRoutines::Maximum(result, this->TheObjects[i][VariableIndex].NumShort);
    if (!this->TheObjects[i][VariableIndex].IsSmallInteger())
    { std::cout << " This is a programming error: GetMaxPowerOfVariableIndex is called on a polynomial whose monomials"
      << " have degrees that are not small integers. This neesd to be fixed! "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  return result;
}

template <class Element>
void Polynomial<Element>::GetConstantTerm(Element& output, const Element& theRingZero)const
{ MonomialP tempM;
  tempM.MakeOne();
  int i=this->GetIndex(tempM);
  if (i==-1)
    output=theRingZero;
  else
    output=this->theCoeffs[i];
}

template <class CoefficientType>
void Polynomial<CoefficientType>::GetCoeffInFrontOfLinearTermVariableIndex
(int index, CoefficientType& output, const CoefficientType& theRingZero)
{ MonomialP tempM;
  tempM.MakeEi(index);
  int i=this->GetIndex(tempM);
  if (i==-1)
    output=theRingZero;
  else
    output=this->theCoeffs[i];
}

template <class CoefficientType>
void Polynomial<CoefficientType>::AddConstant(const CoefficientType& theConst)
{ MonomialP tempMon;
  tempMon.MakeOne();
  this->AddMonomial(tempMon, theConst);
}

template <class TemplateMonomial, class CoefficientType>
inline bool MonomialCollection<TemplateMonomial, CoefficientType>::operator==
(int x)const
{ if (x==0)
    return this->size==0;
  std::cout << "This is either a programming error, or an unforeseen use of operator==. If the second is the case, "
  << "an audit/careful proofreading of the code calling this function is needed; I am crashing just in case. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  return false;
}

template <class TemplateMonomial, class CoefficientType>
inline bool MonomialCollection<TemplateMonomial, CoefficientType>::operator==
(const MonomialCollection<TemplateMonomial, CoefficientType>& other)const
{ if (this->theCoeffs.size!=other.theCoeffs.size)
    return false;
  for (int i=0; i<this->size; i++)
  { int indexInOther=other.GetIndex(this->TheObjects[i]);
    if (indexInOther==-1)
      return false;
    if (!(this->theCoeffs[i]==other.theCoeffs[indexInOther]))
      return false;
  }
  return true;
}

template <class TemplateMonomial, class CoefficientType>
void MonomialCollection<TemplateMonomial, CoefficientType>::WriteToFile
(std::fstream& output)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << " numMons: " << this->size;
  for (int i=0; i<this->size; i++)
  { output << " ";
    if (i>0)
      if (! this->theCoeffs[i].BeginsWithMinus())
        output << "+ ";
    this->theCoeffs[i].WriteToFile(output);
    TemplateMonomial& tempM=this->TheObjects[i];
    tempM.WriteToFile(output);
  }
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class TemplateMonomial, class CoefficientType>
inline bool MonomialCollection<TemplateMonomial, CoefficientType>::ReadFromFile
(std::fstream& input, GlobalVariables* theGlobalVariables)
{ int numReadWords, targetSize;
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string ReaderString;
  bool result=true;
  input >> ReaderString >> targetSize;
  if (ReaderString!="numMons:" )
  { assert(false);
    return false;
  }
  this->MakeZero();
  TemplateMonomial tempM;
  this->SetExpectedSize(targetSize);
  input.ignore();
  CoefficientType theCoeff;
  for (int i=0; i<targetSize; i++)
  { if (input.peek()=='+')
      input >> ReaderString;
//    input.ignore();
    theCoeff.ReadFromFile(input);
    tempM.ReadFromFile(input);
    if (!result)
      break;
    this->AddMonomial(tempM, theCoeff);
  }
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  assert(numReadWords==0);
  return result;
}

template <class CoefficientType>
void Polynomial<CoefficientType>::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ MacroRegisterFunctionWithName("Polynomial_CoefficientType::SetNumVariablesSubDeletedVarsByOne");
  if (newNumVars>=this->GetMinNumVars())
    return;
  if (newNumVars<0)
  { std::cout << "This is a programming error. Requesting negative number of variables"
    << " (more precisely, " << newNumVars << ") is not allowed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Polynomial<CoefficientType> Accum;
  Accum.MakeZero();
  Accum.SetExpectedSize(this->size);
  MonomialP tempM;
  for (int i=0; i<this->size; i++)
  { tempM.MakeOne(newNumVars);
    for (int j=0; j<newNumVars; j++)
      tempM[j]=this->TheObjects[i][j];
    Accum.AddMonomial(tempM, this->theCoeffs[i]);
  }
  this->operator=(Accum);
}

template <class CoefficientType>
void Polynomial<CoefficientType>::ShiftVariableIndicesToTheRight
(int VarIndexShift)
{ if (VarIndexShift<0)
  { std::cout << "This is a programming error. Requesting negative variable shift "
    << "(more precisely, " << VarIndexShift << ") not allowed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (VarIndexShift==0)
    return;
  int oldNumVars=this->GetMinNumVars();
  int newNumVars=oldNumVars+VarIndexShift;
  Polynomial<CoefficientType> Accum;
  Accum.MakeZero();
  Accum.SetExpectedSize(this->size);
  MonomialP tempM;
  for (int i=0; i<this->size; i++)
  { tempM.MakeOne(newNumVars);
    for (int j=0; j<oldNumVars; j++)
      tempM[j+VarIndexShift]=this->TheObjects[i][j];
    Accum.AddMonomial(tempM, this->theCoeffs[i]);
  }
  *this=Accum;
}

template <class Element>
void Polynomial<Element>::Evaluate
(const Vector<Element>& input, Element& output, const Element& theRingZero)
{ output=theRingZero;
  Element tempElt;
  for (int i=0; i<this->size; i++)
  { tempElt=this->theCoeffs[i];
    MonomialP& currentMon=(*this)[i];
    for (int j=0; j<currentMon.GetMinNumVars(); j++)
    { int numCycles;
      if (!this->TheObjects[i][j].IsSmallInteger(&numCycles) )
      { std::cout << "This is a programming error. Attempting to evaluate a polynomial whose"
        <<  i+1 << "^{th} variable is raised to the power "
        << this->TheObjects[i][j].ToString()
        << ". Raising variables to power is allowed only if the power is a small integer. "
        << "If the user has requested such an operation, it"
        << " *must* be intercepted at an earlier level (and the user must be informed)."
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
        return;
      }
      bool isPositive=numCycles>0;
      if (numCycles<0)
        numCycles=-numCycles;
      for (int k=0; k<numCycles; k++)
      { if (isPositive)
          tempElt*=input[j];
        else
          tempElt/=input[j];
        ParallelComputing::SafePointDontCallMeFromDestructors();
      }
    }
    output+=(tempElt);
  }
}

template <class TemplateMonomial, class CoefficientType>
inline void ElementMonomialAlgebra<TemplateMonomial, CoefficientType>::MultiplyBy
(const ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& other,
 ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& output,
 ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& bufferPoly,
 TemplateMonomial& bufferMon)const
{ if (other.IsEqualToZero())
  { output.MakeZero();
    return;
  }
  int maxNumMonsFinal=this->size*other.size;
  bufferPoly.SetExpectedSize(maxNumMonsFinal);
  CoefficientType theCoeff;
  for (int i=0; i<other.size; i++)
    for (int j=0; j<this->size; j++)
    { bufferMon=this->TheObjects[j];
      bufferMon*=(other[i]);
      theCoeff=this->theCoeffs[j];
      theCoeff*=other.theCoeffs[i];
      bufferPoly.AddMonomial(bufferMon, theCoeff);
      ParallelComputing::SafePointDontCallMeFromDestructors();
    }
  output=bufferPoly;
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::IsEqualToZero()const
{ return this->size==0;
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::IsInteger(LargeInt* whichInteger)const
{ if (this->size>1)
    return false;
  if(this->size==0)
  { if (whichInteger!=0)
      *whichInteger=0;
    return true;
  }
  bool result= this->TheObjects[0].IsAConstant();
  if (result)
    result=this->theCoeffs[0].IsInteger(whichInteger);
  return result;
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::IsSmallInteger(int* whichInteger)const
{ if (this->size>1)
    return false;
  if(this->size==0)
  { if (whichInteger!=0)
      *whichInteger=0;
    return true;
  }
  bool result= this->TheObjects[0].IsAConstant();
  if (result)
    result=this->theCoeffs[0].IsSmallInteger(whichInteger);
  return result;
}

template <class Element>
void Polynomial<Element>::MakeLinPolyFromRootLastCoordConst
(const Vector<Rational>& input)
{ this->MakeZero();
  MonomialP tempM;
  for (int i=0; i<input.size-1; i++)
  { tempM.MakeEi(i);
    this->AddMonomial(tempM, input[i]);
  }
  this->operator+=(*input.LastObject());
}

template <class Element>
void Polynomial<Element>::MakeLinPolyFromRootNoConstantTerm(const Vector<Rational>& r)
{ this->MakeZero();
  MonomialP tempM;
  for (int i=0; i<r.size; i++)
  { tempM.MakeEi(i);
    if (!r[i].IsInteger())
    { std::cout << "This may or may not be a programming error: "
      << "requesting a monomial with non-integer exponent. "
      << "It appears this operation should be allowed, "
      << "however this requires further consideration. I am crashing just in case. "
      << " Please debug file "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->AddMonomial(tempM, r[i].GetNumerator());
  }
}

template <class CoefficientType>
void Polynomial<CoefficientType>::MakeMonomiaL
(int LetterIndex, const Rational& Power, const CoefficientType& Coeff,
 int ExpectedNumVars)
{ if (LetterIndex<0 )
  { std::cout << "This is a programming error: the index"
    << LetterIndex+1 << " is  non-positive which is not allowed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int numVars=MathRoutines::Maximum(LetterIndex+1, ExpectedNumVars);
  this->MakeZero();
  MonomialP tempM;
  tempM.MakeOne(numVars);
  tempM[LetterIndex]=Power;
  this->AddMonomial(tempM, Coeff);
}

template <class TemplateMonomial, class CoefficientType>
void MonomialCollection<TemplateMonomial, CoefficientType>::SubtractOtherTimesCoeff
(const MonomialCollection<TemplateMonomial, CoefficientType>& other, CoefficientType* inputcf)
{ if (this==&other)
  { if (inputcf==0)
    { this->MakeZero();
      return;
    }
    MonomialCollection<TemplateMonomial, CoefficientType> otherNew=other;
    this->SubtractOtherTimesCoeff(otherNew, inputcf);
    return;
  }
  this->SetExpectedSize(other.size+this->size);
  CoefficientType tempCF;
  for (int i=0; i<other.size; i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    tempCF=other.theCoeffs[i];
    if (inputcf!=0)
      tempCF*=*inputcf;
    this->SubtractMonomial(other[i], tempCF);
  }
}

template <class TemplateMonomial, class CoefficientType>
void MonomialCollection<TemplateMonomial, CoefficientType>::
operator+=(const MonomialCollection<TemplateMonomial, CoefficientType>& other)
{ this->SetExpectedSize(other.size+this->size);
  for (int i=0; i<other.size; i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    this->AddMonomial(other[i], other.theCoeffs[i]);
  }
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::HasGEQMonomial(TemplateMonomial& m, int& WhichIndex)
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IsGEQpartialOrder(m))
    { WhichIndex=i;
      return true;
    }
  WhichIndex=-1;
  return false;
}

template <class Element>
int Polynomial<Element>::GetIndexMaxMonomialLexicographicLastVariableStrongest()const
{ return this->GetIndexMaxMonomial(MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest);
}

template <class Element>
void Polynomial<Element>::ScaleToPositiveMonomials(MonomialP& outputScale)
{ int numVars=this->GetMinNumVars();
  outputScale.MakeOne(numVars);
  for (int i=0; i<numVars; i++)
    for (int j=0; j<this->size; j++)
      outputScale[i]=MathRoutines::Minimum(outputScale(i), this->TheObjects[j](i));
  outputScale.Invert();
  this->MultiplyBy(outputScale, 1);
}

template <class Element>
bool Polynomial<Element>::IsProportionalTo(const Polynomial<Element>& other, Element& TimesMeEqualsOther, const Element& theRingUnit)const
{ if (this->size!=other.size)
    return false;
  if (other.size==0)
  { TimesMeEqualsOther=theRingUnit;
    return true;
  }
  MonomialP& firstMon= this->TheObjects[0];
  int indexInOther=other.GetIndex(firstMon);
  if (indexInOther==-1)
    return false;
  TimesMeEqualsOther=other.theCoeffs[indexInOther];
  TimesMeEqualsOther/=this->theCoeffs[0];
  Polynomial<Element> tempP;
  tempP=*this;
  tempP*=TimesMeEqualsOther;
  tempP-=other;
  return tempP.IsEqualToZero();
}

template <class Element>
void Polynomial<Element>::DivideBy
(const Polynomial<Element>& inputDivisor, Polynomial<Element>& outputQuotient, Polynomial<Element>& outputRemainder)const
{ MacroRegisterFunctionWithName("Polynomial_Element::DivideBy");
  if (&outputRemainder==this || &outputQuotient==this ||
      &outputRemainder==&inputDivisor || &outputQuotient==&inputDivisor )
  { Polynomial<Element> newQuot, newRemaind;
    this->DivideBy(inputDivisor, newQuot, newRemaind);
    outputQuotient=newQuot;
    outputRemainder=newRemaind;
    return;
  }
  outputRemainder=(*this);
  MonomialP scaleRemainder;
  MonomialP scaleInput;
  Polynomial tempInput;
  tempInput=(inputDivisor);
  outputRemainder.ScaleToPositiveMonomials(scaleRemainder);
  tempInput.ScaleToPositiveMonomials(scaleInput);
  int remainderMaxMonomial=outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int inputMaxMonomial= tempInput.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  outputQuotient.MakeZero();
  if (remainderMaxMonomial==-1)
    return;
  outputQuotient.SetExpectedSize(this->size);
  MonomialP tempMon;
  int numVars=MathRoutines::Maximum(this->GetMinNumVars(), inputDivisor.GetMinNumVars());
  tempMon.MakeOne(numVars);
  Polynomial<Element> tempP;
  tempP.SetExpectedSize(this->size);
  //if (this->flagAnErrorHasOccuredTimeToPanic)
  //{ this->ComputeDebugString();
   // tempInput.ComputeDebugString();
  //}
  assert(remainderMaxMonomial<outputRemainder.size);
  if (inputMaxMonomial>=tempInput.size || inputMaxMonomial<0)
  { std::cout << "This is a programming error: the index of the maximal input monomial is "
    << inputMaxMonomial << " while the polynomial has " << tempInput.size
    << "  monomials. I am attempting to divide "
    << this->ToString() << " by " << inputDivisor.ToString() << ". "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
//  std::cout << "<hr>Dividing " << this->ToString() << " by " << inputDivisor.ToString();
//  std::cout << " comparing " << outputRemainder[remainderMaxMonomial].ToString()
//  << " and " << tempInput[inputMaxMonomial].ToString();
  while (outputRemainder[remainderMaxMonomial].IsGEQLexicographicLastVariableStrongest
        (tempInput[inputMaxMonomial]))
  { assert(remainderMaxMonomial<outputRemainder.size);
    tempMon=outputRemainder[remainderMaxMonomial];
    tempMon/=tempInput[inputMaxMonomial];
    if (!tempMon.HasPositiveOrZeroExponents())
      break;
    Element tempCoeff=outputRemainder.theCoeffs[remainderMaxMonomial];
    tempCoeff/=tempInput.theCoeffs[inputMaxMonomial] ;
    outputQuotient.AddMonomial(tempMon, tempCoeff);
    tempP=(tempInput);
    tempP.MultiplyBy(tempMon, tempCoeff);
/*    std::cout << "<br>hash function tempMon: " <<  tempMon.HashFunction();
    std::cout << "<br>HashFunctions of outputRemainder monomials: ";
    for (int i=0; i<outputRemainder.size; i++)
      std::cout << outputRemainder[i].HashFunction() << ", ";
    std::cout << "<br>subbing " << tempP.ToString() << " from remainder " << outputRemainder.ToString();*/
    outputRemainder-=(tempP);
//    std::cout << " to get " << outputRemainder.ToString();
    remainderMaxMonomial= outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    if (remainderMaxMonomial==-1)
      break;
  }
  scaleInput.Invert();
  outputQuotient.MultiplyBy(scaleInput, 1);
  outputQuotient.MultiplyBy(scaleRemainder, 1);
  outputRemainder.MultiplyBy(scaleRemainder, 1);
//  std::cout << "<br> " << this->ToString() << " divided by " << inputDivisor.ToString() << " yields " << outputQuotient.ToString()
//  << " with remainder " << outputRemainder.ToString();

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

template <class TemplateMonomial, class CoefficientType>
template <class TemplateMonomialCollection>
void MonomialCollection<TemplateMonomial, CoefficientType>::GaussianEliminationByRows
  (List<TemplateMonomialCollection>& theList, bool *IvemadeARowSwitch,
   HashedList<TemplateMonomial>* seedMonomials)
{ MemorySaving<HashedList<TemplateMonomial> > bufferMons;
  HashedList<TemplateMonomial>& allMons = seedMonomials==0 ? bufferMons.GetElement() : *seedMonomials;
  if (seedMonomials==0)
  { int topBoundNumMons=0;
    for (int i =0; i<theList.size; i++)
      topBoundNumMons+=theList[i].size;
    allMons.SetExpectedSize(topBoundNumMons);
  }
  for (int i =0; i<theList.size; i++)
    allMons.AddOnTopNoRepetition(theList[i]);
  allMons.QuickSortDescending();
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
//  std::cout << "<hr>Gaussian elimnation. All mons(" << allMons.size << " total): "
//  << allMons.ToString(&tempFormat);
  tempFormat.flagUseLatex=true;
  if (IvemadeARowSwitch!=0)
    *IvemadeARowSwitch=false;
//  std::cout << "<br><b>starting list:</b> ";
//  for (int i=0; i<theList.size; i++)
//    std::cout << //"<br>" << CGI::GetHtmlMathSpanPure
//    (theList[i].ToString(&tempFormat)) << ", ";
  int currentRowIndex=0;
  CoefficientType tempCF;
  for (int i=0; i<allMons.size && currentRowIndex<theList.size; i++)
  { TemplateMonomial& currentMon=allMons[i];
    int goodRow=currentRowIndex;
    for (; goodRow< theList.size; goodRow++)
      if (theList[goodRow].Contains(currentMon))
        break;
    if (goodRow>=theList.size)
      continue;
    if (currentRowIndex!=goodRow)
    { theList.SwapTwoIndices(currentRowIndex, goodRow);
      if (IvemadeARowSwitch!=0)
        *IvemadeARowSwitch=true;
    }
    MonomialCollection<TemplateMonomial, CoefficientType>& currentPivot=
    theList[currentRowIndex];
    int colIndex=currentPivot.GetIndex(currentMon);
    if (colIndex==-1)
    { std::cout << "This is a programming error. "
      << "An internal check at the Gaussian elimination method for "
      << " monomial collections fails. Something is wrong. Here is the List you wanted "
      << " to perform Gaussian elimination upon. "
      << theList.ToString()
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    tempCF=currentPivot.theCoeffs[colIndex];
    currentPivot/=tempCF;
    for (int j=0; j<theList.size; j++)
      if (j!=currentRowIndex)
      { MonomialCollection<TemplateMonomial, CoefficientType>& currentOther=theList[j];
        int otherColIndex=currentOther.GetIndex(currentMon);
        if (otherColIndex!=-1)
        { tempCF=currentOther.theCoeffs[otherColIndex];
          //std::cout << "<br>subtracting " << CGI::GetHtmlMathSpanPure(currentPivot.ToString())
            //<< " times " << tempCF.ToString() << " from "
            //<< CGI::GetHtmlMathSpanPure(currentOther.ToString());
          currentOther.SubtractOtherTimesCoeff(currentPivot, &tempCF);
          //std::cout << "<br>to get " << CGI::GetHtmlMathSpanPure(currentOther.ToString());
        }
      }
    currentRowIndex++;
  }
//    std::cout << "<br><b>final list:</b> ";
//  for (int i=0; i<theList.size; i++)
//    std::cout << //"<br>" << CGI::GetHtmlMathSpanPure
//    (theList[i].ToString(&tempFormat)) << ", ";
}

template <class TemplateMonomial, class CoefficientType>
int MonomialCollection<TemplateMonomial, CoefficientType>::AddMonomialNoCoeffCleanUpReturnsCoeffIndex
(const TemplateMonomial& inputMon, const CoefficientType& inputCoeff)
{ ///
//  this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
  ///
  if (inputCoeff==0 || inputMon.IsEqualToZero())
    return -1;
  int j= this->GetIndex(inputMon);
  if (j>=this->size)
  { std::cout << "This is a programming error: function GetIndex "
    << " evaluated on " << inputMon << " with hash function " << inputMon.HashFunction(inputMon)
    << " returns index " << j
    << " but I have only " << this->size << " elements "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (j==-1)
  { this->::HashedList<TemplateMonomial>::AddOnTop(inputMon);
    this->theCoeffs.AddOnTop(inputCoeff);
    j=this->size-1;
  } else
  { ///
//    this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
    if (j>=this->theCoeffs.size)
    { std::cout << "This is a programming error. "
      << " Looking for coefficient index " << j << " when number of coefficients is "
      << this->theCoeffs.size <<  ". " <<  CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    ///
    this->theCoeffs[j]+=inputCoeff;
  }
  return j;
}

template <class TemplateMonomial, class CoefficientType>
int MonomialCollection<TemplateMonomial, CoefficientType>::
SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex
(const TemplateMonomial& inputMon, const CoefficientType& inputCoeff)
{ if (inputCoeff.IsEqualToZero() || inputMon.IsEqualToZero())
    return -1;
  int j= this->GetIndex(inputMon);
  if (j==-1)
  { this->::HashedList<TemplateMonomial>::AddOnTop(inputMon);
    this->theCoeffs.AddOnTop(inputCoeff);
    *this->theCoeffs.LastObject()*=-1;
    j=this->theCoeffs.size-1;
  } else
    this->theCoeffs[j]-=inputCoeff;
  return j;
}

template <class TemplateMonomial, class CoefficientType>
void ElementMonomialAlgebra<TemplateMonomial, CoefficientType>::RaiseToPower
(int d, ElementMonomialAlgebra<TemplateMonomial, CoefficientType>& output, const CoefficientType& theRingUniT)
{ ElementMonomialAlgebra<TemplateMonomial, CoefficientType> tempOne;
  tempOne.MakeConst(theRingUniT);
  output=*this;
  MathRoutines::RaiseToPower(output, d, tempOne);
}

template <class Element>
Rational Polynomial<Element>::TotalDegree()const
{ Rational result=0;
  for (int i=0; i<this->size; i++)
    result=MathRoutines::Maximum(this->TheObjects[i].TotalDegree(), result);
  return result;
}

template <class CoefficientType>
bool Polynomial<CoefficientType>::SubstitutioN
(const List<Polynomial<CoefficientType> >& TheSubstitution, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ MacroRegisterFunctionWithName("Polynomial<CoefficientType>::Substitution");
  if (TheSubstitution.size<this->GetMinNumVars())
  { std::cout << "This is a programming error: attempting to carry out a substitution"
    << "in a polynomial of " << this->GetMinNumVars() << " variables while specifying the images of only "
    << TheSubstitution.size << " of the variables. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Polynomial<CoefficientType> Accum, TempPoly;
//  int commentGrandMasterCheckWhenDone;
//  this->GrandMasterConsistencyCheck();
  for(int i=0; i<this->size; i++)
  { if(!this->TheObjects[i].SubstitutioN(TheSubstitution, TempPoly, theRingUnit))
      return false;
    TempPoly*=this->theCoeffs[i];
    Accum+=(TempPoly);
    //std::cout << "<br>So far accum is :<br> " << Accum.ToString(theFormat);
  }
  *this=(Accum);
//  this->GrandMasterConsistencyCheck();
  //std::cout << "<hr>to finally get<br>" << output.ToString(theFormat);
  return true;
}

template <class Element>
void Polynomial<Element>::MakeOne(int ExpectedNumVars)
{ this->MakeConsT(1, ExpectedNumVars);
}

template <class Element>
void Polynomial<Element>::MakeDegreeOne(int NVar, int NonZeroIndex, const Element& coeff)
{ this->MakeZero();
  MonomialP tempM;
  tempM.MakeEi(NonZeroIndex, 1, NVar);
  this->AddMonomial(tempM, coeff);
}

template <class Element>
void Polynomial<Element>::MakeDegreeOne(int NVar, int NonZeroIndex1, int NonZeroIndex2, const Element& coeff1, const Element& coeff2)
{ this->MakeZero();
  MonomialP tempM;
  tempM.MakeEi(NonZeroIndex1);
  this->AddMonomial(tempM, coeff1);
  tempM.MakeEi(NonZeroIndex2);
  this->AddMonomial(tempM, coeff2);
}

template <class Element>
void Polynomial<Element>::MakeDegreeOne(int NVar, int NonZeroIndex, const Element& coeff1, const Element& ConstantTerm)
{ this->MakeDegreeOne(NVar, NonZeroIndex, coeff1);
  *this+=ConstantTerm;
}

template <class Element>
void PolynomialSubstitution<Element>::PrintPolys(std::string &output, Element& TheRingUnit, Element& TheRingZero)
{ std::stringstream out;
  FormatExpressions PolyFormatLocal;
  for (int i=0; i<this->size; i++)
  { std::string tempS;
    out << i << ". ";
    this->TheObjects[i]->StringPrintOutAppend(tempS, PolyFormatLocal);
    out << tempS << "\r\n";
  }
  output=out.str();
}

template <class Element>
void PolynomialSubstitution<Element>::MakeIdSubstitution(int numVars, const Element& theRingUnit)
{ this->MakeIdLikeInjectionSub(numVars, numVars, theRingUnit);
}

template <class Element>
void PolynomialSubstitution<Element>::
MakeIdLikeInjectionSub
(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const Element& theRingUnit)
{ assert(numStartingVars<=numTargetVarsMustBeLargerOrEqual);
  this->SetSize(numStartingVars);
  for (int i=0; i<this->size; i++)
  { Polynomial<Element>& currentPoly=this->TheObjects[i];
    currentPoly.MakeDegreeOne(numTargetVarsMustBeLargerOrEqual, i, theRingUnit);
  }
}

template <class Element>
void PolynomialSubstitution<Element>::MakeExponentSubstitution(Matrix<LargeInt>& theSub)
{ Polynomial<Element> tempP;
  MonomialP tempM;
  tempM.MakeOne(theSub.NumRows);
  this->size=0;
  this->SetSize(theSub.NumCols);
  for (int i=0; i<theSub.NumCols; i++)
  { for (int j=0; j<theSub.NumRows; j++)
      tempM[j]= (Rational) theSub(j,i);
    tempP.MakeZero();
    tempP.AddMonomial(tempM, 1);
//    tempP->ComputeDebugString();
    this->TheObjects[i]=tempP;
  }
}

template <class Element>
void PolynomialSubstitution<Element>::MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<Element>& EndPoint)
{ this->SetSize(numVars);
  for (int i=0; i<numVars-1; i++)
    this->TheObjects[i].MakeDegreeOne(numVars, i, Element::TheRingUnit);
  this->TheObjects[numVars-1].CopyFromPoly(EndPoint);
}

class oneFracWithMultiplicitiesAndElongations
{
public:
  ListLight<int> Multiplicities;
  ListLight<int> Elongations;
  void AddMultiplicity(int MultiplicityIncrement, int Elongation);
  int IndexLargestElongation();
  int GetLargestElongation();
  void GetPolyDenominator(Polynomial<LargeInt>& output, int MultiplicityIndex, Vector<Rational>& theExponent);
  int GetMultiplicityLargestElongation();
  int GetLCMElongations();
  int GetTotalMultiplicity() const;
  void invert();
  void init();
  static Vector<Rational> GetCheckSumRoot(int NumVars);
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const oneFracWithMultiplicitiesAndElongations& input)
  { return input.HashFunction();
  }
  void ComputeOneCheckSum(Rational& output, Vector<Rational>& theExp, int theDimension);
  bool IsHigherThan(oneFracWithMultiplicitiesAndElongations& f);
  void operator=(oneFracWithMultiplicitiesAndElongations& right);
  bool operator==(oneFracWithMultiplicitiesAndElongations& right);
  std::string ToString(int index, bool LatexFormat);
  void OneFracToStringBasisChange
  (partFractions& owner, int indexElongation, Matrix<LargeInt>& VarChange, bool UsingVarChange,
   std::string& output, bool LatexFormat, int indexInFraction, int theDimension,
   FormatExpressions& PolyFormatLocal);
};

class ElementWeylGroup: public List<int>
{
public:
  void ToString(std::string& output)
  { output=this->ToString();
  }
  std::string ToString
  (FormatExpressions* theFormat=0,  List<int>* DisplayIndicesOfSimpleRoots=0)
  { return this->ToString(-1, theFormat,  DisplayIndicesOfSimpleRoots);
  }
  std::string ToString
  (int NumSimpleGens, FormatExpressions* theFormat=0, List<int>* DisplayIndicesOfSimpleRoots=0)
  ;
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const ElementWeylGroup& input)
  { return input.HashFunction();
  }
  void operator=(const ElementWeylGroup& right);
  bool operator==(const ElementWeylGroup& right);
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

class permutation: public SelectionWithDifferentMaxMultiplicities
{
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int TotalNumElements);
  void incrementAndGetPermutation(List<int>& output);
  void GetPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int GetNumPermutations() {return this->getTotalNumSubsets();}
};

struct IndicatorWindowVariables
{
public:
  bool Busy;
  int NumProcessedMonomialsCurrentFraction;
  int NumProcessedMonomialsTotal;
  Vector<Rational> modifiedRoot;
  bool flagRootIsModified;
  bool Pause;
  IndicatorWindowVariables(){this->init(); }
  List<std::string> ProgressReportStringS;
  void Assign(IndicatorWindowVariables& right);
  void init()
  { this->Busy=false;
    this->Pause=true;
    this->ProgressReportStringS.SetSize(0);
    this->NumProcessedMonomialsCurrentFraction=0;
    this->NumProcessedMonomialsTotal=0;
    this->modifiedRoot.MakeZero(1);
  }
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
  void init(const Vector<Rational> & input1, const Vector<Rational> & input2, unsigned long PenStyle, int colorIndex)
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
  void init(const Vector<Rational> & input, const std::string& inputText, int colorIndex, int theFontSize, int theTextStyle)
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
  void init(const Vector<Rational> & input, double theRadius, unsigned long thePenStylE, int colorIndex)
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
  (Matrix<Rational>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
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
  void drawLineBetweenTwoVectorsBuffer(const Vector<Rational> & vector1, const Vector<Rational> & vector2, unsigned long thePenStyle, int ColorIndex);
  void drawTextAtVectorBuffer(const Vector<Rational> & input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawCircleAtVectorBuffer(const Vector<Rational> & input, double radius, unsigned long thePenStyle, int theColor);
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
  { this->IndexNthDrawOperation.ReservE(10000);
    this->TypeNthDrawOperation.ReservE(10000);
    this->theDrawLineBetweenTwoRootsOperations.ReservE(10000);
    this->theDrawTextAtVectorOperations.ReservE(15);
    this->theDrawCircleAtVectorOperations.ReservE(280);
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
  std::string ToString();
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
//the following items are a part of a dirty hack i need to do for an
//under-stress presentation. Please do not use them !
//they need a rewrite!
  std::string GetHtmlFromDrawOperationsCreateDivWithUniqueName
  (int theDimension)
  ;
  List<DrawingVariables> theFrames;
  DrawOperations theBuffer;
////////////////////////////////////////end of dirty hack
  DrawOperations stillFrame;
  List<DrawOperations> thePhysicalDrawOps;
  List<VirtualDrawOp> theVirtualOpS;
  bool flagAnimating;
  bool flagIsPausedWhileAnimating;
  int indexVirtualOp;
  std::string ToString();
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
  bool flagFillUserDefinedProjection;
  Vectors<Rational> FillUserDefinedProjection;
  int Selected;
  static int NumHtmlGraphics;
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
  static void GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational> & r, double& x, double& y);
  static void ProjectOnToHyperPlaneGraphics(Vector<Rational> & input, Vector<Rational> & output, Vectors<Rational>& directions);
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
  inline void drawLineBetweenTwoVectorsBuffer(const Vector<Rational> & r1, const Vector<Rational> & r2, int PenStyle, int PenColor){this->theBuffer.drawLineBetweenTwoVectorsBuffer(r1, r2, PenStyle, PenColor);}
  void drawTextAtVectorBuffer(const Vector<Rational> & point, const std::string& inputText, int textColor, int theTextStyle, std::fstream* LatexOutFile);
  void drawCircleAtVectorBuffer
  (const Vector<Rational> & point, double radius, unsigned long thePenStyle, int theColor)
;
  void operator=(const DrawingVariables& other)
  { this->theDrawLineFunction=other.theDrawLineFunction;
    this->theDrawTextFunction=other.theDrawTextFunction;
  }
};

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

template <class CoefficientType>
bool Vectors<CoefficientType>::LinSpanContainsRoot(const Vector<CoefficientType>& input, Matrix<CoefficientType>& bufferMatrix, Selection& bufferSelection)const
{ Vectors<CoefficientType> tempVectors;
  tempVectors=(*this);
  tempVectors.AddOnTop(input);
//  this->ComputeDebugString();
//  tempRoots.ComputeDebugString();
//  input.ComputeDebugString();
  return this->GetRankOfSpanOfElements(&bufferMatrix, &bufferSelection)==tempVectors.GetRankOfSpanOfElements(&bufferMatrix, &bufferSelection);
}

template <class CoefficientType>
int Vectors<CoefficientType>::GetRankOfSpanOfElements(Matrix<CoefficientType>* buffer, Selection* bufferSelection)const
{ if (this->size==0)
    return 0;
  int theDimension= this->TheObjects[0].size;
  MemorySaving<Matrix<CoefficientType> > emergencyMatBuf;
  MemorySaving<Selection> emergencySelBuf;
  if (buffer==0)
    buffer=&emergencyMatBuf.GetElement();
  if (bufferSelection==0)
    bufferSelection=&emergencySelBuf.GetElement();
  this->GaussianEliminationForNormalComputation(*buffer, *bufferSelection, theDimension);
  return (theDimension-bufferSelection->CardinalitySelection);
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
std::string Vectors<CoefficientType>::ElementsToInequalitiesString
(bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat)
{ std::stringstream out;
  std::string tempS;
  std::string theLetter="x";
  if (useLatex)
    out << "\\begin{array}{l}";
  for (int i=0; i<this->size; i++)
  { Vector<Rational>& current=this->TheObjects[i];
    tempS= current.ToStringLetterFormat(theFormat.polyDefaultLetter, &theFormat, LastVarIsConstant);
    if (tempS=="")
      out << "(0";
    out << tempS;
    if (!LastVarIsConstant)
    { if (useLatex)
        out << "\\geq 0\\\\";
      else
        out << "=>0\n";
    } else
    { if (useLatex)
        out << "\\geq " << (-(*current.LastObject())).ToString() << "\\\\";
      else
        out << "=>" <<  (-(*current.LastObject())).ToString();
    }
    if (tempS=="")
      out << ")";
    if (useHtml)
      out << "<br>";
    else
      out << "\n";
  }
  if (useLatex)
    out << "\\end{array}";
  return out.str();
}

template <class CoefficientType>
bool Vectors<CoefficientType>::GetCoordsInBasis
(const Vectors<CoefficientType>& inputBasis, Vectors<CoefficientType>& outputCoords,
 Vectors<CoefficientType>& bufferVectors, Matrix<CoefficientType>& bufferMat,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ assert(this!=&outputCoords);
  outputCoords.SetSize(this->size);
  for(int i=0; i<this->size; i++)
    if (!(*this)[i].GetCoordsInBasiS
    (inputBasis, outputCoords[i], bufferVectors, bufferMat, theRingUnit, theRingZero))
      return false;
  return true;
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
  //std::cout << "<br> the matrix before integral gaussian elimination: " << bufferMatGaussianElimination.ToString(true, false) << " and the other matrix: " << bufferMatGaussianEliminationCC.ToString(true, false);
  bufferMatGaussianElimination.GaussianEliminationEuclideanDomain(&bufferMatGaussianEliminationCC, theRingMinusUnit, theRingUnit);
  //std::cout << "<br> the matrix after integral gaussian elimination: " << bufferMatGaussianElimination.ToString(true, false) << " and the other matrix: " << bufferMatGaussianEliminationCC.ToString(true, false);
  Vector<CoefficientType> tempRoot, theCombination;
  assert(this!=&output);
  output.MakeZero(inputBasis.size, theRingZero);
  theCombination=*this;
  int col=0;
//  std::cout << "<br>vector whose coords we wanna find: " << this->ToString();
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
//  std::cout << "<br>" << bufferMatGaussianEliminationCC.ToString(true, false) << " acting on " << output.ToString();
  bufferMatGaussianEliminationCC.ActMultiplyVectorRowOnTheRight(output, theRingZero);
//  std::cout << " gives " << output.ToString();
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
//  std::cout << "<br>input for GetLinearDependence: " << bufferVectors.ToString();
  if(!bufferVectors.GetLinearDependence(bufferMat, theRingUnit, theRingZero))
    return false;
  //std::cout << "<br>output for GetLinearDependence: "<< bufferMat.ToString();
//  tempRoots.ComputeDebugString();
//  tempMat.ComputeDebugString();
  CoefficientType tempCF=bufferMat.elements[bufferMat.NumRows-1][0];
  bufferMat/=tempCF;
  output.SetSize(bufferMat.NumRows-1);
  for (int i=0; i<bufferMat.NumRows-1; i++)
  { bufferMat.elements[i][0].Minus();
    output[i]=(bufferMat.elements[i][0]);
  }
//  std::cout << "outpuf final: " << output.ToString();
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
      outputTheSystem.elements[j][i]=(this->TheObjects[i][j]);
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
  //std::cout << tempMat.ToString(true, false);
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

class GlobalVariables
{
  //I love doxygen!
  /// @cond
private:
  FeedDataToIndicatorWindow FeedDataToIndicatorWindowDefault;
  double (*getElapsedTimePrivate)();
  void (*callSystem)(const std::string& theSystemCommand);
public:
  double MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  FormatExpressions theDefaultFormat;
//progress report flags:
  bool flagGaussianEliminationProgressReport;

  IndicatorWindowVariables theIndicatorVariables;
  DrawingVariables theDrawingVariables;
  FormatExpressions thePolyFormat;
  Controller theLocalPauseController;

  //buffers:
  MemorySaving<Selection> selWallSelection;
  MemorySaving<Selection> selComputeNormalExcludingIndex;
  MemorySaving<Selection> selApproveSelAgainstOneGenerator;
  MemorySaving<Selection> selSimplexAlg2;
  MemorySaving<Selection> selGetNewVerticesAppend;
  MemorySaving<Selection> selGetNewVerticesAppend2;
  MemorySaving<Selection> selSplitChamber;
  MemorySaving<Selection> selEliminateFakeNormalsUsingVertices;
  MemorySaving<Selection> selCreateFromVertices;

  MemorySaving<HashedList<Selection> > hashedSelSimplexAlg;

  MemorySaving<HashedList<Vector<Rational> > > hashedRootsNewChamberSplit;

  MemorySaving<Matrix<Rational> > matIdMatrix;
  MemorySaving<Matrix<Rational> > matOneColumn;
  MemorySaving<Matrix<Rational> > matReduceMonomialByMonomial2;
  MemorySaving<Matrix<Rational> > matReduceMonomialByMonomial;
  MemorySaving<Matrix<Rational> > matConeCondition1;
  MemorySaving<Matrix<Rational> > matConeCondition2;
  MemorySaving<Matrix<Rational> > matConeCondition3;
  MemorySaving<Matrix<Rational> > matComputeEpsCoordsWRTk;
  MemorySaving<Matrix<Rational> > matSimplexAlgorithm1;
  MemorySaving<Matrix<Rational> > matSimplexAlgorithm2;
  MemorySaving<Matrix<Rational> > matSimplexAlgorithm3;
  MemorySaving<Matrix<Rational> > matGetNewVerticesAppend;
  MemorySaving<Matrix<Rational> > matSplitChamberBuff;
  MemorySaving<Matrix<Rational> > matComputeNormalFromSelection;
  MemorySaving<Matrix<Rational> > matEliminateFakeNormalsUsingVertices;
  MemorySaving<Matrix<Rational> > matCreateFromVertices;


  MemorySaving<Polynomial<LargeInt> > PolyLargeIntPartFracBuffer5;
  MemorySaving<Polynomial<LargeInt> > PolyLargeIntPartFracBuffer6;

  MemorySaving<Polynomial<Rational> > RFgcdBuffer1;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer2;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer3;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer4;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer5;

  MemorySaving<List<Polynomial<Rational> > > RFgcdBufferList1;

  MemorySaving<MonomialP> RFgcdBuferMon1;
  MemorySaving<MonomialP> RFgcdBuferMon2;

  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;

  MemorySaving<ReflectionSubgroupWeylGroup> subGroupActionNormalizerCentralizer;

  MemorySaving<DynkinDiagramRootSubalgebra > dynAttemptTheTripleTrick;

  MemorySaving<Vectors<Rational> > rootsNilradicalRoots;
  MemorySaving<Vectors<Rational> > rootsConeConditionHolds2;
  MemorySaving<Vectors<Rational> > rootsAttemptTheTripleTrick;
  MemorySaving<Vectors<Rational> > rootsAttepmtTheTripleTrickWRTSA;
  MemorySaving<Vectors<Rational> > rootsComputeEpsCoordsWRTk2;
  MemorySaving<Vectors<Rational> > rootsComputeEpsCoordsWRTk;
  MemorySaving<Vectors<Rational> > rootsGetEpsilonCoords;
  MemorySaving<Vectors<Rational> > rootsEliminateFakeNormalsUsingVertices;
  MemorySaving<Vectors<Rational> > rootsCreateFromVertices;
  MemorySaving<Vectors<Rational> > rootsConeWriteToFileBuffer;

  MemorySaving<HashedList<Vector<Rational> > > hashedRootsComputeSubGroupFromGeneratingReflections;

  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso1;
  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso2;
  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso3;
  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso4;

  MemorySaving<rootSubalgebra> rootSAProverIsos;
  MemorySaving<rootSubalgebra> rootSAAttemptExtensionToIso1;
  MemorySaving<rootSubalgebra> rootSAAttemptExtensionToIso2;
  MemorySaving<rootSubalgebras> rootSAAttemptExtensionIso1;
  MemorySaving<rootSubalgebras> rootSAAttemptExtensionIso2;
  MemorySaving<rootSubalgebras> rootSAsGenerateAll;
  MemorySaving<GroebnerBasisComputation> theGroebnerBasisComputation;

  MemorySaving<Cone> coneBuffer1NewSplit;
  MemorySaving<Cone> coneBuffer2NewSplit;

  GlobalVariables();
  void SetTimerFunction(double (*timerFunction)())
  { this->getElapsedTimePrivate=timerFunction;
  }
  double GetElapsedSeconds()
  { if (this->getElapsedTimePrivate!=0)
      return this->getElapsedTimePrivate();
    return -1;
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
  inline void MakeReport()
  { if (this->FeedDataToIndicatorWindowDefault!=0)
      this->FeedDataToIndicatorWindowDefault(this->theIndicatorVariables);
  }
  /// @endcond
};

class ProgressReport
{
public:
  GlobalVariables* pointerGV;
  int currentLevel;
  void Report(const std::string& theReport)
  { if (this->pointerGV==0)
      return;
    this->pointerGV->theIndicatorVariables.ProgressReportStringS[currentLevel]=theReport;
    this->pointerGV->MakeReport();
  }
  void initFromGV(GlobalVariables* theGlobalVariables)
  { this->pointerGV=theGlobalVariables;
    if (this->pointerGV==0)
      return;
    currentLevel=this->pointerGV->theIndicatorVariables.ProgressReportStringS.size;
    this->pointerGV->theIndicatorVariables.ProgressReportStringS.SetSize
    (this->pointerGV->theIndicatorVariables.ProgressReportStringS.size+1);
    *this->pointerGV->theIndicatorVariables.ProgressReportStringS.LastObject()="";
  }
  ProgressReport(GlobalVariables* theGlobalVariables)
  { this->initFromGV(theGlobalVariables);
  }
  ProgressReport(GlobalVariables* theGlobalVariables, const std::string& theReport)
  { this->initFromGV(theGlobalVariables);
    this->Report(theReport);
  }
  ~ProgressReport()
  { if (this->pointerGV==0)
      return;
    pointerGV->theIndicatorVariables.ProgressReportStringS.size--;
  }
};

template <class Element>
void Matrix<Element>::GaussianEliminationEuclideanDomain
(Matrix<Element>* otherMatrix, const Element& theRingMinusUnit, const Element& theRingUnit,
 bool (*comparisonGEQFunction) (const Element& left, const Element& right),
 GlobalVariables* theGlobalVariables
 )
{ MacroRegisterFunctionWithName("Matrix_Element::GaussianEliminationEuclideanDomain");
  ProgressReport theReport(theGlobalVariables);
  if (otherMatrix==this)
  { std::cout << "This is a programming error: the Carbon copy in the Gaussian elimination "
    << " coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int col=0;
  Element tempElt;
  int row=0;
  while(row<this->NumRows && col<this->NumCols)
  { //std::cout << "<br>****************row: " << row << " status: " << this->ToString(true, false);
    int foundPivotRow=-1;
    for (int i=row; i<this->NumRows; i++)
      if(!this->elements[i][col].IsEqualToZero())
      { foundPivotRow=i;
        break;
      }
    if (foundPivotRow!=-1)
    { /*if (this->elements[foundPivotRow][col].IsEqualToZero())
      { std::cout << "This is a programming error. "
        << "Something is very wrong: I am getting 0 for a pivot element in "
        << "Gaussian elimination over Euclidean domains. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }*/
      this->SwitchTwoRowsWithCarbonCopy(row, foundPivotRow, otherMatrix);
      /*if (this->elements[row][col].IsEqualToZero())
      { std::cout << "This is a programming error. "
        << "Something is very wrong: I am getting 0 for a pivot element in "
        << "Gaussian elimination over Euclidean domains. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }*/
      if (this->elements[row][col].IsNegative())
        this->RowTimesScalarWithCarbonCopy(row, theRingMinusUnit, otherMatrix);
      int ExploringRow= row+1;
//      std::cout << "<br>before second while: " << this->ToString(true, false);
      while (ExploringRow<this->NumRows)
      { if (theGlobalVariables!=0)
        { std::stringstream out;
          out << "Pivotting on row of index " << row +1 << " with exploring row of index "
          << ExploringRow+1 << "; total rows: " << this->NumRows;
          theReport.Report(out.str());
        }
        Element& PivotElt=this->elements[row][col];
        Element& otherElt=this->elements[ExploringRow][col];
        /*if (PivotElt.IsEqualToZero())
        { std::cout << "This is a programming error. "
          << "Something is very wrong: I am getting 0 for a pivot element in "
          << "Gaussian elimination over Euclidean domains. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }*/
        if (otherElt.IsNegative())
          this->RowTimesScalarWithCarbonCopy(ExploringRow, theRingMinusUnit, otherMatrix);
        bool isSmallerOrEqualTo= comparisonGEQFunction==0 ? PivotElt<=otherElt :
        comparisonGEQFunction(otherElt, PivotElt);
        if (isSmallerOrEqualTo)
        { tempElt=otherElt;
          tempElt/=PivotElt;
          tempElt.AssignFloor();
          this->SubtractRowsWithCarbonCopy(ExploringRow, row, 0, tempElt, otherMatrix);
        }
        if (this->elements[ExploringRow][col].IsEqualToZero())
          ExploringRow++;
        else
          this->SwitchTwoRowsWithCarbonCopy(ExploringRow, row, otherMatrix);
//        std::cout << "<br>second while cycle end: " << this->ToString(true, false);
      }
      Element& PivotElt = this->elements[row][col];
      for (int i=0; i<row; i++)
      { tempElt =this->elements[i][col];
        tempElt/=PivotElt;
//        std::cout << " the floor of " << tempElt.ToString();
        tempElt.AssignFloor();
//        std::cout << " is " << tempElt.ToString();
        this->SubtractRowsWithCarbonCopy(i, row, 0, tempElt, otherMatrix);
        if (this->elements[i][col].IsNegative())
          this->AddTwoRowsWithCarbonCopy(row, i, 0, theRingUnit, otherMatrix);
      }
      row++;
    }
    col++;
//    std::cout << "end of cycle status: " << this->ToString(true, false) << "<br>****************";
  }
}

template <class CoefficientType>
void Vectors<CoefficientType>::SelectABasis
  (Vectors<CoefficientType>& output, const CoefficientType& theRingZero, Selection& outputSelectedIndices, GlobalVariables& theGlobalVariables)const
{ assert(this!=&output);
  ProgressReport theReport(&theGlobalVariables);
  if (this->size==0)
    return;
  int theDim=this->TheObjects[0].size;
  output.ReservE(this->size);
  output.size=0;
  Matrix<CoefficientType> theMat, matEmpty;
  theMat.init(1, theDim);
  int currentRow=0;
  Selection theSel;
  outputSelectedIndices.init(this->size);
  for (int i=0; i<this->size; i++)
  { if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Selecting a basis in dimension " << theDim << ", processed " << i
      << " out of " << this->size;
      theReport.Report(out.str());
    }
    for (int j=0; j<theDim; j++)
      theMat.elements[currentRow][j]=(*this)[i][j];
    theMat.GaussianEliminationByRows(theMat, matEmpty, theSel);
    if (currentRow<theDim-theSel.CardinalitySelection)
    { output.AddOnTop(this->TheObjects[i]);
      outputSelectedIndices.AddSelectionAppendNewIndex(i);
      theMat.Resize(currentRow+2, theDim, true, &theRingZero);
    }
    currentRow=theDim-theSel.CardinalitySelection;
    if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Selecting a basis in dimension " << theDim << ", processed " << i << " out of "
      << this->size;
      theReport.Report(out.str());
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
        ProgressReport tempReport(theGlobalVariables, report.str());
      }
      theGlobalVariables->theLocalPauseController.SafePointDontCallMeFromDestructors();
    }
    //</reporting_and_safepoint_duties>
  }
  bool tempBool= XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsBeforeTag, this->GetXMLClassName());
  assert(tempBool);
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
  std::string ToString()
  { std::stringstream tempStream;
    tempStream << *this;
    return tempStream.str();
  }
  void ToString(std::string& output){ output=this->ToString(); }
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

template<class CoefficientType>
class ElementSemisimpleLieAlgebra :public MonomialCollection<ChevalleyGenerator, CoefficientType>
{
public:
  bool CheckConsistency()const
  { if (this->size==0)
      return true;
    SemisimpleLieAlgebra* owner= (*this)[0].owneR;
    for (int i=1; i<this->size; i++)
      if (owner!=(*this)[i].owneR)
      { std::cout << "This is a programming error: ElementSemisimpleLieAlgebra "
        << " contains Chevalley generators with different owners. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    return true;
  }
  bool NeedsBrackets()const;
  Vector<CoefficientType> GetCartanPart()const;
  void MakeGGenerator
  (const Vector<Rational>& theRoot, SemisimpleLieAlgebra& inputOwner)
  ;
  bool IsElementCartan()
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i].IsNilpotent())
        return false;
    return true;
  }
  void MakeHgenerator(const Vector<CoefficientType>& theH, SemisimpleLieAlgebra& inputOwners);
  void MakeGenerator
  (int generatorIndex, SemisimpleLieAlgebra& inputOwner)
  ;
  void ElementToVectorNegativeRootSpacesFirst(Vector<Rational>& output)const;
  void AssignVectorNegRootSpacesCartanPosRootSpaces
(const Vector<Rational>& input, SemisimpleLieAlgebra& owner)
  ;
  bool GetCoordsInBasis
  (const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<Rational>& output, GlobalVariables& theGlobalVariables)const
;
  SemisimpleLieAlgebra* GetOwner()const
  { this->CheckConsistency();
    if (this->size==0)
      return 0;
    return (*this)[0].owneR;
  }
  bool GetCoordsInBasis
  (const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)const
  { Vector<Rational> tempVect;
    if (! this->GetCoordsInBasis(theBasis, tempVect, theGlobalVariables))
      return false;
    output.SetSize(tempVect.size);
    return true;
  }
  void MultiplyByRational(SemisimpleLieAlgebra& owner, const Rational& theNumber);
  static void GetBasisFromSpanOfElements
  (List<ElementSemisimpleLieAlgebra>& theElements, Vectors<RationalFunctionOld>& outputCoords, List<ElementSemisimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables)
    ;
  void ActOnMe
  (const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner)
  ;
  void ActOnMe
    (const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner,
   const RationalFunctionOld& theRingUnit, const RationalFunctionOld& theRingZero, GlobalVariables* theGlobalVariables)
   ;
  bool IsACoeffOneChevalleyGenerator(int& outputGenerator, SemisimpleLieAlgebra& owner);
  bool IsProportionalTo(const ElementSemisimpleLieAlgebra& other)const
  { Vector<Rational>  tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
   return tempRoot1.IsProportionalTo(tempRoot2);
  }
  bool IsProportionalTo(const ElementSemisimpleLieAlgebra& other, Rational& outputTimesMeEqualsInput)const
  { Vector<Rational>  tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.IsProportionalTo(tempRoot2, outputTimesMeEqualsInput);
  }
  bool MustUseBracketsWhenDisplayingMeRaisedToPower();
  void MakeZero
  (SemisimpleLieAlgebra& inputOwner)
  ;
  unsigned int HashFunction()const
  { return this->indexOfOwnerAlgebra*SomeRandomPrimes[0]
    +this->::MonomialCollection<ChevalleyGenerator, Rational>::HashFunction()*SomeRandomPrimes[1];
  }
  static unsigned int HashFunction(const ElementSemisimpleLieAlgebra& input)
  { return input.HashFunction();
  }
  template<class otherType>
  void operator=(const ElementSemisimpleLieAlgebra<otherType>& other)
  { this->::MonomialCollection<ChevalleyGenerator, CoefficientType>::operator=(other);
  }
};

template <class Object>
const Object& MemorySaving<Object>::GetElementConst()const
{ if (this->theValue==0)
  { std::cout << "Programming error: attempting to access zero pointer. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__,__LINE__);
    assert(false);
  }
  return *this->theValue;
}

template <class Object>
MemorySaving<Object>::~MemorySaving()
{ this->FreeMemory();
}

template <class Object>
void MemorySaving<Object>::FreeMemory()
{ delete this->theValue;
  this->theValue=0;
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter--;
  ParallelComputing::CheckPointerCounters();
#endif
}

template <class Object>
Object& MemorySaving<Object>::GetElement()
{ if (this->theValue==0)
  { this->theValue= new Object;
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  return *(this->theValue);
}

template <class Element>
int Matrix<Element>::FindPositiveLCMCoefficientDenominatorsTruncated()
{ int result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      result*=(this->elements[i][j].DenShort/Rational::gcdSigned(result, this->elements[i][j].DenShort));
  return result;
}

template <class Element>
LargeIntUnsigned Matrix<Element>::FindPositiveLCMCoefficientDenominators()
{ LargeIntUnsigned result=1;
  LargeIntUnsigned tempI;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j].GetDenominator(tempI);
      result=LargeIntUnsigned::lcm(result, tempI);
    }
  return result;
}

template <class Element>
void Matrix<Element>::GetMatrixIntWithDen
(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen)
{ outputDen=this->FindPositiveLCMCoefficientDenominators();
  outputMat.init(this->NumRows, this->NumCols);
  Rational tempRat;
  LargeIntUnsigned tempI;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempRat=this->elements[i][j]*outputDen;
      tempRat.GetDenominator(tempI);
      outputMat.elements[i][j]=tempI;
    }
}

template <class Element>
int Matrix<Element>::FindPositiveGCDCoefficientNumeratorsTruncated()
  { int result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      if (this->elements[i][j].NumShort!=0)
        result=Rational::gcdSigned(result, this->elements[i][j].NumShort);
  assert(result!=0);
  if (result<0)
    result=-result;
  return result;
}

template <class Element>
void Matrix<Element>::ScaleToIntegralForMinRationalHeightNoSignChange()
{ Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(this->FindPositiveLCMCoefficientDenominatorsTruncated(), this->FindPositiveGCDCoefficientNumeratorsTruncated());
  *this*=(tempRat);
}

template <class Element>
Element Matrix<Element> ::GetDeterminant()
{ Matrix<Element> tempMat=*this;
  Element result;
  tempMat.ComputeDeterminantOverwriteMatrix(result);
  return result;
}

template <class Element>
Element Matrix<Element>::GetTrace()
{   Element acc = 0;
    for(int i=0; i<NumCols; i++)
        acc += this->elements[i][i];
    return acc;
}

template <class Element>
Matrix<Element> Matrix<Element>::operator*(const Matrix<Element>& right)const
{ Matrix<Element> tempMat;
  tempMat=right;
  tempMat.MultiplyOnTheLeft(*this);
  return tempMat;
}

template <class Element>
Vector<Element> Matrix<Element>::operator*(const Vector<Element>& v) const
{
  if(v.size != NumCols){
    std::cout << "matrix application mismatch: matrix with" << NumCols << "columns attempted to multiply vector of length" << v.size << CGI::GetStackTraceEtcErrorMessage(__FILE__,__LINE__)  << std::endl;
    assert(false);
  }
  Vector<Element> out;
  out.MakeZero(NumRows);
  for(int i=0;i<NumRows;i++){
    for(int j=0;j<NumCols;j++){
      out[i] += elements[i][j] * v[j];
    }
  }
  return out;
}

template <class Element>
void Matrix<Element>::AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den)
{ this->init(theMat.NumRows, theMat.NumCols);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j]=theMat.elements[i][j];
      this->elements[i][j]/=Den;
    }
}

template<class CoefficientType>
bool Vectors<CoefficientType>::IsRegular
(Vector<CoefficientType>& r, Vector<CoefficientType>& outputFailingNormal, GlobalVariables& theGlobalVariables,
 int theDimension)
{ //this function needs a complete rewrite
  Selection WallSelection=theGlobalVariables.selWallSelection.GetElement();
  WallSelection.init(this->size);
  int x= MathRoutines::NChooseK(this->size, theDimension-1);
  Matrix<CoefficientType> bufferMat;
  for (int i=0; i<x; i++)
  { WallSelection.incrementSelectionFixedCardinality(theDimension-1);
    Vector<CoefficientType> tempRoot;
    if (this->ComputeNormalFromSelection(tempRoot, WallSelection, bufferMat, theDimension))
    { CoefficientType tempRat=Vector<CoefficientType>::ScalarEuclidean(tempRoot, r);
      if (tempRat.IsEqualToZero())
      { outputFailingNormal=(tempRoot);
        return false;
      }
    }
  }
  return true;
}

template<class CoefficientType>
bool Vectors<CoefficientType>::ComputeNormalExcludingIndex
(Vector<CoefficientType>& output, int index, Matrix<CoefficientType>& bufferMatrix)
{ Matrix<CoefficientType> matOutputEmpty;
  Selection NonPivotPoints;
  if (this->size==0)
    return false;
  int theDimension= this->TheObjects[0].size;
  output.SetSize(theDimension);
  bufferMatrix.init((int)this->size-1, (int)theDimension);
  matOutputEmpty.init(-1, -1);
  int k=-1;
  for(int i=0; i<this->size; i++)
    if (i!=index)
    { k++;
      for(int j=0; j<theDimension; j++)
        bufferMatrix.elements[k][j].Assign(this->TheObjects[i].TheObjects[j]);
    }
  Matrix<CoefficientType>::GaussianEliminationByRows(bufferMatrix, matOutputEmpty, NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection!=1)
    return false;
  bufferMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template<class CoefficientType>
bool Vectors<CoefficientType>::ComputeNormalFromSelection
(Vector<CoefficientType>& output, Selection& theSelection, Matrix<CoefficientType>& bufferMatrix, int theDimension)
{ Matrix<CoefficientType> matOutputEmpty;
  Selection NonPivotPoints;
  matOutputEmpty.init(-1, -1);
  output.SetSize(theDimension);
  bufferMatrix.init((int)theSelection.CardinalitySelection, (int)theDimension);
  for(int i=0; i<theSelection.CardinalitySelection; i++)
    for(int j=0; j<theDimension; j++)
      bufferMatrix.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].TheObjects[j]);
  Matrix<Rational>::GaussianEliminationByRows(bufferMatrix, matOutputEmpty, NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection!=1)
    return false;
  bufferMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template<class CoefficientType>
bool Vectors<CoefficientType>::ComputeNormalFromSelectionAndExtraRoot
(Vector<CoefficientType>& output, Vector<CoefficientType>& ExtraRoot,
 Selection& theSelection, Matrix<CoefficientType>& bufferMatrix, Selection& bufferSel)
{ if (this->size==0)
    return false;
  int theDimension= this->TheObjects[0].size;
  output.SetSize(theDimension);
  Matrix<CoefficientType> matOutputEmpty;
  Selection& NonPivotPoints=bufferSel;
  bufferMatrix.init((int)theSelection.CardinalitySelection+1, (int)theDimension);
  matOutputEmpty.init(-1, -1);
  for(int j=0; j<theDimension; j++)
  { for(int i=0; i<theSelection.CardinalitySelection; i++)
      bufferMatrix.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].TheObjects[j]);
    bufferMatrix.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot.TheObjects[j]);
  }
  Matrix<CoefficientType>::GaussianEliminationByRows(bufferMatrix, matOutputEmpty, NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection!=1)
    return false;
  bufferMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template<class CoefficientType>
bool Vectors<CoefficientType>::ComputeNormalFromSelectionAndTwoExtraRoots
(Vector<CoefficientType>& output, Vector<CoefficientType>& ExtraRoot1, Vector<CoefficientType>& ExtraRoot2,
 Selection& theSelection, Matrix<CoefficientType>& bufferMat, Selection& bufferSel)
{ Matrix<Rational> matOutputEmpty;
  Selection& NonPivotPoints= bufferSel;
  if (this->size==0)
    return false;
  int theDimension= this->TheObjects[0].size;
  output.SetSize(theDimension);
  matOutputEmpty.init(-1, -1);
  bufferMat.init((int)theSelection.CardinalitySelection+2, (int)theDimension);
  for(int j=0; j<theDimension; j++)
  { for(int i=0; i<theSelection.CardinalitySelection; i++)
      bufferMat.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].TheObjects[j]);
    bufferMat.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot1.TheObjects[j]);
    bufferMat.elements[theSelection.CardinalitySelection+1][j].Assign(ExtraRoot2.TheObjects[j]);
  }
  Matrix<Rational> ::GaussianEliminationByRows(bufferMat, matOutputEmpty, NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection!=1)
    return false;
  bufferMat.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template<class CoefficientType>
void Vectors<CoefficientType>::GetGramMatrix
(Matrix<CoefficientType> & output, const Matrix<CoefficientType>& theBilinearForm) const
{ output.Resize(this->size, this->size, false);
  for (int i=0; i<this->size; i++)
    for(int j=i; j<this->size; j++)
    { Vector<CoefficientType>::ScalarProduct(this->TheObjects[i], this->TheObjects[j], theBilinearForm, output.elements[i][j]);
      if (i!=j)
        output.elements[j][i].Assign(output.elements[i][j]);
    }
}

template<class CoefficientType>
bool Vectors<CoefficientType>::ContainsARootNonPerpendicularTo
(const Vector<CoefficientType>& input, Matrix<CoefficientType>& theBilinearForm)
{ CoefficientType tempRat;
  for (int i=0; i<this->size; i++)
    if (!Vector<CoefficientType>::ScalarProduct(this->TheObjects[i], input, theBilinearForm).IsEqualToZero())
      return true;
  return false;
}

template<class CoefficientType>
int Vectors<CoefficientType>::ArrangeFirstVectorsBeOfMaxPossibleRank
(Matrix<CoefficientType>& bufferMat, Selection& bufferSel)
{ if (this->size==0)
    return 0;
  int theDimension= this->GetDimensionOfElements();
  Selection NonPivotPoints;
  Matrix<Rational>  tempMatrix;
  Vectors<Rational> tempRoots;
  int oldRank=0;
  for (int i=0; i<this->size; i++)
  { tempRoots.AddOnTop(this->TheObjects[i]);
    int newRank= tempRoots.GetRankOfSpanOfElements(bufferMat, bufferSel);
    if (newRank==oldRank)
      tempRoots.RemoveIndexSwapWithLast(tempRoots.size-1);
    else
    { this->SwapTwoIndices(oldRank, i);
      assert(oldRank+1==newRank);
      oldRank=newRank;
    }
    if (oldRank== theDimension)
      return theDimension;
  }
  return (oldRank);
}

template <class CoefficientType>
void Polynomial<CoefficientType> ::MakePolyFromDirectionAndNormal
(Vector<CoefficientType>& direction, Vector<CoefficientType>& normal, CoefficientType& Correction)
{ Rational tempRat2= Vector<CoefficientType>::ScalarEuclidean(direction, normal);
  this->MakeZero(direction.size);
  MonomialP tempM;
  for (int i=0; i<direction.size; i++)
  { tempM.MakeEi(i);
    this->AddMonomial(tempM, normal.TheObjects[i]/tempRat2);
  }
  *this+=Correction;
}

template <class CoefficientType>
bool Vectors<CoefficientType>::GetNormalSeparatingCones
(GlobalVariables& theGlobalVariables, int theDimension, List<Vector<CoefficientType> >& coneStrictlyPositiveCoeffs,
  List<Vector<CoefficientType> >& coneNonNegativeCoeffs, Vector<CoefficientType>& outputNormal)
{ Matrix<Rational>& matA= theGlobalVariables.matConeCondition1.GetElement();
  Matrix<Rational>& matb= theGlobalVariables.matConeCondition2.GetElement();
  Matrix<Rational>& matX= theGlobalVariables.matConeCondition3.GetElement();
  if (coneStrictlyPositiveCoeffs.size==0)
    return true;
  int numRows= coneStrictlyPositiveCoeffs.size + coneNonNegativeCoeffs.size;
  matA.init((int)numRows, (int)theDimension*2+numRows);
  matA.NullifyAll();
  matb.init((int)numRows, 1);
  matb.NullifyAll();
  for (int i=0; i<coneStrictlyPositiveCoeffs.size; i++)
  { for (int k=0; k<theDimension; k++)
    { matA.elements[i][k].Assign(coneStrictlyPositiveCoeffs.TheObjects[i].TheObjects[k]);
      matA.elements[i][k+theDimension].Assign(matA.elements[i][k]);
      matA.elements[i][k+theDimension].Minus();
    }
    matb.elements[i][0].MakeOne();
    matA.elements[i][theDimension*2+i].MakeMOne();
  }
  for (int i=0; i<coneNonNegativeCoeffs.size; i++)
  { int currentRow=i+coneStrictlyPositiveCoeffs.size;
    for (int k=0; k<theDimension; k++)
    { matA.elements[currentRow][k].Assign(coneNonNegativeCoeffs.TheObjects[i].TheObjects[k]);
      matA.elements[currentRow][k+theDimension].Assign(matA.elements[currentRow][k]);
      matA.elements[currentRow][k+theDimension].Minus();
    }
    matA.elements[currentRow][2*theDimension+currentRow].MakeOne();
  }
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  bool result=Matrix<Rational> ::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(matA, matb, matX, theGlobalVariables);
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  outputNormal.MakeZero(theDimension);
  for (int i=0; i<theDimension; i++)
    outputNormal.TheObjects[i]=matX.elements[i][0]-matX.elements[i+theDimension][0];
  if (result)
  { Rational tempRat;
    for(int i=0; i<coneStrictlyPositiveCoeffs.size; i++)
    { tempRat=Vector<Rational>::ScalarEuclidean(coneStrictlyPositiveCoeffs.TheObjects[i], outputNormal);
      assert(tempRat.IsPositive());
    }
    for(int i=0; i<coneNonNegativeCoeffs.size; i++)
    { tempRat=Vector<Rational>::ScalarEuclidean(coneNonNegativeCoeffs.TheObjects[i], outputNormal);
      assert(tempRat.IsNonPositive());
    }
  }
//  outputNormal.ComputeDebugString();
  return result;
}

template <class CoefficientType>
bool Vectors<CoefficientType>::ConesIntersect
(GlobalVariables& theGlobalVariables, List<Vector<Rational> >& StrictCone, List<Vector<Rational> >& NonStrictCone,
 int theDimension)
{ Matrix<Rational>& matA= theGlobalVariables.matConeCondition1.GetElement();
  Matrix<Rational>& matb= theGlobalVariables.matConeCondition2.GetElement();
  Matrix<Rational>& matX= theGlobalVariables.matConeCondition3.GetElement();
  if (StrictCone.size==0)
    return false;
  int numCols= StrictCone.size + NonStrictCone.size;
  matA.init((int)theDimension+1, (int)numCols);
  matb.init((int)theDimension+1, 1);
  matb.NullifyAll(); matb.elements[theDimension][0].MakeOne();
  for (int i=0; i<StrictCone.size; i++)
  { for (int k=0; k<theDimension; k++)
      matA.elements[k][i].Assign(StrictCone.TheObjects[i].TheObjects[k]);
    matA.elements[theDimension][i].MakeOne();
  }
  for (int i=0; i<NonStrictCone.size; i++)
  { int currentCol=i+StrictCone.size;
    for (int k=0; k<theDimension; k++)
    { matA.elements[k][currentCol].Assign(NonStrictCone.TheObjects[i].TheObjects[k]);
      matA.elements[k][currentCol].Minus();
    }
    matA.elements[theDimension][currentCol].MakeZero();
  }
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  return Matrix<Rational> ::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(matA, matb, matX, theGlobalVariables);
}

template<class CoefficientType>
void Matrix<CoefficientType>::ComputePotentialChangeGradient
(Matrix<CoefficientType> & matA, Selection& BaseVariables,
 int NumTrueVariables, int ColumnIndex,
 Rational& outputChangeGradient, bool& hasAPotentialLeavingVariable)
{ hasAPotentialLeavingVariable = false;
  outputChangeGradient.MakeZero();
  for (int j=0; j<matA.NumRows; j++)
  { if (BaseVariables.elements[j]>=NumTrueVariables)
      outputChangeGradient+=(matA.elements[j][ColumnIndex]);
    hasAPotentialLeavingVariable =hasAPotentialLeavingVariable || matA.elements[j][ColumnIndex].IsPositive();
  }
  if (ColumnIndex>=NumTrueVariables)
    outputChangeGradient-=1;
}

template<class Element>
void Matrix<Element>::GetMaxMovementAndLeavingVariableRow
  (Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables,
   Matrix<Element>& tempMatA, Matrix<Element>& matX, Selection& BaseVariables)
{ LeavingVariableRow=-1;
  maxMovement.MakeZero();
  for(int i=0; i<tempMatA.NumRows; i++)
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

template <typename Element>
void Matrix<Element>::AssignRootsToRowsOfMatrix(const Vectors<Rational>& input)
{ this->init(input.size, input.GetDim());
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j]=input[i][j];
}

template<class Element>
bool Matrix<Element>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
(Matrix<Element>& matA, Matrix<Element>& matb, Matrix<Element>& outputSolution, GlobalVariables& theGlobalVariables)
//this function return true if Ax=b>=0 has a solution with x>=0 and records a solution x at outputPoint
//else returns false, where b is a given nonnegative column vector, A is an n by m matrix
//and x is a column vector with m entries
{ Matrix<Rational> & tempMatA=theGlobalVariables.matSimplexAlgorithm1.GetElement();
  Matrix<Rational> & matX=theGlobalVariables.matSimplexAlgorithm2.GetElement();
  Matrix<Rational> & tempDebugMat=theGlobalVariables.matSimplexAlgorithm3.GetElement();
  //Matrix<Rational> & tempMatb=theGlobalVariables.matSimplexAlgorithm3;
  Selection& BaseVariables = theGlobalVariables.selSimplexAlg2.GetElement();
  Rational GlobalGoal; GlobalGoal.MakeZero();
  assert (matA.NumRows== matb.NumRows);
  for (int j=0; j<matb.NumRows; j++)
  { GlobalGoal+=(matb.elements[j][0]);
    assert(!matb.elements[j][0].IsNegative());
  }
  if (GlobalGoal.IsEqualToZero())
    return false;
  int NumTrueVariables=matA.NumCols;
  //tempMatb.Assign(matb);
  tempMatA.init(matA.NumRows, NumTrueVariables+matA.NumRows);
  matX.init(tempMatA.NumCols, 1);
  HashedList<Selection>& VisitedVertices = theGlobalVariables.hashedSelSimplexAlg.GetElement();
  VisitedVertices.Clear();
  BaseVariables.init(tempMatA.NumCols);
  tempMatA.NullifyAll();  matX.NullifyAll();
  for (int j=0; j<matA.NumCols; j++)
    for (int i=0; i<matA.NumRows; i++)
      tempMatA.elements[i][j].Assign(matA.elements[i][j]);
  for (int j=0; j<matA.NumRows; j++)
  { tempMatA.elements[j][j+NumTrueVariables].MakeOne();
    matX.elements[j+NumTrueVariables][0].Assign(matb.elements[j][0]);
    BaseVariables.AddSelectionAppendNewIndex(j+NumTrueVariables);
  }
  if (Matrix<Rational> ::flagAnErrorHasOccurredTimeToPanic)
  { tempDebugMat=(tempMatA);
  }
  Rational  PotentialChangeGradient, ChangeGradient; //Change, PotentialChange;
  int EnteringVariable=0;
  bool WeHaveNotEnteredACycle=true;
//  int ProblemCounter=0;
  while (EnteringVariable!=-1 && WeHaveNotEnteredACycle && GlobalGoal.IsPositive())
  {//  ProblemCounter++;
  //  if (ProblemCounter==8)
    //{ BaseVariables.ComputeDebugString();
    //}
    //tempMatA.ComputeDebugString(); matX.ComputeDebugString();
    EnteringVariable=-1; ChangeGradient.MakeZero();
    for (int i=0; i<tempMatA.NumCols; i++)
    { if (!BaseVariables.selected[i])
      { Rational PotentialChangeGradient; bool hasAPotentialLeavingVariable;
        Matrix<Rational> ::ComputePotentialChangeGradient(  tempMatA, BaseVariables, NumTrueVariables, i, PotentialChangeGradient, hasAPotentialLeavingVariable);
        if (PotentialChangeGradient.IsGreaterThanOrEqualTo(ChangeGradient) && hasAPotentialLeavingVariable)
        { EnteringVariable= i;
          ChangeGradient.Assign(PotentialChangeGradient);
        }
      }
    }
    if (EnteringVariable!=-1)
    { int LeavingVariableRow;  Rational MaxMovement;
      Matrix<Rational> ::GetMaxMovementAndLeavingVariableRow(MaxMovement, LeavingVariableRow, EnteringVariable, NumTrueVariables, tempMatA, matX, BaseVariables);
      Rational tempRat, tempTotalChange;
      assert(!tempMatA.elements[LeavingVariableRow][EnteringVariable].IsEqualToZero());
      tempRat.Assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
      tempRat.Invert();
  //    if (BaseVariables.elements[LeavingVariableRow]==34)
  //      tempMatA.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
        assert(tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1));
      tempMatA.RowTimesScalar(LeavingVariableRow, tempRat);
      //if (BaseVariables.elements[LeavingVariableRow]==34)
      //  tempMatA.ComputeDebugString();
      tempTotalChange.Assign(MaxMovement);
      tempTotalChange.MultiplyBy(ChangeGradient);
      matX.elements[EnteringVariable][0]+=(MaxMovement);
      if (!tempTotalChange.IsEqualToZero())
      { VisitedVertices.Clear();
        GlobalGoal.Subtract(tempTotalChange);
      }
      else
      { //BaseVariables.ComputeDebugString();
        int tempI= VisitedVertices.GetIndex(BaseVariables);
        if (tempI==-1)
          VisitedVertices.AddOnTop(BaseVariables);
        else
          WeHaveNotEnteredACycle=false;
      }
      //if (BaseVariables.elements[LeavingVariableRow]==34)
      //  tempMatA.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
      { if (!tempMatA.elements[i][EnteringVariable].IsEqualToZero()&& i!=LeavingVariableRow)
        { tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.MultiplyBy(MaxMovement);
          matX.elements[BaseVariables.elements[i]][0].Subtract(tempRat);
          tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.Minus();
          tempMatA.AddTwoRows(LeavingVariableRow, i, 0, tempRat);
        }
        if (i==LeavingVariableRow)
          matX.elements[BaseVariables.elements[i]][0].MakeZero();
        //tempMatA.ComputeDebugString();
        //matX.ComputeDebugString();
      }
      assert(matX.elements[BaseVariables.elements[LeavingVariableRow]][0].IsEqualToZero());
      BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]]=false;
      BaseVariables.elements[LeavingVariableRow]= EnteringVariable;
      BaseVariables.selected[EnteringVariable]= true;
      //BaseVariables.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
        assert(tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1));
    }
//    if (::Matrix<Rational> ::flagAnErrorHasOccurredTimeToPanic)
//    { Matrix<Rational>  tempMat;
//      tempMat.Assign(matX);
//      tempMat.ComputeDebugString();
//      tempDebugMat.ComputeDebugString();
//      tempMat.MultiplyOnTheLeft(tempDebugMat);
//      tempMat.ComputeDebugString();
//      assert(tempMat.IsEqualTo(matb));
//    }
  }
//  std::string tempS;
//  std::stringstream out;
//  for (int i=0; i<BaseVariables.CardinalitySelection; i++)
//  { int tempI=BaseVariables.elements[i];
//    matX.elements[tempI][0].ToString(tempS);
//    out << tempS <<"(";
//    if (tempI<matA.NumCols)
//    {  for (int j=0; j<matA.NumRows; j++)
//      { matA.elements[j][tempI].ToString(tempS);
//        out << tempS;
//        if (j!=matA.NumRows-1)
//          out <<", ";
//      }
//    } else
//      out<<"dummy column " << i <<" ";
//    out <<")";
//    if (i!=BaseVariables.CardinalitySelection-1)
//      out <<"+";
//  }
//  tempS=out.str();
  for(int i=NumTrueVariables; i<matX.NumRows; i++)
    if (matX.elements[i][0].IsPositive())
      return false;
  outputSolution.Resize(NumTrueVariables, 1, false);
  for (int i=0; i<NumTrueVariables; i++)
    outputSolution.elements[i][0].Assign(matX.elements[i][0]);
  return true;
}

template<class CoefficientType>
void Polynomial<CoefficientType>::ScaleToIntegralNoGCDCoeffs()
{ if(this->size==0)
    return;
  int indexHighestMon=0;
  LargeIntUnsigned tempInt1, tempInt2, accumNum, accumDen;
  accumDen.MakeOne();
  this->theCoeffs[0].GetNumerator(accumNum);
  for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i].IsGEQLexicographicLastVariableStrongest(this->TheObjects[indexHighestMon]))
      indexHighestMon=i;
    Rational& tempRat=this->theCoeffs[i];
    tempRat.GetDenominator(tempInt1);
    tempRat.GetNumerator(tempInt2);
    LargeIntUnsigned::lcm(tempInt1, accumDen, accumDen);
    LargeIntUnsigned::gcd(tempInt2, accumNum, accumNum);
  }
  Rational theMultiple;
  theMultiple.MakeOne();
  if (this->theCoeffs[indexHighestMon].IsNegative())
    theMultiple.MakeMOne();
  theMultiple.MultiplyByLargeIntUnsigned(accumDen);
  Rational tempRat2;
  LargeInt tempInt3;
  tempInt3.AssignLargeIntUnsigned(accumNum);
  tempRat2.AssignLargeInteger(tempInt3);
  theMultiple.DivideBy(tempRat2);
  *this*=(theMultiple);
}

template <typename Element>
inline void Matrix<Element>::ActOnMonomialAsDifferentialOperator
(MonomialP& input, Polynomial<Rational>& output)
{ assert(this->NumRows==this->NumCols);
  MonomialP tempMon;
  output.MakeZero();
  Rational coeff;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempMon=input;
      coeff=tempMon(j);
      coeff*=this->elements[i][j];
      tempMon[j]-=1;
      tempMon[i]+=1;
      output.AddMonomial(tempMon, coeff);
    }
}

template <typename Element>
void Matrix<Element>::FindZeroEigenSpaceOneOneForEachNonPivot(Vectors<Element>& output)
{ Matrix<Element> tempMat;
  tempMat=(*this);
  Matrix<Element> emptyMat;
  Selection nonPivotPts;
  tempMat.GaussianEliminationByRows(tempMat, emptyMat, nonPivotPts);
  output.SetSize(nonPivotPts.CardinalitySelection);
  for (int i=0; i<nonPivotPts.CardinalitySelection; i++)
  { Vector<Element>& current= output[i];
    current.MakeZero(this->NumCols);
    int currentPivotIndex = nonPivotPts.elements[i];
    current[currentPivotIndex]=1;
    int rowCounter=0;
    for (int j=0; j<this->NumCols; j++)
      if (!nonPivotPts.selected[j])
      { current[j]=-tempMat.elements[rowCounter][currentPivotIndex];
        rowCounter++;
      }
  }
}

template <class CoefficientType>
std::string Vectors<CoefficientType>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  std::string tempS;
  bool useLaTeX=false;
  bool useHtml=false;
  bool makeTable=false;
  if (theFormat!=0)
  { useLaTeX=theFormat->flagUseLatex;
    useHtml=theFormat->flagUseHTML;
//    makeTable=theFormat->flagma
  }
  if (!useLaTeX && !useHtml)
    out << this->size << " vectors: \n";
  if (useLaTeX && makeTable)
    out << "\\begin{tabular}{c}";
  if (useHtml && makeTable)
    out << "<table>";
  for (int i=0; i<this->size; i++)
  { tempS=this->TheObjects[i].ToString(theFormat);
    if (useHtml && makeTable)
      out << "<tr><td>";
    out << tempS;
    if (!makeTable && i!=this->size-1)
      out << ", ";
    if (useLaTeX && makeTable)
      out << "\\\\\n";
    if (useHtml && makeTable)
      out << "</td></tr>";
  }
  if (useHtml && makeTable)
    out << "</table>";
  if (useLaTeX && makeTable)
    out << "\\end{tabular}";
  return out.str();
}

template <class Object>
void List<Object>::IntersectWith(const List<Object>& other, List<Object>& output)const
{ if (&output==&other || this==&output)
  { List<Object> l1=*this;
    List<Object> l2=other;
    l1.IntersectWith(l2, output);
    return;
  }
  HashedList<Object> tempList;
  tempList=*this;
  output.SetSize(0);
  for (int i=0; i<other.size; i++)
    if (tempList.Contains(other[i]))
      output.AddOnTop(other[i]);
}

template <class CoefficientType>
std::string Vector<CoefficientType>::ToStringLetterFormat
(const std::string& inputLetter, FormatExpressions* theFormat, bool DontIncludeLastVar)const
{ if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  bool found=false;
  int NumVars= DontIncludeLastVar ? this->size-1 : this->size;
  for(int i=0; i<NumVars; i++)
    if (!this->TheObjects[i].IsEqualToZero())
    { tempS=this->TheObjects[i].ToString(theFormat);
      if (this->TheObjects[i].NeedsBrackets())
        tempS="("+tempS+")";
      if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
      if (found)
      { if (tempS.size()>0)
        { if (tempS[0]!='-')
            out << "+";
        } else
          out << "+";
      }
      found=true;
      out << tempS;
      out << inputLetter << "_{" << i+1<< "}";
    }
  return out.str();
}

template <class CoefficientType, unsigned int inputHashFunction(const CoefficientType&)>
void MonomialTensor<CoefficientType, inputHashFunction>::MultiplyByGeneratorPowerOnTheLeft
(int theGeneratorIndexStandsOnTheLeft, const CoefficientType& thePower)
{ if (thePower==0)
    return;
  List<int> newGeneratorIndices;
  List<CoefficientType> newPowers;
  newGeneratorIndices.SetExpectedSize(this->generatorsIndices.size+1);
  newPowers.SetExpectedSize(this->generatorsIndices.size+1);
  newGeneratorIndices.AddOnTop(theGeneratorIndexStandsOnTheLeft);
  newPowers.AddOnTop(thePower);
  newGeneratorIndices.AddListOnTop(this->generatorsIndices);
  newPowers.AddListOnTop(this->Powers);
  this->generatorsIndices=newGeneratorIndices;
  this->Powers=newPowers;
  this->SimplifyEqualConsecutiveGenerators(0);
}

template <class CoefficientType, unsigned int inputHashFunction(const CoefficientType&)>
void MonomialTensor<CoefficientType, inputHashFunction>::MultiplyByGeneratorPowerOnTheRight
(int theGeneratorIndex, const CoefficientType& thePower)
{ if (thePower==0)
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { (*this->Powers.LastObject())+=(thePower);
      return;
    }
  this->Powers.AddOnTop(thePower);
  this->generatorsIndices.AddOnTop(theGeneratorIndex);
}

template <class CoefficientType, unsigned int inputHashFunction(const CoefficientType&)>
std::string MonomialTensor<CoefficientType, inputHashFunction>::ToString(FormatExpressions* theFormat)const
{ if (this->generatorsIndices.size==0)
    return "1";
  std::string theLetter= theFormat==0 ?  "g" : theFormat->chevalleyGgeneratorLetter;
  std::stringstream out;
  for (int i=0; i< this->generatorsIndices.size; i++)
  { out << theLetter << "_{" << this->generatorsIndices[i] << "}";
    if (!(this->Powers[i]==1))
      out << "^{" << this->Powers[i] << "}";
  }
  return out.str();
}

template <class CoefficientType, unsigned int inputHashFunction(const CoefficientType&)>
bool MonomialTensor<CoefficientType, inputHashFunction>::SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex)
{ if (this->generatorsIndices.size<1)
    return false;
  if (lowestNonReducedIndex<0)
    lowestNonReducedIndex=0;
  bool result=false;
  for (int next=lowestNonReducedIndex+1; next<this->generatorsIndices.size; next++)
    if (this->generatorsIndices[lowestNonReducedIndex]==this->generatorsIndices[next])
    { result=true;
      this->Powers[lowestNonReducedIndex]+=this->Powers[next];
    }
    else
    { lowestNonReducedIndex++;
      this->Powers[lowestNonReducedIndex]=this->Powers[next];
      this->generatorsIndices[lowestNonReducedIndex]=this->generatorsIndices[next];
    }
  this->generatorsIndices.SetSize(lowestNonReducedIndex+1);
  this->Powers.SetSize(lowestNonReducedIndex+1);
  return result;
}

template <typename Element>
std::string Matrix<Element>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  std::string tempS;
  bool useHtml= theFormat==0 ? true : theFormat->flagUseHTML;
  bool useLatex= theFormat==0 ? false : theFormat->flagUseLatex;
  if (useHtml)
    out << "<table>";
  if (useLatex)
  { int verticalLineIndex= theFormat==0 ? -1 : theFormat->MatrixColumnVerticalLineIndex;
    out << "\\left(\\begin{array}{";
    for (int j=0; j<this->NumCols; j++)
    { out << "c";
      if (verticalLineIndex==j)
        out << "|";
    }
    out << "}";
  }
  for (int i=0; i<this->NumRows; i++)
  { if (useHtml)
      out << "<tr>";
    for (int j=0; j<this->NumCols; j++)
    { tempS=this->elements[i][j].ToString();
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
  return out.str();
}

template <class TemplateMonomial, class CoefficientType>
std::string MonomialCollection<TemplateMonomial, CoefficientType>::GetBlendCoeffAndMon
(const TemplateMonomial& inputMon, CoefficientType& inputCoeff, bool addPlusToFront,
 FormatExpressions* theFormat)
{ std::stringstream out;
  std::string coeffStr=inputCoeff.ToString();
  if (inputMon.IsAConstant())
  { if (coeffStr[0]!='-' && addPlusToFront)
      out << "+" << coeffStr;
    else
      out << coeffStr;
    return out.str();
  }
  std::string monString=inputMon.ToString(theFormat);
  if (coeffStr=="1")
  { if (addPlusToFront)
      out << "+";
    out << monString;
    return out.str();
  }
  if (coeffStr=="-1")
  { out << "-" << monString;
    return out.str();
  }
  if (coeffStr[0]!='-' && addPlusToFront)
    out << "+" << coeffStr << monString;
  else
    out << coeffStr << monString;
  return out.str();
}

template <class TemplateMonomial, class CoefficientType>
std::string MonomialCollection<TemplateMonomial, CoefficientType>::ToString
(FormatExpressions* theFormat)const
{ if (this->size==0)
    return "0";
  std::stringstream out;
  std::string tempS1, tempS2;
  List<TemplateMonomial> sortedMons;
  sortedMons=*this;
  //If this line fails to link, you must do the following.
  //You need to implement FormatExpressions::GetMonOrder<TemplateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  //wish to use a custom one.
  typename List<TemplateMonomial>::OrderLeftGreaterThanRight
// ALERT!! Some feature was disabled because I clobbered Dr. Milev's changes
// because I forgot to svn diff prior to svn up and don't know how he made this
// actually work.  So now I'm committing something that builds, but is broken.
  theOrder= theFormat==0? 0: theFormat->GetMonOrder<TemplateMonomial>();
  sortedMons.QuickSortDescending(theOrder);
//  out << "(hash: " << this->HashFunction() << ")";
  int cutOffCounter=0;
  bool useCustomPlus=false;
  bool useCustomTimes=false;
  int MaxLineLength=theFormat==0? 200 : theFormat->MaxLineLength;
  int NumAmpersandsPerNewLineForLaTeX=theFormat==0? 1: theFormat->NumAmpersandsPerNewLineForLaTeX;
  bool flagUseLaTeX=theFormat==0? false: theFormat->flagUseLatex;
  bool flagUseHTML=theFormat==0? false: theFormat->flagUseHTML;
  std::string oldCustomTimes="";
  if (theFormat!=0)
  { useCustomPlus=(theFormat->CustomPlusSign!="");
    useCustomTimes=(theFormat->CustomCoeffMonSeparator!="");
    if (theFormat->flagPassCustomCoeffMonSeparatorToCoeffs==false)
    { oldCustomTimes=theFormat->CustomCoeffMonSeparator;
      theFormat->CustomCoeffMonSeparator="";
    }
  }
  for (int i=0; i<sortedMons.size; i++)
  { TemplateMonomial& currentMon=sortedMons[i];
    CoefficientType& currentCoeff=this->theCoeffs[this->GetIndex(currentMon)];
    if (currentCoeff.NeedsBrackets())
      tempS1="("+currentCoeff.ToString(theFormat)+ ")";
    else
      tempS1=currentCoeff.ToString(theFormat);
    tempS2=currentMon.ToString(theFormat);
    if (!useCustomTimes)
    { if (tempS1=="1" && tempS2!="1")
        tempS1="";
      if (tempS1=="-1"&& tempS2!="1")
        tempS1="-";
      if(tempS2!="1")
        tempS1+=tempS2;
    } else
    { tempS1+=oldCustomTimes;
      tempS1+=tempS2;
    }
    if (i>0)
    { if (!useCustomPlus)
      { if (tempS1.size()>0)
        { if (tempS1[0]!='-')
          { out << "+";
            cutOffCounter+=1;
          }
        } else
        { out << "+";
          cutOffCounter+=1;
        }
      } else
        out << theFormat->CustomPlusSign;
    }
    out << tempS1;
    cutOffCounter+=tempS1.size();
    if (MaxLineLength>0)
      if (cutOffCounter>MaxLineLength)
      { cutOffCounter=0;
        if (flagUseLaTeX && i!=sortedMons.size-1)
        { out << " \\\\";
          for (int k=0; k<NumAmpersandsPerNewLineForLaTeX; k++)
            out << "&";
          out << " ";
        }
        if (flagUseHTML && !flagUseLaTeX)
          out << " <br>";
      }
  }
  if (theFormat!=0)
    theFormat->CustomCoeffMonSeparator=oldCustomTimes;
  return out.str();
}

template <typename Element>
inline void Matrix<Element>::AddTwoRows
(int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar,
 GlobalVariables* theGlobalVariables)
{ ProgressReport theReport(theGlobalVariables);
  bool doProgressReport=
  theGlobalVariables==0 ? false : theGlobalVariables->flagGaussianEliminationProgressReport;
  Element tempElement;
  for (int i = StartColIndex; i< this->NumCols; i++)
  { tempElement=this->elements[fromRowIndex][i];
    tempElement*=scalar;
    if (doProgressReport)
    { std::stringstream out;
      out << "Processing row, element " << i+1 << " out of " << this->NumCols;
      theReport.Report(out.str());
    }
    this->elements[ToRowIndex][i]+=tempElement;
  }
}

template <typename Element>
void Matrix<Element>::GaussianEliminationByRows
(Matrix<Element>& mat, Matrix<Element>& output, Selection& outputSelection,
 bool returnNonPivotPoints, GlobalVariables* theGlobalVariables)
{ int tempI;
  int NumFoundPivots = 0;
  int MaxRankMat = MathRoutines::Minimum(mat.NumRows, mat.NumCols);
  Element tempElement;
  outputSelection.init(mat.NumCols);
  bool doProgressReport= theGlobalVariables==0 ? false : theGlobalVariables->flagGaussianEliminationProgressReport;
  ProgressReport theReport(theGlobalVariables);
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
//      tempS=tempElement.ToString();
      tempElement.Invert();
//      assert(tempElement.checkConsistency());
//      tempS=tempElement.ToString();
      mat.RowTimesScalar(NumFoundPivots, tempElement);
      output.RowTimesScalar(NumFoundPivots, tempElement);
//      assert(tempElement.checkConsistency());
      for (int j = 0; j<mat.NumRows; j++)
        if (j!=NumFoundPivots)
          if (!mat.elements[j][i].IsEqualToZero())
          { //if (i==9 && j==8 && Matrix<Element>::flagAnErrorHasOccurredTimeToPanic==true)
            //{ tempS=tempElement.ToString();
              //mat.ComputeDebugString();
            //}
            tempElement=(mat.elements[j][i]);
            tempElement.Minus();
            //if (i==9 && j==8 && Matrix<Element>::flagAnErrorHasOccurredTimeToPanic==true)
              //tempS=tempElement.ToString();
            if (doProgressReport)
            { std::stringstream out;
              out << "Gaussian elimination (" << mat.NumRows << "x" << mat.NumCols
              << "): column " << i+1 << " out of " << mat.NumCols
              << ".\n<br>Pivot row: " << NumFoundPivots+1 << ", eliminating row "
              << j+1 << " out of " << mat.NumRows;
              theReport.Report(out.str());
            }
            mat.AddTwoRows(NumFoundPivots, j, i, tempElement, theGlobalVariables);
            output.AddTwoRows(NumFoundPivots, j, 0, tempElement, theGlobalVariables);
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
#endif
