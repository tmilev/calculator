// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"
#include "serialization_basic.h"

#include <unistd.h> //<- close, open defined here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <iomanip>
#include <sys/time.h> // <- timeval

// http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
// openssl req -x509 -keyalg RSA -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 3001
// Alternatively:
// certificate with certificate signing request:
// openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key
// then get the CSR.csr file to a signing authority,
// from where you get the signedFileCertificate1 and signedFileCertificate3
const std::string TransportLayerSecurity::certificateSelfSignedPem = "cert.pem";
const std::string TransportLayerSecurity::keySelfSigned = "key.pem";

const std::string TransportLayerSecurity::certificateFolder = "certificates/";

TransportLayerSecurity::TransportLayerSecurity() {
  this->flagIsServer = true;
  this->flagInitializedPrivateKey = false;
  this->flagInitialized = false;
  this->openSSLData.owner = this;
  this->readBufferStandardSize = 100000;
  this->flagUseBuiltInTlS = false;
}

TransportLayerSecurity::~TransportLayerSecurity() {
}

void TransportLayerSecurity::initializeNonThreadSafePartsCommon() {
  ASNObject::initializeNonThreadSafe();
}

void TransportLayerSecurity::initializeNonThreadSafeOnFirstCall(bool isServer) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initializeNonThreadSafeOnFirstCall");
  if (this->flagInitialized) {
    return;
  }
  this->initializeNonThreadSafePartsCommon();
  if (isServer) {
    this->openSSLData.initSSLServer();
    bool flagBuiltInTLSAvailable = false;
    if (flagBuiltInTLSAvailable) {
      this->server.initialize();
    }
  } else {
    this->openSSLData.initSSLClient();
  }
  this->flagInitialized = true;
}

void TransportLayerSecurity::freeEverythingShutdown() {
  MacroRegisterFunctionWithName("TransportLayerSecurity::freeEverythingShutdown");
  this->openSSLData.freeSession();
}

bool TransportLayerSecurity::initSSLKeyFiles(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initSSLKeyFiles");
  if (!this->openSSLData.initSSLKeyFilesSelfSigned()) {
    return false;
  }
  bool flagBuiltInTLSAvailable = false;
  if (!flagBuiltInTLSAvailable) {
    return true;
  }
  if (!this->initSSLKeyFilesInternal(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      global.fatal << commentsOnFailure->str();
    }
    global.fatal << "Failed to initialize ssl keys of built-in tls server. " << global.fatal;
  }
  return true;
}

bool TransportLayerSecurity::sslReadLoop(
  int numTries,
  std::string& output,
  const LargeInteger& expectedLength,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::sslReadLoop");
  if (!global.flagUsingSSLinCurrentConnection || !global.flagSSLAvailable) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Error in ssl read loop: ssl not available in current connection. ";
    }
    return false;
  }
  this->readBuffer.setSize(this->readBufferStandardSize);
  output = "";
  int i = 0;
  std::string next;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->readOnce(
      - 1,
      outputError,
      commentsGeneral,
      includeNoErrorInComments
    );
    if (numBytes > 0) {
      next.assign(this->readBuffer.objects, static_cast<unsigned>(numBytes));
      output += next;
      if (expectedLength <= 0) {
        break;
      } else {
        if (expectedLength <= static_cast<int>(output.size())) {
          break;
        }
      }
    }
    if (numBytes == 0) {
      break;
    }
  }
  if (numBytes < 0) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "SSL-ERROR reading from socket. ";
    }
    return false;
  }
  return true;
}

bool TransportLayerSecurity::sslWriteLoop(
  int numTries,
  const std::string& input,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::sslWriteLoop");
  Crypto::convertStringToListBytesSigned(input, this->writeBuffer);
  int i = 0;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->writeOnce(
      - 1,
      this->writeBuffer,
      outputError,
      commentsGeneral,
      commentsGeneral,
      includeNoErrorInComments
    );
    if (numBytes == this->writeBuffer.size) {
      break;
    }
  }
  if (numBytes != this->writeBuffer.size) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral  << i << " errors writing to socket.\n NumBytes: " << numBytes << ". ";
    }
    return false;
  }
  return true;
}

void TransportLayerSecurityOpenSSL::setSocketAddToStack(int socketFileDescriptor) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::setSocketAddToStack");
  this->socketStack.addOnTop(socketFileDescriptor);
  this->doSetSocket(socketFileDescriptor);
}

void TransportLayerSecurity::free() {
  this->openSSLData.freeSession();
  this->flagInitialized = false;
}

void TransportLayerSecurity::removeLastSocket() {
  this->openSSLData.removeLastSocket();
}

void TransportLayerSecurityOpenSSL::removeLastSocket() {
  MacroRegisterFunctionWithName("TransportLayerSecurity::removeLastSocket");
  if (this->socketStack.size > 0) {
    int lastSocket = this->socketStack.popLastObject();
    close(lastSocket);
  }
  if (this->socketStack.size <= 0) {
    return;
  }
  this->doSetSocket(*this->socketStack.lastObject());
}

CipherSuiteSpecification TransportLayerSecurityServer::getCipherNoFailure(int inputId) {
  if (inputId == 0) {
    global.fatal << "zero cipher suite specification not allowed here. " << global.fatal;
  }
  if (!this->cipherSuiteNames.contains(inputId)) {
    global.fatal << "Unknown cipher suite specification not allowed here. " << global.fatal;
  }
  CipherSuiteSpecification result;
  result.id = inputId;
  result.owner = this;
  result.computeName();
  return result;
}

void TransportLayerSecurityServer::addSupportedCipher(int inputId) {
  this->supportedCiphers.setKeyValue(inputId, this->getCipherNoFailure(inputId));
}

void TransportLayerSecurityServer::initializeExtensions() {
  // Extension specification should be available here:
  // https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml
  this->extensionNames.setKeyValue(
    SSLContent::tokensExtension::signatureAlgorithms,
    "signature algorithms"
  );
  this->extensionNames.setKeyValue(
    SSLContent::tokensExtension::renegotiateConnection, "renegotiate"
  );
  this->extensionNames.setKeyValue(
    SSLContent::tokensExtension::ellipticCurvePointFormat, "elliptic curve point format"
  );
  this->extensionNames.setKeyValue(
    SSLContent::tokensExtension::requestOnlineCertificateStatus, "request online certificate status"
  );
  this->extensionNames.setKeyValue(
    SSLContent::tokensExtension::serverName, "server name"
  );
  this->extensionNames.setKeyValue(
    SSLContent::tokensExtension::extendedMasterSecret, "extended master secret"
  );
}

void TransportLayerSecurityServer::initializeCipherSuites() {
  if (this->cipherSuiteNames.size() != 0) {
    return;
  }
  this->cipherSuiteNames.setKeyValue(0xc02f, "ECDHE/RSA/AES 128/GCM/SHA2"          ); // (RFC 5289)
  this->addSupportedCipher(0xc02f);
  this->cipherSuiteNames.setKeyValue(0x1301, "TLS_AES_128_GCM_SHA256"              ); //
  this->cipherSuiteNames.setKeyValue(0x1302, "TLS_AES_256_GCM_SHA384"              ); //
  this->cipherSuiteNames.setKeyValue(0x1303, "TLS_CHACHA20_POLY1305_SHA256"        ); //
  this->cipherSuiteNames.setKeyValue(0xc02b, "ECDHE/ECDSA/AES 128/GCM/SHA2"        ); // (RFC 5289)
  this->cipherSuiteNames.setKeyValue(0xc02c, "ECDHE/ECDSA/AES 128/GCM/SHA3"        ); // (RFC 5289)
  this->cipherSuiteNames.setKeyValue(0xc030, "ECDHE/RSA/AES 256/GCM/SHA3"          ); // (RFC 5289)
  this->cipherSuiteNames.setKeyValue(0xcca9, "ECDHE/ECDSA/CHACHA 20/POLY 1305/SHA2"); // (RFC 7905)
  this->cipherSuiteNames.setKeyValue(0xcca8, "ECDHE/RSA/CHACHA 20/POLY 1305/SHA2"  ); // (RFC 7905)
  this->cipherSuiteNames.setKeyValue(0xc013, "ECDHE/RSA/AES 128/CBC/SHA"           ); // (RFC 4492)
  this->cipherSuiteNames.setKeyValue(0xc014, "ECDHE/RSA/AES 256/CBC/SHA"           ); // (RFC 4492)
  this->cipherSuiteNames.setKeyValue(0x009c, "RSA/RSA/AES 128/GCM/SHA2"            ); // (RFC 5288)
  this->cipherSuiteNames.setKeyValue(0x009d, "RSA/RSA/AES 256/GCM/SHA2"            ); // (RFC 5288)
  this->cipherSuiteNames.setKeyValue(0x002f, "RSA/RSA/AES 128/CBC/SHA"             ); // (RFC 5246)
  this->cipherSuiteNames.setKeyValue(0x0035, "RSA/RSA/AES 256/CBC/SHA"             ); // (RFC 5246)
  this->cipherSuiteNames.setKeyValue(0x000a, "RSA/RSA/3DES/CBC/SHA"                ); // (RFC 5246)
  this->cipherSuiteNames.setKeyValue(0,      "unknown"                             );
}

