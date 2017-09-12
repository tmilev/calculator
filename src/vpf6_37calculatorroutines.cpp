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
#include "vpfHeader5Crypto.h"
//////////////////////////////////////
#include "webserver.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
///////////////////////////////////
ProjectInformationInstance ProjectInfoVpf6_37cpp(__FILE__, "More calculator built-in functions. ");

bool CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return theCommands << "Automated tests available to logged-in admins only. ";
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
    out << "<tr>";
//    out << "<td>DEBUG: Random seed at start: "
//    << theProblem.theProblemData.randomSeed;

    if (isGoodLoad)
      isGoodInterpretation=theProblem.InterpretHtml(problemComments);
//    out << ".<br>DEBUG: random seed end: "
//    << theProblem.theProblemData.randomSeed
//    << "</td>";
    std::stringstream randSeedCurrentStream;
    randSeedCurrentStream << theProblem.theProblemData.randomSeed;
    randomSeedCurrent=randSeedCurrentStream.str();
    out << "<td>" << numInterpretations << ". <td>";
    out << "<td>"
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=exerciseNoLogin"
    << "&"
    << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
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
      theGlobalVariables.SetWebInpuT(currentKey, HtmlRoutines::ConvertStringToURLString(currentAnswer, false));
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
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntervalClosedFromSequence");
  if (!input.IsSequenceNElementS(2))
    return false;
  output=input;
  return output.SetChildAtomValue(0, theCommands.opIntervalClosed());
}

bool CalculatorFunctionsGeneral::innerIntervalOpenFromSequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntervalOpenFromSequence");
  if (!input.IsSequenceNElementS(2))
    return false;
  output=input;
  return output.SetChildAtomValue(0, theCommands.opIntervalOpen());
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
  if (! input.StartsWithGivenAtom("GetSummand", 3))
    return false;
  const Expression& theExpression=input[1];
  if (theExpression.StartsWith(theCommands.opPlus(), 3))
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
  if (input.size()>=7)
  { if (!input[6].IsOfType<std::string>(&thePlotObj.colorJS))
      thePlotObj.colorJS=input[6].ToString();
  } else
    thePlotObj.colorJS="blue";
  thePlotObj.colorRGB=HtmlRoutines::RedGreenBlue(0, 0, 255);
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

