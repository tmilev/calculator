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


class CipherSpec {

};

// Specs of ssl client hello:
// Section E.1 of https://www.ietf.org/rfc/rfc2246.txt

class SSLHello {
public:
  unsigned char handshakeType;
  int version;
  int cipherSpecLength;
  int sessionIdLength;
  int challengeLength;
  List<CipherSpec> supportedCiphers;
  List<char> sessionId;
  List<char> challenge;
  SSLHello();
  bool Decode(std::stringstream* commentsOnFailure);
};

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
  unsigned char theType;
  int version;
  int length;
  List<char> body;
  SSLHello hello;
  SSLRecord();
  bool Decode(List<char>& input, int offset, std::stringstream* commentsOnFailure);
  bool DecodeBody(std::stringstream* commentsOnFailure);
  std::string ToString() const;
  std::string ToStringType() const;
};

class TransportLayerSecurityServer {
public:
  class recordsHandshake {
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
  int socketId;
  int64_t millisecondsTimeOut;
  int64_t millisecondsDefaultTimeOut;
  int defaultBufferCapacity;
  List<char> lastRead;
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
