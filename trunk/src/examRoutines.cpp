//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader1General5TimeDate.h"
#include "vpfHeader8HtmlInterpretation.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCustomCode
(__FILE__, "Routines for calculus teaching: calculator exam mode. Shared code. ");

std::string CalculatorHTML::stringScoredQuizzes="Scored Quizzes";
std::string CalculatorHTML::stringPracticE="Practice";
std::string CalculatorHTML::stringProblemLink="Problem link";

CalculatorHTML::CalculatorHTML()
{ this->NumAttemptsToInterpret=0;
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
  this->flagSectionsPrepared=false;
}

#ifdef MACRO_use_MySQL
bool CalculatorHTML::ReadProblemInfoAppend
  (const std::string& inputInfoString,
   MapLisT<std::string, ProblemData, MathRoutines::hashString>&
   outputProblemInfo,
   std::stringstream& commentsOnFailure
   )
{ MacroRegisterFunctionWithName("DatabaseRoutines::ReadProblemInfoAppend");
  MapLisT<std::string, std::string, MathRoutines::hashString>
  CGIedProbs, currentKeyValues, sectionDeadlineInfo, sectionProblemInfo;
  if (!CGI::ChopCGIString(inputInfoString, CGIedProbs, commentsOnFailure) )
    return false;
  //stOutput << "<hr>Debug: reading problem info from: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(inputInfoString);
  outputProblemInfo.SetExpectedSize(outputProblemInfo.size()+ CGIedProbs.size());
  std::string currentProbName, currentProbString;
  ProblemData emptyData;
  for (int i=0; i<CGIedProbs.size(); i++)
  { currentProbName=MathRoutines::StringTrimWhiteSpace(CGI::URLStringToNormal(CGIedProbs.theKeys[i]));
    if (currentProbName=="")
      continue;
    currentProbString=CGI::URLStringToNormal(CGIedProbs.theValues[i]);
    if (!outputProblemInfo.Contains(currentProbName))
      outputProblemInfo.GetValueCreateIfNotPresent(currentProbName)=emptyData;
    ProblemData& currentProblemValue=
    outputProblemInfo.GetValueCreateIfNotPresent(currentProbName);
    if (!CGI::ChopCGIString(currentProbString, currentKeyValues, commentsOnFailure))
      return false;
    //stOutput << "<hr>Debug: reading problem info from: " << currentProbString << " resulted in pairs: "
    //<< currentKeyValues.ToStringHtml();
    std::string deadlineString=CGI::URLStringToNormal(currentKeyValues.GetValueCreateIfNotPresent("deadlines"));
    std::string problemWeightsCollectionString=CGI::URLStringToNormal(currentKeyValues.GetValueCreateIfNotPresent("weights"));
    if (problemWeightsCollectionString!="")
    { if (!CGI::ChopCGIString(problemWeightsCollectionString, sectionProblemInfo, commentsOnFailure))
        return false;
      for (int j=0; j<sectionProblemInfo.size(); j++)
        currentProblemValue.adminData.problemWeightsPerSectionDB.SetKeyValue
        (CGI::URLStringToNormal(sectionProblemInfo.theKeys[j]),
         CGI::URLStringToNormal(sectionProblemInfo.theValues[j]));
    }
    if (deadlineString!="")
    { if (!CGI::ChopCGIString(deadlineString, sectionDeadlineInfo, commentsOnFailure))
        return false;
      for (int j=0; j<sectionDeadlineInfo.size(); j++)
        currentProblemValue.adminData.deadlinesPerSection.SetKeyValue
        (CGI::URLStringToNormal(sectionDeadlineInfo.theKeys[j]),
         CGI::URLStringToNormal(sectionDeadlineInfo.theValues[j]));
    }
    std::string problemWeightNoSectionsString=MathRoutines::StringTrimWhiteSpace
    (currentKeyValues.GetValueCreateIfNotPresent("weight"));
    if (problemWeightNoSectionsString!="")
    { if (this->databaseStudentSections.size<=0)
      { commentsOnFailure << "<span style=\"color:red\"><b>Got default weight: "
        << problemWeightNoSectionsString << " but "
        << "user does not appear to be teaching any sections.</b></span> ";
        return false;
      }
      for (int j=0; j<this->databaseStudentSections.size; j++)
        currentProblemValue.adminData.problemWeightsPerSectionDB.SetKeyValue
        (CGI::URLStringToNormal(this->databaseStudentSections[j]),
         CGI::URLStringToNormal(problemWeightNoSectionsString));
    }

  }
  return true;
}

void CalculatorHTML::StoreProblemWeightInfo
(std::string& outputString,
 MapLisT<std::string, ProblemData, MathRoutines::hashString>&
 inputProblemInfo)
{ MacroRegisterFunctionWithName("CalculatorHTML::StoreProblemWeightInfo");
  std::stringstream out;
  for (int i=0; i<inputProblemInfo.size(); i++)
  { ProblemDataAdministrative& currentProblem=inputProblemInfo.theValues[i].adminData;
    if (currentProblem.problemWeightsPerSectionDB.size()==0)
      continue;
    std::string currentProbName=inputProblemInfo.theKeys[i];
    std::stringstream currentProblemStream, currentWeightStream;
    for (int j=0; j<currentProblem.problemWeightsPerSectionDB.size(); j++)
    { std::string currentWeight=MathRoutines::StringTrimWhiteSpace
      (currentProblem.problemWeightsPerSectionDB.theValues[j]);
      if (currentWeight=="")
        continue;
      std::string currentSection=MathRoutines::StringTrimWhiteSpace
      (currentProblem.problemWeightsPerSectionDB.theKeys[j]);
      currentWeightStream
      << CGI::StringToURLString(currentSection)
      << "="
      << CGI::StringToURLString(currentWeight)
      << "&";
    }
    currentProblemStream << "weights=" << CGI::StringToURLString(currentWeightStream.str()) << "&";
    out << CGI::StringToURLString(currentProbName) << "="
    << CGI::StringToURLString(currentProblemStream.str()) << "&";
  }
  outputString= out.str();
}

void CalculatorHTML::StoreDeadlineInfo
(std::string& outputString,
 MapLisT<std::string, ProblemData, MathRoutines::hashString>&
 inputProblemInfo)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreDeadlineInfo");
  std::stringstream out;
  for (int i=0; i<inputProblemInfo.size(); i++)
  { ProblemDataAdministrative& currentProblem=inputProblemInfo.theValues[i].adminData;
    if (currentProblem.deadlinesPerSection.size()==0)
      continue;
    std::string currentProbName=inputProblemInfo.theKeys[i];
    std::stringstream currentProblemStream, currentDeadlineStream;
    for (int j=0; j<currentProblem.deadlinesPerSection.size(); j++)
    { std::string currentDeadline=MathRoutines::StringTrimWhiteSpace(currentProblem.deadlinesPerSection.theValues[j]);
      if (currentDeadline=="")
        continue;
      std::string currentSection=MathRoutines::StringTrimWhiteSpace(currentProblem.deadlinesPerSection.theKeys[j]);
      currentDeadlineStream << CGI::StringToURLString(currentSection)
      << "="
      << CGI::StringToURLString(currentDeadline)
      << "&";
    }
    currentProblemStream << "deadlines=" << CGI::StringToURLString(currentDeadlineStream.str()) << "&";
    out << CGI::StringToURLString(currentProbName) << "="
    << CGI::StringToURLString(currentProblemStream.str()) << "&";
  }
  outputString= out.str();
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
(const UserCalculatorData& theUser, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  //stOutput << "<hr>DEBUG: About to store back: "
  //<< "<br>deadline:<br> "
  //<< theUser.deadlineInfoString.value
  //<< "<br>problem:<br> "
  //<< theUser.problemInfoString.value
  //<< "<br> problem row id: " << theUser.problemInfoRowId.value << "<hr>";
  //;
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
    return false;
  if (!this->SetEntry
      (DatabaseStrings::deadlinesIdColumnName,
       theUser.deadlineInfoRowId,
       DatabaseStrings::deadlinesTableName,
       DatabaseStrings::infoColumnInDeadlinesTable,
       theUser.deadlineInfoString, &commentsOnFailure))
    return false;
  if (!this->SetEntry
      (DatabaseStrings::problemWeightsIdColumnName,
       theUser.problemInfoRowId,
       DatabaseStrings::problemWeightsTableName,
       DatabaseStrings::infoColumnInProblemWeightsTable,
       theUser.problemInfoString, &commentsOnFailure))
    return false;
  return true;
}

