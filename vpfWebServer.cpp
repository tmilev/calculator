//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

extern void static_html4(std::stringstream& output);
extern void static_html3(std::stringstream& output);

WebServer theWebServer;

std::string consoleBlue(const std::string& input)
{ std::stringstream out ;
  out << "\e[1;34m" << input << "\e[0m";
  return out.str();
}

std::string consoleRed(const std::string& input)
{ std::stringstream out ;
  out << "\e[1;31m" << input << "\e[0m";
  return out.str();
}

const char* PORT ="8080";  // the port users will be connecting to
const int BACKLOG =10;     // how many pending connections queue will hold

void Signal_SIGINT_handler(int s)
{ std::cout << "Signal interrupt handler called with input: " << s << "." << std::endl;
  while(waitpid(-1, NULL, WNOHANG) > 0)
  { }
  exit(0);
}

void Signal_SIGCHLD_handler(int s)
{ std::cout << "Signal child handler called with input: " << s << "." << std::endl;
//  while(waitpid(-1, NULL, WNOHANG) > 0)
//  { }
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string WebWorker::ToStringMessageShort(FormatExpressions* theFormat)const
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
  else if (this->requestType==this->requestTypeGetIndicator)
    out << "GET (indicator)";
  else
    out << "Request type undefined.";
  out << lineBreak << "<hr>Main address raw: " << this->mainAddresSRAW;
  out << lineBreak << "<br>Main address: " << this->mainAddress;
  out << lineBreak << "Main argument: " << this->mainArgumentRAW;
  out << lineBreak << "Physical file address referred to by main address: " << this->PhysicalFileName;
  return out.str();
}

std::string WebWorker::ToStringMessageFull()const
{ std::stringstream out;
  out << this->ToStringMessage();
  if (this->theStrings.size>0)
  { out << "<hr>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];
  }
  return out.str();
}

std::string WebWorker::ToStringMessage()const
{ std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  out << this->ToStringMessageShort(&tempFormat);

  out << "<hr>";
  out << "Main address RAW: " << this->mainAddresSRAW << "<br>";
  if (this->requestType==this->requestTypeGetCalculator ||
      this->requestType==this->requestTypeGetNotCalculator ||
      this->requestType==this->requestTypeGetIndicator)
    out << "GET " << this->mainAddresSRAW;
  if (requestType==this->requestTypePostCalculator)
    out << "POST " << this->mainAddresSRAW;
  out << "\n<br>\nFull client message:\n" << this->theMessage;
  return out.str();
}

void WebWorker::resetMessageComponenetsExceptRawMessage()
{ this->mainArgumentRAW="";
  this->mainAddress="";
  this->mainAddresSRAW="";
  this->PhysicalFileName="";
  this->theStrings.SetSize(0);
  this->requestType=this->requestTypeUnknown;
  this->ContentLength=-1;
}

void WebWorker::StandardOutputPart1BeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::StandardOutputPart1BeforeComputation");
  onePredefinedCopyOfGlobalVariables.flagComputationComplete=false;

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

void WebWorker::StandardOutputPart2AfterComputation()
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
  bool displayClientMessage=theWebServer.flagUsingBuiltInServer && theWebServer.activeWorker!=-1;
  //displayClientMessage=false;
  if (theParser.outputCommentsString!="" || displayClientMessage)
  { stOutput << "<td valign=\"top\">";
    if (displayClientMessage)
      stOutput << "<b>Message from client: </b>" << theWebServer.GetActiveWorker().ToStringMessageFull() << "<hr>";
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

void WebWorker::ExtractArgumentFromAddress()
{ MacroRegisterFunctionWithName("WebWorker::ExtractArgumentFromAddress");
//  std::cout << "\nmain address:" << this->mainAddress << "=?="
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size(): "
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size();
  CGI::CivilizedStringTranslationFromCGI(this->mainAddresSRAW, this->mainAddress);
  this->mainArgumentRAW="";
  std::string calculatorArgumentRawWithQuestionMark, tempS;
  if (!MathRoutines::StringBeginsWith(this->mainAddresSRAW, onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath, &calculatorArgumentRawWithQuestionMark))
    return;
  this->requestType=this->requestTypeGetCalculator;
  MathRoutines::SplitStringInTwo(calculatorArgumentRawWithQuestionMark, 1, tempS, this->mainArgumentRAW);
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "indicator"))
    this->requestType=this->requestTypeGetIndicator;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "status"))
    this->requestType=this->requestTypeGetServerStatus;
}

