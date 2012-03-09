#include <iostream>
#include <vector>
//#include <algorithm>
#include <sstream>
#include<assert.h>

class CommandList;
class Command;
class Expression;
class ExpressionPairs;

const int SomeRandomPrimesSize= 25;
//used for hashing various things.
const int SomeRandomPrimes[SomeRandomPrimesSize]={ 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911};

inline int IntIdentity(const int& x)
{ return x;
}

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
  void RemoveKeyOfObjectWithIndex(int theIndex)
  { element& current=this->theElts[theIndex];
    int indexHash=this->GetHash(current);
    std::vector<int>& currentHashes=this->hashedIndices[indexHash];
    int indexKey=-1;
    for (unsigned i=0; i<currentHashes.size(); i++)
      if (currentHashes[i]==theIndex)
      { indexKey=i;
        break;
      }
    if (indexKey==-1)
    { std::cout << "faulty hashed list. This is a programming error (aka bug). ";
      assert(false);
    }
    std::swap(currentHashes[indexKey], currentHashes[currentHashes.size()-1]);
    currentHashes.resize(currentHashes.size()-1);
  }
  public:
  const element& operator[](int i)const {return this->theElts[i];}
  int GetHash(const element& x)const
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
  void AdjustHashes()
  { if (this->theElts.size()>this->hashedIndices.size()*2)
      this->setHashSize(5*this->theElts.size());
  }
  void setHashSize(int N)
  { this->hashedIndices.resize(N);
    for (unsigned i=0; i<this->hashedIndices.size(); i++)
      this->hashedIndices[i].resize(0);
    for (unsigned i=0; i<this->theElts.size(); i++)
      this->hashedIndices[this->GetHash(this->theElts[i])].push_back(i);
  }
  int GetIndex(const element& x)const
  { int theHash=this->GetHash(x);
    const std::vector<int>& currentHash=this->hashedIndices[theHash];
    for (unsigned i=0; i<currentHash.size(); i++)
      if (x==this->theElts[currentHash[i]])
        return currentHash[i];
    return -1;
  }
  void PopIndexSwapWithLast(int theIndex)
  { this->RemoveKeyOfObjectWithIndex(theIndex);
    if (theIndex!=(signed)this->theElts.size()-1)
    { this->RemoveKeyOfObjectWithIndex(this->theElts.size()-1);
      this->theElts[theIndex]=this->theElts[this->theElts.size()-1];
      element& current=this->theElts[theIndex];
      std::vector<int>& currentHashes=this->hashedIndices[this->GetHash(current)];
      currentHashes.push_back(theIndex);
    }
    this->theElts.resize(this->theElts.size()-1);
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
    this->AdjustHashes();
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
  int theOperation;
  int theData;
  std::vector<Expression> children;
  std::string errorString;
  ///////////////////////////////////////
  //two objects are considered equal even when the the following data is different:
  int format;
  CommandList* theBoss;
  int commandIndex;
  enum format
  { formatDefault, formatFunctionUseUnderscore, formatTimesDenotedByStar,
    formatFunctionUseCdot,
  };
  void reset(CommandList* newBoss, int indexOfTheCommand)
  { this->theBoss=newBoss;
    this->commandIndex=indexOfTheCommand;
    this->children.clear();
    this->theOperation=-1;
    this->theData=0;
    this->errorString="";
    this->format=this->formatDefault;
  }
  void AssignChild(int childIndex)
  { Expression tempExp=this->children[childIndex];
    this->operator=(tempExp);
  }
  void SetPropertyValue(const std::string& propertyName, int PropertyValue)
  ;
  int GetPropertyValue(const std::string& propName)const
;
  int GetPropertyValue(int propertyIndex)const
;
  void MakeInT(int theValue, CommandList* newBoss, int indexOfTheCommand)
  ;
void MakeVariableNonBoundNoProperties
  (CommandList* owner, int indexOfTheCommand, int varIndex)
;
void MakeVariableNonBounD
  (CommandList* owner, int indexOfTheCommand, int varIndex)
;
  void MakeFunction
  (CommandList* owner, int indexOfTheCommand, const Expression& argument, const std::string& functionName)
;
  void MakeFunction
  (CommandList* owner, int indexOfTheCommand, const Expression& argument, int functionIndex)
;
  void MakeProduct
  (CommandList* owner, int indexOfTheCommand, const Expression& left, const Expression& right)
  ;
  std::string ElementToString(int recursionDepth=0, int maxRecursionDepth=1000, bool AddBrackets=false, bool AddCurlyBraces=false)const;
  std::string ElementToStringPolishForm(int recursionDepth=0, int maxRecursionDepth=1000);
  static int HashFunction(const Expression& input)
  { return input.HashFunctionRecursive(0, 1000);
  }
  int HashFunctionRecursive(int RecursionDepth, int MaxRecursionDepth)const
  { if (RecursionDepth>MaxRecursionDepth)
      return 0;
    int result=this->theOperation*SomeRandomPrimes[0]+this->theData*SomeRandomPrimes[1];
    int numCycles=std::min((signed)this->children.size(), SomeRandomPrimesSize);
    for (int i=0; i<numCycles; i++)
      result+=this->children[i].HashFunctionRecursive(RecursionDepth+1, MaxRecursionDepth)*SomeRandomPrimes[i];
    return result;
  }
  std::string GetOperation()const;
  Expression()
  { this->reset(0,-1);
  }
  Expression(const Expression& other)
  { this->operator=(other);
  }
  bool AreEqualExcludingChildren(const Expression& other) const
  { return
    this->theBoss==other.theBoss &&
    this->theData==other.theData &&
    this->theOperation==other.theOperation &&
    this->children.size()==other.children.size() &&
    this->errorString==other.errorString
    ;
  }
  bool NeedBracketsForMultiplication()const;
  bool NeedBracketsForAddition()const;
  bool NeedBracketsForFunctionName()const;
  bool NeedBracketsForThePower()const;
  bool operator==(const Expression& other)const;
  void CopyValueFromNoChildrenCopy(const Expression& other)
  { this->theBoss=other.theBoss;
    this->commandIndex=other.commandIndex;
    this->theData=other.theData;
    this->theOperation=other.theOperation;
    this->errorString=other.errorString;
    this->children.resize(other.children.size());
    this->format=other.format;
  }
  inline void operator=(const Expression& other)
  { //warning: we are assuming that invoking function OperatorAssignRecursively
    // recursively RecursionDepthThatWouldNonPopTheStack times
    //will not pop the stack. If this is not the case, the program might crash unexpectedly.
    const int RecursionDepthThatWouldNonPopTheStack=100000;
    this->OperatorAssignRecursively(other, 0, RecursionDepthThatWouldNonPopTheStack);
  }
  void OperatorAssignRecursively(const Expression& other, int RecursionDepth, int MaxRecursionDepth)
  { if (this==&other)
      return;
    this->CopyValueFromNoChildrenCopy(other);
    if (RecursionDepth>MaxRecursionDepth)
    { std::stringstream out;
      out << "Error while copying expression: expression depth exceeded maximum allowed depth of " << MaxRecursionDepth;
      this->errorString=out.str();
      return;
    }
    this->children.resize(other.children.size());
    for (unsigned i=0; i<other.children.size(); i++)
      this->children[i].OperatorAssignRecursively(other.children[i], RecursionDepth+1, MaxRecursionDepth);
  }
};

class ExpressionPairs
{
public:
  std::string ElementToString();
  hashedVector<int, IntIdentity> BoundVariableIndices;
  hashedVector<Expression, Expression::HashFunction> variableImages;
  void reset(){this->BoundVariableIndices.reset(); this->variableImages.reset();}
};

