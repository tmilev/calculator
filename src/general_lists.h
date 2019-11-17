//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_h_already_included
#define vpfHeader1_h_already_included

#include "macros.h"
#include "general_multitasking.h"
#include <algorithm>
#include <string.h>

static ProjectInformationInstance vpfHeader1instance(__FILE__, "Header, general routines. ");

// IMPORTANT.
// Convention on Hash functions.
// 1. C++ objects that represent mathematically equal objects
//    are allowed to have different bit representations in RAM memory.
// 2. Mathematically equal objects must have their object::HashFunction return identical
//    values, even if the objects have bitwise different representations in RAM.
// 3. Mathematical objects representing 0 in an abelian group
//    must have their hash function return 0.
//
// In particular, zero rational numbers, zero polynomials/monomial collections/elements
// of semisimple Lie algebras, etc. must have their hash functions return 0.
// Motivation: for speed purposes, it is not desirable to fix unique
// bitwise representations in RAM memory for mathematically equal objects.
// For example, this means that monomials in a polynomial do not
// need to be ordered in a specific order. Yet polynomials can be quickly compared by
// computing their hash functions. Here, as an example we may give two 1-million monomial polynomials
// (computed, say, by adding two 0.5 million monomial polynomials in two different orders). The
// polynomials' monomials are ordered differently, yet the polynomials can quickly be compared.
// Note that sorting N monomials of an arbitrary polynomial is at best O(N log(N) ) operations, while
// computing the hash functions is only O(N) operations.

//used for hashing various things.
const int SomeRandomPrimesSize = 25;
const unsigned int SomeRandomPrimes[SomeRandomPrimesSize] = {
  607,  1013, 2207, 3001, 4057, 5419, 5849, 6221,
  7057, 7411, 7417, 7681, 7883, 8011, 8209, 8369, 8447,
  9539, 10267, 10657, 11489, 12071, 12613, 13933, 14759
};

//the following class is for buffers, i/o function pointers, multitasking, general purpose.
class GlobalVariables;

//Simple math routines (min, max, etc.) that I have not been able to find a better placement for
class MathRoutines;

//Rationals and integers:
class LargeIntegerUnsigned;
class LargeInteger;
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
class SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms;
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
template <class ObjectType1, class ObjectType2, unsigned int hashFunction1(const ObjectType1&) = ObjectType1::HashFunction, unsigned int hashFunction2(const ObjectType2&)=ObjectType2::HashFunction>
class Pair;
template <class Object>
class List;
template <class Object>
class ListZeroAfterUse;
template <class Object>
class Matrix;
template <class Object, unsigned int hashFunction(const Object&)>
class HashedList;
template <class coefficient>
class Vector;
template <class coefficient>
class Vectors;
class MonomialP;
template <class templateMonomial, class coefficient>
class MonomialCollection;
template <class coefficient>
class Polynomial;
template <class coefficient>
class PolynomialSubstitution;

//algebraic geometry computation class:
template <class coefficient>
class GroebnerBasisComputation;

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

// The calculator parsing routines:
class Calculator;
class Function;
class Expression;

// this class is used as a custom completely portable
// stack trace log.
class RegisterFunctionCall {
public:
  int threadIndex;
  RegisterFunctionCall(const char* fileName, int line, const std::string& functionName = "");
  ~RegisterFunctionCall();
};

typedef void (*drawLineFunction)(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
typedef void (*drawTextFunction)(double X1, double Y1, const char* theText, int length, int ColorIndex, int fontSize);
typedef void (*drawCircleFunction)(double X1, double Y1, double radius, unsigned long thePenStyle, int ColorIndex);
typedef void (*drawClearScreenFunction)();

class MathRoutines {
public:
  template <class coefficient>
  static bool InvertXModN(const coefficient& X, const coefficient& N, coefficient& output) {
    coefficient q, r, p, d; // d - divisor, q - quotient, r - remainder, p is the number to be divided
    coefficient vD[2], vP[2], temp;
    vP[0] = 1;
    vP[1] = 0; // at any given moment, p = vP[0] * N + vP[1] * X
    vD[0] = 0;
    vD[1] = 1; // at any given moment, d = vD[0] * N + vD[1] * X
    p = N;
    d = X;
    d %= N;
    if (d < 0) {
      d += N;
    }
    while (d > 0) {
      q = p / d;
      r = p % d;
      p = d;
      d = r;
      for (int i = 0; i < 2; i ++) {
        temp = vP[i];
        vP[i] = vD[i];
        vD[i] = temp - q * vD[i];
      }
    }
    if (!(p == 1)) {
      return false;//d and p were not relatively prime.
    }
    p = vP[1] % N;
    if (p < 0) {
      p += N;
    }
    output = p;
    return true;
  }
  static int lcm(int a, int b);
  template <typename integral>
  static integral gcd(integral a, integral b) {
    integral temp;
    while (!(b == 0)) {
      temp = a % b;
      a = b;
      b = temp;
    }
    return a;
  }
  template <typename integral>
  static integral lcm(integral a, integral b) {
    integral result;
    result = a;
    result /= MathRoutines::gcd(a, b);
    result *= b;
    return result;
  }
  static int TwoToTheNth(int n);
  static bool isALatinLetter(char input);
  static bool isADigit(const std::string& input, int* whichDigit = nullptr);
  static bool hasDecimalDigitsOnly(const std::string& input);
  static bool IsAHexDigit(char digitCandidate);
  static bool isADigit(char theChar, int* whichDigit = nullptr);
  template <class theType>
  static bool GenerateVectorSpaceClosedWRTLieBracket(List<theType>& inputOutputElts, int upperDimensionBound) {
    return MathRoutines::GenerateVectorSpaceClosedWRTOperation(
      inputOutputElts, upperDimensionBound, theType::LieBracket
    );
  }
  static Vector<double> GetVectorDouble(Vector<Rational>& input);
  template <class theType>
  static bool GenerateVectorSpaceClosedWRTOperation(
    List<theType>& inputOutputElts,
    int upperDimensionBound,
    void (*theBinaryOperation)(const theType& left, const theType& right, theType& output)
  );

  static char ConvertHumanReadableHexToCharValue(char input);
  static void NChooseK(int n, int k, LargeInteger& result);
  static int NChooseK(int n, int k);
  static int Factorial(int n);
  static inline double E() {
    return 2.718281828459;
  }
  static inline double Pi() {
    return 3.141592653589793238462643383279;
  }
  // the MS compiler refuses to compile the following, hence the above line.
  //static const double Pi =(double)3.141592653589793238462643383279;
  static int KToTheNth(int k, int n);
  static void KToTheNth(int k, int n, LargeInteger& output);
  inline static int parity(int n);
  static int BinomialCoefficientMultivariate(int N, List<int>& theChoices);
  static bool IsPrime(int theInt);
  template <class coefficient, typename theIntegerType>
  static void RaiseToPower(
    coefficient& theElement,
    const theIntegerType& thePower,
    const coefficient& theRingUnit
  );
  template <typename coefficient>
  inline static coefficient Maximum(const coefficient& a, const coefficient& b) {
    if (a > b) {
      return a;
    } else {
      return b;
    }
  }
  template <typename T>
  inline static void swap(T& a, T& b) {
    if (&a == &b) {
      return;
    }
    T temp;
    temp = a;
    a = b;
    b = temp;
  }
  template <class Element>
  inline static Element Minimum(const Element& a, const Element& b) {
    if (a > b) {
      return b;
    } else {
      return a;
    }
  }
  template<class Element>
  static std::string ElementToStringBrackets(const Element& input) {
    if (!input.NeedsParenthesisForMultiplication()) {
      return input.ToString();
    }
    std::string result;
    result.append("(");
    result.append(input.ToString());
    result.append(")");
    return result;
  }
  template<class Element>
  static std::string ElementToStringBrackets(const Element& input, FormatExpressions* theFormat) {
    if (!input.NeedsParenthesisForMultiplication()) {
      return input.ToString(theFormat);
    }
    std::string result;
    result.append("(");
    result.append(input.ToString(theFormat));
    result.append(")");
    return result;
  }
  static double ReducePrecision(double x);
  inline static unsigned int HashDouble(const double& input) {
    return static_cast<unsigned>(input * 10000);
  }
  static unsigned int HashVectorDoubles(const Vector<double>& input);
  static unsigned int HashListDoubles(const List<double>& input);
  static unsigned int HashListInts(const List<int>& input);
  static unsigned int HashListUnsignedChars(const List<unsigned char>& input);
  static unsigned int HashListStrings(const List<std::string>& input);
  inline static unsigned int IntUnsignIdentity(const int& input) {
    return static_cast<unsigned>(input);
  }
  static unsigned int HashString(const std::string& x);
  static unsigned int HashChar(const char& x) {
    return static_cast<unsigned int>(x);
  }
  template <class Element>
  static void LieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output);
  template <typename number>
  static number ComplexConjugate(number x) {
    return x.GetComplexConjugate();
  }
  static int ComplexConjugate(int x) {
    return x;
  }
  static double ComplexConjugate(double x) {
    return x;
  }
  static bool IsInteger(Rational x);
  template <typename hashobject>
  static unsigned int HashFunction(const hashobject& in) {
    return in.HashFunction();
  }
  static unsigned int HashFunction(bool in) {
    if (in) {
      return 1;
    } else {
      return 0;
    }
  }
  template <class templateList, typename objectType, class otherList>
  static void QuickSortAscending(
    templateList& theList,
    bool theOrder(const objectType& left, const objectType& right) = 0,
    otherList* carbonCopy = 0
  ) {
    if (carbonCopy != 0) {
      if (carbonCopy->size != theList.size) {
        crash << "Programming error: requesting quicksort with carbon copy on a list with "
        << theList.size << " elements, but the but the carbon copy has "
        << carbonCopy->size << " elements. " << crash;
      }
    }
    if (theList.size == 0) {
      return;
    }
    if (theOrder == 0) {
      MathRoutines::QuickSortAscendingNoOrder(theList, 0, theList.size - 1, carbonCopy);
    } else {
      MathRoutines::QuickSortAscendingOrder(theList, 0, theList.size - 1, theOrder, carbonCopy);
    }
  }
  static bool ParseListInt(const std::string& input, List<int>& result, std::stringstream* commentsOnFailure);
  static void ParseListIntCrashOnFailure(const std::string& input, List<int>& result);
  template <class templateList, typename objectType, class otherList>
  static void QuickSortDescending(
    templateList& theList,
    bool theOrder(const objectType& left, const objectType& right) = 0,
    otherList* carbonCopy = 0
  ) {
    MathRoutines::QuickSortAscending(theList, theOrder, carbonCopy);
    theList.ReverseOrderElements();
    if (carbonCopy != 0) {
      carbonCopy->ReverseOrderElements();
    }
  }
private:
  template <class templateList, typename objectType, class otherList>
  static void QuickSortAscendingOrder(
    templateList& theList,
    int BottomIndex,
    int TopIndex,
    bool theOrder(const objectType& left, const objectType& right),
    otherList* carbonCopy
  ) {
    if (TopIndex <= BottomIndex) {
      return;
    }
    int HighIndex = TopIndex;
    for (int LowIndex = BottomIndex + 1; LowIndex <= HighIndex; LowIndex ++) {
      if (theOrder(theList[LowIndex],(theList[BottomIndex]))) {
        theList.SwapTwoIndices(LowIndex, HighIndex);
        if (carbonCopy != 0) {
          carbonCopy->SwapTwoIndices(LowIndex, HighIndex);
        }
        LowIndex --;
        HighIndex --;
      }
    }
    if (theOrder(theList[HighIndex], theList[BottomIndex])) {
      if (HighIndex == BottomIndex) {
        std::stringstream crashStream;
        crashStream << "This is a programming error. "
        << "The programmer has given me a bad strict order: the order claims that object "
        << theList[HighIndex] << " of index "
        << HighIndex << " is strictly greater than itself which is not allowed for strict orders. "
        << "Maybe the programmer has given a "
        << "non-strict order instead of strict one by mistake? ";
        crash << crashStream.str() << crash;
      }
      HighIndex --;
    }
    theList.SwapTwoIndices(BottomIndex, HighIndex);
    if (carbonCopy != 0) {
      carbonCopy->SwapTwoIndices(BottomIndex, HighIndex);
    }
    MathRoutines::QuickSortAscendingOrder(theList, BottomIndex, HighIndex - 1, theOrder, carbonCopy);
    MathRoutines::QuickSortAscendingOrder(theList, HighIndex + 1, TopIndex, theOrder, carbonCopy);
  }
  template <class templateList, class otherList>
  static void QuickSortAscendingNoOrder(
    templateList& theList,
    int BottomIndex,
    int TopIndex,
    otherList* carbonCopy
  ) {
    if (TopIndex <= BottomIndex) {
      return;
    }
    int HighIndex = TopIndex;
    for (int LowIndex = BottomIndex + 1; LowIndex <= HighIndex; LowIndex ++) {
      if (theList[LowIndex] > theList[BottomIndex]) {
        theList.SwapTwoIndices(LowIndex, HighIndex);
        if (carbonCopy != 0) {
          carbonCopy->SwapTwoIndices(LowIndex, HighIndex);
        }
        LowIndex --;
        HighIndex --;
      }
    }
    if (theList[HighIndex] > theList[BottomIndex]) {
      HighIndex --;
    }
    theList.SwapTwoIndices(BottomIndex, HighIndex);
    if (carbonCopy != 0) {
      carbonCopy->SwapTwoIndices(BottomIndex, HighIndex);
    }
    MathRoutines::QuickSortAscendingNoOrder(theList, BottomIndex, HighIndex - 1, carbonCopy);
    MathRoutines::QuickSortAscendingNoOrder(theList, HighIndex + 1, TopIndex, carbonCopy);
  }
};

