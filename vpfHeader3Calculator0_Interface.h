//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader2_h_already_included
#define vpfHeader2_h_already_included

//the following  include contains all the c++ math routines used in the calculator.
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math3_FiniteGroups.h"
#include "vpfHeader2Math4_Graph.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader2Math1_3SemisimpleLieSubalgebras.h"
#include "vpfHeader2Math15_UniversalEnveloping.h"
#include "vpfHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h"

static ProjectInformationInstance ProjectInfoVpfHeader2(__FILE__, "Header file, calculator declaration. ");

class Expression
{ void reset()
  { this->owner=0;
    this->children.size=0;
    this->theData=-1;
    this->format=this->formatDefault;
//    this->IndexBoundVars=inputIndexBoundVars;
  }
  //Definitions.
  //1. Fundamentals.
  //1.1. An atom is an expression with zero children.
  //     *******************************************
  //     We will say informally "an atom equals (the keyword) X" to mean that
  //     the theData of the corresponding atom equals the integer Calculator::opX().
  //     Note that this language use is completely informal, and could be ambiguous:
  //     the theData entry of an atom can be interpreted either as keyword or as an
  //     an actual piece of data (not necessarily equal to Calculator::opX() for some X).
  //     Whenever this ambiguity becomes an issue, the informal language should be dropped,
  //     and explicit reference to the Expression::theData and Expression::children members
  //     should be made.
  //1.2. A list is an expression with 1 or more children whose theData entry equals
  //     0. The atom 0 *MUST* be equal to Calculator::opList().
  //1.3. An expression with 1 or more children is not allowed to have theData entry different
  //     from 0=Calculator::opList(). The system is instructed to
  //     crash and burn shall such a configuration be detected.
  //2. Basic building blocks
  //2.1. A frozen expression is a list whose first entry is an atom equal to Freeze.
  //2.2. A sequence is a list whose first entry is an atom equal to Sequence.
  //2.3. A bound variable is a list with two atomic entries, the first of which
  //     equals Bind.
  //2.4. An error is a list with two entries whose first entry is an atom equal to Error,
  //     and whose second entry is a string.
  //*Note that Calculator::opList() is required to equal zero for reasons of program speed.
  //This is GUARANTEED, and you MAY assume it.
  //If you want to have a list of mathematical objects, use the Sequence
  //data structure. A sequence is a List whose first entry is an atom whose value
  //is Sequence=Calculator::opSequence().
  //
  //-------------------------------------------------------
  //
  //Expressions of built-in types.
  //1. Expressions of built-in types represent hard-coded C++ types.
  //2. An expression is said to be of built-in type if it is a list of 2 or 3 elements
  //   starting with an atom
  //   equal to one of a set of hard-coded names, and ending in an atom whose value
  //   is an integer that uniquely (up to operator==) identifies a C++ structure with the corresponding type.
  //3. A context is a list of 1 or more elements starting with the atom Context. If
  //   a context has only one child (which must then be equal to the atom Context),
  //   then we say that we have an "empty context".
  //4. An expression is said to have context if it is a list of at least two elements,
  //   the second element of which is a context.
  //5. If an expression of built-in type has 3 children, the middle child
  //   must be a context. If an expression of built-in type has 2 children, we say that
  //   the expression does not have context.
  //6. Two expressions of built-in type with equal types and C++ identifiers, one having a
  //   context that is empty, and the other having no context,
  //   are considered to represent one and the same element.
  //-------------------------------------------------------
  //-------------------------------------------------------
  //-------------------------------------------------------
  //Notes on Expression implementation.
  //While the preceding notes are to be considered as fixed, the following notes
  //are implementation specific and are subject to change.
  //The i^th child of an expression can be accessed as const using operator[](i).
  //The children of an expression are kept as a list of integers indicating the children's
  //position in Calculator::theExpressionContainer.
  //Calculator::theExpressionContainer is a Hashed List of references and must not be modified
  //directly in any way.
  //Motivation for this implementation choice. The original implementation
  //had Expression contain all of its children as List<Expression>, making the copy operator=
  //a recursive function. While this is one of the least-head-aching designs,
  //it also proved to be remarkably slow: here is why.
  //When processing an expression, one must traverse all of its subtrees.
  //Making temporary copies of all subtrees is then approximately O(n^2),
  //where n^2 is the number of nodes of the tree.
  //
  //For a tree with 1k nodes this is already
  //unacceptably slow, if it could be avoided.
  //
  //If change is to be applied to
  //most subtrees, then the approximate cost of O(n^2) operations obviously cannot be avoided.
  //However, in most computations, subtrees need not be changed - in reality, most expression
  //will arrive to the calculator in reduced or close-to-reduced form.
  //
  //This explains our choice of keeping all children of an expression as a reference to
  //an ever-growing collection of Expressions.
  //If a single instance of Calculator is to run over long periods of time,
  //it is possible that not all such expressions are in use, and we run out of RAM memory.
  //If that is to happen, some sort of garbage collection will have to be implemented.
  //However, for the current calculator purposes, no such danger exists.
  public:
  int theData;
  HashedList<int, MathRoutines::IntUnsignIdentity> children;
  Calculator* owner;
  ///////////////////////////////////////
  //The status of the following data has not been decided:
  int format;
//////
  bool flagDeallocated;
  typedef bool (*FunctionAddress) (Calculator& theCommands, const Expression& input, Expression& output);
//////
  void operator=(const Expression& other)
  { this->theData=other.theData;
    this->children=other.children;
    this->owner=other.owner;
    this->format=other.format;
  }
  void operator=(const Rational& other)
  { this->CheckInitialization();
    this->AssignValue(other, *this->owner);
  }
  void operator=(int other)
  { this->CheckInitialization();
    this->AssignValue(other, *this->owner);
  }
  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot, formatNoBracketsForFunctionArgument, formatMatrix, formatMatrixRow, formatUseFrac
  };
  friend std::ostream& operator << (std::ostream& output, const Expression& theMon)
  { output << theMon.ToString();
    return output;
  }
  friend Expression operator*(const Expression& left, const Expression& right);
  friend Expression operator/(const Expression& left, const Expression& right);
  friend Expression operator+(const Expression& left, const Expression& right);
  friend Expression operator-(const Expression& left, const Expression& right);

  void reset(Calculator& newBoss, int numExpectedChildren=0)
  { this->owner=&newBoss;
    this->theData=0;
    this->format=this->formatDefault;
    this->children.Clear();
    this->children.SetExpectedSize(numExpectedChildren);
  }
  bool AddChildOnTop(const Expression& inputChild);
  bool AddChildAtomOnTop(const std::string& theOperationString);
  bool AddChildAtomOnTop(int theOp)
  { this->CheckInitialization();
    Expression tempE;
    tempE.MakeAtom(theOp, *this->owner);
    return this->AddChildOnTop(tempE);
  }
  void GetBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList)const;
  bool SplitProduct(int numDesiredMultiplicandsLeft, Expression& outputLeftMultiplicand, Expression& outputRightMultiplicand)const;
  void GetBaseExponentForm(Expression& outputBase, Expression& outputExponent)const;
  void GetCoefficientMultiplicandForm(Expression& outputCoeff, Expression& outputNoCoeff)const;
  void GetCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff)const;
  bool SetChildAtomValue(int childIndex, const std::string& theAtom);
  bool SetChildAtomValue(int childIndex, int TheAtomValue);
  bool SetChilD(int childIndexInMe, const Expression& inputChild);
  bool SetChilD(int childIndexInMe, int childIndexInBoss);
  bool AssignMatrixExpressions(const Matrix<Expression>& input, Calculator& owner);
  template<class coefficient>
  bool AssignMatrix(const Matrix<coefficient>& input, Calculator& owner);
  bool DivisionByMeShouldBeWrittenInExponentForm()const;
  bool IsCalculatorStatusChanger()const;
  bool AssignMeMyChild(int childIndex)
  { Expression tempExp=(*this)[childIndex];
    this->operator=(tempExp);
    return true;
  }
  bool IsLisT()const;
  bool IsListNElements(int N=-1)const
  { if (!this->IsLisT())
      return false;
    if (N==-1)
      return true;
    return this->children.size==N;
  }
  bool StartsWithBuiltInAtom()const;
  bool StartsWithFunctionWithComplexRange()const;
  bool StartsWithArithmeticOperation()const;
  bool StartsWith(int theOp=-1, int N=-1)const;
  bool IsListStartingWithAtom(int theOp=-1)const
  { if (!this->IsLisT())
      return false;
    if (!(*this)[0].IsAtom())
      return false;
    if (theOp==-1)
      return true;
    return (*this)[0].theData==theOp;
  }
  bool IsListOfTwoAtomsStartingWith(int theOp)const
  { if (!this->IsListStartingWithAtom(theOp))
      return false;
    if (this->children.size!=2)
      return false;
    return (*this)[1].IsAtom();
  }
  bool HasSameContextArgumentsNoLog()const;
  bool IsFrozen()const;
  bool IsAtomThatFreezesArguments(std::string* outputWhichAtom=0)const;
  bool IsAtomWhoseExponentsAreInterpretedAsFunction(std::string* outputWhichAtom=0)const;
  bool IsPowerOfAtomWhoseExponentsAreInterpretedAsFunction()const;
  bool IsAtomNotInterpretedAsFunction(std::string* outputWhichAtom=0)const;

  bool IsAtom(std::string* outputWhichOperation=0)const;
  bool IsAtomGivenData(int desiredDataUseMinusOneForAny=-1)const;
  bool IsBuiltInAtom(std::string* outputWhichOperation=0)const;
  bool IsGoodForChainRuleFunction(std::string* outputWhichOperation=0)const;

  bool IsDifferentialOneFormOneVariable(Expression* outputDifferentialOfWhat=0, Expression* outputCoeffInFrontOfDifferential=0)const;
  bool IsKnownFunctionWithComplexRange(std::string* outputWhichOperation=0)const;
  bool IsArithmeticOperation(std::string* outputWhichOperation=0)const;
  bool IsCacheableExpression()const;
  bool IsBuiltInScalar()const;
  bool IsBuiltInType(std::string* outputWhichOperation=0)const;
  bool IsBuiltInType(int* outputWhichType)const;
  const Expression& operator[](int n)const;
  bool IsSequenceNElementS(int N=-2)const;
  bool IsError(std::string* outputErrorMessage=0)const;
  bool IsContext()const;
  bool NeedsParenthesisForBaseOfExponent()const;
  bool NeedsParenthesisForMultiplication()const;
  bool NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost()const;

  int GetExpressionTreeSize()const;
  template <class theType>
  bool ConvertToType(Expression& output)const;
  template <class theType>
  bool ConvertsToType(theType* whichElement=0)const;
  template <class theType>
  bool IsOfType(theType* whichElement=0)const
  { MacroRegisterFunctionWithName("Expression::IsOfType");
    if (this->owner==0)
      return false;
    if (!this->StartsWith(this->GetTypeOperation<theType>()))
      return false;
    if(this->children.size<2 || !this->GetLastChild().IsAtom())
      return false;
    if (whichElement==0)
      return true;
    *whichElement=this->GetValue<theType>();
    return true;
  }
  template <class theType>
  const theType& GetValue()const
  { return this->GetValueNonConst<theType>();
  }
  template <class theType>
  theType& GetValueNonConst()const;

  template<class theType>
  int GetTypeOperation()const;
  template<class theType>
  int AddObjectReturnIndex(const theType& inputValue)const;

  //note: the following always returns true:
  template <class theType>
  bool AssignValue(const theType& inputValue, Calculator& owner);
  //note: the following always returns true:
  template <class theType>
  bool AssignValueWithContext(const theType& inputValue, const Expression& theContext, Calculator& owner);
  template <class theType>
  bool AddChildValueOnTop(const theType& inputValue)
  { this->CheckInitialization();
    Expression tempE;
    tempE.AssignValue(inputValue, *this->owner);
    return this->AddChildOnTop(tempE);
  }
  template <class theType>
  bool AssignValueWithContextToChild(int childIndex, const theType& inputValue, const Expression& theContext, Calculator& owner)
  { Expression tempE;
    tempE.AssignValueWithContext(inputValue, theContext, owner);
    return this->SetChilD(childIndex, tempE);
  }
  bool SetContextAtLeastEqualTo(Expression& inputOutputMinContext);
  int GetNumContextVariables()const;
  bool RemoveContext();
  bool HasContext()const;
  bool HasNonEmptyContext()const;
  Expression GetContext()const;
  static bool MergeContexts(Expression& leftE, Expression& rightE);
  bool MergeContextsMyAruments(Expression& output)const;
  template <class theType>
  bool MergeContextsMyArumentsAndConvertThem(Expression& output)const;

  bool ContainsAsSubExpression
  (const Expression& input)const;
  bool ContainsAsSubExpression
  (int inputAtom)const;

  Expression ContextGetContextVariable(int variableIndex) const;
  int ContextGetIndexAmbientSSalg()const;
  void ContextGetFormatExpressions(FormatExpressions& output)const;
  int ContextGetNumContextVariables()const;
  static bool ContextMergeContexts(const Expression& leftContext, const Expression& rightContext, Expression& outputContext);
  template <class coefficient>
  bool ContextGetPolySubFromSuperContext(const Expression& largerContext, PolynomialSubstitution<coefficient>& output)const;
  template <class coefficient>
  bool ContextGetPolySubFromSuperContextNoFailure(const Expression& largerContext, PolynomialSubstitution<coefficient>& output)const;
  bool ContextGetPolyAndEWASubFromSuperContext
  (const Expression& largerContext, PolynomialSubstitution<Rational>& outputPolyPart, PolynomialSubstitution<Rational>& outputEWApart)const;
  bool ContextGetPolyAndEWASubFromSuperContextNoFailure
  (const Expression& largerContext, PolynomialSubstitution<Rational>& outputPolyPart, PolynomialSubstitution<Rational>& outputEWApart)const;
  Expression ContextGetPolynomialVariables()const;
  Expression ContextGetDifferentialOperatorVariables()const;
  Expression ContextGetContextType(int theType)const;
  bool ContextSetSSLieAlgebrA(int indexInOwners, Calculator& owner);
  bool ContextSetDiffOperatorVar(const Expression& thePolyVar, const Expression& theDiffOpVar);
  bool ContextMakeContextSSLieAlgebrA(int indexInOwners, Calculator& owner)
  { this->MakeEmptyContext(owner);
    return this->ContextSetSSLieAlgebrA(indexInOwners, owner);
  }
  SemisimpleLieAlgebra* GetAmbientSSAlgebraNonConstUseWithCaution()const;
  const SemisimpleLieAlgebra* GetAmbientSSAlgebra()const
  { return this->GetAmbientSSAlgebraNonConstUseWithCaution();
  }
  bool IsEqualToZero()const;
  bool IsMonEqualToZero()const;
  bool IsEqualToOne()const;
  bool IsEqualToMOne()const;
  bool MakeIdMatrixExpressions(int theDim, Calculator& inputBoss);
  void MakeMonomialGenVerma(const MonomialGeneralizedVerma<RationalFunctionOld>& inputMon, Calculator& newBoss);
  void MakeElementTensorsGeneralizedVermas(const ElementTensorsGeneralizedVermas<RationalFunctionOld>& inputMon, Calculator& newBoss);
  bool MakeAtom(int input, Calculator& newBoss)
  { this->reset(newBoss);
    this->theData=input;
    return true;
  }
  bool MakeAtom(const std::string& atomName, Calculator& newBoss);
  bool EvaluatesToVariableNonBound()const;
  Expression::FunctionAddress GetHandlerFunctionIamNonBoundVar();
  bool MakeIntegral(Calculator& theCommands, const Expression& theFunction, const Expression& theVariable);
  template<class coefficient>
  bool MakeSum(Calculator& theCommands, const MonomialCollection<Expression, coefficient>& theSum);
  bool MakeSum(Calculator& theCommands, const List<Expression>& theSum);
  bool MakeProducT(Calculator& owner, const List<Expression>& theMultiplicands);
  bool MakeProducT(Calculator& owner, const Expression& left, const Expression& right);
  void MakeFunction(Calculator& owner, const Expression& theFunction, const Expression& theArgument);
  int GetNumCols()const;
  bool MakeSequenceCommands(Calculator& owner, List<std::string>& inputKeys, List<Expression>& inputValues);
  bool MakeSequenceStatements(Calculator& owner, List<Expression>* inputStatements=0);
  bool MakeSequence(Calculator& owner, List<Expression>* inputSequence=0);
  bool MakeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right);
  bool MakeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex=2);
  bool MakeSqrt(Calculator& owner, const Expression& argument, const Rational& radicalSuperIndex=2);

  bool MakeXOXOdotsOX(Calculator& owner, int theOp, const List<Expression>& theOpands);
  bool MakeOXdotsX(Calculator& owner, int theOp, const List<Expression>& input);
  bool MakeOX(Calculator& owner, int theOp, const Expression& opArgument);
  bool Sequencefy();
  void Substitute(const Expression& input, Expression& output);
  void AssignXOXToChild(int childIndex, Calculator& owner, int theOp, const Expression& left, const Expression& right)
  { Expression tempE;
    tempE.MakeXOX(owner, theOp, left, right);
    this->SetChilD(childIndex, tempE);
  }
  bool ContextMakeContextWithOnePolyVar(Calculator& owner, const std::string& inputPolyVarName);
  bool ContextMakeContextWithOnePolyVar(Calculator& owner, const Expression& inputPolyVarE);
  bool MakeContextWithOnePolyVarOneDiffVar(Calculator& owner, const Expression& inputPolyVarE, const Expression& inputDiffVarE);
  bool MakeContextSSLieAlg(Calculator& owner, const SemisimpleLieAlgebra& theSSLiealg);
  bool MakeEmptyContext(Calculator& owner);
  std::string Lispify()const;
  bool ToStringData(std::string& output, FormatExpressions* theFormat=0)const;
  std::string ToStringSemiFull()const;
  std::string ToStringFull()const;
  std::string ToString(FormatExpressions* theFormat=0, Expression* startingExpression=0, Expression* Context=0)const;
  static unsigned int HashFunction(const Expression& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->HashFunctionRecursive(0, 1000);
  }
  int HashFunctionRecursive(int RecursionDepth, int MaxRecursionDepth)const
  { if (RecursionDepth>MaxRecursionDepth)
      return 0;
    int result=this->theData*SomeRandomPrimes[0];
    int numCycles=MathRoutines::Minimum(this->children.size, SomeRandomPrimesSize);
    for (int i=0; i<numCycles; i++)
      result+=(*this)[i].HashFunctionRecursive(RecursionDepth+1, MaxRecursionDepth)*SomeRandomPrimes[i];
    return result;
  }
  int GetIndexChild(const Expression& input)const
  { for (int i=0; i<this->children.size; i++)
      if (input==(*this)[i])
        return i;
    return -1;
  }
  Expression() :flagDeallocated(false)
  { this->reset();
  }
  Expression(int x) :flagDeallocated(false)
  { this->reset();
    this->theData=x;
  }
  const Expression& GetLastChild()const
  { return (*this)[this->children.size-1];
  }
  bool MakeError (const std::string& theError, Calculator& owner);
  Expression(const Expression& other):flagDeallocated(false)
  { this->operator=(other);
  }
  Expression(Calculator& inputBoss):flagDeallocated(false)
  { this->reset(inputBoss);
  }
  ~Expression()
  { this->flagDeallocated=true;
  }
  bool CheckInitialization()const;
  bool CheckInitializationRecursively()const;
  bool CheckConsistency()const;
  bool CheckConsistencyRecursively()const;
  bool IsIntegerFittingInInt(int* whichInteger=0)const;
  bool IsSmallInteger(int* whichInteger=0)const;
  bool IsInteger(LargeInt* whichInteger=0)const;
  bool IsRational(Rational* whichRational=0)const;
  bool IsConstantNumber()const;
  bool EvaluatesToDoubleInRange
  (const std::string& varName, double lowBound, double highBound, int numPoints,
   double* outputYmin=0, double* outputYmax=0, Vectors<double>* outputPoints=0)const
   ;
  bool EvaluatesToDouble(double* whichDouble=0)const;
  bool GetFreeVariables(HashedList<Expression>& outputAccumulateFreeVariables, bool excludeNamedConstants)const;
