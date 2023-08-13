#include "calculator_problem_storage.h"
#include "string_constants.h"

ProblemData::ProblemData() {
  this->randomSeed = 0;
  this->flagRandomSeedGiven = false;
  this->totalCorrectlyAnswered = 0;
  this->totalSubmissions = 0;
  this->expectedNumberOfAnswersFromDB = 0;
  this->knownNumberOfAnswersFromHD = - 1;
  this->flagProblemWeightIsOK = false;
}

std::string ProblemData::toStringAvailableAnswerIds() {
  std::stringstream out;
  out << "Available answer ids: ";
  for (int i = 0; i < this->answers.size(); i ++) {
    out << this->answers.values[i].answerId;
    if (i != this->answers.size() - 1) {
      out << ", ";
    }
  }
  return out.str();
}

bool ProblemDataAdministrative::getWeightFromCourse(
  const std::string& courseNonURLed,
  Rational& output,
  std::string* outputAsGivenByInstructor
) {
  STACK_TRACE("ProblemDataAdministrative::getWeightFromCourse");
  if (!this->problemWeightsPerCourse.contains(courseNonURLed)) {
    return false;
  }
  std::string tempString;
  if (outputAsGivenByInstructor == nullptr) {
    outputAsGivenByInstructor = &tempString;
  }
  *outputAsGivenByInstructor =
  this->problemWeightsPerCourse.getValueCreateEmpty(courseNonURLed);
  return output.assignStringFailureAllowed(*outputAsGivenByInstructor);
}

std::string ProblemDataAdministrative::toString() const {
  std::stringstream out;
  out << this->deadlinesPerSection.toStringHtml();
  out << this->problemWeightsPerCourse.toStringHtml();
  return out.str();
}

bool ProblemData::checkConsistency() const {
  STACK_TRACE("ProblemData::checkConsistency");
  for (int i = 0; i < this->answers.size(); i ++) {
    if (
      StringRoutines::stringTrimWhiteSpace(
        this->answers.values[i].answerId
      ) ==
      ""
    ) {
      global.fatal
      << "This is not supposed to happen: empty answer id."
      << global.fatal;
    }
  }
  return true;
}

bool ProblemData::checkConsistencyMathQuillIds() const {
  STACK_TRACE("ProblemData::checkConsistencyMathQuillIds");
  for (int i = 0; i < this->answers.size(); i ++) {
    if (
      StringRoutines::stringTrimWhiteSpace(
        this->answers.values[i].idMathEquationField
      ) ==
      ""
    ) {
      std::stringstream errorStream;
      errorStream
      << "This is not supposed to happen: empty idMQField. The answer id is: "
      << this->answers.values[i].answerId
      << "<br>"
      << this->toString()
      << "<hr>Answer information: "
      << this->toString()
      << "<br>";
      global.fatal << errorStream.str() << global.fatal;
    }
  }
  return true;
}

std::string ProblemData::toString() const {
  std::stringstream out;
  out << "Problem data. " << "Random seed: " << this->randomSeed;
  if (this->flagRandomSeedGiven) {
    out << " (given)";
  }
  out << ". <br>";
  for (int i = 0; i < this->answers.size(); i ++) {
    Answer& currentA = this->answers.values[i];
    out << "AnswerId: " << currentA.answerId;
    out << ", numCorrectSubmissions: " << currentA.numberOfCorrectSubmissions;
    out << ", numSubmissions: " << currentA.numberOfSubmissions;
    out << ", firstCorrectAnswer: ";
    if (currentA.firstCorrectAnswerClean == "") {
      out << "[none yet], ";
    } else {
      out << "[" << currentA.firstCorrectAnswerClean << "], ";
    }
    out << "<br>";
  }
  out << "Administrator data: " << this->adminData.toString();
  return out.str();
}

std::string ProblemData::store() {
  STACK_TRACE("ProblemData::store");
  std::stringstream out;
  if (this->flagRandomSeedGiven) {
    out << "randomSeed=" << this->randomSeed;
  }
  for (int i = 0; i < this->answers.size(); i ++) {
    Answer& currentA = this->answers.values[i];
    if (this->flagRandomSeedGiven || i != 0) {
      out << "&";
    }
    out
    << HtmlRoutines::convertStringToURLString(currentA.answerId, false)
    << "=";
    std::stringstream questionsStream;
    questionsStream
    << "numCorrectSubmissions="
    << currentA.numberOfCorrectSubmissions
    << "&numSubmissions="
    << currentA.numberOfSubmissions
    << "&firstCorrectAnswer="
    << HtmlRoutines::convertStringToURLString(
      currentA.firstCorrectAnswerClean, false
    );
    out
    << HtmlRoutines::convertStringToURLString(
      questionsStream.str(), false
    );
  }
  return out.str();
}

JSData ProblemData::storeJSON() const {
  STACK_TRACE("ProblemData::storeJSON");
  JSData result;
  result.elementType = JSData::Token::tokenObject;
  if (this->flagRandomSeedGiven) {
    std::stringstream stringConverter;
    stringConverter << this->randomSeed;
    // Store random seed as string to avoid type conversion issues.
    result[WebAPI::Problem::randomSeed] = stringConverter.str();
  }
  for (int i = 0; i < this->answers.size(); i ++) {
    Answer& currentA = this->answers.values[i];
    JSData currentAnswerJSON;
    currentAnswerJSON["numCorrectSubmissions"] =
    std::to_string(currentA.numberOfCorrectSubmissions);
    currentAnswerJSON["numSubmissions"] =
    std::to_string(currentA.numberOfSubmissions);
    currentAnswerJSON["firstCorrectAnswer"] =
    HtmlRoutines::convertStringToURLString(
      currentA.firstCorrectAnswerClean, false
    );
    result[
      HtmlRoutines::convertStringToURLString(currentA.answerId, false)
    ] =
    currentAnswerJSON;
  }
  return result;
}

