//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader2_h_already_included
#define vpfHeader2_h_already_included

//the following  include contains all the c++ math routines used in the calculator.
#include "vpfHeader1_3.h"
#include "vpfHeader3ListReferences.h"
#include "vpfHeader1_4SemisimpleLieAlgebras.h"
#include "vpfCharacters.h"
#include "vpfGraph.h"

static ProjectInformationInstance ProjectInfoVpfHeader2(__FILE__, "Header file containing the calculator's parsing routines. ");

class Expression
{ void reset()
  { this->theBoss=0;
    this->children.size=0;
    this->theData=-1;
    this->format=this->formatDefault;
//    this->IndexBoundVars=inputIndexBoundVars;
  }
  //Definitions.
  //1. Fundamentals.
  //1.1. An atom is an expression with zero children.
  //     *******************************************
  //     We will say informally "an atom equals (the integer) X" to mean that
  //     the theData of the corresponding atom equals X.
  //     We will say informally "an atom equals (the keyword) X" to mean that
  //     the theData of the corresponding atom equals the integer CommandList::opX().
  //     Note that this language use is completely informal, and could be ambiguous:
  //     the theData entry of an atom can be intepretted either as keyword or as an
  //     an actual piece of data (not necessarily equal to CommandList::opX() for some X).
  //     Whenever this ambiguity becomes and issue, the informal language should be dropped,
  //     and explicit reference to the Expression::theData and Expression::children members
  //     should be made.
  //1.2. A list is an expression with 1 or more children whose theData entry equals
  //     0 which *MUST* be equal to CommandList::opList().
  //1.3. An expression with 1 or more children whose is not allowed to have theData entry different
  //     from CommandList::opList(). The system is instructed to
  //     crash and burn shall such a configuration be detected.
  //2. Basic building blocks
  //2.1. A quote, or a frozen expression, is list whose first entry is an atom equal to Quote.
  //2.2. A sequence is a list whose first entry is an atom equal to Sequence.
  //2.3. A bound variable is a list with two atomic entries, the first of which
  //     equals Bind.
  //2.4. A non-bound variable is a list with two atomic entries. The first entry equals NonBound.
  //     The second entry equals an integer that uniquely identifies the variable.
  //     In the current implementation, the integer is the index in the object container.
  //2.5. An error is a list with two entries whose first entry is an atom equal to Error,
  //     and whose second entry is a string.
  //*Note that CommandList::opList() is required to equal zero for reasons of program speed.
  //This is GUARANTEED, and you MAY assume it.
  //If you want to have a list of mathematical objects, use the Sequence
  //data structure. A sequence is a List whose first entry is an atom whose value
  //is opSequence.
  //
  //-------------------------------------------------------
  //
  //Expressions of built-in types.
  //1. An expression of built-in type represent hard-coded C++ types.
  //2. An expression is said to be of built-in type if it is a list of 2 or 3 elements
  //   starting with an atom
  //   equal to one of a set of hard-coded names, and ending in an atom whose value
  //   is an integer that uniquely (up to operator==) identifies the C++ structure.
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
  //position in CommandList::theExpressionContainer.
  //CommandList::theExpressionContainer is a Hashed List of references and must not be modified
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
  //If a single instance of CommandList is to run over long periods of time,
  //it is possible that not all such expressions are in use, and we run out of RAM memory.
  //If that is to happen, some sort of garbage collection will have to be implemented.
  //However, for the current calculator purposes, no such danger exists.
  public:
  int theData;
  List<int> children;
  CommandList* theBoss;
  ///////////////////////////////////////
  //two objects are considered equal even when the the following data is different:
  int format;
//////
  typedef bool (*FunctionAddress)
  (CommandList& theCommands, const Expression& input, Expression& output);
//////
  friend std::ostream& operator << (std::ostream& output, const Expression& theMon)
  { output << theMon.ToString();
    return output;
  }
  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot, formatNoBracketsForFunctionArgument, formatMatrix, formatMatrixRow
  };
  void reset(CommandList& newBoss, int newNumChildren=0)
  { this->theBoss=&newBoss;
    this->theData=0;
    this->format=this->formatDefault;
    if (newNumChildren<0)
      newNumChildren=0;
    this->children.SetSize(newNumChildren);
    for (int i=0; i<newNumChildren; i++)
      this->children[i]=-1;
  }
  bool AssignChildAtomValue(int childIndex, int theAtom, CommandList& owner);
  bool AddChildOnTop(const Expression& inputChild)
  { this->children.SetSize(this->children.size+1);
    return this->AssignChild(this->children.size-1, inputChild);
  }
  bool AddAtomOnTop(const std::string& theOperationString);
  bool AddAtomOnTop(int theOp)
  { this->CheckInitialization();
    Expression tempE;
    tempE.MakeAtom(theOp, *this->theBoss);
    return this->AddChildOnTop(tempE);
  }
  bool AssignChild(int childIndexInMe, const Expression& inputChild);
  bool AssignChild(int childIndexInMe, int childIndexInBoss);
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
  bool IsListNElementsStartingWithAtom(int theOp=-1, int N=-1)const;
  bool IsListStartingWithAtom(int theOp=-1)const
  { if (!this->IsLisT())
      return false;
    if (!(*this)[0].IsAtoM())
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
    return (*this)[1].IsAtoM();
  }
  bool IsAtoM(int desiredDataUseMinusOneForAny=-1)const
  { if (this->IsLisT())
      return false;
    if (desiredDataUseMinusOneForAny==-1)
      return true;
    return this->theData==desiredDataUseMinusOneForAny;
  }
  bool IsOperation(std::string* outputWhichOperation=0)const;
  bool IsBuiltInType(std::string* outputWhichOperation=0)const;
  bool IsBuiltInType(int* outputWhichType)const;
  const Expression& operator[](int n)const;
  bool IsSequenceNElementS(int N=-2)const;
  bool IsError(std::string* outputErrorMessage=0)const;
  bool IsContext()const;

  template <class theType>
  bool ConvertToType(Expression& output)const;
  template <class theType>
  bool IsOfType(theType* whichElement=0)const
  { MacroRegisterFunctionWithName("Expression::IsOfType");
    if (this->theBoss==0)
      return false;
    if (!this->IsListNElementsStartingWithAtom(this->GetTypeOperation<theType>()))
      return false;
    if(this->children.size<2 || !this->GetLastChild().IsAtoM())
      return false;
    if (whichElement==0)
      return true;
    *whichElement=this->GetValuE<theType>();
    return true;
  }
  template <class theType>
  const theType& GetValuE()const
  { return this->GetValuENonConstUseWithCaution<theType>();
  }
  template <class theType>
  theType& GetValuENonConstUseWithCaution()const;

  template<class theType>
  int GetTypeOperation()const;
  template<class theType>
  int AddObjectReturnIndex(const theType& inputValue)const;

  //note: the following always returns true:
  template <class theType>
  bool AssignValue(const theType& inputValue, CommandList& owner)
  { Expression tempE;
    tempE.MakeEmptyContext(owner);
    this->AssignValueWithContext(inputValue, tempE, owner);
    return true;
  }
  //note: the following always returns true:
  template <class theType>
  bool AssignValueWithContext
  (const theType& inputValue, const Expression& theContext, CommandList& owner)
  { this->reset(owner, 3);
    int theIndex =this->AddObjectReturnIndex(inputValue);
    Expression tempE;
    tempE.MakeAtom(this->GetTypeOperation<theType>(), owner);
    this->AssignChild(0, tempE);
    this->AssignChild(1, theContext);
    tempE.MakeAtom(theIndex, owner);
    this->AssignChild(2, tempE);
    return true;
  }
  template <class theType>
  bool AssignValueToChild(int childIndex, const theType& inputValue, CommandList& owner)
  { Expression tempE;
    tempE.AssignValue(inputValue, owner);
    return this->AssignChild(childIndex, tempE);
  }
  template <class theType>
  bool AssignValueWithContextToChild
  (int childIndex, const theType& inputValue, const Expression& theContext, CommandList& owner)
  { Expression tempE;
    tempE.AssignValueWithContext(inputValue, theContext, owner);
    return this->AssignChild(childIndex, tempE);
  }
  bool SetContextAtLeastEqualTo(Expression& inputOutputMinContext);
  int GetNumContextVariables()const;
  bool RemoveContext();
  bool HasContext()const;
  bool HasNonEmptyContext()const;
  Expression GetContext()const;
  static bool MergeContexts(Expression& leftE, Expression& rightE);

  template<class dataType>
  bool GetContextForConversionIgnoreMyContext(Expression& output)const;

  Expression ContextGetContextVariable(int variableIndex) const;
  int ContextGetIndexAmbientSSalg()const;
  void ContextGetFormatExpressions(FormatExpressions& output)const;
  int ContextGetNumContextVariables()const;
  static bool ContextMergeContexts
  (const Expression& leftContext, const Expression& rightContext, Expression& outputContext);
  static bool ContextConvertBothToCommonContext
  (Expression& leftE, Expression& rightE, Expression& outputContext)
  { if (!Expression::ContextMergeContexts(leftE.GetContext(), rightE.GetContext(), outputContext))
      return false;
    if (!leftE.SetContextAtLeastEqualTo(outputContext))
      return false;
    if (!rightE.SetContextAtLeastEqualTo(outputContext))
      return false;
    return true;
  }
  bool ContextGetPolySubFromSuperContext
  (const Expression& largerContext, PolynomialSubstitution<Rational>& output)const
  ;
  Expression ContextGetPolynomialVariables()const;
  bool ContextSetSSLieAlgebrA(int indexInOwners, CommandList& owner);
  bool ContextMakeContextSSLieAlgebrA(int indexInOwners, CommandList& owner)
  { this->MakeEmptyContext(owner);
    return this->ContextSetSSLieAlgebrA(indexInOwners, owner);
  }

  SemisimpleLieAlgebra* GetAmbientSSAlgebraNonConstUseWithCaution()const;
  const SemisimpleLieAlgebra* GetAmbientSSAlgebra()const
  { return this->GetAmbientSSAlgebraNonConstUseWithCaution();
  }

  bool IsEqualToZero()const;
  bool IsEqualToOne()const;
  void MakeMonomialGenVerma
  (const MonomialGeneralizedVerma<RationalFunctionOld>& inputMon, CommandList& newBoss)
 ;
  void MakeElementTensorsGeneralizedVermas
  (const ElementTensorsGeneralizedVermas<RationalFunctionOld>& inputMon, CommandList& newBoss)
 ;
  void MakeSerialization
  (const std::string& secondEntry, CommandList& theCommands, int numElementsToReserve=0);
  void MakeAtom(int input, CommandList& newBoss)
  { this->reset(newBoss);
    this->theData=input;
  }
  bool EvaluatesToVariableNonBound()const;
  Expression::FunctionAddress GetHandlerFunctionIamNonBoundVar();
  void MakeProducT
  (CommandList& owner, const Expression& left, const Expression& right)
  ;
  void MakeFunction
  (CommandList& owner, const Expression& theFunction, const Expression& theArgument)
  ;
  int GetNumCols()const
  ;
  bool MakeXOX
  (CommandList& owner, int theOp, const Expression& left, const Expression& right)
  ;
  bool MakeOX
  (CommandList& owner, int theOp, const Expression& opArgument)
  ;
  bool Sequencefy
  ()
  ;
  void AssignXOXToChild
  (int childIndex, CommandList& owner, int theOp, const Expression& left, const Expression& right)
  { Expression tempE;
    tempE.MakeXOX(owner, theOp, left, right);
    this->AssignChild(childIndex, tempE);
  }
  bool MakeEmptyContext
  (CommandList& owner)
  ;
  std::string Lispify
  ()const
  ;
  bool ToStringData
  (std::string& output, FormatExpressions* theFormat=0, Expression* startingExpression=0)const;
  std::string ToString(FormatExpressions* theFormat=0, Expression* startingExpression=0)const;
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
  Expression()
  { this->reset();
  }
  const Expression& GetLastChild()const
  { return (*this)[this->children.size-1];
  }
  bool SetError (const std::string& theError, CommandList& owner);
  Expression(const Expression& other)
  { this->operator=(other);
  }
  Expression(CommandList& inputBoss)
  { this->reset(inputBoss);
  }
  bool CheckInitialization()const
  { if (this->theBoss==0)
    { std::cout << "This is a programming error: Expression has non-initialized "
      << "owner. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
    return true;
  }
  bool IsSmallInteger(int* whichInteger=0)const;
  bool IsInteger(LargeInt* whichInteger=0)const;
  bool HasBoundVariables()const;
  bool IsMeltable(int* numResultingChildren=0)const;
  bool AreEqualExcludingChildren(const Expression& other) const
  { return
    this->theBoss==other.theBoss &&
    this->theData==other.theData &&
    this->children.size==other.children.size
    ;
  }
//  Rational GetConstantTerm() const;
  bool operator==(const Expression& other)const;
  bool operator!=(const Expression& other)const
  { return ! (*this==other);
  }
  void operator=(const Expression& other);
  bool operator>(const Expression& other)const;
};