bool EvaluatesToDoubleUnderSubstitutions
(const HashedList<Expression>& knownEs, const List<double>& valuesKnownEs, double* whichDouble=0)const
  ;

  bool HasBoundVariables()const;
  bool IsMeltable(int* numResultingChildren=0)const;
  bool AreEqualExcludingChildren(const Expression& other) const
  { return this->owner==other.owner && this->theData==other.theData && this->children.size==other.children.size;
  }
  void operator/=(const Expression& other);
  void operator+=(const Expression& other);
  void operator-=(const Expression& other);
  void operator*=(const Expression& other);
//  Rational GetConstantTerm() const;
  bool operator==(int other)const;
  bool operator==(const Expression& other)const;
  bool operator==(const std::string& other)const;
  bool operator!=(const std::string& other)const
  { return !this->operator==(other);
  }

  bool operator!=(const Expression& other)const
  { return ! (*this==other);
  }
//  void operator=(const Expression& other);
  bool operator>(const Expression& other)const;
  bool GreaterThanNoCoeff(const Expression& other)const;
};

class Function
{
  public:
  Calculator* owner;
  Expression theArgumentTypes;
  std::string theDescription;
  std::string theExample;
  std::string calculatorIdentifier;
  std::string additionalIdentifier;
  int indexOperation;
  int indexAmongOperationHandlers;

  bool flagIsCompositeHandler;
  bool flagIsInner;
  bool flagMayActOnBoundVars;
  bool flagIamVisible;
  bool flagIsExperimental;
  bool flagDisabledByUser;
  Expression::FunctionAddress theFunction;

