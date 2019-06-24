#include "transport_security_layer.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader5Crypto.h"

#include <unistd.h> //<- close, open defined here

ProjectInformationInstance projectInfoInstanceTransportSecurityLayerOpenSSLImplementation(__FILE__, "Contains all openSSL-related implementation.");

std::string TransportSecurityLayerOpenSSL::errors::errorWantRead = "SSL_ERROR_WANT_READ";
bool TransportSecurityLayerOpenSSL::flagSSLlibraryInitialized = false;

extern logger logServer;

TransportSecurityLayerOpenSSL::~TransportSecurityLayerOpenSSL() {
  this->FreeSSL();
  this->FreeContext();
}

void TransportSecurityLayerOpenSSL::FreeSSL() {
  SSL_free(this->sslData);
  this->sslData = 0;
}

void TransportSecurityLayerOpenSSL::FreeEverythingShutdownSSL() {
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return;
  }
  theGlobalVariables.flagSSLIsAvailable = false;
  this->FreeSSL();
  this->FreeContext();
}

void TransportSecurityLayerOpenSSL::FreeContext() {
  //SSL_CTX_free (this->contextClient);
  SSL_CTX_free (this->contextServer);
  //this->contextClient = 0;
  this->contextServer = 0;
}

void TransportSecurityLayerOpenSSL::initSSLlibrary() {
  MacroRegisterFunctionWithName("TransportSecurityLayerOpenSSL::initSSLlibrary");
  if (this->flagSSLlibraryInitialized) {
    return;
  }
  this->flagSSLlibraryInitialized = true;
  std::stringstream commentsOnError;
  // this command loads error strings and initializes openSSL.
  SSL_load_error_strings();
  //loadedSuccessfully = OPENSSL_init_ssl(0, NULL, &commentsOnError);
  //if (loadedSuccessfully != 1) {
  //  logServer << logger::red << commentsOnError.str() << logger::endL;
  //  crash << "Failed to initialize ssl library. " << crash;
  //}
  if (commentsOnError.str() != "") {
    logServer << logger::red << "OpenSSL initialization comments: " << logger::blue << commentsOnError.str() << logger::endL;
  }
  //logServer << logger::green << "DEBUG: InitialIzation of ssl successfull." << logger::endL;
}

