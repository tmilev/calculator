//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"
#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics

#ifdef MACRO_use_open_ssl
//installation of these headers in ubuntu:
//sudo apt-get install libssl-dev
//openssl tutorial: http://www.ibm.com/developerworks/library/l-openssl/
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#endif // MACRO_use_open_ssl

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

extern void static_html4(std::stringstream& output);

bool ProgressReportWebServer::flagServerExists=true;

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
{ if (!this->flagServerExists)
    return;
  theWebServer.theProgressReports.SetSize(this->indexProgressReport);
  this->flagDeallocated=true;
}

void ProgressReportWebServer::SetStatus(const std::string& inputStatus)
{ MacroRegisterFunctionWithName("ProgressReportWebServer::SetStatus");
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing || !this->flagServerExists)
    return;
  if (!theGlobalVariables.flagUsingBuiltInWebServer)
    return;
  theWebServer.CheckConsistency();
//  theLog << logger::endL << logger::red << "SetStatus: outputFunction: "
//  << (int) stOutput.theOutputFunction << logger::endL;
  MutexRecursiveWrapper& safetyFirst=theGlobalVariables.MutexWebWorkerStaticFiasco;
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 2" << std::endl;
  safetyFirst.LockMe();
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 3" << std::endl;
  if (this->indexProgressReport>=theWebServer.theProgressReports.size)
    theWebServer.theProgressReports.SetSize(this->indexProgressReport+1);
  theWebServer.theProgressReports[this->indexProgressReport]=inputStatus;
//    std::cout << "SetStatus: passed mutex section" << std::endl;
  std::stringstream toBePiped;
  for (int i=0; i<theWebServer.theProgressReports.size; i++)
    if (theWebServer.theProgressReports[i]!="")
      toBePiped << "<br>" << theWebServer.theProgressReports[i];
  safetyFirst.UnlockMe();
  // theLog << logger::endL << logger::red << "SetStatus before the issue: outputFunction: "
  // << (int) stOutput.theOutputFunction << logger::endL;
  if (!theGlobalVariables.flagUsingBuiltInWebServer)
    return;
  theWebServer.GetActiveWorker().pipeWorkerToServerWorkerStatus.WriteAfterEmptying(toBePiped.str());
}

void WebServer::Signal_SIGINT_handler(int s)
{ theLog << "Signal interrupt handler called with input: " << s;
//  << ". Waiting for children to exit... " << logger::endL;
  theWebServer.ReleaseActiveWorker();
  theWebServer.ReleaseNonActiveWorkers();
  while(waitpid(-1, NULL, WNOHANG | WEXITED) > 0)
  { }
  theLog << "All children have exited. " << logger::endL;
  exit(0);
}

void WebServer::Signal_SIGCHLD_handler(int s)
{ theLog << "Signal child handler called with input: " << s << "." << logger::endL;
  int waitResult=0;
  do
  { waitResult= waitpid(-1, NULL, WNOHANG| WEXITED);
    if (waitResult>0)
      for (int i=0; i<theWebServer.theWorkers.size; i++)
        if (theWebServer.theWorkers[i].ProcessPID==waitResult)
          theWebServer.theWorkers[i].pipeWorkerToServerControls.WriteAfterEmptying("close");
  }while (waitResult>0);
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
  out << lineBreak << "Display path server base: " << theGlobalVariables.DisplayPathServerBase;
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
//  theLog << logger::endL << logger::green << " Standard output: " << standardOutputStreamAfterTimeout.str()
//  << logger::endL;
}

void WebWorker::OutputBeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::OutputBeforeComputation");
  theGlobalVariables.flagComputationCompletE=false;

  stOutput << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>calculator version  " << __DATE__ << ", " << __TIME__ << "</title>";
//  if (theGlobalVariables.flagUsingBuiltInWebServer)
  stOutput << CGI::GetLaTeXProcessingJavascript();
