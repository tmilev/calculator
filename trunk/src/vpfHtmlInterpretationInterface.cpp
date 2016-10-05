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
  double startTime=theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  std::stringstream out;
  theProblem.LoadCurrentProblemItem();
  if (!theProblem.flagLoadedSuccessfully)
  { out << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    if(theProblem.comments.str()!="")
      out << " Comments: " << theProblem.comments.str();
    return out.str();
  }
  if (theProblem.flagIsForReal)
  { out << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    return out.str();
  }
  if(theGlobalVariables.GetWebInput("randomSeed")=="")
  { out << " <b>I could not figure out the exercise problem (missing random seed). </b>";
    return out.str();
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
  { stOutput << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
    return out.str();
  }
  std::string lastStudentAnswerID;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  for (int i=0; i<theArgs.size(); i++)
    MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  int indexLastAnswerId=theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId==-1)
  { out << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b><br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    { out << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
      //out << "<hr>Client input: " << this->mainArgumentRAW << "<hr>";
      out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
    }
    return out.str();
  }
  Answer& currentA=theProblem.theProblemData.theAnswers[indexLastAnswerId];
  Calculator theInterpreteR;
  theInterpreteR.init();
  if(!theProblem.PrepareCommands(comments))
  { out << "<b>Failed to prepare calculator commands. </b> <br>Comments:<br>" << comments.str();
    return out.str();
  }
  if (currentA.solutionElements.size==0)
  { out << "<b> Unfortunately there is no solution given for this question (answerID: " << lastStudentAnswerID << ").";
    return out.str();
  }
  std::stringstream answerCommands;
  answerCommands << "CommandEnclosure{}(" << currentA.commandsBeforeAnswer << "); "
  << currentA.commandsSolutionOnly;
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors!="")
  { out << "<span style=\"color:red\"><b>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false);
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  if (theInterpreteR.flagAbortComputationASAP)
  { out << "<span style=\"color:red\"><b>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  if (!theProblem.ProcessInterprettedCommands
       (theInterpreteR, currentA.solutionElements, out))
    return out.str();
  for (int i=1; i<currentA.solutionElements.size; i++)
    if (!currentA.solutionElements[i].IsHidden())
      out << currentA.solutionElements[i].ToStringInterpretedBody();
  out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    out <<  "<hr>" << theInterpreteR.outputString << "<hr>" << theInterpreteR.outputCommentsString
    << "<hr>Raw input: " << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  return out.str();
}

std::string HtmlInterpretation::GetSetProblemDatabaseInfoHtml()
{ MacroRegisterFunctionWithName("WebWorker::GetSetProblemDatabaseInfoHtml");
#ifdef MACRO_use_MySQL
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "<b>Only admins may set problem weights.</b>";
  CalculatorHTML theProblem;
  std::string inputProblemInfo=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  theProblem.topicListFileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
  std::stringstream commentsOnFailure;
  if (!theProblem.LoadAndParseTopicList(commentsOnFailure))
    return "Failed to load topic list from file name: " + theProblem.topicListFileName + ". "+ commentsOnFailure.str();
  //stOutput << "DEBUG: userDefault: " << theGlobalVariables.userDefault.ToStringUnsecure();
  theProblem.currentUseR.UserCalculatorData::operator=(theGlobalVariables.userDefault);
  std::stringstream out;
  if (!theProblem.ReadProblemInfoAppend
      (theProblem.currentUseR.deadlineInfoString.value, theProblem.currentUseR.theProblemData, out))
  { out << "Failed to interpret the deadline string. ";
    return out.str();
  }
  if (!theProblem.ReadProblemInfoAppend
      (theProblem.currentUseR.problemInfoString.value, theProblem.currentUseR.theProblemData, out))
  { out << "Failed to interpret the problem weight string. ";
    return out.str();
  }
  if (theProblem.MergeProblemInfoInDatabase(inputProblemInfo, commentsOnFailure))
  { out << "<span style=\"color:green\"><b>Modified. </b></span>";
    //out << "<meta http-equiv=\"refresh\" content=\"0;\">";
  } else
    out << "<span style=\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
  //out << "<br>Debug message:<br>inputProblemInfo raw: " << inputProblemInfo << "<br>Processed: "
  //<< CGI::URLKeyValuePairsToNormalRecursiveHtml(inputProblemInfo)
  //<< "<br>inputProblemHome: " << inputProblemHome;
  return out.str();
#else
  return "Cannot modify problem weights (no database available)";
#endif // MACRO_use_MySQL
}

