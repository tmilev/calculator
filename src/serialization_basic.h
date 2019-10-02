#ifndef SERIALIZATION_BASIC_ALREADY_DEFINED
#define SERIALIZATION_BASIC_ALREADY_DEFINED
#include "general_lists.h"
static ProjectInformationInstance projectInfoSerializationBasic(__FILE__, "Byte-level serialization routines.");

class Serialization {
public:
  class JSLabels {
  public:
    static std::string length;
  };
  template <unsigned int numberOfBytes>
  class LengthWriterNBytes {
  public:
    List<unsigned char>* outputPointer;
    JSData* annotation;
    int offset;
    LengthWriterNBytes(List<unsigned char>& output) {
      this->outputPointer = &output;
      this->offset = output.size;
      this->annotation = nullptr;
      int theSize = output.size;
      Serialization::WriteNByteUnsignedInt(numberOfBytes, 0, output, theSize);
    }
    ~LengthWriterNBytes() {
      unsigned int totalLength = static_cast<unsigned int>(this->outputPointer->size - this->offset - numberOfBytes);
      Serialization::WriteNByteUnsignedInt(numberOfBytes, totalLength, *this->outputPointer, this->offset);
      if (this->annotation != nullptr) {
        (*this->annotation)[Serialization::JSLabels::length] = static_cast<int64_t>(totalLength);
      }
    }
  };
  typedef LengthWriterNBytes<3> LengthWriterThreeBytes;
  typedef LengthWriterNBytes<2> LengthWriterTwoBytes;
  static bool ReadTwoByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool ReadThreeByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool ReadNByteInt(int numBytes,
    const List<unsigned char>& input,
    int& inputOutputOffset,
    int& result,
    std::stringstream* commentsOnFailure
  );
  static bool ReadTwoByteLengthFollowedByBytesDontOutputOffset(
    const List<unsigned char>& input,
    int Offset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static bool ReadTwoByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    int& outputOffset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static bool ReadOneByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    int& outputOffset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static bool ReadNByteLengthFollowedByBytes(int numBytesLength,
    const List<unsigned char>& input,
    int& outputOffset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static void WriteNByteUnsignedInt(// how many bytes are used to record the length
    int byteCountOfLength,
    unsigned int input,
    List<unsigned char>& output,
    int& inputOutputOffset
  );
  static void WriteTwoByteInt(
    int input,
    List<unsigned char>& output
  );
  static void WriteThreeByteInt(
    int input,
    List<unsigned char>& output
  );
  static void WriteTwoByteUnsignedInt(
    unsigned int input,
    List<unsigned char>& output
  );
  static void WriteThreeByteUnsignedInt(
    unsigned int input,
    List<unsigned char>& output
  );
  static void WriteOneByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    List<unsigned char>& output
  );
  static void WriteTwoByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    List<unsigned char>& output
  );
  static void WriteNByteLengthFollowedByBytes(
    // how many bytes are used to record the length
    int byteCountOfLength,
    const List<unsigned char>& input,
    List<unsigned char>& output,
    int& inputOutputOffset
  );
};

#endif // SERIALIZATION_BASIC_ALREADY_DEFINED
