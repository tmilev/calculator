// The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_calculator_educational_functions_ALREADY_INCLUDED
#define header_calculator_educational_functions_ALREADY_INCLUDED

#include "calculator_interface.h"

class UnivariateEquation {
public:
  Expression variable;
  Expression equationAllTermsOnLeftHandSide;
  Expression simplified;
  List<Expression> solutions;
  bool solve(Calculator& calculator);
  // Returns a list of solutions.
  // For the equation (x-1)(x-2)=0,
  // the solutions would be the list (1,2).
  bool getSolutions(Calculator& calculator, Expression& output);
  // Returns a mathematically well-formed list of solutions.
  // For the equation (x-1)(x-2)=0,
  // this returns the expression (x=1,x=2).
  bool getSolutionEquations(Calculator& calculator, Expression& output);
  bool getOneSolutionEquation(
    int solutionIndex, Calculator& calculator, Expression& output
  );
};

class ProblemWithSolution {
public:
  Expression toBeSolved;
  std::string simplification;
  std::string factorization;
  Expression solutionsNonSimplified;
  Expression solutionsSimplified;
  List<Calculator::ExpressionHistoryEnumerator::Step> steps;
  std::string error;
  std::string comments;
  void addAnnotationStep(Calculator& calculator, const std::string& input);
  bool solve(Calculator& calculator);
  bool solveOther(Calculator& calculator);
  bool solveEquation(Calculator& calculator);
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

class CalculatorEducationalFunctions {
public:
  static bool solveJSON(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareExpressionsJSON(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareExpressionsJSONInternal(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    CompareExpressions& comparison
  );
  static bool divideByNumberTrivial(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif //header_calculator_educational_functions_ALREADY_INCLUDED

