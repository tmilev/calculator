//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderLoggingGlobalVars_already_defined
#define vpfHeaderLoggingGlobalVars_already_defined
#include "vpfHeader1General0_General.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader1General6Maps.h"
#include "vpfHeader1General8DatabaseSystemIndependent.h"
#include "vpfHeader1General8DatabaseSystemIndependent.h"
#include <thread>

static ProjectInformationInstance projectInfoHeaderLoggingRoutines(__FILE__, "Logging routines, global variables. ");

class ThreadData
{
public:
  std::thread::id theId;
  int index;
  std::string name;
  std::string ToStringHtml()const;
  std::string ToStringConsole()const;
  static int getCurrentThreadId();
  static void RegisterFirstThread(const std::string& inputName = "");
  static ThreadData& RegisterNewThread(const std::string& inputName = "");
  static void CreateThread(void (*InputFunction)(int), const std::string& inputName);
  static std::string ToStringAllThreadsHtml();
  static std::string ToStringAllThreadsConsole();
  ThreadData();
  ~ThreadData();
};

struct ProcessTypes{
  static std::string worker;
  static std::string server;
  static std::string serverMonitor;
};

class GlobalVariables
{
  //Warning: please pay attention to the static initialization order fiasco.
  //The fiasco states that global objects (allocated before main)
  //may be allocated in an unexpected order.
  //In particular an object allocated before main cannot assume that the constructor of theGlobalVariables
  //object has already been called.
  //In particular one should avoid declaring objects at global scope as
  //the constructors of those may rely on the theGlobalVariables object.
  //A possible "horror" scenario: the programmer decides to register a stack trace
  //in the constructor of an object. That runs just fine. One year later, the programmer decides to
  //declare a global object of that type, and again everything runs just fine as theGlobalVariables
  //happens to be initialized before that object.
  //Finally, two years later, the same programmer decides to declare a global object of the same type in
  //a file being compiled before the declaration of theGlobalVariables. This causes a nasty and difficult to catch
  //crash before main.
  /// @cond
private:
  double (*getElapsedTimePrivate)();
public:
  void (*pointerCallSystemNoOutput)(const std::string& theSystemCommand, bool ignoreOutput);
  std::string (*pointerCallSystemWithOutput)(const std::string& theSystemCommand);
  void (*pointerCallChDir)(const std::string& theDirectoryName);
  void (*IndicatorStringOutputFunction)(const std::string& input);
  void (*sleepFunction)(int microseconds);
  void (*WebServerReturnDisplayIndicatorCloseConnection)();
  void (*WebServerTimerPing)(double pingTime);
  void (*PauseUponUserRequest)();
  void Pause()
  { if (this->PauseUponUserRequest != 0)
      this->PauseUponUserRequest();
  }
  void FallAsleep(int microseconds)
  { if (this->sleepFunction != 0)
      this->sleepFunction(microseconds);
  }
//  double MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit;
  double MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  double MaxTimeNoPingBeforeChildIsPresumedDead;
  double MaxComputationTimeBeforeWeTakeAction;
//  bool flagLogInterProcessCommunication;
//flags: what mode are we running in?

  bool flagRunningApache;
  bool flagRunningCommandLine;
  bool flagRunningConsoleTest;
  bool flagRunningBuiltInWebServer;

//special flags
  bool flagRunningAce;
  bool flagAceIsAvailable;
//webserver flags and variables
  bool flagRunServerOnEmptyCommandLine;
  bool flagCachingInternalFilesOn;
  bool flagServerDetailedLog;
  bool flagIsChildProcess;
  std::string processType;

  bool flagLoggedIn;
  bool flagLogInAttempted;
  bool flagUsingSSLinCurrentConnection;
  bool flagSSLisAvailable;
  bool flagCertificatesAreOfficiallySigned;
  bool flagCrashInitiated;
  std::string buildVersionSimple;
  std::string buildHeadHashWithServerTime;
  std::string OperatingSystem;
  std::string hostWithPort;
  std::string hostNoPort;

//internal infrastructure flags
  bool flagServerForkedIntoWorker;
  bool flagNotAllocated;

