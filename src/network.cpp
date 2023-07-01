#include "network_calculator.h"
#include "webserver.h"

const int Listener::maximumPendingConnections = 1000000;

Connector::Connector() {
  this->serverOtherSide = nullptr;
  this->serverInfo = nullptr;
  this->socketInteger = - 1;
}

Connector::~Connector() {
  this->closeSocket();
  this->freeAddressInformation();
}

void Connector::initialize(
  const std::string& inputAddressToConnectTo,
  const std::string& inputPortOrService
) {
  memset(&this->hints, 0, sizeof this->hints);
  // make sure the struct is empty
  this->hints.ai_family = AF_UNSPEC;
  // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM;
  // TCP stream sockets
  this->hints.ai_flags = AI_PASSIVE;
  // fill in my IP for me
  this->serverInfo = nullptr;
  this->addressToConnectTo = inputAddressToConnectTo;
  this->portOrService = inputPortOrService;
}

void Connector::closeSocket() {
  close(this->socketInteger);
  this->socketInteger = - 1;
}

void Connector::freeAddressInformation() {
  if (this->serverInfo != nullptr) {
    freeaddrinfo(this->serverInfo);
  }
  this->serverInfo = nullptr;
}

void Connector::closeEverything() {
  this->closeSocket();
  this->freeAddressInformation();
}

bool Connector::connectWrapper() {
  int status =
  getaddrinfo(
    this->addressToConnectTo.c_str(),
    this->portOrService.c_str(),
    &hints,
    &this->serverInfo
  );
  if (status != 0) {
    this->lastTransactionErrors =
    "Could not find address: getaddrinfo error: ";
    this->lastTransactionErrors += gai_strerror(status);
    this->freeAddressInformation();
    return false;
  }
  if (this->serverInfo == nullptr) {
    this->lastTransactionErrors = "Server info is zero. ";
    return false;
  }
  // will point to the results
  this->socketInteger = - 1;
  for (
    struct addrinfo* p = this->serverInfo; p != nullptr;
    p = p->ai_next,
    close(this->socketInteger)
  ) {
    if (this->oneConnectionAttempt(p)) {
      return true;
    }
  }
  return false;
}

bool Connector::oneConnectionAttempt(addrinfo* peer) {
  char ipString[INET6_ADDRSTRLEN];
  void* address = nullptr;
  std::stringstream reportStream;
  this->socketInteger = - 1;
  // get the pointer to the address itself,
  // different fields in IPv4 and IPv6:
  if (peer->ai_family == AF_INET) {
    // IPv4
    struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(
      peer->ai_addr
    );
    address = &(ipv4->sin_addr);
    reportStream << "IPv4: ";
  } else {
    // IPv6
    struct sockaddr_in6* ipv6 = reinterpret_cast<struct sockaddr_in6*>(
      peer->ai_addr
    );
    address = &(ipv6->sin6_addr);
    reportStream << "IPv6: ";
  }
  // convert the IP to a string and print it:
  inet_ntop(peer->ai_family, address, ipString, sizeof ipString);
  reportStream << this->addressToConnectTo << ": " << ipString << "<br>";
  this->socketInteger =
  socket(
    this->serverInfo->ai_family,
    this->serverInfo->ai_socktype,
    this->serverInfo->ai_protocol
  );
  int connectionResult = - 1;
  if (this->socketInteger < 0) {
    this->lastTransactionErrors = "Failed to create socket ";
    return false;
  }
  fd_set fdConnectSockets;
  FD_ZERO(&fdConnectSockets);
  FD_SET(this->socketInteger, &fdConnectSockets);
  timeval timeOut;
  timeOut.tv_sec = 1;
  timeOut.tv_usec = 0;
  int totalPingFails = 0;
  int totalSelected = 0;
  std::stringstream failStream;
  do {
    if (totalPingFails > 10) {
      break;
    }
    totalSelected =
    select(
      this->socketInteger + 1,
      &fdConnectSockets,
      nullptr,
      nullptr,
      &timeOut
    );
    failStream
    << "While pinging, select failed. Error message: "
    << strerror(errno)
    << ". \n";
    totalPingFails ++;
  } while (totalSelected < 0);
  if (totalSelected <= 0) {
    global << Logger::red << failStream.str() << Logger::endL;
    reportStream
    << failStream.str()
    << "Could not connect through port. select returned: "
    << totalSelected;
    connectionResult = - 1;
  } else {
    connectionResult =
    connect(
      this->socketInteger,
      this->serverInfo->ai_addr,
      this->serverInfo->ai_addrlen
    );
  }
  if (connectionResult == - 1) {
    reportStream
    << "<br>Failed to connect: address: "
    << this->addressToConnectTo
    << " port: "
    << this->portOrService
    << ". ";
    this->lastTransactionErrors = reportStream.str();
    close(this->socketInteger);
    return false;
  }
  reportStream
  << "<br>connected: "
  << this->addressToConnectTo
  << " port: "
  << this->portOrService
  << ". ";
  timeOut.tv_sec = 3;
  timeOut.tv_usec = 0;
  // Not initializing this can cause strange errors.
  if (
    setsockopt(
      connectionResult,
      SOL_SOCKET,
      SO_RCVTIMEO,
      static_cast<void*>(&timeOut),
      sizeof(timeval)
    ) ==
    - 1
  ) {
    this->lastTransactionErrors = "Failed to set socket options.";
    this->closeEverything();
    return false;
  }
  return true;
}

