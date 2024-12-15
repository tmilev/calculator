#include "calculator_interface.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "general_time_date.h"
#include "main.h"
#include "math_large_integers.h"
#include "signals_infrastructure.h"
#include "source_code_formatter.h"
#include "string_constants.h"
#include "system_functions_global_objects.h"
#include "user.h"
#include "web_assembly.h"
#include "webserver.h"
#include <iostream>

int main(int argc, char** argv) {
  bool useWasm = false;
#ifdef MACRO_use_wasm
  useWasm = true;
#endif
  if (useWasm) {
    return WebAssembly::main(argc, argv);
  }
  return MainFunctions::main(argc, argv);
}

int MainFunctions::main(int argc, char** argv) {
  global.initialize();
  global.initThreadsExecutableStart();
  STACK_TRACE("MainFunctions::main");
  try {
    return MainFunctions::mainWithoutExceptions(argc, argv);
  } catch(...) {
    global.fatal
    << "Exception caught: something very wrong has happened. "
    << global.fatal;
  }
  global.fatal
  << "This point of code is not supposed to be reachable. "
  << global.fatal;
  return - 1;
}

bool MainFunctions::analyzeMainArgumentsTimeString(
  const std::string& timeLimitString
) {
  if (timeLimitString == "") {
    return false;
  }
  Rational timeLimit;
  if (!timeLimit.assignStringFailureAllowed(timeLimitString)) {
    return false;
  }
  int timeLimitInt = 0;
  if (timeLimit.isIntegerFittingInInt(&timeLimitInt)) {
    global.millisecondsMaxComputation = timeLimitInt;
    if (global.millisecondsMaxComputation <= 0) {
      global.millisecondsMaxComputation = 0;
    } else {
      global.millisecondsMaxComputation *= 1000;
    }
  }
  return true;
}

void MainFunctions::initializeBuildFlags() {
#ifdef MACRO_use_open_ssl
  global.flagSSLAvailable = true;
#endif
}

int MainFunctions::mainWithoutExceptions(int argc, char** argv) {
  // Initializations basic (timer, ...).
  initializeGlobalObjects();
  // Initializations of build flags.
  MainFunctions::initializeBuildFlags();
  // Process executable arguments.
  // May set the value of
  // global.PathProjectBaseUserInputOrDeduced.
  MainFunctions::analyzeMainArguments(argc, argv);
  if (global.runMode == GlobalVariables::RunMode::consoleHelp) {
    return MainFunctions::mainConsoleHelp();
  }
  // Initializes server base path
  // using global.PathProjectBaseUserInputOrDeduced.
  global.initDefaultFolderAndFileNames();
  // Ensure the server path coincides with the current
  // directory.
  global.changeDirectory(global.physicalPathProjectBase);
  // Initializes folder locations needed by logging facilities.
  FileOperations::initializeFoldersSensitive();
  global
  << Logger::green
  << "Project base folder: "
  << Logger::blue
  << global.physicalPathProjectBase
  << Logger::endL;
  global
  << Logger::green
  << "Current folder: "
  << Logger::blue
  << FileOperations::getCurrentFolder()
  << Logger::endL;
  // compute configuration file location.
  // load the configuration file.
  global.configurationLoad();
  // compute various flags and settings from the desired configuration.
  // Correct bad configuration settings if any.
  global.configurationProcess();
  // Store back the config file if it changed.
  global.configurationStore();
  if (global.millisecondsMaxComputation > 0) {
    global.millisecondsNoPingBeforeChildIsPresumedDead =
    global.millisecondsMaxComputation + 20000;
    // + 20 seconds
  } else {
    global.millisecondsNoPingBeforeChildIsPresumedDead = - 1;
  }
  // Uses the configuration file.
  // Calls again global.server().InitializeMainFoldersSensitive();
  global.server().initializeMainAll();
  global
  << "Computation timeout: "
  << Logger::red
  << global.millisecondsMaxComputation
  << " ms."
  << Logger::endL;
  switch (global.runMode) {
  case GlobalVariables::RunMode::daemonMonitor:
    return global.server().daemon();
  case GlobalVariables::RunMode::builtInWebServer:
    return global.server().run();
  case GlobalVariables::RunMode::consoleTest:
    return MainFunctions::mainTest(global.programArguments);
  case GlobalVariables::RunMode::deploy:
    return MainFunctions::mainDeploy();
  case GlobalVariables::RunMode::formatCode:
    return MainFunctions::mainFormat();
  case GlobalVariables::RunMode::loadDatabase:
    return MainFunctions::mainLoadDatabase();
  default:
    return MainFunctions::mainCommandLine();
  }
}

