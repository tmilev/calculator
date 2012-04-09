#include "vpf.h"

inline int IntIdentity(const int& x)
{ return x;
}

void Expression::MakeProducT
  (CommandList* owner, int commandIndex, const Expression& left, const Expression& right)
{ this->MakeXOX(owner, commandIndex, owner->opTimes(), left, right);
}

void Expression::MakeXOX
  (CommandList* owner, int commandIndex, int theOp, const Expression& left, const Expression& right)
{ this->reset(owner, commandIndex);
  this->theOperation=theOp;
  this->children.SetSize(2);
  this->children[0]=left;
  this->children[1]=right;
}

bool Command::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=this->syntacticStack[this->syntacticStack.size-8];
  SyntacticElement& leftE = this->syntacticStack[this->syntacticStack.size-6];
  SyntacticElement& middleE= this->syntacticStack[this->syntacticStack.size-4];
  SyntacticElement& rightE = this->syntacticStack[this->syntacticStack.size-2];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.format=formatOptions;
  newExpr.children.AddOnTop(leftE.theData);
  newExpr.children.AddOnTop(middleE.theData);
  newExpr.children.AddOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(7);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOXXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=this->syntacticStack[this->syntacticStack.size-9];
  SyntacticElement& leftE = this->syntacticStack[this->syntacticStack.size-6];
  SyntacticElement& middleE= this->syntacticStack[this->syntacticStack.size-4];
  SyntacticElement& rightE = this->syntacticStack[this->syntacticStack.size-2];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.AddOnTop(leftE.theData);
  newExpr.children.AddOnTop(middleE.theData);
  newExpr.children.AddOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(8);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOXEByE(int formatOptions)
{ SyntacticElement& left=this->syntacticStack[this->syntacticStack.size-3];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(left.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.AddOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceOEByE(int formatOptions)
{ SyntacticElement& middle=this->syntacticStack[this->syntacticStack.size-2];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.AddOnTop(right.theData);
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
    case '$':
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
    case '$':
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
{ this->theCommands.SetSize(this->theCommands.size+1);
  Command& currentCommand=this->theCommands[this->theCommands.size-1];
  currentCommand.reset(this, this->theCommands.size-1);
}

std::string ExpressionPairs::ElementToString()
{ std::stringstream out;
  out << "the pairs: ";
  for (int i=0; i<this->BoundVariableIndices.size; i++)
    out << this->BoundVariableIndices[i] << "->" << this->variableImages[i].ElementToString() << "<br>";
  return out.str();
}

void CommandList::ParseFillDictionary
  (const std::string& input)
{ std::string current;
  this->theCommands.Reserve(1000);
  this->theCommands.SetSize(0);
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
    { this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(this->GetSyntacticElementEnd());
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
          this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(currentElement);
        } else if (this->isADigit(current, currentDigit))
        { Data tempData;
          tempData.SetInt(currentDigit);
          currentElement.controlIndex=this->conData();
          currentElement.theData.reset(this, this->theCommands.size-1);
          currentElement.theData.theOperation=this->opData();
          currentElement.theData.theData=this->theData.AddNoRepetitionOrReturnIndexFirst(tempData);
          this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(currentElement);
        } else
        { this->theDictionary.AddNoRepetition(current);
          currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
          currentElement.theData.reset(this, this->theCommands.size-1);
          currentElement.theData.theOperation=this->operations.GetIndex("Variable");
          currentElement.theData.theData=this->theDictionary.GetIndex(current);
          this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(currentElement);
        }
        current="";
      }
  }
  if (!lastStatementProperlyTerminated)
    this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(this->GetSyntacticElementEnd());
}

int Command::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->theBoss->operations.GetIndex(this->theBoss->controlSequences[controlIndex]);
}

int Command::GetExpressionIndex()
{ return this->theBoss->controlSequences.GetIndex("Expression");
}

bool Command::ReplaceObyE()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size-1];
  theElt.theData.theOperation=this->theBoss->operations.GetIndex(this->theBoss->controlSequences[theElt.controlIndex]);
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::ReplaceVbyE()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size-1];
  const std::string& theVarString=this->theBoss->theDictionary[theElt.theData.theData];
  int indexBoundVar=this->BoundVariables.GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->theBoss->opVariableBound();
    theElt.theData.theData=indexBoundVar;
  } else
  { theElt.theData.theOperation=this->theBoss->opVariableNonBound();
    theElt.theData.theData=this->theBoss->AddNonBoundVarReturnVarIndex(theVarString, 0, "", "", "");
    //note:     theElt.theData.theOperation.theData     should be initialized with the index of the non-bound variable!
  }
  theElt.controlIndex=this->theBoss->conExpression();
//  std::cout << "now i'm here!";
  return true;
}

bool Command::ReplaceVXbyEX()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size-2];
  const std::string& theVarString=this->theBoss->theDictionary[theElt.theData.theData];
  int indexBoundVar=this->BoundVariables.GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->theBoss->opVariableBound();
    theElt.theData.theData=indexBoundVar;
  } else
  { theElt.theData.theOperation=this->theBoss->opVariableNonBound();
    theElt.theData.theData=this->theBoss->AddNonBoundVarReturnVarIndex(theVarString, 0, "", "", "");
  }
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
}

bool Command::RegisterBoundVariable()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size-3];
  const std::string& theVarString=this->theBoss->theDictionary[theElt.theData.theData];
  if (!this->BoundVariables.Contains(theVarString))
  { if (!this->flagOpDefineEncountered)
      this->BoundVariables.AddOnTop(theVarString);
    else
    { theElt.controlIndex=this->theBoss->conError();
      theElt.ErrorString=  "Error: bound variables cannot be declared after the definition operation := .";
    }
  } else
  { this->DecreaseStackSetCharacterRanges(2);
    this->ReplaceXXYByY();
  }
  return true;
}

