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

class Pipe
{
private:
  void ReadFileDescriptor(int readEnd, bool doNotBlock);
public:
  List<int> thePipe; //thePipe[0] is the read end; thePipe[1] is the write end.
  List<int> pipeEmptyingBlocksRead;
  List<int> pipeEmptyingBlocksWrite;
  List<int> pipeIndicesOnCreation;
  List<char> lastRead;
  List<char> pipeBuffer;

  void Read(bool doNotBlock);
  void Write(const std::string& toBeSent, bool doNotBlock);
  void WriteAfterClearingPipe(const std::string& toBeSent, bool doNotBlock);

  std::string ToString()const;
  void Release();
  void CreateMe();
  ~Pipe();
  Pipe()
  { this->thePipe.initFillInObject(2,-1);
    this->pipeIndicesOnCreation.initFillInObject(2,-1);
    this->pipeEmptyingBlocksRead.initFillInObject(2,-1);
    this->pipeEmptyingBlocksWrite.initFillInObject(2,-1);
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
  List<std::string> theStrings;
  int ContentLength;
  int requestType;
  int connectedSocketID;
  int connectedSocketIDLastValueBeforeRelease;
  int connectionID;
  List<char> remainingBytesToSend;
  List<char> bufferFileIO;
  Pipe pipeServerToWorkerComputationReportReceived;
  Pipe pipeServerToWorkerEmptyingPausesWorker;
  Pipe pipeServerToWorkerRequestIndicator;
  Pipe pipeWorkerToServerControls;
  Pipe pipeWorkerToServerIndicatorData;
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
  void ReleaseServerSideResources();
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
  void ExtractPhysicalAddressFromMainAddress();
  void reset();
  void resetMessageComponentsExceptRawMessage();
};

class WebServer
{
public:
  int listeningSocketID;
  bool flagUsingBuiltInServer;
  bool flagTryToKillOlderProcesses;
  List<WebWorker> theWorkers;
  int activeWorker;
  int timeLastExecutableModification;
  WebServer();
  ~WebServer();
  void ReleaseWorkerSideResources();
  void ReleaseActiveWorker();
  void ReleaseSocketsNonActiveWorkers();
  void ReleaseNonActiveWorkers();
  void CreateNewActiveWorker();
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
#endif