class SyntacticElement
{
  public:
  int controlIndex;
  int IndexFirstChar;
  int IndexLastCharPlusOne;
  int format;
  std::string ErrorString;
  Expression theData;
  void operator=(const SyntacticElement& other)
  { this->controlIndex=other.controlIndex;
    this->theData=other.theData;
    this->ErrorString=other.ErrorString;
    this->IndexFirstChar=other.IndexFirstChar;
    this->IndexLastCharPlusOne=other.IndexLastCharPlusOne;
  }
  std::string ElementToString(CommandList& theBoss);
  SyntacticElement()
  { this->controlIndex=0;//controlIndex=0 *MUST* point to the empty control sequence.
    this->ErrorString="";
    this->IndexFirstChar=-1;
    this->IndexLastCharPlusOne=-1;
    this->format=Expression::formatDefault;
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
  std::vector<SyntacticElement> syntacticStack;
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
    this->syntacticStack=other.syntacticStack;
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
    assert(((signed) this->syntacticStack.size())-decrease>0);
    this->syntacticStack[this->syntacticStack.size()-decrease-1].IndexLastCharPlusOne=
      this->syntacticStack[this->syntacticStack.size()-1].IndexLastCharPlusOne;
    this->syntacticStack.resize(this->syntacticStack.size()-decrease);
    return true;
  }
  void reset(CommandList* owner=0, int commandIndexInBoss=-1)
  { this->numEmptyTokensStart=9;
    this->syntacticStack.resize(0);
    this->syntacticSoup.resize(0);
    this->BoundVariables.reset();
    this->finalValue.reset(owner, -1);
    this->theBoss=owner;
    this->IndexInBoss=commandIndexInBoss;
    this->ErrorString="";
  }
  std::string ElementToStringSyntacticStack();
  std::string ElementToString(bool usePolishForm=false)
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
    for (unsigned i=this->numEmptyTokensStart; i<this->syntacticStack.size(); i++)
    { out << this->syntacticStack[i].ElementToString(*this->theBoss);
      if (i!=this->syntacticStack.size()-1)
        out << ", ";
    }
    out << "<hr>\nSyntactic soup:";
    for (unsigned i=0; i<this->syntacticSoup.size(); i++)
    { out << this->syntacticSoup[i].ElementToString(*this->theBoss);
      if (i!=this->syntacticSoup.size()-1)
        out << ", ";
    }
    out << "<br>\nExpression stack(excluding empty tokens in the start): ";
    out << this->ElementToStringSyntacticStack();
    out << "<br>\n Current value: " << this->finalValue.ElementToString(0,5, false);
    if (usePolishForm)
      out << "<br>=(in polish form):" << this->finalValue.ElementToStringPolishForm(0, 20);
    return out.str();
  }
  bool isStrongLeftSeparator(const std::string& input)
  { return
    input=="{" || input=="(" || input=="[" ||
    input=="," || input==":" || input==";" ||
    input==" "
    ;
  }
  bool isStrongRightSeparator(const std::string& input)
  { return
    input=="}" || input==")" || input=="]" ||
    input=="," || input==":" || input==";" ||
    input==" "
    ;
  }
  bool LookAheadAllowsThePower(const std::string& lookAhead)
  { return
      lookAhead!="{}"
      ;
  }
  bool LookAheadAllowsPlus(const std::string& lookAhead)
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
  { SyntacticElement& middle = this->syntacticStack[this->syntacticStack.size()-3];
    SyntacticElement& left = this->syntacticStack[this->syntacticStack.size()-6];
    SyntacticElement& right = this->syntacticStack[this->syntacticStack.size()-1];
    Expression newExpr;
    newExpr.reset(this->theBoss, this->IndexInBoss);
    newExpr.theOperation=theOp;
    newExpr.children.push_back(left.theData);
    newExpr.children.push_back(middle.theData);
    newExpr.children.push_back(right.theData);
    left.theData=newExpr;
    this->DecreaseStackSetCharacterRanges(5);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
    return true;
  }
  bool ReplaceEEByEusingO(int theOp)
  { SyntacticElement& left = this->syntacticStack[this->syntacticStack.size()-2];
    SyntacticElement& right = this->syntacticStack[this->syntacticStack.size()-1];
    Expression newExpr;
    newExpr.reset(this->theBoss, this->IndexInBoss);
    newExpr.theOperation=theOp;
//    newExpr.commandIndex=
    newExpr.children.push_back(left.theData);
    newExpr.children.push_back(right.theData);
    left.theData=newExpr;
    this->DecreaseStackSetCharacterRanges(1);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
    return true;
  }
  bool ReplaceOEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceOXEXEXEXByE(int formatOptions=Expression::formatDefault);
  bool ReplaceEOEByE(int formatOptions=Expression::formatDefault)
  { SyntacticElement& middle=this->syntacticStack[this->syntacticStack.size()-2];
    SyntacticElement& left = this->syntacticStack[this->syntacticStack.size()-3];
    SyntacticElement& right = this->syntacticStack[this->syntacticStack.size()-1];
    Expression newExpr;
    newExpr.reset(this->theBoss, this->IndexInBoss);
    newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
    newExpr.children.push_back(left.theData);
    newExpr.children.push_back(right.theData);
    left.theData=newExpr;
    newExpr.format=formatOptions;
    this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
    return true;
  }
  bool ReplaceXXByCon(int theCon, int theFormat=Expression::formatDefault)
  { this->syntacticStack[this->syntacticStack.size()-2].controlIndex=theCon;
    this->syntacticStack[this->syntacticStack.size()-2].format=theFormat;
    this->DecreaseStackSetCharacterRanges(1);
    return true;
  }
  bool ReplaceXByCon(int theCon, int theFormat=Expression::formatDefault)
  { this->syntacticStack[this->syntacticStack.size()-1].controlIndex=theCon;
    this->syntacticStack[this->syntacticStack.size()-1].format=theFormat;
//    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXXXByCon(int theCon)
  { this->syntacticStack[this->syntacticStack.size()-3].controlIndex=theCon;
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXXYByY()
  { this->syntacticStack[this->syntacticStack.size()-3]=this->syntacticStack[this->syntacticStack.size()-1];
    this->syntacticStack.resize(this->syntacticStack.size()-2);
    return true;
  }
  bool ReplaceXYXByY()
  { this->syntacticStack[this->syntacticStack.size()-3]=this->syntacticStack[this->syntacticStack.size()-2];
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceXYByY()
  { this->syntacticStack[this->syntacticStack.size()-2]=this->syntacticStack[this->syntacticStack.size()-1];
    this->syntacticStack.resize(this->syntacticStack.size()-1);
    return true;
  }
  bool ReplaceXEXByE()
  { this->syntacticStack[this->syntacticStack.size()-3]=this->syntacticStack[this->syntacticStack.size()-2];
    this->DecreaseStackSetCharacterRanges(2);
    return true;
  }
  bool ReplaceObyE();
  bool ReplaceVXbyEX();
  bool ReplaceVbyE();
  bool ReplaceIntIntBy10IntPlusInt()
  { SyntacticElement& left=this->syntacticStack[this->syntacticStack.size()-2];
    SyntacticElement& right=this->syntacticStack[this->syntacticStack.size()-1];
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
    this->syntacticStack.resize(this->numEmptyTokensStart, emptyElement);
  }
  bool SetError(const std::string& theError)
  { this->ErrorString=theError;
    return true;
  }
  Command()
  { this->reset();
  }
};

class VariableNonBound
{
  public:
  std::string theName;
  bool (*theHandler)(CommandList& theCommands, int commandIndex, Expression& theExpression);
  inline static int hashVariableNonBound(const VariableNonBound& input)
  { return hashString(input.theName);
  }
  bool operator==(const VariableNonBound& other)const
  { return this->theName==other.theName;
  }
};

class CommandList
{
public:
//control sequences parametrize the syntactical elements
  hashedVector<std::string, hashString> controlSequences;
//operations parametrize the expression elements
  hashedVector<std::string, hashString> operations;
  std::vector <bool ((*)(CommandList& theCommands, int commandIndex, Expression& theExpression))> theStandardOpEvalFunctions;
//used to parametrize the input data for the special operation "VariableNonBound"
  hashedVector<VariableNonBound, VariableNonBound::hashVariableNonBound> theNonBoundVars;
  std::vector<int> targetProperties;
  hashedVector<std::string, hashString> theDictionary;
  hashedVector<std::string, hashString> thePropertyNameS;
  std::vector<Expression> buffer1, buffer2;
  int MaxRecursionDepthDefault;
  int MaxAlgTransformationsPerExpression;
  int TotalNumPatternMatchedPerformed;
  std::vector<Command> theCommands;
//  std::vector<std::stringstream> theLogs;
  hashedVector<Expression, Expression::HashFunction> cachedExpressions;
  std::vector<Expression> imagesCashedExpressions;

  std::vector <std::string> syntaxErrors;
  std::vector <std::string> evaluationErrors;
  std::string input;
  std::string output;
  std::string theLog;
  std::string ElementToString();
  double StartTimeInSeconds;
  double (*GetElapsedTimeNonSafe)();
  double GetElapsedTime()
  { if (this->GetElapsedTimeNonSafe==0)
      return -1;
    return this->GetElapsedTimeNonSafe();
  }
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
  int conEqualEqual()
  { return this->controlSequences.GetIndexIMustContainTheObject("==");
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
  int opEqualEqual()
  { return this->operations.GetIndexIMustContainTheObject("==");
  }
  void AddTargetProperty(const std::string& propertyName, bool theHandler(CommandList& theCommands, int commandIndex, Expression& theExpression))
  { this->theDictionary.AddNoRepetition(propertyName);
    VariableNonBound tempV;
    tempV.theName=propertyName;
    tempV.theHandler=theHandler;
    int theIndex=this->theNonBoundVars.GetIndex(tempV);
    if (theIndex==-1)
    { this->theNonBoundVars.AddOnTop(tempV);
      theIndex=this->theNonBoundVars.size()-1;
    }
    this->targetProperties.push_back(theIndex);
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
  bool AppendOpandsReturnTrueIfOrderNonCanonical
    (Expression& theExpression, std::vector<Expression>& output, int theOp, int RecursionDepth, int MaxRecursionDepth)
;
  bool AppendMultiplicandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, std::vector<Expression>& output, int RecursionDepth, int MaxRecursionDepth)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opTimes(), RecursionDepth, MaxRecursionDepth);
  }
  bool AppendSummandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, std::vector<Expression>& output, int RecursionDepth, int MaxRecursionDepth)
  { return this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression, output, this->opPlus(), RecursionDepth, MaxRecursionDepth);
  }
  void SpecializeBoundVars
