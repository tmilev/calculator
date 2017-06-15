//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6_05cpp(__FILE__, "Calculator core evaluation engine. ");

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
  out << "\n <b> " << numOpsHandled << " built-in atoms are handled by a total of " << numHandlers << " handler functions ("
  << numInnerHandlers << " inner and " << numHandlers-numInnerHandlers << " outer).</b><br>\n";
  bool found=false;
  List<std::string> atomsSorted=this->theAtoms;
  List<int> theIndices;
  theIndices.SetSize(this->theAtoms.size);
  for (int i=0; i<theIndices.size; i++)
    theIndices[i]=i;
  atomsSorted.QuickSortAscending(0, &theIndices);

  for (int k=0; k<this->theAtoms.size; k++)
  { int theAtomIndex=theIndices[k];
    int indexCompositeHander=this->operationsComposite.GetIndex(this->theAtoms[theAtomIndex]);
    if (this->FunctionHandlers[theAtomIndex].size>0)
      for (int j=0; j<this->FunctionHandlers[theAtomIndex].size; j++)
        if (this->FunctionHandlers[theAtomIndex][j].flagIamVisible)
        { if (found)
            out << "<br>\n";
          found=true;
          out << this->FunctionHandlers[theAtomIndex][j].ToStringFull();
        }
    if (indexCompositeHander!=-1)
      for (int j=0; j<this->operationsCompositeHandlers[indexCompositeHander].size; j++)
        if (this->operationsCompositeHandlers[indexCompositeHander][j].flagIamVisible)
        { if (found)
            out << "<br>\n";
          found=true;
          out << this->operationsCompositeHandlers[indexCompositeHander][j].ToStringFull();
        }
  }
  return out.str();
}

const Expression& Calculator::EZero()
{ if (this->frequentConstantZero.owner==0)
    this->frequentConstantZero.AssignValue(0,*this);
  return this->frequentConstantZero;
}

const Expression& Calculator::EOne()
{ if (this->frequentConstantOne.owner==0)
    this->frequentConstantOne.AssignValue(1,*this);
  return this->frequentConstantOne;
}

const Expression& Calculator::EMOne()
{ if (this->frequentConstantMinusOne.owner==0)
    this->frequentConstantMinusOne.AssignValue(-1,*this);
  return this->frequentConstantMinusOne;
}

const Expression& Calculator::EFour()
{ if (this->frequentConstantFour.owner==0)
    this->frequentConstantFour.AssignValue(4,*this);
  return this->frequentConstantFour;
}

const Expression& Calculator::ETwo()
{ if (this->frequentConstantTwo.owner==0)
    this->frequentConstantTwo.AssignValue(2,*this);
  return this->frequentConstantTwo;
}

const Expression& Calculator::EMHalf()
{ if (this->frequentConstantMinusHalf.owner==0)
    this->frequentConstantMinusHalf.AssignValue(Rational(-1,2),*this);
  return this->frequentConstantMinusHalf;
}

void Calculator::DoLogEvaluationIfNeedBe(Function& inputF)
{ if (!this->flagLogEvaluatioN)
    return;
  *this << "<hr>Built-in substitution: " << inputF.ToStringSummary()
  << "<br>" << theGlobalVariables.GetElapsedSeconds()-this->LastLogEvaluationTime
  << " second(s) since last log entry. "
  << "Rule stack id: "
  << this->RuleStackCacheIndex << ", stack size: " << this->RuleStack.size();
  this->LastLogEvaluationTime=theGlobalVariables.GetElapsedSeconds();
}

bool Calculator::outerStandardFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerStandardFunction");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsLisT())
    return false;
  const Expression& functionNameNode =input[0];
  if (functionNameNode.StartsWith())
  { const List<Function>* theHandlers=theCommands.GetOperationCompositeHandlers(functionNameNode[0].theData);
    if (theHandlers!=0)
      for (int i=0; i<theHandlers->size; i++)
        if (!((*theHandlers)[i].flagDisabledByUser))
          if ((*theHandlers)[i].theFunction(theCommands, input, output))
          { theCommands.DoLogEvaluationIfNeedBe((*theHandlers)[i]);
            return true;
          }
  }
  if (!functionNameNode.IsAtom())
    return false;
