// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "math_general_implementation.h"
#include "math_extra_Lie_theory_extras.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_drawing_variables.h"
#include <assert.h>
#include "calculator_interface.h"
#include "system_functions_global_objects.h"
#include "string_constants.h"

std::string UserCalculatorData::Roles::administator = "admin";
std::string UserCalculatorData::Roles::student = "student";
std::string UserCalculatorData::Roles::instructor = "instructor";
std::string UserCalculatorData::Roles::teacher = "teacher";

void fatalCrash(const std::string& input) {
  global.fatal << input << global.fatal;
}

GlobalVariables::Crasher::Crasher() {
  this->flagCrashInitiated = false;
  this->flagFinishingCrash = false;
}

void GlobalVariables::Crasher::firstRun() {
  if (
    !this->flagCrashInitiated &&
    global.flagRunningBuiltInWebServer
  ) {
    double elapsedSeconds = global.getElapsedSeconds();
    this->crashReportHtml << "\n<b style = 'color:red'>Crash</b> "
    << elapsedSeconds << " second(s) from the start.<hr>";
    this->crashReportConsolE << Logger::consoleRed() << "Crash " << elapsedSeconds
    << " second(s)" << Logger::consoleNormal() << " from the start.\n";
    this->crashReportFile << "Crash " << elapsedSeconds
    << " second(s) from the start.\n";
  }
  this->flagCrashInitiated = true;
}

GlobalVariables::Crasher& GlobalVariables::Crasher::operator<<(const GlobalVariables::Crasher& dummyCrasherSignalsActualCrash) {
  (void) dummyCrasherSignalsActualCrash;
  this->firstRun();
  if (this->flagFinishingCrash) {
    std::cout << "Recursion within the crashing mechanism detected. "
    << "Something is very wrong. "
    << this->crashReportConsolE.str() << std::endl;
    std::exit(- 1);
    assert(false);
  }
  this->crashReportConsolE << this->crashReport.str();
  this->crashReportHtml << this->crashReport.str();
  this->crashReportFile << this->crashReport.str();

  this->flagFinishingCrash = true;
  if (!global.flagNotAllocated) {
    this->crashReportConsolE << "\n";
  }
  if (!global.flagNotAllocated) {
    if (global.userInputStringIfAvailable != "") {
      this->crashReportHtml << "<hr>User input: <br> "
      << global.userInputStringIfAvailable << "<hr>";
      this->crashReportConsolE << "User input: "
      << Logger::consoleBlue() << global.userInputStringIfAvailable
      << Logger::consoleNormal() << "\n";
      this->crashReportFile << "User input:\n" << global.userInputStringIfAvailable << "\n";
    }
  }
  this->crashReportConsolE << Crasher::getStackTraceEtcErrorMessageConsole();
  this->crashReportHtml << Crasher::getStackTraceEtcErrorMessageHTML();
  this->crashReportFile << Crasher::getStackTraceEtcErrorMessageHTML();
  if (!global.flagNotAllocated) {
    if (global.progressReportStrings.size > 0) {
      this->crashReportHtml
      << "<hr><b>Computation progress report strings:</b><br>"
      << global.toStringProgressReportNoThreadData(true);
      this->crashReportFile
      << "<hr><b>Computation progress report strings:</b><br>"
      << global.toStringProgressReportNoThreadData(true);

      this->crashReportConsolE << "Computation progress strings:\n";
      this->crashReportConsolE << global.toStringProgressReportNoThreadData(false);
    }
  }
  if (!global.flagNotAllocated) {
    if (!global.calculator().isZeroPointer()) {
      if (global.calculator().getElement().comments.str() != "") {
        this->crashReportHtml << "<hr>Additional comments follow. " << global.calculator().getElement().comments.str();
      }
    }
    std::fstream theFile;
    bool openSuccess = FileOperations::openFileCreateIfNotPresentVirtual(
      theFile,
      "crashes/" + global.relativePhysicalNameCrashReport,
      false,
      true,
      false,
      true,
      true
    );
    if (openSuccess) {
      this->crashReportHtml << "<hr>Crash dumped in folder "
      << "results/crashes/. Not visible through the web server. "
      << "If running locally, simply open the results/crashes "
      << "folder within your calculator folder. "
      << "If running remotely, you will need an ssh connection. ";
      this->crashReportConsolE << "Crash dumped in file: " << Logger::consoleGreen()
      << global.relativePhysicalNameCrashReport << Logger::consoleNormal() << "\n";
    } else {
      this->crashReportHtml << "<hr>Failed to open crash report file: "
      << global.relativePhysicalNameCrashReport
      << ". check file permissions. ";
      this->crashReportConsolE << "Failed to open crash report file: "
      << Logger::consoleRed()
      << global.relativePhysicalNameCrashReport
      << Logger::consoleNormal() << "\n";
    }
    theFile << this->crashReportFile.str();
    theFile.flush();
    theFile.close();
  } else {
    this->crashReportHtml << "GlobalVariables.flagNotAllocated is true. ";
    this->crashReportConsolE << "GlobalVariables.flagNotAllocated is true. ";
  }
  std::cout << this->crashReportConsolE.str() << std::endl;
  JSData output;
  output[WebAPI::result::crashReport] = this->crashReportHtml.str();
  output[WebAPI::result::comments] = global.comments.getCurrentReset();
  global.theResponse.writeResponse(output, true);
  assert(false);
  return *this;
}

std::string GlobalVariables::Crasher::getStackTraceEtcErrorMessageHTML() {
  std::stringstream out;
  out << "A partial stack trace follows (function calls not explicitly logged not included).";
  out << "<table><tr>";
  for (int threadCounter = 0; threadCounter < global.customStackTrace.size; threadCounter ++) {
    if (threadCounter >= global.theThreadData.size) {
      out << "<td><b>WARNING: the stack trace reports "
      << global.customStackTrace.size
      << " threads but the thread data array has record of only "
      << global.theThreadData.size
      << " threads. " << "</b></td>";
      break;
    }
    out << "<td>" << global.theThreadData[threadCounter].toStringHtml() << "</td>";
  }
  out << "</tr> <tr>";
  for (int threadCounter = 0; threadCounter<global.customStackTrace.size; threadCounter ++) {
    if (threadCounter >= global.theThreadData.size) {
      break;
    }
    if (ThreadData::getCurrentThreadId() != threadCounter) {
      out << "<td>Stack trace available only for current thread.</td>";
      //<-to avoid coordinating threads
      continue;
    }
    ListReferences<StackInfo>& currentInfo = global.customStackTrace[threadCounter];
    out << "<td><table><tr><td>file</td><td>line</td><td>function name (if known)</td></tr>";
    for (int i = currentInfo.size - 1; i >= 0; i --) {
      out << "<tr><td>" << HtmlRoutines::getHtmlLinkFromProjectFileName(currentInfo[i].fileName, "", currentInfo[i].line)
      << "</td><td>" << currentInfo[i].line << "</td>";
      if (currentInfo[i].functionName != "") {
        out << "<td>" << currentInfo[i].functionName << "</td>";
      }
      out << "</tr>";
    }
    out << "</table></td>";
  }
  out << "</tr></table>";
  return out.str();
}

std::string GlobalVariables::Crasher::getStackTraceEtcErrorMessageConsole() {
  std::stringstream out;
  for (int threadCounter = 0; threadCounter<global.customStackTrace.size; threadCounter ++) {
    if (threadCounter >= global.theThreadData.size) {
      out << "WARNING: stack trace reports " << global.customStackTrace.size << " threads "
      << "while I have only " << global.theThreadData.size << " registered threads. ";
      break;
    }
    out << "********************\r\nThread index " << threadCounter << ": \r\n";
    if (ThreadData::getCurrentThreadId() != threadCounter) {
      out << "Stack trace available only for current thread.\n";
      //<-to avoid coordinating threads
      continue;
    }
    ListReferences<StackInfo>& currentInfo = global.customStackTrace[threadCounter];
    for (int i = currentInfo.size - 1; i >= 0; i --) {
      out << currentInfo[i].functionName << "\n";
    }
  }
  return out.str();
}

