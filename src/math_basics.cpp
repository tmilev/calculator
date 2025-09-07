#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_basics.h"
#include "math_large_integers.h"

std::string MathRoutines::toMathML(
  const std::string& mathMLContent, const std::string& latex
) {
  std::stringstream out;
  out
  << "<span class='mathcalculatorbackendrendered'>"
  << "<math>"
  << mathMLContent
  << "<semantics>"
  << "<annotation>"
  << latex
  << "</annotation"
  << "</semantics></math>"
  << "</span>";
  return out.str();
}

void MathRoutines::parseListIntegersNoFailure(
  const std::string& input, List<int>& result
) {
  bool success = MathRoutines::parseListIntegers(input, result, nullptr);
  if (!success) {
    global.fatal
    << "Failed to parse list int with a function that does not allow failure. "
    << global.fatal;
  }
}

bool MathRoutines::parseListIntegers(
  const std::string& input,
  List<int>& result,
  std::stringstream* commentsOnFailure
) {
  List<char> delimiters;
  delimiters.addOnTopNoRepetition('\n');
  delimiters.addOnTopNoRepetition(',');
  delimiters.addOnTopNoRepetition('[');
  delimiters.addOnTopNoRepetition(']');
  delimiters.addOnTopNoRepetition('(');
  delimiters.addOnTopNoRepetition(')');
  List<std::string> numberStrings;
  StringRoutines::splitExcludeDelimiters(input, delimiters, numberStrings);
  result.setSize(numberStrings.size);
  for (int i = 0; i < numberStrings.size; i ++) {
    LargeInteger integerValue;
    bool success =
    integerValue.assignStringFailureAllowed(
      numberStrings[i], commentsOnFailure
    );
    if (!success) {
      return false;
    }
    if (!integerValue.isIntegerFittingInInt(&result[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Integer at position " << i << " is too large. ";
      }
      result.setSize(0);
      return false;
    }
  }
  return true;
}
