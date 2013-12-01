//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderFiniteGroupsIncluded
#define vpfImplementationHeaderFiniteGroupsIncluded

#include "vpfHeader2Math3_FiniteGroups.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderFiniteGroups(__FILE__, "Implementation header, finite groups. ");

template <typename element>
bool FiniteGroup<element>::MakeFrom(const List<element>& generators, int MaxElements)
{ List<int> newElements;
  for(int i=0; i<generators.size; i++)
  { this->theElements.AddOnTop(generators[i]);
    this->lengths.AddOnTop(1);
    int g = this->theElements.GetIndex(generators[i]);
    this->generators.AddOnTop(g);
    newElements.AddOnTop(g);
  }

  while(newElements.size > 0)
  { int gi = newElements.PopLastObject();
    element g = this->theElements[gi];
    for(int i=0; i<this->generators.size; i++)
    { element h = this->theElements[this->generators[i]] * g;
      if(this->theElements.GetIndex(h) == -1)
      { this->theElements.AddOnTop(h);
        this->lengths.AddOnTop(this->lengths[gi]+1);
        if(this->theElements.size > MaxElements)
          return false;
        int hi = this->theElements.GetIndex(h);
        newElements.AddOnTop(hi);
      }
    }
  }
  return true;
}

template <typename element>
void FiniteGroup<element>::ComputeConjugacyClasses()
{ List<bool> Accounted;
  Accounted.initFillInObject(this->theElements.size, false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.ReservE(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->theElements.size);
  List<int> inverseGenerators;
  inverseGenerators.SetSize(this->generators.size);
  for(int i=0; i<this->generators.size; i++)
    inverseGenerators[i] = this->theElements.GetIndex(this->theElements[this->generators[i]].Inverse());
  for(int i=0; i<this->theElements.size; i++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j=0; j<theStack.size; j++)
        for (int k=0; k<this->generators.size; k++)
        { element g = this->theElements[inverseGenerators[k]] * this->theElements[theStack[j]] * this->theElements[this->generators[k]];
          int accountedIndex=this->theElements.GetIndex(g);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex]=true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
    }
  this->conjugacyClasses.QuickSortAscending();
}

template <typename element>
void FiniteGroup<element>::GetSignCharacter(Vector<Rational>& Xs)
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  Xs.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<Xs.size; i++)
  { int yn = this->lengths[conjugacyClasses[i][0]] % 2;
    if(yn == 0)
      Xs[i] = 1;
    else
      Xs[i] = -1;
  }
}

template <typename element>
Rational FiniteGroup<element>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const
{ Rational acc = 0;
  for(int i=0; i<X1.size; i++)
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  return acc / this->theElements.size;
}
#endif
