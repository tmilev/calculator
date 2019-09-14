//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef HEADER_GENERAL_STRINGS_ALREADY_INCLUDED
#define HEADER_GENERAL_STRINGS_ALREADY_INCLUDED

#include "general_lists.h"

static ProjectInformationInstance headerGeneralStringsProjectInformation(__FILE__, "Header, string operations. ");

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
  static std::string ConvertStringToHexPrependConversionIfNeeded(const std::string& input);

  static std::string StringTrimWhiteSpace(const std::string& inputString);
  static void SplitStringInTwo(const std::string& inputString, int firstStringSize, std::string& outputFirst, std::string& outputSecond);
  static std::string StringShortenInsertDots(const std::string& inputString, int maxNumChars);

  static bool Difference(
    const std::string& left,
    const std::string& right,
    List<std::string>& outputLeft,
    List<std::string>& outputRight,
    std::stringstream* commentsOnFailure
  );
};
#endif
