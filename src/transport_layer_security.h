#ifndef header_transport_layer_security_ALREADY_INCLUDED
#define header_transport_layer_security_ALREADY_INCLUDED

#include "general_lists.h"
#include "serialization_basic.h"
#include "math_extra_elliptic_curves.h"
#include "math_extra_algebraic_numbers.h"

// installation of these headers in ubuntu:
// sudo apt-get install libssl-dev
// on opensuse:
// sudo yast -i libopenssl-devel
#include "crypto_calculator.h"
#include "general_logging_global_variables.h"

class TransportLayerSecurity;

class TransportLayerSecurityConfiguration {
  // Contains the string "certificates/"
  // The relative file name of the folder
  // where the certificates + private keys of the
  // server are stored. This folder is in one of the
  // "ultra-sensitive" folders that are
  // guarded by the
  // sanitizers in FileOperations.
  static const std::string certificateFolder;
  static const std::string selfSignedCertificate;
  static const std::string selfSignedPrivateKey;
  // Contains the string "certificates/additional/".
  // Similar to the certificatesFolder, but has additional certificates to be
  // used by stand-alone web server configurations.
  static const std::string additionalCertificateFolder;
  void readCertificateFilename();
  void readPrivateKeyFilename();
public:
  // The physical file name of the certificate. Not sanitized once created.
  std::string certificateFileNamePhysical;
  // The private key file name. Not sanitized once created.
  std::string privateKeyFileNamePhysical;
  // Makes this the official certificate configuration.
  void makeOfficialKeyAndCertificate();
  bool makeSelfSignedKeyAndCertificate();
  bool makeFromAdditionalKeyAndCertificate(
    const std::string& key, const std::string& certificate
  );
  bool keysExist() const;
};

// Forward-declare openssl dependencies
typedef struct ssl_st SSL;
typedef struct x509_st X509;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_method_st SSL_METHOD;

class TransportLayerSecurityOpenSSL {
public:
  SSL* sslData;
  X509* peer_certificate;
  // One context per program as per ssl documetnation.
  static SSL_CTX* contextGlobal;
  static const SSL_METHOD* methodGlobal;
  TransportLayerSecurityConfiguration configuration;
  TransportLayerSecurity* owner;
  std::string name;
  int errorCode;
  static bool flagSSLContextInitialized;
  bool flagSSLHandshakeSuccessful;
  bool flagSSLInitialized;
  bool flagIsServer;
  List<int> socketStack;
  struct errors {
    static std::string errorWantRead;
  };

