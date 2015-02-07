//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

extern void static_html4(std::stringstream& output);
extern void static_html3(std::stringstream& output);
const char* PORT ="8080";  // the port users will be connecting to
const int BACKLOG =10;     // how many pending connections queue will hold

class logger
{
  public:
  int currentColor;
  std::fstream theFile;
  bool flagStopWritingToFile;
  logger()
  { FileOperations::OpenFileCreateIfNotPresent(theFile, "./../output/LogFile.html", false, true, false);
    this->currentColor=logger::normalColor;
    this->flagStopWritingToFile=false;
  }
  void CheckLogSize()
  { theFile.seekg(0, std::ios::end);
    if (theFile.tellg()>500000)
      this->flagStopWritingToFile=true;
  }
  enum loggerSpecialSymbols{ endL, red, blue, yellow, green, purple, cyan, normalColor};
  std::string closeTagConsole()
  { return "\e[0m";
  }
  std::string closeTagHtml()
  { if (currentColor==logger::normalColor)
      return "";
    return "</span>";
  }
  std::string openTagConsole()
  { switch (this->currentColor)
    { case logger::red:
        return "\e[1;31m";
      case logger::green:
        return "\e[1;32m";
      case logger::yellow:
        return "\e[1;33m";
      case logger::blue:
        return "\e[1;34m";
      case logger::purple:
        return "\e[1;35m";
      case logger::cyan:
        return "\e[1;36m";
      default:
        return "";
    }
  }
  std::string openTagHtml()
  { switch (this->currentColor)
    { case logger::red:
        return "<span style=\"color:red\">";
      case logger::green:
        return "<span style=\"color:green\">";
      case logger::blue:
        return "<span style=\"color:blue\">";
      case logger::yellow:
        return "<span style=\"color:yellow\">";
      case logger::purple:
        return "<span style=\"color:purple\">";
      default:
        return "";
    }
  }
  logger& operator << (const loggerSpecialSymbols& input)
  { this->CheckLogSize();
    switch (input)
    { case logger::endL:
        if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
          std::cout << this->closeTagConsole() << std::endl;
        if (this->flagStopWritingToFile)
          return *this;
        theFile << this->closeTagHtml() << "\n<br>\n";
        theFile.flush();
        return *this;
      case logger::red:
      case logger::blue:
      case logger::yellow:
      case logger::green:
      case logger::purple:
      case logger::cyan:
        this->currentColor=input;
        if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
          std::cout << this->openTagConsole();
        if (this->flagStopWritingToFile)
          return *this;
        this->theFile << this->closeTagHtml();
        this->theFile << this->openTagHtml();
        return *this;
      default:
        return *this;
    }
  }
  template <typename theType>
  logger& operator << (const theType& toBePrinted)
  { if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
      std::cout << toBePrinted;
    this->CheckLogSize();
    if (this->flagStopWritingToFile)
      return *this;
    std::stringstream out;
    out << toBePrinted;
    theFile << out.str();
    return *this;
  }
};

logger theLog;
WebServer theWebServer;

ProgressReportWebServer::ProgressReportWebServer(const std::string& inputStatus)
{ this->flagDeallocated=false;
  this->indexProgressReport=theWebServer.theProgressReports.size;
  this->SetStatus(inputStatus);
}

ProgressReportWebServer::ProgressReportWebServer()
{ this->flagDeallocated=false;
  this->indexProgressReport=theWebServer.theProgressReports.size;
}

ProgressReportWebServer::~ProgressReportWebServer()
{ theWebServer.theProgressReports.SetSize(this->indexProgressReport);
  this->flagDeallocated=true;
}

void ProgressReportWebServer::SetStatus(const std::string& inputStatus)
{ MacroRegisterFunctionWithName("ProgressReportWebServer::SetStatus");
  theWebServer.CheckConsistency();
//  theLog << logger::endL << logger::red << "SetStatus: outputFunction: "
//  << (int) stOutput.theOutputFunction << logger::endL;
  MutexWrapper& safetyFirst=onePredefinedCopyOfGlobalVariables.MutexWebWorkerStaticFiasco;
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 2" << std::endl;
  safetyFirst.LockMe();
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 3" << std::endl;
  if (this->indexProgressReport>=theWebServer.theProgressReports.size)
    theWebServer.theProgressReports.SetSize(this->indexProgressReport+1);
  safetyFirst.UnlockMe();
//    std::cout << "SetStatus: passed mutex section" << std::endl;
  theWebServer.theProgressReports[this->indexProgressReport]=inputStatus;
  std::stringstream toBePiped;
  for (int i=0; i<theWebServer.theProgressReports.size; i++)
    toBePiped << "<br>" << theWebServer.theProgressReports[i];
  if (!onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    return;
  // theLog << logger::endL << logger::red << "SetStatus before the issue: outputFunction: "
  // << (int) stOutput.theOutputFunction << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToServerWorkerStatus.WriteAfterEmptying(toBePiped.str());
}

void PauseController::Release()
{ WebServer::Release(this->mutexPipe[0]);
  WebServer::Release(this->mutexPipe[1]);
  WebServer::Release(this->thePausePipe[0]);
  WebServer::Release(this->thePausePipe[1]);
}

PauseController::PauseController()
{ this->thePausePipe.initFillInObject(2,-1);
  this->mutexPipe.initFillInObject(2, -1);
}

void PauseController::CreateMe()
{ this->Release();
  this->buffer.SetSize(200);
  if (pipe(this->thePausePipe.TheObjects)<0)
    crash << "Failed to open pipe from parent to child. " << crash;
  if (pipe(this->mutexPipe.TheObjects)<0)
    crash << "Failed to open pipe from parent to child. " << crash;
  fcntl(this->thePausePipe[1], F_SETFL, O_NONBLOCK);
  write (this->thePausePipe[1], "!", 1);
  write (this->mutexPipe[1], "!", 1);
}

void PauseController::PauseIfRequested()
{ if (this->CheckPauseIsRequested())
    theLog << logger::red << "BLOCKING on " << this->ToString() << logger::endL;
  bool pauseWasRequested= !((read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size))>0);
  if (!pauseWasRequested)
    write(this->thePausePipe[1], "!", 1);
}

void PauseController::LockMe()
{ if (this->CheckPauseIsRequested())
    theLog << logger::red << "BLOCKING on pause controller " << this->ToString() << logger::endL;
  read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size);
}

bool PauseController::CheckPauseIsRequested()
{ read (this->mutexPipe[0], this->buffer.TheObjects, this->buffer.size);
  fcntl(this->thePausePipe[0], F_SETFL, O_NONBLOCK);
  bool result = !(read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size)>0);
  if (!result)
    write (this->thePausePipe[1], "!", 1);
  fcntl(this->thePausePipe[0], F_SETFL, 0);
  write (this->mutexPipe[1], "!", 1);
  return result;
}

void PauseController::UnlockMe()
{ MacroRegisterFunctionWithName("PauseController::UnlockMe");
//  theLog << crash.GetStackTraceShort() << logger::endL;
  write(this->thePausePipe[1], "!", 1);
//  theLog << "Unlocking done!" << logger::endL;

}

std::string PauseController::ToString()const
{ if (this->thePausePipe[0]==-1)
    return "(not in use)";
  std::stringstream out;
  out << "pause-controller pipe " << this->thePausePipe[0] << "<-" << this->thePausePipe[1];
  return out.str();
}

