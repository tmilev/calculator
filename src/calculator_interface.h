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

static ProjectInformationInstance ProjectInfoVpfHeader2(__FILE__, "Header file, calculator declaration. ");

class Expression {
private:
  void reset() {
    this->owner = nullptr;
    this->children.size = 0;
    this->theData = - 1;
  }
  bool SetChilD(int childIndexInMe, int childIndexInBoss);
  // Definitions.
  // 1. Fundamentals.
  // 1.1. An atom is an expression with zero children.
  //      *******************************************
  //      We will say informally "an atom equals (the keyword) X" to mean that
  //      the theData of the corresponding atom equals the integer Calculator::opX().
  //      Note that this language use is completely informal, and could be ambiguous:
  //      the theData entry of an atom can be interpreted either as keyword or as an
  //      an actual piece of data (not necessarily equal to Calculator::opX() for some X).
  //      Whenever this ambiguity becomes an issue, the informal language should be dropped,
  //      and explicit reference to the Expression::theData and Expression::children members
  //      should be made.
  // 1.2. A list is an expression with 1 or more children whose theData entry equals
  //      0. The atom 0 *MUST* be equal to Calculator::opList().
  // 1.3. An expression with 1 or more children is not allowed to have theData entry different
  //      from 0 = Calculator::opList(). The system is instructed to
  //      crash and burn shall such a configuration be detected.
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
  // position in Calculator::theExpressionContainer.
  // Calculator::theExpressionContainer is a Hashed List of references and must not be modified
  // directly in any way.
  // Motivation for this implementation choice. The original implementation
  // had Expression contain all of its children as List<Expression>, making the copy operator=
  // a recursive function. While this is one of the least-head-aching designs,
  // it also proved to be remarkably slow: here is why.
  // When processing an expression, one must traverse all of its subtrees.
  // Making temporary copies of all subtrees is then approximately O(n^2),
  // where n^2 is the number of nodes of the tree.
  //
  // For a tree with 1k nodes this is already
  // unacceptably slow, if it could be avoided.
  //
  // If change is to be applied to
  // most subtrees, then the approximate cost of O(n^2) operations obviously cannot be avoided.
  // However, in most computations, subtrees need not be changed - in reality, most expression
  // will arrive to the calculator in reduced or close-to-reduced form.
  //
  // This explains our choice of keeping all children of an expression as a reference to
  // an ever-growing collection of Expressions.
  // If a single instance of Calculator is to run over long periods of time,
  // it is possible that not all such expressions are in use, and we run out of RAM memory.
  // If that is to happen, some sort of garbage collection will have to be implemented.
  // However, for the current calculator purposes, no such danger exists.
  public:
  Calculator* owner;
  int theData;
  HashedList<int, MathRoutines::IntUnsignIdentity> children;
  bool flagDeallocated;
//////
  typedef bool (*FunctionAddress) (Calculator& theCommands, const Expression& input, Expression& output);
//////
  void operator=(const Expression& other) {
    this->theData = other.theData;
    this->children = other.children;
    this->owner = other.owner;
  }
  void operator=(const Rational& other) {
    MacroRegisterFunctionWithName("Expression::operator=(Rational)");
    this->CheckInitialization();
    this->AssignValue(other, *this->owner);
  }
  void operator=(int other) {
    MacroRegisterFunctionWithName("Expression::operator=(int)");
    this->CheckInitialization();
    this->AssignValue(other, *this->owner);
  }
  friend std::ostream& operator << (std::ostream& output, const Expression& theMon) {
    output << theMon.ToString();
    return output;
  }
  friend Expression operator*(const Expression& left, const Expression& right);
  friend Expression operator/(const Expression& left, const Expression& right);
  friend Expression operator+(const Expression& left, const Expression& right);
  friend Expression operator-(const Expression& left, const Expression& right);

