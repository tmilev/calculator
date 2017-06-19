
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
    << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
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
      output.AssignValue(0, theCommands);
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
  BoundVariablesSubstitution matchedExpressions;
  if(!theCommands.ExpressionMatchesPattern(input[2], input[1], matchedExpressions))
    return output.AssignValue(0, theCommands);
  Expression commandList;
  commandList.reset(theCommands);
  commandList.AddChildAtomOnTop(theCommands.opEndStatement());
  for (int i=0; i<matchedExpressions.theBoundVariables.size; i++)
  { Expression currentCommand;
    //stOutput << "<hr>DEBUG: Bound var: " << matchedExpressions.theBoundVariables[i].ToString()
    //<< ", image: " << matchedExpressions.variableImages[i].ToString();
    if (!matchedExpressions.theBoundVariables[i].StartsWith(theCommands.opBind(),2) )
    { std::stringstream errorStream;
      errorStream << "Bound variable " << matchedExpressions.theBoundVariables[i].ToString()
      << " does not start with the bind atom. ";
      return output.MakeError(errorStream.str(), theCommands);
    }
    currentCommand.MakeXOX
    (theCommands, theCommands.opDefine(), matchedExpressions.theBoundVariables[i][1],
     matchedExpressions.variableImages[i]);
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