void WebServer::Signal_SIGINT_handler(int s)
{ theLog << "Signal interrupt handler called with input: " << s;
//  << ". Waiting for children to exit... " << logger::endL;
  theWebServer.ReleaseActiveWorker();
  theWebServer.ReleaseNonActiveWorkers();
  while(waitpid(-1, NULL, WNOHANG) > 0)
  { }
  theLog << "All children have exited. " << logger::endL;
  exit(0);
}

void WebServer::Signal_SIGCHLD_handler(int s)
{ theLog << "Signal child handler called with input: " << s << "." << logger::endL;
  while(waitpid(-1, NULL, WNOHANG) > 0)
  { }
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
  if (this->requestType==this->requestGetCalculator)
    out << "GET " << "(from calculator)";
  else if (this->requestType==this->requestPostCalculator)
    out << "POST " << "(from calculator)";
  else if (this->requestType==this->requestGetNotCalculator)
    out << "GET " << "(NOT from calculator)";
  else if (this->requestType==this->requestTogglePauseCalculator)
    out << "GET " << "(pause computation)";
  else if (this->requestType==this->requestGetServerStatus)
    out << "GET (server status)";
  else if (this->requestType==this->requestGetComputationIndicator)
    out << "GET (computation indicator)";
  else
    out << "Request type undefined.";
  out << "<hr>Main address raw: " << this->mainAddresSRAW;
  out << lineBreak << "Main address: " << this->mainAddress;
  out << lineBreak << "Main argument: " << this->mainArgumentRAW;
  out << lineBreak << "Physical file address referred to by main address: " << this->PhysicalFileName;
  out << lineBreak << "Display path server base: " << onePredefinedCopyOfGlobalVariables.DisplayPathServerBase;
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
  if (this->requestType==this->requestGetCalculator || this->requestType==this->requestGetNotCalculator ||
      this->requestType==this->requestGetServerStatus || this->requestType==this->requestGetComputationIndicator)
    out << "GET " << this->mainAddresSRAW;
  if (requestType==this->requestPostCalculator)
    out << "POST " << this->mainAddresSRAW;
  out << "\n<br>\nFull client message:\n" << this->theMessage;
  return out.str();
}

void WebWorker::resetMessageComponentsExceptRawMessage()
{ this->mainArgumentRAW="";
  this->mainAddress="";
  this->mainAddresSRAW="";
  this->PhysicalFileName="";
  this->displayUserInput="";
  this->theStrings.SetSize(0);
  this->requestType=this->requestUnknown;
  this->ContentLength=-1;
}

static std::stringstream standardOutputStreamAfterTimeout;
void WebWorker::StandardOutputAfterTimeOut(const std::string& input)
{ standardOutputStreamAfterTimeout << input;
  theLog << logger::endL << logger::green << " Standard output: " << standardOutputStreamAfterTimeout.str()
  << logger::endL;
}

void WebWorker::OutputBeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::OutputBeforeComputation");
  onePredefinedCopyOfGlobalVariables.flagComputationCompletE=false;

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

//  civilizedInput="\\int( 1/x dx)";
//  civilizedInput="\\int (1/(x(1+x^2)^2))dx";
//  civilizedInput="%LogEvaluation \\int (1/(5+2x+7x^2)^2)dx";
  crash.userInputStringRAWIfAvailable=theParser.inputStringRawestOfTheRaw;
  crash.userInputStringIfAvailable=civilizedInput;

  std::stringstream tempStreamXX;
  static_html4(tempStreamXX);

//stOutput << "Folders: " << theParser.theGlobalVariableS->ToStringFolderInfo();

  stOutput << tempStreamXX.str();
  stOutput << "<table>\n <tr valign=\"top\">\n ";
  stOutput << "<td>"; //<td> tag will be closed later in WebWorker::OutputStandardResult
  stOutput << "\n<FORM method=\"POST\" id=\"formCalculator\" name=\"formCalculator\" action=\""
  << theParser.theGlobalVariableS->DisplayNameCalculatorWithPath << "\">\n" ;
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    stOutput << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. <br>";
  stOutput << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\", style=\"white-space:normal\" "
  << "onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {storeSettings(); "
  << " this.form.submit(); return false;}\" "
  << "onkeyup=\"suggestWord();\", onkeydown=\"suggestWord(); arrowAction(event);\", onmouseup=\"suggestWord();\", oninput=\"suggestWord();\""
  << ">";
  stOutput << civilizedInputSafish;
  stOutput << "</textarea>\n<br>\n";
  stOutput << "<input type=\"submit\" title=\"Shift+Enter=shortcut from input text box. \" name=\"buttonGo\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
  if (civilizedInput!="")
    stOutput << "<a href=\"" << theParser.theGlobalVariableS->DisplayNameCalculatorWithPath << "?" << theParser.inputStringRawestOfTheRaw << "\">Link to your input.</a>";
  stOutput << "\n</FORM>";
  //  stOutput << "<br>Number of lists created before evaluation: " << NumListsCreated;
}

void WebWorker::OutputResultAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputResultAfterTimeout");
  std::stringstream out;
//  out << theParser.ToStringOutputSpecials();
  if (standardOutputStreamAfterTimeout.str().size()!=0)
    out << standardOutputStreamAfterTimeout.str() << "<hr>";
  out << "<table><tr><td>" << theParser.ToStringOutputAndSpecials() << "</td><td>"
  << theParser.outputCommentsString << "</td></tr></table>";
  std::stringstream outputTimeOutFileName;
  std::fstream outputTimeOutFile;
  std::string inputAbbreviated=theParser.inputStringRawestOfTheRaw;
  if (theParser.inputStringRawestOfTheRaw.size()>100)
    inputAbbreviated=inputAbbreviated.substr(0, 100)+ "_input_too_long_remainder_truncated";
  outputTimeOutFileName << theParser.theGlobalVariableS->PhysicalPathOutputFolder
  << inputAbbreviated << ".html";
  FileOperations::OpenFileCreateIfNotPresent(outputTimeOutFile, outputTimeOutFileName.str(), false, true, false);
  outputTimeOutFile << "<html><body>" << out.str() << "</body></html>";
  outputTimeOutFile.close();
  WebWorker::OutputSendAfterTimeout(out.str());
}

void WebWorker::OutputCrashAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputCrashAfterTimeout");
  theLog << logger::red << theWebServer.ToStringActiveWorker() << ": crashing AFTER timeout!" << logger::endL;
  WebWorker::OutputSendAfterTimeout(standardOutputStreamAfterTimeout.str());
  theWebServer.SignalActiveWorkerDoneReleaseEverything();
}

void WebWorker::OutputSendAfterTimeout(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::OutputSendAfterTimeout");
  onePredefinedCopyOfGlobalVariables.flagTimedOutComputationIsDone=true;
  theLog << "WebWorker::StandardOutputPart2ComputationTimeout called with worker number " << theWebServer.GetActiveWorker().indexInParent+1
  << "." << logger::endL;
  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.LockMe();
  theLog << logger::red << "Sending result through indicator pipe." << logger::endL;
  ProgressReportWebServer theReport("Sending result through indicator pipe.");

  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  theLog << logger::red << "Final output written to indicator, blocking until data is received on the other end." << logger::endL;
  theReport.SetStatus("Blocking until result data is received.");
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequested();

  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.LockMe();
  theLog << logger::red << "Result data is received, sending \"finished\"." << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying("finished");
  theReport.SetStatus(" \"finished\" sent through indicator pipe, waiting.");
  theLog << logger::red << "\"finished\" sent through indicator pipe, waiting." << logger::endL;
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequested();
}

