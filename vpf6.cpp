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
{ this->reset(*owner, commandIndex);
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
  newExpr.reset(*this->theBoss, this->IndexInBoss);
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
  newExpr.reset(*this->theBoss, this->IndexInBoss);
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
  newExpr.reset(*this->theBoss, this->IndexInBoss);
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
  newExpr.reset(*this->theBoss, this->IndexInBoss);
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
  result.theData.reset(*this->theBoss, -1);
  return result;
}

void CommandList::AddEmptyHeadedCommand()
{ this->theCommands.SetSize(this->theCommands.size+1);
  Command& currentCommand=this->theCommands[this->theCommands.size-1];
  currentCommand.reset(this, this->theCommands.size-1, this->flagLogSyntaxRules);
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
          currentElement.theData.reset(*this, -1);
          this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(currentElement);
        } else if (this->isADigit(current, currentDigit))
        { Data tempData= Data(currentDigit, *this);
          currentElement.controlIndex=this->conData();
          currentElement.theData.MakeDatA(tempData, *this, this->theCommands.size-1);
          this->theCommands[this->theCommands.size-1].syntacticSoup.AddOnTop(currentElement);
        } else
        { this->theDictionary.AddNoRepetition(current);
          currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
          currentElement.theData.reset(*this, this->theCommands.size-1);
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

bool Command::ReplaceOXbyEX()
{ SyntacticElement& theElt=this->syntacticStack[this->syntacticStack.size-2];
  theElt.theData.theOperation=this->theBoss->operations.GetIndex(this->theBoss->controlSequences[theElt.controlIndex]);
  theElt.controlIndex=this->theBoss->conExpression();
  return true;
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

bool Command::LookAheadAllowsApplyFunction(const std::string& lookAhead)
{ return lookAhead!="{" && lookAhead!="_" && lookAhead!="\\circ" && lookAhead!="{}" &&  lookAhead!="$";
}

bool Command::ReplaceListXEByList()
{ SyntacticElement& left = this->syntacticStack[this->syntacticStack.size-3];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->theBoss->opList();
  newExpr.children.AddListOnTop(left.theData.children);
  newExpr.children.AddOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->theBoss->conList();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceEXEByList()
{ SyntacticElement& left = this->syntacticStack[this->syntacticStack.size-3];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->theBoss->opList();
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->theBoss->conList();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceEEByEusingO(int theControlIndex)
{ SyntacticElement& left = this->syntacticStack[this->syntacticStack.size-2];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  //    newExpr.commandIndex=
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(1);
  //    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceEXXEXEByEusingO(int theControlIndex)
{ SyntacticElement& middle = this->syntacticStack[this->syntacticStack.size-3];
  SyntacticElement& left = this->syntacticStack[this->syntacticStack.size-6];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(middle.theData);
  newExpr.children.AddOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(5);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::ReplaceEOEXByEX(int formatOptions)
{ SyntacticElement& middle=this->syntacticStack[this->syntacticStack.size-3];
  SyntacticElement& left = this->syntacticStack[this->syntacticStack.size-4];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-2];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  middle=*this->syntacticStack.LastObject();
  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  this->syntacticStack.SetSize(this->syntacticStack.size-2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Command::LookAheadAllowsTimes(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="*" || lookAhead=="/" ||
    lookAhead=="Expression" || lookAhead==")" ||
    lookAhead=="Variable" ||
    lookAhead==";" || lookAhead=="]" ||
    lookAhead=="}" || lookAhead==":"
    ;
}

bool Command::LookAheadAllowsPlus(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="," ||
    lookAhead==")" || lookAhead==";" ||
    lookAhead=="]" || lookAhead=="}" ||
    lookAhead==":"
    ;
}

bool Command::LookAheadAllowsDivide(const std::string& lookAhead)
{ return this->LookAheadAllowsTimes(lookAhead);
}

bool Command::ApplyOneRule(const std::string& lookAhead)
{ //return false;
  const SyntacticElement& lastE=this->syntacticStack[this->syntacticStack.size-1];
  const std::string& lastS=this->theBoss->controlSequences[lastE.controlIndex];
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
//  const SyntacticElement& eighthToLastE=this->syntacticStack[this->syntacticStack.size-8];
//  const std::string& eighthToLastS=this->theBoss->controlSequences[eighthToLastE.controlIndex];
//  const SyntacticElement& ninthToLastE=this->syntacticStack[this->syntacticStack.size()-9];
//  const std::string& ninthToLastS=this->theBoss->controlSequences[ninthToLastE.controlIndex];
  if (this->flagLogApplyRule)
    std::cout <<"<hr>" << this->ElementToStringSyntacticStack();
  if (secondToLastS==":" && lastS=="=")
  { this->flagOpDefineEncountered=true;
    return this->ReplaceXXByCon(this->theBoss->conDefine());
  }
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->theBoss->conApplyFunction(), Expression::formatDefault);
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->theBoss->conApplyFunction(), Expression::formatFunctionUseCdot);
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
    return this->ReplaceObyE();
  //there is an ambiguity on how should function application be associated
  //Which is better: x{}y{}z:= x{} (y{}z), or x{}y{}z:=(x{}y){}z ?
  //In our implementation, we choose x{}y{}z:= x{} (y{}z). Although this is slightly harder to implement,
  //it appears to be the more natural choice.
//  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression"
//      && this->LookAheadAllowsApplyFunction(lastS) )
//    return this->ReplaceEOEXByEX(secondToLastE.theData.format);
  if (thirdToLastS=="Expression" && secondToLastS=="{}" && lastS=="Expression"
      && this->LookAheadAllowsApplyFunction(lookAhead))
    return this->ReplaceEOEByE(secondToLastE.theData.format);
  if (thirdToLastS=="Expression" && secondToLastS=="_" && lastS=="Expression"
      && lookAhead!="_")
    return this->ReplaceEXEByEusingO(this->theBoss->conApplyFunction(), Expression::formatFunctionUseUnderscore);
  //end of ambiguity.
  if (fourthToLastS=="{"  && thirdToLastS=="{}" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXYYXByYY();
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
  if (lastS=="Expression" && secondToLastS=="/" && thirdToLastS=="Expression" && this->LookAheadAllowsDivide(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEEByEusingO(this->theBoss->conTimes());
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE();
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXEXByE(Expression::formatNoBracketsForFunctionArgument);
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (this->isStrongSeparatorFromTheRight(lookAhead) && lastS=="Expression" && secondToLastS=="==" && thirdToLastS=="Expression")
    return this->ReplaceEOEByE();
  if (fourthToLastS==" " && lookAhead==";" && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression")
    return this->ReplaceEOEByE();

  if ( thirdToLastS=="Expression" && secondToLastS=="\\cup" && lastS== "Expression" && this->isStrongSeparatorFromTheRight(lookAhead))
    return this->ReplaceEOEByE();
  if ( thirdToLastS=="Expression" && secondToLastS=="\\sqcup" && lastS== "Expression" && this->isStrongSeparatorFromTheRight(lookAhead))
    return this->ReplaceEOEByE();

  if (fourthToLastS!="[" && thirdToLastS=="Expression" && secondToLastS=="," && lastS== "Expression")
    return this->ReplaceEXEByList();
  if (thirdToLastS=="List" && secondToLastS=="," && lastS=="Expression" )
    return this->ReplaceListXEByList();
  if (thirdToLastS=="(" && secondToLastS=="List" && lastS==")")
    return this->ReplaceOXbyEX();
  if (thirdToLastS=="{" && secondToLastS=="List" && lastS=="}")
    return this->ReplaceOXbyEX();
  if (fifthToLastS=="[" && fourthToLastS=="Expression" && thirdToLastS=="," && secondToLastS=="Expression" && lastS=="]")
    return this->ReplaceXEXEXByEusingO(this->theBoss->conLieBracket());
  if (seventhToLastS==" " && lookAhead==";" && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression"
      && fourthToLastS=="if" && fifthToLastS==":" && sixthToLastS=="Expression")
    return this->ReplaceEXXEXEByEusingO(this->theBoss->conDefineConditional());
  return false;
}

bool CommandList::fPolynomial
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
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

bool CommandList::fSSAlgebra
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ std::stringstream errorStream;
  errorStream << "Error: the simple Lie algebra takes as argument of the form VariableNonBound_Data "
    <<" (in mathematical language Type_Rank). Instead I recieved " << theExpression.ElementToString() << " at file "
    << __FILE__ << " line " <<  __LINE__ << ".";
  if (theExpression.children.size!=2)
  { errorStream << " The input of the function does not have two arguments";
    return theExpression.SetError(errorStream.str());
  }
  Expression& typeE=theExpression.children[0];
  Expression& rankE=theExpression.children[1];
  if (rankE.theOperation!=theCommands.opData() || typeE.theOperation!=theCommands.opVariableNonBound())
  { errorStream << " The first node is " << typeE.ElementToString() << ". The second node is "
    << rankE.ElementToString();
    return theExpression.SetError(errorStream.str());
  }
  int theRank=1;
  if (!theCommands.theData[rankE.theData].IsSmallInteger(theRank))
  { errorStream << "The rank of a Lie algebra must be a small integer.";
    return theExpression.SetError(errorStream.str());
  }
  if (theRank<1 || theRank>8)
  { errorStream << "The rank of a simple Lie algebra must be between 1 and 8; you entered " << theRank << " instead.";
    return theExpression.SetError(errorStream.str());
  }
  std::string& theTypeName=theCommands.theNonBoundVars[typeE.theData].theName;
  if (theTypeName.size()!=1)
  { errorStream << "The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G. Instead, it is "
    << theTypeName << ".";
    return theExpression.SetError(errorStream.str());
  }
  char theWeylLetter=theTypeName[0];
  if (theWeylLetter=='a') theWeylLetter='A';
  if (theWeylLetter=='b') theWeylLetter='B';
  if (theWeylLetter=='c') theWeylLetter='C';
  if (theWeylLetter=='d') theWeylLetter='D';
  if (theWeylLetter=='e') theWeylLetter='E';
  if (theWeylLetter=='f') theWeylLetter='F';
  if (theWeylLetter=='g') theWeylLetter='G';
  if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C' || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F' || theWeylLetter=='G'))
  { errorStream << "The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G.";
    return theExpression.SetError(errorStream.str());
  }
  SemisimpleLieAlgebra theSSalgebra;
  theSSalgebra.ComputeChevalleyConstantS(theWeylLetter, theRank, theCommands.theLieAlgebras, *theCommands.theGlobalVariableS);
  Data tempData(theCommands);
  tempData.theRational.GetElement()=theCommands.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(theSSalgebra);
  tempData.type=tempData.typeSSalgebra;
  theExpression.theData=theCommands.theData.AddNoRepetitionOrReturnIndexFirst(tempData);
  theExpression.theOperation=theCommands.opData();
  theExpression.children.SetSize(0);
  return true;
}

bool CommandList::fIsInteger
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ bool isInteger=false;
  if (theExpression.theOperation==theCommands.opData())
    if (theCommands.theData[theExpression.theData].IsInteger())
      isInteger=true;
  if (isInteger)
    theExpression.theData=theCommands.theData.AddNoRepetitionOrReturnIndexFirst(Data(1, theCommands));
  else
    theExpression.theData=theCommands.theData.AddNoRepetitionOrReturnIndexFirst(Data(0, theCommands));
  theExpression.children.SetSize(0);
  theExpression.theOperation=theExpression.theBoss->opData();
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
  this->AddNonBoundVarMustBeNew("Polynomial", & this->fPolynomial, "", "Creates an internal data structure representation of a polynomial expression, implemented without use of expression trees.", "");
  this->NumPredefinedVars=this->theNonBoundVars.size;
}

void CommandList::init(GlobalVariables& inputGlobalVariables)
{ this->theGlobalVariableS=& inputGlobalVariables;
  this->MaxAlgTransformationsPerExpression=100000;
  this->MaxRecursionDepthDefault=10000;
  this->MaxAllowedTimeInSeconds=inputGlobalVariables.MaxAllowedComputationTimeInSeconds/2;
  this->flagLogSyntaxRules=false;
  this->MaxLatexChars=2000;
  this->controlSequences.Clear();
  this->operations.Clear();
  this->flagTimeLimitErrorDetected=false;
  this->flagFirstErrorEncountered=false;
  this->flagMaxTransformationsErrorEncountered=false;
  this->flagMaxRecursionErrorEncountered=false;
  this->theNonBoundVars.Clear();
  this->theData.Clear();
  this->theDictionary.Clear();
  this->theFunctions.Clear();
  this->syntaxErrors.SetSize(0);
  this->evaluationErrors.SetSize(0);
  this->targetProperties.SetSize(0);
  this->AddOperationNoFail("+",this->EvaluateStandardPlus, "Plus", "", "", "");
  this->AddOperationNoFail("-", this->EvaluateStandardMinus, "Minus", "", "", "");
  this->AddOperationNoFail("/", this->EvaluateStandardDivide, "Divide", "", "", "");
  this->AddOperationNoFail("*", this->EvaluateStandardTimes, "Times", "", "", "");
  this->AddOperationNoFail("[]", this->EvaluateStandardLieBracket, "LieBracket", "", "", "");
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
  this->AddOperationNoFail("List", 0, "List", "", "", "");
  this->AddOperationNoFail("\\cup", this->EvaluateStandardUnion, "Union", "", "", "");
  this->AddOperationNoFail("\\sqcup", this->EvaluateStandardUnionNoRepetition, "UnionNoRepetition", "", "", "");

  this->AddOperationNoFail("Error", 0, "Error", "", "", "");
  this->AddOperationNoFail("Data", 0, "Data", "", "", "");

  this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTop("{{}}");
  this->controlSequences.AddOnTop("Variable");
  for (int i=0; i<this->operations.size; i++)
    this->controlSequences.AddOnTop(this->operations[i].theName);//all operations are also control sequences
  this->controlSequences.AddOnTop("Expression");
  this->controlSequences.AddOnTop(",");
  this->controlSequences.AddOnTop("if");
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
  List<Rational> theCoeffs;
  Rational constTerm=0;
  bool foundConstTerm=false;
//  std::cout << "<b>" << theExpression.ElementToString() << "</b>";
//  if (theExpression.ElementToString()=="(4)*(a) b+(a) b")
//    std::cout << "problem!";
  for (int i=0; i<summands.size; i++)
  { Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
//    this->OrderMultiplicationTreeProperly(commandIndex, *currentSummandNoCoeff);
    Rational theCoeff=1;
    if (currentSummandNoCoeff->theOperation==this->opTimes())
    { if(currentSummandNoCoeff->children[0].theOperation==this->opData())
      { Data& curData=this->theData[currentSummandNoCoeff->children[0].theData];
        if (curData.type==curData.typeRational)
        { theCoeff=curData.theRational.GetElement();
          currentSummandNoCoeff=& currentSummandNoCoeff->children[1];
          if (theCoeff.IsEqualToZero())
            needSimplification=true;
        }
      }
    } else if (currentSummandNoCoeff->theOperation==this->opData())
    { Data& curData=this->theData[currentSummandNoCoeff->theData];
      if (curData.type==curData.typeRational)
      { if (!foundConstTerm)
          constTerm=curData.theRational.GetElement();
        else
          constTerm+=curData.theRational.GetElement();
        if (foundConstTerm || curData.IsEqualToZero())
          needSimplification=true;
        foundConstTerm=true;
        continue;
      }
    }
    int currentIndex=summandsNoCoeff.GetIndex(*currentSummandNoCoeff);
    if (currentIndex==-1)
    { summandsNoCoeff.AddOnTop(*currentSummandNoCoeff);
      theCoeffs.AddOnTop(0);
      currentIndex=summandsNoCoeff.size-1;
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
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(0);
  summandsWithCoeff.Reserve(summandsNoCoeff.size);
  for (int i=0; i<summandsNoCoeff.size; i++)
  { if (theCoeffs[i].IsEqualToZero())
      continue;
    summandsWithCoeff.SetSize(summandsWithCoeff.size+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size-1];
    if (!theCoeffs[i].IsEqualToOne())
    { current.reset(*this, commandIndex);
      current.theOperation=this->opTimes();
      current.children.SetSize(2);
      current.children[0].MakeDatA(theCoeffs[i], *this, commandIndex);
      current.children[1]=summandsNoCoeff[i];
    } else
      current=summandsNoCoeff[i];
  }
  if (!constTerm.IsEqualToZero() || summandsWithCoeff.size==0)
  { summandsWithCoeff.SetSize(summandsWithCoeff.size+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size-1];
    current.MakeDatA(constTerm, *this, commandIndex);
  }
  if (summandsWithCoeff.size==1)
  { theExpression=summandsWithCoeff[0];
    return true;
  }
  Expression* currentExp;
  currentExp=&theExpression;
  for (int i=0; i<summandsWithCoeff.size-1; i++)
  { currentExp->reset(*this, commandIndex);
    currentExp->theOperation=this->opPlus();
    currentExp->children.SetSize(2);
    currentExp->children[0]=summandsWithCoeff[i];
    currentExp=& currentExp->children[1];
  }
  *currentExp=summandsWithCoeff[summandsWithCoeff.size-1];
  return true;
}

bool CommandList::EvaluateStandardTimes
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ElementToString();
  if (theCommands.EvaluateDoDistribute
      (theCommands, commandIndex, theExpression, comments, theCommands.opTimes(), theCommands.opPlus()))
    return true;
  //std::cout << "<br>After distribute: " << theExpression.ElementToString();
  if (theCommands.EvaluateDoAssociatE
      (theCommands, commandIndex, theExpression, comments, theCommands.opTimes()))
    return true;
  if (theCommands.EvaluateDoExtractBaseMultiplication(theCommands, commandIndex, theExpression, comments))
    return true;
  if (theExpression.children.size!=2)
    return false;
  //std::cout << "<br>After do associate: " << theExpression.ElementToString();
  return false;
}

bool CommandList::EvaluateDoExtractBaseMultiplication
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=2 || theExpression.theOperation!=theCommands.opTimes())
    return false;
  bool result=false;
  Expression& leftE=theExpression.children[0];
  Expression& rightE=theExpression.children[1];
  //handle Rational*Rational:
  if (leftE.theOperation==theCommands.opData() && rightE.theOperation==theCommands.opData())
  { Data& leftD=theCommands.theData[leftE.theData];
    Data& rightD=theCommands.theData[rightE.theData];
    if (leftD.type==leftD.typeRational && rightD.type==rightD.typeRational)
    { theExpression.MakeDatA(leftD*rightD, theCommands, commandIndex);
      return true;
    }
  }
  //handle Anything*Rational:=Rational*Anything
  if (rightE.theOperation==theCommands.opData())
    if (theCommands.theData[rightE.theData].type==Data::typeRational)
    { MathRoutines::swap(leftE, rightE);
      result=true;
    }
  if (rightE.theOperation==theCommands.opTimes())
  { assert(rightE.children.size==2);
    Expression& rightLeftE=rightE.children[0];
    //handle Anything1*(Rational*Anything2):=Rational*(Anything1*Anything2)
    if (rightLeftE.theOperation==theCommands.opData())
      if (theCommands.theData[rightLeftE.theData].type==Data::typeRational)
      { MathRoutines::swap(rightLeftE, leftE);
        result=true;
      }
    //handle Rational1*(Rational2*Anything):=(Rational1*Rational2)*Anything
    if (leftE.theOperation==theCommands.opData() && rightLeftE.theOperation==theCommands.opData())
    { Data& leftD=theCommands.theData[leftE.theData];
      Data& righLefttD=theCommands.theData[rightLeftE.theData];
      if (leftD.type==leftD.typeRational && righLefttD.type==righLefttD.typeRational)
      { leftE.MakeDatA(leftD*righLefttD, theCommands, commandIndex);
        rightE.AssignChild(1);
        result=true;
      }
    }
  }
  //handle 0*Anything:=0
  if (leftE.theOperation==theCommands.opData() )
  { Data& leftD=theCommands.theData[leftE.theData];
    if (leftD.type==leftD.typeRational && leftD.IsEqualToZero())
    { theExpression.MakeInt(0, theCommands, commandIndex);
      result=true;
    }
  }
  return result;
}

bool CommandList::EvaluateDoAssociatE
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments, int theOperation)
{ List<Expression>& opands=theCommands.buffer1;
  opands.SetSize(0);
  //std::cout << "<br>At start of do associate: " << theExpression.ElementToString();
  bool needsModification=theCommands.AppendOpandsReturnTrueIfOrderNonCanonical
  (theExpression, opands, theOperation, 0, theCommands.MaxRecursionDepthDefault);
  if (!needsModification)
    return false;
  Expression* currentExpression;
  currentExpression=&theExpression;
  Expression emptyE;
  for (int i=0; i<opands.size-1; i++)
  { currentExpression->MakeXOX(& theCommands, commandIndex, theOperation, opands[i], emptyE);
    currentExpression=&currentExpression->children[1];
  }
  *currentExpression=*opands.LastObject();
  //std::cout << "<br>At end do associate: " << theExpression.ElementToString();
  return true;
}

bool CommandList::EvaluateDoDistribute
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
{ Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if ((left.theOperation==theCommands.opPlus() || left.theOperation==theCommands.opMinus()) && left.children.size==2)
  { theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft(theCommands, commandIndex, theExpression, comments, theMultiplicativeOP, theAdditiveOp);
    return true;
  }
  if ((right.theOperation==theCommands.opPlus() || right.theOperation==theCommands.opMinus()) && right.children.size==2)
  { theCommands.EvaluateDoRightDistributeBracketIsOnTheRight(theCommands, commandIndex, theExpression, comments, theMultiplicativeOP);
    return true;
  }
  return false;
}

bool CommandList::EvaluateDoLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
{ if (theExpression.theOperation!=theMultiplicativeOP)
    return false;
  if (theExpression.children[0].theOperation!=theAdditiveOp)
    return false;
  if (theExpression.children[0].children.size!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(theCommands, commandIndex);
  theExpression.theOperation=theAdditiveOp;
  theExpression.children.SetSize(2);
  theExpression.children[0].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left.children[0], right);
  theExpression.children[1].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left.children[1], right);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::EvaluateDoRightDistributeBracketIsOnTheRight
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP)
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
  theExpression.reset(theCommands, commandIndex);
  theExpression.theOperation=rightOp;
  theExpression.children.SetSize(2);
  theExpression.children[0].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left, right.children[0]);
  theExpression.children[1].MakeXOX(&theCommands, commandIndex, theMultiplicativeOP, left, right.children[1]);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::EvaluateStandardPlus
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ return theCommands.CollectSummands(commandIndex, theExpression);
}

