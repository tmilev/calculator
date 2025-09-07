#ifndef header_math_mathml_ALREADY_INCLUDED
#define header_math_mathml_ALREADY_INCLUDED

#include <sstream>

class MathML {
public:
  static std::string toMathML(
    const std::string& mathMLContent, const std::string& latex
  );
  static std::string leftParenthesis;
  static std::string rightParenthesis;
};

#endif // header_math_mathml_ALREADY_INCLUDED
