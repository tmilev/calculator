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

static ProjectInformationInstance projectInfoMathImplementation4CPP(__FILE__, "Math routines implementation. ");

std::string UserCalculatorData::Roles::admin = "admin";
std::string UserCalculatorData::Roles::student = "student";
std::string UserCalculatorData::Roles::instructor = "instructor";
std::string UserCalculatorData::Roles::teacher = "teacher";

void fatalCrash(const std::string& input) {
  global.fatal << input << global.fatal;
}

GlobalVariables::Crasher::Crasher() {
  this->flagCrashInitiateD = false;
  this->flagFinishingCrash = false;
}

void GlobalVariables::Crasher::FirstRun() {
  if (
    !this->flagCrashInitiateD &&
    global.flagRunningBuiltInWebServer
  ) {
    double elapsedSeconds = global.GetElapsedSeconds();
    this->crashReportHtml << "\n<b style = 'color:red'>Crash</b> "
    << elapsedSeconds << " second(s) from the start.<hr>";
    this->crashReportConsolE << logger::consoleRed() << "Crash " << elapsedSeconds
    << " second(s)" << logger::consoleNormal() << " from the start.\n";
    this->crashReportFile << "Crash " << elapsedSeconds
    << " second(s) from the start.\n";
  }
  this->flagCrashInitiateD = true;
}

