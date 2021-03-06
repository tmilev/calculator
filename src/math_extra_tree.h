#ifndef vpfTreeHeaderAlreadyDefined
#define vpfTreeHeaderAlreadyDefined
#include "math_general.h"

template<typename data>
class Tree;

template<typename data>
class TreeNode {
public:
  Tree<data>* owner;
  int parent;
  int myIndex;
  List<int> children;
  data theData;
  TreeNode(): owner(0), parent(- 1), myIndex(- 1) {
   
  }
  bool checkInitialization() const {
    if (this->owner == nullptr) {
      global.fatal << "Tree node without parent. " << global.fatal;
    }
    return true;
  }
  void addChild(const data& inputData);
  void removeAllChildren();
  const TreeNode<data>& getChild(int i) const;
  std::string toStringHTML(int indentation) const;
};

template <typename data>
class Tree {
public:
  ListReferences<TreeNode<data> > theNodes;
  void resetAddRoot(const data& inputData);
  void reset();
  std::string toString() const;
};


template <typename data>
void TreeNode<data>::addChild(const data& inputData) {
  int newNodeIndex = this->owner->theNodes.size;
  this->owner->theNodes.setSize(this->owner->theNodes.size + 1);
  this->owner->theNodes[newNodeIndex].children.setSize(0);
  this->owner->theNodes[newNodeIndex].owner = this->owner;
  this->owner->theNodes[newNodeIndex].theData = inputData;
  this->owner->theNodes[newNodeIndex].parent = this->myIndex;
  this->owner->theNodes[newNodeIndex].myIndex = newNodeIndex;
  this->children.addOnTop(newNodeIndex);
}

template <typename data>
void TreeNode<data>::removeAllChildren() {
  for (int i = 0; i < this->children.size; i ++) {
    TreeNode<data>& currentNode = this->owner->theNodes[this->children[i]];
    if (currentNode.myIndex == - 1) {
      global.fatal << "Faulty index in tree node: " << this->children[i] << global.fatal;
    }
    currentNode.myIndex = - 1;
    currentNode.removeAllChildren();
  }
  this->children.setSize(0);
}

template <typename data>
const TreeNode<data>& TreeNode<data>::getChild(int i) const {
  this->checkInitialization();
  return this->owner->theNodes[this->children[i]];
}

template <typename data>
std::string TreeNode<data>::toStringHTML(int indentation) const {
  std::stringstream out;
  for (int i = 0; i < indentation; i ++) {
    out << "&nbsp;";
  }
  out << this->myIndex << ": ";
  out << this->theData;
  if (this->children.size > 0) {
    out << "-> " << this->children.toStringCommaDelimited();
  }
  indentation += 2;
  for (int i = 0; i < this->children.size; i ++) {
    out << "<br>" << this->getChild(i).toStringHTML(indentation);
  }
  return out.str();
}

template <typename data>
void Tree<data>::resetAddRoot(const data& inputData) {
  this->reset();
  this->theNodes.setSize(1);
  this->theNodes[0].owner = this;
  this->theNodes[0].children.setSize(0);
  this->theNodes[0].theData = inputData;
  this->theNodes[0].myIndex = 0;
  this->theNodes[0].parent = - 1;
}

template <typename data>
void Tree<data>::reset() {
  this->theNodes.setSize(0);
}

template <typename data>
std::string Tree<data>::toString() const {
  if (this->theNodes.size == 0) {
    return "[empty tree]";
  }
  std::stringstream out;
  out << "Tree with " << this->theNodes.size << " elements. <br>";
  out << this->theNodes[0].toStringHTML(0);
  return out.str();
}
#endif
