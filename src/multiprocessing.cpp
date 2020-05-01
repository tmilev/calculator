// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "multiprocessing.h"
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
#include <unistd.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include "webserver.h"

std::string MutexProcess::currentProcessName = "S: ";

void MutexProcess::Release(int& theDescriptor) {
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

std::string PipePrimitive::toString() const {
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
  MutexProcess::Release(this->pipeEnds[0]);
  MutexProcess::Release(this->pipeEnds[1]);
  this->numberOfBytesLastWrite = 0;
  this->lastRead.setSize(0);
}

std::string MutexProcess::lockContent = "!";

void MutexProcess::Release() {
  this->lockPipe.Release();
  this->name = "";
}

bool MutexProcess::checkConsistency() {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of " << this->toString() << global.fatal;
  }
  return true;
}

MutexProcess::MutexProcess() {
  this->flagDeallocated = false;
  this->flagLockHeldByAnotherThread = false;
}

MutexProcess::~MutexProcess() {
  this->flagDeallocated = true;
}

std::string PipePrimitive::GetLastRead() {
  MacroRegisterFunctionWithName("PipePrimitive::GetLastRead");
  std::string result(this->lastRead.theObjects, static_cast<unsigned>(this->lastRead.size));
  return result;
}

bool PipePrimitive::checkConsistency() {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of pipe: " << this->toString() << global.fatal;
  }
  return true;
}

bool PipePrimitive::CreateMe(
  const std::string& inputPipeName,
  bool readEndBlocks,
  bool writeEndBlocks,
  bool dontCrashOnFail
) {
  this->name = inputPipeName;
  if (pipe(this->pipeEnds.theObjects) < 0) {
    global << Logger::red << "FAILED to create pipe: " << this->name << ". " << Logger::endL;
    this->Release();
    global.server().StopKillAll();
    // return false;
  }
  if (!readEndBlocks) {
    if (!this->SetReadNonBlocking(dontCrashOnFail)) {
      this->Release();
      return false;
    }
  }
  if (!writeEndBlocks) {
    if (!this->SetPipeWriteNonBlockingIfFailThenCrash(dontCrashOnFail)) {
      this->Release();
      return false;
    }
  }
  return true;
}

bool MutexProcess::CreateMe(
  const std::string& inputName,
  bool dontCrashOnFail
) {
  this->Release();
  this->name = inputName;
  if (!this->lockPipe.CreateMe(inputName + "lockPipe", true, false, dontCrashOnFail)) {
    return false;
  }
  this->ResetNoAllocation();
  return true;
}

bool MutexProcess::ResetNoAllocation() {
  if (
    this->lockPipe.WriteOnceIfFailThenCrash(MutexProcess::lockContent, 0, true)
  ) {
    return true;
  }
  global << Logger::red << this->toString() << ": failed to reset without allocation. " << Logger::endL;
  return false;
}

bool MutexProcess::Lock() {
  this->checkConsistency();
  if (this->flagLockHeldByAnotherThread) {
    global.fatal << "MutexProcess about to deadlock itself. " << global.fatal;
  }
  MutexLockGuard guard(this->lockThreads.getElement()); // <- lock out other threads
  this->flagLockHeldByAnotherThread = true;
  bool success = this->lockPipe.ReadOnceIfFailThenCrash(true);
  if (!success) {
    global << Logger::red << "Error: " << this->currentProcessName
    << " failed to lock pipe " << this->toString() << Logger::endL;
  }
  this->flagLockHeldByAnotherThread = false;
  return success;
}

bool MutexProcess::Unlock() {
  MacroRegisterFunctionWithName("MutexProcess::Unlock");
  bool success = this->lockPipe.WriteOnceIfFailThenCrash(MutexProcess::lockContent, 0, true);
  if (!success) {
    global << Logger::red << "Error: " << this->currentProcessName
    << " failed to unlock pipe " << this->toString() << Logger::endL;
  }
  return success;
}

std::string MutexProcess::toString() const {
  std::stringstream out;
  if (this->name != "") {
    out << this->name << ": ";
  }
  out << "mutexProcess: " << this->lockPipe.toString();
  return out.str();
}

