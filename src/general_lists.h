// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_general_lists_ALREADY_INCLUDED
#define header_general_lists_ALREADY_INCLUDED

#include "macros.h"
#include "general_multitasking.h"
#include <algorithm>
#include <string.h>
#include <math.h>

// IMPORTANT.
// Convention on Hash functions.
// 1. C++ objects that represent mathematically equal objects
//    are allowed to have different bit representations in RAM memory.
// 2. Mathematically equal objects must
//    have their object::hashFunction return identical
//    values, even if the objects have bitwise different
//    representations in RAM.
// 3. Mathematical objects representing 0 in an abelian group
//    must have their hash function return 0.
//
// In particular, zero rational numbers,
// zero polynomials/monomial collections/elements
// of semisimple Lie algebras, etc. must have their hash functions return 0.
// Motivation: for speed purposes, it is not desirable to fix unique
// bitwise representations in RAM memory for mathematically equal objects.
// For example, this means that monomials in a polynomial do not
// need to be ordered in a specific order.
// Yet polynomials can be quickly compared by
// computing their hash functions. Here, as an example
// we may give two 1-million monomial polynomials
// (computed, say, by adding two 0.5 million monomial polynomials
// in two different orders). The
// polynomials' monomials are ordered differently,
// yet the polynomials can quickly be compared.
// Note that sorting N monomials of an arbitrary polynomial
// is at best O(N log(N)) operations, while
// computing the hash functions is only O(N) operations.

// Used for hashing.
class HashConstants {
private:
  // Selected for no particular reason:
  // 25 is a frequently used "large" number.
  static const int size = 25;
  // Selected by tmilev.
  // Procedure: I inspected the list of primes up to 15000.
  // Then I copied and pasted a few numbers at random,
  // trying not to pick consecutive primes.
  // I also went for 3+ decimal digits.
  static const unsigned int primeConstants[size];
public:
  static inline unsigned int getConstantIncrementCounter(int& counter) {
    if (counter > size) {
      counter = 0;
    }
    return primeConstants[counter];
  }
  static const unsigned constant0 = 607;
  static const unsigned constant1 = 1013;
  static const unsigned constant2 = 2207;
};

// The following class is for buffers, i/o function pointers, multitasking, general purpose.
class GlobalVariables;

// Simple math routines (min, max, etc.) that I have not been able to find a better placement for
class MathRoutines;

// Rationals and integers:
class LargeIntegerUnsigned;
class LargeInteger;
class Rational;

// More involved mathematical types
template<class Base>
class Complex;
class AlgebraicNumber;
template <class Coefficient>
class RationalFraction;
class SemisimpleLieAlgebra;
class RootSubalgebra;
class RootSubalgebras;
class ChevalleyGenerator;
class DynkinDiagramRootSubalgebra;
template<class Coefficient>
class ElementSemisimpleLieAlgebra;
template<class Coefficient>
class ElementUniversalEnveloping;
template <class Coefficient, unsigned int inputHashFunction(const Coefficient&)>
class MonomialTensor;
template<class Coefficient>
class MonomialUniversalEnveloping;
template<class Coefficient>
class ElementSumGeneralizedVermas;
template <class Coefficient>
class CharacterSemisimpleLieAlgebraModule;
class SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms;
template<class Coefficient>
class ModuleSSalgebra;
class SlTwoSubalgebras;
template<class Coefficient>
class MonomialTensorGeneralizedVermas;
template<class Coefficient>
class ElementTensorsGeneralizedVermas;
struct BranchingData;
class QuasiDifferentialMononomial;
template<class Coefficient>
class QuasiDifferentialOperator;

//classes related to linear integral programming (polyhedra, lattices, quasipolynomials)
class Cone;
class ConeCollection;
class Lattice;
template <class Coefficient>
class AffineHyperplane;
class AffineCones;

// Hybrid classes that serve both memory-management and mathematical purposes
// (Matrices, Vectors, PolynomialSubstitution, etc.)
template <class ObjectType1, class ObjectType2, unsigned int hashFunction1(const ObjectType1&) = ObjectType1::hashFunction, unsigned int hashFunction2(const ObjectType2&)=ObjectType2::hashFunction>
class Pair;
template <class Object>
class List;
template <class Object>
class ListZeroAfterUse;
template <class Object>
class Matrix;
template <class Object, unsigned int hashFunction(const Object&)>
class HashedList;
template <class Coefficient>
class Vector;
template <class Coefficient>
class Vectors;
class MonomialPolynomial;
template <class TemplateMonomial, class Coefficient>
class LinearCombination;
template <class Coefficient>
class Polynomial;
template <class Coefficient>
class PolynomialSubstitution;

//algebraic geometry computation class:
template <class Coefficient>
class GroebnerBasisComputation;

//combinatorial classes
class Selection;
class SubsetWithMultiplicities;

//the following classes have to do with user interface/displaying/drawing information
class FormatExpressions;
struct IndicatorWindowVariables;
class DrawingVariables;
class DrawOperations;
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

class MathRoutines {
public:
  template <class Coefficient>
  static bool invertXModN(const Coefficient& x, const Coefficient& n, Coefficient& output) {
    Coefficient quotient;
    Coefficient remainder;
    Coefficient dividend;
    Coefficient divisor;
    Coefficient dividendN;
    Coefficient dividendX;
    Coefficient divisorN;
    Coefficient divisorX;
    Coefficient swapX;
    Coefficient swapN;
    if (x < 0 || n < 0) {
      fatalCrash("Negative inputs to invertXModN");
    }
    // At any given moment, dividend = dividendN * N + dividendX * X.
    dividend = n;
    dividendN = 1;
    dividendX = 0;
    // At any given moment, divisor = divisorN * N + divisorX * X.
    divisor = x;
    divisorN = 0;
    divisorX = 1;
    while (divisor > 0) {
      quotient = dividend / divisor;
      remainder = dividend % divisor;
      dividend = divisor;
      divisor = remainder;

      swapX = dividendX;
      swapN = dividendN;

      dividendX = divisorX;
      dividendN = divisorN;

      divisorX = swapX - quotient * divisorX;
      divisorN = swapN - quotient * divisorN;
    }
    if (dividend != 1) {
      return false; //d and p were not relatively prime.
    }
    output = dividendX % n;
    if (output < 0) {
      output += n;
    }
    return true;
  }
  static int leastCommonMultiple(int a, int b);
  template <typename Integral>
  static Integral greatestCommonDivisor(Integral left, Integral right) {
    Integral temp;
    while (!(right == 0)) {
      temp = left % right;
      left = right;
      right = temp;
    }
    return left;
  }
  template <typename Integral>
  static Integral leastCommonMultiple(Integral a, Integral b) {
    Integral result;
    result = a;
    result /= MathRoutines::greatestCommonDivisor(a, b);
    result *= b;
    return result;
  }
  static int twoToTheNth(int n);
  static bool isLatinLetter(char input);
  static bool isDigit(const std::string& input, int* whichDigit = nullptr);
  static bool isDigit(char input, int* whichDigit = nullptr);
  static bool hasDecimalDigitsOnly(const std::string& input);
  static bool isHexDigit(char digitCandidate);
  template <class Type>
  static bool generateVectorSpaceClosedWithRespectToLieBracket(List<Type>& inputOutputElements, int upperDimensionBound) {
    return MathRoutines::generateVectorSpaceClosedWithRespectToOperation(
      inputOutputElements, upperDimensionBound, Type::lieBracket
    );
  }
  static Vector<double> getVectorDouble(Vector<Rational>& input);
  template <class Type>
  static bool generateVectorSpaceClosedWithRespectToOperation(
    List<Type>& inputOutputElts,
    int upperDimensionBound,
    void (*binaryOperation)(const Type& left, const Type& right, Type& output)
  );

