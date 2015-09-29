//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfSymmetricGroupsAndGeneralizationsHeader_alreadyIncluded
#define vpfSymmetricGroupsAndGeneralizationsHeader_alreadyIncluded
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math8_VectorSpace.h"
#include "vpfHeader2Math4_Graph.h"


static ProjectInformationInstance ProjectInfoVpfSymmetricGroups(__FILE__, "Header file, symmetric and related groups. Work in progress.");


// dense bases for sparse spaces
// should this carry around the
// List<MonomialCollection<templateMonomial, coefficient> >
// it came from?
template <class templateVector, class templateMonomial, class coefficient>
class SparseSubspaceBasis
{ public:
  bool flagDeallocated;
  int rank;
  List<templateMonomial> involvedMonomials;
  Matrix<coefficient> projectionOperator;

  void SetBasis(const List<templateVector>& basis);
  void DenseVectorInBasis(Vector<coefficient>& out, const templateVector& in);
  bool CheckConsistency() const
  { if (this->flagDeallocated)
      crash << "This is a programming error: use of SparseSubspaceBasis after free. " << crash;
    for (int i=0; i<this->involvedMonomials.size; i++)
    { this->involvedMonomials[i].CheckConsistency();
    }
    this->projectionOperator.CheckConsistency();
    return true;
  }
  SparseSubspaceBasis(){this->flagDeallocated=false;}
  ~SparseSubspaceBasis(){this->flagDeallocated=true;}
  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
};

template <class templateVector, class templateMonomial, class coefficient>
void SparseSubspaceBasis<templateVector, templateMonomial, coefficient>::SetBasis(const List<templateVector>& basis)
{ this->CheckConsistency();

  for(int i=0; i<basis.size; i++)
    for(int j=0; j<basis[i].theMonomials.size; j++)
      this->involvedMonomials.BSInsertDontDup(basis[i].theMonomials[j]);
  Matrix<coefficient> basisMatrix;
  basisMatrix.init(this->involvedMonomials.size, basis.size);
  for(int j=0; j<basis.size; j++)
    for(int i=0; i<involvedMonomials.size; i++)
      basisMatrix.elements[i][j] = basis[j].GetMonomialCoefficient(involvedMonomials[i]);
  Matrix<coefficient> basisMatrixT = basisMatrix;
  basisMatrixT.Transpose();
  Matrix<coefficient> tmp;
  basisMatrix.MultiplyOnTheLeft(basisMatrixT,this->projectionOperator);
  this->projectionOperator.Invert();
  this->projectionOperator *= basisMatrixT;
  this->rank = basis.size;
}

template <class templateVector, class templateMonomial, class coefficient>
void SparseSubspaceBasis<templateVector,templateMonomial,coefficient>::DenseVectorInBasis(Vector<coefficient>& out, const templateVector& in)
{ Vector<coefficient> inDense;
  inDense.SetSize(this->involvedMonomials.size);
  for(int i=0; i<involvedMonomials.size; i++)
    inDense[i] = in.GetMonomialCoefficient(involvedMonomials[i]);
  out = this->projectionOperator * inDense;
}

template <class templateVector, class templateMonomial, class coefficient>
template <typename somestream>
somestream& SparseSubspaceBasis<templateVector,templateMonomial,coefficient>::IntoStream(somestream& out) const
{ out << "Sparse subspace basis object for basis of rank " << this->rank;
  out << " involving monomials " << this->involvedMonomials.ToStringCommaDelimited() << '\n';
  out << " with projection operator\n" << this->projectionOperator.ToStringPlainText() << '\n';
  return out;
}

template <class templateVector, class templateMonomial, class coefficient>
std::string SparseSubspaceBasis<templateVector,templateMonomial,coefficient>::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

template <class templateVector, class templateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& out, const SparseSubspaceBasis<templateVector,templateMonomial,coefficient>& data)
{ return data.IntoStream(out);
}


class Partition;
class Tableau;
class PermutationR2;
class PermutationGroup;

// Partitions are ordered from largest to smallest
class Partition
{ public:
  int n;
  List<int> p;

