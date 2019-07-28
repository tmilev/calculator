//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "vpfHeader5Crypto.h"

ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation(__FILE__, "Abstract syntax notation one (ASN-1) implementation");

AbstractSyntaxNotationOneSubsetObject::AbstractSyntaxNotationOneSubsetObject() {
  this->owner = 0;
  this->indexInOwner = - 1;
}

bool AbstractSyntaxNotationOneSubsetDecoder::isCostructedByte(unsigned char input) {
  int sixthBit = input / 32;
  sixthBit %= 2;
  return sixthBit != 0;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrentConstructed(std::stringstream* commentsOnError) {
  unsigned char startByte = this->rawData[this->dataPointer];
  if (!this->isCostructedByte(startByte)) {
    crash << "Byte " << (int) startByte << " must be constructed. " << crash;
  }
  unsigned char nonConstructedPart = startByte - 32;
  AbstractSyntaxNotationOneSubsetDecoder::typeDecoder handler = this->decodersByByteValue[nonConstructedPart];
  if (handler == 0) {
    if (commentsOnError != 0) {
      *commentsOnError << "No handler for byte: " << (int) startByte << ". ";
    }
    return false;
  }
  this->dataPointer ++;
  return handler(*this, commentsOnError);
}

AbstractSyntaxNotationOneSubsetDecoder::AbstractSyntaxNotationOneSubsetDecoder() {
  this->flagLogByteInterpretation = false;
}

void AbstractSyntaxNotationOneSubsetDecoder::initialize() {
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointer(
  int& outputLengthNegativeOneForVariable, std::stringstream* commentsOnError
) {
  outputLengthNegativeOneForVariable = 0;
  unsigned char currentByte = this->rawData[this->dataPointer];
  if (currentByte < 128) { // 128 = 0x80
    outputLengthNegativeOneForVariable = currentByte;
    this->dataPointer ++;
    return true;
  }
  if (currentByte == 128) { // 128 = 0x80 signals varible-length encoding
    outputLengthNegativeOneForVariable = - 1;
    this->dataPointer ++;
    return true;
  }
  int numberOfLengthBytes = currentByte - 128;
  LargeInt length = 0;
  this->dataPointer ++;
  for (int i = 0; i < numberOfLengthBytes; i ++) {
    length *= 256;
    if (this->PointerIsBad(commentsOnError)) {
      return false;
    }
    length += this->rawData[this->dataPointer];
  }
  if (!length.IsIntegerFittingInInt(&outputLengthNegativeOneForVariable)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Variable length is too large. ";
    }
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::PointerIsBad(std::stringstream* commentsOnError) {
  if (this->dataPointer >= (signed) this->rawData.size() || this->dataPointer < 0) {
    if (commentsOnError != 0) {
      *commentsOnError << "Unexpected overflow error: data pointer is negative. ";
    }
    return true;
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent(std::stringstream* commentsOnError) {
  if (this->PointerIsBad(commentsOnError)) {
    return false;
  }
  unsigned char startByte = this->rawData[this->dataPointer];
  this->dataPointer ++;
  int currentLength = 0;
  if (!this->DecodeLengthIncrementDataPointer(currentLength, commentsOnError)) {
    return false;
  }
  if (currentLength < 0) {
    if (commentsOnError != 0) {
      *commentsOnError << "Variable length encoding not implemented. ";
    }
    return false;
  }
  if (commentsOnError != 0) {
    *commentsOnError << "Decoded length: " << currentLength << ". ";
  }
  return false;
}


bool AbstractSyntaxNotationOneSubsetDecoder::Decode(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::Decode");
  unsigned int maxAllowedSize = 1000000000;
  if (this->rawData.size() >= maxAllowedSize) {
    if (commentsOnError != 0) {
      *commentsOnError << "Input size: " << this->rawData.size() << " too large, max allowed: " << maxAllowedSize << ". ";
    }
    return false;
  }
  this->initialize();
  this->dataPointer = 0;
  this->DecodeCurrent(commentsOnError);
  if (this->dataPointer < (signed) this->rawData.size()) {
    if (commentsOnError != 0) {
      *commentsOnError << "Decoded " << this->dataPointer << " bytes but the input had: " << this->rawData.size() << ". ";
    }
    return false;
  }
  return true;
}

std::string AbstractSyntaxNotationOneSubsetDecoder::ToStringDebug() const {
  std::stringstream out;
  out << "Data: ";
  out << MathRoutines::StringShortenInsertDots(Crypto::ConvertStringToHex(this->rawData), 4000);
  return out.str();
}

/*
#define RET_TAG_NEXT_SAME_LEVEL_THIS_OBJECT_ONLY			0
#define RET_TAG_NEXT_SAME_LEVEL_OBJECT_CONTAINS				1

struct ASN1{
  unsigned char tag;
  int length;
  char data[2048];
  char value[2048];
  short activeStatus;
};

struct X509_TAG_OBJECT{
  int numberOfTagElements;
  int numberOfasnElements;
  struct ASN1 asnObjectList[32];
};


int convertHexStringToByte(char *dataset,unsigned char *bytearray){
  int i = strlen(dataset),j=0,counter=0;
  char c[2];
  unsigned int bytes[2];

  memset(bytearray,0x00,sizeof(bytearray));

  for(j=0;j<i;j++){
    if(0 == j%2){

      c[0] = dataset[j];
      c[1] = dataset[j+1];

      sscanf(c, "%02x", &bytes[0]);

      bytearray[counter] = bytes[0];

      counter++;
    }
  }

  return counter;

}

unsigned is_bit_set(unsigned value, unsigned bitindex)
{
  return (value & (1 << bitindex)) != 0;
}

int ipow(int base, int exp)
{
  int result = 1;
  while (exp)
  {
    if (exp & 1)
      result *= base;
    exp >>= 1;
    base *= base;
  }

  return result;
}

int read_generic_tag(char *hexStr,struct ASN1 *asnObject,unsigned char tag,int *nextPointIndex,int *endPointIndex,int *lengthOfData){
  char data[10240];
  int length = 0;
  int recvStrFullLength = 0;
  int startIndex = 0;
  int i = 0;
  int j = 0;

  recvStrFullLength = strlen(hexStr);

  memcpy(data,hexStr+2,2);
  data[2] = '\0';

  startIndex = 4;

  if(0 == strcmp("82",data)){
    memcpy(data,hexStr+4,4);
    data[4] = '\0';
    startIndex = 8;
  }else if(0 == strcmp("81",data)){
    memcpy(data,hexStr+4,2);
    data[2] = '\0';
    startIndex = 6;
  }

  length = (int)strtol(data, NULL, 16);

  asnObject->tag = tag;
  asnObject->length = length;
  memcpy(asnObject->data,hexStr+startIndex,(length*2));
  asnObject->data[(length*2)] = '\0';

  if(0x06 == tag){
    int tmpLen = 0;
    char temp[64];
    char tempTwo[64];
    int valueSet[10];
    int numberOfItem = 0;
    int tempVal = 0;

    strcpy(data,asnObject->data);
    strcpy(tempTwo,"");

    for (i = 0; i < strlen(data); i += 2) {
      strcpy(temp,"");
      memcpy(temp,data+i,2);
      temp[2] = '\0';

      tmpLen = (int)strtol(temp, NULL, 16);

      if(128 > tmpLen){

        if(0 == i && 40 <= tmpLen){
          if(0 != tmpLen%40){
            tempVal = (tmpLen - (tmpLen%40))/40;
            sprintf(temp,"%d",tempVal);
            strcat(tempTwo,temp);
            strcat(tempTwo,".");
            sprintf(temp,"%d",(tmpLen%40));
            strcat(tempTwo,temp);
          }else{
            tempVal = tmpLen/40;
            sprintf(temp,"%d",tempVal);
            strcat(tempTwo,temp);
            strcat(tempTwo,".0");
          }
        }else{
          sprintf(temp,"%d",tmpLen);
          strcat(tempTwo,temp);
        }

        strcat(tempTwo,".");
      }else{
        numberOfItem = 0;

        strcpy(temp,"");
        memcpy(temp,data+i+1,1);
        temp[1] = '\0';

        valueSet[numberOfItem] = atoi(temp);
        numberOfItem++;

        for (j = (i+2); j <= strlen(data); j += 2) {
          strcpy(temp,"");
          memcpy(temp,data+j,2);
          temp[2] = '\0';

          tmpLen = (int)strtol(temp, NULL, 16);

          if(128 <= tmpLen && 143 >= tmpLen){
            break;
          }else if(!is_bit_set(tmpLen,7)){
            i += 2;
            valueSet[numberOfItem] = tmpLen;
            numberOfItem++;
            break;
          }

          tmpLen &= ~(1 << 7);

          valueSet[numberOfItem] = tmpLen;

          i += 2;
          numberOfItem++;
        }

        tmpLen = 0;

        for (j = (numberOfItem-1); j >= 0; --j) {
          tmpLen += (valueSet[j] * ipow(2,(7*((numberOfItem-1)-j))));
        }

        sprintf(temp,"%d",tmpLen);
        strcat(tempTwo,temp);
        strcat(tempTwo,".");
      }
    }

    tempTwo[strlen(tempTwo)-1] = '\0';
    strcpy(asnObject->value,tempTwo);

    memcpy(temp,hexStr+(startIndex+(length*2)),4);
    temp[4] = '\0';

    if(0 == strcmp("0500",temp)){
      length += 2;
    }
  }else if(0x13 == tag || 0x14 == tag){
    unsigned byteData[256];

    memset(byteData,0x00,sizeof(byteData));

    strcpy(data,asnObject->data);

    convertHexStringToByte(data,byteData);

    sprintf(data,"%s",byteData);
    strcpy(asnObject->value,data);
  }else if(0x01 == tag){
    if(0 == strcasecmp("FF",asnObject->data)){
      strcpy(asnObject->value,"TRUE");
    }else{
      strcpy(asnObject->value,"FALSE");
    }
  }

  *nextPointIndex = startIndex;
  *endPointIndex  = (startIndex + (length*2));
  *lengthOfData 	= (length*2);

  if((recvStrFullLength-startIndex) == (length*2)){
    asnObject->activeStatus = 1;
    return RET_TAG_NEXT_SAME_LEVEL_THIS_OBJECT_ONLY;
  }else{
    asnObject->activeStatus = 0;
    return RET_TAG_NEXT_SAME_LEVEL_OBJECT_CONTAINS;
  }
}


int decode_hex_x509_certificate(char *hexStr){
  unsigned char byteData[5120];
  char hexStrCopy[10240];
  char data[10240];
  char tmpChar[10240];
  int startindex = 0;
  int startPoint = 0;
  int endPoint = 0;
  int fullStrLen = 0;
  int lastLen = 0;
  int endIndex = 0;
  int length = 0;
  int retVal = 0;
  short splitStatus = 0;
  unsigned char tag;

  struct ASN1 asnList[256];
  int asnObjectCounter = 0;
  int indexList[256];
  int indexListSize = 0;
  int i = 0;

  memset(byteData,0x00,sizeof(byteData));

  strcpy(hexStrCopy,hexStr);
  strcpy(data,hexStrCopy);

  convertHexStringToByte(data,byteData);

  strcpy(data,hexStrCopy);

  fullStrLen = strlen(hexStrCopy);

  while (1) {
    struct ASN1 asn;
    splitStatus = 0;

    tag = byteData[startindex];

    switch(byteData[startindex]){
    case 0x30:
      retVal = read_generic_tag(data,&asn,0x30,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x02:
      retVal = read_generic_tag(data,&asn,0x02,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x06:
      retVal = read_generic_tag(data,&asn,0x06,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x31:
      retVal = read_generic_tag(data,&asn,0x31,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x13:
      retVal = read_generic_tag(data,&asn,0x13,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x17:
      retVal = read_generic_tag(data,&asn,0x17,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x18:
      retVal = read_generic_tag(data,&asn,0x18,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x03:
      retVal = read_generic_tag(data,&asn,0x03,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x04:
      retVal = read_generic_tag(data,&asn,0x04,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x14:
      retVal = read_generic_tag(data,&asn,0x14,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    case 0x01:
      retVal = read_generic_tag(data,&asn,0x01,&startPoint,&endPoint,&length);
      splitStatus = 1;
      break;
    default:
      sprintf(tmpChar,"%02X",byteData[startindex]);

      if('A' == tmpChar[0]){
        retVal = read_generic_tag(data,&asn,byteData[startindex],&startPoint,&endPoint,&length);
        splitStatus = 1;
      }
      break;
    }

    if(splitStatus){

      startindex 	= ((lastLen+startPoint)/2);
      endIndex 	= ((lastLen+endPoint)/2);

      if(RET_TAG_NEXT_SAME_LEVEL_OBJECT_CONTAINS == retVal){
        indexList[indexListSize] = endIndex;
        indexListSize++;
      }

      asnList[asnObjectCounter] = asn;
      asnObjectCounter++;

      if(0x04 == tag || 0x03 == tag || 0x02 == tag || 0x01 == tag || 0x14 == tag || 0x17 == tag || 0x18 == tag){

        if(fullStrLen == (endIndex*2)){
          break;
        }

        if(0 < indexListSize){
          indexListSize--;
          startindex = indexList[indexListSize];
          length = fullStrLen - (startindex*2);

        }else{
          break;
        }

      }

    }else{
      if(0 < indexListSize){
        indexListSize--;
        startindex = indexList[indexListSize];
        length = fullStrLen - (startindex*2);

      }else{
        break;
      }
    }

    lastLen = (startindex*2);

    strcpy(data,"");
    memcpy(data,hexStrCopy+(startindex*2),length);
    data[length] = '\0';

  }

  return 0;
}

*/
