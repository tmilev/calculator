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
  this->controlIndex==theBoss.conSequenceStatements()|| this->controlIndex==theBoss.conVariable();
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
  this->NumListsStart               =-1;
  this->NumListResizesStart         =-1;
  this->NumHashResizesStart         =-1;
  this->NumSmallAdditionsStart      =-1;
  this->NumSmallMultiplicationsStart=-1;
  this->NumSmallGCDcallsStart       =-1;
  this->NumLargeAdditionsStart      =-1;
  this->NumLargeMultiplicationsStart=-1;
  this->NumLargeGCDcallsStart       =-1;
  this->LastLogEvaluationTime=theGlobalVariables.GetElapsedSeconds();
  this->DepthRecursionReached=0;
  this->flagWriteLatexPlots=false;
  this->flagLogSyntaxRules=false;
  this->flagLogEvaluatioN=false;
  this->flagUseNumberColors=false;
  this->flagLogRules=false;
  this->flagLogCache=false;
  this->flagLogPatternMatching=false;
  this->flagUseLnAbsInsteadOfLogForIntegrationNotation=false;
  this->flagLogFullTreeCrunching=false;
  this->flagNewContextNeeded=true;
  this->flagProduceLatexLink=false;
  this->flagDisplayFullExpressionTree=false;
  this->flagHidePolynomialBuiltInTypeIndicator=false;
  this->flagUseFracInRationalLaTeX=true;
  this->flagForkingProcessAllowed=true;
  this->flagNoApproximationS=false;
  this->flagDefaultRulesWereTamperedWith=false;
  this->flagUsePredefinedWordSplits=true;
  this->flagPlotNoControls=true;
  this->flagPlotShowJavascriptOnly=false;
  this->flagHasGraphics=false;
  this->flagUseBracketsForIntervals=false;

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
  this->atomsNotInterpretedAsFunctions.Clear();
  this->atomsNotAllowingChainRule.Clear();
  this->atomsWhoseExponentsAreInterpretedAsFunctions.Clear();
  this->atomsThatMustNotBeCached.Clear();
  this->arithmeticOperations.Clear();
  this->knownFunctionsWithComplexRange.Clear();
  this->knownDoubleConstants.Clear();
  this->knownDoubleConstantValues.SetSize(0);
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.Clear();
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
}

void Calculator::init()
{ MacroRegisterFunctionWithName("Calculator::init");
  this->reset();

  this->theAtoms.SetExpectedSize(1000);
  this->FunctionHandlers.SetExpectedSize(1000);
  this->namedRules.SetExpectedSize(500);
  this->builtInTypes.SetExpectedSize(50);
  this->operationsComposite.SetExpectedSize(50);
  this->operationsCompositeHandlers.SetExpectedSize(50);

  this->formatVisibleStrings.flagExpressionIsFinal=true;

  //operation List is the very first operation. It signifies a non-atomic expression.
  //operation List is signified by the empty string
  //operation List must always have index 0.
  this->AddOperationNoRepetitionAllowed("");

  this->AddOperationNoRepetitionAllowed("=");
  this->AddOperationNoRepetitionAllowed(";");
  this->AddOperationNoRepetitionAllowed("if=");
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
  this->AddOperationNoRepetitionAllowed("\\binom");
  this->AddOperationNoRepetitionAllowed("\\sqrt");
  this->AddOperationNoRepetitionAllowed("\\in");
  this->AddOperationNoRepetitionAllowed("[]");
  this->AddOperationNoRepetitionAllowed("=:");
  this->AddOperationNoRepetitionAllowed("^");
  this->AddOperationNoRepetitionAllowed("\\circ");
  this->AddOperationNoRepetitionAllowed("\\geq");
  this->AddOperationNoRepetitionAllowed("\\leq");
  this->AddOperationNoRepetitionAllowed(">");
  this->AddOperationNoRepetitionAllowed("<");
  this->AddOperationNoRepetitionAllowed("==");
  this->AddOperationNoRepetitionAllowed("===");
  this->AddOperationNoRepetitionAllowed("\\cup");
  this->AddOperationNoRepetitionAllowed("\\sqcup");
  this->AddOperationNoRepetitionAllowed("\\cap");
  this->AddOperationNoRepetitionAllowed("Error");
  this->AddOperationNoRepetitionAllowed("Sequence");
  this->AddOperationNoRepetitionAllowed("Matrix");
  this->AddOperationNoRepetitionAllowed("Context");
  this->AddOperationNoRepetitionAllowed("|");
  this->AddOperationNoRepetitionAllowed("\"");
  this->AddOperationNoRepetitionAllowed("PolyVars");
  this->AddOperationNoRepetitionAllowed("DiffOpVars");
  this->AddOperationNoRepetitionAllowed("\\to");
  this->AddOperationNoRepetitionAllowed("if");
  this->AddOperationNoRepetitionAllowed("\\lim");
  this->AddOperationNoRepetitionAllowed("LogBase");
  this->AddOperationNoRepetitionAllowed("userInputTextBox");
  this->AddOperationNoRepetitionAllowed("\\int");

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
  this->AddOperationBuiltInType("MatrixRational");
  this->AddOperationBuiltInType("MatrixDouble");
  this->AddOperationBuiltInType("MatrixAlgebraic");
  this->AddOperationBuiltInType("MatrixTensorRational");
  this->AddOperationBuiltInType("MatrixRF");
  this->AddOperationBuiltInType("MatrixPolynomialRational");
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

  this->initOperationsInterpretedAsFunctionsMultiplicatively();

  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("{{}}");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("Variable");
  this->controlSequences.AddOnTop(this->theAtoms);//all operations defined up to this point are also control sequences
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("Expression");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("Integer");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("{}");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot(",");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot(".");
//  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\"");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\choose");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\frac");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cdot");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("_");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("(");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot(")");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("[");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("]");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("{");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("}");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot(":");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\varnothing");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("pi");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("infty");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("infinity");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("ln");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\ln");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("log");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("sin");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("cos");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("tan");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("cot");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("csc");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("sec");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("sqrt");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("arcsin");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("arccos");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("arctan");
  this->controlSequences.AddOnTopNoRepetition
  (this->knownOperationsInterpretedAsFunctionsMultiplicatively);
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("SequenceStatements");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("MakeSequence");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\setminus");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("$");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("MatrixEnd");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\begin");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\left");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\right");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("array");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("pmatrix");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\end");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\\\");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\displaystyle");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("&");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("%");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("NoFrac");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("ShowContext");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("NoLogarithmExponentShortcut");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogParsing");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogEvaluation");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("HidePolynomialDataStructure");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("NumberColors");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogRules");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogCache");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogFull");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("UseBracketForIntervals");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("DontUsePredefinedWordSplits");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("PlotShowJavascriptOnly");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("PlotDetails");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("LatexLink");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("UseLnInsteadOfLog");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("UseLnAbsInsteadOfLog");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("CalculatorStatus");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("FullTree");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("HideLHS");
  this->controlSequences.AddOnTopNoRepetitionMustBeNewCrashIfNot("EndProgram");

  this->initPredefinedStandardOperationsWithoutHandler();

  this->TotalNumPatternMatchedPerformed=0;
  this->initPredefinedStandardOperations();
  this->initPredefinedInnerFunctions();
  this->initCalculusTestingFunctions();
  this->initPredefinedOperationsComposite();
  this->initAtomsThatAllowCommutingOfArguments();
  this->initAtomsThatFreezeArguments();
  this->initAtomsNotGoodForChainRule();
  this->initBuiltInAtomsNotInterpretedAsFunctions();
  this->initBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  this->initOperationsThatAreKnownFunctions();
  this->initAtomsNonCacheable();
  this->initArithmeticOperations();
  this->initPredefinedWordSplits();
  this->initStringsThatSplitIfFollowedByDigit();
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