  int& operator[](int i) const;
  void FromListInt(const List<int> &in, int lastElement = -1);
  static void GetPartitions(List<Partition> &out, int n);
  void FillTableau(Tableau& out, List<int>& stuffing) const;
  void FillTableauOrdered(Tableau& out) const;
  void GetAllStandardTableaux(List<Tableau>& out) const;
  template <typename scalar>
  void SpechtModuleMatricesOfTranspositions1j(List<Matrix<scalar> >& out) const;
  template <typename scalar>
  void SpechtModuleMatricesOfTranspositionsjjplusone(List<Matrix<scalar> >& out) const;
  template <typename scalar>
  void SpechtModuleMatricesOfPermutations(List<Matrix<scalar> >& out, const List<PermutationR2>& perms) const;
  template <typename scalar>
  void SpechtModuleMatrixOfPermutation(List<Matrix<scalar> >& out, const PermutationR2& p) const;
  static void TestAllSpechtModules(int n);
  // int might not be wide enough
  int Fulton61z() const;
  bool operator==(const Partition& right) const;
  bool operator!=(const Partition& right) const;
  bool operator<(const Partition& right) const;
  bool operator>(const Partition& right) const;

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& out, const Partition& data)
  { return data.IntoStream(out);
  }
};

// Tableau methods assume the partition ordering from largest to smallest
class Tableau
{ public:
  List<List<int> > t;

  bool IsStandard() const;
  void RowStabilizer(PermutationGroup& in) const;
  void ColumnStabilizer(PermutationGroup& in) const;
  template <typename coefficient>
  void YoungSymmetrizerAction(ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>&out, const ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>& in);
  List<int> TurnIntoList() const;

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& out, const Tableau& data)
  { return data.IntoStream(out);
  }
};


// Permutations are expected to be represented as a list of cycles
// with the smallest numbers coming first.  each cycle is expected
// to be at least a transposition
// Permutations are also elements of the direct limit of the symmetric groups
class PermutationR2
{
public:
  List<List<int> > cycles;

  // the name MakeCanonical clashes with MakeFromMul and MakeFromTableau
  // but it seems to be typical of this project
  void MakeCanonical();
  unsigned int HashFunction() const;
  static  unsigned int HashFunction(const PermutationR2& in)
                                      { return in.HashFunction();};
  bool operator==(const PermutationR2& right) const;
  int BiggestOccurringNumber() const;
  int operator*(int i) const;
  int Sign() const;
  void BuildTransposition(int i, int j);
  void BuildCycle(const List<int>& cycle);
  // Build operations leave the Permutation in a possibly not canonical state
  // you must MakeCanonical() after using them to get virtually anything to work
  // properly.
  void AddTransposition(int i, int j);
  void AddCycle(const List<int>& cycle);
  void MakeFromListOfCycles(const List<List<int> >& cycle);
  // A jumbled up list of integers 0-n
  void MakeFromActionDescription(const List<int>& actionDescription);
  // Cycle structure as a histogram of cycle lengths
  void GetCycleStructure(List<int>& out) const;
  // Cycle structure as the related partition
  void GetCycleStructure(Partition& out, int n_in_Sn = -1) const;
  // Row by row
  void MakeFromTableauRows(const Tableau& in);
  // this type is hard to *=
  // we are returing the smallest n such that this is an element of Sn
  int MakeFromMul(const PermutationR2& left, const PermutationR2& right);
  void MakeID(const PermutationR2& unused);
  // the purpose of this is compat with other element classes that need
  // to see some things before they can be a proper identity element
  void Invert();
  // some day i'm going to move the out to the first parameter everywhere
  // after i find out why it was put last this time
  static void Conjugate(const PermutationR2& conjugateMe, const PermutationR2& conjugateBy, PermutationR2& out);

  PermutationR2 operator*(const PermutationR2& right) const;

  static bool AreConjugate(const PermutationR2& x, const PermutationR2& y);

