//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeaderWebServerInterProcessLogisticsAlreadyDefined
#define vpfHeaderWebServerInterProcessLogisticsAlreadyDefined

#include "math_general.h"
#include "system_functions_global_objects.h"

static ProjectInformationInstance projectInfoInstanceWebServerInterProcessLogisticsHeader(
  __FILE__, "Web server: inter-process communication/logistics."
);

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
    bool restartServerOnFail,
    bool dontCrashOnFail
  );
  std::string GetLastRead();
  bool CheckConsistency();
  bool SetPipeFlagsIfFailThenCrash(
    int inputFlags,
    int whichEnd,
    bool restartServerOnFail,
    bool dontCrashOnFail
  );

  bool SetReadNonBlocking (bool restartServerOnFail, bool dontCrashOnFail);
  bool SetReadBlocking(bool restartServerOnFail, bool dontCrashOnFail);

  bool SetWriteBlocking(bool restartServerOnFail, bool dontCrashOnFail);
  bool SetPipeWriteNonBlockingIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail);

  bool SetPipeWriteFlagsIfFailThenCrash(
    int inputFlags,
    int whichEnd,
    bool restartServerOnFail,
    bool dontCrashOnFail
  );
  bool WriteOnceIfFailThenCrash(
    const std::string& input,
    int offset,
    bool restartServerOnFail,
    bool dontCrashOnFail
  );
  bool ReadOnceIfFailThenCrash(bool restartServerOnFail, bool dontCrashOnFail);
  bool WriteOnceAfterEmptying(const std::string& input, bool restartServerOnFail, bool dontCrashOnFail);

  bool ReadOnceWithoutEmptying(bool restartServerOnFail, bool dontCrashOnFail);
  bool HandleFailedWriteReturnFalse(
    const std::string& toBeSent,
    bool restartServerOnFail,
    bool dontCrashOnFail,
    int numBadAttempts
  );
  std::string ToString() const;
  PipePrimitive();
  ~PipePrimitive();
};

// This class is similar to the controller class but coordinates across different processes,
// rather than across different threads.
// inter-process communication is achieved via pipes.

class PauseProcess {
public:
  static std::string currentProcessName;

  PipePrimitive thePausePipe;
  PipePrimitive mutexPipe;
  std::string name;
  MemorySaving<MutexRecursiveWrapper> lockThreads; //<- to avoid two threads from the same process blocking the process.
  bool flagDeallocated;
  std::string ToString() const;
  void Release();
  bool CreateMe(const std::string& inputName, bool restartServerOnFail, bool dontCrashOnFail);
  bool ResetNoAllocation();

  bool CheckConsistency();
  bool CheckPauseIsRequested(bool restartServerOnFail, bool dontCrashOnFail, bool dontLockImServer);
  void PauseIfRequested(bool restartServerOnFail, bool dontCrashOnFail);
  bool PauseIfRequestedWithTimeOut(bool restartServerOnFail, bool dontCrashOnFail);

  void RequestPausePauseIfLocked(bool restartServerOnFail, bool dontCrashOnFail);

  void ResumePausedProcessesIfAny(bool restartServerOnFail, bool dontCrashOnFail);
  void LoCkMe();
  void UnloCkMe();
  static void Release(int& theDescriptor);
  PauseProcess();
  ~PauseProcess();
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
  PauseProcess theMutexPipe;
  bool flagDeallocated;
  std::string name;
  //  static int ConnectWithTimeoutViaSelect(int theFD, const std::string& input);
  std::string GetLastRead() {
    return this->thePipe.GetLastRead();
  }
  static void WriteIfFailThenCrash(
    int theFD, const List<char>& input, bool restartServerOnFail, bool dontCrashOnFail
  );
  void ReadOnce(bool restartServerOnFail, bool dontCrashOnFail);
  void ReadOnceWithoutEmptying(bool restartServerOnFail, bool dontCrashOnFail);
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
  void WriteLoopAfterEmptyingBlocking(const std::string& toBeSent);
  void WriteOnceAfterEmptying(
    const std::string& toBeSent,
    bool restartServerOnFail,
    bool dontCrashOnFail
  );

  std::string ToString() const;
  void Release();
  bool CheckConsistency();
  bool CreateMe(const std::string& inputPipeName);
  bool ResetNoAllocation();
  ~Pipe();
  Pipe();
};

#endif

