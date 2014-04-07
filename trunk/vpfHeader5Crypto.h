#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "vpfHeader1General0_General.h"
static ProjectInformationInstance projectInfoCryptoHeader(__FILE__, "Crypto class declaration.");

class Crypto
{
public:
  static std::string CharsToBase64String(const List<unsigned char>& input);
  static bool StringBase64ToBitStream(const std::string& input, List<unsigned char>& output, std::stringstream* comments=0);
  static bool Get6bitFromChar(char input, uint32_t& output);
  static unsigned char GetCharFrom6bit(uint32_t input);
};
#endif // header_crypto_was_already_defined