  template <typename Object>
  void ActOnList(List<Object>& in) const;
//  MonomialTensor<T1, T2> operator*(MonomialTensor<T1,T2>& right) const;
  void ActOnMonomialTensor(MonomialTensor<int,MathRoutines::IntUnsignIdentity>& out, const MonomialTensor<int,MathRoutines::IntUnsignIdentity>& in) const;

  template <typename coefficient>
  void ActOnTensor(ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>& out, const ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>& in) const;

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& out, const PermutationR2& data)
  { return data.IntoStream(out);
  }
};

// a hyperoctahedral group is a semidirect product of a symmetric group and
// a group of bits.  is there a better name for the group of bits than s?
// false and true are 0 and 1 by convention, which is kinda backwards lol
// false is 1, true is -1.
// an unallocated ElementHyperoctahedralGroup does not have a bits field
// having no bits field is always acceptable and it simply means the bits
// are all unset.
// Multiplication is as folows: (p1,s1)*(p2,s2) = (p1*p2,(p2*s1)*s2)
// where the action of p on s comes from conjugation p⁻¹sp in the standard
// representation, fortuitously (0,1,2) conjugated with xyz gives yzx
// an ElementHyperoctahedralGroup is not an element of any particular
// hyperoctahedral group, but rather of the direct limit of the hyperoctahedral
// groups.  It is the responsibility of the caller, of course, to ensure that
// all requested operations are defined.
class ElementHyperoctahedralGroup
{ public:
  PermutationR2 p;
  List<bool> s;

  void MakeFromPermutation(const PermutationR2& in);
  void MakeFromBits(const List<bool>& in);
  void MakeFromMul(const ElementHyperoctahedralGroup& left, const ElementHyperoctahedralGroup& right);
  // for compatibility with element classes that need a prototype to be able to
  // turn themselves into the identity element
  void MakeID(const ElementHyperoctahedralGroup& unused);
  void Invert();

  ElementHyperoctahedralGroup operator*(const ElementHyperoctahedralGroup& right) const;

  int CountSetBits() const;
  static void Conjugate(const ElementHyperoctahedralGroup& element, const ElementHyperoctahedralGroup& conjugateBy, ElementHyperoctahedralGroup& out);
  bool HasDifferentConjugacyInvariantsFrom(const ElementHyperoctahedralGroup& other) const;
  static bool AreConjugate(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y);
  void GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& out) const;
  bool operator==(const ElementHyperoctahedralGroup& right) const;
  bool operator>(const ElementHyperoctahedralGroup& right) const;
  unsigned int HashFunction() const;
  static  unsigned int HashFunction(const ElementHyperoctahedralGroup& in)
                                      { return in.HashFunction();};

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;

  std::string ToString() const;

  friend std::ostream& operator<<(std::ostream& out, const ElementHyperoctahedralGroup& data)
  { return data.IntoStream(out);
  }
};

// it is forseen that an implementation may throw away theElements
template <typename elementSomeGroup>
class ConjugacyClassR2
{ public:
  elementSomeGroup representative;
  int representativeIndex;
//  List<int> representativeWord;
//  bool haveRepresentativeWord = false;
  List<int> theElements;


  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
};


// this is here until I figure out how to use FiniteGroup
// that, and I guess it might be useful for some Tableau stuff
// ok scratch that, it is a SimpleFiniteGroup class with easier criteria
// for being an element than FiniteGroup
template <typename elementSomeGroup>
class SimpleFiniteGroup
{ public:
  List<elementSomeGroup> generators;
  HashedList<elementSomeGroup> theElements;
  bool haveElements = false;
  int GetSize();

//  List<List<int> > theWords;
//  bool haveWords;

  List<ConjugacyClassR2<elementSomeGroup> > conjugacyClasseS;
  bool flagCCsComputed = false;

  bool easyConjugacyDetermination = false;

  void AddGenDontCompute(const elementSomeGroup& gen) { this->generators.AddOnTop(gen); }
  void ComputeAllElements();
  void ComputeCCRepresentatives(void* unused = NULL);
  void ComputeElementsAndCCs(void* unused = NULL);


  bool AreConjugate(const elementSomeGroup& x, const elementSomeGroup& y);
  bool PossiblyConjugate(const elementSomeGroup& x, const elementSomeGroup& y);