bool TransportLayerSecurityServer::initializeAll(
  const std::string& privateKeyPEMEncoded,
  const std::string& serverCertificatePEMEncoded,
  std::stringstream* commentsOnError
) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::initializeAll");
  this->initialize();
  if (!this->privateKey.loadFromPEM(privateKeyPEMEncoded, commentsOnError)) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to load private key. ";
    }
    return false;
  }
  this->privateKey.computeFromTwoPrimes(
    this->privateKey.primeOne, this->privateKey.primeTwo, false, commentsOnError
  );
  if (!this->certificate.loadFromPEM(serverCertificatePEMEncoded, commentsOnError)) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to load server certificate. ";
    }
    return false;
  }
  return true;
}

void TransportLayerSecurityServer::initialize() {
  this->initializeCipherSuites();
  this->initializeExtensions();
}

TransportLayerSecurityServer::NetworkSpoofer::NetworkSpoofer() {
  this->flagDoSpoof = false;
  this->currentInputMessageIndex = - 1;
  this->owner = nullptr;
}

TransportLayerSecurityServer::TransportLayerSecurityServer() {
  this->session.owner = this;
  this->millisecondsDefaultTimeOut = 5000;
  this->millisecondsTimeOut = this->millisecondsDefaultTimeOut;
  this->defaultBufferCapacity = 1000000;
  this->lastReaD.owner = this;
  this->serverHelloCertificate.owner = this;
  this->serverHelloKeyExchange.owner = this;
  this->serverHelloStart.owner = this;
  this->spoofer.owner = this;
}

bool TransportLayerSecurityServer::NetworkSpoofer::writeSSLRecords(List<SSLRecord>& input) {
  this->outgoingMessages.addOnTop(input);
  return true;
}

bool TransportLayerSecurityServer::writeSSLRecords(
  List<SSLRecord>& input, std::stringstream* commentsOnFailure
) {
  if (this->spoofer.flagDoSpoof) {
    return this->spoofer.writeSSLRecords(input);
  }
  this->outgoingBytes.setSize(0);
  for (int i = 0; i < input.size; i ++) {
    input[i].writeBytes(this->outgoingBytes, nullptr);
  }
  return this->writeBytesOnce(this->outgoingBytes, commentsOnFailure);
}

bool TransportLayerSecurityServer::writeBytesOnce(
  List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::writeBytesOnce");
  struct timeval tv;
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not initialize'ing this can cause strange errors
  setsockopt(this->session.socketId, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&tv), sizeof(timeval));
  ssize_t numBytesSent = send(
    this->session.socketId,
    input.objects,
    static_cast<size_t>(input.size),
    0
  );
  if (numBytesSent < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error receiving bytes. " << strerror(errno);
    }
  }
  return numBytesSent >= 0;
}

std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::inputMessages = "inputMessages";
std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::outputMessages = "outputMessages";
std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnInput = "errorsOnInput";
std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnOutput = "errorsOnOutput";

std::string TransportLayerSecurityServer::JSLabels::spoofer = "spoofer";
std::string TransportLayerSecurityServer::JSLabels::session = "session";

JSData TransportLayerSecurityServer::toJSON() {
  JSData result;
  if (this->spoofer.flagDoSpoof) {
    result[TransportLayerSecurityServer::JSLabels::spoofer] = this->spoofer.toJSON();
  }
  result[TransportLayerSecurityServer::JSLabels::session] = this->session.toJSON();
  return result;
}

JSData TransportLayerSecurityServer::NetworkSpoofer::toJSON() {
  JSData result, inputMessage, outputMessages, inErrors, outErrors;
  result.elementType = JSData::token::tokenArray;
  inputMessage.elementType = JSData::token::tokenArray;
  outputMessages.elementType = JSData::token::tokenArray;
  inErrors.elementType = JSData::token::tokenArray;
  outErrors.elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->incomingMessages.size; i ++) {
    inputMessage[i] = this->incomingMessages[i].toJSON();
  }
  inErrors = this->errorsOnIncoming;
  outErrors = this->errorsOnOutgoing;
  for (int i = 0; i < this->outgoingMessages.size; i ++) {
    JSData currentOutputMessages;
    currentOutputMessages.elementType = JSData::token::tokenArray;
    for (int j = 0; j < this->outgoingMessages[i].size; j ++) {
      currentOutputMessages[j] = this->outgoingMessages[i][j].toJSON();
    }
    outputMessages[i] = currentOutputMessages;
  }
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnInput] = inErrors;
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnOutput] = outErrors;
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::inputMessages] = inputMessage;
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::outputMessages] = outputMessages;
  return result;
}

bool TransportLayerSecurityServer::NetworkSpoofer::readBytesOnce(
  std::stringstream* commentsOnError
) {
  global << Logger::red
  << "Transport layer security server is spoofing the network. " << Logger::endL;
  if (
    this->currentInputMessageIndex < 0 ||
    this->currentInputMessageIndex >= this->incomingMessages.size
  ) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "No available spoofed messages. ";
    }
    return false;
  }
  this->owner->incomingBytes = this->incomingMessages[
    this->currentInputMessageIndex
  ].incomingBytes;
  this->currentInputMessageIndex ++;
  return true;
}

bool TransportLayerSecurityServer::readBytesOnce(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::readBytesOnce");
  if (this->spoofer.flagDoSpoof) {
    return this->spoofer.readBytesOnce(commentsOnError);
  }
  struct timeval tv;
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not initialize'ing this can cause strange errors
  setsockopt(this->session.socketId, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&tv), sizeof(timeval));
  this->incomingBytes.setSize(this->defaultBufferCapacity);
  int numBytesInBuffer = static_cast<int>(recv(
    this->session.socketId,
    this->incomingBytes.objects,
    static_cast<unsigned>(this->incomingBytes.size - 1),
    0
  ));
  if (numBytesInBuffer >= 0) {
    this->incomingBytes.setSize(numBytesInBuffer);
  }
  global << "Read bytes:\n"
  << Crypto::convertListUnsignedCharsToHexFormat(this->incomingBytes, 40, false) << Logger::endL;
  return numBytesInBuffer > 0;
}

TransportLayerSecurityServer& SSLContent::GetServer() const {
  return *this->owner->owner;
}

void SSLContent::resetExceptOwner() {
  this->contentType           = 0;
  this->length            = 0;
  this->version           = 0;
  this->cipherSpecLength  = 0;
  this->challengeLength   = 0;
  this->compressionMethod = 0;
  this->flagRenegotiate            = false;
  this->flagIncomingRandomIncluded = false;
  this->flagOutgoingRandomIncluded = false;
  this->extensions             .setSize(0);
  this->sessionId              .setSize(0);
  this->challenge              .setSize(0);
}

SSLContent::SSLContent() {
  this->owner = nullptr;
  this->resetExceptOwner();
}

Logger::StringHighligher SSLContent::getStringHighlighter() {
  Logger::StringHighligher result("2,4,4,2,6,4,64,2,64,4,||");
  for (int i = 0; i < this->owner->owner->session.incomingCiphers.size; i ++) {
    result.sections.addOnTop(4);
  }
  result.sections.addOnTop(std::string("||"));
  result.sections.addOnTop(4);
  result.sections.addOnTop(4);
  result.sections.addOnTop(std::string("|"));
  for (int i = 0; i < this->extensions.size; i ++) {
    result.sections.addOnTop(4);
    result.sections.addOnTop(4);
    if (this->extensions[i].content.size > 0) {
      result.sections.addOnTop(this->extensions[i].content.size * 2);
    }
  }
  return result;
}

std::string SSLContent::ToStringVersion() const {
  List<char> twoBytes;
  twoBytes.setSize(2);
  twoBytes[0] = static_cast<char> (this->version / 256);
  twoBytes[1] = static_cast<char> (this->version % 256);
  std::stringstream out;
  out << Crypto::convertListCharsToHex(twoBytes, 0, false);
  return out.str();
}


