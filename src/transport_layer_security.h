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

#include "vpfHeader5Crypto.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
class TransportLayerSecurity;
struct TransportLayerSecurityOpenSSL {
  std::string name;
  TransportLayerSecurity* owner;
  int errorCode;
  static bool flagSSLlibraryInitialized;
  SSL* sslData;
  X509* peer_certificate;
  SSL_CTX* context;
  bool flagSSLHandshakeSuccessful;
  const SSL_METHOD* theSSLMethod;
  bool flagContextInitialized;
  bool flagIsServer;
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
  int SSLWrite(const List<char> &writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream *commentsOnError,
    bool includeNoErrorInComments
  );
  bool CheckCanInitializeToClient();
  bool CheckCanInitialize(bool toServer);
  bool CheckCanInitializeToServer();
  void DoSetSocket(int theSocket);
  void SetSocketAddToStack(int theSocket);
  void RemoveLastSocket();
  bool HandShakeIamServer(int inputSocketID);
  bool InspectCertificates(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  static bool initSSLKeyFiles();
  static bool initSSLKeyFilesCreateOnDemand();
};

class SSLRecord;

class CipherSuiteSpecification {
public:
  int encoding;
  std::string name;
  bool ComputeName();
};

// SSL client hello helpful links.
// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art059
// https://serializethoughts.com/2014/07/27/dissecting-tls-client-hello-message/
// https://idea.popcount.org/2012-06-16-dissecting-ssl-handshake/
//
class SSLHello {
public:
  unsigned char handshakeType;
  int length;
  int version;
  int cipherSpecLength;
  int sessionIdLength;
  int challengeLength;
  static const int LengthRandomBytesInSSLHello = 32;
  List<char> RandomBytes;
  List<CipherSuiteSpecification> supportedCiphers;
  List<char> sessionId;
  List<char> challenge;
  SSLHello();
  logger::StringHighligher getStringHighlighter();
  bool Decode(SSLRecord& owner, std::stringstream* commentsOnFailure);
  JSData ToJSON() const;
  std::string ToStringVersion() const;
};

// A basic explanation of ssl records:
// https://www.cisco.com/c/en/us/support/docs/security-vpn/secure-socket-layer-ssl/116181-technote-product-00.html
class SSLRecord {
public:
  class tokens {
  public:
    static const unsigned char handshake = 22; // 0x16
    static const unsigned char changeCipherSpec = 20; // 0x14
    static const unsigned char alert = 21; //0x15
    static const unsigned char applicationData = 23; //0x17
    static const unsigned char unknown = 0;
  };
  int offsetDecoded;
  unsigned char theType;
  int version;
  int length;
  List<unsigned char> body;
  SSLHello hello;
  SSLRecord();
  bool Decode(std::stringstream* commentsOnFailure);
  bool DecodeBody(std::stringstream* commentsOnFailure);
  std::string ToString() const;
  std::string ToStringType() const;
};

class TransportLayerSecurityServer {
public:
  class recordsHandshake {
  public:
    static const unsigned char helloRequest = 0; //0x00
    static const unsigned char clientHello = 1; //0x01
    static const unsigned char serverHello = 2; //0x02
    static const unsigned char certificate = 11; //0x0b
    static const unsigned char serverKeyExchange = 12; //0x0c
    static const unsigned char certificateRequest = 13; //0x0d
    static const unsigned char serverHelloDone = 14; //0x0e
    static const unsigned char certificateVerify = 15; //0x0f
    static const unsigned char clientKeyExchange = 16; //0x10
    static const unsigned char finished = 20; //0x14
  };
  static MapLisT<int, std::string, MathRoutines::IntUnsignIdentity>& cipherSuites();
  int socketId;
  int64_t millisecondsTimeOut;
  int64_t millisecondsDefaultTimeOut;
  int defaultBufferCapacity;
  SSLRecord lastRecord;
  TransportLayerSecurityServer();
  bool HandShakeIamServer(int inputSocketID, std::stringstream *commentsOnFailure);
  bool ReadBytesOnce();
  bool DecodeSSLRecord(std::stringstream* commentsOnFailure);
  bool ReadBytesDecodeOnce(std::stringstream *commentsOnFailure);
  void WriteBytesOnce();
  void ReadLoop();
  void WriteLoop();
};

class TransportLayerSecurity {
public:
  static bool flagDontUseOpenSSL;
  bool flagInitializedPrivateKey;
  bool flagIsServer;
  bool flagInitialized;
  X509Certificate theCertificate;
  TransportLayerSecurityOpenSSL openSSLData;
  TransportLayerSecurityServer theServer;
  List<char> readBuffer;
  List<char> writeBuffer;
  int readBufferStandardSize;

  static const std::string fileCertificate;
  static const std::string fileKey;
  static const std::string signedFileCertificate1;
  static const std::string signedFileCertificate3;
  static const std::string signedFileKey;
  void initialize(bool IamServer);
  int SSLRead(std::string *outputError,
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
  bool HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure);
  bool HandShakeIamClientNoSocketCleanup(
    int inputSocketID, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  bool initSSLKeyFiles();
  bool initSSLKeyFilesInternal(std::stringstream* commentsOnFailure);
  bool LoadPEMCertificate(std::stringstream *commentsOnFailure);
  bool LoadPEMPrivateKey(std::stringstream* commentsOnFailure);
  void Free();
  void FreeEverythingShutdown();
};

#endif
