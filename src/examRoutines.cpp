//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"
#include "calculator_html_functions.h"
#include "database.h"
#include "general_time_date.h"
#include "calculator_html_interpretation.h"
#include "html_snippets.h"
#include "calculator_html_interpretation_interface.h"
#include "math_extra_latex_routines.h"
#include <iomanip>
#include "database_mongo.h"
#include "string_constants.h"

extern ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCustomCode;
ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutinesCustomCode(
  __FILE__, "Routines for calculus teaching: calculator exam mode. Shared code. "
);

std::string CalculatorHTML::stringScoredQuizzes = "Quiz";
std::string CalculatorHTML::stringPracticE = "Practice";
std::string CalculatorHTML::stringProblemLink = "Problem";

CalculatorHTML::CalculatorHTML() {
  this->NumAttemptsToInterpret = 0;
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

bool CalculatorHTML::LoadProblemInfoFromJSONAppend(
  const JSData& inputJSON,
  MapList<std::string, ProblemData, MathRoutines::HashString>& outputProblemInfo,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::LoadProblemInfoFromJSONAppend");
  (void) commentsOnFailure;
  if (inputJSON.theType == JSData::token::tokenUndefined) {
    return true;
  }
  outputProblemInfo.SetExpectedSize(inputJSON.objects.size());
  ProblemData emptyData;
  std::string currentCourse = theGlobalVariables.userDefault.courseComputed;
  for (int i = 0; i < inputJSON.objects.size(); i ++) {
    std::string currentProbName = inputJSON.objects.theKeys[i];
    JSData& currentProblem = inputJSON.objects.theValues[i];
    if (currentProbName == "") {
      continue;
    }
    if (!outputProblemInfo.Contains(currentProbName)) {
      outputProblemInfo.GetValueCreate(currentProbName) = emptyData;
    }
    ProblemData& currentProblemValue = outputProblemInfo.GetValueCreate(currentProbName);
    JSData& currentDeadlines = currentProblem[DatabaseStrings::labelDeadlines];
    JSData& currentWeight = currentProblem[DatabaseStrings::labelProblemWeights];
    if (currentWeight.theType == JSData::token::tokenObject) {
      for (int j = 0; j < currentWeight.objects.size(); j ++) {
        JSData& currentWeightValue = currentWeight.objects.theValues[j];
        if (currentWeightValue.theType != JSData::token::tokenString) {
          continue;
        }
        currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue(
          currentWeight.objects.theKeys[j], currentWeightValue.theString
        );
      }
    } else if (currentWeight.theType == JSData::token::tokenString) {
      currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue(currentCourse, currentWeight.theString);
    } else if (currentWeight.theType != JSData::token::tokenUndefined) {
      commentsOnFailure << "Could extract neither weight nor weights-per course from "
      << currentWeight.ToString(false) << ". Your input was: " << inputJSON.ToString(false);
      return false;
    }
    if (currentDeadlines.theType != JSData::token::tokenUndefined) {
      for (int j = 0; j < currentDeadlines.objects.size(); j ++) {
        currentProblemValue.adminData.deadlinesPerSection.SetKeyValue(
          currentDeadlines.objects.theKeys[j], currentDeadlines.objects.theValues[j].theString
        );
      }
    }
  }
  return true;
}

bool CalculatorHTML::LoadProblemInfoFromURLedInputAppend(
  const std::string& inputInfoString,
  MapList<std::string, ProblemData, MathRoutines::HashString>& outputProblemInfo,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::LoadProblemInfoFromURLedInputAppend");
  MapList<std::string, std::string, MathRoutines::HashString>
  CGIedProbs, currentKeyValues, sectionDeadlineInfo, problemWeightInfo;
  if (!HtmlRoutines::ChopCGIString(inputInfoString, CGIedProbs, commentsOnFailure)) {
    return false;
  }
  outputProblemInfo.SetExpectedSize(outputProblemInfo.size() + CGIedProbs.size());
  std::string currentProbName, currentProbString;
  ProblemData emptyData;
  for (int i = 0; i < CGIedProbs.size(); i ++) {
    currentProbName = StringRoutines::StringTrimWhiteSpace(
      HtmlRoutines::ConvertURLStringToNormal(CGIedProbs.theKeys[i], false)
    );
    if (currentProbName == "") {
      continue;
    }
    currentProbString = HtmlRoutines::ConvertURLStringToNormal(CGIedProbs.theValues[i], false);
    if (!outputProblemInfo.Contains(currentProbName)) {
      outputProblemInfo.GetValueCreate(currentProbName) = emptyData;
    }
    ProblemData& currentProblemValue = outputProblemInfo.GetValueCreate(currentProbName);
    if (!HtmlRoutines::ChopCGIString(currentProbString, currentKeyValues, commentsOnFailure)) {
      return false;
    }
    //stOutput << "<hr>Debug: reading problem info from: " << currentProbString << " resulted in pairs: "
    //<< currentKeyValues.ToStringHtml();
    std::string deadlineString = HtmlRoutines::ConvertURLStringToNormal(
      currentKeyValues.GetValueCreate(DatabaseStrings::labelDeadlines), false
    );
    std::string problemWeightsCollectionString = HtmlRoutines::ConvertURLStringToNormal(
      currentKeyValues.GetValueCreate(DatabaseStrings::labelProblemWeights), false
    );
    if (problemWeightsCollectionString != "") {
      if (!HtmlRoutines::ChopCGIString(problemWeightsCollectionString, problemWeightInfo, commentsOnFailure)) {
        return false;
      }
      for (int j = 0; j < problemWeightInfo.size(); j ++) {
        currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue(
          HtmlRoutines::ConvertURLStringToNormal(problemWeightInfo.theKeys[j], false),
          HtmlRoutines::ConvertURLStringToNormal(problemWeightInfo.theValues[j], false)
        );
      }
    }
    if (deadlineString != "") {
      if (!HtmlRoutines::ChopCGIString(deadlineString, sectionDeadlineInfo, commentsOnFailure)) {
        return false;
      }
      for (int j = 0; j < sectionDeadlineInfo.size(); j ++) {
        currentProblemValue.adminData.deadlinesPerSection.SetKeyValue(
          HtmlRoutines::ConvertURLStringToNormal(sectionDeadlineInfo.theKeys[j], false),
          HtmlRoutines::ConvertURLStringToNormal(sectionDeadlineInfo.theValues[j], false)
        );
      }
    }
    std::string problemWeightString = StringRoutines::StringTrimWhiteSpace(
      currentKeyValues.GetValueCreate(DatabaseStrings::labelProblemWeights)
    );
    if (problemWeightString != "") {
      currentProblemValue.adminData.problemWeightsPerCoursE.SetKeyValue(
        HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.userDefault.courseComputed, false),
        HtmlRoutines::ConvertURLStringToNormal(problemWeightString, false)
      );
    }
  }
  return true;
}

JSData CalculatorHTML::ToJSONDeadlines(
  MapList<std::string, ProblemData, MathRoutines::HashString>& inputProblemInfo
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToJSONDeadlines");
  JSData output;
  output.theType = JSData::token::tokenObject;

  for (int i = 0; i < inputProblemInfo.size(); i ++) {
    ProblemDataAdministrative& currentProblem = inputProblemInfo.theValues[i].adminData;
    if (currentProblem.deadlinesPerSection.size() == 0) {
      continue;
    }
    std::string currentProblemName = inputProblemInfo.theKeys[i];
    JSData currentProblemJSON;
    for (int j = 0; j < currentProblem.deadlinesPerSection.size(); j ++) {
      std::string currentDeadline = StringRoutines::StringTrimWhiteSpace(
        currentProblem.deadlinesPerSection.theValues[j]
      );
      if (currentDeadline == "") {
        continue;
      }
      std::string currentSection = StringRoutines::StringTrimWhiteSpace(
        currentProblem.deadlinesPerSection.theKeys[j]
      );
      currentProblemJSON[DatabaseStrings::labelDeadlines][currentSection] = currentDeadline;
    }
    output[currentProblemName] = currentProblemJSON;
  }
  return output;
}

JSData CalculatorHTML::ToJSONProblemWeights(
  MapList<std::string, ProblemData, MathRoutines::HashString>& inputProblemInfo
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToJSONProblemWeights");
  JSData output;
  for (int i = 0; i < inputProblemInfo.size(); i ++) {
    ProblemDataAdministrative& currentProblem = inputProblemInfo.theValues[i].adminData;
    if (currentProblem.problemWeightsPerCoursE.size() == 0) {
      continue;
    }
    std::string currentProblemName = inputProblemInfo.theKeys[i];
    JSData currentProblemJSON;
    for (int j = 0; j < currentProblem.problemWeightsPerCoursE.size(); j ++) {
      std::string currentWeight = StringRoutines::StringTrimWhiteSpace(currentProblem.problemWeightsPerCoursE.theValues[j]);
      if (currentWeight == "") {
        continue;
      }
      std::string currentCourse = StringRoutines::StringTrimWhiteSpace(currentProblem.problemWeightsPerCoursE.theKeys[j]);
      currentProblemJSON[currentCourse] = currentWeight;
    }
    JSData currentWeight;
    currentWeight[DatabaseStrings::labelProblemWeights] = currentProblemJSON;
    output[currentProblemName] = currentWeight;
  }
  return output;
}

bool DatabaseRoutineS::StoreProblemInfoToDatabase(
  const UserCalculatorData& theUser, bool overwrite, std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  JSData findQueryWeights, findQueryDeadlines;
  findQueryWeights[DatabaseStrings::labelProblemWeightsSchema] = theUser.problemWeightSchema;
  findQueryDeadlines[DatabaseStrings::labelDeadlinesSchema] = theUser.deadlineSchema;
  if (theUser.problemWeights.theType != JSData::token::tokenUndefined) {
    if (overwrite) {
      JSData setQueryWeights;
      setQueryWeights[DatabaseStrings::labelProblemWeights] = theUser.problemWeights;
      if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
        DatabaseStrings::tableProblemWeights,
        findQueryWeights,
        setQueryWeights,
        nullptr,
        &commentsOnFailure
      )) {
        return false;
      }
    } else {
      List<std::string> adjustLabels;
      adjustLabels.SetSize(2);
      adjustLabels[0] = DatabaseStrings::labelProblemWeights;
      for (int i = 0; i < theUser.problemWeights.objects.size(); i ++) {
        adjustLabels[1] = theUser.problemWeights.objects.theKeys[i];
        if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
          DatabaseStrings::tableProblemWeights,
          findQueryWeights, theUser.problemWeights.objects.theValues[i],
          &adjustLabels,
          &commentsOnFailure
        )) {
          return false;
        }
      }
    }
  }
  if (theUser.deadlines.theType != JSData::token::tokenUndefined) {
    if (overwrite) {
      JSData setQueryDeadlines;
      setQueryDeadlines[DatabaseStrings::labelDeadlines] = theUser.deadlines;
      if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
        DatabaseStrings::tableProblemWeights,
        findQueryDeadlines,
        setQueryDeadlines,
        nullptr,
        &commentsOnFailure
      )) {
        return false;
      }
    } else {
      List<std::string> adjustLabels;
      adjustLabels.SetSize(2);
      adjustLabels[0] = DatabaseStrings::labelDeadlines;
      for (int i = 0; i < theUser.deadlines.objects.size(); i ++) {
        adjustLabels[1] = theUser.deadlines.objects.theKeys[i];
        if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
          DatabaseStrings::tableDeadlines,
          findQueryDeadlines,
          theUser.deadlines.objects.theValues[i],
          &adjustLabels,
          &commentsOnFailure
        )) {
          return false;
        }
      }
    }
  }
  return true;
}

bool CalculatorHTML::MergeOneProblemAdminData(
  const std::string& inputProblemName,
  ProblemData& inputProblemInfo,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::MergeOneProblemAdminData");
  if (!this->theTopicS.Contains(inputProblemName)) {
    commentsOnFailure << "Did not find " << inputProblemName
    << " among the list of topics/problems. ";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn()) {
      commentsOnFailure << "The topics are: " << this->theTopicS.ToStringHtml();
    }
    return false;
  }
  if (!this->currentUseR.theProblemData.Contains(inputProblemName)) {
    this->currentUseR.theProblemData.SetKeyValue(inputProblemName, inputProblemInfo);
  }
  ProblemDataAdministrative& currentProblem =
  this->currentUseR.theProblemData.GetValueCreate(inputProblemName).adminData;
  MapList<std::string, std::string, MathRoutines::HashString>&
  currentDeadlines = currentProblem.deadlinesPerSection;
  MapList<std::string, std::string, MathRoutines::HashString>&
  incomingDeadlines = inputProblemInfo.adminData.deadlinesPerSection;
  MapList<std::string, std::string, MathRoutines::HashString>&
  currentWeightS = currentProblem.problemWeightsPerCoursE;
  MapList<std::string, std::string, MathRoutines::HashString>&
  incomingWeightS = inputProblemInfo.adminData.problemWeightsPerCoursE;

  for (int i = 0; i < incomingDeadlines.size(); i ++) {
    if (this->databaseStudentSections.size >= 1000) {
      commentsOnFailure << "Failed to account deadlines: max 999 sections allowed. ";
      return false;
    }
    this->databaseStudentSections.AddOnTopNoRepetition(incomingDeadlines.theKeys[i]);
  }
  ////////////////////////////////////////////
  for (int i = 0; i < incomingDeadlines.size(); i ++) {
    currentDeadlines.SetKeyValue(incomingDeadlines.theKeys[i], incomingDeadlines.theValues[i]);
  }
  for (int i = 0; i < incomingWeightS.size(); i ++) {
    currentWeightS.SetKeyValue(incomingWeightS.theKeys[i], incomingWeightS.theValues[i]);
  }
  return true;
}

bool CalculatorHTML::MergeProblemInfoInDatabaseJSON(
  std::string& incomingProblemInfo, std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::MergeProblemInfoInDatabase");
  JSData theProblemJSON;
  if (!theProblemJSON.readstring(incomingProblemInfo, false, &commentsOnFailure)) {
    return false;
  }
  MapList<std::string, ProblemData, MathRoutines::HashString> incomingProblems;
  if (!this->LoadProblemInfoFromJSONAppend(theProblemJSON, incomingProblems, commentsOnFailure)) {
    commentsOnFailure << "Failed to parse your request";
    return false;
  }
  theGlobalVariables.userDefault.problemWeights = this->ToJSONProblemWeights(incomingProblems);
  theGlobalVariables.userDefault.deadlines = this->ToJSONDeadlines(incomingProblems);
  if (!DatabaseRoutineS::StoreProblemInfoToDatabase(theGlobalVariables.userDefault, false, commentsOnFailure)) {
    return false;
  }
  return true;
}

bool CalculatorHTML::LoadDatabaseInfo(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::LoadDatabaseInfo");
  if (!theGlobalVariables.flagDatabaseCompiled) {
    comments << "Database not available (cannot load problem info). ";
    return false;
  }
  this->currentUseR.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  if (!this->PrepareSectionList(comments)) {
    return false;
  }
  if (this->currentUseR.problemDataJSON.objects.size() != 0) {
    if (!this->currentUseR.InterpretDatabaseProblemDataJSON(this->currentUseR.problemDataJSON, comments)) {
      comments << "Failed to interpret user's problem saved data. ";
      return false;
    }
  } else {
    if (!this->currentUseR.InterpretDatabaseProblemDatA(this->currentUseR.problemDataStrinG, comments)) {
      comments << "Failed to interpret user's problem saved data. ";
      return false;
    }
  }
  if (!this->LoadProblemInfoFromJSONAppend(
    this->currentUseR.problemWeights, this->currentUseR.theProblemData, comments
  )) {
    comments << "Failed to load problem weights. ";
    return false;
  }
  if (!this->LoadProblemInfoFromJSONAppend(
    this->currentUseR.deadlines, this->currentUseR.theProblemData, comments
  )) {
    comments << "Failed to load problem deadlines. ";
    return false;
  }

  if (this->currentUseR.theProblemData.Contains(this->fileName)) {
    this->theProblemData = this->currentUseR.theProblemData.GetValueCreate(this->fileName);
  }
  theGlobalVariables.userDefault = this->currentUseR;
  return true;
}

bool CalculatorHTML::LoadMe(
  bool doLoadDatabase, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::LoadMe");
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(
    this->fileName, this->RelativePhysicalFileNameWithFolder, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get physical file name from " << this->fileName << ". ";
    }
    return false;
  }
  (void) doLoadDatabase;
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(this->fileName, this->inputHtml, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<br>User-input file name: <b>" << this->fileName << "</b>";
    }
    return false;
  }
  this->flagIsForReal = theGlobalVariables.UserRequestRequiresLoadingRealExamData();
  if (theGlobalVariables.flagDatabaseCompiled) {
    this->topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
    if (doLoadDatabase) {
      std::stringstream errorStream;
      this->LoadDatabaseInfo(errorStream);
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << errorStream.str();
      }
      for (int i = 0; i < this->theTopicS.size(); i ++) {
        this->ComputeDeadlinesAllSectionsNoInheritance(this->theTopicS.theValues[i]);
      }
    }
  }
  this->theProblemData.CheckConsistency();
  if (!this->flagIsForReal) {
    std::string randString = inputRandomSeed;
    if (randString != "") {
      std::stringstream randSeedStream(randString);
      randSeedStream >> this->theProblemData.randomSeed;
      this->theProblemData.flagRandomSeedGiven = true;
    }
  }
  return true;
}

std::string CalculatorHTML::LoadAndInterpretCurrentProblemItemJSON(
  bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::LoadAndInterpretCurrentProblemItemJSON");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  this->LoadCurrentProblemItem(needToLoadDatabaseMayIgnore, desiredRandomSeed, commentsOnFailure);
  if (!this->flagLoadedSuccessfully) {
    return WebAPI::problem::failedToLoadProblem;
  }
  std::stringstream out;
  if (!this->InterpretHtml(commentsOnFailure)) {
    out << "<b>Failed to interpret file: " << this->fileName << "</b>. ";
    out << "<br>We limit the number of generation attemps to 10 for performance reasons; "
    << "with bad luck, some finicky problems require more. "
    << "<br> <b>Please refresh the page.</b><br>";
    if (!this->flagIsForReal) {
      out
      << "If you specified the problem through the 'problem link' link,"
      << " please go back to the course page. Alternatively, remove the randomSeed=... "
      << "portion of the page address and reload the page (with the randomSeed portion removed). ";
    } else {
      out << "<b>Your random seed must have been reset. </b>";
    }
    out << "<br><b style =\"color:red\">If the problem persists after a couple of page refreshes, "
    << "it's a bug. Please take a screenshot and email the site admin/your instructor. </b>";
    out
    << "Generated in "
    << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds() - startTime)
    << " second(s). ";
    return out.str();
  }
  if (this->flagDoPrependProblemNavigationBar) {
    out << this->outputProblemNavigatioN;
  }
  out << this->outputHtmlBodyNoTag;
  return out.str();
}

void CalculatorHTML::LoadFileNames() {
  this->fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput(WebAPI::problem::fileName), false);
  this->courseHome = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  this->topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
}

void CalculatorHTML::LoadCurrentProblemItem(
  bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->LoadFileNames();
  this->flagLoadedSuccessfully = false;
  if (theGlobalVariables.UserGuestMode()) {
    needToLoadDatabaseMayIgnore = false;
  }
  this->flagLoadedSuccessfully = true;
  if (this->fileName == "") {
    this->flagLoadedSuccessfully = false;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b>No problem file name found. </b>";
    }
  }
  if (!this->LoadMe(needToLoadDatabaseMayIgnore, inputRandomSeed, commentsOnFailure)) {
    this->flagLoadedSuccessfully = false;
  }
  this->theProblemData.CheckConsistency();
}

bool CalculatorHTML::IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt) {
  MacroRegisterFunctionWithName("CalculatorHTML::IsStateModifierApplyIfYes");
  if (inputElt.syntacticRole != "command") {
    return false;
  }
  std::string tagClass = inputElt.GetKeyValue("class");
  if (tagClass == "setCalculatorExamHome") {
    this->flagIsExamHome = true;
    this->flagIsExamProblem = false;
    theGlobalVariables.SetWebInpuT("courseHome", HtmlRoutines::ConvertStringToURLString(this->fileName, false));
  }
  if (tagClass == "setCalculatorExamProblem") {
    this->flagIsExamHome = false;
    this->flagIsExamProblem = true;
    return true;
  }
  return false;
}

std::string Answer::ToString() {
  MacroRegisterFunctionWithName("Answer::ToString");
  std::stringstream out;
  out << "Answer id: " << this->answerId;
  out << "<br>Answer commands on give-up: " << this->commandsNoEnclosureAnswerOnGiveUpOnly;
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringCalculatorProblemSourceFromFileName");
  std::stringstream out;
  out //<< "<span class =\"calculatorExamProblem\">\n"
  << "Title: " << theFileName << "\n"
  << "Problem: " << theFileName << "\n"
  //<< "\n</span>"
  ;
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  inputOutput.interpretedCommand = this->ToStringProblemInfo(this->CleanUpFileName(inputOutput.content));
}

std::string CalculatorHTML::ToStringLinkCurrentAdmin(
  const std::string& displayString, bool setDebugFlag, bool includeRandomSeed
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkCurrentAdmin");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return "";
  }
  std::stringstream out;
  out << "<a class =\"linkStandardButtonLike\" href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
  << theGlobalVariables.userCalculatorRequestType << "&";
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(this->fileName, false);
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  out << "fileName=" << urledProblem << "&"
  << theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (includeRandomSeed) {
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
  }
  if (setDebugFlag) {
    out << "debugFlag=true&";
  } else {
    out << "debugFlag=false&";
  }
  if (this->topicListFileName != "") {
    out << "topicList=" << this->topicListFileName << "&";
  }
  if (this->courseHome != "") {
    out << "courseHome=" << this->courseHome << "&";
  }
  if (theGlobalVariables.UserStudentVieWOn()) {
    out << "studentView=true&";
    if (theGlobalVariables.GetWebInput("studentSection") != "") {
      out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
    }
  }
  out << "\">" << displayString << "</a>";
  return out.str();
}