class XML {
private:
  static std::string GetOpenTagNoInputCheck(const std::string& tagNameNoSpacesNoForbiddenCharacters);
  static std::string GetCloseTagNoInputCheck(const std::string& tagNameNoSpacesNoForbiddenCharacters);
public:
  std::string theString;
  unsigned positionInString;
  XML();
  bool ReadFromFile(std::fstream& inputFile);
  static std::string GetOpenTagNoInputCheckAppendSpacE(const std::string& tagNameNoSpacesNoForbiddenCharacters);
  static std::string GetCloseTagNoInputCheckAppendSpacE(const std::string& tagNameNoSpacesNoForbiddenCharacters);
  static bool ReadThroughFirstOpenTag(
    std::istream& streamToMoveIn,
    const std::string& tagNameNoSpacesNoForbiddenCharacters
  );
  static bool ReadEverythingPassedTagOpenUntilTagClose(
    std::istream& streamToMoveIn, const std::string& tagNameNoSpacesNoForbiddenCharacters
  );
  static bool ReadThroughFirstOpenTag(
    std::istream& streamToMoveIn,
    int& NumReadWordsExcludingTag,
    const std::string& tagNameNoSpacesNoForbiddenCharacters
  );
  static bool ReadEverythingPassedTagOpenUntilTagClose(
    std::istream& streamToMoveIn,
    int& NumReadWordsExcludingTag,
    const std::string& tagNameNoSpacesNoForbiddenCharacters
  );
  bool GetStringEnclosedIn(const std::string& theTagName, std::string& outputString);
};

class DrawElementInputOutput {
public:
  int TopLeftCornerX;
  int TopLeftCornerY;
  int outputWidth;
  int outputHeight;
};

// This object wraps a regular pointer
// and ensures the regular pointer is deleted
// when the object goes out of scope.
template <class Object>
class PointerObjectDestroyer {
  public:
  Object** theObjectPointer;

  void RenewObject() {
    if (this->theObjectPointer == 0) {
      crash << "Unexpeced: uninitialized object pointer. " << crash;
    }
    delete *(this->theObjectPointer);
    *(this->theObjectPointer) = 0;
    *(this->theObjectPointer) = new Object;
  }
  PointerObjectDestroyer() {
    this->theObjectPointer = 0;
  }
  PointerObjectDestroyer(Object*& inputObject): theObjectPointer(&inputObject) {
  }
  ~PointerObjectDestroyer() {
    if (this->theObjectPointer == 0) {
      return;
    }
    delete *(this->theObjectPointer);
    *(this->theObjectPointer) = 0;
  }
};

class RecursionDepthCounter {
public:
  int* theCounter;
  RecursionDepthCounter(int* inputCounter): theCounter(nullptr) {
    if (inputCounter == nullptr) {
      return;
    }
    this->theCounter = inputCounter;
    (*this->theCounter) ++;
  }
  ~RecursionDepthCounter() {
    if (this->theCounter != nullptr) {
      (*this->theCounter) --;
    }
    this->theCounter = nullptr;
  }
};

//The below class is to be used together with List.
//The purpose of the class is to save up RAM memory use.
//This is the "light" version it is to be used for storage purposes only.
//To use it as a normal List simply copy it to a buffer List and there
//use the full functionality of List.
//Then copy the buffer List back to the light version to store to RAM.
template <class Object>
class ListLight {
private:
  ListLight(const ListLight<Object>& right);
public:
  int size;
  Object* TheObjects;
  void AddObjectOnTopLight(const Object& o);
  void CopyFromHeavy(const List<Object>& from);
  void CopyFromLight(const ListLight<Object>& from);
  void PopIndexSwapWithLastLight(int index);
  int SizeWithoutObjects() const;
  int IndexInList(const Object& o) {
    for (int i = 0; i < this->size; i ++) {
      if (this->TheObjects[i] == o) {
        return i;
      }
    }
    return - 1;
  }
  bool Contains(const Object& o) {
    return this->IndexInList(o) != - 1;
  }
  void SetSize(int theSize);
  void initializeFillInObject(int theSize, const Object& o) {
    this->SetSize(theSize);
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] = o;
    }
  }
  Object& operator[](int i) const {
    return this->TheObjects[i];
  }
  void operator=(const ListLight<Object>& right);
  void operator=(const List<Object>& right) {
    this->SetSize(right.size);
    for (int i = 0; i < right.size; i ++) {
      this->TheObjects[i] = right[i];
    }
  }
  bool operator== (const ListLight<Object>& right) const {
    if (this->size != right.size) {
      return false;
    }
    for (int i = 0; i < this->size; i ++) {
      if (!(this->TheObjects[i] == right[i])) {
        return false;
      }
    }
    return true;
  }
  Object* LastObject() const {
    return &this->TheObjects[this->size - 1];
  }
  ListLight();
  ~ListLight();
};

