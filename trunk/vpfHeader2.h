//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader2_h_already_included
#define vpfHeader2_h_already_included

//the following  include contains all the c++ math routines used in the calculator.
//The calculator uses relatively few functions, nevertheless the include is needed.
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"
#ifndef ProjectInfoVpfHeader2AlreadyDefined
#define ProjectInfoVpfHeader2AlreadyDefined
static ProjectInformationInstance ProjectInfoVpfHeader2(__FILE__, "Header file containing the calculator's parsing routines. ");
#endif
class Context;

class Data
{
public:
  CommandList* owner;
  int theIndex;
  int theContextIndex;
  int type;
  MemorySaving<std::string> theError;
//  MemorySaving<ElementTensorsGeneralizedVermas<RationalFunction> > theElementTensorGenVermas;
  enum DataType
  { typeError=1, typeRational, typePoly, typeRationalFunction, typeSSalgebra,
    typeEltTensorGenVermasOverRF, typeMonomialGenVerma, typeElementUE, typeEltSumGenVermas,
    typeString, typeVariableNonBound
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
  void MakeSSAlgebra
  (CommandList& theBoss, char WeylLetter, int WeylRank)
  ;
  void MakeRational
(CommandList& theBoss, const Rational& inputRational)
;
  void MakeRF
(CommandList& theBoss, const RationalFunction& inputRF, int inputContextIndex)
;
  template<class dataType>
  void Make(CommandList& theBoss, const dataType& input)
  ;
  void MakeUE
(CommandList& theBoss, const ElementUniversalEnveloping<RationalFunction>& inputUE, int inputContextIndex)
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
(CommandList& theBoss, ElementTensorsGeneralizedVermas<RationalFunction>& theElt, int inputContextIndex)
  ;
  int GetNumContextVars()const;
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
  const ElementUniversalEnveloping<RationalFunction>& GetUE()const;
  const RationalFunction& GetRF()const;
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
  bool IsSmallInteger(int & whichInteger)const
  ;
  bool Add(const Data& right, Data& output)const
  ;
  bool Exponentiate(const Data& right, Data& output)const
  ;
  bool IsInteger()const;
  ElementSemisimpleLieAlgebra& GetEltSimpleLieAlgebra()const;
  int GetSmallInT()const;
  MonomialGeneralizedVerma<RationalFunction>& GetMonGenVerma()const;
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
  static bool MultiplyAnyByEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
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
  static bool AddUEToAny(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool AddEltTensorToEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool AddRatOrPolyToRatOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
  static bool MultiplyRatOrPolyByRatOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments=0);
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
  MemorySaving<List<Expression> > theArgumentPatterns;
  MemorySaving<List<bool> > theArgumentPatternIsParsed;
  typedef  bool (*FunctionAddress)(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments);
  FunctionAddress theFunction;
  std::string ToString(CommandList& theBoss)const;
  void operator =(const Function& other)
  { this->theName=other.theName;
    this->theArgumentList=other.theArgumentList;
    this->theDescription=other.theDescription;
    this->theExample=other.theExample;
    this->theFunction=other.theFunction;
    this->flagNameIsVisible=other.flagNameIsVisible;
  }
  bool operator==(const Function& other)const
  { return this->theArgumentList==other.theArgumentList &&
    this->theFunction==other.theFunction && this->theName==other.theName;
  }
  Function(){this->theFunction=0;}
  Function
  (const Function::FunctionAddress& functionPointer, const std::string& functionName, const std::string& argumentList,
   const std::string& description, const std::string& inputExample, bool inputflagNameIsVisible)
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
  }
  inline static unsigned int HashFunction(const Function& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return (unsigned int) this->theFunction;
  }
};

class Expression
{ void reset(int inputIndexBoundVars)
  { this->theBoss=0;
    this->children.size=0;
    this->theOperation=-1;
    this->theDatA=-1;
    this->errorString="";
    this->format=this->formatDefault;
    this->IndexBoundVars=inputIndexBoundVars;
  }
  public:
  int theOperation;
  int theDatA;
  List<Expression> children;
  std::string errorString;
  ///////////////////////////////////////
  //two objects are considered equal even when the the following data is different:
  int format;
  int IndexBoundVars;
  CommandList* theBoss;
  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot, formatNoBracketsForFunctionArgument, formatMatrix, formatMatrixRow
  };
  void reset(CommandList& newBoss, int inputIndexBoundVars)
  { this->reset(inputIndexBoundVars);
    this->theBoss=&newBoss;
  }
  void AssignChild(int childIndex)
  { Expression tempExp=this->children[childIndex];
    this->operator=(tempExp);
  }
  const Data& GetAtomicValue()const;
  Rational GetRationalValue()const;
  void MakeMonomialGenVerma
  (const MonomialGeneralizedVerma<RationalFunction>& inputMon, CommandList& newBoss, int inputIndexBoundVars)
 ;
  void MakeElementTensorsGeneralizedVermas
  (const ElementTensorsGeneralizedVermas<RationalFunction>& inputMon, CommandList& newBoss, int inputIndexBoundVars)
 ;
  void MakePolyAtom