std::string MainFlags::server = "server";
std::string MainFlags::format = "format";
std::string MainFlags::daemon = "daemon";
std::string MainFlags::help = "help";
std::string MainFlags::basePath = "base_path";
std::string MainFlags::configurationFile = "configuration_file";
std::string MainFlags::test = "test";
std::string MainFlags::loadDatabase = "load_database";
std::string MainFlags::deploy = "deploy";
class ArgumentAnalyzer {
public:
  int currentIndex;
  HashedList<std::string> commandLineConfigurations;
  bool processOneArgument();
  bool setFormat();
  bool setServer();
  bool setTest();
  bool setBasePath();
  bool setConfigurationFile();
  bool setLoadDatabase();
  bool setDeploy();
  bool processCommandLineConfigurations(std::string& input);
};

bool ArgumentAnalyzer::setFormat() {
  global.runMode = GlobalVariables::RunMode::formatCode;
  return false;
}

bool ArgumentAnalyzer::setServer() {
  std::string timeLimitString;
  global.runMode = GlobalVariables::RunMode::builtInWebServer;
  if (this->currentIndex + 1 < global.programArguments.size) {
    timeLimitString = global.programArguments[this->currentIndex + 1];
    if (MainFunctions::analyzeMainArgumentsTimeString(timeLimitString)) {
      this->currentIndex ++;
    }
  }
  return true;
}

bool ArgumentAnalyzer::setLoadDatabase() {
  global.runMode = GlobalVariables::RunMode::loadDatabase;
  return true;
}

bool ArgumentAnalyzer::setDeploy() {
  global.runMode = GlobalVariables::RunMode::deploy;
  return true;
}

bool ArgumentAnalyzer::setTest() {
  global.runMode = GlobalVariables::RunMode::consoleTest;
  global.configurationFileName =
  "test/configuration_for_testing.json";
  return true;
}

bool ArgumentAnalyzer::setBasePath() {
  if (this->currentIndex + 1 >= global.programArguments.size) {
    global
    << Logger::red
    << "The executable path is missing. "
    << Logger::endL;
    return false;
  }
  this->currentIndex ++;
  global.pathBaseUserInputOrDeduced =
  global.programArguments[this->currentIndex];
  return true;
}

bool ArgumentAnalyzer::setConfigurationFile() {
  if (this->currentIndex + 1 >= global.programArguments.size) {
    global
    << Logger::red
    << "The configuration filename is missing. "
    << Logger::endL;
    return false;
  }
  this->currentIndex ++;
  global.configurationFileName = global.programArguments[this->currentIndex];
  return true;
}

bool ArgumentAnalyzer::processCommandLineConfigurations(std::string& input) {
  if (!this->commandLineConfigurations.contains(input)) {
    return true;
  }
  if (this->currentIndex + 1 >= global.programArguments.size) {
    global
    << Logger::red
    << "The configuration: "
    << input
    << " is missing. "
    << Logger::endL;
    return false;
  }
  this->currentIndex ++;
  global.configurationCommandLine[input] =
  global.programArguments[this->currentIndex];
  return true;
}