template <class Object>
void ListLight<Object>::AddObjectOnTopLight(const Object& o) {
  this->SetSize(this->size + 1);
  this->TheObjects[this->size - 1] = o;
}

template <class Object>
void ListLight<Object>::PopIndexSwapWithLastLight(int index) {
  this->TheObjects[index] = this->TheObjects[this->size - 1];
  this->SetSize(this->size - 1);
}

template <class Object>
void ListLight<Object>::operator=(const ListLight<Object>& right) {
  this->CopyFromLight(right);
}

template <class Object>
void ListLight<Object>::SetSize(int theSize) {
  if (theSize == this->size) {
    return;
  }
  if (theSize < 0) {
    theSize = 0;
  }
  if (theSize == 0) {
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter -= this->size;
  ParallelComputing::CheckPointerCounters();
#endif
    this->size = 0;
    delete [] this->TheObjects;
    this->TheObjects = 0;
    return;
  }
  Object* newArray = new Object[theSize];
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter += theSize;
  ParallelComputing::CheckPointerCounters();
#endif
  int CopyUpTo = this->size;
  if (this->size > theSize) {
    CopyUpTo = theSize;
  }
  for (int i = 0; i < CopyUpTo; i ++) {
    newArray[i] = this->TheObjects[i];
  }
  delete [] this->TheObjects;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter -= this->size;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheObjects = newArray;
  this->size = theSize;
}

template <class Object>
ListLight<Object>::ListLight() {
  this->size = 0;
  this->TheObjects = 0;
}

template <class Object>
ListLight<Object>::~ListLight() {
  delete [] this->TheObjects;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter -= this->size;
  ParallelComputing::CheckPointerCounters();
#endif
  this->TheObjects = 0;
}

template <class Object>
void ListLight<Object>::CopyFromHeavy(const List<Object>& from) {
  this->SetSize(from.size);
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i] = from.TheObjects[i];
  }
}

template <class Object>
void ListLight<Object>::CopyFromLight(const ListLight<Object>& from) {
  this->SetSize(from.size);
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i] = from.TheObjects[i];
  }
}

template <class Object>
int ListLight<Object>::SizeWithoutObjects() const {
  return sizeof(Object*) * this->size + sizeof(int);
}

template <class Object>
std::ostream& operator<<(std::ostream& output, const List<Object>& theList) {
  output << theList.size << " elements: \n";
  for (int i = 0; i < theList.size; i ++) {
    output << theList[i];
    if (i != theList.size - 1) {
      output << ", ";
    }
  }
  return output;
}

template <class Object>
std::iostream& operator>>(std::iostream& input, List<Object>& theList) {
  std::string tempS;
  int tempI;
  input >> tempS >> tempI;
  if (tempS != "size:") {
    crash << "Failed reading list from stream. " << crash;
  }
  theList.SetSize(tempI);
  for (int i = 0; i < theList.size; i ++) {
    input >> theList[i];
  }
  return input;
}

