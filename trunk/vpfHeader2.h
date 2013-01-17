//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader2_h_already_included
#define vpfHeader2_h_already_included

//the following  include contains all the c++ math routines used in the calculator.
#include "vpfHeader1_3.h"
static ProjectInformationInstance ProjectInfoVpfHeader2(__FILE__, "Header file containing the calculator's parsing routines. ");

class Function
{
  public:
  //the functions may not possess a name.
  //If you want your function to possess a name, use a VariableNonBound which has a member function handler.
  std::string theName;
  MemorySaving<List<std::string> > theArgumentList;
  std::string theDescription;
  std::string theExample;
  bool flagNameIsVisible;
  bool flagMayActOnBoundVars;
  MemorySaving<List<Expression> > theArgumentPatterns;
  MemorySaving<List<bool> > theArgumentPatternIsParsed;
  typedef bool (*FunctionAddress)
  (CommandList& theCommands, const Expression& input, Expression& output);
  FunctionAddress theFunction;
  std::string ToString(CommandList& theBoss)const;
  void operator =(const Function& other)
  { this->theName=other.theName;
    this->theArgumentList=other.theArgumentList;
    this->theDescription=other.theDescription;
    this->theExample=other.theExample;
    this->theFunction=other.theFunction;
    this->flagNameIsVisible=other.flagNameIsVisible;
    this->flagMayActOnBoundVars=other.flagMayActOnBoundVars;
  }
  bool operator==(const Function& other)const
  { return this->theArgumentList==other.theArgumentList &&
    this->theFunction==other.theFunction && this->theName==other.theName;
  }
  Function(){this->theFunction=0;}
  Function
  (const Function::FunctionAddress& functionPointer, const std::string& functionName, const std::string& argumentList,
   const std::string& description, const std::string& inputExample, bool inputflagNameIsVisible, bool inputflagMayActOnBoundVars=false)
  { this->theFunction=functionPointer;
    this->theName=functionName;
    this->theDescription=description;
    this->theExample=inputExample;
    if (argumentList!="")
    { this->theArgumentList.GetElement().AddOnTop(argumentList);
      this->theArgumentPatternIsParsed.GetElement().AddOnTop(false);
      this->theArgumentPatterns.GetElement().SetSize(1);
    }
    this->flagNameIsVisible=inputflagNameIsVisible;
    this->flagMayActOnBoundVars=inputflagMayActOnBoundVars;
  }
  inline static unsigned int HashFunction(const Function& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return (unsigned int) MathRoutines::hashString(this->theName);
  }
};