void WebWorker::ParseMessage()
{ MacroRegisterFunctionWithName("WebWorker::ParseMessage");
  this->resetMessageComponenetsExceptRawMessage();
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
          this->mainArgumentRAW=*this->theStrings.LastObject();
        else
          this->mainArgumentRAW="";
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

void WebWorker::QueueBytesForSending
  (const List<char>& bytesToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueBytesForSending");
  this->remainingBytesToSend.AddListOnTop(bytesToSend);
  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
    this->SendAllBytes();
}

void WebWorker::QueueStringForSending(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::SendStringToSocket");
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

void WebWorker::SendAllBytes()
{ if (this->remainingBytesToSend.size==0)
    return;
  MacroRegisterFunctionWithName("WebWorker::SendAllBytes");
  if (this->connectedSocketID==-1)
    crash << "Socket::SendAllBytes  called with connectedSocketID=-1, this shouldn't happen." << crash;
//  std::cout << "\r\nIn response to: " << this->theMessage;
  std::cout << "Sending " << this->remainingBytesToSend.size << " bytes in chunks of: " << std::endl;
  while (this->remainingBytesToSend.size>0)
  { int numBytesSent=send(this->connectedSocketID, &this->remainingBytesToSend[0], this->remainingBytesToSend.size,0);
    if (numBytesSent<0)
    { std::cout << "WebWorker::SendAllBytes failed. Error: " << this->parent->ToStringLastErrorDescription() << std::endl;
      return;
    }
    std::cout << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      std::cout << ", ";
    std::cout << std::endl;
  }
}

bool WebWorker::CheckConsistency()
{ stOutput.theOutputFunction=0;
  if (this->parent==0)
    crash << "Parent of web worker is not initialized." << crash;
  if (this->indexInParent==-1)
    crash << "Index in parent is bad. " << crash;
  if (this->connectionID==-1)
    crash << "Connection id is bad. " << crash;
  stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
  return true;
}

bool WebWorker::ReceiveAll()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAll");
  unsigned const int bufferSize=60000;
  char buffer[bufferSize];
  if (this->connectedSocketID==-1)
    crash << "Attempting to receive on a socket with ID equal to -1. " << crash;
  int numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
  if (numBytesInBuffer<0)
  { std::cout << "Socket::ReceiveAll on socket " << this->connectedSocketID << " failed. Error: "
    << this->parent->ToStringLastErrorDescription() << std::endl;
    return false;
  }
  this->theMessage.assign(buffer, numBytesInBuffer);
  std::cout << this->parent->ToStringStatusActive() << ": received " << numBytesInBuffer << " bytes. " << std::endl;
  this->ParseMessage();
//  std::cout << "Content length computed to be: " << this->ContentLength;
  if (this->ContentLength<=0)
    return true;
  if (this->mainArgumentRAW.size()==(unsigned) this->ContentLength)
    return true;
//  std::cout << "Content-length parsed to be: " << this->ContentLength
//  << "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  if (this->ContentLength>10000000)
  { error="Content-length parsed to be more than 10 million bytes, aborting.";
    std::cout << this->error << std::endl;
    return false;
  }
  if (this->mainArgumentRAW!="")
  { error= "Content-length does not coincide with the size of the message-body, yet the message-body is non-empty. Aborting.";
    std::cout << this->error << std::endl;
    return false;
  }
  this->remainingBytesToSend="HTTP/1.1 100 Continue\r\n";
  this->SendAllBytes();
  this->remainingBytesToSend.SetSize(0);
  this->mainArgumentRAW="";
  std::string bufferString;
  while ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      return false;
    }
    if (numBytesInBuffer<0)
    { std::cout << "Error fetching message body: " << this->parent->ToStringLastErrorDescription() << std::endl;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->mainArgumentRAW+=bufferString;
  }
  if ((signed) this->mainArgumentRAW.size()!=this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length " << this->mainArgumentRAW.size()
    << " yet I expected a message of length " << this->ContentLength << ".";
    this->error=out.str();
    std::cout << this->error << std::endl;
    return false;
  }
  return true;
}

