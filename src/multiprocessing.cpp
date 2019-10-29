//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "multiprocessing.h"
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
#include <unistd.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include "webserver.h"

extern WebServer theWebServer;

extern ProjectInformationInstance projectInfoInstanceWebServerInterProcessLogisticsImplementation;
ProjectInformationInstance projectInfoInstanceWebServerInterProcessLogisticsImplementation(
  __FILE__, "Web server interprocess communication implementation."
);

std::string PauseProcess::currentProcessName = "S: ";

void PauseProcess::Release(int& theDescriptor) {
  if (theDescriptor == - 1) {
    return;
  }
  close(theDescriptor);
  theDescriptor = - 1;
}

PipePrimitive::PipePrimitive() {
  this->flagDeallocated = false;
  this->flagReadEndBlocks = true;
  this->flagWriteEndBlocks = true;
  this->numberOfBytesLastWrite = 0;
  this->pipeEnds.initializeFillInObject(2, - 1);
}

std::string PipePrimitive::ToString() const {
  if (this->pipeEnds.size == 0) {
    return "pipe: non-initialized";
  }
  if (this->pipeEnds[0] == - 1) {
    return "pipe: not created";
  }
  std::stringstream out;
  out << "pipe: " << this->pipeEnds[1] << "->" << this->pipeEnds[0];
  return out.str();
}

PipePrimitive::~PipePrimitive() {
  this->Release();
  this->flagDeallocated = true;
}

void PipePrimitive::Release() {
  PauseProcess::Release(this->pipeEnds[0]);
  PauseProcess::Release(this->pipeEnds[1]);
  this->numberOfBytesLastWrite = 0;
  this->lastRead.SetSize(0);
}

void PauseProcess::Release() {
  this->thePausePipe.Release();
  this->mutexPipe.Release();
  this->name = "";
}

bool PauseProcess::CheckConsistency() {
  if (this->flagDeallocated) {
    crash << "Use after free of " << this->ToString() << crash;
  }
  return true;
}

PauseProcess::PauseProcess() {
  this->flagDeallocated = false;
}

PauseProcess::~PauseProcess() {
  this->flagDeallocated = true;
}

std::string PipePrimitive::GetLastRead() {
  MacroRegisterFunctionWithName("PipePrimitive::GetLastRead");
  std::string result(this->lastRead.TheObjects, static_cast<unsigned>(this->lastRead.size));
  return result;
}

bool PipePrimitive::CheckConsistency() {
  if (this->flagDeallocated) {
    crash << "Use after free of pipe: " << this->ToString() << crash;
  }
  return true;
}

bool PipePrimitive::CreateMe(
  const std::string& inputPipeName,
  bool readEndBlocks,
  bool writeEndBlocks,
  bool restartServerOnFail,
  bool dontCrashOnFail
) {
  this->name = inputPipeName;
  if (pipe(this->pipeEnds.TheObjects) < 0) {
    logServer << logger::red << "FAILED to create pipe: " << this->name << ". " << logger::endL;
    this->Release();
    theWebServer.StopKillAll(false);
    // return false;
  }
  if (!readEndBlocks) {
    if (!this->SetPipeReadNonBlockingIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
      this->Release();
      return false;
    }
  }
  if (!writeEndBlocks) {
    if (!this->SetPipeWriteNonBlockingIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
      this->Release();
      return false;
    }
  }
  return true;
}

