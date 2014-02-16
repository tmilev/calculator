//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_h_already_included
#define vpfHeader1_h_already_included

#include "vpfMacros.h"
static ProjectInformationInstance vpfHeader1instance(__FILE__, "Header, general routines. ");

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
class SubgroupWeylGroupOLD;
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
template <class coefficient>
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
class XML;
struct CGI;

//The calculator parsing routines:
class Calculator;
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
  static long long GlobalPointerCounter;
  static long long PointerCounterPeakRamUse;
  static ControllerStartsRunning controllerSignalPauseUseForNonGraciousExitOnly;
#ifdef CGIversionLimitRAMuse
  static long long cgiLimitRAMuseNumPointersInList;
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
typedef void (*FunctionStandardStringOutput)(const std::string& input);
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
      crash << "This is a programming error: the invert X mod N algorithm requires that X and N be relatively prime, which appears to have not been the case. "
      << crash;
    //if d and p were relatively prime this should be so. Otherwise the function was not called properly.
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
  static bool isADigit(const std::string& input, int* whichDigit=0)
  { if (input.size()!=1)
      return false;
    return MathRoutines::isADigit(input[0], whichDigit);
  }
  static inline bool isADigit(char theChar, int* whichDigit=0)
  { int theDigit=theChar-'0';
    bool result=(theDigit<10 && theDigit>=0);
    if (result && whichDigit!=0)
      *whichDigit=theDigit;
    return result;
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
  static inline double E()
  { return 2.718281828459;
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
  template <class coefficient>
  static void RaiseToPower(coefficient& theElement, int thePower, const coefficient& theRingUnit);
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
};

class XML
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
  std::string theString;
  int positionInString;
  XML();
  bool ReadFromFile(std::fstream& inputFile);
  static bool FileExists(const std::string& theFileName);
  static bool OpenFileCreateIfNotPresent(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary);
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
    return XML::ReadThroughFirstOpenTag(streamToMoveIn, tempInt, tagNameNoSpacesNoForbiddenCharacters);
  }
  inline static bool ReadEverythingPassedTagOpenUntilTagClose(std::istream& streamToMoveIn, const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { int tempInt;
    return XML::ReadEverythingPassedTagOpenUntilTagClose(streamToMoveIn, tempInt, tagNameNoSpacesNoForbiddenCharacters);
  }
  static bool ReadThroughFirstOpenTag(std::istream& streamToMoveIn, int& NumReadWordsExcludingTag, const std::string& tagNameNoSpacesNoForbiddenCharacters)
  { std::string tagOpen=XML::GetOpenTagNoInputCheck(tagNameNoSpacesNoForbiddenCharacters);
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
  { std::string tagClose=XML::GetCloseTagNoInputCheck(tagNameNoSpacesNoForbiddenCharacters);
    std::string tagOpen=XML::GetOpenTagNoInputCheck(tagNameNoSpacesNoForbiddenCharacters);
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
  bool GetStringEnclosedIn(const std::string& theTagName, std::string& outputString);
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
  { crash << crash;
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
    theSize=0;
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
{ output << theList.size << " elements: \n";
  for (int i=0; i<theList.size; i++)
  { output << theList[i];
    if (i!=theList.size-1)
      output << ", ";
  }
  return output;
}

template <class Object>
std::iostream& operator>>(std::iostream& input, List<Object>& theList)
{ std::string tempS; int tempI;
  input >> tempS >> tempI;
  if(tempS!="size:")
    crash << crash;
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
  template <class otherType>
  void QuickSortAscendingNoOrder(int BottomIndex, int TopIndex, List<otherType>* carbonCopy=0);
  //submitting zero comparison function is not allowed!
  //that is why the function is private.
  template <class otherType>
  void QuickSortAscendingOrder(int BottomIndex, int TopIndex, List<Object>::OrderLeftGreaterThanRight theOrder, List<otherType>* carbonCopy=0);
  template <class compareClass, class carbonCopyType>
  bool QuickSortAscendingCustomRecursive(int BottomIndex, int TopIndex, compareClass& theCompareror, List<carbonCopyType>* carbonCopy);
  void QuickSortDescending(int BottomIndex, int TopIndex);
  inline void initConstructorCallOnly()
  { this->TheObjects=0;
    this->ActualSize=0;
    this->size=0;
    this->flagDeallocated=false;
    MacroIncrementCounter(NumListsCreated);
  }
  bool flagDeallocated;
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
  bool CheckConsistency()const
  { if (this->flagDeallocated)
    { crash << "This is a programming error: use after free of List. " << crash;
    }
    return true;
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
  template <class compareClass, class carbonCopyType=Object>
  bool QuickSortDescendingCustom(compareClass& theCompareror, List<carbonCopyType>* carbonCopy=0)
  { bool result=this->QuickSortAscendingCustomRecursive(0, this->size-1, theCompareror, carbonCopy);
    this->ReverseOrderElements();
    if (carbonCopy!=0)
      carbonCopy->ReverseOrderElements();
    return result;
  }
  //The below function is required to preserve the order of elements given by theSelection.elements.
  void SubSelection(const Selection& theSelection, List<Object>& output);
  //If comparison function is not specified, QuickSortAscending usese operator>, else it uses the given
  //comparison function
  template <class otherType=Object>
  void QuickSortAscending(List<Object>::OrderLeftGreaterThanRight theOrder=0, List<otherType>* carbonCopy=0)
  { if (carbonCopy!=0)
      if (carbonCopy->size!=this->size)
        crash << "Programming error: requesting to quick with carbon copy, but the carbon copy has a different number of elements." << crash;
    if (this->size==0)
      return;
    if (theOrder==0)
      this->QuickSortAscendingNoOrder(0, this->size-1, carbonCopy);
    else
      this->QuickSortAscendingOrder(0, this->size-1, theOrder, carbonCopy);
  }
  template <class otherType=Object>
  void QuickSortDescending(List<Object>::OrderLeftGreaterThanRight theOrder=0, List<otherType>* carbonCopy=0)
  { this->QuickSortAscending(theOrder, carbonCopy);
    this->ReverseOrderElements();
    if (carbonCopy!=0)
      carbonCopy->ReverseOrderElements();
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
    if(n == this->size)
      return -1;
    return (this->TheObjects[n]==o) ? n : -1;
  }
  // return the last index n of which we can say o <= l[n], or l.size
  // i.e. not the first index at which o <= l[n], as if we were implementing l.GetIndex(o)
  // the reason we want the last index is to support insertion sorting
  int BSExpectedIndex(const Object& o) const{
    if(this->size == 0)
      return 0;
    int i = 0, j = this->size-1;
    while(true){
        int s=(j-i)/2;
        int n = j-s;
        if(s == 0)
        { if(this->TheObjects[n]<o)
            return n+1;
          return n;
        }
        if(o<this->TheObjects[n])
        { j = n-1;
          continue;
        }
        i = n;
    }
  }
  // indexing is O(n log n) and insertion is O(n), whereas hash table insertion and indexing
  // are both O(1)
  bool BSInsertDontDup(const Object& o){
    int n = BSExpectedIndex(o);
    if(n==this->size)
    { this->AddOnTop(o);
      return false;
    }
    if(this->TheObjects[n] == o)
      return true;
    this->InsertAtIndexShiftElementsUp(o,n);
    return false;
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
      crash << "Programming error: attempting to access the entry of index " << i << " in an array of " << this->size << " elements. " << crash;
    this->CheckConsistency();
    return this->TheObjects[i];
  }
  inline bool operator!=(const List<Object>& other)const{ return !this->IsEqualTo(other);}
  inline bool operator==(const List<Object>& other)const{ return this->IsEqualTo(other);}
  bool operator>(const List<Object>& other)const;
  bool operator<(const List<Object>& other)const;
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

struct CGI
{
public:
  static std::stringstream outputStream;
  static int GlobalMathSpanID;
  static int GlobalCanvasID;
  static int GlobalGeneralPurposeID;
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
  static std::string DoubleBackslashes(const std::string& input);
  static std::string GetMathSpanPure(const std::string& input, int upperNumChars=700);
  static std::string GetMathSpanBeginArrayL(const std::string& input, int upperNumChars=700);
  static std::string GetMathMouseHover(const std::string& input, int upperNumChars=700);
  static std::string GetMathMouseHoverBeginArrayL(const std::string& input, int upperNumChars=700);
  static std::string GetStyleButtonLikeHtml()
  { return " style=\"background:none; border:0; text-decoration:underline; color:blue; cursor:pointer\" ";
  }
  static std::string GetHtmlButton(const std::string& buttonID, const std::string& theScript, const std::string& buttonText);
  static std::string GetHtmlSpanHidableStartsHiddeN(const std::string& input);
  static std::string clearSlashes(const std::string& theString);
  static std::string CleanUpForFileNameUse(const std::string& inputString);
  static std::string CleanUpForLaTeXLabelUse(const std::string& inputString);
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
  static void MakeStdCoutReport(const std::string& input);
  static void MakeReportIndicatorFile(const std::string& input);
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
  const List<int>& GetHashArray(int hashIndex)const
  { return this->TheHashedArrays[hashIndex];
  }
  void GrandMasterConsistencyCheck()const
  { for (int i=0; i<this->TheHashedArrays.size; i++)
    { List<int>& current=this->TheHashedArrays[i];
      for (int j=0; j<current.size; j++)
      { int theIndex=current[j];
        if (theIndex>=this->size)
          crash << "This is a programming error: hash lookup array of index " << i << ", entry of index " << j << " reports index "
          << theIndex << " but I have only " << this->size << " entries. " << crash;
        if (this->GetHash((*this)[theIndex])!=(unsigned) i)
        { crash << "<hr>This is a programming error: the hashed element in position " << theIndex << " is recorded in hash array of index "
          << i << ", however its hash value is instead " << this->GetHash((*this)[theIndex]) << ". The hash size is "
          << this->TheHashedArrays.size << "<br>hashes of objects: ";
          for (int l=0; l<this->size; l++)
            crash << this->GetHash((*this)[l]) << "= " << this->GetHash((*this)[l])%this->TheHashedArrays.size << ", ";
          crash << "<br>hashes recorded: ";
          for (int l=0; l<this->TheHashedArrays.size; l++)
            for (int k=0; k<this->TheHashedArrays[l].size; k++)
              crash << this->TheHashedArrays[l][k] << ", ";
          crash << crash;
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
  bool AddOnTopNoRepetitionMustBeNewCrashIfNot(const Object& o)
  { if (this->GetIndex(o)!=-1)
    { crash.theCrashReport << "This is a programming error: the programmer requested to add the object " << o << " without repetition "
      << " to the hashed list with a function that does not allow repetition, but the hashed list already contains the object. ";
      crash << crash;
    }
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
      crash << "This is a programming error. You are attempting to pop out index " << index << " out of hashed array "
      << " of size " << this->size << ". " << crash;
    int hashIndexPop = this->GetHash(this->TheObjects[index]);
    this->TheHashedArrays[hashIndexPop].RemoveFirstOccurenceSwapWithLast(index);
    int hashIndexIncoming=this->GetHash(theObject);
    this->TheHashedArrays[hashIndexIncoming].AddOnTop(index);
    this->TheObjects[index]=theObject;
  }
  void RemoveIndexSwapWithLast(int index)
  { if (index<0 || index>=this->size)
      crash << "This is a programming error. You are attempting to pop out index " << index << " out of hashed array "
      << " of size " << this->size << ". " << crash;
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
        crash << "This is a programming error: corrupt hash table: at hashindex= " << hashIndex << " I get instructed to look up index " << j
        << " but I have only " << this->size << "elements. " << crash;
      if((*this)[j]==o)
        return j;
    }
    return -1;
  }
  inline int GetIndexIMustContainTheObject(const Object& o) const
  { int result=this->GetIndex(o);
    if (result==-1)
    { crash.theCrashReport << "This is a programming error: the programmer has requested the index of object " << o
      << " with a function that does not allow failure. However, the container array does not contain this object. ";
      crash << crash;
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
  Object& GetElement(int theObjectIndex)const
  { return TemplateList::operator[](theObjectIndex);
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

class FileInformation
{
  public:
  std::string FileName;
  std::string FileDescription;
  bool operator>(const FileInformation& other)
  { return this->FileName>other.FileName;
  }
  bool operator==(const FileInformation& other)const
  { return this->FileName==other.FileName;
  }
  static unsigned int HashFunction(const FileInformation& input)
  { return MathRoutines::hashString(input.FileName);
  }
};

class ProjectInformation
{
  public:
  List<stackInfo> CustomStackTrace;
  MutexWrapper infoIsInitialized;
  HashedList<FileInformation> theFiles;
  static ProjectInformation& GetMainProjectInfo()
  { //This is required to avoid the static initialization order fiasco.
    //For more information, google "static initialization order fiasco"
    //and go to the first link. The solution used here was proposed inside that link.
    static ProjectInformation MainProjectInfo;
    MainProjectInfo.CustomStackTrace.ReservE(30);
    MainProjectInfo.theFiles.SetExpectedSize(100);
    return MainProjectInfo;
  }
  std::string ToString();
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

template <typename coefficient>
class Matrix
{ // friend std::iostream& operator<< <coefficient>(std::iostream& output, const Matrix<coefficient>& theMat);
  friend std::ostream& operator<< (std::ostream& output, const Matrix<coefficient>& theMat)
  { output << theMat.ToString();
    return output;
  }
//  friend std::iostream& operator>> <coefficient>(std::iostream& input, Matrix<coefficient>& theMat);
public:
  int NumRows; int ActualNumRows;
  int NumCols; int ActualNumCols;
  coefficient** elements;
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
  void Resize(int r, int c, bool PreserveValues, const coefficient* TheRingZero=0);
  inline static std::string GetXMLClassName()
  { std::string result="Matrix_";
    result.append(coefficient::GetXMLClassName());
    return result;
  }
  Matrix():NumRows(0), ActualNumRows(0), NumCols(0), ActualNumCols(0), elements(0){}
  Matrix(const Matrix<coefficient>& other):NumRows(0), ActualNumRows(0), NumCols(0), ActualNumCols(0), elements(0)
  { *this=other;
  }
  ~Matrix()
  { this->ReleaseMemory();
  }
  void Transpose()
  { if (this->NumCols==this->NumRows)
    { for (int i=0; i<this->NumRows; i++)
        for (int j=i+1; j<this->NumCols; j++)
          MathRoutines::swap<coefficient>(this->elements[j][i], this->elements[i][j]);
      return;
    }
    Matrix<coefficient> tempMat;
    tempMat.init(this->NumCols, this->NumRows);
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        tempMat.elements[j][i]=this->elements[i][j];
    *this=tempMat;
  }
  void AppendMatrixOnTheRight(const Matrix<coefficient>& standsOnTheRight)
  { if (&standsOnTheRight==this)
    { Matrix<coefficient> copyThis=*this;
      this->AppendMatrixOnTheRight(copyThis);
      return;
    }
    if (standsOnTheRight.NumRows<this->NumRows)
    { coefficient theZero;
      theZero=0;
      Matrix<coefficient> standsOnTheRightNew=standsOnTheRight;
      standsOnTheRightNew.Resize(this->NumRows, standsOnTheRight.NumCols, true, &theZero);
      this->AppendMatrixOnTheRight(standsOnTheRightNew);
      return;
    }
    if (this->NumRows<standsOnTheRight.NumRows)
    { coefficient theZero;
      theZero=0;
      this->Resize(standsOnTheRight.NumRows, this->NumCols, true, &theZero);
    }
    int oldNumCols=this->NumCols;
    this->Resize(this->NumRows, standsOnTheRight.NumCols+oldNumCols, true);
    for (int i=0; i<this->NumRows; i++)
      for (int j=oldNumCols; j<this->NumCols; j++)
        this->elements[i][j]=standsOnTheRight.elements[i][j-oldNumCols];
  }
  void AppendMatrixToTheBottom(const Matrix<coefficient>& standsBelow)
  { if (&standsBelow==this)
    { Matrix<coefficient> copyThis=*this;
      this->AppendMatrixToTheBottom(copyThis);
      return;
    }
    if (standsBelow.NumCols<this->NumCols)
    { coefficient theZero;
      theZero=0;
      Matrix<coefficient> standsBelowNew=standsBelow;
      standsBelowNew.Resize(standsBelow.NumRows, this->NumCols, true, &theZero);
      this->AppendMatrixOnTheRight(standsBelowNew);
      return;
    }
    if (this->NumCols<standsBelow.NumCols)
    { coefficient theZero;
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
  static void MatrixInBasis(const Matrix<coefficient>& input, Matrix<coefficient>& output, const List<Vector<coefficient> >& basis, const Matrix<coefficient>& gramMatrixInverted)
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
  void ComputeDeterminantOverwriteMatrix(coefficient& output, const coefficient& theRingOne=1, const coefficient& theRingZero=0);
  void ActOnVectorROWSOnTheLeft(List<Vector<coefficient> >& standOnTheRightAsVectorRow)const
  { List<Vector<coefficient> > output;
    this->ActOnVectorROWSOnTheLeft(standOnTheRightAsVectorRow, output);
    standOnTheRightAsVectorRow=output;
  }
  void ActOnVectorROWSOnTheLeft(List<Vector<coefficient> >& standOnTheRightAsVectorRow, List<Vector<coefficient> >& output)const
  { if (this->NumCols!=standOnTheRightAsVectorRow.size)
      crash << "This is a programming error: attempting to multiply a matrix, standing on the left, with "
      << this->NumCols << " columns, by a matrix, standing on the right, with " << standOnTheRightAsVectorRow.size << " rows. "
      << crash;
    output.SetSize(this->NumRows);
    for (int i=0; i<this->NumRows; i++)
    { output[i].MakeZero(standOnTheRightAsVectorRow[0].size);
      for (int j=0; j<this->NumCols; j++)
        output[i]+= standOnTheRightAsVectorRow[j]*(*this)(i,j);
    }
  }
  void ActMultiplyVectorRowOnTheRight(Vector<coefficient>& standsOnTheLeft, const coefficient& TheRingZero=0)const
  { Vector<coefficient> output;
    this->ActMultiplyVectorRowOnTheRight(standsOnTheLeft, output, TheRingZero);
    standsOnTheLeft=output;
  }
  void ActMultiplyVectorRowOnTheRight(const Vector<coefficient>& standsOnTheLeft, Vector<coefficient>& output, const coefficient& TheRingZero=0)const
  { if (&standsOnTheLeft==&output || this->NumRows!=standsOnTheLeft.size)
      crash << crash;
    output.MakeZero(this->NumCols);
    coefficient tempElt;
    for (int i=0; i<this->NumCols; i++)
      for (int j=0; j<this->NumRows; j++)
      { tempElt=this->elements[j][i];
        tempElt*=standsOnTheLeft[j];
        output[i]+=tempElt;
      }
  }
  void GetNSquaredVectorForm(Vector<coefficient>& output)
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
    { crash << "This is a programming error: attempting to multply a matrix with " << this->NumCols << " columns with a vector(column) of "
      << " dimension " << input.size << ". " << crash;
    }
    output.MakeZero(this->NumRows);
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
  static unsigned int HashFunction(const Matrix<coefficient>& input)
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
  { if(&input==&output)
      crash << crash;
    if (input.size==0)
      return;
    if (this->NumCols!=input.GetDim())
    { crash << "This is a programming error: attempting to act by " << this->ToString() << "(an " << this->NumRows << " x "
      << this->NumCols << " matrix) on a column vector " << input.ToString() << "(dimension " << input.size << ")." << crash;
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
  std::string ToStringLatex(FormatExpressions* theFormat=0)const;
  std::string ElementToStringWithBlocks(List<int>& theBlocks);
  void MakeIdMatrix(int theDimension, const coefficient& theRingUnit=1, const coefficient& theRingZero=0)
  { this->init(theDimension, theDimension);
    for (int i=0; i<theDimension; i++)
      for (int j=0; j<theDimension; j++)
        if (j!=i)
          this->elements[i][j]=theRingZero;
        else
          this->elements[i][j]=theRingUnit;
  }
  void MakeZeroMatrix(int theDimension, const coefficient& theRingZero=0)
  { this->init(theDimension, theDimension);
    for (int i=0; i<theDimension; i++)
      for (int j=0; j<theDimension; j++)
        this->elements[i][j] = theRingZero;
  }
  void ActOnMonomialAsDifferentialOperator(const MonomialP& input, Polynomial<Rational>& output);
  void SwitchTwoRows(int row1, int row2);
  inline void SwitchTwoRowsWithCarbonCopy(int row1, int row2, Matrix<coefficient>* theCarbonCopy)
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
  inline coefficient ScalarProduct(const Vector<coefficient>& left, const Vector<coefficient>& right)
  { return this->ScalarProduct(left, right, (coefficient) 0);
  }
  coefficient ScalarProduct(const Vector<coefficient>& left, const Vector<coefficient>& right, const coefficient& theRingZero)
  { if(left.size!=this->NumCols || right.size!=this->NumRows)
      crash << crash;
    coefficient Result, tempElt;
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
  inline coefficient& operator()(int i, int j)const
  { if (i<0 || i>=this->NumRows || j<0 || j>=this->NumCols)
    { crash << "This is a programming error: requesting row, column indexed by " << i+1 << " and " << j+1 << " but I am a matrix with "
      << this->NumRows << " rows and " << this->NumCols << " colums. " << crash;
    }
    return this->elements[i][j];
  }
  bool IsSquare()const
  { return this->NumCols==this->NumRows;
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
  void GetVectorFromColumn(int colIndex, Vector<coefficient>& output)const;
  void GetVectorFromRow(int rowIndex, Vector<coefficient>& output)const;
  int FindPivot(int columnIndex, int RowStartIndex);
  bool FindFirstNonZeroElementSearchEntireRow(coefficient& output)
  { for (int i=0; i<this->NumCols; i++)
      for(int j=0; j<this->NumRows; j++)
        if (!this->elements[i][j].IsEqualToZero())
        { output=this->elements[i][j];
          return true;
        }
    return false;
  }
  void MakeLinearOperatorFromDomainAndRange(Vectors<coefficient>& domain, Vectors<coefficient>& range)
  { Matrix<coefficient> A;
    Matrix<coefficient> B;
    A.AssignVectorsToRows(domain);
    B.AssignVectorsToRows(range);
    A.Invert();
    (*this)=A*B;
    this->Transpose();
  }
  void RowTimesScalar(int rowIndex, const coefficient& scalar);
  inline void RowTimesScalarWithCarbonCopy(int rowIndex, const coefficient& scalar, Matrix<coefficient>* theCarbonCopy)
  { this->RowTimesScalar(rowIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->RowTimesScalar(rowIndex, scalar);
  }
  void AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const coefficient& scalar, GlobalVariables* theGlobalVariables=0);
  inline void AddTwoRowsWithCarbonCopy(int fromRowIndex, int ToRowIndex, int StartColIndex, const coefficient& scalar, Matrix<coefficient>* theCarbonCopy)
  { this->AddTwoRows(fromRowIndex, ToRowIndex, StartColIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->AddTwoRows(fromRowIndex, ToRowIndex, StartColIndex, scalar);
  }
  void SubtractRows(int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const coefficient& scalar);
  inline void SubtractRowsWithCarbonCopy(int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const coefficient& scalar, Matrix<coefficient>* theCarbonCopy)
  { this->SubtractRows(indexRowWeSubtractFrom, indexSubtracted, StartColIndex, scalar);
    if (theCarbonCopy!=0)
      theCarbonCopy->SubtractRows(indexRowWeSubtractFrom, indexSubtracted, StartColIndex, scalar);
  }
  void MultiplyOnTheLeft(const Matrix<coefficient>& standsOnTheLeft, Matrix<coefficient>& output, const coefficient& theRingZero=0);
  void MultiplyOnTheLeft(const Matrix<coefficient>& standsOnTheLeft, const coefficient& theRingZero=0);
  void MultiplyOnTheRight(const Matrix<coefficient>& standsOnTheRight, const coefficient& theRingZero=0)
  { Matrix<coefficient> temp=*this;
    *this=standsOnTheRight;
    this->MultiplyOnTheLeft(temp);
  }
  void NonPivotPointsToEigenVectorMatrixForm(Selection& TheNonPivotPoints, Matrix<coefficient>& output);
  void GetVectorsFromRows(List<Vector<coefficient> >& output)
  { output.SetSize(this->NumRows);
    for (int i=0; i<this->NumRows; i++)
    { output[i].SetSize(this->NumCols);
      for (int j=0; j<this->NumCols; j++)
        output[i][j]=this->elements[i][j];
    }
  }
  void NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, Vector<coefficient>& output, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  //returns true if successful, false otherwise
//  bool ExpressColumnAsALinearCombinationOfColumnsModifyMyself
//    (Matrix<coefficient>& inputColumn, Matrix<coefficient>* outputTheGaussianTransformations Matrix<coefficient>& outputColumn);
  bool Invert(GlobalVariables* theGlobalVariables=0);
  void MakeZero(const coefficient& theRingZero=0);
  //if m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below function
  //corresponds to the linear operator from V1+W1 to V2+W2 (direct sum)
  //this means you write the matrix m1 in the upper left corner m2 in the lower right
  // and everything else you fill with zeros
  void AssignDirectSum(Matrix<coefficient>& m1,  Matrix<coefficient>& m2);
  // if S and T are endomorphisms of V and W, build the matrix of SⓧT that acts on
  // VⓧW with basis (v1ⓧw1,v1ⓧw2,...,v2ⓧw1,v2ⓧw2,...vnⓧwn)
  void AssignTensorProduct(const Matrix<coefficient>& left, const Matrix<coefficient>& right);
  void AssignVectorsToRows(const Vectors<coefficient>& input)
  { int numCols=-1;
    if (input.size>0)
      numCols=input[0].size;
    this->init(input.size, numCols);
    for (int i=0; i<input.size; i++)
      for (int j=0; j<numCols; j++)
        this->elements[i][j]=input[i][j];
  }
  void AssignVectorsToColumns(const Vectors<coefficient>& input)
  { this->init(input[0].size, input.size);
    for (int i=0; i<this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        (*this)(i,j)=input[j][i];
  }
  void AssignVectorColumn(const Vector<coefficient>& input)
  { this->init(input.size, 1);
    for (int i=0; i<input.size; i++)
      this->elements[i][0]=input[i];
  }
  void AssignVectorToRowKeepOtherRowsIntactNoInit(int rowIndex, const Vector<coefficient>& input)
  { if(input.size!=this->NumCols || rowIndex>=this->NumRows || rowIndex<0)
      crash << "Error: attempting to assign vector " << input.ToString() << " (" << input.size << " coordinates) "
      << " to row with index " << rowIndex << " in a matrix with " << this->NumRows << " rows and " << this->NumCols << " columns. " << crash;
    for (int i=0; i<this->NumCols; i++)
      this->elements[rowIndex][i]=input[i];
  }
  void AssignVectorToColumnKeepOtherColsIntactNoInit(int colIndex, const Vector<coefficient>& input)
  { if(input.size!=this->NumRows || colIndex>=this->NumCols || colIndex<0)
      crash << crash;
    for (int i=0; i<this->NumRows; i++)
      this->elements[i][colIndex]=input[i];
  }
  void AssignVectorRow(const Vector<coefficient>& input)
  { this->init(1, input.size);
    for (int i=0; i<input.size; i++)
      this->elements[0][i]=input[i];
  }
  void DirectSumWith(const Matrix<coefficient>& m2, const coefficient& theRingZero=0);
  inline bool operator!=(const Matrix<coefficient>& other)const
  { return !((*this)==other);
  }
  bool operator==(const Matrix<coefficient>& other)const
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
  bool IsProportionalTo(const Matrix<coefficient>& input, coefficient& outputTimesMeEqualsInput)
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
    Matrix<coefficient> tempMat;
    tempMat=*this;
    tempMat*=outputTimesMeEqualsInput;
    tempMat-=(input);
    return tempMat.IsEqualToZero();
  }
  //returns true if the system has a solution, false otherwise
  bool RowEchelonFormToLinearSystemSolution(Selection& inputPivotPoints, Matrix<coefficient>& inputRightHandSide, Matrix<coefficient>& outputSolution);
  void operator+=(const Matrix<coefficient>& right)
  { if (this->NumRows!=right.NumRows || this->NumCols!=right.NumCols)
      crash << "This is a programming error: attempting to add matrix with " << this->NumRows << " rows and " << this->NumCols
      << " columns to a matrix with " << right.NumRows << " rows and " << right.NumCols << " columns. " << crash;
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        this->elements[i][j]+=(right.elements[i][j]);
  }
  LargeIntUnsigned FindPositiveLCMCoefficientDenominators();
  void operator-=(const Matrix<coefficient>& right)
  { if (this->NumRows!=right.NumRows || this->NumCols!=right.NumCols)
      crash << "This is a programming error: attempting to subtract from matrix with " << this->NumRows << " rows and " << this->NumCols
      << " columns a matrix with " << right.NumRows << " rows and " << right.NumCols << " columns. " << crash;
    for (int i=0; i< this->NumRows; i++)
      for (int j=0; j<this->NumCols; j++)
        (*this)(i,j)-=right(i,j);
  }
  void WriteToFile(std::fstream& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->WriteToFile(output);
  }
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { return this->ReadFromFile(input);
  }
  bool ReadFromFile(std::fstream& input);
  void operator/=(const coefficient& input)
  { for (int j=0; j<this->NumRows; j++)
      for (int i=0; i<this->NumCols; i++)
        this->elements[j][i]/=input;
  }
  void operator*=(const coefficient& input)
  { for (int j=0; j<this->NumRows; j++)
      for (int i=0; i<this->NumCols; i++)
        this->elements[j][i]*=input;
  }
  void operator*=(const Matrix<coefficient>& input)
  { this->MultiplyOnTheRight(input);
  }
  void LieBracketWith(Matrix<coefficient>& right)
  { Matrix<coefficient> tempMat;
    this->LieBracket(*this, right, tempMat);
    *this=tempMat;
  }
  static void LieBracket(const Matrix<coefficient>& left, const Matrix<coefficient>& right, Matrix<coefficient>& output)
  { if (left.NumCols!=left.NumRows || right.NumCols!=right.NumRows || left.NumCols!=right.NumCols)
      crash << crash;
    Matrix<coefficient> tempPlus, tempMinus;
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
  (Matrix<coefficient>* carbonCopyMat=0, Selection* outputNonPivotColumns=0,
   Selection* outputPivotColumns=0, GlobalVariables* theGlobalVariables=0,
   std::stringstream* humanReadableReport=0, bool formatAsLinearSystem=false)
  ;
  void GaussianEliminationByRowsNoRowSwapPivotPointsByRows(int firstNonProcessedRow, Matrix<coefficient>& output, List<int>& outputPivotPointCols, Selection* outputNonPivotPoints__WarningSelectionNotInitialized);
  void GaussianEliminationEuclideanDomain
  (Matrix<coefficient>* otherMatrix=0, const coefficient& theRingMinusUnit=-1, const coefficient& theRingUnit=1,
   bool (*comparisonGEQFunction) (const coefficient& left, const coefficient& right)=0, GlobalVariables* theGlobalVariables=0);
  static bool Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(Matrix<coefficient>& A, Matrix<coefficient>& b, Matrix<coefficient>& output);
  coefficient GetDeterminant();
  coefficient GetTrace()const;
  void Transpose(GlobalVariables& theGlobalVariables){this->Transpose();}
  void AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den);
  void ScaleToIntegralForMinRationalHeightNoSignChange();
  void GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen);
  void LieBracketWith(const Matrix<coefficient>& right);
  void ApproximateLargestEigenSpace(Vectors<Rational>& outputBasis, const Rational& DesiredError, int SuggestedOrder, int numIterations);
  void MatrixToRoot(Vector<Rational> & output);
  bool GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(List<Vectors<coefficient> >& output)const;
  void GetZeroEigenSpace(List<Vector<coefficient> >& output)const
  { Matrix<coefficient> tempMat=*this;
    tempMat.GetZeroEigenSpaceModifyMe(output);
  }
  void GetZeroEigenSpaceModifyMe(List<Vector<coefficient> >& output);
  void GetEigenspaceModifyMe(const coefficient &inputEigenValue, List<Vector<coefficient> >& outputEigenspace)
  { for(int i=0; i<this->NumCols; i++)
      this->elements[i][i] -= inputEigenValue;
    this->GetZeroEigenSpaceModifyMe(outputEigenspace);
  }
  static bool SystemLinearInequalitiesHasSolution(Matrix<coefficient>& matA, Matrix<coefficient>& matb, Matrix<coefficient>& outputPoint);
  static bool SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(Matrix<coefficient>& matA, Matrix<coefficient>& matb, Vector<coefficient>* outputSolution=0, GlobalVariables* theGlobalVariables=0);
  static void ComputePotentialChangeGradient(Matrix<coefficient>& matA, Selection& BaseVariables, int NumTrueVariables, int ColumnIndex, Rational& outputChangeGradient, bool& hasAPotentialLeavingVariable);
  static void GetMaxMovementAndLeavingVariableRow
  (Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables, Matrix<coefficient>& tempMatA, Vector<coefficient>& inputVectorX,
   Selection& BaseVariables);
  int FindPositiveLCMCoefficientDenominatorsTruncated();
  int FindPositiveGCDCoefficientNumeratorsTruncated();
  Matrix<coefficient> operator-(const Matrix<coefficient>& right)const
  { Matrix<coefficient> tempMat=(*this);
    tempMat-=right;
    return tempMat;
  }
  Matrix<coefficient> operator*(const Matrix<coefficient>& right)const;
  Vector<coefficient> operator*(const Vector<coefficient>& v) const;

  void operator=(const Matrix<coefficient>& other);
  template<class otherType>
  void operator=(const Matrix<otherType>& other);
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
  bool IncrementReturnFalseIfPastLast()
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
  bool IncrementReturnFalseIfPastLast();
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
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "r: " << this->NumRows << " c: " << this->NumCols << "\n";
  for (int i=0; i<this->NumRows; i++)
  { for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j].WriteToFile(output);
      output << " ";
    }
    output << "\n";
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

template <typename Element>
bool Matrix<Element>::ReadFromFile(std::fstream& input)
{ int r, c; std::string tempS;
  int NumReadWords;
  XML::ReadThroughFirstOpenTag(input, NumReadWords, this->GetXMLClassName());
  if(NumReadWords!=0)
    crash << crash;
  input >> tempS >> r >> tempS >> c;
  if (tempS!="c:")
  { crash << crash;
    return false;
  }
  this->init(r, c);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j].ReadFromFile(input);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumReadWords, this->GetXMLClassName());
  if(NumReadWords!=0)
    crash << crash;
  return true;
}

template <typename Element>
bool Matrix<Element>::Invert(GlobalVariables* theGlobalVariables)
{ if (this->NumCols!=this->NumRows)
    crash << "This is a programming error: requesting to invert a non-square matrix of " << this->NumRows << " rows and "
    << this->NumCols << " columns. " << crash;
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
    crash << "This is a programming error: attempting to multiply a matrix with " << standsOnTheLeft.NumCols
    << " columns by a matrix with " << this->NumRows << "rows. " << crash;
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
{ if(this==&m1 || this==&m2)
    crash << crash;
  this->Resize(m1.NumRows+m2.NumRows, m1.NumCols+m2.NumCols, false);
  this->MakeZero();
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
  Element* tmp = this->elements[row1];
  this->elements[row1] = this->elements[row2];
  this->elements[row2] = tmp;
}

template <typename Element>
bool Matrix<Element>::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output)
{ if(A.NumRows!= b.NumRows)
    crash << crash;
  Selection thePivotPoints;
  A.GaussianEliminationByRows(&b, 0, &thePivotPoints);
  return A.RowEchelonFormToLinearSystemSolution(thePivotPoints, b, output);
}

template <typename Element>
bool Matrix<Element>::RowEchelonFormToLinearSystemSolution(Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide, Matrix<Element>& outputSolution)
{ if(inputPivotPoints.MaxSize!=this->NumCols || inputRightHandSide.NumCols!=1 || inputRightHandSide.NumRows!=this->NumRows)
    crash << crash;
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
inline void Matrix<Element>::MakeZero(const Element& theRingZero)
{ for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j]=theRingZero;
}

template <typename Element>
inline void Matrix<Element>::init(int r, int c)
{ this->Resize(r, c, false);
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

template <class Object>
template <class otherType>
void List<Object>::QuickSortAscendingNoOrder(int BottomIndex, int TopIndex, List<otherType>* carbonCopy)
{ if (TopIndex<=BottomIndex)
    return;
  int HighIndex = TopIndex;
  for (int LowIndex = BottomIndex+1; LowIndex<=HighIndex; LowIndex++)
    if (this->TheObjects[LowIndex]>(this->TheObjects[BottomIndex]))
    { this->SwapTwoIndices(LowIndex, HighIndex);
      if (carbonCopy!=0)
        carbonCopy->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex--;
      HighIndex--;
    }
  if (this->TheObjects[HighIndex]>this->TheObjects[BottomIndex])
    HighIndex--;
  this->SwapTwoIndices(BottomIndex, HighIndex);
  if (carbonCopy!=0)
    carbonCopy->SwapTwoIndices(BottomIndex, HighIndex);
  this->QuickSortAscendingNoOrder(BottomIndex, HighIndex-1, carbonCopy);
  this->QuickSortAscendingNoOrder(HighIndex+1, TopIndex, carbonCopy);
}

template <class Object>
template <class otherType>
void List<Object>::QuickSortAscendingOrder(int BottomIndex, int TopIndex, List<Object>::OrderLeftGreaterThanRight theOrder, List<otherType>* carbonCopy)
{ if (TopIndex<=BottomIndex)
    return;
  int HighIndex = TopIndex;
  for (int LowIndex = BottomIndex+1; LowIndex<=HighIndex; LowIndex++)
    if (theOrder(this->TheObjects[LowIndex],(this->TheObjects[BottomIndex])))
    { this->SwapTwoIndices(LowIndex, HighIndex);
      if (carbonCopy!=0)
        carbonCopy->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex--;
      HighIndex--;
    }
  if (theOrder(this->TheObjects[HighIndex], this->TheObjects[BottomIndex]))
  { if (HighIndex==BottomIndex)
      crash << "This is a programming error. The programmer has given me a bad strict order: the order claims that object of index "
      << HighIndex << " is strictly greater than itself which is not allowed for strict orders. Maybe the programmer has given a "
      << "non-strict order instead of strict one by mistake? " << crash;
    HighIndex--;
  }
  this->SwapTwoIndices(BottomIndex, HighIndex);
  if (carbonCopy!=0)
    carbonCopy->SwapTwoIndices(BottomIndex, HighIndex);
  this->QuickSortAscendingOrder(BottomIndex, HighIndex-1, theOrder, carbonCopy);
  this->QuickSortAscendingOrder(HighIndex+1, TopIndex, theOrder, carbonCopy);
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
    crash << "This is a programming error: requested to the elements with indices " << index1 << " and " << index2 << " in a list that has "
    << this->size << " elements. This is impossible. " << crash;
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
bool List<Object>::operator<(const List<Object>& other)const
{ if (this->size>other.size)
    return false;
  if (other.size>this->size)
    return true;
  for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i]>other.TheObjects[i])
      return false;
    if (other.TheObjects[i]>this->TheObjects[i])
      return true;
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
    crash << "This is a programming error: trying to fetch the last object of an array with " << this->size << " elements. " << crash;
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
  { crash << "Programming error: attempting to pop empty list" << crash;
  }
  this->size--;
}

template <class Object>
Object List<Object>::PopLastObject()
{ if (this->size==0)
    crash << "Programming error: attempting to pop empty list" << crash;
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
  this->flagDeallocated=true;
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
    crash << "This is a programming error: the actual size of the list is " << this->ActualSize << " but this->size equals " << this->size
    << ". " << crash;
  if (this->size==this->ActualSize)
    this->ExpandArrayOnTop(this->GetNewSizeRelativeToExpectedSize(this->ActualSize+1)- this->size);
  this->TheObjects[size]=o;
  this->size++;
}
#endif