  template <typename coefficient>
  coefficient GetHermitianProduct(const Vector<coefficient>& x1, const Vector<coefficient>& X2);

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
};

class PermutationGroup: public SimpleFiniteGroup<PermutationR2>
{ public:
  bool isSymmetricGroup;

  void MakeSymmetricGroup(int n);

  bool AreConjugate(const PermutationR2& x, const PermutationR2& y);

  // haha, whatever this means
  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& out, const PermutationGroup& data)
  { return data.IntoStream(out);
  }
};

class HyperoctahedralGroup: public SimpleFiniteGroup<ElementHyperoctahedralGroup>
{ public:
  bool isEntireHyperoctahedralGroup;
  bool isEntireDn;

  void MakeHyperoctahedralGroup(int n);

  bool AreConjugate(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y);

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;

  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& out, const HyperoctahedralGroup& data)
  { return data.IntoStream(out);
  }
};

template <typename scalar>
void Partition::SpechtModuleMatricesOfTranspositions1j(List<Matrix<scalar> >& out) const
{ List<PermutationR2> perms;
  perms.SetSize(this->n-1);
  for(int i=0; i<this->n-1; i++)
    perms[i].BuildTransposition(0,i+1);
  this->SpechtModuleMatricesOfPermutations(out, perms);
}

template <typename scalar>
void Partition::SpechtModuleMatricesOfTranspositionsjjplusone(List<Matrix<scalar> >& out) const
{ List<PermutationR2> perms;
  perms.SetSize(this->n-1);
  for(int i=0; i<this->n-1; i++)
    perms[i].BuildTransposition(i,i+1);
  this->SpechtModuleMatricesOfPermutations(out, perms);
}

template <typename scalar>
void Partition::SpechtModuleMatrixOfPermutation(List<Matrix<scalar> >& out, const PermutationR2& p) const
{ List<PermutationR2> perms;
  perms.AddOnTop(p);
  this->SpechtModuleMatricesOfPermutations(out, perms);
}


template <typename scalar>
void Partition::SpechtModuleMatricesOfPermutations(List<Matrix<scalar> >& out, const List<PermutationR2>& perms) const
{ stOutput << "Debugging Partition::SpechtModuleMatricesOfPermutations: " << *this;
  stOutput << " with permutations " << perms.ToStringCommaDelimited() << '\n';
  Tableau initialTableau;
  List<int> stuffing;
  stuffing.SetSize(this->n);
  for(int i=0; i<this->n; i++)
    stuffing[i] = i;
  this->FillTableau(initialTableau, stuffing);
  MonomialTensor<int,MathRoutines::IntUnsignIdentity> tm1;
  tm1.generatorsIndices.SetSize(n);
  tm1.Powers.SetSize(n);
  for(int i=0; i<n; i++)
  { tm1.generatorsIndices[i] = i;
    tm1.Powers[i] = 1;
  }
  ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,scalar> t1, t2, t3;
  t1.AddMonomial(tm1,1);
  initialTableau.YoungSymmetrizerAction(t2,t1);
  stOutput << "Young symmetrizer: " << t1 << " projects to " << t2 << '\n';
  List<ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,scalar> > basisvs;
  SparseSubspaceBasis<ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,scalar>,MonomialTensor<int,MathRoutines::IntUnsignIdentity>,scalar> basis;
  List<Tableau> standardTableaux;
  this->GetAllStandardTableaux(standardTableaux);
  stOutput << "Standard tableaux are " << standardTableaux.ToStringCommaDelimited() << '\n';
  basisvs.SetSize(standardTableaux.size);
  for(int i=0; i<standardTableaux.size; i++)
  { PermutationR2 p;
    p.MakeFromActionDescription(standardTableaux[i].TurnIntoList());
    p.ActOnTensor(basisvs[i],t2);
    stOutput << "Tableau " << standardTableaux[i] << " is assigned permutation " << p;
    stOutput << " permuting Young symmetrized tensor to " << basisvs[i] << '\n';
  }
  basis.SetBasis(basisvs);
  stOutput << "Basis generated: " << basis << '\n';
  out.SetSize(perms.size);
  for(int permi=0; permi<perms.size; permi++)
  { out[permi].init(basis.rank,basis.rank);
    for(int bi=0; bi<basis.rank; bi++)
    { ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,scalar> sparse;
      perms[permi].ActOnTensor(sparse,basisvs[bi]);
      Vector<scalar> dense;
      basis.DenseVectorInBasis(dense, sparse);
      // AssignColumnFromVector?  oh well.
      for(int j=0; j<basis.rank; j++)
        out[permi].elements[j][bi] = dense[j];
    }
  }
}

