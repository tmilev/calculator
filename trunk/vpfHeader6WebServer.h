//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderWebServerAlreadyDefined
#define vpfHeaderWebServerAlreadyDefined
#include "vpfHeader6WebServerInterprocessLogistics.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

class WebServer;

class WebWorker
{
public:
  WebServer* parent;
  bool flagInUse;
  bool flagUsingSSLinCurrentConnection;
  int indexInParent;
  int ProcessPID;
  std::string displayUserInput;

  std::string userAddress;
  std::string theMessage;
  std::string mainArgumentRAW;
  std::string mainAddresSRAW;
  std::string mainAddress;
  std::string PhysicalFileName;
  std::string status;
  std::string pingMessage;
  double timeOfLastPingServerSideOnly;
  bool flagDeallocated;
  bool flagMainAddressSanitized;
  std::string mainAddressNonSanitized;
  List<std::string> theStrings;
  int ContentLength;
  int requestType;
  int connectedSocketID;
  int connectedSocketIDLastValueBeforeRelease;
  int connectionID;
  List<char> remainingBytesToSend;
  List<char> bufferFileIO;
  PauseController PauseComputationReportReceived;
  PauseController PauseWorker;
  PauseController PauseIndicatorPipeInUse;
  Pipe pipeServerToWorkerRequestIndicator;
  Pipe pipeWorkerToServerControls;
  Pipe pipeWorkerToServerTimerPing;
  Pipe pipeWorkerToServerIndicatorData;
  Pipe pipeWorkerToServerWorkerStatus;
  Pipe pipeWorkerToServerUserInput;

  std::string error;

  int ProcessServerStatus();
  int ProcessComputationIndicator();
  int ProcessNonCalculator();
  int ProcessFolder();
  int ProcessPauseWorker();
  int ProcessMonitor();
  int ProcessUnknown();

  int ServeClient();
  void QueueStringForSending(const std::string& stringToSend, bool MustSendAll=false);
  bool CheckConsistency();
  void PipeProgressReportToParentProcess(const std::string& input);
  void WriteProgressReportToFile(const std::string& input);

  static void StandardOutputAfterTimeOut(const std::string& input);

  static int OutputWeb();
  static void OutputBeforeComputation();
  static void OutputStandardResult();
  static void OutputSendAfterTimeout(const std::string& input);
  static void OutputResultAfterTimeout();
  static void OutputCrashAfterTimeout();
  void OutputShowIndicatorOnTimeout();

  void QueueBytesForSending
  (const List<char>& bytesToSend, bool MustSendAll=false)
  ;
  void SignalIamDoneReleaseEverything();
  void reset();
  void resetMessageComponentsExceptRawMessage();
  void Release();
  void ReleaseKeepInUseFlag();
  void SendAllBytes();
  void SendAllBytesHttp();
  void SendAllBytesHttpSSL();
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);
  static std::string GetJavaScriptIndicatorFromHD();
  std::string GetJavaScriptIndicatorBuiltInServer(int inputIndex);
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveOnce();
  bool ReceiveAll();
  bool ReceiveAllHttp();
  bool ReceiveAllHttpSSL();
  void SendDisplayUserInputToServer();
  enum requestTypes {requestUnknown, requestGetCalculator, requestTogglePauseCalculator,
  requestPostCalculator, requestGetNotCalculator, requestGetServerStatus,
  requestGetComputationIndicator, requestGetMonitor};
  std::string ToStringStatus()const;
  std::string ToStringMessage()const;
  std::string ToStringMessageShort(FormatExpressions* theFormat=0)const;
  std::string ToStringMessageFull()const;
  void ParseMessage();
  void ExtractArgumentFromAddress();
  void SanitizeMainAddress();
  void ExtractPhysicalAddressFromMainAddress();
};

class WebServer
{
public:
  static const int maxNumPendingConnections;     // how many pending connections queue will hold
  int listeningSocketHTTP;
  int listeningSocketHttpSSL;
  bool flagTryToKillOlderProcesses;
  bool flagUsESSL;
  List<std::string> PortsITryHttp;
  List<std::string> PortsITryHttpSSL;
  ListReferences<WebWorker> theWorkers;

  int activeWorker;
  int timeLastExecutableModification;
  List<std::string> theProgressReports;
  bool flagDeallocated;
  bool flagPort8155;
  WebServer();
  ~WebServer();
  static int main(int argc, char **argv);
  static int main_apache_client();
  static int main_command_input();
  static void AnalyzeMainArguments(int argC, char **argv);
  static void InitializeGlobalVariables();
  void ReleaseWorkerSideResources();
  void ReleaseActiveWorker();
  void ReleaseSocketsNonActiveWorkers();
  void ReleaseNonActiveWorkers();
  void ReleaseEverything();
  void CreateNewActiveWorker();
  bool CheckConsistency();
  int Run();
  WebWorker& GetActiveWorker();
  static void WorkerTimerPing(double pingTime);
  static void Release(int& theDescriptor);
  static void SignalActiveWorkerDoneReleaseEverything();
  static void FlushActiveWorker();
  static void ReturnActiveIndicatorAlthoughComputationIsNotDone();
  static void SendStringThroughActiveWorker(const std::string& input);
  static void PipeProgressReportToParentProcess(const std::string& input);
  static void Signal_SIGINT_handler(int s);
  static void Signal_SIGCHLD_handler(int s);
  static std::string ToStringActiveWorker();
  bool initPrepareWebServerALL();
  void initPrepareSignals();
  bool initBindToPorts();
  void initPortsITry();
  void initSSL();
  void SSLfreeResources();
  void SSLServerSideHandShake();
  void SSLfreeEverythingShutdownSSL();
  void RecycleChildrenIfPossible();
  void Restart();
  void CheckExecutableVersionAndRestartIfNeeded();
  void initDates();
  std::string ToStringLastErrorDescription();
  std::string ToStringStatusActive();
  std::string ToStringStatusAll();
};

class ProgressReportWebServer
{
  public:
  int indexProgressReport;
  bool flagDeallocated;
  static bool flagServerExists;
  ProgressReportWebServer(const std::string& inputStatus);
  ProgressReportWebServer();
  void SetStatus (const std::string& inputStatus);
  ~ProgressReportWebServer();
};
#endif
