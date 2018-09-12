//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader7DatabaseInterface.h"
#include "vpfHeader1General5TimeDate.h"
#include "vpfHeader8HtmlInterpretation.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include "vpfHeader2Math10_LaTeXRoutines.h"
#include <iomanip>
#include "vpfHeader7DatabaseInterface_Mongodb.h"
#include "vpfWebAPI.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCustomCode
(__FILE__, "Routines for calculus teaching: calculator exam mode. Shared code. ");

std::string CalculatorHTML::stringScoredQuizzes = "Quiz";
std::string CalculatorHTML::stringPracticE = "Practice";
std::string CalculatorHTML::stringProblemLink = "Problem";

CalculatorHTML::CalculatorHTML()
{ this->NumAttemptsToInterpret = 0;
  this->NumAnswerIdsMathquilled = 0;
  this->MaxInterpretationAttempts = 25;
  this->flagUseJSON = false;
  this->flagLoadedSuccessfully = false;
  this->flagIsExamHome = false;
  this->flagIsExamProblem = false;
  this->flagParentInvestigated = false;
  this->NumProblemsFound = 0;
  this->NumVideosFound = 0;
  this->NumHandwrittenSolutionsFound = 0;
  this->NumVideosHandwrittenFound = 0;
  this->NumVideosWithSlidesFound = 0;
  this->NumSlidesFound = 0;
  this->flagIsForReal = false;
  this->flagLoadedFromDB = false;
  this->flagLoadedClassDataSuccessfully = false;
  this->flagTagHeadPresent = false;
  this->flagTagHtmlPresent = false;
  this->flagTagBodyPresent = false;
  this->flagDoPrependCalculatorNavigationBar = true;
  this->flagDoPrependProblemNavigationBar = true;
  this->flagDoPrependEditPagePanel = true;

  this->flagTopicTableStarted      = false;
  this->flagTopicSectionStarted    = false;
  this->flagTopicSubSectionStarted = false;
  this->flagTopicChapterStarted    = false;

  this->timeToParseHtml = 0;
  this->flagMathQuillWithMatrices = false;
  this->flagSectionsPrepared = false;
  this->topicLectureCounter = 0;
}

extern logger logWorker;

#ifdef MACRO_use_MongoDB
bool CalculatorHTML::LoadProblemInfoFromJSONAppend
(const JSData& inputJSON,
 MapLisT<std::string, ProblemData, MathRoutines::hashString>& outputProblemInfo,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::LoadProblemInfoFromJSONAppend");
  (void) commentsOnFailure;
  if (inputJSON.type == inputJSON.JSUndefined)
    return true;
  //stOutput << crash.GetStackTraceShort()
  //<< "<br>DEBUG: Read json: " << inputJSON.ToString(false);
  outputProblemInfo.SetExpectedSize(inputJSON.objects.size());
  ProblemData emptyData;
  std::string currentCourse = theGlobalVariables.userDefault.courseComputed;
  for (int i = 0; i < inputJSON.objects.size(); i ++)
  { std::string currentProbName = inputJSON.objects.theKeys[i];
    JSData& currentProblem = inputJSON.objects.theValues[i];
    if (currentProbName == "")
      continue;
    //if (theGlobalVariables.UserDebugFlagOn())
    //  stOutput << "<br>DEBUG: current problem json: " << currentProblem.ToString(false)
    //  << ";<br> currentProbName: " << currentProbName << "\n\n\n";
    if (!outputProblemInfo.Contains(currentProbName))
      outputProblemInfo.GetValueCreate(currentProbName) = emptyData;
    ProblemData& currentProblemValue = outputProblemInfo.GetValueCreate(currentProbName);
    JSData& currentDeadlines = currentProblem[DatabaseStrings::labelDeadlines];
    JSData& currentWeight = currentProblem[DatabaseStrings::labelProblemWeights];
    //stOutput << "<br>DEBUG: current deadlines: " << currentDeadlines.ToString(false);
    //stOutput << "<br>DEBUG: Current problems: " << currentProblem.ToString(false)
    //<< "<br>Current weight: " << currentWeight.ToString(false);
    //stOutput.Flush();
    if (currentWeight.type == JSData::JSObject)
      for (int j = 0; j < currentWeight.objects.size(); j ++)
      { JSData& currentWeightValue = currentWeight.objects.theValues[j];
        if (currentWeightValue.type != JSData::JSstring)
          continue;
        currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue
        (currentWeight.objects.theKeys[j], currentWeightValue.string);
      }
    else if (currentWeight.type == JSData::JSstring)
      currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue
      (currentCourse, currentWeight.string);
    else if (currentWeight.type != JSData::JSUndefined)
    { commentsOnFailure << "Could extract neither weight nor weights-per course from "
      << currentWeight.ToString(false) << ". Your input was: " << inputJSON.ToString(false);
      return false;
    }
    if (currentDeadlines.type != JSData::JSUndefined)
    { for (int j = 0; j < currentDeadlines.objects.size(); j ++)
        currentProblemValue.adminData.deadlinesPerSection.SetKeyValue
        (currentDeadlines.objects.theKeys[j], currentDeadlines.objects.theValues[j].string);
      //stOutput << "<br>DEBUG: Current deadlines: " << currentDeadlines.ToString(false)
      //<< "<br> Total input: " << inputJSON.ToString(false) << "<hr>";
    }
  }
  return true;
}

bool CalculatorHTML::LoadProblemInfoFromURLedInputAppend
(const std::string& inputInfoString,
 MapLisT<std::string, ProblemData, MathRoutines::hashString>& outputProblemInfo,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::LoadProblemInfoFromURLedInputAppend");
  MapLisT<std::string, std::string, MathRoutines::hashString>
  CGIedProbs, currentKeyValues, sectionDeadlineInfo, problemWeightInfo;
  if (!HtmlRoutines::ChopCGIString(inputInfoString, CGIedProbs, commentsOnFailure))
    return false;
  //stOutput << "<hr>Debug: reading problem info from: " << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(inputInfoString);
  outputProblemInfo.SetExpectedSize(outputProblemInfo.size() + CGIedProbs.size());
  std::string currentProbName, currentProbString;
  ProblemData emptyData;
  for (int i = 0; i < CGIedProbs.size(); i ++)
  { currentProbName = MathRoutines::StringTrimWhiteSpace(HtmlRoutines::ConvertURLStringToNormal(CGIedProbs.theKeys[i], false));
    if (currentProbName == "")
      continue;
    currentProbString = HtmlRoutines::ConvertURLStringToNormal(CGIedProbs.theValues[i], false);
    if (!outputProblemInfo.Contains(currentProbName))
      outputProblemInfo.GetValueCreate(currentProbName) = emptyData;
    ProblemData& currentProblemValue = outputProblemInfo.GetValueCreate(currentProbName);
    if (!HtmlRoutines::ChopCGIString(currentProbString, currentKeyValues, commentsOnFailure))
      return false;
    stOutput << "<hr>Debug: reading problem info from: " << currentProbString << " resulted in pairs: "
    << currentKeyValues.ToStringHtml();
    std::string deadlineString = HtmlRoutines::ConvertURLStringToNormal(currentKeyValues.GetValueCreate(DatabaseStrings::labelDeadlines), false);
    std::string problemWeightsCollectionString = HtmlRoutines::ConvertURLStringToNormal
    (currentKeyValues.GetValueCreate(DatabaseStrings::labelProblemWeights), false);
    if (problemWeightsCollectionString != "")
    { if (!HtmlRoutines::ChopCGIString(problemWeightsCollectionString, problemWeightInfo, commentsOnFailure))
        return false;
      for (int j = 0; j < problemWeightInfo.size(); j ++)
        currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue
        (HtmlRoutines::ConvertURLStringToNormal(problemWeightInfo.theKeys[j], false),
         HtmlRoutines::ConvertURLStringToNormal(problemWeightInfo.theValues[j], false));
    }
    if (deadlineString != "")
    { if (!HtmlRoutines::ChopCGIString(deadlineString, sectionDeadlineInfo, commentsOnFailure))
        return false;
      for (int j = 0; j < sectionDeadlineInfo.size(); j ++)
      { currentProblemValue.adminData.deadlinesPerSection.SetKeyValue
        (HtmlRoutines::ConvertURLStringToNormal(sectionDeadlineInfo.theKeys[j], false),
         HtmlRoutines::ConvertURLStringToNormal(sectionDeadlineInfo.theValues[j], false));
        stOutput << "<hr>DEBUG: Current key, value pair: "
        << HtmlRoutines::ConvertURLStringToNormal(sectionDeadlineInfo.theKeys[j], false)
        << ", " << HtmlRoutines::ConvertURLStringToNormal(sectionDeadlineInfo.theValues[j], false) << ". ";
      }
    }
    std::string problemWeightString = MathRoutines::StringTrimWhiteSpace
    (currentKeyValues.GetValueCreate(DatabaseStrings::labelProblemWeights));
    if (problemWeightString != "")
    { currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue
      (HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.userDefault.courseComputed, false),
       HtmlRoutines::ConvertURLStringToNormal(problemWeightString, false));
    }
  }
  return true;
}

JSData CalculatorHTML::ToJSONDeadlines
(MapLisT<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToJSONDeadlines");
  JSData output;
  output.type = output.JSObject;

  for (int i = 0; i < inputProblemInfo.size(); i ++)
  { ProblemDataAdministrative& currentProblem = inputProblemInfo.theValues[i].adminData;
    if (currentProblem.deadlinesPerSection.size() == 0)
      continue;
    std::string currentProblemName = inputProblemInfo.theKeys[i];
    JSData currentProblemJSON;
    for (int j = 0; j < currentProblem.deadlinesPerSection.size(); j ++)
    { std::string currentDeadline = MathRoutines::StringTrimWhiteSpace(currentProblem.deadlinesPerSection.theValues[j]);
      if (currentDeadline == "")
        continue;
      std::string currentSection = MathRoutines::StringTrimWhiteSpace(currentProblem.deadlinesPerSection.theKeys[j]);
      currentProblemJSON[DatabaseStrings::labelDeadlines][currentSection] = currentDeadline;
    }
    output[currentProblemName] = currentProblemJSON;
  }
  //if (theGlobalVariables.UserDebugFlagOn())
  //  stOutput << "DEBUG: deadline Output: " << output.ToString(false);
  return output;
}

JSData CalculatorHTML::ToJSONProblemWeights
(MapLisT<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToJSONProblemWeights");
  JSData output;
  for (int i = 0; i < inputProblemInfo.size(); i ++)
  { ProblemDataAdministrative& currentProblem = inputProblemInfo.theValues[i].adminData;
    if (currentProblem.problemWeightsPerCoursE.size() == 0)
      continue;
    std::string currentProblemName = inputProblemInfo.theKeys[i];
    JSData currentProblemJSON;
    for (int j = 0; j < currentProblem.problemWeightsPerCoursE.size(); j ++)
    { std::string currentWeight = MathRoutines::StringTrimWhiteSpace(currentProblem.problemWeightsPerCoursE.theValues[j]);
      if (currentWeight == "")
        continue;
      std::string currentCourse = MathRoutines::StringTrimWhiteSpace(currentProblem.problemWeightsPerCoursE.theKeys[j]);
      currentProblemJSON[currentCourse] = currentWeight;
    }
    JSData currentWeight;
    currentWeight[DatabaseStrings::labelProblemWeights] = currentProblemJSON;
    output[currentProblemName] = currentWeight;
  }
  return output;
}

bool DatabaseRoutineS::StoreProblemInfoToDatabase
(const UserCalculatorData& theUser, bool overwrite, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  JSData findQueryWeights, findQueryDeadlines;
  findQueryWeights[DatabaseStrings::labelProblemWeightsSchema] = theUser.problemWeightSchema;
  findQueryDeadlines[DatabaseStrings::labelDeadlinesSchema] = theUser.deadlineSchema;
  //stOutput << "<br>DEBUG: deadline find query: " << findQueryWeights.ToString(false) << "<br>";
  if (theUser.problemWeights.type != JSData::JSUndefined)
  { if (overwrite)
    { JSData setQueryWeights;
      setQueryWeights[DatabaseStrings::labelProblemWeights] = theUser.problemWeights;
      if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
           (DatabaseStrings::tableProblemWeights, findQueryWeights, setQueryWeights, 0, &commentsOnFailure))
        return false;
    } else
    { List<std::string> adjustLabels;
      adjustLabels.SetSize(2);
      adjustLabels[0] = DatabaseStrings::labelProblemWeights;
      for (int i = 0; i < theUser.problemWeights.objects.size(); i ++)
      { adjustLabels[1] = theUser.problemWeights.objects.theKeys[i];
        if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
             (DatabaseStrings::tableProblemWeights,
              findQueryWeights, theUser.problemWeights.objects.theValues[i],
              &adjustLabels, &commentsOnFailure))
          return false;
      }
    }
  }
  //stOutput << "<br>DEBUG: deadline find query: " << findQueryDeadlines.ToString(false);
  //stOutput << "<br>DEBUG: deadline update query: " << theUser.deadlines.ToString(false);

  if (theUser.deadlines.type != JSData::JSUndefined)
  { if (overwrite)
    { JSData setQueryDeadlines;
      setQueryDeadlines[DatabaseStrings::labelDeadlines] = theUser.deadlines;
      if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
           (DatabaseStrings::tableProblemWeights, findQueryDeadlines, setQueryDeadlines, 0, &commentsOnFailure))
        return false;
    } else
    { List<std::string> adjustLabels;
      adjustLabels.SetSize(2);
      adjustLabels[0] = DatabaseStrings::labelDeadlines;
      for (int i = 0; i < theUser.deadlines.objects.size(); i ++)
      { adjustLabels[1] = theUser.deadlines.objects.theKeys[i];
        if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
             (DatabaseStrings::tableDeadlines,
              findQueryDeadlines, theUser.deadlines.objects.theValues[i],
              &adjustLabels, &commentsOnFailure))
          return false;
      }
    }
  }
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
  ProblemDataAdministrative& currentProblem =
  this->currentUseR.theProblemData.GetValueCreate(inputProblemName).adminData;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  currentDeadlines = currentProblem.deadlinesPerSection;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  incomingDeadlines = inputProblemInfo.adminData.deadlinesPerSection;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  currentWeightS = currentProblem.problemWeightsPerCoursE;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  incomingWeightS = inputProblemInfo.adminData.problemWeightsPerCoursE;

  for (int i = 0; i < incomingDeadlines.size(); i ++)
  { if (this->databaseStudentSections.size >= 1000)
    { commentsOnFailure << "Failed to account deadlines: max 999 sections allowed. ";
      return false;
    }
    this->databaseStudentSections.AddOnTopNoRepetition(incomingDeadlines.theKeys[i]);
  }
  ////////////////////////////////////////////
  for (int i = 0; i < incomingDeadlines.size(); i ++)
    currentDeadlines.SetKeyValue(incomingDeadlines.theKeys[i], incomingDeadlines.theValues[i]);
  for (int i = 0; i < incomingWeightS.size(); i ++)
    currentWeightS.SetKeyValue(incomingWeightS.theKeys[i], incomingWeightS.theValues[i]);
  return true;
}

bool CalculatorHTML::MergeProblemInfoInDatabaseJSON
(std::string& incomingProblemInfo, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::MergeProblemInfoInDatabase");
  //stOutput << "DEBUG: Here I am, merging in data: " << incomingProblemInfo;
  JSData theProblemJSON;
  if (!theProblemJSON.readstring(incomingProblemInfo, false, &commentsOnFailure))
    return false;
  MapLisT<std::string, ProblemData, MathRoutines::hashString> incomingProblems;
  if (!this->LoadProblemInfoFromJSONAppend(theProblemJSON, incomingProblems, commentsOnFailure))
  { commentsOnFailure << "Failed to parse your request";
    return false;
  }
  theGlobalVariables.userDefault.problemWeights = this->ToJSONProblemWeights(incomingProblems);
  theGlobalVariables.userDefault.deadlines = this->ToJSONDeadlines(incomingProblems);
  if (!DatabaseRoutineS::StoreProblemInfoToDatabase(theGlobalVariables.userDefault, false, commentsOnFailure))
    return false;
  return true;
}

#endif // MACRO_use_MongoDB

bool CalculatorHTML::LoadDatabaseInfo(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadDatabaseInfo");
#ifdef MACRO_use_MongoDB
  this->currentUseR.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBug: got to before PrepareSectionList.<hr>";
  if (!this->PrepareSectionList(comments))
    return false;
  //this->theProblemData.CheckConsistency();
  //stOutput << "<hr>DEBug: got to before InterpretDatabaseProblemData.<hr>";
  //stOutput << "Problem weight schema: " << this->currentUseR.problemWeightSchema
  //<< "Problem weight string: " << this->currentUseR.problemWeightString
  //<< " - of global vars: " << theGlobalVariables.userDefault.problemWeightString;
  if (this->currentUseR.problemDataJSON.objects.size() != 0)
  { if (!this->currentUseR.InterpretDatabaseProblemDataJSON(this->currentUseR.problemDataJSON, comments))
    { comments << "Failed to interpret user's problem saved data. ";
      return false;
    }
  } else
  { if (!this->currentUseR.InterpretDatabaseProblemDatA(this->currentUseR.problemDataStrinG, comments))
    { comments << "Failed to interpret user's problem saved data. ";
      //stOutput << "Failed to interpret user's problem saved data. ";
      return false;
    }
  }
  if (!this->LoadProblemInfoFromJSONAppend
       (this->currentUseR.problemWeights, this->currentUseR.theProblemData, comments))
  { comments << "Failed to load problem weights. ";
    return false;
  }
  if (!this->LoadProblemInfoFromJSONAppend
       (this->currentUseR.deadlines, this->currentUseR.theProblemData, comments))
  { comments << "Failed to load problem deadlines. ";
    return false;
  }

  if (this->currentUseR.theProblemData.Contains(this->fileName))
  { this->theProblemData = this->currentUseR.theProblemData.GetValueCreate(this->fileName);
    //stOutput << "<hr>Debug: found problem data! " << this->theProblemData.ToString() << "<hr>";
  } //else
    //stOutput << "<hr>Did not find problem data for filename: " << this->fileName << ". USer details: " << this->currentUseR.ToString() << "<hr>";
  theGlobalVariables.userDefault = this->currentUseR;
  //stOutput << "<hr>DEBUG: About to return true from LoadDatabaseInfo";
  return true;
#else
  comments << "Database not available. ";
  return false;
#endif
}

bool CalculatorHTML::LoadMe(bool doLoadDatabase, std::stringstream& comments, const std::string& inputRandomSeed)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadMe");
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly
       (this->fileName, this->RelativePhysicalFileNameWithFolder, &comments))
  { comments << "Failed to get physical file name from " << this->fileName << ". ";
    return false;
  }
  (void) doLoadDatabase;
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(this->fileName, this->inputHtml, &comments))
  { comments << "<b>Failed to open: " << this->fileName
    << " with computed file name: " << this->RelativePhysicalFileNameWithFolder << "</b> ";
    return false;
  }
  //stOutput << "Debug: got to here pt1";
  this->flagIsForReal = theGlobalVariables.UserRequestRequiresLoadingRealExamData();
#ifdef MACRO_use_MongoDB
  this->topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
  //stOutput << "Debug: got to here pt2";
  //this->theProblemData.CheckConsistency();
  //stOutput << "Debug: got to here pt3";
  if (doLoadDatabase)
  { this->LoadDatabaseInfo(comments);
   // stOutput << "DEBUG: the topics: " << this->theTopicS.ToStringHtml();
    for (int i = 0; i < this->theTopicS.size(); i ++)
      this->ComputeDeadlinesAllSectionsNoInheritance(this->theTopicS[i]);
  }

#endif // MACRO_use_MongoDB
  //stOutput << "<hr>DEBUG: Loaded successfully, checking: <hr>";
  this->theProblemData.CheckConsistency();
  //stOutput << "<hr>OK<hr> ";

  //stOutput << "DEBUG: flagIsForReal: " << this->flagIsForReal;
  if (!this->flagIsForReal)
  { std::string randString = inputRandomSeed;
    if (randString != "")
    { std::stringstream randSeedStream(randString);
      //stOutput << "radSeedStream: " << randString;
      randSeedStream >> this->theProblemData.randomSeed;
      //stOutput << ", interpreted as: " << this->theProblemData.randomSeed;
      this->theProblemData.flagRandomSeedGiven = true;
    }
  }
  //  stOutput << "debug info: rand seed after all's been said and done: " << this->theProblemData.randomSeed;
  return true;
}

std::string CalculatorHTML::LoadAndInterpretCurrentProblemItemJSON(bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadAndInterpretCurrentProblemItemJSON");
  double startTime = theGlobalVariables.GetElapsedSeconds();

  this->LoadCurrentProblemItem(needToLoadDatabaseMayIgnore, desiredRandomSeed);
  if (!this->flagLoadedSuccessfully)
    return this->comments.str();
  std::stringstream out;
  if (!this->InterpretHtml(this->comments))
  { out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
    << "<small>Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds() - startTime)
    << " second(s).</small>"
    << "</calculatorNavigation>";
    if (theGlobalVariables.UserDefaultHasAdminRights())
      out
      << "<editPagePanel>"
      << this->GetEditPageButton(this->fileName)
      << "</editPagePanel>"
      << "<br>";
    out << "<b>Failed to interpret file: " << this->fileName
    << "</b>. ";
    out << "<br>We limit the number of generation attemps to 10 for performance reasons; "
    << "with bad luck, some finicky problems require more. "
    << "<br> <b>Please refresh the page.</b><br>";
    if (!this->flagIsForReal)
      out
      << "If you specified the problem through the 'problem link' link,"
      << " please go back to the course page. Alternatively, remove the randomSeed=... "
      << "portion of the page address and reload the page (with the randomSeed portion removed). ";
    else
      out << "<b>Your random seed must have been reset. </b>";
    out << "<br><span style =\"color:red\"><b>If the problem persists after a couple of page refreshes, "
    << "it's a bug. Please take a screenshot and email the site admin/your instructor. </b></span>";
    if (theGlobalVariables.UserDefaultHasProblemComposingRights())
      out << "<hr> <b>Comments, admin view only.</b><br> " << this->comments.str();
    return out.str();
  }
  //out << "DEBUG: flagMathQuillWithMatrices =" << this->flagMathQuillWithMatrices << "<br>";
  if (this->flagDoPrependCalculatorNavigationBar)
  { out << "<calculatorNavigation>"
    << theGlobalVariables.ToStringNavigation()
    << "<small>Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds() - startTime)
    << " second(s).</small>" << "</calculatorNavigation>\n";
  }
  if (this->flagDoPrependProblemNavigationBar)
    out << this->outputProblemNavigatioN;
  out << this->comments.str();
  out << this->outputHtmlBodyNoTag;
  return out.str();
}

std::string CalculatorHTML::LoadAndInterpretCurrentProblemItem(bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadAndInterpretCurrentProblemItem");
  double startTime = theGlobalVariables.GetElapsedSeconds();
//  this->theProblemData.CheckConsistency();
  this->LoadCurrentProblemItem(needToLoadDatabaseMayIgnore, desiredRandomSeed);
//  this->theProblemData.CheckConsistency();
  if (!this->flagLoadedSuccessfully)
    return this->comments.str();
  std::stringstream out;
  if (!this->InterpretHtml(this->comments))
  { out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
    << "<small>Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds() - startTime)
    << " second(s).</small>"
    << "</calculatorNavigation>";
    if (theGlobalVariables.UserDefaultHasAdminRights())
      out
      << "<editPagePanel>"
      << this->GetEditPageButton(this->fileName)
      << "</editPagePanel>"
      << "<br>";
    out << "<b>Failed to interpret file: " << this->fileName
    << "</b>. ";
    out << "<br>We limit the number of generation attemps to 10 for performance reasons; "
    << "with bad luck, some finicky problems require more. "
    << "<br> <b>Please refresh the page.</b><br>";
    if (!this->flagIsForReal)
      out
      << "If you specified the problem through the 'problem link' link,"
      << " please go back to the course page. Alternatively, remove the randomSeed=... "
      << "portion of the page address and reload the page (with the randomSeed portion removed). ";
    else
      out << "<b>Your random seed must have been reset. </b>";
    out << "<br><span style =\"color:red\"><b>If the problem persists after a couple of page refreshes, "
    << "it's a bug. Please take a screenshot and email the site admin/your instructor. </b></span>";
    if (theGlobalVariables.UserDefaultHasProblemComposingRights())
      out << "<hr> <b>Comments, admin view only.</b><br> " << this->comments.str();
    return out.str();
  }
  //out << "DEBUG: flagMathQuillWithMatrices =" << this->flagMathQuillWithMatrices << "<br>";
  if (this->flagDoPrependCalculatorNavigationBar)
  { out << "<calculatorNavigation>"
    << theGlobalVariables.ToStringNavigation()
    << "<small>Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds() - startTime)
    << " second(s).</small>" << "</calculatorNavigation>\n";
  }
  if (this->flagDoPrependProblemNavigationBar)
    out << this->outputProblemNavigatioN;
  out << this->comments.str();
  out << this->outputHtmlBodyNoTag;
  return out.str();
}

void CalculatorHTML::LoadFileNames()
{ this->fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("fileName"), false);
  this->courseHome = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  this->topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
}

