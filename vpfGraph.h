#include "vpf.h"

// a bit of graph theory
// a graph is stored as a continuous block, indexed as *(data + v*max_edges), after which
// point is up to max_edges vertices.  If the vertices aren't a dense collection of integers,
// there will need to be a hashmap somewhere to get a dense collection of integers

class Graph
{
public:
    Graph(int max_vertices, int max_edges);
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
