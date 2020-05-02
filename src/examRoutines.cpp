// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"
#include "calculator_html_functions.h"
#include "database.h"
#include "general_time_date.h"
#include "calculator_html_interpretation.h"
#include "web_api.h"
#include "math_extra_latex_routines.h"
#include <iomanip>
#include "database.h"
#include "string_constants.h"

std::string CalculatorHTML::stringScoredQuizzes = "Quiz";
std::string CalculatorHTML::stringPracticE = "Practice";
std::string CalculatorHTML::stringProblemLink = "Problem";

std::string SyntacticElementHTML::Tags::calculator = "calculator";
std::string SyntacticElementHTML::Tags::calculatorHidden = "calculatorHidden";
std::string SyntacticElementHTML::Tags::calculatorSolution = "calculatorSolution";
std::string SyntacticElementHTML::Tags::calculatorExamProblem = "calculatorExamProblem";
std::string SyntacticElementHTML::Tags::calculatorAnswer = "calculatorAnswer";

CalculatorHTML::CalculatorHTML() {
  this->NumAttemptsToInterpret = 0;
  this->NumAnswerIdsMathquilled = 0;
  this->MaxInterpretationAttempts = 25;
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
  this->topics.owner = this;
}

bool CalculatorHTML::mergeProblemWeight(
  const JSData& inputJSON,
  MapList<std::string, ProblemData, MathRoutines::HashString>& outputAppendProblemInfo,
  bool checkFileExistence,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::LoadProblemWeightsAppend");
  (void) commentsOnFailure;
  if (inputJSON.theType != JSData::token::tokenObject) {
    return true;
  }
  global << Logger::green << "About to merge problem weight: " << inputJSON.toString() << Logger::endL;
  ProblemData emptyData;
  std::string currentCourse = global.userDefault.courseComputed;
  for (int i = 0; i < inputJSON.objects.size(); i ++) {
    std::string currentProblemName = inputJSON.objects.theKeys[i];
    if (checkFileExistence) {
      if (!FileOperations::fileExistsVirtualCustomizedReadOnly(
        currentProblemName, commentsOnFailure
      )) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Problem "
          << currentProblemName << " <b>does not appear to exist</b>. ";
        }
        return false;
      }
    }
    JSData currentProblem = inputJSON.objects.theValues[i];
    if (!outputAppendProblemInfo.contains(currentProblemName)) {
      outputAppendProblemInfo.setKeyValue(currentProblemName, emptyData);
    }
    ProblemData& currentProblemValue = outputAppendProblemInfo.GetValueCreate(currentProblemName);
    JSData& currentWeight = currentProblem[DatabaseStrings::labelProblemWeight];
    if (currentWeight.theType == JSData::token::tokenString) {
      currentProblemValue.adminData.problemWeightsPerCoursE.setKeyValue(currentCourse, currentWeight.theString);
    } else if (currentWeight.theType == JSData::token::tokenObject) {
      for (int i = 0; i < currentWeight.objects.size(); i ++) {
        if (currentWeight.objects.theValues[i].theType != JSData::token::tokenString) {
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << "Failed to extract weight from: "
            << currentWeight.objects.theValues[i]
            << " in weight: " << currentWeight.toString();
          }
          return false;
        }
        currentProblemValue.adminData.problemWeightsPerCoursE.setKeyValue(
          currentWeight.objects.theKeys[i],
          currentWeight.objects.theValues[i].theString
        );
      }
    } else {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Could extract weight from "
        << currentWeight.toString(nullptr) << ". Your input was: "
        << inputJSON.toString(nullptr);
      }
      return false;
    }
  }
  return true;
}

bool CalculatorHTML::mergeProblemDeadline(
  const JSData& inputJSON,
  MapList<std::string, ProblemData, MathRoutines::HashString>& outputAppendProblemInfo,
  std::stringstream *commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::LoadProblemWeightsAppend");
  (void) commentsOnFailure;
  if (inputJSON.theType != JSData::token::tokenObject) {
    return true;
  }
  ProblemData emptyData;
  for (int i = 0; i < inputJSON.objects.size(); i ++) {
    std::string currentProbName = inputJSON.objects.theKeys[i];
    JSData currentProblem = inputJSON.objects.theValues[i];
    if (currentProbName == "") {
      continue;
    }
    if (!outputAppendProblemInfo.contains(currentProbName)) {
      outputAppendProblemInfo.setKeyValue(currentProbName, emptyData);
    }
    ProblemData& currentProblemValue = outputAppendProblemInfo.GetValueCreate(currentProbName);
    JSData& currentDeadlines = currentProblem[DatabaseStrings::labelDeadlines];
    if (currentDeadlines.theType == JSData::token::tokenObject) {
      for (int j = 0; j < currentDeadlines.objects.size(); j ++) {
        currentProblemValue.adminData.deadlinesPerSection.setKeyValue(
          currentDeadlines.objects.theKeys[j],
          currentDeadlines.objects.theValues[j].theString
        );
      }
    } else {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Unexpected deadline format. ";
      }
      return false;
    }
  }
  return true;
}

JSData CalculatorHTML::toJSONDeadlines(
  MapList<std::string, ProblemData, MathRoutines::HashString>& inputProblemInfo
) {
  MacroRegisterFunctionWithName("CalculatorHTML::toJSONDeadlines");
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
      std::string currentDeadline = StringRoutines::stringTrimWhiteSpace(
        currentProblem.deadlinesPerSection.theValues[j]
      );
      if (currentDeadline == "") {
        continue;
      }
      std::string currentSection = StringRoutines::stringTrimWhiteSpace(
        currentProblem.deadlinesPerSection.theKeys[j]
      );
      currentProblemJSON[DatabaseStrings::labelDeadlines][currentSection] = currentDeadline;
    }
    output[currentProblemName] = currentProblemJSON;
  }
  return output;
}

QuerySet CalculatorHTML::toQuerySetProblemWeights(
  MapList<std::string, ProblemData, MathRoutines::HashString>& inputProblemInfo
) {
  MacroRegisterFunctionWithName("CalculatorHTML::toQuerySetProblemWeights");
  QuerySet output;
  output.nestedLabels.addOnTop(DatabaseStrings::labelProblemWeight);
  for (int i = 0; i < inputProblemInfo.size(); i ++) {
    ProblemDataAdministrative& currentProblem = inputProblemInfo.theValues[i].adminData;
    if (currentProblem.problemWeightsPerCoursE.size() == 0) {
      continue;
    }
    std::string currentProblemName = inputProblemInfo.theKeys[i];
    JSData currentProblemJSON;
    for (int j = 0; j < currentProblem.problemWeightsPerCoursE.size(); j ++) {
      std::string currentWeight = StringRoutines::stringTrimWhiteSpace(
        currentProblem.problemWeightsPerCoursE.theValues[j]
      );
      if (currentWeight == "") {
        continue;
      }
      std::string currentCourse = StringRoutines::stringTrimWhiteSpace(
        currentProblem.problemWeightsPerCoursE.theKeys[j]
      );
      currentProblemJSON[currentCourse] = currentWeight;
    }
    JSData currentWeight;
    currentWeight[DatabaseStrings::labelProblemWeight] = currentProblemJSON;
    output.value[currentProblemName] = currentWeight;
  }
  return output;
}

bool CalculatorHTML::mergeOneProblemAdminData(
  const std::string& inputProblemName,
  ProblemData& inputProblemInfo,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::mergeOneProblemAdminData");
  if (!this->topics.theTopics.contains(inputProblemName)) {
    commentsOnFailure << "Did not find " << inputProblemName
    << " among the list of topics/problems. ";
    if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
      commentsOnFailure << "The topics are: " << this->topics.theTopics.toStringHtml();
    }
    return false;
  }
  if (!this->currentUseR.theProblemData.contains(inputProblemName)) {
    this->currentUseR.theProblemData.setKeyValue(inputProblemName, inputProblemInfo);
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
    this->databaseStudentSections.addOnTopNoRepetition(incomingDeadlines.theKeys[i]);
  }
  ////////////////////////////////////////////
  for (int i = 0; i < incomingDeadlines.size(); i ++) {
    currentDeadlines.setKeyValue(incomingDeadlines.theKeys[i], incomingDeadlines.theValues[i]);
  }
  for (int i = 0; i < incomingWeightS.size(); i ++) {
    currentWeightS.setKeyValue(incomingWeightS.theKeys[i], incomingWeightS.theValues[i]);
  }
  return true;
}

bool CalculatorHTML::mergeProblemWeightAndStore(
  std::string& incomingProblemInfo, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::MergeProblemInfoInDatabase");
  JSData theProblemJSON;
  if (!theProblemJSON.readstring(incomingProblemInfo, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse your input. ";
    }
    return false;
  }
  MapList<std::string, ProblemData, MathRoutines::HashString> incomingProblems;
  if (!this->mergeProblemWeight(theProblemJSON, incomingProblems, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse your request";
    }
    return false;
  }
  return this->storeProblemWeights(incomingProblems, commentsOnFailure);
}

bool CalculatorHTML::mergeProblemDeadlineAndStore(
  std::string& incomingProblemInfo, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::mergeProblemDeadlineAndStore");
  JSData theProblemJSON;
  if (!theProblemJSON.readstring(incomingProblemInfo, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse your input. ";
    }
    return false;
  }
  MapList<std::string, ProblemData, MathRoutines::HashString> incomingProblems;
  if (!this->mergeProblemDeadline(theProblemJSON, incomingProblems, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse your request";
    }
    return false;
  }
  return this->storeProblemDeadlines(incomingProblems, commentsOnFailure);
}

bool CalculatorHTML::storeProblemWeights(
  MapList<std::string, ProblemData, MathRoutines::HashString>& toStore,
  std::stringstream *commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  QueryExact weightFinder;
  weightFinder.collection = DatabaseStrings::tableProblemWeights;
  weightFinder.SetLabelValue(
    DatabaseStrings::labelProblemWeightsSchema,
    global.userDefault.problemWeightSchema
  );
  QuerySet updateQuery = this->toQuerySetProblemWeights(toStore);
  if (!Database::get().UpdateOne(weightFinder, updateQuery, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update weight schema. ";
      global << Logger::red
      << "Failed to update weight schema with update query: "
      << updateQuery.toStringDebug() << Logger::endL;
    }
    return false;
  }
  return true;
}

bool CalculatorHTML::storeProblemDeadlines(
  MapList<std::string, ProblemData, MathRoutines::HashString>& toStore,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  QueryExact deadlineSchema;
  deadlineSchema.collection = DatabaseStrings::tableDeadlines;
  deadlineSchema.SetLabelValue(DatabaseStrings::labelDeadlinesSchema, global.userDefault.deadlineSchema);
  QuerySet updateQuery = this->toJSONDeadlines(toStore);
  if (!Database::get().UpdateOne(deadlineSchema, updateQuery, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update deadline schema. ";
    }
    return false;
  }
  return true;
}

bool CalculatorHTML::loadDatabaseInfo(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::loadDatabaseInfo");
  if (!global.flagDatabaseCompiled) {
    comments << "Database not available (cannot load problem info). ";
    return false;
  }
  this->currentUseR.::UserCalculatorData::operator=(global.userDefault);
  if (!this->prepareSectionList(comments)) {
    return false;
  }
  if (this->currentUseR.problemDataJSON.objects.size() != 0) {
    if (!this->currentUseR.interpretDatabaseProblemDataJSON(this->currentUseR.problemDataJSON, comments)) {
      comments << "Failed to interpret user's problem saved data. ";
      return false;
    }
  } else {
    if (!this->currentUseR.interpretDatabaseProblemData(this->currentUseR.problemDataStrinG, comments)) {
      comments << "Failed to interpret user's problem saved data. ";
      return false;
    }
  }
  if (!this->mergeProblemWeight(
    this->currentUseR.problemWeights,
    this->currentUseR.theProblemData,
    false,
    &comments
  )) {
    comments << "Failed to load problem weights. ";
    return false;
  }
  if (!this->mergeProblemDeadline(
    this->currentUseR.deadlines, this->currentUseR.theProblemData, &comments
  )) {
    comments << "Failed to load problem deadlines. ";
    return false;
  }

  if (this->currentUseR.theProblemData.contains(this->fileName)) {
    this->theProblemData = this->currentUseR.theProblemData.GetValueCreate(this->fileName);
  }
  global.userDefault = this->currentUseR;
  return true;
}

bool CalculatorHTML::loadMe(
  bool doLoadDatabase,
  const std::string& inputRandomSeed,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::loadMe");
  if (!FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
    this->fileName, this->RelativePhysicalFileNameWithFolder, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get physical file name from "
      << this->fileName << ". ";
    }
    return false;
  }
  (void) doLoadDatabase;
  if (!FileOperations::loadFileToStringVirtualCustomizedReadOnly(
    this->fileName, this->inputHtml, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<br>User-input file name: [<b>" << this->fileName
      << "</b>]. ";
    }
    return false;
  }
  this->flagIsForReal = global.userRequestRequiresLoadingRealExamData();
  if (global.flagDatabaseCompiled) {
    this->topicListFileName = HtmlRoutines::convertURLStringToNormal(
      global.getWebInput(WebAPI::problem::topicList), false
    );
    if (doLoadDatabase) {
      std::stringstream errorStream;
      this->loadDatabaseInfo(errorStream);
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << errorStream.str();
      }
      for (int i = 0; i < this->topics.theTopics.size(); i ++) {
        this->computeDeadlinesAllSectionsNoInheritance(
          this->topics.theTopics.theValues[i]
        );
      }
    }
  }
  this->theProblemData.checkConsistency();
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
  double startTime = global.getElapsedSeconds();
  this->loadCurrentProblemItem(needToLoadDatabaseMayIgnore, desiredRandomSeed, commentsOnFailure);
  if (!this->flagLoadedSuccessfully) {
    return WebAPI::problem::failedToLoadProblem;
  }
  std::stringstream out;
  if (!this->interpretHtml(commentsOnFailure)) {
    out << "<b>Failed to interpret file: " << this->fileName << "</b>. ";
    out << "<br>We limit the number of generation attemps to "
    << this->MaxInterpretationAttempts << " for performance reasons; "
    << "with bad luck, some finicky problems require more. "
    << "Random seeds tried: "
    << this->randomSeedsIfInterpretationFails.toStringCommaDelimited()
    << "<br> <b>Please refresh the page.</b><br>";
    if (!this->flagIsForReal) {
      out
      << "If you specified the problem through the 'problem link' link, "
      << "please go back to the course page. ";
    } else {
      out << "<b>Your random seed must have been reset. </b>";
    }
    out << "<br><b style ='color:red'>If the problem persists after a couple of page refreshes, "
    << "it's a bug. Please take a screenshot and email the site administrator/your instructor. </b>";
    out
    << "Generated in "
    << MathRoutines::ReducePrecision(global.getElapsedSeconds() - startTime)
    << " second(s). ";
    return out.str();
  }
  if (this->flagDoPrependProblemNavigationBar) {
    out << this->outputProblemNavigatioN;
  }
  out << this->outputHtmlBodyNoTag;
  return out.str();
}

void CalculatorHTML::loadFileNames() {
  this->fileName = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::fileName), false);
  this->courseHome = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::courseHome), false);
  this->topicListFileName = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::topicList), false);
}

void CalculatorHTML::loadCurrentProblemItem(
  bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CalculatorHTML::loadCurrentProblemItem");
  this->loadFileNames();
  this->flagLoadedSuccessfully = false;
  if (global.userGuestMode()) {
    needToLoadDatabaseMayIgnore = false;
  }
  this->flagLoadedSuccessfully = true;
  if (this->fileName == "") {
    this->flagLoadedSuccessfully = false;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b>No problem file name found. </b>";
    }
  }
  if (!this->loadMe(needToLoadDatabaseMayIgnore, inputRandomSeed, commentsOnFailure)) {
    this->flagLoadedSuccessfully = false;
  }
  this->theProblemData.checkConsistency();
}

bool CalculatorHTML::isStateModifierApplyIfYes(SyntacticElementHTML& inputElt) {
  MacroRegisterFunctionWithName("CalculatorHTML::isStateModifierApplyIfYes");
  if (inputElt.syntacticRole != "command") {
    return false;
  }
  std::string tagClass = inputElt.getKeyValue("class");
  if (tagClass == "setCalculatorExamHome") {
    this->flagIsExamHome = true;
    this->flagIsExamProblem = false;
    global.setWebInput(WebAPI::problem::courseHome, HtmlRoutines::convertStringToURLString(this->fileName, false));
  }
  if (tagClass == "setCalculatorExamProblem") {
    this->flagIsExamHome = false;
    this->flagIsExamProblem = true;
    return true;
  }
  return false;
}

std::string Answer::toString() {
  MacroRegisterFunctionWithName("Answer::toString");
  std::stringstream out;
  out << "Answer id: " << this->answerId;
  out << "<br>Answer commands on give-up: " << this->commandsNoEnclosureAnswerOnGiveUpOnly;
  return out.str();
}

std::string CalculatorHTML::toStringCalculatorProblemSourceFromFileName(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringCalculatorProblemSourceFromFileName");
  std::stringstream out;
  out //<< "<span class =\"calculatorExamProblem\">\n"
  << "Title: " << theFileName << "\n"
  << "Problem: " << theFileName << "\n"
  //<< "\n</span>"
  ;
  return out.str();
}

