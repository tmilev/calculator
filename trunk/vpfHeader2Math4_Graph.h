#ifndef vpfGraphHeaderAlreadyDefined
#define vpfGraphHeaderAlreadyDefined
#include "vpf.h"
static ProjectInformationInstance ProjectInfoVpfGraphsHeader
(__FILE__, "Header file, graphs (Thomas).");

// a bit of graph theory
// a graph is stored as a continuous block, indexed as *(data + v*max_edges), after which
// point is up to max_edges vertices.  If the vertices aren't a dense collection of integers,
// there will need to be a hashmap somewhere to get a dense collection of integers

class GraphOLD
{
public:
    GraphOLD(int max_vertices, int max_edges);
    int vertices;
    int edges;
    int max_edges;
    List<int> data;
    void AddEdge(int v1, int v2);
    void AddDiEdge(int v1, int v2);
    List<List<int> > DestructivelyGetConnectedComponents();
private:
    void TreeRecurseCopyDelete(List<int> &l, int v, int m);
};

struct GraphEdge
{
  friend std::ostream& operator << (std::ostream& output, const GraphEdge& theEdge)
  { output << theEdge.ToString();
    return output;
  }
  int vStart;
  int vEnd;
  std::string label;
  GraphEdge():vStart(-1), vEnd(-1) {}
  GraphEdge(int inputStart, int inputEnd):vStart(inputStart), vEnd(inputEnd) {}
  GraphEdge(int inputStart, int inputEnd, const std::string& inputLabel)
  :vStart(inputStart), vEnd(inputEnd), label(inputLabel) {}
  std::string ToString(FormatExpressions* theFormat=0)const;
  static unsigned int HashFunction(const GraphEdge& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return vStart+vEnd*SomeRandomPrimes[0];
  }
  inline static bool IsMonEqualToZero()
  { return false;
  }
  bool operator==(const GraphEdge& other)const
  { return this->vStart==other.vStart && this->vEnd==other.vEnd
    && this->label==other.label;
  }
};

class Graph
{
public:
  int numNodes;
  List<std::string> nodeLabels;
  MonomialCollection<GraphEdge, Rational> theEdges;
  List<List<int> > edgesPerNodeNoMultiplicities;
  List<List< int> > nodeGroupsForDisplay;
  List<int> distanceToFirstNode;
  Graph(): numNodes(-1){}
  void AddEdge(int i, int j);
  void ComputeEdgesPerNodesNoMultiplicities();
  void ComputeDistanceToFirstNode();
  void ComputeNodeGroupsForDisplayAccordingToDistanceFromFirstNode();
  void AddEdge(int i, int j, const std::string& inputLabel);
  std::string ToStringLatex(FormatExpressions* theFormat);
};

#endif
