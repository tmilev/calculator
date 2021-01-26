// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader2_h_already_included
#define vpfHeader2_h_already_included

//the following  include contains all the c++ math routines used in the calculator.
#include "math_general.h"
#include "math_extra_finite_groups.h"
#include "math_extra_graph.h"
#include "math_extra_algebraic_numbers.h"
#include "general_list_references.h"
#include "math_extra_semisimple_Lie_subalgebras.h"
#include "math_extra_universal_enveloping.h"
#include "math_extra_modules_semisimple_Lie_algebras.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_elliptic_curves_implementation.h"

template <class builtIn>
class WithContext;
class ExpressionContext;

class Expression {
private:
  void reset() {
    this->owner = nullptr;
    this->children.size = 0;
    this->data = - 1;
  }
  bool setChild(int childIndexInMe, int childIndexInBoss);
  // Definitions.
  // 1. Fundamentals.
  // 1.1. An atom is an expression with zero children.
  // 1.2. At atom whose theData entry is a valid index of
  //      Calculator::operations will also be called an operation.
  //      Let the string corresponding to this index be X.
  //      Then, we say that "at atom equals the operation X".
  //      Note that this is ambiguous:
  //      theData entry of an atom can be interpreted either
  //      as an operation or as an an actual piece of data,
  //      not necessarily an index of Calculator::operations.
  //      Whenever this ambiguity becomes an issue
  //      explicit reference to the Expression::theData
  //      and Expression::children members should be made.
  // 1.3. A list is an expression with 1 or more children whose theData entry equals
  //      0. The atom 0 *MUST* be equal to Calculator::opList().
  // 1.4. An expression with 1 or more children is
  //      not allowed to have theData entry different
  //      from 0 = Calculator::opList(). The system is instructed to
  //      crash and burn shall such a configuration be detected.
  //      Rationale: keep the system simple,
  //      at the cost of leaving theData entry non-used for non-atomic expressions.
  // 2. Basic building blocks
  // 2.1. A frozen expression is a list whose first entry is an atom equal to Freeze.
  // 2.2. A sequence is a list whose first entry is an atom equal to Sequence.
  // 2.3. A bound variable is a list with two atomic entries, the first of which
  //      equals Bind.
  // 2.4. An error is a list with two entries whose first entry is an atom equal to Error,
  //      and whose second entry is a string.
  // *Note that Calculator::opList() is required to equal zero for reasons of program speed.
  // This is GUARANTEED, and you MAY assume it.
  // If you want to have a list of mathematical objects, use the Sequence
  // data structure. A sequence is a List whose first entry is an atom whose value
  // is Sequence = Calculator::opSequence().
  //
  //-------------------------------------------------------
  //
  // Expressions of built-in types.
  // 1. Expressions of built-in types represent hard-coded C++ types.
  // 2. An expression is said to be of built-in type if it is a list of 2 or 3 elements
  //    starting with an atom
  //    equal to one of a set of hard-coded names, and ending in an atom whose value
  //    is an integer that uniquely (up to operator==) identifies a C++ structure with the corresponding type.
  // 3. A context is a list of 1 or more elements starting with the atom Context. If
  //    a context has only one child (which must then be equal to the atom Context),
  //    then we say that we have an "empty context".
  // 4. An expression is said to have context if it is a list of at least two elements,
  //    the second element of which is a context.
  // 5. If an expression of built-in type has 3 children, the middle child
  //    must be a context. If an expression of built-in type has 2 children, we say that
  //    the expression does not have context.
  // 6. Two expressions of built-in type with equal types and C++ identifiers, one having a
  //    context that is empty, and the other having no context,
  //    are considered to represent one and the same element.
  //-------------------------------------------------------
  //-------------------------------------------------------
  //-------------------------------------------------------
  // Notes on Expression implementation.
  // While the preceding notes are to be considered as fixed, the following notes
  // are implementation specific and are subject to change.
  // The i^th child of an expression can be accessed as const using operator[](i).
  // The children of an expression are kept as a list of integers indicating the children's
  // position in Calculator::expressionContainer.
  // Calculator::expressionContainer is a Hashed List of references and must not be modified
  // directly in any way.
  // Motivation for this implementation choice. The original implementation
  // had Expression contain all of its children as List<Expression>, making the copy operator=
  // a recursive function. While this is one of the least-head-aching designs,
  // it also proved to be remarkably slow: here is why.
  // When processing an expression, one must traverse all of its subtrees.
  // Making temporary copies of all subtrees is then approximately O(n^2),
  // where n^2 is the number of nodes of the tree.
  // For a tree with 1k nodes this is already unacceptably slow.
  //
  // If most subtrees are subject to change, then the
  // approximate cost of O(n^2) operations seems justified.
  // However, in most computations, subtrees need not be changed - in reality, most expression
  // will arrive to the calculator in reduced or close-to-reduced form.
  //
  // This explains our choice of keeping all children of an expression as a reference to
  // an ever-growing collection of Expressions.
  // If a single instance of Calculator is to run over long periods of time,
  // it is possible that not all such expressions are in use, and we run out of RAM memory
  // due to our fault [bad], rather than because the
  // user requested a large computation [good].
  // If that is to happen, some sort of garbage collection will have to be implemented.
  // However, currently the calculator resets after each user-facing operation, so
  // this should not present a practical problem.
  public:
  Calculator* owner;
  int data;
  // List<int> children;
  HashedList<int, HashFunctions::hashFunction> children;
  bool flagDeallocated;
//////
  typedef bool (*FunctionAddress) (Calculator& calculator, const Expression& input, Expression& output);
  typedef bool (*ToStringHandler) (const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
//////
  friend std::ostream& operator<<(std::ostream& output, const Expression& element) {
    output << element.toString();
    return output;
  }
  friend Expression operator*(const Expression& left, const Expression& right);
  friend Expression operator/(const Expression& left, const Expression& right);
  friend Expression operator+(const Expression& left, const Expression& right);
  friend Expression operator-(const Expression& left, const Expression& right);

  void reset(Calculator& newBoss, int numExpectedChildren = 0) {
    this->owner = &newBoss;
    this->data = 0;
    this->children.clear();
    this->children.setExpectedSize(numExpectedChildren);
  }
  Expression zero();
  static Expression zeroStatic();
  bool addChildRationalOnTop(const Rational& inputRat);
  bool addChildOnTop(const Expression& inputChild);
  bool addChildAtomOnTop(const std::string& theOperationString);
  bool addChildAtomOnTop(int theOp);
  void getBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList) const;
  bool splitProduct(int numDesiredMultiplicandsLeft, Expression& outputLeftMultiplicand, Expression& outputRightMultiplicand) const;
  void getBaseExponentForm(Expression& outputBase, Expression& outputExponent) const;
  void getMultiplicandsRecursive(List<Expression>& outputAppendList, int depth = 0) const;
  void getMultiplicandsDivisorsRecursive(List<Expression>& outputAppendList, int depth = 0) const;
  void getCoefficientMultiplicandForm(Expression& outputCoeff, Expression& outputNoCoeff) const;
  void getCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff) const;
  bool setChildAtomValue(int childIndex, const std::string& theAtom);
  bool setChildAtomValue(int childIndex, int theAtomValue);
  int size() const {
    return this->children.size;
  }
  bool setChild(int childIndexInMe, const Expression& inputChild);
  bool assignMatrixExpressions(
    const Matrix<Expression>& input,
    Calculator& owner,
    bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
    bool dontReduceTypes
  );
  template<class Coefficient>
  bool assignMatrix(
    const Matrix<Coefficient>& input,
    Calculator& owner,
    ExpressionContext const* inputContext = nullptr,
    bool reduceOneRowToSequenceAndOneByOneToNonMatrix = true
  );
  bool divisionByMeShouldBeWrittenInExponentForm() const;
  bool isCalculatorStatusChanger() const;
  bool isList() const;
  bool isListNElements(int N = - 1) const;
  bool startsWithBuiltInAtom() const;
  bool startsWithFunctionWithComplexRange() const;
  bool startsWithArithmeticOperation() const;
  bool startsWith(int theOp = - 1, int N = - 1) const;

  bool startsWithGivenOperation(const std::string& theOperation, int desiredChildren = - 1) const;
  bool isListStartingWithAtom(int operation = - 1) const;
  bool isListOfTwoAtomsStartingWith(int theOp) const;
  bool isFrozen() const;
  bool isAtomThatFreezesArguments(std::string* outputWhichAtom = nullptr) const;
  bool isAtomWhoseExponentsAreInterpretedAsFunction(std::string* outputWhichAtom = nullptr) const;
  bool isPowerOfAtomWhoseExponentsAreInterpretedAsFunction() const;
  bool isAtomNotInterpretedAsFunction(std::string* outputWhichAtom = nullptr) const;
  bool isMatrix(int* outputNumRows = nullptr, int* outputNumCols = nullptr) const;
  template<class theType>
  bool isMatrixOfType(int* outputNumRows = nullptr, int* outputNumCols = nullptr) const;

  bool isAtom() const;
  bool isAtomUserDefined() const;
  bool isOperationGiven(const std::string& desiredAtom) const;
  bool isOperationGiven(int desiredDataUseMinusOneForAny = - 1) const;
  bool isOperation(int& outputWhichOperationIndex) const;
  bool isOperation(std::string* outputWhichOperation = nullptr) const;

  bool isBuiltInAtom(std::string* outputWhichOperation = nullptr) const;
  bool isGoodForChainRuleFunction(std::string* outputWhichOperation = nullptr) const;
  bool isIntegralFdx(
    Expression* differentialVariable = nullptr,
    Expression* functionToIntegrate = nullptr,
    Expression* integrationSet = nullptr
  ) const;
  bool isIndefiniteIntegralFdx(
    Expression* differentialVariable = nullptr,
    Expression* functionToIntegrate = nullptr,
    Expression* integrationSet = nullptr
  ) const;
  bool isDefiniteIntegralOverIntervalFdx(
    Expression* differentialVariable = nullptr,
    Expression* functionToIntegrate = nullptr,
    Expression* integrationSet = nullptr
  ) const;
  bool isDifferentialOneFormOneVariable(
    Expression* outputDifferentialOfWhat = nullptr,
    Expression* outputCoeffInFrontOfDifferential = nullptr
  ) const;
  bool isKnownFunctionWithComplexRange(std::string* outputWhichOperation = nullptr) const;
  bool isArithmeticOperation(std::string* outputWhichOperation = nullptr) const;
  bool isCacheableExpression() const;
  bool isBuiltInScalar() const;
  bool isElementaryObject() const;
  bool isBuiltInType(std::string* outputWhichOperation = nullptr) const;
  bool isBuiltInType(int* outputWhichType) const;
  bool isIntervalRealLine() const;
  bool isSequenceDoubleButNotTripleNested() const;
  bool isSequenceNElements(int n = - 2) const;
  bool isError(std::string* outputErrorMessage = nullptr) const;
  bool isContext() const;
  bool needsParenthesisForBaseOfExponent() const;
  bool needsParenthesisForMultiplication(FormatExpressions* theFormat = nullptr) const;
  bool needsParenthesisForAddition() const;
  bool needsParenthesisForMultiplicationWhenSittingOnTheRightMost(const Expression* leftNeighbor = nullptr) const;

  int getExpressionTreeSize() const;

  template <class theType>
  bool convertInternally(Expression& output) const;
  template <class theType>
  bool convertsInternally(WithContext<theType>* whichElement = nullptr) const;

  template <class theType>
  bool isOfType() const {
    MacroRegisterFunctionWithName("Expression::isOfType");
    if (this->owner == nullptr) {
      return false;
    }
    if (!this->startsWith(this->getTypeOperation<theType>())) {
      return false;
    }
    if (this->children.size < 2 || !this->getLastChild().isAtom()) {
      return false;
    }
    return true;
  }
  template <class theType>
  bool hasType() const {
    if (this->isOfType<theType>()) {
      return true;
    }
    if (this->isElementaryObject()) {
      return false;
    }
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].hasType<theType>()) {
        return true;
      }
    }
    return false;
  }
  template <class theType>
  bool isOfType(theType* whichElement) const {
    MacroRegisterFunctionWithName("Expression::isOfType");
    if (this->owner == nullptr) {
      return false;
    }
    if (!this->startsWith(this->getTypeOperation<theType>())) {
      return false;
    }
    if (this->children.size < 2 || !this->getLastChild().isAtom()) {
      return false;
    }
    if (whichElement == 0) {
      return true;
    }
    *whichElement = this->getValue<theType>();
    return true;
  }
  template <class theType>
  bool isOfTypeWithContext(WithContext<theType>* whichElement) const;
  template <class theType>
  const theType& getValue() const {
    return this->getValueNonConst<theType>();
  }
  template <class theType>
  theType& getValueNonConst() const;
  template<class theType>
  int getTypeOperation() const;
  template<class theType>
  int addObjectReturnIndex(const theType& inputValue) const;
  // note: the following always returns true:
  template <class theType>
  bool assignValue(const theType& inputValue, Calculator& owner);
  // note: the following always returns true:
  template <class theType>
  bool assignValueWithContext(
    const theType& inputValue,
    const ExpressionContext& theContext,
    Calculator& owner
  );
  template <class BuiltIn>
  bool assignWithContext(
    const WithContext<BuiltIn>& input,
    Calculator& owner
  ) {
    return this->assignValueWithContext(input.content, input.context, owner);
  }
  template <class theType>
  bool addChildValueOnTop(const theType& inputValue) {
    this->checkInitialization();
    Expression tempE;
    tempE.assignValue(inputValue, *this->owner);
    tempE.checkConsistency();
    return this->addChildOnTop(tempE);
  }
  bool setContextAtLeastEqualTo(
    ExpressionContext& inputOutputMinContext, std::stringstream* commentsOnFailure
  );
  bool hasContext() const;
  bool hasNonEmptyContext() const;
  ExpressionContext getContext() const;
  bool getContext(ExpressionContext& output) const;
  static bool mergeContexts(Expression& leftE, Expression& rightE);
  bool mergeContextsMyAruments(Expression& output, std::stringstream* commentsOnFailure) const;
  template <class theType>
  bool mergeContextsMyArumentsAndConvertThem(Expression& output, std::stringstream* commentsOnFailure) const;

  bool containsAsSubExpressionNoBuiltInTypes(const Expression& input) const;
  bool containsAsSubExpressionNoBuiltInTypes(int inputAtom) const;
  bool contextSetDifferentialOperatorVariable(const Expression& thePolyVar, const Expression& theDiffOpVar);
  SemisimpleLieAlgebra* getAmbientSemisimpleLieAlgebraNonConstUseWithCaution() const;
  bool isEqualToZero() const;
  bool isEqualToOne() const;
  bool isEqualToTwo() const;
  bool isEqualToHalf() const;
  bool isEqualToMOne() const;
  bool isKnownToBeNonNegative() const;
  bool isNegativeConstant() const;
  bool makeIdentityMatrixExpressions(int theDim, Calculator& inputBoss);
  bool makeAtom(int input, Calculator& newBoss);
  // TODO(tmilev): rename to MakeOperation
  bool makeAtom(const std::string& atomName, Calculator& newBoss);
  Expression::FunctionAddress GetHandlerFunctionIamNonBoundVar();
  bool makeIntegral(
    Calculator& calculator,
    const Expression& integrationSet,
    const Expression& theFunction,
    const Expression& theVariable
  );
  template<class Coefficient>
  bool makeSum(Calculator& calculator, const LinearCombination<Expression, Coefficient>& summands);
  bool makeSum(Calculator& calculator, const List<Expression>& summands);
  bool makeProduct(Calculator& owner, const List<Expression>& theMultiplicands);
  bool makeProduct(Calculator& owner, const Expression& left, const Expression& right);
  int getNumberOfColumns() const;
  bool makeSequenceCommands(Calculator& owner, List<std::string>& inputKeys, List<Expression>& inputValues);
  bool makeSequenceStatements(Calculator& owner, List<Expression>* inputStatements = nullptr);
  bool makeMatrix(Calculator& owner, Matrix<Expression>* inputMat = nullptr);
  bool makeSequence(Calculator& owner, List<Expression>* inputSequence = nullptr);
  bool makeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right);
  bool makeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex = 2);
  bool makeSqrt(Calculator& owner, const Expression& argument, const Rational& radicalSuperIndex = 2);

  bool makeXOXOdotsOX(Calculator& owner, int theOp, const List<Expression>& theOpands);
  bool makeOXdotsX(Calculator& owner, int theOp, const List<Expression>& input);
  bool makeOX(Calculator& owner, int theOp, const Expression& opArgument);
  bool sequencefy();
  bool isSuitableForSubstitution() const;
  bool isSuitableForRecursion() const;

  void substituteRecursively(const Expression& toBeSubbed, const Expression& toBeSubbedWith);
  void substituteRecursivelyInChildren(const Expression& toBeSubbed, const Expression& toBeSubbedWith);
  void assignXOXToChild(
    int childIndex,
    Calculator& owner,
    int theOp,
    const Expression& left,
    const Expression& right
  ) {
    Expression tempE;
    tempE.makeXOX(owner, theOp, left, right);
    this->setChild(childIndex, tempE);
  }
  std::string lispify() const;
  bool toStringData(std::stringstream& out, FormatExpressions* theFormat = nullptr) const;

  std::string toStringSemiFull() const;
  std::string toStringFull() const;
  std::string toStringAllSlidersInExpression() const;
  std::string toUTF8String(FormatExpressions* theFormat = nullptr) const;

  template <class builtIn>
  static bool toStringBuiltIn(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* theFormat
  );

  bool toStringWithAtomHandler(std::stringstream& out, FormatExpressions* theFormat) const;
  bool toStringWithCompositeHandler(std::stringstream& out, FormatExpressions* theFormat) const;

  static bool toStringLnAbsoluteInsteadOfLogarithm(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDifferential(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDifferential2(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDifferential3(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDifferentiate(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDefine(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIsDenotedBy(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLogBase(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringQuote(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDefineConditional(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringTensor(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIn(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringAnd(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringBinom(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringUnderscore(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringSetMinus(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLimitBoundary(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringCrossProduct(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringAbsoluteValue(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringBind(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringMod(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringPlus(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringMatrix(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringSequence(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLieBracket(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringSumOrIntegral(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringGreaterThan(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringGreaterThanOrEqualTo(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLessThanOrEqualTo(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDirectSum(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLimitProcess(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLessThan(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringLimit(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringTimes(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringSqrt(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringSqrt2(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringSqrt3(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringOr(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIntervalOpen(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIntervalLeftClosed(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIntervalRightClosed(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIntervalClosed(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringDivide(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);

  static bool toStringPower(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringFactorial(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringUnion(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringMinus(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringMinus2(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringMinus3(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringIntersection(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringUnionNoRepetition(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringEqualEqual(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringEqualEqualEqual(const Expression& input, std::stringstream& out, FormatExpressions* theFormat);
  static bool toStringError(
    const Expression& input, std::stringstream& out, FormatExpressions* theFormat
  );
  void toStringOpMultiplicative(std::stringstream& out, const std::string& operation, FormatExpressions* theFormat) const;

  std::string toStringTreeHtml(int depth) const;
  std::string toString(
    FormatExpressions* format = nullptr,
    Expression* startingExpression = nullptr,
    bool unfoldCommandEnclosures = true,
    JSData* outputJS = nullptr
  ) const;
  bool toStringGeneral(
    std::stringstream& out,
    FormatExpressions* theFormat
  ) const;
  std::string toStringWithStartingExpression(
    FormatExpressions* theFormat,
    Expression* startingExpression,
    std::stringstream& out,
    JSData* outputJS
  ) const;
  bool toStringEndStatement(
    std::stringstream& out,
    Expression* startingExpression,
    JSData* outputJS,
    FormatExpressions* theFormat
  ) const;
  bool requiresNoMathTags() const;
  static unsigned int hashFunction(const Expression& input);
  unsigned int hashFunction() const;
  Expression(): flagDeallocated(false) {
    this->reset();
  }
  Expression(int x): flagDeallocated(false) {
    MacroRegisterFunctionWithName("Expression::Expression(int)");
    this->reset();
    this->data = x;
  }
  const Expression& getLastChild() const {
    return (*this)[this->children.size - 1];
  }
  bool makeError(const std::string& theError, Calculator& owner);
  Expression(const Expression& other): flagDeallocated(false) {
    this->operator=(other);
  }
  Expression(Calculator& inputBoss): flagDeallocated(false) {
    this->reset(inputBoss);
  }
  ~Expression() {
    this->flagDeallocated = true;
  }
  bool checkInitialization() const;
  bool checkInitializationRecursively() const;
  bool checkConsistency() const;
  bool checkConsistencyRecursively() const;
  bool isIntegerFittingInInt(int* whichInteger = nullptr) const;
  bool isSmallInteger(int* whichInteger = nullptr) const;
  bool isInteger(LargeInteger* whichInteger = nullptr) const;
  bool isIntegerNonNegative(LargeIntegerUnsigned* whichInteger = nullptr) const;
  bool isRational(Rational* whichRational = nullptr) const;
  bool isAlgebraicRadical() const;
  bool isConstantNumber() const;
  bool isPositiveNumber() const;
  bool evaluatesToDoubleInRange(
    const std::string& varName,
    double lowBound,
    double highBound,
    int numPoints,
    double* outputYmin = nullptr,
    double* outputYmax = nullptr,
    Vectors<double>* outputPoints = nullptr
  ) const;
  bool evaluatesToDouble(double* whichDouble = nullptr) const;
  bool getExpressionLeafs(HashedList<Expression>& outputAccumulateLeafs) const;
  bool getFreeVariables(HashedList<Expression>& outputAccumulateFreeVariables, bool excludeNamedConstants) const;
  bool allowedAsFreeVariableAtom(const std::string& input) const;
  bool getBoundVariables(HashedList<Expression>& outputAccumulateBoundVariables) const;
  bool evaluatesToDoubleUnderSubstitutions(
    const HashedList<Expression>& knownEs, const List<double>& valuesKnownEs, double* whichDouble = nullptr
  ) const;
  bool hasBoundVariables() const;
  bool hasInputBoxVariables(HashedList<std::string, MathRoutines::hashString>* boxNames = nullptr) const;
  bool isMeltable(int* numResultingChildren = nullptr) const;
  bool areEqualExcludingChildren(const Expression& other) const {
    return this->owner == other.owner &&
    this->data == other.data &&
    this->children.size == other.children.size;
  }
  // The following function creates an expression by parsing a calculator-like string.
  // The purpose of this function is to reduce the number of lines needed to create an Expression using C++.
  // Consider creating the expression f{}{{a}}={{a}}+ 1; f{}b;
  // We would need to create a large expression tree, so many calls of Expression::makeXOX.
  // Instead, we can simply parse the expression from a string.
  // The inputExpressions give us the ability to specify substitutions
  bool assignStringParsed(
    const std::string& input,
    MapList<std::string, Expression, MathRoutines::hashString>* substitutions,
    Calculator& owner
  );
  const Expression& operator[](int n) const;
  void operator=(const Expression& other) {
    this->data = other.data;
    this->children = other.children;
    this->owner = other.owner;
  }
  void operator=(const Rational& other) {
    MacroRegisterFunctionWithName("Expression::operator=(Rational)");
    this->checkInitialization();
    this->assignValue(other, *this->owner);
  }
  void operator=(int other) {
    MacroRegisterFunctionWithName("Expression::operator=(int)");
    this->checkInitialization();
    this->assignValue(Rational(other), *this->owner);
  }
  void operator/=(const Expression& other);
  void operator+=(const Expression& other);
  void operator-=(const Expression& other);
  Expression operator+(int other);
  Expression operator-(int other);
  Expression operator*(int other);
  Expression operator/(int other);
  Expression operator+(const Expression& other);
  Expression operator-(const Expression& other);
  Expression operator*(const Expression& other);
  Expression operator/(const Expression& other);
  void operator*=(const Expression& other);
  // Rational getConstantTerm() const;
  bool isEqualToMathematically(const Expression& other) const;
  bool operator==(int other) const;
  bool operator==(const Expression& other) const;
  bool operator==(const std::string& other) const;
  bool operator!=(const std::string& other) const {
    return !this->operator==(other);
  }
  bool operator!=(const Expression& other) const {
    return ! (*this == other);
  }
  // void operator=(const Expression& other);
  bool operator>(const Expression& other) const;
  bool greaterThanNoCoefficient(const Expression& other) const;
  void substituteRecursively(MapList<Expression, Expression>& theSubs);
  void substituteRecursivelyInChildren(MapList<Expression, Expression>& theSubs);
  class Test {
  public:
    static bool all();
    static bool toStringTestRecode(Calculator& owner);
    static bool toStringTestRecodeOnce(const std::string& inputHardCodedMustParse, Calculator& owner);
    static bool isUserDefinedAtomOnce(Calculator& owner, const std::string& input, bool isUserDefinedAtom);
    static bool isUserDefinedAtom(Calculator& owner);
  };
};

class ExpressionContext {
private:
  HashedList<Expression> variables;
  List<Expression> differentialOperatorVariables;
  LargeIntegerUnsigned defaultModulus;
  bool mergeSemisimpleLieAlgebraContexts(
    const ExpressionContext& other,
    ExpressionContext& outputContext
  );
  bool mergeModuli(
    const ExpressionContext& other,
    ExpressionContext& outputContext
  );
  bool mergeVariables(
    const ExpressionContext& other,
    ExpressionContext& outputContext
  );
  bool mergeDifferentialOperators(
    const ExpressionContext& other,
    ExpressionContext& outputContext
  );
  bool mergeDifferentialOperatorsOnce(
    Selection &differentialOperatorVariablesFound,
    ExpressionContext& outputContext
  ) const;
public:
  Calculator* owner;
  int indexAmbientSemisimpleLieAlgebra;

  ExpressionContext(Calculator& inputOwner);
  ExpressionContext();
  const HashedList<Expression>& getVariables() {
    return this->variables;
  }
  bool checkInitialization() const;
  bool mergeContexts(
    const ExpressionContext& other,
    ExpressionContext& outputContext
  );
  void initialize(Calculator& inputOwner);
  void makeOneVariable(const Expression& inputVariable);
  void makeOneVariableOneDifferentialOperator(
    const Expression& inputVariable,
    const Expression &inputDifferentialOperatorVariable
  );
  void makeOneVariableFromString(const std::string& inputVariable);
  void makeOneVariableCreate(const std::string& variable);
  void addVariable(const Expression& inputVariable);
  bool setVariables(const List<Expression>& inputVariables);
  bool setVariablesFromStrings(const List<std::string>& inputVariables);
  bool setAmbientSemisimpleLieAlgebra(SemisimpleLieAlgebra& input);
  void setIndexAmbientSemisimpleLieAlgebra(int index);
  void setDefaultModulus(const LargeIntegerUnsigned& input);
  LargeIntegerUnsigned getDefaultModulus();
  void getFormat(FormatExpressions& output) const;
  FormatExpressions getFormat() const;
  Expression getVariable(int variableIndex) const;
  bool isEmpty();
  SemisimpleLieAlgebra* getAmbientSemisimpleLieAlgebra() const;
  template <class Coefficient>
  bool polynomialSubstitution(
    const ExpressionContext& largerContext,
    PolynomialSubstitution<Coefficient>& output,
    const Coefficient& one
  ) const;
  template <class Coefficient>
  void polynomialSubstitutionNoFailure(
    const ExpressionContext& largerContext,
    PolynomialSubstitution<Coefficient>& output,
    const Coefficient& one
  ) const;
  template <class Coefficient>
  bool polynomialAndWeylAlgebraSubstitution(
    const ExpressionContext& largerContext,
    PolynomialSubstitution<Coefficient>& outputPolynomialPart,
    PolynomialSubstitution<Coefficient>& outputDifferentialPart
  ) const;
  template <class Coefficient>
  void polynomialAndWeylAlgebraSubstitutionNoFailure(
    const ExpressionContext& largerContext,
    PolynomialSubstitution<Coefficient>& outputPolynomialPart,
    PolynomialSubstitution<Coefficient>& outputDifferentialPart
  ) const;
  bool operator==(const ExpressionContext& other) const;
  bool hasAtomicUserDefinedVariables() const;
  int numberOfVariables() const;
  Expression toExpression() const;
  bool fromExpression(const Expression& input);
  bool fromExpressionOneContext(const Expression& input);
  bool fromExpressionPolynomialVariables(const Expression& input);
  bool fromExpressionDifferentialOperatorVariables(const Expression& input);
  bool fromExpressionSemisimpleLieAlgebra(const Expression& input);
  bool fromExpressionDefaultModulus(const Expression& input);
  Expression toExpressionDefaultModulus() const;
  Expression toExpressionSemisimpleLieAlgebra() const;
  Expression toExpressionPolynomialVariables() const;
  Expression toExpressionDifferntialOperators() const;
  std::string toString() const;
};

template <class BuiltIn>
class WithContext {
  bool extendContext(ExpressionContext& newContext, std::stringstream* commentsOnFailure);
  bool extendContextTrivially(ExpressionContext& newContext, std::stringstream* commentsOnFailure) {
    (void) commentsOnFailure;
    this->context = newContext;
    return true;
  }
public:
  ExpressionContext context;
  BuiltIn content;
  std::string toStringContentWithFormat() {
    FormatExpressions theFormat;
    this->context.getFormat(theFormat);
    return this->content.toString(&theFormat);
  }
  WithContext() {
  }
  static bool mergeContexts(
    WithContext<BuiltIn>& leftOutput,
    WithContext<BuiltIn>& rightOutput,
    std::stringstream* commentsOnFailure
  );
  bool setContextAtLeast(ExpressionContext& inputOutputContext, std::stringstream* commentsOnFailure);
  bool setContextAndSerialize(
    ExpressionContext& inputOutputContext,
    Expression& output,
    std::stringstream* commentsOnFailure
  );
};

class Function {
  public:
  Calculator* owner;
  Expression theArgumentTypes;
  std::string theDescription;
  std::string theExample;
  std::string calculatorIdentifier;
  std::string additionalIdentifier;
  int indexOperation;
  int indexInOperationHandlers;
  int indexOperationParentThatBansHandler;

  class Options {
  public:
    bool flagIsCompositeHandler;
    bool flagIsInner;
    bool flagMayActOnBoundVars;
    bool visible;
    bool flagIsExperimental;
    bool disabledByUser;
    bool disabledByUserDefault;
    bool freezesArguments;
    // The dontTestAutomatically
    // indicates that the function should not be auto-tested.
    // This functions that are not auto-tested include the following.
    // - The automated test trigger function.
    // - Calculator functions that do not have reproducible results,
    //   such as, for example, the GenerateRandomPrime function.
    //   Note such C++ functions are not ``functions''
    //   in the mathematical sense.
    // - Functions that crash the calculator deliberately,
    //   such as the Crash function.
    // - Functions that run extra slowly.
    // - Functions that are considered experimental / in development.
    bool dontTestAutomatically;
    bool adminOnly;
    void reset();
    static Options adminNoTest();
    static Options innerInvisible();
    static Options standard();
    static Options compositeStandard();
    static Options innerFreezesArguments();
    static Options innerAdminNoTestInvisibleOffByDefault();
    static Options innerInvisibleExperimental();
    static Options innerAdminNoTestExperimental();
    static Options innerAdminNoTest();
    static Options experimental();
    static Options invisibleNoTest();
    static Options innerNoTest();
    static Options innerNoTestExperimental();
    static Options outerOffByDefault();
    Options();
  };
  Options options;
  Expression::FunctionAddress theFunction;

  std::string toStringShort() const;
  std::string toStringSummary() const;
  std::string toStringFull() const;
  JSData toJSON() const;
  bool shouldBeApplied(int parentOpIfAvailable);
  bool operator==(const Function& other) const {
    return
    this->theArgumentTypes == other.theArgumentTypes &&
    this->theFunction == other.theFunction &&
    this->options.flagIsInner == other.options.flagIsInner;
  }
  void reset(Calculator& owner) {
    this->theArgumentTypes.reset(owner);
    this->owner = &owner;
    this->resetExceptOwner();
  }
  void resetExceptOwner() {
    this->theFunction = nullptr;
    this->indexInOperationHandlers = - 1;
    this->indexOperation = - 1;
    this->indexOperationParentThatBansHandler = - 1;
    this->options.reset();
  }
  bool inputFitsMyInnerType(const Expression& input);
  Function();
  Function(
    Calculator& inputOwner,
    int inputIndexOperation,
    const Expression::FunctionAddress& functionPointer,
    Expression* inputArgTypes,
    const std::string& description,
    const std::string& inputExample,
    const std::string& inputAdditionalIndentifier,
    const std::string& inputCalculatorIdentifier,
    const Function::Options& inputOptions,
    int inputIndexParentThatBansHandler = - 1
  );
  static unsigned int hashFunction(const Function& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this->theFunction));
  }
  bool apply(Calculator& calculator, const Expression& input, Expression& output, int opIndexParentIfAvailable, Function** outputHandler);
  bool checkConsistency() const;
};

class SyntacticElement {
  public:
  int controlIndex;
  int numNonBoundVariablesInherited;
  int numBoundVariablesInherited;
  Expression theData;
  List<Expression> dataList;
  std::string errorString;
  std::string toStringHumanReadable(Calculator& owner, bool includeLispifiedExpressions) const;
  std::string toStringSyntaxRole(const Calculator& owner) const;
  SyntacticElement() {
    this->controlIndex = 0; //controlIndex = 0 *MUST* point to the empty control sequence.
    this->errorString = "";
    this->numNonBoundVariablesInherited = - 1; // - 1 stands for unknown
    this->numBoundVariablesInherited = - 1; // - 1 stands for unknown
  }
  std::string getIntegerStringCrashIfNot(Calculator& owner);
  bool isCommandEnclosure() const;
};

class PlotObject {
private:
  JSData coordinateFunction(int index);
  JSData manifoldImmersionFunctionsJS();
  void writeColorWidthSegments(JSData& output);
  void writeColorLineWidth(JSData& output);
  JSData functionFromString(const std::string& input);
public:
  struct Labels {
  public:
    static std::string points;
    static std::string functionLabel;
    static std::string coordinateFunctions;
    static std::string left;
    static std::string right;
    static std::string numberOfSegments;
    static std::string color;
    static std::string lineWidth;
    static std::string manifoldImmersion;
    static std::string variableRange;
    static std::string segmentRange;
    static std::string defaultLength;
    static std::string plotType;
    static std::string viewWindow;
    static std::string body;
    static std::string arguments;
  };

  std::string thePlotString;
  std::string fillStyle;
  std::string thePlotStringWithHtml;
  double xLow;
  double xHigh;
  double yLow;
  double yHigh;
  double paramLow;
  double paramHigh;
  double lineWidth;
  int dimension;
  int colorRGB;
  int colorFillRGB;
  bool leftBoundaryIsMinusInfinity;
  bool rightBoundaryIsMinusInfinity;
  std::string defaultLengthJS;
  std::string colorJS;
  std::string colorFillJS;
  std::string colorUV;
  std::string colorVU;
  std::string lineWidthJS;
  List<std::string> numSegmenTsJS;
  Matrix<Expression> thePointS;
  Matrix<std::string> thePointsJS;
  Vectors<double> thePointsDouble;
  List<Vectors<double> > theRectangles;
  // Each rectangle is a list of two 2-dim vectors.
  // The first vector gives the (x, y) - coordinates
  // of the lower left corner of the rectangle.
  // The Second vector gives the (width, height) of the rectangle.
  std::string plotType;

  Expression manifoldImmersion;
  std::string manifoldImmersionJS;
  List<Expression> coordinateFunctionsE;
  List<std::string> coordinateFunctionsJS;
  HashedList<Expression> variablesInPlay;
  List<List<std::string> > theVarRangesJS;
  Expression leftPtE;
  Expression rightPtE;
  Expression paramLowE;
  Expression paramHighE;

  Expression numSegmentsE;
  List<std::string> variablesInPlayJS;
  std::string leftPtJS;
  std::string rightPtJS;
  std::string paramLowJS;
  std::string paramHighJS;
  ////////////////
  std::string getJavascriptSurfaceImmersion(
    std::string& outputSurfaceInstantiationJS,
    const std::string& canvasName,
    int& funCounter
  );
  std::string getJavascriptCurveImmersionIn3d(
    std::string& outputCurveInstantiationJS, const std::string& canvasName, int& funCounter
  );
  std::string toStringDebug();
  void computeYBounds();
  std::string toStringPointsList();
  std::string getPlotStringFromFunctionStringAndRanges(
    bool useHtml,
    const std::string& functionStringPostfixNotation,
    const std::string& functionStringCalculatorFormat,
    double lowerBound,
    double upperBound
  );
  JSData toJSON();
  JSData toJSON2dDrawFunction();
  JSData toJSONParametricCurveInTwoDimensions();
  JSData toJSONPoints();
  JSData toJSONDirectionFieldInTwoDimensions(  );
  PlotObject();
  bool operator==(const PlotObject& other) const;
};

//the following class is meant to use to draw plots for calculus students.
class Plot {
private:
  std::string canvasNamE;
  static JSData getCoordinateSystem();
  static JSData getComputeViewWindow();
  JSData getSetViewWindow();
public:
  struct Labels {
  public:
    static std::string canvasName;
    static std::string controlsName;
    static std::string messagesName;
    static std::string graphicsType;
  };
  List<PlotObject> plotObjects;
  HashedList<std::string, MathRoutines::hashString> boxesThatUpdateMe;
  double theLowerBoundAxes;
  double theUpperBoundAxes;
  double lowBoundY;
  double highBoundY;
  int desiredHtmlHeightInPixels;
  int desiredHtmlWidthInPixels;
  int defaultLineColor;
  bool flagIncludeExtraHtmlDescriptions;
  bool flagPlotShowJavascriptOnly;
  bool flagDivAlreadyDisplayed;
  bool flagIncludeCoordinateSystem;
  int dimension;
  int priorityViewRectangle; // 0 or less: compute the view Window. If this quantity is greater than zero,
  int priorityWindow; // 0 or less: compute the view Window. If this quantity is greater than zero,
  // the user-given bounding box will overwrite any computations.
  // When adding two plots with positive viewing window priorities, the window with the larger priority is used.
  // If the priorities are equal, the windows are combined to the smallest window that fits both.
  int priorityCanvasName; // same as priorityViewWindow but with respect to canvas names.
  void setCanvasName(const std::string& inputName);
  std::string getCanvasName() const;

  std::string toStringDebug();
  std::string getPlotHtml(Calculator& owner);
  void computeCanvasNameIfNecessary(int& canvasCounter);
  std::string getPlotHtml3d(Calculator& owner);
  std::string getPlotHtml2d(Calculator& owner);
  std::string getPlotStringAddLatexCommands(bool useHtml);
  bool isOKVector(const Vector<double>& input);
  std::string commonCanvasSetup();
  Plot();
  void computeAxesAndBoundingBox3d();
  void computeAxesAndBoundingBox();
  void operator+=(const Plot& other);
  void operator+=(const PlotObject& other);
  bool operator==(const Plot& other) const;
};

class InputBox {
public:
  friend std::ostream& operator<<(std::ostream& output, const InputBox& input) {
    output << "name: " << input.name << ", ...";
    return output;
  }
  std::string name;
  Expression value;
  Expression min;
  Expression max;
  Expression step;
  std::string getSliderName() const;
  std::string getUserInputBox() const;
};

class ObjectContainer {
  // Following are containers for data structures that are implemented in C++.
  // These objects are dynamically allocated and used by the calculator as requested
  // by various predefined function handlers.
public:
  HashedListReferences<ElementWeylGroup> theWeylGroupElements;
  MapReferences<DynkinType, SemisimpleLieAlgebra> semisimpleLieAlgebras;
  ListReferences<SemisimpleLieAlgebra*> semisimpleLieAlgebraPointers;
  MapReferences<DynkinType, SemisimpleSubalgebras> theSSSubalgebraS;
  HashedListReferences<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > weylGroupRepresentations;
  HashedListReferences<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > weylGroupVirtualRepresentations;
  ListReferences<ModuleSSalgebra<RationalFunction<Rational> > > theCategoryOmodules;
  ListReferences<SlTwoSubalgebras> theSltwoSAs;
  HashedListReferences<ElementEllipticCurve<ElementZmodP> > ellipticCurveElementsZmodP;
  HashedListReferences<ElementEllipticCurve<Rational> > ellipticCurveElementsRational;
  HashedListReferences<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > > theTensorElts;
  HashedListReferences<Polynomial<Rational> > polynomialsRational;
  HashedListReferences<Polynomial<AlgebraicNumber> > polynomialsAlgebraic;
  HashedListReferences<Polynomial<ElementZmodP> > polynomialsModular;
  HashedListReferences<PolynomialModuloPolynomial<ElementZmodP> > polynomialQuotientsModular;

  HashedListReferences<ElementWeylAlgebra<Rational> > theWeylAlgebraElements;
  HashedListReferences<ElementUniversalEnveloping<RationalFunction<Rational> > > theUEs;
  HashedListReferences<RationalFunction<Rational> > rationalFunctions;
  HashedListReferences<RationalFunction<AlgebraicNumber> > rationalFunctionsAlgebraic;
  HashedListReferences<RationalFunction<ElementZmodP> > rationalFunctionsModular;
  HashedListReferences<Rational> theRationals;
  HashedListReferences<CharacterSemisimpleLieAlgebraModule<Rational> > theCharsSSLieAlgFD;
  HashedListReferences<double, MathRoutines::hashDouble> theDoubles;
  HashedListReferences<std::string, MathRoutines::hashString> theStrings;
  HashedListReferences<JSData> jsonObjects;
  HashedListReferences<std::string, MathRoutines::hashString> expressionNotation;
  HashedListReferences<Expression> expressionWithNotation;
  HashedListReferences<Expression> constraints;
  HashedListReferences<LittelmannPath> lakshmibaiSeshadriPaths;
  HashedListReferences<MatrixTensor<Rational> > theMatTensorRats;
  HashedListReferences<ElementZmodP> theEltsModP;
  HashedListReferences<Weight<Rational> > theWeights;
  HashedListReferences<Weight<Polynomial<Rational> > > theWeightsPoly;
  ListReferences<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational > > hyperoctahedralRepresentations;
  ListReferences<Plot> thePlots;
  List<bool> userInputBoxSliderDisplayed;
  MapReferences<std::string, InputBox, MathRoutines::hashString> userInputTextBoxesWithValues;
  MapReferences<std::string, std::string, MathRoutines::hashString> graphicsScripts;
  AlgebraicClosureRationals theAlgebraicClosure;
  HashedList<AlgebraicNumber> theAlgebraicNumbers;
  HashedListReferences<ElementHyperoctahedralGroupR2> theElementsHyperOctGroup;
  ListReferences<HyperoctahedralGroupData> theHyperOctahedralGroups;
  HashedListReferences<MonomialTensor<int, HashFunctions::hashFunction> > theLittelmannOperators;
  WeylGroupData& getWeylGroupDataCreateIfNotPresent(const DynkinType& input);
  SemisimpleLieAlgebra& getLieAlgebraCreateIfNotPresent(const DynkinType& input);
  SemisimpleSubalgebras& getSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input);
  UnsecurePseudoRandomGenerator pseudoRandom;
  int canvasPlotCounter;
  void reset();
  void resetSliders();
  void resetPlots();
  bool checkConsistencyAfterReset();
  std::string toString();
  std::string toStringJavascriptForUserInputBoxes();
};

class ExpressionTripleCrunchers {
public:
  int operation;
  int leftType;
  int rightType;
  bool operator==(const ExpressionTripleCrunchers& other) const {
    return this->leftType == other.leftType && this->rightType == other.rightType && this->operation == other.operation;
  }
  void operator=(const ExpressionTripleCrunchers& other) {
    this->leftType = other.leftType;
    this->rightType = other.rightType;
    this->operation = other.operation;
  }
  ExpressionTripleCrunchers(): operation(- 1), leftType(- 1), rightType(- 1) {
  }
  ExpressionTripleCrunchers(
    int inputOperation, int inputLeft, int inputRight
  ): operation(inputOperation), leftType(inputLeft), rightType(inputRight) {

  }
  static unsigned int hashFunction(const ExpressionTripleCrunchers& input) {
    return
      static_cast<unsigned int>(input.leftType) * someRandomPrimes[0] +
      static_cast<unsigned int>(input.rightType) * someRandomPrimes[1] +
      static_cast<unsigned int>(input.operation) * someRandomPrimes[2];
  }
};

class StateMaintainerCalculator {
public:
  Calculator* owner;
  int startingRuleStackIndex;
  int startingRuleStackSize;
  StateMaintainerCalculator(Calculator& inputBoss);
  void addRule(const Expression& theRule);
  ~StateMaintainerCalculator();
};

class Calculator {
  template<typename anyType>
  friend Calculator& operator<<(Calculator& output, const anyType& any) {
    output.comments << any;
    return output;
  }
public:
  class OperationHandlers {
  public:
    std::string atom;
    List<Function> handlers;
    List<Function> compositeHandlers;
    bool flagDeallocated;
    OperationHandlers();
    ~OperationHandlers();
    List<Function> mergeHandlers();
    bool checkConsistency();
    JSData toJSON();
    std::string toStringRuleStatusUser();
  };
  class Atoms {
  public:
    static std::string commandEnclosure;
    static std::string setInputBox;
    static std::string setRandomSeed;
    static std::string sort;
    static std::string transpose;
  };

  // Initialization mode for the calculator.
  // Allows to avoid bootstrapping a number of
  // functions/operations.
  enum Mode {
    // Run in full scientific mode;
    // initialize all scientific functions.
    full,
    // Initialize only the functions required for
    // basic mathematics. Integrals and
    // commutative algebra are included but not
    // representation theory, Lie theory and other
    // advanced functions
    educational
  };

  Mode mode;

  // Operations parametrize the expression elements.
  // Operations are the labels of the atom nodes of the expression tree.
  MapReferences<std::string, MemorySaving<OperationHandlers>, HashFunctions::hashFunction> operations;

  HashedList<std::string, MathRoutines::hashString> atomsThatAllowCommutingOfCompositesStartingWithThem;
  HashedList<std::string, MathRoutines::hashString> atomsNotAllowingChainRule;
  HashedList<std::string, MathRoutines::hashString> builtInTypes;
  HashedList<std::string, MathRoutines::hashString> arithmeticOperations;
  HashedList<std::string, MathRoutines::hashString> knownOperationsInterpretedAsFunctionsMultiplicatively;
  HashedList<std::string, MathRoutines::hashString> knownFunctionsWithComplexRange;
  HashedList<std::string, MathRoutines::hashString> atomsThatFreezeArguments;
  HashedList<std::string, MathRoutines::hashString> atomsWhoseExponentsAreInterpretedAsFunctions;
  HashedList<std::string, MathRoutines::hashString> atomsNotInterpretedAsFunctions;
  HashedList<std::string, MathRoutines::hashString> atomsThatMustNotBeCached;
  HashedList<std::string, MathRoutines::hashString> autoCompleteKeyWords;
  HashedList<std::string, MathRoutines::hashString> stringsThatSplitIfFollowedByDigit;

  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction> toStringHandlersAtoms;
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction> toStringHandlersComposite;
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction> toStringDataHandlers;

  MapList<std::string, List<std::string>, MathRoutines::hashString> predefinedWordSplits;
  class NamedRuleLocation {
  public:
    // Operation for which the named rule was registered.
    // Since each rule name can be registered only once,
    // this is unique.
    std::string containerOperation;
    bool isComposite;
    int index;
    NamedRuleLocation();
  };
  MapList<std::string, NamedRuleLocation, MathRoutines::hashString> namedRules;

  // Calculator functions have as arguments two expressions passed by reference,
  // const Expression& input and Expression& output. Calculator functions
  // return bool. It is forbidden to pass the same object as input and output.
  // If a calculator function returns false this
  // means that the calculator failed to evaluate the
  // function. If that is the case, the value of output is not specified and
  // *MUST NOT* be used in the calling function.
  // If a function returns true this means that output contains the result of the function.
  // Note that the output of a function may be of type Error. Error results come, like any other
  // result, with a true return from the function.
  //-----------------------------------------------
  // In addition, built-in functions are split into two flavors:
  // inner functions (or just "functions")
  // and outer functions (or "laws").
  // The only difference between inner functions and outer functions is the
  // way they are applied when the calculator reduces an expression.
  //
  // Suppose the calculator is reducing Expression X.
  // 1. Outer functions ("laws").
  // 1.1. Let X be expression whose first child is an atom equal to the name of the outer function.
  // 1.2  Call the outer function with input argument equal to X.
  // 1.3. If the outer function returns true but the output argument is identically equal to
  //      X, nothing is done (the action of the outer function is ignored).
  // 1.4. If an outer function returns true and the output argument is different from X,
  //      X is replaced by this output.
  // 2. Inner functions ("functions").
  // 2.1. Let X be expression whose first child is an atom equal to the name of the inner function. We define Argument as follows.
  // 2.1.1. If X has two children, Argument is set to the second child of X.
  // 2.1.2. If X does not have two children, Argument is set to be equal to the entire X.
  // 2.2. The inner function is called with input argument equal to Argument.
  // 2.3. If the inner function returns true, X is substituted with
  //      the output argument of the inner function, else nothing is done.
  //
  // As explained above, the distinction between inner functions and outer functions
  // is only practical. The notions of inner and outer functions do not apply to user-defined
  // substitution rules entered via the calculator. User-defined substitution rules are
  // processed like outer functions, with the
  // major difference that even if their output coincides
  // with their input the substitution is carried out, resulting in an infinite cycle.
  // Here, by ``infinite cycle'' we either mean a 100% CPU run until the timeout& algebraic
  // safety kicks in, or error interception with a ``detected substitution cycle'' or
  // similar error message.
  //
  //----------------------------------------------------------


  // Control sequences parametrize the syntactical elements
  HashedList<std::string, MathRoutines::hashString> controlSequences;

  HashedList<ExpressionTripleCrunchers> theCruncherIds;
  List<Function> theCruncherS;

  HashedList<Expression> knownDoubleConstants;
  List<double> knownDoubleConstantValues;

  int maximumRecursionDepth;
  int recursionDepth;
  int depthRecursionReached;
  int maximumAlgebraicTransformationsPerExpression;
  int maximumLatexChars;
  int maximumCachedExpressionPerRuleStack;
  int maximumRuleStacksCached;
  int ruleStackCacheIndex;
  int numberExpectedExpressionsAtInitialization;
  class EvaluationStatistics {
  public:
    int expressionsEvaluated;
    int callsSinceReport;
    int maximumCallsBeforeReportGeneration;
    int totalSubstitutions;
    int numberOfListsStart;
    int numberListResizesStart;
    int numberHashResizesStart;
    long long int numberOfSmallAdditionsStart;
    long long int numberOfSmallMultiplicationsStart;
    long long int numberOfSmallGreatestCommonDivisorsStart;
    long long int numberOfLargeAdditionsStart;
    long long int numberOfLargeMultiplicationsStart;
    long long int numberOfLargeGreatestCommonDivisorsStart;
    int64_t millisecondsLastLog;
    int64_t startTimeEvaluationMilliseconds;
    EvaluationStatistics();
    void initialize();
    void reset();
  };
  EvaluationStatistics statistics;
  ///////////////////////////////////////////////////////////////////////////
  bool flagAbortComputationASAP;
  bool flagTimeLimitErrorDetected;
  bool flagFirstErrorEncountered;
  bool flagMaxRecursionErrorEncountered;
  bool flagMaxTransformationsErrorEncountered;
  bool flagNewContextNeeded;

  bool flagUsePredefinedWordSplits;

  bool flagPlotShowJavascriptOnly;
  bool flagPlotNoControls;

  bool flagLogSyntaxRules;
  bool flagUseLnInsteadOfLog;
  bool flagUseLnAbsInsteadOfLogForIntegrationNotation;
  bool flagLogEvaluation;
  bool flagUseNumberColors;
  bool flagLogRules;
  bool flagUseBracketsForIntervals;
  bool flagLogCache;
  bool flagLogpatternMatching;
  bool flagLogFullTreeCrunching;
  bool flagHideLHS;
  bool flagHidePolynomialBuiltInTypeIndicator;
  bool flagDisplayFullExpressionTree;
  bool flagUseFracInRationalLaTeX;
  bool flagUseHtml;
  bool flagDisplayContext;
  bool flagHasGraphics;
  bool flagWriteLatexPlots;

  bool flagNoApproximations;

  bool flagForkingprocessAllowed;

  int totalPatternMatchesPerformed;
  int numberOfPredefinedAtoms;
  int numEmptyTokensStart;
  Expression programExpression;
  int counterInSyntacticSoup;
  List<SyntacticElement> syntacticSoup;
  List<SyntacticElement> syntacticStack;

  List<SyntacticElement>* currrentSyntacticSoup;
  List<SyntacticElement>* currentSyntacticStack;

  HashedList<Expression> cachedExpressions;
  List<Expression> imagesCachedExpressions;
  ////
  HashedList<Expression> evaluatedExpressionsStack;
  HashedList<int, HashFunctions::hashFunction> nonBoundVariablesInContext;
  HashedList<int, HashFunctions::hashFunction> boundVariablesInContext;

  Expression ruleStack;
  HashedList<Expression> cachedRuleStacks;

  HashedListReferences<Expression> allChildExpressions;
  List<unsigned int> allChildExpressionHashes;

  std::string syntaxErrors;
  List<std::string> evaluationErrors;

  // std::string inputStringRawestOfTheRaw;
  std::string inputString;
  std::string outputString;
  JSData outputJS;

  std::string outputCommentsString;
  std::string parsingLog;
  ObjectContainer objectContainer;

  std::string javaScriptDisplayingIndicator;
  int numOutputFileS;
  std::string userLabel;
  // List<std::string> logEvaluationSteps;
  std::stringstream comments;
  std::stringstream errorsPublic;
  FormatExpressions formatVisibleStrings;
  std::string toStringRuleStatusUser();
  std::string toString();
  JSData toJSONPerformance();
  Expression getNewBoundVariable();
  Expression getNewAtom();
  void computeAutoCompleteKeyWords();
  void writeAutoCompleteKeyWordsToFile();
  std::string toStringOutputAndSpecials();
  class Examples {
    std::string toStringOneOperationHandler(
      const std::string& escapedAtom,
      bool isComposite,
      const Function& function
    );
    std::string escape(const std::string& atom);
  public:
    Calculator* owner;
    HashedList<std::string, HashFunctions::hashFunction> toBeEscaped;
    // Generates a JSON with all functions and their documentation.
    JSData toJSONFunctionHandlers();
    // Writes a readme file in folder examples/Readme.md.
    bool writeExamplesReadme();
    std::string getExamplesReadmeFragment();
    Examples();
    class Test {
    public:
      static bool compose();
      static bool all();
    };
  };
  Examples examples;
  // The purpose of the operator below is to save on typing
  // when returning false with a comment.
  operator bool() const {
    return false;
  }
  // Adds an expression to the global list of expressions that are children of another expression.
  int addChildExpression(const Expression& child);
  void registerCalculatorFunction(Function& inputFunction, int indexOperation);
  std::string toStringSemismipleLieAlgebraLinksFromHD(
    const DynkinType& theType, FormatExpressions* theFormat = nullptr
  );
  bool isBoundVariableInContext(int inputOp);
  bool isNonBoundVariableInContext(int inputOp);
  Function& getFunctionHandlerFromNamedRule(const std::string& inputRuleName);
  bool checkPredefinedFunctionNameRepetitions();
  bool checkOperationHandlers();
  bool checkConsistencyAfterInitialization();
  // To make operations read only, we make operations private and return const pointer to it.
  const HashedList<std::string, HashFunctions::hashFunction>& getOperations() {
    return this->operations.keys;
  }
  const HashedList<std::string, MathRoutines::hashString>& getBuiltInTypes() {
    return this->builtInTypes;
  }
  const List<Function>* getOperationHandlers(int theOp);
  const List<Function>* getOperationCompositeHandlers(int theOp);
  SyntacticElement getSyntacticElementEnd() {
    SyntacticElement result;
    result.controlIndex = this->controlSequences.getIndex(";");
    return result;
  }
  bool decreaseStackSetCharacterRanges(int decrease) {
    if (decrease <= 0) {
      return true;
    }
    if ((*this->currentSyntacticStack).size - decrease <= 0) {
      global.fatal << "Bad stack decrease. " << global.fatal;
    }
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - decrease);
    return true;
  }
  Expression expressionZero();
  Expression expressionOne();
  Expression expressionTwo();
  Expression expressionFour();
  Expression expressionMinusOne();
  Expression expressionHalf();
  Expression expressionMinusHalf();
  Expression expressionInfinity();
  Expression expressionMinusInfinity();
  void logFunctionWithTime(Function& inputF);
  void logTime();
  void logPublicError(const std::string& theError);
  bool decreaseStackExceptLast(int decrease);
  bool decreaseStackExceptLastTwo(int decrease);
  std::string toStringSyntacticStackHTMLTable(
    bool includeLispifiedExpressions, bool ignoreCommandEnclosures
  );
  std::string toStringSyntacticStackHTMLSimple();
  std::string writeDefaultLatexFileReturnHtmlLink(
    const std::string& fileContent,
    std::string* outputFileNameNoExtension,
    bool useLatexDviPSpsToPNG = false
  );
  std::string toStringIsCorrectAsciiCalculatorString(const std::string& input);
  bool isInterpretedAsEmptySpace(const std::string& input);
  bool isInterpretedAsEmptySpace(unsigned char input);
  bool isSeparatorFromTheLeftGeneral(const std::string& input);
  bool isSeparatorFromTheLeftForDefinition(const std::string& input);
  bool isSeparatorFromTheLeftForList(const std::string& input);
  bool isStandardCalculatorCharacter(unsigned char input);
  bool isSeparatorFromTheLeftForInterval(const std::string& input);
  bool isSeparatorFromTheRightGeneral(const std::string& input);
  bool isSeparatorFromTheRightForDefinition(const std::string& input);
  bool isSeparatorFromTheRightForList(const std::string& input);
  bool allowsPowerInPreceding(const std::string& lookAhead);
  bool allowsPowerInNext(const std::string& lookBehind);
  bool recursionDepthExceededHandleRoughly(const std::string& additionalErrorInfo = "");

  bool allowsLimitProcessInPreceding(const std::string& lookAhead);
  bool allowsApplyFunctionInPreceding(const std::string& lookAhead);
  bool allowsIfInPreceding(const std::string& lookAhead);
  bool allowsOrInPreceding(const std::string& lookAhead);
  bool allowsAndInPreceding(const std::string& lookAhead);
  bool allowsInInPreceding(const std::string& lookAhead);
  bool allowsPlusInPreceding(const std::string& lookAhead);
  bool allowsTimesInNext(const std::string& thePreceding);
  bool allowsTimesInPreceding(const SyntacticElement& thePreceding, const std::string& lookAhead);

  bool allowsTimesInPreceding(const std::string& lookAhead);
  bool allowsTensorInPreceding(const std::string& lookAhead);
  bool allowsDivideInPreceding(const std::string& lookAhead);
  bool popTopSyntacticStack() {
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - 1);
    return true;
  }
  bool replaceEXXEXEBy_CofEEE(int theOp);
  bool replaceEXXEXEXBy_CofEEE_X(int theOp);
  bool replaceEOXbyEX();
  bool replaceEEBy_CofEE(int theControlIndex);
  bool replaceEEXBy_CofEE_X(int theControlIndex);
  bool replaceOOEEXbyEXpowerLike();
  bool replaceEEByE();
  bool replaceEXEXByEX();
  bool replaceEXEXBy_OofEE_X(int theOp);
  bool replaceSsSsXdotsXbySsXdotsX(int numDots);
  bool replaceEXdotsXbySsXdotsX(int numDots);
  bool replaceEXdotsXBySs(int numDots) {
    this->replaceEXdotsXbySsXdotsX(numDots);
    return this->decreaseStackSetCharacterRanges(numDots);
  }
  bool replaceOEXByE();
  bool replaceOEXByEX();
  bool replaceC1C2Eby_C2ofC1E();
  bool replaceEOByE();
  bool replaceOEByE();
  bool replaceOXXByEXX    ();
  bool replaceOXEByE      ();
  bool replaceOXEXByEX    ();
  bool replaceOXEEXByEX   ();
  bool replaceOXXEXEXEXByE();
  bool replaceSqrtEXXByEXX();
  bool replaceSqrtEXByEX  ();
  bool replaceSqrtXEXByEX ();
  bool replaceOXEXEByE    ();
  bool replaceOXEXEXByEX  ();
  bool replaceOXEXEXXByEXX();
  bool replaceOXEXEXEXByE ();
  bool replaceEOEXByEX    ();
  bool replaceUnderscoreEPowerEbyLimits();
  bool replacePowerEUnderScoreEbyLimits();
  bool replaceXEEXBy_OofEE_X(int inputOperation);
  bool replaceXEEBy_OofEE(int inputOperation);
  bool replaceECByC();
  bool replaceEXEBySequence(int theControlIndex);
  bool replaceYXBySequenceX(int theControlIndex);
  bool replaceXXXbyE();
  bool replaceYBySequenceY(int theControlIndex);
  bool replaceXXYBySequenceY(int theControlIndex);
  bool replaceXXYXBySequenceYX(int theControlIndex);
  bool replaceYXdotsXBySequenceYXdotsX(int theControlIndex, int numXs = 0);
  bool replaceSequenceXEBySequence    (int theControlIndex);
  bool replaceSequenceUXEYBySequenceZY(int theControlIndex);
  bool replaceCEByC();
  bool replaceCCByC();
  bool replaceEOEByE() {
    return this->replaceEXEByCofEE((*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].controlIndex);
  }
  bool replaceMatrixEXByMatrix();
  bool replaceMatrixEXByMatrixX();
  bool replaceMatrixEXByMatrixNewRow();
  bool replaceMatrixXByE();
  bool replaceCXByE();
  bool replaceCXByEX();
  bool replaceXEXEXBy_CofEE(int theOperation);
  bool replaceEXEByCofEE  (int theOperation);
  bool replaceXYByConY(int theCon) {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].controlIndex = theCon;
    return true;
  }
  bool replaceXYYByConYY(int theCon) {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = theCon;
    return true;
  }
  bool replaceXYYYByConYYY(int theCon) {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = theCon;
    return true;
  }
  bool replaceXXByCon(int theCon) {
    this->replaceXYByConY(theCon);
    return this->decreaseStackSetCharacterRanges(1);
  }
  bool replaceXByCon(int theCon);
  bool replaceXByO(int theOperation);
  bool replaceXByConCon(int con1, int con2);
  bool replaceXXXByCon(int theCon);
  bool replaceXXXByConCon(int con1, int con2);
  bool replaceXXXXXByCon(int theCon);
  bool replaceXXXXXByConCon(int con1, int con2);
  bool replaceXXXXByConCon (int con1, int con2);
  bool replaceXdotsXByMatrixStart(int numXes);
  bool replaceXXXXByCon(int con1);
  bool replaceXXYByY() {
    (*this->currentSyntacticStack)[this->currentSyntacticStack->size - 3] =
    (*this->currentSyntacticStack)[this->currentSyntacticStack->size - 1];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - 2);
    return true;
  }
  bool replaceXXYXByYX() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4] =
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3] =
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - 2);
    return true;
  }
  bool replaceXYYXByYY() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4] =
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3] =
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - 2);
    return true;
  }
  bool replaceXYXByY() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3] =
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
    this->decreaseStackSetCharacterRanges(2);
    return true;
  }
  bool replaceXYByY() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2] =
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - 1);
    return true;
  }
  bool replaceXEXByE();
  bool replaceXEXYByEY();
  bool replaceVbyVdotsVAccordingToPredefinedWordSplits();
  bool replaceAXbyEX();
  bool replaceIntegerXbyEX();
  bool replaceIntegerDotIntegerByE();
  bool replaceXXByEEmptySequence();
  bool replaceOXdotsXbyEXdotsX(int numXs);
  bool replaceOXbyEX();
  bool replaceObyE();
  bool replaceXXbyEX();
  bool replaceXXbyO(int theOperation);
  bool replaceXXYbyOY(int theOperation);
  bool replaceXEXByEContainingOE(int inputOpIndex);
  bool replaceXXByEmptyString();
  bool replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  bool replaceXXbyE();
  bool getMatrixExpressions(
    const Expression& input,
    Matrix<Expression>& output,
    int desiredNumRows = - 1,
    int desiredNumCols = - 1
  );
  bool getMatrixExpressionsFromArguments(
    const Expression& input,
    Matrix<Expression>& output,
    int desiredNumRows = - 1,
    int desiredNumCols = - 1
  );
  void makeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output);
  bool replaceXXVXdotsXbyE_BOUND_XdotsX(int numXs);
  bool replaceVXdotsXbyE_NONBOUND_XdotsX(int numXs);
  int getOperationIndexFromControlIndex(int controlIndex);
  int getExpressionIndex();
  SyntacticElement getEmptySyntacticElement();
  bool accountRule(const Expression& ruleE, StateMaintainerCalculator& theRuleStackMaintainer);
  bool applyOneRule();
  void resetStack() {
    SyntacticElement emptyElement = this->getEmptySyntacticElement();
    (*this->currentSyntacticStack).initializeFillInObject(this->numEmptyTokensStart, emptyElement);
  }
  int conError() {
    return this->controlSequences.getIndexNoFail("Error");
  }
  int conExpression() {
    return this->controlSequences.getIndexNoFail("Expression");
  }
  int conVariable() {
    return this->controlSequences.getIndexNoFail("Variable");
  }
  int conBindVariable() {
    return this->controlSequences.getIndexNoFail("{{}}");
  }
  int conInteger() {
    return this->controlSequences.getIndexNoFail("Integer");
  }
  int conEqualEqual() {
    return this->controlSequences.getIndexNoFail("==");
  }
  int conEqualEqualEqual() {
    return this->controlSequences.getIndexNoFail("===");
  }
  int conApplyFunction() {
    return this->controlSequences.getIndexNoFail("{}");
  }
  int conTimes() {
    return this->controlSequences.getIndexNoFail("*");
  }
  int conEndStatement() {
    return this->controlSequences.getIndexNoFail(";");
  }
  int conDefineConditional() {
    return this->controlSequences.getIndexNoFail("if=");
  }
  int conGEQ() {
    return this->controlSequences.getIndexNoFail("\\geq");
  }
  int conComma() {
    return this->controlSequences.getIndexNoFail(",");
  }
  int conDefine() {
    return this->controlSequences.getIndexNoFail("=");
  }
  int conIsDenotedBy() {
    return this->controlSequences.getIndexNoFail("=:");
  }
  int conSequenceStatements() {
    return this->controlSequences.getIndexNoFail("SequenceStatements");
  }
  int conSequence() {
    return this->controlSequences.getIndexNoFail("Sequence");
  }
  int conSequenceNoRepetition() {
    return this->controlSequences.getIndexNoFail("SequenceNoRepetition");
  }
  int conMatrixStart() {
    return this->controlSequences.getIndexNoFail("Matrix");
  }
  int conMatrixEnd() {
    return this->controlSequences.getIndexNoFail("MatrixEnd");
  }
  int conLieBracket() {
    return this->controlSequences.getIndexNoFail("[]");
  }
  int conOpenBracket() {
    return this->controlSequences.getIndexNoFail("(");
  }
  int conCloseBracket() {
    return this->controlSequences.getIndexNoFail(")");
  }
  int conEndProgram() {
    return this->controlSequences.getIndexNoFail("EndProgram");
  }
  int opEltZmodP() {
    return this->operations.getIndexNoFail("EltZmodP");
  }
  int opIsDenotedBy() {
    return this->operations.getIndexNoFail("=:");
  }
  int opDefine() {
    return this->operations.getIndexNoFail("=");
  }
  int opCoefficientOf() {
    return this->operations.getIndexNoFail("CoefficientOf");
  }
  int opCommandEnclosure() {
    return this->operations.getIndexNoFail(Calculator::Atoms::commandEnclosure);
  }
  int opRulesOff() {
    return this->operations.getIndexNoFail("RulesOff");
  }
  int opRulesOn() {
    return this->operations.getIndexNoFail("RulesOn");
  }
  int opCommandEnclosureStart() {
    return this->operations.getIndexNoFail("CommandEnclosureStart");
  }
  int opCommandEnclosureFinish() {
    return this->operations.getIndexNoFail("CommandEnclosureFinish");
  }
  int opTurnOnRules() {
    return this->operations.getIndexNoFail("TurnOnRules");
  }
  int opTurnOffRules() {
    return this->operations.getIndexNoFail("TurnOffRules");
  }
  int opSqrt() {
    return this->operations.getIndexNoFail("\\sqrt");
  }
  int opIf() {
    return this->operations.getIndexNoFail("if");
  }
  int opDefineConditional() {
    return this->operations.getIndexNoFail("if=");
  }
  int opThePower() {
    return this->operations.getIndexNoFail("^");
  }
  int opUnderscore() {
    return this->operations.getIndexNoFail("_");
  }
  int opEqualEqual() {
    return this->operations.getIndexNoFail("==");
  }
  int opNotEqual() {
    return this->operations.getIndexNoFail("\\neq");
  }
  int opEqualEqualEqual() {
    return this->operations.getIndexNoFail("===");
  }
  int opGreaterThanOrEqualTo() {
    return this->operations.getIndexNoFail("\\geq");
  }
  int opLessThanOrEqualTo() {
    return this->operations.getIndexNoFail("\\leq");
  }
  int opGreaterThan() {
    return this->operations.getIndexNoFail(">");
  }
  int opLessThan() {
    return this->operations.getIndexNoFail("<");
  }
  int opWeightLieAlgebra() {
    return this->operations.getIndexNoFail("weightLieAlg");
  }
  int opWeightLieAlgPoly() {
    return this->operations.getIndexNoFail("weightLieAlgPoly");
  }
  int opError() {
    return this->operations.getIndexNoFail("Error");
  }
  int opList() {
    return this->operations.getIndexNoFail("");
  }
  int opMonomialPoly() {
    return this->operations.getIndexNoFail("MonomialPoly");
  }
  int opCalculusPlot() {
    return this->operations.getIndexNoFail("CalculusPlot");
  }
  int opMatrix() {
    return this->operations.getIndexNoFail("Matrix");
  }
  int opSequence() {
    return this->operations.getIndexNoFail("Sequence");
  }
  int opIntervalOpen() {
    return this->operations.getIndexNoFail("IntervalOpen");
  }
  int opIntervalLeftClosed() {
    return this->operations.getIndexNoFail("[)");
  }
  int opIntervalRightClosed() {
    return this->operations.getIndexNoFail("(]");
  }
  int opIntervalClosed() {
    return this->operations.getIndexNoFail("IntervalClosed");
  }
  int opExpressionHistory() {
    return this->operations.getIndexNoFail("ExpressionHistory");
  }
  int opExpressionHistorySet() {
    return this->operations.getIndexNoFail("ExpressionHistorySet");
  }
  int opExpressionHistorySetChild() {
    return this->operations.getIndexNoFail("ExpressionHistorySetChild");
  }
  int opQuote() {
    return this->operations.getIndexNoFail("\"");
  }
  int opMelt() {
    return this->operations.getIndexNoFail("Melt");
  }
  int opRational() {
    return this->operations.getIndexNoFail("Rational");
  }
  int opElementHyperOctahedral() {
    return this->operations.getIndexNoFail("ElementHyperoctahedral");
  }
  int opDouble() {
    return this->operations.getIndexNoFail("Double");
  }
  int opAlgebraicNumber() {
    return this->operations.getIndexNoFail("AlgebraicNumber");
  }
  int opElementWeylAlgebra() {
    return this->operations.getIndexNoFail("ElementWeylAlgebra");
  }
  int opPolynomialRational() {
    return this->operations.getIndexNoFail("PolynomialRational");
  }
  int opPolynomialModuloInteger() {
    return this->operations.getIndexNoFail("PolynomialModuloInteger");
  }
  int opPolynomialAlgebraicNumbers() {
    return this->operations.getIndexNoFail("PolynomialOverANs");
  }
  int opPolynomialModuloPolynomialModuloInteger() {
    return this->operations.getIndexNoFail("PolynomialModuloPolynomialModuloInteger");
  }
  int opEllipticCurveElementsRational() {
    return this->operations.getIndexNoFail("ellipticCurveElementsRational");
  }
  int opEllipticCurveElementsZmodP() {
    return this->operations.getIndexNoFail("ellipticCurveElementsZmodP");
  }
  int opRationalFunction() {
    return this->operations.getIndexNoFail("RationalFunction");
  }
  int opRationalFunctionAlgebraicCoefficients() {
    return this->operations.getIndexNoFail("RationalFunctionAlgebraicCoefficients");
  }
  int opRationalFunctionModuloInteger() {
    return this->operations.getIndexNoFail("RationalFunctionModuloInteger");
  }
  int opDifferentiate() {
    return this->operations.getIndexNoFail("Differentiate");
  }
  int opDifferential() {
    return this->operations.getIndexNoFail("Differential");
  }
  int opIndefiniteIndicator() {
    return this->operations.getIndexNoFail("IndefiniteIndicator");
  }
  int opIntegral() {
    return this->operations.getIndexNoFail("\\int");
  }
  int opAbsoluteValue() {
    return this->operations.getIndexNoFail("|");
  }
  int opMatrixTensorRational() {
    return this->operations.getIndexNoFail("MatrixTensorRational");
  }
  int opWeylGroupRep() {
    return this->operations.getIndexNoFail("WeylGroupRep");
  }
  int opFreeze() {
    return this->operations.getIndexNoFail("Freeze");
  }
  int opString() {
    return this->operations.getIndexNoFail("string");
  }
  int opJSON() {
    return this->operations.getIndexNoFail("JSON");
  }
  int opElementUEOverRF() {
    return this->operations.getIndexNoFail("ElementUEoverRF");
  }
  int opHyperoctahedralGroupRep() {
    return this->operations.getIndexNoFail("HyperoctahedralGroupRepresentation");
  }
  int opWeylGroupVirtualRep() {
    return this->operations.getIndexNoFail("WeylGroupVirtualRep");
  }
  int opElementTensorGVM() {
    return this->operations.getIndexNoFail("ElementTensorGVM");
  }
  int opCharSSAlgMod() {
    return this->operations.getIndexNoFail("CharSSAlgMod");
  }
  int opSemisimpleLieAlgebrA() {
    return this->operations.getIndexNoFail("SemisimpleLieAlg");
  }
  int opSemisimpleSubalgebras() {
    return this->operations.getIndexNoFail("SemisimpleSubalgebras");
  }
  int opCandidateSSsubalgebra() {
    return this->operations.getIndexNoFail("CandidateSSsubalgebra");
  }
  int opInfinity() {
    return this->operations.getIndexNoFail("\\infty");
  }
  int opEmptySet() {
    return this->operations.getIndexNoFail("\\emptyset");
  }
  int opIn() {
    return this->operations.getIndexNoFail("\\in");
  }
  int opPi() {
    return this->operations.getIndexNoFail("\\pi");
  }
  int opE() {
    return this->operations.getIndexNoFail("e");
  }
  int opImaginaryUnit() {
    return this->operations.getIndexNoFail("i");
  }
  int opLogBase() {
    return this->operations.getIndexNoFail("LogBase");
  }
  int opLog() {
    return this->operations.getIndexNoFail("\\log");
  }
  int opPhantom() {
    return this->operations.getIndexNoFail("\\phantom");
  }
  int opCirc() {
    return this->operations.getIndexNoFail("\\circ");
  }
  int opSin() {
    return this->operations.getIndexNoFail("\\sin");
  }
  int opFactorial() {
    return this->operations.getIndexNoFail("!");
  }
  int opLimitBoundary() {
    return this->operations.getIndexNoFail("\\limits");
  }
  int opLimitProcess() {
    return this->operations.getIndexNoFail("\\to");
  }
  int opLimit() {
    return this->operations.getIndexNoFail("\\lim");
  }
  int opCos() {
    return this->operations.getIndexNoFail("\\cos");
  }
  int opArcTan() {
    return this->operations.getIndexNoFail("\\arctan");
  }
  int opArcSin() {
    return this->operations.getIndexNoFail("\\arcsin");
  }
  int opArcCos() {
    return this->operations.getIndexNoFail("\\arccos");
  }
  int opTan() {
    return this->operations.getIndexNoFail("\\tan");
  }
  int opCot() {
    return this->operations.getIndexNoFail("\\cot");
  }
  int opSec() {
    return this->operations.getIndexNoFail("\\sec");
  }
  int opCsc() {
    return this->operations.getIndexNoFail("\\csc");
  }
  int opLittelmannPath() {
    return this->operations.getIndexNoFail("LittelmannPath");
  }
  int opLRO() {
    return this->operations.getIndexNoFail("LRO");
  }
  int opWeylGroup() {
    return this->operations.getIndexNoFail("WeylGroup");
  }
  int opUnion() {
    return this->operations.getIndexNoFail("\\cup");
  }
  int opIntersection() {
    return this->operations.getIndexNoFail("\\cap");
  }
  int opPolynomialVariables() {
    return this->operations.getIndexNoFail("PolyVars");
  }
  int opWeylAlgebraVariables() {
    return this->operations.getIndexNoFail("DiffOpVars");
  }
  int opContext() {
    return this->operations.getIndexNoFail("Context");
  }
  int opWeylGroupElement() {
    return this->operations.getIndexNoFail("ElementWeylGroup");
  }
  int opCommandSequence() {
    return this->operations.getIndexNoFail(";");
  }
  int opUnionNoRepetition() {
    return this->operations.getIndexNoFail("\\sqcup");
  }
  int opBind() {
    return this->operations.getIndexNoFail("Bind");
  }
  int opOr() {
    return this->operations.getIndexNoFail("or");
  }
  int opAnd() {
    return this->operations.getIndexNoFail("and");
  }
  int opPlus() {
    return this->operations.getIndexNoFail("+");
  }
  int opSolveJSON() {
    return this->operations.getIndexNoFail("SolveJSON");
  }
  int opDirectSum() {
    return this->operations.getIndexNoFail("\\oplus");
  }
  int opUserInputTextBox() {
    return this->operations.getIndexNoFail("userInputTextBox");
  }
  int opMod() {
    return this->operations.getIndexNoFail("mod");
  }
  int opMinus() {
    return this->operations.getIndexNoFail("-");
  }
  int opTimes() {
    return this->operations.getIndexNoFail("*");
  }
  int opSum() {
    return this->operations.getIndexNoFail("\\sum");
  }
  int opCrossProduct() {
    return this->operations.getIndexNoFail("\\times");
  }
  int opTensor() {
    return this->operations.getIndexNoFail("\\otimes");
  }
  int opBinom() {
    return this->operations.getIndexNoFail("\\binom");
  }
  int opSetMinus() {
    return this->operations.getIndexNoFail("\\setminus");
  }
  int opLieBracket() {
    return this->operations.getIndexNoFail("[]");
  }
  int opDivide() {
    return this->operations.getIndexNoFail("/");
  }
  int opInterpretProblem() {
    return this->operations.getIndexNoFail("InterpretProblem");
  }
  int opInterpretProblemGiveUp() {
    return this->operations.getIndexNoFail("ProblemGiveUp");
  }
  bool appendOpandsReturnTrueIfOrderNonCanonical(const Expression& input, List<Expression>& output, int operation);
  bool appendMultiplicandsReturnTrueIfOrderNonCanonical(Expression& theExpression, List<Expression>& output) {
    return this->appendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opTimes());
  }
  bool appendSummandsReturnTrueIfOrderNonCanonical(const Expression& theExpression, List<Expression>& output) {
    return this->appendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus());
  }
  void specializeBoundVariables(Expression& toBeSubbedIn, MapList<Expression, Expression>& matchedPairs);
  Expression* patternMatch(
    const Expression& thePattern,
    Expression& theExpression,
    MapList<Expression, Expression>& bufferPairs,
    const Expression* condition = nullptr,
    std::stringstream* theLog = nullptr
  );
  bool processOneExpressionOnePatternOneSub(
    const Expression& thePattern,
    Expression& theExpression,
    MapList<Expression, Expression>& bufferPairs, std::stringstream* theLog = nullptr
  );
  static void checkInputNotSameAsOutput(const Expression& input, const Expression& output) {
    if (&input == &output) {
      global.fatal << "The input expression, equal to "
      << input.toString() << " has the same address as the output expression. "
      << global.fatal;
    }
  }
  bool callCalculatorFunction(Expression::FunctionAddress toBeCalled, const Expression& input, Expression& output) {
    if (&input == &output) {
      Expression inputCopy = input;
      return this->callCalculatorFunction(toBeCalled, inputCopy, output);
    }
    if (!toBeCalled(*this, input, output)) {
      return false;
    }
    return !output.isError();
  }
  bool expressionMatchesPattern(
    const Expression& thePattern,
    const Expression& input,
    MapList<Expression, Expression>& matchedExpressions,
    std::stringstream* commentsGeneral = nullptr
  );

  static bool functionReverseOrderRecursively(
    Calculator& calculator, const Expression& input, Expression& output
  );
  std::string writeFileToOutputFolderReturnLink(
    const std::string& fileContent,
    const std::string& fileName,
    const std::string& linkText
  );
  class Test {
  public:
    int64_t startTime;
    int startIndex;
    int numberOfTests;
    int lastIndexNotTested;
    int inconsistencies;
    int unknown;
    // see Function::Options::
    int noTestSkips;
    Calculator* owner;
    std::string reportHtml;
    bool flagTestResultsExist;
    std::string debugFlagAtStart;
    class OneTest {
    public:
      std::string atom;
      std::string functionAdditionalIdentifier;
      std::string command;
      // empty string = not computed
      std::string actualResult;
      // empty string = not known
      std::string expectedResult;
    };
    MapList<std::string, Calculator::Test::OneTest, MathRoutines::hashString> commands;
    JSData storedResults;
    Test(Calculator& inputOwner);
    bool processResults();
    bool loadTestStrings(std::stringstream* commentsOnFailure);
    bool writeTestStrings(std::stringstream* commentsOnFailure);
    bool processOneTest(JSData& input);
    static bool all();
    static bool numberOfTestFunctions(Calculator& ownerInitialized);
    static bool parseDecimal(Calculator& ownerInitialized);
    static bool parseAllExamples(Calculator& ownerInitialized);
    static bool builtInFunctionsABTest(Calculator& ownerInitialized);

    static bool checkBuiltInInitializations(Calculator& ownerInitialized);
    void calculatorTestPrepare();
    bool calculatorTestRun();
  };
  static bool innerAutomatedTest(Calculator& calculator, const Expression& input, Expression& output);
  int getNumberOfBuiltInFunctions();
  static bool outerStandardFunction(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int opIndexParentIfAvailable,
    Function* *outputHandler
  );
  static bool outerStandardCompositeHandler(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int opIndexParentIfAvailable,
    Function** outputHandler
  );
  static bool outerStandardHandler(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int opIndexParentIfAvailable,
    Function** outputHandler
  );
  static bool outerPowerRaiseToFirst(Calculator& calculator, const Expression& input, Expression& output);
  static bool collectCoefficientsPowersVariables(
    const Expression& input,
    const Expression& theVariable,
    VectorSparse<Expression>& outputPositionIiscoeffXtoIth
  );
  bool collectOpands(const Expression& input, int operation, List<Expression>& outputOpands);
  bool collectOpandsAccumulate(const Expression& input, int operation, List<Expression>& outputOpands);
  static bool functionCollectOneSummand(
    Calculator& calculator,
    const Expression& input,
    List<Expression>& outputMonomials,
    List<Rational>& outputCoefficients
  );
  static bool functionCollectSummandsSeparatelyTrueIfOrderNonCanonical(
    Calculator& calculator,
    const Expression& input,
    List<Expression>& summands,
    List<Expression>& outputMonomials,
    List<Rational>& outputCoefficients
  );
  static bool functionCollectSummandsCombine(
    Calculator& calculator,
    const Expression& input,
    LinearCombination<Expression, Rational>& outputSum
  );
  template<class theType>
  bool functionGetMatrix(
    const Expression& input,
    Matrix<theType>& outputMatrix,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetNumColsNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr,
    std::stringstream* commentsOnError = nullptr
  );
  template <class theType>
  bool getVectorFromFunctionArguments(
    const Expression& input,
    Vector<theType>& output,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetDimNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr
  ) {
    Expression list = input;
    if (list.isList()) {
      list.setChildAtomValue(0, this->opSequence());
    }
    return this->getVector(
      list,
      output,
      inputOutputStartingContext,
      targetDimNonMandatory,
      conversionFunction
    );
  }
  bool getSumProductsExpressions(const Expression& inputSum, List<List<Expression> >& outputSumMultiplicands);
  bool getVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory = - 1);
  bool convertExpressionsToCommonContext(
    List<Expression>& inputOutputEs,
    ExpressionContext* inputOutputStartingContext = nullptr
  );
  bool getVectorInt(const Expression& input, List<int>& output);
  bool getVectorLargeIntegerFromFunctionArguments(const Expression& input, List<LargeInteger>& output);
  bool getMatrixDoubles(const Expression& input, Matrix<double>& output, int desiredNumberOfColumns = - 1);

  bool getVectorDoubles(const Expression& input, Vector<double>& output, int desiredDimensionNonMandatory = - 1);
  bool getVectorDoublesFromFunctionArguments(
    const Expression& input, Vector<double>& output, int desiredDimensionNonMandatory = - 1
  );
  template <class theType>
  bool getVector(
    const Expression& input,
    Vector<theType>& output,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetDimensionNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr
  );
  bool getListPolynomialVariableLabelsLexicographic(
    const Expression& input,
    Vector<Polynomial<AlgebraicNumber> >& output,
    ExpressionContext& outputContext
  );
  template<class Coefficient>
  bool getTypeWeight(
    Calculator& calculator,
    const Expression& input,
    Vector<Coefficient>& outputWeightSimpleCoords,
    WithContext<SemisimpleLieAlgebra*>& outputAmbientSSalgebra,
    Expression::FunctionAddress ConversionFun
  );
  template<class Coefficient>
  bool getTypeHighestWeightParabolic(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vector<Coefficient>& outputWeightHWFundcoords,
    Selection& outputInducingSelection,
    WithContext<SemisimpleLieAlgebra*>& outputAmbientSSalgebra,
    Expression::FunctionAddress ConversionFun
  );
  void addEmptyHeadedCommand();
  Calculator();
  int addOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName);
  void addOperationNoRepetitionAllowed(const std::string& operation);
  void addOperationBuiltInType(const std::string& operationBuiltIn);
  void addTrigonometricSplit(const std::string& trigonometricFunction, const List<std::string>& variables);
  void addKnownDoubleConstant(const std::string& constantName, double value);
  void addOperationBinaryInnerHandlerWithTypes(
    const std::string& operation,
    Expression::FunctionAddress innerHandler,
    int leftType,
    int rightType,
    const std::string& opDescription,
    const std::string& opExample,
    const std::string& inputAdditionalIdentifier,
    const std::string& inputCalculatorIdentifier,
    const Function::Options& options
  );
  void addOperationHandler(
    const std::string& operation,
    Expression::FunctionAddress handler,
    const std::string& opArgumentListIgnoredForTheTimeBeing,
    const std::string& opDescription,
    const std::string& opExample,
    const std::string& inputAdditionalIdentifier,
    const std::string& inputCalculatorIdentifier,
    const Function::Options& options,
    const std::string& parentOpThatBansHandler = ""
  );
  void addOneStringAtomHandler(
    int atom,
    Expression::ToStringHandler handler
  );
  void addOneStringCompositeHandler(
    int atom,
    Expression::ToStringHandler handler
  );
  template <class builtInType>
  void addOneBuiltInHandler();
  void addOneStringHandler(
    int atom,
    Expression::ToStringHandler handler,
    MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction>& handlerCollection
  );
  void reset();
  void initialize(Calculator::Mode desiredMode);
  void initializeToStringHandlers();
  void initializePredefinedWordSplits();
  void initializeAtomsThatFreezeArguments();
  void initializeBuiltInsFreezeArguments();
  void initializeAtomsNonCacheable();
  void initializeAtomsThatAllowCommutingOfArguments();
  void initializeStringsThatSplitIfFollowedByDigit();
  void initializeOperationsInterpretedAsFunctionsMultiplicatively();
  void initializeOperationsThatAreKnownFunctions();
  void initializeArithmeticOperations();
  void initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  void initializeBuiltInAtomsNotInterpretedAsFunctions();
  void initializeAtomsNotGoodForChainRule();
  void initializeFunctionsStandard();
  void initializeFunctionsScientificBasic();
  void initializeFunctionsExtra();
  void initializeFunctionsCryptoAndEncoding();
  void initializeFunctionsSemisimpleLieAlgebras();
  void initializePredefinedStandardOperationsWithoutHandler();
  void initializeAdminFunctions();
  bool extractExpressions(Expression& outputExpression, std::string* outputErrors);
  JSData toJSONOutputAndSpecials();
  void evaluateCommands();
  JSData extractSolution();
  bool isTimedOut();
  static bool evaluateExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool evaluateExpression(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool& outputIsCacheable,
    int opIndexParentIfAvailable,
    Expression* outputHistory
  );
  class EvaluateLoop {
  public:
    Calculator* owner;
    bool flagIsNonCacheable;
    int opIndexParent;
    int numberOfTransformations;
    int indexInCache;
    ProgressReport theReport;
    bool reductionOccurred;
    Expression* output;
    Expression* history;
    Expression currentChild;
    bool reduceOnce();
    bool builtInEvaluation();
    bool userDefinedEvaluation();
    void initializeOneRun();
    bool outputHasErrors();
    EvaluateLoop(Calculator& inputOwner);
    bool evaluateChildren(StateMaintainerCalculator& maintainRuleStack);
    void reportChildEvaluation(Expression& output, int childIndex);
    void lookUpCache();
    bool setOutput(
      const Expression& input, Function* handler, const std::string& info
    );
    void accountHistory(Function *handler, const std::string &info);
    bool checkInitialization();
    void accountHistoryChildTransformation(
      const Expression& transformedChild, const Expression& childHistory, int childIndex
    );
  };

  class ExpressionHistoryEnumerator {
  public:
    Calculator* owner;
    int recursionDepth;
    int maximumRecursionDepth;
    Expression theHistory;
    List<Expression> output;
    List<List<std::string> > rulesNames;
    // List<List<std::string> > rulesDisplayNames;
    HashedList<std::string, MathRoutines::hashString> rulesToBeIgnored;
    MapList<std::string, std::string, MathRoutines::hashString> rulesDisplayNamesMap;
    bool computeRecursively(int incomingRecursionDepth, std::stringstream* commentsOnFailure);
    bool processTransformation(const Expression& current, std::stringstream* commentsOnFailure);
    bool processChildrenTransformations(int startIndex, int numberOfChildren, std::stringstream* commentsOnFailure);
    void initializeComputation();
    ExpressionHistoryEnumerator();
    bool checkInitialization() {
      if (this->owner == nullptr) {
        global.fatal << "Expression history enumerator has zero owner. " << global.fatal;
      }
      return true;
    }
    std::string toStringExpressionHistoryMerged();
    std::string toStringDebug();
  };
  void evaluate(const std::string& input);
  // Attempts to interpret the input string as a high-school or calculus problem and solve it.
  JSData solve(const std::string& input);
  bool parseAndExtractExpressions(
    const std::string& input,
    Expression& output,
    List<SyntacticElement>& outputSynSoup,
    List<SyntacticElement>& outputSynStack,
    std::string* outputSyntacticErrors
  );
  bool parse(
    const std::string& input,
    bool stripCommandSequence,
    Expression& output
  );
  // For internal use only; will crash the calculator if the input has syntax errors.
  Expression parseOrCrash(const std::string& input, bool stripCommandSequence);
  bool isLeftSeparator(unsigned char c);
  bool isRightSeparator(unsigned char c);
  bool shouldSplitOutsideQuotes(const std::string& left, char right);
  void parseFillDictionary(const std::string& input, List<SyntacticElement>& output);
  void parseFillDictionary(const std::string& input);
};

class CalculatorBasics {
public:
  static bool tensorProductStandard(Calculator& calculator, const Expression& input, Expression& output);
  static bool standardIsDenotedBy(Calculator& calculator, const Expression& input, Expression& output);
  static bool minus(Calculator& calculator, const Expression& input, Expression& output);
  static bool combineFractions(Calculator& calculator, const Expression& input, Expression& output);
  static bool checkRule(Calculator& calculator, const Expression& input, Expression& output);
  static bool associate(Calculator& calculator, const Expression& input, Expression& output);
  static bool subZeroDivAnythingWithZero(Calculator& calculator, const Expression& input, Expression& output);
  static bool cancelMultiplicativeInverse(Calculator& calculator, const Expression& input, Expression& output);
  static bool associateExponentExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool associateTimesDivision(Calculator& calculator, const Expression& input, Expression& output);
  static bool flattenCommandEnclosuresOneLayeR(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionFlattenCommandEnclosuresOneLayer(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAtoXtimesAtoYequalsAtoXplusY(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool extractBaseMultiplication(Calculator& calculator, const Expression& input, Expression& output);
  static bool meltBrackets(Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplyByOne(Calculator& calculator, const Expression& input, Expression& output);
  static bool timesToFunctionApplication(Calculator& calculator, const Expression& input, Expression& output);
  static bool distributeTimes(Calculator& calculator, const Expression& input, Expression& output);
  static bool distributeTimesConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool distribute(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int additiveOperation = - 1,
    int multiplicativeOperation = - 1,
    bool constantsOnly = false
  );
  static bool leftDistributeBracketIsOnTheLeft(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int additiveOperation = - 1,
    int multiplicativeOperation = - 1,
    bool constantsOnly = false
  );
  static bool rightDistributeBracketIsOnTheRight(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int additiveOperation = - 1,
    int multiplicativeOperation = - 1,
    bool constantsOnly = false
  );
  static bool evaluateIf(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorSteps {
public:
  static bool innerLogEvaluationStepsDebug(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerLogEvaluationSteps(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerLogEvaluationStepsHumanReadableMerged(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doDebug
  );
};

class CalculatorConversions {
public:
  ////////////////////Conversion from expression tree/////////////////////////////////
  // conversions from expression tree to type
  static bool innerElementSemisimpleLieAlgebraRationalCoeffs(
    Calculator& calculator,
    const Expression& input,
    ElementSemisimpleLieAlgebra<Rational>& output,
    SemisimpleLieAlgebra& owner
  );
  static bool innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
    Calculator& calculator,
    const Expression& input,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
    SemisimpleLieAlgebra& owner
  );
  static bool innerDynkinTypE(Calculator& calculator, const Expression& input, DynkinType& output);
  static bool functionDynkinType(Calculator& calculator, const Expression& input, DynkinType& output);
  static bool innerDynkinSimpleTypE(Calculator& calculator, const Expression& input, DynkinSimpleType& output);
  static bool functionDynkinSimpleType(Calculator& calculator, const Expression& input, DynkinSimpleType& output);
  static bool innerSlTwoSubalgebraPrecomputed(Calculator& calculator, const Expression& input, SlTwoSubalgebra& output);
  static bool innerLoadFromObject(Calculator& calculator, const Expression& input, RationalFunction<Rational>& output);
  static bool innerAlgebraicNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPolynomialModuloInteger(Calculator& calculator, const Expression& input, Expression& output);
  template <class Coefficient>
  static bool innerPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  // Conversions from expression tree to expression containing type.
  template <class Coefficient>
  static bool functionPolynomial(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorConversions::functionPolynomialWithExponentLimit<Coefficient, - 1, - 1>(calculator, input, output);
  }
  template <class Coefficient, int MaximumPower, int MaximumVariables>
  static bool functionPolynomialWithExponentLimit(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerRationalFunction(Calculator& calculator, const Expression& input, Expression& output);
  template <class Coefficient>
  static bool functionRationalFunction(
    Calculator& calculator,
    const Expression& input,
    Expression& output
  );
  static bool innerElementUE(
    Calculator& calculator, const Expression& input, Expression& output, SemisimpleLieAlgebra& inputOwner
  );
  static bool outerMatrixExpressionsToMatrixOfType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerMakeMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMatrixDouble(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMatrixAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMatrixRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMatrixRationalFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMatrixRationalFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMatrixRationalTensorForM(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMatrixRationalTensorForm(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLoadFileIntoString(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMakeElementHyperOctahedral(Calculator& calculator, const Expression& input, Expression& output);
  ////////////////////Conversion to expression tree/////////////////////////////////////
  //converstion from type to expression tree.
  template <class Coefficient>
  static bool expressionFromPolynomial(
    Calculator& calculator,
    const Polynomial<Coefficient>& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  template <class Coefficient>
  static bool expressionFromRationalFunction(
    Calculator& calculator,
    const RationalFunction<Coefficient>& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  template <class Coefficient>
  static bool expressionFromRationalFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerLoadKey(
    Calculator& calculator,
    const Expression& inputStatementList,
    const std::string& inputKey,
    Expression& output
  );
  static bool innerLoadKeysFromStatementList(
    Calculator& calculator,
    const Expression& input,
    MapList<std::string, Expression, MathRoutines::hashString>& output,
    std::stringstream* commentsOnFailure = nullptr,
    bool allowFailure = false
  );
  static bool innerLoadKeysFromStatementList(
    Calculator& calculator,
    const Expression& input,
    MapList<Expression, Expression>& output,
    std::stringstream* commentsOnFailure = nullptr,
    bool allowFailure = false
  );
  static bool innerStoreSemisimpleSubalgebras(
    Calculator& calculator, const SemisimpleSubalgebras& input, Expression& output
  );
  static std::string innerStringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input);

  static bool innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
    Calculator& calculator, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output
  );
  static bool innerExpressionFromUE(
    Calculator& calculator,
    const ElementUniversalEnveloping<RationalFunction<Rational> >& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  static bool innerStoreCandidateSubalgebra(Calculator& calculator, const CandidateSemisimpleSubalgebra& input, Expression& output);
  static bool innerExpressionFromDynkinType(Calculator& calculator, const DynkinType& input, Expression& output);
  static bool innerExpressionFromDynkinSimpleType(Calculator& calculator, const DynkinSimpleType& input, Expression& output);
  static bool innerExpressionFromElementSemisimpleLieAlgebraRationals(
    Calculator& calculator, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output
  );
  //conversions from expression containing type to expression tree
  static bool innerStoreSemisimpleLieAlgebra(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionSemisimpleLieAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionSemisimpleLieAlgebra(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    SemisimpleLieAlgebra*& outputSSalgebra
  );
  static bool innerSemisimpleLieAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerCandidateSubalgebraPrecomputed(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    CandidateSemisimpleSubalgebra& outputPointer,
    SemisimpleSubalgebras& owner
  );
  static bool innerLoadWeylGroup(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerStoreSemisimpleSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerSlTwoSubalgebraPrecomputed(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLoadSemisimpleSubalgebras(Calculator& calculator, const Expression& inpuT, Expression& output);
  static bool innerExpressionFromChevalleyGenerator(
    Calculator& calculator, const ChevalleyGenerator& input, Expression& output
  );
  static bool innerExpressionFromMonomialUE(
    Calculator& calculator,
    const MonomialUniversalEnveloping<RationalFunction<Rational> >& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  static bool functionExpressionFromBuiltInType(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerExpressionFromBuiltInType(Calculator& calculator, const Expression& input, Expression& output);
  template <class Coefficient>
  static bool functionExpressionFromPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerExpressionFromUE(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerExpressionFrom(Calculator& calculator, const MonomialP& input, Expression& output);
  // TODO: move to calculator conversions.
  template <class theType>
  static bool convertToTypeUsingFunction(
    Expression::FunctionAddress theFun, const Expression& input, Expression& output
  ) {
    MacroRegisterFunctionWithName("Calculator::convertToTypeUsingFunction");
    if (input.isOfType<theType>()) {
      output = input;
      return true;
    }
    if (theFun == nullptr) {
      return false;
    }
    Calculator& calculator = *input.owner;
    if (!theFun(calculator, input, output)) {
      calculator.comments << "<hr>Conversion function failed on " << input.toString() << ". ";
      return false;
    }
    return output.isOfType<theType>();
  }
  template <class theType>
  static bool convert(
    const Expression& input,
    Expression::FunctionAddress conversion,
    WithContext<theType>& output
  ) {
    MacroRegisterFunctionWithName("CalculatorConversions::convert");
    Expression conversionExpression;
    if (!CalculatorConversions::convertToTypeUsingFunction<theType>(conversion, input, conversionExpression)) {
      return false;
    }
    if (!conversionExpression.isOfType(&output.content)) {
      return false;
    }
    output.context = conversionExpression.getContext();
    return true;
  }
};

template <class theType>
bool Calculator::getVector(
  const Expression& input,
  Vector<theType>& output,
  ExpressionContext* inputOutputStartingContext,
  int targetDimensionNonMandatory,
  Expression::FunctionAddress conversionFunction
) {
  MacroRegisterFunctionWithName("Calculator::getVector");
  List<Expression> nonConvertedEs;
  if (!this->getVectorExpressions(input, nonConvertedEs, targetDimensionNonMandatory)) {
    return false;
  }
  List<Expression> convertedEs;
  convertedEs.setSize(nonConvertedEs.size);
  for (int i = 0; i < nonConvertedEs.size; i ++) {
    if (!CalculatorConversions::convertToTypeUsingFunction<theType>(
      conversionFunction, nonConvertedEs[i], convertedEs[i]
    )) {
      return false;
    }
  }
  if (!this->convertExpressionsToCommonContext(convertedEs, inputOutputStartingContext)) {
    return false;
  }
  if (targetDimensionNonMandatory > 0) {
    if (convertedEs.size != targetDimensionNonMandatory) {
      return false;
    }
  }
  output.setSize(convertedEs.size);
  for (int i = 0; i < convertedEs.size; i ++) {
    output[i] = convertedEs[i].getValue<theType>();
  }
  return true;
}

template <class Coefficient>
bool CalculatorConversions::functionExpressionFromPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionExpressionFromPolynomial");
  if (!input.isOfType<Polynomial<Coefficient> >()) {
    return false;
  }
  const Polynomial<Coefficient>& polynomial = input.getValue<Polynomial<Coefficient> >();
  ExpressionContext context = input.getContext();
  return CalculatorConversions::expressionFromPolynomial(calculator, polynomial, output, &context);
}

template <class theType>
bool Expression::isMatrixOfType(int* outputNumRows, int* outputNumCols) const {
  if (!this->isMatrix()) {
    return false;
  }
  if (!(*this)[0].startsWith(this->owner->opMatrix(), 2)) {
    return false;
  }
  if (!(*this)[0][1].isOperationGiven(this->getTypeOperation<theType>())) {
    return false;
  }
  const Expression& rows = (*this)[1];
  if (!rows.isSequenceNElements()) {
    return false;
  }
  if (rows.size() < 2) {
    return false;
  }
  const Expression& firstRow = rows[1];
  if (!firstRow.isSequenceNElements()) {
    return false;
  }
  if (firstRow.size() < 2) {
    return false;
  }
  if (outputNumRows != nullptr) {
    *outputNumRows = rows.size() - 1;
  }
  if (outputNumCols != nullptr) {
    *outputNumCols = firstRow.size() - 1;
  }
  return true;
}

template <class theType>
bool Expression::isOfTypeWithContext(WithContext<theType>* whichElement) const {
  MacroRegisterFunctionWithName("Expression::isOfTypeWithContext");
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->getTypeOperation<theType>())) {
    return false;
  }
  if (this->size() < 2 || !this->getLastChild().isAtom()) {
    return false;
  }
  if (whichElement == 0) {
    return true;
  }
  whichElement->context = this->getContext();
  whichElement->content = this->getValue<theType>();
  return true;
}

template <class theType>
bool Calculator::functionGetMatrix(
  const Expression& input,
  Matrix<theType>& outputMatrix,
  ExpressionContext* inputOutputStartingContext,
  int targetNumColsNonMandatory,
  Expression::FunctionAddress conversionFunction,
  std::stringstream* commentsOnError
) {
  MacroRegisterFunctionWithName("Calculator::functionGetMatrix");
  Matrix<Expression> nonConvertedEs, convertedEs;
  if (!this->getMatrixExpressions(
    input, nonConvertedEs, - 1, targetNumColsNonMandatory
  )) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to extract matrix of expressions from "
      << input.toString();
    }
    return false;
  }
  convertedEs.initialize(nonConvertedEs.numberOfRows, nonConvertedEs.numberOfColumns);
  ExpressionContext theContext(*this);
  if (inputOutputStartingContext != nullptr) {
    theContext = *inputOutputStartingContext;
  }
  for (int i = 0; i < nonConvertedEs.numberOfRows; i ++) {
    for (int j = 0; j < nonConvertedEs.numberOfColumns; j ++) {
      if (!CalculatorConversions::convertToTypeUsingFunction<theType>(
        conversionFunction, nonConvertedEs(i, j), convertedEs(i, j)
      )) {
        if (!nonConvertedEs(i, j).convertInternally<theType>(convertedEs.elements[i][j])) {
          if (commentsOnError != nullptr) {
            *commentsOnError << "Failed to convert matrix element: "
            << "row: " << i << ", column: "
            << j << ", expression: "
            << nonConvertedEs(i, j).toString() << ". ";
          }
          return false;
        }
      }
      theContext.mergeContexts(convertedEs(i, j).getContext(), theContext);
    }
  }
  for (int i = 0; i < convertedEs.numberOfRows; i ++) {
    for (int j = 0; j < convertedEs.numberOfColumns; j ++) {
      if (!convertedEs(i, j).setContextAtLeastEqualTo(theContext, commentsOnError)) {
        if (commentsOnError != nullptr) {
          *commentsOnError
          << "Failed to set context to matrix element: "
          << "row: " << i << ", column: "
          << j << ", expression: "
          << convertedEs.elements[i][j].toString()
          << ". ";
        }
        return false;
      }
    }
  }
  outputMatrix.initialize(convertedEs.numberOfRows, convertedEs.numberOfColumns);
  for (int i = 0; i < convertedEs.numberOfRows; i ++) {
    for (int j = 0; j < convertedEs.numberOfColumns; j ++) {
      outputMatrix(i, j) = convertedEs(i, j).getValue<theType>();
    }
  }
  if (inputOutputStartingContext != nullptr) {
    *inputOutputStartingContext = theContext;
  }
  return true;
}

template <class Coefficient>
bool Expression::makeSum(Calculator& calculator,
  const LinearCombination<Expression, Coefficient>& summands
) {
  MacroRegisterFunctionWithName("Expression::makeSum");
  Expression oneE; //used to record the constant term
  oneE.assignValue<Rational>(1, calculator);
  if (summands.isEqualToZero()) {
    return this->assignValue<Rational>(0, calculator);
  }
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.setSize(summands.size());
  for (int i = 0; i < summands.size(); i ++) {
    Expression& current = summandsWithCoeff[i];
    if (summands[i] == oneE) {
      current.assignValue(summands.coefficients[i], calculator);
    } else if (!(summands.coefficients[i] == 1)) {
      current.reset(calculator, 3);
      current.addChildAtomOnTop(calculator.opTimes());
      current.addChildValueOnTop(summands.coefficients[i]);
      current.addChildOnTop(summands[i]);
    } else {
      current = summands[i];
    }
  }
  if (summandsWithCoeff.size < 5) {
    for (int i = 0; i < summandsWithCoeff.size; i ++) {
      for (int j = i; j < summandsWithCoeff.size; j ++) {
        if (
          summandsWithCoeff[i] > summandsWithCoeff[j] &&
          summandsWithCoeff[j] > summandsWithCoeff[i]
        ) {
          global.fatal << "Bad comparison "
          << "function: each of the expressions "
          << summandsWithCoeff[i].toString()
          << " and " << summandsWithCoeff[j].toString()
          << " is reported to be greater than the other. " << global.fatal;
        }
      }
    }
  }
  return this->makeOXdotsX(calculator, calculator.opPlus(), summandsWithCoeff);
}

template< >
int Expression::getTypeOperation<Rational>() const;

template < >
int Expression::addObjectReturnIndex(const
Rational
& inputValue) const;

template <class theType>
bool Expression::assignValueWithContext(
  const theType& inputValue,
  const ExpressionContext& theContext,
  Calculator& owner
) {
  this->reset(owner, 3);
  this->addChildAtomOnTop(this->getTypeOperation<theType>());
  this->addChildOnTop(theContext.toExpression());
  return this->addChildAtomOnTop(this->addObjectReturnIndex(inputValue));
}

template <class theType>
bool Expression::assignValue(const theType& inputValue, Calculator& owner) {
  Expression typeComputer;
  typeComputer.owner = &owner;
  int currentType = typeComputer.getTypeOperation<theType>();
  if (
    currentType == owner.opEltZmodP() ||
    currentType == owner.opPolynomialRational() ||
    currentType == owner.opRationalFunction() ||
    currentType == owner.opElementTensorGVM() ||
    currentType == owner.opElementUEOverRF() ||
    currentType == owner.opElementWeylAlgebra() ||
    currentType == owner.opWeightLieAlgPoly()
  ) {
    global.fatal << "This may or may not be a programming error. "
    << "Assigning value WITHOUT CONTEXT to data type "
    << owner.getOperations()[currentType]
    << " is discouraged, and most likely is an error. Crashing to let you know. "
    << global.fatal;
  }
  ExpressionContext emptyContext(owner);
  return this->assignValueWithContext(inputValue, emptyContext, owner);
}

template <class theType>
bool Expression::mergeContextsMyArumentsAndConvertThem(
  Expression& output, std::stringstream* commentsOnFailure
) const {
  MacroRegisterFunctionWithName("Expression::mergeContextsMyArumentsAndConvertThem");
  this->checkInitialization();
  Expression mergedContexts;
  if (!this->mergeContextsMyAruments(mergedContexts, commentsOnFailure)) {
    return false;
  }
  output.reset(*this->owner, this->children.size);
  output.addChildOnTop((*this)[0]);
  Expression convertedExpression;
  for (int i = 1; i < mergedContexts.size(); i ++) {
    if (!mergedContexts[i].convertInternally<theType>(convertedExpression)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "<hr>Failed to convert "
        << mergedContexts[i].toString() << " to the desired type. ";
      }
      return false;
    }
    output.addChildOnTop(convertedExpression);
  }
  return true;
}

template<class Coefficient>
bool Calculator::getTypeWeight(
  Calculator& calculator,
  const Expression& input,
  Vector<Coefficient>& outputWeightSimpleCoords,
  WithContext<SemisimpleLieAlgebra*>& outputAmbientSemisimpleLieAlgebra,
  Expression::FunctionAddress ConversionFun
) {
  MacroRegisterFunctionWithName("Calculator::getTypeWeight");
  if (input.size() != 3) {
    return calculator
    << "Function typeHighestWeightParabolic is expected to have two arguments: "
    << "Semisimple algebra type, highest weight in simple coordinates. ";
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  if (!CalculatorConversions::convert(
    leftE,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    outputAmbientSemisimpleLieAlgebra
  )) {
    calculator << "Error extracting Lie algebra from " << leftE.toString();
    return false;
  }
  SemisimpleLieAlgebra* ambientSSalgebra = outputAmbientSemisimpleLieAlgebra.content;
  if (!calculator.getVector<Coefficient>(
    middleE,
    outputWeightSimpleCoords,
    &outputAmbientSemisimpleLieAlgebra.context,
    ambientSSalgebra->getRank(),
    ConversionFun
  )) {
    calculator << "Failed to convert the second "
    << "argument of HWV to a list of " << ambientSSalgebra->getRank()
    << " polynomials. The second argument you gave is "
    << middleE.toString() << ".";
    return false;
  }
  if (!calculator.objectContainer.semisimpleLieAlgebras.contains(
    ambientSSalgebra->theWeyl.theDynkinType
  )) {
    global.fatal
    << ambientSSalgebra->toStringLieAlgebraName()
    << " contained object container more than once. " << global.fatal;
  }
  int algebraIndex = calculator.objectContainer.semisimpleLieAlgebras.getIndex(
    ambientSSalgebra->theWeyl.theDynkinType
  );
  outputAmbientSemisimpleLieAlgebra.context.setIndexAmbientSemisimpleLieAlgebra(algebraIndex);
  return true;
}

template <class builtInType>
void Calculator::addOneBuiltInHandler() {
  Expression typeConverter(*this);
  this->addOneStringHandler(
    typeConverter.getTypeOperation<builtInType>(),
    Expression::toStringBuiltIn<builtInType>,
    this->toStringDataHandlers
  );
}

template<class Coefficient>
bool Calculator::getTypeHighestWeightParabolic(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vector<Coefficient>& outputWeightHWFundcoords,
  Selection& outputInducingSelection,
  WithContext<SemisimpleLieAlgebra*>& outputAmbientSSalgebra,
  Expression::FunctionAddress ConversionFun
) {
  if (!input.isListNElements(4) && !input.isListNElements(3)) {
    return output.makeError(
      "Function typeHighestWeightParabolic is "
      "expected to have two or three arguments: "
      "semisimple Lie algebra type, highest weight, "
      "[optional] parabolic selection. ",
      calculator
    );
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  if (!CalculatorConversions::convert(
    leftE,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    outputAmbientSSalgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* ambientSSalgebra = outputAmbientSSalgebra.content;
  if (!calculator.getVector<Coefficient>(
    middleE,
    outputWeightHWFundcoords,
    &outputAmbientSSalgebra.context,
    ambientSSalgebra->getRank(),
    ConversionFun
  )) {
    std::stringstream tempStream;
    tempStream
    << "Failed to convert the second argument of highest weight vector to a list of "
    << ambientSSalgebra->getRank()
    << " polynomials. The second argument you gave is "
    << middleE.toString() << ".";
    return output.makeError(tempStream.str(), calculator);
  }
  if (input.isListNElements(4)) {
    Vector<Rational> parabolicSel;
    const Expression& rightE = input[3];
    if (!calculator.getVector<Rational>(
      rightE,
      parabolicSel,
      &outputAmbientSSalgebra.context,
      ambientSSalgebra->getRank(),
      nullptr
    )) {
      std::stringstream tempStream;
      tempStream
      << "Failed to convert the third argument of highest weight vector to a list of "
      << ambientSSalgebra->getRank()
      << " rationals. The third argument you gave is "
      << rightE.toString() << ".";
      return output.makeError(tempStream.str(), calculator);
    }
    outputInducingSelection = parabolicSel;
  } else {
    outputInducingSelection.initialize(ambientSSalgebra->getRank());
    for (int i = 0; i < outputWeightHWFundcoords.size; i ++) {
      if (!outputWeightHWFundcoords[i].isSmallInteger()) {
        outputInducingSelection.addSelectionAppendNewIndex(i);
      }
    }
  }
  if (!calculator.objectContainer.semisimpleLieAlgebras.contains(
    ambientSSalgebra->theWeyl.theDynkinType
  )) {
    global.fatal
    << ambientSSalgebra->toStringLieAlgebraName()
    << " contained object container more than once. "
    << global.fatal;
  }
  int algebraIndex = calculator.objectContainer.semisimpleLieAlgebras.getIndex(
    ambientSSalgebra->theWeyl.theDynkinType
  );
  outputAmbientSSalgebra.context.setIndexAmbientSemisimpleLieAlgebra(algebraIndex);
  return true;
}

template <class Coefficient>
bool Expression::assignMatrix(
  const Matrix<Coefficient>& input,
  Calculator& owner,
  const ExpressionContext* inputContext,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix
) {
  MacroRegisterFunctionWithName("Expression::assignMatrix");
  Matrix<Expression> theMatEs;
  theMatEs.initialize(input.numberOfRows, input.numberOfColumns);
  Expression currentElt;
  for (int i = 0; i < input.numberOfRows; i ++) {
    for (int j = 0; j < input.numberOfColumns; j ++) {
      if (inputContext == nullptr) {
        currentElt.assignValue(input(i, j), owner);
      } else {
        currentElt.assignValueWithContext(input(i, j), *inputContext, owner);
      }
      theMatEs(i, j) = currentElt;
    }
  }
  return this->assignMatrixExpressions(
    theMatEs, owner, reduceOneRowToSequenceAndOneByOneToNonMatrix, false
  );
}

template <class Coefficient>
bool CalculatorConversions::expressionFromPolynomial(
  Calculator& calculator,
  const Polynomial<Coefficient>& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::expressionFromPolynomial");
  LinearCombination<Expression, Coefficient> theTerms;
  Expression currentBase, currentPower, currentTerm, currentMultTermE;
  if (!input.isConstant() && inputContext == nullptr) {
    calculator << "While converting polynomial to expression, "
    << "I was given no variable names. Using the "
    << "default variable names x_1, x_2, ... "
    << "Please make sure you are not using those "
    << "variables for other purposes.";
  }
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].isConstant()) {
      currentTerm.assignValue(1, calculator);
      theTerms.addMonomial(currentTerm, input.coefficients[i]);
      continue;
    }
    bool found = false;
    for (int j = 0; j < input[i].minimalNumberOfVariables(); j ++) {
      if (input[i](j) != 0) {
        if (inputContext != nullptr) {
          currentBase = inputContext->getVariable(j);
        } else {
          currentBase.reset(calculator);
          currentBase.addChildAtomOnTop("x");
          currentBase.addChildValueOnTop(Rational(j));
        }
        if (input[i](j) == 1) {
          currentMultTermE = currentBase;
        } else {
          currentPower.assignValue(input[i](j), calculator);
          currentMultTermE.makeXOX(
            calculator, calculator.opThePower(), currentBase, currentPower
          );
        }
        if (!found) {
          currentTerm = currentMultTermE;
        } else {
          currentTerm *= currentMultTermE;
        }
        found = true;
      }
    }
    theTerms.addMonomial(currentTerm, input.coefficients[i]);
  }
  return output.makeSum(calculator, theTerms);
}

template <class Coefficient>
bool CalculatorConversions::expressionFromRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::expressionFromRationalFunction");
  if (!input.isOfType<RationalFunction<Coefficient> >()) {
    return false;
  }
  const RationalFunction<Coefficient>& rationalFunction = input.getValue<RationalFunction<Coefficient> >();
  ExpressionContext context = input.getContext();
  return CalculatorConversions::expressionFromRationalFunction<Coefficient>(
    calculator, rationalFunction, output, &context
  );
}

template <class Coefficient>
bool CalculatorConversions::expressionFromRationalFunction(
  Calculator& calculator,
  const RationalFunction<Coefficient>& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::expressionFromRationalFunction");
  Rational aConst;
  if (input.isConstant(&aConst)) {
    return output.assignValue(aConst, calculator);
  }
  Polynomial<Coefficient> numerator, denominator;
  input.getNumerator(numerator);

  if (input.isConstant() || input.expressionType == input.typePolynomial) {
    return CalculatorConversions::expressionFromPolynomial<Coefficient>(calculator, numerator, output, inputContext);
  }
  Expression numeratorExpression, denominatorExpression;
  input.getDenominator(denominator);
  Polynomial<Coefficient> numeratorRescaled = numerator;
  Polynomial<Coefficient> denominatorRescaled = denominator;
  Coefficient topMultiple = numeratorRescaled.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Coefficient bottomMultiple = denominatorRescaled.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Coefficient multipleTopBottom = bottomMultiple / topMultiple;
  numeratorRescaled *= multipleTopBottom.getNumerator();
  denominatorRescaled *= multipleTopBottom.getDenominator();
  CalculatorConversions::expressionFromPolynomial<Coefficient>(calculator, numeratorRescaled, numeratorExpression, inputContext);
  CalculatorConversions::expressionFromPolynomial<Coefficient>(calculator, denominatorRescaled, denominatorExpression, inputContext);
  return output.makeXOX(calculator, calculator.opDivide(), numeratorExpression, denominatorExpression);
}

template <class BuiltIn>
bool WithContext<BuiltIn>::setContextAtLeast(
  ExpressionContext& inputOutputContext,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("WithContext::setContextAtLeast");
  ExpressionContext newContext;
  if (!inputOutputContext.mergeContexts(this->context, newContext)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to merge context "
      << inputOutputContext.toString()
      << " with " << this->context.toString();
    }
    return false;
  }
  inputOutputContext = newContext;
  return this->extendContext(inputOutputContext, commentsOnFailure);
}

template <class BuiltIn>
bool WithContext<BuiltIn>::setContextAndSerialize(
  ExpressionContext& inputOutputContext,
  Expression& output,
  std::stringstream* commentsOnFailure
) {
  if (!this->setContextAtLeast(inputOutputContext, commentsOnFailure)) {
    return false;
  }
  return output.assignWithContext(*this, *this->context.owner);
}

template <class BuiltIn>
bool WithContext<BuiltIn>::mergeContexts(
  WithContext<BuiltIn>& leftOutput,
  WithContext<BuiltIn>& rightOutput,
  std::stringstream* commentsOnFailure
) {
  ExpressionContext resultContext;
  if (!leftOutput.context.mergeContexts(rightOutput.context, resultContext)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to merge contexts.";
    }
    return false;
  }
  return
  leftOutput.extendContext(resultContext, commentsOnFailure) &&
  rightOutput.extendContext(resultContext, commentsOnFailure);
}
#endif