std::string HtmlInterpretation::SubmitProblemPreview()
{ MacroRegisterFunctionWithName("WebWorker::SubmitProblemPreview");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  std::stringstream studentInterpretation;
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  std::stringstream out;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  for (int i=0; i<theArgs.size(); i++)
    if (MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID))
      lastAnswer= "("+ CGI::URLStringToNormal(theArgs[i]) + "); ";
  studentInterpretation << lastAnswer;
  out << "Your answer(s): \\(" << lastAnswer << "\\)" << "\n<br>\n";
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem();
  if (!theProblem.flagLoadedSuccessfully)
    out << "<br><b>Failed to load problem.</b> Comments: " << theProblem.comments.str();
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
  int indexLastAnswerId=theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId==-1)
  { out << "<br>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  Answer& currentA=theProblem.theProblemData.theAnswers[indexLastAnswerId];
  if (!theProblem.PrepareCommands(comments))
  { out << "Something went wrong while interpreting the problem file. ";
    if (theGlobalVariables.UserDebugFlagOn() &&
        theGlobalVariables.UserDefaultHasAdminRights())
      out << comments.str();
    out << "<br>Response time: "
    << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  std::stringstream studentInterpretationWithComments;
  if (MathRoutines::StringTrimWhiteSpace( currentA.commandsBeforeInterpretation)!="")
    studentInterpretationWithComments << "CommandEnclosure{}{" << currentA.commandsBeforeInterpretation << "}; ";
  studentInterpretationWithComments << studentInterpretation.str();

  Calculator theInterpreteR;
  theInterpreteR.flagUseLnInsteadOfLog=true;
  theInterpreteR.init();
  theInterpreteR.Evaluate(studentInterpretationWithComments.str());
  if (theInterpreteR.syntaxErrors!="")
  { out << "<span style=\"color:red\"><b>Failed to parse your answer, got:</b></span><br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false);
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  } else if (theInterpreteR.flagAbortComputationASAP)
  { out << "<span style=\"color:red\"><b>Failed to evaluate your answer, got:</b></span><br>"
    << theInterpreteR.outputString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  const Expression& studentAnswerNoContextE=theInterpreteR.theProgramExpression[theInterpreteR.theProgramExpression.size()-1];
  out << "<span style=\"color:magenta\"><b>Interpreting your answer as:</b></span><br>";
  FormatExpressions theFormat;
  theFormat.flagUseLatex=true;
  theFormat.flagUsePmatrix=true;
  out << "\\(" << studentAnswerNoContextE.ToString(&theFormat) << "\\)";
  if (currentA.commandIndicesVisibleCommentsBeforeSubmission.size==0)
  { out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  Calculator theInterpreterWithAdvice;
  theInterpreterWithAdvice.flagUseLnInsteadOfLog=true;
  theInterpreterWithAdvice.init();
  theInterpreterWithAdvice.flagWriteLatexPlots=false;
  std::stringstream calculatorInputStream;
  calculatorInputStream << "CommandEnclosure{}("
  << currentA.commandsBeforeAnswer << ");";
  calculatorInputStream << "CommandEnclosure{}("
  << currentA.answerId << " = " << lastAnswer << "); ";
  calculatorInputStream << currentA.commandsCommentsBeforeSubmissionOnly;
  theInterpreterWithAdvice.Evaluate(calculatorInputStream.str());
  if (theInterpreterWithAdvice.syntaxErrors!="")
  { out << "<br><span style=\"color:red\"><b>"
    << "Something went wrong when parsing your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
      out << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  if (theInterpreterWithAdvice.flagAbortComputationASAP )
  { out << "<br><span style=\"color:red\"><b>"
    << "Something went wrong when interpreting your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn() )
      out << "<br>Logged-in as admin with debug flag on=> printing error details. "
      << theInterpreterWithAdvice.outputString << "<br>" << theInterpreterWithAdvice.outputCommentsString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  theFormat.flagExpressionIsFinal=true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
  theFormat.flagUseQuotes=false;
  for (int j=0; j<currentA.commandIndicesVisibleCommentsBeforeSubmission.size; j++)
  { int actualIndex=3+currentA.commandIndicesVisibleCommentsBeforeSubmission[j];//<-adding 3 to actual index; the first
    //3 indices are reserved.
    if (actualIndex>=theInterpreterWithAdvice.theProgramExpression.size() )
      continue;
    const Expression& currentE=theInterpreterWithAdvice.theProgramExpression[actualIndex][1];
    theFormat.flagUseLatex=true;
    theFormat.flagUseQuotes=false;
    theFormat.flagExpressionIsFinal=true;
    out << currentE.ToString(&theFormat);
  }
  out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
  if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn() )
    out << "<hr>Logged-in as admin with debug flag on = including (lots of) details. "
    << "Executed command:<br>" << calculatorInputStream.str() << "<hr>"
    << theInterpreterWithAdvice.outputString << "<br>" << theInterpreterWithAdvice.outputCommentsString;
  return out.str();
}

std::string HtmlInterpretation::ClonePageResult()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ClonePageResult");
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights() ||
      !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "<b>Cloning problems allowed only for logged-in admins under ssl connection. </b>";
  std::string fileNameResulT = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  std::string fileNameToBeCloned = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"));
  std::stringstream out;
  std::string startingFileString;
  if (!FileOperations::LoadFileToStringVirtual(fileNameToBeCloned, startingFileString, out))
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
  out << "<b><span style=\"color:green\">Written content to file: "
  << fileNameResulT << ". </span></b>";
  CalculatorHTML linkInterpreter;
  out << linkInterpreter.ToStringLinkFromFileName(fileNameResulT);
  return out.str();
}

std::string HtmlInterpretation::GetExamPageInterpreter()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetExamPageInterpreter");
  CalculatorHTML theFile;
  std::stringstream out;
  out << theFile.LoadAndInterpretCurrentProblemItem();
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  return out.str();

}

std::string HtmlInterpretation::GetSelectCourse()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetSelectCourse");
  std::stringstream out;
  out << "<html><body>";
  List<std::string> theFileNames, theExtensions;
  if (!FileOperations::GetFolderFileNamesVirtual("topiclists/", theFileNames, &theExtensions))
  { out << "<b>Failed to fetch file names from topiclists/</b>. </body></html>";
    return out.str();
  }
  for (int i=0; i<theFileNames.size; i++)
    if (theExtensions[i]==".txt")
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=template&topicList=topiclists/" << theFileNames[i]
    << "&fileName=pagetemplates/ace-learning-Singapore-H2.html\">"
    << theFileNames[i] << "</a><br>";
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetTopicTable()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetTopicTable");
  std::stringstream out;
  out << "<html><body>";
  out << "Not implemented yet.";
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetHtmlTagWithManifest()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetHtmlTagWithManifest");
  std::stringstream out;
  out << "<!DOCTYPE HTML>\n";
  out << "<html";
  if (theGlobalVariables.flagAceIsAvailable && theGlobalVariables.flagCertificatesAreOfficiallySigned && false)
    out << " manifest=\"/cache.appcache\" type=\"text/cache-manifest\"";
  out << ">\n<!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

std::string HtmlInterpretation::GetPageFromTemplate()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetPageFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  bool includeDeadlineJavascript=
  theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn();
  thePage.fileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("courseHome"));
  if (!thePage.LoadMe(true, comments))
  { out << "<html><body><b>Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str() << "</body></html>";
    return out.str();
  }
  if (!thePage.InterpretHtml(comments))
  { out << "<html><body><b>Failed to interpret file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str() << "</body></html>";
    return out.str();
  }
  out << HtmlInterpretation::GetHtmlTagWithManifest();
  out << "<!-- File automatically generated from template: "
  << theGlobalVariables.GetWebInput("fileName")
  << ".-->\n";
  out << "<head><!-- tag added automatically; user-specified head tag ignored-->\n";
  out << thePage.outputHtmlHeadNoTag;
  out << HtmlSnippets::GetJavascriptStandardCookies();
  if (includeDeadlineJavascript)
    out << CGI::GetJavascriptInitilizeButtons();
  out << "</head><!-- tag added automatically; user-specified head tag ignored-->\n";
  out << "<body" //<< ">"
  << " onload=\"loadSettings();";
  if (includeDeadlineJavascript)
    out << " initializeButtonsCommon(); ";
  out <<"\"><!-- tag added automatically; user-specified body tag ignored-->\n"
  ;
  if (!theGlobalVariables.flagRunningApache)
    out << "<problemNavigation>" << thePage.outputHtmlNavigatioN
    << theGlobalVariables.ToStringNavigation() << "<small>Generated in " << theGlobalVariables.GetElapsedSeconds()
    << " second(s).</small>" << "</problemNavigation>\n";
  out << thePage.outputHtmlBodyNoTag;
  out << "</body><!-- tag added automatically; user-specified body tag ignored-->\n";
  out << "</html><!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

std::string HtmlInterpretation::GetExamPage()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetExamPage");
  CalculatorHTML theFile;
  std::string problemBody= theFile.LoadAndInterpretCurrentProblemItem();
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << HtmlSnippets::GetJavascriptStandardCookies() << "\n"
  << CGI::GetJavascriptMathjax() << "\n";
  if (theFile.flagMathQuillWithMatrices)
    out << CGI::GetJavascriptMathQuillMatrixSupport() << "\n";
  else
    out << CGI::GetJavascriptMathQuillDefault() << "\n";
  out << CGI::GetMathQuillStyleSheetWithTags() << "\n"
  << CGI::GetCalculatorStyleSheetWithTags() << "\n"  ;
  out << CGI::GetJavascriptInitilizeButtons() << "\n";
  if (theFile.flagLoadedSuccessfully)
    out << theFile.outputHtmlHeadNoTag;
  //<- ?????must come after theFile.outputHtmlHeadNoTag
  out << "</head>"
  << "<body onload=\"loadSettings(); initializeMathQuill(); ";
  if (!theFile.flagMathQuillWithMatrices)
    out << "initializeButtons();";
  else
    out << "initializeButtonsMatrixSupport();";
  out << "\">\n";
  out << problemBody;
  if (theFile.logCommandsProblemGeneration!="")
    out << "<hr>" << theFile.logCommandsProblemGeneration << "<hr>";
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetEditPageHTML()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetEditPageHTML");
  std::stringstream out;
  if ((!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights()) &&
      !theGlobalVariables.flagRunningApache)
    return "<b>Only logged-in admins are allowed to edit pages. </b>";
  CalculatorHTML theFile;
  theFile.LoadFileNames();
  out << "<html>"
  << "<head>"
  << HtmlSnippets::GetJavascriptStandardCookies()
  //  << CGI::GetLaTeXProcessingJavascript()
  //  << CGI::GetCalculatorStyleSheetWithTags()
  << HtmlSnippets::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << "<style type=\"text/css\" media=\"screen\">\n"
  << "    #editor { \n"
  << "      height: 400px;\n"
  << "      font-size: 100%;\n"
  << "   }\n"
  << "</style>\n"
  << "<script src=\"https://cdn.jsdelivr.net/ace/1.2.3/min/ace.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\n"
  //  << "<link rel=\"stylesheet\" href=\"//cdnjs.cloudflare.com/ajax/libs/highlight.js/9.2.0/styles/default.min.css\">"
  //  << "<script src=\"//cdnjs.cloudflare.com/ajax/libs/highlight.js/9.2.0/highlight.min.js\"></script>"
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  std::stringstream failureStream;
  if (!theFile.LoadMe(false, failureStream))
  { out << "<b>Failed to load file: " << theFile.fileName << ", perhaps the file does not exist. </b>";
    out << "</body></html>";
    return out.str();
  }
  if (!theFile.ParseHTML(failureStream))
    out << "<b>Failed to parse file: " << theFile.fileName << ".</b> Details:<br>" << failureStream.str();
  std::stringstream buttonStream, submitModPageJS;
  submitModPageJS
  //  << "submitStringAsMainInput(document.getElementById('mainInput').value, 'spanSubmitReport', 'modifyPage');"
  << "submitStringAsMainInput(editor.getValue(), 'spanSubmitReport', 'modifyPage');"
  ;
  buttonStream
  << "<button "
  << "onclick=\"" << submitModPageJS.str() << "\" >Save changes</button>";
  //  out << "<form>";
  //  out << "<input type=\"submit\" value=\"Save changes\"> ";
  out << buttonStream.str();
  out << "To include the problem in your topic list, add the following two lines. <br>"
  << "<textarea cols=\"70\", rows=\"3\">"
  << theFile.ToStringCalculatorProblemSourceFromFileName(theFile.fileName) << "</textarea>";
  out << "<br>\n";
  out << "Ctrl+S saves your changes. Edit bravely, all files are backed-up/stored in a svn history tree."
  << " You only risk losing your own changes.";
  out << "<br>\n";
  out
  << "Many thanks to the <a href=\"https://ace.c9.io\">ace editor</a> project. <br>"
  << "<div id=\"editor\" onkeydown=\"ctrlSPress(event);\" name=\"editor\">"
  //<< "<textarea cols=\"150\", rows=\"30\" id=\"mainInput\" name=\"mainInput\" onkeydown=\"ctrlSPress(event);\">"
  ;
  out
  //<< "</textarea>"
  << "</div>"
  << "\n<br>\n";
  out << "<script type=\"text/javascript\"> \n"
  << "function ctrlSPress(event){\n"
  << "   if (event.ctrlKey!=true)\n"
  << "     return;\n"
  << "   if (event.keyCode!=83)\n"
  << "     return;\n"
  << "   event.preventDefault();"
  << submitModPageJS.str() << "\n"
  << "}\n"
  << "</script>\n";
  out << "<script type=\"text/javascript\"> \n"
  //<< " document.getElementById('mainInput').value=decodeURIComponent(\""
  << " document.getElementById('editor').textContent=decodeURIComponent(\""
  << CGI::StringToURLString(theFile.inputHtml, false)
  << "\");\n"
  << "    var editor = ace.edit(\"editor\");\n"
  << "    editor.setTheme(\"ace/theme/chrome\");\n"
  << "    editor.getSession().setMode(\"ace/mode/xml\");\n"
  << "</script>\n";
  out << buttonStream.str();
  out << "<span id=\"spanSubmitReport\"></span><br>";
  out << theFile.ToStringLinkFromFileName(theFile.fileName);
  //  out << "<input type=\"submit\" value=\"Save changes\">";
  //  out << "</form>";
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::SubmitProblem()
{ MacroRegisterFunctionWithName("HtmlInterpretation::SubmitProblem");
  std::stringstream out;
  double startTime=theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem();
  if (!theProblem.flagLoadedSuccessfully)
  { out << "Failed to load problem. " << theProblem.comments.str();
    return out.str();
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
  { out << "<b>Failed to parse problem. </b>Comments: " << comments.str();
    return out.str();
  }
  Calculator theInterpreter;
  if (!theProblem.theProblemData.flagRandomSeedGiven && !theProblem.flagIsForReal)
    out << "<b>Random seed not given.</b>";
//  stOutput << "<b>DEBUG remove when done: Random seed: " << theProblem.theProblemData.randomSeed << "</b>";
  if (theProblem.fileName=="")
    crash << "This shouldn't happen: empty file name: theProblem.fileName." << crash;
  std::string problemStatement=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("problemStatement"));
  std::string studentAnswerNameReader;
  theProblem.studentTagsAnswered.init(theProblem.theProblemData.theAnswers.size);
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  int answerIdIndex=-1;
  for (int i=0; i<theArgs.size(); i++)
    if (MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &studentAnswerNameReader))
    { int newAnswerIndex=theProblem.GetAnswerIndex(studentAnswerNameReader);
      if (answerIdIndex==-1)
        answerIdIndex=newAnswerIndex;
      else if (answerIdIndex!=newAnswerIndex && answerIdIndex!=-1 && newAnswerIndex!=-1)
      { out << "<b>You submitted two or more answers [answer tags: "
        << theProblem.theProblemData.theAnswers[answerIdIndex].answerId
        << " and " << theProblem.theProblemData.theAnswers[newAnswerIndex].answerId
        << "].</b> At present, multiple answer submission is not supported. ";
        return out.str();
      }
      if (answerIdIndex==-1)
      { out << "<b> You submitted an answer to tag with id "
        << studentAnswerNameReader
        << " which is not on my list of answerable tags. </b>";
        return out.str();
      }
      Answer& currentA=theProblem.theProblemData.theAnswers[answerIdIndex];
      currentA.currentAnswerURLed = theArgs.theValues[i];
      if (currentA.currentAnswerURLed=="")
      { out << "<b> Your answer to tag with id " << studentAnswerNameReader
        << " appears to be empty, please resubmit. </b>";
        return out.str();
      }
    }
  if (answerIdIndex==-1)
  { out << "<b>Something is wrong: I found no submitted answers.</b>";
    return out.str();
  }
  Answer& currentA=theProblem.theProblemData.theAnswers[answerIdIndex];
  currentA.currentAnswerClean=CGI::URLStringToNormal(currentA.currentAnswerURLed);
  currentA.currentAnswerURLed=CGI::StringToURLString(currentA.currentAnswerClean);//<-encoding back to overwrite malformed input
  //stOutput << "<hr>DEBUG: Processing answer: " << currentA.currentAnswerClean << " to answer object: " << currentA.ToString();
  theProblem.studentTagsAnswered.AddSelectionAppendNewIndex(answerIdIndex);
  std::stringstream studentAnswerStream, completedProblemStream;
  completedProblemStream << currentA.commandsBeforeAnswer;
  studentAnswerStream << currentA.answerId << "= (" << currentA.currentAnswerClean << ");";
  completedProblemStream << studentAnswerStream.str();
  completedProblemStream << theProblem.CleanUpCommandString(currentA.commandVerificationOnly);
  //stOutput << "<br>DEBUG: input to the calculator: " << completedProblemStream.str() << "<hr>";
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds()+20;
  theInterpreter.init();
  theInterpreter.Evaluate(completedProblemStream.str());
  if (theInterpreter.flagAbortComputationASAP || theInterpreter.syntaxErrors!="")
  { out << "<b>Error while processing your answer(s).</b> Here's what I understood. ";
    Calculator isolatedInterpreter;
    isolatedInterpreter.init();
    theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds()+20;
    isolatedInterpreter.Evaluate("("+currentA.currentAnswerClean+")");
    if (isolatedInterpreter.syntaxErrors!="")
      out << isolatedInterpreter.ToStringSyntacticStackHumanReadable(false);
    else
      out << isolatedInterpreter.outputString;
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    { out << "<hr><b>Admin view internals:</b><br>" << theInterpreter.outputString
      << "<br>" << theInterpreter.outputCommentsString
      << "<hr><b>Submited completed problem stream.</b><br>" << completedProblemStream.str();

    }
//    stOutput << "yer input: " << completedProblemStream.str();
//    stOutput << theInterpreter.outputString;
    return out.str();
  }
  bool isCorrect=false;
  int mustBeOne=-1;
  if (!theInterpreter.theProgramExpression[theInterpreter.theProgramExpression.size()-1].IsSmallInteger(&mustBeOne))
    isCorrect=false;
  else
    isCorrect=(mustBeOne==1);
  out << "<table width=\"300\"><tr><td>";
#ifdef MACRO_use_MySQL
  int correctSubmissionsRelevant=0;
  int totalSubmissionsRelevant=0;
  DatabaseRoutines theRoutines;
  UserCalculator& theUser=theProblem.currentUseR;
  theUser.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  bool deadLinePassed=false;
  bool hasDeadline=true;
  double secondsTillDeadline=-1;
  if (theProblem.flagIsForReal)
  { if (!theUser.InterpretDatabaseProblemData(theUser.problemDataString.value, comments))
    { out << "<b>Failed to load user information from database. Answer not recorded. "
      << "This should not happen. " << CalculatorHTML::BugsGenericMessage << "</b>";
      theProblem.flagIsForReal=false;
    } else
    { if (!theProblem.LoadAndParseTopicList(out))
        hasDeadline=false;
      if (hasDeadline)
      { bool unused=false;
        std::string theDeadlineString=
        theProblem.GetDeadline(theProblem.fileName, theProblem.currentUseR.userGroup.GetDataNoQuotes(), unused);
        if (theDeadlineString=="" || theDeadlineString==" ")
          hasDeadline=false;
        else
        { TimeWrapper now, deadline; //<-needs a fix for different time formats.
          //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
          std::stringstream badDateStream;
          if (!deadline.AssignMonthDayYear(theDeadlineString, badDateStream))
          { out << "<b>Problem reading deadline. </b> The deadline string was: "
            << theDeadlineString << ". Comments: "
            << "<span style=\"color:red\">" << badDateStream.str() << "</span>"
            << " This should not happen. " << CalculatorHTML::BugsGenericMessage;
            return out.str();
          }
          //  out << "deadline.date: " << deadline.theTime.tm_mday;
          now.AssignLocalTime();
          //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
          //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
          secondsTillDeadline= deadline.SubtractAnotherTimeFromMeInSeconds(now)+7*3600;
          deadLinePassed=(secondsTillDeadline<-18000);
          deadLinePassed=false;
        }
      }
      if (deadLinePassed)
        out << "<tr><td><span style=\"color:red\"><b>Deadline passed, attempt not recorded</b></span></td></tr>";
      else
        for (int i=0; i<theProblem.studentTagsAnswered.CardinalitySelection; i++)
        { int theIndex=theProblem.studentTagsAnswered.elements[i];
          Answer& currentA=theProblem.theProblemData.theAnswers[theIndex];
          currentA.numSubmissions++;
          totalSubmissionsRelevant+=currentA.numSubmissions;
          correctSubmissionsRelevant+=currentA.numCorrectSubmissions;
          if (isCorrect)
          { currentA.numCorrectSubmissions++;
            correctSubmissionsRelevant++;
            if (currentA.firstCorrectAnswerClean=="")
              currentA.firstCorrectAnswerClean=currentA.currentAnswerClean;
            else
              out << "[correct answer already submitted: " << currentA.firstCorrectAnswerClean << "]";
          }
        }
    }
  }
#endif // MACRO_use_MySQL
  if (!isCorrect)
  { out << "<span style=\"color:red\"><b>Your answer appears to be incorrect. </b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
      out << "For debug purposes: the calculator output is: " << theInterpreter.outputString
      << "Comments: " << theInterpreter.Comments.str() << "<hr>Calculator input was:<hr>"
      << theInterpreter.inputString << "<hr>";
  } else
    out << "<span style=\"color:green\"><b>Correct! </b></span>";
  out << "</td></tr>";
#ifdef MACRO_use_MySQL
  if (theProblem.flagIsForReal)
  { std::stringstream comments;
    theProblem.currentUseR.SetProblemData(theProblem.fileName, theProblem.theProblemData);
    if (!theProblem.currentUseR.StoreProblemDataToDatabase(theRoutines, comments))
      out << "<tr><td><b>This shouldn't happen and may be a bug: failed to store your answer in the database. "
      << CalculatorHTML::BugsGenericMessage << "</b><br>Comments: "
      << comments.str() << "</td></tr>";
    else
      out << "<tr><td>So far " << correctSubmissionsRelevant << " correct and "
      << totalSubmissionsRelevant-correctSubmissionsRelevant << " incorrect submissions.</td></tr>";
    if (hasDeadline)
    { if (secondsTillDeadline<0)
        secondsTillDeadline*=-1;
      if (deadLinePassed)
        out << "<tr><td><span style=\"color:red\"><b>Submission "
        << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false)
        << " after deadline. </b></span></td></tr>";
      else
        out << "<tr><td><span style=\"color:green\"><b>Submission "
        << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false)
        << " before deadline. </b></span></td></tr>";
    } else
      out << "<tr><td><span style=\"color:green\"><b>No deadline yet.</b></span></td></tr>";
  } //else
    //stOutput << "<tr><td><b>Submitting problem solutions allowed only for logged-in users. </b></td></tr>";