bool CalculatorHTML::MergeOneProblemAdminData
(const std::string& inputProblemName, ProblemData& inputProblemInfo,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::MergeOneProblemAdminData");
  if (!this->theTopicS.Contains(inputProblemName))
  { commentsOnFailure << "Did not find " << inputProblemName
    << " among the list of topics/problems. ";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
      commentsOnFailure << "The topics are: " << this->theTopicS.ToStringHtml();
    //stOutput << "DEBUG: NOT merging " << inputProblemName << ": topics list does not contain it. "
    //<< inputProblemInfo.ToString();
    return false;
  }
  //stOutput << "Debug: MERGING-in prob data: " << inputProblemInfo.ToString();
  if (!this->currentUseR.theProblemData.Contains(inputProblemName))
    this->currentUseR.theProblemData.SetKeyValue(inputProblemName, inputProblemInfo);
  ProblemDataAdministrative& currentProblem=
  this->currentUseR.theProblemData.GetValueCreateIfNotPresent(inputProblemName).adminData;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  currentDeadlines=currentProblem.deadlinesPerSection;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  incomingDeadlines=inputProblemInfo.adminData.deadlinesPerSection;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  currentWeights=currentProblem.problemWeightsPerSectionDB;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  incomingWeights=inputProblemInfo.adminData.problemWeightsPerSectionDB;

  for (int i=0; i<incomingDeadlines.size(); i++)
  { if (this->databaseStudentSections.size>=1000)
    { commentsOnFailure << "Failed to account deadlines: max 999 sections allowed. ";
      return false;
    }
    this->databaseStudentSections.AddOnTopNoRepetition(incomingDeadlines.theKeys[i]);
  }
  ////////////////////////////////////////////
  for (int i=0; i<incomingWeights.size(); i++)
  { if (this->databaseStudentSections.size>=1000)
    { commentsOnFailure << "Failed to account deadlines: max 999 sections allowed. ";
      return false;
    }
    this->databaseStudentSections.AddOnTopNoRepetition(incomingWeights.theKeys[i]);
  }

  for (int i=0; i<incomingDeadlines.size(); i++)
    currentDeadlines.SetKeyValue(incomingDeadlines.theKeys[i], incomingDeadlines.theValues[i]);
  for (int i=0; i<incomingWeights.size(); i++)
    currentWeights.SetKeyValue(incomingWeights.theKeys[i], incomingWeights.theValues[i]);
  return true;
}

bool CalculatorHTML::MergeProblemInfoInDatabase
(std::string& incomingProblemInfo,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::MergeProblemInfoInDatabase");
  //stOutput << "DEBUG: Here I am, merging in data: " << incomingProblemInfo;
  MapLisT<std::string, ProblemData, MathRoutines::hashString>
  incomingProblems;
//  stOutput << "<hr>DEBUG: merging problem info with stack trace: "
//  << crash.GetStackTraceEtcErrorMessage()
  //<< incomingProblemInfo
//  ;
  if (!this->ReadProblemInfoAppend(incomingProblemInfo, incomingProblems, commentsOnFailure))
  { commentsOnFailure << "Failed to parse your request";
    return false;
  }
  bool result=true;
  //stOutput << "<hr><hr>Debug: incoming problems: " << incomingProblems.ToStringHtml();
  for (int i=0; i<incomingProblems.size(); i++)
    if (!this->MergeOneProblemAdminData
        (incomingProblems.theKeys[i], incomingProblems.theValues[i], commentsOnFailure))
      result=false;
  //stOutput << "<hr><hr>Debug: after merge, resulting MERGED probs: "
  //<< this->currentUseR.theProblemData.ToStringHtml() << "<hr>";
  this->StoreDeadlineInfo
  (theGlobalVariables.userDefault.deadlineInfoString.value,
   this->currentUseR.theProblemData);
  //stOutput << "<hr>Debug: about to store WEIGHT with row id: "
  //<< this->currentUseR.problemInfoRowId.value << "<hr>";
  //stOutput << "<hr>About to transform to database string: "
  //<< this->currentUseR.theProblemData.ToStringHtml()
  //<< "<hr>";

  this->StoreProblemWeightInfo
  (theGlobalVariables.userDefault.problemInfoString.value,
   this->currentUseR.theProblemData);
  //theGlobalVariables.userDefault=this->currentUseR;
  //stOutput << "<hr>Resulting string: "
  //<< theGlobalVariables.userDefault.problemInfoString.value
  //<< "<hr>";
  DatabaseRoutines theRoutines;
  if (!theRoutines.StoreProblemDatabaseInfo(theGlobalVariables.userDefault, commentsOnFailure))
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
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBug: got to loadDB info.<hr>";
  if (this->currentUseR.problemDataString=="")
    return true;
  //stOutput << "<hr>DEBug: continuing to load ....<hr>";
  if (! this->PrepareSectionList(comments))
    return false;
  //this->theProblemData.CheckConsistency();
//  stOutput << "<hr>DEBug: got to before InterpretDatabaseProblemData.<hr>";
  if (!this->currentUseR.InterpretDatabaseProblemData(this->currentUseR.problemDataString.value, comments))
  { comments << "Failed to interpret user's problem saved data. ";
    //stOutput << "Failed to interpret user's problem saved data. ";
    return false;
  }
  //stOutput << "<hr>DEBUG: After interpretation of datastring: user: " << this->currentUseR.ToString();
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBug: got to before this->currentUseR.theProblemData.Contains.<hr>";
  //stOutput << this->currentUseR.theProblemData.ToStringHtml() << "<hr><hr>";
  if (this->currentUseR.theProblemData.Contains(this->fileName))
  { this->theProblemData=this->currentUseR.theProblemData.GetValueCreateIfNotPresent(this->fileName);
    //stOutput << "<hr>Debug: found problem data! " << this->theProblemData.ToString() << "<hr>";
  } //else
    //stOutput << "<hr>Did not find problem data for filename: " << this->fileName << ". USer details: " << this->currentUseR.ToString() << "<hr>";
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr><hr>DEBug: got to before read prob append.";
  if (!this->ReadProblemInfoAppend(this->currentUseR.deadlineInfoString.value, this->currentUseR.theProblemData, comments))
  { comments << "Failed to interpret the deadline string. ";
    return false;
  }
  //stOutput << "<hr>Debug: reading problem data from: " << CGI::URLKeyValuePairsToNormalRecursiveHtml( this->currentUseR.problemInfoString.value);
  if (!this->ReadProblemInfoAppend(this->currentUseR.problemInfoString.value, this->currentUseR.theProblemData, comments))
  { comments << "Failed to interpret the problem weight string. ";
    return false;
  }
  //stOutput << "<hr>Debug: user: " << this->currentUseR.ToString();
  //stOutput << "<hr><hr>DEBUG read databaseProblemAndHomeworkGroupList: "
  //<< this->databaseProblemAndHomeworkGroupList;
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBUG: before computing points ...";
  this->currentUseR.ComputePointsEarned(this->currentUseR.theProblemData.theKeys, 0);
  //stOutput << "<hr>DEBUG: after computing points ...";
  this->theProblemData.CheckConsistency();
  theGlobalVariables.userDefault=this->currentUseR;
  //stOutput << "<hr>DEBUG: After interpretation of deadline and weight strings: user: " << this->currentUseR.ToString();
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
  //stOutput << "Debug: got to here pt1";
  std::stringstream contentStream;
  contentStream << theFile.rdbuf();
  this->inputHtml=contentStream.str();
  this->flagIsForReal=theGlobalVariables.UserRequestRequiresLoadingRealExamData();
#ifdef MACRO_use_MySQL
  this->topicListFileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
//  stOutput << "Debug: got to here pt2";
//  this->theProblemData.CheckConsistency();
//  stOutput << "Debug: got to here pt3";
  if (doLoadDatabase)
    this->LoadDatabaseInfo(comments);
#endif // MACRO_use_MySQL
//  stOutput << "<hr>DEBUG: Loaded successfully, checking: <hr>";
  this->theProblemData.CheckConsistency();
//  stOutput << "<hr>OK<hr> ";

  //stOutput << "DEBUG: flagIsForReal: " << this->flagIsForReal;
  if (!this->flagIsForReal)
  { std::string randString= theGlobalVariables.GetWebInput("randomSeed");
    if (randString!="")
    { std::stringstream randSeedStream(randString);
      //stOutput << "radSeedStream: " << randString;
      randSeedStream >> this->theProblemData.randomSeed;
      //stOutput << ", interpreted as: " << this->theProblemData.randomSeed;
      this->theProblemData.flagRandomSeedGiven=true;
    }
  }
  //  stOutput << "debug info: rand seed after all's been said and done: " << this->theProblemData.randomSeed;
  return true;
}

std::string CalculatorHTML::LoadAndInterpretCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadAndInterpretCurrentProblemItem ");
  double startTime=theGlobalVariables.GetElapsedSeconds();
//  this->theProblemData.CheckConsistency();
  this->LoadCurrentProblemItem();
