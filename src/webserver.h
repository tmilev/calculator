#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "multiprocessing.h"
#include "system_functions_global_objects.h"
#include "transport_layer_security.h"

class WebServer;

class WebWorker {
public:
  WebServer* parent;
  int indexInParent;
  int ProcessPID;
  std::string workerId;
  int numberOfReceivesCurrentConnection;
  int lastResult;
  int64_t millisecondsServerAtWorkerStart;
  int64_t millisecondsLastPingServerSideOnly;
  int64_t millisecondsAfterSelect;

  std::string displayUserInput;

  std::string messageHead;
  std::string messageBody;

  JSData resultWork;

  MonomialWrapper<std::string, MathRoutines::HashString> userAddress;
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
  bool flagInUsE;
  bool flagExited;
  bool flagDeallocated;
  bool flagKeepAlive;
  bool flagFileNameSanitized;
  bool flagFoundMalformedFormInput;
  bool flagArgumentsAreOK;
  bool flagDoAddContentLength;
  bool flagUsingSSLInWorkerProcess;
  bool flagDidSendAll;
  List<std::string> theMessageHeaderStrings;
  MapList<std::string, std::string, MathRoutines::HashString> requestHeaders;
  int ContentLength;
  int requestTypE;
  int connectedSocketID;
  int connectedSocketIDLastValueBeforeRelease;
  int connectionID;
  int indentationLevelHTML;
  int numberOfConsecutiveNoReportsBeforeDisconnect;
  const int maxNumberOfConsecutiveNoReports = 2;
  List<char> remainingBytesToSenD;
  List<char> remainingHeaderToSend;
  List<char> remainingBodyToSend;
  List<char> bufferFileIO;
  MutexProcess PauseWorker;
  MutexProcess writingReportFile;
  PipePrimitive workerToWorkerRequestIndicator;
  PipePrimitive pipeWorkerToWorkerStatus;
  PipePrimitive pipeWorkerToServerControls;
  PipePrimitive pipeWorkerToServerTimerPing;

  WebAPIResponse response;
  std::string error;
  void PrepareFullMessageHeaderAndFooter();
  std::string openIndentTag(const std::string& theTag);
  std::string closeIndentTag(const std::string& theTag);
  JSData ProcessComputationIndicatorJSData();
  int GetIndexIfRunningWorkerId(JSData& outputComputationStatus);
  int ProcessFolderOrFile();
  int ProcessFolder();
  int ProcessFile();
  int ProcessFileDoesntExist();
  int ProcessFileCantOpen();
  int ProcessFileTooLarge(long fileSize);