std::string SSLContent::JSLabels::incomingRandom                    = "incomingRandom"                   ;
std::string SSLContent::JSLabels::outgoingRandom                    = "outgoingRandom"                   ;
std::string SSLContent::JSLabels::version                           = "version"                          ;
std::string SSLContent::JSLabels::length                            = "length"                           ;
std::string SSLContent::JSLabels::cipherSpecLength                  = "cipherSpecLength"                 ;
std::string SSLContent::JSLabels::sessionId                         = "sessionId"                        ;
std::string SSLContent::JSLabels::extensionsLength                  = "extensionsLength"                 ;
std::string SSLContent::JSLabels::compressionMethod                 = "compressionMethod"                ;
std::string SSLContent::JSLabels::extensions                        = "extensions"                       ;
std::string SSLContent::JSLabels::renegotiate                       = "renegotiate"                      ;
std::string SSLContent::JSLabels::OCSPrequest                       = "OCSPrequest"                      ;
std::string SSLContent::JSLabels::signedCertificateTimestampRequest = "signedCertificateTimestampRequest";

JSData SSLContent::toJSON() const {
  MacroRegisterFunctionWithName("SSLHello::toJSON");
  JSData result;
  result[SSLContent::JSLabels::version] = this->ToStringVersion();
  result[SSLContent::JSLabels::length] = this->length;
  result[SSLContent::JSLabels::cipherSpecLength] = this->cipherSpecLength;
  result[SSLContent::JSLabels::sessionId] = Crypto::convertListUnsignedCharsToHex(this->sessionId);
  result[SSLContent::JSLabels::extensionsLength ] = this->extensionsLength;
  result[SSLContent::JSLabels::compressionMethod] = Crypto::convertIntToHex(this->compressionMethod, 2);
  JSData extensionsObject;
  extensionsObject.elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->extensions.size; i ++) {
    extensionsObject.listObjects.addOnTop(this->extensions[i].toJSON());
  }
  result[SSLContent::JSLabels::extensions] = extensionsObject;
  result[SSLContent::JSLabels::renegotiate                      ] = this->flagRenegotiate;
  return result;
}

unsigned int CipherSuiteSpecification::hashFunction(const CipherSuiteSpecification &input) {
  return input.hashFunction();
}

unsigned int CipherSuiteSpecification::hashFunction() const {
  return static_cast<unsigned>(this->id);
}

CipherSuiteSpecification::CipherSuiteSpecification() {
  this->owner = nullptr;
}

bool CipherSuiteSpecification::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "CipherSuiteSpecification not initialized correctly. " << global.fatal;
  }
  return true;
}

bool CipherSuiteSpecification::computeName() {
  MacroRegisterFunctionWithName("CipherSuiteSpecification::computeName");
  this->checkInitialization();
  if (!this->owner->cipherSuiteNames.contains(this->id)) {
    // GREASE = deliberately invalid cipher suite code.
    // [Generate Random Extensions and Sustain Extensibility]
    this->name = "unknown/GREASE";
    return false;
  }
  this->name = this->owner->cipherSuiteNames.getValueNoFail(this->id);
  return true;
}

void SSLRecord::writeBytes(List<unsigned char>& output, List<Serialization::Marker>* annotations) const {
  MacroRegisterFunctionWithName("SSLRecord::writeBytes");
  Serialization::WriterOneByteInteger recordTypeWriter(
    SSLRecord::tokens::handshake, output, annotations, "SSL handshake tag"
  );
  Serialization::WriterTwoByteInteger writerVersion(this->version, output, annotations, "SSL record version");
  Serialization::LengthWriterTwoBytes writerLength(output, annotations, "SSL handshake body");
  this->content.writeBytes(output, annotations);
}

void SSLContent::writeBytes(List<unsigned char>& output, List<Serialization::Marker>* annotations) const {
  switch (this->contentType) {
    case SSLContent::tokens::clientHello:
      return SSLContent::writeBytesHandshakeClientHello(output, annotations);
    case SSLContent::tokens::serverHello:
      return SSLContent::writeBytesHandshakeServerHello(output, annotations);
    case SSLContent::tokens::certificate:
      return SSLContent::writeBytesHandshakeCertificate(output, annotations);
    case SSLContent::tokens::serverKeyExchange:
      return SSLContent::writeBytesHandshakeSecretExchange(output, annotations);
  }
}

void SSLContent::writeBytesHandshakeServerHello(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesHandshakeServerHello");
  if (this->contentType != SSLContent::tokens::serverHello) {
    global.fatal << "Not allowed to serialize non server-hello content as server hello. " << global.fatal;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "SSL content body");
  this->WriteVersion(output, annotations);
  this->writeBytesMyRandomAndSessionId(output, annotations);
  // 256 = 0x0100 = no compression:
  Serialization::WriterTwoByteInteger compressionWriter(256, output, annotations, "compression: none");
  this->writeBytesExtensionsOnly(output, annotations);
}

std::string SSLContent::getType(unsigned char token) {
  switch (token) {
  case SSLContent::tokens::certificate:
    return "certificate";
  case SSLContent::tokens::serverKeyExchange:
    return "server key exchange";
  default:
    return "unknown";
  }
}

void TransportLayerSecurityServer::Session::writeNamedCurveAndPublicKey(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLContent::writeNamedCurveAndPublicKey");
  Serialization::WriterOneByteInteger curveName(
    SSLContent::namedCurve, output, annotations, "named curve"
  );
  Serialization::writeTwoByteUnsignedAnnotated(
    static_cast<unsigned int>(this->chosenEllipticCurve),
    output,
    annotations,
    this->chosenEllipticCurveName
  );
  std::stringstream shouldNotBeNeeded;
  List<unsigned char> publicKeyBytes;
  bool mustBeTrue =
  this->ephemerealPublicKey.xCoordinate.value.writeBigEndianFixedNumberOfBytes(
    publicKeyBytes, 32, &shouldNotBeNeeded
  );
  Serialization::writeOneByteLengthFollowedByBytes(publicKeyBytes, output, annotations, "public key");
  if (!mustBeTrue) {
    global.fatal << "Internally generated public key does not serialize properly. "
    << shouldNotBeNeeded.str()
    << global.fatal;
  }
  Serialization::WriterOneByteInteger(
    SignatureAlgorithmSpecification::HashAlgorithm::sha256,
    output,
    annotations,
    "hash label"
  );
  Serialization::WriterOneByteInteger(
    SignatureAlgorithmSpecification::SignatureAlgorithm::RSA,
    output,
    annotations,
    "RSA"
  );
  output.addListOnTop(this->signature);
}

void SSLContent::WriteType(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  if (annotations != nullptr) {
    annotations->addOnTop(Serialization::Marker(output.size, 1, this->getType(this->contentType)));
  }
  output.addOnTop(this->contentType);
}

void SSLContent::WriteVersion(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  Serialization::WriterTwoByteInteger(this->version, output, annotations, "SSL content version");
}

void SSLContent::writeBytesHandshakeCertificate(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesHandshakeCertificate");
  if (this->contentType != SSLContent::tokens::certificate) {
    global.fatal << "Not allowed to serialize non-certificate content as certificate. " << global.fatal;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "certificateCollection");
  Serialization::LengthWriterThreeBytes writeLengthFirstCertificate(output, annotations, "certificateBody");
  this->GetServer().certificate.writeBytesASN1(output, annotations);
}

void SSLContent::writeBytesHandshakeSecretExchange(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLContent::writeBytesHandshakeSecretExchange");
  this->checkInitialization();
  if (this->contentType != SSLContent::tokens::serverKeyExchange) {
    global.fatal << "Not allowed to serialize non-server key exchange as such. " << global.fatal;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "server key exchange");
  this->owner->owner->session.writeNamedCurveAndPublicKey(output, annotations);
}

void SSLContent::writeBytesHandshakeClientHello(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesHandshakeClientHello");
  if (this->contentType != SSLContent::tokens::clientHello) {
    global.fatal << "Not allowed to serialize non client-hello content as client hello. " << global.fatal;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "Handshake clientHello body");
  this->WriteVersion(output, annotations);
  this->writeBytesIncomingRandomAndSessionId(output, annotations);
  this->writeBytesSupportedCiphers(output, annotations);
  // 256 = 0x0100 = no compression:
  Serialization::WriterTwoByteInteger noCompression(256, output, annotations, "compression: none");
  this->writeBytesExtensionsOnly(output, annotations);
}