//  else
//    stOutput << "<script src=\"" << theGlobalVariables.DisplayPathServerBase << "/jsmath/easy/load.js\">";
  stOutput << "\n</head>\n<body onload=\"checkCookie();\">\n";
  List<std::string> inputStrings, inputStringNames;
  CGI::ChopCGIInputStringToMultipleStrings(theParser.inputStringRawestOfTheRaw, inputStrings, inputStringNames);
  std::string& civilizedInput=theParser.inputString;
  if (inputStringNames.Contains("textInput"))
    civilizedInput= inputStrings[inputStringNames.GetIndex("textInput")];
  CGI::CGIStringToNormalString(civilizedInput, civilizedInput);

//  civilizedInput="\\int( 1/x dx)";
//  civilizedInput="\\int (1/(x(1+x^2)^2))dx";
//  civilizedInput="%LogEvaluation \\int (1/(5+2x+7x^2)^2)dx";
  theGlobalVariables.initOutputReportAndCrashFileNames(theParser.inputStringRawestOfTheRaw, civilizedInput);

  std::stringstream tempStreamXX;
  static_html4(tempStreamXX);

//stOutput << "Folders: " << theGlobalVariables.ToStringFolderInfo();

  stOutput << tempStreamXX.str();
  stOutput << "<table>\n <tr valign=\"top\">\n ";
  stOutput << "<td>"; //<td> tag will be closed later in WebWorker::OutputStandardResult
  stOutput << "\n<FORM method=\"POST\" id=\"formCalculator\" name=\"formCalculator\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
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
    stOutput << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?" << theParser.inputStringRawestOfTheRaw << "\">Link to your input.</a>";
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
  std::fstream outputTimeOutFile;
  FileOperations::OpenFileCreateIfNotPresent(outputTimeOutFile, theGlobalVariables.PhysicalNameOutpuT, false, true, false);
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
  theGlobalVariables.flagTimedOutComputationIsDone=true;
  theLog << "WebWorker::StandardOutputPart2ComputationTimeout called with worker number "
  << theWebServer.GetActiveWorker().indexInParent+1 << "." << logger::endL;
  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked();
  theLog << logger::blue << "Sending result through indicator pipe." << logger::endL;
  ProgressReportWebServer theReport("Sending result through indicator pipe.");

  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  logBlock << logger::blue << "Final output written to indicator, blocking until data "
  << "is received on the other end." << logger::endL;
  theReport.SetStatus("Blocking until result data is received.");
  if(!theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut())
    theReport.SetStatus("Computation report NOT RECEIVED (timed out).");
  else
  { theReport.SetStatus("Computation report received.");
  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
    theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked();
  }
