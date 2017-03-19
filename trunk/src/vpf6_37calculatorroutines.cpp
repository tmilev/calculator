
//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretation.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include "vpfHeader2Math9DrawingVariables.h"
ProjectInformationInstance ProjectInfoVpf6_37cpp(__FILE__, "More calculator built-in functions. ");

bool CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation");
  if (input.size()!=4)
   return theCommands
   << "I expected three arguments: "
   << "1) first problem number to test (1 or less= start at the beginning) "
   << "2) number of tests to run (0 or less= run all) and "
   << "3) number of tests to interpret. ";
  std::stringstream out;
  ProgressReport theReport;
  List<std::string> theFileNames, theFileTypes;
  int numDesiredTests=0;
  int numSamples=1;
  int firstTestToRun=1;
  input[1].IsSmallInteger(&firstTestToRun);
  input[2].IsSmallInteger(&numDesiredTests);
  input[3].IsSmallInteger(&numSamples);
  FileOperations::GetFolderFileNamesVirtual
  ("DefaultProblemLocation/", theFileNames, &theFileTypes, false);
  std::stringstream randSeedStreaM;
  randSeedStreaM << theCommands.theObjectContainer.CurrentRandomSeed;
  std::string randomSeedCurrent=randSeedStreaM.str();
  out << "Random seed at start: " << randomSeedCurrent << "<br>";
  out << "<table>";
  out << "<tr><th></th>"
  << "<th>File name </th>"
  << "<th>Loading</th>"
  << "<th>Interpretation</th>"
  << "<th>AnswerGeneration</th>"
  << "<th>Accepting built in answer?</th>"
  << "</tr>";
  if (numDesiredTests<=0)
    numDesiredTests=theFileNames.size;
  if (firstTestToRun<1)
    firstTestToRun=1;
  int numInterpretations=0;
  int totalToInterpret=0;
  for (int i=0; i< theFileNames.size; i++)
    if (theFileTypes[i]==".html")
      totalToInterpret++;
  totalToInterpret= MathRoutines::Minimum(numDesiredTests, totalToInterpret);
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  globalKeys= theGlobalVariables.webArguments;
  for (int i=0; i< theFileNames.size; i++)
  { if (numInterpretations>=numDesiredTests)
      break;
    if (theFileTypes[i]!=".html")
      continue;
    numInterpretations++;
    if (numInterpretations<firstTestToRun)
      continue;
    std::stringstream reportStream;
    reportStream << "Interpreting file "
    << theFileNames[i] << " ("
    << numInterpretations
    << " out of "
    << totalToInterpret
    << "). Random seed: "
    << randomSeedCurrent << "."
    ;
    theReport.Report(reportStream.str());
    CalculatorHTML theProblem;
    std::stringstream problemComments;
    theProblem.fileName="DefaultProblemLocation/"+theFileNames[i];
    bool isGoodLoad=theProblem.LoadMe(false, problemComments, randomSeedCurrent);
    bool isGoodInterpretation=false;
    if (isGoodLoad)
      isGoodInterpretation=theProblem.InterpretHtml(problemComments);
    std::stringstream randSeedCurrentStream;
    randSeedCurrentStream << theProblem.theProblemData.randomSeed;
    randomSeedCurrent=randSeedCurrentStream.str();
    out << "<tr>";
    out << "<td>" << numInterpretations << ". <td>";
    out << "<td>"
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=exerciseNoLogin"
    << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
    << "fileName=" << theProblem.fileName << "&randomSeed="
    << randomSeedCurrent << "\">"
    << theFileNames[i]
    << "</a>"
    << "</td>";  
    if (!isGoodLoad)
    { out << "<td><b>Couldn't load. </b>"
      << problemComments.str() << "</td>";
      out << "</tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (!isGoodInterpretation)
    { out << "<td><span style=\"color:red\"><b>Failure.</b></span> "
      << "Comments: " << problemComments.str();
      out << "</td></tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    bool answerGenerated=false;
    bool answersWork=false;
    std::string answerGeneration;
    std::string solutionReport;
    for (int j=0; j<theProblem.theProblemData.theAnswers.size; j++)
    { std::string currentAnswer;
      std::string currentKey="calculatorAnswer"+
      theProblem.theProblemData.theAnswers[j].answerId;
      theGlobalVariables.SetWebInpuT(currentKey, "1");
      theGlobalVariables.SetWebInpuT("fileName", theProblem.fileName);
      answerGeneration+=HtmlInterpretation::GetAnswerOnGiveUp
      (randomSeedCurrent, &currentAnswer, &answerGenerated)+"<hr>";
      if (!answerGenerated)
        break;
      theGlobalVariables.SetWebInpuT(currentKey, CGI::StringToURLString(currentAnswer, false));
      solutionReport+=
      HtmlInterpretation::SubmitProblem(randomSeedCurrent, &answersWork, false)+"<hr>";
      if (!answersWork)
        break;
      globalKeys.RemoveKey(currentKey);
    }
    if (!answerGenerated)
    { out << "<td><span style=\"color:red\"><b>Failure.</b></span>";
      out << "</td>"
      << "<td>"
      << answerGeneration
      << "</td>"
      << "</tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (!answersWork)
    { out << "<td><span style=\"color:red\"><b>Failure.</b></span>";
      out << "</td></tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (numInterpretations<=numSamples)
    { out << "<td><b>Problem</b><hr>" << theProblem.outputHtmlBodyNoTag
      << "</td>"
      ;
      out << "<td><b>Answer(s)</b><hr>" << answerGeneration
      << "</td>"
      ;
      out << "<td><b>Answer(s) confirmation(s)</b><hr>" << solutionReport
      << "</td>"
      ;
    }
    out << "</tr>";
  }
  out << "</table>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerIntervalClosedFromSequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntervalToSequence");
  if (!input.IsSequenceNElementS())
    return false;
  output=input;
  return output.SetChildAtomValue(0, theCommands.opIntervalClosed());
}

bool CalculatorFunctionsGeneral::innerIntervalRightClosedFromSequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntervalRightClosedFromSequence");
  if (!input.IsSequenceNElementS())
    return false;
  output=input;
  return output.SetChildAtomValue(0, theCommands.opIntervalRightClosed());
}

bool CalculatorFunctionsGeneral::innerIntervalLeftClosedFromSequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntervalLeftClosedFromSequence");
  if (!input.IsSequenceNElementS())
    return false;
  output=input;
  return output.SetChildAtomValue(0, theCommands.opIntervalLeftClosed());
}