#endif
  out << "<tr><td>Your answer was: ";
  std::string errorMessage;
  out << "\\(" << currentA.currentAnswerClean << "\\)";
  errorMessage=theInterpreter.ToStringIsCorrectAsciiCalculatorString(currentA.currentAnswerClean);
  if (errorMessage!="")
    out << "<br>" << errorMessage
    << "<hr><b>If you entered this expression through the keyboard (without copying + pasting) this is a bug: "
    << "please report it to the web site administrator. Don't forget to mention your keyboard/character setup. "
    << "Are you using the standard English keyboard? Cyrillic, Chinese, etc. characters are not accepted. </b> "
    << "<hr><span style=\"color:red\"><b>Copying and pasting an answer not computed by yourself "
    << " is considered cheating (example: answer from an online program for doing homework).</b> </span>";
  out << "</td></tr>";
  out << "</table>";
  out << "Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";

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
  std::string mainInput=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  if (!CGI::ChopCGIString(mainInput, theMap, out))
  { out << "<b>Failed to extract input from: " << mainInput << ".</b>";
    return out.str();
  }
  std::string desiredUsers=
  CGI::URLStringToNormal(theMap.GetValueCreateIfNotPresent("teachers"));
  std::string desiredSections=
  CGI::URLStringToNormal(theMap.GetValueCreateIfNotPresent("sections"));
  List<std::string> theDelimiters;
