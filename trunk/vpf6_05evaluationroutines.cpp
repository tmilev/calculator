//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6_05cpp(__FILE__, "Calculator core evaluation engine. ");

StackMaintainerRules::StackMaintainerRules(Calculator* inputBoss)
{ this->theBoss=inputBoss;
  if (this->theBoss!=0)
    this->startingRuleStackSize=inputBoss->RuleStack.size;
}

StackMaintainerRules::~StackMaintainerRules()
{ if (this->theBoss!=0)
    this->theBoss->RuleStack.SetSize(this->startingRuleStackSize);
  this->theBoss=0;
}

std::string Calculator::ToStringFunctionHandlers()
{ MacroRegisterFunctionWithName("Calculator::ToStringFunctionHandlers");
  std::stringstream out;
  int numOpsHandled=0;
  int numHandlers=0;
  int numInnerHandlers=0;
  for (int i=0; i<this->theAtoms.size; i++)
  { if (this->FunctionHandlers[i].size!=0)
      numOpsHandled++;
    numHandlers+=this->FunctionHandlers[i].size;
    for (int j=0; j<this->FunctionHandlers[i].size; j++)
      if (this->FunctionHandlers[i][j].flagIsInner)
        numInnerHandlers++;
  }
  out << "\n <b> " << numOpsHandled << "  built-in atoms are handled by a total of " << numHandlers << " handler functions ("
  << numInnerHandlers << " inner and " << numHandlers-numInnerHandlers << " outer).</b><br>\n";
  bool found=false;
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  for (int i=0; i<this->theAtoms.size; i++)
  { int indexCompositeHander=this->operationsComposite.GetIndex(this->theAtoms[i]);
    int totalDirectHandlers=this->FunctionHandlers[i].size;

    if (this->FunctionHandlers[i].size>0)
      for (int j=0; j<this->FunctionHandlers[i].size; j++)
        if (this->FunctionHandlers[i][j].flagIamVisible)
        { if (found)
            out << "<br>\n";
          found=true;
          out << openTag2 << this->theAtoms[i] << closeTag2;
          if (totalDirectHandlers>1)
            out << " (" << j+1 << " out of " << totalDirectHandlers << ")";
          out << "\n" << this->FunctionHandlers[i][j].GetString(*this);
        }
    if (indexCompositeHander!=-1)
      for (int j=0; j<this->operationsCompositeHandlers[indexCompositeHander].size; j++)
        if (this->operationsCompositeHandlers[indexCompositeHander][j].flagIamVisible)
        { if (found)
            out << "<br>\n";
          found=true;
          out << openTag2 << this->theAtoms[i] << closeTag2;
          out << " (" << j+1 << " out of " << this->operationsCompositeHandlers[indexCompositeHander].size << " composite handlers)";
          out << "\n" << this->operationsCompositeHandlers[indexCompositeHander][j].GetString(*this);
        }
  }
  return out.str();
}

bool Calculator::outerStandardFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerStandardFunction");
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
  if (!functionNameNode.IsAtom())
    return false;
