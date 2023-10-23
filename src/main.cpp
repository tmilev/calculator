#include "main.h"
#include "web_assembly.h"
#include "general_logging_global_variables.h"
#include "webserver.h"
#include "general_test.h"
#include "system_functions_global_objects.h"
#include "signals_infrastructure.h"
#include "string_constants.h"
#include "database.h"
#include "calculator_interface.h"
#include "source_code_formatter.h"

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

int MainFunctions::mainWithoutExceptions(int argc, char** argv) {
  // Initializations basic (timer, ...).
  initializeGlobalObjects();
  // Initializations of build flags.
  global.server().WebServer::initializeBuildFlags();
  // Process executable arguments.
  // May set the value of
  // global.PathProjectBaseUserInputOrDeduced.
  global.server().analyzeMainArguments(argc, argv);
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
    return mainTest(global.programArguments);
  case GlobalVariables::RunMode::formatCode:
    return MainFunctions::mainFormat();
  case GlobalVariables::RunMode::loadDatabase:
    return MainFunctions::mainLoadDatabase();
  default:
    return MainFunctions::mainCommandLine();
  }
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
  <<
  "All database backups and other server data is stored in a sub-folder of the base path.\n"
  ;
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
  <<
  "\nBackup the current database and load a different database from a folder.\n"
  << indent
  << "Your current database will be backed "
  << "up before the new database is loaded.\n"
  << indent
  <<
  "Leave [[folder_name]] empty to backup the current database without loading a new one.\n"
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
