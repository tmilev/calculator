#ifndef vpfGraphHeaderAlreadyDefined
#define vpfGraphHeaderAlreadyDefined
#include "math_general.h"

class GraphOLD {
  // This type of graph is stored as a continuous block,
  // indexed as *(data + v * max_edges), after which
  // point is up to max_edges vertices.
  // If the vertices aren't a dense collection of integers,
  // there will need to be a hashmap
  // somewhere to get a dense collection of integers
public:
  GraphOLD(int max_vertices, int max_edges);
  int vertices;
  int edges;
  int max_edges;
  List<int> data;
  void addEdge(int v1, int v2);
  void addDiEdge(int v1, int v2);
  List<List<int> > destructivelyGetConnectedComponents();
private:
  void treeRecurseCopyDelete(List<int>& l, int v, int m);
};

class GraphEdge {
public:
  friend std::ostream& operator<<(std::ostream& output, const GraphEdge& edge) {
    output << edge.toString();
    return output;
  }
  int vStart;
  int vEnd;
  std::string label;
  GraphEdge() : vStart(- 1), vEnd(- 1) {}
  GraphEdge(int inputStart, int inputEnd):vStart(inputStart), vEnd(inputEnd) {}
  GraphEdge(int inputStart, int inputEnd, const std::string& inputLabel)
  : vStart(inputStart), vEnd(inputEnd), label(inputLabel) {}
  std::string toString(FormatExpressions* format = nullptr) const;
  static unsigned int hashFunction(const GraphEdge& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return static_cast<unsigned>(this->vStart) + static_cast<unsigned>(this->vEnd) * HashConstants::constant0;
  }
  bool operator==(const GraphEdge& other) const {
    return
    this->vStart == other.vStart &&
    this->vEnd == other.vEnd &&
    this->label == other.label;
  }
};

// An oriented graph represented as a collection
// of nodes and edges.
// Edges and nodes are stored independently.
class GraphWeightedLabeledEdges {
public:
  int numberOfNodes;
  int groupMaxSize;
  List<std::string> nodeLabels;
  LinearCombination<GraphEdge, Rational> edges;
  List<List<int> > edgesPerNodeNoMultiplicities;
  List<List<int> > nodeGroupsForDisplay;
  List<List<List<int> > > connectedComponents;
  List<int> distanceToBaseNode;
  List<int> baseNode;
  List<int> positionInDisplayGroup;
  List<int> displayGroupIndices;
  List<int> connectedComponentSizes;
  GraphWeightedLabeledEdges(): numberOfNodes(- 1), groupMaxSize(- 1){}
  bool checkConsistency() const;
  void addEdge(int i, int j);
  void computeEdgesPerNodesNoMultiplicities();
  double getXNode(int groupIndex, int indexInGroup);
  double getYNode(int groupIndex, int indexInGroup);
  std::string getNodePSStrings(int groupIndex, int indexInGroup);
  void computeDisplayGroups();
  void computeConnectedComponentsAndBaseNodeDistances();
  void addNodeToComponent(int nodeIndex);
  void addEdge(int i, int j, const std::string& inputLabel);
  std::string toStringPsTricks(FormatExpressions* format);
  std::string toStringNodesAndEdges(FormatExpressions* format);
  std::string toStringPsTricksEdge(int fromIndex, int toIndex, FormatExpressions* format = nullptr);
};

#endif