  MemorySaving<FormatExpressions> theDefaultFormat;
  MemorySaving<FormatExpressions> thePolyFormat;
//status flags:
  bool flagComputationCompletE;
  bool flagComputationStarted;
  bool flagTimeOutExplanationAlreadyDisplayed;
  bool flagTimedOutComputationIsDone;
  bool flagOutputTimedOut;
  bool flagComputationFinishedAllOutputSentClosing;
//experimental flags, known to cause stability issues when set to true:
  bool flagAllowProcessMonitoring;
//progress report flags:  
//  bool flagPreparingReport;
  bool flagReportEverything;
  bool flagReportFileIO;
  bool flagReportLargeIntArithmetic;
  bool flagReportGaussianElimination;
  bool flagReportProductsMonomialAlgebras;

  MutexRecursiveWrapper infoIsInitialized;
  ListReferences<std::thread>theThreads;
  ListReferences<ThreadData> theThreadData;
  ListReferences<ListReferences<stackInfo> > CustomStackTrace;
  ListReferences<ListReferences<std::string> > ProgressReportStringS;
  List<std::string> programArguments;

  MapLisT<std::string, std::string, MathRoutines::hashString> webArguments;
  MapLisT<std::string, std::string, MathRoutines::hashString> CookiesToSetUsingHeaders;
  UserCalculatorData userDefault;
  std::string userCalculatorRequestType;

  JSData configuration;
  PauseThread theLocalPauseController;

  static std::string hopefullyPermanentWebAdress;
  static std::string hopefullyPermanentWebAdressOfServerExecutable;
  static std::string hopefullyPermanent_HTTPS_WebAdressOfServerExecutable;
  static std::string hopefullyPermanent_HTTPS_WebAdressJavascriptFolder;
  static std::string hopefullyPermanentWebAdressOfServerOutputFolder;

  std::string userInputStringRAWIfAvailable;
  std::string userInputStringIfAvailable;

  std::string RelativePhysicalNameProgressReport;
  std::string RelativePhysicalNameCrashLog;
  std::string RelativePhysicalNameOutpuT;

  std::string PhysicalPathProjectBase;
  std::string PhysicalPathServerBasE;
  std::string PhysicalPathHtmlFolder;
  std::string PhysicalNameExtraOutputNoPatH;
  std::string RelativePhysicalNameExtraOutputWithPath;
  std::string PhysicalNameFolderBelowExecutable;
  std::string PhysicalNameExecutableNoPath;
  std::string PhysicalNameExecutableWithPath;

  std::string DisplayPathExecutable;
  std::string DisplayPathOutputFolder;
  std::string DisplayNameExtraOutputNoPath;
  std::string DisplayNameExtraOutputWithPath;
  //To contain the url of the executable.
  // Example:
  // DisplayNameExecutable="/cgi-bin/calculator";
  std::string DisplayNameExecutable;

  std::string IPAdressCaller;

  MutexRecursiveWrapper MutexWebWorkerPipeWriteLock;
  MutexRecursiveWrapper MutexWebWorkerPipeReadLock;
  MutexRecursiveWrapper MutexWebWorkerStaticFiasco;
  MutexRecursiveWrapper MutexRegisterFunctionStaticFiasco;
  MutexRecursiveWrapper MutexParallelComputingStaticFiasco;
  MutexRecursiveWrapper MutexProgressReportinG;
  MutexRecursiveWrapper MutexRegisterNewThread;

  MemorySaving<Matrix<Rational> > matIdMatrix;
  MemorySaving<Matrix<Rational> > matOneColumn;


  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;
  MemorySaving<GroebnerBasisComputation<Rational> > theGroebnerBasisComputation;

  bool StoreConfiguration();
  bool LoadConfiguration();
  void ComputeConfigurationFlags();
  GlobalVariables();
  ~GlobalVariables();
  static HashedList<FileInformation>& theSourceCodeFiles();
  void WriteSourceCodeFilesJS();
  void SetTimerFunction(double (*timerFunction)())
  { this->getElapsedTimePrivate = timerFunction;
  }
  int GetGlobalTimeInSeconds();
  double GetElapsedSeconds()
  { if (this->getElapsedTimePrivate != 0)
      return this->getElapsedTimePrivate();
    return -1;
  }
  static void InitThreadsExecutableStart();
  bool UserDefaultHasAdminRights();
  bool UserDefaultHasProblemComposingRights();
  bool UserRequestRequiresLoadingRealExamData();
  bool UserRequestMustBePromptedToLogInIfNotLoggedIn();

