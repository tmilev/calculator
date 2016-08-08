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
    this->outputHtmlMain=out.str();
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
      out << this->theContent[i].ToStringInterpreted();
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
  this->outputHtmlNavigation=this->ToStringProblemNavigation();
  this->outputHtmlMain=out.str();
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