std::string CalculatorHTML::ToStringLinkFromFileName(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkFromFileName");
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(theFileName, false);
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation(nullptr)
  << "fileName=" << urledProblem << "&";
  if (theGlobalVariables.UserStudentVieWOn()) {
    refStreamNoRequest << "studentView=true&";
    if (theGlobalVariables.GetWebInput("studentSection") != "") {
      refStreamNoRequest << "studentSection="
      << theGlobalVariables.GetWebInput("studentSection") << "&";
    }
  }
  if (this->topicListFileName != "") {
    refStreamNoRequest << "topicList=" << this->topicListFileName << "&";
  }
  if (this->courseHome != "") {
    refStreamNoRequest << "courseHome=" << this->courseHome << "&";
  }
  if (
    theFileName == this->topicListFileName ||
    theFileName == this->courseHome ||
    StringRoutines::StringEndsWith(theFileName, ".txt")
  ) {
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=template&" << refStreamNoRequest.str() << "\">" << "Home" << "</a> ";
    return out.str();
  }
  if (!theGlobalVariables.UserGuestMode()) {
    refStreamExercise << theGlobalVariables.DisplayNameExecutable
    << "?request=exercise&" << refStreamNoRequest.str();
    refStreamForReal << theGlobalVariables.DisplayNameExecutable
    << "?request=scoredQuiz&" << refStreamNoRequest.str();
  } else {
    refStreamExercise << "?request=exerciseNoLogin&" << refStreamNoRequest.str();
  }
  if (!theGlobalVariables.UserGuestMode()) {
    out << "<b><a class =\"problemLinkQuiz\" href=\"" << refStreamForReal.str() << "\">"
    << CalculatorHTML::stringScoredQuizzes << "</a></b>";
  }
  out << "<a class =\"problemLinkPractice\" href=\"" << refStreamExercise.str() << "\">"
  << CalculatorHTML::stringPracticE << "</a>";
  return out.str();
}

std::string CalculatorHTML::ToStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinksFromFileName");
  std::stringstream out;
  out << this->ToStringLinkFromFileName(theFileName);
  out << this->ToStringProblemScoreFull(theFileName);
  out << this->ToStringProblemWeightButton(theFileName);
  if (theGlobalVariables.flagDatabaseCompiled) {
    bool problemAlreadySolved = false;
    if (this->currentUseR.theProblemData.Contains(theFileName)) {
      ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
      if (theProbData.numCorrectlyAnswered >= theProbData.theAnswers.size()) {
        problemAlreadySolved = true;
      }
    }
    out << this->ToStringDeadline(theFileName, problemAlreadySolved, false, false);
  }
  std::string finalStringToDisplay = stringToDisplay;
  if (finalStringToDisplay == "") {
    finalStringToDisplay = FileOperations::GetFileNameFromFileNameWithPath(theFileName);
  }
  out << finalStringToDisplay;
  return out.str();
}

bool CalculatorHtmlFunctions::innerInterpretProblemGiveUp(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretProblemGiveUp");
  (void) input;
  return output.AssignValue(
    HtmlInterpretation::GetAnswerOnGiveUp(theGlobalVariables.GetWebInput("randomSeed")), theCommands
  );
}

bool CalculatorHtmlFunctions::innerInterpretProblem(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretProblem");
  CalculatorHTML theProblem;
  if (!input.IsOfType<std::string>(&theProblem.inputHtml)) {
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  }
  theProblem.theProblemData.flagRandomSeedGiven = true;
  theProblem.theProblemData.randomSeed = static_cast<unsigned>(theCommands.theObjectContainer.CurrentRandomSeed);
  theProblem.InterpretHtml(&theCommands.Comments);
  std::stringstream out;
  out << theProblem.outputHtmlBodyNoTag;
  out << "<hr>Time to parse html: " << std::fixed << theProblem.timeToParseHtml << " second(s). ";
  out << "<br>Intermediate interpretation times (per attempt): ";
  for (int i = 0; i < theProblem.timeIntermediatePerAttempt.size; i ++) {
    for (int j = 0; j < theProblem.timeIntermediateComments[i].size; j ++) {
      out << "<br>" << theProblem.timeIntermediateComments[i][j]
      << ": " << theProblem.timeIntermediatePerAttempt[i][j] << " second(s)";
    }
  }
  out << "<br>Interpretation times (per attempt): "
  << theProblem.timePerAttempt.ToStringCommaDelimited();
  return output.AssignValue(out.str(), theCommands);
}

std::string CalculatorHTML::ToStringExtractedCommands() {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringExtractedCommands");
  std::stringstream out;
  out << "<hr><b>The commands extracted from the HTML follow.</b><br>";
  out << "<table>";
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].syntacticRole != "") {
      out << "<tr>" << "<td>" << this->theContent[i].ToStringDebug() << "</td>"
      << "</tr>";
    } else {
      out << "<tr><td></td></tr>";
    }
  }
  out << "</table>";
  out << "<hr><b>The HTML from which the commands were extracted follows.</b><br>" << this->inputHtml
  << "<hr><b>The parsing stack follows.</b>" << this->ToStringParsingStack(this->eltsStack) << "<hr>";
  return out.str();
}

std::string CalculatorHTML::ToStringContent() {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringContent");
  std::stringstream out;
//  out << "<hr><b>The split strings follow. </b><hr>" << splitStrings.ToStringCommaDelimited();
  out << "<hr><b>The extracted commands follow.</b><br>";
  for (int i = 0; i < this->theContent.size; i ++) {
    out << this->theContent[i].ToStringTagAndContent();
  }
  out << "<hr><b>The html read follows.</b><br>" << this->inputHtml << "<hr>";
  return out.str();
}

void SyntacticElementHTML::resetAllExceptContent() {
  this->tag = "";
  this->properties.Clear();
  this->syntacticRole = "";
  this->flagUseDisplaystyleInMathMode = false;
  this->children.SetSize(0);
}

std::string SyntacticElementHTML::ToStringOpenTag(const std::string& overrideTagIfNonEmpty, bool immediatelyClose) {
  if (this->tag == "" || this->flagUseMathSpan == false) {
    return "";
  }
  std::stringstream out;
  if (overrideTagIfNonEmpty == "") {
    out << "<" << this->tag;
  } else {
    out << "<" << overrideTagIfNonEmpty;
  }
  for (int i = 0; i < this->properties.size(); i ++) {
    out << " " << this->properties.theKeys[i] << "=\"" << this->properties.theValues[i] << "\"";
  }
  for (int i = 0; i < this->defaultKeysIfMissing.size; i ++) {
    if (!this->properties.Contains(this->defaultKeysIfMissing[i])) {
      out << " " << this->defaultKeysIfMissing[i] << "=\"" << this->defaultValuesIfMissing[i] << "\"";
    }
  }
  if (this->tagKeysWithoutValue.size > 0) {
    out << " " << this->tagKeysWithoutValue[0];
  }
  if (immediatelyClose) {
    out << "/";
  }
  out << ">";
  return out.str();
}

std::string SyntacticElementHTML::ToStringCloseTag(const std::string& overrideTagIfNonEmpty) {
  if (this->tag == "" || this->flagUseMathSpan == false) {
    return "";
  }
  if (overrideTagIfNonEmpty == "") {
    return "</" + this->tag + ">";
  } else {
    return "</" + overrideTagIfNonEmpty + ">";
  }
}

std::string SyntacticElementHTML::ToStringTagAndContent() {
  MacroRegisterFunctionWithName("SyntacticElementHTML::ToStringTagAndContent");
  if (this->syntacticRole == "") {
    return this->content;
  }
  std::stringstream out;
  out << this->ToStringOpenTag("") + this->content + this->ToStringCloseTag("");
  if (this->children.size > 0) {
    out << "[";
    for (int i = 0; i < this->children.size; i ++) {
      out << this->children[i].ToStringDebug();
      if (i != this->children.size - 1) {
        out << ", ";
      }
    }
    out << "]";
  }
  return out.str();
}

std::string SyntacticElementHTML::ToStringDebug() {
  MacroRegisterFunctionWithName("SyntacticElementHTML::ToString");
  if (this->syntacticRole == "") {
    return HtmlRoutines::ConvertStringToHtmlString(this->ToStringTagAndContent(), false);
  }
  std::stringstream out;
  out << "<span style =\"color:green\">";
  out << HtmlRoutines::ConvertStringToHtmlString(this->syntacticRole, false);
  out << "</span>";
  out << "[" << HtmlRoutines::ConvertStringToHtmlString(this->ToStringTagAndContent(), false) << "]";
  return out.str();
}

std::string SyntacticElementHTML::GetKeyValue(const std::string& theKey) const {
  MacroRegisterFunctionWithName("SyntacticElementHTML::GetKeyValue");
  if (!this->properties.Contains(theKey)) {
    return "";
  }
  return this->properties.GetValueConstCrashIfNotPresent(theKey);
}

void SyntacticElementHTML::SetKeyValue(const std::string& theKey, const std::string& theValue) {
  MacroRegisterFunctionWithName("SyntacticElementHTML::SetKeyValue");
  this->properties.SetKeyValue(theKey, theValue);
}

std::string SyntacticElementHTML::ToStringInterpretedBody() {
  if (this->syntacticRole == "") {
    return this->content;
  }
  if (this->IsInterpretedNotByCalculator()) {
    return this->interpretedCommand;
  }
  std::stringstream out;
  out << this->ToStringOpenTag("");
  if (this->interpretedCommand != "") {
    if (this->flagUseMathMode) {
      out << "\\( ";
      if (this->flagUseDisplaystyleInMathMode) {
        out << "\\displaystyle ";
      }
    }
    out << this->interpretedCommand;
    if (this->flagUseMathMode) {
      out << " \\)";
    }
  }
  out << this->ToStringCloseTag("");
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedNotByCalculator() {
  MacroRegisterFunctionWithName("SyntacticElementHTML::IsInterpretedNotByCalculator");
  if (this->syntacticRole != "command") {
    return false;
  }
  if (
    this->tag == "answerCalculatorHighlightStart" ||
    this->tag == "answerCalculatorHighlightFinish"
  ) {
    return true;
  }
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
  this->IsAnswerElement(nullptr);
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringProblemGeneration() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculator" || tagClass == "calculatorHidden" ||
  tagClass == "calculatorShowToUserOnly" ;
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringSubmission() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculator" || tagClass == "calculatorHidden";
}

bool SyntacticElementHTML::IsAnswer() {
  if (this->syntacticRole != "command") {
    return false;
  }
  return this->GetKeyValue("class") == "calculatorAnswer";
}

bool SyntacticElementHTML::IsCalculatorCommand() {
  if (this->syntacticRole != "command") {
    return false;
  }
  return this->GetKeyValue("class") == "calculator";
}

bool SyntacticElementHTML::IsCalculatorHidden() {
  if (this->syntacticRole != "command") {
    return false;
  }
  return this->GetKeyValue("class") == "calculatorHidden";
}

bool SyntacticElementHTML::IsHidden() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorHidden" || tagClass == "calculatorCommentsBeforeInterpretation";
}

bool SyntacticElementHTML::IsSolution() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorSolution";
}

bool SyntacticElementHTML::IsAnswerOnGiveUp() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorAnswerOnGiveUp";
}

bool SyntacticElementHTML::IsCommentBeforeSubmission() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorCommentsBeforeSubmission";
}

bool SyntacticElementHTML::IsCommentBeforeInterpretation() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  return tagClass == "calculatorCommentsBeforeInterpretation";
}

bool SyntacticElementHTML::IsAnswerElement(std::string* desiredAnswerId) {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->GetKeyValue("class");
  bool result = tagClass == "calculatorButtonSubmit" ||
  tagClass == "calculatorButtonInterpret" ||
  tagClass == "calculatorButtonGiveUp" ||
  tagClass == "calculatorButtonSolution" ||
  tagClass == "calculatorMQField" ||
  tagClass == "calculatorMQButtonPanel" ||
  tagClass == "calculatorAnswerVerification" ||
  tagClass == "calculatorSolution";
  if (result && desiredAnswerId != nullptr) {
    *desiredAnswerId = this->GetKeyValue("name");
  }
  return result;
}

std::string CalculatorHTML::PrepareUserInputBoxes() {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareInterpreter");
  if (this->flagIsForReal) {
    return "";
  }
  std::stringstream out;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = theGlobalVariables.webArguments;
  std::string inputNonAnswerReader;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::StringBeginsWith(theArgs.theKeys[i], "userInputBox", &inputNonAnswerReader)) {
      if (inputNonAnswerReader != "" && theArgs.theValues[i] != "") {
        out << "setInputBox(name = "
        << inputNonAnswerReader
        << ", value = " << HtmlRoutines::ConvertURLStringToNormal(theArgs.theValues[i], false)
        << "); ";
      }
    }
  }
  return out.str();
}

std::string CalculatorHTML::GetProblemHeaderEnclosure() {
  std::stringstream out;
  out << "CommandEnclosure{}(";
  out <<  " SetRandomSeed{}(" << this->theProblemData.randomSeed << "); ";
  out << this->PrepareUserInputBoxes();
  out << "); ";
  return out.str();
}

std::string CalculatorHTML::GetProblemHeaderWithoutEnclosure() {
  std::stringstream out;
  out <<  " SetRandomSeed{}(" << this->theProblemData.randomSeed << "); ";
  out << this->PrepareUserInputBoxes();
  return out.str();
}

bool CalculatorHTML::PrepareCommandsGenerateProblem(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsGenerateProblem");
  (void) comments;
  std::stringstream streamCommands, streamCommandsNoEnclosures;
  streamCommandsNoEnclosures << this->GetProblemHeaderWithoutEnclosure();
  streamCommands << this->GetProblemHeaderEnclosure();//first calculator enclosure contains the header
  int numCommandsSoFar = 2;//two commands at the start: the opEndStatement command and
  // the first enclosure.
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration()) {
      continue;
    }
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
  debugStream << "<a href='"
  << HtmlRoutines::GetCalculatorComputationURL(this->theProblemData.commandsGenerateProblemNoEnclosures)
  << "'>"
  << "Input link </a>";
  this->theProblemData.commandsGenerateProblemLink = debugStream.str();
  return true;
}

bool CalculatorHTML::ParseHTMLPrepareCommands(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::ParseHTMLPrepareCommands");
  if (!this->ParseHTML(comments)) {
    return false;
  }
  return this->PrepareCommands(comments);
}

bool CalculatorHTML::PrepareCommands(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommands");
  if (!this->PrepareCommandsGenerateProblem(comments)) {
    return false;
  }
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++) {
    if (!this->PrepareCommandsAnswer(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->PrepareCommandsAnswerOnGiveUp(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->PrepareCommandsSolution(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->PrepareCommentsBeforeSubmission(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->PrepareCommentsBeforeInterpretation(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
  }
  return true;
}

bool CalculatorHTML::PrepareCommandsAnswerOnGiveUp(Answer& theAnswer, std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsAnswerOnGiveUp");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsAnswerOnGiveUp()) {
      continue;
    }
    if (currentElt.GetKeyValue("name") == theAnswer.answerId) {
      streamCommands << this->CleanUpCommandString(currentElt.content);
    }
  }
  theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly = streamCommands.str();
  return true;
}

bool CalculatorHTML::PrepareCommentsBeforeSubmission(
  Answer& theAnswer, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommentsBeforeSubmission");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsCommentBeforeSubmission()) {
      continue;
    }
    if (currentElt.GetKeyValue("name") != theAnswer.answerId) {
      continue;
    }
    streamCommands << this->CleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsCommentsBeforeSubmission = streamCommands.str();
  return true;
}

bool CalculatorHTML::PrepareCommentsBeforeInterpretation(
  Answer& theAnswer, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommentsBeforeInterpretation");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.IsCommentBeforeInterpretation()) {
      continue;
    }
    if (currentElt.GetKeyValue("name") != theAnswer.answerId) {
      continue;
    }
    streamCommands << this->CleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsCommentsBeforeInterpretatioN = streamCommands.str();
  return true;
}

bool CalculatorHTML::PrepareCommandsSolution(Answer& theAnswer, std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsSolution");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& solutionElt = this->theContent[i];
    if (!solutionElt.IsSolution()) {
      continue;
    }
    if (solutionElt.GetKeyValue("name") != theAnswer.answerId) {
      continue;
    }
    int numCommandsSoFar = 2;
    theAnswer.solutionElements = solutionElt.children;
    for (int j = 0; j < theAnswer.solutionElements.size; j ++) {
      SyntacticElementHTML& currentElt = theAnswer.solutionElements[j];
      if (!currentElt.IsCalculatorCommand() && !currentElt.IsCalculatorHidden()) {
        continue;
      }
      currentElt.commandIndex = numCommandsSoFar;
      numCommandsSoFar ++;
      streamCommands << "CommandEnclosure{}("
      << this->CleanUpCommandString(currentElt.content) << "); ";
    }
  }
  theAnswer.commandsSolutionOnly = streamCommands.str();
  return true;
}

bool CalculatorHTML::PrepareCommandsAnswer(Answer& theAnswer, std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommandsAnswer");
  std::stringstream streamCommandS;
  std::stringstream streamCommandsNoEnclosures;
  streamCommandS << this->GetProblemHeaderEnclosure();//first calculator enclosure contains the header
  streamCommandsNoEnclosures << this->GetProblemHeaderWithoutEnclosure();
  std::stringstream streamCommandsBody;
  std::stringstream streamCommandsBodyNoEnclosures;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (
      !currentElt.IsCalculatorHidden() && !currentElt.IsCalculatorCommand()
      && !currentElt.IsAnswer()
    ) {
      continue;
    }
    std::string commandCleaned = this->CleanUpCommandString(this->theContent[i].content);
    std::string commandEnclosed = "CommandEnclosure{}( " + commandCleaned + " );";
    if (currentElt.IsAnswer() && currentElt.GetKeyValue("id") == theAnswer.answerId) {
      std::string stringCommandsBody = streamCommandsBody.str();
      if (stringCommandsBody != "") {
        streamCommandS << "CommandEnclosure{}(" << stringCommandsBody << ");\n";
        streamCommandsNoEnclosures << streamCommandsBodyNoEnclosures.str();
      }
      theAnswer.commandsBeforeAnswer = streamCommandS.str();
      theAnswer.commandsBeforeAnswerNoEnclosuresForDEBUGGING = streamCommandsNoEnclosures.str();
      theAnswer.commandVerificationOnly = commandCleaned;
      return true;
    }
    if (this->theContent[i].IsCalculatorHidden() || this->theContent[i].IsCalculatorCommand()) {
      streamCommandsBody << commandEnclosed;
      streamCommandsBodyNoEnclosures << commandCleaned;
    }
  }
  if (comments != nullptr) {
    *comments << "<b>Something is wrong: did not find answer for answer tag: "
    << theAnswer.answerId << ". </b>";
  }
  return false;
}

bool CalculatorHTML::PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream* comments) {
  MacroRegisterFunctionWithName("Problem::PrepareAndExecuteCommands");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  this->PrepareCommands(comments);

  theInterpreter.initialize();
  theInterpreter.flagWriteLatexPlots = false;
  theInterpreter.flagPlotNoControls = true;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator init time");
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasProblemComposingRights()) {
    this->logCommandsProblemGeneratioN << "<b>Input commands:</b> "
    << this->theProblemData.commandsGenerateProblemLink
    << "<br>\n"
    << this->theProblemData.commandsGenerateProblem << "<br>";
  }
  theInterpreter.Evaluate(this->theProblemData.commandsGenerateProblem);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator evaluation time");
  //stOutput << "<hr>Fter eval: " << theInterpreter.outputString;
  bool result = !theInterpreter.flagAbortComputationASAP && theInterpreter.syntaxErrors == "";
  if (!result && comments != nullptr) {
    *comments << "<br>Failed to interpret your file. "
    << "<a href=\""
    << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString(this->theProblemData.commandsGenerateProblemNoEnclosures, false)
    << "\">Input link</a><br>"
    << "The interpretation input was:<br> "
    << this->theProblemData.commandsGenerateProblem << "<br>";
    if (theGlobalVariables.UserDefaultHasAdminRights()) {
      *comments << "The result of the interpretation attempt is:<br>"
      << theInterpreter.outputString << "<br><b>Comments</b><br>"
      << theInterpreter.outputCommentsString;
    } else {
      *comments << "This may be a bug with the problem. Feel free to take a screenshot of the issue and "
      << "email it to the site admin(s). ";
    }
  }
  for (int i = 0; i < theInterpreter.theObjectContainer.theUserInputTextBoxesWithValues.size(); i ++) {
    this->theProblemData.inputNonAnswerIds.AddOnTop(
      theInterpreter.theObjectContainer.theUserInputTextBoxesWithValues.theKeys[i]
    );
  }
  return result;
}

std::string SyntacticElementHTML::GetTagClass() {
  return this->GetKeyValue("class");
}

bool CalculatorHTML::PrepareSectionList(std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareSectionList");
  if (!theGlobalVariables.flagDatabaseCompiled) {
    commentsOnFailure << "Database not running. ";
    return false;
  }
  (void) commentsOnFailure;
  if (this->flagSectionsPrepared) {
    return true;
  }
  this->flagSectionsPrepared = true;
  if (
    this->currentUseR.sectionsTaught.size == 0 ||
    (this->currentUseR.userRole != "admin" && this->currentUseR.userRole != "teacher")
  ) {
    if (this->currentUseR.sectionComputed != "") {
      this->databaseStudentSections.AddOnTop(this->currentUseR.sectionComputed);
      return true;
    }
  }
  this->databaseStudentSections.AddListOnTop(this->currentUseR.sectionsTaught);
  return true;
}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentVieWOn()) {
    return;
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    inputOutput.interpretedCommand = "<b>Managing class not available (no database).</b>";
    return;
  }
  std::stringstream out;
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts\"> Manage accounts</a>";
  inputOutput.interpretedCommand = out.str();
}

