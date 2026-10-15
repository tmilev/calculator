#ifndef header_expression_printer_ALREADY_INCLUDED
#define header_expression_printer_ALREADY_INCLUDED

#include "general_list_references.h"
#include "general_lists.h"
#include "math_mathml.h"

class ExpressionPrintout;

// A class that holds a math printout, with all properties computed.
// The class is ready to be converted to string.
class ExpressionPrintoutNode {
private:
  friend class List<ExpressionPrintoutNode>;
  friend class ListReferences<ExpressionPrintoutNode>;
  friend class ExpressionPrintout;

  ExpressionPrintoutNode(): owner(nullptr){}
  // If the content is non-empty,
  // the expression is expected to
  // have no children.
  std::string content;
  // The indices of the children elements in the owner collection.
  List<int> children;
  // The owner collection.
public:
  ExpressionPrintout* owner;
  MathExpressionFormattingProperties properties;
  List<int> childrenExpressionIndices;
  int stringSize;
  ExpressionPrintoutNode(ExpressionPrintout& inputOwner): owner(&inputOwner){}
  ExpressionPrintoutNode& addChildIndex(int index) ;
  ExpressionPrintoutNode& addString(const std::string& input) ;
  ExpressionPrintout& getOwner();
  bool checkConsistency()const;
  ExpressionPrintoutNode& getChild(int childIndex);
  bool isAtomic()const;
 void clear();
  inline void setContent(const std::string& inputContent){
    this->clear();
    this->content = inputContent;
  }
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