  void reset(Calculator& newBoss, int numExpectedChildren = 0) {
    this->owner = &newBoss;
    this->theData = 0;
    this->children.Clear();
    this->children.SetExpectedSize(numExpectedChildren);
  }
  bool AddChildRationalOnTop(const Rational& inputRat);
  bool AddChildOnTop(const Expression& inputChild);
  bool AddChildAtomOnTop(const std::string& theOperationString);
  bool AddChildAtomOnTop(int theOp) {
    this->CheckInitialization();
    Expression tempE;
    tempE.MakeAtom(theOp, *this->owner);
    return this->AddChildOnTop(tempE);
  }
  void GetBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList) const;
  bool SplitProduct(int numDesiredMultiplicandsLeft, Expression& outputLeftMultiplicand, Expression& outputRightMultiplicand) const;
  void GetBaseExponentForm(Expression& outputBase, Expression& outputExponent) const;
  void GetMultiplicandsRecursive(List<Expression>& outputAppendList, int depth = 0) const;
  void GetMultiplicandsDivisorsRecursive(List<Expression>& outputAppendList, int depth = 0) const;
  void GetCoefficientMultiplicandForm(Expression& outputCoeff, Expression& outputNoCoeff) const;
  void GetCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff) const;
  bool SetChildAtomValue(int childIndex, const std::string& theAtom);
  bool SetChildAtomValue(int childIndex, int TheAtomValue);
  int size() const {
    return this->children.size;
  }
  bool SetChilD(int childIndexInMe, const Expression& inputChild);
  bool AssignMatrixExpressions(
    const Matrix<Expression>& input,
    Calculator& owner,
    bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
    bool dontReduceTypes
  );
  template<class coefficient>
  bool AssignMatrix(
    const Matrix<coefficient>& input,
    Calculator& owner,
    Expression const* inputContext = nullptr,
    bool reduceOneRowToSequenceAndOneByOneToNonMatrix = true
  );
  bool DivisionByMeShouldBeWrittenInExponentForm() const;
  bool IsCalculatorStatusChanger() const;
  bool AssignMeMyChild(int childIndex) {
    Expression tempExp = (*this)[childIndex];
    this->operator=(tempExp);
    return true;
  }
  bool IsLisT() const;
  bool IsListNElements(int N = - 1) const {
    if (!this->IsLisT()) {
      return false;
    }
    if (N == - 1) {
      return true;
    }
    return this->children.size == N;
  }
  bool StartsWithBuiltInAtom() const;
  bool StartsWithFunctionWithComplexRange() const;
  bool StartsWithArithmeticOperation() const;
  bool StartsWith(int theOp = - 1, int N = - 1) const;

  bool StartsWithGivenAtom(const std::string& theAtom, int desiredChildren = - 1) const;
  bool IsListStartingWithAtom(int theOp = - 1) const {
    if (!this->IsLisT()) {
      return false;
    }
    if (!(*this)[0].IsAtom()) {
      return false;
    }
    if (theOp == - 1) {
      return true;
    }
    return (*this)[0].theData == theOp;
  }
  bool IsListOfTwoAtomsStartingWith(int theOp) const {
    if (!this->IsListStartingWithAtom(theOp)) {
      return false;
    }
    if (this->children.size != 2) {
      return false;
    }
    return (*this)[1].IsAtom();
  }
  bool HasSameContextArgumentsNoLog() const;
  bool IsFrozen() const;
  bool IsAtomThatFreezesArguments(std::string* outputWhichAtom = nullptr) const;
  bool IsAtomWhoseExponentsAreInterpretedAsFunction(std::string* outputWhichAtom = nullptr) const;
  bool IsPowerOfAtomWhoseExponentsAreInterpretedAsFunction() const;
  bool IsAtomNotInterpretedAsFunction(std::string* outputWhichAtom = nullptr) const;
  bool IsMatrix(int* outputNumRows = nullptr, int* outputNumCols = nullptr) const;
  template <typename theType>
  bool IsMatrixGivenType(int* outputNumRows = nullptr, int* outputNumCols = nullptr, Matrix<theType>* outputMat = nullptr) const;
  bool IsAtom(std::string* outputWhichOperation = nullptr) const;
  bool IsAtomGivenData(const std::string& desiredAtom) const;
  bool IsAtomGivenData(int desiredDataUseMinusOneForAny = - 1) const;
  bool IsBuiltInAtom(std::string* outputWhichOperation = nullptr) const;
  bool IsGoodForChainRuleFunction(std::string* outputWhichOperation = nullptr) const;
  bool IsIntegraLfdx(
    Expression* differentialVariable = nullptr, Expression* functionToIntegrate = nullptr, Expression* integrationSet = nullptr
  ) const;
  bool IsIndefiniteIntegralfdx(
    Expression* differentialVariable = nullptr, Expression* functionToIntegrate = nullptr, Expression* integrationSet = nullptr
  ) const;
  bool IsDefiniteIntegralOverIntervalfdx(
    Expression* differentialVariable = nullptr, Expression* functionToIntegrate = nullptr, Expression* integrationSet = nullptr
  ) const;
  bool IsDifferentialOneFormOneVariablE(
    Expression* outputDifferentialOfWhat = nullptr, Expression* outputCoeffInFrontOfDifferential = nullptr
  ) const;
  bool IsKnownFunctionWithComplexRange(std::string* outputWhichOperation = nullptr) const;
  bool IsArithmeticOperation(std::string* outputWhichOperation = nullptr) const;
  bool IsCacheableExpression() const;
  bool IsBuiltInScalar() const;
  bool IsElementaryObject() const;
  bool IsBuiltInTypE(std::string* outputWhichOperation = nullptr) const;
  bool IsBuiltInTypE(int* outputWhichType) const;
  const Expression& operator[](int n) const;
  bool IsIntervalRealLine() const;
  bool IsSequenceDoubleButNotTripleNested() const;
  bool IsSequenceNElementS(int N = - 2) const;
  bool IsError(std::string* outputErrorMessage = nullptr) const;
  bool IsContext() const;
  bool NeedsParenthesisForBaseOfExponent() const;
  bool NeedsParenthesisForMultiplication() const;
  bool NeedsParenthesisForAddition() const;
  bool NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost(const Expression* leftNeighbor = nullptr) const;

  int GetExpressionTreeSize() const;
  template <class theType>
  bool ConvertToType(Expression& output) const;
  template <class theType>
  bool ConvertsToType(theType* whichElement = nullptr) const;
  template <class theType>
  bool IsOfType() const {
    MacroRegisterFunctionWithName("Expression::IsOfType");
    if (this->owner == nullptr) {
      return false;
    }
    if (!this->StartsWith(this->GetTypeOperation<theType>())) {
      return false;
    }
    if (this->children.size < 2 || !this->GetLastChild().IsAtom()) {
      return false;
    }
    return true;
  }
  template <class theType>
  bool HasType() const {
    if (this->IsOfType<theType>()) {
      return true;
    }
    if (this->IsElementaryObject()) {
      return false;
    }
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].HasType<theType>()) {
        return true;
      }
    }
    return false;
  }
  template <class theType>
  bool IsOfType(theType* whichElement) const {
    MacroRegisterFunctionWithName("Expression::IsOfType");
    if (this->owner == nullptr) {
      return false;
    }
    if (!this->StartsWith(this->GetTypeOperation<theType>())) {
      return false;
    }
    if (this->children.size < 2 || !this->GetLastChild().IsAtom()) {
      return false;
    }
    if (whichElement == 0) {
      return true;
    }
    *whichElement = this->GetValue<theType>();
    return true;
  }
  template <class theType>
  const theType& GetValue() const {
    return this->GetValueNonConst<theType>();
  }
  template <class theType>
  theType& GetValueNonConst() const;
  template<class theType>
  int GetTypeOperation() const;
  template<class theType>
  int AddObjectReturnIndex(const theType& inputValue) const;
  //note: the following always returns true:
  template <class theType>
  bool AssignValue(const theType& inputValue, Calculator& owner);
  //note: the following always returns true:
  template <class theType>
  bool AssignValueWithContext(const theType& inputValue, const Expression& theContext, Calculator& owner);
  template <class theType>
  bool AddChildValueOnTop(const theType& inputValue) {
    this->CheckInitialization();
    Expression tempE;
    tempE.AssignValue(inputValue, *this->owner);
    return this->AddChildOnTop(tempE);
  }
  template <class theType>
  bool AssignValueWithContextToChild(int childIndex, const theType& inputValue, const Expression& theContext, Calculator& owner) {
    Expression tempE;
    tempE.AssignValueWithContext(inputValue, theContext, owner);
    return this->SetChilD(childIndex, tempE);
  }
  bool SetContextAtLeastEqualTo(Expression& inputOutputMinContext);
  int GetNumContextVariables() const;
  bool RemoveContext();
  bool HasContext() const;
  bool HasNonEmptyContext() const;
  Expression GetContext() const;
  static bool MergeContexts(Expression& leftE, Expression& rightE);
  bool MergeContextsMyAruments(Expression& output, std::stringstream* commentsOnFailure) const;
  template <class theType>
  bool MergeContextsMyArumentsAndConvertThem(Expression& output, std::stringstream* commentsOnFailure) const;

  bool ContainsAsSubExpressionNoBuiltInTypes(const Expression& input) const;
  bool ContainsAsSubExpressionNoBuiltInTypes(int inputAtom) const;
  Expression ContextGetContextVariable(int variableIndex) const;
  int ContextGetIndexAmbientSSalg() const;
  void ContextGetFormatExpressions(FormatExpressions& output) const;
  int ContextGetNumContextVariables() const;
  static bool ContextMergeContexts(const Expression& leftContext, const Expression& rightContext, Expression& outputContext);
  template <class coefficient>
  bool ContextGetPolySubFromSuperContext(const Expression& largerContext, PolynomialSubstitution<coefficient>& output) const;
  template <class coefficient>
  bool ContextGetPolySubFromSuperContextNoFailure(const Expression& largerContext, PolynomialSubstitution<coefficient>& output) const;
  bool ContextGetPolyAndEWASubFromSuperContext(
    const Expression& largerContext,
    PolynomialSubstitution<Rational>& outputPolyPart,
    PolynomialSubstitution<Rational>& outputEWApart
  ) const;
  bool ContextGetPolyAndEWASubFromSuperContextNoFailure(
    const Expression& largerContext,
    PolynomialSubstitution<Rational>& outputPolyPart,
    PolynomialSubstitution<Rational>& outputEWApart
  ) const;
  Expression ContextGetPolynomialVariables() const;
  Expression ContextGetDifferentialOperatorVariables() const;
  Expression ContextGetContextType(int theType) const;
  bool ContextSetSSLieAlgebrA(int indexInOwners, Calculator& owner);
  bool ContextSetDiffOperatorVar(const Expression& thePolyVar, const Expression& theDiffOpVar);
  bool ContextMakeContextSSLieAlgebrA(int indexInOwners, Calculator& owner) {
    this->MakeEmptyContext(owner);
    return this->ContextSetSSLieAlgebrA(indexInOwners, owner);
  }
  SemisimpleLieAlgebra* GetAmbientSSAlgebraNonConstUseWithCaution() const;
  const SemisimpleLieAlgebra* GetAmbientSSAlgebra() const {
    return this->GetAmbientSSAlgebraNonConstUseWithCaution();
  }
  bool IsEqualToZero() const;
  bool IsMonEqualToZero() const;
  bool IsEqualToOne() const;
  bool IsEqualToTwo() const;
  bool IsEqualToHalf() const;
  bool IsEqualToMOne() const;
  bool IsKnownToBeNonNegative() const;
  bool IsNegativeConstant() const;
  bool MakeIdMatrixExpressions(int theDim, Calculator& inputBoss);
  void MakeMonomialGenVerma(const MonomialGeneralizedVerma<RationalFunctionOld>& inputMon, Calculator& newBoss);
  void MakeElementTensorsGeneralizedVermas(
    const ElementTensorsGeneralizedVermas<RationalFunctionOld>& inputMon,
    Calculator& newBoss
  );
  bool MakeAtom(int input, Calculator& newBoss) {
    this->reset(newBoss);
    this->theData = input;
    return true;
  }
  bool MakeAtom(const std::string& atomName, Calculator& newBoss);
  bool EvaluatesToVariableNonBound() const;
  Expression::FunctionAddress GetHandlerFunctionIamNonBoundVar();
  bool MakeIntegral(
    Calculator& theCommands,
    const Expression& integrationSet,
    const Expression& theFunction,
    const Expression& theVariable
  );
  template<class coefficient>
  bool MakeSum(Calculator& theCommands, const MonomialCollection<Expression, coefficient>& theSum);
  bool MakeSum(Calculator& theCommands, const List<Expression>& theSum);
  bool MakeProducT(Calculator& owner, const List<Expression>& theMultiplicands);
  bool MakeProducT(Calculator& owner, const Expression& left, const Expression& right);
  int GetNumCols() const;
  bool MakeSequenceCommands(Calculator& owner, List<std::string>& inputKeys, List<Expression>& inputValues);
  bool MakeSequenceStatements(Calculator& owner, List<Expression>* inputStatements = nullptr);
  bool MakeMatrix(Calculator& owner, Matrix<Expression>* inputMat = nullptr);
  bool MakeSequence(Calculator& owner, List<Expression>* inputSequence = nullptr);
  bool MakeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right);
  bool MakeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex = 2);
  bool MakeSqrt(Calculator& owner, const Expression& argument, const Rational& radicalSuperIndex = 2);

  bool MakeXOXOdotsOX(Calculator& owner, int theOp, const List<Expression>& theOpands);
  bool MakeOXdotsX(Calculator& owner, int theOp, const List<Expression>& input);
  bool MakeOX(Calculator& owner, int theOp, const Expression& opArgument);
  bool Sequencefy();
  bool IsSuitableForSubstitution() const;
  bool IsSuitableForRecursion() const;

  void SubstituteRecursively(const Expression& toBeSubbed, const Expression& toBeSubbedWith);
  void SubstituteRecursivelyInChildren(const Expression& toBeSubbed, const Expression& toBeSubbedWith);
  void AssignXOXToChild(
    int childIndex,
    Calculator& owner,
    int theOp,
    const Expression& left,
    const Expression& right
  ) {
    Expression tempE;
    tempE.MakeXOX(owner, theOp, left, right);
    this->SetChilD(childIndex, tempE);
  }
  bool ContextMakeContextWithPolyVars(Calculator& owner, const List<std::string>& inputPolyVarNames);
  bool ContextMakeContextWithPolyVars(Calculator& owner, const List<Expression>& inputPolyVarEs);
  bool ContextMakeContextWithOnePolyVar(Calculator& owner, const std::string& inputPolyVarName);
  bool ContextMakeContextWithOnePolyVar(Calculator& owner, const Expression& inputPolyVarE);
  bool MakeContextWithOnePolyVarOneDiffVar(
    Calculator& owner,
    const Expression& inputPolyVarE,
    const Expression& inputDiffVarE
  );
  bool MakeContextSSLieAlg(Calculator& owner, const SemisimpleLieAlgebra& theSSLiealg);
  bool MakeEmptyContext(Calculator& owner);
  std::string Lispify() const;
  bool ToStringData(std::string& output, FormatExpressions* theFormat = nullptr) const;
  std::string ToStringSemiFull() const;
  std::string ToStringFull() const;
  std::string ToStringAllSlidersInExpression() const;
  std::string ToUTF8String(FormatExpressions* theFormat = nullptr) const;
  bool GetListExpressionsFromExpressionHistoryRecursiveNested(Expression& outputAppend) const;
  std::string ToStringExpressionHistoryRecursiveNested();

  void ToStringOpTimes(std::stringstream& out, FormatExpressions* theFormat) const;
  void ToStringOpMultiplicative(std::stringstream& out, const std::string& operation, FormatExpressions* theFormat) const;

  std::string ToString(
    FormatExpressions* theFormat = nullptr,
    Expression* startingExpression = nullptr,
    bool unfoldCommandEnclosures = true,
    JSData* outputJS = nullptr
  ) const;
  JSData ToJSData(FormatExpressions* theFormat, const Expression& startingExpression) const;
  static unsigned int HashFunction(const Expression& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
    return this->HashFunctionRecursive(0, 1000);
  }
  unsigned int HashFunctionRecursive(int RecursionDepth, int MaxRecursionDepth) const {
    if (RecursionDepth > MaxRecursionDepth) {
      return 0;
    }
    unsigned int result = static_cast<unsigned>(this->theData) * SomeRandomPrimes[0];
    int numCycles = MathRoutines::Minimum(this->children.size, SomeRandomPrimesSize);
    for (int i = 0; i < numCycles; i ++) {
      result += (*this)[i].HashFunctionRecursive(RecursionDepth + 1, MaxRecursionDepth) * SomeRandomPrimes[i];
    }
    return result;
  }
  int GetIndexChild(const Expression& input) const {
    for (int i = 0; i < this->children.size; i ++) {
      if (input == (*this)[i]) {
        return i;
      }
    }
    return - 1;
  }
  Expression() :flagDeallocated(false) {
    this->reset();
  }
  Expression(int x) :flagDeallocated(false) {
    MacroRegisterFunctionWithName("Expression::Expression(int)");
    this->reset();
    this->theData = x;
  }
  const Expression& GetLastChild() const {
    return (*this)[this->children.size - 1];
  }
  bool MakeError (const std::string& theError, Calculator& owner, bool isPublicError = false);
  Expression(const Expression& other):flagDeallocated(false) {
    this->operator=(other);
  }
  Expression(Calculator& inputBoss):flagDeallocated(false) {
    this->reset(inputBoss);
  }
  ~Expression() {
    this->flagDeallocated = true;
  }
  bool CheckInitialization() const;
  bool CheckInitializationRecursively() const;
  bool CheckConsistency() const;
  bool CheckConsistencyRecursively() const;
  bool IsIntegerFittingInInt(int* whichInteger = nullptr) const;
  bool IsSmallInteger(int* whichInteger = nullptr) const;
  bool IsInteger(LargeInteger* whichInteger = nullptr) const;
  bool IsIntegerNonNegative(LargeIntegerUnsigned* whichInteger = nullptr) const;
  bool IsRational(Rational* whichRational = nullptr) const;
  bool IsAlgebraicRadical() const;
  bool IsConstantNumber() const;
  bool IsPositiveNumber() const;
  bool EvaluatesToDoubleInRange(
    const std::string& varName,
    double lowBound,
    double highBound,
    int numPoints,
    double* outputYmin = nullptr,
    double* outputYmax = nullptr,
    Vectors<double>* outputPoints = nullptr
  ) const;
  bool EvaluatesToDouble(double* whichDouble = nullptr) const;
  bool GetExpressionLeafs(HashedList<Expression>& outputAccumulateLeafs) const;
  bool GetFreeVariables(HashedList<Expression>& outputAccumulateFreeVariables, bool excludeNamedConstants) const;
  bool GetBoundVariables(HashedList<Expression>& outputAccumulateBoundVariables) const;
  bool EvaluatesToDoubleUnderSubstitutions(
    const HashedList<Expression>& knownEs, const List<double>& valuesKnownEs, double* whichDouble = nullptr
  ) const;
  bool HasBoundVariables() const;
  bool HasInputBoxVariables(HashedList<std::string, MathRoutines::HashString>* boxNames = nullptr) const;
  bool IsMeltable(int* numResultingChildren = nullptr) const;
  bool AreEqualExcludingChildren(const Expression& other) const {
    return this->owner == other.owner &&
    this->theData == other.theData &&
    this->children.size == other.children.size;
  }
  // The following function creates an expression by parsing a calculator-like string.
  // The purpose of this function is to reduce the number of lines needed to create an Expression using C++.
  // Consider creating the expression f{}{{a}}={{a}}+ 1; f{}b;
  // We would need to create a large expression tree, so many calls of Expression::MakeXOX.
  // Instead, we can simply parse the expression from a string.
  // The inputExpressions give us the ability to specify substitutions
  bool AssignStringParsed(
    const std::string& theString,
    MapList<std::string, Expression, MathRoutines::HashString>* substitutions,
    Calculator& owner
  );
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
//  Rational GetConstantTerm() const;
  bool IsEqualToMathematically(const Expression& other) const;
  bool operator==(int other) const;
  bool operator==(const Expression& other) const;
  bool operator==(const std::string& other) const;
  bool operator!=(const std::string& other) const {
    return !this->operator==(other);
  }
  bool operator!=(const Expression& other) const {
    return ! (*this == other);
  }
//  void operator=(const Expression& other);
  bool operator>(const Expression& other) const;
  bool GreaterThanNoCoeff(const Expression& other) const;
  void SubstituteRecursively(MapList<Expression, Expression>& theSubs);
  void SubstituteRecursivelyInChildren(MapList<Expression, Expression>& theSubs);
  class Test {
  public:
    static bool All();
    static bool ToStringTest(Calculator& ownerInitialized);
    static bool ToStringTestRecode(const std::string& inputHardCodedMustParse, Calculator& ownerInitialized);
  };
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
  int indexAmongOperationHandlers;
  int indexOperationParentThatBansHandler;

  bool flagIsCompositeHandler;
  bool flagIsInner;
  bool flagMayActOnBoundVars;
  bool flagIamVisible;
  bool flagIsExperimental;
  bool flagDisabledByUser;
  bool flagDisabledByUserDefaultValue;
  Expression::FunctionAddress theFunction;

  std::string ToStringShort() const;
  std::string ToStringSummary() const;
  std::string ToStringFull() const;
  JSData ToJSON() const;
  bool ShouldBeApplied(int parentOpIfAvailable);
  bool operator==(const Function& other) const {
    return this->theArgumentTypes == other.theArgumentTypes &&
    this->theFunction == other.theFunction &&
    this->flagIsInner == other.flagIsInner;
  }
  void reset(Calculator& owner) {
    this->theArgumentTypes.reset(owner);
    this->theFunction = nullptr;
    this->flagIsInner = true;
  }
  bool inputFitsMyInnerType(const Expression& input);
  Function() {
    this->theFunction = nullptr;
    this->indexAmongOperationHandlers = - 1;
    this->indexOperation = - 1;
    this->indexOperationParentThatBansHandler = - 1;
    this->owner = nullptr;
    this->flagIsCompositeHandler = false;
    this->flagDisabledByUserDefaultValue = false;
    this->flagDisabledByUser = false;
  }
  Function(
    Calculator& inputOwner,
    int inputIndexOperation,
    int inputIndexAmongOperationHandlers,
    const Expression::FunctionAddress& functionPointer,
    Expression* inputArgTypes,
    const std::string& description,
    const std::string& inputExample,
    bool inputflagIsInner,
    bool inputIsVisible = true,
    bool inputIsExperimental = false,
    bool inputflagMayActOnBoundVars = false,
    bool inputDisabledByUser = false,
    int inputIndexParentThatBansHandler = - 1
  ) {
    this->owner = &inputOwner;
    this->indexOperation = inputIndexOperation;
    this->indexAmongOperationHandlers = inputIndexAmongOperationHandlers;
    this->theFunction = functionPointer;
    this->theDescription = description;
    this->theExample = inputExample;
    if (inputArgTypes != nullptr) {
      this->theArgumentTypes = *inputArgTypes;
    }
    this->flagIsInner = inputflagIsInner;
    this->flagIamVisible = inputIsVisible;
    this->flagMayActOnBoundVars = inputflagMayActOnBoundVars;
    this->flagIsExperimental = inputIsExperimental;
    this->flagIsCompositeHandler = false;
    this->flagDisabledByUserDefaultValue = inputDisabledByUser;
    this->flagDisabledByUser = inputDisabledByUser;
    this->indexOperationParentThatBansHandler = inputIndexParentThatBansHandler;
  }
  static unsigned int HashFunction(const Function& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this->theFunction));
  }
};