bool CalculatorHTML::ComputeAnswerRelatedStrings(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::ComputeAnswerRelatedStrings");
  std::string desiredAnswerId = inputOutput.GetKeyValue("id");
  if (desiredAnswerId == "") {
    inputOutput.interpretedCommand = "<b>Error: could not generate submit button: "
    "the answer tag does not have a valid id. Please fix the problem template.</b>";
    return false;
  }
  int theIndex = this->GetAnswerIndex(desiredAnswerId);
  if (theIndex == - 1) {
    crash << "This is not supposed to happen: problem has syntactic element with answerId: "
    << desiredAnswerId << " but the answerId is missing from the list of known answer ids. "
    << this->theProblemData.ToStringAvailableAnswerIds() << crash;
  }
  Answer& currentA = this->theProblemData.theAnswers.theValues[theIndex];
  if (theIndex < this->answerHighlights.size) {
    currentA.htmlAnswerHighlight = this->answerHighlights[theIndex];
  } else {
    currentA.htmlAnswerHighlight = "";
  }
  std::string& answerId = currentA.answerId;
  currentA.idAnswerPanel = "spanAnswerPanel" + answerId;
  currentA.varAnswerId = answerId + "spanVariable";
  currentA.varMQfield = answerId + "MQspanVar";
  currentA.MQobject = "answerMathQuillObjects[" + std::to_string(this->NumAnswerIdsMathquilled) + "]";
  this->NumAnswerIdsMathquilled ++;
  currentA.MQUpdateFunction = answerId + "MQFieldUpdate";
  currentA.idVerificationSpan = "verification" + answerId;
  currentA.idSpanSolution = "solution" + answerId;
  currentA.idMQfielD = answerId + "MQSpanId";
  currentA.idMQFieldLocation = answerId + "MQSpanIdLocation";

  if (currentA.idMQButtonPanelLocation == "") {
    currentA.idMQButtonPanelLocation = answerId + "MQbuttonPanel";
  }
  std::stringstream previewAnswerStream;

  previewAnswerStream << "previewAnswers('" << answerId << "', '"
  << currentA.idVerificationSpan << "');";
  currentA.properties.Clear();
  for (int i = 0; i < inputOutput.properties.size(); i ++) {
    if (inputOutput.properties.theKeys[i] == "id") {
      continue;
    }
    currentA.properties.SetKeyValue(inputOutput.properties.theKeys[i], inputOutput.properties.theValues[i]);
  }

  currentA.javascriptPreviewAnswer = previewAnswerStream.str();
  currentA.idButtonSubmit = "buttonSubmit" + answerId;
  currentA.idButtonInterpret = "buttonInterpret" + answerId;
  currentA.idButtonAnswer = "buttonAnswer" + answerId;
  currentA.idButtonSolution = "buttonSolution" + answerId;
  if (!this->flagUseJSON) {
    inputOutput.defaultKeysIfMissing.AddOnTop("onkeyup");
    inputOutput.defaultValuesIfMissing.AddOnTop(
      currentA.javascriptPreviewAnswer + currentA.MQUpdateFunction + "();"
    );
  }
  std::stringstream verifyStream;
  int numCorrectSubmissions = currentA.numCorrectSubmissions;
  int numSubmissions = currentA.numSubmissions;
  if (
    theGlobalVariables.userCalculatorRequestType == "scoredQuiz" ||
    theGlobalVariables.userCalculatorRequestType == "scoredQuizJSON"
  ) {
    if (numCorrectSubmissions > 0) {
      verifyStream << "<b style =\"color:green\">Correctly answered: \\("
      << currentA.firstCorrectAnswerClean << "\\) </b> ";
      if (numSubmissions > 0) {
        verifyStream << "<br>Used: " << numSubmissions << " attempt(s) (" << numCorrectSubmissions << " correct).";
      }
    } else if (numSubmissions > 0) {
      verifyStream << numSubmissions << " attempt(s) so far. ";
    }
  }
  currentA.htmlSpanVerifyAnswer = verifyStream.str();
  return true;
}

void CalculatorHTML::InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateStudentAnswerButton");
  if (!this->ComputeAnswerRelatedStrings(inputOutput)) {
    return;
  }
  Answer& currentA = this->theProblemData.theAnswers.theValues[this->GetAnswerIndex(inputOutput.GetKeyValue("id"))];
  std::stringstream out;
  out << "<br><span class =\"panelAnswer\" id = \"" << currentA.idAnswerPanel << "\"></span>";
  inputOutput.interpretedCommand = out.str();
}

void CalculatorHTML::InterpretIfAnswer(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretIfAnswer");
  std::string tagClass = inputOutput.GetTagClass();
  if (tagClass != "calculatorAnswer") {
    return;
  }
  this->InterpretGenerateStudentAnswerButton(inputOutput);
}

void CalculatorHTML::InterpretNotByCalculatorNotAnswer(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretNotByCalculatorNotAnswer");
  std::string tagClass = inputOutput.GetTagClass();
  //std::string tag= inputOutput.tag;
  if (tagClass == "calculatorExamProblem" || tagClass == "calculatorExamIntermediate") {
    this->InterpretGenerateLink(inputOutput);
  } else if (tagClass == "calculatorManageClass") {
    this->InterpretManageClass(inputOutput);
  } else if (tagClass == "generateTopicTable") {
    this->InterpretTopicList(inputOutput);
  } else if (tagClass == "generateLectureMaterials") {
    this->InterpretLectureMaterials(inputOutput);
  } else if (tagClass == "generateTableOfContents") {
    this->InterpretTableOfContents(inputOutput);
  } else if (tagClass == "accountInformationLinks") {
    this->InterpretAccountInformationLinks(inputOutput);
  } else if (tagClass == "calculatorJavascript") {
    this->InterpretJavascripts(inputOutput);
  } else if (tagClass == "calculatorProblemNavigationHere") {
    this->InterpretProblemNavigationBar(inputOutput);
  } else if (tagClass == "calculatorEditPageHere") {
    this->InterpretEditPagePanel(inputOutput);
  }
}

std::string CalculatorHTML::CleanUpFileName(const std::string& inputLink) {
  MacroRegisterFunctionWithName("CalculatorHTML::CleanUpFileName");
  if (inputLink.size() == 0) {
    return inputLink;
  }
  unsigned firstMeaningfulChar = 0;
  for (; firstMeaningfulChar < inputLink.size(); firstMeaningfulChar ++) {
    if (
      inputLink[firstMeaningfulChar] != '\n' &&
      inputLink[firstMeaningfulChar] != '\r' &&
      inputLink[firstMeaningfulChar] != '\t' &&
      inputLink[firstMeaningfulChar] != ' '
    ) {
      break;
    }
  }
  unsigned lastMeaningfulChar = static_cast<unsigned>(inputLink.size()) - 1;
  for (; lastMeaningfulChar > firstMeaningfulChar; lastMeaningfulChar --) {
    if (
      inputLink[lastMeaningfulChar] != '\n' &&
      inputLink[lastMeaningfulChar] != '\r' &&
      inputLink[lastMeaningfulChar] != '\t' &&
      inputLink[lastMeaningfulChar] != ' '
    ) {
      break;
    }
  }
  if (firstMeaningfulChar >= inputLink.size()) {
    return "";
  }
  return inputLink.substr(firstMeaningfulChar, lastMeaningfulChar - firstMeaningfulChar + 1);
}

#include "crypto.h"

std::string CalculatorHTML::GetDeadlineNoInheritance(const std::string& id) {
  MacroRegisterFunctionWithName("CalculatorHTML::GetDeadlineNoInheritance");
  (void) id;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Database not present. ";
  }
  if (!this->currentUseR.theProblemData.Contains(id)) {
    return "";
  }
  ProblemDataAdministrative& currentProb = this->currentUseR.theProblemData.GetValueCreateNoInit((id)).adminData;
  if (!currentProb.deadlinesPerSection.Contains(this->currentUseR.sectionComputed)) {
    return "";
  }
  return currentProb.deadlinesPerSection.GetValueCreate(this->currentUseR.sectionComputed);
}

std::string CalculatorHTML::GetDeadline(
  const std::string& problemName, const std::string& sectionNumber, bool& outputIsInherited
) {
  MacroRegisterFunctionWithName("CalculatorHTML::GetDeadline");
  (void) problemName;
  (void) sectionNumber;
  (void) outputIsInherited;
  outputIsInherited = true;
  std::string result;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "While getting deadline: database not compiled";
  }
  int topicIndex = this->theTopicS.GetIndex(problemName);
  if (topicIndex == - 1) {
    return problemName + " not found in topic list. ";
  }
  TopicElement& currentTopic = this->theTopicS.GetValueCreate(problemName);
  for (int i = currentTopic.parentTopics.size - 1; i >= 0; i --) {
    const std::string& containerName = this->theTopicS.theKeys[currentTopic.parentTopics[i]];
    if (this->currentUseR.theProblemData.Contains(containerName)) {
      ProblemDataAdministrative& currentProb =
      this->currentUseR.theProblemData.GetValueCreateNoInit(containerName).adminData;
      result = currentProb.deadlinesPerSection.GetValueCreate(sectionNumber);
      if (StringRoutines::StringTrimWhiteSpace(result) != "") {
        outputIsInherited = (containerName != problemName);
        return result;
      }
    }
  }
  return result;
}

std::string CalculatorHTML::ToStringOnEDeadlineFormatted(
  const std::string& topicID,
  const std::string& sectionNumber,
  bool problemAlreadySolved,
  bool returnEmptyStringIfNoDeadline,
  bool isSection
) {
  std::stringstream out;
  (void) problemAlreadySolved;
  (void) isSection;
  bool deadlineIsInherited = false;
  std::string currentDeadline = this->GetDeadline(topicID, sectionNumber, deadlineIsInherited);
  if (currentDeadline == "") {
    if (returnEmptyStringIfNoDeadline) {
      return "";
    }
    out << "<span style =\"color:orange\">No deadline yet</span>";
    return out.str();
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    out  << "Database not running: no deadlines";
    return out.str();
  }
  TimeWrapper now, deadline; //<-needs a fix for different time formats.
  //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
  std::stringstream badDateStream;
  if (!deadline.AssignMonthDayYear(currentDeadline, badDateStream)) {
    out << "<span style =\"color:red\">" << badDateStream.str() << "</span>";
  }
  //  out << "deadline.date: " << deadline.theTime.tm_mday;
  now.AssignLocalTime();
  //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
  //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
  double secondsTillDeadline = deadline.SubtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;

  std::stringstream hoursTillDeadlineStream;
  bool deadlineIsNear = secondsTillDeadline < 24 * 3600 && !problemAlreadySolved && !isSection;
  bool deadlineHasPassed = (secondsTillDeadline < 0);
  if (deadlineIsInherited && !theGlobalVariables.UserStudentVieWOn()) {
    out << "Inherited: ";
  } else if (deadlineIsInherited && isSection && returnEmptyStringIfNoDeadline) {
    return "";
  } else {
    out << "Deadline: ";
  }
  if (!deadlineHasPassed) {
    if (deadlineIsNear) {
      hoursTillDeadlineStream << "<span style =\"color:red\">"
      << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false, true) << "</span>";
    } else {
      hoursTillDeadlineStream << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false, true)
      << " left. ";
    }
  } else {
    hoursTillDeadlineStream << "[passed].";
  }
  if (deadlineHasPassed && !problemAlreadySolved) {
    out << "<span style =\"color:blue\">" << currentDeadline << "</span> ";
    out << "<b style =\"red\">[passed].</b>";
  } else {
    if (problemAlreadySolved && !isSection) {
      out << "<span style =\"color:green\">" << currentDeadline << "</span> ";
    } else if (deadlineIsNear && !isSection) {
      out << "<span style =\"color:red\">" << currentDeadline << "</span> ";
    } else {
      out << "<span style =\"color:brown\">" << currentDeadline << "</span> ";
    }
    out << hoursTillDeadlineStream.str();
  }
  return
  //"[<span style =\"color:green\"><b>disabled</b> </span>] "+
  out.str();
}

std::string CalculatorHTML::ToStringAllSectionDeadlines(const std::string& topicID, bool isSection) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringAllSectionDeadlines");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "";
  std::stringstream out;
  out << "<table>";
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    if (this->databaseStudentSections[i] == "") {
      continue;
    }
    out << "<tr><td>Section " << this->databaseStudentSections[i] << ":</td>";
    out << "<td>"
    << this->ToStringOnEDeadlineFormatted(topicID, this->databaseStudentSections[i], false, false, isSection)
    << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string CalculatorHTML::ToStringDeadline(
  const std::string& topicID, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineWithModifyButton");
  (void) topicID;
  (void) problemAlreadySolved;
  (void) returnEmptyStringIfNoDeadline;
  (void) isSection;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Database not available";
  }
  if (theGlobalVariables.UserGuestMode()) {
    return "deadlines require login";
  } else if (
    theGlobalVariables.UserDefaultHasAdminRights() &&
    theGlobalVariables.UserStudentVieWOn()
  ) {
    std::string sectionNum = HtmlRoutines::ConvertURLStringToNormal(
      theGlobalVariables.GetWebInput("studentSection"), false
    );
    return this->ToStringOnEDeadlineFormatted(
      topicID, sectionNum, problemAlreadySolved, returnEmptyStringIfNoDeadline, isSection
    );
  } else {
    return this->ToStringOnEDeadlineFormatted(
      topicID,
      this->currentUseR.sectionComputed,
      problemAlreadySolved,
      returnEmptyStringIfNoDeadline,
      isSection
    );
  }
  // return "";
}

void CalculatorHTML::ComputeDeadlinesAllSections(TopicElement& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::ComputeDeadlinesAllSections");
  inputOutput.deadlinesPerSectioN.initializeFillInObject(this->databaseStudentSections.size, "");
  inputOutput.deadlinesAreInherited.initializeFillInObject(this->databaseStudentSections.size, false);
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    inputOutput.deadlinesPerSectioN[i] = this->GetDeadline(
      inputOutput.id, this->databaseStudentSections[i], inputOutput.deadlinesAreInherited[i]
    );
    if (inputOutput.deadlinesAreInherited[i]) {
      inputOutput.deadlinesPerSectioN[i] = "";
    }
  }
}

void CalculatorHTML::ComputeDeadlinesAllSectionsNoInheritance(TopicElement& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::ComputeDeadlinesAllSectionsNoInheritance");
  inputOutput.deadlinesPerSectioN.initializeFillInObject(this->databaseStudentSections.size, "");
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    ProblemDataAdministrative& currentProb =
    this->currentUseR.theProblemData.GetValueCreateNoInit(inputOutput.id).adminData;
    inputOutput.deadlinesPerSectioN[i] =
    currentProb.deadlinesPerSection.GetValueCreate(this->databaseStudentSections[i]);
  }
}

void CalculatorHTML::ComputeDeadlineModifyButton(
  TopicElement& inputOutput, bool problemAlreadySolved, bool isProblemGroup
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineModifyButton");
  if (!theGlobalVariables.UserDefaultHasProblemComposingRights()) {
    return;
  }
  this->ComputeDeadlinesAllSections(inputOutput);
  (void) problemAlreadySolved;
  std::stringstream out;
  std::stringstream deadlineStream;
  inputOutput.idDeadlineTable = "deadlineTable" + Crypto::computeSha3_256OutputBase64URL(inputOutput.id);
  inputOutput.idDeadlineButton = "deadlineButton" + Crypto::computeSha3_256OutputBase64URL(inputOutput.id);
  deadlineStream << "<table class =\"deadlineTable\" id =\""
  << inputOutput.idDeadlineTable
  << "\">";
  deadlineStream << "<tr><th>Grp.</th><th>Deadline</th></tr>";
  inputOutput.idsDeadlines.SetSize(this->databaseStudentSections.size);
  inputOutput.deadlinesPerSectionFormatted.initializeFillInObject(this->databaseStudentSections.size, "");
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    std::string& currentDeadlineId = inputOutput.idsDeadlines[i];
    if (this->databaseStudentSections[i] == "") {
      continue;
    }
    currentDeadlineId = "deadline" + Crypto::ConvertStringToBase64URL(
      this->databaseStudentSections[i] + inputOutput.id
    );
    if (currentDeadlineId[currentDeadlineId.size() - 1] == '=') {
      currentDeadlineId.resize(currentDeadlineId.size() - 1);
    }
    if (currentDeadlineId[currentDeadlineId.size() - 1] == '=') {
      currentDeadlineId.resize(currentDeadlineId.size() - 1);
    }
    inputOutput.deadlinesAreInherited[i] = false;
    deadlineStream << "<tr>";
    deadlineStream << "<td>" << this->databaseStudentSections[i] << "</td>";
    inputOutput.deadlinesPerSectionFormatted[i] =
    this->ToStringOnEDeadlineFormatted(
      inputOutput.id, this->databaseStudentSections[i], problemAlreadySolved, false, isProblemGroup
    );
    deadlineStream
    << "<td> <input class =\"modifyDeadlineInput\" type =\"text\" id =\""
    << currentDeadlineId << "\" value =\"";
    if (!inputOutput.deadlinesAreInherited[i]) {
      deadlineStream << inputOutput.deadlinesPerSectioN[i];
    }
    deadlineStream << "\"> " ;
    deadlineStream << "</td>";
    deadlineStream << "</tr>";
  }
  deadlineStream << "<tr><td>\n";
  inputOutput.idDeadlineReport = "deadlineReport" + Crypto::computeSha3_256OutputBase64URL(inputOutput.id);

  if (inputOutput.idDeadlineReport[inputOutput.idDeadlineReport.size() - 1] == '=') {
    inputOutput.idDeadlineReport.resize(inputOutput.idDeadlineReport.size() - 1);
  }
  if (inputOutput.idDeadlineReport[inputOutput.idDeadlineReport.size() - 1] == '=') {
    inputOutput.idDeadlineReport.resize(inputOutput.idDeadlineReport.size() - 1);
  }
  deadlineStream << "<button onclick=\"";
  deadlineStream << "submitStringAsMainInput('"
  << HtmlRoutines::ConvertStringToURLString(inputOutput.id, false)
  << "='+encodeURIComponent('deadlines ='+encodeURIComponent(";
  bool isFirst = true;
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    if (this->databaseStudentSections[i] == "") {
      continue;
    }
    if (!isFirst) {
      deadlineStream << "+";
    }
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
  if (!isProblemGroup) {
    deadlineStream << "<tr><td colspan =\"2\">(overrides section deadline).</td></tr> ";
  } else {
    deadlineStream << "<tr><td colspan =\"2\">(overriden by per-problem-deadline).</td></tr>";
  }
  deadlineStream << "</table>";
  out << "<button class =\"accordion\" id =\""
  << inputOutput.idDeadlineButton
  << "\">"
  << inputOutput.displayDeadlinE << "</button>";
  out << "<span class =\"panel\">";
  out << deadlineStream.str();
  out << "</span>";
  inputOutput.displayDeadlinE = out.str();
}

std::string CalculatorHTML::ToStringInterprettedCommands(Calculator &theInterpreter, List<SyntacticElementHTML>& theElements) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringInterprettedCommands");
  std::stringstream out;
  out << "<table>";
  int commandCounter = theInterpreter.theProgramExpression.size() - 1;
  for (int eltCounter = theElements.size - 1; eltCounter > 0; eltCounter --) {
    SyntacticElementHTML& currentElt = theElements[eltCounter];
    std::string currentEltString = currentElt.GetTagClass() + "[" + currentElt.content.substr(0, 10) + "...]";
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration()) {
      out << "<tr><td>" << currentEltString << "</td>"
      << "<td>"
      << theInterpreter.theProgramExpression[commandCounter].ToString()
      << "</td></tr>";
      commandCounter --;
      continue;
    }
    for (; commandCounter > 1; commandCounter --) {
      std::string currentString= theInterpreter.theProgramExpression[commandCounter].ToString();
      out << "<tr><td>" << currentEltString << "</td><td>"
      << currentString << "</td></tr>";
      if (currentString == "SeparatorBetweenSpans") {
        break;
      }
    }
  }
  out << "</table>";
  return out.str();
}

bool CalculatorHTML::InterpretProcessExecutedCommands(
  Calculator &theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream& comments
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ProcessInterprettedCommands");
  (void) comments;
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagMakingExpressionTableWithLatex = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theFormat.flagUseQuotes = false;
  theFormat.flagUseLatex = true;
  bool result = true;
  theInterpreter.theObjectContainer.resetPlots();
  for (int i = 0; i < theElements.size; i ++) {
    SyntacticElementHTML& currentElt = theElements[i];
    if (!currentElt.IsInterpretedByCalculatorDuringProblemGeneration()) {
      currentElt.interpretedCommand = "";
      continue;
    }
    if (
      currentElt.commandIndex >= theInterpreter.theProgramExpression.size() ||
      currentElt.commandIndex < 0
    ) {
      std::stringstream errorStream;
      errorStream << "<b>This is a programming error: syntactic element "
      << currentElt.ToStringDebug() << " has wrongly computed commandIndex: "
      << currentElt.commandIndex << ". "
      << "Please report this error to the website admins. </b>";
      currentElt.interpretedCommand = errorStream.str();
      result = false;
      continue;
    }
    if (!theInterpreter.theProgramExpression[currentElt.commandIndex].StartsWith(theInterpreter.opCommandEnclosure())) {
      crash << "Element: " << theInterpreter.theProgramExpression[currentElt.commandIndex].ToString()
      << " in " << theInterpreter.theProgramExpression.ToString()
      << " is supposed to be a command enclosure but apparently isn't. " << crash;
    }
    Expression currentExpr = theInterpreter.theProgramExpression[currentElt.commandIndex][1];
    if (currentExpr.StartsWith(theInterpreter.opEndStatement()) && currentExpr.size() == 2) {
      currentExpr = currentExpr[1];
    }
    if (currentExpr.StartsWith(theInterpreter.opEndStatement())) {
      currentElt.flagUseMathMode = false;
    }
    theFormat.flagUseQuotes = false;
    theFormat.flagMakingExpressionTableWithLatex = true;
    currentElt.interpretedCommand = currentExpr.ToString(&theFormat);
    currentElt.flagUseDisplaystyleInMathMode = (currentElt.content.find("\\displaystyle") != std::string::npos);
    currentElt.flagUseMathMode = true;
    currentElt.flagUseMathSpan = false;
    if (
      currentExpr.IsOfType<std::string> () ||
      currentExpr.IsOfType<Plot>() ||
      currentElt.GetKeyValue("noTags") == "true"
    ) {
      currentElt.flagUseMathMode = false;
    }
  }
  return result;
}

