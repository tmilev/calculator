#include "vpfHeader5Crypto.h"
ProjectInformationInstance projectInfoCryptoFile1(__FILE__, "SHA-1 and base64 implementation.");

unsigned char Crypto::GetCharFrom6bit(uint32_t input)
{ switch(input)
  { case 0: return  'A';
    case 1: return  'B';
    case 2: return  'C';
    case 3: return  'D';
    case 4: return  'E';
    case 5: return  'F';
    case 6: return  'G';
    case 7: return  'H';
    case 8: return  'I';
    case 9: return  'J';
    case 10: return 'K';
    case 11: return 'L';
    case 12: return 'M';
    case 13: return 'N';
    case 14: return 'O';
    case 15: return 'P';
    case 16: return 'Q';
    case 17: return 'R';
    case 18: return 'S';
    case 19: return 'T';
    case 20: return 'U';
    case 21: return 'V';
    case 22: return 'W';
    case 23: return 'X';
    case 24: return 'Y';
    case 25: return 'Z';
    case 26: return 'a';
    case 27: return 'b';
    case 28: return 'c';
    case 29: return 'd';
    case 30: return 'e';
    case 31: return 'f';
    case 32: return 'g';
    case 33: return 'h';
    case 34: return 'i';
    case 35: return 'j';
    case 36: return 'k';
    case 37: return 'l';
    case 38: return 'm';
    case 39: return 'n';
    case 40: return 'o';
    case 41: return 'p';
    case 42: return 'q';
    case 43: return 'r';
    case 44: return 's';
    case 45: return 't';
    case 46: return 'u';
    case 47: return 'v';
    case 48: return 'w';
    case 49: return 'x';
    case 50: return 'y';
    case 51: return 'z';
    case 52: return '0';
    case 53: return '1';
    case 54: return '2';
    case 55: return '3';
    case 56: return '4';
    case 57: return '5';
    case 58: return '6';
    case 59: return '7';
    case 60: return '8';
    case 61: return '9';
    case 62: return '+';
    case 63: return '/';
    default: crash << "Requesting character from a purported 6 bit integer, which in fact has more significant bits. " << crash;
      break;
  }
  return -1;
}

bool Crypto::Get6bitFromChar(char input, uint32_t& output)
{ switch (input)
  { case 'A': output= 0;  return true;
    case 'B': output= 1;  return true;
    case 'C': output= 2;  return true;
    case 'D': output= 3;  return true;
    case 'E': output= 4;  return true;
    case 'F': output= 5;  return true;
    case 'G': output= 6;  return true;
    case 'H': output= 7;  return true;
    case 'I': output= 8;  return true;
    case 'J': output= 9;  return true;
    case 'K': output= 10; return true;
    case 'L': output= 11; return true;
    case 'M': output= 12; return true;
    case 'N': output= 13; return true;
    case 'O': output= 14; return true;
    case 'P': output= 15; return true;
    case 'Q': output= 16; return true;
    case 'R': output= 17; return true;
    case 'S': output= 18; return true;
    case 'T': output= 19; return true;
    case 'U': output= 20; return true;
    case 'V': output= 21; return true;
    case 'W': output= 22; return true;
    case 'X': output= 23; return true;
    case 'Y': output= 24; return true;
    case 'Z': output= 25; return true;
    case 'a': output= 26; return true;
    case 'b': output= 27; return true;
    case 'c': output= 28; return true;
    case 'd': output= 29; return true;
    case 'e': output= 30; return true;
    case 'f': output= 31; return true;
    case 'g': output= 32; return true;
    case 'h': output= 33; return true;
    case 'i': output= 34; return true;
    case 'j': output= 35; return true;
    case 'k': output= 36; return true;
    case 'l': output= 37; return true;
    case 'm': output= 38; return true;
    case 'n': output= 39; return true;
    case 'o': output= 40; return true;
    case 'p': output= 41; return true;
    case 'q': output= 42; return true;
    case 'r': output= 43; return true;
    case 's': output= 44; return true;
    case 't': output= 45; return true;
    case 'u': output= 46; return true;
    case 'v': output= 47; return true;
    case 'w': output= 48; return true;
    case 'x': output= 49; return true;
    case 'y': output= 50; return true;
    case 'z': output= 51; return true;
    case '0': output= 52; return true;
    case '1': output= 53; return true;
    case '2': output= 54; return true;
    case '3': output= 55; return true;
    case '4': output= 56; return true;
    case '5': output= 57; return true;
    case '6': output= 58; return true;
    case '7': output= 59; return true;
    case '8': output= 60; return true;
    case '9': output= 61; return true;
    case '+': output= 62; return true;
    case '/': output= 63; return true;
    case '=': return false;
    default: return false;
  }
  return false;
}

