#include "math_extra_graph.h"
#include "math_general.h"

GraphOLD::GraphOLD(int max_vertices, int max_edges) {
  this->vertices = max_vertices;
  this->edges = 0;
  this->max_edges = max_edges;
  this->data.initializeFillInObject(max_vertices * max_edges, - 1);
}

void GraphOLD::AddEdge(int v1, int v2) {
  this->AddDiEdge(v1, v2);
  this->AddDiEdge(v2, v1);
}

// assuming that - 1 only happens after all the edges is okay because
// the DeleteEdge method packs the vertices when it finishes
void GraphOLD::AddDiEdge(int v1, int v2) {
  for (int i = 0; i < this->max_edges; i ++) {
    if (this->data[v1 * this->max_edges + i] == v2) {
      return;
    }
    if (this->data[v1 * this->max_edges + i] == - 1) {
      this->data[v1 * this->max_edges + i] = v2;
      this->edges ++;
      return;
    }
  }
  global.fatal << "attempting to add an edge beyond max_edges" << global.fatal;
}

// methods that can only be called once can have long names
List<List<int> > GraphOLD::DestructivelyGetConnectedComponents() {
  List<List<int> > outerl;
  for (int i = 0; i < this->vertices * this->max_edges; i ++) {
    if (this->data[i] != - 1) {
      List<int> l;
      l.AddOnTop(i / this->max_edges);
      this->TreeRecurseCopyDelete(l, i / this->max_edges, this->max_edges);
      outerl.AddOnTop(l);
    }
  }
  for (int i = 0; i < outerl.size; i ++) {
    outerl[i].QuickSortAscending();
  }
  return outerl;
}

void GraphOLD::TreeRecurseCopyDelete(List<int>& l, int v, int m) {
  for (int i = 0; i < m; i ++) {
    if (this->data[v * m + i] != - 1) {
      int w = this->data[v * m + i];
      if (!l.Contains(w)) {
        l.AddOnTop(w);
      }
      this->data[v * m + i] = - 1;
      this->TreeRecurseCopyDelete(l, w, m);
    }
  }
}

std::string GraphEdge::ToString(FormatExpressions* theFormat) const {
  (void) theFormat; //avoid unused parameter warning, portable.
  std::stringstream out;
  out << this->vStart + 1 << "->" << this->vEnd + 1;
  if (this->label != "") {
    out << "(" << this->label << ")";
  }
  return out.str();
}

void GraphWeightedLabeledEdges::AddEdge(int i, int j) {
  GraphEdge theEdge(i, j);
  this->theEdges.AddMonomial(theEdge, 1);
}

void GraphWeightedLabeledEdges::AddEdge(int i, int j, const std::string& inputLabel) {
  GraphEdge theEdge(i, j, inputLabel);
  this->theEdges.AddMonomial(theEdge, 1);
}

bool GraphWeightedLabeledEdges:: CheckConsistency() const {
  for (int i = 0; i < this->theEdges.size(); i ++) {
    if (
      this->theEdges[i].vStart < 0 ||
      this->theEdges[i].vStart >= this->numNodes ||
      this->theEdges[i].vEnd  < 0 ||
      this->theEdges[i].vEnd >= this->numNodes
    ) {
      global.fatal << "Graph error in graph with " << this->numNodes << " nodes: detected a corrupt edge: "
      << this->theEdges[i].ToString() << " is a bad." << global.fatal;
    }
  }
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    for (int j = 0; j < this->nodeGroupsForDisplay[i].size; j ++) {
      if (
        this->nodeGroupsForDisplay[i][j] < 0 ||
        this->nodeGroupsForDisplay[i][j] >= numNodes
      ) {
        global.fatal << "Graph error in graph with "
        << this->numNodes << " nodes: display group "
        << i + 1 << " is corrupt: has "
        << " node of index " << this->nodeGroupsForDisplay[i][j]
        << global.fatal;
      }
    }
  }
  return true;
}

void GraphWeightedLabeledEdges::ComputeEdgesPerNodesNoMultiplicities() {
  MacroRegisterFunctionWithName("Graph::ComputeEdgesPerNodesNoMultiplicities");
  List<int> emptyList;
  this->edgesPerNodeNoMultiplicities.initializeFillInObject(this->numNodes, emptyList);
  this->CheckConsistency();
  for (int i = 0; i < this->theEdges.size(); i ++) {
    this->edgesPerNodeNoMultiplicities[this->theEdges[i].vStart].AddOnTop(this->theEdges[i].vEnd);
  }
}