void WebWorker::ExtractPhysicalAddressFromMainAddress()
{ MacroRegisterFunctionWithName("WebWorker::ExtractPhysicalAddressFromMainAddress");
  if (this->mainAddress.size()<onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size())
    return;
  this->PhysicalFileName=onePredefinedCopyOfGlobalVariables.PhysicalPathServerBase+
  this->mainAddress.substr(onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size(), std::string::npos);
}

int WebWorker::ProcessGetStatus()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetStatus");
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  stOutput << "<html><body> " << this->parent->ToStringStatusAll() << "</body></html>";
  return 0;
}

int WebWorker::ProcessGetRequestIndicator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestIndicator");
  std::cout << "Processing get request indicator." << std::endl;
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  if (this->mainArgumentRAW.size()<=9)
  { stOutput << "<b>Indicator takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerID= atoi(this->mainAddress.substr(9, std::string::npos).c_str());
  bool isGood=false;
  if (inputWebWorkerID<1 || inputWebWorkerID>this->parent->theWorkers.size)
    isGood=false;
  if (isGood)
    if (!this->parent->theWorkers[inputWebWorkerID-1].flagInUse)
      isGood=false;
  if (!isGood)
  { stOutput << "<b>Indicator requested child number " << inputWebWorkerID << " (out of "
    << this->parent->theWorkers.size << ") but the id is either not in use or out of range. </b>";
    return 0;
  }
  stOutput << "<b>Not implemented: request for indicator " << inputWebWorkerID << " out of "
  << this->parent->theWorkers.size << ".</b>";
  return 0;
}

void WebWorker::DisplayIndicator(const std::string& input)
{

}

