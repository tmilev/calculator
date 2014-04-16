//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

Socket ClientSocket;

const char* PORT ="80";  // the port users will be connecting to
const int BACKLOG =10;     // how many pending connections queue will hold

void sigchld_handler(int s)
{ while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string ClientMessage::ToString()const
{ std::stringstream out;
  out << "<br>\n";
  if (this->requestType==this->requestTypeGetCalculator)
    out << "<br>GET " << "(from calculator)";
  else if (this->requestType==this->requestTypePostCalculator)
    out << "<br>POST " << "(from calculator)";
  else if (this->requestType==this->requestTypeGetNotCalculator)
    out << "<br>GET " << "(NOT from calculator)";
  else
    out << "<br>Request type undefined.";
  out << "<hr>Main address: " << this->mainAddress;
  out << "<br>Main argument: " << this->mainArgument;
  out << "<br>Physical file address referred to by main address: " << this->PhysicalFileName;

  out << "<hr>";
  if (requestType==this->requestTypeGetCalculator || requestType==this->requestTypeGetNotCalculator)
    out << "GET " << this->mainAddress;
  if (requestType==this->requestTypePostCalculator)
    out << "POST " << this->mainAddress;
  if (this->theStrings.size>0)
  { out << "<br>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];
  }
  out << "\n<br>\nFull client message:\n" << this->theMessage;
  return out.str();
}

void ClientMessage::resetEverythingExceptMessageString()
{ this->mainArgument="";
  this->mainAddress="";
  this->PhysicalFileName="";
  this->theStrings.SetSize(0);
  this->requestType=this->requestTypeUnknown;
}

void ClientMessage::ExtractArgumentFromAddress()
{ MacroRegisterFunctionWithName("ClientMessage::ExtractArgumentFromAddress");
//  std::cout << "\nmain address:" << this->mainAddress << "=?="
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size(): "
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size();

  if (this->mainAddress.size()<onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size())
    return;
  std::string theAddressNoCalculator=
  this->mainAddress.substr(0, onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size());
//  std::cout << "\nthe address no calculator:" << theAddressNoCalculator;
  if (theAddressNoCalculator!=onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath)
    return;
  this->requestType=this->requestTypeGetCalculator;
  if (this->mainAddress.size()==onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size())
    return;
  this->mainArgument = this->mainAddress.substr
  (onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size()+1, std::string::npos);
}

void ClientMessage::ParseMessage()
{ MacroRegisterFunctionWithName("ClientMessage::ParseMessage");
  this->resetEverythingExceptMessageString();
  std::string buffer;
  buffer.reserve(this->theMessage.size());
  this->theStrings.SetExpectedSize(20);
  for (unsigned i =0; i<this->theMessage.size(); i++)
    if (theMessage[i]!=' ' && theMessage[i]!='\n' && theMessage[i]!='\r')
    { buffer.push_back(this->theMessage[i]);
      if (i==this->theMessage.size()-1)
        this->theStrings.AddOnTop(buffer);
    } else
      if (buffer!="")
      { this->theStrings.AddOnTop(buffer);
        buffer="";
      }
  for (int i=0; i<this->theStrings.size; i++)
    if (this->theStrings[i]=="GET")
    { this->requestType=this->requestTypeGetNotCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddress=this->theStrings[i];
        this->ExtractArgumentFromAddress();
      }
    } else if (this->theStrings[i]=="POST")
    { this->requestType=this->requestTypePostCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddress=this->theStrings[i];
        this->mainArgument=*this->theStrings.LastObject();
      }
    }
}

void SendStringToSocket(const std::string& theString)
{ ClientSocket.QueueStringForSending(theString, false);
}

void Socket::QueueStringForSending(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("Socket::SendStringToSocket");
  this->remainsToBeSent+=stringToSend;
  if (this->remainsToBeSent.size()>=1024*512 || MustSendAll)
    this->SendAll();
}

void Socket::SendAll()
{ if (this->remainsToBeSent.size()==0)
    return;
  MacroRegisterFunctionWithName("Socket::SendAll");
  if (this->socketID==-1)
    crash << "SocketID with id -1 was requested to send string " << this->remainsToBeSent
    << ", this shouldn't happen." << crash;
  std::cout << "\nSending " << this->remainsToBeSent.size() << " bytes in chunks of: ";
  int numBytesToSend=this->remainsToBeSent.size();
  int numBytesSent=0;
  while (numBytesSent<numBytesToSend)
  { numBytesSent+=send(ClientSocket.socketID, this->remainsToBeSent.c_str(), this->remainsToBeSent.size(),0);
    std::cout << numBytesSent;
    if (numBytesSent<numBytesToSend)
    { std::cout << ", ";
      this->remainsToBeSent=this->remainsToBeSent.substr(numBytesSent, std::string::npos);
    }
  }
  this->remainsToBeSent="";
}

bool Socket::Receive()
{ MacroRegisterFunctionWithName("Socket::Receive");
  int numBytesReceived=-1;
  char buffer[4096*2];
  numBytesReceived= recv(this->socketID, &buffer, 4096*2-1, 0);
  if (numBytesReceived>0)
  { numBytesReceived++;
    buffer[numBytesReceived]='\0';//null termination ensured
    this->lastMessageReceived.theMessage=buffer;
  } else
    this->lastMessageReceived.theMessage="";
  this->lastMessageReceived.ParseMessage();
//  std::cout << "\nReceived from client: " << this->lastMessageReceived.ToString() << "\n";
  return true;
}

void ClientMessage::ExtractPhysicalAddressFromMainAddress()
{ MacroRegisterFunctionWithName("ClientMessage::ExtractPhysicalAddressFromMainAddress");
  if (this->mainAddress.size()<onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size())
    return;
  this->PhysicalFileName=onePredefinedCopyOfGlobalVariables.PhysicalPathServerBase+
  this->mainAddress.substr(onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size(), std::string::npos);
}