  std::string ToStringShort();
  std::string ToStringSummary();
  std::string ToStringFull();
  void operator =(const Function& other)
  { this->theArgumentTypes=other.theArgumentTypes;
    this->theDescription=other.theDescription;
    this->theExample=other.theExample;
    this->theFunction=other.theFunction;
    this->flagIsInner=other.flagIsInner;
    this->flagMayActOnBoundVars=other.flagMayActOnBoundVars;
    this->flagIamVisible=other.flagIamVisible;
    this->flagIsExperimental=other.flagIsExperimental;
    this->calculatorIdentifier=other.calculatorIdentifier;
    this->additionalIdentifier=other.additionalIdentifier;
    this->indexOperation=other.indexOperation;
    this->indexAmongOperationHandlers=other.indexAmongOperationHandlers;
    this->owner=other.owner;
    this->flagIsCompositeHandler=other.flagIsCompositeHandler;
  }
  bool operator==(const Function& other)const
  { return this->theArgumentTypes==other.theArgumentTypes &&
    this->theFunction==other.theFunction && this->flagIsInner==other.flagIsInner;
  }
  void reset(Calculator& owner)
  { this->theArgumentTypes.reset(owner);
    this->theFunction=0;
    this->flagIsInner=true;
  }
  bool inputFitsMyInnerType(const Expression& input);
  Function()
  { this->theFunction=0;
    this->indexAmongOperationHandlers=-1;
    this->indexOperation=-1;
    this->owner=0;
    this->flagIsCompositeHandler=false;
    this->flagDisabledByUser=false;
  }
  Function
  (Calculator& inputOwner, int inputIndexOperation, int inputIndexAmongOperationHandlers, const Expression::FunctionAddress& functionPointer, Expression* inputArgTypes, const std::string& description, const std::string& inputExample,
   bool inputflagIsInner, bool inputIsVisible=true, bool inputIsExperimental=false,
   bool inputflagMayActOnBoundVars=false)
  { this->owner=&inputOwner;
    this->indexOperation=inputIndexOperation;
    this->indexAmongOperationHandlers=inputIndexAmongOperationHandlers;
    this->theFunction=functionPointer;
    this->theDescription=description;
    this->theExample=inputExample;
    if (inputArgTypes!=0)
      this->theArgumentTypes=*inputArgTypes;
    this->flagIsInner=inputflagIsInner;
    this->flagIamVisible=inputIsVisible;
    this->flagMayActOnBoundVars=inputflagMayActOnBoundVars;
    this->flagIsExperimental=inputIsExperimental;
    this->flagIsCompositeHandler=false;
  }
  inline static unsigned int HashFunction(const Function& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return (unsigned int)(uintptr_t)this->theFunction;
  }
};

class BoundVariablesSubstitution
{
public:
  std::string ToString();
  HashedList<Expression> theBoundVariables;
  HashedList<Expression> variableImages;
  void reset()
  { this->theBoundVariables.Clear();
    this->variableImages.Clear();
  }
};

class SyntacticElement
{
  public:
  int controlIndex;
//  int IndexFirstChar;
//  int IndexLastCharPlusOne;
  std::string errorString;
  Expression theData;
  void operator=(const SyntacticElement& other)
  { this->controlIndex=other.controlIndex;
    this->theData=other.theData;
    this->errorString=other.errorString;
//    this->IndexFirstChar=other.IndexFirstChar;
//    this->IndexLastCharPlusOne=other.IndexLastCharPlusOne;
  }
  std::string ToStringHumanReadable(Calculator& theBoss, bool includeLispifiedExpressions)const;
  SyntacticElement()
  { this->controlIndex=0;//controlIndex=0 *MUST* point to the empty control sequence.
    this->errorString="";
//    this->IndexFirstChar=-1;
//    this->IndexLastCharPlusOne=-1;
  }
  SyntacticElement(const SyntacticElement& other)
  { this->operator=(other);
  }
};

class PlotObject
{
public:
  std::string thePlotString;
  std::string thePlotStringWithHtml;
  double xLow;
  double xHigh;
  double yLow;
  double yHigh;
  int colorRGB;
  Vectors<double> thePoints;
  Expression thePlotElement;
  std::string thePlotType;
  void ComputeYbounds();
  std::string GetPlotStringFromFunctionStringAndRanges
  (bool useHtml, const std::string& functionStringPostfixNotation, const std::string& functionStringCalculatorFormat, double lowerBound, double upperBound);
  void CreatePlotFunction
  (const Expression& inputE, const std::string& inputPostfixNotation, double inputLowerBound, double inputUpperBound,
   double inputYmin, double inputYmax, Vectors<double>* inputPoints, int* inputColorRGB=0);
  PlotObject();
  bool operator==(const PlotObject& other)const;
};

//the following class is meant to use to draw plots for calculus students.
class Plot
{ public:
  List<PlotObject> thePlots;
  double theLowerBoundAxes;
  double theUpperBoundAxes;
  double lowBoundY;
  double highBoundY;
  int DesiredHtmlHeightInPixels;
  int DesiredHtmlWidthInPixels;
  bool flagIncludeExtraHtmlDescriptions;
  std::string GetPlotHtml();
  std::string GetPlotStringAddLatexCommands(bool useHtml);
  Plot();
  void ComputeAxesAndBoundingBox();
  void AddFunctionPlotOnTop
  (const Expression& inputE, const std::string& inputPostfixNotation, double inputLowerBound,
   double inputUpperBound, double inputYmin, double inputYmax, Vectors<double>* inputPoints, int* colorRGB=0);
  void operator+=(const Plot& other);
  void operator+=(const PlotObject& other);
  bool operator==(const Plot& other)const
  { return this->thePlots==other.thePlots;
  }
};

class ObjectContainer
{ //Following are containers for data structures that are implemented in C++.
  //These objects are dynamically allocated and used by the calculator as requested
  //by various predefined function handlers.
public:
  HashedListReferences<ElementWeylGroup<WeylGroupData> > theWeylGroupElements;
  HashedListReferences<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> > theWeylGroupReps;
  HashedListReferences<VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> > theWeylGroupVirtualReps;
  ListReferences<ModuleSSalgebra<RationalFunctionOld> > theCategoryOmodules;
  HashedListReferences<SemisimpleLieAlgebra> theLieAlgebras;
  ListReferences<SltwoSubalgebras> theSltwoSAs;
  ListReferences<SemisimpleSubalgebras> theSSsubalgebras;
  HashedListReferences<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theTensorElts;
  HashedListReferences<Polynomial<Rational> > thePolys;
  HashedListReferences<Polynomial<AlgebraicNumber> > thePolysOverANs;
  HashedListReferences<ElementWeylAlgebra<Rational> > theWeylAlgebraElements;
  HashedListReferences<ElementUniversalEnveloping<RationalFunctionOld> > theUEs;
  HashedListReferences<RationalFunctionOld> theRFs;
  HashedListReferences<Rational> theRationals;
  HashedListReferences<charSSAlgMod<Rational> > theCharsSSLieAlgFD;
  HashedListReferences<double, MathRoutines::HashDouble> theDoubles;
  HashedListReferences<std::string, MathRoutines::hashString> theStrings;
  HashedListReferences<std::string, MathRoutines::hashString> ExpressionNotation;
  HashedListReferences<Expression> ExpressionWithNotation;
  HashedListReferences<LittelmannPath> theLSpaths;
  HashedListReferences<Matrix<Rational> > theMatRats;
  HashedListReferences<Matrix<AlgebraicNumber> > theMatsAlgebraic;
  HashedListReferences<MatrixTensor<Rational> > theMatTensorRats;
  HashedListReferences<Matrix<RationalFunctionOld> > theMatRFs;
  HashedListReferences<ElementZmodP> theEltsModP;
  HashedListReferences<Weight<Rational> > theWeights;
  HashedListReferences<Weight<Polynomial<Rational> > > theWeightsPoly;
  ListReferences<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational > > theHyperoctahedralReps;
  ListReferences<Plot> thePlots;
  AlgebraicClosureRationals theAlgebraicClosure;
  HashedList<AlgebraicNumber> theAlgebraicNumbers;
  HashedListReferences<ElementHyperoctahedralGroupR2> theElementsHyperOctGroup;
  ListReferences<HyperoctahedralGroupData> theHyperOctahedralGroups;
//  HashedList<DifferentialForm<Rational> > theDiffForm;
  HashedListReferences<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
  void reset();
  std::string ToString();
};

struct ExpressionTripleCrunchers
{
  int theOp;
  int leftType;
  int rightType;
  bool operator==(const ExpressionTripleCrunchers& other)const
  { return this->leftType==other.leftType && this->rightType==other.rightType && this->theOp==other.theOp;
  }
  void operator=(const ExpressionTripleCrunchers& other)
  { this->leftType=other.leftType;
    this->rightType=other.rightType;
    this->theOp=other.theOp;
  }
  ExpressionTripleCrunchers():theOp(-1), leftType(-1), rightType(-1){}
  ExpressionTripleCrunchers(int inputOp, int inputLeft, int inputRight): theOp(inputOp), leftType(inputLeft), rightType(inputRight)
  {}
  static unsigned int HashFunction(const ExpressionTripleCrunchers& input)
  { return (unsigned int) input.leftType*SomeRandomPrimes[0]+(unsigned int) input.rightType*SomeRandomPrimes[1]+(unsigned int) input.theOp*SomeRandomPrimes[2];
  }
};

class Calculator
{
  template<typename anyType>
  friend Calculator& operator << (Calculator& output, const anyType& any)
  { output.Comments << any;
    return output;
  }
public:
  //Operations parametrize the expression elements.
  //Operations are the labels of the atom nodes of the expression tree.
  HashedList<std::string, MathRoutines::hashString> theAtoms;
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
  HashedList<std::string, MathRoutines::hashString> operationsComposite;
  List<List<Function> > FunctionHandlers;
  List<List<Function> > operationsCompositeHandlers;
  HashedList<std::string, MathRoutines::hashString> namedRules;
  List<List<int> > namedRulesLocations;//for each named rule we store a list of triple of ints.
  //If first int is 0 then the named rule is a function handler.
  //If first int is 1 then the named rule is a composite operation handler.
  //Second int gives the index of the atom handled by the named rule.
  //Third int gives the index of the rule within the list of handlers for that atom.

//Calculator functions have as arguments two expressions passed by reference,
//const Expression& input and Expression& output. Calculator functions
//return bool. It is forbidden to pass the same object as input and output.
//If a calculator function returns false this
//means that the calculator failed to evaluate the
//function. If that is the case, the value of output is not specified and
//*MUST NOT* be used in the calling function.
//If a function returns true this means that output contains the result of the function.
//Note that the output of a function may be of type Error. Error results come, like any other
//result, with a true return from the function.
//-----------------------------------------------
//In addition, built-in functions are split into two flavors:
//inner functions (or just "functions")
// and outer functions (or "laws").
//The only difference between inner functions and outer functions is the
//way they are applied when the calculator reduces an expression.
//
//Suppose the calculator is reducing Expression X.
//1. Outer functions ("laws").
//1.1. Let X be expression whose first child is an atom equal to the name of the outer function.
//1.2  Call the outer function with input argument equal to X.
//1.3. If the outer function returns true but the output argument is identically equal to
//     X, nothing is done (the action of the outer function is ignored).
//1.4. If an outer function returns true and the output argument is different from X,
//     X is replaced by this output.
//2. Inner functions ("functions").
//2.1. Let X be expression whose first child is an atom equal to the name of the inner function. We define Argument as follows.
//2.1.1. If X has two children, Argument is set to the second child of X.
//2.1.2. If X does not have two children, Argument is set to be equal to the entire X.
//2.2. The inner function is called with input argument equal to Argument.
//2.3. If the inner function returns true, X is substituted with
//     the output argument of the inner function, else nothing is done.
//
//As explained above, the distinction between inner functions and outer functions
//is only practical. The notions of inner and outer functions do not apply to user-defined
//substitution rules entered via the calculator. User-defined substitution rules are
//processed like outer functions, with the
//major difference that even if their output coincides
//with their input the substitution is carried out, resulting in an infinite cycle.
//Here, by ``infinite cycle'' we either mean a 100% CPU run until the timeout& algebraic
//safety kicks in, or error interception with a ``detected substitution cycle'' or
//similar error message.
//
//----------------------------------------------------------


//control sequences parametrize the syntactical elements
  HashedList<std::string, MathRoutines::hashString> controlSequences;