void GraphWeightedLabeledEdges::AddNodeToComponent(int nodeIndex) {
  MacroRegisterFunctionWithName("Graph::AddNodeToComponent");
  int componentIndex = this->baseNode[nodeIndex];
  if (componentIndex == - 1) {
    global.fatal << "Bad component index." << global.fatal;
  }
  if (componentIndex >= this->connectedComponents.size) {
    int oldSize = this->connectedComponents.size;
    this->connectedComponents.SetSize(componentIndex + 1);
    for (int i = oldSize; i < this->connectedComponents.size; i ++) {
      this->connectedComponents[i].SetSize(0);
    }
  }
  List<List<int> >& currentComponent = this->connectedComponents[componentIndex];
  int distanceFromBase = this->distanceToBaseNode[nodeIndex];
  if (distanceFromBase == - 1) {
    global.fatal << "Bad node distance." << global.fatal;
  }
  if (distanceFromBase >= currentComponent.size) {
    int oldSize = currentComponent.size;
    currentComponent.SetSize(distanceFromBase + 1);
    for (int i = oldSize; i < currentComponent.size; i ++) {
      currentComponent[i].SetSize(0);
    }
  }
  List<int>& currentDistanceGroup = currentComponent[distanceFromBase];
  currentDistanceGroup.AddOnTop(nodeIndex);
}

void GraphWeightedLabeledEdges::ComputeConnectedComponentsAndBaseNodeDistances() {
  MacroRegisterFunctionWithName("Graph::ComputeConnectedComponentsAndBaseNodeDistances");
  this->distanceToBaseNode.initializeFillInObject(this->numNodes, - 1);
  this->baseNode.initializeFillInObject(this->numNodes, - 1);
  List<int> theOrbit;
  theOrbit.SetExpectedSize(this->theEdges.size());
  for (int indexBaseNode = 0; indexBaseNode < this->numNodes; indexBaseNode ++) {
    if (this->baseNode[indexBaseNode] == - 1) {
      this->distanceToBaseNode[indexBaseNode] = 0;
      this->baseNode[indexBaseNode] = indexBaseNode;
      theOrbit.SetSize(0);
      theOrbit.AddOnTop(indexBaseNode);
      for (int i = 0; i < theOrbit.size; i ++) {
        List<int>& currentHeirs = this->edgesPerNodeNoMultiplicities[theOrbit[i]];
        for (int j = 0; j<currentHeirs.size; j ++) {
          if (this->distanceToBaseNode[currentHeirs[j]] == - 1) {
            this->distanceToBaseNode[currentHeirs[j]] = this->distanceToBaseNode[theOrbit[i]] + 1;
            this->baseNode[currentHeirs[j]] = indexBaseNode;
            theOrbit.AddOnTop(currentHeirs[j]);
          }
        }
      }
    }
  }
  this->connectedComponents.SetSize(0);
  for (int i = 0; i < this->numNodes; i ++) {
    this->AddNodeToComponent(i);
  }
}

void GraphWeightedLabeledEdges::ComputeDisplayGroups() {
  MacroRegisterFunctionWithName("Graph::ComputeDisplayGroups");
  int numGroups = 0;
  for (int i = 0; i < this->connectedComponents.size; i ++) {
    numGroups += this->connectedComponents[i].size;
  }
  this->nodeGroupsForDisplay.SetExpectedSize(numGroups);
  this->nodeGroupsForDisplay.SetSize(0);
  for (int i = 0; i < this->connectedComponents.size; i ++) {
    for (int j = 0; j < this->connectedComponents[i].size; j ++) {
      this->nodeGroupsForDisplay.AddOnTop(this->connectedComponents[i][j]);
    }
  }
  this->groupMaxSize = 0;
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    this->groupMaxSize =MathRoutines::Maximum(this->groupMaxSize, this->nodeGroupsForDisplay[i].size);
  }
  this->positionInDisplayGroup.initializeFillInObject(this->numNodes, - 1);
  this->displayGroupIndices.initializeFillInObject(this->numNodes, - 1);
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    for (int j = 0; j < this->nodeGroupsForDisplay[i].size; j ++) {
      this->positionInDisplayGroup[this->nodeGroupsForDisplay[i][j]] = j;
      this->displayGroupIndices[this->nodeGroupsForDisplay[i][j]] = i;
    }
  }
}

std::string GraphWeightedLabeledEdges::GetNodePSStrings(int groupIndex, int indexInGroup) {
  MacroRegisterFunctionWithName("Graph::GetNodePSStrings");
  std::stringstream out;
  out << "\\rput[l](" << this->GetXnode(groupIndex, indexInGroup) << ", " << this->GetYnode(groupIndex, indexInGroup)
  << ") " << "{\\framebox{\\parbox{0.8cm}{ ";
  int nodeIndex = this->nodeGroupsForDisplay[groupIndex][indexInGroup];
  if (nodeIndex < this->nodeLabels.size) {
    out << this->nodeLabels[nodeIndex];
  } else {
    out << "node " << nodeIndex + 1;
  }
  out << "}}}";
  return out.str();
}

