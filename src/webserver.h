#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "../../calculator/src/vpfHeader6WebServerInterprocessLogistics.h"
#include "../../calculator/src/vpfHeader4SystemFunctionsGlobalObjects.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

/////////////////////////////////////////////////////////
#ifdef MACRO_use_open_ssl
//installation of these headers in ubuntu:
//sudo apt-get install libssl-dev
//on opensuse:
//sudo yast -i libopenssl-devel
//Instructions: look at the examples folder in the openssl.
//openssl tutorial (couldn't make it work myself):
//http://www.ibm.com/developerworks/library/l-openssl/
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct SSLdata
{
public:
  static bool flagSSLlibraryInitialized;
  int errorCode;
  SSL* sslClient;
  SSL* sslServeR;
  X509* my_certificate;
  X509* peer_certificate;
  SSL_CTX* contextServer;
  //SSL_CTX* contextClient;
  const SSL_METHOD* theSSLMethod;
  List<int> socketStackServer;
  List<int> socketStackClient;
  List<char> buffer;
  std::string otherCertificateIssuerName, otherCertificateSubjectName;
  bool flagSSLHandshakeSuccessful;
  void DoSetSocket(int theSocket, SSL *theSSL);
  void SetSocketAddToStackServer(int theSocket);
  void RemoveLastSocketServer();
  void SetSocketAddToStackClient(int theSocket);
  void RemoveLastSocketClient();
  void FreeClientSSL();
  void ClearErrorQueue
  (int errorCode, SSL* theSSL, std::stringstream* commentsOnError,
   std::stringstream* commentsGeneral, bool includeNoErrorInComments);
  static bool initSSLkeyFiles();
  void initSSLlibrary();
  void initSSLserver();
  void initSSLclient();
  int SSLread
  (SSL* theSSL, void *buffer, int bufferSize,
   std::stringstream *commentsOnFailure,
   std::stringstream *commentsGeneral, bool includeNoErrorInComments);
  bool SSLreadLoop
  (int numTries, SSL* theSSL, std::string& output, const LargeInt& expectedLength,
   std::stringstream *commentsOnFailure,
   std::stringstream *commentsGeneral, bool includeNoErrorInComments);
  bool SSLwriteLoop
  (int numTries, SSL* theSSL, const std::string& input,
   std::stringstream *commentsOnFailure,
   std::stringstream *commentsGeneral, bool includeNoErrorInComments);
  int SSLwrite
  (SSL* theSSL, void *buffer, int bufferSize,
   std::stringstream *commentsOnFailure,
   std::stringstream *commentsGeneral, bool includeNoErrorInComments);
  SSLdata()
  { this->errorCode = - 1;
    this->sslClient = 0;
    this->sslServeR = 0;
    this->my_certificate = 0;
    this->peer_certificate = 0;
    this->theSSLMethod = 0;
    this->contextServer = 0;
//    this->contextClient=0;
    this->flagSSLHandshakeSuccessful = false;
  }
  ~SSLdata();
  void FreeSSL();
  void FreeContext();
  void HandShakeIamServer(int inputSocketID);
  bool InspectCertificates
  (std::stringstream* commentsOnFailure,
   std::stringstream* commentsGeneral);
  bool HandShakeIamClientNoSocketCleanup
  (int inputSocketID, std::stringstream* commentsOnFailure,
   std::stringstream* commentsGeneral);
  void FreeEverythingShutdownSSL();
};
#endif
////////////////////////////////////////////


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
  MapLisT<std::string, std::string, MathRoutines::hashString> requestHeaders;
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
  PipePrimitive pipeWorkerToWorkerUserInput;
  PipePrimitive pipeWorkerToWorkerStatus;
  PipePrimitive pipeWorkerToServerControls;
  PipePrimitive pipeWorkerToServerTimerPing;

  std::string error;
  std::string errorLogin;
  void PrepareFullMessageHeaderAndFooter();
  std::string openIndentTag(const std::string& theTag);
  std::string closeIndentTag(const std::string& theTag);
  int ProcessSetProblemDatabaseInfo();
  int ProcessSlidesOrHomeworkFromSource();
  int ProcessSlidesSource();
  int ProcessClonePage();
  int ProcessModifyPage();
  int ProcessAddUserEmails();
  int ProcessServerStatus();
  int ProcessServerStatusJSON();
  int ProcessToggleMonitoring();
  int ProcessServerStatusPublic();
  int ProcessComputationIndicator();
  int ProcessBrowseProblems();
  int ProcessEditPage();
  int ProcessFolderOrFile();
  int ProcessFolder();
  int ProcessFile();
  int ProcessChangePassword();
  int SetEmail(const std::string& input);
  bool DoSetEmail
  (UserCalculatorData& inputOutputUser,
   std::stringstream* commentsOnFailure, std::stringstream* commentsGeneralNonSensitive,
   std::stringstream* commentsGeneralSensitive);
  int ProcessChangePasswordPage();
  int ProcessCalculator();
  int ProcessDatabase();
  int ProcessDatabaseJSON();
  int ProcessDatabaseModifyEntry();
  int ProcessDatabaseOneEntry();
  int ProcessAccounts();
  int ProcessScores();
  int ProcessAbout();
  int ProcessApp(bool appendBuildHash);
  int ProcessTopicListJSON();
  int ProcessScoresInCoursePage();
  int ProcessAssignTeacherToSection();
  int ProcessExamPage();
  int ProcessExamPageJSON();
  int ProcessTemplate();
  int ProcessTemplateJSON();
  int ProcessUserInfoJSON();
  int ProcessSelectCourse();
  int ProcessSelectCourseJSON();
  int ProcessExamPageInterpreter();
  int ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication);
  int ProcessLoginNeededOverUnsecureConnection();
  bool ProcessRedirectAwayFromWWW();
  int ProcessLoginPage(const std::string& reasonForLogin = "");
  int ProcessLogout();
  int ProcessSignUP();
  int ProcessSignUpPage();
  int ProcessForgotLogin();
  int ProcessForgotLoginPage();
  int ProcessNavigation();
  int ProcessCompute();
  int ProcessCalculatorExamples();
  int ProcessCalculatorExamplesJSON();
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
  bool CorrectRequestsBEFORELoginReturnFalseIfModified();
  bool CorrectRequestsAFTERLoginReturnFalseIfModified();
  bool RedirectIfNeeded(std::stringstream& argumentProcessingFailureComments);

  bool ExtractArgumentsFromMessage
  (const std::string& input, std::stringstream& argumentProcessingFailureComments,
   int recursionDepth = 0);
  bool ExtractArgumentsFromCookies
  (std::stringstream& argumentProcessingFailureComments);
  static void OutputSendAfterTimeout(const std::string& input);
  void OutputResultAfterTimeout();
  static void OutputCrashAfterTimeout();
  void OutputShowIndicatorOnTimeout();
  void QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll = false);
  void QueueBytesForSendingNoHeadeR
  (const List<char>& bytesToSend, bool MustSendAll = false);
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
  std::string GetJavaScriptIndicatorBuiltInServer();
  std::string GetChangePasswordPage();
  std::string GetChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField);
  std::string GetClonePageResult();
  std::string GetModifyProblemReport();
  std::string GetLoginPage(const std::string& reasonForLogin = "");
  std::string GetForgotLoginPage();
  std::string GetSignUpPage();
  std::string GetLoginHTMLinternal(const std::string& reasonForLogin = "");
  std::string GetAuthenticationToken(const std::string& reasonForNoAuthentication = "");
  std::string GetBrowseProblems();
  std::string GetEditPageHTML();
  std::string GetExamPageInterpreter();
  std::string GetDatabaseJSON();
  std::string GetDatabasePage();
  //std::string GetEmailChangePage();
  std::string GetNavigationPage();
  std::string GetAddUserEmails();
  static std::string GetJavaScriptIndicatorFromHD();
  static std::string GetJavascriptSubmitLoginInfo();
  std::string GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken);
  void SetHeaderOKNoContentLength();
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
  void ReportDisplayUserInput();
  enum requestTypes {requestUnknown, requestGet, requestPost, requestHead, requestChunked};
  std::string ToStringStatus()const;
  std::string ToStringMessageUnsafe(bool useHTML = true)const;
  std::string ToStringMessageShortUnsafe(FormatExpressions* theFormat = 0)const;
  std::string ToStringMessageFullUnsafe(bool useHTML = true)const;
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
  bool flagReapingChildren;
