
#ifndef header_network_calculator_ALREADY_INCLUDED
#define header_network_calculator_ALREADY_INCLUDED

// The classes in this header are not portable.
// Do not include this header in other calculator headers,
// but only in .cpp files.
#include "multiprocessing.h"
#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop = network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors
#include <sys/resource.h> //<- for setrlimit(...) function. Restricts the time the executable can run.

// A wrapper around a listening socket.
// This class works on linux only because of
// fd_set and sockaddr_storage structures.
class Listener {
public:
  static const int maximumPendingConnections;
  fd_set fdSetListenSockets;
  int highestSocketNumber;
  MapList<int, std::string>* allListeningSockets;
  sockaddr_storage theirAddress;
  // connector's address information
  char userAddressBuffer[INET6_ADDRSTRLEN];
  std::string userAddress;
  void zeroSocketSet();
  Listener(
    int inputHighestSocketNumber,
    MapList<int, std::string>* inputListeningSockets
  ) {
    this->highestSocketNumber = inputHighestSocketNumber;
    this->allListeningSockets = inputListeningSockets;
  }
  int acceptWrapper();
  void selectWrapper();
  void computeUserAddress();
  static bool initializeBindToOnePort(
    const std::string& desiredPort,
    int& outputListeningSocket,
    int& outputActualPort,
    bool blockWhenWaitingToAccept
  );
  static std::string toStringLastErrorDescription();
};

// A wrapper around the data structures needed to connect()
class Connector {
public:
  struct addrinfo hints;
  struct sockaddr_in serverAddress;
  struct hostent* serverOtherSide;
  struct addrinfo* serverInfo;
  int socketInteger;
  std::string addressToConnectTo;
  std::string portOrService;
  std::string lastTransactionErrors;
  Connector();
  ~Connector();
  void initialize(
    const std::string& inputAddressToConnectTo,
    const std::string& inputPortOrService
  );
  bool connectWrapper();
  bool oneConnectionAttempt(addrinfo* peer);
  void freeAddressInformation();
  void closeSocket();
  void closeEverything();
};

class SystemFunctions {
public:
  static void* getIncomingAddress(sockaddr* sa);
  static void segfaultSigaction[[noreturn]](
    int signal, siginfo_t* si, void* arg
  );
};

#endif // header_network_calculator_ALREADY_INCLUDED