bool Crypto::StringBase64ToString(const std::string& input, std::string& output, std::stringstream* comments)
{ List<unsigned char> byteList;
  if (!Crypto::StringBase64ToBitStream(input, byteList, comments))
    return false;
  Crypto::ConvertBitStreamToString(byteList, output);
  return true;
}

bool Crypto::StringBase64ToBitStream(const std::string& input, List<unsigned char>& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Crypto::StringBase64ToBitStream");
  output.Reserve((3*input.size())/4+1);
  output.SetSize(0);
  uint32_t theStack=0, sixBitDigit=0;
  int numBitsInStack=0;
  for (unsigned i=0; i<input.size(); i++)
  { if (!Crypto::Get6bitFromChar(input[i], sixBitDigit))
    { if (input[i]!='=')
      { if (comments!=0)
          *comments << "<hr>Error: the input string: <br>\n" << input << "\n<br>had characters outside of base64";
        return false;
      }
      theStack=0;
      numBitsInStack+=6;
      numBitsInStack%=8;
      continue;
    }
    theStack*=64;
    theStack+=sixBitDigit;
    numBitsInStack+=6;
    if (numBitsInStack==12)
    { output.AddOnTop(theStack/16);
      numBitsInStack=4;
      theStack=theStack%16;
    }
    if (numBitsInStack==8)
    { output.AddOnTop(theStack);
      numBitsInStack=0;
      theStack=0;
    }
    if (numBitsInStack==10)
    { output.AddOnTop(theStack/4);
      numBitsInStack=2;
      theStack=theStack%4;
    }
  }
//  stOutput << "<br>output is: " << output << ", Converted back: " << Crypto::CharsToBase64String(output);
  if (comments!=0 && numBitsInStack!=0)
  { *comments << "<br>Input " << input << " corresponds modulo 8 to " << numBitsInStack
    << " bits. Perhaps the input was not padded correctly with = signs.";
  }
  return true;
}

void Crypto::ConvertBitStreamToString(const List<unsigned char>& input, std::string& output)
{ MacroRegisterFunctionWithName("Crypto::ConvertBitStreamToString");
  output.clear();
  output.reserve(input.size);
  for (int i=0; i<input.size; i++)
    output.push_back(input[i]);
}

void Crypto::ConvertStringToBitStream(const std::string& input, List<unsigned char>& output)
{ MacroRegisterFunctionWithName("Crypto::ConvertStringToBitStream");
  output.SetSize(input.size());
  for (unsigned i=0; i<input.size(); i++)
    output[i]=input[i];
}

std::string Crypto::computeSha1outputBase64(const std::string& inputString)
{ MacroRegisterFunctionWithName("Crypto::computeSha1outputBase64");
  List<uint32_t> theSha1Uint;
  List<unsigned char> theSha1Uchar;
  Crypto::computeSha1(inputString, theSha1Uint);
  Crypto::ConvertUint32ToUcharBigendian(theSha1Uint, theSha1Uchar);
  return Crypto::CharsToBase64String(theSha1Uchar);
}


