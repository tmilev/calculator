//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServerInterprocessLogistics.h"
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
#include <unistd.h>
#include <netdb.h> //<-addrinfo and related data structures defined here



ProjectInformationInstance projectInfoInstanceWebServerInterProcessLogisticsImplementation
(__FILE__, "Web server interprocess communication implementation.");

void PauseController::Release(int& theDescriptor)
{ close(theDescriptor);
  theDescriptor=-1;
}

void PauseController::Release()
{ PauseController::Release(this->mutexPipe[0]);
  PauseController::Release(this->mutexPipe[1]);
  PauseController::Release(this->thePausePipe[0]);
  PauseController::Release(this->thePausePipe[1]);
}

PauseController::PauseController()
{ this->thePausePipe.initFillInObject(2,-1);
  this->mutexPipe.initFillInObject(2, -1);
}

void PauseController::CreateMe(const std::string& inputName)
{ this->Release();
  this->name=inputName;
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
    logBlock << logger::blue << "Blocking on " << this->ToString() << logger::endL;
  bool pauseWasRequested= !((read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size))>0);
  if (!pauseWasRequested)
    write(this->thePausePipe[1], "!", 1);
}

bool PauseController::PauseIfRequestedWithTimeOut()
{ fd_set read_fds, write_fds, except_fds;
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  FD_SET(this->thePausePipe[0], &read_fds);
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  if (this->CheckPauseIsRequested())
    logBlock << logger::blue << "Blocking on " << this->ToString() << logger::endL;
  bool pauseWasRequested=false;
  if (!select(this->thePausePipe[0]+1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
  { logBlock << logger::green << "Blocking on " << this->ToString() << logger::green
    << " timed out. " << logger::endL;
    return false;
  }
  pauseWasRequested = !((read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size))>0);
  if (!pauseWasRequested)
    write(this->thePausePipe[1], "!", 1);
  return true;
}

void PauseController::RequestPausePauseIfLocked()
{ this->mutexForProcessBlocking.GetElement().LockMe();//<- make sure the pause controller is not locking itself
  //through competing threads
  if (this->CheckPauseIsRequested())
    logBlock << logger::blue << "Blocking on " << this->ToString() << logger::endL;
  read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size);
  this->mutexForProcessBlocking.GetElement().UnlockMe();
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

void PauseController::ResumePausedProcessesIfAny()
{ MacroRegisterFunctionWithName("PauseController::ResumePausedProcessesIfAny");
//  theLog << crash.GetStackTraceShort() << logger::endL;
  write(this->thePausePipe[1], "!", 1);
//  theLog << "Unlocking done!" << logger::endL;
}

std::string PauseController::ToString()const
{ std::stringstream out;
  if (this->name!="")
    out << this->name << ": ";
  if (this->thePausePipe[0]==-1)
    return "(not in use)";
  out << "pause-controller pipe " << this->thePausePipe[0] << "<-" << this->thePausePipe[1];
  return out.str();
}

void Pipe::WriteAfterEmptying(const std::string& toBeSent)
{ //theLog << "Step -1: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction;
  MacroRegisterFunctionWithName("Pipe::WriteAfterEmptying");
  //theLog << "Step 1: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction;
  MutexLockGuard safety(theGlobalVariables.MutexWebWorkerPipeWriteLock);
  this->pipeAvailable.RequestPausePauseIfLocked();
//  theLog << logger::endL << "Step 2: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction
//  << logger::endL;
  this->ReadNoLocks();
  this->lastRead.SetSize(0);
  this->WriteNoLocks(toBeSent);
  this->pipeAvailable.ResumePausedProcessesIfAny();
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
    logIO << logger::red << "Error in " << this->ToString() << ". The error is: " << strerror(errno) << ". ";
}

std::string Pipe::ToString()const
{ std::stringstream out;
  if (this->name!="")
    out << this->name << ": ";
  if (this->thePipe[0]==-1 || this->thePipe[1]==-1)
  { out << "released";
    return out.str();
  }
  out << this->thePipe[1] << "->" << this->thePipe[0];
  return out.str();
}

void Pipe::CreateMe(const std::string& inputPipeName)
{ this->Release();
  if (pipe(this->thePipe.TheObjects)<0)
    crash << "Failed to open pipe from parent to child. " << crash;
  fcntl(this->thePipe[1], F_SETFL, O_NONBLOCK);
  fcntl(this->thePipe[0], F_SETFL, O_NONBLOCK);
  this->name=inputPipeName;
  this->pipeAvailable.CreateMe("pause controller for pipe: "+ inputPipeName);
}

Pipe::~Pipe()
{ //Pipes are not allowed to release resources in the destructor:
  //a pipe's destructor is called when expanding List<Pipe>.
}

void Pipe::Release()
{ PauseController::Release(this->thePipe[0]);
  PauseController::Release(this->thePipe[1]);
  this->pipeAvailable.Release();
  this->lastRead.SetSize(0);
}

void Pipe::ReadNoLocks()
{ MacroRegisterFunctionWithName("Pipe::ReadFileDescriptor");
  this->lastRead.SetSize(0);
  if (this->thePipe[0]==-1)
    return;
  int counter=0;
  const unsigned int bufferSize=20000000;
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
      logIO << logger::red << this->ToString()
      << ". This is not supposed to happen: more than 100 iterations of read from pipe." << logger::endL;
  }
  if (numReadBytes>150000)
    logIO << logger::red << this->ToString() << "This is not supposed to happen: pipe read more than 150000 bytes." << logger::endL;
  if (numReadBytes>0)
  { this->pipeBuffer.SetSize(numReadBytes);
    this->lastRead=this->pipeBuffer;
  }
}

