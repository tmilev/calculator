#ifndef vpfHeaderTransportLayerSecurity_already_included
#define vpfHeaderTransportLayerSecurity_already_included

#include "general_lists.h"

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

#include "crypto.h"
#include "general_logging_global_variables.h"
class TransportLayerSecurity;
class TransportLayerSecurityOpenSSL {
public:
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
    std::string* outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  int SSLWrite(const List<char>& writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsOnError,
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

class TransportLayerSecurityServer;
class SSLRecord;
class SSLContent;

class CipherSuiteSpecification {
public:
  int id;
  std::string name;
  TransportLayerSecurityServer* owner;
  bool ComputeName();
  bool CheckInitialization() const;
  CipherSuiteSpecification();
  CipherSuiteSpecification(TransportLayerSecurityServer* owner, int inputType);
  std::string ToString() const;
  static unsigned int HashFunction(const CipherSuiteSpecification& input);
  unsigned int HashFunction() const;
};

class SSLHelloExtension {
public:
  SSLContent* owner;
  List<unsigned char> content;
  int theType;
  std::string Name();
  SSLHelloExtension();
  JSData ToJSON();
  bool CheckInitialization();
  bool ProcessMe(std::stringstream* commentsOnError);
  void WriteBytes(List<unsigned char>& output);
  void MakeEllipticCurvePointFormat(SSLContent* inputOwner);
  void MakeExtendedMasterSecret(SSLContent* inputOwner);
  void MakeGrease(SSLContent* inputOwner);
};

// SSL client hello helpful links.
// Detailed walkthrough of the handshake process:
// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art059
// Byte by byte annotated explanations of the handshake:
// https://tls.ulfheim.net
// https://serializethoughts.com/2014/07/27/dissecting-tls-client-hello-message/
// https://idea.popcount.org/2012-06-16-dissecting-ssl-handshake/

class SSLContent {
public:
  SSLRecord* owner;
  unsigned char theType;
  int length;
  int version;
  int cipherSpecLength;
  int challengeLength;
  int extensionsLength;
  int compressionMethod;
  int chosenCipher;
  bool flagRenegotiate;
  bool flagRequestOnlineCertificateStatusProtocol;
  bool flagRequestSignedCertificateTimestamp;
  static const int LengthRandomBytesInSSLHello = 32;
  List<unsigned char> renegotiationCharacters;
  List<unsigned char> RandomBytes;
  List<CipherSuiteSpecification> declaredCiphers;
  List<SSLHelloExtension> extensions;
  List<unsigned char> sessionId;
  List<unsigned char> challenge;
  class tokens {
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
  class tokensExtension {
  public:
    static const unsigned int renegotiateConnection = 255 * 256 + 1; // 0xff01
    static const unsigned int ellipticCurvePointFormat = 11; // 0x000b
    static const unsigned int requestOnlineCertificateStatus = 5; //0x0005
    static const unsigned int requestSignedCertificateTimestamp = 18; // 0x0012
    static const unsigned int serverName = 0; // 0x0000
    static const unsigned int extendedMasterSecret = 23; // 0x0017
  };

  SSLContent();
  void resetExceptOwner();
  TransportLayerSecurityServer& GetServer() const;
  bool CheckInitialization() const;
  logger::StringHighligher getStringHighlighter();
  bool Decode(std::stringstream* commentsOnFailure);
  bool DecodeSupportedCiphers(std::stringstream* commentsOnFailure);
  bool DecodeExtensions(std::stringstream* commentsOnFailure);
  bool ProcessExtensions(std::stringstream* commentsOnFailure);
  // https://tls.ulfheim.net
  // https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art059
  void PrepareServerHello1Start(SSLContent& clientHello);
  // https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art061
  void PrepareServerHello2Certificate();
  // https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art060
  void PrepareServerHello3SecretNegotiation(SSLContent& clientHello);

  JSData ToJSON() const;

  std::string ToStringVersion() const;
  // As the name suggests, this will append the output bytes, without
  // wiping the already existing contents of output.
  void WriteBytes(List<unsigned char>& output) const;
  // Writes the message header, using zeroes instead of the message length
  void WriteType(List<unsigned char>& output) const;
  void WriteVersion(List<unsigned char>& output) const;
  void WriteBytesHandshakeClientHello(List<unsigned char>& output) const;
  void WriteBytesHandshakeServerHello(List<unsigned char>& output) const;
  void WriteBytesHandshakeCertificate(List<unsigned char>& output) const;
  void WriteBytesRandomAndSessionId(List<unsigned char>& output) const;
  void WriteBytesSupportedCiphers(List<unsigned char>& output) const;
  void WriteBytesExtensionsOnly(List<unsigned char>& output) const;
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
  class Test {
  public:
    static bool Serialization();
  };
  int offsetDecoded;
  unsigned char theType;
  int version;
  int length;
  List<unsigned char> body;
  SSLContent content;
  TransportLayerSecurityServer* owner;
  SSLRecord();
  bool CheckInitialization() const;
  bool Decode(std::stringstream* commentsOnFailure);
  bool DecodeBody(std::stringstream* commentsOnFailure);
  std::string ToBytes() const;
  std::string ToString() const;
  std::string ToStringType() const;
  std::string ToJSON() const;

  void PrepareServerHello1Start(SSLRecord& clientHello);
  void PrepareServerHello2Certificate();
  void WriteBytes(List<unsigned char>& output) const;
};

class TransportLayerSecurityServer {
public:
  class NetworkSpoofer {
  public:
    TransportLayerSecurityServer* owner;
    bool flagDoSpoof;
    List<List<SSLRecord> > outgoingMessages;
    List<List<unsigned char> > incomingBytes;
    int currentInputMessageIndex;
    NetworkSpoofer();
    bool ReadBytesOnce(std::stringstream* commentsOnError);
    bool WriteSSLRecords(List<SSLRecord>& input);
  };
  NetworkSpoofer spoofer;
  X509Certificate certificate;
  PrivateKeyRSA privateKey;
  // Ordered by preference (lower index = more preferred):
  MapList<int, std::string, MathRoutines::IntUnsignIdentity> cipherSuiteNames;
  MapList<int, CipherSuiteSpecification, MathRoutines::IntUnsignIdentity> supportedCiphers;
  MapList<int, std::string, MathRoutines::IntUnsignIdentity> extensionNames;
  int socketId;
  int64_t millisecondsTimeOut;
  int64_t millisecondsDefaultTimeOut;
  int defaultBufferCapacity;
  SSLRecord lastRead;
  SSLRecord serverHelloStart;
  SSLRecord serverHelloCertificate;
  SSLRecord serverHelloKeyExchange;

  List<unsigned char> lastIncomingBytes;
  List<unsigned char> nextOutgoingBytes;
  List<SSLRecord> outgoingRecords;

  CipherSuiteSpecification GetCipherCrashIfUnknown(int inputId);
  void AddSupportedCipher(int inputId);
    void initialize();
  void initializeCipherSuites();
  TransportLayerSecurityServer();
  bool HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure);
  bool ReadBytesOnce(std::stringstream* commentsOnError);
  bool DecodeSSLRecord(std::stringstream* commentsOnFailure);
  bool ReplyToClientHello(int inputSocketID, std::stringstream* commentsOnFailure);
  bool ReadBytesDecodeOnce(std::stringstream* commentsOnFailure);
  bool WriteBytesOnce(List<unsigned char>& input, std::stringstream* commentsOnFailure);
  bool WriteSSLRecords(List<SSLRecord>& input, std::stringstream* commentsOnFailure);
};

class TransportLayerSecurity {
public:
  static bool flagBuiltInTLSAvailable;
  bool flagInitializedPrivateKey;
  bool flagIsServer;
  bool flagInitialized;
  bool flagUseBuiltInTlS;
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
  // Once the first function call returns, the function becomes thread-safe.
  static void initializeNonThreadSafePartsCommon();
  // First call of function (with any member function) is not thread safe (must be called in a single thread).
  // Once the first function call returns, the function becomes thread-safe for all members.
  void initializeNonThreadSafeOnFirstCall(bool IamServer);
  int SSLRead(std::string* outputError,
    std::stringstream* commentsGeneral,
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
    std::string* outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  int SSLWrite(
    List<char>& writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsOnError,
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
  bool initSSLKeyFiles(std::stringstream* commentsOnFailure);
  bool initSSLKeyFilesInternal(std::stringstream* commentsOnFailure);
  bool LoadPEMCertificate(std::stringstream *commentsOnFailure);
  bool LoadPEMPrivateKey(std::stringstream* commentsOnFailure);
  void Free();
  void FreeEverythingShutdown();
};

#endif