bool PauseProcess::CreateMe(
  const std::string& inputName,
  bool restartServerOnFail,
  bool dontCrashOnFail
) {
  this->Release();
  this->name = inputName;
  if (!this->thePausePipe.CreateMe(inputName + "pausePipe", false, false, restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  if (!this->mutexPipe.CreateMe(inputName + "mutexPipe", true, true, restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  this->ResetNoAllocation();
  return true;
}

bool PauseProcess::ResetNoAllocation() {
  if (
    this->thePausePipe.WriteOnceIfFailThenCrash("!", 0, false, true) &&
    this->mutexPipe.WriteOnceIfFailThenCrash("!", 0, false, true)
  ) {
    return true;
  }
  logIO << logger::red << this->ToString() << ": failed to reset without allocation. " << logger::endL;
  return false;
}

void PauseProcess::PauseIfRequested(bool restartServerOnFail, bool dontCrashOnFail) {
  this->CheckConsistency();
  if (this->CheckPauseIsRequested(restartServerOnFail, dontCrashOnFail, false)) {
    logBlock << logger::blue << this->currentProcessName << "blocking on " << this->ToString() << logger::endL;
  }
  bool pauseWasRequested = !this->thePausePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  if (!pauseWasRequested) {
    this->thePausePipe.WriteOnceIfFailThenCrash("!", 0, restartServerOnFail, dontCrashOnFail);
  }
}

bool PauseProcess::PauseIfRequestedWithTimeOut(bool restartServerOnFail, bool dontCrashOnFail) {
  this->CheckConsistency();
  fd_set read_fds, write_fds, except_fds;
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  FD_SET(this->thePausePipe.pipeEnds[0], &read_fds);
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  if (this->CheckPauseIsRequested(restartServerOnFail, dontCrashOnFail, false)) {
    logBlock << logger::blue << this->currentProcessName << "Blocking on " << this->ToString() << logger::endL;
  }
  if (!(select(this->thePausePipe.pipeEnds[0] + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)) {
    logBlock << logger::green << this->currentProcessName << "Blocking on " << this->ToString() << logger::green
    << " timed out. " << logger::endL;
    return false;
  }
  this->thePausePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  if (this->thePausePipe.lastRead.size > 0) {
    this->thePausePipe.WriteOnceIfFailThenCrash("!", 0, restartServerOnFail, dontCrashOnFail);
  }
  return true;
}

void PauseProcess::RequestPausePauseIfLocked(bool restartServerOnFail, bool dontCrashOnFail) {
  this->CheckConsistency();
  this->lockThreads.GetElement().LockMe(); // <- make sure the pause controller is not locking itself
  // through competing threads
  if (this->CheckPauseIsRequested(restartServerOnFail, dontCrashOnFail, false)) {
    logBlock << logger::blue << this->currentProcessName << "Blocking on " << this->ToString() << logger::endL;
  }
  this->thePausePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  this->lockThreads.GetElement().UnlockMe();
}

bool PauseProcess::CheckPauseIsRequested(bool restartServerOnFail, bool dontCrashOnFail, bool dontLockImServer) {
  this->CheckConsistency();
  if (!dontLockImServer) {
    if (!this->mutexPipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
      return false;
    }
  }
  if (!this->thePausePipe.SetPipeReadNonBlockingIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  if (!this->thePausePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  bool result = (this->thePausePipe.lastRead.size == 0);
  if (!result) {
    if (!this->thePausePipe.WriteOnceIfFailThenCrash("!", 0, restartServerOnFail, dontCrashOnFail)) {
      return false;
    }
  }
  if (!this->thePausePipe.SetPipeReadBlockingModeIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  if (!this->mutexPipe.WriteOnceIfFailThenCrash("!", 0, restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  return result;
}

void PauseProcess::ResumePausedProcessesIfAny(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("PauseController::ResumePausedProcessesIfAny");
  this->thePausePipe.WriteOnceIfFailThenCrash("!", 0, restartServerOnFail, dontCrashOnFail);
}

std::string PauseProcess::ToString() const {
  std::stringstream out;
  if (this->name != "") {
    out << this->name << ": ";
  }
  out << "pausing: " << this->thePausePipe.ToString() << ", mutex: "
  << this->mutexPipe.ToString();
  return out.str();
}

int Pipe::WriteWithTimeoutViaSelect(
  int theFD, const std::string& input, int timeOutInSeconds, int maxNumTries, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Pipe::WriteWithTimeoutViaSelect");
  fd_set theFDcontainer;
  FD_ZERO(&theFDcontainer);
  FD_SET(theFD, &theFDcontainer);
  timeval timeOut;
  timeOut.tv_sec = timeOutInSeconds;
  timeOut.tv_usec = 0;
  int numFails = 0;
  int numSelected = - 1;
  std::stringstream failStream;
  do {
    if (numFails > maxNumTries) {
      failStream << maxNumTries << " failed or timed-out select attempts on file descriptor: " << theFD;
      break;
    }
    numSelected = select(theFD + 1, nullptr, &theFDcontainer, nullptr, &timeOut);
    failStream << "While select-writing on file descriptor: " << theFD << ", select failed. Error message: "
    << strerror(errno) << ". \n";
    numFails ++;
  } while (numSelected < 0);
  if (numSelected <= 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << failStream.str();
    }
    return - 1;
  }
  return Pipe::WriteNoInterrupts(theFD, input);
}

int Pipe::ReadWithTimeOutViaSelect(
  int theFD,
  List<char>& output,
  int timeOutInSeconds,
  int maxNumTries,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Pipe::ReadWithTimeOutViaSelect");
  if (theFD < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Attempting to read from a negative file descriptor: " << theFD;
    }
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
  do {
    if (numFails > maxNumTries) {
      failStream << maxNumTries << " failed or timed-out select attempts on file descriptor: " << theFD;
      break;
    }
    numSelected = select(theFD + 1, &theFDcontainer, nullptr, nullptr, &timeOut);
    failStream << "While select-reading from file descriptor: "
    << theFD << ", select failed. Error message: "
    << strerror(errno) << ". \n";
    numFails ++;
  } while (numSelected < 0);
  //numSelected == 0 most probably means timeout has expired.
  if (numSelected <= 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << failStream.str();
    }
    return - 1;
  }
  int bytesRead = - 1;
  do {
    bytesRead = static_cast<int>(read(
      theFD,
      output.TheObjects,
      static_cast<unsigned>(output.size - 1)
    ));
    if (bytesRead > 0) {
      return bytesRead;
    }
    numFails ++;

    if (numFails > maxNumTries) {
      failStream << "Too many failed attempts: " << maxNumTries
      << " at reading from file descriptor: " << theFD << ". Error message: "
      << strerror(errno) << ".\n";
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << failStream.str();
      }
      return - 1;
    }
  } while (bytesRead <= 0);
  return bytesRead;
}

bool PipePrimitive::SetPipeFlagsIfFailThenCrash(int inputFlags, int whichEnd, bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeBlockingModeCrashIfFail");
  int counter = 0;
  while (fcntl(this->pipeEnds[whichEnd], F_SETFL, inputFlags) < 0) {
    std::string theError = strerror(errno);
    logBlock << logger::purple << "Failed to fcntl pipe end with file descriptor: "
    << this->pipeEnds[whichEnd] << ": "
    << theError << ". " << logger::endL;
    if (++ counter > 100) {
      if (restartServerOnFail) {
        theWebServer.StopKillAll(false);
      } else if (!dontCrashOnFail) {
        crash << "fcntl failed more than 100 times on "
        << "file desciptor: " << this->pipeEnds[whichEnd] << ": " << theError << ". " << crash;
      }
      return false;
    }
  }
  return true;
}

bool PipePrimitive::SetPipeReadNonBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeReadNonBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(O_NONBLOCK, 0, restartServerOnFail, dontCrashOnFail);
  if (result) {
    this->flagReadEndBlocks = false;
  }
  return result;
}

bool PipePrimitive::SetPipeReadBlockingModeIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeReadBlockingModeIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(0, 0, restartServerOnFail, dontCrashOnFail);
  if (result) {
    this->flagReadEndBlocks = true;
  }
  return result;
}

bool PipePrimitive::SetPipeWriteNonBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeWriteNonBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(O_NONBLOCK, 1, restartServerOnFail, dontCrashOnFail);
  if (result) {
    this->flagWriteEndBlocks = false;
  }
  return result;
}

bool PipePrimitive::SetPipeWriteBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeWriteBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(0, 1, restartServerOnFail, dontCrashOnFail);
  if (result) {
    this->flagWriteEndBlocks = true;
  }
  return result;
}

int Pipe::WriteNoInterrupts(int theFD, const std::string& input) {
  int numAttempts = 0;
  for (;;) {
    int result = static_cast<int>(write(theFD, input.c_str(), input.size()));
    if (result >= 0) {
      return result;
    }
    if (result < 0) {
      if (errno == EINTR) {
        logBlock << logger::red << "Write operation interrupted, repeating. " << logger::endL;
        numAttempts ++;
        if (numAttempts > 100) {
          logBlock << logger::red
          << "Write operation interrupted, more than 100 times, this is not supposed to happen. "
          << logger::endL;
          return - 1;
        }
        continue;
      }
    }
    return result;
  }
  //  return - 1;
}

void Pipe::ReadLoop() {
  int implementReadLoop;
}

void Pipe::WriteLoopAfterEmptyingBlocking(const std::string& toBeSent) {
  MacroRegisterFunctionWithName("Pipe::WriteLoopAfterEmptyingBlocking");
  if (
    !this->thePipe.flagReadEndBlocks || !this->thePipe.flagWriteEndBlocks
  ) {
    crash
    << "Pipe write loop requires that both read and write end be blocking. "
    << crash;
  }
  MutexLockGuard safety(theGlobalVariables.MutexWebWorkerPipeWriteLock);
  this->theMutexPipe.RequestPausePauseIfLocked(false, true);
  List<unsigned char> sizeFourBytes;
  Serialization::WriteFourByteUnsigned(static_cast<unsigned>(toBeSent.size()), sizeFourBytes);
  std::string toBeSentString = Serialization::ConvertListUnsignedCharsToString(toBeSent);
  this->metaData.WriteOnceAfterEmptying(toBeSentString, false, true);
  unsigned totalSent = 0;
  while (totalSent < toBeSent.size()) {
    if (!this->thePipe.WriteOnceIfFailThenCrash(toBeSent, static_cast<signed>(totalSent), false, true)) {
      logIO << logger::red << "Failed to write on pipe: " << this->ToString();
      break;
    }
    totalSent += static_cast<unsigned>(this->thePipe.numberOfBytesLastWrite);
  }
  this->theMutexPipe.ResumePausedProcessesIfAny(false, true);
}

void Pipe::WriteOnceAfterEmptying(const std::string& toBeSent, bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::WriteOnceAfterEmptying");
  MutexLockGuard safety(theGlobalVariables.MutexWebWorkerPipeWriteLock);
  this->theMutexPipe.RequestPausePauseIfLocked(restartServerOnFail, dontCrashOnFail);
  this->thePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  this->thePipe.lastRead.SetSize(0);
  this->thePipe.WriteOnceIfFailThenCrash(toBeSent, 0, restartServerOnFail, dontCrashOnFail);
  this->theMutexPipe.ResumePausedProcessesIfAny(restartServerOnFail, dontCrashOnFail);
}

bool PipePrimitive::ReadOnceWithoutEmptying(bool restartServerOnFail, bool dontCrashOnFail) {
  if (this->flagReadEndBlocks || this->flagWriteEndBlocks) {
    crash << this->ToString() << ": read without emptying allowed only on fully non-blocking pipes. " << crash;
  }
  if (!this->ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  return this->WriteOnceIfFailThenCrash(this->GetLastRead(), 0, restartServerOnFail, dontCrashOnFail);
}

bool PipePrimitive::WriteOnceAfterEmptying(const std::string& input, bool restartServerOnFail, bool dontCrashOnFail) {
  if (this->flagReadEndBlocks) {
    crash << this->ToString() << ": write after emptying allowed only on non-blocking read pipes. " << crash;
  }
  if (!this->ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail)) {
    return false;
  }
  return this->WriteOnceIfFailThenCrash(input, 0, restartServerOnFail, dontCrashOnFail);
}

bool PipePrimitive::HandleFailedWriteReturnFalse(
  const std::string& toBeSent, bool restartServerOnFail, bool dontCrashOnFail, int numBadAttempts
) {
  std::stringstream errorStream;
  errorStream << "Failed write: " << toBeSent.size() << " bytes ["
  << StringRoutines::StringShortenInsertDots(toBeSent, 200) << "] onto: " << this->ToString() << numBadAttempts
  << " or more times. Last error: "
  << strerror(errno) << ". ";
  if (theGlobalVariables.processType == ProcessTypes::worker) {
    logIO << logger::red << errorStream.str() << logger::endL;
  }
  if (theGlobalVariables.processType == ProcessTypes::server) {
    logServer << logger::red << errorStream.str() << logger::endL;
  }
  if (restartServerOnFail) {
    theWebServer.StopKillAll(false);
  } else if (!dontCrashOnFail) {
    crash << "Failed write on: " << this->ToString() << numBadAttempts << " or more times. Last error: "
    << strerror(errno) << ". " << crash;
  }
  return false;
}

bool PipePrimitive::WriteOnceIfFailThenCrash(
  const std::string& toBeSent,
  int offset,
  bool restartServerOnFail,
  bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("PipePrimitive::WriteIfFailThenCrash");
  if (this->pipeEnds[1] == - 1) {
    logIO << logger::yellow << "WARNING: " << this->ToString()
    << " writing on non-initialized pipe. ";
    return false;
  }
  if (static_cast<unsigned>(offset) >= toBeSent.size() || offset < 0) {
    crash << "Invalid offset: " << offset << "; toBeSent string has size: " << toBeSent.size() << ". ";
  }
  unsigned remaining = static_cast<unsigned>(toBeSent.size() - static_cast<unsigned>(offset));
  if (remaining == 0) {
    return true;
  }
  int maximumBadAttempts = 30;
  this->numberOfBytesLastWrite = 0;
  int numBadAttempts = 0;
  for (;;) {
    this->numberOfBytesLastWrite = static_cast<int>(write(
      this->pipeEnds[1],
      &toBeSent[static_cast<unsigned>(offset)],
      remaining
    ));
    if (this->numberOfBytesLastWrite < 0) {
      if (
        errno == EAI_AGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EIO
      ) {
        numBadAttempts ++;
        if (numBadAttempts > maximumBadAttempts) {
          return this->HandleFailedWriteReturnFalse(
            toBeSent, restartServerOnFail, dontCrashOnFail, numBadAttempts
          );
        }
        continue;
      }
    }
    break;
  }
  if (this->numberOfBytesLastWrite < 0) {
    return false;
  }
  if (static_cast<unsigned>(this->numberOfBytesLastWrite) < remaining) {
    if (theGlobalVariables.processType == ProcessTypes::worker) {
      logIO << logger::red << this->ToString() << ": wrote only "
      << this->numberOfBytesLastWrite << " bytes out of " << remaining << " total. " << logger::endL;
    }
    if (theGlobalVariables.processType == ProcessTypes::server) {
      logServer << logger::red << this->ToString() << ": wrote only "
      << this->numberOfBytesLastWrite << " bytes out of " << remaining << " total. " << logger::endL;
    }
  }
  return true;
}

std::string Pipe::ToString() const {
  std::stringstream out;
  if (this->name != "") {
    out << this->name << ": information pipe: ";
  }
  out << this->thePipe.ToString();
  out << " mutex data: " << this->theMutexPipe.ToString();
  return out.str();
}

bool Pipe::ResetNoAllocation() {
  if (this->theMutexPipe.ResetNoAllocation() && this->thePipe.ReadOnceIfFailThenCrash(false, true)) {
    return true;
  }
  logIO << logger::red << this->ToString() << ": failed to reset without allocation. " << logger::endL;
  return false;
}

bool Pipe::CreateMe(const std::string& inputPipeName) {
  this->CheckConsistency();
  this->Release();
  this->name = inputPipeName;
  if (!this->thePipe.CreateMe("pipe[" + inputPipeName + "]", false, false, false, true)) {
    this->Release();
    return false;
  }
  if (!this->theMutexPipe.CreateMe("mutex[" + inputPipeName + "]", false, true)) {
    this->Release();
    return false;
  }
  if (!this->metaData.CreateMe("metaData[" + inputPipeName + "]", true, false, false, true)) {
    this->Release();
    return false;
  }
  return true;
}

bool Pipe::CheckConsistency() {
  MacroRegisterFunctionWithName("Pipe::CheckConsistency");
  if (this->flagDeallocated) {
    crash << "This is a programming error: use after free of pipe. " << crash;
  }
  return true;
}

Pipe::~Pipe() {
  // Pipes are not allowed to release resources in the destructor:
  // a pipe's destructor is called when expanding List<Pipe>.
  this->flagDeallocated = true;
}

Pipe::Pipe() {
  this->flagDeallocated = false;
}

void Pipe::Release() {
  this->CheckConsistency();
  this->theMutexPipe.Release();
  this->thePipe.Release();
  this->metaData.Release();
}

bool PipePrimitive::ReadOnceIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("PipePrimitive::ReadOnceIfFailThenCrash");
  this->CheckConsistency();
  this->lastRead.SetSize(0);
  if (this->pipeEnds[0] == - 1) {
    return false;
  }
  int counter = 0;
  const unsigned int bufferSize = 200000;
  this->buffer.SetSize(bufferSize); // <-once the buffer is resized, this operation does no memory allocation and is fast.
  int numReadBytes = 0;
  for (;;) {
    numReadBytes = static_cast<int>(read(this->pipeEnds[0], this->buffer.TheObjects, bufferSize));
    if (numReadBytes >= 0) {
      break;
    }
    counter ++;
    if (counter > 100) {
      logIO << logger::red << this->ToString()
      << ": more than 100 iterations of read resulted in an error. "
      << logger::endL;
      if (restartServerOnFail) {
        theWebServer.StopKillAll(false);
      } else if (!dontCrashOnFail) {
        crash << this->ToString() << ": more than 100 iterations of read resulted in an error. " << crash;
      }
      return false;
    }
    if (errno == EWOULDBLOCK) {
      return true;
    }
  }
  if (numReadBytes > 150000) {
    logIO << logger::red << this->ToString()
    << "This is not supposed to happen: pipe read more than 150000 bytes. " << logger::endL;
  }
  if (numReadBytes > 0) {
    this->buffer.SetSize(numReadBytes);
    this->lastRead = this->buffer;
  }
  return true;
}

void Pipe::ReadOnceWithoutEmptying(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::ReadOnceWithoutEmptying");
  MutexRecursiveWrapper& safetyFirst = theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); // Prevent threads from locking one another
  this->theMutexPipe.RequestPausePauseIfLocked(restartServerOnFail, dontCrashOnFail);
  this->thePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  if (this->thePipe.lastRead.size > 0) {
    std::string tempS;
    tempS = this->GetLastRead();
    this->thePipe.WriteOnceIfFailThenCrash(tempS, 0, restartServerOnFail, dontCrashOnFail);
  }
  this->theMutexPipe.ResumePausedProcessesIfAny(restartServerOnFail, dontCrashOnFail);
  safetyFirst.UnlockMe();
}

void Pipe::ReadOnce(bool restartServerOnFail, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::ReadOnce");
  this->CheckConsistency();
  MutexRecursiveWrapper& safetyFirst = theGlobalVariables.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); // Prevent threads from locking one another.
  this->theMutexPipe.RequestPausePauseIfLocked(restartServerOnFail, dontCrashOnFail);
  this->thePipe.ReadOnceIfFailThenCrash(restartServerOnFail, dontCrashOnFail);
  this->theMutexPipe.ResumePausedProcessesIfAny(restartServerOnFail, dontCrashOnFail);
  safetyFirst.UnlockMe(); // Prevent threads from locking one another.
}