std::string GlobalVariables::toHTMLTopCommandLinuxSystem() {
  MacroRegisterFunctionWithName("GlobalVariables::toHTMLTopCommandLinuxSystem");
  if (!global.userDefaultHasAdminRights()) {
    return "Login as admin for RAM memory statistics.";
  }
  std::string topString = this->externalCommandReturnOutput("top -b -n 1 -s");
  std::stringstream out;
  std::string lineString, wordString;
  std::stringstream topStream(topString);
  for (int i = 0; i < 4; i ++) {
    std::getline(topStream, lineString);
    out << lineString << "<br>\n ";
  }
  out << "<table>";
  for (; std::getline(topStream, lineString);) {
    out << "<tr>";
    for (std::stringstream nextLineStream(lineString); nextLineStream >> wordString;) {
      out << "<td>" << wordString << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string GlobalVariables::toStringFolderInfo() const {
  std::stringstream out;
  out << "<br>Physical path server base: " << this->physicalPathServerBase;
  out << "<br>Diplay name executable: " << this->displayNameExecutable;
  out << "<br>Physical name folder below executable: " << this->physicalNameFolderExecutable;
  out << "<br>Display path output folder: " << this->displayPathOutputFolder;
  return out.str();
}

std::string GlobalVariables::toStringThreadData(bool useHTML) {
  std::stringstream out;
  for (int threadIndex = 0; threadIndex < this->progressReportStrings.size; threadIndex ++) {
    if (useHTML) {
      out << "<hr><b>";
    }
    out << this->theThreadData[threadIndex].toStringHtml();
    if (useHTML) {
      out << "</b><br>";
    }
    out << "\n";
  }
  return out.str();
}

std::string GlobalVariables::toStringProgressReportWithThreadData(bool useHTML) {
  MacroRegisterFunctionWithName("GlobalVariables::ToStringProgressReportHtmlWithThreadData");
  std::stringstream out;
  out << global.toStringThreadData(useHTML);
  out << global.toStringProgressReportNoThreadData(useHTML);
  return out.str();
}

std::string GlobalVariables::toStringProgressReportNoThreadData(bool useHTML) {
  MacroRegisterFunctionWithName("GlobalVariables::toStringProgressReportNoThreadData");

  std::stringstream reportStream;
  for (int threadIndex = 0; threadIndex < this->progressReportStrings.size; threadIndex ++) {
    int currentThreadID = ThreadData::getCurrentThreadId();
    if (currentThreadID != threadIndex) {
      //<-to avoid coordinating threads
      continue;
    }
    if (useHTML) {
      reportStream << "<b>Current thread id: "
      << currentThreadID
      << ". </b>";
    } else {
      reportStream << "Thread id: " << Logger::consoleBlue() << currentThreadID << Logger::consoleNormal() << "\n";
    }
    for (int i = 0; i < this->progressReportStrings[threadIndex].size; i ++) {
      if (this->progressReportStrings[threadIndex][i] != "") {
        if (useHTML) {
          reportStream << "\n<div id = \"divProgressReport" << i << "\">"
          << this->progressReportStrings[threadIndex][i] << "\n</div>\n<hr>";
        } else {
          reportStream << this->progressReportStrings[threadIndex][i] << "\n";
        }
      }
    }
  }
  if (!global.fatal.flagCrashInitiated) {
    if (useHTML) {
      reportStream << global.fatal.getStackTraceEtcErrorMessageHTML();
    } else {
      reportStream << global.fatal.getStackTraceEtcErrorMessageConsole();
    }
    reportStream << global.getElapsedMilliseconds()
    << " ms elapsed. ";
    if (global.millisecondsMaxComputation > 0) {
      if (useHTML) {
        reportStream << "<br>";
      }
      reportStream << "\nHard limit: "
      << global.millisecondsMaxComputation
      << " ms [system crash if limit exceeded].";
      if (useHTML) {
        reportStream << "<br>";
      }
      reportStream << "\nSoft limit: "
      << global.millisecondsMaxComputation / 2
      << " ms [computation error if limit exceeded, triggered between calculator/atomic functions].";
    }
  }
  return reportStream.str();
}

std::string GlobalVariables::toStringProgressReportConsole() {
  MacroRegisterFunctionWithName("GlobalVariables::toStringProgressReportConsole");
  std::stringstream reportStream;
  for (int threadIndex = 0; threadIndex < this->progressReportStrings.size; threadIndex ++) {
    if (ThreadData::getCurrentThreadId() != threadIndex) {
      reportStream << "Progress report available only for current thread.<br>";
      //<-to avoid coordinating threads
      continue;
    }
    reportStream << this->theThreadData[threadIndex].toStringConsole();
    for (int i = 0; i < this->progressReportStrings[threadIndex].size; i ++) {
      reportStream << this->progressReportStrings[threadIndex][i];
    }
  }
  reportStream << "\n";
  return reportStream.str();
}

void GlobalVariables::initThreadsExecutableStart() {
  //<-Stack trace forbidden this is running before anything has been initialized!
  ThreadData::registerFirstThread("main");
}

void GlobalVariables::initFoldersProjectBase(const std::string& inputPhysicalExecutable) {
  StateMaintainerCurrentFolder preserveCurrentFolder;
  this->physicalPathProjectBase = FileOperations::getPathFromFileNameWithPath(inputPhysicalExecutable) + "./";
  this->changeDirectory(this->physicalPathProjectBase);
  this->physicalPathProjectBase = FileOperations::getCurrentFolder() + "/";
}

void GlobalVariables::initDefaultFolderAndFileNames() {
  this->initFoldersProjectBase(global.pathExecutableUserInputOrDeduced);
  this->physicalNameFolderExecutable = this->physicalPathProjectBase;
  this->physicalNameExecutableNoPath = FileOperations::getFileNameFromFileNameWithPath(global.pathExecutableUserInputOrDeduced);
  this->physicalNameExecutableWithPath = this->physicalNameFolderExecutable + this->physicalNameExecutableNoPath;
  this->physicalPathServerBase = this->physicalPathProjectBase;
  this->displayPathOutputFolder = "/output/";

  this->displayNameExecutable = "/cgi-bin/" + this->physicalNameExecutableNoPath;
  this->displayApplication = "/" + WebAPI::app;
  this->displayNameExecutableAppNoCache = "/" + WebAPI::appNoCache;
  this->initOutputReportAndCrashFileNames("", "");
}

void GlobalVariables::setWebInput(const std::string& inputName, const std::string& inputValue) {
  MacroRegisterFunctionWithName("GlobalVariables::SetWebInput");
  this->webArguments.setKeyValue(inputName, inputValue);
}

bool GlobalVariables::userSecureNonAdminOperationsAllowed() {
  return this->flagLoggedIn && this->flagUsingSSLinCurrentConnection;
}

bool GlobalVariables::userDebugFlagOn() {
  return global.getWebInput(WebAPI::request::debugFlag) == "true";
}

bool GlobalVariables::userStudentVieWOn() {
  return global.getWebInput("studentView") == "true";
}

std::string GlobalVariables::LogData::toStringProcessType() const {
  switch (this->logType) {
  case GlobalVariables::LogData::type::server:
    return "server";
  case GlobalVariables::LogData::type::serverMonitor:
    return "server monitor";
  case GlobalVariables::LogData::type::worker:
    return "worker";
  case GlobalVariables::LogData::type::daemon:
    return "daemon";
  default:
    return "uknown process type";
  }
}

bool GlobalVariables::checkConsistency() {
  if (this->flagDeallocated) {
    global.fatal << "Global variables not allowed to be deallocated. " << global.fatal;
  }
  return true;
}

bool GlobalVariables::userDefaultIsDebuggingAdmin() {
  return this->userDefaultHasAdminRights() && this->userDebugFlagOn();
}

bool GlobalVariables::userDefaultHasAdminRights() {
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return true;
  }
  return this->flagLoggedIn && (this->userDefault.userRole == UserCalculatorData::Roles::administator);
}

bool GlobalVariables::userDefaultHasProblemComposingRights() {
  return this->flagLoggedIn && (
    this->userDefault.userRole == UserCalculatorData::Roles::administator ||
    this->userDefault.userRole == UserCalculatorData::Roles::teacher
  );
}

bool GlobalVariables::userGuestMode() {
  if (!this->flagUsingSSLinCurrentConnection) {
    return true;
  }
  return
  this->requestType == "exerciseNoLogin" ||
  this->requestType == WebAPI::request::problemGiveUpNoLogin ||
  this->requestType == WebAPI::request::submitExerciseNoLogin ||
  this->requestType == WebAPI::request::submitExercisePreviewNoLogin ||
  this->requestType == "templateNoLogin";
}

bool GlobalVariables::userRequestRequiresLoadingRealExamData() {
  if (this->userGuestMode()) {
    return false;
  }
  return this->flagLoggedIn && (
    this->requestType == "scoredQuiz" ||
    this->requestType == "scoredQuizJSON" ||
    this->requestType == "submitAnswers" ||
    this->requestType == "submitAnswersPreview"
  );
}

bool GlobalVariables::userRequestMustBePromptedToLogInIfNotLoggedIn() {
  return
  this->requestType == "scoredQuiz" ||
  this->requestType == "exercise";
}

std::string GlobalVariables::toStringCalculatorComputation(
  const std::string& computation,
  const std::string& display
) {
  std::stringstream out;
  JSData request;
  request[WebAPI::request::requestAPI] = WebAPI::request::compute;
  request[WebAPI::request::calculatorInput] = computation;
  request[WebAPI::request::currentPage] = WebAPI::request::calculatorPage;
  out << "<a href = \"" << global.displayApplication << "#"
  << HtmlRoutines::convertStringToURLString(request.toString(), false) << "\">";
  out << HtmlRoutines::convertStringToHtmlString(display, true) << "</a>";
  return out.str();
}

std::string GlobalVariables::toStringCalculatorArgumentsNoNavigation(List<std::string>* tagsToExclude) {
  MacroRegisterFunctionWithName("GlobalVariables::toStringCalculatorArgumentsNoNavigation");
  if (!this->flagLoggedIn && !this->userGuestMode()) {
    return "";
  }
  std::stringstream out;
  for (int i = 0; i < this->webArguments.size(); i ++) {
    const std::string& currentName = this->webArguments.theKeys[i];
    if (
      currentName == "request" ||
      currentName == "password" ||
      currentName == WebAPI::problem::fileName ||
      currentName == WebAPI::problem::courseHome ||
      currentName == WebAPI::problem::topicList ||
      currentName == "currentDatabaseTable" ||
      currentName == "mainInput" ||
      currentName == "studentView" ||
      currentName == "studentSection" ||
      currentName == "error" ||
      currentName == "navigationBar" ||
      currentName == "problemLinkStyle" ||
      currentName == "googleToken" ||
      currentName == "G_AUTHUSER_H" ||
      currentName == "mjx.menu" ||
      currentName == "username" ||
      currentName == "authenticationToken"
    ) {
      continue;
    }
    if (tagsToExclude != nullptr) {
      if (tagsToExclude->contains(currentName)) {
        continue;
      }
    }
    out << global.webArguments.theKeys[i] << "="
    << global.webArguments.theValues[i]
    << "&";
  }
  return out.str();
}

std::string GlobalVariables::getWebInput(const std::string& inputName) {
  return this->webArguments.getValueCreate(inputName);
}

void GlobalVariables::makeReport() {
  MacroRegisterFunctionWithName("GlobalVariables::makeReport");
  if (!global.theResponse.monitoringAllowed()) {
    return;
  }
  std::string reportString;
  if (this->flagRunningConsoleRegular || this->flagRunningConsoleTest) {
    reportString = this->toStringProgressReportConsole();
  } else {
    reportString = this->toStringProgressReportNoThreadData(true);
  }
  this->theResponse.report(reportString);
}

void GlobalVariables::initOutputReportAndCrashFileNames(
  const std::string& inputUserStringRAW, const std::string& inputUserStringCivilized
) {
  std::string inputAbbreviated;
  this->userInputStringIfAvailable =
  FileOperations::cleanUpForFileNameUse(
    inputUserStringCivilized
  );
  if (!global.flagUsingSSLinCurrentConnection) {
    this->userInputStringRAWIfAvailable = inputUserStringRAW;
    inputAbbreviated = this->userInputStringRAWIfAvailable;
  } else {
    this->userInputStringRAWIfAvailable = "Raw user input string not available in SSL mode. ";
    inputAbbreviated = this->userInputStringIfAvailable;
  }
  StringRoutines::stringTrimToLengthWithHash(inputAbbreviated, 150);
  this->relativePhysicalNameCrashReport = "crash_" + inputAbbreviated + ".html";
}

UserCalculatorData::UserCalculatorData() {
  this->approximateHoursSinceLastTokenWasIssued = - 1;
  this->flagEnteredAuthenticationToken = false;
  this->flagMustLogin = true;
  this->flagEnteredPassword = false;
  this->flagStopIfNoLogin = true;
  this->flagUserHasNoPassword = false;
  this->flagUserHasActivationToken = false;
  this->flagEnteredActivationToken = false;
}

void UserCalculatorData::reset() {
  MacroRegisterFunctionWithName("UserCalculatorData::reset");
  for (unsigned i = 0; i < this->username.size(); i ++) {
    this->username[i] = '*';
  }
  this->username = "";
  this->email = "";
  this->clearAuthenticationTokenAndPassword();
}

void UserCalculatorData::clearPasswordFromMemory() {
  MacroRegisterFunctionWithName("UserCalculatorData::clearPasswordFromMemory");
  for (unsigned i = 0; i < this->actualHashedSaltedPassword.size(); i ++) {
    this->actualHashedSaltedPassword[i] = ' ';
  }
  this->actualHashedSaltedPassword = "";
  for (unsigned i = 0; i < this->enteredPassword.size(); i ++) {
    this->enteredPassword[i] = ' ';
  }
  this->enteredPassword = "";
  for (unsigned i = 0; i < this->enteredHashedSaltedPassword.size(); i ++) {
    this->enteredHashedSaltedPassword[i] = ' ';
  }
  this->enteredHashedSaltedPassword = "";
  for (unsigned i = 0; i < this->actualActivationToken.size(); i ++) {
    this->actualActivationToken[i] = ' ';
  }
  this->actualActivationToken = "";
  for (unsigned i = 0; i < this->enteredActivationToken.size(); i ++) {
    this->enteredActivationToken[i] = ' ';
  }
  this->enteredActivationToken = "";
}

void UserCalculatorData::clearAuthenticationTokenAndPassword() {
  MacroRegisterFunctionWithName("UserCalculatorData::clearAuthenticationTokenAndPassword");
  this->clearPasswordFromMemory();
  for (unsigned i = 0; i < this->actualAuthenticationToken.size(); i ++) {
    this->actualAuthenticationToken[i] = ' ';
  }
  this->actualAuthenticationToken = "";
}

std::string UserCalculatorData::toStringCourseInfo() {
  std::stringstream out;
  out << "Course name:\n" << this->courseComputed
  << "\n<br>Deadline schema:\n" << this->deadlines.toString(nullptr)
  << "\n<hr>Problem weight schema:\n" << this->problemWeights.toString(nullptr);
  return out.str();
}

std::string UserCalculatorData::toStringUnsecure() {
  MacroRegisterFunctionWithName("UserCalculatorData::toStringUnsecure");
  std::stringstream out;
  out << "User: " << this->username << "\n<br>"
  << this->toStringCourseInfo()
  << "\n<br>Actual authentication token: "
  << this->actualAuthenticationToken
  << "\n<br>Entered authentication token: "
  << this->enteredAuthenticationToken
  << "\n<br>Entered password: "
  << this->enteredPassword
  << "\n<br>Entered activation token: "
  << this->enteredActivationToken
  << "\n<br>Actual activation token: "
  << this->actualActivationToken;
  return out.str();
}

template<>
List<Weight<RationalFunction<Rational> > >::Comparator*
FormatExpressions::getMonomialOrder<Weight<RationalFunction<Rational> > >() {
  return nullptr;
}

template<>
List<Weight<Rational> >::Comparator*
FormatExpressions::getMonomialOrder<Weight<Rational> >() {
  return nullptr;
}

void DynkinDiagramRootSubalgebra::swapDynkinStrings(int i, int j) {
  this->simpleComponentTypes.swapTwoIndices(i, j);
  this->SimpleBasesConnectedComponents.swapTwoIndices(i, j);
  this->indicesThreeNodes.swapTwoIndices(i, j);
  this->indicesEnds.swapTwoIndices(i, j);
}

void DynkinDiagramRootSubalgebra::sort() {
  //doing bubble sort -> shortest to code
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    for (int j = i + 1; j < this->SimpleBasesConnectedComponents.size; j ++) {
      bool tempBool = false;
      if (this->SimpleBasesConnectedComponents[i].size < this->SimpleBasesConnectedComponents[j].size) {
        tempBool = true;
      }
      if (this->SimpleBasesConnectedComponents[i].size == this->SimpleBasesConnectedComponents[j].size) {
        tempBool = ((this->simpleComponentTypes[i]) < (this->simpleComponentTypes[j]));
      }
      if (tempBool) {
        this->swapDynkinStrings(i, j);
      }
    }
  }
  this->sameTypeComponents.size = 0;
  this->indexInUniComponent.setSize(this->SimpleBasesConnectedComponents.size);
  this->indexUniComponent.setSize(this->SimpleBasesConnectedComponents.size);
  this->sameTypeComponents.reserve(this->SimpleBasesConnectedComponents.size);
  DynkinSimpleType tempType;
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    if (!(this->simpleComponentTypes[i] == tempType)) {
      this->sameTypeComponents.setSize(this->sameTypeComponents.size + 1);
      this->sameTypeComponents.lastObject()->size = 0;
      tempType = this->simpleComponentTypes[i];
    }
    this->sameTypeComponents.lastObject()->addOnTop(i);
    this->indexUniComponent[i] = this->sameTypeComponents.size - 1;
    this->indexInUniComponent[i] = this->sameTypeComponents.lastObject()->size - 1;
  }
}

