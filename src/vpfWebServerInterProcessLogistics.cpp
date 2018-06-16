//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServerInterprocessLogistics.h"
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
#include <unistd.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include "webserver.h"

extern WebServer theWebServer;

ProjectInformationInstance projectInfoInstanceWebServerInterProcessLogisticsImplementation
(__FILE__, "Web server interprocess communication implementation.");

std::string PauseProcess::currentProcessName = "S: ";

void PauseProcess::Release(int& theDescriptor)
{ if (theDescriptor == - 1)
    return;
  close(theDescriptor);
  theDescriptor = - 1;
}

PipePrimitive::PipePrimitive()
{ this->flagDeallocated = false;
  this->flagReadEndBlocks = true;
  this->flagWriteEndBlocks = true;
  this->pipeEnds.initFillInObject(2, - 1);
}

std::string PipePrimitive::ToString() const
{ if (this->pipeEnds.size == 0)
    return "pipe: non-initialized";
  if (this->pipeEnds[0] == - 1)
    return "pipe: not created";
  std::stringstream out;
  out << "pipe: " << this->pipeEnds[1] << "->" << this->pipeEnds[0];
  return out.str();
}

PipePrimitive::~PipePrimitive()
{ this->Release();
  this->flagDeallocated = true;
}

void PipePrimitive::Release()
{ PauseProcess::Release(this->pipeEnds[0]);
  PauseProcess::Release(this->pipeEnds[1]);
  this->lastRead.SetSize(0);
}

void PauseProcess::Release()
{ this->thePausePipe.Release();
  this->mutexPipe.Release();
  this->name = "";
}

bool PauseProcess::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of " << this->ToString() << crash;
  return true;
}

PauseProcess::PauseProcess()
{ this->flagDeallocated = false;
}

PauseProcess::~PauseProcess()
{ this->flagDeallocated = true;
}

std::string PipePrimitive::GetLastRead()
{ MacroRegisterFunctionWithName("PipePrimitive::GetLastRead");
  std::string result(this->lastRead.TheObjects, this->lastRead.size);
  return result;
}

bool PipePrimitive::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of pipe: " << this->ToString() << crash;
  return true;
}

