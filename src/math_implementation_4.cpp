#include "math_general.h"
#include "math_general_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_drawing_variables.h"
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
  if (!this->flagCrashInitiated && global.flagRunningBuiltInWebServer) {
    double elapsedSeconds = global.getElapsedSeconds();
    this->crashReportHtml
    << "\n<b style = 'color:red'>Crash</b> "
    << elapsedSeconds
    << " second(s) from the start.<hr>";
    this->crashReportConsolE
    << Logger::consoleRed()
    << "Crash "
    << elapsedSeconds
    << " second(s)"
    << Logger::consoleNormal()
    << " from the start.\n";
    this->crashReportFile
    << "Crash "
    << elapsedSeconds
    << " second(s) from the start.\n";
  }
  this->flagCrashInitiated = true;
}

GlobalVariables::Crasher& GlobalVariables::Crasher::doCrash(
  const Crasher& dummyCrasherSignalsActualCrash
) {
  (void) dummyCrasherSignalsActualCrash;
  this->firstRun();
  if (this->flagFinishingCrash) {
    std::cout
    << "Recursion within the crashing mechanism detected. "
    << "Something is very wrong. "
    << this->crashReportConsolE.str()
    << std::endl;
    std::exit(- 1);
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
      this->crashReportHtml
      << "<hr>User input: <br> "
      << global.userInputStringIfAvailable
      << "<hr>";
      this->crashReportConsolE
      << "User input: "
      << Logger::consoleBlue()
      << global.userInputStringIfAvailable
      << Logger::consoleNormal()
      << "\n";
      this->crashReportFile
      << "User input:\n"
      << global.userInputStringIfAvailable
      << "\n";
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
      this->crashReportConsolE
      << global.toStringProgressReportNoThreadData(false);
    }
  }
  this->writeCrashFile();
  std::cout << this->crashReportConsolE.str() << std::endl;
  JSData output;
  output[WebAPI::Result::crashReport] = this->crashReportHtml.str();
  output[WebAPI::Result::comments] = global.comments.getCurrentReset();
  global.response.writeResponse(output, true);
  std::exit(- 1);
  return *this;
}

void GlobalVariables::Crasher::writeCrashFile() {
  if (global.flagNotAllocated) {
    this->crashReportHtml << "GlobalVariables.flagNotAllocated is true. ";
    this->crashReportConsolE << "GlobalVariables.flagNotAllocated is true. ";
    return;
  }
  if (global.flagRunningWebAssembly) {
    this->crashReportHtml << "Crash while running web assembly.";
    this->crashReportConsolE << "Crash while running web assembly.";
    return;
  }
  if (!global.calculator().isZeroPointer()) {
    if (global.calculator().getElement().comments.str() != "") {
      this->crashReportHtml
      << "<hr>Additional comments follow. "
      << global.calculator().getElement().comments.str();
    }
  }
  std::fstream crashFile;
  bool openSuccess =
  FileOperations::openFileCreateIfNotPresentVirtual(
    crashFile,
    "crashes/" + global.relativePhysicalNameCrashReport,
    false,
    true,
    false,
    true,
    true
  );
  if (openSuccess) {
    this->crashReportHtml
    << "<hr>Crash dumped in folder "
    << "results/crashes/. Not visible through the web server. "
    << "If running locally, simply open the results/crashes "
    << "folder within your calculator folder. "
    << "If running remotely, you will need an ssh connection. ";
    this->crashReportConsolE
    << "Crash dumped in file: "
    << Logger::consoleGreen()
    << global.relativePhysicalNameCrashReport
    << Logger::consoleNormal()
    << "\n";
  } else {
    this->crashReportHtml
    << "<hr>Failed to open crash report file: "
    << global.relativePhysicalNameCrashReport
    << ". check file permissions. ";
    this->crashReportConsolE
    << "Failed to open crash report file: "
    << Logger::consoleRed()
    << global.relativePhysicalNameCrashReport
    << Logger::consoleNormal()
    << "\n";
  }
  crashFile << this->crashReportFile.str();
  crashFile.flush();
  crashFile.close();
}

