 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_crypto_public_domain_ALREADY_INCLUDED
#define header_crypto_public_domain_ALREADY_INCLUDED

#include "crypto.h"

// Class CryptoPublicDomain serves the same purpose as
// class Crypto, but excludes all code not written
// specifically for the purposes of the calculator project.
// however e
class CryptoPublicDomain {
public:
  static void computeRIPEMD160(
    const std::string& input, List<unsigned char>& output
  );
  static bool encryptAES_CBC_256(
    const std::string& inputKey,
    const std::string& inputPlainText,
    List<unsigned char>& output,
    std::stringstream* commentsOnFailure
  );
  static bool decryptAES_CBC_256(
    const std::string& inputKey,
    const std::string& inputCipherText,
    List<unsigned char>& output,
    std::stringstream* commentsOnFailure
  );
  CryptoPublicDomain();
};

#endif // header_crypto_public_domain_ALREADY_INCLUDED
