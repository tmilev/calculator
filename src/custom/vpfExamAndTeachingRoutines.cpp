//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "../vpfHeader8HtmlExamAndTeachingCommon.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCustomCode
(__FILE__, "Routines for calculus teaching: calculator exam mode. Shared code. ");

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  std::stringstream out;
  if (!this->flagIsForReal || !this->theProblemData.flagRandomSeedComputed)
    if (!this->flagRandomSeedGiven || this->NumAttemptsToInterpret>1)
      this->theProblemData.randomSeed=this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret-1];
  //stOutput << "DEBUG: Interpreting problem with random seed: " << this->theProblemData.randomSeed;
  this->FigureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before after loading problem list");
  if (!theGlobalVariables.flagRunningAceWebserver)
    out << this->GetJavascriptSubmitMainInputIncludeCurrentFile();
//  else
//    out << " no date picker";
  theInterpreter.flagWriteLatexPlots=false;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before execution");
  if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    return false;
//////////////////////////////interpretation takes place before javascript generation as the latter depends on the former.
  if (this->flagIsExamProblem)
    out << this->GetJavascriptSubmitAnswers();
  else if (this->flagIsExamHome)
  { if (theGlobalVariables.flagRunningAceWebserver)
      out << HtmlSnippets::GetJavascriptSubmitEmailsAce(this->fileName);
    else
      out << HtmlSnippets::GetJavascriptSubmitEmailS(this->fileName);
  }
  if (this->flagIsExamHome && theGlobalVariables.UserDefaultHasAdminRights() &&
      !theGlobalVariables.UserStudentViewOn())
  { out << HtmlSnippets::GetJavascriptHideHtml();
    out << this->GetDatePickerJavascriptInit();
  }
  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
    out << this->GetEditPageButton();
  if (this->flagIsExamProblem)
  { bool problemAlreadySolved=false;
#ifdef MACRO_use_MySQL
    if (this->currentUser.problemNames.Contains(this->fileName))
    { ProblemData& theProbData=this->currentUser.problemData[this->currentUser.problemNames.GetIndex(this->fileName)];
      if (theProbData.numCorrectlyAnswered>=theProbData.theAnswers.size)
        problemAlreadySolved=true;
    }
#endif
    CalculatorHTML theProblemHome;
    theProblemHome.fileName=this->currentExamHomE;
    bool isGood=true;
    if (!theProblemHome.LoadMe(true, comments))
      isGood=false;
    if (isGood)
      if (!theProblemHome.ParseHTML(comments))
        isGood=false;
         //     stOutput << "Time after loading collection: " << theGlobalVariables.GetElapsedSeconds()-startTime;
    if (!isGood && !theGlobalVariables.flagRunningAsProblemInterpreter)
    { out << "<b>Failed to load problem collection home: " << this->currentExamHomE
      << ". Comments: " << comments.str()  << "</b>";
    }
#ifdef MACRO_use_MySQL
    out << theProblemHome.ToStringOnEDeadlineFormatted
    (this->fileName, this->currentUser.extraInfoUnsafe, true, problemAlreadySolved);
#endif
    out << "<br>";
  }

  //////////////////////////////
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time after execution");
  std::string lastCommands;
  //out << "DEBUG nfo, remove when done. <br>randseed: " << this->theProblemData.randomSeed
  //<< "<br> forReal: " << this->flagIsForReal << " seed computed: " << this->theProblemData.flagRandomSeedComputed
  //<< " flagRandomSeedGiven: " << this->flagRandomSeedGiven
  //<< this->ToStringCalculatorArgumentsForProblem("exercise");
  //first command and first syntactic element are the random seed and are ignored.
  if (!this->ProcessInterprettedCommands(theInterpreter, this->theContent, comments))
  { out << comments.str();
    this->outputHtmlBodyNoTag=out.str();
    return false;
  }
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before class management routines");
  //stOutput << "got to here, this->theProblemData: " << this->theProblemData.ToString();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorManageClass")
    { this->flagLoadedClassDataSuccessfully= this->PrepareClassData(comments);
      break;
    }
  //out << "Debug data: homework groups found: " << this->hdHomeworkGroups.ToStringCommaDelimited();
  this->PrepareAnswerElements(comments);
  this->NumAnswerIdsMathquilled=0;
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculator(this->theContent[i]);
  this->InterpretAnswerElements(comments);
  for (int i=0; i<this->theContent.size; i++)
    if (!this->theContent[i].IsHidden())
      out << this->theContent[i].ToStringInterpretedBody();
  for (int i=0; i<this->theProblemData.theAnswers.size; i++)
    out << this->theProblemData.theAnswers[i].htmlMQjavascript;
  ////////////////////////////////////////////////////////////////////
  out << "<script type=\"text/javascript\"> \n ";
  out << "answerMQspanIds = [";
  for (int i=0; i<this->theProblemData.theAnswers.size; i++)
  { out << "\"" << this->theProblemData.theAnswers[i].idMQfield << "\"";
    if (i!=this->theProblemData.theAnswers.size-1)
      out << ", ";
  }
  out << "];\n";
  out << "preferredButtonContainers = [";
  for (int i=0; i<this->theProblemData.theAnswers.size; i++)
  { out << "\"" << this->theProblemData.theAnswers[i].idMQButtonPanelLocation << "\"";
    if (i!=this->theProblemData.theAnswers.size-1)
      out << ", ";
  }
  out << "];\n";
  out << "answerIdsPureLatex = [";
  for (int i=0; i<this->theProblemData.theAnswers.size; i++)
  { out << "\"" << this->theProblemData.theAnswers[i].answerId << "\"";
    if (i!=this->theProblemData.theAnswers.size-1)
      out << ", ";
  }
  out << "];\n";
  out << "</script>";
  ////////////////////////////////////////////////////////////////////
  //out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
  //out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before database storage");