void CalculatorHTML::LoadCurrentProblemItem(bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->LoadFileNames();
  this->flagLoadedSuccessfully = false;
  if (theGlobalVariables.UserGuestMode())
    needToLoadDatabaseMayIgnore = false;
  this->flagDoPrependCalculatorNavigationBar = true;
  if (theGlobalVariables.GetWebInput("navigationBar") == "false" ||
      theGlobalVariables.userCalculatorRequestType == "exerciseJSON" ||
      theGlobalVariables.userCalculatorRequestType == "scoredQuizJSON")
    this->flagDoPrependCalculatorNavigationBar = false;
  this->flagLoadedSuccessfully = true;
  if (this->fileName == "")
  { this->flagLoadedSuccessfully = false;
    this->comments << "<b>No problem file name found. </b>";
  }

//  this->theProblemData.CheckConsistency();
//  stOutput << "<hr>DEBUG: got to before loading<hr>";
  std::stringstream commentsStream;
  if (!this->LoadMe(needToLoadDatabaseMayIgnore, commentsStream, inputRandomSeed))
    this->flagLoadedSuccessfully = false;
//  stOutput << "<hr>DEBUG: loaded<hr>";
//  stOutput << "<hr>DEBUG: OK<hr>";
  if (!this->flagLoadedSuccessfully)
  { if (!this->flagUseJSON)
    { this->comments << "<calculatorNavigation>"
      << theGlobalVariables.ToStringNavigation()
      << "</calculatorNavigation>";
      this->FigureOutCurrentProblemList(this->comments);
      this->flagIsExamProblem = true;
      this->comments << "<problemNavigation>"
      << this->ToStringProblemNavigation()
      << "</problemNavigation>";
      this->comments << commentsStream.str();
      this->comments << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=selectCourse\">Go to course list page.</a>";
    }
  }
  this->theProblemData.CheckConsistency();
}

bool CalculatorHTML::IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt)
{ MacroRegisterFunctionWithName("CalculatorHTML::IsStateModifierApplyIfYes");
  if (inputElt.syntacticRole != "command")
    return false;
  std::string tagClass = inputElt.GetKeyValue("class");
  if (tagClass == "setCalculatorExamHome")
  { this->flagIsExamHome = true;
    this->flagIsExamProblem = false;
    theGlobalVariables.SetWebInpuT("courseHome", HtmlRoutines::ConvertStringToURLString(this->fileName, false));
  }
  if (tagClass == "setCalculatorExamProblem")
  { this->flagIsExamHome = false;
    this->flagIsExamProblem = true;
    return true;
  }
  return false;
}

std::string CalculatorHTML::GetJavascriptSubmitAnswers()
{ std::stringstream out;
  std::string requestTypeSubmit, requestTypePreview, requestGiveUp, requestSolution;
  bool submitRandomSeed = false;
  if (theGlobalVariables.UserRequestRequiresLoadingRealExamData())
  { requestTypeSubmit = "submitAnswers";
    requestTypePreview = "submitAnswersPreview";
  } else if (theGlobalVariables.UserGuestMode())
  { requestTypeSubmit = "submitExerciseNoLogin";
    requestTypePreview = "submitExercisePreviewNoLogin";
    submitRandomSeed = true;
  } else
  { requestTypeSubmit = "submitExercise";
    requestTypePreview = "submitExercisePreview";
    submitRandomSeed = true;
  }
  if (!theGlobalVariables.UserGuestMode())
    requestGiveUp = "problemGiveUp";
  else
    requestGiveUp = "problemGiveUpNoLogin";
  if (!theGlobalVariables.UserGuestMode())
    requestSolution = "problemSolution";
  else
    requestSolution = "problemSolutionNoLogin";
  out
  << "<script type =\"text/javascript\"> \n"
  << "var JavascriptInsertionAlreadyCalled;\n"
  << "JavascriptInsertionAlreadyCalled = false;\n"
  << "var numInsertedJavascriptChildren;\n"
  << "var numInsertedJavascriptChildren = 0;\n"
  << "var timerForPreviewAnswers;\n"
  << "function previewAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  timerForPreviewAnswers =setTimeout(function(){\n"
  << "    params =\"" << this->ToStringCalculatorArgumentsForProblem(requestTypePreview, "true", "", submitRandomSeed) << "\";\n"
  << "    submitOrPreviewAnswers(idAnswer, idVerification, params,\"" << requestTypePreview
  << "\");\n"
  << "  }, 4000);"
  << "}\n"
  << "function previewAnswersNoTimeOut(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params =\"" << this->ToStringCalculatorArgumentsForProblem(requestTypePreview, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params,\""
  << requestTypePreview << "\");\n"
  << "}\n"
  << "function submitAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params =\"" << this->ToStringCalculatorArgumentsForProblem(requestTypeSubmit, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params, \""
  << requestTypeSubmit << "\");\n"
  << "}\n"
  << "function giveUp(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params =\"" << this->ToStringCalculatorArgumentsForProblem(requestGiveUp, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params, \""
  << requestGiveUp << "\");\n"
  << "}\n"
  << "function showSolution(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params =\"" << this->ToStringCalculatorArgumentsForProblem(requestSolution, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params, \""
  << requestSolution << "\");\n"
  << "}\n"
  << "function submitOrPreviewAnswers(idAnswer, idVerification, inputParams, requestType){\n";
  out << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  spanVerification = document.getElementById(idVerification);\n"
  << "  if (spanVerification == null){\n"
  << "    spanVerification = document.createElement('span');\n"
  << "    document.body.appendChild(spanVerification);\n"
  << "    spanVerification.innerHTML= \"<span style ='color:red'> ERROR: span with id \" + idVerification + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanStudentAnswer = document.getElementById(idAnswer);\n"
  << "  inputParams+=\"&calculatorAnswer\" + idAnswer\n"
  << "          + \"=\"+encodeURIComponent(spanStudentAnswer.value);\n";
  for (int i = 0; i < this->theProblemData.inputNonAnswerIds.size; i ++)
  { out << "  inputParams+=\"&userInputBox" << this->theProblemData.inputNonAnswerIds[i]
    << "=\"+encodeURIComponent(document.getElementById(\""
    << this->theProblemData.inputNonAnswerIds[i] << "\").value);\n";
  }
  out
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"GET\", ";
  out << "\"" << theGlobalVariables.DisplayNameExecutable << "\"";
  out << " + \"?\"+ inputParams, true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n";
  //Old code, submits all answers. May need to be used as an alternative
  //submission option.
  //  for (int i = 0; i < this->theContent.size; i ++)
  //    if (this->IsStudentAnswer(this->theContent[i]))
  //      out << "  inputParams+=\"&calculatorAnswer" << this->theContent[i].GetKeyValue("id") << "=\"+encodeURIComponent("
  //      << "document.getElementById('" << this->theContent[i].GetKeyValue("id") << "').value);\n";
  out
  << "  https.onload = function() {\n"
  << "    spanVerification.innerHTML=https.responseText;\n"
  << "    var scripts = spanVerification.getElementsByTagName('script');\n"
  << "    for (i = 0; i <numInsertedJavascriptChildren; i ++)\n"
  << "    { document.getElementsByTagName( 'head' )[0].removeChild(document.getElementsByTagName( 'head' )[0].lastChild);\n"
  << "      document.getElementsByTagName( 'head' )[0].appendChild(scriptChild);\n"
  << "    }\n"
  << "    numInsertedJavascriptChildren = 0;\n"
  << "    for (i = 0; i <scripts.length; i ++){\n"
  << "      scriptChild = document.createElement('script');\n"
  << "      scriptChild.innerHTML=scripts[i].innerHTML;\n"
  << "      scriptChild.type ='text/javascript';\n"
  << "      document.getElementsByTagName( 'head' )[0].appendChild(scriptChild);\n"
  << "      numInsertedJavascriptChildren ++;\n "
  << "    }\n"
  << "    JavascriptInsertionAlreadyCalled = true;\n"
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
  out //<< "<span class =\"calculatorExamProblem\">\n"
  << "Title: " << theFileName << "\n"
  << "Problem: " << theFileName << "\n"
  //<< "\n</span>"
  ;
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
//  stOutput << "Figuring out current prob list ...";
//  std::stringstream notUsed;
//  this->FigureOutCurrentProblemList(notUsed);
  inputOutput.interpretedCommand = this->ToStringProblemInfo(this->CleanUpFileName(inputOutput.content));
}

std::string CalculatorHTML::ToStringLinkCurrentAdmin
(const std::string& displayString, bool setDebugFlag, bool includeRandomSeed)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkCurrentAdmin");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "";
  std::stringstream out;
  out << "<a class =\"linkStandardButtonLike\" href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
  << theGlobalVariables.userCalculatorRequestType << "&";
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(this->fileName, false);
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  out << "fileName=" << urledProblem << "&"
  << theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (includeRandomSeed)
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
  if (setDebugFlag)
    out << "debugFlag=true&";
  else
    out << "debugFlag=false&";
  if (this->topicListFileName != "")
    out << "topicList=" << this->topicListFileName << "&";
  if (this->courseHome != "")
    out << "courseHome=" << this->courseHome << "&";
  if (theGlobalVariables.UserStudentVieWOn())
  { out << "studentView=true&";
    if (theGlobalVariables.GetWebInput("studentSection") != "")
      out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
  }
  out << "\">" << displayString << "</a>";
  return out.str();
}

std::string CalculatorHTML::ToStringLinkFromFileName(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkFromFileName");
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(theFileName, false);
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
  << "fileName=" << urledProblem << "&";
  if (theGlobalVariables.UserStudentVieWOn())
  { refStreamNoRequest << "studentView=true&";
    if (theGlobalVariables.GetWebInput("studentSection") != "")
      refStreamNoRequest << "studentSection="
      << theGlobalVariables.GetWebInput("studentSection") << "&";
  }
  if (this->topicListFileName != "")
    refStreamNoRequest << "topicList=" << this->topicListFileName << "&";
  if (this->courseHome != "")
    refStreamNoRequest << "courseHome=" << this->courseHome << "&";
  if (theFileName == this->topicListFileName || theFileName == this->courseHome ||
      MathRoutines::StringEndsWith(theFileName, ".txt"))
  { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=template&" << refStreamNoRequest.str() << "\">" << "Home" << "</a> ";
    return out.str();
  }
  if (!theGlobalVariables.UserGuestMode())
  { refStreamExercise << theGlobalVariables.DisplayNameExecutable
    << "?request=exercise&" << refStreamNoRequest.str();
    refStreamForReal << theGlobalVariables.DisplayNameExecutable
    << "?request=scoredQuiz&" << refStreamNoRequest.str();
  } else
    refStreamExercise << "?request=exerciseNoLogin&" << refStreamNoRequest.str();
  if (!theGlobalVariables.UserGuestMode())
    out << "<b><a class =\"problemLinkQuiz\" href=\"" << refStreamForReal.str() << "\">"
    << CalculatorHTML::stringScoredQuizzes << "</a></b>";
  out << "<a class =\"problemLinkPractice\" href=\"" << refStreamExercise.str() << "\">"
  << CalculatorHTML::stringPracticE << "</a>";
  //out << "DEBUG: topiclist: " << this->topicListFileName << " courseHome: " << this->courseHome
  //<< " filename: " << theFileName;
  return out.str();
}

std::string CalculatorHTML::ToStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinksFromFileName");
  std::stringstream out;
  out << this->ToStringLinkFromFileName(theFileName);
  out << this->ToStringProblemScoreFull(theFileName);
  out << this->ToStringProblemWeightButton(theFileName);
#ifdef MACRO_use_MongoDB
  bool problemAlreadySolved = false;
  if (this->currentUseR.theProblemData.Contains(theFileName))
  { ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    if (theProbData.numCorrectlyAnswered >= theProbData.theAnswers.size())
      problemAlreadySolved = true;
  }
  out << this->ToStringDeadline(theFileName, problemAlreadySolved, false, false);
#endif // MACRO_use_MongoDB
  std::string finalStringToDisplay = stringToDisplay;
  if (finalStringToDisplay == "")
    finalStringToDisplay = FileOperations::GetFileNameFromFileNameWithPath(theFileName);
  out << finalStringToDisplay;
  return out.str();
}

bool CalculatorHtmlFunctions::innerInterpretProblemGiveUp
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretProblemGiveUp");
  (void) input;
  return output.AssignValue
  (HtmlInterpretation::GetAnswerOnGiveUp(theGlobalVariables.GetWebInput("randomSeed")), theCommands);
}

bool CalculatorHtmlFunctions::innerInterpretProblem
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretProblem");
  CalculatorHTML theProblem;
  if (!input.IsOfType<std::string>(&theProblem.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  theProblem.theProblemData.flagRandomSeedGiven = true;
  theProblem.theProblemData.randomSeed = theCommands.theObjectContainer.CurrentRandomSeed;
  theProblem.InterpretHtml(theCommands.Comments);
  std::stringstream out;
  out << theProblem.outputHtmlBodyNoTag;
  out << "<hr>Time to parse html: " << std::fixed << theProblem.timeToParseHtml << " second(s). ";
  out << "<br>Intermediate interpretation times (per attempt): ";
  for (int i = 0; i < theProblem.timeIntermediatePerAttempt.size; i ++)
    for (int j = 0; j < theProblem.timeIntermediateComments[i].size; j ++)
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
  for (int i = 0; i < this->theContent.size; i ++)
    if (this->theContent[i].syntacticRole != "")
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
  for (int i = 0; i < this->theContent.size; i ++)
    out << this->theContent[i].ToStringTagAndContent();
  out << "<hr><b>The html read follows.</b><br>" << this->inputHtml << "<hr>";
  return out.str();
}

void SyntacticElementHTML::resetAllExceptContent()
{ this->tag = "";
  this->tagKeys.SetSize(0);
  this->tagValues.SetSize(0);
  this->syntacticRole = "";
  this->flagUseDisplaystyleInMathMode = false;
  this->children.SetSize(0);
}

std::string SyntacticElementHTML::ToStringOpenTag(const std::string& overrideTagIfNonEmpty, bool immediatelyClose)
{ if (this->tag == "" || this->flagUseMathSpan == false)
    return "";  
  std::stringstream out;
  if (overrideTagIfNonEmpty == "")
    out << "<" << this->tag;
  else
    out << "<" << overrideTagIfNonEmpty;
  for (int i = 0; i < this->tagKeys.size; i ++)
    out << " " << this->tagKeys[i] << "=\"" << this->tagValues[i] << "\"";
  for (int i = 0; i < this->defaultKeysIfMissing.size; i ++)
    if (!this->tagKeys.Contains(this->defaultKeysIfMissing[i]))
      out << " " << this->defaultKeysIfMissing[i] << "=\"" << this->defaultValuesIfMissing[i] << "\"";
  if (this->tagKeysWithoutValue.size > 0)
    out << " " << this->tagKeysWithoutValue[0];
  if (immediatelyClose)
    out << "/";
  out << ">";
  return out.str();
}

std::string SyntacticElementHTML::ToStringCloseTag(const std::string& overrideTagIfNonEmpty)
{ if (this->tag == "" || this->flagUseMathSpan == false)
    return "";
  if (overrideTagIfNonEmpty == "")
    return "</" + this->tag + ">";
  else
    return "</" + overrideTagIfNonEmpty + ">";
}

std::string SyntacticElementHTML::ToStringTagAndContent()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::ToStringTagAndContent");
  if (this->syntacticRole == "")
    return this->content;
  std::stringstream out;
  out << this->ToStringOpenTag("") + this->content + this->ToStringCloseTag("");
  if (this->children.size > 0)
  { out << "[";
    for (int i = 0; i < this->children.size; i ++)
    { out << this->children[i].ToStringDebug();
      if (i != this->children.size - 1)
        out << ", ";
    }
    out << "]";
  }
  return out.str();
}

std::string SyntacticElementHTML::ToStringDebug()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::ToString");
  if (this->syntacticRole == "")
    return HtmlRoutines::ConvertStringToHtmlString(this->ToStringTagAndContent(), false);
  std::stringstream out;
  out << "<span style =\"color:green\">";
  out << HtmlRoutines::ConvertStringToHtmlString(this->syntacticRole, false);
  out << "</span>";
  out << "[" << HtmlRoutines::ConvertStringToHtmlString(this->ToStringTagAndContent(), false) << "]";
  return out.str();
}

std::string SyntacticElementHTML::GetKeyValue(const std::string& theKey) const
{ MacroRegisterFunctionWithName("SyntacticElementHTML::GetKeyValue");
  int theIndex = this->tagKeys.GetIndex(theKey);
  if (theIndex == - 1)
    return "";
  return this->tagValues[theIndex];
}

void SyntacticElementHTML::SetKeyValue(const std::string& theKey, const std::string& theValue)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::SetKeyValue");
  if (this->tagKeys.size != this->tagValues.size)
    crash << "Programming error: number of keys different from number of values" << crash;
  int theIndex = this->tagKeys.GetIndex(theKey);
  if (theIndex == - 1)
  { theIndex = this->tagKeys.size;
    this->tagKeys.AddOnTop(theKey);
    this->tagValues.SetSize(this->tagKeys.size);
  }
  this->tagValues[theIndex] = theValue;
}

std::string SyntacticElementHTML::ToStringInterpretedBody()
{ if (this->syntacticRole == "")
    return this->content;
  if (this->IsInterpretedNotByCalculator())
    return this->interpretedCommand;
  std::stringstream out;
  out << this->ToStringOpenTag("");
  if (this->interpretedCommand != "")
  { if (this->flagUseMathMode)
    { out << "\\( ";
      if (this->flagUseDisplaystyleInMathMode)
        out << "\\displaystyle ";
    }
    out << this->interpretedCommand;
    if (this->flagUseMathMode)
      out << " \\)";
  }
  out << this->ToStringCloseTag("");
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedNotByCalculator()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::IsInterpretedNotByCalculator");
  if (this->syntacticRole != "command")
    return false;
  if (this->tag == "answerCalculatorHighlightStart" ||
      this->tag == "answerCalculatorHighlightFinish")
    return true;
  std::string tagClass = this->GetKeyValue("class");
  return
  tagClass == "calculatorExamProblem" || tagClass == "calculatorExamIntermediate" ||
  tagClass == "calculatorAnswer" || tagClass == "calculatorManageClass" ||
  tagClass == "generateTopicTable" ||
  tagClass == "generateLectureMaterials" ||
  tagClass == "calculatorJavascript" ||
  tagClass == "accountInformationLinks" ||
  tagClass == "generateTableOfContents" ||
  tagClass == "calculatorNavigationHere" ||
  tagClass == "calculatorProblemNavigationHere"||
  tagClass == "calculatorEditPageHere" ||
  this->IsAnswerElement(0);
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringProblemGeneration()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculator" || tagClass == "calculatorHidden" ||
  tagClass == "calculatorShowToUserOnly" ;
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringSubmission()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculator" || tagClass == "calculatorHidden";
}

bool SyntacticElementHTML::IsAnswer()
{ if (this->syntacticRole != "command")
    return false;
  return this->GetKeyValue("class") == "calculatorAnswer";
}

bool SyntacticElementHTML::IsCalculatorCommand()
{ if (this->syntacticRole != "command")
    return false;
  return this->GetKeyValue("class") == "calculator";
}

bool SyntacticElementHTML::IsCalculatorHidden()
{ if (this->syntacticRole != "command")
    return false;
  return this->GetKeyValue("class") == "calculatorHidden";
}

bool SyntacticElementHTML::IsHidden()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorHidden" || tagClass == "calculatorCommentsBeforeInterpretation";
}

bool SyntacticElementHTML::IsSolution()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorSolution"
;
}

bool SyntacticElementHTML::IsAnswerOnGiveUp()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorAnswerOnGiveUp";
}

bool SyntacticElementHTML::IsCommentBeforeSubmission()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorCommentsBeforeSubmission";
}

bool SyntacticElementHTML::IsCommentBeforeInterpretation()
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorCommentsBeforeInterpretation";
}

bool SyntacticElementHTML::IsAnswerElement(std::string* desiredAnswerId)
{ if (this->syntacticRole != "command")
    return false;
  std::string tagClass = this->GetKeyValue("class");
  bool result = tagClass == "calculatorButtonSubmit" ||
  tagClass == "calculatorButtonInterpret" ||
  tagClass == "calculatorButtonGiveUp" ||
  tagClass == "calculatorButtonSolution" ||
  tagClass == "calculatorMQField" ||
  tagClass == "calculatorMQButtonPanel" ||
  tagClass == "calculatorAnswerVerification" ||
  tagClass == "calculatorSolution";
  if (result && desiredAnswerId != 0)
    *desiredAnswerId = this->GetKeyValue("name");
  return result;
}

std::string CalculatorHTML::PrepareUserInputBoxes()
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareInterpreter");
  if (this->flagIsForReal)
    return "";
  std::stringstream out;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs = theGlobalVariables.webArguments;
  std::string inputNonAnswerReader;
  for (int i = 0; i < theArgs.size(); i ++)
    if (MathRoutines::StringBeginsWith(theArgs.theKeys[i], "userInputBox", &inputNonAnswerReader))
      if (inputNonAnswerReader != "" && theArgs.theValues[i] != "")
      { out << "setInputBox(name ="
        << inputNonAnswerReader
        << ", value =" << HtmlRoutines::ConvertURLStringToNormal(theArgs.theValues[i], false)
        << "); ";
      }
  return out.str();
}

std::string CalculatorHTML::GetProblemHeaderEnclosure()
{ std::stringstream out;
  out << "CommandEnclosure{}(";
  out <<  " SetRandomSeed{}(" << this->theProblemData.randomSeed << "); ";
  out << this->PrepareUserInputBoxes();
  out << "); ";
  return out.str();
}

std::string CalculatorHTML::GetProblemHeaderWithoutEnclosure()
{ std::stringstream out;
  out <<  " SetRandomSeed{}(" << this->theProblemData.randomSeed << "); ";
  out << this->PrepareUserInputBoxes();
  return out.str();
}

bool CalculatorHTML::PrepareCommandsGenerateProblem(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsGenerateProblem");
  (void) comments;
  std::stringstream streamCommands, streamCommandsNoEnclosures;
  streamCommandsNoEnclosures << this->GetProblemHeaderWithoutEnclosure();
  streamCommands << this->GetProblemHeaderEnclosure();//first calculator enclosure contains the header
  int numCommandsSoFar = 2;//two commands at the start: the opEndStatement command and
  // the first enclosure.
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration())
      continue;
    std::string commandCleaned = this->CleanUpCommandString(currentElt.content);
    std::string commandEnclosed = "CommandEnclosure{}( " + commandCleaned + " );";
    streamCommands << commandEnclosed;
    streamCommandsNoEnclosures << commandCleaned;
    currentElt.commandIndex = numCommandsSoFar;
    numCommandsSoFar ++;
  }
  this->theProblemData.commandsGenerateProblem = streamCommands.str();
  this->theProblemData.commandsGenerateProblemNoEnclosures = streamCommandsNoEnclosures.str();
  std::stringstream debugStream;
  debugStream << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
  << "?request=calculator&mainInput="
  << HtmlRoutines::ConvertStringToURLString(this->theProblemData.commandsGenerateProblemNoEnclosures, false)
  << "\"> "
  << "Input link </a>";
  this->theProblemData.commandsGenerateProblemLink = debugStream.str();
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
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++)
  { if (!this->PrepareCommandsAnswer(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommandsAnswerOnGiveUp(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommandsSolution(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommentsBeforeSubmission(this->theProblemData.theAnswers[i], comments))
      return false;
    if (!this->PrepareCommentsBeforeInterpretation(this->theProblemData.theAnswers[i], comments))
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
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsAnswerOnGiveUp())
      continue;
    //stOutput << "<br>Current element: " << currentElt.ToStringDebug();
    //stOutput << "<br>Comparing: " << theAnswer.answerId << " to: "
    //<< currentElt.GetKeyValue("name");
    if (currentElt.GetKeyValue("name") == theAnswer.answerId)
      streamCommands << this->CleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly = streamCommands.str();
  //stOutput << "<br>Final give up command: " << theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly;
  return true;
}

bool CalculatorHTML::PrepareCommentsBeforeSubmission
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommentsBeforeSubmission");
  (void) comments;
  std::stringstream streamCommands;
  //stOutput << "<hr>DEBUG: Preparing comment commands for: "
  //<< theAnswer.answerId << "<hr>";
//  stOutput << "<hr>DEBUG: Call stack: " << crash.GetStackTraceEtcErrorMessage();
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsCommentBeforeSubmission())
      continue;
    if (currentElt.GetKeyValue("name") != theAnswer.answerId)
      continue;
    streamCommands << this->CleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsCommentsBeforeSubmission = streamCommands.str();
  //stOutput << "<br>DEBUG: Final comments command: "
  //<< theAnswer.commandsBeforeInterpretation;
  return true;
}

bool CalculatorHTML::PrepareCommentsBeforeInterpretation
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommentsBeforeInterpretation");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsCommentBeforeInterpretation())
      continue;
    if (currentElt.GetKeyValue("name") != theAnswer.answerId)
      continue;
    streamCommands << this->CleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsCommentsBeforeInterpretatioN = streamCommands.str();
  return true;
}

bool CalculatorHTML::PrepareCommandsSolution
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsSolution");
  (void) comments;
  std::stringstream streamCommands;
  //stOutput << "<hr>DEBUG: Preparing give-up commands for: " << theAnswer.answerId << "<hr>";
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& solutionElt = this->theContent[i];
    if (!solutionElt.IsSolution())
      continue;
    //stOutput << "<br>Current element: " << currentElt.ToStringDebug();
    //stOutput << "<br>Comparing: " << theAnswer.answerId << " to: "
    //<< currentElt.GetKeyValue("name");
    if (solutionElt.GetKeyValue("name") != theAnswer.answerId)
      continue;
    int numCommandsSoFar = 2;
    theAnswer.solutionElements = solutionElt.children;
    for (int j = 0; j < theAnswer.solutionElements.size; j ++)
    { SyntacticElementHTML& currentElt = theAnswer.solutionElements[j];
      if (!currentElt.IsCalculatorCommand() && !currentElt.IsCalculatorHidden())
        continue;
      currentElt.commandIndex = numCommandsSoFar;
      numCommandsSoFar ++;
      streamCommands << "CommandEnclosure{}("
      << this->CleanUpCommandString(currentElt.content) << "); ";
    }
  }
  theAnswer.commandsSolutionOnly =streamCommands.str();
  //stOutput << "<br>Final give up command: " << theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly;
  return true;
}