bool CommandList::EvaluateIf
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=3)
  { theExpression.errorString="Programming error: operation :if := takes three arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  if (left.theOperation!=theCommands.opData())
    return false;
  if (theCommands.theData[left.theData].IsEqualToOne())
  { theExpression.AssignChild(1);
    return true;
  }
  if (theCommands.theData[left.theData].IsEqualToZero())
  { theExpression.AssignChild(2);
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardFunction
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.theOperation!=theCommands.opApplyFunction())
    return false;
  if (theExpression.children.size==0)
  { theExpression.errorString=
    "Programming error: function has no name; this should never happen. \
    Please dubug function CommandList::EvaluateStandardFunction";
    return true;
  }
//  if (theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft
//      (theCommands, commandIndex, theExpression, theCommands.opFunction(), theCommands.opPlus()))
//    return true;
//  if (theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft
//      (theCommands, commandIndex, theExpression, theCommands.opFunction(), theCommands.opTimes()))
//    return true;
  Expression& functionNameNode =theExpression.children[0];
  Expression& functionArgumentNode=theExpression.children[1];

  assert(theExpression.children.size==2);
  if (functionNameNode.theOperation==theCommands.opData())
  { Data tempData(theCommands);
    Data& theFunData=theCommands.theData[functionNameNode.theData];
    if (theFunData.type==Data::typeRational)
    { theExpression.AssignChild(0);
      return true;
    }
    if (functionArgumentNode.theOperation!=theCommands.opList())
    { if (functionArgumentNode.theOperation!=theCommands.opData() )
        return false;
      if (!theFunData.OperatorDereference
          (theCommands.theData[functionArgumentNode.theData], tempData, comments))
        return false;
      theExpression.MakeDatA(tempData, theCommands, commandIndex);
      return true;
    }
    if (functionArgumentNode.children.size!=2)
      return false;
    Expression& leftE=functionArgumentNode.children[0];
    Expression& rightE=functionArgumentNode.children[1];
    if (leftE.theOperation!=theCommands.opData() || rightE.theOperation!=theCommands.opData())
      return false;
    Data& leftD=theCommands.theData[leftE.theData];
    Data& rightD=theCommands.theData[rightE.theData];
    if (!theFunData.OperatorDereference(leftD, rightD, tempData, comments))
      return false;
    theExpression.MakeDatA(tempData, theCommands, commandIndex);
    return true;
  }
  if (functionNameNode.theOperation!=theExpression.theBoss->opVariableNonBound())
    return false;
  Function::FunctionAddress theFun;
  theFun= functionNameNode.GetFunctionFromVarName();
  if (theFun==0)
    return false;
  Expression result;
  result=theExpression.children[1];
  if(theFun(theCommands, commandIndex, result, comments))
  { theExpression=result;
    return true;
  }
  return false;
}