#ifdef MACRO_use_open_ssl
  SSLdata theSSLdata;
#endif
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
  HashedList<std::string, MathRoutines::hashString> addressStartsInterpretedAsCalculatorRequest;

  int activeWorker;
  int timeLastExecutableModification;
  ListReferences<std::string> theProgressReports;
  bool flagDeallocated;
  bool flagPort8155;
  WebServer();
  ~WebServer();
  static void FigureOutOperatingSystem();
  static void CheckSystemInstallationMongoDB();
  static void CheckSystemInstallationOpenSSL();
  static void CheckMongoDBSetup();
  static void CheckMathJaxSetup();
  static void CheckFreecalcSetup();
  static void CheckSVNSetup();
  static void AnalyzeMainArguments(int argC, char **argv);
  void InitializeGlobalVariables();
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
  //static void Signal_SIGINT_handler(int s); //<- signals appear to be racy. No signals please!
  //static void Signal_SIGCHLD_handler(int s);//<- signals appear to be racy. No signals please!
  static void fperror_sigaction(int signal);
  //<- this signal should never happen in
  //<- server, so even if racy, we take the risk of a hang.
  //<- racy-ness in child process does not bother us: hanged children are still fine.
  //void ReapChildren();
  bool initPrepareWebServerALL();
  void initPrepareSignals();
  bool initBindToPorts();
  void initPortsITry();
  void initListeningSockets();
  void initSSL();
  void SSLServerSideHandShake();
  void TerminateChildSystemCall(int i);
  void RecycleChildrenIfPossible();
  void HandleTooManyConnections(const std::string& incomingUserAddress);
  void Restart();
  void CheckExecutableVersionAndRestartIfNeeded(bool callReload);
  void initDates();
  std::string ToStringLastErrorDescription();
  std::string ToStringStatusActive();
  std::string ToStringStatusAll();
  std::string ToStringStatusPublic();
  std::string ToStringStatusPublicNoTop();
  std::string ToStringStatusForLogFile();
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

