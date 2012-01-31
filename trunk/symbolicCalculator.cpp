#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include<assert.h>

class CommandList;
class Command;
class Expression;
class ExpressionPairs;

const int SomeRandomPrimesSize= 25;
//used for hashing various things.
const int SomeRandomPrimes[SomeRandomPrimesSize]={ 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911};

int hashString(const std::string& x)
{ int numCycles=x.size();
  if (numCycles>SomeRandomPrimesSize)
    numCycles=SomeRandomPrimesSize;
  int result=0;
  for (int i=0; i<numCycles; i++)
    result+=x[i]*SomeRandomPrimes[i];
  return result;
}

template <class element, int hashFunction(const element&)>
class hashedVector
{ std::vector<element> theElts;
  std::vector<std::vector<int> > hashedIndices;
  public:
  const element& operator[](int i)const {return this->theElts[i];}
  int GetHash(const element& x)
  { int result=hashFunction(x)% this->hashedIndices.size();
    if (result<0)
      result+=this->hashedIndices.size();
    return result;
  }
  int size()const{return this->theElts.size();}
  void reset()
  { if (this->theElts.size()<this->hashedIndices.size())
      for (unsigned i=0; i<this->theElts.size(); i++)
        this->hashedIndices[this->GetHash(this->theElts[i])].resize(0);
    else
      for (unsigned i=0; i<this->hashedIndices.size(); i++)
        this->hashedIndices[i].resize(0);
    this->theElts.resize(0);
  }
  void resetSetHashSize(int N)
  { this->reset();
    if (N<1)
      N=1;
    this->hashedIndices.resize(N);
  }
  int GetIndex(const element& x)
  { int theHash=this->GetHash(x);
    std::vector<int>& currentHash=this->hashedIndices[theHash];
    for (unsigned i=0; i<currentHash.size(); i++)
      if (x==this->theElts[currentHash[i]])
        return currentHash[i];
    return -1;
  }
  int GetIndexIMustContainTheObject(const element& x)
  { int result=this->GetIndex(x);
    if(result==-1)
      assert(false);
    return result;
  }
  void reserve(int N){this->theElts.reserve(N);}
  bool Contains(const element & x)
  { return (this->GetIndex(x)!=-1);
  }
  hashedVector()
  { this->resetSetHashSize(1000);
  }
  inline void push_back(const element& x)
  { this->AddOnTop(x);
  }
  bool AddNoRepetition(const element& x)
  { if (!this->Contains(x))
    { this->AddOnTop(x);
      return true;
    }
    return false;
  }
  void AddOnTop(const element& x)
  { this->theElts.push_back(x);
    this->hashedIndices[this->GetHash(x)].push_back(this->theElts.size()-1);
  }
  void AddOnTop(const hashedVector<element, hashFunction>& x)
  { this->reserve(x.size()+this->size());
    for (int i=0; i<x.size(); i++)
      this->AddOnTop(x[i]);
  }
  hashedVector(int initialHashSize)
  { this->resetSetHashSize(initialHashSize);
  }
};

class Expression
{
  public:
  CommandList* theBoss;
  int commandIndex;
  ///////////////////////////////////////
  int theOperation;
  int theData;
  std::vector<Expression> children;
  ///////////////////////////////////////
  int formattingOptions;
  std::string correspondingString;
  std::string errorString;

  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot,
  };
  void reset(CommandList* newBoss, int indexOfTheCommand)
  { this->theBoss=newBoss;
    this->commandIndex=indexOfTheCommand;
    this->children.resize(0);
    this->theOperation=-1;
  }
  void AssignMeMyChild(int childIndex)
  { Expression tempExp=this->children[childIndex];
    this->operator=(tempExp);
  }
  std::string ElementToString(int recursionDepth=0, int maxRecursionDepth=1000, bool AddBrackets=false, bool AddCurlyBraces=false)const;
  std::string ElementToStringPolishForm(int recursionDepth=0, int maxRecursionDepth=1000);
  static int HashFunction(const Expression& input)
  { return input.theData*SomeRandomPrimes[1]+input.theOperation*SomeRandomPrimes[0];
  }
  std::string GetOperation()const;
  Expression()
  { this->theBoss=0;
    this->errorString="";
    this->theData=0;
    this->theOperation=-1;
    this->commandIndex=-1;
    this->formattingOptions=this->formatDefault;
  }
  Expression(const Expression& other)
  { this->theBoss=0;
    this->operator=(other);
  }
  bool NeedBracketsForMultiplication()const;
  bool NeedBracketsForAddition()const;
  bool NeedBracketsForFunctionName()const;
  bool NeedBracketsForThePower()const;
  bool operator==(const Expression& other)const;
  void operator=(const Expression& other)
  { this->theBoss=other.theBoss;
    this->commandIndex=other.commandIndex;
    this->children=other.children;
    this->theData=other.theData;
    this->theOperation=other.theOperation;
    //////////////////////////////////////////////////////////////
    this->formattingOptions=other.formattingOptions;
    this->correspondingString=other.correspondingString;
    this->errorString=other.errorString;
  }
};