(Expression& toBeSubbed, int targetCommandIndex, ExpressionPairs& matchedPairs, int RecursionDepth, int MaxRecursionDepth)
  ;
  bool ExpressionHasBoundVars(Expression& theExpression, int RecursionDepth, int MaxRecursionDepth)
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
        if (this->ExpressionHasBoundVars(theExpression.children[i], RecursionDepth+1, MaxRecursionDepth+1))
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
   int RecursionDepth, int maxRecursionDepth, std::stringstream* theLog=0, bool logAttempts=false)
  ;
/*  Expression GetProperty(const std::string& propertyName, int theData=0)
  { ExpressionProperty result;
    result.value=theData;
    result.nameIndex=this->thePropertyNames.GetIndex(propertyName);
    if (result.nameIndex==-1)
    { this->thePropertyNames.AddOnTop(propertyName);
      result.nameIndex=this->thePropertyNames.size()-1;
    }
    return result;
  }*/
  bool isADigit(const std::string& input, int& whichDigit)
  { if (input.size()!=1)
      return false;
    whichDigit=input[0]-'0';
    return whichDigit<10 && whichDigit>=0;
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
//  bool OrderMultiplicationTreeProperly(int commandIndex, Expression& theExpression);
  bool CollectSummands(int commandIndex, Expression& theExpression);
  bool ExpressionMatchesPattern
  (const Expression& thePattern, const Expression& input, ExpressionPairs& matchedExpressions,
   int RecursionDepth=0, int MaxRecursionDepth=500, std::stringstream* theLog=0)
  ;
  bool ExpressionsAreEqual
  (const Expression& left, const Expression& right, int RecursionDepth=0, int MaxRecursionDepth=500)
  ;

  static bool EvaluateStandardPlus(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateStandardTimes(CommandList& theCommands, int commandIndex, Expression& theExpression)
  ;
  static bool EvaluateStandardMinus(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateStandardFunction(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateStandardEqualEqual(CommandList& theCommands, int commandIndex, Expression& theExpression);

  static bool EvaluateDoCollect(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateDoAssociate(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateDoDistribute(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateDoLeftDistributeBracketIsOnTheLeft(CommandList& theCommands, int commandIndex, Expression& theExpression);
  static bool EvaluateDoRightDistributeBracketIsOnTheRight(CommandList& theCommands, int commandIndex, Expression& theExpression);

  static bool EvaluateFunctionIsInteger(CommandList& theCommands, int commandIndex, Expression& theExpression);

  static bool EvaluateIf(CommandList& theCommands, int commandIndex, Expression& theExpression);
  void AddEmptyHeadedCommand();
  CommandList()
  { this->GetElapsedTimeNonSafe=0;
    init();
  }
  void AddOperationNoFail
  (const std::string& theOp,
   bool (theOpHandler(CommandList& theCommands, int commandIndex, Expression& theExpression))
   )
  { if (this->operations.Contains(theOp))
    { assert(false);
      return;
    }
    this->operations.AddOnTop(theOp);
    this->theStandardOpEvalFunctions.push_back(theOpHandler);
  }
  void AddNonBoundVarMustBeNew
  (const std::string& theName,
   bool (theOpHandler(CommandList& theCommands, int commandIndex, Expression& theExpression))
   )
  { int theIndex=this->AddNonBoundVarReturnVarIndex(theName, theOpHandler);
    if (theIndex!=this->theNonBoundVars.size()-1)
      assert(false);
  }
  int GetIndexNonBoundVar(const std::string& theName)
  { VariableNonBound tempVar;
    tempVar.theName=theName;
    tempVar.theHandler=0;
    return this->theNonBoundVars.GetIndex(tempVar);
  }
  int AddNonBoundVarReturnVarIndex
  (const std::string& theName,
   bool (theOpHandler(CommandList& theCommands, int commandIndex, Expression& theExpression))
   )
  { VariableNonBound tempVar;
    tempVar.theName=theName;
    int theIndex=this->theNonBoundVars.GetIndex(tempVar);
    if (theIndex!=-1)
      return theIndex;
    tempVar.theHandler=theOpHandler;
    this->theNonBoundVars.AddOnTop(tempVar);
    this->theDictionary.AddNoRepetition(theName);
    return this->theNonBoundVars.size()-1 ;
  }
  void init()
  { this->controlSequences.reset();
    this->operations.reset();
    this->theNonBoundVars.reset();
    this->theDictionary.reset();
    this->cachedExpressions.reset();
    this->imagesCashedExpressions.resize(0);
    this->theStandardOpEvalFunctions.clear();
    this->syntaxErrors.clear();
    this->evaluationErrors.resize(0);
    this->thePropertyNameS.reset();
    this->targetProperties.resize(0);
    this->MaxRecursionDepthDefault=1000;
    this->AddOperationNoFail("+",this->EvaluateStandardPlus);
    this->AddOperationNoFail("-", this->EvaluateStandardMinus);
    this->AddOperationNoFail("/", 0);
    this->AddOperationNoFail("*", this->EvaluateStandardTimes);
    this->AddOperationNoFail(":=", 0);
    this->AddOperationNoFail("if:=", 0);
    this->AddOperationNoFail("^", 0);
    this->AddOperationNoFail("==", this->EvaluateStandardEqualEqual);
    //the following two operations are chosen on purpose so that they correspond to LaTeX-undetectable
    //expressions
    //the following is the binding variable operation
    this->AddOperationNoFail("{{}}", 0);
    //the following is the operation for using a variable as a function
    this->AddOperationNoFail("{}", this->EvaluateStandardFunction);
    this->AddOperationNoFail("if", this->EvaluateIf);
    this->AddOperationNoFail("Integer", 0);
    this->AddOperationNoFail("VariableNonBound", 0);
    this->AddOperationNoFail("VariableBound", 0);
    this->AddOperationNoFail("Error", 0);

    this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
    this->controlSequences.AddOnTop("Variable");
    this->controlSequences.AddOnTop(this->operations);//all operations are also control sequences
    this->controlSequences.AddOnTop("Expression");
    this->controlSequences.AddOnTop(",");
    this->controlSequences.AddOnTop("\\cdot");
    this->controlSequences.AddOnTop("_");
    this->controlSequences.AddOnTop("(");
    this->controlSequences.AddOnTop(")");
    this->controlSequences.AddOnTop("[");
    this->controlSequences.AddOnTop("]");
    this->controlSequences.AddOnTop("{");
    this->controlSequences.AddOnTop("}");
    this->controlSequences.AddOnTop(":");
    this->controlSequences.AddOnTop("=");
    this->controlSequences.AddOnTop(";");
//    this->thePropertyNames.AddOnTop("IsCommutative");
    this->TotalNumPatternMatchedPerformed=0;
    this->initPredefinedVars();
    this->initTargetProperties();
  }
  void initPredefinedVars()
  { this->AddNonBoundVarMustBeNew("IsInteger", &this->EvaluateFunctionIsInteger);
  }
  void initTargetProperties()
  {// this->AddTargetProperty("IsDistributed", & this->EvaluateDoDistribute);
  }
  void ExtractExpressions()
  { std::string lookAheadToken;
    std::stringstream errorLog;
    for (unsigned j=0; j<this->theCommands.size(); j++)
    { this->theCommands[j].resetStack();
      for (unsigned i=0; i<this->theCommands[j].syntacticSoup.size(); i++)
      { if (i+1<this->theCommands[j].syntacticSoup.size())
          lookAheadToken=this->controlSequences[this->theCommands[j].syntacticSoup[i+1].controlIndex];
        else
          lookAheadToken=";";
        this->theCommands[j].syntacticStack.push_back(this->theCommands[j].syntacticSoup[i]);
        while(this->theCommands[j].ApplyOneRule(lookAheadToken))
        {}
      }
      if (this->theCommands[j].ErrorString=="" && (signed)(this->theCommands[j].syntacticStack.size())==this->theCommands[j].numEmptyTokensStart+1)
        this->theCommands[j].finalValue=this->theCommands[j].syntacticStack[this->theCommands[j].numEmptyTokensStart].theData;
      else if (this->theCommands[j].ErrorString!="")
        errorLog << "Syntax error at command " << j+1 << ":" << theCommands[j].ErrorString << "<br>";
      else if ((signed) this->theCommands[j].syntacticStack.size()!=this->theCommands[j].numEmptyTokensStart)
      { errorLog << "Syntax error at command " << j+1 << ": your command does not simplify to a single expression. <br>";
        errorLog << "Instead it simplifies to:<br> " << this->theCommands[j].ElementToStringSyntacticStack();
      }
    }
    std::string error = errorLog.str();
    if (error!="")
      this->syntaxErrors.push_back(error);
  }
  void EvaluateCommands();
  bool EvaluateExpressionReturnFalseIfExpressionIsBound
(int commandIndex, Expression& theExpression, int RecursionDepth, int maxRecursionDepth,
 ExpressionPairs& bufferPairs,
 std::stringstream* theLog=0)
 ;
  void Evaluate(const std::string& theInput)
  { this->StartTimeInSeconds=this->GetElapsedTime();
    std::vector<Command> startingExpressions;
    this->input=theInput;
    this->ParseFillDictionary(this->input);
    this->ExtractExpressions();
    this->EvaluateCommands();
  }
  bool isLeftSeparator(char c);
  bool isRightSeparator(char c);
  void ParseFillDictionary(const std::string& input);
};

void Expression::MakeProduct
  (CommandList* owner, int commandIndex, const Expression& left, const Expression& right)
{ this->reset(owner, commandIndex);
  this->theOperation=owner->opTimes();
  this->children.resize(2);
  this->children[0]=left;
  this->children[1]=right;
}

void Expression::SetPropertyValue(const std::string& propertyName, int PropertyValue)
{ if (this->theBoss==0)
  { assert(false);
    return;
  }
  Expression tempExp;
  tempExp.MakeFunction(this->theBoss, this->commandIndex,*this, propertyName);
  int theIndex= this->theBoss->cachedExpressions.GetIndex(tempExp);
  if (theIndex==-1)
  { this->theBoss->cachedExpressions.AddOnTop(tempExp);
    this->theBoss->imagesCashedExpressions.resize(this->theBoss->cachedExpressions.size());
    theIndex=(signed)this->theBoss->cachedExpressions.size()-1;
//    if (theIndex==4)
//      std::cout <<"!";
  }
  this->theBoss->imagesCashedExpressions[theIndex].MakeInT(PropertyValue, this->theBoss, this->commandIndex);
}

int Expression::GetPropertyValue(const std::string& propertyName)const
{ if (this->theBoss==0)
  { assert(false);
    return 0;
  }
  return this->GetPropertyValue (this->theBoss->GetIndexNonBoundVar(propertyName));
}

int Expression::GetPropertyValue(int propertyIndex)const
{ if (this->theBoss==0)
  { assert(false);
    return 0;
  }
  Expression tempExp;
  tempExp.MakeFunction(this->theBoss, this->commandIndex,*this, propertyIndex);
  int theIndex= this->theBoss->cachedExpressions.GetIndex(tempExp);
  if (theIndex==-1)
    return -1;
  return this->theBoss->imagesCashedExpressions[theIndex].theData;
}

bool Command::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=this->syntacticStack[this->syntacticStack.size()-8];
  SyntacticElement& leftE = this->syntacticStack[this->syntacticStack.size()-6];
  SyntacticElement& middleE= this->syntacticStack[this->syntacticStack.size()-4];
  SyntacticElement& rightE = this->syntacticStack[this->syntacticStack.size()-2];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.format=formatOptions;
  newExpr.children.push_back(leftE.theData);
  newExpr.children.push_back(middleE.theData);
  newExpr.children.push_back(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(7);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOXXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=this->syntacticStack[this->syntacticStack.size()-9];
  SyntacticElement& leftE = this->syntacticStack[this->syntacticStack.size()-6];
  SyntacticElement& middleE= this->syntacticStack[this->syntacticStack.size()-4];
  SyntacticElement& rightE = this->syntacticStack[this->syntacticStack.size()-2];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.push_back(leftE.theData);
  newExpr.children.push_back(middleE.theData);
  newExpr.children.push_back(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(8);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOXEByE(int formatOptions)
{ SyntacticElement& left=this->syntacticStack[this->syntacticStack.size()-3];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size()-1];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(left.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.push_back(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOEByE(int formatOptions)
{ SyntacticElement& middle=this->syntacticStack[this->syntacticStack.size()-2];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size()-1];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.push_back(right.theData);
  middle.theData=newExpr;
  middle.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(1);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
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
  for (int i=0; i<this->BoundVariableIndices.size(); i++)
    out << this->BoundVariableIndices[i] << "->" << this->variableImages[i].ElementToString() << "<br>";
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
        { this->theDictionary.AddNoRepetition(current);
          currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
          currentElement.theData.reset(this, this->theCommands.size()-1);
          currentElement.theData.theOperation=this->operations.GetIndex("Variable");
          currentElement.theData.theData=this->theDictionary.GetIndex(current);
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
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size()-1];
  theElt.theData.theOperation=this->theBoss->operations.GetIndex(this->theBoss->controlSequences[theElt.controlIndex]);
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::ReplaceVbyE()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size()-1];
  const std::string& theVarString=this->theBoss->theDictionary[theElt.theData.theData];
  int indexBoundVar=this->BoundVariables.GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->theBoss->opVariableBound();
    theElt.theData.theData=indexBoundVar;
  } else
  { theElt.theData.theOperation=this->theBoss->opVariableNonBound();
    theElt.theData.theData=this->theBoss->AddNonBoundVarReturnVarIndex(theVarString, 0);
    //note:     theElt.theData.theOperation.theData     should be initialized with the index of the non-bound variable!
  }
//  std::cout << "now i'm here!";
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::ReplaceVXbyEX()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size()-2];
  const std::string& theVarString=this->theBoss->theDictionary[theElt.theData.theData];
  int indexBoundVar=this->BoundVariables.GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->theBoss->opVariableBound();
    theElt.theData.theData=indexBoundVar;
  } else
  { theElt.theData.theOperation=this->theBoss->opVariableNonBound();
    theElt.theData.theData=this->theBoss->AddNonBoundVarReturnVarIndex(theVarString, 0);
  }
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::RegisterBoundVariable()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size()-3];
  const std::string& theVarString=this->theBoss->theDictionary[theElt.theData.theData];
  if (!this->BoundVariables.Contains(theVarString))
    this->BoundVariables.AddOnTop(theVarString);
  this->DecreaseStackSetCharacterRanges(2);
  this->ReplaceXXYByY();
  return true;
}

bool Command::ApplyOneRule(const std::string& lookAhead)
{ //return false;
  const SyntacticElement& lastE=this->syntacticStack[this->syntacticStack.size()-1];
  const std::string&    lastS=this->theBoss->controlSequences[lastE.controlIndex];
  if (lastS==" " && signed (this->syntacticStack.size())>this->numEmptyTokensStart)
  { this->syntacticStack.resize(this->syntacticStack.size()-1);
    return false;
  }
  const SyntacticElement& secondToLastE=this->syntacticStack[this->syntacticStack.size()-2];
  const std::string& secondToLastS=this->theBoss->controlSequences[secondToLastE.controlIndex];
  const SyntacticElement& thirdToLastE=this->syntacticStack[this->syntacticStack.size()-3];
  const std::string& thirdToLastS=this->theBoss->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE=this->syntacticStack[this->syntacticStack.size()-4];
  const std::string& fourthToLastS=this->theBoss->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE=this->syntacticStack[this->syntacticStack.size()-5];
  const std::string& fifthToLastS=this->theBoss->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE=this->syntacticStack[this->syntacticStack.size()-6];
  const std::string& sixthToLastS=this->theBoss->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE=this->syntacticStack[this->syntacticStack.size()-7];
  const std::string& seventhToLastS=this->theBoss->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE=this->syntacticStack[this->syntacticStack.size()-8];
  const std::string& eighthToLastS=this->theBoss->controlSequences[eighthToLastE.controlIndex];
//  const SyntacticElement& ninthToLastE=this->syntacticStack[this->syntacticStack.size()-9];
//  const std::string& ninthToLastS=this->theBoss->controlSequences[ninthToLastE.controlIndex];

//  std::cout << "<hr>" << this->ElementToString();
//this->theBoss->ElementToString();
  if (secondToLastS==":" && lastS=="=")
    return this->ReplaceXXByCon(this->theBoss->conDefine());
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->theBoss->conDeclareFunction(), Expression::formatDefault);
  if (lastS=="_")
    return this->ReplaceXByCon(this->theBoss->conDeclareFunction(), Expression::formatFunctionUseUnderscore);
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->theBoss->conDeclareFunction(), Expression::formatFunctionUseCdot);
//  if ( thirdToLastS=="{" && secondToLastS=="{}" && lastS=="}")
//    return this->ReplaceXXXByCon(this->theBoss->conBindVariable());
  if (fifthToLastS=="{" && fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS=="}")
    return this->RegisterBoundVariable();
  if (lastS=="Variable" && (lookAhead!="}" || secondToLastS!="{"|| thirdToLastS!="{"))
    return this->ReplaceVbyE();
  if (lastS=="=" && secondToLastS=="=")
    return this->ReplaceXXByCon(this->theBoss->conEqualEqual());
  if (lastS==";")
    return this->DecreaseStackSetCharacterRanges(1);
  if (lastS=="Integer" && secondToLastS=="Integer")
    return this->ReplaceIntIntBy10IntPlusInt();
  if (lastS=="Integer" && lookAhead!="Integer")
  { this->syntacticStack[this->syntacticStack.size()-1].theData.MakeInT
    (lastE.theData.theData, lastE.theData.theBoss, lastE.theData.commandIndex);
    return this->ReplaceObyE();
  }
  if (thirdToLastS=="Expression" && secondToLastS=="{}" && lastS=="Expression")
    return this->ReplaceEOEByE(secondToLastE.theData.format);
  if (eighthToLastS=="if" && seventhToLastS=="(" && sixthToLastS=="Expression"
      && fifthToLastS=="," && fourthToLastS=="Expression" && thirdToLastS==","
      && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceOXEXEXEXByE();
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
  if (lastS=="Expression" && secondToLastS=="/" && thirdToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEEByEusingO(this->theBoss->opTimes());
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE();
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}" && lookAhead!="}")
    return this->ReplaceXEXByE();
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (this->isStrongRightSeparator(lookAhead) && lastS=="Expression" && secondToLastS=="==" && thirdToLastS=="Expression")
    return this->ReplaceEOEByE();
  if (fourthToLastS==" " && lookAhead==";" && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression")
    return this->ReplaceEOEByE();
  if (seventhToLastS==" " && lookAhead==";" && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression"
      && fourthToLastS=="if" && fifthToLastS==":" && sixthToLastS=="Expression")
    return this->ReplaceEXXEXEByEusingO(this->theBoss->opDefineConditional());
  return false;
}

bool CommandList::EvaluateFunctionIsInteger
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ bool IsInteger=false;
  if (theExpression.children.size()==2)
    if (theExpression.children[1].theOperation==theExpression.theBoss->opInteger())
      IsInteger=true;
  theExpression.children.resize(0);
  theExpression.theOperation=theExpression.theBoss->opInteger();
  if (IsInteger)
    theExpression.theData=1;
  else
    theExpression.theData=0;
  return true;
}

bool CommandList::AppendOpandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, std::vector<Expression>& output, int theOp, int RecursionDepth, int MaxRecursionDepth)
{ if (RecursionDepth>MaxRecursionDepthDefault)
    return false;
  bool result=false;
  if (theExpression.theOperation!=theOp)
    output.push_back(theExpression);
  else
    for (unsigned i=0; i<theExpression.children.size(); i++)
    { if (this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression.children[i], output, theOp, RecursionDepth+1, MaxRecursionDepth))
        result=true;
      if (i<theExpression.children.size()-1 &&
          theExpression.children[i].theOperation==theOp &&
          theExpression.children[i].children.size()>1)
        result=true;
    }
  return result;
}