  static char convertHumanReadableHexToCharValue(char input);
  static void nChooseK(int n, int k, LargeInteger& result);
  static LargeInteger nChooseK(int n, int k);
  static int factorial(int n);
  static inline double E() {
    return 2.718281828459;
  }
  static inline double pi() {
    return 3.141592653589793238462643383279;
  }
  // The MS compiler refuses to compile the following, hence the above line.
  // static const double Pi = (double)3.141592653589793238462643383279;
  static int kToTheNth(int k, int n);
  static void kToTheNth(int k, int n, LargeInteger& output);
  inline static int parity(int n);
  static int binomialCoefficientMultivariate(int n, List<int>& choices);
  static bool isPrimeSimple(int input);
  template <class Coefficient, typename IntegerType>
  static void raiseToPower(
    Coefficient& element,
    const IntegerType& power,
    const Coefficient& ringUnit
  );
  template <typename Coefficient>
  inline static Coefficient maximum(const Coefficient& a, const Coefficient& b) {
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
  inline static Element minimum(const Element& a, const Element& b) {
    if (a > b) {
      return b;
    } else {
      return a;
    }
  }
  template<class Element>
  static std::string toStringBrackets(const Element& input) {
    if (!input.needsParenthesisForMultiplication()) {
      return input.toString();
    }
    std::string result;
    result.append("(");
    result.append(input.toString());
    result.append(")");
    return result;
  }
  template<class Element>
  static std::string toStringBrackets(const Element& input, FormatExpressions* format) {
    if (!input.needsParenthesisForMultiplication()) {
      return input.toString(format);
    }
    std::string result;
    result.append("(");
    result.append(input.toString(format));
    result.append(")");
    return result;
  }
  static double reducePrecision(double x);
  static unsigned int hashDouble(const double& input);
  static unsigned int hashVectorDoubles(const Vector<double>& input);
  static unsigned int hashListDoubles(const List<double>& input);
  static unsigned int hashListInts(const List<int>& input);
  static unsigned int hashListUnsignedChars(const List<unsigned char>& input);
  static unsigned int hashListStrings(const List<std::string>& input);
  static unsigned int hashString(const std::string& x);
  template <class Element>
  static void lieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output);
  template <typename number>
  static number complexConjugate(number x) {
    return x.getComplexConjugate();
  }
  static int complexConjugate(int x) {
    return x;
  }
  static double complexConjugate(double x) {
    return x;
  }
  static bool isInteger(Rational x);
  static bool parseListIntegers(const std::string& input, List<int>& result, std::stringstream* commentsOnFailure);
  static void parseListIntegersNoFailure(const std::string& input, List<int>& result);
};

class DrawElementInputOutput {
public:
  int topLeftCornerX;
  int topLeftCornerY;
  int outputWidth;
  int outputHeight;
};

template <typename basicType>
class StateMaintainer {
public:
  basicType* toMaintain;
  basicType contentAtStart;
  StateMaintainer() {
    this->toMaintain = nullptr;
  }
  StateMaintainer(basicType& inputToMaintain) {
    this->initialize(inputToMaintain);
  }
  void initialize(basicType& inputToMaintain) {
    this->toMaintain = &inputToMaintain;
    this->contentAtStart = inputToMaintain;
  }
  ~StateMaintainer() {
    if (this->toMaintain == nullptr) {
      return;
    }
    *(this->toMaintain) = this->contentAtStart;
    this->toMaintain = nullptr;
  }
};

// Maintains a recursion depth counter through constructor/destructor calls.
// Use like this:
// RecursionDepthCounter recursionDepthCounter(&this->recursionDepth);
// <- this is more elegant but some IDEs and compilers
// complain that recursionDepthCounter is not used, which is non-sense
// as the object is clearly used through its destructor.
//
// or like this:
//
// RecursionDepthCounter recursionDepthCounter;
// recursionDepthCounter.initialize(&this->recursionDepth);
// <- less elegant but you may get fewer IDE complaints.
class RecursionDepthCounter {
public:
  int* counter;
  RecursionDepthCounter(): counter(nullptr) {
  }
  RecursionDepthCounter(int* inputCounter): counter(nullptr) {
    if (inputCounter == nullptr) {
      return;
    }
    this->counter = inputCounter;
    (*this->counter) ++;
  }
  void initialize(int* inputCounter){
    this->counter = inputCounter;
    (*this->counter)++;
  }
  ~RecursionDepthCounter() {
    if (this->counter != nullptr) {
      (*this->counter) --;
    }
    this->counter = nullptr;
  }
};

template <class Object>
std::ostream& operator<<(std::ostream& output, const List<Object>& input) {
  output << input.size << " elements: \n";
  for (int i = 0; i < input.size; i ++) {
    output << input[i];
    if (i != input.size - 1) {
      output << ", ";
    }
  }
  return output;
}

template <class Object>
std::iostream& operator>>(std::iostream& input, List<Object>& outputList) {
  std::string tempS;
  int reader = 0;
  input >> tempS >> reader;
  if (tempS != "size:") {
    fatalCrash("Failed reading list from stream. ");
  }
  outputList.setSize(reader);
  for (int i = 0; i < outputList.size; i ++) {
    input >> outputList[i];
  }
  return input;
}

class HashFunctions {
public:
  template<class Object>
  static unsigned int hashFunction(const Object& object) {
    return object.hashFunction();
  }
  static unsigned int hashFunction(const bool& input) {
    return static_cast<unsigned int>(input);
  }
  static unsigned int hashFunction(const double& input);
  static unsigned int hashFunction(const std::string& input) {
    size_t numCycles = input.size();
    unsigned int result = 0;
    int hashCounter = 0;
    for (unsigned i = 0; i < numCycles; i ++) {
      result += static_cast<unsigned>(input[i]) * HashConstants::getConstantIncrementCounter(hashCounter);
    }
    return result;
  }
  static unsigned int hashFunction(const int& input) {
    return static_cast<unsigned>(input);
  }
  static unsigned int hashFunction(const char& input) {
    return static_cast<unsigned int>(input);
  }
  static unsigned int hashFunction(const unsigned char& input) {
    return static_cast<unsigned int>(input);
  }
};