List<unsigned char>& SSLContent::getMyRandomBytes() const {
  if (
    this->owner->owner->session.myRandomBytes.size !=
    this->LengthRandomBytesInSSLHello
  ) {
    global.fatal << "Writing non-initialized SSL hello forbidden. " << global.fatal;
  }
  return this->owner->owner->session.myRandomBytes;
}

void SSLContent::writeBytesIncomingRandomAndSessionId(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesIncomingRandomAndSessionId");
  Serialization::writeBytesAnnotated(
    this->owner->owner->session.incomingRandomBytes, output, annotations, "client random"
  );
  Serialization::writeOneByteLengthFollowedByBytes(this->sessionId, output, annotations, "session id");
}

void SSLContent::writeBytesMyRandomAndSessionId(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesMyRandomAndSessionId");
  Serialization::writeBytesAnnotated(this->getMyRandomBytes(), output, annotations, "server random");
  Serialization::writeOneByteLengthFollowedByBytes(this->sessionId, output, annotations, "session id");
}

void SSLContent::writeBytesSupportedCiphers(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesSupportedCiphers");
  List<CipherSuiteSpecification>& ciphers = this->owner->owner->session.incomingCiphers;
  Serialization::WriterTwoByteInteger cipherNumberWriter(
    ciphers.size * 2,
    output,
    annotations,
    "number of ciphers"
  );
  for (int i = 0; i < ciphers.size; i ++) {
    Serialization::WriterTwoByteInteger currentCipher(
      ciphers[i].id, output, annotations, "cipher id"
    );
  }
}

bool SSLContent::decodeSupportedCiphers(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::decodeSupportedCiphers");
  if (!Serialization::readTwoByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->cipherSpecLength,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read cipher suite length. ";
    }
    return false;
  }
  int halfCipherLength = this->cipherSpecLength / 2;
  this->cipherSpecLength = halfCipherLength * 2; // make sure cipher spec length is even.
  if (this->cipherSpecLength + this->owner->offsetDecoded >= this->owner->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "cipher suite length: " << this->cipherSpecLength << " exceeds message length. ";
    }
    return false;
  }
  List<CipherSuiteSpecification>& ciphers = this->owner->owner->session.incomingCiphers;
  if (ciphers.size > 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Ciphers already declared. ";
    }
    return false;
  }
  ciphers.setSize(halfCipherLength);
  for (int i = 0; i < halfCipherLength; i ++) {
    ciphers[i].owner = this->owner->owner;
    if (!Serialization::readTwoByteInt(
      this->owner->incomingBytes,
      this->owner->offsetDecoded,
      ciphers[i].id,
      commentsOnFailure
    )) {
      // this should not happen.
      return false;
    }
    ciphers[i].computeName();
  }
  if (!this->owner->owner->session.chooseCipher(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to choose a cipher. ";
    }
    return false;
  }
  return true;
}

void SSLContent::writeBytesExtensionsOnly(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::writeBytesExtensionsOnly");
  Serialization::LengthWriterTwoBytes extensionsLength(output, annotations, "extensions");
  for (int i = 0; i < this->extensions.size; i ++) {
    this->extensions[i].writeBytes(output, annotations);
  }
}

bool SSLContent::decode(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::decode");
  if (this->owner->incomingBytes.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty message. ";
    }
    return false;
  }
  this->contentType = this->owner->incomingBytes[this->owner->offsetDecoded];
  this->owner->offsetDecoded ++;
  if (
    this->contentType != SSLContent::tokens::clientHello &&
    this->contentType != SSLContent::tokens::serverHello
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Message does not appear to be a client/server hello. ";
    }
    return false;
  }
  if (!Serialization::readThreeByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->length,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read ssl record length. ";
    }
    return false;
  }
  if (this->length + this->owner->offsetDecoded > this->owner->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Client hello length is too big. ";
    }
    return false;
  }
  if (!Serialization::readTwoByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->version,
    commentsOnFailure
  )) {
    return false;
  }
  List<unsigned char> incomingRandomBytes;
  if (!Serialization::readBytesFixedLength(
    this->owner->incomingBytes,
    this->LengthRandomBytesInSSLHello,
    this->owner->offsetDecoded,
    incomingRandomBytes,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to fetch session random bytes. ";
    }
    return false;
  }
  if (!this->owner->owner->session.setIncomingRandomBytes(
    incomingRandomBytes, commentsOnFailure
  )) {
    return false;
  }
  this->flagIncomingRandomIncluded = true;

  if (!Serialization::readOneByteLengthFollowedByBytes(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    nullptr,
    &this->sessionId,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read session id. ";
    }
    return false;
  }
  if (!this->decodeSupportedCiphers(commentsOnFailure)){
    return false;
  }
  if (!Serialization::readTwoByteInt(
    this->owner->incomingBytes, this->owner->offsetDecoded, this->compressionMethod, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Compression bytes not set. ";
    }
    return false;
  }
  // compression is ignored.
  return this->decodeExtensions(commentsOnFailure);
}

bool SSLContent::decodeExtensions(std::stringstream *commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::decodeExtensions");
  if (this->owner->offsetDecoded + 2 >= this->owner->incomingBytes.size) {
    // No extensions
    return true;
  }
  if (!Serialization::readTwoByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->extensionsLength,
    commentsOnFailure
  )) {
    return false;
  }
  int extensionsLimit = this->owner->offsetDecoded + this->extensionsLength;
  if (extensionsLimit > this->owner->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Extension length exceeds message size. ";
    }
    return false;
  }
  this->extensions.setSize(0);

  while (this->owner->offsetDecoded < extensionsLimit) {
    SSLHelloExtension incoming;
    incoming.owner = this;
    if (!Serialization::readTwoByteInt(
      this->owner->incomingBytes,
      this->owner->offsetDecoded,
      incoming.extensionType,
      commentsOnFailure
    )) {
      return false;
    }
    if (!Serialization::readTwoByteLengthFollowedByBytes(
      this->owner->incomingBytes,
      this->owner->offsetDecoded,
      nullptr,
      &incoming.content,
      commentsOnFailure
    )) {
      return false;
    }
    this->extensions.addOnTop(incoming);
  }
  return this->processExtensions(commentsOnFailure);
}

SSLHelloExtension::SSLHelloExtension() {
  this->owner = nullptr;
  this->extensionType = 0;
}

void SSLHelloExtension::writeBytes(
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations
) {
  Serialization::WriterTwoByteInteger writer(this->extensionType, output, annotations, "ssl hello extension type");
  Serialization::writeTwoByteLengthFollowedByBytes(this->content, output, annotations, "ssl hello extension content");
}

void SSLHelloExtension::makeGrease(SSLContent* inputOwner) {
  this->owner = inputOwner;
  this->content.setSize(0);
  this->extensionType = (13 * 16 + 10) * 256 + (13 * 16 + 10); // 0xdada;
}

void SSLHelloExtension::makeExtendedMasterSecret(SSLContent* inputOwner) {
  this->owner = inputOwner;
  this->content.setSize(0);
  this->extensionType = SSLContent::tokensExtension::extendedMasterSecret;
}

void SSLHelloExtension::makeEllipticCurvePointFormat(SSLContent* inputOwner) {
  this->owner = inputOwner;
  this->extensionType = SSLContent::tokensExtension::ellipticCurvePointFormat;
  this->content.setSize(0);
  this->content.addOnTop(0);
  this->content.addOnTop(4);
  this->content.addOnTop(3);
  this->content.addOnTop(0);
  this->content.addOnTop(1);
  this->content.addOnTop(2);
}

bool SSLHelloExtension::checkInitialization() {
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized owner of ssl hello extension. " << global.fatal;
  }
  return true;
}

JSData SSLHelloExtension::toJSON() {
  JSData result;
  result.elementType = JSData::token::tokenObject;
  result["name"] = this->name();
  std::stringstream hex;
  hex << std::hex << std::setfill('0') << std::setw(4) << this->extensionType;
  result["type"] = hex.str();
  result["data"] = Crypto::convertListUnsignedCharsToHex(this->content);
  return result;
}

std::string SSLHelloExtension::name() {
  this->checkInitialization();
  if (this->owner->owner->owner->extensionNames.contains(this->extensionType)) {
    return this->owner->owner->owner->extensionNames.getValueNoFail(this->extensionType);
  }
  return "unknown";
}

