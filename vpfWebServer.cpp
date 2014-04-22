//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

extern void static_html4(std::stringstream& output);
extern void static_html3(std::stringstream& output);

WebServer theWebServer;

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
  out << lineBreak << "<hr>Main address raw: " << this->mainAddresSRAW;
  out << lineBreak << "<br>Main address: " << this->mainAddress;
  out << lineBreak << "Main argument: " << this->mainArgument;
  out << lineBreak << "Physical file address referred to by main address: " << this->PhysicalFileName;
  return out.str();
}

std::string ClientMessage::ToStringFull()const
{ std::stringstream out;
  out << this->ToString();
  if (this->theStrings.size>0)
  { out << "<hr>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];
  }
  return out.str();
}

std::string ClientMessage::ToString()const
{ std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  out << this->ToStringShort(&tempFormat);

  out << "<hr>";
  out << "Main address RAW: " << this->mainAddresSRAW << "<br>";
  if (requestType==this->requestTypeGetCalculator || requestType==this->requestTypeGetNotCalculator)
    out << "GET " << this->mainAddresSRAW;
  if (requestType==this->requestTypePostCalculator)
    out << "POST " << this->mainAddresSRAW;
  out << "\n<br>\nFull client message:\n" << this->theMessage;
  return out.str();
}

void ClientMessage::resetEverythingExceptMessageString()
{ this->mainArgument="";
  this->mainAddress="";
  this->mainAddresSRAW="";
  this->PhysicalFileName="";
  this->theStrings.SetSize(0);
  this->requestType=this->requestTypeUnknown;
  this->ContentLength=-1;
}

void ClientMessage::ExtractArgumentFromAddress()
{ MacroRegisterFunctionWithName("ClientMessage::ExtractArgumentFromAddress");
//  std::cout << "\nmain address:" << this->mainAddress << "=?="
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size(): "
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size();
  CGI::CivilizedStringTranslationFromCGI(this->mainAddresSRAW, this->mainAddress);
  this->mainArgument ="";
  if (this->mainAddresSRAW.size()<onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size())
    return;
  if (this->mainAddresSRAW.size()==0)
    return;
  std::string theAddressNoCalculator=
  this->mainAddresSRAW.substr(0, onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size());
//  std::cout << "\nthe address no calculator:" << theAddressNoCalculator;
  if (theAddressNoCalculator!=onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath)
    return;
  this->requestType=this->requestTypeGetCalculator;
  if (this->mainAddresSRAW.size()<=onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size()+1)
    return;
  this->mainArgument = this->mainAddresSRAW.substr
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
    { if (buffer!="")
      { this->theStrings.AddOnTop(buffer);
        buffer="";
      }
      if (i>0)
        if (theMessage[i]=='\n' && theMessage[i-1]=='\r')
          this->theStrings.AddOnTop("!CRLF!");
    }
  for (int i=0; i<this->theStrings.size; i++)
    if (this->theStrings[i]=="GET")
    { this->requestType=this->requestTypeGetNotCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddresSRAW=this->theStrings[i];
        this->ExtractArgumentFromAddress();
      }
    } else if (this->theStrings[i]=="POST")
    { this->requestType=this->requestTypePostCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddresSRAW=this->theStrings[i];
        if (*this->theStrings.LastObject()!="!CRLF!")
          this->mainArgument=*this->theStrings.LastObject();
        else
          this->mainArgument="";
      }
    } else if ((this->theStrings[i]=="Content-Length:" || this->theStrings[i]=="Content-length:" ||
                this->theStrings[i]=="content-length:")
               && i+1<this->theStrings.size)
    { if (this->theStrings[i+1].size()<10000)
      { LargeIntUnsigned theLI;
        if (theLI.AssignStringFailureAllowed(this->theStrings[i+1], true))
          if (!theLI.IsSmallEnoughToFitInInt(&this->ContentLength))
            this->ContentLength=-1;
      }
    }
}

void ReturnIndicatorAlthoughComputationIsNotDone()
{ theWebServer.StandardOutputReturnIndicatorWaitForComputation();
}

void FlushSocket()
{ theWebServer.theSocket.SendAllBytes();
}

void SendStringToSocket(const std::string& theString)
{ theWebServer.theSocket.QueueStringForSending(theString, false);
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
  if (this->connectedSocketID==-1)
    crash << "connectedSocketID with id -1 was requested to send bytes, this shouldn't happen." << crash;
  std::cout << "\r\nIn response to: " << this->lastMessageReceived.theMessage;
  std::cout << "\r\nSending " << this->remainingBytesToSend.size << " bytes in chunks of: ";
  while (this->remainingBytesToSend.size>0)
  { int numBytesSent=send(this->connectedSocketID, &this->remainingBytesToSend[0], this->remainingBytesToSend.size,0);
    std::cout << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      std::cout << ", ";
  }
}