logger::logger(
  const std::string& logFileName,
  logger* inputCarbonCopy,
  bool inputResetLogWhenTooLarge,
  const std::string& inputProcessType
) {
  this->flagInitialized = false;
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

void logger::initializeIfNeeded() {
  if (!this->flagInitialized) {
    this->reset();
  }
}

void logger::reset() {
  this->flagInitialized = true;
  this->currentColor = logger::normalColor;
  this->flagTagColorHtmlOpened = false;
  this->flagTagColorConsoleOpened = false;
  this->flagStopWritingToFile = false;
  this->MaxLogSize = //10000
  50000000;
  if (theGlobalVariables.flagRunningApache || this->theFileName == "") {
    this->flagStopWritingToFile = true;
    return;
  }
  if (this->theFile.is_open()) {
    this->theFile.close();
  }
  FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    this->theFile, this->theFileName, false, true, false, true
  );
  if (!this->theFile.is_open()) {
    this->currentColor = logger::red;
    std::string computedFileName;
    FileOperations::GetPhysicalFileNameFromVirtual(
      this->theFileName, computedFileName, true, false, nullptr
    );
    std::cout << this->openTagConsole() << "Could not open log file with virtual name: "
    << this->theFileName << " and computed name: "
    << computedFileName << this->closeTagConsole()
    << std::endl;
  }
  this->theFile.seekg(0);
  this->theFile.seekp(0);
}