bool SSLHelloExtension::processRenegotiateConnection(std::stringstream *commentsOnError) {
  if (this->content.size == 1) {
    if (this->content[0] == 0) {
      this->owner->flagRenegotiate = true;
      return true;
    }
  }
  if (commentsOnError != nullptr) {
    *commentsOnError << "Unexpected renegotiate connection extension content. ";
  }
  return false;
}

bool SSLHelloExtension::processServerName(std::stringstream *commentsOnError) {
  TransportLayerSecurityServer::Session& session = this->owner->owner->owner->session;
  if (session.serverName.size > 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Server name extension can be set only once. ";
    }
    return false;
  }
  if (!Serialization::readTwoByteLengthFollowedByBytesDontOutputOffset(
    this->content, 0, nullptr, &session.serverName, commentsOnError
  )) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to read server name extension. ";
    }
    return false;
  }
  return true;
}

bool SSLHelloExtension::processRequestOnlineCertificateStatus(std::stringstream* commentsOnError) {
  (void) commentsOnError;
  this->owner->owner->owner->session.flagRequestOnlineCertificateStatusProtocol = true;
  return true;
}

bool SSLHelloExtension::processRequestSignedCertificateTimestamp(std::stringstream* commentsOnError) {
  (void) commentsOnError;
  this->owner->owner->owner->session.flagRequestSignedCertificateTimestamp = true;
  return true;
}

SignatureAlgorithmSpecification::SignatureAlgorithmSpecification() {
  this->ownerServer = nullptr;
  this->signatureAlgorithm = 0;
  this->hash = 0;
  this->value = 0;
}

void SignatureAlgorithmSpecification::processValue() {
  // Works for all RSA and ECDSA-based algorithms:
  this->hash = this->value / 256;
  this->signatureAlgorithm = this->value % 256;
}

bool SSLHelloExtension::processSignatureAlgorithms(std::stringstream* commentsOnError) {
  // See:
  // 1. [7.4.1.4.1., RFC5246]
  // 2. [page 41, RFC 8446].
  TransportLayerSecurityServer::Session& session = this->owner->owner->owner->session;
  int offset = 0;
  List<unsigned char> specifications;
  if (!Serialization::readTwoByteLengthFollowedByBytes(
    this->content, offset, nullptr, &specifications, commentsOnError
  )) {
    return false;
  }
  int numberOfPairs = specifications.size / 2;
  for (int i = 0; i < numberOfPairs; i ++) {
    SignatureAlgorithmSpecification incoming;
    incoming.ownerServer = this->owner->owner->owner;
    incoming.value = specifications[i * 2] * 256 + specifications[i * 2 + 1];
    incoming.processValue();
    session.incomingAlgorithmSpecifications.addOnTop(incoming);
  }
  if (session.incomingAlgorithmSpecifications.size == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to recognize any hash / algorithm pair supported by the client. ";
    }
    return false;
  }
  return true;
}

bool SSLHelloExtension::processMe(std::stringstream* commentsOnError) {
  switch (this->extensionType) {
  case SSLContent::tokensExtension::renegotiateConnection:
    return this->processRenegotiateConnection(commentsOnError);
  case SSLContent::tokensExtension::serverName:
    return this->processServerName(commentsOnError);
  case SSLContent::tokensExtension::requestOnlineCertificateStatus:
    return this->processRequestOnlineCertificateStatus(commentsOnError);
  case SSLContent::tokensExtension::requestSignedCertificateTimestamp:
    return this->processRequestSignedCertificateTimestamp(commentsOnError);
  case SSLContent::tokensExtension::signatureAlgorithms:
    return this->processSignatureAlgorithms(commentsOnError);
  default:
    break;
  }
  return true;
}

bool SSLContent::processExtensions(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::processExtensions");
  for (int i = 0; i < this->extensions.size; i ++) {
    if (!this->extensions[i].processMe(commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool Serialization::readTwoByteInt(
  const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
) {
  return Serialization::readNByteInt(2, input, inputOutputOffset, result, commentsOnFailure);
}

bool Serialization::readThreeByteInt(
  const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
) {
  return Serialization::readNByteInt(3, input, inputOutputOffset, result, commentsOnFailure);
}

bool Serialization::readFourByteInt(
  const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
) {
  return Serialization::readNByteInt(4, input, inputOutputOffset, result, commentsOnFailure);
}

bool Serialization::readNByteInt(
  int numBytes,
  const List<unsigned char>& input,
  int& inputOutputOffset,
  int& result,
  std::stringstream* commentsOnFailure
) {
  if (numBytes > 4) {
    global.fatal << "Not allowed to read more than 4 bytes into an integer. " << global.fatal;
  }
  if (numBytes + inputOutputOffset > input.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "At least " << numBytes
      << " bytes required to read integer. Input has: " << input.size
      << " bytes and offset is: " << inputOutputOffset << ". ";
    }
    return false;
  }
  result = 0;
  for (int i = 0; i < numBytes; i ++) {
    result *= 256;
    result += static_cast<unsigned>(input[inputOutputOffset + i]);
  }
  inputOutputOffset += numBytes;
  return true;
}

bool Serialization::readBytesFixedLength(
  const List<unsigned char>& input,
  int desiredNumberOfBytes,
  int& inputOutputOffset,
  List<unsigned char>& output,
  std::stringstream* commentsOnError
) {
  if (desiredNumberOfBytes < 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Cannot read " << desiredNumberOfBytes << " bytes. ";
    }
    return false;
  }
  if (inputOutputOffset + desiredNumberOfBytes >= input.size) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to read " << desiredNumberOfBytes
      << " bytes at offset: " << inputOutputOffset
      << ": this would exceed the input buffer size: " << input.size << ". ";
    }
    return false;
  }
  output.setSize(desiredNumberOfBytes);
  for (int i = 0; i < desiredNumberOfBytes; i ++) {
    output[i] = input[inputOutputOffset + i];
  }
  inputOutputOffset += desiredNumberOfBytes;
  return true;
}

bool Serialization::readOneByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  int& outputOffset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  return Serialization::readNByteLengthFollowedByBytes(
    1, input, outputOffset, resultLength, output, commentsOnError
  );
}

bool Serialization::readTwoByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  int& outputOffset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  return Serialization::readNByteLengthFollowedByBytes(
    2, input, outputOffset, resultLength, output, commentsOnError
  );
}

bool Serialization::readTwoByteLengthFollowedByBytesDontOutputOffset(
  const List<unsigned char>& input,
  int offset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  int outputOffset = offset;
  return Serialization::readNByteLengthFollowedByBytes(
    2, input, outputOffset, resultLength, output, commentsOnError
  );
}

bool Serialization::readNByteLengthFollowedByBytes(
  int numBytesLength,
  const List<unsigned char>& input,
  int& outputOffset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  int temp = 0;
  if (resultLength == nullptr) {
    resultLength = &temp;
  }
  if (!Serialization::readNByteInt(
    numBytesLength, input, outputOffset, *resultLength, commentsOnError
  )) {
    return false;
  }
  if (*resultLength + outputOffset > input.size) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Indicated byte sequence length exceeds input size. ";
    }
    return false;
  }
  if (output != nullptr) {
    output->setSize(*resultLength);
    for (int i = 0; i < *resultLength; i ++) {
      (*output)[i] = input[outputOffset + i];
    }
  }
  outputOffset += *resultLength;
  return true;
}

void Serialization::writeTwoByteInt(
  int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::writeNByteUnsigned(2, static_cast<unsigned>(input), output, inputOutputOffset);
}

void Serialization::writeThreeByteInt(
  int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::writeNByteUnsigned(3, static_cast<unsigned>(input), output, inputOutputOffset);
}

void Serialization::writeTwoByteUnsignedAnnotated(
  unsigned int input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  if (annotations != nullptr) {
    annotations->addOnTop(Serialization::Marker(output.size, 2, label));
  }
  Serialization::writeTwoByteUnsigned(input, output);
}

void Serialization::writeTwoByteUnsigned(
  unsigned int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::writeNByteUnsigned(2, input, output, inputOutputOffset);
}

void Serialization::writeThreeByteUnsigned(
  unsigned int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::writeNByteUnsigned(3, input, output, inputOutputOffset);
}

void Serialization::writeFourByteUnsigned(
  unsigned int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::writeNByteUnsigned(4, input, output, inputOutputOffset);
}