// List serves the same purpose as std::vector
// List is not thread safe!!!!
// Lists are used in the implementation of mutexes!!!
template <class Object>
class List {
  friend std::ostream& operator<< <Object>(std::ostream& output, const List<Object>& input);
  friend std::iostream& operator>> <Object>(std::iostream& input, List<Object>& inputList);
public:
  class Comparator {
  public:
    bool (*leftGreaterThanRight) (const Object& left, const Object& right);
    std::string name;
    Comparator() {
      this->leftGreaterThanRight = nullptr;
    }
    Comparator(bool (*inputLeftGreaterThanRight)(const Object& left, const Object& right)) {
      this->setComparison(inputLeftGreaterThanRight);
    }
    bool checkConsistency() const {
      if (this->leftGreaterThanRight == nullptr) {
        return true;
      }
      Object nonInitialized;
      if (this->leftGreaterThanRight(nonInitialized, nonInitialized)) {
        fatalCrash("Order function is not strict. ");
      }
      return true;
    }
    void setComparison(bool (*inputLeftGreaterThanRight)(const Object& left, const Object& right)) {
      this->leftGreaterThanRight = inputLeftGreaterThanRight;
    }
    bool greaterThan(const Object& left, const Object& right) const {
      if (this->leftGreaterThanRight == nullptr) {
        fatalCrash("Non-initialized comparison function.");
      }
      return this->leftGreaterThanRight(left, right);
    }
    bool greaterThanOrEqualTo(const Object& left, const Object& right) const {
      if (this->greaterThan(left, right)) {
        return true;
      }
      return left == right;
    }
  };
private:
  friend class ListZeroAfterUse<Object>;
  void expandArrayOnTop(int increase);
  template <class compareClass, class carbonCopyType>
  bool quickSortAscendingCustomRecursive(
    int bottomIndex,
    int topIndex,
    compareClass& comparator,
    List<carbonCopyType>* carbonCopy
  );
  void quickSortDescending(int bottomIndex, int topIndex);
  inline void initConstructorCallOnly() {
    this->objects = nullptr;
    this->actualSize = 0;
    this->size = 0;
    this->flagDeallocated = false;
    MacroIncrementCounter(GlobalStatistics::numberOfListsCreated);
  }
  int actualSize;
public:
  bool flagDeallocated;
  Object* objects;
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
  List<Object> sliceCopy(int startingIndex, int sizeOfSlice) const;
  void sliceInPlace(int startingIndex, int sizeOfSlice);
  void slice(int startingIndex, int sizeOfSlice, List<Object>& output) const;
  ~List();
  bool checkConsistency() const {
    if (this->flagDeallocated) {
      fatalCrash("Use after free of List. ");
    }
    return true;
  }
  int getNewSizeRelativeToExpectedSize(int expectedSize) const {
    // <-Registering stack trace forbidden! Multithreading deadlock alert.
    if (expectedSize == 1) {
      return 1;
    }
    if (expectedSize == 2) {
      return 2;
    }
    return (expectedSize * 4) / 3 + 1;
  }
  void setExpectedSize(int desiredSize) {
    // <-Registering stack trace forbidden! Multithreading deadlock alert.
    if ((this->actualSize * 5 / 6) < desiredSize) {
      this->reserve(this->getNewSizeRelativeToExpectedSize(desiredSize));
    }
  }
  void expandOnTop(int increase) {
    int newSize = this->size + increase;
    if (newSize < 0) {
      newSize = 0;
    }
    this->setSize(newSize);
  }
  void setSize(int incomingSize); // <-Registering stack trace forbidden! Multithreading deadlock alert.
  void clear() {
    // <-Registering stack trace forbidden! Multithreading deadlock alert.
    this->setSize(0);
  }
  void setObjectAtIndex(int index, const Object& o) {
    (*this)[index] = o;
  }
  void setSizeMakeMatrix(int numberOfRows, int numberOfColumns) {
    this->setSize(numberOfRows);
    for (int i = 0; i < numberOfRows; i ++) {
      this->objects[i].setSize(numberOfColumns);
    }
  }
  void initializeFillInObject(int incomingSize, const Object& o);
  void reserve(int desiredSize);// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void insertAtIndexShiftElementsUp(const Object& o, int desiredIndex) {
    this->shiftUpExpandOnTop(desiredIndex);
    (*this)[desiredIndex] = o;
  }
  void addOnTop(const Object& o);
  void addListOnTop(const List<Object>& incoming);
  bool addOnTopNoRepetition(const Object& o);
  void addOnTopNoRepetition(const List<Object>& other) {
    this->setExpectedSize(this->size + other.size);
    for (int i = 0; i < other.size; i ++) {
      this->addOnTopNoRepetition(other[i]);
    }
  }
  int addNoRepetitionOrReturnIndexFirst(const Object& o) {
    int indexOfObject = this->getIndex(o);
    if (indexOfObject == - 1) {
      this->addOnTop(o);
      return this->size - 1;
    }
    return indexOfObject;
  }
  void removeIndexShiftDown(int index) {
    for (int i = index; i < this->size - 1; i ++) {
      this->objects[i] = this->objects[i + 1];
    }
    this->size --;
  }
  void removeIndicesShiftDown(int firstToRemove, int numberToRemove) {
    for (int i = firstToRemove; i < this->size - numberToRemove; i ++) {
      this->objects[i] = this->objects[i + numberToRemove];
    }
    this->size -= numberToRemove;
  }
  void removeIndexSwapWithLast(int index);
  void removeLastObject();
  void removeObjectsShiftDown(const List<Object>& toBeRemoved) {
    int currentIndex = 0;
    for (int i = 0; i < this->size; i ++) {
      if (!toBeRemoved.contains((*this)[i])) {
        toBeRemoved.swapTwoIndices(i, currentIndex);
        currentIndex ++;
      }
    }
    this->setSize(currentIndex);
  }
  // The below function is named a bit awkwardly because otherwise there is a risk of confusion
  // with the removeIndexSwapWithLast when selecting from autocomplete list.
  // This cost me already 2 hours of lost time,
  // so the awkward name is necessary.
  void removeFirstOccurenceSwapWithLast(const Object& o);
  Object popLastObject();
  Object popIndexSwapWithLast(int index) {
    Object result;
    result = (*this)[index];
    this->removeIndexSwapWithLast(index);
    return result;
  }
  Object popIndexShiftDown(int index) {
    Object result;
    result = (*this)[index];
    this->removeIndexShiftDown(index);
    return result;
  }
  // The following function returns false if the comparison operator failed!!!!
  template <class compareClass, class carbonCopyType = Object>
  bool quickSortAscendingCustom(compareClass& comparator, List<carbonCopyType>* carbonCopy = nullptr) {
    return this->quickSortAscendingCustomRecursive(0, this->size - 1, comparator, carbonCopy);
  }
  template <class compareClass, class carbonCopyType = Object>
  bool quickSortDescendingCustom(compareClass& comparator, List<carbonCopyType>* carbonCopy = nullptr) {
    bool result = this->quickSortAscendingCustomRecursive(0, this->size - 1, comparator, carbonCopy);
    this->reverseElements();
    if (carbonCopy != 0) {
      carbonCopy->reverseElements();
    }
    return result;
  }
  // The function below is required to reserve the order of elements given by selection.elements.
  void subSelection(const Selection& selection, List<Object>& output);
  // If comparison function is not specified, quickSortAscending uses operator >, else it uses the given
  // comparison function
  template <class otherType = Object>
  void quickSortAscending(
    const List<Object>::Comparator* order = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    List<Object>::quickSortAscending<List<Object>, List<otherType> >(*this, order, carbonCopy);
  }

  template <class otherType = Object>
  void quickSortDescending(
    const List<Object>::Comparator* order = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    List<Object>::quickSortDescending<List<Object>, List<otherType> >(*this, order, carbonCopy);
  }

  template <class templateList, class otherList>
  static void quickSortAscending(
    templateList& other,
    const typename List<Object>::Comparator* order = nullptr,
    otherList* carbonCopy = nullptr
  ) {
    if (carbonCopy != 0) {
      if (carbonCopy->size != other.size) {
        std::stringstream crashComments;
        crashComments << "Quicksort with carbon copy requested on a list with "
        << other.size << " elements, but the carbon copy has "
        << carbonCopy->size << " elements. ";
        fatalCrash(crashComments.str());
      }
    }
    if (other.size == 0) {
      return;
    }
    if (order != nullptr) {
      if (order->leftGreaterThanRight == nullptr) {
        order = nullptr;
      }
    }
    if (order == nullptr) {
      List<Object>::quickSortAscendingNoOrder(other, 0, other.size - 1, carbonCopy);
    } else {
      List<Object>::quickSortAscendingOrder<templateList, otherList>(
        other, 0, other.size - 1, *order, carbonCopy
      );
    }
  }

  template <class templateList, class otherList>
  static void quickSortDescending(
    templateList& other,
    const List<Object>::Comparator* order = nullptr,
    otherList* carbonCopy = 0
  ) {
    List<Object>::quickSortAscending<templateList, otherList>(other, order, carbonCopy);
    other.reverseElements();
    if (carbonCopy != 0) {
      carbonCopy->reverseElements();
    }
  }

  template <class templateList, class otherList>
  static void quickSortAscendingOrder(
    templateList& other,
    int bottomIndex,
    int topIndex,
    const List<Object>::Comparator& order,
    otherList* carbonCopy
  ) {
    if (topIndex <= bottomIndex) {
      return;
    }
    int highIndex = topIndex;
    for (int lowIndex = bottomIndex + 1; lowIndex <= highIndex; lowIndex ++) {
      if (order.greaterThan(other[lowIndex], (other[bottomIndex]))) {
        other.swapTwoIndices(lowIndex, highIndex);
        if (carbonCopy != 0) {
          carbonCopy->swapTwoIndices(lowIndex, highIndex);
        }
        lowIndex --;
        highIndex --;
      }
    }
    if (order.greaterThan(other[highIndex], other[bottomIndex])) {
      if (highIndex == bottomIndex) {
        std::stringstream crashStream;
        crashStream
        << "Bad strict order: the order claims that object "
        << other[highIndex] << " of index "
        << highIndex << " is strictly greater than itself which is not allowed for strict orders. "
        << "Maybe the programmer has given a "
        << "non-strict order instead of strict one by mistake? ";
        fatalCrash(crashStream.str());
      }
      highIndex --;
    }
    other.swapTwoIndices(bottomIndex, highIndex);
    if (carbonCopy != 0) {
      carbonCopy->swapTwoIndices(bottomIndex, highIndex);
    }
    List<Object>::quickSortAscendingOrder<templateList, otherList>(
      other, bottomIndex, highIndex - 1, order, carbonCopy
    );
    List<Object>::quickSortAscendingOrder<templateList, otherList>(
      other, highIndex + 1, topIndex, order, carbonCopy
    );
  }
  template <class templateList, class otherList>
  static void quickSortAscendingNoOrder(
    templateList& input,
    int bottomIndex,
    int topIndex,
    otherList* carbonCopy
  ) {
    if (topIndex <= bottomIndex) {
      return;
    }
    int highIndex = topIndex;
    for (int lowIndex = bottomIndex + 1; lowIndex <= highIndex; lowIndex ++) {
      if (input[lowIndex] > input[bottomIndex]) {
        input.swapTwoIndices(lowIndex, highIndex);
        if (carbonCopy != 0) {
          carbonCopy->swapTwoIndices(lowIndex, highIndex);
        }
        lowIndex --;
        highIndex --;
      }
    }
    if (input[highIndex] > input[bottomIndex]) {
      highIndex --;
    }
    input.swapTwoIndices(bottomIndex, highIndex);
    if (carbonCopy != 0) {
      carbonCopy->swapTwoIndices(bottomIndex, highIndex);
    }
    List<Object>::quickSortAscendingNoOrder(input, bottomIndex, highIndex - 1, carbonCopy);
    List<Object>::quickSortAscendingNoOrder(input, highIndex + 1, topIndex, carbonCopy);
  }
  bool hasCommonElementWith(List<Object>& right);
  void swapTwoIndices(int index1, int index2);
  void cycleIndices(const List<int>& cycle);
  void permuteIndices(const List<List<int> >& cycles);
  std::string toStringConcatenate() const;
  std::string toString(FormatExpressions* format) const;
  std::string toString() const;
  std::string toStringCommaDelimited(FormatExpressions* format) const;
  std::string toStringCommaDelimited() const;
  void toString(std::string& output, FormatExpressions* format = nullptr) const {
    output = this->toString(format);
  }
  int getIndex(const Object& o) const;
  bool contains(const Object& o) const {
    return this->getIndex(o) != - 1;
  }
  bool containsAtLeastOneCopyOfEach(const List<Object>& other) const {
    for (int i = 0; i < other.size; i ++) {
      if (!this->contains(other[i])) {
        return false;
      }
    }
    return true;
  }

  // The "sorted" family of operations
  // assume that the list is sorted in ascending order
  // and that each object has the comparison operator <=.
  // The objective is fast, stable insertion sorting of partially ordered data.
  // The data that has x <= y and y <= x is called a bin. The methods
  // sortedIndexFirstGreaterThan and sortedIndexFirstNotLessThan find the edges of the bin.
  // The insertion methods return the index at which the object was inserted, or - 1
  // sortedInsert inserts the element at the end of the bin.
  // sortedInsertDontDup searches the bin for the element before inserting.
  // sortedInsertNextTo searches the bin for an equal element and inserts the new element
  //   next to the existing element
  // sortedGetIndex searches the bin for the element and returns its index
  // sortedContains searches the bin for the element and returns whether it was found.
  int sortedIndexFirstNotLessThan(const Object& o) const {
    if (this->size == 0) {
      return 0;
    }
    int start = 0;
    int end = this->size;
    while (true) {
      int halfLength = (end - start) / 2;
      if (halfLength == 0) {
        if ((start == this->size ) || (o > this->objects[start])) {
          return end;
        } else {
          return start;
        }
      }
      int n = start + halfLength;
      if (o > this->objects[n]) {
        start = n + 1;
      } else {
        end = n;
      }
    }
  }

  int sortedIndexFirstGreaterThan(const Object& o) const {
    if (this->size == 0) {
      return 0;
    }
    int start = 0;
    int end = this->size;
    while (true) {
      int halflength = (end - start) / 2;
      if (halflength == 0) {
        if (!(this->objects[start] > o)) {
          return end;
        } else {
          return start;
        }
      }
      int n = start + halflength;
      if (!(this->objects[n] > o)) {
        start = n + 1;
      } else {
        end = n;
      }
    }
  }

  // sortedGetIndex
  int sortedGetIndex(const Object& o) const {
    int n = this->sortedIndexFirstNotLessThan(o);
    if (n != this->size) {
      if (this->objects[n] == o) {
        return n;
      }
    }
    for (int i = n - 1; i > - 1; i --) {
      if (this->objects[i] == o) {
        return i;
      }
      if (o > this->objects[i]) {
        return - 1;
      }
    }
    return - 1;
  }
  bool sortedContains(const Object& o) const {
    return this->sortedGetIndex(o) != - 1;
  }
  int sortedInsert(const Object& o) {
    int n = sortedIndexFirstGreaterThan(o);
    if (n == this->size) {
      this->addOnTop(o);
    } else {
      this->insertAtIndexShiftElementsUp(o, n);
    }
    return n;
  }

  int sortedInsertDontDup(const Object& o) {
    int n = sortedIndexFirstNotLessThan(o);
    for (int i = n; i < this->size; i ++) {
      if (this->objects[i] == o) {
        return - 1;
      }
      if (this->objects[i] > o) {
        this->insertAtIndexShiftElementsUp(o, i);
        return i;
      }
    }
    this->addOnTop(o);
    return this->size - 1;
  }

  int sortedInsertNextToExisting(const Object& o) {
    int n = sortedIndexFirstNotLessThan(o);
    int existing_index = - 1;
    int bin_end_index = - 1;
    for (int i = n; i < this->size; i ++) {
      if (this->objects[i] == o) {
        existing_index = i;
      }
      if (this->objects[i] > o) {
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
    this->insertAtIndexShiftElementsUp(o, outdex);
    return outdex;
  }

  void rotate(int r) {
    std::rotate(this->objects, this->objects + r, this->objects + (this->size - 1));
  }
  Object* lastObject() const;// <-Registering stack trace forbidden! Multithreading deadlock alert.
  void releaseMemory();

  unsigned int hashFunction() const {
    unsigned int result = 0;
    int j = - 1;
    int k = 0;
    int hashCounter = 0;
    for (int i = 0; i < this->size; i ++) {
      j ++;
      k ++;
      result += k * HashConstants::getConstantIncrementCounter(hashCounter) * HashFunctions::hashFunction(objects[i]);
    }
    return result;
  }
  static unsigned int hashFunction(const List<Object>& input) {
    return input.hashFunction();
  }
  void intersectWith(const List<Object>& other, List<Object>& output) const;
  void operator=(const std::string& right) {
    this->setSize(right.size());
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] = right[static_cast<unsigned>(i)];
    }
  }
  void operator=(const std::initializer_list<Object>& other) {
    this->setSize(other.size());
    int counter = 0;
    for (
      auto otherIterator = other.begin();
      otherIterator != other.end();
      otherIterator ++, counter ++
    ) {
      this->objects[counter] = *otherIterator;
    }
  }
  void operator=(const List<Object>& right) {
    if (this == &right) {
      return;
    }
    this->setSize(right.size);
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] = right.objects[i];
    }
  }
  template <class otherObjectType>
  void operator=(const List<otherObjectType>& right) {
    this->setSize(right.size);
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] = right.objects[i];
    }
  }
  static void swap(List<Object>& l1, List<Object>& l2);
  void reverseElements();
  void reverseRange(int rangeBegin, int rangeEnd);
  bool isEqualTo(const List<Object>& other) const {
    if (this->size != other.size) {
      return false;
    }
    for (int i = 0; i < other.size; i ++) {
      if (!(this->objects[i] == other.objects[i])) {
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
    this->checkConsistency();
    if ((i >= this->size) || i < 0) {
      std::stringstream crashReport;
      crashReport
      << "Attempt to access the entry of index "
      << i << " in an array of " << this->size << " elements. ";
      fatalCrash(crashReport.str());
    }
    return this->objects[i];
  }
  bool operator!=(const List<Object>& other) const {
    return !this->isEqualTo(other);
  }
  bool operator==(const std::string& other) {
    if ((static_cast<unsigned>(this->size)) != other.size()) {
      return false;
    }
    for (int i = 0; i < this->size; i ++) {
      if (!(this->objects[i] == other[i])) {
        return false;
      }
    }
    return true;
  }
  bool operator==(const List<Object>& other) const {
    return this->isEqualTo(other);
  }
  bool operator>(const List<Object>& other) const;
  bool operator<(const List<Object>& other) const;
  void shiftUpExpandOnTop(int startingIndex);
  void shiftUpExpandOnTopRepeated(int startingIndex, int numberOfNewElements);
  void assignListList(const List<List<Object> >& input) {
    int count = 0;
    for (int i = 0; i < input.size; i ++) {
      count += input[i].size;
    }
    this->setSize(0);
    this->reserve(count);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < input[i].size; j ++) {
        this->addOnTop(input[i][j]);
      }
    }
  }
};