bool CalculatorHTML::PrepareCommandsAnswer
(Answer& theAnswer, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsAnswer");
  std::stringstream streamCommandS;
  std::stringstream streamCommandsNoEnclosures;
  streamCommandS << this->GetProblemHeaderEnclosure();//first calculator enclosure contains the header
  streamCommandsNoEnclosures << this->GetProblemHeaderWithoutEnclosure();
  std::stringstream streamCommandsBody;
  std::stringstream streamCommandsBodyNoEnclosures;
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsCalculatorHidden() && !currentElt.IsCalculatorCommand()
        && !currentElt.IsAnswer())
      continue;
    std::string commandCleaned = this->CleanUpCommandString(this->theContent[i].content);
    std::string commandEnclosed = "CommandEnclosure{}( " + commandCleaned + " );";
    if (currentElt.IsAnswer() && currentElt.GetKeyValue("id") == theAnswer.answerId)
    { std::string stringCommandsBody = streamCommandsBody.str();
      if (stringCommandsBody != "")
      { streamCommandS << "CommandEnclosure{}(" << stringCommandsBody << ");\n";
        streamCommandsNoEnclosures << streamCommandsBodyNoEnclosures.str();
      }
      theAnswer.commandsBeforeAnswer = streamCommandS.str();
      theAnswer.commandsBeforeAnswerNoEnclosuresForDEBUGGING = streamCommandsNoEnclosures.str();
      theAnswer.commandVerificationOnly = commandCleaned;
      return true;
    }
    if (this->theContent[i].IsCalculatorHidden() || this->theContent[i].IsCalculatorCommand())
    { streamCommandsBody << commandEnclosed;
      streamCommandsBodyNoEnclosures << commandCleaned;
    }
  }
  comments << "<b>Something is wrong: did not find answer for answer tag: "
  << theAnswer.answerId << ". </b>";
  return false;
}

bool CalculatorHTML::PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::PrepareAndExecuteCommands");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  this->PrepareCommands(comments);

  theInterpreter.init();
  theInterpreter.flagWriteLatexPlots = false;
  theInterpreter.flagPlotNoControls = true;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator init time");

  //stOutput << "nput cmds: " << calculatorCommands.str();
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasProblemComposingRights())
  { this->logCommandsProblemGeneratioN << "<b>Input commands:</b> "
    << this->theProblemData.commandsGenerateProblemLink
    << "<br>\n"
    << this->theProblemData.commandsGenerateProblem << "<br>";
  }
  theInterpreter.Evaluate(this->theProblemData.commandsGenerateProblem);


  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator evaluation time");
  //stOutput << "<hr>Fter eval: " << theInterpreter.outputString;
  bool result = !theInterpreter.flagAbortComputationASAP && theInterpreter.syntaxErrors == "";
  if (!result)
  { comments << "<br>Failed to interpret your file. "
    << "<a href=\""
    << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString( this->theProblemData.commandsGenerateProblemNoEnclosures,false)
    << "\">Input link</a><br>"
    << "The interpretation input was:<br> "
    << this->theProblemData.commandsGenerateProblem << "<br>";
    if (theGlobalVariables.UserDefaultHasAdminRights())
      comments << "The result of the interpretation attempt is:<br>"
      << theInterpreter.outputString << "<br><b>Comments</b><br>"
      << theInterpreter.outputCommentsString;
    else
      comments << "This may be a bug with the problem. Feel free to take a screenshot of the issue and "
      << "email it to the site admin(s). ";
  }
  for (int i = 0; i < theInterpreter.theObjectContainer.theUserInputTextBoxesWithValues.size(); i ++)
    this->theProblemData.inputNonAnswerIds.AddOnTop
    (theInterpreter.theObjectContainer.theUserInputTextBoxesWithValues.theKeys[i]);
  return result;
}

std::string SyntacticElementHTML::GetTagClass()
{ return this->GetKeyValue("class");
}

bool CalculatorHTML::PrepareSectionList(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareSectionList");
#ifdef MACRO_use_MongoDB
  (void) commentsOnFailure;
  if (this->flagSectionsPrepared)
    return true;
  this->flagSectionsPrepared = true;
  if (this->currentUseR.sectionsTaught.size == 0 ||
      (this->currentUseR.userRole != "admin" && this->currentUseR.userRole != "teacher") )
    if (this->currentUseR.sectionComputed != "")
    { this->databaseStudentSections.AddOnTop(this->currentUseR.sectionComputed);
      return true;
    }
  this->databaseStudentSections.AddListOnTop(this->currentUseR.sectionsTaught);
  return true;
#else
  commentsOnFailure << "Database not running. ";
  return false;
#endif
}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentVieWOn())
    return;
#ifdef MACRO_use_MongoDB
  std::stringstream out;
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts\"> Manage accounts</a>";
  inputOutput.interpretedCommand = out.str();
#else
  inputOutput.interpretedCommand = "<b>Managing class not available (no database).</b>";
#endif // MACRO_use_MongoDB
}

bool CalculatorHTML::ComputeAnswerRelatedStrings(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::ComputeAnswerRelatedStrings");
  std::string desiredAnswerId = inputOutput.GetKeyValue("id");
  if (desiredAnswerId == "")
  { inputOutput.interpretedCommand = "<b>Error: could not generate submit button: the answer tag does not have a valid id. Please fix the problem template.</b>";
    return false;
  }
  int theIndex = this->GetAnswerIndex(desiredAnswerId);
  if (theIndex == - 1)
    crash  << "This is not supposed to happen: problem has syntactic element with answerId: "
    << desiredAnswerId << " but the answerId is missing from the list of known answer ids. "
    << this->theProblemData.ToStringAvailableAnswerIds() << crash;
  Answer& currentA = this->theProblemData.theAnswers[theIndex];
  if (theIndex < this->answerHighlights.size)
    currentA.htmlAnswerHighlight = this->answerHighlights[theIndex];
  else
    currentA.htmlAnswerHighlight = "";
  std::string& answerId = currentA.answerId;
  currentA.varAnswerId = answerId + "spanVariable";
  currentA.varMQfield = answerId + "MQspanVar";
  currentA.MQobject = "answerMathQuillObjects[" + std::to_string(this->NumAnswerIdsMathquilled) + "]";
  this->NumAnswerIdsMathquilled ++;
  currentA.MQUpdateFunction = answerId + "MQFieldUpdate";
  currentA.idVerificationSpan = "verification" + answerId;
  currentA.idSpanSolution = "solution" + answerId;
  if (currentA.idMQfield == "")
    currentA.idMQfield = answerId + "MQspanId";
  if (currentA.idMQButtonPanelLocation == "")
    currentA.idMQButtonPanelLocation = answerId + "MQbuttonPanel";
  std::stringstream previewAnswerStream;

  previewAnswerStream << "previewAnswers('" << answerId << "', '"
  << currentA.idVerificationSpan << "');";

  currentA.javascriptPreviewAnswer = previewAnswerStream.str();
  currentA.idButtonSubmit = "buttonSubmit" + answerId;
  currentA.idButtonInterpret = "buttonInterpret" + answerId;
  currentA.idButtonAnswer = "buttonAnswer" + answerId;

  currentA.htmlButtonSubmit = "<button class =\"buttonSubmit\" id =\"" + currentA.idButtonSubmit + "\"";
  if (!this->flagUseJSON)
    currentA.htmlButtonSubmit += " onclick = \"submitAnswers('" + answerId + "', '" + currentA.idVerificationSpan + "')\"";
  currentA.htmlButtonSubmit +=  ">Submit</button>";
  currentA.htmlButtonInterpret = (std::string)"<button class =\"buttonPreview\" ";
  currentA.htmlButtonInterpret += " id =\"" + currentA.idButtonInterpret + "\" ";
  if (!this->flagUseJSON)
    currentA.htmlButtonInterpret += "onclick=\"previewAnswersNoTimeOut('" + answerId + "', '" + currentA.idVerificationSpan + "')" + "\"";
  currentA.htmlButtonInterpret += ">Interpret</button>";
  if (!this->flagIsForReal)
  { if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly != "")
    { currentA.htmlButtonAnswer = "<button id =\"" + currentA.idButtonAnswer +
      "\" class =\"buttonAnswer\"";
      if (!this->flagUseJSON)
        currentA.htmlButtonAnswer += " onclick=\"giveUp('" + answerId + "', '" + currentA.idVerificationSpan + "')\"";
      currentA.htmlButtonAnswer += ">Answer</button>";
    } else
      currentA.htmlButtonAnswer = "No ``give-up'' answer available. ";
    if (currentA.flagSolutionFound)
    { currentA.htmlButtonSolution = "<button class =\"buttonSolution\"";
      if (!this->flagUseJSON)
        currentA.htmlButtonSolution += " onclick=\"showSolution('" + answerId +
        "','" + currentA.idSpanSolution + "')\"";
      currentA.htmlButtonSolution += "> Solution</button>";
    } else
      currentA.htmlButtonSolution = "";
  }
  if (!this->flagUseJSON)
  { inputOutput.defaultKeysIfMissing.AddOnTop("onkeyup");
    inputOutput.defaultValuesIfMissing.AddOnTop
    (currentA.javascriptPreviewAnswer + currentA.MQUpdateFunction + "();");
  }
//  inputOutput.defaultKeysIfMissing.AddOnTop("style");
//  inputOutput.defaultValuesIfMissing.AddOnTop("height:70px");
  currentA.htmlTextareaLatexAnswer =
  inputOutput.ToStringOpenTag("textarea") + inputOutput.ToStringCloseTag("textarea");
  currentA.htmlSpanMQfield =
  (std::string)"<div class =\"calculatorMQfieldEnclosure\">" +
  "<span id ='" + currentA.idMQfield + "'>" + "</span>" +
  "</div>";
  //currentA.htmlMQjavascript = CalculatorHtmlFunctions::GetJavascriptMathQuillBox(currentA);
  currentA.htmlSpanMQButtonPanel =
  "<div class =\"mqButtonPanel\" id =\"" + currentA.idMQButtonPanelLocation + "\" "+  "buttons =\"" +
  currentA.MQpanelButtonOptions + "\"></div>";
  currentA.htmlSpanSolution = "<span id =\"" + currentA.idSpanSolution + "\"></span>";
  std::stringstream verifyStream;
  verifyStream << "<span id =\"" << currentA.idVerificationSpan << "\">";
  int numCorrectSubmissions = currentA.numCorrectSubmissions;
  int numSubmissions = currentA.numSubmissions;
  if (theGlobalVariables.userCalculatorRequestType == "scoredQuiz" ||
      theGlobalVariables.userCalculatorRequestType == "scoredQuizJSON")
  { if (numCorrectSubmissions > 0)
    { verifyStream << "<b><span style =\"color:green\">Correctly answered: \\("
      << currentA.firstCorrectAnswerClean << "\\) </span></b> ";
      if (numSubmissions > 0)
        verifyStream << "<br>Used: " << numSubmissions << " attempt(s) (" << numCorrectSubmissions << " correct).";
    } else
    { verifyStream << " <b><span style =\"color:brown\">Waiting for answer. </span></b>";
      if (numSubmissions > 0)
        verifyStream << numSubmissions << " attempt(s) so far. ";
    }
  } else
    verifyStream << " <b><span style =\"color:brown\">Waiting for answer [unlimited tries]</span></b>";
  verifyStream << "</span>";
  currentA.htmlSpanVerifyAnswer = verifyStream.str();
  return true;
}

void CalculatorHTML::InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateStudentAnswerButton");
  if (!this->ComputeAnswerRelatedStrings(inputOutput))
    return;
  Answer& currentA = this->theProblemData.
  theAnswers[this->GetAnswerIndex(inputOutput.GetKeyValue("id"))];
  if (!currentA.flagAutoGenerateMQButtonPanel && !currentA.flagAutoGenerateMQfield &&
      !currentA.flagAutoGenerateSubmitButtons && !currentA.flagAutoGenerateVerificationField)
  { inputOutput.interpretedCommand = currentA.htmlTextareaLatexAnswer;
    return;
  }
  std::stringstream out;
  out << "<br><span class =\"panelAnswer\">";
  out << "<table>";
  out << "<tr><td>";
  out << "<table><tr>";
  if (currentA.htmlAnswerHighlight != "")
    out << "<td><answerCalculatorHighlight>" << currentA.htmlAnswerHighlight << "</answerCalculatorHighlight></td>";
  if (currentA.flagAutoGenerateMQfield)
    out << "<td class =\"tableCellMQfield\">" << currentA.htmlSpanMQfield  << "</td>";
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
  out << "<button class =\"accordion\">details</button>";
//  out << "<div class =\"panel\">";
  out << currentA.htmlTextareaLatexAnswer;
//  out << "</div>";
  out << "</td>";
  out << "</tr>";
  out << "</table>";
  if (currentA.flagAutoGenerateVerificationField)
    out << currentA.htmlSpanVerifyAnswer;
  out << "</span>";

  inputOutput.interpretedCommand = out.str();
}

void CalculatorHTML::InterpretIfAnswer(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretIfAnswer");
  std::string tagClass = inputOutput.GetTagClass();
  if (tagClass != "calculatorAnswer")
    return;
  this->InterpretGenerateStudentAnswerButton(inputOutput);
}

void CalculatorHTML::InterpretNotByCalculatorNotAnswer(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretNotByCalculatorNotAnswer");
  std::string tagClass = inputOutput.GetTagClass();
  //std::string tag= inputOutput.tag;
  if (tagClass == "calculatorExamProblem" || tagClass == "calculatorExamIntermediate")
    this->InterpretGenerateLink(inputOutput);
  else if (tagClass == "calculatorManageClass")
    this->InterpretManageClass(inputOutput);
  else if (tagClass == "generateTopicTable")
    this->InterpretTopicList(inputOutput);
  else if (tagClass == "generateLectureMaterials")
    this->InterpretLectureMaterials(inputOutput);
  else if (tagClass == "generateTableOfContents")
    this->InterpretTableOfContents(inputOutput);
  else if (tagClass == "accountInformationLinks")
    this->InterpretAccountInformationLinks(inputOutput);
  else if (tagClass == "calculatorJavascript")
    this->InterpretJavascripts(inputOutput);
  else if (tagClass == "calculatorNavigationHere")
    this->InterpretCalculatorNavigationBar(inputOutput);
  else if (tagClass == "calculatorProblemNavigationHere")
    this->InterpretProblemNavigationBar(inputOutput);
  else if (tagClass == "calculatorEditPageHere")
    this->InterpretEditPagePanel(inputOutput);
}

std::string CalculatorHTML::CleanUpFileName(const std::string& inputLink)
{ MacroRegisterFunctionWithName("CalculatorHTML::CleanUpFileName");
  if (inputLink.size() == 0)
    return inputLink;
  unsigned firstMeaningfulChar = 0;
  for (; firstMeaningfulChar < inputLink.size(); firstMeaningfulChar ++)
    if (inputLink[firstMeaningfulChar] != '\n' &&
        inputLink[firstMeaningfulChar] != '\r' &&
        inputLink[firstMeaningfulChar] != '\t' &&
        inputLink[firstMeaningfulChar] != ' ')
      break;
  unsigned lastMeaningfulChar = inputLink.size() - 1;
  for (; lastMeaningfulChar > firstMeaningfulChar; lastMeaningfulChar --)
    if (inputLink[lastMeaningfulChar] != '\n' &&
        inputLink[lastMeaningfulChar] != '\r' &&
        inputLink[lastMeaningfulChar] != '\t' &&
        inputLink[lastMeaningfulChar] != ' ')
      break;
  if (firstMeaningfulChar >= inputLink.size())
    return "";
  return inputLink.substr(firstMeaningfulChar, lastMeaningfulChar - firstMeaningfulChar + 1);
}

#include "vpfHeader5Crypto.h"

std::string CalculatorHTML::GetDeadlineNoInheritance(const std::string& id)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetDeadlineNoInheritance");
#ifdef MACRO_use_MongoDB
  if (!this->currentUseR.theProblemData.Contains(id))
    return "";
  ProblemDataAdministrative& currentProb =
  this->currentUseR.theProblemData.GetValueCreateNoInit((id)).adminData;
  if (!currentProb.deadlinesPerSection.Contains(this->currentUseR.sectionComputed))
    return "";
  return currentProb.deadlinesPerSection.GetValueCreate(this->currentUseR.sectionComputed);
#else
  return "Database not present. ";
#endif
}

std::string CalculatorHTML::GetDeadline
(const std::string& problemName, const std::string& sectionNumber, bool& outputIsInherited)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetDeadline");
  (void) problemName;
  (void) sectionNumber;
  (void) outputIsInherited;
  outputIsInherited = true;
  std::string result;
  #ifdef MACRO_use_MongoDB
  int topicIndex = this->theTopicS.GetIndex(problemName);
  if (topicIndex == - 1)
    return problemName + " not found in topic list. ";
  TopicElement& currentTopic = this->theTopicS.GetValueCreate(problemName);
  for (int i = currentTopic.parentTopics.size - 1; i >= 0; i --)
  { const std::string& containerName = this->theTopicS.theKeys[currentTopic.parentTopics[i]];
    if (this->currentUseR.theProblemData.Contains(containerName))
    { ProblemDataAdministrative& currentProb =
      this->currentUseR.theProblemData.GetValueCreateNoInit(containerName).adminData;
      result = currentProb.deadlinesPerSection.GetValueCreate(sectionNumber);
      if (MathRoutines::StringTrimWhiteSpace(result) != "")
      { outputIsInherited = (containerName != problemName);
        return result;
      }
    }
  }
  #endif
  return result;
}

std::string CalculatorHTML::ToStringOnEDeadlineFormatted
(const std::string& topicID, const std::string& sectionNumber,
 bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection)
{ std::stringstream out;
  (void) problemAlreadySolved;
  (void) isSection;
  bool deadlineIsInherited = false;
  std::string currentDeadline = this->GetDeadline(topicID, sectionNumber, deadlineIsInherited);
  if (currentDeadline == "")
  { if (returnEmptyStringIfNoDeadline)
      return "";
    out << "<span style =\"color:orange\">No deadline yet</span>";
    //out << "DEBUG: section: " << sectionNumber;
    return out.str();
  }
#ifdef MACRO_use_MongoDB
  TimeWrapper now, deadline; //<-needs a fix for different time formats.
  //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
  std::stringstream badDateStream;
  if (!deadline.AssignMonthDayYear(currentDeadline, badDateStream))
    out << "<span style =\"color:red\">" << badDateStream.str() << "</span>";
  //  out << "deadline.date: " << deadline.theTime.tm_mday;
  now.AssignLocalTime();
  //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
  //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
  double secondsTillDeadline = deadline.SubtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;

  std::stringstream hoursTillDeadlineStream;
  bool deadlineIsNear = secondsTillDeadline < 24 * 3600 && !problemAlreadySolved && !isSection;
  bool deadlineHasPassed = (secondsTillDeadline < 0);
  //if (isSection)
  //  out << "DEBUG: isSection = true";
  if (deadlineIsInherited && !theGlobalVariables.UserStudentVieWOn())
    out << "Inherited: ";
  else if (deadlineIsInherited && isSection && returnEmptyStringIfNoDeadline)
    return "";
  else
    out << "Deadline: ";
  if (!deadlineHasPassed)
  { if (deadlineIsNear)
      hoursTillDeadlineStream << "<span style =\"color:red\">"
      << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
      (secondsTillDeadline, false, true) << "</span>";
    else
      hoursTillDeadlineStream << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
      (secondsTillDeadline, false, true)
      << " left. ";
  } else
    hoursTillDeadlineStream << "[passed].";
  if (deadlineHasPassed && !problemAlreadySolved)
  { out << "<span style =\"color:blue\">" << currentDeadline << "</span> ";
    out << "<span style =\"red\"><b>[passed].</b></span>";
  } else
  { if (problemAlreadySolved && !isSection)
      out << "<span style =\"color:green\">" << currentDeadline << "</span> ";
    else if (deadlineIsNear && !isSection)
      out << "<span style =\"color:red\">" << currentDeadline << "</span> ";
    else
      out << "<span style =\"color:brown\">" << currentDeadline << "</span> ";
    out << hoursTillDeadlineStream.str();
  }
  return
  //"[<span style =\"color:green\"><b>disabled</b> </span>] "+
  out.str();
#else
  out  << "Database not running: no deadlines";
  return out.str();
#endif // MACRO_use_MongoDB
}

std::string CalculatorHTML::ToStringAllSectionDeadlines
  (const std::string& topicID, bool isSection)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringAllSectionDeadlines");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "";
  std::stringstream out;
  out << "<table>";
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { if (this->databaseStudentSections[i] == "")
      continue;
    out << "<tr><td>Section " << this->databaseStudentSections[i] << ":</td>";
    out << "<td>" << this->ToStringOnEDeadlineFormatted
    (topicID, this->databaseStudentSections[i], false, false, isSection) << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string CalculatorHTML::ToStringDeadline
(const std::string& topicID, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineWithModifyButton");
  (void) topicID;
  (void) problemAlreadySolved;
  (void) returnEmptyStringIfNoDeadline;
  (void) isSection;
#ifdef MACRO_use_MongoDB
  if (theGlobalVariables.UserGuestMode())
    return "deadlines require login";
  else if (theGlobalVariables.UserDefaultHasAdminRights() &&
             theGlobalVariables.UserStudentVieWOn())
  { std::string sectionNum = HtmlRoutines::ConvertURLStringToNormal
    (theGlobalVariables.GetWebInput("studentSection"), false);
    return this->ToStringOnEDeadlineFormatted
    (topicID, sectionNum, problemAlreadySolved,
     returnEmptyStringIfNoDeadline, isSection);
  } else
    return this->ToStringOnEDeadlineFormatted
    (topicID, this->currentUseR.sectionComputed, problemAlreadySolved,
     returnEmptyStringIfNoDeadline, isSection);
#endif // MACRO_use_MongoDB
  return "";
}

void CalculatorHTML::ComputeDeadlinesAllSections(TopicElement& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::ComputeDeadlinesAllSections");
  inputOutput.deadlinesPerSectioN.initFillInObject(this->databaseStudentSections.size, "");
  inputOutput.deadlinesAreInherited.initFillInObject(this->databaseStudentSections.size, false);
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { inputOutput.deadlinesPerSectioN[i] = this->GetDeadline
    (inputOutput.id, this->databaseStudentSections[i], inputOutput.deadlinesAreInherited[i]);
    if (inputOutput.deadlinesAreInherited[i])
      inputOutput.deadlinesPerSectioN[i] = "";
  }
}

void CalculatorHTML::ComputeDeadlinesAllSectionsNoInheritance(TopicElement& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::ComputeDeadlinesAllSectionsNoInheritance");
  inputOutput.deadlinesPerSectioN.initFillInObject(this->databaseStudentSections.size, "");
  //stOutput << "<hr>DEBUG: deadlinespersection: "
  //<< this->databaseStudentSections.ToStringCommaDelimited() << "<hr>";
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { ProblemDataAdministrative& currentProb =
    this->currentUseR.theProblemData.GetValueCreateNoInit(inputOutput.id).adminData;
    inputOutput.deadlinesPerSectioN[i] =
    currentProb.deadlinesPerSection.GetValueCreate(this->databaseStudentSections[i]);
  }
}

void CalculatorHTML::ComputeDeadlineModifyButton
(TopicElement& inputOutput, bool problemAlreadySolved, bool isProblemGroup)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineModifyButton");
  if (!theGlobalVariables.UserDefaultHasProblemComposingRights())
    return;
  this->ComputeDeadlinesAllSections(inputOutput);
//    this->id, this->displayDeadline
  (void) problemAlreadySolved;
  std::stringstream out;
  std::stringstream deadlineStream;
  inputOutput.idDeadlineTable = "deadlineTable" +
  Crypto::computeSha1outputBase64(inputOutput.id);
  inputOutput.idDeadlineButton = "deadlineButton" +
  Crypto::computeSha1outputBase64(inputOutput.id);
  deadlineStream << "<table class =\"deadlineTable\" id =\""
  << inputOutput.idDeadlineTable
  << "\">";
  deadlineStream << "<tr><th>Grp.</th><th>Deadline</th></tr>";
  inputOutput.idsDeadlines.SetSize(this->databaseStudentSections.size);
  inputOutput.deadlinesPerSectionFormatted.initFillInObject
  (this->databaseStudentSections.size, "");
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { std::string& currentDeadlineId = inputOutput.idsDeadlines[i];
    if (this->databaseStudentSections[i] == "")
      continue;
    currentDeadlineId = "deadline" + Crypto::ConvertStringToBase64
    (this->databaseStudentSections[i] + inputOutput.id);
    if (currentDeadlineId[currentDeadlineId.size() - 1] == '=')
      currentDeadlineId.resize(currentDeadlineId.size() - 1);
    if (currentDeadlineId[currentDeadlineId.size() - 1] == '=')
      currentDeadlineId.resize(currentDeadlineId.size() - 1);
    inputOutput.deadlinesAreInherited[i] = false;
//    std::string sectionNumber;
//    std::string currentDeadline =
//    this->GetDeadline(inputFileName, sectionNumber, deadlineInherited);
    deadlineStream << "<tr>";
    deadlineStream << "<td>" << this->databaseStudentSections[i] << "</td>";
    inputOutput.deadlinesPerSectionFormatted[i] =
    this->ToStringOnEDeadlineFormatted
    (inputOutput.id, this->databaseStudentSections[i],
     problemAlreadySolved, false, isProblemGroup);
    deadlineStream
    << "<td> <input class =\"modifyDeadlineInput\" type =\"text\" id =\""
    << currentDeadlineId << "\" value =\"";
    if (!inputOutput.deadlinesAreInherited[i])
      deadlineStream << inputOutput.deadlinesPerSectioN[i];
    deadlineStream << "\"> " ;
    deadlineStream << "</td>";
    deadlineStream << "</tr>";
  }
  deadlineStream << "<tr><td>\n";
  inputOutput.idDeadlineReport = "deadlineReport" +
  Crypto::computeSha1outputBase64(inputOutput.id);

  if (inputOutput.idDeadlineReport[inputOutput.idDeadlineReport.size() - 1] == '=')
    inputOutput.idDeadlineReport.resize(inputOutput.idDeadlineReport.size() - 1);
  if (inputOutput.idDeadlineReport[inputOutput.idDeadlineReport.size() - 1] == '=')
    inputOutput.idDeadlineReport.resize(inputOutput.idDeadlineReport.size() - 1);
  deadlineStream << "<button onclick=\"";
  deadlineStream << "submitStringAsMainInput('"
  << HtmlRoutines::ConvertStringToURLString(inputOutput.id, false)
  << "='+encodeURIComponent('deadlines ='+encodeURIComponent(";
  bool isFirst = true;
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { if (this->databaseStudentSections[i] == "")
      continue;
    if (!isFirst)
      deadlineStream << "+";
    isFirst = false;
    deadlineStream << "'"
    << HtmlRoutines::ConvertStringToURLString(this->databaseStudentSections[i], false)
    << "='";
    deadlineStream << "+ encodeURIComponent(document.getElementById('"
    << inputOutput.idsDeadlines[i] << "').value) +'&'";
  }
  deadlineStream << ")), '" << inputOutput.idDeadlineReport << "',"
  << " 'setProblemData', "
  << "null, "
  << "'" << inputOutput.idDeadlineReport << "' ); "
  << "updateDeadlines('" << inputOutput.idBase64 << "', '"
  << inputOutput.idDeadlineTable << "');"
  << "\""
  << ">\n";
  deadlineStream << "<b>Set</b></button>";
  deadlineStream << "</td>";
  deadlineStream << "<td>";
  deadlineStream << "<span id =\"" << inputOutput.idDeadlineReport << "\"></span>";
  deadlineStream << "</td>";
  deadlineStream << "</tr>";
  if (!isProblemGroup)
    deadlineStream << "<tr><td colspan =\"2\">(overrides section deadline).</td></tr> ";
  else
    deadlineStream << "<tr><td colspan =\"2\">(overriden by per-problem-deadline).</td></tr>";
  deadlineStream << "</table>";
//  out << deadlineStream.str();
//  out << "<table>";
//  out << "<tr><td>";
  out << "<button class =\"accordion\" id =\""
  << inputOutput.idDeadlineButton
  << "\">"
  << inputOutput.displayDeadlinE << "</button>"
//  << "</td></tr>"
  ;
//  out << "<tr class =\"panel\"><td>";
  out << "<span class =\"panel\">";
  out << deadlineStream.str();
  out << "</span>";
//  out << "</td></tr>";
//  out << "</table>";

//  out << HtmlRoutines::GetHtmlSpanHidableStartsHiddeN(deadlineStream.str(), "deadline + ");
  inputOutput.displayDeadlinE = out.str();
}