class Function
{
  public:
  Expression theArgumentTypes;
  std::string theDescription;
  std::string theExample;
  bool flagIsInner;
  bool flagMayActOnBoundVars;
  bool flagIamVisible;
  Expression::FunctionAddress theFunction;

  std::string GetString(CommandList& theBoss);
  void operator =(const Function& other)
  { this->theArgumentTypes=other.theArgumentTypes;
    this->theDescription=other.theDescription;
    this->theExample=other.theExample;
    this->theFunction=other.theFunction;
    this->flagIsInner=other.flagIsInner;
    this->flagMayActOnBoundVars=other.flagMayActOnBoundVars;
    this->flagIamVisible=other.flagIamVisible;
  }
  bool operator==(const Function& other)const
  { return this->theArgumentTypes==other.theArgumentTypes &&
    this->theFunction==other.theFunction && this->flagIsInner==other.flagIsInner;
  }
  void reset(CommandList& owner)
  { this->theArgumentTypes.reset(owner);
    this->theFunction=0;
    this->flagIsInner=true;
  }
  bool inputFitsMyInnerType(const Expression& input);
  Function()
  { this->theFunction=0;
  }
  Function
  (const Expression::FunctionAddress& functionPointer, Expression* inputArgTypes,
   const std::string& description, const std::string& inputExample, bool inputflagIsInner,
   bool inputflagNameIsVisible, bool inputflagMayActOnBoundVars=false)
  { this->theFunction=functionPointer;
    this->theDescription=description;
    this->theExample=inputExample;
    if (inputArgTypes!=0)
      this->theArgumentTypes=*inputArgTypes;
    this->flagIsInner=inputflagIsInner;
    this->flagIamVisible=inputflagNameIsVisible;
    this->flagMayActOnBoundVars=inputflagMayActOnBoundVars;
  }
  inline static unsigned int HashFunction(const Function& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return *( (unsigned int *) &(this->theFunction));
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
  std::string ToString(CommandList& theBoss)const;
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

//the following class is meant to use to draw plots for calculus students.
class CalculusFunctionPlot
{ public:
  List<std::string> theFunctionsCalculatorInput;
  List<std::string> theFunctions;
  List<Rational> lowerBounds;
  List<Rational> upperBounds;
  std::string GetPlotStringAddLatexCommands(bool useHtml);
  void operator=(const CalculusFunctionPlot& other)
  { this->theFunctions=other.theFunctions;
    this->lowerBounds=other.lowerBounds;
    this->upperBounds=other.upperBounds;
    this->theFunctionsCalculatorInput=other.theFunctionsCalculatorInput;
  }
  bool operator==(const CalculusFunctionPlot& other)const
  { return
    this->theFunctions==other.theFunctions &&
    this->lowerBounds==other.lowerBounds &&
    this->upperBounds==other.upperBounds &&
    this->theFunctionsCalculatorInput==other.theFunctionsCalculatorInput;
  }
  void operator+=(const CalculusFunctionPlot& other);
};

class ObjectContainer
{ //Following are containers for data structures that are implemented in C++.
  //These objects are dynamically allocated and used by the calculator as requested
  //by various predefined function handlers.
public:
  HashedListReferences<CoxeterElement> theCoxeterElements;
  HashedListReferences<Character<Rational> > theCharacters;
  ListReferences<CoxeterGroup> theCoxeterGroups;
  ListReferences<ModuleSSalgebra<RationalFunctionOld> > theCategoryOmodules;
  ListReferences<SemisimpleLieAlgebra> theLieAlgebras;
  ListReferences<SemisimpleSubalgebras> theSSsubalgebras;
  HashedListReferences<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theTensorElts;
  HashedListReferences<Polynomial<Rational> > thePolys;
  HashedListReferences<ElementUniversalEnveloping<RationalFunctionOld> > theUEs;
  HashedListReferences<RationalFunctionOld> theRFs;
  HashedListReferences<Rational> theRationals;
  HashedListReferences<charSSAlgMod<Rational> > theCharsSSLieAlgFD;
  AlgebraicNumberRegistry theAlgebraicNumberRegistry;
  HashedListReferences<AlgebraicNumber> theAlgebraicNumbers;
  HashedListReferences<double, MathRoutines::HashDouble> theDoubles;
  HashedListReferences<std::string, MathRoutines::hashString> theStrings;
  HashedListReferences<std::string, MathRoutines::hashString> ExpressionNotation;
  HashedListReferences<Expression> ExpressionWithNotation;
  HashedListReferences<LittelmannPath> theLSpaths;
  HashedListReferences<Matrix<Rational> > theMatRats;
  HashedListReferences<Matrix<RationalFunctionOld> > theMatRFs;
  ListReferences<CalculusFunctionPlot> thePlots;
//  HashedList<DifferentialForm<Rational> > theDiffForm;
  HashedListReferences<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
  void reset();
  std::string ToString();
};

struct StackMaintainerRules
{
public:
  CommandList* theBoss;
  int startingRuleStackSize;
  StackMaintainerRules(CommandList* inputBoss);
  ~StackMaintainerRules();
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
  ExpressionTripleCrunchers(int inputOp, int inputLeft, int inputRight)
  : theOp(inputOp), leftType(inputLeft), rightType(inputRight)
  {}
  static unsigned int HashFunction(const ExpressionTripleCrunchers& input)
  { return (unsigned int) input.leftType*SomeRandomPrimes[0]+
    (unsigned int) input.rightType*SomeRandomPrimes[1]+
    (unsigned int) input.theOp*SomeRandomPrimes[2];
  }
};

class CommandList
{ template <class dataType>
  bool EvaluatePMTDtree
(const Expression& inputContext, const Expression& input, Expression& output)
  ;
  template <class dataType>
  static bool innerExtractPMTDtreeContext
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  //Operations parametrize the expression elements.
  //Operations are the labels of the atom nodes of the expression tree.
  HashedList<std::string, MathRoutines::hashString> operations;
  HashedList<std::string, MathRoutines::hashString> builtInTypes;
  List<List<Function> > FunctionHandlers;
public:
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
//1.1. An outer function is called on X if the first child of X
//     is an atom equal to the name of the outer function.
//1.2. If the outer function returns true but its output is identically equal to the
//     starting expression X, nothing is done (the action of the outer function is ignored).
//1.3. If an outer function returns true and its output is different from X,
//     X is replaced by this output.
//2. Inner functions ("functions").
//2.1. If the first child of X is an atom equal to the name of the inner function,
//     we define Argument as follows.
//2.1.1. If X has two children, Argument is set to the second child of X.
//2.1.2. If X does not have two children, Argument is set to be equal to the entire X.
//2.2. The inner function is called on Argument.
//2.3. If the inner function returns true, X is substituted with
//     the output of the inner function, else nothing is done.
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

  List<Expression> buffer1, buffer2;
  int MaxRecursionDeptH;
  int RecursionDeptH;
  int DepthRecursionReached;
  int MaxAlgTransformationsPerExpression;
  int MaxLatexChars;
  int MaxNumCachedExpressionPerContext;
  int NumErrors;
  ///////////////////////////////////////////////////////////////////////////
  bool flagAbortComputationASAP;
  bool flagTimeLimitErrorDetected;
  bool flagFirstErrorEncountered;
  bool flagMaxRecursionErrorEncountered;
  bool flagMaxTransformationsErrorEncountered;
  bool flagNewContextNeeded;

  bool flagLogSyntaxRules;
  bool flagLogEvaluatioN;
  bool flagLogPatternMatching;
  bool flagLogFullTreeCrunching;
  bool flagProduceLatexLink;
  bool flagHideLHS;
  ///////////////////////////////////////////////////////////////////////////
  int TotalNumPatternMatchedPerformed;
  int NumPredefinedVars;
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
  HashedList<Expression> ExpressionStack;
  List<HashedList<int, MathRoutines::IntUnsignIdentity> > NonBoundVariablesStack;
  List<HashedList<int, MathRoutines::IntUnsignIdentity> > BoundVariablesStack;

  int RuleContextIdentifier;
  List<Expression> RuleStack;

  HashedListReferences<Expression> theExpressionContainer;

  std::string syntaxErrors;
  List<std::string> evaluationErrors;
  std::string inputStringRawestOfTheRaw;
  std::string inputString;
  std::string outputString;
  std::string outputCommentsString;
  std::string DisplayNameCalculator;
  std::string parsingLog;
  GlobalVariables* theGlobalVariableS;
  ObjectContainer theObjectContainer;
  double StartTimeEvaluationInSecondS;

  char DefaultWeylLetter;
  int DefaultWeylRank;
  std::string javaScriptDisplayingIndicator;
  std::string DisplayPathServerBase;
  std::string PhysicalPathServerBase;
  int numOutputFiles;
  std::string DisplayPathOutputFolder;
  std::string PhysicalPathOutputFolder;
  std::string DisplayNameDefaultOutput;
  std::string DisplayNameDefaultOutputNoPath;
  std::string PhysicalNameDefaultOutput;
  std::string indicatorFileNamE;
  std::string indicatorFileNameDisplaY;
  std::string userLabel;
  List<std::string> SystemCommands;
  std::string ToString();
  std::string ElementToStringNonBoundVars();
  std::string ToStringFunctionHandlers();
  std::stringstream Comments;
  FormatExpressions formatVisibleStrings;
  bool IsBoundVarInContext(int inputOp);
  bool IsNonBoundVarInContext(int inputOp);
  //to make operations read only, we make operations private and return const pointer to it.
  inline const HashedList<std::string, MathRoutines::hashString>& GetOperations()
  { return this->operations;
  }
  inline const HashedList<std::string, MathRoutines::hashString>& GetBuiltInTypes()
  { return this->builtInTypes;
  }
  SyntacticElement GetSyntacticElementEnd()
  { SyntacticElement result;
    result.controlIndex=this->controlSequences.GetIndex(";");
    return result;
  }
  bool DecreaseStackSetCharacterRangeS(int decrease)
  { if (decrease<=0)
      return true;
    assert((*this->CurrentSyntacticStacK).size-decrease>0);
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-1].IndexLastCharPlusOne=
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].IndexLastCharPlusOne;
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-decrease);
    return true;
  }
  bool DecreaseStackExceptLast(int decrease)
  { if (decrease<=0)
      return true;
    assert((*this->CurrentSyntacticStacK).size-decrease>0);
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-2].IndexLastCharPlusOne=
//    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].IndexLastCharPlusOne;
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-1]=
    *this->CurrentSyntacticStacK->LastObject();
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-decrease);
    return true;
  }

  std::string ElementToStringSyntacticStack();
  std::string ElementToStringSyntactic(bool usePolishForm=false)
  ;
  std::string WriteDefaultLatexFileReturnHtmlLink
  (const std::string& fileContent, bool useLatexDviPSpsTopdf=false)
  ;
  std::string GetCalculatorLink(const std::string& input)
  { return CGI::GetCalculatorLink(this->DisplayNameCalculator, input);
  }
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
  Expression::FunctionAddress GetInnerFunctionFromOp
  (int theOp, const Expression& left, const Expression& right)