bool Calculator::ReplaceOXEXEXEXByE()
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(middleE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(7);
}

bool Calculator::ReplaceSqrtEXByEX()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& argument=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSqrt());
  newExpr.AddChildOnTop(this->ETwo());
  newExpr.AddChildOnTop(argument.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(1);
}

bool Calculator::ReplaceSqrtEXXByEXX()
{ int lastSize=(*this->CurrentSyntacticStacK).size;
  SyntacticElement& left=(*this->CurrentSyntacticStacK)[lastSize-4];
  SyntacticElement& argument=(*this->CurrentSyntacticStacK)[lastSize-3];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSqrt());
  newExpr.AddChildOnTop(this->ETwo());
  newExpr.AddChildOnTop(argument.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  (*this->CurrentSyntacticStacK)[lastSize-3]=(*this->CurrentSyntacticStacK)[lastSize-2];
  (*this->CurrentSyntacticStacK)[lastSize-2]=(*this->CurrentSyntacticStacK)[lastSize-1];
  (*this->CurrentSyntacticStacK).SetSize(lastSize-1);
  return true;
}

bool Calculator::ReplaceSqrtXEXByEX()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& argument=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSqrt());
  newExpr.AddChildOnTop(this->ETwo());
  newExpr.AddChildOnTop(argument.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceOXEXEByE()
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(4);
}

bool Calculator::ReplaceOXEXEXByEX()
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(4);
}

bool Calculator::ReplaceOXEXEXXByEXX()
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-7];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLastTwo(4);
}

void Calculator::LogPublicError(const std::string& theError)
{ if (this->errorsPublic.str()!="")
    this->errorsPublic << theError;
}

bool Calculator::DecreaseStackExceptLast(int decrease)
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

bool Calculator::DecreaseStackExceptLastTwo(int decrease)
{ if (decrease<=0)
    return true;
  if ((*this->CurrentSyntacticStacK).size-decrease<=0)
    crash << crash;
  //    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-2].IndexLastCharPlusOne=
  //    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].IndexLastCharPlusOne;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-2]=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease-1]=
  *this->CurrentSyntacticStacK->LastObject();
  (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-decrease);
  return true;
}

bool Calculator::ReplaceOXEEXByEX()
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr(*this);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(3);
}

bool Calculator::ReplaceOXXEXEXEXByE()
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
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(8);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceOXEXByEX()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::ReplaceOXEByE()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::ReplaceOEByE()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::ReplaceOEXByEX()
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceOEXByEX]";
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  middle.theData=newExpr;
  middle.controlIndex=this->conExpression();
  this->DecreaseStackExceptLast(1);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceXXXByCon(int theCon)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXByConCon(int con1, int con2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXXXXXByCon(int theCon)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(4);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXXXXXByCon]";
  return true;
}

bool Calculator::ReplaceXXXXXByConCon(int con1, int con2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=con1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool Calculator::ReplaceXXXXByConCon(int con1, int con2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXXByCon(int con1)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  this->DecreaseStackSetCharacterRangeS(3);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXXXXByCon]";
  return true;
}

bool Calculator::ReplaceXByCon(int theCon)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=theCon;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceXByEusingO(int theOperation)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=this->conExpression();
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.MakeAtom(theOperation, *this);
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceXByConCon(int con1, int con2)
{ (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size+1);
  (*this->CurrentSyntacticStacK).LastObject()->theData.reset(*this);
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=con2;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceOEXByE()
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceOEXByE]";
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  middle.theData=newExpr;
  middle.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::ReplaceXXByEmptyString()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  left.theData.AssignValue((std::string)"", *this);
  left.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceEOByE()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(right.controlIndex));
  newExpr.AddChildOnTop(left.theData);
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
    case '\t':
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
    case '|':
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
    case '\t':
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
    case '|':
    case '\"':
      return true;
    default:
      return false;
  }
}

bool Calculator::isInterpretedAsEmptySpace(const std::string& input)
{ if (input.size()!=1)
    return false;
  return this->isInterpretedAsEmptySpace(input[0]);
}

bool Calculator::isInterpretedAsEmptySpace(unsigned char input)
{ switch (input)
  {
  case '\n': //numerical value: 10
  case '~':
  case '\r': //numerical value: 13
  case '\t':
  case 160: //&nbsp
    return true;
  default: return false;
  }
  return false;
}

void Calculator::ParseFillDictionary(const std::string& input)
{ MacroRegisterFunctionWithName("Calculator::ParseFillDictionary");
  this->ParseFillDictionary(input, *this->CurrrentSyntacticSouP);
  SyntacticElement currentElement;
  currentElement.theData.reset(*this);
  currentElement.controlIndex=this->conEndProgram();
  (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
}

void Calculator::ParseFillDictionary(const std::string& input, List<SyntacticElement>& output)
{ MacroRegisterFunctionWithName("Calculator::ParseFillDictionary");
  std::string current;
  output.Reserve(input.size());
  output.SetSize(0);
  char LookAheadChar;
  SyntacticElement currentElement;
  int currentDigit;
  bool inQuotes=false;
  bool escapedInQuotes=false;
  bool escapingAllowed=true;
  for (unsigned i=0; i<input.size(); i++)
  { if (escapedInQuotes)
    { if (current.size()>0)
        current.resize(current.size()-1);
      escapedInQuotes=false;
      escapingAllowed=false;
    }
    current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
    { LookAheadChar=' ';
      if (inQuotes)
        LookAheadChar='"';
    }
    if (!inQuotes )
      if (this->isInterpretedAsEmptySpace(current))
        current=" ";
    bool shouldSplit=false;
    if (!inQuotes)
    { shouldSplit=
      (this->isLeftSeparator(current[0]) || this->isRightSeparator(LookAheadChar) || current==" ");
      if (MathRoutines::isADigit(LookAheadChar))
      { //stOutput << "DEBUG: lookahead digit, current: " << current;
        if (current[current.size()-1]=='\\' || this->stringsThatSplitIfFollowedByDigit.Contains(current))
        { shouldSplit=true;
          //stOutput << "DEBUG: shouldsplit true";
        }
      }
    }
    if (inQuotes)
    { if (escapingAllowed)
        if (current.size()>=1)
          if (current[current.size()-1]=='\\')
            if (LookAheadChar=='\\' || LookAheadChar=='"')
            { escapedInQuotes=true;
              continue;
            }
      shouldSplit=(LookAheadChar=='"');
    }
    if (current=="\"" && escapingAllowed)
    { inQuotes=!inQuotes;
      shouldSplit=true;
    }
    //stOutput << "<br>in quotes: "<< inQuotes << " current: " << current << " shouldsplit: " << shouldSplit;
    if (!shouldSplit)
    { escapingAllowed=true;
      continue;
    }
    bool mustInterpretAsVariable=false;
    if (inQuotes && (current!="\"" || !escapingAllowed))
      mustInterpretAsVariable=true;
    escapingAllowed=true;
//    stOutput << "must interpret as var: " << mustInterpretAsVariable;
    if (this->controlSequences.Contains(current) && !mustInterpretAsVariable)
    { currentElement.controlIndex=this->controlSequences.GetIndex(current);
      currentElement.theData.reset(*this);
      output.AddOnTop(currentElement);
    } else if (MathRoutines::isADigit(current, &currentDigit) && !mustInterpretAsVariable)
    { currentElement.theData.AssignValue<Rational>(currentDigit, *this);
      currentElement.controlIndex=this->conInteger();
      output.AddOnTop(currentElement);
    } else
    { currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
      currentElement.theData.MakeAtom(this->AddOperationNoRepetitionOrReturnIndexFirst(current), *this);
      output.AddOnTop(currentElement);
    // stOutput << "<br>Adding syntactic element " << currentElement.ToString(*this);
    }
    current="";
  }
}

int Calculator::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->theAtoms.GetIndex(this->controlSequences[controlIndex]);
}

