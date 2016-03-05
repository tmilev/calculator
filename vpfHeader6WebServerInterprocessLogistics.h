//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderWebServerInterProcessLogisticsAlreadyDefined
#define vpfHeaderWebServerInterProcessLogisticsAlreadyDefined

#include "vpfHeader2Math0_General.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"

static ProjectInformationInstance projectInfoInstanceWebServerInterProcessLogisticsHeader
(__FILE__, "Web server: inter-process communication/logistics.");

//this class is similar to the controller class but coordinates across different processes,
//rather than across different threads.
//inter-process communication is achieved via pipes.
class PauseController
{
public:
  List<int> thePausePipe; //thePipe[0] is the read end; thePipe[1] is the write end.
  List<int> mutexPipe;
  List<char> buffer;
  std::string name;
  MemorySaving<MutexRecursiveWrapper> mutexForProcessBlocking; //<- to avoid two threads from the same process blocking the process.
  bool flagDeallocated;
  std::string ToString()const;
  void Release();
  bool CreateMe(const std::string& inputName);

  bool CheckConsistency();
  bool CheckPauseIsRequested();
  void PauseIfRequested();
  bool PauseIfRequestedWithTimeOut();

  void RequestPausePauseIfLocked();

  void ResumePausedProcessesIfAny();
  void LoCkMe();
  void UnloCkMe();
  static void Release(int& theDescriptor);
  PauseController();
  ~PauseController();
};

class Pipe
{
private:
  void ReadNoLocks();
  void WriteNoLocks(const std::string& toBeSent);
public:
  List<int> thePipe; //thePipe[0] is the read end; thePipe[1] is the write end.
  PauseController pipeAvailable;
  List<char> lastRead;
  List<char> pipeBuffer;
  bool flagDeallocated;
  std::string name;
  static int WriteNoInterrupts(int theFD, const std::string& input);
  void Read();
  void ReadWithoutEmptying();
  void WriteAfterEmptying(const std::string& toBeSent);

  std::string ToString()const;
  void Release();
  bool CheckConsistency();
  bool CreateMe(const std::string& inputPipeName);
  ~Pipe();
  Pipe();
};
#endif

