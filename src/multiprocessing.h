// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderWebServerInterProcessLogisticsAlreadyDefined
#define vpfHeaderWebServerInterProcessLogisticsAlreadyDefined

#include "math_general.h"
#include "system_functions_global_objects.h"

// This is a basic wrapper around linux unnamed pipes.
class PipePrimitive {
public:
  List<int> pipeEnds; //pipeEnds[0] is the read end; pipeEnds[1] is the write end.
  List<char> lastRead;
  List<char> buffer;
  bool flagReadEndBlocks;
  bool flagWriteEndBlocks;
  bool flagDeallocated;
  std::string name;
  int numberOfBytesLastWrite;
  void Release();
  bool CreateMe(
    const std::string& inputPipeName,
    bool readEndBlocks,
    bool writeEndBlocks,
    bool dontCrashOnFail
  );
  std::string GetLastRead();
  bool checkConsistency();
  bool SetPipeFlagsIfFailThenCrash(
    int inputFlags,
    int whichEnd,
    bool dontCrashOnFail
  );

  bool SetReadNonBlocking (bool dontCrashOnFail);
  bool SetReadBlocking(bool dontCrashOnFail);

  bool SetWriteBlocking(bool dontCrashOnFail);
  bool SetPipeWriteNonBlockingIfFailThenCrash(bool dontCrashOnFail);

  bool SetPipeWriteFlagsIfFailThenCrash(
    int inputFlags,
    int whichEnd,
    bool dontCrashOnFail
  );
  bool WriteOnceIfFailThenCrash(
    const std::string& input,
    int offset,
    bool dontCrashOnFail
  );
  bool ReadOnceIfFailThenCrash(bool dontCrashOnFail);
  bool WriteOnceAfterEmptying(const std::string& input, bool dontCrashOnFail);

  bool ReadOnceWithoutEmptying(bool dontCrashOnFail);
  bool HandleFailedWriteReturnFalse(
    const std::string& toBeSent,
    bool dontCrashOnFail,
    int numBadAttempts
  );
  std::string toString() const;
  PipePrimitive();
  ~PipePrimitive();
};

// Inter-process mutex.
// Every inter-process mutex operation can fail.
class MutexProcess {
public:
  static std::string currentProcessName;

  static std::string lockContent;
  PipePrimitive lockPipe;
  bool flagLockHeldByAnotherThread;
  bool flagInitialized;
  std::string name;
  MemorySaving<MutexRecursiveWrapper> lockThreads;
  //<- to avoid two threads from the same process blocking the process.
  bool flagDeallocated;
  std::string toString() const;
  void Release();
  // inputName is the display name of the mutex - something you want
  // to see in error messages and logs.
  bool CreateMe(
    const std::string& inputName,
    bool dontCrashOnFail
  );
  bool ResetNoAllocation();

  bool checkConsistency();
  // Acts in a similar fashion to single-process mutex lock.
  // If the lock is held in another process or another thread,
  // the current thread in the current process
  // falls asleep until the lock is released.
  // When released, the lock is acquired and true is returned.
  //
  // MutexProcess::Lock() returns false on
  // critical pipe read/write errors.
  // When false is returned, the lock was not
  // successfully acquired.
  // The lock may or may not be currently held by
  // another process.
  //
  // A false return from MutexProcess::Lock()
  // usually indicates a fatal error,
  // but we may choose to handle it gracefully
  // for stability reasons.
  bool Lock();

  // Unlock the mutex.
  //
  // False return may indicate an i/o failure.
  // False result should normally be a fatal error but
  // may be handled gracefully for stability reasons.
  bool Unlock();
  static void Release(int& theDescriptor);
  MutexProcess();
  ~MutexProcess();
};

class MutexProcessLockGuard {
public:
  MutexProcess* guarded;
  MutexProcessLockGuard(MutexProcess& input) {
    this->guarded = &input;
    if (!this->guarded->Lock()) {
      this->guarded = nullptr;
      return;
    }
  }
  ~MutexProcessLockGuard() {
    if (this->guarded == nullptr) {
      return;
    }
    this->guarded->Unlock();
    this->guarded = nullptr;
  }
};

// This is an unnamed pipe with which may be shared by two or more processes
// and by more than one thread in each process.
// Multiple processes may try to send bytes through the pipe.
// A writer to the pipe may lock access to the pipe via theMutexPipe lock.
// TheMutexPipe lock has a (pipe-based) mechanism for locking out other processes and
// a mutex-based mechanism for locking out other threads within the same process.
class Pipe {
public:
  PipePrimitive thePipe;
  PipePrimitive metaData;
  MutexProcess theMutexPipe;
  bool flagDeallocated;
  std::string name;
  //  static int ConnectWithTimeoutViaSelect(int theFD, const std::string& input);
  std::string GetLastRead() {
    return this->thePipe.GetLastRead();
  }
  static void WriteIfFailThenCrash(
    int theFD, const List<char>& input, bool dontCrashOnFail
  );
  void ReadOnce(bool dontCrashOnFail);
  void ReadOnceWithoutEmptying(bool dontCrashOnFail);
  void ReadLoop(List<char>& output);
  static int WriteNoInterrupts(int theFD, const std::string& input);
  static int WriteWithTimeoutViaSelect(
    int theFD,
    const std::string& input,
    int timeOutInSeconds,
    int maxNumTries = 10,
    std::stringstream* commentsOnFailure = nullptr
  );
  static int ReadWithTimeOutViaSelect(
    int theFD,
    List<char>& output,
    int timeOutInSeconds,
    int maxNumTries = 10,
    std::stringstream* commentsOnFailure = nullptr
  );
  void WriteOnceAfterEmptying(
    const std::string& toBeSent,
    bool dontCrashOnFail
  );

  std::string toString() const;
  void Release();
  bool checkConsistency();
  bool CreateMe(const std::string& inputPipeName);
  bool ResetNoAllocation();
  ~Pipe();
  Pipe();
};

#endif