int WebWorker::ProcessGetRequestFolder()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestFolder");
  std::stringstream out;
  out << "HTTP/1.1 200 OK\r\n"
  << "Content-Type: text/html\r\n\r\n"
  << "<html><body>";
  //out << this->ToString();
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNames(this->PhysicalFileName, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address " << this->PhysicalFileName
    << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->mainAddress << "<br>Physical address: " << this->PhysicalFileName << "<hr>";
  for (int i=0; i<theFileNames.size; i++)
  { bool isDir= theFileTypes[i]==".d";
    out << "<a href=\"" << this->mainAddress << theFileNames[i];
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

void WebWorker::reset()
{ this->connectedSocketID=-1;
  this->connectedSocketIDLastValueBeforeRelease=-1;
  this->connectionID=-1;
  this->indexInParent=-1;
  this->parent=0;
  this->requestType=this->requestTypeUnknown;
  this->pipeServerToWorker.initFillInObject(2, -1);
  this->pipeWorkerToServer.initFillInObject(2, -1);
}

WebWorker::WebWorker()
{ this->reset();
}

bool WebWorker::IamActive()
{ if (this->parent==0 || this->indexInParent==-1)
    return false;
  return this->parent->activeWorker==this->indexInParent;
}

bool WebWorker::IsFileExtensionOfBinaryFile(const std::string& fileExtension)
{ if (fileExtension==".png")
    return true;

  return false;
}

void WebWorker::SignalIamDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebWorker::SignalIamDoneReleaseEverything");
  if (!this->IamActive())
    return;
  std::cout << consoleRed("Worker: ") << this->indexInParent+1  << consoleRed(" is done with the work. ") << std::endl;
  this->parent->SendThroughPipe("close", this->pipeWorkerToServer, false);
  std::cout << consoleRed("Notification dispatched.") << std::endl;
  this->ReleaseResourcesMarkNotInUse();
}

WebWorker::~WebWorker()
{
}

std::string WebWorker::GetMIMEtypeFromFileExtension(const std::string& fileExtension)
{ MacroRegisterFunctionWithName("WebWorker::GetMIMEtypeFromFileExtension");
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

int WebWorker::ProcessGetRequestNonCalculator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestNonCalculator");
  this->ExtractPhysicalAddressFromMainAddress();
  //std::cout << this->ToStringShort() << "\r\n";
  if (FileOperations::IsFolder(this->PhysicalFileName))
    return this->ProcessGetRequestFolder();
  if (!FileOperations::FileExists(this->PhysicalFileName))
  { stOutput << "HTTP/1.1 404 Object not found\r\n";
    stOutput << "Content-Type: text/html\r\n";
    stOutput << "\r\n";
    stOutput << "<html><body>";
    stOutput << "<b>File does not exist.</b><br><b> File display name:</b> " << this->mainAddress
    << "<br><b>File physical name:</b> " << this->PhysicalFileName;
    stOutput << "<hr><hr><hr>Message details:<br>" <<  this->ToStringMessage();
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->PhysicalFileName);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFile(theFile, this->PhysicalFileName, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\r\n" << "Content-Type: text/html\r\n";
    stOutput << "\r\n"
    << "<html><body><b>Error: file appears to exist but I could not open it.</b> "
    << "<br><b> File display name: </b>"
    << this->mainAddress << "<br><b>File physical name: </b>"
    << this->PhysicalFileName << "</body></html>";
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
//  std::cout << "*****Message summary begin\r\n" << theHeader.str();
//  std::cout << "Sending file  " << this->PhysicalFileName; << " with file extension " << fileExtension
//  << ", file size: " << fileSize;
//  std::cout << "\r\n*****Message summary end\r\n";
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

int WebWorker::ProcessRequestTypeUnknown()
{ MacroRegisterFunctionWithName("WebWorker::ProcessRequestTypeUnknown");
  stOutput << "HTTP/1.1 501 Method Not Implemented\r\n";
  stOutput << "Content-Type: text/html\r\n";
  stOutput << "\r\n"
  << "<b>Requested method is not implemented. </b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessage();
  return 0;
}

int WebWorker::StandardOutput()
{ MacroRegisterFunctionWithName("WebServer::StandardOutput");
  WebWorker::StandardOutputPart1BeforeComputation();
  if (theParser.inputString!="")
  { theParser.Evaluate(theParser.inputString);
    onePredefinedCopyOfGlobalVariables.flagComputationComplete=true;
  }
  WebWorker::StandardOutputPart2AfterComputation();
  //exit(0);
  return 0;
}

int WebWorker::ServeClient()
{ MacroRegisterFunctionWithName("WebServer::ServeClient");
  if (this->requestType==this->requestTypeGetIndicator)
    this->parent->ReleaseNonActiveWorkers();
    //unless the worker is an indicator, it has no access to communication channels of the other workers
  if (this->requestType==this->requestTypeGetCalculator || this->requestType==this->requestTypePostCalculator)
  { stOutput << "HTTP/1.1 200 OK\n";
    stOutput << "Content-Type: text/html\r\n\r\n";
    theParser.inputStringRawestOfTheRaw=this->mainArgumentRAW;
    this->StandardOutput();
  } else if (this->requestType==this->requestTypeGetNotCalculator)
    this->ProcessGetRequestNonCalculator();
  else if (this->requestType==this->requestTypeGetIndicator)
  { this->ProcessGetRequestIndicator();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestTypeGetServerStatus)
    this->ProcessGetStatus();
  else if (this->requestType==this->requestTypeUnknown)
    this->ProcessRequestTypeUnknown();
  this->SignalIamDoneReleaseEverything();
  return 0;
}

void WebWorker::ReleaseResourcesKeepUseStatus()
{ MacroRegisterFunctionWithName("WebServer::ReleaseMyPipe");
  if (this->IamActive())
    this->SendAllBytes();
//  std::cout << "Releasing resources: " << this->ToStringStatus();
  WebServer::Release(this->pipeServerToWorker[0]);
  WebServer::Release(this->pipeServerToWorker[1]);
  WebServer::Release(this->pipeWorkerToServer[0]);
  WebServer::Release(this->pipeWorkerToServer[1]);
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::ReleaseResourcesMarkNotInUse()
{ this->ReleaseResourcesKeepUseStatus();
  this->flagInUse=false;
}

void WebWorker::StandardOutputReturnIndicatorWaitForComputation()
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
  out << "  var sURL  = \"" << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath << "?indicator"
  << this->indexInParent+1 << "\";\n";
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
  this->SendAllBytes();
  WebServer::Release(this->connectedSocketID);
}

std::string WebWorker::ToStringStatus()const
{ std::stringstream out;
  out << "Worker " << this->indexInParent+1;
  if (this->flagInUse)
    out << ", in use";
  else
    out << ", not in use";
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->ProcessPID!=0)
    out << this->ProcessPID;
  else
    out << "(not accessible)";
  out << ", socketID: ";
  if (this->connectedSocketID==-1)
    out << "released in current process, value before release: " << this->connectedSocketIDLastValueBeforeRelease;
  else
    out << this->connectedSocketID;
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

void WebServer::DisplayActiveIndicator(const std::string& input)
{ return theWebServer.GetActiveWorker().DisplayIndicator(input);
}

WebServer::~WebServer()
{ if (this->activeWorker!=-1)
    this->ReleaseActiveWorker();
  close(this->listeningSocketID);
}

void WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone()
{ theWebServer.GetActiveWorker().StandardOutputReturnIndicatorWaitForComputation();
}

void WebServer::FlushActiveWorker()
{ theWebServer.GetActiveWorker().SendAllBytes();
}

void WebServer::SendStringThroughActiveWorker(const std::string& theString)
{ theWebServer.GetActiveWorker().QueueStringForSending(theString, false);
}

WebServer::WebServer()
{ this->flagUsingBuiltInServer=false;
  this->activeWorker=-1;
}

WebWorker& WebServer::GetActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  stOutput.theOutputFunction=0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrput) web server
  //to report the error over the internet.
  if (this->activeWorker<0 || this->activeWorker>=this->theWorkers.size)
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;//<-the web server is in order,
  //therefore we restore the ability to report crashes over the internet.
  return this->theWorkers[this->activeWorker];
}

void WebServer::SignalActiveWorkerDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebServer::ReleaseActiveWorker");
  if (theWebServer.activeWorker==-1)
    return;
  theWebServer.GetActiveWorker().SignalIamDoneReleaseEverything();
  theWebServer.activeWorker=-1;
}