//  this->theProblemData.CheckConsistency();
  if (!this->flagLoadedSuccessfully)
    return this->comments.str();
  std::stringstream out;
  if (!this->InterpretHtml(this->comments))
  { if (theGlobalVariables.UserDefaultHasAdminRights())
      out << this->GetEditPageButton(this->fileName) << "<br>";
    out << "<b>Failed to interpret file: " << this->fileName << "</b>. Comments: " << this->comments.str();
    return out.str();
  }
  //out << "DEBUG: flagMathQuillWithMatrices=" << this->flagMathQuillWithMatrices << "<br>";
  if (this->flagUseNavigationBar && !theGlobalVariables.flagRunningApache)
  { out << "<problemNavigation>"
    << this->outputHtmlNavigatioN
    << theGlobalVariables.ToStringNavigation()
    << "<small>Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds()-startTime)
    << " second(s).</small>" << "</problemNavigation>\n";
   }
  out << this->outputHtmlBodyNoTag;
  return out.str();
}

void CalculatorHTML::LoadFileNames()
{ this->fileName = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"));
  this->courseHome = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("courseHome"));
  this->topicListFileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"));
}

void CalculatorHTML::LoadCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->LoadFileNames();
  this->flagLoadedSuccessfully=false;
  bool needToLoadDatabase=true;
  if (theGlobalVariables.UserGuestMode())
    needToLoadDatabase=false;
  if (theGlobalVariables.flagRunningApache)
    this->flagUseNavigationBar=(theGlobalVariables.GetWebInput("navigationBar")=="true");
  else
    this->flagUseNavigationBar=true;
  this->flagLoadedSuccessfully=true;
  if (this->fileName=="")
  { this->flagLoadedSuccessfully=false;
    this->comments << "<b>No problem file name found. </b>";
  }

//  this->theProblemData.CheckConsistency();
//  stOutput << "<hr>DEBUG: got to before loading<hr>";
  if (!this->LoadMe(needToLoadDatabase, this->comments))
    this->flagLoadedSuccessfully =false;
//  stOutput << "<hr>DEBUG: loaded<hr>";
//  stOutput << "<hr>DEBUG: OK<hr>";
  if (!this->flagLoadedSuccessfully)
    this->comments << "<a href=\"/selectCourse.html\">Go to course list page.</a>";
  this->theProblemData.CheckConsistency();
}

bool CalculatorHTML::IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt)
{ MacroRegisterFunctionWithName("CalculatorHTML::IsStateModifierApplyIfYes");
  if (inputElt.syntacticRole!="command")
    return false;
  std::string tagClass=inputElt.GetKeyValue("class");
  if (tagClass=="setCalculatorExamHome")
  { this->flagIsExamHome=true;
    this->flagIsExamProblem=false;
    theGlobalVariables.SetWebInpuT("courseHome", CGI::StringToURLString(this->fileName));
  }
  if (tagClass=="setCalculatorExamProblem")
  { this->flagIsExamHome=false;
    this->flagIsExamProblem=true;
    return true;
  }
  return false;
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
  << "  }, 4000);"
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
  out //<< "<span class=\"calculatorExamProblem\">\n"
  << "Title: " << theFileName << "\n"
  << "Problem: " << theFileName << "\n"
  //<< "\n</span>"
  ;
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  this->NumProblemsFound++;
//  stOutput << "Figuring out current prob list ...";
//  std::stringstream notUsed;
//  this->FigureOutCurrentProblemList(notUsed);
  inputOutput.interpretedCommand= this->ToStringProblemInfo(this->CleanUpFileName(inputOutput.content));
}

std::string CalculatorHTML::ToStringLinkFromFileName(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkFromFileName");
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
  std::string urledProblem=CGI::StringToURLString(theFileName);
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
  << "fileName=" << urledProblem << "&";
  if (theGlobalVariables.UserStudentViewOn())
  { refStreamNoRequest << "studentView=true&";
    if (theGlobalVariables.GetWebInput("studentSection")!="")
      refStreamNoRequest << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
  }
  if (this->topicListFileName!="")
    refStreamNoRequest << "topicList=" << this->topicListFileName << "&";
  if (this->courseHome!="")
    refStreamNoRequest << "courseHome=" << this->courseHome << "&";
  if (theFileName==this->topicListFileName || theFileName==this->courseHome)
  { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=template&" << refStreamNoRequest.str() << "\">" << "Home" << "</a> ";
    return out.str();
  }
  if (!theGlobalVariables.UserGuestMode())
  { refStreamExercise << theGlobalVariables.DisplayNameExecutable << "?request=exercise&" << refStreamNoRequest.str();
    refStreamForReal << theGlobalVariables.DisplayNameExecutable << "?request=scoredQuiz&" << refStreamNoRequest.str();
  } else
    refStreamExercise << "?request=exerciseNoLogin&" << refStreamNoRequest.str();
  if (!theGlobalVariables.UserGuestMode())
    out << " <a href=\"" << refStreamForReal.str() << "\">" << CalculatorHTML::stringScoredQuizzes << "</a> ";
  out << " | <a href=\"" << refStreamExercise.str() << "\">" << CalculatorHTML::stringPracticE << "</a> ";
  //out << "DEBUG: topiclist: " << this->topicListFileName << " courseHome: " << this->courseHome
  //<< " filename: " << theFileName;
  return out.str();

}

std::string CalculatorHTML::ToStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinksFromFileName");
  std::stringstream out;
  out << this->ToStringLinkFromFileName(theFileName);
  out << this->ToStringProblemScoreFull(theFileName);
  out << this->ToStringProblemWeighT(theFileName);
#ifdef MACRO_use_MySQL
  bool problemAlreadySolved=false;
  if (this->currentUseR.theProblemData.Contains(theFileName))
  { ProblemData& theProbData=this->currentUseR.theProblemData.GetValueCreateIfNotPresent(theFileName);
    if (theProbData.numCorrectlyAnswered>=theProbData.theAnswers.size)
      problemAlreadySolved=true;
  }
  out << this->ToStringDeadline(theFileName, problemAlreadySolved, false);
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
  theProblem.theProblemData.flagRandomSeedGiven=true;
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
  tagClass=="calculatorJavascript" ||
  tagClass=="accountInformationLinks" ||
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
  ||
  tagClass=="calculatorHiddenIncludeInCommentsBeforeSubmission"
;
}

bool SyntacticElementHTML::IsVisibleCommentBeforeSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorCommentBeforeSubmission"
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
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
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
  std::stringstream streamCommands, streamCommandsBeforeInterpretation;
  //stOutput << "<hr>DEBUG: Preparing give-up commands for: "
  //<< theAnswer.answerId << "<hr>";
  int counter=0;
//  stOutput << "<hr>DEBUG: Call stack: " << crash.GetStackTraceEtcErrorMessage();
  theAnswer.commandIndicesCommentsBeforeSubmissioN.SetSize(0);
  theAnswer.commandIndicesVisibleCommentsBeforeSubmission.SetSize(0);
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
      theAnswer.commandIndicesCommentsBeforeSubmissioN.AddOnTop(counter);
      if (currentElt.IsVisibleCommentBeforeSubmission())
        theAnswer.commandIndicesVisibleCommentsBeforeSubmission.AddOnTop(counter);
      else
        streamCommandsBeforeInterpretation << this->CleanUpCommandString(currentElt.content);
      counter++;
    }
  }
  theAnswer.commandsCommentsBeforeSubmissionOnly=streamCommands.str();
  theAnswer.commandsBeforeInterpretation=streamCommandsBeforeInterpretation.str();
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

bool CalculatorHTML::PrepareSectionList(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareSectionList");
  (void) commentsOnFailure;
  if (this->flagSectionsPrepared)
    return true;
  this->flagSectionsPrepared=true;
  if (this->currentUseR.sectionsTaughtByUserString=="" ||
      (this->currentUseR.userRole!="admin" && this->currentUseR.userRole!="teacher") )
    if (this->currentUseR.userGroup.value!="")
    { this->databaseStudentSections.AddOnTop(this->currentUseR.userGroup.value);
      return true;
    }
  List<std::string> sectionList;
  MathRoutines::StringSplitDefaultDelimiters(this->currentUseR.sectionsTaughtByUserString.value, sectionList);
  this->databaseStudentSections.AddListOnTop(sectionList);
  return true;
}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return;
#ifdef MACRO_use_MySQL
  std::stringstream out;
  DatabaseRoutines theRoutines;
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts\"> Manage accounts</a>";
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
  else if (tagClass=="accountInformationLinks")
    this->InterpretAccountInformationLinks(inputOutput);
  else if (tagClass=="calculatorJavascript")
    this->InterpretJavascripts(inputOutput);
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
(const std::string& problemName, const std::string& sectionNumber, bool& outputIsInherited)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetDeadline");
  outputIsInherited=false;
  std::string result;
  #ifdef MACRO_use_MySQL
  int topicIndex=this->theTopicS.GetIndex(problemName);
  if (topicIndex==-1)
    return problemName + " not found in topic list. ";
  TopicElement& currentTopic=this->theTopicS.GetValueCreateIfNotPresent(problemName);
  for(int i=currentTopic.parentTopics.size-1; i>=0; i--)
  { const std::string& containerName=this->theTopicS.theKeys[currentTopic.parentTopics[i]];
    if (this->currentUseR.theProblemData.Contains(containerName))
    { ProblemDataAdministrative& currentProb=
      this->currentUseR.theProblemData.GetValueCreateIfNotPresent(containerName).adminData;
      result=currentProb.deadlinesPerSection.GetValueCreateIfNotPresent(sectionNumber);
//      stOutput << "DEBUG: deadline for containerName: " << containerName << " : " << result
//      << "<br>from problem data: " << currentProb.ToString();
      if (result!="")
      { outputIsInherited=(containerName!=problemName);
        return result;
      }
    }
  }
  #endif
  return result;
}