void CalculatorHTML::LogProblemGenerationObsolete(Calculator &theInterpreter) {
  if (!theGlobalVariables.UserDebugFlagOn() || !theGlobalVariables.UserDefaultHasProblemComposingRights()) {
    return;
  }
  std::stringstream streamLog;
  streamLog << "<table border ='1'>";
  for (int i = 0; i < theInterpreter.theProgramExpression.size(); i ++) {
    streamLog << "<tr>";
    for (int j = 0; j < this->theContent.size; j ++) {
      if (this->theContent[j].commandIndex == i) {
        streamLog << "<td>" << this->theContent[j].ToStringDebug() << "</td>";
      }
    }
    streamLog << "<td>" << theInterpreter.theProgramExpression[i].ToString()
    << "</td></tr>";
  }
  streamLog << "</table>";
  this->logCommandsProblemGeneratioN << streamLog.str();
}

void CalculatorHTML::FigureOutCurrentProblemList(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::FigureOutCurrentProblemList");
  if (this->flagParentInvestigated) {
    return;
  }
  this->flagParentInvestigated = true;
  this->topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
  this->LoadAndParseTopicList(comments);
}

bool CalculatorHTML::InterpretHtml(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  if (!this->ParseHTML(comments)) {
    this->outputHtmlBodyNoTag = "<b>Failed to interpret html input. </b><br>" + this->ToStringContent();
    this->timeToParseHtml = theGlobalVariables.GetElapsedSeconds() - startTime;
    return false;
  }
  this->timeToParseHtml = theGlobalVariables.GetElapsedSeconds() - startTime;
  this->MaxInterpretationAttempts = 25;
  this->randomSeedsIfInterpretationFails.SetSize(this->MaxInterpretationAttempts);
  if (!this->theProblemData.flagRandomSeedGiven) {
    srand(1003 + static_cast<unsigned>(time(nullptr)));
    this->randomSeedsIfInterpretationFails[0] = (103 + rand()) % 100000000;
  } else {
    this->randomSeedsIfInterpretationFails[0] = static_cast<int>(this->theProblemData.randomSeed);
  }
  srand(static_cast<unsigned>(this->randomSeedsIfInterpretationFails[0]));
  for (int i = 1; i < this->randomSeedsIfInterpretationFails.size; i ++) {
    this->randomSeedsIfInterpretationFails[i] = (103 + rand()) % 100000000;
  }
  this->timePerAttempt.SetSize(0);
  this->timeIntermediatePerAttempt.SetSize(0);
  this->timeIntermediateComments.SetSize(0);
  this->NumAttemptsToInterpret = 0;
  while (this->NumAttemptsToInterpret < this->MaxInterpretationAttempts) {
    startTime = theGlobalVariables.GetElapsedSeconds();
    this->timeIntermediatePerAttempt.SetSize(this->timeIntermediatePerAttempt.size + 1);
    this->timeIntermediatePerAttempt.LastObject()->SetSize(0);
    this->timeIntermediateComments.SetSize(this->timeIntermediateComments.size + 1);
    this->timeIntermediateComments.LastObject()->SetSize(0);
    Calculator theInterpreter;
    this->NumAttemptsToInterpret ++;
    std::stringstream commentsOnLastFailure;
    if (this->InterpretHtmlOneAttempt(theInterpreter, commentsOnLastFailure)) {
      this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
      this->theProblemData.CheckConsistency();
      return true;
    }
    this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
    if (this->NumAttemptsToInterpret >= this->MaxInterpretationAttempts && comments != nullptr) {
      *comments << commentsOnLastFailure.str();
    }
  }
  if (comments != nullptr) {
    *comments << "<hr>Failed to evaluate the commands: " << this->NumAttemptsToInterpret
    << " attempts made. ";
  }
  if (this->flagIsForReal) {
    this->StoreRandomSeedCurrent(comments);
    if (comments != nullptr) {
      *comments << "<b>Your random seed has been reset due to a finicky problem generation. </b>";
    }
  }
  this->theProblemData.CheckConsistency();
  return false;
}

bool CalculatorHTML::IsSplittingChar(const std::string& input) {
  if (input.size() != 1) {
    return false;
  }
  return this->splittingChars.Contains(input[0]);
}

int SyntacticElementHTML::ParsingNumDummyElements = 8;
std::string CalculatorHTML::ToStringParsingStack(List<SyntacticElementHTML>& theStack) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringParsingStack");
  std::stringstream out;
  out << "#Non-dummy elts: " << theStack.size - SyntacticElementHTML::ParsingNumDummyElements << ". ";
  for (int i = SyntacticElementHTML::ParsingNumDummyElements; i < theStack.size; i ++) {
    out << "<span style =\"color:" << ((i % 2 == 0) ? "orange" : "blue") << "\">";
    std::string theContent = theStack[i].ToStringDebug();
    if (theContent.size() == 0) {
      theContent = "<b>empty</b>";
    } else if (theContent == " ") {
      theContent = "_";
    }
    out << theContent << "</span>";
  }
  return out.str();
}

int CalculatorHTML::GetAnswerIndex(const std::string& desiredAnswerId) {
  return this->theProblemData.theAnswers.GetIndex(desiredAnswerId);
}

bool CalculatorHTML::CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right) {
  MacroRegisterFunctionWithName("SyntacticElementHTML::CanBeMerged");
  if (left.syntacticRole != "" || right.syntacticRole != "") {
    return false;
  }
  if (this->IsSplittingChar(left.content) && left.content != " ") {
    return false;
  }
  if (this->IsSplittingChar(right.content) && right.content != " ") {
    return false;
  }
  return true;
}

bool CalculatorHTML::SetTagClassFromCloseTag(SyntacticElementHTML& output) {
  MacroRegisterFunctionWithName("CalculatorHTML::SetTagClassFromCloseTag");
  std::string& lastTag = output.tag;
  if (lastTag == "head" || lastTag == "HEAD" || lastTag == "Head") {
    output.SetKeyValue("class", "headFinish");
    this->flagTagHeadPresent = true;
    return true;
  } else if (lastTag == "body" || lastTag == "BODY" || lastTag == "Body") {
    output.SetKeyValue("class", "bodyFinish");
    this->flagTagBodyPresent = true;
    return true;
  } else if (lastTag == "html" || lastTag == "HTML" || lastTag == "html") {
    output.SetKeyValue("class", "htmlFinish");
    this->flagTagHtmlPresent = true;
    return true;
  }
  return false;
}

bool CalculatorHTML::SetTagClassFromOpenTag(SyntacticElementHTML& output) {
  MacroRegisterFunctionWithName("CalculatorHTML::SetTagClassFromOpenTag");
  std::string& lastTag = output.tag;
  if (lastTag == "head" || lastTag == "HEAD" || lastTag == "Head") {
    output.SetKeyValue("class", "headStart");
    this->flagTagHeadPresent = true;
    return true;
  } else if (lastTag == "body" || lastTag == "BODY" || lastTag == "Body") {
    output.SetKeyValue("class", "bodyStart");
    this->flagTagBodyPresent = true;
    return true;
  } else if (lastTag == "html" || lastTag == "HTML" || lastTag == "html") {
    output.SetKeyValue("class", "htmlStart");
    this->flagTagHtmlPresent = true;
    return true;
  } else if (this->calculatorTagsRecordedLiterally.Contains(lastTag)) {
    output.SetKeyValue("class", lastTag);
    return true;
  }
  return false;
}

void CalculatorHTML::initTopicElementNames() {
  MacroRegisterFunctionWithName("CalculatorHTML::initTopicElementNames");
  if (this->calculatorTopicElementNames.size == 0) {
    this->calculatorTopicElementNames.AddOnTop("Chapter");
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

void CalculatorHTML::initAutocompleteExtras() {
  MacroRegisterFunctionWithName("CalculatorHTML::initAutocompleteExtras");
  if (this->autoCompleteExtras.size > 0) {
    return;
  }
  this->autoCompleteExtras.AddOnTop("answerCalculatorHighlight");
  this->autoCompleteExtras.AddOnTop("algebra");
  this->autoCompleteExtras.AddOnTop("logarithms");
  this->autoCompleteExtras.AddOnTop("buttons");
  this->autoCompleteExtras.AddOnTop("displaystyle");
}

void CalculatorHTML::initBuiltInSpanClasses() {
  MacroRegisterFunctionWithName("CalculatorHTML::initBuiltInSpanClasses");
  if (this->calculatorTagsRecordedLiterally.size == 0) {
    this->calculatorTagsRecordedLiterally.AddOnTopNoRepetition("answerCalculatorHighlight");
  }
  if (this->calculatorClassesAnswerFields.size == 0) {
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonSubmit");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonInterpret");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonGiveUp");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorButtonSolution");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorMQField");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorMQButtonPanel");
    this->calculatorClassesAnswerFields.AddOnTop("calculatorAnswerVerification");
  }
  if (this->calculatorClasses.size == 0) {
    this->calculatorClasses.AddOnTop("calculator");
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

bool CalculatorHTML::ParseHTML(std::stringstream* comments) {
  MacroRegisterFunctionWithName("Problem::ParseHTML");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  std::string word;
  char currentChar;
  List<SyntacticElementHTML> theElements;
  theElements.SetSize(0);
  theElements.SetExpectedSize(static_cast<int>(theReader.str().size()) / 4);
  this->splittingChars.AddOnTop('<');
  this->splittingChars.AddOnTop('\"');
  this->splittingChars.AddOnTop('>');
  this->splittingChars.AddOnTop('=');
  this->splittingChars.AddOnTop('/');
  this->splittingChars.AddOnTop(' ');
  while (theReader.get(currentChar)) {
    if (splittingChars.Contains(currentChar)) {
      if (word != "") {
        theElements.AddOnTop(word);
      }
      std::string charToString;
      charToString.push_back(currentChar);
      theElements.AddOnTop(charToString);
      word = "";
    } else {
      word.push_back(currentChar);
    }
  }
  if (word != "") {
    theElements.AddOnTop(word);
  }
  this->initBuiltInSpanClasses();
  this->eltsStack.SetSize(0);
  SyntacticElementHTML dummyElt, tempElt;
  dummyElt.content = "<>";
  dummyElt.syntacticRole = "filler";
  tempElt.syntacticRole = "command";
  tempElt.tag = "";
  tempElt.content = "";
  eltsStack.SetExpectedSize(theElements.size + SyntacticElementHTML::ParsingNumDummyElements);
  for (int i = 0; i < SyntacticElementHTML::ParsingNumDummyElements; i ++) {
    eltsStack.AddOnTop(dummyElt);
  }
  int indexInElts = - 1;
  bool reduced = false;
  this->flagIsExamProblem = false;
  this->flagIsExamHome = false;
  this->flagTagHeadPresent = false;
  this->flagTagBodyPresent = false;
  this->flagTagHtmlPresent = false;
  std::string tagClass, tag;
  do {
    if (!reduced) {
      indexInElts ++;
      if (indexInElts < theElements.size) {
        eltsStack.AddOnTop(theElements[indexInElts]);
      }
    }
    reduced = true;
    int sizeMinus1 = eltsStack.size - 1;
    //<- needed to avoid compiler warning:
    //"warning: assuming signed overflow does not occur when assuming that (X + c) < X is always ..."
    int sizeMinus2 = eltsStack.size - 2;
    int sizeMinus3 = eltsStack.size - 3;
    int sizeMinus4 = eltsStack.size - 4;
    int sizeMinus5 = eltsStack.size - 5;
    int sizeMinus6 = eltsStack.size - 6;
    SyntacticElementHTML& last         = eltsStack[sizeMinus1]; //<- needed to avoid compiler warning
    SyntacticElementHTML& secondToLast = eltsStack[sizeMinus2];
    SyntacticElementHTML& thirdToLast  = eltsStack[sizeMinus3];
    SyntacticElementHTML& fourthToLast = eltsStack[sizeMinus4];
    SyntacticElementHTML& fifthToLast  = eltsStack[sizeMinus5];
    SyntacticElementHTML& sixthToLast  = eltsStack[sizeMinus6];
    //SyntacticElementHTML& seventhToLast = eltsStack[eltsStack.size-7];
    if (
      (secondToLast.syntacticRole == "<openTagCalc>" || secondToLast.syntacticRole == "<calculatorSolution>") &&
      last.syntacticRole == "</closeTag>" &&
      secondToLast.tag == last.tag
    ) {
      secondToLast.syntacticRole = "command";
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(secondToLast)) {
        eltsStack.RemoveLastObject();
      }
      continue;
    }
    if (
      thirdToLast.syntacticRole == "<openTag" &&
      secondToLast == "/" && last.syntacticRole == ">"
    ) {
      tagClass = thirdToLast.GetKeyValue("class");
      if (tagClass == "calculatorSolution") {
        thirdToLast.syntacticRole = "<calculatorSolution>";
      } else if (this->calculatorClasses.Contains(tagClass)) {
        thirdToLast.syntacticRole = "command";
      } else {
        thirdToLast.content = thirdToLast.ToStringOpenTag("", true);
        thirdToLast.resetAllExceptContent();
      }
      eltsStack.RemoveLastObject();
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(thirdToLast)) {
        eltsStack.RemoveLastObject();
      }
      continue;
    }
    if (last.syntacticRole == "</closeTag>" && this->calculatorTagsRecordedLiterally.Contains(last.tag)) {
      last.content = last.ToStringCloseTag("");
      last.syntacticRole = "command";
      last.tag += "Finish";
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (last.syntacticRole == "</closeTag>") {
      last.content = last.ToStringCloseTag("");
      last.resetAllExceptContent();
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTagCalc>" && secondToLast == "<" && last == "/") {
      secondToLast.syntacticRole = "</";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTagCalc>" && secondToLast.syntacticRole == "") {
      thirdToLast.content += secondToLast.content;
      secondToLast = last;
      eltsStack.SetSize(eltsStack.size - 1);
      continue;
    }
    if (secondToLast.syntacticRole != "<openTagCalc>" && last == "<") {
      last.content = "";
      last.syntacticRole = "<";
      continue;
    }
    if (secondToLast.syntacticRole != "<openTagCalc>" && last == ">") {
      last.content = "";
      last.syntacticRole = ">";
      continue;
    }
    if (secondToLast.syntacticRole == "<" && last != "/") {
      secondToLast.tag = last.content;
      secondToLast.syntacticRole = "<openTag";
      secondToLast.content = "";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "" && secondToLast != "\"" && last == "/") {
      secondToLast.content += last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "<" && last == "/") {
      secondToLast.syntacticRole = "</";
      secondToLast.tag = "";
      secondToLast.content = "";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "</" ) {
      secondToLast.syntacticRole = "</closeTag";
      secondToLast.tag = last.content;
      secondToLast.content = "";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "</closeTag" && last.syntacticRole == ">") {
      secondToLast.syntacticRole = "</closeTag>";
      if (this->SetTagClassFromCloseTag(secondToLast)) {
        secondToLast.syntacticRole = "command";
      }
      eltsStack.RemoveLastObject();
      continue;
    }
    if (last == " " &&(
      secondToLast.syntacticRole == "<openTag" ||
      thirdToLast.syntacticRole == "<openTag" ||
      fourthToLast.syntacticRole == "<openTag"
    )) {
      eltsStack.RemoveLastObject();
      continue;
    }
    if (
      thirdToLast.syntacticRole == "<openTag" &&
      secondToLast.syntacticRole == "" &&
      last.syntacticRole == "" &&
      secondToLast != "=" &&
      secondToLast != "\"" &&
      last != "=" &&
      last != "\""
    ) {
      thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size - 2] = *eltsStack.LastObject();
      eltsStack.RemoveLastObject();
      continue;
    }
    if (this->CanBeMerged(secondToLast, last)) {
      secondToLast.content += last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<calculatorSolution>" &&(
      secondToLast.syntacticRole == "" ||
      secondToLast.syntacticRole == "command" ||
      secondToLast.syntacticRole == "<"
    )) {
      thirdToLast.children.AddOnTop(secondToLast);
      eltsStack[eltsStack.size - 2] = last;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (
      sixthToLast.syntacticRole == "<openTag" &&
      fourthToLast == "=" &&
      thirdToLast == "\"" &&
      last != "\""
    ) {
      if (last.syntacticRole != "" && last.content == "") {
        secondToLast.content += last.syntacticRole;
      } else {
        secondToLast.content += last.content;
      }
      eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast == "\"" && secondToLast != "\"" && last != "\"") {
      if (secondToLast.syntacticRole != "" && secondToLast.content == "") {
        secondToLast.content = secondToLast.syntacticRole;
      }
      if (last.syntacticRole != "" && last.content == "") {
        last.content = last.syntacticRole;
      }
      secondToLast.content += last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (
      sixthToLast.syntacticRole == "<openTag" &&
      fourthToLast == "=" &&
      thirdToLast == "\"" &&
      last == "\""
    ) {
      sixthToLast.SetKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.SetSize(eltsStack.size - 5);
      continue;
    }
    if (
      fifthToLast.syntacticRole == "<openTag" && thirdToLast == "=" && secondToLast == "\"" && last == "\""
    ) {
      fifthToLast.SetKeyValue(fourthToLast.content, "");
      eltsStack.SetSize(eltsStack.size - 4);
      continue;
    }
    if (thirdToLast == "\"" && secondToLast != "\"" && last == "\"") {
      thirdToLast.content += secondToLast.content;
      thirdToLast.content += last.content;
      eltsStack.SetSize(eltsStack.size - 2);
      continue;
    }
    if (
      thirdToLast.syntacticRole == "<openTag" && secondToLast.syntacticRole == "" && last.syntacticRole == ">"
    ) {
      thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size - 2] = *eltsStack.LastObject();
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "<openTag" && last.syntacticRole == ">") {
      tagClass = secondToLast.GetKeyValue("class");
      tag = secondToLast.tag;
      if (tagClass == "calculatorSolution" || tag == "calculatorSolution") {
        secondToLast.syntacticRole = "<calculatorSolution>";
      } else if (this->calculatorClasses.Contains(tagClass) || this->calculatorClasses.Contains(tag)) {
        secondToLast.syntacticRole = "<openTagCalc>";
        if (this->calculatorClasses.Contains(tag)) {
          tagClass = tag;
          secondToLast.SetKeyValue("class", tagClass);
        }
      } else if (this->calculatorTagsRecordedLiterally.Contains(secondToLast.tag)) {
        secondToLast.syntacticRole = "command";
        secondToLast.tag += "Start";
      } else if (this->SetTagClassFromOpenTag(secondToLast)) {
        secondToLast.syntacticRole = "command";
      } else {
        secondToLast.content =secondToLast.ToStringOpenTag("");
        if (theGlobalVariables.UserDefaultHasProblemComposingRights() && comments != nullptr) {
          if (StringRoutines::StringBeginsWith(tagClass, "calculator")) {
            if (!this->calculatorClasses.Contains(tagClass)) {
              *comments
              << "<hr><b style =\"color:red\">Warning: found class tag: "
              << tagClass
              << ". The name of this class starts with calculator yet this is not "
              << "a calculator class. This may be a typo. "
              << "</b>";
            }
          }
        }
        secondToLast.resetAllExceptContent();
      }
      //stOutput << "<hr>Rule 3: processed: " << secondToLast.ToStringDebug() << "<hr>";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "" && last.syntacticRole == ">") {
      secondToLast.content += ">";
      eltsStack.RemoveLastObject();
      continue;
    }
    reduced = false;
  } while (reduced || indexInElts < theElements.size);
  this->theContent.SetSize(0);
  bool result = true;
  for (int i = SyntacticElementHTML::ParsingNumDummyElements; i < eltsStack.size; i ++) {
    bool needNewTag = false;
    if (i == SyntacticElementHTML::ParsingNumDummyElements) {
      needNewTag = true;
    } else if (this->theContent.LastObject()->syntacticRole != "") {
      needNewTag = true;
    }
    if (eltsStack[i].syntacticRole != "") {
      needNewTag = true;
    }
    if (eltsStack[i].GetTagClass() == "calculatorAnswer") {
      if (eltsStack[i].GetKeyValue("mqMatrices") == "true") {
        this->flagMathQuillWithMatrices = true;
      }
    }
    if (eltsStack[i].syntacticRole != "command" && eltsStack[i].syntacticRole != "") {
      result = false;
      if (comments != nullptr) {
        *comments << "<br>Syntactic element: " << eltsStack[i].ToStringDebug()
        << " is not a command but has non-empty syntactic role.";
      }
    }
    if (!needNewTag) {
      this->theContent.LastObject()->content += eltsStack[i].content;
    } else {
      if (this->theContent.size > 0) {
        if (
          this->theContent.LastObject()->IsInterpretedByCalculatorDuringProblemGeneration() &&
          eltsStack[i].IsInterpretedByCalculatorDuringProblemGeneration()
        ) {
          SyntacticElementHTML emptyElt;
          this->theContent.AddOnTop(emptyElt);
        }
      }
      this->theContent.AddOnTop(eltsStack[i]);
    }
  }
  if (!result && comments != nullptr) {
    *comments << "<hr>Parsing stack.<hr>" << this->ToStringParsingStack(this->eltsStack);
  }
  if (result) {
    result = this->ExtractAnswerIds(comments);
  }
  for (int i = 0; i < this->theContent.size; i ++) {
    this->theContent[i].indexInOwner = i;
  }
  if (result) {
    result = this->CheckContent(comments);
  }
  this->theProblemData.CheckConsistency();
  return result;
}