//  theLog << logger::red << "Result data is received, sending \"finished\"." << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying("finished");
  theReport.SetStatus(" \"finished\" sent through indicator pipe, waiting.");
  theLog << logger::red << "\"finished\" sent through indicator pipe, waiting." << logger::endL;
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut();
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
  theGlobalVariables.theSourceCodeFiles().QuickSortAscending();
  stOutput << theGlobalVariables.ToStringSourceCodeInfo();
  stOutput << "<hr><b>Calculator status. </b><br>";
  stOutput << theParser.ToString();

  stOutput << "</td></tr></table>";
  std::stringstream tempStream3;
  stOutput << "\n\n<script language=\"javascript\">\n" << "  var theAutocompleteDictionary = new Array;\n  ";
  for (int i=0; i<theParser.theAtoms.size; i++)
    if (theParser.theAtoms[i].size()>2)
      stOutput << "  theAutocompleteDictionary.push(\"" << theParser.theAtoms[i] << "\");\n";
  stOutput << "</script>\n";
  stOutput << "</body></html>";
  stOutput << "<!--";
  ProgressReport theReport(&theGlobalVariables);
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
//  << theGlobalVariables.DisplayNameCalculatorWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size(): "
//  << theGlobalVariables.DisplayNameCalculatorWithPath.size();
  CGI::CGIStringToNormalString(this->mainAddresSRAW, this->mainAddress);
  this->mainArgumentRAW="";
  std::string calculatorArgumentRawWithQuestionMark, tempS;
  if (!MathRoutines::StringBeginsWith
      (this->mainAddresSRAW, theGlobalVariables.DisplayNameCalculatorWithPath,
       &calculatorArgumentRawWithQuestionMark))
  { CGI::CGIFileNameToFileName(this->mainAddresSRAW, this->mainAddress);

    return;
  }
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
  theReport.SetStatus("WebWorker::SendAllBytes: starting ...");
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    theReport.SetStatus("Socket::SendAllBytes failed: connectedSocketID=-1. WebWorker::SendAllBytes - finished.");
    return;
  }
  theLog << "Sending " << this->remainingBytesToSend.size << " bytes in chunks of: " << logger::endL;
  std::stringstream reportStream;
  ProgressReportWebServer theReport2;
  reportStream << "Sending " << this->remainingBytesToSend.size << " bytes...";
  theReport.SetStatus(reportStream.str());
  //  theLog << "\r\nIn response to: " << this->theMessage;
  double startTime=theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending=0;
  int timeOutInSeconds =20;
  while (this->remainingBytesToSend.size>0)
  { std::stringstream reportStream2;
    reportStream2 << this->remainingBytesToSend.size << " bytes remaining to send. ";
    if (numTimesRunWithoutSending>0)
      reportStream2 << "Previous attempt to send bytes resulted in 0 bytes sent; this is attempt number "
      << numTimesRunWithoutSending+1 << ". ";
    theReport2.SetStatus(reportStream2.str());
    if (theGlobalVariables.GetElapsedSeconds()-startTime>timeOutInSeconds)
    { theLog << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      std::stringstream reportStream3;
      reportStream3 << "WebWorker::SendAllBytes failed: more than "
      << timeOutInSeconds << " seconds have elapsed. Continuing ...";
      theReport.SetStatus(reportStream3.str());
      return;
    }
    setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO,(void*)(&tv), sizeof(timeval));
    int numBytesSent=send
    (this->connectedSocketID, &this->remainingBytesToSend[0], this->remainingBytesToSend.size,0);
    if (numBytesSent<0)
    { theLog << "WebWorker::SendAllBytes failed. Error: "
      << this->parent->ToStringLastErrorDescription() << logger::endL;
      theReport.SetStatus
      ("WebWorker::SendAllBytes failed. Error: " + this->parent->ToStringLastErrorDescription()
       + "WebWorker::SendAllBytes - continue ...");
      return;
    }
    if (numBytesSent==0)
      numTimesRunWithoutSending++;
    else
      numTimesRunWithoutSending=0;
    theLog << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      theLog << ", ";
    if (numTimesRunWithoutSending>3)
    { theLog << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      theReport.SetStatus
      ("WebWorker::SendAllBytes failed: send function went through 3 cycles without sending any bytes. ");
      return;
    }
    theLog << logger::endL;
  }
  reportStream << " done. ";
  theReport2.SetStatus(reportStream.str());
  theReport.SetStatus("WebWorker::SendAllBytes - finished.");
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
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 30;  // 30 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO,(void*)(&tv), sizeof(timeval));
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker: receiving bytes...");
  int numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
//  std::cout << "Got thus far 11" << std::endl;
  theReport.SetStatus("WebWorker: first bytes received...");
  double numSecondsAtStart=theGlobalVariables.GetElapsedSeconds();
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
  if (this->requestType==WebWorker::requestTypes::requestPostCalculator)
    this->displayUserInput="POST " + this->mainArgumentRAW;
  else
    this->displayUserInput="GET " + this->mainAddresSRAW;
  if (this->ContentLength<=0)
    return true;
  if (this->mainArgumentRAW.size()==(unsigned) this->ContentLength)
    return true;
//  theLog << "Content-length parsed to be: " << this->ContentLength
//  << "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  std::stringstream reportStream;
  reportStream << "WebWorker: received first message of "
  << this->ContentLength << " bytes. ";
  theReport.SetStatus(reportStream.str());
  if (this->ContentLength>10000000)
  { this->CheckConsistency();
    error="Content-length parsed to be more than 10 million bytes, aborting.";
    theLog << this->error << logger::endL;
    this->displayUserInput=this->error;
    return false;
  }
