//to be merged in the main file polyhedra.cpp later
#include "polyhedra.h"

void SimpleLieAlgebra::ComputeChevalleyConstants
  (char WeylLetter, int WeylIndex, GlobalVariables& theGlobalVariables)
{ this->theWeyl.MakeArbitrary(WeylLetter, WeylIndex);
  this->theWeyl.ComputeRho();
  this->ChevalleyConstants.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.NullifyAll(false);
  this->ChevalleyConstants.NullifyAll();
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic=true;
  roots& posRoots=this->theWeyl.RootsOfBorel;
  nonExploredRoots.init(posRoots.size);
  for (int i=0;i<posRoots.size;i++)
    nonExploredRoots.AddSelectionAppendNewIndex(i);
  for (int i=0;i<this->theWeyl.RootSystem.size;i++)
		for(int j=i+1;j<this->theWeyl.RootSystem.size;j++)
			if(!this->theWeyl.IsARoot(this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j]))
			{ this->Computed.elements[i][j]=true;
				this->ChevalleyConstants.elements[i][j].MakeZero();
				this->Computed.elements[j][i]=true;
				this->ChevalleyConstants.elements[j][i].MakeZero();
			}
  Rational tempRat;
  while (nonExploredRoots.CardinalitySelection>0)
  { this->ComputeDebugString();
		int theBorelIndex=nonExploredRoots.elements[0];
    Rational theHeight= posRoots.TheObjects[theBorelIndex].GetHeight();
    for (int i=1;i<nonExploredRoots.CardinalitySelection;i++)
    { posRoots.TheObjects[nonExploredRoots.elements[i]].GetHeight(tempRat);
      if (theHeight.IsGreaterThan(tempRat))
      { theHeight.Assign(tempRat);
        theBorelIndex=nonExploredRoots.elements[i];
      }
    }
    root& theRoot= posRoots.TheObjects[theBorelIndex];
    int theIndex=this->theWeyl.RootSystem.IndexOfObjectHash(theRoot);
    root smallRoot2;
    int FirstIndexFirstChoice=-1;
    int SecondIndexFirstChoice=-1;
    for (int i=0;i<this->theWeyl.RootSystem.size;i++)
    { root& smallRoot1=this->theWeyl.RootSystem.TheObjects[i];
      smallRoot1.GetHeight(tempRat);
      tempRat.AssignAbsoluteValue();
      int indexMinusRoot=this->theWeyl.RootSystem.IndexOfObjectHash(-smallRoot1);
      if (theHeight.IsGreaterThan(tempRat))
      { smallRoot2=theRoot-smallRoot1;
        int indexSmallRoot2=this->theWeyl.RootSystem.IndexOfObjectHash(smallRoot2);
        if (indexSmallRoot2!=-1)
        { if (FirstIndexFirstChoice==-1)
          { FirstIndexFirstChoice=i;
            SecondIndexFirstChoice=indexSmallRoot2;
            this->ChevalleyConstants.elements[indexMinusRoot][theIndex].MakeOne();
          }
          this->ComputeOneChevalleyConstant
						(FirstIndexFirstChoice, SecondIndexFirstChoice, indexSmallRoot2, theIndex);
          this->ComputeDebugString();
          this->ExploitSymmetryChevalleyConstants(indexSmallRoot2,theIndex);
        }
      }
      if (this->flagAnErrorHasOccurredTimeToPanic)
				this->ComputeDebugString();
    }
    nonExploredRoots.selected[theIndex]=false;
    nonExploredRoots.ComputeIndicesFromSelection();
  }
}