bool ProblemData::loadFromOldFormatDeprecated(
  const std::string& inputData, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("ProblemData::loadFromOldFormatDeprecated");
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > mapStrings;
  if (
    !HtmlRoutines::chopPercentEncodedString(
      inputData, mapStrings, commentsOnFailure
    )
  ) {
    return false;
  }
  this->points = 0;
  this->totalCorrectlyAnswered = 0;
  this->totalSubmissions = 0;
  this->flagRandomSeedGiven = false;
  if (global.userRequestRequiresLoadingRealExamData()) {
    if (mapStrings.contains(WebAPI::Problem::randomSeed)) {
      global.comments << "Loading random seed from old format.";
      this->randomSeed = static_cast<uint32_t>(
        atoi(
          mapStrings.getValueCreateEmpty(WebAPI::Problem::randomSeed).c_str()
        )
      );
      this->flagRandomSeedGiven = true;
    }
  }
  this->answers.clear();
  bool result = true;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > currentQuestionMap;
  for (int i = 0; i < mapStrings.size(); i ++) {
    if (mapStrings.keys[i] == WebAPI::Problem::randomSeed) {
      continue;
    }
    Answer answer;
    answer.answerId = mapStrings.keys[i];
    this->answers.setKeyValue(answer.answerId, answer);
    Answer& currentA = *this->answers.values.lastObject();
    std::string currentQuestion =
    HtmlRoutines::convertURLStringToNormal(mapStrings.values[i], false);
    result =
    HtmlRoutines::chopPercentEncodedString(
      currentQuestion, currentQuestionMap, commentsOnFailure
    );
    if (!result) {
      commentsOnFailure
      << "Failed to interpret as key-value pair: "
      << currentQuestion
      << ". ";
      continue;
    }
    if (currentQuestionMap.contains("numCorrectSubmissions")) {
      currentA.numberOfCorrectSubmissions =
      atoi(
        currentQuestionMap.getValueCreateEmpty("numCorrectSubmissions").c_str()
      );
    }
    if (currentQuestionMap.contains("numSubmissions")) {
      currentA.numberOfSubmissions =
      atoi(
        currentQuestionMap.getValueCreateEmpty("numSubmissions").c_str()
      );
    }
    if (currentQuestionMap.contains("firstCorrectAnswer")) {
      currentA.firstCorrectAnswerURLed =
      currentQuestionMap.getValueCreateEmpty("firstCorrectAnswer");
      currentA.firstCorrectAnswerClean =
      HtmlRoutines::convertURLStringToNormal(
        currentA.firstCorrectAnswerURLed, false
      );
      currentA.firstCorrectAnswerURLed =
      HtmlRoutines::convertStringToURLString(
        currentA.firstCorrectAnswerClean, false
      );
      // url-encoding back the cleaned up answer:
      // this protects from the possibility that
      // currentA.firstCorrectAnswerURLed was not encoded properly.
    }
  }
  //  this->checkConsistency();
  return result;
}

bool ProblemData::loadFromJSON(
  const JSData& inputData, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("ProblemData::loadFromJSON");
  (void) commentsOnFailure;
  this->points = 0;
  this->totalCorrectlyAnswered = 0;
  this->totalSubmissions = 0;
  this->flagRandomSeedGiven = false;
  this->randomSeed = - 1;
  if (global.userRequestRequiresLoadingRealExamData()) {
    if (inputData.objects.contains(WebAPI::Problem::randomSeed)) {
      this->randomSeed = static_cast<uint32_t>(
        atoi(
          inputData.objects.getValueNoFail(WebAPI::Problem::randomSeed).
          stringValue.c_str()
        )
      );
      this->flagRandomSeedGiven = true;
    }
  }
  this->answers.clear();
  bool result = true;
  for (int i = 0; i < inputData.objects.size(); i ++) {
    if (inputData.objects.keys[i] == WebAPI::Problem::randomSeed) {
      continue;
    }
    Answer newAnswer;
    newAnswer.answerId = inputData.objects.keys[i];
    this->answers.setKeyValue(newAnswer.answerId, newAnswer);
    Answer& currentA = *this->answers.values.lastObject();
    JSData currentQuestionJSON = inputData.objects.values[i];
    if (currentQuestionJSON.objects.contains("numCorrectSubmissions")) {
      currentA.numberOfCorrectSubmissions =
      atoi(
        currentQuestionJSON.objects.getValueNoFail("numCorrectSubmissions").
        stringValue.c_str()
      );
    }
    if (currentQuestionJSON.objects.contains("numSubmissions")) {
      currentA.numberOfSubmissions =
      atoi(
        currentQuestionJSON.objects.getValueNoFail("numSubmissions").
        stringValue.c_str()
      );
    }
    if (currentQuestionJSON.objects.contains("firstCorrectAnswer")) {
      currentA.firstCorrectAnswerURLed =
      currentQuestionJSON.objects.getValueNoFail("firstCorrectAnswer").
      stringValue;
      currentA.firstCorrectAnswerClean =
      HtmlRoutines::convertURLStringToNormal(
        currentA.firstCorrectAnswerURLed, false
      );
      currentA.firstCorrectAnswerURLed =
      HtmlRoutines::convertStringToURLString(
        currentA.firstCorrectAnswerClean, false
      );
      // url-encoding back the cleaned up answer:
      // this protects from the possibility that
      // currentA.firstCorrectAnswerURLed was not encoded properly,
      // say, by an older version of the calculator
    }
  }
  //  this->checkConsistency();
  return result;
}
