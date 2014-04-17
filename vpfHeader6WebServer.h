//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderWebServerAlreadyDefined
#define vpfHeaderWebServerAlreadyDefined
#include <unistd.h>
#include <string.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <sys/wait.h>
#include "vpfHeader4SystemFunctionsGlobalObjects.h"

static ProjectInformationInstance projectInfoInstanceWebServerHeader(__FILE__, "Web server classes declarations.");

class ClientMessage
{
public:
  std::string theMessage;
  std::string mainArgument;
  std::string mainAddress;
  std::string PhysicalFileName;
  List<std::string> theStrings;


  int requestType;
  ClientMessage():requestType(ClientMessage::requestTypeUnknown){}
  enum{requestTypeUnknown, requestTypeGetCalculator, requestTypePostCalculator, requestTypeGetNotCalculator};
  std::string ToString()const;
  std::string ToStringShort(FormatExpressions* theFormat=0)const;
  void ParseMessage();
  void ExtractArgumentFromAddress();
  void ExtractPhysicalAddressFromMainAddress();
  void resetEverythingExceptMessageString();
};

class Socket
{
public:
  int socketID;
  ClientMessage lastMessageReceived;
  List<char> remainingBytesToSend;
  List<char> bufferFileIO;
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
  Socket(): socketID(-1){}
  ~Socket()
  { this->SendAllBytes();
    close(this->socketID);
    this->socketID=-1;
  }
  bool Receive();
};
#endif