void WebWorker::OutputStandardResult()
{ MacroRegisterFunctionWithName("WebServer::OutputStandardResult");
  stOutput << theParser.ToStringOutputAndSpecials();
  stOutput << "\n\n</td>\n\n";
  stOutput << "<td>";
  stOutput << "<table>"
  << "<tr><td nowrap>"
  << "<span \"style:nowrap\" id=\"idAutocompleteHints\">Ctrl+space autocompletes, ctrl+arrow selects word</span>"
  << "</td></tr>"
  << "<tr>"
  << "<td id=\"idAutocompleteSpan\"></td>"
  << "</tr>"
  << "<tr>"
  << "<td id=\"idAutocompleteDebug\"></td>"
  << "</tr>"
  << "</table>";
  stOutput << "<script type=\"text/javascript\" src=\"../autocomplete.js\"></script>\n";
  stOutput << "</td>";
//  bool displayClientMessage=theWebServer.flagUsingBuiltInServer && theWebServer.activeWorker!=-1;
  //displayClientMessage=false;
  if (theParser.outputCommentsString!="")
  { stOutput << "<td valign=\"top\">";
//    if (displayClientMessage)
//      stOutput << "<b>Message from client: </b>" << theWebServer.GetActiveWorker().ToStringMessageFull() << "<hr>";
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
  stOutput << "\n\n<script language=\"javascript\">\n"
  << "  var theAutocompleteDictionary = new Array;\n  ";
  for (int i=0; i<theParser.theAtoms.size; i++)
    if (theParser.theAtoms[i].size()>2)
      stOutput << "  theAutocompleteDictionary.push(\"" << theParser.theAtoms[i] << "\");\n";
  stOutput << "</script>\n";
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
//  theLog << "\nmain address:" << this->mainAddress << "=?="
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size(): "
//  << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size();
  CGI::CivilizedStringTranslationFromCGI(this->mainAddresSRAW, this->mainAddress);
  this->mainArgumentRAW="";
  std::string calculatorArgumentRawWithQuestionMark, tempS;
  if (!MathRoutines::StringBeginsWith(this->mainAddresSRAW, onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath, &calculatorArgumentRawWithQuestionMark))
    return;
  this->requestType=this->requestGetCalculator;
  MathRoutines::SplitStringInTwo(calculatorArgumentRawWithQuestionMark, 1, tempS, this->mainArgumentRAW);
  theLog << logger::yellow << "this->mainArgumentRAW=" << this->mainArgumentRAW << logger::endL;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "monitor"))
    this->requestType=this->requestGetMonitor;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "indicator"))
    this->requestType=this->requestGetComputationIndicator;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "status"))
    this->requestType=this->requestGetServerStatus;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "pauseIndicator"))
    this->requestType=this->requestTogglePauseCalculator;
}

void WebWorker::ParseMessage()
{ MacroRegisterFunctionWithName("WebWorker::ParseMessage");
  //std::cout << "Got thus far 13" << std::endl;
  this->resetMessageComponentsExceptRawMessage();
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
    { this->requestType=this->requestGetNotCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddresSRAW=this->theStrings[i];
        this->ExtractArgumentFromAddress();
      }
    } else if (this->theStrings[i]=="POST")
    { this->requestType=this->requestPostCalculator;
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
          if (!theLI.IsIntegerFittingInInt(&this->ContentLength))
            this->ContentLength=-1;
      }
    }
//  std::cout << "Got thus far 14" << std::endl;

}

void WebWorker::QueueBytesForSending(const List<char>& bytesToSend, bool MustSendAll)
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
//      theLog << "WARNING: string is null terminated!\r\n";
//    theLog << "Last character string: " << *this->remainingBytesToSend.LastObject() << "\r\n";
  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
    this->SendAllBytes();
}

void WebWorker::SendAllBytes()
{ if (this->remainingBytesToSend.size==0)
    return;
  MacroRegisterFunctionWithName("WebWorker::SendAllBytes");
  this->CheckConsistency();
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker::SendAllBytes");
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    theReport.SetStatus("WebWorker::SendAllBytes - continue ...");
    return;
  }
  theLog << "Sending " << this->remainingBytesToSend.size << " bytes in chunks of: " << logger::endL;
  //  theLog << "\r\nIn response to: " << this->theMessage;
  while (this->remainingBytesToSend.size>0)
  { int numBytesSent=send(this->connectedSocketID, &this->remainingBytesToSend[0], this->remainingBytesToSend.size,0);
    if (numBytesSent<0)
    { theLog << "WebWorker::SendAllBytes failed. Error: " << this->parent->ToStringLastErrorDescription() << logger::endL;
      theReport.SetStatus("WebWorker::SendAllBytes - continue ...");
      return;
    }
    theLog << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      theLog << ", ";
    theLog << logger::endL;
  }
  theReport.SetStatus("WebWorker::SendAllBytes - continue ...");
}

bool WebWorker::CheckConsistency()
{ stOutput.theOutputFunction=0;
  if (this->flagDeallocated)
    crash << "Use after free of webworker." << crash;
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
//  std::cout << "Got thus far 9" << std::endl;

  if (this->connectedSocketID==-1)
    crash << "Attempting to receive on a socket with ID equal to -1. " << crash;
//  std::cout << "Got thus far 10" << std::endl;
  int numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
//  std::cout << "Got thus far 11" << std::endl;
  if (numBytesInBuffer<0 || numBytesInBuffer>(signed)bufferSize)
  { std::stringstream out;
    out << "Socket::ReceiveAll on socket " << this->connectedSocketID << " failed. Error: "
    << this->parent->ToStringLastErrorDescription();
    this->displayUserInput=out.str();
    theLog << out.str() << logger::endL;
    return false;
  }
  this->theMessage.assign(buffer, numBytesInBuffer);
//  theLog << this->parent->ToStringStatusActive() << ": received " << numBytesInBuffer << " bytes. " << logger::endL;
  this->ParseMessage();
//    std::cout << "Got thus far 12" << std::endl;

//  theLog << "Content length computed to be: " << this->ContentLength;
  if (this->requestType==this->requestTypes::requestPostCalculator)
    this->displayUserInput="POST " + this->mainArgumentRAW;
  else
    this->displayUserInput="GET " + this->mainAddresSRAW;
  if (this->ContentLength<=0)
    return true;
  if (this->mainArgumentRAW.size()==(unsigned) this->ContentLength)
    return true;
//  theLog << "Content-length parsed to be: " << this->ContentLength
//  << "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  if (this->ContentLength>10000000)
  { this->CheckConsistency();
    error="Content-length parsed to be more than 10 million bytes, aborting.";
    theLog << this->error << logger::endL;
    this->displayUserInput=this->error;
    return false;
  }
  std::cout << "Debug code here!!!";
/*  if (this->mainArgumentRAW!="")
  { std::stringstream errorstream;
    errorstream << "Content-length equals: " << this->ContentLength
    << " and does not coincide with the size of the message-body (total " << this->mainArgumentRAW.size()
    << "bytes), yet the message-body is non-empty. ";
    this->error=errorstream.str();
    theLog << this->error << logger::endL;
    theLog << logger::red << "The messaged received was: " << logger::endL;
    theLog << this->mainArgumentRAW;
    this->displayUserInput=this->error;
    return false;
  }*/
  this->remainingBytesToSend=(std::string) "HTTP/1.1 100 Continue\r\n";
  this->SendAllBytes();
  this->remainingBytesToSend.SetSize(0);
  std::string bufferString;
  while ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput=this->error;
      return false;
    }
    if (numBytesInBuffer<0)
    { this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      theLog << this->error << logger::endL;
      this->displayUserInput=this->error;
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
    this->displayUserInput=this->error;
    theLog << this->error << logger::endL;
    return false;
  }
  return true;
}