//List serves the same purpose as std::vector
//List is not thread safe!!!!
//Lists are used in the implementation of mutexes!!!
template <class Object>
class List {
  friend std::ostream& operator<< <Object>(std::ostream& output, const List<Object>& theList);
  friend std::iostream& operator>> <Object>(std::iostream& input, List<Object>& theList);
public:
  typedef bool (*OrderLeftGreaterThanRight) (const Object& left, const Object& right);
private:
  friend class ListZeroAfterUse<Object>;
  friend class Polynomial<Rational>;
  friend class IntegerPoly;
  friend class PartFractions;
  friend class PartFraction;
  void ExpandArrayOnTop(int increase);
  template <class compareClass, class carbonCopyType>
  bool QuickSortAscendingCustomRecursive(
    int BottomIndex, int TopIndex, compareClass& theCompareror, List<carbonCopyType>* carbonCopy
  );
  void QuickSortDescending(int BottomIndex, int TopIndex);
  inline void initConstructorCallOnly() {
    this->TheObjects = 0;
    this->ActualSize = 0;
    this->size = 0;
    this->flagDeallocated = false;
    MacroIncrementCounter(ParallelComputing::NumListsCreated);
  }
  int ActualSize;
public:
  bool flagDeallocated;
  Object* TheObjects;
  int size;
  List() {
    this->initConstructorCallOnly();
  }
  List(const std::string& input) {
    this->initConstructorCallOnly();
    *this = input;
  }
  List(const std::initializer_list<Object>& input) {
    this->initConstructorCallOnly();
    *this = input;
  }
  List(const List<Object>& other) {
    this->initConstructorCallOnly();
    *this = other;
  }
  List(const ListLight<Object>& other) {
    this->initConstructorCallOnly();
    this->AssignLight(other);
  }
  List<Object> SliceCopy(int StartingIndex, int SizeOfSlice) const;
  void SliceInPlace(int StartingIndex, int SizeOfSlice);
  void Slice(int StartingIndex, int SizeOfSlice, List<Object>& output) const;
  ~List();
  bool CheckConsistency() const {
    if (this->flagDeallocated) {
      crash << "This is a programming error: use after free of List. " << crash;
    }
    return true;
  }
  static std::string GetXMLClassName() {
    std::string result = "List_";
    result.append(Object::GetXMLClassName());
    return result;
  }
  int GetNewSizeRelativeToExpectedSize(int expectedSize) const {
    // <-Registering stack trace forbidden! Multithreading deadlock alert.
    if (expectedSize == 1) {
      return 1;
    }
    if (expectedSize == 2) {
      return 2;
    }
    return (expectedSize * 4) / 3 + 1;
  }
  void SetExpectedSize(int theSize) {
    // <-Registering stack trace forbidden! Multithreading deadlock alert.
    if ((this->ActualSize * 5 / 6) < theSize) {
      this->Reserve(this->GetNewSizeRelativeToExpectedSize(theSize));
    }
  }
  void AssignLight(const ListLight<Object>& from);
  void ExpandOnTop(int theIncrease) {
    int newSize = this->size + theIncrease;
    if (newSize < 0) {
      newSize = 0;
    }
    this->SetSize(newSize);
  }
  void SetSize(int theSize);// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void SetSizeMakeMatrix(int numRows, int numCols) {
    this->SetSize(numRows);
    for (int i = 0; i < numRows; i ++) {
      this->TheObjects[i].SetSize(numCols);
    }
  }
  void initializeFillInObject(int theSize, const Object& o);
  void AddObjectOnTopCreateNew();
  void Reserve(int theSize);// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void SortedInsert(const Object& o) {
    this->BSInsert(o); //fixed :) now all this function does is throw away the return value
  }
  void InsertAtIndexShiftElementsUp(const Object& o, int desiredIndex) {
    this->ShiftUpExpandOnTop(desiredIndex);
    (*this)[desiredIndex] = o;
  }
  void AddOnTop(const Object& o);
  void AddListOnTop(const List<Object>& theList);
  bool AddOnTopNoRepetition(const Object& o);
  void AddOnTopNoRepetition(const List<Object>& theList) {
    this->SetExpectedSize(this->size + theList.size);
    for (int i = 0; i < theList.size; i ++) {
      this->AddOnTopNoRepetition(theList[i]);
    }
  }
  int AddNoRepetitionOrReturnIndexFirst(const Object& o) {
    int indexOfObject = this->GetIndex(o);
    if (indexOfObject == - 1) {
      this->AddOnTop(o);
      return this->size - 1;
    }
    return indexOfObject;
  }
  void RemoveIndexShiftDown(int index) {
    for (int i = index; i < this->size - 1; i ++) {
      this->TheObjects[i] = this->TheObjects[i + 1];
    }
    this->size --;
  }
  void RemoveIndicesShiftDown(int firstToRemove, int numberToRemove) {
    for (int i = firstToRemove; i < this->size - numberToRemove; i ++) {
      this->TheObjects[i] = this->TheObjects[i + numberToRemove];
    }
    this->size -= numberToRemove;
  }
  void RemoveIndexSwapWithLast(int index);
  void RemoveLastObject();
  void RemoveObjectsShiftDown(const List<Object>& theList) {
    int currentIndex = 0;
    for (int i = 0; i < this->size; i ++) {
      if (!theList.Contains((*this)[i])) {
        theList.SwapTwoIndices(i, currentIndex);
        currentIndex ++;
      }
    }
    this->SetSize(currentIndex);
  }
  // The below function is named a bit awkwardly because otherwise there is a risk of confusion
  // with the RemoveIndexSwapWithLast when selecting from autocomplete list.
  // This cost me already 2 hours of lost time,
  // so the awkward name is necessary.
  void RemoveFirstOccurenceSwapWithLast(const Object& o);
  Object PopLastObject();
  Object PopIndexSwapWithLast(int index) {
    Object result;
    result = (*this)[index];
    this->RemoveIndexSwapWithLast(index);
    return result;
  }
  Object PopIndexShiftDown(int index) {
    Object result;
    result = (*this)[index];
    this->RemoveIndexShiftDown(index);
    return result;
  }
  //The following function returns false if the comparison operator failed!!!!
  template <class compareClass, class carbonCopyType = Object>
  bool QuickSortAscendingCustom(compareClass& theCompareror, List<carbonCopyType>* carbonCopy = nullptr) {
    return this->QuickSortAscendingCustomRecursive(0, this->size - 1, theCompareror, carbonCopy);
  }
  template <class compareClass, class carbonCopyType = Object>
  bool QuickSortDescendingCustom(compareClass& theCompareror, List<carbonCopyType>* carbonCopy = nullptr) {
    bool result = this->QuickSortAscendingCustomRecursive(0, this->size - 1, theCompareror, carbonCopy);
    this->ReverseOrderElements();
    if (carbonCopy != 0) {
      carbonCopy->ReverseOrderElements();
    }
    return result;
  }
  //The below function is required to pReserve the order of elements given by theSelection.elements.
  void SubSelection(const Selection& theSelection, List<Object>& output);
  //If comparison function is not specified, QuickSortAscending uses operator>, else it uses the given
  //comparison function
  template <class otherType = Object>
  void QuickSortAscending(List<Object>::OrderLeftGreaterThanRight theOrder = nullptr, List<otherType>* carbonCopy = nullptr) {
    MathRoutines::QuickSortAscending(*this, theOrder, carbonCopy);
  }
  template <class otherType = Object>
  void QuickSortDescending(List<Object>::OrderLeftGreaterThanRight theOrder = nullptr, List<otherType>* carbonCopy = nullptr) {
    MathRoutines::QuickSortDescending(*this, theOrder, carbonCopy);
  }
  bool HasACommonElementWith(List<Object>& right);
  void SwapTwoIndices(int index1, int index2);
  void CycleIndices(const List<int>& cycle);
  void PermuteIndices(const List<List<int> >& cycles);
  std::string ToStringConcatenate() const;
  std::string ToString(FormatExpressions* theFormat) const;
  std::string ToString() const;
  std::string ToStringCommaDelimited(FormatExpressions* theFormat) const;
  std::string ToStringCommaDelimited() const;
  void ToString(std::string& output, FormatExpressions* theFormat = nullptr) const {
    output = this->ToString(theFormat);
  }
  int GetIndex(const Object& o) const;
  bool Contains(const Object& o) const {
    return this->GetIndex(o) != - 1;
  }
  bool ContainsAtLeastOneCopyOfEach(const List<Object>& other) const {
    for (int i = 0; i < other.size; i ++) {
      if (!this->Contains(other[i])) {
        return false;
      }
    }
    return true;
  }

  // The BS family of operations assume that the list is sorted in ascending order
  // and that each object has a comparison operator, since there should be one, it
  // should be <=.  This project appears to use its logical complement > everywhere
  // The objective is fast, stable insertion sorting of partially ordered data.
  // The data that has x <= y and y <= x is called a bin.  The methods
  // BSIndexFirstGreaterThan and BSIndexFirstNotLessThan find the edges of the bin.
  // The insertion methods return the index at which the object was inserted, or - 1
  // BSInsert inserts the element at the end of the bin.
  // BSInsertDontDup searches the bin for the element before inserting.
  // BSInsertNextTo searches the bin for an equal element and inserts the new element
  //   next to the existing element
  // BSGetIndex searches the bin for the element and returns its index
  // BSContains searches the bin for the element and returns whether it was found.
  int BSIndexFirstNotLessThan(const Object& o) const {
    if (this->size == 0) {
      return 0;
    }
    int start = 0;
    int end = this->size;
    while (true) {
      int halflength = (end - start) / 2;
      if (halflength == 0) {
        if ((start == this->size )|| (o > this->TheObjects[start])) {
          return end;
        } else {
          return start;
        }
      }
      int n = start + halflength;
      if (o > this->TheObjects[n]) {
        start = n + 1;
      } else {
        end = n;
      }
    }
  }

  int BSIndexFirstGreaterThan(const Object& o) const {
    if (this->size == 0) {
      return 0;
    }
    int start = 0;
    int end = this->size;
    while (true) {
      int halflength = (end - start) / 2;
      if (halflength == 0) {
        if (!(this->TheObjects[start] > o)) {
          return end;
        } else {
          return start;
        }
      }
      int n = start + halflength;
      if (!(this->TheObjects[n] > o)) {
        start = n + 1;
      } else {
        end = n;
      }
    }
  }

  // BSGetIndex
  int BSGetIndex(const Object& o) const {
    int n = this->BSIndexFirstNotLessThan(o);
    if (n != this->size) {
      if (this->TheObjects[n] == o) {
        return n;
      }
    }
    for (int i = n - 1; i > - 1; i --) {
      if (this->TheObjects[i] == o) {
        return i;
      }
      if (o > this->TheObjects[i]) {
        return - 1;
      }
    }
    return - 1;
  }
  bool BSContains(const Object& o) const {
    return this->BSGetIndex(o) != - 1;
  }
  int BSInsert(const Object& o) {
    int n = BSIndexFirstGreaterThan(o);
    if (n == this->size) {
      this->AddOnTop(o);
    } else {
      this->InsertAtIndexShiftElementsUp(o, n);
    }
    return n;
  }

  int BSInsertDontDup(const Object& o) {
    int n = BSIndexFirstNotLessThan(o);
    for (int i = n; i < this->size; i ++) {
      if (this->TheObjects[i] == o) {
        return - 1;
      }
      if (this->TheObjects[i] > o) {
        this->InsertAtIndexShiftElementsUp(o, i);
        return i;
      }
    }
    this->AddOnTop(o);
    return this->size - 1;
  }

  int BSInsertNextToExisting(const Object& o) {
    int n = BSIndexFirstNotLessThan(o);
    int existing_index = - 1;
    int bin_end_index = - 1;
    for (int i = n; i < this->size; i ++) {
      if (this->TheObjects[i] == o) {
        existing_index = i;
      }
      if (this->TheObjects[i] > o) {
        bin_end_index = i;
        break;
      }
    }
    int outdex;
    if (existing_index != - 1) {
      outdex = existing_index + 1;
    } else if (bin_end_index != - 1) {
      outdex = bin_end_index;
    } else {
      outdex = this->size;
    }
    this->InsertAtIndexShiftElementsUp(o, outdex);
    return outdex;
  }

  void Rotate(int r) {
    std::rotate(this->TheObjects, this->TheObjects + r, this->TheObjects + (this->size - 1));
  }
  bool ReadFromFile(std::fstream& input, int UpperLimitForDebugPurposes = - 1);
  void WriteToFile(std::fstream& output, int UpperLimitForDebugPurposes = - 1) const;
