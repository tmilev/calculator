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

class SSLServer {
public:
  ssl_st* theData;
  SSLServer();
  void Free();
};

struct TransportSecurityLayer {
public:
  static bool flagSSLlibraryInitialized;
  static bool flagDontUseOpenSSL;
  int errorCode;
  ssl_st* sslClient;
  SSLServer sslServeR;
  //X509* my_certificate;
  X509* peer_certificate;
  SSL_CTX* contextServer;
  //SSL_CTX* contextClient;
  const SSL_METHOD* theSSLMethod;
  List<int> socketStackServer;
  List<int> socketStackClient;
  List<char> buffer;
  struct errors {
    static std::string errorWantRead;
  };
  std::string otherCertificateIssuerName, otherCertificateSubjectName;
  bool flagSSLHandshakeSuccessful;

  static const std::string fileCertificate;
  static const std::string fileCertificateConfiguration;
  static const std::string fileKey;
  static const std::string signedFileCertificate1;
  static const std::string signedFileCertificate3;
  static const std::string signedFileKey;
  void DoSetSocket(int theSocket, SSL *theSSL);
  void SetSocketAddToStackServer(int theSocket);
  void RemoveLastSocketServer();
  void SetSocketAddToStackClient(int theSocket);
  void RemoveLastSocketClient();
  void FreeClientSSL();
  void ClearErrorQueue(
    int errorCode,
    SSL* theSSL,
    std::string *outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  static bool initSSLkeyFiles();
  void initSSLlibrary();
  void initSSLserver();
  void initSSLclient();
  int SSLRead(
    SSL* theSSL,
    void *buffer,
    int bufferSize,
    std::string *outputError,
    std::stringstream *commentsGeneral,
    bool includeNoErrorInComments
  );
  bool SSLReadLoop(
    int numTries,
    SSL* theSSL,
    std::string& output,
    const LargeInt& expectedLength,
    std::string* commentsOnFailure,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  bool SSLWriteLoop(
    int numTries,
    SSL* theSSL,
    const std::string& input,
    std::string *outputError,
    std::stringstream *commentsGeneral,
    bool includeNoErrorInComments
  );
  int SSLWrite(SSL* theSSL,
    void* buffer,
    int bufferSize,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream *commentsOnError,
    bool includeNoErrorInComments
  );
  TransportSecurityLayer();
  ~TransportSecurityLayer();
  void FreeSSL();
  void FreeContext();
  void AddMoreEntropyFromTimer();
  bool HandShakeIamServer(int inputSocketID);
  bool InspectCertificates(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool HandShakeIamClientNoSocketCleanup(
    int inputSocketID, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  void FreeEverythingShutdownSSL();
};

#endif