bool Socket::ReceiveAll()
{ MacroRegisterFunctionWithName("Socket::ReceiveAll");
  unsigned const int bufferSize=60000;
  char buffer[bufferSize];
  int numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
  this->lastMessageReceived.theMessage.assign(buffer, numBytesInBuffer);
  std::cout << "\r\nConnection " << this->connectionID << ": received " << numBytesInBuffer << " bytes. ";
  this->lastMessageReceived.ParseMessage();
  std::cout << "\r\nContent length computed to be: " << this->lastMessageReceived.ContentLength;
  if (this->lastMessageReceived.ContentLength==-1)
    return true;
  if (this->lastMessageReceived.mainArgument.size()==(unsigned) this->lastMessageReceived.ContentLength)
    return true;
  std::cout << "\r\nContent-length parsed to be: " << this->lastMessageReceived.ContentLength
  << "\r\nHowever the size of mainArgument is: " << this->lastMessageReceived.mainArgument.size();
  if (this->lastMessageReceived.ContentLength>10000000)
  { error="\r\nContent-length parsed to be more than 10 million bytes, aborting.";
    std::cout << this->error;
    return false;
  }
  if (this->lastMessageReceived.mainArgument!="")
  { error= "\r\nContent-length does not coincide with the size of the message-body, yet the message-body is non-empty. Aborting.";
    std::cout << this->error;
    return false;
  }
  this->remainingBytesToSend="HTTP/1.1 100 Continue\r\n";
  this->SendAllBytes();
  this->remainingBytesToSend.SetSize(0);
  this->lastMessageReceived.mainArgument="";
  std::string bufferString;
  while ((signed) this->lastMessageReceived.mainArgument.size()<this->lastMessageReceived.ContentLength)
  { numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
    if (numBytesInBuffer<=0)
    { this->error= "\r\nWhile trying to fetch message-body, got an error or received 0 bytes. ";
      std::cout << this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->lastMessageReceived.mainArgument+=bufferString;
  }
  if ((signed) this->lastMessageReceived.mainArgument.size()!=this->lastMessageReceived.ContentLength)
  { std::stringstream out;
    out << "\r\nThe message-body received by me had length " << this->lastMessageReceived.mainArgument.size()
    << " yet I expected a message of length " << this->lastMessageReceived.ContentLength << ".";
    this->error=out.str();
    std::cout << this->error;
    return false;
  }
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
  out << "HTTP/1.1 200 OK\r\n"
  << "Content-Type: text/html\r\n" << "\r\n"
  << "<html><body>";
  //out << this->lastMessageReceived.ToString();
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNames(this->lastMessageReceived.PhysicalFileName, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address " << this->lastMessageReceived.PhysicalFileName
    << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->lastMessageReceived.mainAddress
  << "<br>Physical address: " << this->lastMessageReceived.PhysicalFileName << "<hr>";
  for (int i=0; i<theFileNames.size; i++)
  { bool isDir= theFileTypes[i]==".d";
    out << "<a href=\"" << this->lastMessageReceived.mainAddress << theFileNames[i];
    if (isDir)
      out << "/";
    out << "\">" << theFileNames[i];
    if (isDir)
      out << "/";
    out << "</a><br>";
  }
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
    stOutput << "\r\n";
    stOutput << "<html><body>";
    stOutput << "<b>File does not exist.</b><br><b> File display name:</b> " << this->lastMessageReceived.mainAddress
    << "<br><b>File physical name:</b> " << this->lastMessageReceived.PhysicalFileName;
    stOutput << "<hr><hr><hr>Message details:<br>" <<  this->lastMessageReceived.ToString();
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->lastMessageReceived.PhysicalFileName);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFile(theFile, this->lastMessageReceived.PhysicalFileName, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\r\n" << "Content-Type: text/html\r\n";
    stOutput << "\r\n"
    << "<html><body><b>Error: file appears to exist but I could not open it.</b> "
    << "<br><b> File display name: </b>"
    << this->lastMessageReceived.mainAddress << "<br><b>File physical name: </b>"
    << this->lastMessageReceived.PhysicalFileName << "</body></html>";
    return 0;
  }
  theFile.seekp(0, std::ifstream::end);
  unsigned int fileSize=theFile.tellp();
  std::stringstream theHeader;
  theHeader << "HTTP/1.1 200 OK\r\n" << this->GetMIMEtypeFromFileExtension(fileExtension)
  << "Content-length: " << fileSize << "\r\n\r\n";
  this->QueueStringForSending(theHeader.str());
  const int bufferSize=64*1024;
  this->bufferFileIO.SetSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  int numBytesRead=theFile.gcount();
  ///////////////////
  std::cout << "*****Message summary begin\r\n" << theHeader.str();
  std::cout << "Sending file  " << this->lastMessageReceived.PhysicalFileName << " with file extension " << fileExtension
  << ", file size: " << fileSize;
  std::cout << "\r\n*****Message summary end\r\n";
  ///////////////////
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
  stOutput << "HTTP/1.1 501 Method Not Implemented\r\n";
  stOutput << "Content-Type: text/html\r\n";
  stOutput << "\r\n"
  << "<b>Requested method is not implemented. </b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->lastMessageReceived.ToString();

  return 0;
}

WebServer::WebServer()
{ this->flagUsingBuiltInServer=false;
  this->flagIsChildProcess=false;
}

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
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
  { if ((this->listeningSocketID = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    { stOutput << "Error: socket failed\n";
      continue;
    }
    if (setsockopt(this->listeningSocketID, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      crash << "Error: setsockopt failed.\n" << crash;
    if (bind(this->listeningSocketID, p->ai_addr, p->ai_addrlen) == -1)
    { close(this->listeningSocketID);
      std::cout << "Error: bind failed. \n";
      continue;
    }
    break;
  }
  if (p == NULL)
    crash << "Failed to bind to port " << PORT << "\n" << crash;
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(this->listeningSocketID, BACKLOG) == -1)
    crash << "Listen function failed." << crash;
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    std::cout << "sigaction returned -1";
  std::cout << "\nServer: waiting for connections...\r\n";
  std::cout.flush();
  unsigned int connectionsSoFar=0;
  this->flagIsChildProcess=false;
  while(1)
  { // main accept() loop
    sin_size = sizeof their_addr;
    this->theSocket.connectedSocketID = accept(this->listeningSocketID, (struct sockaddr *)&their_addr, &sin_size);
    if (this->theSocket.connectedSocketID == -1)
    { std::cout << "Accept failed.";
      continue;
    }
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddress, sizeof userAddress);
    connectionsSoFar++;
    std::cout << "\r\nConnection " << connectionsSoFar << ": got connection from " << userAddress;
    std::cout.flush();
    if (!fork()) //creates an almost identical copy of this process, the original process is the parent, the almost identical copy is the child.
    { // this is the child process
      InitializeTimer();
      this->flagIsChildProcess=true;
      //close(sockfd); child does need the listener
      this->theSocket.connectionID=connectionsSoFar;
      stOutput.theOutputFunction=SendStringToSocket;
      stOutput.flushOutputFunction=FlushSocket;
      onePredefinedCopyOfGlobalVariables.ReturnIndicator=ReturnIndicatorAlthoughComputationIsNotDone;
      if (!this->theSocket.ReceiveAll())
      { stOutput << "HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\n\r\n" << this->theSocket.error;
        return 0;
      }
      return 1;
    }
    close(this->theSocket.connectedSocketID);  // parent doesn't need this
  }
  return 0;
}

int WebServer::StandardOutput()
{ MacroRegisterFunctionWithName("WebServer::StandardOutput");
  this->StandardOutputPart1BeforeComputation();
  if (theParser.inputString!="")
  { theParser.Evaluate(theParser.inputString);
    onePredefinedCopyOfGlobalVariables.flagComputationComplete=true;
  }
  this->StandardOutputPart2AfterComputation();
  return 0;
}

void WebServer::StandardOutputPart1BeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::StandardOutputPart1BeforeComputation");
  onePredefinedCopyOfGlobalVariables.flagComputationComplete=false;
  CGI::functionCGIServerIgnoreUserAbort=&ignoreUserAbortSignal;

  stOutput << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>calculator version  " << __DATE__ << ", " << __TIME__ << "</title>";
  stOutput << "<script src=\"../jsmath/easy/load.js\"></script>\n</head>\n<body onload=\"checkCookie();\">\n";
  List<std::string> inputStrings, inputStringNames;
  CGI::ChopCGIInputStringToMultipleStrings(theParser.inputStringRawestOfTheRaw, inputStrings, inputStringNames);
  std::string& civilizedInput=theParser.inputString;
  if (inputStringNames.Contains("textInput"))
    civilizedInput= inputStrings[inputStringNames.GetIndex("textInput")];
  CGI::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  PredefinedStrings(civilizedInput);

  crash.userInputStringIfAvailable=civilizedInput;

  std::stringstream tempStreamXX;
  static_html4(tempStreamXX);

  stOutput << tempStreamXX.str();
  stOutput << "<table>\n <tr valign=\"top\">\n <td>";
  stOutput << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\""
  << theParser.theGlobalVariableS->DisplayNameCalculatorWithPath << "\">\n" ;
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    stOutput << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. <br>";
  stOutput << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\" onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {storeSettings(); "
  << " this.form.submit(); return false;}\" >";
  stOutput << civilizedInputSafish;
  stOutput << "</textarea>\n<br>\n";
  stOutput << "<input type=\"submit\" title=\"Shift+Enter=shortcut from input text box. \" name=\"buttonGo\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
  if (civilizedInput!="")
    stOutput << "<a href=\"" << theParser.theGlobalVariableS->DisplayNameCalculatorWithPath << "?" << theParser.inputStringRawestOfTheRaw << "\">Link to your input.</a>";
  stOutput << "\n</FORM>";
  stOutput << theParser.javaScriptDisplayingIndicator;
  //  stOutput << "<br>Number of lists created before evaluation: " << NumListsCreated;
}

void WebServer::StandardOutputReturnIndicatorWaitForComputation()
{ MacroRegisterFunctionWithName("WebServer::StandardOutputReturnIndicatorWaitForComputation");
  std::stringstream out;
  out << " <!>\n";
  out << " <script type=\"text/javascript\"> \n";
  out << " var timeOutCounter=0;\n";
//  out << " var newReportString=\"\";\n";
  out << " var showProgress=false;";
  out << " function progressReport()\n";
  out << "{ var el = document.getElementById(\"idProgressReport\");	\n";
  out << "  if (!showProgress) \n";
  out << "  { el.style.display = 'none';\n";
  out << "    return;";
  out << "  }\n";
  out << "  el.style.display = '';\n";
//  out << "  el.contentWindow.location.reload();";
  out << "  timeOutCounter++;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  out << "  var sURL  = \"" << onePredefinedCopyOfGlobalVariables.DisplayNameIndicatorWithPath << "\";\n";
  out << "  oRequest.open(\"GET\",sURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  oRequest.send(null)\n";
  out << "  if (oRequest.status==200)\n";
  out << "  { newReportString= oRequest.responseText;\n";
  out << "    el.innerHTML= \"<hr>Refreshing each second. Client time: ~\"+ timeOutCounter+\" second(s)<br>\" +newReportString+\"<hr>\";\n";
  out << "  }\n";
  out << "   window.setTimeout(\"progressReport()\",1000);\n";
  out << " }\n";
  out << " </script>\n";
  out << "<br><div id=\"idProgressReport\">\n";
  out << " </div>\n";
  out << " \n";
  out << " \n";
  //////////////////
  out << "</td></tr></table></body></html>";
  stOutput << out.str();
}

void WebServer::StandardOutputPart2AfterComputation()
{ MacroRegisterFunctionWithName("WebServer::StandardOutputPart2AfterComputation");
  if (theParser.inputString!="")
  { if (theParser.flagProduceLatexLink)
      stOutput << "<br>LaTeX link (\\usepackage{hyperref}):<br> "
      << CGI::GetLatexEmbeddableLinkFromCalculatorInput(theParser.inputStringRawestOfTheRaw, theParser.inputString);
    stOutput << theParser.outputString;
    if (theParser.parsingLog!="")
      stOutput << "<b> As requested, here is a calculator parsing log</b><br>" << theParser.parsingLog;
  }
  stOutput << "\n\n</td>\n\n";
  bool displayClientMessage=theWebServer.flagUsingBuiltInServer;
  //displayClientMessage=false;
  if (theParser.outputCommentsString!="" || displayClientMessage)
  { stOutput << "<td valign=\"top\">";
    if (displayClientMessage)
      stOutput << "<b>Message from client: </b>" << theWebServer.theSocket.lastMessageReceived.ToStringFull() << "<hr>";
    //if (theParser.outputCommentsString.size()<10000)
    stOutput << theParser.outputCommentsString;
    //else
    //stOutput << "The comments generated by your computation are a bit too long. If you want to see them click the show/hide button below.<br>"
    //<< CGI::GetHtmlSpanHidableStartsHiddeN(theParser.outputCommentsString);
    stOutput << "</td>";
  }
  stOutput << "<td valign=\"top\">";
  ProjectInformation::GetMainProjectInfo().theFiles.QuickSortAscending();
  stOutput << ProjectInformation::GetMainProjectInfo().ToString();
  stOutput << "<hr><b>Calculator status. </b><br>";
  stOutput << theParser.ToString();

  stOutput << "</td></tr></table>";
  std::stringstream tempStream3;
  stOutput << "\n\n<script language=\"javascript\">\n// List of words to show in drop down\n var functionNameArray = new Array(";
  stOutput << ");\n  //var obj = actb(document.getElementById('textInputID'), functionNameArray);\n</script>\n";
  stOutput << "</body></html>";
  stOutput << "<!--";
  ProgressReport theReport(theParser.theGlobalVariableS);
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { std::stringstream out;
    out << "\n\ncommand: " << theParser.SystemCommands[i] << "\n" ;
    theReport.Report(out.str());
    system(theParser.SystemCommands[i].c_str());
  }
  stOutput << "-->";
  //close()
}