//  bool Contains(Object& o){return this->Contains(o) != - 1; };
  int SizeWithoutObjects() const;
  Object* LastObject() const;// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void ReleaseMemory();

  unsigned int HashFunction() const {
    int numCycles = MathRoutines::Minimum(SomeRandomPrimesSize, this->size);
    unsigned int result = 0;
    for (int i = 0; i < numCycles; i ++) {
      result += SomeRandomPrimes[i] * MathRoutines::HashFunction(TheObjects[i]);
    }
    return result;
  }
  static unsigned int HashFunction(const List<Object>& input) {
    return input.HashFunction();
  }
  void IntersectWith(const List<Object>& other, List<Object>& output) const;
  void operator=(const std::string& right) {
    this->SetSize(right.size());
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] = right[static_cast<unsigned>(i)];
    }
  }
  void operator=(const std::initializer_list<Object>& other) {
    this->SetSize(other.size());
    int counter = 0;
    for (
      auto otherIterator = other.begin();
      otherIterator != other.end();
      otherIterator ++, counter ++
    ) {
      this->TheObjects[counter] = *otherIterator;
    }
  }
  void operator=(const List<Object>& right) {
    if (this == &right) {
      return;
    }
    this->SetSize(right.size);
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] = right.TheObjects[i];
    }
  }
  template <class otherObjectType>
  void operator=(const List<otherObjectType>& right) {
    this->SetSize(right.size);
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] = right.TheObjects[i];
    }
  }
  static void swap(List<Object>& l1, List<Object>& l2);
  void ReverseOrderElements();
  void ReverseRange(int rangeBegin, int rangeEnd);
  bool IsEqualTo(const List<Object>& Other) const {
    if (this->size != Other.size) {
      return false;
    }
    for (int i = 0; i < Other.size; i ++) {
      if (!(this->TheObjects[i] == Other.TheObjects[i])) {
        return false;
      }
    }
    return true;
  }
  inline Object& operator[](unsigned i) const {
    int static_cast_i = static_cast<signed>(i);
    return (*this)[static_cast_i];
  }
  Object& operator[](int i) const {
    this->CheckConsistency();
    if ((i >= this->size) || i < 0) {
      crash << "Programming error: attempting to access the entry of index "
      << i << " in an array of " << this->size << " elements. " << crash;
    }
    return this->TheObjects[i];
  }
  bool operator!=(const List<Object>& other) const {
    return !this->IsEqualTo(other);
  }
  bool operator==(const std::string& other) {
    if ((static_cast<unsigned>(this->size)) != other.size()) {
      return false;
    }
    for (int i = 0; i < this->size; i ++) {
      if (!(this->TheObjects[i] == other[i])) {
        return false;
      }
    }
    return true;
  }
  bool operator==(const List<Object>& other) const {
    return this->IsEqualTo(other);
  }
  bool operator>(const List<Object>& other) const;
  bool operator<(const List<Object>& other) const;
  void ShiftUpExpandOnTop(int StartingIndex);
  void ShiftUpExpandOnTopRepeated(int StartingIndex, int numberOfNewElements);
  void AssignListList(const List<List<Object> >& input) {
    int count = 0;
    for (int i = 0; i < input.size; i ++) {
      count += input[i].size;
    }
    this->SetSize(0);
    this->Reserve(count);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < input[i].size; j ++) {
        this->AddOnTop(input[i][j]);
      }
    }
  }
};

template <typename Object>
class ListZeroAfterUse {
  public:
  List<Object> data;
  ~ListZeroAfterUse() {
    for (int i = 0; i < this->data.ActualSize; i ++) {
      // We zero the underlying buffer:
      // this->data[i] may be out of bounds if
      // the actual size does not equal the size.
      this->data.TheObjects[i] = 0;
    }
  }
};

template <class ObjectType1, class ObjectType2, unsigned int hashFunction1(const ObjectType1&), unsigned int hashFunction2(const ObjectType2&)>
class Pair {
  friend std::ostream& operator << (
    std::ostream& output, const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& thePair
  ) {
    output << "("  << thePair.Object1 << ", " <<  thePair.Object2 << ")";
    return output;
  }
public:
  ObjectType1 Object1;
  ObjectType2 Object2;
  Pair(){}
  Pair(const ObjectType1& o1, const ObjectType2& o2): Object1(o1), Object2(o2) {}
  static unsigned int HashFunction(
    const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& input
  ) {
    return
      SomeRandomPrimes[0] * hashFunction1(input.Object1) +
      SomeRandomPrimes[1] * hashFunction2(input.Object2);
  }
  unsigned int HashFunction() const {
    return Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>::HashFunction(*this);
  }
  void operator=(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other) {
    this->Object1 = other.Object1;
    this->Object2 = other.Object2;
  }
  bool operator==(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other) const {
    return this->Object1 == other.Object1 && this->Object2 == other.Object2;
  }
  bool operator>(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other) {
    if (this->Object1 > other.Object1) {
      return true;
    }
    if (other.Object1 > this->Object1) {
      return false;
    }
    if (this->Object2 > other.Object2) {
      return true;
    }
    if (other.Object2 > this->Object2) {
      return false;
    }
    return false;
  }
};
typedef Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> PairInts;

template <class Object, class TemplateList, unsigned int hashFunction(const Object&) = Object::HashFunction>
class HashTemplate: public TemplateList {
private:
  void AddObjectOnBottom(const Object& o);
  void AddListOnTop(List<Object>& theList);
  Object PopIndexShiftDown(int index);
  void AssignLight(const ListLight<Object>& from);
  void ReverseOrderElements();
  void ShiftUpExpandOnTop(int StartingIndex);

protected:
  List<List<int> > TheHashedArrays;
public:
  static std::string GetXMLClassName() {
    std::string result = "HashedList_";
    result.append(Object::GetXMLClassName());
    return result;
  }
  unsigned int GetHash(const Object& input) const {
    unsigned int result = hashFunction(input);
    result %= this->TheHashedArrays.size;
    return result;
  }
  void Clear() {
    // If the hashed list is somewhat sparse, and the index is somewhat large,
    // (above 20 entries), we clear the hash by finding the occupied hashes and
    // nullifying them one by one.
    // else, we simply go through the entire hash index and nullify everything.
    // Note: for better performance, 20 entries should probably be changed to 100+,
    // however the smaller number is a good consistency test (it would make it easier to
    // detect a faulty hash).
    // If this program ever gets to do some hard-core number crunching, the 20 entries
    // should be increased.
    if (this->IsSparse() && this->TheHashedArrays.size > 20) {
      for (int i = 0; i < this->size; i ++) {
        int hashIndex = this->GetHash((*this)[i]);
        this->TheHashedArrays[hashIndex].size = 0;
      }
    } else {
      for (int i = 0; i < this->TheHashedArrays.size; i ++) {
        this->TheHashedArrays[i].size = 0;
      }
    }
    this->size = 0;
  }
  std::string GetReport() {
    std::stringstream out;
    out << "<br>List size: " << this->size;
    out << "<br>Hash size: " << this->TheHashedArrays.size;
    int maxHashSize = 0;
    int numNonZeroHashes = 0;
    for (int i = 0; i < this->TheHashedArrays.size; i ++) {
      maxHashSize = MathRoutines::Maximum(maxHashSize, this->TheHashedArrays[i].size);
      if (this->TheHashedArrays[i].size > 0) {
        numNonZeroHashes ++;
      }
    }
    out << "<br>Max hash array size: " << maxHashSize;
    out << "<br>Average hash array size: " << (static_cast<double>(this->size)) / (static_cast<double>(numNonZeroHashes));
    return out.str();
  }
  void AddOnTop(const Object& o) {
    unsigned int hashIndex = this->GetHash(o);
    this->TheHashedArrays[hashIndex].AddOnTop(this->size);
    this->TemplateList::AddOnTop(o);
    if (this->size > 1) {
      this->AdjustHashes();
    }
  }
  void AddOnTop(const List<Object>& theList) {
    this->SetExpectedSize(this->size + theList.size);
    for (int i = 0; i < theList.size; i ++) {
      this->AddOnTop(theList[i]);
    }
  }
  void RemoveFirstOccurenceSwapWithLast(const Object& o) {
    int theIndex = this->GetIndex(o);
    if (theIndex == - 1) {
      return;
    }
    this->RemoveIndexSwapWithLast(theIndex);
  }
  const List<int>& GetHashArray(int hashIndex) const {
    return this->TheHashedArrays[hashIndex];
  }
  void GrandMasterConsistencyCheck() const {
    for (int i = 0; i < this->TheHashedArrays.size; i ++) {
      List<int>& current = this->TheHashedArrays[i];
      for (int j = 0; j < current.size; j ++) {
        int theIndex = current[j];
        if (theIndex >= this->size) {
          crash << "This is a programming error: hash lookup array of index "
          << i << ", entry of index " << j << " reports index "
          << theIndex << " but I have only " << this->size << " entries. " << crash;
        }
        if (this->GetHash((*this)[theIndex]) != static_cast<unsigned>(i)) {
          crash << "<hr>This is a programming error: the hashed element in position "
          << theIndex << " is recorded in hash array of index "
          << i << ", however its hash value is instead " << this->GetHash((*this)[theIndex]) << ". The hash size is "
          << this->TheHashedArrays.size << "<br>hashes of objects: ";
          for (int l = 0; l < this->size; l ++) {
            crash << this->GetHash((*this)[l]) << "= " << this->GetHash((*this)[l]) % this->TheHashedArrays.size << ", ";
          }
          crash << "<br>hashes recorded: ";
          for (int l = 0; l < this->TheHashedArrays.size; l ++) {
            for (int k = 0; k < this->TheHashedArrays[l].size; k ++) {
              crash << this->TheHashedArrays[l][k] << ", ";
            }
          }
          crash << crash;
        }
      }
    }
  }
  int AddNoRepetitionOrReturnIndexFirst(const Object& o) {
    int result = this->GetIndex(o);
    if (result != - 1) {
      return result;
    }
    this->AddOnTop(o);
    return this->size - 1;
  }
  void AdjustHashes() {
    this->SetExpectedSize(this->size);
  }
  void AddOnTopNoRepetition(const List<Object>& theList) {
    this->SetExpectedSize(this->size + theList.size);
    for (int i = 0; i < theList.size; i ++) {
      this->AddOnTopNoRepetition(theList.TheObjects[i]);
    }
  }
  bool AddOnTopNoRepetition(const Object& o) {
    if (this->GetIndex(o) != - 1) {
      return false;
    }
    this->AddOnTop(o);
    return true;
  }
  bool AddOnTopNoRepetitionMustBeNewCrashIfNot(const Object& o) {
    if (this->GetIndex(o) != - 1) {
      std::stringstream crashStream;
      crashStream << "This is a programming error: the programmer requested to add the object "
      << o << " without repetition "
      << "to the hashed list with a function that does not allow repetition, "
      << "but the hashed list already contains the object. ";
      crash << crashStream.str() << crash;
    }
    this->AddOnTop(o);
    return true;
  }
  Object PopLastObject() {
    Object result = *this->LastObject();
    this->RemoveIndexSwapWithLast(this->size - 1);
    return result;
  }
  Object PopIndexSwapWithLast(int index) {
    Object result = (*this)[index];
    this->RemoveIndexSwapWithLast(index);
    return result;
  }
  void RemoveLastObject() {
    this->RemoveIndexSwapWithLast(this->size - 1);
  }
  void RemoveIndexShiftDown(int index) {
    for (int i = index; i < this->size - 1; i ++) {
      this->SetObjectAtIndex(i, (*this)[i + 1]);
    }
    this->RemoveLastObject();
  }
  void SetObjectAtIndex(int index, const Object& theObject) {
    if (index < 0 || index >= this->size) {
      crash << "This is a programming error. You are attempting to pop out index "
      << index << " out of hashed array "
      << " of size " << this->size << ". " << crash;
    }
    int hashIndexPop = this->GetHash(this->TheObjects[index]);
    this->TheHashedArrays[hashIndexPop].RemoveFirstOccurenceSwapWithLast(index);
    int hashIndexIncoming = this->GetHash(theObject);
    this->TheHashedArrays[hashIndexIncoming].AddOnTop(index);
    this->TheObjects[index] = theObject;
  }
  void RemoveIndexSwapWithLast(int index) {
    if (index < 0 || index >= this->size) {
      crash << "This is a programming error. You are attempting to pop out index "
      << index << " out of hashed array "
      << " of size " << this->size << ". " << crash;
    }
    Object* oPop = &this->TheObjects[index];
    int hashIndexPop = this->GetHash(*oPop);
    this->TheHashedArrays[hashIndexPop].RemoveFirstOccurenceSwapWithLast(index);
    if (index == this->size - 1) {
      this->size --;
      return;
    }
    int tempI = this->size - 1;
    Object* oTop = &this->TheObjects[tempI];
    int hashIndexTop = this->GetHash(*oTop);
    this->TheHashedArrays[hashIndexTop].RemoveFirstOccurenceSwapWithLast(tempI);
    this->TheHashedArrays[hashIndexTop].AddOnTop(index);
    this->TemplateList::RemoveIndexSwapWithLast(index);
  }
  void SwapTwoIndices(int i1, int i2) {
    Object tempO;
    int i1Hash = this->GetHash(this->TheObjects[i1]);
    int i2Hash = this->GetHash(this->TheObjects[i2]);
    this->TheHashedArrays[i1Hash].RemoveFirstOccurenceSwapWithLast(i1);
    this->TheHashedArrays[i2Hash].RemoveFirstOccurenceSwapWithLast(i2);
    tempO = this->TheObjects[i1];
    this->TheObjects[i1] = this->TheObjects[i2];
    this->TheObjects[i2] = tempO;
    this->TheHashedArrays[i1Hash].AddOnTop(i2);
    this->TheHashedArrays[i2Hash].AddOnTop(i1);
  }
  bool Contains(const Object& o) const {
    return this->GetIndex(o) != - 1;
  }
  bool Contains(const List<Object>& theList) const {
    for (int i = 0; i < theList.size; i ++) {
      if (this->GetIndex(theList[i]) == - 1) {
        return false;
      }
    }
    return true;
  }
  int GetIndex(const Object& o) const {
    unsigned int hashIndex = this->GetHash(o);
    for (int i = 0; i < this->TheHashedArrays[hashIndex].size; i ++) {
      int j = this->TheHashedArrays[hashIndex].TheObjects[i];
      if (j >= this->size) {
        crash << "This is a programming error: corrupt hash table: at hashindex = "
        << hashIndex << " I get instructed to look up index " << j
        << " but I have only " << this->size << " elements. " << crash;
      }
      if ((*this)[j] == o) {
        return j;
      }
    }
    return - 1;
  }
  int GetIndexIMustContainTheObject(const Object& o) const {
    int result = this->GetIndex(o);
    if (result == - 1) {
      std::stringstream errorStream;
      errorStream << "This is a programming error: "
      << "the programmer has requested the index of object " << o
      << " with a function that does not allow failure. "
      << "However, the container array does not contain this object. ";
      crash << errorStream.str() << crash;
    }
    return result;
  }
  void SetSize(int inputSize) {
    if (inputSize < 0) {
      inputSize = 0;
    }
    if (inputSize > this->size) {
      crash << "SetSize is allowed for hashed lists only when resizing the hashed list to smaller. "
      << "This is because if I was to resize to larger, I would have to allocate non-initialized "
      << " objects, and those will have to be rehashed which does not make sense. "
      << crash;
    }
    for (int i = this->size - 1; i >= inputSize; i --) {
      this->PopLastObject();
    }
  }