class SyntacticElement {
  public:
  int controlIndex;
  int numNonBoundVariablesInherited;
  int numBoundVariablesInherited;
  Expression theData;
  List<Expression> dataList;
  std::string errorString;
  std::string ToStringHumanReadable(Calculator& theBoss, bool includeLispifiedExpressions) const;
  SyntacticElement() {
    this->controlIndex = 0; //controlIndex = 0 *MUST* point to the empty control sequence.
    this->errorString = "";
    this->numNonBoundVariablesInherited = - 1; // - 1 stands for unknown
    this->numBoundVariablesInherited = - 1; // - 1 stands for unknown
  }
  std::string GetIntegerStringCrashIfNot(Calculator& owner);
};

class PlotObject {
public:
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
  std::string thePlotType;

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
  std::string GetJavascriptSurfaceImmersion(
    std::string& outputSurfaceInstantiationJS,
    const std::string& canvasName,
    int& funCounter
  );
  std::string GetJavascriptCurveImmersionIn3d(
    std::string& outputCurveInstantiationJS, const std::string& canvasName, int& funCounter
  );
  std::string ToStringDebug();
  void ComputeYbounds();
  std::string ToStringPointsList();
  std::string GetPlotStringFromFunctionStringAndRanges(
    bool useHtml,
    const std::string& functionStringPostfixNotation,
    const std::string& functionStringCalculatorFormat,
    double lowerBound,
    double upperBound
  );
  std::string GetJavascript2dPlot(
    std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
  );
  std::string GetJavascriptParametricCurve2D(
    std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
  );
  std::string GetJavascriptPoints(
    std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
  );
  std::string GetJavascriptDirectionField(
    std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
  );
  PlotObject();
  bool operator==(const PlotObject& other) const;
};

//the following class is meant to use to draw plots for calculus students.
class Plot {
private:
  std::string canvasNamE;
public:
  List<PlotObject> thePlots;
  HashedList<std::string, MathRoutines::HashString> boxesThatUpdateMe;
  double theLowerBoundAxes;
  double theUpperBoundAxes;
  double lowBoundY;
  double highBoundY;
  int DesiredHtmlHeightInPixels;
  int DesiredHtmlWidthInPixels;
  int defaultLineColor;
  bool flagIncludeExtraHtmlDescriptions;
  bool flagPlotShowJavascriptOnly;
  bool flagDivAlreadyDisplayed;
  bool flagIncludeCoordinateSystem;
  int dimension;
  static int canvasCounteR;
  int priorityViewRectangle; // 0 or less: compute the view Window. If this quantity is greater than zero,
  int priorityWindow; // 0 or less: compute the view Window. If this quantity is greater than zero,
  // the user-given bounding box will overwrite any computations.
  // When adding two plots with positive viewing window priorities, the window with the larger priority is used.
  // If the priorities are equal, the windows are combined to the smallest window that fits both.
  int priorityCanvasName; // same as priorityViewWindow but with respect to canvas names.
  void SetCanvasName(const std::string& inputName);
  std::string GetCanvasName() const;

  std::string ToStringDebug();
  std::string GetPlotHtml(Calculator& owner);
  void ComputeCanvasNameIfNecessary();
  std::string commonCanvasSetup();
  std::string GetPlotHtml3d_New(Calculator& owner);
  std::string GetPlotHtml2d_New(Calculator& owner);
  std::string GetPlotStringAddLatexCommands(bool useHtml);
  bool IsOKVector(const Vector<double>& input);
  Plot();
  void ComputeAxesAndBoundingBox3d();
  void ComputeAxesAndBoundingBox();
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
  std::string GetSliderName() const;
  std::string GetUserInputBox() const;
};

class ObjectContainer {
  // Following are containers for data structures that are implemented in C++.
  // These objects are dynamically allocated and used by the calculator as requested
  // by various predefined function handlers.
public:
  HashedListReferences<ElementWeylGroup> theWeylGroupElements;
///////////////////////
  MapReferenceS<DynkinType, SemisimpleLieAlgebra> theSSLieAlgebras;
  MapReferenceS<DynkinType, SemisimpleSubalgebras> theSSSubalgebraS;
////////////////////////
  HashedListReferences<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > theWeylGroupReps;
  HashedListReferences<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > theWeylGroupVirtualReps;
  ListReferences<ModuleSSalgebra<RationalFunctionOld> > theCategoryOmodules;
  ListReferences<SltwoSubalgebras> theSltwoSAs;
  HashedListReferences<ElementEllipticCurve<ElementZmodP> > EllipticCurveElementsZmodP;
  HashedListReferences<ElementEllipticCurve<Rational> > EllipticCurveElementsRational;
  HashedListReferences<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theTensorElts;
  HashedListReferences<Polynomial<Rational> > thePolys;
  HashedListReferences<Polynomial<AlgebraicNumber> > thePolysOverANs;
  HashedListReferences<ElementWeylAlgebra<Rational> > theWeylAlgebraElements;
  HashedListReferences<ElementUniversalEnveloping<RationalFunctionOld> > theUEs;
  HashedListReferences<RationalFunctionOld> theRFs;
  HashedListReferences<Rational> theRationals;
  HashedListReferences<charSSAlgMod<Rational> > theCharsSSLieAlgFD;
  HashedListReferences<double, MathRoutines::HashDouble> theDoubles;
  HashedListReferences<std::string, MathRoutines::HashString> theStrings;
  HashedListReferences<std::string, MathRoutines::HashString> ExpressionNotation;
  HashedListReferences<Expression> ExpressionWithNotation;
  HashedListReferences<LittelmannPath> theLSpaths;
  HashedListReferences<MatrixTensor<Rational> > theMatTensorRats;
  //ListReferences<Matrix<double> > theMatDoubles;
  //HashedListReferences<Matrix<Rational> > theMatRats;
  //HashedListReferences<Matrix<AlgebraicNumber> > theMatsAlgebraic;
  //HashedListReferences<Matrix<Polynomial<Rational> > > theMatPolyRational;
  //HashedListReferences<Matrix<RationalFunctionOld> > theMatRFs;
  HashedListReferences<ElementZmodP> theEltsModP;
  HashedListReferences<Weight<Rational> > theWeights;
  HashedListReferences<Weight<Polynomial<Rational> > > theWeightsPoly;
  ListReferences<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational > > theHyperoctahedralReps;
  ListReferences<Plot> thePlots;
  List<bool> userInputBoxSliderDisplayed;
  MapReferenceS<std::string, InputBox, MathRoutines::HashString> theUserInputTextBoxesWithValues;
  MapReferenceS<std::string, std::string, MathRoutines::HashString> graphicsScripts;

  AlgebraicClosureRationals theAlgebraicClosure;
  HashedList<AlgebraicNumber> theAlgebraicNumbers;
  HashedListReferences<ElementHyperoctahedralGroupR2> theElementsHyperOctGroup;
  ListReferences<HyperoctahedralGroupData> theHyperOctahedralGroups;
//  HashedList<DifferentialForm<Rational> > theDiffForm;
  HashedListReferences<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
  WeylGroupData& GetWeylGroupDataCreateIfNotPresent(const DynkinType& input);
  SemisimpleLieAlgebra& GetLieAlgebraCreateIfNotPresent(const DynkinType& input);
  SemisimpleSubalgebras& GetSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input);
  int CurrentRandomSeed;
  void reset();
  void resetSliders();
  void resetPlots();
  std::string ToString();
  std::string ToStringJavascriptForUserInputBoxes();
};

class ExpressionTripleCrunchers {
public:
  int theOp;
  int leftType;
  int rightType;
  bool operator==(const ExpressionTripleCrunchers& other) const {
    return this->leftType == other.leftType && this->rightType == other.rightType && this->theOp == other.theOp;
  }
  void operator=(const ExpressionTripleCrunchers& other) {
    this->leftType = other.leftType;
    this->rightType = other.rightType;
    this->theOp = other.theOp;
  }
  ExpressionTripleCrunchers(): theOp(- 1), leftType(- 1), rightType(- 1) {
  }
  ExpressionTripleCrunchers(
    int inputOp, int inputLeft, int inputRight
  ): theOp(inputOp), leftType(inputLeft), rightType(inputRight) {

  }
  static unsigned int HashFunction(const ExpressionTripleCrunchers& input) {
    return
      static_cast<unsigned int>(input.leftType) * SomeRandomPrimes[0] +
      static_cast<unsigned int>(input.rightType) * SomeRandomPrimes[1] +
      static_cast<unsigned int>(input.theOp) * SomeRandomPrimes[2];
  }
};

class StateMaintainerCalculator {
public:
  Calculator* owner;
  int startingRuleStackIndex;
  int startingRuleStackSize;
  int historyOuterSize; //<-outermost layer of expression history
  int historyMiddleSize; //<- inner layer of expression history
  //int historyInnerSize; //<- inner-most layer of expression history
  std::string& GetCurrentHistoryRuleNames();
  Expression& GetCurrentHistory();
  StateMaintainerCalculator(Calculator& inputBoss);
  void AddRule(const Expression& theRule);
  ~StateMaintainerCalculator();
};

class Calculator {
  template<typename anyType>
  friend Calculator& operator << (Calculator& output, const anyType& any) {
    output.Comments << any;
    return output;
  }
private:
  Expression frequentConstantOne, frequentConstantTwo,
  frequentConstantThree, frequentConstantFour,
  frequentConstantFive, frequentConstantHalf,
  frequentConstantMinusHalf,
  frequentConstantZero,
  frequentConstantMinusOne,
  frequentEInfinity,
  frequentEMInfinity;

public:
  //Operations parametrize the expression elements.
  //Operations are the labels of the atom nodes of the expression tree.
  HashedList<std::string, MathRoutines::HashString> theAtoms;
  HashedList<std::string, MathRoutines::HashString> atomsThatAllowCommutingOfCompositesStartingWithThem;
  HashedList<std::string, MathRoutines::HashString> atomsNotAllowingChainRule;
  HashedList<std::string, MathRoutines::HashString> builtInTypes;
  HashedList<std::string, MathRoutines::HashString> arithmeticOperations;
  HashedList<std::string, MathRoutines::HashString> knownOperationsInterpretedAsFunctionsMultiplicatively;
  HashedList<std::string, MathRoutines::HashString> knownFunctionsWithComplexRange;
  HashedList<std::string, MathRoutines::HashString> atomsThatFreezeArguments;
  HashedList<std::string, MathRoutines::HashString> atomsWhoseExponentsAreInterpretedAsFunctions;
  HashedList<std::string, MathRoutines::HashString> atomsNotInterpretedAsFunctions;
  HashedList<std::string, MathRoutines::HashString> atomsThatMustNotBeCached;
  HashedList<std::string, MathRoutines::HashString> operationsComposite;
  HashedList<std::string, MathRoutines::HashString> autoCompleteKeyWords;

  HashedList<std::string, MathRoutines::HashString> stringsThatSplitIfFollowedByDigit;

  MapList<std::string, List<std::string>, MathRoutines::HashString> predefinedWordSplits;
  List<List<Function> > FunctionHandlers;
  List<List<Function> > operationsCompositeHandlers;
  HashedList<std::string, MathRoutines::HashString> namedRules;
  List<List<int> > namedRulesLocations;//for each named rule we store a list of triple of ints.
  // If first int is 0 then the named rule is a function handler.
  // If first int is 1 then the named rule is a composite operation handler.
  // Second int gives the index of the atom handled by the named rule.
  // Third int gives the index of the rule within the list of handlers for that atom.

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
  HashedList<std::string, MathRoutines::HashString> controlSequences;

  HashedList<ExpressionTripleCrunchers> theCruncherIds;
  List<Function> theCruncherS;

  HashedList<Expression> knownDoubleConstants;
  List<double> knownDoubleConstantValues;

  ListReferences<ListReferences<Expression> > historyStack;
  ListReferences<ListReferences<std::string> > historyRuleNames;
  List<Expression> buffer1, buffer2;
  int MaxRecursionDeptH;
  int RecursionDeptH;
  int DepthRecursionReached;
  int MaxAlgTransformationsPerExpression;
  int MaxLatexChars;
  int MaxCachedExpressionPerRuleStack;
  int MaxRuleStacksCached;
  int NumErrors;
  int RuleStackCacheIndex;
  class EvaluationStats {
  public:
    int expressionEvaluated;
    int callsSinceReport;
    int maximumCallsBeforeReportGeneration;
    EvaluationStats();
  };
  EvaluationStats stats;
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
  bool flagDontUseDiff;
  bool flagUseLnAbsInsteadOfLogForIntegrationNotation;
  bool flagLogEvaluatioN;
  bool flagUseNumberColors;
  bool flagLogRules;
  bool flagUseBracketsForIntervals;
  bool flagLogCache;
  bool flagLogPatternMatching;
  bool flagLogFullTreeCrunching;
  bool flagHideLHS;
  bool flagHidePolynomialBuiltInTypeIndicator;
  bool flagDisplayFullExpressionTree;
  bool flagUseFracInRationalLaTeX;
  bool flagUseHtml;
  bool flagDisplayContext;
  bool flagShowCalculatorExamples;
  bool flagDefaultRulesWereTamperedWith;
  bool flagHasGraphics;
  bool flagWriteLatexPlots;