//  std::cout << "Debug code here!!!";
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
  reportStream << "Sending continue message ...";
  theReport.SetStatus(reportStream.str());
  this->remainingBytesToSend=(std::string) "HTTP/1.1 100 Continue\r\n";
  this->SendAllBytes();
  this->remainingBytesToSend.SetSize(0);
  reportStream << " done. ";
  theReport.SetStatus(reportStream.str());
  if ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { reportStream << " Only " << this->mainArgumentRAW.size() << " out of "
    << this->ContentLength << " bytes received, proceeding to receive the rest. ";
    theReport.SetStatus(reportStream.str());
  }
  std::string bufferString;
  while ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds()-numSecondsAtStart>180)
    { this->error= "Receiving bytes timed out (180 seconds).";
      theLog << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
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
    theReport.SetStatus(out.str());
    return false;
  }
  theReport.SetStatus("Webworker: received everything, processing. ");
  return true;
}

void WebWorker::SendDisplayUserInputToServer()
{ MacroRegisterFunctionWithName("WebWorker::SendDisplayUserInputToServer");
  if (this->displayUserInput.size()>3000)
    this->displayUserInput.resize(3000);
  this->pipeWorkerToServerUserInput.WriteAfterEmptying(this->displayUserInput);
//  theLog << logger::blue << "Piping " << this->displayUserInput << " to the server. " << logger::endL;
}

void WebWorker::SanitizeMainAddress()
{ MacroRegisterFunctionWithName("WebWorker::SanitizeMainAddress");
  std::string resultAddress;
  this->mainAddressNonSanitized=this->mainAddress;
  resultAddress.reserve(this->mainAddress.size());
  bool foundslash=false;
  for (signed i=(signed) this->mainAddress.size()-1; i>=0; i--)
  { if (foundslash && this->mainAddress[i]=='.')
      this->flagMainAddressSanitized=true;
    else
      resultAddress.push_back(this->mainAddress[i]);
    if (this->mainAddress[i]=='/')
      foundslash=true;
  }
  this->mainAddress="";
  for (int i=resultAddress.size()-1; i>=0;i--)
    this->mainAddress.push_back(resultAddress[i]);
}

void WebWorker::ExtractPhysicalAddressFromMainAddress()
{ MacroRegisterFunctionWithName("WebWorker::ExtractPhysicalAddressFromMainAddress");
  int numBytesToChop=theGlobalVariables.DisplayPathServerBase.size();
  std::string displayAddressStart= this->mainAddress.substr(0, numBytesToChop);
  if (displayAddressStart!=theGlobalVariables.DisplayPathServerBase)
    numBytesToChop=0;
  this->SanitizeMainAddress();
  this->PhysicalFileName=theGlobalVariables.PhysicalPathServerBase+
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
//  if (theGlobalVariables.flagLogInterProcessCommunication)
//  theLog << "About to read pipeServerToWorker..." << logger::endL;
  if (!otherWorker.PauseWorker.CheckPauseIsRequested())
  { otherWorker.PauseWorker.RequestPausePauseIfLocked();
    stOutput << "paused";
    return 0;
  }
  stOutput << "unpaused";
  otherWorker.PauseWorker.ResumePausedProcessesIfAny();
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
    otherWorker.PauseComputationReportReceived.ResumePausedProcessesIfAny();
  }
//  stOutput << "<b>Not implemented: request for indicator for worker " << inputWebWorkerNumber
//  << " out of " << this->parent->theWorkers.size << ".</b>";
  return 0;
}

void WebWorker::WriteProgressReportToFile(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::WriteProgressReportToFile");
  theLog << logger::green << "Progress report written to file: " << theGlobalVariables.PhysicalNameProgressReport << logger::endL;
  std::fstream theFile;
  if (!FileOperations::OpenFileCreateIfNotPresent(theFile, theGlobalVariables.PhysicalNameProgressReport, false, true, false))
    FileOperations::OpenFileCreateIfNotPresent
    (theFile, theGlobalVariables.PhysicalPathOutputFolder+"progressReport_failed_to_create_file.html",
     false, true, false);
  theFile << standardOutputStreamAfterTimeout.str() << "<hr>" << input;
  theFile.flush();
  theFile.close();
}