std::string CalculatorHTML::ToStringOnEDeadlineFormatted
(const std::string& cleanedUpLink,  const std::string& sectionNumber,
 bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline)
{ std::stringstream out;
  bool deadlineInherited=false;
  std::string currentDeadline =
  this->GetDeadline(cleanedUpLink, sectionNumber, deadlineInherited);
  if (currentDeadline=="")
  { if (returnEmptyStringIfNoDeadline)
      return "";
    out << "<span style=\"color:orange\">No deadline yet. </span>";
    //out << "DEBUG: section: " << sectionNumber;
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
  bool deadlineIsNear=secondsTillDeadline<24*3600 && !problemAlreadySolved;
  bool deadlineHasPassed=(secondsTillDeadline<0);
  if (!deadlineHasPassed)
  { if (deadlineIsNear)
      hoursTillDeadlineStream << "<span style=\"color:red\">"
      << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false) << "</span>";
    else
      hoursTillDeadlineStream << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false)
      << " left. ";
  } else
    hoursTillDeadlineStream << "[passed].";
  if (deadlineHasPassed && !problemAlreadySolved)
    out << "<span style=\"red\"><b>[Deadline passed].</b></span>"
    << "<span style=\"color:blue\">" << currentDeadline << "</span>. ";
  else
  { out << "Deadline: ";
    if (problemAlreadySolved)
      out << "<span style=\"color:green\">" << currentDeadline << "</span>. ";
    else if (deadlineIsNear)
      out << "<span style=\"color:red\">" << currentDeadline << "</span>. ";
    else if (deadlineInherited)
      out << "<span style=\"color:blue\">" << currentDeadline << "</span>. ";
    else
      out << "<span style=\"color:brown\">" << currentDeadline << "</span>. ";
    out << hoursTillDeadlineStream.str();
  }
  return
  //"[<span style=\"color:green\"><b>disabled</b> </span>] "+
  out.str();
#else
  out  << "Database not running: no deadlines";
  return out.str();
#endif // MACRO_use_MySQL
}

std::string CalculatorHTML::ToStringAllSectionDeadlines
  (const std::string& cleanedUpLink)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringAllSectionDeadlines");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "";
  std::stringstream out;
  out << "<table>";
  for (int i=0; i<this->databaseStudentSections.size; i++)
  { if (this->databaseStudentSections[i]=="")
      continue;
    out << "<tr><td>Section " << this->databaseStudentSections[i] << ":</td>";
    out << "<td>" << this->ToStringOnEDeadlineFormatted
    (cleanedUpLink, this->databaseStudentSections[i], false, false) << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string CalculatorHTML::ToStringDeadline
(const std::string& inputFileName, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineWithModifyButton");
#ifdef MACRO_use_MySQL
  if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserStudentViewOn())
  { std::string sectionNum=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("studentSection"));
    return this->ToStringOnEDeadlineFormatted
    (inputFileName, sectionNum, problemAlreadySolved, returnEmptyStringIfNoDeadline);
  } else
    return this->ToStringOnEDeadlineFormatted
    (inputFileName, this->currentUseR.userGroup.value, problemAlreadySolved, returnEmptyStringIfNoDeadline);
#endif // MACRO_use_MySQL
  return "";
}

std::string CalculatorHTML::ToStringDeadlineModifyButton
(const std::string& inputFileName, std::string& buttonHtmlContent,
 bool problemAlreadySolved, bool isProblemGroup)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineModifyButton");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return "";
  (void) problemAlreadySolved;
  std::stringstream out;
  std::stringstream deadlineStream;
  deadlineStream << "<table class=\"deadlineTable\"><tr>";
  deadlineStream << "<td><table><tr><th>Grp.</th><th>Deadline</th></tr>";
  List<std::string> deadlineIds;
  deadlineIds.SetSize(this->databaseStudentSections.size);
  for (int i=0; i<this->databaseStudentSections.size; i++)
  { std::string& currentDeadlineId=deadlineIds[i];
    if (this->databaseStudentSections[i]=="")
      continue;
    currentDeadlineId = "deadline" + Crypto::CharsToBase64String
    (this->databaseStudentSections[i]+inputFileName);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    bool deadlineInherited=false;
    std::string sectionNumber;
    std::string currentDeadline =
    this->GetDeadline(inputFileName, sectionNumber, deadlineInherited);
    deadlineStream << "<tr>";
    deadlineStream << "<td>" << this->databaseStudentSections[i] << "</td>";
    deadlineStream << "<td> <input type=\"text\" id=\"" << currentDeadlineId << "\" value=\""
    << this->GetDeadline(inputFileName, this->databaseStudentSections[i], deadlineInherited)
    << "\"> " ;
    deadlineStream << HtmlSnippets::GetDatePickerStart(currentDeadlineId);
    deadlineStream << "</td>";
    deadlineStream << "</tr>";
  }
  deadlineStream << "</table></td>";
  deadlineStream << "<td>\n";
  std::string deadlineIdReport="deadlineReport"+Crypto::CharsToBase64String(inputFileName);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  deadlineStream << "<button onclick=\"";
  deadlineStream << "submitStringAsMainInput('" << CGI::StringToURLString(inputFileName)
  << "='+encodeURIComponent('deadlines='+encodeURIComponent(";
  bool isFirst=true;
  for (int i=0; i<this->databaseStudentSections.size; i++)
  { if (this->databaseStudentSections[i]=="")
      continue;
    if (!isFirst)
      deadlineStream << "+";
    isFirst=false;
    deadlineStream << "'" << CGI::StringToURLString(this->databaseStudentSections[i]) << "='";
    deadlineStream << "+ encodeURIComponent(document.getElementById('"
    << deadlineIds[i] << "').value)+'&'";
  }
  deadlineStream << ")), '" << deadlineIdReport << "', 'setProblemData' );"
  << "\""
  << ">\n";
  deadlineStream << "Set</button>";
  deadlineStream << "<span id=\"" << deadlineIdReport << "\"></span>";
  deadlineStream << "</td>";
  deadlineStream << "</tr>";
  if (!isProblemGroup)
    deadlineStream << "<tr><td>(overrides section deadline).</td></tr> ";
  else
    deadlineStream << "<tr><td>(overriden by per-problem-deadline).</td></tr> ";

  deadlineStream << "</table>";
//  out << deadlineStream.str();
  out << "<button class=\"accordion\">" << buttonHtmlContent << "</button><span class=\"panel\">";
  out << deadlineStream.str();
  out << "</span>";