#ifdef MACRO_use_MySQL
  List<std::string> theTeachers, theSections;
  List<unsigned char> delimiters;
  delimiters.AddOnTop(' ');
  delimiters.AddOnTop('\r');
  delimiters.AddOnTop('\n');
  delimiters.AddOnTop('\t');
  delimiters.AddOnTop(',');
  delimiters.AddOnTop(';');
  delimiters.AddOnTop(160);//<-&nbsp
  MathRoutines::StringSplitExcludeDelimiters(desiredUsers, delimiters, theTeachers);
//  MathRoutines::StringSplitExcludeDelimiters(desiredSections, delimiters, theSections);
  if (theTeachers.size==0)
  { out << "<b>Could not extract teachers from " << desiredUsers << ".</b>";
    return out.str();
  }
//  if (theSections.size==0)
//  { out << "<b>Could not extract sections from " << desiredSections << ".</b>";
//    return out.str();
//  }
  DatabaseRoutines theRoutines;
  for (int i=0; i<theTeachers.size; i++)
    if (!theRoutines.SetEntry
        (DatabaseStrings::userColumnLabel, theTeachers[i], DatabaseStrings::usersTableName,
         DatabaseStrings::sectionsList, desiredSections, &out))
      out << "<span style=\"color:red\">Failed to assign: " << theTeachers[i] << " to section: "
      << desiredSections << "</span><br>";
    else
      out << "<span style=\"color:red\">Assigned " << theTeachers[i] << " to section: "
      << desiredSections << "</span><br>";
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
  std::string inputEmails=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("userList"));
  std::string userPasswords=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("passwordList"));
  std::string userGroup=
  MathRoutines::StringTrimWhiteSpace(CGI::URLStringToNormal(
  theGlobalVariables.GetWebInput(DatabaseStrings::userGroupLabel)));
  std::string userRole=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("userRole"));

  if (inputEmails=="")
  { out << "<b>No emails to add</b>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  bool sentEmails=true;
  //stOutput << "DEBUG: here be i!";
  bool doSendEmails= theGlobalVariables.userCalculatorRequestType=="sendEmails" ?  true : false;
  bool createdUsers=theRoutines.AddUsersFromEmails(inputEmails, userPasswords, userRole, userGroup, comments);
  if (createdUsers)
    out << "<span style=\"color:green\">Users successfully added. </span>";
  else
    out << "<span style=\"color:red\">Failed to add all users. </span>";
  if (doSendEmails)
  { if (sentEmails)
      out << "<span style=\"color:green\">Activation emails successfully sent. </span>";
    else
      out << "<span style=\"color:red\">Failed to send all activation emails. </span>";
  }
  bool usersAreAdmins= (userRole=="admin");
  List<List<std::string> > userTable;
  List<std::string> userLabels;
  if (!theRoutines.FetchAllUsers(userTable, userLabels, comments))
    out << comments.str();
  out << HtmlInterpretation::ToStringUserDetailsTable(usersAreAdmins, userTable, userLabels, hostWebAddressWithPort);
//    out << "<hr>Debug: got to here. ";
  if (!createdUsers || !sentEmails)
    out << "<br>Comments:<br>" << comments.str();
  return out.str();
#else
  return "<b>no database present.</b>";
#endif // MACRO_use_MySQL
}