std::string CalculatorHTML::ToStringInterprettedCommands(Calculator &theInterpreter, List<SyntacticElementHTML> &theElements)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringInterprettedCommands");
  std::stringstream out;
  out << "<table>";
  int commandCounter = theInterpreter.theProgramExpression.size() - 1;
  for (int eltCounter = theElements.size - 1; eltCounter > 0; eltCounter --)
  { SyntacticElementHTML& currentElt = theElements[eltCounter];
    std::string currentEltString = currentElt.GetTagClass() + "[" + currentElt.content.substr(0, 10) + "...]";
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration())
    { out << "<tr><td>" << currentEltString << "</td>"
      << "<td>"
      << theInterpreter.theProgramExpression[commandCounter].ToString()
      << "</td></tr>";
      commandCounter --;
      continue;
    }
    for (; commandCounter > 1; commandCounter --)
    { std::string currentString= theInterpreter.theProgramExpression[commandCounter].ToString();
      out << "<tr><td>" << currentEltString << "</td><td>"
      << currentString << "</td></tr>";
      if (currentString == "SeparatorBetweenSpans")
        break;
    }
  }
  out << "</table>";
  return out.str();
}

bool CalculatorHTML::InterpretProcessExecutedCommands
(Calculator &theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ProcessInterprettedCommands");
  (void) comments;
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagMakingExpressionTableWithLatex = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theFormat.flagUseQuotes = false;
  theFormat.flagUseLatex = true;
  bool result = true;
  theInterpreter.theObjectContainer.resetPlots();
  for (int i = 0; i < theElements.size; i ++)
  { SyntacticElementHTML& currentElt = theElements[i];
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration())
    { currentElt.interpretedCommand = "";
      continue;
    }
    if (currentElt.commandIndex >= theInterpreter.theProgramExpression.size() ||
        currentElt.commandIndex < 0)
    { std::stringstream errorStream;
      errorStream << "<b>This is a programming error: syntactic element "
      << currentElt.ToStringDebug() << " has wrongly computed commandIndex: "
      << currentElt.commandIndex << ". "
      << "Please report this error to the website admins. </b>";
      currentElt.interpretedCommand = errorStream.str();
      result = false;
      continue;
    }
    if (!theInterpreter.theProgramExpression[currentElt.commandIndex].StartsWith(theInterpreter.opCommandEnclosure()) )
      crash << "Element: " << theInterpreter.theProgramExpression[currentElt.commandIndex].ToString()
      << " in " << theInterpreter.theProgramExpression.ToString()
      << " is supposed to be a command enclosure but apparently isn't. " << crash;
    Expression currentExpr = theInterpreter.theProgramExpression[currentElt.commandIndex][1];
    if (currentExpr.StartsWith(theInterpreter.opEndStatement()) && currentExpr.size() == 2)
      currentExpr = currentExpr[1];
    if (currentExpr.StartsWith(theInterpreter.opEndStatement()))
      currentElt.flagUseMathMode = false;
    theFormat.flagUseQuotes = false;
    theFormat.flagMakingExpressionTableWithLatex = true;
    currentElt.interpretedCommand = currentExpr.ToString(&theFormat);
    currentElt.flagUseDisplaystyleInMathMode = (currentElt.content.find("\\displaystyle") != std::string::npos);
    currentElt.flagUseMathMode = true;
    currentElt.flagUseMathSpan = false;
    if (currentExpr.IsOfType<std::string> () ||
        currentExpr.IsOfType<Plot>() ||
        currentElt.GetKeyValue("noTags") == "true")
    { currentElt.flagUseMathMode = false;
//      stOutput << "<hr>currentExpr: " << currentExpr.ToString() << "is plot or string<hr>";
    }// else
    //{ stOutput << "<hr>currentExpr: " << currentExpr.ToString() << "not of type plot or string";
    //  if (currentExpr.StartsWith(theInterpreter.opEndStatement()))
    //    stOutput << " but is a command list statement";
    //  stOutput << "<hr>";
    //}
  }
  return result;
}

void CalculatorHTML::LogProblemGenerationObsolete(Calculator &theInterpreter)
{ if (! theGlobalVariables.UserDebugFlagOn() || !theGlobalVariables.UserDefaultHasProblemComposingRights())
    return;
  std::stringstream streamLog;
  streamLog << "<table border ='1'>";
  for (int i = 0; i < theInterpreter.theProgramExpression.size(); i ++)
  { streamLog << "<tr>";
    for (int j = 0; j < this->theContent.size; j ++)
      if (this->theContent[j].commandIndex == i)
        streamLog << "<td>" << this->theContent[j].ToStringDebug() << "</td>";
    streamLog << "<td>" << theInterpreter.theProgramExpression[i].ToString()
    << "</td></tr>";
  }
  streamLog << "</table>";
  this->logCommandsProblemGeneratioN << streamLog.str();
}

void CalculatorHTML::FigureOutCurrentProblemList(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::FigureOutCurrentProblemList");
  if (this->flagParentInvestigated)
    return;
  this->flagParentInvestigated = true;
  this->topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
  this->LoadAndParseTopicList(comments);
}

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  double startTime = theGlobalVariables.GetElapsedSeconds();
//  stOutput << "<hr>DEBUG: Checking consistency 1 passed<hr>";
  if (!this->ParseHTML(comments))
  { this->outputHtmlBodyNoTag = "<b>Failed to interpret html input. </b><br>" + this->ToStringContent();
    this->timeToParseHtml = theGlobalVariables.GetElapsedSeconds() - startTime;
    return false;
  }
  //this->theProblemData.CheckConsistency();
  this->timeToParseHtml = theGlobalVariables.GetElapsedSeconds() - startTime;
  this->MaxInterpretationAttempts = 25;
  this->randomSeedsIfInterpretationFails.SetSize(this->MaxInterpretationAttempts);
  if (!this->theProblemData.flagRandomSeedGiven)
  { srand(1003 + time(NULL));
    this->randomSeedsIfInterpretationFails[0] = (103 + rand()) % 100000000;
  } else
    this->randomSeedsIfInterpretationFails[0] = this->theProblemData.randomSeed;
  srand(this->randomSeedsIfInterpretationFails[0]);
  for (int i = 1; i < this->randomSeedsIfInterpretationFails.size; i ++)
    this->randomSeedsIfInterpretationFails[i] = (103 + rand()) % 100000000;
  this->timePerAttempt.SetSize(0);
  this->timeIntermediatePerAttempt.SetSize(0);
  this->timeIntermediateComments.SetSize(0);
  this->NumAttemptsToInterpret = 0;
  while (this->NumAttemptsToInterpret < this->MaxInterpretationAttempts)
  { startTime = theGlobalVariables.GetElapsedSeconds();
    this->timeIntermediatePerAttempt.SetSize(this->timeIntermediatePerAttempt.size + 1);
    this->timeIntermediatePerAttempt.LastObject()->SetSize(0);
    this->timeIntermediateComments.SetSize(this->timeIntermediateComments.size + 1);
    this->timeIntermediateComments.LastObject()->SetSize(0);
    Calculator theInterpreter;
    this->NumAttemptsToInterpret ++;
//    stOutput << "DEBUG: Interpretation attempt #: " << this->NumAttemptsToInterpret;
//    stOutput << "DEBUG: flagPlotNoControls: " << theInterpreter.flagPlotNoControls;
    std::stringstream commentsOnLastFailure;
    if (this->InterpretHtmlOneAttempt(theInterpreter, commentsOnLastFailure))
    { this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
      this->theProblemData.CheckConsistency();
      return true;
    }
    this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
    if (this->NumAttemptsToInterpret >= this->MaxInterpretationAttempts)
      comments << commentsOnLastFailure.str();
  }
  comments << "<hr>Failed to evaluate the commands: " << this->NumAttemptsToInterpret
  << " attempts made. ";
  if (this->flagIsForReal)
  { this->StoreRandomSeedCurrent(comments);
    comments << "<b>Your random seed has been reset due to a finicky problem generation. </b>";
  }
  this->theProblemData.CheckConsistency();
  return false;
}

bool CalculatorHTML::IsSplittingChar(const std::string& input)
{ if (input.size() != 1)
    return false;
  return this->splittingChars.Contains(input[0]);
}

int SyntacticElementHTML::ParsingNumDummyElements = 8;
std::string CalculatorHTML::ToStringParsingStack(List<SyntacticElementHTML>& theStack)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringParsingStack");
  std::stringstream out;
  out << "#Non-dummy elts: " << theStack.size - SyntacticElementHTML::ParsingNumDummyElements << ". ";
  for (int i = SyntacticElementHTML::ParsingNumDummyElements; i < theStack.size; i ++)
  { out << "<span style =\"color:" << ((i % 2 == 0) ? "orange" : "blue") << "\">";
    std::string theContent = theStack[i].ToStringDebug();
    if (theContent.size() == 0)
      theContent = "<b>empty</b>";
    else if (theContent == " ")
      theContent = "_";
    out << theContent << "</span>";
  }
  return out.str();
}

int CalculatorHTML::GetAnswerIndex(const std::string& desiredAnswerId)
{ return this->theProblemData.theAnswers.GetIndex(desiredAnswerId);
}

bool CalculatorHTML::CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::CanBeMerged");
  if (left.syntacticRole != "" || right.syntacticRole != "")
    return false;
  if (this->IsSplittingChar(left.content) && left.content != " ")
    return false;
  if (this->IsSplittingChar(right.content) && right.content != " ")
    return false;
  return true;
}

bool CalculatorHTML::SetTagClassFromCloseTag(SyntacticElementHTML& output)
{ MacroRegisterFunctionWithName("CalculatorHTML::SetTagClassFromCloseTag");
  std::string& lastTag = output.tag;
  if (lastTag == "head" || lastTag == "HEAD" || lastTag == "Head")
  { output.SetKeyValue("class", "headFinish");
    this->flagTagHeadPresent = true;
    return true;
  } else if (lastTag == "body" || lastTag == "BODY" || lastTag == "Body")
  { output.SetKeyValue("class", "bodyFinish");
    this->flagTagBodyPresent = true;
    return true;
  } else if (lastTag == "html" || lastTag == "HTML" || lastTag == "html")
  { output.SetKeyValue("class", "htmlFinish");
    this->flagTagHtmlPresent = true;
    return true;
  }
  return false;
}

bool CalculatorHTML::SetTagClassFromOpenTag(SyntacticElementHTML& output)
{ MacroRegisterFunctionWithName("CalculatorHTML::SetTagClassFromOpenTag");
  std::string& lastTag = output.tag;
  if (lastTag == "head" || lastTag == "HEAD" || lastTag == "Head")
  { output.SetKeyValue("class", "headStart");
    this->flagTagHeadPresent = true;
    return true;
  } else if (lastTag == "body" || lastTag == "BODY" || lastTag == "Body")
  { output.SetKeyValue("class", "bodyStart");
    this->flagTagBodyPresent = true;
    return true;
  } else if (lastTag == "html" || lastTag == "HTML" || lastTag == "html")
  { output.SetKeyValue("class", "htmlStart");
    this->flagTagHtmlPresent = true;
    return true;
  } else if (this->calculatorTagsRecordedLiterally.Contains(lastTag))
  { output.SetKeyValue("class", lastTag);
    return true;
  }
  return false;
}

void CalculatorHTML::initTopicElementNames()
{ MacroRegisterFunctionWithName("CalculatorHTML::initTopicElementNames");
  if (this->calculatorTopicElementNames.size == 0)
  { this->calculatorTopicElementNames.AddOnTop("Chapter");
    this->calculatorTopicElementNames.AddOnTop("Section");
    this->calculatorTopicElementNames.AddOnTop("Topic");
    this->calculatorTopicElementNames.AddOnTop("Title");
    this->calculatorTopicElementNames.AddOnTop("Problem");
    this->calculatorTopicElementNames.AddOnTop("Video");
    this->calculatorTopicElementNames.AddOnTop("VideoHandwritten");
    this->calculatorTopicElementNames.AddOnTop("SlidesLatex");
    this->calculatorTopicElementNames.AddOnTop("SlidesSource");
    this->calculatorTopicElementNames.AddOnTop("HomeworkLatex");
    this->calculatorTopicElementNames.AddOnTop("HomeworkSource");
    this->calculatorTopicElementNames.AddOnTop("HomeworkSolutionSource");
    this->calculatorTopicElementNames.AddOnTop("SlidesSourceHeader");
    this->calculatorTopicElementNames.AddOnTop("HomeworkSourceHeader");
    this->calculatorTopicElementNames.AddOnTop("LoadTopicBundles");
    this->calculatorTopicElementNames.AddOnTop("TopicBundle");
    this->calculatorTopicElementNames.AddOnTop("BundleBegin");
    this->calculatorTopicElementNames.AddOnTop("BundleEnd");
  }
}

void CalculatorHTML::initAutocompleteExtras()
{ MacroRegisterFunctionWithName("CalculatorHTML::initAutocompleteExtras");
  if (this->autoCompleteExtras.size > 0)
    return;
  this->autoCompleteExtras.AddOnTop("answerCalculatorHighlight");
  this->autoCompleteExtras.AddOnTop("algebra");
  this->autoCompleteExtras.AddOnTop("logarithms");
  this->autoCompleteExtras.AddOnTop("buttons");
  this->autoCompleteExtras.AddOnTop("displaystyle");
}

void CalculatorHTML::initBuiltInSpanClasses()
{ MacroRegisterFunctionWithName("CalculatorHTML::initBuiltInSpanClasses");
  if (this->calculatorTagsRecordedLiterally.size == 0)
  { this->calculatorTagsRecordedLiterally.AddOnTopNoRepetition("answerCalculatorHighlight");
  }
  if (this->calculatorClassesAnswerFields.size == 0)
  { this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonSubmit");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonInterpret");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonGiveUp");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonSolution");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorMQField");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorMQButtonPanel");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorAnswerVerification");
  }
  if (this->calculatorClasses.size == 0)
  { this->calculatorClasses.AddOnTop("calculator");
    this->calculatorClasses.AddOnTop("calculatorSolution");
    this->calculatorClasses.AddOnTop("calculatorShowToUserOnly");
    this->calculatorClasses.AddOnTop("calculatorHidden");
    this->calculatorClasses.AddOnTop("calculatorCommentsBeforeInterpretation");
    this->calculatorClasses.AddOnTop("calculatorCommentsBeforeSubmission");
    this->calculatorClasses.AddOnTop("calculatorAnswer");
    this->calculatorClasses.AddOnTop("calculatorAnswerOnGiveUp");
    this->calculatorClasses.AddOnTop("calculatorExamIntermediate");
    this->calculatorClasses.AddOnTop("calculatorExamProblem");
    this->calculatorClasses.AddOnTop("calculatorNavigationHere");
    this->calculatorClasses.AddOnTop("calculatorProblemNavigationHere");
    this->calculatorClasses.AddOnTop("calculatorEditPageHere");
    this->calculatorClasses.AddOnTop("calculatorManageClass");
    this->calculatorClasses.AddOnTop("setCalculatorExamProblem");
    this->calculatorClasses.AddOnTop("setCalculatorExamHome");
    this->calculatorClasses.AddOnTop("generateTopicTable");
    this->calculatorClasses.AddOnTop("generateLectureMaterials");
    this->calculatorClasses.AddOnTop("generateTableOfContents");
    this->calculatorClasses.AddOnTop("accountInformationLinks");
    this->calculatorClasses.AddOnTop("calculatorJavascript");
    this->calculatorClasses.AddListOnTop(this->calculatorClassesAnswerFields);
  }
}