//  out << CGI::GetHtmlSpanHidableStartsHiddeN(deadlineStream.str(), "deadline+ ");
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
  this->LoadAndParseTopicList(comments);
}

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  double startTime=theGlobalVariables.GetElapsedSeconds();
//  stOutput << "<hr>DEBUG: Checking consistency 1 passed<hr>";
  if (!this->ParseHTML(comments))
  { this->outputHtmlBodyNoTag="<b>Failed to interpret html input. </b><br>" +this->ToStringContent();
    this->timeToParseHtml=theGlobalVariables.GetElapsedSeconds()-startTime;
    return false;
  }
  //stOutput << "<hr>DEBUG: about to check 2nd time<hr>";
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBUG: Checking consistency 2 passed<hr>";
  this->timeToParseHtml=theGlobalVariables.GetElapsedSeconds()-startTime;
  //stOutput << "DEBUG: this->theProblemData.flagRandomSeedGiven: " << this->theProblemData.flagRandomSeedGiven;
  this->MaxInterpretationAttempts=10;
  this->randomSeedsIfInterpretationFails.SetSize(this->MaxInterpretationAttempts);
  if (!this->theProblemData.flagRandomSeedGiven)
  { srand(time(NULL));
    this->randomSeedsIfInterpretationFails[0]=rand()%100000000;
  } else
    this->randomSeedsIfInterpretationFails[0]=this->theProblemData.randomSeed;
  srand(this->randomSeedsIfInterpretationFails[0]);
  for (int i=1; i<this->randomSeedsIfInterpretationFails.size; i++)
    this->randomSeedsIfInterpretationFails[i]=rand()%100000000;
  this->timePerAttempt.SetSize(0);
  this->timeIntermediatePerAttempt.SetSize(0);
  this->timeIntermediateComments.SetSize(0);
  this->NumAttemptsToInterpret=0;
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
      this->theProblemData.CheckConsistency();
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
  this->theProblemData.CheckConsistency();
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
    this->calculatorClasses.AddOnTop("accountInformationLinks");
    this->calculatorClasses.AddOnTop("calculatorJavascript");
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
    if (secondToLast.syntacticRole=="" && last.syntacticRole==">")
    { secondToLast.content+=">";
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
//  stOutput << "<hr>DEBUG: About to check consistency while parsing<hr>";
  //this->theProblemData.CheckConsistency();
  if (!result)
    comments << "<hr>Parsing stack.<hr>" << this->ToStringParsingStack(this->eltsStack);
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBUG: got to extracting answer ids<hr>";
  if (result)
    result=this->ExtractAnswerIds(comments);
  //stOutput << "<hr>DEBUG: after extracting answer ids<hr>";
  for (int i=0; i<this->theContent.size; i++)
    this->theContent[i].indexInOwner=i;
  if (result)
    result=this->CheckContent(comments);
  this->theProblemData.CheckConsistency();
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
    { std::string currentId= MathRoutines::StringTrimWhiteSpace(currentE.GetKeyValue("id"));
      if (currentId=="")
      { comments << "The answer element: " << currentE.ToStringDebug() << " has empty id. This is not allowed. ";
        return false;
      }
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
    { comments << "Auxilary answer element: " << currentE.ToStringDebug()
      << " has no name and appears before the first answer tag."
      << " Auxilary answers apply the answer tag whose id is specified in the name"
      << " tag of the auxilary answer. If the auxilary answer has no "
      << " name tag, it is assumed to apply to the (nearest) answer tag above it."
      << " To fix the issue either place the auxilary element after the answer or "
      << " specify the answer's id in the name tag of the auxilary element. "
      ;
      return false;
    }
    if (currentE.GetKeyValue("name")=="")
      currentE.SetKeyValue("name", *answerIdsSeenSoFar.LastObject());
//    stOutput << "<hr>Debug: processed element " << currentE.ToStringDebug();
  }
//  stOutput << "<hr>DEBUG: Elements: " << this->ToStringContent() << "<hr>";
  this->theProblemData.CheckConsistency();
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
  //stOutput << "<br>DEBUG: Cleaning up: " << inputCommand;
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
    { //stOutput << " DEBUG: to get: " << result;
      return result;
    }
    break;
  }
  if (result=="")
    return "";
  result.push_back(';');
  //stOutput << "DEBUG: to get: " << result << "<br>";
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
  { out << "\"" << CGI::StringToURLString( this->theProblemData.theAnswers[i].answerId ) << "\"";
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
    << "supSubsRequireOperand: true, // configurable\n"
    << "autoSubscriptNumerals: true, // configurable\n"
    << "handlers: {\n"
    << "edit: function() { // useful event handlers\n"
    << "if (ignoreNextMathQuillUpdateEvent){\n"
  //  << "  ignoreNextMathQuillUpdateEvent=false;\n"
    << "  return;\n"
    << "}\n"
    << currentA.varAnswerId << ".value = processMathQuillLatex(" <<  currentA.MQobject << ".latex() ); // simple API\n"
    << currentA.javascriptPreviewAnswer
    << "}\n"
    << "}\n"
    << "});\n";
  }
  out << "}//closing initializeMathQuill\n";
  out << "</script>";
  return out.str();
}

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  std::stringstream outBody;
  std::stringstream outHeaD, outHeadPt1, outHeadPt2;
  this->flagIsExamHome=(theGlobalVariables.userCalculatorRequestType=="template");
  this->theProblemData.randomSeed=this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret-1];
  this->FigureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before after loading problem list");
  outHeadPt2 << HtmlSnippets::GetJavascriptSubmitMainInputIncludeCurrentFile();
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
  if (this->flagIsExamHome && theGlobalVariables.UserDefaultHasAdminRights() &&
      !theGlobalVariables.UserStudentViewOn())
  { outHeadPt2 << HtmlSnippets::GetJavascriptHideHtml();
    outHeadPt2 << HtmlSnippets::GetDatePickerJavascriptInit();
  }
  if (this->flagUseNavigationBar)
    if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
    { outBody << this->GetEditPageButton(this->fileName);
      if (this->flagIsExamHome)
        outBody <<this->GetEditPageButton(this->topicListFileName);
      outBody << "<br>";
    }
  if (this->flagIsExamProblem)
  { bool problemAlreadySolved=false;
#ifdef MACRO_use_MySQL
    if (this->currentUseR.theProblemData.Contains(this->fileName))
    { ProblemData& theProbData=this->currentUseR.theProblemData.GetValueCreateIfNotPresent(this->fileName);
      if (theProbData.numCorrectlyAnswered>=theProbData.theAnswers.size)
        problemAlreadySolved=true;
    }
    std::string theDeadlineString=this->ToStringDeadline(this->fileName, problemAlreadySolved, true);
    if (theDeadlineString!="")
      theDeadlineString+="\n<hr>\n";
    outBody << theDeadlineString;
#endif
    //outBody << "<br>";
  }

  //////////////////////////////
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time after execution");
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  { outBody << "Debug information follows. ";
    if (this->flagIsExamProblem)
      outBody << "Exam problem here. ";
    outBody << "<br>Random seed: " << this->theProblemData.randomSeed
    << "<br>ForReal: " << this->flagIsForReal << "<br>seed given: "
    << this->theProblemData.flagRandomSeedGiven
    << "<br>flagRandomSeedGiven: " << this->theProblemData.flagRandomSeedGiven
    << "\n<br>\n"
    << CGI::StringToHtmlString(this->ToStringCalculatorArgumentsForProblem("exercise", "false"));

    #ifdef MACRO_use_MySQL
    outBody << "<br>Problem names: " << this->currentUseR.theProblemData.theKeys.ToStringCommaDelimited();
    outBody << "<br>Problem data string: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(this->currentUseR.problemDataString.value);
    #endif
    outBody << "<hr>";
    outBody << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
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
  //out << "Debug data: homework groups found: " << this->hdHomeworkGroups.ToStringCommaDelimited();
  //stOutput << "<hr>DEBUG: preparing answer elements... <hr>";
  this->PrepareAnswerElements(comments);
  this->NumAnswerIdsMathquilled=0;
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculator(this->theContent[i]);
  this->InterpretAnswerElements(comments);
  //stOutput << "<hr>DEBUG: All answers interpreted. <hr>";
  this->theProblemData.CheckConsistency();
  this->theProblemData.CheckConsistencyMQids();
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
  if (this->flagIsForReal && !this->theProblemData.flagRandomSeedGiven)
  { //stOutput << "This is for real!<br>";
    this->theProblemData.flagRandomSeedGiven=true;
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
    out << "<b>Guest mode</b>" << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=login\">Log in</a> ";
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  std::string calcArgsNoPassExamDetails=
  theGlobalVariables.ToStringCalcArgsNoNavigation(true, &randomSeedContainer);
  if (theGlobalVariables.UserDefaultHasAdminRights())
  { if (theGlobalVariables.UserStudentViewOn())
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
      << this->ToStringCalculatorArgumentsForProblem
      (theGlobalVariables.userCalculatorRequestType, "false", theGlobalVariables.GetWebInput("studentSection"))
      << "\">Admin view</a>" << linkSeparator;
    else
      out << "<b>Admin view</b>" << linkSeparator;
    if (this->databaseStudentSections.size==0)
    { if (theGlobalVariables.UserStudentViewOn())
        out << "<b>Student View</b>";
      else
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
        << this->ToStringCalculatorArgumentsForProblem
        (theGlobalVariables.userCalculatorRequestType, "true", "")
        << "\">Student view</a>";
    } else
      out << "Student view: ";
    for (int i=0; i<this->databaseStudentSections.size; i++)
      if (this->databaseStudentSections[i]!="")
      { if (theGlobalVariables.UserStudentViewOn() &&
            this->databaseStudentSections[i] == theGlobalVariables.GetWebInput("studentSection"))
          out << "<b>section " << this->databaseStudentSections[i] << "</b>";
        else
          out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
          << this->ToStringCalculatorArgumentsForProblem
          (theGlobalVariables.userCalculatorRequestType, "true", this->databaseStudentSections[i])
          << "\">section " << this->databaseStudentSections[i] << " </a>";
        if (i!=this->databaseStudentSections.size-1)
          out << linkSeparator;
      }
    out << linkBigSeparator;
  }
  if (this->flagIsExamProblem)
  { if (theGlobalVariables.userCalculatorRequestType=="exercise")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=scoredQuiz&"
      << this->ToStringCalculatorArgumentsForProblem("scoredQuiz", studentView)
      << "\">" << this->stringScoredQuizzes << "</a>" << linkSeparator;
    else if (theGlobalVariables.userCalculatorRequestType=="scoredQuiz")
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=exercise&"
      << this->ToStringCalculatorArgumentsForProblem("exercise", studentView)
      << "\">" << this->stringPracticE << "</a>" << linkSeparator;
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated)
  { int indexInParent=this->problemNamesNoTopics.GetIndex(this->fileName);
    if (indexInParent==-1)
    { out << "<b>Problem not in course</b>" << linkSeparator;
    } else
    { if (indexInParent>0)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << CGI::StringToURLString(this->topicListFileName) << "&";
        out << "courseHome=" << CGI::StringToURLString(this->courseHome) << "&";
        out << "fileName=" << CGI::StringToURLString(this->problemNamesNoTopics[indexInParent-1])
        << "\">&#8592;</a>" << linkSeparator;
      } else
        out << "<a disabled=\"disabled\">&#8592;</a>" << linkSeparator;
      if (indexInParent<this->problemNamesNoTopics.size-1)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection")!="")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << CGI::StringToURLString(this->topicListFileName) << "&";
        out << "courseHome=" << CGI::StringToURLString(this->courseHome) << "&";
        out << "fileName=" << CGI::StringToURLString(this->problemNamesNoTopics[indexInParent+1] )
        << "\">&#8594;</a>" << linkSeparator;
      } else
        out << "<a disabled=\"disabled\">&#8594;</a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem &&
      (theGlobalVariables.userCalculatorRequestType=="exercise" ||
       theGlobalVariables.userCalculatorRequestType=="exerciseNoLogin"))
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=" << theGlobalVariables.userCalculatorRequestType << "&"
    << this->ToStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">" << this->stringProblemLink << "</a>" << linkBigSeparator;
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
  out << theGlobalVariables.ToStringCalcArgsNoNavigation(true, &excludedTags)
  << "courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "&";
  if  (!theGlobalVariables.flagRunningApache && this->fileName!="")
    out << "fileName=" << CGI::StringToURLString(this->fileName) << "&";
  else
    out << "fileName=" << CGI::StringToURLString(theGlobalVariables.GetWebInput("fileName") )
    << "&";
  out << "topicList=" << theGlobalVariables.GetWebInput("topicList") << "&";
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

std::string CalculatorHTML::GetEditPageButton(const std::string& desiredFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetEditPageButton");
  std::stringstream out;
  out << "\n<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=editPage&";
  std::string urledProblem=CGI::StringToURLString(desiredFileName);
  std::stringstream refStreamNoRequest;
  std::string spanCloningAttemptResultID="spanCloningAttemptResultID"+desiredFileName;
  std::string clonePageAreaID="clonePageAreaID"+desiredFileName;
  //  out << "cleaned up link: " << cleaneduplink;
  //  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
  << "fileName=" << urledProblem << "&"
  << "courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "&";
  out << refStreamNoRequest.str() << "\">" << "Edit" << "</a>";
  out << "<textarea class=\"currentFileNameArea\" id=\""<< clonePageAreaID << "\" cols=\""
  << desiredFileName.size()+4 << "\">" << desiredFileName << "</textarea>\n"
  << "<button class=\"normalButton\" onclick=\""
  << "submitStringAsMainInput(document.getElementById('"
  << clonePageAreaID << "').value, '" << spanCloningAttemptResultID << "', 'clonePage');"
  << "\" >Clone</button> <span id=\"" << spanCloningAttemptResultID <<"\"></span>";
  return out.str();
}

std::string HtmlSnippets::GetJavascriptSubmitMainInputIncludeCurrentFile()
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
  << "  inputParams+='" << theGlobalVariables.ToStringCalcArgsNoNavigation(true) << "';\n"
  << "  inputParams+='&fileName=" << theGlobalVariables.GetWebInput("fileName") << "';\n"
  << "  inputParams+='&topicList=" << theGlobalVariables.GetWebInput("topicList") << "';\n"
  << "  inputParams+='&courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "';\n"
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

std::string CalculatorHTML::ToStringProblemScoreFull(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemScoreFull");
  std::stringstream out;
  if (theGlobalVariables.UserGuestMode())
  { out << "need to login";
    return out.str();
  }
  //stOutput << "<hr>CurrentUser.problemNames=" << this->currentUser.problemNames.ToStringCommaDelimited();
  #ifdef MACRO_use_MySQL
  Rational currentWeight;
  if (this->currentUseR.theProblemData.Contains(theFileName))
  { ProblemData& theProbData=this->currentUseR.theProblemData.GetValueCreateIfNotPresent(theFileName);
    if (!theProbData.flagProblemWeightIsOK)
    { out << "<span style=\"color:orange\">No point weight assigned yet. </span>";
      if (!theProbData.adminData.GetWeightFromSection(this->currentUseR.userGroup.value, currentWeight))
        currentWeight=0;
      if (theProbData.theAnswers.size==1)
      { if (theProbData.numCorrectlyAnswered==1)
          out << theProbData.totalNumSubmissions << " submission(s), problem correctly answered. ";
        else
          out << theProbData.totalNumSubmissions << " submission(s), problem not correctly answered yet. ";
      } else if (theProbData.theAnswers.size>1)
        out << theProbData.totalNumSubmissions << " submission(s), " << theProbData.numCorrectlyAnswered
        << " out of "<< theProbData.theAnswers.size << " subproblems correctly answered. ";
    } else if (theProbData.totalNumSubmissions!=0)
    { if (theProbData.numCorrectlyAnswered<theProbData.theAnswers.size)
      { out << "<span style=\"color:red\"><b> "
        << theProbData.Points << " out of "
        << currentWeight << " point(s). </b></span>";
      } else if (theProbData.numCorrectlyAnswered==theProbData.theAnswers.size)
      { out << "<span style=\"color:green\"><b> "
        << theProbData.Points << " out of "
        << currentWeight << " point(s). </b></span>";
      }
    }
  } else
  { out << "<span style=\"color:brown\"><b>No submissions.</b> </span>" ;
  }
  #endif // MACRO_use_MySQL
  return out.str();

}

std::string CalculatorHTML::ToStringProblemScoreShort(const std::string& theFileName, bool& outputAlreadySolved)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemScoreShort");
  std::stringstream out;
  if (theGlobalVariables.UserGuestMode())
  { out << "need to login";
    return out.str();
  }
  #ifdef MACRO_use_MySQL
  std::stringstream problemWeight;
  ProblemData theProbData;
  bool showModifyButton=theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn();
  outputAlreadySolved=false;
  Rational currentWeight;
  std::string currentWeightAsGivenByInstructor;
  if (this->currentUseR.theProblemData.Contains(theFileName))
  { theProbData=this->currentUseR.theProblemData.GetValueCreateIfNotPresent(theFileName);
    Rational percentSolved=0, totalPoints=0;
    percentSolved.AssignNumeratorAndDenominator(theProbData.numCorrectlyAnswered, theProbData.theAnswers.size);
    theProbData.flagProblemWeightIsOK=
    theProbData.adminData.GetWeightFromSection(this->currentUseR.userGroup.value, currentWeight, &currentWeightAsGivenByInstructor);
    if (!theProbData.flagProblemWeightIsOK)
    { problemWeight << "?";
      if (currentWeightAsGivenByInstructor!="")
        problemWeight << "<span style=\"color:red\">"
        << currentWeightAsGivenByInstructor << "(Error)</span>";
    } else
    { problemWeight << currentWeight;
      totalPoints=percentSolved*currentWeight;
    }
    outputAlreadySolved=(percentSolved==1);
    if (!outputAlreadySolved)
    { if (!theProbData.flagProblemWeightIsOK)
        out << "<span style=\"color:brown\"><b>" << percentSolved << " out of " << problemWeight.str() << "</b></span>";
      else
        out << "<span style=\"color:red\"><b>" << totalPoints << " out of " << problemWeight.str() << "</b></span>";
    } else
    { if (!theProbData.flagProblemWeightIsOK)
        out << "<span style=\"color:green\"><b>solved</b></span>";
      else
        out << "<span style=\"color:green\"><b>" << totalPoints << " out of " << problemWeight.str() << "</b></span>";
    }
  } else
    out << "<span style=\"color:brown\"><b>need to solve</b></span>";
  if (showModifyButton)
    out << " | " << this->ToStringProblemWeighT(theFileName);
  #endif // MACRO_use_MySQL
  return out.str();
}