int Pipe::WriteWithTimeoutViaSelect(
  int theFD,
  const std::string& input,
  int timeOutInSeconds,
  int maxNumTries,
  std::stringstream* commentsOnFailure
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
      failStream << maxNumTries
      << " failed or timed-out select attempts on file descriptor: " << theFD;
      break;
    }
    numSelected = select(theFD + 1, nullptr, &theFDcontainer, nullptr, &timeOut);
    failStream << "While select-writing on file descriptor: "
    << theFD << ", select failed. Error message: "
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
      output.theObjects,
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

bool PipePrimitive::SetPipeFlagsIfFailThenCrash(int inputFlags, int whichEnd, bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeBlockingModeCrashIfFail");
  int counter = 0;
  while (fcntl(this->pipeEnds[whichEnd], F_SETFL, inputFlags) < 0) {
    std::string theError = strerror(errno);
    global << Logger::purple << "Failed to fcntl pipe end with file descriptor: "
    << this->pipeEnds[whichEnd] << ": "
    << theError << ". " << Logger::endL;
    if (++ counter > 100) {
      if (!dontCrashOnFail) {
        global.fatal << "fcntl failed more than 100 times on "
        << "file desciptor: " << this->pipeEnds[whichEnd] << ": " << theError << ". " << global.fatal;
      }
      return false;
    }
  }
  return true;
}

bool PipePrimitive::SetReadNonBlocking(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeReadNonBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(O_NONBLOCK, 0, dontCrashOnFail);
  if (result) {
    this->flagReadEndBlocks = false;
  }
  return result;
}

bool PipePrimitive::SetReadBlocking(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeReadBlockingModeIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(0, 0, dontCrashOnFail);
  if (result) {
    this->flagReadEndBlocks = true;
  }
  return result;
}

bool PipePrimitive::SetPipeWriteNonBlockingIfFailThenCrash(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeWriteNonBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(O_NONBLOCK, 1, dontCrashOnFail);
  if (result) {
    this->flagWriteEndBlocks = false;
  }
  return result;
}

bool PipePrimitive::SetWriteBlocking(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeWriteBlockingIfFailThenCrash");
  bool result = this->SetPipeFlagsIfFailThenCrash(0, 1, dontCrashOnFail);
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
        global << Logger::red << "Write operation interrupted, repeating. " << Logger::endL;
        numAttempts ++;
        if (numAttempts > 100) {
          global << Logger::red
          << "Write operation interrupted, more than 100 times, this is not supposed to happen. "
          << Logger::endL;
          return - 1;
        }
        continue;
      }
    }
    return result;
  }
  //  return - 1;
}

void Pipe::ReadLoop(List<char>& output) {
  MacroRegisterFunctionWithName("Pipe::ReadLoop");
  this->checkConsistency();
  MutexRecursiveWrapper& safetyFirst = global.MutexWebWorkerPipeReadLock;
  safetyFirst.LockMe(); // Prevent threads from locking one another.
  this->metaData.lastRead.setSize(0);
  this->metaData.ReadOnceIfFailThenCrash(true);
  int expectedBytes = 0;
  int offset = 0;
  List<unsigned char> metaDataBuffer;
  metaDataBuffer = this->metaData.lastRead;
  Serialization::ReadFourByteInt(
    metaDataBuffer, offset, expectedBytes, nullptr
  );
  output.setSize(0);
  while (output.size < expectedBytes) {
    this->thePipe.ReadOnceIfFailThenCrash(true);
    output.addListOnTop(this->thePipe.lastRead);
  }
  safetyFirst.UnlockMe(); // Prevent threads from locking one another.
}

void Pipe::WriteOnceAfterEmptying(
  const std::string& toBeSent, bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("Pipe::WriteOnceAfterEmptying");
  MutexLockGuard safety(global.MutexWebWorkerPipeWriteLock);
  this->theMutexPipe.Lock();
  this->thePipe.ReadOnceIfFailThenCrash(dontCrashOnFail);
  this->thePipe.lastRead.setSize(0);
  this->thePipe.WriteOnceIfFailThenCrash(toBeSent, 0, dontCrashOnFail);
  this->theMutexPipe.Unlock();
}