class Expression
{ void reset()
  { this->theBoss=0;
    this->children.size=0;
    this->theData=-1;
    this->format=this->formatDefault;
//    this->IndexBoundVars=inputIndexBoundVars;
  }
  public:
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
  //     CommandList::opList().*
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
  //2.5. A string is a list with two entries
  //     whose first entry is an atom equal to String and whose second entry
  //     is an integer that uniquely identifies the string.
  //     In the current implementation, the integer is the index in the object container.
  //2.6. A rational number is a list with two entries
  //     whose first entry is an atom equal to Rational and
  //     whose second entry is an integer that uniquely identifies the rational number.
  //     In the current implementation, the integer is the index in the object container.
  //2.7. An error is a list with two entries whose first entry is an atom equal to Error,
  //     and whose second entry is a string.
  //*Note that CommandList::opList() is required to equal zero for reasons of program speed.
  //However you MAY NOT assume that: you should always call CommandList::opList() explicitly.
  //Instead, if you want to have a list of mathematical objects, use the Sequence
  //data structure. A sequence is a List whose first entry is an atom whose value
  //is opSequence.
  int theData;
  List<Expression> children;
  CommandList* theBoss;
  ///////////////////////////////////////
  //two objects are considered equal even when the the following data is different:
  int format;
  friend std::ostream& operator << (std::ostream& output, const Expression& theMon)
  { output << theMon.ToString();
    return output;
  }
  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot, formatNoBracketsForFunctionArgument, formatMatrix, formatMatrixRow
  };
  typedef  bool (*OperationCruncher)
  (const Expression& left, const Expression& right, Expression& output);
  void reset(CommandList& newBoss, int newNumChildren=0)
  { this->theBoss=&newBoss;
    this->theData=-1;
    if (newNumChildren>=0)
      this->children.SetSize(newNumChildren);
  }
  bool AssignChild(int childIndex)
  { Expression tempExp=this->children[childIndex];
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
  bool IsListNElementsStartingWithAtom(int theOp=-1, int N=-1)const
  { if (N!=-1)
      if (this->children.size!=N)
        return false;
    if (!this->children[0].IsAtoM())
      return false;
    if (theOp==-1)
      return true;
    return this->children[0].theData==theOp;
  }
  bool IsListStartingWithAtom(int theOp=-1)const
  { if (!this->IsLisT())
      return false;
    if (!this->children[0].IsAtoM())
      return false;
    if (theOp==-1)
      return true;
    return this->children[0].theData==theOp;
  }
  bool IsListOfTwoAtomsStartingWith(int theOp)const
  { if (!this->IsListStartingWithAtom(theOp))
      return false;
    if (this->children.size!=2)
      return false;
    return this->children[1].IsAtoM();
  }
  bool IsAtoM(int desiredDataUseMinusOneForAny=-1)const
  { if (this->IsLisT())
      return false;
    if (desiredDataUseMinusOneForAny==-1)
      return true;
    return this->theData==desiredDataUseMinusOneForAny;
  }
  Expression& operator[](int n)const
  { return this->children[n];
  }
  bool IsSequenceNElementS(int N=-2)const;
  bool IsError(std::string* outputErrorMessage=0)const;
  bool IsContext()const;

  template <class theType>
  bool ConvertToType(Expression& output);
  template <class theType>
  bool IsOfType(theType* whichElement=0)const
  { bool result=
    this->IsListNElementsStartingWithAtom(this->GetOpType<theType>())
    && this->children.size>1 && this->children.LastObject()->IsAtoM();
    ;
    if (whichElement==0)
      return result;
    *whichElement=this->GetValuE<theType>();
    return result;
  }
  template <class theType>
  const theType& GetValuE()const
  { return this->GetValuENonConstUseWithCaution<theType>();
  }
  template <class theType>
  theType& GetValuENonConstUseWithCaution()const;

  template<class theType>
  int GetOpType()const;
  template<class theType>
  int AddObjectReturnIndex(const theType& inputValue)const;

  //note: the following always returns true:
  template <class theType>
  bool AssignValue(const theType& inputValue, CommandList& owner)
  { int theIndex =this->AddObjectReturnIndex(inputValue);
    this->reset(owner, 2);
    this->children[0].MakeAtom(this->GetOpType<theType>(), owner);
    this->children[1].MakeAtom(theIndex, owner);
    return true;
  }
  //note: the following always returns true:
  template <class theType>
  bool AssignValueWithContext
  (const theType& inputValue, const Expression& theContext, CommandList& owner)
  { int theIndex =this->AddObjectReturnIndex(inputValue);
    this->reset(owner, 3);
    this->children[0].MakeAtom(this->GetOpType<theType>(), owner);
    this->children[1]=theContext;
    this->children[2].MakeAtom(theIndex, owner);
    return true;
  }
  bool SetContextAtLeastEqualTo(Expression& inputOutputMinContext);
  int GetNumContextVariables()const;

  template <class dataType>
  bool ContextGetPolynomialMonomial
  (const Expression& input, dataType& output, GlobalVariables& theGlobalVariables)const;

  Expression GetContext()const;

  Expression ContextGetContextVariable(int variableIndex);
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
  Expression ContextGetSSLieAlg()const;

  void SetContextAmbientAlgebra(int indexInObjectContainer);
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
  void MakeAtom(int input, CommandList& newBoss)
  { this->reset(newBoss);
    this->theData=input;
  }
void MakeVariableNonBounD
  (CommandList& owner, const std::string& varName)
;
  void MakeFunction
  (CommandList& owner, const Expression& argument, const std::string& functionName)
;
  bool EvaluatesToVariableNonBound()const;
  void MakeFunction
  (CommandList& owner, const Expression& argument, int functionIndex)
;
  Function::FunctionAddress GetHandlerFunctionIamNonBoundVar();
  void MakeProducT
  (CommandList& owner, const Expression& left, const Expression& right)
  ;
  void MakeFunction
  (CommandList& owner, const Expression& theFunction, const Expression& theArgument)
  ;
  int GetNumCols()const
  ;
  void MakeXOX
  (CommandList& owner, int theOp, const Expression& left, const Expression& right)
  ;
  std::string Lispify
  ()const
  ;
  std::string ToString
  (FormatExpressions* theFormat=0, std::stringstream* outComments=0,
   Expression* startingExpression=0)const;
  std::string ElementToStringPolishForm(int recursionDepth=0, int maxRecursionDepth=1000);
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
      result+=this->children[i].HashFunctionRecursive(RecursionDepth+1, MaxRecursionDepth)*SomeRandomPrimes[i];
    return result;
  }
  std::string GetOperation()const;
  Expression()
  { this->reset();
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
  bool AreEqualExcludingChildren(const Expression& other) const
  { return
    this->theBoss==other.theBoss &&
    this->theData==other.theData &&
    this->children.size==other.children.size
    ;
  }
//  Rational GetConstantTerm() const;
  bool operator==(const Expression& other)const;
  void CopyValueFromNoChildrenCopy(const Expression& other)
  { this->theBoss=other.theBoss;
    this->theData=other.theData;
    this->children.SetSize(other.children.size);
    this->format=other.format;
//    this->IndexBoundVars=other.IndexBoundVars;
  }
  void operator=(const Expression& other);
  bool operator>(const Expression& other)const;
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
  int IndexFirstChar;
  int IndexLastCharPlusOne;
  std::string errorString;
  Expression theData;
  void operator=(const SyntacticElement& other)
  { this->controlIndex=other.controlIndex;
    this->theData=other.theData;
    this->errorString=other.errorString;
    this->IndexFirstChar=other.IndexFirstChar;
    this->IndexLastCharPlusOne=other.IndexLastCharPlusOne;
  }
  std::string ToString(CommandList& theBoss)const;
  SyntacticElement()
  { this->controlIndex=0;//controlIndex=0 *MUST* point to the empty control sequence.
    this->errorString="";
    this->IndexFirstChar=-1;
    this->IndexLastCharPlusOne=-1;
  }
  SyntacticElement(const SyntacticElement& other)
  { this->operator=(other);
  }
};

