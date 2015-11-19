//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderLoggingGlobalVars_already_defined
#define vpfHeaderLoggingGlobalVars_already_defined
#include "vpfHeader1General0_General.h"
#include "vpfHeader1General1_ListReferences.h"


static ProjectInformationInstance projectInfoHeaderLoggingRoutines
(__FILE__, "Logging routines, global variables. ");

class GlobalVariables
{
  //I love doxygen!
  /// @cond
private:
  double (*getElapsedTimePrivate)();
  void (*callSystem)(const std::string& theSystemCommand);
  void (*callChDir)(const std::string& theDirectoryName);
public:
  void (*IndicatorStringOutputFunction)(const std::string& input);
  void (*sleepFunction)(int microseconds);
  void (*WebServerReturnDisplayIndicatorCloseConnection)();
  void (*WebServerTimerPing)(double pingTime);
  void (*PauseUponUserRequest)();
  void Pause()
  { if (this->PauseUponUserRequest!=0)
      this->PauseUponUserRequest();
  }
  void FallAsleep(int microseconds)
  { if (this->sleepFunction!=0)
      this->sleepFunction(microseconds);
  }
//  double MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit;
  double MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  double MaxComputationTimeBeforeWeTakeAction;
//  bool flagLogInterProcessCommunication;
  bool flagUsingBuiltInWebServer;
  bool flagAllowUseOfThreadsAndMutexes;
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
//progress report flags:
  bool flagReportEverything;
  bool flagReportFileIO;
  bool flagReportLargeIntArithmetic;
  bool flagReportGaussianElimination;
  bool flagReportProductsMonomialAlgebras;

  int progressReportStringsRegistered;
  List<stackInfo> CustomStackTrace;
  MutexRecursiveWrapper infoIsInitialized;
  ListReferences<ThreadData> theThreadData;
  List<std::string> ProgressReportStringS;
  Controller theLocalPauseController;

  static const std::string hopefullyPermanentWebAdressOfServerExecutable;
  static const std::string hopefullyPermanentWebAdressOfServerOutputFolder;

  std::string userInputStringRAWIfAvailable;
  std::string userInputStringIfAvailable;

  std::string PhysicalNameProgressReport;
  std::string PhysicalNameCrashLog;
  std::string PhysicalNameOutpuT;

  std::string PhysicalPathServerBase;
  std::string PhysicalPathOutputFolder;
  std::string PhysicalNameIndicatorWithPath;
  std::string PhysicalNameExtraOutputNoPatH;
  std::string PhysicalNameExtraOutputWithPath;
  std::string PhysicalNameFolderBelowExecutable;
  std::string PhysicalNameExecutableNoPath;
  std::string PhysicalNameExecutableWithPath;

  std::string DisplayPathServerBase;
  std::string DisplayPathExecutable;
  std::string DisplayPathOutputFolder;
  std::string DisplayNameExtraOutputNoPath;
  std::string DisplayNameExtraOutputWithPath;
  std::string DisplayNameExecutableWithPath;
  std::string DisplayNameIndicatorWithPath;
  std::string DisplayNameCalculatorWithPath;


  std::string IPAdressCaller;
  std::string defaultUserLabel;

  MutexRecursiveWrapper MutexWebWorkerPipeWriteLock;
  MutexRecursiveWrapper MutexWebWorkerPipeReadLock;
  MutexRecursiveWrapper MutexWebWorkerStaticFiasco;
  MutexRecursiveWrapper MutexRegisterFunctionStaticFiasco;
  MutexRecursiveWrapper MutexParallelComputingStaticFiasco;
  MutexRecursiveWrapper MutexProgressReporting;
  MutexRecursiveWrapper MutexRegisterNewThread;

  MemorySaving<Matrix<Rational> > matIdMatrix;
  MemorySaving<Matrix<Rational> > matOneColumn;


  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;

  MemorySaving<SubgroupWeylGroupOLD> subGroupActionNormalizerCentralizer;

  MemorySaving<GroebnerBasisComputation<Rational> > theGroebnerBasisComputation;

  GlobalVariables();
  static HashedList<FileInformation>& theSourceCodeFiles();
  void SetTimerFunction(double (*timerFunction)())
  { this->getElapsedTimePrivate=timerFunction;
  }
  double GetElapsedSeconds()
  { if (this->getElapsedTimePrivate!=0)
      return this->getElapsedTimePrivate();
    return -1;
  }
  void initDefaultFolderAndFileNames
(const std::string& inputPhysicalExecutableWithPathServerBaseIsFolderBelow,
 const std::string& scrambledIP);
  void initOutputReportAndCrashFileNames
(const std::string& inputUserStringRAW,
 const std::string& inputUserStringCivilized);

//  void operator=(const GlobalVariables& other)
//  { this->IndicatorStringOutputFunction=other.IndicatorStringOutputFunction;
//    this->theDrawingVariables=other.theDrawingVariables;
//  }
//  inline void DrawBufferNoIniT()
//  { this->theDrawingVariables.drawBufferNoIniT();
//  }
  void SetCallSystem(void (*theSystemCall)(const std::string&))
  { this->callSystem=theSystemCall;
  }
  void SetCallChDir(void (*theChDir)(const std::string&))
  { this->callChDir=theChDir;
  }
  void System(const std::string& systemCommand)
  { if (this->callSystem!=0)
      this->callSystem(systemCommand);
  }
  void ChDir(const std::string& systemCommand)
  { if (this->callChDir!=0)
      this->callChDir(systemCommand);
  }
  std::string ToStringSourceCodeInfo();
  std::string ToStringFolderInfo()const;
  inline void MakeReport(const std::string& input)
  { if (this->IndicatorStringOutputFunction!=0)
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
  std::fstream theFile;
  bool flagStopWritingToFile;
  logger(const std::string& logFileName);
  void CheckLogSize();
  enum loggerSpecialSymbols{ endL, red, blue, yellow, green, purple, cyan, normalColor};
  std::string closeTagConsole();
  std::string closeTagHtml();
  std::string openTagConsole();
  std::string openTagHtml();
  logger& operator << (const loggerSpecialSymbols& input);
  template <typename theType>
  logger& operator << (const theType& toBePrinted)
  { if (theGlobalVariables.flagUsingBuiltInWebServer)
      std::cout << toBePrinted;
    this->CheckLogSize();
    if (this->flagStopWritingToFile)
      return *this;
    std::stringstream out;
    out << toBePrinted;
    theFile << out.str();
    return *this;
  }
};


#endif

