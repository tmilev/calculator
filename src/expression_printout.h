#ifndef header_expression_printer_ALREADY_INCLUDED
#define header_expression_printer_ALREADY_INCLUDED

#include "general_list_references.h"
#include "general_lists.h"
#include "math_mathml.h"

// A class that holds a math printout, with all properties computed.
// The class is ready to be converted to string.
class ExpressionPrintoutNode {
public:
  // If the content is non-empty,
  // the expression is expected to
  // have no children.
  std::string content;
  //
  List<int> children;
  MathExpressionFormattingProperties properties;
};

// A class for non-recursively printing possibly deeply nested
// math expressions in linear time.
class ExpressionPrintout {
public:
  ListReferences<ExpressionPrintoutNode> allPrintouts;
  int indexRootNode;
  bool useMathML;
  void clear();
  int addString(const std::string& input);
  void appendStringTo(
    const std::string& input, ExpressionPrintoutNode& appendTo
  );
  std::string toStringRootNode();
  ExpressionPrintout(): indexRootNode(- 1), useMathML(false) {}
};

#endif // header_expression_printer_ALREADY_INCLUDED