const std::string CalculatorHTML::BugsGenericMessage=
"Please take a screenshot, copy the link address and send those along \
with a short explanation to the administrator of the web site. ";

std::string HtmlInterpretation::GetAnswerOnGiveUp()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetAnswerOnGiveUp");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem();
  std::stringstream out;
  if (!theProblem.flagLoadedSuccessfully)
  { out << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    if(theProblem.comments.str()!="")
      out << " Comments: " << theProblem.comments.str();
    return out.str();
  }
  if (theProblem.flagIsForReal)
  { out << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    return out.str();
  }
  if(theGlobalVariables.GetWebInput("randomSeed")=="")
  { out << " <b>I could not figure out the exercise problem (missing random seed). </b>";
    return out.str();
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(comments))
  { out << "<br><b>Problem preparation failed.</b><br>" << comments.str();
    return out.str();
  }

  std::string lastStudentAnswerID;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  for (int i=0; i<theArgs.size(); i++)
    MathRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  int indexLastAnswerId=theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId==-1)
  { out << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b><br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    { out << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
      //out << "<hr>Client input: " << this->mainArgumentRAW << "<hr>";
      out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
    }
    return out.str();
  }
  Answer& currentA=theProblem.theProblemData.theAnswers[indexLastAnswerId];
  if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly=="")
  { out << "<b> Unfortunately there is no answer given for this "
    << "question (answerID: " << lastStudentAnswerID << ").</b>";
    if (theGlobalVariables.UserDebugFlagOn()&&
        theGlobalVariables.UserDefaultHasProblemComposingRights())
      out << "<br>Answer status: " << currentA.ToString();
    return out.str();
  }
  Calculator theInterpreteR;
  theInterpreteR.init();
  std::stringstream answerCommands;
  answerCommands << currentA.commandsBeforeAnswer;
  answerCommands << "CommandEnclosure{}(" << currentA.commandsNoEnclosureAnswerOnGiveUpOnly << ");";
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors!="")
  { out << "<span style=\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false);
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  if (theInterpreteR.flagAbortComputationASAP)
  { out << "<span style=\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return out.str();
  }
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal=true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
  theFormat.flagUseQuotes=false;
  theFormat.flagUseLatex=true;
  theFormat.flagUsePmatrix=true;
  bool isFirst=true;
  const Expression& currentE=
  theInterpreteR.theProgramExpression[theInterpreteR.theProgramExpression.size()-1][1];
  if (!currentE.StartsWith(theInterpreteR.opEndStatement()))
    out << "\\(" << currentE.ToString(&theFormat) << "\\)";
  else
    for (int j=1; j<currentE.size(); j++)
    { if (currentE[j].StartsWith(theInterpreteR.opRulesOff()) ||
          currentE[j].StartsWith(theInterpreteR.opRulesOn()))
        continue;
      if (!isFirst)
        out << "<br>";
      isFirst=false;
      theFormat.flagExpressionIsFinal=true;
      theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
      theFormat.flagUseQuotes=false;
      theFormat.flagUseLatex=true;
      if (currentE[j].IsOfType<std::string>())
        out << currentE[j].GetValue<std::string>();
      else
        out << "\\(" << currentE[j].ToString(&theFormat) << "\\)";
    }
  out << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
    out << "<hr>" << theInterpreteR.outputString << "<hr>" << theInterpreteR.outputCommentsString
    << "<hr>Raw input: " << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
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
  << "<br>Database user: " << theRoutines.databaseUser;
  if (!theRoutines.PrepareClassData(notUsed, userTable, columnLabels, commentsOnFailure))
  { out << "<b>Failed to load user info.</b> Comments: " << commentsOnFailure.str();
    return out.str();
  }
//  out << "DEBUG: Usertable: " << userTable.ToStringCommaDelimited();
  int indexExtraInfo=-1;
  for (int i=0; i<columnLabels.size; i++)
    if (columnLabels[i]=="userInfo")
      indexExtraInfo=i;
  if (indexExtraInfo==-1)
  { out << "Failed to load extra user info. ";
    return out.str();
  }
  HashedList<std::string, MathRoutines::hashString> theSections;
  for (int i=0; i<userTable.size; i++)
    theSections.AddOnTopNoRepetition(userTable[i][indexExtraInfo]);
  theSections.QuickSortAscending();
  out << "<hr><hr>";
  out << HtmlInterpretation::ToStringAssignSection();
  out << "<hr><hr>";
  out << HtmlInterpretation::ToStringUserDetails(true, userTable, columnLabels, hostWebAddressWithPort);
  out << "<hr><hr>";
  out << HtmlInterpretation::ToStringUserDetails(false, userTable, columnLabels, hostWebAddressWithPort);
  return out.str();
#else
  return "<b>Database not available. </b>";
#endif // MACRO_use_MySQL
}