bool CommandList::CollectSummands(int commandIndex, Expression& theExpression)
{ std::vector<Expression>& summands= this->buffer1;
  summands.resize(0);
  bool needSimplification=this->AppendSummandsReturnTrueIfOrderNonCanonical
  (theExpression, summands, 0, this->MaxRecursionDepthDefault);
  hashedVector<Expression, Expression::HashFunction> summandsNoCoeff;
  output.reserve(summands.size());
  std::vector<int> theCoeffs;
  int constTerm=0;
  bool foundConstTerm=false;
//  std::cout << "<b>" << theExpression.ElementToString() << "</b>";
//  if (theExpression.ElementToString()=="(4)*(a) b+(a) b")
//    std::cout << "problem!";
  for (unsigned i=0; i<summands.size(); i++)
  { Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
//    this->OrderMultiplicationTreeProperly(commandIndex, *currentSummandNoCoeff);
    int theCoeff=1;
    if (currentSummandNoCoeff->theOperation==this->opTimes())
    { if(currentSummandNoCoeff->children[0].theOperation==this->opInteger())
      { theCoeff=currentSummandNoCoeff->children[0].theData;
        currentSummandNoCoeff=& currentSummandNoCoeff->children[1];
        if (theCoeff==0)
          needSimplification=true;
      }
    } else if (currentSummandNoCoeff->theOperation==this->opInteger())
    { constTerm+=currentSummandNoCoeff->theData;
      if (foundConstTerm || currentSummandNoCoeff->theData==0)
          needSimplification=true;
        foundConstTerm=true;
        continue;
    }
    int currentIndex= summandsNoCoeff.GetIndex(*currentSummandNoCoeff);
//    std::cout << "<hr>" << currentSummandNoCoeff->ElementToStringPolishForm(0,1000);
//    std::cout << "<hr>" << currentSummandNoCoeff->ElementToString();
    if (currentIndex==-1)
    { summandsNoCoeff.AddOnTop(*currentSummandNoCoeff);
      theCoeffs.push_back(0);
      currentIndex=summandsNoCoeff.size()-1;
    } else
      needSimplification=true;
    theCoeffs[currentIndex]+=theCoeff;
  }
/*
  std::cout << "<hr>summands: ";
  for (unsigned i=0; i< summands.size(); i++)
    std::cout << summands[i].ElementToString() << ", ";
  std::cout << " const term: " << constTerm;
    std::cout << "<br>coeff->summand_no_coeff: ";
  for (int i=0; i< summandsNoCoeff.size(); i++)
    std::cout << theCoeffs[i] << "->" << summandsNoCoeff[i].ElementToString() << ", ";
  std::cout << " const term: " << constTerm;
*/
  if (!needSimplification)
    return false;
  for (int i=0; i<summandsNoCoeff.size(); i++)
    if (theCoeffs[i]==0)
    { theCoeffs[i]=theCoeffs[theCoeffs.size()-1];
      summandsNoCoeff.PopIndexSwapWithLast(i);
      i--;
    }
  std::vector<Expression> summandsWithCoeff;
  summandsWithCoeff.reserve(summandsNoCoeff.size());
  for (int i=0; i<summandsNoCoeff.size(); i++)
  { summandsWithCoeff.resize(summandsWithCoeff.size()+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size()-1];
    current.reset(this, commandIndex);
    current.theOperation=this->opTimes();
    current.children.resize(2);
    current.children[0].MakeInT(theCoeffs[i],this, commandIndex);
    current.children[1]=summandsNoCoeff[i];
  }
  if (constTerm!=0 || summandsNoCoeff.size()==0)
  { summandsWithCoeff.resize(summandsWithCoeff.size()+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size()-1];
    current.MakeInT(constTerm, this, commandIndex);
  }
  if (summandsWithCoeff.size()==1)
  { theExpression=summandsWithCoeff[0];
    return true;
  }
  Expression* currentExp;
  currentExp=&theExpression;
  for (int i=0; i<(signed) summandsWithCoeff.size()-1; i++)
  { currentExp->reset(this, commandIndex);
    currentExp->theOperation=this->opPlus();
    currentExp->children.resize(2);
    currentExp->children[0]=summandsWithCoeff[i];
    currentExp=& currentExp->children[1];
  }
  *currentExp=summandsWithCoeff[summandsWithCoeff.size()-1];
  return true;
}