  bool IsSparseRelativeToExpectedSize(int expectedSize) const {
    return expectedSize * 3 < this->TheHashedArrays.size;
  }
  bool IsSparse() const {
    return this->IsSparseRelativeToExpectedSize(this->size);
  }
  void SetExpectedSize(int expectedSize) {
    if (expectedSize < 1) {
      return;
    }
    if (expectedSize == 1 || expectedSize == 2) {
      this->SetHashSizE(1);
      return;
    }
    this->TemplateList::SetExpectedSize(expectedSize);
    if (!this->IsSparseRelativeToExpectedSize(expectedSize)) {
      this->SetHashSizE(expectedSize * 5);
    }
  }
  inline void SetHashSizE(int inputHashSize) {
    return this->SetHashSizE(static_cast<unsigned>(inputHashSize));
  }
  void SetHashSizE(unsigned int HS) {
    if (HS == static_cast<unsigned>(this->TheHashedArrays.size)) {
      return;
    }
    MacroIncrementCounter(ParallelComputing::NumHashResizes);
    List<int> emptyList; //<-empty list has size 0
    this->TheHashedArrays.initializeFillInObject(HS, emptyList);
    if (this->size > 0) {
      for (int i = 0; i < this->size; i ++) {
        int theIndex = this->GetHash((*this)[i]);
        this->TheHashedArrays[theIndex].AddOnTop(i);
      }
    }
  }
  template<typename otherType = int>
  void QuickSortAscending(
    typename List<Object>::OrderLeftGreaterThanRight theOrder = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    List<Object> theList;
    theList = *this;
    theList.QuickSortAscending(theOrder, carbonCopy);
    this->operator=(theList);
  }
  template<typename otherType>
  void QuickSortDescending(
    typename List<Object>::OrderLeftGreaterThanRight theOrder = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    List<Object> theList;
    theList = *this;
    theList.QuickSortDescending(theOrder, carbonCopy);
    this->operator=(theList);
  }
  void initHashesToOne() {
    this->TheHashedArrays.SetSize(1);
    this->TheHashedArrays[0].size = 0;
  }
  HashTemplate(const HashTemplate& other) {
    this->initHashesToOne();
    this->operator=(other);
  }
  HashTemplate() {
    this->initHashesToOne();
  }
  std::string ToString(FormatExpressions* theFormat) const {
    return this->List<Object>::ToString(theFormat);
  }
  std::string ToString() const {
    return this->List<Object>::ToString();
  }
  void operator=(const HashedList<Object, hashFunction>& From) {
    if (&From == this) {
      return;
    }
    this->Clear();
    this->SetHashSizE(From.TheHashedArrays.size);
    this->::List<Object>::operator=(From);
    if (From.IsSparse()) {
      for (int i = 0; i < this->size; i ++) {
        unsigned int hashIndex = this->GetHash(this->TheObjects[i]);
        this->TheHashedArrays[hashIndex].Reserve(From.TheHashedArrays[hashIndex].size);
        this->TheHashedArrays[hashIndex].AddOnTop(i);
      }
    } else {
      this->TheHashedArrays = From.TheHashedArrays;
    }
  }
  const Object& operator[](int i) const {
    return TemplateList::operator[](i);
  }
  Object& GetElement(int theObjectIndex) const {
    return TemplateList::operator[](theObjectIndex);
  }
  void operator=(const TemplateList& other) {
    if (this == &other) {
      return;
    }
    this->Clear();
    this->SetExpectedSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->AddOnTop(other.TheObjects[i]);
    }
  }
};