std::string HtmlInterpretation::GetScoresPage()
{ MacroRegisterFunctionWithName("WebWorker::GetScoresPage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << HtmlSnippets::GetJavascriptStandardCookies()
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  out << "<problemNavigation>" << theGlobalVariables.ToStringNavigation() << "</problemNavigation>\n";
  out << HtmlInterpretation::ToStringUserScores();
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();


}

std::string HtmlInterpretation::GetAccountsPage(const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("WebWorker::GetAccountsPage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << HtmlSnippets::GetJavascriptStandardCookies()
  << HtmlSnippets::GetJavascriptSubmitEmails()
  << HtmlSnippets::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  out << "<problemNavigation>" << theGlobalVariables.ToStringNavigation() << "</problemNavigation>\n";
  out << HtmlInterpretation::GetAccountsPageBody(hostWebAddressWithPort);
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
  std::string userRole = adminsOnly ? "admin" : "student";
  int numUsers=0;
  std::stringstream tableStream;
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th>"
  << "<th>Activation manual email</th>"
  << "<th>Section/Group</th> <th>Pre-filled login link</th><th>Score</th></tr>";
  UserCalculator currentUser;
  currentUser.currentTable=DatabaseStrings::usersTableName;
  int indexUser=-1;
  int indexEmail=-1;
  int indexActivationToken=-1;
  int indexUserRole=-1;
  int indexExtraInfo=-1;
  //int indexProblemData=-1;
  for (int i=0; i<columnLabels.size; i++)
  { if (columnLabels[i]==DatabaseStrings::userColumnLabel)
      indexUser=i;
    if (columnLabels[i]=="email")
      indexEmail=i;
    if (columnLabels[i]=="activationToken")
      indexActivationToken=i;
    if (columnLabels[i]=="userRole")
      indexUserRole=i;
    if (columnLabels[i]==DatabaseStrings::userGroupLabel)
      indexExtraInfo=i;
  }
  if (
      indexUser           ==-1 ||
      indexEmail          ==-1 ||
      indexActivationToken==-1 ||
      indexUserRole       ==-1 ||
      indexExtraInfo      ==-1
      )
  { out << "<span style=\"color:red\"><b>This shouldn't happen: failed to find necessary "
    << "column entries in the database. "
    << "This is likely a software bug. Function: HtmlInterpretation::ToStringUserDetailsTable. </b></span>"
    << "indexUser, indexExtraInfo, indexEmail, indexActivationToken, indexUserRole:  "
    << indexUser << ", "
    << indexEmail          << ", "
    << indexActivationToken << ", "
    << indexUserRole << ", "
    << indexExtraInfo << ". "
    ;
    return out.str();
  }
  HashedList<std::string, MathRoutines::hashString> sectionNames;
  List<List<std::string> > activatedAccountBucketsBySection;
  List<List<std::string> > nonActivatedAccountBucketsBySection;
  for (int i=0; i<userTable.size; i++)
    sectionNames.AddOnTopNoRepetition(userTable[i][indexExtraInfo]);
  sectionNames.QuickSortAscending();
  activatedAccountBucketsBySection.SetSize(sectionNames.size);
  nonActivatedAccountBucketsBySection.SetSize(sectionNames.size);
  int numActivatedUsers=0;
  std::stringstream preFilledLoginLinks;
  for (int i=0; i<userTable.size; i++)
  { std::stringstream failureStream;
    currentUser.username=userTable[i][indexUser];
    currentUser.userGroup=CGI::URLStringToNormal( userTable[i][indexExtraInfo]);
    currentUser.email=userTable[i][indexEmail];
    currentUser.actualActivationToken=userTable[i][indexActivationToken];
    currentUser.userRole=userTable[i][indexUserRole];
    if (adminsOnly xor (currentUser.userRole=="admin"))
      continue;
    numUsers++;
    std::stringstream oneTableLineStream;
    oneTableLineStream << "<tr>"
    << "<td>" << currentUser.username.value << "</td>"
    << "<td>" << currentUser.email.value << "</td>"
    ;
    bool isActivated=true;
    std::string webAddress="https://"+ hostWebAddressWithPort;
    if (!theGlobalVariables.flagRunningAce)
      webAddress+="/calculator";
    if (currentUser.actualActivationToken.value!="activated" && currentUser.actualActivationToken.value!="error")
    { isActivated=false;
      numActivatedUsers++;
      oneTableLineStream << "<td><span style=\"color:red\">not activated</span></td>";
      if (currentUser.actualActivationToken.value!="")
        oneTableLineStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken.value, webAddress, userTable[i][indexUser])
        << "\"> (Re)activate account and change password</a>"
        << "</td>";
      oneTableLineStream << "<td>";
      oneTableLineStream
      << "<a href=\"mailto:" << currentUser.email.value
      << "?subject=Math 140 Homework account activation&";

      oneTableLineStream << "body=";
      std::stringstream emailBody;
      emailBody << "Dear student,\n you have not activated your homework server account yet. \n"
      << "To activate your account and set your password please use the link: "
      << CGI::StringToURLString("\n\n")
      << CGI::StringToURLString( UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken.value, webAddress,
         userTable[i][indexUser]) )
      << CGI::StringToURLString("\n\n")
      << "The link does not work with apple safari; if you use safari, please contact us by email"
      << " and we will activate your account manually. "
      << " Once you activate your account, you can log in safely here: \n"
      << CGI::StringToURLString("\n\n")
      << webAddress
      << CGI::StringToURLString("\n\n")
      << "Best regards, \n your Math 140 instructors."
      ;
      oneTableLineStream << emailBody.str() << "\">Send email manually.</a> "
      ;
      oneTableLineStream << "</td>";
      //      else
        //  oneTableLineStream << "<td>Activation token: " << currentUser.activationToken.value << "</td>";
    } else if (currentUser.actualActivationToken=="error")
      oneTableLineStream << "<td>error</td><td></td>";
    else
      oneTableLineStream << "<td><span style=\"color:green\">activated</span></td><td></td><td></td>";
    oneTableLineStream << "<td>" << userTable[i][indexExtraInfo] << "</td>";
    std::stringstream oneLink;
    oneLink << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=login&username="
    << currentUser.username.value << "\">" << currentUser.username.value << "</a>";
    oneTableLineStream << "<td>"  << oneLink.str()<< "</td>";
    preFilledLoginLinks << oneLink.str() << "<br>";
    oneTableLineStream << "</tr>";
    if (isActivated)
      activatedAccountBucketsBySection[sectionNames.GetIndex(userTable[i][indexExtraInfo])].AddOnTop(oneTableLineStream.str());
    else
      nonActivatedAccountBucketsBySection[sectionNames.GetIndex(userTable[i][indexExtraInfo])].AddOnTop(oneTableLineStream.str());
  }
  for (int i=0; i<nonActivatedAccountBucketsBySection.size; i++)
    nonActivatedAccountBucketsBySection[i].QuickSortAscending();
  for (int i=0; i<activatedAccountBucketsBySection.size; i++)
    activatedAccountBucketsBySection[i].QuickSortAscending();
  for (int i=0; i<nonActivatedAccountBucketsBySection.size; i++)
    for (int j=0; j<nonActivatedAccountBucketsBySection[i].size; j++)
      tableStream << nonActivatedAccountBucketsBySection[i][j];
    for (int i=0; i<activatedAccountBucketsBySection.size; i++)
      for (int j=0; j<activatedAccountBucketsBySection[i].size; j++)
        tableStream << activatedAccountBucketsBySection[i][j];
    tableStream << "</table>";
    out << "\n" << numUsers << " user(s)";
    if (numActivatedUsers>0)
      out << ", <span style=\"color:red\">" << numActivatedUsers << " have not activated their accounts. </span>";
    out << tableStream.str() << preFilledLoginLinks.str();

  return out.str();
#else
  return "Compiled without database support. ";
#endif
}