void WebWorker::PipeProgressReportToParentProcess(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::PipeProgressReportToParentProcess");
  static int counter=0;
  counter++;
  std::stringstream debugStream1, debugStream2;
  debugStream1 << "PipeProgressReportToParentProcess called " << counter << " times. Calling pause...";
  ProgressReportWebServer theReport(debugStream1.str());
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
//    theLog << "about to potentially block " << logger::endL;
  debugStream2 << "PipeProgressReportToParentProcess called " << counter << " times. Pause passed...";
  theReport.SetStatus(debugStream2.str());
  if (this->PauseWorker.CheckPauseIsRequested())
  { theReport.SetStatus("PipeProgressReportToParentProcess: pausing as requested...");
    this->WriteProgressReportToFile(input);
  }
  this->PauseWorker.PauseIfRequested();     //if pause was requested, here we block
//    theLog << "(possible) block passed" << logger::endL;
  theReport.SetStatus("PipeProgressReportToParentProcess: computing...");
  this->pipeServerToWorkerRequestIndicator.Read();
  if (this->pipeServerToWorkerRequestIndicator.lastRead.size==0)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
    return;
  }
  if (theGlobalVariables.flagTimedOutComputationIsDone)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
    return;
  }
//  if (theGlobalVariables.flagLogInterProcessCommunication)
//  theLog << " data written!";
  theReport.SetStatus("PipeProgressReportToParentProcess: piping computation process...");
  this->pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  theReport.SetStatus("PipeProgressReportToParentProcess: exiting 1...");
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
  theReport.SetStatus("PipeProgressReportToParentProcess: exiting 2...");
}

int WebWorker::ProcessFolder()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  std::stringstream out;
  out << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << "<html><body>";
//  out << this->ToString();
  if (this->flagMainAddressSanitized)
  { out << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
    << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
    << " Therefore I have sanitized the main address to: " << this->mainAddress;
  }
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
  this->flagMainAddressSanitized=false;
  this->timeOfLastPingServerSideOnly=-1;
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
//  std::cout << "got thus far xxxxxxx" << std:SignalIamDoneReleaseEverything:endl;
  this->pipeWorkerToServerControls.WriteAfterEmptying("close");
  theLog << logger::blue << "Notification dispatched." << logger::endL;
  this->SendAllBytes();
  this->Release();
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing=true;
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
  ProgressReportWebServer theProgressReport;
  theProgressReport.SetStatus("<br>Processing non-computational web-server request.");
  //theLog << this->ToStringShort() << "\r\n";
  if (FileOperations::IsFolder(this->PhysicalFileName))
    return this->ProcessFolder();
  if (!FileOperations::FileExists(this->PhysicalFileName))
  { stOutput << "HTTP/1.1 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>File does not exist.</b>";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name:</b> "
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
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> ";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name: </b>"
    << this->mainAddress << "<br><b>File physical name: </b>"
    << this->PhysicalFileName << "</body></html>";
    return 0;
  }
  theFile.seekp(0, std::ifstream::end);
  unsigned int fileSize=theFile.tellp();
  std::stringstream reportStream;
  reportStream << "<br>Serving file " << this->PhysicalFileName << " ...";
  ProgressReportWebServer theProgressReport2;
  theProgressReport2.SetStatus(reportStream.str());

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
  reportStream << " done!";
  theProgressReport2.SetStatus(reportStream.str());
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
  //stOutput << "Thread data follows.<br> " << ThreadData::ToStringAllThreads();
  theWebServer.CheckExecutableVersionAndRestartIfNeeded();
  //stOutput << theParser.javaScriptDisplayingIndicator;
  //theParser.inputString="TestCalculatorIndicator 0";
  //theParser.inputString="printSemisimpleSubalgebrasRecompute(B_3)";
  ProgressReportWebServer theReport;
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computing...");
  if (theParser.inputString!="")
    theParser.Evaluate(theParser.inputString);
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computation complete, preparing output");
  theGlobalVariables.flagComputationCompletE=true;
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    if (theGlobalVariables.flagOutputTimedOut)
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
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameIndicatorWithPath << "\";\n";
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
  out << "  pauseURL  = \"" << theGlobalVariables.DisplayNameCalculatorWithPath
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
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?indicator"
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
  ProgressReportWebServer theReport;
  theReport.SetStatus("All bytes from client received, processing. Worker process in use ...");
  theGlobalVariables.flagComputationStarted=true;
  theGlobalVariables.IndicatorStringOutputFunction=WebServer::PipeProgressReportToParentProcess;
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
  theReport.SetStatus("Reply to client prepared, proceeding to send and release all resources...");
  this->SignalIamDoneReleaseEverything();
  return 0;
}