void Pipe::ReadWithoutEmptying()
{ MacroRegisterFunctionWithName("Pipe::ReadWithoutEmptying");
  MutexRecursiveWrapper& safetyFirst=theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); //preventing threads from locking one another
  this->pipeAvailable.RequestPausePauseIfLocked();
  this->ReadNoLocks();
  if (this->lastRead.size>0)
  { std::string tempS;
    tempS.assign(this->lastRead.TheObjects, this->lastRead.size);
    this->WriteNoLocks(tempS);
  }
  this->pipeAvailable.ResumePausedProcessesIfAny();
  safetyFirst.UnlockMe();
}

void Pipe::Read()
{ MacroRegisterFunctionWithName("Pipe::Read");
  MutexRecursiveWrapper& safetyFirst=theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); //preventing threads from locking one another
  this->pipeAvailable.RequestPausePauseIfLocked();
  this->ReadNoLocks();
  this->pipeAvailable.ResumePausedProcessesIfAny();
  safetyFirst.UnlockMe(); //preventing threads from locking one another
}

logger::logger(const std::string& logFileName )
{ FileOperations::OpenFileCreateIfNotPresent(theFile,logFileName, false, true, false);
  this->currentColor=logger::normalColor;
  this->flagStopWritingToFile=false;
}

void logger::CheckLogSize()
{ theFile.seekg(0, std::ios::end);
  if (theFile.tellg()>500000)
    this->flagStopWritingToFile=true;
}

std::string logger::closeTagConsole()
{ return "\e[0m";
}

std::string logger::closeTagHtml()
{ if (currentColor==logger::normalColor)
    return "";
  return "</span>";
}

std::string logger::openTagConsole()
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

std::string logger::openTagHtml()
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

logger& logger::operator << (const loggerSpecialSymbols& input)
{ this->CheckLogSize();
  switch (input)
  { case logger::endL:
      if (theGlobalVariables.flagUsingBuiltInWebServer)
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
      if (theGlobalVariables.flagUsingBuiltInWebServer)
        std::cout << this->openTagConsole();
      if (this->flagStopWritingToFile)
        return *this;
      this->theFile << this->closeTagHtml();
      this->theFile << this->openTagHtml();
      return *this;
    case logger::normalColor:
      if (theGlobalVariables.flagUsingBuiltInWebServer)
        std::cout << this->closeTagConsole();
      if (this->flagStopWritingToFile)
        return *this;
      theFile << this->closeTagHtml();
      return *this;
    default:
      return *this;
  }
}