GlobalVariables::Crasher& GlobalVariables::Crasher::operator<<(const GlobalVariables::Crasher& dummyCrasherSignalsActualCrash) {
  (void) dummyCrasherSignalsActualCrash;
  this->FirstRun();
  if (this->flagFinishingCrash) {
    std::cout << "Recursion within the crashing mechanism detected. "
    << "Something is very wrong. "
    << this->crashReportConsolE.str() << std::endl;
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
      << logger::consoleBlue() << global.userInputStringIfAvailable
      << logger::consoleNormal() << "\n";
      this->crashReportFile << "User input:\n" << global.userInputStringIfAvailable << "\n";
    }
  }
  this->crashReportConsolE << Crasher::GetStackTraceEtcErrorMessageConsole();
  this->crashReportHtml << Crasher::GetStackTraceEtcErrorMessageHTML();
  this->crashReportFile << Crasher::GetStackTraceEtcErrorMessageHTML();
  if (!global.flagNotAllocated) {
    if (global.progressReportStrings.size > 0) {
      this->crashReportHtml
      << "<hr><b>Computation progress report strings:</b><br>"
      << global.ToStringProgressReportNoThreadData(true);
      this->crashReportFile
      << "<hr><b>Computation progress report strings:</b><br>"
      << global.ToStringProgressReportNoThreadData(true);

      this->crashReportConsolE << "Computation progress strings:\n";
      this->crashReportConsolE << global.ToStringProgressReportNoThreadData(false);
    }
  }
  if (!global.flagNotAllocated) {
    if (!global.calculator().IsZeroPointer()) {
      if (global.calculator().GetElement().Comments.str() != "") {
        this->crashReportHtml << "<hr>Additional comments follow. " << global.calculator().GetElement().Comments.str();
      }
    }
    std::fstream theFile;
    bool openSuccess = FileOperations::OpenFileCreateIfNotPresentVirtual(
      theFile,
      "crashes/" + global.RelativePhysicalNameCrashReport,
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
      this->crashReportConsolE << "Crash dumped in file: " << logger::consoleGreen()
      << global.RelativePhysicalNameCrashReport << logger::consoleNormal() << "\n";
    } else {
      this->crashReportHtml << "<hr>Failed to open crash report file: "
      << global.RelativePhysicalNameCrashReport
      << ". Check file permissions. ";
      this->crashReportConsolE << "Failed to open crash report file: "
      << logger::consoleRed()
      << global.RelativePhysicalNameCrashReport
      << logger::consoleNormal() << "\n";
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
  output[WebAPI::result::comments] = global.Comments.getCurrentReset();
  global.WriteCrash(output);
  assert(false);
  return *this;
}

std::string GlobalVariables::Crasher::GetStackTraceEtcErrorMessageHTML() {
  std::stringstream out;
  out << "A partial stack trace follows (function calls not explicitly logged not included).";
  out << "<table><tr>";
  for (int threadCounter = 0; threadCounter < global.CustomStackTrace.size; threadCounter ++) {
    if (threadCounter >= global.theThreadData.size) {
      out << "<td><b>WARNING: the stack trace reports "
      << global.CustomStackTrace.size
      << " threads but the thread data array has record of only "
      << global.theThreadData.size
      << " threads. " << "</b></td>";
      break;
    }
    out << "<td>" << global.theThreadData[threadCounter].ToStringHtml() << "</td>";
  }
  out << "</tr> <tr>";
  for (int threadCounter = 0; threadCounter<global.CustomStackTrace.size; threadCounter ++) {
    if (threadCounter >= global.theThreadData.size) {
      break;
    }
    if (ThreadData::getCurrentThreadId() != threadCounter) {
      out << "<td>Stack trace available only for current thread.</td>";
      //<-to avoid coordinating threads
      continue;
    }
    ListReferences<stackInfo>& currentInfo = global.CustomStackTrace[threadCounter];
    out << "<td><table><tr><td>file</td><td>line</td><td>function name (if known)</td></tr>";
    for (int i = currentInfo.size - 1; i >= 0; i --) {
      out << "<tr><td>" << HtmlRoutines::GetHtmlLinkFromProjectFileName(currentInfo[i].fileName, "", currentInfo[i].line)
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

std::string GlobalVariables::Crasher::GetStackTraceEtcErrorMessageConsole() {
  std::stringstream out;
  for (int threadCounter = 0; threadCounter<global.CustomStackTrace.size; threadCounter ++) {
    if (threadCounter >= global.theThreadData.size) {
      out << "WARNING: stack trace reports " << global.CustomStackTrace.size << " threads "
      << "while I have only " << global.theThreadData.size << " registered threads. ";
      break;
    }
    out << "********************\r\nThread index " << threadCounter << ": \r\n";
    if (ThreadData::getCurrentThreadId() != threadCounter) {
      out << "Stack trace available only for current thread.\n";
      //<-to avoid coordinating threads
      continue;
    }
    ListReferences<stackInfo>& currentInfo = global.CustomStackTrace[threadCounter];
    for (int i = currentInfo.size - 1; i >= 0; i --) {
      out << currentInfo[i].functionName << "\n";
    }
  }
  return out.str();
}

std::string GlobalVariables::ToStringHTMLTopCommandLinuxSystem() {
  MacroRegisterFunctionWithName("GlobalVariables::ToStringHTMLTopCommandLinuxSystem");
  if (!global.UserDefaultHasAdminRights()) {
    return "Login as admin for RAM memory statistics.";
  }
  std::string topString = this->CallSystemWithOutput("top -b -n 1 -s");
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

std::string GlobalVariables::ToStringFolderInfo() const {
  std::stringstream out;
  out << "<br>Physical path server base: " << this->PhysicalPathServerBase;
  out << "<br>Diplay name executable: " << this->DisplayNameExecutable;
  out << "<br>Physical name folder below executable: " << this->PhysicalNameFolderExecutable;
  out << "<br>Physical path output folder: " << this->PhysicalPathHtmlFolder;
  out << "<br>Display path output folder: " << this->DisplayPathOutputFolder;
  return out.str();
}

std::string GlobalVariables::ToStringThreadData(bool useHTML) {
  std::stringstream out;
  for (int threadIndex = 0; threadIndex < this->progressReportStrings.size; threadIndex ++) {
    if (useHTML) {
      out << "<hr><b>";
    }
    out << this->theThreadData[threadIndex].ToStringHtml();
    if (useHTML) {
      out << "</b><br>";
    }
    out << "\n";
  }
  return out.str();
}

std::string GlobalVariables::ToStringProgressReportWithThreadData(bool useHTML) {
  MacroRegisterFunctionWithName("GlobalVariables::ToStringProgressReportHtmlWithThreadData");
  std::stringstream out;
  out << global.ToStringThreadData(useHTML);
  out << global.ToStringProgressReportNoThreadData(useHTML);
  return out.str();
}

std::string GlobalVariables::ToStringProgressReportNoThreadData(bool useHTML) {
  MacroRegisterFunctionWithName("GlobalVariables::ToStringProgressReportHtmlNoThreadData");
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
      reportStream << "Thread id: " << logger::consoleBlue() << currentThreadID << logger::consoleNormal() << "\n";
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
  if (!global.fatal.flagCrashInitiateD) {
    if (useHTML) {
      reportStream << global.fatal.GetStackTraceEtcErrorMessageHTML();
    } else {
      reportStream << global.fatal.GetStackTraceEtcErrorMessageConsole();
    }
    reportStream << global.GetElapsedMilliseconds()
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

std::string GlobalVariables::ToStringProgressReportConsole() {
  MacroRegisterFunctionWithName("GlobalVariables::ToStringProgressReportConsole");
  std::stringstream reportStream;
  for (int threadIndex = 0; threadIndex < this->progressReportStrings.size; threadIndex ++) {
    if (ThreadData::getCurrentThreadId() != threadIndex) {
      reportStream << "Progress report available only for current thread.<br>";
      //<-to avoid coordinating threads
      continue;
    }
    reportStream << this->theThreadData[threadIndex].ToStringConsole();
    for (int i = 0; i < this->progressReportStrings[threadIndex].size; i ++) {
      reportStream << this->progressReportStrings[threadIndex][i];
    }
  }
  reportStream << "\n";
  return reportStream.str();
}

void GlobalVariables::InitThreadsExecutableStart() {
  //<-Stack trace forbidden this is running before anything has been initialized!
  ThreadData::RegisterFirstThread("main");
}

void GlobalVariables::initFoldersProjectBase(const std::string& inputPhysicalExecutable) {
  StateMaintainerCurrentFolder preserveCurrentFolder;
  this->PhysicalPathProjectBase = FileOperations::GetPathFromFileNameWithPath(inputPhysicalExecutable) + "./";
  this->ChDir(this->PhysicalPathProjectBase);
  this->PhysicalPathProjectBase = FileOperations::GetCurrentFolder() + "/";
}

void GlobalVariables::initDefaultFolderAndFileNames(
  const std::string& inputPhysicalExecutable
) {
  this->initFoldersProjectBase(inputPhysicalExecutable);
  this->PhysicalNameFolderExecutable = this->PhysicalPathProjectBase + "bin/";
  this->PhysicalNameExecutableNoPath = FileOperations::GetFileNameFromFileNameWithPath(inputPhysicalExecutable);
  this->PhysicalNameExecutableWithPath = this->PhysicalNameFolderExecutable + this->PhysicalNameExecutableNoPath;
  this->PhysicalPathHtmlFolder = this->PhysicalPathProjectBase + "../public_html/";
  this->PhysicalPathServerBase = this->PhysicalPathHtmlFolder;
  this->DisplayPathOutputFolder = "/output/";

  this->DisplayNameExecutable = "/cgi-bin/" + this->PhysicalNameExecutableNoPath;
  this->DisplayNameExecutableApp = "/" + WebAPI::app;
  this->DisplayNameExecutableAppNoCache = "/" + WebAPI::appNoCache;
  this->initOutputReportAndCrashFileNames("", "");
}

void GlobalVariables::SetWebInpuT(const std::string& inputName, const std::string& inputValue) {
  MacroRegisterFunctionWithName("GlobalVariables::SetWebInput");
  this->webArguments.SetKeyValue(inputName, inputValue);
}

bool GlobalVariables::UserSecureNonAdminOperationsAllowed() {
  return this->flagLoggedIn && this->flagUsingSSLinCurrentConnection;
}

bool GlobalVariables::UserDebugFlagOn() {
  return global.GetWebInput("debugFlag") == "true";
}

bool GlobalVariables::UserStudentVieWOn() {
  return global.GetWebInput("studentView") == "true";
}

std::string GlobalVariables::LogData::ToStringProcessType() const {
  switch (this->logType) {
  case GlobalVariables::LogData::type::server:
    return "server";
  case GlobalVariables::LogData::type::serverMonitor:
    return "server monitor";
  case GlobalVariables::LogData::type::worker:
    return "worker";
  default:
    return "uknown process type";
  }
}

bool GlobalVariables::CheckConsistency() {
  if (this->flagDeallocated) {
    global.fatal << "Global variables not allowed to be deallocated. " << global.fatal;
  }
  return true;
}

bool GlobalVariables::UserDefaultIsDebuggingAdmin() {
  return this->UserDefaultHasAdminRights() && this->UserDebugFlagOn();
}

bool GlobalVariables::UserDefaultHasAdminRights() {
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return true;
  }
  return this->flagLoggedIn && (this->userDefault.userRole == "admin");
}

bool GlobalVariables::UserDefaultHasProblemComposingRights() {
  return this->flagLoggedIn && (this->userDefault.userRole == "admin" || this->userDefault.userRole == "teacher");
}

bool GlobalVariables::UserGuestMode() {
  if (!this->flagUsingSSLinCurrentConnection) {
    return true;
  }
  return
  this->userCalculatorRequestType == "exerciseNoLogin" ||
  this->userCalculatorRequestType == WebAPI::request::problemGiveUpNoLogin ||
  this->userCalculatorRequestType == WebAPI::request::submitExerciseNoLogin ||
  this->userCalculatorRequestType == WebAPI::request::submitExercisePreviewNoLogin ||
  this->userCalculatorRequestType == "templateNoLogin";
}

bool GlobalVariables::UserRequestRequiresLoadingRealExamData() {
  if (this->UserGuestMode()) {
    return false;
  }
  return this->flagLoggedIn && (
    this->userCalculatorRequestType == "scoredQuiz" ||
    this->userCalculatorRequestType == "scoredQuizJSON" ||
    this->userCalculatorRequestType == "submitAnswers" ||
    this->userCalculatorRequestType == "submitAnswersPreview"
  );
}

bool GlobalVariables::UserRequestMustBePromptedToLogInIfNotLoggedIn() {
  return
  this->userCalculatorRequestType == "scoredQuiz" ||
  this->userCalculatorRequestType == "exercise";
}

std::string GlobalVariables::ToStringCalcArgsNoNavigation(List<std::string>* tagsToExclude) {
  MacroRegisterFunctionWithName("GlobalVariables::ToStringCalcArgsNoNavigation");
  if (!this->flagLoggedIn && !this->UserGuestMode()) {
    return "";
  }
  std::stringstream out;
  for (int i = 0; i < this->webArguments.size(); i ++) {
    const std::string& currentName = this->webArguments.theKeys[i];
    if (
      currentName == "request" ||
      currentName == "password" ||
      currentName == WebAPI::problem::fileName ||
      currentName == "courseHome" ||
      currentName == "topicList" ||
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
      if (tagsToExclude->Contains(currentName)) {
        continue;
      }
    }
    out << global.webArguments.theKeys[i] << "="
    << global.webArguments.theValues[i]
    << "&";
  }
  return out.str();
}

std::string GlobalVariables::GetWebInput(const std::string& inputName) {
  return this->webArguments.GetValueCreate(inputName);
}

void GlobalVariables::MakeReport() {
  MacroRegisterFunctionWithName("GlobalVariables::MakeReport");
  if (this->IndicatorStringOutputFunction == nullptr) {
    return;
  }
  if (this->flagRunningCommandLine || this->flagRunningConsoleTest) {
    this->MakeReport(this->ToStringProgressReportConsole());
  } else {
    this->MakeReport(this->ToStringProgressReportNoThreadData(true));
  }
}

void GlobalVariables::initOutputReportAndCrashFileNames(
  const std::string& inputUserStringRAW, const std::string& inputUserStringCivilized
) {
  std::string inputAbbreviated;
  this->userInputStringIfAvailable =
  FileOperations::CleanUpForFileNameUse(
    inputUserStringCivilized
  );
  if (!global.flagUsingSSLinCurrentConnection) {
    this->userInputStringRAWIfAvailable = inputUserStringRAW;
    inputAbbreviated = this->userInputStringRAWIfAvailable;
  } else {
    this->userInputStringRAWIfAvailable = "Raw user input string not available in SSL mode. ";
    inputAbbreviated = this->userInputStringIfAvailable;
  }
  StringRoutines::StringTrimToLengthWithHash(inputAbbreviated, 150);
  this->RelativePhysicalNameCrashReport = "crash_" + inputAbbreviated + ".html";
}

void FileInformation::AddProjectInfo(const std::string& fileName, const std::string& fileDescription) {
  FileInformation theInfo;
  theInfo.FileName = fileName;
  theInfo.FileDescription = fileDescription;
  if (global.flagNotAllocated) {
    std::cout << "The global variables are not allocated: "
    << "this is the static initialization order fiasco at work! ";
    assert(false);//cannot crash with mechanisms: nothing works yet!
  }
  global.theSourceCodeFiles().AddOnTopNoRepetition(theInfo);
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

std::string UserCalculatorData::ToStringCourseInfo() {
  std::stringstream out;
  out << "Course name:\n" << this->courseComputed
  << "\n<br>Deadline schema:\n" << this->deadlines.ToString(false)
  << "\n<hr>Problem weight schema:\n" << this->problemWeights.ToString(false);
  return out.str();
}

std::string UserCalculatorData::ToStringUnsecure() {
  MacroRegisterFunctionWithName("UserCalculatorData::ToStringUnsecure");
  std::stringstream out;
  out << "User: " << this->username << "\n<br>"
  << this->ToStringCourseInfo()
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
List<Weight<RationalFunctionOld> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<Weight<RationalFunctionOld> >() {
  return nullptr;
}

template<>
List<Weight<Rational> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<Weight<Rational> >() {
  return nullptr;
}

void DynkinDiagramRootSubalgebra::SwapDynkinStrings(int i, int j) {
  this->SimpleComponentTypes.SwapTwoIndices(i, j);
  this->SimpleBasesConnectedComponents.SwapTwoIndices(i, j);
  this->indicesThreeNodes.SwapTwoIndices(i, j);
  this->indicesEnds.SwapTwoIndices(i, j);
}

void DynkinDiagramRootSubalgebra::Sort() {
  //doing bubble sort -> shortest to code
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    for (int j = i + 1; j < this->SimpleBasesConnectedComponents.size; j ++) {
      bool tempBool = false;
      if (this->SimpleBasesConnectedComponents[i].size < this->SimpleBasesConnectedComponents[j].size) {
        tempBool = true;
      }
      if (this->SimpleBasesConnectedComponents[i].size == this->SimpleBasesConnectedComponents[j].size) {
        tempBool = ((this->SimpleComponentTypes[i]) < (this->SimpleComponentTypes[j]));
      }
      if (tempBool) {
        this->SwapDynkinStrings(i, j);
      }
    }
  }
  this->sameTypeComponents.size = 0;
  this->indexInUniComponent.SetSize(this->SimpleBasesConnectedComponents.size);
  this->indexUniComponent.SetSize(this->SimpleBasesConnectedComponents.size);
  this->sameTypeComponents.Reserve(this->SimpleBasesConnectedComponents.size);
  DynkinSimpleType tempType;
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    if (!(this->SimpleComponentTypes[i] == tempType)) {
      this->sameTypeComponents.SetSize(this->sameTypeComponents.size + 1);
      this->sameTypeComponents.LastObject()->size = 0;
      tempType = this->SimpleComponentTypes[i];
    }
    this->sameTypeComponents.LastObject()->AddOnTop(i);
    this->indexUniComponent[i] = this->sameTypeComponents.size - 1;
    this->indexInUniComponent[i] = this->sameTypeComponents.LastObject()->size - 1;
  }
}

Rational DynkinDiagramRootSubalgebra::GetSquareLengthLongestRootLinkedTo(const Vector<Rational>& inputVector) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::GetSquareLengthLongestRootLinkedTo");
  Rational result = 0;
  for (int i = 0; i < this->AmbientRootSystem.size; i ++) {
    if (inputVector.ScalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm) != 0) {
      Rational squareLength = this->AmbientRootSystem[i].ScalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm);
      if (result < squareLength) {
        result = squareLength;
      }
    }
  }
  return result;
}

Rational DynkinDiagramRootSubalgebra::GetSquareLengthShortestRootLinkedTo(const Vector<Rational>& inputVector) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::GetSquareLengthLongestRootLinkedTo");
  Rational result = inputVector.ScalarProduct(inputVector, this->AmbientBilinearForm);
  for (int i = 0; i < this->AmbientRootSystem.size; i ++) {
    if (inputVector.ScalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm) != 0) {
      Rational squareLength = this->AmbientRootSystem[i].ScalarProduct(this->AmbientRootSystem[i], this->AmbientBilinearForm);
      if (squareLength < result) {
        result = squareLength;
      }
    }
  }
  return result;
}

void DynkinDiagramRootSubalgebra::ComputeDynkinString(int indexComponent) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDynkinString");
  this->CheckInitialization();
  if (indexComponent >= this->SimpleBasesConnectedComponents.size) {
    global.fatal << "Bad Dynkin index. " << global.fatal;
  }
  DynkinSimpleType& outputType = this->SimpleComponentTypes[indexComponent];
  Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[indexComponent];
  List<int>& currentEnds = this->indicesEnds[indexComponent];
  if (currentComponent.size < 1) {
    global.fatal << "This is a programming error: currentComponent is empty which is impossible. " << global.fatal;
  }
  if (this->numberOfThreeValencyNodes(indexComponent) == 1) {
    //type D or E
    //in type D first comes the triple node, then the long string, then the one-root strings
    //the long string is oriented with the end that is connected to the triple node having
    //smaller index
    //in type E similarly the longest string comes first oriented with the root that is
    //linked to the triple node having smaller index
    // then comes the second longest string (oriented in the same fashion)
    // and last the one-root string
    Vector<Rational> tripleNode;
    tripleNode = currentComponent[this->indicesThreeNodes[indexComponent]];
    Vectors<Rational> rootsWithoutTripleNode = currentComponent;
    rootsWithoutTripleNode.RemoveIndexSwapWithLast(this->indicesThreeNodes[indexComponent]);
    DynkinDiagramRootSubalgebra diagramWithoutTripleNode;
    diagramWithoutTripleNode.AmbientBilinearForm = this->AmbientBilinearForm;
    diagramWithoutTripleNode.AmbientRootSystem = this->AmbientRootSystem;
    diagramWithoutTripleNode.ComputeDiagramInputIsSimple(rootsWithoutTripleNode);
    if (diagramWithoutTripleNode.SimpleBasesConnectedComponents.size != 3) {
      global.fatal << "This is a programming error: Dynkin diagram has a triple "
      << "node whose removal does not yield 3 connected components. " << global.fatal;
    }
    for (int i = 0; i < 3; i ++) {
      if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[i][0].ScalarProduct(tripleNode, this->AmbientBilinearForm) == 0) {
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[i].ReverseOrderElements();
      }
    }
    for (int i = 0; i < 3; i ++) {
      for (int j = i + 1; j < 3; j ++) {
        if (
          diagramWithoutTripleNode.SimpleBasesConnectedComponents[i].size <
          diagramWithoutTripleNode.SimpleBasesConnectedComponents[j].size
        ) {
          diagramWithoutTripleNode.SwapDynkinStrings(i, j);
        }
      }
    }
    currentComponent.SetSize(0);
    if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].size == 1) {
      //<- components are sorted by length, therefore the second and third component are of length 1,
      //therefore we have type D_n
      Rational theScale = DynkinSimpleType::GetDefaultLongRootLengthSquared('D') /
      tripleNode.ScalarProduct(tripleNode, this->AmbientBilinearForm);
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0]);//<-first long component
      if (!tripleNode.ScalarProduct(currentComponent[0], this->AmbientBilinearForm).IsEqualToZero()) {
        currentComponent.ReverseOrderElements();
      }
      currentComponent.AddOnTop(tripleNode);//<-then triple node
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1]);//<-last two vectors
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[2]);//<-last two vectors
      outputType.MakeArbitrary('D', currentComponent.size, theScale);
    } else {
      //the second largest component has more than one element, hence we are in type E_n.
      Rational theScale = DynkinSimpleType::GetDefaultLongRootLengthSquared('E') / tripleNode.ScalarProduct(tripleNode, this->AmbientBilinearForm);
      if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].size != 2) {
        global.fatal << "This is a programming error: the Dynkin diagram has two components of "
        << "length larger than 2 linked to the triple node."
        << global.fatal;
      }
      if (!tripleNode.ScalarProduct(
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][0],
        this->AmbientBilinearForm).IsEqualToZero()
      ) {
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].ReverseOrderElements(); //<-the 2-root component has the first root perpendicular to the triple node
      }
      if (
        tripleNode.ScalarProduct(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0][0],
        this->AmbientBilinearForm).IsEqualToZero()
      ) {
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[0].ReverseOrderElements(); //<-the largest component has the first root non-perpendicular to the triple node
      }
      currentComponent.AddOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][0]); //<-first root from 2-element component
      currentComponent.AddOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[2][0]); //<-then the small sticky part of the Dynkin diagram
      currentComponent.AddOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][1]); //<-next the second root from 2-element component
      currentComponent.AddOnTop(tripleNode); //<- next the triple node
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0]); //<-finally the longest component. Conventions, conventions...
      outputType.MakeArbitrary('E', currentComponent.size, theScale);
    }
   return;
  }
  Rational length1, length2;
  length1 = currentComponent[0].ScalarProduct(currentComponent[0], this->AmbientBilinearForm);
  int numLength1 = 1;
  int numLength2 = 0;
  for (int i = 1; i < currentComponent.size; i ++) {
    if (currentComponent[i].ScalarProduct(currentComponent[i], this->AmbientBilinearForm) == length1) {
      numLength1 ++;
    } else {
      numLength2 ++;
      length2 = currentComponent[i].ScalarProduct(currentComponent[i], this->AmbientBilinearForm);
    }
  }
  if (numLength2 == 0) {
    //type A
    outputType.MakeArbitrary('A', numLength1, DynkinSimpleType::GetDefaultLongRootLengthSquared('A') / length1);
  } else {
    if (length1 < length2) {
      MathRoutines::swap(length1, length2);
      MathRoutines::swap(numLength1, numLength2);
      currentEnds.SwapTwoIndices(0, 1);
    }//<-so far we made sure the first length is long
    //By convention, in types G and C, in the Dynkin diagram the long root comes last
    //This is handled at the very end of this function (outside all the if clauses).
    if (numLength1 == numLength2) {
      //B2, C2, F4 or G2
      if (numLength1 == 2) {
        outputType.MakeArbitrary('F', 4, DynkinSimpleType::GetDefaultLongRootLengthSquared('F') / length1);
      } else if (length1 / length2 == 3) {
        outputType.MakeArbitrary('G', 2, DynkinSimpleType::GetDefaultLongRootLengthSquared('G') / length1);
      } else {
        outputType.MakeArbitrary('B', 2, DynkinSimpleType::GetDefaultLongRootLengthSquared('B') / length1);
      }
    } else {
      if (numLength1>numLength2) {
        outputType.MakeArbitrary(
          'B', currentComponent.size, DynkinSimpleType::GetDefaultLongRootLengthSquared('B') / length1
        );
      } else {
        outputType.MakeArbitrary(
          'C', currentComponent.size, DynkinSimpleType::GetDefaultLongRootLengthSquared('C') / length1
        );
      }
    }
  }
  //The following code ensures the Dynkin diagram is properly ordered
  currentComponent.SwapTwoIndices(0, currentEnds[0]);
  for (int i = 0; i < currentComponent.size; i ++) {
    for (int j = i + 1; j < currentComponent.size; j ++) {
      if (!currentComponent[i].ScalarProduct(currentComponent[j], this->AmbientBilinearForm).IsEqualToZero()) {
        currentComponent.SwapTwoIndices(i + 1, j);
        break;
      }
    }
  }
  //so far we made sure the entire component is one properly ordered string, starting with the long root.
  if (outputType.theLetter == 'G' || outputType.theLetter == 'C' ) {
    currentComponent.ReverseOrderElements();//<-in G_2 and C_n the short root comes first so we need to reverse elements.
  }
}

