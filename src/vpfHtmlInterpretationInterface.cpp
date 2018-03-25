//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader8HtmlInterpretation.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceHtmlInterpretationInterfaceImplementation
(__FILE__, "Routines for calculus teaching: calculator exam mode.");

std::string HtmlInterpretation::GetProblemSolution()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetProblemSolution");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  std::stringstream out;
  theProblem.LoadCurrentProblemItem(false, theGlobalVariables.GetWebInput("randomSeed"));
  if (!theProblem.flagLoadedSuccessfully)
  { out << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    if(theProblem.comments.str() != "")
      out << " Comments: " << theProblem.comments.str();
    return out.str();
  }
  if (theProblem.flagIsForReal)
  { out << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    return out.str();
  }
  if(theGlobalVariables.GetWebInput("randomSeed") == "")
  { out << " <b>I could not figure out the exercise problem (missing random seed). </b>";
    return out.str();
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
  { stOutput << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
    return out.str();
  }
  std::string lastStudentAnswerID;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs = theGlobalVariables.webArguments;
  for (int i = 0; i < theArgs.size(); i ++)
    MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1)
  { out << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b><br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    { out << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
      //out << "<hr>Client input: " << this->mainArgumentRAW << "<hr>";
      out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
    }
    return out.str();
  }
  Answer& currentA = theProblem.theProblemData.theAnswers[indexLastAnswerId];
  Calculator theInterpreteR;
  theInterpreteR.init();
  theInterpreteR.flagPlotNoControls = true;
  theInterpreteR.flagWriteLatexPlots = false;
  if(!theProblem.PrepareCommands(comments))
  { out << "<b>Failed to prepare calculator commands. </b> <br>Comments:<br>" << comments.str();
    return out.str();
  }
  if (currentA.solutionElements.size == 0)
  { out << "<b> Unfortunately there is no solution given for this question (answerID: " << lastStudentAnswerID << ").";
    return out.str();
  }
  std::stringstream answerCommands, answerCommandsNoEnclosures;
  answerCommands << "CommandEnclosure{}(" << currentA.commandsBeforeAnswer << "); "
  << currentA.commandsSolutionOnly;
  answerCommandsNoEnclosures
  << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING
  << currentA.commandsSolutionOnly;
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors != "")
  { out << "<span style=\"color:red\"><b>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, false);
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  if (theInterpreteR.flagAbortComputationASAP)
  { out << "<span style=\"color:red\"><b>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  if (!theProblem.InterpretProcessExecutedCommands(theInterpreteR, currentA.solutionElements, out))
    return out.str();
  for (int i = 0; i < currentA.solutionElements.size; i ++)
    if (!currentA.solutionElements[i].IsHidden())
      out << currentA.solutionElements[i].ToStringInterpretedBody();
  out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    out << "<hr>"
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosures.str(), false)
    << "\">Input link</a>"
    <<  "<br>" << theInterpreteR.outputString << "<hr>" << theInterpreteR.outputCommentsString
    << "<hr>Raw input: " << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  return out.str();
}

std::string HtmlInterpretation::GetSetProblemDatabaseInfoHtml()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetSetProblemDatabaseInfoHtml");
#ifdef MACRO_use_MySQL
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "<b>Only admins may set problem weights.</b>";
  CalculatorHTML theProblem;
  std::string inputProblemInfo = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  theProblem.topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
  std::stringstream commentsOnFailure;
  if (!theProblem.LoadAndParseTopicList(commentsOnFailure))
    return "Failed to load topic list from file name: " + theProblem.topicListFileName + ". "+ commentsOnFailure.str();
  //stOutput << "DEBUG: userDefault: " << theGlobalVariables.userDefault.ToStringUnsecure();
  theProblem.currentUseR.UserCalculatorData::operator=(theGlobalVariables.userDefault);
  std::stringstream out;
  if(!theProblem.PrepareSectionList(commentsOnFailure))
  { out << "<span style=\"color:red\"><b>Failed to prepare section list. </b></span>" << commentsOnFailure.str();
    return out.str();

  }
  if (!theProblem.ReadProblemInfoAppend
      (theProblem.currentUseR.courseInfo.deadlinesString,
       theProblem.currentUseR.theProblemData, out))
  { out << "Failed to interpret the deadline string. ";
    return out.str();
  }
  if (!theProblem.ReadProblemInfoAppend
      (theProblem.currentUseR.courseInfo.problemWeightString, theProblem.currentUseR.theProblemData, out))
  { out << "Failed to interpret the problem weight string. ";
    return out.str();
  }
  if (theProblem.MergeProblemInfoInDatabase(inputProblemInfo, commentsOnFailure))
  { out << "<span style=\"color:green\"><b>Modified. </b></span>";
    //out << "<meta http-equiv=\"refresh\" content=\"0;\">";
  } else
    out << "<span style=\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
  //out << "<br>Debug message:<br>inputProblemInfo raw: " << inputProblemInfo << "<br>Processed: "
  //<< HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(inputProblemInfo)
  //<< "<br>inputProblemHome: " << inputProblemHome;
  return out.str();
#else
  return "Cannot modify problem weights (no database available)";
#endif // MACRO_use_MySQL
}

std::string HtmlInterpretation::GetSanitizedComment
(const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot)
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetSanitizedComment");
  theFormat.flagUseQuotes = false;
  resultIsPlot = false;
  //if (input.IsOfType<std::string>(&theString))
  //{ stOutput << "<br>DEBUG: string input: "
  //  << theString;
  //}
  std::string theString;
  if (input.IsOfType<std::string>(&theString))
  { if (MathRoutines::StringBeginsWith(theString, "Approximations have been"))
      return "";
    return input.ToString(&theFormat);
  }
  if (input.IsOfType<Plot>())
  { resultIsPlot = true;
    return input.ToString(&theFormat);
  }
  if (input.HasType<Plot>())
    return "";
  //<- expression has a partially drawn plot-> not displaying.
  if (input.owner == 0)
    return "";
  if (input.StartsWith(input.owner->opRulesOff()) ||
      input.StartsWith(input.owner->opRulesOn()))
    return "";
  return input.ToString(&theFormat);
}

std::string HtmlInterpretation::GetCommentsInterpretation
(Calculator& theInterpreterWithAdvice,
 int indexShift, FormatExpressions& theFormat)
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetCommentsInterpretation");
  std::stringstream out;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theInterpreterWithAdvice.theObjectContainer.resetPlots();
  //stOutput << "DEBUG: theInterpreterWithAdvice.flagPlotNoCtrls: "
  //<< theInterpreterWithAdvice.flagPlotNoControls;
  if (indexShift >= theInterpreterWithAdvice.theProgramExpression.size())
  { stOutput << "<br>DEBUG: something is very wrong with indexshift in comments!";
    return "";
  }
  const Expression& currentE = theInterpreterWithAdvice.theProgramExpression[indexShift][1];
  //out << "<br>DEBUG: currentE: " << HtmlRoutines::ConvertStringToHtmlString(currentE.ToString(), true);
  bool resultIsPlot = false;
  if (!currentE.StartsWith(theInterpreterWithAdvice.opEndStatement()))
  { //out << "<hr>DEBUG: currentE is not starting with commands!<hr>";
    out << HtmlInterpretation::GetSanitizedComment
    (currentE, theFormat, resultIsPlot);
    return out.str();
  }
  //out << "<hr>DEBUG: case currentE is commandList!<hr>CurrentE: "
  //<< HtmlRoutines::StringToHtmlString(currentE.ToString(), true)
  //;
  std::string currentS;
  for (int i = 1; i < currentE.size(); i ++)
  { currentS = HtmlInterpretation::GetSanitizedComment(currentE[i], theFormat,resultIsPlot);
    if (MathRoutines::StringTrimWhiteSpace(currentS) == "")
      continue;
    out << currentS;
    //out << "<br>DEBUG: Lispified: "
    //<< HtmlRoutines::ConvertStringToHtmlString(currentE[i].ToString(), true);
    if (i != currentE.size() - 1 && !resultIsPlot)
      out << "<br>";
  }
  return out.str();
}

std::string HtmlInterpretation::SubmitProblemPreview()
{ MacroRegisterFunctionWithName("HtmlInterpretation::SubmitProblemPreview");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  std::stringstream out, studentAnswerSream;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs =
  theGlobalVariables.webArguments;
  for (int i = 0; i < theArgs.size(); i ++)
    if (MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID))
      lastAnswer = "(" + HtmlRoutines::ConvertURLStringToNormal(theArgs[i], false) + "); ";
  studentAnswerSream << lastAnswer;
  out << "Your answer(s): \\(\\displaystyle "
  << lastAnswer << "\\)" << "\n<br>\n";
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem
  (theGlobalVariables.UserRequestRequiresLoadingRealExamData(), theGlobalVariables.GetWebInput("randomSeed"));
  if (!theProblem.flagLoadedSuccessfully)
    out << "<br><b>Failed to load problem.</b> Comments: " << theProblem.comments.str();
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1)
  { out << "<br>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime
    << " second(s).";
    return out.str();
  }
  Answer& currentA = theProblem.theProblemData.theAnswers[indexLastAnswerId];
  if (!theProblem.PrepareCommands(comments))
  { out << "Something went wrong while interpreting the problem file. ";
    if (theGlobalVariables.UserDebugFlagOn() &&
        theGlobalVariables.UserDefaultHasAdminRights())
      out << comments.str();
    out << "<br>Response time: "
    << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  Calculator theInterpreteR;
  theInterpreteR.flagUseLnInsteadOfLog = true;
  theInterpreteR.init();
  theInterpreteR.flagWriteLatexPlots = false;
  theInterpreteR.flagPlotNoControls = true;
  std::stringstream studentAnswerWithComments;
  studentAnswerWithComments
  << "CommandEnclosure{}("
  << currentA.commandsCommentsBeforeInterpretatioN
  << ");"
  << studentAnswerSream.str();

  theInterpreteR.Evaluate(studentAnswerWithComments.str());
  if (theInterpreteR.syntaxErrors != "")
  { out << "<span style=\"color:red\"><b>Failed to parse your answer, got:</b></span><br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, true);
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  } else if (theInterpreteR.flagAbortComputationASAP)
  { out << "<span style=\"color:red\"><b>Failed to evaluate your answer, got:</b></span><br>"
    << theInterpreteR.outputString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUsePmatrix = true;
  const Expression& studentAnswerNoContextE =
  theInterpreteR.theProgramExpression[theInterpreteR.theProgramExpression.size() - 1];
  out << "<span style=\"color:magenta\"><b>Interpreting as:</b></span><br>";
  out << "\\(\\displaystyle "
  << studentAnswerNoContextE.ToString(&theFormat) << "\\)";
  Calculator theInterpreterWithAdvice;
  theInterpreterWithAdvice.flagUseLnInsteadOfLog = true;
  theInterpreterWithAdvice.init();
  theInterpreterWithAdvice.flagWriteLatexPlots = false;
  theInterpreterWithAdvice.flagPlotNoControls = true;
  std::stringstream calculatorInputStream,
  calculatorInputStreamNoEnclosures;

  calculatorInputStream << "CommandEnclosure{}("
  << currentA.commandsBeforeAnswer << ");";
  calculatorInputStreamNoEnclosures
  << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;

  calculatorInputStream << "CommandEnclosure{}("
  << currentA.answerId << " = " << lastAnswer
  << ");";
  calculatorInputStreamNoEnclosures
  << currentA.answerId << " = " << lastAnswer << "";
  //stOutput << "<br>DEBUG: calculatorInputStreamNoEnclosures: " << calculatorInputStreamNoEnclosures.str();
  bool hasCommentsBeforeSubmission =
  (MathRoutines::StringTrimWhiteSpace
  (currentA.commandsCommentsBeforeSubmission) != "");
  if (hasCommentsBeforeSubmission)
  { calculatorInputStream << "CommandEnclosure{}("
    <<  currentA.commandsCommentsBeforeSubmission
    << ");";
    calculatorInputStreamNoEnclosures
    << currentA.commandsCommentsBeforeSubmission;
    //stOutput << "<br>DEBUG: commandsCommentsBeforeSubmission: " << currentA.commandsCommentsBeforeSubmission;
  }
  std::stringstream problemLinkStream;
  problemLinkStream
  << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
  << "?request=calculator&mainInput="
  << HtmlRoutines::ConvertStringToURLString(calculatorInputStreamNoEnclosures.str(), false)
  << "\">Input link</a>";
  theInterpreterWithAdvice.Evaluate(calculatorInputStream.str());
  if (theInterpreterWithAdvice.syntaxErrors != "")
  { out << "<br><span style=\"color:red\"><b>"
    << "Something went wrong when parsing your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights())
      out
      << problemLinkStream.str()
      << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  if (theInterpreterWithAdvice.flagAbortComputationASAP )
  { out << "<br><span style=\"color:red\"><b>"
    << "Something went wrong when interpreting your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
    { out << "<br>Logged-in as admin with debug flag on=> printing error details. "
      << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
      out << "<hr><b>Calculator input:</b> "
      << problemLinkStream.str() << "<br>"
      << calculatorInputStream.str();
    }
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  if (hasCommentsBeforeSubmission)
    out << HtmlInterpretation::GetCommentsInterpretation
    (theInterpreterWithAdvice, 3, theFormat);
  out << "<br>Response time: "
  << theGlobalVariables.GetElapsedSeconds() - startTime
  << " second(s).<hr>";
  if (theGlobalVariables.UserDefaultHasAdminRights() &&
      theGlobalVariables.UserDebugFlagOn())
  { out << "<hr> " << problemLinkStream.str()
    << "<br>"
    << calculatorInputStreamNoEnclosures.str()
    << "<br>"
    << "Result:<br>"
    << theInterpreterWithAdvice.outputString
    << "<br>" << theInterpreterWithAdvice.outputCommentsString;
  }
  return out.str();
}

std::string HtmlInterpretation::ClonePageResult()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ClonePageResult");
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights() ||
      !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "<b>Cloning problems allowed only for logged-in admins under ssl connection. </b>";
  std::string fileNameResulT = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  std::string fileNameToBeCloned = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("fileName"), false);
  std::stringstream out;
  std::string startingFileString;
  if (!FileOperations::LoadFileToStringVirtual(fileNameToBeCloned, startingFileString, false, false, &out))
  { out << "Could not find file: " << fileNameToBeCloned;
    return out.str();
  }
  std::fstream theFile;
  if (FileOperations::FileExistsVirtual(fileNameResulT))
  { out << "<b>File: " << fileNameResulT << " already exists. </b>";
    return out.str();
  }
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, fileNameResulT, false, false, false))
  { out << "<b><span style=\"color:red\">Failed to open output file: " << fileNameResulT << ". </span></b>";
    return out.str();
  }
  theFile << startingFileString;
  theFile.close();
  std::stringstream svnAddCommand;
  std::string fileNameNonVirtual;
  std::stringstream commandResult;
  if (FileOperations::GetPhysicalFileNameFromVirtual(fileNameResulT, fileNameNonVirtual, false, false, &commandResult))
  { if (FileOperations::IsFileNameSafeForSystemCommands(fileNameNonVirtual, &commandResult))
    { svnAddCommand << "svn add " << fileNameNonVirtual;
      commandResult << "<b>Command:</b> " << svnAddCommand.str() << "<br>";
      commandResult << "<b>Result: </b>"
      << theGlobalVariables.CallSystemWithOutput(svnAddCommand.str());
    }
  } else
    commandResult << "SVN: Could not get physical file name from virtual. ";
  CalculatorHTML linkInterpreter;
  out << "<br>" << linkInterpreter.ToStringLinkFromFileName(fileNameResulT);
  out << "<br><b><span style=\"color:green\">Written content to file: "
  << fileNameResulT << ". </span></b>" << "<br> " << commandResult.str();
  return out.str();
}

