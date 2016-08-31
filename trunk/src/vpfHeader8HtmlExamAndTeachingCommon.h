//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader8htmlExamAndTeachingRoutinesCommon_h
#define vpfHeader8htmlExamAndTeachingRoutinesCommon_h

#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader1General5TimeDate.h"
#include "vpfHeader8HtmlInterpretation.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCommonCode
(__FILE__, "Routines for calculus teaching: calculator exam mode. Shared code. ");

CalculatorHTML::CalculatorHTML()
{ this->flagRandomSeedGiven=false;
  this->NumAttemptsToInterpret=0;
  this->NumAnswerIdsMathquilled=0;
  this->MaxInterpretationAttempts=10;
  this->flagLoadedSuccessfully=false;
  this->flagIsExamHome=false;
  this->flagIsExamProblem=false;
  this->flagParentInvestigated=false;
  this->NumProblemsFound=0;
  this->flagIsForReal=false;
  this->flagLoadedFromDB=false;
  this->flagLoadedClassDataSuccessfully=false;
  this->flagTagHeadPresent=false;
  this->flagTagHtmlPresent=false;
  this->flagTagBodyPresent=false;
  this->flagUseNavigationBar=true;
  this->timeToParseHtml=0;
  this->flagMathQuillWithMatrices=false;
}

#ifdef MACRO_use_MySQL
bool DatabaseRoutines::ReadProblemInfo
  (const std::string& stringToReadFrom, HashedList<std::string, MathRoutines::hashString>& outputProblemNames,
//   List<std::string>& outputHomeworkGroups,
   List<std::string>& outputWeights,
   List<List<std::string> >& outputSections, List<List<std::string> >& outputDeadlinesPerSection,
   std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ReadProblemInfo");
  MapList<std::string, std::string, MathRoutines::hashString> CGIedProbs, currentProblem, sectionInfo;
  if (!CGI::ChopCGIString
      (stringToReadFrom, CGIedProbs, commentsOnFailure) )
    return false;
  outputProblemNames.Clear();
  outputProblemNames.SetExpectedSize(CGIedProbs.size());
  for (int i=0; i<CGIedProbs.size(); i++)
    outputProblemNames.AddOnTop(CGI::URLStringToNormal(CGIedProbs.theKeys[i]));
//  outputHomeworkGroups.initFillInObject(outputProblemNames.size, "");
  outputWeights.initFillInObject(outputProblemNames.size, "");
  outputSections.initFillInObject(outputProblemNames.size, CGIedProbs.size());
  outputDeadlinesPerSection.initFillInObject(outputProblemNames.size, CGIedProbs.size());
  for (int i=0; i<outputProblemNames.size; i++)
  { if (!CGI::ChopCGIString
        (CGI::URLStringToNormal(CGIedProbs.theValues[i]), currentProblem, commentsOnFailure))
      return false;
    //    stOutput << "<br>DEBUG: current problem: " << outputProblemNames[i]
    //    << "<br>being read from data: " << CGIedProbs[i]
    //    << "<br>which is normalized to: " << CGI::URLStringToNormal(CGIedProbs[i]);
    if (currentProblem.Contains("weight"))
      outputWeights[i]=
      CGI::URLStringToNormal(currentProblem.GetValueCreateIfNotPresent("weight"));
    if (!currentProblem.Contains("deadlines"))
      continue;
    std::string deadlineString=CGI::URLStringToNormal(currentProblem.GetValueCreateIfNotPresent("deadlines"));
    //stOutput << "<hr><hr>DEBUG: deadline string: " << deadlineString;
    if (!CGI::ChopCGIString(deadlineString, sectionInfo, commentsOnFailure))
      return false;
    //stOutput << "<hr><hr>DEBUG: sectionInfo: " << sectionInfo.ToStringHtml();
    for (int j=0; j<sectionInfo.size(); j++)
    { outputSections[i].AddOnTop(CGI::URLStringToNormal(sectionInfo.theKeys[j]));
      outputDeadlinesPerSection[i].AddOnTop(CGI::URLStringToNormal(sectionInfo.theValues[j]));
    }
  }
  //stOutput << "reading from: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(stringToReadFrom);
  //stOutput << "<hr><hr>DEBUG: final outputSections: " << outputSections
  //<< "<br>outputDeadlinesPerSection: " << outputDeadlinesPerSection;
  return true;
}

void DatabaseRoutines::StoreProblemInfo
  (std::string& outputString, const HashedList<std::string, MathRoutines::hashString>& inputProblemNames,
   const List<std::string>& inputWeights, const List<List<std::string> >& inputSections,
   const List<List<std::string> >& inputDeadlines)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemInfo");
  if (inputProblemNames.size!=inputWeights.size ||
      inputProblemNames.size!=inputDeadlines.size ||
      inputProblemNames.size!=inputSections.size
      )
    crash << "This shouldn't happen: non-matching data sizes while storing problem info. "
    << "The present function should only be called with sanitized input. " << crash;
  std::stringstream out;
  for (int i=0; i<inputProblemNames.size; i++)
  { std::stringstream currentProblemStream, currentDeadlineStream;
    if (inputWeights[i]!="")
      currentProblemStream << "weight=" << CGI::StringToURLString(inputWeights[i]) << "&";
    if (inputSections[i].size!=inputDeadlines[i].size)
      crash << "Input sections and input deadlines have mismatching sizes. " << crash;
    for (int j=0; j<inputSections[i].size; j++)
      currentDeadlineStream << CGI::StringToURLString(inputSections[i][j])
      << "=" << CGI::StringToURLString(inputDeadlines[i][j]) << "&";
    currentProblemStream << "deadlines=" << CGI::StringToURLString(currentDeadlineStream.str()) << "&";
    out << CGI::StringToURLString(inputProblemNames[i]) << "="
    << CGI::StringToURLString(currentProblemStream.str()) << "&";
  }

//  stOutput << "Storing prob string: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(out.str())
//  << "<br>nput sections: " << inputSections << "<br>";
  outputString=out.str();
}

bool DatabaseRoutines::ReadProblemDatabaseInfo
(const std::string& problemHomeName, std::string& outputString,
  std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ReadProblemDatabaseInfo");
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
    return false;
  if (!this->TableExists("problemData", &commentsOnFailure))
    if (!this->CreateTable("problemData", "problemCollection VARCHAR(255) NOT NULL PRIMARY KEY, \
        problemInformation LONGTEXT", &commentsOnFailure, 0))
      return false;
  if (!this->RowExists
      ((std::string) "problemCollection", problemHomeName,
       (std::string) "problemData", &commentsOnFailure))
    if (!this->SetEntry
        ((std::string) "problemCollection", problemHomeName,
         (std::string) "problemData", (std::string) "problemInformation",
         (std::string) "", &commentsOnFailure))
      return false;
//  stOutput << " ... got to here ...";
  return this->FetchEntry
  ((std::string) "problemCollection", problemHomeName, (std::string) "problemData",
   (std::string) "problemInformation", outputString, &commentsOnFailure);
}

bool DatabaseRoutines::StoreProblemDatabaseInfo
  (const std::string& problemHomeName, const std::string& inputString,
   std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
    return false;
  if (!this->TableExists("problemData", &commentsOnFailure))
    if (!this->CreateTable("problemData", "problemCollection VARCHAR(255) NOT NULL PRIMARY KEY, \
        problemInformation LONGTEXT", &commentsOnFailure, 0))
      return false;
  if (!this->RowExists
      ((std::string) "problemCollection", problemHomeName,
       (std::string) "problemData", &commentsOnFailure))
    if (!this->SetEntry
        ((std::string) "problemCollection", problemHomeName,
         (std::string) "problemData", (std::string) "problemInformation",
         (std::string) "", &commentsOnFailure))
      return false;
  return this->SetEntry
  ((std::string) "problemCollection", problemHomeName, (std::string) "problemData",
   (std::string) "problemInformation", inputString, &commentsOnFailure);
}

bool DatabaseRoutines::MergeProblemInfoInDatabase
(const std::string& problemHomeName, std::string& inputString,
  std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::MergeProblemInfoInDatabase");
  CalculatorHTML problemHome;
  problemHome.fileName=problemHomeName;
  if (!problemHome.LoadMe(true, commentsOnFailure))
    return false;
  if (!problemHome.ParseHTML(commentsOnFailure))
    return false;
  HashedList<std::string, MathRoutines::hashString> incomingProblems;
  List<std::string> incomingWeights;
  List<List<std::string> > incomingSections;
  List<List<std::string> > incomingDeadlines;
//  stOutput << "<hr>DEBUG: incoming string: " << inputString
//  << ", human readable: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(inputString);
  if (!this->ReadProblemInfo
      (inputString, incomingProblems, incomingWeights,
       incomingSections, incomingDeadlines, commentsOnFailure))
  { commentsOnFailure << "Failed to parse your request";
    return false;
  }
//  stOutput << "<br>DEBUG: incoming sections: " << incomingSections.ToStringCommaDelimited() << "<hr>";
//  stOutput << "<br>DEBUG: incoming deadlines: " << incomingDeadlines.ToStringCommaDelimited() << "<hr>";

  std::string currentFileName;
  bool result=true;
  for (int i=0; i<incomingProblems.size; i++)
  { if (!problemHome.hdProblemList.Contains(incomingProblems[i]) &&
        !problemHome.hdHomeworkGroupNames.Contains(incomingProblems[i]))
    { commentsOnFailure << "Did not find " << incomingProblems[i]
      << " among the list of problems and homework groups. ";
//      << " The problem list consists of: "
//      << problemHome.hdProblemList.ToStringCommaDelimited() << ". The homework group names are: "
//      << problemHome.hdHomeworkGroupNames.ToStringCommaDelimited();
      result=false;
      continue;
    }
    int theIndex=problemHome.databaseProblemAndHomeworkGroupList.GetIndex(incomingProblems[i]);
    if (theIndex==-1)
    { problemHome.databaseProblemAndHomeworkGroupList.AddOnTop(incomingProblems[i]);
      problemHome.databaseProblemWeights.AddOnTop(incomingWeights[i]);
      problemHome.databaseStudentSectionsPerProblem.AddOnTop(incomingSections[i]);
      problemHome.databaseDeadlinesBySection.AddOnTop(incomingDeadlines[i]);
      continue;
    }
//    stOutput << "<br>DEBUG: modifying problem info for problem: " << incomingProblems[i];
    if (incomingWeights[i]!="")
      problemHome.databaseProblemWeights[theIndex]=incomingWeights[i];
    if (incomingSections[i].size!=0)
      for (int j=0; j<incomingSections[i].size; j++)
      { int sectionIndex=problemHome.databaseStudentSectionsPerProblem[theIndex].GetIndex(incomingSections[i][j]);
        if (sectionIndex==-1)
        { problemHome.databaseStudentSectionsPerProblem[theIndex].AddOnTop(incomingSections[i][j]);
          problemHome.databaseDeadlinesBySection[theIndex].AddOnTop(incomingDeadlines[i][j]);
          continue;
        }
        //stOutput << "<br>DEBUG: modifying problem info for section: " << incomingSections[i][j];
        problemHome.databaseStudentSectionsPerProblem[theIndex][sectionIndex]=incomingSections[i][j];
        problemHome.databaseDeadlinesBySection[theIndex][sectionIndex]=incomingDeadlines[i][j];
      }
  }
  std::string stringToStore;

  this->StoreProblemInfo
  (stringToStore, problemHome.databaseProblemAndHomeworkGroupList, problemHome.databaseProblemWeights,
   problemHome.databaseStudentSectionsPerProblem, problemHome.databaseDeadlinesBySection);
  //stOutput << "<br>about to store back : <br>" << stringToStore << " interpreted as: <br>"
  //<< CGI::URLKeyValuePairsToNormalRecursiveHtml(stringToStore) ;
  if (!this->StoreProblemDatabaseInfo(problemHomeName, stringToStore, commentsOnFailure))
    return false;
  //stOutput << "<br>probs incoming: <br>" << incomingProblems.ToStringCommaDelimited()
  //<< " with weights: " << incomingWeights.ToStringCommaDelimited()
  //<< "Problems final: " <<  problemHome.databaseProblemAndHomeworkGroupList.ToStringCommaDelimited()
  //<< " weights: " << problemHome.databaseProblemWeights.ToStringCommaDelimited() << "<br>";
  return result;
}
#endif // MACRO_use_MySQL

