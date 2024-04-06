#ifndef header_main_ALREADY_INCLUDED
#define header_main_ALREADY_INCLUDED

#include "general_lists.h"

class MainFunctions {
public:
  static int main(int argc, char** argv);
  static int mainWithoutExceptions(int argc, char** argv);
  static int mainConsoleHelp();
  static int mainCommandLine();
  static int mainFormat();
  static int mainLoadDatabase();
  static int mainTest(List<std::string>& programArguments);
  static int mainDeploy();
  static void analyzeMainArguments(int argC, char** argv);
  static bool analyzeMainArgumentsTimeString(
    const std::string& timeLimitString
  );
  static void initializeBuildFlags();
};

#endif // header_main_ALREADY_INCLUDED