;
  bool LookAheadAllowsThePower(const std::string& lookAhead)
  { return lookAhead!="{}";
  }
  bool LookAheadAllowsApplyFunction(const std::string& lookAhead)
;
  bool AllowsPlusInPreceding(const std::string& lookAhead)
  ;
  bool AllowsTimesInPreceding(const std::string& lookAhead);
  bool AllowsTensorInPreceding(const std::string& lookAhead);
  bool LookAheadAllowsDivide(const std::string& lookAhead);
  bool PopTopSyntacticStack()
  { (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-1);
    return true;
  }
  bool ReplaceEXXEXEByEusingO(int theOp);
  bool ReplaceEXXEXEXByEXusingO(int theOp);
  bool ReplaceEOXbyEX();
  bool ReplaceEEByEusingO(int theControlIndex);
  bool ReplaceEEXByEXusingO(int theControlIndex);
  bool ReplaceEEByE();
  bool ReplaceEXEXByEX(int formatOptions);
  bool ReplaceSsSsXdotsXbySsXdotsX(int numDots);
  bool ReplaceEXdotsXbySsXdotsX(int numDots);
  bool ReplaceEXdotsXBySs(int numDots)
  { this->ReplaceEXdotsXbySsXdotsX(numDots);
    return this->DecreaseStackSetCharacterRangeS(numDots);
  }
  bool ReplaceOEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceEOEXByEX(int formatOptions=Expression::formatDefault);
  bool ReplaceECByC();
  bool ReplaceEXEBySequence(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceYXBySequenceX(int theControlIndex, int inputFormat=Expression::formatDefault)
  { return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, inputFormat, 1);
  }
  bool ReplaceXOXbyEusingO(int theControlIndex, int inputFormat=Expression::formatDefault)
  ;
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
  bool ReplaceXEXEXByEusingO(int theOperation, int formatOptions=Expression::formatDefault)
  ;
  bool ReplaceEXEByEusingO(int theOperation, int formatOptions=Expression::formatDefault)
  ;
  bool ReplaceXYByConY(int theCon, int theFormat=Expression::formatDefault)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=theCon;
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=theFormat;
    return true;
  }
  bool ReplaceXXByCon(int theCon, int theFormat=Expression::formatDefault)
  { this->ReplaceXYByConY(theCon, theFormat);
    return this->DecreaseStackSetCharacterRangeS(1);
  }
  bool ReplaceXByCon(int theCon, int theFormat=Expression::formatDefault);
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
  bool ReplaceXEXByE(int inputFormat=Expression::formatDefault)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=
    this->conExpression();
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat;
    this->DecreaseStackSetCharacterRangeS(2);
    return true;
  }
  bool ReplaceAXbyEX();
  bool ReplaceOXbyEX(int inputFormat=Expression::formatDefault);
  bool ReplaceOXbyEXusingO(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence(int inputFormat=Expression::formatDefault);
  bool ReplaceOXbyE(int inputFormat=Expression::formatDefault);
  bool ReplaceIntIntBy10IntPlusInt()
  ;
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
  { return this->controlSequences.GetIndexIMustContainTheObject("if:=");
  }
  int conComma()
  { return this->controlSequences.GetIndexIMustContainTheObject(",");
  }
  int conDefine()
  { return this->controlSequences.GetIndexIMustContainTheObject(":=");
  }
  int conIsDenotedBy()
  { return this->controlSequences.GetIndexIMustContainTheObject(":=:");
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
  int opApplyFunction()
  { return this->operations.GetIndexIMustContainTheObject("{}");
  }
  int opIsDenotedBy()
  { return this->operations.GetIndexIMustContainTheObject(":=:");
  }
  int opDefine()
  { return this->operations.GetIndexIMustContainTheObject(":=");
  }
  int opDefineConditional()
  { return this->operations.GetIndexIMustContainTheObject("if:=");
  }
  int opThePower()
  { return this->operations.GetIndexIMustContainTheObject("^");
  }
  int opEqualEqual()
  { return this->operations.GetIndexIMustContainTheObject("==");
  }
  int opError()
  { return this->operations.GetIndexIMustContainTheObject("Error");
  }
  int opLisT()
  { return this->operations.GetIndexIMustContainTheObject("");
  }
  int opMonomialCollection()
  { return this->operations.GetIndexIMustContainTheObject("MonomialCollection");
  }
  int opMonomialPoly()
  { return this->operations.GetIndexIMustContainTheObject("MonomialPoly");
  }
  int opSerialization()
  { return this->operations.GetIndexIMustContainTheObject("Serialization");
  }
  int opCalculusPlot()
  { return this->operations.GetIndexIMustContainTheObject("CalculusPlot");
  }
  int opSequence()
  { return this->operations.GetIndexIMustContainTheObject("Sequence");
  }
  int opMelt()
  { return this->operations.GetIndexIMustContainTheObject("Melt");
  }
  int opRational()
  { return this->operations.GetIndexIMustContainTheObject("Rational");
  }
  int opDouble()
  { return this->operations.GetIndexIMustContainTheObject("Double");
  }
  int opAlgNumber()
  { return this->operations.GetIndexIMustContainTheObject("AlgebraicNumber");
  }
  int opPoly()
  { return this->operations.GetIndexIMustContainTheObject("PolynomialRational");
  }
  int opRationalFunction()
  { return this->operations.GetIndexIMustContainTheObject("RationalFunction");
  }
  int opMatRat()
  { return this->operations.GetIndexIMustContainTheObject("Matrix_Rational");
  }
  int opMatRF()
  { return this->operations.GetIndexIMustContainTheObject("Matrix_RF");
  }
  int opString()
  { return this->operations.GetIndexIMustContainTheObject("string");
  }
  int opElementUEoverRF()
  { return this->operations.GetIndexIMustContainTheObject("ElementUEoverRF");
  }
  int opElementTensorGVM()
  { return this->operations.GetIndexIMustContainTheObject("ElementTensorGVM");
  }
  int opCharSSAlgMod()
  { return this->operations.GetIndexIMustContainTheObject("CharSSAlgMod");
  }
  int opSSLieAlg()
  { return this->operations.GetIndexIMustContainTheObject("SemisimpleLieAlg");
  }
  int opSemisimpleSubalgebras()
  { return this->operations.GetIndexIMustContainTheObject("SemisimpleSubalgebras");
  }
  int opCandidateSSsubalgebra()
  { return this->operations.GetIndexIMustContainTheObject("CandidateSSsubalgebra");
  }
  int opLittelmannPath()
  { return this->operations.GetIndexIMustContainTheObject("LittelmannPath");
  }
  int opLRO()
  { return this->operations.GetIndexIMustContainTheObject("LRO");
  }
  int opWeylGroup()
  { return this->operations.GetIndexIMustContainTheObject("WeylGroup");
  }
  int opUnion()
  { return this->operations.GetIndexIMustContainTheObject("\\cup");
  }
  int opPolynomialVariables()
  { return this->operations.GetIndexIMustContainTheObject("PolyVars");
  }
  int opContext()
  { return this->operations.GetIndexIMustContainTheObject("Context");
  }
  int opCoxeterGroup()
  { return this->operations.GetIndexIMustContainTheObject("CoxeterGroup");
  }
  int opCoxeterElement()
  { return this->operations.GetIndexIMustContainTheObject("CoxeterElement");
  }
  int opCharacter()
  { return this->operations.GetIndexIMustContainTheObject("Character");
  }
  int opEndStatement()
  { return this->operations.GetIndexIMustContainTheObject(";");
  }
  int opUnionNoRepetition()
  { return this->operations.GetIndexIMustContainTheObject("\\sqcup");
  }
  int opBind()
  { return this->operations.GetIndexIMustContainTheObject("Bind");
  }
  int opPlus()
  { return this->operations.GetIndexIMustContainTheObject("+");
  }
  int opMinus()
  { return this->operations.GetIndexIMustContainTheObject("-");
  }
  int opTimes()
  { return this->operations.GetIndexIMustContainTheObject("*");
  }
  int opTensor()
  { return this->operations.GetIndexIMustContainTheObject("\\otimes");
  }
  int opLieBracket()
  { return this->operations.GetIndexIMustContainTheObject("[]");
  }
  int opDivide()
  { return this->operations.GetIndexIMustContainTheObject("/");
  }
  bool AppendOpandsReturnTrueIfOrderNonCanonical
  (const Expression& input, List<Expression>& output, int theOp)
;
  bool AppendMultiplicandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, List<Expression>& output, int RecursionDepth, int MaxRecursionDepth)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opTimes());
  }
  bool AppendSummandsReturnTrueIfOrderNonCanonical
  (const Expression& theExpression, List<Expression>& output)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus());
  }
  void SpecializeBoundVars