class ExpressionPairs
{
public:
  std::string ElementToString();
  hashedVector<Expression, Expression::HashFunction> left;
  hashedVector<Expression, Expression::HashFunction> right;
  void reset(){this->left.reset(); this->right.reset();}
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
  std::string ElementToStringNoExpression(CommandList& theBoss);
  std::string ElementToString(CommandList& theBoss);
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

class Command
{
public:
  CommandList* theBoss;
  std::vector<SyntacticElement> syntacticSoup;
  std::vector<SyntacticElement> expressionStacK;
  std::string log;
  Expression finalValue;
  int numEmptyTokensStart;
  int IndexInBoss;
  std::string theLog;
  std::string ErrorString;
  hashedVector<std::string, hashString> BoundVariables;
  void operator=(const Command& other)
  { if (this==& other)
      return;
    this->BoundVariables=other.BoundVariables;
    this->expressionStacK=other.expressionStacK;
    this->finalValue=other.finalValue;
    this->numEmptyTokensStart=other.numEmptyTokensStart;
    this->ErrorString=other.ErrorString;
    this->theBoss=other.theBoss;
    this->IndexInBoss=other.IndexInBoss;
    this->log=other.log;
  }
  bool DecreaseStackSetCharacterRanges(int decrease)
  { if (decrease<=0)
      return true;
    assert(((signed) this->expressionStacK.size())-decrease>0);
    this->expressionStacK[this->expressionStacK.size()-decrease-1].IndexLastCharPlusOne=
      this->expressionStacK[this->expressionStacK.size()-1].IndexLastCharPlusOne;
    this->expressionStacK.resize(this->expressionStacK.size()-decrease);
    return true;
  }
  void reset(CommandList* owner=0, int commandIndexInBoss=-1)
  { this->numEmptyTokensStart=9;
    this->expressionStacK.resize(0);
    this->syntacticSoup.resize(0);
    this->BoundVariables.reset();
    this->finalValue.reset(owner, -1);
    this->theBoss=owner;
    this->IndexInBoss=commandIndexInBoss;
    this->ErrorString="";
  }
  std::string ElementToString()
  { std::stringstream out;
    out << "Bound variables:<br>\n ";
    for (int  i=0; i<this->BoundVariables.size(); i++)
    { out << this->BoundVariables[i];
      if (i!=this->BoundVariables.size()-1)
        out << ", ";
    }
    if (this->theBoss==0)
    { out << "Element not initialized.";
      return out.str();
    }
    out << "<br>\nExpression stack no values (excluding empty tokens in the start): ";
    for (unsigned i=this->numEmptyTokensStart; i<this->expressionStacK.size(); i++)
    { out << this->expressionStacK[i].ElementToStringNoExpression(*this->theBoss);
      if (i!=this->expressionStacK.size()-1)
        out << ", ";
    }
    out << "<hr>\nSyntactic soup: ";
    for (unsigned i=0; i<this->syntacticSoup.size(); i++)
    { out << this->syntacticSoup[i].ElementToString(*this->theBoss);
      if (i!=this->syntacticSoup.size()-1)
        out << ", ";
    }
    out << "<br>\nExpression stack(excluding empty tokens in the start): ";
    for (unsigned i=this->numEmptyTokensStart; i<this->expressionStacK.size(); i++)
    { out << this->expressionStacK[i].ElementToString(*this->theBoss);
      if (i!=this->expressionStacK.size()-1)
        out << ", ";
    }
    out << "<br>\n Current value: " << this->finalValue.ElementToString(0,5, false);
    return out.str();
  }
  bool LookAheadAllowsThePower(const std::string& lookAhead)
  { return
      lookAhead!="{}"
      ;
  }  bool LookAheadAllowsPlus(const std::string& lookAhead)
  { return
      lookAhead=="+" || lookAhead=="-" ||
      lookAhead==")" || lookAhead==";" ||
      lookAhead=="]" || lookAhead=="}" ||
      lookAhead==":"
      ;
  }
  bool LookAheadAllowsTimes(const std::string& lookAhead)
  { return
      lookAhead=="+" || lookAhead=="-" ||
      lookAhead=="*" || lookAhead=="/" ||
      lookAhead=="e" || lookAhead==")" ||
      lookAhead==";" || lookAhead=="]" ||
      lookAhead=="}" || lookAhead==":"
      ;
  }
  bool ReplaceEXXEXEByEusingO(int theOp)
  { SyntacticElement& middle = this->expressionStacK[this->expressionStacK.size()-3];
    SyntacticElement& left = this->expressionStacK[this->expressionStacK.size()-6];
    SyntacticElement& right = this->expressionStacK[this->expressionStacK.size()-1];
    Expression newExpr;
    newExpr.reset(this->theBoss, this->IndexInBoss);
    newExpr.theOperation=theOp;
    newExpr.children.push_back(left.theData);
    newExpr.children.push_back(middle.theData);
    newExpr.children.push_back(right.theData);
    left.theData=newExpr;
    this->DecreaseStackSetCharacterRanges(5);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
    return true;
  }
  bool ReplaceEEByEusingO(int theOp)
  { SyntacticElement& left = this->expressionStacK[this->expressionStacK.size()-2];
    SyntacticElement& right = this->expressionStacK[this->expressionStacK.size()-1];
    Expression newExpr;
    newExpr.reset(this->theBoss, this->IndexInBoss);
    newExpr.theOperation=theOp;
//    newExpr.commandIndex=
    newExpr.children.push_back(left.theData);
    newExpr.children.push_back(right.theData);
    left.theData=newExpr;
    this->DecreaseStackSetCharacterRanges(1);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
    return true;
  }
  bool ReplaceOEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceEOEByE(int formatOptions=Expression::formatDefault)
  { SyntacticElement& middle=this->expressionStacK[this->expressionStacK.size()-2];
    SyntacticElement& left = this->expressionStacK[this->expressionStacK.size()-3];
    SyntacticElement& right = this->expressionStacK[this->expressionStacK.size()-1];
    Expression newExpr;
    newExpr.reset(this->theBoss, this->IndexInBoss);
    newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
    newExpr.formattingOptions=formatOptions;
    newExpr.children.push_back(left.theData);
    newExpr.children.push_back(right.theData);
    left.theData=newExpr;
    this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
    return true;
  }
  bool ReplaceXXByCon(int theCon, int theFormat=Expression::formatDefault)
  { this->expressionStacK[this->expressionStacK.size()-2].controlIndex=theCon;
    this->expressionStacK[this->expressionStacK.size()-2].theData.formattingOptions=theFormat;
    this->DecreaseStackSetCharacterRanges(1);
    return true;
  }
  bool ReplaceXByCon(int theCon, int theFormat=Expression::formatDefault)
  { this->expressionStacK[this->expressionStacK.size()-1].controlIndex=theCon;
    this->expressionStacK[this->expressionStacK.size()-1].theData.formattingOptions=theFormat;
//    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXXXByCon(int theCon)
  { this->expressionStacK[this->expressionStacK.size()-3].controlIndex=theCon;
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXXYByY()
  { this->expressionStacK[this->expressionStacK.size()-3]=this->expressionStacK[this->expressionStacK.size()-1];
    this->expressionStacK.resize(this->expressionStacK.size()-2);
    return true;
  }
  bool ReplaceXYXByY()
  { this->expressionStacK[this->expressionStacK.size()-3]=this->expressionStacK[this->expressionStacK.size()-2];
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXYByY()
  { this->expressionStacK[this->expressionStacK.size()-2]=this->expressionStacK[this->expressionStacK.size()-1];
    this->expressionStacK.resize(this->expressionStacK.size()-1);
    return true;
  }
  bool ReplaceXEXByE()
  { this->expressionStacK[this->expressionStacK.size()-3]=this->expressionStacK[this->expressionStacK.size()-2];
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceObyE();
  bool ReplaceVXbyEX();
  bool ReplaceVbyE();
  bool ReplaceIntIntBy10IntPlusInt()
  { SyntacticElement& left=this->expressionStacK[this->expressionStacK.size()-2];
    SyntacticElement& right=this->expressionStacK[this->expressionStacK.size()-1];
    left.theData.theData*=10;
    left.theData.theData+=right.theData.theData;
    this->DecreaseStackSetCharacterRanges(1);
    return true;
  }
  bool RegisterBoundVariable();
  int GetOperationIndexFromControlIndex(int controlIndex);
  int GetExpressionIndex();
  SyntacticElement GetEmptySyntacticElement();
  bool ApplyOneRule(const std::string& lookAhead);
  void resetStack()
  { SyntacticElement emptyElement=this->GetEmptySyntacticElement();
    this->expressionStacK.resize(this->numEmptyTokensStart, emptyElement);
  }
  bool SetError(const std::string& theError)
  { this->ErrorString=theError;
    return true;
  }
  Command()
  { this->reset();
  }
};

typedef bool (*standardExpressionEvaluation)(CommandList& theCommands, int commandIndex, Expression& theExpression);

class CommandList
{
public:
//control sequences parametrize the syntactical elements
  hashedVector<std::string, hashString> controlSequences;
//operations parametrize the expression elements
  hashedVector<std::string, hashString> operations;
  std::vector <standardExpressionEvaluation> theStandardEvaluationFunctions;
//used to parametrize the input data for the special operation "VariableNonBound"
  hashedVector<std::string, hashString> variableDictionary;
  int MaxRecursionDepthDefault;
  int MaxAlgTransformationsPerExpression;
  std::vector<Command> theCommands;
//  std::vector<std::stringstream> theLogs;

  hashedVector<std::string, hashString> cashedExpressions;
  std::vector <std::string> Errors;
  std::string input;
  std::string output;
  std::string theLog;
  std::string ElementToString();

  SyntacticElement GetSyntacticElementEnd()
  { SyntacticElement result;
    result.controlIndex=this->controlSequences.GetIndex(";");
    return result;
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
  int conDeclareFunction()
  { return this->controlSequences.GetIndexIMustContainTheObject("{}");
  }
  int conDefine()
  { return this->controlSequences.GetIndexIMustContainTheObject(":=");
  }
  int opFunction()
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
  int opVariableNonBound()
  { return this->operations.GetIndexIMustContainTheObject("VariableNonBound");
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
  void CarryOutOneSub
  (Expression& toBeSubbed, Expression& toBeSubbedWith, int targetCommandIndex, ExpressionPairs& matchedPairs)
  ;
  bool ExpressionHasBoundedVars(Expression& theExpression, int RecursionDepth, int MaxRecursionDepth)
  { if (RecursionDepth>MaxRecursionDepth)
    { std::stringstream out;
      out << "Max recursion depth of " << MaxRecursionDepth << " exceeded.";
      theExpression.errorString=out.str();
      return false;
    }
    if (theExpression.theOperation==this->opVariableBound())
      return true;
    else
      for (unsigned i=0; i<theExpression.children.size(); i++)
        if (this->ExpressionHasBoundedVars(theExpression.children[i], RecursionDepth+1, MaxRecursionDepth+1))
          return true;
    return false;
  }
  Expression* DepthFirstSubExpressionPatternMatch
  (int commandIndex, Expression& thePattern, Expression& theExpression,
   ExpressionPairs& bufferPairs, int RecursionDepth,
   int MaxRecursionDepth, Expression* condition=0, std::stringstream* theLog=0, bool logAttempts=false)
;
  bool ProcessOneExpressionOnePatternOneSub
  (int commandIndex, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   int RecursionDepth,
   int maxRecursionDepth, std::stringstream* theLog=0, bool logAttempts=false)
  ;
  bool isADigit(const std::string& input, int& whichDigit)
  { if (input.size()!=1)
      return false;
    whichDigit=input[0]-'0';
    return whichDigit<10 && whichDigit>=0;
  }
  int opIsInteger()
  { return this->operations.GetIndexIMustContainTheObject("IsInteger");
  }
  int opTimes()
  { return this->operations.GetIndexIMustContainTheObject("*");
  }
  int opDivide()
  { return this->operations.GetIndexIMustContainTheObject("/");
  }
  int opInteger()
  { return this->operations.GetIndexIMustContainTheObject("Integer");
  }
  bool ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, ExpressionPairs& matchedExpressions,
   int RecursionDepth=0, int MaxRecursionDepth=500, std::stringstream* theLog=0)
  ;
  bool ExpressionsAreEqual
  (const Expression& left, const Expression& right, int RecursionDepth=0, int MaxRecursionDepth=500)
  ;

  static bool EvaluateStandardPlus(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateStandardTimes(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateStandardMinus(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateIf(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateIsInteger(CommandList& theCommands, int commandIndex, Expression& theExpression);
  void AddEmptyHeadedCommand();
  CommandList()
  { this->controlSequences.reset();
    this->operations.reset();
    this->variableDictionary.reset();
    this->cashedExpressions.reset();
    this->theStandardEvaluationFunctions.resize(0);
    this->MaxRecursionDepthDefault=2000;

    this->operations.AddOnTop("+"); this->theStandardEvaluationFunctions.push_back(&this->EvaluateStandardPlus);
    this->operations.AddOnTop("-");  this->theStandardEvaluationFunctions.push_back(&this->EvaluateStandardMinus);
    this->operations.AddOnTop("/");  this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("*"); this->theStandardEvaluationFunctions.push_back(this->EvaluateStandardTimes);
    this->operations.AddOnTop(":="); this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("if:="); this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("^"); this->theStandardEvaluationFunctions.push_back(0);
    //the following two operations are chosen on purpose so that they correspond to LaTeX-undetectable
    //expressions
    //the following is the binding variable operation
    this->operations.AddOnTop("{{}}"); this->theStandardEvaluationFunctions.push_back(0);
    //the following is the operation for using a variable as a function
    this->operations.AddOnTop("{}"); this->theStandardEvaluationFunctions.push_back(0);
    //the following two operations are aliases for the operation {}
    this->operations.AddOnTop("\\cdot"); this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("_"); this->theStandardEvaluationFunctions.push_back(0);

    this->operations.AddOnTop("IsInteger"); this->theStandardEvaluationFunctions.push_back(&this->EvaluateIsInteger);
    this->operations.AddOnTop("if"); this->theStandardEvaluationFunctions.push_back(&this->EvaluateIf);
    this->operations.AddOnTop("Integer"); this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("VariableNonBound"); this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("VariableBound"); this->theStandardEvaluationFunctions.push_back(0);
//    this->operations.AddOnTop("AnyInteger"); this->theStandardEvaluationFunctions.push_back(0);
    this->operations.AddOnTop("Error"); this->theStandardEvaluationFunctions.push_back(0);

    this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
    this->controlSequences.AddOnTop("Variable");
    this->controlSequences.AddOnTop(this->operations);//all operations are also control sequences
    this->controlSequences.AddOnTop("Expression");
    this->controlSequences.AddOnTop(",");
    this->controlSequences.AddOnTop("(");
    this->controlSequences.AddOnTop(")");
    this->controlSequences.AddOnTop("[");
    this->controlSequences.AddOnTop("]");
    this->controlSequences.AddOnTop("{");
    this->controlSequences.AddOnTop("}");
    this->controlSequences.AddOnTop(":");
    this->controlSequences.AddOnTop("=");
    this->controlSequences.AddOnTop(";");//exression end
  }
  void ExtractExpressions()
  { std::string lookAheadToken;
    for (unsigned j=0; j<this->theCommands.size(); j++)
    { this->theCommands[j].resetStack();
      for (unsigned i=0; i<this->theCommands[j].syntacticSoup.size(); i++)
      { if (i+1<this->theCommands[j].syntacticSoup.size())
          lookAheadToken=this->controlSequences[this->theCommands[j].syntacticSoup[i+1].controlIndex];
        else
          lookAheadToken=";";
        this->theCommands[j].expressionStacK.push_back(this->theCommands[j].syntacticSoup[i]);
        while(this->theCommands[j].ApplyOneRule(lookAheadToken))
        {}
      }
      if (this->theCommands[j].ErrorString=="" && (signed)(this->theCommands[j].expressionStacK.size())==this->theCommands[j].numEmptyTokensStart+1)
        this->theCommands[j].finalValue=this->theCommands[j].expressionStacK[this->theCommands[j].numEmptyTokensStart].theData;
    }
  }
  void EvaluateCommands();
  void EvaluateExpression
(int commandIndex, Expression& theExpression, int RecursionDepth, int maxRecursionDepth,
 ExpressionPairs& bufferPairs,
 std::stringstream* theLog=0)
 ;
  void Evaluate(const std::string& theInput)
  { std::vector<Command> startingExpressions;
    this->input=theInput;
    this->ParseFillDictionary(this->input);
    this->ExtractExpressions();
    this->EvaluateCommands();
  }
  bool isLeftSeparator(char c);
  bool isRightSeparator(char c);
  void ParseFillDictionary(const std::string& input);
};

bool Command::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=this->expressionStacK[this->expressionStacK.size()-8];
  SyntacticElement& leftE = this->expressionStacK[this->expressionStacK.size()-6];
  SyntacticElement& middleE= this->expressionStacK[this->expressionStacK.size()-4];
  SyntacticElement& rightE = this->expressionStacK[this->expressionStacK.size()-2];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.formattingOptions=formatOptions;
  newExpr.children.push_back(leftE.theData);
  newExpr.children.push_back(middleE.theData);
  newExpr.children.push_back(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(7);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOXXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=this->expressionStacK[this->expressionStacK.size()-9];
  SyntacticElement& leftE = this->expressionStacK[this->expressionStacK.size()-6];
  SyntacticElement& middleE= this->expressionStacK[this->expressionStacK.size()-4];
  SyntacticElement& rightE = this->expressionStacK[this->expressionStacK.size()-2];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.formattingOptions=formatOptions;
  newExpr.children.push_back(leftE.theData);
  newExpr.children.push_back(middleE.theData);
  newExpr.children.push_back(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(8);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOXEByE(int formatOptions)
{ SyntacticElement& left=this->expressionStacK[this->expressionStacK.size()-3];
  SyntacticElement& right = this->expressionStacK[this->expressionStacK.size()-1];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(left.controlIndex);
  newExpr.formattingOptions=formatOptions;
  newExpr.children.push_back(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOEByE(int formatOptions)
{ SyntacticElement& middle=this->expressionStacK[this->expressionStacK.size()-2];
  SyntacticElement& right = this->expressionStacK[this->expressionStacK.size()-1];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
  newExpr.formattingOptions=formatOptions;
  newExpr.children.push_back(right.theData);
  middle.theData=newExpr;
  middle.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(1);
//    std::cout << this->expressionStacK[this->expressionStacK.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::isRightSeparator(char c)
{ switch(c)
  { case ' ':
    case '\n':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '=':
    case '_':
    case '/':
    case '-':
    case '`':
    case '[':
    case '}':
    case '{':
    case '~':
    case ']':
    case '(':
    case ')':
    case '\\':
      return true;
    default:
      return false;
  }
}

bool CommandList::isLeftSeparator(char c)
{ switch(c)
  { case ' ':
    case '\n':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '=':
    case '_':
    case '/':
    case '-':
    case '`':
    case '[':
    case '}':
    case '{':
    case '~':
    case ']':
    case '(':
    case ')':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
  }
}

SyntacticElement Command::GetEmptySyntacticElement()
{ SyntacticElement result;
  result.controlIndex=this->theBoss->controlSequences.GetIndex(" ");
  result.theData.reset(0,-1);
  return result;
}

void CommandList::AddEmptyHeadedCommand()
{ this->theCommands.resize(this->theCommands.size()+1);
  Command& currentCommand=this->theCommands[this->theCommands.size()-1];
  currentCommand.reset(this, this->theCommands.size()-1);
}

std::string ExpressionPairs::ElementToString()
{ std::stringstream out;
  out << "the pairs: ";
  for (int i=0; i<this->left.size(); i++)
    out << this->left[i].ElementToString() << "->" << this->right[i].ElementToString() << "<br>";
  return out.str();
}

void CommandList::ParseFillDictionary
  (const std::string& input)
{ std::string current;
  this->theCommands.reserve(1000);
  this->theCommands.resize(0);
  char LookAheadChar;
  this->AddEmptyHeadedCommand();
  SyntacticElement currentElement;
  bool lastStatementProperlyTerminated;
  int currentDigit;
  for (unsigned i=0; i<input.size(); i++)
  { lastStatementProperlyTerminated=false;
    current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if ((current=="\\" && LookAheadChar=='\\')|| current==";")
    { this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(this->GetSyntacticElementEnd());
      this->AddEmptyHeadedCommand();
      lastStatementProperlyTerminated=true;
    }
    if (current==" " || current=="\n")
      current="";
    else
      if (this->isLeftSeparator(current[0]) || this->isRightSeparator(LookAheadChar) )
      { if (this->controlSequences.Contains(current))
        { currentElement.controlIndex=this->controlSequences.GetIndex(current);
          currentElement.theData.reset(this, -1);
          this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(currentElement);
        } else if (this->isADigit(current, currentDigit))
        { currentElement.controlIndex=this->conInteger();
          currentElement.theData.reset(this, -1);
          currentElement.theData.theOperation=this->opInteger();
          currentElement.theData.theData=currentDigit;
          this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(currentElement);
        } else
        { this->variableDictionary.AddNoRepetition(current);
          currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
          currentElement.theData.reset(this, this->theCommands.size()-1);
          currentElement.theData.theOperation=this->operations.GetIndex("Variable");
          currentElement.theData.theData=this->variableDictionary.GetIndex(current);
          this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(currentElement);
        }
        current="";
      }
  }
  if (!lastStatementProperlyTerminated)
    this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(this->GetSyntacticElementEnd());
}

int Command::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->theBoss->operations.GetIndex(this->theBoss->controlSequences[controlIndex]);
}

int Command::GetExpressionIndex()
{ return this->theBoss->controlSequences.GetIndex("Expression");
}

bool Command::ReplaceObyE()
{ SyntacticElement& theElt=this->expressionStacK[this->expressionStacK.size()-1];
  theElt.theData.theOperation=this->theBoss->operations.GetIndex(this->theBoss->controlSequences[theElt.controlIndex]);
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::ReplaceVbyE()
{ SyntacticElement& theElt=this->expressionStacK[this->expressionStacK.size()-1];
  const std::string& theVarString=this->theBoss->variableDictionary[theElt.theData.theData];
  int indexBoundVar=this->BoundVariables.GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->theBoss->opVariableBound();
    theElt.theData.theData=indexBoundVar;
  } else
  { theElt.theData.theOperation=this->theBoss->opVariableNonBound();
    //note:     theElt.theData.theOperation.theData     should be initialized with the index of the non-bound variable!
  }
//  std::cout << "now i'm here!";
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::ReplaceVXbyEX()
{ SyntacticElement& theElt=this->expressionStacK[this->expressionStacK.size()-2];
  const std::string& theVarString=this->theBoss->variableDictionary[theElt.theData.theData];
  int indexBoundVar=this->BoundVariables.GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->theBoss->opVariableBound();
    theElt.theData.theData=indexBoundVar;
  } else
  { theElt.theData.theOperation=this->theBoss->opVariableNonBound();
    //note:     theElt.theData.theOperation.theData     should be initialized with the index of the non-bound variable!
  }
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::RegisterBoundVariable()
{ SyntacticElement& theElt=this->expressionStacK[this->expressionStacK.size()-3];
  const std::string& theVarString=this->theBoss->variableDictionary[theElt.theData.theData];
  if (this->BoundVariables.Contains(theVarString))
  { theElt.controlIndex=this->theBoss->controlSequences.GetIndex("Error");
    theElt.ErrorString= "Variable " + theVarString + " registered as bounded more than once";
    return this->SetError("");
  }
  this->BoundVariables.AddOnTop(theVarString);
  this->DecreaseStackSetCharacterRanges(2);
  this->ReplaceXXYByY();
  return true;
}

bool Command::ApplyOneRule(const std::string& lookAhead)
{ //return false;
  const SyntacticElement& lastE=this->expressionStacK[this->expressionStacK.size()-1];
  const std::string&    lastS=this->theBoss->controlSequences[lastE.controlIndex];
  if (lastS==" " && signed (this->expressionStacK.size())>this->numEmptyTokensStart)
  { this->expressionStacK.resize(this->expressionStacK.size()-1);
    return false;
  }
  const SyntacticElement& secondToLastE=this->expressionStacK[this->expressionStacK.size()-2];
  const std::string& secondToLastS=this->theBoss->controlSequences[secondToLastE.controlIndex];
  const SyntacticElement& thirdToLastE=this->expressionStacK[this->expressionStacK.size()-3];
  const std::string& thirdToLastS=this->theBoss->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE=this->expressionStacK[this->expressionStacK.size()-4];
  const std::string& fourthToLastS=this->theBoss->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE=this->expressionStacK[this->expressionStacK.size()-5];
  const std::string& fifthToLastS=this->theBoss->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE=this->expressionStacK[this->expressionStacK.size()-6];
  const std::string& sixthToLastS=this->theBoss->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE=this->expressionStacK[this->expressionStacK.size()-7];
  const std::string& seventhToLastS=this->theBoss->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE=this->expressionStacK[this->expressionStacK.size()-8];
  const std::string& eighthToLastS=this->theBoss->controlSequences[eighthToLastE.controlIndex];
//  const SyntacticElement& ninthToLastE=this->expressionStacK[this->expressionStacK.size()-9];
//  const std::string& ninthToLastS=this->theBoss->controlSequences[ninthToLastE.controlIndex];

//  std::cout << "<hr>" << this->ElementToString();
  if (secondToLastS==":" && lastS=="=")
    return this->ReplaceXXByCon(this->theBoss->conDefine());
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->theBoss->conDeclareFunction());
  if (lastS=="_")
    return this->ReplaceXByCon(this->theBoss->conDeclareFunction(), Expression::formatFunctionUseUnderscore);
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->theBoss->conDeclareFunction(), Expression::formatFunctionUseCdot);
//  if ( thirdToLastS=="{" && secondToLastS=="{}" && lastS=="}")
//    return this->ReplaceXXXByCon(this->theBoss->conBindVariable());
  if (fifthToLastS=="{" && fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS=="}")
    return this->RegisterBoundVariable();
  if (lastS=="Variable" && lookAhead!="}")
    return this->ReplaceVbyE();
  if (lastS==";")
    return this->DecreaseStackSetCharacterRanges(1);
  if (lastS=="Integer" && secondToLastS=="Integer")
    return this->ReplaceIntIntBy10IntPlusInt();
  if (lastS=="Integer" && lookAhead!="Integer")
    return this->ReplaceObyE();
  if (thirdToLastS=="Expression" && secondToLastS=="{}" && lastS=="Expression")
    return this->ReplaceEOEByE(secondToLastE.theData.formattingOptions);
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXYXByY();
  if (eighthToLastS=="if" && seventhToLastS=="(" && sixthToLastS=="Expression"
      && fifthToLastS=="," && fourthToLastS=="Expression" && thirdToLastS==","
      && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceOXEXEXEXByE();
  if (secondToLastS=="IsInteger" && lastS=="Expression")
    return this->ReplaceOEByE();
  if (lastS=="Expression" && secondToLastS=="^" && thirdToLastS=="Expression" && this->LookAheadAllowsThePower(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="+" && thirdToLastS=="Expression" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="-" && thirdToLastS=="Expression" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="-" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceOEByE();
  if (lastS=="Expression" && secondToLastS=="*" && thirdToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEOEByE(Expression::formatTimesDenotedByStar);
  if (lastS=="Expression" && secondToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEEByEusingO(this->theBoss->opTimes());
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE();
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (fourthToLastS==" " && lookAhead==";" && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression")
    return this->ReplaceEOEByE();
  if (seventhToLastS==" " && lookAhead==";" && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression"
      && fourthToLastS=="if" && fifthToLastS==":" && sixthToLastS=="Expression")
    return this->ReplaceEXXEXEByEusingO(this->theBoss->opDefineConditional());
  return false;
}

bool CommandList::EvaluateStandardTimes
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size()!=2)
  { theExpression.errorString="Programming error: operation * always takes two arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  Expression& right=theExpression.children[1];
  if (left.theOperation==theCommands.opInteger() && right.theOperation==theCommands.opInteger())
  { theExpression.theOperation=theCommands.opInteger();
    theExpression.theData=left.theData*right.theData;
    theExpression.children.resize(0);
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardPlus
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size()!=2)
  { theExpression.errorString="Programming error: operation + always takes two arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  Expression& right=theExpression.children[1];
  bool result=false;
  if (left.theOperation==theCommands.opInteger()  && right.theOperation!=theCommands.opInteger())
  { std::swap(theExpression.children[0], theExpression.children[1]);
    result=true;
  }
  if (left.theOperation==theCommands.opInteger() && right.theOperation==theCommands.opInteger())
  { theExpression.theOperation=theCommands.opInteger();
    theExpression.theData=left.theData+right.theData;
    theExpression.children.resize(0);
    return true;
  }
  //associativity with respect to integers
  if (left.theOperation==theCommands.opPlus() && right.theOperation==theCommands.opInteger())
  { if (left.children.size()!=2)
    { assert(false);
      return false;
    }
    if (left.children[1].theOperation==theCommands.opInteger())
    { left.children[1].theData+=right.theData;
      Expression tempExp=left;
      theExpression=tempExp;
      return true;
    }
  }
  return result;
}

bool CommandList::EvaluateIsInteger
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ bool isInteger=true;
  assert(theExpression.theBoss==&theCommands);
  if (theCommands.ExpressionHasBoundedVars(theExpression, 0, theCommands.MaxRecursionDepthDefault))
    return false;
  if (theExpression.children.size()!=1)
    isInteger=false;
  else
    isInteger= (theExpression.children[0].theOperation==theCommands.opInteger());
  theExpression.children.resize(0);
  theExpression.theOperation=theCommands.opInteger();
  if (isInteger)
    theExpression.theData=1;
  else
    theExpression.theData=0;
  return true;
}

bool CommandList::EvaluateIf
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size()!=3)
  { theExpression.errorString="Programming error: operation - takes three arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  if (left.theOperation!=theCommands.opInteger())
    return false;
  if (left.theData==1)
  { theExpression.AssignMeMyChild(1);
    return true;
  }
  if (left.theData==0)
  { theExpression.AssignMeMyChild(2);
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardMinus
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size()!=1&& theExpression.children.size()!=2)
  { theExpression.errorString="Programming error: operation - takes one or two arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  if (theExpression.children.size()==1)
  { if (left.theOperation==theCommands.opInteger() )
    { theExpression.theData=-left.theData;
      theExpression.theOperation=theCommands.opInteger();
      theExpression.children.resize(0);
      return true;
    }
    return false;
  }
  Expression& right=theExpression.children[1];
  if (left.theOperation==theCommands.opInteger() && right.theOperation==theCommands.opInteger())
  { theExpression.theOperation=theCommands.opInteger();
    theExpression.theData=left.theData-right.theData;
    theExpression.children.resize(0);
    return true;
  }
  return false;
}

bool Expression::operator==
(const Expression& other)const
{ if (this->theBoss==0)
    assert(false);
  return this->theBoss->ExpressionsAreEqual(*this, other);
}

bool CommandList::ExpressionsAreEqual
  (const Expression& left, const Expression& right, int RecursionDepth, int MaxRecursionDepth)
{ if (RecursionDepth>MaxRecursionDepth)
  { std::stringstream out;
    out << "Error: maximium recursion depth of " << MaxRecursionDepth << " exceeded while comparing expressions: " ;
    out << left.ElementToString() << " and " << right.ElementToString();
    std::cout << out.str();
    this->Errors.push_back(out.str());
    return false;
  }
  if (left.theBoss!=this || right.theBoss!=this || left.theData!=right.theData || left.theOperation!=right.theOperation ||
        left.children.size()!=right.children.size())
    return false;
  for (unsigned i=0; i<left.children.size(); i++)
    if (!this->ExpressionsAreEqual (left.children[i], right.children[i], RecursionDepth+1, MaxRecursionDepth))
      return false;
  return true;
}

bool CommandList::ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, ExpressionPairs& matchedExpressions,
   int RecursionDepth, int MaxRecursionDepth, std::stringstream* theLog)
{ ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  assert(thePattern.theBoss==this && input.theBoss==this);
  static int ExpressionMatchesPatternDebugCounter=-1;
  static bool printLocalDebugInfo=false;
  ExpressionMatchesPatternDebugCounter++;
//  std::cout << " ExpressionMatchesPatternDebugCounter: " << ExpressionMatchesPatternDebugCounter;
//  printLocalDebugInfo=(ExpressionMatchesPatternDebugCounter>-1);
  if (printLocalDebugInfo)
  { std::cout << " <hr> current input: " << input.ElementToString() << "<br>current pattern: " << thePattern.ElementToString();
    std::cout << "<br> current matched expressions: " << matchedExpressions.ElementToString();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (RecursionDepth>MaxRecursionDepth)
  { std::stringstream out;
    out << "Max recursion depth of " << MaxRecursionDepth << " exceeded whlie trying to match expression pattern "
    << thePattern.ElementToString() << " onto expression " << input.ElementToString();
    this->Errors.push_back(out.str());
    return false;
  }
  if (this->opDefine()==input.theOperation)
    return false;
  int opVarB=this->opVariableBound();
  if (thePattern.theOperation== opVarB)
  { int indexLeft= matchedExpressions.left.GetIndex(thePattern);
    if (indexLeft==-1)
    { matchedExpressions.left.AddOnTop(thePattern);
      matchedExpressions.right.AddOnTop(input);
      indexLeft=matchedExpressions.left.size()-1;
    }
    if (!(matchedExpressions.right[indexLeft]==input))
      return false;
    if (printLocalDebugInfo)
      std::cout << "<br><b>Match!</b>";
    return true;
  }
  if (thePattern.theOperation!=input.theOperation || thePattern.theData!=input.theData || thePattern.children.size()!=input.children.size() )
    return false;
  for (unsigned i=0; i<thePattern.children.size(); i++)
    if (!(this->ExpressionMatchesPattern
    (thePattern.children[i], input.children[i], matchedExpressions, RecursionDepth+1, MaxRecursionDepth, theLog)))
      return false;
  if (printLocalDebugInfo)
    std::cout << "<br><b>Match!</b>";
  return true;
}

void CommandList::EvaluateExpression
(int commandIndex, Expression& theExpression, int RecursionDepth, int maxRecursionDepth,
 ExpressionPairs& bufferPairs,
 std::stringstream* theLog)
{ if (RecursionDepth>=maxRecursionDepth)
  { std::stringstream out;
    out << "Recursion depth limit of " << this->MaxRecursionDepthDefault << " exceeded while evaluating expressions.";
    theExpression.errorString=out.str();
    this->Errors.push_back(out.str());
    return;
  }
  if (theExpression.theOperation<0 || theExpression.theBoss!=this)
  { return;
  }
  //reduction phase:
  bool NonReduced=true;
  int counter=-1;
  while (NonReduced)
  { counter++;
    NonReduced=false;
    for (unsigned i=0; i<theExpression.children.size(); i++)
      this->EvaluateExpression(commandIndex, theExpression.children[i], RecursionDepth+1, maxRecursionDepth, bufferPairs, theLog);
    if (counter>this->MaxAlgTransformationsPerExpression)
    { std::stringstream out;
      out << "Maximum number of algebraic transformations of " << this->MaxAlgTransformationsPerExpression << " exceeded.";
      theExpression.errorString=out.str();
      return;
    }
    assert((signed) this->theStandardEvaluationFunctions.size()> theExpression.theOperation);
    if (this->theStandardEvaluationFunctions[theExpression.theOperation]!=0)
      if (this->theStandardEvaluationFunctions[theExpression.theOperation](*this, commandIndex, theExpression))
        NonReduced=true;
    for (int i=0; i<commandIndex; i++)
      if (this->theCommands[i].ErrorString=="")
      { Expression& currentPattern=this->theCommands[i].finalValue;
        if (currentPattern.errorString=="")
        { if (currentPattern.theOperation==this->opDefine() || currentPattern.theOperation==this->opDefineConditional())
            if(this->ProcessOneExpressionOnePatternOneSub
            (commandIndex, currentPattern, theExpression, bufferPairs, RecursionDepth+1, maxRecursionDepth, theLog))
            { NonReduced=true;
              break;
            }
        }
      }
  }
}

Expression* CommandList::DepthFirstSubExpressionPatternMatch
  (int commandIndex, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   int RecursionDepth, int MaxRecursionDepth, Expression* condition, std::stringstream* theLog, bool logAttempts)
{ if (RecursionDepth>=MaxRecursionDepthDefault)
  { std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of " << MaxRecursionDepth
    << " was exceeded";
    theExpression.errorString=out.str();
    return 0;
  }
  static int patternMatchDebugCounter=-1;
 // if (RecursionDepth==0)
    patternMatchDebugCounter++;

  if (theLog!=0 && logAttempts)
    if (RecursionDepth==0)
    { patternMatchDebugCounter++;
      (*theLog) << "<hr>(" << patternMatchDebugCounter << ") attempting to match pattern "
      << thePattern.ElementToString();
    }
//  if (patternMatchDebugCounter==0)
//  { std::cout << "<br>problem starts here:";
//    std::cout << "<br>" << bufferPairs.ElementToString();
//    std::cout << "<br>the expression: " << theExpression.ElementToString();
//  }
  if (patternMatchDebugCounter==10480)
  { std::cout <<"<hr>" << thePattern.ElementToString();
    std::cout << "<hr>problem starts here! ";
  }
  if (this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, RecursionDepth+1, MaxRecursionDepth, theLog))
  { if (theLog!=0)
    { if (patternMatchDebugCounter==10480)
      { std::cout << "<hr><hr>" << theExpression.ElementToString();
        std::cout << thePattern.ElementToString();
      //  assert(false);
      }
//      (*theLog) << "<hr>found pattern: " << theExpression.ElementToString() << " -> " << thePattern.ElementToString();
    }
    if (condition==0)
      return &theExpression;
    else
    { Expression tempExp=*condition;
      this->EvaluateExpression(commandIndex, tempExp, RecursionDepth+1, MaxRecursionDepth, bufferPairs, theLog);
      if (tempExp.theOperation==this->opInteger())
        if (tempExp.theData==1)
          return & theExpression;
    }
  }
  Expression* result=0;
  for (unsigned i=0; i<theExpression.children.size(); i++)
  { bufferPairs.reset();
    result=this->DepthFirstSubExpressionPatternMatch
    (commandIndex, thePattern, theExpression.children[i], bufferPairs,
     RecursionDepth+1, MaxRecursionDepth,  condition, theLog);
    if (result!=0)
      return result;
  }
  return result;
}

void CommandList::CarryOutOneSub
  (Expression& toBeSubbed, Expression& toBeSubbedWith, int targetCommandIndex,
   ExpressionPairs& matchedPairs)
{ toBeSubbed.theBoss=this;
  toBeSubbed.commandIndex=targetCommandIndex;
  toBeSubbed.formattingOptions=toBeSubbedWith.formattingOptions;
  int indexMatching= matchedPairs.left.GetIndex(toBeSubbedWith);
  if (indexMatching!=-1)
  { toBeSubbed=matchedPairs.right[indexMatching];
    return;
  }
  toBeSubbed.theOperation=toBeSubbedWith.theOperation;
  toBeSubbed.theData=toBeSubbedWith.theData;
  toBeSubbed.children.resize(toBeSubbedWith.children.size());
  for (unsigned i=0; i<toBeSubbedWith.children.size(); i++)
    this->CarryOutOneSub(toBeSubbed.children[i], toBeSubbedWith.children[i], targetCommandIndex, matchedPairs);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
  (int commandIndex, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   int RecursionDepth,
   int maxRecursionDepth, std::stringstream* theLog, bool logAttempts)
{ assert(thePattern.theOperation==this->opDefine() ||
  thePattern.theOperation==this->opDefineConditional());
  assert(thePattern.children.size()==2 || thePattern.children.size()==3);
  if (theLog!=0 && logAttempts)
  { (*theLog) << "<hr>attempting to reduce expression " << theExpression.ElementToString();
    (*theLog) << " by pattern " << thePattern.ElementToString();
  }
  Expression& currentPattern=thePattern.children[0];
  Expression* theCondition=0;
  bool isConditionalDefine=thePattern.theOperation==this->opDefineConditional();
  if (isConditionalDefine)
    theCondition=&thePattern.children[1];
  Expression* toBeSubbed=0;
  if (thePattern.theOperation==this->opDefine() || isConditionalDefine)
    toBeSubbed=this->DepthFirstSubExpressionPatternMatch
    (commandIndex, currentPattern, theExpression, bufferPairs, RecursionDepth+1, maxRecursionDepth,
     theCondition, theLog);
  if (toBeSubbed==0)
    return false;
  this->CarryOutOneSub(*toBeSubbed, thePattern.children[1], toBeSubbed->commandIndex, bufferPairs);
  return true;
}

void CommandList::EvaluateCommands()
{ std::stringstream out;
  ExpressionPairs thePairs;
//  this->theLogs.resize(this->theCommands.size());
  std::stringstream loggingStream;
  loggingStream << "<b>Debugging information.</b><br> Input:<br> " << this->input << "<hr>";
  out << "<table cellspacing=\"20\">\n";
  for (unsigned i=0; i<this->theCommands.size(); i++)
  { out << "<tr><td>";
    out << "Command_" << i+1 << ": </td>";
    std::stringstream localLogger;
    assert((int)this->theCommands[0].expressionStacK[6].theData.theBoss!=-1);
    if (this->theCommands[i].ErrorString=="")
    { this->EvaluateExpression(i, this->theCommands[i].finalValue, 0, 10, thePairs, &localLogger);
      assert((int)this->theCommands[0].expressionStacK[6].theData.theBoss!=-1);
    }
    std::string commandOutput=this->theCommands[i].finalValue.ElementToString();
    assert((int)this->theCommands[0].expressionStacK[6].theData.theBoss!=-1);
    out << "<td><span class=\"math\">" << commandOutput << "</span></td>";
    out << "<td>" << commandOutput;
    if (i!=this->theCommands.size()-1)
      out << ";";
    out << "</td>";
//    out << " ( for debugging: " << this->theCommands[i].finalValue.ElementToStringPolishForm() << ")";
    this->theCommands[i].theLog= localLogger.str();
    loggingStream << "<hr><hr>Command " << i+1 << " log: " << this->theCommands[i].theLog;
    //if (i!=this->theCommands.size()-1)
    out << "</tr>";
    if (this->Errors.size()>0)
    { out << "<tr><td>Errors encountered; command evaluation terminated. Error messages follow.";
      for (unsigned i=0; i<this->Errors.size(); i++)
        out << "<br>" << this->Errors[i];
      out << "</td></tr>";
      break;
    }
  }
  out << "</table>";
  loggingStream << "<hr><hr><b>CommandList status. </b><hr>";
  loggingStream << this->ElementToString();
  this->theLog=loggingStream.str();
  this->output=out.str();
}

std::string SyntacticElement::ElementToString
(CommandList& theBoss)
{ std::stringstream out;
  out << "(" << this->ElementToStringNoExpression(theBoss);
  if (this->theData.theOperation!=-1)
  { out << " -> ";
    out << this->theData.ElementToString(0, 10);
  }
  out << ")";
  return out.str();
}

std::string SyntacticElement::ElementToStringNoExpression(CommandList& theBoss)
{ std::stringstream out;
  out << theBoss.controlSequences[this->controlIndex];
  return out.str();
}

std::string Expression::ElementToString(int recursionDepth, int maxRecursionDepth, bool AddBrackets, bool AddCurlyBraces)const
{ if (maxRecursionDepth>0)
    if(recursionDepth>maxRecursionDepth)
      return "(...)";
  if (this->theBoss==0)
    return "(non-initialized)";
  assert((int)(this->theBoss)!=-1);
  std::stringstream out;
  if (this->theOperation<0)
  { out << "(operation not initialized)";
    return out.str();
  }
  if (this->errorString!="")
  { out << "This expression contains an error! The error message follows<br> " << this->errorString
    << "<br>In addition, the value of the expression is: ";
  }
  if (AddBrackets)
    out << "(";
  if (AddCurlyBraces)
    out << "{";
  if (this->theOperation==this->theBoss->opDefine())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth)
    << ":=" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth);
  else if (this->theOperation==this->theBoss->opDefineConditional())
    out <<  this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth) << " :if "
    << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth)
    << ":=" << this->children[2].ElementToString(recursionDepth+1, maxRecursionDepth);
  else if (this->theOperation==this->theBoss->opDivide() )
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication())
    << "/" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForMultiplication());
  else if (this->theOperation==this->theBoss->opTimes() )
  { out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication());
    if (this->formattingOptions==this->formatTimesDenotedByStar)
      out << "*"; else out << " ";
    out << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForMultiplication());
  }
  else if (this->theOperation==this->theBoss->opThePower())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForThePower())
    << "^{" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false) << "}";
  else if (this->theOperation==this->theBoss->opPlus() )
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition())
    << "+" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForAddition());
  else if (this->theOperation==this->theBoss->opMinus())
  { if ( this->children.size()==1)
      out << "-" << this->children[0].ElementToString
      (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication());
    else
      out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition())
      << "-" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition());
  }
  else if (this->theOperation==this->theBoss->opVariableBound())
  { assert((unsigned) this->commandIndex< this->theBoss->theCommands.size());
    assert(this->theBoss->theCommands[this->commandIndex].BoundVariables.size()>0);
    assert(this->theBoss->theCommands[this->commandIndex].BoundVariables.size()>this->theData);
    out << "{{" << this->theBoss->theCommands[this->commandIndex].BoundVariables[this->theData] << "}}";
  }
  else if (this->theOperation==this->theBoss->opVariableNonBound())
    out << this->theBoss->variableDictionary[this->theData];
  else if (this->theOperation==this->theBoss->opInteger())
    out << this->theData;
  else if (this->theOperation==this->theBoss->opFunction())
  { out << this->children[0].ElementToString
    (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForFunctionName());
    switch(this->formattingOptions)
    { case Expression::formatFunctionUseUnderscore:
        out << "_{" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth) << "}";
        break;
      case Expression::formatFunctionUseCdot:
        out << "\\cdot(" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth) << ")";
        break;
      default:
        out << "{}(" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth) << ")";
        break;
    }
  }
  else if (this->theOperation==this->theBoss->opIsInteger())
    out << "IsInteger(" << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth) << ")";
  else
    out << "?operation not documented?" ;
  if (AddBrackets)
    out << ")";
  if (AddCurlyBraces)
    out << "}";
  return out.str();
}