template <typename Object>
class ListZeroAfterUse {
  public:
  List<Object> data;
  ~ListZeroAfterUse() {
    for (int i = 0; i < this->data.actualSize; i ++) {
      // We zero the underlying buffer:
      // this->data[i] may be out of bounds if
      // the actual size does not equal the size.
      this->data.objects[i] = 0;
    }
  }
};

template <class ObjectType1, class ObjectType2, unsigned int hashFunction1(const ObjectType1&), unsigned int hashFunction2(const ObjectType2&)>
class Pair {
  friend std::ostream& operator << (
    std::ostream& output, const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& pair
  ) {
    output << "("  << pair.object1 << ", " <<  pair.object2 << ")";
    return output;
  }
public:
  ObjectType1 object1;
  ObjectType2 object2;
  Pair(){}
  Pair(const ObjectType1& o1, const ObjectType2& o2): object1(o1), object2(o2) {}
  static unsigned int hashFunction(
    const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& input
  ) {
    return
      HashConstants::constant0 * hashFunction1(input.object1) +
      HashConstants::constant1 * hashFunction2(input.object2);
  }
  unsigned int hashFunction() const {
    return Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>::hashFunction(*this);
  }
  void operator=(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other) {
    this->object1 = other.object1;
    this->object2 = other.object2;
  }
  bool operator==(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other) const {
    return this->object1 == other.object1 && this->object2 == other.object2;
  }
  bool operator>(const Pair<ObjectType1, ObjectType2, hashFunction1, hashFunction2>& other) {
    if (this->object1 > other.object1) {
      return true;
    }
    if (other.object1 > this->object1) {
      return false;
    }
    if (this->object2 > other.object2) {
      return true;
    }
    if (other.object2 > this->object2) {
      return false;
    }
    return false;
  }
};
typedef Pair<int, int, HashFunctions::hashFunction, HashFunctions::hashFunction> PairInts;