bool CalculatorHTML::ParseHTML(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ParseHTML");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  std::string word;
  char currentChar;
  List<SyntacticElementHTML> theElements;
  theElements.SetSize(0);
  theElements.SetExpectedSize(theReader.str().size() / 4);
  this->splittingChars.AddOnTop('<');
  this->splittingChars.AddOnTop('\"');
  this->splittingChars.AddOnTop('>');
  this->splittingChars.AddOnTop('=');
  this->splittingChars.AddOnTop('/');
  this->splittingChars.AddOnTop(' ');
  while (theReader.get(currentChar))
  { if (splittingChars.Contains(currentChar))
    { if (word != "")
        theElements.AddOnTop(word);
      std::string charToString;
      charToString.push_back(currentChar);
      theElements.AddOnTop(charToString);
      word = "";
    } else
      word.push_back(currentChar);
  }
  if (word != "")
    theElements.AddOnTop(word);
  this->initBuiltInSpanClasses();
  this->eltsStack.SetSize(0);
  SyntacticElementHTML dummyElt, tempElt;
  dummyElt.content = "<>";
  dummyElt.syntacticRole = "filler";
  tempElt.syntacticRole = "command";
  tempElt.tag = "";
  tempElt.content = "";
  eltsStack.SetExpectedSize(theElements.size + SyntacticElementHTML::ParsingNumDummyElements);
  for (int i = 0; i < SyntacticElementHTML::ParsingNumDummyElements; i ++)
    eltsStack.AddOnTop(dummyElt);
  int indexInElts = - 1;
  bool reduced = false;
  this->flagIsExamProblem = false;
  this->flagIsExamHome = false;
  this->flagTagHeadPresent = false;
  this->flagTagBodyPresent = false;
  this->flagTagHtmlPresent = false;
  std::string tagClass, tag;
  do
  { if (!reduced)
    { indexInElts ++;
      if (indexInElts < theElements.size)
        eltsStack.AddOnTop(theElements[indexInElts]);
    }
    //stOutput << "<br>DEBUG: " << this->ToStringParsingStack(eltsStack);
    reduced = true;
    SyntacticElementHTML& last         = eltsStack[eltsStack.size - 1];
    SyntacticElementHTML& secondToLast = eltsStack[eltsStack.size - 2];
    SyntacticElementHTML& thirdToLast  = eltsStack[eltsStack.size - 3];
    SyntacticElementHTML& fourthToLast = eltsStack[eltsStack.size - 4];
    SyntacticElementHTML& fifthToLast  = eltsStack[eltsStack.size - 5];
    SyntacticElementHTML& sixthToLast  = eltsStack[eltsStack.size - 6];
    //SyntacticElementHTML& seventhToLast = eltsStack[eltsStack.size-7];
    if ((secondToLast.syntacticRole == "<openTagCalc>" ||
         secondToLast.syntacticRole == "<calculatorSolution>") &&
        last.syntacticRole == "</closeTag>" &&
        secondToLast.tag == last.tag)
    { secondToLast.syntacticRole = "command";
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(secondToLast))
        eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTag" &&
        secondToLast == "/" && last.syntacticRole == ">")
    { tagClass = thirdToLast.GetKeyValue("class");
      if (tagClass == "calculatorSolution")
        thirdToLast.syntacticRole = "<calculatorSolution>";
      else if (this->calculatorClasses.Contains(tagClass))
        thirdToLast.syntacticRole = "command";
      else
      { thirdToLast.content = thirdToLast.ToStringOpenTag("",true);
        thirdToLast.resetAllExceptContent();
      }
      eltsStack.RemoveLastObject();
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(thirdToLast))
        eltsStack.RemoveLastObject();
      continue;
    }
    if (last.syntacticRole == "</closeTag>" && this->calculatorTagsRecordedLiterally.Contains(last.tag))
    { last.content = last.ToStringCloseTag("");
      last.syntacticRole = "command";
      last.tag += "Finish";
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (last.syntacticRole == "</closeTag>")
    { last.content = last.ToStringCloseTag("");
      last.resetAllExceptContent();
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTagCalc>" && secondToLast == "<" && last == "/")
    { secondToLast.syntacticRole = "</";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTagCalc>" && secondToLast.syntacticRole == "")
    { thirdToLast.content += secondToLast.content;
      secondToLast = last;
      eltsStack.SetSize(eltsStack.size - 1);
      continue;
    }
    if (secondToLast.syntacticRole != "<openTagCalc>" && last == "<")
    { last.content = "";
      last.syntacticRole = "<";
      continue;
    }
    if (secondToLast.syntacticRole != "<openTagCalc>" && last == ">")
    { last.content = "";
      last.syntacticRole = ">";
      continue;
    }
    if (secondToLast.syntacticRole == "<" && last != "/")
    { secondToLast.tag = last.content;
      secondToLast.syntacticRole = "<openTag";
      secondToLast.content = "";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "" && secondToLast != "\"" && last == "/")
    { secondToLast.content += last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "<" && last == "/")
    { secondToLast.syntacticRole = "</";
      secondToLast.tag = "";
      secondToLast.content = "";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "</" )
    { secondToLast.syntacticRole = "</closeTag";
      secondToLast.tag = last.content;
      secondToLast.content = "";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "</closeTag" && last.syntacticRole == ">")
    { secondToLast.syntacticRole = "</closeTag>";
      if (this->SetTagClassFromCloseTag(secondToLast))
        secondToLast.syntacticRole = "command";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (last == " " &&
        (secondToLast.syntacticRole == "<openTag" ||
         thirdToLast.syntacticRole == "<openTag" ||
         fourthToLast.syntacticRole == "<openTag"))
    { eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTag" && secondToLast.syntacticRole == "" && last.syntacticRole == "" &&
        secondToLast != "=" && secondToLast != "\"" && last != "=" && last != "\"")
    { thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size - 2] = *eltsStack.LastObject();
      eltsStack.RemoveLastObject();
      continue;
    }
    if (this->CanBeMerged(secondToLast, last))
    { secondToLast.content += last.content;
      eltsStack.RemoveLastObject();
      //stOutput << "<hr>Merge<hr>";
      continue;
    }
    if (thirdToLast.syntacticRole == "<calculatorSolution>" &&
        (secondToLast.syntacticRole == "" || secondToLast.syntacticRole == "command" ||
         secondToLast.syntacticRole == "<"))
    { thirdToLast.children.AddOnTop(secondToLast);
      eltsStack[eltsStack.size - 2] = last;
      eltsStack.RemoveLastObject();
      //stOutput << "<hr>DEBUG: Rule 2: processed " << thirdToLast.ToStringOpenTag(true) << "<hr>";
      continue;
    }
    if (sixthToLast.syntacticRole == "<openTag" &&
        fourthToLast == "=" && thirdToLast == "\"" &&
        last != "\"" )
    { if (last.syntacticRole != "" && last.content == "")
        secondToLast.content += last.syntacticRole;
      else
        secondToLast.content += last.content;
      eltsStack.RemoveLastObject();
      //stOutput << "<hr>Rule X executed<hr> ";
      continue;
    }
    if (thirdToLast == "\"" && secondToLast != "\"" && last != "\"")
    { if (secondToLast.syntacticRole != "" && secondToLast.content == "")
        secondToLast.content = secondToLast.syntacticRole;
      if (last.syntacticRole != "" && last.content == "")
        last.content = last.syntacticRole;
      secondToLast.content += last.content;
      //stOutput << "<hr>Rule quote executed<hr> ";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (sixthToLast.syntacticRole == "<openTag" && fourthToLast == "=" && thirdToLast == "\"" &&
        last == "\"" )
    { sixthToLast.SetKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.SetSize(eltsStack.size - 5);
      //stOutput << "<hr>Rule ZZ executed<hr> ";
      continue;
    }
    if (thirdToLast == "\"" && secondToLast != "\"" && last == "\"")
    { thirdToLast.content += secondToLast.content;
      thirdToLast.content += last.content;
      eltsStack.SetSize(eltsStack.size - 2);
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTag" &&
        secondToLast.syntacticRole == "" && last.syntacticRole == ">")
    { thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size - 2] = *eltsStack.LastObject();
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "<openTag" && last.syntacticRole == ">")
    { //stOutput << "<hr>DEBUG: " << secondToLast.ToStringDebug() << " class key value: " << secondToLast.GetKeyValue("class");
      tagClass = secondToLast.GetKeyValue("class");
      tag = secondToLast.tag;
      if (tagClass == "calculatorSolution" || tag == "calculatorSolution")
        secondToLast.syntacticRole = "<calculatorSolution>";
      else if (this->calculatorClasses.Contains(tagClass) || this->calculatorClasses.Contains(tag))
      { secondToLast.syntacticRole = "<openTagCalc>";
        if (this->calculatorClasses.Contains(tag))
        { tagClass = tag;
          secondToLast.SetKeyValue("class", tagClass);
        }
      } else if (this->calculatorTagsRecordedLiterally.Contains(secondToLast.tag))
      { secondToLast.syntacticRole = "command";
        secondToLast.tag += "Start";
      } else if (this->SetTagClassFromOpenTag(secondToLast))
        secondToLast.syntacticRole = "command";
      else
      { secondToLast.content =secondToLast.ToStringOpenTag("");
        if (theGlobalVariables.UserDefaultHasProblemComposingRights())
          if (MathRoutines::StringBeginsWith(tagClass, "calculator"))
            if (!this->calculatorClasses.Contains(tagClass))
              comments
              << "<hr><b><span style =\"color:red\">Warning: found class tag: "
              << tagClass
              << ". The name of this class starts with calculator yet this is not"
              << " a calculator class. This may be a typo. "
              << "</span></b>";
        secondToLast.resetAllExceptContent();
      }
      //stOutput << "<hr>Rule 3: processed: " << secondToLast.ToStringDebug() << "<hr>";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "" && last.syntacticRole == ">")
    { secondToLast.content += ">";
      eltsStack.RemoveLastObject();
      continue;
    }
    reduced = false;
  } while (reduced || indexInElts < theElements.size);
  this->theContent.SetSize(0);
  bool result = true;
  for (int i = SyntacticElementHTML::ParsingNumDummyElements; i < eltsStack.size; i ++)
  { bool needNewTag = false;
    if (i == SyntacticElementHTML::ParsingNumDummyElements)
      needNewTag = true;
    else if (this->theContent.LastObject()->syntacticRole != "")
      needNewTag = true;
    if (eltsStack[i].syntacticRole != "")
      needNewTag = true;
    if (eltsStack[i].GetTagClass() == "calculatorAnswer")
      if (eltsStack[i].GetKeyValue("mqMatrices") == "true")
        this->flagMathQuillWithMatrices = true;
    if (eltsStack[i].syntacticRole != "command" && eltsStack[i].syntacticRole != "")
    { result = false;
      comments << "<br>Syntactic element: " << eltsStack[i].ToStringDebug()
      << " is not a command but has non-empty syntactic role.";
    }
    if (!needNewTag)
      this->theContent.LastObject()->content += eltsStack[i].content;
    else
    { if (this->theContent.size > 0)
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
    result = this->ExtractAnswerIds(comments);
  //stOutput << "<hr>DEBUG: after extracting answer ids<hr>";
  for (int i = 0; i < this->theContent.size; i ++)
    this->theContent[i].indexInOwner = i;
  if (result)
    result = this->CheckContent(comments);
  this->theProblemData.CheckConsistency();
  return result;
}

bool CalculatorHTML::InterpretOneAnswerElement(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretOneAnswerElement");
  std::string answerId;
  if (!inputOutput.IsAnswerElement(&answerId))
    return true;
  int theIndex = this->GetAnswerIndex(answerId);
  std::string tagClass = inputOutput.GetTagClass();
  if (theIndex == - 1)
  { std::stringstream out;
    out << "<b>Element of class " << tagClass << " has name: "
    << answerId << " but that does not match any answerId value. "
    << this->theProblemData.ToStringAvailableAnswerIds() << ". </b>";
    inputOutput.interpretedCommand = out.str();
    return true;
  }
  Answer& currentA = this->theProblemData.theAnswers[theIndex];
  if (tagClass == "calculatorButtonInterpret")
    inputOutput.interpretedCommand = currentA.htmlButtonInterpret;
  if (tagClass == "calculatorButtonGiveUp")
    inputOutput.interpretedCommand = currentA.htmlButtonAnswer;
  if (tagClass == "calculatorButtonSolution")
    inputOutput.interpretedCommand = currentA.htmlButtonSolution;
  if (tagClass == "calculatorMQField")
    inputOutput.interpretedCommand = currentA.htmlSpanMQfield;
  if (tagClass == "calculatorMQButtonPanel")
    inputOutput.interpretedCommand = currentA.htmlSpanMQButtonPanel;
  if (tagClass == "calculatorAnswerVerification")
    inputOutput.interpretedCommand = currentA.htmlSpanVerifyAnswer;
  if (tagClass == "calculatorButtonSubmit")
    inputOutput.interpretedCommand = currentA.htmlButtonSubmit;
  if (tagClass == "calculatorSolution")
    inputOutput.interpretedCommand = currentA.htmlSpanSolution;
  return true;
}

bool CalculatorHTML::InterpretAnswerHighlights(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretAnswerHighlights");
  (void) comments;
  this->answerHighlights.SetSize(0);
  bool answerHighlightStarted = false;
  for (int i = 0; i < this->theContent.size; i ++)
  { if (this->theContent[i].tag == "answerCalculatorHighlightStart")
    { answerHighlightStarted = true;
      this->answerHighlights.AddOnTop("");
      this->theContent[i].content = "";
      continue;
    }
    if (!answerHighlightStarted)
      continue;
    if (this->theContent[i].IsAnswerElement(0))
      continue;
    if (this->theContent[i].tag == "answerCalculatorHighlightFinish")
    { answerHighlightStarted = false;
      this->theContent[i].content = "";
      continue;
    }
    *this->answerHighlights.LastObject() += this->theContent[i].ToStringInterpretedBody();
    this->theContent[i].content = "";
    this->theContent[i].interpretedCommand = "";
  }
  return true;
}

bool CalculatorHTML::InterpretAnswerElements(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretAnswerElements");
  (void) comments;
  for (int i = 0; i < this->theContent.size; i ++)
    this->InterpretOneAnswerElement(this->theContent[i]);
  return true;
}

bool CalculatorHTML::PrepareAnswerElements(std::stringstream &comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareAnswerElements");
  (void) comments;
  std::string currentId;
  for (int i = 0; i < this->theContent.size; i ++)
    if (this->theContent[i].IsAnswerElement(&currentId))
    { int index = this->GetAnswerIndex(currentId);
      if (index == - 1)
        continue;
      Answer& currentA = this->theProblemData.theAnswers[index];
      std::string tagClass = this->theContent[i].GetTagClass();
      if (tagClass == "calculatorButtonSubmit" ||
          tagClass == "calculatorButtonInterpret" ||
          tagClass == "calculatorButtonGiveUp")
        currentA.flagAutoGenerateSubmitButtons = false;
      if (tagClass == "calculatorButtonSolution")
        currentA.flagAutoGenerateButtonSolution = false;
      if (tagClass == "calculatorSolution")
        currentA.flagSolutionFound = true;
      if (tagClass == "calculatorMQField")
        currentA.flagAutoGenerateMQfield = false;
      if (tagClass == "calculatorMQButtonPanel")
        currentA.flagAutoGenerateMQButtonPanel = false;
      if (tagClass == "calculatorAnswerVerification")
        currentA.flagAutoGenerateVerificationField = false;
    }
  return true;
}

bool CalculatorHTML::ExtractAnswerIds(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ExtractAnswerIds");
  //we shouldn't clear this->theProblemData.theAnswers: it may contain
  //outdated information loaded from the database. We don't want to loose that info
  //(say we renamed an answerId but students have already stored answers using the old answerId...).
  List<std::string> answerIdsSeenSoFar;
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentE = this->theContent[i];
    if (currentE.IsAnswer())
    { std::string currentId = MathRoutines::StringTrimWhiteSpace(currentE.GetKeyValue("id"));
      if (currentId == "")
      { comments << "The answer element: " << currentE.ToStringDebug() << " has empty id. This is not allowed. ";
        return false;
      }
      int theIndex = this->GetAnswerIndex(currentId);
      if (theIndex == - 1)
        this->theProblemData.AddEmptyAnswerIdOnTop(currentId);
      if (answerIdsSeenSoFar.Contains(currentId))
      { comments << "<b>Answer with id: "
        << currentId << " contained more than once. </b>";
        return false;
      }
      answerIdsSeenSoFar.AddOnTopNoRepetition(currentId);
      this->theProblemData.theAnswers.GetValueCreate(currentId).MQpanelButtonOptions =
      currentE.GetKeyValue("buttons");
      continue;
    }
    if (!currentE.IsCommentBeforeSubmission() &&
        !currentE.IsCommentBeforeInterpretation() &&
        !currentE.IsAnswerOnGiveUp() &&
        !currentE.IsSolution())
      continue;
    if (answerIdsSeenSoFar.size == 0 && currentE.GetKeyValue("name") == "")
    { comments << "Auxilary answer element: " << currentE.ToStringDebug()
      << " has no name and appears before the first answer tag."
      << " Auxilary answers apply the answer tag whose id is specified in the name"
      << " tag of the auxilary answer. If the auxilary answer has no "
      << " name tag, it is assumed to apply to the (nearest) answer tag above it."
      << " To fix the issue either place the auxilary element after the answer or "
      << " specify the answer's id in the name tag of the auxilary element. ";
      return false;
    }
    if (currentE.GetKeyValue("name") == "")
      currentE.SetKeyValue("name", *answerIdsSeenSoFar.LastObject());
//    stOutput << "<hr>Debug: processed element " << currentE.ToStringDebug();
  }
//  stOutput << "<hr>DEBUG: Elements: " << this->ToStringContent() << "<hr>";
  this->theProblemData.CheckConsistency();
  return true;
}

bool CalculatorHTML::CheckConsistencyTopics()
{ for (int i = 0; i < this->theTopicS.size(); i ++)
    if (this->theTopicS[i].type == TopicElement::tProblem)
      if (this->theTopicS[i].immediateChildren.size > 0)
      { crash << "Topic element: " << this->theTopicS[i].ToString()
        << " has non-zero immediate children. " << crash;
        return false;
      }
  return true;
}

bool CalculatorHTML::CheckContent(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::CheckContent");
  bool result = true;
  for (int i = 0; i < this->theContent.size; i ++)
  { SyntacticElementHTML& currentElt = this->theContent[i];
    if (currentElt.syntacticRole == "command" && currentElt.IsAnswer() &&
        currentElt.GetKeyValue("id").find('=') != std::string::npos)
    { result = false;
      comments << "Error: the id of tag " << currentElt.ToStringDebug()
      << " contains the equality sign which is not allowed. ";
    }
  }
  return result;
}

std::string CalculatorHTML::CleanUpCommandString(const std::string& inputCommand)
{ MacroRegisterFunctionWithName("CalculatorHTML::CleanUpCommandString");
  if (inputCommand == "")
    return "";
  //stOutput << "<br>DEBUG: Cleaning up: " << inputCommand;
  int realStart = 0;
  int realEnd = inputCommand.size() - 1;
  for (; realStart < (signed) inputCommand.size(); realStart ++)
  { if (inputCommand[realStart] == ' ' || inputCommand[realStart] == '\n')
      continue;
    if (inputCommand[realStart] == '\\')
      if (realStart + 1 < (signed) inputCommand.size())
        if (inputCommand[realStart + 1] == '(')
        { realStart ++;
          continue;
        }
    break;
  }
  for (; realEnd >= 0; realEnd --)
  { if (inputCommand[realEnd] == ' ' || inputCommand[realEnd] == '\n')
      continue;
    if (inputCommand[realEnd] == ')')
      if (realEnd > 0)
        if (inputCommand[realEnd - 1] == '\\')
        { realEnd --;
          continue;
        }
    break;
  }
  if (realEnd < realStart)
    realEnd = realStart - 1;
  std::string result = inputCommand.substr(realStart, realEnd - realStart + 1);
  for (int i = (signed)result.size() - 1; i >= 0; i --)
  { if (result[i] == ' ' || result[i] == '\n')
      continue;
    if (result[i] == ';')
    { //stOutput << " DEBUG: to get: " << result;
      return result;
    }
    break;
  }
  if (result == "")
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

JSData CalculatorHTML::GetJavascriptMathQuillBoxesForJSON()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetJavascriptMathQuillBoxesForJSON");
  ////////////////////////////////////////////////////////////////////
  JSData output(JSData::JSarray);
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++)
  { JSData currentAnswerJS;
    Answer& currentAnswer = this->theProblemData.theAnswers[i];
    ///////////////
    currentAnswerJS["answerMQspanId"] = currentAnswer.idMQfield;
    currentAnswerJS["preferredButtonContainer"] = currentAnswer.idMQButtonPanelLocation;
    currentAnswerJS["answerIdPureLatex"] = currentAnswer.answerId;
    currentAnswerJS["idButtonSubmit"] = currentAnswer.idButtonSubmit;
    currentAnswerJS["idButtonInterpret"] = currentAnswer.idButtonInterpret;
    currentAnswerJS["idButtonAnswer"] = currentAnswer.idButtonAnswer;
    currentAnswerJS["idButtonSolution"] = currentAnswer.idButtonSolution;
    currentAnswerJS["idVerificationSpan"] = currentAnswer.idVerificationSpan;
    ///////////////
    output[i] = currentAnswerJS;
  }
  return output;
}

std::string CalculatorHTML::GetJavascriptMathQuillBoxes()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetJavascriptMathQuillBoxes");
  std::stringstream out;
  ////////////////////////////////////////////////////////////////////
  out << "<script type =\"text/javascript\">\n";
  out << "answerMQspanIds = [";
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++)
  { out << "\"" << this->theProblemData.theAnswers[i].idMQfield << "\"";
    if (i != this->theProblemData.theAnswers.size() - 1)
      out << ", ";
  }
  out << "];\n";
  out << "preferredButtonContainers = [";
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++)
  { out << "\"" << this->theProblemData.theAnswers[i].idMQButtonPanelLocation << "\"";
    if (i != this->theProblemData.theAnswers.size() - 1)
      out << ", ";
  }
  out << "];\n";
  out << "answerIdsPureLatex = [";
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++)
  { out << "\"" << HtmlRoutines::ConvertStringToURLString(this->theProblemData.theAnswers[i].answerId, false) << "\"";
    if (i != this->theProblemData.theAnswers.size() - 1)
      out << ", ";
  }
  out << "];\n";
  for (int answerCounter = 0; answerCounter < this->theProblemData.theAnswers.size(); answerCounter ++)
  { Answer& currentA = this->theProblemData.theAnswers[answerCounter];
    out << "var " << currentA.varMQfield << ";\n";
    out << "var " << currentA.varAnswerId << ";\n";
    out
    << "function " << currentA.MQUpdateFunction << "(){\n"
    << "ignoreNextMathQuillUpdateEvent = true;\n"
    << currentA.MQobject << ".latex(" << currentA.varAnswerId << ".value +' ');\n"
    //<< "alert('writing: ' +" << currentA.varAnswerId  << ".value);\n"
    //<< currentA.MQobject << ".latex(" << currentA.varAnswerId << ".value);\n"
    << "ignoreNextMathQuillUpdateEvent = false;\n"
    << "}\n";
  }
  out
    << "var ignoreNextMathQuillUpdateEvent = false;\n"
    << "function initializeMathQuill(){\n";

  for (int answerCounter = 0; answerCounter < this->theProblemData.theAnswers.size(); answerCounter ++)
  { Answer& currentA = this->theProblemData.theAnswers[answerCounter];
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
  //  << "  ignoreNextMathQuillUpdateEvent = false;\n"
    << "  return;\n"
    << "}\n"
    << currentA.varAnswerId << ".value = processMathQuillLatex(" << currentA.MQobject << ".latex() ); // simple API\n"
    << currentA.javascriptPreviewAnswer
    << "}\n"
    << "}\n"
    << "});\n";
  }
  out << "}//closing initializeMathQuill\n";
  out << "</script>";
  return out.str();
}

bool CalculatorHTML::StoreRandomSeedCurrent(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::StoreRandomSeedCurrent");
#ifdef MACRO_use_MongoDB
  this->theProblemData.flagRandomSeedGiven = true;
  this->currentUseR.SetProblemData(this->fileName, this->theProblemData);
  if (!this->currentUseR.StoreProblemDataToDatabaseJSON(commentsOnFailure))
  { commentsOnFailure << "<span style =\"color:red\"> <b>"
    << "Error: failed to store problem in database. "
    << "If you see this message, please take a screenshot and email your instructor. "
    << "</b></span>";
    return false;
  }
  return true;
#else
  commentsOnFailure << "Error: database not running. ";
  return false;
#endif
}

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  std::stringstream outBody;
  std::stringstream outHeadPt2;
  this->flagIsExamHome =
  theGlobalVariables.userCalculatorRequestType == "template" ||
  theGlobalVariables.userCalculatorRequestType == "templateNoLogin";
  this->theProblemData.randomSeed = this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret - 1];
  this->FigureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before after loading problem list");
  outHeadPt2 << HtmlRoutines::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << HtmlRoutines::GetJavascriptMathjax();
//  else
//    out << " no date picker";
  //stOutput << "DEBUG: theInterpreter.flagPlotNoControls: " << theInterpreter.flagPlotNoControls;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before execution");
  if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    return false;
  if (theInterpreter.theProgramExpression.HasInputBoxVariables())
    outHeadPt2 << HtmlRoutines::GetJavascriptCalculatorPageLink();
//////////////////////////////interpretation takes place before javascript generation as the latter depends on the former.
  if (this->flagIsExamProblem)
    outHeadPt2 << this->GetJavascriptSubmitAnswers();
  if (this->flagIsExamHome && theGlobalVariables.UserDefaultHasAdminRights() &&
      !theGlobalVariables.UserStudentVieWOn())
  { outHeadPt2 << HtmlRoutines::GetJavascriptHideHtmlWithTags();
    outHeadPt2 << HtmlRoutines::GetDatePickerJavascriptInit();
  }
  std::string problemLabel = "";
  if (!this->flagIsExamHome &&
      theGlobalVariables.userCalculatorRequestType != "template" &&
      theGlobalVariables.userCalculatorRequestType != "templateNoLogin")
    if (this->theTopicS.Contains(this->fileName))
    { TopicElement& current = this->theTopicS.GetValueCreate(this->fileName);
      current.ComputeLinks(*this, true);
      this->outputProblemLabel = current.problemNumberString;
      this->outputProblemTitle = current.title;
      problemLabel = current.displayTitle + "&nbsp;&nbsp;";
      if (this->flagDoPrependProblemNavigationBar)
        problemLabel += current.displayResourcesLinks;
    }
  if (this->flagIsExamProblem && this->flagIsForReal && !this->flagIsExamHome &&
      theGlobalVariables.userCalculatorRequestType != "template" &&
      theGlobalVariables.userCalculatorRequestType != "templateNoLogin")
  {
#ifdef MACRO_use_MongoDB
    bool problemAlreadySolved = false;
    if (this->currentUseR.theProblemData.Contains(this->fileName))
    { ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(this->fileName);
      if (theProbData.numCorrectlyAnswered >= theProbData.theAnswers.size())
        problemAlreadySolved = true;
    }
    this->outputDeadlineString = this->ToStringDeadline(this->fileName, problemAlreadySolved, true, true);
    if (!this->flagUseJSON)
    { if (this->outputDeadlineString == "")
        outBody << "<span style =\"color:orange\"><b>No deadline yet but scores are recorded. </b></span>";
      else
        outBody << "<span style =\"color:brown\"><b>Scores are recorded. </b></span>";
      outBody << problemLabel;
      outBody << this->outputDeadlineString << "\n<hr>\n";
    }
#endif
    //outBody << "<br>";
  } else if (!this->flagIsExamHome && !this->flagIsForReal &&
             theGlobalVariables.userCalculatorRequestType != "template" &&
             theGlobalVariables.userCalculatorRequestType != "templateNoLogin" &&
             theGlobalVariables.userCalculatorRequestType != "templateJSON" &&
             theGlobalVariables.userCalculatorRequestType != "templateJSONNoLogin")
    if (!this->flagUseJSON)
      outBody << "<span style =\"color:green\"><b>Scores not recorded. </b></span>"
      << problemLabel << "<hr>";
  //////////////////////////////
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time after execution");
  //first command and first syntactic element are the random seed and are ignored.
  theInterpreter.theObjectContainer.resetSliders();
  if (!this->InterpretProcessExecutedCommands(theInterpreter, this->theContent, comments))
  { outBody << comments.str();
    this->outputHtmlBodyNoTag = outBody.str();
    return false;
  }
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before class management routines");
  //out << "Debug data: homework groups found: " << this->hdHomeworkGroups.ToStringCommaDelimited();
  //stOutput << "<hr>DEBUG: preparing answer elements... <hr>";
  this->PrepareAnswerElements(comments);
  this->NumAnswerIdsMathquilled = 0;
  for (int i = 0; i < this->theContent.size; i ++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculatorNotAnswer(this->theContent[i]);
  this->InterpretAnswerHighlights(comments);
  for (int i = 0; i < this->theContent.size; i ++)
    this->InterpretIfAnswer(this->theContent[i]);
  outHeadPt2 << this->topicListJavascriptWithTag;
  this->InterpretAnswerElements(comments);
  //stOutput << "<hr>DEBUG: All answers interpreted. <hr>";
  this->theProblemData.CheckConsistency();
  this->theProblemData.CheckConsistencyMQids();
  bool headFinished = !this->flagTagHeadPresent;
  std::string tagClass;
  for (int i = 0; i < this->theContent.size; i ++)
    if (!this->theContent[i].IsHidden())
    { tagClass = this->theContent[i].GetTagClass();
      if (tagClass == "headFinish")
      { headFinished = true;
        outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        continue;
      }
      if (tagClass == "bodyStart" || tagClass == "bodyFinish" ||
          tagClass == "headStart" || tagClass == "headFinish" ||
          tagClass == "htmlStart" || tagClass == "htmlFinish")
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
  if (this->flagIsExamProblem)
  { outHeadPt2 << this->GetJavascriptMathQuillBoxes();
    if (theInterpreter.flagHasGraphics)
    { outHeadPt2 << HtmlRoutines::GetJavascriptCanvasGraphicsLink();
      MapReferenceS<std::string, std::string, MathRoutines::hashString>& theScripts =
      theInterpreter.theObjectContainer.graphicsScripts;
      for (int i = 0; i < theScripts.size(); i ++)
        this->theScripts.SetKeyValue(theScripts.theKeys[i], theScripts.theValues[i]);
    }
  }
  ////////////////////////////////////////////////////////////////////
  //out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
  //out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before database storage");
#ifdef MACRO_use_MongoDB
  bool shouldResetTheRandomSeed = false;
  if (this->flagIsForReal && !this->theProblemData.flagRandomSeedGiven)
    shouldResetTheRandomSeed = true;
  if (this->flagIsForReal && this->NumAttemptsToInterpret > 1)
  { shouldResetTheRandomSeed = true;
    outBody
    << "<hr><span style =\"color:red\"><b>"
    << "Your problem's random seed was just reset. </b></span> "
    << "You should be seeing this message very rarely, "
    << "<b>ONLY IF</b> your problem was changed by your instructor "
    << "<b>AFTER</b> you started solving it. "
    << "You should not be seeing this message a second time. "
    << "<span style =\"color:red\"><b>If you see this message every "
    << "time you reload the problem "
    << "this is a bug. Please take a screenshot and send it to your instructor. </b></span>";
  }
  if (shouldResetTheRandomSeed)
  { bool successStoringSeed = this->StoreRandomSeedCurrent(comments);
    if (!successStoringSeed)
      logWorker << logger::red << "This should not happen: failed to store random seed." << logger::endL
      << logger::yellow << comments.str() << logger::endL;
    //stOutput << "This is for real!<br>";
  }
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  { outBody << "<hr>Debug information follows. ";
    if (this->logCommandsProblemGeneratioN.str() != "")
      outBody << "<br>" << this->logCommandsProblemGeneratioN.str() << "<hr>";
    if (this->flagIsExamProblem)
      outBody << "Exam problem here. ";
    outBody << "<br>Random seed: "
    << this->theProblemData.randomSeed
    << "<br>ForReal: " << this->flagIsForReal << "<br>seed given: "
    << this->theProblemData.flagRandomSeedGiven
    << "<br>flagRandomSeedGiven: "
    << this->theProblemData.flagRandomSeedGiven
    << "\n<br>\n"
    << "<hr>"
    << "<hr>"
    << HtmlRoutines::ConvertStringToHtmlString
    (this->ToStringCalculatorArgumentsForProblem("exercise", "false"), true);
  }
  //out << "Current collection problems: " << this->databaseProblemList.ToStringCommaDelimited()
  //<< " with weights: " << this->databaseProblemWeights.ToStringCommaDelimited();
#endif // MACRO_use_MongoDB
  std::stringstream navigationAndEditTagStream;
  if (this->flagDoPrependProblemNavigationBar)
    navigationAndEditTagStream << this->ToStringProblemNavigation();
  if (this->flagDoPrependEditPagePanel)
    if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentVieWOn())
      navigationAndEditTagStream << this->GetEditPagePanel();
  this->outputProblemNavigatioN = navigationAndEditTagStream.str();
  this->outputHtmlBodyNoTag = outBody.str();
  this->outputHtmlHeadNoTag = outHeadPt2.str();
  return true;
}