void SimpleLieAlgebra::ExploitSymmetryChevalleyConstants
  ( int indexEpsilon, int indexEta)
{ int indexMinusEpsilon=this->theWeyl.RootSystem.IndexOfObjectHash
    (-this->theWeyl.RootSystem.TheObjects[indexEpsilon]);
  int indexMinusEta= this->theWeyl.RootSystem.IndexOfObjectHash
    (-this->theWeyl.RootSystem.TheObjects[indexEta]);
  this->ChevalleyConstants.elements[indexEta][indexMinusEpsilon].Assign
    (this->ChevalleyConstants.elements[indexMinusEpsilon][indexEta]);
  this->ChevalleyConstants.elements[indexEta][indexMinusEpsilon].Minus();
  this->ComputeDebugString();
  this->Computed.elements[indexEta][indexMinusEpsilon]=true;
  root tempRoot;
  tempRoot.Assign(this->theWeyl.RootSystem.TheObjects[indexEta]);
  tempRoot.Add(this->theWeyl.RootSystem.TheObjects[indexMinusEpsilon]);
  int i=1;
  while (this->theWeyl.IsARoot(tempRoot))
  { i++;
    tempRoot.Add(this->theWeyl.RootSystem.TheObjects[indexMinusEpsilon]);
  }
  this->ChevalleyConstants.elements[indexEpsilon][indexMinusEta].AssignInteger(-i*i);
  this->ChevalleyConstants.elements[indexEpsilon][indexMinusEta].DivideBy
    (this->ChevalleyConstants.elements[indexMinusEpsilon][indexEta]);
  this->Computed.elements[indexEpsilon][indexMinusEta]=true;
  this->ComputeDebugString();
  this->ChevalleyConstants.elements[indexMinusEta][indexEpsilon].Assign
    (this->ChevalleyConstants.elements[indexEpsilon][indexMinusEta]);
  this->ChevalleyConstants.elements[indexMinusEta][indexEpsilon].Minus();
  this->Computed.elements[indexMinusEta][indexEpsilon]=true;
  this->ComputeDebugString();
}

void SimpleLieAlgebra::ComputeOneChevalleyConstant
  (int indexGamma, int indexDelta, int indexEpsilon, int indexEta)
{ root& gamma= this->theWeyl.RootSystem.TheObjects[indexGamma];
  root& delta= this->theWeyl.RootSystem.TheObjects[indexDelta];
  root& epsilon= this->theWeyl.RootSystem.TheObjects[indexEpsilon];
  root& eta= this->theWeyl.RootSystem.TheObjects[indexEta];
  assert(eta==gamma+delta);
  assert(this->theWeyl.IsARoot(eta-epsilon));
  int indexMinusEpsilon=this->theWeyl.RootSystem.IndexOfObjectHash(-epsilon);
  int indexDeltaMinusEpsilon=this->theWeyl.RootSystem.IndexOfObjectHash(delta-epsilon);
  int indexGammaMinusEpsilon= this->theWeyl.RootSystem.IndexOfObjectHash(gamma-eta);
  assert
    ( this->Computed.elements[indexDelta][indexMinusEpsilon] &&
      this->Computed.elements[indexGamma][indexDeltaMinusEpsilon] &&
      this->Computed.elements[indexMinusEpsilon][indexGamma] &&
      this->Computed.elements[indexDelta][indexGammaMinusEpsilon] &&
      this->Computed.elements[indexGamma][indexDelta] );
  assert(!this->ChevalleyConstants.elements[indexGamma][indexDelta].IsEqualToZero());
  this->ChevalleyConstants.elements[indexMinusEpsilon][indexEta]=
    ( this->ChevalleyConstants.elements[indexDelta][indexMinusEpsilon]*
      this->ChevalleyConstants.elements[indexGamma][indexDeltaMinusEpsilon]+
      this->ChevalleyConstants.elements[indexMinusEpsilon][indexGamma]*
      this->ChevalleyConstants.elements[indexDelta][indexGammaMinusEpsilon])/
        this->ChevalleyConstants.elements[indexGamma][indexDelta];
  this->ChevalleyConstants.elements[indexMinusEpsilon][indexEta].Minus();
  this->Computed.elements[indexMinusEpsilon][indexEta]=true;
}

void ::SimpleLieAlgebra::ElementToString(std::string &output)
{ std::stringstream out;
	std::string tempS;
	this->ChevalleyConstants.ElementToSting(tempS);
	out << tempS;
	output=out.str();
}