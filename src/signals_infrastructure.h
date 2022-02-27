
#ifndef header_signals_infrastructure_ALREADY_INCLUDED
#define header_signals_infrastructure_ALREADY_INCLUDED

#include <sys/wait.h>//<-waitpid f-n here

class SignalsInfrastructure {
public:
  struct sigaction SignalSEGV;
  struct sigaction SignalFPE;
  struct sigaction SignalChild;
  struct sigaction SignalINT;
  sigset_t allSignals;
  sigset_t oldSignals;
  bool flagSignalsAreBlocked;
  bool flagSignalsAreStored;
  bool flagInitialized;
  SignalsInfrastructure();
  void blockSignals();
  void unblockSignals();
  void initializeSignals();
  static SignalsInfrastructure& signals();
};

#endif // header_signals_infrastructure_ALREADY_INCLUDED