(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs)
  ;
  Expression* PatternMatch
  (const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
   const Expression* condition=0, std::stringstream* theLog=0, bool logAttempts=false)
;
  bool ProcessOneExpressionOnePatternOneSub
  (Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
    std::stringstream* theLog=0, bool logAttempts=false)
  ;
  static void CheckInputNotSameAsOutput(const Expression& input, const Expression& output)
  { if (&input==&output)
    { std::cout << "This is a programming error: the input expression, equal to "
      << input.ToString() << " has the same address as the output expression. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
//  bool OrderMultiplicationTreeProperly(int commandIndex, Expression& theExpression);
  template <class theType>
  bool CallConversionFunctionReturnsNonConstUseCarefully
  (Expression::FunctionAddress theFun, const Expression& input, theType*& outputData,
   std::string* outputError=0)
  { MacroRegisterFunctionWithName("CommandList::CallConversionFunctionReturnsNonConstUseCarefully");
    if (input.IsOfType<theType>())
    { outputData=&input.GetValuENonConstUseWithCaution<theType>();
      return true;
    }
    Expression tempE;
    if (!theFun(*this, input, tempE))
    { if (outputError!=0)
        *outputError="I was unsuccessful in calling the conversion function.";
      return false;
    }
    if (tempE.IsOfType<theType>())
    { outputData=&tempE.GetValuENonConstUseWithCaution<theType>();
      return true;
    }
    if (outputError!=0)
      *outputError=
      "Successfully called the conversion function but did not get the desired type, instead got "
      + tempE.ToString();
    return false;
  }
  bool CallCalculatorFunction
  (Expression::FunctionAddress theFun, const Expression& input, Expression& output)
  { if (&input==&output)
    { Expression inputCopy=input;
      return this->CallCalculatorFunction(theFun, inputCopy, output);
    }
    if (!theFun(*this, input, output))
      return false;
    return !output.IsError();
  }
  bool ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions,
   std::stringstream* theLog=0)
  ;

  static bool outerUnion
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerUnionNoRepetition
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerTimes
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerOperationBinary(theCommands, input, output, theCommands.opTimes());
  }
  static bool innerGetElementWeylGroup
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerOperationBinary
  (CommandList& theCommands, const Expression& input, Expression& output,
   int theOp)
  { for (int i=0; i<theCommands.FunctionHandlers[theOp].size; i++)
      if (theCommands.FunctionHandlers[theOp][i].inputFitsMyInnerType(input))
        if (theCommands.FunctionHandlers[theOp][i].theFunction(theCommands, input, output))
          return true;
    return false;
  }

  static bool innerUnion
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerUnionNoRepetition
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool EvaluateCarryOutActionSSAlgebraOnGeneralizedVermaModule
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerStandardFunction
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerPlus
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool CollectSummands
(CommandList& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum)
  ;
  static bool outerTensor
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerDivide
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool StandardIsDenotedBy
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fExpressionHasBoundVars
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool outerLieBracket
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerMatrixRational
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerMatrixRationalFunction
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerMinus
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerEqualEqual
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerCombineFractions
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerCheckRule
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerAssociate
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerSubZeroDivAnythingWithZero
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerCancelMultiplicativeInverse
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerAssociateDivisionDivision
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerAssociateTimesDivision
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerCollectMultiplicands
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerExtractBaseMultiplication
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool outerMeltBrackets
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool outerDistribute
(CommandList& theCommands, const Expression& input, Expression& output,
  int AdditiveOp, int multiplicativeOp)
  ;
  static bool outerDistributeTimes
(CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.outerDistribute
    (theCommands, input, output, theCommands.opPlus(), theCommands.opTimes());
  }
  static bool outerLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, const Expression& input, Expression& output,
  int AdditiveOp, int multiplicativeOp)
  ;
  static bool outerRightDistributeBracketIsOnTheRight
  (CommandList& theCommands, const Expression& input, Expression& output,
  int AdditiveOp, int multiplicativeOp)
  ;
  static bool EvaluateIf
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  template<class theType>
  bool GetMatriXFromArguments
  (const Expression& theExpression, Matrix<theType>& outputMat,
   Expression* inputOutputStartingContext=0,
   int targetNumColsNonMandatory=-1, Expression::FunctionAddress conversionFunction=0)
  { Expression tempE=theExpression;
    if (tempE.IsLisT())
      tempE.AssignChildAtomValue(0, this->opSequence(), *this);
    return this->GetMatrix
    (tempE, outputMat, inputOutputStartingContext, targetNumColsNonMandatory, conversionFunction);
  }
  template<class theType>
  bool GetMatrix
  (const Expression& theExpression, Matrix<theType>& outputMat,
   Expression* inputOutputStartingContext=0,
   int targetNumColsNonMandatory=-1, Expression::FunctionAddress conversionFunction=0)
  ;
  template <class theType>
  bool GetVectorFromFunctionArguments
  (const Expression& input, Vector<theType>& output,
   Expression* inputOutputStartingContext=0, int targetDimNonMandatory=-1,
   Expression::FunctionAddress conversionFunction=0)
  { Expression tempE=input;
    if (tempE.IsLisT())
    { //std::cout << "<hr>tempE: " << tempE.Lispify();
      tempE.AssignChildAtomValue(0, this->opSequence(), *this);
      //std::cout << "<br>tempE after change: " << tempE.Lispify();
    }
    return this->GetVectoR(tempE, output, inputOutputStartingContext, targetDimNonMandatory, conversionFunction);
  }
  template <class theType>
  bool GetVectoR
  (const Expression& input, Vector<theType>& output,
   Expression* inputOutputStartingContext=0, int targetDimNonMandatory=-1,
   Expression::FunctionAddress conversionFunction=0)
  ;

  template <class dataType>
  static bool outerExtractAndEvaluatePMTDtree
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool innerFunctionToMatrix
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerDeterminant
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerInvertMatrix
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerDrawPolarRfunctionTheta
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerPlot2D
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerSuffixNotationForPostScript
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerIsInteger
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fFreudenthalEval
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
    static bool innerGCDOrLCM
  (CommandList& theCommands, const Expression& input, Expression& output, bool doGCD)
  ;
  static bool innerLCM
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGCDOrLCM(theCommands, input, output, false);
  }
  static bool fGCD
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGCDOrLCM(theCommands, input, output, true);
  }
  bool GetListPolysVariableLabelsInLex
  (const Expression& input, Vector<Polynomial<Rational> >& output, Expression& outputContext)
  ;
  static bool innerPolynomialDivisionRemainder
  (CommandList& theCommands, const Expression& input, Expression& output)
 ;
  static bool innerPolynomialDivisionVerbose
  (CommandList& theCommands, const Expression& input, Expression& output,
 List<MonomialP>::OrderLeftGreaterThanRight theMonOrder)
 ;
  static bool innerPolynomialDivisionVerboseGrLex
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerPolynomialDivisionVerboseGrLexRev
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerPolynomialDivisionVerboseLexRev
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerPolynomialDivisionVerboseLex
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool fPrintAllPartitions
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fPrintB3G2branchingTableCharsOnly
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  bool fPrintB3G2branchingIntermediate
(CommandList& theCommands, const Expression& input, Expression& output,
 Vectors<RationalFunctionOld>& theHWs, branchingData& theG2B3Data, Expression& theContext
 )
 ;
  static bool fPrintB3G2branchingTableInit
  (CommandList& theCommands, const Expression& input, Expression& output,
   branchingData& theG2B3data, int& desiredHeight, Expression& outputContext)
  ;
  static bool fDecomposeCharGenVerma
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fPrintB3G2branchingTable
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fDifferential
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fPrintB3G2branchingTableCommon
(CommandList& theCommands, const Expression& input, Expression& output,
  Vectors<RationalFunctionOld>& outputHWs, branchingData& theG2B3Data, Expression& theContext
  )
  ;
  static bool innerRationalFunction
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerGetChevGen
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerGetCartanGen
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool fDecomposeFDPartGeneralizedVermaModuleOverLeviPart
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  bool innerSplitFDpartB3overG2Init
(CommandList& theCommands, const Expression& input, Expression& output,
 branchingData& theG2B3Data, Expression& outputContext)
 ;
  static bool fSplitFDpartB3overG2CharsOnly
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerElementUniversalEnvelopingAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool innerPrintSSLieAlgebraShort
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerPrintSSLieAlgebra(theCommands, input, output, false);
  }
  static bool innerPrintSSLieAlgebraVerbose
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerPrintSSLieAlgebra(theCommands, input, output, true);
  }
  static bool innerPrintSSLieAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output, bool Verbose)
