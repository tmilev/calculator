//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader1General5TimeDate.h"
#include "webserver.h"
#include "vpfHeader5Crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <libexplain/connect.h>


extern WebServer theWebServer;
ProjectInformationInstance ProjectInfoVpf6_5calculatorWebRoutines(__FILE__, "Calculator web routines. ");

class WebCrawler
{
public:
//  SSLdata theSSlData;
  int theSocket;
  std::string portOrService;
  std::string addressToConnectTo;
  std::string serverToConnectTo;
  std::string lastTransactionErrors;
  std::string lastTransaction;
  std::string headerReceived;
  std::string bodyReceived;

  bool flagInitialized;
  List<char> buffer;
  struct sockaddr_in serverAddress;
  struct hostent *serverOtherSide;
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  WebCrawler();
  ~WebCrawler();
  void UpdatePublicKeys(std::stringstream* comments);
  void FetchWebPagePart2(std::stringstream* comments);
  void init();
  void PingCalculatorStatus();
  void FreeAddressInfo();
  void FetchWebPage(std::stringstream* comments);
};

void MonitorWebServer()
{ MacroRegisterFunctionWithName("MonitorWebServer");
  WebCrawler theCrawler;
  theCrawler.init();
  int maxNumPingFailures=3;
  //warning: setting theWebServer.WebServerPingIntervalInSeconds to more than 1000
  //may overflow the variable int microsecondsToSleep.
  //theWebServer.WebServerPingIntervalInSeconds=1000;
  if (theWebServer.WebServerPingIntervalInSeconds>30)
    theLog << logger::red << "**********WARNING**************"
    << logger::endL
    << logger::red << " The ping interval: "
    << theWebServer.WebServerPingIntervalInSeconds
    << " is set to a large value. "
    << "Set the ping interval to less than 30 seconds to remove this message. " << logger::endL;
  int microsecondsToSleep=1000000*theWebServer.WebServerPingIntervalInSeconds;//
  theLog << logger::blue << "Pinging " << theCrawler.addressToConnectTo << " at port/service "
  << theCrawler.portOrService << " every " << (microsecondsToSleep/1000000) << " second(s). "
  << logger::endL;
  theLog << logger::red << "Please beware that the server will restart and you will lose all computations "
  << "if " << maxNumPingFailures << " consecutive pings fail. " << logger::endL;
  int numConsecutiveFailedPings=0;
  int numPings=0;
  TimeWrapper now;
  for (;;)
  { theGlobalVariables.FallAsleep(microsecondsToSleep);
    theWebServer.BackupDatabaseIfNeeded();
    //std::cout << "Pinging " << theCrawler.addressToConnectTo
    //<< " at port/service " << theCrawler.portOrService << ".\n";
    theCrawler.PingCalculatorStatus();
    numPings++;
    if (theCrawler.lastTransactionErrors!="")
    { now.AssignLocalTime();
      numConsecutiveFailedPings++;
      logIO << logger::red << "Connection monitor: Ping of " << theCrawler.addressToConnectTo
      << " at port/service " << theCrawler.portOrService
      << " failed on " << now.ToStringHumanReadable() << ". " << "Got the following errors/messages: "
      << theCrawler.lastTransactionErrors << theCrawler.lastTransaction << ". "
      <<  numConsecutiveFailedPings << " consecutive fails so far, restarting on " << maxNumPingFailures << "." << logger::endL;
    } else
    { std::cout << "Connection monitor: Ping success #" << numPings << std::endl;
      numConsecutiveFailedPings=0;
    }
    if (numConsecutiveFailedPings>=maxNumPingFailures)
    { logProcessKills << logger::red << "Server stopped responding (probably locked pipe?)"
      << ", restarting. " << logger::endL;
      std::fstream theFile;
      FileOperations::OpenFileCreateIfNotPresentVirtual
      (theFile,"/LogFiles/_unexpected_Restart_Log.txt",true, false, false, true);
      theFile << "Unexpected server restart: server stopped responding (locked pipe?). Time: "
      << now.ToStringHumanReadable() << "\n";
      theFile.flush();
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
  if (!theWebServer.flagPort8155)
  { this->portOrService="8080";
//    this->addressToConnectTo="localhost";
    this->addressToConnectTo="127.0.0.1";
  } else
  { this->portOrService="8155";
    this->addressToConnectTo="127.0.0.1";
  }
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
    { fd_set fdConnectSockets;
      FD_ZERO(&fdConnectSockets);
      FD_SET(this->theSocket, &fdConnectSockets);
      timeval timeOut;
      timeOut.tv_sec=1;
      timeOut.tv_usec=0;
      int numPingFails=0;
      int numSelected=0;
      std::stringstream failStream;
      do
      { if (numPingFails>10)
          break;
        numSelected=select(this->theSocket+1, &fdConnectSockets, 0, 0, &timeOut);
        failStream << "While pinging, select failed. Error message: "
        << strerror(errno) << ". \n";
        numPingFails++;
      } while (numSelected<0);
      if (numSelected<=0)
      { logIO << logger::red << failStream.str() << logger::endL;
        reportStream << failStream.str() << "Could not connect through port. Select returned: " << numSelected;
        connectionResult=-1;
      } else
        connectionResult =connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
    }
    if (connectionResult==-1)
    { reportStream << "<br>Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ". ";
//      << explain_errno_connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      this->lastTransactionErrors= reportStream.str();
      close(this->theSocket);
      continue;
    } else
      reportStream << "<br>connected: " << this->addressToConnectTo << " port: " << this->portOrService << ". ";
    std::string getMessage=  "GET /cgi-bin/calculator?request=statusPublic";
    int numBytes =Pipe::WriteWithTimeoutViaSelect(this->theSocket,getMessage, 1, 10, &reportStream);
    if ((unsigned) numBytes !=getMessage.size())
    { this->lastTransactionErrors+= "\nERROR writing to socket";
      close(this->theSocket);
      continue;
    }
    numBytes = Pipe::ReadWithTimeOutViaSelect(this->theSocket, this->buffer,1,10, &reportStream);
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

void WebCrawler::FetchWebPage(std::stringstream* comments)
{ MacroRegisterFunctionWithName("WebCrawler::FetchWebPage");
  logOpenSSL << logger::green  << "DEBUG: got to FetchWebPage start. " << logger::endL;
  this->lastTransaction="";
  this->lastTransactionErrors="";
  memset(&this->hints, 0, sizeof this->hints); // make sure the struct is empty
  this->hints.ai_family   = AF_UNSPEC  ;       // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM;       // TCP stream sockets
  this->hints.ai_flags    = AI_PASSIVE ;       // fill in my IP for me
  this->serverInfo=0;
  int status=getaddrinfo(this->serverToConnectTo.c_str(), this->portOrService.c_str(), &hints, &this->serverInfo);
  if (status!= 0)
  { this->lastTransactionErrors =  "Error calling getaddrinfo: ";
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
  timeval timeOut;
  for(p = this->serverInfo; p != 0; p = p->ai_next, close(this->theSocket))
  { void *theAddress=0;
    this->theSocket=-1;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      theAddress = &(ipv4->sin_addr);
      if (comments!=0)
        *comments << "IPv4: ";
    } else
    { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      theAddress = &(ipv6->sin6_addr);
      if (comments!=0)
        *comments << "IPv6: ";
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, theAddress, ipString, sizeof ipString);
//    std::string ipString()
    if (comments!=0)
      *comments << this->addressToConnectTo << ": " << ipString << "<br>";
    this->theSocket= socket(this->serverInfo->ai_family, this->serverInfo->ai_socktype, this->serverInfo->ai_protocol);
    int connectionResult=-1;
    if (this->theSocket<0)
    { this->lastTransactionErrors= "failed to create socket ";
      continue;
    } else
    { fd_set fdConnectSockets;
      FD_ZERO(&fdConnectSockets);
      FD_SET(this->theSocket, &fdConnectSockets);
      timeOut.tv_sec=1;
      timeOut.tv_usec=0;
      int numPingFails=0;
      int numSelected=0;
      std::stringstream failStream;
      do
      { if (numPingFails>10)
          break;
        numSelected=select(this->theSocket+1, &fdConnectSockets, 0, 0, &timeOut);
        failStream << "While pinging, select failed. Error message: "
        << strerror(errno) << ". \n";
        numPingFails++;
      } while (numSelected<0);
      if (numSelected<=0)
      { logIO << logger::red << failStream.str() << logger::endL;
        if (comments!=0)
          *comments << failStream.str() << "Could not connect through port. Select returned: " << numSelected;
        connectionResult=-1;
      } else
        connectionResult =connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      timeOut.tv_sec = 3;  // 5 Secs Timeout
      timeOut.tv_usec = 0;  // Not init'ing this can cause strange errors
      setsockopt(connectionResult, SOL_SOCKET, SO_RCVTIMEO, (void*)(&timeOut), sizeof(timeval));
    }
    if (connectionResult==-1)
    { std::stringstream errorStream;
      errorStream
      << "Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ".<br>";
      if (comments!=0)
        *comments << errorStream.str();

      this->lastTransactionErrors= errorStream.str();
      close(this->theSocket);
      continue;
    } else
    { if (comments!=0)
        *comments << "connected: "
        << this->addressToConnectTo << " port: " << this->portOrService << ". <hr>";
    }
    this->FetchWebPagePart2(comments);
    theWebServer.theSSLdata.RemoveLastSocketClient();
    close(this->theSocket);
    break;
  }
  this->FreeAddressInfo();
}

void WebCrawler::FetchWebPagePart2(std::stringstream* comments)
{ MacroRegisterFunctionWithName("WebCrawler::FetchWebPagePart2");
  std::stringstream theMessageHeader, theContinueHeader;
  theMessageHeader << "GET " << this->addressToConnectTo << " HTTP/1.1"
  << "\r\n" << "Host: " << this->serverToConnectTo << "\r\n\r\n";
  if (!theWebServer.theSSLdata.HandShakeIamClientNoSocketCleanup(this->theSocket, comments))
  { if (comments!=0)
      *comments << "Could not shake hands. ";
    return;
  }
  this->headerReceived="";
  this->bodyReceived="";
  if (comments!=0)
    *comments << "<hr>";
  if (!theWebServer.theSSLdata.SSLwriteLoop
      (10, theWebServer.theSSLdata.sslClient, theMessageHeader.str(),
       comments, comments, true))
    return;
  if (!theWebServer.theSSLdata.SSLreadLoop
      (10,theWebServer.theSSLdata.sslClient, this->headerReceived, 0, comments, comments, true))
    return;
  unsigned bodyStart=0;
  int numcrlfs=0;
  //std::stringstream tempStream;
  for (; bodyStart<this->headerReceived.size(); bodyStart++)
  { if (numcrlfs>=4)
      break;
    if (this->headerReceived[bodyStart]=='\n' ||
        this->headerReceived[bodyStart]=='\r')
    { numcrlfs++;
      //if (fullMessage[bodyStart]=='\n')
      //  tempStream << "\\N";
      //else
      //  tempStream << "\\R";
    } else
    { numcrlfs=0;
      //tempStream << fullMessage[bodyStart];
    }
  }
  if (bodyStart<this->headerReceived.size())
  { this->bodyReceived=this->headerReceived.substr(bodyStart);
    this->headerReceived=this->headerReceived.substr(0, bodyStart);
  }
  List<std::string> theHeaderPieces;
  MathRoutines::StringSplitDefaultDelimiters(this->headerReceived, theHeaderPieces);
  std::string expectedLengthString;
  for (int i=0; i<theHeaderPieces.size; i++)
    if (theHeaderPieces[i]=="Content-length:" ||
        theHeaderPieces[i]=="Content-Length:" ||
        theHeaderPieces[i]=="content-length:"
        )
      if (i+1<theHeaderPieces.size)
      { expectedLengthString=theHeaderPieces[i+1];
        break;
      }
  LargeInt expectedLength=-1;
  if (expectedLengthString!="")
    expectedLength.AssignString(expectedLengthString);
  if (comments!=0)
    *comments << "<br>Expected length: " << expectedLength;
  if (comments!=0)
    *comments << "<br>Header:<br>" << this->headerReceived;
  if (this->headerReceived.find("HTTP/1.1 200 OK")==std::string::npos)
  { if (comments!=0)
      *comments << "No http ok message found. " ;
    return;
  }
  theContinueHeader << "HTTP/1.1 100 Continue\r\n\r\n";
  if (!theWebServer.theSSLdata.SSLwriteLoop
      (10, theWebServer.theSSLdata.sslClient, theContinueHeader.str(),
       comments, comments, true))
    return;
  std::string secondPart;
  if (!theWebServer.theSSLdata.SSLreadLoop
      (10,theWebServer.theSSLdata.sslClient, secondPart, expectedLength, comments, comments, true))
    return;
  if (comments!=0)
    *comments << "<br>Second part length: "
    << secondPart.size();
  this->bodyReceived+=secondPart;
  //if (!theWebServer.theSSLdata.SSLreadLoop
  //    (10,theWebServer.theSSLdata.sslClient, secondPart, comments, comments, true))
  //  return;
  //this->bodyReceived+=secondPart;
  //stOutput << tempStream.str();
  if (comments!=0)
    *comments << "<br>Body (length: "
    << this->bodyReceived.size()
    << ")<br>" << this->bodyReceived;
}

bool CalculatorFunctionsGeneral::innerFetchWebPage(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFetchWebPage");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return output.AssignValue((std::string) "Fetching web pages available only for logged-in admins. ", theCommands);
  WebCrawler theCrawler;
  if (input.size()!=4)
    return theCommands << "Fetching web page expects 3 arguments: server, service/port, and webpage. ";
  if (!input[1].IsOfType(&theCrawler.serverToConnectTo))
    theCrawler.serverToConnectTo=input[1].ToString();
  if (!input[2].IsOfType(&theCrawler.portOrService))
    theCrawler.portOrService=input[2].ToString();
  if (!input[3].IsOfType(&theCrawler.addressToConnectTo))
    theCrawler.addressToConnectTo=input[3].ToString();
  std::stringstream out;
  out
  << "Server:  " << theCrawler.serverToConnectTo
  << " port: " << theCrawler.portOrService
  << " resource: " << theCrawler.addressToConnectTo
  << "<br>";
  theCrawler.FetchWebPage(&out);
  out << "<br>" << theCrawler.lastTransactionErrors << "<hr>" << theCrawler.lastTransaction;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerFetchKnownPublicKeys(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFetchKnownPublicKeys");
  (void) input;
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { out << "You need to be a logged-in admin to call this function. ";
    return output.AssignValue(out.str(), theCommands);
  }
  WebCrawler theCrawler;
  theCrawler.UpdatePublicKeys(& out);
  return output.AssignValue(out.str(), theCommands);
}

void WebCrawler::UpdatePublicKeys(std::stringstream* comments)
{ MacroRegisterFunctionWithName("WebCrawler::UpdatePublicKeys");
  this->serverToConnectTo  = "www.googleapis.com";
  this->portOrService      = "https";
  this->addressToConnectTo = "https://www.googleapis.com/oauth2/v3/certs";
  this->FetchWebPage(comments);
  if (this->bodyReceived=="")
  { if (comments!=0)
      *comments << "Could not fetch google certificate list";
    return;
  }
  std::string googleKeysFileName="certificates-public/google.txt";
  std::fstream googleKeysFile;
  if (! FileOperations::OpenFileCreateIfNotPresentVirtual
       (googleKeysFile, googleKeysFileName, false, true, false))
  { if (comments!=0)
      *comments << "<br>Failed to open: " << googleKeysFileName;
    return;
  }
  if (comments!=0)
    *comments << "<br>Updated file: " << googleKeysFileName;
  googleKeysFile << this->bodyReceived;
  googleKeysFile.flush();
}

bool Crypto::VerifyJWTagainstKnownKeys
(const std::string& inputToken, std::stringstream* commentsOnFailure,
 std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("Crypto::VerifyJWTagainstKnownKeys");
  //This function is slightly insecure.
  //If an attacker hijacks a machine connecting the server to the outside
  //and impersonates google
  //the attacker would be able to impersonate another user.
  //Since we are reloading the google keys for
  //every JWT key we fail to find in our cache,
  //this could happen more easily
  //than if we were storing the google keys until their expiry date.
  //To solve this problem, we need to
  //ensure that the google public keys are authentic.
  //For that we need to leverage the built-in web of trust of
  //the host system (pgp keys, etc.).
  //This is system dependent and a lot of work-> not doing now, but will do
  //in the future as the need arises.
  JSONWebToken theToken;
  //if (commentsGeneral!=0)
  //  *commentsGeneral << "DEBUG: inputToken: " << inputToken;
  if (!theToken.AssignString(inputToken, commentsOnFailure))
    return false;
  std::string keyIDstring="";
  JSData header;
  if (!header.readstring(theToken.headerJSON))
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Couldn't load JSON from the user token.";
    return false;
  }
  if (header.type==header.JSObject)
    if (header.HasKey("kid"))
      keyIDstring=header.GetValue("kid").string;
  if (keyIDstring=="")
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Couldn't find key id in the token.";
    return false;
  }
  int theIndex=-1;
  for (int i=0; i<2; i++)
  { Crypto::LoadKnownCertificates(commentsOnFailure);
    for (int j=0; j<Crypto::knownCertificates.size; j++)
      if (keyIDstring==Crypto::knownCertificates[j].keyid)
      { theIndex=j;
        break;
      }
    if (commentsGeneral!=0)
      *commentsGeneral << "<br>Couldn't find key ID from string one. ";
    if (i==1)
      break;
    if (commentsGeneral!=0 )
      *commentsGeneral << "<br>Reloading google public keys. ";
    WebCrawler theCrawler;
    theCrawler.UpdatePublicKeys(commentsGeneral);
  }
  if (theIndex==-1)
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Could not find key id: " << keyIDstring << ". ";
    return false;
  }
  Certificate& currentCert=Crypto::knownCertificates[theIndex];
  return theToken.VerifyRSA256
  (currentCert.theModuluS, currentCert.theExponenT,
   commentsOnFailure, commentsGeneral);
}
