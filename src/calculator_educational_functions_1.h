// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef CALCULATOR_EDUCATIONAL_FUNCTIONS
#define CALCULATOR_EDUCATIONAL_FUNCTIONS
#include "calculator_interface.h"

class ProblemWithSolution {
public:
  Expression toBeSolved;
  std::string solution;

  JSData toJSON();
};

class CalculatorEducationalFunctions{
public:
  static bool solveJSON(
    Calculator& calculator, const Expression& input, Expression& output
  );

};

#endif //CALCULATOR_EDUCATIONAL_FUNCTIONS