Rational DynkinDiagramRootSubalgebra::getSquareLengthLongestRootLinkedTo(const Vector<Rational>& inputVector) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::getSquareLengthLongestRootLinkedTo");
  Rational result = 0;
  for (int i = 0; i < this->AmbientRootSystem.size; i ++) {
    if (inputVector.scalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm) != 0) {
      Rational squareLength = this->AmbientRootSystem[i].scalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm);
      if (result < squareLength) {
        result = squareLength;
      }
    }
  }
  return result;
}

Rational DynkinDiagramRootSubalgebra::getSquareLengthShortestRootLinkedTo(const Vector<Rational>& inputVector) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::getSquareLengthLongestRootLinkedTo");
  Rational result = inputVector.scalarProduct(inputVector, this->AmbientBilinearForm);
  for (int i = 0; i < this->AmbientRootSystem.size; i ++) {
    if (inputVector.scalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm) != 0) {
      Rational squareLength = this->AmbientRootSystem[i].scalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm);
      if (squareLength < result) {
        result = squareLength;
      }
    }
  }
  return result;
}

void DynkinDiagramRootSubalgebra::computeDynkinString(int indexComponent) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::computeDynkinString");
  this->checkInitialization();
  if (indexComponent >= this->SimpleBasesConnectedComponents.size) {
    global.fatal << "Bad Dynkin index. " << global.fatal;
  }
  DynkinSimpleType& outputType = this->simpleComponentTypes[indexComponent];
  Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[indexComponent];
  List<int>& currentEnds = this->indicesEnds[indexComponent];
  if (currentComponent.size < 1) {
    global.fatal << "This is a programming error: currentComponent is empty which is impossible. " << global.fatal;
  }
  if (this->numberOfThreeValencyNodes(indexComponent) == 1) {
    // type D or E
    // in type D first comes the triple node, then the long string, then the one-root strings
    // the long string is oriented with the end that is connected to the triple node having
    // smaller index
    // in type E similarly the longest string comes first oriented with the root that is
    // linked to the triple node having smaller index
    // then comes the second longest string (oriented in the same fashion)
    // and last the one-root string
    Vector<Rational> tripleNode;
    tripleNode = currentComponent[this->indicesThreeNodes[indexComponent]];
    Vectors<Rational> rootsWithoutTripleNode = currentComponent;
    rootsWithoutTripleNode.removeIndexSwapWithLast(this->indicesThreeNodes[indexComponent]);
    DynkinDiagramRootSubalgebra diagramWithoutTripleNode;
    diagramWithoutTripleNode.AmbientBilinearForm = this->AmbientBilinearForm;
    diagramWithoutTripleNode.AmbientRootSystem = this->AmbientRootSystem;
    diagramWithoutTripleNode.computeDiagramInputIsSimple(rootsWithoutTripleNode);
    if (diagramWithoutTripleNode.SimpleBasesConnectedComponents.size != 3) {
      global.fatal << "This is a programming error: Dynkin diagram has a triple "
      << "node whose removal does not yield 3 connected components. " << global.fatal;
    }
    for (int i = 0; i < 3; i ++) {
      if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[i][0].scalarProduct(tripleNode, this->AmbientBilinearForm) == 0) {
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[i].reverseElements();
      }
    }
    for (int i = 0; i < 3; i ++) {
      for (int j = i + 1; j < 3; j ++) {
        if (
          diagramWithoutTripleNode.SimpleBasesConnectedComponents[i].size <
          diagramWithoutTripleNode.SimpleBasesConnectedComponents[j].size
        ) {
          diagramWithoutTripleNode.swapDynkinStrings(i, j);
        }
      }
    }
    currentComponent.setSize(0);
    if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].size == 1) {
      //<- components are sorted by length, therefore the second and third component are of length 1,
      //therefore we have type D_n
      Rational theScale = DynkinSimpleType::getDefaultLongRootLengthSquared('D') /
      tripleNode.scalarProduct(tripleNode, this->AmbientBilinearForm);
      currentComponent.addListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0]);//<-first long component
      if (!tripleNode.scalarProduct(currentComponent[0], this->AmbientBilinearForm).isEqualToZero()) {
        currentComponent.reverseElements();
      }
      currentComponent.addOnTop(tripleNode);//<-then triple node
      currentComponent.addListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1]);//<-last two vectors
      currentComponent.addListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[2]);//<-last two vectors
      outputType.makeArbitrary('D', currentComponent.size, theScale);
    } else {
      //the second largest component has more than one element, hence we are in type E_n.
      Rational theScale = DynkinSimpleType::getDefaultLongRootLengthSquared('E') / tripleNode.scalarProduct(tripleNode, this->AmbientBilinearForm);
      if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].size != 2) {
        global.fatal << "This is a programming error: the Dynkin diagram has two components of "
        << "length larger than 2 linked to the triple node."
        << global.fatal;
      }
      if (!tripleNode.scalarProduct(
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][0],
        this->AmbientBilinearForm).isEqualToZero()
      ) {
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].reverseElements(); //<-the 2-root component has the first root perpendicular to the triple node
      }
      if (
        tripleNode.scalarProduct(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0][0],
        this->AmbientBilinearForm).isEqualToZero()
      ) {
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[0].reverseElements(); //<-the largest component has the first root non-perpendicular to the triple node
      }
      currentComponent.addOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][0]); //<-first root from 2-element component
      currentComponent.addOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[2][0]); //<-then the small sticky part of the Dynkin diagram
      currentComponent.addOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][1]); //<-next the second root from 2-element component
      currentComponent.addOnTop(tripleNode); //<- next the triple node
      currentComponent.addListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0]); //<-finally the longest component. Conventions, conventions...
      outputType.makeArbitrary('E', currentComponent.size, theScale);
    }
   return;
  }
  Rational length1, length2;
  length1 = currentComponent[0].scalarProduct(currentComponent[0], this->AmbientBilinearForm);
  int numLength1 = 1;
  int numLength2 = 0;
  for (int i = 1; i < currentComponent.size; i ++) {
    if (currentComponent[i].scalarProduct(currentComponent[i], this->AmbientBilinearForm) == length1) {
      numLength1 ++;
    } else {
      numLength2 ++;
      length2 = currentComponent[i].scalarProduct(currentComponent[i], this->AmbientBilinearForm);
    }
  }
  if (numLength2 == 0) {
    //type A
    outputType.makeArbitrary('A', numLength1, DynkinSimpleType::getDefaultLongRootLengthSquared('A') / length1);
  } else {
    if (length1 < length2) {
      MathRoutines::swap(length1, length2);
      MathRoutines::swap(numLength1, numLength2);
      currentEnds.swapTwoIndices(0, 1);
    } // <-so far we made sure the first length is long
    // By convention, in types G and C, in the Dynkin diagram the long root comes last
    // This is handled at the very end of this function (outside all the if clauses).
    if (numLength1 == numLength2) {
      //B2, C2, F4 or G2
      if (numLength1 == 2) {
        outputType.makeArbitrary('F', 4, DynkinSimpleType::getDefaultLongRootLengthSquared('F') / length1);
      } else if (length1 / length2 == 3) {
        outputType.makeArbitrary('G', 2, DynkinSimpleType::getDefaultLongRootLengthSquared('G') / length1);
      } else {
        outputType.makeArbitrary('B', 2, DynkinSimpleType::getDefaultLongRootLengthSquared('B') / length1);
      }
    } else {
      if (numLength1>numLength2) {
        outputType.makeArbitrary(
          'B', currentComponent.size, DynkinSimpleType::getDefaultLongRootLengthSquared('B') / length1
        );
      } else {
        outputType.makeArbitrary(
          'C', currentComponent.size, DynkinSimpleType::getDefaultLongRootLengthSquared('C') / length1
        );
      }
    }
  }
  // The following code ensures the Dynkin diagram is properly ordered
  currentComponent.swapTwoIndices(0, currentEnds[0]);
  for (int i = 0; i < currentComponent.size; i ++) {
    for (int j = i + 1; j < currentComponent.size; j ++) {
      if (!currentComponent[i].scalarProduct(currentComponent[j], this->AmbientBilinearForm).isEqualToZero()) {
        currentComponent.swapTwoIndices(i + 1, j);
        break;
      }
    }
  }
  // so far we made sure the entire component is one properly ordered string, starting with the long root.
  if (outputType.theLetter == 'G' || outputType.theLetter == 'C' ) {
    currentComponent.reverseElements(); // <-in G_2 and C_n the short root comes first so we need to reverse elements.
  }
}

std::string DynkinDiagramRootSubalgebra::toString(FormatExpressions* theFormat) const {
  DynkinType theType;
  theType.makeZero();
  for (int j = 0; j < this->simpleComponentTypes.size; j ++) {
    theType.addMonomial(this->simpleComponentTypes[j], 1);
  }
  return theType.toString(theFormat);
}

bool DynkinDiagramRootSubalgebra::checkInitialization() const {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::checkInitialization");
  if (this->AmbientRootSystem.size != 0) {
    if (this->AmbientBilinearForm.numberOfRows != this->AmbientRootSystem[0].size) {
      global.fatal << "Ambient bilinear form of Dynkin subdiagram not initialized. " << global.fatal;
    }
  }
  return true;
}

void DynkinDiagramRootSubalgebra::computeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::computeDiagramInputIsSimple");
  this->checkInitialization();
  this->SimpleBasesConnectedComponents.size = 0;
  this->SimpleBasesConnectedComponents.reserve(simpleBasisInput.size);
  for (int i = 0; i < simpleBasisInput.size; i ++) {
    int indexFirstComponentConnectedToRoot = - 1;
    for (int j = 0; j < this->SimpleBasesConnectedComponents.size; j ++) {
      if (this->SimpleBasesConnectedComponents[j].containsVectorNonPerpendicularTo(
        simpleBasisInput[i], this->AmbientBilinearForm)
      ) {
        if (indexFirstComponentConnectedToRoot == - 1) {
          indexFirstComponentConnectedToRoot = j;
          this->SimpleBasesConnectedComponents[j].addOnTop(simpleBasisInput[i]);
        } else {
          this->SimpleBasesConnectedComponents[indexFirstComponentConnectedToRoot].addListOnTop(
            this->SimpleBasesConnectedComponents[j]
          );
          this->SimpleBasesConnectedComponents.removeIndexSwapWithLast(j);
          j --;
        }
      }
    }
    if (indexFirstComponentConnectedToRoot == - 1) {
      this->SimpleBasesConnectedComponents.setSize(this->SimpleBasesConnectedComponents.size + 1);
      this->SimpleBasesConnectedComponents.lastObject()->size = 0;
      this->SimpleBasesConnectedComponents.lastObject()->addOnTop(simpleBasisInput[i]);
    }
  }
  this->computeDynkinStrings();
  this->sort();
  this->computeDynkinStrings();
  DynkinType tempType;
  this->getDynkinType(tempType);
  if (tempType.isEqualToZero() && simpleBasisInput.size != 0) {
    global.fatal
    << "Dynkin type of zero but the roots generating the type are: "
    << simpleBasisInput.toString() << global.fatal;
  }
}

bool DynkinDiagramRootSubalgebra::letterIsDynkinGreaterThanLetter(char letter1, char letter2) {
  if ((letter1 == 'B' || letter1 == 'D') && (letter2 == 'B' || letter2 == 'D') ) {
    if (letter1 == letter2) {
      return false;
    }
    if (letter1 == 'B') {
      return true;
    } else {
      return false;
    }
  }
  return letter1 > letter2;
}