void WebWorker::SendDisplayUserInputToServer()
{ MacroRegisterFunctionWithName("WebWorker::SendDisplayUserInputToServer");
  if (this->displayUserInput.size()>3000)
    this->displayUserInput.resize(3000);
  this->pipeWorkerToServerUserInput.WriteAfterEmptying(this->displayUserInput);
  theLog << logger::blue << "Piping " << this->displayUserInput << " to the server. " << logger::endL;
}

void WebWorker::ExtractPhysicalAddressFromMainAddress()
{ MacroRegisterFunctionWithName("WebWorker::ExtractPhysicalAddressFromMainAddress");
  int numBytesToChop=onePredefinedCopyOfGlobalVariables.DisplayPathServerBase.size();
  std::string displayAddressStart= this->mainAddress.substr(0, numBytesToChop);
  if (displayAddressStart!=onePredefinedCopyOfGlobalVariables.DisplayPathServerBase)
    numBytesToChop=0;
  this->PhysicalFileName=onePredefinedCopyOfGlobalVariables.PhysicalPathServerBase+
  this->mainAddress.substr(numBytesToChop, std::string::npos);
}

int WebWorker::ProcessServerStatus()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestServerStatus");
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  stOutput << "<html><body> " << this->parent->ToStringStatusAll() << "</body></html>";
  return 0;
}