bool CommandList::EvaluateStandardTimes
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theCommands.EvaluateDoDistribute(theCommands, commandIndex, theExpression))
    return true;
  if (theCommands.EvaluateDoAssociate(theCommands, commandIndex, theExpression))
    return true;
  return false;
}

bool CommandList::EvaluateDoAssociate
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ std::vector<Expression>& multiplicands=theCommands.buffer1;
  std::vector<Expression>& actualMultiplicands=theCommands.buffer2;
  multiplicands.resize(0);
  bool needsModification=theCommands.AppendMultiplicandsReturnTrueIfOrderNonCanonical
  (theExpression, multiplicands, 0, theCommands.MaxRecursionDepthDefault);
  actualMultiplicands.reserve(multiplicands.size());
  actualMultiplicands.resize(0);
  int theCoeff=1;
  for (unsigned i=0; i<multiplicands.size(); i++)
    if (multiplicands[i].theOperation==theCommands.opInteger())
    { theCoeff*=multiplicands[i].theData;
      if (i>0)
        needsModification=true;
    } else
      actualMultiplicands.push_back(multiplicands[i]);
  if (theCoeff==0 && multiplicands.size()>1)
    needsModification=true;
  if (!needsModification)
    return false;
  if (theCoeff==0)
    actualMultiplicands.resize(0);
  if (actualMultiplicands.size()==0)
  { theExpression.MakeInT(theCoeff, &theCommands, commandIndex);
    return true;
  }
  if (actualMultiplicands.size()==1 && theCoeff==1)
  { theExpression=actualMultiplicands[0];
    return true;
  }
  Expression* currentExpression;
  currentExpression=&theExpression;
  if (theCoeff!=1)
  { currentExpression->theOperation=theCommands.opTimes();
    currentExpression->children.resize(2);
    currentExpression->children[0].MakeInT(theCoeff, & theCommands, commandIndex);
    currentExpression=&currentExpression->children[1];
  }