//  stOutput << "<br>Evaluating: " << input.ToString() << "<br>Lispified: " << input.ToStringSemiFull();
  for (int i=0; i<theCommands.FunctionHandlers[functionNameNode.theData].size; i++)
    if (!theCommands.FunctionHandlers[functionNameNode.theData][i].flagIsInner)
    { Function& outerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (outerFun.flagDisabledByUser)
        continue;
      if (outerFun.theFunction(theCommands, input, output))
        if(output!=input)
        { output.CheckConsistency();
          theCommands.DoLogEvaluationIfNeedBe(outerFun);
//          if (input.Lispify()==output.Lispify())
//            crash << "Temporary check failed. " << crash;
//          stOutput << "<hr>DEBUG: Subbing: input: " << input.ToString()
//          << " by: " << output.ToString();
          return true;
        }
    } else
    { Function& innerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (innerFun.flagDisabledByUser)
        continue;
      if (input.children.size>2)
      { //stOutput << "more than 2 children: " << input.Lispify();
        if (innerFun.inputFitsMyInnerType(input))
          if (innerFun.theFunction(theCommands, input, output))
          { output.CheckConsistency();
            theCommands.DoLogEvaluationIfNeedBe(innerFun);
            return true;
          }
      } else if (input.children.size==2)
        if (innerFun.inputFitsMyInnerType(input[1]))
          if (innerFun.theFunction(theCommands, input[1], output))
          { output.CheckConsistency();
            theCommands.DoLogEvaluationIfNeedBe(innerFun);
            return true;
          }
    }
  return false;
}