bool PipePrimitive::CreateMe
(const std::string& inputPipeName,
 bool readEndBlocks, bool writeEndBlocks,
 bool restartServerOnFail, bool dontCrashOnFail)
{ this->name = inputPipeName;
  if (pipe(this->pipeEnds.TheObjects) < 0)
  { logServer << logger::red << "FAILED to create pipe: " << this->name << ". " << logger::endL;
    this->Release();
    return false;
  }
  if (!readEndBlocks)
    if (!this->SetPipeReadNonBlockingIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    { this->Release();
      return false;
    }
  if (!writeEndBlocks)
    if (!this->SetPipeWriteNonBlockingIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    { this->Release();
      return false;
    }
  return true;
}

bool PauseProcess::CreateMe
(const std::string& inputName,
 bool restartServerOnFail, bool dontCrashOnFail)
{ this->Release();
  this->name = inputName;
  if (!this->thePausePipe.CreateMe(inputName + "pausePipe", false, false, restartServerOnFail, dontCrashOnFail))
    return false;
  if (!this->mutexPipe.CreateMe(inputName + "mutexPipe", true, true, restartServerOnFail, dontCrashOnFail))
    return false;
  this->ResetNoAllocation();
  return true;
}

bool PauseProcess::ResetNoAllocation()
{ if (this->thePausePipe.WriteIfFailThenCrash("!", false, true) &&
      this->mutexPipe.WriteIfFailThenCrash("!", false, true))
    return true;
  logIO << logger::red << this->ToString() << ": failed to reset without allocation. " << logger::endL;
  return false;
}

void PauseProcess::PauseIfRequested(bool restartServerOnFail, bool dontCrashOnFail)
{ this->CheckConsistency();
  if (this->CheckPauseIsRequested(restartServerOnFail, dontCrashOnFail, false))
    logBlock << logger::blue << this->currentProcessName << "blocking on " << this->ToString() << logger::endL;
  bool pauseWasRequested= !this->thePausePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  if (!pauseWasRequested)
    this->thePausePipe.WriteIfFailThenCrash("!", restartServerOnFail, dontCrashOnFail);
}

bool PauseProcess::PauseIfRequestedWithTimeOut(bool restartServerOnFail, bool dontCrashOnFail)
{ this->CheckConsistency();
  fd_set read_fds, write_fds, except_fds;
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  FD_SET(this->thePausePipe.pipeEnds[0], &read_fds);
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  if (this->CheckPauseIsRequested(restartServerOnFail, dontCrashOnFail, false))
    logBlock << logger::blue << this->currentProcessName << "Blocking on " << this->ToString() << logger::endL;
  if (!(select(this->thePausePipe.pipeEnds[0]+ 1, &read_fds, &write_fds, &except_fds, &timeout) == 1))
  { logBlock << logger::green << this->currentProcessName << "Blocking on " << this->ToString() << logger::green
    << " timed out. " << logger::endL;
    return false;
  }
  this->thePausePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  if (this->thePausePipe.lastRead.size>0)
    this->thePausePipe.WriteIfFailThenCrash("!", restartServerOnFail, dontCrashOnFail);
  return true;
}

void PauseProcess::RequestPausePauseIfLocked(bool restartServerOnFail, bool dontCrashOnFail)
{ this->CheckConsistency();
  this->mutexForProcessBlocking.GetElement().LockMe();//<- make sure the pause controller is not locking itself
  //through competing threads
  if (this->CheckPauseIsRequested(restartServerOnFail, dontCrashOnFail, false))
    logBlock << logger::blue << this->currentProcessName << "Blocking on " << this->ToString() << logger::endL;
  this->thePausePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  this->mutexForProcessBlocking.GetElement().UnlockMe();
}

bool PauseProcess::CheckPauseIsRequested(bool restartServerOnFail, bool dontCrashOnFail, bool dontLockImServer)
{ this->CheckConsistency();
  if (!dontLockImServer)
    if (!this->mutexPipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
      return false;
  if (!this->thePausePipe.SetPipeReadNonBlockingIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    return false;
  if (!this->thePausePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    return false;
  bool result = (this->thePausePipe.lastRead.size == 0);
  if (!result)
    if (!this->thePausePipe.WriteIfFailThenCrash("!", restartServerOnFail, dontCrashOnFail))
      return false;
  if (!this->thePausePipe.SetPipeReadBlockingModeIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    return false;
  if (!this->mutexPipe.WriteIfFailThenCrash("!", restartServerOnFail, dontCrashOnFail))
    return false;
  return result;
}

void PauseProcess::ResumePausedProcessesIfAny(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("PauseController::ResumePausedProcessesIfAny");
  this->thePausePipe.WriteIfFailThenCrash("!", restartServerOnFail, dontCrashOnFail);
}

std::string PauseProcess::ToString() const
{ std::stringstream out;
  if (this->name != "")
    out << this->name << ": ";
  out << "pausing: " << this->thePausePipe.ToString() << ", mutex: "
  << this->mutexPipe.ToString();
  return out.str();
}

int Pipe::WriteWithTimeoutViaSelect
(int theFD, const std::string& input, int timeOutInSeconds, int maxNumTries, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("Pipe::WriteWithTimeoutViaSelect");
  fd_set theFDcontainer;
  FD_ZERO(&theFDcontainer);
  FD_SET(theFD, &theFDcontainer);
  timeval timeOut;
  timeOut.tv_sec = timeOutInSeconds;
  timeOut.tv_usec = 0;
  int numFails = 0;
  int numSelected = - 1;
  std::stringstream failStream;
  do
  { if (numFails > maxNumTries)
    { failStream << maxNumTries << " failed or timed-out select attempts on file descriptor: " << theFD;
      break;
    }
    numSelected = select(theFD + 1,0, &theFDcontainer, 0, &timeOut);
    failStream << "While select-writing on file descriptor: " << theFD << ", select failed. Error message: "
    << strerror(errno) << ". \n";
    numFails ++;
  } while (numSelected < 0);
  if (numSelected <= 0)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << failStream.str();
    return - 1;
  }
  return Pipe::WriteNoInterrupts(theFD, input);
}

int Pipe::ReadWithTimeOutViaSelect
(int theFD, List<char>& output, int timeOutInSeconds, int maxNumTries, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("Pipe::ReadWithTimeOutViaSelect");
  if (theFD < 0)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Attempting to read from a negative file descriptor: " << theFD;
    return - 1;
  }
  fd_set theFDcontainer;
  FD_ZERO(&theFDcontainer);
  FD_SET(theFD, &theFDcontainer);
  timeval timeOut;
  timeOut.tv_sec = timeOutInSeconds;
  timeOut.tv_usec = 0;
  int numFails = 0;
  int numSelected = - 1;
  std::stringstream failStream;
  do
  { if (numFails > maxNumTries)
    { failStream << maxNumTries << " failed or timed-out select attempts on file descriptor: " << theFD;
      break;
    }
    numSelected = select(theFD + 1, &theFDcontainer, 0, 0, &timeOut);
    failStream << "While select-reading from file descriptor: " << theFD << ", select failed. Error message: "
    << strerror(errno) << ". \n";
    numFails ++;
  } while (numSelected < 0);
  //numSelected== 0 most probably means timeout has expired.
  if (numSelected <= 0)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << failStream.str();
    return - 1;
  }
  return read(theFD, output.TheObjects, output.size - 1);
}

bool PipePrimitive::SetPipeFlagsIfFailThenCrash(int inputFlags, int whichEnd, bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::SetPipeBlockingModeCrashIfFail");
  int counter = 0;
  while (fcntl(this->pipeEnds[whichEnd], F_SETFL, inputFlags) < 0)
  { std::string theError = strerror(errno);
    logBlock << logger::purple << "Failed to fcntl pipe end with file descriptor: "
    << this->pipeEnds[whichEnd] << ": "
    << theError << ". " << logger::endL;
    if (++ counter > 100)
    { if (restartServerOnFail)
        theWebServer.Restart();
      else if (!dontCrashOnFail)
        crash << "fcntl failed more than 100 times on "
        << "file desciptor: " << this->pipeEnds[whichEnd] << ": " << theError << ". " << crash;
      return false;
    }
  }
  return true;
}

bool PipePrimitive::SetPipeReadNonBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::SetPipeReadNonBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(O_NONBLOCK, 0, restartServerOnFail, dontCrashOnFail);
  if (result)
    this->flagReadEndBlocks = false;
  return result;
}

bool PipePrimitive::SetPipeReadBlockingModeIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::SetPipeReadBlockingModeIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(0, 0, restartServerOnFail, dontCrashOnFail);
  if (result)
    this->flagReadEndBlocks = true;
  return result;
}

bool PipePrimitive::SetPipeWriteNonBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::SetPipeWriteNonBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(O_NONBLOCK, 1, restartServerOnFail, dontCrashOnFail);
  if (result)
    this->flagWriteEndBlocks = false;
  return result;
}