//  Expression tmpExp;
  for (unsigned i=0; i<actualMultiplicands.size()-1; i++)
  { currentExpression->MakeProduct(& theCommands, commandIndex, actualMultiplicands[i], actualMultiplicands[i]);
    currentExpression=&currentExpression->children[1];
  }
  *currentExpression=actualMultiplicands[actualMultiplicands.size()-1];
  return true;
}

bool CommandList::EvaluateDoDistribute
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if ((left.theOperation==theCommands.opPlus() || left.theOperation==theCommands.opMinus()) && left.children.size()==2)
  { theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft(theCommands, commandIndex, theExpression);
    return true;
  }
  if ((right.theOperation==theCommands.opPlus() || right.theOperation==theCommands.opMinus()) && right.children.size()==2)
  { theCommands.EvaluateDoRightDistributeBracketIsOnTheRight(theCommands, commandIndex, theExpression);
    return true;
  }
  return false;
}

bool CommandList::EvaluateDoLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.theOperation!=theCommands.opTimes())
    return false;
  int leftOp=theExpression.children[0].theOperation;
  if (leftOp!=theCommands.opPlus() && leftOp!=theCommands.opMinus())
    return false;
  if (theExpression.children[0].children.size()!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(& theCommands, commandIndex);
  theExpression.theOperation=leftOp;
  theExpression.children.resize(2);
  theExpression.children[0].MakeProduct(&theCommands, commandIndex, left.children[0], right);
  theExpression.children[1].MakeProduct(&theCommands, commandIndex, left.children[1], right);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::EvaluateDoRightDistributeBracketIsOnTheRight
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.theOperation!=theCommands.opTimes())
    return false;
  int rightOp=theExpression.children[1].theOperation;
  if (rightOp!=theCommands.opPlus() && rightOp!=theCommands.opMinus())
    return false;
  if (theExpression.children[1].children.size()!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(& theCommands, commandIndex);
  theExpression.theOperation=rightOp;
  theExpression.children.resize(2);
  theExpression.children[0].MakeProduct(&theCommands, commandIndex, left, right.children[0]);
  theExpression.children[1].MakeProduct(&theCommands, commandIndex, left, right.children[1]);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::EvaluateStandardPlus
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ return theCommands.CollectSummands(commandIndex, theExpression);
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
  { theExpression.AssignChild(1);
    return true;
  }
  if (left.theData==0)
  { theExpression.AssignChild(2);
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardFunction
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.theOperation!=theCommands.opFunction())
    return false;
  if (theExpression.children.size()==0)
  { theExpression.errorString=
    "Programming error: function has no name; this should never happen. \
    Please dubug function CommandList::EvaluateStandardFunction";
    return true;
  }
  Expression& functionNameNode =theExpression.children[0];
  if (functionNameNode.theOperation!=theExpression.theBoss->opVariableNonBound())
    return false;
  bool (*theFun)(CommandList& , int , Expression& );
  theFun=theExpression.theBoss->theNonBoundVars [functionNameNode.theData].theHandler;
  if (theFun==0)
    return false;
  theFun(theCommands, commandIndex, theExpression);
  return true;
}

bool CommandList::EvaluateStandardEqualEqual
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ assert(theExpression.children.size()==2);
  Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if (left==right)
    theExpression.MakeInT(1, & theCommands, commandIndex);
  else
    theExpression.MakeInT(0, & theCommands, commandIndex);
  return true;
}

