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

bool PauseController::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of " << this->ToString() << crash;
  return true;
}

PauseController::PauseController()
{ this->thePausePipe.initFillInObject(2, -1);
  this->mutexPipe.initFillInObject(2, -1);
  this->flagDeallocated=false;
}

PauseController::~PauseController()
{ this->flagDeallocated=true;
}

bool PauseController::CreateMe(const std::string& inputName)
{ this->Release();

  this->name=inputName;
  this->buffer.SetSize(200);
//  static int temporaryDebugCounter=0;
//  temporaryDebugCounter++;
//  if (temporaryDebugCounter%200==0)
//  { logBlock << logger::red << "FAILING pipe operation for debugging purposes. " << logger::endL;
//    return false;

//  }
  if (pipe(this->thePausePipe.TheObjects)<0)
  { logBlock << logger::purple << "FAILED to open pipe from parent to child. " << logger::endL;
    return false;
  }
  if (pipe(this->mutexPipe.TheObjects)<0)
  { logBlock << logger::purple << "FAILED to open pipe from parent to child. " << logger::endL;
    this->Release();
    return false;
  }
  if (fcntl(this->thePausePipe[1], F_SETFL, O_NONBLOCK)<0)
  { logBlock << logger::purple << "FAILED to fcntl the pausepipe from parent to child. " << logger::endL;
    this->Release();
    return false;
  }
//  write (this->thePausePipe[1], "!", 1);
//  write (this->mutexPipe[1], "!", 1);
  Pipe::WriteNoInterrupts(this->thePausePipe[1], "!");
  Pipe::WriteNoInterrupts(this->mutexPipe[1], "!");
  return true;
}

void PauseController::PauseIfRequested()
{ this->CheckConsistency();
  if (this->CheckPauseIsRequested())
    logBlock << logger::blue << "Blocking on " << this->ToString() << logger::endL;
  bool pauseWasRequested= !((read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size))>0);
  if (!pauseWasRequested)
    write(this->thePausePipe[1], "!", 1);
}

bool PauseController::PauseIfRequestedWithTimeOut()
{ this->CheckConsistency();
  fd_set read_fds, write_fds, except_fds;
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
  if (!(select(this->thePausePipe[0]+1, &read_fds, &write_fds, &except_fds, &timeout) == 1))
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
{ this->CheckConsistency();
  this->mutexForProcessBlocking.GetElement().LockMe();//<- make sure the pause controller is not locking itself
  //through competing threads
  if (this->CheckPauseIsRequested())
    logBlock << logger::blue << "Blocking on " << this->ToString() << logger::endL;
  read (this->thePausePipe[0], this->buffer.TheObjects, this->buffer.size);
  this->mutexForProcessBlocking.GetElement().UnlockMe();
}

bool PauseController::CheckPauseIsRequested()
{ this->CheckConsistency();
  read (this->mutexPipe[0], this->buffer.TheObjects, this->buffer.size);
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

int Pipe::WriteNoInterrupts(int theFD, const std::string& input)
{ for (;;)
  { int result=write(theFD, input.c_str(), input.size());
    if (result>=0)
      return result;
    if (result<0)
      if (errno==EINTR)
      { logBlock << logger::red << "Write operation interrupted, repeating. " << logger::endL;
        continue;
      }
    return result;
  }
  return -1;
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
      if (errno==EAI_AGAIN || errno==EWOULDBLOCK ||
          errno == EINTR || errno ==EIO)
        continue;
    break;
  }
  if (numBytesWritten<0)
    logIO << logger::red << "Error in " << this->ToString() << ". The error is: "
    << strerror(errno) << ". " << logger::endL;
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

bool Pipe::CreateMe(const std::string& inputPipeName)
{ this->CheckConsistency();
  this->Release();
  if (pipe(this->thePipe.TheObjects)<0)
  { logBlock << logger::purple << "Failed to open pipe from parent to child. " << logger::endL;
    this->Release();
    return false;
  }
  if (fcntl(this->thePipe[1], F_SETFL, O_NONBLOCK)<0)
  { logBlock << logger::purple << "Failed to fcntl pipe from parent to child. " << logger::endL;
    this->Release();
    return false;
  }
  if (fcntl(this->thePipe[0], F_SETFL, O_NONBLOCK)<0)
  { logBlock << logger::purple << "Failed to fcntl pipe from parent to child. " << logger::endL;
    this->Release();
    return false;
  }
  this->name=inputPipeName;
  if (! this->pipeAvailable.CreateMe("pause controller for pipe: "+ inputPipeName))
  { this->Release();
    return false;
  }
  return true;
}

bool Pipe::CheckConsistency()
{ MacroRegisterFunctionWithName("Pipe::CheckConsistency");
  if (this->flagDeallocated)
    crash << "This is a programming error: use after free of pipe. " << crash;
  return true;
}

Pipe::~Pipe()
{ //Pipes are not allowed to release resources in the destructor:
  //a pipe's destructor is called when expanding List<Pipe>.
  this->flagDeallocated=true;
}

Pipe::Pipe()
{ this->thePipe.initFillInObject(2,-1);
  this->flagDeallocated=false;
}


void Pipe::Release()
{ this->CheckConsistency();
  PauseController::Release(this->thePipe[0]);
  PauseController::Release(this->thePipe[1]);
  this->pipeAvailable.Release();
  this->lastRead.SetSize(0);
}

void Pipe::ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY()
{ MacroRegisterFunctionWithName("Pipe::ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY");
  this->ReadNoLocks();
}

void Pipe::ReadNoLocks()
{ MacroRegisterFunctionWithName("Pipe::ReadFileDescriptor");
  this->CheckConsistency();
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
      if (errno==EAI_AGAIN || errno==EWOULDBLOCK || errno==EINTR || errno == EIO)
        numReadBytes=0;
    if (numReadBytes>=0)
      break;
    counter++;
    if (counter>100)
    { logIO << logger::red << this->ToString()
      << ". This is not supposed to happen: more than 100 iterations of read resulted in an error. " << logger::endL;
      break;
    }
  }
  if (numReadBytes>150000)
    logIO << logger::red << this->ToString()
    << "This is not supposed to happen: pipe read more than 150000 bytes. " << logger::endL;
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
  this->CheckConsistency();
//  logIO << "Reading from pipe: " << this->ToString() << logger::endL;
  MutexRecursiveWrapper& safetyFirst=theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); //preventing threads from locking one another
//  logIO << "Passed mutex safety: " << this->ToString() << logger::endL;
  this->pipeAvailable.RequestPausePauseIfLocked();
//  logIO << "Passed pause controller safety, ready to read pipe: " << this->ToString() << logger::endL;
  this->ReadNoLocks();
//  logIO << "Read no locks successful? " << this->ToString() << logger::endL;
  this->pipeAvailable.ResumePausedProcessesIfAny();
  safetyFirst.UnlockMe(); //preventing threads from locking one another
}

logger::logger(const std::string& logFileName )
{ FileOperations::OpenFileCreateIfNotPresentUnsecure(theFile, logFileName, false, true, false);
  this->currentColor=logger::normalColor;
  this->flagStopWritingToFile=false;
  this->MaxLogSize=50000000;
}

void logger::CheckLogSize()
{ theFile.seekg(0, std::ios::end);
  if (theFile.tellg()>this->MaxLogSize)
    this->flagStopWritingToFile=true;
}

void logger::flush()
{
  std::cout.flush();
  theFile.flush();
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
    case logger::orange:
      return "\e[0;33m";
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
    case logger::orange:
      return "<span style=\"color:orange\">";
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
        std::cout.flush();
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
    case logger::orange:
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