bool CalculatorFunctionsGeneral::innerGetFirstSummandContaining(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetFirstSummandContaining");
  if (!input.StartsWithGivenAtom("GetFirstSummandContaining", 3))
    return false;
  List<Expression> theSummands;
  theCommands.CollectOpands(input[1], theCommands.opPlus(), theSummands);
  for (int i=0; i<theSummands.size; i++)
    if (theSummands[i].ContainsAsSubExpressionNoBuiltInTypes(input[2]))
    { output=theSummands[i];
      return true;
    }
  return false;
}

bool CalculatorFunctionsGeneral::innerGetSummand(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetSummand");
  if (! input.StartsWithGivenAtom("GetSummand",3))
    return false;
  const Expression& theExpression=input[1];
  if (theExpression.StartsWith(theCommands.opPlus(),3))
  { int summandIndex=-1;
    if (!input[2].IsSmallInteger(&summandIndex))
      return false;
    if (summandIndex<0)
      return false;
    List<Expression> theSummands;
    List<Expression> theSums;
    theCommands.CollectOpands(theExpression, theCommands.opPlus(),theSummands);
    for (int i=0; i<theSummands.size; i++)
      if (theSummands[i].ContainsAsSubExpressionNoBuiltInTypes(theCommands.opSum()))
      { theSums.AddOnTop(theSummands[i]);
        theSummands.RemoveIndexShiftDown(i);
        i--;
      }
    if (theSums.size>1)
      return false;
    if (summandIndex< theSummands.size)
    { output=theSummands[summandIndex];
      return true;
    }
    if (theSums.size==0)
      return false;
    output.reset(theCommands);
    output.AddChildAtomOnTop("GetSummand");
    output.AddChildOnTop(theSums[0]);
    Expression shiftE;
    shiftE.AssignValue(theSummands.size, theCommands);
    return output.AddChildOnTop(input[2]-shiftE);
  }
  List<Expression> theMultiplicands;
  theExpression.GetMultiplicandsRecursive(theMultiplicands);
  Expression theSum=*theMultiplicands.LastObject();
  theMultiplicands.RemoveLastObject();
  Expression theCoeff;
  if (theMultiplicands.size>0)
    theCoeff.MakeProducT(theCommands, theMultiplicands);
  else
    theCoeff.AssignValue(1, theCommands);
  if (!theSum.StartsWith(theCommands.opSum(),3))
    return false;
  const Expression& theLimits=theSum[1];
  if (!theLimits.StartsWith(theCommands.opLimitBoundary(),3))
    return false;
  const Expression& theBottomBoundary=theLimits[1];
  if (!theBottomBoundary.StartsWith(theCommands.opDefine(),3))
    return false;
  Expression theSub=theBottomBoundary;
  Expression //oneE,
  valueToSubWith;
  //oneE.AssignValue(1, theCommands);
  valueToSubWith=theBottomBoundary[2]+input[2];
  theSub.SetChilD(2, valueToSubWith);
  Expression theCommandSequence(theCommands);
  theCommandSequence.AddChildAtomOnTop(theCommands.opEndStatement());
  theCommandSequence.AddChildOnTop(theSub);
  theCommandSequence.AddChildOnTop(theCoeff*theSum[2]);
  return output.MakeXOX
  (theCommands, theCommands.opUnderscore(), theCommandSequence,
   theCommands.ETwo());
}