template <typename somestream>
somestream& Partition::IntoStream(somestream& out) const
{ out << this->n << ": ";
  for(int i=0; i<this->p.size; i++)
    out << this->p[i] << " ";
  return out;
}

template <typename coefficient>
void Tableau::YoungSymmetrizerAction(ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>&out, const ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>& in)
{ //stOutput << "Debugging Tableau::YoungSymmetrizerAction: " << *this << " acts on " << in << '\n';
  PermutationGroup rs,cs;
  this->RowStabilizer(rs);
  rs.ComputeAllElements();
  //stOutput << "Row stabilizer group: " << rs << '\n';
  this->ColumnStabilizer(cs);
  cs.ComputeAllElements();
  //stOutput << "Column stabilizer group: " << cs << '\n';

  ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient> rst;
  for(int i=0; i<rs.theElements.size; i++)
  { ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient> tmp;
    rs.theElements[i].ActOnTensor(tmp, in);
    rst += tmp;
  }
  //stOutput << "Row stabilized: " << rst << '\n';
  out.MakeZero();
  for(int i=0; i<cs.theElements.size; i++)
  { ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient> tmp;
    cs.theElements[i].ActOnTensor(tmp,rst);
    out += tmp * cs.theElements[i].Sign();
  }
  //stOutput << "Young symmetrized: " << out << '\n';
}

template <typename somestream>
somestream& Tableau::IntoStream(somestream& out) const
{ out << "[";
  for(int i=0; i<this->t.size; i++)
  { out << "[";
    for(int j=0; j<this->t[i].size-1; j++)
      out << this->t[i][j] << " ";
    out << this->t[i][this->t[i].size-1];
    out << "]";
    if(i!=this->t.size-1)
      out << ", ";
  }
  out << "]";
  return out;
}

template <typename somestream>
somestream& PermutationR2::IntoStream(somestream& out) const
{ out << "[";
  for(int i=0; i<this->cycles.size; i++)
  { out << "(";
    for(int j=0; j<this->cycles[i].size; j++)
    { out << this->cycles[i][j];
      if(j!=this->cycles[i].size-1)
        out << ", ";
    }
    out << ")";
  }
  out << "]";
  return out;
}

template <typename Object>
void PermutationR2::ActOnList(List<Object>& in) const
{ in.PermuteIndices(this->cycles);
}

template <typename coefficient>
void PermutationR2::ActOnTensor(ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>& out,
  const ElementMonomialAlgebra<MonomialTensor<int,MathRoutines::IntUnsignIdentity>,coefficient>& in) const
{ for(int i=0; i<in.theMonomials.size; i++)
  { MonomialTensor<int,MathRoutines::IntUnsignIdentity> tmpout,tmpin;
    tmpin = in.theMonomials[i];
    this->ActOnMonomialTensor(tmpout,tmpin);
    out.AddMonomial(tmpout,in.theCoeffs[i]);
  }
}

template <typename elementSomeGroup>
bool SimpleFiniteGroup<elementSomeGroup>::PossiblyConjugate(const elementSomeGroup& x, const elementSomeGroup& y)
{ return true;
}