std::string DynkinDiagramRootSubalgebra::ToString(FormatExpressions* theFormat) const {
  DynkinType theType;
  theType.MakeZero();
  for (int j = 0; j < this->SimpleComponentTypes.size; j ++) {
    theType.AddMonomial(this->SimpleComponentTypes[j], 1);
  }
  return theType.ToString(theFormat);
}

bool DynkinDiagramRootSubalgebra::CheckInitialization() const {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::CheckInitialization");
  if (this->AmbientRootSystem.size != 0) {
    if (this->AmbientBilinearForm.NumRows != this->AmbientRootSystem[0].size) {
      global.fatal << "Ambient bilinear form of Dynkin subdiagram not initialized. " << global.fatal;
    }
  }
  return true;
}

void DynkinDiagramRootSubalgebra::ComputeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramInputIsSimple");
  this->CheckInitialization();
  this->SimpleBasesConnectedComponents.size = 0;
  this->SimpleBasesConnectedComponents.Reserve(simpleBasisInput.size);
  for (int i = 0; i < simpleBasisInput.size; i ++) {
    int indexFirstComponentConnectedToRoot = - 1;
    for (int j = 0; j < this->SimpleBasesConnectedComponents.size; j ++) {
      if (this->SimpleBasesConnectedComponents[j].ContainsARootNonPerpendicularTo(
        simpleBasisInput[i], this->AmbientBilinearForm)
      ) {
        if (indexFirstComponentConnectedToRoot == - 1) {
          indexFirstComponentConnectedToRoot = j;
          this->SimpleBasesConnectedComponents[j].AddOnTop(simpleBasisInput[i]);
        } else {
          this->SimpleBasesConnectedComponents[indexFirstComponentConnectedToRoot].AddListOnTop(
            this->SimpleBasesConnectedComponents[j]
          );
          this->SimpleBasesConnectedComponents.RemoveIndexSwapWithLast(j);
          j --;
        }
      }
    }
    if (indexFirstComponentConnectedToRoot == - 1) {
      this->SimpleBasesConnectedComponents.SetSize(this->SimpleBasesConnectedComponents.size + 1);
      this->SimpleBasesConnectedComponents.LastObject()->size = 0;
      this->SimpleBasesConnectedComponents.LastObject()->AddOnTop(simpleBasisInput[i]);
    }
  }
  this->ComputeDynkinStrings();
  this->Sort();
  this->ComputeDynkinStrings();
  DynkinType tempType;
  this->GetDynkinType(tempType);
  if (tempType.IsEqualToZero() && simpleBasisInput.size != 0) {
    global.fatal << "Dynkin type of zero but the roots generating the type are: " << simpleBasisInput.ToString() << global.fatal;
  }
}

bool DynkinDiagramRootSubalgebra::LetterIsDynkinGreaterThanLetter(char letter1, char letter2) {
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

bool DynkinDiagramRootSubalgebra::IsGreaterThan(DynkinDiagramRootSubalgebra& right) {
  if (this->RankTotal() > right.RankTotal()) {
    return true;
  }
  if (this->RankTotal() < right.RankTotal()) {
    return false;
  }
  if (this->SimpleComponentTypes.size != this->SimpleBasesConnectedComponents.size) {
    global.fatal << "Simple component types do not match number of connected components. " << global.fatal;
  }
  for (int i = 0; i < this->SimpleComponentTypes.size; i ++) {
    if (this->SimpleBasesConnectedComponents[i].size > right.SimpleBasesConnectedComponents[i].size) {
      return true;
    }
    if (right.SimpleBasesConnectedComponents[i].size > this->SimpleBasesConnectedComponents[i].size) {
      return false;
    }
    if (this->SimpleComponentTypes[i] > right.SimpleComponentTypes[i]) {
      return true;
    }
    if (right.SimpleComponentTypes[i] > this->SimpleComponentTypes[i]) {
      return false;
    }
  }
  return false;
}

Rational DynkinDiagramRootSubalgebra::GetSizeCorrespondingWeylGroupByFormula() {
  Rational output = 1;
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    output *= WeylGroupData::SizeByFormulaOrNeg1(
      this->SimpleComponentTypes[i].theLetter, this->SimpleComponentTypes[i].theRank
    );
  }
  return output;
}

void DynkinDiagramRootSubalgebra::GetMapFromPermutation(
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
      domain.AddOnTop( this->SimpleBasesConnectedComponents[i][j]);
      int indexTargetComponent = thePerm[i];
      int indexAutomorphismInComponent = theAutosPerm.Multiplicities[i];
      int indexRoot = theAutos[i][indexAutomorphismInComponent][j];
      range.AddOnTop(right.SimpleBasesConnectedComponents[indexTargetComponent][indexRoot]);
    }
  }
}

void DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInput(Vectors<Rational>& inputRoots, WeylGroupData& theWeyl) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInput");
  this->AmbientRootSystem = theWeyl.RootSystem;
  this->AmbientBilinearForm = theWeyl.CartanSymmetric;
  theWeyl.TransformToSimpleBasisGenerators(inputRoots, theWeyl.RootSystem);
  this->ComputeDiagramInputIsSimple(inputRoots);
}

void DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInputRelative(
  Vectors<Rational>& inputOutputSimpleWeightSystem,
  const HashedList<Vector<Rational> >& weightSystem,
  const Matrix<Rational>& theBilinearForm
) {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInputRelative");
  this->AmbientRootSystem = weightSystem;
  this->AmbientBilinearForm = theBilinearForm;
  WeylGroupData::TransformToSimpleBasisGeneratorsArbitraryCoords(inputOutputSimpleWeightSystem, weightSystem);
  this->ComputeDiagramInputIsSimple(inputOutputSimpleWeightSystem);
}

void DynkinDiagramRootSubalgebra::ComputeDynkinStrings() {
  MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDynkinStrings");
  this->indicesThreeNodes.SetSize(this->SimpleBasesConnectedComponents.size);
  this->SimpleComponentTypes.SetSize(this->SimpleBasesConnectedComponents.size);
  this->indicesEnds.SetSize(this->SimpleBasesConnectedComponents.size);
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    this->ComputeDynkinString(i);
  }
}

bool DynkinDiagramRootSubalgebra::operator==(const DynkinDiagramRootSubalgebra& right) const {
  if (right.SimpleBasesConnectedComponents.size != this->SimpleBasesConnectedComponents.size) {
    return false;
  }
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    bool tempBool =
      ((this->SimpleBasesConnectedComponents[i].size == right.SimpleBasesConnectedComponents[i].size) &&
      (this->SimpleComponentTypes[i] == right.SimpleComponentTypes[i]));
    if (!tempBool) {
      return false;
    }
  }
  return true;
}

void DynkinDiagramRootSubalgebra::GetDynkinType(DynkinType& output) const {
  output.MakeZero();
  output.SetExpectedSize(this->SimpleComponentTypes.size);
  for (int i = 0; i < this->SimpleComponentTypes.size; i ++) {
    output.AddMonomial(this->SimpleComponentTypes[i], 1);
  }
}

void DynkinDiagramRootSubalgebra::GetAutomorphism(List<List<int> >& output, int index) {
  Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[index];
  DynkinSimpleType& currentStrinG = this->SimpleComponentTypes[index];
  List<int> thePermutation;
  thePermutation.SetSize(currentComponent.size);
  output.size = 0;
  for (int i = 0; i < currentComponent.size; i ++) {
    thePermutation[i] = i;
  }
  output.AddOnTop(thePermutation);
  if (currentStrinG.theLetter == 'A' && currentComponent.size != 1) {
    thePermutation.ReverseOrderElements();
    output.AddOnTop(thePermutation);
  }
  if (currentStrinG.theLetter == 'D') {
    if (currentComponent.size == 4) {
     //the automorphism group of the Dynkin Diagram is S3
      thePermutation[1] = 2;
      thePermutation[2] = 3;
      thePermutation[3] = 1;
      output.AddOnTop(thePermutation);
      thePermutation[1] = 1;
      thePermutation[2] = 3;
      thePermutation[3] = 2;
      output.AddOnTop(thePermutation);
      thePermutation[1] = 2;
      thePermutation[2] = 1;
      thePermutation[3] = 3;
      output.AddOnTop(thePermutation);
      thePermutation[1] = 3;
      thePermutation[2] = 1;
      thePermutation[3] = 2;
      output.AddOnTop(thePermutation);
      thePermutation[1] = 3;
      thePermutation[2] = 2;
      thePermutation[3] = 1;
      output.AddOnTop(thePermutation);
    } else {
      thePermutation[currentComponent.size - 2] = currentComponent.size - 1;
      thePermutation[currentComponent.size - 1] = currentComponent.size - 2;
      output.AddOnTop(thePermutation);
    }
  }
  if (currentStrinG.theLetter == 'E' && currentStrinG.theRank == 6) {
    thePermutation[1] = 3;
    thePermutation[2] = 4;
    thePermutation[3] = 1;
    thePermutation[4] = 2;
    output.AddOnTop(thePermutation);
  }
}