std::string HtmlInterpretation::ToStringAssignSection()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringAssignSection");
  std::stringstream out;
  std::string idAddressTextarea= "inputSetTeacher";
  std::string idExtraTextarea= "inputSections";
  std::string idOutput="idOutputSections";
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
  << " 'setTeacher'"
  << " )\"> Set teacher</button> ";
  out << "<br><span id=\"" << idOutput << "\">\n";
  out << "</span>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserScores()
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserScores");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "only admins are allowed to view scores";
  std::stringstream out;
  CalculatorHTML theProblem;
  theProblem.currentUseR.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  theProblem.LoadFileNames();
  if (!theProblem.LoadAndParseTopicList(out))
    return out.str();
  if (!theProblem.PrepareSectionList(out))
    return out.str();
  List<List<std::string> > userTable;
  List<std::string> userLabels;
  DatabaseRoutines theRoutines;
  if (!theRoutines.FetchAllUsers(userTable, userLabels, out))
    return out.str();
  int usernameIndex=userLabels.GetIndex(DatabaseStrings::userColumnLabel);
  if (usernameIndex==-1)
    return "Could not find username column";
  int userInfoIndex=userLabels.GetIndex(DatabaseStrings::userGroupLabel);
  if (userInfoIndex==-1)
    return "Could not find user group column";
  int problemDataIndex=userLabels.GetIndex("problemData");
  if (problemDataIndex==-1)
    return "Could not find problem data column";
  int problemWeightScheme=userLabels.GetIndex(DatabaseStrings::problemWeightsIdColumnName);
  if (problemWeightScheme==-1)
    return "Could not find problem weight scheme";
  List<Rational> userScores;
  CalculatorHTML currentUserRecord;
  userScores.SetSize(userTable.size);
  for (int i=0; i<userTable.size; i++)
  { userScores[i]=-1;
    currentUserRecord.currentUseR.username=userTable[i][usernameIndex];
    //out << "<hr>Debug: reading db problem data from: "
    //<< CGI::URLKeyValuePairsToNormalRecursiveHtml(userTable[i][problemDataIndex]) << "<br>";
    if (!currentUserRecord.currentUseR.InterpretDatabaseProblemData(userTable[i][problemDataIndex], out))
      continue;
    //out << "<br>DEBUG: after db data read: " << currentUserRecord.currentUseR.ToString();
    currentUserRecord.ReadProblemInfoAppend
    (theProblem.currentUseR.problemInfoString.value,
     currentUserRecord.currentUseR.theProblemData, out);
    //out << "<br>DEBUG: after ReadProblemInfoAppend: " << currentUserRecord.currentUseR.ToString();
    currentUserRecord.currentUseR.ComputePointsEarned(theProblem.problemNamesNoTopics);
    userScores[i]=currentUserRecord.currentUseR.pointsEarned;
    //out << "<br>DEBUG: Computed scores from: " << currentUserRecord.currentUseR.ToString();
  }
