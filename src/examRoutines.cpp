//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "../../calculator/src/vpfHeader8HtmlExamAndTeachingCommon.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCustomCode
(__FILE__, "Routines for calculus teaching: calculator exam mode. Shared code. ");

std::string CalculatorHTML::stringScoredQuizzes="Scored Quizzes";
std::string CalculatorHTML::stringPractice="Practice";
std::string CalculatorHTML::stringProblemLink="Problem link";

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  std::stringstream outBody;
  std::stringstream outHeaD, outHeadPt1, outHeadPt2;
  if (!this->flagIsForReal || !this->theProblemData.flagRandomSeedComputed)
    if (!this->flagRandomSeedGiven || this->NumAttemptsToInterpret>1)
      this->theProblemData.randomSeed=this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret-1];
  //stOutput << "DEBUG: Interpreting problem with random seed: " << this->theProblemData.randomSeed;
  this->FigureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before after loading problem list");
  outHeadPt2 << this->GetJavascriptSubmitMainInputIncludeCurrentFile();
//  else
//    out << " no date picker";
  theInterpreter.flagWriteLatexPlots=false;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before execution");
  if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    return false;
//////////////////////////////interpretation takes place before javascript generation as the latter depends on the former.
  if (this->flagIsExamProblem)
    outHeadPt2 << this->GetJavascriptSubmitAnswers();
  else if (this->flagIsExamHome)
    outHeadPt2 << HtmlSnippets::GetJavascriptSubmitEmails();
  if (this->flagIsExamHome && theGlobalVariables.UserDefaultHasAdminRights() &&
      !theGlobalVariables.UserStudentViewOn())
  { outHeadPt2 << HtmlSnippets::GetJavascriptHideHtml();
    outHeadPt2 << this->GetDatePickerJavascriptInit();
  }
  if (this->flagUseNavigationBar)
    if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
      outBody << this->GetEditPageButton();
  if (this->flagIsExamProblem)
  { bool problemAlreadySolved=false;
#ifdef MACRO_use_MySQL
    if (this->currentUseR.problemNames.Contains(this->fileName))
    { ProblemData& theProbData=this->currentUseR.problemData[this->currentUseR.problemNames.GetIndex(this->fileName)];
      if (theProbData.numCorrectlyAnswered>=theProbData.theAnswers.size)
        problemAlreadySolved=true;
    }
#endif
    int todoDetermineNextProblem;
    /*
    CalculatorHTML theProblemHome;
    theProblemHome.fileName=this->currentExamHomE;
    bool isGood=true;
    if (!theProblemHome.LoadMe(true, comments))
      isGood=false;
    if (isGood)
      if (!theProblemHome.ParseHTML(comments))
        isGood=false;
         //     stOutput << "Time after loading collection: " << theGlobalVariables.GetElapsedSeconds()-startTime;
    if (!isGood)
    { out << "<b>Failed to load problem collection home: " << this->currentExamHomE
      << ". Comments: " << comments.str()  << "</b>";
    }*/
#ifdef MACRO_use_MySQL
    int todoDetermineDeadlineIfAny;
//    out << theProblemHome.ToStringOnEDeadlineFormatted
//    (this->fileName, this->currentUser.extraInfoUnsafe, true, problemAlreadySolved);
#endif
    //outBody << "<br>";
  }

  //////////////////////////////
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time after execution");
  std::string lastCommands;
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  { outBody << "Debug information follows. ";
    if (this->flagIsExamProblem)
      outBody << "Exam problem here. ";
    outBody << "<br>randseed: " << this->theProblemData.randomSeed
    << "<br>ForReal: " << this->flagIsForReal << "<br>seed computed: " << this->theProblemData.flagRandomSeedComputed
    << "<br>flagRandomSeedGiven: " << this->flagRandomSeedGiven
    << CGI::StringToHtmlString(this->ToStringCalculatorArgumentsForProblem("exercise", "false"));

    outBody << "<br>Problem data: " << this->currentUseR.problemNames;
    outBody << "<br>Problem data string: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(this->currentUseR.problemDataString.value);
    outBody << "<hr>";
  }
  //first command and first syntactic element are the random seed and are ignored.
  if (!this->ProcessInterprettedCommands(theInterpreter, this->theContent, comments))
  { outBody << comments.str();
    this->outputHtmlBodyNoTag=outBody.str();
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
  bool headFinished=!this->flagTagHeadPresent;
  std::string tagClass;
  for (int i=0; i<this->theContent.size; i++)
    if (!this->theContent[i].IsHidden())
    { tagClass=this->theContent[i].GetTagClass();
      if (tagClass=="headFinish")
      { headFinished=true;
        outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        continue;
      }
      if (tagClass=="bodyStart" || tagClass=="bodyFinish" || tagClass=="headStart" || tagClass=="headFinish" ||
          tagClass=="htmlStart" || tagClass=="htmlFinish")
      { if (headFinished)
          outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        else
          outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        continue;
      }
      if (headFinished)
        outBody << this->theContent[i].ToStringInterpretedBody();
      else
        outHeadPt2 << this->theContent[i].ToStringInterpretedBody();
    }
  outHeadPt2 << this->GetJavascriptMathQuillBoxes();
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
    this->currentUseR.SetProblemData(this->fileName, this->theProblemData);
    if (!this->currentUseR.StoreProblemDataToDatabase(theRoutines, comments))
      outBody << "<b>Error: failed to store problem in database. </b>" << comments.str();
  }
  //out << "Current collection problems: " << this->databaseProblemList.ToStringCommaDelimited()
  //<< " with weights: " << this->databaseProblemWeights.ToStringCommaDelimited();
