
#ifndef header_web_client_ALREADY_INCLUDED
#define header_web_client_ALREADY_INCLUDED

#include "transport_layer_security.h"

class WebClient {
public:
  TransportLayerSecurity transportLayerSecurity;
  int socketInteger;
  std::string portOrService;
  std::string addressToConnectTo;
  std::string serverToConnectTo;
  std::string postMessageToSend;
  std::string lastTransactionErrors;
  std::string lastTransaction;
  std::string headerReceived;
  std::string bodyReceived;
  std::string bodyReceivedWithHeader;
  std::string bodyReceivedOutsideOfExpectedLength;
  bool flagInitialized;
  bool flagContinueWasNeeded;
  bool flagDoUseGET;
  LargeInteger expectedLength;
  List<char> buffer;
  int lastNumBytesRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverOtherSide;
  struct addrinfo hints;
  struct addrinfo* serverInfo;
  WebClient();
  ~WebClient();
  void closeEverything();
  void updatePublicKeys(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  void fetchWebPagePart2(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  void initialize();
  // Ping authentication used for pings only, over localhost connections only.
  void pingCalculatorStatus(const std::string& pingAuthentication);
  void freeAddressInfo();
  void fetchWebPage(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool verifyRecaptcha(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNONsensitive,
    std::stringstream* commentsGeneralSensitive
  );
};

#endif