//  stOutput << "<br>Evaluating: " << input.ToString();
//  stOutput.flush();
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
      //   stOutput << "<br>Here i am!";
      //}
      if (input.children.size>2)
      { //stOutput << "more than 2 children: " << input.Lispify();
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

bool Calculator::ExpressionMatchesPattern(const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions, std::stringstream* theLog)
{ MacroRegisterFunctionWithName("Calculator::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.theBoss==this && input.theBoss==this))
    crash << "This is a programming error. Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << thePattern.ToString() << " and the input is " << input.ToString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error might be in code preceding the stack loading. "
    << crash;
//  static int ExpressionMatchesPatternDebugCounter=-1;
  static const bool printLocalDebugInfo=false;
//  if (input.ToString()=="f{}((a)):=a+5")
//    printLocalDebugInfo=true;
  //ExpressionMatchesPatternDebugCounter++;
//  stOutput << " ExpressionMatchesPatternDebugCounter: " << ExpressionMatchesPatternDebugCounter;
//  printLocalDebugInfo=(ExpressionMatchesPatternDebugCounter>-1);
  if (printLocalDebugInfo)
  { stOutput << " <hr> current input: " << input.ToString() << "<br>current pattern: " << thePattern.ToString();
    stOutput << "<br> current matched expressions: " << matchedExpressions.ToString();
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
      stOutput << "<br><b>Match!</b>";
    return true;
  }
  if (thePattern.theData!=input.theData || thePattern.children.size!=input.children.size )
    return false;
  for (int i=0; i<thePattern.children.size; i++)
    if (!(this->ExpressionMatchesPattern(thePattern[i], input[i], matchedExpressions, theLog)))
      return false;
  if (printLocalDebugInfo)
    stOutput << "<br><b>Match!</b>";
  return true;
}

bool Calculator::EvaluateExpression(const Expression& input, Expression& output, BoundVariablesSubstitution& bufferPairs, bool& outputIsFree)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::EvaluateExpression");
  if (this->flagLogFullTreeCrunching && this->RecursionDeptH<3)
  { this->Comments << "<br>";
    for (int i=0; i<this->RecursionDeptH; i++)
      this->Comments << "&nbsp&nbsp&nbsp&nbsp";
      //stOutput << "&nbsp&nbsp&nbsp&nbsp";
    this->Comments << "Evaluating " << input.Lispify() << " with rule stack of size " << this->RuleStack.size; // << this->RuleStack.ToString();
//    stOutput << "Evaluating " << input.Lispify() << " with rule stack of size " << this->RuleStack.size; // << this->RuleStack.ToString();
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
  //////////////////////////////
  //  stOutput << "temporary check comment me out";
  //  this->ExpressionStack.GrandMasterConsistencyCheck();
  //  input.CheckConsistency();
  //  input.HashFunction();
  //////////////////////////////
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
  //stOutput << "<br>";
  //for (int i=0; i<this->RecursionDeptH; i++)
    //stOutput << "&nbsp;";
  //stOutput << "Evaluating " << input.ToString();
  //stOutput.flush();
  while (ReductionOcurred && !this->flagAbortComputationASAP)
  { StackMaintainerRules theRuleStackMaintainer(this);
    ReductionOcurred=false;
    counterNumTransformations++;
//    stOutput << "<hr>transforming " << output.ToString() << " at recursion depth " << this->RecursionDeptH;
//    stOutput.flush();

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
          stOutput << "<br><b>Max allowed computational time is " << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2 << ";  so far, "
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
    if (!output.IsFrozen())
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
        { stOutput << "<hr>Considering whether "
          << output[i].Lispify() << " is rule-stack-worthy.";
          if (output.ToString()=="a:=b")
            stOutput << "<hr>a:=b is here<hr>";
        }*/
        if (output.IsListNElementsStartingWithAtom(this->opEndStatement()))
          if (output[i].IsListNElementsStartingWithAtom(this->opDefine()) || output[i].IsListNElementsStartingWithAtom(this->opDefineConditional()))
          { this->RuleStack.AddOnTop(output[i]);
            this->RuleContextIdentifier++;
           // stOutput << ".. added !!!!";
          }
      }
    if (this->flagAbortComputationASAP)
      break;
    //->/////-------Default operation handling-------
      //stOutput << "<br>got to standard functions";
    //stOutput.flush();
    if (this->outerStandardFunction(*this, output, tempE))
    { ReductionOcurred=true;
      if (this->flagLogEvaluatioN)
        this->Comments << CGI::GetMathMouseHover(output.ToString()) << "  ->  " << CGI::GetMathMouseHover(tempE.ToString())
        << "<br>" << output.ToStringSemiFull() << "  ->  " << tempE.ToStringSemiFull();
      output=tempE;
      continue;
    }
    //->/////-------Default operation handling end-------
    //if (foundError || !resultExpressionIsFree)
      //break;
/////-------Evaluating children end-------
/////-------User-defined pattern matching------
      //stOutput << "<br>got to custom rules";
    //stOutput.flush();
    for (int i=0; i<this->RuleStack.size && !this->flagAbortComputationASAP; i++)
    { Expression& currentPattern=this->RuleStack[i];
      this->TotalNumPatternMatchedPerformed++;
      bufferPairs.reset();
      //stOutput << "<br>Checking whether "
      //<< output.ToString() << " matches " << currentPattern.ToString();
    //stOutput.flush();

      if (this->flagLogEvaluatioN)
        beforePatternMatch=output;
      if(this->ProcessOneExpressionOnePatternOneSub(currentPattern, output, bufferPairs, &this->Comments, this->flagLogPatternMatching))
      { ReductionOcurred=true;
        if (this->flagLogEvaluatioN)
          this->Comments << CGI::GetMathSpanPure(beforePatternMatch.ToString()) << "  ->  "
          << CGI::GetMathSpanPure(output.ToString()) << "<br>" << beforePatternMatch.ToStringSemiFull() << "  ->  " << output.ToStringSemiFull();
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

Expression* Calculator::PatternMatch
(const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, const Expression* condition, std::stringstream* theLog, bool logAttempts)
{ MacroRegisterFunctionWithName("Calculator::PatternMatch");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of " << this->MaxRecursionDeptH << " was exceeded";
    theExpression.SetError(out.str(), *this);
    return 0;
  }
//  if (theExpression.ToString()=="f{}((a)):=a+5")
//  { stOutput << "!here";
//  }
  thePattern.CheckInitialization();
  theExpression.CheckInitialization();
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, theLog))
    return 0;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>found pattern: " << theExpression.ToString() << " -> " << thePattern.ToString() << " with " << bufferPairs.ToString();
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

void Calculator::SpecializeBoundVars(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs)
{ MacroRegisterFunctionWithName("Calculator::SpecializeBoundVars");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
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

bool Calculator::ProcessOneExpressionOnePatternOneSub
(Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, std::stringstream* theLog, bool logAttempts)
{ MacroRegisterFunctionWithName("Calculator::ProcessOneExpressionOnePatternOneSub");
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

bool Calculator::ParseAndExtractExpressions
(const std::string& theInputString, Expression& outputExp, List<SyntacticElement>& outputSynSoup, List<SyntacticElement>& outputSynStack, std::string* outputSynErrors)
{ MacroRegisterFunctionWithName("Calculator::ParseAndExtractExpressions");
  this->CurrentSyntacticStacK=&outputSynStack;
  this->CurrrentSyntacticSouP=&outputSynSoup;
  this->ParseFillDictionary(theInputString);
  bool result=this->ExtractExpressions(outputExp, outputSynErrors);
  this->CurrentSyntacticStacK=&this->syntacticStacK;
  this->CurrrentSyntacticSouP=&this->syntacticSouP;
  return result;
}

void Calculator::Evaluate(const std::string& theInput)
{ MacroRegisterFunctionWithName("Calculator::Evaluate");
  if (this->theGlobalVariableS==0)
  { this->outputString= "This is a programming error: commandList not initialized properly. Please report this bug. ";
    return;
  }
  this->StartTimeEvaluationInSecondS=this->theGlobalVariableS->GetElapsedSeconds();
  this->inputString=theInput;
  this->ParseAndExtractExpressions(theInput, this->theProgramExpression, this->syntacticSouP, this->syntacticStacK, & this->syntaxErrors);
  this->EvaluateCommands();
}

void Calculator::EvaluateCommands()
{ MacroRegisterFunctionWithName("Calculator::EvaluateCommands");
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
//  stOutput
//  << "Starting expression: " << this->theProgramExpression.ToString()
//  << "<hr>";
  Expression StartingExpression=this->theProgramExpression;
//  stOutput << "comment me out when done with debugging";
//  StartingExpression.HashFunction();
  this->flagAbortComputationASAP=false;
  bool tempBool;
  this->Comments.clear();
  this->EvaluateExpression(this->theProgramExpression, this->theProgramExpression, thePairs, tempBool);
  if (this->RecursionDeptH!=0)
    crash << "This is a programming error: the starting recursion depth before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << "." << crash;
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