std::string CalculatorHTML::ToStringProblemNavigation() const
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemNavigation");
  std::stringstream out;
  std::string exerciseRequest = "exercise";
  std::string studentView = theGlobalVariables.UserStudentVieWOn() ? "true" : "false";
  std::string linkSeparator = " | ";
  std::string linkBigSeparator = " || ";
  if (theGlobalVariables.UserGuestMode())
    exerciseRequest = "exerciseNoLogin";
  if (theGlobalVariables.UserGuestMode())
    out << "<b>Guest mode</b>" << linkSeparator;
  if (!theGlobalVariables.flagLoggedIn)
     out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
     << "?request=login\">Log in</a> " << linkSeparator;
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  std::string calcArgsNoPassExamDetails =
  theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (this->flagIsExamProblem)
  { if (theGlobalVariables.userCalculatorRequestType == "exercise")
    { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=scoredQuiz&"
      << this->ToStringCalculatorArgumentsForProblem("scoredQuiz", studentView)
      << "\">" << this->stringScoredQuizzes << "</a>" << linkSeparator;
      out << "<span style =\"color:green\"><b>" << this->stringPracticE << "</b>"
      << "</span>" << linkSeparator;
    } else if (theGlobalVariables.userCalculatorRequestType == "scoredQuiz")
    { out << "<span style =\"color:brown\"><b>"
      << this->stringScoredQuizzes << "</b></span>" << linkSeparator;
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=exercise&"
      << this->ToStringCalculatorArgumentsForProblem("exercise", studentView)
      << "\">" << this->stringPracticE << "</a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated)
  { int indexInParent = this->problemNamesNoTopics.GetIndex(this->fileName);
    if (indexInParent == - 1)
      out << "<b>Problem not in course</b>" << linkSeparator;
    else
    { if (indexInParent > 0)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection") != "")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << HtmlRoutines::ConvertStringToURLString(this->topicListFileName, false) << "&";
        out << "courseHome=" << HtmlRoutines::ConvertStringToURLString(this->courseHome, false) << "&";
        out << "fileName=" << HtmlRoutines::ConvertStringToURLString(this->problemNamesNoTopics[indexInParent - 1], false)
        << "\">&#8592;</a>" << linkSeparator;
      } else
        out << "<a disabled =\"disabled\">&#8592;</a>" << linkSeparator;
      if (indexInParent < this->problemNamesNoTopics.size - 1)
      { out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection") != "")
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        out << "topicList=" << HtmlRoutines::ConvertStringToURLString(this->topicListFileName, false) << "&";
        out << "courseHome=" << HtmlRoutines::ConvertStringToURLString(this->courseHome, false) << "&";
        out << "fileName=" << HtmlRoutines::ConvertStringToURLString(this->problemNamesNoTopics[indexInParent + 1], false)
        << "\">&#8594;</a>" << linkSeparator;
      } else
        out << "<a disabled =\"disabled\">&#8594;</a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem &&
      (theGlobalVariables.userCalculatorRequestType == "exercise" ||
       theGlobalVariables.userCalculatorRequestType == "exerciseNoLogin"))
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=" << theGlobalVariables.userCalculatorRequestType << "&"
    << this->ToStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">" << this->stringProblemLink << " (#"
    << this->theProblemData.randomSeed << ")</a>" << linkBigSeparator;
  if (theGlobalVariables.UserDefaultHasAdminRights())
  { if (theGlobalVariables.UserStudentVieWOn())
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
      << this->ToStringCalculatorArgumentsForProblem
      (theGlobalVariables.userCalculatorRequestType, "false",
       HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("studentSection"), false))
      << "\">Admin view</a>" << linkSeparator;
    else
      out << "<b>Admin view</b>" << linkSeparator;
    if (this->databaseStudentSections.size == 0)
    { if (theGlobalVariables.UserStudentVieWOn())
        out << "<b>Student View</b>";
      else
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
        << this->ToStringCalculatorArgumentsForProblem(theGlobalVariables.userCalculatorRequestType, "true", "")
        << "\">Student view</a>";
    } else
      out << "Student view, section: ";
    for (int i = 0; i < this->databaseStudentSections.size; i ++)
      if (this->databaseStudentSections[i] != "")
      { if (theGlobalVariables.UserStudentVieWOn() &&
            this->databaseStudentSections[i] == HtmlRoutines::ConvertURLStringToNormal
            (theGlobalVariables.GetWebInput("studentSection"), false))
          out << "<b>" << this->databaseStudentSections[i] << "</b>";
        else
          out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
          << this->ToStringCalculatorArgumentsForProblem
          (theGlobalVariables.userCalculatorRequestType, "true", this->databaseStudentSections[i])
          << "\">" << this->databaseStudentSections[i] << " </a>";
        if (i != this->databaseStudentSections.size - 1)
          out << linkSeparator;
        //stOutput << "DEBUG: student section: " << this->databaseStudentSections[i];
      }
  }
  if (out.str() == "")
    return "";
  std::stringstream outFinal;
  outFinal << "<problemNavigation>" << out.str();
  if (!this->flagDoPrependCalculatorNavigationBar)
    outFinal << " | <small>Generated in: "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds())
    << " second(s).</small>";
  outFinal << "</problemNavigation>";
  return outFinal.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem
(const std::string& requestType, const std::string& studentView,
 const std::string& studentSection, bool includeRandomSeedIfAppropriate) const
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsForProblem");
  if (!theGlobalVariables.flagLoggedIn && !theGlobalVariables.UserGuestMode())
    return "";
  std::stringstream out;
  out << "request=" << requestType << "&";
  List<std::string> excludedTags;
  excludedTags.AddOnTop("randomSeed");
  out << theGlobalVariables.ToStringCalcArgsNoNavigation(&excludedTags)
  << "courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "&";
  if (!theGlobalVariables.flagRunningApache && this->fileName != "")
    out << "fileName=" << HtmlRoutines::ConvertStringToURLString(this->fileName, false) << "&";
  else
    out << "fileName=" << HtmlRoutines::ConvertStringToURLString(theGlobalVariables.GetWebInput("fileName"), false)
    << "&";
  out << "topicList=" << theGlobalVariables.GetWebInput("topicList") << "&";
  out << "studentView=" << studentView << "&";
  if (studentSection != "")
    out << "studentSection=" << HtmlRoutines::ConvertStringToURLString(studentSection, false) << "&";
  if (includeRandomSeedIfAppropriate)
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
//  out << "fileName=" << HtmlRoutines::ConvertStringToURLString(this->fileName) << "&";
  return out.str();
}

std::string CalculatorHTML::GetEditPageButton(const std::string& desiredFileName, bool includeCloneButton)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetEditPageButton");
  std::stringstream out;
  out << "\n<a class =\"linkStandardButtonLike\" href=\""
  << theGlobalVariables.DisplayNameExecutable << "?request=editPage&";
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(desiredFileName, false);
  std::stringstream refStreamNoRequest;
  std::string spanCloningAttemptResultID = "spanCloningAttemptResultID" + desiredFileName;
  std::string clonePageAreaID = "clonePageAreaID" + desiredFileName;
  //  out << "cleaned up link: " << cleaneduplink;
  //  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation(0)
  << "fileName=" << urledProblem << "&"
  << "topicList=" << HtmlRoutines::ConvertStringToURLString(this->topicListFileName, false) << "&"
  << "courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "&";
  out << refStreamNoRequest.str() << "\">" << "Edit" << "</a>";
  out << "<textarea class =\"currentFileNameArea\" id =\"" << clonePageAreaID << "\" cols =\""
  << desiredFileName.size() + 7 << "\">" << desiredFileName << "</textarea>\n";
  if (includeCloneButton)
    out
    << "<button class =\"buttonClone\" onclick=\""
    << "submitStringAsMainInput(document.getElementById('"
    << clonePageAreaID << "').value, '" << spanCloningAttemptResultID << "', 'clonePage'"
    << ", '"
    << spanCloningAttemptResultID
    << "'"
    << ");"
    << "\" >Clone</button> <span id =\"" << spanCloningAttemptResultID << "\"></span>";
  return out.str();
}

std::string HtmlRoutines::GetJavascriptSubmitURLString()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptSubmitURLString");
  std::stringstream out;
  out
  << "var GlobalSubmitStringAsMainInputCounter = 0;\n"
  << "function submitStringCalculatorArgument(inputParams, idOutput, onLoadFunction, idStatus)\n"
  << "{ var spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput == null)\n"
  << "  { spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style ='color:red'> ERROR: span with id \" + idOutput + \"MISSING! </span>\";\n"
  << "  }\n";

  ////////////////////////////////////////////
  out
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameExecutable << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  if (idStatus ===undefined)\n"
  << "    idStatus = idOutput;\n"
  << "  var statusSpan = document.getElementById(idStatus);\n "
  << "  timeOutCounter = 0;"
  << "  GlobalSubmitStringAsMainInputCounter ++;\n"
  << "  var addressDetailsIndicatorID=\"addressDetailsID\"+GlobalSubmitStringAsMainInputCounter;\n"
  << "  var tranmissionIndicatorID=\"transmissionIndicatorID\"+GlobalSubmitStringAsMainInputCounter;\n"
  << "  var postRequest ="
  << "'<br>POST "
  << theGlobalVariables.DisplayNameExecutable
  << "'+\n '<br>'"
  << "+\n inputParams;\n"
  << "  var stringSwitchMenu='switchMenu(\\\"'+"
  << "  addressDetailsIndicatorID+'\\\");';\n"
  << "  statusSpan.innerHTML="
  << "\"<button style ='background:none; border:0; cursor:pointer' id ='\"+tranmissionIndicatorID + \"'' "
  << "onclick='\"+ stringSwitchMenu+\"'>Connection details</button>\""
  << "+ \"<span style ='display:none' id ='\"+ addressDetailsIndicatorID + \"'>\"+"
  << "postRequest + \"</span>\""
  << ";\n"
  << "  var buttonHandle = document.getElementById(tranmissionIndicatorID);"
  << "  var lastRequestCounter = GlobalSubmitStringAsMainInputCounter;\n"
  << "  https.onload = function()\n"
  << "  { if (lastRequestCounter!== GlobalSubmitStringAsMainInputCounter)\n"
  << "    { statusSpan.innerHTML+=\"<br><span style ='color:red'><b>Old request number \" + lastRequestCounter +\" just received, output suppressed.</b></span>\" \n"
  << "      return;\n"
  << "    }\n"
  << "    buttonHandle.innerHTML=\"<span style ='color:green'><b>Received</b></span>\";\n"
  << "    spanOutput.innerHTML=https.responseText;\n"
  << "    if (onLoadFunction !==undefined && onLoadFunction !== null && onLoadFunction !== 0)\n"
  << "      onLoadFunction(idOutput);\n"
  << "  }\n"
  << " buttonHandle.innerHTML=\"<span style ='color:orange'><b>Sent</b></span>\";\n"
  ////////////////////////////////////////////
  << "  https.send(inputParams);\n"
  /////////////////or/////////////////////////
  //  << "  https.send();\n"
  ////////////////////////////////////////////
  << "}\n";
  return out.str();
}

std::string HtmlRoutines::GetJavascriptSubmitMainInputIncludeCurrentFile()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptSubmitMainInputIncludeCurrentFile");
  std::stringstream out;
  out
  << "<script type =\"text/javascript\"> \n";
  out << "\"use srict\";\n";
  out << HtmlRoutines::GetJavascriptSubmitURLString()
  << "function submitStringAsMainInput(theString, idOutput, requestType, onLoadFunction, idStatus){\n"
  << "  var inputParams ='';\n"
  << "  inputParams+='request='+requestType +'&';\n"
  << "  inputParams+='" << theGlobalVariables.ToStringCalcArgsNoNavigation(0) << "';\n"
//  << "  inputParams+='&debugFlag= true';\n"
  << "  inputParams+='&fileName=" << theGlobalVariables.GetWebInput("fileName") << "';\n"
  << "  inputParams+='&topicList=" << theGlobalVariables.GetWebInput("topicList") << "';\n"
  << "  inputParams+='&courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "';\n"
  << "  inputParams+='&mainInput=' + encodeURIComponent(theString);\n"
  << "  submitStringCalculatorArgument(inputParams, idOutput, onLoadFunction, idStatus);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

std::string CalculatorHTML::ToStringProblemScoreFull(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemScoreFull");
  (void) theFileName;
  std::stringstream out;
  if (theGlobalVariables.UserGuestMode())
  { out << "scores require login";
    return out.str();
  }
  //stOutput << "<hr>CurrentUser.problemNames =" << this->currentUser.problemNames.ToStringCommaDelimited();
  #ifdef MACRO_use_MongoDB
  Rational currentWeight;
  if (this->currentUseR.theProblemData.Contains(theFileName))
  { ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    if (!theProbData.flagProblemWeightIsOK)
    { out << "<span style =\"color:orange\">No point weight assigned yet. </span>";
      if (!theProbData.adminData.GetWeightFromCoursE(this->currentUseR.courseComputed, currentWeight))
        currentWeight = 0;
      if (theProbData.theAnswers.size() == 1)
      { if (theProbData.numCorrectlyAnswered == 1)
          out << theProbData.totalNumSubmissions << " submission(s), problem correctly answered. ";
        else
          out << theProbData.totalNumSubmissions << " submission(s), problem not correctly answered yet. ";
      } else if (theProbData.theAnswers.size() > 1)
        out << theProbData.totalNumSubmissions << " submission(s), " << theProbData.numCorrectlyAnswered
        << " out of " << theProbData.theAnswers.size() << " subproblems correctly answered. ";
    } else if (theProbData.totalNumSubmissions != 0)
    { if (theProbData.numCorrectlyAnswered < theProbData.theAnswers.size())
      { out << "<span style =\"color:red\"><b> "
        << theProbData.Points << " out of "
        << currentWeight << " point(s). </b></span>";
      } else if (theProbData.numCorrectlyAnswered == theProbData.theAnswers.size())
      { out << "<span style =\"color:green\"><b> "
        << theProbData.Points << " out of "
        << currentWeight << " point(s). </b></span>";
      }
    }
  } else
  { out << "<span style =\"color:brown\"><b>No submissions.</b> </span>" ;
  }
  #endif // MACRO_use_MongoDB
  return out.str();

}

std::string CalculatorHTML::ToStringProblemScoreShort(const std::string& theFileName, bool& outputAlreadySolved)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemScoreShort");
  (void) theFileName;
  (void) outputAlreadySolved;
#ifdef MACRO_use_MongoDB
  std::stringstream out;
  if (theGlobalVariables.UserGuestMode())
  { out << "scores require login";
    return out.str();
  }
  std::stringstream problemWeight;
  ProblemData theProbData;
  bool showModifyButton = theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentVieWOn();
  outputAlreadySolved = false;
  Rational currentWeight;
  std::string currentWeightAsGivenByInstructor;
  if (this->currentUseR.theProblemData.Contains(theFileName))
  { theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    Rational percentSolved = 0, totalPoints = 0;
    percentSolved.AssignNumeratorAndDenominator(theProbData.numCorrectlyAnswered, theProbData.theAnswers.size());
    theProbData.flagProblemWeightIsOK =
    theProbData.adminData.GetWeightFromCoursE
    (this->currentUseR.courseComputed, currentWeight, &currentWeightAsGivenByInstructor);
    if (!theProbData.flagProblemWeightIsOK)
    { problemWeight << "?";
      if (currentWeightAsGivenByInstructor != "")
        problemWeight << "<span style =\"color:red\">"
        << currentWeightAsGivenByInstructor << "(Error)</span>";
    } else
    { problemWeight << currentWeight;
      totalPoints = percentSolved * currentWeight;
    }
    outputAlreadySolved = (percentSolved == 1);
    if (!outputAlreadySolved)
    { if (!theProbData.flagProblemWeightIsOK)
        out << "<span style =\"color:brown\"><b>" << percentSolved << " out of " << problemWeight.str() << "</b></span>";
      else
        out << "<span style =\"color:red\"><b>" << totalPoints << " out of " << problemWeight.str() << "</b></span>";
    } else
    { if (!theProbData.flagProblemWeightIsOK)
        out << "<span style =\"color:green\"><b>solved</b></span>";
      else
        out << "<span style =\"color:green\"><b>" << totalPoints << " out of " << problemWeight.str() << "</b></span>";
    }
  } else
    out << "<span style =\"color:brown\"><b>need to solve</b></span>";
  if (!showModifyButton)
    return out.str();
  std::stringstream finalOut;
  finalOut << "<button class =\"accordionLike\" onclick=\"toggleProblemWeights();\">"
  << out.str()
  << "</button>";
  finalOut << "<span class =\"panelProblemWeights\"><br>"
  << this->ToStringProblemWeightButton(theFileName)
  << "</span>";
  return finalOut.str();
#else
  return "Error: database not running. ";
#endif // MACRO_use_MongoDB
}

std::string CalculatorHTML::ToStringProblemWeightButton(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemWeighT");
  if (!theGlobalVariables.UserDefaultHasAdminRights() ||
      theGlobalVariables.UserStudentVieWOn())
    return "";
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(theFileName, false);
  std::stringstream out;
  //stOutput << "<hr>this->databaseProblemList is: " << this->databaseProblemList.ToStringCommaDelimited();
  //stOutput << "<br>this->databaseProblemWeights is: " << this->databaseProblemWeights.ToStringCommaDelimited();
  //stOutput << "<br> cleanedupLink: " << cleaneduplink;
  std::string idPoints = "points" + urledProblem;
//  std::string idNumTries = "numTries"+urledProblem;
  std::string idButtonModifyPoints = "modifyPoints" + urledProblem;
  std::string idPointsModOutput = "modifyPointsOutputSpan" + urledProblem;
  out << "Pts: <textarea class =\"textareaStudentPoints\" rows =\"1\" cols =\"2\" id =\"" << idPoints << "\">";
  bool weightIsOK = false;
  std::string problemWeightAsGivenByInstructor;
  #ifdef MACRO_use_MongoDB
  Rational unusedRat;
  weightIsOK = this->currentUseR.theProblemData.GetValueCreate(theFileName).
  adminData.GetWeightFromCoursE(this->currentUseR.courseComputed, unusedRat, &problemWeightAsGivenByInstructor);
  out << problemWeightAsGivenByInstructor;
  #endif
  out << "</textarea>";
  if (!weightIsOK && problemWeightAsGivenByInstructor != "")
    out << "<span style =\"color:red\"><b>Error</b></span>";
  out << "<button id =\"" << idButtonModifyPoints << "\" "
  << "onclick=\"" << "modifyWeight('" << urledProblem << "');\""
  << " disabled>";
  out << "Modify";
  out << "</button>";
  out << "<span id =\"" << idPointsModOutput << "\">" << "</span>";
  return out.str();
}

void TopicElement::ComputeID()
{ MacroRegisterFunctionWithName("TopicElement::ComputeID");
  if (this->problemFileName != "")
  { this->id = this->problemFileName;
    this->type = this->tProblem;
  } else
  { std::stringstream out;
    out << this->title;
    if (this->type == this->tTexHeader)
      out << "[sourceHeader]";
    if (this->type == this->tChapter)
      out << "[Chapter]";
    if (this->type == this->tSection)
      out << "[Section]";
    if (this->type == this->tSubSection)
      out << "[SubSection]";
    if (this->type == this->tProblem)
      out << "[TitledItem]";
    this->id = out.str();
  }
  this->idBase64 = Crypto::computeSha1outputBase64(this->id);
  this->studentScoresSpanId = "topic" + Crypto::computeSha1outputBase64(this->id);
}

void TopicElement::AddTopic(TopicElement& inputElt, MapLisT<std::string, TopicElement, MathRoutines::hashString>& output)
{ MacroRegisterFunctionWithName("TopicElement::AddTopic");
  int numToCheck = 4;
  if (output.size() >= numToCheck && inputElt.type != inputElt.tTexHeader && inputElt.type != inputElt.tChapter)
  { bool startsWithChapter = false;
    for (int i = 0; i < numToCheck - 1; i ++)
      if (output.theValues[i].type == inputElt.tChapter)
      { startsWithChapter = true;
        break;
      }
    if (!startsWithChapter)
    { TopicElement chapterlessChapter;
      chapterlessChapter.parentTopics.AddOnTop(output.size());
      chapterlessChapter.type = chapterlessChapter.tChapter;
      chapterlessChapter.title = "Topics without chapter";
      TopicElement::AddTopic(chapterlessChapter, output);
    }
  }
  inputElt.ComputeID();
  if (inputElt.id == "")
  { inputElt.type = inputElt.tError;
    inputElt.id = "error";
  }
  if (output.Contains(inputElt.id))
  { inputElt.id += "[Error]";
    inputElt.title = "[Error]: Entry " + inputElt.title + " already present. ";
  }
  inputElt.indexInParent = output.size();
  output.SetKeyValue(inputElt.id, inputElt);
  if (inputElt.parentTopics.size > 1)
  { int indexImmediateParent = - 1;
    int indexCurrentElement = output.GetIndex(inputElt.id);
    for (int i = 0; i < inputElt.parentTopics.size; i ++)
      if (inputElt.parentTopics[i] > indexImmediateParent && inputElt.parentTopics[i] < indexCurrentElement)
        indexImmediateParent = inputElt.parentTopics[i];
    if (indexImmediateParent > - 1)
      output.theValues[indexImmediateParent].immediateChildren.AddOnTop(indexCurrentElement);
  }
  if (inputElt.immediateChildren.size > 0)
    crash << "New topic element must have zero children. " << crash;
  //stOutput << "DEBUG: just added problem: " << inputElt.ToString() << "<br>";
  //  stOutput << "DEBUG: testing for multiple entries";
  //  bool found = false;
  //  for (int i = 0; i < output.size(); i ++)
  //  { if (inputElt.id == output.theKeys[i])
  //    { if (found)
  //        crash << "Multiple occurences of " << output.theK
  //
  //    }
  //
  //  }
}

void TopicElement::reset(int parentSize, MapLisT<std::string, TopicElement, MathRoutines::hashString>* containerElements)
{ this->type = this->tUndefined;
  this->indexInParent = - 1;
  this->flagSubproblemHasNoWeight = false;
  this->title = "empty";
  this->id = "";
  this->video = "";
  this->videoHandwritten = "";
  this->slidesProjector = "";
  this->slidesPrintable = "";
  this->sourceSlides.SetSize(0);
  this->sourceHomework.SetSize(0);
  this->sourceHomeworkIsSolution.SetSize(0);
  this->problemFileName = "";
  this->error = "";
  if (parentSize != - 1)
  { this->parentTopics.SetSize(MathRoutines::Minimum(parentSize, this->parentTopics.size));
    if (this->problemNumber.size < 4)
      this->problemNumber.initFillInObject(4, 0);
    for (int i = parentSize + 1; i < this->problemNumber.size; i ++)
      this->problemNumber[i] = 0;
    this->problemNumber[parentSize] ++;
  }
  this->immediateChildren.SetSize(0);
  this->totalSubSectionsUnderME = 0;
  this->totalSubSectionsUnderMeIncludingEmptySubsections = 0;
  this->flagContainsProblemsNotInSubsection = false;
  this->flagHasLectureTag = true;
  this->pointsEarnedInProblemsThatAreImmediateChildren = 0;
  this->totalPointsEarned = 0;
  this->maxPointsInAllChildren = 0;
  if (parentSize == 0)
    this->type = this->tChapter;
  if (parentSize == 1)
    this->type = this->tSection;
  if (parentSize == 2)
    this->type = this->tSubSection;
  if (parentSize == 3)
    this->type = this->tProblem;
  if (containerElements == 0)
    return;
  for (int i = 0; i < this->parentTopics.size; i ++)
    if (containerElements->theValues[this->parentTopics[i]].type >= this->type)
    { this->parentTopics.PopIndexShiftDown(i);
      i --;
    }
}

