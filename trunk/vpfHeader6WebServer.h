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

class ClientMessage
{
public:
  std::string theMessage;
  std::string mainArgument;
  std::string mainAddresSRAW;
  std::string mainAddress;
  std::string PhysicalFileName;
  List<std::string> theStrings;
  int ContentLength;

  int requestType;
  ClientMessage():requestType(ClientMessage::requestTypeUnknown){}
  enum{requestTypeUnknown, requestTypeGetCalculator, requestTypePostCalculator, requestTypeGetNotCalculator,
  requestTypeGetIndicator};
  std::string ToString()const;
  std::string ToStringShort(FormatExpressions* theFormat=0)const;
  std::string ToStringFull()const;
  void ParseMessage();
  void ExtractArgumentFromAddress();
  void ExtractPhysicalAddressFromMainAddress();
  void resetEverythingExceptMessageString();
};

class WebServer;
class Socket
{
public:
  WebServer* parent;
  int connectedSocketID;
  int connectionID;
  ClientMessage lastMessageReceived;
  List<char> remainingBytesToSend;
  List<char> bufferFileIO;
  std::string error;
  int ProcessGetRequestIndicator();
  int ProcessGetRequestNonCalculator();
  int ProcessGetRequestFolder();
  int ProcessRequestTypeUnknown();
  void QueueStringForSending(const std::string& stringToSend, bool MustSendAll=false);
  void QueueBytesForSending
  (const List<char>& bytesToSend, bool MustSendAll=false)
  ;
  void SendAllBytes();
  std::string GetMIMEtypeFromFileExtension(const std::string& fileExtension);
  bool IsFileExtensionOfBinaryFile(const std::string& fileExtension);
  Socket();
  ~Socket()
  { this->SendAllBytes();
    close(this->connectedSocketID);
    this->connectedSocketID=-1;
    this->connectionID=-1;
  }
  bool ReceiveOnce();
  bool ReceiveAll();
};

class WebServer
{
public:
  int listeningSocketID;
  Socket theSocket;
  bool flagUsingBuiltInServer;
  int myPipeIndex;
  int childPID;
  List<List<int> > pipeChildToParent;
  List<List<int> > pipeParentToChild;
  List<bool> pipeInUse;
  WebServer();
  ~WebServer();
  List<char> lastPipeReadResult;
  List<char> pipeBuffer;
  void ReadFromPipe(List<int>& inputPipe, bool doNotBlock);
  void SendThroughPipe(const std::string& toBeSent, List<int>& outputPipe, bool doNotBlock);
  void getNewPipe();
  void ReleaseMyPipe();
  int Run();
  int StandardOutput();
  int ServeClient();
  void StandardOutputPart1BeforeComputation();
  void StandardOutputPart2AfterComputation();
  void StandardOutputReturnIndicatorWaitForComputation();
  std::string GetLastErrorDescription();
  std::string GetStatus();
};
#endif
