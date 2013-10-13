//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6_05cpp(__FILE__, "Calculator evaluation (general) routines. ");

StackMaintainerRules::StackMaintainerRules(CommandList* inputBoss)
{ this->theBoss=inputBoss;
  if (this->theBoss!=0)
    this->startingRuleStackSize=inputBoss->RuleStack.size;
}

StackMaintainerRules::~StackMaintainerRules()
{ if (this->theBoss!=0)
    this->theBoss->RuleStack.SetSize(this->startingRuleStackSize);
  this->theBoss=0;
}

bool CommandList::outerStandardFunction(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerStandardFunction");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsLisT())
    return false;
  const Expression& functionNameNode =input[0];
  if (functionNameNode.IsListNElementsStartingWithAtom())
  { const List<Function>* theHandlers=theCommands.GetOperationCompositeHandlers(functionNameNode[0].theData);
    if (theHandlers!=0)
      for (int i=0; i<theHandlers->size; i++)
        if ((*theHandlers)[i].theFunction(theCommands, input, output))
        { if (theCommands.flagLogEvaluatioN)
            theCommands.Comments << "<hr>Substitution, composite rule " << theCommands.GetOperations()[functionNameNode[0].theData]
            << ", " << i+1 << " out of " << theHandlers->size << "<br>";
          return true;
        }
  }
  if (!functionNameNode.IsAtoM())
    return false;
//  std::cout << "<br>Evaluating: " << input.ToString();
//  std::cout.flush();
  for (int i=0; i<theCommands.FunctionHandlers[functionNameNode.theData].size; i++)
    if (!theCommands.FunctionHandlers[functionNameNode.theData][i].flagIsInner)
    { Function& outerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (outerFun.theFunction(theCommands, input, output))
        if(output!=input)
        { output.CheckConsistency();
          if (theCommands.flagLogEvaluatioN)
            theCommands.Comments << "<hr>Substitution, outer rule " << theCommands.GetOperations()[functionNameNode.theData]
            << ", handler " << i+1 << " out of " << theCommands.FunctionHandlers[functionNameNode.theData].size << "<br>";
          return true;
        }
    } else
    { Function& innerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      //if (functionNameNode.ToString()=="+")
      //{ bool tempbool=true;
      //   std::cout << "<br>Here i am!";
      //}
      if (input.children.size>2)
      { //std::cout << "more than 2 children: " << input.Lispify();
        if (innerFun.inputFitsMyInnerType(input))
          if (innerFun.theFunction(theCommands, input, output))
          { output.CheckConsistency();
            if (theCommands.flagLogEvaluatioN)
              theCommands.Comments << "<hr>Substitution, inner rule " << theCommands.GetOperations()[functionNameNode.theData]
              << ", handler " << i+1 << " out of " << theCommands.FunctionHandlers[functionNameNode.theData].size << "<br>";
            return true;
          }
      } else
        if (innerFun.inputFitsMyInnerType(input[1]))
          if (innerFun.theFunction(theCommands, input[1], output))
          { output.CheckConsistency();
            if (theCommands.flagLogEvaluatioN)
              theCommands.Comments << "<hr>Substitution, inner rule " << theCommands.GetOperations()[functionNameNode.theData]
              << ", handler " << i+1 << " out of " << theCommands.FunctionHandlers[functionNameNode.theData].size << "<br>";
            return true;
          }
    }
  return false;
}

