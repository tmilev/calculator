//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader2_h_already_included
#define vpfHeader2_h_already_included

//the following  include contains all the c++ math routines used in the calculator.
#include "vpfHeader1_3.h"
static ProjectInformationInstance ProjectInfoVpfHeader2(__FILE__, "Header file containing the calculator's parsing routines. ");
class Context;

class Data
{
public:
  CommandList* owner;
  int theIndex;
  int theContextIndex;
  int type;
  MemorySaving<std::string> theError;
//  MemorySaving<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theElementTensorGenVermas;
  enum DataType
  { typeError=1, typeZmodP, typeRational, typePoly, typeRationalFunction, typeSSalgebra,
    typeEltTensorGenVermasOverRF, typeMonomialGenVerma, typeElementUE, typeEltSumGenVermas,
    typeLSpath, typeLittelmannRootOperator, typeString, typeVariableNonBound,
    typeCharSSalgFinite,
    typeDifferentialForm, typeRationalRadical
  };
  void operator=(const Data& other);
  bool operator==(const Data& other)const;
  unsigned int HashFunction()const;
  static inline unsigned int HashFunction(const Data& input)  {return input.HashFunction();}
  Data (const Rational& x, CommandList& inputOwner);
  Data(CommandList& theOwner): theIndex(-1), theContextIndex(-1){this->type=this->typeError; this->owner=&theOwner;}
  Data():owner(0), theIndex(-1), theContextIndex(-1){this->type=this->typeError;}
  Data(const Data& otherData) {this->operator=(otherData);}
  bool SetError(const std::string& inputError);
  bool IsEqualToOne()const;
  void MakeLittelmannRootOperator
(int inputIndex, CommandList& theBoss)
  ;
  void MakeLSpath
  (CommandList& theBoss, SemisimpleLieAlgebra& owner, List<Vector<Rational> >& waypts)
  ;
  void MakeSSAlgebra
  (CommandList& theBoss, char WeylLetter, int WeylRank)
  ;
  void MakeChar
  (CommandList& theBoss, charSSAlgMod<Rational>& theChar)
  ;
  void MakeSSAlgebra
  (CommandList& theBoss, const Matrix<Rational>& cartanSymmetric)
  ;
  void MakeRational
(CommandList& theBoss, const Rational& inputRational)
;
  void MakeRationalRadical
(CommandList& theBoss, const AlgebraicNumber& inputRationalRad)
;
  void MakeRF
(CommandList& theBoss, const RationalFunctionOld& inputRF, int inputContextIndex)
;
  template<class dataType>
  void Make(CommandList& theBoss, const dataType& input)
  ;
  void MakeUE
(CommandList& theBoss, const ElementUniversalEnveloping<RationalFunctionOld>& inputUE, int inputContextIndex)
;
  void MakeVariableNonBound
(CommandList& theBoss, const VariableNonBound& input)
;
  void MakeString
(CommandList& theBoss, const std::string& inputString, const Context& inputContext)
;
  void MakeString
(CommandList& theBoss, const std::string& inputString, int contextIndex)
;
  void MakePoly
(CommandList& theBoss, const Polynomial<Rational>& inputPoly, int inputContextIndex)
;
  void MakeElementTensorGeneralizedVermas
(CommandList& theBoss, ElementTensorsGeneralizedVermas<RationalFunctionOld>& theElt, int inputContextIndex)
  ;
  int GetNumContextVars()const;
  void reset(CommandList& inputOwner)
  { this->owner=&inputOwner;
    this->theIndex=-1;
    this->theContextIndex=-1;
    this->type=-1;
    this->theError.FreeMemory();
  }
  bool MakeElementSemisimpleLieAlgebra
(CommandList& inputOwner, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners,
 int theDisplayIndex, std::stringstream* comments)
  ;
  static bool MergeContexts
  (Data& left, Data& right)
  ;
  bool SetContextResizesContextArray(const Context& inputContext);
  const Context& GetContext()const;
  bool MakeElementSemisimpleLieAlgebra
(CommandList& inputOwner, int inputIndexInOwners, int index1, int index2, std::stringstream* comments)
 ;
  const ElementUniversalEnveloping<RationalFunctionOld>& GetUE()const;
  const RationalFunctionOld& GetRF()const;
  int GetIndexAmbientSSLieAlgebra()const;
  SemisimpleLieAlgebra& GetAmbientSSAlgebra()const;
  template<class theType>
  bool IsOfType()const;

