//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

Socket ClientSocket;

const char* PORT ="8080";  // the port users will be connecting to
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

std::string ClientMessage::ToStringShort(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool useHtml=theFormat==0 ? false: theFormat->flagUseHTML;
  std::string lineBreak= useHtml ? "<br>\n" : "\r\n";
  out << lineBreak;
  if (this->requestType==this->requestTypeGetCalculator)
    out << "GET " << "(from calculator)";
  else if (this->requestType==this->requestTypePostCalculator)
    out << "POST " << "(from calculator)";
  else if (this->requestType==this->requestTypeGetNotCalculator)
    out << "GET " << "(NOT from calculator)";
  else
    out << "Request type undefined.";
  out << lineBreak << "<hr>Main address: " << this->mainAddress;
  out << lineBreak << "Main argument: " << this->mainArgument;
  out << lineBreak << "Physical file address referred to by main address: " << this->PhysicalFileName;
  return out.str();
}

std::string ClientMessage::ToString()const
{ std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  out << this->ToStringShort(&tempFormat);

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

  if (this->mainAddress.size()<=onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size()|| this->mainAddress.size()==0)
    return;
  std::string theAddressNoCalculator=
  this->mainAddress.substr(0, onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size());
//  std::cout << "\nthe address no calculator:" << theAddressNoCalculator;
  if (theAddressNoCalculator!=onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath)
    return;
  this->requestType=this->requestTypeGetCalculator;
  if (this->mainAddress.size()<=onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size())
    return;
  this->mainArgument = this->mainAddress.substr
  (onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size()+1, std::string::npos);
}

