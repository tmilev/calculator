//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_5_h_already_included
#define vpfHeader1_5_h_already_included

#include "vpfHeader1.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_5
(__FILE__, "Header: basic data structures: selections, subsets, etc. ");

class SelectionOneItem
{
  public:
  int MaxMultiplicity;
  int SelectedMult;
  SelectionOneItem():MaxMultiplicity(0), SelectedMult(-1){}
  bool IncrementReturnFalseIfBackToBeginning()
  { if (this->MaxMultiplicity==0)
      return false;
    this->SelectedMult++;
    if (this->SelectedMult>this->MaxMultiplicity)
      this->SelectedMult=0;
    return this->SelectedMult!=0;
  }
  void initFromMults(int theMult)
  { this->MaxMultiplicity=theMult;
    this->SelectedMult=0;
  }
  std::string ToString()const
  { std::stringstream out;
    out << this->SelectedMult << " out of " << this->MaxMultiplicity;
    return out.str();
  }
};

template <class Base>
class Incrementable
{ public:
  List<Base> theElements;
  Base& operator[](int i)
  { return this->theElements[i];
  }
  LargeInt GetNumTotalCombinations()
  { LargeInt result=1;
    for (int i=0; i<this->theElements.size; i++)
      result*=this->theElements[i].GetNumTotalCombinations();
    return result;
  }
  bool IncrementReturnFalseIfBackToBeginning()
  { for (int i=this->theElements.size-1; i>=0; i--)
      if (this->theElements[i].IncrementReturnFalseIfBackToBeginning())
        return true;
    return false;
  }
  void initFromMults(int inputBase, int numElts)
  { this->theElements.SetSize(numElts);
    for (int i=0; i<this->theElements.size; i++)
      this->theElements[i].initFromMults(inputBase);
  }
  template<class Element>
  void initFromMults(const List<Element>& input, int useOnlyNElementsOnly=0)
  { if (useOnlyNElementsOnly>0 && useOnlyNElementsOnly<=input.size)
      this->theElements.SetSize(useOnlyNElementsOnly);
    else
      this->theElements.SetSize(input.size);
    for (int i=0; i<this->theElements.size; i++)
      this->theElements[i].initFromMults(input[i]);
  }
  std::string ToString()const
  { std::stringstream out;
    out << "(";
    for (int i=0; i<this->theElements.size; i++)
    { out << this->theElements[i].ToString();
      if (i!=this->theElements.size-1)
        out << ", ";
    }
    out << ")";
    return out.str();
  }
};

class SelectionFixedRank
{
public:
  Selection theSelection;
  int DesiredSubsetSize;
  LargeInt GetNumTotalCombinations()
  { LargeInt result;
    MathRoutines::NChooseK(theSelection.MaxSize, DesiredSubsetSize, result);
    return result;
  }
  void SetNumItemsAndDesiredSubsetSize(int inputDesiredSubsetSize, int inputNumItems)
  { if(inputDesiredSubsetSize<0 || inputNumItems<0)
    { std::cout << "This is a programming error: requesting to initialize"
      << " a selection of size " << inputDesiredSubsetSize << " out of "
      << inputNumItems << " elements, which does not make sense. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->theSelection.init(inputNumItems);
    this->DesiredSubsetSize=inputDesiredSubsetSize;
    if (this->DesiredSubsetSize>0)
      this->theSelection.incrementSelectionFixedCardinality(this->DesiredSubsetSize);
  }
  std::string ToString()const
  { return this->theSelection.ToString();
  }
  bool IncrementReturnFalseIfBackToBeginning()
  { this->theSelection.incrementSelectionFixedCardinality(this->DesiredSubsetSize);
    for (int i=0; i<this->DesiredSubsetSize; i++)
      if (!this->theSelection.selected[i])
        return true;
    return false;
  }
};

#endif
