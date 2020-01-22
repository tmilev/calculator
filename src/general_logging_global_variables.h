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
  std::string ToStringHtml() const;
  std::string ToStringConsole() const;
  static int getCurrentThreadId();
  static void RegisterFirstThread(const std::string& inputName = "");
  static ThreadData& RegisterNewThread(const std::string& inputName = "");
  static void CreateThread(void (*InputFunction)(int), const std::string& inputName);
  static std::string ToStringAllThreadsHtml();
  static std::string ToStringAllThreadsConsole();
  ThreadData();
  ~ThreadData();
};

class logger {
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
    List<logger::StringHighligher::Section> sections;
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
  logger();
  void CheckLogSize();
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
  logger& operator<<(const logger::StringHighligher& input);
  logger& operator<<(const std::string& input);
  logger& operator<<(const loggerSpecialSymbols& input);
  void flush();
  template <typename theType>
  logger& operator<<(const theType& toBePrinted) {
    return this->doTheLogging(toBePrinted);
  }
  template <typename theType>
  logger& doTheLogging(const theType& toBePrinted);
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
// In particular an object allocated before main cannot
// assume that the constructor of global
// object has already been called.
// In particular one should avoid declaring objects at global scope as
// the constructors of those may rely on the global object.
// A possible "horror" scenario: the programmer decides to register a stack trace
// in the constructor of an object. That runs just fine.
// One year later, the programmer decides to
// declare a global object of that type,
// and again everything runs just fine as global
// happens to be initialized before that object.
// Finally, two years later, the same programmer
// decides to declare a global object of the same type in
// a file initialized before the declaration of global.
// This causes a nasty and difficult to catch
// crash before main.
class GlobalVariables {
  MutexRecursiveWrapper MutexReturnBytes;
public:
  MutexRecursiveWrapper MutexWebWorkerPipeWriteLock;
  MutexRecursiveWrapper MutexWebWorkerPipeReadLock;
  MutexRecursiveWrapper MutexParallelComputingCrash;
  MutexRecursiveWrapper MutexRegisterNewThread;

  GlobalVariables();
  ~GlobalVariables();
  class LogData {
  public:
    class type {
    public:
      static const int server = 0;
      static const int serverMonitor = 1;
      static const int worker = 2;
    };
    int logType;
    logger server;
    logger serverMonitor;
    logger worker;
    LogData() {
      this->logType = LogData::type::server;
    }
    std::string ToStringProcessType() const;
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
    default:
      this->logs.worker << toBePrinted;
      return *this;
    }
  }
  int (*pointerCallSystemNoOutput)(const std::string& theSystemCommand);
  std::string (*pointerCallSystemWithOutput)(const std::string& theSystemCommand);
  void (*pointerCallChDir)(const std::string& theDirectoryName);
  //  double MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit;
  MemorySaving<Calculator>& calculator();

  int64_t millisecondOffset;
  int64_t millisecondsMaxComputation;
  int64_t millisecondsNoPingBeforeChildIsPresumedDead;
  int64_t millisecondsReplyAfterComputation;
  int64_t millisecondsComputationStart;

  //  bool flagLogInterProcessCommunication;
  //flags: what mode are we running in?
  bool flagRunningConsoleRegular;
  bool flagRunningConsoleTest;
  bool flagRunningBuiltInWebServer;

  // bool flagTesting;
  //webserver flags and variables
  bool flagRunServerOnEmptyCommandLine;
  bool flagCachingInternalFilesOn;
  bool flagServerDetailedLog;
  bool flagIsChildProcess;
  bool flagRestartNeeded;
  bool flagStopNeeded;

  bool flagLoggedIn;
  bool flagLogInAttempted;
  bool flagUsingSSLinCurrentConnection;
  bool flagSSLIsAvailable;
  bool flagCertificatesAreOfficiallySigned;
  bool flagCrashInitiated;

  bool flagDatabaseCompiled;
  bool flagDisableDatabaseLogEveryoneAsAdmin;
  bool flagServerAutoMonitor;

  std::string buildVersionSimple;
  std::string buildHeadHashWithServerTime;
  std::string OperatingSystem;
  std::string hostWithPort;
  std::string hostNoPort;

  bool flagRequestComingLocally;
  // Various unit tests will be run on calculator boot if this is true:
  bool flagAutoUnitTest;

  // internal infrastructure flags
  bool flagServerForkedIntoWorker;
  bool flagNotAllocated;

  // Used for cryptographic purposes.
  // Initialized once, securely, in the
  // parent process.
  // When running as a web server, this happens inside
  // WebServer::Run.
  // When not initialized, contains 0 bytes.
  List<unsigned char> randomBytesCurrent;
  // Random bytes to keep as internal state.
  const unsigned int numberOfRandomBytes = 64; // = 512 bits
  // Not allowed to extract more than this many random bytes at once.
  const unsigned int maximumExtractedRandomBytes = 32; // = 256 bits

  MemorySaving<FormatExpressions> theDefaultFormat;
  MemorySaving<FormatExpressions> thePolyFormat;
  //status flags:
  bool flagComputationCompletE;
  bool flagComputationStarted;
  bool flagComputationFinishedAllOutputSentClosing;
  bool flagDeallocated;
  class Response {
  private:
    bool flagTimedOut;
    bool flagReportAllowed;
  public:
    bool flagReportDesired;
    bool flagBanProcessMonitorinG;
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
    void Report(const std::string& input);
    void Initiate(const std::string& message);
    // Respond functions end here.

    // returns true
    bool WriteResponse(const JSData& incoming, bool isCrash = false);