#endif // MACRO_use_MySQL
  this->outputHtmlNavigatioN=this->ToStringProblemNavigation();
  this->outputHtmlBodyNoTag=outBody.str();
  outHeaD << outHeadPt1.str() << outHeadPt2.str();
  this->outputHtmlHeadNoTag=outHeaD.str();
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
    out << "<b>Guest mode</b>" << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=login\">Log in</a> ";
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  std::string calcArgsNoPassExamDetails=
  theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (theGlobalVariables.UserDefaultHasAdminRights())
  { if (theGlobalVariables.UserStudentViewOn())
      out << "<a href=\"" << theGlobalVariables.userCalculatorRequestType << "?"
      << this->ToStringCalculatorArgumentsForProblem
      (theGlobalVariables.userCalculatorRequestType, "false", theGlobalVariables.GetWebInput("studentSection"))
      << "\">Admin view</a>" << linkSeparator;
    else
    { if (this->databaseStudentSectionS.size==0)
        out << "<a href=\"" << theGlobalVariables.userCalculatorRequestType << "?"
        << this->ToStringCalculatorArgumentsForProblem
        (theGlobalVariables.userCalculatorRequestType, "true", "")
        << "\">Student view</a>" << linkSeparator;
      for (int i=0; i<this->databaseStudentSectionS.size; i++)
        if (this->databaseStudentSectionS[i]!="")
          out << "<a href=\"" << theGlobalVariables.userCalculatorRequestType << "?"
          << this->ToStringCalculatorArgumentsForProblem
          (theGlobalVariables.userCalculatorRequestType, "true", this->databaseStudentSectionS[i])
          << "\">Student view section " << this->databaseStudentSectionS[i] << " </a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem)
  { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=" << exerciseRequest << "&" << calcArgsNoPassExamDetails
    << "studentView=" << studentView << "&";
    if (theGlobalVariables.GetWebInput("studentSection")!="")
      out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
    out << "topicList=" << CGI::StringToURLString(this->topicListFileName) << "&";
    out << "currentExamHome=" << CGI::StringToURLString(this->currentExamHome) << "&";
    out << "fileName=" << CGI::StringToURLString(this->currentExamHome) << "&\"> Home </a>"
    << linkSeparator;
  }
  if (this->flagIsExamHome)
    out << "<b>Course homework home</b>";
  if (this->flagIsExamProblem)
  { if (theGlobalVariables.userCalculatorRequestType=="exercise")
      out << "<a href=\"scoredQuiz?"
      << this->ToStringCalculatorArgumentsForProblem("scoredQuiz", studentView)
      << "\">" << this->stringScoredQuizzes << "</a>" << linkSeparator;
    else if (theGlobalVariables.userCalculatorRequestType=="scoredQuiz")
      out << "<a href=\"exercise?"
      << this->ToStringCalculatorArgumentsForProblem("exercise", studentView)
      << "\">" << this->stringPractice << "</a>" << linkSeparator;
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated)
  { int indexInParent=this->problemListOfParent.GetIndex(this->fileName);
    if (indexInParent==-1)
    { //out << "<b>Could not find the problem collection that contains this problem.</b>";
//      out << "<b>This is either a faulty link or a programming error. </b> "
      //out << "this->filename is: " << this->fileName << " and the problem list of the parent is: "
      //<< this->problemListOfParent.ToStringCommaDelimited();
    } else
    { if (indexInParent>0)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << CGI::StringToURLString(this->topicListFileName) << "&";
        out << "currentExamHome=" << CGI::StringToURLString(this->currentExamHome) << "&";
        out << "fileName=" << CGI::StringToURLString(this->problemListOfParent[indexInParent-1])
        << "\"> <-Previous </a>" << linkSeparator;
      }
      if (indexInParent<this->problemListOfParent.size-1)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << CGI::StringToURLString(this->topicListFileName) << "&";
        out << "currentExamHome=" << CGI::StringToURLString(this->currentExamHome) << "&";
        out << "fileName=" << CGI::StringToURLString(this->problemListOfParent[indexInParent+1] )
        << "\"> Next-> </a>" << linkSeparator;
      } else
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=template";
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << CGI::StringToURLString(this->topicListFileName) << "&";
        out << "currentExamHome=" << CGI::StringToURLString(this->currentExamHome) << "&";
        out << "fileName=" << CGI::StringToURLString(this->currentExamHome)
        << "\">Last problem, return to course page.</a>" << linkSeparator;
      }
    }
  }
  if (this->flagIsExamProblem &&
      (theGlobalVariables.userCalculatorRequestType=="exercise" ||
       theGlobalVariables.userCalculatorRequestType=="exerciseNoLogin"))
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=" << theGlobalVariables.userCalculatorRequestType << "&"
    << this->ToStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">" << this->stringProblemLink << "</a>";
