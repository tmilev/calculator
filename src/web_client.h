#ifndef header_web_client_ALREADY_INCLUDED
#define header_web_client_ALREADY_INCLUDED

#include "transport_layer_security.h"

// Forward-declared for maximum portability.
class Connector;

class WebClient {
public:
  TransportLayerSecurity transportLayerSecurity;
  // The peer address, along the lines of http://google.com.
  std::string peerAddress;
  // The full address, along the lines of https://google.com/index.html.
  std::string url;
  // A service such as https.
  std::string desiredPortOrService;
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
  int lastNumberOfBytesRead;
  MemorySaving<Connector> connectorContainer;
  WebClient();
  ~WebClient();
  void closeEverything();
  void updatePublicKeys(
    std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  void fetchWebPagePart2(
    std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  void initialize();
  // Ping authentication used for pings only, over localhost connections only.
  void pingCalculatorStatus(const std::string& pingAuthentication);
  void freeAddressInfo();
  void fetchWebPage(
    std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  std::string toString() const;
};

#endif