void CalculatorHTML::interpretGenerateLink(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretGenerateLink");
  inputOutput.interpretedCommand = this->toStringProblemInfo(this->cleanUpFileName(inputOutput.content));
}

std::string CalculatorHTML::toStringLinkCurrentAdmin(
  const std::string& displayString, bool setDebugFlag, bool includeRandomSeed
) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringLinkCurrentAdmin");
  if (!global.userDefaultHasAdminRights()) {
    return "";
  }
  std::stringstream out;
  out << "<a class =\"linkStandardButtonLike\" href=\"" << global.DisplayNameExecutable << "?request="
  << global.requestType << "&";
  std::string urledProblem = HtmlRoutines::convertStringToURLString(this->fileName, false);
  List<std::string> randomSeedContainer;
  randomSeedContainer.addOnTop(WebAPI::problem::randomSeed);
  out << "fileName=" << urledProblem << "&"
  << global.toStringCalculatorArgumentsNoNavigation(&randomSeedContainer);
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
  if (global.userStudentVieWOn()) {
    out << "studentView=true&";
    if (global.getWebInput("studentSection") != "") {
      out << "studentSection=" << global.getWebInput("studentSection") << "&";
    }
  }
  out << "\">" << displayString << "</a>";
  return out.str();
}

std::string CalculatorHTML::toStringLinkFromFileName(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringLinkFromFileName");
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
  std::string urledProblem = HtmlRoutines::convertStringToURLString(theFileName, false);
  refStreamNoRequest << global.toStringCalculatorArgumentsNoNavigation(nullptr)
  << "fileName=" << urledProblem << "&";
  if (global.userStudentVieWOn()) {
    refStreamNoRequest << "studentView=true&";
    if (global.getWebInput("studentSection") != "") {
      refStreamNoRequest << "studentSection="
      << global.getWebInput("studentSection") << "&";
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
    out << "<a href=\"" << global.DisplayNameExecutable
    << "?request=template&" << refStreamNoRequest.str() << "\">" << "Home" << "</a> ";
    return out.str();
  }
  if (!global.userGuestMode()) {
    refStreamExercise << global.DisplayNameExecutable
    << "?request=exercise&" << refStreamNoRequest.str();
    refStreamForReal << global.DisplayNameExecutable
    << "?request=scoredQuiz&" << refStreamNoRequest.str();
  } else {
    refStreamExercise << "?request=exerciseNoLogin&" << refStreamNoRequest.str();
  }
  if (!global.userGuestMode()) {
    out << "<b><a class =\"problemLinkQuiz\" href=\"" << refStreamForReal.str() << "\">"
    << CalculatorHTML::stringScoredQuizzes << "</a></b>";
  }
  out << "<a class =\"problemLinkPractice\" href=\"" << refStreamExercise.str() << "\">"
  << CalculatorHTML::stringPracticE << "</a>";
  return out.str();
}

std::string CalculatorHTML::toStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinksFromFileName");
  std::stringstream out;
  out << this->toStringLinkFromFileName(theFileName);
  out << this->toStringProblemScoreFull(theFileName);
  if (global.flagDatabaseCompiled) {
    bool problemAlreadySolved = false;
    if (this->currentUseR.theProblemData.contains(theFileName)) {
      ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
      if (theProbData.numCorrectlyAnswered >= theProbData.theAnswers.size()) {
        problemAlreadySolved = true;
      }
    }
    out << this->toStringDeadline(theFileName, problemAlreadySolved, false, false);
  }
  std::string finalStringToDisplay = stringToDisplay;
  if (finalStringToDisplay == "") {
    finalStringToDisplay = FileOperations::getFileNameFromFileNameWithPath(theFileName);
  }
  out << finalStringToDisplay;
  return out.str();
}

bool CalculatorHtmlFunctions::innerInterpretProblemGiveUp(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerInterpretProblemGiveUp");
  if (input.size() != 4) {
    return theCommands << "Expected 3 arguments: problem filename, answer id and randomSeed string. ";
  }
  std::string oldProblem = global.getWebInput(WebAPI::problem::fileName);
  std::string testedProblem = input[1].toString();
  global.setWebInput(WebAPI::problem::fileName, testedProblem);
  std::string randomSeed = input[3].toString();
  std::string answerId = input[2].toString();
  global.setWebInput(WebAPI::problem::calculatorAnswerPrefix + answerId, "not used");
  JSData result = WebAPIResponse::getAnswerOnGiveUp(randomSeed, nullptr, nullptr, false);
  global.webArguments.RemoveKey(WebAPI::problem::calculatorAnswerPrefix + answerId);
  global.setWebInput(WebAPI::problem::fileName, oldProblem);
  std::stringstream out;
  out << WebAPI::problem::answerGenerationSuccess
  << ":" << result[WebAPI::problem::answerGenerationSuccess] << "<br>";
  out << "<br>resultHTML:<br>" << result[WebAPI::result::resultHtml].theString;
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorHtmlFunctions::innerInterpretProblem(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerInterpretProblem");
  CalculatorHTML theProblem;
  if (!input.isOfType<std::string>(&theProblem.inputHtml)) {
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  }
  theProblem.theProblemData.flagRandomSeedGiven = true;
  theProblem.theProblemData.randomSeed = static_cast<unsigned>(theCommands.theObjectContainer.CurrentRandomSeed);
  theProblem.interpretHtml(&theCommands.comments);
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
  << theProblem.timePerAttempt.toStringCommaDelimited();
  return output.assignValue(out.str(), theCommands);
}

std::string CalculatorHTML::ToStringExtractedCommands() {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringExtractedCommands");
  std::stringstream out;
  out << "<hr><b>The commands extracted from the HTML follow.</b><br>";
  out << "<table>";
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].syntacticRole != "") {
      out << "<tr>" << "<td>" << this->theContent[i].toStringDebug() << "</td>"
      << "</tr>";
    } else {
      out << "<tr><td></td></tr>";
    }
  }
  out << "</table>";
  out << "<hr><b>The HTML from which the commands were extracted follows.</b><br>" << this->inputHtml
  << "<hr><b>The parsing stack follows.</b>" << this->toStringParsingStack(this->eltsStack) << "<hr>";
  return out.str();
}

std::string CalculatorHTML::toStringContent() {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringContent");
  std::stringstream out;
//  out << "<hr><b>The split strings follow. </b><hr>" << splitStrings.toStringCommaDelimited();
  out << "<hr><b>The extracted commands follow.</b><br>";
  for (int i = 0; i < this->theContent.size; i ++) {
    out << this->theContent[i].toStringTagAndContent();
  }
  out << "<hr><b>The html read follows.</b><br>" << this->inputHtml << "<hr>";
  return out.str();
}

void SyntacticElementHTML::resetAllExceptContent() {
  this->tag = "";
  this->properties.clear();
  this->syntacticRole = "";
  this->flagUseDisplaystyleInMathMode = false;
  this->children.setSize(0);
}

std::string SyntacticElementHTML::toStringOpenTag(const std::string& overrideTagIfNonEmpty, bool immediatelyClose) {
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
    if (!this->properties.contains(this->defaultKeysIfMissing[i])) {
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

std::string SyntacticElementHTML::toStringCloseTag(const std::string& overrideTagIfNonEmpty) {
  if (this->tag == "" || this->flagUseMathSpan == false) {
    return "";
  }
  if (overrideTagIfNonEmpty == "") {
    return "</" + this->tag + ">";
  } else {
    return "</" + overrideTagIfNonEmpty + ">";
  }
}

std::string SyntacticElementHTML::toStringTagAndContent() {
  MacroRegisterFunctionWithName("SyntacticElementHTML::toStringTagAndContent");
  if (this->syntacticRole == "") {
    return this->content;
  }
  std::stringstream out;
  out << this->toStringOpenTag("") + this->content + this->toStringCloseTag("");
  if (this->children.size > 0) {
    out << "[";
    for (int i = 0; i < this->children.size; i ++) {
      out << this->children[i].toStringDebug();
      if (i != this->children.size - 1) {
        out << ", ";
      }
    }
    out << "]";
  }
  return out.str();
}

std::string SyntacticElementHTML::toStringDebug() {
  MacroRegisterFunctionWithName("SyntacticElementHTML::toString");
  if (this->syntacticRole == "") {
    return HtmlRoutines::convertStringToHtmlString(this->toStringTagAndContent(), false);
  }
  std::stringstream out;
  out << "<span style =\"color:green\">";
  out << HtmlRoutines::convertStringToHtmlString(this->syntacticRole, false);
  out << "</span>";
  out << "[" << HtmlRoutines::convertStringToHtmlString(this->toStringTagAndContent(), false) << "]";
  return out.str();
}

std::string SyntacticElementHTML::getKeyValue(const std::string& theKey) const {
  MacroRegisterFunctionWithName("SyntacticElementHTML::getKeyValue");
  if (!this->properties.contains(theKey)) {
    return "";
  }
  return this->properties.getValueNoFail(theKey);
}

void SyntacticElementHTML::setKeyValue(const std::string& theKey, const std::string& theValue) {
  MacroRegisterFunctionWithName("SyntacticElementHTML::setKeyValue");
  this->properties.setKeyValue(theKey, theValue);
}

std::string SyntacticElementHTML::toStringInterpretedBody() {
  if (this->syntacticRole == "") {
    return this->content;
  }
  if (this->isInterpretedNotByCalculator()) {
    return this->interpretedCommand;
  }
  std::stringstream out;
  out << this->toStringOpenTag("");
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
  out << this->toStringCloseTag("");
  return out.str();
}

bool SyntacticElementHTML::isInterpretedNotByCalculator() {
  MacroRegisterFunctionWithName("SyntacticElementHTML::isInterpretedNotByCalculator");
  if (this->syntacticRole != "command") {
    return false;
  }
  if (
    this->tag == "answerCalculatorHighlightStart" ||
    this->tag == "answerCalculatorHighlightFinish"
  ) {
    return true;
  }
  std::string tagClass = this->getKeyValue("class");
  return
  tagClass == SyntacticElementHTML::Tags::calculatorExamProblem || tagClass == "calculatorExamIntermediate" ||
  tagClass == SyntacticElementHTML::Tags::calculatorAnswer || tagClass == "calculatorManageClass" ||
  tagClass == "generateTopicTable" ||
  tagClass == "calculatorJavascript" ||
  tagClass == "accountInformationLinks" ||
  tagClass == "generateTableOfContents" ||
  tagClass == "calculatorNavigationHere" ||
  tagClass == "calculatorProblemNavigationHere"||
  tagClass == "calculatorEditPageHere" ||
  this->isAnswerElement(nullptr);
}

bool SyntacticElementHTML::isInterpretedByCalculatorDuringProblemGeneration() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == "calculator" || tagClass == "calculatorHidden" ||
  tagClass == "calculatorShowToUserOnly" ;
}

bool SyntacticElementHTML::isInterpretedByCalculatorDuringSubmission() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == "calculator" || tagClass == "calculatorHidden";
}

bool SyntacticElementHTML::isAnswer() {
  if (this->syntacticRole != "command") {
    return false;
  }
  return this->getKeyValue("class") == SyntacticElementHTML::Tags::calculatorAnswer;
}

bool SyntacticElementHTML::isCalculatorCommand() {
  if (this->syntacticRole != "command") {
    return false;
  }
  return this->getKeyValue("class") == "calculator";
}

bool SyntacticElementHTML::isCalculatorHidden() {
  if (this->syntacticRole != "command") {
    return false;
  }
  return this->getKeyValue("class") == "calculatorHidden";
}

bool SyntacticElementHTML::isHidden() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == "calculatorHidden" || tagClass == "calculatorCommentsBeforeInterpretation";
}

bool SyntacticElementHTML::isSolution() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == SyntacticElementHTML::Tags::calculatorSolution;
}

bool SyntacticElementHTML::isAnswerOnGiveUp() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == "calculatorAnswerOnGiveUp";
}

bool SyntacticElementHTML::isCommentBeforeSubmission() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == "calculatorCommentsBeforeSubmission";
}

bool SyntacticElementHTML::isCommentBeforeInterpretation() {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  return tagClass == "calculatorCommentsBeforeInterpretation";
}

bool SyntacticElementHTML::isAnswerElement(std::string* desiredAnswerId) {
  if (this->syntacticRole != "command") {
    return false;
  }
  std::string tagClass = this->getKeyValue("class");
  bool result = tagClass == "calculatorButtonSubmit" ||
  tagClass == "calculatorButtonInterpret" ||
  tagClass == "calculatorButtonGiveUp" ||
  tagClass == "calculatorButtonSolution" ||
  tagClass == "calculatorMQField" ||
  tagClass == "calculatorMQButtonPanel" ||
  tagClass == "calculatorAnswerVerification" ||
  tagClass == SyntacticElementHTML::Tags::calculatorSolution;
  if (result && desiredAnswerId != nullptr) {
    *desiredAnswerId = this->getKeyValue("name");
  }
  return result;
}

std::string CalculatorHTML::prepareUserInputBoxes() {
  MacroRegisterFunctionWithName("CalculatorHTML::PrepareInterpreter");
  if (this->flagIsForReal) {
    return "";
  }
  std::stringstream out;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = global.webArguments;
  std::string inputNonAnswerReader;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::StringBeginsWith(theArgs.theKeys[i], "userInputBox", &inputNonAnswerReader)) {
      if (inputNonAnswerReader != "" && theArgs.theValues[i] != "") {
        out << Calculator::Atoms::setInputBox << "(name = "
        << inputNonAnswerReader
        << ", value = " << HtmlRoutines::convertURLStringToNormal(theArgs.theValues[i], false)
        << "); ";
      }
    }
  }
  return out.str();
}

std::string CalculatorHTML::getProblemHeaderEnclosure() {
  std::stringstream out;
  out << Calculator::Atoms::commandEnclosure << "{}(";
  out << Calculator::Atoms::setRandomSeed
  << "{}(" << this->theProblemData.randomSeed << "); ";
  out << this->prepareUserInputBoxes();
  out << "); ";
  return out.str();
}

std::string CalculatorHTML::getProblemHeaderWithoutEnclosure() {
  std::stringstream out;
  out << Calculator::Atoms::setRandomSeed
  << " {}(" << this->theProblemData.randomSeed << "); ";
  out << this->prepareUserInputBoxes();
  return out.str();
}

bool CalculatorHTML::prepareCommandsGenerateProblem(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommandsGenerateProblem");
  (void) comments;
  std::stringstream streamCommands, streamCommandsNoEnclosures;
  streamCommandsNoEnclosures << this->getProblemHeaderWithoutEnclosure();
  streamCommands << this->getProblemHeaderEnclosure();//first calculator enclosure contains the header
  int numCommandsSoFar = 2;//two commands at the start: the opEndStatement command and
  // the first enclosure.
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.isInterpretedByCalculatorDuringProblemGeneration()) {
      continue;
    }
    std::string commandCleaned = this->cleanUpCommandString(currentElt.content);
    std::string commandEnclosed = Calculator::Atoms::commandEnclosure + "{}( " + commandCleaned + " );";
    streamCommands << commandEnclosed;
    streamCommandsNoEnclosures << commandCleaned;
    currentElt.commandIndex = numCommandsSoFar;
    numCommandsSoFar ++;
  }
  this->theProblemData.commandsGenerateProblem = streamCommands.str();
  this->theProblemData.commandsGenerateProblemNoEnclosures = streamCommandsNoEnclosures.str();
  std::stringstream debugStream;
  debugStream << "<a href='"
  << HtmlRoutines::getCalculatorComputationURL(this->theProblemData.commandsGenerateProblemNoEnclosures)
  << "'>"
  << "Input link </a>";
  this->theProblemData.commandsGenerateProblemLink = debugStream.str();
  return true;
}

bool CalculatorHTML::parseHTMLPrepareCommands(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::parseHTMLPrepareCommands");
  if (!this->parseHTML(comments)) {
    return false;
  }
  return this->prepareCommands(comments);
}

bool CalculatorHTML::prepareCommands(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommands");
  if (!this->prepareCommandsGenerateProblem(comments)) {
    return false;
  }
  for (int i = 0; i < this->theProblemData.theAnswers.size(); i ++) {
    if (!this->prepareCommandsAnswer(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->prepareCommandsAnswerOnGiveUp(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->prepareCommandsSolution(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->prepareCommentsBeforeSubmission(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
    if (!this->prepareCommentsBeforeInterpretation(this->theProblemData.theAnswers.theValues[i], comments)) {
      return false;
    }
  }
  return true;
}

bool CalculatorHTML::prepareCommandsAnswerOnGiveUp(Answer& theAnswer, std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommandsAnswerOnGiveUp");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.isAnswerOnGiveUp()) {
      continue;
    }
    if (currentElt.getKeyValue("name") == theAnswer.answerId) {
      streamCommands << this->cleanUpCommandString(currentElt.content);
    }
  }
  theAnswer.commandsNoEnclosureAnswerOnGiveUpOnly = streamCommands.str();
  return true;
}

bool CalculatorHTML::prepareCommentsBeforeSubmission(
  Answer& theAnswer, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommentsBeforeSubmission");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.isCommentBeforeSubmission()) {
      continue;
    }
    if (currentElt.getKeyValue("name") != theAnswer.answerId) {
      continue;
    }
    streamCommands << this->cleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsCommentsBeforeSubmission = streamCommands.str();
  return true;
}

bool CalculatorHTML::prepareCommentsBeforeInterpretation(
  Answer& theAnswer, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommentsBeforeInterpretation");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (!currentElt.isCommentBeforeInterpretation()) {
      continue;
    }
    if (currentElt.getKeyValue("name") != theAnswer.answerId) {
      continue;
    }
    streamCommands << this->cleanUpCommandString(currentElt.content);
  }
  theAnswer.commandsCommentsBeforeInterpretatioN = streamCommands.str();
  return true;
}