std::string HtmlInterpretation::GetExamPageInterpreter()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetExamPageInterpreter");
  CalculatorHTML theFile;
  std::stringstream out;
  out << theFile.LoadAndInterpretCurrentProblemItem(false, theGlobalVariables.GetWebInput("randomSeed"));
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  return out.str();
}

std::string HtmlInterpretation::GetAboutPage()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetAboutPage");
  std::stringstream out;
  out << "<!DOCTYPE html>";
  out << "<html>";
  out << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator()
  << "</head>";
  out << "<body>";
  out << "<calculatorNavigation>"
  << HtmlInterpretation::ToStringNavigation()
  << "</calculatorNavigation>";
  std::string theFile;
  std::stringstream commentsOnFailure;
  bool isGood = FileOperations::LoadFileToStringVirtual
  ("/calculator-html/about.html", theFile, false, false, &commentsOnFailure);
  if (!isGood)
    isGood = FileOperations::LoadFileToStringVirtual("/html/about.html", theFile, false, false, &commentsOnFailure);
  if (!isGood)
  { out << "<span style=\"color:red\"><b>"
    << commentsOnFailure.str()
    << "</b></span>";
  } else
    out << theFile;
  out << "</body></html>";
  return out.str();
}

void HtmlInterpretation::BuildHtmlJSpage()
{ MacroRegisterFunctionWithName("HtmlInterpretation::BuildHtmlJSpage");
  std::stringstream out;
  std::stringstream theReader(this->htmlRaw);
  theReader.seekg(0);
  std::string theCalculatorHtmlFolder = "/calculator-html";
  for (std::string currentLine; std::getline(theReader, currentLine, '\n');)
  { int startChar = currentLine.find(theCalculatorHtmlFolder);
    if (startChar == - 1)
    { out << currentLine << "\n";
      continue;
    }
    std::string firstPart, secondAndThirdPart, thirdPart, notUsed;
    MathRoutines::SplitStringInTwo(currentLine, startChar, firstPart, secondAndThirdPart);
    MathRoutines::SplitStringInTwo(secondAndThirdPart, theCalculatorHtmlFolder.size(), notUsed, thirdPart);
    out << firstPart << FileOperations::GetVirtualNameWithHash(theCalculatorHtmlFolder) << thirdPart << "\n";
  }
  this->htmlJSbuild = out.str();
}

std::string HtmlInterpretation::GetApp(bool appendBuildHash)
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetApp");
  HtmlInterpretation theInterpretation;
  std::stringstream out;
  std::stringstream errorStream;
  if (!FileOperations::LoadFileToStringVirtual
      ("/calculator-html/new/index.html", theInterpretation.htmlRaw, false, false, &errorStream))
  { out << "<html><body><b>Failed to load the application file. "
    << "Further comments follow. " << errorStream.str() << "</body></html>";
    return out.str();
  }
  if (appendBuildHash)
  { theInterpretation.BuildHtmlJSpage();
    return theInterpretation.htmlJSbuild;
  }
  return theInterpretation.htmlRaw;
}

class Course
{
public:
  std::string courseTemplate;
  std::string courseTopics;
  std::string title;
  bool IsEmpty();
  void reset();
  std::string ToString() const;
};

std::string Course::ToString() const
{ std::stringstream out;
  out << "Html: " << this->courseTemplate
  << "\n" << "Topics: " << this->courseTopics;
  return out.str();
}

bool Course::IsEmpty()
{ return this->courseTemplate == "" && this->courseTopics == "" && this->title == "";
}

void Course::reset()
{ this->courseTemplate = "";
  this->courseTopics = "";
  this->title = "";
}

class CourseList
{
public:
  List<Course> theCourses;
  void LoadFromString(const std::string& input, std::stringstream* commentsOnFailure);
  std::string ToHtml();

};

std::string CourseList::ToHtml()
{ return this->theCourses.ToString();

}

void CourseList::LoadFromString(const std::string& input, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("CourseList::LoadFromString");
  (void) commentsOnFailure;
  std::stringstream tableReader(input);
  std::string currentLine, currentArgument;
  Course current;
  while (std::getline(tableReader, currentLine, '\n'))
  { if (MathRoutines::StringBeginsWith(currentLine, "Html:", &currentArgument))
    { if (current.courseTemplate != "")
      { this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.courseTemplate = MathRoutines::StringTrimWhiteSpace(currentArgument);
    }
    if (MathRoutines::StringBeginsWith(currentLine, "Topics:", &currentArgument))
    { if (current.courseTopics != "")
      { this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.courseTopics = MathRoutines::StringTrimWhiteSpace(currentArgument);
    }
    if (MathRoutines::StringBeginsWith(currentLine, "Title:", &currentArgument))
    { if (current.title != "")
      { this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.title = MathRoutines::StringTrimWhiteSpace(currentArgument);
    }
  }
  if (!current.IsEmpty())
    this->theCourses.AddOnTop(current);
}

std::string HtmlInterpretation::GetSelectCourseJSON()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetSelectCourseJSON");
  JSData output;
  std::stringstream comments;
  std::string coursesAvailableList = "/coursesavailable/default.txt";
  std::string theTopicFile;
  std::stringstream commentsOnFailure;
  std::string temp;
  FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly
  (coursesAvailableList, temp, &commentsOnFailure);
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly
      ("/coursesavailable/default.txt", theTopicFile, &commentsOnFailure))
  { comments << "Failed to fetch available courses from /coursesavailable/default.txt. "
    << commentsOnFailure.str();
    output["error"] = comments.str();
    return output.ToString(false);
  }
  CourseList theCourses;
  theCourses.LoadFromString(theTopicFile, &comments);
  output["courses"].type = JSData::JSarray;
  for (int i = 0; i < theCourses.theCourses.size; i ++)
  { JSData currentCourse;
    currentCourse["title"] = theCourses.theCourses[i].title;
    currentCourse["html"] = "coursetemplates/" + theCourses.theCourses[i].courseTemplate;
    currentCourse["topics"] = "topiclists/" + theCourses.theCourses[i].courseTopics;
    output["courses"].list.AddOnTop(currentCourse);
  }
  return output.ToString();
}

std::string HtmlInterpretation::GetSelectCourse()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetSelectCourse");
  std::stringstream out;
  out << "<!DOCTYPE html>";
  out << "<html>";
  out << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator()
  << "</head>";
  out << "<body>";
  out << "<calculatorNavigation>"
  << HtmlInterpretation::ToStringNavigation()
  << "</calculatorNavigation>";
  CalculatorHTML tempObject;
  std::string coursesAvailableList = "/coursesavailable/default.txt";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out
    << "<editPagePanel>"
    << tempObject.GetEditPageButton(coursesAvailableList, false)
    << "</editPagePanel>";

  std::string theTopicFile;
  std::stringstream commentsOnFailure;
  std::string temp;
  FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly
  (coursesAvailableList, temp, &commentsOnFailure);
  //stOutput << "DEBUG: coursesAvailableList: " << temp;
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly
      ("/coursesavailable/default.txt", theTopicFile, &commentsOnFailure))
  { out << "<b>Failed to fetch available courses from /coursesavailable/default.txt</b>. " << commentsOnFailure.str();
    out << " </body></html>";
    return out.str();
  }
  CourseList theCourses;
  theCourses.LoadFromString(theTopicFile, &out);
  out << "<div style=\"text-align:center\">";
  for (int i = 0; i < theCourses.theCourses.size; i ++)
  { out << "<a class=\"courseLink\" href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=template&courseHome=coursetemplates/"
    << theCourses.theCourses[i].courseTemplate
    << "&topicList=topiclists/"
    << theCourses.theCourses[i].courseTopics
    << "\">" << theCourses.theCourses[i].title << "</a>";
    if (i != theCourses.theCourses.size - 1)
      out << "<br>";
  }
  out << "</div>";
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetHtmlTagWithManifest()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetHtmlTagWithManifest");
  std::stringstream out;
  out << "<!DOCTYPE HTML>\n";
  out << "<html>\n<!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