bool CommandList::ExpressionMatchesPattern(const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions, std::stringstream* theLog)
{ MacroRegisterFunctionWithName("CommandList::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.theBoss==this && input.theBoss==this))
  { std::cout << "This is a programming error. Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << thePattern.ToString() << " and the input is " << input.ToString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error might be in code preceding the stack loading. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
//  static int ExpressionMatchesPatternDebugCounter=-1;
  static const bool printLocalDebugInfo=false;
//  if (input.ToString()=="f{}((a)):=a+5")
//    printLocalDebugInfo=true;
  //ExpressionMatchesPatternDebugCounter++;
//  std::cout << " ExpressionMatchesPatternDebugCounter: " << ExpressionMatchesPatternDebugCounter;
//  printLocalDebugInfo=(ExpressionMatchesPatternDebugCounter>-1);
  if (printLocalDebugInfo)
  { std::cout << " <hr> current input: " << input.ToString() << "<br>current pattern: " << thePattern.ToString();
    std::cout << "<br> current matched expressions: " << matchedExpressions.ToString();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Max recursion depth of " << this->MaxRecursionDeptH << " exceeded whlie trying to match expression pattern "
    << thePattern.ToString() << " onto expression " << input.ToString();
    this->evaluationErrors.AddOnTop(out.str());
    return false;
  }
//  if (this->opDefine()==input.theOperation)
//    return false;
  int opVarB=this->opBind();
  if (thePattern.IsListStartingWithAtom(opVarB))
  { int indexLeft= matchedExpressions.theBoundVariables.GetIndex(thePattern);
    if (indexLeft==-1)
    { matchedExpressions.theBoundVariables.AddOnTop(thePattern);
      matchedExpressions.variableImages.AddOnTop(input);
      indexLeft=matchedExpressions.theBoundVariables.size-1;
    }
    if (matchedExpressions.variableImages[indexLeft]!=input)
      return false;
    if (printLocalDebugInfo)
      std::cout << "<br><b>Match!</b>";
    return true;
  }
  if (thePattern.theData!=input.theData || thePattern.children.size!=input.children.size )
    return false;
  for (int i=0; i<thePattern.children.size; i++)
    if (!(this->ExpressionMatchesPattern(thePattern[i], input[i], matchedExpressions, theLog)))
      return false;
  if (printLocalDebugInfo)
    std::cout << "<br><b>Match!</b>";
  return true;
}

bool CommandList::EvaluateExpression(const Expression& input, Expression& output, BoundVariablesSubstitution& bufferPairs, bool& outputIsFree)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::EvaluateExpression");
  if (this->flagLogFullTreeCrunching && this->RecursionDeptH<3)
  { this->Comments << "<br>";
    for (int i=0; i<this->RecursionDeptH; i++)
      this->Comments << "&nbsp&nbsp&nbsp&nbsp";
      //std::cout << "&nbsp&nbsp&nbsp&nbsp";
    this->Comments << "Evaluating " << input.Lispify() << " with rule stack of size " << this->RuleStack.size; // << this->RuleStack.ToString();
//    std::cout << "Evaluating " << input.Lispify() << " with rule stack of size " << this->RuleStack.size; // << this->RuleStack.ToString();
  }
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Recursion depth limit of " << this->MaxRecursionDeptH << " exceeded while evaluating expressions.";
    output.SetError(out.str(), *this);
    if (this->flagMaxRecursionErrorEncountered)
      this->evaluationErrors.AddOnTop(out.str());
    this->flagMaxRecursionErrorEncountered=true;
    this->flagAbortComputationASAP=true;
    return true;
  }
  output=input;
  if (output.IsError())
  { this->flagAbortComputationASAP=true;
    return true;
  }
  if (this->ExpressionStack.Contains(input))
  { std::stringstream errorStream;
    errorStream << "I think I have detected an infinite cycle: I am asked to reduce " << input.ToString()
    << " but I have already seen that expression in the expression stack. ";
    this->flagAbortComputationASAP=true;
    return output.SetError(errorStream.str(), *this);
  }
  this->ExpressionStack.AddOnTop(input);
  Expression theExpressionWithContext;
  theExpressionWithContext.reset(*this, 3);
  theExpressionWithContext.AddChildAtomOnTop(this->opSequence());
  theExpressionWithContext.AddChildValueOnTop(this->RuleContextIdentifier);
  theExpressionWithContext.AddChildOnTop(input);
  int indexInCache=this->cachedExpressions.GetIndex(theExpressionWithContext);
  if (indexInCache!=-1)
    output= this->imagesCachedExpressions[indexInCache];
  else if (this->cachedExpressions.size<this->MaxNumCachedExpressionPerContext)
  { this->cachedExpressions.AddOnTop(theExpressionWithContext);
    indexInCache=this->cachedExpressions.size-1;
    this->imagesCachedExpressions.SetSize(indexInCache+1);
    this->imagesCachedExpressions.LastObject()->reset(*this);
    this->imagesCachedExpressions.LastObject()->SetError("Error: not computed yet.", *this);
  }
  //reduction phase:
  outputIsFree=true;
  bool ReductionOcurred=true;
  int counterNumTransformations=0;
  Expression tempE;
  tempE.reset(*this);
  Expression beforePatternMatch;
  //std::cout << "<br>";
  //for (int i=0; i<this->RecursionDeptH; i++)
    //std::cout << "&nbsp;";
  //std::cout << "Evaluating " << input.ToString();
  //std::cout.flush();
  while (ReductionOcurred && !this->flagAbortComputationASAP)
  { StackMaintainerRules theRuleStackMaintainer(this);
    ReductionOcurred=false;
    counterNumTransformations++;
//    std::cout << " transforming " << output.ToString() << " at recursion depth " << this->RecursionDeptH;
//    std::cout.flush();

    //if (this->flagLogEvaluation && counterNumTransformations>1 )
    //{ this->Comments << "<br>input: " << input.ToString() << "->"
    //  << output.ToString();
    //}
    if (indexInCache!=-1)
      this->imagesCachedExpressions[indexInCache]=output;
//////------Handling naughty expressions------
    if (this->theGlobalVariableS->GetElapsedSeconds()!=0)
      if (this->theGlobalVariableS->GetElapsedSeconds()>this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2)
      { if (!this->flagTimeLimitErrorDetected)
          std::cout << "<br><b>Max allowed computational time is " << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2 << ";  so far, "
          << this->theGlobalVariableS->GetElapsedSeconds()-this->StartTimeEvaluationInSecondS << " have elapsed -> aborting computation ungracefully.</b>";
        this->flagTimeLimitErrorDetected=true;
        this->flagAbortComputationASAP=true;
        break;
      }
    if (counterNumTransformations>this->MaxAlgTransformationsPerExpression)
    { if (!this->flagMaxTransformationsErrorEncountered)
      { std::stringstream out;
        out << " While simplifying " << output.ToString(&this->formatVisibleStrings) << "<br>Maximum number of algebraic transformations of "
        << this->MaxAlgTransformationsPerExpression << " exceeded.";
        output.SetError(out.str(), *this);
        this->flagAbortComputationASAP=true;
        this->flagMaxTransformationsErrorEncountered=true;
      }
      break;
    }
//////------End of handling naughty expressions------
/////-------Evaluating children if the expression is not of built-in type-------
    //bool foundError=false;
    if (!output.IsBuiltInType())
      for (int i=0; i<output.children.size && !this->flagAbortComputationASAP; i++)
      { bool tempBool=true;
  //      bool debugBool=false;
  //      if (output[i].ToString()=="c");
  //        debugBool=true;
        if (this->EvaluateExpression(output[i], tempE, bufferPairs, tempBool))
          output.SetChilD(i, tempE);
        if (!tempBool)
          outputIsFree=false;
        if (output[i].IsError())
        { this->flagAbortComputationASAP=true;
          break;
        }
  /*      if (this->RecursionDeptH==1)
        { std::cout << "<hr>Considering whether "
          << output[i].Lispify() << " is rule-stack-worthy.";
          if (output.ToString()=="a:=b")
            std::cout << "<hr>a:=b is here<hr>";
        }*/
        if (output.IsListNElementsStartingWithAtom(this->opEndStatement()))
          if (output[i].IsListNElementsStartingWithAtom(this->opDefine()) || output[i].IsListNElementsStartingWithAtom(this->opDefineConditional()))
          { this->RuleStack.AddOnTop(output[i]);
            this->RuleContextIdentifier++;
           // std::cout << ".. added !!!!";
          }
      }
    if (this->flagAbortComputationASAP)
      break;
    //->/////-------Default operation handling-------
      //std::cout << "<br>got to standard functions";
    //std::cout.flush();
    if (this->outerStandardFunction(*this, output, tempE))
    { ReductionOcurred=true;
      if (this->flagLogEvaluatioN)
        this->Comments << CGI::GetHtmlMathSpanPure(output.ToString()) << "  ->  " << CGI::GetHtmlMathSpanPure(tempE.ToString())
        << "<br>" << output.ToStringFull() << "  ->  " << tempE.ToStringFull();
      output=tempE;
      continue;
    }
    //->/////-------Default operation handling end-------
    //if (foundError || !resultExpressionIsFree)
      //break;
/////-------Evaluating children end-------
/////-------User-defined pattern matching------
      //std::cout << "<br>got to custom rules";
    //std::cout.flush();
    for (int i=0; i<this->RuleStack.size && !this->flagAbortComputationASAP; i++)
    { Expression& currentPattern=this->RuleStack[i];
      this->TotalNumPatternMatchedPerformed++;
      bufferPairs.reset();
      //std::cout << "<br>Checking whether "
      //<< output.ToString() << " matches " << currentPattern.ToString();
    //std::cout.flush();

      if (this->flagLogEvaluatioN)
        beforePatternMatch=output;
      if(this->ProcessOneExpressionOnePatternOneSub(currentPattern, output, bufferPairs, &this->Comments, this->flagLogPatternMatching))
      { ReductionOcurred=true;
        if (this->flagLogEvaluatioN)
          this->Comments << CGI::GetHtmlMathSpanPure(beforePatternMatch.ToString()) << "  ->  "
          << CGI::GetHtmlMathSpanPure(output.ToString()) << "<br>" << beforePatternMatch.ToStringFull() << "  ->  " << output.ToStringFull();
        break;
      }
    }
/////-------User-defined pattern matching end------
  }
  this->ExpressionStack.RemoveIndexSwapWithLast(this->ExpressionStack.size-1);
  if (output.IsListStartingWithAtom(this->opBind()))
    outputIsFree=false;
  if (this->flagLogFullTreeCrunching && this->RecursionDeptH<3)
  { this->Comments << "<br>";
    for (int i=0; i<this->RecursionDeptH; i++)
      this->Comments << "&nbsp&nbsp&nbsp&nbsp";
    this->Comments << "to get: " << output.Lispify();
  }

  return true;
}