bool CalculatorFunctionsGeneral::innerJWTverifyAgainstRSA256(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerJWTverifyAgainstRSA256");
  if (input.size()!=4)
    return theCommands << "The JWT verify command expects 3 arguments:"
    << " string with the token in the usual format (\"a.b.c\"),"
    << " the modulus of the key and the exponent of the key. ";
  std::string theTokenString;
  if (!input[1].IsOfType(&theTokenString))
    return theCommands << "The first argument of " << input.ToString() << " is not a string. ";
  std::stringstream out;
  std::string theModBase64, theExpBase64;
  if (!input[2].IsOfType(&theModBase64) || !input[3].IsOfType(&theExpBase64))
    return theCommands << "Failed to convert the arguments "
    << input[2].ToString()
    << " and " << input[3].ToString()
    << " to base64 strings";
  JSONWebToken theToken;
  LargeIntUnsigned theMod, theExp;
  if (!theToken.AssignString(theTokenString, &out))
    return output.AssignValue(out.str(), theCommands);
  out << "Sucesfully extracted JWT token. <br>"
  << theToken.ToString()
  << "<br>";
  if (!Crypto::ConvertBase64ToLargeUnsignedInt(theModBase64, theMod, &out) ||
      !Crypto::ConvertBase64ToLargeUnsignedInt(theExpBase64, theExp, &out))
    return output.AssignValue(out.str(), theCommands);
  out << "<br>Successfully extracted modulus and exponent";
  theToken.VerifyRSA256(theMod, theExp, &out, &out);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerJWTverifyAgainstKnownKeys(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerJWTverifyAgainstKnownKeys");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return theCommands << "This function is only available to logged-in admins. ";
  if (!input.IsOfType<std::string>())
    return false;
  const std::string& inputString=input.GetValue<std::string>();
  std::stringstream out;
  //out << "DEBUG: Input string: " << inputString << "<br>";
  Crypto::VerifyJWTagainstKnownKeys(inputString, &out, &out);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerHexToString(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerHexToString");
  std::string inputString;
  if (!input.IsOfType(&inputString))
    inputString=input.ToString();
  std::string result;
  if (!Crypto::ConvertHexToString(inputString, result))
    return theCommands << "Failed to interpret " << inputString
    << " as a hex string ";
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerIntegerToHex(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegerToHex");
  LargeInt theLI;
  if (!input.IsInteger(&theLI))
    return false;
  if (theLI<0)
    return theCommands << "I only convert positive integers to hex strings. ";
  std::string result;
  if (!Crypto::ConvertLargeUnsignedIntToHex(theLI.value, result))
    return theCommands << "Failed to convert " << theLI
    << " as a hex string ";
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerHexToInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerHexToInteger");
  std::string inputString;
  if (!input.IsOfType(&inputString))
    inputString=input.ToString();
  LargeInt result;
  if (!Crypto::ConvertHexToInteger(inputString, result))
    return theCommands << "Failed to interpret " << inputString
    << " as a hex string ";
  Rational resultRat=result;
  return output.AssignValue(resultRat, theCommands);
}

bool CalculatorFunctionsGeneral::innerTestJSON(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTestJSON");
  std::string inputString;
  if (!input.IsOfType(&inputString))
    inputString=input.ToString();
  JSData theData;
  std::stringstream out;
  if (!theData.readstring(inputString, &out))
    return output.AssignValue(out.str(), theCommands);
  out << "<hr>Output: <br>"
  << theData.ToString(true);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerBase64ToHex(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerBase64ToHex");
  std::string inputString;
  if (!input.IsOfType(&inputString))
    inputString=input.ToString();
  std::string result, bitStream;
  if (!Crypto::ConvertBase64ToString(inputString, bitStream, &theCommands.Comments, &theCommands.Comments))
    return false;
  Crypto::ConvertStringToHex(bitStream, result);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerRSAencrypt(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerRSAencrypt");
  //stOutput << "DEBUG: here be i";
  if (input.size()!=4)
    return false;
  LargeInt theExponent, theModulus, theMessage, result;
  if (!input[1].IsInteger(& theModulus) ||
      !input[2].IsInteger(&theExponent) ||
      !input[3].IsInteger(&theMessage))
    return theCommands << "Failed to extract three (large) integers from the arguments of "
    << input.ToString();
  if (theModulus<0)
    theModulus*=-1;
  if (theModulus==0 || theExponent==0)
    return theCommands << "The modulus and exponent must be non-zero.";
  if (theModulus==1)
    return theCommands << "Modulus 1 not allowed";
  result=Crypto::RSAencrypt(theModulus.value, theExponent, theMessage);
  return output.AssignValue((Rational)result, theCommands);
}

bool CalculatorFunctionsGeneral::innerSendEmailWithMailGun
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSendEmailWithMailGun");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return theCommands << "Sending mail available to logged-in admins only. ";
  std::stringstream out;
#ifdef MACRO_use_MySQL
  if (input.size()!=4)
    return theCommands << "Send email requires three arguments. ";
  EmailRoutines theEmail;
  if (! input[1].IsOfType(&theEmail.toEmail) ||
      ! input[2].IsOfType(&theEmail.emailContent) ||
      ! input[3].IsOfType(&theEmail.subject)
      )
    return theCommands << "Arguments of "
    << input.ToString()
    << "expected to be strings (enclose in \"\" please). ";

  theEmail.SendEmailWithMailGun(&out, &out);
#else
  out << "Error: database not running. ";
#endif
  return output.AssignValue(out.str(),theCommands);
}

bool CalculatorFunctionsGeneral::innerIsSquare(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsSquare");
  LargeInt theLI;
  if (!input.IsInteger(&theLI))
    return false;
  if (theLI<0)
    return output.AssignValue(0, theCommands);
  if (theLI==0)
    return output.AssignValue(1, theCommands);
  List<int> theMults;
  List<LargeInt> theFactors;
  if (!theLI.value.Factor(theFactors, theMults))
    return theCommands << "Failed to factor: " << theLI.ToString() << " (may be too large?).";
  int result=1;
  for (int i=0; i<theMults.size; i++)
    if ((theMults[i]%2)!=0)
    { result=0;
      break;
    }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsSquareFree(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsSquareFree");
  LargeInt theLI;
  if (!input.IsInteger(&theLI))
    return false;
  List<int> theMults;
  List<LargeInt> theFactors;
  if (!theLI.value.Factor(theFactors, theMults))
    return theCommands << "Failed to factor: " << theLI.ToString() << " (may be too large?).";
  int result=1;
  for (int i=0; i<theMults.size; i++)
    if (theMults[i]>1)
    { result=0;
      break;
    }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsPower(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsPower");
  LargeInt theLI;
  if (!input.IsInteger(&theLI))
    return false;
  if (theLI.IsEqualToZero())
    return false;
  List<int> theMults;
  List<LargeInt> theFactors;
  if (!theLI.value.Factor(theFactors, theMults))
    return theCommands << "Failed to factor: " << theLI.ToString() << " (may be too large?).";
  int result=1;
  if (theMults.size>0)
    result = (theMults[0]>1);
  for (int i=1; i<theMults.size; i++)
    if (theMults[i]!=theMults[0])
    { result=0;
      break;
    }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerFactorInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFactorInteger");
  LargeInt theLI;
  if (!input.IsInteger(&theLI))
    return false;
  if (theLI.IsEqualToZero())
    return false;
  List<LargeInt> primeFactors;
  List<int> mults;
  if (!theLI.value.Factor(primeFactors,mults))
    return false;
  List<Expression> result;
  for (int i=0; i<primeFactors.size; i++)
  { Expression currentE;
    currentE.AssignValue((Rational) primeFactors[i],theCommands);
    for (int j=0; j<mults[i]; j++)
      result.AddOnTop(currentE);
  }
  if (theLI<0 && result.size>0)
    result[0]*=-1;
  return output.MakeSequence(theCommands, &result);
}

bool CalculatorFunctionsGeneral::innerFactorOutNumberContent(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFactorOutNumberContent");
  MonomialCollection<Expression, Rational> theV;
  if (!theCommands.CollectSummands(theCommands, input, theV))
    return theCommands << "Failed to extract summands from: " << input.ToString();
  if (theV.IsEqualToZero())
    return output.AssignValue(0, theCommands);
  Rational theCF= theV.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedByLeadingCoefficientPositive();
  if (theCF==0)
    return false;
  theCF.Invert();
  if (theCF==1 )
  { if (!input.StartsWithGivenAtom("FactorOutNumberContent"))
    { output=input;
      return true;
    } else
      return false;
  }
  Expression left, right;
  left.AssignValue(theCF, theCommands);
  right.MakeSum(theCommands, theV);
  output=left*right;
  return true;
}

bool CalculatorFunctionsGeneral::innerSubList(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSubList");
  if(input.size()!=3)
    return false;
  if (!input[1].IsSequenceNElementS())
    return false;
  HashedList<Expression> boundVars;
  if(!input[2].GetBoundVariables(boundVars))
    return theCommands << "Could not get bound variables from: " << input[2].ToString();
  if (input[2].IsEqualToOne())
  { output=input[1];
    return true;
  }
  if (boundVars.size==0)
    return output.MakeSequence(theCommands,0);
  Expression theSubbed, toBeSubbed, subbedSimplified;
  toBeSubbed.reset(theCommands);
  toBeSubbed.AddChildAtomOnTop(theCommands.opBind());
  toBeSubbed.AddChildOnTop(boundVars[0]);
  List<Expression> theList;
  for (int i=1; i<input[1].size(); i++)
  { theSubbed=input[2];
    theSubbed.SubstituteRecursively(toBeSubbed, input[1][i]);
    if (!theCommands.EvaluateExpression(theCommands, theSubbed, subbedSimplified))
      return theCommands << "Failed to evaluate " << theSubbed.ToString();
    if (subbedSimplified.IsEqualToOne())
      theList.AddOnTop(input[1][i]);
  }
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctionsGeneral::innerApplyToList(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerApplyToList");
  if (input.size()!=3)
    return false;
  const Expression& theFun=input[1];
  if (!input[2].IsSequenceNElementS())
    return false;
  List<Expression> result;
  result.SetSize(input[2].size()-1);
  for (int i=1; i<input[2].size(); i++)
  { result[i-1].reset(theCommands);
    result[i-1].AddChildOnTop(theFun);
    result[i-1].AddChildOnTop(input[2][i]);
  }
  return output.MakeSequence(theCommands, &result);
}

bool CalculatorFunctionsGeneral::innerPolynomialDivisionQuotient(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionQuotient");
  Expression theContext;
  Vector<Polynomial<Rational> > thePolys;
  if (!theCommands.GetListPolysVariableLabelsInLex(input, thePolys, theContext))
    return output.MakeError("Failed to extract list of polynomials. ", theCommands);
  GroebnerBasisComputation<Rational> theGB;
  theGB.flagStoreQuotients=true;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.MakeError("Division by zero.", theCommands, true);
    theGB.theBasiS[i-1]=thePolys[i];
  }
//  stOutput << "<hr>The polys: " << thePolys.ToString() << "<br>The gb basis: "
//  << theGB.theBasiS.ToString() << "<hr>";
  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS);
  theGB.RemainderDivisionWithRespectToBasis(thePolys[0], &outputRemainder, -1);
  Expression currentE, thePolyE;
  List<Expression> theList;
  for (int i=0; i<theGB.theQuotients.size; i++)
  { currentE.reset(theCommands);
    currentE.AddChildAtomOnTop("MakeExpression");
    thePolyE.AssignValueWithContext(theGB.theQuotients[i], theContext, theCommands);
    currentE.AddChildOnTop(thePolyE);
    theList.AddOnTop(currentE);
  }
  if (theList.size==1)
  { output=theList[0];
    return true;
  }
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctionsGeneral::innerArccosAlgebraic(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArccosAlgebraic");
  Rational theRat;
  if (input.IsRational(&theRat))
  { if (theRat==1)
      return output.AssignValue(0, theCommands);
    if (theRat==0)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=2;
      return true;
    }
    if (theRat==-1)
      return output.MakeAtom(theCommands.opPi(), theCommands);
    if (theRat==Rational(1,2))
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=3;
      return true;
    }
    if (theRat==Rational(-1,2))
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output*=2;
      output/=3;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (input.IsOfType<AlgebraicNumber>(&argument))
  { candidate.AssignRationalQuadraticRadical(Rational(1,2), theCommands.theObjectContainer.theAlgebraicClosure);
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=4;
      return true;
    }
    candidate*=-1;
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=4;
      output*=3;
      return true;
    }
    candidate.AssignRationalQuadraticRadical(Rational(3,4), theCommands.theObjectContainer.theAlgebraicClosure);
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=6;
      return true;
    }
    candidate*=-1;
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=6;
      output*=5;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerArcsinAlgebraic(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArcsinAlgebraic");
  Rational theRat;
  if (input.IsRational(&theRat))
  { if (theRat==1)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=2;
      return true;
    }
    if (theRat==0)
      return output.AssignValue(0, theCommands);
    if (theRat==-1)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=-2;
      return true;
    }
    if (theRat==Rational(1,2))
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=6;
      return true;
    }
    if (theRat==Rational(-1,2))
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=-6;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (input.IsOfType<AlgebraicNumber>(&argument))
  { candidate.AssignRationalQuadraticRadical(Rational(1,2), theCommands.theObjectContainer.theAlgebraicClosure);
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=4;
      return true;
    }
    candidate*=-1;
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=-4;
      return true;
    }
    candidate.AssignRationalQuadraticRadical(Rational(3,4), theCommands.theObjectContainer.theAlgebraicClosure);
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=3;
      return true;
    }
    candidate*=-1;
    if (candidate==argument)
    { output.MakeAtom(theCommands.opPi(), theCommands);
      output/=-3;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerMatchesPattern(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMatchesPattern");
  if (input.size()!=3)
    return false;
  MapLisT<Expression, Expression> matchedExpressions;
  if (!theCommands.ExpressionMatchesPattern(input[2], input[1], matchedExpressions, &theCommands.Comments))
    return output.AssignValue(0, theCommands);
  Expression commandList;
  commandList.reset(theCommands);
  commandList.AddChildAtomOnTop(theCommands.opEndStatement());
  for (int i=0; i<matchedExpressions.size(); i++)
  { Expression currentCommand;
    //stOutput << "<hr>DEBUG: Bound var: " << matchedExpressions.theBoundVariables[i].ToString()
    //<< ", image: " << matchedExpressions.variableImages[i].ToString();
    if (!matchedExpressions.theKeys[i].StartsWith(theCommands.opBind(), 2) )
    { std::stringstream errorStream;
      errorStream << "Bound variable " << matchedExpressions.theKeys[i].ToString()
      << " does not start with the bind atom. ";
      return output.MakeError(errorStream.str(), theCommands);
    }
    currentCommand.MakeXOX
    (theCommands, theCommands.opDefine(), matchedExpressions.theKeys[i][1],
     matchedExpressions.theValues[i]);
    commandList.AddChildOnTop(currentCommand);
  }
  output.reset(theCommands);
  output.AddChildAtomOnTop(theCommands.opCommandEnclosure());
  return output.AddChildOnTop(commandList);
}

bool CalculatorFunctionsGeneral::innerDegreesToRadians(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDegreesToRadians");
  if (!input.StartsWith(theCommands.opThePower(),3))
    return false;
  if (!input[2].IsAtomGivenData("\\circ"))
    return false;
  Expression piE;
  piE.MakeAtom(theCommands.opPi(), theCommands);
  output=input[1]*piE;
  output/=180;
  return true;
}

bool CalculatorFunctionsGeneral::innerLessThanOrEqualTo(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLessThanOrEqualTo");
  if (input.size()!=3)
    return false;
  Expression result(theCommands);
  result.AddChildAtomOnTop("\\geq");
  result.AddChildOnTop(input[2]);
  result.AddChildOnTop(input[1]);
  output=result;
  return true;
}


bool CalculatorFunctionsGeneral::innerGreaterThanOrEqualTo(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGreaterThanOrEqualTo");
  if (input.size()!=3)
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  Rational leftRat, rightRat;
  if (left.IsRational(&leftRat) && right.IsRational(&rightRat))
  { if (leftRat>=rightRat)
      return output.AssignValue(1, theCommands);
    return output.AssignValue(0, theCommands);
  }
  double leftD, rightD;
//  stOutput << "DEBUG: Got to here, comparing " << left.ToString() << " to " << right.ToString();
  if (left.EvaluatesToDouble(&leftD) && right.EvaluatesToDouble(&rightD) )
  { if (leftD>=rightD)
      return output.AssignValue(1, theCommands);
    return output.AssignValue(0, theCommands);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerGreaterThan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGreaterThan");
  if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  Rational leftRat, rightRat;
  if (left.IsRational(&leftRat) && right.IsRational(&rightRat))
  { if (leftRat>rightRat)
      return output.AssignValue(1, theCommands);
    return output.AssignValue(0, theCommands);
  }
  double leftD, rightD;
//  stOutput << "DEBUG: Got to here, comparing " << left.ToString() << " to " << right.ToString();
  if (left.EvaluatesToDouble(&leftD) && right.EvaluatesToDouble(&rightD) )
  { if (leftD>rightD)
      return output.AssignValue(1, theCommands);
    return output.AssignValue(0, theCommands);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerLessThan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLessThan");
  if (!input.IsListNElements(3))
    return false;
  Expression swappedE(theCommands);
  swappedE.AddChildOnTop(input[0]);
  swappedE.AddChildOnTop(input[2]);
  swappedE.AddChildOnTop(input[1]);
  return CalculatorFunctionsGeneral::innerGreaterThan(theCommands, swappedE, output);
}

bool CalculatorFunctionsGeneral::innerCollectOpands
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCollectOpands");
  if (input.size()!=3)
    return false;
  if (!input[1].IsAtom())
    return false;
  List<Expression> theList;
  theCommands.AppendOpandsReturnTrueIfOrderNonCanonical
  (input[2], theList, input[1].theData);
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctionsGeneral::innerCollectMultiplicands
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCollectSummands");
  List<Expression> theList;
  theCommands.AppendOpandsReturnTrueIfOrderNonCanonical
  (input, theList, theCommands.opTimes());
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctionsGeneral::innerCollectSummands
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCollectSummands");
  List<Expression> theList;
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical
  (input, theList);
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctionsGeneral::LeftIntervalGreaterThanRight
(const Expression& left, const Expression& right)
{ if (left.owner==0 || right.owner==0)
    return left>right;
  if (left.size()!=3 || right.size()!=3)
    return left>right;
  if (!left.IsIntervalRealLine())
    return left>right;
  if (!right.IsIntervalRealLine())
    return left>right;
  double left1, right1, left2, right2;
  bool left1IsDouble=left[1].EvaluatesToDouble(&left1);
  bool right1IsDouble=right[1].EvaluatesToDouble(&right1);
  bool left2IsDouble=left[2].EvaluatesToDouble(&left2);
  bool righ2IsDouble=right[2].EvaluatesToDouble(&right2);
  const Expression& inftyE=right.owner->EInfinity();
  const Expression& mInftyE=right.owner->EMInfinity();
  if (left1IsDouble && right1IsDouble)
  { if (left1>right1)
      return true;
    if (right1>left1)
      return false;
    if (left2IsDouble && righ2IsDouble)
    { if (left2>right2)
        return true;
      if (right2>left2)
        return false;
    }
    if (left[2]==inftyE && righ2IsDouble)
      return true;
    if (left2IsDouble && right[2]==inftyE)
      return false;
  }
  if (left[1]==mInftyE && right1IsDouble)
    return false;
  if (left1IsDouble && right[1]==mInftyE)
    return true;
  if (left[1]==mInftyE || right[1]==mInftyE)
  { if (left2IsDouble && righ2IsDouble)
    { if (left2>right2)
        return true;
      if (right2>left2)
        return false;
    }
    if (left[2]==inftyE && righ2IsDouble)
      return true;
    if (left2IsDouble && right[2]==inftyE)
      return false;
  }
  return left>right;
}

bool CalculatorFunctionsGeneral::innerIntersectIntervals
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntersectIntervals");
  if (!input.StartsWith(theCommands.opIntersection(), 3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if(!leftE.IsIntervalRealLine())
    return false;
  if(!rightE.IsIntervalRealLine())
    return false;
  double left1=0, left2=0, right1=0, right2=0;
  if (!leftE[1].EvaluatesToDouble(&left1) || !leftE[2].EvaluatesToDouble(&left2))
    return false;
  if (!rightE[1].EvaluatesToDouble(&right1) || !rightE[2].EvaluatesToDouble(&right2))
    return false;
  //stOutput << "intervals double: " << left1 << ", " << left2 << " , "
  //<< right1 << " " << right2;
  if (left1>left2 || right1>right2)
    return false;
  bool leftIsClosed =true;
  bool rightIsClosed=true;
  Expression leftFinal, rightFinal;
  double leftResult=0, rightResult=0;
  if (left1<right1)
  { leftFinal=rightE[1];
    leftResult=right1;
    if (rightE.StartsWith(theCommands.opSequence()) ||
        rightE.StartsWith(theCommands.opIntervalRightClosed()))
      leftIsClosed=false;
  }
  if (left1==right1)
  { leftFinal=leftE[1];
    leftResult=left1;
    if (leftE .StartsWith(theCommands.opSequence()) ||
        leftE .StartsWith(theCommands.opIntervalRightClosed())||
        rightE.StartsWith(theCommands.opSequence()) ||
        rightE.StartsWith(theCommands.opIntervalRightClosed())
        )
      leftIsClosed=false;
  }
  if (left1>right1)
  { leftFinal=leftE[1];
    leftResult=left1;
    if (leftE.StartsWith(theCommands.opSequence()) ||
        leftE.StartsWith(theCommands.opIntervalRightClosed())
        )
      leftIsClosed=false;
  }
///////////////////////////
  if (left2>right2)
  { rightFinal=rightE[2];
    rightResult=right2;
    if (rightE.StartsWith(theCommands.opSequence()) ||
        rightE.StartsWith(theCommands.opIntervalLeftClosed()))
      rightIsClosed=false;
  }
  if (left2==right2)
  { rightFinal=rightE[2];
    rightResult=right2;
    if (leftE .StartsWith(theCommands.opSequence()) ||
        leftE .StartsWith(theCommands.opIntervalLeftClosed())||
        rightE.StartsWith(theCommands.opSequence()) ||
        rightE.StartsWith(theCommands.opIntervalLeftClosed())
        )
      rightIsClosed=false;
  }
  if (left2<right2)
  { rightFinal=leftE[2];
    rightResult=left2;
    if (leftE.StartsWith(theCommands.opSequence()) ||
        leftE.StartsWith(theCommands.opIntervalLeftClosed())
        )
      rightIsClosed=false;
  }
  if (leftResult>rightResult)
    return output.MakeAtom("\\emptyset", theCommands);
  if (leftIsClosed && rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalClosed(), leftFinal, rightFinal);
  if (!leftIsClosed && rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalRightClosed(), leftFinal, rightFinal);
  if (leftIsClosed && !rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalLeftClosed(), leftFinal, rightFinal);
  if (!leftIsClosed && !rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opSequence(), leftFinal, rightFinal);
  return false;
}

bool CalculatorFunctionsGeneral::innerUnionUnionIntervals
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerUnionUnionIntervals");
  if (input.size()!=3)
    return false;
  const Expression& leftE=input[1];
  const Expression& rightComposite=input[2];
  if (!rightComposite.StartsWith(theCommands.opUnion(), 3))
    return false;
  const Expression& rightE=input[2][1];
  Expression theMiddleUnion, middleUnionReduced;
  theMiddleUnion.MakeXOX(theCommands, theCommands.opUnion(), leftE, rightE);
  if (!CalculatorFunctionsGeneral::innerUnionIntervals(theCommands, theMiddleUnion, middleUnionReduced))
    return false;
  return output.MakeXOX(theCommands, theCommands.opUnion(), middleUnionReduced, rightComposite[2]);
}

bool CalculatorFunctionsGeneral::innerUnionIntervals
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerUnionIntervals");
  if (input.size()!=3)
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if(!leftE.IsIntervalRealLine())
    return false;
  if(!rightE.IsIntervalRealLine())
    return false;
  double left1=0, left2=0, right1=0, right2=0;
  if (!leftE[1].EvaluatesToDouble(&left1) || !leftE[2].EvaluatesToDouble(&left2))
    return false;
  if (!rightE[1].EvaluatesToDouble(&right1) || !rightE[2].EvaluatesToDouble(&right2))
    return false;
  if (left1>left2 || right1>right2)
    return false;
  bool makeUnion=false;
  if (right1<=left2 && left2<=right2)
    makeUnion=true;
  if (right1==left2 &&
      leftE.StartsWith(theCommands.opIntervalOpen(), 3) &&
      rightE.StartsWith(theCommands.opIntervalOpen(), 3))
    makeUnion=false;
  if (!makeUnion)
    return false;
  bool leftIsClosed=false;
  bool rightIsClosed=false;
  Expression leftFinal, rightFinal;
  if (left1<right1)
  { leftFinal=leftE[1];
    if (leftE.StartsWith(theCommands.opIntervalClosed()) ||
        leftE.StartsWith(theCommands.opIntervalLeftClosed()))
      leftIsClosed=true;
  }
  if (left1==right1)
  { leftFinal=leftE[1];
    if (leftE .StartsWith(theCommands.opIntervalClosed()) ||
        leftE .StartsWith(theCommands.opIntervalLeftClosed())||
        rightE.StartsWith(theCommands.opIntervalClosed()) ||
        rightE.StartsWith(theCommands.opIntervalLeftClosed())
        )
      leftIsClosed=true;
  }
  if (left1>right1)
  { leftFinal=rightE[1];
    if (rightE.StartsWith(theCommands.opIntervalClosed()) ||
        rightE.StartsWith(theCommands.opIntervalLeftClosed())
        )
      leftIsClosed=true;
  }
///////////////////////
  if (left2>right2)
  { rightFinal=leftE[2];
    if (leftE.StartsWith(theCommands.opIntervalClosed()) ||
        leftE.StartsWith(theCommands.opIntervalRightClosed()))
      rightIsClosed=true;
  }
  if (left2==right2)
  { rightFinal=rightE[2];
    if (leftE .StartsWith(theCommands.opIntervalClosed()) ||
        leftE .StartsWith(theCommands.opIntervalRightClosed())||
        rightE.StartsWith(theCommands.opIntervalClosed()) ||
        rightE.StartsWith(theCommands.opIntervalRightClosed())
        )
      rightIsClosed=true;
  }
  if (left2<right2)
  { rightFinal=rightE[2];
    if (rightE.StartsWith(theCommands.opIntervalClosed()) ||
        rightE.StartsWith(theCommands.opIntervalRightClosed())
        )
      rightIsClosed=true;
  }
  if (leftIsClosed && rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalClosed(), leftFinal, rightFinal);
  if (!leftIsClosed && rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalRightClosed(), leftFinal, rightFinal);
  if (leftIsClosed && !rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalLeftClosed(), leftFinal, rightFinal);
  if (!leftIsClosed && !rightIsClosed)
    return output.MakeXOX(theCommands, theCommands.opIntervalOpen(), leftFinal, rightFinal);
  return false;
}

bool CalculatorFunctionsGeneral::innerNormalizeIntervals
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerNormalizeIntervals");
  List<Expression> outputList;
  if (!theCommands.CollectOpands(input, theCommands.opUnion(), outputList))
    return false;
  outputList.QuickSortAscending(CalculatorFunctionsGeneral::LeftIntervalGreaterThanRight);
  return output.MakeXOXOdotsOX(theCommands, theCommands.opUnion(), outputList);
}

bool CalculatorFunctionsGeneral::innerUnionEmptySet
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerUnionEmptySet");
  if (!input.StartsWith(theCommands.opUnion(), 3))
    return false;
  if (input[1].IsAtomGivenData("\\emptyset"))
  { output=input[2];
    return true;
  }
  if (input[2].IsAtomGivenData("\\emptyset"))
  { output=input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerIntersectUnion
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntersectUnion");
  return theCommands.outerDistribute(theCommands, input, output, theCommands.opUnion(), theCommands.opIntersection(), false);
}

bool CalculatorFunctionsGeneral::innerCompareIntervalsNumerically(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompareIntervalsNumerically");
  if (input.size()!=4)
    return theCommands << "Comparing intervals numerically takes 3 arguments: two unions and precision. ";
  double precision=0;
  if (!input[3].EvaluatesToDouble(&precision))
    return theCommands << "Could not extract precision from the last argument.";
  List<Expression> leftList, rightList;
  if (!theCommands.CollectOpands(input[1], theCommands.opUnion(), leftList))
    return false;
  if (!theCommands.CollectOpands(input[2], theCommands.opUnion(), rightList))
    return false;
  if (leftList.size!=rightList.size)
    return output.AssignValue(0, theCommands);
  leftList.QuickSortAscending(CalculatorFunctionsGeneral::LeftIntervalGreaterThanRight);
  rightList.QuickSortAscending(CalculatorFunctionsGeneral::LeftIntervalGreaterThanRight);
  for (int i=0; i<leftList.size; i++)
  { if (leftList[i]==rightList[i])
      continue;
    if ((!leftList[i].IsIntervalRealLine() ) || (!rightList[i].IsIntervalRealLine()))
      return output.AssignValue(0, theCommands);
    if (leftList[i][0]!=rightList[i][0])
      return output.AssignValue(0, theCommands);
    double left1=0, left2=0, right1=0, right2=0;
    if (! leftList[i][1].EvaluatesToDouble(&left1) ||
        ! rightList[i][1].EvaluatesToDouble(&right1)
        )
      if (leftList[i][1]!=rightList[i][1])
        return output.AssignValue(0, theCommands);
    if (! rightList[i][2].EvaluatesToDouble(&right2)||
        ! leftList[i][2].EvaluatesToDouble(&left2)
        )
      if (leftList[i][2]!=rightList[i][2])
        return output.AssignValue(0, theCommands);
    //std::stringstream temp;
    //temp.precision(20);
    //temp << "left1: " << FloatingPoint::DoubleToString(left1) << " left2: " << FloatingPoint::DoubleToString(left2)
    //<< " right1: " << FloatingPoint::DoubleToString(right1) << " right2: " << FloatingPoint::DoubleToString(right2)
    //<< " diff1: " << FloatingPoint::DoubleToString(FloatingPoint::abs(left1-right1))
    //<< " diff2: " << FloatingPoint::DoubleToString(FloatingPoint::abs(left2-right2));
    //stOutput << temp.str();
    if (FloatingPoint::abs(left1-right1)>precision ||
        FloatingPoint::abs(left2-right2)>precision)
      return output.AssignValue(0, theCommands);
  }
  return output.AssignValue(1, theCommands);

}

bool CalculatorFunctionsGeneral::innerIntersectEmptySet
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntersectEmptySet");
  if (!input.StartsWith(theCommands.opIntersection(), 3))
    return false;
  if (input[1].IsAtomGivenData("\\emptyset"))
  { output=input[1];
    return true;
  }
  if (input[2].IsAtomGivenData("\\emptyset"))
  { output=input[2];
    return true;
  }
  return false;

}

bool CalculatorFunctionsGeneral::innerIsLinearOrConstantIn(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsLinearOrConstantIn");
  if (input.size()<3)
    return false;
  List<List<Expression> > theSummands;
  if (!theCommands.GetSumProductsExpressions(input[2], theSummands))
    return theCommands << "Failed to extract sum from "
    << input[2].ToString();
  for(int i=0; i<theSummands.size; i++)
  { bool found=false;
    for (int j=0; j<theSummands[i].size; j++)
      if (theSummands[i][j]==input[1])
      { if (found)
          return output.AssignValue(0, theCommands);
        found=true;
      } else if (! theSummands[i][j].EvaluatesToDouble())
        return output.AssignValue(0, theCommands);
  }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsProductLinearOrConstTermsIn(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsProductLinearOrConstTermsIn");
  if (input.size()<3)
    return false;
  List<Expression> theMultiplicands;
  if (!theCommands.CollectOpands(input[2], theCommands.opTimes(),theMultiplicands))
    return theCommands << "Could not extract multiplicands from: "
    << input[2].ToString();
  for (int k=0; k<theMultiplicands.size; k++)
  { List<List<Expression> > theSummands;
    if (!theCommands.GetSumProductsExpressions(theMultiplicands[k], theSummands))
      return theCommands << "Failed to extract sum from "
      << theMultiplicands[k].ToString();
    for(int i=0; i<theSummands.size; i++)
    { bool found=false;
      for (int j=0; j<theSummands[i].size; j++)
        if (theSummands[i][j]==input[1])
        { if (found)
            return output.AssignValue(0, theCommands);
          found=true;
        } else if (! theSummands[i][j].EvaluatesToDouble())
          return output.AssignValue(0, theCommands);
    }
  }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerOrIdentical(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerOrIdentical");
  (void) theCommands;
  if (!input.StartsWithGivenAtom("or",3))
    return false;
  if (input[1]==input[2])
  { output=input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerSineOfAngleSumToTrig
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSineOfAngleSumToTrig");
  if (!input.StartsWith(theCommands.opPlus(),3))
    return false;
  Expression sinA, sinB, cosA, cosB;
  sinA.MakeOX(theCommands, theCommands.opSin(),input[1]);
  sinB.MakeOX(theCommands, theCommands.opSin(),input[2]);
  cosA.MakeOX(theCommands, theCommands.opCos(),input[1]);
  cosB.MakeOX(theCommands, theCommands.opCos(),input[2]);
  output= sinA*cosB+cosA*sinB;
  return true;
}

bool CalculatorFunctionsGeneral::innerTrigSumToTrigProduct
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTrigSumToTrigProduct");
  if (!input.StartsWith(theCommands.opPlus(), 3))
    return false;
  Expression leftE=input[1], rightE=input[2];
  bool isGood=false;
  int leftSign=1;
  int rightSign=1;
  if (leftE.StartsWith(theCommands.opTimes(),3))
    if (leftE[1].IsEqualToMOne())
    { leftE=leftE[2];
      leftSign=-1;
    }
  if (rightE.StartsWith(theCommands.opTimes(),3))
    if (rightE[1].IsEqualToMOne())
    { rightE=rightE[2];
      rightSign=-1;
    }

  if (leftE.StartsWith(theCommands.opSin(), 2) &&
      rightE.StartsWith(theCommands.opSin(), 2))
    isGood=true;
  else if ( leftE.StartsWith(theCommands.opCos(), 2) &&
            rightE.StartsWith(theCommands.opCos(), 2))
    isGood=true;
  if (!isGood)
    return false;
  Expression argSum, argDiff, leftMultiplicand, rightMultiplicand;
  if (leftE.StartsWith(theCommands.opSin(),2))
  { //stOutput << "DEBUG: leftSign: " << leftSign << "Right sign: " << rightSign;
    argSum=  (leftE[1]*leftSign+ rightE[1]*rightSign)/2;
    argDiff= (leftE[1]*leftSign- rightE[1]*rightSign)/2;
    leftMultiplicand.MakeOX(theCommands, theCommands.opCos(), argDiff);
    rightMultiplicand.MakeOX(theCommands, theCommands.opSin(), argSum);
    output=leftMultiplicand*rightMultiplicand*2;
    return true;
  } else
  { if (leftSign== rightSign)
    { argSum=(leftE[1]+  rightE[1])/2;
      argDiff= (leftE[1]-rightE[1])/2;
      leftMultiplicand.MakeOX(theCommands, theCommands.opCos(), argDiff);
      rightMultiplicand.MakeOX(theCommands, theCommands.opCos(), argSum);
      output=leftMultiplicand*rightMultiplicand*2;
      if (leftSign==-1)
        output*=-1;
      return true;
    } else
    { argSum=(leftE[1]+  rightE[1])/2;
      argDiff= (leftE[1]*leftSign+rightE[1]*rightSign)/2;
      leftMultiplicand.MakeOX(theCommands, theCommands.opSin(), argDiff);
      rightMultiplicand.MakeOX(theCommands, theCommands.opSin(), argSum);
      output=leftMultiplicand*rightMultiplicand*2;
      return true;
    }
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerCosineOfAngleSumToTrig
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCosineOfAngleSumToTrig");
  if (!input.StartsWith(theCommands.opPlus(),3))
    return false;
  Expression sinA, sinB, cosA, cosB;
  sinA.MakeOX(theCommands, theCommands.opSin(),input[1]);
  sinB.MakeOX(theCommands, theCommands.opSin(),input[2]);
  cosA.MakeOX(theCommands, theCommands.opCos(),input[1]);
  cosB.MakeOX(theCommands, theCommands.opCos(),input[2]);
  output= cosA*cosB-sinA*sinB;
  return true;
}

bool CalculatorFunctionsGeneral::innerIsAlgebraicRadical(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsAlgebraicRadical");
  //stOutput << "<br>Evaluating isconstant on: " << input.ToString();
  int result=(int) input.IsAlgebraicRadical();
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerExpressionToUTF8String
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExpressionToUTF8String");
  if (input.StartsWithGivenAtom("ToUTF8String"))
  { Expression inputCopy=input;
    inputCopy.SetChildAtomValue(0, theCommands.opSequence());
    return output.AssignValue(inputCopy.ToUTF8String(), theCommands);
  }
  return output.AssignValue(input.ToUTF8String(), theCommands);
}

bool CalculatorFunctionsGeneral::innerMatrixComputeTypeComposite(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMatrixComputeTypeComposite");
  if (input.size()<1)
    return false;
  if (!input[0].StartsWith(theCommands.opMatriX(), 1))
    return false;
  //stOutput << "DEBUG: Here be i";
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressions(input, theMat))
    return false;
  Expression conversionAttempt;
  conversionAttempt.AssignMatrixExpressions(theMat, theCommands, true);
  if (!conversionAttempt.IsMatrix())
  { output=conversionAttempt;
    return true;
  }
  if (conversionAttempt.size()>=1)
    if (conversionAttempt[0].StartsWith(theCommands.opMatriX()))
      if (conversionAttempt[0].size()>1)
      { output=conversionAttempt;
        return true;
      }
  return false;
}

bool CalculatorFunctionsGeneral::innerIsProductTermsUpToPower(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsProductTermsUpToPower");
  if (input.size()<3)
    return false;
  Expression theBase;
  theBase=input[1];
  LargeInt desiredMaxPower=1;
  if (theBase.StartsWith(theCommands.opThePower(), 3))
  { if (theBase[2].IsInteger(&desiredMaxPower))
    { if (desiredMaxPower>0)
        theBase=input[1][1];
      else
        desiredMaxPower=1;
    } else
      desiredMaxPower=1;
  }
  List<Expression> theMultiplicands;
  if (!theCommands.CollectOpands(input[2], theCommands.opTimes(), theMultiplicands))
    return theCommands << "Could not extract multiplicands from: "
    << input[2].ToString();

  for (int k=0; k<theMultiplicands.size; k++)
  { List<List<Expression> > theSummands;
    if (!theCommands.GetSumProductsExpressions(theMultiplicands[k], theSummands))
      return theCommands << "Failed to extract sum from "
      << theMultiplicands[k].ToString();
    for(int i=0; i<theSummands.size; i++)
    { LargeInt foundPower=0;
      for (int j=0; j<theSummands[i].size; j++)
      { if (theSummands[i][j]==theBase)
        { foundPower++;
          continue;
        }
        if (theSummands[i][j].StartsWith(theCommands.opThePower(),3))
          if (theSummands[i][j][1]==theBase)
          { LargeInt localPower;
            if (theSummands[i][j][2].IsInteger(&localPower))
            { foundPower+=localPower;
              continue;
            }
          }
        if (! theSummands[i][j].EvaluatesToDouble())
          return output.AssignValue(0, theCommands);
      }
      if (foundPower>desiredMaxPower)
        return output.AssignValue(0, theCommands);
    }
  }
  return output.AssignValue(1, theCommands);
}