  HashedList<ExpressionTripleCrunchers> theCruncherIds;
  List<Function> theCruncherS;

  HashedList<Expression> knownDoubleConstants;
  List<double> knownDoubleConstantValues;

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
  ///////////////////////////////////////////////////////////////////////////
  bool flagAbortComputationASAP;
  bool flagTimeLimitErrorDetected;
  bool flagFirstErrorEncountered;
  bool flagMaxRecursionErrorEncountered;
  bool flagMaxTransformationsErrorEncountered;
  bool flagNewContextNeeded;
  bool flagCurrentExpressionIsNonCacheable;

  bool flagLogSyntaxRules;
  bool flagUseLnInsteadOfLog;
  bool flagLogEvaluatioN;
  bool flagUseNumberColors;
  bool flagLogRules;
  bool flagLogCache;
  bool flagLogPatternMatching;
  bool flagLogFullTreeCrunching;
  bool flagProduceLatexLink;
  bool flagHideLHS;
  bool flagDisplayFullExpressionTree;
  bool flagUseFracInRationalLaTeX;
  bool flagUseHtml;
  bool flagDisplayContext;
  bool flagShowCalculatorInternalStatus;
  bool flagShowCalculatorExamples;
  bool flagDefaultRulesWereTamperedWith;

  bool flagWriteLatexPlots;

  bool flagNoApproximations;

  bool flagForkingProcessAllowed;
  ///////////////////////////////////////////////////////////////////////////
  int TotalNumPatternMatchedPerformed;
  int NumPredefinedAtoms;
  int numEmptyTokensStart;
  Expression theProgramExpression;
//  std::vector<std::stringstream> theLogs;
  int registerNumNonClosedBeginArray;
  int registerPositionAfterDecimalPoint;
  int counterInSyntacticSoup;
  List<SyntacticElement> syntacticSouP;
  List<SyntacticElement> syntacticStacK;

  List<SyntacticElement>* CurrrentSyntacticSouP;
  List<SyntacticElement>* CurrentSyntacticStacK;

  HashedList<Expression> cachedExpressions;
  List<Expression> imagesCachedExpressions;
  ////
  HashedList<Expression> EvaluatedExpressionsStack;
  List<HashedList<int, MathRoutines::IntUnsignIdentity> > NonBoundVariablesStack;
  List<HashedList<int, MathRoutines::IntUnsignIdentity> > BoundVariablesStack;

  Expression RuleStack;
  HashedList<Expression> cachedRuleStacks;

  HashedListReferences<Expression> theExpressionContainer;

  std::string syntaxErrors;
  List<std::string> evaluationErrors;
  std::string inputStringRawestOfTheRaw;
  std::string inputString;
  std::string outputString;
  std::string outputCommentsString;
  std::string parsingLog;
  ObjectContainer theObjectContainer;
  double StartTimeEvaluationInSecondS;

  char DefaultWeylLetter;
  int DefaultWeylRank;
  std::string javaScriptDisplayingIndicator;
  std::fstream theTestFile;
  std::string theTestFileName;
  int numOutputFileS;
  std::string userLabel;
  std::stringstream Comments;
  FormatExpressions formatVisibleStrings;