bool CalculatorHTML::prepareCommandsSolution(Answer& theAnswer, std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommandsSolution");
  (void) comments;
  std::stringstream streamCommands;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& solutionElt = this->theContent[i];
    if (!solutionElt.isSolution()) {
      continue;
    }
    if (solutionElt.getKeyValue("name") != theAnswer.answerId) {
      continue;
    }
    int numCommandsSoFar = 2;
    theAnswer.solutionElements = solutionElt.children;
    for (int j = 0; j < theAnswer.solutionElements.size; j ++) {
      SyntacticElementHTML& currentElt = theAnswer.solutionElements[j];
      if (!currentElt.isCalculatorCommand() && !currentElt.isCalculatorHidden()) {
        continue;
      }
      currentElt.commandIndex = numCommandsSoFar;
      numCommandsSoFar ++;
      streamCommands << Calculator::Atoms::commandEnclosure << "{}("
      << this->cleanUpCommandString(currentElt.content) << "); ";
    }
  }
  theAnswer.commandsSolutionOnly = streamCommands.str();
  return true;
}

bool CalculatorHTML::prepareCommandsAnswer(Answer& theAnswer, std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareCommandsAnswer");
  std::stringstream streamCommandS;
  std::stringstream streamCommandsNoEnclosures;
  streamCommandS << this->getProblemHeaderEnclosure(); // first calculator enclosure contains the header
  streamCommandsNoEnclosures << this->getProblemHeaderWithoutEnclosure();
  std::stringstream streamCommandsBody;
  std::stringstream streamCommandsBodyNoEnclosures;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (
      !currentElt.isCalculatorHidden() && !currentElt.isCalculatorCommand()
      && !currentElt.isAnswer()
    ) {
      continue;
    }
    std::string commandCleaned = this->cleanUpCommandString(this->theContent[i].content);
    std::string commandEnclosed = Calculator::Atoms::commandEnclosure + "{}( " + commandCleaned + " );";
    if (currentElt.isAnswer() && currentElt.getKeyValue("id") == theAnswer.answerId) {
      std::string stringCommandsBody = streamCommandsBody.str();
      if (stringCommandsBody != "") {
        streamCommandS << Calculator::Atoms::commandEnclosure << "{}(" << stringCommandsBody << ");\n";
        streamCommandsNoEnclosures << streamCommandsBodyNoEnclosures.str();
      }
      theAnswer.commandsBeforeAnswer = streamCommandS.str();
      theAnswer.commandsBeforeAnswerNoEnclosuresForDEBUGGING = streamCommandsNoEnclosures.str();
      theAnswer.commandVerificationOnly = commandCleaned;
      return true;
    }
    if (this->theContent[i].isCalculatorHidden() || this->theContent[i].isCalculatorCommand()) {
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

bool CalculatorHTML::prepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream* comments) {
  MacroRegisterFunctionWithName("Problem::prepareAndExecuteCommands");
  double startTime = global.getElapsedSeconds();
  this->prepareCommands(comments);

  theInterpreter.initialize();
  theInterpreter.flagWriteLatexPlots = false;
  theInterpreter.flagPlotNoControls = true;
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds()-startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("calculator initialize time");
  if (global.userDebugFlagOn() && global.userDefaultHasProblemComposingRights()) {
    this->logCommandsProblemGeneratioN << "<b>Input commands:</b> "
    << this->theProblemData.commandsGenerateProblemLink
    << "<br>\n"
    << this->theProblemData.commandsGenerateProblem << "<br>";
  }
  theInterpreter.evaluate(this->theProblemData.commandsGenerateProblem);
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds() - startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("calculator evaluation time");
  bool result = !theInterpreter.flagAbortComputationASAP && theInterpreter.syntaxErrors == "";
  if (!result && comments != nullptr) {
    *comments << "<br>Failed to interpret your file. "
    << global.toStringCalculatorComputation(
      this->theProblemData.commandsGenerateProblemNoEnclosures, "Failed commands:"
    ) << "<br>"
    << this->theProblemData.commandsGenerateProblem
    << "<br>";
    if (global.userDefaultHasAdminRights()) {
      *comments << "The result of the interpretation attempt is:<br>"
      << theInterpreter.outputString << "<br><b>Comments</b><br>"
      << theInterpreter.outputCommentsString;
    } else {
      *comments << "This may be a bug with the problem. "
      << "Feel free to take a screenshot of the issue and "
      << "email it to the site admin(s). ";
    }
  }
  for (int i = 0; i < theInterpreter.theObjectContainer.theUserInputTextBoxesWithValues.size(); i ++) {
    this->theProblemData.inputNonAnswerIds.addOnTop(
      theInterpreter.theObjectContainer.theUserInputTextBoxesWithValues.theKeys[i]
    );
  }
  return result;
}

std::string SyntacticElementHTML::gGetTagClass() {
  return this->getKeyValue("class");
}

bool CalculatorHTML::prepareSectionList(std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareSectionList");
  if (!global.flagDatabaseCompiled) {
    commentsOnFailure << "Database not running. ";
    return false;
  }
  (void) commentsOnFailure;
  if (this->flagSectionsPrepared) {
    return true;
  }
  this->flagSectionsPrepared = true;
  if (
    this->currentUseR.sectionsTaught.size == 0 || (
      this->currentUseR.userRole != UserCalculator::Roles::administator &&
      this->currentUseR.userRole != UserCalculator::Roles::teacher
    )
  ) {
    if (this->currentUseR.sectionComputed != "") {
      this->databaseStudentSections.addOnTop(this->currentUseR.sectionComputed);
      return true;
    }
  }
  this->databaseStudentSections.addListOnTop(this->currentUseR.sectionsTaught);
  return true;
}

void CalculatorHTML::interpretManageClass(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretManageClass");
  if (!global.userDefaultHasAdminRights() || global.userStudentVieWOn()) {
    return;
  }
  if (!global.flagDatabaseCompiled) {
    inputOutput.interpretedCommand = "<b>Managing class not available (no database).</b>";
    return;
  }
  std::stringstream out;
  out << "<a href=\"" << global.DisplayNameExecutable << "?request=accounts\"> Manage accounts</a>";
  inputOutput.interpretedCommand = out.str();
}

bool CalculatorHTML::computeAnswerRelatedStrings(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::computeAnswerRelatedStrings");
  std::string desiredAnswerId = inputOutput.getKeyValue("id");
  if (desiredAnswerId == "") {
    inputOutput.interpretedCommand = "<b>Error: could not generate submit button: "
    "the answer tag does not have a valid id. Please fix the problem template.</b>";
    return false;
  }
  int theIndex = this->getAnswerIndex(desiredAnswerId);
  if (theIndex == - 1) {
    global.fatal << "This is not supposed to happen: problem has syntactic element with answerId: "
    << desiredAnswerId << " but the answerId is missing from the list of known answer ids. "
    << this->theProblemData.toStringAvailableAnswerIds() << global.fatal;
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
  currentA.properties.clear();
  for (int i = 0; i < inputOutput.properties.size(); i ++) {
    if (inputOutput.properties.theKeys[i] == "id") {
      continue;
    }
    currentA.properties.setKeyValue(inputOutput.properties.theKeys[i], inputOutput.properties.theValues[i]);
  }

  currentA.javascriptPreviewAnswer = previewAnswerStream.str();
  currentA.idButtonSubmit = "buttonSubmit" + answerId;
  currentA.idButtonInterpret = "buttonInterpret" + answerId;
  currentA.idButtonAnswer = "buttonAnswer" + answerId;
  currentA.idButtonSolution = "buttonSolution" + answerId;
  std::stringstream verifyStream;
  int numCorrectSubmissions = currentA.numCorrectSubmissions;
  int numSubmissions = currentA.numSubmissions;
  if (
    global.requestType == "scoredQuiz" ||
    global.requestType == "scoredQuizJSON"
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

void CalculatorHTML::interpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretGenerateStudentAnswerButton");
  if (!this->computeAnswerRelatedStrings(inputOutput)) {
    return;
  }
  Answer& currentA = this->theProblemData.theAnswers.theValues[this->getAnswerIndex(inputOutput.getKeyValue("id"))];
  std::stringstream out;
  out << "<br><span class =\"panelAnswer\" id = \"" << currentA.idAnswerPanel << "\"></span>";
  inputOutput.interpretedCommand = out.str();
}

void CalculatorHTML::interpretIfAnswer(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretIfAnswer");
  std::string tagClass = inputOutput.gGetTagClass();
  if (tagClass != SyntacticElementHTML::Tags::calculatorAnswer) {
    return;
  }
  this->interpretGenerateStudentAnswerButton(inputOutput);
}

void CalculatorHTML::interpretNotByCalculatorNotAnswer(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretNotByCalculatorNotAnswer");
  std::string tagClass = inputOutput.gGetTagClass();
  //std::string tag= inputOutput.tag;
  if (tagClass == SyntacticElementHTML::Tags::calculatorExamProblem || tagClass == "calculatorExamIntermediate") {
    this->interpretGenerateLink(inputOutput);
  } else if (tagClass == "calculatorManageClass") {
    this->interpretManageClass(inputOutput);
  } else if (tagClass == "generateTopicTable") {
    this->interpretTopicList(inputOutput);
  } else if (tagClass == "generateTableOfContents") {
    this->interpretTableOfContents(inputOutput);
  } else if (tagClass == "accountInformationLinks") {
    this->interpretAccountInformationLinks(inputOutput);
  } else if (tagClass == "calculatorJavascript") {
    this->interpretJavascripts(inputOutput);
  } else if (tagClass == "calculatorProblemNavigationHere") {
    this->interpretProblemNavigationBar(inputOutput);
  }
}

std::string CalculatorHTML::cleanUpFileName(const std::string& inputLink) {
  MacroRegisterFunctionWithName("CalculatorHTML::cleanUpFileName");
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

std::string CalculatorHTML::getDeadlineNoInheritance(const std::string& id) {
  MacroRegisterFunctionWithName("CalculatorHTML::getDeadlineNoInheritance");
  (void) id;
  if (!global.flagDatabaseCompiled) {
    // deadline not present.
    return "";
  }
  if (!this->currentUseR.theProblemData.contains(id)) {
    return "";
  }
  ProblemDataAdministrative& currentProb = this->currentUseR.theProblemData.GetValueCreateNoInit((id)).adminData;
  if (!currentProb.deadlinesPerSection.contains(this->currentUseR.sectionComputed)) {
    return "";
  }
  return currentProb.deadlinesPerSection.GetValueCreate(this->currentUseR.sectionComputed);
}

std::string CalculatorHTML::getDeadline(
  const std::string& problemName, const std::string& sectionNumber, bool& outputIsInherited
) {
  MacroRegisterFunctionWithName("CalculatorHTML::getDeadline");
  (void) problemName;
  (void) sectionNumber;
  (void) outputIsInherited;
  outputIsInherited = true;
  std::string result;
  if (!global.flagDatabaseCompiled) {
    return "While getting deadline: database not compiled";
  }
  int topicIndex = this->topics.theTopics.getIndex(problemName);
  if (topicIndex == - 1) {
    return problemName + " not found in topic list. ";
  }
  TopicElement& currentTopic = this->topics.theTopics.GetValueCreate(problemName);
  for (int i = currentTopic.parentTopics.size - 1; i >= 0; i --) {
    const std::string& containerName = this->topics.theTopics.theKeys[currentTopic.parentTopics[i]];
    if (this->currentUseR.theProblemData.contains(containerName)) {
      ProblemDataAdministrative& currentProb =
      this->currentUseR.theProblemData.GetValueCreateNoInit(containerName).adminData;
      result = currentProb.deadlinesPerSection.GetValueCreate(sectionNumber);
      if (StringRoutines::stringTrimWhiteSpace(result) != "") {
        outputIsInherited = (containerName != problemName);
        return result;
      }
    }
  }
  return result;
}

std::string CalculatorHTML::toStringOneDeadlineFormatted(
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
  std::string currentDeadline = this->getDeadline(topicID, sectionNumber, deadlineIsInherited);
  if (currentDeadline == "") {
    if (returnEmptyStringIfNoDeadline) {
      return "";
    }
    out << "<span style =\"color:orange\">No deadline yet</span>";
    return out.str();
  }
  if (!global.flagDatabaseCompiled) {
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
  if (deadlineIsInherited && !global.userStudentVieWOn()) {
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

std::string CalculatorHTML::toStringAllSectionDeadlines(const std::string& topicID, bool isSection) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringAllSectionDeadlines");
  if (!global.userDefaultHasAdminRights())
    return "";
  std::stringstream out;
  out << "<table>";
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    if (this->databaseStudentSections[i] == "") {
      continue;
    }
    out << "<tr><td>Section " << this->databaseStudentSections[i] << ":</td>";
    out << "<td>"
    << this->toStringOneDeadlineFormatted(topicID, this->databaseStudentSections[i], false, false, isSection)
    << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string CalculatorHTML::toStringDeadline(
  const std::string& topicID, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection
) {
  MacroRegisterFunctionWithName("CalculatorHTML::ToStringDeadlineWithModifyButton");
  (void) topicID;
  (void) problemAlreadySolved;
  (void) returnEmptyStringIfNoDeadline;
  (void) isSection;
  if (!global.flagDatabaseCompiled) {
    return "Database not available";
  }
  if (global.userGuestMode()) {
    return "deadlines require login";
  } else if (
    global.userDefaultHasAdminRights() &&
    global.userStudentVieWOn()
  ) {
    std::string sectionNum = HtmlRoutines::convertURLStringToNormal(
      global.getWebInput("studentSection"), false
    );
    return this->toStringOneDeadlineFormatted(
      topicID, sectionNum, problemAlreadySolved, returnEmptyStringIfNoDeadline, isSection
    );
  } else {
    return this->toStringOneDeadlineFormatted(
      topicID,
      this->currentUseR.sectionComputed,
      problemAlreadySolved,
      returnEmptyStringIfNoDeadline,
      isSection
    );
  }
  // return "";
}

void CalculatorHTML::computeDeadlinesAllSections(TopicElement& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::computeDeadlinesAllSections");
  inputOutput.deadlinesPerSectioN.initializeFillInObject(this->databaseStudentSections.size, "");
  inputOutput.deadlinesAreInherited.initializeFillInObject(this->databaseStudentSections.size, false);
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    inputOutput.deadlinesPerSectioN[i] = this->getDeadline(
      inputOutput.id, this->databaseStudentSections[i], inputOutput.deadlinesAreInherited[i]
    );
    if (inputOutput.deadlinesAreInherited[i]) {
      inputOutput.deadlinesPerSectioN[i] = "";
    }
  }
}

void CalculatorHTML::computeDeadlinesAllSectionsNoInheritance(TopicElement& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::computeDeadlinesAllSectionsNoInheritance");
  inputOutput.deadlinesPerSectioN.initializeFillInObject(this->databaseStudentSections.size, "");
  for (int i = 0; i < this->databaseStudentSections.size; i ++) {
    ProblemDataAdministrative& currentProb =
    this->currentUseR.theProblemData.GetValueCreateNoInit(inputOutput.id).adminData;
    inputOutput.deadlinesPerSectioN[i] =
    currentProb.deadlinesPerSection.GetValueCreate(this->databaseStudentSections[i]);
  }
}

std::string CalculatorHTML::toStringInterprettedCommands(Calculator &theInterpreter, List<SyntacticElementHTML>& theElements) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringInterprettedCommands");
  std::stringstream out;
  out << "<table>";
  int commandCounter = theInterpreter.theProgramExpression.size() - 1;
  for (int eltCounter = theElements.size - 1; eltCounter > 0; eltCounter --) {
    SyntacticElementHTML& currentElt = theElements[eltCounter];
    std::string currentEltString = currentElt.gGetTagClass() + "[" + currentElt.content.substr(0, 10) + "...]";
    if (!currentElt.isInterpretedByCalculatorDuringProblemGeneration()) {
      out << "<tr><td>" << currentEltString << "</td>"
      << "<td>"
      << theInterpreter.theProgramExpression[commandCounter].toString()
      << "</td></tr>";
      commandCounter --;
      continue;
    }
    for (; commandCounter > 1; commandCounter --) {
      std::string currentString= theInterpreter.theProgramExpression[commandCounter].toString();
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

bool CalculatorHTML::interpretProcessExecutedCommands(
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
    if (!currentElt.isInterpretedByCalculatorDuringProblemGeneration()) {
      currentElt.interpretedCommand = "";
      continue;
    }
    if (
      currentElt.commandIndex >= theInterpreter.theProgramExpression.size() ||
      currentElt.commandIndex < 0
    ) {
      std::stringstream errorStream;
      errorStream << "<b>This is a programming error: syntactic element "
      << currentElt.toStringDebug() << " has wrongly computed commandIndex: "
      << currentElt.commandIndex << ". "
      << "Please report this error to the website admins. </b>";
      currentElt.interpretedCommand = errorStream.str();
      result = false;
      continue;
    }
    if (!theInterpreter.theProgramExpression[currentElt.commandIndex].startsWith(theInterpreter.opCommandEnclosure())) {
      global.fatal << "Element: " << theInterpreter.theProgramExpression[currentElt.commandIndex].toString()
      << " in " << theInterpreter.theProgramExpression.toString()
      << " is supposed to be a command enclosure but apparently isn't. " << global.fatal;
    }
    Expression currentExpr = theInterpreter.theProgramExpression[currentElt.commandIndex][1];
    if (currentExpr.startsWith(theInterpreter.opEndStatement()) && currentExpr.size() == 2) {
      currentExpr = currentExpr[1];
    }
    if (currentExpr.startsWith(theInterpreter.opEndStatement())) {
      currentElt.flagUseMathMode = false;
    }
    theFormat.flagUseQuotes = false;
    theFormat.flagMakingExpressionTableWithLatex = true;
    currentElt.interpretedCommand = currentExpr.toString(&theFormat);
    currentElt.flagUseDisplaystyleInMathMode = (currentElt.content.find("\\displaystyle") != std::string::npos);
    currentElt.flagUseMathMode = true;
    currentElt.flagUseMathSpan = false;
    if (
      currentExpr.isOfType<std::string> () ||
      currentExpr.isOfType<Plot>() ||
      currentElt.getKeyValue("noTags") == "true"
    ) {
      currentElt.flagUseMathMode = false;
    }
  }
  return result;
}

void CalculatorHTML::logProblemGenerationObsolete(Calculator &theInterpreter) {
  if (!global.userDebugFlagOn() || !global.userDefaultHasProblemComposingRights()) {
    return;
  }
  std::stringstream streamLog;
  streamLog << "<table border ='1'>";
  for (int i = 0; i < theInterpreter.theProgramExpression.size(); i ++) {
    streamLog << "<tr>";
    for (int j = 0; j < this->theContent.size; j ++) {
      if (this->theContent[j].commandIndex == i) {
        streamLog << "<td>" << this->theContent[j].toStringDebug() << "</td>";
      }
    }
    streamLog << "<td>" << theInterpreter.theProgramExpression[i].toString()
    << "</td></tr>";
  }
  streamLog << "</table>";
  this->logCommandsProblemGeneratioN << streamLog.str();
}

void CalculatorHTML::figureOutCurrentProblemList(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::figureOutCurrentProblemList");
  if (this->flagParentInvestigated) {
    return;
  }
  this->flagParentInvestigated = true;
  this->topicListFileName = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::topicList), false);
  this->loadAndParseTopicList(comments);
}

bool CalculatorHTML::interpretHtml(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretHtml");
  double startTime = global.getElapsedSeconds();
  if (!this->parseHTML(comments)) {
    this->outputHtmlBodyNoTag = "<b>Failed to interpret html input. </b><br>" + this->toStringContent();
    this->timeToParseHtml = global.getElapsedSeconds() - startTime;
    return false;
  }
  this->timeToParseHtml = global.getElapsedSeconds() - startTime;
  this->MaxInterpretationAttempts = 25;
  this->randomSeedsIfInterpretationFails.setSize(this->MaxInterpretationAttempts);
  if (!this->theProblemData.flagRandomSeedGiven) {
    int randomSeedFromTime = static_cast<signed>(time(nullptr));
    global.unsecurePseudoRandomGenerator.setRandomSeed(103 + randomSeedFromTime);
    this->randomSeedsIfInterpretationFails[0] = (103 + global.unsecurePseudoRandomGenerator.getRandomLessThanBillion()) % 100000000;
  } else {
    this->randomSeedsIfInterpretationFails[0] = static_cast<int>(this->theProblemData.randomSeed);
  }
  global.unsecurePseudoRandomGenerator.setRandomSeed(this->randomSeedsIfInterpretationFails[0] + 1);
  for (int i = 1; i < this->randomSeedsIfInterpretationFails.size; i ++) {
    this->randomSeedsIfInterpretationFails[i] = (103 + global.unsecurePseudoRandomGenerator.getRandomLessThanBillion()) % 100000000;
  }
  this->timePerAttempt.setSize(0);
  this->timeIntermediatePerAttempt.setSize(0);
  this->timeIntermediateComments.setSize(0);
  this->NumAttemptsToInterpret = 0;
  while (this->NumAttemptsToInterpret < this->MaxInterpretationAttempts) {
    startTime = global.getElapsedSeconds();
    this->timeIntermediatePerAttempt.setSize(this->timeIntermediatePerAttempt.size + 1);
    this->timeIntermediatePerAttempt.lastObject()->setSize(0);
    this->timeIntermediateComments.setSize(this->timeIntermediateComments.size + 1);
    this->timeIntermediateComments.lastObject()->setSize(0);
    Calculator theInterpreter;
    this->NumAttemptsToInterpret ++;
    std::stringstream commentsOnLastFailure;
    if (this->interpretHtmlOneAttempt(theInterpreter, commentsOnLastFailure)) {
      this->timePerAttempt.addOnTop(global.getElapsedSeconds() - startTime);
      this->theProblemData.checkConsistency();
      return true;
    }
    this->timePerAttempt.addOnTop(global.getElapsedSeconds() - startTime);
    if (this->NumAttemptsToInterpret >= this->MaxInterpretationAttempts && comments != nullptr) {
      *comments << "Failed attempt " << this->NumAttemptsToInterpret
      << " to interpret your file. Attempted random seeds: "
      << this->randomSeedsIfInterpretationFails.toStringCommaDelimited()
      << "Last interpretation failure follows. <br>"
      << commentsOnLastFailure.str();
    }
  }
  if (comments != nullptr) {
    *comments << "<hr>Failed to evaluate the commands: " << this->NumAttemptsToInterpret
    << " attempts made. ";
  }
  if (this->flagIsForReal) {
    this->storeRandomSeedCurrent(comments);
    if (comments != nullptr) {
      *comments << "<b>Your random seed has been reset due to a finicky problem generation. </b>";
    }
  }
  this->theProblemData.checkConsistency();
  return false;
}

bool CalculatorHTML::isSplittingChar(const std::string& input) {
  if (input.size() != 1) {
    return false;
  }
  return this->splittingChars.contains(input[0]);
}

int SyntacticElementHTML::ParsingNumDummyElements = 8;
std::string CalculatorHTML::toStringParsingStack(List<SyntacticElementHTML>& theStack) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringParsingStack");
  std::stringstream out;
  out << "#Non-dummy elts: " << theStack.size - SyntacticElementHTML::ParsingNumDummyElements << ". ";
  for (int i = SyntacticElementHTML::ParsingNumDummyElements; i < theStack.size; i ++) {
    out << "<span style =\"color:" << ((i % 2 == 0) ? "orange" : "blue") << "\">";
    std::string theContent = theStack[i].toStringDebug();
    if (theContent.size() == 0) {
      theContent = "<b>empty</b>";
    } else if (theContent == " ") {
      theContent = "_";
    }
    out << theContent << "</span>";
  }
  return out.str();
}

int CalculatorHTML::getAnswerIndex(const std::string& desiredAnswerId) {
  return this->theProblemData.theAnswers.getIndex(desiredAnswerId);
}

bool CalculatorHTML::canBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right) {
  MacroRegisterFunctionWithName("SyntacticElementHTML::canBeMerged");
  if (left.syntacticRole != "" || right.syntacticRole != "") {
    return false;
  }
  if (this->isSplittingChar(left.content) && left.content != " ") {
    return false;
  }
  if (this->isSplittingChar(right.content) && right.content != " ") {
    return false;
  }
  return true;
}

bool CalculatorHTML::setTagClassFromCloseTag(SyntacticElementHTML& output) {
  MacroRegisterFunctionWithName("CalculatorHTML::setTagClassFromCloseTag");
  std::string& lastTag = output.tag;
  if (lastTag == "head" || lastTag == "HEAD" || lastTag == "Head") {
    output.setKeyValue("class", "headFinish");
    this->flagTagHeadPresent = true;
    return true;
  } else if (lastTag == "body" || lastTag == "BODY" || lastTag == "Body") {
    output.setKeyValue("class", "bodyFinish");
    this->flagTagBodyPresent = true;
    return true;
  } else if (lastTag == "html" || lastTag == "HTML" || lastTag == "html") {
    output.setKeyValue("class", "htmlFinish");
    this->flagTagHtmlPresent = true;
    return true;
  }
  return false;
}

bool CalculatorHTML::setTagClassFromOpenTag(SyntacticElementHTML& output) {
  MacroRegisterFunctionWithName("CalculatorHTML::setTagClassFromOpenTag");
  std::string& lastTag = output.tag;
  if (lastTag == "head" || lastTag == "HEAD" || lastTag == "Head") {
    output.setKeyValue("class", "headStart");
    this->flagTagHeadPresent = true;
    return true;
  } else if (lastTag == "body" || lastTag == "BODY" || lastTag == "Body") {
    output.setKeyValue("class", "bodyStart");
    this->flagTagBodyPresent = true;
    return true;
  } else if (lastTag == "html" || lastTag == "HTML" || lastTag == "html") {
    output.setKeyValue("class", "htmlStart");
    this->flagTagHtmlPresent = true;
    return true;
  } else if (this->calculatorTagsRecordedLiterally.contains(lastTag)) {
    output.setKeyValue("class", lastTag);
    return true;
  }
  return false;
}

void TopicElementParser::initializeElementTypes() {
  MacroRegisterFunctionWithName("CalculatorHTML::initializeElementTypes");
  // Please note: the static_cast<int> below is needed with the present gcc compiler version.
  // Without it, you should get linker errors.
  // See: https://stackoverflow.com/questions/5391973/undefined-reference-to-static-const-int
  if (this->elementTypes.size() == 0) {
    this->elementTypes.setKeyValue("Undefined"              , static_cast<int>(TopicElement::types::chapter               ));
    this->elementTypes.setKeyValue("Chapter"                , static_cast<int>(TopicElement::types::chapter               ));
    this->elementTypes.setKeyValue("Section"                , static_cast<int>(TopicElement::types::section               ));
    this->elementTypes.setKeyValue("Topic"                  , static_cast<int>(TopicElement::types::topic                 ));
    this->elementTypes.setKeyValue("Problem"                , static_cast<int>(TopicElement::types::problem               ));
    this->elementTypes.setKeyValue("Error"                  , static_cast<int>(TopicElement::types::error                 ));
    this->elementTypes.setKeyValue("TexHeader"              , static_cast<int>(TopicElement::types::error                 ));
    this->elementTypes.setKeyValue("Title"                  , static_cast<int>(TopicElement::types::title                 ));
    this->elementTypes.setKeyValue("Video"                  , static_cast<int>(TopicElement::types::video                 ));
    this->elementTypes.setKeyValue("VideoHandwritten"       , static_cast<int>(TopicElement::types::videoHandwritten      ));
    this->elementTypes.setKeyValue("SlidesLatex"            , static_cast<int>(TopicElement::types::slidesLatex           ));
    this->elementTypes.setKeyValue("SlidesSource"           , static_cast<int>(TopicElement::types::slidesSource          ));
    this->elementTypes.setKeyValue("HomeworkLatex"          , static_cast<int>(TopicElement::types::homeworkLatex         ));
    this->elementTypes.setKeyValue("HomeworkSource"         , static_cast<int>(TopicElement::types::homeworkSource        ));
    this->elementTypes.setKeyValue("HomeworkSolutionSource" , static_cast<int>(TopicElement::types::homeworkSolutionSource));
    this->elementTypes.setKeyValue("SlidesSourceHeader"     , static_cast<int>(TopicElement::types::slidesSourceHeader    ));
    this->elementTypes.setKeyValue("HomeworkSourceHeader"   , static_cast<int>(TopicElement::types::homeworkSourceHeader  ));
    this->elementTypes.setKeyValue("LoadTopicBundles"       , static_cast<int>(TopicElement::types::loadTopicBundles      ));
    this->elementTypes.setKeyValue("TopicBundle"            , static_cast<int>(TopicElement::types::topicBundle           ));
    this->elementTypes.setKeyValue("BundleBegin"            , static_cast<int>(TopicElement::types::bundleBegin           ));
    this->elementTypes.setKeyValue("BundleEnd"              , static_cast<int>(TopicElement::types::bundleEnd             ));
    for (int i = 0; i < this->elementTypes.size(); i ++) {
      this->elementNames.setKeyValue(this->elementTypes.theValues[i], this->elementTypes.theKeys[i]);
    }
  }
}

void CalculatorHTML::initAutocompleteExtras() {
  MacroRegisterFunctionWithName("CalculatorHTML::initAutocompleteExtras");
  if (this->autoCompleteExtras.size > 0) {
    return;
  }
  this->autoCompleteExtras.addOnTop("answerCalculatorHighlight");
  this->autoCompleteExtras.addOnTop("algebra");
  this->autoCompleteExtras.addOnTop("logarithms");
  this->autoCompleteExtras.addOnTop("buttons");
  this->autoCompleteExtras.addOnTop("displaystyle");
}

void CalculatorHTML::initBuiltInSpanClasses() {
  MacroRegisterFunctionWithName("CalculatorHTML::initBuiltInSpanClasses");
  if (this->calculatorTagsRecordedLiterally.size == 0) {
    this->calculatorTagsRecordedLiterally.addOnTopNoRepetition("answerCalculatorHighlight");
  }
  if (this->calculatorClassesAnswerFields.size == 0) {
    this->calculatorClassesAnswerFields.addOnTop("calculatorButtonSubmit");
    this->calculatorClassesAnswerFields.addOnTop("calculatorButtonInterpret");
    this->calculatorClassesAnswerFields.addOnTop("calculatorButtonGiveUp");
    this->calculatorClassesAnswerFields.addOnTop("calculatorButtonSolution");
    this->calculatorClassesAnswerFields.addOnTop("calculatorMQField");
    this->calculatorClassesAnswerFields.addOnTop("calculatorMQButtonPanel");
    this->calculatorClassesAnswerFields.addOnTop("calculatorAnswerVerification");
  }
  if (this->calculatorClasses.size == 0) {
    this->calculatorClasses.addOnTop("calculator");
    this->calculatorClasses.addOnTop(SyntacticElementHTML::Tags::calculatorSolution);
    this->calculatorClasses.addOnTop("calculatorShowToUserOnly");
    this->calculatorClasses.addOnTop("calculatorHidden");
    this->calculatorClasses.addOnTop("calculatorCommentsBeforeInterpretation");
    this->calculatorClasses.addOnTop("calculatorCommentsBeforeSubmission");
    this->calculatorClasses.addOnTop(SyntacticElementHTML::Tags::calculatorAnswer);
    this->calculatorClasses.addOnTop("calculatorAnswerOnGiveUp");
    this->calculatorClasses.addOnTop("calculatorExamIntermediate");
    this->calculatorClasses.addOnTop(SyntacticElementHTML::Tags::calculatorExamProblem);
    this->calculatorClasses.addOnTop("calculatorNavigationHere");
    this->calculatorClasses.addOnTop("calculatorProblemNavigationHere");
    this->calculatorClasses.addOnTop("calculatorEditPageHere");
    this->calculatorClasses.addOnTop("calculatorManageClass");
    this->calculatorClasses.addOnTop("setCalculatorExamProblem");
    this->calculatorClasses.addOnTop("setCalculatorExamHome");
    this->calculatorClasses.addOnTop("generateTopicTable");
    this->calculatorClasses.addOnTop("generateTableOfContents");
    this->calculatorClasses.addOnTop("accountInformationLinks");
    this->calculatorClasses.addOnTop("calculatorJavascript");
    this->calculatorClasses.addListOnTop(this->calculatorClassesAnswerFields);
  }
}

bool CalculatorHTML::parseHTML(std::stringstream* comments) {
  MacroRegisterFunctionWithName("Problem::parseHTML");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  std::string word;
  char currentChar;
  List<SyntacticElementHTML> theElements;
  theElements.setSize(0);
  theElements.setExpectedSize(static_cast<int>(theReader.str().size()) / 4);
  this->splittingChars.addOnTop('<');
  this->splittingChars.addOnTop('\"');
  this->splittingChars.addOnTop('>');
  this->splittingChars.addOnTop('=');
  this->splittingChars.addOnTop('/');
  this->splittingChars.addOnTop(' ');
  while (theReader.get(currentChar)) {
    if (splittingChars.contains(currentChar)) {
      if (word != "") {
        theElements.addOnTop(word);
      }
      std::string charToString;
      charToString.push_back(currentChar);
      theElements.addOnTop(charToString);
      word = "";
    } else {
      word.push_back(currentChar);
    }
  }
  if (word != "") {
    theElements.addOnTop(word);
  }
  this->initBuiltInSpanClasses();
  this->eltsStack.setSize(0);
  SyntacticElementHTML dummyElt, tempElt;
  dummyElt.content = "<>";
  dummyElt.syntacticRole = "filler";
  tempElt.syntacticRole = "command";
  tempElt.tag = "";
  tempElt.content = "";
  eltsStack.setExpectedSize(theElements.size + SyntacticElementHTML::ParsingNumDummyElements);
  for (int i = 0; i < SyntacticElementHTML::ParsingNumDummyElements; i ++) {
    eltsStack.addOnTop(dummyElt);
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
        eltsStack.addOnTop(theElements[indexInElts]);
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
      eltsStack.removeLastObject();
      if (this->isStateModifierApplyIfYes(secondToLast)) {
        eltsStack.removeLastObject();
      }
      continue;
    }
    if (
      thirdToLast.syntacticRole == "<openTag" &&
      secondToLast == "/" && last.syntacticRole == ">"
    ) {
      tagClass = thirdToLast.getKeyValue("class");
      if (tagClass == SyntacticElementHTML::Tags::calculatorSolution) {
        thirdToLast.syntacticRole = "<calculatorSolution>";
      } else if (this->calculatorClasses.contains(tagClass)) {
        thirdToLast.syntacticRole = "command";
      } else {
        thirdToLast.content = thirdToLast.toStringOpenTag("", true);
        thirdToLast.resetAllExceptContent();
      }
      eltsStack.removeLastObject();
      eltsStack.removeLastObject();
      if (this->isStateModifierApplyIfYes(thirdToLast)) {
        eltsStack.removeLastObject();
      }
      continue;
    }
    if (last.syntacticRole == "</closeTag>" && this->calculatorTagsRecordedLiterally.contains(last.tag)) {
      last.content = last.toStringCloseTag("");
      last.syntacticRole = "command";
      last.tag += "Finish";
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (last.syntacticRole == "</closeTag>") {
      last.content = last.toStringCloseTag("");
      last.resetAllExceptContent();
      //this->SetTagClassFromTag(last, true);
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTagCalc>" && secondToLast == "<" && last == "/") {
      secondToLast.syntacticRole = "</";
      eltsStack.removeLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<openTagCalc>" && secondToLast.syntacticRole == "") {
      thirdToLast.content += secondToLast.content;
      secondToLast = last;
      eltsStack.setSize(eltsStack.size - 1);
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
      eltsStack.removeLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "" && secondToLast != "\"" && last == "/") {
      secondToLast.content += last.content;
      eltsStack.removeLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "<" && last == "/") {
      secondToLast.syntacticRole = "</";
      secondToLast.tag = "";
      secondToLast.content = "";
      eltsStack.removeLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "</" ) {
      secondToLast.syntacticRole = "</closeTag";
      secondToLast.tag = last.content;
      secondToLast.content = "";
      eltsStack.removeLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "</closeTag" && last.syntacticRole == ">") {
      secondToLast.syntacticRole = "</closeTag>";
      if (this->setTagClassFromCloseTag(secondToLast)) {
        secondToLast.syntacticRole = "command";
      }
      eltsStack.removeLastObject();
      continue;
    }
    if (last == " " &&(
      secondToLast.syntacticRole == "<openTag" ||
      thirdToLast.syntacticRole == "<openTag" ||
      fourthToLast.syntacticRole == "<openTag"
    )) {
      eltsStack.removeLastObject();
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
      thirdToLast.tagKeysWithoutValue.addOnTop(secondToLast.content);
      eltsStack[eltsStack.size - 2] = *eltsStack.lastObject();
      eltsStack.removeLastObject();
      continue;
    }
    if (this->canBeMerged(secondToLast, last)) {
      secondToLast.content += last.content;
      eltsStack.removeLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole == "<calculatorSolution>" && (
      secondToLast.syntacticRole == "" ||
      secondToLast.syntacticRole == "command" ||
      secondToLast.syntacticRole == "<"
    )) {
      thirdToLast.children.addOnTop(secondToLast);
      eltsStack[eltsStack.size - 2] = last;
      eltsStack.removeLastObject();
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
      eltsStack.removeLastObject();
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
      eltsStack.removeLastObject();
      continue;
    }
    if (
      sixthToLast.syntacticRole == "<openTag" &&
      fourthToLast == "=" &&
      thirdToLast == "\"" &&
      last == "\""
    ) {
      sixthToLast.setKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.setSize(eltsStack.size - 5);
      continue;
    }
    if (
      fifthToLast.syntacticRole == "<openTag" &&
      thirdToLast == "=" &&
      secondToLast == "\"" &&
      last == "\""
    ) {
      fifthToLast.setKeyValue(fourthToLast.content, "");
      eltsStack.setSize(eltsStack.size - 4);
      continue;
    }
    if (thirdToLast == "\"" && secondToLast != "\"" && last == "\"") {
      thirdToLast.content += secondToLast.content;
      thirdToLast.content += last.content;
      eltsStack.setSize(eltsStack.size - 2);
      continue;
    }
    if (
      thirdToLast.syntacticRole == "<openTag" && secondToLast.syntacticRole == "" && last.syntacticRole == ">"
    ) {
      thirdToLast.tagKeysWithoutValue.addOnTop(secondToLast.content);
      eltsStack[eltsStack.size - 2] = *eltsStack.lastObject();
      eltsStack.removeLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "<openTag" && last.syntacticRole == ">") {
      tagClass = secondToLast.getKeyValue("class");
      tag = secondToLast.tag;
      if (
        tagClass == SyntacticElementHTML::Tags::calculatorSolution ||
        tag == SyntacticElementHTML::Tags::calculatorSolution
      ) {
        secondToLast.syntacticRole = "<calculatorSolution>";
        secondToLast.setKeyValue("class", SyntacticElementHTML::Tags::calculatorSolution);
      } else if (this->calculatorClasses.contains(tagClass) || this->calculatorClasses.contains(tag)) {
        secondToLast.syntacticRole = "<openTagCalc>";
        if (this->calculatorClasses.contains(tag)) {
          tagClass = tag;
          secondToLast.setKeyValue("class", tagClass);
        }
      } else if (this->calculatorTagsRecordedLiterally.contains(secondToLast.tag)) {
        secondToLast.syntacticRole = "command";
        secondToLast.tag += "Start";
      } else if (this->setTagClassFromOpenTag(secondToLast)) {
        secondToLast.syntacticRole = "command";
      } else {
        secondToLast.content = secondToLast.toStringOpenTag("");
        if (global.userDefaultHasProblemComposingRights() && comments != nullptr) {
          if (StringRoutines::StringBeginsWith(tagClass, "calculator")) {
            if (!this->calculatorClasses.contains(tagClass)) {
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
      eltsStack.removeLastObject();
      continue;
    }
    if (secondToLast.syntacticRole == "" && last.syntacticRole == ">") {
      secondToLast.content += ">";
      eltsStack.removeLastObject();
      continue;
    }
    reduced = false;
  } while (reduced || indexInElts < theElements.size);
  this->theContent.setSize(0);
  bool result = true;
  for (int i = SyntacticElementHTML::ParsingNumDummyElements; i < eltsStack.size; i ++) {
    bool needNewTag = false;
    if (i == SyntacticElementHTML::ParsingNumDummyElements) {
      needNewTag = true;
    } else if (this->theContent.lastObject()->syntacticRole != "") {
      needNewTag = true;
    }
    if (eltsStack[i].syntacticRole != "") {
      needNewTag = true;
    }
    if (eltsStack[i].gGetTagClass() == SyntacticElementHTML::Tags::calculatorAnswer) {
      if (eltsStack[i].getKeyValue("mqMatrices") == "true") {
        this->flagMathQuillWithMatrices = true;
      }
    }
    if (eltsStack[i].syntacticRole != "command" && eltsStack[i].syntacticRole != "") {
      result = false;
      if (comments != nullptr) {
        *comments << "<br>Syntactic element: " << eltsStack[i].toStringDebug()
        << " is not a command but has non-empty syntactic role.";
      }
    }
    if (!needNewTag) {
      this->theContent.lastObject()->content += eltsStack[i].content;
    } else {
      if (this->theContent.size > 0) {
        if (
          this->theContent.lastObject()->isInterpretedByCalculatorDuringProblemGeneration() &&
          eltsStack[i].isInterpretedByCalculatorDuringProblemGeneration()
        ) {
          SyntacticElementHTML emptyElt;
          this->theContent.addOnTop(emptyElt);
        }
      }
      this->theContent.addOnTop(eltsStack[i]);
    }
  }
  if (!result && comments != nullptr) {
    *comments << "<hr>Parsing stack.<hr>" << this->toStringParsingStack(this->eltsStack);
  }
  if (result) {
    result = this->extractAnswerIds(comments);
  }
  for (int i = 0; i < this->theContent.size; i ++) {
    this->theContent[i].indexInOwner = i;
  }
  if (result) {
    result = this->checkContent(comments);
  }
  this->theProblemData.checkConsistency();
  return result;
}

bool CalculatorHTML::interpretOneAnswerElement(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretOneAnswerElement");
  std::string answerId;
  if (!inputOutput.isAnswerElement(&answerId)) {
    return true;
  }
  int theIndex = this->getAnswerIndex(answerId);
  std::string tagClass = inputOutput.gGetTagClass();
  if (theIndex == - 1) {
    std::stringstream out;
    out << "<b>Element of class " << tagClass << " has name: "
    << answerId << " but that does not match any answerId value. "
    << this->theProblemData.toStringAvailableAnswerIds() << ". </b>";
    inputOutput.interpretedCommand = out.str();
    return true;
  }
  Answer& currentA = this->theProblemData.theAnswers.theValues[theIndex];
  if (tagClass == "calculatorAnswerVerification") {
    inputOutput.interpretedCommand = currentA.htmlSpanVerifyAnswer;
  }
  return true;
}

bool CalculatorHTML::interpretAnswerHighlights(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretAnswerHighlights");
  (void) comments;
  this->answerHighlights.setSize(0);
  bool answerHighlightStarted = false;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].tag == "answerCalculatorHighlightStart") {
      answerHighlightStarted = true;
      this->answerHighlights.addOnTop("");
      this->theContent[i].content = "";
      continue;
    }
    if (!answerHighlightStarted) {
      continue;
    }
    if (this->theContent[i].isAnswerElement(nullptr)) {
      continue;
    }
    if (this->theContent[i].tag == "answerCalculatorHighlightFinish") {
      answerHighlightStarted = false;
      this->theContent[i].content = "";
      continue;
    }
    *this->answerHighlights.lastObject() += this->theContent[i].toStringInterpretedBody();
    this->theContent[i].content = "";
    this->theContent[i].interpretedCommand = "";
  }
  return true;
}

bool CalculatorHTML::interpretAnswerElements(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretAnswerElements");
  (void) comments;
  for (int i = 0; i < this->theContent.size; i ++) {
    this->interpretOneAnswerElement(this->theContent[i]);
  }
  return true;
}

bool CalculatorHTML::prepareAnswerElements(std::stringstream &comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::prepareAnswerElements");
  (void) comments;
  std::string currentId;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].isAnswerElement(&currentId)) {
      int index = this->getAnswerIndex(currentId);
      if (index == - 1) {
        continue;
      }
      Answer& currentA = this->theProblemData.theAnswers.theValues[index];
      std::string tagClass = this->theContent[i].gGetTagClass();
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
      if (tagClass == SyntacticElementHTML::Tags::calculatorSolution) {
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

bool CalculatorHTML::extractAnswerIds(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::extractAnswerIds");
  // we shouldn't clear this->theProblemData.theAnswers: it may contain
  // outdated information loaded from the database. We don't want to loose that info
  // (say we renamed an answerId but students have already stored answers using the old answerId...).
  List<std::string> answerIdsSeenSoFar;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentE = this->theContent[i];
    if (currentE.isAnswer()) {
      std::string currentId = StringRoutines::stringTrimWhiteSpace(currentE.getKeyValue("id"));
      if (currentId == "") {
        if (comments != nullptr) {
          *comments << "The answer element: " << currentE.toStringDebug() << " has empty id. This is not allowed. ";
        }
        return false;
      }
      int theIndex = this->getAnswerIndex(currentId);
      if (theIndex == - 1) {
        this->theProblemData.addEmptyAnswerIdOnTop(currentId);
      }
      if (answerIdsSeenSoFar.contains(currentId)) {
        if (comments != nullptr) {
          *comments << "<b>Answer with id: "
          << currentId << " contained more than once. </b>";
        }
        return false;
      }
      answerIdsSeenSoFar.addOnTopNoRepetition(currentId);
      this->theProblemData.theAnswers.GetValueCreate(currentId).MQpanelButtonOptions =
      currentE.getKeyValue("buttons");
      continue;
    }
    if (
      !currentE.isCommentBeforeSubmission() &&
      !currentE.isCommentBeforeInterpretation() &&
      !currentE.isAnswerOnGiveUp() &&
      !currentE.isSolution()
    ) {
      continue;
    }
    if (answerIdsSeenSoFar.size == 0 && currentE.getKeyValue("name") == "") {
      if (comments != nullptr) {
        *comments << "Auxilary answer element: " << currentE.toStringDebug()
        << " has no name and appears before the first answer tag. "
        << "Auxilary answers apply the answer tag whose id is specified in the name "
        << "tag of the auxilary answer. If the auxilary answer has no "
        << "name tag, it is assumed to apply to the (nearest) answer tag above it. "
        << "To fix the issue either place the auxilary element after the answer or "
        << "specify the answer's id in the name tag of the auxilary element. ";
      }
      return false;
    }
    if (currentE.getKeyValue("name") == "") {
      currentE.setKeyValue("name", *answerIdsSeenSoFar.lastObject());
    }
  }
  this->theProblemData.checkConsistency();
  return true;
}

bool CalculatorHTML::checkContent(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::checkContent");
  bool result = true;
  for (int i = 0; i < this->theContent.size; i ++) {
    SyntacticElementHTML& currentElt = this->theContent[i];
    if (
      currentElt.syntacticRole == "command" &&
      currentElt.isAnswer() &&
      currentElt.getKeyValue("id").find('=') != std::string::npos
    ) {
      result = false;
      if (comments != nullptr) {
        *comments << "Error: the id of tag " << currentElt.toStringDebug()
        << " contains the equality sign which is not allowed. ";
      }
    }
  }
  return result;
}

std::string CalculatorHTML::cleanUpCommandString(const std::string& inputCommand) {
  MacroRegisterFunctionWithName("CalculatorHTML::cleanUpCommandString");
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
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExtractCalculatorExpressionFromHtml");
  CalculatorHTML theFile;
  if (!input.isOfType<std::string>(&theFile.inputHtml)) {
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  }
  if (!theFile.parseHTML(&theCommands.comments)) {
    return false;
  }
  return output.assignValue(theFile.ToStringExtractedCommands(), theCommands);
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

JSData CalculatorHTML::getJavascriptMathQuillBoxesForJSON() {
  MacroRegisterFunctionWithName("CalculatorHTML::getJavascriptMathQuillBoxesForJSON");
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

bool CalculatorHTML::storeRandomSeedCurrent(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("CalculatorHTML::storeRandomSeedCurrent");
  if (!global.flagDatabaseCompiled) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error: database not running. ";
    }
    return false;
  }
  this->theProblemData.flagRandomSeedGiven = true;
  this->currentUseR.setProblemData(this->fileName, this->theProblemData);
  if (!this->currentUseR.storeProblemData(this->fileName, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b style = 'color:red'>"
      << "Error: failed to store problem in database. "
      << "If you see this message, please take a screenshot and email your instructor. "
      << "</b>";
    }
    return false;
  }
  return true;
}

void CalculatorHTML::computeProblemLabel() {
  if (this->outputProblemLabel != "") {
    return;
  }
  if (
    this->flagIsExamHome ||
    global.requestType == "template" ||
    global.requestType == "templateNoLogin"
  ) {
    return;
  }
  if (!this->topics.theTopics.contains(this->fileName)) {
    return;
  }
  TopicElement& current = this->topics.theTopics.GetValueCreate(this->fileName);
  current.computeLinks(*this, true);
  this->outputProblemLabel = current.problemNumberString;
  this->outputProblemTitle = current.title;
}

void CalculatorHTML::computeBodyDebugString() {
  MacroRegisterFunctionWithName("CalculatorHTML::computeBodyDebugString");
  this->outputDebugInformationBody = "";
  if (!global.userDebugFlagOn() || !global.userDefaultHasAdminRights()) {
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
  << HtmlRoutines::convertStringToHtmlString(this->toStringCalculatorArgumentsForProblem("exercise", "false"), true);
  this->outputDebugInformationBody = out.str();
}

bool CalculatorHTML::interpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretHtmlOneAttempt");
  double startTime = global.getElapsedSeconds();
  std::stringstream outBody;
  std::stringstream outHeadPt2;
  this->flagIsExamHome =
  global.requestType == "template" ||
  global.requestType == "templateNoLogin";
  this->theProblemData.randomSeed = static_cast<unsigned>(this->randomSeedsIfInterpretationFails[
    this->NumAttemptsToInterpret - 1
  ]);
  this->figureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds() - startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("Time before after loading problem list");
  outHeadPt2 << HtmlRoutines::getJavascriptMathjax("");
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds() - startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("Time before execution");
  if (!this->prepareAndExecuteCommands(theInterpreter, &comments)) {
    return false;
  }
  //////////////////////////////interpretation takes place before javascript generation as the latter depends on the former.
  this->computeProblemLabel();
  std::string problemLabel = "";
  if (
    !this->flagIsExamHome &&
    global.requestType != "template" &&
    global.requestType != "templateNoLogin"
  ) {
    if (this->topics.theTopics.contains(this->fileName)) {
      TopicElement& current = this->topics.theTopics.GetValueCreate(this->fileName);
      current.computeLinks(*this, true);
      problemLabel = current.displayTitle + "&nbsp;&nbsp;";
      if (this->flagDoPrependProblemNavigationBar) {
        problemLabel += current.displayResourcesLinks;
      }
    }
  }
  if (
    this->flagIsExamProblem &&
    this->flagIsForReal &&
    !this->flagIsExamHome &&
    global.requestType != "template" &&
    global.requestType != "templateNoLogin"
  ) {
    if (global.flagDatabaseCompiled) {
      bool problemAlreadySolved = false;
      if (this->currentUseR.theProblemData.contains(this->fileName)) {
        ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(this->fileName);
        if (theProbData.numCorrectlyAnswered >= theProbData.theAnswers.size()) {
          problemAlreadySolved = true;
        }
      }
      this->outputDeadlineString = this->toStringDeadline(this->fileName, problemAlreadySolved, true, true);
    }
  }
  //////////////////////////////
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds() - startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("Time after execution");
  //first command and first syntactic element are the random seed and are ignored.
  theInterpreter.theObjectContainer.resetSliders();
  if (!this->interpretProcessExecutedCommands(theInterpreter, this->theContent, comments)) {
    outBody << comments.str();
    this->outputHtmlBodyNoTag = outBody.str();
    return false;
  }
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds() - startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("Time before class management routines");
  this->prepareAnswerElements(comments);
  this->NumAnswerIdsMathquilled = 0;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (this->theContent[i].isInterpretedNotByCalculator()) {
      this->interpretNotByCalculatorNotAnswer(this->theContent[i]);
    }
  }
  this->interpretAnswerHighlights(comments);
  for (int i = 0; i < this->theContent.size; i ++) {
    this->interpretIfAnswer(this->theContent[i]);
  }
  outHeadPt2 << this->topicListJavascriptWithTag;
  this->interpretAnswerElements(comments);
  this->theProblemData.checkConsistency();
  this->theProblemData.checkConsistencyMathQuillIds();
  bool headFinished = !this->flagTagHeadPresent;
  std::string tagClass;
  for (int i = 0; i < this->theContent.size; i ++) {
    if (!this->theContent[i].isHidden()) {
      tagClass = this->theContent[i].gGetTagClass();
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
        outBody << this->theContent[i].toStringInterpretedBody();
      } else {
        outHeadPt2 << this->theContent[i].toStringInterpretedBody();
      }
    }
  }
  if (this->flagIsExamProblem) {
    if (theInterpreter.flagHasGraphics) {
      MapReferences<std::string, std::string, MathRoutines::HashString>& theScripts =
      theInterpreter.theObjectContainer.graphicsScripts;
      for (int i = 0; i < theScripts.size(); i ++) {
        this->theScripts.setKeyValue(theScripts.theKeys[i], theScripts.theValues[i]);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////
  //out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
  //out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //out << "<hr> Between the commands:" << this->betweenTheCommands.toStringCommaDelimited();
  this->timeIntermediatePerAttempt.lastObject()->addOnTop(global.getElapsedSeconds() - startTime);
  this->timeIntermediateComments.lastObject()->addOnTop("Time before database storage");
  if (global.flagDatabaseCompiled) {
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
      bool successStoringSeed = this->storeRandomSeedCurrent(&comments);
      if (!successStoringSeed) {
        global << Logger::red << "This should not happen: failed to store random seed." << Logger::endL
        << Logger::yellow << comments.str() << Logger::endL;
      }
    }
    this->computeBodyDebugString();

  }
  std::stringstream navigationAndEditTagStream;
  if (this->flagDoPrependProblemNavigationBar) {
    navigationAndEditTagStream << this->toStringProblemNavigation();
  }
  this->outputProblemNavigatioN = navigationAndEditTagStream.str();
  this->outputHtmlBodyNoTag = outBody.str();
  this->outputHtmlHeadNoTag = outHeadPt2.str();
  return true;
}

std::string CalculatorHTML::toStringProblemNavigation() const {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringProblemNavigation");
  std::stringstream out;
  std::string exerciseRequest = "exercise";
  std::string studentView = global.userStudentVieWOn() ? "true" : "false";
  std::string linkSeparator = " | ";
  std::string linkBigSeparator = " || ";
  if (global.userGuestMode()) {
    exerciseRequest = "exerciseNoLogin";
  }
  if (global.userGuestMode()) {
    out << "<b>Guest mode</b>" << linkSeparator;
  }
  if (!global.flagLoggedIn) {
    out << "<a href=\"" << global.DisplayNameExecutable
    << "?request=login\">Log in</a> " << linkSeparator;
  }
  List<std::string> randomSeedContainer;
  randomSeedContainer.addOnTop(WebAPI::problem::randomSeed);
  std::string calcArgsNoPassExamDetails =
  global.toStringCalculatorArgumentsNoNavigation(&randomSeedContainer);
  if (this->flagIsExamProblem) {
    if (global.requestType == "exercise") {
      out << "<a href=\"" << global.DisplayNameExecutable << "?request=scoredQuiz&"
      << this->toStringCalculatorArgumentsForProblem("scoredQuiz", studentView)
      << "\">" << this->stringScoredQuizzes << "</a>" << linkSeparator;
      out << "<b style =\"color:green\">" << this->stringPracticE
      << "</b>" << linkSeparator;
    } else if (global.requestType == "scoredQuiz") {
      out << "<b style =\"color:brown\">"
      << this->stringScoredQuizzes << "</b>" << linkSeparator;
      out << "<a href=\"" << global.DisplayNameExecutable
      << "?request=exercise&"
      << this->toStringCalculatorArgumentsForProblem("exercise", studentView)
      << "\">" << this->stringPracticE << "</a>" << linkSeparator;
    }
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated) {
    int indexInParent = this->problemNamesNoTopics.getIndex(this->fileName);
    if (indexInParent == - 1) {
      out << "<b>Problem not in course</b>" << linkSeparator;
    } else {
      if (indexInParent > 0) {
        out << "<a href=\"" << global.DisplayNameExecutable << "?request="
        << global.requestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (global.getWebInput("studentSection") != "") {
          out << "studentSection=" << global.getWebInput("studentSection") << "&";
        }
        out << "topicList=" << HtmlRoutines::convertStringToURLString(this->topicListFileName, false) << "&";
        out << "courseHome=" << HtmlRoutines::convertStringToURLString(this->courseHome, false) << "&";
        out << "fileName=" << HtmlRoutines::convertStringToURLString(this->problemNamesNoTopics[indexInParent - 1], false)
        << "\">&#8592;</a>" << linkSeparator;
      } else {
        out << "<a disabled =\"disabled\">&#8592;</a>" << linkSeparator;
      }
      if (indexInParent < this->problemNamesNoTopics.size - 1) {
        out << "<a href=\"" << global.DisplayNameExecutable << "?request="
        << global.requestType;
        out << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&";
        if (global.getWebInput("studentSection") != "") {
          out << "studentSection=" << global.getWebInput("studentSection") << "&";
        }
        out << "topicList=" << HtmlRoutines::convertStringToURLString(this->topicListFileName, false) << "&";
        out << "courseHome=" << HtmlRoutines::convertStringToURLString(this->courseHome, false) << "&";
        out << "fileName=" << HtmlRoutines::convertStringToURLString(this->problemNamesNoTopics[indexInParent + 1], false)
        << "\">&#8594;</a>" << linkSeparator;
      } else {
        out << "<a disabled =\"disabled\">&#8594;</a>" << linkSeparator;
      }
    }
  }
  if (this->flagIsExamProblem &&(
    global.requestType == "exercise" ||
    global.requestType == "exerciseNoLogin"
  )) {
    out << "<a href=\"" << global.DisplayNameExecutable
    << "?request=" << global.requestType << "&"
    << this->toStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">" << this->stringProblemLink << " (#"
    << this->theProblemData.randomSeed << ")</a>" << linkBigSeparator;
  }
  if (global.userDefaultHasAdminRights()) {
    if (global.userStudentVieWOn()) {
      out << "<a href=\"" << global.DisplayNameExecutable << "?"
      << this->toStringCalculatorArgumentsForProblem(
        global.requestType,
        "false",
        HtmlRoutines::convertURLStringToNormal(global.getWebInput("studentSection"), false)
      ) << "\">Admin view</a>" << linkSeparator;
    } else {
      out << "<b>Admin view</b>" << linkSeparator;
    }
    if (this->databaseStudentSections.size == 0) {
      if (global.userStudentVieWOn()) {
        out << "<b>Student View</b>";
      } else {
        out << "<a href=\"" << global.DisplayNameExecutable << "?"
        << this->toStringCalculatorArgumentsForProblem(global.requestType, "true", "")
        << "\">Student view</a>";
      }
    } else {
      out << "Student view, section: ";
    }
    for (int i = 0; i < this->databaseStudentSections.size; i ++) {
      if (this->databaseStudentSections[i] != "") {
        if (
          global.userStudentVieWOn() &&
          this->databaseStudentSections[i] == HtmlRoutines::convertURLStringToNormal(
            global.getWebInput("studentSection"), false
          )
        ) {
          out << "<b>" << this->databaseStudentSections[i] << "</b>";
        } else {
          out << "<a href=\"" << global.DisplayNameExecutable << "?"
          << this->toStringCalculatorArgumentsForProblem(
            global.requestType, "true", this->databaseStudentSections[i]
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

std::string CalculatorHTML::toStringCalculatorArgumentsForProblem(
  const std::string& requestType,
  const std::string& studentView,
  const std::string& studentSection,
  bool includeRandomSeedIfAppropriate
) const {
  MacroRegisterFunctionWithName("WebWorker::toStringCalculatorArgumentsForProblem");
  if (!global.flagLoggedIn && !global.userGuestMode()) {
    return "";
  }
  std::stringstream out;
  out << "request=" << requestType << "&";
  List<std::string> excludedTags;
  excludedTags.addOnTop(WebAPI::problem::randomSeed);
  out << global.toStringCalculatorArgumentsNoNavigation(&excludedTags)
  << "courseHome=" << global.getWebInput(WebAPI::problem::courseHome) << "&";
  if (this->fileName != "") {
    out << WebAPI::problem::fileName << "=" << HtmlRoutines::convertStringToURLString(this->fileName, false) << "&";
  } else {
    out << WebAPI::problem::fileName << "=" << HtmlRoutines::convertStringToURLString(global.getWebInput(WebAPI::problem::fileName), false)
    << "&";
  }
  out << "topicList=" << global.getWebInput(WebAPI::problem::topicList) << "&";
  out << "studentView=" << studentView << "&";
  if (studentSection != "") {
    out << "studentSection=" << HtmlRoutines::convertStringToURLString(studentSection, false) << "&";
  }
  if (includeRandomSeedIfAppropriate) {
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
  }
  return out.str();
}

std::string CalculatorHTML::toStringProblemScoreFull(const std::string& theFileName) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringProblemScoreFull");
  (void) theFileName;
  std::stringstream out;
  if (global.userGuestMode()) {
    out << "scores require login";
    return out.str();
  }
  if (!global.flagDatabaseCompiled) {
    out << "scores not available: no database. ";
    return out.str();
  }
  Rational currentWeight;
  if (this->currentUseR.theProblemData.contains(theFileName)) {
    ProblemData& theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    if (!theProbData.flagProblemWeightIsOK) {
      out << "<span style =\"color:orange\">No point weight assigned yet. </span>";
      if (!theProbData.adminData.getWeightFromCourse(this->currentUseR.courseComputed, currentWeight)) {
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

std::string CalculatorHTML::toStringProblemScoreShort(const std::string& theFileName, bool& outputAlreadySolved) {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringProblemScoreShort");
  (void) theFileName;
  (void) outputAlreadySolved;
  if (!global.flagDatabaseCompiled) {
    return "Error: database not running. ";
  }
  std::stringstream out;
  if (global.userGuestMode()) {
    out << "scores require login";
    return out.str();
  }
  std::stringstream problemWeight;
  ProblemData theProbData;
  bool showModifyButton = global.userDefaultHasAdminRights() && !global.userStudentVieWOn();
  outputAlreadySolved = false;
  Rational currentWeight;
  std::string currentWeightAsGivenByInstructor;
  if (this->currentUseR.theProblemData.contains(theFileName)) {
    theProbData = this->currentUseR.theProblemData.GetValueCreate(theFileName);
    Rational percentSolved = 0, totalPoints = 0;
    percentSolved.AssignNumeratorAndDenominator(theProbData.numCorrectlyAnswered, theProbData.theAnswers.size());
    theProbData.flagProblemWeightIsOK = theProbData.adminData.getWeightFromCourse(
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
        out << "<b style =\"color:brown\">" << percentSolved
        << " out of " << problemWeight.str() << "</b>";
      } else {
        out << "<b style =\"color:red\">" << totalPoints
        << " out of " << problemWeight.str() << "</b>";
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
  return finalOut.str();
}

void TopicElement::computeID(int elementIndex, TopicElementParser& owner) {
  MacroRegisterFunctionWithName("TopicElement::computeID");
  if (this->problemFileName != "") {
    this->id = this->problemFileName;
    if (this->type != TopicElement::types::problem) {
      global.fatal
      << "Type problem is the only type allowed to have non-empty file name. "
      << global.fatal;
    }
  } else {
    std::stringstream out;
    out << elementIndex << ". ";
    out << "[" << owner.elementNames.getValueNoFail(this->type) << "] ";
    out << this->title;
    this->id = out.str();
  }
  this->idBase64 = Crypto::computeSha3_256OutputBase64URL(this->id);
}

TopicElementParser::TopicElementParser() {
  this->owner = nullptr;
  this->maximumTopics = 10000;
}

void TopicElementParser::addTopic(TopicElement& inputElt, int index) {
  MacroRegisterFunctionWithName("TopicElement::addTopic");
  if (this->theTopics.contains(inputElt.id)) {
    std::stringstream out;
    out << index << ". [Error] Element id: " << inputElt.id << " already present. ";
    inputElt.id = out.str();
    inputElt.title = out.str();
  }
  this->theTopics.setKeyValue(inputElt.id, inputElt);
}

void TopicElement::makeError(const std::string& message) {
  this->type = TopicElement::types::error;
  this->error = message;
  this->immediateChildren.setSize(0);
}

TopicElement::TopicElement() {
  this->reset();
}

void TopicElement::reset() {
  this->type = TopicElement::types::unknown;
  this->indexInParent = - 1;
  this->flagSubproblemHasNoWeight = false;
  this->title = "empty";
  this->id = "";
  this->video = "";
  this->videoHandwritten = "";

  this->querySlides   = "";
  this->queryHomework = "";

  this->handwrittenSolution = "";

  this->sourceSlides.setSize(0);
  this->sourceHomework.setSize(0);
  this->sourceHomeworkIsSolution.setSize(0);
  this->problemFileName = "";
  this->error = "";

  this->immediateChildren.setSize(0);
  this->totalSubSectionsUnderME = 0;
  this->totalSubSectionsUnderMeIncludingEmptySubsections = 0;
  this->flagContainsProblemsNotInSubsection = false;
  this->flagHasLectureTag = true;
  this->pointsEarnedInProblemsThatAreImmediateChildren = 0;
  this->totalPointsEarned = 0;
  this->maxPointsInAllChildren = 0;
  this->type = TopicElement::types::unknown;
}

bool TopicElementParser::checkConsistencyParsed() {
  MacroRegisterFunctionWithName("TopicElementParser::checkConsistencyParsed");
  for (int i = 0; i < this->theTopics.size(); i ++) {
    if (this->theTopics.theValues[i].type == TopicElement::types::problem) {
      if (this->theTopics.theValues[i].immediateChildren.size > 0) {
        global.fatal << "Topic element: "
        << this->theTopics.theValues[i].toString()
        << " has non-zero immediate children. " << global.fatal;
        return false;
      }
    }
  }
  return true;
}

bool TopicElementParser::checkNoErrors(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TopicElementParser::checkNoErrors");
  for (int i = 0; i < this->theTopics.size(); i ++) {
    TopicElement& current = this->theTopics.theValues[i];
    if (current.isError()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Element index "
        << i << " has an error. " << current.toString();
      }
      return false;
    }
  }
  return true;
}

bool TopicElement::isError() {
  return this->type == TopicElement::types::error;
}

bool TopicElement::pdfSlidesOpenIfAvailable(
  CalculatorHTML& owner, std::stringstream* commentsOnFailure
) {
  if (
    this->type != TopicElement::types::chapter &&
    this->type != TopicElement::types::section &&
    this->type != TopicElement::types::topic &&
    this->type != TopicElement::types::title &&
    this->type != TopicElement::types::problem
  ) {
    return true;
  }
  if (this->sourceSlides.size == 0) {
    return true;
  }
  LaTeXCrawler theCrawler;
  theCrawler.desiredPresentationTitle = this->title;

  theCrawler.AddSlidesOnTop(owner.slidesSourcesHeaders);
  theCrawler.AddSlidesOnTop(this->sourceSlides);
  theCrawler.flagProjectorMode = true;
  theCrawler.flagHomeworkRatherThanSlides = false;
  if (!theCrawler.ExtractFileNamesPdfExists(commentsOnFailure, commentsOnFailure)) {
    return false;
  }
  std::string actualOutput;
  FileOperations::getPhysicalFileNameFromVirtual(
    theCrawler.targetPDFFileNameWithPathVirtual, actualOutput, false, false, nullptr
  );
  global << "Physical filename: " << actualOutput << Logger::endL;
  if (!theCrawler.flagPDFExists && commentsOnFailure != nullptr) {
    *commentsOnFailure << "Could not find file: "
    << theCrawler.targetPDFFileNameWithPathVirtual << ". ";
  }
  return theCrawler.flagPDFExists;
}

bool TopicElement::pdfHomeworkOpensIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure) {
  if (
    this->type != TopicElement::types::chapter &&
    this->type != TopicElement::types::section &&
    this->type != TopicElement::types::topic &&
    this->type != TopicElement::types::title &&
    this->type != TopicElement::types::problem
  ) {
    return true;
  }
  if (this->sourceHomework.size == 0) {
    return true;
  }
  LaTeXCrawler theCrawler;
  theCrawler.desiredPresentationTitle = this->title;

  theCrawler.AddSlidesOnTop(owner.sourcesHomeworkHeaders);
  theCrawler.AddSlidesOnTop(this->sourceHomework);
  theCrawler.flagHomeworkRatherThanSlides = true;
  if (!theCrawler.ExtractFileNamesPdfExists(commentsOnFailure, commentsOnFailure)) {
    return false;
  }
  std::string actualOutput;
  FileOperations::getPhysicalFileNameFromVirtual(
    theCrawler.targetPDFFileNameWithPathVirtual, actualOutput, false, false, nullptr
  );
  global << "Physical filename: " << actualOutput << Logger::endL;
  if (!theCrawler.flagPDFExists && commentsOnFailure != nullptr) {
    *commentsOnFailure << "Could not find file: "
    << theCrawler.targetPDFFileNameWithPathVirtual << ". ";
  }
  return theCrawler.flagPDFExists;
}

bool TopicElement::pdfsOpenIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure) {
  return
  this->pdfSlidesOpenIfAvailable(owner, commentsOnFailure) &&
  this->pdfHomeworkOpensIfAvailable(owner, commentsOnFailure);
}

bool TopicElement::problemOpensIfAvailable(std::stringstream* commentsOnFailure) {
  if (this->problemFileName == "") {
    return true;
  }
  bool result = FileOperations::fileExistsVirtualCustomizedReadOnly(
    this->problemFileName, commentsOnFailure
  );
  if (!result && commentsOnFailure != nullptr) {
    *commentsOnFailure << "File "
    << CalculatorHTML::toStringLinkFromProblem(this->problemFileName)
    << " does not appear to exist. ";
  }
  return result;
}

bool TopicElementParser::checkProblemsOpen(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TopicElementParser::checkProblemsOpen");
  for (int i = 0; i < this->theTopics.size(); i ++) {
    TopicElement& current = this->theTopics.theValues[i];
    if (!current.problemOpensIfAvailable(commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool TopicElementParser::checkTopicPdfs(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TopicElementParser::checkTopicPdfs");
  this->checkInitialization();
  for (int i = 0; i < this->theTopics.size(); i ++) {
    TopicElement& current = this->theTopics.theValues[i];
    if (!current.pdfsOpenIfAvailable(*this->owner, commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool TopicElementParser::checkInitialization() {
  if (this->owner == nullptr) {
    global.fatal << "TopicElementParser not initialized when it should be. " << global.fatal;
  }
  return true;
}

void TopicElementParser::TopicLine::makeError(const std::string& message) {
  this->contentTrimmedWhiteSpace = StringRoutines::stringTrimWhiteSpace(message);
  this->tag = "Error";
  this->theType = TopicElement::types::error;
}

void TopicElementParser::TopicLine::MakeEmpty() {
  this->contentTrimmedWhiteSpace = "";
  this->tag = "";
  this->theType = TopicElement::types::empty;
}

void TopicElementParser::insertTopicBundle(TopicElementParser::TopicLine& input) {
  MacroRegisterFunctionWithName("TopicElementParser::insertTopicBundle");
  std::string bundleId = input.contentTrimmedWhiteSpace;
  if (!this->knownTopicBundles.contains(bundleId)) {
    std::stringstream out;
    out << "Failed to find bundle: " << bundleId << ". ";
    input.makeError(out.str());
    this->bundleStack.addOnTop(input);
    return;
  }
  List<TopicElementParser::TopicLine>& currentBundle = this->knownTopicBundles.GetValueCreate(bundleId);
  for (int i = currentBundle.size - 1; i >= 0; i --) {
    this->bundleStack.addOnTop(currentBundle[i]);
  }
}

void TopicElementParser::loadTopicBundleFile(
  TopicElementParser::TopicLine& input
) {
  MacroRegisterFunctionWithName("TopicElement::loadTopicBundleFile");
  std::string fileName = input.contentTrimmedWhiteSpace;
  if (this->loadedTopicBundleFiles.contains(fileName)) {
    return;
  }
  std::string newTopicBundles;
  std::stringstream errorStream;
  if (!FileOperations::isOKFileNameVirtual(fileName, false, &errorStream)) {
    errorStream << "The file name " << fileName << " is not a valid topic bundle file name. ";
    input.makeError(errorStream.str());
    this->bundleStack.addOnTop(input);
    return;
  }
  if (!FileOperations::loadFileToStringVirtualCustomizedReadOnly(fileName, newTopicBundles, &errorStream)) {
    errorStream << "Could not open topic bundle file. ";
    input.makeError(errorStream.str());
    this->bundleStack.addOnTop(input);
    return;
  }
  this->loadedTopicBundleFiles.addOnTop(fileName);
  std::string currentLineString;
  List<std::string> bundleNameStack;
  std::stringstream bundleReader(newTopicBundles);
  while (std::getline(bundleReader, currentLineString, '\n')) {
    TopicElementParser::TopicLine currentLine = this->extractLine(currentLineString);
    if (currentLine.theType == TopicElement::types::bundleBegin) {
      bundleNameStack.addOnTop(currentLine.contentTrimmedWhiteSpace);
    } else if (currentLine.theType == TopicElement::types::bundleEnd) {
      if (bundleNameStack.size > 0) {
        bundleNameStack.removeLastObject();
      } else {
        errorStream << "BundleEnd command without BungleBegin.";
        input.makeError(errorStream.str());
        this->bundleStack.addOnTop(input);
        return;
      }
    } else {
      for (int i = 0; i < bundleNameStack.size; i ++) {
        this->knownTopicBundles.GetValueCreate(bundleNameStack[i]).addOnTop(currentLine);
      }
    }
  }
}

TopicElementParser::TopicLine TopicElementParser::extractLine(const std::string& inputNonTrimmed) {
  TopicElementParser::TopicLine result;
  std::string input = StringRoutines::stringTrimWhiteSpace(inputNonTrimmed);
  if (input.size() == 0) {
    result.MakeEmpty();
    return result;
  }
  if (input[0] == '%') {
    result.MakeEmpty();
    return result;
  }
  result.theType = TopicElement::types::unknown;
  for (unsigned i = 0; i < input.size(); i ++) {
    char current = input[i];
    if (current == ':') {
      result.contentTrimmedWhiteSpace = StringRoutines::stringTrimWhiteSpace(input.substr(i + 1));
      break;
    }
    result.tag.push_back(current);
  }
  result.tag = StringRoutines::stringTrimWhiteSpace(result.tag);
  if (result.contentTrimmedWhiteSpace.size() == 0) {
    result.contentTrimmedWhiteSpace = result.tag;
    result.tag = "";
  }
  int indexElement = this->elementTypes.getIndex(result.tag);
  if (indexElement >= 0) {
    result.theType = this->elementTypes.theValues[indexElement];
  }
  return result;
}

void TopicElementParser::exhaustCrawlStack() {
  while (this->bundleStack.size > 0) {
    if (this->crawled.size > this->maximumTopics) {
      TopicElementParser::TopicLine errorLine;
      std::stringstream errorStream;
      errorStream << "Too many topics (" << this->crawled.size << ") while crawling bundles. ";
      errorLine.makeError(errorStream.str());
      this->crawled.addOnTop(errorLine);
      return;
    }
    TopicElementParser::TopicLine nextLine = bundleStack.popLastObject();
    if (nextLine.theType == TopicElement::types::loadTopicBundles) {
      this->loadTopicBundleFile(nextLine);
      continue;
    }
    if (nextLine.theType == TopicElement::types::topicBundle) {
      this->insertTopicBundle(nextLine);
      continue;
    }
    this->crawled.addOnTop(nextLine);
  }
}

void TopicElementParser::crawl(const std::string& inputString) {
  this->initializeElementTypes();
  std::string currentLine, currentArgument;
  std::stringstream tableReader(inputString);
  this->crawled.setSize(0);
  while (true) {
    if (this->crawled.size > this->maximumTopics) {
      TopicElementParser::TopicLine errorLine;
      std::stringstream errorStream;
      errorStream << "Too many topics (" << this->crawled.size << "). ";
      errorLine.makeError(errorStream.str());
      this->crawled.addOnTop(errorLine);
      return;
    }
    this->exhaustCrawlStack();
    if (!std::getline(tableReader, currentLine, '\n')) {
      break;
    }
    this->bundleStack.addOnTop(this->extractLine(currentLine));
  }
}

bool TopicElementParser::TopicLine::accountIfStateChanger(CalculatorHTML& owner) const {
  if (this->theType == TopicElement::types::slidesSourceHeader) {
    owner.slidesSourcesHeaders.addOnTop(this->contentTrimmedWhiteSpace);
    return true;
  }
  if (this->theType == TopicElement::types::homeworkSourceHeader){
    owner.sourcesHomeworkHeaders.addOnTop(this->contentTrimmedWhiteSpace);
    return true;
  }
  return false;
}

void TopicElementParser::addNewTopicElementFromLine(const TopicElementParser::TopicLine& input) {
  TopicElement incoming = input.toTopicElement();
  if (incoming.type == TopicElement::types::empty) {
    return;
  }
  this->elements.addOnTop(incoming);
}

bool TopicElement::mergeTopicLine(
  const TopicElementParser::TopicLine& input
) {
  if (this->type != TopicElement::types::problem) {
    return false;
  }
  if (input.theType == TopicElement::types::problem) {
    if (this->problemFileName == "") {
      this->problemFileName = input.contentTrimmedWhiteSpace;
      return true;
    } else {
      return false;
    }
  }
  switch (input.theType) {
  case TopicElement::types::video:
    this->video = input.contentTrimmedWhiteSpace;
    return true;
  case TopicElement::types::videoHandwritten:
    this->videoHandwritten = input.contentTrimmedWhiteSpace;
    return true;
  case TopicElement::types::slidesSource:
    this->sourceSlides.addOnTop(input.contentTrimmedWhiteSpace);
    return true;
  case TopicElement::types::homeworkSource:
    this->sourceHomework.addOnTop(input.contentTrimmedWhiteSpace);
    this->sourceHomeworkIsSolution.addOnTop(false);
    return true;
  case TopicElement::types::homeworkSolutionSource:
    this->sourceHomework.addOnTop(input.contentTrimmedWhiteSpace);
    this->sourceHomeworkIsSolution.addOnTop(true);
    return true;
  case TopicElement::types::slidesLatex:
      this->sourceSlides.addOnTop("LaTeX: " + input.contentTrimmedWhiteSpace);
    return true;
  case TopicElement::types::homeworkLatex:
      this->sourceHomework.addOnTop("LaTeX: " + input.contentTrimmedWhiteSpace);
      this->sourceHomeworkIsSolution.addOnTop(false);
    return true;
  case TopicElement::types::handwrittenSolutions:
      this->handwrittenSolution = input.contentTrimmedWhiteSpace;
    return true;
  }
  return  false;
}

std::string TopicElementParser::TopicLine::toString() const {
  std::stringstream out;
  out << this->tag << ": " << this->contentTrimmedWhiteSpace;
  return out.str();
}

TopicElement TopicElementParser::TopicLine::toTopicElement() const {
  TopicElement result;
  result.type = TopicElement::types::empty;
  if (this->theType == TopicElement::types::problem) {
    result.problemFileName = this->contentTrimmedWhiteSpace;
    result.type = TopicElement::types::problem;
  }
  if (this->theType == TopicElement::types::title) {
    result.type = TopicElement::types::problem;
    result.title = this->contentTrimmedWhiteSpace;
  }
  if (
    this->theType == TopicElement::types::chapter ||
    this->theType == TopicElement::types::section ||
    this->theType == TopicElement::types::topic
  ) {
    result.type = this->theType;
    result.title = this->contentTrimmedWhiteSpace;
  }
  return result;
}

void TopicElementParser::compressOneTopicLine(
  const TopicElementParser::TopicLine& input, CalculatorHTML& owner
) {
  if (input.accountIfStateChanger(owner)) {
    return;
  }
  if (this->elements.size == 0) {
    this->addNewTopicElementFromLine(input);
    return;
  }
  TopicElement& last = *this->elements.lastObject();
  if (last.mergeTopicLine(input)) {
    return;
  }
  this->addNewTopicElementFromLine(input);
}

void TopicElementParser::compressTopicLines() {
  MacroRegisterFunctionWithName("TopicElementParser::compressTopicLines");
  this->checkInitialization();
  this->elements.setSize(0);
  for (int i = 0; i < this->crawled.size; i ++) {
    this->compressOneTopicLine(this->crawled[i], *this->owner);
  }
}

void TopicElementParser::computeIds() {
  for (int i = 0; i < this->elements.size; i ++) {
    this->elements[i].computeID(i, *this);
  }
}

std::string TopicElementParser::toString() const {
  std::stringstream out;
  out << "Loaded bundle files: " << this->loadedTopicBundleFiles;
  out << "<hr>";
  out << "Known bundles: " << this->knownTopicBundles.toStringHtml();
  return out.str();
}

void TopicElementParser::parseTopicList(
  const std::string& inputString
) {
  MacroRegisterFunctionWithName("TopicElementParser::parseTopicList");
  this->crawl(inputString);
  this->compressTopicLines();
  this->computeIds();
  for (int i = 0; i < this->elements.size; i ++) {
    this->addTopic(this->elements[i], i);
  }
  this->computeTopicHierarchy();
  this->computeTopicNumbers();
}

void TopicElementParser::computeTopicNumbers() {
  List<int> currentProblemNumber;
  for (int i = 0; i < this->theTopics.size(); i ++) {
    TopicElement& current = this->theTopics.theValues[i];
    int labelsNeeded = current.type - TopicElement::types::chapter + 1;
    if (labelsNeeded > 4 || labelsNeeded < 0) {
      labelsNeeded = 4;
    }
    for (int j = currentProblemNumber.size; j < labelsNeeded; j ++) {
      currentProblemNumber.addOnTop(0);
    }
    currentProblemNumber.setSize(labelsNeeded);
    (*currentProblemNumber.lastObject()) ++;
    current.problemNumber = currentProblemNumber;
  }
}

void TopicElementParser::computeTopicHierarchy() {
  this->computeTopicHierarchyPartOne();
  this->computeTopicHierarchyPartTwo();
}

void TopicElementParser::computeTopicHierarchyPartOne() {
  MacroRegisterFunctionWithName("computeTopicHierarchyPartOne");
  List<int> parentChain;
  List<int> parentTypes;
  for (int i = 0; i < this->theTopics.size(); i ++) {
    int currentAdjustedtype = this->theTopics.theValues[i].type;
    if (
      currentAdjustedtype != TopicElement::types::chapter &&
      currentAdjustedtype != TopicElement::types::section &&
      currentAdjustedtype != TopicElement::types::topic
    ) {
      currentAdjustedtype = TopicElement::types::problem;
    }
    for (int j = parentChain.size - 1; j >= 0; j --) {
      if (parentTypes[j] >= currentAdjustedtype) {
        parentTypes.removeLastObject();
        parentChain.removeLastObject();
      }
    }
    if (parentChain.size > 0) {
      TopicElement& parent = this->theTopics.theValues[*parentChain.lastObject()];
      parent.immediateChildren.addOnTop(i);
    }
    this->theTopics.theValues[i].parentTopics.setSize(0);
    for (int j = 0; j < parentChain.size; j ++) {
      this->theTopics.theValues[i].parentTopics.addOnTop(parentChain[j]);
    }
    parentChain.addOnTop(i);
    parentTypes.addOnTop(currentAdjustedtype);
  }
}

void TopicElementParser::computeTopicHierarchyPartTwo() {
  MacroRegisterFunctionWithName("TopicElementParser::computeTopicHierarchyPartTwo");
  for (int i = this->theTopics.size() - 1; i >= 0; i --) {
    TopicElement& currentElt = this->theTopics.theValues[i];
    if (currentElt.problemFileName != "") {
      continue;
    }
    if (currentElt.type == TopicElement::types::topic) {
      currentElt.totalSubSectionsUnderME = 0;
      currentElt.totalSubSectionsUnderMeIncludingEmptySubsections = 0;
      currentElt.flagContainsProblemsNotInSubsection = false;
      continue;
    }
    currentElt.flagContainsProblemsNotInSubsection = false;
    currentElt.totalSubSectionsUnderME = 0;
    for (int j = 0; j < currentElt.immediateChildren.size; j ++) {
      TopicElement& currentChild = this->theTopics.theValues[currentElt.immediateChildren[j]];
      if (currentChild.type == TopicElement::types::topic) {
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
  this->checkConsistencyParsed();
}

void CalculatorHTML::interpretAccountInformationLinks(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretAccountInformationLinks");
  std::stringstream out;
  if (!global.flagLoggedIn) {
    out << "<b>User not logged-in.</b>";
    inputOutput.interpretedCommand = out.str();
    return;
  }
  if (!global.flagUsingSSLinCurrentConnection) {
    out << "<b>Account management requires https.</b>";
    inputOutput.interpretedCommand = out.str();
    return;
  }
  out << "<a href=\"" << global.DisplayNameExecutable << "?request=changePasswordPage\">Change password</a>";
  if (global.userDefaultHasAdminRights()) {
    out << "<br>\n<a href=\"" << global.DisplayNameExecutable << "?request=accounts\">Manage accounts</a>";
  }
  inputOutput.interpretedCommand = out.str();
  return;
}

bool CalculatorHTML::loadAndParseTopicList(std::stringstream& comments) {
  MacroRegisterFunctionWithName("CalculatorHTML::loadAndParseTopicList");
  if (this->topics.theTopics.size() != 0) {
    return true;
  }
  if (this->topicListContent == "") {
    if (!FileOperations::loadFileToStringVirtualCustomizedReadOnly(
      this->topicListFileName, this->topicListContent, &comments
    )) {
      comments << "Failed to load the topic list associated with this course. "
      << "Go to ``select course'' from the menu to see a list of available courses. ";
      return false;
    }
  }
  if (this->topicListContent == "") {
    comments  << "Topic list empty. Topic list file name: " << this->topicListFileName << ". ";
    return false;
  }
  this->topics.parseTopicList(this->topicListContent);
  this->problemNamesNoTopics.clear();
  for (int i = 0; i < this->topics.theTopics.size(); i ++) {
    if (this->topics.theTopics.theValues[i].problemFileName != "") {
      this->problemNamesNoTopics.addOnTop(this->topics.theTopics.theValues[i].problemFileName);
    }
  }
  return true;
}

void CalculatorHTML::interpretProblemNavigationBar(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::InterpretCalculatorNavigationBar");
  inputOutput.interpretedCommand = this->toStringProblemNavigation();
  this->flagDoPrependProblemNavigationBar = false;
}

JSData CalculatorHTML::toStringTopicListJSON() {
  MacroRegisterFunctionWithName("CalculatorHTML::toStringTopicListJSON");
  std::stringstream out;
  JSData output, topicBundleFiles;
  if (!this->loadAndParseTopicList(out)) {
    output[WebAPI::result::error] = out.str();
    return output;
  }
  topicBundleFiles.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->topics.loadedTopicBundleFiles.size; i ++) {
    topicBundleFiles[i] = this->topics.loadedTopicBundleFiles[i];
  }
  output["topicBundleFile"] = topicBundleFiles;
  output["children"].theType = JSData::token::tokenArray;
  for (int i = 0; i < this->topics.theTopics.size(); i ++) {
    TopicElement& currentElt = this->topics.theTopics.theValues[i];
    if (currentElt.type == TopicElement::types::chapter) {
      output["children"].theList.addOnTop(currentElt.toJSON(*this));
    }
  }
  return output;
}

void CalculatorHTML::interpretTableOfContents(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretTableOfContents");
  std::stringstream out;
  if (!this->loadAndParseTopicList(out)) {
    inputOutput.interpretedCommand = out.str();
    return;
  }
  bool sectionStarted = false;
  bool subSectionStarted = false;
  bool chapterStarted = false;
  out << "\n\n\n<!--Topic list automatically generated from topic list: " << this->topicListFileName
  << ".-->";
  out << "<a href=\"" << global.DisplayNameExecutable
  << "?request=template&fileName=" << this->fileName << "&"
  << "topicList=" << this->topicListFileName << "&" << "\">All topics</a>";
  out << "<ul>";
  for (int i = 0; i < this->topics.theTopics.size(); i ++) {
    TopicElement& currentElt = this->topics.theTopics.theValues[i];
    if (subSectionStarted) {
      if (
        currentElt.type == TopicElement::types::topic ||
        currentElt.type == TopicElement::types::section ||
        currentElt.type == TopicElement::types::chapter
      ) {
        out << "</li>";
      }
    }
    if (sectionStarted) {
      if (
        currentElt.type == TopicElement::types::section ||
        currentElt.type == TopicElement::types::chapter
      ) {
        out << "</ul></li>";
      }
    }
    if (chapterStarted) {
      if (currentElt.type == TopicElement::types::chapter) {
        out << "</ul></li>";
      }
    }
    if (currentElt.type == TopicElement::types::chapter) {
      out << "<li>" << "<a href=\"" << global.DisplayNameExecutable
      << "?request=template&fileName=" << this->fileName << "&"
      << "topicList=" << this->topicListFileName << "&" << "chapter =" << currentElt.title
      << "\">" << currentElt.title << "</a>" << "<br>\n";
      chapterStarted = true;
      sectionStarted = false;
      subSectionStarted = false;
      out << "<ul>";
    } else if (currentElt.type == TopicElement::types::section) {
      out << "<li>" << currentElt.title << "<br>\n";
      sectionStarted = true;
      subSectionStarted = false;
      out << "<ul>";
    } else if (currentElt.type == TopicElement::types::error) {
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

void CalculatorHTML::interpretJavascripts(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretJavascripts");
  std::string javascriptName = StringRoutines::stringTrimWhiteSpace(inputOutput.content);
  if (javascriptName == "MathJax") {
    inputOutput.interpretedCommand = HtmlRoutines::getJavascriptMathjax("");
  }
}

int TopicElement::scoreButtonCounter = 0;

bool CalculatorHTML::computeTopicListAndPointsEarned(std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("CalculatorHTML::computeTopicListAndPointsEarned");
  if (!this->loadAndParseTopicList(commentsOnFailure)) {
    return false;
  }
  if (!this->loadDatabaseInfo(commentsOnFailure)) {
    commentsOnFailure << "Error loading problem history. ";
  }
  if (!this->prepareSectionList(commentsOnFailure)) {
    commentsOnFailure << "Error preparing section list. ";
  }
  if (global.flagDatabaseCompiled) {
    this->flagIncludeStudentScores =
    global.userDefaultHasAdminRights() &&
    !global.userStudentVieWOn() &&
    global.requestType != "templateNoLogin";
    HashedList<std::string, MathRoutines::HashString> gradableProblems;
    for (int i = 0; i < this->topics.theTopics.size(); i ++) {
      if (this->topics.theTopics.theValues[i].type == TopicElement::types::problem) {
        gradableProblems.addOnTopNoRepetition(this->topics.theTopics.theValues[i].id);
        if (this->topics.theTopics.theValues[i].immediateChildren.size > 0) {
          global.fatal << "Error: problem " << this->topics.theTopics.theValues[i].toString()
          << " has children topics which is not allowed. "
          << global.fatal;
        }
      }
    }
    this->currentUseR.computePointsEarned(gradableProblems, &this->topics.theTopics, commentsOnFailure);
  }
  this->topics.initializeElementTypes();
  return true;
}

void CalculatorHTML::interpretTopicList(SyntacticElementHTML& inputOutput) {
  MacroRegisterFunctionWithName("CalculatorHTML::interpretTopicList");
  inputOutput.interpretedCommand = "<topicList></topicList>";
}

JSData LaTeXCrawler::FileWithOption::toJSON() {
  JSData result;
  result[WebAPI::request::slides::slideFilename] = this->fileName;
  if (this->isSolution) {
    result[WebAPI::request::slides::isSolution] = "true";
  }
  return result;
}

bool LaTeXCrawler::FileWithOption::fromJSON(JSData& input, std::stringstream* commentsOnFailure) {
  JSData& file = input[WebAPI::request::slides::slideFilename];
  if (file.theType != JSData::token::tokenString) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "While parsing file, failed to find key: " << WebAPI::request::slides::slideFilename << ". ";
    }
    return false;
  }
  this->fileName = file.theString;
  this->isSolution = false;
  if (input[WebAPI::request::slides::isSolution].theString == "true") {
    this->isSolution = true;
  }
  return true;
}

void LaTeXCrawler::Slides::AddSlidesOnTop(const List<std::string>& input) {
  for (int i = 0; i < input.size; i ++) {
    this->filesToCrawl.addOnTop(LaTeXCrawler::FileWithOption(input[i]));
  }
}

JSData LaTeXCrawler::Slides::toJSON() {
  JSData result;
  result[WebAPI::request::slides::title] = this->title;
  JSData theFiles;
  theFiles.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->filesToCrawl.size; i ++) {
    theFiles.theList.addOnTop(this->filesToCrawl[i].toJSON());
  }
  result[WebAPI::request::slides::files] = theFiles;
  return result;
}

bool LaTeXCrawler::Slides::FromJSON(
  JSData& input, std::stringstream* commentsOnFailure
) {
  if (!input[WebAPI::request::slides::title].isString(&this->title)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "slideTitle entry missing or not a string. ";
    }
    return false;
  }
  JSData& files = input[WebAPI::request::slides::files];
  if (files.theType != JSData::token::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "files entry missing or not a list. ";
    }
    return false;
  }
  this->filesToCrawl.setSize(files.theList.size);
  for (int i = 0; i < files.theList.size; i ++) {
    if (!this->filesToCrawl[i].fromJSON(files.theList[i], commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to extract file from entry index " << i << ". ";
      }
      return false;
    }
  }
  return  true;
}

bool LaTeXCrawler::Slides::FromString(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  if (input == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Slide specification string is empty. ";
    }
    return false;
  }
  std::string decoded;
  HtmlRoutines::convertURLStringToNormal(input, decoded, false);
  JSData parsed;
  if (!parsed.readstring(decoded, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse your input in "
      << "LaTeXCrawler::Slides::FromString. ";
    }
    return false;
  }
  return this->FromJSON(parsed, commentsOnFailure);
}

JSData TopicElement::computeSlidesJSON(CalculatorHTML& owner) {
  LaTeXCrawler::Slides theSlides;
  theSlides.AddSlidesOnTop(owner.slidesSourcesHeaders);
  theSlides.AddSlidesOnTop(this->sourceSlides);
  theSlides.title = this->title;
  return theSlides.toJSON();
}

JSData TopicElement::computeHomeworkJSON(CalculatorHTML& owner) {
  LaTeXCrawler::Slides theSlides;
  theSlides.AddSlidesOnTop(owner.sourcesHomeworkHeaders);
  for (int i = 0; i < this->sourceHomework.size; i ++) {
    LaTeXCrawler::FileWithOption file;
    file.fileName = this->sourceHomework[i];
    if (i < this->sourceHomeworkIsSolution.size) {
      file.isSolution = this->sourceHomeworkIsSolution[i];
    }
    theSlides.filesToCrawl.addOnTop(file);
  }
  theSlides.title = this->title;
  return theSlides.toJSON();
}


void TopicElement::computeHomework(CalculatorHTML& owner) {
  MacroRegisterFunctionWithName("TopicElement::computeHomework");
  if (this->sourceHomework.size == 0) {
    return;
  }
  JSData resultJSON = this->computeHomeworkJSON(owner);
  this->queryHomework = WebAPI::request::slides::content + "=" + HtmlRoutines::convertStringToURLString(resultJSON.toString(), false);
}


void TopicElement::computeSlides(CalculatorHTML& owner) {
  MacroRegisterFunctionWithName("TopicElement::computeSlides");
  if (this->sourceSlides.size == 0) {
    return;
  }
  JSData resultJSON = this->computeSlidesJSON(owner);
  this->querySlides = WebAPI::request::slides::content + "=" + HtmlRoutines::convertStringToURLString(resultJSON.toString(), false);
}

void TopicElement::computeLinks(CalculatorHTML& owner, bool plainStyle) {
  MacroRegisterFunctionWithName("TopicElement::computeLinks");
  (void) plainStyle;
  if (this->displayProblemLink != "") {
    return;
  }
  int depth = 3;
  if (this->type == TopicElement::types::chapter) {
    depth = 0;
  }
  if (this->type == TopicElement::types::section) {
    depth = 1;
  }
  if (this->type == TopicElement::types::topic) {
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
    this->displayTitle = this->problemNumberString + "-" ;
  } else {
    this->displayTitle = this->problemNumberString + titleWithLectureNumber;
  }
  if (this->video == "" || this->video == "-" || this->video == "--") {
    this->displayVideoLink = "";
  } else {
    this->displayVideoLink = "<a href=\"" + this->video +
    "\" class ='videoLink' class = 'videoLink' target = '_blank'>Video</a>";
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
  this->computeSlides(owner);
  this->computeHomework(owner);
  bool problemSolved = false;
  bool returnEmptyStringIfNoDeadline = false;
  if (this->problemFileName == "") {
    if (this->type == TopicElement::types::problem) {
      this->displayProblemLink = "(theory)";
    } else {
      this->displayProblemLink = "";
    }
    this->displayScore = "";
    this->displayModifyWeight = "";
    problemSolved = false;
    returnEmptyStringIfNoDeadline = true;
  } else {
    //std::string theRawSQLink = global.DisplayNameExecutable +
    //"?request=scoredQuiz&fileName=" + this->problem;
    std::string theRawExerciseLink;
    theRawExerciseLink = global.DisplayNameExecutable + "?request=exercise&fileName=" + this->problemFileName;
    this->displayProblemLink = owner.toStringLinkFromFileName(this->problemFileName);
    this->displayScore = owner.toStringProblemScoreShort(this->problemFileName, problemSolved);
  }
  if (this->problemFileName == "" && this->type == TopicElement::types::problem) {
    this->displayDeadlinE = "";
  } else {
    this->displayDeadlinE = owner.toStringDeadline(
      this->id, problemSolved, returnEmptyStringIfNoDeadline, (this->type != TopicElement::types::problem)
    );
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

JSData TopicElement::toJSON(CalculatorHTML& owner) {
  MacroRegisterFunctionWithName("TopicElement::toJSON");
  JSData output;
  output["title"] = this->title;
  std::string elementType = owner.topics.elementNames.GetValueCreate(this->type);
  if (elementType == "") {
    elementType = "not documented";
  }
  output["type"] = elementType;
  output["children"].theType = JSData::token::tokenArray;
  this->computeLinks(owner, true);
  if (this->type == TopicElement::types::problem && this->immediateChildren.size > 0) {
    global.fatal << "Error: Problem " << this->toString() << " reported to have children topic elements: "
    << this->immediateChildren.toStringCommaDelimited() << global.fatal;
  }
  for (int i = 0; i < this->immediateChildren.size; i ++) {
    TopicElement& currentChild = owner.topics.theTopics.theValues[this->immediateChildren[i]];
    output["children"].theList.addOnTop(currentChild.toJSON(owner));
  }
  output["problemNumberString"] = this->problemNumberString;
  output["video"] = this->video;
  output["videoHandwritten"] = this->videoHandwritten;
  if (this->querySlides != "") {
    output["querySlides"] = this->querySlides;
  }
  if (this->queryHomework != "") {
    output[WebAPI::request::slides::queryHomework] = this->queryHomework;
  }
  output[DatabaseStrings::labelDeadlines] = this->deadlinesPerSectioN;
  if (!global.userDefaultHasProblemComposingRights()) {
    std::string theDeadline = owner.getDeadlineNoInheritance(this->id);
    output[WebAPI::problem::deadlineSingle] = theDeadline;
  }
  output["handwrittenSolution"] = this->handwrittenSolution;

  output[WebAPI::problem::fileName] = this->problemFileName;
  output[WebAPI::problem::idProblem] = this->id;
  if (global.flagDatabaseCompiled) {
    if (owner.currentUseR.theProblemData.contains(this->problemFileName)) {
      ProblemData& currentData = owner.currentUseR.theProblemData.GetValueCreate(this->problemFileName);
      output["correctlyAnswered"] = currentData.numCorrectlyAnswered;
      output["totalQuestions"] = currentData.theAnswers.size();
      Rational currentWeight;
      std::string currentWeightAsGivenByInstructor;
      currentData.flagProblemWeightIsOK = currentData.adminData.getWeightFromCourse(
        owner.currentUseR.courseComputed, currentWeight, &currentWeightAsGivenByInstructor
      );
      if (currentData.flagProblemWeightIsOK) {
        output["weight"] = currentWeightAsGivenByInstructor;
      }
    }
  }
  return output;
}

std::string TopicElement::toString() const {
  std::stringstream out;
  out << this->title << ", id: " << this->id << " ";
  if (this->title == "") {
    out << "-";
  }
  if (this->type == TopicElement::types::problem) {
    out << "(problem)";
  }
  if (this->type == TopicElement::types::chapter) {
    out << "(chapter)";
  }
  if (this->type == TopicElement::types::section) {
    out << "(section)";
  }
  if (this->type == TopicElement::types::topic) {
    out << "(subsection)";
  }
  out << ". Index in parent: " << this->indexInParent;
  out << ". Parents: " << this->parentTopics.toStringCommaDelimited()
  << ". Immediate children: " << this->immediateChildren.toStringCommaDelimited() << ". ";
  return out.str();
}