bool Command::ApplyOneRule(const std::string& lookAhead)
{ //return false;
  const SyntacticElement& lastE=this->syntacticStack[this->syntacticStack.size-1];
  const std::string&    lastS=this->theBoss->controlSequences[lastE.controlIndex];
  if (lastS==" " && signed (this->syntacticStack.size)>this->numEmptyTokensStart)
  { this->syntacticStack.SetSize(this->syntacticStack.size-1);
    return false;
  }
  const SyntacticElement& secondToLastE=this->syntacticStack[this->syntacticStack.size-2];
  const std::string& secondToLastS=this->theBoss->controlSequences[secondToLastE.controlIndex];
  const SyntacticElement& thirdToLastE=this->syntacticStack[this->syntacticStack.size-3];
  const std::string& thirdToLastS=this->theBoss->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE=this->syntacticStack[this->syntacticStack.size-4];
  const std::string& fourthToLastS=this->theBoss->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE=this->syntacticStack[this->syntacticStack.size-5];
  const std::string& fifthToLastS=this->theBoss->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE=this->syntacticStack[this->syntacticStack.size-6];
  const std::string& sixthToLastS=this->theBoss->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE=this->syntacticStack[this->syntacticStack.size-7];
  const std::string& seventhToLastS=this->theBoss->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE=this->syntacticStack[this->syntacticStack.size-8];
  const std::string& eighthToLastS=this->theBoss->controlSequences[eighthToLastE.controlIndex];
//  const SyntacticElement& ninthToLastE=this->syntacticStack[this->syntacticStack.size()-9];
//  const std::string& ninthToLastS=this->theBoss->controlSequences[ninthToLastE.controlIndex];

//  std::cout << "<hr>" << this->ElementToString();
//this->theBoss->ElementToString();
  if (secondToLastS==":" && lastS=="=")
  { this->flagOpDefineEncountered=true;
    return this->ReplaceXXByCon(this->theBoss->conDefine());
  }
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->theBoss->conDeclareFunction(), Expression::formatDefault);
  if (lastS=="$")
    return this->ReplaceXByCon(this->theBoss->conDeclareFunction(), Expression::formatDefault);
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
  if (lastS=="Data" && secondToLastS=="Data")
    return this->ReplaceIntIntBy10IntPlusInt();
  if (lastS=="Data" && lookAhead!="Data")
  { this->syntacticStack[this->syntacticStack.size-1].theData.MakeData
    (lastE.theData.theData, *lastE.theData.theBoss, lastE.theData.commandIndex);
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
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
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

bool CommandList::fPolynomial
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ Data outputData;
  std::stringstream errorLog;
  outputData.type=outputData.typePoly;
  HashedList<Expression> VariableImages;
  if (!theCommands.ExtractPolyRational(outputData.thePoly.GetElement(), theExpression, VariableImages, 0, 10000, &errorLog))
  { outputData.type=outputData.typeError;
    outputData.theError.GetElement()=errorLog.str();
  }
  theExpression.theOperation=theCommands.opData();
  theExpression.theData=theCommands.theData.AddNoRepetitionOrReturnIndexFirst(outputData);
  std::stringstream out;
  for (int i=0; i<VariableImages.size; i++)
  { out  << "<br> x_{" << i+1 << "}:=" << VariableImages[i].ElementToString(0, 10);
    if (i!=VariableImages.size-1)
      out << ";";
  }
  theExpression.theComments=out.str();
  return true;
}

bool CommandList::fElementSSAlgebra
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{
  return false;
}

bool CommandList::fSSAlgebra
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ /*SemisimpleLieAlgebra theSSalgebra;
  std::stringstream errorStream;
  if (theExpression.children.size!=2)
  { errorStream << "Error: the simple Lie algebra takes one argument and I received "
    << theExpression.children.size-1 << " instead.";
    return theExpression.SetError(errorStream.str());
  }
  Expression& theArgs=theExpression.children[1];
  if (theArgs.children.size!=2)
  { errorStream << "Error: the argument of the simple Lie algebra must be of the form Something_Something";
    return theExpression.SetError(errorStream.str());
  }
  Expression& leftE=theArgs.children[0];
  Expression& rightE=theArgs.children[1];
  if (theExpression.children.size==1)
  {
  }
  else
  { leftE=&theExpression.children[0];
    rightE=&theExpression.children[1];
  }
  if (leftE.theOperation!=theCommands.opVariableNonBound)
  { errorStream << "Error: the first argument of the ";
  }*/
  return false;
}

bool CommandList::fIsInteger
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ bool IsInteger=false;
  if (theExpression.children.size==2)
    if (theExpression.children[1].theOperation==theExpression.theBoss->opData())
    { IsInteger=theCommands.theData[theExpression.children[1].theData].IsInteger();
    }
  Data endData;
  if (IsInteger)
    endData=1;
  else
    endData=0;
  theExpression.children.SetSize(0);
  theExpression.theOperation=theExpression.theBoss->opData();
  theExpression.theData=theCommands.theData.AddNoRepetitionOrReturnIndexFirst(endData);
  return true;
}

bool CommandList::AppendOpandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, List<Expression>& output, int theOp, int RecursionDepth, int MaxRecursionDepth)
{ if (RecursionDepth>MaxRecursionDepthDefault)
    return false;
  bool result=false;
  if (theExpression.theOperation!=theOp)
    output.AddOnTop(theExpression);
  else
    for (int i=0; i<theExpression.children.size; i++)
    { if (this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression.children[i], output, theOp, RecursionDepth+1, MaxRecursionDepth))
        result=true;
      if (i<theExpression.children.size-1 &&
          theExpression.children[i].theOperation==theOp &&
          theExpression.children[i].children.size>1)
        result=true;
    }
  return result;
}

void CommandList::initPredefinedVars()
{ this->AddNonBoundVarMustBeNew("IsInteger", &this->fIsInteger, "", " returns 1 if the argument is an integer, 0 otherwise. ", "");
  this->AddNonBoundVarMustBeNew("SemisimpleLieAlgebra", & this->fSSAlgebra, "", "creates a semisimple Lie algebra (at the moment implemented for simple Lie algebras only).", "");
  this->AddNonBoundVarMustBeNew("ElementSemisimpleAlgebra", & this->fElementSSAlgebra, "", "Creates an element of a semisimple Lie algebra.", "");
  this->AddNonBoundVarMustBeNew("Polynomial", & this->fPolynomial, "", "Creates an internal data structure representation of a polynomial expression, implemented without use of expression trees.", "");
}