  template<class theType>
  bool ConvertToTypE
  ()
  ;
  bool MergeContext
  (Context& outputInputContext);
  template<class theType>
  bool MergeContextsAndConvertToType
  (Context& outputInputContext)
  ;
  template<class theType>
  bool ConvertToTypeResizesContextArrays
  (const Context& desiredNewContext)
  ;

  template<class theType>
  theType GetValueCopy()const;
  template<class theType>
  const theType& GetValuE()const;
  bool IsEqualToZero()const;
  bool IsSmallInteger(int* whichInteger=0)const
  ;
  bool Add(const Data& right, Data& output)const
  ;
  bool Exponentiate(const Data& right, Data& output)const
  ;
  bool IsInteger()const;
  ElementSemisimpleLieAlgebra<Rational>& GetEltSimpleLieAlgebra()const;
  int GetSmallInT()const;
  MonomialGeneralizedVerma<RationalFunctionOld>& GetMonGenVerma()const;
  bool operator+=(const Data& other);
  bool operator*=(const Rational& other);
  bool operator/=(const Data& other);
  bool operator*=(const Data& other);
  std::string ToString(std::stringstream* comments=0, bool isFinal=true, FormatExpressions* inputFormat=0)const;
  std::string ElementToStringDataType()const;
  bool operator!=(const Data& other)const;
  static bool LieBracket
  (const Data& left, const Data& right, Data& output, std::stringstream* comments)
  ;
  bool OperatorDereference
  (const Data& argument, Data& output, std::stringstream* comments)const
  ;
  bool OperatorDereference
  (const Data& argument1, const Data& argument2, Data& output, std::stringstream* comments)const
  ;
  static bool DivideRFOrPolyOrRatByRFOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyLRObyLSPath(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyLRObyLRO(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyRatOrPolyOrRFByRatOrPolyOrRF(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyRatOrPolyByRatOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyAnyByEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyRatOrAlgebraicByRatOrAlgebraic
  (const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyEltTensorByCoeff(const Data& left, const Data& right, Data& output, std::stringstream* comments=0)
  { if (right.type==Data::typeRational || right.type==Data::typePoly || right.type==Data::typeRationalFunction)
      return Data::MultiplyAnyByEltTensor(right, left, output, comments);
    return false;
  }
  static bool MultiplyUEByAny(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyAnyByUE(const Data& left, const Data& right, Data& output, std::stringstream* comments=0)
  { if (left.type==Data::typeRational || left.type==Data::typePoly || left.type==Data::typeRationalFunction)
      return Data::MultiplyUEByAny(right, left, output, comments);
    return false;
  }
  static bool AddRatOrAlgebraicToRatOrAlgebraic(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool AddUEToAny(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool AddEltTensorToEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool AddRatOrPolyToRatOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool AddRatOrPolyOrRFToRatOrPolyOrRF(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool TensorAnyByEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);

  Data operator/(const Data& right)const;
  Data operator*(const Data& right)const;
};

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
  typedef  bool (*FunctionAddress)(CommandList& theCommands, Expression& theExpression, std::stringstream* comments);
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
    this->theOperation=-1;
    this->theDatA=-1;
    this->errorString="";
    this->format=this->formatDefault;
//    this->IndexBoundVars=inputIndexBoundVars;
  }
  public:
  int theOperation;
  int theDatA;
  List<Expression> children;
  std::string errorString;
  ///////////////////////////////////////
  //two objects are considered equal even when the the following data is different:
  int format;
//  int IndexBoundVars;
  CommandList* theBoss;
  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot, formatNoBracketsForFunctionArgument, formatMatrix, formatMatrixRow
  };
  void reset(CommandList& newBoss)
  { this->theBoss=&newBoss;
  }
  void AssignChild(int childIndex)
  { Expression tempExp=this->children[childIndex];
    this->operator=(tempExp);
  }
  const Data& GetAtomicValue()const;
  Rational GetRationalValue()const;
  void MakeMonomialGenVerma
  (const MonomialGeneralizedVerma<RationalFunctionOld>& inputMon, CommandList& newBoss)
 ;
  void MakeElementTensorsGeneralizedVermas
  (const ElementTensorsGeneralizedVermas<RationalFunctionOld>& inputMon, CommandList& newBoss)
 ;
  void MakePolyAtom
(const Polynomial<Rational>& inputData, int inputContextIndex, CommandList& newBoss)
  ;
  void MakePolY
(const Polynomial<Rational>& inputData, int inputContextIndex, CommandList& newBoss)
  ;
  void MakeRFAtom(const RationalFunctionOld& inputData, int inputContextIndex, CommandList& newBoss);
  void MakeAtom(const Data& inputData, CommandList& newBoss)
  ;
  void MakeAtom(const Rational& inputRat, CommandList& newBoss)
  ;
  void MakeAtom(int input, CommandList& newBoss)
  { this->MakeAtom((Rational) input, newBoss);
  }
  void MakeAtom(const VariableNonBound& input, CommandList& newBoss)
  ;
  bool MakeStringAtom
(CommandList& newBoss, const std::string& theString, const Context& inputContext)
;
  bool MakeStringAtom
(CommandList& newBoss, const std::string& theString)
;
  void MakeInt(int theInt, CommandList& newBoss)
  ;
void MakeVariableNonBounD
  (CommandList& owner, const std::string& varName)
;
  void MakeFunction
  (CommandList& owner, const Expression& argument, const std::string& functionName)
;
  bool EvaluatesToAtom()const;
  bool EvaluatesToVariableNonBound()const;
  void MakeFunction
  (CommandList& owner, const Expression& argument, int functionIndex)
;
  Function::FunctionAddress GetFunctionAddressFromVarName();
  Function& GetFunctionFromVarNamE();
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
    int result=(this->theOperation+1)*SomeRandomPrimes[0]+(this->theDatA+SomeRandomPrimes[1])*SomeRandomPrimes[2];
    int numCycles=MathRoutines::Minimum(this->children.size, SomeRandomPrimesSize);
    for (int i=0; i<numCycles; i++)
      result+=this->children[i].HashFunctionRecursive(RecursionDepth+1, MaxRecursionDepth)*SomeRandomPrimes[i];
    return result;
  }
  std::string GetOperation()const;
  Expression()
  { this->reset();
  }
  inline bool SetError (const std::string& theError)
  { Data tempData(*this->theBoss);
    tempData.SetError(theError);
    this->MakeAtom(tempData, *this->theBoss);
    this->errorString=theError;
    return true;
  }
  Expression(const Expression& other)
  { this->operator=(other);
  }
  bool HasBoundVariables();
//  bool IsRationalAtom()const;
  bool EvaluatesToRational(Rational* whichRational=0)const;
  bool IsString()const;
  bool IsElementUE()const;
  bool IsInteger()const;
  bool IsSmallInteger(int* whichinteger)const;
  bool EvaluatesToSmallInteger(int* whichInteger=0)const;
  bool AreEqualExcludingChildren(const Expression& other) const
  { return
    this->theBoss==other.theBoss &&
    this->theDatA==other.theDatA &&
    this->theOperation==other.theOperation &&
    this->children.size==other.children.size &&
    this->errorString==other.errorString
    ;
  }
  Rational GetConstantTerm() const;
  bool operator==(const Expression& other)const;
  void CopyValueFromNoChildrenCopy(const Expression& other)
  { this->theBoss=other.theBoss;
    this->theDatA=other.theDatA;
    this->theOperation=other.theOperation;
    this->errorString=other.errorString;
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

class Context
{ public:
  HashedList<Expression> VariableImages;
  int indexAmbientSSalgebra;
  CommandList* theOwner;
  Context(CommandList& inputOwner):indexAmbientSSalgebra(-1)
  { this->theOwner=&inputOwner;
  }
  Context():indexAmbientSSalgebra(-1),theOwner(0){}
  Context(const Context& other)
  { this->operator=(other);
  }
  void GetFormatExpressions(FormatExpressions& output)const;
  void operator=(const Context& other);
  bool operator==(const Context& other)
  { return this->VariableImages==other.VariableImages &&
    indexAmbientSSalgebra==other.indexAmbientSSalgebra;
  }
  std::string ToString()const;
  static inline unsigned int HashFunction(const Context& input){return input.HashFunction();}
  unsigned int HashFunction()const
  { return this->VariableImages.HashFunction()*SomeRandomPrimes[0]+this->indexAmbientSSalgebra*SomeRandomPrimes[1];
  }
  bool MergeContextWith(const Context& other)
  { this->VariableImages.AddOnTopNoRepetition(other.VariableImages);
    this->VariableImages.QuickSortAscending();
//    std::cout << "Variable images sorted: " << this->VariableImages.ToString();
    if (this->indexAmbientSSalgebra==-1)
      this->indexAmbientSSalgebra=other.indexAmbientSSalgebra;
    if (other.indexAmbientSSalgebra==-1)
      return true;
    return this->indexAmbientSSalgebra==other.indexAmbientSSalgebra;
  }
  template <class dataType>
  dataType GetPolynomialMonomial(int theIndex, GlobalVariables& theGlobalVariables)const;
  bool GetPolySubFromVariableSuperSet(const Context& theSuperset, PolynomialSubstitution<Rational>& output)const
;
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
  HashedList<Context> theContexts;
  HashedList<std::string, MathRoutines::hashString> theStrings;
  HashedList<VariableNonBound> theNonBoundVars;
  HashedList<std::string, MathRoutines::hashString> ExpressionNotation;
  HashedList<Expression> ExpressionWithNotation;
  HashedList<LittelmannPath> theLSpaths;
//  HashedList<DifferentialForm<Rational> > theDiffForm;
  HashedList<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
  void reset();
  std::string ToString();
};

class DataCruncher
{
public:
  typedef  bool (*CruncherDataTypes)(const Data& left, const Data& right, Data& output, std::stringstream* comments);
  int leftType;
  int RightType;
  int theOperation;
  CruncherDataTypes theCruncher;
  DataCruncher():leftType(-1), RightType(-1), theOperation(-1), theCruncher(0){}
  DataCruncher(int inputOp, int inputLeftType, int inputRightType, CruncherDataTypes inputCruncher)
  { this->leftType=inputLeftType;
    this->RightType=inputRightType;
    this->theCruncher=inputCruncher;
    this->theOperation=inputOp;
  }
  bool operator==(const DataCruncher& other)
  { return this->leftType==other.leftType && this->RightType==other.RightType && this->theOperation==other.theOperation;
  }
  void operator=(const DataCruncher& other)
  { this->leftType=other.leftType;
    this->RightType=other.RightType;
    this->theCruncher=other.theCruncher;
    this->theOperation=other.theOperation;
  }
  static unsigned int HashFunction(const DataCruncher& input)
  { return input.leftType*SomeRandomPrimes[0]+input.RightType*SomeRandomPrimes[1]+input.theOperation*SomeRandomPrimes[2];
  }
};

struct StackMaintainerRules
{
public:
  CommandList* theBoss;
  int startingRuleStackSize;
  StackMaintainerRules(CommandList* inputBoss);
  ~StackMaintainerRules();
};

class CommandList
{ template <class dataType>
  bool EvaluatePMTDtreeFromContextRecursive
(dataType& output, const Context& inputContext,
 const Expression& theInput, std::stringstream* errorLog=0)
  ;
template <class dataType>
  bool ExtractPMTDtreeContext
  (Context& outputContext, const Expression& theInput, std::stringstream* errorLog=0)
  ;
public:
//control sequences parametrize the syntactical elements
  HashedList<std::string, MathRoutines::hashString> controlSequences;
//operations parametrize the expression elements
//operations are the labels of the nodes of the expression tree
//As operations can be thought of as functions, and functions are named by the class VariableNonBound,
//operations are in fact realized as elements of type VariableNonBound.
  HashedList<VariableNonBound> operations;

  HashedList<Function> theFunctions;
  List<int> targetProperties;
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
  Expression theCommands;
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
  HashedList<Data> theData;
  HashedList<DataCruncher> theDataCrunchers;
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
  void RegisterCruncherNoFail(int theOp, int inputLeftType, int inputRightType, DataCruncher::CruncherDataTypes inputCruncher)
  { DataCruncher d(theOp, inputLeftType, inputRightType, inputCruncher);
    if (this->theDataCrunchers.Contains(d))
    { std::cout << "This is a programming error: attempting to add more than one handler for the same "
      << "pair of data types. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->theDataCrunchers.AddOnTop(d);
  }
  void RegisterDivCruncherNoFail(int inputLeftType, int inputRightType, DataCruncher::CruncherDataTypes inputCruncher)
  { this->RegisterCruncherNoFail(this->opDivide(), inputLeftType, inputRightType, inputCruncher);
  }
  void RegisterMultiplicativeDataCruncherNoFail(int inputLeftType, int inputRightType, DataCruncher::CruncherDataTypes inputCruncher)
  { this->RegisterCruncherNoFail(this->opTimes(), inputLeftType, inputRightType, inputCruncher);
  }
  void RegisterAdditiveDataCruncherNoFail(int inputLeftType, int inputRightType, DataCruncher::CruncherDataTypes inputCruncher)
  { this->RegisterCruncherNoFail(this->opPlus(), inputLeftType, inputRightType, inputCruncher);
  }
  DataCruncher::CruncherDataTypes GetMultiplicativeCruncher(int inputLeftType, int inputRightType)
  { return this->GetOpCruncher(this->opTimes(), inputLeftType, inputRightType);
  }
  DataCruncher::CruncherDataTypes GetDivCruncher(int inputLeftType, int inputRightType)
  { return this->GetOpCruncher(this->opDivide(), inputLeftType, inputRightType);
  }
  DataCruncher::CruncherDataTypes GetAdditiveCruncher(int inputLeftType, int inputRightType)
  { return this->GetOpCruncher(this->opPlus(), inputLeftType, inputRightType);
  }
  DataCruncher::CruncherDataTypes GetOpCruncher(int theOp, int inputLeftType, int inputRightType)
  { DataCruncher d(theOp, inputLeftType, inputRightType, 0);
    int theIndex=this->theDataCrunchers.GetIndex(d);
    if (theIndex==-1)
      return 0;
    return this->theDataCrunchers[theIndex].theCruncher;
  }
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
  bool ReplaceEXEByList(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceYXByListX(int theControlIndex, int inputFormat=Expression::formatDefault)
  { return this->ReplaceYXdotsXByListYXdotsX(theControlIndex, inputFormat, 1);
  }
  bool ReplaceXOXbyEusingO(int theControlIndex, int inputFormat=Expression::formatDefault)
  ;
  bool ReplaceYByListY(int theControlIndex, int inputFormat=Expression::formatDefault)
  { return this->ReplaceYXdotsXByListYXdotsX(theControlIndex, inputFormat, 0);
  }
  bool ReplaceXXYByListY(int theControlIndex, int inputFormat=Expression::formatDefault)
  { this->ReplaceYByListY(theControlIndex, inputFormat);
    return this->ReplaceXXYByY();
  }
  bool ReplaceYXdotsXByListYXdotsX(int theControlIndex, int inputFormat=Expression::formatDefault, int numXs=0);
  bool ReplaceListXEByList(int theControlIndex, int inputFormat=Expression::formatDefault);
  bool ReplaceListXEYByListY(int theControlIndex, int inputFormat=Expression::formatDefault);
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
  int conList()
  { return this->controlSequences.GetIndexIMustContainTheObject("OperationList");
  }
  int conListNoRepetition()
  { return this->controlSequences.GetIndexIMustContainTheObject("ListNoRepetition");
  }
  int conListMatrixRow()
  { return this->controlSequences.GetIndexIMustContainTheObject("ListMatrixRows");
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
  int opAtom()
  { return this->operations.GetIndexIMustContainTheObject("Atom");
  }
  int opList()
  { return this->operations.GetIndexIMustContainTheObject("OperationList");
  }
  int opUnion()
  { return this->operations.GetIndexIMustContainTheObject("\\cup");
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
  (Expression& theExpression, List<Expression>& output, int theOp)
;
  bool AppendMultiplicandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, List<Expression>& output, int RecursionDepth, int MaxRecursionDepth)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opTimes());
  }
  bool AppendSummandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, List<Expression>& output)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus());
  }
  template <class dataType>
  bool EvaluatePMTDtree
  (dataType& output, Context& outputContext, const Expression& theInput, std::stringstream* errorLog=0)
  ;
  void SpecializeBoundVars
(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs)
  ;
  bool ExpressionHasBoundVars(Expression& theExpression);
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
  bool CollectSummands(Expression& theExpression);
  bool CallCalculatorFunction(Function::FunctionAddress theFun, Expression& theExpression, std::stringstream* comments=0)
  { if (!theFun(*this, theExpression, comments))
      return false;
    return theExpression.errorString=="";
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

  static bool StandardUnion
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
static bool EvaluateCarryOutActionSSAlgebraOnGeneralizedVermaModule
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
static bool EvaluateDereferenceOneArgument
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool StandardUnionNoRepetition
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardPower
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardPlus
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardTimes
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardTensor
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardDivide
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardIsDenotedBy
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardLieBracket
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardMinus
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardFunction
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardEqualEqual
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool EvaluateDoAssociatE
(CommandList& theCommands, Expression& theExpression, std::stringstream* coments, int theOperation)
  ;
  static bool EvaluateDoExtractBaseMultiplication
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool EvaluateDoDistribute
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
  ;
  static bool DoThePower
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool DoTheOperation
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments, int theOperation)
  ;
  static bool EvaluateDoLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
  ;
  static bool EvaluateDoRightDistributeBracketIsOnTheRight
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP)
  ;
  static bool EvaluateIf
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  template<class theType>
  bool fGetMatrix
  (Expression& theExpression,
   Matrix<theType>& outputMat, Context* inputOutputStartingContext=0,
   int targetNumColsNonMandatory=-1, Function::FunctionAddress conversionFunction=0,
   std::stringstream* comments=0)
  ;
  template <class theType>
  bool GetVector
  (Expression& theExpression,
   Vector<theType>& output, Context* inputOutputStartingContext=0,
   int targetDimNonMandatory=-1, Function::FunctionAddress conversionFunction=0,
   std::stringstream* comments=0)
  ;

  static bool fMatrix
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments)
  ;
  static bool fDet
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fInvertMatrix
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fDrawPolarRfunctionTheta
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fSuffixNotationForPostScript
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fIsInteger
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fFreudenthalEval
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
    static bool fGCDOrLCM
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments, bool doGCD)
  ;
  static bool fLCM
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments)
  { return theCommands.fGCDOrLCM(theCommands, theExpression, comments, false);
  }
  static bool fGCD
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments)
  { return theCommands.fGCDOrLCM(theCommands, theExpression, comments, true);
  }
  static bool fPolynomialDivisionQuotient
  (CommandList& theCommands, Expression& theExpression,
  std::stringstream* comments)
  {return theCommands.fPolynomialDivisionQuotientRemainder(theCommands, theExpression, comments, true);
  }
  static bool fPolynomialDivisionRemainder
  (CommandList& theCommands, Expression& theExpression,
  std::stringstream* comments)
  {return theCommands.fPolynomialDivisionQuotientRemainder(theCommands, theExpression, comments, false);
  }
  static bool fPolynomialDivisionQuotientRemainder
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments, bool returnQuotient)
 ;
  static bool fPrintAllPartitions
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fPrintB3G2branchingTableCharsOnly
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  bool fPrintB3G2branchingIntermediate
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
 Vectors<RationalFunctionOld>& theHWs, branchingData& theG2B3Data, Context& theContext
 )
 ;
  static bool fPrintB3G2branchingTableInit
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
   branchingData& theG2B3data, int& desiredHeight, Context& outputContext)
  ;
  static bool fDecomposeCharGenVerma
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fPrintB3G2branchingTable
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fDifferential
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fPrintB3G2branchingTableCommon
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
  Vectors<RationalFunctionOld>& outputHWs, branchingData& theG2B3Data, Context& theContext
  )
  ;
  static bool fPolynomial
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fDecomposeFDPartGeneralizedVermaModuleOverLeviPart
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments)
  ;
  bool fSplitFDpartB3overG2Init
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
 branchingData& theG2B3Data, Context& outputContext
 )
 ;
  static bool fSplitFDpartB3overG2CharsOnly
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fElementUniversalEnvelopingAlgebra
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fSSAlgebraShort
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
{ return theCommands.fSSAlgebra(theCommands, theExpression, comments, false);
}
  static bool fSSAlgebraVerbose
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
{ return theCommands.fSSAlgebra(theCommands, theExpression, comments, true);
}
template<class CoefficientType>
bool fGetTypeHighestWeightParabolic
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments,
 Vector<CoefficientType>& outputWeightHWFundcoords, Selection& outputInducingSel,
 Context* outputContext=0)
 ;
 static bool fGroebnerBuchbergerGrLex
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 { return theCommands.fGroebnerBuchberger
  (theCommands, theExpression, comments, true);
 }
 static bool fGroebnerBuchbergerLex
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 { return theCommands.fGroebnerBuchberger
  (theCommands, theExpression, comments, false);
 }
  static bool fGroebnerBuchbergerLexUpperLimit
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 ;
 static bool fGroebnerBuchberger
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments, bool useGr, int upperBoundComputations=0)
 ;
 static bool fParabolicWeylGroups
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 ;
 static bool fParabolicWeylGroupsBruhatGraph
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 ;
  static bool fKLcoeffs
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 ;
  static bool fEmbedSSalgInSSalg
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
 ;
  static bool fWeylOrbit
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments, bool useFundCoords, bool useRho)
 ;
  static bool fWeylOrbitFund
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
{ return theCommands.fWeylOrbit(theCommands, theExpression, comments, true, false);
}
  static bool fWeylOrbitSimple
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
{ return theCommands.fWeylOrbit(theCommands, theExpression, comments, false, false);
}
  static bool fWeylOrbitFundRho
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments)
{ return theCommands.fWeylOrbit(theCommands, theExpression, comments, true, true);
}
  static bool fSSAlgebra
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
  { return theCommands.fSSAlgebra(theCommands, theExpression, comments, false);
  }
  static bool fSSAlgebra
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments, bool Verbose)
;
  static bool fSplitFDpartB3overG2CharsOutput
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
 branchingData& theG2B3Data
 )