bool CalculatorHTML::LoadDatabaseInfo(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadDatabaseInfo");
#ifdef MACRO_use_MySQL
  this->currentUseR.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  if (this->currentUseR.problemDataString=="")
  { comments << "Failed to load current user's problem save-file. ";
    return false;
  }
  if (!this->currentUseR.InterpretDatabaseProblemData(this->currentUseR.problemDataString.value, comments))
  { comments << "Failed to interpret user's problem save-file. ";
    return false;
  }
  this->theProblemData=this->currentUseR.GetProblemDataAddIfNotPresent(this->fileName);
  //if (this->currentExamHomE=="")
  //  return true;
  //stOutput << "loading db, problem collection: " << this->currentExamHomE;
  int toDoFixDeadlines;
/*  this->currentUser.currentTable=theRoutines.GetTableUnsafeNameUsersOfFile(this->currentExamHomE);
  //stOutput << "loading extra info ... " << this->currentExamHomE;
  if(!this->currentUser.FetchOneColumn("extraInfo", this->currentUser.extraInfoUnsafe, theRoutines, &comments))
  { comments << "Failed to load the section/group of the current user. ";
    //stOutput << "Failed to load the section/group of the current user. ";
    //return false;
  }
  if (!theRoutines.ReadProblemDatabaseInfo
      (this->currentExamHomE, this->currentProblemCollectionDatabaseString, comments))
  { comments << "Failed to load current problem collection's database string. ";
    return false;
  }
  //stOutput << "<hr><hr>DEBUG GOT HERE:  the db string is: " << CGI::URLKeyValuePairsToNormalRecursiveHtml( this->currentProblemCollectionDatabaseString);
  if (!theRoutines.ReadProblemInfo
      (this->currentProblemCollectionDatabaseString, this->databaseProblemAndHomeworkGroupList,
       this->databaseProblemWeights,
       this->databaseStudentSectionsPerProblem, this->databaseDeadlinesBySection, comments))
  { comments << "Failed to interpret the database problem string. ";
    return false;
  }
  //stOutput << "<hr><hr>DEBUG read databaseProblemAndHomeworkGroupList: "
  //<< this->databaseProblemAndHomeworkGroupList;
  this->currentUser.ComputePointsEarned
  (this->databaseProblemAndHomeworkGroupList, this->databaseProblemWeights);*/
  theGlobalVariables.userDefault=this->currentUseR;
  return true;
#else
  comments << "Database not available. ";
  return false;
#endif
}

bool CalculatorHTML::LoadMe(bool doLoadDatabase, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadMe");
  this->RelativePhysicalFileNameWithFolder=
  this->fileName
  ;
  (void) doLoadDatabase;
  std::ifstream theFile;
  if (!FileOperations::OpenFileVirtualReadOnly(theFile, this->RelativePhysicalFileNameWithFolder, false))
  { std::string theFileName;
    FileOperations::GetPhysicalFileNameFromVirtual(this->RelativePhysicalFileNameWithFolder, theFileName);
    comments << "<b>Failed to open:<br>\n"
    << this->RelativePhysicalFileNameWithFolder << "<br>computed file name: <br>"
    << theFileName
    << "</b> ";
    //comments << "Call stack: " << crash.GetStackTraceEtcErrorMessage();
    return false;
  }
  std::stringstream contentStream;
  contentStream << theFile.rdbuf();
  this->inputHtml=contentStream.str();
  this->flagIsForReal=theGlobalVariables.UserRequestRequiresLoadingRealExamData();
#ifdef MACRO_use_MySQL
  this->topicListFileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
//  if (doLoadDatabase)
    this->LoadDatabaseInfo(comments);
#endif // MACRO_use_MySQL
  if (!this->flagIsForReal)
  { std::string randString= theGlobalVariables.GetWebInput("randomSeed");
    if (randString!="")
    { std::stringstream randSeedStream(randString);
      //stOutput << "radSeedStream: " << randString;
      randSeedStream >> this->theProblemData.randomSeed;
      //stOutput << ", interpreted as: " << this->theProblemData.randomSeed;
      this->flagRandomSeedGiven=true;
    }
  }
//  stOutput << "debug info: rand seed after all's been said and done: " << this->theProblemData.randomSeed;
  return true;
}

bool CalculatorHTML::FindExamItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::FindExamItem");
  List<std::string> theExerciseFileNames, theExerciseFileNameExtensions;
  if (!FileOperations::GetFolderFileNamesVirtual
      ("ProblemCollections/", theExerciseFileNames, &theExerciseFileNameExtensions))
  { this->comments << "Failed to open folder with relative file name: "
    << "ProblemCollections/";
    return false;
  }
  for (int i=0; i<theExerciseFileNames.size; i++)
  { if (theExerciseFileNameExtensions[i]!=".html")
      continue;
    this->fileName="ProblemCollections/"+ theExerciseFileNames[i];
    return true;
  }
  return false;
}

std::string CalculatorHTML::LoadAndInterpretCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetCurrentExamItem");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  this->LoadCurrentProblemItem();
  if (!this->flagLoadedSuccessfully)
    return this->comments.str();
  std::stringstream out;
  if (!this->InterpretHtml(this->comments))
  { if (theGlobalVariables.UserDefaultHasAdminRights())
      out << this->GetEditPageButton();
    out << "<b>Failed to interpret file: " << this->fileName << "</b>. Comments: " << this->comments.str();
    return out.str();
  }
  //out << "DEBUG: flagMathQuillWithMatrices=" << this->flagMathQuillWithMatrices << "<br>";
  if (this->flagUseNavigationBar && !theGlobalVariables.flagRunningApache)
  { std::string linkSeparator=" | ";
    std::string linkBigSeparator=" || ";
    out << "<nav>"
    << this->outputHtmlNavigatioN
    << linkBigSeparator << "<small>Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds()-startTime)
    << " second(s).</small>" << "</nav> ";
   }
  out << this->outputHtmlBodyNoTag;
  return out.str();
}

void CalculatorHTML::LoadFileNames()
{ this->fileName = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"));
  this->currentExamHome = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  this->topicListFileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
}

void CalculatorHTML::LoadCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->LoadFileNames();
  this->flagLoadedSuccessfully=false;
  bool needToFindDefault=(this->fileName=="");
  bool needToLoadDatabase=true;
  if (theGlobalVariables.UserGuestMode())
    needToLoadDatabase=false;
  this->flagUseNavigationBar=(theGlobalVariables.GetWebInput("navigationBar")=="true");
  if (!needToFindDefault)
    needToFindDefault=!this->LoadMe(needToLoadDatabase, this->comments);
  else
    this->comments << "<b>Selecting default course homework file. </b><br>";
  if (needToFindDefault)
  { needToLoadDatabase=false;
    if (!this->FindExamItem())
    { this->comments << "<b>No problems/exams to serve: found no html content in folder: "
      << "ProblemCollections/" << ". </b>";
      return;
    }
    if (!this->LoadMe(needToLoadDatabase, this->comments))
      return;
    this->inputHtml=this->comments.str()+this->inputHtml;
  }
  this->flagLoadedSuccessfully=true;
}

bool CalculatorHTML::IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt)
{ MacroRegisterFunctionWithName("CalculatorHTML::IsStateModifierApplyIfYes");
  if (inputElt.syntacticRole!="command")
    return false;
  std::string tagClass=inputElt.GetKeyValue("class");
  if (tagClass=="setCalculatorExamHome")
  { this->flagIsExamHome=true;
    this->flagIsExamProblem=false;
    theGlobalVariables.SetWebInpuT("currentExamHome", CGI::StringToURLString(this->fileName));
  }
  if (tagClass=="setCalculatorExamProblem")
  { this->flagIsExamHome=false;
    this->flagIsExamProblem=true;
    return true;
  }
  return false;
}

std::string CalculatorHTML::GetDatePickerStart(const std::string& theId)
{ std::stringstream out;
  out << "\n<script type=\"text/javascript\">\n"
  << "$(function() {\n"
  << "$('#" << theId << "').datepicker();\n"
  << "});\n"
  << "</script>\n";
  return out.str();
}

std::string CalculatorHTML::GetJavascriptSubmitAnswers()
{ std::stringstream out;
  std::string requestTypeSubmit, requestTypePreview, requestGiveUp, requestSolution;
  bool submitRandomSeed=false;
  if (theGlobalVariables.UserRequestRequiresLoadingRealExamData())
  { requestTypeSubmit  = "submitProblem";
    requestTypePreview = "submitProblemPreview";
  } else if (theGlobalVariables.UserGuestMode())
  { requestTypeSubmit  = "submitExerciseNoLogin";
    requestTypePreview = "submitExercisePreviewNoLogin";
    submitRandomSeed=true;
  } else
  { requestTypeSubmit  = "submitExercise";
    requestTypePreview = "submitExercisePreview";
    submitRandomSeed=true;
  }
  if (!theGlobalVariables.UserGuestMode())
    requestGiveUp="problemGiveUp";
  else
    requestGiveUp="problemGiveUpNoLogin";
  if (!theGlobalVariables.UserGuestMode())
    requestSolution="problemSolution";
  else
    requestSolution="problemSolutionNoLogin";
  out
  << "<script type=\"text/javascript\"> \n"
  << "var JavascriptInsertionAlreadyCalled;\n"
  << "JavascriptInsertionAlreadyCalled=false;\n"
  << "var numInsertedJavascriptChildren;\n"
  << "var numInsertedJavascriptChildren=0;\n"
  << "var timerForPreviewAnswers;\n"
  << "function previewAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  timerForPreviewAnswers=setTimeout(function(){\n"
  << "    params=\"" << this->ToStringCalculatorArgumentsForProblem(requestTypePreview, "true", "", submitRandomSeed) << "\";\n"
  << "    submitOrPreviewAnswers(idAnswer, idVerification, params,\"" << requestTypePreview
  << "\");\n"
  << "  }, 1700);"
  << "}\n"
  << "function previewAnswersNoTimeOut(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestTypePreview, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params,\""
  << requestTypePreview << "\");\n"
  << "}\n"
  << "function submitAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestTypeSubmit, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params, \""
  << requestTypeSubmit <<  "\");\n"
  << "}\n"
  << "function giveUp(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestGiveUp, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params, \""
  << requestGiveUp << "\");\n"
  << "}\n"
  << "function showSolution(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestSolution, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params, \""
  << requestSolution << "\");\n"
  << "}\n"
  << "function submitOrPreviewAnswers(idAnswer, idVerification, inputParams, requestType){\n";
  out << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  spanVerification = document.getElementById(idVerification);\n"
  << "  if (spanVerification==null){\n"
  << "    spanVerification = document.createElement('span');\n"
  << "    document.body.appendChild(spanVerification);\n"
  << "    spanVerification.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idVerification + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanStudentAnswer = document.getElementById(idAnswer);\n"
  << "  inputParams+=\"&calculatorAnswer\" + idAnswer\n"
  << "          + \"=\"+encodeURIComponent(spanStudentAnswer.value);\n";
  for (int i=0; i<this->theProblemData.inputNonAnswerIds.size; i++)
  { out << "  inputParams+=\"&userInputBox" << this->theProblemData.inputNonAnswerIds[i]
    << "=\"+encodeURIComponent(document.getElementById(\""
    << this->theProblemData.inputNonAnswerIds[i] << "\").value);\n";
  }
  out
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"GET\", ";
  out << "\"" << theGlobalVariables.DisplayNameExecutable << "\"";
  out << " + \"?\"+inputParams, true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
;
//Old code, submits all answers. May need to be used as an alternative
//submission option.
//  for (int i=0; i<this->theContent.size; i++)
//    if (this->IsStudentAnswer(this->theContent[i]))
//      out << "  inputParams+=\"&calculatorAnswer" << this->theContent[i].GetKeyValue("id") << "=\"+encodeURIComponent("
//      << "document.getElementById('" << this->theContent[i].GetKeyValue("id") << "').value);\n";
  out
  << "  https.onload = function() {\n"
  << "    spanVerification.innerHTML=https.responseText;\n"
  << "    var scripts = spanVerification.getElementsByTagName('script');\n"
  << "    for (i=0; i<numInsertedJavascriptChildren; i++)\n"
  << "    { document.getElementsByTagName( 'head' )[0].removeChild(document.getElementsByTagName( 'head' )[0].lastChild);\n"
  << "      document.getElementsByTagName( 'head' )[0].appendChild(scriptChild);\n"
  << "    }\n"
  << "    numInsertedJavascriptChildren=0;\n"
  << "    for (i=0; i<scripts.length; i++){\n"
  << "      scriptChild= document.createElement('script');\n"
  << "      scriptChild.innerHTML=scripts[i].innerHTML;\n"
  << "      scriptChild.type='text/javascript';\n"
  << "      document.getElementsByTagName( 'head' )[0].appendChild(scriptChild);\n"
  << "      numInsertedJavascriptChildren++;\n "
  << "    }\n"
  << "    JavascriptInsertionAlreadyCalled=true;\n"
  << "    MathJax.Hub.Queue(['Typeset', MathJax.Hub, spanVerification]);\n"
  << "  }\n"
//  << "  https.send(inputParams);\n"
  << "  https.send();\n"
  << "}\n"
  << "</script>";
  return out.str();
}

std::string CalculatorHTML::GetDatePickerJavascriptInit()
{ std::stringstream out;
  out
  << "<link rel=\"stylesheet\" href=\"//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css\">\n"
  << "<script src=\"//code.jquery.com/jquery-1.10.2.js\"></script>\n"
  << "<script src=\"//code.jquery.com/ui/1.11.4/jquery-ui.js\"></script>\n"
  ;
  return out.str();
}

std::string Answer::ToString()
{ MacroRegisterFunctionWithName("Answer::ToString");
  std::stringstream out;
  out << "Answer id: " << this->answerId;
  out << "<br>Answer commands on give-up: " << this->commandsNoEnclosureAnswerOnGiveUpOnly;
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringCalculatorProblemSourceFromFileName");
  std::stringstream out;
  out << "<span class=\"calculatorExamProblem\">\n"
  << theFileName
  << "\n</span>";
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  this->NumProblemsFound++;
//  stOutput << "Figuring out current prob list ...";
//  std::stringstream notUsed;
//  this->FigureOutCurrentProblemList(notUsed);
  inputOutput.interpretedCommand= this->ToStringLinkFromFileName(this->CleanUpFileName(inputOutput.content));
}