bool PipePrimitive::SetPipeWriteBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::SetPipeWriteBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(0, 1, restartServerOnFail, dontCrashOnFail);
  if (result)
    this->flagWriteEndBlocks = true;
  return result;
}

int Pipe::WriteNoInterrupts(int theFD, const std::string& input)
{ int numAttempts = 0;
  for (;;)
  { int result = write(theFD, input.c_str(), input.size());
    if (result >= 0)
      return result;
    if (result < 0)
      if (errno == EINTR)
      { logBlock << logger::red << "Write operation interrupted, repeating. " << logger::endL;
        numAttempts ++;
        if (numAttempts > 100)
        { logBlock << logger::red << "Write operation interrupted, more than 100 times, this is not supposed to happen. "
          << logger::endL;
          return - 1;
        }
        continue;
      }
    return result;
  }
  return - 1;
}

void Pipe::WriteAfterEmptying(const std::string& toBeSent, bool restartServerOnFail, bool dontCrashOnFail)
{ //logWorker << "Step - 1: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction;
  MacroRegisterFunctionWithName("Pipe::WriteAfterEmptying");
  //logWorker << "Step 1: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction;
  MutexLockGuard safety(theGlobalVariables.MutexWebWorkerPipeWriteLock);
  this->theMutexPipe.RequestPausePauseIfLocked(restartServerOnFail, dontCrashOnFail);
//  logWorker << logger::endL << "Step 2: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction
//  << logger::endL;
  this->thePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  this->thePipe.lastRead.SetSize(0);
  this->thePipe.WriteIfFailThenCrash(toBeSent, restartServerOnFail, dontCrashOnFail);
  this->theMutexPipe.ResumePausedProcessesIfAny(restartServerOnFail, dontCrashOnFail);
//  logWorker << logger::endL << "Step 3: Pipe::WriteAfterEmptying: outputFunction: " << (int) stOutput.theOutputFunction
//  << logger::endL;
}