bool ArgumentAnalyzer::processOneArgument() {
  if (this->currentIndex >= global.programArguments.size) {
    return false;
  }
  std::string current =
  StringRoutines::stringTrimWhiteSpace(
    global.programArguments[this->currentIndex]
  );
  if (current == MainFlags::format) {
    return this->setFormat();
  }
  if (current == MainFlags::server) {
    return this->setServer();
  }
  if (current == MainFlags::loadDatabase) {
    return this->setLoadDatabase();
  }
  if (current == MainFlags::deploy) {
    return this->setDeploy();
  }
  if (current == MainFlags::help && this->currentIndex == 1) {
    global.runMode = GlobalVariables::RunMode::consoleHelp;
    return true;
  }
  if (current == MainFlags::daemon && this->currentIndex == 1) {
    global.runMode = GlobalVariables::RunMode::daemonMonitor;
    return true;
  }
  if (current == MainFlags::test) {
    return this->setTest();
  }
  if (current == MainFlags::basePath) {
    return this->setBasePath();
  }
  if (current == MainFlags::configurationFile) {
    return this->setConfigurationFile();
  }
  return this->processCommandLineConfigurations(current);
}

void MainFunctions::analyzeMainArguments(int argC, char** argv) {
  STACK_TRACE("MainFunctions::analyzeMainArguments");
  global.configurationCommandLine.reset(JSData::Type::tokenObject);
  if (argC < 0) {
    argC = 0;
  }
  global.programArguments.setSize(argC);
  for (int i = 0; i < argC; i ++) {
    global.programArguments[i] = argv[i];
  }
  global.runMode = GlobalVariables::RunMode::consoleRegular;
  if (argC == 0) {
    return;
  }
  global.pathBaseUserInputOrDeduced = global.programArguments[0];
  if (argC < 2) {
    global.runMode = GlobalVariables::RunMode::builtInWebServer;
    return;
  }
  ArgumentAnalyzer arguments;
  arguments.currentIndex = 1;
  arguments.commandLineConfigurations.addListOnTop(
    List<std::string>({
        Configuration::portHTTP,
        Configuration::portHTTPSOpenSSL,
        Configuration::serverAutoMonitor,
      }
    )
  );
  for (; arguments.processOneArgument(); arguments.currentIndex ++) {}
}

int MainFunctions::mainConsoleHelp() {
  STACK_TRACE("MainFunctions::mainConsoleHelp");
  std::string indent = "  ";
  std::cout << "Run the calculator as a web server.\n";
  std::cout
  << Logger::consoleGreen()
  << "calculator "
  << MainFlags::server
  << " [[max_seconds_per_computation]] "
  << MainFlags::configurationFile
  << " [[configuration_file_name]] "
  << MainFlags::basePath
  << " [[custom_path_to_run_from]]"
  << "\n"
  << Logger::consoleNormal();
  std::cout << indent << "Examples.\n";
  std::cout << indent << "Run as server with defaults:\n";
  std::cout
  << indent
  << Logger::consolePurple()
  << "./calculator\n"
  << Logger::consoleNormal();
  std::cout
  << indent
  << "Run as server with computation timeout of 200 seconds:\n";
  std::cout
  << indent
  << Logger::consolePurple()
  << "./calculator server 200\n"
  << Logger::consoleNormal();
  std::cout
  << indent
  << "Run as server with timeout of 50 seconds, "
  << "custom base path and custom configuration file.\n"
  << indent
  << "By default, the base path is the location of the executable.\n"
  << indent
  << "All database backups and other server data "
  << "is stored in a sub-folder of the base path.\n";
  std::cout
  << indent
  << Logger::consolePurple()
  << "./calculator "
  << MainFlags::server
  << " 50 "
  << MainFlags::configurationFile
  << " \"./configuration/configuration.json\" "
  << MainFlags::basePath
  << " \"./\""
  << "\n"
  << Logger::consoleNormal();
  std::cout << "\nRun the calculator test suite.\n";
  std::cout
  << Logger::consoleGreen()
  << "calculator "
  << MainFlags::test
  << "\n"
  << Logger::consoleNormal();
  std::cout << "\nRun the calculator in restart-when-rebuilt mode.\\n";
  std::cout << indent << "Ideal for development.\n";
  std::cout
  << Logger::consoleGreen()
  << "calculator "
  << MainFlags::daemon
  << "\n"
  << Logger::consoleNormal();
  std::cout << "\nRun the calculator's source code auto-formatter.\n";
  std::cout
  << Logger::consoleGreen()
  << "calculator "
  << MainFlags::format
  << "\n"
  << Logger::consoleNormal();
  std::cout
  << "\nBackup the current database and load "
  << "a different database from a folder.\n"
  << indent
  << "Your current database will be backed "
  << "up before the new database is loaded.\n"
  << indent
  << "Leave [[folder_name]] empty to backup the "
  << "current database without loading a new one.\n"
  << indent
  << "All backups are written to folder database_backups/\n";
  std::cout
  << Logger::consoleGreen()
  << "calculator "
  << MainFlags::loadDatabase
  << " [[folder_name]]\n"
  << Logger::consoleNormal();
  std::cout
  << indent
  << "Examples.\n"
  << indent
  << "Store the current database to a json file without loading a new one.\n"
  << indent
  << Logger::consolePurple()
  << "./calculator load_database\n"
  << Logger::consoleNormal()
  << indent
  << "Load the test database.\n"
  << indent
  << Logger::consolePurple()
  << "./calculator load_database ./test/database/test_local.json\n"
  << Logger::consoleNormal();
  std::cout << "\nDisplay this help message again.\n";
  std::cout
  << Logger::consoleGreen()
  << "calculator "
  << MainFlags::help
  << "\n";
  return 0;
}