int Calculator::GetExpressionIndex()
{ return this->controlSequences.GetIndex("Expression");
}

bool Calculator::ReplaceXXbyE()
{ this->ReplaceXXbyEX();
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXXXbyE()
{ this->ReplaceXXbyEX();
  return this->ReplaceXEXByE();
}

bool Calculator::ReplaCeOXdotsXbyEXdotsX(int numXs)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1-numXs];
  theElt.theData.MakeAtom(this->GetOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules)
  { std::stringstream out;
    out << "[Rule: Calculator::ReplaCeOXdotsXbyEXdotsX: " << numXs << "]";
    this->parsingLog+=out.str();
  }
  theElt.controlIndex=this->conExpression();
  return true;
}

bool Calculator::ReplaCeOXbyEX()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theElt.theData.MakeAtom(this->GetOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceOXbyEX]";
  return this->ReplaceXXbyEX();
}

bool Calculator::ReplaCeObyE()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  theElt.theData.MakeAtom(this->GetOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceObyE]";
  theElt.controlIndex=this->conExpression();
  return true;
}

bool Calculator::ReplaceXXbyEX()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theElt.controlIndex=this->conExpression();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXXbyEX]";
  return true;
}

bool Calculator::ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence()
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

bool Calculator::ReplaceXEXByE()
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3]=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceXEXByEcontainingOE(int inputOpIndex)
{ SyntacticElement& outputElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& inputElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXEXByEcontainingOE]";
  outputElt.theData.reset(*this, 2);
  outputElt.theData.AddChildAtomOnTop(inputOpIndex);
  outputElt.theData.AddChildOnTop(inputElt.theData);
  outputElt.controlIndex=this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
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

bool Calculator::ReplaceOOEEXbyEXpowerLike()
{ SyntacticElement& outerO = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& innerO = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  SyntacticElement& innerArg = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& outerArg = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceOOEEXbyEXpowerLike]";
  Expression newInnerE(*this), newFinalE(*this);
  newInnerE.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(innerO.controlIndex));
  newInnerE.AddChildOnTop(innerArg.theData);
  newFinalE.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(outerO.controlIndex));
  newFinalE.AddChildOnTop(newInnerE);
  newFinalE.AddChildOnTop(outerArg.theData);

  innerO.theData=newFinalE;
  innerO.controlIndex=this->conExpression();
  return this->DecreaseStackExceptLast(3);
}

bool Calculator::ReplaceCXByE()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
//  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
  return this->PopTopSyntacticStack();
}

bool Calculator::ReplaceCXByEX()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
//  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
  return true;
}

bool Calculator::ReplaceXdotsXByMatrixStart(int numXes)
{ SyntacticElement& currentElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXes];
  currentElt.dataList.SetExpectedSize(10);
  currentElt.dataList.SetSize(1);
  currentElt.dataList.LastObject()->MakeSequence(*this);
  currentElt.controlIndex=this->conMatrixStart();
  //stOutput << "DEBUG: calling: ReplaceXdotsXByMatrixStart. datalist last:"
  //<< currentElt.dataList.LastObject()->ToString() << "<hr>";
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXdotsXByMatrixStart]";
  return this->DecreaseStackSetCharacterRangeS(numXes-1);
}

bool Calculator::ReplaceMatrixXByE()
{ SyntacticElement& theMatElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  //stOutput << "<hr>DEBUG: START: Calculator::ReplaceMatrixXByE: "
  //<< theMatElt.theData.ToString();
  Matrix<Expression> theMat;
  int numRows=theMatElt.dataList.size;
  int numCols=0;
  for (int i=0; i<theMatElt.dataList.size; i++)
    numCols= MathRoutines::Maximum(theMatElt.dataList[i].size()-1, numCols);
  if (numCols>0 && numRows>0)
  { theMat.init(numRows, numCols);
    for (int i=0; i<numRows; i++)
      for (int j=0; j<numCols; j++)
      { if (j+1>=theMatElt.dataList[i].size())
        { theMat.elements[i][j].AssignValue(0,*this);
          continue;
        }
        theMat.elements[i][j]=theMatElt.dataList[i][j+1];
      }
    theMatElt.theData.AssignMatrixExpressions(theMat, *this, false);
  } else
    theMatElt.theData.MakeMatrix(*this);
  theMatElt.dataList.SetSize(0);
  theMatElt.controlIndex=this->conExpression();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceMatrixXByE]";
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceMatrixEXByMatrixNewRow()
{ SyntacticElement& theMatElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theMatElt.dataList.LastObject()->AddChildOnTop(theElt.theData);
  theMatElt.dataList.SetSize(theMatElt.dataList.size+1);
  theMatElt.dataList.LastObject()->MakeSequence(*this);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceMatrixEXByMatrixNewRow]";
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceMatrixEXByMatrix()
{ SyntacticElement& theMatElt=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& theElt=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theMatElt.dataList.LastObject()->AddChildOnTop(theElt.theData);
  //stOutput << "<hr>DEBUG: Calculator::ReplaceMatrixEXByMatrix: "
  //<< theElt.theData.ToString();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceMatrixEXByMatrix]";
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceMatrixEXByMatrixX()
{ SyntacticElement& theMatElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theMatElt.dataList.LastObject()->AddChildOnTop(theElt.theData);
  //stOutput << "<hr>DEBUG: Calculator::ReplaceMatrixEXByMatrixXXX: "
  //<< theElt.theData.ToString();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceMatrixEXByMatrixX]";
  return this->DecreaseStackExceptLast(1);
}

bool Calculator::ReplaceAXbyEX()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
//  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
//  stOutput << "replaceAbyE: " << theElt.theData.ToString();
  return true;
}