;

  template<class CoefficientType>
  bool innerGetTypeHighestWeightParabolic
  (CommandList& theCommands, const Expression& input, Expression& output,
   Vector<CoefficientType>& outputWeightHWFundcoords, Selection& outputInducingSel,
   Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra,
   Expression::FunctionAddress ConversionFun)
 ;
  static bool innerGroebnerGrLex
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGroebner
    (theCommands, input, output, true, false);
  }
  static bool innerGroebnerLex
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerGroebner
    (theCommands, input, output, false, false);
  }
  static bool innerGroebnerModZpLex
  (CommandList& theCommands, const Expression& input, Expression& output, bool useGr)
  { return theCommands.innerGroebner
    (theCommands, input, output, false, true);
  }
  static bool innerGroebner
  (CommandList& theCommands, const Expression& input, Expression& output, bool useGr, bool useModZp)
  ;
 static bool fParabolicWeylGroups
(CommandList& theCommands, const Expression& input, Expression& output)
 ;
 static bool fParabolicWeylGroupsBruhatGraph
(CommandList& theCommands, const Expression& input, Expression& output)
 ;
  static bool fKLcoeffs
(CommandList& theCommands, const Expression& input, Expression& output)
 ;
  static bool innerEmbedSSalgInSSalg
(CommandList& theCommands, const Expression& input, Expression& output)
 ;
  static bool innerWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
 ;
  static bool fWeylOrbitFund
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerWeylOrbit(theCommands, input, output, true, false);
}
  static bool fWeylOrbitSimple
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerWeylOrbit(theCommands, input, output, false, false);
}
  static bool fWeylOrbitFundRho
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerWeylOrbit(theCommands, input, output, true, true);
}
//  static bool innerSSLieAlgebra
//  (CommandList& theCommands, const Expression& input, Expression& output)
//  { return theCommands.innerSSLieAlgebra(theCommands, input, output, false);
//  }
  static bool fSplitFDpartB3overG2CharsOutput
(CommandList& theCommands, const Expression& input, Expression& output,
 branchingData& theG2B3Data
 )
;
  static bool fSplitFDpartB3overG2old
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fSplitFDpartB3overG2
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fSplitGenericGenVermaTensorFD
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fSplitFDpartB3overG2inner
(CommandList& theCommands, branchingData& theG2B3Data, Expression& output)
;
  static bool innerDrawWeightSupportWithMults
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerDrawWeightSupport
  (CommandList& theCommands, const Expression& input, Expression& output)
;

  static bool fHWTAABF
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fElementSSAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fWeylDimFormula
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fLittelmannOperator
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fAnimateLittelmannPaths
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fSqrt
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fFactor
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fSolveSeparableBilinearSystem
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fMinPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fLSPath
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fTestMonomialBaseConjecture
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fJacobiSymbol
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerHWVCommon
(CommandList& theCommands, Expression& output,
 Vector<RationalFunctionOld>& highestWeightFundCoords,
 Selection& selectionParSel, Expression& hwContext, SemisimpleLieAlgebra* owner, bool Verbose=true)
 ;
 bool fWriteGenVermaModAsDiffOperatorInner
(CommandList& theCommands, const Expression& input, Expression& output,
  Vectors<Polynomial<Rational> >& theHws, Expression& hwContext,
  Selection& selInducing, SemisimpleLieAlgebra* owner)
  ;
  template<class CoefficientType>
static bool TypeHighestWeightParabolic
(CommandList& theCommands, const Expression& input, Expression& output,
 Vector<CoefficientType>& outputWeight,
 Selection& outputInducingSel, Expression* outputContext=0)
;
static bool innerDrawRootSystem
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerPrintGenVermaModule
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerHWV
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fWriteGenVermaModAsDiffOperatorUpToLevel
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fWriteGenVermaModAsDiffOperators
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fEmbedG2inB3
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fCasimir
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerRootSAsAndSltwos
  (CommandList& theCommands, const Expression& input, Expression& output, bool showSLtwos)
;
  static bool fprintRootSAs
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerRootSAsAndSltwos
    (theCommands, input, output, false);
  }
  static bool fprintSltwos
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.innerRootSAsAndSltwos
    (theCommands, input, output, true);
  }
  static bool innerCoxeterGroup
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerCoxeterElement
  (CommandList& theCommands, const Expression& input, Expression& output)
;

  static bool innerCharacter
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerCharacterSSLieAlgFD
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerSSsubalgebras
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerAddRatToRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;

static bool innerMultiplyRatByRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyDoubleOrRatByDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyCoxeterEltByCoxeterElt
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyCharacterByCharacter
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyCharSSLieAlgByCharSSLieAlg
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDivideRatByRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDouble
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDivideRFOrPolyOrRatByRFOrPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyRatOrPolyByRatOrPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddUEToAny
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyLRObyLRO
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyLRObyLSPath
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddEltTensorToEltTensor
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddRatOrPolyToRatOrPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddPlotToPlot
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerRatPowerRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDoubleOrRatPowerDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDoubleOrRatTimesDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDoubleOrRatPlusDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddRatOrPolyOrRFToRatOrPolyOrRF
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyUEByAny
  (CommandList& theCommands, const Expression& input, Expression& output)
;

static bool innerTensorEltTensorByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyAnyByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
;
  void AddEmptyHeadedCommand();
  CommandList();
  int AddOperationNoRepetitionOrReturnIndexFirst
  (const std::string& theOpName)
;
  void AddOperationNoRepetitionAllowed
  (const std::string& theOpName)
  ;
  void AddOperationBuiltInType
  (const std::string& theOpName)
  ;

  void AddOperationBinaryInnerHandlerWithTypes
  (const std::string& theOpName, Expression::FunctionAddress innerHandler,
   int leftType, int rightType,
   const std::string& opDescription, const std::string& opExample,
   bool visible=true)
   ;
  void AddOperationHandler
  (const std::string& theOpName, Expression::FunctionAddress handler,
   const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample,
    bool isInner, bool visible=true)
;
  void AddOperationInnerHandler
  (const std::string& theOpName, Expression::FunctionAddress innerHandler,
   const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample,
   bool visible=true)
  { this->AddOperationHandler
    (theOpName, innerHandler, opArgumentListIgnoredForTheTimeBeing, opDescription,
     opExample, true, visible);
  }
  void AddOperationOuterHandler
  (const std::string& theOpName, Expression::FunctionAddress outerHandler,
   const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample,
   bool visible=true)
  { this->AddOperationHandler
    (theOpName, outerHandler, opArgumentListIgnoredForTheTimeBeing, opDescription,
     opExample, false, visible);
  }


  void init(GlobalVariables& inputGlobalVariables);

  void initPredefinedStandardOperations();
  void initPredefinedInnerFunctions()
  ;
  bool ExtractExpressions
(Expression& outputExpression, std::string* outputErrors)
    ;
  void EvaluateCommands();
  bool EvaluateExpression
