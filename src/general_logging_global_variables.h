
#ifndef header_general_logging_global_variables_ALREADY_INCLUDED
#define header_general_logging_global_variables_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_list_references.h"
#include "general_maps.h"
#include "general_database_system_independent.h"
#include <thread>

// Forward-declared class webserver.
class WebServer;

class ThreadData {
public:
  std::thread::id id;
  int index;
  std::string name;
  std::string toStringHtml() const;
  std::string toStringConsole() const;
  static int getCurrentThreadId();
  static void registerFirstThread(const std::string& inputName = "");
  static ThreadData& registerNewThread(const std::string& inputName = "");
  static void createThread(
    void(*InputFunction)(int), const std::string& inputName
  );
  static std::string toStringAllThreadsHtml();
  static std::string toStringAllThreadsConsole();
  static void runTimerThread(int threadIndex);
  ThreadData();
  ~ThreadData();
};

class Logger {
public:
  class StringHighligher {
  public:
    class Section {
    public:
      int length;
      std::string sectionType;
      Section();
      Section(int inputLength);
      Section(const std::string& input);
    };

    List<Logger::StringHighligher::Section> sections;
    StringHighligher();
    StringHighligher(const std::string& input);
    void reset();
  };

  StringHighligher nextHighlighter;
  int currentColor;
  int maximumLogSize;
  std::string fileName;
  std::string bufferFile;
  std::string bufferStandardOutput;
  std::fstream logFile;
  bool flagStopWritingToFile;
  bool flagInitialized;
  bool flagTagColorHtmlOpened;
  bool flagTagColorConsoleOpened;
  bool flagResetLogFileWhenTooLarge;
  Logger();
  void checkLogSize();
  enum LoggerSpecialSymbols {
    endL, red, blue, yellow, green, purple, cyan, normalColor, orange  };
  static std::string consoleRed();
  static std::string consoleBlue();
  static std::string consoleYellow();
  static std::string consoleGreen();
  static std::string purpleConsole();
  static std::string cyanConsole();
  static std::string orangeConsole();
  static std::string consoleNormal();
  std::string getStamp();
  std::string getStampShort();
  std::string closeTagConsole();
  std::string closeTagHtml();
  std::string openTagConsole();
  std::string openTagHtml();
  void initializeIfNeeded();
  void reset();
  Logger& operator<<(const Logger::StringHighligher& input);
  Logger& operator<<(const std::string& input) {
    // The operator overloading causes issues qtcreator finding the right
    // method,
    // so redirecting to something that's easy to find.
    return this->logString(input);
  }
  // Logs special logger symbols such as console color control.
  // Also the flushing of the log happens here (on an end-line special symbol).
  Logger& operator<<(const LoggerSpecialSymbols& input) {
    return this->logSpecialSymbol(input);
  }
  Logger& logSpecialSymbol(const LoggerSpecialSymbols& input);
  void flush();
  template <typename Type>
  Logger& operator<<(const Type& toBePrinted) {
    return this->doTheLogging(toBePrinted);
  }
  template <typename Type>
  Logger& doTheLogging(const Type& toBePrinted);
  Logger& logString(const std::string& input);
};

// All global objects are either
// 1) fields of variable:  GlobalVariables& global
// or
// 2) are returned as results of static functions calls along the pattern
// below.
//
// object& getObject() { static object result; result result; }
//
// or
// 3) are small static objects without constructors/destructors.
//    Those include global strings, integers and timeval structures.
//
// Discussion.
//
// Please pay attention to the static initialization order fiasco.
// The fiasco states that global objects (allocated before main)
// may be allocated in an unexpected order.
// In particular an object X allocated before main cannot
// assume that the constructor of another global
// object Y has already been called, even if the
// constructor of X calls Y.something().
// A possible "horror" scenario: the programmer decides
// to register a stack trace in the constructor of an object X.
// Stack traces are stored inside the global object
// GlobalVariables global.
// That runs just fine.
// one year later, the programmer decides to
// make X a global object,
// and again everything runs just fine as object global
// happens to be initialized before X.
// Finally, two years later, the same programmer
// decides to declare a global object Y of the same type in
// a file initialized before object global.
// This causes a nasty and difficult to catch
// crash before main.
class GlobalVariables {
  MutexRecursiveWrapper mutexReturnBytes;
public:
  MutexRecursiveWrapper mutexWebWorkerPipeWritelock;
  MutexRecursiveWrapper mutexWebWorkerPipeReadlock;
  MutexRecursiveWrapper mutexParallelComputingCrash;
  MutexRecursiveWrapper mutexRegisterNewThread;
  GlobalVariables();
  ~GlobalVariables();
  class LogData {
  public:
    class type {
    public:
      // Listed in approximate order of appearance.
      // This process type initializes the system.
      static const int initialization = 0;
      // This optional process monitors the entire system and reboots it if it
      // goes down.
      static const int daemon = 1;
      // This optional process monitors the server process over a localhost
      // connection.
      // Attempts to kill the server process if the localhost connection is
      // lost.
      static const int serverMonitor = 2;
      // This process type runs the web server and accepts connections.
      static const int server = 3;
      // This process type runs the calculator connections
      // and does the actual internet input/output.
      static const int worker = 4;
    };