std::string CalculatorHTML::ToStringProblemWeighT(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemWeighT");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return "";
  std::string urledProblem=CGI::StringToURLString(theFileName);
  std::stringstream out;
  //stOutput << "<hr>this->databaseProblemList is: " << this->databaseProblemList.ToStringCommaDelimited();
  //stOutput << "<br>this->databaseProblemWeights is: " << this->databaseProblemWeights.ToStringCommaDelimited();
  //stOutput << "<br> cleanedupLink: " << cleaneduplink;
  std::string idPoints = "points" + urledProblem;
  std::string idNumTries= "numTries"+urledProblem;
  std::string idButtonModifyPoints = "modifyPoints" + urledProblem;
  std::string idPointsModOutput = "modifyPointsOutputSpan" + urledProblem;
  out << "Pts: <textarea rows=\"1\" cols=\"2\" id=\"" << idPoints << "\">";
  bool weightIsOK=false;
  std::string problemWeightAsGivenByInstructor;
  #ifdef MACRO_use_MySQL
  Rational unusedRat;
  weightIsOK=this->currentUseR.theProblemData.GetValueCreateIfNotPresent(theFileName).
  adminData.GetWeightFromSection(this->currentUseR.userGroup.value, unusedRat, &problemWeightAsGivenByInstructor);
  out << problemWeightAsGivenByInstructor;
  #endif
  out << "</textarea>";
  if (!weightIsOK && problemWeightAsGivenByInstructor!="")
    out << "<span style=\"color:red\"><b>Error</b></span>";
  out << "<button id=\"" << idButtonModifyPoints << "\" "
  << "onclick=\"" << "submitStringAsMainInput('" << urledProblem
  << "='+encodeURIComponent('weight='+  getElementById('" << idPoints << "').value)"
//  << "  +encodeURIComponent('numTries='+getElementById('"
  << ", '"
  << idPointsModOutput << "', 'setProblemData');"
  << "\""
  << ">";
  out << "Modify";
  out << "</button>";
  out << "<span id=\"" << idPointsModOutput << "\">" << "</span>";
  return out.str();
}