bool CalculatorHTML::InterpretOneAnswerElement(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretOneAnswerElement");
  std::string answerId;
  if (!inputOutput.IsAnswerElement(&answerId)) {
    return true;
  }
  int theIndex = this->GetAnswerIndex(answerId);
  std::string tagClass = inputOutput.GetTagClass();
  if (theIndex == - 1) {
    std::stringstream out;
    out << "<b>Element of class " << tagClass << " has name: "
    << answerId << " but that does not match any answerId value. "
    << this->theProblemData.ToStringAvailableAnswerIds() << ". </b>";
    inputOutput.interpretedCommand = out.str();
    return true;
  }
  Answer& currentA = this->theProblemData.theAnswers.theValues[theIndex];
  if (tagClass == "calculatorAnswerVerification") {
    inputOutput.interpretedCommand = currentA.htmlSpanVerifyAnswer;
  }
  return true;
}

bool CalculatorHTML::InterpretAnswerHighlights(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretAnswerHighlights");
  (void) comments;
  this->answerHighlights.SetSize(0);
  bool answerHighlightStarted = false;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].tag == "answerCalculatorHighlightStart") {
      answerHighlightStarted = true;
      this->answerHighlights.AddOnTop("");
      this->theContent[i].content = "";
      continue;
    }
    if (!answerHighlightStarted) {
      continue;
    }
    if (this->theContent[i].IsAnswerElement(nullptr)) {
      continue;
    }
    if (this->theContent[i].tag == "answerCalculatorHighlightFinish") {
      answerHighlightStarted = false;
      this->theContent[i].content = "";
      continue;
    }
    *this->answerHighlights.LastObject() += this->theContent[i].ToStringInterpretedBody();
    this->theContent[i].content = "";
    this->theContent[i].interpretedCommand = "";
  }
  return true;
}

bool CalculatorHTML::InterpretAnswerElements(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretAnswerElements");
  (void) comments;
  for (int i = 0; i < this->theContent.size; i ++) {
    this->InterpretOneAnswerElement(this->theContent[i]);
  }
  return true;
}

bool CalculatorHTML::PrepareAnswerElements(std::stringstream &comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareAnswerElements");
  (void) comments;
  std::string currentId;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].IsAnswerElement(&currentId)) {
      int index = this->GetAnswerIndex(currentId);
      if (index == - 1) {
        continue;
      }
      Answer& currentA = this->theProblemData.theAnswers.theValues[index];
      std::string tagClass = this->theContent[i].GetTagClass();
      if (
        tagClass == "calculatorButtonSubmit" ||
        tagClass == "calculatorButtonInterpret" ||
        tagClass == "calculatorButtonGiveUp"
      ) {
        currentA.flagAutoGenerateSubmitButtons = false;
      }
      if (tagClass == "calculatorButtonSolution") {
        currentA.flagAutoGenerateButtonSolution = false;
      }
      if (tagClass == "calculatorSolution") {
        currentA.flagSolutionFound = true;
      }
      if (tagClass == "calculatorMQField") {
        currentA.flagAutoGenerateMQfield = false;
      }
      if (tagClass == "calculatorMQButtonPanel") {
        currentA.flagAutoGenerateMQButtonPanel = false;
      }
      if (tagClass == "calculatorAnswerVerification") {
        currentA.flagAutoGenerateVerificationField = false;
      }
    }
  }
  return true;
}

bool CalculatorHTML::ExtractAnswerIds(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::ExtractAnswerIds");
  //we shouldn't clear this->theProblemData.theAnswers: it may contain
  //outdated information loaded from the database. We don't want to loose that info
  //(say we renamed an answerId but students have already stored answers using the old answerId...).
  List<std::string> answerIdsSeenSoFar;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentE = this->theContent[i];
    if (currentE.IsAnswer()) {
      std::string currentId = StringRoutines::StringTrimWhiteSpace(currentE.GetKeyValue("id"));
      if (currentId == "") {
        if (comments != nullptr) {
          *comments << "The answer element: " << currentE.ToStringDebug() << " has empty id. This is not allowed. ";
        }
        return false;
      }
      int theIndex = this->GetAnswerIndex(currentId);
      if (theIndex == - 1) {
        this->theProblemData.AddEmptyAnswerIdOnTop(currentId);
      }
      if (answerIdsSeenSoFar.Contains(currentId)) {
        if (comments != nullptr) {
          *comments << "<b>Answer with id: "
          << currentId << " contained more than once. </b>";
        }
        return false;
      }
      answerIdsSeenSoFar.AddOnTopNoRepetition(currentId);
      this->theProblemData.theAnswers.GetValueCreate(currentId).MQpanelButtonOptions =
      currentE.GetKeyValue("buttons");
      continue;
    }
    if (
      !currentE.IsCommentBeforeSubmission() &&
      !currentE.IsCommentBeforeInterpretation() &&
      !currentE.IsAnswerOnGiveUp() &&
      !currentE.IsSolution()
    ) {
      continue;
    }
    if (answerIdsSeenSoFar.size == 0 && currentE.GetKeyValue("name") == "") {
      if (comments != nullptr) {
        *comments << "Auxilary answer element: " << currentE.ToStringDebug()
        << " has no name and appears before the first answer tag. "
        << "Auxilary answers apply the answer tag whose id is specified in the name "
        << "tag of the auxilary answer. If the auxilary answer has no "
        << "name tag, it is assumed to apply to the (nearest) answer tag above it. "
        << "To fix the issue either place the auxilary element after the answer or "
        << "specify the answer's id in the name tag of the auxilary element. ";
      }
      return false;
    }
    if (currentE.GetKeyValue("name") == "") {
      currentE.SetKeyValue("name", *answerIdsSeenSoFar.LastObject());
    }
  }
  this->theProblemData.CheckConsistency();
  return true;
}

bool CalculatorHTML::CheckConsistencyTopics() {
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    if (this->theTopicS.theValues[i].type == TopicElement::tProblem) {
      if (this->theTopicS.theValues[i].immediateChildren.size > 0) {
        crash << "Topic element: " << this->theTopicS.theValues[i].ToString()
        << " has non-zero immediate children. " << crash;
        return false;
      }
    }
  }
  return true;
}

bool CalculatorHTML::CheckContent(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::CheckContent");
  bool result = true;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (
      currentElt.syntacticRole == "command" &&
      currentElt.IsAnswer() &&
      currentElt.GetKeyValue("id").find('=') != std::string::npos
    ) {
      result = false;
      if (comments != nullptr) {
        *comments << "Error: the id of tag " << currentElt.ToStringDebug()
        << " contains the equality sign which is not allowed. ";
      }
    }
  }
  return result;
}

std::string CalculatorHTML::CleanUpCommandString(const std::string& inputCommand) {
  MacroRegisterFunctionWithName("CalculatorHTML::CleanUpCommandString");
  if (inputCommand == "") {
    return "";
  }
  int realStart = 0;
  int realEnd = static_cast<signed>(inputCommand.size()) - 1;
  for (; realStart < static_cast<signed>(inputCommand.size()); realStart ++) {
    if (
      inputCommand[static_cast<unsigned>(realStart)] == ' ' ||
      inputCommand[static_cast<unsigned>(realStart)] == '\n'
    ) {
      continue;
    }
    if (inputCommand[static_cast<unsigned>(realStart)] == '\\') {
      if (realStart + 1 < static_cast<signed>(inputCommand.size())) {
        if (inputCommand[static_cast<unsigned>(realStart) + 1] == '(') {
          realStart ++;
          continue;
        }
      }
    }
    break;
  }
  for (; realEnd >= 0; realEnd --) {
    if (
      inputCommand[static_cast<unsigned>(realEnd)] == ' ' ||
      inputCommand[static_cast<unsigned>(realEnd)] == '\n'
    ) {
      continue;
    }
    if (inputCommand[static_cast<unsigned>(realEnd)] == ')') {
      if (realEnd > 0) {
        if (inputCommand[static_cast<unsigned>(realEnd) - 1] == '\\') {
          realEnd --;
          continue;
        }
      }
    }
    break;
  }
  if (realEnd < realStart) {
    realEnd = realStart - 1;
  }
  std::string result = inputCommand.substr(static_cast<unsigned>(realStart), static_cast<unsigned>(realEnd - realStart + 1));
  for (int i = static_cast<signed>(result.size()) - 1; i >= 0; i --) {
    if (result[static_cast<unsigned>(i)] == ' ' || result[static_cast<unsigned>(i)] == '\n') {
      continue;
    }
    if (result[static_cast<unsigned>(i)] == ';') {
      return result;
    }
    break;
  }
  if (result == "") {
    return "";
  }
  result.push_back(';');
  return result;
}

bool CalculatorHtmlFunctions::innerExtractCalculatorExpressionFromHtml(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExtractCalculatorExpressionFromHtml");
  CalculatorHTML theFile;
  if (!input.IsOfType<std::string>(&theFile.inputHtml)) {
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  }
  if (!theFile.ParseHTML(&theCommands.Comments)) {
    return false;
  }
  return output.AssignValue(theFile.ToStringExtractedCommands(), theCommands);
}

std::string CalculatorHTML::answerLabels::properties = "properties";
std::string CalculatorHTML::answerLabels::idPanel = "answerPanelId";
std::string CalculatorHTML::answerLabels::answerHighlight = "answerHighlight";
std::string CalculatorHTML::answerLabels::idMQSpan = "idMQSpan";
std::string CalculatorHTML::answerLabels::idMQSpanLocation = "idMQSpanLocation";
std::string CalculatorHTML::answerLabels::idButtonContainer = "idButtonContainer";
std::string CalculatorHTML::answerLabels::MQpanelButtonOptions = "MQpanelButtonOptions";
std::string CalculatorHTML::answerLabels::idPureLatex = "idPureLatex";
std::string CalculatorHTML::answerLabels::idButtonSubmit = "idButtonSubmit";
std::string CalculatorHTML::answerLabels::idButtonInterpret = "idButtonInterpret";
std::string CalculatorHTML::answerLabels::idButtonAnswer = "idButtonAnswer";
std::string CalculatorHTML::answerLabels::idButtonSolution = "idButtonSolution";
std::string CalculatorHTML::answerLabels::idVerificationSpan = "idVerificationSpan";
std::string CalculatorHTML::answerLabels::previousAnswers = "previousAnswers";

JSData CalculatorHTML::GetJavascriptMathQuillBoxesForJSON() {
  MacroRegisterFunctionWithName("CalculatorHTML::GetJavascriptMathQuillBoxesForJSON");
  ////////////////////////////////////////////////////////////////////
  JSData output;
  output.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++) {
    JSData currentAnswerJS;
    Answer& currentAnswer = this->theProblemData.theAnswers.theValues[i];
    ///////////////
    JSData properties;
    for (int j = 0; j < currentAnswer.properties.size(); j ++) {
      properties[currentAnswer.properties.theKeys[j]] = currentAnswer.properties.theValues[j];
    }
    if (currentAnswer.properties.size() > 0) {
      currentAnswerJS[answerLabels::properties] = properties;
    }
    currentAnswerJS[answerLabels::idPanel] = currentAnswer.idAnswerPanel;
    currentAnswerJS[answerLabels::answerHighlight] = currentAnswer.htmlAnswerHighlight;
    currentAnswerJS[answerLabels::idMQSpan] = currentAnswer.idMQfielD;
    currentAnswerJS[answerLabels::idMQSpanLocation] = currentAnswer.idMQFieldLocation;
    currentAnswerJS[answerLabels::idButtonContainer] = currentAnswer.idMQButtonPanelLocation;
    currentAnswerJS[answerLabels::MQpanelButtonOptions] = currentAnswer.MQpanelButtonOptions;
    currentAnswerJS[answerLabels::idPureLatex] = currentAnswer.answerId;
    currentAnswerJS[answerLabels::idButtonSubmit] = currentAnswer.idButtonSubmit;
    currentAnswerJS[answerLabels::idButtonInterpret] = currentAnswer.idButtonInterpret;
    if (currentAnswer.commandsNoEnclosureAnswerOnGiveUpOnly != "") {
      currentAnswerJS[answerLabels::idButtonAnswer] = currentAnswer.idButtonAnswer;
    }
    if (currentAnswer.flagSolutionFound) {
      currentAnswerJS[answerLabels::idButtonSolution] = currentAnswer.idButtonSolution;
    }
    currentAnswerJS[answerLabels::idVerificationSpan] = currentAnswer.idVerificationSpan;
    currentAnswerJS[answerLabels::previousAnswers] = currentAnswer.htmlSpanVerifyAnswer;
    ///////////////
    output[i] = currentAnswerJS;
  }
  return output;
}

std::string CalculatorHTML::GetJavascriptMathQuillBoxes() {
  MacroRegisterFunctionWithName("CalculatorHTML::GetJavascriptMathQuillBoxes");
  std::stringstream out;
  ////////////////////////////////////////////////////////////////////
  out << "<script type =\"text/javascript\">\n";
  out << "answerMQspanIds = [";
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++) {
    out << "\"" << this->theProblemData.theAnswers.theValues[i].idMQfielD << "\"";
    if (i != this->theProblemData.theAnswers.size() - 1) {
      out << ", ";
    }
  }
  out << "];\n";
  out << "preferredButtonContainers = [";
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++) {
    out << "\"" << this->theProblemData.theAnswers.theValues[i].idMQButtonPanelLocation << "\"";
    if (i != this->theProblemData.theAnswers.size() - 1) {
      out << ", ";
    }
  }
  out << "];\n";
  out << "answerIdsPureLatex = [";
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++) {
    out << "\"" << HtmlRoutines::ConvertStringToURLString(this->theProblemData.theAnswers.theValues[i].answerId, false) << "\"";
    if (i != this->theProblemData.theAnswers.size() - 1) {
      out << ", ";
    }
  }
  out << "];\n";
  for (int answerCounter = 0; answerCounter < this->theProblemData.theAnswers.size(); answerCounter ++) {
    Answer& currentA = this->theProblemData.theAnswers.theValues[answerCounter];
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

  for (int answerCounter = 0; answerCounter < this->theProblemData.theAnswers.size(); answerCounter ++) {
    Answer& currentA = this->theProblemData.theAnswers.theValues[answerCounter];
    out << "////////////////////////\n";
    out << currentA.varMQfield  << " = document.getElementById('" << currentA.idMQfielD << "');\n"
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

bool CalculatorHTML::StoreRandomSeedCurrent(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("CalculatorHTML::StoreRandomSeedCurrent");
  if (!theGlobalVariables.flagDatabaseCompiled) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error: database not running. ";
    }
    return false;
  }
  this->theProblemData.flagRandomSeedGiven = true;
  this->currentUseR.SetProblemData(this->fileName, this->theProblemData);
  if (!this->currentUseR.StoreProblemDataToDatabaseJSON(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b style =\"color:red\">"
      << "Error: failed to store problem in database. "
      << "If you see this message, please take a screenshot and email your instructor. "
      << "</b>";
    }
    return false;
  }
  return true;
}

void CalculatorHTML::ComputeProblemLabel() {
  if (this->outputProblemLabel != "") {
    return;
  }
  if (
    this->flagIsExamHome ||
    theGlobalVariables.userCalculatorRequestType == "template" ||
    theGlobalVariables.userCalculatorRequestType == "templateNoLogin"
  ) {
    return;
  }
  if (!this->theTopicS.Contains(this->fileName)) {
    return;
  }
  TopicElement& current = this->theTopicS.GetValueCreate(this->fileName);
  current.ComputeLinks(*this, true);
  this->outputProblemLabel = current.problemNumberString;
  this->outputProblemTitle = current.title;
}