    int logType;
    Logger server;
    Logger serverMonitor;
    Logger worker;
    // Logger daemon currently logs to stdout only (no file backup).
    Logger daemon;
    LogData() {
      this->logType = LogData::type::server;
    }
    std::string toStringProcessType() const;
  };

  LogData logs;
  template <typename Type>
  GlobalVariables& operator<<(const Type& toBePrinted) {
    switch (this->logs.logType) {
    case GlobalVariables::LogData::type::server:
      this->logs.server << toBePrinted;
      return *this;
    case GlobalVariables::LogData::type::serverMonitor:
      this->logs.serverMonitor << toBePrinted;
      return *this;
    case GlobalVariables::LogData::type::worker:
      this->logs.worker << toBePrinted;
      return *this;
    case GlobalVariables::LogData::type::daemon:
      this->logs.daemon << toBePrinted;
      return *this;
    default:
      this->logs.worker << toBePrinted;
      return *this;
    }
  }
  // When non-null, usually points to:
  // int externalCommandNoOutput(const std::string& command)
  int(*pointerExternalCommandNoOutput)(const std::string& systemCommand);
  // When non-null, usually points to:
  // std::string externalCommandReturnStandardOut(const std::string&
  // inputCommand)
  std::string(*pointerExternalCommandReturnOutput)(
    const std::string& systemCommand
  );
  // When non-null, usually points to:
  // int externalCommandStream(const std::string& inputCommand)
  int(*pointerExternalCommandStream)(const std::string& systemCommand);
  // When non-null, this usually points to: callChDirWrapper.
  void(*pointerCallChDir)(const std::string& directoryName);
  // double
  // MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit;
  MemorySaving<Calculator>& calculator();
  int64_t millisecondOffset;
  int64_t millisecondsMaxComputation;
  int64_t millisecondsNoPingBeforeChildIsPresumedDead;
  int64_t millisecondsReplyAfterComputation;
  int64_t millisecondsComputationStart;
  //  bool flagLogInterprocessCommunication;
  // flags: what mode are we running in?
  bool flagRunningConsoleHelp;
  bool flagRunningConsoleRegular;
  bool flagRunningConsoleTest;
  bool flagRunningBuiltInWebServer;
  bool flagRunningWebAssembly;
  bool flagRunningFormatCode;
  // bool flagTesting;
  // webserver flags and variables
  bool flagRunServerOnEmptyCommandLine;
  bool flagCachingInternalFilesOn;
  bool flagServerDetailedLog;
  bool flagIsChildProcess;
  bool flagLoggedIn;
  bool flagLogInAttempted;
  bool flagUsingSSLinCurrentConnection;
  // Whether we can carry out transport layer security
  // "ssl" connections.
  bool flagSSLAvailable;
  bool flagCertificatesAreOfficiallySigned;
  bool flagCrashInitiated;
  // If false, signals use of a mock database.
  // Mock database = simple json file that
  // is 100% read/overwritten on each database
  // read/store operation.
  bool flagDatabaseCompiled;
  // A flag that triggers debug messages during login.
  // This will reveal secrets to end users and cannot run in production.
  // The flag can only be used with fallback database.
  // It will automatically be turned off if using the regular database.
  bool flagDebugLogin;
  bool flagDisableDatabaseLogEveryoneAsAdmin;
  // When this is set, the calculator will spin a separate
  // process that will periodically ping the main calculator
  // server.
  bool flagLocalhostConnectionMonitor;
  // If this flag is set, the calculator should run as a
  // daemon that spawns worker calculator processes
  // and monitors their deaths.
  // Setting this flag will turn off web server
  // self-pinging.
  bool flagDaemonMonitor;
  std::string buildVersionSimple;
  std::string buildHeadHashWithServerTime;
  std::string operatingSystem;
  std::string hostWithPort;
  std::string hostNoPort;
  bool flagRequestComingLocally;
  // internal infrastructure flags
  bool flagServerForkedIntoWorker;
  bool flagNotAllocated;
  // Used for cryptographic purposes.
  // Initialized once, securely, in the
  // parent process.
  // When running as a web server, this happens inside
  // WebServer::run.
  // When not initialized, contains 0 bytes.
  List<unsigned char> randomBytesCurrent;
  // Random bytes to keep as internal state.
  const unsigned int numberOfRandomBytes = 64;
  // = 512 bits
  // Not allowed to extract more than this many random bytes at once.
  const unsigned int maximumExtractedRandomBytes = 32;
  // = 256 bits
  MemorySaving<FormatExpressions> defaultFormat;
  // status flags:
  bool flagComputationComplete;
  bool flagComputationStarted;
  bool flagComputationFinishedAllOutputSentClosing;
  bool flagDeallocated;
  class Response {
  private:
    bool flagTimedOut;
    bool flagReportAllowed;
  public:
    bool flagReportDesired;
    bool flagBanProcessMonitoring;
    class StateMaintainer {
    public:
      GlobalVariables::Response* owner;
      bool flagReportAllowed;
      StateMaintainer(GlobalVariables::Response& inputOwner) {
        this->owner = &inputOwner;
        this->flagReportAllowed = this->owner->flagReportAllowed;
      }
      ~StateMaintainer() {
        this->owner->flagReportAllowed = this->flagReportAllowed;
      }
    };