  bool flagNoApproximationS;

  //  bool flagReplaceInputBoxesByValues;

  bool flagForkingProcessAllowed;

  double LastLogEvaluationTime;
  ///////////////////////////////////////////////////////////////////////////
  int NumListsStart;
  int NumListResizesStart;
  int NumHashResizesStart;
  long long int NumSmallAdditionsStart;
  long long int NumSmallMultiplicationsStart;
  long long int NumSmallGCDcallsStart;
  long long int NumLargeAdditionsStart;
  long long int NumLargeMultiplicationsStart;
  long long int NumLargeGCDcallsStart;
  ////////////////////////////////////////////////
  int TotalNumPatternMatchedPerformed;
  int NumPredefinedAtoms;
  int numEmptyTokensStart;
  Expression theProgramExpression;
  //  std::vector<std::stringstream> theLogs;
  int counterInSyntacticSoup;
  List<SyntacticElement> syntacticSouP;
  List<SyntacticElement> syntacticStacK;

  List<SyntacticElement>* CurrrentSyntacticSouP;
  List<SyntacticElement>* CurrentSyntacticStacK;

  HashedList<Expression> cachedExpressions;
  List<Expression> imagesCachedExpressions;
  ////
  HashedList<Expression> EvaluatedExpressionsStack;
  HashedList<int, MathRoutines::IntUnsignIdentity> NonBoundVariablesInContext;
  HashedList<int, MathRoutines::IntUnsignIdentity> BoundVariablesInContext;

  Expression RuleStack;
  HashedList<Expression> cachedRuleStacks;

  HashedListReferences<Expression> theExpressionContainer;

  std::string syntaxErrors;
  List<std::string> evaluationErrors;

  //std::string inputStringRawestOfTheRaw;
  std::string inputString;
  std::string outputString;
  JSData outputJS;

  std::string outputCommentsString;
  std::string parsingLog;
  ObjectContainer theObjectContainer;
  int64_t startTimeEvaluationMilliseconds;

  char DefaultWeylLetter;
  int DefaultWeylRank;
  std::string javaScriptDisplayingIndicator;
  std::fstream theTestFile;
  std::string theTestFileName;
  int numOutputFileS;
  std::string userLabel;
  //List<std::string> logEvaluationSteps;
  std::stringstream Comments;
  std::stringstream errorsPublic;
  FormatExpressions formatVisibleStrings;
  std::string ToStringRuleStatusUser();
  std::string ToString();
  std::string ToStringPerformance();
  Expression GetNewBoundVar();
  Expression GetNewAtom();
  void ComputeAutoCompleteKeyWords();
  void WriteAutoCompleteKeyWordsToFile();
  std::string ElementToStringNonBoundVars();
  JSData ToJSONOutputAndSpecials();
  std::string ToStringOutputAndSpecials();
  JSData FunctionHandlersJSON();
  //the purpose of the operator below is to save on typing when returning false with a comment.
  operator bool() const {
    return false;
  }