std::string Crypto::CharsToBase64String(const std::string& input)
{ List<unsigned char> inputChar;
  inputChar.SetSize(input.size());
  for (unsigned i=0; i<input.size(); i++)
    inputChar[i]=input[i];
  return Crypto::CharsToBase64String(inputChar);
}

std::string Crypto::CharsToBase64String(const List<unsigned char>& input)
{ MacroRegisterFunctionWithName("Crypto::CharsToBase64String");
  uint32_t theStack=0;
  int numBitsInTheStack=0;
  std::string result;
  result.reserve((input.size*4)/3+1);
  for (int i =0; i<input.size; i++)
  { theStack*=256;
    theStack+=input[i];
    numBitsInTheStack+=8;
    if (numBitsInTheStack==8)
    { result.push_back(Crypto::GetCharFrom6bit(theStack/4));
      numBitsInTheStack=2;
      theStack%=4;
    }
    if (numBitsInTheStack==10)
    { result.push_back(Crypto::GetCharFrom6bit(theStack/16));
      numBitsInTheStack=4;
      theStack%=16;
    }
    if (numBitsInTheStack==12)
    { result.push_back(Crypto::GetCharFrom6bit(theStack/64));
      result.push_back(Crypto::GetCharFrom6bit(theStack%64));
      numBitsInTheStack=0;
      theStack=0;
    }
  }
  if (numBitsInTheStack==2)
  { result.push_back(Crypto::GetCharFrom6bit(theStack*16));
    result.push_back('=');
    result.push_back('=');
  }
  if (numBitsInTheStack==4)
  { result.push_back(Crypto::GetCharFrom6bit(theStack*4));
    result.push_back('=');
  }
  return result;
}

uint32_t Crypto::leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate)
{ uint32_t result=input;
  for (int i=0; i<numBitsToRotate; i++)
  { uint32_t last_bit = result/2147483648;
    result*=2;
    result+=last_bit;
  }
  return result;
}

void Crypto::ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output)
{ MacroRegisterFunctionWithName("Crypto::ConvertUint32ToUcharBigendian");
  output.SetSize(input.size*4);
  for (int i=0; i<input.size; i++)
  { output[i*4+0]=input[i]/16777216;
    output[i*4+1]=(input[i]/65536)%256;
    output[i*4+2]=(input[i]/256)%256;
    output[i*4+3]=input[i]%256;
  }

}

void Crypto::convertUint64toBigendianStringAppendResult(uint64_t& input, std::string& outputAppend)
{ //the following code should work on both big- and little-endian systems:
  outputAppend.push_back((unsigned char)  (input/72057594037927936) );
  outputAppend.push_back((unsigned char) ((input/281474976710656)%256 ));
  outputAppend.push_back((unsigned char) ((input/1099511627776)%256 ));
  outputAppend.push_back((unsigned char) ((input/4294967296)%256 ));
  outputAppend.push_back((unsigned char) ((input/16777216)%256 ));
  outputAppend.push_back((unsigned char) ((input/65536)%256 ));
  outputAppend.push_back((unsigned char) ((input/256)%256 ));
  outputAppend.push_back((unsigned char)  (input%256 ));
}

void Crypto::GetUInt32FromCharBigendianPadLastIntWithZeroes(const List<unsigned char>& input, List<uint32_t>& output)
{ MacroRegisterFunctionWithName("Crypto::GetUInt32FromCharBigendianPadLastIntWithZeroes");
  List<unsigned char> theConvertor;
  theConvertor.SetSize(4);
  int finalSize=input.size/4;
  if (input.size%4!=0)
    finalSize++;
  output.SetSize(finalSize);
  for (int i=0; i<output.size; i++)
  { for (int j=0; j<4; j++)
      theConvertor[j]= (i*4+j<input.size) ? input[i*4+j] : 0;
    output[i]=Crypto::GetUInt32FromCharBigendian(theConvertor);
  }
}