void DynkinDiagramRootSubalgebra::GetAutomorphisms(List<List<List<int> > >& output) {
  output.SetSize(this->SimpleBasesConnectedComponents.size);
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    this->GetAutomorphism(output[i], i);
  }
}

int DynkinDiagramRootSubalgebra::RankTotal() {
  int result = 0;
  for (int i = 0; i < this->SimpleBasesConnectedComponents.size; i ++) {
    result += this->SimpleBasesConnectedComponents[i].size;
  }
  return result;
}

int DynkinDiagramRootSubalgebra::NumRootsGeneratedByDiagram() {
  int result = 0;
  if (this->SimpleBasesConnectedComponents.size != this->SimpleComponentTypes.size) {
    global.fatal << "Number of simple connected components does not match the number of types. " << global.fatal;
  }
  for (int i = 0; i < this->SimpleComponentTypes.size; i ++) {
    int Rank = this->SimpleBasesConnectedComponents[i].size;
    if (this->SimpleComponentTypes[i].theLetter == 'A') {
      result += Rank * (Rank + 1);
    }
    if (this->SimpleComponentTypes[i].theLetter == 'B' || this->SimpleComponentTypes[i].theLetter == 'C') {
      result += Rank * Rank * 2;
    }
    if (this->SimpleComponentTypes[i].theLetter == 'D') {
      result += Rank * (Rank - 1) * 2;
    }
    if (this->SimpleComponentTypes[i].theLetter == 'E') {
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
    if (this->SimpleComponentTypes[i].theLetter == 'F') {
      result += 48;
    }
    if (this->SimpleComponentTypes[i].theLetter == 'G') {
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
      if (currentComponent[i].ScalarProduct(currentComponent[j], this->AmbientBilinearForm).IsNegative()) {
        counter ++;
      }
    }
    if (counter > 3) {
      Matrix<Rational> theGram;
      currentComponent.GetGramMatrix(theGram, &this->AmbientBilinearForm);
      global.fatal << "This is a programming error: corrupt simple basis corresponding to "
      << "Dynkin diagram: the Dynkin diagram should have nodes with"
      << " valency at most 3, but this diagram has node with valency " << counter << ". The current component is: "
      << currentComponent.ToString() << ". The corresponding Symmetric Cartan is: "
      << theGram.ToString() << ". " << global.fatal;
    }
    if (counter == 3) {
      result ++;
      this->indicesThreeNodes[indexComponent] = i;
    }
    if (counter <= 1) {
      numEnds ++;
      this->indicesEnds[indexComponent].AddOnTop(i);
    }
  }
  if (result > 1) {
    global.fatal << "numEnds variable equals: " << numEnds << ", number of three-nodes equals: "
    << result << "; this should not happen. The bilinear form is: "
    << this->AmbientBilinearForm.ToString() << global.fatal;
  }
  if (result == 1) {
    if (numEnds != 3) {
      global.fatal << "numEnds variable equals: " << numEnds << ", number of three-nodes equals: "
      << result << "; this should not happen. The bilinear form is: " << this->AmbientBilinearForm.ToString() << global.fatal;
    }
  } else {
    if (numEnds > 2) {
      global.fatal << "numEnds variable equals: " << numEnds << ", number of three-nodes equals: "
      << result << "; this should not happen. The bilinear form is: " << this->AmbientBilinearForm.ToString() << global.fatal;
    }
  }
  return result;
}

bool affineCone::SplitByAffineHyperplane(affineHyperplane<Rational>& theKillerPlane, affineCones& output) {
  (void) theKillerPlane;
  (void) output;
  return true;
}

bool affineCone::WallIsInternalInCone(affineHyperplane<Rational>& theKillerCandidate) {
  (void) theKillerCandidate;
  return true;
}

int affineCone::GetDimension() {
  if (this->theWalls.size == 0) {
    return 0;
  }
  return this->theWalls.TheObjects[0].affinePoint.size;
}

unsigned int affineCone::HashFunction() const {
  int tempMin = MathRoutines::Minimum(this->theWalls.size, ::SomeRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < tempMin; i ++) {
    result += this->theWalls[i].HashFunction() * ::SomeRandomPrimes[i];
  }
  return result;
}

void affineHyperplanes::ToString(std::string& output) {
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    std::string tempS;
    this->TheObjects[i].ToString(tempS);
    out << "index: " << i << " " << tempS << "\n";
  }
  output = out.str();
}

void permutation::initPermutation(int n) {
  this->initPart1(n);
  for (int i = 0; i < n; i ++) {
    this->MaxMultiplicities[i] = n - i - 1;
    this->Multiplicities[i] = 0;
  }
}

void permutation::initPermutation(List<int>& disjointSubsets, int TotalNumElements) {
  this->initPart1(TotalNumElements);
  int counter = 0;
  for (int i = 0; i < disjointSubsets.size; i ++) {
    for (int j = 0; j < disjointSubsets[i]; j ++) {
      this->MaxMultiplicities[counter] = disjointSubsets[i] - j - 1;
      this->Multiplicities[counter] = 0;
      counter ++;
    }
    TotalNumElements -= disjointSubsets[i];
  }
  if (TotalNumElements != 0) {
    global.fatal << "Permutations with 0 elements not allowed. " << global.fatal;
  }
}

void permutation::incrementAndGetPermutation(List<int>& output) {
  this->IncrementReturnFalseIfPastLast();
  this->GetPermutationLthElementIsTheImageofLthIndex(output);
}

void permutation::GetPermutationLthElementIsTheImageofLthIndex(List<int>& output) {
  int numElements = this->Multiplicities.size;
  output.SetSize(numElements);
  for (int i = 0; i < numElements; i ++) {
    output[i] = i;
  }
  for (int i = 0; i < numElements; i ++) {
    MathRoutines::swap(output[i], output[i + this->Multiplicities[i]]);
  }
}