bool PipePrimitive::ReadWithoutEmptying(bool restartServerOnFail, bool dontCrashOnFail)
{ if (this->flagReadEndBlocks || this->flagWriteEndBlocks)
    crash << this->ToString() << ": read without emptying allowed only on fully non-blocking pipes. " << crash;
  if (!this->ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    return false;
  return this->WriteIfFailThenCrash(this->GetLastRead(), restartServerOnFail, dontCrashOnFail);
}

bool PipePrimitive::WriteAfterEmptying(const std::string& input, bool restartServerOnFail, bool dontCrashOnFail)
{ if (this->flagReadEndBlocks)
    crash << this->ToString() << ": write after emptying allowed only on non-blocking read pipes. " << crash;
  if (!this->ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail))
    return false;
  return this->WriteIfFailThenCrash(input, restartServerOnFail, dontCrashOnFail);
}

bool PipePrimitive::WriteIfFailThenCrash(const std::string& toBeSent, bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("PipePrimitive::WriteIfFailThenCrash");
  if (this->pipeEnds[1] == - 1)
  { logIO << logger::yellow << "WARNING: " << this->ToString()
    << " writing on non-initialized pipe. ";
    return false;
  }
  if (toBeSent.size() == 0)
    return true;
//  std::string toBeSentCopy= toBeSent;
  int numBytesWritten = 0;
  int numBadAttempts = 0;
  for (;;)
  { numBytesWritten = write(this->pipeEnds[1], toBeSent.c_str(), toBeSent.size());
    if (numBytesWritten < 0)
      if (errno == EAI_AGAIN || errno == EWOULDBLOCK ||
          errno == EINTR || errno == EIO)
      { numBadAttempts ++;
        if (theGlobalVariables.processType == ProcessTypes::worker)
          logIO << logger::red << "Failed write on pipe: " << this->ToString() << numBadAttempts
          << " times. Last error: " << strerror(errno) << ". " << logger::endL;
        if (theGlobalVariables.processType == ProcessTypes::server)
          logServer << logger::red << "Failed write on pipe: " << this->ToString() << numBadAttempts
          << " times. Last error: " << strerror(errno) << ". " << logger::endL;
        if (numBadAttempts > 30)
        { if (theGlobalVariables.processType == ProcessTypes::worker)
            logIO << logger::red << "Failed write on pipe: " << this->ToString() << ". Last error: "
            << strerror(errno) << ". " << logger::endL;
          if (theGlobalVariables.processType == ProcessTypes::server)
            logServer << logger::red << "Failed write on pipe: " << this->ToString() << ". Last error: "
            << strerror(errno) << ". " << logger::endL;
          if (restartServerOnFail)
            theWebServer.Restart();
          else if (!dontCrashOnFail)
            crash << "Failed write on pipe: " << this->ToString() << ". Last error: "
            << strerror(errno) << ". " << crash;
          return false;
        }
        continue;
      }
    break;
  }
  if (numBytesWritten < 0)
    return false;
  if ((unsigned) numBytesWritten < toBeSent.size())
  { if (theGlobalVariables.processType == ProcessTypes::worker)
      logIO << logger::red << this->ToString() << ": wrote only "
      << numBytesWritten << " bytes out of " << toBeSent.size() << " total. " << logger::endL;
    if (theGlobalVariables.processType == ProcessTypes::server)
      logServer << logger::red << this->ToString() << ": wrote only "
      << numBytesWritten << " bytes out of " << toBeSent.size() << " total. " << logger::endL;
  }
  return true;
}

std::string Pipe::ToString() const
{ std::stringstream out;
  if (this->name != "")
    out << this->name << ": information pipe: ";
  out << this->thePipe.ToString();
  out << " mutex data: " << this->theMutexPipe.ToString();
  return out.str();
}

bool Pipe::ResetNoAllocation()
{ if (this->theMutexPipe.ResetNoAllocation() &&
      this->thePipe.ReadIfFailThenCrash(false, true))
    return true;
  logIO << logger::red << this->ToString() << ": failed to reset without allocation. " << logger::endL;
  return false;
}

bool Pipe::CreateMe(const std::string& inputPipeName)
{ this->CheckConsistency();
  this->Release();
  this->name = inputPipeName;
  if (!this->thePipe.CreateMe("pipe[" + inputPipeName + "]", false, false, false, true))
  { this->Release();
    return false;
  }
  if (!this->theMutexPipe.CreateMe("mutex[" + inputPipeName + "]", false, true))
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
  this->flagDeallocated = true;
}

Pipe::Pipe()
{ this->flagDeallocated = false;
}

void Pipe::Release()
{ this->CheckConsistency();
  this->theMutexPipe.Release();
  this->thePipe.Release();
}