void Serialization::writeNByteUnsigned(
  int byteCountOfLength,
  unsigned int input,
  List<unsigned char>& output,
  int& inputOutputOffset
) {
  if (byteCountOfLength < 0 || byteCountOfLength > 4) {
    global.fatal << "Invalid byteCountOfLength: " << byteCountOfLength << global.fatal;
  }
  if (inputOutputOffset + byteCountOfLength > output.size) {
    output.setSize(inputOutputOffset + byteCountOfLength);
  }
  for (int i = byteCountOfLength - 1 + inputOutputOffset; i >= inputOutputOffset; i --) {
    output[i] = static_cast<unsigned char>(input % 256);
    input /= 256;
  }
  if (input > 0) {
    global.fatal << "Input has more significant digits than the number of bytes allow. " << global.fatal;
  }
  inputOutputOffset += byteCountOfLength;
}

void Serialization::writeOneByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  Serialization::writeNByteLengthFollowedByBytes(
    1, input, output, annotations, label
  );
}

void Serialization::writeTwoByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  Serialization::writeNByteLengthFollowedByBytes(
    2, input, output, annotations, label
  );
}

std::string Serialization::convertListUnsignedCharsToHex(const List<unsigned char>& input) {
  return Crypto::convertListUnsignedCharsToHexFormat(input, 0, false);
}

std::string Serialization::convertListUnsignedCharsToString(const List<unsigned char>& input) {
  std::string result;
  result.resize(static_cast<unsigned>(input.size));
  for (unsigned i = 0; i < result.size(); i ++) {
    input[i] = static_cast<unsigned char>(input[i]);
  }
  return result;
}

void Serialization::writeBytesAnnotated(
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  if (annotations != nullptr) {
    annotations->addOnTop(Serialization::Marker(output.size, input.size, label));
  }
  output.addListOnTop(input);
}

void Serialization::writeNByteLengthFollowedByBytes(
  int byteCountOfLength,
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  Serialization::WriterIntegerWithMarker writer(
    byteCountOfLength, static_cast<unsigned>(input.size), output, annotations, label
  );
  output.addListOnTop(input);
}

bool SSLRecord::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Uninitialized ssl record. " << global.fatal;
  }
  return true;
}

SSLRecord::SSLRecord() {
  this->recordType = SSLRecord::tokens::unknown;
  this->length = 0;
  this->version = 0;
  this->offsetDecoded = 0;
  this->content.owner = this;
  this->owner = nullptr;
}

std::string Serialization::JSLabels::markers = "markers";
std::string Serialization::JSLabels::body = "body";
std::string Serialization::JSLabels::length = "length";
std::string Serialization::JSLabels::offset = "offset";
std::string Serialization::JSLabels::label = "label";
std::string Serialization::JSLabels::serialization = "serialization";

std::string Serialization::Marker::toString() const {
  std::stringstream out;
  out << "[" << this->offset << ", " << this->length << ", '" << this->label << "']";
  return out.str();
}

JSData Serialization::Marker::toJSON() {
  JSData result;
  result.elementType = JSData::token::tokenObject;
  result[Serialization::JSLabels::offset] = LargeInteger(this->offset);
  result[Serialization::JSLabels::length] = LargeInteger(this->length);
  result[Serialization::JSLabels::label] = this->label;
  return result;
}

std::string SSLRecord::toHtml(int id) {
  std::stringstream out, spanId;
  spanId << "spanSSLRecord_" << "_" << id;
  JSData content;
  out << "<div id = '" << spanId.str() << "'></div>";
  content["id"] = spanId.str();
  content["content"] = this->toJSON();
  out << HtmlRoutines::scriptFromJSON("displaySSLRecord", content);
  return out.str();
}

std::string SSLRecord::JSLabels::type = "type";
std::string SSLRecord::JSLabels::content = "content";
std::string SSLRecord::JSLabels::session = "session";

JSData SSLRecord::toJSON() {
  MacroRegisterFunctionWithName("SSLRecord::toJSON");
  JSData result;
  result[Serialization::JSLabels::serialization] = this->toJSONSerialization();
  result[SSLRecord::JSLabels::type] = this->toStringType();
  result[SSLRecord::JSLabels::content] = this->content.toJSON();
  result[SSLRecord::JSLabels::session] = this->owner->session.toJSON();
  return result;
}

std::string TransportLayerSecurityServer::Session::toStringChosenCipher() {
  if (this->chosenCipher == 0) {
    return "unknown";
  }
  return this->owner->cipherSuiteNames.getValueNoFail(this->chosenCipher);
}

std::string SignatureAlgorithmSpecification::JSLabels::hash = "hash";
std::string SignatureAlgorithmSpecification::JSLabels::hashName = "hashName";
std::string SignatureAlgorithmSpecification::JSLabels::signatureAlgorithm = "signatureAlgorithm";
std::string SignatureAlgorithmSpecification::JSLabels::signatureAlgorithmName = "signatureAlgorithmName";
std::string SignatureAlgorithmSpecification::JSLabels::valueHex = "valueHex";

std::string SignatureAlgorithmSpecification::getSignatureAlgorithmName() {
  switch (this->signatureAlgorithm) {
  case SignatureAlgorithmSpecification::SignatureAlgorithm::anonymous:
    return "anonymous";
  case SignatureAlgorithmSpecification::SignatureAlgorithm::DSA:
    return "DSA";
  case SignatureAlgorithmSpecification::SignatureAlgorithm::ECDSA:
    return "ECDSA";
  case SignatureAlgorithmSpecification::SignatureAlgorithm::RSA:
    return "RSA";
  }
  return "unknown";
}

std::string SignatureAlgorithmSpecification::getHashName() {
  switch (this->hash) {
  case SignatureAlgorithmSpecification::HashAlgorithm::md5:
    return "md5";
  case SignatureAlgorithmSpecification::HashAlgorithm::none:
    return "none";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha1:
    return "sha1";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha224:
    return "sha224";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha256:
    return "sha256";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha384:
    return "sha384";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha512:
    return "sha512";
  }
  return "unknown";
}

std::string SignatureAlgorithmSpecification::toHex() {
  return Crypto::convertUintToHex(this->value, 2);
}

JSData SignatureAlgorithmSpecification::toJSON() {
  JSData result;
  result[SignatureAlgorithmSpecification::JSLabels::valueHex] = this->toHex();
  result[SignatureAlgorithmSpecification::JSLabels::hash] = static_cast<int>(this->hash);
  result[SignatureAlgorithmSpecification::JSLabels::hashName] = this->getHashName();
  result[SignatureAlgorithmSpecification::JSLabels::signatureAlgorithm] = static_cast<int>(this->signatureAlgorithm);
  result[SignatureAlgorithmSpecification::JSLabels::signatureAlgorithmName] = this->getSignatureAlgorithmName();
  return result;
}

std::string TransportLayerSecurityServer::Session::JSLabels::chosenCipher                      = "chosenCipher"                     ;
std::string TransportLayerSecurityServer::Session::JSLabels::chosenCipherName                  = "chosenCipherName"                 ;
std::string TransportLayerSecurityServer::Session::JSLabels::incomingRandomBytes               = "incomingRandomBytes"              ;
std::string TransportLayerSecurityServer::Session::JSLabels::myRandomBytes                     = "myRandomBytes"                    ;
std::string TransportLayerSecurityServer::Session::JSLabels::OCSPrequest                       = "OnlineCertificateStatusProtocol"  ;
std::string TransportLayerSecurityServer::Session::JSLabels::signedCertificateTimestampRequest = "RequestSignedCertificateTimestamp";
std::string TransportLayerSecurityServer::Session::JSLabels::cipherSuites                      = "cipherSuites"                     ;
std::string TransportLayerSecurityServer::Session::JSLabels::serverName                        = "serverName"                       ;
std::string TransportLayerSecurityServer::Session::JSLabels::algorithmSpecifications           = "algorithmSpecifications"          ;
std::string TransportLayerSecurityServer::Session::JSLabels::ellipticCurveId                   = "ellipticCurveId"                  ;
std::string TransportLayerSecurityServer::Session::JSLabels::ellipticCurveName                 = "ellipticCurveName"                ;
std::string TransportLayerSecurityServer::Session::JSLabels::bytesToSign                       = "bytesToSign"                      ;

