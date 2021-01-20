// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderLoggingGlobalVars_already_defined
#define vpfHeaderLoggingGlobalVars_already_defined
#include "general_lists.h"
#include "general_list_references.h"
#include "general_maps.h"
#include "general_database_system_independent.h"
#include <thread>

// Forward-declared class webserver.
class WebServer;

class ThreadData {
public:
  std::thread::id theId;
  int index;
  std::string name;
  std::string toStringHtml() const;
  std::string toStringConsole() const;
  static int getCurrentThreadId();
  static void registerFirstThread(const std::string& inputName = "");
  static ThreadData& registerNewThread(const std::string& inputName = "");
  static void createThread(void (*InputFunction)(int), const std::string& inputName);
  static std::string toStringAllThreadsHtml();
  static std::string toStringAllThreadsConsole();
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
      std::string theType;
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
  int MaxLogSize;
  std::string theFileName;
  std::string bufferFile;
  std::string bufferStandardOutput;
  std::fstream theFile;
  bool flagStopWritingToFile;
  bool flagInitialized;
  bool flagTagColorHtmlOpened;
  bool flagTagColorConsoleOpened;
  bool flagResetLogFileWhenTooLarge;
  Logger();
  void checkLogSize();
  enum loggerSpecialSymbols{ endL, red, blue, yellow, green, purple, cyan, normalColor, orange};
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
  Logger& operator<<(const std::string& input);
  Logger& operator<<(const loggerSpecialSymbols& input);
  void flush();
  template <typename theType>
  Logger& operator<<(const theType& toBePrinted) {
    return this->doTheLogging(toBePrinted);
  }
  template <typename theType>
  Logger& doTheLogging(const theType& toBePrinted);
};

// All global objects are either
// 1) fields of variable:  GlobalVariables& global
// or
// 2) are returned as results of static functions calls along the pattern below.
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
      // This optional process monitors the entire system and reboots it if it goes down.
      static const int daemon = 1;
      // This optional process monitors the server process over a localhost connection.
      // Attempts to kill the server process if the localhost connection is lost.
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
  template <typename theType>
  GlobalVariables& operator<<(const theType& toBePrinted) {
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
  // int externalCommandNoOutput(const std::string& theCommand)
  int (*pointerExternalCommandNoOutput)(const std::string& theSystemCommand);
  // When non-null, usually points to:
  // std::string externalCommandReturnStandartOut(const std::string& inputCommand)
  std::string (*pointerExternalCommandReturnOutput)(const std::string& theSystemCommand);
  // When non-null, usually points to:
  // int externalCommandStream(const std::string& inputCommand)
  int (*pointerExternalCommandStream)(const std::string& theSystemCommand);
  // When non-null, this usually points to: callChDirWrapper.
  void (*pointerCallChDir)(const std::string& theDirectoryName);
  //  double MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit;
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

  // bool flagTesting;
  // webserver flags and variables
  bool flagRunServerOnEmptyCommandLine;
  bool flagCachingInternalFilesOn;
  bool flagServerDetailedLog;
  bool flagIsChildProcess;

  bool flagLoggedIn;
  bool flagLogInAttempted;
  bool flagUsingSSLinCurrentConnection;
  bool flagSSLIsAvailable;
  bool flagCertificatesAreOfficiallySigned;
  bool flagCrashInitiated;

  // If false, signals use of a mock database.
  // Mock database = simple json file that
  // is 100% read/overwritten on each database
  // read/store operation.
  bool flagDatabaseCompiled;
  bool flagDisableDatabaseLogEveryoneAsAdmin;
  bool flagLocalhostConnectionMonitor;
  bool flagDaemonMonitor;

  std::string buildVersionSimple;
  std::string buildHeadHashWithServerTime;
  std::string operatingSystem;
  std::string hostWithPort;
  std::string hostNoPort;

  bool flagRequestComingLocally;
  // internal infrastructure flags
  bool flagServerforkedIntoWorker;
  bool flagNotAllocated;

  // Used for cryptographic purposes.
  // Initialized once, securely, in the
  // parent process.
  // When running as a web server, this happens inside
  // WebServer::run.
  // When not initialized, contains 0 bytes.
  List<unsigned char> randomBytesCurrent;
  // Random bytes to keep as internal state.
  const unsigned int numberOfRandomBytes = 64; // = 512 bits
  // Not allowed to extract more than this many random bytes at once.
  const unsigned int maximumExtractedRandomBytes = 32; // = 256 bits

  MemorySaving<FormatExpressions> theDefaultFormat;
  MemorySaving<FormatExpressions> thePolyFormat;
  //status flags:
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
  ListReferences<std::thread> theThreads;
  ListReferences<ThreadData> theThreadData;
  ListReferences<ListReferences<StackInfo> > customStackTrace;
  ListReferences<ListReferences<std::string> > progressReportStrings;
  List<std::string> programArguments;

  MapList<std::string, std::string, MathRoutines::hashString> webArguments;
  MapList<std::string, std::string, MathRoutines::hashString> cookiesToBeSent;
  UserCalculatorData userDefault;
  MapList<std::string, int, MathRoutines::hashString> problemExpectedNumberOfAnswers;

  std::string requestType;

  // location of the configuration file. Can be overridden with command line flags.
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
  PauseThread theLocalPauseController;

  static std::string hopefullyPermanentWebAdress;
  static std::string hopefullyPermanentWebAdressOfServerExecutable;
  static std::string hopefullyPermanent_HTTPS_WebAdressOfServerExecutable;
  static std::string hopefullyPermanent_HTTPS_WebAdressJavascriptFolder;
  static std::string hopefullyPermanentWebAdressOfServerOutputFolder;

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
  std::string displayNameExecutableAppNoCache;

  std::string IPAdressCaller;

  bool flagUseMathTags;

  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;
  MemorySaving<GroebnerBasisComputation<Rational> > theGroebnerBasisComputation;
  class Crasher {
    public:
    std::stringstream crashReport;
    std::stringstream crashReportHtml;
    std::stringstream crashReportConsolE;
    std::stringstream crashReportFile;
    bool flagCrashInitiated; //<-we crash only once, and we do not resume execution after a crash
    bool flagFinishingCrash; //<-we crash only once, and we do not resume execution after a crash
    Crasher();
    void firstRun();
    static std::string getStackTraceEtcErrorMessageHTML();
    static std::string getStackTraceEtcErrorMessageConsole();
    Crasher& operator<<(const Crasher& dummyCrasherSignalsActualCrash);
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
    template <typename theType>
    CommentsCurrentConnection& operator<<(const theType& comment) {
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
  void setWebInput(const std::string& inputName, const std::string& inputValue);
  std::string getWebInput(const std::string& inputName);
  void initialize();
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
  int externalCommandNoOutput(const std::string& systemCommand, bool logErrors);
  std::string externalCommandReturnOutput(const std::string& systemCommand);
  void changeDirectory(const std::string& systemCommand);
  std::string toStringCalculatorArgumentsNoNavigation(List<std::string>* tagsToExclude);
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
  /// @endcond
};

template <typename theType>
Logger& Logger::doTheLogging(const theType& toBePrinted) {
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
#endif