void logger::CheckLogSize() {
  this->initializeIfNeeded();
  if (theFile.tellp() > this->MaxLogSize) {
    if (this->flagResetLogFileWhenTooLarge) {
      this->reset();
      if (this->MaxLogSize > 1000) {
        *this << logger::endL << "Log file reset. " << logger::endL;
      }
    } else {
      this->flagStopWritingToFile = true;
    }
  }
}

void logger::flush() {
  this->initializeIfNeeded();
  std::cout.flush();
  theFile.flush();
  theFile.clear();
}

std::string logger::consoleBlue() {
  return "\x1b[1;34m";
}

std::string logger::consoleYellow() {
  return "\x1b[1;33m";
}

std::string logger::consoleGreen() {
  return "\x1b[1;32m";
}

std::string logger::consoleRed() {
  return "\x1b[1;31m";
}

std::string logger::consoleNormal() {
  return "\x1b[0m";
}

std::string logger::closeTagConsole() {
  if (!this->flagTagColorConsoleOpened) {
    return "";
  }
  this->flagTagColorConsoleOpened = false;
  return "\x1b[0m";
}

std::string logger::closeTagHtml() {
  if (!this->flagTagColorHtmlOpened) {
    return "";
  }
  this->flagTagColorHtmlOpened = false;
  return "</span>";
}

