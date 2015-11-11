//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderWebServerInterProcessLogisticsAlreadyDefined
#define vpfHeaderWebServerInterProcessLogisticsAlreadyDefined

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
  MemorySaving<MutexWrapper> mutexForProcessBlocking; //<- to avoid two threads from the same process blocking the process.
  std::string ToString()const;
  void Release();
  void CreateMe(const std::string& inputName);

  bool CheckPauseIsRequested();
  void PauseIfRequested();
  bool PauseIfRequestedWithTimeOut();

  void RequestPausePauseIfLocked();

  void ResumePausedProcessesIfAny();
  void LoCkMe();
  void UnloCkMe();
  static void Release(int& theDescriptor);
  PauseController();
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
  std::string name;

  void Read();
  void ReadWithoutEmptying();
  void WriteAfterEmptying(const std::string& toBeSent);

  std::string ToString()const;
  void Release();
  void CreateMe(const std::string& inputPipeName);
  ~Pipe();
  Pipe()
  { this->thePipe.initFillInObject(2,-1);
  }
};

class logger
{
  public:
  int currentColor;
  std::fstream theFile;
  bool flagStopWritingToFile;
  logger(const std::string& logFileName);
  void CheckLogSize();
  enum loggerSpecialSymbols{ endL, red, blue, yellow, green, purple, cyan, normalColor};
  std::string closeTagConsole();
  std::string closeTagHtml();
  std::string openTagConsole();
  std::string openTagHtml();
  logger& operator << (const loggerSpecialSymbols& input);
  template <typename theType>
  logger& operator << (const theType& toBePrinted)
  { if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
      std::cout << toBePrinted;
    this->CheckLogSize();
    if (this->flagStopWritingToFile)
      return *this;
    std::stringstream out;
    out << toBePrinted;
    theFile << out.str();
    return *this;
  }
};
#endif