std::string GlobalVariables::Crasher::getStackTraceEtcErrorMessageHTML() {
  std::stringstream out;
  out
  << "A partial stack trace follows "
  << "(function calls not explicitly logged not included).";
  out << "<table><tr>";
  for (
    int threadCounter = 0; threadCounter < global.customStackTrace.size;
    threadCounter ++
  ) {
    if (threadCounter >= global.threadData.size) {
      out
      << "<td><b>WARNING: the stack trace reports "
      << global.customStackTrace.size
      << " threads but the thread data array has record of only "
      << global.threadData.size
      << " threads. "
      << "</b></td>";
      break;
    }
    out
    << "<td>"
    << global.threadData[threadCounter].toStringHtml()
    << "</td>";
  }
  out << "</tr> <tr>";
  for (
    int threadCounter = 0; threadCounter < global.customStackTrace.size;
    threadCounter ++
  ) {
    if (threadCounter >= global.threadData.size) {
      break;
    }
    if (ThreadData::getCurrentThreadId() != threadCounter) {
      out << "<td>Stack trace available only for current thread.</td>";
      // <-to avoid coordinating threads
      continue;
    }
    ListReferences<StackInfo>& currentInfo =
    global.customStackTrace[threadCounter];
    out
    << "<td><table><tr><td>file</td><td>line</td>"
    << "<td>function name (if known)</td></tr>";
    for (int i = currentInfo.size - 1; i >= 0; i --) {
      out
      << "<tr><td>"
      << HtmlRoutines::getHtmlLinkFromProjectFileName(
        currentInfo[i].fileName, "", currentInfo[i].line
      )
      << "</td><td>"
      << currentInfo[i].line
      << "</td>";
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
  for (
    int threadCounter = 0; threadCounter < global.customStackTrace.size;
    threadCounter ++
  ) {
    if (threadCounter >= global.threadData.size) {
      out
      << "WARNING: stack trace reports "
      << global.customStackTrace.size
      << " threads "
      << "while I have only "
      << global.threadData.size
      << " registered threads. ";
      break;
    }
    out
    << "********************\r\nThread index "
    << threadCounter
    << ": \r\n";
    if (ThreadData::getCurrentThreadId() != threadCounter) {
      out << "Stack trace available only for current thread.\n";
      // <-to avoid coordinating threads
      continue;
    }
    ListReferences<StackInfo>& currentInfo =
    global.customStackTrace[threadCounter];
    for (int i = currentInfo.size - 1; i >= 0; i --) {
      out << currentInfo[i].functionName << "\n";
    }
  }
  return out.str();
}

std::string GlobalVariables::toHTMLTopCommandLinuxSystem() {
  STACK_TRACE("GlobalVariables::toHTMLTopCommandLinuxSystem");
  std::string topString = this->externalCommandReturnOutput("top -b -n 1 -s");
  std::stringstream out;
  std::string lineString, wordString;
  std::stringstream topStream(topString);
  for (int i = 0; i < 4; i ++) {
    std::getline(topStream, lineString);
    out << lineString << "<br>\n ";
  }
  if (!global.userDefaultHasAdminRights()) {
    out << "Login as admin for full top printout.";
    return out.str();
  }
  out << "<table>";
  for (; std::getline(topStream, lineString);) {
    out << "<tr>";
    for (
      std::stringstream nextLineStream(lineString); nextLineStream >>
      wordString;
    ) {
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
  out
  << "<br>Physical name folder below executable: "
  << this->physicalNameFolderExecutable;
  out << "<br>Display path output folder: " << this->displayPathOutputFolder;
  return out.str();
}

std::string GlobalVariables::toStringThreadData(bool useHTML) {
  std::stringstream out;
  for (
    int threadIndex = 0; threadIndex < this->progressReportStrings.size;
    threadIndex ++
  ) {
    if (useHTML) {
      out << "<hr><b>";
    }
    out << this->threadData[threadIndex].toStringHtml();
    if (useHTML) {
      out << "</b><br>";
    }
    out << "\n";
  }
  return out.str();
}

std::string GlobalVariables::toStringProgressReportWithThreadData(bool useHTML)
{
  STACK_TRACE("GlobalVariables::toStringProgressReportWithThreadData");
  std::stringstream out;
  out << global.toStringThreadData(useHTML);
  out << global.toStringProgressReportNoThreadData(useHTML);
  return out.str();
}

std::string GlobalVariables::toStringProgressReportNoThreadData(bool useHTML) {
  STACK_TRACE("GlobalVariables::toStringProgressReportNoThreadData");
  std::stringstream reportStream;
  for (
    int threadIndex = 0; threadIndex < this->progressReportStrings.size;
    threadIndex ++
  ) {
    int currentThreadID = ThreadData::getCurrentThreadId();
    if (currentThreadID != threadIndex) {
      // <-to avoid coordinating threads
      continue;
    }
    if (useHTML) {
      reportStream << "<b>Current thread id: " << currentThreadID << ". </b>";
    } else {
      reportStream
      << "Thread id: "
      << Logger::consoleBlue()
      << currentThreadID
      << Logger::consoleNormal()
      << "\n";
    }
    for (
      int i = 0; i < this->progressReportStrings[threadIndex].size; i ++
    ) {
      if (this->progressReportStrings[threadIndex][i] != "") {
        if (useHTML) {
          reportStream
          << "\n<div id ='divProgressReport"
          << i
          << "'>"
          << this->progressReportStrings[threadIndex][i]
          << "\n</div>\n<hr>";
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
    reportStream << global.getElapsedMilliseconds() << " ms elapsed. ";
    if (global.millisecondsMaxComputation > 0) {
      if (useHTML) {
        reportStream << "<br>";
      }
      reportStream
      << "\nHard limit: "
      << global.millisecondsMaxComputation
      << " ms [system crash if limit exceeded].";
      if (useHTML) {
        reportStream << "<br>";
      }
      reportStream
      << "\nSoft limit: "
      << global.millisecondsMaxComputation / 2
      << " ms [computation error if limit exceeded, "
      << "triggered between calculator/atomic functions].";
    }
  }
  return reportStream.str();
}

std::string GlobalVariables::toStringProgressReportConsole() {
  STACK_TRACE("GlobalVariables::toStringProgressReportConsole");
  std::stringstream reportStream;
  for (
    int threadIndex = 0; threadIndex < this->progressReportStrings.size;
    threadIndex ++
  ) {
    if (ThreadData::getCurrentThreadId() != threadIndex) {
      reportStream << "Progress report available only for current thread.<br>";
      // <-to avoid coordinating threads
      continue;
    }
    reportStream << this->threadData[threadIndex].toStringConsole();
    for (
      int i = 0; i < this->progressReportStrings[threadIndex].size; i ++
    ) {
      reportStream << this->progressReportStrings[threadIndex][i];
    }
  }
  reportStream << "\n";
  return reportStream.str();
}

void GlobalVariables::initThreadsExecutableStart() {
  // <-Stack trace forbidden this is running before anything has been
  // initialized!
  ThreadData::registerFirstThread("main");
}

void GlobalVariables::initializeMathematics() {
  int start = global.getElapsedMilliseconds();
  LargeIntegerUnsigned::allPrimesSmallerThan15Bits();
  int duration = global.getElapsedMilliseconds() - start;
  global
  << "Initializing precomputed math took "
  << Logger::blue
  << duration
  << " ms."
  << Logger::endL;
}

void GlobalVariables::initFoldersProjectBase(
  const std::string& inputPhysicalExecutable
) {
  StateMaintainerCurrentFolder preserveCurrentFolder;
  this->physicalPathProjectBase =
  FileOperations::getPathFromFileNameWithPath(inputPhysicalExecutable) + "./";
  this->changeDirectory(this->physicalPathProjectBase);
  this->physicalPathProjectBase = FileOperations::getCurrentFolder() + "/";
}

void GlobalVariables::initDefaultFolderAndFileNames() {
  this->initFoldersProjectBase(global.pathExecutableUserInputOrDeduced);
  this->physicalNameFolderExecutable = this->physicalPathProjectBase;
  this->physicalNameExecutableNoPath =
  FileOperations::getFileNameFromFileNameWithPath(
    global.pathExecutableUserInputOrDeduced
  );
  this->physicalNameExecutableWithPath =
  this->physicalNameFolderExecutable + this->physicalNameExecutableNoPath;
  this->physicalPathServerBase = this->physicalPathProjectBase;
  this->displayPathOutputFolder = "/output/";
  this->displayNameExecutable =
  "/cgi-bin/" + this->physicalNameExecutableNoPath;
  this->displayApplication = "/" + WebAPI::app;
  this->displayApplicationNoCache = "/" + WebAPI::appNoCache;
  this->initOutputReportAndCrashFileNames("", "");
}

void GlobalVariables::setWebInput(
  const std::string& inputName, const std::string& inputValue
) {
  STACK_TRACE("GlobalVariables::setWebInput");
  this->webArguments.setKeyValue(inputName, inputValue);
}

bool GlobalVariables::userSecureNonAdminOperationsAllowed() {
  return this->flagLoggedIn && this->flagUsingSSLinCurrentConnection;
}

bool GlobalVariables::userDebugFlagOn() {
  return global.getWebInput(WebAPI::Request::debugFlag) == "true";
}

bool GlobalVariables::userStudentVieWOn() {
  return global.getWebInput("studentView") == "true";
}

std::string GlobalVariables::LogData::toStringProcessType() const {
  switch (this->logType) {
  case GlobalVariables::LogData::type::initialization:
    return "initialization";
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
    global.fatal
    << "Global variables not allowed to be deallocated. "
    << global.fatal;
  }
  return true;
}

bool GlobalVariables::userDefaultIsDebuggingAdmin() {
  return this->userDefaultHasAdminRights() && this->userDebugFlagOn();
}

bool GlobalVariables::userDefaultHasAdminRights() {
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    return true;
  }
  return
  this->flagLoggedIn && (
    this->userDefault.userRole == UserCalculatorData::Roles::administator
  );
}

bool GlobalVariables::userDefaultHasProblemComposingRights() {
  return
  this->flagLoggedIn && (
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
  this->requestType == WebAPI::Request::problemGiveUpNoLogin ||
  this->requestType == WebAPI::Request::submitExerciseNoLogin ||
  this->requestType == WebAPI::Request::submitExercisePreviewNoLogin ||
  this->requestType == "templateNoLogin";
}

bool GlobalVariables::userRequestRequiresLoadingRealExamData() {
  if (this->userGuestMode()) {
    return false;
  }
  return
  this->flagLoggedIn && (
    this->requestType == "scoredQuiz" ||
    this->requestType == WebAPI::Frontend::scoredQuiz ||
    this->requestType == "submitAnswers" ||
    this->requestType == "submitAnswersPreview"
  );
}

bool GlobalVariables::userRequestMustBePromptedToLogInIfNotLoggedIn() {
  return this->requestType == "scoredQuiz" || this->requestType == "exercise";
}

std::string GlobalVariables::toStringCalculatorArgumentsNoNavigation(
  List<std::string>* tagsToExclude
) {
  STACK_TRACE("GlobalVariables::toStringCalculatorArgumentsNoNavigation");
  if (!this->flagLoggedIn && !this->userGuestMode()) {
    return "";
  }
  std::stringstream out;
  for (int i = 0; i < this->webArguments.size(); i ++) {
    const std::string& currentName = this->webArguments.keys[i];
    if (
      currentName == "request" ||
      currentName == DatabaseStrings::labelPassword ||
      currentName == WebAPI::Problem::fileName ||
      currentName == WebAPI::Problem::courseHome ||
      currentName == WebAPI::Problem::topicList ||
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
      currentName == DatabaseStrings::labelUsername ||
      currentName == "authenticationToken"
    ) {
      continue;
    }
    if (tagsToExclude != nullptr) {
      if (tagsToExclude->contains(currentName)) {
        continue;
      }
    }
    out
    << global.webArguments.keys[i]
    << "="
    << global.webArguments.values[i]
    << "&";
  }
  return out.str();
}

std::string GlobalVariables::getWebInput(const std::string& inputName) {
  return this->webArguments.getValueCreateEmpty(inputName);
}

void GlobalVariables::makeReport() {
  STACK_TRACE("GlobalVariables::makeReport");
  if (!global.response.monitoringAllowed()) {
    return;
  }
  std::string reportString;
  if (this->flagRunningConsoleRegular || this->flagRunningConsoleTest) {
    reportString = this->toStringProgressReportConsole();
  } else {
    reportString = this->toStringProgressReportNoThreadData(true);
  }
  this->response.report(reportString);
}

void GlobalVariables::initOutputReportAndCrashFileNames(
  const std::string& inputUserStringRAW,
  const std::string& inputUserStringCivilized
) {
  std::string inputAbbreviated;
  this->userInputStringIfAvailable =
  FileOperations::cleanUpForFileNameUse(inputUserStringCivilized);
  if (!global.flagUsingSSLinCurrentConnection) {
    this->userInputStringRAWIfAvailable = inputUserStringRAW;
    inputAbbreviated = this->userInputStringRAWIfAvailable;
  } else {
    this->userInputStringRAWIfAvailable =
    "Raw user input string not available in SSL mode. ";
    inputAbbreviated = this->userInputStringIfAvailable;
  }
  StringRoutines::stringTrimToLengthWithHash(inputAbbreviated, 150);
  this->relativePhysicalNameCrashReport =
  "crash_" + inputAbbreviated + ".html";
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
  STACK_TRACE("UserCalculatorData::reset");
  for (unsigned i = 0; i < this->username.size(); i ++) {
    this->username[i] = '*';
  }
  this->username = "";
  this->email = "";
  this->clearAuthenticationTokenAndPassword();
}

void UserCalculatorData::clearPasswordFromMemory() {
  STACK_TRACE("UserCalculatorData::clearPasswordFromMemory");
  for (
    unsigned i = 0; i < this->actualHashedSaltedPassword.size(); i ++
  ) {
    this->actualHashedSaltedPassword[i] = ' ';
  }
  this->actualHashedSaltedPassword = "";
  for (unsigned i = 0; i < this->enteredPassword.size(); i ++) {
    this->enteredPassword[i] = ' ';
  }
  this->enteredPassword = "";
  for (
    unsigned i = 0; i < this->enteredHashedSaltedPassword.size(); i ++
  ) {
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
  STACK_TRACE("UserCalculatorData::clearAuthenticationTokenAndPassword");
  this->clearPasswordFromMemory();
  for (
    unsigned i = 0; i < this->actualAuthenticationToken.size(); i ++
  ) {
    this->actualAuthenticationToken[i] = ' ';
  }
  this->actualAuthenticationToken = "";
}

std::string UserCalculatorData::toStringCourseInfo() {
  std::stringstream out;
  out
  << "Course name:\n"
  << this->courseComputed
  << "\n<br>Deadline schema:\n"
  << this->deadlines.toString(nullptr)
  << ". "
  << "\n<hr>Problem weight schema:\n"
  << this->problemWeights.toString(nullptr)
  << ". ";
  return out.str();
}

std::string UserCalculatorData::toStringSecure() {
  STACK_TRACE("UserCalculatorData::toStringSecure");
  std::stringstream out;
  out
  << "User: [["
  << this->username
  << "]]"
  << ", email: [["
  << this->email
  << "]]";
  return out.str();
}

std::string UserCalculatorData::toStringUnsecure() {
  STACK_TRACE("UserCalculatorData::toStringUnsecure");
  std::stringstream out;
  out
  << "User: "
  << this->username
  << "\n<br>"
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

template < >
List<Weight<RationalFraction<Rational> > >::Comparator* FormatExpressions::
getMonomialOrder<Weight<RationalFraction<Rational> > >() {
  return nullptr;
}

template < >
List<Weight<Rational> >::Comparator* FormatExpressions::getMonomialOrder<
  Weight<Rational>
>() {
  return nullptr;
}

void DynkinDiagramRootSubalgebra::swapDynkinStrings(int i, int j) {
  this->simpleComponentTypes.swapTwoIndices(i, j);
  this->simpleBasesConnectedComponents.swapTwoIndices(i, j);
  this->indicesThreeNodes.swapTwoIndices(i, j);
  this->indicesEnds.swapTwoIndices(i, j);
}

void DynkinDiagramRootSubalgebra::sort() {
  // doing bubble sort -> shortest to code
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    for (
      int j = i + 1; j < this->simpleBasesConnectedComponents.size; j ++
    ) {
      bool tempBool = false;
      if (
        this->simpleBasesConnectedComponents[i].size < this->
        simpleBasesConnectedComponents[j].size
      ) {
        tempBool = true;
      }
      if (
        this->simpleBasesConnectedComponents[i].size ==
        this->simpleBasesConnectedComponents[j].size
      ) {
        tempBool = ((this->simpleComponentTypes[i]) < (
            this->simpleComponentTypes[j]
          )
        );
      }
      if (tempBool) {
        this->swapDynkinStrings(i, j);
      }
    }
  }
  this->sameTypeComponents.size = 0;
  this->indexInUniComponent.setSize(
    this->simpleBasesConnectedComponents.size
  );
  this->indexUniComponent.setSize(
    this->simpleBasesConnectedComponents.size
  );
  this->sameTypeComponents.reserve(
    this->simpleBasesConnectedComponents.size
  );
  DynkinSimpleType tempType;
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    if (!(this->simpleComponentTypes[i] == tempType)) {
      this->sameTypeComponents.setSize(this->sameTypeComponents.size + 1);
      this->sameTypeComponents.lastObject()->size = 0;
      tempType = this->simpleComponentTypes[i];
    }
    this->sameTypeComponents.lastObject()->addOnTop(i);
    this->indexUniComponent[i] = this->sameTypeComponents.size - 1;
    this->indexInUniComponent[i] =
    this->sameTypeComponents.lastObject()->size - 1;
  }
}

Rational DynkinDiagramRootSubalgebra::getSquareLengthLongestRootLinkedTo(
  const Vector<Rational>& inputVector
) {
  STACK_TRACE(
    "DynkinDiagramRootSubalgebra::getSquareLengthLongestRootLinkedTo"
  );
  Rational result = 0;
  for (int i = 0; i < this->ambientRootSystem.size; i ++) {
    if (
      inputVector.scalarProduct(
        this->ambientRootSystem[i], this->ambientBilinearForm
      ) !=
      0
    ) {
      Rational squareLength =
      this->ambientRootSystem[i].scalarProduct(
        this->ambientRootSystem[i], this->ambientBilinearForm
      );
      if (result < squareLength) {
        result = squareLength;
      }
    }
  }
  return result;
}

Rational DynkinDiagramRootSubalgebra::getSquareLengthShortestRootLinkedTo(
  const Vector<Rational>& inputVector
) {
  STACK_TRACE(
    "DynkinDiagramRootSubalgebra::getSquareLengthLongestRootLinkedTo"
  );
  Rational result =
  inputVector.scalarProduct(inputVector, this->ambientBilinearForm);
  for (int i = 0; i < this->ambientRootSystem.size; i ++) {
    if (
      inputVector.scalarProduct(
        this->ambientRootSystem[i], this->ambientBilinearForm
      ) !=
      0
    ) {
      Rational squareLength =
      this->ambientRootSystem[i].scalarProduct(
        this->ambientRootSystem[i], this->ambientBilinearForm
      );
      if (squareLength < result) {
        result = squareLength;
      }
    }
  }
  return result;
}

void DynkinDiagramRootSubalgebra::computeDynkinString(int indexComponent) {
  STACK_TRACE("DynkinDiagramRootSubalgebra::computeDynkinString");
  this->checkInitialization();
  if (indexComponent >= this->simpleBasesConnectedComponents.size) {
    global.fatal << "Bad Dynkin index. " << global.fatal;
  }
  DynkinSimpleType& outputType = this->simpleComponentTypes[indexComponent];
  Vectors<Rational>& currentComponent =
  this->simpleBasesConnectedComponents[indexComponent];
  List<int>& currentEnds = this->indicesEnds[indexComponent];
  if (currentComponent.size < 1) {
    global.fatal
    << "CurrentComponent is empty which is impossible. "
    << global.fatal;
  }
  if (this->numberOfThreeValencyNodes(indexComponent) == 1) {
    // type D or E
    // in type D first comes the triple node, then the long string, then the
    // one-root strings
    // the long string is oriented with the end that is connected to the triple
    // node having
    // smaller index
    // in type E similarly the longest string comes first oriented with the
    // root
    // that is
    // linked to the triple node having smaller index
    // then comes the second longest string (oriented in the same fashion)
    // and last the one-root string
    Vector<Rational> tripleNode;
    tripleNode = currentComponent[this->indicesThreeNodes[indexComponent]];
    Vectors<Rational> rootsWithoutTripleNode = currentComponent;
    rootsWithoutTripleNode.removeIndexSwapWithLast(
      this->indicesThreeNodes[indexComponent]
    );
    DynkinDiagramRootSubalgebra diagramWithoutTripleNode;
    diagramWithoutTripleNode.ambientBilinearForm = this->ambientBilinearForm;
    diagramWithoutTripleNode.ambientRootSystem = this->ambientRootSystem;
    diagramWithoutTripleNode.computeDiagramInputIsSimple(
      rootsWithoutTripleNode
    );
    if (diagramWithoutTripleNode.simpleBasesConnectedComponents.size != 3) {
      global.fatal
      << "Dynkin diagram has a triple "
      << "node whose removal does not yield 3 connected components. "
      << global.fatal;
    }
    for (int i = 0; i < 3; i ++) {
      if (
        diagramWithoutTripleNode.simpleBasesConnectedComponents[i][0].
        scalarProduct(tripleNode, this->ambientBilinearForm) ==
        0
      ) {
        diagramWithoutTripleNode.simpleBasesConnectedComponents[i].
        reverseElements();
      }
    }
    for (int i = 0; i < 3; i ++) {
      for (int j = i + 1; j < 3; j ++) {
        if (
          diagramWithoutTripleNode.simpleBasesConnectedComponents[i].size <
          diagramWithoutTripleNode.simpleBasesConnectedComponents[j].size
        ) {
          diagramWithoutTripleNode.swapDynkinStrings(i, j);
        }
      }
    }
    currentComponent.setSize(0);
    if (
      diagramWithoutTripleNode.simpleBasesConnectedComponents[1].size == 1
    ) {
      // <- components are sorted by length, therefore the second and third
      // component are of length 1,
      // therefore we have type D_n
      Rational scale = DynkinSimpleType::getDefaultLongRootLengthSquared('D') /
      tripleNode.scalarProduct(tripleNode, this->ambientBilinearForm);
      currentComponent.addListOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[0]
      );
      // <-first long component
      if (
        !tripleNode.scalarProduct(
          currentComponent[0], this->ambientBilinearForm
        ).isEqualToZero()
      ) {
        currentComponent.reverseElements();
      }
      currentComponent.addOnTop(tripleNode);
      // <-then triple node
      currentComponent.addListOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[1]
      );
      // <-last two vectors
      currentComponent.addListOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[2]
      );
      // <-last two vectors
      outputType.makeArbitrary('D', currentComponent.size, scale);
    } else {
      // the second largest component has more than one element, hence we are
      // in
      // type E_n.
      Rational scale = DynkinSimpleType::getDefaultLongRootLengthSquared('E') /
      tripleNode.scalarProduct(tripleNode, this->ambientBilinearForm);
      if (
        diagramWithoutTripleNode.simpleBasesConnectedComponents[1].size != 2
      ) {
        global.fatal
        << "The Dynkin diagram has two components of "
        << "length larger than 2 linked to the triple node."
        << global.fatal;
      }
      if (
        !tripleNode.scalarProduct(
          diagramWithoutTripleNode.simpleBasesConnectedComponents[1][0],
          this->ambientBilinearForm
        ).isEqualToZero()
      ) {
        diagramWithoutTripleNode.simpleBasesConnectedComponents[1].
        reverseElements();
        // <-the 2-root component has the first root perpendicular to the
        // triple
        // node
      }
      if (
        tripleNode.scalarProduct(
          diagramWithoutTripleNode.simpleBasesConnectedComponents[0][0],
          this->ambientBilinearForm
        ).isEqualToZero()
      ) {
        diagramWithoutTripleNode.simpleBasesConnectedComponents[0].
        reverseElements();
        // <-the largest component has the first root non-perpendicular to the
        // triple node
      }
      currentComponent.addOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[1][0]
      );
      // <-first root from 2-element component
      currentComponent.addOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[2][0]
      );
      // <-then the small sticky part of the Dynkin diagram
      currentComponent.addOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[1][1]
      );
      // <-next the second root from 2-element component
      currentComponent.addOnTop(tripleNode);
      // <- next the triple node
      currentComponent.addListOnTop(
        diagramWithoutTripleNode.simpleBasesConnectedComponents[0]
      );
      // <-finally the longest component. Conventions, conventions...
      outputType.makeArbitrary('E', currentComponent.size, scale);
    }
    return;
  }
  Rational length1, length2;
  length1 =
  currentComponent[0].scalarProduct(
    currentComponent[0], this->ambientBilinearForm
  );
  int numLength1 = 1;
  int numLength2 = 0;
  for (int i = 1; i < currentComponent.size; i ++) {
    if (
      currentComponent[i].scalarProduct(
        currentComponent[i], this->ambientBilinearForm
      ) ==
      length1
    ) {
      numLength1 ++;
    } else {
      numLength2 ++;
      length2 =
      currentComponent[i].scalarProduct(
        currentComponent[i], this->ambientBilinearForm
      );
    }
  }
  if (numLength2 == 0) {
    // type A
    outputType.makeArbitrary(
      'A',
      numLength1,
      DynkinSimpleType::getDefaultLongRootLengthSquared('A') / length1
    );
  } else {
    if (length1 < length2) {
      MathRoutines::swap(length1, length2);
      MathRoutines::swap(numLength1, numLength2);
      currentEnds.swapTwoIndices(0, 1);
    }
    // <-so far we made sure the first length is long
    // By convention, in types G and C, in the Dynkin diagram the long root
    // comes last
    // This is handled at the very end of this function (outside all the if
    // clauses).
    if (numLength1 == numLength2) {
      // B2, C2, F4 or G2
      if (numLength1 == 2) {
        outputType.makeArbitrary(
          'F',
          4,
          DynkinSimpleType::getDefaultLongRootLengthSquared('F') / length1
        );
      } else if (length1 / length2 == 3) {
        outputType.makeArbitrary(
          'G',
          2,
          DynkinSimpleType::getDefaultLongRootLengthSquared('G') / length1
        );
      } else {
        outputType.makeArbitrary(
          'B',
          2,
          DynkinSimpleType::getDefaultLongRootLengthSquared('B') / length1
        );
      }
    } else {
      if (numLength1 > numLength2) {
        outputType.makeArbitrary(
          'B',
          currentComponent.size,
          DynkinSimpleType::getDefaultLongRootLengthSquared('B') / length1
        );
      } else {
        outputType.makeArbitrary(
          'C',
          currentComponent.size,
          DynkinSimpleType::getDefaultLongRootLengthSquared('C') / length1
        );
      }
    }
  }
  // The following code ensures the Dynkin diagram is properly ordered
  currentComponent.swapTwoIndices(0, currentEnds[0]);
  for (int i = 0; i < currentComponent.size; i ++) {
    for (int j = i + 1; j < currentComponent.size; j ++) {
      if (
        !currentComponent[i].scalarProduct(
          currentComponent[j], this->ambientBilinearForm
        ).isEqualToZero()
      ) {
        currentComponent.swapTwoIndices(i + 1, j);
        break;
      }
    }
  }
  // so far we made sure the entire component is one properly ordered string,
  // starting with the long root.
  if (outputType.letter == 'G' || outputType.letter == 'C') {
    currentComponent.reverseElements();
    // <-in G_2 and C_n the short root comes first so we need to reverse
    // elements.
  }
}

std::string DynkinDiagramRootSubalgebra::toString(FormatExpressions* format)
const {
  DynkinType dynkinType;
  dynkinType.makeZero();
  for (int j = 0; j < this->simpleComponentTypes.size; j ++) {
    dynkinType.addMonomial(this->simpleComponentTypes[j], 1);
  }
  return dynkinType.toString(format);
}

bool DynkinDiagramRootSubalgebra::checkInitialization() const {
  STACK_TRACE("DynkinDiagramRootSubalgebra::checkInitialization");
  if (this->ambientRootSystem.size != 0) {
    if (
      this->ambientBilinearForm.numberOfRows != this->ambientRootSystem[0].size
    ) {
      global.fatal
      << "Ambient bilinear form of Dynkin subdiagram not initialized. "
      << global.fatal;
    }
  }
  return true;
}

void DynkinDiagramRootSubalgebra::computeDiagramInputIsSimple(
  const Vectors<Rational>& simpleBasisInput
) {
  STACK_TRACE("DynkinDiagramRootSubalgebra::computeDiagramInputIsSimple");
  this->checkInitialization();
  this->simpleBasesConnectedComponents.size = 0;
  this->simpleBasesConnectedComponents.reserve(simpleBasisInput.size);
  for (int i = 0; i < simpleBasisInput.size; i ++) {
    int indexFirstComponentConnectedToRoot = - 1;
    for (int j = 0; j < this->simpleBasesConnectedComponents.size; j ++) {
      if (
        this->simpleBasesConnectedComponents[j].
        containsVectorNonPerpendicularTo(
          simpleBasisInput[i], this->ambientBilinearForm
        )
      ) {
        if (indexFirstComponentConnectedToRoot == - 1) {
          indexFirstComponentConnectedToRoot = j;
          this->simpleBasesConnectedComponents[j].addOnTop(
            simpleBasisInput[i]
          );
        } else {
          this->simpleBasesConnectedComponents[
            indexFirstComponentConnectedToRoot
          ].addListOnTop(this->simpleBasesConnectedComponents[j]);
          this->simpleBasesConnectedComponents.removeIndexSwapWithLast(j);
          j --;
        }
      }
    }
    if (indexFirstComponentConnectedToRoot == - 1) {
      this->simpleBasesConnectedComponents.setSize(
        this->simpleBasesConnectedComponents.size + 1
      );
      this->simpleBasesConnectedComponents.lastObject()->size = 0;
      this->simpleBasesConnectedComponents.lastObject()->addOnTop(
        simpleBasisInput[i]
      );
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
    << simpleBasisInput.toString()
    << global.fatal;
  }
}

bool DynkinDiagramRootSubalgebra::letterIsDynkinGreaterThanLetter(
  char letter1, char letter2
) {
  if ((letter1 == 'B' || letter1 == 'D') && (letter2 == 'B' || letter2 == 'D')
  ) {
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

bool DynkinDiagramRootSubalgebra::isGreaterThan(
  DynkinDiagramRootSubalgebra& right
) {
  if (this->rankTotal() > right.rankTotal()) {
    return true;
  }
  if (this->rankTotal() < right.rankTotal()) {
    return false;
  }
  if (
    this->simpleComponentTypes.size !=
    this->simpleBasesConnectedComponents.size
  ) {
    global.fatal
    << "Simple component types do "
    << "not match number of connected components. "
    << global.fatal;
  }
  for (int i = 0; i < this->simpleComponentTypes.size; i ++) {
    if (
      this->simpleBasesConnectedComponents[i].size >
      right.simpleBasesConnectedComponents[i].size
    ) {
      return true;
    }
    if (
      right.simpleBasesConnectedComponents[i].size >
      this->simpleBasesConnectedComponents[i].size
    ) {
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

Rational DynkinDiagramRootSubalgebra::getSizeCorrespondingWeylGroupByFormula()
{
  Rational output = 1;
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    output *=
    WeylGroupData::sizeByFormulaOrNegative1(
      this->simpleComponentTypes[i].letter,
      this->simpleComponentTypes[i].rank
    );
  }
  return output;
}

void DynkinDiagramRootSubalgebra::getMapFromPermutation(
  Vectors<Rational>& domain,
  Vectors<Rational>& range,
  List<int>& permutation,
  List<List<List<int> > >& automorphisms,
  SelectionWithDifferentMaxMultiplicities& autosPermutations,
  DynkinDiagramRootSubalgebra& right
) {
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    for (
      int j = 0; j < this->simpleBasesConnectedComponents[i].size; j ++
    ) {
      if (
        this->simpleBasesConnectedComponents[i].size !=
        right.simpleBasesConnectedComponents[permutation[i]].size
      ) {
        global.fatal
        << "Connected components simple bases sizes do not match. "
        << global.fatal;
      }
      domain.addOnTop(this->simpleBasesConnectedComponents[i][j]);
      int indexTargetComponent = permutation[i];
      int indexAutomorphismInComponent = autosPermutations.multiplicities[i];
      int indexRoot = automorphisms[i][indexAutomorphismInComponent][j];
      range.addOnTop(
        right.simpleBasesConnectedComponents[indexTargetComponent][indexRoot]
      );
    }
  }
}

void DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInput(
  Vectors<Rational>& inputRoots, WeylGroupData& weylGroup
) {
  STACK_TRACE("DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInput");
  this->ambientRootSystem = weylGroup.rootSystem;
  this->ambientBilinearForm = weylGroup.cartanSymmetric;
  weylGroup.transformToSimpleBasisGenerators(
    inputRoots, weylGroup.rootSystem
  );
  this->computeDiagramInputIsSimple(inputRoots);
}

void DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInputRelative(
  Vectors<Rational>& inputOutputSimpleWeightSystem,
  const HashedList<Vector<Rational> >& weightSystem,
  const Matrix<Rational>& bilinearForm
) {
  STACK_TRACE(
    "DynkinDiagramRootSubalgebra::computeDiagramTypeModifyInputRelative"
  );
  this->ambientRootSystem = weightSystem;
  this->ambientBilinearForm = bilinearForm;
  WeylGroupData::transformToSimpleBasisGeneratorsArbitraryCoordinates(
    inputOutputSimpleWeightSystem, weightSystem
  );
  this->computeDiagramInputIsSimple(inputOutputSimpleWeightSystem);
}

void DynkinDiagramRootSubalgebra::computeDynkinStrings() {
  STACK_TRACE("DynkinDiagramRootSubalgebra::computeDynkinStrings");
  this->indicesThreeNodes.setSize(
    this->simpleBasesConnectedComponents.size
  );
  this->simpleComponentTypes.setSize(
    this->simpleBasesConnectedComponents.size
  );
  this->indicesEnds.setSize(this->simpleBasesConnectedComponents.size);
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    this->computeDynkinString(i);
  }
}

bool DynkinDiagramRootSubalgebra::operator==(
  const DynkinDiagramRootSubalgebra& right
) const {
  if (
    right.simpleBasesConnectedComponents.size !=
    this->simpleBasesConnectedComponents.size
  ) {
    return false;
  }
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    bool tempBool = ((
        this->simpleBasesConnectedComponents[i].size ==
        right.simpleBasesConnectedComponents[i].size
      ) && (
        this->simpleComponentTypes[i] == right.simpleComponentTypes[i]
      )
    );
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

void DynkinDiagramRootSubalgebra::getAutomorphism(
  List<List<int> >& output, int index
) {
  Vectors<Rational>& currentComponent =
  this->simpleBasesConnectedComponents[index];
  DynkinSimpleType& currentStrinG = this->simpleComponentTypes[index];
  List<int> permutation;
  permutation.setSize(currentComponent.size);
  output.size = 0;
  for (int i = 0; i < currentComponent.size; i ++) {
    permutation[i] = i;
  }
  output.addOnTop(permutation);
  if (currentStrinG.letter == 'A' && currentComponent.size != 1) {
    permutation.reverseElements();
    output.addOnTop(permutation);
  }
  if (currentStrinG.letter == 'D') {
    if (currentComponent.size == 4) {
      // the automorphism group of the Dynkin Diagram is S3
      permutation[1] = 2;
      permutation[2] = 3;
      permutation[3] = 1;
      output.addOnTop(permutation);
      permutation[1] = 1;
      permutation[2] = 3;
      permutation[3] = 2;
      output.addOnTop(permutation);
      permutation[1] = 2;
      permutation[2] = 1;
      permutation[3] = 3;
      output.addOnTop(permutation);
      permutation[1] = 3;
      permutation[2] = 1;
      permutation[3] = 2;
      output.addOnTop(permutation);
      permutation[1] = 3;
      permutation[2] = 2;
      permutation[3] = 1;
      output.addOnTop(permutation);
    } else {
      permutation[currentComponent.size - 2] = currentComponent.size - 1;
      permutation[currentComponent.size - 1] = currentComponent.size - 2;
      output.addOnTop(permutation);
    }
  }
  if (currentStrinG.letter == 'E' && currentStrinG.rank == 6) {
    permutation[1] = 3;
    permutation[2] = 4;
    permutation[3] = 1;
    permutation[4] = 2;
    output.addOnTop(permutation);
  }
}

void DynkinDiagramRootSubalgebra::getAutomorphisms(
  List<List<List<int> > >& output
) {
  output.setSize(this->simpleBasesConnectedComponents.size);
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    this->getAutomorphism(output[i], i);
  }
}

int DynkinDiagramRootSubalgebra::rankTotal() {
  int result = 0;
  for (int i = 0; i < this->simpleBasesConnectedComponents.size; i ++) {
    result += this->simpleBasesConnectedComponents[i].size;
  }
  return result;
}

int DynkinDiagramRootSubalgebra::numberRootsGeneratedByDiagram() {
  int result = 0;
  if (
    this->simpleBasesConnectedComponents.size !=
    this->simpleComponentTypes.size
  ) {
    global.fatal
    << "Number of simple connected components "
    << "does not match the number of types. "
    << global.fatal;
  }
  for (int i = 0; i < this->simpleComponentTypes.size; i ++) {
    int rank = this->simpleBasesConnectedComponents[i].size;
    if (this->simpleComponentTypes[i].letter == 'A') {
      result += rank *(rank + 1);
    }
    if (
      this->simpleComponentTypes[i].letter == 'B' ||
      this->simpleComponentTypes[i].letter == 'C'
    ) {
      result += rank * rank * 2;
    }
    if (this->simpleComponentTypes[i].letter == 'D') {
      result += rank *(rank - 1) * 2;
    }
    if (this->simpleComponentTypes[i].letter == 'E') {
      if (rank == 6) {
        result += 72;
      }
      if (rank == 7) {
        result += 126;
      }
      if (rank == 8) {
        result += 240;
      }
    }
    if (this->simpleComponentTypes[i].letter == 'F') {
      result += 48;
    }
    if (this->simpleComponentTypes[i].letter == 'G') {
      result += 12;
    }
  }
  return result;
}

int DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes(int indexComponent)
{
  STACK_TRACE("DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes");
  Vectors<Rational>& currentComponent =
  this->simpleBasesConnectedComponents[indexComponent];
  int numEnds = 0;
  int result = 0;
  this->indicesThreeNodes[indexComponent] = - 1;
  this->indicesEnds[indexComponent].size = 0;
  for (int i = 0; i < currentComponent.size; i ++) {
    int counter = 0;
    for (int j = 0; j < currentComponent.size; j ++) {
      if (
        currentComponent[i].scalarProduct(
          currentComponent[j], this->ambientBilinearForm
        ).isNegative()
      ) {
        counter ++;
      }
    }
    if (counter > 3) {
      Matrix<Rational> gramMatrix;
      currentComponent.getGramMatrix(
        gramMatrix, &this->ambientBilinearForm
      );
      global.fatal
      << "Corrupt simple basis corresponding to "
      << "Dynkin diagram: the Dynkin diagram should have nodes with "
      << "valency at most 3, but this diagram has node with valency "
      << counter
      << ". The current component is: "
      << currentComponent.toString()
      << ". The corresponding Symmetric Cartan is: "
      << gramMatrix.toString()
      << ". "
      << global.fatal;
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
    global.fatal
    << "numEnds variable equals: "
    << numEnds
    << ", number of three-nodes equals: "
    << result
    << "; this should not happen. The bilinear form is: "
    << this->ambientBilinearForm.toString()
    << global.fatal;
  }
  if (result == 1) {
    if (numEnds != 3) {
      global.fatal
      << "numEnds variable equals: "
      << numEnds
      << ", number of three-nodes equals: "
      << result
      << "; this should not happen. The bilinear form is: "
      << this->ambientBilinearForm.toString()
      << global.fatal;
    }
  } else {
    if (numEnds > 2) {
      global.fatal
      << "numEnds variable equals: "
      << numEnds
      << ", number of three-nodes equals: "
      << result
      << "; this should not happen. The bilinear form is: "
      << this->ambientBilinearForm.toString()
      << global.fatal;
    }
  }
  return result;
}

bool AffineCone::splitByAffineHyperplane(
  AffineHyperplane<Rational>& killerPlane, AffineCones& output
) {
  (void) killerPlane;
  (void) output;
  return true;
}

bool AffineCone::wallIsInternalInCone(
  AffineHyperplane<Rational>& killerCandidate
) {
  (void) killerCandidate;
  return true;
}

int AffineCone::getDimension() {
  if (this->walls.size == 0) {
    return 0;
  }
  return this->walls.objects[0].affinePoint.size;
}

unsigned int AffineCone::hashFunction() const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->walls.size; i ++) {
    result +=
    this->walls[i].hashFunction() *
    HashConstants::getConstantIncrementCounter(j);
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
  this->initializePart1(n);
  for (int i = 0; i < n; i ++) {
    this->capacities[i] = n - i - 1;
    this->multiplicities[i] = 0;
  }
}

void Permutation::initPermutation(
  List<int>& disjointSubsets, int totalNumberOfElements
) {
  this->initializePart1(totalNumberOfElements);
  int counter = 0;
  for (int i = 0; i < disjointSubsets.size; i ++) {
    for (int j = 0; j < disjointSubsets[i]; j ++) {
      this->capacities[counter] = disjointSubsets[i] - j - 1;
      this->multiplicities[counter] = 0;
      counter ++;
    }
    totalNumberOfElements -= disjointSubsets[i];
  }
  if (totalNumberOfElements != 0) {
    global.fatal
    << "Permutations with 0 elements not allowed. "
    << global.fatal;
  }
}

void Permutation::incrementAndGetPermutation(List<int>& output) {
  this->incrementReturnFalseIfPastLast();
  this->getPermutationLthElementIsTheImageofLthIndex(output);
}

void Permutation::getPermutationLthElementIsTheImageofLthIndex(
  List<int>& output
) {
  int numberOfElements = this->multiplicities.size;
  output.setSize(numberOfElements);
  for (int i = 0; i < numberOfElements; i ++) {
    output[i] = i;
  }
  for (int i = 0; i < numberOfElements; i ++) {
    MathRoutines::swap(
      output[i], output[i + this->multiplicities[i]]
    );
  }
}

bool WeylGroupData::areMaximallyDominantGroupInner(
  List<Vector<Rational> >& weights
) {
  STACK_TRACE("WeylGroupData::areMaximallyDominantGroupInner");
  for (int i = 0; i < weights.size; i ++) {
    for (int j = 0; j < this->rootsOfBorel.size; j ++) {
      if (
        this->rootScalarCartanRoot(this->rootsOfBorel[j], weights[i]) < 0
      ) {
        bool reflectionDoesRaise = true;
        for (int k = 0; k < i; k ++) {
          if (
            this->rootScalarCartanRoot(
              this->rootsOfBorel[j], weights[k]
            ) >
            0
          ) {
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
    global.fatal
    << "Use after free of Weyl group automorphism. "
    << global.fatal;
  }
  if (this->weylGroup == nullptr) {
    global.fatal
    << "Non-initialized Weyl group automorphisms. "
    << global.fatal;
  }
  return true;
}

bool WeylGroupAutomorphisms::areMaximallyDominantGroupOuter(
  List<Vector<Rational> >& weights
) {
  STACK_TRACE("WeylGroupAutomorphisms::areMaximallyDominantGroupOuter");
  this->checkInitialization();
  MemorySaving<Vectors<Rational> > weightsCopy;
  Vector<Rational> zeroWeight;
  this->computeOuterAutomorphisms();
  zeroWeight.makeZero(this->weylGroup->getDimension());
  for (int i = 0; i < weights.size; i ++) {
    for (int j = 0; j < this->weylGroup->rootsOfBorel.size; j ++) {
      if (
        this->weylGroup->rootScalarCartanRoot(
          this->weylGroup->rootsOfBorel[j], weights[i]
        ) <
        0
      ) {
        bool reflectionDoesRaise = true;
        for (int k = 0; k < i; k ++) {
          if (
            this->weylGroup->rootScalarCartanRoot(
              this->weylGroup->rootsOfBorel[j], weights[k]
            ) >
            0
          ) {
            reflectionDoesRaise = false;
            break;
          }
        }
        if (reflectionDoesRaise) {
          return false;
        }
      }
    }
    for (int j = 0; j < this->outerAutomorphisms.elements.size; j ++) {
      weightsCopy.getElement() = weights;
      this->outerAutomorphisms.elements[j].actOnVectorsColumn(
        weightsCopy.getElement()
      );
      bool isGood = true;
      for (int k = 0; k < i; k ++) {
        if (
          !(weightsCopy.getElement()[k] - weights[k]).isPositiveOrZero()
        ) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      if (
        !(weightsCopy.getElement()[i] - weights[i]).isGreaterThanLexicographic(
          zeroWeight
        )
      ) {
        continue;
      }
      return false;
    }
  }
  return true;
}

void WeylGroupData::generateRootSubsystem(Vectors<Rational>& roots) {
  Vector<Rational> root;
  int oldsize = roots.size;
  for (int i = 0; i < oldsize; i ++) {
    roots.addOnTopNoRepetition(- roots[i]);
  }
  for (int i = 0; i < roots.size; i ++) {
    for (int j = 0; j < roots.size; j ++) {
      root = roots[i] + roots[j];
      if (this->isARoot(root)) {
        roots.addOnTopNoRepetition(root);
      }
    }
  }
}

void GeneralizedVermaModuleCharacters::computeQPsFromChamberComplex() {
  std::stringstream out;
  FormatExpressions format;
  Vector<Rational> root;
  FileOperations::openFileCreateIfNotPresentVirtual(
    this->multiplicitiesMaxOutputReport2,
    "output/ExtremaPolys.txt",
    false,
    true,
    false
  );
  this->partialFractions.initializeFromVectors(
    this->gModKNegativeWeightsBasisChanged, nullptr
  );
  out << this->partialFractions.toString(&format);
  this->partialFractions.split(nullptr);
  out << "=" << this->partialFractions.toString(&format);
  // int totalDim = this->translationS[0].size
  // +this->translationsProjecteD[0].size;
  this->quasiPolynomialsSubstituted.setSize(
    this->projectivizedChamber.nonRefinedCones.size()
  );
  global.fatal
  << "not implemented fully, crashing to let you know. "
  << global.fatal;
  //  this->pfs.chambersOld.initialize();
  //  this->pfs.chambersOld.directions = this->GmodKNegWeightsBasisChanged;
  //  this->pfs.chambersOld.SliceEuclideanSpace(false);
  //  this->qPsNonSubstituted.setSize(this->pfs.chambersOld.size);
  //  this->qPsSubstituted.setSize(this->pfs.chambersOld.size);
  out << "\n\nThe vector partition functions in each chamber follow.";
  global.fatal << "Not implemented yet. " << global.fatal;
  /*
  for (int i = 0; i < this->pfs.chambersOld.size; i ++)
    if (this->pfs.chambersOld.objects[i] != 0) {
      QuasiPolynomial& currentQPNoSub = this->qPsNonSubstituted.objects[i];
      this->qPsSubstituted.objects[i].setSize(this->linearOperators.size);
      this->pfs.getVectorPartitionFunction(currentQPNoSub, this->pfs.chambersOld.objects[i]->InternalPoint);
      out << "\nChamber " << i + 1 << " with internal point " << this->pfs.chambersOld.objects[i]->InternalPoint.toString() << " the quasipoly is: " << currentQPNoSub.toString(false, false);
      for (int k = 0; k< this->linearOperators.size; k++) {
        QuasiPolynomial& currentQPSub = this->qPsSubstituted.objects[i].objects[k];
        std::stringstream tempStream;
        tempStream << "Processing chamber " << i + 1 << " linear operator " << k+ 1;
        global.indicatorVariables.ProgressReportStrings[0] = tempStream.str();
        global.makeReport();
        currentQPNoSub.substitution(this->linearOperatorsExtended.objects[k], this->translationsProjectedBasisChanged[k], this->ExtendedIntegralLatticeMatForM, currentQPSub);
        out << "; after substitution we get: " << currentQPSub.toString(false, false);
      }
    }
    */
  // out << "\nThe integral lattice:\n" << integralLattice.toString(false,
  // false);
  // this->multiplicitiesMaxOutputReport2.flush();
  this->projectivizedChamber.checkIsRefinedOrCrash();
  QuasiPolynomial tempQP;
  this->multiplicities.setSize(
    this->projectivizedChamber.refinedCones.size()
  );
  this->numberNonZeroMultiplicities = 0;
  ProgressReport report;
  ProgressReport report2;
  for (
    int i = 0; i < this->projectivizedChamber.refinedCones.size(); i ++
  ) {
    QuasiPolynomial& currentSum = this->multiplicities.objects[i];
    currentSum.makeZeroOverLattice(this->extendedIntegralLatticeMatrixForm);
    for (int k = 0; k < this->linearOperators.size; k ++) {
      this->getProjection(
        k,
        this->projectivizedChamber.refinedCones[i].internalPoint(),
        root
      );
      root -= this->nonIntegralOriginModificationBasisChanged;
      global.fatal << global.fatal;
      int index = - 1;
      // = this->pfs.chambersOld.GetFirstChamberIndexContainingPoint(root);
      if (index != - 1) {
        tempQP = this->quasiPolynomialsSubstituted[index][k];
        tempQP *= this->coefficients[k];
        currentSum += tempQP;
      }
      std::stringstream tempStream;
      tempStream << " Chamber " << i + 1 << " translation " << k + 1;
      report.report(tempStream.str());
    }
    if (!currentSum.isEqualToZero()) {
      this->numberNonZeroMultiplicities ++;
    }
    std::stringstream tempStream;
    tempStream
    << " So far "
    << i + 1
    << " out of "
    << this->projectivizedChamber.refinedCones.size()
    << " processed "
    << this->numberNonZeroMultiplicities
    << " non-zero total.";
    report2.report(tempStream.str());
    out
    << "\nChamber "
    << i + 1
    << ": the quasipolynomial is: "
    << currentSum.toString();
    out
    << "\nThe chamber is: "
    << this->projectivizedChamber.refinedCones[i].toString(&format);
  }
  //  this->projectivizedChamber.ComputeDebugString();
  //  out << "\n\n" << this->projectivizedChamber.DebugString;
  report.report(out.str());
  this->multiplicitiesMaxOutputReport2 << out.str();
}

std::string GeneralizedVermaModuleCharacters::
computeMultiplicitiesLargerAlgebraHighestWeight(
  Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords,
  Vector<Rational>& parabolicSel
) {
  std::stringstream out;
  WeylGroupData& largeWeylGroup =
  this->homomorphism.coDomainAlgebra().weylGroup;
  WeylGroupData& smallWeylGroup = this->homomorphism.domainAlgebra().weylGroup;
  if (!largeWeylGroup.isOfSimpleType('B', 3)) {
    return "Error: algebra is not so(7).";
  }
  this->initFromHomomorphism(parabolicSel, this->homomorphism);
  this->transformToWeylProjectiveStep1();
  this->transformToWeylProjectiveStep2();
  Vector<Rational> highestWeightLargerAlgSimpleCoords;
  highestWeightLargerAlgSimpleCoords =
  largeWeylGroup.getSimpleCoordinatesFromFundamental(
    highestWeightLargerAlgebraFundamentalCoords
  );
  Vector<Rational> root;
  DrawingVariables drawOps;
  int smallDimension = smallWeylGroup.cartanSymmetric.numberOfRows;
  Vectors<double> draggableBasis;
  draggableBasis.makeEiBasis(smallDimension);
  WeylGroupData tmpWeyl;
  tmpWeyl.makeArbitrarySimple('A', 2);
  drawOps.initializeDimensions(
    tmpWeyl.cartanSymmetric, draggableBasis, draggableBasis
  );
  FormatExpressions format;
  drawOps.basisProjectionPlane[0][0] = 1;
  drawOps.basisProjectionPlane[0][1] = 0;
  drawOps.basisProjectionPlane[1][0] = 1;
  drawOps.basisProjectionPlane[1][1] = 1;
  drawOps.modifyToOrthonormalNoShiftSecond(
    drawOps.basisProjectionPlane[1], drawOps.basisProjectionPlane[0]
  );
  drawOps.graphicsUnit = 50;
  PiecewiseQuasipolynomial
  startingPolynomial,
  substitutedPolynomial,
  accumulator;
  std::string tempS;
  startingPolynomial.makeVPF(this->gModKNegativeWeightsBasisChanged, tempS);
  Vectors<Rational> translationsProjectedFinal;
  translationsProjectedFinal.setSize(this->linearOperators.size);
  this->linearOperators[0].actOnVectorColumn(
    highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[0]
  );
  out
  << "<br>Input so(7)-highest weight: "
  << highestWeightLargerAlgSimpleCoords.toString();
  out << "<br>Input parabolics selections: " << parabolicSel.toString();
  out
  << "<br>the argument translations: "
  << this->translationsProjectedBasisChanged.toString();
  out
  << "<br>Element u_w: projection, "
  << "multiplication by - 1, and basis "
  << "change of so(7)-highest weight to G_2: "
  << translationsProjectedFinal[0].toString();
  startingPolynomial.makeVPF(this->gModKNegativeWeightsBasisChanged, tempS);
  drawOps.drawCoordinateSystemBuffer(2);
  Cone smallWeylChamber;
  Matrix<Rational> invertedCartan;
  invertedCartan = smallWeylGroup.cartanSymmetric;
  invertedCartan.invert();
  Vectors<Rational> tempVertices;
  Vector<Rational> tMpRt;
  tMpRt = this->parabolicSelectionSmallerAlgebra;
  for (
    int i = 0; i < this->parabolicSelectionSmallerAlgebra.numberOfElements; i
    ++
  ) {
    invertedCartan.getVectorFromRow(i, root);
    tempVertices.addOnTop(root);
    if (this->parabolicSelectionSmallerAlgebra.selected[i]) {
      tempVertices.addOnTop(- root);
    }
  }
  smallWeylChamber.createFromVertices(tempVertices);
  Matrix<Rational> basisChange;
  basisChange.initialize(2, 2);
  basisChange.elements[0][0] = 1;
  basisChange.elements[0][1] = 0;
  basisChange.elements[1][0] = 1;
  basisChange.elements[1][1] = 1;
  basisChange.transpose();
  smallWeylChamber.changeBasis(basisChange);
  out << "<br> The small Weyl chamber: " << smallWeylChamber.toString(&format);
  Vector<Rational> highestWeightSmallAlgBasisChanged =
  - translationsProjectedFinal[0];
  for (int i = 0; i < this->linearOperators.size; i ++) {
    this->linearOperators[i].actOnVectorColumn(
      highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[i]
    );
    translationsProjectedFinal[i] +=
    this->translationsProjectedBasisChanged[i];
    drawOps.drawCircleAtVectorBufferRational(
      - translationsProjectedFinal[i], "red", 3
    );
  }
  out
  << "<br>the translations projected final: "
  << translationsProjectedFinal.toString();
  accumulator.makeZero(startingPolynomial.numberOfVariables);
  for (int i = 0; i < this->linearOperators.size; i ++) {
    substitutedPolynomial = startingPolynomial;
    substitutedPolynomial *= this->coefficients[i];
    substitutedPolynomial.translateArgument(translationsProjectedFinal[i]);
    accumulator += substitutedPolynomial;
  }
  accumulator.drawMe(
    drawOps, 10, &smallWeylChamber, &highestWeightSmallAlgBasisChanged
  );
  out << drawOps.getHTMLDiv(2, true);
  out << accumulator.toString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::sortMultiplicities() {
  List<Cone> sortedCones;
  sortedCones = this->projectivizedChamber.refinedCones.values;
  sortedCones.quickSortAscending();
  List<QuasiPolynomial> quasipolynomialList;
  quasipolynomialList.setExpectedSize(this->multiplicities.size);
  for (int i = 0; i < this->multiplicities.size; i ++) {
    int index =
    this->projectivizedChamber.refinedCones.getIndex(sortedCones[i].id);
    quasipolynomialList.addOnTop(this->multiplicities[index]);
  }
  this->multiplicities = quasipolynomialList;
  this->projectivizedChamber.refinedCones.clear();
  for (int i = 0; i < sortedCones.size; i ++) {
    this->projectivizedChamber.refinedCones.setKeyValue(
      sortedCones[i].id, sortedCones[i]
    );
  }
}

std::string GeneralizedVermaModuleCharacters::checkMultiplicitiesVsOrbits() {
  STACK_TRACE("GeneralizedVermaModuleCharacters::checkMultiplicitiesVsOrbits");
  this->checkInitialization();
  std::stringstream out;
  int totalDimensionAffine =
  this->weylLarger->getDimension() + this->weylSmaller->getDimension();
  int smallDimension = this->weylSmaller->getDimension();
  Vector<Rational> normal;
  normal.makeZero(totalDimensionAffine + 1);
  Vectors<Rational> newWalls;
  ConeCollection tempComplex;
  tempComplex = this->projectivizedChamber;
  for (int i = 0; i < this->weylChamberSmallerAlgebra.walls.size; i ++) {
    for (int j = 0; j < smallDimension; j ++) {
      normal[j] = this->weylChamberSmallerAlgebra.walls[i].normal[j];
    }
    newWalls.addOnTop(normal);
    tempComplex.splittingNormals.addOnTop(normal);
  }
  tempComplex.refineByNormals();
  out << "Number chambers with new walls: " << tempComplex.refinedCones.size();
  out << "\n" << tempComplex.toString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::incrementComputation(
  Vector<Rational>& parabolicSel
) {
  std::stringstream out;
  this->parabolicLeviPartRootSpacesZeroStandsForSelected = parabolicSel;
  switch (this->computationPhase) {
  case 0:
    //      this->parser.hmm.MakeG2InB3(this->parser);
    this->initFromHomomorphism(parabolicSel, this->homomorphism);
    this->transformToWeylProjectiveStep1();
    // out <<
    // global.indicatorVariables.StatusString1;
    this->transformToWeylProjectiveStep2();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  case 1:
    this->projectivizedChamber.refineByNormals();
    this->sortMultiplicities();
    out << this->projectivizedChamber.toString();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  case 2:
    this->computeQPsFromChamberComplex();
    out << this->elementToStringMultiplicitiesReport();
    break;
  case 3:
    //      out << this->checkMultiplicitiesVsOrbits();
    break;
  case 4:
    this->inititializeMaximumComputation();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  case 5:
    this->maximumComputation.findExtremaParametricStep1();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  case 6:
    this->maximumComputation.findExtremaParametricStep3();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  case 7:
    this->maximumComputation.findExtremaParametricStep4();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  case 8:
    this->maximumComputation.findExtremaParametricStep5();
    // out <<
    // global.indicatorVariables.StatusString1;
    break;
  default:
    break;
  }
  this->computationPhase ++;
}

GeneralizedVermaModuleCharacters::GeneralizedVermaModuleCharacters() {
  this->UpperLimitChambersForDebugPurposes = - 1;
  this->computationPhase = 0;
  this->NumProcessedConesParam = 0;
  this->NumProcessedExtremaEqualOne = 0;
  this->numberNonZeroMultiplicities = 0;
  this->weylLarger = nullptr;
  this->weylSmaller = nullptr;
}

bool GeneralizedVermaModuleCharacters::checkInitialization() const {
  if (this->weylLarger == nullptr || this->weylSmaller == nullptr) {
    global.fatal
    << "Use of non-initialized Weyl group "
    << "within generalized Verma module characters. "
    << global.fatal;
  }
  if (
    this->weylLarger->flagDeallocated || this->weylSmaller->flagDeallocated
  ) {
    global.fatal
    << "Use after free of Weyl group within Verma module characters. "
    << global.fatal;
  }
  return true;
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism(
  Vector<Rational>& parabolicSelection,
  HomomorphismSemisimpleLieAlgebra& input
) {
  STACK_TRACE("GeneralizedVermaModuleCharacters::initFromHomomorphism");
  Vectors<Rational> roots;
  this->weylLarger = &input.coDomainAlgebra().weylGroup;
  this->weylSmaller = &input.domainAlgebra().weylGroup;
  WeylGroupData& weylGroupCoDomain = input.coDomainAlgebra().weylGroup;
  //  input.projectOntoSmallCartan(weylSmaller.rootsOfBorel, roots);
  this->log << "projections: " << roots.toString();
  weylGroupCoDomain.group.computeAllElements(false);
  this->nonIntegralOriginModificationBasisChanged = "(1/2,1/2)";
  Matrix<Rational> projectionBasisChanged;
  Vector<Rational> startingWeight, projectedWeight;
  FormatExpressions format;
  global.fatal << "Not implemented. " << global.fatal;
  //  SSalgebraModuleOld tempM;
  input.computeHomomorphismFromImagesSimpleChevalleyGenerators(nullptr);
  global.fatal << "Not implemented. " << global.fatal;
  //  tempM.InduceFromEmbedding(tempStream, input);
  input.getWeightsGmodKInSimpleCoordinatesK(this->gModKNegativeWeights);
  // this->log << "weights of g mod k: " <<
  // this->GmodKnegativeWeights.toString();
  //  matrix.actOnVectorsColumn(this->GmodKnegativeWeightS);
  this->log << this->gModKNegativeWeights.toString();
  this->preferredBasis.setSize(2);
  this->preferredBasis[0] = - this->gModKNegativeWeights[1];
  this->preferredBasis[1] = - this->gModKNegativeWeights[2];
  // ///////////////////////////////////////
  // this->preferredBasiS[0] ="(1,0)";
  // this->preferredBasiS[1] ="(0,1)";
  // //////////////////////////////////////
  this->preferredBasisChangE.assignVectorsToRows(this->preferredBasis);
  this->preferredBasisChangE.transpose();
  this->preferredBasisChangeInversE = this->preferredBasisChangE;
  this->preferredBasisChangeInversE.invert();
  this->preferredBasisChangeInversE.actOnVectorsColumn(
    this->gModKNegativeWeights, this->gModKNegativeWeightsBasisChanged
  );
  this->log
  << "\nWeights after basis change: "
  << this->gModKNegativeWeightsBasisChanged.toString();
  for (int i = 0; i < this->gModKNegativeWeights.size; i ++) {
    if (this->gModKNegativeWeights[i].isPositiveOrZero()) {
      this->gModKNegativeWeights.removeIndexSwapWithLast(i);
      i --;
    }
  }
  for (int i = 0; i < this->gModKNegativeWeightsBasisChanged.size; i ++) {
    if (this->gModKNegativeWeightsBasisChanged[i].isPositiveOrZero()) {
      this->gModKNegativeWeightsBasisChanged.removeIndexSwapWithLast(i);
      i --;
    }
  }
  this->log
  << "\nNegative weights after basis change: "
  << this->gModKNegativeWeightsBasisChanged.toString();
  projectionBasisChanged.initialize(
    input.domainAlgebra().getRank(),
    input.coDomainAlgebra().getRank()
  );
  for (int i = 0; i < input.coDomainAlgebra().getRank(); i ++) {
    startingWeight.makeEi(input.coDomainAlgebra().getRank(), i);
    input.projectOntoSmallCartan(startingWeight, projectedWeight);
    this->preferredBasisChangeInversE.actOnVectorColumn(projectedWeight);
    for (int j = 0; j < projectedWeight.size; j ++) {
      projectionBasisChanged.elements[j][i] = projectedWeight[j];
    }
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  subgroup;
  this->parabolicLeviPartRootSpacesZeroStandsForSelected = parabolicSelection;
  Matrix<Rational> DualCartanEmbedding;
  input.getMapSmallCartanDualToLargeCartanDual(DualCartanEmbedding);
  Vector<Rational> parabolicEvaluationRootImage, root;
  parabolicEvaluationRootImage =
  this->parabolicLeviPartRootSpacesZeroStandsForSelected;
  this->parabolicSelectionSmallerAlgebra.initialize(
    input.domainAlgebra().getRank()
  );
  for (int i = 0; i < input.domainAlgebra().getRank(); i ++) {
    DualCartanEmbedding.getVectorFromColumn(i, root);
    if (
      parabolicEvaluationRootImage.scalarEuclidean(root).isPositive()
    ) {
      this->parabolicSelectionSmallerAlgebra.addSelectionAppendNewIndex(i);
    }
  }
  this->log
  << "\nDual cartan embedding smaller into larger:\n"
  << DualCartanEmbedding.toString(&global.defaultFormat.getElement());
  this->log
  << "\nParabolic subalgebra large algebra: "
  << this->parabolicLeviPartRootSpacesZeroStandsForSelected.toString();
  root = this->parabolicSelectionSmallerAlgebra;
  this->log << "\nParabolic subalgebra smaller algebra: " << root.toString();
  subgroup.makeParabolicFromSelectionSimpleRoots(
    weylGroupCoDomain,
    this->parabolicLeviPartRootSpacesZeroStandsForSelected,
    - 1
  );
  this->linearOperators.setSize(subgroup.allElements.size);
  this->linearOperatorsExtended.setSize(subgroup.allElements.size);
  this->translations.setSize(subgroup.allElements.size);
  this->translationsProjectedBasisChanged.setSize(
    subgroup.allElements.size
  );
  this->coefficients.setSize(subgroup.allElements.size);
  this->log
  << " \n******************\nthe subgroup: \n"
  << subgroup.toString()
  << "\n\n\n\n\n\n";
  this->log << subgroup.toStringBruhatGraph();
  this->log
  << "\nMatrix form of the elements of Weyl group "
  << "of the Levi part of the parabolic ("
  << subgroup.allElements.size
  << " elements):\n";
  for (int i = 0; i < subgroup.allElements.size; i ++) {
    Matrix<Rational>& currentLinearOperator = this->linearOperators[i];
    subgroup.getMatrixOfElement(
      subgroup.allElements[i], currentLinearOperator
    );
    //    currentLinearOperator.multiplyOnTheLeft(preferredBasisChangeInverse);
    this->log
    << "\n"
    << currentLinearOperator.toString(
      &global.defaultFormat.getElement()
    );
    currentLinearOperator.actOnVectorColumn(
      subgroup.getRho(), this->translations[i]
    );
    this->translations[i] -= subgroup.getRho();
    this->translations[i].negate();
    projectionBasisChanged.actOnVectorColumn(
      this->translations[i],
      this->translationsProjectedBasisChanged[i]
    );
    if (
      subgroup.allElements[i].generatorsLastAppliedFirst.size % 2 == 0
    ) {
      this->coefficients[i] = 1;
    } else {
      this->coefficients[i] = - 1;
    }
  }
  this->log
  << "\n\n\nMatrix of the projection operator (basis-changed):\n"
  << projectionBasisChanged.toString(&global.defaultFormat.getElement());
  this->log
  << "\n\n\nMatrix form of the operators $u_w$, "
  << "the translations $\tau_w$ and their projections ("
  << this->linearOperatorsExtended.size
  << "):";
  // List<Matrix<Rational> > tempList;
  for (int k = 0; k < this->linearOperators.size; k ++) {
    Matrix<Rational>& currentLO = this->linearOperators[k];
    Matrix<Rational>& currentLOExtended = this->linearOperatorsExtended[k];
    currentLO.multiplyOnTheLeft(projectionBasisChanged);
    currentLO *= - 1;
    // tempList.addOnTopNoRepetition(this->linearOperators.objects[i]);
    currentLOExtended.makeIdentityMatrix(currentLO.numberOfRows);
    currentLOExtended.resize(
      currentLO.numberOfRows,
      currentLO.numberOfRows + currentLO.numberOfColumns,
      true
    );
    for (int i = 0; i < currentLO.numberOfRows; i ++) {
      for (int j = 0; j < currentLO.numberOfColumns; j ++) {
        currentLOExtended.elements[i][j + currentLO.numberOfRows] =
        currentLO.elements[i][j];
      }
    }
    this->log
    << "\n\n"
    << currentLOExtended.toString(&global.defaultFormat.getElement());
    this->log
    << this->translations[k].toString()
    << ";   "
    << this->translationsProjectedBasisChanged[k].toString();
  }
  List<int> displayIndicesReflections;
  for (
    int i = 0; i < this->parabolicLeviPartRootSpacesZeroStandsForSelected.
    numberOfElements; i ++
  ) {
    if (
      !this->parabolicLeviPartRootSpacesZeroStandsForSelected.selected[i]
    ) {
      displayIndicesReflections.addOnTop(i + 1);
    }
  }
  Matrix<Polynomial<Rational> > matrixPoly;
  Vector<Polynomial<Rational> > tempVect, tempVect2;
  tempVect.setSize(
    input.domainAlgebra().weylGroup.getDimension() +
    input.coDomainAlgebra().weylGroup.getDimension()
  );
  for (int i = 0; i < tempVect.size; i ++) {
    tempVect[i].makeMonomial(i, 1, Rational(1));
  }
  matrixPoly.initialize(
    input.domainAlgebra().weylGroup.getDimension(), tempVect.size
  );
  Polynomial<Rational> polyZero;
  polyZero.makeZero();
  format.polynomialAlphabet.setSize(5);
  format.polynomialAlphabet[0] = "x_1";
  format.polynomialAlphabet[1] = "x_2";
  format.polynomialAlphabet[2] = "y_1";
  format.polynomialAlphabet[3] = "y_2";
  format.polynomialAlphabet[4] = "y_3";
  root = subgroup.getRho();
  this->linearOperators[0].actOnVectorColumn(root);
  this->preferredBasisChangE.actOnVectorColumn(root);
  root.negate();
  this->log
  << "\n\nIn $so(7)$-simple basis coordinates, $\\rho_{\\mathfrak l}="
  << subgroup.getRho().toStringLetterFormat("\\eta")
  << "$; $\\pr(\\rho)="
  << root.toStringLetterFormat("\\alpha")
  << "$.";
  this->log
  << "\n\n\\begin{longtable}{r|l}$w$ & \\begin{tabular}{c}"
  << "Argument of the vector partition function "
  << "in (\\ref{eqMultG2inB3General}) =\\\\ $u_w\\circ"
  << tempVect.toString(&format)
  << "-\\tau_w$ \\end{tabular}  \\\\ \\hline \\endhead";
  for (int i = 0; i < this->linearOperatorsExtended.size; i ++) {
    Matrix<Rational>& currentLoExt = this->linearOperatorsExtended[i];
    for (int j = 0; j < currentLoExt.numberOfRows; j ++) {
      for (int k = 0; k < currentLoExt.numberOfColumns; k ++) {
        matrixPoly.elements[j][k].makeConstant(
          currentLoExt.elements[j][k]
        );
      }
    }
    matrixPoly.actOnVectorColumn(tempVect, tempVect2, polyZero);
    for (int j = 0; j < tempVect2.size; j ++) {
      tempVect2[j] += this->translationsProjectedBasisChanged[i][j];
    }
    this->log
    << "\n$"
    << subgroup.allElements[i].toString()
    << "$&$"
    << tempVect2.toString(&format)
    << "$\\\\";
  }
  this->log << "\\end{longtable}\n\n";
  // this->log << "\n\n\nThere are " << tempList.size << " different
  // operators.";
  Lattice tempLattice;
  weylGroupCoDomain.getIntegralLatticeInSimpleCoordinates(tempLattice);
  this->extendedIntegralLatticeMatrixForm.basisRationalForm.makeIdentityMatrix(
    input.domainAlgebra().getRank()
  );
  this->extendedIntegralLatticeMatrixForm.basisRationalForm.directSumWith(
    tempLattice.basisRationalForm, Rational(0)
  );
  this->extendedIntegralLatticeMatrixForm.makeFromMatrix(
    this->extendedIntegralLatticeMatrixForm.basisRationalForm
  );
  Matrix<Rational> invertedCartan;
  invertedCartan = weylGroupCoDomain.cartanSymmetric;
  invertedCartan.invert();
  Vectors<Rational> wallsWeylChamberLargerAlgebra;
  List<List<int> > neighbors;
  for (int i = 0; i < invertedCartan.numberOfRows; i ++) {
    invertedCartan.getVectorFromRow(i, root);
    if (parabolicEvaluationRootImage[i].isEqualToZero()) {
      wallsWeylChamberLargerAlgebra.addOnTop(root);
      neighbors.addOnTop(List<int>());
    }
  }
  this->log
  << "\n\n\n**************\nParabolic selection larger algebra:"
  << parabolicEvaluationRootImage.toString()
  << "\nWalls Weyl chamber larger algebra: "
  << wallsWeylChamberLargerAlgebra.toString();
  this->log << "\n**************\n\n";
  Vectors<Rational> rootsGeneratingExtendedLattice;
  int totalDim =
  input.coDomainAlgebra().getRank() + input.domainAlgebra().getRank();
  rootsGeneratingExtendedLattice.setSize(totalDim);
  this->log
  << "\n"
  << invertedCartan.toString(&global.defaultFormat.getElement())
  << "\n";
  this->log << this->extendedIntegralLatticeMatrixForm.toString();
  this->weylChamberSmallerAlgebra.createFromNormals(
    wallsWeylChamberLargerAlgebra, neighbors
  );
  this->log
  << "\nWeyl chamber larger algebra before projectivizing: "
  << this->weylChamberSmallerAlgebra.toString(&format)
  << "\n";
  this->preimageWeylChamberSmallerAlgebra.walls =
  this->weylChamberSmallerAlgebra.walls;
  for (
    int i = 0; i < this->preimageWeylChamberLargerAlgebra.walls.size; i ++
  ) {
    root.makeZero(
      input.coDomainAlgebra().getRank() + input.domainAlgebra().getRank() + 1
    );
    for (int j = 0; j < input.coDomainAlgebra().getRank(); j ++) {
      root[j + input.domainAlgebra().getRank()] =
      this->preimageWeylChamberLargerAlgebra.walls[i].normal[j];
    }
    this->preimageWeylChamberLargerAlgebra.walls[i].normal = root;
  }
  invertedCartan = input.domainAlgebra().weylGroup.cartanSymmetric;
  invertedCartan.invert();
  roots.size = 0;
  Vector<Rational> parabolicEvaluationRootSmallerAlgebra;
  parabolicEvaluationRootSmallerAlgebra =
  this->parabolicSelectionSmallerAlgebra;
  for (int i = 0; i < invertedCartan.numberOfRows; i ++) {
    input.domainAlgebra().weylGroup.cartanSymmetric.getVectorFromRow(i, root);
    if (
      root.scalarEuclidean(parabolicEvaluationRootSmallerAlgebra).isEqualToZero
      ()
    ) {
      roots.setSize(roots.size + 1);
      invertedCartan.getVectorFromRow(i, *roots.lastObject());
    }
  }
  this->preferredBasisChangeInversE.actOnVectorsColumn(roots);
  this->log << "**********************\n\n\n";
  this->log
  << "\nthe smaller parabolic selection: "
  << this->parabolicSelectionSmallerAlgebra.toString();
  this->log
  << "the Vectors<Rational> generating the chamber walls: "
  << roots.toString();
  this->preimageWeylChamberSmallerAlgebra.createFromVertices(roots);
  this->log
  << "\nWeyl chamber smaller algebra: "
  << this->preimageWeylChamberSmallerAlgebra.toString(&format)
  << "\n";
  this->log << "**********************\n\n\n";
  this->log
  << "\nThe first operator extended:\n"
  << this->linearOperatorsExtended[0].toString(
    &global.defaultFormat.getElement()
  )
  << "\n";
  this->log
  << "\nThe second operator extended:\n"
  << this->linearOperatorsExtended[1].toString(
    &global.defaultFormat.getElement()
  )
  << "\n";
  for (
    int i = 0; i < this->preimageWeylChamberSmallerAlgebra.walls.size; i ++
  ) {
    root.makeZero(
      input.coDomainAlgebra().getRank() + input.domainAlgebra().getRank() + 1
    );
    for (int j = 0; j < input.domainAlgebra().getRank(); j ++) {
      root[j] = this->preimageWeylChamberSmallerAlgebra.walls[i].normal[j];
    }
    //  for (int j = 0; j < input.range.getRank(); j ++)
    //   root.objects[j+ input.domain.getRank()] = root2.objects[j];
    this->preimageWeylChamberSmallerAlgebra.walls[i].normal = root;
  }
  root.makeEi(
    input.coDomainAlgebra().getRank() + input.domainAlgebra().getRank() + 1,
    input.coDomainAlgebra().getRank() + input.domainAlgebra().getRank()
  );
  Wall wall;
  wall.normal = root;
  this->preimageWeylChamberLargerAlgebra.walls.addOnTop(wall);
  this->log
  << "\nPreimage Weyl chamber smaller algebra: "
  << this->preimageWeylChamberSmallerAlgebra.toString(&format)
  << "\n";
  this->log
  << "\nPreimage Weyl chamber larger algebra: "
  << this->preimageWeylChamberLargerAlgebra.toString(&format)
  << "\n";
}

std::string GeneralizedVermaModuleCharacters::prepareReport() {
  std::stringstream out;
  FormatExpressions format;
  int variableIndex = 0;
  format.polynomialAlphabet.setSize(5);
  format.polynomialAlphabet[variableIndex] = "x_1";
  variableIndex ++;
  format.polynomialAlphabet[variableIndex] = "x_2";
  variableIndex ++;
  format.polynomialAlphabet[variableIndex] = "y_1";
  variableIndex ++;
  format.polynomialAlphabet[variableIndex] = "y_2";
  variableIndex ++;
  format.polynomialAlphabet[variableIndex] = "y_3";
  variableIndex ++;
  out
  << "\\documentclass{article}"
  << "\\usepackage{"
  << "amsmath, longtable, amsfonts, amssymb, verbatim, hyperref}"
  << "\n\\begin{document}\\tiny\n";
  out
  << "\n The chamber complex + multiplicities follow.\n\n\n"
  << "\\begin{longtable}{cc}"
  << "\\caption{multiplicities of "
  << "generalized Verma modules $m(x_1,x_2, y_1, y_2, y_3)$"
  << " for $\\gop$ with Dynkin diagram";
  std::stringstream tempStream;
  tempStream << "(";
  for (
    int i = 0; i < this->parabolicLeviPartRootSpacesZeroStandsForSelected.
    numberOfElements; i ++
  ) {
    if (
      this->parabolicLeviPartRootSpacesZeroStandsForSelected.selected[i]
    ) {
      tempStream << "+";
    } else {
      tempStream << "0";
    }
    if (
      i !=
      this->parabolicLeviPartRootSpacesZeroStandsForSelected.numberOfElements -
      1
    ) {
      tempStream << ",";
    }
  }
  tempStream << ")";
  out
  << "$"
  << tempStream.str()
  << "$ \\label{table"
  << tempStream.str()
  << "}}\\\\\n";
  out << "Inequlities& $m(x_1,x_2, y_1, y_2, y_3)$\\endhead\n";
  int numFoundChambers = 0;
  List<int> displayIndicesprojectivizedChambers;
  this->projectivizedChamber.checkIsRefinedOrCrash();
  for (
    int i = 0; i < this->projectivizedChamber.refinedCones.size(); i ++
  ) {
    QuasiPolynomial& currentMultiplicity = this->multiplicities[i];
    if (!currentMultiplicity.isEqualToZero()) {
      numFoundChambers ++;
      out
      << "\\hline\\multicolumn{2}{c}{Chamber "
      << numFoundChambers
      << "}\\\\\n";
      displayIndicesprojectivizedChambers.addOnTop(numFoundChambers);
      out
      << this->prepareReportOneCone(
        format, this->projectivizedChamber.refinedCones[i]
      )
      << "&";
      out << "\\begin{tabular}{c}";
      out << currentMultiplicity.toString(&format) << "\\end{tabular}\\\\\n";
    } else {
      displayIndicesprojectivizedChambers.addOnTop(- 1);
    }
  }
  out << "\\end{longtable}\n\n\n Multiplicity free chambers \n";
  numFoundChambers = 0;
  out << "\n\\begin{longtable}{cc} ";
  out
  << "normals& Multiplicity of module with highest "
  << "weight $(x_1,x_2)$\\endhead\n";
  /* for (int i = 0; i < this->projectivezedChambersSplitByMultFreeWalls.size; i ++) {
    root = this->projectivezedChambersSplitByMultFreeWalls.objects[i].getInternalPoint();
    bool found = false;
    for (int j = 0; j < this->projectivizedChamber.size; j ++)
      if (this->projectivizedChamber.objects[j].isInCone(root)) {
        if (found)
          global.fatal << global.fatal;
        found = true;
      }
  }
  for (int i = 0; i < this->projectivizedChamber.size; i ++) {
    QuasiPolynomial& multiplicity = this->multiplicityiplicities.objects[i];
    if (!multiplicity.isEqualToZero()) {
      int indexMultFreeChamber = - 1;
      for (int j = 0; j < this->projectivezedChambersSplitByMultFreeWalls.size; j ++) {
        root = this->projectivezedChambersSplitByMultFreeWalls.objects[j].getInternalPoint();
        if (this->projectivizedChamber.objects[i].isInCone(root)) {
          Rational scalar;
          scalar =*root.lastObject();
          if (scalar != 0)
            root/= scalar;
          multiplicity.valueOnEachLatticeShift.objects[0].evaluate(root, scalar);
          if (scalar<1) {
            indexMultFreeChamber = j;
            break;
          }
        }
      }
      if (indexMultFreeChamber!= - 1) {
        numFoundChambers++;
        out << "\\hline\\multicolumn{2}{c}{Chamber " << DisplayIndicesprojectivizedChambers.objects[i] << "}\\\\\n";
        out << this->prepareReportOneCone(format, this->projectivezedChambersSplitByMultFreeWalls.objects[indexMultFreeChamber]) << "&";
        out << multiplicity.toString(false, true, format) << "\\\\\n";
      }
    }
  }*/
  out
  << "Total number of chambers with multiplicity 1 or less: "
  << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

void GeneralizedVermaModuleCharacters::inititializeMaximumComputation() {
  STACK_TRACE(
    "GeneralizedVermaModuleCharacters::inititializeMaximumComputation"
  );
  this->maximumComputation.numNonParaM = 2;
  this->maximumComputation.conesLargerDimension.reserve(
    this->projectivizedChamber.refinedCones.size()
  );
  this->maximumComputation.LPtoMaximizeLargerDim.reserve(
    this->multiplicities.size
  );
  this->maximumComputation.conesLargerDimension.setSize(0);
  this->maximumComputation.LPtoMaximizeLargerDim.setSize(0);
  Lattice ZnLattice;
  int affineDimension = 5;
  ZnLattice.makeZn(affineDimension);
  this->numberNonZeroMultiplicities = 0;
  ProgressReport report;
  ConeLatticeAndShift currentCLS;
  Vector<Rational> latticePtoMax;
  for (int i = 0; i < this->multiplicities.size; i ++) {
    if (this->multiplicities[i].isEqualToZero()) {
      continue;
    }
    currentCLS.projectivizedCone = this->projectivizedChamber.refinedCones[i];
    currentCLS.shift.makeZero(affineDimension);
    currentCLS.lattice = ZnLattice;
    bool tempBool =
    this->multiplicities[i].valueOnEachLatticeShift[0].
    rootFromLinearPolynomialConstantTermLastVariable(latticePtoMax);
    if (!tempBool) {
      global.fatal << "This should not happen. " << global.fatal;
    }
    this->maximumComputation.conesLargerDimension.addOnTop(currentCLS);
    this->maximumComputation.LPtoMaximizeLargerDim.addOnTop(latticePtoMax);
    this->numberNonZeroMultiplicities ++;
    std::stringstream out;
    out
    << "Initialized "
    << i + 1
    << " out of "
    << this->maximumComputation.conesLargerDimension.size
    << "; so far "
    << this->numberNonZeroMultiplicities
    << " non-zero multiplicities";
    report.report(out.str());
  }
}

std::string GeneralizedVermaModuleCharacters::prepareReportOneCone(
  FormatExpressions& format, const Cone& cone
) {
  std::stringstream out1;
  Vector<Rational> normalNoConstant;
  int dimSmallerAlgebra = this->linearOperators[0].numberOfRows;
  int dimLargerAlgebra = this->linearOperators[0].numberOfColumns;
  Rational rationalConstant;
  out1 << "\\begin{tabular}{rcl}";
  for (int i = 0; i < cone.walls.size; i ++) {
    Vector<Rational>& currentNormal = cone.walls[i].normal;
    normalNoConstant = currentNormal;
    normalNoConstant.setSize(dimSmallerAlgebra + dimLargerAlgebra);
    rationalConstant = -(*currentNormal.lastObject());
    if (!normalNoConstant.isEqualToZero()) {
      out1
      << "$"
      << normalNoConstant.toStringLetterFormat("x", &format)
      << "$ & $\\geq$ & $"
      << rationalConstant.toString()
      << "$ \\\\";
    }
  }
  out1 << "\\end{tabular}";
  return out1.str();
}

std::string GeneralizedVermaModuleCharacters::
elementToStringMultiplicitiesReport() {
  if (
    this->multiplicities.size != this->projectivizedChamber.refinedCones.size()
  ) {
    global.fatal << "Bad number of multiplicities. " << global.fatal;
  }
  std::stringstream out;
  FormatExpressions format;
  format.polynomialAlphabet.setSize(5);
  format.polynomialAlphabet[0] = "x_1";
  format.polynomialAlphabet[1] = "x_2";
  format.polynomialAlphabet[2] = "y_1";
  format.polynomialAlphabet[3] = "y_2";
  format.polynomialAlphabet[4] = "y_3";
  out
  << "Number chambers: "
  << projectivizedChamber.refinedCones.size()
  << " of them "
  << this->numberNonZeroMultiplicities
  << " non-zero.";
  int inequalityCount = 0;
  for (
    int i = 0; i < this->projectivizedChamber.refinedCones.size(); i ++
  ) {
    inequalityCount += this->projectivizedChamber.refinedCones[i].walls.size;
  }
  out << "\nNumber of inequalities: " << inequalityCount;
  if (
    this->parabolicLeviPartRootSpacesZeroStandsForSelected.cardinalitySelection
    !=
    0
  ) {
    out << this->prepareReport();
  }
  return out.str();
}

void GeneralizedVermaModuleCharacters::getSubstitutionFromNonParametricArray(
  Matrix<Rational>& output,
  Vector<Rational>& outputTranslation,
  Vectors<Rational>& nonParameters,
  int numberOfParameters
) {
  // Reminder: the very last variable comes from the projectivization and
  // contributes to the translation only!
  int numNonParams = nonParameters.size;
  output.initialize(
    numberOfParameters + numNonParams - 1, numberOfParameters - 1
  );
  outputTranslation.makeZero(numberOfParameters + numNonParams - 1);
  output.makeZero();
  for (int l = 0; l < numNonParams; l ++) {
    for (int k = 0; k < numberOfParameters - 1; k ++) {
      output.elements[l][k] = nonParameters[l][k];
    }
    outputTranslation[l] = *nonParameters[l].lastObject();
  }
  for (int l = 0; l < numberOfParameters - 1; l ++) {
    output.elements[l + numNonParams][l] = 1;
  }
}

void GeneralizedVermaModuleCharacters::getProjection(
  int indexOperator,
  const Vector<Rational>& input,
  Vector<Rational>& output
) {
  Matrix<Rational>& currentExtendedOperator =
  this->linearOperatorsExtended[indexOperator];
  Vector<Rational>& currentTranslation =
  this->translationsProjectedBasisChanged[indexOperator];
  if (input.lastObject()->isEqualToZero()) {
    global.fatal
    << "Last coordinate is not supposed to be be zero. "
    << global.fatal;
  }
  output = input;
  output /= *output.lastObject();
  output.size --;
  currentExtendedOperator.actOnVectorColumn(output);
  output += currentTranslation;
}

void GeneralizedVermaModuleCharacters::getSubstitutionFromIndex(
  PolynomialSubstitution<Rational>& outputSub,
  Matrix<LargeInteger>& outputMat,
  LargeIntegerUnsigned& outputDen,
  int index
) {
  Matrix<Rational>& matrixOperator = this->linearOperators[index];
  int dimLargerAlgebra = matrixOperator.numberOfColumns;
  int dimSmallerAlgebra = matrixOperator.numberOfRows;
  Vector<Rational>& translation = this->translations[index];
  Matrix<Rational> matrix;
  matrix.initialize(
    dimLargerAlgebra + dimSmallerAlgebra + 1, dimSmallerAlgebra
  );
  matrix.makeZero();
  for (int j = 0; j < dimSmallerAlgebra; j ++) {
    matrix.elements[j][j] = 1;
    for (int i = 0; i < dimLargerAlgebra; i ++) {
      matrix.elements[i][j] -= matrixOperator.elements[j][i];
    }
    matrix.elements[dimLargerAlgebra + dimSmallerAlgebra][j] =
    - translation[j];
  }
  matrix.getMatrixIntegerWithDenominator(outputMat, outputDen);
  outputSub.makeSubstitutionFromMatrixIntegerAndDenominator(
    outputMat, outputDen
  );
}

void GeneralizedVermaModuleCharacters::transformToWeylProjective(
  int indexOperator,
  Vector<Rational>& startingNormal,
  Vector<Rational>& outputNormal
) {
  Matrix<Rational> operatorExtended =
  this->linearOperatorsExtended[indexOperator];
  Vector<Rational>& translation =
  this->translationsProjectedBasisChanged[indexOperator];
  // The sign in front of translation should be +
  // and not -.
  Rational rationalConstant;
  startingNormal.scalarEuclidean(
    this->nonIntegralOriginModificationBasisChanged + translation,
    rationalConstant
  );
  operatorExtended.transpose();
  outputNormal = startingNormal;
  operatorExtended.actOnVectorColumn(outputNormal);
  outputNormal.setSize(outputNormal.size + 1);
  *outputNormal.lastObject() = - rationalConstant;
}

void GeneralizedVermaModuleCharacters::transformToWeylProjectiveStep1() {
  this->smallerAlgebraChamber.initializeFromDirectionsAndRefine(
    this->gModKNegativeWeightsBasisChanged
  );
  ProgressReport report1;
  ProgressReport report2;
  report1.report(this->smallerAlgebraChamber.toString());
  this->log
  << "Directions for making the chamber basis changed: "
  << this->gModKNegativeWeightsBasisChanged.toString()
  << "\nResulting chamber before projectivization:\n "
  << this->smallerAlgebraChamber.toString();
  report2.report(this->log.str());
}

void GeneralizedVermaModuleCharacters::transformToWeylProjectiveStep2() {
  std::stringstream out;
  ConeCollection projectivizedChamberFinal;
  List<Wall> walls;
  Vector<Rational> wallToSliceWith;
  ProgressReport report;
  projectivizedChamberFinal.initialize();
  for (
    int i = 0; i < this->smallerAlgebraChamber.refinedCones.size(); i ++
  ) {
    const Cone& currentAffineCone =
    this->smallerAlgebraChamber.refinedCones.values[i];
    Wall projectivized;
    for (int j = 0; j < currentAffineCone.walls.size; j ++) {
      this->transformToWeylProjective(
        0, currentAffineCone.walls[j].normal, projectivized.normal
      );
      projectivized.neighbors = currentAffineCone.walls[j].neighbors;
      walls.addOnTop(projectivized);
    }
    report.report(walls.toString());
    Cone currentProjectiveCone(currentAffineCone.id);
    currentProjectiveCone.createFromWalls(walls, false);
    projectivizedChamberFinal.addRefinedCone(currentProjectiveCone);
    projectivizedChamberFinal.convexHull.makeConvexHullOfMeAnd(
      currentProjectiveCone
    );
  }
  for (
    int i = 0; i < this->preimageWeylChamberSmallerAlgebra.walls.size; i ++
  ) {
    projectivizedChamberFinal.splittingNormals.addOnTop(
      this->preimageWeylChamberSmallerAlgebra.walls[i].normal
    );
  }
  out
  << "projectivized chamber before chopping non-dominant part:\n"
  << projectivizedChamberFinal.toString();
  projectivizedChamberFinal.refineByNormals();
  out
  << "Refined projectivized chamber before chopping non-dominant part:\n"
  << projectivizedChamberFinal.toString();
  for (
    int i = 0; i < projectivizedChamberFinal.refinedCones.size(); i ++
  ) {
    const Cone& currentCone = projectivizedChamberFinal.refinedCones[i];
    bool isNonDominant = false;
    for (
      int j = 0; j < this->preimageWeylChamberSmallerAlgebra.walls.size; j ++
    ) {
      if (
        currentCone.internalPoint().scalarEuclidean(
          this->preimageWeylChamberSmallerAlgebra.walls[j].normal
        ).isNegative()
      ) {
        isNonDominant = true;
        break;
      }
    }
    if (isNonDominant) {
      projectivizedChamberFinal.refinedCones.removeKey(currentCone.id);
      i --;
    }
  }
  report.report(out.str());
  this->projectivizedChamber = projectivizedChamberFinal;
  for (int k = 1; k < this->linearOperators.size; k ++) {
    for (
      int i = 0; i < this->smallerAlgebraChamber.refinedCones.size(); i ++
    ) {
      for (
        int j = 0; j < this->smallerAlgebraChamber.refinedCones[i].walls.size;
        j ++
      ) {
        this->transformToWeylProjective(
          k,
          this->smallerAlgebraChamber.refinedCones[i].walls[j].normal,
          wallToSliceWith
        );
        wallToSliceWith.scaleNormalizeFirstNonZero();
        this->projectivizedChamber.splittingNormals.addOnTopNoRepetition(
          wallToSliceWith
        );
      }
    }
  }
  out
  << "projectivized chamber chopped non-dominant part:\n"
  << this->projectivizedChamber.toString();
  report.report(out.str());
}