template <class Object, unsigned int hashFunction(const Object&) = Object::HashFunction>
class HashedList: public HashTemplate<Object, List<Object>, hashFunction> {
public:
  HashedList(const HashedList& other):HashTemplate<Object, List<Object>, hashFunction>() {
    this->operator=(other);
  }
  HashedList() {
  }
  void operator=(const HashedList& other) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::operator=(other);
  }
  void operator=(const List<Object>& other) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::operator=(other);
  }
  //Note The following function specializations are declared entirely in order to
  //facilitate autocomplete in my current IDE. If I find a better autocompletion
  //IDE the following should be removed.
  void AddOnTopNoRepetition(const List<Object>& theList) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTopNoRepetition(theList);
  }
  bool AddOnTopNoRepetition(const Object& o) {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTopNoRepetition(o);
  }
  void AddOnTop(const Object& o) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTop(o);
  }
  void AddOnTop(const List<Object>& theList) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::AddOnTop(theList);
  }
  bool Contains(const Object& o) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::Contains(o);
  }
  bool Contains(const List<Object>& theList) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::Contains(theList);
  }
  Object& GetElement(int theObjectIndex) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::GetElement(theObjectIndex);
  }
  void SetObjectAtIndex(int index, const Object& theObject) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::SetObjectAtIndex(index, theObject);
  }
  void RemoveIndexShiftDown(int index) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::RemoveIndexShiftDown(index);
  }
  void RemoveIndexSwapWithLast(int index) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::RemoveIndexSwapWithLast(index);
  }
  int GetIndex(const Object& o) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::GetIndex(o);
  }
  int GetIndexIMustContainTheObject(const Object& o) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::GetIndexIMustContainTheObject(o);
  }
  int AddNoRepetitionOrReturnIndexFirst(const Object& o) {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::AddNoRepetitionOrReturnIndexFirst(o);
  }
  template <typename otherType = int>
  void QuickSortAscending(
    typename List<Object>::OrderLeftGreaterThanRight theOrder = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::QuickSortAscending(theOrder, carbonCopy);
  }
  template <typename otherType = int>
  void QuickSortDescending(
    typename List<Object>::OrderLeftGreaterThanRight theOrder = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::QuickSortDescending(theOrder, carbonCopy);
  }
  void SetExpectedSize(int expectedSize) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::SetExpectedSize(expectedSize);
  }
};

//class used to avoid a gcc compiler bug.
//This class should probably be removed as soon as the bug is resolved.
template <class Object>
class HashedListSpecialized: public HashedList<Object, Object::HashFunction> {
};

struct stackInfo {
public:
  std::string fileName;
  int line;
  std::string functionName;
  void operator=(const stackInfo& other) {
    this->fileName = other.fileName;
    this->line = other.line;
    this->functionName = other.functionName;
  }
};

class FileInformation {
  friend std::ostream& operator<<(std::ostream& output, const FileInformation& theFI) {
    output << theFI.ToString();
    return output;
  }
  public:
  std::string FileName;
  std::string FileDescription;
  std::string ToString() const {
    std::stringstream out;
    out << "file: " << this->FileName << ", description: " << this->FileDescription;
    return out.str();
  }
  bool operator>(const FileInformation& other) {
    return this->FileName>other.FileName;
  }
  bool operator==(const FileInformation& other) const {
    return this->FileName == other.FileName;
  }
  static unsigned int HashFunction(const FileInformation& input) {
    return MathRoutines::HashString(input.FileName);
  }
  static void AddProjectInfo(const std::string& fileName, const std::string& fileDescription);
};

class ProgressReport {
public:
  int currentLevel;
  int threadIndex;
  int ticks;
  int ticksPerReport;
  int reportType;
  bool flagInitialized;
  // Call TickAndWantReport before generating report.
  // If a report is not wanted, it is wise to not generate one:
  // the string operations required to generate a progress report
  // would are expected to cost more than the mathematical computations
  // they are reporting.
  void Report(const std::string& theReport);
  bool TickAndWantReport();
  void init();
  ProgressReport() {
    this->init();
  }
  ProgressReport(int inputTicksPerReport, int inputReportType) {
    this->init();
    this->ticksPerReport = inputTicksPerReport;
    this->reportType = inputReportType;
  }
  ProgressReport(const std::string& theReport) {
    this->init();
    this->Report(theReport);
  }
  ~ProgressReport();
};

template<class Base>
std::iostream& operator<<(std::iostream& output, const CompleX<Base>& input);

template <typename Element>
std::iostream& operator<<(std::iostream& output, const Matrix<Element>& theMat) {
  output << "<table>";
  for (int i = 0; i < theMat.NumRows; i ++) {
    output << "<tr>";
    for (int j = 0; j < theMat.NumCols; j ++) {
      output << "<td>";
      output << theMat.elements[i][j];
      output << "</td> ";
    }
    output << "</tr>\n";
  }
  output << "</table>";
  return output;
}

template <class coefficient>
std::ostream& operator<<(std::ostream& out, const Vector<coefficient>& theVector) {
  out  << "(";
  for (int i = 0; i < theVector.size; i ++) {
    out << theVector[i];
    if (i != theVector.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  return out;
}

template <class Object>
template <class compareClass, class carbonCopyType>
bool List<Object>::QuickSortAscendingCustomRecursive(
  int BottomIndex, int TopIndex, compareClass& theCompareror, List<carbonCopyType>* carbonCopy
) {
  if (TopIndex <= BottomIndex) {
    return true;
  }
  int HighIndex = TopIndex;
  for (int LowIndex = BottomIndex + 1; LowIndex <= HighIndex; LowIndex ++) {
    if (theCompareror.CompareLeftGreaterThanRight(
      this->TheObjects[LowIndex], this->TheObjects[BottomIndex]
    )) {
      if (carbonCopy != nullptr) {
        carbonCopy->SwapTwoIndices(LowIndex, HighIndex);
      }
      this->SwapTwoIndices(LowIndex, HighIndex);
      LowIndex --;
      HighIndex --;
    }
  }
  if (theCompareror.CompareLeftGreaterThanRight(this->TheObjects[HighIndex], this->TheObjects[BottomIndex])) {
    HighIndex --;
  }
  if (carbonCopy != nullptr) {
    carbonCopy->SwapTwoIndices(BottomIndex, HighIndex);
  }
  this->SwapTwoIndices(BottomIndex, HighIndex);
  if (!this->QuickSortAscendingCustomRecursive(BottomIndex, HighIndex - 1, theCompareror, carbonCopy)) {
    return false;
  }
  if (!this->QuickSortAscendingCustomRecursive(HighIndex + 1, TopIndex, theCompareror, carbonCopy)) {
    return false;
  }
  return true;
}

template <class Object>
void List<Object>::AddListOnTop(const List<Object>& theList) {
  int oldsize = this->size;
  int otherSize = theList.size;
  this->SetSize(oldsize + otherSize);
  for (int i = 0; i < otherSize; i ++) {
    this->TheObjects[i + oldsize] = theList.TheObjects[i];
  }
}

template<class Object>
void List<Object>::SwapTwoIndices(int index1, int index2) {
  if (index1 < 0 || index1 >= this->size || index2 < 0 || index2 >= this->size) {
    crash << "This is a programming error: requested elements with indices "
    << index1 << " and " << index2 << " in a list that has "
    << this->size << " elements. This is impossible. " << crash;
  }
  if (index1 == index2) {
    return;
  }
  Object tempO;
  tempO = this->TheObjects[index1];
  this->TheObjects[index1] = this->TheObjects[index2];
  this->TheObjects[index2] = tempO;
}

template<class Object>
void List<Object>::CycleIndices(const List<int>& cycle) {
  if (cycle.size < 2) {
    return;
  }
  for (int i = 0; i < cycle.size; i ++) {
    if ((cycle[i] >= this->size) || (cycle[i] < 0)) {
      crash << "Programming error: request to cycle indices " << cycle
      << " in list of " << this->size << " elements." << crash;
    }
  }
  // ironically, it's easier to program the cycles to go backwards XD
  Object tail;
  tail = this->TheObjects[cycle[cycle.size - 1]];
  for (int i = cycle.size - 1; i != 0; i --) {
    this->TheObjects[cycle[i]] = this->TheObjects[cycle[i - 1]];
  }
  this->TheObjects[cycle[0]] = tail;
}

template<class Object>
void List<Object>::PermuteIndices(const List<List<int> >& cycles) {
  for (int i = 0; i < cycles.size; i ++) {
    this->CycleIndices(cycles[i]);
  }
}

template<class Object>
int List<Object>::GetIndex(const Object& o) const {
  for (int i = 0; i < this->size; i ++) {
    if (this->TheObjects[i] == o) {
      return i;
    }
  }
  return - 1;
}

template <class Object>
void List<Object>::swap(List<Object>& l1, List<Object>& l2) {
  List<Object>* bigL = nullptr;
  List<Object>* smallL = nullptr;
  int smallSize;
  if (l1.size < l2.size) {
    smallL = &l1;
    bigL = &l2;
    smallSize = l1.size;
  } else {
    bigL = &l1;
    smallL = &l2;
    smallSize = l2.size;
  }
  smallL->SetSize(bigL->size);
  Object tempO;
  for (int i = 0; i < smallSize; i ++) {
    tempO = smallL->TheObjects[i];
    smallL->TheObjects[i] = bigL->TheObjects[i];
    bigL->TheObjects[i] = tempO;
  }
  for (int i = smallSize; i < bigL->size; i ++) {
    smallL->TheObjects[i] = bigL->TheObjects[i];
  }
  bigL->size = smallSize;
}

template <class Object>
int List<Object>::SizeWithoutObjects() const {
  return sizeof(this->ActualSize) + sizeof(this->size) + sizeof(this->TheObjects);
}

template <class Object>
bool List<Object>::operator>(const List<Object>& other) const {
  if (this->size > other.size) {
    return true;
  }
  if (other.size > this->size) {
    return false;
  }
  for (int i = 0; i < this->size; i ++) {
    if (this->TheObjects[i] > other.TheObjects[i]) {
      return true;
    }
    if (other.TheObjects[i] > this->TheObjects[i]) {
      return false;
    }
  }
  return false;
}

template <class Object>
bool List<Object>::operator<(const List<Object>& other) const {
  if (this->size > other.size) {
    return false;
  }
  if (other.size > this->size) {
    return true;
  }
  for (int i = 0; i < this->size; i ++) {
    if (this->TheObjects[i] > other.TheObjects[i]) {
      return false;
    }
    if (other.TheObjects[i] > this->TheObjects[i]) {
      return true;
    }
  }
  return false;
}

template <class Object>
void List<Object>::ShiftUpExpandOnTop(int StartingIndex) {
  this->ShiftUpExpandOnTopRepeated(StartingIndex, 1);
}

template <class Object>
void List<Object>::ShiftUpExpandOnTopRepeated(int StartingIndex, int numberOfNewElements) {
  this->SetSize(this->size + numberOfNewElements);
  int lowBound = StartingIndex + numberOfNewElements;
  for (int i = this->size - 1; i >= lowBound; i --) {
    this->TheObjects[i] = this->TheObjects[i - numberOfNewElements];
  }
}

template <class Object>
void List<Object>::SliceInPlace(int StartingIndex, int SizeOfSlice) {
  this->Slice(StartingIndex, SizeOfSlice, *this);
}

template <class Object>
List<Object> List<Object>::SliceCopy(int StartingIndex, int SizeOfSlice) const {
  List<Object> result;
  this->Slice(StartingIndex, SizeOfSlice, result);
  return result;
}

template <class Object>
void List<Object>::Slice(int StartingIndex, int SizeOfSlice, List<Object>& output) const {
  // output allowed to equal this
  if (StartingIndex < 0) {
    StartingIndex = 0;
  }
  if (SizeOfSlice < 0) {
    SizeOfSlice = 0;
  }
  if (SizeOfSlice + StartingIndex > this->size) {
    SizeOfSlice = this->size - StartingIndex;
  }
  if (output.size < SizeOfSlice) {
    // Implies output is not this.
    // In case output is this we only need to resize at the end.
    output.SetSize(SizeOfSlice);
  }
  for (int i = 0; i < SizeOfSlice; i ++) {
    output[i] = this->TheObjects[i + StartingIndex];
  }
  output.SetSize(SizeOfSlice);
}

template <class Object>
void List<Object>::initializeFillInObject(int theSize, const Object& o) {
  this->SetSize(theSize);
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i] = o;
  }
}