bool WeylGroupData::AreMaximallyDominantGroupInner(List<Vector<Rational> >& theWeights) {
  MacroRegisterFunctionWithName("WeylGroup::AreMaximallyDominantInner");
  for (int i = 0; i < theWeights.size; i ++) {
    for (int j = 0; j < this->RootsOfBorel.size; j ++) {
      if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[i]) < 0) {
        bool reflectionDoesRaise = true;
        for (int k = 0; k < i; k ++) {
          if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[k]) > 0) {
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

bool WeylGroupAutomorphisms::AreMaximallyDominantGroupOuter(List<Vector<Rational> >& theWeights) {
  MacroRegisterFunctionWithName("WeylGroup::AreMaximallyDominantGroupOuter");
  this->checkInitialization();
  MemorySaving<Vectors<Rational> > theWeightsCopy;
  Vector<Rational> zeroWeight;
  this->ComputeOuterAutos();
  zeroWeight.MakeZero(this->theWeyl->GetDim());
  for (int i = 0; i < theWeights.size; i ++) {
    for (int j = 0; j < this->theWeyl->RootsOfBorel.size; j ++) {
      if (this->theWeyl->RootScalarCartanRoot(this->theWeyl->RootsOfBorel[j], theWeights[i]) < 0) {
        bool reflectionDoesRaise = true;
        for (int k = 0; k < i; k ++) {
          if (this->theWeyl->RootScalarCartanRoot(this->theWeyl->RootsOfBorel[j], theWeights[k]) > 0) {
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
      theWeightsCopy.GetElement() = theWeights;
      this->theOuterAutos.theElements[j].ActOnVectorsColumn(theWeightsCopy.GetElement());
      bool isGood = true;
      for (int k = 0; k < i; k ++) {
        if (!(theWeightsCopy.GetElement()[k] - theWeights[k]).IsPositiveOrZero()) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      if (!(theWeightsCopy.GetElement()[i] - theWeights[i]).IsGreaterThanLexicographic(zeroWeight)) {
        continue;
      }
      return false;
    }
  }
  return true;
}
void WeylGroupData::GenerateRootSubsystem(Vectors<Rational>& theRoots) {
  Vector<Rational> tempRoot;
  int oldsize = theRoots.size;
  for (int i = 0; i < oldsize; i ++) {
    theRoots.AddOnTopNoRepetition(- theRoots[i]);
  }
  for (int i = 0; i < theRoots.size; i ++) {
    for (int j = 0; j < theRoots.size; j ++) {
      tempRoot = theRoots[i] + theRoots[j];
      if (this->IsARoot(tempRoot)) {
        theRoots.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
}

void GeneralizedVermaModuleCharacters::ComputeQPsFromChamberComplex() {
  std::stringstream out;
  FormatExpressions theFormat;
  Vector<Rational> tempRoot;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    this->theMultiplicitiesMaxOutputReport2, "output/ExtremaPolys.txt", false, true, false
  );
  this->thePfs.initFromRoots(this->GmodKNegWeightsBasisChanged);
  this->thePfs.ComputeDebugString();
  out << this->thePfs.DebugString;
  this->thePfs.split(nullptr);
  this->thePfs.ComputeDebugString();
  out << "=" << this->thePfs.DebugString;
//  int totalDim= this->theTranslationS[0].size +this->theTranslationsProjecteD[0].size;
  this->theQPsSubstituted.SetSize(this->projectivizedChambeR.size);
  global.fatal << "not implemented fully, crashing to let you know. " << global.fatal;
//  this->thePfs.theChambersOld.init();
//  this->thePfs.theChambersOld.theDirections = this->GmodKNegWeightsBasisChanged;
//  this->thePfs.theChambersOld.SliceTheEuclideanSpace(false);
//  this->theQPsNonSubstituted.SetSize(this->thePfs.theChambersOld.size);
//  this->theQPsSubstituted.SetSize(this->thePfs.theChambersOld.size);
  out << "\n\nThe vector partition functions in each chamber follow.";
  global.fatal << "Not implemented yet. " << global.fatal;
/*
  for (int i = 0; i < this->thePfs.theChambersOld.size; i ++)
    if (this->thePfs.theChambersOld.TheObjects[i] != 0) {
      QuasiPolynomial& currentQPNoSub= this->theQPsNonSubstituted.TheObjects[i];
      this->theQPsSubstituted.TheObjects[i].SetSize(this->theLinearOperators.size);
      this->thePfs.GetVectorPartitionFunction(currentQPNoSub, this->thePfs.theChambersOld.TheObjects[i]->InternalPoint);
      out << "\nChamber " << i + 1 << " with internal point " << this->thePfs.theChambersOld.TheObjects[i]->InternalPoint.ToString() << " the quasipoly is: " << currentQPNoSub.ToString(false, false);
      for (int k = 0; k< this->theLinearOperators.size; k++) {
        QuasiPolynomial& currentQPSub= this->theQPsSubstituted.TheObjects[i].TheObjects[k];
        std::stringstream tempStream;
        tempStream << "Processing chamber " << i + 1 << " linear operator " << k+ 1;
        global.theIndicatorVariables.ProgressReportStrings[0] = tempStream.str();
        global.MakeReport();
        currentQPNoSub.Substitution(this->theLinearOperatorsExtended.TheObjects[k], this->theTranslationsProjectedBasisChanged[k], this->theExtendedIntegralLatticeMatForM, currentQPSub);
        out << "; after substitution we get: " << currentQPSub.ToString(false, false);
      }
    }
    */
//  this->theParser.theHmm.theRange.theWeyl.GetIntegralLatticeInSimpleCoordinates(integralLattice);
  //out << "\nThe integral lattice:\n" << integralLattice.ToString(false, false);
  //this->theMultiplicitiesMaxOutputReport2.flush();
  QuasiPolynomial tempQP;
  this->theMultiplicities.SetSize(this->projectivizedChambeR.size);
  this->numNonZeroMults = 0;
  ProgressReport theReport;
  ProgressReport theReport2;
  for (int i = 0; i < this->projectivizedChambeR.size; i ++) {
    QuasiPolynomial& currentSum = this->theMultiplicities.TheObjects[i];
    currentSum.MakeZeroOverLattice(this->theExtendedIntegralLatticeMatForM);
    for (int k = 0; k < this->theLinearOperators.size; k ++) {
      this->GetProjection(k, this->projectivizedChambeR.TheObjects[i].GetInternalPoint(), tempRoot);
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
      theReport.Report(tempStream.str());
    }
    if (!currentSum.IsEqualToZero()) {
      this->numNonZeroMults ++;
    }
    std::stringstream tempStream;
    tempStream << " So far " << i + 1 << " out of " << this->projectivizedChambeR.size << " processed " << this->numNonZeroMults
    << " non-zero total.";
    theReport2.Report(tempStream.str());
    out << "\nChamber " << i + 1 << ": the quasipolynomial is: " << currentSum.ToString(false, false);
    out << "\nThe chamber is: " << this->projectivizedChambeR[i].ToString(&theFormat);
  }
//  this->projectivizedChamber.ComputeDebugString();
//  out << "\n\n" << this->projectivizedChamber.DebugString;
  theReport.Report(out.str());
  this->theMultiplicitiesMaxOutputReport2 << out.str();
}

std::string GeneralizedVermaModuleCharacters::ComputeMultsLargerAlgebraHighestWeight(
  Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel
) {
  std::stringstream out;
  WeylGroupData& LargerWeyl = this->theHmm.theRange().theWeyl;
  WeylGroupData& SmallerWeyl = this->theHmm.theDomain().theWeyl;
  if (!LargerWeyl.IsOfSimpleType('B', 3)) {
    return "Error: algebra is not so(7).";
  }
  this->initFromHomomorphism(parabolicSel, this->theHmm);
  this->TransformToWeylProjectiveStep1();
  this->TransformToWeylProjectiveStep2();
  Vector<Rational> highestWeightLargerAlgSimpleCoords;
  highestWeightLargerAlgSimpleCoords = LargerWeyl.GetSimpleCoordinatesFromFundamental(highestWeightLargerAlgebraFundamentalCoords);
  Matrix<Rational> tempMat;
  Vector<Rational> tempRoot;
  DrawingVariables drawOps;
  int theSmallDim = SmallerWeyl.CartanSymmetric.NumRows;
//  drawOps.theBuffer.initDimensions(theSmallDim, 1);
  Vectors<double> theDraggableBasis;
  theDraggableBasis.MakeEiBasis(theSmallDim);
  WeylGroupData tmpWeyl;
  tmpWeyl.MakeArbitrarySimple('A', 2);
  drawOps.theBuffer.initDimensions(tmpWeyl.CartanSymmetric, theDraggableBasis, theDraggableBasis);
  FormatExpressions theFormat;
  drawOps.theBuffer.BasisProjectionPlane[0][0] = 1;
  drawOps.theBuffer.BasisProjectionPlane[0][1] = 0;
  drawOps.theBuffer.BasisProjectionPlane[1][0] = 1;
  drawOps.theBuffer.BasisProjectionPlane[1][1] = 1;
  drawOps.theBuffer.ModifyToOrthonormalNoShiftSecond
  (drawOps.theBuffer.BasisProjectionPlane[1], drawOps.theBuffer.BasisProjectionPlane[0]);
  drawOps.theBuffer.GraphicsUnit = 50;
  PiecewiseQuasipolynomial theStartingPoly, theSubbedPoly, Accum;
  std::string tempS;
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS);
  Vectors<Rational> translationsProjectedFinal;
  translationsProjectedFinal.SetSize(this->theLinearOperators.size);
  this->theLinearOperators[0].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[0]);
  out << "<br>Input so(7)-highest weight: " << highestWeightLargerAlgSimpleCoords.ToString();
  out << "<br>Input parabolics selections: " << parabolicSel.ToString();
  out << "<br>the argument translations: " << this->theTranslationsProjectedBasisChanged.ToString();
  out << "<br>Element u_w: projection, multiplication by - 1, and basis change of so(7)-highest weight to G_2: "
  << translationsProjectedFinal[0].ToString();
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS);
  drawOps.drawCoordSystemBuffer(drawOps, 2);
  Cone smallWeylChamber;
  tempMat = SmallerWeyl.CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempVertices;
  Vector<Rational> tMpRt;
  tMpRt = this->ParabolicSelectionSmallerAlgebra;
  for (int i = 0; i < this->ParabolicSelectionSmallerAlgebra.MaxSize; i ++) {
    tempMat.GetVectorFromRow(i, tempRoot);
    tempVertices.AddOnTop(tempRoot);
    if (this->ParabolicSelectionSmallerAlgebra.selected[i]) {
      tempVertices.AddOnTop(- tempRoot);
    }
  }
  smallWeylChamber.CreateFromVertices(tempVertices);
  tempMat.init(2, 2);
  tempMat.elements[0][0] = 1; tempMat.elements[0][1] = 0;
  tempMat.elements[1][0] = 1; tempMat.elements[1][1] = 1;
  tempMat.Transpose();
  smallWeylChamber.ChangeBasis(tempMat);
  out << "<br> The small Weyl chamber: " << smallWeylChamber.ToString(&theFormat);
  Vector<Rational> highestWeightSmallAlgBasisChanged = - translationsProjectedFinal[0];
  for (int i = 0; i < this->theLinearOperators.size; i ++) {
    this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[i]);
    translationsProjectedFinal[i] += this->theTranslationsProjectedBasisChanged[i];
    drawOps.drawCircleAtVectorBufferRational(- translationsProjectedFinal[i], "red", 3);
  }
  out << "<br>the translations projected final: " << translationsProjectedFinal.ToString();
  Accum.MakeZero(theStartingPoly.NumVariables);
  for (int i = 0; i < this->theLinearOperators.size; i ++) {
    theSubbedPoly = theStartingPoly;
    theSubbedPoly *= this->theCoeffs[i];
    theSubbedPoly.TranslateArgument(translationsProjectedFinal[i]);
    Accum += theSubbedPoly;
  }
  Accum.DrawMe(drawOps, 10, &smallWeylChamber, &highestWeightSmallAlgBasisChanged);
  out << drawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << Accum.ToString(false, true);
  return out.str();
}

void GeneralizedVermaModuleCharacters::SortMultiplicities() {
  List<Cone> tempList;
  tempList = this->projectivizedChambeR;
  tempList.QuickSortAscending();
  List<QuasiPolynomial> tempQPlist;
  tempQPlist.SetSize(this->theMultiplicities.size);
  for (int i = 0; i < this->theMultiplicities.size; i ++) {
    tempQPlist[i] = this->theMultiplicities[this->projectivizedChambeR.GetIndex(tempList[i])];
  }
  this->theMultiplicities = tempQPlist;
  this->projectivizedChambeR.Clear();
  for (int i = 0; i < tempList.size; i ++) {
    this->projectivizedChambeR.AddOnTop(tempList[i]);
  }
}

std::string GeneralizedVermaModuleCharacters::CheckMultiplicitiesVsOrbits() {
  MacroRegisterFunctionWithName("GeneralizedVermaModuleCharacters::CheckMultiplicitiesVsOrbits");
  this->CheckInitialization();
  std::stringstream out;
  int totalDimAffine = this->WeylLarger->GetDim() + this->WeylSmaller->GetDim();
  int smallDim = this->WeylSmaller->GetDim();
  Vector<Rational> normal;
  normal.MakeZero(totalDimAffine + 1);
  Vectors<Rational> newWalls;
  ConeComplex tempComplex;
  tempComplex = this->projectivizedChambeR;
  for (int i = 0; i < this->WeylChamberSmallerAlgebra.Normals.size; i ++) {
    for (int j = 0; j < smallDim; j ++) {
      normal[j] = this->WeylChamberSmallerAlgebra.Normals[i][j];
    }
    newWalls.AddOnTop(normal);
    tempComplex.splittingNormals.AddOnTop(normal);
  }
  tempComplex.indexLowestNonRefinedChamber = 0;
  tempComplex.Refine();
  out << "Number chambers with new walls: " << tempComplex.size;
  out << "\n" << tempComplex.ToString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::IncrementComputation(Vector<Rational>& parabolicSel) {
  std::stringstream out;
  this->thePauseControlleR.InitComputation();
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected = parabolicSel;
  if ((false)) {
    if (this->UpperLimitChambersForDebugPurposes == 0 || this->theLinearOperators.size == 0) {
      this->ReadFromDefaultFile();
    }
  }
  switch (this->computationPhase) {
    case 0:
//      this->theParser.theHmm.MakeG2InB3(this->theParser);
      this->initFromHomomorphism(parabolicSel, this->theHmm);
      this->TransformToWeylProjectiveStep1();
//      out << global.theIndicatorVariables.StatusString1;
      this->TransformToWeylProjectiveStep2();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 1:
      this->projectivizedChambeR.Refine();
      this->SortMultiplicities();
      out << this->projectivizedChambeR.ToString(false);
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 2:
      this->ComputeQPsFromChamberComplex();
      out << this->ElementToStringMultiplicitiesReport();
      break;
    case 3:
//      out << this->CheckMultiplicitiesVsOrbits();
      break;
    case 4:
      this->InitTheMaxComputation();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 5:
      this->theMaxComputation.FindExtremaParametricStep1(this->thePauseControlleR);
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 6:
      this->theMaxComputation.FindExtremaParametricStep3();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 7:
      this->theMaxComputation.FindExtremaParametricStep4();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    case 8:
      this->theMaxComputation.FindExtremaParametricStep5();
//      out << global.theIndicatorVariables.StatusString1;
      break;
    default:
      break;
  }
  this->computationPhase ++;
  if (this->computationPhase > 8) {
  }
  if (this->UpperLimitChambersForDebugPurposes <= 0) {
    if (this->computationPhase < 30) {
      this->WriteToDefaultFile();
    }
  }
  this->thePauseControlleR.ExitComputation();
}

void GeneralizedVermaModuleCharacters::WriteToFile(std::fstream& output) {
  output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "ComputationPhase: " << this->computationPhase << "\n";
  output << "NumProcessedConesParam: " << this->NumProcessedConesParam << "\n";
  output << "NumProcessedExtremaEqualOne: " << this->NumProcessedExtremaEqualOne << "\n";
  output << "NormalConstAdjustment: ";
  this->NonIntegralOriginModificationBasisChanged.WriteToFile(output);
  output << "\n";
  output << "ChamberIndicatorHighestWeightLargerAlgebra: ";
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected.WriteToFile(output);
  this->ParabolicSelectionSmallerAlgebra.WriteToFile(output);
  output << "\n";
  this->WeylLarger->WriteToFile(output);
  this->WeylSmaller->WriteToFile(output);
  this->preferredBasiS.WriteToFile(output);
  this->preferredBasisChangE.WriteToFile(output);
  this->preferredBasisChangeInversE.WriteToFile(output);
  this->theExtendedIntegralLatticeMatForM.WriteToFile(output);
  ProgressReport theReport(1, GlobalVariables::Progress::ReportType::fileInputOutput);
  this->theMaxComputation.WriteToFile(output);
  this->GmodKnegativeWeightS.WriteToFile(output);
  this->GmodKNegWeightsBasisChanged.WriteToFile(output);
  this->theLinearOperators.WriteToFile(output);
  this->theLinearOperatorsExtended.WriteToFile(output);
  this->PreimageWeylChamberLargerAlgebra.WriteToFile(output);
  this->PreimageWeylChamberSmallerAlgebra.WriteToFile(output);
  this->WeylChamberSmallerAlgebra.WriteToFile(output);
  if (theReport.TickAndWantReport()) {
    theReport.Report("Writing QP's non-subbed... ");
  }
  this->theQPsNonSubstituted.WriteToFile(output);
  if (theReport.TickAndWantReport()) {
    theReport.Report("Writing QP's subbed... ");
  }
  output << XML::GetOpenTagNoInputCheckAppendSpacE("QPsSubbed");
  this->theQPsSubstituted.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("QPsSubbed");
  if (theReport.TickAndWantReport()) {
    theReport.Report("Writing small data... ");
  }
  output << XML::GetOpenTagNoInputCheckAppendSpacE("theMultiplicities");
  this->theMultiplicities.WriteToFile(output, this->UpperLimitChambersForDebugPurposes);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theMultiplicities");
  //  this->theMultiplicitiesExtremaCandidates.WriteToFile(output);
  this->theCoeffs.WriteToFile(output);
  this->theTranslationS.WriteToFile(output);
  this->theTranslationsProjectedBasisChanged.WriteToFile(output);
  this->thePfs.WriteToFile(output);
  //  this->paramSubChambers.WriteToFile(output);
  //  this->nonParamVertices.WriteToFile(output);
  if (theReport.TickAndWantReport()) {
    theReport.Report("Writing param chamber complex... ");
  }
  this->projectivizedParamComplex.WriteToFile(output);
  if (theReport.TickAndWantReport()) {
    theReport.Report("Writing projectivized chamber complex... ");
  }
  this->smallerAlgebraChamber.WriteToFile(output, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.WriteToFile(output, this->UpperLimitChambersForDebugPurposes);
  if (theReport.TickAndWantReport()) {
    theReport.Report("Writing to file done...");
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

GeneralizedVermaModuleCharacters::GeneralizedVermaModuleCharacters() {
  this->UpperLimitChambersForDebugPurposes = - 1;
  this->computationPhase = 0;
  this->NumProcessedConesParam = 0;
  this->NumProcessedExtremaEqualOne = 0;
  this->numNonZeroMults = 0;
  this->WeylLarger = nullptr;
  this->WeylSmaller = nullptr;
}

bool GeneralizedVermaModuleCharacters::CheckInitialization() const {
  if (this->WeylLarger == nullptr || this->WeylSmaller == nullptr) {
    global.fatal << "Use of non-initialized Weyl group within generalized Verma module characters. " << global.fatal;
  }
  if (this->WeylLarger->flagDeallocated || this->WeylSmaller->flagDeallocated) {
    global.fatal << "Use after free of Weyl group within Verma module characters. " << global.fatal;
  }
  return true;
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism(
  Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input
) {
  MacroRegisterFunctionWithName("GeneralizedVermaModuleCharacters::initFromHomomorphism");
  Vectors<Rational> tempRoots;
  this->WeylLarger = &input.theRange().theWeyl;
  this->WeylSmaller = &input.theDomain().theWeyl;
  WeylGroupData& theWeYl = input.theRange().theWeyl;
//  input.ProjectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots);
  this->log << "projections: " << tempRoots.ToString();
  theWeYl.theGroup.ComputeAllElements(false);
  this->NonIntegralOriginModificationBasisChanged ="(1/2,1/2)";
  Matrix<Rational> theProjectionBasisChanged;
  Vector<Rational> startingWeight, projectedWeight;
  FormatExpressions theFormat;
  global.fatal << "Not implemented. " << global.fatal;
//  SSalgebraModuleOld tempM;
  input.ComputeHomomorphismFromImagesSimpleChevalleyGenerators();
  global.fatal << "Not implemented. " << global.fatal;
//  tempM.InduceFromEmbedding(tempStream, input);
  input.GetWeightsGmodKInSimpleCoordsK(this->GmodKnegativeWeightS);
//  this->log << "weights of g mod k: " << this->GmodKnegativeWeights.ToString();
  Matrix<Rational> tempMat;
  tempMat = input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert();
//  tempMat.ActOnVectorsColumn(this->GmodKnegativeWeightS);
  this->log << this->GmodKnegativeWeightS.ToString();
  this->preferredBasiS.SetSize(2);
  this->preferredBasiS[0] = - this->GmodKnegativeWeightS[1];
  this->preferredBasiS[1] = - this->GmodKnegativeWeightS[2];
  /////////////////////////////////////////
  //this->preferredBasiS[0] ="(1,0)";
  //this->preferredBasiS[1] ="(0,1)";
  ////////////////////////////////////////
  this->preferredBasisChangE.AssignVectorsToRows(this->preferredBasiS);
  this->preferredBasisChangE.Transpose();
  this->preferredBasisChangeInversE = this->preferredBasisChangE;
  this->preferredBasisChangeInversE.Invert();
  this->preferredBasisChangeInversE.ActOnVectorsColumn
  (this->GmodKnegativeWeightS, this->GmodKNegWeightsBasisChanged);
  this->log << "\nWeights after basis change: " << this->GmodKNegWeightsBasisChanged.ToString();
  for (int i = 0; i < this->GmodKnegativeWeightS.size; i ++) {
    if (this->GmodKnegativeWeightS[i].IsPositiveOrZero()) {
      this->GmodKnegativeWeightS.RemoveIndexSwapWithLast(i);
      i --;
    }
  }
  for (int i = 0; i < this->GmodKNegWeightsBasisChanged.size; i ++) {
    if (this->GmodKNegWeightsBasisChanged[i].IsPositiveOrZero()) {
      this->GmodKNegWeightsBasisChanged.RemoveIndexSwapWithLast(i);
      i --;
    }
  }
  this->log << "\nNegative weights after basis change: " << this->GmodKNegWeightsBasisChanged.ToString();
  theProjectionBasisChanged.init(input.theDomain().GetRank(), input.theRange().GetRank());
  for (int i = 0; i < input.theRange().GetRank(); i ++) {
    startingWeight.MakeEi(input.theRange().GetRank(), i);
    input.ProjectOntoSmallCartan(startingWeight, projectedWeight);
    this->preferredBasisChangeInversE.ActOnVectorColumn(projectedWeight);
    for (int j = 0; j < projectedWeight.size; j ++) {
      theProjectionBasisChanged.elements[j][i] = projectedWeight[j];
    }
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected = theParabolicSel;
  Matrix<Rational> DualCartanEmbedding;
  input.GetMapSmallCartanDualToLargeCartanDual(DualCartanEmbedding);
  Vector<Rational> ParabolicEvaluationRootImage, tempRoot;
  ParabolicEvaluationRootImage = this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
  this->ParabolicSelectionSmallerAlgebra.init(input.theDomain().GetRank());
  for (int i = 0; i < input.theDomain().GetRank(); i ++) {
    DualCartanEmbedding.GetVectorFromColumn(i, tempRoot);
    if (ParabolicEvaluationRootImage.ScalarEuclidean(tempRoot).IsPositive()) {
      this->ParabolicSelectionSmallerAlgebra.AddSelectionAppendNewIndex(i);
    }
  }
  this->log << "\nDual cartan embedding smaller into larger:\n" <<
  DualCartanEmbedding.ToString(&global.theDefaultFormat.GetElement());
  this->log << "\nParabolic subalgebra large algebra: " << this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ToString();
  tempRoot = this->ParabolicSelectionSmallerAlgebra;
  this->log << "\nParabolic subalgebra smaller algebra: " << tempRoot.ToString();
  theSubgroup.MakeParabolicFromSelectionSimpleRoots(theWeYl, this->ParabolicLeviPartRootSpacesZeroStandsForSelected, - 1);

  this->theLinearOperators.SetSize(theSubgroup.allElements.size);
  this->theLinearOperatorsExtended.SetSize(theSubgroup.allElements.size);
  this->theTranslationS.SetSize(theSubgroup.allElements.size);
  this->theTranslationsProjectedBasisChanged.SetSize(theSubgroup.allElements.size);
  this->theCoeffs.SetSize(theSubgroup.allElements.size);
  this->log << " \n******************\nthe subgroup: \n" << theSubgroup.ToString() << "\n\n\n\n\n\n";
  this->log << theSubgroup.ElementToStringBruhatGraph();
  this->log << "\nMatrix form of the elements of Weyl group of the Levi part of the parabolic ("
  << theSubgroup.allElements.size << " elements):\n";
  for (int i = 0; i < theSubgroup.allElements.size; i ++) {
    Matrix<Rational>& currentLinearOperator = this->theLinearOperators[i];
    theSubgroup.GetMatrixOfElement(theSubgroup.allElements[i], currentLinearOperator);
//    currentLinearOperator.MultiplyOnTheLeft(preferredBasisChangeInverse);
    this->log << "\n" << currentLinearOperator.ToString(&global.theDefaultFormat.GetElement());
    currentLinearOperator.ActOnVectorColumn(theSubgroup.GetRho(), this->theTranslationS[i]);
    this->theTranslationS[i] -= theSubgroup.GetRho();
    this->theTranslationS[i].Minus();
    theProjectionBasisChanged.ActOnVectorColumn(this->theTranslationS[i], this->theTranslationsProjectedBasisChanged[i]);
    if (theSubgroup.allElements[i].generatorsLastAppliedFirst.size % 2 == 0) {
      this->theCoeffs[i] = 1;
    } else {
      this->theCoeffs[i] = - 1;
    }
  }
  this->log << "\n\n\nMatrix of the projection operator (basis-changed):\n"
  << theProjectionBasisChanged.ToString(&global.theDefaultFormat.GetElement());
  this->log << "\n\n\nMatrix form of the operators $u_w$, "
  << "the translations $\tau_w$ and their projections (" << this->theLinearOperatorsExtended.size << "):";
  //List<Matrix<Rational> > tempList;
  for (int k = 0; k < this->theLinearOperators.size; k ++) {
    Matrix<Rational>& currentLO = this->theLinearOperators[k];
    Matrix<Rational>& currentLOExtended = this->theLinearOperatorsExtended[k];
    currentLO.MultiplyOnTheLeft(theProjectionBasisChanged);
    currentLO *= - 1;
    //tempList.AddOnTopNoRepetition(this->theLinearOperators.TheObjects[i]);
    currentLOExtended.MakeIdMatrix(currentLO.NumRows);
    currentLOExtended.Resize(currentLO.NumRows, currentLO.NumRows + currentLO.NumCols, true);
    for (int i = 0; i < currentLO.NumRows; i ++) {
      for (int j = 0; j < currentLO.NumCols; j ++) {
        currentLOExtended.elements[i][j + currentLO.NumRows] = currentLO.elements[i][j];
      }
    }
    this->log << "\n\n" << currentLOExtended.ToString(&global.theDefaultFormat.GetElement());
    this->log << this->theTranslationS[k].ToString() << ";   " << this->theTranslationsProjectedBasisChanged[k].ToString();
  }

  List<int> displayIndicesReflections;
  for (int i = 0; i < this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i ++) {
    if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i]) {
      displayIndicesReflections.AddOnTop(i + 1);
    }
  }
  Matrix<Polynomial<Rational> > tempMatPoly;
  Vector<Polynomial<Rational> > tempVect, tempVect2;
  tempVect.SetSize(input.theDomain().theWeyl.GetDim() + input.theRange().theWeyl.GetDim());
  for (int i = 0; i < tempVect.size; i ++) {
    tempVect[i].MakeMonomiaL(i, 1, Rational(1), tempVect.size);
  }
  tempMatPoly.init(input.theDomain().theWeyl.GetDim(), tempVect.size);
  Polynomial<Rational> polyZero;
  polyZero.MakeZero();
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[0] = "x_1";
  theFormat.polyAlphabeT[1] = "x_2";
  theFormat.polyAlphabeT[2] = "y_1";
  theFormat.polyAlphabeT[3] = "y_2";
  theFormat.polyAlphabeT[4] = "y_3";
  tempRoot = theSubgroup.GetRho();
  this->theLinearOperators[0].ActOnVectorColumn(tempRoot);
  this->preferredBasisChangE.ActOnVectorColumn(tempRoot);
  tempRoot.Minus();
  this->log << "\n\nIn $so(7)$-simple basis coordinates, $\\rho_{\\mathfrak l}="
  << theSubgroup.GetRho().ToStringLetterFormat("\\eta") << "$; $\\pr(\\rho)="
  << tempRoot.ToStringLetterFormat("\\alpha") << "$.";
  this->log << "\n\n\\begin{longtable}{r|l}$w$ & \\begin{tabular}{c}"
  << "Argument of the vector partition function in (\\ref{eqMultG2inB3General}) =\\\\ $u_w\\circ"
  << tempVect.ToString(&theFormat) << "-\\tau_w$ \\end{tabular}  \\\\ \\hline \\endhead";
  for (int i = 0; i < this->theLinearOperatorsExtended.size; i ++) {
    Matrix<Rational>& currentLoExt = this->theLinearOperatorsExtended[i];
    for (int j = 0; j < currentLoExt.NumRows; j ++) {
      for (int k = 0; k < currentLoExt.NumCols; k ++) {
        tempMatPoly.elements[j][k].MakeConst(currentLoExt.elements[j][k], tempVect.size);
      }
    }
    tempMatPoly.ActOnVectorColumn(tempVect, tempVect2, polyZero);
    for (int j = 0; j < tempVect2.size; j ++) {
      tempVect2[j] += this->theTranslationsProjectedBasisChanged[i][j];
    }
    this->log << "\n$" << theSubgroup.allElements[i].ToString() << "$&$"
    << tempVect2.ToString(&theFormat) << "$\\\\";
  }
  this->log << "\\end{longtable}\n\n";
//  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
  Lattice tempLattice;
  theWeYl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.MakeIdMatrix(input.theDomain().GetRank());
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.DirectSumWith(tempLattice.basisRationalForm, Rational(0));
  this->theExtendedIntegralLatticeMatForM.MakeFromMat(this->theExtendedIntegralLatticeMatForM.basisRationalForm);
  tempMat = theWeYl.CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> WallsWeylChamberLargerAlgebra;
  for (int i = 0; i < tempMat.NumRows; i ++) {
    tempMat.GetVectorFromRow(i, tempRoot);
    if (ParabolicEvaluationRootImage[i].IsEqualToZero()) {
      WallsWeylChamberLargerAlgebra.SetSize(WallsWeylChamberLargerAlgebra.size + 1);
      *WallsWeylChamberLargerAlgebra.LastObject() = tempRoot;
    }
  }
  this->log << "\n\n\n**************\nParabolic selection larger algebra:"
  << ParabolicEvaluationRootImage.ToString() << "\nWalls Weyl chamber larger algebra: "
  << WallsWeylChamberLargerAlgebra.ToString();
  this->log << "\n**************\n\n";
  Vectors<Rational> rootsGeneratingExtendedLattice;
  int totalDim = input.theRange().GetRank() + input.theDomain().GetRank();
  rootsGeneratingExtendedLattice.SetSize(totalDim);
  this->log << "\n" << tempMat.ToString(&global.theDefaultFormat.GetElement()) << "\n";
  this->log << this->theExtendedIntegralLatticeMatForM.ToString();
  this->WeylChamberSmallerAlgebra.CreateFromNormals(WallsWeylChamberLargerAlgebra);
  this->log << "\nWeyl chamber larger algebra before projectivizing: " << this->WeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->PreimageWeylChamberSmallerAlgebra.Normals = this->WeylChamberSmallerAlgebra.Normals;
  for (int i = 0; i < this->PreimageWeylChamberLargerAlgebra.Normals.size; i ++) {
    tempRoot.MakeZero(input.theRange().GetRank() + input.theDomain().GetRank() + 1);
    for (int j = 0; j < input.theRange().GetRank(); j ++) {
      tempRoot[j + input.theDomain().GetRank()] = this->PreimageWeylChamberLargerAlgebra.Normals[i][j];
    }
    this->PreimageWeylChamberLargerAlgebra.Normals[i] = tempRoot;
  }
  tempMat = input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert();
  tempRoots.size = 0;
  Vector<Rational> ParabolicEvaluationRootSmallerAlgebra;
  ParabolicEvaluationRootSmallerAlgebra = this->ParabolicSelectionSmallerAlgebra;
  for (int i = 0; i < tempMat.NumRows; i ++) {
    input.theDomain().theWeyl.CartanSymmetric.GetVectorFromRow(i, tempRoot);
    if (tempRoot.ScalarEuclidean(ParabolicEvaluationRootSmallerAlgebra).IsEqualToZero()) {
      tempRoots.SetSize(tempRoots.size + 1);
      tempMat.GetVectorFromRow(i, *tempRoots.LastObject());
    }
  }
  this->preferredBasisChangeInversE.ActOnVectorsColumn(tempRoots);
  this->log << "**********************\n\n\n";
  this->log << "\nthe smaller parabolic selection: " << this->ParabolicSelectionSmallerAlgebra.ToString();
  this->log << "the Vectors<Rational> generating the chamber walls: " << tempRoots.ToString();
  this->PreimageWeylChamberSmallerAlgebra.CreateFromVertices(tempRoots);
  this->log << "\nWeyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->log << "**********************\n\n\n";
  this->log << "\nThe first operator extended:\n"
  << this->theLinearOperatorsExtended[0].ToString(&global.theDefaultFormat.GetElement()) << "\n";
  this->log << "\nThe second operator extended:\n"
  << this->theLinearOperatorsExtended[1].ToString(&global.theDefaultFormat.GetElement()) << "\n";
  /*tempMat = this->theLinearOperatorsExtended.TheObjects[0];
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(this->PreimageWeylChamberSmallerAlgebra);
  for (int i = 0; i < this->PreimageWeylChamberSmallerAlgebra.size; i ++) {
    this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].SetSize(input.theRange.GetRank() + input.theDomain.GetRank() + 1);
    *this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].LastObject()= 0;
  }*/
  for (int i = 0; i < this->PreimageWeylChamberSmallerAlgebra.Normals.size; i ++) {
    tempRoot.MakeZero(input.theRange().GetRank() + input.theDomain().GetRank() + 1);
    for (int j = 0; j < input.theDomain().GetRank(); j ++) {
      tempRoot[j] = this->PreimageWeylChamberSmallerAlgebra.Normals[i][j];
    }
  //  for (int j = 0; j < input.theRange.GetRank(); j ++)
   //   tempRoot.TheObjects[j+ input.theDomain.GetRank()] = tempRoot2.TheObjects[j];
    this->PreimageWeylChamberSmallerAlgebra.Normals[i] = tempRoot;
  }

  tempRoot.MakeEi(
    input.theRange().GetRank() + input.theDomain().GetRank() + 1,
    input.theRange().GetRank() + input.theDomain().GetRank()
  );
  this->PreimageWeylChamberLargerAlgebra.Normals.AddOnTop(tempRoot);
  this->log << "\nPreimage Weyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->log << "\nPreimage Weyl chamber larger algebra: " << this->PreimageWeylChamberLargerAlgebra.ToString(&theFormat) << "\n";

  //global.theIndicatorVariables.StatusString1NeedsRefresh = true;
  //global.theIndicatorVariables.StatusString1= this->log.str();
  //global.MakeReport();
}

std::string GeneralizedVermaModuleCharacters::PrepareReport() {
  std::stringstream out;
  FormatExpressions theFormat;
  int tempI = 0;
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[tempI] = "x_1";
  tempI ++;
  theFormat.polyAlphabeT[tempI] = "x_2";
  tempI ++;
  theFormat.polyAlphabeT[tempI] = "y_1";
  tempI ++;
  theFormat.polyAlphabeT[tempI] = "y_2";
  tempI ++;
  theFormat.polyAlphabeT[tempI] = "y_3";
  tempI ++;
  out << "\\documentclass{article}\\usepackage{amsmath, longtable, amsfonts, amssymb, verbatim, hyperref}"
  << "\n\\begin{document}\\tiny\n";
  out << "\n The chamber complex + multiplicities follow.\n\n\n"
  << "\\begin{longtable}{cc}\\caption{Multiplicities of generalized Verma modules $m(x_1,x_2, y_1, y_2, y_3)$"
  << " for $\\gop$ with Dynkin diagram";
  std::stringstream tempStream;
  tempStream << "(";
  for (int i = 0; i < this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i ++) {
    if (this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i]) {
      tempStream << "+";
    } else {
      tempStream << "0";
    }
    if (i != this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize - 1) {
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
    if (!theMult.IsEqualToZero()) {
      numFoundChambers ++;
      out << "\\hline\\multicolumn{2}{c}{Chamber " << numFoundChambers << "}\\\\\n";
      DisplayIndicesprojectivizedChambers.AddOnTop(numFoundChambers);
      out << this->PrepareReportOneCone(theFormat, this->projectivizedChambeR[i]) << "&";
      out << "\\begin{tabular}{c}";
      out << theMult.ToString(false, true, &theFormat) << "\\end{tabular}\\\\\n";
    } else {
      DisplayIndicesprojectivizedChambers.AddOnTop(- 1);
    }
  }
  out << "\\end{longtable}\n\n\n Multiplicity free chambers \n";
  numFoundChambers = 0;
  out << "\n\\begin{longtable}{cc} ";
  out << "Normals& Multiplicity of module with highest weight $(x_1,x_2)$\\endhead\n";
 /* for (int i = 0; i < this->projectivezedChambersSplitByMultFreeWalls.size; i ++) {
    tempRoot = this->projectivezedChambersSplitByMultFreeWalls.TheObjects[i].GetInternalPoint();
    bool found = false;
    for (int j = 0; j < this->projectivizedChamber.size; j ++)
      if (this->projectivizedChamber.TheObjects[j].IsInCone(tempRoot)) {
        if (found)
          global.fatal << global.fatal;
        found = true;
      }
  }
  for (int i = 0; i < this->projectivizedChamber.size; i ++) {
    QuasiPolynomial& theMult = this->theMultiplicities.TheObjects[i];
    if (!theMult.IsEqualToZero()) {
      int indexMultFreeChamber = - 1;
      for (int j = 0; j < this->projectivezedChambersSplitByMultFreeWalls.size; j ++) {
        tempRoot = this->projectivezedChambersSplitByMultFreeWalls.TheObjects[j].GetInternalPoint();
        if (this->projectivizedChamber.TheObjects[i].IsInCone(tempRoot)) {
          Rational tempRat;
          tempRat =*tempRoot.LastObject();
          if (tempRat != 0)
            tempRoot/= tempRat;
          theMult.valueOnEachLatticeShift.TheObjects[0].Evaluate(tempRoot, tempRat);
          if (tempRat<1) {
            indexMultFreeChamber = j;
            break;
          }
        }
      }
      if (indexMultFreeChamber!= - 1) {
        numFoundChambers++;
        out << "\\hline\\multicolumn{2}{c}{Chamber " << DisplayIndicesprojectivizedChambers.TheObjects[i] << "}\\\\\n";
        out << this->PrepareReportOneCone(theFormat, this->projectivezedChambersSplitByMultFreeWalls.TheObjects[indexMultFreeChamber]) << "&";
        out << theMult.ToString(false, true, theFormat) << "\\\\\n";
      }
    }
  }*/
  out << "Total number of chambers with multiplicity 1 or less: " << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

void GeneralizedVermaModuleCharacters::InitTheMaxComputation() {
  MacroRegisterFunctionWithName("GeneralizedVermaModuleCharacters::InitTheMaxComputation");
  this->theMaxComputation.numNonParaM = 2;
  this->theMaxComputation.theConesLargerDim.Reserve(this->projectivizedChambeR.size);
  this->theMaxComputation.LPtoMaximizeLargerDim.Reserve(this->theMultiplicities.size);
  this->theMaxComputation.theConesLargerDim.SetSize(0);
  this->theMaxComputation.LPtoMaximizeLargerDim.SetSize(0);
  Lattice ZnLattice;
  int theAffineDim = 5;
//  int theProjectivizedDim= theAffineDim + 1;
  ZnLattice.MakeZn(theAffineDim);
  this->numNonZeroMults = 0;
  ProgressReport theReport;
  ConeLatticeAndShift currentCLS;
  Vector<Rational> theLPtoMax;
  for (int i = 0; i < this->theMultiplicities.size; i ++) {
    if (! this->theMultiplicities[i].IsEqualToZero()) {
      currentCLS.theProjectivizedCone = this->projectivizedChambeR[i];
      currentCLS.theShift.MakeZero(theAffineDim);
      currentCLS.theLattice = ZnLattice;
      bool tempBool = this->theMultiplicities[i].valueOnEachLatticeShift[0].GetRootFromLinPolyConstTermLastVariable(theLPtoMax);
      if (!tempBool) {
        global.fatal << "This should not happen. " << global.fatal;
      }
      this->theMaxComputation.theConesLargerDim.AddOnTop(currentCLS);
      this->theMaxComputation.LPtoMaximizeLargerDim.AddOnTop(theLPtoMax);
      this->numNonZeroMults ++;
      std::stringstream out;
      out << "Initialized " << i + 1 << " out of " << this->theMaxComputation.theConesLargerDim.size
      << "; so far " << this->numNonZeroMults << " non-zero multiplicities";
      theReport.Report(out.str());
    }
  }
}

std::string GeneralizedVermaModuleCharacters::PrepareReportOneCone(FormatExpressions& theFormat, const Cone& theCone) {
  std::stringstream out1;
  Vector<Rational> normalNoConstant;
  int dimSmallerAlgebra = this->theLinearOperators[0].NumRows;
  int dimLargerAlgebra = this->theLinearOperators[0].NumCols;
  Rational theConst;
  out1 << "\\begin{tabular}{rcl}";
  for (int i = 0; i < theCone.Normals.size; i ++) {
    Vector<Rational>& currentNormal = theCone.Normals[i];
    normalNoConstant = currentNormal;
    normalNoConstant.SetSize(dimSmallerAlgebra + dimLargerAlgebra);
    theConst = - (*currentNormal.LastObject());
    if (!normalNoConstant.IsEqualToZero()) {
      out1 << "$" << normalNoConstant.ToStringLetterFormat("x", &theFormat) << "$ & $\\geq$ & $"
      << theConst.ToString() << "$ \\\\";
    }
  }
  out1 << "\\end{tabular}";
  return out1.str();
}

bool GeneralizedVermaModuleCharacters::ReadFromFileNoComputationPhase(std::fstream& input) {
  std::string tempS;
  ProgressReport theReport;
  input >> tempS >> this->NumProcessedConesParam;
  input >> tempS >> this->NumProcessedExtremaEqualOne;
  input >> tempS;
  int numReadWords;
  if (tempS != "NormalConstAdjustment:") {
    global.fatal << "Unexpected identifier while reading from file. " << global.fatal;
    return false;
  }
  this->NonIntegralOriginModificationBasisChanged.ReadFromFile(input);
  input >> tempS;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ReadFromFile(input);
  this->ParabolicSelectionSmallerAlgebra.ReadFromFile(input);
  this->WeylLarger->ReadFromFile(input);
  this->WeylSmaller->ReadFromFile(input);
  this->preferredBasiS.ReadFromFile(input);
  this->preferredBasisChangE.ReadFromFile(input);
  this->preferredBasisChangeInversE.ReadFromFile(input);
  this->theExtendedIntegralLatticeMatForM.ReadFromFile(input);
  theReport.Report("Loading param subchambers cone form... ");
  this->theMaxComputation.ReadFromFile(input, this->UpperLimitChambersForDebugPurposes);
  theReport.Report("Loading more pieces of data... ");
  this->GmodKnegativeWeightS.ReadFromFile(input);
  this->GmodKNegWeightsBasisChanged.ReadFromFile(input);
  this->theLinearOperators.ReadFromFile(input);
  this->theLinearOperatorsExtended.ReadFromFile(input);
  this->PreimageWeylChamberLargerAlgebra.ReadFromFile(input);
  this->PreimageWeylChamberSmallerAlgebra.ReadFromFile(input);
  this->WeylChamberSmallerAlgebra.ReadFromFile(input);
  this->theQPsNonSubstituted.ReadFromFile(input);
  XML::ReadThroughFirstOpenTag(input, numReadWords, "QPsSubbed");
  this->theQPsSubstituted.ReadFromFile(input);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "QPsSubbed");
  theReport.Report("Loading multiplicities... ");
  XML::ReadThroughFirstOpenTag(input, numReadWords, "theMultiplicities");
  this->theMultiplicities.ReadFromFile(input, this->UpperLimitChambersForDebugPurposes);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "theMultiplicities");
  this->theCoeffs.ReadFromFile(input);
  this->theTranslationS.ReadFromFile(input);
  this->theTranslationsProjectedBasisChanged.ReadFromFile(input);
  theReport.Report("Loading partial fractions... ");
//  this->thePfs.ReadFromFile(input);
  theReport.Report("Loading projectivized param complex... ");
  this->projectivizedParamComplex.ReadFromFile(input);
  theReport.Report("Loading the complex... ");
  this->smallerAlgebraChamber.ReadFromFile(input, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.ReadFromFile(input, this->UpperLimitChambersForDebugPurposes);
  theReport.Report("Loading complete... ");
  return true;
}

void GeneralizedVermaModuleCharacters::ReadFromDefaultFile() {
  std::fstream input;
  if (!FileOperations::FileExistsVirtual("output/GenVermaComputation.txt")) {
    this->computationPhase = 0;
    return;
  }
  FileOperations::OpenFileCreateIfNotPresentVirtual(input, "output/GenVermaComputation.txt", false, false, false);
  this->ReadFromFile(input);
  input.close();
}

void GeneralizedVermaModuleCharacters::WriteToDefaultFile() {
  std::fstream output;
  FileOperations::OpenFileCreateIfNotPresentVirtual(output, "output/GenVermaComputation.txt", false, true, false);
  this->WriteToFile(output);
}

std::string GeneralizedVermaModuleCharacters::ElementToStringMultiplicitiesReport() {
  if (this->theMultiplicities.size != this->projectivizedChambeR.size) {
    global.fatal << "Bad number of multiplicities. " << global.fatal;
  }
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[0] = "x_1";
  theFormat.polyAlphabeT[1] = "x_2";
  theFormat.polyAlphabeT[2] = "y_1";
  theFormat.polyAlphabeT[3] = "y_2";
  theFormat.polyAlphabeT[4] = "y_3";
  out << "Number chambers: " << projectivizedChambeR.size << " of them " << this->numNonZeroMults << " non-zero.";
  int numInequalities = 0;
  for (int i = 0; i < this->projectivizedChambeR.size; i ++) {
    numInequalities += this->projectivizedChambeR[i].Normals.size;
  }
  out << "\nNumber of inequalities: " << numInequalities;
  if (this->ParabolicLeviPartRootSpacesZeroStandsForSelected.CardinalitySelection != 0) {
    out << this->PrepareReport();
  }
  return out.str();
}

void GeneralizedVermaModuleCharacters::GetSubFromNonParamArray(
  Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams
) {
  //reminder: the very last variable comes from the projectivization and contributes to the translation only!
  int numNonParams = NonParams.size;
  output.init(numParams + numNonParams - 1, numParams - 1);
  outputTranslation.MakeZero(numParams + numNonParams - 1);
  output.MakeZero();
  for (int l = 0; l < numNonParams; l ++) {
    for (int k = 0; k < numParams - 1; k ++) {
      output.elements[l][k] = NonParams[l][k];
    }
    outputTranslation[l] = *NonParams[l].LastObject();
  }
  for (int l = 0; l < numParams - 1; l ++) {
    output.elements[l + numNonParams][l] = 1;
  }
}

void GeneralizedVermaModuleCharacters::GetProjection(
  int indexOperator, const Vector<Rational>& input, Vector<Rational>& output
) {
  Matrix<Rational>& currentExtendedOperator = this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& currentTranslation = this->theTranslationsProjectedBasisChanged[indexOperator];
  if (input.LastObject()->IsEqualToZero()) {
    global.fatal << "Last coordinate is not supposed to be be zero. " << global.fatal;
  }
  output = input;
  Rational tempRat = *output.LastObject();
  output /= tempRat;
  output.size --;
  currentExtendedOperator.ActOnVectorColumn(output);
  output += currentTranslation;
}

void GeneralizedVermaModuleCharacters::GetSubFromIndex(
  PolynomialSubstitution<Rational>& outputSub, Matrix<LargeInteger>& outputMat, LargeIntegerUnsigned& outputDen, int theIndex
) {
  Matrix<Rational>& theOperator = this->theLinearOperators[theIndex];
  int dimLargerAlgebra = theOperator.NumCols;
  int dimSmallerAlgebra = theOperator.NumRows;
  Vector<Rational>& theTranslation = this->theTranslationS[theIndex];
  Matrix<Rational> tempMat;
  tempMat.init(dimLargerAlgebra + dimSmallerAlgebra + 1, dimSmallerAlgebra);
  tempMat.MakeZero();
  for (int j = 0; j < dimSmallerAlgebra; j ++) {
    tempMat.elements[j][j] = 1;
    for (int i = 0; i < dimLargerAlgebra; i ++) {
      tempMat.elements[i][j] -= theOperator.elements[j][i];
    }
    tempMat.elements[dimLargerAlgebra + dimSmallerAlgebra][j] = - theTranslation[j];
  }
  tempMat.GetMatrixIntWithDen(outputMat, outputDen);
  outputSub.MakeSubFromMatrixIntAndDen(outputMat, outputDen);
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective(
  int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal
) {
  Matrix<Rational> theOperatorExtended = this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& theTranslation = this->theTranslationsProjectedBasisChanged[indexOperator];
  //the goddamned sign in front of theTranslation is now checked: it should be + and not -
  Rational theConst;
  startingNormal.ScalarEuclidean(this->NonIntegralOriginModificationBasisChanged + theTranslation, theConst);
  theOperatorExtended.Transpose();
  outputNormal = startingNormal;
  theOperatorExtended.ActOnVectorColumn(outputNormal);
  outputNormal.SetSize(outputNormal.size + 1);
  *outputNormal.LastObject() = - theConst;
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep1() {
  this->smallerAlgebraChamber.InitFromDirectionsAndRefine(this->GmodKNegWeightsBasisChanged);
  ProgressReport theReport1;
  ProgressReport theReport2;
  theReport1.Report(this->smallerAlgebraChamber.ToString(false));
  this->log << "Directions for making the chamber basis changed: " << this->GmodKNegWeightsBasisChanged.ToString()
  << "\n Resulting chamber before projectivization:\n "
  << this->smallerAlgebraChamber.ToString(false);
  theReport2.Report(this->log.str());
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep2() {
  std::stringstream out;
  ConeComplex projectivizedChamberFinal;
  Cone currentProjectiveCone;
  Vectors<Rational> tempRoots;
  Vector<Rational> wallToSliceWith;
  ProgressReport theReport;
  projectivizedChamberFinal.init();
  for (int i = 0; i < this->smallerAlgebraChamber.size; i ++) {
    Cone& currentAffineCone = this->smallerAlgebraChamber.TheObjects[i];
    tempRoots.SetSize(currentAffineCone.Normals.size);
    for (int j = 0; j < currentAffineCone.Normals.size; j ++) {
      this->TransformToWeylProjective(0, currentAffineCone.Normals[j], tempRoots[j]);
    }
    tempRoots.AddListOnTop(this->PreimageWeylChamberLargerAlgebra.Normals);
    theReport.Report(tempRoots.ToString());
    currentProjectiveCone.CreateFromNormals(tempRoots);
    projectivizedChamberFinal.AddNonRefinedChamberOnTopNoRepetition(currentProjectiveCone);
  }
  for (int i = 0; i < this->PreimageWeylChamberSmallerAlgebra.Normals.size; i ++) {
    projectivizedChamberFinal.splittingNormals.AddOnTop(this->PreimageWeylChamberSmallerAlgebra.Normals[i]);
  }
  out << "projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ToString(false);
  projectivizedChamberFinal.Refine();
  out << "Refined projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ToString(false);
  for (int i = 0; i < projectivizedChamberFinal.size; i ++) {
    const Cone& currentCone = projectivizedChamberFinal[i];
    bool isNonDominant = false;
    for (int j = 0; j < this->PreimageWeylChamberSmallerAlgebra.Normals.size; j ++) {
      if (currentCone.GetInternalPoint().ScalarEuclidean(this->PreimageWeylChamberSmallerAlgebra.Normals[j]).IsNegative()) {
        isNonDominant = true;
        break;
      }
    }
    if (isNonDominant) {
      projectivizedChamberFinal.PopChamberSwapWithLast(i);
      i --;
    }
  }
  theReport.Report(out.str());
  projectivizedChamberFinal.indexLowestNonRefinedChamber = 0;
  this->projectivizedChambeR= projectivizedChamberFinal;
  for (int k = 1; k < this->theLinearOperators.size; k ++) {
    for (int i = 0; i < this->smallerAlgebraChamber.size; i ++) {
      for (int j = 0; j < this->smallerAlgebraChamber[i].Normals.size; j ++) {
        this->TransformToWeylProjective(k, this->smallerAlgebraChamber[i].Normals[j], wallToSliceWith);
        wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
        this->projectivizedChambeR.splittingNormals.AddOnTopNoRepetition(wallToSliceWith);
      }
    }
  }
  out << "projectivized chamber chopped non-dominant part:\n"  << this->projectivizedChambeR.ToString(false);
  theReport.Report(out.str());
}