bool TopicElement::LoadTopicBundle
(const std::string& inputFileName, MapLisT<std::string, List<std::string>, MathRoutines::hashString>& output,
 CalculatorHTML& owner, std::stringstream& errorStream)
{ MacroRegisterFunctionWithName("TopicElement::LoadTopicBundle");
  std::string fileName = MathRoutines::StringTrimWhiteSpace(inputFileName);
  std::string newTopicBundles;
  if (!FileOperations::IsOKfileNameVirtual(fileName, false, &errorStream))
  { errorStream << "The file name " << fileName << " is not a valid topic bundle file name. ";
    return false;
  }
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(fileName, newTopicBundles, &errorStream))
  { errorStream << "Could not open topic bundle file. ";
    return false;
  }
  owner.loadedTopicBundles.AddOnTop(fileName);
  std::string currentLine, currentId;
  List<std::string> bundleNameStack;
  std::stringstream bundleReader(newTopicBundles);
  while (std::getline(bundleReader, currentLine, '\n'))
  { if (MathRoutines::StringBeginsWith(currentLine, "BundleBegin:", &currentId))
      bundleNameStack.AddOnTop(MathRoutines::StringTrimWhiteSpace(currentId));
    else if (MathRoutines::StringBeginsWith(currentLine, "BundleEnd:", &currentId))
    { if (bundleNameStack.size > 0)
        bundleNameStack.RemoveLastObject();
      else
      { errorStream << "<b style =\"color:red\">BundleEnd command without BungleBegin.</b>";
        return false;
      }
    } else
      for (int i = 0; i < bundleNameStack.size; i ++)
        output.GetValueCreate(bundleNameStack[i]).AddOnTop(currentLine);
  }
  return true;
}

void TopicElement::GetTopicList
(const std::string& inputString, MapLisT<std::string, TopicElement, MathRoutines::hashString>& output,
 CalculatorHTML& owner)
{ MacroRegisterFunctionWithName("TopicElement::GetTopicList");
  std::stringstream tableReader(inputString);
  std::string currentLine, currentArgument;
  TopicElement currentElt;
  bool found = false;
  currentElt.problemNumber.initFillInObject(4, 0);
  MemorySaving<MapLisT<std::string, List<std::string>, MathRoutines::hashString> > topicBundles;
  List<std::string> lineStack;
  owner.initTopicElementNames();
  int numLinesSoFar = 0;
  bool showedAllowedDataEntries = false;
  for (;;)
  { if (lineStack.size == 0)
      if (std::getline(tableReader, currentLine, '\n'))
        lineStack.AddOnTop(currentLine);
    if (lineStack.size <= 0)
      break;
    if (numLinesSoFar > 10000)
    { std::stringstream errorStream;
      errorStream << "More than 10000 topic lines have been read so far; this is not allowed. "
      << "Could this be an error due to infinite inclusion of topic bundles -"
      << "perhaps a topic bundle file is requesting to load itself? ";
      currentElt.error = errorStream.str();
      currentElt.type = currentElt.tError;
      found = true;
      break;
    }
    currentLine = lineStack.PopLastObject();
    if (MathRoutines::StringTrimWhiteSpace(currentLine) == "")
      continue;
    if (currentLine.size() > 0)
      if (currentLine[0] == '%')
        continue;
    if (MathRoutines::StringBeginsWith(currentLine, "LoadTopicBundles:", &currentArgument))
    { std::stringstream errorStream;
      if (!TopicElement::LoadTopicBundle(MathRoutines::StringTrimWhiteSpace(currentArgument), topicBundles.GetElement(), owner, errorStream))
      { currentElt.error = errorStream.str();
        currentElt.type = currentElt.tError;
        found = true;
      }
    } else if (MathRoutines::StringBeginsWith(currentLine, "TopicBundle:", &currentArgument))
    { currentArgument = MathRoutines::StringTrimWhiteSpace(currentArgument);
      std::stringstream errorStream;
      if (topicBundles.GetElement().Contains(currentArgument))
      { List<std::string>& currentBundle = topicBundles.GetElement().GetValueCreate(currentArgument);
        for (int j = currentBundle.size - 1; j >= 0; j --)
          lineStack.AddOnTop(currentBundle[j]);
      } else
      { if (found)
          TopicElement::AddTopic(currentElt, output);
        errorStream << "Topic bundle does not appear to contain the desired element: "
        << currentArgument;
        currentElt.error = errorStream.str();
        currentElt.type = currentElt.tError;
      }
    } else if (MathRoutines::StringBeginsWith(currentLine, "SlidesSourceHeader:", &currentArgument))
    { owner.slidesSourcesHeaders.AddOnTop(MathRoutines::StringTrimWhiteSpace(currentArgument));
      continue;
    } else if (MathRoutines::StringBeginsWith(currentLine, "HomeworkSourceHeader:", &currentArgument))
    { owner.sourcesHomeworkHeaders.AddOnTop(MathRoutines::StringTrimWhiteSpace(currentArgument));
      continue;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Chapter:", &currentArgument))
    { if (found)
        TopicElement::AddTopic(currentElt, output);
      found = true;
      currentElt.reset(0, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = MathRoutines::StringTrimWhiteSpace(currentArgument);
    } else if (MathRoutines::StringBeginsWith(currentLine, "Section:", &currentArgument))
    { if (found)
        TopicElement::AddTopic(currentElt, output);
      found = true;
      currentElt.reset(1, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.title;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Topic:", &currentArgument))
    { if (found)
        TopicElement::AddTopic(currentElt, output);
      found = true;
      currentElt.reset(2, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.title;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Title:", &currentArgument))
    { if (found)
        TopicElement::AddTopic(currentElt, output);
      found = true;
      currentElt.reset(3, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.title;
    } else if (MathRoutines::StringBeginsWith(currentLine, "Video:", &currentArgument))
      currentElt.video = MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "VideoHandwritten:", &currentArgument))
      currentElt.videoHandwritten = MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "SlidesProjector:", &currentArgument))
      currentElt.slidesProjector = MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "SlidesSource:", &currentArgument))
      currentElt.sourceSlides.AddOnTop(MathRoutines::StringTrimWhiteSpace(currentArgument));
    else if (MathRoutines::StringBeginsWith(currentLine, "HomeworkSource:", &currentArgument))
    { currentElt.sourceHomework.AddOnTop(MathRoutines::StringTrimWhiteSpace(currentArgument));
      currentElt.sourceHomeworkIsSolution.AddOnTop(false);
    } else if (MathRoutines::StringBeginsWith(currentLine, "HomeworkSolutionSource:", &currentArgument))
    { currentElt.sourceHomework.AddOnTop(MathRoutines::StringTrimWhiteSpace(currentArgument));
      currentElt.sourceHomeworkIsSolution.AddOnTop(true);
    } else if (MathRoutines::StringBeginsWith(currentLine, "SlidesLatex:", &currentArgument))
      currentElt.sourceSlides.AddOnTop("LaTeX: " + MathRoutines::StringTrimWhiteSpace(currentArgument));
    else if (MathRoutines::StringBeginsWith(currentLine, "HomeworkLatex:", &currentArgument))
    { currentElt.sourceHomework.AddOnTop("LaTeX: " + MathRoutines::StringTrimWhiteSpace(currentArgument));
      currentElt.sourceHomeworkIsSolution.AddOnTop(false);
    } else if (MathRoutines::StringBeginsWith(currentLine, "SlidesPrintable:", &currentArgument))
      currentElt.slidesPrintable = MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "HandwrittenSolutions:", &currentArgument))
      currentElt.handwrittenSolution = MathRoutines::StringTrimWhiteSpace(currentArgument);
    else if (MathRoutines::StringBeginsWith(currentLine, "Problem:", &currentArgument))
    { currentElt.problemFileName = MathRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.problemFileName;
      found = true;
    } else
    { std::stringstream errorStream;
      errorStream << "Failed to parse topic element: " << currentLine << ". ";
      if (!showedAllowedDataEntries)
      { errorStream << "<br>The allowed data labels are CASE SENSITIVE: ";
        for (int j = 0; j < owner.calculatorTopicElementNames.size; j ++)
          errorStream << "<br>" << owner.calculatorTopicElementNames[j];
        errorStream << "<br>You need to include the column character  <b>:</b> "
        << "immediately after the data labels. The data entries are terminated by new line. "
        << "Here is a correctly entered example:"
        << "<br>Title: Complex multiplication"
        << "<br>Problem: DefaultProblemLocation/Complex-multiplication-z-times-w.html"
        << "<br>SlidesSource: freecalc/modules/complex-numbers/complex-numbers-addition-multiplication-example- 1"
        << "<br>\n";
      }
      showedAllowedDataEntries = true;
      currentElt.error = errorStream.str();
      currentElt.type = currentElt.tError;
      found = true;
    }
    //stOutput << "<br>DEBUG: Current elt: " << currentElt.ToString();
    //int debugwarning;
    //owner.CheckConsistencyTopics();
  }
  owner.calculatorTopicBundles.AddOnTopNoRepetition(topicBundles.GetElement().theKeys);
  //stOutput << "DEBUG: topicBundles: " << topicBundles.GetElement().ToStringHtml();
  if (found)
    TopicElement::AddTopic(currentElt, output);
}

void CalculatorHTML::InterpretAccountInformationLinks(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretAccountInformationLinks");
  std::stringstream out;
  if (!theGlobalVariables.flagLoggedIn)
  { out << "<b>User not logged-in.</b>";
    inputOutput.interpretedCommand = out.str();
    return;
  }
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
  { out << "<b>Account management requires https.</b>";
    inputOutput.interpretedCommand = out.str();
    return;
  }
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage\">Change password</a>";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts\">Manage accounts</a>";
  inputOutput.interpretedCommand = out.str();
  return;
}

bool CalculatorHTML::LoadAndParseTopicList(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadAndParseTopicList");
  if (this->theTopicS.size() != 0)
    return true;
  if (this->topicListContent == "")
    if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly
         (this->topicListFileName, this->topicListContent, &comments))
    { comments << "Failed to load the topic list associated with this course. "
      << "Go to  ``Select course'' from the menu to see a list of available courses. ";
      return false;
    }
  if (this->topicListContent == "")
  { comments  << "Topic list empty. Topic list file name: " << this->topicListFileName << ". ";
    return false;
  }
  TopicElement::GetTopicList(this->topicListContent, this->theTopicS, *this);
  this->CheckConsistencyTopics();
  this->problemNamesNoTopics.Clear();
  for (int i = 0; i < this->theTopicS.size(); i ++)
    if (this->theTopicS[i].problemFileName != "")
      this->problemNamesNoTopics.AddOnTop(this->theTopicS[i].problemFileName);
  for (int i = this->theTopicS.size() - 1; i >= 0; i --)
  { TopicElement& currentElt = this->theTopicS.theValues[i];
    if (currentElt.problemFileName != "")
      continue;
    if (currentElt.type == currentElt.tSubSection)
    { currentElt.totalSubSectionsUnderME = 0;
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections = 0;
      currentElt.flagContainsProblemsNotInSubsection = false;
      continue;
    }
    currentElt.flagContainsProblemsNotInSubsection = false;
    currentElt.totalSubSectionsUnderME = 0;
    for (int j = 0; j < currentElt.immediateChildren.size; j ++)
    { TopicElement& currentChild = this->theTopicS.theValues[currentElt.immediateChildren[j]];
      if (currentChild.type == currentChild.tSubSection)
      { currentElt.totalSubSectionsUnderME ++;
        currentElt.totalSubSectionsUnderMeIncludingEmptySubsections ++;
      } else if (currentChild.problemFileName != "")
        currentElt.flagContainsProblemsNotInSubsection = true;
      else
      { currentElt.totalSubSectionsUnderME += currentChild.totalSubSectionsUnderME;
        currentElt.totalSubSectionsUnderMeIncludingEmptySubsections += currentChild.totalSubSectionsUnderMeIncludingEmptySubsections;
      }
    }
    if (currentElt.flagContainsProblemsNotInSubsection)
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections ++;
  }
  this->CheckConsistencyTopics();
  return true;
}

void CalculatorHTML::InterpretCalculatorNavigationBar(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretCalculatorNavigationBar");
  std::stringstream out;
  out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
    << "</calculatorNavigation>";
  inputOutput.interpretedCommand = out.str();
  this->flagDoPrependCalculatorNavigationBar = false;
}

void CalculatorHTML::InterpretProblemNavigationBar(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretCalculatorNavigationBar");
  inputOutput.interpretedCommand = this->ToStringProblemNavigation();
  this->flagDoPrependProblemNavigationBar = false;
}

std::string CalculatorHTML::GetEditPagePanel()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetEditPagePanel");
  std::stringstream out;
  out
  << "<editPagePanel>";
//  out << "DEbug: loaded topic bundles: " << this->loadedTopicBundles.ToStringCommaDelimited();
  if (theGlobalVariables.UserDebugFlagOn())
    out << this->ToStringLinkCurrentAdmin("Turn off debug", false, false);
  else
    out << this->ToStringLinkCurrentAdmin("Debug page", true, true);
  out << this->GetEditPageButton(this->fileName);
  if (this->flagIsExamHome)
  { out << this->GetEditPageButton(this->topicListFileName);
    for (int i = 0; i < this->loadedTopicBundles.size; i ++)
      out << this->GetEditPageButton(this->loadedTopicBundles[i]);
  }
  out << "</editPagePanel>";
  return out.str();
}

void CalculatorHTML::InterpretEditPagePanel(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretCalculatorNavigationBar");
  inputOutput.interpretedCommand = this->GetEditPagePanel();
  this->flagDoPrependEditPagePanel = false;
}

std::string CalculatorHTML::ToStringTopicListJSON()
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringTopicListJSON");
  std::stringstream out;
  if (!this->LoadAndParseTopicList(out))
    return "\"" + out.str() + "\"";
  JSData output, topicBundles;
//  output["DEBUGdatabaseTopicList"] =
  topicBundles.type = JSData::JSarray;
  for (int i = 0; i < this->loadedTopicBundles.size; i ++)
    topicBundles[i] = this->loadedTopicBundles[i];
  output["topicBundleFile"] = topicBundles;
  output["children"].type = JSData::JSarray;
  for (int i = 0; i < this->theTopicS.size(); i ++)
  { TopicElement& currentElt = this->theTopicS.theValues[i];
    if (currentElt.type == currentElt.tChapter)
      output["children"].list.AddOnTop(currentElt.ToJSON(*this));
  }
  return output.ToString(false);
}

void CalculatorHTML::InterpretTableOfContents(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretTableOfContents");
  std::stringstream out;
  if (!this->LoadAndParseTopicList(out))
  { inputOutput.interpretedCommand = out.str();
    return;
  }
  bool sectionStarted = false;
  bool subSectionStarted = false;
  bool chapterStarted = false;
  out << "\n\n\n<!--Topic list automatically generated from topic list: " << this->topicListFileName
  << ".-->";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
  << "?request=template&fileName=" << this->fileName << "&"
  << "topicList=" << this->topicListFileName << "&" << "\">All topics</a>";
  out << "<ul>";
  for (int i = 0; i < this->theTopicS.size(); i ++)
  { TopicElement& currentElt = this->theTopicS.theValues[i];
    if (subSectionStarted)
      if (currentElt.type == currentElt.tSubSection ||
          currentElt.type == currentElt.tSection ||
          currentElt.type == currentElt.tChapter)
        out << "</li>";
    if (sectionStarted)
      if (currentElt.type == currentElt.tSection ||
          currentElt.type == currentElt.tChapter)
        out << "</ul></li>";
    if (chapterStarted)
      if (currentElt.type == currentElt.tChapter)
        out << "</ul></li>";
    if (currentElt.type == currentElt.tChapter)
    { out << "<li>" << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=template&fileName=" << this->fileName << "&"
      << "topicList=" << this->topicListFileName << "&" << "chapter =" << currentElt.title
      << "\">" << currentElt.title << "</a>" << "<br>\n";
      chapterStarted = true;
      sectionStarted = false;
      subSectionStarted = false;
      out << "<ul>";
    } else if (currentElt.type == currentElt.tSection)
    { out << "<li>" << currentElt.title << "<br>\n";
      sectionStarted = true;
      subSectionStarted = false;
      out << "<ul>";
    } else if (currentElt.type == currentElt.tError)
      out << "Error parsing topic list. " << currentElt.error;
  }
  if (subSectionStarted)
      out << "</li>";
  if (sectionStarted)
      out << "</ul></li>";
  if (chapterStarted)
      out << "</ul></li>";
  out << "</ul>";
  inputOutput.interpretedCommand = out.str();
}

void CalculatorHTML::InterpretJavascripts(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretJavascripts");
  std::string javascriptName = MathRoutines::StringTrimWhiteSpace(inputOutput.content);
  if (javascriptName == "MathJax")
    inputOutput.interpretedCommand = HtmlRoutines::GetJavascriptMathjax();
}

std::string TopicElement::GetItemFinish(CalculatorHTML& owner)
{ std::stringstream out;
  if (this->type == this->tChapter)
  { if (owner.flagTopicTableStarted)
      out << "\n</tbody>\n</table>\n</div><!--bodyItem-->";
    if (owner.flagTopicSubSectionStarted)
      out << "\n</div><!--bodySubsection-->";
    if (owner.flagTopicSectionStarted)
      out << "\n</div><!--bodySection-->";
    if (owner.flagTopicChapterStarted)
      out << "\n</div><!--bodyChapter-->";
    owner.flagTopicSectionStarted = false;
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  }
  if (this->type == this->tSection)
  { if (owner.flagTopicTableStarted)
      out << "\n</tbody>\n</table>\n</div><!--bodyItem-->";
    if (owner.flagTopicSubSectionStarted)
      out << "\n</div><!--bodySubsection-->";
    if (owner.flagTopicSectionStarted)
      out << "\n</div><!--bodySection-->";
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  }
  if (this->type == this->tSubSection)
  { if (owner.flagTopicTableStarted)
      out << "\n</tbody>\n</table>\n</div><!--bodyItem-->";
    if (owner.flagTopicSubSectionStarted)
      out << "\n</div><!--bodySubsection-->";
    owner.flagTopicTableStarted = false;
  }
  return out.str();
}

std::string TopicElement::GetItemStart(CalculatorHTML& owner, bool doIncludeScoreButton, bool plainStyle)
{ std::stringstream out;
  out << this->GetItemFinish(owner);
  std::string theClass;
  if (this->type == this->tChapter)
  { theClass = "Chapter";
    owner.flagTopicChapterStarted = true;
    owner.flagTopicSectionStarted = false;
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  } else if (this->type == this->tSection)
  { theClass = "Section";
    if (!owner.flagTopicChapterStarted)
      out << "\n<div class =\"bodyChapter\">";
    owner.flagTopicChapterStarted = true;
    owner.flagTopicSectionStarted = true;
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  } else if (this->type == this->tSubSection)
  { theClass = "Subsection";
    if (!owner.flagTopicChapterStarted)
      out << "\n<div class =\"bodyChapter\">";
    if (!owner.flagTopicSectionStarted)
      out << "\n<div class =\"bodySection\">";
    owner.flagTopicChapterStarted = true;
    owner.flagTopicSectionStarted = true;
    owner.flagTopicSubSectionStarted = true;
    owner.flagTopicTableStarted = false;
  } else
  { if (!owner.flagTopicTableStarted)
    { if (!owner.flagTopicChapterStarted)
        out << "\n<div class =\"bodyChapter\">";
      if (!owner.flagTopicSectionStarted)
        out << "\n<div class =\"bodySection\">";
      if (!owner.flagTopicSubSectionStarted)
        out << "\n<div class =\"bodySubsection\">";
      owner.flagTopicChapterStarted = true;
      owner.flagTopicSectionStarted = true;
      owner.flagTopicSubSectionStarted = true;
      owner.flagTopicTableStarted = true;
      out
      << "\n<div class =\"bodyItem\">"
      << "\n<table class =\"topicList\">";
      out << "\n<colgroup><col><col><col><col><col></colgroup>";
      out << "\n<tbody>\n";
      if (!plainStyle)
        out
        << "\n<tr> <th>Sub-Topic</th>"
        << "<th>Resource Links</th>"
        << "<th>Current Score</th>"
        << "<th>Deadlines</th>"
        << "</tr>";
    }
    out << "<tr class =\"calculatorProblem\" "
    << "id =\"" << this->idBase64 << "\"" << ">\n";
    out << "  <td>\n";
    out << this->displayTitle;
    if (owner.flagIncludeStudentScores)
      out << this->ToStringStudentScoreReportPanel();
    out << "  </td>\n";
    out << "  <td>\n" << this->displayResourcesLinks << "</td>";
    out << "<td>";
    out << this->displayProblemLink;
    out << "  </td>\n";
    out << "  <td>";
    if (this->problemFileName == "")
      out << "-";
    else
      out << this->displayScore;
    out << "  </td>\n";
    out << "  <td class =\"deadlineCell\">\n" << this->displayDeadlinE << " </td>\n";
    out << "</tr>\n";
    return out.str();
  }
  out << "\n<div class =\"head" << theClass << "\" "
  << "id =\"" << this->idBase64 << "\"" << ">";
  out << "\n<table class =\"tableItem\">";
  out << "<colgroup><col><col><col><col><col></colgroup>\n";
  out << "<tr>"
  << "<td>" << this->displayTitle;
  out << "<button id =\"buttonToggle" << this->idBase64
  << "\" class =\"buttonToggleTopics\" onclick=\"toggleHeight(this, 'body" << this->idBase64 << "');\">&#9650;</button>";
  if (doIncludeScoreButton)
    out << this->ToStringStudentScoreReportPanel();
  out  << "</td>";
  out << "<td>" << this->displayResourcesLinks << "</td>";
  out << "<td></td>";
  if (doIncludeScoreButton)
    out << "<td>" << this->ToStringStudentScoreButton() << "</td>";
  else
    out << "<td></td>";
  out << "<td class =\"deadlineCell\">" << this->displayDeadlinE << "</td>";
  out << "</tr></table></div>\n";
  out << "<div class =\"body" << theClass << "\" id =\"body" << this->idBase64 << "\">";
  return out.str();
}

int TopicElement::scoreButtonCounter = 0;

std::string TopicElement::ToStringStudentScoreReportPanel()
{ std::stringstream out;
  TopicElement::scoreButtonCounter ++;
  out << "<span class =\"studentScoresWrapper\">";
  out << "<span id ='studentScoresLoadReport"
  << scoreButtonCounter << "'></span>"
  << "<span class ='studentScoreOutputJavascriptSpan' id ='studentScoresOutput"
  << scoreButtonCounter << "'></span>"
  << "<span class ='studentScoresContent' id ='" << this->studentScoresSpanId << "'></span>"
  << "</span>";
  return out.str();
}

std::string TopicElement::ToStringStudentScoreButton()
{ std::stringstream out;
  out << "<button class =\"studentScoresButton\" "
  << "onclick=\"toggleStudentScores"
  << "('studentScoresLoadReport" << TopicElement::scoreButtonCounter << "', "
  << "'scoresInCoursePage',"
  << "'studentScoresLoadReport"
  << TopicElement::scoreButtonCounter << "');\">"
  << "Scores</button>";
  return out.str();
}

std::string CalculatorHTML::GetSectionSelector()
{ if (!theGlobalVariables.UserDefaultHasProblemComposingRights() ||
      theGlobalVariables.UserStudentVieWOn())
    return "";
#ifdef MACRO_use_MongoDB
  std::stringstream out;
  out << "<sectionSelection>Sections: ";
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { out << "<input type =\"radio\" name =\"sectionSelector\" "
    << "onclick=\"populateTopicList("
    << "'"
    << this->databaseStudentSections[i]
    << "'"
    << ");\"";
    if (this->databaseStudentSections[i] == this->currentUseR.sectionComputed)
      out << "checked";
    out <<  ">"
    << this->databaseStudentSections[i]
    << "</input>";
  }
  out << "</sectionSelection>\n";
  return out.str();
#else
  return "";
#endif

}

void CalculatorHTML::InterpretLectureMaterials(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretLectureMaterials");
  std::stringstream out;
  if (this->flagUseJSON)
  { inputOutput.interpretedCommand = "<lectureList></lectureList>";
    return;
  }
  if (!this->LoadAndParseTopicList(out))
  { inputOutput.interpretedCommand = out.str();
    return;
  }
  bool plainStyle = (inputOutput.GetKeyValue("topicListStyle") == "plain");
  out << "<div class =\"headChapter\">Lecture materials "
  << "<button id =\"buttonToggleCourseInfo\" class =\"buttonToggleTopics\" "
  << "onclick=\"toggleHeight(this,'tableWithLectureMaterialsFull')\">&#9650;</button>"
  << "<span style =\"font-weight:normal; font-size:small\">(&larr; click to (un)hide)</span><br>\n"
  << "</div>";
  out
  << "\n<div class =\"bodyChapter\" id =\"tableWithLectureMaterialsFull\">"
  << "\n<table class =\"lectureMaterials\">";
  out << "\n<colgroup><col><col><col></colgroup>";
  out << "\n<tbody>\n";
  if (!plainStyle)
    out
    << "\n<tr> <th>Sub-Topic</th>"
    << "<th>Resource Links</th>"
    << "</tr>";
  this->topicLectureCounter = 0;
  for (int i = 0; i < this->theTopicS.size(); i ++)
  { TopicElement currentTopic = this->theTopicS[i];
    currentTopic.ComputeLinks(*this, plainStyle);
    if (!currentTopic.flagHasLectureTag)
      continue;
    out << "<tr>"
    << "<td>" << currentTopic.displayTitle << "</td>"
    << "<td>" << currentTopic.displayResourcesLinks << "</td>"
    << "<td>" << "<a href=\"#" << currentTopic.idBase64 << "\">" << "Go to section</a>" << "</td>"
    << "</tr>";
  }
  out << "</table></div>";
  this->topicLectureCounter = 0;
  inputOutput.interpretedCommand = out.str();
}