void TopicElement::ComputeID()
{ if (this->problem!="")
  { this->id=this->problem;
    return;
  }
  std::stringstream out;
  out << this->title;
  if (this->flagIsChapter)
    out << "[Chapter]";
  if (this->flagIsSection)
    out << "[Section]";
  if (this->flagIsSubSection)
    out << "[SubSection]";
  this->id=out.str();
}

void TopicElement::AddTopic(TopicElement& inputElt, MapLisT<std::string, TopicElement, MathRoutines::hashString>& output)
{ MacroRegisterFunctionWithName("TopicElement::AddTopic");
  if (output.size()==0)
    if (!inputElt.flagIsChapter)
    { TopicElement chapterlessChapter;
      chapterlessChapter.parentTopics.AddOnTop(0);
      chapterlessChapter.flagIsChapter=true;
      chapterlessChapter.title="Topics without chapter";
      TopicElement::AddTopic(chapterlessChapter, output);
    }
  inputElt.ComputeID();
  if (inputElt.id=="")
  { inputElt.flagIsError=true;
    inputElt.id="error";
  }
  if (output.Contains(inputElt.id))
  { inputElt.id+="[Error]";
    inputElt.title= "[Error]: Entry " + inputElt.title + " already present. ";
  }
  output.SetKeyValue(inputElt.id, inputElt);
  if (inputElt.parentTopics.size>1)
    output.theValues[inputElt.parentTopics[inputElt.parentTopics.size-2]]
    .immediateChildren.AddOnTop(output.GetIndex(inputElt.id));
}

void TopicElement::GetTopicList(const std::string& inputString, MapLisT<std::string, TopicElement, MathRoutines::hashString>& output)
{ MacroRegisterFunctionWithName("TopicElement::GetTopicList");
  std::stringstream tableReader(inputString);
  std::string currentLine, currentArgument;
  TopicElement currentElt;
  bool found=false;
  while(std::getline(tableReader, currentLine, '\n'))
  { if (MathRoutines::StringTrimWhiteSpace(currentLine)=="")
      continue;
    if (currentLine.size()>0)
      if (currentLine[0]=='%')
        continue;
    if (MathRoutines::StringBeginsWith(currentLine, "Chapter:", &currentArgument))
    { if(found)
        TopicElement::AddTopic(currentElt, output);
      found=true;
      currentElt.reset(0);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.flagIsChapter=true;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Section:", &currentArgument))
    { if(found)
        TopicElement::AddTopic(currentElt, output);
      found=true;
      currentElt.reset(1);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id=currentElt.title;
      currentElt.flagIsSection=true;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Topic:", &currentArgument))
    { if(found)
        TopicElement::AddTopic(currentElt, output);
      found=true;
      currentElt.reset(2);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id=currentElt.title;
      currentElt.flagIsSubSection=true;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Title:", &currentArgument))
    { if(found)
        TopicElement::AddTopic(currentElt, output);
      found=true;
      currentElt.reset(3);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id=currentElt.title;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Video:", &currentArgument))
      currentElt.video=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "Slides:", &currentArgument))
      currentElt.slides=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "PrintableSlides:", &currentArgument))
      currentElt.slidesPrintable=MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "Problem:", &currentArgument))
    { currentElt.problem=MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id=currentElt.problem;
      found=true;
    } else
    { currentElt.error+="Unrecognized entry: " + currentLine + ". ";
      currentElt.flagIsError=true;
      found=true;
    }
  }
  if(found)
    TopicElement::AddTopic(currentElt, output);
}

void CalculatorHTML::InterpretAccountInformationLinks(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretAccountInformationLinks");
  std::stringstream out;
  if (!theGlobalVariables.flagLoggedIn)
  { out << "<b>User not logged-in.</b>";
    inputOutput.interpretedCommand= out.str();
    return;
  }
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
  { out << "<b>Account management requires https.</b>";
    inputOutput.interpretedCommand= out.str();
    return;
  }
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage\">Change password</a>";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts\">Manage accounts</a>";
  inputOutput.interpretedCommand= out.str();
  return;
}

bool CalculatorHTML::LoadAndParseTopicList(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadAndParseTopicList");
  if (this->theTopicS.size()!=0)
    return true;
  if (this->topicListContent=="")
    if (!FileOperations::LoadFileToStringVirtual(this->topicListFileName, this->topicListContent, comments))
      return false;
  if (this->topicListContent=="")
    return false;
  TopicElement::GetTopicList(this->topicListContent, this->theTopicS);
  this->problemNamesNoTopics.Clear();
  for (int i=0; i<this->theTopicS.size(); i++)
    if (this->theTopicS[i].problem!="")
      this->problemNamesNoTopics.AddOnTop(this->theTopicS[i].problem);
  for (int i=this->theTopicS.size()-1; i>=0; i--)
  { TopicElement& currentElt=this->theTopicS.theValues[i];
    if (currentElt.problem!="")
      continue;
    if (currentElt.flagIsSubSection)
    { currentElt.totalSubSectionsUnderME=0;
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections=0;
      currentElt.flagContainsProblemsNotInSubsection=false;
      continue;
    }
    currentElt.flagContainsProblemsNotInSubsection=false;
    currentElt.totalSubSectionsUnderME=0;
    for (int j=0; j<currentElt.immediateChildren.size; j++)
    { TopicElement& currentChild=this->theTopicS.theValues[currentElt.immediateChildren[j]];
      if (currentChild.flagIsSubSection)
      { currentElt.totalSubSectionsUnderME++;
        currentElt.totalSubSectionsUnderMeIncludingEmptySubsections++;
      } else if (currentChild.problem!="")
        currentElt.flagContainsProblemsNotInSubsection=true;
      else
      { currentElt.totalSubSectionsUnderME+=currentChild.totalSubSectionsUnderME;
        currentElt.totalSubSectionsUnderMeIncludingEmptySubsections+=currentChild.totalSubSectionsUnderMeIncludingEmptySubsections;
      }
    }
    if (currentElt.flagContainsProblemsNotInSubsection)
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections++;
  }
  return true;
}

