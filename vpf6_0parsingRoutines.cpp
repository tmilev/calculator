//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6_0cpp(__FILE__, "Calculator input parsing routines. ");

SyntacticElement Calculator::GetEmptySyntacticElement()
{ SyntacticElement result;
  result.controlIndex=this->controlSequences.GetIndex(" ");
  result.theData.reset(*this);
  return result;
}

std::string SyntacticElement::ToStringHumanReadable(Calculator& theBoss, bool includeLispifiedExpressions)const
{ std::string controlString;
  if (this->controlIndex<0)
    controlString= "Error: control index is not initialized! This is likely a programming error.";
  else
    controlString=theBoss.controlSequences[this->controlIndex];
  bool makeTable=this->controlIndex==theBoss.conExpression() ||
  this->controlIndex==theBoss.conError() || this->controlIndex==theBoss.conSequence() ||
  this->controlIndex==theBoss.conSequenceStatements();
  if (!makeTable)
    return controlString;
  std::stringstream out;
  out << "<table style=\"vertical-align:top;border-spacing=0px 0px\">";
  out << "<tr><td style=\"text-align:center\">" << this->theData.ToString(0) << "</td></tr>";
  out << "<tr><td style=\"color:#AAAAAA\">" << controlString << "</td></tr>";
  if (includeLispifiedExpressions)
    out <<  "<tr><td style=\"color:#AAAAAA\">" << this->theData.ToStringFull() << "</td></tr>";
  if (this->errorString!="")
    out << "<tr><td>" << this->errorString << "</td></tr>";
  out << "</table>";
  return out.str();
}

void Calculator::reset()
{ this->MaxAlgTransformationsPerExpression=100;
  this->MaxRuleStacksCached=500;
  this->MaxCachedExpressionPerRuleStack=100000;
  this->MaxRecursionDeptH=10000;
  this->RecursionDeptH=0;
  this->NumErrors=0;
  this->DepthRecursionReached=0;
  this->flagLogSyntaxRules=false;
  this->flagLogEvaluatioN=false;
  this->flagUseNumberColors=false;
  this->flagLogRules=false;
  this->flagLogCache=false;
  this->flagLogPatternMatching=false;
  this->flagLogFullTreeCrunching=false;
  this->flagNewContextNeeded=true;
  this->flagProduceLatexLink=false;
  this->flagDisplayFullExpressionTree=false;
  this->flagUseFracInRationalLaTeX=true;
  this->flagDontDistribute=false;
  this->flagForkingProcessAllowed=true;
  this->flagNoApproximations=false;
  this->flagCurrentExpressionIsNonCacheable=false;
  this->MaxLatexChars=2000;
  this->numEmptyTokensStart=9;
  this->theObjectContainer.reset();
  this->controlSequences.Clear();

  this->theAtoms.Clear();
  this->operationsComposite.Clear();
  this->operationsCompositeHandlers.SetSize(0);
  this->builtInTypes.Clear();
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.Clear();
  this->atomsThatFreezeArguments.Clear();
  this->atomsNotInterprettedAsFunctions.Clear();
  this->atomsNotAllowingChainRule.Clear();
  this->atomsWhoseExponentsAreInterprettedAsFunctions.Clear();
  this->arithmeticOperations.Clear();
  this->knownFunctionsWithComplexRange.Clear();
  this->knownDoubleConstants.Clear();
  this->knownDoubleConstantValues.SetSize(0);
  this->FunctionHandlers.SetSize(0);

  this->syntacticSouP.SetSize(0);
  this->syntacticStacK.SetSize(0);
  this->flagTimeLimitErrorDetected=false;
  this->flagFirstErrorEncountered=false;
  this->flagMaxTransformationsErrorEncountered=false;
  this->flagMaxRecursionErrorEncountered=false;
  this->flagAbortComputationASAP=false;
  this->flagDisplayContext=false;
  this->EvaluatedExpressionsStack.Clear();
  this->theCruncherIds.Clear();
  this->theCruncherS.SetSize(0);
  this->syntaxErrors="";
  this->evaluationErrors.SetSize(0);
  this->CurrentSyntacticStacK=&this->syntacticStacK;
  this->CurrrentSyntacticSouP=&this->syntacticSouP;
  this->cachedExpressions.Clear();
  this->imagesCachedExpressions.SetSize(0);
  this->theProgramExpression.reset(*this);
  this->RuleStackCacheIndex=-1;
  this->RuleStack.reset(*this,this->MaxRuleStacksCached);
  this->cachedRuleStacks.Clear();
  //The expression container must be cleared last!
  this->theExpressionContainer.Clear();
  //Setting up a random seed.
  srand (time(NULL));
}