  std::string ToStringNavigation();
  std::string ToStringCalcArgsNoNavigation(List<std::string>* tagsToExclude);

  static std::string GetDateForLogFiles();
  static std::string GetDateTime();
  void SetWebInpuT(const std::string& inputName, const std::string& inputValue);
  std::string GetWebInput(const std::string& inputName);
  void initDefaultFolderAndFileNames
  (const std::string& inputPhysicalExecutableWithPathServerBaseIsFolderBelow);
  void initOutputReportAndCrashFileNames
  (const std::string& inputUserStringRAW,
   const std::string& inputUserStringCivilized);
  bool UserSecureNonAdminOperationsAllowed();
  bool UserGuestMode();
  bool UserDebugFlagOn();
  bool UserStudentVieWOn();
//  void operator=(const GlobalVariables& other)
//  { this->IndicatorStringOutputFunction=other.IndicatorStringOutputFunction;
//    this->theDrawingVariables=other.theDrawingVariables;
//  }
//  inline void DrawBufferNoIniT()
//  { this->theDrawingVariables.drawBufferNoIniT();
//  }
  void CallSystemNoOutput(const std::string& systemCommand, bool ignoreNonZeroReturn);
  std::string CallSystemWithOutput(const std::string& systemCommand);
  void ChDir(const std::string& systemCommand);
  std::string ToStringHTMLTopCommandLinuxSystem();
  std::string ToStringSourceCodeInfo();
  std::string ToStringFolderInfo()const;
  std::string ToStringProgressReportHtml();
  std::string ToStringProgressReportConsole();
  inline void MakeReport(const std::string& input)
  { if (this->IndicatorStringOutputFunction != 0)
      this->IndicatorStringOutputFunction(input);
  }
  void MakeReport();
  /// @endcond
};
//extern GlobalVariables theGlobalVariables;

class logger
{
  public:
  int currentColor;
  int MaxLogSize;
  std::string theFileName;
  std::string buffer;
  std::fstream theFile;
  bool flagStopWritingToFile;
  bool flagInitialized;
  bool flagTagColorHtmlOpened;
  bool flagTagColorConsoleOpened;
  bool flagResetLogFileWhenTooLarge;
  bool flagWriteImmediately;
  std::string processType;
  logger* carbonCopy;
  logger
  (const std::string& logFileName, logger* inputCarbonCopy,
   bool inputResetLogWhenTooLarge, const std::string& inputProcessType);
  void CheckLogSize();
  enum loggerSpecialSymbols{ endL, red, blue, yellow, green, purple, cyan, normalColor, orange};
  static std::string redConsole();
  static std::string blueConsole();
  static std::string yellowConsole();
  static std::string greenConsole();
  static std::string purpleConsole();
  static std::string cyanConsole();
  static std::string orangeConsole();
  static std::string normalConsole();
  std::string getStamp();
  std::string closeTagConsole();
  std::string closeTagHtml();
  std::string openTagConsole();
  std::string openTagHtml();
  void initializeIfNeeded();
  void reset();
  logger& operator << (const loggerSpecialSymbols& input);
  void flush();
  template <typename theType>
  logger& operator << (const theType& toBePrinted)
  { if (theGlobalVariables.flagRunningApache)
      return *this;
    this->initializeIfNeeded();
    if (!this->flagStopWritingToFile)
    { std::stringstream out;
      if (this->processType != theGlobalVariables.processType)
        out << "WARNING: logger is for process type: " << this->processType
        << " but current process is of type: " << theGlobalVariables.processType << ". ";
      out << toBePrinted;
      this->buffer += out.str();
      if (this->flagWriteImmediately)
      //<- Please do not use flagWriteImmediately
      //unless it is an extremely time-sensitive logging
      //issue such as investigating fork hangups.
      { this->theFile << this->buffer;
        this->theFile.flush();
      }
    }
    if (this->carbonCopy != 0)
      (*(this->carbonCopy)) << toBePrinted;
    else if (theGlobalVariables.flagRunningBuiltInWebServer || theGlobalVariables.flagRunningCommandLine)
      std::cout << toBePrinted;
    this->CheckLogSize();
    return *this;
  }
};


#endif