    bool MonitoringAllowed();
    bool ReportDesired(int type = Response::ReportType::general);
    void DisallowReport();
    void AllowReport();
    bool TimedOut();
    Response();
  };
  // Do not use for cryptographic purposes.
  // Intended use:
  // generate random numbers for mathematical problems/education.
  UnsecurePseudoRandomGenerator unsecurePseudoRandomGenerator;
  Response theResponse;
  ListReferences<std::thread>theThreads;
  ListReferences<ThreadData> theThreadData;
  ListReferences<ListReferences<stackInfo> > CustomStackTrace;
  ListReferences<ListReferences<std::string> > progressReportStrings;
  List<std::string> programArguments;

  MapList<std::string, std::string, MathRoutines::HashString> webArguments;
  MapList<std::string, std::string, MathRoutines::HashString> CookiesToSetUsingHeaders;
  UserCalculatorData userDefault;
  MapList<std::string, int, MathRoutines::HashString> problemExpectedNumberOfAnswers;

  std::string requestType;

  std::string configurationFileContent;
  JSData configuration;
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

  std::string RelativePhysicalNameCrashReport;
  std::string RelativePhysicalNameOptionalProgressReport;
  std::string RelativePhysicalNameOptionalResult;

  std::string PathProjectBaseUserInputOrDeduced;

  std::string PhysicalPathProjectBase;
  std::string PhysicalPathServerBase;
  std::string PhysicalNameFolderExecutable;
  std::string PhysicalNameExecutableNoPath;
  std::string PhysicalNameExecutableWithPath;

  std::string DisplayPathExecutable;
  std::string DisplayPathOutputFolder;
  // The backend URL, for example
  // DisplayNameExecutable ="/cgi-bin/calculator";
  std::string DisplayNameExecutable;
  // The app display name in the web browser,
  // DisplayNameExecutableApp ="/app";
  std::string DisplayNameExecutableApp;
  std::string DisplayNameExecutableAppNoCache;

  std::string IPAdressCaller;

  MemorySaving<Matrix<Rational> > matIdMatrix;
  MemorySaving<Matrix<Rational> > matOneColumn;

  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;
  MemorySaving<GroebnerBasisComputation<Rational> > theGroebnerBasisComputation;
  class Crasher {
    public:
    std::stringstream crashReport;
    std::stringstream crashReportHtml;
    std::stringstream crashReportConsolE;
    std::stringstream crashReportFile;
    bool flagCrashInitiateD; //<-we crash only once, and we do not resume execution after a crash
    bool flagFinishingCrash; //<-we crash only once, and we do not resume execution after a crash
    Crasher();
    void FirstRun();
    static std::string GetStackTraceEtcErrorMessageHTML();
    static std::string GetStackTraceEtcErrorMessageConsole();
    Crasher& operator<<(const Crasher& dummyCrasherSignalsActualCrash);
    template <class AnyObject>
    Crasher& operator<<(const AnyObject& input) {
      this->FirstRun();
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
      this->container.GetElement() << comment;
      return *this;
    }
    void resetComments() {
      this->container.FreeMemory();
    }
    std::string getCurrentReset() {
      std::string result = this->container.GetElement().str();
      this->resetComments();
      return result;
    }
  };
  CommentsCurrentConnection Comments;
  void JoinAllThreads();
  bool ConfigurationStore();
  bool ConfigurationLoad();
  void ConfigurationProcess();
  int GetGlobalTimeInSeconds();
  int64_t GetElapsedMilliseconds();
  double GetElapsedSeconds() {
    return static_cast<double>(this->GetElapsedMilliseconds()) / 1000;
  }
  static void InitThreadsExecutableStart();
  bool UserDefaultIsDebuggingAdmin();
  bool UserDefaultHasAdminRights();
  bool UserDefaultHasProblemComposingRights();
  bool UserRequestRequiresLoadingRealExamData();
  bool UserRequestMustBePromptedToLogInIfNotLoggedIn();
  void FallAsleep(int microseconds);

  static std::string GetDateForLogFiles();
  static std::string GetTimeGM();
  static std::string GetTimeLocal();
  void SetWebInpuT(const std::string& inputName, const std::string& inputValue);
  std::string GetWebInput(const std::string& inputName);
  void init();
  void initModifiableDatabaseFields();
  void initDefaultFolderAndFileNames();
  void initFoldersProjectBase(const std::string& inputPhysicalExecutable);
  void initOutputReportAndCrashFileNames(
    const std::string& inputUserStringRAW,
    const std::string& inputUserStringCivilized
  );
  bool UserSecureNonAdminOperationsAllowed();
  bool UserGuestMode();
  bool UserDebugFlagOn();
  bool UserStudentVieWOn();
  bool CheckConsistency();
  int CallSystemNoOutput(const std::string& systemCommand, bool logErrors);
  std::string CallSystemWithOutput(const std::string& systemCommand);
  void ChDir(const std::string& systemCommand);
  std::string ToStringCalculatorComputation(const std::string& computation, const std::string &display);
  std::string ToStringCalcArgsNoNavigation(List<std::string>* tagsToExclude);
  std::string ToStringHTMLTopCommandLinuxSystem();
  std::string ToStringFolderInfo() const;
  std::string ToStringThreadData(bool useHTML);
  std::string ToStringProgressReportNoThreadData(bool useHTML);
  std::string ToStringProgressReportWithThreadData(bool useHTML);
  std::string ToStringProgressReportConsole();
  WebServer& server();
  void MakeReport();
  class Test {
  public:
    static bool All();
    static bool Builds();
  };
  /// @endcond
};

template <typename theType>
logger& logger::doTheLogging(const theType& toBePrinted) {
  this->initializeIfNeeded();
  std::stringstream out;
  out << toBePrinted;
  this->bufferStandardOutput += out.str();

  if (!this->flagStopWritingToFile) {
    this->bufferFile += out.str();
  }
  this->CheckLogSize();
  return *this;
}
#endif

