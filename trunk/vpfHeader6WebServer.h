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
  int indexInParent;
  int ProcessPID;
  std::string displayUserInput;

  MonomialWrapper<std::string, MathRoutines::hashString> userAddress;
  std::string theMessage;
  std::string mainArgumentRAW;
  std::string mainAddresSRAW;
  std::string mainAddress;
  std::string RelativePhysicalFileName;
  std::string PhysicalFileName_COMPUTED_DO_NOT_CHANGE;
  std::string status;
  std::string pingMessage;
  std::string authenticationToken;
  double timeOfLastPingServerSideOnly;
  bool flagDeallocated;
  bool flagMainAddressSanitized;
  bool flagAuthenticationTokenWasSubmitted;
  bool flagPasswordWasSubmitted;
  bool flagFoundMalformedFormInput;
  std::string mainAddressNonSanitized;
  List<std::string> theStrings;
  int ContentLength;
  int requestType;
  int connectedSocketID;
  int connectedSocketIDLastValueBeforeRelease;
  int connectionID;
  int indentationLevelHTML;
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

  std::string openIndentTag(const std::string& theTag);
  std::string closeIndentTag(const std::string& theTag);
  int ProcessSetProblemDatabaseInfo();
  int ProcessClonePage();
  int ProcessModifyPage();
  int ProcessAddUserEmails();
  int ProcessServerStatus();
  int ProcessComputationIndicator();
  int ProcessEditPage();
  int ProcessNonCalculator();
  int ProcessFolder();
  int ProcessChangePassword();
  int ProcessChangePasswordPage();
  int ProcessDatabase();
  int ProcessExamPage();
  int ProcessLoginPage();
  int ProcessLogout();
  int ProcessCalculatorExamples();
  int ProcessSubmitProblem();
  int ProcessSubmitProblemPreview();
  int ProcessProblemGiveUp();
  int ProcessPauseWorker();
  int ProcessMonitor();
  int ProcessUnknown();
  int ServeClient();
  void QueueStringForSending(const std::string& stringToSend, bool MustSendAll=false);
  bool CheckConsistency();
  void PipeProgressReportToParentProcess(const std::string& input);
  void WriteProgressReportToFile(const std::string& input);

  static void StandardOutputAfterTimeOut(const std::string& input);

  bool IsAllowedAsRequestCookie(const std::string& input);
  int ProcessCalculator();
  bool ProcessRawArguments
  (const std::string& urlEncodedInputString, std::stringstream& argumentProcessingFailureComments, int recursionDepth=0)
  ;
  void OutputBeforeComputation();
  void OutputBeforeComputationUserInputAndAutoComplete();
  void OutputStandardResult();
  static void OutputSendAfterTimeout(const std::string& input);
  void OutputResultAfterTimeout();
  static void OutputCrashAfterTimeout();
  void OutputShowIndicatorOnTimeout();
  static std::string GetInsecureConnectionAngryMessage();
  void QueueBytesForSending
  (const List<char>& bytesToSend, bool MustSendAll=false)
  ;
  bool ShouldDisplayLoginPage();
  void SignalIamDoneReleaseEverything();
  void reset();
  void resetMessageComponentsExceptRawMessage();
  void Release();
  void ReleaseKeepInUseFlag();
  void SendAllBytes();
  void SendAllBytesHttp();
  void SendAllBytesHttpSSL();
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);
  std::string GetJavaScriptIndicatorBuiltInServer(int inputIndex, bool callProgressReport);
  std::string GetChangePasswordPage();
  std::string GetClonePageResult();
  std::string GetModifyProblemReport();
  std::string GetLoginPage();
  std::string GetLoginHTMLinternal();
  std::string GetEditPageHTML();
  std::string GetExamPage();
  std::string GetDatabasePage();
  std::string GetAddUserEmails();
  std::string GetSetProblemDatabaseInfoHtml();
  static std::string GetJavaScriptIndicatorFromHD();
  static std::string GetJavascriptHideHtml();
  std::string GetHtmlHiddenInputExercise();
  std::string GetHtmlHiddenInputComputation();
  std::string GetHtmlHiddenInputs();
  std::string GetJavascriptStandardCookies();
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveAll();
  bool ReceiveAllHttp();
  bool ReceiveAllHttpSSL();
  void SendDisplayUserInputToServer();
  enum requestTypes {requestUnknown, requestGetCalculator, requestPostCalculator,
  requestGetNotCalculator};
  static std::string ToStringCalculatorArgumentsHumanReadable();
  std::string ToStringStatus()const;
  std::string ToStringMessageUnsafe()const;
  std::string ToStringMessageShortUnsafe(FormatExpressions* theFormat=0)const;
  std::string ToStringMessageFullUnsafe()const;
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
  int highestSocketNumber;
  int MaxNumWorkersPerIPAdress;
  int MaxTotalUsedWorkers;
  int NumConnectionsSoFar;
  int NumProcessesReaped;
  int NumProcessAssassinated;
  int NumWorkersNormallyExited;
  long long NumFailedSelectsSoFar;
  long long NumSuccessfulSelectsSoFar;
  bool flagTryToKillOlderProcesses;
  bool flagSSLHandshakeSuccessful;
  bool flagReapingChildren;
  MonomialCollection<MonomialWrapper<std::string, MathRoutines::hashString>, LargeInt> currentlyConnectedAddresses;
  List<std::string> PortsITryHttp;
  List<std::string> PortsITryHttpSSL;
  std::string httpPort;
  std::string httpSSLPort;
  List<int> theListeningSockets;
//  List<int> theListeningSocketsReadyToAccept;
  ListReferences<WebWorker> theWorkers;

  int activeWorker;
  int timeLastExecutableModification;
  ListReferences<std::string> theProgressReports;
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
  bool CreateNewActiveWorker();
  bool EmergencyRemoval_LastCreatedWorker();
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
  void ReapChildren();
  static std::string ToStringActiveWorker();
  bool initPrepareWebServerALL();
  void initPrepareSignals();
  bool initBindToPorts();
  void initPortsITry();
  void initListeningSockets();
  void initSSL();
  void SSLfreeResources();
  void SSLServerSideHandShake();
  void SSLfreeEverythingShutdownSSL();
  void TerminateChildSystemCall(int i);
  void RecycleChildrenIfPossible(const std::string& incomingUserAddress);
  void Restart();
  void CheckExecutableVersionAndRestartIfNeeded(bool callReload);
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