void WebServer::ReleaseActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::ReleaseActiveWorker");
  if (this->activeWorker==-1)
    return;
  this->GetActiveWorker().ReleaseResourcesMarkNotInUse();
  this->activeWorker=-1;
}

void WebServer::ReleaseNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->theWorkers[i].ReleaseResourcesKeepUseStatus();
}

void WebServer::CreateNewActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker!=-1)
  { crash << "Calling CreateNewActiveWorker requres the active worker index to be -1." << crash;
    return;
  }
  for (int i=0; i<this->theWorkers.size; i++)
    if (!this->theWorkers[i].flagInUse)
    { this->activeWorker=i;
      break;
    }
  if (this->activeWorker==-1)
  { this->activeWorker=this->theWorkers.size;
    this->theWorkers.SetSize(this->theWorkers.size+1);
  }
  this->GetActiveWorker().ReleaseResourcesMarkNotInUse();
  this->theWorkers[this->activeWorker].flagInUse=true;
  if (pipe(this->GetActiveWorker().pipeServerToWorker.TheObjects)<0)
    crash << "Failed to open pipe from child to parent. " << crash;
  if (pipe(this->GetActiveWorker().pipeWorkerToServer.TheObjects)<0)
    crash << "Failed to open pipe from parent to child. " << crash;
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().parent=this;
}