int Socket::ProcessGetRequestFolder()
{ MacroRegisterFunctionWithName("Socket::ProcessGetRequestFolder");
  std::stringstream out;
  out << "HTTP/1.1 200 OK\n" << "Content-Type: text/html\n\n"
  << "<html><body>" << this->lastMessageReceived.ToString();

  List<std::string> theFileNames;
  if (!FileOperations::GetFolderFileNames(this->lastMessageReceived.PhysicalFileName, theFileNames))
  { out << "<b>Failed to open directory with physical address " << this->lastMessageReceived.PhysicalFileName
    << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->lastMessageReceived.mainAddress
  << "<br>Physical address: " << this->lastMessageReceived.PhysicalFileName << "<hr>";
  for (int i=0; i<theFileNames.size; i++)
    out << "<a href=\"" << theFileNames[i] << "\">" << theFileNames[i] << "</a><br>";
  out << "</body></html>";
  stOutput << out.str();
  return 0;
}

bool Socket::IsFileExtensionOfBinaryFile(const std::string& fileExtension)
{

return false;
}

std::string Socket::GetMIMEtypeFromFileExtension(const std::string& fileExtension)
{ MacroRegisterFunctionWithName("Socket::GetMIMEtypeFromFileExtension");
  if (fileExtension==".html")
    return "Content-Type: text/html\n\n";
  if (fileExtension==".txt")
    return "Content-Type: text/plain\n\n";
  if (fileExtension==".png")
  { std::cout << "\nGET-ing .png!";
    return "Content-Type: image/png\n\n";
  }
  if (fileExtension==".js")
  { std::cout << "\bGET-ing .js!";
    return "Content-Type: text/script-javascript";
  }
  return "Content-Type: application/octet-stream\n\n";
}

int Socket::ProcessGetRequestNonCalculator()
{ MacroRegisterFunctionWithName("Socket::ProcessGetRequestNonCalculator");
  this->lastMessageReceived.ExtractPhysicalAddressFromMainAddress();
  if (FileOperations::IsFolder(this->lastMessageReceived.PhysicalFileName))
    return this->ProcessGetRequestFolder();
  if (!FileOperations::FileExists(this->lastMessageReceived.PhysicalFileName))
  { stOutput << "HTTP/1.1 404 Object not found\n";
    stOutput << "Content-Type: text/html\n\n";
    stOutput << this->lastMessageReceived.ToString();
    stOutput << "<html><body>File does not exist.<br> File display name: "
    << this->lastMessageReceived.mainAddress << "<br>File physical name: "
    << this->lastMessageReceived.PhysicalFileName << "</body></html>";
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->lastMessageReceived.PhysicalFileName);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFile(theFile, this->lastMessageReceived.PhysicalFileName, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\n"
    << "Content-Type: text/html\n\n";
    stOutput << "Content-Type: text/html\n\n";
    stOutput << "<html><body><b>Error: failed to open file. "
    << "<br> File display name: "
    << this->lastMessageReceived.mainAddress << "<br>File physical name: "
    << this->lastMessageReceived.PhysicalFileName << "</body></html>";
    return 0;
  }
  stOutput << "HTTP/1.1 200 OK\n" << this->GetMIMEtypeFromFileExtension(fileExtension);
  const unsigned int bufferSize=64*1024;
  char buffer[bufferSize];
  theFile.read(&buffer[0], bufferSize);
  int numBytesRead=theFile.gcount();
  while (numBytesRead!=0)
  { stOutput << (buffer);
    theFile.read(buffer, bufferSize);
    numBytesRead=theFile.gcount();
  }
  return 0;
}

int Socket::ProcessRequestTypeUnknown()
{ MacroRegisterFunctionWithName("Socket::ProcessGetRequestNonCalculator");
  stOutput << "HTTP/1.0 501 Method Not Implemented\n";
  stOutput << "Content-Type: text/html\n\n";
  stOutput << "<b>Requested method is not implemented. <b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->lastMessageReceived.ToString();

  return 0;
}

int main_HttpServer()
{ MacroRegisterFunctionWithName("main_HttpServer");
  int sockfd;  // listen on sock_fd, new connection on new_fd
  addrinfo hints, *servinfo, *p;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char userAddress[INET6_ADDRSTRLEN];
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
  { std::cout << "getaddrinfo: " << gai_strerror(rv);
    return 1;
  }
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next)
  { if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    { stOutput << "Error: socket failed\n";
      continue;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      crash << "Error: setsockopt failed.\n" << crash;
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    { close(sockfd);
      std::cout << "Error: bind failed. \n";
      continue;
    }
    break;
  }
  if (p == NULL)
    crash << "Failed to bind to port " << PORT << "\n" << crash;
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(sockfd, BACKLOG) == -1)
    crash << "Listen function failed." << crash;
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    std::cout << "sigaction returned -1";
  std::cout << "\nServer: waiting for connections...";
  while(1)
  { // main accept() loop
    sin_size = sizeof their_addr;
    ClientSocket.socketID = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (ClientSocket.socketID == -1)
    { std::cout << "Accept failed.";
      continue;
    }
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddress, sizeof userAddress);
    std::cout << "\n\rServer: got connection from " << userAddress;
    if (!fork()) //creates an almost identical copy of this process, the original process is the parent, the almost identical copy is the child.
    { // this is the child process
      InitializeTimer();
      close(sockfd); // child doesn't need the listener

      ClientSocket.Receive();
      stOutput.theOutputFunction=SendStringToSocket;
      return 1;
    }
    close(ClientSocket.socketID);  // parent doesn't need this
  }
  return 0;
}

