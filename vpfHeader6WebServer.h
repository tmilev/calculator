//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderWebServerAlreadyDefined
#define vpfHeaderWebServerAlreadyDefined
#include <unistd.h>
#include <string.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
#include "vpfHeader4SystemFunctionsGlobalObjects.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

class WebServer;

class PauseController
{
public:
  List<int> thePausePipe; //thePipe[0] is the read end; thePipe[1] is the write end.
  List<int> mutexPipe;
  List<char> buffer;
  std::string ToString()const;
  void Release();
  void CreateMe();

  bool CheckPauseIsRequested();
  void PauseIfRequested();
  void PauseIfRequestedWithTimeOut();
  void LockMe();
  void UnlockMe();
  PauseController();
};

class Pipe
{
private:
  void ReadNoLocks();
  void WriteNoLocks(const std::string& toBeSent);
public:
  List<int> thePipe; //thePipe[0] is the read end; thePipe[1] is the write end.
  PauseController pipeAvailable;
  List<char> lastRead;
  List<char> pipeBuffer;

  void Read();
  void ReadWithoutEmptying();
  void WriteAfterEmptying(const std::string& toBeSent);

  std::string ToString()const;
  void Release();
  void CreateMe();
  ~Pipe();
  Pipe()
  { this->thePipe.initFillInObject(2,-1);
  }
};

class WebWorker
{
public:
  WebServer* parent;
  bool flagInUse;
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
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);
  static std::string GetJavaScriptIndicatorFromHD();
  std::string GetJavaScriptIndicatorBuiltInServer(int inputIndex);
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveOnce();
  bool ReceiveAll();
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
  int listeningSocketID;
  bool flagTryToKillOlderProcesses;
  ListReferences<WebWorker> theWorkers;
  int activeWorker;
  int timeLastExecutableModification;
  List<std::string> theProgressReports;
  bool flagDeallocated;
  WebServer();
  ~WebServer();
  void ReleaseWorkerSideResources();
  void ReleaseActiveWorker();
  void ReleaseSocketsNonActiveWorkers();
  void ReleaseNonActiveWorkers();
  void CreateNewActiveWorker();
  bool CheckConsistency();
  int Run();
  WebWorker& GetActiveWorker();
  static void Release(int& theDescriptor);
  static void SignalActiveWorkerDoneReleaseEverything();
  static void FlushActiveWorker();
  static void ReturnActiveIndicatorAlthoughComputationIsNotDone();
  static void SendStringThroughActiveWorker(const std::string& input);
  static void PipeProgressReportToParentProcess(const std::string& input);
  static void Signal_SIGINT_handler(int s);
  static void Signal_SIGCHLD_handler(int s);
  static std::string ToStringActiveWorker();
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
  ProgressReportWebServer(const std::string& inputStatus);
  ProgressReportWebServer();
  void SetStatus (const std::string& inputStatus);
  ~ProgressReportWebServer();
};
#endif