template <class Object>
bool List<Object>::AddOnTopNoRepetition(const Object& o) {
  if (this->GetIndex(o) != - 1) {
    return false;
  }
  this->AddOnTop(o);
  return true;
}

template <class Object>
Object* List<Object>::LastObject() const {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->size <= 0) {
    crash << "This is a programming error: trying to fetch the last object of an array with "
    << this->size << " elements. " << crash;
  }
  return &this->TheObjects[this->size - 1];
}

template <class Object>
bool List<Object>::HasACommonElementWith(List<Object>& right) {
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < right.size; j ++) {
      if (this->TheObjects[i] == right.TheObjects[j]) {
        return true;
      }
    }
  }
  return false;
}

template <class Object>
void List<Object>::AssignLight(const ListLight<Object>& From) {
  this->SetSize(From.size);
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i] = From.TheObjects[i];
  }
}

template <class Object>
void List<Object>::Reserve(int theSize) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->ActualSize < theSize) {
    this->ExpandArrayOnTop(theSize - this->ActualSize);
  }
}

template <class Object>
void List<Object>::RemoveFirstOccurenceSwapWithLast(const Object& o) {
  for (int i = 0; i < this->size; i ++) {
    if (o == this->TheObjects[i]) {
      this->RemoveIndexSwapWithLast(i);
      return;
    }
  }
}

template <class Object>
void List<Object>::SetSize(int theSize) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (theSize < 0) {
    theSize = 0;
  }
  this->SetExpectedSize(theSize);
  this->Reserve(theSize);
  this->size = theSize;
}

template <class Object>
std::string List<Object>::ToString() const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->TheObjects[i].ToString() << "\n";
  }
  return out.str();
}

template <class Object>
std::string List<Object>::ToStringCommaDelimited() const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->TheObjects[i];
    if (i != this->size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

template <class Object>
std::string List<Object>::ToStringConcatenate() const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->TheObjects[i];
  }
  return out.str();
}

template <class Object>
std::string List<Object>::ToString(FormatExpressions* theFormat) const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->TheObjects[i].ToString(theFormat) << "\n";
  }
  return out.str();
}

template <class Object>
std::string List<Object>::ToStringCommaDelimited(FormatExpressions* theFormat) const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->TheObjects[i].ToString(theFormat);
    if (i != this->size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

template <class Object>
void List<Object>::AddObjectOnTopCreateNew() {
  this->SetSize(this->size + 1);
}

template <class Object>
void List<Object>::RemoveIndexSwapWithLast(int index) {
  if (this->size == 0) {
    return;
  }
  this->size --;
  this->TheObjects[index] = this->TheObjects[this->size];
}

template <class Object>
void List<Object>::RemoveLastObject() {
  if (this->size == 0) {
    crash << "Programming error: attempting to pop empty list" << crash;
  }
  this->size --;
}

template <class Object>
Object List<Object>::PopLastObject() {
  if (this->size == 0) {
    crash << "Programming error: attempting to pop empty list" << crash;
  }
  this->size --;
  return this->TheObjects[size];
}

template <class Object>
void List<Object>::ReleaseMemory() {
  delete [] this->TheObjects;
  this->size = 0;
  this->TheObjects = 0;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter -= this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->ActualSize = 0;
}

template <class Object>
List<Object>::~List() {
  delete [] this->TheObjects;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter -= this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->size = 0;
  this->ActualSize = 0;
  this->flagDeallocated = true;
}

template <class Object>
void List<Object>::ExpandArrayOnTop(int increase) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (increase <= 0) {
    return;
  }
  MacroIncrementCounter(ParallelComputing::NumListResizesTotal);
  Object* newArray = 0;
  try {
    newArray = new Object[this->ActualSize + increase];
  } catch (std::bad_alloc& theBA) {
    (void) theBA;
    crash << "Memory allocation failure: failed to allocate " << this->ActualSize + increase << " objects. " << crash;
  }
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter += this->ActualSize + increase;
  ParallelComputing::CheckPointerCounters();
#endif
  for (int i = 0; i < this->size; i ++) {
    newArray[i] = this->TheObjects[i];
  }
  Object* oldPointer = this->TheObjects; //<-store the old memory block
  this->TheObjects = newArray;//<-replace TheObjects with newArray
  delete [] oldPointer;//<-only then we delete the non-used memory.
  //The above looks complicated but is safe:
  //if another thread jumps and requests operator[]
  //on our list exactly between the two instructions above
  //we are safe with this implementation.
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter -= this->ActualSize;
  ParallelComputing::CheckPointerCounters();
#endif
  this->ActualSize += increase;
}

template <class Object>
void List<Object>::ReverseOrderElements() {
  int tempI = this->size / 2;
  for (int i = 0; i < tempI; i ++) {
    this->SwapTwoIndices(i, this->size - i - 1);
  }
}

template <class Object>
void List<Object>::ReverseRange(int rangeBegin, int rangeEnd) {
  for (int i = rangeBegin, j = rangeEnd - 1; i < j; i ++, j --) {
    Object oi = this->TheObjects[i];
    this->TheObjects[i] = this->TheObjects[j];
    this->TheObjects[j] = oi;
  }
}

template <class Object>
void List<Object>::AddOnTop(const Object& o) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->size > this->ActualSize) {
    crash << "This is a programming error: the actual size of the list is "
    << this->ActualSize << " but this->size equals " << this->size
    << ". " << crash;
  }
  if (this->size == this->ActualSize) {
    this->ExpandArrayOnTop(this->GetNewSizeRelativeToExpectedSize(this->ActualSize + 1) - this->size);
  }
  this->TheObjects[size] = o;
  this->size ++;
}
#endif
