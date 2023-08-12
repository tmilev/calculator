#include "calculator_interface.h"

bool Expression::Test::all() {
  STACK_TRACE("Expression::Test::all");
  Calculator tester;
  tester.initialize(Calculator::Mode::full);
  Expression::Test::toStringTestRecode(tester);
  Expression::Test::isUserDefinedAtom(tester);
  return true;
}

bool Expression::Test::toStringTestRecodeOnce(
  const std::string& inputHardCodedMustParse, Calculator& owner
) {
  STACK_TRACE("Expression::Test::toStringTestRecodeOnce");
  Expression parsed = owner.parseOrCrash(inputHardCodedMustParse, true);
  std::string recoded = parsed.toString();
  if (recoded != inputHardCodedMustParse) {
    global.fatal
    << "Recoded string: "
    << recoded
    << " does not coincide with the original: "
    << inputHardCodedMustParse
    << "."
    << global.fatal;
  }
  return true;
}

bool Expression::Test::toStringTestRecode(Calculator& owner) {
  STACK_TRACE("Expression::Test::toStringTestRecode");
  owner.initialize(Calculator::Mode::full);
  Expression::Test::toStringTestRecodeOnce("1+1", owner);
  Expression::Test::toStringTestRecodeOnce("\"\u00B0\"", owner);
  Expression::Test::toStringTestRecodeOnce("\u00B0", owner);
  Expression::Test::toStringTestRecodeOnce("\"\\\\\\\"\"", owner);
  return true;
}

bool Expression::Test::isUserDefinedAtomOnce(
  Calculator& owner, const std::string& input, bool isUserDefinedAtom
) {
  STACK_TRACE("Expression::Test::isUserDefinedAtomOnce");
  Expression expression = owner.parseOrCrash(input, true);
  if (expression.isAtomUserDefined() != isUserDefinedAtom) {
    global.fatal
    << "Expression::isAtomUserDefined did not return "
    << isUserDefinedAtom
    << " for input: "
    << expression.toString()
    << " with lispification: "
    << expression.toStringFull()
    << ", parsed from: "
    << input
    << global.fatal;
  }
  return true;
}

bool Expression::Test::isUserDefinedAtom(Calculator& owner) {
  STACK_TRACE("Expression::Test::isUserDefinedAtom");
  owner.initialize(Calculator::Mode::full);
  Expression::Test::isUserDefinedAtomOnce(owner, "x", true);
  Expression::Test::isUserDefinedAtomOnce(owner, "x+y", false);
  Expression::Test::isUserDefinedAtomOnce(owner, "x+1", false);
  Expression::Test::isUserDefinedAtomOnce(owner, "1", false);
  return true;
}