bool PipePrimitive::ReadOnceWithoutEmptying(bool dontCrashOnFail) {
  if (this->flagReadEndBlocks || this->flagWriteEndBlocks) {
    global.fatal << this->toString() << ": read without emptying allowed only on fully non-blocking pipes. " << global.fatal;
  }
  if (!this->ReadOnceIfFailThenCrash(dontCrashOnFail)) {
    return false;
  }
  return this->WriteOnceIfFailThenCrash(this->GetLastRead(), 0, dontCrashOnFail);
}

bool PipePrimitive::WriteOnceAfterEmptying(const std::string& input, bool dontCrashOnFail
) {
  if (this->flagReadEndBlocks) {
    global.fatal << this->toString() << ": write after emptying allowed only on non-blocking read pipes. " << global.fatal;
  }
  if (!this->ReadOnceIfFailThenCrash(dontCrashOnFail)) {
    return false;
  }
  return this->WriteOnceIfFailThenCrash(input, 0, dontCrashOnFail);
}

bool PipePrimitive::HandleFailedWriteReturnFalse(
  const std::string& toBeSent, bool dontCrashOnFail, int numBadAttempts
) {
  std::stringstream errorStream;
  errorStream << "Failed write: " << toBeSent.size() << " bytes ["
  << StringRoutines::StringShortenInsertDots(toBeSent, 200) << "] onto: " << this->toString() << numBadAttempts
  << " or more times. Last error: "
  << strerror(errno) << ". ";
  global << Logger::red << errorStream.str() << Logger::endL;
  if (!dontCrashOnFail) {
    global.fatal << "Failed write on: " << this->toString() << numBadAttempts << " or more times. Last error: "
    << strerror(errno) << ". " << global.fatal;
  }
  return false;
}

bool PipePrimitive::WriteOnceIfFailThenCrash(
  const std::string& toBeSent,
  int offset,
  bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("PipePrimitive::WriteIfFailThenCrash");
  if (this->pipeEnds[1] == - 1) {
    global << Logger::yellow << "WARNING: " << this->toString()
    << " writing on non-initialized pipe. ";
    return false;
  }
  if (static_cast<unsigned>(offset) >= toBeSent.size() || offset < 0) {
    global.fatal << "Invalid offset: " << offset << "; toBeSent string has size: " << toBeSent.size() << ". ";
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
            toBeSent, dontCrashOnFail, numBadAttempts
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
    global << Logger::red << this->toString() << ": wrote only "
    << this->numberOfBytesLastWrite << " bytes out of " << remaining << " total. " << Logger::endL;
  }
  return true;
}

std::string Pipe::toString() const {
  std::stringstream out;
  if (this->name != "") {
    out << this->name << ": information pipe: ";
  }
  out << this->thePipe.toString();
  out << " mutex data: " << this->theMutexPipe.toString();
  return out.str();
}

bool Pipe::ResetNoAllocation() {
  bool oldReadEndBlocks = this->thePipe.flagReadEndBlocks;
  if (oldReadEndBlocks) {
    if (!this->thePipe.SetReadNonBlocking(true)) {
      return false;
    }
  }
  this->thePipe.ReadOnceIfFailThenCrash(true);
  if (oldReadEndBlocks) {
    if (!this->thePipe.SetReadBlocking(true)) {
      return false;
    }
  }
  if (
    this->metaData.ReadOnceIfFailThenCrash(true) &&
    this->theMutexPipe.ResetNoAllocation()
  ) {
    return true;
  }
  global << Logger::red << this->toString() << ": failed to reset without allocation. " << Logger::endL;
  return false;
}

bool Pipe::CreateMe(const std::string& inputPipeName) {
  this->checkConsistency();
  this->Release();
  this->name = inputPipeName;
  if (!this->thePipe.CreateMe("pipe[" + inputPipeName + "]", false, false, true)) {
    this->Release();
    return false;
  }
  if (!this->theMutexPipe.CreateMe("mutex[" + inputPipeName + "]", true)) {
    this->Release();
    return false;
  }
  if (!this->metaData.CreateMe("metaData[" + inputPipeName + "]", false, false, true)) {
    this->Release();
    return false;
  }
  return true;
}

