#ifndef header_webserver_ALREADY_INCLUDED
#define header_webserver_ALREADY_INCLUDED

#include "multiprocessing.h"
#include "transport_layer_security.h"
#include "web_api.h"

class WebServer;
class Listener;

class WebWorker {
public:
  WebServer* parent;
  int indexInParent;
  int processPID;
  std::string workerId;
  class Statistics {
  public:
    int allReceives;
    int pingReceives;
    int standaloneServerReceives;
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
  MonomialWrapper<
    std::string, HashFunctions::hashFunction<std::string>
  > userAddress;
  std::string hostWithPort;
  std::string hostNoPort;
  std::string cookiesApache;
  List<std::string> cookies;
  List<std::string> connectionFlags;
  std::string addressGetOrPost;
  std::string lastHeadMessage;
  std::string addressComputed;
  // Query parameters coming from the url:
  // the strings that come after the ? in the address.
  std::string argumentFomURL;
  // Query parameters given in the message body.
  std::string argumentFomMessageBody;
  std::string virtualFileName;
  std::string relativePhysicalFileName;
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
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > requestHeaders;
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
  int processFolderOrFile(bool generateLinkToCalculatorOnMissingFile);
  int processFolder();
  int processFile(bool generateLinkToCalculatorOnMissingFile);
  int processFileDoesntExist(bool generateLinkToCalculatorOnMissingFile);
  int processFileCantOpen();
  int processFileTooLarge(long fileSize);
  int processgetAuthenticationToken(
    const std::string& reasonForNoAuthentication
  );
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
  void redirect(const std::string& address);
  bool extractArgumentsFromMessage(
    const std::string& input,
    std::stringstream& argumentProcessingFailureComments,
    int recursionDepth = 0
  );
  bool extractArgumentsFromCookies(
    std::stringstream& argumentProcessingFailureComments
  );
  bool isValidCookieWebArgument(
    const std::string& argumentName, const std::string& argumentValue
  );
  void writeAfterTimeoutShowIndicator(const std::string& message);
  void writeAfterTimeoutProgress(
    const std::string& input, bool forceFileWrite
  );
  // /////
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
    const JSData& input, const std::string& fileNameCarbonCopy
  );
  void getIndicatorOnTimeout(JSData& output, const std::string& message);
  void queueStringForSendingNoHeader(
    const std::string& stringToSend, bool mustSendAll = false
  );
  void queueBytesForSendingNoHeader(
    const List<char>& bytesToSend, bool mustSendAll = false
  );
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
  std::string getChangePasswordPagePartOne(
    bool& outputDoShowPasswordChangeField
  );
  JSData getClonePageResult();
  std::string getAuthenticationToken(
    const std::string& reasonForNoAuthentication = ""
  );
  JSData getDatabaseJSON();
  std::string getDatabaseDeleteOneItem();
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
  enum requestTypes {
    requestUnknown, requestGet, requestPost, requestHead, requestChunked
  };
  std::string toStringAddressRequest() const;
  std::string toStringStatus() const;
  std::string toStringMessageShort() const;
  std::string toStringMessageFull() const;
  void parseMessageHead();
  void extractHostInfo();
  void extractAddressParts();
  void sanitizeVirtualFileName();
  int serveClient();
  std::string toStringAddressAndRequest();
};

class WebServer {
public:
  // how many pending connections queue will hold
  int listeningSocketHTTP;
  int listeningSocketHTTPSOpenSSL;
  int listeningSocketHTTPSBuiltIn;
  int listeningSocketHTTPSDefault;
  int lastListeningSocket;
  int highestSocketNumber;
  int maximumWorkersPerIPAdress;
  int maximumTotalUsedWorkers;
  class Statististics {
  public:
    static const std::string allRequestsString;
    static const std::string pingRequestsString;
    static const std::string standaloneServerRequestsString;
    int allConnections;
    int pingConnections;
    int allRequests;
    int pingRequests;
    int standaloneServerRequests;
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
  bool flagDeallocated;
  std::string pingAuthentication;
  TransportLayerSecurity transportLayerSecurity;
  LinearCombination<
    MonomialWrapper<
      std::string, HashFunctions::hashFunction<std::string>
    >,
    LargeInteger
  > currentlyConnectedAddresses;
  std::string portHTTP;
  std::string portHTTPSOpenSSL;
  std::string portHTTPSBuiltIn;
  std::string portHTTPSDefault;
  // Additional connections.
  // A map from ports to sockets.
  MapList<std::string, int> additionalPorts;
  // Map from listening sockets to ports.
  MapList<int, std::string> allListeningSockets;
  ListReferences<WebWorker> allWorkers;
  MapList<std::string, int> workerIds;
  HashedList<std::string> requestsNotNeedingLogin;
  List<std::string> addressStartsNotNeedingLogin;
  List<std::string> addressStartsSentWithCacheMaxAge;
  HashedList<std::string> addressStartsInterpretedAsCalculatorRequest;
  MapList<std::string, std::string> MIMETypes;
  int activeWorker;
  int64_t timeLastExecutableModification;
  ListReferences<std::string> progressReports;
  WebServer();
  ~WebServer();
  static void figureOutOperatingSystem();
  void computeSSLFlags();
  static void checkInstallation();
  static void checkSystemInstallationOpenSSL();
  static void checkDatabaseSetup();
  static void checkFreecalcSetup();
  void initializeBuildFlags();
  void initializeMainAll();
  void initializeMainHashes();
  void initializeMainRequests();
  void initializeMainAddresses();
  void initializeMainFoldersInstructorSpecific();
  void initializeMainMIMETypes();
  void markChildNotInUse(int childIndex);
  bool requiresLogin(
    const std::string& inputRequest, const std::string& inputAddress
  );
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
  // Runs the main loop once.
  // When false is returned, the main loop should be exited.
  // False will be returned:
  // 1) in the worker process and
  // 2) if the server encounters a fatal error.
  bool runOnce(
    Listener& listener,
    long long& previousReportedNumberOfSelects,
    int& returnCode
  );
  // Wraps the system level fork() call.
  // Addionally computes a
  // unique [with probability ~1] process id and
  // unique [with probability ~1] non-openSSL random bytes
  // for the child and parent processes.
  bool createProcessMutex();
  void computeActiveWorkerId();
  int forkWorkerProcess();
  void initializeRandomBytes();
  void writeVersionJSFile();
  WebWorker& getActiveWorker();
  static void workerTimerPing(int64_t pingTime);
  static void release(int& descriptor);
  static void wrapUp();
  static void fperror_sigaction[[noreturn]](int signal);
  void reapChildren();
  static void signal_SIGCHLD_handler(int s);
  bool initializePrepareWebServerAll();
  void initializeSignals();
  bool initializeBindToPorts();
  void initializePortsITry();
  void initializeListeningSockets();
  void initializeSSL();
  bool sslServerSideHandShake(std::stringstream* commentsOnFailure);
  static void terminateProcessId(int processId);
  void terminateChildSystemCall(int i);
  void processOneChildMessage(int childIndex, int& outputTotalInUse);
  void recycleChildrenIfPossible();
  void recycleOneChild(int childIndex, int& numberInUse);
  void handleTooManyConnections(const std::string& incomingUserAddress);
  void handleTooManyWorkers(int& workerCount);
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
};

#endif // header_webserver_ALREADY_INCLUDED