template <class Object, class TemplateList, unsigned int hashFunctionObject(const Object&) = Object::hashFunction>
class HashTemplate: public TemplateList {
private:
  Object popIndexShiftDown(int index);
  void reverseElements();
  void shiftUpExpandOnTop(int startingIndex);

protected:
  // In the i^th bucket, we store the indices of all objects
  // for which (hash(object) mod hashBuckets.size) equals i.
  // To look up whether an object is in the hash list, we
  // compute the object's hash, then look up the corresponding bucket
  // and check all entries whose indices are listed in the bucket.
  List<List<int> > hashBuckets;
public:
  unsigned int getHash(const Object& input) const {
    unsigned int result = hashFunctionObject(input);
    result %= this->hashBuckets.size;
    return result;
  }
  void clear() {
    // If the hashed list is somewhat sparse, and the index is somewhat large,
    // (above 20 entries), we clear the hash by finding the occupied hashes and
    // nullifying them one by one.
    // else, we simply go through the entire hash index and nullify everything.
    // Note: for better performance, 20 entries should probably be changed to 100+,
    // however the smaller number is a good consistency test (it would make it easier to
    // detect a faulty hash).
    // If this program ever gets to do some hard-core number crunching, the 20 entries
    // should be increased.
    if (this->isSparse() && this->hashBuckets.size > 20) {
      for (int i = 0; i < this->size; i ++) {
        int hashIndex = this->getHash((*this)[i]);
        this->hashBuckets[hashIndex].size = 0;
      }
    } else {
      for (int i = 0; i < this->hashBuckets.size; i ++) {
        this->hashBuckets[i].size = 0;
      }
    }
    this->size = 0;
  }
  std::string getReport() {
    std::stringstream out;
    out << "<br>List size: " << this->size;
    out << "<br>Hash size: " << this->hashBuckets.size;
    int maxHashSize = 0;
    int numNonZeroHashes = 0;
    for (int i = 0; i < this->hashBuckets.size; i ++) {
      maxHashSize = MathRoutines::maximum(maxHashSize, this->hashBuckets[i].size);
      if (this->hashBuckets[i].size > 0) {
        numNonZeroHashes ++;
      }
    }
    out << "<br>Max hash array size: " << maxHashSize;
    out << "<br>Average hash array size: " << (static_cast<double>(this->size)) / (static_cast<double>(numNonZeroHashes));
    return out.str();
  }
  void addOnTop(const Object& o) {
    unsigned int hashIndex = this->getHash(o);
    this->hashBuckets[hashIndex].addOnTop(this->size);
    this->TemplateList::addOnTop(o);
    if (this->size > 1) {
      this->adjustHashes();
    }
  }
  void addListOnTop(const List<Object>& input) {
    this->setExpectedSize(this->size + input.size);
    for (int i = 0; i < input.size; i ++) {
      this->addOnTop(input[i]);
    }
  }
  void removeFirstOccurenceSwapWithLast(const Object& o) {
    int index = this->getIndex(o);
    if (index == - 1) {
      return;
    }
    this->removeIndexSwapWithLast(index);
  }
  const List<int>& getHashArray(int hashIndex) const {
    return this->hashBuckets[hashIndex];
  }
  void grandMasterConsistencyCheck() const {
    for (int i = 0; i < this->hashBuckets.size; i ++) {
      List<int>& current = this->hashBuckets[i];
      for (int j = 0; j < current.size; j ++) {
        int index = current[j];
        if (index >= this->size) {
          std::stringstream commentsOnCrash;
          commentsOnCrash << "Hash lookup array of index "
          << i << ", entry of index " << j << " reports index "
          << index << " but I have only " << this->size << " entries. ";
          fatalCrash(commentsOnCrash.str());
        }
        if (this->getHash((*this)[index]) != static_cast<unsigned>(i)) {
          std::stringstream commentsOnCrash;

          commentsOnCrash << "<hr>The hashed element in position "
          << index << " is recorded in hash array of index "
          << i << ", however its hash value is instead "
          << this->getHash((*this)[index]) << ". The hash size is "
          << this->hashBuckets.size << "<br>hashes of objects: ";
          for (int l = 0; l < this->size; l ++) {
            commentsOnCrash << this->getHash((*this)[l]) << "= "
            << this->getHash((*this)[l]) % this->hashBuckets.size << ", ";
          }
          commentsOnCrash << "<br>hashes recorded: ";
          for (int l = 0; l < this->hashBuckets.size; l ++) {
            for (int k = 0; k < this->hashBuckets[l].size; k ++) {
              commentsOnCrash << this->hashBuckets[l][k] << ", ";
            }
          }
          fatalCrash(commentsOnCrash.str());
        }
      }
    }
  }
  int addNoRepetitionOrReturnIndexFirst(const Object& o) {
    int result = this->getIndex(o);
    if (result != - 1) {
      return result;
    }
    this->addOnTop(o);
    return this->size - 1;
  }
  void adjustHashes() {
    this->setExpectedSize(this->size);
  }
  void addOnTopNoRepetition(const List<Object>& other) {
    this->setExpectedSize(this->size + other.size);
    for (int i = 0; i < other.size; i ++) {
      this->addOnTopNoRepetition(other.objects[i]);
    }
  }
  bool addOnTopNoRepetition(const Object& o) {
    if (this->getIndex(o) != - 1) {
      return false;
    }
    this->addOnTop(o);
    return true;
  }
  bool addOnTopNoRepetitionMustBeNew(const Object& o) {
    if (this->getIndex(o) != - 1) {
      std::stringstream crashStream;
      crashStream << "Request to add object "
      << o << " does not allow repetition, "
      << "but the object is already in the list. ";
      fatalCrash(crashStream.str());
    }
    this->addOnTop(o);
    return true;
  }
  Object popLastObject() {
    Object result = *this->lastObject();
    this->removeIndexSwapWithLast(this->size - 1);
    return result;
  }
  Object popIndexSwapWithLast(int index) {
    Object result = (*this)[index];
    this->removeIndexSwapWithLast(index);
    return result;
  }
  void removeLastObject() {
    this->removeIndexSwapWithLast(this->size - 1);
  }
  void removeIndexShiftDown(int index) {
    for (int i = index; i < this->size - 1; i ++) {
      this->setObjectAtIndex(i, (*this)[i + 1]);
    }
    this->removeLastObject();
  }
  void setObjectAtIndex(int index, const Object& input) {
    if (index < 0 || index >= this->size) {
      std::stringstream commentsOnCrash;
      commentsOnCrash << "Attempt to pop out index "
      << index << " out of hashed array "
      << "of size " << this->size << ". ";
      fatalCrash(commentsOnCrash.str());
    }
    int hashIndexPop = this->getHash(this->objects[index]);
    this->hashBuckets[hashIndexPop].removeFirstOccurenceSwapWithLast(index);
    int hashIndexIncoming = this->getHash(input);
    this->hashBuckets[hashIndexIncoming].addOnTop(index);
    this->objects[index] = input;
  }
  void removeIndexSwapWithLast(int index) {
    if (index < 0 || index >= this->size) {
      std::stringstream commentsOnCrash;
      commentsOnCrash << "Attemp to pop out index "
      << index << " out of hashed array "
      << "of size " << this->size << ". ";
      fatalCrash(commentsOnCrash.str());
    }
    Object* oPop = &this->objects[index];
    int hashIndexPop = this->getHash(*oPop);
    this->hashBuckets[hashIndexPop].removeFirstOccurenceSwapWithLast(index);
    if (index == this->size - 1) {
      this->size --;
      return;
    }
    int lastIndex = this->size - 1;
    Object* oTop = &this->objects[lastIndex];
    int hashIndexTop = this->getHash(*oTop);
    this->hashBuckets[hashIndexTop].removeFirstOccurenceSwapWithLast(lastIndex);
    this->hashBuckets[hashIndexTop].addOnTop(index);
    this->TemplateList::removeIndexSwapWithLast(index);
  }
  void swapTwoIndices(int left, int right) {
    Object tempO;
    int i1Hash = this->getHash(this->objects[left]);
    int i2Hash = this->getHash(this->objects[right]);
    this->hashBuckets[i1Hash].removeFirstOccurenceSwapWithLast(left);
    this->hashBuckets[i2Hash].removeFirstOccurenceSwapWithLast(right);
    tempO = this->objects[left];
    this->objects[left] = this->objects[right];
    this->objects[right] = tempO;
    this->hashBuckets[i1Hash].addOnTop(right);
    this->hashBuckets[i2Hash].addOnTop(left);
  }
  bool contains(const Object& o) const {
    return this->getIndex(o) != - 1;
  }
  bool contains(const List<Object>& input) const {
    for (int i = 0; i < input.size; i ++) {
      if (this->getIndex(input[i]) == - 1) {
        return false;
      }
    }
    return true;
  }
  int getIndex(const Object& o) const {
    unsigned int hashIndex = this->getHash(o);
    for (int i = 0; i < this->hashBuckets[hashIndex].size; i ++) {
      int j = this->hashBuckets[hashIndex].objects[i];
      if (j >= this->size) {
        std::stringstream commentsOnCrash;
        commentsOnCrash << "Corrupt hash table: at hashindex = "
        << hashIndex << " I need to look up index " << j
        << " but I have only " << this->size << " elements. ";
        fatalCrash(commentsOnCrash.str());
      }
      if ((*this)[j] == o) {
        return j;
      }
    }
    return - 1;
  }
  int getIndexNoFail(const Object& o) const {
    int result = this->getIndex(o);
    if (result == - 1) {
      std::stringstream errorStream;
      errorStream
      << "Request to get the index of object " << o
      << " with a function that does not allow failure, "
      << "however, the container array does not contain this object. ";
      fatalCrash(errorStream.str());
    }
    return result;
  }
  void setSize(int inputSize) {
    if (inputSize < 0) {
      inputSize = 0;
    }
    if (inputSize > this->size) {
      fatalCrash(
        "Function setSize is allowed for hashed lists only when "
        "resizing the hashed list to smaller. "
        "This is because if I was to resize to larger, "
        "I would have to allocate non-initialized "
        "objects, and those will have to be rehashed "
        "which does not make sense. "
      );
    }
    for (int i = this->size - 1; i >= inputSize; i --) {
      this->popLastObject();
    }
  }