std::string Expression::ElementToStringPolishForm(int recursionDepth, int maxRecursionDepth)
{ if (maxRecursionDepth>0)
    if(recursionDepth>maxRecursionDepth)
      return "...";
  if (this->theBoss==0)
    return " non-initialized ";
  std::stringstream out;
  if (this->theOperation<0)
  { out << " operation not initialized ";
    return out.str();
  }
  out << "(" << this->theBoss->operations[this->theOperation] << " (operation index: " << this->theOperation << ")";
  out << ", " << this->theData << ")";
  if (this->children.size()>0)
  { out << "{ ";
    for (unsigned i=0; i<this->children.size(); i++)
    { out << this->children[i].ElementToStringPolishForm(recursionDepth+1, maxRecursionDepth);
      if (i!=this->children.size()-1)
        out << ", ";
    }
    out << "}";
  }
  return out.str();
}

std::string CommandList::ElementToString()
{ std::stringstream out;
  out << "Control sequences (" << this->controlSequences.size() << " of them):\n<br>\n";
  for (int i=0; i<this->controlSequences.size(); i++)
  { out << this->controlSequences[i] << ", ";
  }
  out << "<br>\nOperators (" << this->operations.size() << " of them):\n<br>\n";
  for (int i=0; i<this->operations.size(); i++)
  { out << this->operations[i] << ", ";
  }
  out << "<br>\n Variables (" << this->variableDictionary.size() << " of them):\n<br>\n";
  for (int i=0; i<this->variableDictionary.size(); i++)
  { out << this->variableDictionary[i] << ", ";
  }
  out << "<br>\n Cashed expressions: (" << this->cashedExpressions.size() << " of them):\n<br>\n";
  for (int i=0; i<this->cashedExpressions.size(); i++)
  { out << this->cashedExpressions[i] << ", ";
  }
  for (unsigned i=0; i<this->theCommands.size(); i++)
  { out << this->theCommands[i].ElementToString();
  }
  return out.str();
}