void CommandList::init()
{ this->MaxAlgTransformationsPerExpression=100000;
  this->MaxRecursionDepthDefault=10000;
  this->MaxAllowedTimeInSeconds=10000;
  this->controlSequences.Clear();
  this->operations.Clear();
  this->theNonBoundVars.Clear();
  this->theData.Clear();
  this->theDictionary.Clear();
  this->cachedExpressions.Clear();
  this->imagesCashedExpressions.SetSize(0);
  this->theFunctions.Clear();
  this->syntaxErrors.SetSize(0);
  this->evaluationErrors.SetSize(0);
  this->targetProperties.SetSize(0);
  this->AddOperationNoFail("+",this->EvaluateStandardPlus, "Plus", "", "", "");
  this->AddOperationNoFail("-", this->EvaluateStandardMinus, "Minus", "", "", "");
  this->AddOperationNoFail("/", this->EvaluateStandardDivide, "Divide", "", "", "");
  this->AddOperationNoFail("*", this->EvaluateStandardTimes, "Times", "", "", "");
  this->AddOperationNoFail(":=", 0, ":=", "", "", "");
  this->AddOperationNoFail("if:=", 0, "if:=", "", "", "");
  this->AddOperationNoFail("^", 0, "Power", "", "", "");
  this->AddOperationNoFail("==", this->EvaluateStandardEqualEqual, "==", "", "", "");
  //the following operation for function application is chosen on purpose so that it corresponds to LaTeX-undetectable
  //expression
  this->AddOperationNoFail("{}", this->EvaluateStandardFunction, "ApplyFunction", "", "", "");
  //the following is the binding variable operation
  this->AddOperationNoFail("VariableNonBound", 0, "VariableNonBound", "", "", "");
  this->AddOperationNoFail("VariableBound", 0, "BoundVariable", "", "", "");
  this->AddOperationNoFail("if", this->EvaluateIf, "If", "", "", "");
  this->AddOperationNoFail("Error", 0, "Error", "", "", "");
  this->AddOperationNoFail("Data", 0, "Data", "", "", "");

  this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTop("{{}}");
  this->controlSequences.AddOnTop("Variable");
  for (int i=0; i<this->operations.size; i++)
    this->controlSequences.AddOnTop(this->operations[i].theName);//all operations are also control sequences
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
  this->controlSequences.AddOnTop("$");
//    this->thePropertyNames.AddOnTop("IsCommutative");
  this->TotalNumPatternMatchedPerformed=0;
  this->initPredefinedVars();
  this->initTargetProperties();
}

bool CommandList::CollectSummands(int commandIndex, Expression& theExpression)
{ List<Expression>& summands= this->buffer1;
  summands.SetSize(0);
  bool needSimplification=this->AppendSummandsReturnTrueIfOrderNonCanonical
  (theExpression, summands, 0, this->MaxRecursionDepthDefault);
  HashedList<Expression> summandsNoCoeff;
  List<Data> theCoeffs;
  Data constTerm=0;
  bool foundConstTerm=false;
//  std::cout << "<b>" << theExpression.ElementToString() << "</b>";
//  if (theExpression.ElementToString()=="(4)*(a) b+(a) b")
//    std::cout << "problem!";
  for (int i=0; i<summands.size; i++)
  { Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
//    this->OrderMultiplicationTreeProperly(commandIndex, *currentSummandNoCoeff);
    Data theCoefF=1;
    if (currentSummandNoCoeff->theOperation==this->opTimes())
    { if(currentSummandNoCoeff->children[0].theOperation==this->opData())
      { theCoefF=this->theData[currentSummandNoCoeff->children[0].theData];
        currentSummandNoCoeff=& currentSummandNoCoeff->children[1];
        if (theCoefF.IsEqualToZero())
          needSimplification=true;
      }
    } else if (currentSummandNoCoeff->theOperation==this->opData())
    { constTerm+=this->theData[currentSummandNoCoeff->theData];
      if (foundConstTerm || this->theData[currentSummandNoCoeff->theData].IsEqualToZero())
        needSimplification=true;
      foundConstTerm=true;
      continue;
    }
    int currentIndex= summandsNoCoeff.GetIndex(*currentSummandNoCoeff);
//    std::cout << "<hr>" << currentSummandNoCoeff->ElementToStringPolishForm(0,1000);
//    std::cout << "<hr>" << currentSummandNoCoeff->ElementToString();
    if (currentIndex==-1)
    { summandsNoCoeff.AddOnTop(*currentSummandNoCoeff);
      theCoeffs.AddOnTop(0);
      currentIndex=summandsNoCoeff.size-1;
    } else
      needSimplification=true;
    theCoeffs[currentIndex]+=theCoefF;
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
  for (int i=0; i<summandsNoCoeff.size; i++)
    if (theCoeffs[i].IsEqualToZero())
    { theCoeffs[i]=theCoeffs[theCoeffs.size-1];
      summandsNoCoeff.PopIndexSwapWithLast(i);
      i--;
    }
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.Reserve(summandsNoCoeff.size);
  for (int i=0; i<summandsNoCoeff.size; i++)
  { summandsWithCoeff.SetSize(summandsWithCoeff.size+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size-1];
    current.reset(this, commandIndex);
    current.theOperation=this->opTimes();
    current.children.SetSize(2);
    current.children[0].MakeData(this->theData.AddNoRepetitionOrReturnIndexFirst(theCoeffs[i]), *this, commandIndex);
    current.children[1]=summandsNoCoeff[i];
  }
  if (!constTerm.IsEqualToZero() || summandsNoCoeff.size==0)
  { summandsWithCoeff.SetSize(summandsWithCoeff.size+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size-1];
    current.MakeData(this->theData.AddNoRepetitionOrReturnIndexFirst(constTerm), *this, commandIndex);
  }
  if (summandsWithCoeff.size==1)
  { theExpression=summandsWithCoeff[0];
    return true;
  }
  Expression* currentExp;
  currentExp=&theExpression;
  for (int i=0; i<summandsWithCoeff.size-1; i++)
  { currentExp->reset(this, commandIndex);
    currentExp->theOperation=this->opPlus();
    currentExp->children.SetSize(2);
    currentExp->children[0]=summandsWithCoeff[i];
    currentExp=& currentExp->children[1];
  }
  *currentExp=summandsWithCoeff[summandsWithCoeff.size-1];
  return true;
}

bool CommandList::EvaluateStandardTimes
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ElementToString();
  if (theCommands.EvaluateDoDistribute(theCommands, commandIndex, theExpression, theCommands.opTimes(), theCommands.opPlus()))
    return true;
  //std::cout << "<br>After distribute: " << theExpression.ElementToString();
  if (theCommands.EvaluateDoAssociate(theCommands, commandIndex, theExpression))
    return true;
  if (theExpression.children.size!=2)
    return false;
  //std::cout << "<br>After do associate: " << theExpression.ElementToString();
  return false;
}

