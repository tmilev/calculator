#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "multiprocessing.h"
#include "system_functions_global_objects.h"
#include "transport_layer_security.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

class WebServer;

class WebWorker {
public:
  WebServer* parent;
  int indexInParent;
  int ProcessPID;
  std::string workerId;
  int numberOfReceivesCurrentConnection;
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
  bool flagAllBytesSentUsingFile;
  bool flagEncounteredErrorWhileServingFile;
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

  std::string error;
  void PrepareFullMessageHeaderAndFooter();
  std::string openIndentTag(const std::string& theTag);
  std::string closeIndentTag(const std::string& theTag);
  int ProcessSetProblemDatabaseInfo();
  int ProcessSlidesOrHomeworkFromSource();
  int ProcessSlidesSource();
  int ProcessClonePage();
  int ProcessModifyPage();
  int ProcessAddUserEmails();
  int ProcessServerStatusJSON();
  int ProcessComputationIndicator();
  JSData ProcessComputationIndicatorJSData();
  int GetIndexIfRunningWorkerId(JSData& outputComputationStatus);
  int ProcessEditPageJSON();
  int ProcessFolderOrFile();
  int ProcessFolder();
  int ProcessFile();
  int ProcessChangePassword(const std::string& reasonForNoAuthentication);
  int SetEmail(const std::string& input);
  bool DoSetEmail(
    UserCalculatorData& inputOutputUser,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive,
    std::stringstream* commentsGeneralSensitive
  );
  int ProcessDatabaseJSON();
  int ProcessDatabaseDeleteEntry();
  int ProcessDatabaseModifyEntry();
  int ProcessAccounts();
  int ProcessAccountsJSON();
  int ProcessActivateAccount();
  int ProcessScores();
  int ProcessApp(bool appendBuildHash);
  int ProcessCalculatorOnePageJS(bool appendBuildHash);
  int ProcessTopicListJSON();
  int ProcessScoresInCoursePage();
  int ProcessAssignTeacherToSection();
  int ProcessExamPageJSON();
  int ProcessTemplate();
  int ProcessTemplateJSON();
  int ProcessLoginUserInfo(const std::string &comments);
  int ProcessSelectCourseJSON();
  int ProcessExamPageInterpreter();
  int ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication);
  int ProcessLoginNeededOverUnsecureConnection();
  bool ProcessRedirectAwayFromWWW();
  int ProcessLogout();
  int ProcessSignUP();
  int ProcessForgotLogin();
  int ProcessCompute();
  int ProcessCalculatorExamplesJSON();
  int ProcessSubmitAnswers();
  int ProcessSubmitAnswersPreview();
  int ProcessProblemGiveUp();
  int ProcessProblemSolution();
  int ProcessPauseWorker();
  int ProcessUnpauseWorker();
  int ProcessUnknown();
  int Run();
  bool CheckConsistency();
  static void StandardOutputAfterTimeOut(const std::string& input);

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
  void WriteAfterTimeoutProgress(const std::string& input, bool forceFileWrite);
  static void WriteAfterTimeoutProgressStatic(const std::string& input);
  void PauseIfRequested() ;
  void WriteAfterTimeoutResult();
  static void WriteAfterTimeout(
    const std::string& input,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  static void WriteAfterTimeoutCarbonCopy(
    const std::string& input,
    const std::string& fileNameCarbonCopy
  );
  static void WriteAfterTimeoutCrash();
  void OutputShowIndicatorOnTimeout(const std::string& message);
  void GetIndicatorOnTimeout(JSData &output, const std::string& message);
  void QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll = false);
  bool ShouldDisplayLoginPage();
  void SendAllAndWrapUp();
  void WrapUpConnection();
  void ResetMutexProcessesNoAllocation();
  void reset();
  void resetMessageComponentsExceptRawMessage();
  void resetConnection();
  void Release();
  void ReleaseKeepInUseFlag();
  void SendAllBytesWithHeaders();
  void SendAllBytesNoHeaders();
  void SendAllBytesHttp();
  void SendAllBytesHttpSSL();
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);

  std::string GetChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField);
  std::string GetClonePageResult();
  std::string GetModifyProblemReport();
  std::string GetSignUpRequestResult();
  std::string GetAuthenticationToken(const std::string& reasonForNoAuthentication = "");
  std::string GetBrowseProblems();
  std::string GetDatabaseJSON();
  std::string GetDatabaseDeleteOneItem();
  std::string GetAddUserEmails();
  std::string GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken);
  void SetHeaderOKNoContentLength(const std::string& extraHeader, const std::string& contentType = "text/html");
  void SetHeader(const std::string& httpResponseNoTermination, const std::string& remainingHeaderNoTermination);
  std::string GetHeaderConnectionClose();
  std::string GetHeaderConnectionKeepAlive();
  std::string GetHeaderSetCookie();
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveAll();
  void AttemptUnknownRequestErrorCorrection();
  bool ReceiveAllHttp();
  bool ReceiveAllHttpSSL();
  bool RequireSSL();
  enum requestTypes {requestUnknown, requestGet, requestPost, requestHead, requestChunked};
  std::string ToStringAddressRequest() const;
  std::string ToStringStatus() const;
  std::string ToStringMessageUnsafe(bool useHTML = true) const;
  std::string ToStringMessageShortUnsafe(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringMessageFullUnsafe(bool useHTML = true) const;
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
  PointerObjectDestroyer<Calculator> theCalculator;
  MonomialCollection<MonomialWrapper<std::string, MathRoutines::HashString>, LargeInteger> currentlyConnectedAddresses;
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

  List<unsigned char> idLastWorker;

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
  static void CheckFreecalcSetup();
  static void AnalyzeMainArguments(int argC, char** argv);
  static void AnalyzeMainArgumentsTimeString(const std::string& timeLimitString);
  void InitializeGlobalVariables();
  void InitializeGlobalVariablesHashes();
  void MarkChildNotInUse(int childIndex);
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
  // Wraps the system level fork() call.
  // Addionally computes a
  // unique [with probability ~1] process id and
  // unique [with probability ~1] non-openSSL random bytes
  // for the child and parent processes.
  int Fork();
  void initializeRandomBytes();
  void WriteVersionJSFile();
  WebWorker& GetActiveWorker();
  static void WorkerTimerPing(int64_t pingTime);
  static void Release(int& theDescriptor);
  static void SignalActiveWorkerDoneReleaseEverything();
  static void FlushActiveWorker();
  static void OutputShowIndicatorOnTimeoutStatic(const std::string &message);
  static void SendStringThroughActiveWorker(const std::string& input);
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
  void StopKillAll[[noreturn]](bool attemptToRestart);
  bool RestartIsNeeded();
  void initDates();
  std::string ToStringLastErrorDescription();
  std::string ToStringStatusActive();
  std::string ToStringStatusAll();
  std::string ToStringStatusForLogFile();
  std::string ToStringConnectionSummary();
  static void TurnProcessMonitoringOn();
  static void TurnProcessMonitoringOff();
  static std::string GetEnvironment(const std::string& envVarName);
  static int main(int argc, char** argv);
  static int mainApache();
  static int mainCommandLine();
};

extern WebServer theWebServer;

class ProgressReportWebServer {
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