std::string HtmlInterpretation::GetPageFromTemplate()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetPageFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  bool includeDeadlineJavascript =
  theGlobalVariables.UserDefaultHasAdminRights() &&
  !theGlobalVariables.UserStudentVieWOn();
  bool includeInitializeButtonsJS =
  theGlobalVariables.UserDefaultHasAdminRights();
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  if (!thePage.LoadMe(true, comments, theGlobalVariables.GetWebInput("randomSeed")))
  { out << "<html>"
    << "<head>" << HtmlRoutines::GetCSSLinkCalculator() << "</head>"
    << "<body>";
    out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation() << " </calculatorNavigation>"
    << "<b>Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    out << "</body></html>";
    return out.str();
  }
  if (!thePage.InterpretHtml(comments))
  { out << "<html>"
    << "<head>"
    << HtmlRoutines::GetCSSLinkCalculator()
    << "</head>"
    << "<body>";
    out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation() << " </calculatorNavigation>"
    << "<b>Failed to interpret as template the following file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    out << "</body></html>";
    return out.str();
  }
  out << HtmlInterpretation::GetHtmlTagWithManifest();
  out << "<!-- File automatically generated from template: "
  << theGlobalVariables.GetWebInput("fileName")
  << ".-->\n";
  out << "<head><!-- tag added automatically; user-specified head tag ignored-->\n";
  out << thePage.outputHtmlHeadNoTag;
  out << HtmlRoutines::GetJavascriptStandardCookiesWithTags();
  out << HtmlRoutines::GetJavascriptProblemLinksLink();
  out << HtmlRoutines::GetJavascriptInitializeButtonsLink();
  if (includeDeadlineJavascript || includeInitializeButtonsJS)
    out << HtmlRoutines::GetJavascriptCalculatorPageLink();
  if (theGlobalVariables.UserDefaultHasProblemComposingRights())
    out << HtmlRoutines::GetJavascriptTopicListLink();
  out << "</head><!-- tag added automatically; user-specified head tag ignored-->\n";
  out << "<body" //<< ">"
  << " onload=\"loadSettings();";
  out << " initializeButtonsCommon(); ";
  out <<"\"><!-- tag added automatically; user-specified body tag ignored-->\n";
  if (thePage.flagDoPrependCalculatorNavigationBar)
  { out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
    << "<small>Generated in " << theGlobalVariables.GetElapsedSeconds()
    << " second(s).</small>" << "</calculatorNavigation>\n";
  }
  if (thePage.flagDoPrependProblemNavigationBar)
    out << thePage.outputProblemNavigatioN;
  out << thePage.outputHtmlBodyNoTag;
  out << "</body><!-- tag added automatically; user-specified body tag ignored-->\n";
  out << "</html><!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

std::string HtmlInterpretation::GetTopicTableJSON()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetTopicTableJSON");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  if (!thePage.LoadMe(true, comments, theGlobalVariables.GetWebInput("randomSeed")))
  { out << "\"Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ""
    << "<br>Comments:<br> " << comments.str();
    out << "\"";
    return out.str();
  }
  thePage.FigureOutCurrentProblemList(comments);
  out << thePage.ToStringTopicListJSON();
  return out.str();
}

std::string HtmlInterpretation::GetJSONUserInfo()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetJSONUserInfo");
  if (! theGlobalVariables.flagLoggedIn)
    return "\"not logged in\"";
  JSData output;
  output["username"] = theGlobalVariables.userDefault.username.value;
  output["authenticationToken"] = theGlobalVariables.userDefault.actualAuthenticationToken.value;
  return output.ToString();
}

std::string HtmlInterpretation::GetJSONFromTemplate()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetJSONFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  thePage.flagUseJSON = true;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  if (!thePage.LoadMe(true, comments, theGlobalVariables.GetWebInput("randomSeed")))
  { out << "<b>Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  if (!thePage.InterpretHtml(comments))
  { out << "<b>Failed to interpret as template the following file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  out << "<!-- File automatically generated from template: "
  << theGlobalVariables.GetWebInput("fileName")
  << ".-->\n";
  out << thePage.outputHtmlBodyNoTag;
  out << "<small>Generated in " << theGlobalVariables.GetElapsedSeconds()
  << " second(s).</small>";
  return out.str();
}

std::string HtmlInterpretation::GetExamPage()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetExamPage");
  CalculatorHTML theFile;
  std::string problemBody = theFile.LoadAndInterpretCurrentProblemItem
  (theGlobalVariables.UserRequestRequiresLoadingRealExamData(),
   theGlobalVariables.GetWebInput("randomSeed"));
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << HtmlRoutines::GetJavascriptStandardCookiesWithTags() << "\n"
  << HtmlRoutines::GetJavascriptMathjax() << "\n";
  if (theFile.flagMathQuillWithMatrices)
    out << HtmlRoutines::GetJavascriptMathQuillMatrixSupportFull() << "\n";
  else
    out << HtmlRoutines::GetJavascriptMathQuillDefaultFull() << "\n";
  out << HtmlRoutines::GetMathQuillStyleSheetLink() << "\n"
  << HtmlRoutines::GetCSSLinkCalculator() << "\n"  ;
  out << HtmlRoutines::GetJavascriptInitializeButtonsLink() << "\n";
  if (theFile.flagLoadedSuccessfully)
    out << theFile.outputHtmlHeadNoTag;
  //<-must come after theFile.outputHtmlHeadNoTag
  out << "</head>"
  << "<body onload=\"loadSettings(); initializeMathQuill(); ";
  out << "initializeButtons();";
  out << "\">\n";
  out << problemBody;
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetBrowseProblems()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetBrowseProblems");
  std::stringstream out;
  out << "<!DOCTYPE html>";
  out << "<html>";
  out << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator()
  << "</head>";
  out << "<body>";
  out << "<calculatorNavigation>"
  << HtmlInterpretation::ToStringNavigation()
  << "</calculatorNavigation>";
  out << "Not implmented yet.";
  out << "</body></html>";
  return out.str();

}

std::string HtmlInterpretation::GetEditPageHTML()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetEditPageHTML");
  std::stringstream ouT, outHead, outBody;
  if ((!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights()) &&
      !theGlobalVariables.flagRunningApache)
    return "<b>Only logged-in admins are allowed to edit pages. </b>";
  CalculatorHTML theFile;
  theFile.LoadFileNames();
  ouT << "<html>";
  outHead << "<head>"
  << HtmlRoutines::GetJavascriptStandardCookiesWithTags()
  //  << HtmlRoutines::GetLaTeXProcessingJavascript()
  //  << HtmlRoutines::GetCalculatorStyleSheetWithTags()
  << HtmlRoutines::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << HtmlRoutines::GetCSSLinkCalculator()
  << "<style type=\"text/css\" media=\"screen\">\n"
  << "    #editor { \n"
  << "      height: 380px;\n"
  << "      font-size: 100%;\n"
  << "   }\n"
  << "</style>\n";
  outHead << HtmlRoutines::GetJavascriptAceEditorScriptWithTags();
  outBody << "<body onload=\"loadSettings();\">\n";
  outBody << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
  << "</calculatorNavigation>";
  std::stringstream failureStream;
  if (!theFile.LoadMe(false, failureStream, theGlobalVariables.GetWebInput("randomSeed")))
  { outBody << "<b>Failed to load file: " << theFile.fileName << ", perhaps the file does not exist. </b>";
    outBody << "</body></html>";
    ouT << outHead.str() << outBody.str();
    return ouT.str();
  }
  if (!theFile.ParseHTML(failureStream))
    outBody << "<b>Failed to parse file: " << theFile.fileName
    << ".</b> Details:<br>" << failureStream.str();
  if (theFile.flagIsExamProblem)
  { theFile.FigureOutCurrentProblemList(outBody);
    outBody << "<problemNavigation>" << theFile.ToStringProblemNavigation() << "</problemNavigation>";
  }
  HashedList<std::string, MathRoutines::hashString> theAutocompleteKeyWords;
  theFile.initBuiltInSpanClasses();
  if (theFile.flagIsExamProblem)
  { Calculator tempCalculator;
    tempCalculator.init();
    tempCalculator.ComputeAutoCompleteKeyWords();
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorClasses);
    theAutocompleteKeyWords.AddOnTopNoRepetition(tempCalculator.autoCompleteKeyWords);
    theFile.initAutocompleteExtras();
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.autoCompleteExtras);
  } else
  { theFile.LoadAndParseTopicList(outBody);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorClasses);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorClassesAnswerFields);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorTopicElementNames);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorTopicBundles);
  }
  outHead << "<script type=\"text/javascript\">\n";
  outHead << "var AceEditorAutoCompletionWordList=[";
  bool found = false;
  for (int i = 0; i < theAutocompleteKeyWords.size; i ++)
    if (theAutocompleteKeyWords[i].size() > 2)
    { if (found)
        outHead << ", ";
      found = true;
      outHead << "\"" << theAutocompleteKeyWords[i] << "\"";
    }
  outHead << "];\n";
  outHead << "\n</script>";
  outHead << "</head>";
  std::stringstream buttonStream, submitModPageJS;
  submitModPageJS
  << "submitStringAsMainInput(editor.getValue(), 'spanSubmitReport', 'modifyPage', null, 'spanSubmitReport');";
  buttonStream
  << "<button class=\"buttonSaveEdit\" "
  << "onclick=\"" << submitModPageJS.str() << "\" >Save changes</button>";
  //  out << "<form>";
  //  out << "<input type=\"submit\" value=\"Save changes\"> ";
  outBody << buttonStream.str();
  outBody << "To include the problem in your topic list, add the following two lines. <br>"
  << "<textarea cols=\"140\", rows=\"2\">"
  << theFile.ToStringCalculatorProblemSourceFromFileName(theFile.fileName) << "</textarea>";
  outBody << "<br>\n";
  outBody << "Ctrl+S saves your changes. Edit bravely, you are not overwriting the defaults, "
  << "but only editing your (and your students') version. ";
//  outBody << "<br>\n";
  outBody
  << "Many thanks to the <a href=\"https://ace.c9.io\">ace editor</a> project. <br>"
  << "<div id=\"editor\" onkeydown=\"ctrlSPress(event);\" name=\"editor\">"
  //<< "<textarea cols=\"150\", rows=\"30\" id=\"mainInput\" name=\"mainInput\" onkeydown=\"ctrlSPress(event);\">"
  ;
  outBody
  //<< "</textarea>"
  << "</div>"
  << "\n<br>\n";
  outBody << "<script type=\"text/javascript\"> \n"
  << "function ctrlSPress(event){\n"
  << "   if (event.ctrlKey!=true)\n"
  << "     return;\n"
  << "   if (event.keyCode!=83)\n"
  << "     return;\n"
  << "   event.preventDefault();"
  << submitModPageJS.str() << "\n"
  << "}\n"
  << "</script>\n";
  outBody
  << "<script src=\"/html-common/ace/src-min/ext-language_tools.js\"></script>";
  outBody << "<script type=\"text/javascript\"> \n"
  //<< " document.getElementById('mainInput').value=decodeURIComponent(\""
  << " document.getElementById('editor').textContent=decodeURIComponent(\""
  << HtmlRoutines::ConvertStringToURLString(theFile.inputHtml, false)
  << "\");\n"
  << "    ace.require(\"ace/ext/language_tools\");\n"
  << "    var editor = ace.edit(\"editor\");\n"
  << "    editor.setTheme(\"ace/theme/chrome\");\n"
  << "    editor.getSession().setMode(\"ace/mode/xml\");\n"
  << "    editor.setOptions({\n"
  << "      enableBasicAutocompletion: true,\n"
  << "      enableLiveAutocompletion: true\n"
  << "    });\n"
  << "    editor.completers = [staticWordCompleter];"
  << "    editor.$blockScrolling = Infinity;"
  << "</script>\n";
  outBody << buttonStream.str();
  outBody << "<span id=\"spanSubmitReport\"></span><br>";
  outBody << theFile.ToStringLinkFromFileName(theFile.fileName);
  //  out << "<input type=\"submit\" value=\"Save changes\">";
  //  out << "</form>";
  outBody << "</body>";
  ouT << outHead.str() << outBody.str() << "</html>";
  return ouT.str();
}

std::string HtmlInterpretation::SubmitProblem()
{ return HtmlInterpretation::SubmitProblem(theGlobalVariables.GetWebInput("randomSeed"), 0, true);
}