bool CommandList::EvaluateDoAssociate
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ List<Expression>& multiplicands=theCommands.buffer1;
  List<Expression>& actualMultiplicands=theCommands.buffer2;
  multiplicands.SetSize(0);
  //std::cout << "<br>At start of do associate: " << theExpression.ElementToString();
  bool needsModification=theCommands.AppendMultiplicandsReturnTrueIfOrderNonCanonical
  (theExpression, multiplicands, 0, theCommands.MaxRecursionDepthDefault);
  actualMultiplicands.Reserve(multiplicands.size);
  actualMultiplicands.SetSize(0);
  Data theCoeff=1;
  for (int i=0; i<multiplicands.size; i++)
    if (multiplicands[i].theOperation==theCommands.opData())
    { theCoeff*=theCommands.theData[multiplicands[i].theData];
      if (i>0)
        needsModification=true;
    } else
      actualMultiplicands.AddOnTop(multiplicands[i]);
  if (theCoeff==0 && multiplicands.size>1)
    needsModification=true;
  if (!needsModification)
  { //std::cout << " no modification do associate: " << theExpression.ElementToString();
    return false;
  }
  if (theCoeff==0)
    actualMultiplicands.SetSize(0);
  if (actualMultiplicands.size==0)
  { theExpression.MakeData(theCommands.theData.AddNoRepetitionOrReturnIndexFirst(theCoeff), theCommands, commandIndex);
    //std::cout << "<br>  do associate return: " << theExpression.ElementToString();
    return true;
  }
  if (actualMultiplicands.size==1 && theCoeff==1)
  { theExpression=actualMultiplicands[0];
    //std::cout << "<br>  do associate return: " << theExpression.ElementToString();
    return true;
  }
  Expression* currentExpression;
  currentExpression=&theExpression;
  if (theCoeff!=1)
  { currentExpression->theOperation=theCommands.opTimes();
    currentExpression->children.SetSize(2);
    currentExpression->children[0].MakeData(theCommands.theData.AddNoRepetitionOrReturnIndexFirst(theCoeff), theCommands, commandIndex);
    currentExpression=&currentExpression->children[1];
  }
  for (int i=0; i<actualMultiplicands.size-1; i++)
  { currentExpression->MakeProducT(& theCommands, commandIndex, actualMultiplicands[i], actualMultiplicands[i]);
    currentExpression=&currentExpression->children[1];
  }
  *currentExpression=actualMultiplicands[actualMultiplicands.size-1];
  //std::cout << "<br>At end do associate: " << theExpression.ElementToString();
  return true;
}

bool CommandList::EvaluateDoDistribute
(CommandList& theCommands, int commandIndex, Expression& theExpression, int theMultiplicativeOP, int theAdditiveOp)
{ Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if ((left.theOperation==theCommands.opPlus() || left.theOperation==theCommands.opMinus()) && left.children.size==2)
  { theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft(theCommands, commandIndex, theExpression, theMultiplicativeOP, theAdditiveOp);
    return true;
  }
  if ((right.theOperation==theCommands.opPlus() || right.theOperation==theCommands.opMinus()) && right.children.size==2)
  { theCommands.EvaluateDoRightDistributeBracketIsOnTheRight(theCommands, commandIndex, theExpression, theMultiplicativeOP);
    return true;
  }
  return false;
}

bool CommandList::EvaluateDoLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, int commandIndex, Expression& theExpression, int theMultiplicativeOP, int theAdditiveOp)
{ if (theExpression.theOperation!=theMultiplicativeOP)
    return false;
  if (theExpression.children[0].theOperation!=theAdditiveOp)
    return false;
  if (theExpression.children[0].children.size!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(& theCommands, commandIndex);
  theExpression.theOperation=theAdditiveOp;
  theExpression.children.SetSize(2);
  theExpression.children[0].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left.children[0], right);
  theExpression.children[1].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left.children[1], right);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::EvaluateDoRightDistributeBracketIsOnTheRight
(CommandList& theCommands, int commandIndex, Expression& theExpression, int theMultiplicativeOP)
{ if (theExpression.theOperation!=theMultiplicativeOP)
    return false;
  int rightOp=theExpression.children[1].theOperation;
  if (rightOp!=theCommands.opPlus() && rightOp!=theCommands.opMinus())
    return false;
  if (theExpression.children[1].children.size!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(& theCommands, commandIndex);
  theExpression.theOperation=rightOp;
  theExpression.children.SetSize(2);
  theExpression.children[0].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left, right.children[0]);
  theExpression.children[1].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left, right.children[1]);
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
{ if (theExpression.children.size!=3)
  { theExpression.errorString="Programming error: operation :if := takes three arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  if (left.theOperation!=theCommands.opData())
    return false;
  if (theCommands.theData[left.theData]==1)
  { theExpression.AssignChild(1);
    return true;
  }
  if (theCommands.theData[left.theData]==0)
  { theExpression.AssignChild(2);
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardFunction
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.theOperation!=theCommands.opFunction())
    return false;
  if (theExpression.children.size==0)
  { theExpression.errorString=
    "Programming error: function has no name; this should never happen. \
    Please dubug function CommandList::EvaluateStandardFunction";
    return true;
  }
  if (theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft
      (theCommands, commandIndex, theExpression, theCommands.opFunction(), theCommands.opPlus()))
    return true;
  if (theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft
      (theCommands, commandIndex, theExpression, theCommands.opFunction(), theCommands.opTimes()))
    return true;
  Expression& functionNameNode =theExpression.children[0];
  assert(theExpression.children.size==2);
  if (functionNameNode.theOperation==theCommands.opData())
  { theExpression.AssignChild(0);
    return true;
  }
  if (functionNameNode.theOperation!=theExpression.theBoss->opVariableNonBound())
    return false;
  bool (*theFun)(CommandList& , int , Expression& );
  theFun= functionNameNode.GetFunctionFromVarName();
  if (theFun==0)
    return false;
  Expression result;
  result=theExpression.children[1];
  if(theFun(theCommands, commandIndex, result))
  { theExpression=result;
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardEqualEqual
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ assert(theExpression.children.size==2);
  Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if (left==right)
    theExpression.MakeInt(1, theCommands, commandIndex);
  else
    theExpression.MakeInt(0, theCommands, commandIndex);
  return true;
}

bool CommandList::EvaluateStandardDivide
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size!=2)
    return false;
  Expression& leftE= theExpression.children[0];
  Expression& rightE= theExpression.children[1];
  if (leftE.theOperation!= theCommands.opData() || rightE.theOperation!=theCommands.opData())
    return false;
  Data& leftData=theCommands.theData[leftE.theData];
  Data& rightData=theCommands.theData[rightE.theData];
  if (leftData.type!=leftData.typeRational || rightData.type!=rightData.typeRational)
    return false;
  Data resultData=leftData/rightData;
  theExpression.MakeData
  (theCommands.theData.AddNoRepetitionOrReturnIndexFirst(resultData), theCommands, commandIndex)
  ;
  return true;
}


