#include "vpfHeader1General0_General.h"
ProjectInformationInstance projectInfoCryptoFile1(__FILE__, "SHA-1 and base64 implementation.");

class Crypto
{
public:
static std::string CharsToBase64String(const std::string& input, std::string& output);
static bool StringBase64ToBitStream(const std::string& input, List<unsigned char>& output);
static bool get6bitFromChar(char input, uint32_t& output);
};

bool Crypto::StringBase64ToBitStream(const std::string& input, List<unsigned char>& output)
{ MacroRegisterFunctionWithName("Crypto::StringBase64ToBitStream");
  if (input.size()%3!=0)
  { stOutput << "Input of StringBase64ToBitStream has length " << input.size() << " which is not divible by 3. ";
    return false;
  }
  output.ReservE((3*input.size())/4+1);
  output.SetSize(0);
  int indexInInput=0;
  uint32_t Arithmetics, sixBitDigit1;
/*  for (; indexInInput<input.size(); indexInInput+=4)
  { Arithmetics=0;
    for (int i=indexInInput; i<indexInInput+4 && i<input.size(); i++ )

  }
    switch(input[indexInInput])
    { case 'A':


      case '='
    }*/
}