;
  static bool fSplitFDpartB3overG2old
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fSplitFDpartB3overG2
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fSplitGenericGenVermaTensorFD
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fSplitFDpartB3overG2inner
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
  branchingData& theG2B3Data)
;
  static bool fDrawWeightSupportWithMults
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fDrawWeightSupport
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;

  static bool fHWTAABF
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fElementSSAlgebra
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fWeylDimFormula
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fLittelmannOperator
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fAnimateLittelmannPaths
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fSqrt
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fFactor
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fSolveSeparableBilinearSystem
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fMinPoly
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fLSPath
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fTestMonomialBaseConjecture
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fJacobiSymbol
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fHWVinner
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
 Vector<RationalFunctionOld>& highestWeightFundCoords,
 Selection& selectionParSel, Context& hwContext, int indexOfAlgebra)
 ;
 bool fWriteGenVermaModAsDiffOperatorInner
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
  Vectors<Polynomial<Rational> >& theHws, Context& hwContext, Selection& selInducing, int indexOfAlgebra)
  ;
  template<class CoefficientType>
static bool TypeHighestWeightParabolic
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments,
 Vector<CoefficientType>& outputWeight,
 Selection& outputInducingSel, Context* outputContext=0)
;
  static bool fHWV
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fWriteGenVermaModAsDiffOperatorUpToLevel
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fWriteGenVermaModAsDiffOperators
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fEmbedG2inB3
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fCasimir
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
;
  static bool fRootSAsAndSltwos
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments, bool showSLtwos)
;
  static bool fprintRootSAs
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments)
  { return theCommands.fRootSAsAndSltwos
    (theCommands, theExpression, comments, false);
  }
  static bool fprintSltwos
  (CommandList& theCommands, Expression& theExpression,
   std::stringstream* comments)
  { return theCommands.fRootSAsAndSltwos
    (theCommands, theExpression, comments, true);
  }
  static bool fSSsubalgebras
  (CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
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
//    if (theIndex!=this->theNonBoundVars.size-1)
//      assert(false);
//  }
  int GetIndexNonBoundVar(const std::string& theName)
  { VariableNonBound tempVar;
    tempVar.theName=theName;
    return this->theObjectContainer.theNonBoundVars.GetIndex(tempVar);
  }
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
  bool EvaluateExpressionReturnFalseIfExpressionIsBound
(Expression& theExpression, BoundVariablesSubstitution& bufferPairs, std::stringstream* comments=0)
 ;
  void Evaluate(const std::string& theInput)
  { MacroRegisterFunctionWithName("CommandList::Evaluate");
    if (this->theGlobalVariableS==0)
    { this->outputString= "This is a programming error: commandList not initialized properly. Please report this bug. ";
      return;
    }
    this->StartTimeEvaluationInSecondS=this->theGlobalVariableS->GetElapsedSeconds();
    this->inputString=theInput;
    this->ParseAndExtractExpressions(theInput, this->theCommands, this->syntacticSouP, this->syntacticStacK, & this->syntaxErrors);
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
(Expression& theExpression,
 Vector<theType>& output, Context* inputOutputStartingContext,
 int targetDimNonMandatory, Function::FunctionAddress conversionFunction,
 std::stringstream* comments)
{ MemorySaving<Context> tempContext;
  Context& startContext=
  inputOutputStartingContext==0 ? tempContext.GetElement() : *inputOutputStartingContext;
  if (theExpression.theOperation!=this->opList())
  { if (targetDimNonMandatory>0)
      if (targetDimNonMandatory!=1)
        return false;
    Data outputData;
    if (!theExpression.EvaluatesToAtom())
    { Expression tempExpression=theExpression;
      if (conversionFunction!=0)
        if (!this->CallCalculatorFunction
            (conversionFunction, tempExpression, comments))
          return false;
      if (!tempExpression.EvaluatesToAtom())
        return false;
      outputData=tempExpression.GetAtomicValue();
    } else
      outputData=theExpression.GetAtomicValue();
    if (!outputData.MergeContextsAndConvertToType<theType>(startContext))
      return false;
    output.SetSize(1);
    output.TheObjects[0]=outputData.GetValuE<theType>();
    return true;
  }
  if (targetDimNonMandatory>0)
    if (targetDimNonMandatory!=theExpression.children.size)
      return false;
  Vector<Data> outputData;
  outputData.SetSize(theExpression.children.size);
  for (int i=0; i<theExpression.children.size; i++)
  { Expression& currentE=theExpression.children[i];
    bool needsConversion=true;
    if (currentE.EvaluatesToAtom())
      if (currentE.GetAtomicValue().IsOfType<theType>())
        needsConversion=false;
    if (needsConversion)
    { Expression tempExpression=currentE;
      if (conversionFunction!=0)
        if (!this->CallCalculatorFunction
            (conversionFunction, tempExpression, comments))
          return false;
      if (!tempExpression.EvaluatesToAtom())
        return false;
      outputData[i]=tempExpression.GetAtomicValue();
    } else
      outputData[i]=currentE.GetAtomicValue();
    if (outputData[i].theContextIndex!=-1)
      if (!startContext.MergeContextWith(outputData[i].GetContext()))
        return false;
  }
  output.SetSize(theExpression.children.size);
  for (int i=0; i<outputData.size; i++)
  { if (!outputData[i].ConvertToTypeResizesContextArrays<theType>(startContext))
      return false;
    output[i]=outputData[i].GetValuE<theType>();
  }
  return true;
}

template <class theType>
bool CommandList::fGetMatrix
(Expression& theExpression,
 Matrix<theType>& outputMat, Context* inputOutputStartingContext,
 int targetNumColsNonMandatory, Function::FunctionAddress conversionFunction,
 std::stringstream* comments)
{ MemorySaving<Context> tempContext;
  Context& startContext=
  inputOutputStartingContext==0 ? tempContext.GetElement() : *inputOutputStartingContext;
  if (theExpression.theOperation!=this->opList())
  { if (targetNumColsNonMandatory!=-1)
      if (targetNumColsNonMandatory!=1)
        return false;
    Data outputData;
    if (!theExpression.EvaluatesToAtom())
    { Expression tempExpression=theExpression;
      if (conversionFunction!=0)
        if (!this->CallCalculatorFunction
            (conversionFunction, tempExpression, comments))
          return false;
      if (!theExpression.EvaluatesToAtom())
        return false;
      outputData=tempExpression.GetAtomicValue();
    } else
      outputData=theExpression.GetAtomicValue();
    if (!outputData.MergeContextsAndConvertToType<theType>(startContext))
      return false;
    outputMat.init(1,1);
    outputMat.elements[0][0]=outputData.GetValuE<theType>();
    return true;
  }
  int targetNumRows=theExpression.children.size;
  for (int i=0; i<theExpression.children.size; i++)
  { Expression& currentE=theExpression.children[i];
    if (i==0)
    { int numCols=1;
      if (currentE.theOperation==this->opList())
        numCols=currentE.children.size;
      if (targetNumColsNonMandatory!=-1)
        if (numCols!=targetNumColsNonMandatory)
        { if (comments!=0)
            *comments << "Error getting matrix: target number of columns is "
            << targetNumColsNonMandatory << " but the first row has " << numCols << " columns instead. ";
          return false;
        }
      targetNumColsNonMandatory=numCols;
      outputMat.init(targetNumRows, targetNumColsNonMandatory);
    }
    Vector<theType> currentRow;
    if (!this->GetVector
        (currentE, currentRow, &startContext, targetNumColsNonMandatory, conversionFunction, comments))
      return false;
    for (int j=0; j<targetNumColsNonMandatory; j++)
      outputMat.elements[i][j]=currentRow[j];
  }
  return true;
}

#endif