    class ReportType {
    public:
      static const int general = 0;
      static const int gaussianElimination = 1;
      static const int monomialAlgebraProduct = 2;
      static const int fileInputOutput = 3;
      static const int largeIntegerArithmetic = 4;
    };

    // Respond functions start here.
    // The functions below lock one another out.
    void report(const std::string& input);
    void initiate(const std::string& message);
    // Respond functions end here.
    // returns true
    bool writeResponse(const JSData& incoming, bool isCrash = false);
    bool monitoringAllowed();
    bool reportDesired(int type = Response::ReportType::general);
    void disallowReport();
    void allowReport();
    bool isTimedOut();
    Response();
  };

  Response response;
  ListReferences<std::thread> allThreads;
  ListReferences<ThreadData> threadData;
  ListReferences<ListReferences<StackInfo> > customStackTrace;
  ListReferences<ListReferences<std::string> > progressReportStrings;
  List<std::string> programArguments;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > webArguments;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > cookiesToBeSent;
  UserCalculatorData userDefault;
  MapList<
    std::string, int, HashFunctions::hashFunction<std::string>
  > problemExpectedNumberOfAnswers;
  std::string requestType;
  // location of the configuration file. Can be overridden with command line
  // flags.
  std::string configurationFileName;
  std::string configurationFileContent;
  // The configuration is computed by merging a configuration file
  // with commandline configuration flags.
  // If a configuration file is missing, defaults will be chosen.
  //
  // On condition that there were no command line configuration options,
  // the configuration file will be reformatted/corrected if needed.
  JSData configuration;
  // Configuration coming from command line.
  // Does not affect the configuration file defaults:
  // the configuration file is stored before
  // the command-line configurations are merged in.
  JSData configurationCommandLine;
  JSData timeStats;
  List<List<std::string> > databaseModifiableFields;
  std::string userInputStringRAWIfAvailable;
  std::string userInputStringIfAvailable;
  std::string relativePhysicalNameCrashReport;
  std::string relativePhysicalNameOptionalProgressReport;
  std::string relativePhysicalNameOptionalResult;
  std::string pathExecutableUserInputOrDeduced;
  std::string physicalPathProjectBase;
  std::string physicalPathServerBase;
  std::string physicalNameFolderExecutable;
  std::string physicalNameExecutableNoPath;
  std::string physicalNameExecutableWithPath;
  std::string displayPathOutputFolder;
  // The backend URL, for example
  // displayNameExecutable ="/cgi-bin/calculator";
  std::string displayNameExecutable;
  // The app display name in the web browser,
  // displayApplication ="/app";
  std::string displayApplication;
  std::string displayApplicationNoCache;
  std::string IPAdressCaller;
  bool flagUseMathTags;
  MemorySaving<DynkinDiagramRootSubalgebra> dynGetEpsCoords;
  class Crasher {
  public:
    std::stringstream crashReport;
    std::stringstream crashReportHtml;
    std::stringstream crashReportConsolE;
    std::stringstream crashReportFile;
    bool flagCrashInitiated;
    // <-we crash only once, and we do not resume execution after a crash
    bool flagFinishingCrash;
    // <-we crash only once, and we do not resume execution after a crash
    Crasher();
    void firstRun();
    static std::string getStackTraceEtcErrorMessageHTML();
    static std::string getStackTraceEtcErrorMessageConsole();
    Crasher& doCrash(const Crasher& dummyCrasherSignalsActualCrash);
    void writeCrashFile();
    // Syntactic sugar for crashing, so we can write:
    //
    // global.fatal << "explanation for crash" << global.fatal;
    //
    // for a single-line crashing.
    Crasher& operator<<(const Crasher& dummyCrasherSignalsActualCrash) {
      return this->doCrash(dummyCrasherSignalsActualCrash);
    }
    template <class AnyObject>
    Crasher& operator<<(const AnyObject& input) {
      this->firstRun();
      this->crashReport << input;
      return *this;
    }
  };