class VariableNonBound
{
  public:
  friend std::ostream& operator << (std::ostream& output, const VariableNonBound& theV)
  { output << theV.ToString();
    return output;
  }
  std::string theName;
  int HandlerFunctionIndex;
  inline static unsigned int HashFunction(const VariableNonBound& input)
  { return MathRoutines::hashString(input.theName);
  }
  VariableNonBound(const char x[]):theName(x), HandlerFunctionIndex(-1){}
  VariableNonBound(const std::string& input):theName(input), HandlerFunctionIndex(-1) {}
  VariableNonBound():HandlerFunctionIndex(-1) {}
  std::string GetHandlerFunctionName(CommandList& owner);
  std::string ToString()const{return this->theName;}
  //C++ does have horrible syntax:
//the following returns a pointer to a function that return bool and takes theCommands, commandIndex and Expression
// as input
//[Edit:] masked the horror in a typedef. Now the horror lies in a single isolated line. Nevertheless, a horror.
  Function::FunctionAddress GetFunction
  (CommandList& owner)const
  ;
  unsigned int HashFunction()const
  { return VariableNonBound::HashFunction(*this);
  }
  void operator=(const VariableNonBound& other)
  { this->theName=other.theName;
    this->HandlerFunctionIndex=other.HandlerFunctionIndex;
  }
  bool operator==(const VariableNonBound& other)const
  { return this->theName==other.theName;
  }
};