int WebWorker::ProcessPauseWorker()
{ MacroRegisterFunctionWithName("WebWorker::ProcessPauseWorker");
  theLog << "Proceeding to toggle worker pause." << logger::endL;
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  if (this->mainArgumentRAW.size()<=14)
  { stOutput << "<b>Indicator takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(this->mainArgumentRAW.substr(14, std::string::npos).c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>User requested " << this->mainArgumentRAW << " (worker number " << inputWebWorkerNumber
    << " out of " << this->parent->theWorkers.size << ") but the worker number is out of range. </b>";
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. Total number of workers: "
    << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  WebWorker& otherWorker=this->parent->theWorkers[inputWebWorkerIndex];
//  if (onePredefinedCopyOfGlobalVariables.flagLogInterProcessCommunication)
//  theLog << "About to read pipeServerToWorker..." << logger::endL;
  if (!otherWorker.PauseWorker.CheckPauseIsRequested())
  { otherWorker.PauseWorker.LockMe();
    stOutput << "paused";
    return 0;
  }
  stOutput << "unpaused";
  otherWorker.PauseWorker.UnlockMe();
  return 0;
}

int WebWorker::ProcessMonitor()
{ MacroRegisterFunctionWithName("WebWorker::ProcessMonitor");
  theLog << "Processing get monitor." << logger::endL;
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  if (this->mainArgumentRAW.size()<=7)
  { stOutput << "<b>Monitor takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(this->mainArgumentRAW.substr(7, std::string::npos).c_str());
  stOutput << "<html><body>" << this->GetJavaScriptIndicatorBuiltInServer(inputWebWorkerNumber-1)
  << "</body></html>";
  return 0;
}

int WebWorker::ProcessComputationIndicator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicator");
  theLog << "Processing get request indicator." << logger::endL;
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  ProgressReportWebServer theReport("Preparing indicator report");
  if (this->mainArgumentRAW.size()<=9)
  { stOutput << "<b>Indicator takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(this->mainArgumentRAW.substr(9, std::string::npos).c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>Indicator requested " << this->mainArgumentRAW << " (extracted worker number "
    << inputWebWorkerNumber << " out of " << this->parent->theWorkers.size << ") but the id is out of range. </b>";
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. "
    << "Total number of workers: " << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  WebWorker& otherWorker=this->parent->theWorkers[inputWebWorkerIndex];
//  theLog << "Worker " << this->parent->activeWorker
//  << consoleYellow(" piping 'indicator'" ) << logger::endL;
  otherWorker.pipeServerToWorkerRequestIndicator.WriteAfterEmptying("!");
//  theLog << "'indicator' piped, waiting for return." << logger::endL;
  otherWorker.pipeWorkerToServerIndicatorData.Read();
//  theLog << "indicator returned." << logger::endL;
  if (otherWorker.pipeWorkerToServerIndicatorData.lastRead.size>0)
  { std::string outputString;
    outputString.assign(otherWorker.pipeWorkerToServerIndicatorData.lastRead.TheObjects, otherWorker.pipeWorkerToServerIndicatorData.lastRead.size);
    //theLog << logger::yellow << "Indicator string read: " << logger::blue << outputString << logger::endL;
    stOutput << outputString;
    otherWorker.PauseComputationReportReceived.UnlockMe();
  }
//  stOutput << "<b>Not implemented: request for indicator for worker " << inputWebWorkerNumber
//  << " out of " << this->parent->theWorkers.size << ".</b>";
  return 0;
}

void WebWorker::PipeProgressReportToParentProcess(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::PipeProgressReportToParentProcess");
  static int counter=0;
  counter++;
  std::stringstream debugStream1, debugStream2;
  debugStream1 << "PipeProgressReportToParentProcess called " << counter << " times. Calling pause...";
  ProgressReportWebServer theReport(debugStream1.str());
  this->PauseIndicatorPipeInUse.LockMe();
//    theLog << "about to potentially block " << logger::endL;
  debugStream2 << "PipeProgressReportToParentProcess called " << counter << " times. Pause passed...";
  theReport.SetStatus(debugStream2.str());
  if (this->PauseWorker.CheckPauseIsRequested())
    theReport.SetStatus("PipeProgressReportToParentProcess: pausing as requested...");

  this->PauseWorker.PauseIfRequested();     //if pause was requested, here we block
//    theLog << "(possible) block passed" << logger::endL;
  theReport.SetStatus("PipeProgressReportToParentProcess: computing...");
  this->pipeServerToWorkerRequestIndicator.Read();
  if (this->pipeServerToWorkerRequestIndicator.lastRead.size==0)
  { this->PauseIndicatorPipeInUse.UnlockMe();
    return;
  }
  if (onePredefinedCopyOfGlobalVariables.flagTimedOutComputationIsDone)
  { this->PauseIndicatorPipeInUse.UnlockMe();
    return;
  }
//  if (onePredefinedCopyOfGlobalVariables.flagLogInterProcessCommunication)
//  theLog << " data written!";
  theReport.SetStatus("PipeProgressReportToParentProcess: piping computation process...");
  this->pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  theReport.SetStatus("PipeProgressReportToParentProcess: exiting 1...");
  this->PauseIndicatorPipeInUse.UnlockMe();
  theReport.SetStatus("PipeProgressReportToParentProcess: exiting 2...");
}

int WebWorker::ProcessFolder()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  std::stringstream out;
  out << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << "<html><body>";
  //out << this->ToString();
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNames(this->PhysicalFileName, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address " << this->PhysicalFileName << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->mainAddress << "<br>Physical address: " << this->PhysicalFileName << "<hr>";
  List<std::string> theFolderNamesHtml, theFileNamesHtml;
  for (int i=0; i<theFileNames.size; i++)
  { std::stringstream currentStream;
    bool isDir= theFileTypes[i]==".d";
    currentStream << "<a href=\"" << this->mainAddress << theFileNames[i];
    if (isDir)
      currentStream << "/";
    currentStream << "\">" << theFileNames[i];
    if (isDir)
      currentStream << "/";
    currentStream << "</a><br>";
    if (isDir)
      theFolderNamesHtml.AddOnTop(currentStream.str());
    else
      theFileNamesHtml.AddOnTop(currentStream.str());
  }
  theFolderNamesHtml.QuickSortAscending();
  theFileNamesHtml.QuickSortAscending();
  for (int i =0; i< theFolderNamesHtml.size; i++)
    out << theFolderNamesHtml[i];
  for (int i =0; i< theFileNamesHtml.size; i++)
    out << theFileNamesHtml[i];
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
  this->displayUserInput="";
  this->requestType=this->requestUnknown;
  this->Release();
}

WebWorker::WebWorker()
{ this->flagDeallocated=false;
  this->reset();
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
  { theLog << logger::red << "Signal done called on non-active worker" << logger::endL;
    return;
  }
  theLog << logger::blue << "worker " << this->indexInParent+1 << " is done with the work. " << logger::endL;
//  std::cout << "got thus far xxxxxxx" << std::endl;
  this->pipeWorkerToServerControls.WriteAfterEmptying("close");
  theLog << logger::blue << "Notification dispatched." << logger::endL;
  this->SendAllBytes();
  this->Release();
}

WebWorker::~WebWorker()
{ MacroRegisterFunctionWithName("WebWorker::~WebWorker");
  //Workers are not allowed to release resources in the destructor:
  //a Worker's destructor is called when expanding List<WebWorker>.
  this->flagDeallocated=true;
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
  if (fileExtension==".ico")
    return "Content-Type: image/x-icon\r\n";
  return "Content-Type: application/octet-stream\r\n";
}

int WebWorker::ProcessNonCalculator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessNonCalculator");
  this->ExtractPhysicalAddressFromMainAddress();
  //theLog << this->ToStringShort() << "\r\n";
  if (FileOperations::IsFolder(this->PhysicalFileName))
    return this->ProcessFolder();
  if (!FileOperations::FileExists(this->PhysicalFileName))
  { stOutput << "HTTP/1.1 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>File does not exist.</b><br><b> File display name:</b> "
    << this->mainAddress << "<br><b>File physical name:</b> " << this->PhysicalFileName;
    stOutput << "<hr><hr><hr>Message details:<br>" <<  this->ToStringMessage();
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->PhysicalFileName);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFile(theFile, this->PhysicalFileName, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> "
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
//  theLog << "*****Message summary begin\r\n" << theHeader.str();
//  theLog << "Sending file  " << this->PhysicalFileName; << " with file extension " << fileExtension
//  << ", file size: " << fileSize;
//  theLog << "\r\n*****Message summary end\r\n";
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

int WebWorker::ProcessUnknown()
{ MacroRegisterFunctionWithName("WebWorker::ProcessUnknown");
  stOutput << "HTTP/1.1 501 Method Not Implemented\r\nContent-Type: text/html\r\n\r\n";
  stOutput << "<b>Requested method is not implemented. </b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessage();
  return 0;
}

int WebWorker::OutputWeb()
{ MacroRegisterFunctionWithName("WebServer::OutputWeb");
  WebWorker::OutputBeforeComputation();
  theWebServer.CheckExecutableVersionAndRestartIfNeeded();
  stOutput << theParser.javaScriptDisplayingIndicator;
  //theParser.inputString="TestCalculatorIndicator 0";
  //theParser.inputString="printSemisimpleSubalgebrasRecompute(B_3)";
  ProgressReportWebServer theReport;
  if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computing...");
  if (theParser.inputString!="")
    theParser.Evaluate(theParser.inputString);
  if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computation complete, preparing output");
  onePredefinedCopyOfGlobalVariables.flagComputationCompletE=true;
  if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    if (onePredefinedCopyOfGlobalVariables.flagOutputTimedOut)
    { theReport.SetStatus("OutputWeb: calling OutputResultAfterTimeout.");
      WebWorker::OutputResultAfterTimeout();
      theReport.SetStatus("OutputWeb: called OutputResultAfterTimeout, flushing.");
      stOutput.Flush();
      theReport.SetStatus("OutputWeb: called OutputResultAfterTimeout, exiting.");
      return 0;
    }
  theReport.SetStatus("OutputWeb:  calling OutputStandardResult");
  WebWorker::OutputStandardResult();
  theReport.SetStatus("OutputWeb: called OutputStandardResult, flushing.");
  stOutput.Flush();
  theReport.SetStatus("OutputWeb: called OutputStandardResult, exiting...");
  return 0;
}

std::string WebWorker::GetJavaScriptIndicatorFromHD()
{ std::stringstream out;
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
  out << CGI::GetHtmlButton
  ("progressReportButton", "showProgress=!showProgress; progressReport()", "expand/collapse progress report");
  out << "<br><div "
  //<< "src=\"" << this->indicatorFileNameDisplaY << "\" "
  << "id=\"idProgressReport\" style=\"display:none\">\n";
  out << " </div>\n";
  out << " \n";
  out << " \n";
  return out.str();
}

std::string WebWorker::GetJavaScriptIndicatorBuiltInServer(int inputIndex)
{ MacroRegisterFunctionWithName("WebWorker::GetJavaScriptIndicatorBuiltInServer");
  std::stringstream out;
  out << " <!>\n";
//  out << "\n<br>\n<button onclick=\"progressReport()\">Manual report</button>";
  out << "\n<br>\n<button id=\"idButtonSendTogglePauseRequest\" onclick=\"SendTogglePauseRequest()\">Pause</button>";
  out << "<span id=\"idPauseToggleServerResponse\"></span>\n";
  out << "<span id=\"idProgressReportTimer\"></span>\n";
  out << "\n<br>\n<div id=\"idProgressReport\"></div>\n";
  out << "\n<script type=\"text/javascript\"> \n";
  out << "var isPaused=false;\n";
  out << "var isFinished=false;\n";
  out << "var timeIncrementInTenthsOfSecond=4;//measured in tenths of a second\n";
  out << "var timeOutCounter=0;//measured in tenths of a second\n";
  out << "progressReport();\n";
  out << "function SendTogglePauseRequest()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var pauseRequest = new XMLHttpRequest();\n";
  theLog << "Generating indicator address for worker number " << inputIndex+1 << "." << logger::endL;
  out << "  pauseURL  = \"" << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath
  << "?pauseIndicator" << inputIndex+1 << "\";\n";
  out << "  pauseRequest.open(\"GET\",pauseURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  pauseRequest.send(null)\n";
  out << "  if (pauseRequest.status!=200)\n";
  out << "    return;\n";
  out << "  if(pauseRequest.responseText==\"paused\")\n";
  out << "    isPaused=true;\n";
  out << "  if(pauseRequest.responseText==\"unpaused\")\n";
  out << "    isPaused=false;\n";
  out << "  if (isPaused)\n";
  out << "    document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Continue\";\n";
  out << "  else\n";
  out << "    document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Pause\";\n";
  out << "  document.getElementById(\"idPauseToggleServerResponse\").innerHTML=pauseRequest.responseText;\n";
  out << "  if (!isPaused)\n";
  out << "    progressReport();\n";
  out << "}\n";
//  out << " progressReport();";
//  out << " var newReportString=\"\";\n";
  out << "\nfunction progressReport()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var progReport = document.getElementById(\"idProgressReport\");	\n";
  out << "  var progReportTimer = document.getElementById(\"idProgressReportTimer\");	\n";
  out << "  if(isPaused)\n";
  out << "    return;\n";
  out << "  progReportTimer.innerHTML =\"<hr>Refreshing every \"+timeIncrementInTenthsOfSecond/10+\" second(s). Client time: ~\"+ Math.floor(timeOutCounter/10)+\" second(s)<br>\";\n";
  out << "  progReportTimer.style.display = '';\n";
  out << "  progReport.style.display = '';\n";
//  out << "  el.contentWindow.location.reload();";
  out << "  timeOutCounter+=timeIncrementInTenthsOfSecond;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  if (inputIndex==-1)
    theLog << logger::red << "Worker index in parent is -1!!!" << logger::endL;
  else
    theLog << "Worker index: " << inputIndex << logger::endL;
  out << "  var sURL  = \"" << onePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath << "?indicator"
  << inputIndex+1 << "\";\n";
  out << "  oRequest.open(\"GET\",sURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  oRequest.send(null)\n";
  out << "  if (oRequest.status==200)\n";
  out << "  { newReportString= oRequest.responseText;\n";
  out << "    if (oRequest.responseText==\"finished\")\n";
  out << "    { isFinished=true;\n";
  out << "      document.getElementById(\"idPauseToggleServerResponse\").innerHTML=\"Computation finished.\";\n";
  out << "      return;\n";
  out << "    }\n";
  out << "    if (oRequest.responseText!=\"\")\n";
  out << "      progReport.innerHTML=newReportString+\"<hr>\";\n";
  out << "  }\n";
  out << "   window.setTimeout(\"progressReport()\",timeIncrementInTenthsOfSecond*100);\n";
  out << " }\n";
  out << " </script>\n";
  out << " \n";
  out << " \n";
  return out.str();
}

int WebWorker::ServeClient()
{ MacroRegisterFunctionWithName("WebServer::ServeClient");
  onePredefinedCopyOfGlobalVariables.flagComputationStarted=true;
  onePredefinedCopyOfGlobalVariables.IndicatorStringOutputFunction=WebServer::PipeProgressReportToParentProcess;
  if (this->requestType!=this->requestGetComputationIndicator &&
      this->requestType!=this->requestGetServerStatus &&
      this->requestType!=this->requestTogglePauseCalculator &&
      this->requestType!=this->requestGetMonitor)
    this->parent->ReleaseNonActiveWorkers();
    //unless the worker is an indicator, it has no access to communication channels of the other workers
  if (this->requestType==this->requestGetCalculator || this->requestType==this->requestPostCalculator)
  { stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    theParser.inputStringRawestOfTheRaw=this->mainArgumentRAW;
//    theParser.javaScriptDisplayingIndicator=this->GetJavaScriptIndicatorBuiltInServer(true);
    theParser.javaScriptDisplayingIndicator="";
    this->OutputWeb();
  } else if (this->requestType==this->requestGetNotCalculator)
    this->ProcessNonCalculator();
  else if (this->requestType==this->requestTogglePauseCalculator)
    this->ProcessPauseWorker();
  else if (this->requestType==this->requestGetServerStatus)
  { this->ProcessServerStatus();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestGetComputationIndicator)
  { this->ProcessComputationIndicator();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestGetMonitor)
  { this->ProcessMonitor();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestUnknown)
    this->ProcessUnknown();
  this->SignalIamDoneReleaseEverything();
  return 0;
}

void WebWorker::ReleaseKeepInUseFlag()
{ MacroRegisterFunctionWithName("WebServer::ReleaseMyPipe");
  this->pipeWorkerToServerControls.Release();
  this->pipeServerToWorkerRequestIndicator.Release();
  this->pipeWorkerToServerIndicatorData.Release();
  this->pipeWorkerToServerUserInput.Release();
  this->pipeWorkerToServerWorkerStatus.Release();
  this->PauseComputationReportReceived.Release();
  this->PauseWorker.Release();
  this->PauseIndicatorPipeInUse.Release();
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release()
{ this->ReleaseKeepInUseFlag();
  this->flagInUse=false;
}

void WebWorker::OutputShowIndicatorOnTimeout()
{ MacroRegisterFunctionWithName("WebServer::OutputShowIndicatorOnTimeout");
  this->PauseIndicatorPipeInUse.LockMe();
  onePredefinedCopyOfGlobalVariables.flagOutputTimedOut=true;
  onePredefinedCopyOfGlobalVariables.flagTimedOutComputationIsDone=false;
  ProgressReportWebServer theReport("WebServer::OutputShowIndicatorOnTimeout");
  theLog << logger::blue << "Computation timeout, sending progress indicator instead of output. " << logger::endL;
  stOutput << "</td></tr>";
  if (onePredefinedCopyOfGlobalVariables.flagDisplayTimeOutExplanation)
    stOutput << "<tr><td>Your computation is taking more than " << onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction
    << " seconds.</td></tr>";
  stOutput << "<tr><td>A progress indicator, as reported by your current computation, is displayed below. "
  << "When done, your computation result will be displayed below. </td></tr>";
  stOutput << "<tr><td>" << this->GetJavaScriptIndicatorBuiltInServer(this->indexInParent) << "</td></tr>"
  << "</table></body></html>";

//  theLog << logger::red << "Indicator: sending all bytes" << logger::endL;
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: sending all bytes.");
  this->SendAllBytes();
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: all bytes sent.");
//  theLog << logger::blue << "Indicator: sending all bytes DONE" << logger::endL;
  for (int i=0; i<this->parent->theWorkers.size; i++)
    if (i!=this->indexInParent)
      this->parent->theWorkers[i].Release();
  //this->parent->Release(this->pipeServerToWorkerControls[0]);
  //this->parent->Release(this->pipeServerToWorkerControls[1]);
  //this->parent->Release(this->pipeWorkerToServerControls[0]);
  //this->parent->Release(this->pipeWorkerToServerControls[1]);
//  this->parent->Release(this->pipeServerToWorkerIndicator[0]);
//  this->parent->Release(this->pipeServerToWorkerIndicator[1]);
//  this->parent->Release(this->pipeWorkerToServerIndicator[0]);
//  this->parent->Release(this->pipeWorkerToServerIndicator[1]);
  this->parent->Release(this->connectedSocketID);
  //set flags properly:
  //we need to rewire the standard output and the crashing mechanism:
  crash.CleanUpFunction=WebWorker::OutputCrashAfterTimeout;
  //note that standard output cannot be rewired in the beginning of the function as we use the old stOutput
  stOutput.theOutputFunction=WebWorker::StandardOutputAfterTimeOut;
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: continuing computation.");
  this->PauseIndicatorPipeInUse.UnlockMe();
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: exiting function.");
//  this->SignalIamDoneReleaseEverything();
//  theLog << consoleGreen("Indicator: released everything and signalled end.") << logger::endL;
}

std::string WebWorker::ToStringStatus()const
{ std::stringstream out;
  out << "worker " << this->indexInParent+1;
  if (this->flagInUse)
  { if (this->parent->activeWorker==this->indexInParent)
      out << ", <span style=\"color:green\"><b>current process</b></span>";
    else
      out << ", <b>in use</b>";
    out << ", <a href=\"calculator?monitor" << this->indexInParent +1 << "\">monitor process "
    << this->indexInParent +1 << "</a>";
  } else
    out << ", not in use";
  if (this->displayUserInput!="")
    out << ", user input: <span style=\"color:blue\">" << this->displayUserInput << "</span>";
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
  out << ". ";
  if (this->status!="")
    out << "<br><span style=\"color:red\"><b> Status: " << this->status << "</b></span><br>";
  out << "Pipe indices: worker to server controls: " << this->pipeWorkerToServerControls.ToString()
  << ", server to worker request indicator: " << this->pipeServerToWorkerRequestIndicator.ToString()
  << ", worker to server indicator data: " << this->pipeWorkerToServerIndicatorData.ToString()
  << ", worker to server user input: " << this->pipeWorkerToServerUserInput.ToString()
  << ", server to worker:  " << this->PauseWorker.ToString()
  << ", server to worker computation report received: " << this->PauseComputationReportReceived.ToString()
  << ", server to worker indicator pipe in use: " << this->PauseIndicatorPipeInUse.ToString()
  ;
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of WebServer." << crash;
  return true;
}

WebServer::~WebServer()
{ if (this->activeWorker!=-1)
    this->GetActiveWorker().SendAllBytes();
  for (int i=0; i<this->theWorkers.size; i++)
    this->theWorkers[i].Release();
  this->activeWorker=-1;
  close(this->listeningSocketID);
  this->flagDeallocated=true;
}

void WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone()
{ //theLog << logger::red << ("Got THUS far") << logger::endL;
//  theLog << "here am i";
  theWebServer.GetActiveWorker().OutputShowIndicatorOnTimeout();
//  stOutput << "What the hell";
//  stOutput.Flush();
}

void WebServer::FlushActiveWorker()
{ theWebServer.GetActiveWorker().SendAllBytes();
}

void WebServer::SendStringThroughActiveWorker(const std::string& theString)
{ theWebServer.GetActiveWorker().QueueStringForSending(theString, false);
}

void WebServer::PipeProgressReportToParentProcess(const std::string& theString)
{ theWebServer.GetActiveWorker().PipeProgressReportToParentProcess(theString);
}

WebServer::WebServer()
{ this->flagDeallocated=false;
  this->flagTryToKillOlderProcesses=true;
  this->activeWorker=-1;
  this->timeLastExecutableModification=-1;
}

WebWorker& WebServer::GetActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  void (*oldOutputFunction)(const std::string& stringToOutput) =stOutput.theOutputFunction;
  stOutput.theOutputFunction=0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrput) web server
  //to report the error over the internet.
  if (this->activeWorker<0 || this->activeWorker>=this->theWorkers.size)
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  stOutput.theOutputFunction=oldOutputFunction;//<-the web server is in order,
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
  this->GetActiveWorker().Release();
  this->activeWorker=-1;
}

void WebServer::ReleaseNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->theWorkers[i].ReleaseKeepInUseFlag();
}

void WebServer::ReleaseSocketsNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseSocketsNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->Release(this->theWorkers[i].connectedSocketID);
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
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUse=true;

  this->GetActiveWorker().PauseComputationReportReceived.CreateMe();
  this->GetActiveWorker().PauseWorker.CreateMe();
  this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe();
  this->GetActiveWorker().pipeServerToWorkerRequestIndicator.CreateMe();
  this->GetActiveWorker().pipeWorkerToServerControls.CreateMe();
  this->GetActiveWorker().pipeWorkerToServerIndicatorData.CreateMe();
  this->GetActiveWorker().pipeWorkerToServerUserInput.CreateMe();
  this->GetActiveWorker().pipeWorkerToServerWorkerStatus.CreateMe();
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().parent=this;
}