std::string HtmlInterpretation::SubmitProblem
(const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake)
{ MacroRegisterFunctionWithName("HtmlInterpretation::SubmitProblem");
  std::stringstream out;
  double startTime = theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem
  (theGlobalVariables.UserRequestRequiresLoadingRealExamData(), inputRandomSeed);
  if (!theProblem.flagLoadedSuccessfully)
  { out << "Failed to load problem. " << theProblem.comments.str();
    return out.str();
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
  { out << "<b>Failed to parse problem. </b>Comments: " << comments.str();
    return out.str();
  }
  if (!theProblem.theProblemData.flagRandomSeedGiven && !theProblem.flagIsForReal)
    out << "<b>Random seed not given.</b>";
//  stOutput << "<b>DEBUG remove when done: Random seed: " << theProblem.theProblemData.randomSeed << "</b>";
  if (theProblem.fileName == "")
    crash << "This shouldn't happen: empty file name: theProblem.fileName." << crash;
  std::string studentAnswerNameReader;
  theProblem.studentTagsAnswered.init(theProblem.theProblemData.theAnswers.size());
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs = theGlobalVariables.webArguments;
  int answerIdIndex = - 1;
  for (int i = 0; i < theArgs.size(); i ++)
    if (MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &studentAnswerNameReader))
    { int newAnswerIndex = theProblem.GetAnswerIndex(studentAnswerNameReader);
      if (answerIdIndex == - 1)
        answerIdIndex = newAnswerIndex;
      else if (answerIdIndex != newAnswerIndex && answerIdIndex != - 1 && newAnswerIndex != - 1)
      { out << "<b>You submitted two or more answers [answer tags: "
        << theProblem.theProblemData.theAnswers[answerIdIndex].answerId
        << " and " << theProblem.theProblemData.theAnswers[newAnswerIndex].answerId
        << "].</b> At present, multiple answer submission is not supported. ";
        return out.str();
      }
      if (answerIdIndex == - 1)
      { out << "<b> You submitted an answer to tag with id "
        << studentAnswerNameReader
        << " which is not on my list of answerable tags. </b>";
        return out.str();
      }
      Answer& currentProblemData = theProblem.theProblemData.theAnswers[answerIdIndex];
      currentProblemData.currentAnswerURLed = theArgs.theValues[i];
      if (currentProblemData.currentAnswerURLed == "")
      { out << "<b> Your answer to tag with id " << studentAnswerNameReader
        << " appears to be empty, please resubmit. </b>";
        return out.str();
      }
    }
  if (answerIdIndex == - 1)
  { out << "<b>Something is wrong: I found no submitted answers.</b>";
    return out.str();
  }
  ProblemData& currentProblemData = theProblem.theProblemData;
  Answer& currentA = currentProblemData.theAnswers[answerIdIndex];

  currentA.currentAnswerClean = HtmlRoutines::ConvertURLStringToNormal(currentA.currentAnswerURLed, false);
  currentA.currentAnswerURLed = HtmlRoutines::ConvertStringToURLString(currentA.currentAnswerClean, false);//<-encoding back to overwrite malformed input
  //stOutput << "<hr>DEBUG: Processing answer: " << currentA.currentAnswerClean << " to answer object: " << currentA.ToString();
  theProblem.studentTagsAnswered.AddSelectionAppendNewIndex(answerIdIndex);
  std::stringstream completedProblemStreamNoEnclosures;

  std::stringstream completedProblemStream;
  completedProblemStream << "CommandEnclosure{}("
  << currentA.commandsBeforeAnswer
  << ");";
  completedProblemStreamNoEnclosures << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;

  completedProblemStream << "CommandEnclosure{}("
  << currentA.answerId << "= (" << currentA.currentAnswerClean << ");"
  << ");"
  ;
  completedProblemStreamNoEnclosures << currentA.answerId << "= (" << currentA.currentAnswerClean << ");";

  //stOutput << "DEBUG: " << "adding: commands: " << currentA.commandsCommentsBeforeSubmissionOnly;
  bool hasCommentsBeforeSubmission =
  (MathRoutines::StringTrimWhiteSpace(currentA.commandsCommentsBeforeSubmission)!="");
  if (hasCommentsBeforeSubmission)
  { completedProblemStream
    << "CommandEnclosure{}("
    << currentA.commandsCommentsBeforeSubmission
    << ");";
    completedProblemStreamNoEnclosures << currentA.commandsCommentsBeforeSubmission;
  }
  completedProblemStream << theProblem.CleanUpCommandString(currentA.commandVerificationOnly);
  completedProblemStreamNoEnclosures << theProblem.CleanUpCommandString(currentA.commandVerificationOnly);

  std::stringstream debugInputStream;
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  { debugInputStream
    << "Input, no enclosures, direct link: "
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString(completedProblemStreamNoEnclosures.str(), false)
    << "\">Input link</a>";
  }


  //stOutput << "<br>DEBUG: input to the calculator: " << completedProblemStream.str() << "<hr>";
  if (timeSafetyBrake)
    theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds() + 20;
  Calculator theInterpreter;
  theInterpreter.init();
  theInterpreter.flagWriteLatexPlots = false;
  theInterpreter.flagPlotNoControls = true;

  theInterpreter.Evaluate(completedProblemStream.str());
  if (theInterpreter.flagAbortComputationASAP || theInterpreter.syntaxErrors != "")
  { if (theInterpreter.errorsPublic.str() != "")
      out << "While checking your answer, got the error: "
      << "<br><b><span style=\"color:red\">"
      << theInterpreter.errorsPublic.str()
      << "</span></b> "
      << "<br><b>Most likely your answer is wrong. </b>";
    else
      out << "<b>Error while processing your answer(s).</b> "
      << "<br>Perhaps your answer was wrong and "
      << "generated division by zero during checking. ";
    out << "<br>Here's what I understood. ";
    Calculator isolatedInterpreter;
    isolatedInterpreter.init();
    isolatedInterpreter.flagWriteLatexPlots = false;
    isolatedInterpreter.flagPlotNoControls = true;
    if (timeSafetyBrake)
      theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit = theGlobalVariables.GetElapsedSeconds() + 20;
    isolatedInterpreter.Evaluate("(" + currentA.currentAnswerClean + ")");
    if (isolatedInterpreter.syntaxErrors != "")
      out << isolatedInterpreter.ToStringSyntacticStackHumanReadable(false, true);
    else
      out << isolatedInterpreter.outputString;
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    { out << "<hr><b>Admin view internals.</b><hr>"
      << debugInputStream.str() << "<hr>"
      << theInterpreter.outputString
      << "<br>" << theInterpreter.outputCommentsString
      << "<hr>Input, no enclosures: <hr>"
      << completedProblemStreamNoEnclosures.str()
      << "<br>";
    }
//    stOutput << "yer input: " << completedProblemStream.str();
//    stOutput << theInterpreter.outputString;
    return out.str();
  }
  bool tempIsCorrect = false;
  if (outputIsCorrect == 0)
    outputIsCorrect = &tempIsCorrect;
  *outputIsCorrect = false;
  int mustBeOne = - 1;
  if (!theInterpreter.theProgramExpression[theInterpreter.theProgramExpression.size() - 1].IsSmallInteger(&mustBeOne))
    *outputIsCorrect = false;
  else
    *outputIsCorrect = (mustBeOne == 1);
  FormatExpressions theFormat;
  out << "<table width=\"300\">";
  if (!(*outputIsCorrect))
  { out << "<tr><td><span style=\"color:red\"><b>Your answer appears to be incorrect. </b></span></td></tr>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
    { out << "<tr><td>Admin view internals. "
      << "<hr>" << debugInputStream.str()
      << "<br>The calculator output is: " << theInterpreter.outputString
      << "Comments: " << theInterpreter.Comments.str()
      << "<hr>Input, no enclosures: <hr>"
      << completedProblemStreamNoEnclosures.str()
      ;
      out << "<hr>Input, full:<hr>"
      << theInterpreter.inputString << "<hr></td></tr>";
    }
  } else
  { out << "<tr><td><span style=\"color:green\"><b>Correct! </b></span>" << "</td></tr>";
  }
  if (hasCommentsBeforeSubmission)
    out << "<tr><td>" << HtmlInterpretation::GetCommentsInterpretation
    (theInterpreter, 3, theFormat) << "</td></tr>\n";
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  UserCalculator& theUser = theProblem.currentUseR;
  theUser.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  bool deadLinePassed = false;
  bool hasDeadline = true;
  double secondsTillDeadline = - 1;
  if (theProblem.flagIsForReal)
  { //out << "<tr><td><hr><hr><hr>DEBUG: before interpreting anything prob data is: "
    //<< theUser.theProblemData.ToStringHtml() << "<hr><hr><hr></td></tr>";
    if (!theProblem.LoadAndParseTopicList(out))
      hasDeadline = false;
    DatabaseData theSQLstring;
    theSQLstring = theUser.courseInfo.sectionComputed;
    if (hasDeadline)
    { bool unused = false;
      std::string theDeadlineString =
      theProblem.GetDeadline(theProblem.fileName, theSQLstring.GetDataNoQuotes(), unused);
      //out << "<tr><td>DEBUG: getting deadline for section: " << theUser.userGroup.value
      //<< "<br>The prob data is: "
      //<< theUser.theProblemData.ToStringHtml()
      //<< "<br> the dealineinfoString is: "
      //<< HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(theUser.deadlineInfoString.value)
      //<< " <br>getDeadline output: "
      //<< theProblem.GetDeadline(theProblem.fileName, theUser.userGroup.GetDataNoQuotes(), unused) << "</td></tr>";

      if (theDeadlineString == "" || theDeadlineString == " ")
        hasDeadline = false;
      else
      { TimeWrapper now, deadline; //<-needs a fix for different time formats.
        //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
        std::stringstream badDateStream;
        if (!deadline.AssignMonthDayYear(theDeadlineString, badDateStream))
        { out << "<tr><td><b>Problem reading deadline. </b> The deadline string was: "
          << theDeadlineString << ". Comments: "
          << "<span style=\"color:red\">" << badDateStream.str() << "</span>"
          << "</td></tr><tr><td> This should not happen. " << CalculatorHTML::BugsGenericMessage << "</td></tr>";
          return out.str();
        }
        now.AssignLocalTime();
        //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
        //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
        secondsTillDeadline = deadline.SubtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;
        deadLinePassed = (secondsTillDeadline < - 18000);
      }
    }
    if (deadLinePassed)
      out << "<tr><td><span style=\"color:red\"><b>Deadline passed, attempt not recorded.</b></span></td></tr>";
    else
    { currentA.numSubmissions ++;
      if ((*outputIsCorrect))
      { currentA.numCorrectSubmissions ++;
        if (currentA.firstCorrectAnswerClean == "")
          currentA.firstCorrectAnswerClean = currentA.currentAnswerClean;
        else
          out << "<tr><td>[first correct answer: " << currentA.firstCorrectAnswerClean << "]</td></tr>";
      }
    }
  }
#endif // MACRO_use_MySQL
#ifdef MACRO_use_MySQL
  if (theProblem.flagIsForReal)
  { std::stringstream comments;
    //if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
    //  stOutput << "<hr>DEBUG: adding prob data for file name: " << theProblem.fileName
    //  << "<br>"
    //  << currentProblemData.ToString() << "<br> into:<br> "
    //  << theUser.theProblemData.GetValueCreateIfNotPresent(theProblem.fileName).ToString()
    //  << "<hr>";
    theUser.SetProblemData(theProblem.fileName, currentProblemData);
    //if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
    //  stOutput << "<hr>result: " << theUser.theProblemData.GetValueCreateIfNotPresent(theProblem.fileName).ToString() << "<hr>";
    if (!theUser.StoreProblemDataToDatabase(theRoutines, comments))
      out << "<tr><td><b>This shouldn't happen and may be a bug: failed to store your answer in the database. "
      << CalculatorHTML::BugsGenericMessage << "</b><br>Comments: "
      << comments.str() << "</td></tr>";
    else
      out << "<tr><td>So far " << currentA.numCorrectSubmissions << " correct and "
      << currentA.numSubmissions - currentA.numCorrectSubmissions
      << " incorrect submissions.</td></tr>";
    if (hasDeadline)
    { if (secondsTillDeadline < 0)
        secondsTillDeadline *= - 1;
      if (deadLinePassed)
        out << "<tr><td><span style=\"color:red\"><b>Submission "
        << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
        (secondsTillDeadline, false, false)
        << " after deadline. </b></span></td></tr>";
      else
        out << "<tr><td><span style=\"color:green\"><b>Submission "
        << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
        (secondsTillDeadline, false, false)
        << " before deadline. </b></span></td></tr>";
    } else
      out << "<tr><td><span style=\"color:green\"><b>No deadline yet.</b></span></td></tr>";
  } //else
    //stOutput << "<tr><td><b>Submitting problem solutions allowed only for logged-in users. </b></td></tr>";
#endif
  out << "<tr><td>Your answer was: ";
  out << "\\(\\displaystyle ";
  out << currentA.currentAnswerClean;
  out << "\\)";
  std::string errorMessage;
  errorMessage = theInterpreter.ToStringIsCorrectAsciiCalculatorString(currentA.currentAnswerClean);
  if (errorMessage != "")
    out << "<br>" << errorMessage
    << "<hr><b>If you entered this expression through the keyboard (without copying + pasting) this is a bug: "
    << "please report it to the web site administrator. Don't forget to mention your keyboard/character setup. "
    << "Are you using the standard English keyboard? Cyrillic, Chinese, etc. characters are not accepted. </b> "
    << "<hr><span style=\"color:red\"><b>Copying and pasting an answer not computed by yourself "
    << " is considered cheating (example: answer from an online program for doing homework).</b> </span>";
  out << "</td></tr>";
  out << "</table>";
  out << "Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).<hr>";

//  stOutput << "<hr>" << theInterpreter.outputString << "<hr><hr><hr><hr><hr><hr>";
//  stOutput << this->ToStringCalculatorArgumentsHumanReadable();
  //Calculator answerInterpretter;
  //answerInterpretter.theProgramExpression=theGlobalVariables.GetWebInput("mainInput");
  //answerInterpretter.init();
  return out.str();
}

