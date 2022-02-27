
#ifndef header_math_extra_tree_ALREADY_INCLUDED
#define header_math_extra_tree_ALREADY_INCLUDED

#include "math_general.h"

template <typename data>
class Tree;

template <typename Data>
class TreeNode {
public:
  Tree<Data>* owner;
  int parent;
  int myIndex;
  List<int> children;
  Data data;
  TreeNode(): owner(0), parent(- 1), myIndex(- 1) {}
  bool checkInitialization() const {
    if (this->owner == nullptr) {
      global.fatal << "Tree node without parent. " << global.fatal;
    }
    return true;
  }
  void addChild(const Data& inputData);
  void removeAllChildren();
  const TreeNode<Data>& getChild(int i) const;
  std::string toStringHTML(int indentation) const;
};

template <typename data>
class Tree {
public:
  ListReferences<TreeNode<data> > nodes;
  void resetAddRoot(const data& inputData);
  void reset();
  std::string toString() const;
};

template <typename Data>
void TreeNode<Data>::addChild(const Data& inputData) {
  int newNodeIndex = this->owner->nodes.size;
  this->owner->nodes.setSize(this->owner->nodes.size + 1);
  this->owner->nodes[newNodeIndex].children.setSize(0);
  this->owner->nodes[newNodeIndex].owner = this->owner;
  this->owner->nodes[newNodeIndex].data = inputData;
  this->owner->nodes[newNodeIndex].parent = this->myIndex;
  this->owner->nodes[newNodeIndex].myIndex = newNodeIndex;
  this->children.addOnTop(newNodeIndex);
}

template <typename Data>
void TreeNode<Data>::removeAllChildren() {
  for (int i = 0; i < this->children.size; i ++) {
    TreeNode<Data>& currentNode = this->owner->nodes[this->children[i]];
    if (currentNode.myIndex == - 1) {
      global.fatal
      << "Faulty index in tree node: "
      << this->children[i]
      << global.fatal;
    }
    currentNode.myIndex = - 1;
    currentNode.removeAllChildren();
  }
  this->children.setSize(0);
}

template <typename Data>
const TreeNode<Data>& TreeNode<Data>::getChild(int i) const {
  this->checkInitialization();
  return this->owner->nodes[this->children[i]];
}

template <typename Data>
std::string TreeNode<Data>::toStringHTML(int indentation) const {
  std::stringstream out;
  for (int i = 0; i < indentation; i ++) {
    out << "&nbsp;";
  }
  out << this->myIndex << ": ";
  out << this->data;
  if (this->children.size > 0) {
    out << "-> " << this->children.toStringCommaDelimited();
  }
  indentation += 2;
  for (int i = 0; i < this->children.size; i ++) {
    out << "<br>" << this->getChild(i).toStringHTML(indentation);
  }
  return out.str();
}

template <typename Data>
void Tree<Data>::resetAddRoot(const Data& inputData) {
  this->reset();
  this->nodes.setSize(1);
  this->nodes[0].owner = this;
  this->nodes[0].children.setSize(0);
  this->nodes[0].data = inputData;
  this->nodes[0].myIndex = 0;
  this->nodes[0].parent = - 1;
}

template <typename Data>
void Tree<Data>::reset() {
  this->nodes.setSize(0);
}

template <typename Data>
std::string Tree<Data>::toString() const {
  if (this->nodes.size == 0) {
    return "[empty tree]";
  }
  std::stringstream out;
  out << "Tree with " << this->nodes.size << " elements. <br>";
  out << this->nodes[0].toStringHTML(0);
  return out.str();
}

#endif // header_math_extra_tree_ALREADY_INCLUDED

