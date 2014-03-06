#include "vpfHeader2Math4_Graph.h"
#include "vpfHeader2Math0_General.h"
static ProjectInformationInstance ProjectInfoVpfGraphs(__FILE__, "A graph class.");

GraphOLD::GraphOLD(int max_vertices, int max_edges){
    this->vertices = max_vertices;
    this->edges = 0;
    this->max_edges = max_edges;
    this->data = List<int>(max_vertices*max_edges,-1);
}

void GraphOLD::AddEdge(int v1, int v2){
    this->AddDiEdge(v1,v2);
    this->AddDiEdge(v2,v1);
}
        // assuming that -1 only happens after all the edges is okay because
        // the DeleteEdge method packs the vertices when it finishes
void GraphOLD::AddDiEdge(int v1, int v2){
    for(int i=0;i<this->max_edges;i++){
        if(this->data[v1*this->max_edges+i] == v2)
            return;
        if(this->data[v1*this->max_edges+i] == -1){
            this->data[v1*this->max_edges+i] = v2;
            this->edges++;
            return;
        }
    }
    std::cout << "attempting to add an edge beyond max_edges" << std::endl;
}

// methods that can only be called once can have long names
List<List<int> > GraphOLD::DestructivelyGetConnectedComponents(){
    List<List<int> > outerl;
    for(int i=0; i<this->vertices*this->max_edges; i++){
        if(this->data[i]!= -1){
            List<int> l;
            l.AddOnTop(i/this->max_edges);
            this->TreeRecurseCopyDelete(l, i/this->max_edges, this->max_edges);
            outerl.AddOnTop(l);
        }
    }
    for(int i=0; i<outerl.size; i++)
        outerl[i].QuickSortAscending();
    return outerl;
}

void GraphOLD::TreeRecurseCopyDelete(List<int> &l, int v, int m){
    for(int i=0; i<m; i++)
        if(this->data[v*m+i] != -1){
            int w = this->data[v*m+i];
            if(!l.Contains(w))
                l.AddOnTop(w);
            this->data[v*m+i] = -1;
            this->TreeRecurseCopyDelete(l,w,m);
        }
}

std::string GraphEdge::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << this->vStart << "->" << this->vEnd;
  if (this->label!="")
    out << "(" << this->label << ")";
  return out.str();
}

void Graph::AddEdge(int i, int j)
{ GraphEdge theEdge(i,j);
  this->theEdges.AddMonomial(theEdge, 1);
}

void Graph::AddEdge(int i, int j, const std::string& inputLabel)
{ GraphEdge theEdge(i,j, inputLabel);
  this->theEdges.AddMonomial(theEdge, 1);
}

std::string Graph::ToStringLatex(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("Graph::ToStringLatex");
  std::stringstream out;
  out << "The graph has " << this->theEdges.size() << " edges. ";
  return out.str();
}