std::string HtmlInterpretation::AddTeachersSections()
{ MacroRegisterFunctionWithName("HtmlInterpretation::AddTeachersSections");
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagUsingSSLinCurrentConnection)
  { out << "<b>Only admins may assign sections to teachers.</b>";
    return out.str();
  }
  std::string mainInput = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  if (!HtmlRoutines::ChopCGIString(mainInput, theMap, out))
  { out << "<b>Failed to extract input from: " << mainInput << ".</b>";
    return out.str();
  }
  std::string desiredUsers =
  HtmlRoutines::ConvertURLStringToNormal(theMap.GetValueCreate("teachers"), false);
  std::string desiredSections =
  HtmlRoutines::ConvertURLStringToNormal(theMap.GetValueCreate("sections"), false);
#ifdef MACRO_use_MySQL
  List<std::string> theTeachers;
  List<char> delimiters;
  delimiters.AddOnTop(' ');
  delimiters.AddOnTop('\r');
  delimiters.AddOnTop('\n');
  delimiters.AddOnTop('\t');
  delimiters.AddOnTop(',');
  delimiters.AddOnTop(';');
  delimiters.AddOnTop(160);//<-&nbsp
  MathRoutines::StringSplitExcludeDelimiters(desiredUsers, delimiters, theTeachers);
//  MathRoutines::StringSplitExcludeDelimiters(desiredSections, delimiters, theSections);
  if (theTeachers.size == 0)
  { out << "<b>Could not extract teachers from " << desiredUsers << ".</b>";
    return out.str();
  }
//  if (theSections.size==0)
//  { out << "<b>Could not extract sections from " << desiredSections << ".</b>";
//    return out.str();
//  }
  DatabaseRoutines theRoutines;
  UserCalculator currentTeacher;
  for (int i = 0; i < theTeachers.size; i ++)
  { currentTeacher.reset();
    currentTeacher.username = theTeachers[i];
    currentTeacher.currentTable = DatabaseStrings::tableUsers;
    if (!currentTeacher.FetchOneUserRow(theRoutines, &out, &out))
    { out << "<span style=\"color:red\">Failed to fetch teacher: " << theTeachers[i] << "</span><br>";
      continue;
    }
    currentTeacher.courseInfo.courseInfoJSON.GetElement()[DatabaseStrings::columnSectionsTaught]=desiredSections;
    if(!currentTeacher.SetColumnEntry
       (DatabaseStrings::columnCourseInfo, currentTeacher.courseInfo.ToStringForDBStorage(), theRoutines, &out))
      out << "<span style=\"color:red\">Failed to store course info of instructor: " << theTeachers[i] << ". </span><br>";
    else
      out << "<span style=\"color:green\">Assigned " << theTeachers[i] << " to section: "
      << desiredSections << "</span><br>";
  }
  return out.str();
#else
  return "<b>no database present.</b>";
#endif // MACRO_use_MySQL
}

std::string HtmlInterpretation::AddUserEmails(const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("HtmlInterpretation::AddUserEmails");
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagUsingSSLinCurrentConnection)
  { out << "<b>Only admins may add users, under ssl connection. </b>";
    return out.str();
  }
  //stOutput << "<br>DEBUG: userlist: " << theGlobalVariables.GetWebInput("userList") << "<br>";
  //stOutput << "DEBUG: userlist: " << theGlobalVariables.GetWebInput("passwordList") << "<br>";
  std::string inputEmails = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("userList"), false);
  std::string userPasswords = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("passwordList"), false);
  std::string userGroup =
  MathRoutines::StringTrimWhiteSpace(HtmlRoutines::ConvertURLStringToNormal
  (theGlobalVariables.GetWebInput(DatabaseStrings::columnSection), false));
  std::string userRole = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("userRole"), false);

  if (inputEmails == "")
  { out << "<b>No emails to add</b>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  bool sentEmails = true;
  //stOutput << "DEBUG: here be i!";
  bool doSendEmails = theGlobalVariables.userCalculatorRequestType == "sendEmails" ?  true : false;
  int numNewUsers = 0;
  int numUpdatedUsers = 0;
  bool createdUsers = theRoutines.AddUsersFromEmails
  (inputEmails, userPasswords, userRole, userGroup, comments,
   numNewUsers, numUpdatedUsers);
  if (createdUsers)
  { out << "<span style=\"color:green\">Success: "
    << numNewUsers << " new users and " << numUpdatedUsers
    << " user updates. </span>";
  } else
    out << "<span style=\"color:red\">Failed to add all users. </span> Errors follow. <hr>"
    << comments.str() << "<hr>";
  if (doSendEmails)
  { if (sentEmails)
      out << "<span style=\"color:green\">Activation emails successfully sent. </span>";
    else
      out << "<span style=\"color:red\">Failed to send all activation emails. </span>";
  }
  bool usersAreAdmins = (userRole == "admin");
  List<List<std::string> > userTable;
  List<std::string> userLabels;
  if (!theRoutines.FetchAllUsers(userTable, userLabels, comments))
    out << comments.str();
  out << HtmlInterpretation::ToStringUserDetailsTable
  (usersAreAdmins, userTable, userLabels, hostWebAddressWithPort);
//    out << "<hr>Debug: got to here. ";
  if (!createdUsers || !sentEmails)
    out << "<br>Comments:<br>" << comments.str();
  return out.str();
#else
  return "<b>no database present.</b>";
#endif // MACRO_use_MySQL
}

const std::string CalculatorHTML::BugsGenericMessage =
"Please take a screenshot, copy the link address and send those along \
with a short explanation to the administrator of the web site. ";

std::string HtmlInterpretation::GetAnswerOnGiveUp()
{ return HtmlInterpretation::GetAnswerOnGiveUp(theGlobalVariables.GetWebInput("randomSeed"));
}

std::string HtmlInterpretation::GetAnswerOnGiveUp
(const std::string& inputRandomSeed, std::string* outputNakedAnswer, bool* outputDidSucceed)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetAnswerOnGiveUp");
  if (outputNakedAnswer != 0)
    *outputNakedAnswer = "";
  if (outputDidSucceed != 0)
    *outputDidSucceed = false;
  double startTime = theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem(false, inputRandomSeed);
  std::stringstream out;
  if (!theProblem.flagLoadedSuccessfully)
  { out << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    if(theProblem.comments.str() != "")
      out << " Comments: " << theProblem.comments.str();
    return out.str();
  }
  if (theProblem.flagIsForReal)
  { out << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    return out.str();
  }
  if (inputRandomSeed == "")
  { out << " <b>I could not figure out the exercise problem (missing random seed). </b>";
    return out.str();
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
  { out << "<br><b>Problem preparation failed.</b><br>" << comments.str();
    return out.str();
  }
  std::string lastStudentAnswerID;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs = theGlobalVariables.webArguments;
  for (int i = 0; i < theArgs.size(); i ++)
    MathRoutines::StringBeginsWith
    (theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1)
  { out << "File: "
    << theProblem.fileName
    << "<br><b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b><br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    { out << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
      //out << "<hr>Client input: " << this->mainArgumentRAW << "<hr>";
      out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
    }
    return out.str();
  }
  Answer& currentA = theProblem.theProblemData.theAnswers[indexLastAnswerId];
  if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly == "")
  { out << "<b> Unfortunately there is no answer given for this "
    << "question (answerID: " << lastStudentAnswerID << ").</b>";
    if (theGlobalVariables.UserDebugFlagOn() &&
        theGlobalVariables.UserDefaultHasProblemComposingRights())
      out << "<br>Answer status: " << currentA.ToString();
    return out.str();
  }
  Calculator theInterpreteR;
  theInterpreteR.init();
  theInterpreteR.flagPlotNoControls = true;
  theInterpreteR.flagWriteLatexPlots = false;
  std::stringstream answerCommands, answerCommandsNoEnclosure;
  answerCommands << currentA.commandsBeforeAnswer;
  answerCommandsNoEnclosure << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;
  answerCommands << "CommandEnclosure{}(" << currentA.commandsNoEnclosureAnswerOnGiveUpOnly << ");";
  answerCommandsNoEnclosure << currentA.commandsNoEnclosureAnswerOnGiveUpOnly;
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors != "")
  { out << "<span style=\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>";
    if (theGlobalVariables.UserDefaultHasProblemComposingRights())
      out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&"
      << "mainInput="
      << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosure.str(), false)
      << "\">Calculator input no enclosures</a>";
    out << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, false);
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  if (theInterpreteR.flagAbortComputationASAP)
  { out << "<span style=\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>";
    if (theGlobalVariables.UserDefaultHasProblemComposingRights())
      out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&"
      << "mainInput="
      << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosure.str(), false)
      << "\">Calculator input no enclosures</a>";
    out << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime << " second(s).";
    return out.str();
  }
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theFormat.flagUseQuotes = false;
  theFormat.flagUseLatex = true;
  theFormat.flagUsePmatrix = true;
  bool isFirst = true;
  const Expression& currentE =
  theInterpreteR.theProgramExpression[theInterpreteR.theProgramExpression.size() - 1][1];
  if (!currentE.StartsWith(theInterpreteR.opEndStatement()))
  { out << "\\(\\displaystyle " << currentE.ToString(&theFormat) << "\\)";
    if (outputNakedAnswer != 0)
      *outputNakedAnswer = currentE.ToString(&theFormat);
    if (outputDidSucceed != 0)
      *outputDidSucceed = true;
  } else
    for (int j = 1; j < currentE.size(); j ++)
    { if (currentE[j].StartsWith(theInterpreteR.opRulesOff()) ||
          currentE[j].StartsWith(theInterpreteR.opRulesOn()))
        continue;
      std::string stringAnswer;
      if (currentE[j].IsOfType<std::string>(&stringAnswer))
        if (MathRoutines::StringBeginsWith(stringAnswer, "Approximations have been"))
          continue;
      if (!isFirst)
        out << "<br>";
      theFormat.flagExpressionIsFinal=true;
      theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
      theFormat.flagUseQuotes=false;
      theFormat.flagUseLatex=true;
      if (currentE[j].IsOfType<std::string>())
        out << currentE[j].GetValue<std::string>();
      else
        out << "\\(\\displaystyle " << currentE[j].ToString(&theFormat) << "\\)";
//      if (j==currentE.size()-1)
      if (isFirst)
      { if (outputNakedAnswer != 0)
          *outputNakedAnswer = currentE[j].ToString(&theFormat);
        if (outputDidSucceed != 0)
          *outputDidSucceed = true;
      }
      isFirst = false;
    }
  out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()- startTime << " second(s).";
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    out
    << "<hr><a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&"
    << "mainInput="
    << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosure.str() ,false)
    << "\">Calculator input no enclosures</a>"
    << theInterpreteR.outputString << "<hr>"
    << theInterpreteR.outputCommentsString
    << "<hr>Raw input: <br>" << theInterpreteR.inputString;
  return out.str();
}

std::string HtmlInterpretation::GetAccountsPageBody(const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetAccountsPageBody");
  (void) hostWebAddressWithPort;
#ifdef MACRO_use_MySQL
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagLoggedIn || !theGlobalVariables.flagUsingSSLinCurrentConnection)
  { out << "Browsing accounts allowed only for logged-in admins over ssl connection.";
    return out.str();
  }
  DatabaseRoutines theRoutines;
  std::string notUsed;
  List<List<std::string> > userTable;
  List<std::string> columnLabels;
  std::stringstream commentsOnFailure;
  out << "Database: " << theRoutines.theDatabaseName
  << "<br>Database user: " << theRoutines.theDatabaseUser << "<br>";
  if (!theRoutines.PrepareClassData(notUsed, userTable, columnLabels, commentsOnFailure))
  { out << "<b>Failed to load user info.</b> Comments: " << commentsOnFailure.str();
    return out.str();
  }
//  out << "DEBUG: Usertable: " << userTable.ToStringCommaDelimited();
  int indexCourseInfo = - 1;
  for (int i = 0; i < columnLabels.size; i ++)
    if (columnLabels[i] == DatabaseStrings::columnCourseInfo)
      indexCourseInfo = i;
  if (indexCourseInfo == - 1)
  { out << "Failed to load extra user info. ";
    return out.str();
  }
  HashedList<std::string, MathRoutines::hashString> theSections;
  UserCalculator currentUser;
  for (int i = 0; i < userTable.size; i ++)
  { currentUser.reset();
    currentUser.courseInfo.rawStringStoredInDB = userTable[i][indexCourseInfo];
    currentUser.AssignCourseInfoString(&out);
    theSections.AddOnTopNoRepetition(currentUser.courseInfo.getSectionInDB());
  }
  theSections.QuickSortAscending();
  out << "<hr>";
  out << HtmlInterpretation::ToStringAssignSection();
  out << "<hr>";
  out << HtmlInterpretation::ToStringUserDetails(true, userTable, columnLabels, hostWebAddressWithPort);
  out << "<hr>";
  out << HtmlInterpretation::ToStringUserDetails(false, userTable, columnLabels, hostWebAddressWithPort);
  return out.str();
