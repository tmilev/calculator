#include "crypto_calculator.h"
#include "crypto_public_domain.h"

extern CryptoPublicDomain bootStrapCrypto;
CryptoPublicDomain bootStrapCrypto;

CryptoPublicDomain::CryptoPublicDomain() {
  Crypto::externalCrypto().computeRIPEMD160 =
  CryptoPublicDomain::computeRIPEMD160;
  Crypto::externalCrypto().decryptAES_CBC_256 =
  CryptoPublicDomain::decryptAES_CBC_256;
  Crypto::externalCrypto().encryptAES_CBC_256 =
  CryptoPublicDomain::encryptAES_CBC_256;
}