class ObjectContainer
{ //Following are containers for data structures that are implemented in C++.
  //These objects are dynamically allocated and used by the calculator as requested
  //by various predefined function handlers.
public:
  List<ModuleSSalgebra<RationalFunctionOld> > theCategoryOmodules;
  List<SemisimpleLieAlgebra> theLieAlgebras;
  HashedList<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theTensorElts;
  HashedList<Polynomial<Rational> > thePolys;
  HashedList<ElementUniversalEnveloping<RationalFunctionOld> > theUEs;
  HashedList<RationalFunctionOld> theRFs;
  HashedList<Rational> theRationals;
  HashedList<charSSAlgMod<Rational> > theChars;
  AlgebraicNumberRegistry theAlgebraicNumberRegistry;
  HashedList<AlgebraicNumber> theAlgebraicNumbers;
  HashedList<std::string, MathRoutines::hashString> theStrings;
  HashedList<VariableNonBound> theVariablesNonBound;
  HashedList<std::string, MathRoutines::hashString> ExpressionNotation;
  HashedList<Expression> ExpressionWithNotation;
  HashedList<LittelmannPath> theLSpaths;
//  HashedList<DifferentialForm<Rational> > theDiffForm;
  HashedList<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
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

struct ExpressionPairCruncherIds
{
  int theOp;
  int leftType;
  int rightType;
  bool operator==(const ExpressionPairCruncherIds& other)const
  { return this->leftType==other.leftType && this->rightType==other.rightType && this->theOp==other.theOp;
  }
  void operator=(const ExpressionPairCruncherIds& other)
  { this->leftType=other.leftType;
    this->rightType=other.rightType;
    this->theOp=other.theOp;
  }
  ExpressionPairCruncherIds():theOp(-1), leftType(-1), rightType(-1){}
  ExpressionPairCruncherIds(int inputOp, int inputLeft, int inputRight)
  : theOp(inputOp), leftType(inputLeft), rightType(inputRight)
  {}
  static unsigned int HashFunction(const ExpressionPairCruncherIds& input)
  { return (unsigned int) input.leftType*SomeRandomPrimes[0]+
    (unsigned int) input.rightType*SomeRandomPrimes[1]+
    (unsigned int) input.theOp*SomeRandomPrimes[2];
  }
};

class CommandList
{ template <class dataType>
  bool EvaluatePMTDtree
(Expression& output, const Expression& inputContext, const Expression& input)
  ;

public:
//Calculator functions have as arguments two expressions passed by reference,
//const Expression& input and Expression& output. Calculator functions
//return bool. It is forbidden to pass the same object as input and output.
//If a calculator function returns false this
//means that the calculator failed to evaluate the
//function. If that is the case, the value of output is not specified and
//*MUST NOT* be used.
//If a function returns true this means that output contains the result of the function.
//Note that the output of a function may be of type Error. Error results come, like any other
//result, with a true return from the function.
//The input of a function *MUST* contain the arguments of the function
//in entries of index 1... input.children.size. The first entry (index 0) of input
//must contain the name of the function called.
//Note that functions are not required to check whether the first entry (the function name)
//is as expected. In addition, the name of the function may be used as an additional argument:
//for example, fAssociate is allowed for arbitrary function names.

//control sequences parametrize the syntactical elements
  HashedList<std::string, MathRoutines::hashString> controlSequences;
//operations parametrize the expression elements
//operations are the labels of the nodes of the expression tree
//As operations can be thought of as functions, and functions are named by the class VariableNonBound,
//operations are in fact realized as elements of type VariableNonBound.
  HashedList<VariableNonBound> operations;
  HashedList<Function> theFunctions;

  HashedList<ExpressionPairCruncherIds> theCruncherIds;
  List<Expression::OperationCruncher> theCruncherS;

  List<Expression> buffer1, buffer2;
  int MaxRecursionDeptH;
  int RecursionDeptH;
  int DepthRecursionReached;
  int MaxAlgTransformationsPerExpression;
  int MaxLatexChars;
  int MaxNumCachedExpressionPerContext;
  ///////////////////////////////////////////////////////////////////////////
  bool flagAbortComputationASAP;
  bool flagTimeLimitErrorDetected;
  bool flagFirstErrorEncountered;
  bool flagMaxRecursionErrorEncountered;
  bool flagMaxTransformationsErrorEncountered;
  bool flagNewContextNeeded;

  bool flagLogSyntaxRules;
  ///////////////////////////////////////////////////////////////////////////
  int TotalNumPatternMatchedPerformed;
  int NumPredefinedVars;
  int numEmptyTokensStart;
  Expression theProgramExpression;
//  std::vector<std::stringstream> theLogs;
  int registerNumNonClosedBeginArray;
  int counterInSyntacticSoup;
  List<SyntacticElement> syntacticSouP;
  List<SyntacticElement> syntacticStacK;

  List<SyntacticElement>* CurrrentSyntacticSouP;
  List<SyntacticElement>* CurrentSyntacticStacK;

  HashedList<Expression> cachedExpressions;
  List<Expression> imagesCachedExpressions;
  ////
  HashedList<Expression> ExpressionStack;
  List<HashedList<std::string, MathRoutines::hashString> > NonBoundVariablesStack;
  List<HashedList<std::string, MathRoutines::hashString> > BoundVariablesStack;

  int RuleContextIdentifier;
  List<Expression> RuleStack;

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
  bool IsBoundVarInContext(const std::string& input);
  bool IsNonBoundVarInContext(const std::string& input);
  SyntacticElement GetSyntacticElementEnd()
  { SyntacticElement result;
    result.controlIndex=this->controlSequences.GetIndex(";");
    return result;
  }
  bool DecreaseStackSetCharacterRanges(int decrease)
  { if (decrease<=0)
      return true;
    assert((*this->CurrentSyntacticStacK).size-decrease>0);
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-1].IndexLastCharPlusOne=
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].IndexLastCharPlusOne;
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
  bool isSeparatorFromTheLeftForStatement(const std::string& input);
  bool isSeparatorFromTheLeftForList(const std::string& input);
  bool isSeparatorFromTheLeftForListMatrixRow(const std::string& input);
  bool isSeparatorFromTheLeftForMatrixRow(const std::string& input);