void CalculatorHTML::InterpretTableOfContents(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretTableOfContents");
  std::stringstream out;
  if (!this->LoadAndParseTopicList(out))
  { inputOutput.interpretedCommand=out.str();
    return;
  }
  bool sectionStarted=false;
  bool subSectionStarted=false;
  bool chapterStarted=false;
  out << "<!--Topic list automatically generated from topic list: " << this->topicListFileName
  << ".-->";
  out << "<ol>";
  for (int i=0; i<this->theTopicS.size(); i++)
  { TopicElement& currentElt=this->theTopicS.theValues[i];
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

void CalculatorHTML::InterpretJavascripts(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretJavascripts");
  std::string javascriptName=MathRoutines::StringTrimWhiteSpace(inputOutput.content);
  if (javascriptName=="MathJax")
    inputOutput.interpretedCommand=CGI::GetJavascriptMathjax();
}

void CalculatorHTML::InterpretTopicList(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretTopicList");
  std::stringstream out;
  if (!this->LoadAndParseTopicList(out))
  { inputOutput.interpretedCommand=out.str();
    return;
  }
  if (!this->LoadDatabaseInfo(out))
    out << "<span style=\"color:red\">Could not load your problem history.</span> <br>";
  if (!this->PrepareSectionList(out))
    out << "<span style=\"color:red\">Error preparing section list.</span> <br>";
  //out << "DEBUG: sections: " << this->databaseStudentSections.ToStringCommaDelimited();
  //out << "DEBUG: prob data: " << this->currentUseR.theProblemData.ToStringHtml();
  #ifdef MACRO_use_MySQL
  this->currentUseR.ComputePointsEarned(this->currentUseR.theProblemData.theKeys, 0);
  #endif
  bool plainStyle=(inputOutput.GetKeyValue("topicListStyle")=="plain");
  bool tableStarted=false;
  bool sectionStarted=false;
  bool subSectionStarted=false;
  bool chapterStarted=false;
  std::string desiredChapter= CGI::URLStringToNormal(theGlobalVariables.GetWebInput("chapter"));
  std::string currentChapter="";
  out << "<!--Topic list automatically generated from topic list: " << this->topicListFileName
  << ".-->";
  //out << "<br>DEBUG: Desired chapter: " << desiredChapter << "<hr>Total chapters: " << this->theTopics.size;
  bool firstListStarted=false;
  int chapterCounter=0;
  //CalculatorHTML currentProblem;
  for (int i=0; i<this->theTopicS.size(); i++)
  { TopicElement& currentElt=this->theTopicS[i];
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
    currentElt.ComputeLinks(*this, plainStyle);
    if (!currentElt.flagIsSection && !currentElt.flagIsSubSection && !currentElt.flagIsChapter && !tableStarted)
    { out << TopicElement::GetTableStart(plainStyle);
      tableStarted=true;
    }
    if ((currentElt.flagIsSection || currentElt.flagIsSubSection || currentElt.flagIsChapter || currentElt.flagIsError)
        && tableStarted)
    { out << TopicElement::GetTableFinish(plainStyle);
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
    { out << "<li class=\"listChapter\">\n" << currentElt.displayTitleWithDeadline << "\n<br>\n";
      chapterStarted=true;
      sectionStarted=false;\
      subSectionStarted=false;
      tableStarted=false;
      out << "<ol>\n";
    } else if (currentElt.flagIsSection)
    { out << "<li class=\"listSection\">\n" << currentElt.displayTitleWithDeadline << "\n<br>\n";
      sectionStarted=true;
      subSectionStarted=false;
      tableStarted=false;
      out << "<ol>\n";
    } else if (currentElt.flagIsSubSection)
    { out << "<li class=\"listSubsection\">\n";
      out << currentElt.displayTitleWithDeadline << "\n<br>\n";
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
    { out << "<tr class=\"topicList\">\n";
      out << "  <td>\n";
      out << currentElt.displayTitle;
      out << "  </td>\n";
      out << "  <td>\n" << currentElt.displayVideoLink;
      if (currentElt.displaySlidesPrintableLink!="")
        out << " | " << currentElt.displaySlidesLink;
      if (currentElt.displaySlidesPrintableLink!="")
        out << " | " << currentElt.displaySlidesPrintableLink;
      if (theGlobalVariables.flagRunningAce)
        out << currentElt.displayAceProblemLink;
      else
        out << currentElt.displayProblemLink;
      out << "  </td>\n";
      out << "  <td>";
      if (currentElt.problem=="")
        out << "-";
      else
        out << currentElt.displayScore;
      out << "  </td>\n";
      out << "  <td>\n" << currentElt.displayDeadline << "  </td>\n";
      out << "</tr>\n";
    }
  }
  if (tableStarted)
    out << TopicElement::GetTableFinish(plainStyle);
  out << "</ol>";
  tableStarted=false;
  inputOutput.interpretedCommand=out.str();
}

void TopicElement::ComputeLinks(CalculatorHTML& owner, bool plainStyle)
{ MacroRegisterFunctionWithName("TopicElement::ComputeLinks");
  (void) plainStyle;
  if (this->displayProblemLink!="")
    return;
  if (this->title=="")
    this->displayTitle= (std::string)"-" ;//+ "<br>DEBUG: "+this->ToString();
  else
    this->displayTitle=this->title ;//+ "<br>DEBUG: "+this->ToString();
  if (this->video=="")
    this->displayVideoLink = "Lesson coming soon";
  else if (this->video=="-")
    this->displayVideoLink = "";
  else
    this->displayVideoLink= "<a href=\"#\" onclick=\"window.open('"
    + this->video + "',  'width=300', 'height=250', 'top=400'); return false;\">Go to lesson</a>";
  if (this->slides!="")
    this->displaySlidesLink = "<a href=\"" + this->slides + "\">Slides</a>";
  if (this->slidesPrintable!="")
    this->displaySlidesPrintableLink = "<a href=\"" + this->slidesPrintable + "\">Printable slides</a>";
  bool problemSolved=false;
  bool returnEmptyStringIfNoDeadline=false;
  if (this->problem=="")
  { this->displayProblemLink="";
    this->displayScore="";
    this->displayModifyWeight="";
    this->displayDeadline="";
    problemSolved=false;
    returnEmptyStringIfNoDeadline=true;
  } else
  { std::string theRawSQLink=theGlobalVariables.DisplayNameExecutable +
    "?request=scoredQuiz&fileName=" + this->problem;
    std::string theRawExerciseLink=theGlobalVariables.DisplayNameExecutable +
    "?request=exercise&fileName=" + this->problem;
    this->displayAceProblemLink=
    " | <a href=\"#\" onclick=\"window.open('" + theRawSQLink +
    "', 'width=300', 'height=250', 'top=400'); return false;\">" + CalculatorHTML::stringScoredQuizzes + "</a>"+
    " | <a href=\"#\" onclick=\"window.open('" + theRawExerciseLink+
    "',  'width=300', 'height=250', 'top=400'); return false;\">" +CalculatorHTML::stringPracticE+ "</a>";
    this->displayProblemLink= owner.ToStringLinkFromFileName(this->problem);
    this->displayScore=owner.ToStringProblemScoreShort(this->problem, problemSolved);
    this->displayModifyWeight=owner.ToStringProblemWeighT(this->problem);
  }
  this->displayDeadline=owner.ToStringDeadline(this->id, problemSolved, returnEmptyStringIfNoDeadline);
  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
  { if (this->displayDeadline=="")
      this->displayDeadline+="Deadline";
    this->displayDeadline=owner.ToStringDeadlineModifyButton
    (this->id, this->displayDeadline, problemSolved, this->flagIsSubSection || this->flagIsSection || this->flagIsChapter);
    std::stringstream titleAndDeadlineStream;
    titleAndDeadlineStream
    << "<span class=\"deadlineAndTitleContainer\">"
    << "<span class=\"titleContainer\">" << this->displayTitle << "</span>"
    << "<span class=\"deadlineContainer\">" << this->displayDeadline << "</span>"
    << "</span>"
    ;
    this->displayTitleWithDeadline=titleAndDeadlineStream.str();
  } else
    this->displayTitleWithDeadline=this->displayTitle;
}

std::string TopicElement::ToString()const
{ std::stringstream out;
  out << this->title;
  if (this->title=="")
    out << "-";
  if (this->flagIsChapter)
    out << "(chapter)";
  if (this->flagIsSection)
    out << "(section)";
  if (this->flagIsSubSection)
    out << "(subsection)";
  out << "<br>Parents: " << this->parentTopics.ToStringCommaDelimited();
  return out.str();
}

std::string TopicElement::GetTableStart(bool plainStyle)
{ std::stringstream out;
  out << "\n\n<table class=\"topicList\">\n";
  out << "<colgroup><col><col><col><col></colgroup>\n";
  out << "<tbody>\n";
  if (!plainStyle)
    out
    << "<tr> <th>Sub-Topic</th>"
    << "<th>Resource Links</th>"
    << "<th>Current Score</th>"
    << "<th>Deadlines</th>"
    << "</tr>\n";
  return out.str();
}

std::string TopicElement::GetTableFinish(bool plainStyle)
{ std::stringstream out;
  (void) plainStyle;
  out << "</tbody>\n";
  out << "</table>\n\n";
  return out.str();
}