std::string Listener::toStringLastErrorDescription() {
  std::stringstream out;
  out << strerror(errno) << ". ";
  return out.str();
}

bool Listener::initializeBindToOnePort(
  const std::string& desiredPort,
  int& outputListeningSocket,
  int& outputActualPort,
  bool blockWhenWaitingToAccept
) {
  STACK_TRACE("WebServer::initializeBindToOnePort");
  addrinfo hints;
  addrinfo* servinfo = nullptr;
  addrinfo* p = nullptr;
  int yes = 1;
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  // Use my IP.
  // Loop through all the results and bind to the first we can.
  rv = getaddrinfo(nullptr, desiredPort.c_str(), &hints, &servinfo);
  if (rv != 0) {
    global << "getaddrinfo failed: " << gai_strerror(rv) << Logger::endL;
    return false;
  }
  outputListeningSocket = - 1;
  for (p = servinfo; p != nullptr; p = p->ai_next) {
    outputListeningSocket =
    socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (outputListeningSocket == - 1) {
      global << "Error: socket failed.\n";
      continue;
    }
    if (
      setsockopt(
        outputListeningSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &yes,
        sizeof(int)
      ) ==
      - 1
    ) {
      global.fatal
      << "Error: setsockopt failed, error: \n"
      << strerror(errno)
      << global.fatal;
    }
    if (bind(outputListeningSocket, p->ai_addr, p->ai_addrlen) == - 1) {
      close(outputListeningSocket);
      outputListeningSocket = - 1;
      global
      << "Error: bind failed at port: "
      << desiredPort
      << ". Error: "
      << Listener::toStringLastErrorDescription()
      << Logger::endL;
      continue;
    }
    struct sockaddr_in serverAddress;
    unsigned int serverAddressLength = 0;
    if (
      getsockname(
        outputListeningSocket, (sockaddr*) & serverAddress,
        &serverAddressLength
      ) ==
      - 1
    ) {
      global
      << Logger::red
      << "Error: failed to find socket name. "
      << Logger::endL;
    } else {
      outputActualPort = ntohs(serverAddress.sin_port);
      std::stringstream listeningPortString;
      listeningPortString << outputActualPort;
      if (desiredPort == "0") {
        global
        << "Listening on port: "
        << Logger::red
        << listeningPortString.str()
        << Logger::normalColor
        << ", desired port: "
        << Logger::yellow
        << desiredPort
        << Logger::endL;
      }
    }
    if (blockWhenWaitingToAccept) {
      break;
    }
    int setFlagCounter = 0;
    while (fcntl(outputListeningSocket, F_SETFL, O_NONBLOCK) != 0) {
      if (++ setFlagCounter > 10) {
        global.fatal
        << "Error: failed to set non-blocking status to listening socket. "
        << global.fatal;
      }
    }
    break;
  }
  freeaddrinfo(servinfo);
  // all done with this structure
  if (outputListeningSocket == - 1) {
    global.fatal
    << "Failed to bind to port: "
    << desiredPort
    << ". "
    << global.fatal;
  }
  return true;
}

void Listener::zeroSocketSet() {
  FD_ZERO(&this->fdSetListenSockets);
  for (int i = 0; i < this->allListeningSockets->size(); i ++) {
    FD_SET(
      this->allListeningSockets->keys[i], &this->fdSetListenSockets
    );
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::red
    << "Detail: About to enter select loop. "
    << Logger::endL;
  }
}

void Listener::selectWrapper() {
  while (true) {
    int selectResult =
    select(
      this->highestSocketNumber + 1,
      &this->fdSetListenSockets,
      nullptr,
      nullptr,
      nullptr
    );
    if (selectResult != - 1) {
      return;
    }
    global.server().statistics.failedSelectsSoFar ++;
  }
}

int Listener::acceptWrapper() {
  STACK_TRACE("Listener::acceptWrapper");
  socklen_t sin_size = sizeof(this->theirAddress);
  if (this->allListeningSockets->size() == 0) {
    global << "Unexpected empty set of listening sockets. " << Logger::endL;
  }
  for (int i = this->allListeningSockets->size() - 1; i >= 0; i --) {
    int currentListeningSocket = this->allListeningSockets->keys[i];
    if (!FD_ISSET(currentListeningSocket, &this->fdSetListenSockets)) {
      continue;
    }
    int result =
    accept(
      currentListeningSocket,
      reinterpret_cast<struct sockaddr*>(&this->theirAddress),
      &sin_size
    );
    if (result >= 0) {
      global.server().lastListeningSocket = currentListeningSocket;
      global
      << Logger::green
      << "Connection candidate "
      << global.server().statistics.allConnections + 1
      << ". "
      << "Connected via listening socket "
      << currentListeningSocket
      << " on socket: "
      << result
      << Logger::endL;
      return result;
    } else {
      global
      << Logger::red
      << "This is not supposed to happen: accept failed. Error: "
      << global.server().toStringLastErrorDescription()
      << Logger::endL;
    }
  }
  return - 1;
}

void Listener::computeUserAddress() {
  inet_ntop(
    this->theirAddress.ss_family,
    SystemFunctions::getIncomingAddress(
      reinterpret_cast<struct sockaddr*>(&this->theirAddress)
    ),
    this->userAddressBuffer,
    sizeof this->userAddressBuffer
  );
  this->userAddress = this->userAddressBuffer;
}