  bool isSeparatorFromTheRightGeneral(const std::string& input);
  bool isSeparatorFromTheRightForDefinition(const std::string& input);
  bool isSeparatorFromTheRightForStatement(const std::string& input);
  bool isSeparatorFromTheRightForList(const std::string& input);
  bool isSeparatorFromTheRightForListMatrixRow(const std::string& input);
  bool isSeparatorFromTheRightForMatrixRow(const std::string& input);
  void RegisterCruncherNoFail
  (int theOp, int inputLeftType, int inputRightType, Expression::OperationCruncher inputCruncher)
  { ExpressionPairCruncherIds epc(inputLeftType, inputRightType, theOp);
    if (this->theCruncherIds.Contains(epc))
    { std::cout
      << "This is a programming error: attempting to add more than one handler for the same "
      << "pair of data types. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->theCruncherIds.AddOnTop(epc);
    this->theCruncherS.AddOnTop(inputCruncher);
  }
  void RegisterDivCruncherNoFail
  (int inputLeftType, int inputRightType, Expression::OperationCruncher inputCruncher)
  { this->RegisterCruncherNoFail(this->opDivide(), inputLeftType, inputRightType, inputCruncher);
  }
  void RegisterMultiplicativeDataCruncherNoFail
  (int inputLeftType, int inputRightType, Expression::OperationCruncher inputCruncher)
  { this->RegisterCruncherNoFail(this->opTimes(), inputLeftType, inputRightType, inputCruncher);
  }
  void RegisterAdditiveDataCruncherNoFail
  (int inputLeftType, int inputRightType, Expression::OperationCruncher inputCruncher)
  { this->RegisterCruncherNoFail(this->opPlus(), inputLeftType, inputRightType, inputCruncher);
  }
  Expression::OperationCruncher GetMultiplicativeCruncher
  (int inputLeftType, int inputRightType)
  { return this->GetOpCruncher(this->opTimes(), inputLeftType, inputRightType);
  }
  Expression::OperationCruncher GetAdditiveCruncher(int inputLeftType, int inputRightType)
  { return this->GetOpCruncher(this->opPlus(), inputLeftType, inputRightType);
  }
  Expression::OperationCruncher GetAdditiveCruncher
  (const Expression& leftE, const Expression& rightE);
  Expression::OperationCruncher GetOpCruncher
  (int theOp, int inputLeftType, int inputRightType)
  { ExpressionPairCruncherIds d(theOp, inputLeftType, inputRightType);
    int theIndex=this->theCruncherIds.GetIndex(d);
    if (theIndex==-1)
      return 0;
    return this->theCruncherS[theIndex];
  }
  Function::FunctionAddress GetfOp
  (int theOp, const Expression& left, const Expression& right)
;
  bool LookAheadAllowsThePower(const std::string& lookAhead)
  { return lookAhead!="{}";
  }
  bool LookAheadAllowsApplyFunction(const std::string& lookAhead)
;
  bool LookAheadAllowsPlus(const std::string& lookAhead)
  ;
  bool LookAheadAllowsTimes(const std::string& lookAhead);
  bool LookAheadAllowsTensor(const std::string& lookAhead);
  bool LookAheadAllowsDivide(const std::string& lookAhead);
  bool PopTopSyntacticStack()
  { (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-1);
    return true;
  }
  bool ReplaceEXXEXEByEusingO(int theOp);
  bool ReplaceEEByEusingO(int theControlIndex);
  bool ReplaceEEndCommandEbyE();
  bool ReplaceOEByE(int formatOptions=Expression::formatDefault);
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
  bool ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int inputFormat=Expression::formatDefault, int numXs=0);
  bool ReplaceSequenceXEBySequence(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceSequenceXEYBySequenceY(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceCEByC();
  bool ReplaceCCByC();
  bool ReplaceEOEByE(int formatOptions=Expression::formatDefault)
  { return this->ReplaceEXEByEusingO((*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex, formatOptions);
  }
  bool ReplaceXEXEXByEusingO(int theOperation, int formatOptions=Expression::formatDefault)
  ;
  bool ReplaceEXEByEusingO(int theOperation, int formatOptions=Expression::formatDefault)
  ;
  bool ReplaceXXByCon(int theCon, int theFormat=Expression::formatDefault)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=theCon;
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=theFormat;
    this->DecreaseStackSetCharacterRanges(1);
    return true;
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
  bool ReplaceXYYXByYY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-2);
    return true;
  }
  bool ReplaceXYXByY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXYByY()
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
    (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-1);
    return true;
  }
  bool ReplaceXEXByE(int inputFormat=Expression::formatDefault)
  { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat;
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceAbyE();
  bool ReplaceOXbyEX(int inputFormat=Expression::formatDefault);
  bool ReplaceOXbyEXusingO(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceOXbyE(int inputFormat=Expression::formatDefault);
  bool ReplaceVXdotsXbyEXdotsX(int numXs);
  bool ReplaceVXbyEX(){return this->ReplaceVXdotsXbyEXdotsX(1);}
  bool ReplaceVbyE(){return this->ReplaceVXdotsXbyEXdotsX(0);}
  bool ReplaceIntIntBy10IntPlusInt()
  ;
  void MakeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output);
  bool RegisterBoundVariable();
  int GetOperationIndexFromControlIndex(int controlIndex);
  int GetExpressionIndex();
  SyntacticElement GetEmptySyntacticElement();
  bool ApplyOneRule(const std::string& lookAhead);
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
  int opSequence()
  { return this->operations.GetIndexIMustContainTheObject("Sequence");
  }
  int opRational()
  { return this->operations.GetIndexIMustContainTheObject("Rational");
  }
  int opAlgNumber()
  { return this->operations.GetIndexIMustContainTheObject("AlgebraicNumber");
  }
  int opPoly()
  { return this->operations.GetIndexIMustContainTheObject("Polynomial<Rational>");
  }
  int opRationalFunction()
  { return this->operations.GetIndexIMustContainTheObject("RationalFunction");
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
  { return this->operations.GetIndexIMustContainTheObject("SSLieAlg");
  }
  int opLittelmannPath()
  { return this->operations.GetIndexIMustContainTheObject("LittelmannPath");
  }
  int opLRO()
  { return this->operations.GetIndexIMustContainTheObject("LRO");
  }
  int opUnion()
  { return this->operations.GetIndexIMustContainTheObject("\\cup");
  }
  int opPolynomialVariables()
  { return this->operations.GetIndexIMustContainTheObject("PolyVars");
  }
  int opEndStatement()
  { return this->operations.GetIndexIMustContainTheObject(";");
  }
  int opUnionNoRepetition()
  { return this->operations.GetIndexIMustContainTheObject("\\sqcup");
  }
  int opContext()
  { return this->operations.GetIndexIMustContainTheObject("Context");
  }
  int opBind()
  { return this->operations.GetIndexIMustContainTheObject("Bind");
  }
  int opVariableNonBound()
  { return this->operations.GetIndexIMustContainTheObject("NonBound");
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
  (Expression& theExpression, List<Expression>& output)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus());
  }
  void SpecializeBoundVars
(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs)
  ;
  Expression* PatternMatch
  (Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
   Expression* condition=0, std::stringstream* theLog=0, bool logAttempts=false)
;
  bool ProcessOneExpressionOnePatternOneSub
  (Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
    std::stringstream* theLog=0, bool logAttempts=false)
  ;
  bool isADigit(const std::string& input, int& whichDigit)
  { if (input.size()!=1)
      return false;
    whichDigit=input[0]-'0';
    return whichDigit<10 && whichDigit>=0;
  }
//  bool OrderMultiplicationTreeProperly(int commandIndex, Expression& theExpression);
  bool fCollectSummands(Expression& theExpression);
  template <class theType>
  bool CallConversionFunctionReturnsNonConstUseCarefully
  (Function::FunctionAddress theFun, const Expression& input, theType*& outputData,
   std::string* outputError=0)
  { Expression tempE;
    if (!theFun(*this, input, tempE))
    { if (outputError!=0)
        *outputError="I was unsuccessful in callin the conversion function.";
      return false;
    }
    if (tempE.IsOfType<theType>())
      return true;
    if (!tempE.IsError(outputError))
      if (outputError!=0)
        *outputError="Successfully called the conversion function but did not get the desired type.";
    return false;
  }
  bool CallCalculatorFunction
  (Function::FunctionAddress theFun, const Expression& input, Expression& output)
  { if (!theFun(*this, input, output))
      return false;
    return !output.IsError();
  }
bool CollectSummands
(List<Expression>& summands, bool needSimplification, Expression& theExpression)
;
  bool ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions,
   std::stringstream* theLog=0)
  ;
  bool ExpressionsAreEqual
  (const Expression& left, const Expression& right)
  ;