double GraphWeightedLabeledEdges::GetXnode(int groupIndex, int indexInGroup) {
  (void) indexInGroup; //avoid unused parameter warning, portable.
  return groupIndex;
}

double GraphWeightedLabeledEdges::GetYnode(int groupIndex, int indexInGroup) {
  double result = 0;
  if (groupIndex % 2 == 1) {
    result = indexInGroup / 2;
    if (indexInGroup % 2 == 1) {
      result = - 1 - result;
    }
    result += 0.5;
  } else {
    result = - indexInGroup / 2;
    if (indexInGroup % 2 == 1) {
      result = 1 - result;
    }
  }
  return result;
}

std::string GraphWeightedLabeledEdges::ToStringNodesAndEdges(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("Graph::ToStringNodesAndEdges");
  (void) theFormat; //avoid unused parameter warning, portable.
  std::stringstream out;
  out << "The graph has " << this->theEdges.size() << " edges: <br>\n";
  for (int i = 0; i < this->theEdges.size(); i ++) {
    out << this->theEdges[i].ToString();
    if (i != this->theEdges.size() - 1) {
      out << ", ";
    }
  }
  out << "<br>DisplayGroups:";
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    out << "<br>Group " << i + 1 << ", nodes: ";
    for (int j = 0; j < this->nodeGroupsForDisplay[i].size; j ++) {
      out << this->nodeGroupsForDisplay[i][j] << ", ";
    }
  }
  for (int i = 0; i < this->baseNode.size; i ++) {
    out << "<br>Node " << i + 1 << " has base node " << this->baseNode[i] + 1;
  }
  return out.str();
}

std::string GraphWeightedLabeledEdges::ToStringPsTricksEdge(int fromIndex, int toIndex, FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("Graph::ToStringPsTricksEdge");
  (void) theFormat; //avoid unused parameter warning, portable.
  std::stringstream out;
  int startGroupIndex = this->displayGroupIndices[fromIndex];
  int startIndexInGroup = this->positionInDisplayGroup[fromIndex];
  int endGroupIndex = this->displayGroupIndices[toIndex];
  int endIndexInGroup = this->positionInDisplayGroup[toIndex];
  double startX = this->GetXnode(startGroupIndex, startIndexInGroup);
  double startY = this->GetYnode(startGroupIndex, startIndexInGroup);
  double endX = this->GetXnode(endGroupIndex, endIndexInGroup);
  double endY = this->GetYnode(endGroupIndex, endIndexInGroup);
  bool goingRight = (startX < endX);
  bool goingLeft = (startX > endX);
  if (goingRight) {
    out << "\\psline[linecolor =red, arrows =<-](" << startX + 0.7 << ", " << startY << ")";
  } else {
    out << "\\psline[linecolor =red, arrows =<-](" << startX << ", " << startY << ")";
  }
  if (goingLeft) {
    out << "(" << endX + 0.7;
  } else {
    out << "(" << endX;
  }
  out << ", " << endY - (endY - startY) * 0.05 << ")";
  return out.str();
}

std::string GraphWeightedLabeledEdges::ToStringPsTricks(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("Graph::ToStringPsTricks");
  this->ComputeEdgesPerNodesNoMultiplicities();
  this->ComputeConnectedComponentsAndBaseNodeDistances();
  this->ComputeDisplayGroups();
  this->CheckConsistency();
  std::stringstream out;
//  out << this->ToStringNodesAndEdges(theFormat);
  out << "<br>\\documentclass{article}<br>\n\n";
  out << "\\usepackage{pstricks}\\usepackage{lscape}\\usepackage{auto-pst-pdf}\\usepackage{pst-plot}<br>\n";
  out << "\\begin{document}<br>\n";
  out << "\\psset{xunit=1.5cm,yunit=1.5cm}\\begin{pspicture}(0," << - (this->groupMaxSize + 1) / 2 << ")("
  << this->nodeGroupsForDisplay.size << ", " << (this->groupMaxSize + 1) / 2 << ")"
  << "\n<br>\\tiny\n<br>\n";
  for (int i = 0; i < this->edgesPerNodeNoMultiplicities.size; i ++) {
    for (int j = 0; j < this->edgesPerNodeNoMultiplicities[i].size; j ++) {
      out << this->ToStringPsTricksEdge(i, this->edgesPerNodeNoMultiplicities[i][j], theFormat);
    }
  }
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    for (int j = 0; j < this->nodeGroupsForDisplay[i].size; j ++) {
      out << this->GetNodePSStrings(i, j);
    }
  }
  out << "\n<br>\\end{pspicture}";
  out << "\\end{document}<br><br>\n";
  return out.str();
}