bool CommandList::EvaluateStandardMinus
(CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (theExpression.children.size!=1&& theExpression.children.size!=2)
  { theExpression.errorString="Programming error: operation - takes one or two arguments.";
    return false;
  }
  Expression* toBeTransformed=0;
  if (theExpression.children.size==1)
  { theExpression.AssignChild(0);
    toBeTransformed=&theExpression;
  }
  else
  { toBeTransformed=& theExpression.children[1];
    theExpression.theOperation=theCommands.opPlus();
  }
  Expression result, minusOne;
  minusOne.MakeInt(-1, theCommands, commandIndex);
  result.MakeProducT(& theCommands, commandIndex, minusOne, *toBeTransformed);
  *toBeTransformed=result;
  //std::cout << toBeTransformed->ElementToString();
  return true;
}

bool Expression::operator==
(const Expression& other)const
{ if (this->theBoss==0)
    assert(false);
  return this->theBoss->ExpressionsAreEqual(*this, other);
}

bool CommandList::ExtractPolyRational
  (PolynomialRationalCoeff& output, const Expression& theInput, HashedList<Expression>& VariableImages,
   int RecursionDepth, int MaxRecursionDepthMustNotPopStack, std::stringstream* errorLog)
{ if (RecursionDepth>MaxRecursionDepthMustNotPopStack)
  { if (errorLog!=0)
      *errorLog << "Max recursion depth of " << MaxRecursionDepthMustNotPopStack
      << " exceeded while trying to evaluate polynomial expression (i.e. your polynomial expression is too large).";
    return false;
  }
  if (RecursionDepth==0)
  { VariableImages.Clear();
    output.Nullify(0);
  }
  if (theInput.theOperation==this->opTimes() || theInput.theOperation==this->opPlus() || theInput.theOperation==this->opMinus())
  { PolynomialRationalCoeff bufferPoly;
    for (int i=0; i<theInput.children.size; i++)
    { if (!this->ExtractPolyRational(bufferPoly, theInput.children[i], VariableImages, RecursionDepth+1, MaxRecursionDepthMustNotPopStack, errorLog))
        return false;
      bufferPoly.SetNumVariablesSubDeletedVarsByOne(VariableImages.size);
      output.SetNumVariablesSubDeletedVarsByOne(VariableImages.size);
      if (theInput.theOperation==this->opTimes())
        output*=bufferPoly;
      else if (theInput.theOperation==this->opMinus())
      { if (theInput.children.size==1)
          output.Subtract(bufferPoly);
        else if (i==0)
          output+=bufferPoly;
        else
          output.Subtract(bufferPoly);
      } else if (theInput.theOperation==this->opPlus())
        output+=bufferPoly;
    }
    return true;
  }
  if (theInput.theOperation==this->opThePower())
  { assert(theInput.children.size==2);
    Expression& right= theInput.children[1];
    if (right.theOperation==this->opData())
    { int  thePower=0;
      if (this->theData[right.theData].IsSmallInteger(thePower))
      { if(!this->ExtractPolyRational(output, theInput.children[0], VariableImages, RecursionDepth+1, MaxRecursionDepthMustNotPopStack, errorLog))
          return false;
        output.RaiseToPower(thePower, 1);
        return true;
      }
    }
  }
  if (theInput.theData==this->opData())
  { Data& theData=this->theData[theInput.theData];
    if (theData.type==theData.typeRational)
    { output.MakeNVarConst(0, theData.theRational.GetElement());
      return true;
    }
  }
  std::string debugString=theInput.ElementToString();
  int theIndex=VariableImages.AddNoRepetitionOrReturnIndexFirst(theInput);
  output.MakeMonomialOneLetter(VariableImages.size, theIndex, 1, 1);
  return true;
}