#ifdef MACRO_use_MySQL
  if (this->flagIsForReal)
  { //stOutput << "This is for real!<br>";
    this->theProblemData.flagRandomSeedComputed=true;
    DatabaseRoutines theRoutines;
    //this->currentUser.username=theGlobalVariables.userDefault;
    //stOutput << "About to store problem data: " << this->theProblemData.ToString();
    //if (!this->currentUser.InterpretDatabaseProblemData(this->currentUserDatabaseString, comments))
      //out << "<b>Error: corrupt database string. </b>";
    //else
    this->currentUser.SetProblemData(this->fileName, this->theProblemData);
    if (!this->currentUser.StoreProblemDataToDatabase(theRoutines, comments))
      out << "<b>Error: failed to store problem in database. </b>" << comments.str();
  }
  //out << "Current collection problems: " << this->databaseProblemList.ToStringCommaDelimited()
  //<< " with weights: " << this->databaseProblemWeights.ToStringCommaDelimited();
#endif // MACRO_use_MySQL
  this->outputHtmlNavigatioN=this->ToStringProblemNavigation();
  this->outputHtmlBodyNoTag=out.str();
  return true;
}

std::string CalculatorHTML::ToStringProblemNavigation()const
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemNavigation");
  std::stringstream out;
  std::string exerciseRequest="exercise";
  std::string studentView= theGlobalVariables.UserStudentViewOn() ? "true" : "false";
  std::string linkSeparator=" | ";
  std::string linkBigSeparator=" || ";
  if (theGlobalVariables.UserGuestMode())
    exerciseRequest="exerciseNoLogin";
  if (theGlobalVariables.UserGuestMode())
  { out << "<b>Guest mode</b><br>No points scored/deadlines. <hr>"
    << "<a href=\"login\" "
    << ">Log in</a><br>";
  }
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  std::string calcArgsNoPassExamDetails=
  theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (theGlobalVariables.UserDefaultHasAdminRights())
  { if (theGlobalVariables.UserStudentViewOn())
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem
      (theGlobalVariables.userCalculatorRequestType, "false", theGlobalVariables.GetWebInput("studentSection"))
      << "\">Admin view</a>" << linkSeparator;
    else
    { if (this->databaseStudentSectionS.size==0)
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?"
        << this->ToStringCalculatorArgumentsForProblem
        (theGlobalVariables.userCalculatorRequestType, "true", "")
        << "\">Student view</a><br>";
      for (int i=0; i<this->databaseStudentSectionS.size; i++)
        if (this->databaseStudentSectionS[i]!="")
          out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?"
          << this->ToStringCalculatorArgumentsForProblem
          (theGlobalVariables.userCalculatorRequestType, "true", this->databaseStudentSectionS[i])
          << "\">Student view section " << this->databaseStudentSectionS[i] << " </a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem)
  { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?request="
    << exerciseRequest << "&"
    << calcArgsNoPassExamDetails
    << "studentView=" << studentView << "&";
    if (theGlobalVariables.GetWebInput("studentSection")!="")
      out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
    out << "currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "&"
    << "fileName=" << CGI::StringToURLString(this->currentExamHomE) << "&\"> Home </a>"
    << linkSeparator;
  }
  if (this->flagIsExamHome)
    out << "<b>Course homework home</b>";
  if (this->flagIsExamProblem)
  { if (theGlobalVariables.userCalculatorRequestType=="exercise")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem("scoredQuiz", studentView)
      << "\">&nbsp&nbspStart for real</a>" << linkSeparator;
    else if (theGlobalVariables.userCalculatorRequestType=="scoredQuiz")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem("exercise", studentView)
      << "\">&nbsp&nbspExercise</a>" << linkSeparator;
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated)
  { int indexInParent=this->problemListOfParent.GetIndex(this->fileName);
    if (indexInParent==-1)
    { out << "<b>Could not find the problem collection that contains this problem.</b><br>";
//      out << "<b>This is either a faulty link or a programming error. </b> "
      //out << "this->filename is: " << this->fileName << " and the problem list of the parent is: "
      //<< this->problemListOfParent.ToStringCommaDelimited();
    } else
    { if (indexInParent>0)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&fileName=" << CGI::StringToURLString(this->problemListOfParent[indexInParent-1] )
        << "&\"> <-Previous </a>" << linkSeparator;
      }
      if (indexInParent<this->problemListOfParent.size-1)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&fileName=" << CGI::StringToURLString(this->problemListOfParent[indexInParent+1] )
        << "\"> Next-> </a>" << linkSeparator;
      } else
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&fileName=" << CGI::StringToURLString(this->currentExamHomE)
        << "&\">Last problem, return to course page.</a>" << linkSeparator;
      }
    }
  }
  if (this->flagIsExamProblem &&
      (theGlobalVariables.userCalculatorRequestType=="exercise" ||
       theGlobalVariables.userCalculatorRequestType=="exerciseNoLogin"))
    out << linkBigSeparator << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?"
    << this->ToStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">Link to this problem</a>";
  out << linkBigSeparator << theGlobalVariables.ToStringNavigation();
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem
(const std::string& requestType, const std::string& studentView,
 const std::string& studentSection, bool includeRandomSeedIfAppropriate)const
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsForProblem");
  if (!theGlobalVariables.flagLoggedIn && !theGlobalVariables.UserGuestMode() &&
      !theGlobalVariables.flagRunningAsProblemInterpreter)
    return "";
  std::stringstream out;
  List<std::string> excludedTags;
  excludedTags.AddOnTop("randomSeed");
  out << "request=" << requestType << "&";
  out << theGlobalVariables.ToStringCalcArgsNoNavigation(&excludedTags)
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&";
  if  (!theGlobalVariables.flagRunningAsProblemInterpreter && this->fileName!="")
    out << "fileName=" << CGI::StringToURLString(this->fileName) << "&";
  else
    out << "fileName=" << CGI::StringToURLString(theGlobalVariables.GetWebInput("fileName") )
    << "&";
  if (!theGlobalVariables.UserGuestMode())
  { out << "studentView=" << studentView << "&";
    if (studentSection!="")
      out << "studentSection=" << CGI::StringToURLString(studentSection) << "&";
  }
  if (//theGlobalVariables.GetWebInput("randomSeed")=="" &&
      //theGlobalVariables.userCalculatorRequestType!="scoredQuiz" &&
      includeRandomSeedIfAppropriate)
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
//  out << "fileName=" << CGI::StringToURLString(this->fileName) << "&";
  return out.str();
}