bool CalculatorHTML::ComputeTopicListAndPointsEarned(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::ComputeTopicListAndPointsEarned");
  if (!this->LoadAndParseTopicList(commentsOnFailure))
    return false;
  if (!this->LoadDatabaseInfo(commentsOnFailure))
  { if (!this->flagUseJSON)
      commentsOnFailure << "<span style =\"color:red\">Could not load your problem history.</span> <br>";
    else
      commentsOnFailure << "Error loading problem history. ";
  }
  if (!this->PrepareSectionList(commentsOnFailure))
  { if (!this->flagUseJSON)
      commentsOnFailure << "<span style =\"color:red\">Error preparing section list.</span> <br>";
    else
      commentsOnFailure << "Error preparing section list. ";
  }
  #ifdef MACRO_use_MongoDB
  this->flagIncludeStudentScores =
  theGlobalVariables.UserDefaultHasAdminRights() &&
  !theGlobalVariables.UserStudentVieWOn() &&
  theGlobalVariables.userCalculatorRequestType != "templateNoLogin";
  HashedList<std::string, MathRoutines::hashString> gradableProblems;
  for (int i = 0; i < this->theTopicS.size(); i ++)
    if (this->theTopicS[i].type == TopicElement::tProblem)
    { gradableProblems.AddOnTopNoRepetition(this->theTopicS[i].id);
      if (this->theTopicS[i].immediateChildren.size > 0)
        crash << "Error: problem " << this->theTopicS[i].ToString() << " has children topics which is not allowed. "
        << crash;
    }
  this->currentUseR.ComputePointsEarned(gradableProblems, &this->theTopicS, commentsOnFailure);
  #endif
  this->initTopicElementNames();
  return true;
}

void CalculatorHTML::InterpretTopicList(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretTopicList");
  if (this->flagUseJSON)
  { inputOutput.interpretedCommand = "<topicList></topicList>";
    return;
  }
  std::stringstream out;
  if (!this->ComputeTopicListAndPointsEarned(out))
  { inputOutput.interpretedCommand = out.str();
    return;
  }
  std::stringstream outFinal, outHead;
#ifdef MACRO_use_MongoDB
  if (this->currentUseR.pointsMax != 0)
  { double percent = 100 * this->currentUseR.pointsEarned.GetDoubleValue() /
    this->currentUseR.pointsMax.GetDoubleValue();
    outHead.precision(2);
    outHead << "<panelStudentScores>Total score: "
    << std::fixed << percent << "% = ";
    outHead << std::fixed << this->currentUseR.pointsEarned.GetDoubleValue()
    << " out of " << this->currentUseR.pointsMax.GetDoubleValue()
    << " points earned.</panelStudentScores>"
    << "<button id =\"buttonToggleCourseInfo\" class =\"buttonToggleTopics\" onclick=\"toggleHeight(this,'bodyCourseInformation')\">&#9650;</button><br>\n";
    outHead << "<div class =\"bodySection\" id = \"bodyCourseInformation\">"
    << "<small>Includes problems without deadline, but not problems without weights.<br> "
    << "If a problem is assigned a new weight, your % score may drop. </small><br>";
    outHead << "<panelCourseInfo>" << this->currentUseR.ToStringCourseInfo() << "</panelCourseInfo></div><br>";
  }
#endif
  out << "<panelProblemLinkStyleSelection>Problem links open in: ";
//  out << "<br>DEBUG: problinkstyle: "
//  << theGlobalVariables.GetWebInput("problemLinkStyle")
//  << "<br>";
  out << "<input type =\"radio\" name =\"problemLinkStyleSelector\" onclick=\"setProblemLinkStyle('accordion');\" ";
  if (theGlobalVariables.GetWebInput("problemLinkStyle") == "accordion")
    out << "checked";
  out << ">same tab, under topics</input>";
  out << "<input type =\"radio\" name =\"problemLinkStyleSelector\" onclick=\"setProblemLinkStyle('sameWindow');\" ";
  if (theGlobalVariables.GetWebInput("problemLinkStyle") == "sameWindow")
    out << "checked";
  out << ">same tab, replace topics</input>";
  out << "<input type =\"radio\" name =\"problemLinkStyleSelector\" onclick=\"setProblemLinkStyle('newWindow');\" ";
  if (theGlobalVariables.GetWebInput("problemLinkStyle") == "newWindow")
    out << "checked";
  out << ">new tab</input>"
  << ". </panelProblemLinkStyleSelection>";
  bool plainStyle = (inputOutput.GetKeyValue("topicListStyle") == "plain");
  std::string desiredChapter = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("chapter"), false);
  std::string currentChapter = "";
  out << "\n\n\n<!--Topic list automatically generated from topic list: "
  << this->topicListFileName
  << ".-->";
  this->flagTopicChapterStarted = false;
  this->flagTopicSectionStarted = false;
  this->flagTopicSubSectionStarted = false;
  this->flagTopicTableStarted = false;
  for (int i = 0; i < this->theTopicS.size(); i ++)
  { TopicElement& currentElt = this->theTopicS[i];
    if (currentElt.type == currentElt.tTexHeader)
      continue;
    if (currentElt.type == currentElt.tChapter)
      currentChapter = currentElt.title;
    if (desiredChapter != "")
      if (currentChapter != desiredChapter)
        continue;
    currentElt.ComputeLinks(*this, plainStyle);
    out << currentElt.GetItemStart(*this, this->flagIncludeStudentScores, plainStyle);
  }
  TopicElement finishChapter;
  finishChapter.type = finishChapter.tChapter;
  out << finishChapter.GetItemFinish(*this);
  this->NumVideosFound = this->NumVideosHandwrittenFound + this->NumVideosWithSlidesFound;
  outHead << "<panelStudentScores>Calculator build " << theGlobalVariables.buildVersionSimple << ". The course contains "
  << this->NumProblemsFound << " problem templates, "
  << this->NumSlidesFound << " slides (printable + projector mode counted as a single slide) and "
  << this->NumVideosFound << " = " << this->NumVideosWithSlidesFound << " with-slide + "
  << this->NumVideosHandwrittenFound << " handwritten videos.</panelStudentScores><br>";
  //outHead << "DEBUG: " << theGlobalVariables.buildHeadHash;
  outFinal << outHead.str() << out.str();
  inputOutput.interpretedCommand = outFinal.str();
  std::stringstream topicListJS;
  topicListJS << "<script type =\"text/javascript\">";

  topicListJS << "var currentStudentSection =";
#ifdef MACRO_use_MongoDB
  topicListJS << "'" << this->currentUseR.sectionComputed << "'" << ";\n";
#else
  topicListJS << "''" << ";\n";
#endif
  topicListJS << "var studentSections =[";
  for (int i = 0; i < this->databaseStudentSections.size; i ++)
  { topicListJS
    << "'"
    << HtmlRoutines::ConvertStringToURLString(this->databaseStudentSections[i], false)
    << "'";
    if (i != this->databaseStudentSections.size - 1)
      topicListJS << ", ";
  }
  topicListJS << "];\n";
  topicListJS << "var listTopics =[";
  for (int i = 0; i < this->theTopicS.size(); i ++)
  { TopicElement& currentE = this->theTopicS[i];
    if (currentE.type == currentE.tTexHeader)
      continue;
    topicListJS << "{id: '" << currentE.idBase64 << "', ";
    topicListJS << "type: '";
    if (currentE.type == currentE.tChapter)
      topicListJS << "chapter";
    else if (currentE.type == currentE.tSection)
      topicListJS << "section";
    else if (currentE.type == currentE.tSubSection)
      topicListJS << "subSection";
    else if (currentE.type == currentE.tError)
      topicListJS << "error";
    else
      topicListJS << "problem";
    topicListJS << "', ";
    //////////////////////////////////////////////////
    topicListJS << "idButton: '" << currentE.idDeadlineButton << "', ";
    //////////////////////////////////////////////////
    topicListJS << "deadlines: {";
    bool found = false;
    for (int j = 0; j < currentE.deadlinesPerSectioN.size; j ++)
    { if (currentE.deadlinesPerSectioN[j] == "")
        continue;
      if (found)
        topicListJS << ", ";
      found = true;
      topicListJS << "'" << this->databaseStudentSections[j] << "': "
      << "'" << currentE.deadlinesPerSectioN[j] << "'";
    }
    topicListJS << "}, ";
    //////////////////////////////////////////////////
    topicListJS << "deadlinesFormatted: {";
    found = false;
    for (int j = 0; j < currentE.deadlinesPerSectionFormatted.size; j ++)
    { if (currentE.deadlinesPerSectionFormatted[j] == "")
        continue;
      if (found)
        topicListJS << ", ";
      found = true;
      topicListJS << "'" << this->databaseStudentSections[j] << "': "
      << "'" << currentE.deadlinesPerSectionFormatted[j] << "'";
    }
    topicListJS << "}, ";
    ////////////////////////////
    topicListJS << "isInherited: {";
    for (int j = 0; j < currentE.deadlinesAreInherited.size; j ++)
    { topicListJS << "'" << this->databaseStudentSections[j] << "': ";
      if (currentE.deadlinesAreInherited[j])
        topicListJS << "true";
      else
        topicListJS << "false";
      if (j != currentE.deadlinesAreInherited.size - 1)
        topicListJS << ", ";
    }
    topicListJS << "}, ";
    //////////////////////////////////////////////////
    topicListJS << "immediateChildren: [";
    for (int j = 0; j < currentE.immediateChildren.size; j ++)
    { topicListJS << currentE.immediateChildren[j];
      if (j != currentE.immediateChildren.size - 1)
        topicListJS << ", ";
    }
    topicListJS << "]";
    //////////////////////////////////////////////////
    topicListJS << "}";
    if (i != this->theTopicS.size() - 1)
      topicListJS << ", ";
  }
  topicListJS << "];\n";
  topicListJS << "</script>";
  this->topicListJavascriptWithTag = topicListJS.str();
}

void TopicElement::ComputeLinks(CalculatorHTML& owner, bool plainStyle)
{ MacroRegisterFunctionWithName("TopicElement::ComputeLinks");
  (void) plainStyle;
  if (this->displayProblemLink != "")
    return;
  int depth = 3;
  if (this->type == this->tChapter)
    depth = 0;
  if (this->type == this->tSection)
    depth = 1;
  if (this->type == this->tSubSection)
    depth = 2;
  std::stringstream problemLabel;
  for (int i = 0; i < depth + 1; i ++)
  { problemLabel << this->problemNumber[i];
    problemLabel << ".";
  }
  problemLabel << " ";
  this->problemNumberString = problemLabel.str();
  std::string titleWithLectureNumber = this->title;
  this->flagHasLectureTag = false;
  int lectureTagStart = titleWithLectureNumber.find("<lectureTag>");
  if (lectureTagStart >= 0)
  { int lectureTagFinish = titleWithLectureNumber.find("</lectureTag>");
    if (lectureTagFinish >= 0)
    { owner.topicLectureCounter ++;
      lectureTagFinish += 13;
      std::stringstream newTitle;
      newTitle
      << titleWithLectureNumber.substr(0, lectureTagStart)
      << "<lectureTag>\\(\\mathcal L\\) "
      << "</lectureTag>"
      << "Lecture "
      << owner.topicLectureCounter
      << "."
      << titleWithLectureNumber.substr(lectureTagFinish);
      this->flagHasLectureTag = true;
      titleWithLectureNumber = newTitle.str();
    }
  }
  if (this->title == "")
    this->displayTitle = this->problemNumberString + "-" ;//+ "<br>DEBUG: "+this->ToString();
  else
    this->displayTitle = this->problemNumberString + titleWithLectureNumber;//+ "<br>DEBUG: "+this->ToString();
  if (this->video == "" || this->video == "-" || this->video == "--")
    this->displayVideoLink = "";
  else
    this->displayVideoLink = "<a href=\"" + this->video + "\" class =\"videoLink\" class =\"videoLink\" target =\"_blank\">Video</a>";
  if (this->videoHandwritten == "" || this->videoHandwritten == "-" || this->videoHandwritten == "--")
    this->displayVideoHandwrittenLink = "";
  else
    this->displayVideoHandwrittenLink = "<a href=\"" +
    this->videoHandwritten + "\" class =\"videoLink\" class =\"videoLink\" target =\"_blank\">Video <b>(H)</b></a>";
  if (this->slidesProjector != "")
    this->displaySlidesLink = "<a href=\"" + this->slidesProjector + "\" class =\"slidesLink\">Slides</a>";
  if (this->slidesPrintable != "")
    this->displaySlidesPrintableLink = "<a href=\"" + this->slidesPrintable + "\" class =\"slidesLink\">Printable slides</a>";
  if (this->handwrittenSolution != "")
    this->displayHandwrittenSolution = "<a href=\"" +
    this->handwrittenSolution + "\" class =\"slidesLink\">Handwritten solutions</a>";
  if (this->slidesProjector == "" && this->slidesPrintable == "")
  { std::stringstream
    slideFromSourceStreamHandouT, slideFromSourceStreamHandoutLink,
    slideFromSourceStreamProjectoR, homeworkFromSourceStreamNoAnswerKey, homeworkFromSourceStreamAnswerKey,
    sourceStreamSlides, sourceStreamSlidesLink, sourceStreamHomework,
    sourceStreamSlidesCommon, sourceStreamHomeworkCommon;

    slideFromSourceStreamHandouT << "<a href=\"";
    slideFromSourceStreamHandoutLink << theGlobalVariables.DisplayNameExecutable << "?request=slidesFromSource&";
    homeworkFromSourceStreamNoAnswerKey << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=homeworkFromSource&";

    sourceStreamSlidesLink << theGlobalVariables.DisplayNameExecutable << "?request=slidesSource&";
    sourceStreamHomework << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=homeworkSource&";

    sourceStreamSlidesCommon << "title=" << HtmlRoutines::ConvertStringToURLString(this->title, false) << "&";
    sourceStreamHomeworkCommon << "title=" << HtmlRoutines::ConvertStringToURLString(this->title, false) << "&";

    /////////
    int sourceSlidesCounter = 0;
    for (int i = 0; i < owner.slidesSourcesHeaders.size; i ++)
    { sourceSlidesCounter ++;
      sourceStreamSlidesCommon << "file" << sourceSlidesCounter
      << "=" << HtmlRoutines::ConvertStringToURLString(owner.slidesSourcesHeaders[i], false) << "&";
    }
    for (int i = 0; i < this->sourceSlides.size; i ++)
    { sourceSlidesCounter ++;
      sourceStreamSlidesCommon << "file" << sourceSlidesCounter
      << "=" << HtmlRoutines::ConvertStringToURLString(this->sourceSlides[i], false) << "&";
    }
    /////////
    int sourceHomeworkCounter = 0;
    for (int i = 0; i < owner.sourcesHomeworkHeaders.size; i ++)
    { sourceHomeworkCounter ++;
      sourceStreamHomeworkCommon << "file" << sourceHomeworkCounter
      << "=" << HtmlRoutines::ConvertStringToURLString(owner.sourcesHomeworkHeaders[i], false) << "&isSolutionFile"
      << sourceHomeworkCounter << "=false&";
    }
    for (int i = 0; i < this->sourceHomework.size; i ++)
    { sourceHomeworkCounter ++;
      sourceStreamHomeworkCommon << "file" << sourceHomeworkCounter
      << "=" << HtmlRoutines::ConvertStringToURLString(this->sourceHomework[i], false) << "&";
      if (i < this->sourceHomeworkIsSolution.size)
      { if (this->sourceHomeworkIsSolution[i])
          sourceStreamHomeworkCommon << "isSolutionFile" << sourceHomeworkCounter << "=true&";
        else
          sourceStreamHomeworkCommon << "isSolutionFile" << sourceHomeworkCounter << "=false&";
      } else
      { stOutput << "<b>ERROR: </b>this->sourceHomeworkIsSolution is: "
        << this->sourceHomeworkIsSolution.ToStringCommaDelimited()
        << " but this->sourceHomework is: " << this->sourceHomework.ToStringCommaDelimited();
      }
    }
    /////////
    slideFromSourceStreamHandoutLink << sourceStreamSlidesCommon.str();
    this->slidesProjector = slideFromSourceStreamHandoutLink.str() + "layout=projector&";
    slideFromSourceStreamProjectoR << "<a href=\"" << this->slidesProjector;
    slideFromSourceStreamHandoutLink << "layout=printable&";

    homeworkFromSourceStreamNoAnswerKey << sourceStreamHomeworkCommon.str();
    homeworkFromSourceStreamAnswerKey << homeworkFromSourceStreamNoAnswerKey.str();

    sourceStreamSlidesLink << sourceStreamSlidesCommon.str();
    sourceStreamHomework << sourceStreamHomeworkCommon.str();

    slideFromSourceStreamHandouT << slideFromSourceStreamHandoutLink.str();
    slideFromSourceStreamHandouT << "\" class =\"slidesLink\" target =\"_blank\">Printable slides</a>";
    homeworkFromSourceStreamNoAnswerKey << "answerKey=true&";
    homeworkFromSourceStreamNoAnswerKey << "\" class =\"slidesLink\" target =\"_blank\">Homework with answers</a>";
    homeworkFromSourceStreamAnswerKey << "answerKey=false&";
    homeworkFromSourceStreamAnswerKey << "\" class =\"slidesLink\" target =\"_blank\">Homework</a>";

    slideFromSourceStreamProjectoR << "\" class =\"slidesLink\" target =\"_blank\">Slides</a>";

    sourceStreamSlidesLink << "layout=printable&";
    this->linkSlidesTex = sourceStreamSlidesLink.str();
    sourceStreamSlides << "<a href=\"" << this->linkSlidesTex << "\" class =\"slidesLink\" download =\""
    << FileOperations::ConvertStringToLatexFileName(this->title) << ".tex\">.tex</a>";

    sourceStreamHomework << "answerKey=true&\" class =\"slidesLink\" download=\""
    << FileOperations::ConvertStringToLatexFileName(this->title) << ".tex\">.tex</a>";

    this->slidesPrintable = slideFromSourceStreamHandoutLink.str();

    this->displaySlidesLink = "";
    if (this->sourceSlides.size > 0)
      this->displaySlidesLink += slideFromSourceStreamHandouT.str() + slideFromSourceStreamProjectoR.str() + sourceStreamSlides.str();
    if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentVieWOn())
      this->displaySlidesLink += "<a class =\"slidesLink\" style =\"color:gray; display:none\" href=\"" +
      theGlobalVariables.DisplayNameExecutable + "?request=modifySlide&topicID=" + this->id + "\">Modify</a>";
    if (this->sourceHomework.size > 0)
      this->displaySlidesLink += homeworkFromSourceStreamAnswerKey.str() + homeworkFromSourceStreamNoAnswerKey.str() + sourceStreamHomework.str();
  }
  bool problemSolved = false;
  bool returnEmptyStringIfNoDeadline = false;
  if (this->problemFileName == "")
  { if (this->type == this->tProblem)
      this->displayProblemLink = "(theory)";
    else
      this->displayProblemLink = "";
    this->displayScore = "";
    this->displayModifyWeight = "";
    problemSolved = false;
    returnEmptyStringIfNoDeadline = true;
  } else
  { //std::string theRawSQLink = theGlobalVariables.DisplayNameExecutable +
    //"?request=scoredQuiz&fileName=" + this->problem;
    std::string theRawExerciseLink;
    theRawExerciseLink = theGlobalVariables.DisplayNameExecutable + "?request=exercise&fileName=" + this->problemFileName;
    this->displayProblemLink = owner.ToStringLinkFromFileName(this->problemFileName);
    this->displayScore = owner.ToStringProblemScoreShort(this->problemFileName, problemSolved);
    this->displayModifyWeight = owner.ToStringProblemWeightButton(this->problemFileName);
  }
  if (this->problemFileName == "" && this->type == this->tProblem)
    this->displayDeadlinE = "";
  else
    this->displayDeadlinE = owner.ToStringDeadline
    (this->id, problemSolved, returnEmptyStringIfNoDeadline, (this->type != this->tProblem));
  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentVieWOn() &&
      theGlobalVariables.userCalculatorRequestType != "templateNoLogin")
  { if (this->displayDeadlinE == "")
      this->displayDeadlinE += "Deadline";
    owner.ComputeDeadlineModifyButton
    (*this, problemSolved, this->type == this->tSubSection || this->type == this->tSection || this->type == this->tChapter);
    //std::stringstream titleAndDeadlineStream;
    //titleAndDeadlineStream
    //<< "<span class =\"deadlineAndTitleContainer\">"
    //<< "<span class =\"titleContainer\">" << this->displayTitle
    //<< "</span>"
    //<< "&nbsp;&nbsp;<span style =\"font-weight:normal; font-size: medium\">"
    //<< this->displaySlidesLink
    //<< "</span>"
    //<< "" << this->displayDeadlinE << "</span>"
    //<< "</span>"
    //
    //;
    //this->displayTitleWithDeadline = titleAndDeadlineStream.str();
  }
  std::stringstream displayResourcesLinksStream;
  displayResourcesLinksStream
  << this->displayVideoLink << this->displayVideoHandwrittenLink << this->displaySlidesLink
  << this->displaySlidesPrintableLink << this->displayHandwrittenSolution;
  this->displayResourcesLinks = displayResourcesLinksStream.str();
  if (this->problemFileName != "")
    owner.NumProblemsFound ++;
  if (this->video != "")
    owner.NumVideosWithSlidesFound ++;
  if (this->videoHandwritten != "")
    owner.NumVideosHandwrittenFound ++;
  if (this->displaySlidesLink != "")
    owner.NumSlidesFound ++;
}

JSData TopicElement::ToJSON(CalculatorHTML& owner)
{ MacroRegisterFunctionWithName("TopicElement::ToJSON");
  JSData output;
  output["title"] = this->title;
  switch (this->type)
  { case TopicElement::tChapter:
      output["type"] = (std::string) "chapter";
      break;
    case TopicElement::tSection:
      output["type"] = (std::string) "section";
      break;
    case TopicElement::tSubSection:
      output["type"] = (std::string) "subSection";
      break;
    case TopicElement::tProblem:
      output["type"] = (std::string) "problem";
      break;
    case TopicElement::tError:
      output["type"] = (std::string) "error";
      break;
    default:
      output["type"] = (std::string) "Not documented";
      break;
  }
  //output["DEBUGindicesImmediateChildren"].type = JSData::JSarray;
  output["children"].type = JSData::JSarray;
  this->ComputeLinks(owner, true);
  if (this->type == TopicElement::tProblem && this->immediateChildren.size > 0)
  { crash << "Error: Problem " << this->ToString() << " reported to have children topic elements: "
    << this->immediateChildren.ToStringCommaDelimited() << crash;
  }
  for (int i = 0; i < this->immediateChildren.size; i ++)
  { TopicElement& currentChild = owner.theTopicS[this->immediateChildren[i]];
    //std::stringstream out;
    //out << this->immediateChildren[i];
    //output["DEBUGindicesImmediateChildren"][i] = out.str();
    output["children"].list.AddOnTop(currentChild.ToJSON(owner));
  }
  output["problemNumberString"] = this->problemNumberString;
  output["video"] = this->video;
  output["videoHandwritten"] = this->videoHandwritten;
  output["slidesProjector"] = this->slidesProjector;
  output["slidesPrintable"] = this->slidesPrintable;
  output[DatabaseStrings::labelDeadlines] = this->deadlinesPerSectioN;
  //stOutput << "DEBUG: deadlinesPerSection: " << this->deadlinesPerSectioN.ToStringCommaDelimited();
  if (!theGlobalVariables.UserDefaultHasProblemComposingRights())
  { std::string theDeadline = owner.GetDeadlineNoInheritance(this->id);
    output[WebAPI::problemSingleDeadline] = theDeadline;
  }
  if (theGlobalVariables.UserDefaultHasProblemComposingRights())
    output["linkSlidesLaTeX"] = this->linkSlidesTex;
  output["handwrittenSolution"]  = this->handwrittenSolution;

  output[WebAPI::problemFileName] = this->problemFileName;
  output[WebAPI::problemId] = this->id;
#ifdef MACRO_use_MongoDB
  if (owner.currentUseR.theProblemData.Contains(this->problemFileName))
  { ProblemData& currentData = owner.currentUseR.theProblemData.GetValueCreate(this->problemFileName);
    output["correctlyAnswered"] = currentData.numCorrectlyAnswered;
    output["totalQuestions"] = currentData.theAnswers.size();
    Rational currentWeight;
    std::string currentWeightAsGivenByInstructor;
    currentData.flagProblemWeightIsOK =
    currentData.adminData.GetWeightFromCoursE
    (owner.currentUseR.courseComputed, currentWeight, &currentWeightAsGivenByInstructor);
    if (currentData.flagProblemWeightIsOK)
      output["weight"] = currentWeightAsGivenByInstructor;
  }
#endif
  return output;
}

std::string TopicElement::ToString() const
{ std::stringstream out;
  out << this->title << ", id: " << this->id << " ";
  if (this->title == "")
    out << "-";
  if (this->type == this->tProblem)
    out << "(problem)";
  if (this->type == this->tChapter)
    out << "(chapter)";
  if (this->type == this->tSection)
    out << "(section)";
  if (this->type == this->tSubSection)
    out << "(subsection)";
  out << ". Index in parent: " << this->indexInParent;
  out << ". Parents: " << this->parentTopics.ToStringCommaDelimited()
  << ". Immediate children: " << this->immediateChildren.ToStringCommaDelimited() << ". ";
  return out.str();
}