void WebWorker::ReleaseKeepInUseFlag()
{ MacroRegisterFunctionWithName("WebServer::ReleaseMyPipe");
  this->pipeWorkerToServerTimerPing.Release();
  this->pipeWorkerToServerControls.Release();
  this->pipeServerToWorkerRequestIndicator.Release();
  this->pipeWorkerToServerIndicatorData.Release();
  this->pipeWorkerToServerUserInput.Release();
  this->pipeWorkerToServerWorkerStatus.Release();
  this->PauseComputationReportReceived.Release();
  this->PauseWorker.Release();
  this->PauseIndicatorPipeInUse.Release();
  this->timeOfLastPingServerSideOnly=-1;
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release()
{ this->ReleaseKeepInUseFlag();
  this->flagInUse=false;
}

void WebWorker::OutputShowIndicatorOnTimeout()
{ MacroRegisterFunctionWithName("WebServer::OutputShowIndicatorOnTimeout");
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
  theGlobalVariables.flagOutputTimedOut=true;
  theGlobalVariables.flagTimedOutComputationIsDone=false;
  ProgressReportWebServer theReport("WebServer::OutputShowIndicatorOnTimeout");
  theLog << logger::blue << "Computation timeout, sending progress indicator instead of output. " << logger::endL;
  stOutput << "</td></tr>";
  if (theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed)
    stOutput << "<tr><td>Your computation is taking more than " << theGlobalVariables.MaxComputationTimeBeforeWeTakeAction
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
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: exiting function.");
//  this->SignalIamDoneReleaseEverything();
//  theLog << consoleGreen("Indicator: released everything and signalled end.") << logger::endL;
}

std::string WebWorker::ToStringStatus()const
{ std::stringstream out;
  out << "Worker " << this->indexInParent+1;
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
  out << " Server time at last ping: " << this->timeOfLastPingServerSideOnly << " seconds. ";
  if (this->pingMessage!="")
    out << " Message at last ping: " << this->pingMessage;
  if (this->status!="")
    out << "<br><span style=\"color:red\"><b> Status: " << this->status << "</b></span><br>";
  out << "Pipe indices: " << this->pipeWorkerToServerControls.ToString()
  << ", " << this->pipeWorkerToServerTimerPing.ToString()
  << ", " << this->pipeServerToWorkerRequestIndicator.ToString()
  << ", " << this->pipeWorkerToServerIndicatorData.ToString()
  << ", " << this->pipeWorkerToServerUserInput.ToString()
  << ", " << this->pipeWorkerToServerWorkerStatus.ToString()
  << ", " << this->PauseWorker.ToString()
  << ", " << this->PauseComputationReportReceived.ToString()
  << ", " << this->PauseIndicatorPipeInUse.ToString()
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
  ProgressReportWebServer::flagServerExists=false;
  for (int i=0; i<this->theWorkers.size; i++)
    this->theWorkers[i].Release();
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=0;
  theGlobalVariables.WebServerTimerPing=0;
  theGlobalVariables.IndicatorStringOutputFunction=0;
  theGlobalVariables.PauseUponUserRequest=0;
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
  this->flagPort8155=false;
  this->activeWorker=-1;
  this->timeLastExecutableModification=-1;
}

WebWorker& WebServer::GetActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  void (*oldOutputFunction)(const std::string& stringToOutput) =stOutput.theOutputFunction;
  stOutput.theOutputFunction=0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrupt) web server
  //to report the error over the Internet.
  if (this->activeWorker<0 || this->activeWorker>=this->theWorkers.size)
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  stOutput.theOutputFunction=oldOutputFunction;//<-the web server is in order,
  //therefore we restore the ability to report crashes over the internet.
  return this->theWorkers[this->activeWorker];
}

void WebServer::SignalActiveWorkerDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebServer::SignalActiveWorkerDoneReleaseEverything");
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