void WebServer::SendThroughPipe(const std::string& toBeSent, List<int>& outputPipe, bool doNotBlock)
{ MacroRegisterFunctionWithName("WebServer::SendThroughPipe");
  std::cout << consoleRed("Sending ") << toBeSent.size()
  << consoleRed(" bytes through pipe ") << outputPipe[1] << std::endl;
  if (outputPipe[1]==-1)
    return;
/*  if (doNotBlock)
    std::cout << ", NON-blocking." << std::endl;
  else
    std::cout << ", BLOCKING." << std::endl;*/
  if (doNotBlock)
    fcntl(outputPipe[1], F_SETFL, O_NONBLOCK);
  else
    fcntl(outputPipe[1], F_SETFL, 0);
  std::string toBeSentCopy=toBeSent;
  for (;;)
  { this->pipeBuffer.SetSize(4096); // <-once the buffer is resized, this operation does no memory allocation and is fast.
    int numBytesRemaining=toBeSentCopy.size();
    int numBytesWritten=write(outputPipe[1], toBeSentCopy.c_str(), toBeSentCopy.size());
    numBytesRemaining-=numBytesWritten;
    if (numBytesWritten<=0)
      return;
    if (numBytesRemaining<=0)
      return;
    toBeSentCopy= toBeSentCopy.substr(numBytesWritten, numBytesRemaining);
  }
}


void WebServer::ReadFromPipe(List<int>& inputPipe, bool doNotBlock)
{ MacroRegisterFunctionWithName("WebServer::ReadFromPipe");
  std::cout << consoleBlue("Reading from pipe: ") << inputPipe[0] << std::endl;
  if (doNotBlock)
    fcntl(inputPipe[0], F_SETFL, O_NONBLOCK);
  else
    fcntl(inputPipe[0], F_SETFL, 0);
  this->lastPipeReadResult.SetSize(0);
  for (;;)
  { this->pipeBuffer.SetSize(4096); // <-once the buffer is resized, this operation does no memory allocation and is fast.
    int numReadBytes =read(inputPipe[0], this->pipeBuffer.TheObjects, 4096);
    if (numReadBytes<=0)
      break;
//    std::cout << " " << numReadBytes << "read, ";
    this->pipeBuffer.SetSize(numReadBytes);
    this->lastPipeReadResult.AddListOnTop(this->pipeBuffer);
  }
}

std::string WebServer::ToStringLastErrorDescription()
{ std::stringstream out;
  out << this->ToStringStatusActive() << (strerror(errno)) << ". ";
  return out.str();
}

std::string WebServer::ToStringStatusActive()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusActive");
  if (this->activeWorker==-1)
    return "Server.";
  std::stringstream out;
  if (this->activeWorker!=this->GetActiveWorker().indexInParent)
    crash << "Bad index in parent!" << crash;
  out << this->GetActiveWorker().ToStringStatus();
  return out.str();
}