void ClientMessage::ParseMessage()
{ MacroRegisterFunctionWithName("ClientMessage::ParseMessage");
  this->resetEverythingExceptMessageString();
  std::string buffer;
  buffer.reserve(this->theMessage.size);
  this->theStrings.SetExpectedSize(20);
  for (int i =0; i<this->theMessage.size; i++)
    if (theMessage[i]!=' ' && theMessage[i]!='\n' && theMessage[i]!='\r')
    { buffer.push_back(this->theMessage[i]);
      if (i==this->theMessage.size-1)
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

void FlushSocket()
{ ClientSocket.SendAllBytes();
}

void SendStringToSocket(const std::string& theString)
{ ClientSocket.QueueStringForSending(theString, false);
}

void Socket::QueueBytesForSending
  (const List<char>& bytesToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("Socket::QueueBytesForSending");
  this->remainingBytesToSend.AddListOnTop(bytesToSend);
  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
    this->SendAllBytes();
}

void Socket::QueueStringForSending(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("Socket::SendStringToSocket");
  int oldSize=this->remainingBytesToSend.size;
  this->remainingBytesToSend.SetSize(this->remainingBytesToSend.size+stringToSend.size());
  for (unsigned i=0; i<stringToSend.size(); i++)
    this->remainingBytesToSend[i+oldSize]=stringToSend[i];
//  if (stringToSend.size()>0)
//    if (stringToSend[stringToSend.size()-1]=='\0')
//      std::cout << "WARNING: string is null terminated!\r\n";
//    std::cout << "Last character string: " << *this->remainingBytesToSend.LastObject() << "\r\n";
  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
    this->SendAllBytes();
}

void Socket::SendAllBytes()
{ if (this->remainingBytesToSend.size==0)
    return;
  MacroRegisterFunctionWithName("Socket::SendAllBytes");
  if (this->socketID==-1)
    crash << "SocketID with id -1 was requested to send bytes, this shouldn't happen." << crash;
  std::cout << "\r\nSending " << this->remainingBytesToSend.size << " bytes in chunks of: ";
  while (this->remainingBytesToSend.size>0)
  { int numBytesSent=send(ClientSocket.socketID, &this->remainingBytesToSend[0], this->remainingBytesToSend.size,0);
    std::cout << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      std::cout << ", ";
  }
}

bool Socket::Receive()
{ MacroRegisterFunctionWithName("Socket::Receive");
  int numBytesReceived=-1;
  char buffer[4096*2];
  numBytesReceived= recv(this->socketID, &buffer, 4096*2-1, 0);
  this->lastMessageReceived.theMessage.SetSize(0);
  if (numBytesReceived>0)
  { int oldSize=this->lastMessageReceived.theMessage.size;
    this->lastMessageReceived.theMessage.SetSize(this->lastMessageReceived.theMessage.size+numBytesReceived);
    for (int i=0; i<numBytesReceived; i++)
      this->lastMessageReceived.theMessage[i+oldSize]=buffer[i];
  }
  this->lastMessageReceived.ParseMessage();
  return true;
}

void ClientMessage::ExtractPhysicalAddressFromMainAddress()
{ MacroRegisterFunctionWithName("ClientMessage::ExtractPhysicalAddressFromMainAddress");
  std::cout << Crasher::GetStackTraceEtcErrorMessage();
  if (this->mainAddress.size()<=onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size())
  { this->PhysicalFileName="";
    return;
  }
  this->PhysicalFileName=onePredefinedCopyOfGlobalVariables.PhysicalPathServerBase+
  this->mainAddress.substr(onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size(), std::string::npos);
}

int Socket::ProcessGetRequestFolder()
{ MacroRegisterFunctionWithName("Socket::ProcessGetRequestFolder");
  std::stringstream out;
  out << "HTTP/1.1 200 OK\r\n"
  << "Content-Type: text/html\r\n"
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
    out << "<a href=\"" << this->lastMessageReceived.mainAddress << "/" << theFileNames[i] << "\">"
    << theFileNames[i] << "</a><br>";
  out << "</body></html>";
  stOutput << out.str();
  return 0;
}

bool Socket::IsFileExtensionOfBinaryFile(const std::string& fileExtension)
{ if (fileExtension==".png")
    return true;

  return false;
}

std::string Socket::GetMIMEtypeFromFileExtension(const std::string& fileExtension)
{ MacroRegisterFunctionWithName("Socket::GetMIMEtypeFromFileExtension");
  if (fileExtension==".html")
    return "Content-Type: text/html\r\n";
  if (fileExtension==".txt")
    return "Content-Type: text/plain\r\n";
  if (fileExtension==".png")
    return "Content-Type: image/png\r\n";
  if (fileExtension==".js")
    return "Content-Type: text/javascript\r\n";
  return "Content-Type: application/octet-stream\r\n";
}

int Socket::ProcessGetRequestNonCalculator()
{ MacroRegisterFunctionWithName("Socket::ProcessGetRequestNonCalculator");
  this->lastMessageReceived.ExtractPhysicalAddressFromMainAddress();
  std::cout << this->lastMessageReceived.ToStringShort() << "\r\n";
  if (FileOperations::IsFolder(this->lastMessageReceived.PhysicalFileName))
    return this->ProcessGetRequestFolder();
  if (!FileOperations::FileExists(this->lastMessageReceived.PhysicalFileName))
  { stOutput << "HTTP/1.1 404 Object not found\r\n";
    stOutput << "Content-Type: text/html\r\n";
    stOutput << this->lastMessageReceived.ToString();
    stOutput << "<html><body>File does not exist.<br> File display name: "
    << this->lastMessageReceived.mainAddress << "<br>File physical name: "
    << this->lastMessageReceived.PhysicalFileName << "</body></html>";
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->lastMessageReceived.PhysicalFileName);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFile(theFile, this->lastMessageReceived.PhysicalFileName, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\r\n"
    << "Content-Type: text/html\r\n";
    stOutput << "Content-Type: text/html\r\n";
    stOutput << "<html><body><b>Error: failed to open file. "
    << "<br> File display name: "
    << this->lastMessageReceived.mainAddress << "<br>File physical name: "
    << this->lastMessageReceived.PhysicalFileName << "</body></html>";
    return 0;
  }
  theFile.seekp(0, std::ifstream::end);
  unsigned int fileSize=theFile.tellp();
  std::cout << "Serving file: " << this->lastMessageReceived.PhysicalFileName << " with file extension " << fileExtension
  << ", file size: " << fileSize << "\r\n";
  std::stringstream theHeader;
  theHeader << "HTTP/1.1 200 OK\r\n" << this->GetMIMEtypeFromFileExtension(fileExtension)
  << "Content-length: " << fileSize << "\r\n\r\n"
;
  std::cout << "Message: " << theHeader.str();
  this->QueueStringForSending(theHeader.str());
  const int bufferSize=64*1024;
  this->bufferFileIO.SetSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  int numBytesRead=theFile.gcount();
  std::cout << "Sending file ...  ";
  while (numBytesRead!=0)
  { this->bufferFileIO.SetSize(numBytesRead);
    this->QueueBytesForSending(this->bufferFileIO);
    this->bufferFileIO.SetSize(bufferSize);
    theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
    numBytesRead=theFile.gcount();
  }
  this->SendAllBytes();
  return 0;
}

int Socket::ProcessRequestTypeUnknown()
{ MacroRegisterFunctionWithName("Socket::ProcessRequestTypeUnknown");
  stOutput << "HTTP/1.0 501 Method Not Implemented\r\n";
  stOutput << "Content-Type: text/html\r\n";
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
  std::cout.flush();
  while(1)
  { // main accept() loop
    sin_size = sizeof their_addr;
    ClientSocket.socketID = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (ClientSocket.socketID == -1)
    { std::cout << "Accept failed.";
      continue;
    }
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddress, sizeof userAddress);
    std::cout << "\r\n\r\nServer: got connection from " << userAddress;
    std::cout.flush();
    if (!fork()) //creates an almost identical copy of this process, the original process is the parent, the almost identical copy is the child.
    { // this is the child process
      InitializeTimer();
      close(sockfd); // child doesn't need the listener

      ClientSocket.Receive();
      stOutput.theOutputFunction=SendStringToSocket;
      stOutput.flushOutputFunction=FlushSocket;
      return 1;
    }
    close(ClientSocket.socketID);  // parent doesn't need this
  }
  return 0;
}