bool CommandList::EvaluateStandardMinus
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size()!=1&& theExpression.children.size()!=2)
  { theExpression.errorString="Programming error: operation - takes one or two arguments.";
    return false;
  }
  Expression* toBeTransformed=0;
  if (theExpression.children.size()==1)
  { theExpression.AssignChild(0);
    toBeTransformed=&theExpression;
  }
  else
  { toBeTransformed=& theExpression.children[1];
    theExpression.theOperation=theCommands.opPlus();
  }
  Expression result, minusOne;
  minusOne.MakeInT(-1, & theCommands, commandIndex);
  result.MakeProduct(& theCommands, commandIndex, minusOne, *toBeTransformed);
  *toBeTransformed=result;
  return true;
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
    this->evaluationErrors.push_back(out.str());
    return false;
  }
  if (!left.AreEqualExcludingChildren(right))
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
    this->evaluationErrors.push_back(out.str());
    return false;
  }
  if (this->opDefine()==input.theOperation)
    return false;
  int opVarB=this->opVariableBound();
  if (thePattern.theOperation== opVarB)
  { int indexLeft= matchedExpressions.BoundVariableIndices.GetIndex(thePattern.theData);
    if (indexLeft==-1)
    { matchedExpressions.BoundVariableIndices.AddOnTop(thePattern.theData);
      matchedExpressions.variableImages.AddOnTop(input);
      indexLeft=matchedExpressions.BoundVariableIndices.size()-1;
    }
    if (!(matchedExpressions.variableImages[indexLeft]==input))
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

bool CommandList::EvaluateExpressionReturnFalseIfExpressionIsBound
(int commandIndex, Expression& theExpression, int RecursionDepth, int maxRecursionDepth,
 ExpressionPairs& bufferPairs, std::stringstream* theLog)
{ if (RecursionDepth>=maxRecursionDepth)
  { std::stringstream out;
    out << "Recursion depth limit of " << this->MaxRecursionDepthDefault << " exceeded while evaluating expressions.";
    theExpression.errorString=out.str();
    this->evaluationErrors.push_back(out.str());
    return true;
  }
  if (theExpression.theOperation<0 || theExpression.theBoss!=this)
    return true;
  //reduction phase:
  bool NonReduced=true;
  int counter=-1;
  while (NonReduced)
  { const double MaxAllowedTimeInSeconds=30;
    if (this->GetElapsedTimeNonSafe!=0)
      if (this->GetElapsedTime()-this->StartTimeInSeconds >MaxAllowedTimeInSeconds)
      { std::cout << "<br><b>Max allowed computational time is " << MaxAllowedTimeInSeconds << ";  so far, "
        << this->GetElapsedTime()-this->StartTimeInSeconds  << " have elapsed -> aborting computation ungracefully.</b>";
        return true;
      }
    counter++;
    NonReduced=false;
    for (unsigned i=0; i<theExpression.children.size(); i++)
      if(! this->EvaluateExpressionReturnFalseIfExpressionIsBound
         (commandIndex, theExpression.children[i], RecursionDepth+1, maxRecursionDepth, bufferPairs, theLog))
        return false;
    if (counter>this->MaxAlgTransformationsPerExpression)
    { std::stringstream out;
      out << "Maximum number of algebraic transformations of " << this->MaxAlgTransformationsPerExpression << " exceeded.";
      theExpression.errorString=out.str();
      return true;
    }
    assert((signed) this->theStandardOpEvalFunctions.size()> theExpression.theOperation);
//    std::cout << "<hr>before standard eval: " << theExpression.ElementToString();
    if (this->theStandardOpEvalFunctions[theExpression.theOperation]!=0)
      if (this->theStandardOpEvalFunctions[theExpression.theOperation](*this, commandIndex, theExpression))
        NonReduced=true;
//    std::cout << "<br>after standard eval: " << theExpression.ElementToString();
//    for (unsigned i=0; i<this->targetProperties.size(); i++)
//      if (theExpression.GetPropertyValue(this->targetProperties[i])!=1)
//        if (this->theNonBoundVars[this->targetProperties[i]].theHandler!=0)
//          if (this->theNonBoundVars[this->targetProperties[i]].theHandler(*this, commandIndex, theExpression))
//            NonReduced=true;
    for (int i=0; i<commandIndex; i++)
      if (this->theCommands[i].ErrorString=="")
      { Expression& currentPattern=this->theCommands[i].finalValue;
        if (currentPattern.errorString=="")
          if (currentPattern.theOperation==this->opDefine() || currentPattern.theOperation==this->opDefineConditional())
          { this->TotalNumPatternMatchedPerformed++;
            if(this->ProcessOneExpressionOnePatternOneSub
            (commandIndex, currentPattern, theExpression, bufferPairs, RecursionDepth+1, maxRecursionDepth, theLog))
            { NonReduced=true;

              break;
            }
          }
      }
//    if (NonReduced)
//      theExpression.ElementToString();
  }
  if (theExpression.theOperation==this->opVariableBound())
    return false;
  return true;
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
  if (this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, RecursionDepth+1, MaxRecursionDepth, theLog))
  { if (theLog!=0 && logAttempts)
      (*theLog) << "<hr>found pattern: " << theExpression.ElementToString() << " -> " << thePattern.ElementToString();
    if (condition==0)
      return &theExpression;
    else
    { Expression tempExp=*condition;
      if (theLog!=0 && logAttempts)
        (*theLog) << "<hr>Specializing condition pattern: " << tempExp.ElementToString();
      this->SpecializeBoundVars(tempExp, commandIndex, bufferPairs, RecursionDepth+1, MaxRecursionDepth);
      if (theLog!=0 && logAttempts)
        (*theLog) << "<hr>Specialized condition: " << tempExp.ElementToString() << "; evaluating...";
      ExpressionPairs tempPairs;
      this->EvaluateExpressionReturnFalseIfExpressionIsBound
      (commandIndex, tempExp, RecursionDepth+1, MaxRecursionDepth, tempPairs, theLog);
      if (theLog!=0 && logAttempts)
        (*theLog) << "<hr>The evaluated specialized condition: " << tempExp.ElementToString() << "; evaluating...";
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

void CommandList::SpecializeBoundVars
(Expression& toBeSubbed, int targetCommandIndex, ExpressionPairs& matchedPairs, int RecursionDepth, int MaxRecursionDepth)
{ toBeSubbed.commandIndex=targetCommandIndex;
  if (toBeSubbed.theOperation==this->opVariableBound())
  { int indexMatching= matchedPairs.BoundVariableIndices.GetIndex(toBeSubbed.theData);
    toBeSubbed=matchedPairs.variableImages[indexMatching];
    this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
    return;
  }
  for (unsigned i=0; i<toBeSubbed.children.size(); i++)
    this->SpecializeBoundVars(toBeSubbed.children[i], targetCommandIndex, matchedPairs, RecursionDepth+1, MaxRecursionDepth);
  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
  (int commandIndex, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   int RecursionDepth, int maxRecursionDepth, std::stringstream* theLog, bool logAttempts)
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
  if ((thePattern.theOperation==this->opDefine()) || isConditionalDefine)
    toBeSubbed=this->DepthFirstSubExpressionPatternMatch
    (commandIndex, currentPattern, theExpression, bufferPairs, RecursionDepth+1, maxRecursionDepth,
     theCondition, theLog);
  if (toBeSubbed==0)
    return false;
  if (theLog!=0 && logAttempts)
    *theLog << "<br><b>found a match!</b>";
  if (isConditionalDefine)
    *toBeSubbed=thePattern.children[2];
  else
    *toBeSubbed=thePattern.children[1];
  this->SpecializeBoundVars(*toBeSubbed, toBeSubbed->commandIndex, bufferPairs, RecursionDepth+1, maxRecursionDepth);
  return true;
}

void CommandList::EvaluateCommands()
{ std::stringstream out;
  ExpressionPairs thePairs;

//  this->theLogs.resize(this->theCommands.size());
//this->ElementToString();
  std::stringstream loggingStream;
  if (this->syntaxErrors.size()>0)
  { out << "<hr><b>Syntax errors encountered</b><br>";
    for (unsigned i=0; i<this->syntaxErrors.size(); i++)
      out << this->syntaxErrors[i];
    out << "<hr>";
  }
  out << "<table border=\"1\">";
  for (unsigned i=0; i<this->theCommands.size(); i++)
  { out  << "<tr><td>" << i+1 << ": </td><td><span class=\"math\">" << this->theCommands[i].finalValue.ElementToString()
    << "</span></td><td>=</td>";
    std::stringstream localLogger;
    assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    if (this->theCommands[i].ErrorString=="")
    { this->EvaluateExpressionReturnFalseIfExpressionIsBound
    (i, this->theCommands[i].finalValue, 0, this->MaxRecursionDepthDefault, thePairs, &localLogger);
      assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    }
    std::string commandOutput=this->theCommands[i].finalValue.ElementToString();
    assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    if (commandOutput.size() <300)
      out << "<td><span class=\"math\">" << commandOutput << "</span></td>";
    else
      out << "<td><b>output is more than 300 characters, use the non- LaTeX version to the right. </b></td>";
//    std::cout <<"<hr>" << this->theCommands[i].finalValue.ElementToStringPolishForm(0,100);

    out << "<td>=</td><td>" << commandOutput;
    out << "</td>";
//    out << " ( for debugging: " << this->theCommands[i].finalValue.ElementToStringPolishForm() << ")";
    this->theCommands[i].theLog= localLogger.str();
    if (this->evaluationErrors.size()>0)
    { out << "<tr><td>Errors encountered; command evaluation terminated. Error messages follow.";
      for (unsigned i=0; i<this->evaluationErrors.size(); i++)
        out << "<br>" << this->evaluationErrors[i];
      out << "</td></tr>";
      break;
    }
    out << "</td></tr>";
  }
  out << "</table>";
  loggingStream << "<b>CommandList status. </b><br>";
  loggingStream << this->ElementToString();
  this->theLog= loggingStream.str();
  this->output=out.str();
}

std::string SyntacticElement::ElementToString(CommandList& theBoss)
{ std::stringstream out;
  if (this->controlIndex==theBoss.conExpression())
    out <<"<table border=\"1\"><tr><td>";
  if (this->controlIndex<0)
    out << "Error: control index is not initialized! This is likely a programming error.";
  else
    out << theBoss.controlSequences[this->controlIndex];
  if (this->controlIndex==theBoss.conExpression())
  { out << "</td></tr><tr><td>";
    out << this->theData.ElementToString(0, 10);
    out << "</td></tr></table>";
  }
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
  if (this->errorString!="")
    out << "Error: " << this->errorString << " ";
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
    << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, true)
    << ":=" << this->children[2].ElementToString(recursionDepth+1, maxRecursionDepth);
  else if (this->theOperation==this->theBoss->opDivide() )
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication())
    << "/" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForMultiplication());
  else if (this->theOperation==this->theBoss->opTimes() )
  { std::string tempS=this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication());
//    if (false)
//    {
    if (tempS=="-1")
      tempS="-";
    if (tempS=="1")
      tempS="";
//    } else
//      tempS="("+tempS+")";
    out << tempS;
    if (this->format==this->formatTimesDenotedByStar && tempS!="-" && tempS!="")
      out << "*"; else out << " ";
    out << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForMultiplication());
  }
  else if (this->theOperation==this->theBoss->opThePower())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForThePower())
    << "^{" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false) << "}";
  else if (this->theOperation==this->theBoss->opPlus() )
  { assert(this->children.size()>=2);
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition());
    std::string tempS=this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForAddition());
    if (tempS.size()>0)
      if (tempS[0]!='-')
        out << "+";
    out << tempS;
  }
  else if (this->theOperation==this->theBoss->opMinus())
  { if ( this->children.size()==1)
      out << "-" << this->children[0].ElementToString
      (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication());
    else
    { assert(children.size()==2);
      out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition())
      << "-" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForAddition());
    }
  }
  else if (this->theOperation==this->theBoss->opVariableBound())
  { assert((unsigned) this->commandIndex< this->theBoss->theCommands.size());
    assert(this->theBoss->theCommands[this->commandIndex].BoundVariables.size()>0);
    assert(this->theBoss->theCommands[this->commandIndex].BoundVariables.size()>this->theData);
    out << "{{" << this->theBoss->theCommands[this->commandIndex].BoundVariables[this->theData] << "}}";
  }
  else if (this->theOperation==this->theBoss->opVariableNonBound())
    out << this->theBoss->theNonBoundVars[this->theData].theName;
  else if (this->theOperation==this->theBoss->opInteger())
    out << this->theData;
  else if (this->theOperation==this->theBoss->opFunction())
  { assert(this->children.size()>=2);
    out << this->children[0].ElementToString
    (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForFunctionName());
    int theFormat=this->format;
    switch(theFormat)
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
  else if (this->theOperation==this->theBoss->opEqualEqual())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth)
    << "==" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth);
  else
    out << "?operation not documented? This is probably a programming error!" ;
  if (AddBrackets)
    out << ")";
  if (AddCurlyBraces)
    out << "}";
  return out.str();
}