  bool isSparseRelativeToExpectedSize(int expectedSize) const {
    return expectedSize * 3 < this->hashBuckets.size;
  }
  bool isSparse() const {
    return this->isSparseRelativeToExpectedSize(this->size);
  }
  void setExpectedSize(int expectedSize) {
    if (expectedSize < 1) {
      return;
    }
    if (expectedSize == 1 || expectedSize == 2) {
      this->setHashSize(1);
      return;
    }
    this->TemplateList::setExpectedSize(expectedSize);
    if (!this->isSparseRelativeToExpectedSize(expectedSize)) {
      this->setHashSize(expectedSize * 5);
    }
  }
  inline void setHashSize(int inputHashSize) {
    return this->setHashSize(static_cast<unsigned>(inputHashSize));
  }
  void setHashSize(unsigned int desiredHashSize) {
    if (desiredHashSize == static_cast<unsigned>(this->hashBuckets.size)) {
      return;
    }
    MacroIncrementCounter(GlobalStatistics::numberOfHashResizes);
    List<int> emptyList; //<-empty list has size 0
    this->hashBuckets.initializeFillInObject(desiredHashSize, emptyList);
    if (this->size > 0) {
      for (int i = 0; i < this->size; i ++) {
        int index = this->getHash((*this)[i]);
        this->hashBuckets[index].addOnTop(i);
      }
    }
  }
  template<typename otherType = int>
  void quickSortAscending(
    typename List<Object>::Comparator* order = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    List<Object> myCopy;
    myCopy = *this;
    myCopy.quickSortAscending(order, carbonCopy);
    this->operator=(myCopy);
  }
  template<typename otherType>
  void quickSortDescending(
    typename List<Object>::Comparator* order = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    List<Object> myCopy;
    myCopy = *this;
    myCopy.quickSortDescending(order, carbonCopy);
    this->operator=(myCopy);
  }
  void initializeHashesToOne() {
    this->hashBuckets.setSize(1);
    this->hashBuckets[0].size = 0;
  }
  HashTemplate(const HashTemplate& other) {
    this->initializeHashesToOne();
    this->operator=(other);
  }
  HashTemplate() {
    this->initializeHashesToOne();
  }
  std::string toString(FormatExpressions* format) const {
    return this->List<Object>::toString(format);
  }
  std::string toString() const {
    return this->::List<Object>::toString();
  }
  void operator=(const HashedList<Object, hashFunctionObject>& from) {
    if (&from == this) {
      return;
    }
    this->clear();
    this->setHashSize(from.hashBuckets.size);
    this->::List<Object>::operator=(from);
    if (from.isSparse()) {
      for (int i = 0; i < this->size; i ++) {
        unsigned int hashIndex = this->getHash(this->objects[i]);
        this->hashBuckets[hashIndex].reserve(from.hashBuckets[hashIndex].size);
        this->hashBuckets[hashIndex].addOnTop(i);
      }
    } else {
      this->hashBuckets = from.hashBuckets;
    }
  }
  const Object& operator[](int i) const {
    return TemplateList::operator[](i);
  }
  Object& getElement(int objectIndex) const {
    return TemplateList::operator[](objectIndex);
  }
  void operator=(const TemplateList& other) {
    if (this == &other) {
      return;
    }
    this->clear();
    this->setExpectedSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->addOnTop(other.objects[i]);
    }
  }
};