int MainFunctions::mainCommandLine() {
  STACK_TRACE("MainFunctions::mainCommandLine");
  global.databaseType = DatabaseType::noDatabaseEveryoneIsAdmin;
  if (!Database::get().initialize(1)) {
    global.fatal << "Failed to initialize database. " << global.fatal;
  }
  Calculator& calculator = global.calculator().getElement();
  calculator.initialize(Calculator::Mode::full);
  if (global.programArguments.size > 1) {
    for (int i = 1; i < global.programArguments.size; i ++) {
      calculator.inputString += global.programArguments[i];
      if (i != global.programArguments.size - 1) {
        calculator.inputString += " ";
      }
    }
  } else {
    global << "Input: " << Logger::endL;
    std::cin >> calculator.inputString;
  }
  calculator.flagUseHtml = false;
  calculator.evaluate(calculator.inputString);
  std::fstream outputFile;
  std::string outputFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      "output/outputFileCommandLine.html",
      outputFileName,
      false,
      false,
      nullptr
    )
  ) {
    outputFileName =
    "Failed to extract output file from output/outputFileCommandLine.html";
  }
  FileOperations::openFileCreateIfNotPresentVirtual(
    outputFile, "output/outputFileCommandLine.html", false, true, false
  );
  global << calculator.outputHTMLString;
  outputFile << calculator.outputHTMLString;
  global
  << "Total running time: "
  << Logger::blue
  << global.getElapsedMilliseconds()
  << " ms. "
  << Logger::endL
  << "Output written in: "
  << Logger::green
  << outputFileName
  << Logger::endL;
  global
  << Logger::red
  << "To run the calculator as a web server, run:"
  << Logger::endL
  << Logger::green
  << "calculator server"
  << Logger::endL
  << "or:"
  << Logger::endL
  << Logger::green
  << "calculator daemon"
  << Logger::endL
  << "if you'd like the server to auto-monitor and restart itself when down.";
  return 0;
}

int MainFunctions::mainFormat() {
  SignalsInfrastructure::signals().initializeSignals();
  CodeFormatter::formatCalculatorInPlace();
  return 0;
}

int MainFunctions::mainLoadDatabase() {
  std::string backupReloadCommand;
  if (!DatabaseLoader::writeBackup(backupReloadCommand)) {
    return - 1;
  }
  if (global.programArguments.size < 3) {
    global
    << "No database provided. Not loading anything. "
    << Logger::green
    << "Your database should have still been backed up."
    << Logger::endL;
    return 0;
  }
  std::string databaseName = global.programArguments[2];
  global
  << "Loading database "
  << Logger::red
  << "local"
  << Logger::normalColor
  << " from: "
  << Logger::purple
  << databaseName
  << Logger::endL;
  std::stringstream comments;
  bool exitCode = 0;
  if (!DatabaseLoader::loadDatabase(databaseName, true, comments)) {
    global
    << Logger::red
    << "Error loading database. "
    << Logger::endL
    << comments.str()
    << Logger::endL;
    exitCode = - 1;
  }
  global
  << Logger::green
  << "To re-load back the database you just backed up "
  << "and overwrote, use:"
  << Logger::endL
  << Logger::purple
  << backupReloadCommand
  << Logger::endL;
  return exitCode;
}