bool PipePrimitive::ReadIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("PipePrimitive::ReadIfFailThenCrash");
  this->CheckConsistency();
  this->lastRead.SetSize(0);
  if (this->pipeEnds[0] == - 1)
    return false;
  int counter = 0;
  const unsigned int bufferSize = 200000;
  this->buffer.SetSize(bufferSize); // <-once the buffer is resized, this operation does no memory allocation and is fast.
  int numReadBytes = 0;
  for (;;)
  { //logWorker << logger::blue << theWebServer.ToStringActiveWorker() << " pipe, " << this->ToString() << " calling read." << logger::endL;
    numReadBytes = read(this->pipeEnds[0], this->buffer.TheObjects, bufferSize);
    if (numReadBytes >= 0)
      break;
    counter ++;
    if (counter > 100)
    { logIO << logger::red << this->ToString()
      << ": more than 100 iterations of read resulted in an error. "
      << logger::endL;
      if (restartServerOnFail)
        theWebServer.Restart();
      else if (!dontCrashOnFail)
        crash << this->ToString() << ": more than 100 iterations of read resulted in an error. " << crash;
      return false;
    }
    if (errno == EWOULDBLOCK)
      return true;
  }
  if (numReadBytes > 150000)
    logIO << logger::red << this->ToString()
    << "This is not supposed to happen: pipe read more than 150000 bytes. " << logger::endL;
  if (numReadBytes > 0)
  { this->buffer.SetSize(numReadBytes);
    this->lastRead = this->buffer;
  }
  return true;
}

void Pipe::ReadWithoutEmptying(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::ReadWithoutEmptying");
  MutexRecursiveWrapper& safetyFirst = theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); //preventing threads from locking one another
  this->theMutexPipe.RequestPausePauseIfLocked(restartServerOnFail, dontCrashOnFail);
  this->thePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  if (this->thePipe.lastRead.size > 0)
  { std::string tempS;
    tempS = this->GetLastRead();
    this->thePipe.WriteIfFailThenCrash(tempS, restartServerOnFail, dontCrashOnFail);
  }
  this->theMutexPipe.ResumePausedProcessesIfAny(restartServerOnFail, dontCrashOnFail);
  safetyFirst.UnlockMe();
}

void Pipe::Read(bool restartServerOnFail, bool dontCrashOnFail)
{ MacroRegisterFunctionWithName("Pipe::Read");
  this->CheckConsistency();
//  logIO << "Reading from pipe: " << this->ToString() << logger::endL;
  MutexRecursiveWrapper& safetyFirst = theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); //preventing threads from locking one another
//  logIO << "Passed mutex safety: " << this->ToString() << logger::endL;
  this->theMutexPipe.RequestPausePauseIfLocked(restartServerOnFail, dontCrashOnFail);
//  logIO << "Passed pause controller safety, ready to read pipe: " << this->ToString() << logger::endL;
  this->thePipe.ReadIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
//  logIO << "Read no locks successful? " << this->ToString() << logger::endL;
  this->theMutexPipe.ResumePausedProcessesIfAny(restartServerOnFail, dontCrashOnFail);
  safetyFirst.UnlockMe(); //preventing threads from locking one another
}

logger::logger(const std::string& logFileName, logger* inputCarbonCopy, bool inputResetLogWhenTooLarge, const std::string& inputProcessType)
{ this->flagInitialized = false;
  this->theFileName = logFileName;
  this->currentColor = logger::normalColor;
  this->flagTagColorHtmlOpened = false;
  this->flagTagColorConsoleOpened = false;
  this->flagStopWritingToFile = true;
  this->carbonCopy = inputCarbonCopy;
  this->flagResetLogFileWhenTooLarge = inputResetLogWhenTooLarge;
  this->processType = inputProcessType;
  this->flagWriteImmediately = false;
}

void logger::initializeIfNeeded()
{ if (!this->flagInitialized)
    this->reset();
}

void logger::reset()
{ this->flagInitialized = true;
  this->currentColor = logger::normalColor;
  this->flagTagColorHtmlOpened = false;
  this->flagTagColorConsoleOpened = false;
  this->flagStopWritingToFile = false;
  this->MaxLogSize = //10000
  50000000;
  if (theGlobalVariables.flagRunningApache || this->theFileName=="")
  { this->flagStopWritingToFile = true;
    return;
  }
  if (this->theFile.is_open())
    this->theFile.close();
  FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(this->theFile, this->theFileName, false, true, false, true);
  if (! this->theFile.is_open())
  { this->currentColor = logger::red;
    std::string computedFileName;
    FileOperations::GetPhysicalFileNameFromVirtual(this->theFileName, computedFileName, true, false, 0);
    std::cout << this->openTagConsole() << "Could not open log file with virtual name: "
    << this->theFileName << " and computed name: "
    << computedFileName << this->closeTagConsole()
    << std::endl;
  }
  this->theFile.seekg(0);
  this->theFile.seekp(0);
}