uint32_t Crypto::GetUInt32FromCharBigendian(const List<unsigned char>& input)
{ uint32_t result=input[0];
  for (int i=1; i<4; i++)
  { result*=256;
    result+=input[i];
  }
  return result;
}

void Crypto::computeSha1(const std::string& inputString, List<uint32_t>& output)
{ MacroRegisterFunctionWithName("Crypto::computeSha1");
  //Reference: wikipedia page on sha1.
  //the Algorithm here is a direct implementation of the Wikipedia pseudocode.
  uint32_t h0 = 0x67452301;
  uint32_t h1 = 0xEFCDAB89;
  uint32_t h2 = 0x98BADCFE;
  uint32_t h3 = 0x10325476;
  uint32_t h4 = 0xC3D2E1F0;
  uint64_t messageLength=inputString.size()*8;//*sizeof(unsigned char);
  std::string inputStringPreprocessed=inputString;
//  if (messageLength%256==0)
  inputStringPreprocessed.push_back(0x80);
  unsigned numbytesMod64= inputStringPreprocessed.size() %64;
  if (numbytesMod64>56)
  { for (unsigned i=numbytesMod64; i<64; i++)
      inputStringPreprocessed.push_back(0);
    numbytesMod64=0;
  }
  for (int i=numbytesMod64; i<56; i++)
    inputStringPreprocessed.push_back(0);
  Crypto::convertUint64toBigendianStringAppendResult(messageLength, inputStringPreprocessed);
  List<unsigned char> convertorToUint32;
  List<uint32_t> inputStringUint32;
  inputStringUint32.Reserve(inputStringPreprocessed.size()/4);
  convertorToUint32.SetSize(4);
  for (unsigned i=0; i<inputStringPreprocessed.size()/4; i++)
  { convertorToUint32[0]=inputStringPreprocessed[i*4];
    convertorToUint32[1]=inputStringPreprocessed[i*4+1];
    convertorToUint32[2]=inputStringPreprocessed[i*4+2];
    convertorToUint32[3]=inputStringPreprocessed[i*4+3];
    inputStringUint32.AddOnTop(Crypto::GetUInt32FromCharBigendian(convertorToUint32));
  }
  List<uint32_t> currentChunk;
  currentChunk.SetSize(80);
  uint32_t a=0,b=0,c=0,d=0,e=0, f=0, k=0, temp=0;
  for (int chunkCounter=0; chunkCounter<inputStringUint32.size; chunkCounter+=16)
  { for (int j=0; j<16; j++)
      currentChunk[j]=inputStringUint32[chunkCounter+j];
    for (int j=16; j<80; j++)
      currentChunk[j]=
      Crypto::leftRotateAsIfBigEndian((
      currentChunk[j-3] xor
      currentChunk[j-8] xor
      currentChunk[j-14] xor
      currentChunk[j-16]), 1 );
    a=h0;
    b=h1;
    c=h2;
    d=h3;
    e=h4;
    for (int j=0; j<80; j++)
    { if (j<20)
      { f= (b bitand c) bitor ((compl b) bitand d);
        k=0x5A827999;
      } else if (20 <=j and j<40)
      { f= b xor c xor d;
        k=0x6ED9EBA1;
      } else if (40<=j and j<60)
      { f = (b bitand c) bitor (b bitand d) bitor (c bitand d);
        k = 0x8F1BBCDC;
      } else //60<=j
      { f = b xor c xor d;
        k = 0xCA62C1D6;
      }
      temp= Crypto::leftRotateAsIfBigEndian(a, 5)+ f + e + k + currentChunk[j];
      e = d;
      d = c;
      c = Crypto::leftRotateAsIfBigEndian(b, 30);
      b = a;
      a = temp;
    }
    h0+=a;
    h1+=b;
    h2+=c;
    h3+=d;
    h4+=e;
  }
  output.SetSize(5);
  output[0]=h0;
  output[1]=h1;
  output[2]=h2;
  output[3]=h3;
  output[4]=h4;
}
