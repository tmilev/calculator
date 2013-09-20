//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_h_already_included
#define vpfHeader1_h_already_included

#include "vpfMacros.h"
static ProjectInformationInstance vpfHeader1instance(__FILE__, "Main header. Math and general routines. ");

//IMPORTANT.
//Convention on Hash functions.
//1. C++ objects that represent mathematically equal objects
//   are allowed to have different bit representations in RAM memory.
//2. Mathematically equal objects must have their object::HashFunction return identical
//   values, even if the objects have bitwise different representations in RAM.
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
{ 607,  1013, 2207, 3001, 4057, 5419, 5849, 6221,  7057, 7411, 7417, 7681, 7883, 8011, 8209, 8369, 8447, 9539, 10267, 10657, 11489, 12071, 12613, 13933, 14759
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
class AlgebraicNumber;
class RationalFunctionOld;
class SemisimpleLieAlgebra;
class rootSubalgebra;
class rootSubalgebras;
class ChevalleyGenerator;
class DynkinDiagramRootSubalgebra;
template<class coefficient>
class ElementSemisimpleLieAlgebra;
template<class coefficient>
class ElementUniversalEnveloping;
template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
class MonomialTensor;
template<class coefficient>
class MonomialUniversalEnveloping;
template<class coefficient>
class ElementSumGeneralizedVermas;
template <class coefficient>
class charSSAlgMod;
class ReflectionSubgroupWeylGroup;
template<class coefficient>
class ModuleSSalgebra;
class SltwoSubalgebras;
template<class coefficient>
class MonomialTensorGeneralizedVermas;
template<class coefficient>
class ElementTensorsGeneralizedVermas;
struct branchingData;
class quasiDiffMon;
template<class coefficient>
class quasiDiffOp;

//classes related to linear integral programming (polyhedra, lattices, quasipolynomials)
class Cone;
class ConeComplex;
class Lattice;
class affineHyperplane;
class affineCones;

//Hybrid classes that serve both memory-management and mathematical purposes
//(Matrices, Vectors, PolynomialSubstitution, etc.)
template <class ObjectType1, class ObjectType2, unsigned int hashFunction1(const ObjectType1&)=ObjectType1::HashFunction, unsigned int hashFunction2(const ObjectType2&)=ObjectType2::HashFunction>
class Pair;
template <class Object>
class List;
template <class Object>
class Matrix;
template <class Object, unsigned int hashFunction(const Object&)>
class HashedList;
template <class coefficient>
class Vector;
template <class coefficient>
class Vectors;
class MonomialP;
template <class TemplateMonomial, class coefficient>
class MonomialCollection;
template <class coefficient>
class Polynomial;
template <class coefficient>
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
  template <class coefficient>
  static coefficient InvertXModN(const coefficient& X, const coefficient& N)
  { coefficient q, r, p, d; // d - divisor, q - quotient, r - remainder, p is the number to be divided
    coefficient vD[2], vP[2], temp;
    vP[0]=1; vP[1]=0; // at any given moment, p=vP[0]*N+vP[1]*X
    vD[0]=0; vD[1]=1;   // at any given moment, d=vD[0]*N+vD[1]*X
    p=N;
    d=X;
    d%=N;
    if (d<0)
    { d+=N;
    }
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
    if (!(p==1))
    { std::cout << "This is a programming error: the invert X mod N algorithm requires that X and N be relatively prime, which appears to have not been the case. "
      << MathRoutines::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }//if d and p were relatively prime this should be so. Otherwise the function was not called properly.
    p=vP[1]%N;
    if (p<0)
      p+=N;
    return p;
  }
  static int lcm(int a, int b);
  template <typename integral>
  static integral gcd(integral a, integral b)
  { integral temp;
    while(!(b==0))
    { temp= a % b;
      a=b;
      b=temp;
    }
    return a;
  }
  template <typename integral>
  static integral lcm(integral a, integral b)
  { //std::cout << "<br>\nlcm(" << a << ',' << b << ")=" << std::endl;
    integral result;
    result=a;
    result/=MathRoutines::gcd(a,b);
    result*=b;
    //std::cout << result << std::endl;
    return result;
  }
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
  template <class theType>
  static bool GenerateVectorSpaceClosedWRTLieBracket(List<theType>& inputOutputElts, int upperDimensionBound, GlobalVariables* theGlobalVariables=0)
  { return MathRoutines::GenerateVectorSpaceClosedWRTOperation(inputOutputElts, upperDimensionBound, theType::LieBracket, theGlobalVariables);
  }
  template <class theType>
  static bool GenerateVectorSpaceClosedWRTOperation
  (List<theType>& inputOutputElts, int upperDimensionBound, void (*theBinaryOperation)(const theType& left, const theType& right, theType& output),
   GlobalVariables* theGlobalVariables=0);
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
  static inline double Pi()
  { return 3.141592653589793238462643383279;
  }
// the MS compiler refuses to compile the following (WTF?), hence the above line.
//  static const double Pi=(double)3.141592653589793238462643383279;
  static int KToTheNth(int k, int n);
  static void KToTheNth(int k, int n, LargeInt& output);
  inline static int parity(int n)
  { if (n%2==0)
      return 1;
    else
      return -1;
  }
  static int BinomialCoefficientMultivariate(int N, List<int>& theChoices);
  static bool IsPrime(int theInt);
  template <class Element>
  static void RaiseToPower(Element& theElement, int thePower, const Element& theRingUnit);
  template <typename coefficient>
  inline static coefficient Maximum
  (const coefficient& a, const coefficient& b)
  { if (a>b)
      return a;
    else
      return b;
  }
  template <typename T>
  inline static void swap(T& a, T& b)
  { if (&a==&b)
      return;
    T temp;
    temp=a;
    a=b;
    b=temp;
  }
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
  static double ReducePrecision(double x)
  { if (x<0.00001 && x>-0.00001)
      return 0;
    return x;
  }
  inline static unsigned int HashDouble(const double& input)
  { return (unsigned) (input*10000);
  }
  inline static unsigned int IntUnsignIdentity(const int& input)
  { return (unsigned) input;
  }
  inline static unsigned int ListIntsHash(const List<int>& input)
  ;
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
  inline static bool ReadThroughFirstOpenTag(std::istream& streamToMoveIn, const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { int tempInt;
    return XMLRoutines::ReadThroughFirstOpenTag(streamToMoveIn, tempInt, tagNameNoSpacesNoForbiddenCharacters);
  }
  inline static bool ReadEverythingPassedTagOpenUntilTagClose(std::istream& streamToMoveIn, const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { int tempInt;
    return XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(streamToMoveIn, tempInt, tagNameNoSpacesNoForbiddenCharacters);
  }
  static bool ReadThroughFirstOpenTag(std::istream& streamToMoveIn, int& NumReadWordsExcludingTag, const std::string& tagNameNoSpacesNoForbiddenCharacters)
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
  static bool ReadEverythingPassedTagOpenUntilTagClose(std::istream& streamToMoveIn, int& NumReadWordsExcludingTag, const std::string& tagNameNoSpacesNoForbiddenCharacters)
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
  MemorySaving(const MemorySaving<Object>& other)
  { assert(false);
  }
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
  bool IsZeroPointer()const
  { return this->theValue==0;
  }
  void FreeMemory();
  MemorySaving()
  { this->theValue=0;
  }
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
  int SizeWithoutObjects()const;
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
  inline Object& operator[](int i)const
  { return this->TheObjects[i];
  }
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
  inline Object* LastObject()const
  { return &this->TheObjects[this->size-1];
  }
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
int ListLight<Object>::SizeWithoutObjects()const
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
  typedef bool (*OrderLeftGreaterThanRight) (const Object& left, const Object& right);
private:
  friend class Polynomial<Rational>;
  friend class IntegerPoly;
  friend class PartFractions;
  friend class PartFraction;
  void ExpandArrayOnTop(int increase);
  void QuickSortAscendingNoOrder(int BottomIndex, int TopIndex);
  //submitting zero comparison function is not allowed!
  //that is why the function is private.
  void QuickSortAscendingOrder(int BottomIndex, int TopIndex, List<Object>::OrderLeftGreaterThanRight theOrder);
  template <class compareClass, class carbonCopyType>
  bool QuickSortAscendingCustomRecursive(int BottomIndex, int TopIndex, compareClass& theCompareror, List<carbonCopyType>* carbonCopy);
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
  { // <-Registering stack trace forbidden! Multithreading deadlock alert.
    if (expectedSize==1)
      return 1;
    if (expectedSize==2)
      return 2;
    return (expectedSize*4)/3+1;
  }
  void SetExpectedSize(int theSize)
  { // <-Registering stack trace forbidden! Multithreading deadlock alert.
    if ((this->ActualSize)*5/6<theSize)
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
  void RemoveObjectsShiftDown(const List<Object>& theList)
  { int currentIndex=0;
    for (int i=0; i<this->size; i++)
      if (!theList.Contains((*this)[i]))
      { theList.SwapTwoIndices(i, currentIndex);
        currentIndex++;
      }
    this->SetSize(currentIndex);
  }
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
  //The following function returns false if the comparison operator failed!!!!
  template <class compareClass, class carbonCopyType=Object>
  bool QuickSortAscendingCustom(compareClass& theCompareror, List<carbonCopyType>* carbonCopy=0)
  { return this->QuickSortAscendingCustomRecursive(0, this->size-1, theCompareror, carbonCopy);
  }
  //The below function is required to preserve the order of elements given by theSelection.elements.
  void SubSelection(Selection& theSelection, List<Object>& output);
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
  bool ContainsAtLeastOneCopyOfEach(const List<Object>& other)const
  { for (int i=0; i<other.size; i++)
      if (!this->Contains(other[i]))
        return false;
    return true;
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
  void WriteToFile(std::fstream& output)const
  { this->WriteToFile(output, 0, -1);
  }
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)const
  { this->WriteToFile(output, theGlobalVariables, -1);
  }
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes)const;
//  inline bool Contains(Object& o){return this->Contains(o)!=-1; };
  int SizeWithoutObjects()const;
  inline Object* LastObject()const;// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void ReleaseMemory();

  unsigned int HashFunction()const
  { int numCycles=MathRoutines::Minimum(SomeRandomPrimesSize, this->size);
    int result=0;
    for (int i=0; i<numCycles; i++)
      result+=SomeRandomPrimes[i]*this->TheObjects[i].HashFunction();
    return result;
  }
  static inline unsigned int HashFunction(const List<Object>& input)
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

class ProgressReport
{
public:
  GlobalVariables* pointerGV;
  int currentLevel;
  void Report(const std::string& theReport);
  void initFromGV(GlobalVariables* theGlobalVariables);
  ProgressReport(GlobalVariables* theGlobalVariables)
  { this->initFromGV(theGlobalVariables);
  }
  ProgressReport(GlobalVariables* theGlobalVariables, const std::string& theReport)
  { this->initFromGV(theGlobalVariables);
    this->Report(theReport);
  }
  ~ProgressReport();
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
  static std::string GetCalculatorLink(const std::string& DisplayNameCalculator, const std::string& input);
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
  static std::string GetLatexEmbeddableLinkFromCalculatorInput(const std::string& address, const std::string& display);
  static bool GetHtmlStringSafeishReturnFalseIfIdentical(const std::string& input, std::string& output);
  static void TransormStringToHtmlSafeish(std::string& theString)
  { std::string tempS;
    CGI::GetHtmlStringSafeishReturnFalseIfIdentical(theString, tempS);
    theString=tempS;
  }
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
  static std::string GetHtmlMathSpanNoButtonAddBeginArrayL(const std::string& input);
  static std::string GetHtmlMathSpanPure(const std::string& input, int upperNumChars=700);
  static std::string GetHtmlMathFromLatexFormulA(const std::string& input, const std::string& prependString, const std::string& appendStringBeforeButton, bool useDiv, bool useBeginArrayRCL);
  static std::string GetStyleButtonLikeHtml()
  { return " style=\"background:none; border:0; text-decoration:underline; color:blue; cursor:pointer\" ";
  }
  static std::string GetHtmlButton(const std::string& buttonID, const std::string& theScript, const std::string& buttonText);
  static std::string GetHtmlSpanHidableStartsHiddeN(const std::string& input);
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
  static void makeStdCoutReport(IndicatorWindowVariables& input);
  static void FormatCPPSourceCode(const std::string& FileName);
  static void(*functionCGIServerIgnoreUserAbort)(void);
  static void SetCGIServerIgnoreUserAbort()
  { if (CGI::functionCGIServerIgnoreUserAbort!=0)
      CGI::functionCGIServerIgnoreUserAbort();
  }
};

template <class ObjectType1, class ObjectType2, unsigned int hashFunction1(const ObjectType1&),unsigned int hashFunction2(const ObjectType2&)>
class Pair
{
  friend std::ostream& operator << (std::ostream& output, const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& thePair)
  { output << "("  << thePair.Object1 << ", " <<  thePair.Object2 << ")";
    return output;
  }
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
  void RemoveFirstOccurenceSwapWithLast(Object& o);
  Object PopIndexShiftDown(int index);
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
          << " hash lookup array of index " << i << ", entry of index " << j << " reports index " << theIndex << " but I have only "
          << this->size << " entries. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
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
  void RemoveIndexShiftDown(int index)
  { for (int i=index; i<this->size-1; i++)
      this->SetObjectAtIndex(i, (*this)[i+1]);
    this->RemoveLastObject();
  }
  void SetObjectAtIndex(int index, const Object& theObject)
  { if (index<0 || index>=this->size)
    { std::cout << "This is a programming error. You are attempting to pop out index " << index << " out of hashed array "
      << " of size " << this->size << ". "<< CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    int hashIndexPop = this->GetHash(this->TheObjects[index]);
    this->TheHashedArrays[hashIndexPop].RemoveFirstOccurenceSwapWithLast(index);
    int hashIndexIncoming=this->GetHash(theObject);
    this->TheHashedArrays[hashIndexIncoming].AddOnTop(index);
    this->TheObjects[index]=theObject;
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
  void SwapTwoIndices(int i1, int i2)
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
      { std::cout << "This is a programming error: corrupt hash table: at hashindex= " << hashIndex << " I get instructed to look up index " << j
        << " but I have only " << this->size << "elements. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
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
    { std::cout << "This is a programming error: the programmer has requested the index of object " << o
      << " with a function that does not allow failure. However, the container array does not contain his object. "
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
  const Object& operator[](int i)const
  { return TemplateList::operator[](i);
  }
  Object& GetElement(int i)const
  { return TemplateList::operator[](i);
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
  Object& GetElement(int theObjectIndex)const
  { return this->::HashTemplate<Object, List<Object>, hashFunction>::GetElement(theObjectIndex);
  }
  void SetObjectAtIndex(int index, const Object& theObject)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::SetObjectAtIndex(index, theObject);
  }
  void RemoveIndexShiftDown(int index)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::RemoveIndexShiftDown(index);
  }
  void RemoveIndexSwapWithLast(int index)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::RemoveIndexSwapWithLast(index);
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
  inline void QuickSortAscending(typename List<Object>::OrderLeftGreaterThanRight theOrder=0)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::QuickSortAscending(theOrder);
  }
  inline void QuickSortDescending(typename List<Object>::OrderLeftGreaterThanRight theOrder=0)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::QuickSortDescending(theOrder);
  }
  inline void SetExpectedSize(int expectedSize)
  { this->::HashTemplate<Object, List<Object>, hashFunction>::SetExpectedSize(expectedSize);
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
  friend std::ostream& operator<< (std::ostream& output, const Matrix<Element>& theMat)
  { output << theMat.ToString();
    return output;
  }
//  friend std::iostream& operator>> <Element>(std::iostream& input, Matrix<Element>& theMat);
public:
  int NumRows; int ActualNumRows;
  int NumCols; int ActualNumCols;
  Element** elements;
  static bool flagComputingDebugInfo;
  void init(int r, int c);
  void ReleaseMemory();
  bool IsPositiveDefinite();
  bool IsNonNegativeAllEntries()
  { for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        if (this->elements[i][j]<0)
          return false;
    return true;
  }
  void Resize(int r, int c, bool PreserveValues, const Element* TheRingZero=0);
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
        tempMat.elements[j][i]=this->elements[i][j];
    *this=tempMat;
  }
  void AppendMatrixOnTheRight(const Matrix<Element>& standsOnTheRight)
  { if (&standsOnTheRight==this)
    { Matrix<Element> copyThis=*this;
      this->AppendMatrixOnTheRight(copyThis);
      return;
    }
    if (standsOnTheRight.NumRows<this->NumRows)
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
  void AppendMatrixToTheBottom(const Matrix<Element>& standsBelow)
  { if (&standsBelow==this)
    { Matrix<Element> copyThis=*this;
      this->AppendMatrixToTheBottom(copyThis);
      return;
    }
    if (standsBelow.NumCols<this->NumCols)
    { Element theZero;
      theZero=0;
      Matrix<Element> standsBelowNew=standsBelow;
      standsBelowNew.Resize(standsBelow.NumRows, this->NumCols, true, &theZero);
      this->AppendMatrixOnTheRight(standsBelowNew);
      return;
    }
    if (this->NumCols<standsBelow.NumCols)
    { Element theZero;
      theZero=0;
      this->Resize(this->NumRows, standsBelow.NumCols, true, &theZero);
    }
    //So far, we have guaranteed that this and &standsBelow have the same number of columns and
    // are different objects.
    int oldNumRows=this->NumRows;
    this->Resize(this->NumRows+standsBelow.NumRows, this->NumCols, true);
    for (int i=oldNumRows; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j]=standsBelow(i-oldNumRows, j);
  }
  static void MatrixInBasis(const Matrix<Element>& input, Matrix<Element>& output, const List<Vector<Element> >& basis, const Matrix<Element>& gramMatrixInverted)
  { int d = basis.size;
    output.init(d, d);
    Vector<Rational> tempV;
    for(int j=0; j<d; j++)
    { input.ActOnVectorColumn(basis[j], tempV);
      for(int i=0; i<d; i++)
        output.elements[i][j] = basis[i].ScalarEuclidean(tempV);
    }
    output.MultiplyOnTheLeft(gramMatrixInverted);
  }
  void ComputeDeterminantOverwriteMatrix(Element& output, const Element& theRingOne=1, const Element& theRingZero=0);
  void ActOnVectorROWSOnTheLeft(List<Vector<Element> >& standOnTheRightAsVectorRow)const
  { List<Vector<Element> > output;
    this->ActOnVectorROWSOnTheLeft(standOnTheRightAsVectorRow, output);
    standOnTheRightAsVectorRow=output;
  }
  void ActOnVectorROWSOnTheLeft(List<Vector<Element> >& standOnTheRightAsVectorRow, List<Vector<Element> >& output)const
  { if (this->NumCols!=standOnTheRightAsVectorRow.size)
    { std::cout << "This is a programming error: attempting to multiply a matrix, standing on the left, with "
      << this->NumCols << " columns, by a matrix, standing on the right, with " << standOnTheRightAsVectorRow.size << " rows. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    output.SetSize(this->NumRows);
    for (int i=0; i<this->NumRows; i++)
    { output[i].MakeZero(standOnTheRightAsVectorRow[0].size);
      for (int j=0; j<this->NumCols; j++)
        output[i]+= standOnTheRightAsVectorRow[j]*(*this)(i,j);
    }
  }
  void ActMultiplyVectorRowOnTheRight(Vector<Element>& standsOnTheLeft, const Element& TheRingZero=0)const
  { Vector<Element> output;
    this->ActMultiplyVectorRowOnTheRight(standsOnTheLeft, output, TheRingZero);
    standsOnTheLeft=output;
  }
  void ActMultiplyVectorRowOnTheRight(const Vector<Element>& standsOnTheLeft, Vector<Element>& output, const Element& TheRingZero=0)const
  { assert(&standsOnTheLeft!=&output);
    assert(this->NumRows==standsOnTheLeft.size);
    output.MakeZero(this->NumCols, TheRingZero);
    Element tempElt;
    for (int i=0; i<this->NumCols; i++)
      for (int j=0; j<this->NumRows; j++)
      { tempElt=this->elements[j][i];
        tempElt*=standsOnTheLeft[j];
        output[i]+=tempElt;
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
    { std::cout << "This is a programming error: attempting to act by " << this->ToString() << "(an " << this->NumRows << " x "
      << this->NumCols << " matrix) on a column vector " << input.ToString() << "(dimension " << input.size << ").";
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
  void ActOnMonomialAsDifferentialOperator(const MonomialP& input, Polynomial<Rational>& output);
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
  inline Element ScalarProduct(const Vector<Element>& left, const Vector<Element>& right)
  { return this->ScalarProduct(left, right, (Element) 0);
  }
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
  { if (i<0 || i>=this->NumRows || j<0 || j>=this->NumCols)
    { std::cout << "This is a programming error: requesting row, column indexed by " << i+1 << " and " << j+1 << " but I am a matrix with "
      << this->NumRows << " rows and " << this->NumCols << " colums. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
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
  void GetVectorFromColumn(int colIndex, Vector<Element>& output)const;
  void GetVectorFromRow(int rowIndex, Vector<Element>& output)const;
  int FindPivot(int columnIndex, int RowStartIndex);
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
    A.AssignVectorsToRows(domain);
    B.AssignVectorsToRows(range);
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
  void AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar, GlobalVariables* theGlobalVariables=0);
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
  void NonPivotPointsToEigenVectorMatrixForm(Selection& TheNonPivotPoints, Matrix<Element>& output);
  void GetVectorsFromRows(List<Vector<Element> >& output)
  { output.SetSize(this->NumRows);
    for (int i=0; i<this->NumRows; i++)
    { output[i].SetSize(this->NumCols);
      for (int j=0; j<this->NumCols; j++)
        output[i][j]=this->elements[i][j];
    }
  }
  void NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, Vector<Element>& output, const Element& theRingUnit=1, const Element& theRingZero=0);
  //returns true if successful, false otherwise
//  bool ExpressColumnAsALinearCombinationOfColumnsModifyMyself
//    (Matrix<Element>& inputColumn, Matrix<Element>* outputTheGaussianTransformations Matrix<Element>& outputColumn);
  bool Invert(GlobalVariables* theGlobalVariables=0);
  void NullifyAll(const Element& theRingZero=0);
  //if m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below function
  //corresponds to the linear operator from V1+W1 to V2+W2 (direct sum)
  //this means you write the matrix m1 in the upper left corner m2 in the lower right
  // and everything else you fill with zeros
  void AssignDirectSum(Matrix<Element>& m1,  Matrix<Element>& m2);
  // if S and T are endomorphisms of V and W, build the matrix of SⓧT that acts on
  // VⓧW with basis (v1ⓧw1,v1ⓧw2,...,v2ⓧw1,v2ⓧw2,...vnⓧwn)
  void AssignTensorProduct(const Matrix<Element>& left, const Matrix<Element>& right);
  void AssignVectorsToRows(const Vectors<Element>& input)
  { int numCols=-1;
    if (input.size>0)
      numCols=input[0].size;
    this->init(input.size, numCols);
    for (int i=0; i<input.size; i++)
      for (int j=0; j<numCols; j++)
        this->elements[i][j]=input[i][j];
  }
  void AssignVectorsToColumns(const Vectors<Element>& input)
  { this->init(input[0].size, input.size);
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        (*this)(i,j)=input[j][i];
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
    { std::cout << "This is a programming error: attempting to subtract from matrix with " << this->NumRows << " rows and "
      << this->NumCols << " columns " << " a matrix with " << right.NumRows << " rows and " << right.NumCols << " columns. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j].Subtract(right.elements[i][j]);
  }
  void WriteToFile(std::fstream& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->WriteToFile(output);
  }
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { return this->ReadFromFile(input);
  }
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
  void operator*=(const Matrix<Element>& input)
  { this->MultiplyOnTheRight(input);
  }
  void LieBracketWith(Matrix<Element>& right)
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
  void GaussianEliminationByRows
  (Matrix<Element>* carbonCopyMat=0, Selection* outputNonPivotColumns=0, Selection* outputPivotColumns=0, GlobalVariables* theGlobalVariables=0);
  void GaussianEliminationByRowsNoRowSwapPivotPointsByRows(int firstNonProcessedRow, Matrix<Element>& output, List<int>& outputPivotPointCols, Selection* outputNonPivotPoints__WarningSelectionNotInitialized);
  void GaussianEliminationEuclideanDomain
  (Matrix<Element>* otherMatrix=0, const Element& theRingMinusUnit=-1, const Element& theRingUnit=1,
   bool (*comparisonGEQFunction) (const Element& left, const Element& right)=0, GlobalVariables* theGlobalVariables=0);
  static bool Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output);
  Element GetDeterminant();
  Element GetTrace()const;
  void Transpose(GlobalVariables& theGlobalVariables){this->Transpose();}
  void AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den);
  void ScaleToIntegralForMinRationalHeightNoSignChange();
  void GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen);
  void LieBracketWith(const Matrix<Element>& right);
  void ApproximateLargestEigenSpace(Vectors<Rational>& outputBasis, const Rational& DesiredError, int SuggestedOrder, int numIterations);
  void MatrixToRoot(Vector<Rational> & output);
  bool GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(List<Vectors<Element> >& output)const;
  void GetZeroEigenSpace(List<Vector<Element> >& output)const
  { Matrix<Element> tempMat=*this;
    tempMat.GetZeroEigenSpaceModifyMe(output);
  }
  void GetZeroEigenSpaceModifyMe(List<Vector<Element> >& output);
  void GetEigenspaceModifyMe(const Element &inputEigenValue, List<Vector<Element> >& outputEigenspace)
  { for(int i=0; i<this->NumCols; i++)
      this->elements[i][i] -= inputEigenValue;
    this->GetZeroEigenSpaceModifyMe(outputEigenspace);
  }
  static bool SystemLinearInequalitiesHasSolution(Matrix<Element>& matA, Matrix<Element>& matb, Matrix<Element>& outputPoint);
  static bool SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(Matrix<Element>& matA, Matrix<Element>& matb, Vector<Element>* outputSolution=0, GlobalVariables* theGlobalVariables=0);
  static void ComputePotentialChangeGradient(Matrix<Element>& matA, Selection& BaseVariables, int NumTrueVariables, int ColumnIndex, Rational& outputChangeGradient, bool& hasAPotentialLeavingVariable);
  static void GetMaxMovementAndLeavingVariableRow
  (Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables, Matrix<Element>& tempMatA, Vector<Element>& inputVectorX,
   Selection& BaseVariables);
  int FindPositiveLCMCoefficientDenominatorsTruncated();
  int FindPositiveGCDCoefficientNumeratorsTruncated();
  Matrix<Element> operator-(const Matrix<Element>& right)const
  { Matrix<Element> tempMat;
    tempMat.Assign(*this);
    tempMat.Subtract(right);
    return tempMat;
  }
  Matrix<Element> operator*(const Matrix<Element>& right)const;
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

class Selection
{
public:
  List<int> elements;
  List<bool> selected;
  int MaxSize;
  int CardinalitySelection;
  inline static std::string GetXMLClassName()
  { return "Selection";
  }
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
  bool IncrementReturnFalseIfBackToBeginning()
  { this->incrementSelection();
    if (this->CardinalitySelection==0)
      return false;
    return true;
  }
  int SelectionToIndex();
  void ExpandMaxSize();
  int GetNumCombinationsFixedCardinality(int theCardinality){return MathRoutines::NChooseK(this->MaxSize, theCardinality);}
  void ShrinkMaxSize();
  void MakeSubSelection(Selection& theSelection, Selection& theSubSelection);
  void initSelectionFixedCardinality(int card);
  void incrementSelectionFixedCardinality(int card);
  void WriteToFile(std::fstream& output);
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables){this->WriteToFile(output);}
  void ReadFromFile(std::fstream& input);
  void InvertSelection()
  { for (int i=0; i<this->MaxSize; i++)
      this->selected[i]=!this->selected[i];
    this->ComputeIndicesFromSelection();
  }
  inline void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){ this->ReadFromFile(input);}
  void operator=(const Selection& right);
  void operator=(const Vector<Rational>& other);
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
  { *this=other;
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
  static unsigned int HashFunction(const SelectionWithMultiplicities& input)
  { unsigned int result=0;
    for (int i=0; i<input.elements.size; i++)
      result+=input.Multiplicities[input.elements[i]]*SomeRandomPrimes[input.elements[i]];
    return result;
  }
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
  LargeInt GetNumTotalCombinations()const;
  int NumSelectionsTotal()
  { return MathRoutines::KToTheNth(MaxMultiplicity, this->Multiplicities.size);
  }
  bool IncrementReturnFalseIfBackToBeginning();
  void IncrementSubset();
  void IncrementSubsetFixedCardinality(int Cardinality);
  bool HasMultiplicitiesZeroAndOneOnly();
  int MaxCardinalityWithMultiplicities()
  { return this->MaxMultiplicity*this->Multiplicities.size;
  }
  int CardinalitySelectionWithMultiplicities();
  static unsigned int HashFunction(const SelectionWithMaxMultiplicity& input)
  { return input.MaxMultiplicity*SomeRandomPrimes[0]+
    input.::SelectionWithMultiplicities::HashFunction(input);
  }
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
bool Matrix<Element>::Invert(GlobalVariables* theGlobalVariables)
{ if (this->NumCols!=this->NumRows)
  { std::cout << "This is a programming error: requesting to invert a non-square matrix of " << this->NumRows << " rows and "
    << this->NumCols << " columns. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  MacroRegisterFunctionWithName("Matrix::Invert");
//  if (this->flagComputingDebugInfo)
//    this->ComputeDebugString();
  Matrix theInverse;
  theInverse.MakeIdMatrix(this->NumRows);
  Selection NonPivotCols;
  this->GaussianEliminationByRows(&theInverse, &NonPivotCols, 0, theGlobalVariables);
  if(NonPivotCols.CardinalitySelection!=0)
    return false;
  *this=theInverse;
  return true;
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(const Matrix<Element>& standsOnTheLeft, const Element& theRingZero)
{ Matrix<Element> tempMat;
  this->MultiplyOnTheLeft(standsOnTheLeft, tempMat, theRingZero);
  this->operator=(tempMat);
}

template <typename Element>
void Matrix<Element>::MultiplyOnTheLeft(const Matrix<Element>& standsOnTheLeft, Matrix<Element>& output, const Element& theRingZero)
{ if (&output==this || &output==&standsOnTheLeft)
  { Matrix<Element> thisCopy=*this;
    Matrix<Element> standsOnTheLeftCopy=standsOnTheLeft;
    thisCopy.MultiplyOnTheLeft(standsOnTheLeftCopy, output, theRingZero);
    return;
  }
  if (this->NumRows!=standsOnTheLeft.NumCols)
  { std::cout << "This is a programming error: attempting to multiply a matrix with " << standsOnTheLeft.NumCols
    << " columns by a matrix with " << this->NumRows << "rows. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Element tempEl;
  output.init(standsOnTheLeft.NumRows, this->NumCols);
  for (int i=0; i< standsOnTheLeft.NumRows; i++)
    for(int j=0; j< this->NumCols; j++)
    { output.elements[i][j]=theRingZero;
      for (int k=0; k<this->NumRows; k++)
      { tempEl=standsOnTheLeft(i,k);
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
void Matrix<Element>::AssignTensorProduct(const Matrix<Element>& left, const Matrix<Element>& right)
{ //handle lazy programmers:
  if (this==&left || this==&right)
  { Matrix<Element> leftCopy=left;
    Matrix<Element> rightCopy=right;
    this->AssignTensorProduct(leftCopy, rightCopy);
    return;
  }
  this->Resize(left.NumRows*right.NumRows, left.NumCols*right.NumCols, false);
  int sr = right.NumRows; int sc = right.NumCols;
  //The basis of the tensor product vector space  MUST be in the SAME order as the one used by MatrixTensor::AssignTensorProduct.
  //indexing. Let the first vector space have basis v_1, ..., v_k, the second: w_1, ..., w_m.
  //Then the basis of the tensor product is given in the order: v_1\otimes w_1, ..., v_1\otimes w_m,
  //..., v_k\otimes w_1, ..., v_k\otimes w_m
  for(int iv = 0; iv<left.NumRows; iv++)
    for(int iw = 0; iw<right.NumRows; iw++)
      for(int jv = 0; jv<left.NumCols; jv++)
        for(int jw = 0; jw <right.NumCols; jw++)
          this->elements[iv*sr+iw][jv*sc+jw] = left.elements[iv][jv] * right.elements[iw][jw];
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
inline int Matrix<Element>::FindPivot(int columnIndex, int RowStartIndex)
{ for(int i = RowStartIndex; i<this->NumRows; i++)
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
  A.GaussianEliminationByRows(&b, 0, &thePivotPoints);
  return A.RowEchelonFormToLinearSystemSolution(thePivotPoints, b, output);
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
    { outputSolution(i,0)=inputRightHandSide(NumPivots,0);
      NumPivots++;
    }
    else
      outputSolution(i,0).MakeZero();
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

class LargeIntUnsigned
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
  List<unsigned int> theDigits;
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
  inline std::string ToString(FormatExpressions* theFormat=0)const
  { std::string tempS;
    this->ToString(tempS);
    return tempS;
  }
  void DivPositive(const LargeIntUnsigned& x, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const;
  void MakeOne();
  void AddUInt(unsigned int x)
  { LargeIntUnsigned tempI;
    tempI.AssignShiftedUInt(x, 0);
    (*this)+=tempI;
  }
  void MakeZero();
  bool IsEqualToZero()const;
  bool IsEven()const;
  bool IsPositive()const;
  bool IsEqualToOne()const;
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
  static void lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output);
  unsigned int HashFunction()const;
  void MultiplyBy(const LargeIntUnsigned& right);
  inline void operator*=(const LargeIntUnsigned& right)
  { this->MultiplyBy(right);
  }
  inline void operator*=(unsigned int x)
  { this->MultiplyByUInt(x);
  }
  inline void operator+=(unsigned int x)
  { this->AddUInt(x);
  }
  LargeIntUnsigned operator+(const LargeIntUnsigned& other)
  { LargeIntUnsigned result=*this;
    result+=other;
    return result;
  }
  inline void operator++(int)
  { this->AddUInt(1);
  }
  void AssignFactorial(unsigned int x){ this->AssignFactorial(x, 0);}
  void AssignFactorial(unsigned int x, GlobalVariables* theGlobalVariables);
  void MultiplyBy(const LargeIntUnsigned& x, LargeIntUnsigned& output)const;
  void MultiplyByUInt(unsigned int x);
  void AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift);
  void AssignShiftedUInt(unsigned int x, int shift);
  inline void AccountPrimeFactor(unsigned int theP, List<unsigned int>& outputPrimeFactors, List<int>& outputMults)
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
  void AssignString(const std::string& input);
  int GetUnsignedIntValueTruncated();
  LargeIntUnsigned operator%(const LargeIntUnsigned& other)const
  { LargeIntUnsigned result, temp;
    this->DivPositive(other, temp, result);
    return result;
  }
  LargeIntUnsigned operator-(const LargeIntUnsigned& other)const;
  int operator%(unsigned int x);
  void operator=(const LargeIntUnsigned& x);
  void operator=(unsigned int x);
  void operator+=(const LargeIntUnsigned& other);
  bool operator==(const LargeIntUnsigned& other)const;
  bool operator!=(const LargeIntUnsigned& other)const;
  inline void operator--(int)
  { if (this->IsEqualToZero())
    { std::cout << "This is a programming error: attempting to subtract 1 from a large unsigned integer with value 0. "
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
  LargeIntUnsigned operator*(const LargeIntUnsigned& x)const
  { LargeIntUnsigned result;
    this->MultiplyBy(x, result);
    return result;
  }
  LargeIntUnsigned(unsigned int x);
  LargeIntUnsigned(const LargeIntUnsigned& x);
  LargeIntUnsigned();
//  LargeIntUnsigned(unsigned int value){this->operator=(value); }
//  LargeIntUnsigned(unsigned int x) {this->AssignShiftedUInt(x,0);}
  static inline LargeIntUnsigned GetOne()
  { LargeIntUnsigned tempI;
    tempI.MakeOne();
    return tempI;
  }
  inline bool operator<(const LargeIntUnsigned& other)const
  { return !this->IsGEQ(other);
  }
  bool operator>(const LargeIntUnsigned& other)const
  { return other<*this;
  }
  //must be rewritten:
  double GetDoubleValue();
  void FitSize();
};

class LargeInt
{ friend class Rational;
  friend LargeInt operator*(const LargeInt& left, const LargeInt& right)
  { LargeInt tempI;
    tempI=left;
    tempI*=right;
    return tempI;
  }
  friend LargeInt operator*(const LargeInt& left, const LargeIntUnsigned& right)
  { LargeInt tempI;
    tempI=right;
    return left*tempI;
  }
public:
  inline static const std::string GetXMLClassName()
  { return "LInt";
  }
  signed char sign;
  LargeIntUnsigned value;
  void operator*=(const LargeInt& x)
  { this->sign*=x.sign;
    this->value.MultiplyBy(x.value);
  }
  void MultiplyByInt(int x);
  void ToString(std::string& output)const;
  inline std::string ToString(FormatExpressions* theFormat=0)const
  { std::string tempS;
    this->ToString(tempS);
    return tempS;
  }
  bool IsPositive()const
  { return this->sign==1 && (this->value.IsPositive());
  }
  bool IsNegative()const
  { return this->sign==-1&& (this->value.IsPositive());
  }
  inline bool BeginsWithMinus()
  { return this->IsNegative();
  }
  inline bool IsPositiveOrZero()const
  { return !this->IsNegative();
  }
  inline bool IsNonPositive()const
  { return !this->IsPositive();
  }
  bool NeedsBrackets()const
  { return false;
  }
  bool operator==(const LargeInt& x)const;
  bool IsEqualToZero()const
  { return this->value.IsEqualToZero();
  }
  bool IsEven()const;
  bool IsEqualToOne()const
  { return this->value.IsEqualToOne() && this->sign==1;
  }
  void AssignLargeIntUnsigned(const LargeIntUnsigned& x)
  { this->value=x;
    this->sign=1;
  }
  void AssignInt(int x);
  void AddLargeIntUnsigned(const LargeIntUnsigned& x);
  inline void AddInt(int x)
  { LargeInt tempInt;
    tempInt.AssignInt(x);
    *this+=(tempInt);
  }
  //bool IsGEQ(LargeInt& x);
  bool CheckForConsistensy();
  void WriteToFile(std::fstream& output);
  void AssignString(const std::string& input);
  void ReadFromFile(std::fstream& input);
  void checkConsistency(){}
  void MakeZero();
  bool GetDivisors(List<int>& output, bool includeNegative);
  void MakeOne()
  { this->value.MakeOne();
    this->sign=1;
  }
  void MakeMOne()
  { this->value.MakeOne();
    this->sign=-1;
  }
  static unsigned int HashFunction (const LargeInt& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->value.HashFunction()+this->sign+3;
  }
  int GetIntValueTruncated()
  {return this->sign* this->value.GetUnsignedIntValueTruncated();
  }
  double GetDoubleValue();
  int operator %(int x);
  inline void AssignFloor(){}
  void operator=(const Rational& x);
  void operator=(const LargeInt& x);
  inline void operator=(int x)
  { this->AssignInt(x);
  }
  inline void operator=(const LargeIntUnsigned& other)
  { this->value=other;
    this->sign=1;
  }
  void operator*=(const LargeIntUnsigned& other)
  { if (other.IsEqualToZero())
    { this->MakeZero();
      return;
    }
    this->value*=other;
  }
  inline void operator*=(int x)
  { this->MultiplyByInt(x);
  }
  inline void Minus()
  { if (!this->IsEqualToZero())
      this->sign*=-1;
  }
  void operator+=(const LargeInt& other);
  inline void operator+=(const LargeIntUnsigned& other)
  { this->AddLargeIntUnsigned(other);
  }
  inline void operator+=(int x)
  { this->AddInt(x);
  }
  inline bool operator!=(const LargeInt& other)const
  { return !(*this==other);
  }
  inline void operator-=(const LargeInt& other)
  { this->Minus();
    *this+=(other);
    this->Minus();
  }
  inline bool operator<=(const LargeInt& other)const
  { return !(other<*this);
  }
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
  inline LargeInt operator+(const LargeInt& other)const
  { LargeInt tempInt;
    tempInt=(*this);
    tempInt+=(other);
    return tempInt;
  }
  inline LargeInt operator-(const LargeInt& other)const
  { LargeInt result=*this;
    result-=(other);
    return result;
  }
  inline LargeInt operator+(int x)const
  { LargeInt result=*this;
    result.AddInt(x);
    return result;
  }
  LargeInt operator*(int x)
  { LargeInt result=*this;
    result.MultiplyByInt(x);
    return result;
  }
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
  inline bool operator>(const LargeInt& other)const
  { return other<*this;
  }
  void operator%=(const LargeInt& other)
  { if (this->IsEqualToZero())
      return;
    LargeIntUnsigned quotient, remainder;
    this->value.DivPositive(other.value, quotient, remainder);
    this->value=remainder;
    if (this->IsNegative())
      *this+=other.value;
  }
  void RaiseToPower(int thePower)
  { MathRoutines::RaiseToPower(*this, thePower, (LargeInt) 1);
  }
  LargeInt operator %(const LargeInt& other)const
  { LargeInt result=*this;
    result%=other;
    return result;
  }
  LargeInt(const LargeInt& x)
  { this->operator=(x);
  }
  LargeInt(int x)
  { this->AssignInt(x);
  }
  LargeInt(const LargeIntUnsigned& other):sign(1), value(other)
  {}
  LargeInt(): sign(1){}
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
    { std::cout << "This is a programming error: corrupt rational number(s) with denominators " << this->DenShort << " and " << OtherDen
      << ". The cause of the error should be in some of " << " the calling functions. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
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
  bool ShrinkExtendedPartIfPossible();
public:
  inline static std::string GetXMLClassName(){ return "Rational";}
  int NumShort;
  //the requirement that the below be unsigned caused a huge problem, so I
  //changed it back to int. Grrrrr.
  int DenShort;
  LargeRationalExtended *Extended;
  static unsigned long long TotalSmallAdditions;
  static unsigned long long TotalLargeAdditions;
  static unsigned long long TotalSmallMultiplications;
  static unsigned long long TotalLargeMultiplications;
  static unsigned long long TotalSmallGCDcalls;
  static unsigned long long TotalLargeGCDcalls;
  void GetDenominator(Rational& output)
  { LargeIntUnsigned tempInt;
    this->GetDenominator(tempInt);
    output.AssignLargeIntUnsigned(tempInt);
  }
  bool NeedsBrackets()const
  { return false;
    //return this->IsNegative();
  }
  bool GetSquareRootIfRational(Rational& output)const;
  inline Rational GetDenominatorRationalPart()
  { return this->GetDenominator();
  }
  Rational GetNumeratorRationalPart()
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
      output=(this->Extended->den);
  }
  bool BeginsWithMinus()
  { return this->IsNegative();
  }
  inline LargeInt GetNumerator()const
  { LargeInt result;
    this->GetNumerator(result);
    return result;
  }
  inline const Rational& GetComplexConjugate()const
  { return *this;
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
      output=(this->Extended->num.value);
  }
  Rational RationalValue()
  { return *this;
  }
//  inline int GetNumValueTruncated(){return this->NumShort; };
//  inline int GetDenValueTruncated(){return this->denShort; };
//  static bool flagMinorRoutinesOnDontUseFullPrecision;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void Subtract(const Rational& r);
  void AdD(const Rational& r)
  { this->operator+=(r);
  }
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
    this->Extended->num=other;
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
    this->NumShort=n;
    this->DenShort=d;
    this->FreeExtended();
    this->Simplify();
  }
  void DivideBy(const Rational& r);
  void DivideByInteger(int x)
  { int tempDen; signed char tempSign;
    if (x<0)
    { tempDen=(-x);
      tempSign=-1;
    } else
    { tempDen=x;
      tempSign=1;
    }
    if (this->TryToMultiplyQuickly(tempSign, tempDen))
      return;
    this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyByUInt(((unsigned int)tempDen));
    this->Extended->num.sign*=tempSign;
    this->Simplify();
  }
  void DivideByLargeInteger(LargeInt& x)
  { this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyBy(x.value);
    this->Extended->num.sign*=x.sign;
    this->Simplify();
  }
  void DivideByLargeIntegerUnsigned(LargeIntUnsigned& x)
  { this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyBy(x);
    this->Simplify();
  }
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
  void MakeZero()
  { this->NumShort=0;
    this->DenShort=1;
    this->FreeExtended();
  }
  void MakeOne()
  { this->NumShort=1;
    this->DenShort=1;
    this->FreeExtended();
  }
  void MakeMOne()
  { this->NumShort=-1;
    this->DenShort=1;
    this->FreeExtended();
  }
  void WriteToFile(std::fstream& output);
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->WriteToFile(output);
  }
  void ReadFromFile(std::istream& input);
  inline void ReadFromFile(std::istream& input, GlobalVariables* theGlobalVariables)
  { this->ReadFromFile(input);
  }
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData);
  inline void AssignAbsoluteValue()
  { if(this->IsNegative())this->Minus();
  }
  static Rational NChooseK(const Rational& n, int k);
  static Rational Factorial(int n, GlobalVariables* theGlobalVariables);
  static Rational Factorial(int n)
  { return Rational::Factorial(n,0);
  }
  static Rational TwoToTheNth(int n);
  static Rational NtoTheKth(int n, int k);
  void RaiseToPower(int x);
  //don't ever call the below manually or you can get memory leak (extended must be nullified here by
  //default!
  Rational(int n, int d)
  { this->Extended=0;
    this->AssignNumeratorAndDenominator(n, d);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const LargeInt& other)
  { this->Extended=0;
    *this=other;
  }
  Rational(const LargeIntUnsigned& other)
  { this->Extended=0;
    *this=other;
  }
  Rational(): NumShort(0), DenShort(0), Extended(0)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(int n)
  { this->Extended=0;
    this->AssignNumeratorAndDenominator(n, 1);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const Rational& right)
  { this->Extended=0; this->Assign(right);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
//  Rational(int x){this->Extended=0; this->AssignInteger(x); };
  ~Rational(){this->FreeExtended();}
  //the below must be called only with positive arguments!
  static inline int gcd(int a, int b)
  { MacroIncrementCounter(Rational::TotalSmallGCDcalls);
    return MathRoutines::gcd(a, b);
  }
  static int gcdSigned(int a, int b)
  { if (a<0)
      a*=-1;
    if (b<0)
      b*=-1;
    return Rational::gcd(a, b);
  }
  inline bool CheckForElementSanity()
  { return this->Extended==0;
  }
  inline bool checkConsistency()
  { return this->ConsistencyCheck();
  }
  inline bool ConsistencyCheck()
  { if (this->Extended==0)
      return this->DenShort>0;
    return true;
  }
  void operator=(const AlgebraicNumber& other);
  inline void operator=(const LargeIntUnsigned& right)
  { LargeInt tempI;
    tempI=right;
    this->operator=(tempI);
  }
  inline void operator=(const Rational& right)
  { this->Assign(right);
  }
  void operator=(const Polynomial<Rational>& other);
  inline bool operator==(const int other)
  { if (other==0)
      return this->IsEqualToZero();
    Rational tempRat=other;
    return this->IsEqualTo(other);
  }
  inline bool operator==(const Rational& right)const
  { return this->IsEqualTo(right);
  }
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
    tempI=tempRat.Extended->num;
    tempI.value.MultiplyBy(this->Extended->den);
    this->Extended->num.value.MultiplyBy(tempRat.Extended->den);
    this->Extended->num+=(tempI);
    this->Extended->den.MultiplyBy(tempRat.Extended->den);
    this->Simplify();
  }
  inline void operator-=(const Rational& right)
  { this->Subtract(right);
  }
  inline void operator*=(const Rational& right)
  { this->MultiplyBy(right);
  }
//  inline void operator*=(const Polynomial<Rational>& right);
  inline void operator/=(const Rational& right)
  { this->DivideBy(right);
  }
//  inline void operator/=(const LargeInt& right){Rational tempRat; tempRat=right; this->DivideBy(tempRat);}
//  inline void operator/=(const LargeIntUnsigned& right){Rational tempRat; tempRat=right; this->DivideBy(tempRat);}
  inline void operator+=(int right)
  { this->AddInteger(right);
  }
  inline void operator-=(int right)
  { Rational tempRat=right;
    this->Subtract(tempRat);
  }
//  inline bool operator==(int right){Rational tempRat; tempRat.AssignInteger(right); return this->IsEqualTo(tempRat); }
  inline void operator=(int right)
  { this->AssignInteger(right);
  }
  inline void operator=(const LargeInt& other)
  { this->AssignLargeInteger(other);
  }
  Rational operator*(const Rational& right)const;
//  { Rational result=*this;
//    result*=right;
//    return result;
//  }
  Rational operator*(int right)const
  { Rational tempRat;
    tempRat.Assign(*this);
    tempRat.MultiplyByInt(right);
    return tempRat;
  }
  Rational operator/(int right)const
  { Rational tempRat;
    tempRat.Assign(*this);
    tempRat.DivideByInteger(right);
    return tempRat;
  }
  Vector<Rational>  operator*(const Vector<Rational> & right)const;
  Rational operator+(const Rational& right)const;
  Rational operator-(const Rational& right)const;
  Rational operator/(const Rational& right)const;
  inline bool operator!=(const Rational& right)const
  { return !this->IsEqualTo(right);
  }
  inline bool operator!=(const int& right)const
  { return !((*this)==right);
  }
  inline bool operator>(const Rational& right)const
  { return this->IsGreaterThan(right);
  }
  inline bool operator<(const Rational& right)const
  { return right.IsGreaterThan(*this);
  }
  inline bool operator>(const int right)const
  { Rational tempRat;
    tempRat.AssignInteger(right);
    return this->IsGreaterThan(tempRat);
  }
  inline bool operator>=(const Rational& right)const
  { return this->IsGreaterThanOrEqualTo(right);
  }
  inline bool operator<(const int right)const
  { Rational tempRat;
    tempRat.AssignInteger(right);
    return tempRat.IsGreaterThan(*this);
  }
};

template<class coefficient>
Vector<coefficient> operator-(const Vector<coefficient>& input)
{ Vector<coefficient> result;
  result.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    result[i]=-input[i];
  return result;
}

template <class coefficient>
std::ostream& operator<<(std::ostream& out, const Vector<coefficient>& theVector)
{ out  << "(";
  for (int i=0; i<theVector.size; i++)
  { out << theVector[i];
    if (i!=theVector.size-1)
      out << ", ";
  }
  out << ")";
  return out;
}

template <class coefficient>
class Vector: public List<coefficient>
{ friend Vector<coefficient> operator-<coefficient>(const Vector<coefficient>& input);
  friend std::ostream& operator<< <coefficient>(std::ostream& output, const Vector<coefficient>& theVector);
public:
  Vector(){}
  template <class otherCoeff>
  Vector(const Vector<otherCoeff>& other)
  { *this=other;
  }
  Vector(const Selection& other) {*this=other;}
  inline static std::string GetXMLClassName()
  { std::string result="Vector_" + coefficient::GetXMLClassName();
    return result;
  }
  std::string ToString(FormatExpressions* theFormat=0)const
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
  std::string ToStringLetterFormat(const std::string& inputLetter, FormatExpressions* theFormat=0, bool DontIncludeLastVar=false)const;
  std::string ToStringEpsilonFormat()const
  { return this->ToStringLetterFormat("\\varepsilon");
  }
  template <class otherType>
  static void ScalarProduct(const Vector<coefficient>& r1, const Vector<coefficient>& r2, const Matrix<otherType>& TheBilinearForm, coefficient& result)
  { if (r1.size!=TheBilinearForm.NumRows || r1.size!=r2.size || r1.size!=TheBilinearForm.NumCols)
    { std::cout << "This is a programming error: attempting to take a bilinear form represented by matrix with "
      << TheBilinearForm.NumRows << " rows and " << TheBilinearForm.NumCols << " columns "
      << " of vectors of dimension " << r1.size << " and " << r2.size << ". "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    coefficient tempRat, accumRow;
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
  }
  template <class otherType>
  static coefficient ScalarProduct(const Vector<coefficient>& r1, const Vector<coefficient>& r2, const Matrix<otherType>& TheBilinearForm)
  { coefficient result;
    Vector<coefficient>::ScalarProduct(r1, r2, TheBilinearForm, result);
    return result;
  }
  void PerturbNoZeroScalarProductWithMe(const List<Vector<coefficient> >& inputVectors);
  void PerturbNormalRelativeToVectorsInGeneralPosition(const Vectors<Rational>& NonStrictConeNonPositiveScalar, const List<Vector<Rational> >& VectorsToBeInGeneralPosition);
  coefficient ScalarProduct (const Vector<coefficient>& r2, const Matrix<coefficient>& form) const
  { return ScalarProduct(*this,r2,form);
  }
  bool IsPositive()
  { return this->IsPositiveOrZero() && !this->IsEqualToZero();
  }
  bool IsNegative()
  { return this->IsNegativeOrZero() && !this->IsEqualToZero();
  }
  Vector<Rational> GetVectorRational()const
  { Vector<Rational> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      result[i]=(*this)[i].RationalValue();
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
  static Vector<coefficient> GetEi(int theDimension, int theIndex)
  { Vector<coefficient> output;
    output.MakeEi(theDimension, theIndex);
    return output;
  }
  bool IsNegativeOrZero()
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]>0)
        return false;
    return true;
  }
  bool IsPositiveOrZero()const
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]<0)
        return false;
    return true;
  }
  coefficient SumCoords()const
  { coefficient result=0;
    for (int i=0; i<this->size; i++)
      result+=this->TheObjects[i];
    return result;
  }
  inline coefficient ScalarEuclidean(const Vector<coefficient>& other)const
  { coefficient output;
    this->ScalarEuclidean(other, output);
    return output;
  }
  void ScalarEuclidean(const Vector<coefficient>& other, coefficient& output)const
  { if (this->size!=other.size)
    { std::cout << "This is a programming error: taking scalar product of elements of different dimensions: "
      << this->ToString() << " and " << other.ToString() << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    coefficient tempElt;
    output=0;
    for (int i=0; i<this->size; i++)
    { tempElt=other[i];
      tempElt*=this->TheObjects[i];
      output+=tempElt;
    }
  }
  static void ProjectOntoHyperPlane(Vector<coefficient>& input, Vector<coefficient>& normal, Vector<coefficient>& ProjectionDirection, Vector<coefficient>& output)
  { coefficient t;
    coefficient tempRat;
    Vector<coefficient>::ScalarEuclidean(input, normal, t);
    Vector<coefficient>::ScalarEuclidean(ProjectionDirection, normal, tempRat);
    t/=(tempRat);
    t.Minus();
    Vector<coefficient>::VectorPlusVectorTimesScalar(input, ProjectionDirection, t, output);
  }
  static void VectorPlusVectorTimesScalar(const Vector<coefficient>& r1, const Vector<coefficient>& r2, const coefficient& theCoeff, Vector<coefficient>& output)
  { coefficient tempRat;
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
  void MakeEi(int DesiredDimension, int NonZeroIndex, const coefficient& theRingUnit=1, const coefficient& theRingZero=0)
  { this->MakeZero(DesiredDimension, theRingZero);
    (*this)[NonZeroIndex]=theRingUnit;
  }
  inline static unsigned int HashFunction(const Vector<coefficient>& input){return input.HashFunction();}
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
  { coefficient theElt;
    theElt=*this->LastObject();
    this->size--;
    this->operator/=(theElt);
  }
  void ScaleByPositiveRationalToIntegralMinHeight();
  bool AssignMatDetectRowOrColumn(const Matrix<coefficient>& input)
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
  void MakeZero(int theDim, const coefficient& theRingZero=0)
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
  bool IsProportionalTo(const Vector<coefficient>& other, coefficient& TimesMeEqualsOther)const;
  bool IsProportionalTo(const Vector<coefficient>& other)const
  { coefficient TimesMeEqualsOther;
    return this->IsProportionalTo(other, TimesMeEqualsOther);
  }
  int FindLCMDenominatorsTruncateToInt();
  void FindLCMDenominators(LargeIntUnsigned& output);
  inline Vector<coefficient> GetShiftToTheLeftOnePosition()
  { return this->GetShiftToTheLeft(1);
  }
  void MakeNormalInProjectivizationFromPointAndNormal(Vector<coefficient>& point, Vector<coefficient>& normal)
  { //the extra dimension is going to be the last dimension
    int newDimension= normal.size+1;
    this->SetSize(newDimension);
    this->RootScalarEuclideanRoot(normal, point, this->TheObjects[newDimension-1]);
    this->TheObjects[newDimension-1].Minus();
    for (int j=0; j<newDimension-1; j++)
      this->TheObjects[j]=normal[j];
  }
  bool ProjectToAffineSpace(Vector<coefficient> &output)
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
  Vector<coefficient> GetShiftToTheLeft(int numPositions)
  { Vector<coefficient> result;
    assert(numPositions<=this->size);
    result.SetSize(this->size-numPositions);
    for (int i=0; i<result.size; i++)
      result[i]=this->TheObjects[i+numPositions];
    return result;
  }
  inline void ShiftToTheLeftOnePos()
  { this->ShiftToTheLeft(1);
  }
  void ShiftToTheLeft(int numPositions)
  { assert(numPositions<=this->size);
    for (int i=0; i<this->size-numPositions; i++)
      this->TheObjects[i]=this->TheObjects[i+numPositions];
    this->size-=numPositions;
  }
  void ShiftToTheRightInsertZeroes(int numPositions, const coefficient& theRingZero)
  { assert(numPositions>=0);
    this->SetSize(this->size+numPositions);
    for (int i=this->size-1; i>=numPositions; i--)
      this->TheObjects[i]=this->TheObjects[i-numPositions];
    for(int i=0; i<numPositions; i++)
      this->TheObjects[i]=theRingZero;
  }
  void SetDimInsertZeroes(int newDim)
  { int oldDim=this->size;
    this->SetSize(newDim);
    for (int i=oldDim; i<newDim; i++)
      (*this)[i]=0;
  }
  bool GetIntegralCoordsInBasisIfTheyExist
  (const Vectors<coefficient>& inputBasis, Vector<coefficient>& output, Matrix<coefficient>& bufferMatGaussianEliminationCC, Matrix<coefficient>& bufferMatGaussianElimination,
   const coefficient& theRingUnit, const coefficient& theRingMinusUnit, const coefficient& theRingZero);
/*  template<class OtherType>
  void operator=(const Vector<OtherType>& other)
  { this->SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->TheObjects[i]=other.TheObjects[i];
  }*/
  void GetVectorsPerpendicularTo(Vectors<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero)
  { int Pivot=-1;
    if (!this->FindIndexFirstNonZeroCoordinateFromTheLeft(Pivot))
    { output.MakeEiBasis(this->size, theRingUnit, theRingZero);
      return;
    }
    output.SetSize(this->size-1);
    for (int i=0; i<this->size; i++)
      if (i!=Pivot)
      { Vector<coefficient>& current=output.TheObjects[i];
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
  bool GetCoordsInBasiS(const Vectors<coefficient>& inputBasis, Vector<coefficient>& output)const;
  Vector<coefficient> GetProjectivizedNormal(Vector<coefficient>& affinePoint);
  Vector<coefficient> operator*(const coefficient& other)const
  { Vector<coefficient> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
    { result[i]=this->TheObjects[i];
      result[i]*=other;
    }
    return result;
  }
  Vector<coefficient> operator/(const coefficient& other)const
  { if (other.IsEqualToZero())
    { std::cout << "This is a programming error: division by zero. Division by zero error are supposed to be handled at an earlier level. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    Vector<coefficient> result;
    result.SetSize(this->size);
    for (int i=0; i<this->size; i++)
    { result[i]=this->TheObjects[i];
      result[i]/=other;
    }
    return result;
  }
  void operator*=(const coefficient& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]*=other;
  }
  void operator/=(const coefficient& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]/=other;
  }
  template <class otherType>
  void operator+=(const Vector<otherType>& other)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i]+=other.TheObjects[i];
  }
  inline bool operator<(const Vector<coefficient>& other)const
  { return other>*this;
  }
  bool isGreaterThanLexicographic(const Vector<coefficient>& other)const
  { return this->::List<coefficient>::operator>(other);
  }
  inline bool operator>(const Vector<coefficient>& other)const
  { if (this->size!=other.size)
    { std::cout << "This is a programming error: comparing Vectors with different number of coordinates, namely, "
      << this->ToString() << " and " << other.ToString() << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    coefficient c1=0, c2=0;
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
    { std::cout << "This is a programming error: subtracting vectors with different dimensions. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    for (int i=0; i<this->size; i++)
      this->TheObjects[i]-=other[i];
  }
  Vector<coefficient> operator+(const Vector<coefficient>& right)const
  { Vector<coefficient> result=*this;
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
        this->AddOnTop(tempRat);
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
  template <class otherType>
  void operator=(const Vector<otherType>& other)
  { if (this==(Vector<coefficient>*)&other)
      return;
    this->SetSize(other.size);
    for(int i=0; i<other.size; i++)
      this->TheObjects[i]=other[i];
  }
  template <class otherType>
  void operator=(const List<otherType>& other)
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
  bool operator!=(const Vector<coefficient>& other)const
  { return ! (*this==other);
  }
  bool IsEqualToZero()const
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
  Vector<coefficient> operator-(const Vector<coefficient>& other)const
  { Vector<coefficient> result;
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

template <class coefficient>
void Vector<coefficient>::ScaleByPositiveRationalToIntegralMinHeight()
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

template <class coefficient>
void Vector<coefficient>::ScaleToFirstNonZeroCoordinatePositive()
{ for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i].IsPositive())
      return;
    if (this->TheObjects[i].IsNegative())
    { this->Minus();
      return;
    }
  }
}

template <class coefficient>
bool Vector<coefficient>::IsProportionalTo
(const Vector<coefficient>& input, coefficient& outputTimesMeEqualsInput)const
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

template <class coefficient>
void Vector<coefficient>::FindLCMDenominators(LargeIntUnsigned& output)
{ LargeIntUnsigned tempI, tempI2;
  output.MakeOne();
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].GetDenominator(tempI2);
    LargeIntUnsigned::gcd(output, tempI2, tempI);
    output.MultiplyBy(tempI2);
    output.DivPositive(tempI, output, tempI2);
  }
}

template <class coefficient>
int Vector<coefficient>::FindLCMDenominatorsTruncateToInt()
{ int result=1;
  for (int i=0; i<this->size; i++)
  { result = MathRoutines::lcm(result, this->TheObjects[i].DenShort);
    assert(this->TheObjects[i].Extended==0);
  }
  return result;
}

template <class coefficient>
class Vectors: public List<Vector<coefficient> >
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
  std::string ElementsToInequalitiesString(bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat)const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool LinearAlgebraForVertexComputation(Selection& theSelection, Vector<coefficient>& output, Matrix<coefficient>& buffer, Selection& NonPivotPointsBuffer)
  { if (this->size==0)
      return false;
    int theDimension=this->TheObjects[0].size;
    output.SetSize(theDimension);
    assert(theDimension -1== theSelection.CardinalitySelection);
    buffer.init((int)(theDimension-1), (int)theDimension);
    for (int i =0; i<theDimension-1; i++)
      for (int j=0; j<theDimension; j++)
        buffer.elements[i][j]=(this->Externalwalls[theSelection.elements[i]].normal[j]);
    buffer.GaussianEliminationByRows(0, NonPivotPointsBuffer);
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
  void AssignListListCoefficientType(const List<List<coefficient> >& input)
  { this->SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->TheObjects[i]=input[i];
  }
  void AssignListList(const List<Vectors<coefficient> >& input)
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
  int ArrangeFirstVectorsBeOfMaxPossibleRank(Matrix<coefficient>& bufferMat, Selection& bufferSel);
  static unsigned int HashFunction(const Vectors<coefficient>& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->::List<Vector<coefficient> >::HashFunction();
  }
  bool HasAnElementWithPositiveScalarProduct(const Vector<coefficient>& input)const
  { for (int i=0; i<this->size; i++)
      if (input.ScalarEuclidean(this->TheObjects[i]).IsPositive())
        return true;
    return false;
  }
  bool HasAnElementWithNegativeScalarProduct(const Vector<coefficient>& input)const
  { for (int i=0; i<this->size; i++)
      if (input.ScalarEuclidean(this->TheObjects[i]).IsNegative())
        return true;
    return false;
  }
  bool HasAnElementPerpendicularTo(const Vector<coefficient>& input)const
  { for (int i=0; i<this->size; i++)
      if (input.ScalarEuclidean(this->TheObjects[i]).IsEqualToZero())
        return true;
    return false;
  }
  bool PerturbVectorToRegular(Vector<coefficient>& inputOutput, GlobalVariables& theGlobalVariables);
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output)
  { output.init((int)OutputDimension, (int)theSelection.CardinalitySelection);
    this->SelectionToMatrix(theSelection, OutputDimension, output, 0);
  }
  void SelectionToMatrixAppend(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex)
  { for(int i=0; i<theSelection.CardinalitySelection; i++)
    { Vector<coefficient>& tempRoot=this->TheObjects[theSelection.elements[i]];
      for (int j=0; j<OutputDimension; j++)
        output.elements[StartRowIndex+i][j]=tempRoot[j];
    }
  }
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex)
  { for(int i=0; i<theSelection.CardinalitySelection; i++)
    { Vector<Rational>& tempRoot=this->TheObjects[theSelection.elements[i]];
      for (int j=0; j<OutputDimension; j++)
        output.elements[StartRowIndex+i][j]=tempRoot[j];
    }
  }
  void GetGramMatrix(Matrix<coefficient>& output, const Matrix<Rational>* theBilinearForm=0)const;
  void GetMatrixRootsToRows(Matrix<Rational>& output)const
  { int tempNumCols= 0;
    if (this->size!=0)
      tempNumCols=(int)this->TheObjects[0].size;
    output.init((int) this->size, tempNumCols);
    for (int i=0; i<this->size; i++)
      for (int j=0; j<tempNumCols; j++)
        output.elements[i][j]=this->TheObjects[i][j];
  }
  void GetOrthogonalComplement(Vectors<coefficient>& output)
  { Matrix<coefficient> tempMat;
    tempMat.AssignVectorsToRows(*this);
    tempMat.GetZeroEigenSpaceModifyMe(output);
  }
  bool LinSpanContainsVector(const Vector<coefficient>& input, Matrix<coefficient>& bufferMatrix, Selection& bufferSelection)const;
  void MakeEiBasis(int theDimension, const coefficient& theRingUnit=1, const coefficient& theRingZero=0)
  { this->SetSize(theDimension);
    for (int i=0; i<this->size; i++)
      this->TheObjects[i].MakeEi(theDimension, i, theRingUnit, theRingZero);
  }
  bool LinSpanContainsVector(const Vector<coefficient>& input)const
  { Matrix<coefficient> buffer;
    Selection bufferSelection;
    return this->LinSpanContainsVector(input, buffer, bufferSelection);
  }
  static void SelectABasisInSubspace
  (const List<Vector<coefficient> >& input, List<Vector<coefficient> >& output, Selection& outputSelectedPivots, GlobalVariables* theGlobalVariables=0);
  int GetRankOfSpanOfElements(Matrix<coefficient>* buffer=0, Selection* bufferSelection=0)const;
  static bool ConesIntersect
  (List<Vector<Rational> >& StrictCone, List<Vector<Rational> >& NonStrictCone, Vector<Rational>* outputLinearCombo=0, Vector<Rational>* outputSplittingNormal=0,
   GlobalVariables* theGlobalVariables=0);
  static bool GetNormalSeparatingCones
  (List<Vector<coefficient> >& coneStrictlyPositiveCoeffs, List<Vector<coefficient> >& coneNonNegativeCoeffs, Vector<coefficient>& outputNormal,
   GlobalVariables* theGlobalVariables=0);
  void average(Vector<coefficient>& output, int theDimension)
  { this->sum(output, theDimension);
    if (this->size==0)
      return;
    output/=this->size;
  }
  void sum (Vector<coefficient>& output, int resultDim)const
  { output.MakeZero(resultDim);
    for (int i=0; i<this->size; i++)
      output+=this->TheObjects[i];
  }
  bool GetCoordsInBasis
  (const Vectors<coefficient>& inputBasis, Vectors<coefficient>& outputCoords, Vectors<coefficient>& bufferVectors, Matrix<coefficient>& bufferMat,
   const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  bool GetIntegralCoordsInBasisIfTheyExist
  (const Vectors<coefficient>& inputBasis, Vectors<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingMinusUnit, const coefficient& theRingZero)const
  { Matrix<coefficient> bufferMatGaussianEliminationCC, bufferMatGaussianElimination;
    bool result=true;
    output.SetSize(this->size);
    for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].GetIntegralCoordsInBasisIfTheyExist(inputBasis, output.TheObjects[i], bufferMatGaussianEliminationCC, bufferMatGaussianElimination, theRingUnit, theRingMinusUnit, theRingZero))
        result=false;
    return result;
  }
  bool GetIntegralCoordsInBasisIfTheyExist
  (const Vectors<coefficient>& inputBasis, Vectors<coefficient>& output, Matrix<coefficient>& bufferMatGaussianEliminationCC,
  Matrix<coefficient>& bufferMatGaussianElimination, const coefficient& theRingUnit, const coefficient& theRingMinusUnit, const coefficient& theRingZero)const
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
  bool ComputeNormalExcludingIndex(Vector<coefficient>& output, int index, Matrix<coefficient>& bufferMatrix);
  bool ComputeNormalFromSelection(Vector<coefficient>& output, Selection& theSelection, Matrix<coefficient>& bufferMatrix, int theDimension);
  bool ComputeNormalFromSelectionAndExtraRoot(Vector<coefficient>& output, Vector<coefficient>& ExtraRoot, Selection& theSelection, Matrix<coefficient>& bufferMatrix, Selection& bufferSel);
  bool ComputeNormalFromSelectionAndTwoExtraRoots
  (Vector<coefficient>& output, Vector<coefficient>& ExtraRoot1, Vector<coefficient>& ExtraRoot2, Selection& theSelection, Matrix<coefficient>& bufferMat, Selection& bufferSel);
  bool ComputeNormal(Vector<coefficient>& output, int inputDim)
  { if (this->size==0)
    { if (inputDim==1)
      { output.MakeEi(1,0);
        return true;
      }
      return false;
    }
    int theDimension= this->TheObjects[0].size;
    Matrix<coefficient> tempMatrix;
    Selection NonPivotPoints;
    NonPivotPoints.init(theDimension);
    output.SetSize(theDimension);
    this->GaussianEliminationForNormalComputation(tempMatrix, NonPivotPoints, theDimension);
    if (NonPivotPoints.CardinalitySelection!=1)
      return false;
    tempMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
    return true;
  }
  void GaussianEliminationForNormalComputation(Matrix<coefficient>& inputMatrix, Selection& outputNonPivotPoints, int theDimension)const;
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool GetLinearDependence(Matrix<coefficient>& outputTheLinearCombination);
  void GetLinearDependenceRunTheLinearAlgebra(Matrix<coefficient>& outputTheLinearCombination, Matrix<coefficient>& outputTheSystem, Selection& outputNonPivotPoints);
  bool ContainsARootNonPerpendicularTo(const Vector<coefficient>& input, const Matrix<coefficient>& theBilinearForm);
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
    HashedList<Vector<coefficient> > tempList;
    tempList.SetExpectedSize(this->size);
    tempList=*this;
    for (int i=0; i <this->size; i++)
      if (tempList.Contains(-this->TheObjects[i]))
        return true;
    return false;
  }
  void AssignMatrixColumns(Matrix<coefficient> & mat)
  { Vector<coefficient> tempRoot;
    this->SetSize(mat.NumCols);
    tempRoot.SetSize(mat.NumRows);
    for (int i=0; i<mat.NumCols; i++)
    { for (int j=0; j<mat.NumRows; j++)
        tempRoot[j]=mat.elements[j][i];
      this->TheObjects[i]=tempRoot;
    }
  }
  void AssignMatrixRows(const Matrix<coefficient>& mat)
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
  void BeefUpWithEiToLinearlyIndependentBasis(int theDim, const coefficient& ringUnit =(coefficient) 1, const coefficient& ringZero=(coefficient) 0)
  { Selection BufferSel;
    Matrix<coefficient> Buffer;
    assert(this->size==0 || theDim==this->GetDim());
    int currentRank=this->GetRankOfSpanOfElements(Buffer, BufferSel);
    if (currentRank==theDim)
      return;
    Vector<coefficient> theVect;
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
    Matrix<coefficient> tempMat;
    Selection tempSel;
    for (int i=0; i<this->size; i++)
    { output.AddOnTop(this->TheObjects[i]);
      if (output.GetRankOfSpanOfElements(&tempMat, &tempSel)< output.size)
        output.RemoveLastObject();
    }
    this->operator=(output);
  }
  static void IntersectTwoLinSpaces
  (const List<Vector<coefficient> >& firstSpace, const List<Vector<coefficient> >& secondSpace, List<Vector<coefficient> >& output,
   GlobalVariables* theGlobalVariables=0);
  bool IsRegular(Vector<coefficient>& r, Vector<coefficient>& outputFailingNormal, GlobalVariables& theGlobalVariables, int theDimension);
  void operator=(const List<List<coefficient> >& other)
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
  bool CheckInitialization()const;
  static const bool IsEqualToZero()
  { return false;
  }
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
  void CheckConsistencyWithOther(const ChevalleyGenerator& other)const;
  bool operator==(const ChevalleyGenerator& other)const
  { this->CheckConsistencyWithOther(other);
    return this->theGeneratorIndex==other.theGeneratorIndex;
  }
};

template <class coefficient, unsigned int inputHashFunction(const coefficient&)= coefficient::HashFunction>
class MonomialTensor
{
  friend std::ostream& operator <<(std::ostream& output, const MonomialTensor<coefficient, inputHashFunction>& theMon)
  { return output << theMon.ToString();
  }
private:
public:
  List<int> generatorsIndices;
  List<coefficient> Powers;
  std::string ToString(FormatExpressions* theFormat=0)const;
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
  void operator=(const MonomialTensor<coefficient, inputHashFunction>& other)
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
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower);
  void MultiplyByGeneratorPowerOnTheLeft(int theGeneratorIndexStandsToTheLeft, const coefficient& thePower);
  unsigned int HashFunction()const
  { int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
    unsigned int result=0;
    for (int i=0; i<top; i++)
      result+=SomeRandomPrimes[i]*this->generatorsIndices[i] + SomeRandomPrimes[top-1-i]* inputHashFunction(this->Powers[i]);
    return result;
  }
  static inline unsigned int HashFunction(const MonomialTensor<coefficient, inputHashFunction>& input)
  { return input.HashFunction();
  }
  void MakeConst()
  { this->generatorsIndices.size=0;
    this->Powers.size=0;
  }
  bool operator>(const MonomialTensor<coefficient, inputHashFunction>& other)const
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
  bool operator==(const MonomialTensor<coefficient, inputHashFunction>& other)const
  { return this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;
  }
  inline void operator*=(const MonomialTensor<coefficient, inputHashFunction>& standsOnTheRight)
  { if (standsOnTheRight.generatorsIndices.size==0)
      return;
    if (this==&standsOnTheRight)
    { MonomialTensor<coefficient, inputHashFunction> tempMon;
      tempMon=standsOnTheRight;
      (*this)*=(tempMon);
      return;
    }
    this->generatorsIndices.SetExpectedSize(standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
    this->Powers.SetExpectedSize(standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
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
    { std::cout << "This is a programming error: requested exponent of monomial variable with index " << i << " which is negative. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (i>=this->monBody.size)
      this->SetNumVariablesSubDeletedVarsByOne(i+1);
    return this->monBody[i];
  }
  Rational operator()(int i)const
  { if (i<0)
    { std::cout << "This is a programming error: requested exponent of monomial variable " << " with index " << i << " which is negative. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (i>=this->monBody.size)
      return 0;
    return this->monBody[i];
  }
  inline static std::string GetXMLClassName()
  { return "MonomialP";
  }
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
  bool SubstitutioN(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, const Element& theRingUnit=1)const;
  void MakeEi(int LetterIndex, int Power=1, int ExpectedNumVars=0);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { for (int i=this->monBody.size-1; i>=0; i--)
      if (this->monBody[i]!=0)
        return i;
    return -1;
  }
  bool IsGEQpartialOrder(MonomialP& m);
  static bool LeftIsGEQLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { if (left==(right))
      return false;
    return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right)
  { if (left==(right))
      return false;
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftIsGEQLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right)
  { return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { if (left==right)
      return false;
    return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right)
  { if (left==right)
      return false;
    if (left.TotalDegree()>right.TotalDegree())
      return true;
    if (left.TotalDegree() <right.TotalDegree())
      return false;
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftIsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
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
  template <class coefficient>
  void operator=(const Vector<coefficient>& other)
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
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)const
  { this->monBody.WriteToFile(output);
  }
  void WriteToFile(std::fstream& output)const
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
  static unsigned int HashFunction(const MonomialWeylAlgebra& input)
  { return
    input.polynomialPart.HashFunction()+
    input.differentialPart.HashFunction()*SomeRandomPrimes[0];
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  int GetMinNumVars()const
  { return MathRoutines::Maximum(this->polynomialPart.GetMinNumVars(), this->differentialPart.GetMinNumVars());
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
  bool HasNonSmallPositiveIntegerDerivation()const
  { for (int i=0; i<this->differentialPart.GetMinNumVars(); i++)
      if (!this->differentialPart(i).IsSmallInteger())
        return true;
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
  std::string WeylAlgebraDefaultLetter;
  std::string CustomPlusSign;
  std::string CustomCoeffMonSeparator;
  std::string FDrepLetter;
  std::string simpleRootLetter;
  std::string PathDisplayNameCalculator;
  std::string PathPhysicalOutputFolder;
  std::string PathDisplayOutputFolder;
  std::string PathDisplayServerBaseFolder;
  List<std::string> polyAlphabeT;
  List<std::string> weylAlgebraLetters;
  List<std::string> vectorSpaceEiBasisNames;
  std::string GetPolyLetter(int index)const;
  std::string GetChevalleyHletter(int index)const;
  std::string GetChevalleyGletter(int index)const;
  char AmbientWeylLetter;
  Rational AmbientWeylLengthFirstCoRoot;
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
  bool flagUseHtmlAndStoreToHD;
//  bool flagDynkinTypeDontUsePlusAndUpperIndex;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsFinal;
  bool flagExpressionNewLineAllowed;
  bool flagIncludeLieAlgebraNonTechnicalNames;
  bool flagIncludeLieAlgebraTypes;
  bool flagUseReflectionNotation;
  bool flagCandidateSubalgebraShortReportOnly;

  List<MonomialP>::OrderLeftGreaterThanRight thePolyMonOrder;
  template <typename TemplateMonomial>
  typename List<TemplateMonomial>::OrderLeftGreaterThanRight GetMonOrder();
  FormatExpressions();
};

template <class Object>
void List<Object>::QuickSortAscendingNoOrder(int BottomIndex, int TopIndex)
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
  this->QuickSortAscendingNoOrder(BottomIndex, HighIndex-1);
  this->QuickSortAscendingNoOrder(HighIndex+1, TopIndex);
}

template <class Object>
void List<Object>::QuickSortAscendingOrder(int BottomIndex, int TopIndex, List<Object>::OrderLeftGreaterThanRight theOrder)
{ if (TopIndex<=BottomIndex)
    return;
  int HighIndex = TopIndex;
  for (int LowIndex = BottomIndex+1; LowIndex<=HighIndex; LowIndex++)
    if (theOrder(this->TheObjects[LowIndex],(this->TheObjects[BottomIndex])))
    { this->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex--;
      HighIndex--;
    }
  if (theOrder(this->TheObjects[HighIndex], this->TheObjects[BottomIndex]))
  { if (HighIndex==BottomIndex)
    { std::cout << "This is a programming error. The programmer has given me a bad strict order: "
      << " the order claims that object of index " << HighIndex << " is strictly greater than itself "
      << " which is not allowed for strict orders. Maybe the programmer has given a non-strict order instead of "
      << " strict one by mistake? " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    HighIndex--;
  }
  this->SwapTwoIndices(BottomIndex, HighIndex);
  this->QuickSortAscendingOrder(BottomIndex, HighIndex-1, theOrder);
  this->QuickSortAscendingOrder(HighIndex+1, TopIndex, theOrder);
}

template <class Object>
template <class compareClass, class carbonCopyType>
bool List<Object>::QuickSortAscendingCustomRecursive(int BottomIndex, int TopIndex, compareClass& theCompareror, List<carbonCopyType>* carbonCopy)
{ if (TopIndex<=BottomIndex)
    return true;
  int HighIndex = TopIndex;
  for (int LowIndex = BottomIndex+1; LowIndex<=HighIndex; LowIndex++)
    if (theCompareror.CompareLeftGreaterThanRight
        (this->TheObjects[LowIndex],(this->TheObjects[BottomIndex])))
    { if (carbonCopy!=0)
        carbonCopy->SwapTwoIndices(LowIndex, HighIndex);
      this->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex--;
      HighIndex--;
    }
  if (theCompareror.CompareLeftGreaterThanRight(this->TheObjects[HighIndex],this->TheObjects[BottomIndex]))
    HighIndex--;
  if (carbonCopy!=0)
    carbonCopy->SwapTwoIndices(BottomIndex, HighIndex);
  this->SwapTwoIndices(BottomIndex, HighIndex);
  if (!this->QuickSortAscendingCustomRecursive(BottomIndex, HighIndex-1, theCompareror, carbonCopy))
    return false;
  if (!this->QuickSortAscendingCustomRecursive(HighIndex+1, TopIndex, theCompareror, carbonCopy))
    return false;
  return true;
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
{ if (index1<0 || index1>=this->size || index2<0 || index2>=this->size)
  { std::cout << "This is a programming error: requested to the elements with indices "
    << index1 << " and " << index2 << " in a list that has " << this->size << " elements. This "
    << " is impossible. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (index1==index2)
    return;
  Object tempO;
  tempO= this->TheObjects[index1];
  this->TheObjects[index1]=this->TheObjects[index2];
  this->TheObjects[index2]=tempO;
}

template<class Object>
int List<Object>::GetIndex(const Object& o) const
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
int List<Object>::SizeWithoutObjects()const
{ return  sizeof(this->ActualSize)+ sizeof(this->size)+ sizeof(this->TheObjects);
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
bool List<Object>::AddOnTopNoRepetition(const Object& o)
{ if (this->GetIndex(o)!=-1)
    return false;
  this->AddOnTop(o);
  return true;
}

template <class Object>
inline Object* List<Object>::LastObject()const
{ // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->size<=0)
  { std::cout << "This is a programming error: trying to fetch the last object of an array with "
    << this->size << " elements. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return &this->TheObjects[this->size-1];
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
void List<Object>::ReservE(int theSize)
{ // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->ActualSize<theSize)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    this->ExpandArrayOnTop(theSize- this->ActualSize);
  }
}

template <class Object>
void List<Object>::RemoveFirstOccurenceSwapWithLast(const Object& o)
{ for (int i=0; i<this->size; i++)
    if (o==this->TheObjects[i])
    { this->RemoveIndexSwapWithLast(i);
      return;
    }
}

template <class Object>
void List<Object>::SetSize(int theSize)
{// <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (theSize<0)
    theSize=0;
  this->SetExpectedSize(theSize);
  this->ReservE(theSize);
  this->size=theSize;
}

template <class Object>
inline std::string List<Object>::ToString()const
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
    out << this->TheObjects[i].ToString() << "\n";
  return out.str();
}

template <class Object>
inline std::string List<Object>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
    out << this->TheObjects[i].ToString(theFormat) << "\n";
  return out.str();
}

template <class Object>
inline void List<Object>::AddObjectOnTopCreateNew()
{ this->SetSize(this->size+1);
}

template <class Object>
void List<Object>::RemoveIndexSwapWithLast(int index)
{ if (this->size==0)
    return;
  this->size--;
  this->TheObjects[index]=this->TheObjects[this->size];
}

template <class Object>
void List<Object>::RemoveLastObject()
{ if (this->size==0)
  { std::cout << "Programming error: attempting to pop empty list"
    << MathRoutines::GetStackTraceEtcErrorMessage(__FILE__,__LINE__);
    assert(false);
  }
  this->size--;
}

template <class Object>
Object List<Object>::PopLastObject()
{ if (this->size==0){
    std::cout << "Programming error: attempting to pop empty list" << MathRoutines::GetStackTraceEtcErrorMessage(__FILE__,__LINE__);
    assert(false);
  }
  this->size--;
  return this->TheObjects[size];
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
{ delete [] this->TheObjects;
  this->size=0;
  this->TheObjects=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->ActualSize=0;
}

template <class Object>
List<Object>::~List()
{ delete [] this->TheObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->size=0;
  this->ActualSize=0;
}

template <class Object>
void List<Object>::ExpandArrayOnTop(int increase)
{// <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (increase<=0)
    return;
  MacroIncrementCounter(NumListResizesTotal);
  Object* newArray = new Object[this->ActualSize+increase];
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=this->ActualSize+increase;
  ParallelComputing::CheckPointerCounters();
#endif
  for (int i=0; i<this->size; i++)
    newArray[i]=this->TheObjects[i];
  delete [] this->TheObjects;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter-=this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheObjects= newArray;
  this->ActualSize+=increase;
}

template <class Object>
void List<Object>::ReverseOrderElements()
{ int tempI= this->size/2;
  for (int i=0; i<tempI; i++)
    this->SwapTwoIndices(i, this->size-i-1);
}

template <class Object>
void List<Object>::AddOnTop(const Object& o)
{// <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->size>this->ActualSize)
  { std::cout << "This is a programming error: the actual size of the list is " << this->ActualSize << " but this->size equals " << this->size
    << ". " << MathRoutines::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (this->size==this->ActualSize)
    this->ExpandArrayOnTop(this->GetNewSizeRelativeToExpectedSize(this->ActualSize+1)- this->size);
  this->TheObjects[size]=o;
  this->size++;
}
#endif
