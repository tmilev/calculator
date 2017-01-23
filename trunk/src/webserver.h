#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "../../calculator/src/vpfHeader6WebServerInterprocessLogistics.h"
#include "../../calculator/src/vpfHeader4SystemFunctionsGlobalObjects.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

class WebServer;

class WebWorker
{
public:
  WebServer* parent;
  int indexInParent;
  int ProcessPID;
  std::string displayUserInput;

  std::string messageHead;
  std::string messageBody;

  MonomialWrapper<std::string, MathRoutines::hashString> userAddress;
  std::string hostWithPort;
  std::string hostNoPort;
  std::string cookiesApache;
  List<std::string> cookies;
  List<std::string> connectionFlags;
  std::string addressGetOrPost;
  std::string lastHeadMessage;
  std::string addressComputed;
  std::string argumentComputed;
  std::string VirtualFileName;
  std::string RelativePhysicalFileNamE;
  std::string status;
  std::string pingMessage;
  double timeServerAtWorkerStart;
  double timeOfLastPingServerSideOnly;
  bool flagInUse;
  bool flagDeallocated;
  bool flagKeepAlive;
  bool flagFileNameSanitized;
  bool flagAuthenticationTokenWasSubmitted;
  bool flagPasswordWasSubmitted;
  bool flagFoundMalformedFormInput;
  bool flagProgressReportAllowed;
  bool flagArgumentsAreOK;
  bool flagDoAddContentLength;
  bool flagMustLogin;
  bool flagUsingSSLInWorkerProcess;
  bool flagDidSendAll;
  bool flagToggleMonitoring;
  List<std::string> theStrings;
  int ContentLength;
  int requestTypE;
  int connectedSocketID;
  int connectedSocketIDLastValueBeforeRelease;
  int connectionID;
  int indentationLevelHTML;
  List<char> remainingBytesToSenD;
  List<char> remainingHeaderToSend;
  List<char> remainingBodyToSend;
  List<char> bufferFileIO;
  PauseProcess PauseComputationReportReceived;
  PauseProcess PauseWorker;
  PauseProcess PauseIndicatorPipeInUse;
  Pipe pipeServerToWorkerRequestIndicator;
  Pipe pipeWorkerToServerControls;
  Pipe pipeWorkerToServerTimerPing;
  Pipe pipeWorkerToServerIndicatorData;
  Pipe pipeWorkerToServerWorkerStatus;
  Pipe pipeWorkerToServerUserInput;

  std::string error;
  std::string errorCalculatorArguments;
  void PrepareFullMessageHeaderAndFooter();
  std::string openIndentTag(const std::string& theTag);
  std::string closeIndentTag(const std::string& theTag);
  int ProcessSetProblemDatabaseInfo();
  int ProcessClonePage();
  int ProcessModifyPage();
  int ProcessAddUserEmails();
  int ProcessServerStatus();
  int ProcessToggleMonitoring();
  int ProcessServerStatusPublic();
  int ProcessComputationIndicator();
  int ProcessEditPage();
  int ProcessFolder();
  int ProcessFile();
  int ProcessChangePassword();
  int ProcessChangePasswordPage();
  int ProcessCalculator();
  int ProcessDatabase();
  int ProcessDatabaseOneEntry();
  int ProcessAccounts();
  int ProcessScores();
  int ProcessAssignTeacherToSection();
  int ProcessExamPage();
  int ProcessTemplate();
  int ProcessSelectCourse();
  int ProcessTopicTable();
  int ProcessExamPageInterpreter();
  int ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication);
  int ProcessLoginPage(const std::string& reasonForLogin="");
  int ProcessLogout();
  int ProcessNavigation();
  int ProcessCompute();
  int ProcessCalculatorExamples();
  int ProcessSubmitProblem();
  int ProcessSubmitProblemPreview();
  int ProcessProblemGiveUp();
  int ProcessProblemSolution();
  int ProcessPauseWorker();
  int ProcessMonitor();
  int ProcessUnknown();
  int Run();
  bool CheckConsistency();
  void PipeProgressReportToParentProcess(const std::string& input);
  void WriteProgressReportToFile(const std::string& input);

  static void StandardOutputAfterTimeOut(const std::string& input);

  bool IsFileServedRaw();

  static bool IsAllowedAsRequestCookie(const std::string& input);
  bool Login(std::stringstream& argumentProcessingFailureComments);
  bool CheckRequestsAddressesReturnFalseIfModified();

  bool ExtractArgumentsFromMessage