bool CommandList::ExpressionsAreEqual
  (const Expression& left, const Expression& right, int RecursionDepth, int MaxRecursionDepth)
{ if (RecursionDepth>MaxRecursionDepth)
  { std::stringstream out;
    out << "Error: maximium recursion depth of " << MaxRecursionDepth << " exceeded while comparing expressions: " ;
    out << left.ElementToString() << " and " << right.ElementToString();
    std::cout << out.str();
    this->evaluationErrors.AddOnTop(out.str());
    return false;
  }
  if (!left.AreEqualExcludingChildren(right))
    return false;
  for (int i=0; i<left.children.size; i++)
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
    this->evaluationErrors.AddOnTop(out.str());
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
      indexLeft=matchedExpressions.BoundVariableIndices.size-1;
    }
    if (!(matchedExpressions.variableImages[indexLeft]==input))
      return false;
    if (printLocalDebugInfo)
      std::cout << "<br><b>Match!</b>";
    return true;
  }
  if (thePattern.theOperation!=input.theOperation || thePattern.theData!=input.theData || thePattern.children.size!=input.children.size )
    return false;
  for (int i=0; i<thePattern.children.size; i++)
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
    this->evaluationErrors.AddOnTop(out.str());
    return true;
  }
  if (theExpression.theOperation<0 || theExpression.theBoss!=this)
    return true;
  //reduction phase:
  bool NonReduced=true;
  int counter=-1;
  while (NonReduced)
  { if (this->GetElapsedTimeNonSafe!=0)
      if (this->GetElapsedTime()-this->StartTimeInSeconds >this->MaxAllowedTimeInSeconds)
      { std::cout << "<br><b>Max allowed computational time is " << this->MaxAllowedTimeInSeconds << ";  so far, "
        << this->GetElapsedTime()-this->StartTimeInSeconds  << " have elapsed -> aborting computation ungracefully.</b>";
        return true;
      }
    counter++;
    NonReduced=false;
    std::string debugString=theExpression.ElementToString();
    for (int i=0; i<theExpression.children.size; i++)
      if(! this->EvaluateExpressionReturnFalseIfExpressionIsBound
         (commandIndex, theExpression.children[i], RecursionDepth+1, maxRecursionDepth, bufferPairs, theLog))
        return false;
    if (counter>this->MaxAlgTransformationsPerExpression)
    { std::stringstream out;
      out << "Maximum number of algebraic transformations of " << this->MaxAlgTransformationsPerExpression << " exceeded.";
      theExpression.errorString=out.str();
      return true;
    }
    bool (*theFun)(CommandList& theCommands, int commandIndex, Expression& theExpression)
    =this->operations[theExpression.theOperation].GetFunction(*this);
    if (theFun!=0)
      if (theFun(*this, commandIndex, theExpression))
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
            bufferPairs.reset();
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

Expression* CommandList::PatternMatch
  (int commandIndex, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   int RecursionDepth, int MaxRecursionDepth, Expression* condition, std::stringstream* theLog, bool logAttempts)
{ if (RecursionDepth>=MaxRecursionDepthDefault)
  { std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of " << MaxRecursionDepth
    << " was exceeded";
    theExpression.errorString=out.str();
    return 0;
  }
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, RecursionDepth+1, MaxRecursionDepth, theLog))
    return 0;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>found pattern: " << theExpression.ElementToString() << " -> " << thePattern.ElementToString();
  if (condition==0)
    return &theExpression;
  Expression tempExp=*condition;
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
  if (tempExp.theOperation==this->opData())
    if (this->theData[tempExp.theData]==1)
      return & theExpression;
  return 0;
}