void logger::CheckLogSize()
{ this->initializeIfNeeded();
  if (theFile.tellp() > this->MaxLogSize)
  { if (this->flagResetLogFileWhenTooLarge)
    { this->reset();
      if (this->MaxLogSize > 1000)
        *this << logger::endL << "Log file reset. " << logger::endL;
    } else
      this->flagStopWritingToFile = true;
  }
}

void logger::flush()
{ this->initializeIfNeeded();
  std::cout.flush();
  theFile.flush();
  theFile.clear();
}

std::string logger::blueConsole()
{ return "\e[1;34m";
}

std::string logger::yellowConsole()
{ return "\e[1;33m";
}

std::string logger::greenConsole()
{ return "\e[1;32m";
}

std::string logger::redConsole()
{ return "\e[1;31m";
}

std::string logger::normalConsole()
{ return "\e[0m";
}

std::string logger::closeTagConsole()
{ if (!this->flagTagColorConsoleOpened)
    return "";
  this->flagTagColorConsoleOpened = false;
  return "\e[0m";
}

std::string logger::closeTagHtml()
{ if (!this->flagTagColorHtmlOpened)
    return "";
  this->flagTagColorHtmlOpened = false;
  return "</span>";
}

std::string logger::openTagConsole()
{ std::stringstream out;
  out << logger::closeTagConsole();
  switch (this->currentColor)
  { case logger::red:
      out << "\e[1;31m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::green:
      out << "\e[1;32m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::yellow:
      out << "\e[1;33m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::blue:
      out << "\e[1;34m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::purple:
      out << "\e[1;35m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::cyan:
      out << "\e[1;36m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::orange:
      out << "\e[0;33m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    default:
      return out.str();
  }
}

std::string logger::openTagHtml()
{ std::stringstream out;
  out << logger::closeTagHtml();
  switch (this->currentColor)
  { case logger::red:
      out << "<span style=\"color:red\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::green:
      out << "<span style=\"color:green\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::blue:
      out << "<span style=\"color:blue\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::yellow:
      out << "<span style=\"color:yellow\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::orange:
      out << "<span style=\"color:orange\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::purple:
      out << "<span style=\"color:purple\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    default:
      return out.str();
  }
}

std::string logger::getStamp()
{ std::stringstream out;
  out << theGlobalVariables.processType << ", ";
  out
  << theGlobalVariables.GetDateForLogFiles() << ", "
  << theGlobalVariables.GetElapsedSeconds() << " s, ";
  if (theWebServer.activeWorker != - 1)
    out << "w: " << theWebServer.activeWorker << ",";
  out << " c: " << theWebServer.NumConnectionsSoFar << ". ";
  //<-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

logger& logger::operator << (const loggerSpecialSymbols& input)
{ if (theGlobalVariables.flagRunningApache)
    return *this;
  this->initializeIfNeeded();
  this->CheckLogSize();
  bool doUseColors = theGlobalVariables.flagRunningBuiltInWebServer || theGlobalVariables.flagRunningCommandLine;
  switch (input)
  { case logger::endL:
      if (doUseColors)
        std::cout << this->closeTagConsole() << std::endl;
      std::cout.flush();
      this->currentColor = logger::normalColor;
      if (this->flagStopWritingToFile)
        return *this;
      this->buffer+= this->closeTagHtml() + "\n<br>\n";
      theFile << this->getStamp() << this->buffer;
      this->buffer = "";
      theFile.flush();
      return *this;
    case logger::red:
    case logger::blue:
    case logger::yellow:
    case logger::green:
    case logger::purple:
    case logger::orange:
    case logger::cyan:
      this->currentColor = input;
      if (doUseColors)
        std::cout << this->openTagConsole();
      if (this->flagStopWritingToFile)
        return *this;
      this->theFile << this->openTagHtml();
      return *this;
    case logger::normalColor:
      if (doUseColors)
        std::cout << this->closeTagConsole();
      this->currentColor = logger::normalColor;
      if (this->flagStopWritingToFile)
        return *this;
      theFile << this->closeTagHtml();
      return *this;
    default:
      return *this;
  }
}