bool CommandList::EvaluateStandardEqualEqual
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ assert(theExpression.children.size==2);
  Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if (left==right)
    theExpression.MakeInt(1, theCommands, commandIndex);
  else
    theExpression.MakeInt(0, theCommands, commandIndex);
  return true;
}

bool CommandList::EvaluateStandardLieBracket
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=2)
    return false;
  assert(theExpression.theOperation=theCommands.opLieBracket());
  Expression& leftE=theExpression.children[0];
  Expression& rightE=theExpression.children[1];
  if (leftE.theOperation!=theCommands.opData() || rightE.theOperation!=theCommands.opData())
    return false;
  Data& leftD=theCommands.theData[leftE.theData];
  Data& rightD=theCommands.theData[rightE.theData];
  Data newData(theCommands);
  if (!Data::LieBracket(leftD, rightD, newData, comments))
    return false;
  theExpression.MakeDatA(newData, theCommands, commandIndex);
  return true;
}

bool Expression::IsRationalNumber()
{ if (this->theOperation!=this->theBoss->opData())
    return false;
  if (this->theBoss->theData[this->theData].type==Data::typeRational)
    return true;
  return false;
}

bool CommandList::EvaluateStandardUnion
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size==1)
  { theExpression.AssignChild(0);
    return true;
  }
  if (theExpression.children.size==0)
    return false;
  Expression resultExpression;
  resultExpression.reset(theCommands, commandIndex);
  resultExpression.theOperation=theCommands.opList();
  for (int i=0; i<theExpression.children.size; i++)
  { Expression& currentExp=theExpression.children[i];
    if (currentExp.theOperation==theCommands.opList())
      resultExpression.children.AddListOnTop(currentExp.children);
    else
      resultExpression.children.AddOnTop(currentExp);
  }
  theExpression=resultExpression;
  return true;
}