void CommandList::SpecializeBoundVars
(Expression& toBeSubbed, int targetCommandIndex, ExpressionPairs& matchedPairs, int RecursionDepth, int MaxRecursionDepth)
{ toBeSubbed.commandIndex=targetCommandIndex;
  if (toBeSubbed.theOperation==this->opVariableBound())
  { int indexMatching= matchedPairs.BoundVariableIndices.GetIndex(toBeSubbed.theData);
    toBeSubbed=matchedPairs.variableImages[indexMatching];
//    this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
    return;
  }
  for (int i=0; i<toBeSubbed.children.size; i++)
    this->SpecializeBoundVars(toBeSubbed.children[i], targetCommandIndex, matchedPairs, RecursionDepth+1, MaxRecursionDepth);
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
  (int commandIndex, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   int RecursionDepth, int maxRecursionDepth, std::stringstream* theLog, bool logAttempts)
{ assert(thePattern.theOperation==this->opDefine() ||
  thePattern.theOperation==this->opDefineConditional());
  assert(thePattern.children.size==2 || thePattern.children.size==3);
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
    toBeSubbed=this->PatternMatch
    (commandIndex, currentPattern, theExpression, bufferPairs, RecursionDepth+1, maxRecursionDepth, theCondition, theLog);
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
  if (this->syntaxErrors.size>0)
  { out << "<hr><b>Syntax errors encountered</b><br>";
    for (int i=0; i<this->syntaxErrors.size; i++)
      out << this->syntaxErrors[i];
    out << "<hr>";
  }
  out << "<table border=\"1\">";
  for (int i=0; i<this->theCommands.size; i++)
  { std::stringstream commentsLog;
    out  << "<tr><td>" << i+1 << ": </td><td><span class=\"math\">"
    << this->theCommands[i].finalValue.ElementToString()
    << "</span></td><td>=</td>";
    std::stringstream localLogger;
    assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    if (this->theCommands[i].ErrorString=="")
    { this->EvaluateExpressionReturnFalseIfExpressionIsBound
    (i, this->theCommands[i].finalValue, 0, this->MaxRecursionDepthDefault, thePairs, &localLogger);
      assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    }
    std::string commandOutput=this->theCommands[i].finalValue.ElementToString(0, 10000, false, false, &commentsLog);
    assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    const int MaxLatexChars=1000;
    if ((signed)commandOutput.size()<MaxLatexChars)
      out << "<td><span class=\"math\">" << commandOutput << "</span>";
    else
      out << "<td><b>output is more than " << MaxLatexChars
      << " characters, use the non- LaTeX version to the right. </b>";
    std::string tempS=commentsLog.str();
    if (tempS!="")
    { out << "<hr>In addition, the expression has the following comments." << tempS;
    }
    out << "</td>";
//    std::cout <<"<hr>" << this->theCommands[i].finalValue.ElementToStringPolishForm(0,100);

    out << "<td>=</td><td>" << commandOutput;
    out << "</td>";
//    out << " ( for debugging: " << this->theCommands[i].finalValue.ElementToStringPolishForm() << ")";
    this->theCommands[i].theLog= localLogger.str();
    if (this->evaluationErrors.size>0)
    { out << "<tr><td>Errors encountered; command evaluation terminated. Error messages follow.";
      for (int i=0; i<this->evaluationErrors.size; i++)
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
  bool makeTable=this->controlIndex==theBoss.conExpression() || this->controlIndex==theBoss.conError();
  if (makeTable)
    out <<"<table border=\"1\"><tr><td>";
  if (this->controlIndex<0)
    out << "Error: control index is not initialized! This is likely a programming error.";
  else
    out << theBoss.controlSequences[this->controlIndex];
  if (makeTable)
  { out << "</td></tr><tr><td>";
    out << this->theData.ElementToString(0, 10);
    if (this->ErrorString!="")
      out << "</td></tr><tr><td>" << this->ErrorString;
    out << "</td></tr></table>";
  }
  return out.str();
}

bool (*VariableNonBound::GetFunction(CommandList& owner)) (CommandList& theCommands, int commandIndex, Expression& theExpression)
{ if (this->HandlerFunctionIndex==-1)
    return 0;
  assert(this->HandlerFunctionIndex>=0 && this->HandlerFunctionIndex <owner.theFunctions.size);
  return owner.theFunctions[this->HandlerFunctionIndex].theFunction;

}

bool (*Expression::GetFunctionFromVarName()) (CommandList& theCommands, int commandIndex, Expression& theExpression)
{ assert(this->theBoss!=0);
  assert(this->theOperation==this->theBoss->opVariableNonBound());
  return this->theBoss->theNonBoundVars[this->theData].GetFunction(*this->theBoss);
}

std::string Expression::ElementToString(int recursionDepth, int maxRecursionDepth, bool AddBrackets, bool AddCurlyBraces, std::stringstream* outComments)const
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
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << ":=" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments);
  else if (this->theOperation==this->theBoss->opDefineConditional())
    out <<  this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments) << " :if "
    << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, true, false, outComments)
    << ":=" << this->children[2].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments);
  else if (this->theOperation==this->theBoss->opDivide() )
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication(), false, outComments)
    << "/" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForMultiplication(), false, outComments);
  else if (this->theOperation==this->theBoss->opTimes() )
  { std::string tempS=this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication(), false, outComments);
    //if (false)
   // {
    if (tempS=="-1")
      tempS="-";
    if (tempS=="1")
      tempS="";
    //} else
      //tempS="("+tempS+")";
    out << tempS;
    if (this->format==this->formatTimesDenotedByStar && tempS!="-" && tempS!="")
      out << "*"; else out << " ";
    out << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForMultiplication(), false, outComments);
  }
  else if (this->theOperation==this->theBoss->opThePower())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForThePower(), false, outComments)
    << "^{" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments) << "}";
  else if (this->theOperation==this->theBoss->opPlus() )
  { assert(this->children.size>=2);
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition(), false, outComments);
    std::string tempS=this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForAddition(), false, outComments);
    if (tempS.size()>0)
      if (tempS[0]!='-')
        out << "+";
    out << tempS;
  }
  else if (this->theOperation==this->theBoss->opMinus())
  { if ( this->children.size==1)
      out << "-" << this->children[0].ElementToString
      (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForMultiplication(), false, outComments);
    else
    { assert(children.size==2);
      out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForAddition(), false, outComments)
      << "-" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForAddition(), false, outComments);
    }
  }
  else if (this->theOperation==this->theBoss->opVariableBound())
  { assert(this->commandIndex< this->theBoss->theCommands.size);
    assert(this->theBoss->theCommands[this->commandIndex].BoundVariables.size>0);
    assert(this->theBoss->theCommands[this->commandIndex].BoundVariables.size>this->theData);
    out << "{{" << this->theBoss->theCommands[this->commandIndex].BoundVariables[this->theData] << "}}";
  }
  else if (this->theOperation==this->theBoss->opVariableNonBound())
    out << this->theBoss->theNonBoundVars[this->theData].theName;
  else if (this->theOperation==this->theBoss->opData())
    out << this->theBoss->theData[this->theData].ElementToString();
  else if (this->theOperation==this->theBoss->opFunction())
  { assert(this->children.size>=2);
    switch(this->format)
    { case Expression::formatFunctionUseUnderscore:
        out <<  this->children[0].ElementToString
        (recursionDepth+1, maxRecursionDepth, false, this->children[0].NeedBracketsForFunctionName(), outComments)
        << "_" << this->children[1].ElementToString
        (recursionDepth+1, maxRecursionDepth, false, this->children[1].NeedBracketsForFunctionName(), outComments) << "";
        break;
      case Expression::formatFunctionUseCdot:
        out <<  this->children[0].ElementToString
        (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForFunctionName(), false, outComments)
        << "\\cdot(" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments) << ")";
        break;
      default:
        out << this->children[0].ElementToString
        (recursionDepth+1, maxRecursionDepth, this->children[0].NeedBracketsForFunctionName(), false, outComments)
        << "{}(" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments) << ")";
        break;
    }
  }
  else if (this->theOperation==this->theBoss->opEqualEqual())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << "==" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments);
  else
    out << "?operation not documented? This is probably a programming error!" ;
  if (AddBrackets)
    out << ")";
  if (AddCurlyBraces)
    out << "}";
  if (outComments!=0)
    if (this->theComments!="")
      *outComments << "<br>Comments to expression " << out.str() << ":" << this->theComments;
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
  if (this->children.size>0)
    out << "<table border=\"1\"> <tr><td  align=\"center\" colspan=\"" << this->children.size << "\">";
  out << this->theBoss->operations[this->theOperation].theName << " (operation index " << this->theOperation << ")";
  out << ", " << this->theData;
  if (this->children.size>0)
  { out << "</td></tr><td>";
    for (int i=0; i<this->children.size; i++)
    { out << this->children[i].ElementToStringPolishForm(recursionDepth+1, maxRecursionDepth);
      if (i!=this->children.size-1)
        out << "</td><td> ";
    }
    out << "</td></tr>";
  }
  return out.str();
}

std::string VariableNonBound::GetHandlerFunctionName(CommandList& owner)
{ if (this->HandlerFunctionIndex==-1)
    return "";
  return owner.theFunctions[this->HandlerFunctionIndex].theName;
}

void CommandList::AddOperationNoFail
(const std::string& theOpName,
 bool (theOpHandler(CommandList& theCommands, int commandIndex, Expression& theExpression)),
 const std::string& opFunctionTechnicalName,
 const std::string& opArgumentList, const std::string& opDescription,
 const std::string& opExample
)
{ VariableNonBound theVar;
  if (theOpHandler!=0)
  { Function currentFunction(theOpHandler, opFunctionTechnicalName, opArgumentList, opDescription, opExample);
    theVar.HandlerFunctionIndex=this->theFunctions.AddNoRepetitionOrReturnIndexFirst(currentFunction);
  } else
    theVar.HandlerFunctionIndex=-1;
  theVar.theName=theOpName;
  if (this->operations.Contains(theOpName))
  { assert(false);
    return;
  }
  this->operations.AddOnTop(theVar);
}