void CalculatorHTML::ComputeBodyDebugString() {
  MacroRegisterFunctionWithName("CalculatorHTML::ComputeBodyDebugString");
  this->outputDebugInformationBody = "";
  if (!theGlobalVariables.UserDebugFlagOn() || !theGlobalVariables.UserDefaultHasAdminRights()) {
    return;
  }
  std::stringstream out;
  out << "<hr>Debug information follows. ";
  if (this->logCommandsProblemGeneratioN.str() != "") {
    out << "<br>" << this->logCommandsProblemGeneratioN.str() << "<hr>";
  }
  if (this->flagIsExamProblem) {
    out << "Exam problem here. ";
  }
  out << "<br>Random seed: "
  << this->theProblemData.randomSeed
  << "<br>ForReal: " << this->flagIsForReal << "<br>seed given: "
  << this->theProblemData.flagRandomSeedGiven
  << "<br>flagRandomSeedGiven: "
  << this->theProblemData.flagRandomSeedGiven
  << "\n<br>\n"
  << "<hr>"
  << "<hr>"
  << HtmlRoutines::ConvertStringToHtmlString(this->ToStringCalculatorArgumentsForProblem("exercise", "false"), true);
  this->outputDebugInformationBody = out.str();
}

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  std::stringstream outBody;
  std::stringstream outHeadPt2;
  this->flagIsExamHome =
  theGlobalVariables.userCalculatorRequestType == "template" ||
  theGlobalVariables.userCalculatorRequestType == "templateNoLogin";
  this->theProblemData.randomSeed = static_cast<unsigned>(this->randomSeedsIfInterpretationFails[
    this->NumAttemptsToInterpret - 1
  ]);
  this->FigureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before after loading problem list");
  outHeadPt2 << HtmlRoutines::GetJavascriptMathjax("");
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before execution");
  if (!this->PrepareAndExecuteCommands(theInterpreter, &comments)) {
    return false;
  }
  //////////////////////////////interpretation takes place before javascript generation as the latter depends on the former.
  this->ComputeProblemLabel();
  std::string problemLabel = "";
  if (
    !this->flagIsExamHome &&
    theGlobalVariables.userCalculatorRequestType != "template" &&
    theGlobalVariables.userCalculatorRequestType != "templateNoLogin"
  ) {
    if (this->theTopicS.Contains(this->fileName)) {
      TopicElement& current = this->theTopicS.GetValueCreate(this->fileName);
      current.ComputeLinks(*this, true);
      problemLabel = current.displayTitle + "&nbsp;&nbsp;";
      if (this->flagDoPrependProblemNavigationBar) {
        problemLabel += current.displayResourcesLinks;
      }
    }
  }
  if (
    this->flagIsExamProblem && this->flagIsForReal && !this->flagIsExamHome &&
    theGlobalVariables.userCalculatorRequestType != "template" &&
    theGlobalVariables.userCalculatorRequestType != "templateNoLogin"
  ) {
    if (theGlobalVariables.flagDatabaseCompiled) {
      bool problemAlreadySolved = false;
      if (this->currentUseR.theProblemData.Contains(this->fileName)) {
        ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(this->fileName);
        if (theProbData.numCorrectlyAnswered >= theProbData.theAnswers.size()) {
          problemAlreadySolved = true;
        }
      }
      this->outputDeadlineString = this->ToStringDeadline(this->fileName, problemAlreadySolved, true, true);
      if (!this->flagUseJSON) {
        if (this->outputDeadlineString == "") {
          outBody << "<b style =\"color:orange\">No deadline yet but scores are recorded. </b>";
        } else {
          outBody << "<b style =\"color:brown\">Scores are recorded. </b>";
        }
        outBody << problemLabel;
        outBody << this->outputDeadlineString << "\n<hr>\n";
      }
    }
  } else if (
    !this->flagIsExamHome && !this->flagIsForReal &&
    theGlobalVariables.userCalculatorRequestType != "template" &&
    theGlobalVariables.userCalculatorRequestType != "templateNoLogin" &&
    theGlobalVariables.userCalculatorRequestType != "templateJSON" &&
    theGlobalVariables.userCalculatorRequestType != "templateJSONNoLogin"
  ) {
    if (!this->flagUseJSON) {
      outBody << "<b style =\"color:green\">Scores not recorded. </b>"
      << problemLabel << "<hr>";
    }
  }
  //////////////////////////////
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time after execution");
  //first command and first syntactic element are the random seed and are ignored.
  theInterpreter.theObjectContainer.resetSliders();
  if (!this->InterpretProcessExecutedCommands(theInterpreter, this->theContent, comments)) {
    outBody << comments.str();
    this->outputHtmlBodyNoTag = outBody.str();
    return false;
  }
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before class management routines");
  this->PrepareAnswerElements(comments);
  this->NumAnswerIdsMathquilled = 0;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].IsInterpretedNotByCalculator()) {
      this->InterpretNotByCalculatorNotAnswer(this->theContent[i]);
    }
  }
  this->InterpretAnswerHighlights(comments);
  for (int i = 0; i < this->theContent.size; i ++) {
    this->InterpretIfAnswer(this->theContent[i]);
  }
  outHeadPt2 << this->topicListJavascriptWithTag;
  this->InterpretAnswerElements(comments);
  this->theProblemData.CheckConsistency();
  this->theProblemData.CheckConsistencyMQids();
  bool headFinished = !this->flagTagHeadPresent;
  std::string tagClass;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (!this->theContent[i].IsHidden()) {
      tagClass = this->theContent[i].GetTagClass();
      if (tagClass == "headFinish") {
        headFinished = true;
        outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        continue;
      }
      if (
        tagClass == "bodyStart" || tagClass == "bodyFinish" ||
        tagClass == "headStart" || tagClass == "headFinish" ||
        tagClass == "htmlStart" || tagClass == "htmlFinish"
      ) {
        if (headFinished) {
          outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        } else {
          outHeadPt2 << "<!--Tag class: " << tagClass << " replaced by auto-generated one-->";
        }
        continue;
      }
      if (headFinished) {
        outBody << this->theContent[i].ToStringInterpretedBody();
      } else {
        outHeadPt2 << this->theContent[i].ToStringInterpretedBody();
      }
    }
  }
  if (this->flagIsExamProblem) {
    outHeadPt2 << this->GetJavascriptMathQuillBoxes();
    if (theInterpreter.flagHasGraphics) {
      MapReferenceS<std::string, std::string, MathRoutines::HashString>& theScripts =
      theInterpreter.theObjectContainer.graphicsScripts;
      for (int i = 0; i < theScripts.size(); i ++) {
        this->theScripts.SetKeyValue(theScripts.theKeys[i], theScripts.theValues[i]);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////
  //out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
  //out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds() - startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before database storage");
  if (theGlobalVariables.flagDatabaseCompiled) {
    bool shouldResetTheRandomSeed = false;
    if (this->flagIsForReal && !this->theProblemData.flagRandomSeedGiven) {
      shouldResetTheRandomSeed = true;
    }
    if (this->flagIsForReal && this->NumAttemptsToInterpret > 1) {
      shouldResetTheRandomSeed = true;
      outBody
      << "<hr><b style =\"color:red\">"
      << "Your problem's random seed was just reset. </b> "
      << "You should be seeing this message very rarely, "
      << "<b>ONLY IF</b> your problem was changed by your instructor "
      << "<b>AFTER</b> you started solving it. "
      << "You should not be seeing this message a second time. "
      << "<b style =\"color:red\">If you see this message every "
      << "time you reload the problem "
      << "this is a bug. Please take a screenshot and send it to your instructor. </b>";
    }
    if (shouldResetTheRandomSeed) {
      bool successStoringSeed = this->StoreRandomSeedCurrent(&comments);
      if (!successStoringSeed) {
        logWorker << logger::red << "This should not happen: failed to store random seed." << logger::endL
        << logger::yellow << comments.str() << logger::endL;
      }
    }
    this->ComputeBodyDebugString();

  }
  std::stringstream navigationAndEditTagStream;
  if (this->flagDoPrependProblemNavigationBar) {
    navigationAndEditTagStream << this->ToStringProblemNavigation();
  }
  if (this->flagDoPrependEditPagePanel) {
    if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentVieWOn()) {
      navigationAndEditTagStream << this->GetEditPagePanel();
    }
  }
  this->outputProblemNavigatioN = navigationAndEditTagStream.str();
  this->outputHtmlBodyNoTag = outBody.str();
  this->outputHtmlHeadNoTag = outHeadPt2.str();
  return true;
}

std::string CalculatorHTML::ToStringProblemNavigation() const {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemNavigation");
  std::stringstream out;
  std::string exerciseRequest = "exercise";
  std::string studentView = theGlobalVariables.UserStudentVieWOn() ? "true" : "false";
  std::string linkSeparator = " | ";
  std::string linkBigSeparator = " || ";
  if (theGlobalVariables.UserGuestMode()) {
    exerciseRequest = "exerciseNoLogin";
  }
  if (theGlobalVariables.UserGuestMode()) {
    out << "<b>Guest mode</b>" << linkSeparator;
  }
  if (!theGlobalVariables.flagLoggedIn) {
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=login\">Log in</a> " << linkSeparator;
  }
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  std::string calcArgsNoPassExamDetails =
  theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (this->flagIsExamProblem) {
    if (theGlobalVariables.userCalculatorRequestType == "exercise") {
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=scoredQuiz&"
      << this->ToStringCalculatorArgumentsForProblem("scoredQuiz", studentView)
      << "\">" << this->stringScoredQuizzes << "</a>" << linkSeparator;
      out << "<b style =\"color:green\">" << this->stringPracticE
      << "</b>" << linkSeparator;
    } else if (theGlobalVariables.userCalculatorRequestType == "scoredQuiz") {
      out << "<b style =\"color:brown\">"
      << this->stringScoredQuizzes << "</b>" << linkSeparator;
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=exercise&"
      << this->ToStringCalculatorArgumentsForProblem("exercise", studentView)
      << "\">" << this->stringPracticE << "</a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated) {
    int indexInParent = this->problemNamesNoTopics.GetIndex(this->fileName);
    if (indexInParent == - 1) {
      out << "<b>Problem not in course</b>" << linkSeparator;
    } else {
      if (indexInParent > 0) {
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection") != "") {
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        }
        out << "topicList=" << HtmlRoutines::ConvertStringToURLString(this->topicListFileName, false) << "&";
        out << "courseHome=" << HtmlRoutines::ConvertStringToURLString(this->courseHome, false) << "&";
        out << "fileName=" << HtmlRoutines::ConvertStringToURLString(this->problemNamesNoTopics[indexInParent - 1], false)
        << "\">&#8592;</a>" << linkSeparator;
      } else {
        out << "<a disabled =\"disabled\">&#8592;</a>" << linkSeparator;
      }
      if (indexInParent < this->problemNamesNoTopics.size - 1) {
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request="
        << theGlobalVariables.userCalculatorRequestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (theGlobalVariables.GetWebInput("studentSection") != "") {
          out << "studentSection=" << theGlobalVariables.GetWebInput("studentSection") << "&";
        }
        out << "topicList=" << HtmlRoutines::ConvertStringToURLString(this->topicListFileName, false) << "&";
        out << "courseHome=" << HtmlRoutines::ConvertStringToURLString(this->courseHome, false) << "&";
        out << "fileName=" << HtmlRoutines::ConvertStringToURLString(this->problemNamesNoTopics[indexInParent + 1], false)
        << "\">&#8594;</a>" << linkSeparator;
      } else {
        out << "<a disabled =\"disabled\">&#8594;</a>" << linkSeparator;
      }
    }
  }
  if (this->flagIsExamProblem &&(
    theGlobalVariables.userCalculatorRequestType == "exercise" ||
    theGlobalVariables.userCalculatorRequestType == "exerciseNoLogin"
  )) {
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=" << theGlobalVariables.userCalculatorRequestType << "&"
    << this->ToStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">" << this->stringProblemLink << " (#"
    << this->theProblemData.randomSeed << ")</a>" << linkBigSeparator;
  }
  if (theGlobalVariables.UserDefaultHasAdminRights()) {
    if (theGlobalVariables.UserStudentVieWOn()) {
      out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
      << this->ToStringCalculatorArgumentsForProblem(
        theGlobalVariables.userCalculatorRequestType,
        "false",
        HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("studentSection"), false)
      ) << "\">Admin view</a>" << linkSeparator;
    } else {
      out << "<b>Admin view</b>" << linkSeparator;
    }
    if (this->databaseStudentSections.size == 0) {
      if (theGlobalVariables.UserStudentVieWOn()) {
        out << "<b>Student View</b>";
      } else {
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
        << this->ToStringCalculatorArgumentsForProblem(theGlobalVariables.userCalculatorRequestType, "true", "")
        << "\">Student view</a>";
      }
    } else {
      out << "Student view, section: ";
    }
    for (int i = 0; i < this->databaseStudentSections.size; i ++) {
      if (this->databaseStudentSections[i] != "") {
        if (
          theGlobalVariables.UserStudentVieWOn() &&
          this->databaseStudentSections[i] == HtmlRoutines::ConvertURLStringToNormal(
            theGlobalVariables.GetWebInput("studentSection"), false
          )
        ) {
          out << "<b>" << this->databaseStudentSections[i] << "</b>";
        } else {
          out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?"
          << this->ToStringCalculatorArgumentsForProblem(
            theGlobalVariables.userCalculatorRequestType, "true", this->databaseStudentSections[i]
          ) << "\">" << this->databaseStudentSections[i] << " </a>";
        }
        if (i != this->databaseStudentSections.size - 1) {
          out << linkSeparator;
        }
      }
    }
  }
  if (out.str() == "") {
    return "";
  }
  std::stringstream outFinal;
  outFinal << "<problemNavigation>" << out.str();
  outFinal << "</problemNavigation>";
  return outFinal.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem(
  const std::string& requestType,
  const std::string& studentView,
  const std::string& studentSection,
  bool includeRandomSeedIfAppropriate
) const {
  MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsForProblem");
  if (!theGlobalVariables.flagLoggedIn && !theGlobalVariables.UserGuestMode()) {
    return "";
  }
  std::stringstream out;
  out << "request=" << requestType << "&";
  List<std::string> excludedTags;
  excludedTags.AddOnTop("randomSeed");
  out << theGlobalVariables.ToStringCalcArgsNoNavigation(&excludedTags)
  << "courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "&";
  if (!theGlobalVariables.flagRunningApache && this->fileName != "") {
    out << WebAPI::problem::fileName << "=" << HtmlRoutines::ConvertStringToURLString(this->fileName, false) << "&";
  } else {
    out << WebAPI::problem::fileName << "=" << HtmlRoutines::ConvertStringToURLString(theGlobalVariables.GetWebInput(WebAPI::problem::fileName), false)
    << "&";
  }
  out << "topicList=" << theGlobalVariables.GetWebInput("topicList") << "&";
  out << "studentView=" << studentView << "&";
  if (studentSection != "") {
    out << "studentSection=" << HtmlRoutines::ConvertStringToURLString(studentSection, false) << "&";
  }
  if (includeRandomSeedIfAppropriate) {
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
  }
  return out.str();
}

std::string CalculatorHTML::GetEditPageButton(const std::string& desiredFileName, bool includeCloneButton) {
  MacroRegisterFunctionWithName("CalculatorHTML::GetEditPageButton");
  std::stringstream out;
  out << "\n<a class =\"linkStandardButtonLike\" href=\""
  << theGlobalVariables.DisplayNameExecutable << "?request=editPage&";
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(desiredFileName, false);
  std::stringstream refStreamNoRequest;
  std::string spanCloningAttemptResultID = "spanCloningAttemptResultID" + desiredFileName;
  std::string clonePageAreaID = "clonePageAreaID" + desiredFileName;
  //  out << "cleaned up link: " << cleaneduplink;
  //  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation(nullptr)
  << "fileName=" << urledProblem << "&"
  << "topicList=" << HtmlRoutines::ConvertStringToURLString(this->topicListFileName, false) << "&"
  << "courseHome=" << theGlobalVariables.GetWebInput("courseHome") << "&";
  out << refStreamNoRequest.str() << "\">" << "Edit" << "</a>";
  out << "<textarea class =\"currentFileNameArea\" id =\"" << clonePageAreaID << "\" cols =\""
  << desiredFileName.size() + 7 << "\">" << desiredFileName << "</textarea>\n";
  if (includeCloneButton) {
    out
    << "<button class =\"buttonClone\" onclick=\""
    << "submitStringAsMainInput(document.getElementById('"
    << clonePageAreaID << "').value, '" << spanCloningAttemptResultID << "', 'clonePage'"
    << ", '"
    << spanCloningAttemptResultID
    << "'"
    << ");"
    << "\" >Clone</button> <span id =\"" << spanCloningAttemptResultID << "\"></span>";
  }
  return out.str();
}

std::string CalculatorHTML::ToStringProblemScoreFull(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemScoreFull");
  (void) theFileName;
  std::stringstream out;
  if (theGlobalVariables.UserGuestMode()) {
    out << "scores require login";
    return out.str();
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    out << "scores not available: no database. ";
    return out.str();
  }
  Rational currentWeight;
  if (this->currentUseR.theProblemData.Contains(theFileName)) {
    ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    if (!theProbData.flagProblemWeightIsOK) {
      out << "<span style =\"color:orange\">No point weight assigned yet. </span>";
      if (!theProbData.adminData.GetWeightFromCoursE(this->currentUseR.courseComputed, currentWeight)) {
        currentWeight = 0;
      }
      if (theProbData.theAnswers.size() == 1) {
        if (theProbData.numCorrectlyAnswered == 1) {
          out << theProbData.totalNumSubmissions << " submission(s), problem correctly answered. ";
        } else {
          out << theProbData.totalNumSubmissions << " submission(s), problem not correctly answered yet. ";
        }
      } else if (theProbData.theAnswers.size() > 1) {
        out << theProbData.totalNumSubmissions << " submission(s), " << theProbData.numCorrectlyAnswered
        << " out of " << theProbData.theAnswers.size() << " subproblems correctly answered. ";
      }
    } else if (theProbData.totalNumSubmissions != 0) {
      if (theProbData.numCorrectlyAnswered < theProbData.theAnswers.size()) {
        out << "<b style =\"color:red\">"
        << theProbData.Points << " out of "
        << currentWeight << " point(s). </b>";
      } else if (theProbData.numCorrectlyAnswered == theProbData.theAnswers.size()) {
        out << "<b style =\"color:green\">"
        << theProbData.Points << " out of "
        << currentWeight << " point(s). </b> ";
      }
    }
  } else {
    out << "<b style =\"color:brown\">No submissions.</b>" ;
  }
  return out.str();
}

std::string CalculatorHTML::ToStringProblemScoreShort(const std::string& theFileName, bool& outputAlreadySolved) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemScoreShort");
  (void) theFileName;
  (void) outputAlreadySolved;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Error: database not running. ";
  }
  std::stringstream out;
  if (theGlobalVariables.UserGuestMode()) {
    out << "scores require login";
    return out.str();
  }
  std::stringstream problemWeight;
  ProblemData theProbData;
  bool showModifyButton = theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentVieWOn();
  outputAlreadySolved = false;
  Rational currentWeight;
  std::string currentWeightAsGivenByInstructor;
  if (this->currentUseR.theProblemData.Contains(theFileName)) {
    theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    Rational percentSolved = 0, totalPoints = 0;
    percentSolved.AssignNumeratorAndDenominator(theProbData.numCorrectlyAnswered, theProbData.theAnswers.size());
    theProbData.flagProblemWeightIsOK = theProbData.adminData.GetWeightFromCoursE(
      this->currentUseR.courseComputed, currentWeight, &currentWeightAsGivenByInstructor
    );
    if (!theProbData.flagProblemWeightIsOK) {
      problemWeight << "?";
      if (currentWeightAsGivenByInstructor != "") {
        problemWeight << "<span style =\"color:red\">"
        << currentWeightAsGivenByInstructor << "(Error)</span>";
      }
    } else {
      problemWeight << currentWeight;
      totalPoints = percentSolved * currentWeight;
    }
    outputAlreadySolved = (percentSolved == 1);
    if (!outputAlreadySolved) {
      if (!theProbData.flagProblemWeightIsOK) {
        out << "<b style =\"color:brown\">" << percentSolved << " out of " << problemWeight.str() << "</b>";
      } else {
        out << "<b style =\"color:red\">" << totalPoints << " out of " << problemWeight.str() << "</b>";
      }
    } else {
      if (!theProbData.flagProblemWeightIsOK) {
        out << "<b style =\"color:green\">solved</b>";
      } else {
        out << "<b style =\"color:green\">" << totalPoints << " out of " << problemWeight.str() << "</b>";
      }
    }
  } else {
    out << "<b style =\"color:brown\">need to solve</b>";
  }
  if (!showModifyButton) {
    return out.str();
  }
  std::stringstream finalOut;
  finalOut << "<button class =\"accordionLike\" onclick=\"toggleProblemWeights();\">"
  << out.str()
  << "</button>";
  finalOut << "<span class =\"panelProblemWeights\"><br>"
  << this->ToStringProblemWeightButton(theFileName)
  << "</span>";
  return finalOut.str();
}

std::string CalculatorHTML::ToStringProblemWeightButton(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemWeighT");
  if (
    !theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentVieWOn()
  ) {
    return "";
  }
  std::string urledProblem = HtmlRoutines::ConvertStringToURLString(theFileName, false);
  std::stringstream out;
  std::string idPoints = "points" + urledProblem;
  std::string idButtonModifyPoints = "modifyPoints" + urledProblem;
  std::string idPointsModOutput = "modifyPointsOutputSpan" + urledProblem;
  out << "Pts: <textarea class =\"textareaStudentPoints\" rows =\"1\" cols =\"2\" id =\"" << idPoints << "\">";
  bool weightIsOK = false;
  std::string problemWeightAsGivenByInstructor;
  if (theGlobalVariables.flagDatabaseCompiled) {
    Rational unusedRat;
    weightIsOK = this->currentUseR.theProblemData.GetValueCreate(theFileName).adminData.GetWeightFromCoursE(
      this->currentUseR.courseComputed, unusedRat, &problemWeightAsGivenByInstructor
    );
    out << problemWeightAsGivenByInstructor;
  }
  out << "</textarea>";
  if (!weightIsOK && problemWeightAsGivenByInstructor != "") {
    out << "<b style ='color:red'>Error</b>";
  }
  out << "<button id =\"" << idButtonModifyPoints << "\" "
  << "onclick=\"" << "modifyWeight('" << urledProblem << "');\""
  << " disabled>";
  out << "Modify";
  out << "</button>";
  out << "<span id =\"" << idPointsModOutput << "\">" << "</span>";
  return out.str();
}

void TopicElement::ComputeID() {
  MacroRegisterFunctionWithName("TopicElement::ComputeID");
  if (this->problemFileName != "") {
    this->id = this->problemFileName;
    this->type = this->tProblem;
  } else {
    std::stringstream out;
    out << this->title;
    if (this->type == this->tTexHeader) {
      out << "[sourceHeader]";
    }
    if (this->type == this->tChapter) {
      out << "[Chapter]";
    }
    if (this->type == this->tSection) {
      out << "[Section]";
    }
    if (this->type == this->tSubSection) {
      out << "[SubSection]";
    }
    if (this->type == this->tProblem) {
      out << "[TitledItem]";
    }
    this->id = out.str();
  }
  this->idBase64 = Crypto::computeSha3_256OutputBase64URL(this->id);
  this->studentScoresSpanId = "topic" + Crypto::computeSha3_256OutputBase64URL(this->id);
}

void TopicElement::AddTopic(TopicElement& inputElt, MapList<std::string, TopicElement, MathRoutines::HashString>& output) {
  MacroRegisterFunctionWithName("TopicElement::AddTopic");
  int numToCheck = 4;
  if (output.size() >= numToCheck && inputElt.type != inputElt.tTexHeader && inputElt.type != inputElt.tChapter) {
    bool startsWithChapter = false;
    for (int i = 0; i < numToCheck - 1; i ++) {
      if (output.theValues[i].type == inputElt.tChapter) {
        startsWithChapter = true;
        break;
      }
    }
    if (!startsWithChapter) {
      TopicElement chapterlessChapter;
      chapterlessChapter.parentTopics.AddOnTop(output.size());
      chapterlessChapter.type = chapterlessChapter.tChapter;
      chapterlessChapter.title = "Topics without chapter";
      TopicElement::AddTopic(chapterlessChapter, output);
    }
  }
  inputElt.ComputeID();
  if (inputElt.id == "") {
    inputElt.type = inputElt.tError;
    inputElt.id = "error";
  }
  if (output.Contains(inputElt.id)) {
    inputElt.id += "[Error]";
    inputElt.title = "[Error]: Entry " + inputElt.title + " already present. ";
  }
  inputElt.indexInParent = output.size();
  output.SetKeyValue(inputElt.id, inputElt);
  if (inputElt.parentTopics.size > 1) {
    int indexImmediateParent = - 1;
    int indexCurrentElement = output.GetIndex(inputElt.id);
    for (int i = 0; i < inputElt.parentTopics.size; i ++) {
      if (inputElt.parentTopics[i] > indexImmediateParent && inputElt.parentTopics[i] < indexCurrentElement) {
        indexImmediateParent = inputElt.parentTopics[i];
      }
    }
    if (indexImmediateParent > - 1) {
      output.theValues[indexImmediateParent].immediateChildren.AddOnTop(indexCurrentElement);
    }
  }
  if (inputElt.immediateChildren.size > 0) {
    crash << "New topic element must have zero children. " << crash;
  }
}

void TopicElement::reset(int parentSize, MapList<std::string, TopicElement, MathRoutines::HashString>* containerElements) {
  this->type = this->tUndefined;
  this->indexInParent = - 1;
  this->flagSubproblemHasNoWeight = false;
  this->title = "empty";
  this->id = "";
  this->video = "";
  this->videoHandwritten = "";

  this->querySlides   = "";
  this->queryHomework = "";

  this->handwrittenSolution = "";

  this->sourceSlides.SetSize(0);
  this->sourceHomework.SetSize(0);
  this->sourceHomeworkIsSolution.SetSize(0);
  this->problemFileName = "";
  this->error = "";
  if (parentSize != - 1) {
    this->parentTopics.SetSize(MathRoutines::Minimum(parentSize, this->parentTopics.size));
    if (this->problemNumber.size < 4) {
      this->problemNumber.initializeFillInObject(4, 0);
    }
    for (int i = parentSize + 1; i < this->problemNumber.size; i ++) {
      this->problemNumber[i] = 0;
    }
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
  if (parentSize == 0) {
    this->type = this->tChapter;
  }
  if (parentSize == 1) {
    this->type = this->tSection;
  }
  if (parentSize == 2) {
    this->type = this->tSubSection;
  }
  if (parentSize == 3) {
    this->type = this->tProblem;
  }
  if (containerElements == nullptr) {
    return;
  }
  for (int i = 0; i < this->parentTopics.size; i ++) {
    if (containerElements->theValues[this->parentTopics[i]].type >= this->type) {
      this->parentTopics.PopIndexShiftDown(i);
      i --;
    }
  }
}

bool TopicElement::LoadTopicBundle(
  const std::string& inputFileName,
  MapList<std::string, List<std::string>, MathRoutines::HashString>& output,
  CalculatorHTML& owner,
  std::stringstream& errorStream
) {
  MacroRegisterFunctionWithName("TopicElement::LoadTopicBundle");
  std::string fileName = StringRoutines::StringTrimWhiteSpace(inputFileName);
  std::string newTopicBundles;
  if (!FileOperations::IsOKfileNameVirtual(fileName, false, &errorStream)) {
    errorStream << "The file name " << fileName << " is not a valid topic bundle file name. ";
    return false;
  }
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(fileName, newTopicBundles, &errorStream)) {
    errorStream << "Could not open topic bundle file. ";
    return false;
  }
  owner.loadedTopicBundles.AddOnTop(fileName);
  std::string currentLine, currentId;
  List<std::string> bundleNameStack;
  std::stringstream bundleReader(newTopicBundles);
  while (std::getline(bundleReader, currentLine, '\n')) {
    if (StringRoutines::StringBeginsWith(currentLine, "BundleBegin:", &currentId)) {
      bundleNameStack.AddOnTop(StringRoutines::StringTrimWhiteSpace(currentId));
    } else if (StringRoutines::StringBeginsWith(currentLine, "BundleEnd:", &currentId)) {
      if (bundleNameStack.size > 0) {
        bundleNameStack.RemoveLastObject();
      } else {
        errorStream << "<b style =\"color:red\">BundleEnd command without BungleBegin.</b>";
        return false;
      }
    } else {
      for (int i = 0; i < bundleNameStack.size; i ++) {
        output.GetValueCreate(bundleNameStack[i]).AddOnTop(currentLine);
      }
    }
  }
  return true;
}