(const Expression& input, Expression& output, BoundVariablesSubstitution& bufferPairs,
 bool& outputIsFree)
 ;
  void Evaluate(const std::string& theInput)
  { MacroRegisterFunctionWithName("CommandList::Evaluate");
    if (this->theGlobalVariableS==0)
    { this->outputString= "This is a programming error: commandList not initialized properly. Please report this bug. ";
      return;
    }
    this->StartTimeEvaluationInSecondS=this->theGlobalVariableS->GetElapsedSeconds();
    this->inputString=theInput;
    this->ParseAndExtractExpressions(theInput, this->theProgramExpression, this->syntacticSouP, this->syntacticStacK, & this->syntaxErrors);
    this->EvaluateCommands();
  }
  bool ParseAndExtractExpressions
  (const std::string& theInputString, Expression& outputExp, List<SyntacticElement>& outputSynSoup,
   List<SyntacticElement>& outputSynStack, std::string* outputSynErrors)
  { this->CurrentSyntacticStacK=&outputSynStack;
    this->CurrrentSyntacticSouP=&outputSynSoup;
    this->ParseFillDictionary(theInputString);
    bool result=this->ExtractExpressions(outputExp, outputSynErrors);
    this->CurrentSyntacticStacK=&this->syntacticStacK;
    this->CurrrentSyntacticSouP=&this->syntacticSouP;
    return result;
  }
  bool isLeftSeparator(char c);
  bool isRightSeparator(char c);
  void ParseFillDictionary
  (const std::string& input)
  ;

  void initDefaultFolderAndFileNames
  (const std::string& inputPathBinaryBaseIsFolderBelow, const std::string& inputDisplayPathBase, const std::string& scrambledIP)
  ;
  void InitJavaScriptDisplayIndicator();

};

class Serialization
{
public:
  static bool innerStore
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerLoad
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerPolynomial
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
static bool innerStoreUE
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerStorePoly
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerStoreSemisimpleLieAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const SemisimpleLieAlgebra& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const RationalFunctionOld& input, Expression& output,
 Expression* theContext)
;
static bool innerStoreObject
(CommandList& theCommands, const SltwoSubalgebras& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const slTwoSubalgebra& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const SemisimpleSubalgebras& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const CandidateSSSubalgebra& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const Rational& input, Expression& output, Expression* theContext=0)
;
static bool innerLoadFromObject
(CommandList& theCommands, const Expression& input, slTwoSubalgebra& output)
;
static bool innerLoadFromObject
(CommandList& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<Rational>& output)
;
static bool innerLoadFromObject
(CommandList& theCommands, const Expression& input, RationalFunctionOld& output)
;
static bool innerLoadFromObject
(CommandList& theCommands, const Expression& input, Expression& output,
 SemisimpleLieAlgebra** outputPointer=0)
;
static bool innerLoadFromObject
(CommandList& theCommands, const Expression& input, Expression& output,
 CandidateSSSubalgebra& outputPointer, SemisimpleSubalgebras& owner)
;
static bool innerSSLieAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output)
;

static bool innerStoreRationalFunction
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerStoreSemisimpleSubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerLoadSltwoSubalgebra
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerLoadSltwoSubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerLoadSemisimpleSubalgebras
(CommandList& theCommands, const Expression& inpuT, Expression& output)
;
static bool innerStoreObject
(CommandList& theCommands, const ChevalleyGenerator& input, Expression& output,
 Expression* theContext=0, bool* isNonConst=0)
;
static bool innerStoreObject
(CommandList& theCommands, const MonomialUniversalEnveloping<RationalFunctionOld>& input,
 Expression& output, Expression* theContext=0, bool* isNonConst=0)
;
static bool innerStoreObject
(CommandList& theCommands, const MonomialP& input,
 Expression& output, Expression* theContext=0, bool* inputOutputNonConst=0)
;
static bool innerStoreObject
(CommandList& theCommands, const DynkinSimpleType& input,
 Expression& output, Expression* theContext=0, bool* inputOutputNonConst=0)
;

template <class TemplateList>
static bool innerStoreObject
(CommandList& theCommands, const TemplateList& input, Expression& output, Expression* theContext=0)
;

template <class TemplateMonomial, typename CoefficientType>
static bool innerStoreMonCollection
(CommandList& theCommands,
 const MonomialCollection<TemplateMonomial, CoefficientType>& input, Expression& output,
 Expression* theContext=0)
 ;
 template <class TemplateMonomial, typename CoefficientType>
static bool DeSerializeMonCollection
(CommandList& theCommands, const Expression& input,
 MonomialCollection<TemplateMonomial, CoefficientType>& output)
 ;
template <class TemplateMonomial>
static bool DeSerializeMonGetContext
(CommandList& theCommands, const Expression& input, Expression& outputContext)
;
template <class TemplateMonomial>
static bool DeSerializeMon
(CommandList& theCommands, const Expression& input, const Expression& inputContext, TemplateMonomial& outputMon)
;
};

template <class TemplateList>
bool Serialization::innerStoreObject
(CommandList& theCommands, const TemplateList& input, Expression& output, Expression* theContext)
{ output.reset(theCommands);
  output.children.ReservE(input.size+1);
  Expression tempE;
  tempE.MakeAtom(theCommands.opSequence(), theCommands);
  output.AddChildOnTop(tempE);
  for (int i=0; i<input.size; i++)
  { if (!Serialization::innerStoreObject(theCommands, input[i], tempE, theContext))
      return false;
    output.AddChildOnTop(tempE);
  }
  return true;
}

template <class TemplateMonomial, typename CoefficientType>
bool Serialization::innerStoreMonCollection
(CommandList& theCommands, const MonomialCollection<TemplateMonomial, CoefficientType>& input,
 Expression& output, Expression* theContext)
{ MacroRegisterFunctionWithName("Serialization::SerializeMonCollection");
  Expression termE, coeffE, tempE;
  if (input.IsEqualToZero())
    return output.AssignValue(0, theCommands);
  for (int i=input.size-1; i>=0; i--)
  { TemplateMonomial& currentMon=input[i];
    bool isNonConst=true;
    if (!Serialization::innerStoreObject
        (theCommands, currentMon, termE, theContext, &isNonConst))
    { theCommands.Comments << "<hr>Failed to store " << currentMon.ToString() << ". ";
      return false;
    }
    if (!isNonConst)
      termE.AssignValue(input.theCoeffs[i], theCommands);
    else
      if (input.theCoeffs[i]!=1)
      { tempE=termE;
        coeffE.AssignValue(input.theCoeffs[i], theCommands);
        termE.MakeXOX(theCommands, theCommands.opTimes(), coeffE, tempE);
      }
    if (i==input.size-1)
      output=termE;
    else
    { tempE=output;
      output.MakeXOX(theCommands, theCommands.opPlus(), termE, tempE);
    }
    output.CheckInitialization();
  }
//  std::cout << " output: " << output.ToString();
  output.CheckInitialization();
  return true;
}

template <class TemplateMonomial, typename CoefficientType>
bool Serialization::DeSerializeMonCollection
(CommandList& theCommands, const Expression& input,
 MonomialCollection<TemplateMonomial, CoefficientType>& output)
{ MacroRegisterFunctionWithName("Serialization::DeSerializeMonCollection");
  MonomialCollection<Expression, Rational> theSum;
  theCommands.CollectSummands(theCommands, input, theSum);
  Expression currentContext, finalContext;
  finalContext.MakeEmptyContext(theCommands);
  for (int i=0; i<theSum.size; i++)
  { if (!Serialization::DeSerializeMonGetContext<TemplateMonomial>(theCommands, theSum[i], currentContext))
    { theCommands.Comments << "<hr>Failed to load monomial context from " << input.ToString() << "</hr>";
      return false;
    }
    if (!finalContext.ContextMergeContexts(finalContext, currentContext, finalContext))
    { theCommands.Comments << "<hr>Failed to merge monomial contexts: "
      << finalContext.ToString() << " and " << currentContext.ToString() << "</hr>";
      return false;
    }
  }
  output.MakeZero();
  TemplateMonomial tempM;
  for (int i=0; i<theSum.size; i++)
  { if (!Serialization::DeSerializeMon(theCommands, theSum[i], finalContext, tempM))
    { theCommands.Comments << "<hr>Failed to load monomial from " << theSum[i].ToString()
      << " using monomial context " << finalContext.ToString() << ". </hr>";
      return false;
    }
    output.AddMonomial(tempM, theSum.theCoeffs[i]);
  }
  return true;
}