bool CommandList::EvaluateStandardUnionNoRepetition
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ return false;
}

bool CommandList::EvaluateStandardDivide
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=2)
    return false;
  Expression& leftE= theExpression.children[0];
  Expression& rightE= theExpression.children[1];
  if (rightE.IsRationalNumber() && !leftE.IsRationalNumber())
  { Data tempData(1, theCommands);
    tempData/=theCommands.theData[rightE.theData];
    rightE=leftE;
    leftE.MakeDatA(tempData, theCommands, commandIndex);
    return true;
  }
  if (leftE.theOperation!= theCommands.opData() || rightE.theOperation!=theCommands.opData())
    return false;
  Data& leftData=theCommands.theData[leftE.theData];
  Data& rightData=theCommands.theData[rightE.theData];
  if (leftData.type!=leftData.typeRational || rightData.type!=rightData.typeRational)
    return false;
  Data resultData=leftData/rightData;
  theExpression.MakeDatA(resultData, theCommands, commandIndex);
  return true;
}

bool CommandList::EvaluateStandardMinus
(CommandList& theCommands, int commandIndex, Expression& theExpression, std::stringstream* comments)
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
    if (this->flagMaxRecursionErrorEncountered)
      this->evaluationErrors.AddOnTop(out.str());
    this->flagMaxRecursionErrorEncountered=true;
    return true;
  }
  if (theExpression.errorString!="")
    return true;
  std::stringstream comments;
  if (theExpression.theOperation<0 || theExpression.theBoss!=this)
    return true;
  Command& currentCommand=this->theCommands[commandIndex];
  if (currentCommand.ExpressionStack.Contains(theExpression))
  { std::stringstream errorStream;
    errorStream << "I think I have detected a cycle: " << theExpression.ElementToString()
    << " is transformed to an expression that contains the starting expression. ";
    theExpression.SetError(errorStream.str());
    return true;
  }
  static int errorCounter=0;
  errorCounter++;
  std::string debugString;
  debugString=theExpression.ElementToString();
  currentCommand.ExpressionStack.AddOnTop(theExpression);
  HashedList<Expression> currentExpressionTransformations;
//  currentExpressionTransformations.AddOnTop(theExpression);

  bool NonReduced=true;
  int counter=-1;
  int indexInCache=currentCommand.cachedExpressions.GetIndex(theExpression);
  if (indexInCache!=-1)
  { theExpression=currentCommand.imagesCachedExpressions[indexInCache];
  } else
  { currentCommand.cachedExpressions.AddOnTop(theExpression);
    currentCommand.imagesCachedExpressions.AddOnTop(theExpression);
    indexInCache=currentCommand.cachedExpressions.size-1;
  }
  //reduction phase:
  while (NonReduced)
  { if (this->theGlobalVariableS->GetElapsedSeconds()!=0)
      if (this->theGlobalVariableS->GetElapsedSeconds()-this->StartTimeInSeconds >this->MaxAllowedTimeInSeconds)
      { if (!this->flagTimeLimitErrorDetected)
          std::cout << "<br><b>Max allowed computational time is " << this->MaxAllowedTimeInSeconds << ";  so far, "
          << this->theGlobalVariableS->GetElapsedSeconds()-this->StartTimeInSeconds  << " have elapsed -> aborting computation ungracefully.</b>";
        this->flagTimeLimitErrorDetected=true;
        currentCommand.ExpressionStack.PopIndexSwapWithLast(currentCommand.ExpressionStack.size-1);
        return true;
      }
    counter++;
    NonReduced=false;
    std::string debugString=theExpression.ElementToString();
    for (int i=0; i<theExpression.children.size; i++)
      if(! this->EvaluateExpressionReturnFalseIfExpressionIsBound
         (commandIndex, theExpression.children[i], RecursionDepth+1, maxRecursionDepth, bufferPairs, theLog))
      { currentCommand.ExpressionStack.PopIndexSwapWithLast(currentCommand.ExpressionStack.size-1);
        return false;
      }
      else
        if (theExpression.children[i].errorString!="")
        { currentCommand.ExpressionStack.PopIndexSwapWithLast(currentCommand.ExpressionStack.size-1);
          std::stringstream errorStream;
          errorStream << "Error: child " << i+1 << " contains an error.";
          theExpression.SetError(errorStream.str());
          return true;
        }
    if (counter>this->MaxAlgTransformationsPerExpression)
    { if (!this->flagMaxTransformationsErrorEncountered)
      { std::stringstream out;
        out << "<br>Maximum number of algebraic transformations of " << this->MaxAlgTransformationsPerExpression << " exceeded."
        << " while simplifying " << theExpression.ElementToString();
        theExpression.errorString=out.str();
      }
      currentCommand.ExpressionStack.PopIndexSwapWithLast(currentCommand.ExpressionStack.size-1);
      return true;
    }
    Function::FunctionAddress theFun=this->operations[theExpression.theOperation].GetFunction(*this);
    if (theFun!=0)
      if (theFun(*this, commandIndex, theExpression, &comments))
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
    currentCommand.imagesCachedExpressions[indexInCache]=theExpression;
    if (NonReduced)
    { if (currentExpressionTransformations.Contains(theExpression))
      { std::stringstream errorStream;
        errorStream << "I think I detected a substitution cycle, " << theExpression.ElementToString()
        << " appears twice in the reduction cycle";
        theExpression.SetError(errorStream.str());
        break;
      } else
        currentExpressionTransformations.AddOnTop(theExpression);
    }
//    if (NonReduced)
//      theExpression.ElementToString();
  }
  if (currentCommand.ExpressionStack.size==0)
  { debugString=theExpression.ElementToString();
  }
  currentCommand.ExpressionStack.PopIndexSwapWithLast(currentCommand.ExpressionStack.size-1);

  theExpression.theComments=comments.str();
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
    if (this->theData[tempExp.theData].IsEqualToOne())
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