template <class Object, unsigned int hashFunction(const Object&) = Object::hashFunction>
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
  // Note The following function specializations are declared entirely in order to
  // facilitate autocomplete in my current IDE. If I find a better autocompletion
  // IDE the following should be removed.
  void addOnTopNoRepetition(const List<Object>& input) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::addOnTopNoRepetition(input);
  }
  bool addOnTopNoRepetition(const Object& o) {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::addOnTopNoRepetition(o);
  }
  void addOnTop(const Object& o) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::addOnTop(o);
  }
  void addListOnTop(const List<Object>& input) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::addListOnTop(input);
  }
  bool contains(const Object& o) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::contains(o);
  }
  bool contains(const List<Object>& input) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::contains(input);
  }
  Object& getElement(int objectIndex) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::getElement(objectIndex);
  }
  void setObjectAtIndex(int index, const Object& object) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::setObjectAtIndex(index, object);
  }
  void removeIndexShiftDown(int index) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::removeIndexShiftDown(index);
  }
  void removeIndexSwapWithLast(int index) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::removeIndexSwapWithLast(index);
  }
  int getIndex(const Object& o) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::getIndex(o);
  }
  int getIndexNoFail(const Object& o) const {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::getIndexNoFail(o);
  }
  int addNoRepetitionOrReturnIndexFirst(const Object& o) {
    return this->::HashTemplate<Object, List<Object>, hashFunction>::addNoRepetitionOrReturnIndexFirst(o);
  }
  template <typename otherType = int>
  void quickSortAscending(
    typename List<Object>::Comparator* order = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::quickSortAscending(order, carbonCopy);
  }
  template <typename otherType = int>
  void quickSortDescending(
    typename List<Object>::Comparator* order = nullptr,
    List<otherType>* carbonCopy = nullptr
  ) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::quickSortDescending(order, carbonCopy);
  }
  void setExpectedSize(int expectedSize) {
    this->::HashTemplate<Object, List<Object>, hashFunction>::setExpectedSize(expectedSize);
  }
};

// Class used to avoid issues with gcc compilation.
// This class should probably be removed as soon as the issue is resolved.
template <class Object>
class HashedListSpecialized: public HashedList<Object, Object::hashFunction> {
};

struct StackInfo {
public:
  std::string fileName;
  int line;
  std::string functionName;
  void operator=(const StackInfo& other) {
    this->fileName = other.fileName;
    this->line = other.line;
    this->functionName = other.functionName;
  }
};

class ProgressReport {
private:
  void initialize();
public:
  int currentLevel;
  int threadIndex;
  int ticks;
  int ticksPerReport;
  // Constant GlobalVariables::Response::ReportType
  int reportType;
  bool flagInitialized;
  std::string name;
  // Call tickAndWantReport before generating report.
  // If a report is not wanted, it is wise to not generate one:
  // the string operations required to generate some progress reports
  // are expected to cost more than the mathematical computations
  // they are reporting.
  void report(const std::string& stringToReport);
  bool tickAndWantReport();
  ProgressReport(const std::string& inputName = "") {
    this->name = inputName;
    this->initialize();
  }
  ProgressReport(int inputTicksPerReport, int inputReportType) {
    this->initialize();
    this->ticksPerReport = inputTicksPerReport;
    this->reportType = inputReportType;
  }
  ~ProgressReport();
};

// Do not use for cryptographic purposes.
// Intended use:
// generate random numbers for mathematical problems/education.
class UnsecurePseudoRandomGenerator {
private:
  uint32_t randomSeed;
public:
  const unsigned int maximumRandomSeed = 1000000000;
  int64_t randomNumbersGenerated;
  int bytesConsumed;
  List<unsigned char> state;
  UnsecurePseudoRandomGenerator();
  uint32_t getRandomSeed();
  void setRandomSeedSmall(uint32_t inputRandomSeed);
  void setRandomSeedLarge(const LargeInteger& inputRandomSeed, std::stringstream* comments);
  unsigned int getRandomNonNegativeLessThanMaximumSeed();
  signed getRandom();
};

template<class Base>
std::iostream& operator<<(std::iostream& output, const Complex<Base>& input);

template <typename Element>
std::iostream& operator<<(std::iostream& output, const Matrix<Element>& matrix) {
  output << "<table>";
  for (int i = 0; i < matrix.numberOfRows; i ++) {
    output << "<tr>";
    for (int j = 0; j < matrix.numberOfColumns; j ++) {
      output << "<td>";
      output << matrix.elements[i][j];
      output << "</td> ";
    }
    output << "</tr>\n";
  }
  output << "</table>";
  return output;
}

template <class Coefficient>
std::ostream& operator<<(std::ostream& out, const Vector<Coefficient>& input) {
  out  << "(";
  for (int i = 0; i < input.size; i ++) {
    out << input[i];
    if (i != input.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  return out;
}

template <class Object>
template <class compareClass, class carbonCopyType>
bool List<Object>::quickSortAscendingCustomRecursive(
  int bottomIndex,
  int topIndex,
  compareClass& comparator,
  List<carbonCopyType>* carbonCopy
) {
  if (topIndex <= bottomIndex) {
    return true;
  }
  int highIndex = topIndex;
  for (int lowIndex = bottomIndex + 1; lowIndex <= highIndex; lowIndex ++) {
    if (comparator.compareLeftGreaterThanRight(
      this->objects[lowIndex], this->objects[bottomIndex]
    )) {
      if (carbonCopy != nullptr) {
        carbonCopy->swapTwoIndices(lowIndex, highIndex);
      }
      this->swapTwoIndices(lowIndex, highIndex);
      lowIndex --;
      highIndex --;
    }
  }
  if (comparator.compareLeftGreaterThanRight(this->objects[highIndex], this->objects[bottomIndex])) {
    highIndex --;
  }
  if (carbonCopy != nullptr) {
    carbonCopy->swapTwoIndices(bottomIndex, highIndex);
  }
  this->swapTwoIndices(bottomIndex, highIndex);
  if (!this->quickSortAscendingCustomRecursive(bottomIndex, highIndex - 1, comparator, carbonCopy)) {
    return false;
  }
  if (!this->quickSortAscendingCustomRecursive(highIndex + 1, topIndex, comparator, carbonCopy)) {
    return false;
  }
  return true;
}

template <class Object>
void List<Object>::addListOnTop(const List<Object>& incoming) {
  int oldsize = this->size;
  int otherSize = incoming.size;
  this->setSize(oldsize + otherSize);
  for (int i = 0; i < otherSize; i ++) {
    this->objects[i + oldsize] = incoming.objects[i];
  }
}

template<class Object>
void List<Object>::swapTwoIndices(int index1, int index2) {
  if (index1 < 0 || index1 >= this->size || index2 < 0 || index2 >= this->size) {
    std::stringstream commentsOnCrash;
    commentsOnCrash
    << "Requested elements with indices "
    << index1 << " and " << index2 << " in a list that has "
    << this->size << " elements. ";
    fatalCrash(commentsOnCrash.str());
  }
  if (index1 == index2) {
    return;
  }
  Object tempO;
  tempO = this->objects[index1];
  this->objects[index1] = this->objects[index2];
  this->objects[index2] = tempO;
}

template<class Object>
void List<Object>::cycleIndices(const List<int>& cycle) {
  if (cycle.size < 2) {
    return;
  }
  for (int i = 0; i < cycle.size; i ++) {
    if ((cycle[i] >= this->size) || (cycle[i] < 0)) {
      std::stringstream commentsOnCrash;
      commentsOnCrash << "Request to cycle indices "
      << cycle << " in list of " << this->size << " elements.";
      fatalCrash(commentsOnCrash.str());
    }
  }
  Object tail;
  tail = this->objects[cycle[cycle.size - 1]];
  for (int i = cycle.size - 1; i != 0; i --) {
    this->objects[cycle[i]] = this->objects[cycle[i - 1]];
  }
  this->objects[cycle[0]] = tail;
}

template<class Object>
void List<Object>::permuteIndices(const List<List<int> >& cycles) {
  for (int i = 0; i < cycles.size; i ++) {
    this->cycleIndices(cycles[i]);
  }
}

template<class Object>
int List<Object>::getIndex(const Object& o) const {
  for (int i = 0; i < this->size; i ++) {
    if (this->objects[i] == o) {
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
  smallL->setSize(bigL->size);
  Object tempO;
  for (int i = 0; i < smallSize; i ++) {
    tempO = smallL->objects[i];
    smallL->objects[i] = bigL->objects[i];
    bigL->objects[i] = tempO;
  }
  for (int i = smallSize; i < bigL->size; i ++) {
    smallL->objects[i] = bigL->objects[i];
  }
  bigL->size = smallSize;
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
    if (this->objects[i] > other.objects[i]) {
      return true;
    }
    if (other.objects[i] > this->objects[i]) {
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
    if (this->objects[i] > other.objects[i]) {
      return false;
    }
    if (other.objects[i] > this->objects[i]) {
      return true;
    }
  }
  return false;
}

template <class Object>
void List<Object>::shiftUpExpandOnTop(int startingIndex) {
  this->shiftUpExpandOnTopRepeated(startingIndex, 1);
}

template <class Object>
void List<Object>::shiftUpExpandOnTopRepeated(int startingIndex, int numberOfNewElements) {
  this->setSize(this->size + numberOfNewElements);
  int lowBound = startingIndex + numberOfNewElements;
  for (int i = this->size - 1; i >= lowBound; i --) {
    this->objects[i] = this->objects[i - numberOfNewElements];
  }
}

template <class Object>
void List<Object>::sliceInPlace(int startingIndex, int sizeOfSlice) {
  this->slice(startingIndex, sizeOfSlice, *this);
}

template <class Object>
List<Object> List<Object>::sliceCopy(int startingIndex, int sizeOfSlice) const {
  List<Object> result;
  this->slice(startingIndex, sizeOfSlice, result);
  return result;
}

template <class Object>
void List<Object>::slice(int startingIndex, int sizeOfSlice, List<Object>& output) const {
  // output allowed to equal this
  if (startingIndex < 0) {
    startingIndex = 0;
  }
  if (sizeOfSlice < 0) {
    sizeOfSlice = 0;
  }
  if (sizeOfSlice + startingIndex > this->size) {
    sizeOfSlice = this->size - startingIndex;
  }
  if (output.size < sizeOfSlice) {
    // Implies output is not this.
    // In case output is this we only need to resize at the end.
    output.setSize(sizeOfSlice);
  }
  for (int i = 0; i < sizeOfSlice; i ++) {
    output[i] = this->objects[i + startingIndex];
  }
  output.setSize(sizeOfSlice);
}

template <class Object>
void List<Object>::initializeFillInObject(int incomingSize, const Object& o) {
  this->setSize(incomingSize);
  for (int i = 0; i < this->size; i ++) {
    this->objects[i] = o;
  }
}

template <class Object>
bool List<Object>::addOnTopNoRepetition(const Object& o) {
  if (this->getIndex(o) != - 1) {
    return false;
  }
  this->addOnTop(o);
  return true;
}

template <class Object>
Object* List<Object>::lastObject() const {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->size <= 0) {
    std::stringstream commentsOnCrash;
    commentsOnCrash
    << "Trying to fetch the last object of an array with "
    << this->size << " elements. ";
    fatalCrash(commentsOnCrash.str());
  }
  return &this->objects[this->size - 1];
}

template <class Object>
bool List<Object>::hasCommonElementWith(List<Object>& right) {
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < right.size; j ++) {
      if (this->objects[i] == right.objects[j]) {
        return true;
      }
    }
  }
  return false;
}

template <class Object>
void List<Object>::reserve(int desiredSize) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->actualSize < desiredSize) {
    this->expandArrayOnTop(desiredSize - this->actualSize);
  }
}