class Deployer {
public:
  static std::string gitBranchName();
  int deploy();
  std::string makeRemoteCommand(
    const std::string& commandToBeEnclosedInSingleQuotes
  );
};

int MainFunctions::mainDeploy() {
  STACK_TRACE("MainFunctions::mainDeploy");
  Deployer deployer;
  return deployer.deploy();
}

std::string Deployer::gitBranchName() {
  TimeWrapper now;
  now.assignLocalTime();
  int year = 1900 + now.timeLocal.tm_year;
  int month = 1 + now.timeLocal.tm_mon;
  int day = now.timeLocal.tm_mday;
  StateMaintainerCurrentFolder maintainFolder;
  global.changeDirectory(global.physicalPathServerBase);
  std::stringstream branchName;
  branchName << "deployed_" << year << "_";
  if (month < 10) {
    branchName << "0";
  }
  branchName << month << "_";
  if (day < 10) {
    branchName << "0";
  }
  branchName << day;
  return branchName.str();
}

std::string Deployer::makeRemoteCommand(
  const std::string& commandToBeEnclosedInSingleQuotes
) {
  const std::string url =
  global.configuration[Configuration::deploy][Configuration::Deploy::url].
  stringValue;
  const std::string username =
  global.configuration[Configuration::deploy][
    Configuration::Deploy::username
  ].stringValue;
  std::stringstream result;
  result
  << "ssh -t "
  << username
  << "@"
  << url
  << " '"
  << commandToBeEnclosedInSingleQuotes
  << "'";
  return result.str();
}

int Deployer::deploy() {
  // If any of the keys below are missing, they will be allocated now.
  const std::string url =
  global.configuration[Configuration::deploy][Configuration::Deploy::url].
  stringValue;
  const std::string username =
  global.configuration[Configuration::deploy][
    Configuration::Deploy::username
  ].stringValue;
  const std::string baseFolder =
  global.configuration[Configuration::deploy][
    Configuration::Deploy::baseFolder
  ].stringValue;
  global
  << "About to deploy to: "
  << Logger::purple
  << url
  << Logger::normalColor
  << " with username: "
  << Logger::orange
  << username
  << Logger::endL;
  // If any keys are newly allocated, let us store these back to the
  // configuration file.
  // In this way, the maintainer of the configuration.json need not remember
  // the name of each key.
  // If the configuration json already has the keys, this will not update the
  // file.
  global.configurationStore();
  if (url == "" || username == "" || baseFolder == "") {
    global
    << Logger::red
    << "Failed to deploy: file configuration/configuration.json "
    << "is missing the deploy.url, deploy.username or deploy.baseFolder keys."
    << Logger::endL;
    return 0;
  }
  global.externalCommandStream("git checkout master");
  std::string gitStatus = global.externalCommandReturnOutput("git status");
  if (StringRoutines::stringContains(gitStatus, "fatal:")) {
    global
    << Logger::red
    << "Git status contained the string fatal:. "
    << "Perhaps you are not in the correct folder? "
    << Logger::endL;
    return 0;
  }
  if (StringRoutines::stringContains(gitStatus, "not staged")) {
    global
    << Logger::red
    << "The `git status` command contains the phrase `not staged`. "
    << "I assume you have non-staged commits, so aborting deployment. "
    << Logger::endL
    << gitStatus;
    return 0;
  }
  std::string branchName = this->gitBranchName();
  if (global.externalCommandStream("git branch " + branchName) != 0) {
    global
    << Logger::red
    << "Failed to execute git command. "
    << Logger::green
    << "Perhaps the branch already exists? "
    << "I am continuing bravely. "
    << Logger::endL;
  }
  global.externalCommandStream("git checkout " + branchName);
  // If the branch already exists
  // - say, from a previous update in the same day -
  // then merge master into it.
  global.externalCommandStream("git merge master ");
  global.externalCommandStream("git push --set-upstream origin " + branchName);
  std::stringstream gitPullRemote;
  gitPullRemote
  << "cd courses_calculator/calculator && git pull && git checkout "
  << branchName;
  std::string sshGitPull = this->makeRemoteCommand(gitPullRemote.str());
  std::string gitPullRemoteResult =
  global.externalCommandReturnOutput(sshGitPull);
  if (
    StringRoutines::stringContains(
      gitPullRemoteResult, "did not match any file"
    )
  ) {
    global
    << Logger::red
    << "Found 'did not match any file' in the remote git pull command."
    << Logger::endL
    << "This is not expected, but I shall continue bravely.";
  }
  global << gitPullRemoteResult << Logger::endL;
  std::string makeRemote =
  "cd courses_calculator/calculator && make -j4 optimize=1";
  global.externalCommandStream(this->makeRemoteCommand(makeRemote));
  std::string rebootServer = "sudo systemctl restart calculator";
  global
  << "Executing the remote calculator reboot command: "
  << Logger::purple
  << rebootServer
  << Logger::endL
  << Logger::green
  << "Please enter your sudo password and press enter: "
  << Logger::endL;
  global.externalCommandStream(this->makeRemoteCommand(rebootServer));
  global.externalCommandStream("git checkout master");
  return 0;
}