  static bool fUnion
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
static bool EvaluateCarryOutActionSSAlgebraOnGeneralizedVermaModule
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool fStandardFunction
(CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fUnionNoRepetition
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fPlus
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fStandardTimes
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool StandardTensor
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fDivide
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool StandardIsDenotedBy
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fExpressionHasBoundVars
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool StandardLieBracket
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fMinus
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool StandardEqualEqual
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fAssociate
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fExtractBaseMultiplication
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fDistribute
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fThePower
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.fOperationBinary(theCommands, input, output, theCommands.opThePower());
  }
  static bool fOperationBinary
  (CommandList& theCommands, const Expression& input, Expression& output, int theOperation)
  ;
  static bool fLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fRightDistributeBracketIsOnTheRight
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool EvaluateIf
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  template<class theType>
  bool fGetMatrix
  (const Expression& theExpression, Matrix<theType>& outputMat,
   Expression* inputOutputStartingContext=0,
   int targetNumColsNonMandatory=-1, Function::FunctionAddress conversionFunction=0)
  ;
  template <class theType>
  bool GetVector
  (const Expression& theExpressioN, Vector<theType>& output,
   Expression* inputOutputStartingContext=0, int targetDimNonMandatory=-1,
   Function::FunctionAddress conversionFunction=0)
  ;
  template <class dataType>
  static bool fExtractPMTDtreeContext
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  template <class dataType>
  static bool fExtractAndEvaluatePMTDtree
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;