  JSData SetEmail(const std::string& input);
  bool DoSetEmail(
    UserCalculatorData& inputOutputUser,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive,
    std::stringstream* commentsGeneralSensitive
  );
  int ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication);
  int ProcessLoginNeededOverUnsecureConnection();
  bool ProcessRedirectAwayFromWWW();
  int ProcessUnknown();
  bool RunInitialize();
  int Run();
  bool RunOnce();
  bool FailReceiveReturnFalse();
  bool checkConsistency();

  bool IsFileServedRaw();

  static bool IsAllowedAsRequestCookie(const std::string& input);
  bool LoginProcedure(
    std::stringstream& argumentProcessingFailureComments,
    std::stringstream* comments
  );
  bool CorrectRequestsBEFORELoginReturnFalseIfModified();
  bool CorrectRequestsAFTERLoginReturnFalseIfModified();
  bool RedirectIfNeeded(std::stringstream& argumentProcessingFailureComments);

  bool ExtractArgumentsFromMessage(
    const std::string& input,
    std::stringstream& argumentProcessingFailureComments,
    int recursionDepth = 0
  );
  bool ExtractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments);
  void WriteAfterTimeoutShowIndicator(const std::string& message);
  void WriteAfterTimeoutProgress(const std::string& input, bool forceFileWrite);
  ///////
  void PauseIfRequested();
  // writes json to body, sanitizes.
  bool WriteToBodyJSOn(const JSData& result);
  bool WriteToBody(const std::string& bytesToAppend);
  void WriteAfterTimeoutString(
    const std::string& input,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  void WriteAfterTimeoutJSON(
    const JSData& input,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  void WriteAfterTimeoutPartTwo(
    JSData& result,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  void WriteAfterTimeoutCarbonCopy(
    const JSData& input,
    const std::string& fileNameCarbonCopy
  );
  void GetIndicatorOnTimeout(JSData &output, const std::string& message);
  void QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll = false);
  bool ShouldDisplayLoginPage();
  void WrapUpConnection();
  void ResetMutexProcesses();
  void reset();
  void resetMessageComponentsExceptRawMessage();
  void resetConnection();
  void Release();
  void ReleaseKeepInUseFlag();
  bool EnsureAllBytesSent();
  void SendPending();
  void SendAllBytesNoHeaderS();
  std::string MIMETypeFromFileExtension(const std::string& fileExtension);
  std::string HeaderFromFileExtension(const std::string& fileExtension);

  std::string GetChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField);
  JSData GetClonePageResult();
  std::string GetModifyProblemReport();
  JSData GetSignUpRequestResult();
  std::string GetAuthenticationToken(const std::string& reasonForNoAuthentication = "");
  std::string GetBrowseProblems();
  JSData GetDatabaseJSON();
  std::string GetDatabaseDeleteOneItem();
  std::string GetAddUserEmails();
  std::string GetHtmlHiddenInputs(
    bool includeUserName, bool includeAuthenticationToken
  );
  void SetHeaderOKNoContentLength(
    const std::string& extraHeader,
    const std::string& contentType = "application/json"
  );
  void SetHeader(
    const std::string& httpResponseNoTermination,
    const std::string& remainingHeaderNoTermination
  );

  std::string GetHeaderConnectionClose();
  std::string GetHeaderConnectionKeepAlive();
  std::string GetHeaderSetCookie();
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveAll();
  void AttemptUnknownRequestErrorCorrection();
  bool RequireSSL();
  enum requestTypes {requestUnknown, requestGet, requestPost, requestHead, requestChunked};
  std::string ToStringAddressRequest() const;
  std::string ToStringStatus() const;
  std::string ToStringMessageShort() const;
  std::string ToStringMessageFull() const;
  void ParseMessageHead();
  void ExtractHostInfo();
  void ExtractAddressParts();
  void SanitizeVirtualFileName();
  int ServeClient();
};

class WebServer {
public:
  static const int maxNumPendingConnections;     // how many pending connections queue will hold
  int listeningSocketHTTP;
  int listeningSocketHTTPSOpenSSL;
  int listeningSocketHTTPSBuiltIn;
  int listeningSocketHTTPSDefault;
  int lastListeningSocket;
  int highestSocketNumber;
  int MaxNumWorkersPerIPAdress;
  int MaxTotalUsedWorkers;
  int NumConnectionsSoFar;
  int NumberOfServerRequestsWithinAllConnections;
  int NumProcessesReaped;
  int NumProcessAssassinated;
  int NumWorkersNormallyExited;
  int WebServerPingIntervalInSeconds;
  int previousServerStatReport;
  int previousServerStatDetailedReport;
  int processIdServer;
  long long NumFailedSelectsSoFar;
  long long NumSuccessfulSelectsSoFar;
  bool flagReapingChildren;
  TransportLayerSecurity theTLS;
  LinearCombination<MonomialWrapper<std::string, MathRoutines::HashString>, LargeInteger> currentlyConnectedAddresses;
  std::string portHTTP;
  std::string portHTTPSOpenSSL;
  std::string portHTTPSBuiltIn;
  std::string portHTTPSDefault;
  List<int> theListeningSockets;
  //List<int> theListeningSocketsReadyToAccept;
  ListReferences<WebWorker> theWorkers;
  MapList<std::string, int, MathRoutines::HashString> workerIds;

