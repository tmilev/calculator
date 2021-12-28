#include "math_extra_graph.h"
#include "math_general.h"

GraphOLD::GraphOLD(int max_vertices, int max_edges) {
  this->vertices = max_vertices;
  this->edges = 0;
  this->max_edges = max_edges;
  this->data.initializeFillInObject(max_vertices * max_edges, - 1);
}

void GraphOLD::addEdge(int v1, int v2) {
  this->addDiEdge(v1, v2);
  this->addDiEdge(v2, v1);
}

// assuming that - 1 only happens after all the edges is okay because
// the DeleteEdge method packs the vertices when it finishes
void GraphOLD::addDiEdge(int v1, int v2) {
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
List<List<int> > GraphOLD::destructivelyGetConnectedComponents() {
  List<List<int> > outerl;
  for (int i = 0; i < this->vertices * this->max_edges; i ++) {
    if (this->data[i] != - 1) {
      List<int> l;
      l.addOnTop(i / this->max_edges);
      this->treeRecurseCopyDelete(l, i / this->max_edges, this->max_edges);
      outerl.addOnTop(l);
    }
  }
  for (int i = 0; i < outerl.size; i ++) {
    outerl[i].quickSortAscending();
  }
  return outerl;
}

void GraphOLD::treeRecurseCopyDelete(List<int>& l, int v, int m) {
  for (int i = 0; i < m; i ++) {
    if (this->data[v * m + i] != - 1) {
      int w = this->data[v * m + i];
      if (!l.contains(w)) {
        l.addOnTop(w);
      }
      this->data[v * m + i] = - 1;
      this->treeRecurseCopyDelete(l, w, m);
    }
  }
}

std::string GraphEdge::toString(FormatExpressions* format) const {
  (void) format;
  std::stringstream out;
  out << this->vStart + 1 << "->" << this->vEnd + 1;
  if (this->label != "") {
    out << "(" << this->label << ")";
  }
  return out.str();
}

void GraphWeightedLabeledEdges::addEdge(int i, int j) {
  GraphEdge graphEdge(i, j);
  this->edges.addMonomial(graphEdge, 1);
}

void GraphWeightedLabeledEdges::addEdge(int i, int j, const std::string& inputLabel) {
  GraphEdge graphEdge(i, j, inputLabel);
  this->edges.addMonomial(graphEdge, 1);
}

bool GraphWeightedLabeledEdges:: checkConsistency() const {
  for (int i = 0; i < this->edges.size(); i ++) {
    if (
      this->edges[i].vStart < 0 ||
      this->edges[i].vStart >= this->numNodes ||
      this->edges[i].vEnd  < 0 ||
      this->edges[i].vEnd >= this->numNodes
    ) {
      global.fatal << "Graph error in graph with " << this->numNodes << " nodes: detected a corrupt edge: "
      << this->edges[i].toString() << " is a bad." << global.fatal;
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

void GraphWeightedLabeledEdges::computeEdgesPerNodesNoMultiplicities() {
  MacroRegisterFunctionWithName("Graph::computeEdgesPerNodesNoMultiplicities");
  List<int> emptyList;
  this->edgesPerNodeNoMultiplicities.initializeFillInObject(this->numNodes, emptyList);
  this->checkConsistency();
  for (int i = 0; i < this->edges.size(); i ++) {
    this->edgesPerNodeNoMultiplicities[this->edges[i].vStart].addOnTop(this->edges[i].vEnd);
  }
}

void GraphWeightedLabeledEdges::addNodeToComponent(int nodeIndex) {
  MacroRegisterFunctionWithName("Graph::addNodeToComponent");
  int componentIndex = this->baseNode[nodeIndex];
  if (componentIndex == - 1) {
    global.fatal << "Bad component index." << global.fatal;
  }
  if (componentIndex >= this->connectedComponents.size) {
    int oldSize = this->connectedComponents.size;
    this->connectedComponents.setSize(componentIndex + 1);
    for (int i = oldSize; i < this->connectedComponents.size; i ++) {
      this->connectedComponents[i].setSize(0);
    }
  }
  List<List<int> >& currentComponent = this->connectedComponents[componentIndex];
  int distanceFromBase = this->distanceToBaseNode[nodeIndex];
  if (distanceFromBase == - 1) {
    global.fatal << "Bad node distance." << global.fatal;
  }
  if (distanceFromBase >= currentComponent.size) {
    int oldSize = currentComponent.size;
    currentComponent.setSize(distanceFromBase + 1);
    for (int i = oldSize; i < currentComponent.size; i ++) {
      currentComponent[i].setSize(0);
    }
  }
  List<int>& currentDistanceGroup = currentComponent[distanceFromBase];
  currentDistanceGroup.addOnTop(nodeIndex);
}

void GraphWeightedLabeledEdges::computeConnectedComponentsAndBaseNodeDistances() {
  MacroRegisterFunctionWithName("Graph::computeConnectedComponentsAndBaseNodeDistances");
  this->distanceToBaseNode.initializeFillInObject(this->numNodes, - 1);
  this->baseNode.initializeFillInObject(this->numNodes, - 1);
  List<int> orbit;
  orbit.setExpectedSize(this->edges.size());
  for (int indexBaseNode = 0; indexBaseNode < this->numNodes; indexBaseNode ++) {
    if (this->baseNode[indexBaseNode] == - 1) {
      this->distanceToBaseNode[indexBaseNode] = 0;
      this->baseNode[indexBaseNode] = indexBaseNode;
      orbit.setSize(0);
      orbit.addOnTop(indexBaseNode);
      for (int i = 0; i < orbit.size; i ++) {
        List<int>& currentHeirs = this->edgesPerNodeNoMultiplicities[orbit[i]];
        for (int j = 0; j<currentHeirs.size; j ++) {
          if (this->distanceToBaseNode[currentHeirs[j]] == - 1) {
            this->distanceToBaseNode[currentHeirs[j]] = this->distanceToBaseNode[orbit[i]] + 1;
            this->baseNode[currentHeirs[j]] = indexBaseNode;
            orbit.addOnTop(currentHeirs[j]);
          }
        }
      }
    }
  }
  this->connectedComponents.setSize(0);
  for (int i = 0; i < this->numNodes; i ++) {
    this->addNodeToComponent(i);
  }
}

void GraphWeightedLabeledEdges::computeDisplayGroups() {
  MacroRegisterFunctionWithName("Graph::computeDisplayGroups");
  int numGroups = 0;
  for (int i = 0; i < this->connectedComponents.size; i ++) {
    numGroups += this->connectedComponents[i].size;
  }
  this->nodeGroupsForDisplay.setExpectedSize(numGroups);
  this->nodeGroupsForDisplay.setSize(0);
  for (int i = 0; i < this->connectedComponents.size; i ++) {
    for (int j = 0; j < this->connectedComponents[i].size; j ++) {
      this->nodeGroupsForDisplay.addOnTop(this->connectedComponents[i][j]);
    }
  }
  this->groupMaxSize = 0;
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    this->groupMaxSize =MathRoutines::maximum(this->groupMaxSize, this->nodeGroupsForDisplay[i].size);
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

std::string GraphWeightedLabeledEdges::getNodePSStrings(int groupIndex, int indexInGroup) {
  MacroRegisterFunctionWithName("Graph::getNodePSStrings");
  std::stringstream out;
  out << "\\rput[l](" << this->getXNode(groupIndex, indexInGroup) << ", " << this->getYNode(groupIndex, indexInGroup)
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

double GraphWeightedLabeledEdges::getXNode(int groupIndex, int indexInGroup) {
  (void) indexInGroup;
  return groupIndex;
}

double GraphWeightedLabeledEdges::getYNode(int groupIndex, int indexInGroup) {
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

std::string GraphWeightedLabeledEdges::toStringNodesAndEdges(FormatExpressions* format) {
  MacroRegisterFunctionWithName("Graph::toStringNodesAndEdges");
  (void) format;
  std::stringstream out;
  out << "The graph has " << this->edges.size() << " edges: <br>\n";
  for (int i = 0; i < this->edges.size(); i ++) {
    out << this->edges[i].toString();
    if (i != this->edges.size() - 1) {
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

std::string GraphWeightedLabeledEdges::toStringPsTricksEdge(int fromIndex, int toIndex, FormatExpressions* format) {
  MacroRegisterFunctionWithName("Graph::toStringPsTricksEdge");
  (void) format;
  std::stringstream out;
  int startGroupIndex = this->displayGroupIndices[fromIndex];
  int startIndexInGroup = this->positionInDisplayGroup[fromIndex];
  int endGroupIndex = this->displayGroupIndices[toIndex];
  int endIndexInGroup = this->positionInDisplayGroup[toIndex];
  double startX = this->getXNode(startGroupIndex, startIndexInGroup);
  double startY = this->getYNode(startGroupIndex, startIndexInGroup);
  double endX = this->getXNode(endGroupIndex, endIndexInGroup);
  double endY = this->getYNode(endGroupIndex, endIndexInGroup);
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

std::string GraphWeightedLabeledEdges::toStringPsTricks(FormatExpressions* format) {
  MacroRegisterFunctionWithName("Graph::toStringPsTricks");
  this->computeEdgesPerNodesNoMultiplicities();
  this->computeConnectedComponentsAndBaseNodeDistances();
  this->computeDisplayGroups();
  this->checkConsistency();
  std::stringstream out;
//  out << this->toStringNodesAndEdges(format);
  out << "<br>\\documentclass{article}<br>\n\n";
  out << "\\usepackage{pstricks}\\usepackage{lscape}\\usepackage{auto-pst-pdf}\\usepackage{pst-plot}<br>\n";
  out << "\\begin{document}<br>\n";
  out << "\\psset{xunit=1.5cm,yunit=1.5cm}\\begin{pspicture}(0," << - (this->groupMaxSize + 1) / 2 << ")("
  << this->nodeGroupsForDisplay.size << ", " << (this->groupMaxSize + 1) / 2 << ")"
  << "\n<br>\\tiny\n<br>\n";
  for (int i = 0; i < this->edgesPerNodeNoMultiplicities.size; i ++) {
    for (int j = 0; j < this->edgesPerNodeNoMultiplicities[i].size; j ++) {
      out << this->toStringPsTricksEdge(i, this->edgesPerNodeNoMultiplicities[i][j], format);
    }
  }
  for (int i = 0; i < this->nodeGroupsForDisplay.size; i ++) {
    for (int j = 0; j < this->nodeGroupsForDisplay[i].size; j ++) {
      out << this->getNodePSStrings(i, j);
    }
  }
  out << "\n<br>\\end{pspicture}";
  out << "\\end{document}<br><br>\n";
  return out.str();
}