  static bool fMatrix
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fDet
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fInvertMatrix
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fDrawPolarRfunctionTheta
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fSuffixNotationForPostScript
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fIsInteger
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fFreudenthalEval
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
    static bool fGCDOrLCM
  (CommandList& theCommands, const Expression& input, Expression& output, bool doGCD)
  ;
  static bool fLCM
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.fGCDOrLCM(theCommands, input, output, false);
  }
  static bool fGCD
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.fGCDOrLCM(theCommands, input, output, true);
  }
  static bool fPolynomialDivisionQuotient
  (CommandList& theCommands, const Expression& input, Expression& output)
  {return theCommands.fPolynomialDivisionQuotientRemainder(theCommands, input, output, true);
  }
  static bool fPolynomialDivisionRemainder
  (CommandList& theCommands, const Expression& input, Expression& output)
  {return theCommands.fPolynomialDivisionQuotientRemainder(theCommands, input, output, false);
  }
  static bool fPolynomialDivisionQuotientRemainder
(CommandList& theCommands, const Expression& input, Expression& output, bool returnQuotient)
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
  static bool fPolynomial
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fDecomposeFDPartGeneralizedVermaModuleOverLeviPart
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  bool fSplitFDpartB3overG2Init
(CommandList& theCommands, const Expression& input, Expression& output,
 branchingData& theG2B3Data, Expression& outputContext)
 ;
  static bool fSplitFDpartB3overG2CharsOnly
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fElementUniversalEnvelopingAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fElementTensorGeneralizedVermas
  (CommandList& theCommands, const Expression& input, Expression& output)
  ;
  static bool fSSAlgebraShort
  (CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.fSSAlgebra(theCommands, input, output, false);
}
  static bool fSSAlgebraVerbose
  (CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.fSSAlgebra(theCommands, input, output, true);
}
template<class CoefficientType>
bool fGetTypeHighestWeightParabolic
(CommandList& theCommands, const Expression& input, Expression& output,
 Vector<CoefficientType>& outputWeightHWFundcoords, Selection& outputInducingSel,
 Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra)
 ;
 static bool fGroebnerGrLex
(CommandList& theCommands, const Expression& input, Expression& output)
 { return theCommands.fGroebner
  (theCommands, input, output, true);
 }
 static bool fGroebnerLex
(CommandList& theCommands, const Expression& input, Expression& output)
 { return theCommands.fGroebner
  (theCommands, input, output, false);
 }
 static bool fGroebner
(CommandList& theCommands, const Expression& input, Expression& output, bool useGr)
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
  static bool fEmbedSSalgInSSalg
(CommandList& theCommands, const Expression& input, Expression& output)
 ;
  static bool fWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
 ;
  static bool fWeylOrbitFund
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.fWeylOrbit(theCommands, input, output, true, false);
}
  static bool fWeylOrbitSimple
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.fWeylOrbit(theCommands, input, output, false, false);
}
  static bool fWeylOrbitFundRho
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.fWeylOrbit(theCommands, input, output, true, true);
}
  static bool fSSAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.fSSAlgebra(theCommands, input, output, false);
  }
  static bool fSSAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output, bool Verbose)
;
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
  static bool fDrawWeightSupportWithMults
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool fDrawWeightSupport
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
  static bool fHWVinner
(CommandList& theCommands, Expression& output,
 Vector<RationalFunctionOld>& highestWeightFundCoords,
 Selection& selectionParSel, Expression& hwContext, int indexOfAlgebra)
 ;
 bool fWriteGenVermaModAsDiffOperatorInner