bool Calculator::ExpressionMatchesPattern(const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions, std::stringstream* theLog)
{ MacroRegisterFunctionWithName("Calculator::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.owner==this && input.owner==this))
    crash << "This is a programming error. "
    << "Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << thePattern.ToString() << " and the input is "
    << input.ToString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error might be in code preceding the stack loading. "
    << crash;
//  static int ExpressionMatchesPatternDebugCounter=-1;
  static const bool printLocalDebugInfo=false;
//  if (input.ToString()=="f{}((a))=a+5")
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

void StackMaintainerRules::AddRule(const Expression& theRule)
{ if (this->owner==0)
    crash << "StackMaintainerRules has zero owner. " << crash;
  this->owner->RuleStack.AddChildOnTop(theRule);
  std::string currentRule;
  if (theRule.StartsWith(this->owner->opRulesOn() ) ||
      theRule.StartsWith(this->owner->opRulesOff() ) )
    for (int j=1; j<theRule.size(); j++)
    { if (!theRule[j].IsOfType(&currentRule))
        continue;
      if (!this->owner->namedRules.Contains(currentRule))
        continue;
      this->owner->GetFunctionHandlerFromNamedRule(currentRule).flagDisabledByUser=
      theRule.StartsWith(this->owner->opRulesOff());
    }
  this->owner->RuleStackCacheIndex=this->owner->cachedRuleStacks.GetIndex(this->owner->RuleStack);
  if (this->owner->RuleStackCacheIndex==-1)
    if (this->owner->cachedRuleStacks.size<this->owner->MaxCachedExpressionPerRuleStack)
    { this->owner->RuleStackCacheIndex=this->owner->cachedRuleStacks.size;
      this->owner->cachedRuleStacks.AddOnTop(this->owner->RuleStack);
    }
  if (this->owner->flagLogRules)
    *this->owner << "<hr>Added rule " << theRule.ToString() << " with state identifier "
    << this->owner->RuleStackCacheIndex;
}

StackMaintainerRules::StackMaintainerRules(Calculator* inputBoss)
{ this->owner=inputBoss;
  if (this->owner==0)
    return;
  this->startingRuleStackIndex=inputBoss->RuleStackCacheIndex;
  this->startingRuleStackSize=inputBoss->RuleStack.size();
}

StackMaintainerRules::~StackMaintainerRules()
{ if (this->owner==0)
    return;
  Expression& theRuleStack=this->owner->RuleStack;
  std::string currentRuleName;
  bool shouldUpdateRules=false;
  for (int i=this->startingRuleStackSize; i<theRuleStack.size(); i++)
    if (theRuleStack[i].StartsWith(this->owner->opRulesOn()) ||
        theRuleStack[i].StartsWith(this->owner->opRulesOff()))
      for (int j=1; j<theRuleStack[i].size(); j++)
      { if (!theRuleStack[i][j].IsOfType<std::string>(&currentRuleName))
          continue;
        if (!this->owner->namedRules.Contains(currentRuleName))
          continue;
        Function& currentRule=this->owner->GetFunctionHandlerFromNamedRule(currentRuleName);
        currentRule.flagDisabledByUser=currentRule.flagDisabledByUserDefaultValue;
        //stOutput << "<br>DEBUG: resetting rule: " << currentRule.calculatorIdentifier
        //<< " to value: "
        //<< currentRule.flagDisabledByUserDefaultValue
        //<< " with startingrulestacksize: "
        //<< this->startingRuleStackSize;
        shouldUpdateRules=true;
      }
  if (shouldUpdateRules)
    for (int i=0; i<this->startingRuleStackSize; i++)
    { if (theRuleStack[i].StartsWith(this->owner->opRulesOn()))
        for (int j=1; j<theRuleStack[i].size(); j++)
        { Function& currentFun=
          this->owner->GetFunctionHandlerFromNamedRule
          (theRuleStack[i][j].GetValue<std::string>());
          currentFun.flagDisabledByUser=false;
        }
      else if (theRuleStack[i].StartsWith(this->owner->opRulesOff()))
        for (int j=1; j<theRuleStack[i].size(); j++)
        { Function& currentFun=
          this->owner->GetFunctionHandlerFromNamedRule
          (theRuleStack[i][j].GetValue<std::string>());
          currentFun.flagDisabledByUser=true;
        }
    }
  this->owner->RuleStackCacheIndex=this->startingRuleStackIndex;
  this->owner->RuleStack.children.SetSize(this->startingRuleStackSize);
  this->owner=0;
}

Expression Calculator::GetNewAtom()
{ std::string atomPrefix;
  std::string candidate;
  while (true)
  { for (char a='a'; a<='z'; a++)
    { candidate=atomPrefix;
      candidate.push_back(a);
      if (!this->theAtoms.Contains(candidate))
      { Expression result;
        result.MakeAtom(candidate, *this);
        return result;
      }
    }
    atomPrefix=candidate;
  }

}

bool Calculator::AccountRule(const Expression& ruleE, StackMaintainerRules& theRuleStackMaintainer)
{ MacroRegisterFunctionWithName("Calculator::AccountRule");
  RecursionDepthCounter theRecursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
    return false;
  if (ruleE.IsCalculatorStatusChanger())
  { theRuleStackMaintainer.AddRule(ruleE);
    //stOutput << "<br>DEBUG: rule stack: "
    //<< this->RuleStackCacheIndex << ": accounted rule: " << ruleE.ToString();
  }
  if (!ruleE.IsListStartingWithAtom(this->opCommandEnclosure()))
    return true;
  if (ruleE.size()<=1)
    return true;
  if (!ruleE[1].StartsWith(this->opEndStatement()))
    return this->AccountRule(ruleE[1], theRuleStackMaintainer);
  for (int i=1; i<ruleE[1].size(); i++)
    if (!this->AccountRule(ruleE[1][i], theRuleStackMaintainer))
      return false;
  return true;
}

bool Calculator::EvaluateExpression
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::EvaluateExpression");
  bool notUsed=false;
  return theCommands.EvaluateExpression(theCommands, input, output, notUsed);
}

bool Calculator::EvaluateExpression
(Calculator& theCommands, const Expression& input, Expression& output,
 bool& outputIsNonCacheable)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::EvaluateExpression");
  //////////////////////////////
  //  stOutput << "DEBUG: temporary check comment me out. Evaluating:"
  //  << input.ToString()
  //  << " <hr>"
  //  ;
  //  input.CheckConsistency();
  //  input.HashFunction();
  //////////////////////////////
  if (theCommands.flagLogFullTreeCrunching && theCommands.RecursionDeptH<3)
  { theCommands << "<br>";
    for (int i=0; i<theCommands.RecursionDeptH; i++)
      theCommands << "&nbsp&nbsp&nbsp&nbsp";
      //stOutput << "&nbsp&nbsp&nbsp&nbsp";
    theCommands << "Evaluating " << input.Lispify()
    << " with rule stack cache index "
    << theCommands.RuleStackCacheIndex; // << this->RuleStack.ToString();
//    stOutput << "Evaluating " << input.Lispify() << " with rule stack of size " << this->RuleStack.size; // << this->RuleStack.ToString();
  }
  if (theCommands.RecursionDepthExceededHandleRoughly())
    return theCommands << " Evaluating expression: " << input.ToString() << " aborted. ";
  output=input;
  if (output.IsError())
  { theCommands.flagAbortComputationASAP=true;
    return true;
  }
  //////////////////////////////
  //  stOutput << "DEBUG: temporary check comment me out. Evaluating:"
  //  << input.ToString()
  //  << " <hr>"
  //  ;
  //  input.CheckConsistency();
  //  input.HashFunction();
  //////////////////////////////
  if (theCommands.EvaluatedExpressionsStack.Contains(input))
  { std::stringstream errorStream;
    errorStream << "I think I have detected an infinite cycle: I am asked to reduce "
    << input.ToString()
    << " but I have already seen that expression in the expression stack. ";
    theCommands.flagAbortComputationASAP=true;
    return output.MakeError(errorStream.str(), theCommands);
  }
  theCommands.EvaluatedExpressionsStack.AddOnTop(input);
  Expression theExpressionWithContext;
  theExpressionWithContext.reset(theCommands, 3);
  theExpressionWithContext.AddChildAtomOnTop(theCommands.opSequence());
  theExpressionWithContext.AddChildValueOnTop(theCommands.RuleStackCacheIndex);
  theExpressionWithContext.AddChildOnTop(input);
  int indexInCache=theCommands.cachedExpressions.GetIndex(theExpressionWithContext);
  if (indexInCache!=-1)
  { if (theCommands.flagLogCache)
    { theCommands << "<hr>Cache hit with state identifier "
      << theCommands.RuleStackCacheIndex << ": "
      << output.ToString() << " -> "
      << theCommands.imagesCachedExpressions[indexInCache].ToString();
      //theCommands << "DEBUG: "
      //<< theCommands.RuleStack.ToString();
    }
    output= theCommands.imagesCachedExpressions[indexInCache];
  } else
  { bool shouldCache=
    theCommands.cachedExpressions.size<theCommands.MaxCachedExpressionPerRuleStack &&
    !output.IsBuiltInTypE() && !output.IsBuiltInAtom();
    if (shouldCache)
    { theCommands.cachedExpressions.AddOnTop(theExpressionWithContext);
      indexInCache=theCommands.cachedExpressions.size-1;
      theCommands.imagesCachedExpressions.SetSize(indexInCache+1);
      theCommands.imagesCachedExpressions.LastObject()->reset(theCommands);
      theCommands.imagesCachedExpressions.LastObject()->MakeError("Error: not computed yet.", theCommands);
    }
  }
  //reduction phase:
  bool ReductionOcurred=true;
  int counterNumTransformations=0;
  Expression transformationE(theCommands);
  Expression beforePatternMatch;
  std::string inputIfAtom;
  outputIsNonCacheable=false;
  //////////////////////////////////
  while (ReductionOcurred && !theCommands.flagAbortComputationASAP)
  { StackMaintainerRules theRuleStackMaintainer(&theCommands);
    ReductionOcurred=false;
    counterNumTransformations++;
    if (output.IsAtom(&inputIfAtom))
      if (theCommands.atomsThatMustNotBeCached.Contains(inputIfAtom))
        outputIsNonCacheable=true;
    //The following code enclosed in the if clause is too
    //complicated/obfuscated for my taste
    //and perhaps needs a rewrite. However, at the moment of writing, I see
    //no better way of doing this, so here we go.
    if (outputIsNonCacheable)
    { if (indexInCache!=-1)
      { //We "undo" the caching process by
        //replacing the cached value with the minusOneExpression,
        //which, having no context, will never match another expression.
        theCommands.cachedExpressions.SetObjectAtIndex(indexInCache, theCommands.EMOne());
      }
      indexInCache=-1;
    }
    if (indexInCache!=-1)
      theCommands.imagesCachedExpressions[indexInCache]=output;
    //////------Handling naughty expressions------
    if (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit>0)
      if (theGlobalVariables.GetElapsedSeconds()!=0)
        if (theGlobalVariables.GetElapsedSeconds()>
            theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit/2)
        { if (!theCommands.flagTimeLimitErrorDetected)
            stOutput << "<br><b>Max allowed computational time is "
            << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit/2
            << ";  so far, "
            << theGlobalVariables.GetElapsedSeconds()
            << " have elapsed -> aborting computation ungracefully.</b>";
          theCommands.flagTimeLimitErrorDetected=true;
          theCommands.flagAbortComputationASAP=true;
          break;
        }
    if (counterNumTransformations>theCommands.MaxAlgTransformationsPerExpression)
    { if (!theCommands.flagMaxTransformationsErrorEncountered)
      { std::stringstream out;
        out << " While simplifying " << output.ToString(&theCommands.formatVisibleStrings)
        << "<br>Maximum number of algebraic transformations of "
        << theCommands.MaxAlgTransformationsPerExpression << " exceeded.";
        output.MakeError(out.str(), theCommands);
        theCommands.flagAbortComputationASAP=true;
        theCommands.flagMaxTransformationsErrorEncountered=true;
      }
      break;
    }
    //////------End of handling naughty expressions------
    //bool foundError=false;
    /////-------Children evaluation-------
    ProgressReport theReport;
    if (!output.IsFrozen())
      for (int i=0; i<output.size() && !theCommands.flagAbortComputationASAP; i++)
      { if (i>0 && output.StartsWith(theCommands.opEndStatement()))
        { if (theGlobalVariables.flagReportEverything)
          { std::stringstream reportStream;
            reportStream << "Substitution rules so far:";
            for (int j=1; j<i; j++)
              if (output[j].StartsWith(theCommands.opDefine()) ||
                  output[j].StartsWith(theCommands.opDefineConditional()))
                reportStream << "<br>" << MathRoutines::StringShortenInsertDots
                (output[j].ToString(), 100);
            reportStream << "<br>Evaluating:<br><b>"
            << MathRoutines::StringShortenInsertDots(output[i].ToString(), 100) << "</b>";
            theReport.Report(reportStream.str());
          }
        }
        bool childIsNonCacheable=false;
        if (theCommands.EvaluateExpression(theCommands, output[i], transformationE, childIsNonCacheable))
          output.SetChilD(i, transformationE);
        //important: here we check if the child is non-cache-able.
        if (childIsNonCacheable)
          outputIsNonCacheable=true;
        if (output[i].IsError())
        { theCommands.flagAbortComputationASAP=true;
          break;
        }
        if (output.StartsWith(theCommands.opEndStatement()))
          if (!theCommands.AccountRule(output[i], theRuleStackMaintainer))
          { std::stringstream out;
            out << "Failed to account rule: " << output[i].ToString()
            << ". Most likely the cause is too deeply nested recursion. ";
            output.MakeError(out.str(), theCommands);
            theCommands.flagAbortComputationASAP=true;
            break;
          }
      }
    /////-------Children evaluation end-------
    if (theCommands.flagAbortComputationASAP)
      break;
    /////-------Built-in evaluation-------
    if (theCommands.outerStandardFunction(theCommands, output, transformationE))
    { ReductionOcurred=true;
      if (theCommands.flagLogEvaluatioN)
      { /* *this << "<br>";
        if (theCommands.flagLogRules)
        { *this << "<br>Rule stack size: " << theCommands.RuleStack.size << ", context identifier: "
          << theCommands.RuleContextIdentifier << "<br>Rules: " << theCommands.RuleStack.ToString() << "<br>";
        }*/
        theCommands << "<br>Rule context identifier: " << theCommands.RuleStackCacheIndex
        << "<br>" << HtmlRoutines::GetMathMouseHover(output.ToString()) << " -> "
        << HtmlRoutines::GetMathMouseHover(transformationE.ToString());
      }
      output=transformationE;
      continue;
    }
    /////-------Built-in evaluation end-------
    /////-------User-defined pattern matching------
    for (int i=0; i<theCommands.RuleStack.size() && !theCommands.flagAbortComputationASAP; i++)
    { const Expression& currentPattern=theCommands.RuleStack[i];
      theCommands.TotalNumPatternMatchedPerformed++;
      if (theCommands.flagLogEvaluatioN)
        beforePatternMatch=output;
      BoundVariablesSubstitution bufferPairs;
      if (theCommands.ProcessOneExpressionOnePatternOneSub
          (currentPattern, output, bufferPairs, &theCommands.Comments,
           theCommands.flagLogPatternMatching))
      { ReductionOcurred=true;
        if (theCommands.flagLogEvaluatioN)
        { theCommands << "<hr>Rule cache index: " << theCommands.RuleStackCacheIndex
          << "<br>Rule: " << currentPattern.ToString()
          << "<br>" << HtmlRoutines::GetMathSpanPure(beforePatternMatch.ToString()) << " -> "
          << HtmlRoutines::GetMathSpanPure(output.ToString());
        }
        break;
      }
    }
/////-------User-defined pattern matching end------
  }
  theCommands.EvaluatedExpressionsStack.RemoveLastObject();
  if (theCommands.flagLogFullTreeCrunching && theCommands.RecursionDeptH<3)
  { theCommands << "<br>";
    for (int i=0; i<theCommands.RecursionDeptH; i++)
      theCommands << "&nbsp&nbsp&nbsp&nbsp";
    theCommands << "to get: " << output.Lispify();
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
    theExpression.MakeError(out.str(), *this);
    return 0;
  }
//  if (theExpression.ToString()=="f{}((a))=a+5")
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
  Expression conditionResult;
  this->EvaluateExpression(*this, tempExp, conditionResult);
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
(const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs, std::stringstream* theLog, bool logAttempts)
{ MacroRegisterFunctionWithName("Calculator::ProcessOneExpressionOnePatternOneSub");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (!thePattern.StartsWith(this->opDefine(), 3) && !thePattern.StartsWith(this->opDefineConditional(), 4))
    return false;
  if (theLog!=0 && logAttempts)
  { (*theLog) << "<hr>attempting to reduce expression " << theExpression.ToString();
    (*theLog) << " by pattern " << thePattern.ToString();
  }
  theExpression.CheckInitialization();
  const Expression& currentPattern=thePattern[1];
  const Expression* theCondition=0;
  bool isConditionalDefine=
  thePattern.StartsWith(this->opDefineConditional(), 4);
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


void Calculator::initComputationStats()
{ this->StartTimeEvaluationInSecondS=theGlobalVariables.GetElapsedSeconds();
  this->NumListsStart               =ParallelComputing::NumListsCreated;
  this->NumListResizesStart         =ParallelComputing::NumListResizesTotal;
  this->NumHashResizesStart         =ParallelComputing::NumHashResizes;
  this->NumSmallAdditionsStart      =Rational::TotalSmallAdditions;
  this->NumSmallMultiplicationsStart=Rational::TotalSmallMultiplications;
  this->NumSmallGCDcallsStart       =Rational::TotalSmallGCDcalls;
  this->NumLargeAdditionsStart      =Rational::TotalLargeAdditions;
  this->NumLargeMultiplicationsStart=Rational::TotalLargeMultiplications;
  this->NumLargeGCDcallsStart       =Rational::TotalLargeGCDcalls;
}

void Calculator::Evaluate(const std::string& theInput)
{ MacroRegisterFunctionWithName("Calculator::Evaluate");
  this->initComputationStats();
  this->inputString=theInput;
  this->ParseAndExtractExpressions(theInput, this->theProgramExpression, this->syntacticSouP, this->syntacticStacK, &this->syntaxErrors);
  this->EvaluateCommands();
}

void Calculator::EvaluateCommands()
{ MacroRegisterFunctionWithName("Calculator::EvaluateCommands");
  std::stringstream out;

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
  this->flagAbortComputationASAP=false;
  this->Comments.clear();
  ProgressReport theReport;
  if (!theGlobalVariables.flagRunningCommandLine)
    theReport.Report("Evaluating expressions, current expression stack:\n");
  this->EvaluateExpression(*this, this->theProgramExpression, this->theProgramExpression);
  if (this->RecursionDeptH!=0)
    crash << "This is a programming error: the starting recursion depth before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << "." << crash;
  theGlobalVariables.theDefaultFormat.GetElement().flagMakingExpressionTableWithLatex=true;
  theGlobalVariables.theDefaultFormat.GetElement().flagUseLatex=true;
  theGlobalVariables.theDefaultFormat.GetElement().flagExpressionNewLineAllowed=true;
  theGlobalVariables.theDefaultFormat.GetElement().flagIncludeExtraHtmlDescriptionsInPlots=!this->flagPlotNoControls;
  theGlobalVariables.theDefaultFormat.GetElement().flagLatexDetailsInHtml=this->flagWriteLatexPlots;
  theGlobalVariables.theDefaultFormat.GetElement().flagExpressionIsFinal=true;
  if(theGlobalVariables.flagRunningCommandLine)
  { theGlobalVariables.theDefaultFormat.GetElement().flagUseQuotes=false;
    theGlobalVariables.theDefaultFormat.GetElement().flagExpressionIsFinal=true;
    out << "Input: " << "\e[1;32m"
    << StartingExpression.ToString(&theGlobalVariables.theDefaultFormat.GetElement())
    << "\033[0m" << std::endl;
    theGlobalVariables.theDefaultFormat.GetElement().flagExpressionIsFinal=true;
    this->theObjectContainer.resetSliders();
    out << "Output: " << "\e[1;33m"
    << this->theProgramExpression.ToString
    (&theGlobalVariables.theDefaultFormat.GetElement())
    << "\033[0m" << std::endl;
  } else if (!this->flagDisplayFullExpressionTree)
  { std::string badCharsString=
    this->ToStringIsCorrectAsciiCalculatorString(this->inputString);
    if (badCharsString!="")
      out << badCharsString << "<hr>";
    this->theObjectContainer.resetSliders();
    this->theObjectContainer.resetPlots();
    out << this->theObjectContainer.ToStringJavascriptForUserInputBoxes();
    out << this->theProgramExpression.ToString
    (&theGlobalVariables.theDefaultFormat.GetElement(), &StartingExpression);
  } else
  { std::string badCharsString=this->ToStringIsCorrectAsciiCalculatorString
    (this->inputString);
    if (badCharsString!="")
      out << badCharsString << "<hr>";
    this->theObjectContainer.resetSliders();
    out << "<hr>Input:<br> " << StartingExpression.ToStringFull() << "<hr>"
    << "Output:<br>" << this->theProgramExpression.ToStringFull();
  }
  this->outputString=out.str();
  std::stringstream commentsStream;
  if (this->theObjectContainer.theAlgebraicClosure.theBasisMultiplicative.size>1)
    commentsStream << "<b>Algebraic number closure status. </b><br>"
    << this->theObjectContainer.theAlgebraicClosure.ToString() << "<hr>";
  if (this->Comments.str()!="")
    commentsStream << "<br><span>" << this->Comments.str() << "</span>";
  this->outputCommentsString=commentsStream.str();
  if(theGlobalVariables.flagRunningCommandLine && this->Comments.str()!="")
    this->outputString+=this->outputCommentsString;
  //std::cout << "DEBUG: got to end of story. " ;
}
