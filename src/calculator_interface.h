
#ifndef header_calculator_interface_ALREADY_INCLUDED
#define header_calculator_interface_ALREADY_INCLUDED

#include "math_general.h"
#include "math_extra_finite_groups.h"
#include "math_extra_graph.h"
#include "math_extra_algebraic_numbers.h"
#include "general_list_references.h"
#include "math_extra_semisimple_lie_subalgebras.h"
#include "math_extra_universal_enveloping.h"
#include "math_extra_modules_semisimple_lie_algebras.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_elliptic_curves_implementation.h"

template <class builtIn>
class WithContext;
class ExpressionContext;

class Expression {
private:
  HashedList<int, HashFunctions::hashFunction> children;
  void reset() {
    this->owner = nullptr;
    this->children.size = 0;
    this->data = - 1;
  }
  static const int maximumCharactersInLatexPrintout = 30001;
  bool setChild(int childIndexInMe, int childIndexInBoss);
  // Definitions.
  // 1. Fundamentals.
  // 1.1. An atom is an expression with zero children.
  // 1.2. At atom whose data entry is a valid index of
  //      Calculator::operations will also be called an operation.
  //      Let the string corresponding to this index be X.
  //      Then, we say that "at atom equals the operation X".
  //      Note that this is ambiguous:
  //      data entry of an atom can be interpreted either
  //      as an operation or as an an actual piece of data,
  //      not necessarily an index of Calculator::operations.
  //      Whenever this ambiguity becomes an issue
  //      explicit reference to the Expression::data
  //      and Expression::children members should be made.
  // 1.3. A list is an expression with 1 or more children whose data entry
  //      equals 0. The atom 0 *MUST* be equal to Calculator::opList().
  // 1.4. An expression with 1 or more children is
  //      not allowed to have data entry different
  //      from 0 = Calculator::opList(). The system is instructed to
  //      crash and burn shall such a configuration be detected.
  //      Rationale: keep the system simple,
  //      at the cost of leaving data entry non-used for non-atomic expressions.
  // 2. Basic building blocks
  // 2.1. A frozen expression is a list whose first entry is an atom equal to
  // Freeze.
  // 2.2. A sequence is a list whose first entry is an atom equal to Sequence.
  // 2.3. A bound variable is a list with two atomic entries, the first of
  // which
  //      equals Bind.
  // 2.4. An error is a list with two entries whose first entry is an atom
  // equal
  // to Error, and whose second entry is a string.
  // *Note that Calculator::opList() is required to equal zero for reasons of
  // program speed.
  // This is GUARANTEED, and you MAY assume it.
  // If you want to have a list of mathematical objects, use the Sequence
  // data structure. A sequence is a List whose first entry is an atom whose
  // value
  // is Sequence = Calculator::opSequence().
  //
  // -------------------------------------------------------
  //
  // Expressions of built-in types.
  // 1. Expressions of built-in types represent hard-coded C++ types.
  // 2. An expression is said to be of built-in type if it is a list of 2 or 3
  // elements
  //    starting with an atom
  // equal to one of a set of hard-coded names, and ending in an atom whose
  // value
  // is an integer that uniquely (up to operator==) identifies a C++ structure
  // with the corresponding type.
  // 3. A context is a list of 1 or more elements starting with the atom
  // Context. If a context has only one child (which must then be
  // equal to the atom Context), then we say that we have an "empty context".
  // 4. An expression is said to have context if it is a list of at least two
  // elements, the second element of which is a context.
  // 5. If an expression of built-in type has 3 children, the middle child
  // must be a context. If an expression of built-in type has 2 children, we
  // say
  // that the expression does not have context.
  // 6. Two expressions of built-in type with equal types and C++ identifiers,
  // one having a context that is empty, and the other having no context,
  // are considered to represent one and the same element.
  // -------------------------------------------------------
  // -------------------------------------------------------
  // -------------------------------------------------------
  // Notes on Expression implementation.
  // While the preceding notes are to be considered as fixed, the following
  // notes are implementation specific and are subject to change.
  // The i^th child of an expression can be accessed as const using
  // operator[](i).
  // The children of an expression are kept as a list of integers indicating
  // the children's position in Calculator::expressionContainer.
  // Calculator::expressionContainer is a Hashed List of references and must
  // not be modified directly in any way.
  // Motivation for this implementation choice. The original implementation
  // had Expression contain all of its children as List<Expression>, making the
  // copy operator= a recursive function.
  // While this is one of the least-head-aching designs,
  // it also proved to be remarkably slow: here is why.
  // When processing an expression, one must traverse all of its subtrees.
  // Making temporary copies of all subtrees is then approximately O(n^2),
  // where n^2 is the number of nodes of the tree.
  // For a tree with 1k nodes this is already unacceptably slow.
  //
  // If most subtrees are subject to change, then the
  // approximate cost of O(n^2) operations seems justified.
  // However, in most computations, subtrees need not be changed - in reality,
  // most expression will arrive to the calculator in reduced
  // or close-to-reduced form.
  //
  // This explains our choice of keeping all children of an expression as a
  // reference to an ever-growing collection of Expressions.
  // If a single instance of Calculator is to run over long periods of time,
  // it is possible that not all such expressions are in use, and we run out of
  // RAM memory due to our fault [bad], rather than because the
  // user requested a large computation [good].
  // If that is to happen, some sort of garbage collection will have to be
  // implemented. However, currently the calculator resets after
  // each user-facing operation, so this should not present a practical
  // problem.
public:
  Calculator* owner;
  int data;
  bool flagDeallocated;
  // ////
  typedef bool(*FunctionAddress)(
    Calculator& calculator, const Expression& input, Expression& output
  );
  typedef bool(*ToStringHandler)(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  // ////
  friend std::ostream& operator<<(
    std::ostream& output, const Expression& element
  ) {
    output << element.toString();
    return output;
  }
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
  bool addChildAtomOnTop(const std::string& operationString);
  bool addChildAtomOnTop(int operationIndex);
  void getBlocksOfCommutativity(HashedList<Expression>& inputOutputList) const;
  bool splitProduct(
    int numDesiredMultiplicandsLeft,
    Expression& outputLeftMultiplicand,
    Expression& outputRightMultiplicand
  ) const;
  void getBaseExponentForm(
    Expression& outputBase, Expression& outputExponent
  ) const;
  void getMultiplicandsRecursive(
    List<Expression>& outputAppendList, int depth = 0
  ) const;
  void getMultiplicandsDivisorsRecursive(
    List<Expression>& outputAppendList, int depth = 0
  ) const;
  void getCoefficientMultiplicandForm(
    Expression& outputCoefficient, Expression& outputNoCoefficient
  ) const;
  void getCoefficientMultiplicandForm(
    Rational& outputCoeff, Expression& outputNoCoeff
  ) const;
  bool setChildAtomValue(int childIndex, const std::string& atom);
  bool setChildAtomValue(int childIndex, int atomValue);
  void swapChildren(int left, int right);
  void removeLastChild();
  int size() const {
    return this->children.size;
  }
  void removeChildShiftDown(int childIndex);
  void setSize(int desiredSize);
  void setExpectedSize(int expectedSize);
  void addChildIndices(List<int>& indices);
  bool setChild(int childIndexInMe, const Expression& inputChild);
  bool assignMatrixExpressions(
    const Matrix<Expression>& input,
    Calculator& owner,
    bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
    bool dontReduceTypes
  );
  bool divisionByMeShouldBeWrittenInExponentForm() const;
  bool isCalculatorStatusChanger() const;
  bool isList() const;
  bool isListNElements(int N = - 1) const;
  bool startsWithBuiltInAtom() const;
  bool startsWithFunctionWithComplexRange() const;
  bool startsWithArithmeticOperation() const;
  bool startsWith(int operation = - 1, int numberOfChildren = - 1) const;
  bool startsWithGivenOperation(
    const std::string& operation, int desiredChildren = - 1
  ) const;
  bool isListStartingWithAtom(int operation = - 1) const;
  bool isListOfTwoAtomsStartingWith(int operation) const;
  bool isFrozen() const;
  bool isAtomThatFreezesArguments(std::string* outputWhichAtom = nullptr)
  const;
  bool isAtomWhoseExponentsAreInterpretedAsFunction(
    std::string* outputWhichAtom = nullptr
  ) const;
  bool isPowerOfAtomWhoseExponentsAreInterpretedAsFunction() const;
  bool isAtomNotInterpretedAsFunction(
    std::string* outputWhichAtom = nullptr
  ) const;
  bool isMatrix(
    int* outputNumberOfRows = nullptr, int* outputNumberOfColumns = nullptr
  ) const;
  template <class Type>
  bool isMatrixOfType(
    int* outputNumberOfRows = nullptr, int* outputNumberOfColumns = nullptr
  ) const;
  bool isAtom() const;
  bool isAtomUserDefined(std::string* whichAtom = nullptr) const;
  bool isOperationGiven(const std::string& desiredAtom) const;
  bool isOperationGiven(int desiredDataUseMinusOneForAny = - 1) const;
  bool isOperation(int& outputWhichOperationIndex) const;
  bool isOperation(std::string* outputWhichOperation = nullptr) const;
  bool isBuiltInAtom(std::string* outputWhichOperation = nullptr) const;
  bool isGoodForChainRuleFunction(
    std::string* outputWhichOperation = nullptr
  ) const;
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
    Expression* outputCoefficientInFrontOfDifferential = nullptr
  ) const;
  bool isKnownFunctionWithComplexRange(
    std::string* outputWhichOperation = nullptr
  ) const;
  bool isArithmeticOperation(std::string* outputWhichOperation = nullptr)
  const;
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
  bool needsParenthesisForMultiplication(FormatExpressions* format = nullptr)
  const;
  bool needsParenthesisForAddition() const;
  bool needsParenthesisForMultiplicationWhenSittingOnTheRightMost(
    const Expression* leftNeighbor = nullptr
  ) const;
  int getExpressionTreeSize() const;
  template <class Type>
  bool isOfType() const {
    STACK_TRACE("Expression::isOfType");
    if (this->owner == nullptr) {
      return false;
    }
    if (!this->startsWith(this->getBuiltInType<Type>())) {
      return false;
    }
    if (this->children.size < 2 || !this->getLastChild().isAtom()) {
      return false;
    }
    return true;
  }
  template <class Type>
  bool hasType() const {
    if (this->isOfType<Type>()) {
      return true;
    }
    if (this->isElementaryObject()) {
      return false;
    }
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].hasType<Type>()) {
        return true;
      }
    }
    return false;
  }
  template <class Type>
  bool isOfType(Type* whichElement) const {
    STACK_TRACE("Expression::isOfType");
    if (this->owner == nullptr) {
      return false;
    }
    if (!this->startsWith(this->getBuiltInType<Type>())) {
      return false;
    }
    if (this->children.size < 2 || !this->getLastChild().isAtom()) {
      return false;
    }
    if (whichElement == 0) {
      return true;
    }
    *whichElement = this->getValue<Type>();
    return true;
  }
  template <class Type>
  bool isOfTypeWithContext(WithContext<Type>* whichElement) const;
  template <class Type>
  const Type& getValue() const {
    return this->getValueNonConst<Type>();
  }
  template <class Type>
  Type& getValueNonConst() const;
  template <class Type>
  int getBuiltInType() const;
  template <class Type>
  int addObjectReturnIndex(const Type& inputValue) const;
  // Please keep the Calculator as the first argument for all
  // assign*(...) functions.
  bool assignError(Calculator& owner, const std::string& error);
  // note: the following always returns true:
  template <class Type>
  bool assignValue(Calculator& owner, const Type& inputValue);
  // note: the following always returns true:
  template <class Type>
  bool assignValueWithContext(
    Calculator& owner,
    const Type& inputValue,
    const ExpressionContext& context
  );
  // Same as assignValueWithContext but with shuffled arguments. Deprecated.
  template <class BuiltIn>
  bool assignWithContext(
    Calculator& owner, const WithContext<BuiltIn>& input
  ) {
    return
    this->assignValueWithContext(owner, input.content, input.context);
  }
  template <class Type>
  bool addChildValueOnTop(const Type& inputValue) {
    this->checkInitialization();
    Expression child;
    child.assignValue(*this->owner, inputValue);
    child.checkConsistency();
    return this->addChildOnTop(child);
  }
  bool setContextAtLeastEqualTo(
    ExpressionContext& inputOutputMinContext,
    std::stringstream* commentsOnFailure
  );
  bool hasContext() const;
  bool hasNonEmptyContext() const;
  ExpressionContext getContext() const;
  bool getContext(ExpressionContext& output) const;
  static bool mergeContexts(Expression& leftE, Expression& rightE);
  bool mergeContextsMyAruments(
    Expression& output, std::stringstream* commentsOnFailure
  ) const;
  template <class Type>
  bool mergeContextsMyArumentsAndConvertThem(
    Expression& output, std::stringstream* commentsOnFailure
  ) const;
  bool containsAsSubExpressionNoBuiltInTypes(const Expression& input) const;
  bool containsAsSubExpressionNoBuiltInTypes(int inputAtom) const;
  bool contextSetDifferentialOperatorVariable(
    const Expression& polynomialVariable,
    const Expression& differentialOperatorVariable
  );
  SemisimpleLieAlgebra* getAmbientSemisimpleLieAlgebraNonConstUseWithCaution()
  const;
  bool isEqualToZero() const;
  bool isEqualToOne() const;
  bool isEqualToTwo() const;
  bool isEqualToHalf() const;
  bool isEqualToMOne() const;
  bool isKnownToBeNonNegative() const;
  bool isNegativeConstant() const;
  bool makeIdentityMatrixExpressions(int dimension, Calculator& inputBoss);
  // bool makeAtom(int input, Calculator& newBoss);
  bool makeAtom(Calculator& newBoss, int input);
  // TODO(tmilev): rename to MakeOperation
  bool makeAtom(Calculator& newBoss, const std::string& atomName);
  bool makeIntegral(
    Calculator& calculator,
    const Expression& integrationSet,
    const Expression& function,
    const Expression& variable
  );
  template <class Coefficient>
  bool makeSum(
    Calculator& calculator,
    const LinearCombination<Expression, Coefficient>& summands
  );
  bool makeSum(Calculator& calculator, const List<Expression>& summands);
  bool makeProduct(
    Calculator& owner, const List<Expression>& multiplicands
  );
  bool makeProduct(
    Calculator& owner, const Expression& left, const Expression& right
  );
  bool makeProductReduceOnes(
    Calculator& owner, const List<Expression>& multiplicands
  );
  bool makeQuotientReduce(
    Calculator& owner,
    const Expression& numerator,
    const Expression& denominator
  );
  int getNumberOfColumns() const;
  bool makeSequenceCommands(
    Calculator& owner,
    List<std::string>& inputKeys,
    List<Expression>& inputValues
  );
  bool makeSequenceStatements(
    Calculator& owner, List<Expression>* inputStatements = nullptr
  );
  template <class Coefficient>
  bool makeMatrix(
    Calculator& owner,
    const Matrix<Coefficient>& input,
    ExpressionContext const*inputContext = nullptr,
    bool reduceOneRowToSequenceAndOneByOneToNonMatrix = true
  );
  bool makeMatrix(Matrix<Expression>* inputMatrix, Calculator& owner);
  bool makeSequence(
    Calculator& owner, List<Expression>* inputSequence = nullptr
  );
  bool makeExponentReduce(
    Calculator& owner, const Expression& base, int power
  );
  bool makeXOX(
    Calculator& owner,
    int operation,
    const Expression& left,
    const Expression& right
  );
  bool makeSqrt(
    Calculator& owner,
    const Rational& argument,
    const Rational& radicalSuperIndex = 2
  );
  bool makeSqrt(
    Calculator& owner,
    const Expression& argument,
    const Rational& radicalSuperIndex = 2
  );
  bool makeXOXOdotsOX(
    Calculator& owner, int operation, const List<Expression>& opands
  );
  bool makeOXdotsX(
    Calculator& owner, int operation, const List<Expression>& opands
  );
  bool makeOX(
    Calculator& owner, int operation, const Expression& opArgument
  );
  bool sequencefy();
  bool isSuitableForSubstitution() const;
  bool isSuitableForRecursion() const;
  void substituteRecursively(
    const Expression& toBeSubstituted, const Expression& substituteWith
  );
  void substituteRecursivelyInChildren(
    const Expression& toBeSubstituted, const Expression& substituteWith
  );
  void assignXOXToChild(
    int childIndex,
    Calculator& owner,
    int operation,
    const Expression& left,
    const Expression& right
  ) {
    Expression tempE;
    tempE.makeXOX(owner, operation, left, right);
    this->setChild(childIndex, tempE);
  }
  std::string lispify() const;
  bool toStringData(
    std::stringstream& out, FormatExpressions* format = nullptr
  ) const;
  std::string toStringSemiFull() const;
  std::string toStringFull() const;
  std::string toStringFullWithHints(bool isBuiltInObjectIndex) const;
  std::string toStringAllSlidersInExpression() const;
  std::string toUTF8String(FormatExpressions* format = nullptr) const;
  template <class builtIn>
  static bool toStringBuiltIn(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  bool toStringWithAtomHandler(
    std::stringstream& out, FormatExpressions* format
  ) const;
  bool toStringWithCompositeHandler(
    std::stringstream& out, FormatExpressions* format
  ) const;
  static bool toStringLnAbsoluteInsteadOfLogarithm(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDifferential(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDifferential2(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDifferential3(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDifferentiate(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDefine(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIsDenotedBy(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLogBase(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringQuote(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDefineConditional(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringTensor(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIn(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringAnd(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringBinom(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringUnderscore(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringSetMinus(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLimitBoundary(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringCrossProduct(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringAbsoluteValue(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringBind(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringMod(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringPlus(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringMatrix(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringSequence(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLieBracket(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringSumOrIntegral(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringGreaterThan(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringGreaterThanOrEqualTo(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLessThanOrEqualTo(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDirectSum(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLimitProcess(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLessThan(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringLimit(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringTimes(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringSqrt(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringSqrt2(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringSqrt3(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringOr(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIntervalOpen(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIntervalLeftClosed(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIntervalRightClosed(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIntervalClosed(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringDivide(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringPower(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringFactorial(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringUnion(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringMinus(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringMinus2(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringMinus3(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringIntersection(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringUnionNoRepetition(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringEqualEqual(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringEqualEqualEqual(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  static bool toStringError(
    const Expression& input,
    std::stringstream& out,
    FormatExpressions* format
  );
  void toStringOpMultiplicative(
    std::stringstream& out,
    const std::string& operation,
    FormatExpressions* format
  ) const;
  std::string toStringTreeHtml(int depth) const;
  std::string toString(
    FormatExpressions* format = nullptr,
    Expression* startingExpression = nullptr,
    bool unfoldCommandEnclosures = true,
    JSData* outputJS = nullptr
  ) const;
  bool toStringGeneral(std::stringstream& out, FormatExpressions* format)
  const;
  std::string toStringWithStartingExpression(
    FormatExpressions* format,
    Expression* startingExpression,
    std::stringstream& out,
    JSData* outputJS
  ) const;
  bool toStringEndStatement(
    std::stringstream& out,
    Expression* startingExpression,
    JSData* outputJS,
    FormatExpressions* format
  ) const;
  bool toStringEndStatementOneRow(
    std::stringstream& out,
    Expression* startingExpression,
    JSData* outputJS,
    int index,
    bool isFinal,
    FormatExpressions& format
  ) const;
  bool requiresNoMathTags() const;
  static unsigned int hashFunction(const Expression& input);
  unsigned int hashFunction() const;
  Expression(): flagDeallocated(false) {
    this->reset();
  }
  Expression(int x): flagDeallocated(false) {
    STACK_TRACE("Expression::Expression(int)");
    this->reset();
    this->data = x;
  }
  const Expression& getLastChild() const {
    return (*this)[this->children.size - 1];
  }
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
  bool isIntegerNonNegative(LargeIntegerUnsigned* whichInteger = nullptr)
  const;
  bool isRational(Rational* whichRational = nullptr) const;
  bool isAlgebraicRadical() const;
  bool isConstantNumber() const;
  bool isPositiveNumber() const;
  bool evaluatesToDoubleInRange(
    const std::string& variableName,
    double lowBound,
    double highBound,
    int numberOfPoints,
    double* outputYmin = nullptr,
    double* outputYmax = nullptr,
    Vectors<double>* outputPoints = nullptr
  ) const;
  bool evaluatesToDouble(double* whichDouble = nullptr) const;
  bool getExpressionLeafs(HashedList<Expression>& outputAccumulateLeafs) const;
  bool getFreeVariables(
    HashedList<Expression>& outputAccumulateFreeVariables,
    bool excludeNamedConstants
  ) const;
  bool allowedAsFreeVariableAtom(const std::string& input) const;
  bool getBoundVariables(
    HashedList<Expression>& outputAccumulateBoundVariables
  ) const;
  bool evaluatesToDoubleUnderSubstitutions(
    const HashedList<Expression>& knownEs,
    const List<double>& valuesKnownEs,
    double* whichDouble = nullptr
  ) const;
  bool hasBoundVariables() const;
  bool hasInputBoxVariables(
    HashedList<std::string, HashFunctions::hashFunction>* outputBoxNames =
    nullptr,
    HashedList<std::string, HashFunctions::hashFunction>*
    outputBoxNamesJavascript =
    nullptr
  ) const;
  bool isMeltable(int* numberOfResultingChildren = nullptr) const;
  bool areEqualExcludingChildren(const Expression& other) const {
    return
    this->owner == other.owner &&
    this->data == other.data &&
    this->children.size == other.children.size;
  }
  // The following function creates an expression by parsing a calculator-like
  // string.
  // The purpose of this function is to reduce the number of lines needed to
  // create an Expression using C++.
  // Consider creating the expression f{}{{a}}={{a}}+ 1; f{}b;
  // We would need to create a large expression tree, so many calls of
  // Expression::makeXOX.
  // Instead, we can simply parse the expression from a string.
  // The inputExpressions give us the ability to specify substitutions
  bool assignStringParsed(
    const std::string& input,
    MapList<
      std::string,
      Expression,
      HashFunctions::hashFunction<std::string>
    >* substitutions,
    Calculator& owner
  );
  const Expression& operator[](int n) const;
  void operator=(const Expression& other) {
    this->data = other.data;
    this->children = other.children;
    this->owner = other.owner;
  }
  void operator=(const Rational& other) {
    STACK_TRACE("Expression::operator=(Rational)");
    this->checkInitialization();
    this->assignValue(*this->owner, other);
  }
  void operator=(int other) {
    STACK_TRACE("Expression::operator=(int)");
    this->checkInitialization();
    this->assignValue(*this->owner, Rational(other));
  }
  void operator/=(const Expression& other);
  void operator+=(const Expression& other);
  void operator-=(const Expression& other);
  Expression operator+(int other) const;
  friend Expression operator*(int left, const Expression& right);
  Expression operator-(int other) const;
  Expression operator*(int other) const;
  Expression operator/(int other) const;
  Expression operator+(const Expression& other) const;
  Expression operator-(const Expression& other) const;
  Expression operator*(const Expression& other) const;
  Expression operator/(const Expression& other) const;
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
    return !(*this == other);
  }
  // void operator=(const Expression& other);
  bool operator>(const Expression& other) const;
  bool greaterThanNoCoefficient(const Expression& other) const;
  void substituteRecursively(
    MapList<Expression, Expression>& substitutions
  );
  void substituteRecursivelyInChildren(
    MapList<Expression, Expression>& substitutions
  );
  class Test {
  public:
    static bool all();
    static bool toStringTestRecode(Calculator& owner);
    static bool toStringTestRecodeOnce(
      const std::string& inputHardCodedMustParse, Calculator& owner
    );
    static bool isUserDefinedAtomOnce(
      Calculator& owner, const std::string& input, bool isUserDefinedAtom
    );
    static bool isUserDefinedAtom(Calculator& owner);
  };
};

class ExpressionContext {
private:
  HashedList<Expression> variables;
  List<Expression> differentialOperatorVariables;
  LargeIntegerUnsigned defaultModulus;
  bool mergeSemisimpleLieAlgebraContexts(
    const ExpressionContext& other, ExpressionContext& outputContext
  );
  bool mergeModuli(
    const ExpressionContext& other, ExpressionContext& outputContext
  );
  bool mergeVariables(
    const ExpressionContext& other, ExpressionContext& outputContext
  );
  bool mergeDifferentialOperators(
    const ExpressionContext& other, ExpressionContext& outputContext
  );
  bool mergeDifferentialOperatorsOnce(
    Selection& differentialOperatorVariablesFound,
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
    const ExpressionContext& other, ExpressionContext& outputContext
  );
  void initialize(Calculator& inputOwner);
  void makeOneVariable(const Expression& inputVariable);
  void makeOneVariableOneDifferentialOperator(
    const Expression& inputVariable,
    const Expression& inputDifferentialOperatorVariable
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
  bool hasVariable(const Expression& input);
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
  bool extendContextTrivially(
    ExpressionContext& newContext, std::stringstream* commentsOnFailure
  ) {
    (void) commentsOnFailure;
    this->context = newContext;
    return true;
  }
public:
  ExpressionContext context;
  BuiltIn content;
  std::string toStringContentWithFormat() {
    FormatExpressions format;
    this->context.getFormat(format);
    return this->content.toString(&format);
  }
  WithContext() {}
  static bool mergeContexts(
    WithContext<BuiltIn>& leftOutput,
    WithContext<BuiltIn>& rightOutput,
    std::stringstream* commentsOnFailure
  );
  bool setContextAtLeast(
    ExpressionContext& inputOutputContext,
    std::stringstream* commentsOnFailure
  );
  bool setContextAndSerialize(
    ExpressionContext& inputOutputContext,
    Expression& output,
    std::stringstream* commentsOnFailure
  );
  std::string toString() const;
  bool toExpression(Calculator& calculator, Expression& output);
  // Transforms the content ands sets the new context.
  // Do not call directly
  // unless you can guarantee that the newContext is suitable:
  // it's variables must be a superset of the old context's variables.
  // As of writing, these are the only requirements that the
  // newContext should satisfy. Should these requirements increase,
  // it's the caller's responsibility to satisfy them.
  bool extendContext(
    ExpressionContext& newContext, std::stringstream* commentsOnFailure
  );
};

template < >
bool WithContext<Polynomial<AlgebraicNumber> >::toExpression(
  Calculator& calculator, Expression& output
);
template < >
bool WithContext<Polynomial<Rational> >::toExpression(
  Calculator& calculator, Expression& output
);
class Function {
private:
  // Used in constructors.
  void reset(Calculator& owner) {
    this->argumentTypes.reset(owner);
    this->owner = &owner;
    this->resetExceptOwner();
  }
  // Used in constructors.
  void resetExceptOwner() {
    this->functionAddress = nullptr;
    this->indexInOperationHandlers = - 1;
    this->indexOperation = - 1;
    this->parentsThatBanHandler.clear();
    this->options.reset();
  }
public:
  Calculator* owner;
  Expression argumentTypes;
  std::string description;
  std::string example;
  std::string calculatorIdentifier;
  std::string additionalIdentifier;
  int indexOperation;
  int indexInOperationHandlers;
  // List of direct parent atoms that ban the use of this handler.
  // We will be looking up entries in this list;
  // please refactor to a hashed list if its size ever exceeds
  // 5.
  List<int> parentsThatBanHandler;
  class Options {
  public:
    // When flagIsCompositeHandler set, the operation OP applies
    // to expressions of the form ((OP ...), argument1, ...).
    // When flagIsCompositeHandler is not set, the operation OP applies
    // to expressions of the form (OP, argument1, ...)
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
    static Options adminNoTestInvisibleExperimental();
    static Options administrativeNotTested();
    static Options innerInvisible();
    static Options standard();
    static Options compositeStandard();
    static Options innerFreezesArguments();
    static Options adminNoTestInvisibleOffByDefault();
    static Options innerInvisibleExperimental();
    static Options innerAdminNoTestExperimental();
    static Options innerAdminNoTest();
    static Options administrativeExperimentalTested();
    static Options administrativeTested();
    static Options experimental();
    static Options invisibleNoTest();
    static Options innerNoTest();
    static Options innerNoTestExperimental();
    static Options invisibleExperimentalNoTest();
    static Options outerOffByDefault();
    Options();
  };

  Options options;
  Expression::FunctionAddress functionAddress;
  std::string toStringShort() const;
  std::string toStringSummary() const;
  std::string toStringFull() const;
  JSData toJSON() const;
  bool shouldBeApplied(int parentOperationIfAvailable);
  bool operator==(const Function& other) const {
    return
    this->argumentTypes == other.argumentTypes &&
    this->functionAddress == other.functionAddress &&
    this->options.flagIsInner == other.options.flagIsInner;
  }
  bool inputFitsMyInnerType(const Expression& input);
  Function();
  Function(
    Calculator& inputOwner,
    int inputIndexOperation,
    const Expression::FunctionAddress& functionPointer,
    Expression* inputArgumentTypes,
    const std::string& description,
    const std::string& inputExample,
    const std::string& inputAdditionalIndentifier,
    const std::string& inputCalculatorIdentifier,
    const Function::Options& inputOptions,
    const List<int>& inputParentsThatBanHandler
  );
  static unsigned int hashFunction(const Function& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return
    static_cast<unsigned int>(
      reinterpret_cast<uintptr_t>(this->functionAddress)
    );
  }
  bool apply(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int opIndexParentIfAvailable,
    Function** outputHandler
  );
  bool checkConsistency() const;
};

class CalculatorParser;

class SyntacticElement {
public:
  int controlIndex;
  int numNonBoundVariablesInherited;
  int numBoundVariablesInherited;
  Expression data;
  List<Expression> dataList;
  std::string errorString;
  std::string toStringHumanReadable(
    CalculatorParser& owner, bool includeLispifiedExpressions
  ) const;
  std::string toStringSyntaxRole(const CalculatorParser& owner) const;
  SyntacticElement() {
    this->controlIndex = 0;
    // controlIndex = 0 *MUST* point to the empty control sequence.
    this->errorString = "";
    this->numNonBoundVariablesInherited = - 1;
    // - 1 stands for unknown
    this->numBoundVariablesInherited = - 1;
    // - 1 stands for unknown
  }
  std::string getIntegerStringCrashIfNot(CalculatorParser& owner);
  bool isCommandEnclosure() const;
};

class PlotObject {
  friend std::ostream& operator<<(
    std::ostream& output, const PlotObject& unused
  ) {
    (void) unused;
    output << "A Plot object.";
    return output;
  }
private:
  JSData coordinateFunction(int index);
  JSData manifoldImmersionFunctionsJS();
  void writeColorWidthSegments(JSData& output);
  void writeColorLineWidth(JSData& output);
  void writeColor(JSData& output);
  void writeLineWidth(JSData& output);
  void writeColorFilled(JSData& output);
  // Variables are quantities that change internally in the canvas,
  // for example a parameter t of a parametric curve.
  void writeVariables(JSData& output);
  // Parameters are quantities that change
  // "externally" to the canvas such as
  // a value governed by an html slider
  // or a value that has changed due to a mouse event on
  // the canvas.
  void writeParameters(JSData& output);
  JSData functionFromString(const std::string& input);
public:
  struct Labels {
  public:
    static std::string point;
    static std::string path;
    static std::string functionLabel;
    static std::string coordinateFunctions;
    static std::string left;
    static std::string right;
    static std::string numberOfSegments;
    static std::string mandelbrotMode;
    static std::string color;
    static std::string colorFront;
    static std::string colorBack;
    static std::string colorContour;
    static std::string colorFill;
    static std::string lineWidth;
    static std::string manifoldImmersion;
    static std::string variableRange;
    static std::string defaultLength;
    static std::string plotType;
    static std::string viewWindow;
    static std::string body;
    static std::string text;
    static std::string arguments;
    static std::string parameters;
    static std::string parameterLetter;
    static std::string parametersOnTheGraph;
    static std::string parametersOnTheGraphLetter;
  };

  struct PlotTypes {
  public:
    static std::string parametricCurve;
    static std::string plotFunction;
    static std::string segment;
    static std::string points;
    static std::string plotFillStart;
    static std::string axesGrid;
    static std::string plotFillFinish;
    static std::string pathFilled;
    static std::string escapeMap;
    static std::string label;
    static std::string latex;
  };

  std::string plotString;
  std::string fillStyle;
  std::string plotStringWithHtml;
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
  List<std::string> numberOfSegmentsJS;
  Matrix<Expression> points;
  Matrix<std::string> pointsJS;
  Vectors<double> pointsDouble;
  List<Vectors<double> > rectangles;
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
  List<std::string> variablesInPlayJS;
  List<List<std::string> > variableRangesJS;
  std::string parameterLetter;
  std::string parametersOnTheGraphLetter;
  Expression leftPoint;
  Expression rightPoint;
  Expression paramLowE;
  Expression paramHighE;
  Expression numSegmentsE;
  // Parameters are user input variables that describe families of curves.
  // As of writing, parameters are input by the front-end via
  // inputs/sliders at the hands of the end-user.
  // Suppose in the function sin(b x), x has been declared a variable and b
  // - a parameter. Then the frontend may display an input box
  // that allows to dynamically change b.
  HashedList<std::string, HashFunctions::hashFunction> parametersInPlay;
  MapList<std::string, double, HashFunctions::hashFunction>
  parametersOnTheGraph;
  // The name of the parameter with hash bytes appended to
  // guarantee the variable will not collide with any programmer-defined
  // variables.
  HashedList<std::string, HashFunctions::hashFunction> parametersInPlayJS;
  std::string leftPtJS;
  std::string rightPtJS;
  std::string paramLowJS;
  std::string paramHighJS;
  MapList<std::string, bool, HashFunctions::hashFunction> extraFlags;
  // //////////////
  JSData toJSONSurfaceImmersion();
  // Plot a parametric curve. Works both in 2d and 3d.
  JSData toJSONParametricCurve();
  void writeCoordinateFunctions(JSData& output);
  JSData toJSONEscapeMap();
  std::string toStringDebug();
  void computeYBounds();
  std::string getPlotStringFromFunctionStringAndRanges(
    bool useHtml,
    const std::string& functionStringPostfixNotation,
    const std::string& functionStringCalculatorFormat,
    double lowerBound,
    double upperBound
  );
  JSData toJSON();
  JSData toJSON2dDrawFunction();
  JSData toJSONSetProjectionScreen();
  JSData toJSONSegment();
  JSData toJSONPoints();
  JSData toJSONDirectionFieldInTwoDimensions();
  JSData toJSONDrawText();
  JSData toJSONDrawPath();
  JSData toJSONDrawPathFilled();
  JSData toJSONPlotFillStart();
  JSData toJSONPlotFillFinish();
  void makeCircle(
    const Vector<Rational>& center,
    const Rational& radius,
    const std::string& color
  );
  void makePlotFillStart();
  void makePlotFillFinish();
  void makeSegment(
    const Vector<Rational>& left, const Vector<Rational>& right
  );
  void makeLabel(
    const Vector<Rational>& position, const std::string& label
  );
  void makeLabel(
    const Vector<double>& position, const std::string& label
  );
  void makeLatex(
    const Vector<double>& position, const std::string& latex
  );
  void makeEscapeMap(
    const Expression& functionX,
    const std::string& javascriptX,
    const std::string& variableX,
    const Expression& functionY,
    const std::string& javascriptY,
    const std::string& variableY,
    const std::string& inputParameterLetter,
    const List<std::string>& parametersInPlayJS,
    const MapList<std::string, double, HashFunctions::hashFunction>&
    inputParametersOnTheGraph,
    const std::string& inputParametersOnTheGraphLetter,
    bool mandelbrotMode
  );
  void makeRectangle(
    double xLowLeft,
    double yLowLeft,
    double width,
    double height,
    const std::string& color
  );
  PlotObject();
  bool operator==(const PlotObject& other) const;
};

// The following class is meant to use to draw plots for calculus students.
class Plot {
  friend std::ostream& operator<<(std::ostream& output, const Plot& plot) {
    output << "Plot[canvasName: " << plot.canvasName << "]";
    return output;
  }
private:
  std::string canvasName;
  static JSData getCoordinateSystem();
  static JSData getComputeViewWindow();
  JSData getSetViewWindow();
  List<PlotObject> plotObjects;
public:
  struct Labels {
  public:
    static std::string canvasName;
    static std::string controlsName;
    static std::string messagesName;
    static std::string graphicsType;
    static std::string graphicsTwoDimensional;
    static std::string graphicsThreeDimensional;
    static std::string plotObjects;
  };

  HashedList<std::string, HashFunctions::hashFunction> parameterNames;
  HashedList<std::string, HashFunctions::hashFunction> parameterNamesJS;
  double lowX;
  double highX;
  double lowY;
  double highY;
  int desiredHtmlHeightInPixels;
  int desiredHtmlWidthInPixels;
  int defaultLineColor;
  bool flagIncludeExtraHtmlDescriptions;
  bool flagPlotShowJavascriptOnly;
  bool flagDivAlreadyDisplayed;
  bool flagIncludeCoordinateSystem;
  int dimension;
  int priorityViewRectangle;
  // 0 or less: compute the view Window. If this quantity is greater than zero,
  // the user-given bounding box will overwrite any computations.
  // When adding two plots with positive viewing window priorities, the window
  // with the larger priority is used.
  // If the priorities are equal, the windows are combined to the smallest
  // window that fits both.
  int priorityWindow;
  int priorityCanvasName;
  // same as priorityViewWindow but with respect to canvas names.
  void setCanvasName(const std::string& inputName);
  void setViewWindow(
    double inputLowX, double inputLowY, double inputHighX, double inputHighY
  );
  std::string getCanvasName() const;
  std::string toStringDebug();
  std::string getPlotHtml(Calculator& owner);
  void computeCanvasNameIfNecessary(int& canvasCounter);
  std::string getPlotHtml3d(Calculator& owner);
  std::string getPlotHtml2d(Calculator& owner);
  void writeParameters(JSData& output, Calculator& owner);
  std::string getPlotStringAddLatexCommands(bool useHtml);
  bool isOKVector(const Vector<double>& input);
  void addPlotOnTop(PlotObject& input);
  void addPlotsOnTop(Plot& input);
  void drawSegment(
    const Vector<Rational>& left, const Vector<Rational>& right
  );
  void drawLabel(
    const Vector<Rational>& position, const std::string& label
  );
  void drawCircle(
    const Vector<Rational>& center,
    const Rational& radius,
    const std::string& color,
    bool filled
  );
  void drawEscapeMap(
    Expression& functionX,
    const std::string& javascriptX,
    const std::string& variableX,
    Expression& functionY,
    const std::string& javascriptY,
    const std::string& variableY,
    const std::string& parameterLetter,
    List<std::string>& parametersInPlayJS,
    const MapList<std::string, double, HashFunctions::hashFunction>&
    parametersOnTheGraph,
    const std::string& parametersOnTheGraphLetter,
    bool mandelbrotMode
  );
  void drawCoordinateAxes();
  void drawGrid();
  void drawPlotFillStart();
  void drawPlotFillFinish();
  void drawRectangle(
    double xLowLeft,
    double yLowLeft,
    double width,
    double height,
    const std::string& color
  );
  List<PlotObject>& getPlots();
  void clearPlotObjects();
  void setExpectedPlotObjects(int expectedSize);
  Plot();
  void computeAxesAndBoundingBox3d();
  void computeAxesAndBoundingBox();
  void operator+=(const Plot& other);
  void operator+=(const PlotObject& other);
  bool operator==(const Plot& other) const;
};

class InputBox {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const InputBox& input
  ) {
    output << "name: " << input.name << ", ...";
    return output;
  }
  std::string name;
  Expression value;
  Expression min;
  Expression max;
  Expression step;
  bool operator==(const InputBox& other) const {
    return this->name == other.name;
  }
  std::string getSliderName() const;
  std::string getUserInputBox() const;
  static unsigned int hashFunction(const InputBox& input) {
    return HashFunctions::hashFunction(input.name);
  }
};

class ObjectContainer {
  // Following are containers for data structures that are implemented in C++.
  // These objects are dynamically allocated and used by the calculator as
  // requested
  // by various predefined function handlers.
public:
  HashedListReferences<ElementWeylGroup> weylGroupElements;
  // Pointers to the values of the semisimpleLieAlgebras map.
  // Required so we can return references SemisimpleLieAlgebra*&
  ListReferences<SemisimpleLieAlgebra*> semisimpleLieAlgebraPointers;
  MapReferences<DynkinType, SemisimpleLieAlgebra> semisimpleLieAlgebras;
  MapReferences<DynkinType, SemisimpleSubalgebras> semisimpleSubalgebras;
  HashedListReferences<
    GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  > weylGroupRepresentations;
  HashedListReferences<
    VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  > weylGroupVirtualRepresentations;
  ListReferences<ModuleSSalgebra<RationalFraction<Rational> > >
  categoryOModules;
  ListReferences<SlTwoSubalgebras> slTwoSubalgebras;
  HashedListReferences<ElementEllipticCurve<ElementZmodP> >
  ellipticCurveElementsZmodP;
  HashedListReferences<ElementEllipticCurve<Rational> >
  ellipticCurveElementsRational;
  HashedListReferences<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  > tensorElements;
  HashedListReferences<Polynomial<Rational> > polynomialsRational;
  HashedListReferences<Polynomial<AlgebraicNumber> > polynomialsAlgebraic;
  HashedListReferences<Polynomial<ElementZmodP> > polynomialsModular;
  HashedListReferences<PolynomialModuloPolynomial<ElementZmodP> >
  polynomialQuotientsModular;
  HashedListReferences<ElementWeylAlgebra<Rational> > weylAlgebraElements;
  HashedListReferences<
    ElementUniversalEnveloping<RationalFraction<Rational> >
  > universalEnvelopingAlgebraElements;
  HashedListReferences<RationalFraction<Rational> > rationalFunctions;
  HashedListReferences<RationalFraction<AlgebraicNumber> >
  rationalFunctionsAlgebraic;
  HashedListReferences<RationalFraction<ElementZmodP> >
  rationalFunctionsModular;
  HashedListReferences<Rational> allRationals;
  HashedListReferences<CharacterSemisimpleLieAlgebraModule<Rational> >
  charactersSemisimpleLieAlgebraFiniteDimensional;
  HashedListReferences<double, MathRoutines::hashDouble> doubleNumbers;
  HashedListReferences<
    std::string, HashFunctions::hashFunction<std::string>
  > allStrings;
  HashedListReferences<JSData> jsonObjects;
  HashedListReferences<
    std::string, HashFunctions::hashFunction<std::string>
  > expressionNotation;
  HashedListReferences<Expression> expressionWithNotation;
  HashedListReferences<Expression> constraints;
  HashedListReferences<LittelmannPath> lakshmibaiSeshadriPaths;
  HashedListReferences<MatrixTensor<Rational> > matrixTensorRationals;
  HashedListReferences<ElementZmodP> elementsModP;
  HashedListReferences<Weight<Rational> > weights;
  HashedListReferences<Weight<Polynomial<Rational> > > weightsPolynomial;
  HashedListReferences<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
  elementsOfSemisimpleLieAlgebrasWithAlgebraicCoefficients;
  ListReferences<
    GroupRepresentation<
      FiniteGroup<
        HyperoctahedralBitsAutomorphism::ElementHyperoctahedralGroupR2
      >,
      Rational
    >
  > hyperoctahedralRepresentations;
  ListReferences<Plot> allPlots;
  List<bool> userInputBoxSliderDisplayed;
  MapReferences<
    std::string, InputBox, HashFunctions::hashFunction<std::string>
  > userInputTextBoxesWithValues;
  MapReferences<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > graphicsScripts;
  AlgebraicClosureRationals algebraicClosure;
  HashedList<AlgebraicNumber> allAlgebraicNumbers;
  HashedListReferences<
    HyperoctahedralBitsAutomorphism::ElementHyperoctahedralGroupR2
  > elementsHyperOctGroup;
  ListReferences<HyperoctahedralGroupData> hyperOctahedralGroups;
  HashedListReferences<MonomialTensor<int, HashFunctions::hashFunction> >
  littelmannOperators;
  MapReferences<List<Vector<Rational> >, VectorPartitionFunction>
  vectorPartitionFunctions;
  WeylGroupData& getWeylGroupDataCreateIfNotPresent(const DynkinType& input);
  SemisimpleLieAlgebra& getLieAlgebraCreateIfNotPresent(
    const DynkinType& input
  );
  SemisimpleSubalgebras& getSemisimpleSubalgebrasCreateIfNotPresent(
    const DynkinType& input
  );
  UnsecurePseudoRandomGenerator pseudoRandom;
  int canvasPlotCounter;
  void reset();
  void resetSliders();
  void resetPlots();
  bool checkConsistencyAfterReset();
  std::string toString();
  std::string toStringJavascriptForUserInputBoxes();
};

class StateMaintainerCalculator {
public:
  Calculator* owner;
  int startingRuleStackSize;
  StateMaintainerCalculator(Calculator& inputBoss);
  void addRule(const Expression& rule);
  ~StateMaintainerCalculator();
};

class CalculatorParser {
private:
  // Point to the calculator object that contains the parser;
  // set in the constructor of Calculator.
  Calculator* owner;
  int numberOfEmptyTokensStart;
  int counterInSyntacticSoup;
  bool flagLogSyntaxRules;
  bool flagEncounteredSplit;
  List<SyntacticElement> syntacticSoup;
  List<SyntacticElement> syntacticStack;
  List<SyntacticElement>* currrentSyntacticSoup;
  List<SyntacticElement>* currentSyntacticStack;
  HashedList<int, HashFunctions::hashFunction> nonBoundVariablesInContext;
  HashedList<int, HashFunctions::hashFunction> boundVariablesInContext;
  HashedList<std::string> stringsThatSplitIfFollowedByDigit;
  MapList<
    std::string,
    List<std::string>,
    HashFunctions::hashFunction<std::string>
  > predefinedWordSplits;
public:
  // Control sequences parametrize the syntactical elements
  HashedList<std::string> controlSequences;
  std::string syntaxErrors;
  List<std::string> parsingLog;
  std::string lastRuleName;
private:
  // Sets an expression value and syntactic role to a given position in the
  // stack.
  bool setStackValue(
    const Expression& newExpression,
    const std::string& newRole,
    int stackOffset
  );
  bool isInterpretedAsEmptySpace(const std::string& input);
  bool isInterpretedAsEmptySpace(unsigned char input);
  bool isSeparatorFromTheLeftGeneral(const std::string& input);
  bool isSeparatorFromTheLeftForDefinition(const std::string& input);
  bool isSeparatorFromTheLeftForList(const std::string& input);
  bool isBoundVariableInContext(int inputOperation);
  bool isNonBoundVariableInContext(int inputOperation);
  bool isStandardCalculatorCharacter(unsigned char input);
  bool isSeparatorFromTheLeftForInterval(const std::string& input);
  bool isSeparatorFromTheRightGeneral(const std::string& input);
  bool isSeparatorFromTheRightForDefinition(const std::string& input);
  bool isSeparatorFromTheRightForList(const std::string& input);
  bool allowsPowerInPreceding(const std::string& lookAhead);
  bool allowsPowerInNext(const std::string& lookBehind);
  bool allowsLimitProcessInPreceding(const std::string& lookAhead);
  bool allowsApplyFunctionInPreceding(const std::string& lookAhead);
  bool allowsIfInPreceding(const std::string& lookAhead);
  bool allowsOrInPreceding(const std::string& lookAhead);
  bool allowsAndInPreceding(const std::string& lookAhead);
  bool allowsInInPreceding(const std::string& lookAhead);
  bool allowsPlusInPreceding(const std::string& lookAhead);
  bool allowsTimesInNext(const std::string& preceding);
  bool allowsTimesInPreceding(
    const SyntacticElement& preceding, const std::string& lookAhead
  );
  bool allowsTimesInPreceding(const std::string& lookAhead);
  bool allowsTensorInPreceding(const std::string& lookAhead);
  bool allowsDivideInPreceding(const std::string& lookAhead);
  void addTrigonometricSplit(
    const std::string& trigonometricFunction,
    const List<std::string>& variables
  );
  bool decreaseStackSetCharacterRanges(int decrease);
  SyntacticElement getSyntacticElementEnd();
  bool popTopSyntacticStack() {
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size
      -
      1
    );
    return true;
  }
  bool popBelowStackTop() {
    (*this->currentSyntacticStack).removeIndexShiftDown((
        *this->currentSyntacticStack
      ).size -
      2
    );
    return true;
  }
  bool canBeRegardedAsDifferentialForm(const SyntacticElement& input);
  bool canBeRegardedAsDifferentialForm(const std::string& input);
  // Sets the last rule name if flagLogSyntaxRules is set.
  // Always returns true [used to shorten the body of applyOneRule()].
  bool setLastRuleName(const std::string& ruleName);
  bool replaceXXVXdotsXbyE_BOUND_XdotsX(int numberOfXs);
  bool replaceVXdotsXbyE_NONBOUND_XdotsX(int numberOfXs);
  bool replaceEXXEXEBy_CofEEE(int controlIndex);
  bool replaceEXXEXEXBy_CofEEE_X(int controlIndex);
  bool replaceEOXbyEX();
  bool replaceEEBy_CofEE(int controlIndex);
  bool replaceEEXBy_CofEE_X(int controlIndex);
  bool replaceOOEEXbyEXpowerLike();
  bool replaceEEByE();
  bool replaceEXEXByEX();
  bool replaceEXEXBy_OofEE_X(int operation);
  bool replaceSsSsXdotsXbySsXdotsX(int numberOfDots);
  bool replaceEXdotsXbySsXdotsX(int numberOfDots);
  bool replaceEXdotsXBySs(int numberOfDots) {
    this->replaceEXdotsXbySsXdotsX(numberOfDots);
    return this->decreaseStackSetCharacterRanges(numberOfDots);
  }
  bool replaceOEXByE();
  bool replaceOEXByEX();
  bool replaceC1C2Eby_C2ofC1E();
  bool replaceEOByE();
  bool replaceOEByE();
  bool replaceOXXByEXX();
  bool replaceOXEByE();
  bool replaceOXEXByEX();
  bool replaceOXEEXByEX();
  bool replaceOXXEXEXEXByE();
  bool replaceSqrtEXXByEXX();
  bool replaceSqrtEXByEX();
  bool replaceSqrtXEXByEX();
  bool replaceOXEXEByE();
  bool replaceOXEXEXByEX();
  bool replaceOXEXEXXByEXX();
  bool replaceOXEXEXEXByE();
  bool replaceEOEXByEX();
  bool replaceEPowerMinusEXByEX();
  bool replaceUnderscoreEPowerEbyLimits();
  bool replacePowerEUnderScoreEbyLimits();
  bool replaceXEEXBy_OofEE_X(int inputOperation);
  bool replaceXEEBy_OofEE(int inputOperation);
  bool replaceECByC();
  bool replaceEXEBySequence(int controlIndex);
  bool replaceYXBySequenceX(int controlIndex);
  bool replaceXXXbyE();
  bool replaceYBySequenceY(int controlIndex);
  bool replaceXXYBySequenceY(int controlIndex);
  bool replaceXXYXBySequenceYX(int controlIndex);
  bool replaceYXdotsXBySequenceYXdotsX(int controlIndex, int numberOfXs = 0);
  bool replaceSequenceXEBySequence(int controlIndex);
  bool replaceSequenceUXEYBySequenceZY(int controlIndex);
  bool replaceCEByC();
  bool replaceCCByC();
  bool replaceEOEByE() {
    return
    this->replaceEXEByCofEE((*this->currentSyntacticStack)[(
          *this->currentSyntacticStack
        ).size -
        2
      ].controlIndex
    );
  }
  bool replaceMatrixEXByMatrix();
  bool replaceMatrixEXByMatrixX();
  bool replaceMatrixEXByMatrixNewRow();
  bool replaceMatrixXByE();
  bool replaceCXByE();
  bool replaceCXByEX();
  bool replaceXEXEXBy_CofEE(int operation);
  bool replaceEXEByCofEE(int operation);
  bool replaceXYByConY(int controlIndex) {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2
    ].controlIndex =
    controlIndex;
    return true;
  }
  bool replaceXYYByConYY(int controlIndex) {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3
    ].controlIndex =
    controlIndex;
    return true;
  }
  bool replaceXYYYByConYYY(int controlIndex) {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4
    ].controlIndex =
    controlIndex;
    return true;
  }
  bool replaceXXByCon(int controlIndex) {
    this->replaceXYByConY(controlIndex);
    return this->decreaseStackSetCharacterRanges(1);
  }
  bool replaceXByCon(int controlIndex);
  bool replaceXByO(int operation);
  bool replaceXByConCon(int controlIndex1, int controlIndex2);
  bool replaceXXXByCon(int controlIndex);
  bool replaceXXXByConCon(int controlIndex1, int controlIndex2);
  bool replaceXXXXXByCon(int controlIndex);
  bool replaceXXXXXByConCon(int controlIndex1, int controlIndex2);
  bool replaceXXXXByConCon(int controlIndex1, int controlIndex2);
  bool replaceXdotsXByMatrixStart(int numberOfXs);
  bool replaceXXXXByCon(int controlIndex1);
  bool replaceXXYByY() {
    (*this->currentSyntacticStack)[this->currentSyntacticStack->size - 3] = (
      *this->currentSyntacticStack
    )[this->currentSyntacticStack->size - 1];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size
      -
      2
    );
    return true;
  }
  bool replaceXXYXByYX() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4
    ] = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2
    ];
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3
    ] = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1
    ];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size
      -
      2
    );
    return true;
  }
  bool replaceXYYXByYY() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4
    ] = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3
    ];
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3
    ] = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2
    ];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size
      -
      2
    );
    return true;
  }
  bool replaceXYXByY() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3
    ] = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2
    ];
    this->decreaseStackSetCharacterRanges(2);
    return true;
  }
  bool replaceXYByY() {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2
    ] = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1
    ];
    (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size
      -
      1
    );
    return true;
  }
  bool replaceXEXByE();
  bool replaceXEXYByEY();
  bool replaceVbyVdotsVAccordingToPredefinedWordSplits();
  bool replaceAXbyEX();
  bool replaceAXXbyEXX();
  bool replaceIntegerXbyEX();
  bool replaceIntegerDotIntegerByE();
  bool replaceXXByEEmptySequence();
  bool replaceOXdotsXbyEXdotsX(int numberOfXs);
  bool replaceOXbyEX();
  bool replaceObyE();
  bool replaceXXbyEX();
  bool replaceXXbyO(int operation);
  bool replaceXXYbyOY(int operation);
  bool replaceXEXByEContainingOE(int inputOpIndex);
  bool replaceXXByEmptyString();
  bool replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  bool replaceXXbyE();
  void resetStack() {
    SyntacticElement emptyElement = this->getEmptySyntacticElement();
    (*this->currentSyntacticStack).initializeFillInObject(
      this->numberOfEmptyTokensStart, emptyElement
    );
  }
  SyntacticElement getEmptySyntacticElement();
  int conBindVariable() {
    return this->controlSequences.getIndexNoFail("{{}}");
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
  int conQuote() {
    return this->controlSequences.getIndexNoFail("\"");
  }
  int conDefine() {
    return this->controlSequences.getIndexNoFail("=");
  }
  int conIsDenotedBy() {
    return this->controlSequences.getIndexNoFail("=:");
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
  bool isDefiniteIntegral(const std::string& syntacticRole);
  void parseFillDictionary(const std::string& input);
  void logParsingOperation();
  bool extractExpressions(
    Expression& outputExpression, std::string* outputErrors
  );
  int getOperationIndexFromControlIndex(int controlIndex);
  bool decreaseStackExceptLast(int decrease);
  bool decreaseStack(int decrease);
  bool decreaseStackExceptLastTwo(int decrease);
  bool applyOneRule();
  bool isLeftSeparator(unsigned char c);
  bool isRightSeparator(unsigned char c);
  bool shouldSplitOutsideQuotes(const std::string& left, char right);
  bool parseEmbedInCommandSequence(
    const std::string& input, Expression& output
  );
  int getExpressionIndex();
public:
  int conSequence() {
    return this->controlSequences.getIndexNoFail("Sequence");
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
  int conInteger() {
    return this->controlSequences.getIndexNoFail("Integer");
  }
  int conIntegralUnderscore() {
    return this->controlSequences.getIndexNoFail("\\int_{*}");
  }
  int conIntegralSuperScript() {
    return this->controlSequences.getIndexNoFail("\\int^{*}");
  }
  int conIntegralSuperSubScript() {
    return this->controlSequences.getIndexNoFail("\\int_{*}^{**}");
  }
  int conSequenceStatements() {
    return this->controlSequences.getIndexNoFail("SequenceStatements");
  }
  std::string toStringIsCorrectAsciiCalculatorString(
    const std::string& input
  );
  std::string toStringSyntacticStackHTMLTable(
    bool includeLispifiedExpressions, bool ignoreCommandEnclosures
  );
  bool parseNoEmbeddingInCommand(
    const std::string& input, Expression& output
  );
  bool parse(
    const std::string& input,
    bool stripCommandSequence,
    Expression& output
  );
  bool parseAndExtractExpressionsDefault(
    const std::string& input, Expression& output
  );
  bool parseAndExtractExpressions(
    const std::string& input,
    Expression& output,
    List<SyntacticElement>& outputAllSyntacticElements,
    List<SyntacticElement>& outputSyntacticStack,
    std::string* outputSyntacticErrors
  );
  void parseFillDictionary(
    const std::string& input, List<SyntacticElement>& output
  );
  void initialize(Calculator* inputOwner) {
    this->owner = inputOwner;
  }
  void initializeControlSequences();
  void initializeStringsThatSplitIfFollowedByDigit();
  void initializePredefinedWordSplits();
  std::string toStringSyntacticStackHTMLSimple();
  void reset();
  void parseConsumeQuote(
    const std::string& input,
    unsigned int& indexOfLast,
    List<SyntacticElement>& output
  );
};

class Calculator {
  template <typename AnyType>
  friend Calculator& operator<<(Calculator& output, const AnyType& any) {
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

  // The built-in type of an object is an string identifier
  // used internally to indicate the type of a built-in object.
  // A built-in object is one that has an internal C++ data structure that
  // represents the entire object.
  // The Expression object records the type string and an integer that
  // represents the position of the object in a global array held in
  // calculator.objectContainer.
  // A built in object is atomic
  // from the standpoint of a calculator user.
  // The built in type is not allowed to be used as a function.
  class BuiltInTypes {
  public:
    Calculator* owner;
    HashedList<std::string> all;
    class Names {
    public:
      static std::string vectorPartitionFunction;
      static std::string elementTensorsGeneralizedVermas;
    };

    int vectorPartitionFunction() {
      return
      this->owner->operations.getIndexNoFail(
        Calculator::BuiltInTypes::Names::vectorPartitionFunction
      );
    }
    BuiltInTypes() {
      this->owner = nullptr;
    }
  };

  // A function is a predefined string that is to be attached to
  // a built-in function handler.
  class Functions {
  public:
    Calculator* owner;
    class Names {
    public:
      static std::string commandEnclosure;
      static std::string setInputBox;
      static std::string setRandomSeed;
      static std::string sort;
      static std::string transpose;
      static std::string approximations;
      static std::string turnOnRules;
      static std::string turnOffRules;
      static std::string vectorPartitionFunction;
      static std::string vectorPartitionFunctionElementary;
      class Trigonometry {
      public:
        static std::string sine;
        static std::string cosine;
      };
    };

    Functions() {
      this->owner = nullptr;
    }
    int vectorPartitionFunction() {
      return
      this->owner->operations.getIndexNoFail(
        Calculator::Functions::Names::vectorPartitionFunction
      );
    }
  };

  Calculator::BuiltInTypes builtInTypes;
  Calculator::BuiltInTypes builtInFunctions;
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
    educational  };
  Mode mode;
  // Operations parametrize the expression elements.
  // Operations are the labels of the atom nodes of the expression tree.
  MapReferences<
    std::string,
    MemorySaving<OperationHandlers>,
    HashFunctions::hashFunction
  > operations;
  HashedList<std::string> atomsThatAllowCommutingOfCompositesStartingWithThem;
  HashedList<std::string> atomsNotAllowingChainRule;
  HashedList<std::string> arithmeticOperations;
  HashedList<std::string>
  knownOperationsInterpretedAsFunctionsMultiplicatively;
  HashedList<std::string> knownFunctionsWithComplexRange;
  HashedList<std::string> atomsThatFreezeArguments;
  HashedList<std::string> atomsWhoseExponentsAreInterpretedAsFunctions;
  HashedList<std::string> atomsNotInterpretedAsFunctions;
  HashedList<std::string> atomsThatMustNotBeCached;
  HashedList<std::string> autoCompleteKeyWords;
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction>
  toStringHandlersAtoms;
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction>
  toStringHandlersComposite;
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction>
  toStringDataHandlers;
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

  MapList<
    std::string,
    NamedRuleLocation,
    HashFunctions::hashFunction<std::string>
  > namedRules;
  // Calculator functions have as arguments two expressions passed by
  // reference,
  // const Expression& input and Expression& output. Calculator functions
  // return bool. It is forbidden to pass the same object as input and output.
  // If a calculator function returns false this
  // means that the calculator failed to evaluate the
  // function. If that is the case, the value of output is not specified and
  // *MUST NOT* be used in the calling function.
  // If a function returns true this means that output contains the result of
  // the function.
  // Note that the output of a function may be of type Error. Error results
  // come, like any other
  // result, with a true return from the function.
  // -----------------------------------------------
  //
  // The concept of an inner and outer function is **deprecated**.
  //
  // The following comments document what used to be the behavior.
  // The new behavior is to have no distinction between "inner" and "outer"
  // functions.
  //
  // In addition, built-in functions are split into two flavors:
  // inner functions (or just "functions")
  // and outer functions (or "laws").
  // The only difference between inner functions and outer functions is the
  // way they are applied when the calculator reduces an expression.
  //
  // Suppose the calculator is reducing Expression X.
  // 1. Outer functions ("laws").
  // 1.1. Let X be expression whose first child is an atom equal to the name of
  // the outer function.
  // 1.2  Call the outer function with input argument equal to X.
  // 1.3. If the outer function returns true but the output argument is
  // identically equal to X, nothing is done
  // (the action of the outer function is ignored).
  // 1.4. If an outer function returns true and the output argument is
  // different from X, X is replaced by this output.
  // 2. Inner functions ("functions").
  // 2.1. Let X be expression whose first child is an atom equal to the name of
  // the inner function. We define Argument as follows.
  // 2.1.1. If X has two children, Argument is set to the second child of X.
  // 2.1.2. If X does not have two children, Argument is set to be equal to the
  // entire X.
  // 2.2. The inner function is called with input argument equal to Argument.
  // 2.3. If the inner function returns true, X is substituted with
  //      the output argument of the inner function, else nothing is done.
  // As explained above, the distinction between inner functions and outer
  // functions is only practical.
  // The notions of inner and outer functions do not apply to user-defined
  // substitution rules entered via the calculator. User-defined substitution
  // rules are processed like outer functions, with the
  // major difference that even if their output coincides
  // with their input the substitution is carried out, resulting in an infinite
  // cycle. Here, by ``infinite cycle'' we either mean a 100% CPU run until the
  // timeout& algebraic safety kicks in, or error interception with a
  // ``detected substitution cycle'' or similar error message.
  //
  // ----------------------------------------------------------
  HashedList<Expression> knownDoubleConstants;
  List<double> knownDoubleConstantValues;
  int maximumRecursionDepth;
  int recursionDepth;
  int depthRecursionReached;
  int maximumAlgebraicTransformationsPerExpression;
  int maximumLatexChars;
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
    int totalPatternMatchesPerformed;
    int totalEvaluationLoops;
    long long int numberOfSmallAdditionsStart;
    long long int numberOfSmallMultiplicationsStart;
    long long int numberOfSmallGreatestCommonDivisorsStart;
    long long int numberOfLargeAdditionsStart;
    long long int numberOfLargeMultiplicationsStart;
    long long int numberOfLargeGreatestCommonDivisorsStart;
    int64_t millisecondsLastLog;
    int64_t startTimeEvaluationMilliseconds;
    int64_t startParsing;
    int64_t lastStopwatchParsing;
    EvaluationStatistics();
    void initialize();
    void reset();
  };

  EvaluationStatistics statistics;
  bool flagAbortComputationASAP;
  bool flagTimeLimitErrorDetected;
  bool flagFirstErrorEncountered;
  bool flagMaxRecursionErrorEncountered;
  bool flagMaxTransformationsErrorEncountered;
  bool flagNewContextNeeded;
  bool flagUsePredefinedWordSplits;
  bool flagPlotShowJavascriptOnly;
  bool flagPlotNoControls;
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
  bool flagForkingprocessAllowed;
  int numberOfPredefinedAtoms;
  Expression programExpression;
  CalculatorParser parser;
  class ExpressionCache {
  public:
    int ruleState;
    bool flagNonCacheable;
    bool flagFinalized;
    Expression reducesTo;
    ExpressionCache();
  };

  // Cached expressions per rule stack.
  MapList<Expression, Calculator::ExpressionCache> cachedExpressions;
  // //
  HashedList<Expression> evaluatedExpressionsStack;
  Expression ruleStack;
  HashedListReferences<Expression> allChildExpressions;
  List<unsigned int> allChildExpressionHashes;
  List<std::string> evaluationErrors;
  // std::string inputStringRawestOfTheRaw;
  std::string inputString;
  std::string outputString;
  JSData outputJS;
  std::string outputCommentsString;
  ObjectContainer objectContainer;
  std::string javaScriptDisplayingIndicator;
  int numberOfOutputFiles;
  std::string userLabel;
  // List<std::string> logEvaluationSteps;
  std::stringstream comments;
  std::stringstream errorsPublic;
  FormatExpressions formatVisibleStrings;
  bool approximationsBanned();
  std::string toStringRuleStatusUser();
  std::string toString();
  JSData toJSONPerformance();
  Expression getNewBoundVariable();
  Expression getNewAtom(const std::string& preferredName = "");
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
  // In other words, instead of doing:
  // calculator << "Some error message."; return false;
  // we can just do: return calculator << "Some error message";
  operator bool() const {
    return false;
  }
  // Adds an expression to the global list of expressions that are children of
  // another expression.
  int addChildExpression(const Expression& child);
  void registerCalculatorFunction(
    Function& inputFunction, int indexOperation
  );
  std::string toStringSemismipleLieAlgebraLinksFromHardDrive(
    const std::string& prefixFolder,
    const DynkinType& dynkinType,
    FormatExpressions* format = nullptr
  );
  Function& getFunctionHandlerFromNamedRule(
    const std::string& inputRuleName
  );
  bool checkPredefinedFunctionNameRepetitions();
  bool checkOperationHandlers();
  bool checkConsistencyAfterInitialization();
  // To make operations read only, we make operations private and return const
  // pointer to it.
  const HashedList<std::string, HashFunctions::hashFunction>& getOperations() {
    return this->operations.keys;
  }
  const HashedList<std::string>& getBuiltInTypes() {
    return this->builtInTypes.all;
  }
  const List<Function>* getOperationHandlers(int operation);
  const List<Function>* getOperationCompositeHandlers(int operation);
  Expression expressionInteger(int input);
  Expression expressionRational(const Rational& input);
  Expression expressionZero();
  Expression expressionOne();
  Expression expressionTwo();
  Expression expressionFour();
  Expression expressionMinusOne();
  Expression expressionHalf();
  Expression expressionMinusHalf();
  Expression expressionInfinity();
  Expression expressionMinusInfinity();
  Expression expressionSquareRootNegativeOne();
  void logFunctionWithTime(Function& input, int64_t startTime);
  void logTime(int64_t startTime);
  void logPublicError(const std::string& error);
  std::string writeDefaultLatexFileReturnHtmlLink(
    const std::string& fileContent,
    std::string* outputFileNameNoExtension,
    bool useLatexDviPSpsToPNG = false
  );
  bool recursionDepthExceededHandleRoughly(
    const std::string& additionalErrorInfo = ""
  );
  bool getMatrixExpressions(
    const Expression& input,
    Matrix<Expression>& output,
    int desiredNumberOfRows = - 1,
    int desiredNumberOfColumns = - 1
  );
  bool getMatrixExpressionsFromArguments(
    const Expression& input,
    Matrix<Expression>& output,
    int desiredNumRows = - 1,
    int desiredNumCols = - 1
  );
  void makeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output);
  bool accountRule(
    const Expression& ruleE, StateMaintainerCalculator& ruleStackMaintainer
  );
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
    return
    this->operations.getIndexNoFail(
      Calculator::Functions::Names::commandEnclosure
    );
  }
  int opRulesOff() {
    return this->operations.getIndexNoFail("RulesOff");
  }
  int opRulesOn() {
    return this->operations.getIndexNoFail("RulesOn");
  }
  int opApproximations() {
    return
    this->operations.getIndexNoFail(
      Calculator::Functions::Names::approximations
    );
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
  int opPower() {
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
    return
    this->operations.getIndexNoFail("PolynomialModuloPolynomialModuloInteger");
  }
  int opEllipticCurveElementsRational() {
    return this->operations.getIndexNoFail("ellipticCurveElementsRational");
  }
  int opEllipticCurveElementsZmodP() {
    return this->operations.getIndexNoFail("ellipticCurveElementsZmodP");
  }
  int opRationalFraction() {
    return this->operations.getIndexNoFail("RationalFunction");
  }
  int opRationalFunctionAlgebraicCoefficients() {
    return
    this->operations.getIndexNoFail("RationalFunctionAlgebraicCoefficients");
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
    return
    this->operations.getIndexNoFail("HyperoctahedralGroupRepresentation");
  }
  int opWeylGroupVirtualRep() {
    return this->operations.getIndexNoFail("WeylGroupVirtualRep");
  }
  int opElementTensorGVM() {
    return
    this->operations.getIndexNoFail(
      Calculator::BuiltInTypes::Names::elementTensorsGeneralizedVermas
    );
  }
  int opElementSemisimpleLieAlgebraAlgebraicCoefficients() {
    return
    this->operations.getIndexNoFail(
      "ElementSemisimpleLieAlgebraAlgebraicCoefficients"
    );
  }
  int opCharSSAlgMod() {
    return this->operations.getIndexNoFail("CharSSAlgMod");
  }
  int opSemisimpleLieAlgebra() {
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
  int opSolveFor() {
    return this->operations.getIndexNoFail("SolveFor");
  }
  int opCoefficientsPowersOf() {
    return this->operations.getIndexNoFail("CoefficientsPowersOf");
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
  bool appendOpandsReturnTrueIfOrderNonCanonical(
    const Expression& input, List<Expression>& output, int operation
  );
  bool appendMultiplicandsReturnTrueIfOrderNonCanonical(
    Expression& expression, List<Expression>& output
  ) {
    return
    this->appendOpandsReturnTrueIfOrderNonCanonical(
      expression, output, this->opTimes()
    );
  }
  bool appendSummandsReturnTrueIfOrderNonCanonical(
    const Expression& expression, List<Expression>& output
  ) {
    return
    this->appendOpandsReturnTrueIfOrderNonCanonical(
      expression, output, this->opPlus()
    );
  }
  void specializeBoundVariables(
    Expression& toBeSubbedIn,
    MapList<Expression, Expression>& matchedPairs
  );
  Expression* patternMatch(
    const Expression& pattern,
    Expression& expression,
    MapList<Expression, Expression>& bufferPairs,
    const Expression* condition = nullptr,
    std::stringstream* logStream = nullptr
  );
  bool processOneExpressionOnePatternOneSubstitution(
    const Expression& pattern,
    Expression& expression,
    MapList<Expression, Expression>& bufferPairs,
    std::stringstream* logStream = nullptr
  );
  static void checkInputNotSameAsOutput(
    const Expression& input, const Expression& output
  ) {
    if (&input == &output) {
      global.fatal
      << "The input expression, equal to "
      << input.toString()
      << " has the same address as the output expression. "
      << global.fatal;
    }
  }
  bool callCalculatorFunction(
    Expression::FunctionAddress toBeCalled,
    const Expression& input,
    Expression& output
  ) {
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
    const Expression& pattern,
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
    // see Function::Options
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
      bool requresAdminAccess;
      // empty string = not computed.
      std::string actualResult;
      // empty string = not known.
      std::string expectedResult;
      OneTest();
    };

    MapList<
      std::string,
      Calculator::Test::OneTest,
      HashFunctions::hashFunction<std::string>
    > commands;
    JSData storedResults;
    Test(Calculator& inputOwner);
    bool processResults();
    bool loadTestStrings(std::stringstream* commentsOnFailure);
    bool writeTestStrings(std::stringstream* commentsOnFailure);
    bool processOneTest(JSData& input);
    static bool all();
    static bool cacheWorks();
    static bool loopDetection();
    static bool loopDetectionCycle();
    static bool loopDetectionEverExpanding();
    static bool numberOfTestFunctions(Calculator& ownerInitialized);
    static bool parseDecimal(Calculator& ownerInitialized);
    static bool parseQuotes(Calculator& ownerInitialized);
    static bool parseAllExamples(Calculator& ownerInitialized);
    static bool parseConsumeQuote(Calculator& ownerInitialized);
    static bool builtInFunctionsABTest(Calculator& ownerInitialized);
    static bool checkBuiltInInitializations(Calculator& ownerInitialized);
    void calculatorTestPrepare();
    bool calculatorTestRun();
  };

  static bool automatedTest(
    Calculator& calculator, const Expression& input, Expression& output
  );
  int getNumberOfBuiltInFunctions();
  static bool outerStandardFunction(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int opIndexParentIfAvailable,
    Function** outputHandler
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
  static bool outerPowerRaiseToFirst(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool collectCoefficientsPowersVariables(
    const Expression& input,
    const Expression& variable,
    VectorSparse<Expression>& outputPositionIiscoeffXtoIth
  );
  bool collectOpands(
    const Expression& input,
    int operation,
    List<Expression>& outputOpands
  );
  bool collectOpandsAccumulate(
    const Expression& input,
    int operation,
    List<Expression>& outputOpands
  );
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
  template <class Type>
  bool getVectorFromFunctionArguments(
    const Expression& input,
    Vector<Type>& output,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetDimNonMandatory = - 1
  ) {
    if (
      this->getVectorFromFunctionArgumentsInternal(
        input, output, inputOutputStartingContext, targetDimNonMandatory
      )
    ) {
      return true;
    }
    if (
      input.size() != 2 || input.isSequenceNElements() || input.isMatrix()
    ) {
      return false;
    }
    return
    getVectorFromFunctionArgumentsInternal(
      input[1], output, inputOutputStartingContext, targetDimNonMandatory
    );
  }
  template <class Type>
  bool getVectorFromFunctionArgumentsInternal(
    const Expression& input,
    Vector<Type>& output,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetDimNonMandatory = - 1
  ) {
    STACK_TRACE("Calculator::getVectorFromFunctionArguments");
    input.checkInitialization();
    Expression sequence = input;
    if (sequence.isList()) {
      sequence.setChildAtomValue(0, this->opSequence());
    }
    sequence.checkInitialization();
    return
    this->getVector(
      sequence, output, inputOutputStartingContext, targetDimNonMandatory
    );
  }
  bool getSumProductsExpressions(
    const Expression& inputSum,
    List<List<Expression> >& outputSumMultiplicands
  );
  bool getVectorExpressions(
    const Expression& input,
    List<Expression>& output,
    int targetDimNonMandatory = - 1
  );
  template <typename Type>
  bool combineContexts(
    List<WithContext<Type> >& inputOutputConverted,
    ExpressionContext* inputOutputStartingContext = nullptr
  );
  bool getVectorInt(const Expression& input, List<int>& output);
  bool getVectorLargeIntegerFromFunctionArguments(
    const Expression& input, List<LargeInteger>& output
  );
  bool getMatrixDoubles(
    const Expression& input,
    Matrix<double>& output,
    int desiredNumberOfColumns = - 1
  );
  bool getVectorDoubles(
    const Expression& input,
    Vector<double>& output,
    int desiredDimensionNonMandatory = - 1
  );
  bool getVectorDoublesFromFunctionArguments(
    const Expression& input,
    Vector<double>& output,
    int desiredDimensionNonMandatory = - 1
  );
  template <class Type>
  bool getVector(
    const Expression& input,
    Vector<Type>& output,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetDimensionNonMandatory = - 1
  );
  template <class Coefficient>
  bool getTypeWeight(
    Calculator& calculator,
    const Expression& input,
    Vector<Coefficient>& outputWeightSimpleCoords,
    WithContext<SemisimpleLieAlgebra*>& outputAmbientSSalgebra
  );
  template <class Coefficient>
  bool getTypeHighestWeightParabolic(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vector<Coefficient>& outputWeightHWFundcoords,
    Selection& outputInducingSelection,
    WithContext<SemisimpleLieAlgebra*>& outputAmbientSemisimpleLieAlgebra
  );
  void addEmptyHeadedCommand();
  Calculator();
  int addOperationNoRepetitionOrReturnIndexFirst(
    const std::string& operationName
  );
  void addOperationNoRepetitionAllowed(const std::string& operation);
  void addBuiltInType(const std::string& operationBuiltIn);
  void addKnownDoubleConstant(
    const std::string& constantName, double value
  );
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
    const List<std::string>* parentsThatBanHandler = nullptr
  );
  void addOneStringAtomHandler(
    int atom, Expression::ToStringHandler handler
  );
  void addOneStringCompositeHandler(
    int atom, Expression::ToStringHandler handler
  );
  template <class builtInType>
  void addOneBuiltInHandler();
  void addOneStringHandler(
    int atom,
    Expression::ToStringHandler handler,
    MapList<
      int, Expression::ToStringHandler, HashFunctions::hashFunction
    >& handlerCollection
  );
  void reset();
  void initialize(Calculator::Mode desiredMode);
  void initializeLogDuration(Calculator::Mode desiredMode);
  void initializeToStringHandlers();
  void initializeAtomsThatFreezeArguments();
  void initializeBuiltInsFreezeArguments();
  void initializeAtomsNonCacheable();
  void initializeAtomsThatAllowCommutingOfArguments();
  void initializeOperationsInterpretedAsFunctionsMultiplicatively();
  void initializeOperationsThatAreKnownFunctions();
  void initializeArithmeticOperations();
  void initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  void initializeBuiltInAtomsNotInterpretedAsFunctions();
  void initializeAtomsNotGoodForChainRule();
  void initializeFunctionsStandard();
  void initializeFunctionsScientificBasic();
  void initializeFunctionsVectorPartitionFunctions();
  void initializeFunctionsExtra();
  void initializeFunctionsCryptoAndEncoding();
  void initializeFunctionsSemisimpleLieAlgebras();
  void initializePredefinedStandardOperationsWithoutHandler();
  void initializeAdminFunctions();
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
  void storeCache();
  class EvaluateLoop {
  private:
    bool detectLoops();
    void accountIntermediateState();
    bool reduceUsingCache();
    bool builtInEvaluation();
    bool userDefinedEvaluation();
    bool evaluateChildren(StateMaintainerCalculator& maintainRuleStack);
    void reportChildEvaluation(Expression& output, int childIndex);
    void storeCache();
  public:
    Calculator* owner;
    bool flagIsNonCacheable;
    int opIndexParent;
    int numberOfTransformations;
    int indexInCache;
    ProgressReport report;
    bool reductionOccurred;
    Expression* output;
    Expression* history;
    HashedList<Expression> intermediateOutputs;
    Expression currentChild;
    // Reduces expression using a cached value
    bool reduceOnce();
    void writeCache();
    bool outputHasErrors();
    EvaluateLoop(Calculator& inputOwner);
    bool setOutput(
      const Expression& input,
      Function* handler,
      const std::string& info
    );
    void accountHistory(Function* handler, const std::string& info);
    bool checkInitialization();
    void accountHistoryChildTransformation(
      const Expression& transformedChild,
      const Expression& childHistory,
      int childIndex
    );
  };

  void reduce(Calculator::EvaluateLoop& state);
  class ExpressionHistoryEnumerator {
  public:
    class Step {
    public:
      Expression content;
      List<std::string> annotations;
      std::string stepType;
      void assignContentAndAnnotation(
        const Expression& input, const std::string& oneAnnotation
      );
      void mergeAnnotations(const List<std::string>& incoming);
      JSData toJSON();
    };

    Calculator* owner;
    int recursionDepth;
    int maximumRecursionDepth;
    Expression history;
    List<Expression> output;
    List<List<std::string> > rulesNames;
    static List<std::string> ruleIgnoreList;
    static HashedList<std::string> ruleIgnoreListHashList;
    HashedList<std::string> rulesToBeIgnored;
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    > rulesDisplayNamesMap;
    bool computeRecursively(
      int incomingRecursionDepth, std::stringstream* commentsOnFailure
    );
    bool processTransformation(
      const Expression& current, std::stringstream* commentsOnFailure
    );
    bool processChildrenTransformations(
      int startIndex,
      int numberOfChildren,
      std::stringstream* commentsOnFailure
    );
    void initializeComputation();
    ExpressionHistoryEnumerator();
    bool checkInitialization() {
      if (this->owner == nullptr) {
        global.fatal
        << "Expression history enumerator has zero owner. "
        << global.fatal;
      }
      return true;
    }
    void toSteps(
      List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
    );
    void toStepsNoMerging(
      List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
    );
    void toStepsWithMerge(
      List<Calculator::ExpressionHistoryEnumerator::Step>& unmerged,
      List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
    );
    void toStepsCleanUp(
      List<Calculator::ExpressionHistoryEnumerator::Step>& raw,
      List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
    );
    bool isIgnorable(
      const Calculator::ExpressionHistoryEnumerator::Step& step
    );
    void toOneStep(
      int stepIndex,
      Calculator::ExpressionHistoryEnumerator::Step& outputStep
    );
    std::string toStringExpressionHistoryMerged();
    std::string toStringDebug();
  };

  void evaluate(const std::string& input);
  // Attempts to interpret the input string as a
  // high-school or calculus problem
  // and solve it.
  JSData solve(const std::string& input);
  // For internal use only; will crash
  // the calculator if the input has syntax
  // errors.
  Expression parseOrCrash(
    const std::string& input, bool stripCommandSequence
  );
};

class CalculatorBasics {
public:
  static bool tensorProductStandard(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool standardIsDenotedBy(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool minus(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool combineFractions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool checkRule(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool associate(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool subZeroDivAnythingWithZero(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool cancelMultiplicativeInverse(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool associateExponentExponent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool associateTimesDivision(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool flattenCommandEnclosuresOneLayer(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionFlattenCommandEnclosuresOneLayer(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAtoXtimesAtoYequalsAtoXplusY(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool extractBaseMultiplication(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool meltBrackets(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyByOne(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool timesToFunctionApplication(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool distributeTimes(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool distributeTimesConstant(
    Calculator& calculator, const Expression& input, Expression& output
  );
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
  static bool evaluateIf(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorSteps {
public:
  static bool logEvaluationStepsDebug(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logEvaluationSteps(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logEvaluationStepsHumanReadableMerged(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doDebug
  );
};

class CalculatorConversions {
private:
  template <class Coefficient>
  static bool extractPolynomialFromSumDifferenceOrProduct(
    Calculator& calculator,
    const Expression& input,
    WithContext<Polynomial<Coefficient> >& output,
    int maximumVariables,
    int maximumPowerToExpand,
    bool acceptNonPositiveOrNonIntegerPowers
  );
  template <class Coefficient>
  static bool extractPolynomialFromPower(
    Calculator& calculator,
    const Expression& input,
    WithContext<Polynomial<Coefficient> >& output,
    int maximumVariables,
    int maximumPowerToExpand,
    bool acceptNonPositiveOrNonIntegerPowers
  );
  template <class Coefficient>
  static bool extractPolynomialMakeAtom(
    Calculator& calculator,
    const Expression& input,
    WithContext<Polynomial<Coefficient> >& output
  );
public:
  // Conversion from expression tree.
  // Conversions from expression tree to type.
  static bool loadElementSemisimpleLieAlgebraRationalCoefficients(
    Calculator& calculator,
    const Expression& input,
    ElementSemisimpleLieAlgebra<Rational>& output,
    SemisimpleLieAlgebra& owner
  );
  static bool loadElementSemisimpleLieAlgebraAlgebraicNumbers(
    Calculator& calculator,
    const Expression& input,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
    SemisimpleLieAlgebra& owner
  );
  static bool dynkinType(
    Calculator& calculator, const Expression& input, DynkinType& output
  );
  static bool functionDynkinType(
    Calculator& calculator, const Expression& input, DynkinType& output
  );
  static bool dynkinSimpleType(
    Calculator& calculator,
    const Expression& input,
    DynkinSimpleType& output
  );
  static bool functionDynkinSimpleType(
    Calculator& calculator,
    const Expression& input,
    DynkinSimpleType& output
  );
  static bool slTwoSubalgebraPrecomputed(
    Calculator& calculator,
    const Expression& input,
    SlTwoSubalgebra& output
  );
  static bool algebraicNumber(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Coefficient>
  static bool getPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  // Conversions from expression tree to expression containing type.
  template <class Coefficient>
  static bool functionPolynomial(
    Calculator& calculator,
    const Expression& input,
    WithContext<Polynomial<Coefficient> >& output,
    int maximumVariables,
    int maximumPowerToExpand,
    bool acceptNonPositiveOrNonIntegerPowers
  );
  static bool rationalFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Coefficient>
  static bool functionRationalFraction(
    Calculator& calculator,
    const Expression& input,
    WithContext<RationalFraction<Coefficient> >& output,
    bool allowNonIntegerPowers
  );
  static bool elementUniversalEnveloping(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    SemisimpleLieAlgebra& inputOwner
  );
  static bool outerMatrixExpressionsToMatrixOfType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool makeMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionMatrixDouble(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionMatrixAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionMatrixRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool matrixRationalFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionMatrixRationalFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool matrixRationalTensorForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionMatrixRationalTensorForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool loadFileIntoString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool makeElementHyperOctahedral(
    Calculator& calculator, const Expression& input, Expression& output
  );
  // Conversion to expression tree.
  // Converstion from type to expression tree.
  template <class Coefficient>
  static bool expressionFromPolynomial(
    Calculator& calculator,
    const Polynomial<Coefficient>& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  template <class Coefficient>
  static bool expressionFromRationalFraction(
    Calculator& calculator,
    const RationalFraction<Coefficient>& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  template <class Coefficient>
  static bool expressionFromRationalFraction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool loadKey(
    Calculator& calculator,
    const Expression& inputStatementList,
    const std::string& inputKey,
    Expression& output
  );
  static bool loadKeysFromStatementList(
    Calculator& calculator,
    const Expression& input,
    MapList<std::string, Expression>& output,
    bool allowFailure,
    List<Expression>* outputNotAKeyValuePair,
    std::stringstream* commentsOnFailure
  );
  // Extract keys form a list of equalities. For example,
  // given input such as
  // (a = 1, b=2),
  // this function will extract the map {a:1, b:2}.
  // If you don't set allowFailure,
  // then input such as
  // (a=1,b=2, c)
  // will fail. If you set allowFailure=true,
  // the input above will ignore c and return the map {a:1, b:2}.
  static bool loadKeysFromStatementList(
    Calculator& calculator,
    const Expression& input,
    MapList<Expression, Expression>& output,
    bool allowFailure,
    List<Expression>* outputNotAKeyValuePair,
    std::stringstream* commentsOnFailure
  );
  static bool storeSemisimpleSubalgebras(
    Calculator& calculator,
    const SemisimpleSubalgebras& input,
    Expression& output
  );
  static std::string stringFromSemisimpleSubalgebras(
    SemisimpleSubalgebras& input
  );
  static bool expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
    Calculator& calculator,
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input,
    Expression& output
  );
  static bool expressionFromElementUniversalEnveloping(
    Calculator& calculator,
    const ElementUniversalEnveloping<RationalFraction<Rational> >& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  static bool storeCandidateSubalgebra(
    Calculator& calculator,
    const CandidateSemisimpleSubalgebra& input,
    Expression& output
  );
  static bool expressionFromDynkinType(
    Calculator& calculator, const DynkinType& input, Expression& output
  );
  static bool expressionFromDynkinSimpleType(
    Calculator& calculator,
    const DynkinSimpleType& input,
    Expression& output
  );
  static bool expressionFromElementSemisimpleLieAlgebraRationals(
    Calculator& calculator,
    const ElementSemisimpleLieAlgebra<Rational>& input,
    Expression& output
  );
  // Conversions from expression containing type to expression tree.
  static bool storeSemisimpleLieAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionSemisimpleLieAlgebraFromDynkinType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionSemisimpleLieAlgebraFromDynkinType(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    WithContext<SemisimpleLieAlgebra*>& outputSSalgebra
  );
  static bool semisimpleLieAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool candidateSubalgebraPrecomputed(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    CandidateSemisimpleSubalgebra& outputPointer,
    SemisimpleSubalgebras& owner
  );
  static bool loadWeylGroup(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool storeSemisimpleSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool slTwoSubalgebraPrecomputed(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool loadSemisimpleSubalgebras(
    Calculator& calculator, const Expression& inpuT, Expression& output
  );
  static bool expressionFromChevalleyGenerator(
    Calculator& calculator,
    const ChevalleyGenerator& input,
    Expression& output
  );
  static bool expressionFromMonomialUniversalEnveloping(
    Calculator& calculator,
    const MonomialUniversalEnveloping<RationalFraction<Rational> >& input,
    Expression& output,
    ExpressionContext* inputContext = nullptr
  );
  static bool functionExpressionFromBuiltInType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool expressionFromBuiltInType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Coefficient>
  static bool functionExpressionFromPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool expressionFromElementUniversalEnveloping(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Type>
  static bool convert(
    Calculator& calculator,
    const Expression& input,
    WithContext<Type>& output
  );
  // Determines whether the input can be converted to the
  // output type using internal C++ conversions (rather than
  // explicit Expression computations).
  // If conversion between the two types is not desired
  // by either C++ or Expression computations, this
  // will be recorded in
  // outputConversionByOtherMeansNotDesired (if provided).
  // Example when a conversion is not desired:
  // conversion from polynomial with modular coefficients and
  // a polynomial with algebraic coefficients.
  template <class Type>
  static bool convertWithoutComputation(
    Calculator& calculator,
    const Expression& input,
    WithContext<Type>& output,
    bool* outputConversionByOtherMeansNotDesired = nullptr
  );
  template <class Type>
  static bool functionGetMatrixNoComputation(
    Calculator& calculator,
    const Expression& input,
    Matrix<Type>& outputMatrix
  ) {
    return
    CalculatorConversions::functionGetMatrix(
      calculator, input, outputMatrix, false
    );
  }
  template <class Type>
  static bool functionGetMatrixInternal(
    Calculator& calculator,
    const Expression& input,
    Matrix<Type>& outputMatrix,
    bool convertWithComputation,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetNumberOfColumnsNonMandatory = - 1,
    std::stringstream* commentsOnError = nullptr
  );
  template <class Type>
  static bool functionGetMatrix(
    Calculator& calculator,
    const Expression& input,
    Matrix<Type>& outputMatrix,
    bool convertWithComputation,
    ExpressionContext* inputOutputStartingContext = nullptr,
    int targetNumberOfColumnsNonMandatory = - 1,
    std::stringstream* commentsOnError = nullptr
  );
  template <class Type>
  static bool getListPolynomialVariableLabelsLexicographic(
    Calculator& calculator,
    const Expression& input,
    Vector<Polynomial<Type> >& output,
    ExpressionContext& outputContext
  );
};

template <class Type>
bool Calculator::getVector(
  const Expression& input,
  Vector<Type>& output,
  ExpressionContext* inputOutputStartingContext,
  int targetDimensionNonMandatory
) {
  STACK_TRACE("Calculator::getVector");
  input.checkInitialization();
  List<Expression> nonConvertedExpressions;
  if (
    !this->getVectorExpressions(
      input, nonConvertedExpressions, targetDimensionNonMandatory
    )
  ) {
    return false;
  }
  if (targetDimensionNonMandatory > 0) {
    if (nonConvertedExpressions.size != targetDimensionNonMandatory) {
      return false;
    }
  }
  List<WithContext<Type> > outputCandidate;
  outputCandidate.setSize(nonConvertedExpressions.size);
  ExpressionContext context;
  context.initialize(*this);
  if (inputOutputStartingContext == nullptr) {
    inputOutputStartingContext = &context;
  }
  for (int i = 0; i < nonConvertedExpressions.size; i ++) {
    nonConvertedExpressions[i].checkInitialization();
    if (
      !CalculatorConversions::convert<Type>(
        *this, nonConvertedExpressions[i], outputCandidate[i]
      )
    ) {
      return false;
    }
    // Expand the context to be the combination of all contexts.
    if (
      !outputCandidate[i].setContextAtLeast(
        *inputOutputStartingContext, &this->comments
      )
    ) {
      return false;
    }
  }
  output.setSize(outputCandidate.size);
  for (int i = 0; i < outputCandidate.size; i ++) {
    if (
      !outputCandidate[i].setContextAtLeast(
        *inputOutputStartingContext, &this->comments
      )
    ) {
      return false;
    }
    output[i] = outputCandidate[i].content;
  }
  return true;
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Rational> >& output
);

template <class Coefficient>
bool CalculatorConversions::functionExpressionFromPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionExpressionFromPolynomial");
  if (!input.isOfType<Polynomial<Coefficient> >()) {
    return false;
  }
  const Polynomial<Coefficient>& polynomial =
  input.getValue<Polynomial<Coefficient> >();
  ExpressionContext context = input.getContext();
  return
  CalculatorConversions::expressionFromPolynomial(
    calculator, polynomial, output, &context
  );
}

template <class Type>
bool Expression::isMatrixOfType(
  int* outputNumberOfRows, int* outputNumberOfColumns
) const {
  if (!this->isMatrix()) {
    return false;
  }
  if (!(*this)[0].startsWith(this->owner->opMatrix(), 2)) {
    return false;
  }
  if (
    !(*this)[0][1].isOperationGiven(this->getBuiltInType<Type>())
  ) {
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
  if (outputNumberOfRows != nullptr) {
    *outputNumberOfRows = rows.size() - 1;
  }
  if (outputNumberOfColumns != nullptr) {
    *outputNumberOfColumns = firstRow.size() - 1;
  }
  return true;
}

template <class Type>
bool Expression::isOfTypeWithContext(WithContext<Type>* whichElement) const {
  STACK_TRACE("Expression::isOfTypeWithContext");
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->getBuiltInType<Type>())) {
    return false;
  }
  if (this->size() < 2 || !this->getLastChild().isAtom()) {
    return false;
  }
  if (whichElement == 0) {
    return true;
  }
  whichElement->context = this->getContext();
  whichElement->content = this->getValue<Type>();
  return true;
}

template <class Type>
bool CalculatorConversions::functionGetMatrix(
  Calculator& calculator,
  const Expression& input,
  Matrix<Type>& outputMatrix,
  bool convertWithComputation,
  ExpressionContext* inputOutputStartingContext,
  int targetNumberOfColumnsNonMandatory,
  std::stringstream* commentsOnError
) {
  if (
    CalculatorConversions::functionGetMatrixInternal(
      calculator,
      input,
      outputMatrix,
      convertWithComputation,
      inputOutputStartingContext,
      targetNumberOfColumnsNonMandatory,
      commentsOnError
    )
  ) {
    return true;
  }
  if (
    input.size() != 2 ||
    input.isSequenceNElements() ||
    input.isMatrix() ||
    input.isIntervalRealLine()
  ) {
    return false;
  }
  return
  CalculatorConversions::functionGetMatrixInternal(
    calculator,
    input[1],
    outputMatrix,
    convertWithComputation,
    inputOutputStartingContext,
    targetNumberOfColumnsNonMandatory,
    commentsOnError
  );
}

template <class Type>
bool CalculatorConversions::functionGetMatrixInternal(
  Calculator& calculator,
  const Expression& input,
  Matrix<Type>& outputMatrix,
  bool convertWithComputation,
  ExpressionContext* inputOutputStartingContext,
  int targetNumberOfColumnsNonMandatory,
  std::stringstream* commentsOnError
) {
  STACK_TRACE("CalculatorConversions::functionGetMatrixInternal");
  Matrix<Expression> matrixExpressions;
  Matrix<WithContext<Type> > outputCandidate;
  Expression transformer = input;
  if (transformer.size() > 0) {
    Expression matrixStart(calculator);
    matrixStart.addChildAtomOnTop(calculator.opMatrix());
    transformer.setChild(0, matrixStart);
  }
  if (
    !calculator.getMatrixExpressions(
      transformer, matrixExpressions, - 1, targetNumberOfColumnsNonMandatory
    )
  ) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Failed to extract matrix of expressions from "
      << input.toString();
    }
    return false;
  }
  outputCandidate.initialize(
    matrixExpressions.numberOfRows, matrixExpressions.numberOfColumns
  );
  ExpressionContext context(calculator);
  if (inputOutputStartingContext != nullptr) {
    context = *inputOutputStartingContext;
  }
  for (int i = 0; i < outputCandidate.numberOfRows; i ++) {
    for (int j = 0; j < outputCandidate.numberOfColumns; j ++) {
      WithContext<Type>& current = outputCandidate(i, j);
      bool success = false;
      if (convertWithComputation) {
        success =
        CalculatorConversions::convert(
          calculator, matrixExpressions(i, j), current
        );
      } else {
        success =
        CalculatorConversions::convertWithoutComputation(
          calculator, matrixExpressions(i, j), current
        );
      }
      if (!success) {
        if (commentsOnError != nullptr) {
          *commentsOnError
          << "Failed to convert matrix element: "
          << "row: "
          << i
          << ", column: "
          << j
          << ", expression: "
          << matrixExpressions(i, j).toString()
          << ". ";
        }
        return false;
      }
      context.mergeContexts(current.context, context);
    }
  }
  for (int i = 0; i < outputCandidate.numberOfRows; i ++) {
    for (int j = 0; j < outputCandidate.numberOfColumns; j ++) {
      WithContext<Type>& current = outputCandidate(i, j);
      if (!current.setContextAtLeast(context, commentsOnError)) {
        return false;
      }
    }
  }
  outputMatrix.initialize(
    outputCandidate.numberOfRows, outputCandidate.numberOfColumns
  );
  for (int i = 0; i < outputMatrix.numberOfRows; i ++) {
    for (int j = 0; j < outputMatrix.numberOfColumns; j ++) {
      outputMatrix(i, j) = outputCandidate(i, j).content;
    }
  }
  if (inputOutputStartingContext != nullptr) {
    *inputOutputStartingContext = context;
  }
  return true;
}

template <class Coefficient>
bool Expression::makeSum(
  Calculator& calculator,
  const LinearCombination<Expression, Coefficient>& summands
) {
  STACK_TRACE("Expression::makeSum");
  Expression oneE;
  // used to record the constant term
  oneE.assignValue<Rational>(calculator, 1);
  if (summands.isEqualToZero()) {
    return this->assignValue<Rational>(calculator, 0);
  }
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.setSize(summands.size());
  for (int i = 0; i < summands.size(); i ++) {
    Expression& current = summandsWithCoeff[i];
    if (summands[i] == oneE) {
      current.assignValue(calculator, summands.coefficients[i]);
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
          global.fatal
          << "Bad comparison "
          << "function: each of the expressions "
          << summandsWithCoeff[i].toString()
          << " and "
          << summandsWithCoeff[j].toString()
          << " is reported to be greater than the other. "
          << global.fatal;
        }
      }
    }
  }
  return
  this->makeOXdotsX(calculator, calculator.opPlus(), summandsWithCoeff);
}

template < >
int Expression::getBuiltInType<Rational>() const;
template < >
int Expression::addObjectReturnIndex(const Rational& inputValue) const;

template <class Type>
bool Expression::assignValueWithContext(
  Calculator& owner,
  const Type& inputValue,
  const ExpressionContext& context
) {
  this->reset(owner, 3);
  this->addChildAtomOnTop(this->getBuiltInType<Type>());
  this->addChildOnTop(context.toExpression());
  int objectIndex = this->addObjectReturnIndex(inputValue);
  this->addChildAtomOnTop(objectIndex);
  this->checkConsistency();
  return true;
}

template <class Type>
bool Expression::assignValue(Calculator& owner, const Type& inputValue) {
  Expression typeComputer;
  typeComputer.owner = &owner;
  int currentType = typeComputer.getBuiltInType<Type>();
  if (
    currentType == owner.opEltZmodP() ||
    currentType == owner.opPolynomialRational() ||
    currentType == owner.opRationalFraction() ||
    currentType == owner.opElementTensorGVM() ||
    currentType == owner.opElementUEOverRF() ||
    currentType == owner.opElementWeylAlgebra() ||
    currentType == owner.opWeightLieAlgPoly()
  ) {
    global.fatal
    << "Assigning value WITHOUT CONTEXT to data type "
    << owner.getOperations()[currentType]
    << " is considered an error. "
    << "If you have legitimate reasons for doing so, "
    << "please refactor by passing empty context."
    << global.fatal;
  }
  ExpressionContext emptyContext(owner);
  return this->assignValueWithContext(owner, inputValue, emptyContext);
}

template <class Type>
bool Expression::mergeContextsMyArumentsAndConvertThem(
  Expression& output, std::stringstream* commentsOnFailure
) const {
  STACK_TRACE("Expression::mergeContextsMyArumentsAndConvertThem");
  this->checkInitialization();
  Expression mergedContexts;
  if (!this->mergeContextsMyAruments(mergedContexts, commentsOnFailure)) {
    return false;
  }
  output.reset(*this->owner, this->children.size);
  output.addChildOnTop((*this)[0]);
  WithContext<Type> converted;
  for (int i = 1; i < mergedContexts.size(); i ++) {
    if (
      !CalculatorConversions::convertWithoutComputation(
        *this->owner, mergedContexts[i], converted
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>Failed to convert "
        << mergedContexts[i].toString()
        << " to the desired type. ";
      }
      return false;
    }
    Expression convertedExpression;
    convertedExpression.assignWithContext(*this->owner, converted);
    output.addChildOnTop(convertedExpression);
  }
  return true;
}

template <class Coefficient>
bool Calculator::getTypeWeight(
  Calculator& calculator,
  const Expression& input,
  Vector<Coefficient>& outputWeightSimpleCoords,
  WithContext<SemisimpleLieAlgebra*>& outputAmbientSemisimpleLieAlgebra
) {
  STACK_TRACE("Calculator::getTypeWeight");
  if (input.size() != 3) {
    return
    calculator
    << "Function typeHighestWeightParabolic "
    << "is expected to have two arguments: "
    << "Semisimple algebra type, highest weight in simple coordinates. ";
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  outputAmbientSemisimpleLieAlgebra.content = nullptr;
  if (
    !CalculatorConversions::convert(
      *this, leftE, outputAmbientSemisimpleLieAlgebra
    )
  ) {
    calculator << "Error extracting Lie algebra from " << leftE.toString();
    return false;
  }
  SemisimpleLieAlgebra* ambientSemisimpleLieAlgebra =
  outputAmbientSemisimpleLieAlgebra.content;
  if (
    !calculator.getVector<Coefficient>(
      middleE,
      outputWeightSimpleCoords,
      &outputAmbientSemisimpleLieAlgebra.context,
      ambientSemisimpleLieAlgebra->getRank()
    )
  ) {
    calculator
    << "Failed to convert the second "
    << "argument of HWV to a list of "
    << ambientSemisimpleLieAlgebra->getRank()
    << " polynomials. The second argument you gave is "
    << middleE.toString()
    << ".";
    return false;
  }
  if (
    !calculator.objectContainer.semisimpleLieAlgebras.contains(
      ambientSemisimpleLieAlgebra->weylGroup.dynkinType
    )
  ) {
    global.fatal
    << ambientSemisimpleLieAlgebra->toStringLieAlgebraName()
    << " contained object container more than once. "
    << global.fatal;
  }
  int algebraIndex =
  calculator.objectContainer.semisimpleLieAlgebras.getIndex(
    ambientSemisimpleLieAlgebra->weylGroup.dynkinType
  );
  outputAmbientSemisimpleLieAlgebra.context.setIndexAmbientSemisimpleLieAlgebra
  (algebraIndex);
  return true;
}

template <class builtInType>
void Calculator::addOneBuiltInHandler() {
  Expression typeConverter(*this);
  this->addOneStringHandler(
    typeConverter.getBuiltInType<builtInType>(),
    Expression::toStringBuiltIn<builtInType>,
    this->toStringDataHandlers
  );
}

template <class Coefficient>
bool Calculator::getTypeHighestWeightParabolic(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vector<Coefficient>& outputWeightHWFundcoords,
  Selection& outputInducingSelection,
  WithContext<SemisimpleLieAlgebra*>& outputAmbientSemisimpleLieAlgebra
) {
  if (!input.isListNElements(4) && !input.isListNElements(3)) {
    return
    output.assignError(
      calculator,
      "Function typeHighestWeightParabolic is "
      "expected to have two or three arguments: "
      "semisimple Lie algebra type, highest weight, "
      "[optional] parabolic selection. "
    );
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  if (
    !CalculatorConversions::convert(
      calculator, leftE, outputAmbientSemisimpleLieAlgebra
    )
  ) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra* ambientSemisimpleLieAlgebra =
  outputAmbientSemisimpleLieAlgebra.content;
  if (
    !calculator.getVector<Coefficient>(
      middleE,
      outputWeightHWFundcoords,
      &outputAmbientSemisimpleLieAlgebra.context,
      ambientSemisimpleLieAlgebra->getRank()
    )
  ) {
    std::stringstream tempStream;
    tempStream
    << "Failed to convert the second argument of "
    << "highest weight vector to a list of "
    << ambientSemisimpleLieAlgebra->getRank()
    << " polynomials. The second argument you gave is "
    << middleE.toString()
    << ".";
    return output.assignError(calculator, tempStream.str());
  }
  if (input.isListNElements(4)) {
    Vector<Rational> parabolicSelection;
    const Expression& rightE = input[3];
    if (
      !calculator.getVector<Rational>(
        rightE,
        parabolicSelection,
        &outputAmbientSemisimpleLieAlgebra.context,
        ambientSemisimpleLieAlgebra->getRank()
      )
    ) {
      std::stringstream tempStream;
      tempStream
      << "Failed to convert the third argument "
      << "of highest weight vector to a list of "
      << ambientSemisimpleLieAlgebra->getRank()
      << " rationals. The third argument you gave is "
      << rightE.toString()
      << ".";
      return output.assignError(calculator, tempStream.str());
    }
    outputInducingSelection = parabolicSelection;
  } else {
    outputInducingSelection.initialize(
      ambientSemisimpleLieAlgebra->getRank()
    );
    for (int i = 0; i < outputWeightHWFundcoords.size; i ++) {
      if (!outputWeightHWFundcoords[i].isSmallInteger()) {
        outputInducingSelection.addSelectionAppendNewIndex(i);
      }
    }
  }
  if (
    !calculator.objectContainer.semisimpleLieAlgebras.contains(
      ambientSemisimpleLieAlgebra->weylGroup.dynkinType
    )
  ) {
    global.fatal
    << ambientSemisimpleLieAlgebra->toStringLieAlgebraName()
    << " contained object container more than once. "
    << global.fatal;
  }
  int algebraIndex =
  calculator.objectContainer.semisimpleLieAlgebras.getIndex(
    ambientSemisimpleLieAlgebra->weylGroup.dynkinType
  );
  outputAmbientSemisimpleLieAlgebra.context.setIndexAmbientSemisimpleLieAlgebra
  (algebraIndex);
  return true;
}

template <class Coefficient>
bool Expression::makeMatrix(
  Calculator& owner,
  const Matrix<Coefficient>& input,
  const ExpressionContext* inputContext,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix
) {
  STACK_TRACE("Expression::assignMatrix");
  Matrix<Expression> matrixExpressions;
  matrixExpressions.initialize(input.numberOfRows, input.numberOfColumns);
  Expression currentElt;
  for (int i = 0; i < input.numberOfRows; i ++) {
    for (int j = 0; j < input.numberOfColumns; j ++) {
      if (inputContext == nullptr) {
        currentElt.assignValue(owner, input(i, j));
      } else {
        currentElt.assignValueWithContext(
          owner, input(i, j), *inputContext
        );
      }
      matrixExpressions(i, j) = currentElt;
    }
  }
  return
  this->assignMatrixExpressions(
    matrixExpressions,
    owner,
    reduceOneRowToSequenceAndOneByOneToNonMatrix,
    false
  );
}

template <class Coefficient>
bool CalculatorConversions::expressionFromPolynomial(
  Calculator& calculator,
  const Polynomial<Coefficient>& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  STACK_TRACE("CalculatorConversions::expressionFromPolynomial");
  LinearCombination<Expression, Coefficient> terms;
  Expression currentBase, currentPower, currentTerm, currentMultTermE;
  if (!input.isConstant() && inputContext == nullptr) {
    calculator
    << "While converting polynomial to expression, "
    << "I was given no variable names. Using the "
    << "default variable names x_1, x_2, ... "
    << "Please make sure you are not using those "
    << "variables for other purposes.";
  }
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].isConstant()) {
      currentTerm.assignValue(calculator, 1);
      terms.addMonomial(currentTerm, input.coefficients[i]);
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
          currentPower.assignValue(calculator, input[i](j));
          currentMultTermE.makeXOX(
            calculator, calculator.opPower(), currentBase, currentPower
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
    terms.addMonomial(currentTerm, input.coefficients[i]);
  }
  return output.makeSum(calculator, terms);
}

template <class Coefficient>
bool CalculatorConversions::expressionFromRationalFraction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::expressionFromRationalFraction");
  if (!input.isOfType<RationalFraction<Coefficient> >()) {
    return false;
  }
  const RationalFraction<Coefficient>& rationalFunction =
  input.getValue<RationalFraction<Coefficient> >();
  ExpressionContext context = input.getContext();
  return
  CalculatorConversions::expressionFromRationalFraction<Coefficient>(
    calculator, rationalFunction, output, &context
  );
}

template <class Coefficient>
bool CalculatorConversions::expressionFromRationalFraction(
  Calculator& calculator,
  const RationalFraction<Coefficient>& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  STACK_TRACE("CalculatorConversions::expressionFromRationalFraction");
  Rational aConst;
  if (input.isConstant(&aConst)) {
    return output.assignValue(calculator, aConst);
  }
  Polynomial<Coefficient> numerator, denominator;
  input.getNumerator(numerator);
  if (
    input.isConstant() ||
    input.expressionType ==
    RationalFraction<Coefficient>::TypeExpression::typePolynomial
  ) {
    return
    CalculatorConversions::expressionFromPolynomial<Coefficient>(
      calculator, numerator, output, inputContext
    );
  }
  Expression numeratorExpression, denominatorExpression;
  input.getDenominator(denominator);
  Polynomial<Coefficient> numeratorRescaled = numerator;
  Polynomial<Coefficient> denominatorRescaled = denominator;
  Coefficient topMultiple =
  numeratorRescaled.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  Coefficient bottomMultiple =
  denominatorRescaled.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  Coefficient multipleTopBottom = bottomMultiple / topMultiple;
  numeratorRescaled *= multipleTopBottom.getNumerator();
  denominatorRescaled *= multipleTopBottom.getDenominator();
  CalculatorConversions::expressionFromPolynomial<Coefficient>(
    calculator, numeratorRescaled, numeratorExpression, inputContext
  );
  CalculatorConversions::expressionFromPolynomial<Coefficient>(
    calculator, denominatorRescaled, denominatorExpression, inputContext
  );
  output.makeXOX(
    calculator,
    calculator.opDivide(),
    numeratorExpression,
    denominatorExpression
  );
  output.checkConsistency();
  output.checkInitialization();
  return true;
}

template <class BuiltIn>
std::string WithContext<BuiltIn>::toString() const {
  std::stringstream out;
  out
  << this->content.toString()
  << " [context: "
  << this->context.toString()
  << "]";
  return out.str();
}

template <class BuiltIn>
bool WithContext<BuiltIn>::setContextAtLeast(
  ExpressionContext& inputOutputContext,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("WithContext::setContextAtLeast");
  ExpressionContext newContext;
  if (!inputOutputContext.mergeContexts(this->context, newContext)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to merge context "
      << inputOutputContext.toString()
      << " with "
      << this->context.toString();
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
  bool result = output.assignWithContext(*this->context.owner, *this);
  return result;
}

template <class BuiltIn>
bool WithContext<BuiltIn>::mergeContexts(
  WithContext<BuiltIn>& leftOutput,
  WithContext<BuiltIn>& rightOutput,
  std::stringstream* commentsOnFailure
) {
  ExpressionContext resultContext;
  if (
    !leftOutput.context.mergeContexts(rightOutput.context, resultContext)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to merge contexts.";
    }
    return false;
  }
  return
  leftOutput.extendContext(resultContext, commentsOnFailure) &&
  rightOutput.extendContext(resultContext, commentsOnFailure);
}

template <class Type>
bool CalculatorConversions::getListPolynomialVariableLabelsLexicographic(
  Calculator& calculator,
  const Expression& input,
  Vector<Polynomial<Type> >& output,
  ExpressionContext& outputContext
) {
  STACK_TRACE(
    "CalculatorConversions::getListPolynomialVariableLabelsLexicographic"
  );
  ExpressionContext contextStart(calculator);
  if (
    !calculator.getVectorFromFunctionArguments(
      input, output, &contextStart, 0
    )
  ) {
    return false;
  }
  if (output.size < 2) {
    return false;
  }
  int numberOfVariables = contextStart.numberOfVariables();
  HashedList<Expression> variables;
  variables.setExpectedSize(numberOfVariables);
  for (int i = 0; i < numberOfVariables; i ++) {
    variables.addOnTop(contextStart.getVariable(i));
  }
  variables.quickSortAscending();
  return outputContext.setVariables(variables);
}

#endif // header_calculator_interface_ALREADY_INCLUDED