std::string CalculatorHTML::ToStringLinkFromFileName(const std::string& theFileName, const std::string& stringToDisplay)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkFromFileName");
  std::string urledProblem=CGI::StringToURLString(theFileName);
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
  if (this->topicListFileName!="")
    refStreamNoRequest << "topicList=" << this->topicListFileName << "&";
  if (this->currentExamHome!="")
    refStreamNoRequest << "currentExamHome=" << this->currentExamHome << "&";
  if (!theGlobalVariables.UserGuestMode())
  { refStreamExercise << theGlobalVariables.DisplayNameExecutable << "?request=exercise&" << refStreamNoRequest.str();
    refStreamForReal << theGlobalVariables.DisplayNameExecutable << "?request=scoredQuiz&" << refStreamNoRequest.str();
  } else
  { refStreamExercise << "?request=exerciseNoLogin&" << refStreamNoRequest.str();
  }
  bool isActualProblem=true;
  if (isActualProblem)
    out << this->InterpretGenerateProblemManagementLink(refStreamForReal, refStreamExercise, theFileName, urledProblem);
  //else
  //  out << " <a href=\"" << refStreamExercise.str() << "\">Start</a> ";
  //if (inputOutput.GetTagClass()=="calculatorExamIntermediate")
#ifdef MACRO_use_MySQL
  bool problemAlreadySolved=false;
  if (this->currentUseR.problemNames.Contains(theFileName))
  { ProblemData& theProbData=this->currentUseR.problemData[this->currentUseR.problemNames.GetIndex(theFileName)];
    if (theProbData.numCorrectlyAnswered>=theProbData.theAnswers.size)
      problemAlreadySolved=true;
  }
  out << this->InterpretGenerateDeadlineLink(isActualProblem, theFileName, urledProblem, problemAlreadySolved);
#endif // MACRO_use_MySQL
  std::string finalStringToDisplay=stringToDisplay;
  if (finalStringToDisplay=="")
    finalStringToDisplay= FileOperations::GetFileNameFromFileNameWithPath(theFileName);
  out << finalStringToDisplay;
  return out.str();
}

bool CalculatorHtmlFunctions::innerInterpretProblemGiveUp
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretProblemGiveUp");
  (void)input;
  return output.AssignValue(HtmlInterpretation::GetAnswerOnGiveUp(), theCommands);
}

bool CalculatorHtmlFunctions::innerInterpretProblem
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretProblem");
  CalculatorHTML theProblem;
  if (!input.IsOfType<std::string>(&theProblem.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  theProblem.flagRandomSeedGiven=true;
  theProblem.theProblemData.randomSeed=theCommands.theObjectContainer.CurrentRandomSeed;
  theProblem.InterpretHtml(theCommands.Comments);
  std::stringstream out;
  out << theProblem.outputHtmlBodyNoTag;
  out << "<hr>Time to parse html: " << std::fixed << theProblem.timeToParseHtml << " second(s). ";
  out << "<br>Intermediate interpretation times (per attempt): ";
  for (int i=0; i<theProblem.timeIntermediatePerAttempt.size; i++)
    for (int j=0; j<theProblem.timeIntermediateComments[i].size; j++ )
      out << "<br>" << theProblem.timeIntermediateComments[i][j]
      << ": " << theProblem.timeIntermediatePerAttempt[i][j] << " second(s)";
  out << "<br>Interpretation times (per attempt): "
  << theProblem.timePerAttempt.ToStringCommaDelimited();
  return output.AssignValue(out.str(), theCommands);
}

std::string CalculatorHTML::ToStringExtractedCommands()
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringExtractedCommands");
  std::stringstream out;
  out << "<hr><b>The commands extracted from the HTML follow.</b><br>";
  out << "<table>";
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].syntacticRole!="")
      out << "<tr>" << "<td>" << this->theContent[i].ToStringDebug() << "</td>"
      << "</tr>";
    else
      out << "<tr><td></td></tr>";
  out << "</table>";
  out << "<hr><b>The HTML from which the commands were extracted follows.</b><br>" << this->inputHtml
  << "<hr><b>The parsing stack follows.</b>" << this->ToStringParsingStack(this->eltsStack) << "<hr>";
  return out.str();
}

std::string CalculatorHTML::ToStringContent()
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringContent");
  std::stringstream out;
//  out << "<hr><b>The split strings follow. </b><hr>" << splitStrings.ToStringCommaDelimited();
  out << "<hr><b>The extracted commands follow.</b><br>";
  for (int i=0; i<this->theContent.size; i++)
    out << this->theContent[i].ToStringTagAndContent();
  out << "<hr><b>The html read follows.</b><br>" << this->inputHtml << "<hr>";
  return out.str();
}

void SyntacticElementHTML::resetAllExceptContent()
{ this->tag="";
  this->tagKeys.SetSize(0);
  this->tagValues.SetSize(0);
  this->syntacticRole="";
  this->flagUseDisplaystyleInMathMode=false;
  this->children.SetSize(0);
}

std::string SyntacticElementHTML::ToStringOpenTag(bool immediatelyClose)
{ if (this->tag=="")
    return "";
  std::stringstream out;
  out << "<" << this->tag;
  for (int i=0; i<this->tagKeys.size; i++)
    out << " " << this->tagKeys[i] << "=\"" << this->tagValues[i] << "\"";
  for (int i=0; i<this->defaultKeysIfMissing.size; i++)
    if (!this->tagKeys.Contains(this->defaultKeysIfMissing[i]))
      out << " " << this->defaultKeysIfMissing[i] << "=\"" << this->defaultValuesIfMissing[i] << "\"";
  if (this->tagKeysWithoutValue.size>0)
    out << " " << this->tagKeysWithoutValue[0];
  if (immediatelyClose)
    out << "/";
  out << ">";
  return out.str();
}

std::string SyntacticElementHTML::ToStringCloseTag()
{ if (this->tag=="")
    return "";
  return "</" + this->tag + ">";
}

std::string SyntacticElementHTML::ToStringTagAndContent()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::ToStringTagAndContent");
  if (this->syntacticRole=="")
    return this->content;
  std::stringstream out;
  out << this->ToStringOpenTag() + this->content + this->ToStringCloseTag();
  if (this->children.size>0)
  { out << "[";
    for (int i=0; i< this->children.size; i++)
    { out << this->children[i].ToStringDebug();
      if (i!=this->children.size-1)
        out << ", ";
    }
    out << "]";
  }
  return out.str();
}

std::string SyntacticElementHTML::ToStringDebug()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::ToString");
  if (this->syntacticRole=="")
    return CGI::StringToHtmlString(this->ToStringTagAndContent());
  std::stringstream out;
  out << "<span style=\"color:green\">";
  out << CGI::StringToHtmlString(this->syntacticRole);
  out << "</span>";
  out << "[" << CGI::StringToHtmlString(this->ToStringTagAndContent()) << "]";
  return out.str();
}

std::string SyntacticElementHTML::GetKeyValue(const std::string& theKey)const
{ MacroRegisterFunctionWithName("SyntacticElementHTML::GetKeyValue");
  int theIndex=this->tagKeys.GetIndex(theKey);
  if (theIndex==-1)
    return "";
  return this->tagValues[theIndex];
}

void SyntacticElementHTML::SetKeyValue(const std::string& theKey, const std::string& theValue)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::SetKeyValue");
  if (this->tagKeys.size!=this->tagValues.size)
    crash << "Programming error: number of keys different from number of values" << crash;
  int theIndex=this->tagKeys.GetIndex(theKey);
  if (theIndex==-1)
  { theIndex=this->tagKeys.size;
    this->tagKeys.AddOnTop(theKey);
    this->tagValues.SetSize(this->tagKeys.size);
  }
  this->tagValues[theIndex]=theValue;
}

std::string SyntacticElementHTML::ToStringInterpretedBody()
{ if (this->syntacticRole=="")
    return this->content;
  if (this->IsInterpretedNotByCalculator())
    return this->interpretedCommand;
  std::stringstream out;
  out << this->ToStringOpenTag();
  if (this->interpretedCommand!="")
  { if (this->flagUseMathMode)
    { out << "\\( ";
      if (this->flagUseDisplaystyleInMathMode)
        out << "\\displaystyle ";
    }
    out << this->interpretedCommand;
    if (this->flagUseMathMode)
      out << " \\)";
  }
  out << this->ToStringCloseTag();
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedNotByCalculator()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::IsInterpretedNotByCalculator");
  if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return
  tagClass=="calculatorExamProblem" || tagClass== "calculatorExamIntermediate" ||
  tagClass=="calculatorAnswer" || tagClass=="calculatorManageClass" ||
  tagClass=="generateTopicTable" ||
  tagClass=="generateTableOfContents" ||
//  tagClass=="htmlStart" || tagClass=="htmlFinish" ||
//  tagClass=="bodyStart" || tagClass=="bodyFinish" ||
//  tagClass=="headStart" || tagClass=="headFinish" ||
  this->IsAnswerElement(0)
  ;
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringProblemGeneration()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculator" || tagClass=="calculatorHidden" ||
  tagClass=="calculatorShowToUserOnly" ;
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculator" || tagClass=="calculatorHidden"
;
}

bool SyntacticElementHTML::IsAnswer()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculatorAnswer";
}

bool SyntacticElementHTML::IsCalculatorCommand()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculator";
}

bool SyntacticElementHTML::IsCalculatorHidden()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculatorHidden";
}

bool SyntacticElementHTML::IsCalculatorHiddenCommentsBeforeSubmission()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class") =="calculatorHiddenIncludeInCommentsBeforeSubmission";
}

bool SyntacticElementHTML::IsHidden()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorHidden" || tagClass== "calculatorHiddenIncludeInCommentsBeforeSubmission"
;
}

bool SyntacticElementHTML::IsSolution()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorSolution"
;
}

bool SyntacticElementHTML::IsAnswerOnGiveUp()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorAnswerOnGiveUp"
;
}

bool SyntacticElementHTML::IsCommentBeforeSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorCommentBeforeSubmission"
  //||
  //tagClass=="calculatorHiddenIncludeInCommentsBeforeSubmission"
;
}

bool SyntacticElementHTML::IsCommentAfterSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorCommentAfterSubmission"
;
}

bool SyntacticElementHTML::IsAnswerElement(std::string* desiredAnswerId)
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  bool result= tagClass=="calculatorButtonSubmit" ||
  tagClass=="calculatorButtonInterpret" ||
  tagClass=="calculatorButtonGiveUp" ||
  tagClass=="calculatorButtonSolution" ||
  tagClass=="calculatorMQField" ||
  tagClass=="calculatorMQButtonPanel" ||
  tagClass=="calculatorAnswerVerification" ||
  tagClass=="calculatorSolution"
;
  if (result && desiredAnswerId!=0)
    *desiredAnswerId=this->GetKeyValue("name");
  return result;
}

std::string CalculatorHTML::PrepareUserInputBoxes()
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareInterpreter");
  if (this->flagIsForReal)
    return "";
  std::stringstream out;
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  std::string inputNonAnswerReader;
  for (int i=0; i<theArgs.size(); i++)
    if (MathRoutines::StringBeginsWith(theArgs.theKeys[i], "userInputBox", &inputNonAnswerReader))
      if (inputNonAnswerReader!="" && theArgs.theValues[i]!="")
      { out << "setInputBox(name="
        << inputNonAnswerReader
        << ", value=" << CGI::URLStringToNormal(theArgs.theValues[i])
        << "); ";
      }
  return out.str();
}

std::string CalculatorHTML::GetProblemHeaderEnclosure()
{ std::stringstream out;
  out << "CommandEnclosure{}(";
  out <<  " setRandomSeed{}(" << this->theProblemData.randomSeed << "); ";
  out << this->PrepareUserInputBoxes();
  out << "); ";
  return out.str();
}

bool CalculatorHTML::PrepareCommandsGenerateProblem(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsGenerateProblem");
  (void) comments;
  std::stringstream streamCommands;
  streamCommands << this->GetProblemHeaderEnclosure();//first calculator enclosure contains the header
  int numCommandsSoFar=2;//two commands at the start: the opEndStatement command and
  // the first enclosure.
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration())
      continue;
    std::string commandCleaned=this->CleanUpCommandString(currentElt.content);
    std::string commandEnclosed="CommandEnclosure{}( " + commandCleaned + " );";
    streamCommands << commandEnclosed;
    currentElt.commandIndex=numCommandsSoFar;
    numCommandsSoFar++;
  }
  this->theProblemData.commandsGenerateProblem=streamCommands.str();
  return true;
}

bool CalculatorHTML::ParseHTMLPrepareCommands(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ParseHTMLPrepareCommands");
  if (!this->ParseHTML(comments))
    return false;
  return this->PrepareCommands(comments);
}

