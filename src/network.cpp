#include "network_calculator.h"
#include "webserver.h"

const int Listener::maximumPendingConnections = 1000000;

Connector::Connector() {
  this->peer = nullptr;
  this->socketInteger = - 1;
}

Connector::~Connector() {
  this->closeSocket();
  this->freeAddressInformation();
}

void Connector::initialize(
  const std::string& inputAddressToConnectTo, int port
) {
  std::stringstream portStream;
  portStream << port;
  this->initialize(inputAddressToConnectTo, portStream.str());
}

void Connector::initialize(
  const std::string& inputAddressToConnectTo,
  const std::string& inputPortOrService
) {
  memset(&this->hints, 0, sizeof this->hints);
  this->hints.ai_family = AF_UNSPEC;
  // IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM;
  // TCP stream sockets
  this->hints.ai_flags = AI_PASSIVE;
  this->peer = nullptr;
  this->addressToConnectTo = inputAddressToConnectTo;
  this->portOrService = inputPortOrService;
}

void Connector::closeSocket() {
  close(this->socketInteger);
  this->socketInteger = - 1;
}

void Connector::freeAddressInformation() {
  if (this->peer != nullptr) {
    freeaddrinfo(this->peer);
  }
  this->peer = nullptr;
}

void Connector::closeEverything() {
  this->closeSocket();
  this->freeAddressInformation();
}

std::string Connector::toString() const {
  std::stringstream out;
  if (this->peer == nullptr) {
    out << "{null}";
  } else {
    out << "{";
    for (struct addrinfo* p = this->peer; p != nullptr; p = p->ai_next) {
      out << this->toStringOneAddrInfo(p);
      if (p->ai_next != nullptr) {
        out << ", ";
      }
    }
    out << "}";
  }
  return out.str();
}

std::string Connector::toStringOneAddrInfo(addrinfo* address) {
  std::stringstream out;
  char ipString[INET6_ADDRSTRLEN];
  void* addressHandle = nullptr;
  // get the pointer to the address itself,
  // different fields in IPv4 and IPv6:
  if (address->ai_family == AF_INET) {
    // IPv4
    struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(
      address->ai_addr
    );
    addressHandle = &(ipv4->sin_addr);
  } else {
    // IPv6
    struct sockaddr_in6* ipv6 = reinterpret_cast<struct sockaddr_in6*>(
      address->ai_addr
    );
    addressHandle = &(ipv6->sin6_addr);
  }
  // convert the IP to a string and print it:
  inet_ntop(address->ai_family, addressHandle, ipString, sizeof ipString);
  out << ipString;
  return out.str();
}

bool Connector::connectWrapper(std::stringstream* commentsOnError) {
  int status =
  getaddrinfo(
    this->addressToConnectTo.c_str(),
    this->portOrService.c_str(),
    &this->hints,
    &this->peer
  );
  global
  << "DEBUG: got address info from: "
  << this->portOrService
  << Logger::endL;
  if (status != 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Could not find address: getaddrinfo error: "
      << gai_strerror(status);
    }
    this->freeAddressInformation();
    return false;
  }
  if (this->peer == nullptr) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Server info is zero. ";
    }
    return false;
  }
  // will point to the results
  this->socketInteger = - 1;
  for (
    struct addrinfo* p = this->peer; p != nullptr;
    p = p->ai_next,
    close(this->socketInteger)
  ) {
    if (this->oneConnectionAttempt(p, commentsOnError)) {
      return true;
    }
  }
  return false;
}

bool Connector::oneConnectionAttempt(
  addrinfo* peer, std::stringstream* commentsOnFailure
) {
  std::stringstream reportStream;
  this->socketInteger = - 1;
  reportStream
  << "Peer address: "
  << this->toStringOneAddrInfo(peer)
  << "[ "
  << this->toString()
  << "]"
  << "<br>";
  this->socketInteger =
  socket(
    this->peer->ai_family,
    this->peer->ai_socktype,
    this->peer->ai_protocol
  );
  int connectionResult = - 1;
  if (this->socketInteger < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to create socket ";
    }
    return false;
  }
  timeval timeOut;
  timeOut.tv_sec = 7;
  timeOut.tv_usec = 0;
  // Not initializing this can cause strange errors.
  if (
    setsockopt(
      this->socketInteger,
      SOL_SOCKET,
      SO_RCVTIMEO,
      static_cast<void*>(&timeOut),
      sizeof(timeval)
    ) ==
    - 1
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to set socket options. "
      << strerror(errno);
    }
    this->closeEverything();
    return false;
  }
  fd_set fdConnectSockets;
  FD_ZERO(&fdConnectSockets);
  FD_SET(this->socketInteger, &fdConnectSockets);
  timeOut.tv_sec = 2;
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
    reportStream
    << "DEBUG: immediately before connect(). Socki: "
    << this->socketInteger;
    connectionResult =
    connect(
      this->socketInteger,
      this->peer->ai_addr,
      this->peer->ai_addrlen
    );
  }
  if (connectionResult == - 1) {
    reportStream
    << "<br>Connector::oneConnectionAttempt: failed to connect: address: "
    << this->addressToConnectTo
    << " port: "
    << this->portOrService
    << ": "
    << strerror(errno)
    << ". ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << reportStream.str();
    }
    close(this->socketInteger);
    return false;
  }
  reportStream
  << "<br>connected: "
  << this->addressToConnectTo
  << " port: "
  << this->portOrService
  << ". ";
  return true;
}