  void ExpressionHistoryPop();
  void ExpressionHistoryAddEmptyHistory();
  void ExpressionHistoryAdd(Expression& theExpression, int expressionLabel);
  void ExpressionHistoryStackAdd();
  void ExpressionHistoryStackPop();
  void RegisterCalculatorFunctionIdentifier(
    const Function& theFun, int indexOp, int functionType, int theFunIndex
  );
  std::string ToStringSemismipleLieAlgebraLinksFromHD(
    const DynkinType& theType, FormatExpressions* theFormat = nullptr
  );
  //void GetOutputFolders(const DynkinType& theType, std::string& outputFolderPhysical, std::string& outputFolderDisplay, FormatExpressions& outputFormat);
  bool IsBoundVarInContext(int inputOp);
  bool IsNonBoundVarInContext(int inputOp);
  Function& GetFunctionHandlerFromNamedRule(const std::string& inputRuleName);
  bool CheckPredefinedFunctions();
  bool CheckConsistencyAfterInitializationExpressionStackEmpty();
  //to make operations read only, we make operations private and return const pointer to it.
  const HashedList<std::string, MathRoutines::HashString>& GetOperations() {
    return this->theAtoms;
  }
  const HashedList<std::string, MathRoutines::HashString>& GetBuiltInTypes() {
    return this->builtInTypes;
  }
  const List<Function>* GetOperationCompositeHandlers(int theOp) {
    int theIndex = this->operationsComposite.GetIndex(this->GetOperations()[theOp]);
    if (theIndex == - 1) {
      return nullptr;
    }
    return &this->operationsCompositeHandlers[theIndex];
  }
  SyntacticElement GetSyntacticElementEnd() {
    SyntacticElement result;
    result.controlIndex = this->controlSequences.GetIndex(";");
    return result;
  }
  bool DecreaseStackSetCharacterRangeS(int decrease) {
    if (decrease <= 0) {
      return true;
    }
    if ((*this->CurrentSyntacticStacK).size - decrease <= 0) {
      crash << "Bad stack decrease. " << crash;
    }
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease- 1].IndexLastCharPlusOne =
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size- 1].IndexLastCharPlusOne;
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size - decrease);
    return true;
  }
  const Expression& EZero();
  const Expression& EOne();
  const Expression& ETwo();
  const Expression& EThree();
  const Expression& EFour();
  const Expression& EFive();
  const Expression& EMOne();
  const Expression& EHalf();
  const Expression& EMHalf();
  const Expression& EInfinity();
  const Expression& EMInfinity();
  void DoLogEvaluationIfNeedBe(Function& inputF);
  void LogPublicError(const std::string& theError);
  bool DecreaseStackExceptLast(int decrease);
  bool DecreaseStackExceptLastTwo(int decrease);
  std::string ToStringSyntacticStackHTMLTable(bool ignoreCommandEnclosures);
  std::string ToStringSyntacticStackHumanReadable(
    bool includeLispifiedExpressions, bool ignoreCommandEnclosures
  );
  std::string WriteDefaultLatexFileReturnHtmlLink(
    const std::string& fileContent, std::string* outputFileNameNoExtension, bool useLatexDviPSpsToPNG = false
  );
  std::string ToStringIsCorrectAsciiCalculatorString(const std::string& input);
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
  //  Expression::FunctionAddress GetInnerFunctionFromOp(int theOp, const Expression& left, const Expression& right);
  bool AllowsPowerInPreceding(const std::string& lookAhead);
  bool AllowsPowerInNext(const std::string& lookBehind);
  bool RecursionDepthExceededHandleRoughly(const std::string& additionalErrorInfo = "");

  bool AllowsLimitProcessInPreceding(const std::string& lookAhead);
  bool AllowsApplyFunctionInPreceding(const std::string& lookAhead);
  bool AllowsIfInPreceding(const std::string& lookAhead);
  bool AllowsOrInPreceding(const std::string& lookAhead);
  bool AllowsAndInPreceding(const std::string& lookAhead);
  bool AllowsInInPreceding(const std::string& lookAhead);
  bool AllowsPlusInPreceding(const std::string& lookAhead);
  bool AllowsTimesInNext(const std::string& thePreceding);
  bool AllowsTimesInPreceding(const SyntacticElement& thePreceding, const std::string& lookAhead);

  bool AllowsTimesInPreceding(const std::string& lookAhead);
  bool AllowsTensorInPreceding(const std::string& lookAhead);
  bool AllowsDivideInPreceding(const std::string& lookAhead);
  bool PopTopSyntacticStack() {
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size - 1);
    return true;
  }
  bool ReplaceEXXEXEByEusingO(int theOp);
  bool ReplaceEXXEXEXByEXusingO(int theOp);
  bool ReplaceEOXbyEX();
  bool ReplaceEEByEusingO(int theControlIndex);
  bool ReplaceEEXByEXusingO(int theControlIndex);
  bool ReplaceOOEEXbyEXpowerLike();
  bool ReplaceEEByE();
  bool ReplaceEXEXByEX();
  bool ReplaceEXEXByEXUsingO(int theOp);
  bool ReplaceSsSsXdotsXbySsXdotsX(int numDots);
  bool ReplaceEXdotsXbySsXdotsX(int numDots);
  bool ReplaceEXdotsXBySs(int numDots) {
    this->ReplaceEXdotsXbySsXdotsX(numDots);
    return this->DecreaseStackSetCharacterRangeS(numDots);
  }
  bool ReplaceOEXByE();
  bool ReplaceOEXByEX();
  bool ReplaceO_2O_1E_3XbyEX();
  bool ReplaceEOByE();
  bool ReplaceOEByE();
  bool ReplaceOXEByE      ();
  bool ReplaceOXEXByEX    ();
  bool ReplaceOXEEXByEX   ();
  bool ReplaceOXXEXEXEXByE();
  bool ReplaceSqrtEXXByEXX();
  bool ReplaceSqrtEXByEX  ();
  bool ReplaceSqrtXEXByEX ();
  bool ReplaceOXEXEByE    ();
  bool ReplaceOXEXEXByEX  ();
  bool ReplaceOXEXEXXByEXX();
  bool ReplaceOXEXEXEXByE ();
  bool ReplaceEOEXByEX    ();
  bool ReplaceUnderscoreEPowerEbyLimits();
  bool ReplacePowerEUnderScoreEbyLimits();
  bool ReplaceXEEXByEXusingO(int inputOperation);
  bool ReplaceXEEByEusingO(int inputOperation);
  bool ReplaceECByC();
  bool ReplaceEXEBySequence(int theControlIndex);
  bool ReplaceYXBySequenceX(int theControlIndex);
  bool ReplaceXXXbyE();
  bool ReplaceYBySequenceY(int theControlIndex);
  bool ReplaceXXYBySequenceY(int theControlIndex);
  bool ReplaceXXYXBySequenceYX(int theControlIndex);
  bool ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int numXs = 0);
  bool ReplaceSequenceXEBySequence    (int theControlIndex);
  bool ReplaceSequenceUXEYBySequenceZY(int theControlIndex);
  bool ReplaceCEByC();
  bool ReplaceCCByC();
  bool ReplaceEOEByE() {
    return this->ReplaceEXEByEusingO((*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2].controlIndex);
  }
  bool ReplaceMatrixEXByMatrix();
  bool ReplaceMatrixEXByMatrixX();
  bool ReplaceMatrixEXByMatrixNewRow();
  bool ReplaceMatrixXByE();
  bool ReplaceCXByE();
  bool ReplaceCXByEX();
  bool ReplaceXEXEXByEusingO(int theOperation);
  bool ReplaceEXEByEusingO  (int theOperation);
  bool ReplaceXYByConY(int theCon) {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2].controlIndex = theCon;
    return true;
  }
  bool ReplaceXYYByConYY(int theCon) {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3].controlIndex = theCon;
    return true;
  }
  bool ReplaceXYYYByConYYY(int theCon) {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4].controlIndex = theCon;
    return true;
  }
  bool ReplaceXXByCon(int theCon) {
    this->ReplaceXYByConY(theCon);
    return this->DecreaseStackSetCharacterRangeS(1);
  }
  bool ReplaceXByCon(int theCon);
  bool ReplaceXByEusingO(int theOperation);
  bool ReplaceXByConCon(int con1, int con2);
  bool ReplaceXXXByCon(int theCon);
  bool ReplaceXXXByConCon(int con1, int con2);
  bool ReplaceXXXXXByCon(int theCon);
  bool ReplaceXXXXXByConCon(int con1, int con2);
  bool ReplaceXXXXByConCon (int con1, int con2);
  bool ReplaceXdotsXByMatrixStart(int numXes);
  bool ReplaceXXXXByCon(int con1);
  bool ReplaceXXYByY() {
    (*this->CurrentSyntacticStacK)[this->CurrentSyntacticStacK->size - 3] =
    (*this->CurrentSyntacticStacK)[this->CurrentSyntacticStacK->size - 1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size - 2);
    return true;
  }
  bool ReplaceXXYXByYX() {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4] =
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3] =
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size - 2);
    return true;
  }
  bool ReplaceXYYXByYY() {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4] =
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3] =
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size - 2);
    return true;
  }
  bool ReplaceXYXByY() {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3] =
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
    this->DecreaseStackSetCharacterRangeS(2);
    return true;
  }
  bool ReplaceXYByY() {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2] =
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size - 1);
    return true;
  }
  bool ReplaceXEXByE();
  bool ReplaceVbyVdotsVAccordingToPredefinedWordSplits();
  bool ReplaceAXbyEX();
  bool ReplaceIntegerXbyEX();
  bool ReplaceIntegerDotIntegerByE();
  bool ReplaceXXByEEmptySequence();
  bool ReplaCeOXdotsXbyEXdotsX(int numXs);
  bool ReplaCeOXbyEX();
  bool ReplaCeObyE();
  bool ReplaceXXbyEX();
  bool ReplaceXEXByEcontainingOE(int inputOpIndex);
  bool ReplaceXXByEmptyString();
  bool ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  bool ReplaceXXbyE();
  bool GetMatrixExpressions(const Expression& input, Matrix<Expression>& output, int desiredNumRows = - 1, int desiredNumCols = - 1);
  bool GetMatrixExpressionsFromArguments(
    const Expression& input, Matrix<Expression>& output, int desiredNumRows = - 1, int desiredNumCols = - 1
  );
  void MakeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output);
  bool ReplaceXXVXdotsXbyE_BOUND_XdotsX(int numXs);
  bool ReplaceVXdotsXbyE_NONBOUND_XdotsX(int numXs);
  int GetOperationIndexFromControlIndex(int controlIndex);
  int GetExpressionIndex();
  SyntacticElement GetEmptySyntacticElement();
  bool AccountRule(const Expression& ruleE, StateMaintainerCalculator& theRuleStackMaintainer);
  bool ApplyOneRule();
  void resetStack() {
    SyntacticElement emptyElement = this->GetEmptySyntacticElement();
    (*this->CurrentSyntacticStacK).initializeFillInObject(this->numEmptyTokensStart, emptyElement);
  }
  int conError() {
    return this->controlSequences.GetIndexIMustContainTheObject("Error");
  }
  int conExpression() {
    return this->controlSequences.GetIndexIMustContainTheObject("Expression");
  }
  int conVariable() {
    return this->controlSequences.GetIndexIMustContainTheObject("Variable");
  }
  int conBindVariable() {
    return this->controlSequences.GetIndexIMustContainTheObject("{{}}");
  }
  int conInteger() {
    return this->controlSequences.GetIndexIMustContainTheObject("Integer");
  }
  int conEqualEqual() {
    return this->controlSequences.GetIndexIMustContainTheObject("==");
  }
  int conEqualEqualEqual() {
    return this->controlSequences.GetIndexIMustContainTheObject("===");
  }
  int conApplyFunction() {
    return this->controlSequences.GetIndexIMustContainTheObject("{}");
  }
  int conTimes() {
    return this->controlSequences.GetIndexIMustContainTheObject("*");
  }
  int conEndStatement() {
    return this->controlSequences.GetIndexIMustContainTheObject(";");
  }
  int conDefineConditional() {
    return this->controlSequences.GetIndexIMustContainTheObject("if=");
  }
  int conGEQ() {
    return this->controlSequences.GetIndexIMustContainTheObject("\\geq");
  }
  int conComma() {
    return this->controlSequences.GetIndexIMustContainTheObject(",");
  }
  int conDefine() {
    return this->controlSequences.GetIndexIMustContainTheObject("=");
  }
  int conIsDenotedBy() {
    return this->controlSequences.GetIndexIMustContainTheObject("=:");
  }
  int conLisT() {
    return this->controlSequences.GetIndexIMustContainTheObject("");
  }
  int conSequenceStatements() {
    return this->controlSequences.GetIndexIMustContainTheObject("SequenceStatements");
  }
  int conSequence() {
    return this->controlSequences.GetIndexIMustContainTheObject("Sequence");
  }
  int conSequenceNoRepetition() {
    return this->controlSequences.GetIndexIMustContainTheObject("SequenceNoRepetition");
  }
  int conMatrixStart() {
    return this->controlSequences.GetIndexIMustContainTheObject("Matrix");
  }
  int conMatrixEnd() {
    return this->controlSequences.GetIndexIMustContainTheObject("MatrixEnd");
  }
  int conLieBracket() {
    return this->controlSequences.GetIndexIMustContainTheObject("[]");
  }
  int conOpenBracket() {
    return this->controlSequences.GetIndexIMustContainTheObject("(");
  }
  int conCloseBracket() {
    return this->controlSequences.GetIndexIMustContainTheObject(")");
  }
  int conEndProgram() {
    return this->controlSequences.GetIndexIMustContainTheObject("EndProgram");
  }
  int opEltZmodP() {
    return this->theAtoms.GetIndexIMustContainTheObject("EltZmodP");
  }
  //int opApplyFunction()
  //{ return this->theAtoms.GetIndexIMustContainTheObject("{}");
  //}
  int opIsDenotedBy() {
    return this->theAtoms.GetIndexIMustContainTheObject("=:");
  }
  int opDefine() {
    return this->theAtoms.GetIndexIMustContainTheObject("=");
  }
  int opCoefficientOf() {
    return this->theAtoms.GetIndexIMustContainTheObject("CoefficientOf");
  }
  int opCommandEnclosure() {
    return this->theAtoms.GetIndexIMustContainTheObject("CommandEnclosure");
  }
  int opRulesOff() {
    return this->theAtoms.GetIndexIMustContainTheObject("RulesOff");
  }
  int opRulesOn() {
    return this->theAtoms.GetIndexIMustContainTheObject("RulesOn");
  }
  int opCommandEnclosureStart() {
    return this->theAtoms.GetIndexIMustContainTheObject("CommandEnclosureStart");
  }
  int opCommandEnclosureFinish() {
    return this->theAtoms.GetIndexIMustContainTheObject("CommandEnclosureFinish");
  }
  int opTurnOnRules() {
    return this->theAtoms.GetIndexIMustContainTheObject("TurnOnRules");
  }
  int opTurnOffRules() {
    return this->theAtoms.GetIndexIMustContainTheObject("TurnOffRules");
  }
  int opSqrt() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\sqrt");
  }
  int opIf() {
    return this->theAtoms.GetIndexIMustContainTheObject("if");
  }
  int opDefineConditional() {
    return this->theAtoms.GetIndexIMustContainTheObject("if=");
  }
  int opThePower() {
    return this->theAtoms.GetIndexIMustContainTheObject("^");
  }
  int opUnderscore() {
    return this->theAtoms.GetIndexIMustContainTheObject("_");
  }
  int opEqualEqual() {
    return this->theAtoms.GetIndexIMustContainTheObject("==");
  }
  int opEqualEqualEqual() {
    return this->theAtoms.GetIndexIMustContainTheObject("===");
  }
  int opGreaterThanOrEqualTo() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\geq");
  }
  int opLessThanOrEqualTo() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\leq");
  }
  int opGreaterThan() {
    return this->theAtoms.GetIndexIMustContainTheObject(">");
  }
  int opLessThan() {
    return this->theAtoms.GetIndexIMustContainTheObject("<");
  }
  int opWeightLieAlg() {
    return this->theAtoms.GetIndexIMustContainTheObject("weightLieAlg");
  }
  int opWeightLieAlgPoly() {
    return this->theAtoms.GetIndexIMustContainTheObject("weightLieAlgPoly");
  }
  int opError() {
    return this->theAtoms.GetIndexIMustContainTheObject("Error");
  }
  int opLisT() {
    return this->theAtoms.GetIndexIMustContainTheObject("");
  }
  int opMonomialCollection() {
    return this->theAtoms.GetIndexIMustContainTheObject("MonomialCollection");
  }
  int opMonomialPoly() {
    return this->theAtoms.GetIndexIMustContainTheObject("MonomialPoly");
  }
  int opCalculusPlot() {
    return this->theAtoms.GetIndexIMustContainTheObject("CalculusPlot");
  }
  int opMatriX() {
    return this->theAtoms.GetIndexIMustContainTheObject("Matrix");
  }
  int opSequence() {
    return this->theAtoms.GetIndexIMustContainTheObject("Sequence");
  }
  int opIntervalOpen() {
    return this->theAtoms.GetIndexIMustContainTheObject("IntervalOpen");
  }
  int opIntervalLeftClosed() {
    return this->theAtoms.GetIndexIMustContainTheObject("[)");
  }
  int opIntervalRightClosed() {
    return this->theAtoms.GetIndexIMustContainTheObject("(]");
  }
  int opIntervalClosed() {
    return this->theAtoms.GetIndexIMustContainTheObject("IntervalClosed");
  }
  int opExpressionHistory() {
    return this->theAtoms.GetIndexIMustContainTheObject("ExpressionHistory");
  }
  int opQuote() {
    return this->theAtoms.GetIndexIMustContainTheObject("\"");
  }
  int opMelt() {
    return this->theAtoms.GetIndexIMustContainTheObject("Melt");
  }
  int opRational() {
    return this->theAtoms.GetIndexIMustContainTheObject("Rational");
  }
  int opElementHyperOctahedral() {
    return this->theAtoms.GetIndexIMustContainTheObject("ElementHyperoctahedral");
  }
  int opDouble() {
    return this->theAtoms.GetIndexIMustContainTheObject("Double");
  }
  int opAlgNumber() {
    return this->theAtoms.GetIndexIMustContainTheObject("AlgebraicNumber");
  }
  int opElementWeylAlgebra() {
    return this->theAtoms.GetIndexIMustContainTheObject("ElementWeylAlgebra");
  }
  int opPoly() {
    return this->theAtoms.GetIndexIMustContainTheObject("PolynomialRational");
  }
  int opPolyOverANs() {
    return this->theAtoms.GetIndexIMustContainTheObject("PolynomialOverANs");
  }
  int opEllipticCurveElementsRational() {
    return this->theAtoms.GetIndexIMustContainTheObject("EllipticCurveElementsRational");
  }
  int opEllipticCurveElementsZmodP() {
    return this->theAtoms.GetIndexIMustContainTheObject("EllipticCurveElementsZmodP");
  }
  int opRationalFunction() {
    return this->theAtoms.GetIndexIMustContainTheObject("RationalFunction");
  }
  int opDifferentiate() {
    return this->theAtoms.GetIndexIMustContainTheObject("Differentiate");
  }
  int opDifferential() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\diff");
  }
  int opIndefiniteIndicator() {
    return this->theAtoms.GetIndexIMustContainTheObject("IndefiniteIndicator");
  }
  int opIntegral() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\int");
  }
  int opAbsoluteValue() {
    return this->theAtoms.GetIndexIMustContainTheObject("|");
  }
  int opMatTensorRat() {
    return this->theAtoms.GetIndexIMustContainTheObject("MatrixTensorRational");
  }
  int opWeylGroupRep() {
    return this->theAtoms.GetIndexIMustContainTheObject("WeylGroupRep");
  }
  int opFreeze() {
    return this->theAtoms.GetIndexIMustContainTheObject("Freeze");
  }
  int opString() {
    return this->theAtoms.GetIndexIMustContainTheObject("string");
  }
  int opElementUEoverRF() {
    return this->theAtoms.GetIndexIMustContainTheObject("ElementUEoverRF");
  }
  int opHyperoctahedralGroupRep() {
    return this->theAtoms.GetIndexIMustContainTheObject("HyperoctahedralGroupRepresentation");
  }
  int opWeylGroupVirtualRep() {
    return this->theAtoms.GetIndexIMustContainTheObject("WeylGroupVirtualRep");
  }
  int opElementTensorGVM() {
    return this->theAtoms.GetIndexIMustContainTheObject("ElementTensorGVM");
  }
  int opCharSSAlgMod() {
    return this->theAtoms.GetIndexIMustContainTheObject("CharSSAlgMod");
  }
  int opSSLieAlg() {
    return this->theAtoms.GetIndexIMustContainTheObject("SemisimpleLieAlg");
  }
  int opSemisimpleSubalgebras() {
    return this->theAtoms.GetIndexIMustContainTheObject("SemisimpleSubalgebras");
  }
  int opCandidateSSsubalgebra() {
    return this->theAtoms.GetIndexIMustContainTheObject("CandidateSSsubalgebra");
  }
  int opInfinity() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\infty");
  }
  int opEmptySet() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\emptyset");
  }
  int opIn() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\in");
  }
  int opPi() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\pi");
  }
  int opE() {
    return this->theAtoms.GetIndexIMustContainTheObject("e");
  }
  int opImaginaryUnit() {
    return this->theAtoms.GetIndexIMustContainTheObject("i");
  }
  int opLogBase() {
    return this->theAtoms.GetIndexIMustContainTheObject("LogBase");
  }
  int opLog() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\log");
  }
  int opPhantom() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\phantom");
  }
  int opCirc() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\circ");
  }
  int opSin() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\sin");
  }
  int opFactorial() {
    return this->theAtoms.GetIndexIMustContainTheObject("!");
  }
  int opLimitBoundary() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\limits");
  }
  int opLimitProcess() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\to");
  }
  int opLimit() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\lim");
  }
  int opCos() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\cos");
  }
  int opArcTan() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\arctan");
  }
  int opArcSin() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\arcsin");
  }
  int opArcCos() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\arccos");
  }
  int opTan() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\tan");
  }
  int opCot() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\cot");
  }
  int opSec() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\sec");
  }
  int opCsc() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\csc");
  }
  int opLittelmannPath() {
    return this->theAtoms.GetIndexIMustContainTheObject("LittelmannPath");
  }
  int opLRO() {
    return this->theAtoms.GetIndexIMustContainTheObject("LRO");
  }
  int opWeylGroup() {
    return this->theAtoms.GetIndexIMustContainTheObject("WeylGroup");
  }
  int opUnion() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\cup");
  }
  int opIntersection() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\cap");
  }
  int opPolynomialVariables() {
    return this->theAtoms.GetIndexIMustContainTheObject("PolyVars");
  }
  int opWeylAlgebraVariables() {
    return this->theAtoms.GetIndexIMustContainTheObject("DiffOpVars");
  }
  int opContexT() {
    return this->theAtoms.GetIndexIMustContainTheObject("Context");
  }
  int opWeylGroupElement() {
    return this->theAtoms.GetIndexIMustContainTheObject("ElementWeylGroup");
  }
  int opEndStatement() {
    return this->theAtoms.GetIndexIMustContainTheObject(";");
  }
  int opUnionNoRepetition() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\sqcup");
  }
  int opBind() {
    return this->theAtoms.GetIndexIMustContainTheObject("Bind");
  }
  int opOr() {
    return this->theAtoms.GetIndexIMustContainTheObject("or");
  }
  int opAnd() {
    return this->theAtoms.GetIndexIMustContainTheObject("and");
  }
  int opPlus() {
    return this->theAtoms.GetIndexIMustContainTheObject("+");
  }
  int opDirectSum() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\oplus");
  }
  int opUserInputTextBox() {
    return this->theAtoms.GetIndexIMustContainTheObject("userInputTextBox");
  }
  int opMod() {
    return this->theAtoms.GetIndexIMustContainTheObject("mod");
  }
  int opMinus() {
    return this->theAtoms.GetIndexIMustContainTheObject("-");
  }
  int opTimes() {
    return this->theAtoms.GetIndexIMustContainTheObject("*");
  }
  int opSum() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\sum");
  }
  int opCrossProduct() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\times");
  }
  int opTensor() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\otimes");
  }
  int opBinom() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\binom");
  }
  int opSetMinus() {
    return this->theAtoms.GetIndexIMustContainTheObject("\\setminus");
  }
  int opLieBracket() {
    return this->theAtoms.GetIndexIMustContainTheObject("[]");
  }
  int opDivide() {
    return this->theAtoms.GetIndexIMustContainTheObject("/");
  }
  int opInterpretProblem() {
    return this->theAtoms.GetIndexIMustContainTheObject("InterpretProblem");
  }
  int opInterpretProblemGiveUp() {
    return this->theAtoms.GetIndexIMustContainTheObject("ProblemGiveUp");
  }
  bool AppendOpandsReturnTrueIfOrderNonCanonical(const Expression& input, List<Expression>& output, int theOp);
  bool AppendMultiplicandsReturnTrueIfOrderNonCanonical(Expression& theExpression, List<Expression>& output) {
    return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opTimes());
  }
  bool AppendSummandsReturnTrueIfOrderNonCanonical(const Expression& theExpression, List<Expression>& output) {
    return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus());
  }
  void SpecializeBoundVars(Expression& toBeSubbedIn, MapList<Expression, Expression>& matchedPairs);
  Expression* PatternMatch(
    const Expression& thePattern,
    Expression& theExpression,
    MapList<Expression, Expression>& bufferPairs,
    const Expression* condition = nullptr,
    std::stringstream* theLog = nullptr
  );
  bool ProcessOneExpressionOnePatternOneSub(
    const Expression& thePattern,
    Expression& theExpression,
    MapList<Expression, Expression>& bufferPairs, std::stringstream* theLog = nullptr
  );
  static void CheckInputNotSameAsOutput(const Expression& input, const Expression& output) {
    if (&input == &output) {
      crash << "This is a programming error: the input expression, equal to "
      << input.ToString() << " has the same address as the output expression. "
      << crash;
    }
  }