void WebServer::WorkerTimerPing(double pingTime)
{
  if (theWebServer.activeWorker==-1)
  { if (!theGlobalVariables.flagComputationFinishedAllOutputSentClosing)
      crash << "WebServer::WorkerTimerPing called when the computation is not entirely complete. " << crash;
    return;
  }
  std::stringstream outTimestream;
  outTimestream << "Worker: " << pingTime << " seconds passed. ";
  theWebServer.GetActiveWorker().pipeWorkerToServerTimerPing.WriteAfterEmptying(outTimestream.str());
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
      this->theWorkers[i].pingMessage="";
      break;
    }
  if (this->activeWorker==-1)
  { this->activeWorker=this->theWorkers.size;
    this->theWorkers.SetSize(this->theWorkers.size+1);
  }
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUse=true;

  this->GetActiveWorker().PauseComputationReportReceived.CreateMe("server to worker computation report received");
  this->GetActiveWorker().PauseWorker.CreateMe("server to worker pause");
  this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe("server to worker indicator pipe in use");
  this->GetActiveWorker().pipeServerToWorkerRequestIndicator.CreateMe("server to worker request indicator");
  this->GetActiveWorker().pipeWorkerToServerTimerPing.CreateMe("worker to server timer ping");
  this->GetActiveWorker().pipeWorkerToServerControls.CreateMe("worker to server controls");
  this->GetActiveWorker().pipeWorkerToServerIndicatorData.CreateMe("worker to server indicator data");
  this->GetActiveWorker().pipeWorkerToServerUserInput.CreateMe("worker to server user input");
  this->GetActiveWorker().pipeWorkerToServerWorkerStatus.CreateMe("worker to server worker status");
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().parent=this;
  this->GetActiveWorker().timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
  this->GetActiveWorker().pingMessage="";
}