bool Connector::sendWithLengthHeader(const std::string& payload) {
  SenderInternal sender(this->socketInteger);
  return sender.sendWithLengthHeader(payload);
}

bool Connector::sendWithLengthHeader(const List<unsigned char>& payload) {
  SenderInternal sender(this->socketInteger);
  return sender.sendWithLengthHeader(payload);
}

bool Connector::receive(std::string& output) {
  return false;
}

bool Connector::sendAndReceive(
  const std::string& payload, std::string& output
) {
  if (!this->sendWithLengthHeader(payload)) {
    return false;
  }
  return this->receive(output);
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
  global << "Creating socket from: " << desiredPort.c_str() << Logger::endL;
  rv = getaddrinfo(nullptr, desiredPort.c_str(), &hints, &servinfo);
  if (rv != 0) {
    global << "getaddrinfo failed: " << gai_strerror(rv) << Logger::endL;
    return false;
  }
  outputListeningSocket = - 1;
  global
  << "DEBUG: about to enter bind loop: desiired port: "
  << desiredPort
  << Logger::endL;
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
      if (desiredPort == "0" || true) {
        global
        << "Bound to port: "
        << Logger::red
        << listeningPortString.str()
        << Logger::normalColor
        << ", desired port: "
        << Logger::yellow
        << desiredPort
        << ". Socket to listen to: "
        << outputListeningSocket
        << ". "
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
        << "Error: failed to set non-blocking status to bound socket. "
        << global.fatal;
      }
    }
    break;
  }
  freeaddrinfo(servinfo);
  // All done with servinfo.
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

ReceiverInternal::ReceiverInternal(int inputConnectedSocket) {
  this->connectedSocket = inputConnectedSocket;
}

bool ReceiverInternal::recieveWithLengthHeader() {
  int expectedTotal = - 1;
  List<unsigned char> buffer;
  // The buffer size is larger than the maximum expected size,
  // so hopefully we will receive the entire buffer with a single read.
  const int bufferSize = 1048576;
  buffer.setSize(bufferSize);
  int totalReceived = 0;
  int totalFailures = 0;
  while (totalReceived < expectedTotal || expectedTotal == - 1) {
    int receivedBytes =
    recv(connectedSocket, this->received.objects, bufferSize, 0);
    if (receivedBytes < 0) {
      global
      << "DEBUG: "
      << Logger::red
      << "Failed to receive"
      << Logger::endL;
      totalFailures ++;
    }
    if (totalFailures >= 10) {
      global
      << "DEBUG: "
      << Logger::red
      << "Too many failures to receive. "
      << strerror(errno)
      << Logger::endL;
      return false;
    }
    totalReceived += receivedBytes;
    this->received.addListOnTop(buffer);
    if (totalReceived >= 4 && expectedTotal < 0) {
      expectedTotal = static_cast<int>(
        Crypto::convertListByteToUnsignedInt32(this->received)
      );
      if (expectedTotal < 0 || expectedTotal > 1000000) {
        global << "Payload too big. " << Logger::endL;
        return false;
      }
    }
  }
  return true;
}

SenderInternal::SenderInternal(int inputConnectedSocket) {
  this->connectedSocket = inputConnectedSocket;
}

bool SenderInternal::sendWithLengthHeader(const std::string& payload) {
  List<unsigned char> payloadCharacters;
  Crypto::convertStringToListBytes(payload, payloadCharacters);
  return this->sendWithLengthHeader(payloadCharacters);
}

bool SenderInternal::sendWithLengthHeader(
  const List<unsigned char>& payload
) {
  List<unsigned char> payloadWithLength;
  uint32_t size = static_cast<uint32_t>(payload.size);
  Crypto::convertUnsignedInt32ToUnsignedCharBigendian(size, payloadWithLength);
  payloadWithLength.addListOnTop(payload);
  return this->sendRaw(payloadWithLength);
}

bool SenderInternal::sendRaw(const List<unsigned char>& payload) {
  if (payload.size == 0) {
    return true;
  }
  int totalSent = 0;
  while (this->sendOnce(payload, totalSent)) {
    if (totalSent >= payload.size) {
      break;
    }
  }
  global
  << "Sent "
  << totalSent
  << " bytes through "
  << this->name
  << ". "
  << Logger::endL;
  return totalSent >= payload.size;
}

bool SenderInternal::sendOnce(
  const List<unsigned char>& payload,
  int& inputOutputSentSoFar,
  int numberOfTries
) {
  int remaining = payload.size - inputOutputSentSoFar;
  for (int i = 0; i < numberOfTries; i ++) {
    int sent =
    send(
      this->connectedSocket,
      &payload[inputOutputSentSoFar],
      static_cast<unsigned>(remaining),
      0
    );
    if (sent < 0) {
      global
      << Logger::red
      << "Connector ["
      << this->name
      << "] failed to send bytes "
      << i + 1
      << " times. "
      << Logger::endL;
      continue;
    }
    inputOutputSentSoFar += sent;
    return true;
  }
  return false;
}