  List<std::string> SystemCommands;
  std::string ToString();
  std::string ElementToStringNonBoundVars();
  std::string ToStringOutputAndSpecials();
  std::string ToStringFunctionHandlers();
  //the purpose of the operator below is to save on typing when returning false with a comment.
  operator bool()const
  { return false;
  }
  std::string ToStringLinksToCalculator(const DynkinType& theType, FormatExpressions* theFormat=0);
  std::string ToStringLinksToCalculatorDirectlyFromHD(const DynkinType& theType, FormatExpressions* theFormat=0);
  //void GetOutputFolders(const DynkinType& theType, std::string& outputFolderPhysical, std::string& outputFolderDisplay, FormatExpressions& outputFormat);
  bool IsBoundVarInContext(int inputOp);
  bool IsNonBoundVarInContext(int inputOp);
  Function& GetFunctionHandlerFromNamedRule(const std::string& inputRuleName);
  bool CheckConsistencyAfterInitializationExpressionStackEmpty();
  //to make operations read only, we make operations private and return const pointer to it.
  inline const HashedList<std::string, MathRoutines::hashString>& GetOperations()
  { return this->theAtoms;
  }
  inline const HashedList<std::string, MathRoutines::hashString>& GetBuiltInTypes()
  { return this->builtInTypes;
  }
  const List<Function>* GetOperationCompositeHandlers(int theOp)
  { int theIndex=this->operationsComposite.GetIndex(this->GetOperations()[theOp]);
    if (theIndex==-1)
      return 0;
    return &this->operationsCompositeHandlers[theIndex];
  }
  SyntacticElement GetSyntacticElementEnd()
  { SyntacticElement result;
    result.controlIndex=this->controlSequences.GetIndex(";");
    return result;
  }
  bool DecreaseStackSetCharacterRangeS(int decrease)
  { if (decrease<=0)
      return true;
    if ((*this->CurrentSyntacticStacK).size-decrease<=0)
      crash << crash;
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-1].IndexLastCharPlusOne=
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].IndexLastCharPlusOne;
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-decrease);
    return true;
  }
  bool DecreaseStackExceptLast(int decrease)
  { if (decrease<=0)
      return true;
    if ((*this->CurrentSyntacticStacK).size-decrease<=0)
      crash << crash;
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-2].IndexLastCharPlusOne=
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].IndexLastCharPlusOne;
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-1]=
    *this->CurrentSyntacticStacK->LastObject();
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-decrease);
    return true;
  }
  std::string ToStringSyntacticStackHTMLTable();
  std::string ToStringSyntacticStackHumanReadable(bool includeLispifiedExpressions);
  std::string WriteDefaultLatexFileReturnHtmlLink(const std::string& fileContent, bool useLatexDviPSpsToPNG=false);
  std::string GetCalculatorLink(const std::string& input);
  bool isInterpretedAsEmptySpace(const std::string& input);
  bool isSeparatorFromTheLeftGeneral(const std::string& input);
  bool isSeparatorFromTheLeftForDefinition(const std::string& input);
  bool isSeparatorFromTheLeftForList(const std::string& input);
  bool isSeparatorFromTheLeftForListMatrixRow(const std::string& input);
  bool isSeparatorFromTheLeftForMatrixRow(const std::string& input);

  bool isSeparatorFromTheRightGeneral(const std::string& input);
  bool isSeparatorFromTheRightForDefinition(const std::string& input);
  bool isSeparatorFromTheRightForList(const std::string& input);
  bool isSeparatorFromTheRightForListMatrixRow(const std::string& input);
  bool isSeparatorFromTheRightForMatrixRow(const std::string& input);
  Expression::FunctionAddress GetInnerFunctionFromOp(int theOp, const Expression& left, const Expression& right);
  bool AllowsPowerInPreceding(const std::string& lookAhead)
  { return lookAhead!="{}";
  }
  bool RecursionDepthExceededHandleRoughly(const std::string& additionalErrorInfo="");

  bool AllowsLimitProcessInPreceding(const std::string& lookAhead);
  bool AllowsApplyFunctionInPreceding(const std::string& lookAhead);
  bool AllowsIfInPreceding(const std::string& lookAhead);
  bool AllowsOrInPreceding(const std::string& lookAhead);
  bool AllowsAndInPreceding(const std::string& lookAhead);
  bool AllowsPlusInPreceding(const std::string& lookAhead);
  bool AllowsTimesInPreceding(const std::string& lookAhead);
  bool AllowsTensorInPreceding(const std::string& lookAhead);
  bool AllowsDivideInPreceding(const std::string& lookAhead);
  bool PopTopSyntacticStack()
  { (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-1);
    return true;
  }
  bool ReplaceEXXEXEByEusingO(int theOp);
  bool ReplaceEXXEXEXByEXusingO(int theOp);
  bool ReplaceEOXbyEX();
  bool ReplaceEEByEusingO(int theControlIndex);
  bool ReplaceEEXByEXusingO(int theControlIndex);
  bool ReplaceOOEEXbyEXpowerLike();
  bool ReplaceEEByE();
  bool ReplaceEXEXByEX(int formatOptions);
  bool ReplaceSsSsXdotsXbySsXdotsX(int numDots);
  bool ReplaceEXdotsXbySsXdotsX(int numDots);
  bool ReplaceEXdotsXBySs(int numDots)
  { this->ReplaceEXdotsXbySsXdotsX(numDots);
    return this->DecreaseStackSetCharacterRangeS(numDots);
  }
  bool ReplaceOEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceEOByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceOXXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceSqrtEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceSqrtXEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceEOEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceXEEXByEXusingO(int inputOperation, int formatOptions=Expression::formatDefault);
  bool ReplaceECByC();
  bool ReplaceEXEBySequence(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceYXBySequenceX(int theControlIndex, int inputFormat=Expression::formatDefault)
  { return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, inputFormat, 1);
  }
  bool ReplaceXXXbyE(int inputFormat=Expression::formatDefault);
  bool ReplaceYBySequenceY(int theControlIndex, int inputFormat=Expression::formatDefault)
  { return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, inputFormat, 0);
  }
  bool ReplaceXXYBySequenceY(int theControlIndex, int inputFormat=Expression::formatDefault)
  { this->ReplaceYBySequenceY(theControlIndex, inputFormat);
    return this->ReplaceXXYByY();
  }
  bool ReplaceXXYXBySequenceYX(int theControlIndex, int inputFormat=Expression::formatDefault)
  { this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, inputFormat, 1);
    return this->ReplaceXXYXByYX();
  }
  bool ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int inputFormat=Expression::formatDefault, int numXs=0);
  bool ReplaceSequenceXEBySequence(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceSequenceUXEYBySequenceZY(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceCEByC();
  bool ReplaceCCByC();
  bool ReplaceEOEByE(int formatOptions=Expression::formatDefault)
  { return this->ReplaceEXEByEusingO((*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex, formatOptions);
  }
  bool ReplaceXEXEXByEusingO(int theOperation, int formatOptions=Expression::formatDefault);
  bool ReplaceEXEByEusingO(int theOperation, int formatOptions=Expression::formatDefault);
  bool ReplaceXYByConY(int theCon, int theFormat=Expression::formatDefault)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=theCon;
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=theFormat;
    return true;
  }
  bool ReplaceXYYByConYY(int theCon)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=theCon;
    return true;
  }
  bool ReplaceXYYYByConYYY(int theCon)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=theCon;
    return true;
  }
  bool ReplaceXXByCon(int theCon, int theFormat=Expression::formatDefault)
  { this->ReplaceXYByConY(theCon, theFormat);
    return this->DecreaseStackSetCharacterRangeS(1);
  }
  bool ReplaceXByCon(int theCon, int theFormat=Expression::formatDefault);
  bool ReplaceXByEusingO(int theOperation);
  bool ReplaceXByConCon(int con1, int con2, int format1=Expression::formatDefault, int format2=Expression::formatDefault);
  bool ReplaceXXXByCon(int theCon);
  bool ReplaceXXXByConCon(int con1, int con2, int inputFormat1=Expression::formatDefault, int inputFormat2=Expression::formatDefault);
  bool ReplaceXXXXXByCon(int theCon, int inputFormat=Expression::formatDefault);
  bool ReplaceXXXXXByConCon(int con1, int con2, int inputFormat1=Expression::formatDefault, int inputFormat2=Expression::formatDefault);
  bool ReplaceXXXXByConCon(int con1, int con2, int inputFormat1=Expression::formatDefault, int inputFormat2=Expression::formatDefault);
  bool ReplaceXXXXByCon(int con1, int inputFormat1=Expression::formatDefault);
  bool ReplaceXXYByY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-2);
    return true;
  }
  bool ReplaceXXYXByYX()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-2);
    return true;
  }
  bool ReplaceXYYXByYY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-2);
    return true;
  }
  bool ReplaceXYXByY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    this->DecreaseStackSetCharacterRangeS(2);
    return true;
  }
  bool ReplaceXYByY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-1);
    return true;
  }
  bool ReplaceXEXByE(int inputFormat=Expression::formatDefault);
  bool ReplaceAXbyEX();
  bool ReplaceXXByEEmptySequence();
  bool ReplaCeOXbyEX(int inputFormat=Expression::formatDefault);
  bool ReplaceXXbyEX(int inputFormat=Expression::formatDefault);
  bool ReplaceXEXByEcontainingOE(int inputOpIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceXXByEmptyString();
  bool ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence(int inputFormat=Expression::formatDefault);
  bool ReplaceXXbyE(int inputFormat=Expression::formatDefault);
  bool ReplaceIntIntBy10IntPlusInt();
  bool GetMatrixExpressions(const Expression& input, Matrix<Expression>& output, int desiredNumRows=-1, int desiredNumCols=-1);
  bool GetMatrixExpressionsFromArguments(const Expression& input, Matrix<Expression>& output, int desiredNumRows=-1, int desiredNumCols=-1);
  void MakeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output);
  bool ReplaceXXVXdotsXbyE_BOUND_XdotsX(int numXs);
  bool ReplaceVXdotsXbyE_NONBOUND_XdotsX(int numXs);
  int GetOperationIndexFromControlIndex(int controlIndex);
  int GetExpressionIndex();
  SyntacticElement GetEmptySyntacticElement();
  bool ApplyOneRule();
  void resetStack()
  { SyntacticElement emptyElement=this->GetEmptySyntacticElement();
    (*this->CurrentSyntacticStacK).initFillInObject(this->numEmptyTokensStart, emptyElement);
  }
  int conError()
  { return this->controlSequences.GetIndexIMustContainTheObject("Error");
  }
  int conExpression()
  { return this->controlSequences.GetIndexIMustContainTheObject("Expression");
  }
  int conVariable()
  { return this->controlSequences.GetIndexIMustContainTheObject("Variable");
  }
  int conBindVariable()
  { return this->controlSequences.GetIndexIMustContainTheObject("{{}}");
  }
  int conInteger()
  { return this->controlSequences.GetIndexIMustContainTheObject("Integer");
  }
  int conEqualEqual()
  { return this->controlSequences.GetIndexIMustContainTheObject("==");
  }
  int conApplyFunction()
  { return this->controlSequences.GetIndexIMustContainTheObject("{}");
  }
  int conTimes()
  { return this->controlSequences.GetIndexIMustContainTheObject("*");
  }
  int conEndStatement()
  { return this->controlSequences.GetIndexIMustContainTheObject(";");
  }
  int conDefineConditional()
  { return this->controlSequences.GetIndexIMustContainTheObject("if=");
  }
  int conComma()
  { return this->controlSequences.GetIndexIMustContainTheObject(",");
  }
  int conDefine()
  { return this->controlSequences.GetIndexIMustContainTheObject("=");
  }
  int conIsDenotedBy()
  { return this->controlSequences.GetIndexIMustContainTheObject("=:");
  }
  int conLisT()
  { return this->controlSequences.GetIndexIMustContainTheObject("");
  }
  int conSequenceStatements()
  { return this->controlSequences.GetIndexIMustContainTheObject("SequenceStatements");
  }
  int conSequence()
  { return this->controlSequences.GetIndexIMustContainTheObject("Sequence");
  }
  int conSequenceNoRepetition()
  { return this->controlSequences.GetIndexIMustContainTheObject("SequenceNoRepetition");
  }
  int conSequenceMatrixRow()
  { return this->controlSequences.GetIndexIMustContainTheObject("SequenceMatrixRows");
  }
  int conMatrixRow()
  { return this->controlSequences.GetIndexIMustContainTheObject("MatrixRow");
  }
  int conMatrixSeparator()
  { return this->controlSequences.GetIndexIMustContainTheObject("MatrixSeparator");
  }
  int conMatrixRowSeparator()
  { return this->controlSequences.GetIndexIMustContainTheObject("MatrixRowSeparator");
  }
  int conLieBracket()
  { return this->controlSequences.GetIndexIMustContainTheObject("[]");
  }
  int conOpenBracket()
  { return this->controlSequences.GetIndexIMustContainTheObject("(");
  }
  int conCloseBracket()
  { return this->controlSequences.GetIndexIMustContainTheObject(")");
  }
  int conEndProgram()
  { return this->controlSequences.GetIndexIMustContainTheObject("EndProgram");
  }
  int opEltZmodP()
  { return this->theAtoms.GetIndexIMustContainTheObject("EltZmodP");
  }
  int opApplyFunction()
  { return this->theAtoms.GetIndexIMustContainTheObject("{}");
  }
  int opIsDenotedBy()
  { return this->theAtoms.GetIndexIMustContainTheObject("=:");
  }
  int opDefine()
  { return this->theAtoms.GetIndexIMustContainTheObject("=");
  }
  int opRulesChanged()
  { return this->theAtoms.GetIndexIMustContainTheObject("RulesChanged");
  }
  int opTurnOnRules()
  { return this->theAtoms.GetIndexIMustContainTheObject("TurnOnRules");
  }
  int opTurnOffRules()
  { return this->theAtoms.GetIndexIMustContainTheObject("TurnOffRules");
  }
  int opSqrt()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\sqrt");
  }
  int opIf()
  { return this->theAtoms.GetIndexIMustContainTheObject("if");
  }
  int opDefineConditional()
  { return this->theAtoms.GetIndexIMustContainTheObject("if=");
  }
  int opThePower()
  { return this->theAtoms.GetIndexIMustContainTheObject("^");
  }
  int opEqualEqual()
  { return this->theAtoms.GetIndexIMustContainTheObject("==");
  }
  int opGreaterThan()
  { return this->theAtoms.GetIndexIMustContainTheObject(">");
  }
  int opLessThan()
  { return this->theAtoms.GetIndexIMustContainTheObject("<");
  }
  int opWeightLieAlg()
  { return this->theAtoms.GetIndexIMustContainTheObject("weightLieAlg");
  }
  int opWeightLieAlgPoly()
  { return this->theAtoms.GetIndexIMustContainTheObject("weightLieAlgPoly");
  }
  int opError()
  { return this->theAtoms.GetIndexIMustContainTheObject("Error");
  }
  int opLisT()
  { return this->theAtoms.GetIndexIMustContainTheObject("");
  }
  int opMonomialCollection()
  { return this->theAtoms.GetIndexIMustContainTheObject("MonomialCollection");
  }
  int opMonomialPoly()
  { return this->theAtoms.GetIndexIMustContainTheObject("MonomialPoly");
  }
  int opCalculusPlot()
  { return this->theAtoms.GetIndexIMustContainTheObject("CalculusPlot");
  }
  int opSequence()
  { return this->theAtoms.GetIndexIMustContainTheObject("Sequence");
  }
  int opQuote()
  { return this->theAtoms.GetIndexIMustContainTheObject("\"");
  }
  int opMelt()
  { return this->theAtoms.GetIndexIMustContainTheObject("Melt");
  }
  int opRational()
  { return this->theAtoms.GetIndexIMustContainTheObject("Rational");
  }
  int opElementHyperOctahedral()
  { return this->theAtoms.GetIndexIMustContainTheObject("ElementHyperoctahedral");
  }
  int opDouble()
  { return this->theAtoms.GetIndexIMustContainTheObject("Double");
  }
  int opAlgNumber()
  { return this->theAtoms.GetIndexIMustContainTheObject("AlgebraicNumber");
  }
  int opElementWeylAlgebra()
  { return this->theAtoms.GetIndexIMustContainTheObject("ElementWeylAlgebra");
  }
  int opPoly()
  { return this->theAtoms.GetIndexIMustContainTheObject("PolynomialRational");
  }
  int opPolyOverANs()
  { return this->theAtoms.GetIndexIMustContainTheObject("PolynomialOverANs");
  }
  int opRationalFunction()
  { return this->theAtoms.GetIndexIMustContainTheObject("RationalFunction");
  }
  int opDifferentiate()
  { return this->theAtoms.GetIndexIMustContainTheObject("Differentiate");
  }
  int opDifferential()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\diff");
  }
  int opIntegral()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\int");
  }
  int opMatAlgebraic()
  { return this->theAtoms.GetIndexIMustContainTheObject("MatrixAlgebraic");
  }
  int opMatRat()
  { return this->theAtoms.GetIndexIMustContainTheObject("Matrix_Rational");
  }
  int opMatTensorRat()
  { return this->theAtoms.GetIndexIMustContainTheObject("MatrixTensorRational");
  }
  int opWeylGroupRep()
  { return this->theAtoms.GetIndexIMustContainTheObject("WeylGroupRep");
  }
  int opMatRF()
  { return this->theAtoms.GetIndexIMustContainTheObject("MatrixRF");
  }
  int opFreeze()
  { return this->theAtoms.GetIndexIMustContainTheObject("Freeze");
  }
  int opString()
  { return this->theAtoms.GetIndexIMustContainTheObject("string");
  }
  int opElementUEoverRF()
  { return this->theAtoms.GetIndexIMustContainTheObject("ElementUEoverRF");
  }
  int opHyperoctahedralGroupRep()
  { return this->theAtoms.GetIndexIMustContainTheObject("HyperoctahedralGroupRepresentation");
  }
  int opWeylGroupVirtualRep()
  { return this->theAtoms.GetIndexIMustContainTheObject("WeylGroupVirtualRep");
  }
  int opElementTensorGVM()
  { return this->theAtoms.GetIndexIMustContainTheObject("ElementTensorGVM");
  }
  int opCharSSAlgMod()
  { return this->theAtoms.GetIndexIMustContainTheObject("CharSSAlgMod");
  }
  int opSSLieAlg()
  { return this->theAtoms.GetIndexIMustContainTheObject("SemisimpleLieAlg");
  }
  int opSemisimpleSubalgebras()
  { return this->theAtoms.GetIndexIMustContainTheObject("SemisimpleSubalgebras");
  }
  int opCandidateSSsubalgebra()
  { return this->theAtoms.GetIndexIMustContainTheObject("CandidateSSsubalgebra");
  }
  int opInfinity()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\infty");
  }
  int opPi()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\pi");
  }
  int opE()
  { return this->theAtoms.GetIndexIMustContainTheObject("e");
  }
  int opLogBase()
  { return this->theAtoms.GetIndexIMustContainTheObject("logBase");
  }
  int opLog()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\log");
  }
  int opSin()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\sin");
  }
  int opFactorial()
  { return this->theAtoms.GetIndexIMustContainTheObject("!");
  }
  int opLimitProcess()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\to");
  }
  int opLimit()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\lim");
  }
  int opCos()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\cos");
  }
  int opArcTan()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\arctan");
  }
  int opArcSin()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\arcsin");
  }
  int opArcCos()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\arccos");
  }
  int opTan()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\tan");
  }
  int opCot()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\cot");
  }
  int opSec()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\sec");
  }
  int opCsc()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\csc");
  }
  int opLittelmannPath()
  { return this->theAtoms.GetIndexIMustContainTheObject("LittelmannPath");
  }
  int opLRO()
  { return this->theAtoms.GetIndexIMustContainTheObject("LRO");
  }
  int opWeylGroup()
  { return this->theAtoms.GetIndexIMustContainTheObject("WeylGroup");
  }
  int opUnion()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\cup");
  }
  int opIntersection()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\cap");
  }
  int opPolynomialVariables()
  { return this->theAtoms.GetIndexIMustContainTheObject("PolyVars");
  }
  int opWeylAlgebraVariables()
  { return this->theAtoms.GetIndexIMustContainTheObject("DiffOpVars");
  }
  int opContexT()
  { return this->theAtoms.GetIndexIMustContainTheObject("Context");
  }
  int opWeylGroupElement()
  { return this->theAtoms.GetIndexIMustContainTheObject("ElementWeylGroup");
  }
  int opEndStatement()
  { return this->theAtoms.GetIndexIMustContainTheObject(";");
  }
  int opUnionNoRepetition()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\sqcup");
  }
  int opBind()
  { return this->theAtoms.GetIndexIMustContainTheObject("Bind");
  }
  int opPlus()
  { return this->theAtoms.GetIndexIMustContainTheObject("+");
  }
  int opMod()
  { return this->theAtoms.GetIndexIMustContainTheObject("mod");
  }
  int opMinus()
  { return this->theAtoms.GetIndexIMustContainTheObject("-");
  }
  int opTimes()
  { return this->theAtoms.GetIndexIMustContainTheObject("*");
  }
  int opCrossProduct()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\times");
  }
  int opTensor()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\otimes");
  }
  int opChoose()
  { return this->theAtoms.GetIndexIMustContainTheObject("\\choose");
  }
  int opLieBracket()
  { return this->theAtoms.GetIndexIMustContainTheObject("[]");
  }
  int opDivide()
  { return this->theAtoms.GetIndexIMustContainTheObject("/");
  }
  bool AppendOpandsReturnTrueIfOrderNonCanonical(const Expression& input, List<Expression>& output, int theOp);
  bool AppendMultiplicandsReturnTrueIfOrderNonCanonical(Expression& theExpression, List<Expression>& output, int RecursionDepth, int MaxRecursionDepth)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opTimes());
  }
  bool AppendSummandsReturnTrueIfOrderNonCanonical(const Expression& theExpression, List<Expression>& output)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus());
  }
  void SpecializeBoundVars(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs);
  Expression* PatternMatch
  (const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, const Expression* condition=0,
   std::stringstream* theLog=0, bool logAttempts=false);
  bool ProcessOneExpressionOnePatternOneSub
  (const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, std::stringstream* theLog=0, bool logAttempts=false);
  static void CheckInputNotSameAsOutput(const Expression& input, const Expression& output)
  { if (&input==&output)
      crash << "This is a programming error: the input expression, equal to " << input.ToString() << " has the same address as the output expression. "
      << crash;
  }
