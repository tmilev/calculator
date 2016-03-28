//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader6WebServer.h"
#include "vpfHeader1General5TimeDate.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

ProjectInformationInstance ProjectInfoVpf6_5calculatorWebRoutines(__FILE__, "Calculator web routines. ");

class WebCrawler
{
public:
  int theSocket;
  std::string portOrService;
  bool flagInitialized;
  List<char> buffer;
  struct sockaddr_in serverAddress;
  struct hostent *serverOtherSide;
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  WebCrawler();
  ~WebCrawler();
  void init();
  void PingCalculatorStatus();
  void FreeAddressInfo();
  std::string addressToConnectTo;
  std::stringstream fatalErrors;
  std::string lastTransactionErrors;
  std::string lastTransaction;
};

void MonitorWebServer()
{ WebCrawler theCrawler;
  theCrawler.init();
  int microsecondsToSleep=10000000;
  if (!theWebServer.flagPort8155)
  { theCrawler.portOrService="8080";
    theCrawler.addressToConnectTo="localhost";
  }
  theLog << logger::blue << "Pinging " << theCrawler.addressToConnectTo << " at port/service "
  << theCrawler.portOrService << " every " << (microsecondsToSleep/1000000) << " second(s). "
  << logger::endL;
  theLog << logger::red << "Please beware that the server will restart and you will lose all computations "
  << "if 3 consecutive pings fail. " << logger::endL;
  int numConsecutiveFailedPings=0;
  int numPings=0;
  TimeWrapper now;
  for (;;)
  { theGlobalVariables.FallAsleep(microsecondsToSleep);
    std::cout << "Pinging " << theCrawler.addressToConnectTo
      << " at port/service " << theCrawler.portOrService << ".\n";
    theCrawler.PingCalculatorStatus();
    numPings++;
    if (theCrawler.lastTransactionErrors!="")
    { now.AssignLocalTime();
      numConsecutiveFailedPings++;
      logIO << logger::red << "Ping of " << theCrawler.addressToConnectTo
      << " at port/service " << theCrawler.portOrService
      << " failed on " << now.ToStringHumanReadable() << ". "
      <<  numConsecutiveFailedPings << " consecutive fails so far, restarting on 3." << logger::endL;
    } else
    { std::cout << "Ping success #" << numPings << std::endl;
      numConsecutiveFailedPings=0;
    }
    if (numConsecutiveFailedPings>2)
    { logProcessKills << logger::red << "Server stopped responding (probably locked pipe?)"
      << ", restarting. " << logger::endL;
      theWebServer.Restart();
    }
  }
}

WebCrawler::WebCrawler()
{ this->flagInitialized=false;
  this->theSocket=-1;
  this->serverOtherSide=0;
  this->serverInfo=0;
}

void WebCrawler::FreeAddressInfo()
{ if (this->serverInfo!=0)
    freeaddrinfo(this->serverInfo);
  this->serverInfo=0;
}

void WebCrawler::init()
{ if (this->flagInitialized)
    return;
  MacroRegisterFunctionWithName("WebCrawler::init");
  buffer.initFillInObject(50000, 0);
  this->portOrService="80";
//  this->socketCommunication = socket(AF_INET, SOCK_STREAM, 0);
//  if (this->socketCommunication < 0)
//  { this->fatalErrors << "Couldn't create socket for initial connection. ";
//    return;
//  }
  this->addressToConnectTo="calculator-algebra.org";
}

WebCrawler::~WebCrawler()
{ if (!this->flagInitialized)
    return;
  this->flagInitialized=false;
  close(this->theSocket);
  this->theSocket=-1;
  this->FreeAddressInfo();
}


void WebCrawler::PingCalculatorStatus()
{ MacroRegisterFunctionWithName("WebCrawler::PingCalculatorStatus");
  std::stringstream reportStream;
  this->lastTransaction="";
  this->lastTransactionErrors="";
  memset(&this->hints, 0, sizeof this->hints); // make sure the struct is empty
  this->hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  this->hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
  this->serverInfo=0;
  int status=getaddrinfo(this->addressToConnectTo.c_str(), this->portOrService.c_str(), &hints, &this->serverInfo);
  if (status!= 0)
  { this->lastTransactionErrors =  "Could not find address: getaddrinfo error: ";
    this->lastTransactionErrors+= gai_strerror(status);
    this->FreeAddressInfo();
    return;
  }
  if (this->serverInfo==0)
  { this->lastTransactionErrors= "Server info is zero";
    return;
  }
  struct addrinfo *p=0;  // will point to the results
  this->theSocket=-1;
  char ipString[INET6_ADDRSTRLEN];
  for(p = this->serverInfo; p != 0; p = p->ai_next, close(this->theSocket))
  { void *theAddress=0;
    this->theSocket=-1;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      theAddress = &(ipv4->sin_addr);
      reportStream << "IPv4: ";
    } else
    { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      theAddress = &(ipv6->sin6_addr);
      reportStream << "IPv6: ";
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, theAddress, ipString, sizeof ipString);
//    std::string ipString()
    reportStream << this->addressToConnectTo << ": " << ipString << "<br>";
    this->theSocket= socket(this->serverInfo->ai_family, this->serverInfo->ai_socktype, this->serverInfo->ai_protocol);
    int connectionResult=-1;
    if (this->theSocket<0)
    { this->lastTransactionErrors= "failed to create socket ";
      continue;
    } else
      connectionResult =connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
    if (connectionResult==-1)
    { this->lastTransactionErrors+= "<br>failed to connect";
      close(this->theSocket);
      continue;
    } else
      reportStream << "\nconnected";
    std::string getMessage= "GET /calculator?request=statusPublic";

    int numBytes = write(this->theSocket, getMessage.c_str(), getMessage.size());
    if ((unsigned) numBytes !=getMessage.size())
    { this->lastTransactionErrors+= "\nERROR writing to socket";
      close(this->theSocket);
      continue;
    }
    numBytes = read(this->theSocket, buffer.TheObjects,49999);
    if (numBytes < 0)
    { this->lastTransactionErrors+= "\nERROR reading from socket";
      close(this->theSocket);
      continue;
    }
    std::string readString;
    readString.assign(buffer.TheObjects, numBytes);
    reportStream << "<br>Read: " << readString;
    this->lastTransaction=reportStream.str();
    close(this->theSocket);
  }
  this->FreeAddressInfo();
}

bool CalculatorFunctionsGeneral::innerFetchWebPage(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFetchWebPage");
//  if (!theGlobalVariables.UserDefaultHasAdminRights())
//    return output.AssignValue((std::string) "Fetching web pages available only for logged-in admins. ", theCommands);

  WebCrawler theCrawler;
  theCrawler.init();
  theCrawler.addressToConnectTo="localhost";
  theCrawler.portOrService="8080";
  theCrawler.PingCalculatorStatus();
  return output.AssignValue(theCrawler.lastTransactionErrors+"<hr>"+theCrawler.lastTransaction, theCommands);
}