(const Polynomial<Rational>& inputData, int inputContextIndex, CommandList& newBoss, int inputIndexBoundVars)
  ;
  void MakePolY
(const Polynomial<Rational>& inputData, int inputContextIndex, CommandList& newBoss, int inputIndexBoundVars)
  ;
  void MakeRFAtom(const RationalFunction& inputData, int inputContextIndex, CommandList& newBoss, int inputIndexBoundVars);
  void MakeAtom(const Data& inputData, CommandList& newBoss, int inputIndexBoundVars)
  ;
  void MakeAtom(const Rational& inputRat, CommandList& newBoss, int inputIndexBoundVars)
  ;
  void MakeAtom(int input, CommandList& newBoss, int inputIndexBoundVars)
  { this->MakeAtom((Rational) input, newBoss, inputIndexBoundVars);
  }
  void MakeAtom(const VariableNonBound& input, CommandList& newBoss, int inputIndexBoundVars)
  ;
  void MakeStringAtom
(CommandList& newBoss, int inputIndexBoundVars, const std::string& theString, const Context& inputContext)
;
  void MakeInt(int theInt, CommandList& newBoss, int inputIndexBoundVars)
  ;
void MakeVariableNonBounD
  (CommandList& owner, int inputIndexBoundVars, const std::string& varName)
;
  void MakeFunction
  (CommandList& owner, int inputIndexBoundVars, const Expression& argument, const std::string& functionName)
;
  bool EvaluatesToAtom()const;
  bool EvaluatesToVariableNonBound()const;
  void MakeFunction
  (CommandList& owner, int inputIndexBoundVars, const Expression& argument, int functionIndex)
;
  Function::FunctionAddress GetFunctionAddressFromVarName();
  Function& GetFunctionFromVarNamE();
  void MakeProducT
  (CommandList& owner, int inputIndexBoundVars, const Expression& left, const Expression& right)
  ;
  void MakeFunction
  (CommandList& owner, int inputIndexBoundVars, const Expression& theFunction, const Expression& theArgument)
  ;
  int GetNumCols()const
  ;
  void MakeXOX
  (CommandList& owner, int inputIndexBoundVars, int theOp, const Expression& left, const Expression& right)
  ;
  std::string ToString
  (FormatExpressions* theFormat=0, bool AddBrackets=false, bool AddCurlyBraces=false, std::stringstream* outComments=0, bool isFinal=true)const;
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
  { this->reset(-1);
  }
  inline bool SetError (const std::string& theError)
  { this->errorString=theError;
    return true;
  }
  Expression(const Expression& other)
  { this->operator=(other);
  }
  template <class theType>
  bool GetVector
  (Vector<theType>& output, Context& inputStartingContext, int targetDimNonMandatory=-1, Function::FunctionAddress conversionFunction=0,
   std::stringstream* comments=0)
  ;
  bool HasBoundVariables();
//  bool IsRationalAtom()const;
  bool EvaluatesToRational()const;
  bool IsElementUE()const;
  bool IsInteger()const;
  bool EvaluatesToSmallInteger(int& whichInteger)const;
  bool EvaluatesToSmallInteger()const{int tempI; return this->EvaluatesToSmallInteger(tempI);}
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
  bool NeedBracketsForMultiplication()const;
  bool NeedBracketsForFunctionArgument()const;
  bool NeedBracketsForAddition()const;
  bool NeedBracketsForFunctionName()const;
  bool NeedBracketsForThePower()const;
  bool operator==(const Expression& other)const;
  void CopyValueFromNoChildrenCopy(const Expression& other)
  { this->theBoss=other.theBoss;
    this->theDatA=other.theDatA;
    this->theOperation=other.theOperation;
    this->errorString=other.errorString;
    this->children.SetSize(other.children.size);
    this->format=other.format;
    this->IndexBoundVars=other.IndexBoundVars;
  }
  void operator=(const Expression& other);
};