std::string WebServer::ToStringStatusAll()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusFull");
  std::stringstream out;
  if (this->activeWorker==-1)
    out << "The process is functioning as a server.";
  else
  { out << "The process is functioning as a worker. The active worker is number " << this->activeWorker+1 << ".";
    out << "<br>" << this->ToStringStatusActive();
  }
  out << "<hr>Total workers: " << this->theWorkers.size;
  for (int i=0; i<this->theWorkers.size; i++)
    out << "<br>" << this->theWorkers[i].ToStringStatus();
  return out.str();
}

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  addrinfo hints, *servinfo, *p;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  int yes=1;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
  { std::cout << "getaddrinfo: " << gai_strerror(rv) << std::endl;
    return 1;
  }
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next)
  { this->listeningSocketID= socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (this->listeningSocketID == -1)
    { stOutput << "Error: socket failed\n";
      continue;
    }
    if (setsockopt(this->listeningSocketID, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      crash << "Error: setsockopt failed.\n" << crash;
    if (bind(this->listeningSocketID, p->ai_addr, p->ai_addrlen) == -1)
    { close(this->listeningSocketID);
      std::cout << "Error: bind failed. " << this->ToStringLastErrorDescription() << std::endl;
      continue;
    }
    break;
  }
  if (p == NULL)
    crash << "Failed to bind to port " << PORT << "\n" << crash;
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(this->listeningSocketID, BACKLOG) == -1)
    crash << "Listen function failed." << crash;
  struct sigaction sa;
  sa.sa_handler = &Signal_SIGCHLD_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    std::cout << "sigaction returned -1" << std::endl;
  sa.sa_handler=&Signal_SIGINT_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1)
    std::cout << "sigaction returned -1" << std::endl;
  std::cout << "Server: waiting for connections...\r\n" << std::endl;
  unsigned int connectionsSoFar=0;
  while(true)
  { // main accept() loop
    sin_size = sizeof their_addr;
    int newConnectedSocket = accept(this->listeningSocketID, (struct sockaddr *)&their_addr, &sin_size);
    if (newConnectedSocket <0)
    { std::cout << "Accept failed. Error: " << this->ToStringLastErrorDescription() << std::endl;
      continue;
    }
    //Listen for children who have exited properly.
    //This might need to be rewritten: I wasn't able to make this work with any
    //mechanism other than pipes. This probably due to my lack of skill or UNIX's
    //crappy design (and is most likely due to both).
    for (int i=0; i<this->theWorkers.size; i++)
      if (this->theWorkers[i].flagInUse)
      { std::cout << "Reading from server: worker: " << i+1 << std::endl;
        this->ReadFromPipe(this->theWorkers[i].pipeWorkerToServer, true);
        std::string tempS;
        tempS.assign(this->lastPipeReadResult.TheObjects, this->lastPipeReadResult.size);
        std::cout << "String read: " << tempS << std::endl;
        if (tempS=="close" && this->lastPipeReadResult!= (std::string) "close")
          crash << "bad darned list" << crash;
        if (this->lastPipeReadResult==(std::string) "close")
          this->theWorkers[i].flagInUse=false;
      }
    this->CreateNewActiveWorker();
    this->GetActiveWorker().connectedSocketID=newConnectedSocket;
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease=newConnectedSocket;
    connectionsSoFar++;
    this->GetActiveWorker().connectionID=connectionsSoFar;
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddressBuffer, sizeof userAddressBuffer);
    this->GetActiveWorker().userAddress=userAddressBuffer;
    std::cout.flush();
//    std::cout << this->ToStringStatus();
    this->GetActiveWorker().ProcessPID=fork(); //creates an almost identical copy of this process.
    //The original process is the parent, the almost identical copy is the child.
    //std::cout << "\r\nChildPID: " << this->childPID;
    if (this->GetActiveWorker().ProcessPID!=0)
    { // this is the child (worker) process
      //releasing all resources worker does not need:
      this->Release(this->GetActiveWorker().pipeWorkerToServer[0]); //no access to read end
      this->Release(this->GetActiveWorker().pipeServerToWorker[1]); //no access to write end
      this->Release(this->listeningSocketID); //release socket listener
      InitializeTimer();
      CreateTimerThread();
      onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=5000;
      crash.CleanUpFunction=WebServer::SignalActiveWorkerDoneReleaseEverything;
      stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
      stOutput.flushOutputFunction=this->FlushActiveWorker;
      onePredefinedCopyOfGlobalVariables.ReturnIndicator=this->ReturnActiveIndicatorAlthoughComputationIsNotDone;
      std::cout << this->ToStringStatusActive() << std::endl;
      this->GetActiveWorker().CheckConsistency();
      if (!this->GetActiveWorker().ReceiveAll())
      { stOutput << "HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\n\r\n" << this->GetActiveWorker().error;
        return 0;
      }
      return 1;
    }
    this->Release(this->GetActiveWorker().pipeWorkerToServer[1]);//release write end
    this->Release(this->GetActiveWorker().pipeServerToWorker[0]);//release read end
    this->Release(this->GetActiveWorker().connectedSocketID);  //release socket:
    //the communication is handled by the worker
    this->activeWorker=-1; //The active worker is needed only in the child process.
  }
  return 0;
}

void WebServer::Release(int& theDescriptor)
{ close(theDescriptor);
  theDescriptor=-1;
}