void Calculator::init()
{ MacroRegisterFunctionWithName("Calculator::init");
  this->reset();

  this->theAtoms.SetExpectedSize(300);
  this->FunctionHandlers.SetExpectedSize(300);
  this->builtInTypes.SetExpectedSize(50);
  this->operationsComposite.SetExpectedSize(50);
  this->operationsCompositeHandlers.SetExpectedSize(50);

  this->formatVisibleStrings.flagExpressionIsFinal=true;
  this->theObjectContainer.theAlgebraicClosure.theGlobalVariables=&theGlobalVariables;

  //operation List is the very first operation. It signifies a non-atomic expression.
  //operation List is signified by the empty string
  //operation List must always have index 0.
  this->AddOperationNoRepetitionAllowed("");

  this->AddOperationNoRepetitionAllowed("=");
  this->AddOperationNoRepetitionAllowed(";");
  this->AddOperationNoRepetitionAllowed("if=");
  this->AddOperationNoRepetitionAllowed("{}");
  this->AddOperationNoRepetitionAllowed("+");
  this->AddOperationNoRepetitionAllowed("-");
  this->AddOperationNoRepetitionAllowed("/");
  this->AddOperationNoRepetitionAllowed("*");
  this->AddOperationNoRepetitionAllowed("!");
  this->AddOperationNoRepetitionAllowed("mod");
  this->AddOperationNoRepetitionAllowed("and");
  this->AddOperationNoRepetitionAllowed("or");
  this->AddOperationNoRepetitionAllowed("\\times");
  this->AddOperationNoRepetitionAllowed("\\otimes");
  this->AddOperationNoRepetitionAllowed("\\choose");
  this->AddOperationNoRepetitionAllowed("\\sqrt");
  this->AddOperationNoRepetitionAllowed("[]");
  this->AddOperationNoRepetitionAllowed("=:");
  this->AddOperationNoRepetitionAllowed("^");
  this->AddOperationNoRepetitionAllowed(">");
  this->AddOperationNoRepetitionAllowed("<");
  this->AddOperationNoRepetitionAllowed("==");
  this->AddOperationNoRepetitionAllowed("\\cup");
  this->AddOperationNoRepetitionAllowed("\\sqcup");
  this->AddOperationNoRepetitionAllowed("\\cap");
  this->AddOperationNoRepetitionAllowed("Error");
  this->AddOperationNoRepetitionAllowed("Sequence");
  this->AddOperationNoRepetitionAllowed("Context");
  this->AddOperationNoRepetitionAllowed("\"");
  this->AddOperationNoRepetitionAllowed("PolyVars");
  this->AddOperationNoRepetitionAllowed("DiffOpVars");


  this->AddOperationBuiltInType("Rational");
  this->AddOperationBuiltInType("EltZmodP");
  this->AddOperationBuiltInType("Double");
  this->AddOperationBuiltInType("AlgebraicNumber");
  this->AddOperationBuiltInType("PolynomialRational");
  this->AddOperationBuiltInType("PolynomialOverANs");
  this->AddOperationBuiltInType("RationalFunction");
  this->AddOperationBuiltInType("string");
  this->AddOperationBuiltInType("ElementUEoverRF");
  this->AddOperationBuiltInType("ElementTensorGVM");
  this->AddOperationBuiltInType("CharSSAlgMod");
  this->AddOperationBuiltInType("SemisimpleLieAlg");
  this->AddOperationBuiltInType("LittelmannPath");
  this->AddOperationBuiltInType("LRO");
  this->AddOperationBuiltInType("Matrix_Rational");
  this->AddOperationBuiltInType("MatrixAlgebraic");
  this->AddOperationBuiltInType("MatrixTensorRational");
  this->AddOperationBuiltInType("MatrixRF");
  this->AddOperationBuiltInType("CalculusPlot");
  this->AddOperationBuiltInType("SemisimpleSubalgebras");
  this->AddOperationBuiltInType("CandidateSSsubalgebra");
  this->AddOperationBuiltInType("WeylGroup");
  this->AddOperationBuiltInType("ElementWeylGroup");
  this->AddOperationBuiltInType("HyperoctahedralGroupRepresentation");
  this->AddOperationBuiltInType("ElementHyperoctahedral");
  this->AddOperationBuiltInType("WeylGroupRep");
  this->AddOperationBuiltInType("WeylGroupVirtualRep");
  this->AddOperationBuiltInType("ElementWeylAlgebra");
  this->AddOperationBuiltInType("weightLieAlg");
  this->AddOperationBuiltInType("weightLieAlgPoly");
//  this->AddOperationBuiltInType("\\arctan");


  this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTop("{{}}");
  this->controlSequences.AddOnTop("Variable");
  this->controlSequences.AddOnTop(this->theAtoms);//all operations defined up to this point are also control sequences
  this->controlSequences.AddOnTop("Expression");
  this->controlSequences.AddOnTop("Integer");
  this->controlSequences.AddOnTop(",");
  this->controlSequences.AddOnTop(".");
  this->controlSequences.AddOnTop("\"");
  this->controlSequences.AddOnTop("if");
  this->controlSequences.AddOnTop("\\frac");
  this->controlSequences.AddOnTop("\\cdot");
  this->controlSequences.AddOnTop("_");
  this->controlSequences.AddOnTop("(");
  this->controlSequences.AddOnTop(")");
  this->controlSequences.AddOnTop("[");
  this->controlSequences.AddOnTop("]");
  this->controlSequences.AddOnTop("{");
  this->controlSequences.AddOnTop("}");
  this->controlSequences.AddOnTop(":");
  this->controlSequences.AddOnTop("\"");
  this->controlSequences.AddOnTop("pi");
  this->controlSequences.AddOnTop("ln");
  this->controlSequences.AddOnTop("\\ln");
  this->controlSequences.AddOnTop("log");
  this->controlSequences.AddOnTop("sin");
  this->controlSequences.AddOnTop("cos");
  this->controlSequences.AddOnTop("tan");
  this->controlSequences.AddOnTop("cot");
  this->controlSequences.AddOnTop("csc");
  this->controlSequences.AddOnTop("sec");
  this->controlSequences.AddOnTop("sqrt");
  this->controlSequences.AddOnTop("arcsin");
  this->controlSequences.AddOnTop("arccos");
  this->controlSequences.AddOnTop("arctan");
  this->controlSequences.AddOnTop("SequenceStatements");
  this->controlSequences.AddOnTop("MakeSequence");
  this->controlSequences.AddOnTop("SequenceMatrixRows");
  this->controlSequences.AddOnTop("MatrixRow");
  this->controlSequences.AddOnTop("\\circ");
  this->controlSequences.AddOnTop("$");
  this->controlSequences.AddOnTop("MatrixSeparator");
  this->controlSequences.AddOnTop("MatrixRowSeparator");
  this->controlSequences.AddOnTop("\\begin");
  this->controlSequences.AddOnTop("\\left");
  this->controlSequences.AddOnTop("\\right");
  this->controlSequences.AddOnTop("array");
  this->controlSequences.AddOnTop("\\end");
  this->controlSequences.AddOnTop("\\\\");
  this->controlSequences.AddOnTop("\\");
  this->controlSequences.AddOnTop("&");
  this->controlSequences.AddOnTop("%");
  this->controlSequences.AddOnTop("NoFrac");
  this->controlSequences.AddOnTop("NoApproximations");
  this->controlSequences.AddOnTop("ShowContext");
  this->controlSequences.AddOnTop("LogParsing");
  this->controlSequences.AddOnTop("LogEvaluation");
  this->controlSequences.AddOnTop("NumberColors");
  this->controlSequences.AddOnTop("LogRules");
  this->controlSequences.AddOnTop("LogCache");
  this->controlSequences.AddOnTop("LogFull");
  this->controlSequences.AddOnTop("LatexLink");
  this->controlSequences.AddOnTop("FullTree");
  this->controlSequences.AddOnTop("HideLHS");
  this->controlSequences.AddOnTop("DontDistribute");
  this->controlSequences.AddOnTop("EndProgram");

  this->initPredefinedStandardOperationsWithoutHandler();

  this->TotalNumPatternMatchedPerformed=0;
  this->initPredefinedStandardOperations();
  this->initPredefinedInnerFunctions();
  this->initCalculusTestingFunctions();
  this->initPredefinedOperationsComposite();
  this->initAtomsThatAllowCommutingOfArguments();
  this->initAtomsThatFreezeArguments();
  this->initAtomsNotGoodForChainRule();
  this->initBuiltInAtomsNotInterprettedAsFunctions();
  this->initBuiltInAtomsWhosePowersAreInterprettedAsFunctions();
  this->initOperationsWhoseDomainsAreTheConstants();
  this->initAtomsNonCacheable();
  this->initArithmeticOperations();

  Expression theSSLieAlgrule;
  this->RuleStack.reset(*this, 100);
  this->RuleStack.AddChildAtomOnTop(this->opEndStatement());
  this->cachedRuleStacks.Clear();
  this->RuleStackCacheIndex=0;
  this->cachedRuleStacks.AddOnTop(this->RuleStack);
  this->NumPredefinedAtoms=this->theAtoms.size; //<-operations added up to this point are called ``operations''

/*  this->Evaluate
  ("(InternalVariable1{}{{InternalVariableA}})_{{InternalVariableB}}\
   =getSemisimpleLieAlgGenerator{}\
   (InternalVariable1{}InternalVariableA,\
   InternalVariableB)")
  ;
  this->theProgramExpression[1][0].SetChildAtomValue(0, this->opSSLieAlg(), *this);
  this->theProgramExpression[2][1][1].SetChildAtomValue(0, this->opSSLieAlg(), *this);
  this->RuleStack.AddOnTop(this->theProgramExpression);
*/


//  stOutput << "<br>Num lists created at command list initialization exit: " << NumListsCreated;
}