class ExpressionPairs
{
public:
  std::string ToString();
  HashedListB<int, MathRoutines::IntUnsignIdentity> BoundVariableIndices;
  HashedList<Expression> variableImages;
  void reset()
  { this->BoundVariableIndices.Clear();
    this->variableImages.Clear();
  }
};

class SyntacticElement
{
  public:
  int controlIndex;
  int IndexFirstChar;
  int IndexLastCharPlusOne;
  std::string ErrorString;
  Expression theData;
  void operator=(const SyntacticElement& other)
  { this->controlIndex=other.controlIndex;
    this->theData=other.theData;
    this->ErrorString=other.ErrorString;
    this->IndexFirstChar=other.IndexFirstChar;
    this->IndexLastCharPlusOne=other.IndexLastCharPlusOne;
  }
  std::string ToString(CommandList& theBoss);
  SyntacticElement()
  { this->controlIndex=0;//controlIndex=0 *MUST* point to the empty control sequence.
    this->ErrorString="";
    this->IndexFirstChar=-1;
    this->IndexLastCharPlusOne=-1;
  }
  SyntacticElement(const SyntacticElement& other)
  { this->operator=(other);
  }
};

class ExpressionContext
{
public:
  HashedListB<std::string, MathRoutines::hashString> BoundVariables;
  HashedList<Expression> cachedExpressions;
  List<Expression> imagesCachedExpressions;
  bool flagOpDefineEncountered;
  void operator=(const ExpressionContext& other)
  { this->BoundVariables=other.BoundVariables;
    this->cachedExpressions=other.cachedExpressions;
    this->imagesCachedExpressions=other.imagesCachedExpressions;
    this->flagOpDefineEncountered=other.flagOpDefineEncountered;
  }
  void reset()
  { this->BoundVariables.Clear();
    this->cachedExpressions.Clear();
    this->imagesCachedExpressions.SetSize(0);
    this->flagOpDefineEncountered=false;
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
  Context(CommandList& inputOwner):indexAmbientSSalgebra(-1){this->theOwner=&inputOwner;}
  Context():indexAmbientSSalgebra(-1),theOwner(0){}
  void operator=(const Context& other);
  bool operator==(const Context& other)
  { return this->VariableImages==other.VariableImages && indexAmbientSSalgebra==other.indexAmbientSSalgebra;
  }
  std::string ToString()const;
  static inline unsigned int HashFunction(const Context& input){return input.HashFunction();}
  unsigned int HashFunction()const
  { return this->VariableImages.HashFunction()*SomeRandomPrimes[0]+this->indexAmbientSSalgebra*SomeRandomPrimes[1];
  }
  bool MergeContextWith(const Context& other)
  { this->VariableImages.AddNoRepetition(other.VariableImages);
    if (this->indexAmbientSSalgebra==-1)
      this->indexAmbientSSalgebra=other.indexAmbientSSalgebra;
    if (other.indexAmbientSSalgebra==-1)
      return true;
    return this->indexAmbientSSalgebra==other.indexAmbientSSalgebra;
  }
  template <class dataType>
  dataType GetPolynomialMonomial(int theIndex, GlobalVariables& theGlobalVariables)const;
  bool GetPolySubFromVaraibleSuperSet(const Context& theSuperset, PolynomialSubstitution<Rational>& output)const
;
};

class ObjectContainer
{ //Following are containers for data structures that are implemented in C++.
  //These objects are dynamically allocated and used by the calculator as requested
  //by various predefined function handlers.
public:
  List<ModuleSSalgebraNew<RationalFunction> > theCategoryOmodules;
  List<SemisimpleLieAlgebra> theLieAlgebras;
  HashedList<ElementTensorsGeneralizedVermas<RationalFunction> > theTensorElts;
  HashedList<Polynomial<Rational> > thePolys;
  HashedList<ElementUniversalEnveloping<RationalFunction> > theUEs;
  HashedList<RationalFunction> theRFs;
  HashedList<Rational> theRationals;
  HashedList<Context> theContexts;
  HashedListB<std::string, MathRoutines::hashString> theStrings;
  HashedList<VariableNonBound> theNonBoundVars;