void CommandList::ExtractExpressions()
{ std::string lookAheadToken;
  std::stringstream errorLog;
  for (int j=0; j<this->theCommands.size; j++)
  { this->theCommands[j].resetStack();
    for (int i=0; i<this->theCommands[j].syntacticSoup.size; i++)
    { if (i+1<this->theCommands[j].syntacticSoup.size)
        lookAheadToken=this->controlSequences[this->theCommands[j].syntacticSoup[i+1].controlIndex];
      else
        lookAheadToken=";";
      this->theCommands[j].syntacticStack.AddOnTop(this->theCommands[j].syntacticSoup[i]);
      while(this->theCommands[j].ApplyOneRule(lookAheadToken))
      {}
    }
    if (this->theCommands[j].ErrorString=="" && this->theCommands[j].syntacticStack.size==this->theCommands[j].numEmptyTokensStart+1)
    { SyntacticElement& result=this->theCommands[j].syntacticStack[this->theCommands[j].numEmptyTokensStart];
      this->theCommands[j].finalValue=result.theData;
      if (result.controlIndex!=this->conExpression())
      { errorLog << "Syntax error at command " << j+1
        << ": your command simplifies to a single syntactic element but it is not an expression. <br>";
        errorLog << "It simplifies to:<br> " << this->theCommands[j].ElementToStringSyntacticStack();
      }
    }
    else if (this->theCommands[j].ErrorString!="")
      errorLog << "Syntax error at command " << j+1 << ":" << theCommands[j].ErrorString << "<br>";
    else if (this->theCommands[j].syntacticStack.size!=this->theCommands[j].numEmptyTokensStart)
    { errorLog << "Syntax error at command " << j+1 << ": your command does not simplify to a syntactic element. <br>";
      errorLog << "Instead it simplifies to:<br> " << this->theCommands[j].ElementToStringSyntacticStack();
    }
  }
  std::string error = errorLog.str();
  if (error!="")
    this->syntaxErrors.AddOnTop(error);
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
    out << "<td>";
    std::string commandOutput=this->theCommands[i].finalValue.ElementToString(0, 10000, false, false, &commentsLog);
    assert((int)this->theCommands[0].syntacticStack[6].theData.theBoss!=-1);
    if ((signed)commandOutput.size()<this->MaxLatexChars)
      out << "<span class=\"math\">" << commandOutput << "</span>";
    else
      out << "<b>output is more than " << this->MaxLatexChars
      << " characters, use the non- LaTeX version to the right. </b>";
    std::string tempS=commentsLog.str();
    if (tempS!="")
      out << "<hr>Additional comments: " << CGI::GetHtmlSpanHidableStartsHidden(tempS);
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
  bool makeTable=this->controlIndex==theBoss.conExpression() || this->controlIndex==theBoss.conError()
  || this->controlIndex==theBoss.conList() ;
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

Function::FunctionAddress VariableNonBound::GetFunction(CommandList& owner)
{ if (this->HandlerFunctionIndex==-1)
    return 0;
  assert(this->HandlerFunctionIndex>=0 && this->HandlerFunctionIndex <owner.theFunctions.size);
  return owner.theFunctions[this->HandlerFunctionIndex].theFunction;
}

Function::FunctionAddress Expression::GetFunctionFromVarName()
{ assert(this->theBoss!=0);
  assert(this->theOperation==this->theBoss->opVariableNonBound());
  return this->theBoss->theNonBoundVars[this->theData].GetFunction(*this->theBoss);
}

std::string Expression::ElementToString(int recursionDepth, int maxRecursionDepth, bool AddBrackets, bool AddCurlyBraces, std::stringstream* outComments)const
{ if (maxRecursionDepth>0)
    if(recursionDepth>maxRecursionDepth)
      return "(...)";
  if (this->theBoss==0)
    return "(ProgrammingErrorNoBoss)";
  assert((int)(this->theBoss)!=-1);
  std::stringstream out;
  std::string additionalDataComments;
  if (this->errorString!="")
  { if (outComments!=0)
      *outComments << this->errorString << " ";
    return "(Error:SeeComments)";
  }
  if (this->theOperation<0)
  { out << "(ErrorNoOp)";
    return out.str();
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
  { std::stringstream dataComments;
    out << this->theBoss->theData[this->theData].ElementToString(&dataComments);
    additionalDataComments=dataComments.str();
  }
  else if (this->theOperation==this->theBoss->opApplyFunction())
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
        << "{{}" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, this->children[1].NeedBracketsForFunctionArgument(), false, outComments) << "}";
        break;
    }
  }
  else if (this->theOperation==this->theBoss->opEqualEqual())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << "==" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments);
  else if (this->theOperation==this->theBoss->opList())
  { out << "(";
    for (int i=0; i<this->children.size; i++)
    { out << this->children[i].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments);
      if (i!=this->children.size-1)
        out << ",";
    }
    out << ")";
  } else if (this->theOperation==this->theBoss->opLieBracket())
    out << "[" << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << "," << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << "]";
  else if (this->theOperation==this->theBoss->opUnion())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << "\\cup" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    ;
  else if (this->theOperation==this->theBoss->opUnionNoRepetition())
    out << this->children[0].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    << "\\sqcup" << this->children[1].ElementToString(recursionDepth+1, maxRecursionDepth, false, false, outComments)
    ;
  else
    out << "(ProgrammingError:NotDocumented)" ;
  if (AddBrackets)
    out << ")";
  if (AddCurlyBraces)
    out << "}";
  if (this->errorString!="")
    out << ")";
  if (outComments!=0)
  { if (this->theComments!="" || additionalDataComments!="")
      *outComments << "<br>Comments to expression " << out.str() << ":<br>";
    if (this->theComments!="")
      *outComments << this->theComments << "<br>";
    if (additionalDataComments!="")
      *outComments << additionalDataComments;
  }
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
   const Function::FunctionAddress& theFunAddress,
   const std::string& opFunctionTechnicalName,
   const std::string& opArgumentList, const std::string& opDescription,
   const std::string& opExample
   )
{ VariableNonBound theVar;
  if (theFunAddress!=0)
  { Function currentFunction(theFunAddress, opFunctionTechnicalName, opArgumentList, opDescription, opExample);
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
(const std::string& theName, const Function::FunctionAddress& funHandler,
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
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  std::string openTag3="<span style=\"color:#00FF00\">";
  std::string closeTag3="</span>";
  out << " Total number of pattern matches performed: " << this->TotalNumPatternMatchedPerformed << "";
  double elapsedSecs=this->theGlobalVariableS->GetElapsedSeconds() - this->StartTimeInSeconds;
  out << "<br>Elapsed time since evaluation was started: "
  << elapsedSecs << " seconds (" << elapsedSecs*1000 << " milliseconds).";
  out << "<br>Control sequences (" << this->controlSequences.size << " total):\n<br>\n";
  for (int i=0; i<this->controlSequences.size; i++)
  { out << openTag1 << this->controlSequences[i] << closeTag1;
    if (i!=this->controlSequences.size)
      out << ", ";
  }
  out << "<br>\nOperators (" << this->operations.size << " total):<br>\n";
  for (int i=0; i<this->operations.size; i++)
  { out << "\n" << openTag1 << this->operations[i].theName << closeTag1;
    std::string handlerName=this->operations[i].GetHandlerFunctionName(*this);
    if (this->operations[i].GetFunction(*this)!= 0)
      out << " [handled by: " << openTag2 << handlerName << closeTag2 << "]";
    if(i!=this->operations.size-1)
      out << ", ";
  }
  out << "<br>\n Handler functions (" << this->theFunctions.size << " total):<br>\n";
  for (int i=0; i<this->theFunctions.size; i++)
  { out << "\n" << openTag2 << this->theFunctions[i].ElementToString() << closeTag2;
    if (this->theFunctions[i].theFunction==0)
      out << " [empty function]";
//    else
//      out << "@" << std::hex << this->theFunctions[i].theFunction;
    if (i!=this->theFunctions.size-1)
      out << ", ";
  }
  out << "<br>\n Dictionary (" << this->theDictionary.size << " total):\n<br>\n";
  for (int i=0; i<this->theDictionary.size; i++)
  { out << openTag1 << this->theDictionary[i] << closeTag1;
    if (i!=this->theDictionary.size-1)
      out << ", ";
  }
  out << "<br>\n" << this->theNonBoundVars.size << " global variables " << " (= "
  << this->NumPredefinedVars  << " predefined + " << this->theNonBoundVars.size-this->NumPredefinedVars
  << " user-defined). <br>Predefined: \n<br>\n";
  for (int i=0; i<this->theNonBoundVars.size; i++)
  { out << openTag1 << this->theNonBoundVars[i].theName << closeTag1;
    std::string handlerName=this->theNonBoundVars[i].GetHandlerFunctionName(*this);
    if (handlerName!="")
      out << " [handled by: " << openTag2 << handlerName << closeTag2 << "]";
    if (i!=this->theNonBoundVars.size-1)
    { out << ", ";
      if (i==this->NumPredefinedVars-1 )
        out << "<br>user-defined:\n<br>\n";
    }
  }
  out << "<br>\nData entries (" << this->theData.size << " total):\n<br>\n";
  for (int i=0; i<this->theData.size; i++)
  { out << openTag3 << this->theData[i].ElementToString() << closeTag3;
    if (i!=this->theData.size-1)
      out  << ", ";
  }
  out << "<hr>";
  for (int i=0; i<this->theCommands.size; i++)
  { out << this->theCommands[i].ElementToString();
  }
  return out.str();
}

std::string Command::ElementToString(bool usePolishForm)
{ std::stringstream out;
  out << "Bound variables:<br>\n ";
  for (int i=0; i<this->BoundVariables.size; i++)
  { out << this->BoundVariables[i];
    if (i!=this->BoundVariables.size-1)
      out << ", ";
  }
  if (this->theBoss==0)
  { out << "Element not initialized.";
    return out.str();
  }
  out << "<br>\nExpression stack no values (excluding empty tokens in the start): ";
  for (int i=this->numEmptyTokensStart; i<this->syntacticStack.size; i++)
  { out << this->syntacticStack[i].ElementToString(*this->theBoss);
    if (i!=this->syntacticStack.size-1)
      out << ", ";
  }
  out << "<hr>\nSyntactic soup:";
  for (int i=0; i<this->syntacticSoup.size; i++)
  { out << this->syntacticSoup[i].ElementToString(*this->theBoss);
    if (i!=this->syntacticSoup.size-1)
      out << ", ";
  }
  out << "<br>\nExpression stack(excluding empty tokens in the start): ";
  out << this->ElementToStringSyntacticStack();
  out << "<br>\n Current value: " << this->finalValue.ElementToString(0, 5, false);
  if (usePolishForm)
    out << "<br>=(in polish form):" << this->finalValue.ElementToStringPolishForm(0, 20);
  assert(this->cachedExpressions.size==this->imagesCachedExpressions.size);
  if (this->cachedExpressions.size>0)
  { out << "<br>\n Cached expressions: (" << this->cachedExpressions.size << " total):\n<br>\n";
    for (int i=0; i<this->cachedExpressions.size; i++)
      out << this->cachedExpressions[i].ElementToString() << " -> " << this->imagesCachedExpressions[i].ElementToString() << "<br>";
  }
  return out.str();
}

std::string Command::ElementToStringSyntacticStack()
{ std::stringstream out;
  if (this->syntacticStack.size<this->numEmptyTokensStart)
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  if (this->theBoss==0)
    return "Error: command is not initialized properly. This is likely a programming error";
  bool HasMoreThanOneSignificantEntriesOrIsNotExpression=(this->syntacticStack.size>this->numEmptyTokensStart+1);
  SyntacticElement& lastSyntacticElt=* this->syntacticStack.LastObject();
  if (this->syntacticStack.size==this->numEmptyTokensStart+1)
    if (lastSyntacticElt.controlIndex!=this->theBoss->conExpression())
      HasMoreThanOneSignificantEntriesOrIsNotExpression=true;
  if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    out << "<table border=\"1\"><tr><td>";
  for (int i=this->numEmptyTokensStart; i<this->syntacticStack.size; i++)
  { out << this->syntacticStack[i].ElementToString(*this->theBoss);
    if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    { out << "</td>";
      if (i!=this->syntacticStack.size-1)
        out << "<td>";
    }
  }
  if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    out << "</td></tr></table>";
  return out.str();
}

Rational Expression::GetConstantTerm()const
{ if (this->theOperation==this->theBoss->opData())
  { Data& curData=this->theBoss->theData[this->theData];
    if (curData.type==curData.typeRational)
      return curData.theRational.GetElement();
  }
  if (this->theOperation==this->theBoss->opTimes())
    if (this->children[0].theOperation==this->theBoss->opData())
    { Data& curData=this->theBoss->theData[this->children[0].theData];
      if (curData.type==curData.typeRational)
        return curData.theRational.GetElement();
    }
  return 1;
}

bool Expression::NeedBracketsForFunctionName() const
{ return !(
  this->theOperation==this->theBoss->opVariableBound() ||
  this->theOperation==this->theBoss->opVariableNonBound()
  || ( this->theOperation==this->theBoss->opApplyFunction() && this->format==this->formatFunctionUseUnderscore)
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

bool Expression::NeedBracketsForFunctionArgument()const
{ return this->format!=formatNoBracketsForFunctionArgument;
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
{ this->reset(newBoss, indexOfTheCommand);
  this->theData=newBoss.theData.AddNoRepetitionOrReturnIndexFirst(Data(theValue, newBoss));
  this->theOperation=newBoss.opData();
}

void Expression::MakeDatA(const Rational& inputData, CommandList& newBoss, int indexOfTheCommand)
{ this->MakeDatA(Data(inputData, newBoss), newBoss, indexOfTheCommand);
}

void Expression::MakeDatA(const Data& inputData, CommandList& newBoss, int indexOfTheCommand)
{ this->reset(newBoss, indexOfTheCommand);
  this->theData=newBoss.theData.AddNoRepetitionOrReturnIndexFirst(inputData);
  assert(this->theData<newBoss.theData.size);
  this->theOperation=newBoss.opData();
}

void Expression::MakeVariableNonBoundNoProperties
  (CommandList& owner, int indexOfTheCommand, int varIndex)
{ this->reset(owner, indexOfTheCommand);
  this->theData=varIndex;
  this->theOperation=owner.opVariableNonBound();
}

void Expression::MakeFunction
  (CommandList& owner, int indexOfTheCommand, const Expression& argument, int functionIndex)
{ this->reset(owner, indexOfTheCommand);
  this->theOperation=owner.opApplyFunction();
  this->children.SetSize(2);
  this->children[0].MakeVariableNonBoundNoProperties(owner, commandIndex, functionIndex);
  this->children[1]=argument;
}

void Expression::MakeFunction
  (CommandList& owner, int indexOfTheCommand, const Expression& argument, const std::string& functionName)
{ int index=owner.GetIndexNonBoundVar(functionName);
  assert(index!=-1);
  this->MakeFunction(owner, indexOfTheCommand, argument, index);
}

Data Data::operator/(const Data& right)
{ Data result(*this);
  if (this->type!=this->typeRational || right.type!=this->typeRational)
  { std::cout << "I cannot divide expression of type " << this->type << " by expression of type " << right.type;
    assert(false);
    return result;
  }
  if (right.theRational.GetElementConst().IsEqualToZero())
    result.SetError("Error: division by zero");
  else
    result=Data(this->theRational.GetElementConst()/right.theRational.GetElementConst(), *this->owner);
  return result;
}

Data Data::operator*(const Data& right)
{ Data result(*this);
  if (this->type!=this->typeRational || right.type!=this->typeRational)
  { std::cout << "I cannot divide expression of type " << this->type << " by expression of type " << right.type;
    assert(false);
    return result;
  }
  result=Data(this->theRational.GetElementConst()*right.theRational.GetElementConst(), *this->owner);
  return result;
}

bool Data::operator+=(const Data& other)
{ std::stringstream out;
  if (this->type!=other.type)
  { out << "Adding different types, " << this->ElementToStringDataType() << " and "
    << other.ElementToStringDataType() << ", is not allowed.";
    return this->SetError(out.str());
  }
  switch(this->type)
  { case Data::typeRational:
      this->theRational.GetElement()+=other.theRational.GetElementConst();
      return true;
    default:
      out << "Don't know how to add elements of type " << this->ElementToStringDataType() << ". ";
      return this->SetError(out.str());
  }
}

bool Data::operator==(const Data& other)
{ if(this->owner!=other.owner)
    return false;
  if (this->type!=other.type)
    return false;
  switch(this->type)
  { case Data::typeSSalgebra:
    case Data::typeElementSSalgebra:
    case Data::typeRational:
      return this->theRational.GetElement()==other.theRational.GetElementConst();
    case Data::typePoly:
      return this->thePoly.GetElement()==other.thePoly.GetElementConst();
    case Data::typeError:
      return this->theError.GetElement()==other.theError.GetElementConst();
    default:
      std::cout << "This is a programming error: operator== does not cover type "
      << this->ElementToStringDataType()
      << " Please debug file " << __FILE__ << " line " << __LINE__ << ".";
      assert(false);
      return false;
  }
}

int Data::HashFunction()const
{ switch (this->type)
  { case Data::typeSSalgebra:
    case Data::typeElementSSalgebra:
    case Data::typeRational:
      return this->theRational.GetElementConst().HashFunction()*this->type;
    case Data::typePoly:
      return this->thePoly.GetElementConst().HashFunction()*this->type;
    case Data::typeError:
      return MathRoutines::hashString(this->theError.GetElementConst());
    default:
      std::cout << "This is a programming error. Data::HashFunction() does not cover type "
      << this->type << ", please debug line"
      << __LINE__;
      assert(false);
      return 0;
  }
}

bool Command::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=this->syntacticStack[this->syntacticStack.size-2];
  SyntacticElement& right=this->syntacticStack[this->syntacticStack.size-1];
  Data tempData(this->theBoss->theData[left.theData.theData]);
  tempData*=10;
  tempData+=this->theBoss->theData[right.theData.theData];
  left.theData.MakeDatA(tempData, *this->theBoss, this->IndexInBoss);
  this->DecreaseStackSetCharacterRanges(1);
  return true;
}

void Data::operator=(const Data& other)
{ if (this==&other)
    return;
  this->type=other.type;
  this->owner=other.owner;
  switch(this->type)
  { case Data::typeElementSSalgebra:
    case Data::typeSSalgebra:
    case Data::typeRational: this->theRational=other.theRational; break;
    case Data::typePoly: this->thePoly=other.thePoly; break;
    case Data::typeError: this->theError=other.theError; break;
    default:
      std::cout << "This is a programming error: operator= does not cover type "
      << this->ElementToStringDataType()
      << ". Please debug file " << __FILE__ << " line " << __LINE__ << ".";
      assert(false);
  }
}

std::string Data::ElementToStringDataType() const
{ switch(this->type)
  { case Data::typeElementSSalgebra: return "ElementSSalgebra";
    case Data::typeSSalgebra: return "SemisimpleLieAlgebra";
    case Data::typeRational:  return "Rational";
    case Data::typeError:  return "Error";
    default:
      std::cout << "This is a programming error: Data::ElementToStringDataType does not cover all cases. Please "
      << " debug file " << __FILE__ << " line " << __LINE__ << ".";
      assert(false);
      return "unknown";
  }
}

std::string Data::ElementToString(std::stringstream* comments)const
{ std::stringstream out;
  if (this->owner==0)
    return "(ProgrammingError:NoOwner)";
  switch(this->type)
  { case Data::typeSSalgebra:
      out << "SemisimpleLieAlgebra{}("
      << this->owner->theLieAlgebras[this->theRational.GetElementConst().NumShort].GetLieAlgebraName(false) << ")";
      if (comments!=0)
        *comments << "Comments to data " << out.str() << ":<br>"
        << this->owner->theLieAlgebras[this->theRational.GetElementConst().NumShort].
        ElementToString(*this->owner->theGlobalVariableS);
      return out.str();
    case Data::typeElementSSalgebra:
      out << this->owner->theLieAlgebraElements[this->theRational.GetElementConst().NumShort].ElementToString();
      return out.str();
    case Data::typeRational:
      return this->theRational.GetElementConst().ElementToString();
    case Data::typePoly:
      return this->thePoly.GetElementConst().ElementToString();
    case Data::typeError:
      out << "(Error)";
      if (comments!=0)
        *comments << this->theError.GetElementConst();
      return out.str();
    default:
      std::cout << "This is a programming error: don't know how to convert element of type " << this->type
      << " (type " << this->ElementToStringDataType() << ") to string. "
      << "Please debug file " << __FILE__ << " line " << __LINE__;
      assert(false);
      return out.str();
  }
}

bool Command::ReplaceXEXEXByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& lefT = this->syntacticStack[this->syntacticStack.size-4];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-2];
  SyntacticElement& result = this->syntacticStack[this->syntacticStack.size-5];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  newExpr.children.AddOnTop(lefT.theData);
  newExpr.children.AddOnTop(right.theData);
  newExpr.format=formatOptions;
  result.theData=newExpr;
  result.controlIndex=this->theBoss->conExpression();
  this->DecreaseStackSetCharacterRanges(4);
  return true;
}

bool Command::ReplaceEXEByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& left = this->syntacticStack[this->syntacticStack.size-3];
  SyntacticElement& right = this->syntacticStack[this->syntacticStack.size-1];
  Expression newExpr;
  newExpr.reset(*this->theBoss, this->IndexInBoss);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << this->syntacticStack[this->syntacticStack.size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Data::MakeElementSemisimpleLieAlgebra
(CommandList& owner, SemisimpleLieAlgebra& ownerAlgebra, int index1, int index2, std::stringstream* comments)
{ bool isGood=(index1==0 && index2 <=ownerAlgebra.GetRank() && index2>0);
  if (!isGood)
  { if (comments!=0)
      *comments
       << "You requested element of the Cartan subalgebra labeled by (" << index1 << ", " << index2
      << " of semisimple Lie algebra " << ownerAlgebra.GetLieAlgebraName()
      << "). For your request to succeed, the first index must be zero and the second must be an integer"
      << " between 1 and the rank of the Algebra which is " << ownerAlgebra.GetRank()
      << ". If you want to request an element that is in a root space outside of the Cartan, you should only one index."
      ;
    return false;
  }
  ElementSimpleLieAlgebra tempElt;
  int actualIndeX=index2-1+ownerAlgebra.GetNumPosRoots();
  tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (actualIndeX, ownerAlgebra);
  this->owner=&owner;
  this->type=this->typeElementSSalgebra;
  this->theRational.GetElement()=owner.theLieAlgebraElements.AddNoRepetitionOrReturnIndexFirst(tempElt);
  return true;
}

bool Data::MakeElementSemisimpleLieAlgebra
(CommandList& inputOwner, SemisimpleLieAlgebra& ownerAlgebra, int theDisplayIndex, std::stringstream* comments)
{ bool isGood=(theDisplayIndex>0 && theDisplayIndex<= ownerAlgebra.GetNumPosRoots()) ||
  (theDisplayIndex<0 && theDisplayIndex>=- ownerAlgebra.GetNumPosRoots());
  if (!isGood)
  { if (comments!=0)
      *comments
       << "<b>Error</b>. You requested element of index " << theDisplayIndex
      << " of semisimple Lie algebra " << ownerAlgebra.GetLieAlgebraName()
      << ". The index of the root space must be a non-zero integer "
      << " of absolute value between 1 and the number of positive roots. "
      << "The number of positive roots for the current semisimple Lie algebra is "
      << ownerAlgebra.GetNumPosRoots()
      << ". If you want to request an element of the Cartan, you should use two indices, the first of which is zero. For example,"
      << " ElementSemisimpleAlgebra{}(0,1) gives you the an element of the Cartan corresponding to the first simple root. "
      ;
    return false;
  }
  ElementSimpleLieAlgebra tempElt;
  int actualIndex=theDisplayIndex;
  if (actualIndex<0)
    actualIndex+=ownerAlgebra.GetNumPosRoots();
  else
    actualIndex+=ownerAlgebra.GetNumPosRoots()+ownerAlgebra.GetRank()-1;
  tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (actualIndex, ownerAlgebra);
  this->owner=&inputOwner;
  this->type=this->typeElementSSalgebra;
  this->theRational.GetElement()=inputOwner.theLieAlgebraElements.AddNoRepetitionOrReturnIndexFirst(tempElt);
  return true;
}

SemisimpleLieAlgebra& ElementSimpleLieAlgebra::GetOwner()
{ if (this->ownerArray==0 || this->indexOfOwnerAlgebra==-1)
  { std::cout << "This is a programming error: a semisimple Lie algebra element has not been initialized properly. "
    << "Please debug file " << __FILE__ << " line " << __LINE__ << ". ";
    assert(false);
  }
  if ( this->indexOfOwnerAlgebra<0 || this->ownerArray->size<=this->indexOfOwnerAlgebra)
  { std::cout << "This is a programming error: a semisimple Lie algebra container has not been initialized properly. "
    << "Please debug file " << __FILE__ << " line " << __LINE__ << ". ";
    assert(false);
  }
  return this->ownerArray->TheObjects[this->indexOfOwnerAlgebra];
}

bool Data::LieBracket
  (Data& left, Data& right, Data& output, std::stringstream* comments)
{ if (left.type==Data::typeError || right.type==Data::typeError)
    return false;
  if (left.type==Data::typeRational || right.type==Data::typeRational)
  { output=Data(0, *left.owner);
    return true;
  }
  if (left.type==Data::typeElementSSalgebra || right.type==Data::typeElementSSalgebra)
  { ElementSimpleLieAlgebra& leftElt=left.owner->theLieAlgebraElements[left.GetSmallInt()];
    ElementSimpleLieAlgebra& rightElt=right.owner->theLieAlgebraElements[right.GetSmallInt()];
    if (leftElt.ownerArray!=rightElt.ownerArray || leftElt.indexOfOwnerAlgebra!=rightElt.indexOfOwnerAlgebra)
      return false;
    SemisimpleLieAlgebra& theOwnerAlg=leftElt.GetOwner();
    ElementSimpleLieAlgebra outputElt;
    theOwnerAlg.LieBracket(leftElt, rightElt, outputElt);
    output.owner=left.owner;
    output.type=output.typeElementSSalgebra;
    output.theRational.GetElement()=output.owner->theLieAlgebraElements.AddNoRepetitionOrReturnIndexFirst(outputElt);
    return true;
  }
  return false;
}

bool Data::OperatorDereference
  (const Data& argument1, const Data& argument2, Data& output, std::stringstream* comments)
{ int whichInteger1, whichInteger2;
  switch(this->type)
  { case Data::typeSSalgebra:
      if (!argument1.IsSmallInteger(whichInteger1) ||!argument2.IsSmallInteger(whichInteger2) )
      { if (comments!=0)
        *comments << "You requested element of a semisimple Lie algebra labeled by ("
        << argument1.ElementToString() << ", " << argument2.ElementToString()
        << ")  which is not a pair of small integers. ";
        return false;
      }
      return output.MakeElementSemisimpleLieAlgebra
      (*this->owner, this->owner->theLieAlgebras[this->GetSmallInt()], whichInteger1, whichInteger2, comments);
    default:
      return false;
  }
}

bool Data::OperatorDereference
  (const Data& argument, Data& output, std::stringstream* comments)
{ int whichInteger;
  switch(this->type)
  { case Data::typeSSalgebra:
      if (!argument.IsSmallInteger(whichInteger))
      { if (comments!=0)
        *comments << "You requested element of a semisimple Lie algebra labeled by "
        << argument.ElementToString() << " which is not a small integer. ";
        return false;
      }
      return output.MakeElementSemisimpleLieAlgebra
      (*this->owner, this->owner->theLieAlgebras[this->GetSmallInt()], whichInteger, comments);
    default:
      return false;
  }
}

bool Data::IsEqualToOne()const
{ switch (this->type)
  { case Data::typeRational:
      return this->theRational.GetElementConst().IsEqualToOne();
    default:
      return false;
  }
}

bool Data::IsEqualToZero()const
{ switch (this->type)
  { case Data::typeRational:
      return this->theRational.GetElementConst().IsEqualToZero();
    default:
      return false;
   }
}

bool Data::operator*=(const Data& other)
{ std::stringstream out;
  if (this->type!=other.type)
  { out << "Multiplying different types, " << this->type << " and " << other.type << ", is not allowed.";
    return this->SetError(out.str());
  }
  switch(this->type)
  { case Data::typeRational:
      this->theRational.GetElement()*=other.theRational.GetElementConst();
      return true;
    default:
      out << "Don't know how to multiply elements of type " << this->type << ". ";
      return this->SetError(out.str());
  }
}

bool Data::operator/=(const Data& other)
{ std::stringstream out;
  if (this->type!=other.type)
  { out << "Multiplying different types, " << this->type << " and " << other.type << ", is not allowed.";
    return this->SetError(out.str());
  }
  if (other.IsEqualToZero())
    return false;
  switch(this->type)
  { case Data::typeRational:
      this->theRational.GetElement()/=other.theRational.GetElementConst();
      return true;
    default:
      out << "Don't know how to multiply elements of type " << this->type << ". ";
      return this->SetError(out.str());
  }
}