Expression* CommandList::PatternMatch
(const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, const Expression* condition, std::stringstream* theLog, bool logAttempts)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::PatternMatch");
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of "
    << this->MaxRecursionDeptH << " was exceeded";
    theExpression.SetError(out.str(), *this);
    return 0;
  }
//  if (theExpression.ToString()=="f{}((a)):=a+5")
//  { std::cout << "!here";
//  }
  thePattern.CheckInitialization();
  theExpression.CheckInitialization();
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, theLog))
    return 0;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>found pattern: " << theExpression.ToString() << " -> "
    << thePattern.ToString() << " with " << bufferPairs.ToString();
  if (condition==0)
    return &theExpression;
  Expression tempExp=*condition;
  tempExp.CheckInitialization();
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.ToString();
  this->SpecializeBoundVars(tempExp, bufferPairs);
  tempExp.CheckInitialization();
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specialized condition: " << tempExp.ToString() << "; evaluating...";
  BoundVariablesSubstitution tempPairs;
  bool isFree;
  Expression conditionResult;
  this->EvaluateExpression(tempExp, conditionResult, tempPairs, isFree);
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>The evaluated specialized condition: " << conditionResult.ToString() << "; evaluating...";
  if (conditionResult.IsEqualToOne())
    return &theExpression;
  return 0;
}

