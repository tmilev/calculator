#ifndef SERIALIZATION_BASIC_ALREADY_DEFINED
#define SERIALIZATION_BASIC_ALREADY_DEFINED
#include "general_lists.h"
#include "json.h"

class Serialization {
public:
  class JSLabels {
  public:
    static std::string length;
    static std::string body;
    static std::string label;
    static std::string offset;
    static std::string markers;
    static std::string serialization;
  };
  class Marker {
  public:
    friend std::ostream& operator<<(std::ostream& output, const Serialization::Marker& marker) {
      output << marker.toString();
      return output;
    }
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
    JSData toJSON();
    std::string toString() const;
  };
  class WriterIntegerWithMarker {
  public:
    List<unsigned char>* outputPointer;
    List<Serialization::Marker>* outputMarkers;
    int offset;
    int markerOffset;
    bool flagDeallocated;
    int numberOfBytes;
    WriterIntegerWithMarker(
      int inputNumberOfBytes,
      int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* outputMarkers,
      const std::string& label
    ) {
      this->initialize(inputNumberOfBytes, static_cast<unsigned int>(input), output, outputMarkers, label);
    }
    WriterIntegerWithMarker(
      int inputNumberOfBytes,
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* outputMarkers,
      const std::string& label
    ) {
      this->initialize(inputNumberOfBytes, input, output, outputMarkers, label);
    }
    inline void initialize(
      int inputNumberOfBytes,
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* outputMarkers,
      const std::string& label
    ) {
      this->numberOfBytes  = inputNumberOfBytes;
      this->flagDeallocated = false;
      this->outputMarkers = outputMarkers;
      this->outputPointer = nullptr;
      this->markerOffset = 0;
      this->offset = 0;
      int notUsed = output.size;
      Serialization::writeNByteUnsigned(this->numberOfBytes, input, output, notUsed);
      if (this->outputMarkers == nullptr) {
        return;
      }
      this->offset = output.size - this->numberOfBytes;
      this->markerOffset = this->outputMarkers->size;
      this->outputPointer = &output;
      this->outputMarkers->addOnTop(Serialization::Marker(this->offset, - 1, label));
      this->outputMarkers->addOnTop(Serialization::Marker(this->offset, this->numberOfBytes, label));
    }
    ~WriterIntegerWithMarker() {
      if (this->flagDeallocated) {
        fatalCrash("Use after free of WriterUnsignedIntegerWithMarker. ");
      }
      this->flagDeallocated = true;
      if (this->outputMarkers == nullptr) {
        this->outputPointer = nullptr;
        return;
      }
      (*(this->outputMarkers))[this->markerOffset].length = this->outputPointer->size - this->offset;
      this->outputMarkers = nullptr;
      this->outputPointer = nullptr;
    }
  };

  template<int numberOfBytesTemplate>
  class WriterIntegerWithMarkerTemplate : public WriterIntegerWithMarker {
  public:
    WriterIntegerWithMarkerTemplate(
      int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* outputMarkers,
      const std::string& label
    ): WriterIntegerWithMarker(numberOfBytesTemplate, input, output, outputMarkers, label) {}
    WriterIntegerWithMarkerTemplate(
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* outputMarkers,
      const std::string& label
    ): WriterIntegerWithMarker(numberOfBytesTemplate, input, output, outputMarkers, label) {}
  };
  typedef WriterIntegerWithMarkerTemplate<1> WriterOneByteInteger;
  typedef WriterIntegerWithMarkerTemplate<2> WriterTwoByteInteger;
  typedef WriterIntegerWithMarkerTemplate<3> WriterThreeByteInteger;
  typedef WriterIntegerWithMarkerTemplate<4> WriterFourByteInteger;
  class LengthWriterNBytes {
  public:
    List<unsigned char>* outputPointer;
    List<Serialization::Marker>* outputMarkers;
    int offset;
    int markerOffset;
    int numberOfBytes;
    LengthWriterNBytes(
      int inputNumberOfBytes,
      List<unsigned char>& output,
      List<Serialization::Marker>* markers,
      const std::string& label
    ) {
      this->numberOfBytes = inputNumberOfBytes;
      this->outputPointer = &output;
      this->outputMarkers = markers;
      this->markerOffset = - 1;
      this->offset = output.size;
      int expectedSize = output.size;
      Serialization::writeNByteUnsigned(numberOfBytes, 0, output, expectedSize);
      if (this->outputMarkers != nullptr) {
        this->markerOffset = this->outputMarkers->size;
        this->outputMarkers->addOnTop(Serialization::Marker(this->offset, - 1, Serialization::JSLabels::length));
        this->outputMarkers->addOnTop(Serialization::Marker(this->offset, numberOfBytes, label));
      }
    }
    ~LengthWriterNBytes() {
      int totalLength = this->outputPointer->size - this->offset - numberOfBytes;
      Serialization::writeNByteUnsigned(
        this->numberOfBytes,
        static_cast<unsigned int>(totalLength),
        *this->outputPointer,
        this->offset
      );
      if (this->outputMarkers != nullptr) {
        (*this->outputMarkers)[this->markerOffset].length = totalLength + this->numberOfBytes;
      }
      this->outputMarkers = nullptr;
      this->outputPointer = nullptr;
    }
  };