//  out << linkBigSeparator << theGlobalVariables.ToStringNavigation();
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem
(const std::string& requestType, const std::string& studentView,
 const std::string& studentSection, bool includeRandomSeedIfAppropriate)const
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsForProblem");
  if (!theGlobalVariables.flagLoggedIn && !theGlobalVariables.UserGuestMode() )
    return "";
  std::stringstream out;
  out << "request=" << requestType << "&";
  List<std::string> excludedTags;
  excludedTags.AddOnTop("randomSeed");
  out << theGlobalVariables.ToStringCalcArgsNoNavigation(&excludedTags)
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&";
  if  (!theGlobalVariables.flagRunningApache && this->fileName!="")
    out << "fileName=" << CGI::StringToURLString(this->fileName) << "&";
  else
    out << "fileName=" << CGI::StringToURLString(theGlobalVariables.GetWebInput("fileName") )
    << "&";
  if (!theGlobalVariables.UserGuestMode())
  { out << "studentView=" << studentView << "&";
    if (studentSection!="")
      out << "studentSection=" << CGI::StringToURLString(studentSection) << "&";
  }
  if (includeRandomSeedIfAppropriate)
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
//  out << "fileName=" << CGI::StringToURLString(this->fileName) << "&";
  return out.str();
}

std::string CalculatorHTML::GetEditPageButton()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetEditPageButton");
  std::stringstream out;
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=editPage&";
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
  << "  inputParams='request='+requestType+'&';\n"
  << "  inputParams+='" << theGlobalVariables.ToStringCalcArgsNoNavigation() << "';\n"
  << "  inputParams+='&fileName=" << CGI::StringToURLString(this->fileName) << "';\n"
  << "  inputParams+='&topicList=" << CGI::StringToURLString(this->topicListFileName) << "';\n"
  << "  inputParams+='&currentExamHome=" << CGI::StringToURLString(this->currentExamHome) << "';\n"
  << "  inputParams+='&mainInput=' + encodeURIComponent(theString);\n"
  << "  var https = new XMLHttpRequest();\n"
  ////////////////////////////////////////////
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameExecutable << "\", true);\n"
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
    out << " <a href=\"" << refStreamForReal.str() << "\">" << CalculatorHTML::stringScoredQuizzes << "</a> ";
  out << " <a href=\"" << refStreamExercise.str() << "\">" << CalculatorHTML::stringPractice << "</a> ";
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
  if (this->currentUseR.problemNames.Contains(cleaneduplink))
  { ProblemData& theProbData=this->currentUseR.problemData[this->currentUseR.problemNames.GetIndex(cleaneduplink)];
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

struct TopicElement{
public:
  bool flagIsChapter;
  bool flagIsSection;
  bool flagIsSubSection;
  bool flagIsError;
  std::string title;
  std::string video;
  std::string slides;
  std::string slidesPrintable;
  std::string problem;
  std::string error;
  void reset()
  { this->flagIsSection=false;
    this->flagIsSubSection=false;
    this->flagIsChapter=false;
    this->flagIsError=false;
    this->title="empty";
    this->video="";
    this->slides="";
    this->slidesPrintable="";
    this->problem="";
    this->error="";
  }
  static std::string GetTableStart();
  static std::string GetTableFinish();
  TopicElement()
  { this->reset();
  }
  static void GetTopicList(const std::string& inputString, List<TopicElement>& output);
};

void TopicElement::GetTopicList(const std::string& inputString, List<TopicElement>& output)
{ MacroRegisterFunctionWithName("TopicElement::GetTopicList");
  std::stringstream tableReader(inputString);
  std::string currentLine, currentArgument;
  TopicElement currentElt;
  while(std::getline(tableReader, currentLine, '\n'))
  { if (MathRoutines::StringTrimWhiteSpace(currentLine)=="")
      continue;
    if (MathRoutines::StringBeginsWith(currentLine, "Chapter:", &currentArgument))
    { if (currentElt.title!="empty")
        output.AddOnTop(currentElt);
      currentElt.reset();
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.flagIsChapter=true;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Section:", &currentArgument))
    { if (currentElt.title!="empty")
        output.AddOnTop(currentElt);
      currentElt.reset();
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.flagIsSection=true;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Topic:", &currentArgument))
    { if (currentElt.title!="empty")
        output.AddOnTop(currentElt);
      currentElt.reset();
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.flagIsSubSection=true;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Title:", &currentArgument))
    { if (currentElt.title!="empty")
        output.AddOnTop(currentElt);
      currentElt.reset();
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
    } else if (MathRoutines::StringBeginsWith(currentLine, "Video:", &currentArgument))
      currentElt.video=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "Slides:", &currentArgument))
      currentElt.slides=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "PrintableSlides:", &currentArgument))
      currentElt.slidesPrintable=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "Problem:", &currentArgument))
      currentElt.problem=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else
    { currentElt.error+="Unrecognized entry: " + currentLine + ". ";
      currentElt.flagIsError=true;
    }
  }
  output.AddOnTop(currentElt);
}