bool CalculatorFunctionsGeneral::innerPlotVectorField(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsGeneral::innerPlotDirectionOrVectorField(theCommands, input, output, false);
}

bool CalculatorFunctionsGeneral::innerPlotDirectionField(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsGeneral::innerPlotDirectionOrVectorField(theCommands, input, output, true);
}

bool CalculatorFunctionsGeneral::innerPlotDirectionOrVectorField(Calculator& theCommands, const Expression& input, Expression& output, bool vectorsAreNormalized)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotDirectionOrVectorField");
  (void) vectorsAreNormalized;
  //stOutput << input.ToString();
  if (input.size()<5)
    return output.MakeError
    ("Vector fields take at least four arguments: \
      the vector field, the low left corner, \
      the upper right, and the number of \
      segments in each direction. ",
     theCommands, true);
  if (input.HasBoundVariables())
    return false;
  Plot thePlot;
  thePlot.dimension=2;
  PlotObject thePlotObj;
  thePlotObj.leftPtE=input[2];
  thePlotObj.rightPtE=input[3];
  thePlot.DesiredHtmlHeightInPixels=300;
  thePlot.DesiredHtmlWidthInPixels=300;
  if (input.size()>=7)
  { if (!input[6].IsOfType<std::string>(&thePlotObj.colorJS))
      thePlotObj.colorJS=input[6].ToString();
  } else
    thePlotObj.colorJS="blue";
  thePlotObj.colorRGB=CGI::RedGreenBlue(0, 0, 255);
  DrawingVariables::GetColorIntFromColorString
  (thePlotObj.colorJS, thePlotObj.colorRGB);
  thePlotObj.lineWidth=1;
  if (input.size()>=8)
    input[7].EvaluatesToDouble(&thePlotObj.lineWidth);
  Vector<double> lowLeft, upRight;
  if (!theCommands.GetVectorDoubles(input[2], lowLeft, 2))
    return theCommands << "Failed to low left corner from: "
    << input[2].ToString();
  if (!theCommands.GetVectorDoubles(input[3], upRight, 2))
    return theCommands << "Failed to up right corner from: "
    << input[3].ToString();
  thePlotObj.yHigh=upRight[1];
  thePlotObj.yLow=lowLeft[1];
  thePlotObj.xHigh=upRight[0];
  thePlotObj.xLow=lowLeft[0];
  List<std::string> lowLeftStrings, upRightStrings;
  lowLeft.ToListStringsBasicType(lowLeftStrings);
  upRight.ToListStringsBasicType(upRightStrings);
  thePlotObj.theVarRangesJS.SetSize(2);
  thePlotObj.theVarRangesJS[0].SetSize(2);
  thePlotObj.theVarRangesJS[1].SetSize(2);
  thePlotObj.theVarRangesJS[0][0]=lowLeftStrings[0];
  thePlotObj.theVarRangesJS[0][1]=upRightStrings[0];
  thePlotObj.theVarRangesJS[1][0]=lowLeftStrings[1];
  thePlotObj.theVarRangesJS[1][1]=upRightStrings[1];
  thePlotObj.manifoldImmersion=input[1];
  Expression jsConverterE;
  //thePlotObj.defaultLengthJS="0.5";
  if (input.size()>=6)
  { //stOutput << "DEBUG: got to here";
    if (CalculatorFunctionsGeneral::
        innerMakeJavascriptExpression
        (theCommands, input[5], jsConverterE)
        )
    { thePlotObj.defaultLengthJS=jsConverterE.ToString();
    } else
      return theCommands << "Failed to extract javascript from "
      << input[5].ToString();
  }