std::string Expression::ElementToStringPolishForm(int recursionDepth, int maxRecursionDepth)
{ if (maxRecursionDepth>0)
    if(recursionDepth>maxRecursionDepth)
      return "...and so on (maximum depth reached)...";
  if (this->theBoss==0)
    return " non-initialized ";
  std::stringstream out;
  if (this->theOperation<0)
  { out << " operation not initialized ";
    return out.str();
  }
  if (this->children.size()>0)
    out << "<table border=\"1\"> <tr><td  align=\"center\" colspan=\"" << this->children.size() << "\">";
  out << this->theBoss->operations[this->theOperation] << " (operation index " << this->theOperation << ")";
  out << ", " << this->theData;
  if (this->children.size()>0)
  { out << "</td></tr><td>";
    for (unsigned i=0; i<this->children.size(); i++)
    { out << this->children[i].ElementToStringPolishForm(recursionDepth+1, maxRecursionDepth);
      if (i!=this->children.size()-1)
        out << "</td><td> ";
    }
    out << "</td></tr>";
  }
  return out.str();
}

std::string CommandList::ElementToString()
{ std::stringstream out;
  out << "Total number of pattern matches performed: " << this->TotalNumPatternMatchedPerformed;
  out << "<br>Elapsed time since evaluation was started: " << this->GetElapsedTime() - this->StartTimeInSeconds;
  out << "<br>Control sequences (" << this->controlSequences.size() << " total):\n<br>\n";
  for (int i=0; i<this->controlSequences.size(); i++)
  { out << this->controlSequences[i] << ", ";
  }
  out << "<br>\nOperators (" << this->operations.size() << " total):\n<br>\n";
  for (int i=0; i<this->operations.size(); i++)
  { out << this->operations[i] << ", ";
  }
  out << "<br>\n Dictionary (" << this->theDictionary.size() << " total):\n<br>\n";
  for (int i=0; i<this->theDictionary.size(); i++)
  { out << this->theDictionary[i] << ", ";
  }
  out << "<br>\n Global variables (" << this->theNonBoundVars.size() << " total):\n<br>\n";
  for (int i=0; i<this->theNonBoundVars.size(); i++)
  { out << this->theNonBoundVars[i].theName << ", ";
  }
  assert(this->cachedExpressions.size()==(signed)this->imagesCashedExpressions.size());
  if (this->cachedExpressions.size()>0)
  { out << "<br>\n Cached expressions: (" << this->cachedExpressions.size() << " total):\n<br>\n";
    for (int i=0; i<this->cachedExpressions.size(); i++)
    { out << this->cachedExpressions[i].ElementToString() << " -> " << this->imagesCashedExpressions[i].ElementToString() << "<br>";
    }
  }
  for (unsigned i=0; i<this->theCommands.size(); i++)
  { out << this->theCommands[i].ElementToString();
  }
  return out.str();
}

std::string Command::ElementToStringSyntacticStack()
{ std::stringstream out;
  if ((signed) this->syntacticStack.size()<this->numEmptyTokensStart)
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  if (this->theBoss==0)
    return "Error: command is not initialized properly. This is likely a programming error";
  bool HasMoreThanOneSignificantEntries=((signed)this->syntacticStack.size()>this->numEmptyTokensStart+1);
  if (HasMoreThanOneSignificantEntries)
    out << "<table border=\"1\"><tr><td>";
  for (unsigned i=this->numEmptyTokensStart; i<this->syntacticStack.size(); i++)
  { out << this->syntacticStack[i].ElementToString(*this->theBoss);
    if (HasMoreThanOneSignificantEntries)
    { out << "</td>";
      if (i!=this->syntacticStack.size()-1)
        out << "<td>";
    }
  }
  if (HasMoreThanOneSignificantEntries)
    out << "</td></tr></table>";
  return out.str();
}

bool Expression::NeedBracketsForFunctionName() const
{ return !(
  this->theOperation==this->theBoss->opVariableBound() ||
  this->theOperation==this->theBoss->opVariableNonBound() ||
  (this->theOperation==this->theBoss->opFunction() && this->format==this->formatFunctionUseUnderscore)
  );
}

bool Expression::NeedBracketsForMultiplication()const
{ //return this->children.size()>1;
  return
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

void Expression::MakeInT(int theValue, CommandList* newBoss, int indexOfTheCommand)
{ this->reset(newBoss, indexOfTheCommand);
  this->theData=theValue;
  this->theOperation=newBoss->opInteger();
}

void Expression::MakeVariableNonBoundNoProperties
  (CommandList* owner, int indexOfTheCommand, int varIndex)
{ this->reset(owner, indexOfTheCommand);
  this->theData=varIndex;
  this->theOperation=owner->opVariableNonBound();
}

void Expression::MakeFunction
  (CommandList* owner, int indexOfTheCommand, const Expression& argument, int functionIndex)
{ this->reset(owner, indexOfTheCommand);
  this->theOperation=owner->opFunction();
  this->children.resize(2);
  this->children[0].MakeVariableNonBoundNoProperties(owner, commandIndex, functionIndex);
  this->children[1]=argument;
}

void Expression::MakeFunction
  (CommandList* owner, int indexOfTheCommand, const Expression& argument, const std::string& functionName)
{ int index=owner->GetIndexNonBoundVar(functionName);
  assert(index!=-1);
  this->MakeFunction(owner, indexOfTheCommand, argument, index);
}

#include <sys/time.h>
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

std::string UnCivilizeStringCGI(const std::string& input)
{ std::stringstream out;
  for (unsigned int i=0; i<input.size(); i++)
    switch (input[i])
    { case ' ': out << "+"; break;
      case '+': out << "%2B"; break;
      case '(': out << "%28"; break;
      case ')': out << "%29"; break;
      case '[': out << "%5B"; break;
      case ']': out << "%5D"; break;
      case ',': out << "%2C"; break;
      case '{': out << "%7B"; break;
      case ';': out << "%3B"; break;
      case '/': out << "%2F"; break;
      case ':': out << "%3A"; break;
      case '^': out << "%5E"; break;
      case '\\': out << "%5C"; break;
      case '}': out << "%7D"; break;
      case '=': out << "%3D"; break;
      case '\n': out << "%0D%0A"; break;
//      case '': out << ""; break;
      default: out << input[i]; break;
    }
  return out.str();
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
  std::cout << "<html><head></head><title>Symbolic calculator updated " << __DATE__ << "</title>  <body> <script src=\"/vpf/jsmath/easy/load.js\"></script>";
  std::string inputString;
//  std::cout << "<br>Uncivilized string: " << inputStringNonCivilized;
  CivilizeCGIString(inputStringNonCivilized, inputString);
  int numBogusChars= 4;
  for (signed i=0; i<signed(inputString.size())-numBogusChars; i++)
  { inputString[i]=inputString[i+numBogusChars];
  }
  int newSize=signed(inputString.size())-numBogusChars;
  if (newSize<0)
    newSize=0;
  inputString.resize(newSize);
//  inputString="{{b}}{{a}}+{{c}}a:if(IsInteger{}(b)IsInteger{}(c)):=(b+c)a;2 *d+3 *d";
//  inputString="{{b}}{{a}}+{{c}}a:if(IsInteger{}(b)IsInteger{}(c)):=(b+c)a";
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
//  inputString="IsInteger{};IsInteger(){}";
//  inputString="2*c*3*a*5*d*2";
 // inputString="2*c*3";
//  inputString="c*1";
//inputString="2*c*d*1";
//  inputString="a+a";
//  inputString="1*1a";
//inputString="a*b";
//inputString="2a b*2+a b";
//inputString="x*1";
//  inputString="-3";
//  inputString="-a";
  std::cout << "<table><tr><td valign=\"top\">";
  std::cout << "\n<form method=\"POST\" name=\"formCalculator\" action=\"/vpf/cgi-bin/symbolicCalculator\">\n" ;
  std::cout << "<textarea rows=\"10\" cols=\"100\" name=\"in1\" id=\"in1\">";
  std::cout << inputString;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
//  std::cout << "<a href=\"/tmp/indicator.html\" target=\"_blank\"> Indicator window  </a>";
  std::cout << "\n</form>";

  CommandList theComputation;
  theComputation.GetElapsedTimeNonSafe=GetElapsedTimeInSeconds;
  theComputation.Evaluate(inputString);

  std::cout << "<hr><b>Result.</b><br>";
  if (inputString!="")
    std::cout <<  theComputation.output;
  std::cout << "</td><td valign=\"top\">";
  std::cout << "<a href=\"/vpf/cgi-bin/symbolicCalculator?" << inputStringNonCivilized << "\">Link to your input.</a><br>";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/vpf6.cpp?view=markup\"> C++ source of the calculator</a>\n<hr>";
  std::cout << theComputation.theLog;
  std::cout << "</td></tr></table>";
  std::cout << "</body></html>";
//  std::system("cls");
	return 0;   // To avoid Apache errors.
}