  template<int numberOfBytesTemplate>
  class LengthWriterNBytesTemplated : public LengthWriterNBytes {
  public:
    LengthWriterNBytesTemplated(
      List<unsigned char>& output,
      List<Serialization::Marker>* markers,
      const std::string& label
    ): LengthWriterNBytes(numberOfBytesTemplate, output, markers, label){}
  };
  typedef LengthWriterNBytesTemplated<2> LengthWriterTwoBytes;
  typedef LengthWriterNBytesTemplated<3> LengthWriterThreeBytes;

  static bool readTwoByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool readThreeByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool readFourByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool readNByteInt(int numBytes,
    const List<unsigned char>& input,
    int& inputOutputOffset,
    int& result,
    std::stringstream* commentsOnFailure
  );
  static void writeBytesAnnotated(
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string& label
  );
  static bool readTwoByteLengthFollowedByBytesDontOutputOffset(
    const List<unsigned char>& input,
    int Offset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static bool readTwoByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    int& outputOffset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static bool readBytesFixedLength(
    const List<unsigned char>& input,
    int desiredNumberOfBytes,
    int& inputOutputOffset,
    List<unsigned char>& output,
    std::stringstream* commentsOnError
  );
  static bool readOneByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    int& outputOffset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static bool readNByteLengthFollowedByBytes(
    int numBytesLength,
    const List<unsigned char>& input,
    int& outputOffset,
    int* resultLength,
    List<unsigned char>* output,
    std::stringstream* commentsOnError
  );
  static void writeNByteUnsigned(// how many bytes are used to record the length
    int byteCountOfLength,
    unsigned int input,
    List<unsigned char>& output,
    int& inputOutputOffset
  );
  static void writeTwoByteInt(
    int input,
    List<unsigned char>& output
  );
  static void writeThreeByteInt(
    int input,
    List<unsigned char>& output
  );
  static void writeTwoByteUnsignedAnnotated(
    unsigned int input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string &label
  );
  static void writeTwoByteUnsigned(
    unsigned int input,
    List<unsigned char>& output
  );
  static void writeThreeByteUnsigned(
    unsigned int input,
    List<unsigned char>& output
  );
  static void writeFourByteUnsigned(
    unsigned int input,
    List<unsigned char>& output
  );
  static void writeOneByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string& label
  );
  static void writeTwoByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Marker>* annotations,
    const std::string &label
  );
  static void writeNByteLengthFollowedByBytes(// how many bytes are used to record the length
    int byteCountOfLength,
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string &label
  );
  static std::string convertListUnsignedCharsToHex(const List<unsigned char>& input);
  static std::string convertListUnsignedCharsToString(const List<unsigned char>& input);
};

#endif // SERIALIZATION_BASIC_ALREADY_DEFINED
