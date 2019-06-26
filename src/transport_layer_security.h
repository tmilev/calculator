#ifndef vpfHeaderTransportLayerSecurity_already_included
#define vpfHeaderTransportLayerSecurity_already_included

#include "vpfHeader1General0_General.h"

static ProjectInformationInstance projectInfoInstanceTransportLayerSecurityHeader(__FILE__, "TLS/ssl header.");

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

class TransportLayerSecurity;
struct TransportLayerSecurityOpenSSL {
  TransportLayerSecurity* owner;
  int errorCode;
  static bool flagSSLlibraryInitialized;
  SSL* sslData;
  X509* peer_certificate;
  SSL_CTX* contextServer;
  bool flagSSLHandshakeSuccessful;
  const SSL_METHOD* theSSLMethod;
  List<int> socketStack;
  struct errors {
    static std::string errorWantRead;
  };
  std::string otherCertificateIssuerName, otherCertificateSubjectName;
  TransportLayerSecurityOpenSSL();
  ~TransportLayerSecurityOpenSSL();
  void FreeSSL();
  void FreeContext();
  void FreeEverythingShutdownSSL();
  void initSSLLibrary();
  void initSSLServer();
  void initSSLClient();
  void initSSLCommon(bool isServer);
  void Reset();
  void ClearErrorQueue(
    int errorCode,
    std::string *outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  bool HandShakeIamClientNoSocketCleanup(
    int inputSocketID,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  int SSLRead(
    List<char>& readBuffer,
    std::string *outputError,
    std::stringstream *commentsGeneral,
    bool includeNoErrorInComments
  );
  int SSLWrite(
    List<char>& writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream *commentsOnError,
    bool includeNoErrorInComments
  );
  void DoSetSocket(int theSocket);
  void SetSocketAddToStack(int theSocket);
  void RemoveLastSocket();
  bool HandShakeIamServer(int inputSocketID);
  bool InspectCertificates(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  static bool initSSLkeyFiles();
};

class TransportLayerSecurity {
public:
  static bool flagDontUseOpenSSL;
  bool flagInitializedPrivateKey;
  bool flagIsServer;
  List<char> buffer;
  TransportLayerSecurityOpenSSL openSSLData;

  int standardBufferSize;

  static const std::string fileCertificate;
  static const std::string fileCertificateConfiguration;
  static const std::string fileKey;
  static const std::string signedFileCertificate1;
  static const std::string signedFileCertificate3;
  static const std::string signedFileKey;
  void initialize(bool IamServer);
  int SSLRead(
    List<char>& readBuffer,
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
    List<char>& writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream *commentsOnError,
    bool includeNoErrorInComments
  );
  TransportLayerSecurity();
  ~TransportLayerSecurity();
  void RemoveLastSocket();
  void AddMoreEntropyFromTimer();
  bool HandShakeIamServer(int inputSocketID);
  bool HandShakeIamClientNoSocketCleanup(
    int inputSocketID, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  bool initSSLKeyFiles();
  void Free();
  void FreeEverythingShutdown();
};

#endif