JSData TransportLayerSecurityServer::Session::toJSON() {
  JSData result;
  result.elementType = JSData::token::tokenArray;
  result[TransportLayerSecurityServer::Session::JSLabels::chosenCipher                     ] = Crypto::convertIntToHex(this->chosenCipher, 2);
  result[TransportLayerSecurityServer::Session::JSLabels::chosenCipherName                 ] = this->toStringChosenCipher();
  result[TransportLayerSecurityServer::Session::JSLabels::incomingRandomBytes              ] = Crypto::convertListUnsignedCharsToHex(this->incomingRandomBytes);
  result[TransportLayerSecurityServer::Session::JSLabels::myRandomBytes                    ] = Crypto::convertListUnsignedCharsToHex(this->myRandomBytes);
  result[TransportLayerSecurityServer::Session::JSLabels::OCSPrequest                      ] = this->flagRequestOnlineCertificateStatusProtocol;
  result[TransportLayerSecurityServer::Session::JSLabels::signedCertificateTimestampRequest] = this->flagRequestSignedCertificateTimestamp;
  result[TransportLayerSecurityServer::Session::JSLabels::ellipticCurveId                  ] = static_cast<int>(this->chosenEllipticCurve);
  result[TransportLayerSecurityServer::Session::JSLabels::ellipticCurveName                ] = this->chosenEllipticCurveName;
  result[TransportLayerSecurityServer::Session::JSLabels::bytesToSign                      ] = Crypto::convertListUnsignedCharsToHex(this->bytesToSign);
  JSData ciphers;
  ciphers.elementType = JSData::token::tokenObject;
  ciphers.listObjects.setSize(this->incomingCiphers.size);
  for (int i = 0; i < this->incomingCiphers.size; i ++) {
    CipherSuiteSpecification& current = this->incomingCiphers[i];
    ciphers[Crypto::convertIntToHex(current.id, 2)] = current.name;
  }
  JSData algorithmSpecifications;
  algorithmSpecifications.elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->incomingAlgorithmSpecifications.size; i ++) {
    algorithmSpecifications.listObjects.addOnTop(this->incomingAlgorithmSpecifications[i].toJSON());
  }
  result[TransportLayerSecurityServer::Session::JSLabels::algorithmSpecifications] = algorithmSpecifications;
  result[TransportLayerSecurityServer::Session::JSLabels::cipherSuites] = ciphers;
  if (this->serverName.size > 0) {
    result[
      TransportLayerSecurityServer::Session::JSLabels::serverName
    ] = Crypto::convertListUnsignedCharsToHex(
      this->serverName
    );
  }
  return result;
}

JSData SSLRecord::toJSONSerialization() {
  MacroRegisterFunctionWithName("SSLRecord::toJSONSerialization");
  JSData result;
  List<unsigned char> serialization;
  List<Serialization::Marker> markers;
  this->writeBytes(serialization, &markers);
  JSData jsMarkers;
  jsMarkers.elementType = JSData::token::tokenArray;
  for (int i = 0; i < markers.size; i ++) {
    jsMarkers.listObjects.addOnTop(markers[i].toJSON());
  }
  result[Serialization::JSLabels::markers] = jsMarkers;
  result[Serialization::JSLabels::body] = Crypto::convertListUnsignedCharsToHex(serialization);
  return result;
}

std::string SSLRecord::toStringType() const {
  switch (this->recordType) {
  case SSLRecord::tokens::alert:
    return "alert";
  case SSLRecord::tokens::applicationData:
    return "applicationData";
  case SSLRecord::tokens::changeCipherSpec:
    return "changeCipherSpec";
  case SSLRecord::tokens::handshake:
    return "handshake";
  case SSLRecord::tokens::unknown:
    return "unknown";
  default:
    break;
  }
  std::stringstream out;
  out << "unknown type: " << static_cast<int>(this->recordType);
  return out.str();
}

std::string SSLRecord::toString() const {
  JSData result;
  result["length"] = this->length;
  result["incomingBytes"] = Crypto::convertListUnsignedCharsToHexFormat(this->incomingBytes, 50, false);
  result["type"] = this->toStringType();
  if (this->recordType == SSLRecord::tokens::handshake) {
    result["hello"] = this->content.toJSON();
  }
  std::string hexVersion;
  Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(
    LargeIntegerUnsigned(static_cast<unsigned>(this->version)), 0, hexVersion
  );
  result["version"] = hexVersion;
  return result.toString();
}

bool SSLRecord::decode(std::stringstream *commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLRecord::decode");
  this->checkInitialization();
  if (this->incomingBytes.size < 5 + this->offsetDecoded) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL record needs to have at least 5 bytes, yours has: "
      << this->incomingBytes.size << ". ";
    }
    return false;
  }
  this->recordType = this->incomingBytes[this->offsetDecoded];
  this->offsetDecoded ++;
  if (
    this->recordType != SSLRecord::tokens::handshake
  //&&
//    this->recordType != SSLRecord::tokens::alert &&
//    this->recordType != SSLRecord::tokens::applicationData &&
//    this->recordType != SSLRecord::tokens::changeCipherSpec
  ) {
    this->recordType = SSLRecord::tokens::unknown;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unknown record type: "
      << static_cast<int>(this->incomingBytes[this->offsetDecoded]) << ". ";
    }
    return false;
  }
  if (!Serialization::readTwoByteInt(
    this->incomingBytes, this->offsetDecoded, this->version, commentsOnFailure
  )) {
    return false;
  }
  if (!Serialization::readTwoByteInt(
    this->incomingBytes, this->offsetDecoded, this->length, commentsOnFailure
  )) {
    return false;
  }
  int highEnd = this->length + this->offsetDecoded;
  if (highEnd > this->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Insufficient record length: expected: 5 + " << this->length
      << ", got: " << this->incomingBytes.size - this->offsetDecoded;
    }
    return false;
  }
  return this->decodeBody(commentsOnFailure);
}

bool SSLRecord::decodeBody(std::stringstream *commentsOnFailure) {
  switch (this->recordType) {
    case SSLRecord::tokens::handshake:
      return this->content.decode(commentsOnFailure);
    default:
      break;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Unrecognized ssl record type: " << static_cast<int>(this->recordType) << ". ";
  }
  return false;
}

bool TransportLayerSecurityServer::decodeSSLRecord(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::decodeSSLRecord");
  if (!this->lastReaD.decode(commentsOnFailure)) {
    return false;
  }
  return true;
}

bool TransportLayerSecurityServer::readBytesDecodeOnce(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::readBytesDecodeOnce");
  if (!this->readBytesOnce(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read bytes. ";
    }
    return false;
  }
  this->lastReaD.incomingBytes = this->incomingBytes;
  if (!this->decodeSSLRecord(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to decode ssl record. ";
    }
    return false;
  }
  return true;
}

bool SSLContent::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Uninitialized ssl content. " << global.fatal;
  }
  this->owner->checkInitialization();
  return true;
}

std::string CipherSuiteSpecification::toString() const {
  std::stringstream out;
  std::string hexVersion;
  Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(
    LargeIntegerUnsigned(static_cast<unsigned>(this->id)), 0, hexVersion
  );
  out << hexVersion << "[" << this->name << "]";
  return out.str();
}

// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art061
void SSLContent::prepareServerHello2Certificate() {
  MacroRegisterFunctionWithName("SSLContent::prepareServerHello2Certificate");
  this->checkInitialization();
  this->version = 3 * 256 + 3;
  this->contentType = SSLContent::tokens::certificate;
}

bool TransportLayerSecurityServer::Session::computeAndSignEphemerealKey(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::Session::computeAndSignEphemerealKey");
  (void) commentsOnError;
  Crypto::Random::getRandomLargeIntegerSecure(this->ephemerealPrivateKey, 32);
  this->chosenEllipticCurve = CipherSuiteSpecification::EllipticCurveSpecification::secp256k1;
  this->chosenEllipticCurveName = "secp256k1";
  this->bytesToSign.setSize(0);
  this->bytesToSign.addListOnTop(this->incomingRandomBytes);
  this->bytesToSign.addListOnTop(this->myRandomBytes);
  //
  this->writeNamedCurveAndPublicKey(this->bytesToSign, nullptr);
  this->owner->privateKey.signBytesPadPKCS1(
    this->bytesToSign,
    SignatureAlgorithmSpecification::HashAlgorithm::sha256,
    this->signature
  );
  return true;
}

// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art060
bool SSLContent::prepareServerHello3ServerKeyExchange(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("SSLContent::PrepareServerHello3SecretNegotiation");
  this->contentType = SSLContent::tokens::serverKeyExchange;
  if (this->owner->owner->session.ephemerealPrivateKey != 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Private key already generated for this session. "
      << "No further modifications allowed in this session. ";
    }
    return false;
  }
  if (!this->owner->owner->session.computeAndSignEphemerealKey(commentsOnError)) {
    return false;
  }
  // this->owner->owner->session.ephemerealPublicKey.
  return true;
}