std::string CalculatorHTML::GetEditPageButton()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetEditPageButton");
  std::stringstream out;
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath
  << "?request=editPage&";
  std::string urledProblem=CGI::StringToURLString(this->fileName);
  std::stringstream refStreamNoRequest;
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "fileName=" << urledProblem << "&"
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&";
  out << refStreamNoRequest.str() << "\">" << "Edit problem/page" << "</a>";
  out << "<textarea id=\"clonePageAreaID\" rows=\"1\" cols=\"100\">" << this->fileName << "</textarea>\n"
  << "<button class=\"normalButton\" onclick=\""
  << "submitStringAsMainInput(document.getElementById('clonePageAreaID').value, 'spanCloningAttemptResultID', 'clonePage');"
  << "\" >Clone page</button> <span id=\"spanCloningAttemptResultID\"></span><br><br>";
  return out.str();
}

std::string CalculatorHTML::GetJavascriptSubmitMainInputIncludeCurrentFile()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitStringAsMainInput(theString, idOutput, requestType){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idOutput + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  inputParams='request='+requestType;\n"
  << "  inputParams+='&" << theGlobalVariables.ToStringCalcArgsNoNavigation() << "';\n"
  << "  inputParams+='&fileName=" << CGI::StringToURLString(this->fileName) << "';\n"
  << "  inputParams+='&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "';\n"
  << "  inputParams+='&mainInput=' + encodeURIComponent(theString);\n"
//  << "  inputParams+='&currentExamHome=' + problemCollectionName;\n"
  << "  var https = new XMLHttpRequest();\n"
////////////////////////////////////////////
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameExecutableWithPath << "\", true);\n"
/////////////////or/////////////////////////
//  << "  https.open(\"GET\", \"" << theGlobalVariables.DisplayNameExecutableWithPath << "\""
//  << "+ \"?\"+inputParams"
//  << ", true);\n"
////////////////////////////////////////////
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    spanOutput.innerHTML=https.responseText;\n"
  << "  }\n"
////////////////////////////////////////////
  << "  https.send(inputParams);\n"
/////////////////or/////////////////////////
//  << "  https.send();\n"
////////////////////////////////////////////
  << "}\n"
  << "</script>";
  return out.str();
}