bool CalculatorHTML::PrepareCommands(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommands");
  if (!this->PrepareCommandsGenerateProblem(comments))
    return false;
  for (int i=0; i<this->theProblemData.theAnswers.size; i++)
  { if (!this->PrepareCommandsAnswer(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommandsAnswerOnGiveUp(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommandsSolution(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommentsBeforeSubmission(this->theProblemData.theAnswers[i], comments))
      return false;
  }
  return true;
}

bool CalculatorHTML::PrepareCommandsAnswerOnGiveUp
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsAnswerOnGiveUp");
  (void) comments;
  std::stringstream streamCommands;
  //stOutput << "<hr>DEBUG: Preparing give-up commands for: " << theAnswer.answerId << "<hr>";
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (!currentElt.IsAnswerOnGiveUp())
      continue;
    //stOutput << "<br>Current element: " << currentElt.ToStringDebug();
    //stOutput << "<br>Comparing: " << theAnswer.answerId << " to: "
    //<< currentElt.GetKeyValue("name");
    if (currentElt.GetKeyValue("name")==theAnswer.answerId)
      streamCommands << this->CleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly=streamCommands.str();
  //stOutput << "<br>Final give up command: " << theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly;
  return true;
}

bool CalculatorHTML::PrepareCommentsBeforeSubmission
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommentsBeforeSubmission");
  (void) comments;
  std::stringstream streamCommands;
  //stOutput << "<hr>DEBUG: Preparing give-up commands for: "
  //<< theAnswer.answerId << "<hr>";
  int counter=0;
//  stOutput << "<hr>DEBUG: Call stack: " << crash.GetStackTraceEtcErrorMessage();
  theAnswer.commandIndicesCommentsBeforeSubmission.SetSize(0);
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (!currentElt.IsCommentBeforeSubmission())
      continue;
    //stOutput << "<br>Current element: " << currentElt.ToStringDebug();
    //stOutput << "<br>Comparing: " << theAnswer.answerId << " to: "
    //<< currentElt.GetKeyValue("name");
    if (currentElt.GetKeyValue("name")==theAnswer.answerId)
    { streamCommands << "CommandEnclosure{}{"
      << this->CleanUpCommandString(currentElt.content)
      << "};";
      theAnswer.commandIndicesCommentsBeforeSubmission.AddOnTop(counter);
      counter++;
    }
  }
  theAnswer.commandsCommentsBeforeSubmissionOnly=streamCommands.str();
//  stOutput << "<br>Final comments command: "
//  << theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly;
  return true;
}

bool CalculatorHTML::PrepareCommandsSolution
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsSolution");
  (void) comments;
  std::stringstream streamCommands;
  //stOutput << "<hr>DEBUG: Preparing give-up commands for: " << theAnswer.answerId << "<hr>";
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& solutionElt=this->theContent[i];
    if (!solutionElt.IsSolution())
      continue;
    //stOutput << "<br>Current element: " << currentElt.ToStringDebug();
    //stOutput << "<br>Comparing: " << theAnswer.answerId << " to: "
    //<< currentElt.GetKeyValue("name");
    if (solutionElt.GetKeyValue("name")!=theAnswer.answerId)
      continue;
    int numCommandsSoFar=2;
    theAnswer.solutionElements=solutionElt.children;
    for (int j=0; j<theAnswer.solutionElements.size; j++)
    { SyntacticElementHTML& currentElt=theAnswer.solutionElements[j];
      if (!currentElt.IsCalculatorCommand() && !currentElt.IsCalculatorHidden())
        continue;
      currentElt.commandIndex=numCommandsSoFar;
      numCommandsSoFar++;
      streamCommands << "CommandEnclosure{}("
      << this->CleanUpCommandString(currentElt.content) << "); ";
    }
  }
  theAnswer.commandsSolutionOnly=streamCommands.str();
  //stOutput << "<br>Final give up command: " << theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly;
  return true;
}

bool CalculatorHTML::PrepareCommandsAnswer
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsAnswer");
  std::stringstream streamCommands;
  streamCommands << this->GetProblemHeaderEnclosure();//first calculator enclosure contains the header
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (!currentElt.IsCalculatorHidden() && !currentElt.IsCalculatorCommand()
        && !currentElt.IsAnswer())
      continue;
    std::string commandCleaned=this->CleanUpCommandString(this->theContent[i].content);
    std::string commandEnclosed="CommandEnclosure{}( " + commandCleaned + " );";
    if (currentElt.IsAnswer() && currentElt.GetKeyValue("id")==theAnswer.answerId)
    { theAnswer.commandsBeforeAnswer = streamCommands.str();
      theAnswer.commandVerificationOnly=commandCleaned;
      return true;
    }
    if (this->theContent[i].IsCalculatorHidden() || this->theContent[i].IsCalculatorCommand())
      streamCommands << commandEnclosed;
  }
  comments << "<b>Somethins is wrong: did not find answer for answer tag: "
  << theAnswer.answerId << ". </b>";
  return false;
}

bool CalculatorHTML::PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::PrepareAndExecuteCommands");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  this->PrepareCommands(comments);

  theInterpreter.init();
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator init time");

  //stOutput << "nput cmds: " << calculatorCommands.str();
  theInterpreter.Evaluate(this->theProblemData.commandsGenerateProblem);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator evaluation time");
  //stOutput << "<hr>Fter eval: " << theInterpreter.outputString;
  bool result=!theInterpreter.flagAbortComputationASAP && theInterpreter.syntaxErrors=="";
  if (!result)
    comments << "Failed to interpret your file. The interpretation input was:<br> "
    << this->theProblemData.commandsGenerateProblem << "<br>"
    << "The result of the interpretation attempt is:<br>"
    << theInterpreter.outputString << "<br><b>Comments</b><br>"
    << theInterpreter.outputCommentsString;

  for (int i=0; i<theInterpreter.theObjectContainer.theUserInputTextBoxes.size; i++)
    this->theProblemData.inputNonAnswerIds.AddOnTop
    (theInterpreter.theObjectContainer.theUserInputTextBoxes[i]);
  return result;
}

std::string SyntacticElementHTML::GetTagClass()
{ return this->GetKeyValue("class");
}

#ifdef MACRO_use_MySQL

std::string DatabaseRoutines::ToStringClassDetails
(bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& userLabels,
 HashedList<std::string, MathRoutines::hashString>& databaseSpanList,
 List<std::string>& databaseProblemWeights
 )
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringClassDetails");
  std::stringstream out;
  std::string userRole = adminsOnly ? "admin" : "student";
  int numUsers=0;
  std::stringstream tableStream;
  tableStream << "Database name: " << this->theDatabaseName;
  tableStream << "<br>Database user: " << this->databaseUser << "<br>";
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th>"
  << "<th>Activation manual email</th>"
  << "<th>Points</th><th>Section/Group</th></tr>";
  UserCalculator currentUser;
  currentUser.currentTable="users";

  int indexUser=-1, indexExtraInfo=-1;
  for (int i=0; i<userLabels.size; i++)
  { if (userLabels[i]=="username")
      indexUser=i;
    if (userLabels[i]=="extraInfo")
      indexExtraInfo=i;
  }
  if (indexUser==-1 || indexExtraInfo==-1)
  { out << "<span style=\"color:red\"><b>This shouldn't happen: failed to find necessary "
    << "column entries in the database. "
    << "This is likely a software bug.</b></span>"
    << "indexUser, indexExtraInfo: "
    << indexUser << ", "
    << indexExtraInfo << ", "
    ;
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
  for (int i=0; i<userTable.size; i++)
  { std::stringstream failureStream;
    currentUser.username=userTable[i][indexUser];
    if (!currentUser.FetchOneUserRow(*this, &failureStream))
    { currentUser.email=failureStream.str();
      currentUser.actualActivationToken="error";
      currentUser.userRole="error";
    }
    if (adminsOnly xor (currentUser.userRole=="admin"))
      continue;
    numUsers++;
    std::stringstream oneTableLineStream;
    oneTableLineStream << "<tr>"
    << "<td>" << userTable[i][indexUser] << "</td>"
    << "<td>" << currentUser.email.value << "</td>"
    ;
    bool isActivated=true;
    if (currentUser.actualActivationToken!="activated" && currentUser.actualActivationToken!="error")
    { isActivated=false;
      numActivatedUsers++;
      oneTableLineStream << "<td><span style=\"color:red\">not activated</span></td>";
      if (currentUser.actualActivationToken!="")
        oneTableLineStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken.value, theGlobalVariables.DisplayNameExecutable,
         userTable[i][indexUser])
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
      << CGI::StringToURLString(UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken.value, theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable,
         userTable[i][indexUser]) )
      << CGI::StringToURLString("\n\n")
      << "The link does not work with apple safari; if you use safari, please contact us by email"
      << " and we will activate your account manually. "
      << " Once you activate your account, you can log in safely here: \n"
      << CGI::StringToURLString("\n\n")
      << theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable
      << CGI::StringToURLString("\n\n")
      << "\n\nIf using the Safari browser/apple, you can use this (a little less safe link): "
      << CGI::StringToURLString("\n\n")
      << CGI::StringToURLString(theGlobalVariables.hopefullyPermanentWebAdressOfServerExecutable + "?request=login&ignoreSecurity=true")
      << CGI::StringToURLString("\n\n")
      << "Best regards, \n your Math 140 instructors."
      ;
      oneTableLineStream << emailBody.str() << "\">Send email manually.</a> "
      ;
      oneTableLineStream << "</td>";
    } else if (currentUser.actualActivationToken=="error")
      oneTableLineStream << "<td>error</td><td></td>";
    else
      oneTableLineStream << "<td><span style=\"color:green\">activated</span></td><td></td><td></td>";

    int indexProblemData=currentUser.selectedRowFieldNamesUnsafe.GetIndex("problemData");
    std::stringstream commentsProblemData;
    if (indexProblemData==-1)
      oneTableLineStream << "<td>No solutions history</td>";
    else if (currentUser.InterpretDatabaseProblemData
             (currentUser.selectedRowFieldsUnsafe[indexProblemData], commentsProblemData))
    { currentUser.ComputePointsEarned(databaseSpanList, databaseProblemWeights);
      oneTableLineStream << "<td>" << std::fixed << std::setw(1)
      << std::setprecision(1) << currentUser.pointsEarned.GetDoubleValue() << "</td>";
    } else
      oneTableLineStream << "<td>Failed to load problem data. Comments: " << commentsProblemData.str() << "</td>";
    oneTableLineStream << "<td>" << userTable[i][indexExtraInfo] << "</td>";
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
    out << ", <span style=\"color:red\">" << numActivatedUsers << " have not activated their accounts";
  out << "</span>. \n"

  << "<br>";
  out << tableStream.str();
  out << "<hr>DEBUG<br>"
  <<  userTable.ToStringCommaDelimited() << "<hr><hr>";
  return out.str();
}
#endif // MACRO_use_MySQL

std::string CalculatorHTML::  ToStringClassDetails
( bool adminsOnly,
  const SyntacticElementHTML& inputElement
)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringClassDetails");
  std::stringstream out;
#ifdef MACRO_use_MySQL
  std::string idAddressTextarea= adminsOnly ? "inputAddAdmins" : "inputAddStudents";
  std::string idExtraTextarea= adminsOnly ? "inputAddAdminsExtraInfo" : "inputAddStudentsExtraInfo";
  if (inputElement.GetKeyValue("id")!="")
  { idAddressTextarea+= inputElement.GetKeyValue("id");
    idExtraTextarea+=inputElement.GetKeyValue("id");
  } else
  { idAddressTextarea+=this->fileName;
    idExtraTextarea+=this->fileName;
  }
  std::string userRole = adminsOnly ? "admin" : "student";
  std::string idOutput="outputAdd";
  if (adminsOnly)
    idOutput+="Admins";
  else
    idOutput+="Students";
  idOutput+= CGI::StringToURLString(this->fileName);
  out << "Add <b>" << userRole << "(s)</b>.<br> ";
//  out << "<b>Warning: there's no remove button yet.</b><br>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idAddressTextarea << "\"";
  out << "placeholder=\"email or user list, comma, space or ; separated\">";
  out << "</textarea>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idExtraTextarea << "\"";
  out << " placeholder=\"optional, for sorting users in groups; for example section #\">";
  out << "</textarea>";
  out << "<br>";
  out
  << "<button class=\"normalButton\" "
//  << "onclick=\"addEmailsOrUsers("
//  << "'"    << idAddressTextarea
//  << "', '" << CGI::StringToURLString(this->fileName)
//  << "', '" << idOutput
//  << "', '" << userRole
//  << "', '" << idExtraTextarea
//  << "', 'addEmails'"
//  << " )\" "
  << "disabled> [Disabled] Add emails</button>";
  out
  << "<button class=\"normalButton\" onclick=\"addEmailsOrUsers("
  << "'"    << idAddressTextarea
  << "', '" << CGI::StringToURLString(this->fileName)
  << "', '" << idOutput
  << "', '" << userRole
  << "', '" << idExtraTextarea
  << "', 'addUsers'"
  << " )\"> Add users (no email sending)</button> ";
  out << "<br><span id=\"" << idOutput << "\">\n";
  DatabaseRoutines theRoutines;
  out << theRoutines.ToStringClassDetails
  (adminsOnly, this->userTablE, this->labelsUserTablE,
   this->databaseProblemAndHomeworkGroupList, this->databaseProblemWeights);
  out << "</span>";
#else
  out << "<b>Adding emails not available (database not present).</b> ";
#endif // MACRO_use_MySQL

  return out.str();
}