bool TransportLayerSecurityServer::Session::chooseCipher(std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  MapList<int, CipherSuiteSpecification, HashFunctions::hashFunction>& suites =
  this->owner->supportedCiphers;
  int bestIndex = suites.size();
  for (int i = 0; i < this->incomingCiphers.size; i ++) {
    CipherSuiteSpecification& cipher = this->incomingCiphers[i];
    if (cipher.id == 0) {
      continue;
    }
    int candidateIndex = suites.getIndex(cipher.id);
    if (candidateIndex < 0 || candidateIndex > bestIndex) {
      continue;
    }
    bestIndex = candidateIndex;
  }
  this->chosenCipher = - 1;
  if (bestIndex < suites.size()) {
    this->chosenCipher = suites.keys[bestIndex];
  }
  return true;
}

void SSLContent::prepareServerHello1Start(SSLContent& clientHello) {
  MacroRegisterFunctionWithName("SSLContent::prepareServerHello1Start");
  this->checkInitialization();
  this->version = 3 * 256 + 3;
  this->contentType = SSLContent::tokens::serverHello;
  this->compressionMethod = clientHello.compressionMethod;
  Crypto::computeSha256(
    this->owner->owner->session.myRandomBytes, this->sessionId
  );
  this->extensions.setSize(0);
  SSLHelloExtension newExtension;
  newExtension.makeGrease(this);
  this->extensions.addOnTop(newExtension);
  newExtension.makeExtendedMasterSecret(this);
  this->extensions.addOnTop(newExtension);
  newExtension.makeEllipticCurvePointFormat(this);
  this->extensions.addOnTop(newExtension);
}

void SSLRecord::resetExceptOwner() {
  this->content.resetExceptOwner();
  this->incomingBytes.setSize(0);
}

void SSLRecord::prepareServerHello1Start(
  SSLRecord& clientHello
) {
  this->resetExceptOwner();
  this->recordType = SSLRecord::tokens::handshake;
  this->version = 3 * 256 + 1;
  this->content.prepareServerHello1Start(clientHello.content);
  this->owner->outgoingRecords.addOnTop(*this);
}

void SSLRecord::prepareServerHello2Certificate() {
  this->resetExceptOwner();
  this->recordType = SSLRecord::tokens::handshake;
  this->version = 3 * 256 + 1;
  this->content.prepareServerHello2Certificate();
  this->owner->outgoingRecords.addOnTop(*this);
}

bool SSLRecord::prepareServerHello3SecretExchange(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLRecord::prepareServerHello3SecretExchange");
  if (this->owner->privateKey.publicKey.modulus.isEqualToZero()) {
    if (!this->owner->privateKey.computeFromTwoPrimes(
      this->owner->privateKey.primeOne,
      this->owner->privateKey.primeTwo,
      false,
      commentsOnFailure
    )) {
      global.fatal << "Failed to compute public key. " << global.fatal;
    }
  }
  this->resetExceptOwner();
  this->recordType = SSLRecord::tokens::handshake;
  this->version = 3 * 256 + 1;
  bool success = this->content.prepareServerHello3ServerKeyExchange(commentsOnFailure);
  this->owner->outgoingRecords.addOnTop(*this);
  return success;
}

bool TransportLayerSecurityServer::replyToClientHello(
  int inputSocketID, std::stringstream* commentsOnFailure
) {
  (void) commentsOnFailure;
  (void) inputSocketID;
  this->outgoingRecords.setSize(0);
  this->serverHelloStart.prepareServerHello1Start(this->lastReaD);
  this->serverHelloCertificate.prepareServerHello2Certificate();
  if (!this->serverHelloKeyExchange.prepareServerHello3SecretExchange(
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to prepare server secret exchange message. ";
    }
    return false;
  }
  this->outgoingBytes.setSize(0);
  if (!this->writeSSLRecords(this->outgoingRecords, commentsOnFailure)) {
    global << "Error replying to client hello. ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure  << "Error replying to client hello. ";
    }
    return false;
  }
  global << "Wrote ssl records successfully. " << Logger::endL;
  return true;
}

TransportLayerSecurityServer::Session::Session() {
  this->socketId = - 1;
  this->owner = nullptr;
  this->chosenCipher = 0;
  this->chosenEllipticCurve = 0;
  this->flagRequestOnlineCertificateStatusProtocol = false;
  this->flagRequestSignedCertificateTimestamp = false;
}

bool TransportLayerSecurityServer::Session::setIncomingRandomBytes(
  List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  if (this->incomingRandomBytes.size != 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Session random bytes already set. ";
    }
    return false;
  }
  this->incomingRandomBytes = input;
  return true;
}

void TransportLayerSecurityServer::Session::initialize() {
  this->socketId = - 1;
  this->incomingRandomBytes.setSize(0);
  this->incomingAlgorithmSpecifications.setSize(0);
  this->chosenCipher = 0;
  this->chosenSignatureAlgorithm = 0;
  this->chosenEllipticCurve = 0;
  this->chosenEllipticCurveName = "";
  Crypto::Random::getRandomBytesSecureInternalMayLeaveTracesInMemory(
    this->myRandomBytes, SSLContent::LengthRandomBytesInSSLHello
  );
  this->ephemerealPrivateKey = 0;
  this->serverName.setSize(0);
  this->bytesToSign.setSize(0);
  this->signature.setSize(0);
  this->incomingCiphers.setSize(0);
}

bool TransportLayerSecurityServer::handShakeIamServer(
  int inputSocketID, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::handShakeIamServer");
  this->session.initialize();
  this->session.socketId = inputSocketID;
  bool success = this->readBytesDecodeOnce(commentsOnFailure);
  if (this->spoofer.flagDoSpoof && this->spoofer.currentInputMessageIndex > 0) {
    this->spoofer.incomingMessages[
      this->spoofer.currentInputMessageIndex - 1
    ] = this->lastReaD;
  }
  if (!success) {
    global << Logger::red << commentsOnFailure->str() << Logger::endL;
    if (this->spoofer.flagDoSpoof) {
      this->spoofer.errorsOnIncoming.addOnTop(commentsOnFailure->str());
    }
    return false;
  }
  success = TransportLayerSecurityServer::replyToClientHello(inputSocketID, commentsOnFailure);
  if (!success) {
    global << Logger::red << commentsOnFailure->str() << Logger::endL;
    if (this->spoofer.flagDoSpoof) {
      this->spoofer.errorsOnOutgoing.addOnTop(commentsOnFailure->str());
    }
    return false;
  }
  //global.fatal << "Handshake i am server not implemented yet. " << global.fatal;
  return false;
}

bool TransportLayerSecurity::handShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure) {
  if (!this->flagUseBuiltInTlS) {
    return this->openSSLData.handShakeIamServer(inputSocketID, commentsOnFailure);
  }
  return this->server.handShakeIamServer(inputSocketID, commentsOnFailure);
}

bool TransportLayerSecurity::handShakeIAmClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("WebServer::handShakeIAmClientNoSocketCleanup");
  this->openSSLData.checkCanInitializeToClient();
  this->initializeNonThreadSafeOnFirstCall(false);
  this->openSSLData.checkCanInitializeToClient();
  return this->openSSLData.handShakeIAmClientNoSocketCleanup(inputSocketID, commentsOnFailure, commentsGeneral);
}

int TransportLayerSecurity::readOnce(
  int socket,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::readOnce");

  bool useSSL = global.flagUsingSSLinCurrentConnection && global.flagSSLAvailable;
  this->readBuffer.setSize(this->readBufferStandardSize);
  int result = - 1;

  if (useSSL) {
    result = this->openSSLData.sslRead(readBuffer, outputError, commentsGeneral, includeNoErrorInComments);
  } else {
    result = static_cast<int>(recv(
      socket,
      this->readBuffer.objects,
      static_cast<size_t>(readBuffer.size - 1), 0
    ));
  }
  return result;
}

int TransportLayerSecurity::writeOnce(
  int socket,
  List<char>& writeBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  bool useSSL = global.flagUsingSSLinCurrentConnection && global.flagSSLAvailable;
  if (useSSL) {
    return this->openSSLData.sslWrite(
      writeBuffer,
      outputError,
      commentsGeneral,
      commentsOnError,
      includeNoErrorInComments
    );
  } else {
    if (socket < 0) {
      if (commentsOnError != nullptr) {
        *commentsOnError << "Negative socket not allowed. ";
      }
      return - 1;
    }
    return static_cast<int>(send(
      socket,
      &writeBuffer[0],
      static_cast<unsigned>(writeBuffer.size),
      0
    ));
  }
}