std::string WebServer::ToStringLastErrorDescription()
{ std::stringstream out;
  out << logger::red << "Process " << this->ToStringActiveWorker() << ": " << strerror(errno) << ". ";
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
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
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
{ //theLog << "Killing all copies of the calculator and restarting..." << logger::endL;
  //theLog << "Time limit after restart: " << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;
  this->Release(this->listeningSocketID);
//  char arg1[7]="server";
//  char arg2[7]="nokill";
//  char* args[2];
//  args[1]=arg1;
//  args[2]=arg2;
//  char** args=0 ;
//  execv("./calculator server nokill", args);
//char *exec_argv[] = { "./calculator", "server", "nokill"};
//sleep(1);
//execv("/proc/self/exe", exec_argv);
  std::stringstream theCommand;
  int timeInteger=(int) theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  theCommand << "killall " <<  theGlobalVariables.PhysicalNameExecutableNoPath + " \r\n./";
  if (this->flagPort8155)
    theCommand << timeInteger << " server8155 nokill " << timeInteger;
  else
    theCommand << theGlobalVariables.PhysicalNameExecutableNoPath << " server nokill " << timeInteger;
  system(theCommand.str().c_str()); //kill any other running copies of the calculator.
}

void WebServer::initDates()
{ this->timeLastExecutableModification=-1;
  struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
    return;
  this->timeLastExecutableModification=theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources()
{ MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  this->Release(this->GetActiveWorker().connectedSocketID);
  //<-release socket- communication is handled by the worker.
  this->activeWorker=-1; //<-The active worker is needed only in the child process.
}

//const char* PORT ="8080";  // the port users will be connecting to
const int BACKLOG =10;     // how many pending connections queue will hold


void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
  crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void fperror_sigaction(int signal)
{
  crash << "Fatal arithmetic error. " << crash;
  exit(0);
}

void WebServer::RecycleChildrenIfPossible()
{ //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
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
      this->theWorkers[i].pipeWorkerToServerTimerPing.Read();
      if (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size>0)
      { this->theWorkers[i].pingMessage.assign
        (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.TheObjects,
         this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size);
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } if (this->theWorkers[i].PauseWorker.CheckPauseIsRequested())
      { this->theWorkers[i].pingMessage="worker paused, no pings.";
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else
        if (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit>0 &&
            theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly>
            theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
            )
          { this->theWorkers[i].flagInUse=false;
            std::stringstream pingTimeoutStream;
            pingTimeoutStream << theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly
            << " seconds have passed since worker " << i+1
            << " pinged the server. I am assuming the worker no longer functions, and am marking it as free for reuse. ";
            theLog << logger::red << pingTimeoutStream.str() << logger::endL;
            this->theWorkers[i].pingMessage="<span style=\"color:red\"><b>" + pingTimeoutStream.str()+"</b></span>";
          }
    }
}

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  List<std::string> thePorts;
  if (this->flagPort8155)
    thePorts.AddOnTop("8155");
  thePorts.AddOnTop("8080");
  thePorts.AddOnTop("8081");
  thePorts.AddOnTop("8082");
  thePorts.AddOnTop("8155");

  if (this->flagTryToKillOlderProcesses)
    this->Restart();
  usleep(10000);
  this->initDates();
  addrinfo hints;
  addrinfo *servinfo=0;
  addrinfo *p=0;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  int yes=1;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  // loop through all the results and bind to the first we can
  for (int i=0; i<thePorts.size; i++)
  { if ((rv = getaddrinfo(NULL, thePorts[i].c_str(), &hints, &servinfo)) != 0)
    { theLog << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
      return 1;
    }
    for(p = servinfo; p != NULL; p = p->ai_next)
    { this->listeningSocketID= socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (this->listeningSocketID == -1)
      { theLog << "Error: socket failed.\n";
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
    if (p!=NULL)
    { theLog << logger::yellow << "Successfully bounded to port " << thePorts[i] << logger::endL;
      break;
    }
  }
  if (p == NULL)
    crash << "Failed to bind to any of the ports " << thePorts.ToStringCommaDelimited() << "\n" << crash;
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(this->listeningSocketID, BACKLOG) == -1)
    crash << "Listen function failed." << crash;

//  struct sigaction sa;
//  memset(&sa, 0, sizeof(sigaction));
//  sigemptyset(&sa.sa_mask);
//  sa.sa_sigaction = segfault_sigaction;
//  sa.sa_flags   = SA_SIGINFO;
//  sigaction(SIGSEGV, &sa, NULL);
  //catch segfaults:
  struct sigaction sa;
  sa.sa_sigaction = &segfault_sigaction;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "Was not able to register SIGSEGV handler. Crashing to let you know. " << crash;
  }
  //catch floating point exceptions
  sa.sa_sigaction=0;
  sa.sa_handler= &fperror_sigaction;
  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGFPE, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "Was not able to register SIGFPE handler. Crashing to let you know. " << crash;
  }
  sa.sa_handler=&WebServer::Signal_SIGINT_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
  sa.sa_handler = &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
  theLog << logger::purple <<  "server: waiting for connections...\r\n" << logger::endL;
  unsigned int connectionsSoFar=0;
//  theLog << "time limit in seconds:  " << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;
#ifdef MACRO_use_open_ssl
  bool flagUseSSl=true;
  if (flagUseSSl)
  { SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    theLog << logger::green << "openssl initialized." << logger::endL;
    BIO* theBio;
    theBio = BIO_new_connect("hostname:port");
    if(theBio == NULL)
    { crash << "Failed to connecte" << crash;
    }

//  if(BIO_do_connect(bio) <= 0)
//  {
      /* Handle failed connection */
//  }
  }
#endif

  while(true)
  { // main accept() loop
    sin_size = sizeof their_addr;
    int newConnectedSocket = accept(this->listeningSocketID, (struct sockaddr *)&their_addr, &sin_size);
    if (newConnectedSocket <0)
    { theLog << "Accept failed. Error: " << this->ToStringLastErrorDescription() << logger::endL;
      continue;
    }
    this->RecycleChildrenIfPossible();
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
      theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=
      this->ReturnActiveIndicatorAlthoughComputationIsNotDone;
      theGlobalVariables.WebServerTimerPing=this->WorkerTimerPing;
      theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
      crash.CleanUpFunction=WebServer::SignalActiveWorkerDoneReleaseEverything;
      InitializeTimer();
      CreateTimerThread();
      /////////////////////////////////////////////////////////////////////////
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
{ PauseController::Release(theDescriptor);
}