std::string Calculator::ToStringIsCorrectAsciiCalculatorString(const std::string& input)
{ std::stringstream out;
  HashedList<char, MathRoutines::hashChar> theBadChars;
/*  List<int> charCodes;
  for (unsigned i=0; i<input.size(); i++)
    charCodes.AddOnTop(input[i]);
  out << "DEBUG: char codes: " << charCodes.ToStringCommaDelimited();*/
  for (unsigned i=0; i<input.size(); i++)
    if (!this->isStandardCalculatorCharacter(input[i]))
      theBadChars.AddOnTopNoRepetition(input[i]);
  if (theBadChars.size>0)
  { out << "Non-ascii characters detected in your input, namely: "
    << theBadChars.ToStringCommaDelimited() << ", ";
    List<int> ListInt;
    ListInt =theBadChars;
    out << "with respective code numbers: " << ListInt.ToStringCommaDelimited() << ". ";
    out << " Perhaps you copy+pasted from webpage/pdf file or are using non-English keyboard setup? ";
  }
  return out.str();
}

bool Calculator::isStandardCalculatorCharacter(unsigned char input)
{ if (10<=input && input<=126)
    return true;
  if (this->isInterpretedAsEmptySpace(input))
    return true;
  return false;
}

bool Calculator::AllowsApplyFunctionInPreceding(const std::string& lookAhead)
{ return
  lookAhead!="{" && lookAhead!="_" &&
  lookAhead!="\\circ" && lookAhead!="{}" &&
  lookAhead!="$";
}

bool Calculator::ReplaceSequenceUXEYBySequenceZY(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& afterleft = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  left.theData.AddChildOnTop(right.theData);
  left.controlIndex=theControlIndex;
  afterleft=*this->CurrentSyntacticStacK->LastObject();
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceYXBySequenceX(int theControlIndex)
{ if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceYXBySequenceX]";
  return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, 1);
}

bool Calculator::ReplaceYBySequenceY(int theControlIndex)
{ if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceYBySequenceY]";
  return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, 0);
}

bool Calculator::ReplaceXXYBySequenceY(int theControlIndex)
{ if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXXYBySequenceY]";
  this->ReplaceYBySequenceY(theControlIndex);
  return this->ReplaceXXYByY();
}

bool Calculator::ReplaceXXYXBySequenceYX(int theControlIndex)
{ if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXXYXBySequenceYX]";
  this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, 1);
  return this->ReplaceXXYXByYX();
}

bool Calculator::ReplaceSequenceXEBySequence(int theControlIndex)
{ SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  left.theData.AddChildOnTop(right.theData);
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceSequenceXEBySequence]";
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int numXs)
{ SyntacticElement& main = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->opSequence());
  newExpr.AddChildOnTop(main.theData);
  main.theData=newExpr;
  main.controlIndex=theControlIndex;
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceYXdotsXBySequenceYXdotsX]";
  return true;
}

bool Calculator::ReplaceEXEBySequence(int theControlIndex)
{ SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSequence());
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEEByEusingO(int theControlIndex)
{ SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
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
{ SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
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

bool Calculator::ReplaceVbyVdotsVAccordingToPredefinedWordSplits()
{ MacroRegisterFunctionWithName("Calculator::ReplaceVbyVdotsVAccordingToPredefinedWordSplits");
  SyntacticElement& theE =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  const std::string& currentVar=this->theAtoms[theE.theData.theData];
  if (!this->predefinedWordSplits.Contains( currentVar))
    crash << "Predefined word splits array does not contain the variable: " << theE.theData.ToString()
    << ". This should not happen in the body of this function. " << crash;
  List<std::string>& theSplit=this->predefinedWordSplits.GetValueCreateIfNotPresent(currentVar);
  SyntacticElement newElt;
  this->PopTopSyntacticStack();
  *this << "Predefined symbol replacement: replacing "
  << currentVar << " with the sequence of symbols " << theSplit.ToStringCommaDelimited()
  << ". If you do not want such replacements to take place you should add the %DontUsePredefinedWordSplits option"
  << "  at the start of your input. "
  << " The predefined symbol replacements are used to guard the user from accidental typos such as confusing  "
  << " x y (the product of x and y) with xy (a single variable whose name contains the letters x and y). "
  ;

  for (int i=0; i<theSplit.size; i++)
  { newElt.theData.MakeAtom(this->AddOperationNoRepetitionOrReturnIndexFirst(theSplit[i]), *this);
    newElt.controlIndex=this->controlSequences.GetIndex(theSplit[i]);
    if (newElt.controlIndex==-1)
      newElt.controlIndex=this->conVariable();
    (*this->CurrentSyntacticStacK).AddOnTop(newElt);
  }
  return true;
}

bool Calculator::ReplaceEEXByEXusingO(int theControlIndex)
{ SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceEEXByEXusingO]";
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
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceEXXEXEXByEXusingO]";
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceUnderscoreEPowerEbyLimits()
{ SyntacticElement& bottom=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& top =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opLimitBoundary());
  newExpr.AddChildOnTop(bottom.theData);
  newExpr.AddChildOnTop(top.theData);
  (*this->CurrentSyntacticStacK)
  [(*this->CurrentSyntacticStacK).size-4].theData=newExpr;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4]
  .controlIndex=this->conExpression();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceUnderscoreEPowerEbyLimits]";
  return this->DecreaseStackSetCharacterRangeS(3);
}

bool Calculator::ReplacePowerEUnderScoreEbyLimits()
{ SyntacticElement& bottom=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& top =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  MathRoutines::swap(bottom, top);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplacePowerEUnderScoreEbyLimits]";
  return this->ReplaceUnderscoreEPowerEbyLimits();
}

bool Calculator::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Rational tempRat=left.theData.GetValue<Rational>();
  if (this->registerPositionAfterDecimalPoint==0)
  { tempRat*=10;
    tempRat+=right.theData.GetValue<Rational>();
  } else
  { Rational afterDecimal=right.theData.GetValue<Rational>();
    for (int i=0; i<this->registerPositionAfterDecimalPoint; i++)
      afterDecimal/=10;
    tempRat+=afterDecimal;
    this->registerPositionAfterDecimalPoint++;
  }
  left.theData.AssignValue(tempRat, *this);
  this->DecreaseStackSetCharacterRangeS(1);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceIntIntBy10IntPlusInt]";
  return true;
}

bool Calculator::ReplaceXEXEXByEusingO(int theControlIndex)
{ SyntacticElement& lefT = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(lefT.theData);
  newExpr.AddChildOnTop(right.theData);
  result.theData=newExpr;
  result.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(4);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXEXEXByEusingO]";
  return true;
}

bool Calculator::ReplaceEXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.Reserve(2);
  newExpr.AddChildAtomOnTop(this->opEndStatement());
  newExpr.AddChildOnTop(left.theData);
  left.theData=newExpr;
  left.controlIndex=this->conSequenceStatements();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceEXdotsXbySsXdotsX]";

//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceSsSsXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  if (!left.theData.StartsWith(this->opEndStatement()))
    crash << "This is a programming error: ReplaceSsSsXdotsXbySsXdotsX called but left expression is not EndStatement." << crash;
  left.theData.children.Reserve(left.theData.children.size+ right.theData.children.size-1);
  for (int i=1; i<right.theData.children.size; i++)
    left.theData.AddChildOnTop(right.theData[i]);
  left.controlIndex=this->conSequenceStatements();
  (*this->CurrentSyntacticStacK).PopIndexShiftDown((*this->CurrentSyntacticStacK).size-numDots-1);
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceSsSsXdotsXbySsXdotsX]";
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEByEusingO(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEXByEX()
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEOEXByEX()
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceEOEXByEX]";
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  middle=*(*this->CurrentSyntacticStacK).LastObject();
//  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEXByEXUsingO(int theOp)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceEOEXByEX]";
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(theOp);
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  middle=*(*this->CurrentSyntacticStacK).LastObject();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceEXEXByEXUsingO]";
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceO_2O_1E_3XbyEX()
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceO_2O_1E_3XbyEX]";
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
//  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  this->DecreaseStackExceptLast(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceXEEXByEXusingO(int inputOperation)
{ //stOutput << "<b>Here iam!</b>";
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left  =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(inputOperation);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  middle=*(*this->CurrentSyntacticStacK).LastObject();
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXEEXByEXusingO]";
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceXEEByEusingO(int inputOperation)
{ //stOutput << "<b>Here iam!</b>";
  SyntacticElement& right =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& left  =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(inputOperation);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
//  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "[Rule: Calculator::ReplaceXEEByEusingO]";
  return this->DecreaseStackSetCharacterRangeS(2);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
}

bool Calculator::isSeparatorFromTheLeftGeneral(const std::string& input)
{ return input=="{" || input=="(" || input=="[" ||
  input=="," || input==":" || input==";" ||
  input==" " || input=="Matrix" ||
  input=="&";
}


bool Calculator::isSeparatorFromTheLeftForInterval(const std::string& input)
{ return input=="," || input=="(" || input=="[" || input==":" || input==";" ||
  input=="=" || input== "==" || input=="\\\\" || input=="\\cup" || input=="\\cap" ||
  input=="\\end" || input=="&" || input=="EndProgram";
}

bool Calculator::isSeparatorFromTheRightGeneral(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input=="," || input==":" || input==";" ||
  input=="Matrix" ||
  input=="=" || input=="\\\\" ||
  input=="\\end" || input=="&" || input=="EndProgram";
}

bool Calculator::isSeparatorFromTheLeftForList(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" " ||
  input=="Matrix";
}

bool Calculator::isSeparatorFromTheRightForList(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" ||
  input=="Matrix" || input=="\\\\" || input=="\\end";
}

bool Calculator::isSeparatorFromTheLeftForDefinition(const std::string& input)
{ return input=="{" || input=="(" || input==";" || input=="," ||
  input=="SequenceStatements" || input==" " ||
  input=="or" ||
  input=="and";
}

bool Calculator::isSeparatorFromTheRightForDefinition(const std::string& input)
{ return input=="}" || input==")" || input==";" || input=="," ||
  input=="EndProgram" || input=="or" ||
  input=="and"
  ;
}

bool Calculator::AllowsTensorInPreceding(const std::string& lookAhead)
{ return this->AllowsTimesInPreceding(lookAhead) || lookAhead=="\\otimes";
}

bool Calculator::AllowsTimesInNext(const std::string& thePreceding)
{ if (thePreceding=="{}" || thePreceding=="^" || thePreceding=="\\sqrt" ||
      thePreceding=="_")
    return false;
  return true;
}

bool Calculator::AllowsTimesInPreceding(const SyntacticElement& thePreceding, const std::string& lookAhead)
{ //stOutput << "<hr><br><br>DEBUG: here i am0. Thepreceding: " << thePreceding.ToStringHumanReadable(*this, false)
  //<< "<br>lookahead: " << lookAhead;
  if (thePreceding.controlIndex==this->conExpression())
    if (thePreceding.theData.IsBuiltInAtom())
    { //stOutput << "DEBUG: here i am";
      return lookAhead=="+" || lookAhead=="-" || lookAhead=="*" || lookAhead=="/" ||
      lookAhead=="\\cup" ||
      lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
      lookAhead=="=" ||
      lookAhead==">" || lookAhead=="<" ||
      lookAhead=="\\leq" || lookAhead=="\\geq" ||
      lookAhead=="," || lookAhead==";" ||
      lookAhead==":" || lookAhead=="&" || lookAhead=="Matrix" || lookAhead=="\\" ||
      lookAhead=="or" ||
      lookAhead=="and" ||
      lookAhead=="EndProgram"
      ;
    }
  return this->AllowsTimesInPreceding(lookAhead);
}