bool Expression::NeedBracketsForFunctionName() const
{ return !(
  this->theOperation==this->theBoss->opVariableBound() ||
  this->theOperation==this->theBoss->opVariableNonBound() ||
  (this->theOperation==this->theBoss->opFunction() && this->formattingOptions==this->formatFunctionUseUnderscore)
  );
}

bool Expression::NeedBracketsForMultiplication()const
{ return
  this->theOperation==this->theBoss->opPlus() ||
  this->theOperation==this->theBoss->opMinus()
  ;
}
bool Expression::NeedBracketsForAddition()const
{ return
  false
  ;
}
bool Expression::NeedBracketsForThePower()const
{ return
  this->theOperation==this->theBoss->opPlus() ||
  this->theOperation==this->theBoss->opMinus() ||
  this->theOperation==this->theBoss->opTimes() ||
  this->theOperation==this->theBoss->opDivide()
  ;
}

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void getPath(char* path, std::string& output)
{ if (path==0)
    return;
  int length=0;
  output= path;
  while (output[length]!=0 && length<150)
    length++;
  for (int i=length-1; i>=0; i--)
    if (output[i]=='/')
    { output.resize(i+1);
      return;
    }
}

timeval ComputationStartTime, lastTimeMeasure;

double GetElapsedTimeInSeconds()
{ gettimeofday(&lastTimeMeasure, NULL);
  int miliSeconds =(lastTimeMeasure.tv_sec- ComputationStartTime.tv_sec)*1000+(lastTimeMeasure.tv_usec- ComputationStartTime.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

bool AttemptToCivilize(std::string& readAhead, std::stringstream& out)
{ if (readAhead[0]!='%' && readAhead[0]!='&' && readAhead[0]!='+')
  { out << readAhead[0];
    return true;
  }
  if (readAhead=="&")
  { out << " ";
    return true;
  }
  if (readAhead=="+")
  { out << " ";
    return true;
  }
  if (readAhead=="%2B")
  { out << "+";
    return true;
  }
  if (readAhead=="%28")
  { out << "(";
    return true;
  }
  if (readAhead=="%29")
  { out << ")";
    return true;
  }
  if (readAhead=="%5B")
  { out << "[";
    return true;
  }
  if (readAhead=="%5D")
  { out << "]";
    return true;
  }
  if (readAhead=="%2C")
  { out << ",";
    return true;
  }
  if (readAhead=="%3D")
  { out << "=";
    return true;
  }
  if (readAhead=="%7B")
  { out << "{";
    return true;
  }
  if (readAhead=="%3B")
  { out << ";";
    return true;
  }
  if (readAhead=="%3C")
  { out << "<";
    return true;
  }
  if (readAhead=="%3E")
  { out << ">";
    return true;
  }
  if (readAhead=="%2F")
  { out << "/";
    return true;
  }
  if (readAhead=="%3A")
  { out << ":";
    return true;
  }
  if (readAhead=="%5E")
  { out << "^";
    return true;
  }
  if (readAhead=="%5C")
  { out << "\\";
    return true;
  }
  if (readAhead=="%26")
  { out << "&";
    return true;
  }
  if (readAhead=="%7D")
  { out << "}";
    return true;
  }
  if (readAhead=="%0D%0A")
  { out << "\n";
    return true;
  }
  return false;
}

//The below function was supposed to be called "sanitize", but I don't think that is true of the function the way it is written
//it increases the hygiene of the input cgi scripts so that the string can be returned to the user
//(html tags in the input must be sanitized).
bool IncreaseHygiene(const std::string& input, std::string& output)
{ std::stringstream out;
  bool result=false;
  for (unsigned int i=0; i<input.size(); i++)
  { if (input[i]=='<')
      out << "&lt;";
    if (input[i]=='>')
      out << "&gt;";
    if (input[i]!='<' && input[i]!='>')
      out << input[i];
    else
      result=true;
  }
  output=out.str();
  return result;
}

void CivilizeCGIString(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (AttemptToCivilize(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

int main(int argc, char **argv)
{ std::string inputStringNonCivilized, inputPath;
  std::string tempS;
	std::cin >> inputStringNonCivilized;
//  gettimeofday(&ComputationStartGlobal, NULL);
	if (inputStringNonCivilized=="")
    inputStringNonCivilized=getenv("QUERY_STRING");
	getPath(argv[0], inputPath);
	std::cout << "Content-Type: text/html\n\n";
  std::cout << "<html><head></head><title>Symbolic calculator updated " << __DATE__ << "</title>  <body> <script src=\"/easy/load.js\"></script>";
  std::string inputString;
  CivilizeCGIString(inputStringNonCivilized, inputString);
  int numBogusChars= 4;
  for (signed i=0; i<signed(inputString.size())-numBogusChars; i++)
  { inputString[i]=inputString[i+numBogusChars];
  }
  int newSize=signed(inputString.size())-numBogusChars;
  if (newSize<0)
    newSize=0;
  inputString.resize(newSize);
//  inputString="{{a}}:if(IsInteger(a)):=x; y";
//  inputString= "{{a}} : if (IsInteger(a)):=x; \n 5";
//  inputString="{{a}} : if (IsInteger(a)):=x;";
//  inputString="IsInteger{}0";
//  inputString="-x";
//  inputString="{{a}}{{b}}:=1;c*d";
//  inputString="c*d";
//  inputString="\\partial_{{i}} {}({{a}}{{b}}):=\\partial_i{}(a)b+ a\\partial_i{} (b);\\partial_i{}(c*d)";
  //inputString = "";
//  inputString="\\partial_{{i}} {}({{a}}^{{b}}):=\\partial_i{}(a)a^{b-1}";
//  inputString="{{a}}^{{b}}:=1";
//  inputString="f{}(x{{}}):=x+1;  f{}(f{}(x))";
//  inputString="f{}(x{{}}):=x+1;  f{}(x)";
//  inputString="f{}(x) ";
  std::string beginMath="<div class=\"math\" scale=\"50\">";
  std::string endMath ="</div>";
  std::cout << "<table><tr><td valign=\"top\">";
  std::cout << "\n<form method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/symbolicCalculator\">\n" ;
  std::cout << "<textarea rows=\"10\" cols=\"100\" name=\"in1\" id=\"in1\">";
  std::cout << inputString;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
//  std::cout << "<a href=\"/tmp/indicator.html\" target=\"_blank\"> Indicator window  </a>";
  std::cout << "\n</form>";

  CommandList theComputation;
  theComputation.Evaluate(inputString);

  std::cout << "<hr>result:<br>";
  std::cout <<  theComputation.output;
  std::cout << "</td><td valign=\"top\">";
  std::cout << theComputation.theLog;
  std::cout << "</td></tr></table>";
  std::cout << "</body></html>";

	return 0;   // To avoid Apache errors.
}