bool GlobalVariables::configurationLoad() {
  STACK_TRACE("GlobalVariables::configurationLoad");
  std::stringstream out;
  if (this->configurationFileName == "") {
    this->configurationFileName = "/configuration/configuration.json";
  }
  if (
    !FileOperations::loadFileToStringVirtual(
      this->configurationFileName,
      global.configurationFileContent,
      true,
      &out
    )
  ) {
    global
    << Logger::yellow
    << "Failed to read configuration file. "
    << out.str()
    << Logger::endL;
    std::string computedPhysicalFileName;
    if (
      FileOperations::getPhysicalFileNameFromVirtual(
        this->configurationFileName,
        computedPhysicalFileName,
        true,
        false,
        nullptr
      )
    ) {
      global
      << Logger::yellow
      << "Computed configuration file name: "
      << Logger::blue
      << computedPhysicalFileName
      << Logger::endL;
    }
    return false;
  }
  if (
    !global.configuration.parse(
      this->configurationFileContent, false, &out
    )
  ) {
    global
    << Logger::red
    << "Failed to read configuration. "
    << out.str()
    << Logger::endL;
    return false;
  }
  return true;
}

bool GlobalVariables::configurationStore() {
  STACK_TRACE("GlobalVariables::configurationStore");
  if (global.configurationCommandLine.objects.size() > 0) {
    global
    << Logger::yellow
    << "Command-line overrides in the present configuration: "
    << "configuration file not stored. ";
    return true;
  }
  std::string correctedConfiguration =
  global.configuration.toString(&JSData::PrintOptions::newLine());
  if (correctedConfiguration == global.configurationFileContent) {
    return true;
  }
  std::fstream configurationFile;
  if (
    !FileOperations::openFileCreateIfNotPresentVirtual(
      configurationFile,
      this->configurationFileName,
      false,
      true,
      false,
      true
    )
  ) {
    global
    << Logger::red
    << "Could not open file: "
    << this->configurationFileName
    << Logger::endL;
    std::string configFileNamePhysical;
    if (
      FileOperations::getPhysicalFileNameFromVirtual(
        this->configurationFileName,
        configFileNamePhysical,
        true,
        false,
        nullptr
      )
    ) {
      global
      << Logger::red
      << "Physical file name configuration: "
      << Logger::blue
      << configFileNamePhysical
      << Logger::endL;
      global
      << Logger::red
      << "Server base: "
      << Logger::blue
      << global.physicalPathProjectBase
      << Logger::endL;
    }
    return false;
  }
  global
  << Logger::blue
  << "Configuration has been reformatted/recomputed, "
  << "overwriting previous configuration file."
  << Logger::endL;
  configurationFile << correctedConfiguration;
  return true;
}