bool CalculatorHTML::PrepareClassData(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareClassData");
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  if (!theRoutines.PrepareClassData(this->fileName, this->userTablE, this->labelsUserTablE, commentsOnFailure))
    return false;
  int indexExtraInfo=-1;
  for (int i=0; i<this->labelsUserTablE.size; i++)
    if (this->labelsUserTablE[i]=="extraInfo")
      indexExtraInfo=i;
  if (indexExtraInfo==-1)
  { commentsOnFailure << "Failed to load section data. ";
    return false;
  }
  HashedList<std::string, MathRoutines::hashString> theSections;
  for (int i=0; i<this->userTablE.size; i++)
    theSections.AddOnTopNoRepetition(this->userTablE[i][indexExtraInfo]);
  this->databaseStudentSectionS=theSections;
  this->databaseStudentSectionS.QuickSortAscending();
  return true;
#else
  commentsOnFailure << "Error: database not running. ";
  return false;
#endif // MACRO_use_MySQL

}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return;
#ifdef MACRO_use_MySQL
  std::stringstream out;
  if (!this->flagLoadedClassDataSuccessfully)
  { out << "<b>Failed to load class data successfully.</b>";
    inputOutput.interpretedCommand=out.str();
    return;
  }
  out << "<hr><hr>";
  out << this->ToStringClassDetails(false, inputOutput);
  out << "<hr><hr>";
  out << this->ToStringClassDetails(true, inputOutput);
  inputOutput.interpretedCommand=out.str();
#else
  inputOutput.interpretedCommand="<b>Managing class not available (no database).</b>";
#endif // MACRO_use_MySQL
}

bool CalculatorHTML::ComputeAnswerRelatedStrings(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::ComputeAnswerRelatedStrings");
  std::string desiredAnswerId = inputOutput.GetKeyValue("id");
  if (desiredAnswerId=="")
  { inputOutput.interpretedCommand= "<b>Error: could not generate submit button: the answer tag does not have a valid id. Please fix the problem template.</b>";
    return false;
  }
  int theIndex=this->GetAnswerIndex(desiredAnswerId);
  if (theIndex==-1)
    crash  << "This is not supposed to happen: problem has syntactic element with answerId: "
    << desiredAnswerId << " but the answerId is missing from the list of known answer ids. "
    << this->theProblemData.ToStringAvailableAnswerIds() << crash;
  Answer& currentA=this->theProblemData.theAnswers[theIndex];
  std::string& answerId=currentA.answerId;
  currentA.varAnswerId = answerId+"spanVariable";
  currentA.varMQfield=answerId+"MQspanVar";
  currentA.MQobject= "answerMathQuillObjects["+ std::to_string(this->NumAnswerIdsMathquilled)+"]";
  this->NumAnswerIdsMathquilled++;
  currentA.MQUpdateFunction  = answerId+"MQFieldUpdate";
  currentA.idVerificationSpan="verification"+answerId;
  currentA.idSpanSolution="solution"+answerId;
  if (currentA.idMQfield=="")
    currentA.idMQfield = answerId+"MQspanId";
  if (currentA.idMQButtonPanelLocation=="")
    currentA.idMQButtonPanelLocation=answerId+"MQbuttonPanel";
  std::stringstream previewAnswerStream;
  previewAnswerStream << "previewAnswers('" << answerId << "', '"
  << currentA.idVerificationSpan << "');";
  currentA.javascriptPreviewAnswer=previewAnswerStream.str();
  currentA.htmlButtonSubmit= "<button class=\"submitButton\" onclick=\"submitAnswers('"
  + answerId + "', '" + currentA.idVerificationSpan + "')\">Submit</button>";
  currentA.htmlButtonInterpret= (std::string)"<button class=\"previewButton\" onclick=\""
  + "previewAnswersNoTimeOut('" + answerId + "', '"
  + currentA.idVerificationSpan + "')" + "\">Interpret</button>";
  if (!this->flagIsForReal)
  { if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly!="")
      currentA.htmlButtonAnswer= "<button class=\"showAnswerButton\" onclick=\"giveUp('"
      + answerId + "', '" + currentA.idVerificationSpan + "')\">Answer</button>";
    else
      currentA.htmlButtonAnswer= "No ``give-up'' answer available. ";
    if (currentA.flagSolutionFound)
      currentA.htmlButtonSolution="<button class=\"showSolutionButton\" onclick=\"showSolution('" +answerId+ "','"
      + currentA.idSpanSolution +"')\"> Solution</button>";
    else
      currentA.htmlButtonSolution="";
  }
  inputOutput.defaultKeysIfMissing.AddOnTop("onkeyup");
  inputOutput.defaultValuesIfMissing.AddOnTop
  (currentA.javascriptPreviewAnswer+currentA.MQUpdateFunction +"();");
//  inputOutput.defaultKeysIfMissing.AddOnTop("style");
//  inputOutput.defaultValuesIfMissing.AddOnTop("height:70px");
  currentA.htmlTextareaLatexAnswer=
  inputOutput.ToStringOpenTag() + inputOutput.ToStringCloseTag();
  currentA.htmlSpanMQfield =
  (std::string)"<div class=\"calculatorMQfieldEnclosure\">"+
  "<span id='" + currentA.idMQfield + "'>" + "</span>"+
  "</div>";
  //currentA.htmlMQjavascript= CalculatorHtmlFunctions::GetJavascriptMathQuillBox(currentA);
  currentA.htmlSpanMQButtonPanel=
  "<span id=\"" + currentA.idMQButtonPanelLocation + "\"></span>";
  currentA.htmlSpanSolution=  "<span id=\"" + currentA.idSpanSolution + "\"></span>";
  std::stringstream verifyStream;
  verifyStream << "<span id=\"" << currentA.idVerificationSpan << "\">";
  int numCorrectSubmissions=currentA.numCorrectSubmissions;
  int numSubmissions= currentA.numSubmissions;
  if (theGlobalVariables.userCalculatorRequestType=="scoredQuiz")
  { if (numCorrectSubmissions >0)
    { verifyStream << "<b><span style=\"color:green\">Correctly answered: \\("
      << currentA.firstCorrectAnswerClean << "\\) </span></b> ";
      if (numSubmissions>0)
        verifyStream << "<br>Used: " << numSubmissions << " attempt(s) (" << numCorrectSubmissions << " correct).";
    } else
    { verifyStream << " <b><span style=\"color:brown\">Waiting for answer. </span></b>";
      if (numSubmissions>0)
        verifyStream << numSubmissions << " attempt(s) so far. ";
    }
  } else
    verifyStream << " <b><span style=\"color:brown\">Waiting for answer [unlimited tries]</span></b>";
  verifyStream << "</span>";
  currentA.htmlSpanVerifyAnswer=verifyStream.str();
  return true;
}

void CalculatorHTML::InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateStudentAnswerButton");
  if (!this->ComputeAnswerRelatedStrings(inputOutput))
    return;
  Answer& currentA=this->theProblemData.
  theAnswers[this->GetAnswerIndex(inputOutput.GetKeyValue("id"))];
  if (!currentA.flagAutoGenerateMQButtonPanel && !currentA.flagAutoGenerateMQfield &&
      !currentA.flagAutoGenerateSubmitButtons && !currentA.flagAutoGenerateVerificationField)
  { inputOutput.interpretedCommand= currentA.htmlTextareaLatexAnswer;
    return;
  }
  std::stringstream out;
  out << "<table>";
//  out << "<tr><td><small>answer:</small><td></tr>";
  out << "<tr><td>";
  out << "<table><tr>";
  if (currentA.flagAutoGenerateMQfield)
    out << "<td>" << currentA.htmlSpanMQfield  << "</td>";
  if (currentA.flagAutoGenerateMQButtonPanel)
    out << "<td>" << currentA.htmlSpanMQButtonPanel << "</td>";
  out << "</tr></table>";
  out << "</td>";
  if (currentA.flagAutoGenerateSubmitButtons)
  { out << "<td>";
    out << "<table>";
    out << "<tr><td>";
    out << currentA.htmlButtonSubmit;
    out << "</td></tr>";
    out << "<tr><td>";
    out << currentA.htmlButtonInterpret;
    out << "</td></tr>";
    out << "<tr><td>";
    out << currentA.htmlButtonAnswer;
    out << "</td></tr>";
    out << "<tr><td>";
    out << currentA.htmlButtonSolution;
    out << "</td></tr>";
    out << "</table>";
    out << "</td>";
  }
  out << "<td>";
  out << "<button class=\"accordion\">details</button>";
//  out << "<div class=\"panel\">";
  out << currentA.htmlTextareaLatexAnswer;
//  out << "</div>";
  out << "</td>";
  out << "</tr>";
  out << "</table>";
  if (currentA.flagAutoGenerateVerificationField)
    out << currentA.htmlSpanVerifyAnswer;
  inputOutput.interpretedCommand=out.str();
}

void CalculatorHTML::InterpretNotByCalculator(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretNotByCalculator");
  std::string tagClass=inputOutput.GetTagClass();
  if (tagClass=="calculatorExamProblem" || tagClass=="calculatorExamIntermediate")
    this->InterpretGenerateLink(inputOutput);
  else if (tagClass=="calculatorAnswer")
    this->InterpretGenerateStudentAnswerButton(inputOutput);
  else if (tagClass=="calculatorManageClass")
    this->InterpretManageClass(inputOutput);
  else if (tagClass=="generateTopicTable")
    this->InterpretTopicList(inputOutput);
  else if (tagClass=="generateTableOfContents")
    this->InterpretTableOfContents(inputOutput);
}

std::string CalculatorHTML::CleanUpFileName(const std::string& inputLink)
{ MacroRegisterFunctionWithName("CalculatorHTML::CleanUpFileName");
  if (inputLink.size()==0)
    return inputLink;
  unsigned firstMeaningfulChar=0;
  for (; firstMeaningfulChar<inputLink.size(); firstMeaningfulChar++)
    if (inputLink[firstMeaningfulChar]!='\n' &&
        inputLink[firstMeaningfulChar]!='\r' &&
        inputLink[firstMeaningfulChar]!='\t' &&
        inputLink[firstMeaningfulChar]!=' ')
      break;
  unsigned lastMeaningfulChar=inputLink.size()-1;
  for (;lastMeaningfulChar>firstMeaningfulChar; lastMeaningfulChar--)
    if (inputLink[lastMeaningfulChar]!='\n' &&
        inputLink[lastMeaningfulChar]!='\r' &&
        inputLink[lastMeaningfulChar]!='\t' &&
        inputLink[lastMeaningfulChar]!=' ')
      break;
  if (firstMeaningfulChar>=inputLink.size())
    return "";
  return inputLink.substr(firstMeaningfulChar, lastMeaningfulChar-firstMeaningfulChar+1);
}

#include "vpfHeader5Crypto.h"

std::string CalculatorHTML::GetDeadline
(const std::string& problemName, const std::string& sectionNumber, bool inheritFromGroup, bool& outputIsInherited)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetDeadline");
  outputIsInherited=false;
  std::string result;
  //stOutput << "<br>DEBUG: Fetching deadline for: " << problemName << "<br>this->databaseStudentSectionsPerProblem: " << this->databaseStudentSectionsPerProblem;
  int indexInDatabase=this->databaseProblemAndHomeworkGroupList.GetIndex(problemName);
  //stOutput << "<br>DEBUG: index of  " << problemName << " in  " << this->databaseProblemAndHomeworkGroupList
  //<< ": " << indexInDatabase;

  if (indexInDatabase!=-1)
  { int indexSection=  this->databaseStudentSectionsPerProblem[indexInDatabase].GetIndex(sectionNumber);
    if (indexSection!=-1)
      result=this->databaseDeadlinesBySection[indexInDatabase][indexSection];
  }
  //stOutput << "DEBUG: Index in db is: " << indexInDatabase << " result:" << result << "Inherit frm grp:"
  //<< inheritFromGroup;
  if (result!="" || !inheritFromGroup)
    return result;
  //stOutput << "DEBUG: hd prob list: " << this->hdProblemList.ToStringCommaDelimited();
  int indexInHDproblemList=this->hdProblemList.GetIndex(problemName);
  if (indexInHDproblemList==-1)
    return "";
//  stOutput << "got to hw group phase for prob: " << problemName;
  const std::string& hwGroup=this->hdHomeworkGroupCorrespondingToEachProblem[indexInHDproblemList];
  indexInDatabase=this->databaseProblemAndHomeworkGroupList.GetIndex(hwGroup);
  if (indexInDatabase!=-1)
  { int indexSection=  this->databaseStudentSectionsPerProblem[indexInDatabase].GetIndex(sectionNumber);
    if (indexSection!=-1)
    { result=this->databaseDeadlinesBySection[indexInDatabase][indexSection];
      outputIsInherited=true;
    }
  }
  return result;
}