void Pipe::WriteAfterEmptying(const std::string& toBeSent)
{ //theLog << "Step -1: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction;
  MacroRegisterFunctionWithName("Pipe::WriteAfterEmptying");
  //theLog << "Step 1: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction;
  this->pipeAvailable.LockMe();
//  theLog << logger::endL << "Step 2: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction
//  << logger::endL;
  this->ReadNoLocks();
  this->lastRead.SetSize(0);
  this->WriteNoLocks(toBeSent);
  this->pipeAvailable.UnlockMe();
//  theLog << logger::endL << "Step 3: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction
//  << logger::endL;
}

void Pipe::WriteNoLocks(const std::string& toBeSent)
{ MacroRegisterFunctionWithName("WebServer::WriteToPipe");
  if (this->thePipe[1]==-1)
    return;
//  std::string toBeSentCopy=toBeSent;
  int numBytesWritten=0;
  for (;;)
  { numBytesWritten=write(this->thePipe[1], toBeSent.c_str(), toBeSent.size());
    if (numBytesWritten<0)
      if (errno==EAI_AGAIN)
        continue;
    break;
  }
  if (numBytesWritten<0)
    theLog << logger::red << theWebServer.ToStringLastErrorDescription();
}

std::string Pipe::ToString()const
{ if (this->thePipe[0]==-1 || this->thePipe[1]==-1)
    return "released";
  std::stringstream out;
  out << this->thePipe[1] << "->" << this->thePipe[0];
  return out.str();
}

