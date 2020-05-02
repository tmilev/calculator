// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef HEADER_GENERAL_STRINGS_ALREADY_INCLUDED
#define HEADER_GENERAL_STRINGS_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_memory_saving.h"

template <typename Coefficient>
class Matrix;

class StringRoutines {
public:
  static bool stringBeginsWith(
    const std::string& theString,
    const std::string& desiredBeginning,
    std::string* outputStringEnd = nullptr
  );
  static bool stringEndsWith(
    const std::string& theString,
    const std::string& desiredEnd,
    std::string* outputStringBeginning = nullptr
  );
  static void stringSplitDefaultDelimiters(
    const std::string& inputString,
    List<std::string>& output
  );
  static void stringSplitExcludeDelimiter(
    const std::string& inputString,
    char delimiter,
    List<std::string>& output
  );
  static void stringSplitExcludeDelimiters(
    const std::string& inputString,
    const List<char>& delimiters,
    List<std::string>& output
  );
  static void stringTrimWhiteSpace(const std::string& inputString, std::string& output);
  static void stringTrimToLengthWithHash(std::string& inputOutput, int desiredLength50AtLeast = 150);
  static std::string stringTrimToLengthForDisplay(const std::string& input, int desiredLength20AtLeast = 50);
  static std::string convertStringToCalculatorDisplay(
    const std::string& input
  );
  static std::string convertStringToHexIfNonReadable(
    const std::string& input, int lineWidthZeroForNone, bool useHTML
  );
  static std::string convertByteToHex(unsigned char byte);

  static std::string convertStringToJSONString(const std::string& input);
  static std::string convertStringToJavascriptString(const std::string& input);
  static std::string convertStringToJavascriptVariable(const std::string& input);
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
  static std::string stringShortenInsertDots(const std::string& inputString, int maxNumChars);
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
      const std::string& labelLeft,
      const std::string& labelRight
    );
    bool computeDifference(std::stringstream* commentsOnFailure);
    void computeLongestSubsequenceMatrix();
    void computeBestStartingIndices(int& outputIndexLeft, int& outputIndexRight);
    void extractCommonStrings(int indexLeft, int indexRight);
    void extractDifferences();
    void extractResult(const List<int>& starts, const std::string& input, List<std::string>& output);
    void pushCommonString(int indexLeft, int indexRight);
    std::string toString();
    Differ();
  };
};
#endif