  void reset();
  std::string ToString();
};

class DataCruncher
{
public:
  typedef  bool (*CruncherDataTypes)(const Data& left, const Data& right, Data& output, std::stringstream* comments);
  int leftType; int RightType;
  CruncherDataTypes theCruncher;
  DataCruncher():leftType(-1), RightType(-1), theCruncher(0){}
  DataCruncher(int inputLeftType, int inputRightType, CruncherDataTypes inputCruncher)
  { this->leftType=inputLeftType;
    this->RightType=inputRightType;
    this->theCruncher=inputCruncher;
  }
  bool operator==(const DataCruncher& other)
  { return this->leftType==other.leftType && this->RightType==other.RightType;
  }
  void operator=(const DataCruncher& other)
  { this->leftType=other.leftType;
    this->RightType=other.RightType;
    this->theCruncher=other.theCruncher;
  }
  static unsigned int HashFunction(const DataCruncher& input)
  { return input.leftType*SomeRandomPrimes[0]+input.RightType*SomeRandomPrimes[1];
  }
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
  HashedListB<std::string, MathRoutines::hashString> controlSequences;
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
  ///////////////////////////////////////////////////////////////////////////
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
  List<SyntacticElement> syntacticSouP;
  List<SyntacticElement> syntacticStacK;

  List<SyntacticElement>* CurrrentSyntacticSouP;
  List<SyntacticElement>* CurrentSyntacticStacK;

  List<ExpressionContext> theExpressionContext;
  ////
  HashedList<Expression> ExpressionStack;

  std::string syntaxErrors;
  List<std::string> evaluationErrors;
  std::string inputStringRawestOfTheRaw;
  std::string inputString;
  std::string outputString;
  std::string outputCommentsString;
  std::string DisplayNameCalculator;
  GlobalVariables* theGlobalVariableS;
  HashedList<Data> theData;
  HashedList<DataCruncher> theMultiplicativeDataCrunchers;
  HashedList<DataCruncher> theAdditiveDataCrunchers;
  ObjectContainer theObjectContainer;
  double StartTimeEvaluationInSecondS;

  char DefaultWeylLetter;
  int DefaultWeylRank;
  std::string javaScriptDisplayingIndicator;
  std::string indicatorReportFileName;
  std::string DisplayPathServerBase;
  std::string PhysicalPathServerBase;
  std::string DisplayPathOutputFolder;
  std::string PhysicalPathOutputFolder;
  std::string DisplayNameDefaultOutput;
  std::string DisplayNameDefaultOutputNoPath;
  std::string PhysicalNameDefaultOutput;
  std::string indicatorFileName;
  std::string indicatorFileNameDisplay;
  std::string indicatorReportFileNameDisplay;
  std::string userLabel;
  List<std::string> SystemCommands;

  std::string ToString();
  std::string ElementToStringNonBoundVars();
  std::string ElementToStringFunctionHandlers();
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
  void RegisterMultiplicativeDataCruncherNoFail(int inputLeftType, int inputRightType, DataCruncher::CruncherDataTypes inputCruncher)
  { DataCruncher d(inputLeftType, inputRightType, inputCruncher);
    if (this->theMultiplicativeDataCrunchers.Contains(d))
    { std::cout << "This is a programming error: attempting to add more than one handler for the same "
      << "pair of data types. Please debug file " << CGI::GetHtmlLinkFromFileName(__FILE__) << " line " << __LINE__ << ".";
      assert(false);
    }
    this->theMultiplicativeDataCrunchers.AddOnTop(d);
  }
  DataCruncher::CruncherDataTypes GetMultiplicativeCruncher(int inputLeftType, int inputRightType)
  { DataCruncher d(inputLeftType, inputRightType, 0);
    int theIndex=this->theMultiplicativeDataCrunchers.GetIndex(d);
    if (theIndex==-1)
      return 0;
    return this->theMultiplicativeDataCrunchers[theIndex].theCruncher;
  }
  void RegisterAdditiveDataCruncherNoFail(int inputLeftType, int inputRightType, DataCruncher::CruncherDataTypes inputCruncher)
  { DataCruncher d(inputLeftType, inputRightType, inputCruncher);
    if (this->theAdditiveDataCrunchers.Contains(d))
    { std::cout << "This is a programming error: attempting to add more than one handler for the same "
      << "pair of data types. Please debug file " << CGI::GetHtmlLinkFromFileName(__FILE__) << " line " << __LINE__ << ".";
      assert(false);
    }
    this->theAdditiveDataCrunchers.AddOnTop(d);
  }
  DataCruncher::CruncherDataTypes GetAdditiveCruncher(int inputLeftType, int inputRightType)
  { DataCruncher d(inputLeftType, inputRightType, 0);
    int theIndex=this->theAdditiveDataCrunchers.GetIndex(d);
    if (theIndex==-1)
      return 0;
    return this->theAdditiveDataCrunchers[theIndex].theCruncher;
  }
  bool LookAheadAllowsThePower(const std::string& lookAhead)
  { return lookAhead!="{}";
  }
  HashedListB<std::string, MathRoutines::hashString>& GetCurrentContextBoundVars()
  { return this->theExpressionContext.LastObject()->BoundVariables;
  }
  bool LookAheadAllowsApplyFunction(const std::string& lookAhead)
;
  bool LookAheadAllowsPlus(const std::string& lookAhead)
  ;
  bool LookAheadAllowsTimes(const std::string& lookAhead);
  bool LookAheadAllowsDivide(const std::string& lookAhead);
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
  bool ReplaceYXByListYX(int theControlIndex, int inputFormat=Expression::formatDefault)
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
  bool ReplaceVXbyEX();
  bool ReplaceVbyE();
  bool ReplaceIntIntBy10IntPlusInt()
  ;
  bool CreateNewExpressionContext();
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
  int opVariableBound()
  { return this->operations.GetIndexIMustContainTheObject("VariableBound");
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
(Expression& toBeSubbed, int targetCommandIndex, ExpressionPairs& matchedPairs)
  ;
  bool ExpressionHasBoundVars(Expression& theExpression);
  Expression* PatternMatch
  (int inputIndexBoundVars, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   Expression* condition=0, std::stringstream* theLog=0, bool logAttempts=false)
;
  bool ProcessOneExpressionOnePatternOneSub
  (int inputIndexBoundVars, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
    std::stringstream* theLog=0, bool logAttempts=false)
  ;
  bool isADigit(const std::string& input, int& whichDigit)
  { if (input.size()!=1)
      return false;
    whichDigit=input[0]-'0';
    return whichDigit<10 && whichDigit>=0;
  }
//  bool OrderMultiplicationTreeProperly(int commandIndex, Expression& theExpression);
  bool CollectSummands(int inputIndexBoundVars, Expression& theExpression);
bool CollectSummands
(List<Expression>& summands, bool needSimplification, int inputIndexBoundVars, Expression& theExpression)
;
  bool ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, ExpressionPairs& matchedExpressions,
   std::stringstream* theLog=0)
  ;
  bool ExpressionsAreEqual
  (const Expression& left, const Expression& right)
  ;