void GlobalVariables::configurationProcess() {
  STACK_TRACE("GlobalVariables::configurationProcess");
  for (
    int i = 0; i < global.configurationCommandLine.objects.size(); i ++
  ) {
    global.configuration.setKeyValue(
      global.configurationCommandLine.objects.keys[i],
      global.configurationCommandLine.objects.values[i]
    );
  }
  global.flagServerDetailedLog =
  global.configuration[Configuration::serverDetailedLog].
  isTrueRepresentationInJSON();
  global.flagDebugLogin =
  global.configuration[Configuration::debugLogin].isTrueRepresentationInJSON();
  Database::name = global.configuration[Configuration::database].stringValue;
  if (Database::name == "") {
    Database::name = "local";
    global.configuration[Configuration::database] = "local";
  }
  if (
    Database::name == "fallback" ||
    Database::name == "fallBack" ||
    Database::name == "Fallback"
  ) {
    Database::name = "fallback";
    global.configuration[Configuration::database] = "fallback";
    global.databaseType = DatabaseType::fallback;
  }
  if (global.flagDebugLogin) {
    global
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::red
    << "WARNING: debug login is on. "
    << "**DO NOT TURN THIS ON** in production. "
    << "It **WILL** leak password data and server crypto secrets. "
    << Logger::endL
    << "To protect you, I've switched the database to 'debug'."
    << "This flag is purely intended for development purposes."
    << Logger::purple
    << "************************"
    << Logger::endL;
    Database::name = "debug";
    global.databaseType = DatabaseType::internal;
  }
  if (
    global.configuration[Configuration::disableDatabaseLogEveryoneAsAdmin].
    isTrueRepresentationInJSON()
  ) {
    global.databaseType = DatabaseType::noDatabaseEveryoneIsAdmin;
    Database::name = "";
    global
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::red
    << "WARNING: database disabled, "
    << Logger::green
    << "no database operations permitted."
    << Logger::endL
    << Logger::red
    << "Everyone gets logged-in as administrator. "
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL;
  }
  if (
    global.configuration[Configuration::serverAutoMonitor].
    isTrueRepresentationInJSON() &&
    global.runMode == GlobalVariables::RunMode::daemonMonitor
  ) {
    global
    << Logger::green
    << "Daemon self-monitoring turned on. "
    << Logger::red
    << "This turns OFF server self-monitoring through http pings. "
    << Logger::endL;
  }
  if (
    global.configuration[Configuration::serverAutoMonitor].
    isTrueRepresentationInJSON() &&
    global.runMode != GlobalVariables::RunMode::daemonMonitor
  ) {
    global.flagLocalhostConnectionMonitor = true;
  } else {
    global.flagLocalhostConnectionMonitor = false;
    if (
      global.runMode != GlobalVariables::RunMode::consoleRegular &&
      global.runMode != GlobalVariables::RunMode::consoleTest &&
      global.runMode != GlobalVariables::RunMode::daemonMonitor
    ) {
      global
      << Logger::red
      << "WARNING: server http self-pings are off. "
      << Logger::endL;
    }
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::red
    << "WARNING: DETAILED server logging is on. "
    << Logger::endL
    << "This is strictly for development purposes, "
    << "please do not deploy on live systems. "
    << Logger::endL
    << "To turn off/on server logging: set serverDetailedLog "
    << "to false in the file configuration/configuration.json."
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL;
  }
  global.flagCachingInternalFilesOn =
  !global.configuration["serverRAMCachingOff"].isTrueRepresentationInJSON();
  if (
    !global.flagCachingInternalFilesOn &&
    global.runMode == GlobalVariables::RunMode::builtInWebServer
  ) {
    global
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::red
    << "WARNING: caching files is off. "
    << Logger::endL
    << "This is for development purposes only, "
    << "please do not deploy on live systems. "
    << Logger::endL
    << "To turn off/on modify variable serverRAMCachingOff "
    << "in configuration/configuration.json."
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL;
  }
  global.flagRunServerOnEmptyCommandLine =
  global.configuration["runServerOnEmptyCommandLine"].
  isTrueRepresentationInJSON();
  if (
    global.runMode == GlobalVariables::RunMode::consoleRegular &&
    global.programArguments.size == 1 &&
    global.flagRunServerOnEmptyCommandLine
  ) {
    global
    << Logger::green
    << "runServerOnEmptyCommandLine is set to true => "
    << "Starting server with default configuration. "
    << Logger::endL;
    global.runMode = GlobalVariables::RunMode::builtInWebServer;
    global.millisecondsMaxComputation = 30000;
    // 30 seconds, default
  }
  int reader = 0;
  if (
    global.configuration[Configuration::millisecondsReplyAfterComputation].
    isIntegerFittingInInt(&reader)
  ) {
    global.millisecondsReplyAfterComputation = reader;
  } else {
    global.configuration[Configuration::millisecondsReplyAfterComputation] =
    global.millisecondsReplyAfterComputation;
  }
  if (
    global.configuration[Configuration::processMonitoringBanned].
    isTrueRepresentationInJSON()
  ) {
    global
    << Logger::blue
    << "Worker process monitoring banned from configuration.json. "
    << Logger::endL;
    WebServer::turnProcessMonitoringOff();
  } else {
    WebServer::turnProcessMonitoringOn();
  }
  if (
    global.configuration[Configuration::monitorPingTime].isIntegerFittingInInt(
      &global.server().webServerPingIntervalInSeconds
    )
  ) {
    if (global.server().webServerPingIntervalInSeconds < 0) {
      global.server().webServerPingIntervalInSeconds = 10;
    }
  } else {
    global.configuration[Configuration::monitorPingTime] =
    global.server().webServerPingIntervalInSeconds;
  }
  if (
    global.configuration[Configuration::gitRepository].elementType ==
    JSData::Type::tokenString
  ) {
    HtmlRoutines::gitRepository =
    global.configuration[Configuration::gitRepository].stringValue;
  } else {
    global.configuration[Configuration::gitRepository] =
    HtmlRoutines::gitRepository;
  }
  if (
    !global.configuration[Configuration::webAddress].isString(
      &EmailRoutines::webAdress
    )
  ) {
    global.configuration[Configuration::webAddress] = EmailRoutines::webAdress;
  }
  if (
    !global.configuration[Configuration::sendEmailFrom].isString(
      &EmailRoutines::sendEmailFrom
    )
  ) {
    global.configuration[Configuration::sendEmailFrom] =
    EmailRoutines::sendEmailFrom;
  }
  if (global.configuration[Configuration::portHTTP].stringValue == "") {
    global.configuration[Configuration::portHTTP] = "8155";
  }
  if (
    global.configuration[Configuration::portHTTPSOpenSSL].stringValue == ""
  ) {
    global.configuration[Configuration::portHTTPSOpenSSL] = "8166";
  }
  if (!global.configuration.hasKey(Configuration::portHTTPSBuiltIn)) {
    global.configuration[Configuration::portHTTPSBuiltIn] = "";
  }
  global.flagUseMathTags =
  global.configuration[Configuration::useMathTags].isTrueRepresentationInJSON(
  );
  JSData& webServerOnly =
  global.configuration[Configuration::actAsWebServerForTheseHosts];
  for (int i = 0; i < webServerOnly.objects.size(); i ++) {
    ActAsWebServerOnly subServerConfiguration;
    subServerConfiguration.fromJSON(webServerOnly.objects.values[i]);
    std::string key = webServerOnly.objects.keys[i];
    global.web.actAsWebServerOnlyForTheseHosts.setKeyValue(
      key, subServerConfiguration
    );
    webServerOnly[key] = subServerConfiguration.toJSON();
  }
  if (webServerOnly.elementType != JSData::Type::tokenObject) {
    webServerOnly.elementType = JSData::Type::tokenObject;
  }
  List<List<std::string> > folderVirtualLinksDefault =
  FileOperations::initializeFolderVirtualLinksDefaults();
  for (int i = 0; i < folderVirtualLinksDefault.size; i ++) {
    std::string key = folderVirtualLinksDefault[i][0];
    std::string value = folderVirtualLinksDefault[i][1];
    if (global.configuration[key].stringValue == "") {
      global.configuration[key] = value;
    }
  }
}
