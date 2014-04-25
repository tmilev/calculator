//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderWebServerAlreadyDefined
#define vpfHeaderWebServerAlreadyDefined
#include <unistd.h>
#include <string.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <sys/wait.h>
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
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
  std::string theMessage;
  std::string mainArgument;
  std::string mainAddresSRAW;
  std::string mainAddress;
  std::string PhysicalFileName;
  List<std::string> theStrings;
  int ContentLength;
  int requestType;
  int connectedSocketID;
  int connectionID;
  List<char> remainingBytesToSend;
  List<char> bufferFileIO;
  List<int> pipeServerToWorker;
  List<int> pipeWorkerToServer;

  std::string error;
  void DisplayIndicator(const std::string& input);

  int ProcessGetRequestIndicator();
  int ProcessGetRequestNonCalculator();
  int ProcessGetRequestFolder();
  int ProcessRequestTypeUnknown();
  int ServeClient();
  void QueueStringForSending(const std::string& stringToSend, bool MustSendAll=false);
  static int StandardOutput();
  void StandardOutputReturnIndicatorWaitForComputation();
  bool CheckConsistency();
  static void StandardOutputPart1BeforeComputation();
  static void StandardOutputPart2AfterComputation();

  void QueueBytesForSending
  (const List<char>& bytesToSend, bool MustSendAll=false)
  ;
  void ReleaseResources();
  void SendAllBytes();
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  WebWorker();
  ~WebWorker();
  bool IamActive();
  bool ReceiveOnce();
  bool ReceiveAll();
  enum{requestTypeUnknown, requestTypeGetCalculator, requestTypePostCalculator, requestTypeGetNotCalculator,
  requestTypeGetIndicator};
  std::string ToStringStatus()const;
  std::string ToStringMessage()const;
  std::string ToStringMessageShort(FormatExpressions* theFormat=0)const;
  std::string ToStringMessageFull()const;
  void ParseMessage();
  void ExtractArgumentFromAddress();
  void ExtractPhysicalAddressFromMainAddress();
  void reset();
  void resetEverythingExceptMessageString();
};

class WebServer
{
public:
  int listeningSocketID;
  bool flagUsingBuiltInServer;
  List<WebWorker> theWorkers;
  int activeWorker;
  List<char> lastPipeReadResult;
  List<char> pipeBuffer;
  WebServer();
  ~WebServer();
  void ReadFromPipe(List<int>& inputPipe, bool doNotBlock);
  void SendThroughPipe(const std::string& toBeSent, List<int>& outputPipe, bool doNotBlock);
  void ReleaseActiveWorker();
  void CreateNewActiveWorker();
  int Run();
  WebWorker& GetActiveWorker();
  static void Release(int& theDescriptor);
  static void FlushActiveWorker();
  static void DisplayActiveIndicator(const std::string& input);
  static void ReturnActiveIndicatorAlthoughComputationIsNotDone();
  static void SendStringThroughActiveWorker(const std::string& input);

  std::string ToStringLastErrorDescription();
  std::string ToStringStatus();
};
#endif