template <class Object>
void List<Object>::removeFirstOccurenceSwapWithLast(const Object& o) {
  for (int i = 0; i < this->size; i ++) {
    if (o == this->objects[i]) {
      this->removeIndexSwapWithLast(i);
      return;
    }
  }
}

template <class Object>
void List<Object>::setSize(int incomingSize) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (incomingSize < 0) {
    incomingSize = 0;
  }
  this->setExpectedSize(incomingSize);
  this->reserve(incomingSize);
  this->size = incomingSize;
}

template <class Object>
std::string List<Object>::toString() const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->objects[i].toString() << "\n";
  }
  return out.str();
}

template <class Object>
std::string List<Object>::toStringCommaDelimited() const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->objects[i];
    if (i != this->size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

template <class Object>
std::string List<Object>::toStringConcatenate() const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->objects[i];
  }
  return out.str();
}

template <class Object>
std::string List<Object>::toString(FormatExpressions* format) const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->objects[i].toString(format) << "\n";
  }
  return out.str();
}

template <class Object>
std::string List<Object>::toStringCommaDelimited(FormatExpressions* format) const {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    out << this->objects[i].toString(format);
    if (i != this->size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

template <class Object>
void List<Object>::removeIndexSwapWithLast(int index) {
  if (this->size == 0) {
    return;
  }
  this->size --;
  this->objects[index] = this->objects[this->size];
}

template <class Object>
void List<Object>::removeLastObject() {
  if (this->size == 0) {
    fatalCrash("Attempt to pop empty list. ");
  }
  this->size --;
}

template <class Object>
Object List<Object>::popLastObject() {
  if (this->size == 0) {
    fatalCrash("Attempt to pop empty list. ");
  }
  this->size --;
  return this->objects[size];
}

template <class Object>
void List<Object>::releaseMemory() {
  delete [] this->objects;
  this->size = 0;
  this->objects = 0;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter -= this->actualSize;
  GlobalStatistics::checkPointerCounters();
#endif
  this->actualSize = 0;
}

template <class Object>
List<Object>::~List() {
  delete [] this->objects;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter -= this->actualSize;
  GlobalStatistics::checkPointerCounters();
#endif
  this->size = 0;
  this->actualSize = 0;
  this->flagDeallocated = true;
}

template <class Object>
void List<Object>::expandArrayOnTop(int increase) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (increase <= 0) {
    return;
  }
  MacroIncrementCounter(GlobalStatistics::numberOfListResizesTotal);
  Object* newArray = 0;
  try {
    newArray = new Object[this->actualSize + increase];
  } catch (std::bad_alloc&) {
    std::stringstream commentsOnCrash;
    commentsOnCrash << "Memory allocation failure: failed to allocate "
    << this->actualSize + increase << " objects. ";
    fatalCrash(commentsOnCrash.str());
  }
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter += this->actualSize + increase;
  GlobalStatistics::checkPointerCounters();
#endif
  for (int i = 0; i < this->size; i ++) {
    newArray[i] = this->objects[i];
  }
  Object* oldPointer = this->objects; // <-store the old memory block
  this->objects = newArray; // <-replace objects with newArray
  delete [] oldPointer; // <-only then we delete the non-used memory.
  // The above looks complicated but is safe:
  // if another thread jumps and requests operator[]
  // on our list exactly between the two instructions above
  // we are safe with this implementation.
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter -= this->actualSize;
  GlobalStatistics::checkPointerCounters();
#endif
  this->actualSize += increase;
}

template <class Object>
void List<Object>::reverseElements() {
  int halfSize = this->size / 2;
  for (int i = 0; i < halfSize; i ++) {
    this->swapTwoIndices(i, this->size - i - 1);
  }
}

template <class Object>
void List<Object>::reverseRange(int rangeBegin, int rangeEnd) {
  for (int i = rangeBegin, j = rangeEnd - 1; i < j; i ++, j --) {
    Object oi = this->objects[i];
    this->objects[i] = this->objects[j];
    this->objects[j] = oi;
  }
}

template <class Object>
void List<Object>::addOnTop(const Object& o) {
  // <-Registering stack trace forbidden! Multithreading deadlock alert.
  if (this->size > this->actualSize) {
    std::stringstream commentsOnCrash;
    commentsOnCrash
    << "The actual size of the list is "
    << this->actualSize << " but this->size equals " << this->size
    << ". ";
    fatalCrash(commentsOnCrash.str());
  }
  if (this->size == this->actualSize) {
    this->expandArrayOnTop(this->getNewSizeRelativeToExpectedSize(this->actualSize + 1) - this->size);
  }
  this->objects[this->size] = o;
  this->size ++;
}
#endif
