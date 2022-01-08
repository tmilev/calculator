#ifndef header_webserver_ALREADY_INCLUDED
#define header_webserver_ALREADY_INCLUDED
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
  class Statistics {
  public:
    int allReceives;
    int pingReceives;
  };
  Statistics statistics;
  int lastResult;
  int64_t millisecondsServerAtWorkerStart;
  int64_t millisecondsLastPingServerSideOnly;
  int64_t millisecondsAfterSelect;

  std::string displayUserInput;

  std::string messageHead;
  std::string messageBody;

  JSData resultWork;

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
  bool flagInUse;
  bool flagExited;
  bool flagDeallocated;
  bool flagKeepAlive;
  bool flagFileNameSanitized;
  bool flagFoundMalformedFormInput;
  bool flagArgumentsAreOK;
  bool flagDoAddContentLength;
  bool flagUsingSSLInWorkerProcess;
  bool flagDidSendAll;
  List<std::string> messageHeaderStrings;
  MapList<std::string, std::string, MathRoutines::hashString> requestHeaders;
  int contentLength;
  int requestType;
  int connectedSocketID;
  int connectedSocketIDLastValueBeforeRelease;
  int connectionID;
  int indentationLevelHTML;
  int numberOfConsecutiveNoReportsBeforeDisconnect;
  List<char> remainingBytesToSend;
  List<char> remainingHeaderToSend;
  List<char> remainingBodyToSend;
  List<char> bufferFileIO;
  MutexProcess pauseWorker;
  MutexProcess writingReportFile;
  PipePrimitive workerToWorkerRequestIndicator;
  PipePrimitive pipeWorkerToWorkerStatus;
  PipePrimitive pipeWorkerToServerControls;
  PipePrimitive pipeWorkerToServerTimerPing;

  WebAPIResponse response;
  std::string error;
  void prepareFullMessageHeaderAndFooter();
  std::string openIndentTag(const std::string& tag);
  std::string closeIndentTag(const std::string& tag);
  JSData processComputationIndicatorJSData();
  int getIndexIfRunningWorkerId(JSData& outputComputationStatus);
  int processFolderOrFile();
  int processFolder();
  int processFile();
  int processFileDoesntExist();
  int processFileCantOpen();
  int processFileTooLarge(long fileSize);

  JSData setEmail(const std::string& input);
  bool doSetEmail(
    UserCalculatorData& inputOutputUser,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive,
    std::stringstream* commentsGeneralSensitive
  );
  int processgetAuthenticationToken(const std::string& reasonForNoAuthentication);
  int processLoginNeededOverUnsecureConnection();
  bool processRedirectAwayFromWWW();
  int processUnknown();
  bool runInitialize();
  int run();
  bool runOnce();
  bool failReceiveReturnFalse();
  bool checkConsistency();

  bool loginProcedure(
    std::stringstream& argumentProcessingFailureComments,
    std::stringstream* comments
  );
  bool correctRequestsBEFORELoginReturnFalseIfModified();
  bool correctRequestsAFTERLoginReturnFalseIfModified();
  bool redirectIfNeeded(std::stringstream& argumentProcessingFailureComments);

  bool extractArgumentsFromMessage(
    const std::string& input,
    std::stringstream& argumentProcessingFailureComments,
    int recursionDepth = 0
  );
  bool extractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments);
  void writeAfterTimeoutShowIndicator(const std::string& message);
  void writeAfterTimeoutProgress(const std::string& input, bool forceFileWrite);
  ///////
  void pauseIfRequested();
  // writes json to body, sanitizes.
  bool writeToBodyJSON(const JSData& result);
  bool writeToBody(const std::string& bytesToAppend);
  void writeAfterTimeoutString(
    const std::string& input,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  void writeAfterTimeoutJSON(
    const JSData& input,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  void writeAfterTimeoutPartTwo(
    JSData& result,
    const std::string& status,
    const std::string& fileNameCarbonCopy
  );
  void writeAfterTimeoutCarbonCopy(
    const JSData& input,
    const std::string& fileNameCarbonCopy
  );
  void getIndicatorOnTimeout(JSData &output, const std::string& message);
  void queueStringForSendingNoHeader(const std::string& stringToSend, bool mustSendAll = false);
  void queueBytesForSendingNoHeader(const List<char>& bytesToSend, bool mustSendAll = false);
  bool shouldDisplayLoginPage();
  void wrapUpConnection();
  void resetMutexProcesses();
  void reset();
  void resetMessageComponentsExceptRawMessage();
  void resetConnection();
  void release();
  void releaseKeepInUseFlag();
  bool ensureAllBytesSent();
  void sendPending();
  void sendAllBytesNoHeaders();
  std::string mimeTypeFromFileExtension(const std::string& fileExtension);
  std::string headerFromFileExtension(const std::string& fileExtension);

  std::string getChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField);
  JSData getClonePageResult();
  JSData getSignUpRequestResult();
  std::string getAuthenticationToken(const std::string& reasonForNoAuthentication = "");
  JSData getDatabaseJSON();
  std::string getDatabaseDeleteOneItem();
  std::string getAddUserEmails();
  void setHeaderOKNoContentLength(
    const std::string& extraHeader,
    const std::string& contentType = "application/json"
  );
  void setHeader(
    const std::string& httpResponseNoTermination,
    const std::string& remainingHeaderNoTermination
  );

  std::string getHeaderConnectionClose();
  std::string getHeaderConnectionKeepAlive();
  std::string getHeaderSetCookie();
  bool isFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool isActive();
  bool receiveAll();
  void attemptUnknownRequestErrorCorrection();
  bool requireSSL();
  enum requestTypes {requestUnknown, requestGet, requestPost, requestHead, requestChunked};
  std::string toStringAddressRequest() const;
  std::string toStringStatus() const;
  std::string toStringMessageShort() const;
  std::string toStringMessageFull() const;
  void parseMessageHead();
  void extractHostInfo();
  void extractAddressParts();
  void sanitizeVirtualFileName();
  int serveClient();
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
  int maximumNumberOfWorkersPerIPAdress;
  int maximumTotalUsedWorkers;
  class Statististics {
  public:
    static const std::string allRequestsString;
    static const std::string pingRequestsString;
    int allConnections;
    int pingConnections;
    int allRequests;
    int pingRequests;
    // Number of processes reaped with the wait loop.
    int processesReaped;
    // Number of processes killed: hopefully zero or close to zero.
    int processKilled;
    // Number of processes that piped their exit status correctly.
    // Hopefully equals the number of processes reaped.
    int workersNormallyExited;
    // Statistics for the select() calls for the accept function of the server.
    long long failedSelectsSoFar;
    long long successfulSelectsSoFar;
  };
  Statististics statistics;
  int webServerPingIntervalInSeconds;
  int previousServerStatReport;
  int previousServerStatDetailedReport;
  int processIdServer;
  bool flagReapingChildren;
  bool flagDeallocated;
  std::string pingAuthentication;
  TransportLayerSecurity transportLayerSecurity;
  LinearCombination<MonomialWrapper<std::string, MathRoutines::hashString>, LargeInteger> currentlyConnectedAddresses;
  std::string portHTTP;
  std::string portHTTPSOpenSSL;
  std::string portHTTPSBuiltIn;
  std::string portHTTPSDefault;
  List<int> listeningSockets;
  ListReferences<WebWorker> allWorkers;
  MapList<std::string, int, MathRoutines::hashString> workerIds;

  HashedList<std::string, MathRoutines::hashString> requestsNotNeedingLogin;
  List<std::string> addressStartsNotNeedingLogin;
  List<std::string> addressStartsSentWithCacheMaxAge;
  HashedList<std::string, MathRoutines::hashString> addressStartsInterpretedAsCalculatorRequest;

  MapList<std::string, std::string, MathRoutines::hashString> MIMETypes;
  int activeWorker;
  int64_t timeLastExecutableModification;
  ListReferences<std::string> progressReports;
  WebServer();
  ~WebServer();
  static void figureOutOperatingSystem();
  void computeSSLFlags();
  static void checkInstallation();
  static void checkSystemInstallationMongoDatabase();
  static void checkSystemInstallationOpenSSL();
  static void checkMongoDatabaseSetup();
  static void checkFreecalcSetup();
  static void analyzeMainArguments(int argC, char** argv);
  static bool analyzeMainArgumentsTimeString(const std::string& timeLimitString);
  void initializeBuildFlags();
  void initializeMainAll();
  void initializeMainHashes();
  void initializeMainRequests();
  void initializeMainAddresses();
  void initializeMainFoldersInstructorSpecific();
  void initializeMainMIMETypes();

  void markChildNotInUse(int childIndex);
  bool requiresLogin(const std::string& inputRequest, const std::string& inputAddress);
  void releaseWorkerSideResources();
  void releaseActiveWorker();
  void releaseSocketsNonActiveWorkers();
  void releaseNonActiveWorkers();
  void releaseEverything();
  bool createNewActiveWorker();
  bool emergencyRemoval_LastCreatedWorker();
  bool checkConsistency();
  int daemon();
  int run();
  // Wraps the system level fork() call.
  // Addionally computes a
  // unique [with probability ~1] process id and
  // unique [with probability ~1] non-openSSL random bytes
  // for the child and parent processes.
  bool createProcessMutex();
  void computeActiveWorkerId();
  int forkProcess();
  int forkRaw();
  void initializeRandomBytes();
  void writeVersionJSFile();
  WebWorker& getActiveWorker();
  static void workerTimerPing(int64_t pingTime);
  static void release(int& descriptor);
  static void wrapUp();
  static void fperror_sigaction[[noreturn]](int signal);
  void reapChildren();
  static void signal_SIGCHLD_handler(int s);
  bool initPrepareWebServerALL();
  void initializeSignals();
  bool initBindToPorts();
  bool initBindToOnePort(const std::string& port, int& outputListeningSocket);
  void initPortsITry();
  void initListeningSockets();
  void initSSL();
  bool sslServerSideHandShake(std::stringstream* commentsOnFailure);
  static void terminateProcessId(int processId);
  void terminateChildSystemCall(int i);
  void processOneChildMessage(int childIndex, int& outputNumInUse);
  void recycleChildrenIfPossible();
  void recycleOneChild(int childIndex, int& numberInUse);
  void handleTooManyConnections(const std::string& incomingUserAddress);
  void handleTooManyWorkers(int& numInUse);
  void stopKillAll();
  void stop();
  bool restartNeeded();
  std::string toStringWorkerToWorker();
  static std::string toStringLastErrorDescription();
  std::string toStringStatusActive();
  std::string toStringStatusAll();
  std::string toStringStatusForLogFile();
  std::string toStringConnectionSummary();
  static void turnProcessMonitoringOn();
  static void turnProcessMonitoringOff();
  static int main(int argc, char** argv);
  static int mainConsoleHelp();
  static int mainCommandLine();
};

#endif // header_webserver_ALREADY_INCLUDED