bool Pipe::checkConsistency() {
  MacroRegisterFunctionWithName("Pipe::checkConsistency");
  if (this->flagDeallocated) {
    global.fatal << "This is a programming error: use after free of pipe. " << global.fatal;
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
  this->checkConsistency();
  this->theMutexPipe.Release();
  this->thePipe.Release();
  this->metaData.Release();
}

bool PipePrimitive::ReadOnceIfFailThenCrash(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("PipePrimitive::ReadOnceIfFailThenCrash");
  this->checkConsistency();
  this->lastRead.setSize(0);
  if (this->pipeEnds[0] == - 1) {
    return false;
  }
  int counter = 0;
  const unsigned int bufferSize = 200000;
  this->buffer.setSize(bufferSize); // <-once the buffer is resized, this operation does no memory allocation and is fast.
  int numReadBytes = 0;
  for (;;) {
    numReadBytes = static_cast<int>(read(this->pipeEnds[0], this->buffer.theObjects, bufferSize));
    if (numReadBytes >= 0) {
      break;
    }
    counter ++;
    if (counter > 100) {
      global << Logger::red << this->toString()
      << ": more than 100 iterations of read resulted in an error. "
      << Logger::endL;
      if (!dontCrashOnFail) {
        global.fatal << this->toString()
        << ": more than 100 iterations of read resulted in an error. "
        << global.fatal;
      }
      return false;
    }
    if (errno == EWOULDBLOCK) {
      return true;
    }
  }
  if (numReadBytes > 150000) {
    global << Logger::red << this->toString()
    << "This is not supposed to happen: pipe read more than 150000 bytes. " << Logger::endL;
  }
  if (numReadBytes > 0) {
    this->buffer.setSize(numReadBytes);
    this->lastRead = this->buffer;
  }
  return true;
}

void Pipe::ReadOnceWithoutEmptying(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::ReadOnceWithoutEmptying");
  MutexRecursiveWrapper& safetyFirst = global.MutexWebWorkerPipeReadLock;
  MutexLockGuard guard (safetyFirst); // guard from other threads.
  this->theMutexPipe.Lock();
  this->thePipe.ReadOnceIfFailThenCrash(dontCrashOnFail);
  if (this->thePipe.lastRead.size > 0) {
    std::string tempS;
    tempS = this->GetLastRead();
    this->thePipe.WriteOnceIfFailThenCrash(tempS, 0, dontCrashOnFail);
  }
  this->theMutexPipe.Unlock();
}

void Pipe::ReadOnce(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::ReadOnce");
  this->checkConsistency();
  MutexRecursiveWrapper& safetyFirst = global.MutexWebWorkerPipeReadLock;
  MutexLockGuard guard(safetyFirst); // guard from other threads.
  MutexProcessLockGuard lock(this->theMutexPipe);
  this->theMutexPipe.Lock();
  this->thePipe.ReadOnceIfFailThenCrash(dontCrashOnFail);
  this->theMutexPipe.Unlock();
}

Logger::Logger() {
  this->flagInitialized = false;
  this->theFileName = "";
  this->currentColor = Logger::normalColor;
  this->flagTagColorHtmlOpened = false;
  this->flagTagColorConsoleOpened = false;
  this->flagStopWritingToFile = true;
  this->flagResetLogFileWhenTooLarge = true;
}

void Logger::initializeIfNeeded() {
  if (!this->flagInitialized) {
    this->reset();
  }
}

void Logger::reset() {
  this->flagInitialized = true;
  this->currentColor = Logger::normalColor;
  this->flagTagColorHtmlOpened = false;
  this->flagTagColorConsoleOpened = false;
  this->flagStopWritingToFile = false;
  this->MaxLogSize = //10000
  50000000;
  if (this->theFileName == "") {
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
    this->currentColor = Logger::red;
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

void Logger::CheckLogSize() {
  this->initializeIfNeeded();
  if (theFile.tellp() > this->MaxLogSize) {
    if (this->flagResetLogFileWhenTooLarge) {
      this->reset();
      if (this->MaxLogSize > 1000) {
        *this << Logger::endL << "Log file reset. " << Logger::endL;
      }
    } else {
      this->flagStopWritingToFile = true;
    }
  }
}

void Logger::flush() {
  this->initializeIfNeeded();
  std::cout.flush();
  theFile.flush();
  theFile.clear();
}

std::string Logger::consoleBlue() {
  return "\x1b[1;34m";
}

std::string Logger::consoleYellow() {
  return "\x1b[1;33m";
}

std::string Logger::consoleGreen() {
  return "\x1b[1;32m";
}

std::string Logger::consoleRed() {
  return "\x1b[1;31m";
}

std::string Logger::consoleNormal() {
  return "\x1b[0m";
}

std::string Logger::closeTagConsole() {
  if (!this->flagTagColorConsoleOpened) {
    return "";
  }
  this->flagTagColorConsoleOpened = false;
  return "\x1b[0m";
}

std::string Logger::closeTagHtml() {
  if (!this->flagTagColorHtmlOpened) {
    return "";
  }
  this->flagTagColorHtmlOpened = false;
  return "</span>";
}

std::string Logger::openTagConsole() {
  std::stringstream out;
  out << Logger::closeTagConsole();
  switch (this->currentColor) {
    case Logger::red:
      out << "\x1b[1;31m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case Logger::green:
      out << "\x1b[1;32m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case Logger::yellow:
      out << "\x1b[1;33m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case Logger::blue:
      out << "\x1b[1;34m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case Logger::purple:
      out << "\x1b[1;35m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case Logger::cyan:
      out << "\x1b[1;36m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    case Logger::orange:
      out << "\x1b[0;33m";
      this->flagTagColorConsoleOpened = true;
      return out.str();
    default:
      return out.str();
  }
}

std::string Logger::openTagHtml() {
  std::stringstream out;
  out << Logger::closeTagHtml();
  switch (this->currentColor) {
    case Logger::red:
      out << "<span style =\"color:red\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case Logger::green:
      out << "<span style =\"color:green\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case Logger::blue:
      out << "<span style =\"color:blue\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case Logger::yellow:
      out << "<span style =\"color:yellow\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case Logger::orange:
      out << "<span style =\"color:orange\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    case Logger::purple:
      out << "<span style =\"color:purple\">";
      this->flagTagColorHtmlOpened = true;
      return out.str();
    default:
      return out.str();
  }
}

std::string Logger::getStampShort() {
  if (global.logs.logType == GlobalVariables::LogData::type::daemon) {
    return "[daemon] ";
  }
  std::stringstream out;
  out << "[" << global.logs.ToStringprocessType() << ", ";
  if (global.server().activeWorker != - 1) {
    out << "w: " << global.server().activeWorker << ", ";
  }
  out << "c: " << global.server().NumConnectionsSoFar;
  if (global.server().activeWorker != -1) {
    out << "." << global.server().getActiveWorker().numberOfReceivesCurrentConnection;
  }
  out << "] ";
  //<-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

std::string Logger::getStamp() {
  std::stringstream out;
  out << global.logs.ToStringprocessType() << ", ";
  out
  << global.GetDateForLogFiles() << ", "
  << global.getElapsedSeconds() << " s, ";
  if (global.server().activeWorker != - 1) {
    out << "w: " << global.server().activeWorker << ",";
  }
  out << " c: " << global.server().NumConnectionsSoFar << ". ";
  //<-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

void Logger::StringHighligher::reset() {
  this->sections.setSize(0);
}

Logger::StringHighligher::Section::Section(int inputLength) {
  this->theType = "";
  this->length = inputLength;
}

Logger::StringHighligher::Section::Section(const std::string& input) {
  this->theType = input;
  this->length = 0;
}

Logger::StringHighligher::Section::Section() {
  this->theType = "";
  this->length = 0;
}

Logger::StringHighligher::StringHighligher() {
}

Logger::StringHighligher::StringHighligher(const std::string& input) {
  List<char> delimiters;
  delimiters.addOnTop(',');
  delimiters.addOnTop('(');
  delimiters.addOnTop(')');
  delimiters.addOnTop('[');
  delimiters.addOnTop(']');
  List<std::string> inputStrings;
  StringRoutines::StringSplitExcludeDelimiters(input, delimiters, inputStrings);
  for (int i = 0; i < inputStrings.size; i ++) {
    std::string current = StringRoutines::stringTrimWhiteSpace(inputStrings[i]);
    Logger::StringHighligher::Section incoming;
    if (current == "|") {
      incoming.theType = "|";
      this->sections.addOnTop(incoming);
      continue;
    }
    if (current == "||") {
      incoming.theType = "||";
      this->sections.addOnTop(incoming);
      continue;
    }
    incoming.theType = "";
    LargeInteger theLI;
    theLI.assignString(current);
    if (!theLI.isIntegerFittingInInt(&incoming.length)) {
      global.fatal << "StringHighligher is not allowed to fail: this is an internal function, "
      << "please do not expose to the outside world. " << global.fatal;
    }
    this->sections.addOnTop(incoming);
  }
}

void MathRoutines::ParseListIntCrashOnFailure(const std::string& input, List<int>& result) {
  bool success = MathRoutines::ParseListInt(input, result, nullptr);
  if (!success) {
    global.fatal << "Failed to parse list int with a function that does not allow failure. " << global.fatal;
  }
}

bool MathRoutines::ParseListInt(
  const std::string& input, List<int>& result, std::stringstream* commentsOnFailure
) {
  List<char> delimiters;
  delimiters.addOnTopNoRepetition('\n');
  delimiters.addOnTopNoRepetition(',');
  delimiters.addOnTopNoRepetition('[');
  delimiters.addOnTopNoRepetition(']');
  delimiters.addOnTopNoRepetition('(');
  delimiters.addOnTopNoRepetition(')');
  List<std::string> theNumbers;
  StringRoutines::StringSplitExcludeDelimiters(input, delimiters, theNumbers);
  result.setSize(theNumbers.size);
  for (int i = 0; i < theNumbers.size; i ++) {
    LargeInteger theInt;
    bool success = theInt.AssignStringFailureAllowed(theNumbers[i], commentsOnFailure);
    if (!success) {
      return false;
    }
    if (!theInt.isIntegerFittingInInt(&result[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Integer at position " << i << " is too large. ";
      }
      result.setSize(0);
      return false;
    }
  }
  return true;
}

Logger& Logger::operator<<(const Logger::StringHighligher& input) {
  this->nextHighlighter.sections = input.sections;
  return *this;
}

Logger& Logger::operator<<(const std::string& input) {
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
    Logger::StringHighligher::Section& currentSection = this->nextHighlighter.sections[i];
    if (currentSection.theType != "") {
      chunks.addOnTop(currentSection.theType);
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
    chunks.addOnTop(current);
    indexLastNonShownByte += nextSectionLength;
  }
  if (indexLastNonShownByte < static_cast<signed>(input.size())) {
    chunks.addOnTop(input.substr(static_cast<unsigned>(indexLastNonShownByte)));
  }
  for (int i = 0; i < chunks.size; i ++) {
    int colorIndex = i % 3;
    switch(colorIndex) {
    case 0:
      *this << Logger::blue;
      break;
    case 1:
      *this << Logger::red;
      break;
    case 2:
      *this << Logger::green;
      break;
    default:
      break;
    }
    this->doTheLogging(chunks[i]);
  }
  this->nextHighlighter.reset();
  return *this;
}

Logger& Logger::operator<<(const loggerSpecialSymbols& input) {
  this->initializeIfNeeded();
  this->CheckLogSize();
  bool doUseColors =
    global.flagRunningBuiltInWebServer ||
    global.flagRunningConsoleRegular ||
    global.flagRunningConsoleTest
  ;
  switch (input) {
    case Logger::endL:
      std::cout << this->getStampShort() << this->bufferStandardOutput;
      this->bufferStandardOutput.clear();
      if (doUseColors) {
        std::cout << this->closeTagConsole() << std::endl;
      }
      std::cout.flush();
      this->currentColor = Logger::normalColor;
      if (this->flagStopWritingToFile) {
        return *this;
      }
      this->bufferFile += this->closeTagHtml() + "\n<br>\n";
      theFile << this->getStamp() << this->bufferFile;
      this->bufferFile = "";
      theFile.flush();
      return *this;
    case Logger::red:
    case Logger::blue:
    case Logger::yellow:
    case Logger::green:
    case Logger::purple:
    case Logger::orange:
    case Logger::cyan:
      this->currentColor = input;
      if (doUseColors) {
        this->bufferStandardOutput += this->openTagConsole();
      }
      if (this->flagStopWritingToFile) {
        return *this;
      }
      this->theFile << this->openTagHtml();
      return *this;
    case Logger::normalColor:
      if (doUseColors) {
        this->bufferStandardOutput += this->closeTagConsole();
      }
      this->currentColor = Logger::normalColor;
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