//  bool OrderMultiplicationTreeProperly(int commandIndex, Expression& theExpression);
  template <class theType>
  bool ConvertToTypeUsingFunction(Expression::FunctionAddress theFun, const Expression& input, Expression& output)
  { MacroRegisterFunctionWithName("Calculator::ConvertToTypeUsingFunction");
    if (input.IsOfType<theType>())
    { output=input;
      return true;
    }
    if (theFun==0)
      return false;
    // Todor: "Cannot convert 'std::basic_ostream<char>' to 'bool' in return. this stanza was
    // return this->Comments << "<hr>Conversion function failed on " << input.ToString() << ". ";
    // I assume that the intention was to return false
    if (!theFun(*this, input, output))
    { this->Comments << "<hr>Conversion function failed on " << input.ToString() << ". ";
      return false;
    }
    return output.IsOfType<theType>();
  }
  template <class theType>
  bool CallConversionFunctionReturnsNonConstUseCarefully(Expression::FunctionAddress theFun, const Expression& input, theType*& outputData)
  { MacroRegisterFunctionWithName("Calculator::CallConversionFunctionReturnsNonConstUseCarefully");
    Expression tempE;
    if (!this->ConvertToTypeUsingFunction<theType>(theFun, input, tempE))
      return false;
    outputData=&tempE.GetValueNonConst<theType>();
    return true;
  }
  bool CallCalculatorFunction(Expression::FunctionAddress theFun, const Expression& input, Expression& output)
  { if (&input==&output)
    { Expression inputCopy=input;
      return this->CallCalculatorFunction(theFun, inputCopy, output);
    }
    if (!theFun(*this, input, output))
      return false;
    return !output.IsError();
  }
  bool ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions, std::stringstream* theLog=0);
  static bool innerReverseOrdeR(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerReverseOrderRecursively(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialWithEWA(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerEWAorPoly(theCommands, input, output, true);
  }
  static bool innerElementWeylAlgebra(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerEWAorPoly(theCommands, input, output, false);
  }
  static bool innerEWAorPoly(Calculator& theCommands, const Expression& input, Expression& output, bool assignPoly);
  static bool innerOperationBinary(Calculator& theCommands, const Expression& input, Expression& output, int theOp);
  static bool innerInterpolatePoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTimes(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerOperationBinary(theCommands, input, output, theCommands.opTimes());
  }
  bool ReadTestStrings(HashedList<std::string, MathRoutines::hashString>& outputCommands, List<std::string>& outputResults);
  bool WriteTestStrings(List<std::string>& inputCommands, List<std::string>& inputResults);
  static bool innerAutomatedTest(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAutomatedTestSetKnownGoodCopy(Calculator& theCommands, const Expression& input, Expression& output);
  int GetNumBuiltInFunctions();
  void AutomatedTestRun
  (List<std::string>& outputCommandStrings, List<std::string>& outputResultsWithInit, List<std::string>& outputResultsNoInit);
  static bool innerTranspose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetElementWeylGroup(Calculator& theCommands, const Expression& input, Expression& output);
  static bool EvaluateCarryOutActionSSAlgebraOnGeneralizedVermaModule(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerStandardFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerPlus(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerPowerRaiseToFirst(Calculator& theCommands, const Expression& input, Expression& output);
  static bool CollectCoefficientsPowersVar
  (const Expression& input, const Expression& theVariable,
   VectorSparse<Expression>& outputPositionIiscoeffXtoIth)
;
  bool CollectOpands
  (const Expression& input, int theOp, List<Expression>& outputOpands)
  ;
  static bool CollectSummands(Calculator& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum);
  static bool outerTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool StandardIsDenotedBy(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerMinus(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerGreaterThan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerLessThan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCombineFractions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCheckRule(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociate(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSubZeroDivAnythingWithZero(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCancelMultiplicativeInverse(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDistributeExponent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAssociateExponentExponent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociateTimesDivision(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAtoXtimesAtoYequalsAtoXplusY(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerExtractBaseMultiplication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerMeltBrackets(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyByOne(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDistribute(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp);
  static bool outerTimesToFunctionApplication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDistributeTimes(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.outerDistribute(theCommands, input, output, theCommands.opPlus(), theCommands.opTimes());
  }
  static bool outerLeftDistributeBracketIsOnTheLeft(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp);
  static bool outerRightDistributeBracketIsOnTheRight(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp);
  static bool EvaluateIf(Calculator& theCommands, const Expression& input, Expression& output);
  template<class theType>
  bool GetMatriXFromArguments
  (const Expression& theExpression, Matrix<theType>& outputMat, Expression* inputOutputStartingContext=0, int targetNumColsNonMandatory=-1, Expression::FunctionAddress conversionFunction=0)
  { Expression tempE=theExpression;
    if (tempE.IsLisT())
      tempE.SetChildAtomValue(0, this->opSequence());
    return this->GetMatrix(tempE, outputMat, inputOutputStartingContext, targetNumColsNonMandatory, conversionFunction);
  }
  template<class theType>
  bool GetMatrix
  (const Expression& input, Matrix<theType>& outputMat, Expression* inputOutputStartingContext=0,int targetNumColsNonMandatory=-1, Expression::FunctionAddress conversionFunction=0);
  template <class theType>
  bool GetVectorFromFunctionArguments
  (const Expression& input, Vector<theType>& output, Expression* inputOutputStartingContext=0, int targetDimNonMandatory=-1, Expression::FunctionAddress conversionFunction=0)
  { Expression tempE=input;
    if (tempE.IsLisT())
    { //stOutput << "<hr>tempE: " << tempE.Lispify();
      tempE.SetChildAtomValue(0, this->opSequence());
      //stOutput << "<br>tempE after change: " << tempE.Lispify();
    }
    return this->GetVectoR(tempE, output, inputOutputStartingContext, targetDimNonMandatory, conversionFunction);
  }
  bool GetSumProductsExpressions(const Expression& inputSum, List<List<Expression> >& outputSumMultiplicands);
  bool GetVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory=-1);
  bool ConvertExpressionsToCommonContext(List<Expression>& inputOutputEs, Expression* inputOutputStartingContext=0);
  bool GetVectoRInt(const Expression& input, List<int>& output);
  template <class theType>
  bool GetVectoR
  (const Expression& input, Vector<theType>& output, Expression* inputOutputStartingContext=0, int targetDimNonMandatory=-1,
   Expression::FunctionAddress conversionFunction=0);
  static bool innerFunctionToMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGenerateMultiplicativelyClosedSet(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeterminantPolynomial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInvertMatrixVerbose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSuffixNotationForPostScript(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFreudenthalEval(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFreudenthalFull(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerKillingForm(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGCDOrLCM(Calculator& theCommands, const Expression& input, Expression& output, bool doGCD);
  static bool innerLCM(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGCDOrLCM(theCommands, input, output, false);
  }
  static bool innerGCD(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGCDOrLCM(theCommands, input, output, true);
  }
  bool GetFunctionFromDiffOneForm
  (const Expression& input, Expression& outputFunction, Expression& outputVariable);
  bool GetListPolysVariableLabelsInLex(const Expression& input, Vector<Polynomial<Rational> >& output, Expression& outputContext);
  static bool innerPolynomialDivisionRemainder(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerbose
  (Calculator& theCommands, const Expression& input, Expression& output, List<MonomialP>::OrderLeftGreaterThanRight theMonOrder);
  static bool innerPolynomialDivisionVerboseGrLex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerboseGrLexRev(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerboseLexRev(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerboseLex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintAllPartitions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintB3G2branchingTableCharsOnly(Calculator& theCommands, const Expression& input, Expression& output);
  bool innerPrintB3G2branchingIntermediate
  (Calculator& theCommands, const Expression& input, Expression& output, Vectors<RationalFunctionOld>& theHWs, branchingData& theG2B3Data, Expression& theContext);
  static bool innerPrintB3G2branchingTable(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintB3G2branchingTableCommon
  (Calculator& theCommands, const Expression& input, Expression& output, Vectors<RationalFunctionOld>& outputHWs, branchingData& theG2B3Data, Expression& theContext);
  static bool innerGetChevGen(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetCartanGen(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2CharsOnly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSLieAlgebraShort(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerPrintSSLieAlgebra(theCommands, input, output, false);
  }
  static bool innerPrintSSLieAlgebraVerbose(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerPrintSSLieAlgebra(theCommands, input, output, true);
  }
  static bool innerPrintSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, bool Verbose);
  static bool innerRootSubsystem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConesIntersect(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPerturbSplittingNormal(Calculator& theCommands, const Expression& input, Expression& output);
  template<class coefficient>
  bool GetTypeWeight
(Calculator& theCommands, const Expression& input, Vector<coefficient>& outputWeightSimpleCoords,
 Expression& outputWeightContext, SemisimpleLieAlgebra*& ambientSSalgebra,
 Expression::FunctionAddress ConversionFun)
;
  template<class coefficient>
  bool GetTypeHighestWeightParabolic
  (Calculator& theCommands, const Expression& input, Expression& output, Vector<coefficient>& outputWeightHWFundcoords, Selection& outputInducingSel,
   Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra, Expression::FunctionAddress ConversionFun);
  static bool innerGroebnerGrLex(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGroebner(theCommands, input, output, true);
  }
  static bool innerGroebnerLex(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGroebner(theCommands, input, output, false);
  }
  static bool innerGroebnerRevLex(Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGroebner(theCommands, input, output, false, true);
  }
  static bool innerGroebnerModZpLex(Calculator& theCommands, const Expression& input, Expression& output, bool useGr)
  { return theCommands.innerGroebner(theCommands, input, output, false, false, true);
  }
  static bool innerGroebner(Calculator& theCommands, const Expression& input, Expression& output, bool useGr, bool useRevLex=false, bool useModZp=false);
  static bool innerKLcoeffs(Calculator& theCommands, const Expression& input, Expression& output);
//  static bool innerSSLieAlgebra
//  (Calculator& theCommands, const Expression& input, Expression& output)
//  { return theCommands.innerSSLieAlgebra(theCommands, input, output, false);
//  }
  static bool innerSplitFDpartB3overG2old(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2inner(Calculator& theCommands, branchingData& theG2B3Data, Expression& output);
  static bool innerLittelmannOperator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAnimateLittelmannPaths(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSqrt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFactorPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLSPath(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestMonomialBaseConjecture(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerJacobiSymbol(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHWVCommon
  (Calculator& theCommands, Expression& output, Vector<RationalFunctionOld>& highestWeightFundCoords, Selection& selectionParSel,
   Expression& hwContext, SemisimpleLieAlgebra* owner, bool Verbose=true);
  bool innerWriteGenVermaModAsDiffOperatorInner
  (Calculator& theCommands, const Expression& input, Expression& output, Vectors<Polynomial<Rational> >& theHws, Expression& hwContext,
   Selection& selInducing, SemisimpleLieAlgebra* owner, bool AllGenerators, std::string* xLetter,
   std::string* partialLetter, std::string* exponentVariableLetter,
   bool useNilWeight, bool ascending);
  template<class coefficient>
  static bool TypeHighestWeightParabolic
  (Calculator& theCommands, const Expression& input, Expression& output, Vector<coefficient>& outputWeight, Selection& outputInducingSel, Expression* outputContext=0);
  static bool innerPrintZnEnumeration(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNot(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerZmodP(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAttemptExtendingEtoHEFwithHinCartan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAdCommonEigenSpaces(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEmbedG2inB3(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWriteGenVermaModAsDiffOperators
  (Calculator& theCommands, const Expression& input, Expression& output,
   bool AllGenerators, bool useNilWeight, bool ascending);
  static bool innerWriteGenVermaModAsDiffOperatorsGeneratorOrder
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerWriteGenVermaModAsDiffOperators(theCommands, input, output, false, false, true);
  }
  static bool innerWriteGenVermaModAsDiffOperatorsNilOrderDescending
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerWriteGenVermaModAsDiffOperators(theCommands, input, output, false, true, false);
  }
  static bool innerWriteGenVermaModAsDiffOperatorsAllGensNilOrderDescending
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerWriteGenVermaModAsDiffOperators(theCommands, input, output, true, false, false);
  }
  static bool innerCasimir(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetLinksToSimpleLieAlgerbas(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCharacterSSLieAlgFD(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebras
  (Calculator& theCommands, const Expression& input, Expression& output, bool doForceRecompute, bool doAttemptToSolveSystems,
   bool doComputePairingTable, bool doComputeModuleDecomposition, bool doComputeNilradicals, bool doAdjustCentralizers);
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
  void AddKnownDoubleConstant(const std::string& theConstantName, double theValue);
  void AddOperationComposite
  (const std::string& theOpName, Expression::FunctionAddress handler,
   const std::string& opArgumentListIgnoredForTheTimeBeing, const std::string& opDescription,
   const std::string& opExample, bool isInner, bool visible, bool experimental,
   const std::string& inputAdditionalIdentifier, const std::string& inputCalculatorIdentifier)
   ;
  void AddOperationBinaryInnerHandlerWithTypes
  (const std::string& theOpName, Expression::FunctionAddress innerHandler, int leftType, int rightType, const std::string& opDescription,
   const std::string& opExample, bool visible=true, bool experimental=false, const std::string& inputAdditionalIdentifier="");
  void AddOperationHandler
  (const std::string& theOpName, Expression::FunctionAddress handler, const std::string& opArgumentListIgnoredForTheTimeBeing, const std::string& opDescription,
   const std::string& opExample, bool isInner, bool visible, bool experimental,
   const std::string& inputAdditionalIdentifier, const std::string& inputCalculatorIdentifier)
   ;
  void AddOperationInnerHandler
  (const std::string& theOpName, Expression::FunctionAddress innerHandler, const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample, bool visible=true, bool experimental=false,
   const std::string& inputAdditionalIdentifier="",
   const std::string& inputCalculatorIdentifier=""
  )
  { this->AddOperationHandler
    (theOpName, innerHandler, opArgumentListIgnoredForTheTimeBeing, opDescription, opExample, true,
     visible, experimental, inputAdditionalIdentifier, inputCalculatorIdentifier);
  }
  void AddOperationOuterHandler
  (const std::string& theOpName, Expression::FunctionAddress outerHandler, const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample, bool visible=true, bool experimental=false,
   const std::string& inputAdditionalIdentifier="",
   const std::string& inputCalculatorIdentifier=""
   )
  { this->AddOperationHandler
    (theOpName, outerHandler, opArgumentListIgnoredForTheTimeBeing, opDescription, opExample, false,
     visible, experimental, inputAdditionalIdentifier, inputCalculatorIdentifier);
  }
  void init();
  void reset();
  void initAtomsThatFreezeArguments();
  void initAtomsNonCacheable();
  void initAtomsThatAllowCommutingOfArguments();
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
  bool ExtractExpressions(Expression& outputExpression, std::string* outputErrors);
  void EvaluateCommands();
  static bool EvaluateExpression
  (Calculator& theCommands, const Expression& input, Expression& output)
  ;
  void Evaluate(const std::string& theInput);
  bool ParseAndExtractExpressions
  (const std::string& theInputString, Expression& outputExp, List<SyntacticElement>& outputSynSoup, List<SyntacticElement>& outputSynStack, std::string* outputSynErrors);
  bool isLeftSeparator(unsigned char c);
  bool isRightSeparator(unsigned char c);
  void ParseFillDictionary(const std::string& input);
};

class CalculatorConversions
{
public:
  ////////////////////Conversion from expression tree/////////////////////////////////
  //conversions from expression tree to type
  static bool innerElementSemisimpleLieAlgebraRationalCoeffs
  (Calculator& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<Rational>& output,
   SemisimpleLieAlgebra& owner);
  static bool innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers
  (Calculator& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
   SemisimpleLieAlgebra& owner);
  static bool innerDynkinType(Calculator& theCommands, const Expression& input, DynkinType& output);
  static bool innerDynkinSimpleType(Calculator& theCommands, const Expression& input, DynkinSimpleType& output);
  static bool innerSlTwoSubalgebraPrecomputed(Calculator& theCommands, const Expression& input, slTwoSubalgebra& output);
  static bool innerLoadFromObject(Calculator& theCommands, const Expression& input, RationalFunctionOld& output);
  static bool innerAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output);
  //conversions from expression tree to expression containing type
  template <class coefficient>
  static bool innerPolynomial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRationalFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerElementUE
  (Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra& inputOwner);

  static bool innerMakeMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixAlgebraic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixRationalFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMatrixRationalTensorForm(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLoadFileIntoString(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMakeElementHyperOctahedral(Calculator& theCommands, const Expression& input, Expression& output);
  ////////////////////Conversion to expression tree/////////////////////////////////////
  //converstion from type to expression tree.
  template <class coefficient>
  static bool innerExpressionFromPoly
  (Calculator& theCommands, const Polynomial<coefficient>& input, Expression& output,
   Expression* inputContext=0);
  static bool innerExpressionFromRF
  (Calculator& theCommands, const RationalFunctionOld& input, Expression& output,
   Expression* inputContext=0);

  static bool innerLoadKey(Calculator& theCommands, const Expression& inputStatementList, const std::string& inputKey, Expression& output);
  static bool innerStoreSemisimpleSubalgebras
  (Calculator& theCommands, const SemisimpleSubalgebras& input, Expression& output);
  static std::string innerStringFromSemisimpleSubalgebras
  (SemisimpleSubalgebras& input);

  static bool innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers
  (Calculator& theCommands, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output);
  static bool innerExpressionFromUE
  (Calculator& theCommands, const ElementUniversalEnveloping<RationalFunctionOld>& input,
   Expression& output, Expression* inputContext=0);
  static bool innerStoreCandidateSA(Calculator& theCommands, const CandidateSSSubalgebra& input, Expression& output);
  static bool innerExpressionFromDynkinType(Calculator& theCommands, const DynkinType& input, Expression& output);
  static bool innerExpressionFromDynkinSimpleType(Calculator& theCommands, const DynkinSimpleType& input, Expression& output);
  static bool innerExpressionFromElementSemisimpleLieAlgebraRationals
  (Calculator& theCommands, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output);
  //conversions from expression containing type to expression tree
  static bool innerStoreSemisimpleLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra*& outputSSalgebra);
  static bool innerCandidateSAPrecomputed
  (Calculator& theCommands, const Expression& input, Expression& output, CandidateSSSubalgebra& outputPointer, SemisimpleSubalgebras& owner);
  static bool innerLoadWeylGroup(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerStoreSemisimpleSubalgebras
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSlTwoSubalgebraPrecomputed(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLoadSltwoSubalgebras(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLoadSemisimpleSubalgebras(Calculator& theCommands, const Expression& inpuT, Expression& output);
  static bool innerExpressionFromChevalleyGenerator
  (Calculator& theCommands, const ChevalleyGenerator& input, Expression& output);
  static bool innerExpressionFromMonomialUE
  (Calculator& theCommands, const MonomialUniversalEnveloping<RationalFunctionOld>& input,
   Expression& output, Expression* inputContext=0);
  static bool innerExpressionFromBuiltInType(Calculator& theCommands, const Expression& input, Expression& output);
  template <class coefficient>
  static bool innerExpressionFromPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionFromRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionFromUE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionFrom(Calculator& theCommands, const MonomialP& input, Expression& output);
};

template <class theType>
bool Calculator::GetVectoR
(const Expression& input, Vector<theType>& output, Expression* inputOutputStartingContext, int targetDimNonMandatory, Expression::FunctionAddress conversionFunction)
{ MacroRegisterFunctionWithName("Calculator::GetVector");
  List<Expression> nonConvertedEs;
  if (!this->GetVectorExpressions(input, nonConvertedEs, targetDimNonMandatory))
    return false;
  List<Expression> convertedEs;
  convertedEs.SetSize(nonConvertedEs.size);
  for (int i=0; i<nonConvertedEs.size; i++)
    if (!this->ConvertToTypeUsingFunction<theType>(conversionFunction, nonConvertedEs[i], convertedEs[i]))
      return false;
  if (!this->ConvertExpressionsToCommonContext(convertedEs, inputOutputStartingContext))
    return false;
  if (targetDimNonMandatory>0)
    if (convertedEs.size!=targetDimNonMandatory)
      return false;
  output.SetSize(convertedEs.size);
  for (int i=0; i<convertedEs.size; i++)
    output[i]=convertedEs[i].GetValue<theType>();
  return true;
}

template <class theType>
bool Calculator::GetMatrix
(const Expression& input, Matrix<theType>& outputMat, Expression* inputOutputStartingContext, int targetNumColsNonMandatory, Expression::FunctionAddress conversionFunction)
{ MacroRegisterFunctionWithName("Calculator::GetMatrix");
  Matrix<Expression> nonConvertedEs;
  //stOutput << " Getting matrix from: " << input.ToString();
  if (!this->GetMatrixExpressions(input, nonConvertedEs, -1, targetNumColsNonMandatory))
    return *this << "Failed to extract matrix of expressions from " << input.ToString();
  //stOutput << " extracted matrix expressions: " << nonConvertedEs.ToString();
  Matrix<Expression> convertedEs;
  convertedEs.init(nonConvertedEs.NumRows, nonConvertedEs.NumCols);
  Expression theContext;
  theContext.MakeEmptyContext(*this);
  if (inputOutputStartingContext!=0)
    if (inputOutputStartingContext->IsContext())
      theContext=*inputOutputStartingContext;
  for (int i=0; i<nonConvertedEs.NumRows; i++)
    for (int j=0; j<nonConvertedEs.NumCols; j++)
    { if (!this->ConvertToTypeUsingFunction<theType>(conversionFunction, nonConvertedEs(i,j), convertedEs(i,j)))
        return false;
      theContext.ContextMergeContexts(theContext, convertedEs(i,j).GetContext(), theContext);
    }
  for (int i=0; i<convertedEs.NumRows; i++)
    for (int j=0; j<convertedEs.NumCols; j++)
      if (!convertedEs(i,j).::Expression::SetContextAtLeastEqualTo(theContext))
        return false;
  outputMat.init(convertedEs.NumRows, convertedEs.NumCols);
  for (int i=0; i<convertedEs.NumRows; i++)
    for (int j=0; j<convertedEs.NumCols; j++)
      outputMat(i,j)=convertedEs(i,j).GetValue<theType>();
  if (inputOutputStartingContext!=0)
    *inputOutputStartingContext=theContext;
  return true;
}

template <class coefficient>
bool Expression::MakeSum(Calculator& theCommands, const MonomialCollection<Expression, coefficient>& theSum)
{ MacroRegisterFunctionWithName("Expression::MakeSum");
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  if (theSum.IsEqualToZero())
    return this->AssignValue<Rational>(0, theCommands);
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(theSum.size());
  for (int i=0; i<theSum.size(); i++)
  { Expression& current=summandsWithCoeff[i];
    if (theSum[i]==oneE)
      current.AssignValue(theSum.theCoeffs[i], theCommands);
    else if (!(theSum.theCoeffs[i]==1))
    { current.reset(theCommands, 3);
      current.AddChildAtomOnTop(theCommands.opTimes());
      current.AddChildValueOnTop(theSum.theCoeffs[i]);
      current.AddChildOnTop(theSum[i]);
    } else
      current=theSum[i];
  }
  if (summandsWithCoeff.size>=2)
    if (summandsWithCoeff[0]>summandsWithCoeff[1] && summandsWithCoeff[1]>summandsWithCoeff[0])
      crash << "This is a programming error: bad comparison! " << crash;
  return this->MakeOXdotsX(theCommands, theCommands.opPlus(), summandsWithCoeff);
}

template <class theType>
bool Expression::AssignValueWithContext(const theType& inputValue, const Expression& theContext, Calculator& owner)
{ this->reset(owner, 3);
  this->AddChildAtomOnTop(this->GetTypeOperation<theType>());
  this->AddChildOnTop(theContext);
  return this->AddChildAtomOnTop(this->AddObjectReturnIndex(inputValue));
}

template <class theType>
bool Expression::AssignValue(const theType& inputValue, Calculator& owner)
{ Expression tempE;
  tempE.owner=&owner;
  int curType=tempE.GetTypeOperation<theType>();
  if (curType==owner.opPoly() || curType==owner.opRationalFunction() || curType==owner.opElementTensorGVM() || curType==owner.opElementUEoverRF() ||
      curType==owner.opMatRF() || curType==owner.opElementWeylAlgebra() || curType==owner.opWeightLieAlgPoly())
  { crash << "This may or may not be a programming error. Assigning value WITHOUT CONTEXT to data type "
    << this->owner->GetOperations()[curType] << " is discouraged, and most likely is an error. Crashing to let you know. "
    << crash;
  }
  Expression emptyContext;
  emptyContext.MakeEmptyContext(owner);
  return this->AssignValueWithContext(inputValue, emptyContext, owner);
}

template <class theType>
bool Expression::MergeContextsMyArumentsAndConvertThem(Expression& output)const
{ MacroRegisterFunctionWithName("Expression::MergeContextsMyArumentsAndConvertThem");
//  stOutput << "<hr>Merging context arguments of " << this->ToString();
  this->CheckInitialization();
  Expression mergedContexts;
  if (!this->MergeContextsMyAruments(mergedContexts))
    return false;
//  stOutput << "<br> continuing to merge " << mergedContexts.ToString();
  output.reset(*this->owner, this->children.size);
  output.AddChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i=1; i<mergedContexts.children.size; i++)
  { if (!mergedContexts[i].ConvertToType<theType>(convertedE))
    { *this->owner << "<hr>Failed to convert " << mergedContexts[i].ToString() << " to the desired type. ";
      return false;
    }
    output.AddChildOnTop(convertedE);
  }
  return true;
}

template<class coefficient>
bool Calculator::GetTypeWeight
(Calculator& theCommands, const Expression& input, Vector<coefficient>& outputWeightSimpleCoords,
 Expression& outputWeightContext, SemisimpleLieAlgebra*& ambientSSalgebra,
 Expression::FunctionAddress ConversionFun)
{ MacroRegisterFunctionWithName("Calculator::GetTypeWeight");
  if (!input.IsListNElements(3))
  { theCommands
    << "Function TypeHighestWeightParabolic is expected to have two arguments: "
    << "SS algebra type, highest weight in simple coords. ";
    return false;
  }
  const Expression& leftE=input[1];
  const Expression& middleE=input[2];
  if (!Calculator::CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, leftE, ambientSSalgebra))
  { theCommands << "Error extracting Lie algebra from " << leftE.ToString();
    return false;
  }
  if (!theCommands.GetVectoR<coefficient>
      (middleE, outputWeightSimpleCoords, &outputWeightContext, ambientSSalgebra->GetRank(), ConversionFun))
  { theCommands << "Failed to convert the second argument of HWV to a list of " << ambientSSalgebra->GetRank()
    << " polynomials. The second argument you gave is " << middleE.ToString() << ".";
    return false;
  }
  if (!theCommands.theObjectContainer.theLieAlgebras.ContainsExactlyOnce(*ambientSSalgebra))
    crash << "This is a programming error: " << ambientSSalgebra->GetLieAlgebraName()
    << " contained object container more than once. " << crash;
  int algebraIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*ambientSSalgebra);
  outputWeightContext.ContextSetSSLieAlgebrA(algebraIndex, theCommands);
  return true;
}

template<class coefficient>
bool Calculator::GetTypeHighestWeightParabolic
(Calculator& theCommands, const Expression& input, Expression& output, Vector<coefficient>& outputWeightHWFundcoords,
 Selection& outputInducingSel, Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra, Expression::FunctionAddress ConversionFun)
{ if (!input.IsListNElements(4) && !input.IsListNElements(3))
    return output.MakeError
    ("Function TypeHighestWeightParabolic is expected to have two or three arguments: SS algebra type, highest weight, [optional] parabolic selection. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& middleE=input[2];
  if (!Calculator::CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, leftE, ambientSSalgebra))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  if (!theCommands.GetVectoR<coefficient>(middleE, outputWeightHWFundcoords, &outputHWContext, ambientSSalgebra->GetRank(), ConversionFun))
  { std::stringstream tempStream;
    tempStream << "Failed to convert the second argument of HWV to a list of " << ambientSSalgebra->GetRank()
    << " polynomials. The second argument you gave is " << middleE.ToString() << ".";
    return output.MakeError(tempStream.str(), theCommands);
  }
  if (input.IsListNElements(4))
  { Vector<Rational> parabolicSel;
    const Expression& rightE=input[3];
    if (!theCommands.GetVectoR<Rational>(rightE, parabolicSel, &outputHWContext, ambientSSalgebra->GetRank(), 0))
    { std::stringstream tempStream;
      tempStream << "Failed to convert the third argument of HWV to a list of " << ambientSSalgebra->GetRank()
      << " rationals. The third argument you gave is " << rightE.ToString() << ".";
      return output.MakeError(tempStream.str(), theCommands);
    }
    outputInducingSel=parabolicSel;
  } else
  { outputInducingSel.init(ambientSSalgebra->GetRank());
    for (int i=0; i<outputWeightHWFundcoords.size; i++)
      if (!outputWeightHWFundcoords[i].IsSmallInteger())
        outputInducingSel.AddSelectionAppendNewIndex(i);
  }
  if (!theCommands.theObjectContainer.theLieAlgebras.ContainsExactlyOnce(*ambientSSalgebra))
    crash << "This is a programming error: " << ambientSSalgebra->GetLieAlgebraName() << " contained object container more than once. " << crash;
  int algebraIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*ambientSSalgebra);
  outputHWContext.ContextSetSSLieAlgebrA(algebraIndex, theCommands);
//  stOutput << "final context of GetTypeHighestWeightParabolic: " << outputHWContext.ToString();
  return true;
}
#endif