  std::string otherCertificateIssuerName;
  std::string otherCertificateSubjectName;
  TransportLayerSecurityOpenSSL();
  ~TransportLayerSecurityOpenSSL();
  // One call per program run.
  static void freeContextGlobal();
  void freeSession();
  void initializeSSLLibrary();
  void initializeSSLServer();
  void initializeSSL(bool isServer);
  void initializeSSLServerCertificatesSelfSigned();
  void initializeOneCertificate(TransportLayerSecurityConfiguration& input);
  void initializeSSLClient();
  void reset();
  void clearErrorQueue(int numberOfTransferredBytes);
  bool handShakeIAmClientNoSocketCleanup(
    int inputSocketID,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  int sslRead(
    List<char>& readBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  int sslWrite(
    const List<char>& writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsOnError,
    bool includeNoErrorInComments
  );
  bool checkCanInitializeToClient();
  bool checkCanInitialize(bool toServer);
  bool checkCanInitializeToServer();
  void doSetSocket(int socketFileDescriptor);
  void setSocketAddToStack(int socketFileDescriptor);
  void removeLastSocket();
  bool handShakeIamServer(
    int inputSocketID, std::stringstream* commentsOnFailure
  );
  bool inspectCertificates(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool initializeSSLKeyFilesSelfSignedCreateOnDemand();
  bool hasOKCertificates();
};

class TransportLayerSecurityServer;
class SSLRecord;
class SSLContent;

class CipherSuiteSpecification {
public:
  class EllipticCurveSpecification {
  public:
    static const int sect163k1 = 1;
    static const int sect163r1 = 2;
    static const int sect163r2 = 3;
    static const int sect193r1 = 4;
    static const int sect193r2 = 5;
    static const int sect233k1 = 6;
    static const int sect233r1 = 7;
    static const int sect239k1 = 8;
    static const int sect283k1 = 9;
    static const int sect283r1 = 10;
    static const int sect409k1 = 11;
    static const int sect409r1 = 12;
    static const int sect571k1 = 13;
    static const int sect571r1 = 14;
    static const int secp160k1 = 15;
    static const int secp160r1 = 16;
    static const int secp160r2 = 17;
    static const int secp192k1 = 18;
    static const int secp192r1 = 19;
    static const int secp224k1 = 20;
    static const int secp224r1 = 21;
    static const int secp256k1 = 22;
    static const int secp256r1 = 23;
    static const int secp384r1 = 24;
    static const int secp521r1 = 25;
    static const int curve25519 = 30;
  };

  int id;
  std::string name;
  TransportLayerSecurityServer* owner;
  bool computeName();
  bool checkInitialization() const;
  CipherSuiteSpecification();
  CipherSuiteSpecification(
    TransportLayerSecurityServer* owner, int inputType
  );
  std::string toString() const;
  static unsigned int hashFunction(const CipherSuiteSpecification& input);
  unsigned int hashFunction() const;
};

class SignatureAlgorithmSpecification {
public:
  // The older RFC (RFC 5246, page 45)
  // requests that algorithm
  // specifications be specified through two bytes.
  // The first byte would specify the hash algorithm,
  // and the second byte - the signature algorithm.
  class HashAlgorithm {
  public:
    static const unsigned char none = 0;
    static const unsigned char md5 = 1;
    static const unsigned char sha1 = 2;
    static const unsigned char sha224 = 3;
    static const unsigned char sha256 = 4;
    static const unsigned char sha384 = 5;
    static const unsigned char sha512 = 6;
  };

  class SignatureAlgorithm {
  public:
    static const unsigned char anonymous = 0;
    static const unsigned char RSA = 1;
    static const unsigned char DSA = 2;
    static const unsigned char ECDSA = 3;
  };

  // Newer RFC 8446
  // fixes the allowed two-byte combinations.
  // Page 41, RFC 8446
  // https://tools.ietf.org/html/rfc8446#page-41
  class AlgorithmPacks {
  public:
    // RSASSA-PKCS1-v1_5 algorithms
    static const unsigned int rsa_pkcs1_sha256 = 0x0401;
    static const unsigned int rsa_pkcs1_sha384 = 0x0501;
    static const unsigned int rsa_pkcs1_sha512 = 0x0601;
    // ECDSA algorithms
    static const unsigned int ecdsa_secp256r1_sha256 = 0x0403;
    static const unsigned int ecdsa_secp384r1_sha384 = 0x0503;
    static const unsigned int ecdsa_secp521r1_sha512 = 0x0603;
    // RSASSA-PSS algorithms with public key OID rsaEncryption
    static const unsigned int rsa_pss_rsae_sha256 = 0x0804;
    static const unsigned int rsa_pss_rsae_sha384 = 0x0805;
    static const unsigned int rsa_pss_rsae_sha512 = 0x0806;
    // EdDSA algorithms
    static const unsigned int ed25519 = 0x0807;
    static const unsigned int ed448 = 0x0808;
    // RSASSA-PSS algorithms with public key OID RSASSA-PSS
    static const unsigned int rsa_pss_pss_sha256 = 0x0809;
    static const unsigned int rsa_pss_pss_sha384 = 0x080a;
    static const unsigned int rsa_pss_pss_sha512 = 0x080b;
    // Legacy algorithms
    static const unsigned int rsa_pkcs1_sha1 = 0x0201;
    static const unsigned int ecdsa_sha1 = 0x0203;
  };

  class JSLabels {
  public:
    static std::string valueHex;
    static std::string hash;
    static std::string hashName;
    static std::string signatureAlgorithm;
    static std::string signatureAlgorithmName;
    static std::string interpretation;
  };

  TransportLayerSecurityServer* ownerServer;
  unsigned int value;
  unsigned int hash;
  unsigned int signatureAlgorithm;
  SignatureAlgorithmSpecification();
  JSData toJSON();
  void processValue();
  std::string toHex();
  std::string getHashName();
  std::string getSignatureAlgorithmName();
};

// At the time of writing, a list of extensions **should** be available here:
// https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml
// Please DO update the documentation if that stops being the case.
class SSLHelloExtension {
public:
  SSLContent* owner;
  List<unsigned char> content;
  int extensionType;
  std::string name();
  SSLHelloExtension();
  JSData toJSON();
  bool checkInitialization();
  bool processMe(std::stringstream* commentsOnError);
  bool processRenegotiateConnection(std::stringstream* commentsOnError);
  bool processServerName(std::stringstream* commentsOnError);
  bool processRequestOnlineCertificateStatus(
    std::stringstream* commentsOnError
  );
  bool processRequestSignedCertificateTimestamp(
    std::stringstream* commentsOnError
  );
  bool processSignatureAlgorithms(std::stringstream* commentsOnError);
  void writeBytes(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  );
  void makeEllipticCurvePointFormat(SSLContent* inputOwner);
  void makeExtendedMasterSecret(SSLContent* inputOwner);
  void makeGrease(SSLContent* inputOwner);
};

// SSL client hello helpful links.
// Detailed walkthrough of the handshake process:
// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art059
// Byte by byte annotated explanations of the handshake:
// https://tls.ulfheim.net
// https://serializethoughts.com/2014/07/27/dissecting-tls-client-hello-message/
// https://idea.popcount.org/2012-06-16-dissecting-ssl-handshake/
// TLS Protocol Version 1.0:
// RFC2246
// https://tools.ietf.org/search/rfc2246
// TLS Protocol Version 2.0:
// RFC5246
// https://tools.ietf.org/search/rfc5246
class SSLContent {
public:
  SSLRecord* owner;
  unsigned char contentType;
  int length;
  int version;
  int cipherSpecLength;
  int challengeLength;
  int extensionsLength;
  int compressionMethod;
  bool flagRenegotiate;
  bool flagIncomingRandomIncluded;
  bool flagOutgoingRandomIncluded;
  static const int LengthRandomBytesInSSLHello = 32;
  List<SSLHelloExtension> extensions;
  List<unsigned char> sessionId;
  List<unsigned char> challenge;
  class JSLabels {
  public:
    static std::string incomingRandom;
    static std::string outgoingRandom;
    static std::string version;
    static std::string length;
    static std::string cipherSpecLength;
    static std::string sessionId;
    static std::string extensionsLength;
    static std::string compressionMethod;
    static std::string extensions;
    static std::string renegotiate;
    static std::string OCSPrequest;
    static std::string signedCertificateTimestampRequest;
  };

  class tokens {
  public:
    static const unsigned char helloRequest = 0;
    // 0x00
    static const unsigned char clientHello = 1;
    // 0x01
    static const unsigned char serverHello = 2;
    // 0x02
    static const unsigned char certificate = 11;
    // 0x0b
    static const unsigned char serverKeyExchange = 12;
    // 0x0c
    static const unsigned char certificateRequest = 13;
    // 0x0d
    static const unsigned char serverHelloDone = 14;
    // 0x0e
    static const unsigned char certificateVerify = 15;
    // 0x0f
    static const unsigned char clientKeyExchange = 16;
    // 0x10
    static const unsigned char finished = 20;
    // 0x14
  };

  // Extensions should be available at:
  // https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml
  class tokensExtension {
  public:
    static const unsigned int serverName = 0;
    // 0x0000
    static const unsigned int requestOnlineCertificateStatus = 5;
    // 0x0005
    static const unsigned int requestSignedCertificateTimestamp = 18;
    // 0x0012
    static const unsigned int ellipticCurvePointFormat = 11;
    // 0x000b
    // used in server key exchange message, critical.
    static const unsigned int signatureAlgorithms = 13;
    // 0x00d
    static const unsigned int extendedMasterSecret = 23;
    // 0x0017
    static const unsigned int renegotiateConnection = 255 * 256 + 1;
    // 0xff01
  };

  static const unsigned char namedCurve = 3;
  SSLContent();
  static std::string getType(unsigned char token);
  void resetExceptOwner();
  TransportLayerSecurityServer& GetServer() const;
  bool checkInitialization() const;
  Logger::StringHighligher getStringHighlighter();
  bool decode(std::stringstream* commentsOnFailure);
  bool decodeSupportedCiphers(std::stringstream* commentsOnFailure);
  bool decodeExtensions(std::stringstream* commentsOnFailure);
  bool processExtensions(std::stringstream* commentsOnFailure);
  // https://tls.ulfheim.net
  // https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art059
  void prepareServerHello1Start(SSLContent& clientHello);
  // https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art061
  void prepareServerHello2Certificate();
  // https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art060
  bool prepareServerHello3ServerKeyExchange(
    std::stringstream* commentsOnError
  );
  JSData toJSON() const;
  std::string ToStringVersion() const;
  // As the name suggests, this will append the output bytes, without
  // wiping the already existing contents of output.
  void writeBytes(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeType(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeVersion(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesHandshakeClientHello(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesHandshakeServerHello(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesHandshakeCertificate(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesHandshakeSecretExchange(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  List<unsigned char>& getMyRandomBytes() const;
  void writeBytesIncomingRandomAndSessionId(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesMyRandomAndSessionId(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesSupportedCiphers(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
  void writeBytesExtensionsOnly(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
};

// A basic explanation of ssl records:
// https://www.cisco.com/c/en/us/support/docs/security-vpn/secure-socket-layer-ssl/116181-technote-product-00.html
// TLS Protocol Version 1.0:
// RFC2246
// https://tools.ietf.org/search/rfc2246
class SSLRecord {
public:
  class tokens {
  public:
    static const unsigned char handshake = 22;
    // 0x16
    static const unsigned char changeCipherSpec = 20;
    // 0x14
    static const unsigned char alert = 21;
    // 0x15
    static const unsigned char applicationData = 23;
    // 0x17
    static const unsigned char unknown = 0;
  };

  class JSLabels {
  public:
    static std::string type;
    static std::string content;
    static std::string session;
  };

  class Test {
  public:
    static std::string sampleClientHelloHex;
    static bool all();
    static bool serialization();
    static bool serializationClientHello(
      TransportLayerSecurityServer& testServer
    );
  };

  int offsetDecoded;
  unsigned char recordType;
  int version;
  int length;
  List<unsigned char> incomingBytes;
  SSLContent content;
  TransportLayerSecurityServer* owner;
  SSLRecord();
  void resetExceptOwner();
  bool checkInitialization() const;
  bool decode(std::stringstream* commentsOnFailure);
  bool decodeBody(std::stringstream* commentsOnFailure);
  std::string toHtml(int id);
  std::string toString() const;
  std::string toStringType() const;
  JSData toJSONSerialization();
  JSData toJSON();
  void prepareServerHello1Start(SSLRecord& clientHello);
  void prepareServerHello2Certificate();
  bool prepareServerHello3SecretExchange(
    std::stringstream* commentsOnFailure
  );
  void writeBytes(
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations
  ) const;
};

class TransportLayerSecurityServer {
public:
  class JSLabels {
  public:
    static std::string spoofer;
    static std::string session;
  };

  class NetworkSpoofer {
  public:
    class JSLabels {
    public:
      static std::string inputMessages;
      static std::string outputMessages;
      static std::string errorsOnInput;
      static std::string errorsOnOutput;
    };

    TransportLayerSecurityServer* owner;
    bool flagDoSpoof;
    List<List<SSLRecord> > outgoingMessages;
    List<SSLRecord> incomingMessages;
    List<std::string> errorsOnOutgoing;
    List<std::string> errorsOnIncoming;
    int currentInputMessageIndex;
    NetworkSpoofer();
    bool readBytesOnce(std::stringstream* commentsOnError);
    bool writeSSLRecords(List<SSLRecord>& input);
    JSData toJSON();
  };

  NetworkSpoofer spoofer;
  class Session {
    class JSLabels {
    public:
      static std::string chosenCipher;
      static std::string chosenCipherName;
      static std::string incomingRandomBytes;
      static std::string myRandomBytes;
      static std::string OCSPrequest;
      static std::string signedCertificateTimestampRequest;
      static std::string cipherSuites;
      static std::string serverName;
      static std::string algorithmSpecifications;
      static std::string ellipticCurveId;
      static std::string ellipticCurveName;
      static std::string bytesToSign;
    };

  public:
    int socketId;
    int chosenCipher;
    int chosenSignatureAlgorithm;
    int chosenEllipticCurve;
    std::string chosenEllipticCurveName;
    bool flagRequestOnlineCertificateStatusProtocol;
    bool flagRequestSignedCertificateTimestamp;
    TransportLayerSecurityServer* owner;
    List<unsigned char> incomingRandomBytes;
    List<unsigned char> myRandomBytes;
    List<unsigned char> bytesToSign;
    List<unsigned char> signature;
    List<SignatureAlgorithmSpecification> incomingAlgorithmSpecifications;
    LargeIntegerUnsigned ephemerealPrivateKey;
    ElementEllipticCurve<ElementZmodP> ephemerealPublicKey;
    List<unsigned char> serverName;
    List<CipherSuiteSpecification> incomingCiphers;
    std::string toStringChosenCipher();
    JSData toJSON();
    Session();
    bool computeAndSignEphemerealKey(std::stringstream* commentsOnError);
    bool chooseCipher(std::stringstream* commentsOnFailure);
    void initialize();
    bool setIncomingRandomBytes(
      List<unsigned char>& input, std::stringstream* commentsOnFailure
    );
    void writeNamedCurveAndPublicKey(
      List<unsigned char>& output,
      List<Serialization::Marker>* annotations
    ) const;
  };

  TransportLayerSecurity* owner;
  TransportLayerSecurityServer::Session session;
  X509Certificate certificate;
  PrivateKeyRSA privateKey;
  // Ordered by preference (lower index = more preferred):
  MapList<int, std::string, HashFunctions::hashFunction> cipherSuiteNames;
  MapList<int, CipherSuiteSpecification, HashFunctions::hashFunction>
  supportedCiphers;
  MapList<int, std::string, HashFunctions::hashFunction> extensionNames;
  int64_t millisecondsTimeOut;
  int64_t millisecondsDefaultTimeOut;
  int defaultBufferCapacity;
  SSLRecord lastRead;
  SSLRecord serverHelloStart;
  SSLRecord serverHelloCertificate;
  SSLRecord serverHelloKeyExchange;
  List<unsigned char> incomingBytes;
  List<unsigned char> outgoingBytes;
  List<SSLRecord> outgoingRecords;
  CipherSuiteSpecification getCipherNoFailure(int inputId);
  void addSupportedCipher(int inputId);
  bool initializeAll(
    const std::string& privateKeyPEMEncoded,
    const std::string& serverCertificatePEMEncoded,
    std::stringstream* commentsOnError
  );
  void initialize();
  void initializeCipherSuites();
  void initializeExtensions();
  TransportLayerSecurityServer();
  bool handShakeIamServer(
    int inputSocketID, std::stringstream* commentsOnFailure
  );
  bool readBytesOnce(std::stringstream* commentsOnError);
  bool decodeSSLRecord(std::stringstream* commentsOnFailure);
  bool replyToClientHello(
    int inputSocketID, std::stringstream* commentsOnFailure
  );
  bool readBytesDecodeOnce(std::stringstream* commentsOnFailure);
  bool writeBytesOnce(
    List<unsigned char>& input, std::stringstream* commentsOnFailure
  );
  bool writeSSLRecords(
    List<SSLRecord>& input, std::stringstream* commentsOnFailure
  );
  JSData toJSON();
  bool initializeCertificatesSelfSigned(
    std::stringstream* commentsOnFailure
  );
  bool loadSelfSignedPEMCertificate(std::stringstream* commentsOnFailure);
  bool loadSelfSignedPEMPrivateKey(std::stringstream* commentsOnFailure);
};

class TransportLayerSecurity {
public:
  // Whether this is a server or a client connection.
  // A server connection occurs when you connect to
  // the computer algebra system and it is your server.
  // A client connection happens when, for some reason, the
  // server needs to look up something on the internet by itself.
  // For example, this could be some login information from a login provider
  // such as google. In this case, this flag would be false and we
  // would be the client.
  bool flagIsServer;
  bool flagInitialized;
  bool flagUseBuiltInTlS;
  bool flagBuiltInTLSAvailable;
  TransportLayerSecurityOpenSSL openSSLData;
  TransportLayerSecurityServer server;
  List<char> readBuffer;
  List<char> writeBuffer;
  int readBufferStandardSize;
  // The current listening socket, where the port is bound.
  // Not the same as the connection socket.
  int currentListeningSocket;
  static TransportLayerSecurity& defaultTLS_READ_ONLY();
  TransportLayerSecurityConfiguration selfSigned;
  // Certificate + private key pair signed by an external authority
  // such as GoDaddy or another institution.
  // This will be the (.crt, .key) file pair
  // in the certificates/ folder that is not
  // the self-signed.
  // If there are multiple .crt file candidates,
  // the server will crash.
  TransportLayerSecurityConfiguration official;
  // Map from ports to certificates.
  MapReferences<std::string, TransportLayerSecurityOpenSSL>
  additionalConfigurations;
  // A map from sockets to additional configurations.
  MapList<int, int> socketsToAdditionalConfigurations;
  // Once the first function call returns, the function becomes thread-safe.
  static void initializeNonThreadSafePartsCommon();
  // First call of function (with any member function) is not thread safe (must
  // be called in a single thread).
  // Once the first function call returns, the function becomes thread-safe for
  // all members.
  void initializeNonThreadSafeOnFirstCall(bool isServer);
  TransportLayerSecurityOpenSSL& getOpenSSLData();
  int readOnce(
    int socket,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  bool sslReadLoop(
    int totalTries,
    std::string& output,
    const LargeInteger& expectedLength,
    std::string* commentsOnFailure,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  bool sslWriteLoop(
    int numberOfTries,
    const std::string& input,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    bool includeNoErrorInComments
  );
  int writeOnce(
    int socket,
    List<char>& writeBuffer,
    std::string* outputError,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsOnError,
    bool includeNoErrorInComments
  );
  static std::string keySelfSignedVirtual();
  TransportLayerSecurity();
  ~TransportLayerSecurity();
  void removeLastSocket();
  bool handShakeIamServer(
    int inputSocketID, std::stringstream* commentsOnFailure
  );
  bool handShakeIAmClientNoSocketCleanup(
    int inputSocketID,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool initializeAdditionalCertificates();
  void free();
  void freeEverythingShutdown();
};

#endif // header_transport_layer_security_ALREADY_INCLUDED
