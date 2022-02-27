// The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "multiprocessing.h"
#include <fcntl.h> //<- setting of flags for pipes and the like (example: making a pipe non-blocking).
#include <unistd.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <sys/select.h> // <- fdset here.
#include "webserver.h"

std::string MutexProcess::currentProcessName = "S: ";
void MutexProcess::release(int& descriptor) {
  if (descriptor == - 1) {
    return;
  }
  close(descriptor);
  descriptor = - 1;
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
  this->release();
  this->flagDeallocated = true;
}

void PipePrimitive::release() {
  MutexProcess::release(this->pipeEnds[0]);
  MutexProcess::release(this->pipeEnds[1]);
  this->numberOfBytesLastWrite = 0;
  this->lastRead.setSize(0);
}

std::string MutexProcess::lockContent = "!";
void MutexProcess::release() {
  this->lockPipe.release();
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
  this->flaglockHeldByAnotherThread = false;
}

MutexProcess::~MutexProcess() {
  this->flagDeallocated = true;
}

std::string PipePrimitive::getLastRead() {
  MacroRegisterFunctionWithName("PipePrimitive::getLastRead");
  std::string result(
    this->lastRead.objects,
    static_cast<unsigned>(this->lastRead.size)
  );
  return result;
}

bool PipePrimitive::checkConsistency() {
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free of pipe: "
    << this->toString()
    << global.fatal;
  }
  return true;
}

bool PipePrimitive::createMe(
  const std::string& inputPipeName,
  bool readEndBlocks,
  bool writeEndBlocks,
  bool dontCrashOnFail
) {
  this->name = inputPipeName;
  if (pipe(this->pipeEnds.objects) < 0) {
    global
    << Logger::red
    << "FAILED to create pipe: "
    << this->name
    << ". "
    << Logger::endL;
    this->release();
    global.server().stop();
    // return false;
  }
  if (!readEndBlocks) {
    if (!this->setReadNonBlocking(dontCrashOnFail)) {
      this->release();
      return false;
    }
  }
  if (!writeEndBlocks) {
    if (!this->setPipeWriteNonBlockingIfFailThenCrash(dontCrashOnFail)) {
      this->release();
      return false;
    }
  }
  return true;
}

bool MutexProcess::createMe(
  const std::string& inputName, bool dontCrashOnFail
) {
  this->release();
  this->name = inputName;
  if (
    !this->lockPipe.createMe(
      inputName + "lockPipe", true, false, dontCrashOnFail
    )
  ) {
    return false;
  }
  this->resetNoAllocation();
  return true;
}

bool MutexProcess::resetNoAllocation() {
  if (
    this->lockPipe.writeOnceNoFailure(MutexProcess::lockContent, 0, true)
  ) {
    return true;
  }
  global
  << Logger::red
  << this->toString()
  << ": failed to reset without allocation. "
  << Logger::endL;
  return false;
}

bool MutexProcess::lock() {
  this->checkConsistency();
  if (this->flaglockHeldByAnotherThread) {
    global.fatal << "MutexProcess about to deadlock itself. " << global.fatal;
  }
  MutexlockGuard guard(this->lockThreads.getElement());
  // <- lock out other threads
  this->flaglockHeldByAnotherThread = true;
  bool success = this->lockPipe.readOnceNoFailure(true);
  if (!success) {
    global
    << Logger::red
    << "Error: "
    << this->currentProcessName
    << " failed to lock pipe "
    << this->toString()
    << Logger::endL;
  }
  this->flaglockHeldByAnotherThread = false;
  return success;
}