void TopicElement::GetTopicList(
  const std::string& inputString,
  MapList<std::string, TopicElement, MathRoutines::HashString>& output,
  CalculatorHTML& owner
) {
  MacroRegisterFunctionWithName("TopicElement::GetTopicList");
  std::stringstream tableReader(inputString);
  std::string currentLine, currentArgument;
  TopicElement currentElt;
  bool found = false;
  currentElt.problemNumber.initializeFillInObject(4, 0);
  MemorySaving<MapList<std::string, List<std::string>, MathRoutines::HashString> > topicBundles;
  List<std::string> lineStack;
  owner.initTopicElementNames();
  int numLinesSoFar = 0;
  bool showedAllowedDataEntries = false;
  for (;;) {
    if (lineStack.size == 0) {
      if (std::getline(tableReader, currentLine, '\n')) {
        lineStack.AddOnTop(currentLine);
      }
    }
    if (lineStack.size <= 0) {
      break;
    }
    if (numLinesSoFar > 10000) {
      std::stringstream errorStream;
      errorStream << "More than 10000 topic lines have been read so far; this is not allowed. "
      << "Could this be an error due to infinite inclusion of topic bundles -"
      << "perhaps a topic bundle file is requesting to load itself? ";
      currentElt.error = errorStream.str();
      currentElt.type = currentElt.tError;
      found = true;
      break;
    }
    currentLine = lineStack.PopLastObject();
    if (StringRoutines::StringTrimWhiteSpace(currentLine) == "") {
      continue;
    }
    if (currentLine.size() > 0) {
      if (currentLine[0] == '%') {
        continue;
      }
    }
    if (StringRoutines::StringBeginsWith(currentLine, "LoadTopicBundles:", &currentArgument)) {
      std::stringstream errorStream;
      if (!TopicElement::LoadTopicBundle(
        StringRoutines::StringTrimWhiteSpace(currentArgument), topicBundles.GetElement(), owner, errorStream
      )) {
        currentElt.error = errorStream.str();
        currentElt.type = currentElt.tError;
        found = true;
      }
    } else if (StringRoutines::StringBeginsWith(currentLine, "TopicBundle:", &currentArgument)) {
      currentArgument = StringRoutines::StringTrimWhiteSpace(currentArgument);
      std::stringstream errorStream;
      if (topicBundles.GetElement().Contains(currentArgument)) {
        List<std::string>& currentBundle = topicBundles.GetElement().GetValueCreate(currentArgument);
        for (int j = currentBundle.size - 1; j >= 0; j --) {
          lineStack.AddOnTop(currentBundle[j]);
        }
      } else {
        if (found) {
          TopicElement::AddTopic(currentElt, output);
        }
        errorStream << "Topic bundle does not appear to contain the desired element: "
        << currentArgument;
        currentElt.error = errorStream.str();
        currentElt.type = currentElt.tError;
      }
    } else if (StringRoutines::StringBeginsWith(currentLine, "SlidesSourceHeader:", &currentArgument)) {
      owner.slidesSourcesHeaders.AddOnTop(StringRoutines::StringTrimWhiteSpace(currentArgument));
      continue;
    } else if (StringRoutines::StringBeginsWith(currentLine, "HomeworkSourceHeader:", &currentArgument)) {
      owner.sourcesHomeworkHeaders.AddOnTop(StringRoutines::StringTrimWhiteSpace(currentArgument));
      continue;
    } else if (StringRoutines::StringBeginsWith(currentLine, "Chapter:", &currentArgument)) {
      if (found) {
        TopicElement::AddTopic(currentElt, output);
      }
      found = true;
      currentElt.reset(0, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = StringRoutines::StringTrimWhiteSpace(currentArgument);
    } else if (StringRoutines::StringBeginsWith(currentLine, "Section:", &currentArgument)) {
      if (found) {
        TopicElement::AddTopic(currentElt, output);
      }
      found = true;
      currentElt.reset(1, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = StringRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.title;
    } else if (StringRoutines::StringBeginsWith(currentLine, "Topic:", &currentArgument)) {
      if (found) {
        TopicElement::AddTopic(currentElt, output);
      }
      found = true;
      currentElt.reset(2, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = StringRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.title;
    } else if (StringRoutines::StringBeginsWith(currentLine, "Title:", &currentArgument)) {
      if (found) {
        TopicElement::AddTopic(currentElt, output);
      }
      found = true;
      currentElt.reset(3, &output);
      currentElt.parentTopics.AddOnTop(output.size());
      currentElt.title = StringRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.title;
    } else if (StringRoutines::StringBeginsWith(currentLine, "Video:", &currentArgument)) {
      currentElt.video = StringRoutines::StringTrimWhiteSpace(currentArgument);
    } else if (StringRoutines::StringBeginsWith(currentLine, "VideoHandwritten:", &currentArgument)) {
      currentElt.videoHandwritten = StringRoutines::StringTrimWhiteSpace(currentArgument);
    } else if (StringRoutines::StringBeginsWith(currentLine, "SlidesSource:", &currentArgument)) {
      currentElt.sourceSlides.AddOnTop(StringRoutines::StringTrimWhiteSpace(currentArgument));
    } else if (StringRoutines::StringBeginsWith(currentLine, "HomeworkSource:", &currentArgument)) {
      currentElt.sourceHomework.AddOnTop(StringRoutines::StringTrimWhiteSpace(currentArgument));
      currentElt.sourceHomeworkIsSolution.AddOnTop(false);
    } else if (StringRoutines::StringBeginsWith(currentLine, "HomeworkSolutionSource:", &currentArgument)) {
      currentElt.sourceHomework.AddOnTop(StringRoutines::StringTrimWhiteSpace(currentArgument));
      currentElt.sourceHomeworkIsSolution.AddOnTop(true);
    } else if (StringRoutines::StringBeginsWith(currentLine, "SlidesLatex:", &currentArgument)) {
      currentElt.sourceSlides.AddOnTop("LaTeX: " + StringRoutines::StringTrimWhiteSpace(currentArgument));
    } else if (StringRoutines::StringBeginsWith(currentLine, "HomeworkLatex:", &currentArgument)) {
      currentElt.sourceHomework.AddOnTop("LaTeX: " + StringRoutines::StringTrimWhiteSpace(currentArgument));
      currentElt.sourceHomeworkIsSolution.AddOnTop(false);
    } else if (StringRoutines::StringBeginsWith(currentLine, "HandwrittenSolutions:", &currentArgument)) {
      currentElt.handwrittenSolution = StringRoutines::StringTrimWhiteSpace(currentArgument);
    } else if (StringRoutines::StringBeginsWith(currentLine, "Problem:", &currentArgument)) {
      currentElt.problemFileName = StringRoutines::StringTrimWhiteSpace(currentArgument);
      currentElt.id = currentElt.problemFileName;
      found = true;
    } else {
      std::stringstream errorStream;
      errorStream << "Failed to parse topic element: " << currentLine << ". ";
      if (!showedAllowedDataEntries) {
        errorStream << "<br>The allowed data labels are CASE SENSITIVE: ";
        for (int j = 0; j < owner.calculatorTopicElementNames.size; j ++) {
          errorStream << "<br>" << owner.calculatorTopicElementNames[j];
        }
        errorStream << "<br>You need to include the column character<b>:</b> "
        << "immediately after the data labels. The data entries are terminated by new line. "
        << "Here is a correctly entered example:"
        << "<br>Title: Complex multiplication"
        << "<br>Problem: DefaultProblemLocation/Complex-multiplication-z-times-w.html"
        << "<br>SlidesSource: freecalc/modules/complex-numbers/complex-numbers-addition-multiplication-example-1"
        << "<br>\n";
      }
      showedAllowedDataEntries = true;
      currentElt.error = errorStream.str();
      currentElt.type = currentElt.tError;
      found = true;
    }
  }
  owner.calculatorTopicBundles.AddOnTopNoRepetition(topicBundles.GetElement().theKeys);
  if (found) {
    TopicElement::AddTopic(currentElt, output);
  }
}

void CalculatorHTML::InterpretAccountInformationLinks(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretAccountInformationLinks");
  std::stringstream out;
  if (!theGlobalVariables.flagLoggedIn) {
    out << "<b>User not logged-in.</b>";
    inputOutput.interpretedCommand = out.str();
    return;
  }
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
    out << "<b>Account management requires https.</b>";
    inputOutput.interpretedCommand = out.str();
    return;
  }
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage\">Change password</a>";
  if (theGlobalVariables.UserDefaultHasAdminRights()) {
    out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts\">Manage accounts</a>";
  }
  inputOutput.interpretedCommand = out.str();
  return;
}

bool CalculatorHTML::LoadAndParseTopicList(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::LoadAndParseTopicList");
  if (this->theTopicS.size() != 0) {
    return true;
  }
  if (this->topicListContent == "") {
    if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(
      this->topicListFileName, this->topicListContent, &comments
    )) {
      comments << "Failed to load the topic list associated with this course. "
      << "Go to ``Select course'' from the menu to see a list of available courses. ";
      return false;
    }
  }
  if (this->topicListContent == "") {
    comments  << "Topic list empty. Topic list file name: " << this->topicListFileName << ". ";
    return false;
  }
  TopicElement::GetTopicList(this->topicListContent, this->theTopicS, *this);
  this->CheckConsistencyTopics();
  this->problemNamesNoTopics.Clear();
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    if (this->theTopicS.theValues[i].problemFileName != "") {
      this->problemNamesNoTopics.AddOnTop(this->theTopicS.theValues[i].problemFileName);
    }
  }
  for (int i = this->theTopicS.size() - 1; i >= 0; i --) {
    TopicElement& currentElt = this->theTopicS.theValues[i];
    if (currentElt.problemFileName != "") {
      continue;
    }
    if (currentElt.type == currentElt.tSubSection) {
      currentElt.totalSubSectionsUnderME = 0;
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections = 0;
      currentElt.flagContainsProblemsNotInSubsection = false;
      continue;
    }
    currentElt.flagContainsProblemsNotInSubsection = false;
    currentElt.totalSubSectionsUnderME = 0;
    for (int j = 0; j < currentElt.immediateChildren.size; j ++) {
      TopicElement& currentChild = this->theTopicS.theValues[currentElt.immediateChildren[j]];
      if (currentChild.type == currentChild.tSubSection) {
        currentElt.totalSubSectionsUnderME ++;
        currentElt.totalSubSectionsUnderMeIncludingEmptySubsections ++;
      } else if (currentChild.problemFileName != "") {
        currentElt.flagContainsProblemsNotInSubsection = true;
      } else {
        currentElt.totalSubSectionsUnderME += currentChild.totalSubSectionsUnderME;
        currentElt.totalSubSectionsUnderMeIncludingEmptySubsections += currentChild.totalSubSectionsUnderMeIncludingEmptySubsections;
      }
    }
    if (currentElt.flagContainsProblemsNotInSubsection) {
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections ++;
    }
  }
  this->CheckConsistencyTopics();
  return true;
}

void CalculatorHTML::InterpretProblemNavigationBar(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretCalculatorNavigationBar");
  inputOutput.interpretedCommand = this->ToStringProblemNavigation();
  this->flagDoPrependProblemNavigationBar = false;
}

std::string CalculatorHTML::GetEditPagePanel() {
  MacroRegisterFunctionWithName("CalculatorHTML::GetEditPagePanel");
  std::stringstream out;
  out
  << "<editPagePanel>";
  if (theGlobalVariables.UserDebugFlagOn()) {
    out << this->ToStringLinkCurrentAdmin("Turn off debug", false, false);
  } else {
    out << this->ToStringLinkCurrentAdmin("Debug page", true, true);
  }
  out << this->GetEditPageButton(this->fileName);
  if (this->flagIsExamHome) {
    out << this->GetEditPageButton(this->topicListFileName);
    for (int i = 0; i < this->loadedTopicBundles.size; i ++) {
      out << this->GetEditPageButton(this->loadedTopicBundles[i]);
    }
  }
  out << "</editPagePanel>";
  return out.str();
}

void CalculatorHTML::InterpretEditPagePanel(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretCalculatorNavigationBar");
  inputOutput.interpretedCommand = this->GetEditPagePanel();
  this->flagDoPrependEditPagePanel = false;
}

std::string CalculatorHTML::ToStringTopicListJSON() {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringTopicListJSON");
  std::stringstream out;
  if (!this->LoadAndParseTopicList(out)) {
    return "\"" + out.str() + "\"";
  }
  JSData output, topicBundles;
  topicBundles.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->loadedTopicBundles.size; i ++) {
    topicBundles[i] = this->loadedTopicBundles[i];
  }
  output["topicBundleFile"] = topicBundles;
  output["children"].theType = JSData::token::tokenArray;
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    TopicElement& currentElt = this->theTopicS.theValues[i];
    if (currentElt.type == currentElt.tChapter) {
      output["children"].theList.AddOnTop(currentElt.ToJSON(*this));
    }
  }
  return output.ToString(false);
}

void CalculatorHTML::InterpretTableOfContents(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretTableOfContents");
  std::stringstream out;
  if (!this->LoadAndParseTopicList(out)) {
    inputOutput.interpretedCommand = out.str();
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
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    TopicElement& currentElt = this->theTopicS.theValues[i];
    if (subSectionStarted) {
      if (
        currentElt.type == currentElt.tSubSection ||
        currentElt.type == currentElt.tSection ||
        currentElt.type == currentElt.tChapter
      ) {
        out << "</li>";
      }
    }
    if (sectionStarted) {
      if (
        currentElt.type == currentElt.tSection ||
        currentElt.type == currentElt.tChapter
      ) {
        out << "</ul></li>";
      }
    }
    if (chapterStarted) {
      if (currentElt.type == currentElt.tChapter) {
        out << "</ul></li>";
      }
    }
    if (currentElt.type == currentElt.tChapter) {
      out << "<li>" << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=template&fileName=" << this->fileName << "&"
      << "topicList=" << this->topicListFileName << "&" << "chapter =" << currentElt.title
      << "\">" << currentElt.title << "</a>" << "<br>\n";
      chapterStarted = true;
      sectionStarted = false;
      subSectionStarted = false;
      out << "<ul>";
    } else if (currentElt.type == currentElt.tSection) {
      out << "<li>" << currentElt.title << "<br>\n";
      sectionStarted = true;
      subSectionStarted = false;
      out << "<ul>";
    } else if (currentElt.type == currentElt.tError) {
      out << "Error parsing topic list. " << currentElt.error;
    }
  }
  if (subSectionStarted) {
    out << "</li>";
  }
  if (sectionStarted) {
    out << "</ul></li>";
  }
  if (chapterStarted) {
    out << "</ul></li>";
  }
  out << "</ul>";
  inputOutput.interpretedCommand = out.str();
}

void CalculatorHTML::InterpretJavascripts(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretJavascripts");
  std::string javascriptName = StringRoutines::StringTrimWhiteSpace(inputOutput.content);
  if (javascriptName == "MathJax") {
    inputOutput.interpretedCommand = HtmlRoutines::GetJavascriptMathjax("");
  }
}

std::string TopicElement::GetItemFinish(CalculatorHTML& owner) {
  std::stringstream out;
  if (this->type == this->tChapter) {
    if (owner.flagTopicTableStarted) {
      out << "\n</tbody>\n</table>\n</div><!--bodyItem-->";
    }
    if (owner.flagTopicSubSectionStarted) {
      out << "\n</div><!--bodySubsection-->";
    }
    if (owner.flagTopicSectionStarted) {
      out << "\n</div><!--bodySection-->";
    }
    if (owner.flagTopicChapterStarted) {
      out << "\n</div><!--bodyChapter-->";
    }
    owner.flagTopicSectionStarted = false;
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  }
  if (this->type == this->tSection) {
    if (owner.flagTopicTableStarted) {
      out << "\n</tbody>\n</table>\n</div><!--bodyItem-->";
    }
    if (owner.flagTopicSubSectionStarted) {
      out << "\n</div><!--bodySubsection-->";
    }
    if (owner.flagTopicSectionStarted) {
      out << "\n</div><!--bodySection-->";
    }
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  }
  if (this->type == this->tSubSection) {
    if (owner.flagTopicTableStarted) {
      out << "\n</tbody>\n</table>\n</div><!--bodyItem-->";
    }
    if (owner.flagTopicSubSectionStarted) {
      out << "\n</div><!--bodySubsection-->";
    }
    owner.flagTopicTableStarted = false;
  }
  return out.str();
}