void CalculatorHTML::InterpretTableOfContents(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretTableOfContents");
  std::stringstream out;
  if (this->topicListContent=="")
    if (!FileOperations::LoadFileToStringVirtual(this->topicListFileName, this->topicListContent, out))
    { inputOutput.interpretedCommand=out.str();
      return;
    }
  List<TopicElement> theTopics;
  TopicElement::GetTopicList(this->topicListContent, theTopics);
  TopicElement currentElt;
  bool sectionStarted=false;
  bool subSectionStarted=false;
  bool chapterStarted=false;
  out << "<!--Topic list automatically generated from topic list: " << this->topicListFileName
  << ".-->";
  out << "<ol>";
  for (int i=0; i<theTopics.size; i++)
  { currentElt=theTopics[i];
    if (subSectionStarted)
      if (currentElt.flagIsSubSection || currentElt.flagIsSection || currentElt.flagIsChapter)
        out << "</li>";
    if (sectionStarted)
      if (currentElt.flagIsSection || currentElt.flagIsChapter)
        out << "</ol></li>";
    if (chapterStarted)
      if (currentElt.flagIsChapter)
        out << "</ol></li>";
    if (currentElt.flagIsChapter)
    { out << "<li>" << "<a href=\"" << "template?fileName=" << this->fileName << "&"
      << "topicList=" << this->topicListFileName << "&" << "chapter=" << currentElt.title
      << "\">" << currentElt.title << "</a>" << "<br>\n";
      chapterStarted=true;
      sectionStarted=false;
      subSectionStarted=false;
      out << "<ol>";
    } else if (currentElt.flagIsSection)
    { out << "<li>" << currentElt.title << "<br>\n";
      sectionStarted=true;
      subSectionStarted=false;
      out << "<ol>";
    } else if (currentElt.flagIsError)
      out << "Error parsing topic list. Could not make sense of: " << currentElt.error << ". "
      << "The allowed data labels are CASE SENSITIVE: "
      << "<br>Chapter<br>Section<br>Topic<br>Title<br>Video<br>Problem<br>Slides<br>PrintableSlides<br>"
      << "You need to include columns immediately after the data labels. The data entries are terminated by new line. "
      << " Here is a correctly entered example:"
      << "Title: What is a logarithmic derivative?<br>\n"
      << "Video: modules/substitution-rule/videos/integral-derivative-f-over-f-intro.html<br>\n"
      << "Slides: modules/substitution-rule/pdf/integral-derivative-f-over-f-intro.pdf<br>\n"
      << "PrintableSlides: modules/substitution-rule/pdf/printable-integral-derivative-f-over-f-intro.pdf<br>\n"
      << "\n";
  }
  out << "</ol>";
  inputOutput.interpretedCommand=out.str();
}