bool MutexProcess::unlock() {
  MacroRegisterFunctionWithName("MutexProcess::unlock");
  bool success =
  this->lockPipe.writeOnceNoFailure(MutexProcess::lockContent, 0, true);
  if (!success) {
    global
    << Logger::red
    << "Error: "
    << this->currentProcessName
    << " failed to unlock pipe "
    << this->toString()
    << Logger::endL;
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

int Pipe::writeWithTimeoutViaSelect(
  int fileDescriptor,
  const std::string& input,
  int timeOutInSeconds,
  int maxNumTries,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Pipe::writeWithTimeoutViaSelect");
  fd_set fileDescriptorContainer;
  FD_ZERO(&fileDescriptorContainer);
  FD_SET(fileDescriptor, &fileDescriptorContainer);
  timeval timeOut;
  timeOut.tv_sec = timeOutInSeconds;
  timeOut.tv_usec = 0;
  int numFails = 0;
  int numSelected = - 1;
  std::stringstream failStream;
  do {
    if (numFails > maxNumTries) {
      failStream
      << maxNumTries
      << " failed or timed-out select attempts on file descriptor: "
      << fileDescriptor;
      break;
    }
    numSelected =
    select(
      fileDescriptor + 1,
      nullptr,
      &fileDescriptorContainer,
      nullptr,
      &timeOut
    );
    failStream
    << "While select-writing on file descriptor: "
    << fileDescriptor
    << ", select failed. Error message: "
    << strerror(errno)
    << ". \n";
    numFails ++;
  } while (numSelected < 0);
  if (numSelected <= 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << failStream.str();
    }
    return - 1;
  }
  return Pipe::writeNoInterrupts(fileDescriptor, input);
}

int Pipe::readWithTimeOutViaSelect(
  int fileDescriptor,
  List<char>& output,
  int timeOutInSeconds,
  int maxNumTries,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Pipe::readWithTimeOutViaSelect");
  if (fileDescriptor < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Attempting to read from a negative file descriptor: "
      << fileDescriptor;
    }
    return - 1;
  }
  fd_set fileDescriptorContainer;
  FD_ZERO(&fileDescriptorContainer);
  FD_SET(fileDescriptor, &fileDescriptorContainer);
  timeval timeOut;
  timeOut.tv_sec = timeOutInSeconds;
  timeOut.tv_usec = 0;
  int numFails = 0;
  int numSelected = - 1;
  std::stringstream failStream;
  do {
    if (numFails > maxNumTries) {
      failStream
      << maxNumTries
      << " failed or timed-out select attempts on file descriptor: "
      << fileDescriptor;
      break;
    }
    numSelected =
    select(
      fileDescriptor + 1,
      &fileDescriptorContainer,
      nullptr,
      nullptr,
      &timeOut
    );
    failStream
    << "While select-reading from file descriptor: "
    << fileDescriptor
    << ", select failed. Error message: "
    << strerror(errno)
    << ". \n";
    numFails ++;
  } while (numSelected < 0);
  // numSelected == 0 most probably means timeout has expired.
  if (numSelected <= 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << failStream.str();
    }
    return - 1;
  }
  int bytesRead = - 1;
  do {
    bytesRead = static_cast<int>(
      read(
        fileDescriptor,
        output.objects,
        static_cast<unsigned>(output.size - 1)
      )
    );
    if (bytesRead > 0) {
      return bytesRead;
    }
    numFails ++;
    if (numFails > maxNumTries) {
      failStream
      << "Too many failed attempts: "
      << maxNumTries
      << " at reading from file descriptor: "
      << fileDescriptor
      << ". Error message: "
      << strerror(errno)
      << ".\n";
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << failStream.str();
      }
      return - 1;
    }
  } while (bytesRead <= 0);
  return bytesRead;
}

bool PipePrimitive::setPipeFlagsNoFailure(
  int inputFlags, int whichEnd, bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("Pipe::SetPipeBlockingModeCrashIfFail");
  int counter = 0;
  while (fcntl(this->pipeEnds[whichEnd], F_SETFL, inputFlags) < 0) {
    std::string errorString = strerror(errno);
    global
    << Logger::purple
    << "Failed to fcntl pipe end with file descriptor: "
    << this->pipeEnds[whichEnd]
    << ": "
    << errorString
    << ". "
    << Logger::endL;
    if (++ counter > 100) {
      if (!dontCrashOnFail) {
        global.fatal
        << "fcntl failed more than 100 times on "
        << "file desciptor: "
        << this->pipeEnds[whichEnd]
        << ": "
        << errorString
        << ". "
        << global.fatal;
      }
      return false;
    }
  }
  return true;
}

bool PipePrimitive::setReadNonBlocking(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeReadNonBlockingIfFailThenCrash");
  bool result = this->setPipeFlagsNoFailure(O_NONBLOCK, 0, dontCrashOnFail);
  if (result) {
    this->flagReadEndBlocks = false;
  }
  return result;
}