std::string TopicElement::GetItemStart(CalculatorHTML& owner, bool doIncludeScoreButton, bool plainStyle) {
  std::stringstream out;
  out << this->GetItemFinish(owner);
  std::string theClass;
  if (this->type == this->tChapter) {
    theClass = "Chapter";
    owner.flagTopicChapterStarted = true;
    owner.flagTopicSectionStarted = false;
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  } else if (this->type == this->tSection) {
    theClass = "Section";
    if (!owner.flagTopicChapterStarted) {
      out << "\n<div class =\"bodyChapter\">";
    }
    owner.flagTopicChapterStarted = true;
    owner.flagTopicSectionStarted = true;
    owner.flagTopicSubSectionStarted = false;
    owner.flagTopicTableStarted = false;
  } else if (this->type == this->tSubSection) {
    theClass = "Subsection";
    if (!owner.flagTopicChapterStarted) {
      out << "\n<div class =\"bodyChapter\">";
    }
    if (!owner.flagTopicSectionStarted) {
      out << "\n<div class =\"bodySection\">";
    }
    owner.flagTopicChapterStarted = true;
    owner.flagTopicSectionStarted = true;
    owner.flagTopicSubSectionStarted = true;
    owner.flagTopicTableStarted = false;
  } else {
    if (!owner.flagTopicTableStarted) {
      if (!owner.flagTopicChapterStarted) {
        out << "\n<div class =\"bodyChapter\">";
      }
      if (!owner.flagTopicSectionStarted) {
        out << "\n<div class =\"bodySection\">";
      }
      if (!owner.flagTopicSubSectionStarted) {
        out << "\n<div class =\"bodySubsection\">";
      }
      owner.flagTopicChapterStarted = true;
      owner.flagTopicSectionStarted = true;
      owner.flagTopicSubSectionStarted = true;
      owner.flagTopicTableStarted = true;
      out
      << "\n<div class =\"bodyItem\">"
      << "\n<table class =\"topicList\">";
      out << "\n<colgroup><col><col><col><col><col></colgroup>";
      out << "\n<tbody>\n";
      if (!plainStyle) {
        out
        << "\n<tr> <th>Sub-Topic</th>"
        << "<th>Resource Links</th>"
        << "<th>Current Score</th>"
        << "<th>Deadlines</th>"
        << "</tr>";
      }
    }
    out << "<tr class =\"calculatorProblem\" "
    << "id =\"" << this->idBase64 << "\"" << ">\n";
    out << "  <td>\n";
    out << this->displayTitle;
    if (owner.flagIncludeStudentScores) {
      out << this->ToStringStudentScoreReportPanel();
    }
    out << "  </td>\n";
    out << "  <td>\n" << this->displayResourcesLinks << "</td>";
    out << "<td>";
    out << this->displayProblemLink;
    out << "  </td>\n";
    out << "  <td>";
    if (this->problemFileName == "") {
      out << "-";
    } else {
      out << this->displayScore;
    }
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
  if (doIncludeScoreButton) {
    out << this->ToStringStudentScoreReportPanel();
  }
  out  << "</td>";
  out << "<td>" << this->displayResourcesLinks << "</td>";
  out << "<td></td>";
  if (doIncludeScoreButton) {
    out << "<td>" << this->ToStringStudentScoreButton() << "</td>";
  } else {
    out << "<td></td>";
  }
  out << "<td class = \"deadlineCell\">" << this->displayDeadlinE << "</td>";
  out << "</tr></table></div>\n";
  out << "<div class = \"body" << theClass << "\" id = \"body" << this->idBase64 << "\">";
  return out.str();
}

int TopicElement::scoreButtonCounter = 0;

std::string TopicElement::ToStringStudentScoreReportPanel() {
  std::stringstream out;
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

std::string TopicElement::ToStringStudentScoreButton() {
  std::stringstream out;
  out << "<button class =\"studentScoresButton\" "
  << "onclick=\"toggleStudentScores"
  << "('studentScoresLoadReport" << TopicElement::scoreButtonCounter << "', "
  << "'scoresInCoursePage',"
  << "'studentScoresLoadReport"
  << TopicElement::scoreButtonCounter << "');\">"
  << "Scores</button>";
  return out.str();
}

std::string CalculatorHTML::GetSectionSelector() {
  if (
    !theGlobalVariables.UserDefaultHasProblemComposingRights() ||
    theGlobalVariables.UserStudentVieWOn()
  ) {
    return "";
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "";
  }
  std::stringstream out;
  out << "<sectionSelection>Sections: ";
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    out << "<input type =\"radio\" name =\"sectionSelector\" "
    << "onclick=\"populateTopicList("
    << "'"
    << this->databaseStudentSections[i]
    << "'"
    << ");\"";
    if (this->databaseStudentSections[i] == this->currentUseR.sectionComputed) {
      out << "checked";
    }
    out <<  ">"
    << this->databaseStudentSections[i]
    << "</input>";
  }
  out << "</sectionSelection>\n";
  return out.str();
}

void CalculatorHTML::InterpretLectureMaterials(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretLectureMaterials");
  std::stringstream out;
  if (this->flagUseJSON) {
    inputOutput.interpretedCommand = "<lectureList></lectureList>";
    return;
  }
  if (!this->LoadAndParseTopicList(out)) {
    inputOutput.interpretedCommand = out.str();
    return;
  }
  bool plainStyle = (inputOutput.GetKeyValue("topicListStyle") == "plain");
  out << "<div class = \"headChapter\">Lecture materials "
  << "<button id = \"buttonToggleCourseInfo\" class = \"buttonToggleTopics\" "
  << "onclick = \"toggleHeight(this,'tableWithLectureMaterialsFull')\">&#9650;</button>"
  << "<span style = \"font-weight:normal; font-size:small\">(&larr; click to (un)hide)</span><br>\n"
  << "</div>";
  out
  << "\n<div class =\"bodyChapter\" id =\"tableWithLectureMaterialsFull\">"
  << "\n<table class =\"lectureMaterials\">";
  out << "\n<colgroup><col><col><col></colgroup>";
  out << "\n<tbody>\n";
  if (!plainStyle) {
    out
    << "\n<tr><th>Sub-Topic</th>"
    << "<th>Resource Links</th>"
    << "</tr>";
  }
  this->topicLectureCounter = 0;
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    TopicElement currentTopic = this->theTopicS.theValues[i];
    currentTopic.ComputeLinks(*this, plainStyle);
    if (!currentTopic.flagHasLectureTag) {
      continue;
    }
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

bool CalculatorHTML::ComputeTopicListAndPointsEarned(std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("CalculatorHTML::ComputeTopicListAndPointsEarned");
  if (!this->LoadAndParseTopicList(commentsOnFailure)) {
    return false;
  }
  if (!this->LoadDatabaseInfo(commentsOnFailure)) {
    if (!this->flagUseJSON) {
      commentsOnFailure << "<span style =\"color:red\">Could not load your problem history.</span> <br>";
    } else {
      commentsOnFailure << "Error loading problem history. ";
    }
  }
  if (!this->PrepareSectionList(commentsOnFailure)) {
    if (!this->flagUseJSON) {
      commentsOnFailure << "<span style =\"color:red\">Error preparing section list.</span> <br>";
    } else {
      commentsOnFailure << "Error preparing section list. ";
    }
  }
  if (theGlobalVariables.flagDatabaseCompiled) {
    this->flagIncludeStudentScores =
    theGlobalVariables.UserDefaultHasAdminRights() &&
    !theGlobalVariables.UserStudentVieWOn() &&
    theGlobalVariables.userCalculatorRequestType != "templateNoLogin";
    HashedList<std::string, MathRoutines::HashString> gradableProblems;
    for (int i = 0; i < this->theTopicS.size(); i ++) {
      if (this->theTopicS.theValues[i].type == TopicElement::tProblem) {
        gradableProblems.AddOnTopNoRepetition(this->theTopicS.theValues[i].id);
        if (this->theTopicS.theValues[i].immediateChildren.size > 0) {
          crash << "Error: problem " << this->theTopicS.theValues[i].ToString()
          << " has children topics which is not allowed. "
          << crash;
        }
      }
    }
    this->currentUseR.ComputePointsEarned(gradableProblems, &this->theTopicS, commentsOnFailure);
  }
  this->initTopicElementNames();
  return true;
}

void CalculatorHTML::InterpretTopicList(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretTopicList");
  if (this->flagUseJSON) {
    inputOutput.interpretedCommand = "<topicList></topicList>";
    return;
  }
  std::stringstream out;
  if (!this->ComputeTopicListAndPointsEarned(out)) {
    inputOutput.interpretedCommand = out.str();
    return;
  }
  std::stringstream outFinal, outHead;
  if (theGlobalVariables.flagDatabaseCompiled) {
    if (this->currentUseR.pointsMax != 0) {
      double percent = 100 * this->currentUseR.pointsEarned.GetDoubleValue() /
      this->currentUseR.pointsMax.GetDoubleValue();
      outHead.precision(2);
      outHead << "<panelStudentScores>Total score: "
      << std::fixed << percent << "% = ";
      outHead << std::fixed << this->currentUseR.pointsEarned.GetDoubleValue()
      << " out of " << this->currentUseR.pointsMax.GetDoubleValue()
      << " points earned.</panelStudentScores>"
      << "<button id =\"buttonToggleCourseInfo\" class =\"buttonToggleTopics\" "
      << "onclick=\"toggleHeight(this,'bodyCourseInformation')\">&#9650;</button><br>\n";
      outHead << "<div class =\"bodySection\" id = \"bodyCourseInformation\">"
      << "<small>Includes problems without deadline, but not problems without weights.<br> "
      << "If a problem is assigned a new weight, your % score may drop. </small><br>";
      outHead << "<panelCourseInfo>" << this->currentUseR.ToStringCourseInfo() << "</panelCourseInfo></div><br>";
    }
  }
  out << "<panelProblemLinkStyleSelection>Problem links open in: ";
  out << "<input type =\"radio\" name =\"problemLinkStyleSelector\" onclick=\"setProblemLinkStyle('accordion');\" ";
  if (theGlobalVariables.GetWebInput("problemLinkStyle") == "accordion") {
    out << "checked";
  }
  out << ">same tab, under topics</input>";
  out << "<input type =\"radio\" name =\"problemLinkStyleSelector\" onclick=\"setProblemLinkStyle('sameWindow');\" ";
  if (theGlobalVariables.GetWebInput("problemLinkStyle") == "sameWindow") {
    out << "checked";
  }
  out << ">same tab, replace topics</input>";
  out << "<input type =\"radio\" name =\"problemLinkStyleSelector\" onclick=\"setProblemLinkStyle('newWindow');\" ";
  if (theGlobalVariables.GetWebInput("problemLinkStyle") == "newWindow") {
    out << "checked";
  }
  out << ">new tab</input>"
  << ". </panelProblemLinkStyleSelection>";
  bool plainStyle = (inputOutput.GetKeyValue("topicListStyle") == "plain");
  std::string desiredChapter = HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput("chapter"), false
  );
  std::string currentChapter = "";
  out << "\n\n\n<!--Topic list automatically generated from topic list: "
  << this->topicListFileName
  << ".-->";
  this->flagTopicChapterStarted = false;
  this->flagTopicSectionStarted = false;
  this->flagTopicSubSectionStarted = false;
  this->flagTopicTableStarted = false;
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    TopicElement& currentElt = this->theTopicS.theValues[i];
    if (currentElt.type == currentElt.tTexHeader) {
      continue;
    }
    if (currentElt.type == currentElt.tChapter) {
      currentChapter = currentElt.title;
    }
    if (desiredChapter != "") {
      if (currentChapter != desiredChapter) {
        continue;
      }
    }
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
  outFinal << outHead.str() << out.str();
  inputOutput.interpretedCommand = outFinal.str();
  std::stringstream topicListJS;
  topicListJS << "<script type =\"text/javascript\">";

  topicListJS << "var currentStudentSection =";
  if (theGlobalVariables.flagDatabaseCompiled) {
    topicListJS << "'" << this->currentUseR.sectionComputed << "'" << ";\n";
  } else {
    topicListJS << "''" << ";\n";
  }
  topicListJS << "var studentSections =[";
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    topicListJS
    << "'"
    << HtmlRoutines::ConvertStringToURLString(this->databaseStudentSections[i], false)
    << "'";
    if (i != this->databaseStudentSections.size - 1) {
      topicListJS << ", ";
    }
  }
  topicListJS << "];\n";
  topicListJS << "var listTopics =[";
  for (int i = 0; i < this->theTopicS.size(); i ++) {
    TopicElement& currentE = this->theTopicS.theValues[i];
    if (currentE.type == currentE.tTexHeader) {
      continue;
    }
    topicListJS << "{id: '" << currentE.idBase64 << "', ";
    topicListJS << "type: '";
    if (currentE.type == currentE.tChapter) {
      topicListJS << "chapter";
    } else if (currentE.type == currentE.tSection) {
      topicListJS << "section";
    } else if (currentE.type == currentE.tSubSection) {
      topicListJS << "subSection";
    } else if (currentE.type == currentE.tError) {
      topicListJS << "error";
    } else {
      topicListJS << "problem";
    }
    topicListJS << "', ";
    //////////////////////////////////////////////////
    topicListJS << "idButton: '" << currentE.idDeadlineButton << "', ";
    //////////////////////////////////////////////////
    topicListJS << "deadlines: {";
    bool found = false;
    for (int j = 0; j < currentE.deadlinesPerSectioN.size; j ++) {
      if (currentE.deadlinesPerSectioN[j] == "") {
        continue;
      }
      if (found) {
        topicListJS << ", ";
      }
      found = true;
      topicListJS << "'" << this->databaseStudentSections[j] << "': "
      << "'" << currentE.deadlinesPerSectioN[j] << "'";
    }
    topicListJS << "}, ";
    //////////////////////////////////////////////////
    topicListJS << "deadlinesFormatted: {";
    found = false;
    for (int j = 0; j < currentE.deadlinesPerSectionFormatted.size; j ++) {
      if (currentE.deadlinesPerSectionFormatted[j] == "") {
        continue;
      }
      if (found) {
        topicListJS << ", ";
      }
      found = true;
      topicListJS << "'" << this->databaseStudentSections[j] << "': "
      << "'" << currentE.deadlinesPerSectionFormatted[j] << "'";
    }
    topicListJS << "}, ";
    ////////////////////////////
    topicListJS << "isInherited: {";
    for (int j = 0; j < currentE.deadlinesAreInherited.size; j ++) {
      topicListJS << "'" << this->databaseStudentSections[j] << "': ";
      if (currentE.deadlinesAreInherited[j]) {
        topicListJS << "true";
      } else {
        topicListJS << "false";
      }
      if (j != currentE.deadlinesAreInherited.size - 1) {
        topicListJS << ", ";
      }
    }
    topicListJS << "}, ";
    //////////////////////////////////////////////////
    topicListJS << "immediateChildren: [";
    for (int j = 0; j < currentE.immediateChildren.size; j ++) {
      topicListJS << currentE.immediateChildren[j];
      if (j != currentE.immediateChildren.size - 1) {
        topicListJS << ", ";
      }
    }
    topicListJS << "]";
    //////////////////////////////////////////////////
    topicListJS << "}";
    if (i != this->theTopicS.size() - 1) {
      topicListJS << ", ";
    }
  }
  topicListJS << "];\n";
  topicListJS << "</script>";
  this->topicListJavascriptWithTag = topicListJS.str();
}

void TopicElement::ComputeHomework(CalculatorHTML& owner) {
  MacroRegisterFunctionWithName("TopicElement::ComputeHomework");
  if (this->sourceHomework.size == 0) {
    return;
  }
  std::stringstream homeworkStream;
  homeworkStream << "title=" << HtmlRoutines::ConvertStringToURLString(this->title, false) << "&";
  int sourceHomeworkCounter = 0;
  for (int i = 0; i < owner.sourcesHomeworkHeaders.size; i ++) {
    sourceHomeworkCounter ++;
    homeworkStream << "file" << sourceHomeworkCounter
    << "=" << HtmlRoutines::ConvertStringToURLString(owner.sourcesHomeworkHeaders[i], false) << "&isSolutionFile"
    << sourceHomeworkCounter << "=false&";
  }
  for (int i = 0; i < this->sourceHomework.size; i ++) {
    sourceHomeworkCounter ++;
    homeworkStream << "file" << sourceHomeworkCounter
    << "=" << HtmlRoutines::ConvertStringToURLString(this->sourceHomework[i], false) << "&";
    if (i < this->sourceHomeworkIsSolution.size) {
      if (this->sourceHomeworkIsSolution[i]) {
        homeworkStream << "isSolutionFile" << sourceHomeworkCounter << "=true&";
      } else {
        homeworkStream << "isSolutionFile" << sourceHomeworkCounter << "=false&";
      }
    } else {
      homeworkStream << "ERROR=ThisShouldNotBeGenerated&";
    }
  }
  this->queryHomework = homeworkStream.str();
}

void TopicElement::ComputeSlides(CalculatorHTML& owner) {
  MacroRegisterFunctionWithName("TopicElement::ComputeSlides");
  if (this->sourceSlides.size == 0) {
    return;
  }
  std::stringstream sourcesStream;
  sourcesStream << "title=" << HtmlRoutines::ConvertStringToURLString(this->title, false) << "&";

  int sourceSlidesCounter = 0;
  for (int i = 0; i < owner.slidesSourcesHeaders.size; i ++) {
    sourceSlidesCounter ++;
    sourcesStream << "file" << sourceSlidesCounter
    << "=" << HtmlRoutines::ConvertStringToURLString(owner.slidesSourcesHeaders[i], false) << "&";
  }
  for (int i = 0; i < this->sourceSlides.size; i ++) {
    sourceSlidesCounter ++;
    sourcesStream << "file" << sourceSlidesCounter
    << "=" << HtmlRoutines::ConvertStringToURLString(this->sourceSlides[i], false) << "&";
  }
  this->querySlides = sourcesStream.str();
}

void TopicElement::ComputeLinks(CalculatorHTML& owner, bool plainStyle) {
  MacroRegisterFunctionWithName("TopicElement::ComputeLinks");
  (void) plainStyle;
  if (this->displayProblemLink != "") {
    return;
  }
  int depth = 3;
  if (this->type == this->tChapter) {
    depth = 0;
  }
  if (this->type == this->tSection) {
    depth = 1;
  }
  if (this->type == this->tSubSection) {
    depth = 2;
  }
  std::stringstream problemLabel;
  for (int i = 0; i < depth + 1; i ++) {
    problemLabel << this->problemNumber[i];
    problemLabel << ".";
  }
  problemLabel << " ";
  this->problemNumberString = problemLabel.str();
  std::string titleWithLectureNumber = this->title;
  this->flagHasLectureTag = false;
  int lectureTagStart = static_cast<int>(titleWithLectureNumber.find("<lectureTag>"));
  if (lectureTagStart >= 0) {
    int lectureTagFinish = static_cast<int>(titleWithLectureNumber.find("</lectureTag>"));
    if (lectureTagFinish >= 0) {
      owner.topicLectureCounter ++;
      lectureTagFinish += 13;
      std::stringstream newTitle;
      newTitle
      << titleWithLectureNumber.substr(0, static_cast<unsigned>(lectureTagStart))
      << "<lectureTag>&#8466;"
      << "</lectureTag>"
      << "Lecture "
      << owner.topicLectureCounter
      << "."
      << titleWithLectureNumber.substr(static_cast<unsigned>(lectureTagFinish));
      this->flagHasLectureTag = true;
      titleWithLectureNumber = newTitle.str();
    }
  }
  if (this->title == "") {
    this->displayTitle = this->problemNumberString + "-" ;//+ "<br>DEBUG: "+this->ToString();
  } else {
    this->displayTitle = this->problemNumberString + titleWithLectureNumber;//+ "<br>DEBUG: "+this->ToString();
  }
  if (this->video == "" || this->video == "-" || this->video == "--") {
    this->displayVideoLink = "";
  } else {
    this->displayVideoLink = "<a href=\"" + this->video + "\" class =\"videoLink\" class =\"videoLink\" target =\"_blank\">Video</a>";
  }
  if (this->videoHandwritten == "" || this->videoHandwritten == "-" || this->videoHandwritten == "--") {
    this->displayVideoHandwrittenLink = "";
  } else {
    this->displayVideoHandwrittenLink = "<a href=\"" +
    this->videoHandwritten + "\" class =\"videoLink\" class =\"videoLink\" target =\"_blank\">Video <b>(H)</b></a>";
  }
  if (this->handwrittenSolution != "") {
    this->displayHandwrittenSolution = "<a href=\"" +
    this->handwrittenSolution + "\" class =\"slidesLink\">Handwritten solutions</a>";
  }
  this->ComputeSlides(owner);
  this->ComputeHomework(owner);
  bool problemSolved = false;
  bool returnEmptyStringIfNoDeadline = false;
  if (this->problemFileName == "") {
    if (this->type == this->tProblem) {
      this->displayProblemLink = "(theory)";
    } else {
      this->displayProblemLink = "";
    }
    this->displayScore = "";
    this->displayModifyWeight = "";
    problemSolved = false;
    returnEmptyStringIfNoDeadline = true;
  } else {
    //std::string theRawSQLink = theGlobalVariables.DisplayNameExecutable +
    //"?request=scoredQuiz&fileName=" + this->problem;
    std::string theRawExerciseLink;
    theRawExerciseLink = theGlobalVariables.DisplayNameExecutable + "?request=exercise&fileName=" + this->problemFileName;
    this->displayProblemLink = owner.ToStringLinkFromFileName(this->problemFileName);
    this->displayScore = owner.ToStringProblemScoreShort(this->problemFileName, problemSolved);
    this->displayModifyWeight = owner.ToStringProblemWeightButton(this->problemFileName);
  }
  if (this->problemFileName == "" && this->type == this->tProblem) {
    this->displayDeadlinE = "";
  } else {
    this->displayDeadlinE = owner.ToStringDeadline(
      this->id, problemSolved, returnEmptyStringIfNoDeadline, (this->type != this->tProblem)
    );
  }
  if (
    theGlobalVariables.UserDefaultHasAdminRights() &&
    !theGlobalVariables.UserStudentVieWOn() &&
    theGlobalVariables.userCalculatorRequestType != "templateNoLogin"
  ) {
    if (this->displayDeadlinE == "") {
      this->displayDeadlinE += "Deadline";
    }
    owner.ComputeDeadlineModifyButton(
      *this, problemSolved, this->type == this->tSubSection || this->type == this->tSection || this->type == this->tChapter
    );
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
  << this->displayVideoLink << this->displayVideoHandwrittenLink
  << this->displayHandwrittenSolution;
  this->displayResourcesLinks = displayResourcesLinksStream.str();
  if (this->problemFileName != "") {
    owner.NumProblemsFound ++;
  }
  if (this->video != "") {
    owner.NumVideosWithSlidesFound ++;
  }
  if (this->videoHandwritten != "") {
    owner.NumVideosHandwrittenFound ++;
  }
}

JSData TopicElement::ToJSON(CalculatorHTML& owner) {
  MacroRegisterFunctionWithName("TopicElement::ToJSON");
  JSData output;
  output["title"] = this->title;
  switch (this->type) {
    case TopicElement::tChapter:
      output["type"] = std::string("chapter");
      break;
    case TopicElement::tSection:
      output["type"] = std::string("section");
      break;
    case TopicElement::tSubSection:
      output["type"] = std::string("subSection");
      break;
    case TopicElement::tProblem:
      output["type"] = std::string("problem");
      break;
    case TopicElement::tError:
      output["type"] = std::string("error");
      break;
    default:
      output["type"] = std::string("Not documented");
      break;
  }
  output["children"].theType = JSData::token::tokenArray;
  this->ComputeLinks(owner, true);
  if (this->type == TopicElement::tProblem && this->immediateChildren.size > 0) {
    crash << "Error: Problem " << this->ToString() << " reported to have children topic elements: "
    << this->immediateChildren.ToStringCommaDelimited() << crash;
  }
  for (int i = 0; i < this->immediateChildren.size; i ++) {
    TopicElement& currentChild = owner.theTopicS.theValues[this->immediateChildren[i]];
    output["children"].theList.AddOnTop(currentChild.ToJSON(owner));
  }
  output["problemNumberString"] = this->problemNumberString;
  output["video"] = this->video;
  output["videoHandwritten"] = this->videoHandwritten;
  if (this->querySlides != "") {
    output["querySlides"] = this->querySlides;
  }
  if (this->queryHomework != "") {
    output["queryHomework"] = this->queryHomework;
  }
  output[DatabaseStrings::labelDeadlines] = this->deadlinesPerSectioN;
  if (!theGlobalVariables.UserDefaultHasProblemComposingRights()) {
    std::string theDeadline = owner.GetDeadlineNoInheritance(this->id);
    output[WebAPI::problem::deadlineSingle] = theDeadline;
  }
  output["handwrittenSolution"] = this->handwrittenSolution;

  output[WebAPI::problem::fileName] = this->problemFileName;
  output[WebAPI::problem::idProblem] = this->id;
  if (theGlobalVariables.flagDatabaseCompiled) {
    if (owner.currentUseR.theProblemData.Contains(this->problemFileName)) {
      ProblemData& currentData = owner.currentUseR.theProblemData.GetValueCreate(this->problemFileName);
      output["correctlyAnswered"] = currentData.numCorrectlyAnswered;
      output["totalQuestions"] = currentData.theAnswers.size();
      Rational currentWeight;
      std::string currentWeightAsGivenByInstructor;
      currentData.flagProblemWeightIsOK = currentData.adminData.GetWeightFromCoursE(
        owner.currentUseR.courseComputed, currentWeight, &currentWeightAsGivenByInstructor
      );
      if (currentData.flagProblemWeightIsOK) {
        output["weight"] = currentWeightAsGivenByInstructor;
      }
    }
  }
  return output;
}

std::string TopicElement::ToString() const {
  std::stringstream out;
  out << this->title << ", id: " << this->id << " ";
  if (this->title == "") {
    out << "-";
  }
  if (this->type == this->tProblem) {
    out << "(problem)";
  }
  if (this->type == this->tChapter) {
    out << "(chapter)";
  }
  if (this->type == this->tSection) {
    out << "(section)";
  }
  if (this->type == this->tSubSection) {
    out << "(subsection)";
  }
  out << ". Index in parent: " << this->indexInParent;
  out << ". Parents: " << this->parentTopics.ToStringCommaDelimited()
  << ". Immediate children: " << this->immediateChildren.ToStringCommaDelimited() << ". ";
  return out.str();
}
