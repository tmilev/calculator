// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef CALCULATOR_EDUCATIONAL_FUNCTIONS
#define CALCULATOR_EDUCATIONAL_FUNCTIONS
#include "calculator_interface.h"

class ProblemWithSolution {
public:
  Expression toBeSolved;
  std::string simplification;
  std::string factorization;

  std::string solution;
  Expression finalExpression;
  std::string error;
  JSData toJSON();
};

class CompareExpressions {
private:
  void comparePartTwo(Calculator& calculator);
public:
  std::string givenString;
  std::string desiredString;
  std::string syntaxErrorsLeftRaw;
  std::string syntaxErrorsLeftFormatted;
  std::string syntaxErrorsRightRaw;
  std::string syntaxErrorsRightFormatted;
  std::string errorInAnswer;
  std::string errorEvaluation;
  bool flagHideDesiredAnswer;
  bool flagAreEqual;
  bool flagAreEqualAsAnswers;
  Expression given;
  Expression desired;
  Expression givenSimplified;
  Expression desiredSimplified;
  Expression comparisonExpression;
  Expression comparisonExpressionEvaluated;
  Expression comparisonStandardRaw;
  Expression comparisonStandardEvaluated;
  Expression comparisonNoDistributionRaw;
  Expression comparisonNoDistributionEvaluated;
  HashedList<Expression> freeVariablesDesired;
  HashedList<Expression> freeVariablesFound;
  List<Expression> unexpectedVariables;
  List<Expression> missingVariables;
  CompareExpressions(bool hideDesiredAnswer);
  JSData toJSON() const;
  void compare(
    const std::string& givenInput,
    const std::string& desiredInput,
    Calculator& calculator
  );
  void processComparisonRestricted();
};

class CalculatorEducationalFunctions{
public:
  static bool solveJSON(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareExpressionsJSON(
    Calculator& calculator,
    const Expression& input,
    Expression& output
  );
  static bool compareExpressionsJSONInternal(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    CompareExpressions& comparison
  );
  static bool divideByNumberTrivial(Calculator& calculator, const Expression& input, Expression& output);
};

#endif //CALCULATOR_EDUCATIONAL_FUNCTIONS
