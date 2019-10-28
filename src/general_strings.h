//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef HEADER_GENERAL_STRINGS_ALREADY_INCLUDED
#define HEADER_GENERAL_STRINGS_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_memory_saving.h"

static ProjectInformationInstance headerGeneralStringsProjectInformation(__FILE__, "Header, string operations. ");

template <typename coefficient>
class Matrix;

class StringRoutines {
public:
  static bool StringBeginsWith(
    const std::string& theString, const std::string& desiredBeginning, std::string* outputStringEnd = nullptr
  );
  static bool StringEndsWith(
    const std::string& theString, const std::string& desiredEnd, std::string* outputStringBeginning = nullptr
  );
  static void StringSplitDefaultDelimiters(const std::string& inputString, List<std::string>& output);
  static void StringSplitExcludeDelimiter(const std::string& inputString, char delimiter, List<std::string>& output);
  static void StringSplitExcludeDelimiters(
    const std::string& inputString, const List<char>& delimiters, List<std::string>& output
  );
  static void StringTrimWhiteSpace(const std::string& inputString, std::string& output);
  static void StringTrimToLengthWithHash(std::string& inputOutput, int desiredLength50AtLeast = 150);
  static std::string StringTrimToLengthForDisplay(const std::string& input, int desiredLength20AtLeast = 50);
  static std::string ConvertStringToCalculatorDisplay(
    const std::string& input
  );
  static std::string ConvertStringToHexIfNonReadable(
    const std::string& input, int lineWidthZeroForNone, bool useHTML
  );
  static std::string ConvertByteToHex(unsigned char byte);

  static std::string ConvertStringForJSOn(const std::string& input);
  static std::string ConvertStringForJavascript(const std::string& input);
  static std::string StringTrimWhiteSpace(const std::string& inputString);
  static void SplitStringInTwo(const std::string& inputString, int firstStringSize, std::string& outputFirst, std::string& outputSecond);
  static std::string StringShortenInsertDots(const std::string& inputString, int maxNumChars);
  static bool isASCIICharacterVisible(char input);
  class Differ {
  public:
    MemorySaving<Matrix<int> > matrixLongestCommonSubsequence;
    std::string left;
    std::string right;
    int MaxMatrixSize;
    List<int> commonStringSizesReverseOrder;
    List<int> commonStringsLeftStartsReverseOrder;
    List<int> commonStringsRightStartsReverseOrder;
    List<std::string> leftResult;
    List<std::string> rightResult;
    int currentCommonStringLength;
    static std::string DifferenceHTMLStatic(
      const std::string& inputLeft,
      const std::string& inputRight
    );
    std::string DifferenceHTML();
    bool ComputeDifference(std::stringstream* commentsOnFailure);
    void ComputeLongestSubsequenceMatrix();
    void ComputeBestStartingIndices(int& outputIndexLeft, int& outputIndexRight);
    void ExtractCommonStrings(int indexLeft, int indexRight, int previousLeft, int previousRight);
    void ExtractDifferences();
    void ExtractResult(const List<int>& starts, const std::string& input, List<std::string>& output);
    void PushCommonString(int indexLeft, int indexRight);
    std::string ToString();
    Differ();
  };
};
#endif