  static bool StandardUnion
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
static bool EvaluateCarryOutActionSSAlgebraOnGeneralizedVermaModule
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
static bool EvaluateDereferenceOneArgument
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool StandardUnionNoRepetition
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardPlus
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardPower
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardTimes
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardDivide
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardLieBracket
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardMinus
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardFunction
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool StandardEqualEqual
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool EvaluateDoAssociatE
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* coments, int theOperation)
  ;
  static bool EvaluateDoExtractBaseMultiplication
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool EvaluateDoDistribute
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
  ;
  static bool DoThePowerIfPossible
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool DoMultiplyIfPossible
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool EvaluateDoLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
  ;
  static bool EvaluateDoRightDistributeBracketIsOnTheRight
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP)
  ;
  static bool EvaluateIf
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fMatrix
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fIsInteger
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fPolynomial
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fDecomposeFDPartGeneralizedVermaModuleOverLeviPart
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fElementUniversalEnvelopingAlgebra
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
  ;
  static bool fSSAlgebra
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fHWTAABF
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fElementSSAlgebra
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fWeylDimFormula
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fAnimateLittelmannPaths
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fHWV
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fEmbedG2inB3
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fCasimir
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  static bool fRootSAsAndSltwos
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
;
  void AddEmptyHeadedCommand();
  CommandList();
  void AddOperationNoFail
  (const std::string& theOpName, const Function::FunctionAddress& theFunAddress, const std::string& opArgumentList,
   const std::string& opDescription, const std::string& opExample, bool inputNameUsed)
   ;
  void AddNonBoundVarMustBeNew
  (const std::string& theName, const Function::FunctionAddress& funHandler,
   const std::string& argList, const std::string& description, const std::string& exampleArgs)
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
(Expression& theExpression, ExpressionPairs& bufferPairs, std::stringstream* comments=0)
 ;
  void Evaluate(const std::string& theInput)
  { if (this->theGlobalVariableS==0)
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

class IncrementRecursion
{ CommandList* theBoss;
  public:
  IncrementRecursion(CommandList* owner): theBoss(0)
  { if (owner!=0)
    { owner->RecursionDeptH++;
      if (owner->DepthRecursionReached<owner->RecursionDeptH)
        owner->DepthRecursionReached=owner->RecursionDeptH;
      this->theBoss=owner;
    }
  }
  ~IncrementRecursion()
  { if (this->theBoss!=0)
      this->theBoss->RecursionDeptH--;
  }
};

#endif
