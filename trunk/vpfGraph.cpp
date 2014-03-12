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
//  std::cout << " adding edge: " << theEdge.ToString();
  this->theEdges.AddMonomial(theEdge, 1);
}

bool Graph::CheckConsistency()const
{ for (int i=0; i<this->theEdges.size(); i++)
  { if (this->theEdges[i].vStart<0 || this->theEdges[i].vStart>=this->numNodes ||
        this->theEdges[i].vEnd  <0 || this->theEdges[i].vEnd>=this->numNodes)
      crash << "Graph error in graph with " << this->numNodes << " nodes: detected a corrupt edge: "
      << this->theEdges[i].ToString() << " is a bad." << crash;
  }
  for (int i=0; i<this->nodeGroupsForDisplay.size; i++)
    for (int j=0; j<this->nodeGroupsForDisplay[i].size; j++)
      if (this->nodeGroupsForDisplay[i][j]<0 || this->nodeGroupsForDisplay[i][j]>=numNodes)
        crash << "Graph error in graph with " << this->numNodes << " nodes: display group " << i+1 << " is corrupt: has "
        << " node of index " << this->nodeGroupsForDisplay[i][j] << crash;
  return true;
}

void Graph::ComputeEdgesPerNodesNoMultiplicities()
{ MacroRegisterFunctionWithName("Graph::ComputeEdgesPerNodesNoMultiplicities");
  List<int> emptyList;
  this->edgesPerNodeNoMultiplicities.initFillInObject(this->numNodes, emptyList);
  this->CheckConsistency();
  for (int i=0; i<this->theEdges.size(); i++)
    this->edgesPerNodeNoMultiplicities[this->theEdges[i].vStart].AddOnTop(this->theEdges[i].vEnd);
}

void Graph::ComputeDistanceToFirstNode()
{ MacroRegisterFunctionWithName("Graph::ComputeDistanceToFirstNode");
  this->distanceToFirstNode.initFillInObject(this->numNodes, -1);
  List<int> theOrbit;
  theOrbit.SetExpectedSize(this->theEdges.size());
  theOrbit.AddOnTop(0);
  this->distanceToFirstNode[0]=0;
  for (int i=0; i<theOrbit.size; i++)
  { List<int>& currentHeirs=this->edgesPerNodeNoMultiplicities[theOrbit[i]];
    for (int j=0; j<currentHeirs.size; j++)
      if (this->distanceToFirstNode[currentHeirs[j]]==-1)
      { this->distanceToFirstNode[currentHeirs[j]]=this->distanceToFirstNode[i]+1;
        theOrbit.AddOnTop(currentHeirs[j]);
      }
  }
}

void Graph::ComputeNodeGroupsForDisplayAccordingToDistanceFromFirstNode()
{ MacroRegisterFunctionWithName("Graph::ComputeNodeGroupsForDisplayAccordingToDistanceFromFirstNode");
  this->nodeGroupsForDisplay.SetSize(0);
  int numNodesInComponentsNotConnectedToFirstNode=0;
  int maxDist=0;
  for (int i=0; i<this->distanceToFirstNode.size; i++)
  { maxDist=MathRoutines::Maximum(maxDist, this->distanceToFirstNode[i]);
    if (this->distanceToFirstNode[i]==-1)
      numNodesInComponentsNotConnectedToFirstNode++;
  }
  int numGroups=maxDist+numNodesInComponentsNotConnectedToFirstNode+1;
  List<int> emptyList;
  this->nodeGroupsForDisplay.initFillInObject(numGroups, emptyList);
  int numDisconnectedFound=0;
  for (int i=0; i<this->distanceToFirstNode.size; i++ )
    if (this->distanceToFirstNode[i]==-1)
    { this->nodeGroupsForDisplay[numDisconnectedFound+maxDist+1].AddOnTop(i);
      numDisconnectedFound++;
    } else
      this->nodeGroupsForDisplay[this->distanceToFirstNode[i]].AddOnTop(i);
  this->groupMaxSize=0;
  for (int i =0; i<this->nodeGroupsForDisplay.size; i++)
    this->groupMaxSize=MathRoutines::Maximum(this->groupMaxSize, this->nodeGroupsForDisplay[i].size);
}

std::string Graph::GetNodePSStrings(int groupIndex, int indexInGroup)
{ MacroRegisterFunctionWithName("Graph::GetNodePSStrings");
  std::stringstream out;
  out << "\\rput(" << this->GetXnode(groupIndex, indexInGroup) << ", " << this->GetYnode(groupIndex, indexInGroup)
  << ") " << "{";
  int nodeIndex=this->nodeGroupsForDisplay[groupIndex][indexInGroup];
  if (nodeIndex<this->nodeLabels.size)
    out << this->nodeLabels[nodeIndex];
  else
    out << "node " << nodeIndex+1;
  out << "}";
  return out.str();
}

double Graph::GetXnode(int groupIndex, int indexInGroup)
{ return groupIndex;
}

double Graph::GetYnode(int groupIndex, int indexInGroup)
{ double result=0;
  result= indexInGroup/2;
  if (indexInGroup%2==1)
    result=-1-result;
  return result;
}

std::string Graph::ToStringNodesAndEdges(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("Graph::ToStringNodesAndEdges");
  std::stringstream out;
  out << "The graph has " << this->theEdges.size() << " edges: <br>\n";
  for (int i=0; i<this->theEdges.size(); i++)
  { out << this->theEdges[i].ToString();
    if (i!=this->theEdges.size()-1)
      out << ", ";
  }
  return out.str();
}

std::string Graph::ToStringLatex(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("Graph::ToStringLatex");
  this->ComputeEdgesPerNodesNoMultiplicities();
  this->ComputeDistanceToFirstNode();
  this->ComputeNodeGroupsForDisplayAccordingToDistanceFromFirstNode();
  this->CheckConsistency();
  std::stringstream out;
  out << this->ToStringNodesAndEdges(theFormat);
  out << "<hr><br><br>\\documentclass{article}<br>\n\n";
  out << "\\usepackage{lscape} \\usepackage{pstricks}\\usepackage{auto-pst-pdf}\\usepackage{pst-plot}<br>\n";
  out << "\\begin{document}\\begin{landscape}<br>\n";
  out << "\\psset{xunit=2cm, yunit=2cm}\\begin{pspicture}(0," << -(this->groupMaxSize+1)/2 << ")("
  << this->nodeGroupsForDisplay.size << ", " << (this->groupMaxSize+1)/2 << ")";
  for (int i=0; i<this->nodeGroupsForDisplay.size; i++)
  { for (int j=0; j<this->nodeGroupsForDisplay[i].size; j++)
      out << this->GetNodePSStrings(i, j);
  }
  out << "\n<br>\\end{pspicture}\\end{landscape}";
  out << "\\end{document}<br><br>\n";
  return out.str();
}