(const std::string& input, std::stringstream& argumentProcessingFailureComments,
int recursionDepth=0)

  ;
  bool ExtractArgumentsFromCookies
  (std::stringstream& argumentProcessingFailureComments)
  ;
  static void OutputSendAfterTimeout(const std::string& input);
  void OutputResultAfterTimeout();
  static void OutputCrashAfterTimeout();
  void OutputShowIndicatorOnTimeout();
  static std::string GetInsecureConnectionAngryMessage();
  void QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll=false);
  void QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll=false);
  void QueueBytesForSendingNoHeadeR
  (const List<char>& bytesToSend, bool MustSendAll=false)
  ;
  bool ShouldDisplayLoginPage();
  void SendAllAndWrapUp();
  void WrapUpConnection();
  void ResetPipesNoAllocation();
  void reset();
  void resetMessageComponentsExceptRawMessage();
  void Release();
  void ReleaseKeepInUseFlag();
  void SendAllBytesWithHeaders();
  void SendAllBytesNoHeaders();
  void SendAllBytesHttp();
  void SendAllBytesHttpSSL();
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);
  std::string GetJavaScriptIndicatorBuiltInServer();
  std::string GetChangePasswordPage();
  std::string GetClonePageResult();
  std::string GetModifyProblemReport();
  std::string GetLoginPage(const std::string& reasonForLogin="");
  std::string GetLoginHTMLinternal(const std::string& reasonForLogin="");
  std::string GetAuthenticationToken(const std::string& reasonForNoAuthentication="");
  std::string GetEditPageHTML();
  std::string GetExamPageInterpreter();
  std::string GetDatabasePage();
  std::string GetDatabaseOneEntry();
  std::string GetNavigationPage();
  std::string GetAddUserEmails();
  static std::string GetJavaScriptIndicatorFromHD();
  static std::string GetJavascriptSubmitLoginInfo();
  std::string GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken);
  void SetHeaderOKNoContentLength();
  void SetHeadeR(const std::string& httpResponseNoTermination, const std::string& remainingHeaderNoTermination);
  std::string GetHeaderConnectionClose();
  std::string GetHeaderSetCookie();
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveAll();
  void AttemptUnknownRequestErrorCorrection();
  bool ReceiveAllHttp();
  bool ReceiveAllHttpSSL();
  std::string ToStringSSLError(int errorCode);
  void SendDisplayUserInputToServer();
  enum requestTypes {requestUnknown, requestGet, requestPost, requestHead, requestChunked};
  std::string ToStringStatus()const;
  std::string ToStringMessageUnsafe(bool useHTML=true)const;
  std::string ToStringMessageShortUnsafe(FormatExpressions* theFormat=0)const;
  std::string ToStringMessageFullUnsafe(bool useHTML=true)const;
  void ParseMessageHead();
  void ExtractHostInfo();
  void ExtractAddressParts();
  void SanitizeVirtualFileName();
  int ServeClient();
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
  int WebServerPingIntervalInSeconds;
  long long NumFailedSelectsSoFar;
  long long NumSuccessfulSelectsSoFar;
  bool flagTryToKillOlderProcesses;
  bool flagSSLHandshakeSuccessful;
  bool flagReapingChildren;
  bool flagThisIsWorkerProcess;
  MonomialCollection<MonomialWrapper<std::string, MathRoutines::hashString>, LargeInt> currentlyConnectedAddresses;
  List<std::string> PortsITryHttp;
  List<std::string> PortsITryHttpSSL;
  std::string httpPort;
  std::string httpSSLPort;
  List<int> theListeningSockets;
//  List<int> theListeningSocketsReadyToAccept;
  ListReferences<WebWorker> theWorkers;

  List<std::string> requestStartsNotNeedingLogin;
  List<std::string> addressStartsNotNeedingLogin;
  List<std::string> addressStartsSentWithCacheMaxAge;

  int activeWorker;
  int timeLastExecutableModification;
  ListReferences<std::string> theProgressReports;
  bool flagDeallocated;
  bool flagPort8155;
  WebServer();
  ~WebServer();
  static void AnalyzeMainArguments(int argC, char **argv);
  static void InitializeGlobalVariables();
  bool RequiresLogin(const std::string& inputRequest, const std::string& inputAddress);

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
  void RecycleChildrenIfPossible();
  void HandleTooManyConnections(const std::string& incomingUserAddress);
  void Restart();
  void CheckExecutableVersionAndRestartIfNeeded(bool callReload);
  void initDates();
  std::string ToStringConnection();
  std::string ToStringLastErrorDescription();
  std::string ToStringStatusActive();
  std::string ToStringStatusAll();
  std::string ToStringStatusPublic();
  std::string ToStringStatusPublicNoTop();
  static void TurnProcessMonitoringOn();
  static void TurnProcessMonitoringOff();
  static void ToggleProcessMonitoring();
  static std::string GetEnvironment(const std::string& envVarName);
  static int main(int argc, char **argv);
  static int mainApache();
  static int mainCommandLine();
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




#endif // WEBSERVER_H