  Crasher fatal;
  class CommentsCurrentConnection {
    MemorySaving<std::stringstream> container;
  public:
    template <typename Type>
    CommentsCurrentConnection& operator<<(const Type& comment) {
      this->container.getElement() << comment;
      return *this;
    }
    void resetComments() {
      this->container.freeMemory();
    }
    std::string getCurrentReset() {
      std::string result = this->container.getElement().str();
      this->resetComments();
      return result;
    }
  };

  CommentsCurrentConnection comments;
  void joinAllThreads();
  bool configurationStore();
  bool configurationLoad();
  void configurationProcess();
  int getGlobalTimeInSeconds();
  int64_t getElapsedMilliseconds();
  double getElapsedSeconds() {
    return static_cast<double>(this->getElapsedMilliseconds()) / 1000;
  }
  static void initThreadsExecutableStart();
  bool userDefaultIsDebuggingAdmin();
  bool userDefaultHasAdminRights();
  bool userDefaultHasProblemComposingRights();
  bool userRequestRequiresLoadingRealExamData();
  bool userRequestMustBePromptedToLogInIfNotLoggedIn();
  void fallAsleep(int microseconds);
  static std::string getDateForLogFiles();
  static std::string getTimeGM();
  static std::string getTimeLocal();
  void setWebInput(
    const std::string& inputName, const std::string& inputValue
  );
  std::string getWebInput(const std::string& inputName);
  void initialize();
  void initializeMathematics();
  void initModifiableDatabaseFields();
  void initDefaultFolderAndFileNames();
  void initFoldersProjectBase(const std::string& inputPhysicalExecutable);
  void initOutputReportAndCrashFileNames(
    const std::string& inputUserStringRAW,
    const std::string& inputUserStringCivilized
  );
  bool userSecureNonAdminOperationsAllowed();
  bool userGuestMode();
  bool userDebugFlagOn();
  bool userStudentVieWOn();
  bool checkConsistency();
  int externalCommandStream(const std::string& systemCommand);
  int externalCommandNoOutput(
    const std::string& systemCommand, bool logErrors
  );
  std::string externalCommandReturnOutput(const std::string& systemCommand);
  void changeDirectory(const std::string& systemCommand);
  std::string toStringCalculatorArgumentsNoNavigation(
    List<std::string>* tagsToExclude
  );
  std::string toHTMLTopCommandLinuxSystem();
  std::string toStringFolderInfo() const;
  std::string toStringThreadData(bool useHTML);
  std::string toStringProgressReportNoThreadData(bool useHTML);
  std::string toStringProgressReportWithThreadData(bool useHTML);
  std::string toStringProgressReportConsole();
  WebServer& server();
  void makeReport();
  class Test {
  public:
    static bool all();
    static bool builds();
  };
};

template <typename Type>
Logger& Logger::doTheLogging(const Type& toBePrinted) {
  this->initializeIfNeeded();
  std::stringstream out;
  out << toBePrinted;
  this->bufferStandardOutput += out.str();
  if (!this->flagStopWritingToFile) {
    this->bufferFile += out.str();
  }
  this->checkLogSize();
  return *this;
}

#endif // header_general_logging_global_variables_ALREADY_INCLUDED