void CommandList::SpecializeBoundVars(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (toBeSubbedIn.IsListOfTwoAtomsStartingWith(this->opBind()))
  { int indexMatching= matchedPairs.theBoundVariables.GetIndex(toBeSubbedIn);
    if (indexMatching!=-1)
    { toBeSubbedIn=matchedPairs.variableImages[indexMatching];
      //this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
      return;
    }
  }
  Expression subbedE;
  for (int i=0; i<toBeSubbedIn.children.size; i++)
  { subbedE=toBeSubbedIn[i];
    this->SpecializeBoundVars(subbedE, matchedPairs);
    toBeSubbedIn.SetChilD(i, subbedE);
  }
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
(Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, std::stringstream* theLog, bool logAttempts)
{ MacroRegisterFunctionWithName("CommandList::ProcessOneExpressionOnePatternOneSub");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (!thePattern.IsListNElementsStartingWithAtom(this->opDefine(), 3) && !thePattern.IsListNElementsStartingWithAtom(this->opDefineConditional(), 4))
    return false;
  if (theLog!=0 && logAttempts)
  { (*theLog) << "<hr>attempting to reduce expression " << theExpression.ToString();
    (*theLog) << " by pattern " << thePattern.ToString();
  }
  theExpression.CheckInitialization();
  const Expression& currentPattern=thePattern[1];
  const Expression* theCondition=0;
  bool isConditionalDefine=
  thePattern.IsListNElementsStartingWithAtom(this->opDefineConditional(), 4);
  if (isConditionalDefine)
    theCondition=&thePattern[2];
  Expression* toBeSubbed=this->PatternMatch
  (currentPattern, theExpression, bufferPairs, theCondition, theLog, logAttempts);
  if (toBeSubbed==0)
    return false;
  if (theLog!=0 && logAttempts)
    *theLog << "<br><b>found a match!</b>";
  if (isConditionalDefine)
    *toBeSubbed=thePattern[3];
  else
    *toBeSubbed=thePattern[2];
  this->SpecializeBoundVars(*toBeSubbed, bufferPairs);
  return true;
}

void CommandList::EvaluateCommands()
{ MacroRegisterFunctionWithName("CommandList::EvaluateCommands");
  std::stringstream out;
  BoundVariablesSubstitution thePairs;

//  this->theLogs.resize(this->theCommands.size());
//this->ToString();
  //std::stringstream comments;
  if (this->syntaxErrors!="")
  { out << "<hr><b>Syntax errors encountered</b><br>";
    out << this->syntaxErrors;
    out << "<hr>";
  }
//  std::cout
//  << "Starting expression: " << this->theProgramExpression.ToString()
//  << "<hr>";
  Expression StartingExpression=this->theProgramExpression;
  this->flagAbortComputationASAP=false;
  bool tempBool;
  this->Comments.clear();
  this->EvaluateExpression(this->theProgramExpression, this->theProgramExpression, thePairs, tempBool);
  if (this->RecursionDeptH!=0)
  { std::cout << "This is a programming error: the starting recursion depth "
    << "before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->theGlobalVariableS->theDefaultFormat.flagMakingExpressionTableWithLatex=true;
  this->theGlobalVariableS->theDefaultFormat.flagUseLatex=true;
  this->theGlobalVariableS->theDefaultFormat.flagExpressionIsFinal=true;
  this->theGlobalVariableS->theDefaultFormat.flagExpressionNewLineAllowed=true;
  if (!this->flagDisplayFullExpressionTree)
    out << this->theProgramExpression.ToString(&this->theGlobalVariableS->theDefaultFormat, &StartingExpression);
  else
  { out << "<hr>Input:<br> " << StartingExpression.ToStringFull() << "<hr>"
    << "Output:<br>" << this->theProgramExpression.ToStringFull();
  }
  this->outputString=out.str();
  std::stringstream commentsStream;
  if (this->theObjectContainer.theAlgebraicClosure.theBasisMultiplicative.size>1)
    commentsStream << "<b>Algebraic number closure status. </b><br>" << this->theObjectContainer.theAlgebraicClosure.ToString() << "<hr>";
  if (this->Comments.str()!="")
    commentsStream << "<b>Comments.</b><br><span>" << this->Comments.str() << "</span>";

  this->outputCommentsString=commentsStream.str();
}
