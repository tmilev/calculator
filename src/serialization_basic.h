#ifndef SERIALIZATION_BASIC_ALREADY_DEFINED
#define SERIALIZATION_BASIC_ALREADY_DEFINED
#include "general_lists.h"
#include "json.h"
static ProjectInformationInstance projectInfoSerializationBasic(__FILE__, "Byte-level serialization routines. ");

class Serialization {
public:
  class JSLabels {
  public:
    static std::string length;
    static std::string body;
    static std::string label;
    static std::string offset;
    static std::string markers;
  };
  class Marker {
  public:
    int offset;
    int length;
    std::string label;
    Marker(int inputOffset, int inputLength, const std::string& inputLabel):
      offset(inputOffset),
      length(inputLength),
      label(inputLabel) {
    }
    Marker(): offset(- 1), length(- 1) {
    }
    JSData ToJSON();
  };
  class WriterUnsignedIntegerWithMarker {
  public:
    List<unsigned char>* outputPointer;
    List<Serialization::Marker>* outputMarkers;
    int offset;
    int markerOffset;
    WriterUnsignedIntegerWithMarker(
      int numberOfBytes,
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* theOutputMarkers,
      const std::string& label
    ) {
      this->outputMarkers = theOutputMarkers;
      int notUsed = 0;
      Serialization::WriteNByteUnsignedInt(numberOfBytes, input, output, notUsed);
      if (theOutputMarkers == nullptr) {
        return;
      }
      this->offset = output.size - 1;
      this->markerOffset = this->outputMarkers->size;
      this->outputPointer = &output;
      this->outputMarkers->AddOnTop(Serialization::Marker(this->offset, - 1, label));
      this->outputMarkers->AddOnTop(Serialization::Marker(this->offset, 1, label));
    }
    ~WriterUnsignedIntegerWithMarker(){
      if (this->outputMarkers == nullptr) {
        return;
      }
      (*this->outputMarkers)[this->markerOffset].length = this->outputPointer->size - this->offset;
    }
  };

  template <unsigned int numberOfBytes>
  class LengthWriterNBytes {
  public:
    List<unsigned char>* outputPointer;
    List<Serialization::Marker>* outputMarkers;
    int offset;
    int markerOffset;
    LengthWriterNBytes(
      List<unsigned char>& output,
      List<Serialization::Marker>* markers,
      const std::string& label
    ) {
      this->outputPointer = &output;
      this->outputMarkers = markers;
      this->markerOffset = - 1;
      this->offset = output.size;
      int theSize = output.size;
      Serialization::WriteNByteUnsignedInt(numberOfBytes, 0, output, theSize);
      if (this->outputMarkers != nullptr) {
        this->markerOffset = this->outputMarkers->size;
        this->outputMarkers->AddOnTop(Serialization::Marker(this->offset, - 1, Serialization::JSLabels::length));
        this->outputMarkers->AddOnTop(Serialization::Marker(this->offset, numberOfBytes, label));
      }
    }
    ~LengthWriterNBytes() {
      unsigned int totalLength = static_cast<unsigned int>(this->outputPointer->size - this->offset - numberOfBytes);
      Serialization::WriteNByteUnsignedInt(numberOfBytes, totalLength, *this->outputPointer, this->offset);
      if (this->outputMarkers != nullptr) {
        (*this->outputMarkers)[this->markerOffset].length = totalLength + numberOfBytes;
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
  static std::string ConvertListUnsignedCharsToHex(const List<unsigned char>& input);
};

#endif // SERIALIZATION_BASIC_ALREADY_DEFINED