//  bool OrderMultiplicationTreeProperly(int commandIndex, Expression& theExpression);
  template <class theType>
  bool ConvertToTypeUsingFunction(Expression::FunctionAddress theFun, const Expression& input, Expression& output) {
    MacroRegisterFunctionWithName("Calculator::ConvertToTypeUsingFunction");
    if (input.IsOfType<theType>()) {
      output = input;
      return true;
    }
    if (theFun == nullptr) {
      return false;
    }
    if (!theFun(*this, input, output)) {
      this->Comments << "<hr>Conversion function failed on " << input.ToString() << ". ";
      return false;
    }
    return output.IsOfType<theType>();
  }
  template <class theType>
  bool CallConversionFunctionReturnsNonConstUseCarefully(
    Expression::FunctionAddress theFun, const Expression& input, theType*& outputData
  ) {
    MacroRegisterFunctionWithName("Calculator::CallConversionFunctionReturnsNonConstUseCarefully");
    Expression tempE;
    if (!this->ConvertToTypeUsingFunction<theType>(theFun, input, tempE)) {
      return false;
    }
    outputData = &tempE.GetValueNonConst<theType>();
    return true;
  }
  bool CallCalculatorFunction(Expression::FunctionAddress theFun, const Expression& input, Expression& output) {
    if (&input == &output) {
      Expression inputCopy = input;
      return this->CallCalculatorFunction(theFun, inputCopy, output);
    }
    if (!theFun(*this, input, output)) {
      return false;
    }
    return !output.IsError();
  }
  bool ExpressionMatchesPattern(
    const Expression& thePattern,
    const Expression& input,
    MapList<Expression, Expression>& matchedExpressions,
    std::stringstream* commentsGeneral = nullptr
  );
  static bool innerLogEvaluationStepsHumanReadableNested(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerLogEvaluationStepsHumanReadableMerged(
    Calculator& theCommands, const Expression& input, Expression& output
  );

  static bool innerReverseOrdeR(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerReverseOrderRecursively(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialWithEWA(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerEWAorPoly(theCommands, input, output, true);
  }
  static bool innerElementWeylAlgebra(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerEWAorPoly(theCommands, input, output, false);
  }
  static bool innerEWAorPoly(Calculator& theCommands, const Expression& input, Expression& output, bool assignPoly);
  static bool innerOperationBinary(Calculator& theCommands, const Expression& input, Expression& output, int theOp);
  static bool innerInterpolatePoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTimes(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerOperationBinary(theCommands, input, output, theCommands.opTimes());
  }
  bool ReadTestStrings(
    HashedList<std::string, MathRoutines::HashString>& outputCommands, List<std::string>& outputResults
  );
  std::string WriteFileToOutputFolderReturnLink(const std::string& fileContent, const std::string& fileName, const std::string &linkText);
  bool WriteTestStrings(List<std::string>& inputCommands, List<std::string>& inputResults);
  static bool innerAutomatedTest(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAutomatedTestSetKnownGoodCopy(Calculator& theCommands, const Expression& input, Expression& output);
  int GetNumBuiltInFunctions();
  void AutomatedTestRun(
    List<std::string>& outputCommandStrings,
    List<std::string>& outputResultsWithInit,
    List<std::string>& outputResultsNoInit
  );
  static bool innerTranspose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetElementWeylGroup(Calculator& theCommands, const Expression& input, Expression& output);
  static bool EvaluateCarryOutActionSSAlgebraOnGeneralizedVermaModule(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool outerStandardFunction(
    Calculator& theCommands, const Expression& input, Expression& output, int opIndexParentIfAvailable
  );
  static bool outerPlus(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerPowerRaiseToFirst(Calculator& theCommands, const Expression& input, Expression& output);
  static bool CollectCoefficientsPowersVar(
    const Expression& input,
    const Expression& theVariable,
    VectorSparse<Expression>& outputPositionIiscoeffXtoIth
  );
  bool CollectOpands(const Expression& input, int theOp, List<Expression>& outputOpands);
  bool CollectOpandsAccumulate(const Expression& input, int theOp, List<Expression>& outputOpands);
  static bool CollectSummands(
    Calculator& theCommands,
    const Expression& input,
    MonomialCollection<Expression, Rational>& outputSum
  );
  static bool outerTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool StandardIsDenotedBy(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerMinus(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCombineFractions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCheckRule(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociate(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSubZeroDivAnythingWithZero(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCancelMultiplicativeInverse(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAssociateExponentExponent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociateTimesDivision(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFlattenCommandEnclosuresOneLayer(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerMultiplyAtoXtimesAtoYequalsAtoXplusY(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool outerExtractBaseMultiplication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerMeltBrackets(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyByOne(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerTimesToFunctionApplication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDistributeTimes(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDistributeTimesConstant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDistribute(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    int theAdditiveOp = - 1,
    int theMultiplicativeOp = - 1,
    bool constantsOnly = false
  );
  static bool outerLeftDistributeBracketIsOnTheLeft(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    int theAdditiveOp = - 1,
    int theMultiplicativeOp = - 1,
    bool constantsOnly = false
  );
  static bool outerRightDistributeBracketIsOnTheRight(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    int theAdditiveOp = - 1,
    int theMultiplicativeOp = - 1,
    bool constantsOnly = false
  );
  static bool EvaluateIf(Calculator& theCommands, const Expression& input, Expression& output);
  template<class theType>
  bool GetMatriXFromArguments(
    const Expression& theExpression,
    Matrix<theType>& outputMat,
    Expression* inputOutputStartingContext = nullptr,
    int targetNumColsNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr
  ) {
    Expression tempE = theExpression;
    if (tempE.IsLisT()) {
      tempE.SetChildAtomValue(0, this->opSequence());
    }
    return this->GetMatrix(tempE, outputMat, inputOutputStartingContext, targetNumColsNonMandatory, conversionFunction);
  }
  template<class theType>
  bool GetMatrix(
    const Expression& input,
    Matrix<theType>& outputMat,
    Expression* inputOutputStartingContext = nullptr,
    int targetNumColsNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr
  );
  template <class theType>
  bool GetVectorFromFunctionArguments(
    const Expression& input,
    Vector<theType>& output,
    Expression* inputOutputStartingContext = nullptr,
    int targetDimNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr
  ) {
    Expression tempE = input;
    if (tempE.IsLisT()) {
      tempE.SetChildAtomValue(0, this->opSequence());
    }
    return this->GetVectoR(tempE, output, inputOutputStartingContext, targetDimNonMandatory, conversionFunction);
  }
  bool GetSumProductsExpressions(const Expression& inputSum, List<List<Expression> >& outputSumMultiplicands);
  bool GetVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory = - 1);
  bool ConvertExpressionsToCommonContext(List<Expression>& inputOutputEs, Expression* inputOutputStartingContext = nullptr);
  bool GetVectoRInt(const Expression& input, List<int>& output);
  bool GetVectorLargeIntFromFunctionArguments(const Expression& input, List<LargeInteger>& output);
  bool GetMatrixDoubles(const Expression& input, Matrix<double>& output, int DesiredNumcols = - 1);

  bool GetVectorDoubles(const Expression& input, Vector<double>& output, int DesiredDimensionNonMandatory = - 1);
  bool GetVectorDoublesFromFunctionArguments(
    const Expression& input, Vector<double>& output, int DesiredDimensionNonMandatory = - 1
  );
  template <class theType>
  bool GetVectoR(
    const Expression& input,
    Vector<theType>& output,
    Expression* inputOutputStartingContext = nullptr,
    int targetDimNonMandatory = - 1,
    Expression::FunctionAddress conversionFunction = nullptr
  );
  static bool innerFunctionToMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGenerateMultiplicativelyClosedSet(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerDeterminantPolynomial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInvertMatrixVerbose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSuffixNotationForPostScript(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFreudenthalEval(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFreudenthalFull(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerKillingForm(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGCDOrLCMPoly(Calculator& theCommands, const Expression& input, Expression& output, bool doGCD);
  static bool innerLCMPoly(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerGCDOrLCMPoly(theCommands, input, output, false);
  }
  static bool innerGCDPoly(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerGCDOrLCMPoly(theCommands, input, output, true);
  }
  bool GetListPolysVariableLabelsInLex(
    const Expression& input, Vector<Polynomial<Rational> >& output, Expression& outputContext
  );
  static bool innerPrintAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintB3G2branchingTableCharsOnly(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  bool innerPrintB3G2branchingIntermediate(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    Vectors<RationalFunctionOld>& theHWs,
    branchingData& theG2B3Data,
    Expression& theContext
  );
  static bool innerPrintB3G2branchingTable(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintB3G2branchingTableCommon(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    Vectors<RationalFunctionOld>& outputHWs,
    branchingData& theG2B3Data,
    Expression& theContext
  );
  static bool innerGetChevGen(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetCartanGen(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2CharsOnly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSLieAlgebraShort(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerPrintSSLieAlgebra(theCommands, input, output, false);
  }
  static bool innerPrintSSLieAlgebraVerbose(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerPrintSSLieAlgebra(theCommands, input, output, true);
  }
  static bool innerWriteSSLieAlgebraToHD(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, bool Verbose);
  static bool innerWriteToHDOrPrintSSLieAlgebra(
    Calculator& theCommands, const Expression& input, Expression& output, bool Verbose, bool writeToHD
  );
  static bool innerRootSubsystem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConesIntersect(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPerturbSplittingNormal(Calculator& theCommands, const Expression& input, Expression& output);
  template<class coefficient>
  bool GetTypeWeight(
    Calculator& theCommands,
    const Expression& input,
    Vector<coefficient>& outputWeightSimpleCoords,
    Expression& outputWeightContext,
    SemisimpleLieAlgebra*& ambientSSalgebra,
    Expression::FunctionAddress ConversionFun
  );
  template<class coefficient>
  bool GetTypeHighestWeightParabolic(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    Vector<coefficient>& outputWeightHWFundcoords,
    Selection& outputInducingSel,
    Expression& outputHWContext,
    SemisimpleLieAlgebra*& ambientSSalgebra,
    Expression::FunctionAddress ConversionFun
  );
  static bool innerGroebnerGrLex(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerGroebner(theCommands, input, output, true);
  }
  static bool innerGroebnerLex(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerGroebner(theCommands, input, output, false);
  }
  static bool innerGroebnerRevLex(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerGroebner(theCommands, input, output, false, true);
  }
  static bool innerGroebnerModZpLex(Calculator& theCommands, const Expression& input, Expression& output) {
    return theCommands.innerGroebner(theCommands, input, output, false, false, true);
  }
  static bool innerGroebner(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    bool useGr,
    bool useRevLex = false,
    bool useModZp = false
  );
  static bool innerKLcoeffs(Calculator& theCommands, const Expression& input, Expression& output);
  //  static bool innerSSLieAlgebra
  //  (Calculator& theCommands, const Expression& input, Expression& output)
  //  { return theCommands.innerSSLieAlgebra(theCommands, input, output, false);
  //  }
  static bool innerSplitFDpartB3overG2old(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2inner(Calculator& theCommands, branchingData& theG2B3Data, Expression& output);
  static bool innerLittelmannOperator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAnimateLittelmannPaths(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFactorPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLSPath(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestMonomialBaseConjecture(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerJacobiSymbol(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHWVCommon(
    Calculator& theCommands,
    Expression& output,
    Vector<RationalFunctionOld>& highestWeightFundCoords,
    Selection& selectionParSel,
    Expression& hwContext,
    SemisimpleLieAlgebra* owner,
    bool Verbose = true
  );
  bool innerWriteGenVermaModAsDiffOperatorInner(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    Vectors<Polynomial<Rational> >& theHws,
    Expression& hwContext,
    Selection& selInducing,
    SemisimpleLieAlgebra* owner,
    bool AllGenerators,
    std::string* xLetter,
    std::string* partialLetter,
    std::string* exponentVariableLetter,
    bool useNilWeight,
    bool ascending
  );
  template<class coefficient>
  static bool TypeHighestWeightParabolic(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    Vector<coefficient>& outputWeight,
    Selection& outputInducingSel,
    Expression* outputContext = nullptr
  );
  static bool innerPrintZnEnumeration(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNot(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerZmodP(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAttemptExtendingEtoHEFwithHinCartan(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerAdCommonEigenSpaces(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEmbedG2inB3(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWriteGenVermaModAsDiffOperators(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    bool AllGenerators,
    bool useNilWeight,
    bool ascending
  );
  static bool innerWriteGenVermaModAsDiffOperatorsGeneratorOrder(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return theCommands.innerWriteGenVermaModAsDiffOperators(theCommands, input, output, false, false, true);
  }
  static bool innerWriteGenVermaModAsDiffOperatorsNilOrderDescending(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return theCommands.innerWriteGenVermaModAsDiffOperators(theCommands, input, output, false, true, false);
  }
  static bool innerWriteGenVermaModAsDiffOperatorsAllGensNilOrderDescending(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return theCommands.innerWriteGenVermaModAsDiffOperators(theCommands, input, output, true, false, false);
  }
  static bool innerCasimir(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetLinksToSimpleLieAlgerbas(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCharacterSSLieAlgFD(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSSubalgebras(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    bool doForceRecompute,
    bool doAttemptToSolveSystems,
    bool doComputePairingTable,
    bool doComputeModuleDecomposition,
    bool doComputeNilradicals,
    bool doAdjustCentralizers
  );
  static bool innerPrintSSsubalgebrasNilradicals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasRecompute(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasNoCentralizers(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasNoSolutions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasRegular(Calculator& theCommands, const Expression& input, Expression& output);
  void AddEmptyHeadedCommand();
  Calculator();
  int AddOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName);
  void AddOperationNoRepetitionAllowed(const std::string& theOpName);
  void AddOperationBuiltInType(const std::string& theOpName);
  void AddTrigSplit(const std::string& trigFun, const List<std::string>& theVars);
  void AddKnownDoubleConstant(const std::string& theConstantName, double theValue);
  void AddOperationComposite(
    const std::string& theOpName,
    Expression::FunctionAddress handler,
    const std::string& opArgumentListIgnoredForTheTimeBeing,
    const std::string& opDescription,
    const std::string& opExample,
    bool isInner,
    bool visible,
    bool experimental,
    const std::string& inputAdditionalIdentifier,
    const std::string& inputCalculatorIdentifier
  );
  void AddOperationBinaryInnerHandlerWithTypes(
    const std::string& theOpName,
    Expression::FunctionAddress innerHandler,
    int leftType,
    int rightType,
    const std::string& opDescription,
    const std::string& opExample,
    bool visible = true,
    bool experimental = false,
    const std::string& inputAdditionalIdentifier = "",
    const std::string& inputCalculatorIdentifier = ""
  );
  void AddOperationHandler(
    const std::string& theOpName,
    Expression::FunctionAddress handler,
    const std::string& opArgumentListIgnoredForTheTimeBeing,
    const std::string& opDescription,
    const std::string& opExample,
    bool isInner,
    bool visible,
    bool experimental,
    const std::string& inputAdditionalIdentifier,
    const std::string& inputCalculatorIdentifier,
    bool inputDisabledByDefault,
    const std::string& parentOpThatBansHandler
  );
  void AddOperationInnerHandler(
    const std::string& theOpName,
    Expression::FunctionAddress innerHandler,
    const std::string& opArgumentListIgnoredForTheTimeBeing,
    const std::string& opDescription,
    const std::string& opExample,
    bool visible = true,
    bool experimental = false,
    const std::string& inputAdditionalIdentifier = "",
    const std::string& inputCalculatorIdentifier = "",
    bool inputDisabledByDefault = false,
    const std::string& parentOpThatBansHandler = ""
  ) {
    this->AddOperationHandler(
      theOpName,
      innerHandler,
      opArgumentListIgnoredForTheTimeBeing,
      opDescription,
      opExample,
      true,
      visible,
      experimental,
      inputAdditionalIdentifier,
      inputCalculatorIdentifier,
      inputDisabledByDefault,
      parentOpThatBansHandler
    );
  }
  void AddOperationOuterHandler(
    const std::string& theOpName,
    Expression::FunctionAddress outerHandler,
    const std::string& opArgumentListIgnoredForTheTimeBeing,
    const std::string& opDescription,
    const std::string& opExample,
    bool visible = true,
    bool experimental = false,
    const std::string& inputAdditionalIdentifier = "",
    const std::string& inputCalculatorIdentifier = "",
    bool inputDisabledByDefault = false,
    const std::string& parentOpThatBansHandler = ""
  );
  void initialize();
  void reset();
  void initPredefinedWordSplits();
  void initAtomsThatFreezeArguments();
  void initAtomsNonCacheable();
  void initAtomsThatAllowCommutingOfArguments();
  void initStringsThatSplitIfFollowedByDigit();
  void initOperationsInterpretedAsFunctionsMultiplicatively();
  void initOperationsThatAreKnownFunctions();
  void initArithmeticOperations();
  void initBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  void initBuiltInAtomsNotInterpretedAsFunctions();
  void initAtomsNotGoodForChainRule();
  void initPredefinedStandardOperations();
  void initPredefinedStandardOperationsWithoutHandler();
  void initPredefinedInnerFunctions();
  void initCalculusTestingFunctions();
  void initAdminFunctions();
  void initPredefinedOperationsComposite();
  void initComputationStats();
  bool ExtractExpressions(Expression& outputExpression, std::string* outputErrors);
  void EvaluateCommands();
  bool TimedOut();
  static bool EvaluateExpression(Calculator& theCommands, const Expression& input, Expression& output);
  static bool EvaluateExpression(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    bool& outputIsCacheable,
    int opIndexParentIfAvailable
  );
  void Evaluate(const std::string& theInput);
  bool ParseAndExtractExpressions(
    const std::string& input,
    Expression& output,
    List<SyntacticElement>& outputSynSoup,
    List<SyntacticElement>& outputSynStack,
    std::string* outputSynErrors
  );
  bool Parse(
    const std::string& input,
    Expression& output
  );
  bool isLeftSeparator(unsigned char c);
  bool isRightSeparator(unsigned char c);
  bool ShouldSplitOutsideQuotes(const std::string& left, char right);
  void ParseFillDictionary(const std::string& input, List<SyntacticElement>& output);
  void ParseFillDictionary(const std::string& input);
  class Test {
  public:
    static bool All();
    static bool ParseDecimal(Calculator& ownerInitialized);
  };
};

class CalculatorConversions {
public:
  ////////////////////Conversion from expression tree/////////////////////////////////
  //conversions from expression tree to type
  static bool innerElementSemisimpleLieAlgebraRationalCoeffs(
    Calculator& theCommands,
    const Expression& input,
    ElementSemisimpleLieAlgebra<Rational>& output,
    SemisimpleLieAlgebra& owner
  );
  static bool innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
    Calculator& theCommands,
    const Expression& input,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
    SemisimpleLieAlgebra& owner
  );
  static bool innerDynkinType(Calculator& theCommands, const Expression& input, DynkinType& output);
  static bool innerDynkinSimpleType(Calculator& theCommands, const Expression& input, DynkinSimpleType& output);
  static bool innerSlTwoSubalgebraPrecomputed(Calculator& theCommands, const Expression& input, slTwoSubalgebra& output);
  static bool innerLoadFromObject(Calculator& theCommands, const Expression& input, RationalFunctionOld& output);
  static bool innerAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output);
  //conversions from expression tree to expression containing type
  template <class coefficient>
  static bool innerPolynomial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRationalFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerElementUE(
    Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra& inputOwner
  );
  static bool outerMatrixExpressionsToMatrixOfType(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerMakeMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixDouble(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixAlgebraic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixRationalFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixRationalTensorForm(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLoadFileIntoString(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMakeElementHyperOctahedral(Calculator& theCommands, const Expression& input, Expression& output);
  ////////////////////Conversion to expression tree/////////////////////////////////////
  //converstion from type to expression tree.
  template <class coefficient>
  static bool innerExpressionFromPoly(
    Calculator& theCommands,
    const Polynomial<coefficient>& input,
    Expression& output,
    Expression* inputContext = nullptr
  );
  static bool innerExpressionFromRF(
    Calculator& theCommands,
    const RationalFunctionOld& input,
    Expression& output,
    Expression* inputContext = nullptr
  );
  static bool innerLoadKey(
    Calculator& theCommands, const Expression& inputStatementList, const std::string& inputKey, Expression& output
  );
  static bool innerLoadKeysFromStatementList(
    Calculator& theCommands,
    const Expression& input,
    MapList<std::string, Expression, MathRoutines::HashString>& output,
    std::stringstream* commentsOnFailure = nullptr,
    bool allowFailure = false
  );
  static bool innerLoadKeysFromStatementList(
    Calculator& theCommands,
    const Expression& input,
    MapList<Expression, Expression>& output,
    std::stringstream* commentsOnFailure = nullptr,
    bool allowFailure = false
  );
  static bool innerStoreSemisimpleSubalgebras(
    Calculator& theCommands, const SemisimpleSubalgebras& input, Expression& output
  );
  static std::string innerStringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input);

  static bool innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
    Calculator& theCommands, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output
  );
  static bool innerExpressionFromUE(
    Calculator& theCommands,
    const ElementUniversalEnveloping<RationalFunctionOld>& input,
    Expression& output,
    Expression* inputContext = nullptr
  );
  static bool innerStoreCandidateSA(Calculator& theCommands, const CandidateSSSubalgebra& input, Expression& output);
  static bool innerExpressionFromDynkinType(Calculator& theCommands, const DynkinType& input, Expression& output);
  static bool innerExpressionFromDynkinSimpleType(Calculator& theCommands, const DynkinSimpleType& input, Expression& output);
  static bool innerExpressionFromElementSemisimpleLieAlgebraRationals(
    Calculator& theCommands, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output
  );
  //conversions from expression containing type to expression tree
  static bool innerStoreSemisimpleLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSSLieAlgebra(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    SemisimpleLieAlgebra*& outputSSalgebra
  );
  static bool innerCandidateSAPrecomputed(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    CandidateSSSubalgebra& outputPointer,
    SemisimpleSubalgebras& owner
  );
  static bool innerLoadWeylGroup(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerStoreSemisimpleSubalgebras(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerSlTwoSubalgebraPrecomputed(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLoadSemisimpleSubalgebras(Calculator& theCommands, const Expression& inpuT, Expression& output);
  static bool innerExpressionFromChevalleyGenerator(
    Calculator& theCommands, const ChevalleyGenerator& input, Expression& output
  );
  static bool innerExpressionFromMonomialUE(
    Calculator& theCommands,
    const MonomialUniversalEnveloping<RationalFunctionOld>& input,
    Expression& output,
    Expression* inputContext = nullptr
  );
  static bool innerExpressionFromBuiltInType(Calculator& theCommands, const Expression& input, Expression& output);
  template <class coefficient>
  static bool innerExpressionFromPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionFromRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionFromUE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionFrom(Calculator& theCommands, const MonomialP& input, Expression& output);
};

template <class theType>
bool Calculator::GetVectoR(
  const Expression& input,
  Vector<theType>& output,
  Expression* inputOutputStartingContext,
  int targetDimNonMandatory,
  Expression::FunctionAddress conversionFunction
) {
  MacroRegisterFunctionWithName("Calculator::GetVector");
  List<Expression> nonConvertedEs;
  if (!this->GetVectorExpressions(input, nonConvertedEs, targetDimNonMandatory)) {
    return false;
  }
  List<Expression> convertedEs;
  convertedEs.SetSize(nonConvertedEs.size);
  for (int i = 0; i < nonConvertedEs.size; i ++) {
    if (!this->ConvertToTypeUsingFunction<theType>(conversionFunction, nonConvertedEs[i], convertedEs[i])) {
      return false;
    }
  }
  if (!this->ConvertExpressionsToCommonContext(convertedEs, inputOutputStartingContext)) {
    return false;
  }
  if (targetDimNonMandatory > 0) {
    if (convertedEs.size != targetDimNonMandatory) {
      return false;
    }
  }
  output.SetSize(convertedEs.size);
  for (int i = 0; i < convertedEs.size; i ++) {
    output[i] = convertedEs[i].GetValue<theType>();
  }
  return true;
}

template <class coefficient>
bool CalculatorConversions::innerExpressionFromPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromPoly");
  if (!input.IsOfType<Polynomial<coefficient> >()) {
    return false;
  }
  const Polynomial<coefficient>& thePoly = input.GetValue<Polynomial<coefficient> >();
  Expression theContext = input.GetContext();
  return CalculatorConversions::innerExpressionFromPoly(theCommands, thePoly, output, &theContext);
}

template <class theType>
bool Calculator::GetMatrix(
  const Expression& input,
  Matrix<theType>& outputMat,
  Expression* inputOutputStartingContext,
  int targetNumColsNonMandatory,
  Expression::FunctionAddress conversionFunction
) {
  MacroRegisterFunctionWithName("Calculator::GetMatrix");
  Matrix<Expression> nonConvertedEs;
  if (!this->GetMatrixExpressions(input, nonConvertedEs, - 1, targetNumColsNonMandatory)) {
    return *this << "Failed to extract matrix of expressions from " << input.ToString();
  }
  Matrix<Expression> convertedEs;
  convertedEs.init(nonConvertedEs.NumRows, nonConvertedEs.NumCols);
  Expression theContext;
  theContext.MakeEmptyContext(*this);
  if (inputOutputStartingContext != nullptr) {
    if (inputOutputStartingContext->IsContext()) {
      theContext =*inputOutputStartingContext;
    }
  }
  for (int i = 0; i <nonConvertedEs.NumRows; i ++) {
    for (int j = 0; j<nonConvertedEs.NumCols; j ++) {
      if (!this->ConvertToTypeUsingFunction<theType>(conversionFunction, nonConvertedEs(i, j), convertedEs(i, j))) {
        if (!nonConvertedEs(i, j).ConvertToType<theType>(convertedEs.elements[i][j])) {
          return false;
        }
      }
      theContext.ContextMergeContexts(theContext, convertedEs(i, j).GetContext(), theContext);
    }
  }
  for (int i = 0; i < convertedEs.NumRows; i ++) {
    for (int j = 0; j < convertedEs.NumCols; j ++) {
      if (!convertedEs(i, j).::Expression::SetContextAtLeastEqualTo(theContext)) {
        return false;
      }
    }
  }
  outputMat.init(convertedEs.NumRows, convertedEs.NumCols);
  for (int i = 0; i < convertedEs.NumRows; i ++) {
    for (int j = 0; j < convertedEs.NumCols; j ++) {
      outputMat(i, j) = convertedEs(i, j).GetValue<theType>();
    }
  }
  if (inputOutputStartingContext != nullptr) {
    *inputOutputStartingContext = theContext;
  }
  return true;
}

template <class coefficient>
bool Expression::MakeSum(Calculator& theCommands, const MonomialCollection<Expression, coefficient>& theSum) {
  MacroRegisterFunctionWithName("Expression::MakeSum");
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  if (theSum.IsEqualToZero()) {
    return this->AssignValue<Rational>(0, theCommands);
  }
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(theSum.size());
  for (int i = 0; i < theSum.size(); i ++) {
    Expression& current = summandsWithCoeff[i];
    if (theSum[i] == oneE) {
      current.AssignValue(theSum.theCoeffs[i], theCommands);
    } else if (!(theSum.theCoeffs[i] == 1)) {
      current.reset(theCommands, 3);
      current.AddChildAtomOnTop(theCommands.opTimes());
      current.AddChildValueOnTop(theSum.theCoeffs[i]);
      current.AddChildOnTop(theSum[i]);
    } else {
      current = theSum[i];
    }
  }
  if (summandsWithCoeff.size < 5) {
    for (int i = 0; i < summandsWithCoeff.size; i ++) {
      for (int j = i; j < summandsWithCoeff.size; j ++) {
        if (summandsWithCoeff[i] > summandsWithCoeff[j] && summandsWithCoeff[j] > summandsWithCoeff[i]) {
          crash << "This is a programming error: bad comparison function: each of the expressions "
          << summandsWithCoeff[i].ToString() << " and " << summandsWithCoeff[j].ToString()
          << " is reported to be greater than the other. " << crash;
        }
      }
    }
  }
  return this->MakeOXdotsX(theCommands, theCommands.opPlus(), summandsWithCoeff);
}

template <typename theType>
int Expression::AddObjectReturnIndex(const theType& inputValue) const {
  (void) inputValue;
  crash << "Please implement AddObjectReturnIndex for the current type. " << crash;
  return - 1;
}

template <class theType>
bool Expression::AssignValueWithContext(const theType& inputValue, const Expression& theContext, Calculator& owner) {
  this->reset(owner, 3);
  this->AddChildAtomOnTop(this->GetTypeOperation<theType>());
  this->AddChildOnTop(theContext);
  return this->AddChildAtomOnTop(this->AddObjectReturnIndex(inputValue));
}

template<class theType>
int Expression::GetTypeOperation() const {
  crash << "Please implement GetTypeOperation for the current type. " << crash;
  return - 1;
}

template <class theType>
bool Expression::AssignValue(const theType& inputValue, Calculator& owner) {
  Expression tempE;
  tempE.owner = &owner;
  int curType = tempE.GetTypeOperation<theType>();
  if (
    curType == owner.opPoly() ||
    curType == owner.opRationalFunction() ||
    curType == owner.opElementTensorGVM() ||
    curType == owner.opElementUEoverRF() ||
    curType == owner.opElementWeylAlgebra() ||
    curType == owner.opWeightLieAlgPoly()
  ) {
    crash << "This may or may not be a programming error. "
    << "Assigning value WITHOUT CONTEXT to data type "
    << this->owner->GetOperations()[curType]
    << " is discouraged, and most likely is an error. Crashing to let you know. "
    << crash;
  }
  Expression emptyContext;
  emptyContext.MakeEmptyContext(owner);
  return this->AssignValueWithContext(inputValue, emptyContext, owner);
}

template <class theType>
bool Expression::MergeContextsMyArumentsAndConvertThem(Expression& output, std::stringstream* commentsOnFailure) const {
  MacroRegisterFunctionWithName("Expression::MergeContextsMyArumentsAndConvertThem");
  this->CheckInitialization();
  Expression mergedContexts;
  if (!this->MergeContextsMyAruments(mergedContexts, commentsOnFailure)) {
    return false;
  }
  output.reset(*this->owner, this->children.size);
  output.AddChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i = 1; i < mergedContexts.children.size; i ++) {
    if (!mergedContexts[i].ConvertToType<theType>(convertedE)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "<hr>Failed to convert "
        << mergedContexts[i].ToString() << " to the desired type. ";
      }
      return false;
    }
    output.AddChildOnTop(convertedE);
  }
  return true;
}

template<class coefficient>
bool Calculator::GetTypeWeight(
  Calculator& theCommands,
  const Expression& input,
  Vector<coefficient>& outputWeightSimpleCoords,
  Expression& outputWeightContext,
  SemisimpleLieAlgebra*& ambientSSalgebra,
  Expression::FunctionAddress ConversionFun
) {
  MacroRegisterFunctionWithName("Calculator::GetTypeWeight");
  if (!input.IsListNElements(3)) {
    theCommands
    << "Function TypeHighestWeightParabolic is expected to have two arguments: "
    << "Semisimple  algebra type, highest weight in simple coordinates. ";
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  if (!Calculator::CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, leftE, ambientSSalgebra
  )) {
    theCommands << "Error extracting Lie algebra from " << leftE.ToString();
    return false;
  }
  if (!theCommands.GetVectoR<coefficient>(
    middleE, outputWeightSimpleCoords, &outputWeightContext, ambientSSalgebra->GetRank(), ConversionFun
  )) {
    theCommands << "Failed to convert the second argument of HWV to a list of " << ambientSSalgebra->GetRank()
    << " polynomials. The second argument you gave is " << middleE.ToString() << ".";
    return false;
  }
  if (!theCommands.theObjectContainer.theSSLieAlgebras.Contains(ambientSSalgebra->theWeyl.theDynkinType)) {
    crash << "This is a programming error: " << ambientSSalgebra->ToStringLieAlgebraName()
    << " contained object container more than once. " << crash;
  }
  int algebraIndex = theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(ambientSSalgebra->theWeyl.theDynkinType);
  outputWeightContext.ContextSetSSLieAlgebrA(algebraIndex, theCommands);
  return true;
}

template<class coefficient>
bool Calculator::GetTypeHighestWeightParabolic(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  Vector<coefficient>& outputWeightHWFundcoords,
  Selection& outputInducingSel,
  Expression& outputHWContext,
  SemisimpleLieAlgebra*& ambientSSalgebra,
  Expression::FunctionAddress ConversionFun
) {
  if (!input.IsListNElements(4) && !input.IsListNElements(3)) {
    return output.MakeError(
      "Function TypeHighestWeightParabolic is expected to have two or three arguments: "
      "SS algebra type, highest weight, [optional] parabolic selection. ",
      theCommands
    );
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  if (!Calculator::CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, leftE, ambientSSalgebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  if (!theCommands.GetVectoR<coefficient>(
    middleE,
    outputWeightHWFundcoords,
    &outputHWContext,
    ambientSSalgebra->GetRank(),
    ConversionFun
  )) {
    std::stringstream tempStream;
    tempStream << "Failed to convert the second argument of HWV to a list of " << ambientSSalgebra->GetRank()
    << " polynomials. The second argument you gave is " << middleE.ToString() << ".";
    return output.MakeError(tempStream.str(), theCommands);
  }
  if (input.IsListNElements(4)) {
    Vector<Rational> parabolicSel;
    const Expression& rightE = input[3];
    if (!theCommands.GetVectoR<Rational>(rightE, parabolicSel, &outputHWContext, ambientSSalgebra->GetRank(), nullptr)) {
      std::stringstream tempStream;
      tempStream << "Failed to convert the third argument of HWV to a list of " << ambientSSalgebra->GetRank()
      << " rationals. The third argument you gave is " << rightE.ToString() << ".";
      return output.MakeError(tempStream.str(), theCommands);
    }
    outputInducingSel = parabolicSel;
  } else {
    outputInducingSel.init(ambientSSalgebra->GetRank());
    for (int i = 0; i < outputWeightHWFundcoords.size; i ++) {
      if (!outputWeightHWFundcoords[i].IsSmallInteger()) {
        outputInducingSel.AddSelectionAppendNewIndex(i);
      }
    }
  }
  if (!theCommands.theObjectContainer.theSSLieAlgebras.Contains(ambientSSalgebra->theWeyl.theDynkinType)) {
    crash << "This is a programming error: " << ambientSSalgebra->ToStringLieAlgebraName()
    << " contained object container more than once. " << crash;
  }
  int algebraIndex = theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(ambientSSalgebra->theWeyl.theDynkinType);
  outputHWContext.ContextSetSSLieAlgebrA(algebraIndex, theCommands);
  return true;
}

template <class coefficient>
bool Expression::AssignMatrix(
  const Matrix<coefficient>& input,
  Calculator& owner,
  Expression const* inputContext,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix
) {
  MacroRegisterFunctionWithName("Expression::AssignMatrix");
  Matrix<Expression> theMatEs;
  theMatEs.init(input.NumRows, input.NumCols);
  Expression currentElt;
  for (int i = 0; i < input.NumRows; i ++) {
    for (int j = 0; j < input.NumCols; j ++) {
      if (inputContext == nullptr) {
        currentElt.AssignValue(input(i, j), owner);
      } else {
        currentElt.AssignValueWithContext(input(i, j), *inputContext, owner);
      }
      theMatEs(i, j) = currentElt;
    }
  }
  return this->AssignMatrixExpressions(theMatEs, owner, reduceOneRowToSequenceAndOneByOneToNonMatrix, false);
}

template <typename theType>
bool Expression::IsMatrixGivenType(int* outputNumRows, int* outputNumCols, Matrix<theType>* outputMat) const {
  MacroRegisterFunctionWithName("Expression::IsMatrixGivenType");
  int numRows = - 1, numCols = - 1;
  if (outputNumRows == nullptr) {
    outputNumRows = &numRows;
  }
  if (outputNumCols == nullptr) {
    outputNumCols = &numCols;
  }
  if (!this->IsMatrix(outputNumRows, outputNumCols)) {
    return false;
  }
  if (!(*this)[0].StartsWith(this->owner->opMatriX(), 2)) {
    return false;
  }
  if (!(*this)[0][1].IsAtomGivenData(this->GetTypeOperation<theType>())) {
    return false;
  }
  if (outputMat == 0) {
    return true;
  }
  this->owner->GetMatrix(*this, *outputMat);
  return true;
}

template <class coefficient>
bool CalculatorConversions::innerExpressionFromPoly(
  Calculator& theCommands, const Polynomial<coefficient>& input, Expression& output, Expression* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromPoly");
  MonomialCollection<Expression, coefficient> theTerms;
  Expression currentBase, currentPower, currentTerm, currentMultTermE;
  if (!input.IsConstant() && inputContext == nullptr) {
    theCommands << "While converting polynomial to expression, I was given no variable names. Using the "
    << "default variable names x_1, x_2, ... "
    << "Please make sure you are not using those variables for other purposes.";
  }
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].IsConstant()) {
      currentTerm.AssignValue(1, theCommands);
      theTerms.AddMonomial(currentTerm, input.theCoeffs[i]);
      continue;
    }
    bool found = false;
    for (int j = 0; j < input[i].GetMinNumVars(); j ++) {
      if (input[i](j) != 0) {
        if (inputContext != nullptr) {
          currentBase = inputContext->ContextGetContextVariable(j);
        } else {
          currentBase.reset(theCommands);
          currentBase.AddChildAtomOnTop("x");
          currentBase.AddChildValueOnTop(Rational(j));
        }
        if (input[i](j) == 1) {
          currentMultTermE = currentBase;
        } else {
          currentPower.AssignValue(input[i](j), theCommands);
          currentMultTermE.MakeXOX(theCommands, theCommands.opThePower(), currentBase, currentPower);
        }
        if (!found) {
          currentTerm = currentMultTermE;
        } else {
          currentTerm *= currentMultTermE;
        }
        found = true;
      }
    }
    theTerms.AddMonomial(currentTerm, input.theCoeffs[i]);
  }
  return output.MakeSum(theCommands, theTerms);
}

#endif