std::string CalculatorHTML::ToStringOnEDeadlineFormatted
  (const std::string& cleanedUpLink,  const std::string& sectionNumber, bool isActualProblem,
   bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline)
{ bool deadlineInherited=false;
  std::stringstream out;
  std::string currentDeadline =
  this->GetDeadline(cleanedUpLink, sectionNumber, true, deadlineInherited);
  if (currentDeadline=="")
  { if (returnEmptyStringIfNoDeadline)
      return "";
    if (isActualProblem)
      out << "<span style=\"color:orange\">No deadline yet. </span>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  TimeWrapper now, deadline; //<-needs a fix for different time formats.
  //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
  std::stringstream badDateStream;
  if (!deadline.AssignMonthDayYear(currentDeadline, badDateStream))
    out << "<span style=\"color:red\">" << badDateStream.str() << "</span>";
  //  out << "deadline.date: " << deadline.theTime.tm_mday;
  now.AssignLocalTime();
  //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
  //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
  double secondsTillDeadline= deadline.SubtractAnotherTimeFromMeInSeconds(now)+7*3600;

  std::stringstream hoursTillDeadlineStream;
  if (secondsTillDeadline>0)
  { if (secondsTillDeadline<24*3600 && !problemAlreadySolved)
      hoursTillDeadlineStream << "<span style=\"color:red\">"
      << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false) << "</span>";
    else
      hoursTillDeadlineStream << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false)
      << " left. ";
  } else
    hoursTillDeadlineStream << "Deadline has passed. ";
  if (isActualProblem)
  { out << "Deadline: ";
    if (deadlineInherited)
      out << "<span style=\"color:blue\">" << currentDeadline << " (topic deadline)</span>. ";
    else
      out << "<span style=\"color:brown\">" << currentDeadline << " (per-problem deadline)</span>. ";
    out << hoursTillDeadlineStream.str();
    return out.str();
  }
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return out.str();
  out << "<span style=\"color:blue\">" << currentDeadline << "</span>. ";
  out << hoursTillDeadlineStream.str();
#else
  out  << "Database not running: no deadlines";
#endif // MACRO_use_MySQL
  return out.str();
}

std::string CalculatorHTML::ToStringDeadlinesFormatted
  (const std::string& cleanedUpLink,  const List<std::string>& sectionNumbers, bool isActualProblem,
   bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline)
{ if (sectionNumbers.size==0)
    return "No section number. ";
  if (sectionNumbers.size==1)
    return this->ToStringOnEDeadlineFormatted
    (cleanedUpLink, sectionNumbers[0], isActualProblem, problemAlreadySolved, returnEmptyStringIfNoDeadline);
  std::stringstream out;
  out << "<table>";
  for (int i=0; i<sectionNumbers.size; i++)
  { if (sectionNumbers[i]=="")
      continue;
    out << "<tr><td>Section " << sectionNumbers[i] << ":</td>";
    out << "<td>" << this->ToStringOnEDeadlineFormatted
    (cleanedUpLink, sectionNumbers[i], isActualProblem, problemAlreadySolved, returnEmptyStringIfNoDeadline) << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string CalculatorHTML::InterpretGenerateDeadlineLink
(bool isActualProblem,
 const std::string& cleaneduplink, const std::string& urledProblem, bool problemAlreadySolved)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateDeadlineLink");
  //  return "Submission deadline: to be announced. ";
  if (theGlobalVariables.UserGuestMode())
    return "";
  std::stringstream out;
  bool deadlineInherited=false;
#ifdef MACRO_use_MySQL
  if (isActualProblem)
  { int todoDeadlines;
    /*if (!theGlobalVariables.UserDefaultHasAdminRights())
      out << this->ToStringOnEDeadlineFormatted
      (cleaneduplink, this->currentUser.extraInfoUnsafe, isActualProblem, problemAlreadySolved);
    else if (!theGlobalVariables.UserStudentViewOn())
      out << this->ToStringDeadlinesFormatted
      (cleaneduplink, this->databaseStudentSectionS, isActualProblem, problemAlreadySolved);
    else
      out << this->ToStringDeadlinesFormatted
      (cleaneduplink,
       CGI::URLStringToNormal(theGlobalVariables.GetWebInput("studentSection")),
       isActualProblem, problemAlreadySolved);*/
  }
#endif // MACRO_use_MySQL
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return out.str();
  std::stringstream deadlineStream;
  deadlineStream << "<table><tr><td> Deadline: </td>";
  deadlineStream << "<td><table><tr><th>Grp.</th><th>Deadline</th></tr>";
  List<std::string> deadlineIds;
  deadlineIds.SetSize(this->databaseStudentSectionS.size);
  for (int i=0; i<this->databaseStudentSectionS.size; i++)
  { std::string& currentDeadlineId=deadlineIds[i];
    if (this->databaseStudentSectionS[i]=="")
      continue;
    currentDeadlineId = "deadline" + Crypto::CharsToBase64String(this->databaseStudentSectionS[i]+cleaneduplink);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    deadlineStream << "<tr>";
    deadlineStream << "<td>" << this->databaseStudentSectionS[i] << "</td>";
    deadlineStream << "<td> <input type=\"text\" id=\"" << currentDeadlineId << "\" value=\""
    << this->GetDeadline(cleaneduplink, this->databaseStudentSectionS[i], false, deadlineInherited)
    << "\"> " ;
    deadlineStream << this->GetDatePickerStart(currentDeadlineId);
    deadlineStream << "</td>";
    deadlineStream << "</tr>";
  }
  deadlineStream << "</table></td>";
  deadlineStream << "<td>\n";
  std::string deadlineIdReport="deadlineReport"+Crypto::CharsToBase64String(cleaneduplink);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  deadlineStream << "<button onclick=\"";
  deadlineStream << "submitStringAsMainInput('" << urledProblem
  << "='+encodeURIComponent('deadlines='+encodeURIComponent(";
  bool isFirst=true;
  for (int i=0; i<this->databaseStudentSectionS.size; i++)
  { if (this->databaseStudentSectionS[i]=="")
      continue;
    if (!isFirst)
      deadlineStream << "+";
    isFirst=false;
    deadlineStream << "'" << CGI::StringToURLString(this->databaseStudentSectionS[i]) << "='";
    deadlineStream << "+ encodeURIComponent(document.getElementById('"
    << deadlineIds[i] << "').value)+'&'";
  }
  deadlineStream << ")), '" << deadlineIdReport << "', 'setProblemData' );"
  << "\""
  << ">\n";
  deadlineStream << "  Set deadline(s)</button>";
  deadlineStream << "<span id=\"" << deadlineIdReport << "\"></span>";
  deadlineStream << "</td>";
  deadlineStream << "</tr></table>";
//  out << deadlineStream.str();
  out << CGI::GetHtmlSpanHidableStartsHiddeN(deadlineStream.str(), "Set deadline (show/hide). ");
  if (isActualProblem)
    out << "(overrides section deadline). ";
  else
    out << "(overridden by per-problem-deadline). ";
  return out.str();
}

std::string CalculatorHTML::ToStringInterprettedCommands(Calculator &theInterpreter, List<SyntacticElementHTML> &theElements)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringInterprettedCommands");
  std::stringstream out;
  out << "<table>";
  int commandCounter=theInterpreter.theProgramExpression.size()-1;
  for (int eltCounter=theElements.size-1; eltCounter>0; eltCounter--)
  { SyntacticElementHTML& currentElt=theElements[eltCounter];
    std::string currentEltString=currentElt.GetTagClass()+ "[" + currentElt.content.substr(0,10) + "...]";
    std::stringstream currentStream;
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration())
    { out << "<tr><td>" << currentEltString << "</td>"
      << "<td>"
      << theInterpreter.theProgramExpression[commandCounter].ToString()
      << "</td></tr>";
      commandCounter--;
      continue;
    }
    for (; commandCounter>1; commandCounter--)
    { std::string currentString=theInterpreter.theProgramExpression[commandCounter].ToString();
      out << "<tr><td>" << currentEltString << "</td><td>"
      << currentString << "</td></tr>";
      if (currentString=="SeparatorBetweenSpans")
        break;
    }
  }
  out << "</table>";
  return out.str();
}

bool CalculatorHTML::ProcessInterprettedCommands
(Calculator &theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ProcessInterprettedCommands");
  (void) comments;
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal=true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
  theFormat.flagUseQuotes=false;
  theFormat.flagUseLatex=true;
  bool result=true;
  for (int i=0; i<theElements.size; i++)
  { SyntacticElementHTML& currentElt=theElements[i];
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration())
    { currentElt.interpretedCommand="";
      continue;
    }
    if (currentElt.commandIndex>= theInterpreter.theProgramExpression.size() ||
        currentElt.commandIndex<0)
    { std::stringstream errorStream;
      errorStream << "<b>This is a programming error: syntactic element "
      << currentElt.ToStringDebug() << " has wrongly computed commandIndex: "
      << currentElt.commandIndex << ". "
      << "Please report this error to the website admins. </b>";
      currentElt.interpretedCommand=errorStream.str();
      result=false;
      continue;
    }
    if (!theInterpreter.theProgramExpression[currentElt.commandIndex].StartsWith(theInterpreter.opCommandEnclosure()) )
      crash << "Element: " << theInterpreter.theProgramExpression[currentElt.commandIndex].ToString()
      << " in " << theInterpreter.theProgramExpression.ToString()
      << " is supposed to be a command enclosure but apparently isn't. " << crash;
    Expression currentExpr=theInterpreter.theProgramExpression[currentElt.commandIndex][1];
    if (currentExpr.StartsWith(theInterpreter.opEndStatement()) && currentExpr.size()==2)
      currentExpr=currentExpr[1];
    theFormat.flagUseQuotes=false;
    currentElt.interpretedCommand=  currentExpr.ToString(&theFormat);
    currentElt.flagUseDisplaystyleInMathMode= ( currentElt.content.find("\\displaystyle")!=std::string::npos);
    currentElt.flagUseMathMode=true;
    if (currentExpr.IsOfType<std::string> () ||
        currentExpr.IsOfType<Plot>())
    { currentElt.flagUseMathMode=false;
//      stOutput << "<hr>currentExpr: " << currentExpr.ToString() << "is plot or string<hr>";
    }// else
    //{ stOutput << "<hr>currentExpr: " << currentExpr.ToString() << "not of type plot or string";
    //  if (currentExpr.StartsWith(theInterpreter.opEndStatement()))
    //    stOutput << " but is a command list statement";
    //  stOutput << "<hr>";
    //}
  }
  if(theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasProblemComposingRights())
  { std::stringstream streamLog;
    streamLog << "<table border='1'>";
    for (int i=0; i<theInterpreter.theProgramExpression.size(); i++)
    { streamLog << "<tr>";
      for (int j=0; j<this->theContent.size; j++)
        if (this->theContent[j].commandIndex==i)
          streamLog << "<td>" << this->theContent[j].ToStringDebug() << "</td>";
      streamLog << "<td>" << theInterpreter.theProgramExpression[i].ToString()
      << "</td></tr>";
    }
    streamLog << "</table>";
    this->logCommandsProblemGeneration=streamLog.str();
  }
  return result;
}

void CalculatorHTML::FigureOutCurrentProblemList(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::FigureOutCurrentProblemList");
  if (this->flagParentInvestigated)
    return;
  this->flagParentInvestigated=true;
  this->topicListFileName = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
  if (!this->flagIsExamProblem)
  { //stOutput << "NONONO! -Emily";
    return;
  }
  //stOutput << "yesyesyes! -Emily";

  CalculatorHTML parserOfParent;
  parserOfParent.fileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  std::stringstream commentsOfparent;
  if (!parserOfParent.LoadMe(false, commentsOfparent))
  { comments << "Failed to load parent problem collection. Comments: " << commentsOfparent.str();
    return;
  }
  if (!parserOfParent.ParseHTML(commentsOfparent))
  { comments << "Failed to parse parent problem collection. Comments: " << commentsOfparent.str();
    return;
  }
  this->problemListOfParent=parserOfParent.hdProblemList;
}

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  if (!this->ParseHTML(comments))
  { this->outputHtmlBodyNoTag="<b>Failed to interpret html input. </b><br>" +this->ToStringContent();
    this->timeToParseHtml=theGlobalVariables.GetElapsedSeconds()-startTime;
    return false;
  }
  this->timeToParseHtml=theGlobalVariables.GetElapsedSeconds()-startTime;
  this->NumAttemptsToInterpret=0;
  if (!this->flagRandomSeedGiven)
  { srand(time(NULL));
  }
  if (this->flagRandomSeedGiven && this->flagIsForReal)
    this->MaxInterpretationAttempts=1;
  this->randomSeedsIfInterpretationFails.SetSize(this->MaxInterpretationAttempts);
  for (int i=0; i<this->randomSeedsIfInterpretationFails.size; i++)
    this->randomSeedsIfInterpretationFails[i]=rand()%100000000;
  this->timePerAttempt.SetSize(0);
  this->timeIntermediatePerAttempt.SetSize(0);
  this->timeIntermediateComments.SetSize(0);
  while (this->NumAttemptsToInterpret<this->MaxInterpretationAttempts)
  { startTime=theGlobalVariables.GetElapsedSeconds();
    this->timeIntermediatePerAttempt.SetSize(this->timeIntermediatePerAttempt.size+1);
    this->timeIntermediatePerAttempt.LastObject()->SetSize(0);
    this->timeIntermediateComments.SetSize(this->timeIntermediateComments.size+1);
    this->timeIntermediateComments.LastObject()->SetSize(0);
    Calculator theInterpreter;
    this->NumAttemptsToInterpret++;
    if (this->InterpretHtmlOneAttempt(theInterpreter, comments))
    { this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
      return true;
    }
    this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
    if (this->NumAttemptsToInterpret>=this->MaxInterpretationAttempts)
    { std::stringstream out;
      out << "Failed to evaluate the commands: " << this->NumAttemptsToInterpret
      << " attempts made. Calculator evaluation details follow.<hr> "
      << theInterpreter.outputString << "<hr><b>Comments</b><br>"
      << theInterpreter.Comments.str();
      this->outputHtmlBodyNoTag=out.str();
      return false;
    }
  }
  return true;
}