//  out << "DBUG: prob names: " << theProblem.problemNamesNoTopics.ToStringCommaDelimited();
  out << "<table><tr><th>User</th><td>Section</td><td>Score</td></tr>";
  for (int i=0; i<userTable.size; i++)
    out << "<tr><td>" << userTable[i][usernameIndex] << "</td>"
    << "<td>" << userTable[i][userInfoIndex] << "</td>"
    << "<td>" << userScores[i].GetDoubleValue() << "</td></tr>";
  out << "</table>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserDetails
(bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& columnLabels,
 const std::string& hostWebAddressWithPort)
{ MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserDetails");
  std::stringstream out;
#ifdef MACRO_use_MySQL
  std::string userRole = adminsOnly ? "admin" : "student";
  std::string idAddressTextarea= "inputAddUsers"+userRole;
  std::string idExtraTextarea= "inputAddExtraInfo"+userRole;
  std::string idOutput="idOutput"+userRole;
  std::string idPasswordTextarea="inputAddDefaultPasswords"+userRole;
  out << "Add <b>" << userRole << "(s)</b>.<br> ";
//  out << "<b>Warning: there's no remove button yet.</b><br>";
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
  std::string linkSeparator=" | ";
  std::string linkBigSeparator=" || ";
  if (theGlobalVariables.userCalculatorRequestType=="template")
    out << "<b>Home</b>" << linkSeparator;
  else
  { std::string topicList=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
    std::string courseHome=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("courseHome"));
    if (topicList!="" && courseHome!="")
    { std::string studentView=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("studentView"));
      std::string section=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("studentSection"));
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=template"
      << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
      << "studentView=" << studentView << "&";
      if (section!="")
        out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
      out << "topicList=" << topicList << "&";
      out << "courseHome=" << courseHome << "&";
      out << "fileName=" << courseHome << "&\">Home</a>"
      << linkSeparator;
    }
  }
  out << "<a href=\"/selectCourse.html\">Select course</a>" << linkBigSeparator;
  if (theGlobalVariables.flagLoggedIn)
  { out << "User";
    if (theGlobalVariables.UserDefaultHasAdminRights())
      out << " <b>(admin)</b>";
    out << ": " << theGlobalVariables.userDefault.username.value << linkSeparator;
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=logout&";
    out << theGlobalVariables.ToStringCalcArgsNoNavigation(true) << " \">Log out</a>" << linkSeparator;
    if (theGlobalVariables.flagUsingSSLinCurrentConnection)
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(true) << "\">Change password</a>" << linkSeparator;
    else
      out << "<b>Password change: <br>secure connection<br>only</b>" << linkSeparator;
    if (theGlobalVariables.userDefault.userGroup.value!="")
      out << "Section: " << theGlobalVariables.userDefault.userGroup.value << linkSeparator;
  }

  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
  { if (theGlobalVariables.userCalculatorRequestType!="accounts")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
      << "\">Accounts</a>" << linkSeparator;
    else
      out << "<b>Accounts</b>" << linkSeparator;
    if (theGlobalVariables.userCalculatorRequestType!="scores")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=scores&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
      << "\">Scores</a>" << linkSeparator;
    else
      out << "<b>Scores</b>" << linkSeparator;
    if (theGlobalVariables.userCalculatorRequestType!="status")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=status&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
      << "\">Server</a>" << linkSeparator;
    else
      out << "<b>Server</b>" << linkBigSeparator;
    if (theGlobalVariables.userCalculatorRequestType!="database")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=database&"
      << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
      << "\">Database</a>" << linkBigSeparator;
    else
      out << "<b>Database</b>" << linkBigSeparator;
  }
  if (theGlobalVariables.userCalculatorRequestType!="compute")
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=compute&"
    << theGlobalVariables.ToStringCalcArgsNoNavigation(true) << " \">Calculator</a>" << linkBigSeparator;
  else
    out << "<b>Calculator</b> " << linkBigSeparator;
  return out.str();
}