bool DynkinDiagramRootSubalgebra::isGreaterThan(DynkinDiagramRootSubalgebra& right) {
  if (this->rankTotal() > right.rankTotal()) {
    return true;
  }
  if (this->rankTotal() < right.rankTotal()) {
    return false;
  }
  if (this->simpleComponentTypes.size != this->SimpleBasesConnectedComponents.size) {
    global.fatal
    << "Simple component types do "
    << "not match number of connected components. " << global.fatal;
  }
  for (int i = 0; i < this->simpleComponentTypes.size; i ++) {
    if (this->SimpleBasesConnectedComponents[i].size > right.SimpleBasesConnectedComponents[i].size) {
      return true;
    }
    if (right.SimpleBasesConnectedComponents[i].size > this->SimpleBasesConnectedComponents[i].size) {
      return false;
    }
    if (this->simpleComponentTypes[i] > right.simpleComponentTypes[i]) {
      return true;
    }
    if (right.simpleComponentTypes[i] > this->simpleComponentTypes[i]) {
      return false;
    }
  }
  return false;
}

Rational DynkinDiagramRootSubalgebra::getSizeCorrespondingWeylGroupByFormula() {
  Rational output = 1;
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    output *= WeylGroupData::sizeByFormulaOrNegative1(
      this->simpleComponentTypes[i].theLetter, this->simpleComponentTypes[i].theRank
    );
  }
  return output;
}

void DynkinDiagramRootSubalgebra::getMapFromPermutation(
  Vectors<Rational>& domain,
  Vectors<Rational>& range,
  List<int>& thePerm, List<List<List<int > > >& theAutos,
  SelectionWithDifferentMaxMultiplicities& theAutosPerm,
  DynkinDiagramRootSubalgebra& right
) {
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    for (int j = 0; j < this->SimpleBasesConnectedComponents[i].size; j ++) {
      if (this->SimpleBasesConnectedComponents[i].size != right.SimpleBasesConnectedComponents[thePerm[i]].size) {
        global.fatal << "Connected components simple bases sizes do not match. " << global.fatal;
      }
      domain.addOnTop( this->SimpleBasesConnectedComponents[i][j]);
      int indexTargetComponent = thePerm[i];
      int indexAutomorphismInComponent = theAutosPerm.multiplicities[i];
      int indexRoot = theAutos[i][indexAutomorphismInComponent][j];
      range.addOnTop(right.SimpleBasesConnectedComponents[indexTargetComponent][indexRoot]);
    }
  }
}

void DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInput(Vectors<Rational>& inputRoots, WeylGroupData& theWeyl) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInput");
  this->AmbientRootSystem = theWeyl.rootSystem;
  this->AmbientBilinearForm = theWeyl.cartanSymmetric;
  theWeyl.transformToSimpleBasisGenerators(inputRoots, theWeyl.rootSystem);
  this->computeDiagramInputIsSimple(inputRoots);
}

void DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInputRelative(
  Vectors<Rational>& inputOutputSimpleWeightSystem,
  const HashedList<Vector<Rational> >& weightSystem,
  const Matrix<Rational>& theBilinearForm
) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInputRelative");
  this->AmbientRootSystem = weightSystem;
  this->AmbientBilinearForm = theBilinearForm;
  WeylGroupData::transformToSimpleBasisGeneratorsArbitraryCoordinates(inputOutputSimpleWeightSystem, weightSystem);
  this->computeDiagramInputIsSimple(inputOutputSimpleWeightSystem);
}

void DynkinDiagramRootSubalgebra::computeDynkinStrings() {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::computeDynkinStrings");
  this->indicesThreeNodes.setSize(this->SimpleBasesConnectedComponents.size);
  this->simpleComponentTypes.setSize(this->SimpleBasesConnectedComponents.size);
  this->indicesEnds.setSize(this->SimpleBasesConnectedComponents.size);
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    this->computeDynkinString(i);
  }
}

bool DynkinDiagramRootSubalgebra::operator==(const DynkinDiagramRootSubalgebra& right) const {
  if (right.SimpleBasesConnectedComponents.size != this->SimpleBasesConnectedComponents.size) {
    return false;
  }
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    bool tempBool =
      ((this->SimpleBasesConnectedComponents[i].size == right.SimpleBasesConnectedComponents[i].size) &&
      (this->simpleComponentTypes[i] == right.simpleComponentTypes[i]));
    if (!tempBool) {
      return false;
    }
  }
  return true;
}

void DynkinDiagramRootSubalgebra::getDynkinType(DynkinType& output) const {
  output.makeZero();
  output.setExpectedSize(this->simpleComponentTypes.size);
  for (int i = 0; i < this->simpleComponentTypes.size; i ++) {
    output.addMonomial(this->simpleComponentTypes[i], 1);
  }
}

void DynkinDiagramRootSubalgebra::getAutomorphism(List<List<int> >& output, int index) {
  Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[index];
  DynkinSimpleType& currentStrinG = this->simpleComponentTypes[index];
  List<int> thePermutation;
  thePermutation.setSize(currentComponent.size);
  output.size = 0;
  for (int i = 0; i < currentComponent.size; i ++) {
    thePermutation[i] = i;
  }
  output.addOnTop(thePermutation);
  if (currentStrinG.theLetter == 'A' && currentComponent.size != 1) {
    thePermutation.reverseElements();
    output.addOnTop(thePermutation);
  }
  if (currentStrinG.theLetter == 'D') {
    if (currentComponent.size == 4) {
     //the automorphism group of the Dynkin Diagram is S3
      thePermutation[1] = 2;
      thePermutation[2] = 3;
      thePermutation[3] = 1;
      output.addOnTop(thePermutation);
      thePermutation[1] = 1;
      thePermutation[2] = 3;
      thePermutation[3] = 2;
      output.addOnTop(thePermutation);
      thePermutation[1] = 2;
      thePermutation[2] = 1;
      thePermutation[3] = 3;
      output.addOnTop(thePermutation);
      thePermutation[1] = 3;
      thePermutation[2] = 1;
      thePermutation[3] = 2;
      output.addOnTop(thePermutation);
      thePermutation[1] = 3;
      thePermutation[2] = 2;
      thePermutation[3] = 1;
      output.addOnTop(thePermutation);
    } else {
      thePermutation[currentComponent.size - 2] = currentComponent.size - 1;
      thePermutation[currentComponent.size - 1] = currentComponent.size - 2;
      output.addOnTop(thePermutation);
    }
  }
  if (currentStrinG.theLetter == 'E' && currentStrinG.theRank == 6) {
    thePermutation[1] = 3;
    thePermutation[2] = 4;
    thePermutation[3] = 1;
    thePermutation[4] = 2;
    output.addOnTop(thePermutation);
  }
}

void DynkinDiagramRootSubalgebra::getAutomorphisms(List<List<List<int> > >& output) {
  output.setSize(this->SimpleBasesConnectedComponents.size);
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    this->getAutomorphism(output[i], i);
  }
}

int DynkinDiagramRootSubalgebra::rankTotal() {
  int result = 0;
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    result += this->SimpleBasesConnectedComponents[i].size;
  }
  return result;
}

int DynkinDiagramRootSubalgebra::numberRootsGeneratedByDiagram() {
  int result = 0;
  if (this->SimpleBasesConnectedComponents.size != this->simpleComponentTypes.size) {
    global.fatal << "Number of simple connected components does not match the number of types. " << global.fatal;
  }
  for (int i = 0; i < this->simpleComponentTypes.size; i ++) {
    int Rank = this->SimpleBasesConnectedComponents[i].size;
    if (this->simpleComponentTypes[i].theLetter == 'A') {
      result += Rank * (Rank + 1);
    }
    if (this->simpleComponentTypes[i].theLetter == 'B' || this->simpleComponentTypes[i].theLetter == 'C') {
      result += Rank * Rank * 2;
    }
    if (this->simpleComponentTypes[i].theLetter == 'D') {
      result += Rank * (Rank - 1) * 2;
    }
    if (this->simpleComponentTypes[i].theLetter == 'E') {
      if (Rank == 6) {
        result += 72;
      }
      if (Rank == 7) {
        result += 126;
      }
      if (Rank == 8) {
        result += 240;
      }
    }
    if (this->simpleComponentTypes[i].theLetter == 'F') {
      result += 48;
    }
    if (this->simpleComponentTypes[i].theLetter == 'G') {
      result += 12;
    }
  }
  return result;
}

int DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes(int indexComponent) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes");
  Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[indexComponent];
  int numEnds = 0;
  int result = 0;
  this->indicesThreeNodes[indexComponent] = - 1;
  this->indicesEnds[indexComponent].size = 0;
  for (int i = 0; i < currentComponent.size; i ++) {
    int counter = 0;
    for (int j = 0; j < currentComponent.size; j ++) {
      if (currentComponent[i].scalarProduct(currentComponent[j], this->AmbientBilinearForm).isNegative()) {
        counter ++;
      }
    }
    if (counter > 3) {
      Matrix<Rational> theGram;
      currentComponent.getGramMatrix(theGram, &this->AmbientBilinearForm);
      global.fatal << "This is a programming error: corrupt simple basis corresponding to "
      << "Dynkin diagram: the Dynkin diagram should have nodes with"
      << " valency at most 3, but this diagram has node with valency "
      << counter << ". The current component is: "
      << currentComponent.toString()
      << ". The corresponding Symmetric Cartan is: "
      << theGram.toString() << ". " << global.fatal;
    }
    if (counter == 3) {
      result ++;
      this->indicesThreeNodes[indexComponent] = i;
    }
    if (counter <= 1) {
      numEnds ++;
      this->indicesEnds[indexComponent].addOnTop(i);
    }
  }
  if (result > 1) {
    global.fatal << "numEnds variable equals: " << numEnds
    << ", number of three-nodes equals: "
    << result << "; this should not happen. The bilinear form is: "
    << this->AmbientBilinearForm.toString() << global.fatal;
  }
  if (result == 1) {
    if (numEnds != 3) {
      global.fatal << "numEnds variable equals: " << numEnds
      << ", number of three-nodes equals: "
      << result << "; this should not happen. The bilinear form is: "
      << this->AmbientBilinearForm.toString() << global.fatal;
    }
  } else {
    if (numEnds > 2) {
      global.fatal << "numEnds variable equals: " << numEnds << ", number of three-nodes equals: "
      << result << "; this should not happen. The bilinear form is: " << this->AmbientBilinearForm.toString() << global.fatal;
    }
  }
  return result;
}

bool AffineCone::splitByAffineHyperplane(AffineHyperplane<Rational>& theKillerPlane, AffineCones& output) {
  (void) theKillerPlane;
  (void) output;
  return true;
}

bool AffineCone::wallIsInternalInCone(AffineHyperplane<Rational>& theKillerCandidate) {
  (void) theKillerCandidate;
  return true;
}

int AffineCone::getDimension() {
  if (this->theWalls.size == 0) {
    return 0;
  }
  return this->theWalls.objects[0].affinePoint.size;
}

unsigned int AffineCone::hashFunction() const {
  int tempMin = MathRoutines::minimum(this->theWalls.size, ::someRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < tempMin; i ++) {
    result += this->theWalls[i].hashFunction() * ::someRandomPrimes[i];
  }
  return result;
}

void AffineHyperplanes::toString(std::string& output) {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    std::string tempS;
    this->objects[i].toString(tempS);
    out << "index: " << i << " " << tempS << "\n";
  }
  output = out.str();
}

void Permutation::initPermutation(int n) {
  this->initPart1(n);
  for (int i = 0; i < n; i ++) {
    this->capacities[i] = n - i - 1;
    this->multiplicities[i] = 0;
  }
}

void Permutation::initPermutation(List<int>& disjointSubsets, int TotalNumElements) {
  this->initPart1(TotalNumElements);
  int counter = 0;
  for (int i = 0; i < disjointSubsets.size; i ++) {
    for (int j = 0; j < disjointSubsets[i]; j ++) {
      this->capacities[counter] = disjointSubsets[i] - j - 1;
      this->multiplicities[counter] = 0;
      counter ++;
    }
    TotalNumElements -= disjointSubsets[i];
  }
  if (TotalNumElements != 0) {
    global.fatal << "Permutations with 0 elements not allowed. " << global.fatal;
  }
}

void Permutation::incrementAndGetPermutation(List<int>& output) {
  this->incrementReturnFalseIfPastLast();
  this->getPermutationLthElementIsTheImageofLthIndex(output);
}

void Permutation::getPermutationLthElementIsTheImageofLthIndex(List<int>& output) {
  int numElements = this->multiplicities.size;
  output.setSize(numElements);
  for (int i = 0; i < numElements; i ++) {
    output[i] = i;
  }
  for (int i = 0; i < numElements; i ++) {
    MathRoutines::swap(output[i], output[i + this->multiplicities[i]]);
  }
}

