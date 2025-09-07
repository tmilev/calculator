#ifndef header_math_mathml_ALREADY_INCLUDED
#define header_math_mathml_ALREADY_INCLUDED

#include <sstream>

// Properties of a printed mathml expression.
struct MathMLExpressionProperties {
  bool startsWithMinus;
  bool isNegativeOne;
  bool isOne;
  MathMLExpressionProperties():
  startsWithMinus(false),
  isNegativeOne(false),
  isOne(false) {}
};

class MathML {
public:
  static std::string toMathMLFinal(
    const std::string& mathMLContent, const std::string& latex
  );
  static std::string toMathMLInteger(int integer);
  static std::string leftParenthesis;
  static std::string rightParenthesis;
  static std::string negativeSign;
  static std::string positiveSign;
  static std::string mtableDefault;
};

#endif // header_math_mathml_ALREADY_INCLUDED
