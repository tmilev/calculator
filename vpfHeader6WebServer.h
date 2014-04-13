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
  List<std::string> theStrings;


  int requestType;
  ClientMessage():requestType(ClientMessage::requestTypeNone){}
  enum{requestTypeNone, requestTypeGetCalculator, requestTypePostCalculator, requestTypeGetNotCalculator};
  std::string ToString()const;
  void ParseMessage();
  void ExtractArgumentFromAddress();
  void resetEverythingExceptMessageString();
};

class Socket
{
public:
  int socketID;
  ClientMessage lastMessageReceived;
  int ProcessGetRequestNonCalculator();
  Socket(): socketID(-1){}
  ~Socket()
  { close(this->socketID);
    this->socketID=-1;
  }
  bool Receive();
};
#endif