bool CalculatorHTML::IsSplittingChar(const std::string& input)
{ if (input.size()!=1)
    return false;
  return this->splittingChars.Contains(input[0]);
}

int SyntacticElementHTML::ParsingNumDummyElements=8;
std::string CalculatorHTML::ToStringParsingStack(List<SyntacticElementHTML>& theStack)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringParsingStack");
  std::stringstream out;
  out << "#Non-dummy elts: " << theStack.size-SyntacticElementHTML::ParsingNumDummyElements << ". ";
  for (int i=SyntacticElementHTML::ParsingNumDummyElements; i<theStack.size; i++)
  { out << "<span style=\"color:" << ((i%2==0) ? "orange":"blue") << "\">";
    std::string theContent=theStack[i].ToStringDebug();
    if (theContent.size()==0)
      theContent= "<b>empty</b>";
    else if (theContent==" ")
      theContent="_";
    out << theContent << "</span>";
  }
  return out.str();
}

int CalculatorHTML::GetAnswerIndex(const std::string& desiredAnswerId)
{ for (int i=0; i<this->theProblemData.theAnswers.size; i++)
    if (this->theProblemData.theAnswers[i].answerId==desiredAnswerId)
      return i;
  return -1;
}

bool CalculatorHTML::CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::CanBeMerged");
  if (left.syntacticRole!="" || right.syntacticRole!="")
    return false;
  if (this->IsSplittingChar(left.content) && left.content!=" ")
    return false;
  if (this->IsSplittingChar(right.content) && right.content!=" ")
    return false;
  return true;
}

bool CalculatorHTML::ParseHTMLComputeChildFiles(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ParseHTMLComputeChildFiles");
  bool result=true;
  this->hdProblemList.Clear();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
      this->hdProblemList.AddOnTop(this->CleanUpFileName(this->theContent[i].content));
  this->hdHomeworkGroups.SetSize(0);
  this->hdHomeworkGroups.SetExpectedSize(50);
  this->hdHomeworkGroupCorrespondingToEachProblem.SetSize(0);
  this->hdHomeworkGroupCorrespondingToEachProblem.SetExpectedSize(50);
  std::string currentHomeworkGroup;
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamIntermediate")
    { this->hdHomeworkGroups.SetSize(this->hdHomeworkGroups.size+1);
      this->hdHomeworkGroups.LastObject()->SetSize(0);
      currentHomeworkGroup=this->CleanUpFileName(this->theContent[i].content);
      this->hdHomeworkGroupNames.AddOnTop(currentHomeworkGroup);
    } else if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
    { if (this->hdHomeworkGroups.size==0)
      { comments << "<b>Error: found a tag of type calculatorExamProblem before finding the first tag of type calculatorExamIntermediate."
        << " Please make a tag of type calculatorExamIntermediate before making tags of type calculatorExamProblem.</b>";
        result=false;
        continue;
      }
      this->hdHomeworkGroups.LastObject()->AddOnTop(this->CleanUpFileName(this->theContent[i].content));
      this->hdHomeworkGroupCorrespondingToEachProblem.AddOnTop(currentHomeworkGroup);
    }
  return result;
}

bool CalculatorHTML::SetTagClassFromCloseTag(SyntacticElementHTML& output)
{ MacroRegisterFunctionWithName("CalculatorHTML::SetTagClassFromCloseTag");
  std::string& lastTag=output.tag;
  if (lastTag=="head" || lastTag=="HEAD" || lastTag=="Head")
  { output.SetKeyValue("class", "headFinish");
    this->flagTagHeadPresent=true;
    return true;
  } else if (lastTag=="body" || lastTag=="BODY" || lastTag=="Body")
  { output.SetKeyValue("class", "bodyFinish");
    this->flagTagBodyPresent=true;
    return true;
  } else if (lastTag=="html" || lastTag=="HTML" || lastTag=="html")
  { output.SetKeyValue("class", "htmlFinish");
    this->flagTagHtmlPresent=true;
    return true;
  }
  return false;
}

bool CalculatorHTML::SetTagClassFromOpenTag(SyntacticElementHTML& output)
{ MacroRegisterFunctionWithName("CalculatorHTML::SetTagClassFromOpenTag");
  std::string& lastTag=output.tag;
  if (lastTag=="head" || lastTag=="HEAD" || lastTag=="Head")
  { output.SetKeyValue("class", "headStart");
    this->flagTagHeadPresent=true;
    return true;
  } else if (lastTag=="body" || lastTag=="BODY" || lastTag=="Body")
  { output.SetKeyValue("class", "bodyStart");
    this->flagTagBodyPresent=true;
    return true;
  } else if (lastTag=="html" || lastTag=="HTML" || lastTag=="html")
  { output.SetKeyValue("class", "htmlStart");
    this->flagTagHtmlPresent=true;
    return true;
  }
  return false;
}

bool CalculatorHTML::ParseHTML(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ParseHTML");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  std::string word;
  char currentChar;
  List<SyntacticElementHTML> theElements;
  theElements.SetSize(0);
  theElements.SetExpectedSize(theReader.str().size()/4);
  this->splittingChars.AddOnTop('<');
  this->splittingChars.AddOnTop('\"');
  this->splittingChars.AddOnTop('>');
  this->splittingChars.AddOnTop('=');
  this->splittingChars.AddOnTop('/');
  this->splittingChars.AddOnTop(' ');
  while (theReader.get(currentChar))
  { if (splittingChars.Contains(currentChar))
    { if (word!="")
        theElements.AddOnTop(word);
      std::string charToString;
      charToString.push_back(currentChar);
      theElements.AddOnTop(charToString);
      word="";
    } else
      word.push_back(currentChar);
  }
  if (word!="")
    theElements.AddOnTop(word);
  if (this->calculatorClassesAnswerFields.size==0)
  { this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonSubmit");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonInterpret");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonGiveUp");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonSolution");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorMQField");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorMQButtonPanel");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorAnswerVerification");
  }
  if (this->calculatorClasses.size==0)
  { this->calculatorClasses.AddOnTop("calculator");
    this->calculatorClasses.AddOnTop("calculatorSolution");
    this->calculatorClasses.AddOnTop("calculatorShowToUserOnly");
    this->calculatorClasses.AddOnTop("calculatorHidden");
    this->calculatorClasses.AddOnTop("calculatorHiddenIncludeInCommentsBeforeSubmission");
    this->calculatorClasses.AddOnTop("calculatorAnswer");
    this->calculatorClasses.AddOnTop("calculatorAnswerOnGiveUp");
    this->calculatorClasses.AddOnTop("calculatorCommentBeforeSubmission");
    this->calculatorClasses.AddOnTop("calculatorExamIntermediate");
    this->calculatorClasses.AddOnTop("calculatorExamProblem");
    this->calculatorClasses.AddOnTop("calculatorManageClass");
    this->calculatorClasses.AddOnTop("setCalculatorExamProblem");
    this->calculatorClasses.AddOnTop("setCalculatorExamHome");
    this->calculatorClasses.AddOnTop("generateTopicTable");
    this->calculatorClasses.AddOnTop("generateTableOfContents");
    this->calculatorClasses.AddListOnTop(this->calculatorClassesAnswerFields);
  }
  this->eltsStack.SetSize(0);
  SyntacticElementHTML dummyElt, tempElt;
  dummyElt.content="<>";
  dummyElt.syntacticRole="filler";
  tempElt.syntacticRole="command";
  tempElt.tag="";
  tempElt.content="";
  eltsStack.SetExpectedSize(theElements.size+SyntacticElementHTML::ParsingNumDummyElements);
  for (int i=0; i<SyntacticElementHTML::ParsingNumDummyElements; i++)
    eltsStack.AddOnTop(dummyElt);
  int indexInElts=-1;
  bool reduced=false;
  this->flagIsExamProblem=false;
  this->flagIsExamHome=false;
  this->flagTagHeadPresent=false;
  this->flagTagBodyPresent=false;
  this->flagTagHtmlPresent=false;
  std::string tagClass;
  do
  { if (!reduced)
    { indexInElts++;
      if (indexInElts<theElements.size)
        eltsStack.AddOnTop(theElements[indexInElts]);
    }
//    stOutput << "<br>" << this->ToStringParsingStack(eltsStack);
    reduced=true;
    SyntacticElementHTML& last         = eltsStack[eltsStack.size-1];
    SyntacticElementHTML& secondToLast = eltsStack[eltsStack.size-2];
    SyntacticElementHTML& thirdToLast  = eltsStack[eltsStack.size-3];
    SyntacticElementHTML& fourthToLast = eltsStack[eltsStack.size-4];
    SyntacticElementHTML& fifthToLast  = eltsStack[eltsStack.size-5];
    SyntacticElementHTML& sixthToLast  = eltsStack[eltsStack.size-6];
//    SyntacticElementHTML& seventhToLast = eltsStack[eltsStack.size-7];
    if ((secondToLast.syntacticRole=="<openTagCalc>" || secondToLast.syntacticRole=="<calculatorSolution>") &&
        last.syntacticRole=="</closeTag>" && secondToLast.tag==last.tag)
    { secondToLast.syntacticRole="command";
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(secondToLast))
        eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTag" && secondToLast=="/" && last.syntacticRole==">")
    { tagClass=thirdToLast.GetKeyValue("class");
      if (tagClass=="calculatorSolution")
        thirdToLast.syntacticRole="<calculatorSolution>";
      else if (this->calculatorClasses.Contains(tagClass))
        thirdToLast.syntacticRole="command";
      else
      { thirdToLast.content=thirdToLast.ToStringOpenTag(true);
        thirdToLast.resetAllExceptContent();
      }
      eltsStack.RemoveLastObject();
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(thirdToLast))
        eltsStack.RemoveLastObject();
      continue;
    }
    if (last.syntacticRole=="</closeTag>")
    { last.content=last.ToStringCloseTag();
      last.resetAllExceptContent();
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTagCalc>" && secondToLast=="<" && last=="/")
    { secondToLast.syntacticRole="</";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTagCalc>" && secondToLast.syntacticRole=="")
    { thirdToLast.content+=secondToLast.content;
      secondToLast=last;
      eltsStack.SetSize(eltsStack.size-1);
      continue;
    }
    if (secondToLast.syntacticRole!="<openTagCalc>" && last=="<")
    { last.content="";
      last.syntacticRole="<";
      continue;
    }
    if (secondToLast.syntacticRole!="<openTagCalc>" && last==">")
    { last.content="";
      last.syntacticRole=">";
      continue;
    }
    if (secondToLast.syntacticRole=="<" && last!="/")
    { secondToLast.tag=last.content;
      secondToLast.syntacticRole="<openTag";
      secondToLast.content="";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="" && secondToLast!="\"" && last=="/")
    { secondToLast.content+=last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="<" && last=="/")
    { secondToLast.syntacticRole="</";
      secondToLast.tag="";
      secondToLast.content="";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="</" )
    { secondToLast.syntacticRole="</closeTag";
      secondToLast.tag=last.content;
      secondToLast.content="";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="</closeTag" && last.syntacticRole==">")
    { secondToLast.syntacticRole="</closeTag>";
      if (this->SetTagClassFromCloseTag(secondToLast))
        secondToLast.syntacticRole="command";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (last==" " &&
        (secondToLast.syntacticRole=="<openTag" || thirdToLast.syntacticRole=="<openTag"
         || fourthToLast.syntacticRole=="<openTag"))
    { eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTag" && secondToLast.syntacticRole=="" && last.syntacticRole=="" &&
        secondToLast!="=" && secondToLast!="\"" && last!="=" && last!="\"")
    { thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size-2]=*eltsStack.LastObject();
      eltsStack.RemoveLastObject();
      continue;
    }
    if (this->CanBeMerged(secondToLast, last))
    { secondToLast.content+=last.content;
      eltsStack.RemoveLastObject();
      //stOutput << "<hr>Merge<hr>";
      continue;
    }
    if (thirdToLast.syntacticRole=="<calculatorSolution>" &&
        (secondToLast.syntacticRole=="" || secondToLast.syntacticRole=="command"))
    { thirdToLast.children.AddOnTop(secondToLast);
      eltsStack[eltsStack.size-2]=last;
      eltsStack.RemoveLastObject();
      //stOutput << "<hr>Rule 2: processed " << thirdToLast.ToStringOpenTag(true) << "<hr>";
      continue;
    }
    if (sixthToLast.syntacticRole=="<openTag" && fourthToLast=="=" && thirdToLast=="\""
        && last!="\"" )
    { if (last.syntacticRole!="" && last.content=="")
        secondToLast.content+=last.syntacticRole;
      else
        secondToLast.content+=last.content;
      eltsStack.RemoveLastObject();
      //stOutput << "<hr>Rule X executed<hr> ";
      continue;
    }
    if (thirdToLast=="\"" && secondToLast!="\"" && last!="\"")
    { if (secondToLast.syntacticRole!="" && secondToLast.content=="")
        secondToLast.content=secondToLast.syntacticRole;
      if (last.syntacticRole!="" && last.content=="")
        last.content=last.syntacticRole;
      secondToLast.content+=last.content;
      //stOutput << "<hr>Rule quote executed<hr> ";

      eltsStack.RemoveLastObject();
      continue;
    }
    if (sixthToLast.syntacticRole=="<openTag" && fourthToLast=="=" && thirdToLast=="\""
        && last=="\"" )
    { sixthToLast.SetKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.SetSize(eltsStack.size-5);
      //stOutput << "<hr>Rule ZZ executed<hr> ";
      continue;
    }
    if (thirdToLast=="\"" && secondToLast!="\"" && last=="\"")
    { thirdToLast.content+=secondToLast.content;
      thirdToLast.content+=last.content;
      eltsStack.SetSize(eltsStack.size-2);
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTag" && secondToLast.syntacticRole=="" && last.syntacticRole==">")
    { thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size-2]=*eltsStack.LastObject();
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="<openTag" && last.syntacticRole==">")
    { //stOutput << "<hr>DEBUG: " << secondToLast.ToStringDebug() << " class key value: " << secondToLast.GetKeyValue("class");
      tagClass=secondToLast.GetKeyValue("class");
      if (tagClass=="calculatorSolution")
        secondToLast.syntacticRole="<calculatorSolution>";
      else if (this->calculatorClasses.Contains(tagClass))
        secondToLast.syntacticRole="<openTagCalc>";
      else if (this->SetTagClassFromOpenTag(secondToLast))
        secondToLast.syntacticRole="command";
      else
      { secondToLast.content=secondToLast.ToStringOpenTag();
        secondToLast.resetAllExceptContent();
      }
      //stOutput << "<hr>Rule 3: processed: " << secondToLast.ToStringDebug() << "<hr>";
      eltsStack.RemoveLastObject();
      continue;
    }
    reduced=false;
  } while (reduced || indexInElts<theElements.size);
  this->theContent.SetSize(0);
  bool result=true;
  for (int i=SyntacticElementHTML::ParsingNumDummyElements; i<eltsStack.size; i++)
  { bool needNewTag=false;
    if (i==SyntacticElementHTML::ParsingNumDummyElements)
      needNewTag=true;
    else if (this->theContent.LastObject()->syntacticRole!="")
      needNewTag=true;
    if (eltsStack[i].syntacticRole!="")
      needNewTag=true;
    if (eltsStack[i].GetTagClass()=="calculatorAnswer")
      if (eltsStack[i].GetKeyValue("mqMatrices")=="true")
        this->flagMathQuillWithMatrices=true;
    if (eltsStack[i].syntacticRole!="command" && eltsStack[i].syntacticRole!="" )
    { result=false;
      comments << "<br>Syntactic element: " << eltsStack[i].ToStringDebug()
      << " is not a command but has non-empty syntactic role.";
    }
    if (!needNewTag)
      this->theContent.LastObject()->content+=eltsStack[i].content;
    else
    { if (this->theContent.size>0)
        if (this->theContent.LastObject()->IsInterpretedByCalculatorDuringProblemGeneration() &&
            eltsStack[i].IsInterpretedByCalculatorDuringProblemGeneration())
        { SyntacticElementHTML emptyElt;
          this->theContent.AddOnTop(emptyElt);
        }
      this->theContent.AddOnTop(eltsStack[i]);
    }
  }
  if (!result)
    comments << "<hr>Parsing stack.<hr>" << this->ToStringParsingStack(this->eltsStack);
  if (!this->ParseHTMLComputeChildFiles(comments))
    result=false;
  if (result)
    result=this->ExtractAnswerIds(comments);
  for (int i=0; i<this->theContent.size; i++)
    this->theContent[i].indexInOwner=i;
  if (result)
    result=this->CheckContent(comments);
  return result;
}