void Pipe::CreateMe()
{ this->Release();
  if (pipe(this->thePipe.TheObjects)<0)
    crash << "Failed to open pipe from parent to child. " << crash;
  fcntl(this->thePipe[1], F_SETFL, O_NONBLOCK);
  fcntl(this->thePipe[0], F_SETFL, O_NONBLOCK);
  this->pipeAvailable.CreateMe();
}

Pipe::~Pipe()
{ //Pipes are not allowed to release resources in the destructor:
  //a pipe's destructor is called when expanding List<Pipe>.
}

void Pipe::Release()
{ WebServer::Release(this->thePipe[0]);
  WebServer::Release(this->thePipe[1]);
  this->pipeAvailable.Release();
  this->lastRead.SetSize(0);
}

void Pipe::ReadNoLocks()
{ MacroRegisterFunctionWithName("Pipe::ReadFileDescriptor");
  this->lastRead.SetSize(0);
  if (this->thePipe[0]==-1)
    return;
  int counter=0;
  const unsigned int bufferSize=200000;
  this->pipeBuffer.SetSize(bufferSize); // <-once the buffer is resized, this operation does no memory allocation and is fast.
  int numReadBytes=0;
  for (;;)
  { //theLog << logger::blue << theWebServer.ToStringActiveWorker() << " pipe, " << this->ToString() << " calling read." << logger::endL;
    numReadBytes =read(this->thePipe[0], this->pipeBuffer.TheObjects, bufferSize);
    if (numReadBytes<0)
      if (errno==EAI_AGAIN || errno==EWOULDBLOCK)
        numReadBytes=0;
    if (numReadBytes>=0)
      break;
    counter++;
    if (counter>100)
      theLog << theWebServer.ToStringLastErrorDescription() << ". This is not supposed to happen: more than 100 iterations of read from pipe." << logger::endL;
  }
  if (numReadBytes>150000)
    theLog << "This is not supposed to happen: pipe read more than 150000 bytes." << logger::endL;
  if (numReadBytes>0)
  { this->pipeBuffer.SetSize(numReadBytes);
    this->lastRead=this->pipeBuffer;
  }
}

void Pipe::ReadWithoutEmptying()
{ MacroRegisterFunctionWithName("Pipe::ReadWithoutEmptying");
  this->pipeAvailable.LockMe();
  this->ReadNoLocks();
  if (this->lastRead.size>0)
  { std::string tempS;
    tempS.assign(this->lastRead.TheObjects, this->lastRead.size);
    this->WriteNoLocks(tempS);
  }
  this->pipeAvailable.UnlockMe();
}

void Pipe::Read()
{ MacroRegisterFunctionWithName("Pipe::Read");
  this->pipeAvailable.LockMe();
  this->ReadNoLocks();
  this->pipeAvailable.UnlockMe();
}

std::string WebServer::ToStringLastErrorDescription()
{ std::stringstream out;
  out << logger::red << this->ToStringActiveWorker() << ": " << strerror(errno) << ". ";
  return out.str();
}

std::string WebServer::ToStringActiveWorker()
{ if (theWebServer.activeWorker==-1)
    return "server";
  std::stringstream out;
  out << "worker " << theWebServer.activeWorker;
  return out.str();
}