  HashedList<std::string, MathRoutines::HashString> requestsNotNeedingLogin;
  List<std::string> addressStartsNotNeedingLogin;
  List<std::string> addressStartsSentWithCacheMaxAge;
  HashedList<std::string, MathRoutines::HashString> addressStartsInterpretedAsCalculatorRequest;

  MapList<std::string, std::string, MathRoutines::HashString> MIMETypes;
  int activeWorker;
  int64_t timeLastExecutableModification;
  ListReferences<std::string> theProgressReports;
  bool flagDeallocated;
  WebServer();
  ~WebServer();
  static void FigureOutOperatingSystem();
  void ComputeSSLFlags();
  static void CheckInstallation();
  static void CheckSystemInstallationMongoDB();
  static void CheckSystemInstallationOpenSSL();
  static void CheckMongoDBSetup();
  static void CheckMathJaxSetup();
  static void CheckUnzipInstall();
  static void CheckFreecalcSetup();
  static void AnalyzeMainArguments(int argC, char** argv);
  static bool AnalyzeMainArgumentsTimeString(const std::string& timeLimitString);
  void InitializeBuildFlags();
  void InitializeMainAll();
  void InitializeMainHashes();
  void InitializeMainRequests();
  void InitializeMainAddresses();
  void InitializeMainFoldersInstructorSpecific();
  void initializeMainMIMETypes();

  void MarkChildNotInUse(int childIndex);
  bool RequiresLogin(const std::string& inputRequest, const std::string& inputAddress);
  void ReleaseWorkerSideResources();
  void ReleaseActiveWorker();
  void ReleaseSocketsNonActiveWorkers();
  void ReleaseNonActiveWorkers();
  void ReleaseEverything();
  bool CreateNewActiveWorker();
  bool EmergencyRemoval_LastCreatedWorker();
  bool checkConsistency();
  int Daemon();
  int Run();
  // Wraps the system level fork() call.
  // Addionally computes a
  // unique [with probability ~1] process id and
  // unique [with probability ~1] non-openSSL random bytes
  // for the child and parent processes.
  bool CreateProcessMutex();
  void ComputeActiveWorkerId();
  int Fork();
  void initializeRandomBytes();
  void WriteVersionJSFile();
  WebWorker& GetActiveWorker();
  static void WorkerTimerPing(int64_t pingTime);
  static void Release(int& theDescriptor);
  static void WrapUp();
  static void fperror_sigaction[[noreturn]](int signal);
  void ReapChildren();
  static void Signal_SIGCHLD_handler(int s);
  bool initPrepareWebServerALL();
  void initializeSignals();
  bool initBindToPorts();
  bool initBindToOnePort(const std::string& thePort, int& outputListeningSocket);
  void initPortsITry();
  void initListeningSockets();
  void initSSL();
  bool SSLServerSideHandShake(std::stringstream* commentsOnFailure);
  static void TerminateProcessId(int processId);
  void TerminateChildSystemCall(int i);
  void ProcessOneChildMessage(int childIndex, int& outputNumInUse);
  void RecycleChildrenIfPossible();
  void RecycleOneChild(int childIndex, int& numberInUse);
  void HandleTooManyConnections(const std::string& incomingUserAddress);
  void HandleTooManyWorkers(int& numInUse);
  void StopKillAll();
  bool RestartIsNeeded();
  void initDates();
  std::string ToStringWorkerToWorker();
  std::string ToStringLastErrorDescription();
  std::string ToStringStatusActive();
  std::string ToStringStatusAll();
  std::string ToStringStatusForLogFile();
  std::string ToStringConnectionSummary();
  static void TurnProcessMonitoringOn();
  static void TurnProcessMonitoringOff();
  static std::string GetEnvironment(const std::string& envVarName);
  static int main(int argc, char** argv);
  static int mainConsoleHelp();
  static int mainCommandLine();
};

#endif // WEBSERVER_H