#else
  return "<b>Database not available. </b>";
#endif // MACRO_use_MySQL
}

std::string HtmlInterpretation::GetNavigationPanelWithGenerationTime()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetNavigationPanelWithGenerationTime");
  std::stringstream out;
  out.precision(3);
  out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
  << "Generated in " << theGlobalVariables.GetElapsedSeconds() << " second(s)"
  << "</calculatorNavigation>\n";
  return out.str();
}

std::string HtmlInterpretation::GetScoresPage()
{ MacroRegisterFunctionWithName("WebWorker::GetScoresPage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator()
  << HtmlRoutines::GetJavascriptStandardCookiesWithTags()
  << "<link rel=\"stylesheet\" href=\"/calculator-html/styleScorePage.css\">"
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  CalculatorHTML thePage;
  thePage.LoadDatabaseInfo(out);
  std::string theScoresHtml = HtmlInterpretation::ToStringUserScores();
  std::string theDebugHtml = HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << HtmlInterpretation::GetNavigationPanelWithGenerationTime();
  out << "<problemNavigation>" << thePage.ToStringProblemNavigation() << "</problemNavigation>";
  out << theScoresHtml;
  out << theDebugHtml;
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetAccountsPage(const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("WebWorker::GetAccountsPage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator()
  << HtmlRoutines::GetJavascriptStandardCookiesWithTags()
  << HtmlRoutines::GetJavascriptSubmitEmails()
  << HtmlRoutines::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  bool isOK = theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights();
  std::string accountsPageBody;
  if (isOK)
    accountsPageBody = HtmlInterpretation::GetAccountsPageBody(hostWebAddressWithPort);
  CalculatorHTML thePage;
  thePage.LoadDatabaseInfo(out);
  out << HtmlInterpretation::GetNavigationPanelWithGenerationTime();
  out << "<problemNavigation>" << thePage.ToStringProblemNavigation() << "</problemNavigation>";
  if (!isOK)
  { out << "<b>Viewing accounts is allowed for logged-in admins only</b>"
    << "</body></html>";
    return out.str();
  }
  out << accountsPageBody;
//  out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation() << "</calculatorNavigation>\n";
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserDetailsTable
(bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& columnLabels,
 const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserDetailsTable");
#ifdef MACRO_use_MySQL
  std::stringstream out;
  //std::string userRole = adminsOnly ? "admin" : "student";
  int numUsers = 0;
  bool flagFilterCourse = (!adminsOnly) && (theGlobalVariables.GetWebInput("filterAccounts") == "true");
  std::string currentCourse =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  if (flagFilterCourse)
  { out << "<br>Displaying only students in course: <span style=\"color:blue\"><b>"
    << currentCourse << "</b></span>. "
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=accounts&"
    << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
    << "filterAccounts=false&Z"
    << "\">Show all. </a>"
    << "<br>";
  }
  UserCalculator currentUser;
  currentUser.currentTable = DatabaseStrings::tableUsers;
  int indexUser = - 1;
  int indexEmail = - 1;
  int indexActivationToken = - 1;
  int indexUserRole = - 1;
  int indexCourseInfo = - 1;
  //int indexProblemData=-1;
  for (int i = 0; i < columnLabels.size; i ++)
  { if (columnLabels[i] == DatabaseStrings::columnUsername)
      indexUser = i;
    if (columnLabels[i] == "email")
      indexEmail = i;
    if (columnLabels[i] == "activationToken")
      indexActivationToken = i;
    if (columnLabels[i] == "userRole")
      indexUserRole = i;
    if (columnLabels[i] == DatabaseStrings::columnCourseInfo)
      indexCourseInfo = i;
  }
  if (
      indexUser            == - 1 ||
      indexEmail           == - 1 ||
      indexActivationToken == - 1 ||
      indexUserRole        == - 1 ||
      indexCourseInfo      == - 1)
  { out << "<span style=\"color:red\"><b>This shouldn't happen: failed to find necessary "
    << "column entries in the database. "
    << "This is likely a software bug. Function: HtmlInterpretation::ToStringUserDetailsTable. </b></span>"
    << "indexUser, indexExtraInfo, indexEmail, indexActivationToken, indexUserRole, indexCurrentCourse:  "
    << indexUser << ", "
    << indexEmail          << ", "
    << indexActivationToken << ", "
    << indexUserRole
    ;
    return out.str();
  }
  HashedList<std::string, MathRoutines::hashString> sectionIDs;
  List<std::string> theSections;

  List<List<std::string> > activatedAccountBucketsBySection;
  List<List<std::string> > preFilledLinkBucketsBySection;
  List<List<std::string> > nonActivatedAccountBucketsBySection;
  for (int i = 0; i < userTable.size; i ++)
  { currentUser.reset();
    currentUser.courseInfo.rawStringStoredInDB = userTable[i][indexCourseInfo];
    currentUser.AssignCourseInfoString(&out);
    if (flagFilterCourse &&
        (currentUser.courseInfo.getCurrentCourseInDB() != currentCourse ||
         currentUser.courseInfo.getInstructorInDB() != theGlobalVariables.userDefault.username.value))
      continue;
    std::string currentID = currentUser.ToStringIdSectionCourse();
    if (!sectionIDs.Contains(currentID))
    { std::stringstream currentSectionInfo;
      currentSectionInfo << "<b>Section: </b>" << currentUser.courseInfo.getSectionInDB()
      << ", <b>Course: </b>" << currentUser.courseInfo.getCurrentCourseInDB()
      << ", <b>Instructor: </b>" << currentUser.courseInfo.getInstructorInDB();
      theSections.AddOnTop(currentSectionInfo.str());
      sectionIDs.AddOnTopNoRepetition(currentID);
    }
  }
  sectionIDs.QuickSortAscending(0, &theSections);
  //out << "DEBUG: Section ids: " << sectionIDs.ToStringCommaDelimited();
  activatedAccountBucketsBySection.SetSize(sectionIDs.size);
  nonActivatedAccountBucketsBySection.SetSize(sectionIDs.size);
  preFilledLinkBucketsBySection.SetSize(sectionIDs.size);
  int numActivatedUsers = 0;
  for (int i = 0; i < userTable.size; i ++)
  { currentUser.username = userTable[i][indexUser];
    currentUser.email = userTable[i][indexEmail];
    currentUser.actualActivationToken = userTable[i][indexActivationToken];
    currentUser.userRole = userTable[i][indexUserRole];
    currentUser.courseInfo.rawStringStoredInDB = userTable[i][indexCourseInfo];
    currentUser.AssignCourseInfoString(&out);
    std::string currentCourseID = currentUser.ToStringIdSectionCourse();

    //if (currentUser. currentCourses.value.find('%')!=std::string::npos)
    //{ out << "<span style=\"color:red\"><b>Non-expected behavior: user: "
    //  << currentUser.username.value
    //  << "current course: "
    //  << currentUser.currentCourses.value
    //  << " contains the % symbol. </b></span><br>";
    //}
    if (!adminsOnly && !sectionIDs.Contains(currentCourseID))
      continue;
    if (adminsOnly xor (currentUser.userRole == "admin"))
      continue;
    numUsers ++;
    std::stringstream oneTableLineStream;
    oneTableLineStream << "<tr>"
    << "<td>" << currentUser.username.value << "</td>"
    << "<td>" << currentUser.email.value << "</td>"
    ;
    bool isActivated = true;
    std::string webAddress = "https://" + hostWebAddressWithPort + "/cgi-bin/calculator";
    if (currentUser.actualActivationToken.value != "activated" && currentUser.actualActivationToken.value != "error")
    { isActivated = false;
      numActivatedUsers ++;
      oneTableLineStream << "<td><span style=\"color:red\">not activated</span></td>";
      if (currentUser.actualActivationToken.value!="")
        oneTableLineStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken.value, hostWebAddressWithPort,
         userTable[i][indexUser], currentUser.email.value)
        << "\"> (Re)activate account and change password</a>"
        << "</td>";
      oneTableLineStream << "<td>";
      oneTableLineStream
      << "<a href=\"mailto:" << currentUser.email.value
      << "?subject=Math 140 Homework account activation&";

      oneTableLineStream << "body=";
      std::stringstream emailBody;
      emailBody << "Dear user,\n you have not activated your homework server account yet. \n"
      << "To activate your account and set your password please use the link: "
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << HtmlRoutines::ConvertStringToURLString( UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken.value, hostWebAddressWithPort,
         userTable[i][indexUser], currentUser.email.value), false)
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << " Once you activate your account, you can log in safely here: \n"
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << webAddress
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << "Best regards, \ncalculator-algebra.org.";
      oneTableLineStream << emailBody.str() << "\">Send email manually.</a> ";
      oneTableLineStream << "</td>";
      //      else
        //  oneTableLineStream << "<td>Activation token: " << currentUser.activationToken.value << "</td>";
    } else if (currentUser.actualActivationToken=="error")
      oneTableLineStream << "<td>error</td><td></td>";
    else
      oneTableLineStream << "<td><span style=\"color:green\">activated</span></td><td></td><td></td>";
    std::stringstream oneLink;
    oneLink << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=login&username="
    << currentUser.username.value << "\">" << currentUser.username.value << "</a>";
    oneTableLineStream << "<td>" << oneLink.str() << "</td>";
    oneTableLineStream << "<td>" << userTable[i][indexCourseInfo] << "</td>";
    //oneTableLineStream << "<td>" << currentUser.courseInfo.ToStringHumanReadable() << "</td>";
    oneTableLineStream << "</tr>";

    int indexCurrentBucket = sectionIDs.GetIndex(currentCourseID);
    if (indexCurrentBucket != - 1)
    { if (isActivated)
        activatedAccountBucketsBySection[indexCurrentBucket].AddOnTop(oneTableLineStream.str());
      else
        nonActivatedAccountBucketsBySection[indexCurrentBucket].AddOnTop(oneTableLineStream.str());
      preFilledLinkBucketsBySection[indexCurrentBucket].AddOnTop(oneLink.str());
    }
  }
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++)
    nonActivatedAccountBucketsBySection[i].QuickSortAscending();
  for (int i = 0; i < activatedAccountBucketsBySection.size; i ++)
    activatedAccountBucketsBySection[i].QuickSortAscending();
  for (int i = 0; i < preFilledLinkBucketsBySection.size; i ++)
    preFilledLinkBucketsBySection[i].QuickSortAscending();
  std::stringstream tableStream;
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th>"
  << "<th>Activation manual email</th>"
  << " <th>Pre-filled login link</th><th>Course info</th></tr>";
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++)
  { if (!adminsOnly)
      if (nonActivatedAccountBucketsBySection[i].size > 0)
        tableStream << "<tr><td colspan=\"6\" style=\"text-align:center\">" << theSections[i] << "</td></tr>";
    for (int j = 0; j < nonActivatedAccountBucketsBySection[i].size; j ++)
      tableStream << nonActivatedAccountBucketsBySection[i][j];
  }
  for (int i = 0; i < activatedAccountBucketsBySection.size; i ++)
  { if (!adminsOnly)
      if (activatedAccountBucketsBySection[i].size > 0)
        tableStream << "<tr><td colspan=\"7\" style=\"text-align:center\">"
        << theSections[i] << "</td></tr>";
    for (int j = 0; j < activatedAccountBucketsBySection[i].size; j ++)
      tableStream << activatedAccountBucketsBySection[i][j];
  }
  tableStream << "</table>";
  std::stringstream preFilledLoginLinks;
  if (!adminsOnly)
  { for (int i = 0; i < preFilledLinkBucketsBySection.size; i ++)
    { if (preFilledLinkBucketsBySection[i].size > 0)
        preFilledLoginLinks << theSections[i] << "<br>";
      for (int j = 0; j < preFilledLinkBucketsBySection[i].size; j ++)
        preFilledLoginLinks << preFilledLinkBucketsBySection[i][j] << "<br>";
    }
  }
  out << "\n" << numUsers << " user(s)";
  if (numActivatedUsers > 0)
    out << ", <span style=\"color:red\">" << numActivatedUsers
    << " have not activated their accounts. </span>";
  out << tableStream.str() << preFilledLoginLinks.str();

  return out.str();
#else
  return "Compiled without database support. ";
#endif
}