bool PipePrimitive::setReadBlocking(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeReadBlockingModeIfFailThenCrash")
  ;
  bool result = this->setPipeFlagsNoFailure(0, 0, dontCrashOnFail);
  if (result) {
    this->flagReadEndBlocks = true;
  }
  return result;
}

bool PipePrimitive::setPipeWriteNonBlockingIfFailThenCrash(
  bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("Pipe::setPipeWriteNonBlockingIfFailThenCrash")
  ;
  bool result = this->setPipeFlagsNoFailure(O_NONBLOCK, 1, dontCrashOnFail);
  if (result) {
    this->flagWriteEndBlocks = false;
  }
  return result;
}

bool PipePrimitive::setWriteBlocking(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::SetPipeWriteBlockingIfFailThenCrash");
  bool result = this->setPipeFlagsNoFailure(0, 1, dontCrashOnFail);
  if (result) {
    this->flagWriteEndBlocks = true;
  }
  return result;
}

int Pipe::writeNoInterrupts(int fileDescriptor, const std::string& input) {
  int numAttempts = 0;
  for (;;) {
    int result = static_cast<int>(
      write(fileDescriptor, input.c_str(), input.size())
    );
    if (result >= 0) {
      return result;
    }
    if (result < 0) {
      if (errno == EINTR) {
        global
        << Logger::red
        << "Write operation interrupted, repeating. "
        << Logger::endL;
        numAttempts ++;
        if (numAttempts > 100) {
          global
          << Logger::red
          <<
          "Write operation interrupted, more than 100 times, this is not supposed to happen. "
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

void Pipe::readLoop(List<char>& output) {
  MacroRegisterFunctionWithName("Pipe::readLoop");
  this->checkConsistency();
  MutexRecursiveWrapper& safetyFirst = global.mutexWebWorkerPipeReadlock;
  safetyFirst.lockMe();
  // Prevent threads from locking one another.
  this->metaData.lastRead.setSize(0);
  this->metaData.readOnceNoFailure(true);
  int expectedBytes = 0;
  int offset = 0;
  List<unsigned char> metaDataBuffer;
  metaDataBuffer = this->metaData.lastRead;
  Serialization::readFourByteInt(
    metaDataBuffer, offset, expectedBytes, nullptr
  );
  output.setSize(0);
  while (output.size < expectedBytes) {
    this->pipe.readOnceNoFailure(true);
    output.addListOnTop(this->pipe.lastRead);
  }
  safetyFirst.unlockMe();
  // Prevent threads from locking one another.
}

void Pipe::writeOnceAfterEmptying(
  const std::string& toBeSent, bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("Pipe::writeOnceAfterEmptying");
  MutexlockGuard safety(global.mutexWebWorkerPipeWritelock);
  this->mutexPipe.lock();
  this->pipe.readOnceNoFailure(dontCrashOnFail);
  this->pipe.lastRead.setSize(0);
  this->pipe.writeOnceNoFailure(toBeSent, 0, dontCrashOnFail);
  this->mutexPipe.unlock();
}

bool PipePrimitive::readOnceWithoutEmptying(bool dontCrashOnFail) {
  if (this->flagReadEndBlocks || this->flagWriteEndBlocks) {
    global.fatal
    << this->toString()
    << ": read without emptying allowed only on fully non-blocking pipes. "
    << global.fatal;
  }
  if (!this->readOnceNoFailure(dontCrashOnFail)) {
    return false;
  }
  return
  this->writeOnceNoFailure(this->getLastRead(), 0, dontCrashOnFail);
}

bool PipePrimitive::writeOnceAfterEmptying(
  const std::string& input, bool dontCrashOnFail
) {
  if (this->flagReadEndBlocks) {
    global.fatal
    << this->toString()
    << ": write after emptying allowed only on non-blocking read pipes. "
    << global.fatal;
  }
  if (!this->readOnceNoFailure(dontCrashOnFail)) {
    return false;
  }
  return this->writeOnceNoFailure(input, 0, dontCrashOnFail);
}

bool PipePrimitive::handleFailedWriteReturnFalse(
  const std::string& toBeSent, bool dontCrashOnFail, int numBadAttempts
) {
  std::stringstream errorStream;
  errorStream
  << "Failed write: "
  << toBeSent.size()
  << " bytes ["
  << StringRoutines::stringShortenInsertDots(toBeSent, 200)
  << "] onto: "
  << this->toString()
  << numBadAttempts
  << " or more times. Last error: "
  << strerror(errno)
  << ". ";
  global << Logger::red << errorStream.str() << Logger::endL;
  if (!dontCrashOnFail) {
    global.fatal
    << "Failed write on: "
    << this->toString()
    << numBadAttempts
    << " or more times. Last error: "
    << strerror(errno)
    << ". "
    << global.fatal;
  }
  return false;
}

bool PipePrimitive::writeOnceNoFailure(
  const std::string& toBeSent, int offset, bool dontCrashOnFail
) {
  MacroRegisterFunctionWithName("PipePrimitive::writeNoFailure");
  if (this->pipeEnds[1] == - 1) {
    global
    << Logger::yellow
    << "WARNING: "
    << this->toString()
    << " writing on non-initialized pipe. ";
    return false;
  }
  if (static_cast<unsigned>(offset) >= toBeSent.size() || offset < 0) {
    global.fatal
    << "Invalid offset: "
    << offset
    << "; toBeSent string has size: "
    << toBeSent.size()
    << ". ";
  }
  unsigned remaining = static_cast<unsigned>(
    toBeSent.size() - static_cast<unsigned>(offset)
  );
  if (remaining == 0) {
    return true;
  }
  int maximumBadAttempts = 30;
  this->numberOfBytesLastWrite = 0;
  int numBadAttempts = 0;
  for (;;) {
    this->numberOfBytesLastWrite = static_cast<int>(
      write(
        this->pipeEnds[1],
        &toBeSent[static_cast<unsigned>(offset)],
        remaining
      )
    );
    if (this->numberOfBytesLastWrite < 0) {
      if (
        errno == EAI_AGAIN ||
        errno == EWOULDBLOCK ||
        errno == EINTR ||
        errno == EIO
      ) {
        numBadAttempts ++;
        if (numBadAttempts > maximumBadAttempts) {
          return
          this->handleFailedWriteReturnFalse(
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
    global
    << Logger::red
    << this->toString()
    << ": wrote only "
    << this->numberOfBytesLastWrite
    << " bytes out of "
    << remaining
    << " total. "
    << Logger::endL;
  }
  return true;
}

std::string Pipe::toString() const {
  std::stringstream out;
  if (this->name != "") {
    out << this->name << ": information pipe: ";
  }
  out << this->pipe.toString();
  out << " mutex data: " << this->mutexPipe.toString();
  return out.str();
}

bool Pipe::resetNoAllocation() {
  bool oldReadEndBlocks = this->pipe.flagReadEndBlocks;
  if (oldReadEndBlocks) {
    if (!this->pipe.setReadNonBlocking(true)) {
      return false;
    }
  }
  this->pipe.readOnceNoFailure(true);
  if (oldReadEndBlocks) {
    if (!this->pipe.setReadBlocking(true)) {
      return false;
    }
  }
  if (
    this->metaData.readOnceNoFailure(true) &&
    this->mutexPipe.resetNoAllocation()
  ) {
    return true;
  }
  global
  << Logger::red
  << this->toString()
  << ": failed to reset without allocation. "
  << Logger::endL;
  return false;
}

bool Pipe::createMe(const std::string& inputPipeName) {
  this->checkConsistency();
  this->release();
  this->name = inputPipeName;
  if (
    !this->pipe.createMe("pipe[" + inputPipeName + "]", false, false, true)
  ) {
    this->release();
    return false;
  }
  if (!this->mutexPipe.createMe("mutex[" + inputPipeName + "]", true)) {
    this->release();
    return false;
  }
  if (
    !this->metaData.createMe(
      "metaData[" + inputPipeName + "]", false, false, true
    )
  ) {
    this->release();
    return false;
  }
  return true;
}

bool Pipe::checkConsistency() {
  MacroRegisterFunctionWithName("Pipe::checkConsistency");
  if (this->flagDeallocated) {
    global.fatal << "Use after free of pipe. " << global.fatal;
  }
  return true;
}

Pipe::~Pipe() {
  // Pipes are not allowed to release resources in the destructor:
  // a pipe's destructor is called when expanding List<Pipe>.
  this->flagDeallocated =
  true;
}

Pipe::Pipe() {
  this->flagDeallocated = false;
}

void Pipe::release() {
  this->checkConsistency();
  this->mutexPipe.release();
  this->pipe.release();
  this->metaData.release();
}

bool PipePrimitive::readOnceNoFailure(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("PipePrimitive::readOnceNoFailure");
  this->checkConsistency();
  this->lastRead.setSize(0);
  if (this->pipeEnds[0] == - 1) {
    return false;
  }
  int counter = 0;
  const unsigned int bufferSize = 200000;
  this->buffer.setSize(bufferSize);
  // <-once the buffer is resized, this operation does no memory allocation and
  // is fast.
  int numReadBytes = 0;
  for (;;) {
    numReadBytes = static_cast<int>(
      read(this->pipeEnds[0], this->buffer.objects, bufferSize)
    );
    if (numReadBytes >= 0) {
      break;
    }
    counter ++;
    if (counter > 100) {
      global
      << Logger::red
      << this->toString()
      << ": more than 100 iterations of read resulted in an error. "
      << Logger::endL;
      if (!dontCrashOnFail) {
        global.fatal
        << this->toString()
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
    global
    << Logger::red
    << this->toString()
    << "This is not supposed to happen: pipe read more than 150000 bytes. "
    << Logger::endL;
  }
  if (numReadBytes > 0) {
    this->buffer.setSize(numReadBytes);
    this->lastRead = this->buffer;
  }
  return true;
}

void Pipe::readOnceWithoutEmptying(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::readOnceWithoutEmptying");
  MutexRecursiveWrapper& safetyFirst = global.mutexWebWorkerPipeReadlock;
  MutexlockGuard guard(safetyFirst);
  // guard from other threads.
  this->mutexPipe.lock();
  this->pipe.readOnceNoFailure(dontCrashOnFail);
  if (this->pipe.lastRead.size > 0) {
    std::string tempS;
    tempS = this->getLastRead();
    this->pipe.writeOnceNoFailure(tempS, 0, dontCrashOnFail);
  }
  this->mutexPipe.unlock();
}

void Pipe::readOnce(bool dontCrashOnFail) {
  MacroRegisterFunctionWithName("Pipe::readOnce");
  this->checkConsistency();
  MutexRecursiveWrapper& safetyFirst = global.mutexWebWorkerPipeReadlock;
  MutexlockGuard guard(safetyFirst);
  // guard from other threads.
  MutexProcesslockGuard lock(this->mutexPipe);
  this->mutexPipe.lock();
  this->pipe.readOnceNoFailure(dontCrashOnFail);
  this->mutexPipe.unlock();
}

Logger::Logger() {
  this->flagInitialized = false;
  this->fileName = "";
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
  this->maximumLogSize =
  // 10000
  50000000;
  if (this->fileName == "") {
    this->flagStopWritingToFile = true;
    return;
  }
  if (this->logFile.is_open()) {
    this->logFile.close();
  }
  FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    this->logFile, this->fileName, false, true, false, true
  );
  if (!this->logFile.is_open()) {
    this->currentColor = Logger::red;
    std::string computedFileName;
    FileOperations::getPhysicalFileNameFromVirtual(
      this->fileName, computedFileName, true, false, nullptr
    );
    std::cout
    << this->openTagConsole()
    << "Could not open log file with virtual name: "
    << this->fileName
    << " and computed name: "
    << computedFileName
    << this->closeTagConsole()
    << std::endl;
  }
  this->logFile.seekg(0);
  this->logFile.seekp(0);
}

void Logger::checkLogSize() {
  this->initializeIfNeeded();
  if (logFile.tellp() > this->maximumLogSize) {
    if (this->flagResetLogFileWhenTooLarge) {
      this->reset();
      if (this->maximumLogSize > 1000) {
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
  logFile.flush();
  logFile.clear();
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
    out << "\x1b[1;31m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  case Logger::green:
    out << "\x1b[1;32m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  case Logger::yellow:
    out << "\x1b[1;33m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  case Logger::blue:
    out << "\x1b[1;34m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  case Logger::purple:
    out << "\x1b[1;35m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  case Logger::cyan:
    out << "\x1b[1;36m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  case Logger::orange:
    out << "\x1b[0;33m"; this->flagTagColorConsoleOpened = true; return
    out.str();
  default:
    return out.str();
  }
}

std::string Logger::openTagHtml() {
  std::stringstream out;
  out << Logger::closeTagHtml();
  switch (this->currentColor) {
  case Logger::red:
    out << "<span style='color:red'>"; this->flagTagColorHtmlOpened = true;
    return out.str();
  case Logger::green:
    out << "<span style='color:green'>"; this->flagTagColorHtmlOpened = true;
    return out.str();
  case Logger::blue:
    out << "<span style='color:blue'>"; this->flagTagColorHtmlOpened = true;
    return out.str();
  case Logger::yellow:
    out << "<span style='color:yellow'>"; this->flagTagColorHtmlOpened = true;
    return out.str();
  case Logger::orange:
    out << "<span style='color:orange'>"; this->flagTagColorHtmlOpened = true;
    return out.str();
  case Logger::purple:
    out << "<span style='color:purple'>"; this->flagTagColorHtmlOpened = true;
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
  out << "[" << global.logs.toStringProcessType() << ", ";
  if (global.server().activeWorker != - 1) {
    out << "w: " << global.server().activeWorker << ", ";
  }
  out << "c: " << global.server().statistics.allConnections;
  if (global.server().activeWorker != - 1) {
    out << "." << global.server().getActiveWorker().statistics.allReceives;
  }
  out << "] ";
  // <-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

std::string Logger::getStamp() {
  std::stringstream out;
  out << global.logs.toStringProcessType() << ", ";
  out
  << global.getDateForLogFiles()
  << ", "
  << global.getElapsedSeconds()
  << " s, ";
  if (global.server().activeWorker != - 1) {
    out << "w: " << global.server().activeWorker << ",";
  }
  out << " c: " << global.server().statistics.allConnections << ". ";
  // <-abbreviating worker to w and connection to c to reduce the log size.
  return out.str();
}

void Logger::StringHighligher::reset() {
  this->sections.setSize(0);
}

Logger::StringHighligher::Section::Section(int inputLength) {
  this->sectionType = "";
  this->length = inputLength;
}

Logger::StringHighligher::Section::Section(const std::string& input) {
  this->sectionType = input;
  this->length = 0;
}

Logger::StringHighligher::Section::Section() {
  this->sectionType = "";
  this->length = 0;
}

Logger::StringHighligher::StringHighligher() {}

Logger::StringHighligher::StringHighligher(const std::string& input) {
  List<char> delimiters;
  delimiters.addOnTop(',');
  delimiters.addOnTop('(');
  delimiters.addOnTop(')');
  delimiters.addOnTop('[');
  delimiters.addOnTop(']');
  List<std::string> inputStrings;
  StringRoutines::stringSplitExcludeDelimiters(
    input, delimiters, inputStrings
  );
  for (int i = 0; i < inputStrings.size; i ++) {
    std::string current =
    StringRoutines::stringTrimWhiteSpace(inputStrings[i]);
    Logger::StringHighligher::Section incoming;
    if (current == "|") {
      incoming.sectionType = "|";
      this->sections.addOnTop(incoming);
      continue;
    }
    if (current == "||") {
      incoming.sectionType = "||";
      this->sections.addOnTop(incoming);
      continue;
    }
    incoming.sectionType = "";
    LargeInteger largeInteger;
    largeInteger.assignString(current);
    if (!largeInteger.isIntegerFittingInInt(&incoming.length)) {
      global.fatal
      <<
      "StringHighligher is not allowed to fail: this is an internal function, "
      << "please do not expose to the outside world. "
      << global.fatal;
    }
    this->sections.addOnTop(incoming);
  }
}

void MathRoutines::parseListIntegersNoFailure(
  const std::string& input, List<int>& result
) {
  bool success = MathRoutines::parseListIntegers(input, result, nullptr);
  if (!success) {
    global.fatal
    << "Failed to parse list int with a function that does not allow failure. "
    << global.fatal;
  }
}

bool MathRoutines::parseListIntegers(
  const std::string& input,
  List<int>& result,
  std::stringstream* commentsOnFailure
) {
  List<char> delimiters;
  delimiters.addOnTopNoRepetition('\n');
  delimiters.addOnTopNoRepetition(',');
  delimiters.addOnTopNoRepetition('[');
  delimiters.addOnTopNoRepetition(']');
  delimiters.addOnTopNoRepetition('(');
  delimiters.addOnTopNoRepetition(')');
  List<std::string> numberStrings;
  StringRoutines::stringSplitExcludeDelimiters(
    input, delimiters, numberStrings
  );
  result.setSize(numberStrings.size);
  for (int i = 0; i < numberStrings.size; i ++) {
    LargeInteger integerValue;
    bool success =
    integerValue.assignStringFailureAllowed(
      numberStrings[i], commentsOnFailure
    );
    if (!success) {
      return false;
    }
    if (!integerValue.isIntegerFittingInInt(&result[i])) {
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

Logger& Logger::logString(const std::string& input) {
  if (input.size() == 0) {
    return *this;
  }
  if (this->nextHighlighter.sections.size == 0) {
    return this->doTheLogging(input);
  }
  int indexLastNonShownByte = 0;
  List<std::string> chunks;
  for (
    int i = 0; i < this->nextHighlighter.sections.size &&
    indexLastNonShownByte < static_cast<signed>(input.size()); i ++
  ) {
    Logger::StringHighligher::Section& currentSection =
    this->nextHighlighter.sections[i];
    if (currentSection.sectionType != "") {
      chunks.addOnTop(currentSection.sectionType);
      continue;
    }
    int nextSectionLength = currentSection.length;
    if (
      indexLastNonShownByte + nextSectionLength > static_cast<signed>(
        input.size()
      )
    ) {
      nextSectionLength = static_cast<int>(input.size()) -
      indexLastNonShownByte;
    }
    std::string current =
    input.substr(
      static_cast<unsigned>(indexLastNonShownByte),
      static_cast<unsigned>(nextSectionLength)
    );
    chunks.addOnTop(current);
    indexLastNonShownByte += nextSectionLength;
  }
  if (indexLastNonShownByte < static_cast<signed>(input.size())) {
    chunks.addOnTop(
      input.substr(static_cast<unsigned>(indexLastNonShownByte))
    );
  }
  for (int i = 0; i < chunks.size; i ++) {
    int colorIndex = i % 3;
    switch (colorIndex) {
    case 0:
      *this << Logger::blue; break;
    case 1:
      *this << Logger::red; break;
    case 2:
      *this << Logger::green; break;
    default:
      break;
    }
    this->doTheLogging(chunks[i]);
  }
  this->nextHighlighter.reset();
  return *this;
}

Logger& Logger::logSpecialSymbol(const LoggerSpecialSymbols& input) {
  this->initializeIfNeeded();
  this->checkLogSize();
  bool doUseColors = global.flagRunningBuiltInWebServer ||
  global.flagRunningConsoleRegular ||
  global.flagRunningConsoleTest;
  switch (input) {
  case Logger::endL:
    std::cout << this->getStampShort() << this->bufferStandardOutput; this->
    bufferStandardOutput.clear(); if (doUseColors) {
      std::cout << this->closeTagConsole();
    } std::cout << std::endl; this->currentColor = Logger::normalColor; if (
      this->flagStopWritingToFile
    ) {
      return *this;
    } this->bufferFile += this->closeTagHtml() + "\n<br>\n"; logFile
    << this->getStamp()
    << this->bufferFile; this->bufferFile = ""; logFile.flush(); return *this;
  case Logger::red:
  case Logger::blue:
  case Logger::yellow:
  case Logger::green:
  case Logger::purple:
  case Logger::orange:
  case Logger::cyan:
    this->currentColor = input; if (doUseColors) {
      this->bufferStandardOutput += this->openTagConsole();
    } if (this->flagStopWritingToFile) {
      return *this;
    } this->logFile << this->openTagHtml(); return *this;
  case Logger::normalColor:
    if (doUseColors) {
      this->bufferStandardOutput += this->closeTagConsole();
    } this->currentColor = Logger::normalColor; if (
      this->flagStopWritingToFile
    ) {
      return *this;
    } logFile << this->closeTagHtml(); return *this;// default:
    //   ;
  }
  return *this;
}