std::string WebServer::ToStringStatusActive()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusActive");
  if (this->activeWorker==-1)
    return "server.";
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
  int numInUse=0;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
      numInUse++;
  for (int i=0; i<this->theWorkers.size; i++)
  { WebWorker& currentWorker=this->theWorkers[i];
    if (!currentWorker.flagInUse)
      continue;
    currentWorker.pipeWorkerToServerWorkerStatus.ReadWithoutEmptying();
    currentWorker.status.assign
    (currentWorker.pipeWorkerToServerWorkerStatus.lastRead.TheObjects,
     currentWorker.pipeWorkerToServerWorkerStatus.lastRead.size);
  }
  out << "<hr><hr>" << numInUse << " workers in use out of total " << this->theWorkers.size << " workers.";
  for (int i=0; i<this->theWorkers.size; i++)
    out << "<hr>" << this->theWorkers[i].ToStringStatus();
  return out.str();
}

void WebServer::CheckExecutableVersionAndRestartIfNeeded()
{ struct stat theFileStat;
  if (stat(onePredefinedCopyOfGlobalVariables.PhysicalNameFolderBelowExecutable.c_str(), &theFileStat)!=0)
    return;
  theLog << "current process spawned from file with time stamp: " << this->timeLastExecutableModification
  << "; latest executable time stamp: " << theFileStat.st_ctime << logger::endL;
  if (this->timeLastExecutableModification!=-1)
    if (this->timeLastExecutableModification!=theFileStat.st_ctime)
    { stOutput << "<b>The server executable was updated, but the server has not been restarted yet. "
      << "Restarting in 0.5 seconds...</b>";
      stOutput << "<script language=\"javascript\">setTimeout(resubmit, 500); "
      << " function resubmit() { document.getElementById('formCalculator').submit();}</script>";
      if (this->activeWorker!=-1)
      { this->GetActiveWorker().SendAllBytes();
        this->ReleaseActiveWorker();
      }
      theLog << logger::red << "Time stamps are different, RESTARTING." << logger::endL;
      this->Restart();
    }
}

void WebServer::Restart()
{ theLog << "Killing all copies of the calculator and restarting..." << logger::endL;
  this->Release(this->listeningSocketID);
  system("killall calculator \r\n./calculator server nokill"); //kill any other running copies of the calculator.
}

void WebServer::initDates()
{ this->timeLastExecutableModification=-1;
  struct stat theFileStat;
  if (stat(onePredefinedCopyOfGlobalVariables.PhysicalNameFolderBelowExecutable.c_str(), &theFileStat)!=0)
    return;
  this->timeLastExecutableModification=theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources()
{ MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  this->Release(this->GetActiveWorker().connectedSocketID);
  //<-release socket- communication is handled by the worker.
  this->activeWorker=-1; //<-The active worker is needed only in the child process.
}

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  if (this->flagTryToKillOlderProcesses)
    this->Restart();
  usleep(10000);
  this->initDates();
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
  { theLog << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
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
      theLog << "Error: bind failed. " << this->ToStringLastErrorDescription() << logger::endL;
      continue;
    }
    break;
  }
  if (p == NULL)
    crash << "Failed to bind to port " << PORT << "\n" << crash;
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(this->listeningSocketID, BACKLOG) == -1)
    crash << "Listen function failed." << crash;
// The following code does not appear to do anything on my system:
/*  struct sigaction sa;
  sa.sa_handler = &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
  sa.sa_handler=&WebServer::Signal_SIGINT_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
*/
  theLog << logger::purple <<  "server: waiting for connections...\r\n" << logger::endL;
  unsigned int connectionsSoFar=0;
  while(true)
  { // main accept() loop
    sin_size = sizeof their_addr;
    int newConnectedSocket = accept(this->listeningSocketID, (struct sockaddr *)&their_addr, &sin_size);
    if (newConnectedSocket <0)
    { theLog << "Accept failed. Error: " << this->ToStringLastErrorDescription() << logger::endL;
      continue;
    }
    //Listen for children who have exited properly.
    //This might need to be rewritten: I wasn't able to make this work with any
    //mechanism other than pipes. This probably due to my lack of skill or UNIX's
    //crappy design (and is most likely due to both).
    for (int i=0; i<this->theWorkers.size; i++)
      if (this->theWorkers[i].flagInUse)
      { this->theWorkers[i].pipeWorkerToServerControls.Read();
        if (this->theWorkers[i].pipeWorkerToServerControls.lastRead.size>0)
        { this->theWorkers[i].flagInUse=false;
          theLog << logger::green << "worker " << i+1 << " done, marking for reuse." << logger::endL;
        } else
          theLog << logger::red << "worker " << i+1 << " not done yet." << logger::endL;
        this->theWorkers[i].pipeWorkerToServerUserInput.Read();
        if (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size>0)
          this->theWorkers[i].displayUserInput.assign
          (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.TheObjects,
           this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size);
      }
    this->CreateNewActiveWorker();
    this->GetActiveWorker().connectedSocketID=newConnectedSocket;
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease=newConnectedSocket;
    connectionsSoFar++;
    this->GetActiveWorker().connectionID=connectionsSoFar;
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddressBuffer, sizeof userAddressBuffer);
    this->GetActiveWorker().userAddress=userAddressBuffer;
//    theLog << this->ToStringStatus();
    this->GetActiveWorker().ProcessPID=fork(); //creates an almost identical copy of this process.
    //The original process is the parent, the almost identical copy is the child.
    //theLog << "\r\nChildPID: " << this->childPID;
    if (this->GetActiveWorker().ProcessPID!=0)
    { // this is the child (worker) process
      this->Release(this->listeningSocketID);//worker has no access to socket listener
      onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=
      this->ReturnActiveIndicatorAlthoughComputationIsNotDone;
//      std::cout << "Got thus far 2" << std::endl;
      onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
//      std::cout << "Got thus far 3" << std::endl;
      MutexWrapper::InitializeAllAllocatedMutexesAllowMutexUse();
//      std::cout << "Got thus far 4" << std::endl;
      InitializeTimer();
//      std::cout << "Got thus far 5" << std::endl;
      CreateTimerThread();
//      std::cout << "Got thus far 6" << std::endl;
      /////////////////////////////////////////////////////////////////////////
      crash.CleanUpFunction=WebServer::SignalActiveWorkerDoneReleaseEverything;
//      stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
//      stOutput.flushOutputFunction=this->FlushActiveWorker;
//      theLog << this->ToStringStatusActive() << logger::endL;
      this->GetActiveWorker().CheckConsistency();
//      std::cout << "Got thus far 7" << std::endl;
      if (!this->GetActiveWorker().ReceiveAll())
      { stOutput << "HTTP/1.1 400 Bad request\r\nContent-type: text/html\r\n\r\n"
        << "<html><body><b>HTTP error 400 (bad request). </b> There was an error with the request. "
        << "One possibility is that the input was too large. "
        << "<br>The error message returned was:<br>"
        << this->GetActiveWorker().error
        << " <hr><hr>The message (part) that was received is: "
        << this->GetActiveWorker().ToStringMessageFull()
        << "</body></html>";
        stOutput.Flush();
        this->GetActiveWorker().SendAllBytes();
        return -1;
      }
//      std::cout << "Got thus far 8" << std::endl;
      this->GetActiveWorker().SendDisplayUserInputToServer();
     // std::cout << "Got thus far 9" << std::endl;
      return 1;
    }
    this->ReleaseWorkerSideResources();
  }
  return 0;
}

void WebServer::Release(int& theDescriptor)
{ close(theDescriptor);
  theDescriptor=-1;
}