template <class theType>
bool CommandList::GetVectoR
(const Expression& input, Vector<theType>& output, Expression* inputOutputStartingContext,
 int targetDimNonMandatory, Expression::FunctionAddress conversionFunction)
{ MacroRegisterFunctionWithName("CommandList::GetVector");
  MemorySaving<Expression> tempContext;
  Expression& startContext=
  inputOutputStartingContext==0 ? tempContext.GetElement() : *inputOutputStartingContext;
  if (!startContext.IsContext())
    startContext.MakeEmptyContext(*this);
  if (!input.IsSequenceNElementS())
  { if (targetDimNonMandatory>0)
      if (targetDimNonMandatory!=1)
      { this->Comments << "<hr>GetVector failure: target dim is " << targetDimNonMandatory
        << " but the input " << input.ToString() << " can only be interpretted as a single element";
        return false;
      }
    Expression theConverted=input;
    if (!input.IsOfType<theType>())
    { if (conversionFunction==0)
      { this->Comments
        << "<hr>GetVector failure: input is not of desired type and there is no conversion function. <hr>";
        return false;
      }
      this->CallCalculatorFunction(conversionFunction, input, theConverted);
      if (!theConverted.IsOfType<theType>())
      { this->Comments << "<hr> GetVector failure: I did call a conversion function on "
        << theConverted.ToString() << " but the output was not of the desired type. ";
        return false;
      }
    }
    Expression tempContext=theConverted.GetContext();
    if (!tempContext.ContextMergeContexts(tempContext, startContext, startContext))
    { this->Comments << "<hr>GetVector failure: failed to merge starting context with "
      << " context obtained by using the conversion function, i.e., failed to merge "
      << tempContext.ToString() << " with " << startContext.ToString() << ". <hr>";
      return false;
    }
    if (!theConverted.SetContextAtLeastEqualTo(startContext))
    { this->Comments << "<hr>GetVector failure: failed to set context  " << startContext.ToString()
      << " onto  " << theConverted.ToString();
      return false;
    }
    output.SetSize(1);
    output[0]=theConverted.GetValuE<theType>();
    startContext=theConverted.GetContext();
    return true;
  }
  if (targetDimNonMandatory>0)
    if (targetDimNonMandatory!=input.children.size-1)
    { this->Comments << "<hr>Failed to GetVector: the input is required to have "
      << targetDimNonMandatory << " columns but "
      << " it has " << input.children.size-1 << " columns instead. <hr>";
      return false;
    }
  targetDimNonMandatory=input.children.size-1;
  Expression currentContext;
  List<Expression> convertedExpressions;
  convertedExpressions.SetSize(targetDimNonMandatory);
  //Conversion has several phases. I am not sure these phases are the best choice,
  //but they appear to handle every conversion I think should be assumed implicit.
  //1. Extract all contexts of all expressions and merge them into the starting context.
  //2. Check types of expressions, if bad call conversion function.
  //3. Merge the contexts obtained in 1.
  //4. Set contexts of converted expressions to the new common context.

  //1. Extract all contexts of all expressions and merge them into the starting context.
  for (int i=0; i<targetDimNonMandatory; i++)
  { convertedExpressions[i]=input[i+1];
    currentContext=convertedExpressions[i].GetContext();
    if (!currentContext.ContextMergeContexts(currentContext, startContext, startContext))
    { this->Comments << "<hr>GetVector() failed: failed to merge contexts "
      << currentContext.ToString() << " and " << startContext.ToString();
      return false;
    }
  }
  //2. Check types of expressions, if bad call conversion function.
  for (int i=0; i<targetDimNonMandatory; i++)
    if (!convertedExpressions[i].IsOfType<theType>())
    { if (conversionFunction==0)
        return false;
      if (!this->CallCalculatorFunction
          (conversionFunction, convertedExpressions[i], convertedExpressions[i]))
      { this->Comments << "<hr>GetVector() failed: conversion function called on "
        << convertedExpressions[i] << " did not return successfully.<hr> ";
        return false;
      }
      if (!convertedExpressions[i].IsOfType<theType>())
      { this->Comments << "<br>Converting function evaluated on " << convertedExpressions[i].ToString()
        << " returned true, but the output of the function is not of the desired type. "
        << "Instead, the function output is " << convertedExpressions[i].ToString();
        return false;
      }
    }
  //3. Merge contexts of converted expressions into a common context.
  for (int i=0; i<targetDimNonMandatory; i++)
    if (!Expression::ContextMergeContexts
        (startContext, convertedExpressions[i].GetContext(), startContext))
    { this->Comments << "<br>Failed to merge context " << startContext.ToString()
      << " with context " << convertedExpressions[i].GetContext().ToString();
      return false;
    }
  //4. Set contexts of converted expressions to the new common context.
  output.SetSize(targetDimNonMandatory);
  for (int i=0; i<targetDimNonMandatory; i++)
  { //std::cout << "<br>Setting context " << startContext.ToString()
    //<< " onto " << convertedExpressions[i].ToString() << ".";
    if (!convertedExpressions[i].SetContextAtLeastEqualTo(startContext))
    { this->Comments << "<hr>Failed to set context  " << startContext.ToString()
      << " in " << convertedExpressions[i].ToString() << "<hr>";
      return false;
    }
    //std::cout << "<br>Context conversion successful with output "
    //<< convertedExpressions[i].ToString();
    output[i]=convertedExpressions[i].GetValuE<theType>();
  }
  return true;
}

template <class theType>
bool CommandList::GetMatrix
(const Expression& theExpression, Matrix<theType>& outputMat,
 Expression* inputOutputStartingContext,
 int targetNumColsNonMandatory, Expression::FunctionAddress conversionFunction)
{ MemorySaving<Expression> tempContext;
  Expression& startContext=
  inputOutputStartingContext==0 ? tempContext.GetElement() : *inputOutputStartingContext;
  Expression theConverted;
//  std::cout << "<br>extracting matrix from: " << theExpression.Lispify();
  if (!theExpression.IsSequenceNElementS())
  { if (targetNumColsNonMandatory>0)
      if (targetNumColsNonMandatory!=1)
        return false;
    theConverted=theExpression;
    if (!theConverted.IsOfType<theType>())
    { if (conversionFunction!=0)
        if (!this->CallCalculatorFunction(conversionFunction, theExpression, theConverted))
          return false;
      if (!theConverted.IsOfType<theType>())
        return false;
    }
    if (!theConverted.SetContextAtLeastEqualTo(startContext))
      return false;
    outputMat.init(1,1);
    outputMat(0,0)=theConverted.GetValuE<theType>();
    return true;
  }
  Vector<theType> currentRow;
  Expression currentContext;
  currentContext.MakeEmptyContext(*this);
  if (!startContext.IsContext())
    startContext.MakeEmptyContext(*this);
  int numRows=theExpression.children.size-1;
  for (int i=0; i<numRows; i++)
  { const Expression& currentE=theExpression[i+1];
    if (!this->GetVectoR
        (currentE, currentRow, &currentContext, targetNumColsNonMandatory, conversionFunction))
    { this->Comments << "<hr>Failed to GetVector() while extracting context from expression "
      << currentE.ToString() << " with context " << currentContext.ToString() << "<hr>";
      return false;
    }
//    std::cout << "<hr>Merging " << currentContext.ToString() << " with " << startContext.ToString();
    if (!currentContext.ContextMergeContexts(currentContext, startContext, startContext))
    { this->Comments << "<hr>Failed to merge context "
      << currentContext.ToString() << " with context " << startContext.ToString() << "<hr>";
      return false;
    }
  }
//  std::cout << "<hr>The context from all rows is: " << startContext.ToString();
  for (int i=0; i<numRows; i++)
  { const Expression& currentE=theExpression[i+1];
    if (!this->GetVectoR
        (currentE, currentRow, &startContext, targetNumColsNonMandatory, conversionFunction))
    { this->Comments << "<hr>Failed to GetVector() while converting expression " <<
      currentE.ToString() << " with context " << startContext.ToString() << "<hr>";
      return false;
    }
    if (i==0)
    { targetNumColsNonMandatory=currentRow.size;
      outputMat.init(numRows, targetNumColsNonMandatory);
    }
    for (int j=0; j<targetNumColsNonMandatory; j++)
      outputMat(i,j)=currentRow[j];
  }
  return true;
}
#endif