std::string logger::openTagConsole() {
  std::stringstream out;
  out << logger::closeTagConsole();
  switch (this->currentColor) {
    case logger::red:
      out << "\x1b[1;31m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::green:
      out << "\x1b[1;32m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::yellow:
      out << "\x1b[1;33m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::blue:
      out << "\x1b[1;34m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::purple:
      out << "\x1b[1;35m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::cyan:
      out << "\x1b[1;36m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case logger::orange:
      out << "\x1b[0;33m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    default:
      return out.str();
  }
}

std::string logger::openTagHtml() {
  std::stringstream out;
  out << logger::closeTagHtml();
  switch (this->currentColor) {
    case logger::red:
      out << "<span style =\"color:red\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::green:
      out << "<span style =\"color:green\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::blue:
      out << "<span style =\"color:blue\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::yellow:
      out << "<span style =\"color:yellow\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::orange:
      out << "<span style =\"color:orange\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case logger::purple:
      out << "<span style =\"color:purple\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    default:
      return out.str();
  }
}

std::string logger::getStampShort() {
  std::stringstream out;
  out << "[" << theGlobalVariables.processType << ", ";
  // out << "||DEBUG: " << this->theFileName << "||";
  if (theWebServer.activeWorker != - 1) {
    out << "w: " << theWebServer.activeWorker << ", ";
  }
  out << "c: " << theWebServer.NumConnectionsSoFar;
  if (theWebServer.activeWorker != -1) {
    out << "." << theWebServer.GetActiveWorker().numberOfReceivesCurrentConnection;
  }
  out << "] ";
  //<-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

std::string logger::getStamp() {
  std::stringstream out;
  out << theGlobalVariables.processType << ", ";
  out
  << theGlobalVariables.GetDateForLogFiles() << ", "
  << theGlobalVariables.GetElapsedSeconds() << " s, ";
  if (theWebServer.activeWorker != - 1) {
    out << "w: " << theWebServer.activeWorker << ",";
  }
  out << " c: " << theWebServer.NumConnectionsSoFar << ". ";
  //<-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

void logger::StringHighligher::reset() {
  this->sections.SetSize(0);
}

logger::StringHighligher::Section::Section(int inputLength) {
  this->theType = "";
  this->length = inputLength;
}

logger::StringHighligher::Section::Section(const std::string& input) {
  this->theType = input;
  this->length = 0;
}

logger::StringHighligher::Section::Section() {
  this->theType = "";
  this->length = 0;
}

logger::StringHighligher::StringHighligher() {
}

logger::StringHighligher::StringHighligher(const std::string& input) {
  List<char> delimiters;
  delimiters.AddOnTop(',');
  delimiters.AddOnTop('(');
  delimiters.AddOnTop(')');
  delimiters.AddOnTop('[');
  delimiters.AddOnTop(']');
  List<std::string> inputStrings;
  StringRoutines::StringSplitExcludeDelimiters(input, delimiters, inputStrings);
  for (int i = 0; i < inputStrings.size; i ++) {
    std::string current = StringRoutines::StringTrimWhiteSpace(inputStrings[i]);
    logger::StringHighligher::Section incoming;
    if (current == "|") {
      incoming.theType = "|";
      this->sections.AddOnTop(incoming);
      continue;
    }
    if (current == "||") {
      incoming.theType = "||";
      this->sections.AddOnTop(incoming);
      continue;
    }
    incoming.theType = "";
    LargeInteger theLI;
    theLI.AssignString(current);
    if (!theLI.IsIntegerFittingInInt(&incoming.length)) {
      crash << "StringHighligher is not allowed to fail: this is an internal function, "
      << "please do not expose to the outside world. " << crash;
    }
    this->sections.AddOnTop(incoming);
  }
}

void MathRoutines::ParseListIntCrashOnFailure(const std::string& input, List<int>& result) {
  bool success = MathRoutines::ParseListInt(input, result, nullptr);
  if (!success) {
    crash << "Failed to parse list int with a function that does not allow failure. " << crash;
  }
}

bool MathRoutines::ParseListInt(
  const std::string& input, List<int>& result, std::stringstream* commentsOnFailure
) {
  List<char> delimiters;
  delimiters.AddOnTopNoRepetition('\n');
  delimiters.AddOnTopNoRepetition(',');
  delimiters.AddOnTopNoRepetition('[');
  delimiters.AddOnTopNoRepetition(']');
  delimiters.AddOnTopNoRepetition('(');
  delimiters.AddOnTopNoRepetition(')');
  List<std::string> theNumbers;
  StringRoutines::StringSplitExcludeDelimiters(input, delimiters, theNumbers);
  result.SetSize(theNumbers.size);
  for (int i = 0; i < theNumbers.size; i ++) {
    LargeInteger theInt;
    bool success = theInt.AssignStringFailureAllowed(theNumbers[i], commentsOnFailure);
    if (!success) {
      return false;
    }
    if (!theInt.IsIntegerFittingInInt(&result[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Integer at position " << i << " is too large. ";
      }
      result.SetSize(0);
      return false;
    }
  }
  return true;
}

logger& logger::operator<<(const logger::StringHighligher& input) {
  this->nextHighlighter.sections = input.sections;
  return *this;
}

logger& logger::operator<<(const std::string& input) {
  if (input.size() == 0) {
    return *this;
  }
  if (this->nextHighlighter.sections.size == 0) {
    return this->doTheLogging(input);
  }
  int indexLastNonShownByte = 0;
  List<std::string> chunks;
  for (
    int i = 0;
    i < this->nextHighlighter.sections.size && indexLastNonShownByte < static_cast<signed>(input.size());
    i ++
  ) {
    logger::StringHighligher::Section& currentSection = this->nextHighlighter.sections[i];
    if (currentSection.theType != "") {
      chunks.AddOnTop(currentSection.theType);
      continue;
    }
    int nextSectionLength = currentSection.length;
    if (indexLastNonShownByte + nextSectionLength > static_cast<signed>(input.size())) {
      nextSectionLength = static_cast<int>(input.size()) - indexLastNonShownByte;
    }
    std::string current = input.substr(
      static_cast<unsigned>(indexLastNonShownByte),
      static_cast<unsigned>(nextSectionLength)
    );
    chunks.AddOnTop(current);
    indexLastNonShownByte += nextSectionLength;
  }
  if (indexLastNonShownByte < static_cast<signed>(input.size())) {
    chunks.AddOnTop(input.substr(static_cast<unsigned>(indexLastNonShownByte)));
  }
  for (int i = 0; i < chunks.size; i ++) {
    int colorIndex = i % 3;
    switch(colorIndex) {
    case 0:
      *this << logger::blue;
      break;
    case 1:
      *this << logger::red;
      break;
    case 2:
      *this << logger::green;
      break;
    default:
      break;
    }
    this->doTheLogging(chunks[i]);
  }
  this->nextHighlighter.reset();
  return *this;
}

logger& logger::operator<<(const loggerSpecialSymbols& input) {
  if (theGlobalVariables.flagRunningApache) {
    return *this;
  }
  this->initializeIfNeeded();
  this->CheckLogSize();
  bool doUseColors = theGlobalVariables.flagRunningBuiltInWebServer || theGlobalVariables.flagRunningCommandLine;
  switch (input) {
    case logger::endL:
      std::cout << this->getStampShort() << this->bufferStandardOutput;
      this->bufferStandardOutput.clear();
      if (doUseColors) {
        std::cout << this->closeTagConsole() << std::endl;
      }
      std::cout.flush();
      this->currentColor = logger::normalColor;
      if (this->flagStopWritingToFile) {
        return *this;
      }
      this->bufferFile += this->closeTagHtml() + "\n<br>\n";
      theFile << this->getStamp() << this->bufferFile;
      this->bufferFile = "";
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
      if (doUseColors) {
        if (this->flagWriteImmediately) {
          std::cout << this->openTagConsole();
        } else {
          this->bufferStandardOutput += this->openTagConsole();
        }
      }
      if (this->flagStopWritingToFile) {
        return *this;
      }
      this->theFile << this->openTagHtml();
      return *this;
    case logger::normalColor:
      if (doUseColors) {
        if (this->flagWriteImmediately) {
          std::cout << this->closeTagConsole();
        } else {
          this->bufferStandardOutput += this->closeTagConsole();
        }
      }
      this->currentColor = logger::normalColor;
      if (this->flagStopWritingToFile) {
        return *this;
      }
      theFile << this->closeTagHtml();
      return *this;
    // default:
    //   ;
  }
  return *this;
}