std::string CalculatorHTML::InterpretGenerateProblemManagementLink
(std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateProblemManagementLink");
  std::stringstream out;
  if (!theGlobalVariables.UserGuestMode())
    out << " <a href=\"" << refStreamForReal.str() << "\">Start (for credit)</a> ";
  out << " <a href=\"" << refStreamExercise.str() << "\">Exercise (no credit, unlimited tries)</a> ";
  if (theGlobalVariables.UserGuestMode())
  { out << "Exercise points/deadlines require login. ";
    return out.str();
  }
  //stOutput << "<hr>CurrentUser.problemNames=" << this->currentUser.problemNames.ToStringCommaDelimited();
  std::string thePoints="";
  if (this->databaseProblemAndHomeworkGroupList.Contains(cleaneduplink))
    thePoints= this->databaseProblemWeights[this->databaseProblemAndHomeworkGroupList.GetIndex(cleaneduplink)];
  #ifdef MACRO_use_MySQL
  bool noSubmissionsYet=false;
  bool weightPrinted=false;
  if (this->currentUser.problemNames.Contains(cleaneduplink))
  { ProblemData& theProbData=this->currentUser.problemData[this->currentUser.problemNames.GetIndex(cleaneduplink)];
    if (!theProbData.flagProblemWeightIsOK)
    { out << "<span style=\"color:orange\">No point weight assigned yet. </span>";
      if (theProbData.ProblemWeightUserInput!="")
        out << "<span style=\"color:red\"><b>Failed to interpret weight string: "
        << theProbData.ProblemWeightUserInput << ". </b></span>";
      if (theProbData.theAnswers.size==1)
      { if (theProbData.numCorrectlyAnswered==1)
          out << theProbData.totalNumSubmissions << " submission(s), problem correctly answered. ";
        else
          out << theProbData.totalNumSubmissions << " submission(s), problem not correctly answered yet. ";
      } else if (theProbData.theAnswers.size>1)
        out << theProbData.totalNumSubmissions << " submission(s), " << theProbData.numCorrectlyAnswered
        << " out of "<< theProbData.theAnswers.size << " subproblems correctly answered. ";
      weightPrinted=true;
    } else if (theProbData.totalNumSubmissions==0)
      noSubmissionsYet=true;
    else if (theProbData.numCorrectlyAnswered<theProbData.theAnswers.size)
    { out << "<span style=\"color:red\"><b> "
      << theProbData.Points << " out of " << theProbData.ProblemWeight << " point(s). </b></span>";
      weightPrinted=true;
    } else if (theProbData.numCorrectlyAnswered==theProbData.theAnswers.size)
    { out << "<span style=\"color:green\"><b> "
      << theProbData.Points << " out of " << theProbData.ProblemWeight << " point(s). </b></span>";
      weightPrinted=true;
    }
  } else
    noSubmissionsYet=true;
  if (thePoints!="" && noSubmissionsYet)
  { out << "<span style=\"color:brown\"><b>No submissions: 0 out of " << thePoints
    << " point(s). </b> </span>" ;
    weightPrinted=true;
  }
  if (!weightPrinted)
    out << "<span style=\"color:orange\">No point weight assigned yet. </span>";
  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
  { //stOutput << "<hr>this->databaseProblemList is: " << this->databaseProblemList.ToStringCommaDelimited();
    //stOutput << "<br>this->databaseProblemWeights is: " << this->databaseProblemWeights.ToStringCommaDelimited();
    //stOutput << "<br> cleanedupLink: " << cleaneduplink;
    std::string idPoints = "points" + urledProblem;
    std::string idNumTries= "numTries"+urledProblem;
    std::string idButtonModifyPoints = "modifyPoints" + urledProblem;
    std::string idPointsModOutput = "modifyPointsOutputSpan" + urledProblem;
    out << "Points: <textarea rows=\"1\" cols=\"3\" id=\"" << idPoints << "\">";
    out << thePoints;
    out << "</textarea>";
    out << "<button id=\"" << idButtonModifyPoints << "\" "
    << "onclick=\"" << "submitStringAsMainInput('" << urledProblem
    << "='+encodeURIComponent('weight='+  getElementById('" << idPoints << "').value)"
//    << "  +encodeURIComponent('numTries='+getElementById('"
    << ", '"
    << idPointsModOutput << "', 'setProblemData');"
    << "\""
    << ">";
    out << "Modify";
    out << "</button>";
    out << "<span id=\"" << idPointsModOutput << "\">" << "</span>";
  }
  #endif // MACRO_use_MySQL
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  this->NumProblemsFound++;
  std::string cleaneduplink = this->CleanUpFileName(inputOutput.content);
  std::string urledProblem=CGI::StringToURLString(cleaneduplink);
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
//  out << "<span style=\"white-space: nowrap; display= inline-block; width=1200px; overflow-x: scroll;\">";
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "fileName=" << urledProblem << "&";
  if (theGlobalVariables.UserStudentViewOn())
  { refStreamNoRequest << "studentView=true&";
    if (theGlobalVariables.GetWebInput("studentSection")!="")
      refStreamNoRequest << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
  }
  if (this->currentExamHomE!="")
    refStreamNoRequest << "currentExamHome=" << this->currentExamHomE << "&";
  if (!theGlobalVariables.UserGuestMode())
  { refStreamExercise << theGlobalVariables.DisplayNameExecutableWithPath << "?request=exercise&" << refStreamNoRequest.str();
    refStreamForReal << theGlobalVariables.DisplayNameExecutableWithPath << "?request=scoredQuiz&" << refStreamNoRequest.str();
  } else
  { refStreamExercise << theGlobalVariables.DisplayNameExecutableWithPath
    << "?request=exerciseNoLogin&" << refStreamNoRequest.str();
  }
  if (inputOutput.GetTagClass()=="calculatorExamProblem")
    out << this->InterpretGenerateProblemManagementLink(refStreamForReal, refStreamExercise, cleaneduplink, urledProblem);
  //else
  //  out << " <a href=\"" << refStreamExercise.str() << "\">Start</a> ";
  //if (inputOutput.GetTagClass()=="calculatorExamIntermediate")
#ifdef MACRO_use_MySQL
  bool problemAlreadySolved=false;
  if (this->currentUser.problemNames.Contains(cleaneduplink))
  { ProblemData& theProbData=this->currentUser.problemData[this->currentUser.problemNames.GetIndex(cleaneduplink)];
    if (theProbData.numCorrectlyAnswered>=theProbData.theAnswers.size)
      problemAlreadySolved=true;
  }
  out << this->InterpretGenerateDeadlineLink
  (inputOutput, cleaneduplink, urledProblem, problemAlreadySolved);
#endif // MACRO_use_MySQL
  std::string stringToDisplay = FileOperations::GetFileNameFromFileNameWithPath(inputOutput.content);
  //out << " " << this->NumProblemsFound << ". "
  out << stringToDisplay;
//  out << "</span>";
  inputOutput.interpretedCommand=out.str();
}

void CalculatorHTML::InterpretTopicList(SyntacticElementHTML& inputOutput)
{ (void) inputOutput;
}
