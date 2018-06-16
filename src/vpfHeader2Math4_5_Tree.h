#ifndef vpfTreeHeaderAlreadyDefined
#define vpfTreeHeaderAlreadyDefined
#include "vpfHeader2Math0_General.h"
static ProjectInformationInstance ProjectInfoVpfTreesHeader
(__FILE__, "Header file, trees.");

template<typename data>
class Tree;

template<typename data>
class TreeNode
{
public:
  Tree<data>* owner;
  int parent;
  int myIndex;
  List<int> children;
  data theData;
  TreeNode(): owner(0), parent(- 1), myIndex(- 1)
  {
  }
  bool CheckInitialization()
  { if (this->owner == 0)
      crash << "Tree node without parent. " << crash;
    return true;
  }
  void AddChild(const data& inputData);
  void RemoveAllChildren();
  TreeNode<data>& GetChild(int i);
  std::string ToStringTextFormat(int indentation);
};

template <typename data>
class Tree{
public:
  ListReferences<TreeNode<data> > theNodes;
  void ResetAddRoot(const data& inputData)
  { this->reset();
    this->theNodes.SetSize(1);
    this->theNodes[0].owner = this;
    this->theNodes[0].children.SetSize(0);
    this->theNodes[0].theData = inputData;
    this->theNodes[0].myIndex = 0;
    this->theNodes[0].parent = - 1;
  }
  void reset()
  { this->theNodes.SetSize(0);
  }
};


template <typename data>
void TreeNode<data>::AddChild(const data& inputData)
{ int newNodeIndex= this->owner->theNodes.size;
  this->owner->theNodes.SetSize(this->owner->theNodes.size + 1);
  this->owner->theNodes[newNodeIndex].children.SetSize(0);
  this->owner->theNodes[newNodeIndex].owner = this->owner;
  this->owner->theNodes[newNodeIndex].theData = inputData;
  this->owner->theNodes[newNodeIndex].parent = this->myIndex;
  this->owner->theNodes[newNodeIndex].myIndex = newNodeIndex;
  this->children.AddOnTop(newNodeIndex);
}

template <typename data>
void TreeNode<data>::RemoveAllChildren()
{ for (int i = 0; i < this->children.size; i ++)
  { TreeNode<data>& currentNode = this->owner->theNodes[this->children[i]];
    if (currentNode.myIndex == - 1)
      crash << "Faulty index in tree node: " << this->children[i] << crash;
    currentNode.myIndex = - 1;
    currentNode.RemoveAllChildren();
  }
  this->children.SetSize(0);
}

template <typename data>
TreeNode<data>& TreeNode<data>::GetChild(int i)
{ this->CheckInitialization();
  return this->owner->theNodes[this->children[i]];
}

template <typename data>
std::string TreeNode<data>::ToStringTextFormat(int indentation)
{ std::stringstream out;
  for (int i = 0; i < indentation; i ++)
    out << "&nbsp;";
  out << this->myIndex << ": ";
  out << this->theData;
  if (this->children.size > 0)
    out << "-> " << this->children.ToStringCommaDelimited();
  indentation += 2;
  for (int i = 0; i < this->children.size; i ++)
    out << "<br>" << this->GetChild(i).ToStringTextFormat(indentation);
  return out.str();
}

#endif