bool Calculator::AllowsTimesInPreceding(const std::string& lookAhead)
{ return lookAhead=="+" || lookAhead=="-" || lookAhead=="*" || lookAhead=="/" ||
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.Contains(lookAhead) ||
  lookAhead=="\\frac" ||
  lookAhead=="Expression" ||  lookAhead== "Integer" || lookAhead=="\\cup" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead=="|" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="=" ||
  lookAhead==">" || lookAhead=="<" ||
  lookAhead=="\\geq" || lookAhead=="\\leq" ||
  lookAhead=="Variable" || lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="MatrixEnd" ||
  lookAhead=="or" ||
  lookAhead=="and" ||
  lookAhead=="\\" ||
  lookAhead=="sqrt" || lookAhead=="\\sqrt" || lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsPowerInPreceding(const std::string& lookAhead)
{ return lookAhead!="{}";
}

bool Calculator::AllowsPowerInNext(const std::string& lookBehind)
{ return lookBehind!="_";
}

bool Calculator::AllowsLimitProcessInPreceding(const std::string& lookAhead)
{ return lookAhead=="and" || lookAhead=="or" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="Matrix" ||
  lookAhead=="\\" ||
  lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsAndInPreceding(const std::string& lookAhead)
{ return lookAhead=="and" || lookAhead=="or" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="Matrix" ||
  lookAhead=="\\" ||
  lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsOrInPreceding(const std::string& lookAhead)
{ return lookAhead=="and" || lookAhead=="or" ||
  lookAhead=="(" || lookAhead=="[" ||
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="," || lookAhead==";" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="Matrix" ||
  lookAhead=="\\" ||
  lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsInInPreceding(const std::string& lookAhead)
{ return this->AllowsIfInPreceding(lookAhead)
  ;
}

bool Calculator::AllowsIfInPreceding(const std::string& lookAhead)
{ return
  lookAhead==")" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead=="," || lookAhead==";" ||
  lookAhead=="+" || lookAhead=="-" ||
  lookAhead=="/" || lookAhead=="*" ||
  lookAhead==":" || lookAhead=="&" || lookAhead=="Matrix" ||
  lookAhead=="\\" ||
  lookAhead=="EndProgram"
  ;
}

bool Calculator::AllowsPlusInPreceding(const std::string& lookAhead)
{ return lookAhead=="+" || lookAhead=="-" || lookAhead=="," ||
  lookAhead=="=" || lookAhead=="<" || lookAhead==">" ||
  lookAhead=="\\geq" || lookAhead== "\\leq" ||
  lookAhead==")" || lookAhead==";" || lookAhead=="]" || lookAhead=="}" ||
  lookAhead==":" || lookAhead=="," || lookAhead=="\\choose" ||
  lookAhead=="or" ||
  lookAhead=="and" ||
  lookAhead=="EndProgram" ||
  lookAhead=="&" || lookAhead=="MatrixEnd" || lookAhead=="\\";
  ;
}

bool Calculator::AllowsDivideInPreceding(const std::string& lookAhead)
{ return this->AllowsTimesInPreceding(lookAhead);
}

bool Calculator::ExtractExpressions(Expression& outputExpression, std::string* outputErrors)
{ MacroRegisterFunctionWithName("Calculator::ExtractExpressions");
  //std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->CurrentSyntacticStacK).Reserve((*this->CurrrentSyntacticSouP).size+this->numEmptyTokensStart);
  (*this->CurrentSyntacticStacK).SetSize(this->numEmptyTokensStart);
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
    errorLog << "Non-meaningful/empty input detected (spacebar, enter characters only?).";
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
      errorLog << "It simplifies to:<br> " << this->ToStringSyntacticStackHTMLTable(false);
    }
  } else
  { errorLog << "Syntax error: your command does not simplify to a single syntactic element. <br>";
    errorLog << "Instead it simplifies to:<br> " << this->ToStringSyntacticStackHTMLTable(false);
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
  const SyntacticElement& secondToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  const std::string& secondToLastS=this->controlSequences[secondToLastE.controlIndex];
  if (secondToLastS=="Integer" && lastS!="Integer" && lastS!=".")
  { this->registerPositionAfterDecimalPoint=0;
    if (this->flagLogSyntaxRules)
      this->parsingLog+= "[Rule: digit to number]";
    return this->ReplaceAXbyEX();
  }
  if (lastS==" " && secondToLastS=="\\" &&
      this->CurrentSyntacticStacK->size>=this->numEmptyTokensStart+2)
  { this->PopTopSyntacticStack();
    if (this->flagLogSyntaxRules)
      this->parsingLog+= "[Rule: remove \\ ]";
    return this->PopTopSyntacticStack();
  }
  if (lastS==" ")
  { if (this->flagLogSyntaxRules)
      this->parsingLog+= "[Rule: remove white space]";
    return this->PopTopSyntacticStack();
  }
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
    this->parsingLog+= "<hr>" + this->ToStringSyntacticStackHTMLTable(false);
  if (secondToLastS=="%" && lastS=="LogParsing")
  { this->parsingLog+= "<hr>" + this->ToStringSyntacticStackHTMLTable(false);
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
  if (secondToLastS=="%" && lastS=="UseBracketForIntervals")
  { this->flagUseBracketsForIntervals=true;
    this->Comments << "Using brackets for intervals. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="DontUsePredefinedWordSplits")
  { this->flagUsePredefinedWordSplits=false;
    this->Comments << "Using predefined word splits -for example xy is replaced by x y. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="WriteLatexDetails")
  { this->flagWriteLatexPlots=true;
    this->Comments << "Creating LaTeX files.";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="PlotShowJavascriptOnly")
  { this->flagPlotShowJavascriptOnly=true;
    this->Comments << "Plots show javascript only. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="PlotDetails")
  { this->flagPlotNoControls=false;
    this->Comments << "Plot details on. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="HidePolynomialDataStructure")
  { this->flagHidePolynomialBuiltInTypeIndicator=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogEvaluation")
  { this->flagLogEvaluatioN=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="UseLnInsteadOfLog")
  { this->Comments << "Displaying complex logarithms as ln. ";
    this->flagUseLnInsteadOfLog=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="UseLnAbsInsteadOfLog")
  { this->flagUseLnAbsInsteadOfLogForIntegrationNotation=true;
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
  if (secondToLastS=="%" && lastS=="LatexLink")
  { this->flagProduceLatexLink=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="NoLogarithmExponentShortcut")
  { this->atomsWhoseExponentsAreInterpretedAsFunctions.
    RemoveFirstOccurenceSwapWithLast((std::string) "\\log");
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
  if (this->flagUsePredefinedWordSplits)
    if (lastS=="Variable")
      if (this->predefinedWordSplits.Contains(this->theAtoms[lastE.theData.theData]))
        return this->ReplaceVbyVdotsVAccordingToPredefinedWordSplits();
  if (secondToLastS=="=" && lastS==":")
    return this->ReplaceXXByCon(this->conIsDenotedBy());
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->conApplyFunction());
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->conTimes());
  if (lastS=="\\varnothing")
    return this->ReplaceXByEusingO(this->opEmptySet());
  if (lastS=="\\circ" && secondToLastS!="{" && secondToLastS!="(" && secondToLastS!="^")
    return this->ReplaceXByCon(this->conApplyFunction());
//  if ( thirdToLastS=="{" && secondToLastS=="{}" && lastS=="}")
//    return this->ReplaceXXXByCon(this->conBindVariable());
  if (fifthToLastS=="{" && fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS=="}")
    return this->ReplaceXXVXdotsXbyE_BOUND_XdotsX(2);
  if (secondToLastS=="Variable" && ((lastS!="}" && lastS!=" ") || thirdToLastS!="{" || fourthToLastS!="{"))
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(1);
  if (thirdToLastS=="Variable" && ((secondToLastS!="}" && secondToLastS!=" ") || fourthToLastS!="{" || fifthToLastS!="{"))
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(2);
  if (fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS!="}" && lastS!=" ")
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(2);
  if ((secondToLastS=="\\left" || secondToLastS=="\\right") && lastS == "|")
    return this->ReplaceXYByY();
  if (secondToLastS=="\\left" && (lastS == "(" || lastS=="[" || lastS=="{"))
    return this->ReplaceXYByY();
  if (secondToLastS=="\\right" && (lastS == ")" || lastS=="]" || lastS=="}"))
    return this->ReplaceXYByY();
  if (lastS=="=" && secondToLastS=="=")
    return this->ReplaceXXByCon(this->conEqualEqual());
  if (lastS=="Integer" && secondToLastS=="Integer")
    return this->ReplaceIntIntBy10IntPlusInt();
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
//      && this->AllowsApplyFunctionInPreceding(lastS) )
//    return this->ReplaceEOEXByEX(secondToLastE.theData.format);
  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression" &&
      this->AllowsApplyFunctionInPreceding(lastS))
    return this->ReplaceEXEXByEX();
  //end of ambiguity.
  if (fifthToLastS=="\\lim" && fourthToLastS=="_" && thirdToLastS=="Expression" &&
      secondToLastS=="Expression" && this->AllowsLimitProcessInPreceding(lastS))
    return this->ReplaceOXEEXByEX();
  if (fourthToLastS=="Expression" && thirdToLastS=="_" &&
      secondToLastS=="Expression" && lastS!="_")
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="{"  && thirdToLastS=="{}" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXYYXByYY();
  if (secondToLastS=="Expression" && lastS=="!")
    return this->ReplaceEOByE();
  if (thirdToLastS=="\\binom" && secondToLastS=="Expression" &&
      lastS=="Expression" )
    return this->ReplaceXEEByEusingO(this->opBinom());

  if (secondToLastS=="Expression" && thirdToLastS=="^" &&
      fourthToLastS=="Expression" && this->AllowsPowerInPreceding(lastS) &&
      this->AllowsPowerInNext(fifthToLastS)
      )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="\\setminus" &&
      fourthToLastS=="Expression" && this->AllowsPowerInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="or" &&
      fourthToLastS=="Expression" && this->AllowsOrInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="and" &&
      fourthToLastS=="Expression" && this->AllowsAndInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="+" &&
      fourthToLastS=="Expression" && this->AllowsPlusInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (fifthToLastS=="|" && secondToLastS=="Expression" &&
      thirdToLastS=="+" && fourthToLastS=="Expression" && lastS=="|")
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" &&
      fourthToLastS=="Expression" && this->AllowsPlusInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" &&
      fourthToLastS=="Expression" && lastS=="|" && fifthToLastS=="|")
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" &&
      this->AllowsPlusInPreceding(lastS))
    return this->ReplaceOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" && lastS=="|" && fourthToLastS=="|" )
    return this->ReplaceOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="mod" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
//  if (lastS=="Expression" && secondToLastS=="\\in" && thirdToLastS=="Expression")
//    return this->ReplaceEOEByE();
  if (secondToLastS=="Expression" && thirdToLastS=="\\in" && fourthToLastS=="Expression" && this->AllowsInInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="\\choose" && fourthToLastS=="Expression")
    return this->ReplaceEXEXByEXUsingO(this->opBinom());
  if (secondToLastS=="Expression" && thirdToLastS=="\\otimes" && fourthToLastS=="Expression" && this->AllowsTensorInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="Expression" && fourthToLastS=="\\frac")
    return this->ReplaceXEEXByEXusingO(this->opDivide());
  if (secondToLastS=="Expression" && thirdToLastS=="\\times" && fourthToLastS=="Expression" && this->AllowsTimesInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="*" && fourthToLastS=="Expression" &&
      this->AllowsTimesInPreceding(secondToLastE, lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="/" && fourthToLastS=="Expression" && this->AllowsDivideInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="\\to" && fourthToLastS=="Expression" &&
      this->AllowsLimitProcessInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (this->atomsWhoseExponentsAreInterpretedAsFunctions.Contains(fifthToLastS) && fourthToLastS=="^" &&
      thirdToLastS=="Expression" && secondToLastS=="Expression" &&
      this->AllowsTimesInPreceding(lastS))
    return this->ReplaceOOEEXbyEXpowerLike();
  if (secondToLastS=="\\int" && lastS!="_")
    return this->ReplaCeOXbyEX();
  if (thirdToLastS=="\\int" && secondToLastS=="_" && lastS=="Expression")
    return this->ReplaCeOXdotsXbyEXdotsX(2);
  if (fourthToLastS=="\\int" && thirdToLastS=="_" &&
      secondToLastS=="Expression" && (lastS=="^" || lastS=="EndProgram"))
    return this->ReplaCeOXdotsXbyEXdotsX(3);
  if (fifthToLastS=="\\int" && fourthToLastS=="_" && thirdToLastS=="{}" &&
      secondToLastS=="^" && lastS=="{}")
  { this->PopTopSyntacticStack();
    this->PopTopSyntacticStack();
    this->PopTopSyntacticStack();
    this->ReplaCeOXbyEX();
    return this->PopTopSyntacticStack();
  }
  if (fifthToLastS=="LogBase" && fourthToLastS=="_" && thirdToLastS=="Expression" &&
      secondToLastS=="Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceOXEEXByEX();
  if (seventhToLastS=="\\sqrt" && sixthToLastS=="[" && fifthToLastS=="Expression" &&
      fourthToLastS=="]" && thirdToLastS=="Expression")
    return this->ReplaceOXEXEXXByEXX();
  if (this->AllowsTimesInNext(fourthToLastS) &&
      thirdToLastS=="Expression" && secondToLastS=="Expression" &&
      this->AllowsTimesInPreceding(secondToLastE, lastS))
  { this->ReplaceEEXByEXusingO(this->conTimes());
    Expression impliedFunApplication;
    if (this->outerTimesToFunctionApplication
        (*this, (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData, impliedFunApplication))
    { (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData=impliedFunApplication;
      if (this->flagLogSyntaxRules)
        this->parsingLog+= "[Rule: implied function application]";
    }
    return true;
  }
  if ((lastS==">" && secondToLastS=="=") ||
      (lastS=="=" && secondToLastS==">"))
    return this->ReplaceXXByCon(this->conGEQ());
  if (thirdToLastS=="|" && secondToLastS=="Expression" && lastS=="|")
    return this->ReplaceOEXByE();
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE();
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXEXByE();
  if (thirdToLastS=="(" && secondToLastS=="\\circ" && lastS==")")
    return this->ReplaCeOXbyEX();
  if (thirdToLastS=="{" && secondToLastS=="\\circ" && lastS=="}")
    return this->ReplaCeOXbyEX();
  if (secondToLastS=="^" && lastS=="\\circ")
    return this->ReplaCeObyE();
  if (this->flagUseBracketsForIntervals)
  { if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS=="]")
      return this->ReplaceXEXByEcontainingOE(this->opIntervalRightClosed());
    if (thirdToLastS=="[" && secondToLastS=="Expression" && lastS==")")
      return this->ReplaceXEXByEcontainingOE(this->opIntervalLeftClosed());
    if (thirdToLastS=="[" && secondToLastS=="Expression" && lastS=="]")
      return this->ReplaceXEXByEcontainingOE(this->opIntervalClosed());
    if (this->flagUseBracketsForIntervals)
      if (secondToLastE.theData.IsSequenceNElementS(2))
        if (this->isSeparatorFromTheLeftForInterval(fourthToLastS) &&
            thirdToLastS=="(" && secondToLastS=="Sequence" && lastS==")")
          return this->ReplaceXEXByEcontainingOE(this->opIntervalOpen());
  }
  if (thirdToLastS=="\"" && secondToLastS=="Expression" && lastS=="\"")
    return this->ReplaceXEXByEcontainingOE(this->opQuote());
  if (secondToLastS=="\"" && lastS=="\"")
    return this->ReplaceXXByEmptyString();
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (secondToLastS=="==" && lastS=="=")
    this->ReplaceXXByCon(this->conEqualEqualEqual());
  if (this->isSeparatorFromTheRightGeneral(lastS) && secondToLastS=="Expression" && thirdToLastS=="===" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS=="Expression" && thirdToLastS=="==" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS=="Expression" && thirdToLastS=="\\geq" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS=="Expression" && thirdToLastS=="\\leq" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS=="Expression" && thirdToLastS==">" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS=="Expression" && thirdToLastS=="<" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && fourthToLastS=="Expression" && thirdToLastS=="=" &&
      secondToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && fourthToLastS=="Expression" && thirdToLastS=="=:" &&
      secondToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEOEXByEX();
  if (lastS=="Sequence" && lastE.theData.size()==0 &&
      lastE.theData.theData==this->opLisT())
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("MakeSequence"));
  //else
  //  stOutput << "lastS is sequence but lastE is |" << lastE.theData.ToString() << "|";
  if (thirdToLastS=="\\sqrt" && secondToLastS=="Expression")
    return this->ReplaceSqrtEXByEX();
  if (fourthToLastS=="\\sqrt" && thirdToLastS=="Expression")
    return this->ReplaceSqrtEXXByEXX();
  if (fourthToLastS=="\\sqrt" && thirdToLastS == "{}" && secondToLastS=="Expression")
    return this->ReplaceSqrtXEXByEX();
  if (sixthToLastS=="\\sqrt" && fifthToLastS=="[" && fourthToLastS=="Expression" &&
     thirdToLastS=="]" && secondToLastS=="Expression")
    return this->ReplaceOXEXEXByEX();
  if (this->knownOperationsInterpretedAsFunctionsMultiplicatively.Contains(thirdToLastS) &&
      secondToLastS=="Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceOEXByEX();
  if (this->knownOperationsInterpretedAsFunctionsMultiplicatively.Contains(fourthToLastS) &&
      thirdToLastS=="^" && secondToLastS=="Expression" && this->isSeparatorFromTheRightGeneral(lastS))
    return this->ReplaceO_2O_1E_3XbyEX();
  if (this->knownOperationsInterpretedAsFunctionsMultiplicatively.Contains(fourthToLastS) &&
      thirdToLastS=="{}" && secondToLastS=="Expression" && this->AllowsApplyFunctionInPreceding(lastS))
    return this->ReplaceOXEXByEX();
  if (this->knownOperationsInterpretedAsFunctionsMultiplicatively.Contains(secondToLastS) &&
      this->isSeparatorFromTheRightGeneral(lastS))
    return this->ReplaCeOXbyEX();

  if (fourthToLastS=="\\log" && thirdToLastS=="_" && secondToLastS=="Expression")
    return this->ReplaceXYYYByConYYY(this->controlSequences.GetIndexIMustContainTheObject("LogBase"));
  //Some synonyms:
  if (lastS=="ln" || lastS=="log" || lastS=="\\ln")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\log"));
  if (lastS=="infinity" || lastS=="infty")
    return this->ReplaceXByEusingO(this->opInfinity());
  if (lastS=="pi")
    return this->ReplaceXByEusingO(this->opPi());
  if (lastS=="arcsin")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\arcsin"));
  if (lastS=="arccos")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\arccos"));
  if (lastS=="arctan")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\arctan"));
  if (lastS=="sin")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\sin"));
  if (lastS=="cos")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\cos"));
  if (lastS=="tan")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\tan"));
  if (lastS=="cot")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\cot"));
  if (lastS=="csc")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\csc"));
  if (lastS=="sec")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\sec"));
  if (lastS=="sqrt")
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\sqrt"));
  //end of some synonyms
  if (fourthToLastS=="Expression" && thirdToLastS=="\\cup" && secondToLastS== "Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Expression" && thirdToLastS=="\\cap" && secondToLastS== "Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Expression" && thirdToLastS=="\\sqcup" && secondToLastS== "Expression" && this->AllowsTimesInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (thirdToLastS=="if" && secondToLastS=="Expression" && this->AllowsIfInPreceding(lastS) )
    return this->ReplaceOEXByEX();
  if (fourthToLastS=="Sequence" && thirdToLastS=="," &&
      secondToLastS=="Expression" &&
      (lastS=="," || lastS==")" || lastS=="}" ||
       (this->flagUseBracketsForIntervals && lastS=="]")
      ))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conSequence());
  if ((thirdToLastS!="[" || this->flagUseBracketsForIntervals)&&
      secondToLastS=="Expression" && lastS==",")
    return this->ReplaceYXBySequenceX(this->conSequence());
  if (thirdToLastS=="MakeSequence" && secondToLastS=="{}" && lastS=="Expression")
    return this->ReplaceXXYBySequenceY(this->conExpression());
  if (fourthToLastS=="MakeSequence" && thirdToLastS=="{}" && secondToLastS=="Expression")
    return this->ReplaceXXYXBySequenceYX(this->conExpression());
  if (fifthToLastS=="Expression" && fourthToLastS== "{}" &&
      thirdToLastS=="(" &&
      secondToLastS=="Sequence" && lastS==")")
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  if (fifthToLastS=="Expression" && fourthToLastS== "{}" &&
      thirdToLastS=="{" &&
      secondToLastS=="Sequence" && lastS=="}")
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  if (secondToLastS=="Sequence" &&
      ((thirdToLastS=="(" && lastS==")")||
       (thirdToLastS=="{" && lastS=="}") ||
       (this->flagUseBracketsForIntervals &&
        ((thirdToLastS=="[" && lastS==")")||
         (thirdToLastS=="(" && lastS=="]")||
         (thirdToLastS=="[" && lastS=="]")
         ))
      ))
    return this->ReplaceXXbyEX();
  if (fourthToLastS=="\\begin" && thirdToLastS=="{" &&
      secondToLastS=="pmatrix" && lastS=="}")
    return this->ReplaceXdotsXByMatrixStart(4);
  if (fifthToLastS=="\\begin" && fourthToLastS=="{" &&
      thirdToLastS=="array" && secondToLastS=="}" && lastS=="Expression")
    return this->ReplaceXdotsXByMatrixStart(5);
  if (fourthToLastS=="\\end" && thirdToLastS=="{" &&
      (secondToLastS=="array"  || secondToLastS=="pmatrix") &&
      lastS=="}" )
    return this->ReplaceXXXXByCon(this->conMatrixEnd());
  if (lastS=="\\" && secondToLastS== "\\")
    return this->ReplaceXXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\\\"));
  if (lastS=="\\displaystyle")
    return this->PopTopSyntacticStack();
  if (thirdToLastS=="Matrix" && secondToLastS=="Expression" &&
      lastS=="&")
    return this->ReplaceMatrixEXByMatrix();
  if (thirdToLastS=="Matrix" && secondToLastS=="Expression" &&
      lastS=="MatrixEnd")
    return this->ReplaceMatrixEXByMatrixX();
  if (thirdToLastS=="Matrix" && secondToLastS=="Expression" &&
      lastS=="\\\\")
    return this->ReplaceMatrixEXByMatrixNewRow();
  if (secondToLastS=="Matrix" && lastS=="MatrixEnd")
    return this->ReplaceMatrixXByE();
  if (!this->flagUseBracketsForIntervals)
    if (fifthToLastS=="[" && fourthToLastS=="Expression" &&
        thirdToLastS=="," &&
        secondToLastS=="Expression" && lastS=="]")
      return this->ReplaceXEXEXByEusingO(this->conLieBracket());
  if (this->isSeparatorFromTheLeftForDefinition(eighthToLastS) &&
      seventhToLastS=="Expression" &&
      sixthToLastS==":" && fifthToLastS=="if" && fourthToLastS=="Expression" &&
      thirdToLastS=="=" && secondToLastS=="Expression" &&
      this->isSeparatorFromTheRightForDefinition(lastS))
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
  if ((thirdToLastS=="(" || thirdToLastS=="{" ) &&
      secondToLastS=="SequenceStatements" &&
      (lastS==")" || lastS== "}"))
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

