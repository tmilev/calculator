#ifndef vpfHeaderTransportSecurityLayer_already_included
#define vpfHeaderTransportSecurityLayer_already_included

#include "vpfHeader1General0_General.h"

static ProjectInformationInstance projectInfoInstanceTransportSecurityLayerHeader(__FILE__, "TLS/ssl header.");

//installation of these headers in ubuntu:
//sudo apt-get install libssl-dev
//on opensuse:
//sudo yast -i libopenssl-devel

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class TransportSecurityLayer;
struct TransportSecurityLayerOpenSSL {
  TransportSecurityLayer* owner;
  int errorCode;
  static bool flagSSLlibraryInitialized;
  ssl_st* sslData;
  X509* peer_certificate;
  SSL_CTX* contextServer;
  bool flagSSLHandshakeSuccessful;
  const SSL_METHOD* theSSLMethod;
  List<char> buffer;
  struct errors {
    static std::string errorWantRead;
  };
  std::string otherCertificateIssuerName, otherCertificateSubjectName;
  TransportSecurityLayerOpenSSL();
  void FreeSSL();
  void FreeContext();
  void FreeEverythingShutdownSSL();
  void initSSLlibrary();
  void initSSLserver();
  void initSSLClient();
  void initSSLCommon();
};

class TransportSecurityLayer {
public:
  static bool flagDontUseOpenSSL;
  bool flagInitializedPrivateKey;
  bool flagIsServer;
  TransportSecurityLayerOpenSSL openSSLData;

  List<int> socketStackServer;
  List<int> socketStackClient;

  static const std::string fileCertificate;
  static const std::string fileCertificateConfiguration;
  static const std::string fileKey;
  static const std::string signedFileCertificate1;
  static const std::string signedFileCertificate3;
  static const std::string signedFileKey;
  void DoSetSocket(int theSocket);
  void SetSocketAddToStack(int theSocket);
  void RemoveLastSocketServer();
  void RemoveLastSocketClient();
  void ClearErrorQueue(
    int errorCode,
    std::string *outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  static bool initSSLkeyFiles();
  void initialize(bool IamServer);
  int SSLRead(
    void *buffer,
    int bufferSize,
    std::string *outputError,
    std::stringstream *commentsGeneral,
    bool includeNoErrorInComments
  );
  bool SSLReadLoop(
    int numTries,
    std::string& output,
    const LargeInt& expectedLength,
    std::string* commentsOnFailure,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  bool SSLWriteLoop(
    int numTries,
    const std::string& input,
    std::string *outputError,
    std::stringstream *commentsGeneral,
    bool includeNoErrorInComments
  );
  int SSLWrite(
    void* buffer,
    int bufferSize,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream *commentsOnError,
    bool includeNoErrorInComments
  );
  TransportSecurityLayer();
  ~TransportSecurityLayer();
  void AddMoreEntropyFromTimer();
  bool HandShakeIamServer(int inputSocketID);
  bool InspectCertificates(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool HandShakeIamClientNoSocketCleanup(
    int inputSocketID, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
};

#endif