std::string HtmlInterpretation::ToStringAssignSection()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringAssignSection");
  std::stringstream out;
  std::string idAddressTextarea = "inputSetTeacher";
  std::string idExtraTextarea = "inputSections";
  std::string idOutput = "idOutputSections";
  out << "Assign section(s) to teacher(s)<br> ";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idAddressTextarea << "\"";
  out << "placeholder=\"email or user list, comma, space or ; separated\">";
  out << "</textarea>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idExtraTextarea << "\"";
  out << " placeholder=\"list of sections\">";
  out << "</textarea>";
  out << "<br>";
  out
  << "<button class=\"normalButton\" onclick=\"submitStringAsMainInput("
  << "'teachers='+ "
  << "encodeURIComponent(document.getElementById('" << idAddressTextarea << "').value)"
  << " + '&sections=' + "
  << "encodeURIComponent(document.getElementById('" << idExtraTextarea << "').value),"
  << "'" << idOutput << "',"
  << " 'setTeacher', '" << idOutput << "'"
  << " )\"> Set teacher</button> ";
  out << "<br><span id=\"" << idOutput << "\">\n";
  out << "</span>";
  return out.str();
}

#ifdef MACRO_use_MySQL
void UserCalculator::ComputePointsEarned
(const HashedList<std::string, MathRoutines::hashString>& gradableProblems,
 MapLisT<std::string, TopicElement, MathRoutines::hashString>* theTopics)
{ MacroRegisterFunctionWithName("UserCalculator::ComputePointsEarned");
  this->pointsEarned = 0;
  this->pointsMax = 0;
  if (theTopics != 0)
    for (int i = 0; i < theTopics->size(); i ++)
    { (*theTopics).theValues[i].totalPointsEarned = 0;
      (*theTopics).theValues[i].pointsEarnedInProblemsThatAreImmediateChildren = 0;
      (*theTopics).theValues[i].maxPointsInAllChildren = 0;
      (*theTopics).theValues[i].flagSubproblemHasNoWeight = false;
      //(*theTopics).theValues[i].maxCorrectAnswersInAllChildren=0;
      //(*theTopics).theValues[i].numAnsweredInAllChildren=0;
    }
  for (int i = 0; i < this->theProblemData.size(); i ++)
  { const std::string problemName = this->theProblemData.theKeys[i];
    if (!gradableProblems.Contains(problemName) )
      continue;
    ProblemData& currentP = this->theProblemData.theValues[i];
    currentP.Points = 0;
    currentP.totalNumSubmissions = 0;
    currentP.numCorrectlyAnswered = 0;
    Rational currentWeight;
    currentP.flagProblemWeightIsOK =
    currentP.adminData.GetWeightFromCoursE(this->courseInfo.courseComputed, currentWeight);
    if (!currentP.flagProblemWeightIsOK)
    { currentWeight = 0;
      //stOutput << "Debug: weight not ok: " << problemName << "<br>";
    }// else
     // stOutput << "Debug: weight IS ok: " << problemName << "<br>";

//    this->problemData[i].numAnswersSought=this->problemData[i].answerIds.size;
    for (int j = 0; j<currentP.theAnswers.size(); j ++)
    { if (currentP.theAnswers[j].numCorrectSubmissions > 0)
        currentP.numCorrectlyAnswered ++;
      currentP.totalNumSubmissions += currentP.theAnswers[j].numSubmissions;
    }
    if (currentP.flagProblemWeightIsOK && currentP.theAnswers.size() > 0)
    { currentP.Points=(currentWeight*currentP.numCorrectlyAnswered)/currentP.theAnswers.size();
      this->pointsEarned += currentP.Points;
      //stOutput << "<br>DEBUG: Accounting points: " << currentP.Points
      //<< " to get: " << this->pointsEarned ;
    }
    if (theTopics != 0)
      if (theTopics->Contains(problemName))
      { TopicElement& currentElt = theTopics->GetValueCreate(problemName);
        this->pointsMax += currentWeight;
        for (int j = 0; j < currentElt.parentTopics.size; j ++)
        { (*theTopics).theValues[currentElt.parentTopics[j]].totalPointsEarned += currentP.Points;
          (*theTopics).theValues[currentElt.parentTopics[j]].maxPointsInAllChildren += currentWeight;
          if (currentWeight == 0)
            (*theTopics).theValues[currentElt.parentTopics[j]].flagSubproblemHasNoWeight = true;
        }
        if (currentElt.parentTopics.size > 1)
          (*theTopics).theValues[currentElt.parentTopics[currentElt.parentTopics.size - 2]]
          .pointsEarnedInProblemsThatAreImmediateChildren += currentP.Points;
      }
  }
}
#endif

struct UserScores
{
public:
  CalculatorHTML theProblem;
  std::string currentSection;
  std::string currentCourse;
  List<MapLisT<std::string, Rational, MathRoutines::hashString> > scoresBreakdown;
  List<List<std::string> > userTablE;
  List<Rational> userScores;
  List<std::string> userInfos;
  List<std::string> userNames;
  List<LargeInt> numStudentsSolvedEntireTopic;
  List<LargeInt> numStudentsSolvedPartOfTopic;
  List<LargeInt> numStudentsSolvedNothingInTopic;
  bool ComputeScoresAndStats(std::stringstream& comments);
};

bool UserScores::ComputeScoresAndStats(std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserScores::ComputeScoresAndStats");
#ifdef MACRO_use_MySQL
  //stOutput << "DEBUG: Computing scores and stats. ";
  theProblem.currentUseR.::UserCalculatorData::operator=(theGlobalVariables.userDefault);

  this->theProblem.LoadFileNames();
  if (!this->theProblem.LoadAndParseTopicList(comments))
    return false;
  if (!this->theProblem.PrepareSectionList(comments))
    return false;
  if (!this->theProblem.LoadDatabaseInfo(comments))
    comments << "<span style=\"color:red\">Could not load your problem history.</span> <br>";
  //stOutput << "DEBUG: got to here";
  theProblem.currentUseR.ComputePointsEarned
  (theProblem.currentUseR.theProblemData.theKeys, &theProblem.theTopicS);
  List<std::string> userLabels;
  DatabaseRoutines theRoutines;
  if (!theRoutines.FetchAllUsers(userTablE, userLabels, comments))
    return false;
  int usernameIndex = userLabels.GetIndex(DatabaseStrings::columnUsername);
  if (usernameIndex == - 1)
    return "Could not find username column. ";
  int problemDataIndex = userLabels.GetIndex("problemData");
  if (problemDataIndex == - 1)
    return "Could not find problem data column. ";
  int courseInfoIndex = userLabels.GetIndex(DatabaseStrings::columnCourseInfo);
  if (courseInfoIndex == - 1)
    return "Could not find course info column. ";
  CalculatorHTML currentUserRecord;
  this->userScores.Reserve(userTablE.size);
  this->userNames.Reserve(userTablE.size);
  this->userInfos.Reserve(userTablE.size);
  this->scoresBreakdown.Reserve(userTablE.size);
  this->userScores.SetSize(0);
  this->userNames.SetSize(0);
  this->userInfos.SetSize(0);
  this->scoresBreakdown.SetSize(0);
  this->numStudentsSolvedEntireTopic.initFillInObject
  (this->theProblem.theTopicS.size(), 0);
  this->numStudentsSolvedPartOfTopic.initFillInObject
  (this->theProblem.theTopicS.size(), 0);
  this->numStudentsSolvedNothingInTopic.initFillInObject
  (this->theProblem.theTopicS.size(), 0);
  bool ignoreSectionsIdontTeach = true;
  this->currentSection = theGlobalVariables.userDefault.courseInfo.sectionComputed;
  this->currentCourse = theGlobalVariables.GetWebInput("courseHome");
  if (theGlobalVariables.GetWebInput("request") == "scoresInCoursePage")
    this->currentSection =
    MathRoutines::StringTrimWhiteSpace(
    HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false));
  //stOutput << "<br>DEBUG: ignoreSectionIdontTEach: " << ignoreSectionsIdontTeach;
  //stOutput << "<br>DEBUG: currentSection: " << this->currentSection;
  for (int i = 0; i < this->userTablE.size; i ++)
  { currentUserRecord.currentUseR.courseInfo.rawStringStoredInDB = this->userTablE[i][courseInfoIndex];
    currentUserRecord.currentUseR.AssignCourseInfoString(&comments);
    //if (i<0)
    //stOutput << "<br>DEBUG: currentsection: " << currentUserRecord.currentUseR.courseInfo.getSectionInDB();
    if (ignoreSectionsIdontTeach)
    { if (currentUserRecord.currentUseR.courseInfo.courseComputed != this->currentCourse)
        continue;
      if (theGlobalVariables.UserStudentVieWOn())
      { if (currentUserRecord.currentUseR.courseInfo.getSectionInDB() != this->currentSection)
          continue;
      } else
      { if (currentUserRecord.currentUseR.courseInfo.getSectionInDB() != this->currentSection)
          continue;
      }
    }
    this->userScores.AddOnTop(- 1);
    this->userNames.AddOnTop(this->userTablE[i][usernameIndex]);
    this->userInfos.AddOnTop(currentUserRecord.currentUseR.courseInfo.getSectionInDB());
    this->scoresBreakdown.SetSize(this->scoresBreakdown.size + 1);
    currentUserRecord.currentUseR.username = this->userTablE[i][usernameIndex];

//    out << "<hr>Debug: reading db problem data from: "
//    << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(userTable[i][problemDataIndex]) << "<br>";
    if (!currentUserRecord.currentUseR.InterpretDatabaseProblemData
        (this->userTablE[i][problemDataIndex], comments))
      continue;
//    out << "<br>DEBUG: after db data read: " << currentUserRecord.currentUseR.ToString();
    currentUserRecord.ReadProblemInfoAppend
    (theProblem.currentUseR.courseInfo.problemWeightString,
    currentUserRecord.currentUseR.theProblemData, comments);
//    out << "<br>DEBUG: after ReadProblemInfoAppend: " << currentUserRecord.currentUseR.ToString();
    currentUserRecord.currentUseR.ComputePointsEarned
    (theProblem.problemNamesNoTopics, &theProblem.theTopicS);
    this->scoresBreakdown.LastObject()->Clear();
    for (int j = 0; j < theProblem.theTopicS.size(); j ++)
    { TopicElement& currentTopic = theProblem.theTopicS[j];
      Rational currentPts = currentTopic.totalPointsEarned;
      Rational maxPts = currentTopic.maxPointsInAllChildren;
      this->scoresBreakdown.LastObject()->SetKeyValue
      (theProblem.theTopicS.theKeys[j],currentPts);
      if ( maxPts == currentPts)
        this->numStudentsSolvedEntireTopic[j] ++;
      else if (currentPts > 0)
        this->numStudentsSolvedPartOfTopic[j] ++;
      else
        this->numStudentsSolvedNothingInTopic[j] ++;
    }
    *this->userScores.LastObject() = currentUserRecord.currentUseR.pointsEarned;
    //out << "<br>DEBUG: Computed scores from: " << currentUserRecord.currentUseR.ToString();
  }
  return true;
#else
  return false;
#endif
}

