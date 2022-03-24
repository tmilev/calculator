 // The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_general_strings_ALREADY_INCLUDED
#define header_general_strings_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_memory_saving.h"

template <typename Coefficient>
class Matrix;

class StringRoutines {
public:
  static bool stringBeginsWith(
    const std::string& input,
    const std::string& desiredBeginning,
    std::string* outputStringEnd = nullptr
  );
  static bool stringEndsWith(
    const std::string& input,
    const std::string& desiredEnd,
    std::string* outputStringBeginning = nullptr
  );
  static void stringSplitDefaultDelimiters(
    const std::string& inputString, List<std::string>& output
  );
  static void splitExcludeDelimiter(
    const std::string& inputString,
    char delimiter,
    List<std::string>& output
  );
  // Splits a string using a set of character delimiters.
  static void stringSplitExcludeDelimiters(
    const std::string& inputString,
    const List<char>& delimiters,
    List<std::string>& output
  );
  static bool stringContains(
    const std::string& container, const std::string& content
  );
  static void stringTrimWhiteSpace(
    const std::string& inputString, std::string& output
  );
  static void stringTrimToLengthWithHash(
    std::string& inputOutput, int desiredLength50AtLeast = 150
  );
  static std::string stringTrimToLengthForDisplay(
    const std::string& input, int desiredLength20AtLeast = 50
  );
  static std::string convertStringToHexIfNonReadable(
    const std::string& input, int lineWidthZeroForNone, bool useHTML
  );
  static std::string convertByteToHex(unsigned char byte);
  static bool isLatinLetterSequence(const std::string& input);
  static bool isLatinLetterOrDigitSequence(const std::string& input);
  class Conversions {
  public:
    // Converts an arbitrary string (byte sequence) to a list of unicode code
    // points that can be recoded back to another format.
    // If the input is valid utf8 string, will write
    // the corresponding code points in the output variable and return true;
    // If the input is invalid, will still read the entire sequence and
    // write a list of integers, but will return false. In this case, the
    // list of integers is not guaranteed to be valid code points.
    static bool utf8StringToUnicodeCodePoints(
      const std::string& input,
      List<uint32_t>& output,
      std::stringstream* commentsOnFailure
    );
    static uint32_t codePointFromUtf8(
      const std::string& input,
      unsigned& indexIncrementedToLastConsumedByte,
      bool& isValid,
      std::stringstream* commentsOnFailure
    );
    // Converts an arbitrary byte sequence to a format suitable for displaying
    // in a web-browser without additional encoding and that can be
    // pasted back in the calculator.
    static std::string stringToCalculatorDisplay(const std::string& input);
    static bool isValidUtf8(const std::string& input);
    // Escapes quotes, backslashes and new lines. Does not do utf16 decoding.
    static std::string escapeQuotesBackslashesNewLines(
      const std::string& input
    );
    // Converts an arbitrary byte sequence to a string suited for json
    // notation:
    // escapes quote, backslash, newline, and uses \u-notation to encode
    // utf-16-like two-byte sequences.
    static std::string stringToJSONStringEscaped(
      const std::string& inputUtf8
    );
    static std::string codePointToBackslashEscapedString(uint32_t input);
    static std::string codePointToUtf8(uint32_t input);
    // Similar to utf8StringToJSONStringEscaped but uses
    // \x-notation to encode arbitrary byte sequences.
    // The "like" part in the name stands for the fact that we do
    // not guarantee implementation up to javascript spec.
    static std::string escapeJavascriptLike(const std::string& input);
    // Unescapes \n, \\, \", as well as \x- and \u- encoded sequences.
    // The "like" part in the name stands for the fact that we do
    // not guarantee implementation up to javascript spec.
    static std::string unescapeJavascriptLike(const std::string& input);
    // Transforms an arbitrary string to a string that is an acceptable
    // javascript variable.
    // Avoid use of underscores and other characters so as attempt to produce a
    // name that is in addition a valid latex string and a valid html tag.
    class Test {
    public:
      static bool all();
      static bool utf8StringToJSONStringEscaped();
      static bool unescapeJavascriptLike();
      static bool convertStringToJSONStringEscapeOnly();
      static bool codePointToUtf8();
      static bool oneCodePointToUtf8(
        uint32_t codePoint, const std::string& expectedHex
      );
      static bool convertUtf8StringToUnicodeCodePoints();
      static bool oneUtf8ToJSONSuccess(
        const std::string& givenInput, const std::string& expectedOutput
      );
      static bool oneConversionUtf8Success(
        const std::string& givenInput,
        uint32_t codePoint1,
        uint32_t codePoint2 = 0xffffffff,
        uint32_t codePoint3 = 0xffffffff
      );
    };
  };

  static std::string convertStringToJavascriptVariable(
    const std::string& input
  );
  static std::string stringTrimWhiteSpace(const std::string& inputString);
  static void splitStringInTwo(
    const std::string& inputString,
    int firstStringSize,
    std::string& outputFirst,
    std::string& outputSecond
  );
  static std::string replaceAll(
    const std::string& input,
    const std::string& subStringToReplace,
    const std::string& replaceWith
  );
  // Returns true if an occurrence of the string was found, false otherwise.
  static bool replaceOnce(
    std::string& inputOutput,
    const std::string& subStringToReplace,
    const std::string& replaceWith
  );
  static std::string stringShortenInsertDots(
    const std::string& inputString, int maxNumChars
  );
  static bool isASCIICharacterVisible(char input);
  class Differ {
  private:
    std::string differenceHTMLPartTwo(
      const std::string& preamble,
      const std::string& labelLeft,
      const std::string& labelRight,
      const std::string& outputLeft,
      const std::string& outputRight
    );
  public:
    MemorySaving<Matrix<int> > matrixLongestCommonSubsequence;
    std::string left;
    std::string right;
    int maximumMatrixSize;
    List<int> commonStringSizesReverseOrder;
    List<int> commonStringsLeftStartsReverseOrder;
    List<int> commonStringsRightStartsReverseOrder;
    List<std::string> leftResult;
    List<std::string> rightResult;
    int currentCommonStringLength;
    static std::string differenceHTMLStatic(
      const std::string& inputLeft,
      const std::string& inputRight,
      const std::string& labelLeft,
      const std::string& labelRight
    );
    std::string differenceHTML(
      const std::string& labelLeft, const std::string& labelRight
    );
    bool computeDifference(std::stringstream* commentsOnFailure);
    void computeLongestSubsequenceMatrix();
    void computeBestStartingIndices(
      int& outputIndexLeft, int& outputIndexRight
    );
    void extractCommonStrings(int indexLeft, int indexRight);
    void extractDifferences();
    void extractResult(
      const List<int>& starts,
      const std::string& input,
      List<std::string>& output
    );
    void pushCommonString(int indexLeft, int indexRight);
    std::string toString();
    Differ();
  };

  template <typename CoefficientType>
  static std::string toStringElement(CoefficientType x) {
    std::stringstream result;
    result << x;
    return result.str();
  }
  class Test {
  public:
    static bool all();
  };
};

#endif