bool CalculatorHTML::InterpretOneAnswerElement(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretOneAnswerElement");
  std::string answerId;
  if (!inputOutput.IsAnswerElement(&answerId))
    return true;
  int theIndex=this->GetAnswerIndex(answerId);
  std::string tagClass=inputOutput.GetTagClass();
  if (theIndex==-1)
  { std::stringstream out;
    out << "<b>Element of class " << tagClass << " has name: "
    << answerId << " but that does not match any answerId value. "
    << this->theProblemData.ToStringAvailableAnswerIds() << ". </b>";
    inputOutput.interpretedCommand=out.str();
    return true;
  }
  Answer& currentA=this->theProblemData.theAnswers[theIndex];
  if (tagClass=="calculatorButtonInterpret")
    inputOutput.interpretedCommand=currentA.htmlButtonInterpret;
  if (tagClass=="calculatorButtonGiveUp")
    inputOutput.interpretedCommand=currentA.htmlButtonAnswer;
  if (tagClass=="calculatorButtonSolution")
    inputOutput.interpretedCommand=currentA.htmlButtonSolution;
  if (tagClass=="calculatorMQField")
    inputOutput.interpretedCommand=currentA.htmlSpanMQfield;
  if (tagClass=="calculatorMQButtonPanel")
    inputOutput.interpretedCommand=currentA.htmlSpanMQButtonPanel;
  if (tagClass=="calculatorAnswerVerification")
    inputOutput.interpretedCommand=currentA.htmlSpanVerifyAnswer;
  if (tagClass=="calculatorButtonSubmit")
    inputOutput.interpretedCommand=currentA.htmlButtonSubmit;
  if (tagClass=="calculatorSolution")
    inputOutput.interpretedCommand=currentA.htmlSpanSolution;
  return true;
}

bool CalculatorHTML::InterpretAnswerElements(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretAnswerElements");
  (void) comments;
  for (int i=0; i<this->theContent.size; i++)
    this->InterpretOneAnswerElement(this->theContent[i]);
  return true;
}

bool CalculatorHTML::PrepareAnswerElements(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareAnswerElements");
  (void) comments;
  std::string currentId;
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsAnswerElement(& currentId))
    { int index=this->GetAnswerIndex(currentId);
      if (index==-1)
        continue;
      Answer& currentA=this->theProblemData.theAnswers[index];
      std::string tagClass=this->theContent[i].GetTagClass();
      if (tagClass=="calculatorButtonSubmit" ||
          tagClass=="calculatorButtonInterpret" ||
          tagClass=="calculatorButtonGiveUp")
        currentA.flagAutoGenerateSubmitButtons=false;
      if (tagClass=="calculatorButtonSolution")
        currentA.flagAutoGenerateButtonSolution=false;
      if (tagClass=="calculatorSolution")
        currentA.flagSolutionFound=true;
      if (tagClass=="calculatorMQField")
        currentA.flagAutoGenerateMQfield=false;
      if (tagClass=="calculatorMQButtonPanel")
        currentA.flagAutoGenerateMQButtonPanel=false;
      if (tagClass=="calculatorAnswerVerification")
        currentA.flagAutoGenerateVerificationField=false;
    }
  return true;
}

bool CalculatorHTML::ExtractAnswerIds(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ExtractAnswerIds");
  //we shouldn't clear this->theProblemData.theAnswers: it may contain
  //outdated information loaded from the database. We don't want to loose that info
  //(say we renamed an answerId but students have already stored answers using the old answerId...).
  List<std::string> answerIdsSeenSoFar;
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentE=this->theContent[i];
    if (currentE.IsAnswer())
    { std::string currentId=currentE.GetKeyValue("id");
      int theIndex=this->GetAnswerIndex(currentId);
      if (theIndex==-1)
        this->theProblemData.AddEmptyAnswerIdOnTop(currentId);
      if (answerIdsSeenSoFar.Contains(currentId))
      { comments << "<b>Answer with id: "
        << currentId << " contained more than once. </b>";
        return false;
      }
      answerIdsSeenSoFar.AddOnTopNoRepetition(currentId);
      continue;
    }
    if (!currentE.IsCommentBeforeSubmission() &&
        !currentE.IsCommentAfterSubmission() &&
        !currentE.IsAnswerOnGiveUp() &&
        !currentE.IsSolution())
      continue;
    if (answerIdsSeenSoFar.size==0 && currentE.GetKeyValue("name")=="")
    { comments << "<b>Auxilary answer element: " << currentE.ToStringDebug()
      << " has no name and appears before the first answer tag."
      << " Auxilary answers apply the answer tag whose id is specified in the name"
      << " tag of the auxilary answer. If the auxilary answer has no "
      << " name tag, it is assumed to apply to the (nearest) answer tag above it."
      << " To fix the issue either place the auxilary element after the answer or "
      << " specify the answer's id in the name tag of the auxilary element. "
      << "</b>";
      return false;
    }
    if (currentE.GetKeyValue("name")=="")
      currentE.SetKeyValue("name", *answerIdsSeenSoFar.LastObject());
//    stOutput << "<hr>Debug: processed element " << currentE.ToStringDebug();
  }
//  stOutput << "<hr>DEBUG: Elements: " << this->ToStringContent() << "<hr>";
  return true;
}

bool CalculatorHTML::CheckContent(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::CheckContent");
  bool result=true;
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (currentElt.syntacticRole=="command" && currentElt.IsAnswer() &&
        currentElt.GetKeyValue("id").find('=')!=std::string::npos)
    { result=false;
      comments << "Error: the id of tag " << currentElt.ToStringDebug() << " contains the equality sign which is not allowed. ";
    }
  }
  return result;
}

std::string CalculatorHTML::CleanUpCommandString(const std::string& inputCommand)
{ MacroRegisterFunctionWithName("CalculatorHTML::CleanUpCommandString");
  if (inputCommand=="")
    return "";
//  stOutput << "Cleaning up: " << inputCommand;
  int realStart=0;
  int realEnd=inputCommand.size()-1;
  for (; realStart< (signed) inputCommand.size(); realStart++)
  { if (inputCommand[realStart]==' ' || inputCommand[realStart]=='\n')
      continue;
    if (inputCommand[realStart]=='\\')
      if (realStart+1<(signed) inputCommand.size())
        if (inputCommand[realStart+1]=='(')
        { realStart++;
          continue;
        }
    break;
  }
  for (;realEnd >=0; realEnd--)
  { if (inputCommand[realEnd]==' ' || inputCommand[realEnd]=='\n')
      continue;
    if (inputCommand[realEnd]==')')
      if (realEnd>0)
        if (inputCommand[realEnd-1]=='\\')
        { realEnd--;
          continue;
        }
    break;
  }
  if (realEnd<realStart)
    realEnd=realStart-1;
  std::string result=inputCommand.substr(realStart, realEnd-realStart+1);
  for (int i=(signed)result.size()-1; i>=0; i--)
  { if (result[i]==' ' || result[i]=='\n')
      continue;
    if (result[i]==';')
    { //stOutput << "to get: " << result;
      return result;
    }
    break;
  }
  if (result=="")
    return "";
  result.push_back(';');
  //stOutput << " to get: " << result << "<br>";
  return result;
}

bool CalculatorHtmlFunctions::innerExtractCalculatorExpressionFromHtml
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExtractCalculatorExpressionFromHtml");
  CalculatorHTML theFile;
  if (!input.IsOfType<std::string>(&theFile.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  if (!theFile.ParseHTML(theCommands.Comments))
    return false;
  return output.AssignValue(theFile.ToStringExtractedCommands(), theCommands);
}

std::string CalculatorHTML::GetJavascriptMathQuillBoxes()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetJavascriptMathQuillBoxes");
  std::stringstream out;
  ////////////////////////////////////////////////////////////////////
  out << "<script type=\"text/javascript\">\n";
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
  for (int answerCounter=0; answerCounter<this->theProblemData.theAnswers.size; answerCounter++)
  { Answer& currentA=this->theProblemData.theAnswers[answerCounter];
    out << "var " << currentA.varMQfield << ";\n";
    out << "var " << currentA.varAnswerId << ";\n";
    out
    << "function " << currentA.MQUpdateFunction << "(){\n"
    << "ignoreNextMathQuillUpdateEvent=true;\n"
    << currentA.MQobject << ".latex(" << currentA.varAnswerId << ".value+' ');\n"
    //<< "alert('writing: ' +" << currentA.varAnswerId  << ".value);\n"
    //<< currentA.MQobject << ".latex(" << currentA.varAnswerId << ".value);\n"
    << "ignoreNextMathQuillUpdateEvent=false;\n"
    << "}\n";
  }
  out
    << "var ignoreNextMathQuillUpdateEvent=false;\n"
    << "function initializeMathQuill(){\n";

  for (int answerCounter=0; answerCounter<this->theProblemData.theAnswers.size; answerCounter++)
  { Answer& currentA=this->theProblemData.theAnswers[answerCounter];
    out << "////////////////////////\n";
    out << currentA.varMQfield  << " = document.getElementById('" << currentA.idMQfield << "');\n"
    << currentA.varAnswerId << " = document.getElementById('" << currentA.answerId << "');\n"
    << "globalMQ.config({\n"
    << "  autoFunctionize: 'sin cos tan sec csc cot log ln'\n"
    << "  });\n"
    << currentA.MQobject << " = globalMQ.MathField(" << currentA.varMQfield << ", {\n"
    << "spaceBehavesLikeTab: true, // configurable\n"
    << "handlers: {\n"
    << "edit: function() { // useful event handlers\n"
    << "if (ignoreNextMathQuillUpdateEvent){\n"
  //  << "  ignoreNextMathQuillUpdateEvent=false;\n"
    << "  return;\n"
    << "}\n"
    << currentA.varAnswerId << ".value = " << currentA.MQobject << ".latex(); // simple API\n"
    << currentA.javascriptPreviewAnswer
    << "}\n"
    << "}\n"
    << "});\n";
  }
  out << "}//closing initializeMathQuill\n";
  out << "</script>";
  return out.str();
}

#endif