void CalculatorHTML::InterpretTopicList(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretTopicList");
  std::stringstream out;
  if (this->topicListContent=="")
    if (!FileOperations::LoadFileToStringVirtual(this->topicListFileName, this->topicListContent, out))
    { inputOutput.interpretedCommand=out.str();
      return;
    }
  List<TopicElement> theTopics;
  TopicElement::GetTopicList(this->topicListContent, theTopics);
  TopicElement currentElt;
  bool tableStarted=false;
  bool sectionStarted=false;
  bool subSectionStarted=false;
  bool chapterStarted=false;
  std::string desiredChapter= CGI::URLStringToNormal(theGlobalVariables.GetWebInput("chapter"));
  std::string currentChapter="";
  out << "<!--Topic list automatically generated from topic list: " << this->topicListFileName
  << ".-->";
  //out << "<br>DEBUG: Desired chapter: " << desiredChapter;
  bool firstListStarted=false;
  int chapterCounter=0;
  //CalculatorHTML currentProblem;
  for (int i=0; i<theTopics.size; i++)
  { currentElt=theTopics[i];
    if (currentElt.flagIsChapter)
    { currentChapter=currentElt.title;
      chapterCounter++;
    }
    if (desiredChapter!="")
      if (currentChapter!=desiredChapter)
      { //out << "<br>DEBUG: Chapter: " << currentChapter << " skipped. ";
        continue;
      }
    if (!firstListStarted)
    { firstListStarted=true;
      out << "\n<style>ol{counter-reset: item " << chapterCounter-1 << "}" << "</style>\n";
      out << "<ol start=\"" << chapterCounter << "\">";
    }
    if (!currentElt.flagIsSection && !currentElt.flagIsSubSection && !currentElt.flagIsChapter && !tableStarted)
    { out << TopicElement::GetTableStart();
      tableStarted=true;
    }
    if ((currentElt.flagIsSection || currentElt.flagIsSubSection || currentElt.flagIsChapter || currentElt.flagIsError)
        && tableStarted)
    { out << TopicElement::GetTableFinish();
      tableStarted=false;
    }
    if (subSectionStarted)
      if (currentElt.flagIsSubSection || currentElt.flagIsSection || currentElt.flagIsChapter)
        out << "</li>\n";
    if (sectionStarted)
      if (currentElt.flagIsSection || currentElt.flagIsChapter)
        out << "</ol></li>\n";
    if (chapterStarted)
      if (currentElt.flagIsChapter)
        out << "</ol></li>\n";
    if (currentElt.flagIsChapter)
    { out << "<li>\n" << currentElt.title << "<br>\n";
      chapterStarted=true;
      sectionStarted=false;
      subSectionStarted=false;
      tableStarted=false;
      out << "<ol>\n";
    } else if (currentElt.flagIsSection)
    { out << "<li>\n" << currentElt.title << "<br>\n";
      sectionStarted=true;
      subSectionStarted=false;
      tableStarted=false;
      out << "<ol>\n";
    } else if (currentElt.flagIsSubSection)
    { out << "<li>\n";
      out << currentElt.title << "\n<br>\n";
      subSectionStarted=true;
      tableStarted=false;
    } else if (currentElt.flagIsError)
      out << "Error parsing topic list. Could not make sense of: " << currentElt.error << ". "
      << "The allowed data labels are [CASE SENSITIVE!]: "
      << "<br>Section<br>Topic<br>Title<br>Video<br>Problem<br>Slides<br>PrintableSlides<br>"
      << "You need to include columns immediately after the data labels. The data entries are terminated by new line. "
      << " Here is a correctly entered example:"
      << "Title: What is a logarithmic derivative?<br>\n"
      << "Video: modules/substitution-rule/videos/integral-derivative-f-over-f-intro.html<br>\n"
      << "Slides: modules/substitution-rule/pdf/integral-derivative-f-over-f-intro.pdf<br>\n"
      << "PrintableSlides: modules/substitution-rule/pdf/printable-integral-derivative-f-over-f-intro.pdf<br>\n"
      << "\n";
    else
    { out << "<tr>\n";
      out << "  <td>\n";
      if (currentElt.title=="")
        out << "    -\n";
      else
        out << "    " << currentElt.title << "\n";
      out << "  </td>\n";
      out << "  <td>\n";
      if (currentElt.video=="")
        out << "    Lesson coming soon";
      else if (currentElt.video=="-")
        out << "    -";
      else
        out << "    <a href=\"#\" onclick=\"window.open('"
        << currentElt.video << "',  'width=300', 'height=250', 'top=400'); return false;\">Go to lesson</a>";
      if (currentElt.slides!="")
        out << " | <a href=\"" << currentElt.slides << "\">Slides</a>";
      if (currentElt.slidesPrintable!="")
        out << " | <a href=\"" << currentElt.slidesPrintable << "\">Printable slides</a>";
      if (currentElt.problem!="")
      { if (theGlobalVariables.flagRunningAce)
        { out << " | <a href=\"#\" onclick=\"window.open('"
          << theGlobalVariables.DisplayNameExecutable << "?request=scoredQuiz&fileName="
          << currentElt.problem << "',  'width=300', 'height=250', 'top=400'); return false;\">Scored Quizzes</a>";
          out << " | <a href=\"#\" onclick=\"window.open('"
          << theGlobalVariables.DisplayNameExecutable << "?request=exercise&fileName="
          << currentElt.problem << "',  'width=300', 'height=250', 'top=400'); return false;\">Practice</a>";
        } else
        { //currentProblem.fileName=currentElt.problem;
          //currentProblem.currentExamHomE=this->fileName;
          out << this->ToStringLinkFromFileName(currentElt.problem);
        }
      }
      out << "  </td>\n";
      out << "  <td>";
      if (currentElt.problem=="")
        out << "<FONT COLOR=\"brown\">-</FONT>";
      else
        out << "<FONT COLOR=\"green\">10/10</FONT>";
      out << "</td>\n";
      out << "</tr>\n";
    }
  }
  if (tableStarted)
    out << TopicElement::GetTableFinish();
  out << "</ol>";
  tableStarted=false;
  inputOutput.interpretedCommand=out.str();
}

std::string TopicElement::GetTableStart()
{ std::stringstream out;
  out
  << "\n\n<table class=\"topicList\"><tr> <th style=\"width:400px\">Sub-Topic</th>"
  << "<th style=\"width:400px\">Resource Links</th>"
  << "<th style=\"width:150px\">Current Score</th></tr>\n";
  return out.str();
}

std::string TopicElement::GetTableFinish()
{ std::stringstream out;
  out << "</table>\n\n";
  return out.str();
}