int CommandList:: AddNonBoundVarReturnVarIndex
(const std::string& theName,
  bool (funHandler(CommandList& theCommands, int commandIndex, Expression& theExpression)),
  const std::string& argList, const std::string& description, const std::string& exampleArgs
)
{ VariableNonBound theVar;
  theVar.theName=theName;
  theVar.HandlerFunctionIndex=-1;
  int theIndex=this->theNonBoundVars.GetIndex(theVar);
  if (theIndex!=-1)
    return theIndex;
  if (funHandler!=0)
  { Function theFun(funHandler, theName, argList, description, exampleArgs);
    theVar.HandlerFunctionIndex=this->theFunctions.AddNoRepetitionOrReturnIndexFirst(theFun);
  }
  this->theNonBoundVars.AddOnTop(theVar);
  this->theDictionary.AddNoRepetition(theName);
  return this->theNonBoundVars.size-1;
}

std::string CommandList::ElementToString()
{ std::stringstream out;
  out << "Total number of pattern matches performed: " << this->TotalNumPatternMatchedPerformed;
  out << "<br>Elapsed time since evaluation was started: " << this->GetElapsedTime() - this->StartTimeInSeconds;
  out << "<br>Control sequences (" << this->controlSequences.size << " total):\n<br>\n";
  for (int i=0; i<this->controlSequences.size; i++)
  { out << this->controlSequences[i] << ", ";
  }
  out << "<br>\nOperators (" << this->operations.size << " total):\n";
  for (int i=0; i<this->operations.size; i++)
  { out << "\n" << this->operations[i].theName;
    std::string handlerName=this->operations[i].GetHandlerFunctionName(*this);
    if (this->operations[i].GetFunction(*this)!= 0)
      out << " [handled by: " << handlerName <<"]";
    if(i!=this->operations.size-1)
      out << ", ";
  }
  out << "<br>\n Handler functions (" << this->theFunctions.size << " total):\n";
  for (int i=0; i<this->theFunctions.size; i++)
  { out << "\n" << this->theFunctions[i].ElementToString();
    if (this->theFunctions[i].theFunction==0)
      out << " [empty function]";
    else
      out << "@" << std::hex << this->theFunctions[i].theFunction;
    if (i!=this->theFunctions.size-1)
      out << ", ";
  }
  out << "<br>\n Dictionary (" << this->theDictionary.size << " total):\n<br>\n";
  for (int i=0; i<this->theDictionary.size; i++)
  { out << this->theDictionary[i] << ", ";
  }
  out << "<br>\n Global variables (" << this->theNonBoundVars.size << " total):\n<br>\n";
  for (int i=0; i<this->theNonBoundVars.size; i++)
  { out << this->theNonBoundVars[i].theName << ", ";
  }
  out << "<br>\n Data entries (" << this->theData.size << " total):\n<br>\n";
  for (int i=0; i<this->theData.size; i++)
  { out << this->theData[i].ElementToString() << ", ";
  }
  out << "<hr>";
  assert(this->cachedExpressions.size==this->imagesCashedExpressions.size);
  if (this->cachedExpressions.size>0)
  { out << "<br>\n Cached expressions: (" << this->cachedExpressions.size << " total):\n<br>\n";
    for (int i=0; i<this->cachedExpressions.size; i++)
    { out << this->cachedExpressions[i].ElementToString() << " -> " << this->imagesCashedExpressions[i].ElementToString() << "<br>";
    }
  }
  for (int i=0; i<this->theCommands.size; i++)
  { out << this->theCommands[i].ElementToString();
  }
  return out.str();
}

std::string Command::ElementToStringSyntacticStack()
{ std::stringstream out;
  if (this->syntacticStack.size<this->numEmptyTokensStart)
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  if (this->theBoss==0)
    return "Error: command is not initialized properly. This is likely a programming error";
  bool HasMoreThanOneSignificantEntries=(this->syntacticStack.size>this->numEmptyTokensStart+1);
  if (HasMoreThanOneSignificantEntries)
    out << "<table border=\"1\"><tr><td>";
  for (int i=this->numEmptyTokensStart; i<this->syntacticStack.size; i++)
  { out << this->syntacticStack[i].ElementToString(*this->theBoss);
    if (HasMoreThanOneSignificantEntries)
    { out << "</td>";
      if (i!=this->syntacticStack.size-1)
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
  this->theOperation==this->theBoss->opVariableNonBound() );
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

void Expression::MakeInt(int theValue, CommandList& newBoss, int indexOfTheCommand)
{ this->reset(&newBoss, indexOfTheCommand);
  this->theData=newBoss.theData.AddNoRepetitionOrReturnIndexFirst((Data)theValue);
  this->theOperation=newBoss.opData();
}

void Expression::MakeData(int DataIndex, CommandList& newBoss, int indexOfTheCommand)
{ this->reset(&newBoss, indexOfTheCommand);
  this->theData=DataIndex;
  assert(this->theData<newBoss.theData.size);
  this->theOperation=newBoss.opData();
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
  this->children.SetSize(2);
  this->children[0].MakeVariableNonBoundNoProperties(owner, commandIndex, functionIndex);
  this->children[1]=argument;
}

void Expression::MakeFunction
  (CommandList* owner, int indexOfTheCommand, const Expression& argument, const std::string& functionName)
{ int index=owner->GetIndexNonBoundVar(functionName);
  assert(index!=-1);
  this->MakeFunction(owner, indexOfTheCommand, argument, index);
}

Data Data::operator/(const Data& right)
{ Data result;
  if (this->type!=this->typeRational || right.type!=this->typeRational)
  { std::cout << "I cannot divide expression of type " << this->type << " by expression of type " << right.type;
    assert(false);
    return result;
  }
  result=this->theRational.GetElementConst()/right.theRational.GetElementConst();
  return result;
}

int Data::HashFunction()const
{ switch (this->type)
  { case Data::typeRational:
      return this->theRational.GetElementConst().HashFunction()*this->typeRational;
    case Data::typePoly:
      return this->thePoly.GetElementConst().HashFunction()*this->typePoly;
    default:
      std::cout << "This is a programming error. Data::HashFunction() does not cover all possible cases, please debug line"
      << __LINE__;
      assert(false);
      return 0;
  }
}

bool Command::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=this->syntacticStack[this->syntacticStack.size-2];
  SyntacticElement& right=this->syntacticStack[this->syntacticStack.size-1];
  Data tempData;
  tempData=this->theBoss->theData[left.theData.theData];
  tempData*=10;
  tempData+=this->theBoss->theData[right.theData.theData];
  left.theData.theData= this->theBoss->theData.AddNoRepetitionOrReturnIndexFirst(tempData);
  this->DecreaseStackSetCharacterRanges(1);
  return true;
}