bool Calculator::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.format=formatOptions;
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(middleE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(7);
}

bool Calculator::ReplaceSqrtEXByEX(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& argument=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr, twoE;
  twoE.AssignValue(2, *this);
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSqrt());
  newExpr.AddChildOnTop(twoE);
  newExpr.AddChildOnTop(argument.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(1);
}

bool Calculator::ReplaceSqrtXEXByEX(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& argument=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr, twoE;
  twoE.AssignValue(2, *this);
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSqrt());
  newExpr.AddChildOnTop(twoE);
  newExpr.AddChildOnTop(argument.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceOXEXEByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.format=formatOptions;
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(4);
}

bool Calculator::ReplaceOXEXEXByEX(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.format=formatOptions;
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(4);
}

bool Calculator::ReplaceOXXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-9];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(middleE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  newExpr.format= formatOptions;
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(8);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceOXEByE(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::ReplaceOEByE(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::ReplaceOEXByEX(int formatOptions)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  newExpr.format= formatOptions;
  middle.theData=newExpr;
  middle.controlIndex=this->conExpression();
  this->DecreaseStackExceptLast(1);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceXXByEmptyString()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  left.theData.AssignValue((std::string)"", *this);
  left.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceEOByE(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(right.controlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::isRightSeparator(unsigned char c)
{ switch(c)
  { case ' ':
    case 160: //&nbsp character
    case '\r':
    case '\n':
    case '>':
    case '<':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '=':
    case '_':
    case '%':
    case '/':
    case '-':
    case '`':
    case '[':
    case '&':
    case '\\':
    case '}':
    case '{':
    case '~':
    case '$':
    case ']':
    case '(':
    case ')':
    case '.':
    case '!':
    case '\"':
      return true;
    default:
      return false;
  }
}

bool Calculator::isLeftSeparator(unsigned char c)
{ switch(c)
  { case ' ':
    case 160: //&nbsp character
    case '\r':
    case '\n':
    case '>':
    case '<':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '&':
    case '=':
    case '_':
    case '/':
    case '-':
    case '`':
    case '[':
    case '%':
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
    case '.':
    case '!':
    case '\"':
      return true;
    default:
      return false;
  }
}

bool Calculator::isInterprettedAsEmptySpace(const std::string& input)
{ if (input.size()!=1)
    return false;
  switch ((unsigned) input[0])
  {
  case '\n': //numerical value: 10
  case '~':
  case '\r': //numerical value: 13
  case '\t':
  case 160: //&nbsp
    return true;
  default: return false;
  }
}

void Calculator::ParseFillDictionary(const std::string& input)
{ std::string current;
  (*this->CurrrentSyntacticSouP).Reserve(input.size());
  (*this->CurrrentSyntacticSouP).SetSize(0);
  char LookAheadChar;
  SyntacticElement currentElement;
  int currentDigit;
  bool inQuotes=false;
  for (unsigned i=0; i<input.size(); i++)
  { current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
    { LookAheadChar=' ';
      if (inQuotes)
        LookAheadChar='"';
    }
    if (!inQuotes )
      if (this->isInterprettedAsEmptySpace(current))
        current=" ";
    bool shouldSplit= (this->isLeftSeparator(current[0]) || this->isRightSeparator(LookAheadChar) || current==" ");
    if (MathRoutines::isADigit(LookAheadChar))
      if (current[current.size()-1]=='\\')
        shouldSplit=true;
    if (inQuotes)
      shouldSplit=(LookAheadChar=='"');
    if (current=="\"")
    { inQuotes=!inQuotes;
      shouldSplit=true;
    }
    //stOutput << "<br>in quotes: "<< inQuotes << " current: " << current << " shouldsplit: " << shouldSplit;

    if (!shouldSplit)
      continue;
    if (this->controlSequences.Contains(current))
    { currentElement.controlIndex=this->controlSequences.GetIndex(current);
      currentElement.theData.reset(*this);
      (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
    } else if (MathRoutines::isADigit(current, &currentDigit))
    { currentElement.theData.AssignValue<Rational>(currentDigit, *this);
      currentElement.controlIndex=this->conInteger();
      (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
    } else
    { currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
      currentElement.theData.MakeAtom(this->AddOperationNoRepetitionOrReturnIndexFirst(current), *this);
      (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
    // stOutput << "<br>Adding syntactic element " << currentElement.ToString(*this);
    }
    current="";
  }
  currentElement.theData.reset(*this);
  currentElement.controlIndex=this->conEndProgram();
  (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
}

int Calculator::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->theAtoms.GetIndex(this->controlSequences[controlIndex]);
}

int Calculator::GetExpressionIndex()
{ return this->controlSequences.GetIndex("Expression");
}

bool Calculator::ReplaceOXbyE(int inputFormat)
{ this->ReplaceOXbyEX(inputFormat);
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXOXbyEusingO(int theControlIndex, int inputFormat)
{ this->ReplaceOXbyEXusingO(theControlIndex, inputFormat);
  return this->ReplaceXEXByE(inputFormat);
}

bool Calculator::ReplaceOXbyEX(int inputFormat)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  return this->ReplaceOXbyEXusingO(theElt.controlIndex, inputFormat);
}

bool Calculator::ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence(int inputFormat)
{ SyntacticElement& theSequenceElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& theFunctionElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.Reserve(theSequenceElt.theData.children.size);
  newExpr.AddChildOnTop(theFunctionElt.theData);
  if (theSequenceElt.theData.IsAtom())
    newExpr.AddChildOnTop(theSequenceElt.theData);
  else
    for (int i=1; i<theSequenceElt.theData.children.size; i++)
      newExpr.AddChildOnTop(theSequenceElt.theData[i]);
  theFunctionElt.theData=newExpr;
  return this->DecreaseStackSetCharacterRangeS(4);
}

bool Calculator::ReplaceXEXByE(int inputFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=this->conExpression();
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat;
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceXEXByEcontainingOE(int inputOpIndex, int inputFormat)
{ SyntacticElement& outputElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& inputElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  outputElt.theData.reset(*this, 2);
  outputElt.theData.AddChildAtomOnTop(inputOpIndex);
  outputElt.theData.AddChildOnTop(inputElt.theData);
  outputElt.theData.format=inputFormat;
  outputElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceOXbyEXusingO(int theControlIndex, int inputFormat)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theElt.theData.format=inputFormat;
  theElt.controlIndex=this->conExpression();
  return true;
}

bool Calculator::ReplaceXXByEEmptySequence()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theElt.theData.reset(*this, 1);
  theElt.theData.AddChildAtomOnTop(this->opSequence());
  theElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceXXVXdotsXbyE_BOUND_XdotsX(int numXs)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  int theBoundVar=theElt.theData.theData;
//  stOutput << "<br>Registering bound variable index: " << theBoundVar;
  if (this->IsNonBoundVarInContext(theBoundVar))
  { std::stringstream out;
    out << "Syntax error. In the same syntactic scope, the string " << this->theAtoms[theBoundVar] << " is first used to denote a non-bound variable"
    << " but later to denote a bound variable. This is not allowed. ";
    theElt.errorString=out.str();
    theElt.controlIndex=this->conError();
    this->DecreaseStackSetCharacterRangeS(numXs);
    this->ReplaceXXYByY();
    return true;
  }
  if (!this->IsBoundVarInContext(theBoundVar))
    this->BoundVariablesStack.LastObject()->AddOnTopNoRepetition(theBoundVar);
  theElt.theData.reset(*this, 2);
  theElt.theData.AddChildAtomOnTop(this->opBind());
  theElt.theData.AddChildAtomOnTop(theBoundVar);
  theElt.controlIndex=this->conExpression();
//  stOutput << ", got to element: " << theElt.theData.ToString();
  this->DecreaseStackSetCharacterRangeS(numXs);
  this->ReplaceXXYByY();
//  stOutput << ", finally got: "
//  << (*this->CurrentSyntacticStacK).LastObject()->ToString(*this);
  return true;
}

bool Calculator::ReplaceVXdotsXbyE_NONBOUND_XdotsX(int numXs)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1-numXs];
  int theBoundVar=theElt.theData.theData;
//  stOutput << "<br>index of variable: " << theElt.ToString(*this);
  if (this->IsBoundVarInContext(theBoundVar))
  { theElt.theData.reset(*this, 2);
    theElt.theData.AddChildAtomOnTop(this->opBind());
    theElt.theData.AddChildAtomOnTop(theBoundVar);
  } else
  { theElt.theData.MakeAtom(theBoundVar, *this);
    if (!this->IsNonBoundVarInContext(theBoundVar))
      this->NonBoundVariablesStack.LastObject()->AddOnTop(theBoundVar);
  }
  theElt.controlIndex=this->conExpression();
  return true;
}

bool Calculator::ReplaceAXbyEX()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
//  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
//  stOutput << "replaceAbyE: " << theElt.theData.ToString();
  return true;
}

bool Calculator::LookAheadAllowsApplyFunction(const std::string& lookAhead)
{ return lookAhead!="{" && lookAhead!="_" && lookAhead!="\\circ" && lookAhead!="{}" &&  lookAhead!="$";
}

bool Calculator::ReplaceSequenceUXEYBySequenceZY(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& afterleft = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  left.theData.AddChildOnTop(right.theData);
  left.theData.format=inputFormat;
  left.controlIndex=theControlIndex;
  afterleft=*this->CurrentSyntacticStacK->LastObject();
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceSequenceXEBySequence(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  left.theData.AddChildOnTop(right.theData);
  left.theData.format=inputFormat;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int inputFormat, int numXs)
{ SyntacticElement& main = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->opSequence());
  newExpr.AddChildOnTop(main.theData);
  newExpr.format=inputFormat;
  main.theData=newExpr;
  main.controlIndex=theControlIndex;
  return true;
}

bool Calculator::ReplaceEXEBySequence(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSequence());
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=inputFormat;
  left.theData=newExpr;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEEByEusingO(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  //    newExpr.inputIndexBoundVars=
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(1);
  //    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEOXbyEX()
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  int theOp=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(theOp);
  newExpr.AddChildOnTop(left.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(1);
  //    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEEXByEXusingO(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  //    newExpr.inputIndexBoundVars=
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(1);
  //    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXXEXEByEusingO(int theControlIndex)
{ SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(5);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXXEXEXByEXusingO(int theControlIndex)
{ SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-7];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(5);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEOEXByEX(int formatOptions)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  middle=*(*this->CurrentSyntacticStacK).LastObject();
//  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceXEEXByEXusingO(int inputOperation, int formatOptions)
{ //stOutput << "<b>Here iam!</b>";
  SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(inputOperation);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  middle=*(*this->CurrentSyntacticStacK).LastObject();
//  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}


bool Calculator::isSeparatorFromTheLeftGeneral(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input=="," || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" || input=="&";
}

bool Calculator::isSeparatorFromTheRightGeneral(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input=="," || input==":" || input==";"
  || input=="MatrixSeparator" || input=="="
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end" || input=="&" || input=="EndProgram";
}

bool Calculator::isSeparatorFromTheLeftForList(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" ;
}

bool Calculator::isSeparatorFromTheLeftForListMatrixRow(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" ;
}

bool Calculator::isSeparatorFromTheLeftForMatrixRow(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" ;
}

bool Calculator::isSeparatorFromTheRightForList(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" || input=="MatrixSeparator"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end";
}

bool Calculator::isSeparatorFromTheRightForListMatrixRow(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" || input=="MatrixSeparator"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end";
}

bool Calculator::isSeparatorFromTheRightForMatrixRow(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" || input=="MatrixSeparator" || input=="&"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end";
}

bool Calculator::isSeparatorFromTheLeftForDefinition(const std::string& input)
{ return input=="{" || input=="(" || input==";" || input=="SequenceStatements" || input==" ";
}

bool Calculator::isSeparatorFromTheRightForDefinition(const std::string& input)
{ return input=="}" || input==")" || input==";" || input=="EndProgram";
}

bool Calculator::AllowsTensorInPreceding(const std::string& lookAhead)
{ return this->AllowsTimesInPreceding(lookAhead) || lookAhead=="\\otimes";
}

bool Calculator::AllowsTimesInPreceding(const std::string& lookAhead)
{ return lookAhead=="+" || lookAhead=="-" || lookAhead=="*" || lookAhead=="/" ||
  lookAhead=="Expression" ||  lookAhead== "Integer" || lookAhead=="\\cup" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="=" || lookAhead=="Variable" || lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="MatrixSeparator" || lookAhead=="\\" ||
  lookAhead=="sqrt" || lookAhead=="\\sqrt" || lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsAndInPreceding(const std::string& lookAhead)
{ return lookAhead=="and" || lookAhead=="or" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="MatrixSeparator" || lookAhead=="\\" ||
  lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsOrInPreceding(const std::string& lookAhead)
{ return lookAhead=="and" || lookAhead=="or" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="MatrixSeparator" || lookAhead=="\\" ||
  lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsPlusInPreceding(const std::string& lookAhead)
{ return lookAhead=="+" || lookAhead=="-" || lookAhead=="," || lookAhead=="=" || lookAhead==")" || lookAhead==";" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead==":" || lookAhead=="," || lookAhead=="\\choose" || lookAhead=="EndProgram" || lookAhead=="&" || lookAhead=="MatrixSeparator" || lookAhead=="\\";
  ;
}

bool Calculator::LookAheadAllowsDivide(const std::string& lookAhead)
{ return this->AllowsTimesInPreceding(lookAhead);
}

bool Calculator::ExtractExpressions(Expression& outputExpression, std::string* outputErrors)
{ MacroRegisterFunctionWithName("Calculator::ExtractExpressions");
  //std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->CurrentSyntacticStacK).Reserve((*this->CurrrentSyntacticSouP).size+this->numEmptyTokensStart);
  (*this->CurrentSyntacticStacK).SetSize(this->numEmptyTokensStart);
  this->registerNumNonClosedBeginArray=0;
  this->registerPositionAfterDecimalPoint=0;
  for (int i=0; i<this->numEmptyTokensStart; i++)
    (*this->CurrentSyntacticStacK)[i]=this->GetEmptySyntacticElement();
  this->parsingLog="";
  this->NonBoundVariablesStack.SetSize(1);
  this->BoundVariablesStack.SetSize(1);
  this->NonBoundVariablesStack.LastObject()->Clear();
  this->BoundVariablesStack.LastObject()->Clear();
  const int maxNumTimesOneRuleCanBeCalled=1000;
  for (this->counterInSyntacticSoup=0; this->counterInSyntacticSoup<(*this->CurrrentSyntacticSouP).size; this->counterInSyntacticSoup++)
  { (*this->CurrentSyntacticStacK).AddOnTop((*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup]);
    int numTimesRulesCanBeAppliedWithoutStackDecrease=0;
    int minStackSize=this->CurrentSyntacticStacK->size ;
    while(this->ApplyOneRule())
    { if (this->CurrentSyntacticStacK->size <minStackSize)
      { numTimesRulesCanBeAppliedWithoutStackDecrease=0;
        minStackSize=this->CurrentSyntacticStacK->size ;
      } else
        numTimesRulesCanBeAppliedWithoutStackDecrease++;
      if (numTimesRulesCanBeAppliedWithoutStackDecrease>maxNumTimesOneRuleCanBeCalled)
        crash << "This may be a programming error: Calculator::ApplyOneRule called more than " << maxNumTimesOneRuleCanBeCalled
        << " times without advancing to the next syntactic element in the syntactic soup. If this is indeed an expression which requires that"
        << " many application of a single parsing rule, then you should modify function Calculator::ExtractExpressions"
        << crash;
    }
  }
  bool success=false;
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart)
    errorLog << "Non-meaningful input detected (spacebar, enter characters only?).";
  else if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
  { SyntacticElement& result=(*this->CurrentSyntacticStacK)[this->numEmptyTokensStart];
    if (result.errorString=="" && result.controlIndex==this->conExpression())
    { outputExpression=result.theData;
      success=true;
//      stOutput << "Success: " << result.theData.ToString();
    } else if (result.errorString!="")
      errorLog << "Syntax error with message: " << result.errorString;
    else
    { errorLog << "Syntax error: your command simplifies to a single syntactic element but it is not an expression. <br>";
      errorLog << "It simplifies to:<br> " << this->ToStringSyntacticStackHTMLTable();
    }
  } else
  { errorLog << "Syntax error: your command does not simplify to a single syntactic element. <br>";
    errorLog << "Instead it simplifies to:<br> " << this->ToStringSyntacticStackHTMLTable();
  }
  if (outputErrors!=0)
    *outputErrors=errorLog.str();
  return success;
}

bool Calculator::ApplyOneRule()
{ //return false;
  MacroRegisterFunctionWithName("Calculator::ApplyOneRule");
  if (this->CurrentSyntacticStacK->size<=this->numEmptyTokensStart)
    return false;
  const SyntacticElement& lastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  const std::string& lastS=this->controlSequences[lastE.controlIndex];
  if (lastS==" ")
    return  this->PopTopSyntacticStack();
  const SyntacticElement& secondToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  const std::string& secondToLastS=this->controlSequences[secondToLastE.controlIndex];
  const SyntacticElement& thirdToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  const std::string& thirdToLastS=this->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  const std::string& fourthToLastS=this->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  const std::string& fifthToLastS=this->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  const std::string& sixthToLastS=this->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-7];
  const std::string& seventhToLastS=this->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  const std::string& eighthToLastS=this->controlSequences[eighthToLastE.controlIndex];
//  const SyntacticElement& ninthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-9];
//  const std::string& ninthToLastS=this->theBoss->controlSequences[ninthToLastE.controlIndex];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "<hr>" + this->ToStringSyntacticStackHTMLTable();
  if (secondToLastS=="%" && lastS=="LogParsing")
  { this->parsingLog+= "<hr>" + this->ToStringSyntacticStackHTMLTable();
    this->flagLogSyntaxRules=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogFull")
  { stOutput
    << "<hr>You are requesting a full log of the evaluation process. <br><b>WARNING requesting  a full log of the evaluation process is very slow, "
    << " and might produce a HUGE printout. </b><br><b>You have been warned. </b><hr>";
    this->flagLogFullTreeCrunching=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogEvaluation")
  { this->flagLogEvaluatioN=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="NumberColors")
  { if (!this->flagUseNumberColors)
    { *this << "<span style=\"color:blue\">Floating point numbers</span> are displayed in <span style=\"color:blue\">blue</span>."
      << "<br><span style=\"color:red\">Algebraic numbers</span> are displayed in <span style=\"color:red\">red</span>. "
      << "<br>Rational numbers are displayed in default color.";
    }
    this->flagUseNumberColors=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogCache")
  { this->flagLogCache=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogRules")
  { this->flagLogRules=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="ShowContext")
  { this->flagDisplayContext=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="DontDistribute")
  { this->flagDontDistribute=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LatexLink")
  { this->flagProduceLatexLink=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="FullTree")
  { this->flagDisplayFullExpressionTree=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="HideLHS")
  { this->flagHideLHS=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="NoFrac")
  { this->flagUseFracInRationalLaTeX=false;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="NoApproximations")
  { this->flagNoApproximations=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }

/*  if (lastE.theData.IndexBoundVars==-1)
  { crash << "<hr>The last expression, " << lastE.ToString(*this) << ", while reducing "
    << this->ElementToStringSyntacticStackHTMLTable()
    << " does not have properly initialized context. "
    << crash;
  }*/
  if (secondToLastS=="=" && lastS==":")
    return this->ReplaceXXByCon(this->conIsDenotedBy());
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->conApplyFunction(), Expression::formatDefault);
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->conApplyFunction(), Expression::formatFunctionUseCdot);
  if (lastS=="\\circ")
    return this->ReplaceXByCon(this->conApplyFunction(), Expression::formatFunctionUseCdot);
//  if ( thirdToLastS=="{" && secondToLastS=="{}" && lastS=="}")
//    return this->ReplaceXXXByCon(this->conBindVariable());
  if (fifthToLastS=="{" && fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS=="}")
    return this->ReplaceXXVXdotsXbyE_BOUND_XdotsX(2);
  if (secondToLastS=="Variable" && ((lastS!="}" && lastS!=" ") || thirdToLastS!="{" || fourthToLastS!="{"))
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(1);
  if (fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS!="}" && lastS!=" ")
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(2);
  if (secondToLastS=="\\left" && lastS == "(")
    return this->ReplaceXYByY();
  if (secondToLastS=="\\right" && lastS == ")")
    return this->ReplaceXYByY();
  if (lastS=="=" && secondToLastS=="=")
    return this->ReplaceXXByCon(this->conEqualEqual());
  if (lastS=="Integer" && secondToLastS=="Integer")
    return this->ReplaceIntIntBy10IntPlusInt();
  if (secondToLastS=="Integer" && lastS!="Integer" && lastS!=".")
  { this->registerPositionAfterDecimalPoint=0;
    return this->ReplaceAXbyEX();
  }
  if (secondToLastS=="(" && lastS==")")
    return this->ReplaceXXByEEmptySequence();
  if (thirdToLastS=="Integer" && secondToLastS=="." && lastS=="Integer" && this->registerPositionAfterDecimalPoint==0)
  { this->registerPositionAfterDecimalPoint=1;
    return this->ReplaceXYByY();
  }
  //there is an ambiguity on how should function application be associated
  //Which is better: x{}y{}z= x{} (y{}z), or x{}y{}z=(x{}y){}z ?
  //In our implementation, we choose x{}y{}z= x{} (y{}z). Although this is slightly harder to implement,
  //it appears to be the more natural choice.
//  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression"
//      && this->LookAheadAllowsApplyFunction(lastS) )
//    return this->ReplaceEOEXByEX(secondToLastE.theData.format);
  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression" && this->LookAheadAllowsApplyFunction(lastS))
    return this->ReplaceEXEXByEX(Expression::formatDefault);
  if (fourthToLastS=="Expression" && thirdToLastS=="_" && secondToLastS=="Expression" && lastS!="_")
    return this->ReplaceEXEXByEX(Expression::formatFunctionUseUnderscore);
  //end of ambiguity.
  if (fourthToLastS=="{"  && thirdToLastS=="{}" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXYYXByYY();
  if (secondToLastS=="Expression" && lastS=="!")
    return this->ReplaceEOByE();
  if (secondToLastS=="Expression" && thirdToLastS=="^" && fourthToLastS=="Expression" && this->LookAheadAllowsThePower(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="or" && fourthToLastS=="Expression" && this->AllowsOrInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="and" && fourthToLastS=="Expression" && this->AllowsAndInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="+" && fourthToLastS=="Expression" && this->AllowsPlusInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" && fourthToLastS=="Expression" && this->AllowsPlusInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" && this->AllowsPlusInPreceding(lastS) )
    return this->ReplaceOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="mod" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="\\choose" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="\\otimes" && fourthToLastS=="Expression" && this->AllowsTensorInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="Expression" && fourthToLastS=="\\frac")
    return this->ReplaceXEEXByEXusingO(this->opDivide(), Expression::formatUseFrac);
  if (secondToLastS=="Expression" && thirdToLastS=="\\times" && fourthToLastS=="Expression" && this->AllowsTimesInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="*" && fourthToLastS=="Expression" && this->AllowsTimesInPreceding(lastS) )
    return this->ReplaceEOEXByEX(Expression::formatTimesDenotedByStar);
  if (secondToLastS=="Expression" && thirdToLastS=="/" && fourthToLastS=="Expression" && this->LookAheadAllowsDivide(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="Expression" && this->AllowsTimesInPreceding(lastS) )
    return this->ReplaceEEXByEXusingO(this->conTimes());
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE(secondToLastE.theData.format);
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXEXByE(secondToLastE.theData.format);
  if (thirdToLastS=="\"" && secondToLastS=="Expression" && lastS=="\"")
    return this->ReplaceXEXByEcontainingOE(this->opQuote());
  if (secondToLastS=="\"" && lastS=="\"")
    return this->ReplaceXXByEmptyString();
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (this->isSeparatorFromTheRightGeneral(lastS) && secondToLastS=="Expression" && thirdToLastS=="==" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheRightGeneral(lastS) && secondToLastS=="Expression" && thirdToLastS==">" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheRightGeneral(lastS) && secondToLastS=="Expression" && thirdToLastS=="<" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && fourthToLastS=="Expression" && thirdToLastS=="=" &&
      secondToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && fourthToLastS=="Expression" && thirdToLastS=="=:" &&
      secondToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEOEXByEX();
  if (lastS=="Sequence" && lastE.theData.children.size==0 && lastE.theData.theData==this->opLisT())
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("MakeSequence"));
  //else
  //  stOutput << "lastS is sequence but lastE is |" << lastE.theData.ToString() << "|";
  if (thirdToLastS=="\\sqrt" && secondToLastS=="Expression")
    return this->ReplaceSqrtEXByEX();
  if (fourthToLastS=="\\sqrt"  && thirdToLastS == "{}" && secondToLastS=="Expression")
    return this->ReplaceSqrtXEXByEX();

  if (sixthToLastS=="\\sqrt" && fifthToLastS=="[" && fourthToLastS=="Expression" && thirdToLastS=="]" && secondToLastS=="Expression")
    return this->ReplaceOXEXEXByEX();
  //Some synonyms:
  if (lastS=="ln" || lastS=="log" || lastS=="\\ln")
    return this->ReplaceXByEusingO(this->opLog());
  if (lastS=="pi")
    return this->ReplaceXByEusingO(this->opPi());
  if (lastS=="arcsin")
    return this->ReplaceXByEusingO(this->opArcSin());
  if (lastS=="arccos")
    return this->ReplaceXByEusingO(this->opArcCos());
  if (lastS=="arctan")
    return this->ReplaceXByEusingO(this->opArcTan());
  if (lastS=="sin")
    return this->ReplaceXByEusingO(this->opSin());
  if (lastS=="cos")
    return this->ReplaceXByEusingO(this->opCos());
  if (lastS=="tan")
    return this->ReplaceXByEusingO(this->opTan());
  if (lastS=="cot")
    return this->ReplaceXByEusingO(this->opCot());
  if (lastS=="csc")
    return this->ReplaceXByEusingO(this->opCsc());
  if (lastS=="sec")
    return this->ReplaceXByEusingO(this->opSec());
  if (lastS=="sqrt")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\sqrt"));
  //end of some synonyms
  if (fourthToLastS=="Expression" && thirdToLastS=="\\cup" && secondToLastS== "Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Expression" && thirdToLastS=="\\cap" && secondToLastS== "Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Expression" && thirdToLastS=="\\sqcup" && secondToLastS== "Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Sequence" && thirdToLastS=="," && secondToLastS=="Expression" && (lastS=="," || lastS==")" || lastS=="}"))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conSequence());
  if (thirdToLastS!="[" && secondToLastS=="Expression" && lastS==",")
    return this->ReplaceYXBySequenceX(this->conSequence(), secondToLastE.theData.format);
  if (thirdToLastS=="MakeSequence" && secondToLastS=="{}" && lastS=="Expression")
    return this->ReplaceXXYBySequenceY(this->conExpression());
  if (fourthToLastS=="MakeSequence" && thirdToLastS=="{}" && secondToLastS=="Expression")
    return this->ReplaceXXYXBySequenceYX(this->conExpression());
  if (fifthToLastS=="Expression" && fourthToLastS== "{}" && thirdToLastS=="(" && secondToLastS=="Sequence" && lastS==")")
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  if (fifthToLastS=="Expression" && fourthToLastS== "{}" && thirdToLastS=="{" && secondToLastS=="Sequence" && lastS=="}")
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  if (secondToLastS=="Sequence" &&
      ((thirdToLastS=="(" && lastS==")")||
       (thirdToLastS=="{" && lastS=="}")
      ))
    return this->ReplaceOXbyEX();
  if (fifthToLastS=="\\begin" && fourthToLastS=="{" && thirdToLastS=="array" && secondToLastS=="}" && lastS=="Expression")
  { this->registerNumNonClosedBeginArray++;
    return this->ReplaceXXXXXByCon(this->conMatrixSeparator(), Expression::formatMatrix);
  }
  if (fourthToLastS=="\\end" && thirdToLastS=="{" && secondToLastS=="array" && lastS=="}" )
  { this->registerNumNonClosedBeginArray--;
    return this->ReplaceXXXXByCon(this->conMatrixSeparator(), Expression::formatMatrix);
  }
  if (lastS=="\\" && secondToLastS== "\\")
    return this->ReplaceXXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\\\"));
  if (lastS=="\\\\")
  { if (this->registerNumNonClosedBeginArray>0)
      return this->ReplaceXByCon(this->conMatrixRowSeparator(), Expression::formatMatrixRow);
    else
      return this->PopTopSyntacticStack();
  }
  if (fourthToLastS=="SequenceMatrixRows" && thirdToLastS=="MatrixRowSeparator" && secondToLastS=="Expression" && this->isSeparatorFromTheRightForListMatrixRow(lastS))
    return this->ReplaceYXdotsXBySequenceYXdotsX(this->conMatrixRow(), Expression::formatMatrixRow, 1);
  if (fourthToLastS=="SequenceMatrixRows" && thirdToLastS=="MatrixRowSeparator" && secondToLastS=="MatrixRow" && this->isSeparatorFromTheRightForListMatrixRow(lastS))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conSequenceMatrixRow(), Expression::formatMatrix);
  if (fourthToLastS=="MatrixRow" && thirdToLastS=="&"  && secondToLastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lastS))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (fourthToLastS=="MatrixRow" && thirdToLastS=="&"  && secondToLastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lastS))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="Expression" && lastS=="&")
    return this->ReplaceYXBySequenceX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="Expression" && (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXBySequenceX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="MatrixRow" &&  (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXBySequenceX(this->conSequenceMatrixRow(), Expression::formatMatrix);
  if (thirdToLastS=="MatrixSeparator" && secondToLastS=="SequenceMatrixRows" && lastS=="MatrixSeparator")
    return this->ReplaceXOXbyEusingO(this->conSequence(), Expression::formatMatrix);
  if (secondToLastS=="MatrixRowSeparator" && lastS == "MatrixSeparator")
    return this->ReplaceXYByY();
  if (fifthToLastS=="[" && fourthToLastS=="Expression" && thirdToLastS=="," && secondToLastS=="Expression" && lastS=="]")
    return this->ReplaceXEXEXByEusingO(this->conLieBracket());
  if (this->isSeparatorFromTheLeftForDefinition(eighthToLastS) && seventhToLastS=="Expression" && sixthToLastS==":" &&
      fifthToLastS=="if" && fourthToLastS=="Expression" && thirdToLastS=="=" && secondToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEXXEXEXByEXusingO(this->conDefineConditional());
  if (lastS==";")
  { this->NonBoundVariablesStack.LastObject()->Clear();
    this->BoundVariablesStack.LastObject()->Clear();
  }
  if (thirdToLastS=="SequenceStatements" && secondToLastS=="Expression" && (lastS==")" || lastS =="}"))
    return this->ReplaceEXdotsXbySsXdotsX(1);
  if (secondToLastS=="Expression" && lastS==";")
    return this->ReplaceEXdotsXBySs(1);
  if (secondToLastS=="Expression" && lastS=="EndProgram")
    return this->ReplaceEXdotsXbySsXdotsX(1);
  if ((thirdToLastS=="(" || thirdToLastS=="{" ) && secondToLastS=="SequenceStatements" && (lastS==")" || lastS== "}"))
    return this->ReplaceXEXByE();
  if (secondToLastS=="SequenceStatements" && lastS=="SequenceStatements")
    return this->ReplaceSsSsXdotsXbySsXdotsX(0);
  if (thirdToLastS=="SequenceStatements" && secondToLastS=="SequenceStatements" && (lastS=="EndProgram" || lastS ==")" || lastS=="}"))
    return this->ReplaceSsSsXdotsXbySsXdotsX(1);
  if (secondToLastS=="SequenceStatements" && lastS=="EndProgram")
    return this->ReplaceXXByCon(this->conExpression());
  if (lastS=="EndProgram")
    return this->DecreaseStackSetCharacterRangeS(1);
  return false;
}