std::string HtmlInterpretation::GetScoresInCoursePage()
{ MacroRegisterFunctionWithName("WebWorker::GetScoresInCoursePage");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "Only admins are allowed to view student scores.";
  std::stringstream out;
  out.precision(4);
  UserScores theScores;
  if (!theScores.ComputeScoresAndStats(out))
    return out.str();
//  for (int i=0; i<theScores.theProblem.theTopicS.size(); i++)
//  { TopicElement& currentElt=theScores.theProblem.theTopicS[i];
//    currentElt.ComputeID();
//    out << currentElt.id << ": "
//    << currentElt.studentScoresSpanId
//    << ": solved all: "
//    << theScores.numStudentsSolvedEntireTopic[i]
//    << ", solved part: "
//    << theScores.numStudentsSolvedPartOfTopic[i]
//    << ", solved none: "
//    << theScores.numStudentsSolvedNothingInTopic[i]
//    << "<br>";
//  }
  out << "Section: " << theScores.currentSection << ". ";
  //out << "DEBUG: solvedl topic 0: "
  //<< theScores.numStudentsSolvedEntireTopic[0]
  //<< ", "
  //<< theScores.numStudentsSolvedPartOfTopic[0]
  //<< ", "
  //<< theScores.numStudentsSolvedNothingInTopic[0]
  //<< ". ";

  out << "<script type=\"text/javascript\">\n";
  out << "studentScoresInHomePage= new Array("
  << theScores.theProblem.theTopicS.size() << ");\n";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++)
  { TopicElement& currentElt = theScores.theProblem.theTopicS[i];
    out << "studentScoresInHomePage[" << i << "]= new Object;\n";
    if (currentElt.flagSubproblemHasNoWeight)
      out << "studentScoresInHomePage[" << i << "].weightsOK=false;\n";
    else
      out << "studentScoresInHomePage[" << i << "].weightsOK=true;\n";
    out << "studentScoresInHomePage[" << i << "].theId="
    << "'"
    << currentElt.studentScoresSpanId
    << "';\n";
    out << "studentScoresInHomePage[" << i << "].numSolvedAll="
    << theScores.numStudentsSolvedEntireTopic[i]
    << ";\n";
    out << "studentScoresInHomePage[" << i << "].numSolvedPart="
    << theScores.numStudentsSolvedPartOfTopic[i]
    << ";\n";
    out << "studentScoresInHomePage[" << i << "].numSolvedNone="
    << theScores.numStudentsSolvedNothingInTopic[i]
    << ";\n";
  }
  out << "</script>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserScores()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserScores");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "only admins are allowed to view scores";
#ifdef MACRO_use_open_ssl
  std::stringstream out;
  out.precision(4);
  UserScores theScores;
  if (!theScores.ComputeScoresAndStats(out))
    return out.str();
//  out << "DBUG: prob names: " << theProblem.problemNamesNoTopics.ToStringCommaDelimited();
  out << "<b>Section: </b>" << theScores.currentSection
  << "<br><b>Course: </b>"
  << theScores.currentCourse << "\n<br>\n";
  out << "<table class=\"scoreTable\"><tr><th rowspan=\"3\">User</th>"
  << "<th rowspan=\"3\">Section</th><th rowspan=\"3\"> Total score</th>";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++)
  { TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    if (currentElt.problem != "" || currentElt.type != currentElt.tChapter)
      continue;
    int numCols = currentElt.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan=\"" << numCols << "\"";
    if (currentElt.totalSubSectionsUnderME == 0 &&
        currentElt.flagContainsProblemsNotInSubsection)
      out << " rowspan=\"3\"";
    out << ">" << currentElt.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++)
  { TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    if (currentElt.problem != "" || currentElt.type != currentElt.tSection)
      continue;
    int numCols = currentElt.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan=\"" << numCols << "\"";
    if (currentElt.totalSubSectionsUnderME == 0 &&
        currentElt.flagContainsProblemsNotInSubsection)
      out << " rowspan=\"2\"";
    out << ">" << currentElt.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++)
  { TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    if (currentElt.problem == "" && currentElt.type != currentElt.tProblem &&
        currentElt.type != currentElt.tSubSection && currentElt.type != currentElt.tTexHeader)
    { if ((currentElt.flagContainsProblemsNotInSubsection &&
           currentElt.totalSubSectionsUnderMeIncludingEmptySubsections > 1)
          || currentElt.immediateChildren.size == 0)
        out << "<td></td>";
      continue;
    }
    if (currentElt.problem != "" || currentElt.type != currentElt.tSubSection)
      continue;
    out << "<td>" << currentElt.title << "</td>";
  }
  out << "</tr>\n";

  out << "<tr><td><b>Maximum score</b></td>"
  << "<td>-</td>"
  << "<td>" << theScores.theProblem.currentUseR.pointsMax.GetDoubleValue()
  << "</td>";
  for (int j = 0; j < theScores.theProblem.theTopicS.size(); j ++)
  { TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[j];
    if (currentElt.problem != "")
      continue;
    if (currentElt.type != currentElt.tSubSection &&
        !currentElt.flagContainsProblemsNotInSubsection)
      continue;
    out << "<td>" << currentElt.maxPointsInAllChildren << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < theScores.userInfos.size; i ++)
  { out << "<tr><td>" << theScores.userNames[i] << "</td>"
    << "<td>" << theScores.userInfos[i] << "</td>"
    << "<td>" << theScores.userScores[i].GetDoubleValue() << "</td>";
    for (int j = 0; j < theScores.theProblem.theTopicS.size(); j ++)
    { TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[j];
      if (currentElt.problem != "")
        continue;
      if (currentElt.type != currentElt.tSubSection && !currentElt.flagContainsProblemsNotInSubsection)
        continue;
      if (theScores.scoresBreakdown[i].Contains(theScores.theProblem.theTopicS.theKeys[j]))
        out << "<td>" << theScores.scoresBreakdown[i].theValues[j].GetDoubleValue() << "</td>";
      else
        out << "<td></td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
#else
  return "Error: database not running. ";
#endif
}

std::string HtmlInterpretation::ToStringUserDetails
(bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& columnLabels,
 const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserDetails");
  std::stringstream out;
#ifdef MACRO_use_MySQL
  std::string userRole = adminsOnly ? "admin" : "student";
  std::string idAddressTextarea = "inputAddUsers" + userRole;
  std::string idExtraTextarea = "inputAddExtraInfo" + userRole;
  std::string idOutput = "idOutput" + userRole;
  std::string idPasswordTextarea = "inputAddDefaultPasswords" + userRole;
  out << "<ul><li>Add <b>" << userRole << "(s)</b> here.</li> ";
  out << "<li>Added/updated users will have their current course set to: <br>"
  << "<span class=\"currentCourseIndicator\">"
  << HtmlRoutines::ConvertURLStringToNormal( theGlobalVariables.GetWebInput("courseHome"), false)
  << "</span></li>"
  << "<li>To change course use the select course link in the top panel.</li>"
  << "<li>List users with a comma/space bar/new line/tab/semicolumn separated list. </li>"
  << "<li>List default passwords with a similarly separated list.</li> "
  << "<li>If left blank, password(s) are not (re)set.</li> "
  << "<li>If the password entries are not left blank and the number of passwords "
  << "does not match the number of added users, the operation will fail (no users will be added).</li> "
  << "</ul>\n";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idAddressTextarea << "\"";
  out << "placeholder=\"user list, comma, space or ; separated\">";
  out << "</textarea>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idPasswordTextarea << "\"";
  out << " placeholder=\"default passwords\">";
  out << "</textarea>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idExtraTextarea << "\"";
  out << " placeholder=\"section/class #\">";
  out << "</textarea>";
  out << "<br>";
  out
  << "<button class=\"normalButton\" onclick=\"addEmailsOrUsers("
  << "'"    << idAddressTextarea
  << "', '"
  << "', '" << idOutput
  << "', '" << userRole
  << "', '" << idExtraTextarea
  << "', '" << idPasswordTextarea
  << "', 'addUsers'"
  << " )\"> Add users</button> ";
  out << "<br><span id=\"" << idOutput << "\">\n";
  out << HtmlInterpretation::ToStringUserDetailsTable(adminsOnly, userTable, columnLabels, hostWebAddressWithPort);
  out << "</span>";
#else
  out << "<b>Adding emails not available (database not present).</b> ";
#endif // MACRO_use_MySQL
  return out.str();
}

std::string GlobalVariables::ToStringNavigation()
{ return HtmlInterpretation::ToStringNavigation();
}

std::string HtmlInterpretation::ToStringNavigation()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringNavigation");
  std::stringstream out;
  //out << "<table>";
  //out << "DEBUG: auth token: " << theGlobalVariables.GetWebInput("authenticationToken") << "<br>";
  //out << "DEBUG: to string nav";
  std::string linkSeparator = " | ";
  std::string linkBigSeparator = " || ";
  if (theGlobalVariables.userCalculatorRequestType == "template" ||
      theGlobalVariables.userCalculatorRequestType == "templateNoLogin")
    out << "<b>Home</b>" << linkSeparator;
  else
  { std::string topicList = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
    std::string courseHome = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
    if (topicList != "" && courseHome != "")
    { std::string studentView = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("studentView"), false);
      std::string section = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("studentSection"), false);
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable;
      if (theGlobalVariables.userCalculatorRequestType == "exerciseNoLogin")
        out << "?request=templateNoLogin";
      else
        out << "?request=template";
      out << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
      << "studentView=" << studentView << "&";
      if (section != "")
        out << "studentSection="
        << theGlobalVariables.GetWebInput("studentSection") << "&";
      out << "topicList=" << topicList << "&";
      out << "courseHome=" << courseHome << "&";
      out << "fileName=" << courseHome << "&\">Home</a>"
      << linkSeparator;
    }
  }
  if (theGlobalVariables.userCalculatorRequestType == "selectCourse")
    out << "<b>Select course</b>" << linkBigSeparator;
  else
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=selectCourse\">Select course</a>" << linkBigSeparator;
  if (theGlobalVariables.flagLoggedIn)
  { out << "User";
    if (theGlobalVariables.UserDefaultHasAdminRights())
      out << " <b>(admin)</b>";
    out << ": " << theGlobalVariables.userDefault.username.value << linkSeparator;
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=logout&";
    out << theGlobalVariables.ToStringCalcArgsNoNavigation(0) << " \">Log out</a>" << linkSeparator;
    if (theGlobalVariables.userCalculatorRequestType == "changePasswordPage")
    { out << "<b>Account";
      out << "</b>" << linkSeparator;
    } else
    { if (theGlobalVariables.flagUsingSSLinCurrentConnection)
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage&"
        << theGlobalVariables.ToStringCalcArgsNoNavigation(0) << "\">Account</a>" << linkSeparator;
      else
        out << "<b>Account settings: requires secure connection</b>" << linkSeparator;
    }
    if (theGlobalVariables.userDefault.courseInfo.sectionComputed != "")
      out << "Section: " << theGlobalVariables.userDefault.courseInfo.sectionComputed
      << linkSeparator;
    //if (theGlobalVariables.UserDefaultHasAdminRights())
    //  out << "Course home: "
    //  << theGlobalVariables.userDefault.currentCourses.value
    //  << linkSeparator;
  } else
  { out << "<b style='color:red'>logged out</b>" << linkSeparator;
  }

  if (theGlobalVariables.UserDefaultHasAdminRights())
  { if (theGlobalVariables.userCalculatorRequestType != "accounts")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=accounts&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
      << "&filterAccounts=true"
      << "\">Accounts</a>" << linkSeparator;
    else
      out << "<b>Accounts</b>" << linkSeparator;
    if (theGlobalVariables.userCalculatorRequestType != "scores")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=scores&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
      << "\">Scores</a>" << linkSeparator;
    else
      out << "<b>Scores</b>" << linkSeparator;
    if (theGlobalVariables.userCalculatorRequestType != "status")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=status&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
      << "\">Server</a>" << linkSeparator;
    else
      out << "<b>Server</b>" << linkBigSeparator;
    if (theGlobalVariables.userCalculatorRequestType != "database")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=database&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
      << "\">Database</a>" << linkBigSeparator;
    else
      out << "<b>Database</b>" << linkBigSeparator;
  }
  if (theGlobalVariables.userCalculatorRequestType != "calculator")
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=calculator&"
    << theGlobalVariables.ToStringCalcArgsNoNavigation(0) << " \">Calculator</a>" << linkBigSeparator;
  else
    out << "<b>Calculator</b> " << linkBigSeparator;
  if (theGlobalVariables.userCalculatorRequestType != "about")
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=about&"
    << theGlobalVariables.ToStringCalcArgsNoNavigation(0) << " \">About</a>" << linkBigSeparator;
  else
    out << "<b>About</b> " << linkBigSeparator;
  out << "<a href=\"https://github.com/tmilev/calculator/issues\" target=\"_blank\">Feedback, bugs</a>"
  << linkBigSeparator;

  if (!theGlobalVariables.flagRunningApache)
  { if (theGlobalVariables.flagAllowProcessMonitoring)
    { if (!theGlobalVariables.UserDefaultHasAdminRights())
        out << "<span style=\"color:red\"><b>Monitoring on</b></span>" << linkSeparator;
      else
        out << "<a style=\"color:red\" href=\""
        << theGlobalVariables.DisplayNameExecutable
        << "?request=toggleMonitoring\""
        << "><b>Monitoring on</b></a>" << linkSeparator;
    } else
      if (theGlobalVariables.UserDefaultHasAdminRights())
        out << "<a style=\"color:green\" href=\""
        << theGlobalVariables.DisplayNameExecutable
        << "?request=toggleMonitoring\""
        << "><b>Monitoring off</b></a>" << linkSeparator;
  }

  return out.str();
}