template <typename elementSomeGroup>
bool SimpleFiniteGroup<elementSomeGroup>::AreConjugate(const elementSomeGroup& x, const elementSomeGroup& y)
{ if(!this->flagCCsComputed)
    this->ComputeCCRepresentatives(NULL);
  int xi = this->theElements.GetIndex(x);
  int yi = this->theElements.GetIndex(y);
  for(int i=0; i<this->conjugacyClasseS.size; i++)
    if(this->conjugacyClasseS[i].theElements.BSContains(xi))
      if(this->conjugacyClasseS[i].theElements.BSContains(yi))
        return true;
  return false;
}

template <typename elementSomeGroup>
template <typename coefficient>
coefficient SimpleFiniteGroup<elementSomeGroup>::GetHermitianProduct(const Vector<coefficient>& X1, const Vector<coefficient>& X2)
{ if(!this->haveConjugacyClasses)
    this->ComputeCCRepresentatives(NULL);
  coefficient acc = 0;
  for(int i=0; i<this->conjugacyClasseS.size; i++)
    acc += MathRoutines::ComplexConjugate(X1[i]) * X2[i] * this->conjugacyClasseS[i].size;
  return  acc / this->GetSize();
}

template <typename elementSomeGroup>
void SimpleFiniteGroup<elementSomeGroup>::ComputeAllElements()
{ elementSomeGroup e;
  // if the user thinks its a good idea to instantiate a group of this type
  // without giving it a single element, it had better be a type of element
  // that can be used without initialization.
  if(this->generators.size > 0)
    e.MakeID(this->generators[0]);
  this->theElements.AddOnTop(e);
  List<elementSomeGroup> recentadds;
  recentadds.AddOnTop(e);
  if(easyConjugacyDetermination)
  { this->conjugacyClasseS.SetSize(1);
    this->conjugacyClasseS[0].representative = e;
    this->conjugacyClasseS[0].theElements.AddOnTop(0);
    this->conjugacyClasseS[0].representativeIndex = 0;
//    this->conjugacyClasseS[0].haveRepresentativeWord = true;
  }
//  List<List<int> > recentwords;
//  recentwords.SetSize(1);
  while(recentadds.size > 0)
  { // this is a necessary copy.  I wish I could tell cxx it is a move.
    elementSomeGroup r = recentadds.PopLastObject();
//    List<int> rword = recentwords.PopLastObject();
    for(int i=0; i<this->generators.size; i++)
    { elementSomeGroup p = r*generators[i];
      if(this->theElements.AddOnTopNoRepetition(p))
      { recentadds.AddOnTop(p);
//        recentwords.AddOnTop(rword);
//        recentwords[recentwords.size-1].AddOnTop(i);
//        if(andWords)
//          this->theWords.AddOnTop(recentwords[recentwords.size-1]);
        if(easyConjugacyDetermination)
        { int pindex = this->theElements.GetIndex(p);
          bool alreadyHaveClass = false;
          for(int i=0; i<this->conjugacyClasseS.size; i++)
          { if(this->AreConjugate(this->conjugacyClasseS[i].representative,p))
            { this->conjugacyClasseS[i].theElements.AddOnTop(pindex);
              alreadyHaveClass = true;
            }
            break;
          } // if only cxx had for...else
          if(!alreadyHaveClass)
          { int ncc = this->conjugacyClasseS.size;
            this->conjugacyClasseS.SetSize(ncc+1);
            this->conjugacyClasseS[ncc].representative = p;
            this->conjugacyClasseS[ncc].theElements.AddOnTop(pindex);
//            this->conjugacyClasseS[ncc].representativeIndex = pindex;
//            this->conjugacyClasseS[ncc].representativeWord = recentwords[recentwords.size-1];
          }
        }
      }
    }
  }
  this->haveElements = true;
  if(easyConjugacyDetermination)
    this->flagCCsComputed = true;
//  if(andWords)
//    this->haveWords = true;
}