bool WeylGroupData::areMaximallyDominantGroupInner(List<Vector<Rational> >& theWeights) {
  MacroRegisterFunctionWithName("WeylGroup::AreMaximallyDominantInner");
  for (int i = 0; i < theWeights.size; i ++) {
    for (int j = 0; j < this->rootsOfBorel.size; j ++) {
      if (this->rootScalarCartanRoot(this->rootsOfBorel[j], theWeights[i]) < 0) {
        bool reflectionDoesRaise = true;
        for (int k = 0; k < i; k ++) {
          if (this->rootScalarCartanRoot(this->rootsOfBorel[j], theWeights[k]) > 0) {
            reflectionDoesRaise = false;
            break;
          }
        }
        if (reflectionDoesRaise) {
          return false;
        }
      }
    }
  }
  return true;
}

bool WeylGroupAutomorphisms::checkInitialization() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of Weyl group automorphism. " << global.fatal;
  }
  if (this->theWeyl == nullptr) {
    global.fatal << "Non-initialized Weyl group automorphisms. " << global.fatal;
  }
  return true;
}

bool WeylGroupAutomorphisms::areMaximallyDominantGroupOuter(List<Vector<Rational> >& theWeights) {
  MacroRegisterFunctionWithName("WeylGroup::areMaximallyDominantGroupOuter");
  this->checkInitialization();
  MemorySaving<Vectors<Rational> > theWeightsCopy;
  Vector<Rational> zeroWeight;
  this->computeOuterAutomorphisms();
  zeroWeight.makeZero(this->theWeyl->getDimension());
  for (int i = 0; i < theWeights.size; i ++) {
    for (int j = 0; j < this->theWeyl->rootsOfBorel.size; j ++) {
      if (this->theWeyl->rootScalarCartanRoot(this->theWeyl->rootsOfBorel[j], theWeights[i]) < 0) {
        bool reflectionDoesRaise = true;
        for (int k = 0; k < i; k ++) {
          if (this->theWeyl->rootScalarCartanRoot(this->theWeyl->rootsOfBorel[j], theWeights[k]) > 0) {
            reflectionDoesRaise = false;
            break;
          }
        }
        if (reflectionDoesRaise) {
          return false;
        }
      }
    }
    for (int j = 0; j < this->theOuterAutos.theElements.size; j ++) {
      theWeightsCopy.getElement() = theWeights;
      this->theOuterAutos.theElements[j].actOnVectorsColumn(theWeightsCopy.getElement());
      bool isGood = true;
      for (int k = 0; k < i; k ++) {
        if (!(theWeightsCopy.getElement()[k] - theWeights[k]).isPositiveOrZero()) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      if (!(theWeightsCopy.getElement()[i] - theWeights[i]).IsGreaterThanLexicographic(zeroWeight)) {
        continue;
      }
      return false;
    }
  }
  return true;
}
void WeylGroupData::generateRootSubsystem(Vectors<Rational>& theRoots) {
  Vector<Rational> tempRoot;
  int oldsize = theRoots.size;
  for (int i = 0; i < oldsize; i ++) {
    theRoots.addOnTopNoRepetition(- theRoots[i]);
  }
  for (int i = 0; i < theRoots.size; i ++) {
    for (int j = 0; j < theRoots.size; j ++) {
      tempRoot = theRoots[i] + theRoots[j];
      if (this->isARoot(tempRoot)) {
        theRoots.addOnTopNoRepetition(tempRoot);
      }
    }
  }
}

void GeneralizedVermaModuleCharacters::computeQPsFromChamberComplex() {
  std::stringstream out;
  FormatExpressions theFormat;
  Vector<Rational> tempRoot;
  FileOperations::openFileCreateIfNotPresentVirtual(
    this->theMultiplicitiesMaxOutputReport2, "output/ExtremaPolys.txt", false, true, false
  );
  this->thePfs.initFromRoots(this->GmodKNegWeightsBasisChanged);
  out << this->thePfs.toString(theFormat);
  this->thePfs.split(nullptr);
  out << "=" << this->thePfs.toString(theFormat);
//  int totalDim= this->theTranslationS[0].size +this->theTranslationsProjecteD[0].size;
  this->theQPsSubstituted.setSize(this->projectivizedChambeR.size);
  global.fatal << "not implemented fully, crashing to let you know. " << global.fatal;
//  this->thePfs.theChambersOld.initialize();
//  this->thePfs.theChambersOld.theDirections = this->GmodKNegWeightsBasisChanged;
//  this->thePfs.theChambersOld.SliceTheEuclideanSpace(false);
//  this->theQPsNonSubstituted.setSize(this->thePfs.theChambersOld.size);
//  this->theQPsSubstituted.setSize(this->thePfs.theChambersOld.size);
  out << "\n\nThe vector partition functions in each chamber follow.";
  global.fatal << "Not implemented yet. " << global.fatal;
/*
  for (int i = 0; i < this->thePfs.theChambersOld.size; i ++)
    if (this->thePfs.theChambersOld.objects[i] != 0) {
      QuasiPolynomial& currentQPNoSub = this->theQPsNonSubstituted.objects[i];
      this->theQPsSubstituted.objects[i].setSize(this->theLinearOperators.size);
      this->thePfs.getVectorPartitionFunction(currentQPNoSub, this->thePfs.theChambersOld.objects[i]->InternalPoint);
      out << "\nChamber " << i + 1 << " with internal point " << this->thePfs.theChambersOld.objects[i]->InternalPoint.toString() << " the quasipoly is: " << currentQPNoSub.toString(false, false);
      for (int k = 0; k< this->theLinearOperators.size; k++) {
        QuasiPolynomial& currentQPSub = this->theQPsSubstituted.objects[i].objects[k];
        std::stringstream tempStream;
        tempStream << "Processing chamber " << i + 1 << " linear operator " << k+ 1;
        global.theIndicatorVariables.ProgressReportStrings[0] = tempStream.str();
        global.makeReport();
        currentQPNoSub.substitution(this->theLinearOperatorsExtended.objects[k], this->theTranslationsProjectedBasisChanged[k], this->theExtendedIntegralLatticeMatForM, currentQPSub);
        out << "; after substitution we get: " << currentQPSub.toString(false, false);
      }
    }
    */
//  this->theParser.theHmm.theRange.theWeyl.getIntegralLatticeInSimpleCoordinates(integralLattice);
  //out << "\nThe integral lattice:\n" << integralLattice.toString(false, false);
  //this->theMultiplicitiesMaxOutputReport2.flush();
  QuasiPolynomial tempQP;
  this->theMultiplicities.setSize(this->projectivizedChambeR.size);
  this->numNonZeroMults = 0;
  ProgressReport theReport;
  ProgressReport theReport2;
  for (int i = 0; i < this->projectivizedChambeR.size; i ++) {
    QuasiPolynomial& currentSum = this->theMultiplicities.objects[i];
    currentSum.makeZeroOverLattice(this->theExtendedIntegralLatticeMatForM);
    for (int k = 0; k < this->theLinearOperators.size; k ++) {
      this->getProjection(k, this->projectivizedChambeR.objects[i].getInternalPoint(), tempRoot);
      tempRoot -= this->NonIntegralOriginModificationBasisChanged;
      global.fatal << global.fatal ;
      int theIndex = - 1;//= this->thePfs.theChambersOld.GetFirstChamberIndexContainingPoint(tempRoot);
      if (theIndex != - 1) {
        tempQP = this->theQPsSubstituted[theIndex][k];
        tempQP *= this->theCoeffs[k];
        currentSum += tempQP;
      }
      std::stringstream tempStream;
      tempStream << " Chamber " << i + 1 << " translation " << k + 1;
      theReport.report(tempStream.str());
    }
    if (!currentSum.isEqualToZero()) {
      this->numNonZeroMults ++;
    }
    std::stringstream tempStream;
    tempStream << " So far " << i + 1 << " out of " << this->projectivizedChambeR.size << " processed " << this->numNonZeroMults
    << " non-zero total.";
    theReport2.report(tempStream.str());
    out << "\nChamber " << i + 1 << ": the quasipolynomial is: " << currentSum.toString(false, false);
    out << "\nThe chamber is: " << this->projectivizedChambeR[i].toString(&theFormat);
  }
//  this->projectivizedChamber.ComputeDebugString();
//  out << "\n\n" << this->projectivizedChamber.DebugString;
  theReport.report(out.str());
  this->theMultiplicitiesMaxOutputReport2 << out.str();
}

std::string GeneralizedVermaModuleCharacters::computeMultiplicitiesLargerAlgebraHighestWeight(
  Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel
) {
  std::stringstream out;
  WeylGroupData& LargerWeyl = this->theHmm.theRange().theWeyl;
  WeylGroupData& SmallerWeyl = this->theHmm.theDomain().theWeyl;
  if (!LargerWeyl.isOfSimpleType('B', 3)) {
    return "Error: algebra is not so(7).";
  }
  this->initFromHomomorphism(parabolicSel, this->theHmm);
  this->transformToWeylProjectiveStep1();
  this->transformToWeylProjectiveStep2();
  Vector<Rational> highestWeightLargerAlgSimpleCoords;
  highestWeightLargerAlgSimpleCoords = LargerWeyl.getSimpleCoordinatesFromFundamental(highestWeightLargerAlgebraFundamentalCoords);
  Matrix<Rational> tempMat;
  Vector<Rational> tempRoot;
  DrawingVariables drawOps;
  int theSmallDim = SmallerWeyl.cartanSymmetric.numberOfRows;
//  drawOps.theBuffer.initDimensions(theSmallDim, 1);
  Vectors<double> theDraggableBasis;
  theDraggableBasis.makeEiBasis(theSmallDim);
  WeylGroupData tmpWeyl;
  tmpWeyl.makeArbitrarySimple('A', 2);
  drawOps.theBuffer.initDimensions(tmpWeyl.cartanSymmetric, theDraggableBasis, theDraggableBasis);
  FormatExpressions theFormat;
  drawOps.theBuffer.BasisProjectionPlane[0][0] = 1;
  drawOps.theBuffer.BasisProjectionPlane[0][1] = 0;
  drawOps.theBuffer.BasisProjectionPlane[1][0] = 1;
  drawOps.theBuffer.BasisProjectionPlane[1][1] = 1;
  drawOps.theBuffer.modifyToOrthonormalNoShiftSecond
  (drawOps.theBuffer.BasisProjectionPlane[1], drawOps.theBuffer.BasisProjectionPlane[0]);
  drawOps.theBuffer.graphicsUnit = 50;
  PiecewiseQuasipolynomial theStartingPoly, theSubbedPoly, Accum;
  std::string tempS;
  theStartingPoly.makeVPF(this->GmodKNegWeightsBasisChanged, tempS);
  Vectors<Rational> translationsProjectedFinal;
  translationsProjectedFinal.setSize(this->theLinearOperators.size);
  this->theLinearOperators[0].actOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[0]);
  out << "<br>Input so(7)-highest weight: " << highestWeightLargerAlgSimpleCoords.toString();
  out << "<br>Input parabolics selections: " << parabolicSel.toString();
  out << "<br>the argument translations: " << this->theTranslationsProjectedBasisChanged.toString();
  out << "<br>Element u_w: projection, multiplication by - 1, and basis change of so(7)-highest weight to G_2: "
  << translationsProjectedFinal[0].toString();
  theStartingPoly.makeVPF(this->GmodKNegWeightsBasisChanged, tempS);
  drawOps.drawCoordSystemBuffer(drawOps, 2);
  Cone smallWeylChamber;
  tempMat = SmallerWeyl.cartanSymmetric;
  tempMat.invert();
  Vectors<Rational> tempVertices;
  Vector<Rational> tMpRt;
  tMpRt = this->ParabolicSelectionSmallerAlgebra;
  for (int i = 0; i < this->ParabolicSelectionSmallerAlgebra.numberOfElements; i ++) {
    tempMat.getVectorFromRow(i, tempRoot);
    tempVertices.addOnTop(tempRoot);
    if (this->ParabolicSelectionSmallerAlgebra.selected[i]) {
      tempVertices.addOnTop(- tempRoot);
    }
  }
  smallWeylChamber.createFromVertices(tempVertices);
  tempMat.initialize(2, 2);
  tempMat.elements[0][0] = 1;
  tempMat.elements[0][1] = 0;
  tempMat.elements[1][0] = 1;
  tempMat.elements[1][1] = 1;
  tempMat.transpose();
  smallWeylChamber.changeBasis(tempMat);
  out << "<br> The small Weyl chamber: " << smallWeylChamber.toString(&theFormat);
  Vector<Rational> highestWeightSmallAlgBasisChanged = - translationsProjectedFinal[0];
  for (int i = 0; i < this->theLinearOperators.size; i ++) {
    this->theLinearOperators[i].actOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[i]);
    translationsProjectedFinal[i] += this->theTranslationsProjectedBasisChanged[i];
    drawOps.drawCircleAtVectorBufferRational(- translationsProjectedFinal[i], "red", 3);
  }
  out << "<br>the translations projected final: " << translationsProjectedFinal.toString();
  Accum.makeZero(theStartingPoly.NumVariables);
  for (int i = 0; i < this->theLinearOperators.size; i ++) {
    theSubbedPoly = theStartingPoly;
    theSubbedPoly *= this->theCoeffs[i];
    theSubbedPoly.translateArgument(translationsProjectedFinal[i]);
    Accum += theSubbedPoly;
  }
  Accum.drawMe(drawOps, 10, &smallWeylChamber, &highestWeightSmallAlgBasisChanged);
  out << drawOps.getHTMLDiv(2);
  out << Accum.toString(false, true);
  return out.str();
}

