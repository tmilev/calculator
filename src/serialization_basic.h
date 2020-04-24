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
    friend std::ostream& operator << (std::ostream& output, const Serialization::Marker& theMarker) {
      output << theMarker.toString();
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
      List<Serialization::Marker>* theOutputMarkers,
      const std::string& label
    ) {
      this->init(inputNumberOfBytes, static_cast<unsigned int>(input), output, theOutputMarkers, label);
    }
    WriterIntegerWithMarker(
      int inputNumberOfBytes,
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* theOutputMarkers,
      const std::string& label
    ) {
      this->init(inputNumberOfBytes, input, output, theOutputMarkers, label);
    }
    inline void init(
      int inputNumberOfBytes,
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* theOutputMarkers,
      const std::string& label
    ) {
      this->numberOfBytes  = inputNumberOfBytes;
      this->flagDeallocated = false;
      this->outputMarkers = theOutputMarkers;
      this->outputPointer = nullptr;
      int notUsed = output.size;
      Serialization::WriteNByteUnsigned(this->numberOfBytes, input, output, notUsed);
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
      List<Serialization::Marker>* theOutputMarkers,
      const std::string& label
    ): WriterIntegerWithMarker(numberOfBytesTemplate, input, output, theOutputMarkers, label) {}
    WriterIntegerWithMarkerTemplate(
      unsigned int input,
      List<unsigned char>& output,
      List<Serialization::Marker>* theOutputMarkers,
      const std::string& label
    ): WriterIntegerWithMarker(numberOfBytes, input, output, theOutputMarkers, label) {}
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
      int theSize = output.size;
      Serialization::WriteNByteUnsigned(numberOfBytes, 0, output, theSize);
      if (this->outputMarkers != nullptr) {
        this->markerOffset = this->outputMarkers->size;
        this->outputMarkers->addOnTop(Serialization::Marker(this->offset, - 1, Serialization::JSLabels::length));
        this->outputMarkers->addOnTop(Serialization::Marker(this->offset, numberOfBytes, label));
      }
    }
    ~LengthWriterNBytes() {
      int totalLength = this->outputPointer->size - this->offset - numberOfBytes;
      Serialization::WriteNByteUnsigned(
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

  static bool ReadTwoByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool ReadThreeByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool ReadFourByteInt(
    const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
  );
  static bool ReadNByteInt(int numBytes,
    const List<unsigned char>& input,
    int& inputOutputOffset,
    int& result,
    std::stringstream* commentsOnFailure
  );
  static void WriteBytesAnnotated(
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string& label
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
  static bool ReadBytesFixedLength(
    const List<unsigned char>& input,
    int desiredNumberOfBytes,
    int& inputOutputOffset,
    List<unsigned char>& output,
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
  static void WriteNByteUnsigned(// how many bytes are used to record the length
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
  static void WriteTwoByteUnsignedAnnotated(
    unsigned int input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string &label
  );
  static void WriteTwoByteUnsigned(
    unsigned int input,
    List<unsigned char>& output
  );
  static void WriteThreeByteUnsigned(
    unsigned int input,
    List<unsigned char>& output
  );
  static void WriteFourByteUnsigned(
    unsigned int input,
    List<unsigned char>& output
  );
  static void WriteOneByteLengthFollowedByBytes(
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string& label
  );
  static void WriteTwoByteLengthFollowedByBytes(const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Marker>* annotations,
    const std::string &label
  );
  static void WriteNByteLengthFollowedByBytes(// how many bytes are used to record the length
    int byteCountOfLength,
    const List<unsigned char>& input,
    List<unsigned char>& output,
    List<Serialization::Marker>* annotations,
    const std::string &label
  );
  static std::string convertListUnsignedCharsToHex(const List<unsigned char>& input);
  static std::string ConvertListUnsignedCharsToString(const List<unsigned char>& input);
};

#endif // SERIALIZATION_BASIC_ALREADY_DEFINED