template <typename elementSomeGroup>
void SimpleFiniteGroup<elementSomeGroup>::ComputeCCRepresentatives(void* unused)
{ if(this->flagCCsComputed)
    return;
  if(this->easyConjugacyDetermination)
  { this->ComputeAllElements();
    return;
  }
//  if(!this->haveWords)
//    this->ComputeAllElements(true);

  GraphOLD conjugacygraph = GraphOLD(this->theElements.size, this->generators.size);
  for(int i=0; i<this->theElements.size; i++)
    for(int j=0; j<this->generators.size; j++)
    { elementSomeGroup x;
      // crazy api lol
      elementSomeGroup::Conjugate(this->theElements[i], this->generators[j], x);
      int xi = this->theElements.GetIndex(x);
      conjugacygraph.AddEdge(i,xi);
    }
  List<List<int> > components = conjugacygraph.DestructivelyGetConnectedComponents();
  // boxing and unboxing...
  this->conjugacyClasseS.SetSize(components.size);
  for(int i=0; i<components.size; i++)
  { this->conjugacyClasseS[i].representative = this->theElements[components[i][0]];
    this->conjugacyClasseS[i].theElements = components[i];
//    this->conjugacyClasseS[i].representativeIndex = components[i][0];
//    this->conjugacyClasseS[i].representativeWord = this->theWords[components[i][0]];
  }
}

template <typename elementSomeGroup>
void SimpleFiniteGroup<elementSomeGroup>::ComputeElementsAndCCs(void* unused)
{ this->ComputeCCRepresentatives();
}

template <typename elementSomeGroup>
int SimpleFiniteGroup<elementSomeGroup>::GetSize()
{ if(!this->haveElements)
    this->ComputeAllElements();
  return this->theElements.size;
}

template <typename elementSomeGroup>
template <typename somestream>
somestream& SimpleFiniteGroup<elementSomeGroup>::IntoStream(somestream& out) const
{ out << "Finite Group with " << this->generators.size << " generators";
  if(this->haveElements)
    out << ", " << this->theElements.size << " elements";
  if(this->flagCCsComputed)
    out << ", " << this->conjugacyClasseS.size << " conjugacy classes";
  out <<", generated by: ";
  for(int i=0; i<this->generators.size; i++)
  { out << this->generators[i];
    if(i != this->generators.size-1)
      out << ", ";
  }
  return out;
}

template <typename somestream>
somestream& PermutationGroup::IntoStream(somestream& out) const
{ out << "Permutation Group with " << this->theElements.size << " elements, generated by: ";
  for(int i=0; i<this->generators.size; i++)
  { out << this->generators[i];
    if(i != this->generators.size-1)
      out << ", ";
  }
  return out;
}

template <typename elementSomeGroup>
std::string SimpleFiniteGroup<elementSomeGroup>::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

template <typename elementSomeGroup>
std::ostream& operator<<(std::ostream& out, const SimpleFiniteGroup<elementSomeGroup>& data)
{ return data.IntoStream(out);
}

template <typename somestream>
somestream& ElementHyperoctahedralGroup::IntoStream(somestream& out) const
{ out << '[' << this->p << ',';
  for(int i=0; i<this->s.size; i++)
    if(this->s[i])
      out << '1';
    else
      out << '0';
  out << ']';
  return out;
}

template <typename somestream>
somestream& HyperoctahedralGroup::IntoStream(somestream& out) const
{ if(this->isEntireHyperoctahedralGroup)
  { out << "Hyperoctahedral Group of rank " << this->generators[0].s.size;
  } else if(this->isEntireDn)
  { out << "Dn group of rank" << this->generators[0].s.size;
  } else
  { out << "Finite group of hyperoctahedral elements, generated by ";
    for(int i=0; i<this->generators.size; i++)
      out << this->generators[i];
    out << " and having " << this->theElements.size << " elements.";
  }
  out << " thus having " << this->theElements.size << " elements";
  return out;
}

template <typename elementSomeGroup>
template <typename somestream>
somestream& ConjugacyClassR2<elementSomeGroup>::IntoStream(somestream& out) const
{ out << "[" << this->representative << ", " << this->theElements.size << "]";
  return out;
}

template <typename elementSomeGroup>
std::string ConjugacyClassR2<elementSomeGroup>::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

template <typename elementSomeGroup>
std::ostream& operator<<(std::ostream& out, const ConjugacyClassR2<elementSomeGroup>& data)
{ return data.IntoStream(out);
}

#endif
