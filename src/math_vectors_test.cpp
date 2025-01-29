#include "math_vectors.h"
#include "test.h"

Vector<Rational> VectorTest::fromString(const std::string& input) {
  Vector<Rational> result;
  bool shouldBeTrue = result.fromString(input);
  if (!shouldBeTrue) {
    global.fatal
    << "Failed to extract vector from "
    << input
    << ". "
    << global.fatal;
  }
  return result;
}

bool VectorTest::all() {
  VectorTest::order();
  return true;
}

bool VectorTest::order() {
  List<Vector<Rational> > currentVectors = List<Vector<Rational> >({
      fromString("(1,2,3)"),
      fromString("(3,2,1)"),
      fromString("(1,2,1)"),
      fromString("(2,2,0)"),
      fromString("(2,0,2)")
    });
  currentVectors.quickSortAscending();
  std::string ascending = currentVectors.toString();
  currentVectors.quickSortDescending();
  std::string descending = currentVectors.toString();
  std::string expectedAscending =
  "(2, 2, 0)\n"
  "(1, 2, 1)\n"
  "(2, 0, 2)\n"
  "(3, 2, 1)\n"
  "(1, 2, 3)\n";
  std::string expectedDescending =
  "(1, 2, 3)\n"
  "(3, 2, 1)\n"
  "(2, 0, 2)\n"
  "(1, 2, 1)\n"
  "(2, 2, 0)\n";
  if (ascending != expectedAscending) {
    global.fatal
    << "Unespected result of soring:\n"
    << ascending
    << global.fatal;
  }
  if (descending != expectedDescending) {
    global.fatal
    << "Unespected result of descending soring:\n"
    << descending
    << global.fatal;
  }
  return true;
}
