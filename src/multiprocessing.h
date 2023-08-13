
#ifndef header_multiprocessing_ALREADY_INCLUDED
#define header_multiprocessing_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_memory_saving.h"

class ForkCreator {
public:
  // Forks out a process. If sighUpOnParentDeath is false,
  // then parent death will send SIGKILL to the child.
  // Else, if sighUpOnParentDeath is true, the milder SIGHUP
  // will be sent instead;
  static int forkProcessAndAcquireRandomness(bool sighUpOnParentDeath = false);
  static int forkRaw();
};

// This is a basic wrapper around linux unnamed pipes.
class PipePrimitive {
public:
  List<int> pipeEnds;
  // pipeEnds[0] is the read end; pipeEnds[1] is the write end.
  List<char> lastRead;
  List<char> buffer;
  bool flagReadEndBlocks;
  bool flagWriteEndBlocks;
  bool flagDeallocated;
  std::string name;
  int numberOfBytesLastWrite;
  // By observation, write a maximum of 2^16 bytes at a time.
  // So, there is no point in allocating more than 65536 bytes;
  // let's allocate an extra byte by default.
  const int bufferSize = 65537;
  void release();
  bool createMe(
    const std::string& inputPipeName,
    bool readEndBlocks,
    bool writeEndBlocks,
    bool dontCrashOnFail
  );
  std::string getLastRead();
  bool checkConsistency();
  bool setPipeFlagsNoFailure(
    int inputFlags, int whichEnd, bool dontCrashOnFail
  );
  bool setReadNonBlocking(bool dontCrashOnFail);
  bool setReadBlocking(bool dontCrashOnFail);
  bool setWriteBlocking(bool dontCrashOnFail);
  bool setPipeWriteNonBlockingIfFailThenCrash(bool dontCrashOnFail);
  bool setPipeWriteFlagsNoFailure(
    int inputFlags, int whichEnd, bool dontCrashOnFail
  );
  bool writeOnceNoFailure(
    const std::string& input, int offset, bool dontCrashOnFail
  );
  bool readOnceNoFailure(bool dontCrashOnFail);
  bool writeOnceAfterEmptying(
    const std::string& input, bool dontCrashOnFail
  );
  bool readOnceWithoutEmptying(bool dontCrashOnFail);
  bool handleFailedWriteReturnFalse(
    const std::string& toBeSent, bool dontCrashOnFail, int numBadAttempts
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
  bool flaglockHeldByAnotherThread;
  bool flagInitialized;
  std::string name;
  MemorySaving<MutexRecursiveWrapper> lockThreads;
  // <- to avoid two threads from the same process blocking the process.
  bool flagDeallocated;
  std::string toString() const;
  void release();
  // inputName is the display name of the mutex - something you want
  // to see in error messages and logs.
  bool createMe(const std::string& inputName, bool dontCrashOnFail);
  bool resetNoAllocation();
  bool checkConsistency();
  // Acts in a similar fashion to single-process mutex lock.
  // If the lock is held in another process or another thread,
  // the current thread in the current process
  // falls asleep until the lock is released.
  // When released, the lock is acquired and true is returned.
  //
  // MutexProcess::lock() returns false on
  // critical pipe read/write errors.
  // When false is returned, the lock was not
  // successfully acquired.
  // The lock may or may not be currently held by
  // another process.
  //
  // A false return from MutexProcess::lock()
  // usually indicates a fatal error,
  // but we may choose to handle it gracefully
  // for stability reasons.
  bool lock();
  // unlock the mutex.
  //
  // False return may indicate an i/o failure.
  // False result should normally be a fatal error but
  // may be handled gracefully for stability reasons.
  bool unlock();
  static void release(int& descriptor);
  MutexProcess();
  ~MutexProcess();
};

class MutexProcesslockGuard {
public:
  MutexProcess* guarded;
  MutexProcesslockGuard(MutexProcess& input) {
    this->guarded = &input;
    if (!this->guarded->lock()) {
      this->guarded = nullptr;
      return;
    }
  }
  ~MutexProcesslockGuard() {
    if (this->guarded == nullptr) {
      return;
    }
    this->guarded->unlock();
    this->guarded = nullptr;
  }
};

// This is an unnamed pipe which may be shared by two or more processes
// and by more than one thread in each process.
// Multiple processes may try to send bytes through the pipe.
// A writer to the pipe may lock access to the pipe via mutexPipe lock.
// mutexPipe lock has a (pipe-based) mechanism for locking out other
// processes and
// a mutex-based mechanism for locking out other threads within the same
// process.
class Pipe {
public:
  PipePrimitive pipe;
  PipePrimitive metaData;
  MutexProcess mutexPipe;
  MutexRecursiveWrapper threadLock;
  bool flagDeallocated;
  std::string name;
  std::string getLastRead() {
    return this->pipe.getLastRead();
  }
  // Executes a single read on the pipe.
  // Can only read the amount of bytes that fit in a buffer.
  bool readOnce(bool dontCrashOnFail);
  void readOnceWithoutEmptying(bool dontCrashOnFail);
  // Reads a completed message in a read loop.
  // The counterpart sender sends the length of
  // the message through the metaData pipe.
  // The counterpart sender is expected to cooperate,
  // and in particular to run on the same machine.
  bool readFullMessage(List<char>& output);
  static int writeNoInterrupts(
    int fileDescriptor, const std::string& input
  );
  static int writeWithTimeoutViaSelect(
    int fileDescriptor,
    const std::string& input,
    int timeOutInSeconds,
    int maximumTries = 10,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool readWithTimeOutViaSelectOneFileDescriptor(
    int fileDescriptor,
    List<char>& output,
    int bufferSize,
    int timeOutInSecondsNonPositiveForNoTimeLimit,
    int maximumTries = 10,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool readWithTimeOutViaSelect(
    const List<int>& fileDescriptors,
    List<char>& output,
    int& outputFileDescriptor,
    int bufferSize,
    int timeOutInSecondsNonPositiveForNoTimeLimit,
    int maximumTries = 10,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool writeOnceAfterEmptying(
    const std::string& toBeSent, bool dontCrashOnFail
  );
  std::string toString() const;
  void release();
  bool checkConsistency();
  bool createMe(const std::string& inputPipeName);
  bool resetNoAllocation();
  ~Pipe();
  Pipe();
};

#endif // header_multiprocessing_ALREADY_INCLUDED
