// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "web_api.h"
#include "json.h"
#include "general_logging_global_variables.h"
#include "string_constants.h"
#include "calculator.h"

bool WebAPIResponse::Test::all() {
  WebAPIResponse::Test::solveJSON();
  return true;
}

bool WebAPIResponse::Test::solveJSON() {
  WebAPIResponse response;
  global.calculator().freeMemory();
  global.calculator().getElement().initialize();
  global.setWebInput(WebAPI::request::calculatorInput, "1+1");
  JSData result = response.solveJSON();
  std::string expected =
  "{\"solution\":{\"solution\":{\"input\":\"1+1\",\"solutionSteps\":"
  "\"\\\\(\\\\begin{array}{ll|l}&\\n1+1&AddRationals\\\\\\\\=&\\n2\\\\end{array}\\\\)\","
  "\"finalExpression\":\"2\",\"error\":\"\"}},"
  "\"commentsGlobal\":\"\",\"error\":\"not implemented yet.\"}";
  std::string got = result.toString();

  if (got != expected) {
    global.fatal << "Got solution:\n" << got
    << "\nExpected solution:\n" << expected << "\nWhat I got, escaped:\n"
    << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(got)
    << global.fatal;
  }
  return true;
}
