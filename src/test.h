#ifndef header_test_ALREADY_INCLUDED
#define header_test_ALREADY_INCLUDED

#include "general_lists.h"

class Test {
public:
  class Suites {
  public:
    static const std::string all;
    static const std::string API;
    static const std::string database;
    static const std::string problems;
    static const std::string build;
    static const std::string wasm;
    static const std::string crypto;
    static const std::string topicLists;
    static const std::string topiclists;
    static const std::string freecalc;
    static const std::string calculator;
    static const std::string json;
    static const std::string polynomial;
    static const std::string basic;
    static const std::string courses;
  };

  static std::string update;
  HashedList<std::string> inputs;
  bool flagTestAll;
  // If this is set, AB tests will not be compared to the known results,
  // but instead the known results file will be updated.
  // Recall that an AB tests ("golden test") is running a bunch of input
  // and comparing the output to previously recorded outputs.
  // Use this flag to ignore previously recorded behavior and instead reset
  // the expected outputs.
  // Use with appropriate caution to avoid
  // accidentally record undesired bad behavior.
  static bool flagUpdateABTests;
  void initialize(List<std::string>& inputArguments);
  void run();
  bool shouldTest(const std::string& testSuite);
  Test();
};

class VectorTest {
public:
  static bool all();
  static bool order();
  static Vector<Rational> fromString(const std::string& input);
};

#endif