//stOutput << "DEBUG: defaultLengthJS: " << thePlotObj.defaultLengthJS;

  if (CalculatorFunctionsGeneral::
      innerMakeJavascriptExpression
      (theCommands, thePlotObj.manifoldImmersion, jsConverterE)
      )
  { thePlotObj.manifoldImmersionJS=jsConverterE.ToString();
    thePlotObj.manifoldImmersion.HasInputBoxVariables
    (&thePlot.boxesThatUpdateMe);
  } else
    return theCommands << "Failed to extract javascript from "
    << input[1].ToString();
  thePlotObj.manifoldImmersion.GetFreeVariables
  (thePlotObj.variablesInPlay, true);
  Expression xE, yE;
  xE.MakeAtom("x", theCommands);
  yE.MakeAtom("y", theCommands);
  if (thePlotObj.variablesInPlay.size==0)
    thePlotObj.variablesInPlay.AddOnTop(xE);
  if (thePlotObj.variablesInPlay.size==1)
  { if (thePlotObj.variablesInPlay.Contains(xE))
      thePlotObj.variablesInPlay.AddOnTop(yE);
    else
      thePlotObj.variablesInPlay.AddOnTop(xE);
  }
  thePlotObj.variablesInPlay.QuickSortAscending();
  thePlotObj.variablesInPlayJS.SetSize(thePlotObj.variablesInPlay.size);
  for (int i=0; i<thePlotObj.variablesInPlay.size; i++)
    thePlotObj.variablesInPlayJS[i]=thePlotObj.variablesInPlay[i].ToString();
  thePlotObj.thePlotType="plotDirectionField";
  if (!input[4].IsSequenceNElementS(2))
    return theCommands
    << "<hr>Could not extract a list of elements for the "
    << "number of segments from: " << input[4].ToString();
  thePlotObj.numSegmenTsJS.SetSize(2);
  for (int i=0; i<2; i++)
  { if (!CalculatorFunctionsGeneral::
        innerMakeJavascriptExpression
        (theCommands, input[4][i+1], jsConverterE))
      return theCommands << "Failed to convert "
      << input[4][i+1].ToString()
      << " to javascript. ";
    thePlotObj.numSegmenTsJS[i]=jsConverterE.ToString();
  }
  thePlot.thePlots.AddOnTop(thePlotObj);
//  stOutput << "DEBUG: height, width: " << desiredHtmlHeightPixels << ", " << desiredHtmlWidthPixels;
  return output.AssignValue(thePlot, theCommands);
}