void GeneralizedVermaModuleCharacters::sortMultiplicities() {
  List<Cone> tempList;
  tempList = this->projectivizedChambeR;
  tempList.quickSortAscending();
  List<QuasiPolynomial> tempQPlist;
  tempQPlist.setSize(this->theMultiplicities.size);
  for (int i = 0; i < this->theMultiplicities.size; i ++) {
    tempQPlist[i] = this->theMultiplicities[this->projectivizedChambeR.getIndex(tempList[i])];
  }
  this->theMultiplicities = tempQPlist;
  this->projectivizedChambeR.clear();
  for (int i = 0; i < tempList.size; i ++) {
    this->projectivizedChambeR.addOnTop(tempList[i]);
  }
}

std::string GeneralizedVermaModuleCharacters::checkMultiplicitiesVsOrbits() {
  MacroRegisterFunctionWithName("GeneralizedVermaModuleCharacters::checkMultiplicitiesVsOrbits");
  this->checkInitialization();
  std::stringstream out;
  int totalDimAffine = this->weylLarger->getDimension() + this->weylSmaller->getDimension();
  int smallDim = this->weylSmaller->getDimension();
  Vector<Rational> normal;
  normal.makeZero(totalDimAffine + 1);
  Vectors<Rational> newWalls;
  ConeComplex tempComplex;
  tempComplex = this->projectivizedChambeR;
  for (int i = 0; i < this->WeylChamberSmallerAlgebra.normals.size; i ++) {
    for (int j = 0; j < smallDim; j ++) {
      normal[j] = this->WeylChamberSmallerAlgebra.normals[i][j];
    }
    newWalls.addOnTop(normal);
    tempComplex.splittingNormals.addOnTop(normal);
  }
  tempComplex.indexLowestNonRefinedChamber = 0;
  tempComplex.refine();
  out << "Number chambers with new walls: " << tempComplex.size;
  out << "\n" << tempComplex.toString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::incrementComputation(Vector<Rational>& parabolicSel) {
  std::stringstream out;
  this->thePauseControlleR.initComputation();
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected = parabolicSel;
  switch (this->computationPhase) {
    case 0:
//      this->theParser.theHmm.MakeG2InB3(this->theParser);
      this->initFromHomomorphism(parabolicSel, this->theHmm);
      this->transformToWeylProjectiveStep1();
//      out << global.theIndicatorVariables.StatusString1;
      this->transformToWeylProjectiveStep2();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 1:
      this->projectivizedChambeR.refine();
      this->sortMultiplicities();
      out << this->projectivizedChambeR.toString(false);
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 2:
      this->computeQPsFromChamberComplex();
      out << this->elementToStringMultiplicitiesReport();
      break;
    case 3:
//      out << this->checkMultiplicitiesVsOrbits();
      break;
    case 4:
      this->initTheMaxComputation();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 5:
      this->theMaxComputation.findExtremaParametricStep1(this->thePauseControlleR);
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 6:
      this->theMaxComputation.findExtremaParametricStep3();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 7:
      this->theMaxComputation.findExtremaParametricStep4();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 8:
      this->theMaxComputation.findExtremaParametricStep5();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    default:
      break;
  }
  this->computationPhase ++;
  this->thePauseControlleR.exitComputation();
}

GeneralizedVermaModuleCharacters::GeneralizedVermaModuleCharacters() {
  this->UpperLimitChambersForDebugPurposes = - 1;
  this->computationPhase = 0;
  this->NumProcessedConesParam = 0;
  this->NumProcessedExtremaEqualOne = 0;
  this->numNonZeroMults = 0;
  this->weylLarger = nullptr;
  this->weylSmaller = nullptr;
}

bool GeneralizedVermaModuleCharacters::checkInitialization() const {
  if (this->weylLarger == nullptr || this->weylSmaller == nullptr) {
    global.fatal << "Use of non-initialized Weyl group within generalized Verma module characters. " << global.fatal;
  }
  if (this->weylLarger->flagDeallocated || this->weylSmaller->flagDeallocated) {
    global.fatal << "Use after free of Weyl group within Verma module characters. " << global.fatal;
  }
  return true;
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism(
  Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input
) {
  MacroRegisterFunctionWithName("GeneralizedVermaModuleCharacters::initFromHomomorphism");
  Vectors<Rational> tempRoots;
  this->weylLarger = &input.theRange().theWeyl;
  this->weylSmaller = &input.theDomain().theWeyl;
  WeylGroupData& theWeYl = input.theRange().theWeyl;
//  input.projectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots);
  this->log << "projections: " << tempRoots.toString();
  theWeYl.theGroup.computeAllElements(false);
  this->NonIntegralOriginModificationBasisChanged ="(1/2,1/2)";
  Matrix<Rational> theProjectionBasisChanged;
  Vector<Rational> startingWeight, projectedWeight;
  FormatExpressions theFormat;
  global.fatal << "Not implemented. " << global.fatal;
//  SSalgebraModuleOld tempM;
  input.computeHomomorphismFromImagesSimpleChevalleyGenerators();
  global.fatal << "Not implemented. " << global.fatal;
//  tempM.InduceFromEmbedding(tempStream, input);
  input.getWeightsGmodKInSimpleCoordinatesK(this->GmodKnegativeWeightS);
//  this->log << "weights of g mod k: " << this->GmodKnegativeWeights.toString();
  Matrix<Rational> tempMat;
  tempMat = input.theDomain().theWeyl.cartanSymmetric;
  tempMat.invert();
//  tempMat.actOnVectorsColumn(this->GmodKnegativeWeightS);
  this->log << this->GmodKnegativeWeightS.toString();
  this->preferredBasiS.setSize(2);
  this->preferredBasiS[0] = - this->GmodKnegativeWeightS[1];
  this->preferredBasiS[1] = - this->GmodKnegativeWeightS[2];
  /////////////////////////////////////////
  //this->preferredBasiS[0] ="(1,0)";
  //this->preferredBasiS[1] ="(0,1)";
  ////////////////////////////////////////
  this->preferredBasisChangE.assignVectorsToRows(this->preferredBasiS);
  this->preferredBasisChangE.transpose();
  this->preferredBasisChangeInversE = this->preferredBasisChangE;
  this->preferredBasisChangeInversE.invert();
  this->preferredBasisChangeInversE.actOnVectorsColumn
  (this->GmodKnegativeWeightS, this->GmodKNegWeightsBasisChanged);
  this->log << "\nWeights after basis change: " << this->GmodKNegWeightsBasisChanged.toString();
  for (int i = 0; i < this->GmodKnegativeWeightS.size; i ++) {
    if (this->GmodKnegativeWeightS[i].isPositiveOrZero()) {
      this->GmodKnegativeWeightS.removeIndexSwapWithLast(i);
      i --;
    }
  }
  for (int i = 0; i < this->GmodKNegWeightsBasisChanged.size; i ++) {
    if (this->GmodKNegWeightsBasisChanged[i].isPositiveOrZero()) {
      this->GmodKNegWeightsBasisChanged.removeIndexSwapWithLast(i);
      i --;
    }
  }
  this->log << "\nNegative weights after basis change: " << this->GmodKNegWeightsBasisChanged.toString();
  theProjectionBasisChanged.initialize(input.theDomain().getRank(), input.theRange().getRank());
  for (int i = 0; i < input.theRange().getRank(); i ++) {
    startingWeight.makeEi(input.theRange().getRank(), i);
    input.projectOntoSmallCartan(startingWeight, projectedWeight);
    this->preferredBasisChangeInversE.actOnVectorColumn(projectedWeight);
    for (int j = 0; j < projectedWeight.size; j ++) {
      theProjectionBasisChanged.elements[j][i] = projectedWeight[j];
    }
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected = theParabolicSel;
  Matrix<Rational> DualCartanEmbedding;
  input.getMapSmallCartanDualToLargeCartanDual(DualCartanEmbedding);
  Vector<Rational> ParabolicEvaluationRootImage, tempRoot;
  ParabolicEvaluationRootImage = this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
  this->ParabolicSelectionSmallerAlgebra.initialize(input.theDomain().getRank());
  for (int i = 0; i < input.theDomain().getRank(); i ++) {
    DualCartanEmbedding.getVectorFromColumn(i, tempRoot);
    if (ParabolicEvaluationRootImage.scalarEuclidean(tempRoot).isPositive()) {
      this->ParabolicSelectionSmallerAlgebra.addSelectionAppendNewIndex(i);
    }
  }
  this->log << "\nDual cartan embedding smaller into larger:\n" <<
  DualCartanEmbedding.toString(&global.theDefaultFormat.getElement());
  this->log << "\nParabolic subalgebra large algebra: " << this->ParabolicLeviPartRootSpacesZeroStandsForSelected.toString();
  tempRoot = this->ParabolicSelectionSmallerAlgebra;
  this->log << "\nParabolic subalgebra smaller algebra: " << tempRoot.toString();
  theSubgroup.makeParabolicFromSelectionSimpleRoots(theWeYl, this->ParabolicLeviPartRootSpacesZeroStandsForSelected, - 1);

  this->theLinearOperators.setSize(theSubgroup.allElements.size);
  this->theLinearOperatorsExtended.setSize(theSubgroup.allElements.size);
  this->theTranslationS.setSize(theSubgroup.allElements.size);
  this->theTranslationsProjectedBasisChanged.setSize(theSubgroup.allElements.size);
  this->theCoeffs.setSize(theSubgroup.allElements.size);
  this->log << " \n******************\nthe subgroup: \n" << theSubgroup.toString() << "\n\n\n\n\n\n";
  this->log << theSubgroup.toStringBruhatGraph();
  this->log << "\nMatrix form of the elements of Weyl group of the Levi part of the parabolic ("
  << theSubgroup.allElements.size << " elements):\n";
  for (int i = 0; i < theSubgroup.allElements.size; i ++) {
    Matrix<Rational>& currentLinearOperator = this->theLinearOperators[i];
    theSubgroup.getMatrixOfElement(theSubgroup.allElements[i], currentLinearOperator);
//    currentLinearOperator.multiplyOnTheLeft(preferredBasisChangeInverse);
    this->log << "\n" << currentLinearOperator.toString(&global.theDefaultFormat.getElement());
    currentLinearOperator.actOnVectorColumn(theSubgroup.getRho(), this->theTranslationS[i]);
    this->theTranslationS[i] -= theSubgroup.getRho();
    this->theTranslationS[i].negate();
    theProjectionBasisChanged.actOnVectorColumn(this->theTranslationS[i], this->theTranslationsProjectedBasisChanged[i]);
    if (theSubgroup.allElements[i].generatorsLastAppliedFirst.size % 2 == 0) {
      this->theCoeffs[i] = 1;
    } else {
      this->theCoeffs[i] = - 1;
    }
  }
  this->log << "\n\n\nMatrix of the projection operator (basis-changed):\n"
  << theProjectionBasisChanged.toString(&global.theDefaultFormat.getElement());
  this->log << "\n\n\nMatrix form of the operators $u_w$, "
  << "the translations $\tau_w$ and their projections (" << this->theLinearOperatorsExtended.size << "):";
  //List<Matrix<Rational> > tempList;
  for (int k = 0; k < this->theLinearOperators.size; k ++) {
    Matrix<Rational>& currentLO = this->theLinearOperators[k];
    Matrix<Rational>& currentLOExtended = this->theLinearOperatorsExtended[k];
    currentLO.multiplyOnTheLeft(theProjectionBasisChanged);
    currentLO *= - 1;
    //tempList.addOnTopNoRepetition(this->theLinearOperators.objects[i]);
    currentLOExtended.makeIdentityMatrix(currentLO.numberOfRows);
    currentLOExtended.resize(currentLO.numberOfRows, currentLO.numberOfRows + currentLO.numberOfColumns, true);
    for (int i = 0; i < currentLO.numberOfRows; i ++) {
      for (int j = 0; j < currentLO.numberOfColumns; j ++) {
        currentLOExtended.elements[i][j + currentLO.numberOfRows] = currentLO.elements[i][j];
      }
    }
    this->log << "\n\n" << currentLOExtended.toString(&global.theDefaultFormat.getElement());
    this->log << this->theTranslationS[k].toString() << ";   " << this->theTranslationsProjectedBasisChanged[k].toString();
  }

  List<int> displayIndicesReflections;
  for (int i = 0; i < this->ParabolicLeviPartRootSpacesZeroStandsForSelected.numberOfElements; i ++) {
    if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i]) {
      displayIndicesReflections.addOnTop(i + 1);
    }
  }
  Matrix<Polynomial<Rational> > tempMatPoly;
  Vector<Polynomial<Rational> > tempVect, tempVect2;
  tempVect.setSize(input.theDomain().theWeyl.getDimension() + input.theRange().theWeyl.getDimension());
  for (int i = 0; i < tempVect.size; i ++) {
    tempVect[i].makeMonomial(i, 1, Rational(1));
  }
  tempMatPoly.initialize(input.theDomain().theWeyl.getDimension(), tempVect.size);
  Polynomial<Rational> polyZero;
  polyZero.makeZero();
  theFormat.polynomialAlphabet.setSize(5);
  theFormat.polynomialAlphabet[0] = "x_1";
  theFormat.polynomialAlphabet[1] = "x_2";
  theFormat.polynomialAlphabet[2] = "y_1";
  theFormat.polynomialAlphabet[3] = "y_2";
  theFormat.polynomialAlphabet[4] = "y_3";
  tempRoot = theSubgroup.getRho();
  this->theLinearOperators[0].actOnVectorColumn(tempRoot);
  this->preferredBasisChangE.actOnVectorColumn(tempRoot);
  tempRoot.negate();
  this->log << "\n\nIn $so(7)$-simple basis coordinates, $\\rho_{\\mathfrak l}="
  << theSubgroup.getRho().toStringLetterFormat("\\eta") << "$; $\\pr(\\rho)="
  << tempRoot.toStringLetterFormat("\\alpha") << "$.";
  this->log << "\n\n\\begin{longtable}{r|l}$w$ & \\begin{tabular}{c}"
  << "Argument of the vector partition function in (\\ref{eqMultG2inB3General}) =\\\\ $u_w\\circ"
  << tempVect.toString(&theFormat) << "-\\tau_w$ \\end{tabular}  \\\\ \\hline \\endhead";
  for (int i = 0; i < this->theLinearOperatorsExtended.size; i ++) {
    Matrix<Rational>& currentLoExt = this->theLinearOperatorsExtended[i];
    for (int j = 0; j < currentLoExt.numberOfRows; j ++) {
      for (int k = 0; k < currentLoExt.numberOfColumns; k ++) {
        tempMatPoly.elements[j][k].makeConstant(currentLoExt.elements[j][k]);
      }
    }
    tempMatPoly.actOnVectorColumn(tempVect, tempVect2, polyZero);
    for (int j = 0; j < tempVect2.size; j ++) {
      tempVect2[j] += this->theTranslationsProjectedBasisChanged[i][j];
    }
    this->log << "\n$" << theSubgroup.allElements[i].toString() << "$&$"
    << tempVect2.toString(&theFormat) << "$\\\\";
  }
  this->log << "\\end{longtable}\n\n";
//  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
  Lattice tempLattice;
  theWeYl.getIntegralLatticeInSimpleCoordinates(tempLattice);
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.makeIdentityMatrix(input.theDomain().getRank());
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.directSumWith(tempLattice.basisRationalForm, Rational(0));
  this->theExtendedIntegralLatticeMatForM.makeFromMatrix(this->theExtendedIntegralLatticeMatForM.basisRationalForm);
  tempMat = theWeYl.cartanSymmetric;
  tempMat.invert();
  Vectors<Rational> WallsWeylChamberLargerAlgebra;
  for (int i = 0; i < tempMat.numberOfRows; i ++) {
    tempMat.getVectorFromRow(i, tempRoot);
    if (ParabolicEvaluationRootImage[i].isEqualToZero()) {
      WallsWeylChamberLargerAlgebra.setSize(WallsWeylChamberLargerAlgebra.size + 1);
      *WallsWeylChamberLargerAlgebra.lastObject() = tempRoot;
    }
  }
  this->log << "\n\n\n**************\nParabolic selection larger algebra:"
  << ParabolicEvaluationRootImage.toString() << "\nWalls Weyl chamber larger algebra: "
  << WallsWeylChamberLargerAlgebra.toString();
  this->log << "\n**************\n\n";
  Vectors<Rational> rootsGeneratingExtendedLattice;
  int totalDim = input.theRange().getRank() + input.theDomain().getRank();
  rootsGeneratingExtendedLattice.setSize(totalDim);
  this->log << "\n" << tempMat.toString(&global.theDefaultFormat.getElement()) << "\n";
  this->log << this->theExtendedIntegralLatticeMatForM.toString();
  this->WeylChamberSmallerAlgebra.createFromNormals(WallsWeylChamberLargerAlgebra);
  this->log << "\nWeyl chamber larger algebra before projectivizing: " << this->WeylChamberSmallerAlgebra.toString(&theFormat) << "\n";
  this->PreimageWeylChamberSmallerAlgebra.normals = this->WeylChamberSmallerAlgebra.normals;
  for (int i = 0; i < this->PreimageWeylChamberLargerAlgebra.normals.size; i ++) {
    tempRoot.makeZero(input.theRange().getRank() + input.theDomain().getRank() + 1);
    for (int j = 0; j < input.theRange().getRank(); j ++) {
      tempRoot[j + input.theDomain().getRank()] = this->PreimageWeylChamberLargerAlgebra.normals[i][j];
    }
    this->PreimageWeylChamberLargerAlgebra.normals[i] = tempRoot;
  }
  tempMat = input.theDomain().theWeyl.cartanSymmetric;
  tempMat.invert();
  tempRoots.size = 0;
  Vector<Rational> ParabolicEvaluationRootSmallerAlgebra;
  ParabolicEvaluationRootSmallerAlgebra = this->ParabolicSelectionSmallerAlgebra;
  for (int i = 0; i < tempMat.numberOfRows; i ++) {
    input.theDomain().theWeyl.cartanSymmetric.getVectorFromRow(i, tempRoot);
    if (tempRoot.scalarEuclidean(ParabolicEvaluationRootSmallerAlgebra).isEqualToZero()) {
      tempRoots.setSize(tempRoots.size + 1);
      tempMat.getVectorFromRow(i, *tempRoots.lastObject());
    }
  }
  this->preferredBasisChangeInversE.actOnVectorsColumn(tempRoots);
  this->log << "**********************\n\n\n";
  this->log << "\nthe smaller parabolic selection: " << this->ParabolicSelectionSmallerAlgebra.toString();
  this->log << "the Vectors<Rational> generating the chamber walls: " << tempRoots.toString();
  this->PreimageWeylChamberSmallerAlgebra.createFromVertices(tempRoots);
  this->log << "\nWeyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.toString(&theFormat) << "\n";
  this->log << "**********************\n\n\n";
  this->log << "\nThe first operator extended:\n"
  << this->theLinearOperatorsExtended[0].toString(&global.theDefaultFormat.getElement()) << "\n";
  this->log << "\nThe second operator extended:\n"
  << this->theLinearOperatorsExtended[1].toString(&global.theDefaultFormat.getElement()) << "\n";
  /*tempMat = this->theLinearOperatorsExtended.objects[0];
  tempMat.transpose();
  tempMat.actOnVectorsColumn(this->PreimageWeylChamberSmallerAlgebra);
  for (int i = 0; i < this->PreimageWeylChamberSmallerAlgebra.size; i ++) {
    this->PreimageWeylChamberSmallerAlgebra.objects[i].setSize(input.theRange.getRank() + input.theDomain.getRank() + 1);
    *this->PreimageWeylChamberSmallerAlgebra.objects[i].lastObject()= 0;
  }*/
  for (int i = 0; i < this->PreimageWeylChamberSmallerAlgebra.normals.size; i ++) {
    tempRoot.makeZero(input.theRange().getRank() + input.theDomain().getRank() + 1);
    for (int j = 0; j < input.theDomain().getRank(); j ++) {
      tempRoot[j] = this->PreimageWeylChamberSmallerAlgebra.normals[i][j];
    }
  //  for (int j = 0; j < input.theRange.getRank(); j ++)
   //   tempRoot.objects[j+ input.theDomain.getRank()] = tempRoot2.objects[j];
    this->PreimageWeylChamberSmallerAlgebra.normals[i] = tempRoot;
  }

  tempRoot.makeEi(
    input.theRange().getRank() + input.theDomain().getRank() + 1,
    input.theRange().getRank() + input.theDomain().getRank()
  );
  this->PreimageWeylChamberLargerAlgebra.normals.addOnTop(tempRoot);
  this->log << "\nPreimage Weyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.toString(&theFormat) << "\n";
  this->log << "\nPreimage Weyl chamber larger algebra: " << this->PreimageWeylChamberLargerAlgebra.toString(&theFormat) << "\n";

  //global.theIndicatorVariables.StatusString1NeedsRefresh = true;
  //global.theIndicatorVariables.StatusString1= this->log.str();
  //global.makeReport();
}

std::string GeneralizedVermaModuleCharacters::prepareReport() {
  std::stringstream out;
  FormatExpressions theFormat;
  int tempI = 0;
  theFormat.polynomialAlphabet.setSize(5);
  theFormat.polynomialAlphabet[tempI] = "x_1";
  tempI ++;
  theFormat.polynomialAlphabet[tempI] = "x_2";
  tempI ++;
  theFormat.polynomialAlphabet[tempI] = "y_1";
  tempI ++;
  theFormat.polynomialAlphabet[tempI] = "y_2";
  tempI ++;
  theFormat.polynomialAlphabet[tempI] = "y_3";
  tempI ++;
  out << "\\documentclass{article}\\usepackage{amsmath, longtable, amsfonts, amssymb, verbatim, hyperref}"
  << "\n\\begin{document}\\tiny\n";
  out << "\n The chamber complex + multiplicities follow.\n\n\n"
  << "\\begin{longtable}{cc}\\caption{multiplicities of generalized Verma modules $m(x_1,x_2, y_1, y_2, y_3)$"
  << " for $\\gop$ with Dynkin diagram";
  std::stringstream tempStream;
  tempStream << "(";
  for (int i = 0; i < this->ParabolicLeviPartRootSpacesZeroStandsForSelected.numberOfElements; i ++) {
    if (this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i]) {
      tempStream << "+";
    } else {
      tempStream << "0";
    }
    if (i != this->ParabolicLeviPartRootSpacesZeroStandsForSelected.numberOfElements - 1) {
      tempStream << ",";
    }
  }
  tempStream << ")";
  out << "$" << tempStream.str() <<  "$ \\label{table" << tempStream.str() << "}}\\\\\n";
  out << "Inequlities& $m(x_1,x_2, y_1, y_2, y_3)$\\endhead\n";
  int numFoundChambers = 0;
  List<int> DisplayIndicesprojectivizedChambers;
  for (int i = 0; i < this->projectivizedChambeR.size; i ++) {
    QuasiPolynomial& theMult = this->theMultiplicities[i];
    if (!theMult.isEqualToZero()) {
      numFoundChambers ++;
      out << "\\hline\\multicolumn{2}{c}{Chamber " << numFoundChambers << "}\\\\\n";
      DisplayIndicesprojectivizedChambers.addOnTop(numFoundChambers);
      out << this->prepareReportOneCone(theFormat, this->projectivizedChambeR[i]) << "&";
      out << "\\begin{tabular}{c}";
      out << theMult.toString(false, true, &theFormat) << "\\end{tabular}\\\\\n";
    } else {
      DisplayIndicesprojectivizedChambers.addOnTop(- 1);
    }
  }
  out << "\\end{longtable}\n\n\n Multiplicity free chambers \n";
  numFoundChambers = 0;
  out << "\n\\begin{longtable}{cc} ";
  out << "normals& Multiplicity of module with highest weight $(x_1,x_2)$\\endhead\n";
 /* for (int i = 0; i < this->projectivezedChambersSplitByMultFreeWalls.size; i ++) {
    tempRoot = this->projectivezedChambersSplitByMultFreeWalls.objects[i].getInternalPoint();
    bool found = false;
    for (int j = 0; j < this->projectivizedChamber.size; j ++)
      if (this->projectivizedChamber.objects[j].isInCone(tempRoot)) {
        if (found)
          global.fatal << global.fatal;
        found = true;
      }
  }
  for (int i = 0; i < this->projectivizedChamber.size; i ++) {
    QuasiPolynomial& theMult = this->theMultiplicities.objects[i];
    if (!theMult.isEqualToZero()) {
      int indexMultFreeChamber = - 1;
      for (int j = 0; j < this->projectivezedChambersSplitByMultFreeWalls.size; j ++) {
        tempRoot = this->projectivezedChambersSplitByMultFreeWalls.objects[j].getInternalPoint();
        if (this->projectivizedChamber.objects[i].isInCone(tempRoot)) {
          Rational tempRat;
          tempRat =*tempRoot.lastObject();
          if (tempRat != 0)
            tempRoot/= tempRat;
          theMult.valueOnEachLatticeShift.objects[0].evaluate(tempRoot, tempRat);
          if (tempRat<1) {
            indexMultFreeChamber = j;
            break;
          }
        }
      }
      if (indexMultFreeChamber!= - 1) {
        numFoundChambers++;
        out << "\\hline\\multicolumn{2}{c}{Chamber " << DisplayIndicesprojectivizedChambers.objects[i] << "}\\\\\n";
        out << this->prepareReportOneCone(theFormat, this->projectivezedChambersSplitByMultFreeWalls.objects[indexMultFreeChamber]) << "&";
        out << theMult.toString(false, true, theFormat) << "\\\\\n";
      }
    }
  }*/
  out << "Total number of chambers with multiplicity 1 or less: " << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

void GeneralizedVermaModuleCharacters::initTheMaxComputation() {
  MacroRegisterFunctionWithName("GeneralizedVermaModuleCharacters::initTheMaxComputation");
  this->theMaxComputation.numNonParaM = 2;
  this->theMaxComputation.theConesLargerDim.reserve(this->projectivizedChambeR.size);
  this->theMaxComputation.LPtoMaximizeLargerDim.reserve(this->theMultiplicities.size);
  this->theMaxComputation.theConesLargerDim.setSize(0);
  this->theMaxComputation.LPtoMaximizeLargerDim.setSize(0);
  Lattice ZnLattice;
  int theAffineDim = 5;
//  int theProjectivizedDim= theAffineDim + 1;
  ZnLattice.makeZn(theAffineDim);
  this->numNonZeroMults = 0;
  ProgressReport theReport;
  ConeLatticeAndShift currentCLS;
  Vector<Rational> theLPtoMax;
  for (int i = 0; i < this->theMultiplicities.size; i ++) {
    if (! this->theMultiplicities[i].isEqualToZero()) {
      currentCLS.theProjectivizedCone = this->projectivizedChambeR[i];
      currentCLS.theShift.makeZero(theAffineDim);
      currentCLS.theLattice = ZnLattice;
      bool tempBool = this->theMultiplicities[i].valueOnEachLatticeShift[0].getRootFromLinearPolynomialConstantTermLastVariable(theLPtoMax);
      if (!tempBool) {
        global.fatal << "This should not happen. " << global.fatal;
      }
      this->theMaxComputation.theConesLargerDim.addOnTop(currentCLS);
      this->theMaxComputation.LPtoMaximizeLargerDim.addOnTop(theLPtoMax);
      this->numNonZeroMults ++;
      std::stringstream out;
      out << "Initialized " << i + 1 << " out of " << this->theMaxComputation.theConesLargerDim.size
      << "; so far " << this->numNonZeroMults << " non-zero multiplicities";
      theReport.report(out.str());
    }
  }
}

std::string GeneralizedVermaModuleCharacters::prepareReportOneCone(FormatExpressions& theFormat, const Cone& theCone) {
  std::stringstream out1;
  Vector<Rational> normalNoConstant;
  int dimSmallerAlgebra = this->theLinearOperators[0].numberOfRows;
  int dimLargerAlgebra = this->theLinearOperators[0].numberOfColumns;
  Rational theConst;
  out1 << "\\begin{tabular}{rcl}";
  for (int i = 0; i < theCone.normals.size; i ++) {
    Vector<Rational>& currentNormal = theCone.normals[i];
    normalNoConstant = currentNormal;
    normalNoConstant.setSize(dimSmallerAlgebra + dimLargerAlgebra);
    theConst = - (*currentNormal.lastObject());
    if (!normalNoConstant.isEqualToZero()) {
      out1 << "$" << normalNoConstant.toStringLetterFormat("x", &theFormat) << "$ & $\\geq$ & $"
      << theConst.toString() << "$ \\\\";
    }
  }
  out1 << "\\end{tabular}";
  return out1.str();
}

std::string GeneralizedVermaModuleCharacters::elementToStringMultiplicitiesReport() {
  if (this->theMultiplicities.size != this->projectivizedChambeR.size) {
    global.fatal << "Bad number of multiplicities. " << global.fatal;
  }
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.polynomialAlphabet.setSize(5);
  theFormat.polynomialAlphabet[0] = "x_1";
  theFormat.polynomialAlphabet[1] = "x_2";
  theFormat.polynomialAlphabet[2] = "y_1";
  theFormat.polynomialAlphabet[3] = "y_2";
  theFormat.polynomialAlphabet[4] = "y_3";
  out << "Number chambers: " << projectivizedChambeR.size << " of them " << this->numNonZeroMults << " non-zero.";
  int numInequalities = 0;
  for (int i = 0; i < this->projectivizedChambeR.size; i ++) {
    numInequalities += this->projectivizedChambeR[i].normals.size;
  }
  out << "\nNumber of inequalities: " << numInequalities;
  if (this->ParabolicLeviPartRootSpacesZeroStandsForSelected.cardinalitySelection != 0) {
    out << this->prepareReport();
  }
  return out.str();
}

void GeneralizedVermaModuleCharacters::getSubstitutionFromNonParametricArray(
  Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams
) {
  //reminder: the very last variable comes from the projectivization and contributes to the translation only!
  int numNonParams = NonParams.size;
  output.initialize(numParams + numNonParams - 1, numParams - 1);
  outputTranslation.makeZero(numParams + numNonParams - 1);
  output.makeZero();
  for (int l = 0; l < numNonParams; l ++) {
    for (int k = 0; k < numParams - 1; k ++) {
      output.elements[l][k] = NonParams[l][k];
    }
    outputTranslation[l] = *NonParams[l].lastObject();
  }
  for (int l = 0; l < numParams - 1; l ++) {
    output.elements[l + numNonParams][l] = 1;
  }
}

void GeneralizedVermaModuleCharacters::getProjection(
  int indexOperator, const Vector<Rational>& input, Vector<Rational>& output
) {
  Matrix<Rational>& currentExtendedOperator = this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& currentTranslation = this->theTranslationsProjectedBasisChanged[indexOperator];
  if (input.lastObject()->isEqualToZero()) {
    global.fatal << "Last coordinate is not supposed to be be zero. " << global.fatal;
  }
  output = input;
  Rational tempRat = *output.lastObject();
  output /= tempRat;
  output.size --;
  currentExtendedOperator.actOnVectorColumn(output);
  output += currentTranslation;
}

void GeneralizedVermaModuleCharacters::getSubstitutionFromIndex(
  PolynomialSubstitution<Rational>& outputSub, Matrix<LargeInteger>& outputMat, LargeIntegerUnsigned& outputDen, int theIndex
) {
  Matrix<Rational>& theOperator = this->theLinearOperators[theIndex];
  int dimLargerAlgebra = theOperator.numberOfColumns;
  int dimSmallerAlgebra = theOperator.numberOfRows;
  Vector<Rational>& theTranslation = this->theTranslationS[theIndex];
  Matrix<Rational> tempMat;
  tempMat.initialize(dimLargerAlgebra + dimSmallerAlgebra + 1, dimSmallerAlgebra);
  tempMat.makeZero();
  for (int j = 0; j < dimSmallerAlgebra; j ++) {
    tempMat.elements[j][j] = 1;
    for (int i = 0; i < dimLargerAlgebra; i ++) {
      tempMat.elements[i][j] -= theOperator.elements[j][i];
    }
    tempMat.elements[dimLargerAlgebra + dimSmallerAlgebra][j] = - theTranslation[j];
  }
  tempMat.getMatrixIntegerWithDenominator(outputMat, outputDen);
  outputSub.makeSubstitutionFromMatrixIntegerAndDenominator(outputMat, outputDen);
}

void GeneralizedVermaModuleCharacters::transformToWeylProjective(
  int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal
) {
  Matrix<Rational> theOperatorExtended = this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& theTranslation = this->theTranslationsProjectedBasisChanged[indexOperator];
  //the goddamned sign in front of theTranslation is now checked: it should be + and not -
  Rational theConst;
  startingNormal.scalarEuclidean(this->NonIntegralOriginModificationBasisChanged + theTranslation, theConst);
  theOperatorExtended.transpose();
  outputNormal = startingNormal;
  theOperatorExtended.actOnVectorColumn(outputNormal);
  outputNormal.setSize(outputNormal.size + 1);
  *outputNormal.lastObject() = - theConst;
}

void GeneralizedVermaModuleCharacters::transformToWeylProjectiveStep1() {
  this->smallerAlgebraChamber.initializeFromDirectionsAndRefine(this->GmodKNegWeightsBasisChanged);
  ProgressReport theReport1;
  ProgressReport theReport2;
  theReport1.report(this->smallerAlgebraChamber.toString(false));
  this->log << "Directions for making the chamber basis changed: " << this->GmodKNegWeightsBasisChanged.toString()
  << "\n Resulting chamber before projectivization:\n "
  << this->smallerAlgebraChamber.toString(false);
  theReport2.report(this->log.str());
}

void GeneralizedVermaModuleCharacters::transformToWeylProjectiveStep2() {
  std::stringstream out;
  ConeComplex projectivizedChamberFinal;
  Cone currentProjectiveCone;
  Vectors<Rational> tempRoots;
  Vector<Rational> wallToSliceWith;
  ProgressReport theReport;
  projectivizedChamberFinal.initialize();
  for (int i = 0; i < this->smallerAlgebraChamber.size; i ++) {
    Cone& currentAffineCone = this->smallerAlgebraChamber.objects[i];
    tempRoots.setSize(currentAffineCone.normals.size);
    for (int j = 0; j < currentAffineCone.normals.size; j ++) {
      this->transformToWeylProjective(0, currentAffineCone.normals[j], tempRoots[j]);
    }
    tempRoots.addListOnTop(this->PreimageWeylChamberLargerAlgebra.normals);
    theReport.report(tempRoots.toString());
    currentProjectiveCone.createFromNormals(tempRoots);
    projectivizedChamberFinal.addNonRefinedChamberOnTopNoRepetition(currentProjectiveCone);
  }
  for (int i = 0; i < this->PreimageWeylChamberSmallerAlgebra.normals.size; i ++) {
    projectivizedChamberFinal.splittingNormals.addOnTop(this->PreimageWeylChamberSmallerAlgebra.normals[i]);
  }
  out << "projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.toString(false);
  projectivizedChamberFinal.refine();
  out << "Refined projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.toString(false);
  for (int i = 0; i < projectivizedChamberFinal.size; i ++) {
    const Cone& currentCone = projectivizedChamberFinal[i];
    bool isNonDominant = false;
    for (int j = 0; j < this->PreimageWeylChamberSmallerAlgebra.normals.size; j ++) {
      if (currentCone.getInternalPoint().scalarEuclidean(this->PreimageWeylChamberSmallerAlgebra.normals[j]).isNegative()) {
        isNonDominant = true;
        break;
      }
    }
    if (isNonDominant) {
      projectivizedChamberFinal.popChamberSwapWithLast(i);
      i --;
    }
  }
  theReport.report(out.str());
  projectivizedChamberFinal.indexLowestNonRefinedChamber = 0;
  this->projectivizedChambeR= projectivizedChamberFinal;
  for (int k = 1; k < this->theLinearOperators.size; k ++) {
    for (int i = 0; i < this->smallerAlgebraChamber.size; i ++) {
      for (int j = 0; j < this->smallerAlgebraChamber[i].normals.size; j ++) {
        this->transformToWeylProjective(k, this->smallerAlgebraChamber[i].normals[j], wallToSliceWith);
        wallToSliceWith.scaleNormalizeFirstNonZero();
        this->projectivizedChambeR.splittingNormals.addOnTopNoRepetition(wallToSliceWith);
      }
    }
  }
  out << "projectivized chamber chopped non-dominant part:\n"  << this->projectivizedChambeR.toString(false);
  theReport.report(out.str());
}
