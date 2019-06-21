#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "vpfHeader6WebServerInterprocessLogistics.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "transport_security_layer.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

class WebServer;

class WebWorker {
public:
  WebServer* parent;
  int indexInParent;
  int ProcessPID;
  int numberOfReceivesCurrentConnection;
  int64_t millisecondOffset;
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
  double timeServerAtWorkerStart;
  double timeOfLastPingServerSideOnly;
  bool flagInUsE;
  bool flagExited;
  bool flagDeallocated;
  bool flagKeepAlive;
  bool flagFileNameSanitized;
  bool flagFoundMalformedFormInput;
  bool flagProgressReportAllowed;
  bool flagArgumentsAreOK;
  bool flagDoAddContentLength;
  bool flagUsingSSLInWorkerProcess;
  bool flagDidSendAll;
  bool flagToggleMonitoring;
  bool flagAllBytesSentUsingFile;
  bool flagEncounteredErrorWhileServingFile;
  List<std::string> theMessageHeaderStrings;
  MapLisT<std::string, std::string, MathRoutines::HashString> requestHeaders;
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
  Pipe pipeWorkerToWorkerRequestIndicator;
  Pipe pipeWorkerToWorkerIndicatorData;
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
  int ProcessToggleMonitoring();
  int ProcessServerStatusJSON();
  int ProcessComputationIndicator();
  int ProcessBrowseProblems();
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
  int ProcessAbout();
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
  int ProcessNavigation();
  int ProcessCompute();
  int ProcessCalculatorExamplesJSON();
  int ProcessSubmitAnswers();
  int ProcessSubmitAnswersPreview();
  int ProcessProblemGiveUp();
  int ProcessProblemSolution();
  int ProcessPauseWorker();
  int ProcessMonitor();
  int ProcessUnknown();
  int Run();
  bool CheckConsistency();
  void PipeProgressReport(const std::string& input);
  void WriteProgressReportToFile(const std::string& input);

  static void StandardOutputAfterTimeOut(const std::string& input);

  bool IsFileServedRaw();

  static bool IsAllowedAsRequestCookie(const std::string& input);
  bool Login(std::stringstream& argumentProcessingFailureComments, std::stringstream *comments);
  bool CorrectRequestsBEFORELoginReturnFalseIfModified();
  bool CorrectRequestsAFTERLoginReturnFalseIfModified();
  bool RedirectIfNeeded(std::stringstream& argumentProcessingFailureComments);

  bool ExtractArgumentsFromMessage(
    const std::string& input, std::stringstream& argumentProcessingFailureComments, int recursionDepth = 0
  );
  bool ExtractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments);
  static void OutputSendAfterTimeout(const std::string& input);
  void OutputResultAfterTimeout();
  static void OutputCrashAfterTimeout();
  void OutputShowIndicatorOnTimeout();
  void QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll = false);
  bool ShouldDisplayLoginPage();
  void SendAllAndWrapUp();
  void WrapUpConnection();
  void ResetPipesNoAllocation();
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
  std::string GetNavigationPage();
  std::string GetAddUserEmails();
  std::string GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken);
  void SetHeaderOKNoContentLength(const std::string& extraHeader, const std::string& contentType = "text/html");
  void SetHeadeR(const std::string& httpResponseNoTermination, const std::string& remainingHeaderNoTermination);
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
  std::string ToStringMessageShortUnsafe(FormatExpressions* theFormat = 0) const;
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
  int listeningSocketHttpSSL;
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
  long long NumFailedSelectsSoFar;
  long long NumSuccessfulSelectsSoFar;
  bool flagReapingChildren;
  TransportSecurityLayer theTSL;
  PointerObjectDestroyer<Calculator> theCalculator;
  MonomialCollection<MonomialWrapper<std::string, MathRoutines::HashString>, LargeInt> currentlyConnectedAddresses;
  List<std::string> PortsITryHttp;
  List<std::string> PortsITryHttpSSL;
  std::string httpPort;
  std::string httpSSLPort;
  List<int> theListeningSockets;
  //List<int> theListeningSocketsReadyToAccept;
  ListReferences<WebWorker> theWorkers;

  HashedList<std::string, MathRoutines::HashString> requestsNotNeedingLogin;
  List<std::string> addressStartsNotNeedingLogin;
  List<std::string> addressStartsSentWithCacheMaxAge;
  HashedList<std::string, MathRoutines::HashString> addressStartsInterpretedAsCalculatorRequest;

  int activeWorker;
  int timeLastExecutableModification;
  ListReferences<std::string> theProgressReports;
  bool flagDeallocated;
  WebServer();
  ~WebServer();
  static void FigureOutOperatingSystem();
  static void CheckSystemInstallationMongoDB();
  static void CheckSystemInstallationOpenSSL();
  static void CheckMongoDBSetup();
  static void CheckMathJaxSetup();
  static void CheckFreecalcSetup();
  static void AnalyzeMainArguments(int argC, char** argv);
  static void AnalyzeMainArgumentsTimeString(const std::string& timeLimitString);
  void InitializeGlobalVariables();
  void InitializeGlobalVariablesHashes();
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
  void WriteVersionJSFile();
  WebWorker& GetActiveWorker();
  static void WorkerTimerPing(int64_t pingTime);
  static void Release(int& theDescriptor);
  static void SignalActiveWorkerDoneReleaseEverything();
  static void FlushActiveWorker();
  static void ReturnActiveIndicatorAlthoughComputationIsNotDone();
  static void SendStringThroughActiveWorker(const std::string& input);
  static void PipeProgressReport(const std::string& input);
  static void fperror_sigaction(int signal);
  void ReapOneChild();
  void ReapChildren();
  static void Signal_SIGCHLD_handler(int s);
  bool initPrepareWebServerALL();
  void initializeSignals();
  bool initBindToPorts();
  void initPortsITry();
  void initListeningSockets();
  void initSSL();
  bool SSLServerSideHandShake();
  static void TerminateProcessId(int processId);
  void TerminateChildSystemCall(int i);
  void ProcessOneChildMessage(int childIndex, int& outputNumInUse);
  void RecycleChildrenIfPossible();
  void RecycleOneChild(int childIndex, int& numberInUse);
  void HandleTooManyConnections(const std::string& incomingUserAddress);
  void HandleTooManyWorkers(int &numInUse);
  void RestarT();
  bool RestartIsNeeded();
  void initDates();
  std::string ToStringLastErrorDescription();
  std::string ToStringStatusActive();
  std::string ToStringStatusAll();
  std::string ToStringStatusForLogFile();
  std::string ToStringConnectionSummary();
  static void TurnProcessMonitoringOn();
  static void TurnProcessMonitoringOff();
  static void ToggleProcessMonitoring();
  static std::string GetEnvironment(const std::string& envVarName);
  static int main(int argc, char** argv);
  static int mainApache();
  static int mainCommandLine();
};

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

