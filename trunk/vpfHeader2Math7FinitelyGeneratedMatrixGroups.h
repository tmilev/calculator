//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader_HeaderFinitelyGeneratedMatrixGroups
#define vpfHeader_HeaderFinitelyGeneratedMatrixGroups
#include "vpfHeader2Math0_General.h"

static ProjectInformationInstance ProjectInfoVpfHeaderFinitelyGeneratedMatrixGroups(__FILE__, "Header, generalized Verma modules. ");

template <class coefficient>
class FinitelyGeneratedMatrixMonoid
{
  public:
  List<MatrixTensor<coefficient> > theGenerators;

  HashedList<MatrixTensor<coefficient> > theElements;
  bool GenerateElements(int upperBoundNonPositiveMeansNoLimit, GlobalVariables* theGlobalVariables=0);
};

template <class coefficient>
bool FinitelyGeneratedMatrixMonoid<coefficient>::GenerateElements(int upperBoundNonPositiveMeansNoLimit, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FinitelyGeneratedMatrixMonoid::GenerateElements");
  this->theElements=theGenerators;
  MatrixTensor<coefficient> currentElement;
  for (int i=0; i<this->theElements.size; i++)
    for (int j=0; j<this->theGenerators.size; j++)
    { currentElement=this->theGenerators[j];
      currentElement*=this->theElements[i];
      this->theElements.AddOnTopNoRepetition(currentElement);
      if (upperBoundNonPositiveMeansNoLimit>0)
        if (this->theElements.size>upperBoundNonPositiveMeansNoLimit)
          return false;
    }
  return true;
}

#endif