(CommandList& theCommands, const Expression& input, Expression& output,
  Vectors<Polynomial<Rational> >& theHws, Expression& hwContext,
  Selection& selInducing, int indexOfAlgebra)
  ;
  template<class CoefficientType>
static bool TypeHighestWeightParabolic
(CommandList& theCommands, const Expression& input, Expression& output,
 Vector<CoefficientType>& outputWeight,
 Selection& outputInducingSel, Expression* outputContext=0)
;
  static bool fHWV
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
  static bool fRootSAsAndSltwos
  (CommandList& theCommands, const Expression& input, Expression& output, bool showSLtwos)
;
  static bool fprintRootSAs
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.fRootSAsAndSltwos
    (theCommands, input, output, false);
  }
  static bool fprintSltwos
  (CommandList& theCommands, const Expression& input, Expression& output)
  { return theCommands.fRootSAsAndSltwos
    (theCommands, input, output, true);
  }
  static bool fSSsubalgebras
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  void AddEmptyHeadedCommand();
  CommandList();
  void AddOperationNoFail
  (const std::string& theOpName, const Function::FunctionAddress& theFunAddress, const std::string& opArgumentList,
   const std::string& opDescription, const std::string& opExample, bool inputNameUsed)
   ;
  void AddNonBoundVarMustBeNew
  (const std::string& theName, const Function::FunctionAddress& funHandler,
   const std::string& argList, const std::string& description, const std::string& exampleArgs,
   bool visible=true)
   ;
//  { int theIndex=this->AddNonBoundVarReturnVarIndex(theName, funHandler, argList, description, exampleArgs);
//    if (theIndex!=this->theVariablesNonBound.size-1)
//      assert(false);
//  }
  Function::FunctionAddress GetFunctionAddressFromOperation
  (int theOp)
;

  int AddNonBoundVarReturnVarIndex
(const std::string& theName, const Function::FunctionAddress& funHandler,
  const std::string& argList, const std::string& description, const std::string& exampleArgs)
;
  void init(GlobalVariables& inputGlobalVariables);
  void initCrunchers();
  void initPredefinedVars()
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

template <class theType>
bool CommandList::GetVector
(const Expression& theExpressioN, Vector<theType>& output, Expression* inputOutputStartingContext,
 int targetDimNonMandatory, Function::FunctionAddress conversionFunction)
{ MemorySaving<Expression> tempContext;
  Expression& startContext=
  inputOutputStartingContext==0 ? tempContext.GetElement() : *inputOutputStartingContext;
  Expression theConverted;
  if (!theExpressioN.IsSequenceNElementS())
  { if (targetDimNonMandatory>0)
      if (targetDimNonMandatory!=1)
        return false;
    theConverted=theExpressioN;
    if (!theExpressioN.IsOfType<theType>())
    { if (conversionFunction==0)
        return false;
      this->CallCalculatorFunction(conversionFunction, theExpressioN, theConverted);
      if (!theConverted.IsOfType<theType>())
        return false;
    }
    if (!theConverted.SetContextAtLeastEqualTo(startContext))
      return false;
    output.SetSize(1);
    output[0]=theConverted.GetValuE<theType>();
    return true;
  }
  if (targetDimNonMandatory>0)
    if (targetDimNonMandatory!=theExpressioN.children.size-1)
      return false;
  targetDimNonMandatory=theExpressioN.children.size-1;
  output.SetSize(targetDimNonMandatory);
  for (int i=0; i<targetDimNonMandatory; i++)
  { Expression& currentE=theExpressioN.children[i+1];
    theConverted=currentE;
    if (!currentE.IsOfType<theType>())
    { if (conversionFunction==0)
        return false;
      this->CallCalculatorFunction(conversionFunction, currentE, theConverted);
      if (!currentE.IsOfType<theType>())
        return false;
    }
    if (!theConverted.SetContextAtLeastEqualTo(startContext))
      return false;
    output[i-1]=theConverted.GetValuE<theType>();
  }
  return true;
}

template <class theType>
bool CommandList::fGetMatrix
(const Expression& theExpression, Matrix<theType>& outputMat,
 Expression* inputOutputStartingContext,
 int targetNumColsNonMandatory, Function::FunctionAddress conversionFunction)
{ MemorySaving<Expression> tempContext;
  Expression& startContext=
  inputOutputStartingContext==0 ? tempContext.GetElement() : *inputOutputStartingContext;
  Expression theConverted;
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
  int numRows=theExpression.children.size-1;
  for (int i=0; i<numRows; i++)
  { Expression& currentE=theExpression.children[i+1];
    if (!this->GetVector
        (currentE, currentRow, &startContext, targetNumColsNonMandatory,
        conversionFunction))
      return false;
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
