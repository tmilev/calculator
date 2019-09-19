#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"

#include <unistd.h> //<- close, open defined here

extern ProjectInformationInstance projectInfoInstanceTransportLayerSecurityOpenSSLImplementation;
ProjectInformationInstance projectInfoInstanceTransportLayerSecurityOpenSSLImplementation(__FILE__, "Contains all openSSL-related implementation.");

std::string TransportLayerSecurityOpenSSL::errors::errorWantRead = "SSL_ERROR_WANT_READ";
bool TransportLayerSecurityOpenSSL::flagSSLlibraryInitialized = false;

extern logger logServer;
extern logger logOpenSSL;

TransportLayerSecurityOpenSSL::~TransportLayerSecurityOpenSSL() {
  this->FreeSSL();
  this->FreeContext();
}

void TransportLayerSecurityOpenSSL::FreeSSL() {
  SSL_free(this->sslData);
  this->sslData = nullptr;
}

void TransportLayerSecurityOpenSSL::FreeEverythingShutdownSSL() {
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return;
  }
  theGlobalVariables.flagSSLIsAvailable = false;
  this->FreeSSL();
  this->FreeContext();
}

void TransportLayerSecurityOpenSSL::FreeContext() {
  //SSL_CTX_free (this->contextClient);
  if (this->context != nullptr && this->name != "") {
    logOpenSSL << logger::blue << "DEBUG: Freeing openSSL context: " << this->name << ". " << logger::endL;
  }
  SSL_CTX_free (this->context);
  //this->contextClient = 0;
  this->context = nullptr;
}

void TransportLayerSecurityOpenSSL::initSSLLibrary() {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLlibrary");
  if (this->flagSSLlibraryInitialized) {
    return;
  }
  this->flagSSLlibraryInitialized = true;
  std::stringstream commentsOnError;
  // this command loads error strings and initializes openSSL.
  SSL_load_error_strings();
  int loadedSuccessfully = OpenSSL_add_ssl_algorithms();
  if (!loadedSuccessfully) {
    logServer << logger::red << commentsOnError.str() << logger::endL;
    crash << "Failed to add ssl algorithms. " << crash;
  }
  if (commentsOnError.str() != "") {
    logServer << logger::red << "OpenSSL initialization comments: " << logger::blue << commentsOnError.str() << logger::endL;
  }
}

