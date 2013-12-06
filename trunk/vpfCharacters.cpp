//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfHeader2Math4_Graph.h"
static ProjectInformationInstance ProjectInfoVpfCharacters(__FILE__, "Finite group characters sandbox. Work in progress by Thomas. ");

extern FormatExpressions consoleFormat;

template<>
typename List<ElementWeylGroup<WeylGroup> >::OrderLeftGreaterThanRight FormatExpressions::GetMonOrder<ElementWeylGroup<WeylGroup> >()
{ return 0;
}

Vector<Rational> WeylGroup::ApplyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv)const
{ Vector<Rational> v = vv;
  for(int i = rightReflectionsActFirst.size-1; i>=0; i--)
  { Rational x=0;
    for(int j=0; j<this->GetDim(); j++)
      x += v[j] * CartanSymmetric(rightReflectionsActFirst[i],j);
    //CartanSymmetric(i,j) is slower than CartanSymmetric.elements[i][j] but includes index checking
    //(we want to catch a rogue call to ApplyReflectionList
    v[rightReflectionsActFirst[i]] -= x * 2 / CartanSymmetric(rightReflectionsActFirst[i], rightReflectionsActFirst[i]);
  }
  return v;
}

void WeylGroup::GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const
{ MacroRegisterFunctionWithName("WeylGroup::GetSimpleReflectionMatrix");
  int rank=this->GetDim();
  output.MakeIdMatrix(rank);
  for (int j=0; j<rank; j++)
    output(indexSimpleRoot,j)-=(this->CartanSymmetric(indexSimpleRoot,j) / CartanSymmetric(indexSimpleRoot, indexSimpleRoot) )*2;
}

Matrix<Rational> WeylGroup::SimpleReflectionMatrix(int i) const
{ Matrix<Rational> result;
  this->GetSimpleReflectionMatrix(i, result);
  return result;
}

ElementWeylGroup<WeylGroup> WeylGroup::SimpleConjugation(int i, const ElementWeylGroup<WeylGroup>& vv)
{ ElementWeylGroup<WeylGroup> eltSimpleReflection;
  eltSimpleReflection.MakeSimpleReflection(i, *this);
  return eltSimpleReflection*this->theElements[i]*eltSimpleReflection;
}

int WeylGroup::MultiplyElements(int indexLeft, int indexRight) const
{ return this->theElements.GetIndex(this->theElements[indexLeft]*this->theElements[indexRight]);
}

int WeylGroup::operator()(int i, int j) const
{ return MultiplyElements(i,j);
}

void WeylGroup::ComputeConjugacyClassesThomasVersion()
{//From Todor: Thomas, you need to explain how this code works.
  if(rhoOrbit.size == 0)
    this->ComputeAllElements();
  Graph G = Graph(this->theElements.size,this->GetDim());
  for(int i=0;i<this->theElements.size;i++)
    for(int j=0;j<this->GetDim();j++)
      G.AddEdge(i, this->theElements.GetIndex(this->SimpleConjugation(j, this->theElements[i])));
  conjugacyClasses = G.DestructivelyGetConnectedComponents();
}

void WeylGroup::ComputeConjugacyClasses(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeConjugacyClasses");
  this->CheckConsistency();
  const int hardcodedUpperLimit=6000000;
  if (this->theDynkinType.GetSizeWeylGroupByFormula()>hardcodedUpperLimit)
    crash << "I am crashing for safety reasons (this is not a programming error). You requested to compute the conjugacy classes of a Weyl group of type "
    << this->theDynkinType.ToString() << ", which, by formula, has " << this->theDynkinType.GetSizeWeylGroupByFormula().ToString()
    << " elements, but I have a hard-coded safety limit of " << hardcodedUpperLimit << "."
    << crash;
  //  std::cout << "<hr>HEre be errors";
  this->ComputeAllElements(hardcodedUpperLimit+1);
  //std::cout << "<hr>rho orbit: " <<  this->rhoOrbit.size << " total: " << this->rhoOrbit.ToString();
  //std::cout << "<hr> the elements: " << this->theElements.size << " total: " << this->theElements.ToString();
  //if (this->rhoOrbit.size!=this->theElements.size)
  //{
  //}
  List<bool> Accounted;
  //std::cout << "ComputeConjugacyClasses: group has " << this->theElements.size << " elements." << std::endl;
  Accounted.initFillInObject(this->theElements.size, false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.ReservE(120); //<- that should be good to go up to E8.
  //For higher number of conjugacy classes List's grow exponentially on demand, so this shouldn't be a big slow-down.
  int theRank=this->GetDim();
  ElementWeylGroup<WeylGroup> theConjugated;
  List<ElementWeylGroup<WeylGroup> > theGenerators;
  theGenerators.SetSize(theRank);
  for (int i=0; i<theRank; i++)
    theGenerators[i].MakeSimpleReflection(i, *this);
  std::cout << "Here be the simple gens:" << theGenerators.ToString();
  for (int i=0; i<this->theElements.size; i++)
    if (!Accounted[i])
    { Accounted[i] = true;
      this->conjugacyClasses.SetSize(conjugacyClasses.size+1);
      List<int>& currentClass=*this->conjugacyClasses.LastObject();
      currentClass.SetSize(0);//<-needed in case the group is being recomputed
      currentClass.AddOnTop(i);
      for (int j=0; j<currentClass.size; j++)
        for (int k=0; k<theRank; k++)
        { theConjugated=theGenerators[k]*this->theElements[currentClass[j]]*theGenerators[k];
          std::cout << "<hr>" << theGenerators[k].ToString() << " * " << this->theElements[currentClass[j]].ToString()
          << "*" << theGenerators[k].ToString() << "=" << theConjugated.ToString() << "<hr>";
          int accountedIndex=this->theElements.GetIndex(theConjugated);
          if (accountedIndex==-1)
            crash << "Programming error: failed to find element " << theConjugated.ToString() << " in list of elements" << crash;
          if(!Accounted[accountedIndex])
          { currentClass.AddOnTop(accountedIndex);
            Accounted[accountedIndex] = true;
          }
        }
      currentClass.QuickSortAscending();
    }
  int checkNumElts=0;
  for (int i=0; i<this->conjugacyClasses.size; i++)
    checkNumElts+=this->conjugacyClasses[i].size;
  if (this->theElements.size!=checkNumElts)
    crash << "This is a programming error: there are total of " << checkNumElts << " elements in the various conjugacy classes while the group has "
    << this->theElements.size << " elements" << crash;
  this->conjugacyClasses.QuickSortAscending();
  std::cout << "conj class report: " << this->ToString();
  this->CheckInitializationFDrepComputation();
  //std::cout << "weyl group of type " << this->theDynkinType.ToString() << " has " << this->conjugacyClasses.size << "conjugacy classes" << std::endl;
  //for(int i=0; i<conjugacyClasses.size; i++)
  //  std::cout << i << " " << conjugacyClasses[i].size << " " << conjugacyClasses[i] << std::endl;
}

void WeylGroup::ComputeSquares()
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  this->squaresFirstConjugacyClassRep.SetExpectedSize(this->conjugacyClasses.size);
  this->squaresFirstConjugacyClassRep.SetSize(this->conjugacyClasses.size);
  ElementWeylGroup<WeylGroup> currentSquare;
  for(int i=0;i<this->conjugacyClasses.size;i++)
  { this->squaresFirstConjugacyClassRep[i]=this->theElements[this->conjugacyClasses[i][0]];
    this->squaresFirstConjugacyClassRep[i]*=this->theElements[this->conjugacyClasses[i][0]];
  }
}

void WeylGroup::ComputeInitialCharacters()
{ MacroRegisterFunctionWithName("WeylGroup::ComputeInitialCharacters");
  if(this->squaresFirstConjugacyClassRep.size == 0)
    this->ComputeSquares();
  this->characterTable.SetSize(0);
  this->characterTable.SetExpectedSize(this->conjugacyClasses.size);

  ClassFunction<Rational> Xe, Xsgn, Xstd;
  Xe.G = this;
  Xe.data.initFillInObject(this->conjugacyClasses.size, 1);
  Xsgn.G = this;
  Xsgn.data.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<this->conjugacyClasses.size; i++)
    Xsgn.data[i]=this->theElements[this->conjugacyClasses[i][0]].Sign();
  this->characterTable.AddOnTop(Xsgn);
  this->characterTable.AddOnTop(Xe);
  Xstd.G = this;
  Xstd.data.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<this->conjugacyClasses.size; i++)
    Xstd.data[i]=this->GetMatrixStandardRep(this->conjugacyClasses[i][0]).GetTrace();
//  Xstd = Xstd.ReducedWithChars();
  characterTable.AddOnTop(Xstd);
}

// This is dumb, but i couldnt figure out what else to do
template<> // haha wat
unsigned int VectorSpace<Rational>::HashFunction(const VectorSpace<Rational>& in)
{ return in.fastbasis.HashFunction();
}
// whenever i figure out cxx i can get rid of this.  but im probably jst
// going to define this for every other field


template <typename coefficient>
void MatrixInBasis(Matrix<coefficient>& out, const Matrix<coefficient>& in, const List<Vector<coefficient> >& basis, const Matrix<coefficient>& gramMatrix)
{ int d = basis.size;
  out.init(d, d);
  for(int i=0; i<d; i++)
    for(int j=0; j<d; j++)
      out.elements[i][j] = basis[i].ScalarEuclidean(in*basis[j]);
  out.MultiplyOnTheLeft(gramMatrix);
}

template <typename coefficient>
void MatrixInBasisFast(Matrix<coefficient>& out, const Matrix<coefficient>& in, const Matrix<coefficient>& BM)
{ int d = BM.NumRows;
  Matrix<coefficient> M = BM;
  Matrix<coefficient> inT = in;
  inT.Transpose();
  M.MultiplyOnTheRight(inT);
  out.MakeZeroMatrix(d);
  for(int i=0; i<d; i++)
  { int jj = 0;
    for(int j=0; j<d; j++)
    {while((jj < M.NumCols) and (M.elements[i][jj] == 0))
      jj++;
     if(jj == M.NumCols){
      out.elements[i][j] = 0;
      continue;
     }
     if(BM.elements[j][jj] == 0){
      out.elements[i][j] = 0;
      continue;
     }
     out.elements[i][j] = M.elements[i][jj] / BM.elements[j][jj];
     for(int k=jj; k<M.NumCols; k++)
       M.elements[i][k] -= BM.elements[j][k] * out.elements[i][j];
    }
  }
//  std::cout << "MatrixInBasisFast" << std::endl;
//  std::cout << in.ToString(&consoleFormat) << '\n' << BM.ToString(&consoleFormat) << '\n' << out.ToString(&consoleFormat) << std::endl;
}

template <typename coefficient>
class SpaceTree
{
public:
   List<Vector<coefficient> > space;
   List<SpaceTree<coefficient> > subspaces;

   void PlaceInTree(const List<Vector<coefficient> >& V);
   void GetLeaves(List<List<Vector<coefficient> > >& leaves) const;
   void DisplayTree() const;
};

template <typename coefficient>
void SpaceTree<coefficient>::PlaceInTree(const List<Vector<coefficient> > &V)
{ List<Vector<coefficient> > U;
  for(int i=0; i<subspaces.size; i++)
  { intersection(V, subspaces[i].space, U);
    if(U.size == 0)
      continue;
    if(U.size == subspaces[i].space.size)
      return;
    subspaces[i].PlaceInTree(U);
  }
  List<Vector<coefficient> > W, tempVspace;
  for(int i=0; i<subspaces.size; i++)
  { getunion(W, subspaces[i].space, tempVspace);
    W=tempVspace;
  }
  intersection(W, V, tempVspace);
  if(tempVspace.size != V.size)
  { SpaceTree<coefficient> vst;
    vst.space = V;
    subspaces.AddOnTop(vst);
  }
}

template <typename coefficient>
void SpaceTree<coefficient>::GetLeaves(List<List<Vector<coefficient> > >& leaves) const
{  if(subspaces.size > 0)
      for(int i=0; i<subspaces.size; i++)
         subspaces[i].GetLeaves(leaves);
   else
      leaves.AddOnTop(space);
}

template <typename coefficient>
void SpaceTree<coefficient>::DisplayTree() const
{  std::cout << '[' << space.size << ',';
   for(int i=0; i<subspaces.size; i++)
      subspaces[i].DisplayTree();
   std::cout << ']';
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::MultiplyBy
(const WeylGroupRepresentation<coefficient>& other, WeylGroupRepresentation<coefficient>& output) const
{ //lazy programmers handling://////
  if (&output==this || &output==&other)
  { WeylGroupRepresentation<coefficient> thisCopy, otherCopy;
    thisCopy=*this;
    otherCopy=other;
    thisCopy.MultiplyBy(otherCopy, output);
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup!=other.ownerGroup)
    crash << "This is a programming error: attempting to multiply representations with different owner groups. " << crash;
  output.reset(this->G);
  int Vd = this->basis[0].size;
  int Wd = other.basis[0].size;
  int Ud = Vd*Wd;
  for(int vi=0; vi<this->basis.size; vi++)
    for(int wi=0; wi<other.basis.size; wi++)
    { Vector<coefficient> u;
      u.SetSize(Ud);
      for(int i=0; i<Vd; i++)
        for(int j=0; j<Wd; j++)
          u[i*Wd+j] = this->basis[vi][i] * other.basis[wi][j];
      output.basis.AddOnTop(u);
    }
  output.generators.SetSize(this->generators.size);
  for(int i=0; i<this->generators.size; i++)
    output.generators[i].AssignTensorProduct(this->generators[i],other.generators[i]);
  /* would that it be this simple
  if((classFunctionMatrices.size > 0) && (other.classFunctionMatrices.size > 0))
  { U.classFunctionMatrices.SetSize(G->ccCount);
    for(int i=0; i<classFunctionMatrices.size; i++)
    { if((classFunctionMatrices[i].NumCols>0) && (other.classFunctionMatrices[i].NumCols>0))
      U.classFunctionMatrices[i].AssignTensorProduct(classFunctionMatrices[i],other.classFunctionMatrices[i]);
    }
  }*/
  output.ownerGroup = this->ownerGroup;
}

template <typename coefficient>
WeylGroupRepresentation<coefficient> WeylGroupRepresentation<coefficient>::Reduced() const
{ int d = basis.size;

  Matrix<coefficient> GM;
  GM.init(d, d);
  for(int i=0; i<d; i++)
    for(int j=0; j<d; j++)
      GM.elements[i][j] = this->basis[i].ScalarEuclidean(this->basis[j]);
  GM.Invert();
  WeylGroupRepresentation<coefficient> out;
  out.generators.SetSize(generators.size);
  for(int i=0; i<this->generators.size; i++)
//     MatrixInBasisFast(out.generators[i], this->generators[i], BM);
    MatrixInBasis(out.generators[i],this->generators[i],this->basis,GM);

   out.ownerGroup = ownerGroup;
   out.basis.MakeEiBasis(d);

/*
   if(classFunctionMatrices.size > 0)
   { out.classFunctionMatrices.SetSize(G->ccCount);
     for(int i=0; i<classFunctionMatrices.size; i++)
      { if(classFunctionMatrices[i].NumRows > 0)
        { //MatrixInBasisFast(out.classFunctionMatrices[i],classFunctionMatrices[i],BM);
          MatrixInBasis(out.classFunctionMatrices[i],classFunctionMatrices[i],basis,GM);
        }
      }
   }
*/
   return out;
}

template <typename coefficient>
VectorSpace<coefficient> WeylGroupRepresentation<coefficient>::FindDecentBasis() const
{ VectorSpace<coefficient> V;
  int d = this->generators[0].NumCols;
  for(int geni=0; geni<this->generators.size; geni++)
  { List<Vector<coefficient> > ess = eigenspaces(this->generators[geni]);
    for(int essi=0; essi<ess.size; essi++)
      for(int esi=0; esi<ess[essi].size; esi++)
      { // the best laid coding practices of mice and men oft go astray
        if(!V.AddVectorToBasis(ess[essi][esi]))
          return V;
      }
  }
  // This should not be possible
  for(int i=0; i<d; i++)
  { Vector<coefficient> v;
    v.MakeEi(d,i);
    if(!V.AddVectorToBasis(v))
      return V;
  }
  // if it gets here, it deserves to crash
}

template <typename coefficient>
List<WeylGroupRepresentation<coefficient> > WeylGroupRepresentation<coefficient>::DecomposeThomasVersion()
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeThomasVersion");
  Matrix<coefficient> splittingOperatorMatrix;
  List<WeylGroupRepresentation<coefficient> > out;
  List<Vector<Rational> > splittingMatrixKernel;
  if(GetNumberOfComponents() == 1)
  { if(this->ownerGroup->characterTable.GetIndex(this->theCharacter) == -1)
    { std::cout << "new irrep found, have " << this->ownerGroup->characterTable.size << std::endl;
      this->ownerGroup->characterTable.AddOnTop(this->theCharacter);
      this->ownerGroup->irreps.AddOnTop(*this);
    }
    out.AddOnTop(*this);
    return out;
  }
  List<Vector<coefficient> > Vb = basis;
  List<Vector<coefficient> > tempVectors;
  for(int i=0; i<this->ownerGroup->characterTable.size; i++)
    if(this->theCharacter.InnerProduct(this->ownerGroup->characterTable[i])!=0)
    { std::cout << "contains irrep " << i << std::endl;
      ClassFunctionMatrix(this->ownerGroup->characterTable[i], splittingOperatorMatrix);
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);
      intersection(Vb, splittingMatrixKernel, tempVectors);
      Vb=tempVectors;
    }
  if((Vb.size < basis.size) && (Vb.size > 0))
  { std::cout << "calculating remaining subrep... ";
    WeylGroupRepresentation<coefficient> V;
    V.ownerGroup = this->ownerGroup;
    V.generators = this->generators;
    V.basis = Vb;
    V = V.Reduced();
    std::cout << "done" << std::endl;
    std::cout << "Decomposing remaining subrep " << V.GetCharacter() << std::endl;
    return V.DecomposeThomasVersion();
  }
  if(Vb.size == 0)
    return out;
/*  SpaceTree<Rational> st;
  st.space = basis;
  for(int cfi=0; cfi<G->ccCount; cfi++)
  {  ClassFunction<coefficient> cf;
      cf.G = G;
      cf.MakeZero();
      cf[cfi] = 1;
      std::cout << "getting matrix " << cf << std::endl;
      Matrix<coefficient> A;
      ClassFunctionMatrix(cf, A);
      List<List<Vector<coefficient> > > es = eigenspaces(A);
      std::cout << "eigenspaces were ";
      for(int i=0; i<es.size; i++)
         std::cout << es[i].size << " ";
      std::cout << std::endl;
      for(int i=0; i<es.size; i++)
         st.PlaceInTree(es[i]);
      std::cout << "tree is ";
      st.DisplayTree();
      std::cout << std::endl;
  }
  List<List<Vector<coefficient> > > leaves;
  st.GetLeaves(leaves);
  std::cout << "leaves are ";
  for(int i=0; i<leaves.size; i++)
    std::cout << leaves[i].size << " ";
  std::cout << std::endl;
*/
  List<List<Vector<coefficient> > > es;
  for(int cfi=this->ownerGroup->conjugacyClasses.size-1; cfi>=0; cfi--)
  { ClassFunction<coefficient> cf;
    cf.MakeZero(*this->ownerGroup);
    cf[cfi] = 1;
    std::cout << "getting matrix " << cf << std::endl;
    Matrix<coefficient> A;
    ClassFunctionMatrix(cf, A);
    es = eigenspaces(A);
    if(es.size > 1)
    { std::cout << "eigenspaces were ";
      for(int i=0; i<es.size; i++)
        std::cout << es[i].size << " ";
      std::cout << std::endl;
      break;
    }
  }

  for(int i=0; i<es.size; i++)
  { WeylGroupRepresentation<coefficient> outeme;
    outeme.ownerGroup =this->ownerGroup;
    outeme.generators = this->generators;
    outeme.basis = es[i];
    out.AddOnTop(outeme.Reduced());
  }
  return out;
}

template <typename coefficient>
const ClassFunction<coefficient>& WeylGroupRepresentation<coefficient>::GetCharacter()
{ if(this->theCharacter.data.size)
    return this->theCharacter;
  this->theCharacter.G = this->ownerGroup;
  this->theCharacter.data.SetSize(this->ownerGroup->conjugacyClasses.size);
  for(int cci=0; cci < this->ownerGroup->conjugacyClasses.size; cci++)
    this->theCharacter.data[cci] = this->GetMatrixElement(this->ownerGroup->conjugacyClasses[cci][0]).GetTrace();
  return this->theCharacter;
}

template <typename coefficient>
coefficient WeylGroupRepresentation<coefficient>::GetNumberOfComponents()
{ ClassFunction<coefficient> X;
  X = GetCharacter();
  return X.Norm();
}

void WeylGroup::ComputeIrreducibleRepresentationsThomasVersion(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeIrreducibleRepresentationsThomasVersion");
  if(this->squaresFirstConjugacyClassRep.size == 0)
    this->ComputeSquares();
  this->characterTable.SetSize(0);
  this->irreps.SetSize(0);
  WeylGroupRepresentation<Rational> sr;
  this->StandardRepresentation(sr);//<-this function adds the standard representation to the list of irreps.
  std::cout << sr.GetCharacter() << std::endl;
  List<WeylGroupRepresentation<Rational> > newspaces;
  newspaces.AddOnTop(sr);
  List<WeylGroupRepresentation<Rational> > incompletely_digested;
  while((newspaces.size > 0) || (incompletely_digested.size > 0))
  { WeylGroupRepresentation<Rational> nspace;
    if(newspaces.size > 0)
      nspace = newspaces.PopLastObject();
    else
    { nspace = incompletely_digested[0];
      incompletely_digested.RemoveIndexShiftDown(0);
    }
    WeylGroupRepresentation<Rational> tspace = sr * nspace;
    tspace.theCharacter.data.size = 0;
    std::cout << "Decomposing" << tspace.GetCharacter() << std::endl;
    List<WeylGroupRepresentation<Rational> > spaces = tspace.DecomposeThomasVersion();
//      tspace = nspace * sr;
//      tspace.character.data.size = 0;
//      std::cout << "Decomposing (right tensor)" << tspace.GetCharacter() << std::endl;
//      spaces.AddListOnTop(tspace.Decomposition());
    for(int spi = 0; spi < spaces.size; spi++)
      if(spaces[spi].GetNumberOfComponents() == 1)
      { if(!characterTable.Contains(spaces[spi].GetCharacter()))
        { characterTable.AddOnTop(spaces[spi].GetCharacter());
          irreps.AddOnTop(spaces[spi]);
          newspaces.AddOnTop(spaces[spi]);
          std::cout << "we have " << irreps.size << " irreps" << std::endl;
        }
      } else
        incompletely_digested.AddOnTop(spaces[spi]);
    if(irreps.size == this->conjugacyClasses.size)
      break;
    for(int spi=0; spi<incompletely_digested.size; spi++)
      for(int ci=0; ci<characterTable.size; ci++)
        if(incompletely_digested[spi].GetCharacter().InnerProduct(characterTable[ci]) != 0)
        { std::cout << "incompletely digested " << incompletely_digested[spi].GetCharacter() << " will now be further decomposed " << std::endl;
          List<WeylGroupRepresentation<Rational> > shards = incompletely_digested[spi].DecomposeThomasVersion();
          incompletely_digested.RemoveIndexShiftDown(spi);
          for(int shi=0; shi<shards.size; shi++)
            if(shards[shi].GetNumberOfComponents() == 1)
            { if(!characterTable.Contains(shards[shi].GetCharacter()))
              { characterTable.AddOnTop(shards[shi].GetCharacter());
                irreps.AddOnTop(spaces[spi]);
                newspaces.AddOnTop(spaces[spi]);
                std::cout << "we have " << irreps.size << " irreps (got from shards)" << std::endl;
              }
            } else
              incompletely_digested.AddOnTop(shards[shi]);
          break;
        }
  }
  this->irreps.QuickSortAscending(0, &this->characterTable);
  for(int i=0; i<this->characterTable.size; i++)
  { std::cout << this->characterTable[i] << '\n';
    for(int j=0; j<this->irreps[i].generators.size; j++)
      std::cout << this->irreps[i].generators[j].ToString() << '\n';
  }
  for(int i=0; i<this->characterTable.size; i++)
  { std::cout << this->characterTable[i] << '\n';
  }
  std::cout << this->characterTable.size << std::endl;
}

// trial division is pretty good.  the factors of 2 were cleared earlier
// by masking. dividing by 3 a few times earlier and only checking
// 1 and 5 mod 6 would require more complicated iteration that +=2
void factor_integer_l(int n, List<int> f)
{ for(int i=2; i<FloatingPoint::sqrt(n); i+=2)
    if(n%i==0)
    { f.AddOnTop(i);
      factor_integer_l(n/i,f);
      return;
    }
}

/* It might be a good idea to give an error for attempts to factor 0 */
List<int> factor_integer(int n)
{ List<int> f;
  if(n==0)
    return f;
  if(n<0)
    n = -n;
  if(n==1)
    return f;
  while((n&1) == 0)
  { f.AddOnTop(2);
    n = n>>1;
  }
  factor_integer_l(n,f);
  return f;
}

int ithfactor(int i, List<int> f)
{ int acc = 1;
  for(int j=0; j<f.size; j++)
    if(i&i<<j)
      acc = acc * f[j];
  return acc;
}

template <typename coefficient>
struct DivisionResult<UDPolynomial<coefficient> > UDPolynomial<coefficient>::DivideBy(const UDPolynomial<coefficient>& divisor) const
{ struct DivisionResult<UDPolynomial<coefficient> > out;
  out.remainder = *this;
  if(data.size < divisor.data.size)
    return out;
  int r = data.size - divisor.data.size + 1;
  out.quotient.data.SetSize(r);
  for(int i=r-1; i!=-1; i--)
  { if(out.remainder.data.size - divisor.data.size != i)
    { out.quotient[i] = 0;
      continue;
    }
    UDPolynomial<coefficient> p = divisor.TimesXn(i);
    out.quotient[i] = out.remainder.data[out.remainder.data.size-1]/divisor.data[divisor.data.size-1];
    p *= out.quotient[i];
    out.remainder -= p;
  }
  return out;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator/(const UDPolynomial<coefficient>& divisor) const
{ struct DivisionResult<UDPolynomial<coefficient> > tmp = this->DivideBy(divisor);
  return tmp.quotient;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator%(const UDPolynomial<coefficient>& divisor) const
{ struct DivisionResult<UDPolynomial<coefficient> > tmp = this->DivideBy(divisor);
  return tmp.remainder;
}

template <typename coefficient>
bool UDPolynomial<coefficient>::operator==(const int other) const
{ if(other == 0)
  { if(data.size == 0)
      return true;
    return false;
  }
  if(data.size != 1)
    return false;
  if(data[0] == other)
    return true;
  return false;
}

template <typename coefficient>
//template <typename integral>
void UDPolynomial<coefficient>::ClearDenominators()
{ int acc = 1;
  for(int i=0; i<data.size; i++)
    acc = lcm(acc,data[i].GetDenominator());
  *this *= acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::FormalDerivative()
{ if(data.size < 2)
    data.size = 0;
  for(int i=1; i<data.size; i++)
    data[i-1] = i*data[i];
  data.size--;
}

template <typename coefficient>
bool UDPolynomial<coefficient>::operator<(const UDPolynomial<coefficient>& right) const
{ if(data.size < right.data.size)
    return true;
  if(data.size > right.data.size)
    return false;
  if(data.size == 0)
    return false;
  return data[data.size-1] < right.data[data.size-1];
}

template <typename coefficient>
void UDPolynomial<coefficient>::SquareFree()
{ std::cout << *this << std::endl;
  UDPolynomial<coefficient> p = FormalDerivative();
  std::cout << p << std::endl;
  UDPolynomial<coefficient> q = gcd(*this,p);
  if(q.data.size > 1)
    data = (*this/q).data;
}

template <typename coefficient>
bool space_contains(const List<Vector<coefficient> > &V, Vector<coefficient> v)
{ if(v.IsEqualToZero())
  { if(V.size == 0)
      return false;
    return true;
  }
  int i=0;
  while(true)
  { int vi = v.GetIndexFirstNonZeroCoordinate();
    while(true)
    { if(i==V.size)
        return false;
      int vii = V[i].GetIndexFirstNonZeroCoordinate();
      if(vii > vi)
        return false;
      if(vii == vi)
        break;
      i++;
    }
    v -= V[i] * v[vi]/V[i][vi];
    if(v.IsEqualToZero())
      return true;
  }
}

template<typename coefficient>
void getunion(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W, List<Vector<coefficient> >& output)
{ if (&output== &V || &output==&W)
  { List<Vector<coefficient> > newOutput;
    getunion(V, W, newOutput);
    output=newOutput;
    return;
  }
  if(V.size == 0)
  { output=W;
    return;
  }
  int d = V[0].size;
  Matrix<coefficient> M;
  M.init(V.size+W.size,d);
  for(int i=0; i<V.size; i++)
    for(int j=0; j<d; j++)
      M.elements[i][j] = V[i][j];
  for(int i=0; i<W.size; i++)
    for(int j=0; j<d; j++)
      M.elements[V.size+i][j] = W[i][j];
  M.GaussianEliminationByRows(M);
  output.SetSize(0);
  Vector<coefficient> v;
  for(int i=0; i<M.NumRows; i++)
  { v.SetSize(d);
    for(int j=0; j<d; j++)
      v[j] = M.elements[i][j];
    if(v.IsEqualToZero())
      break;
    output.AddOnTop(v);
  }
}

template<typename coefficient>
void intersection(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W, List<Vector<coefficient> >& output)
{ if((V.size==0) or (W.size==0))
  { output.SetSize(0);
    return;
  }
  int d = V[0].size;

  Matrix<coefficient> MV;
  MV.init(V.size, d);
  for(int i=0; i<V.size; i++)
    for(int j=0; j<d; j++)
      MV.elements[i][j] = V[i][j];
  List<Vector<coefficient> > Vperp;
  MV.GetZeroEigenSpaceModifyMe(Vperp);

  Matrix<coefficient> MW;
  MW.init(W.size, d);
  for(int i=0; i<W.size; i++)
    for(int j=0; j<d; j++)
      MW.elements[i][j] = W[i][j];
  List<Vector<coefficient> > Wperp;
  MW.GetZeroEigenSpaceModifyMe(Wperp);

  Matrix<coefficient> M;
  M.init(Vperp.size+Wperp.size,d);
  int i=0;
  for(; i<Vperp.size; i++)
    for(int j=0; j<d; j++)
      M.elements[i][j] = Vperp[i][j];
  for(; i<Vperp.size+Wperp.size; i++)
    for(int j=0; j<d; j++)
      M.elements[i][j] = Wperp[i-Vperp.size][j];
  M.GetZeroEigenSpaceModifyMe(output);
}

template <typename coefficient>
List<Vector<coefficient> > orthogonal_complement(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &WW)
{ List<Vector<coefficient> > W = intersection(V,WW);
  if(W.size==0)
    return V;
  int d = W[0].size;
  Matrix<coefficient> GM;
  GM.init(W.size, W.size);
  for(int i=0; i<W.size; i++)
    for(int j=0; j<W.size; j++)
      GM.elements[i][j] = W[i].ScalarEuclidean(W[j]);
  GM.Invert();
  Matrix<coefficient> VM;
  VM.init(W.size,V.size);
  for(int i=0; i<W.size; i++)
    for(int j=0; j<V.size; j++)
      VM.elements[i][j] = W[i].ScalarEuclidean(V[j]);
  VM.MultiplyOnTheLeft(GM);
  Matrix<coefficient> outm;
  outm.init(V.size, d);
  for(int i=0; i<V.size; i++)
  { for(int j=0; j<d; j++)
    { coefficient r = V[i][j];
      outm.elements[i][j] = r;
      for(int k=0; k<W.size; k++)
        outm.elements[i][j] -= VM.elements[k][i] * W[k][j];
    }
  }
  Matrix<coefficient> tmp;
  Selection s;
  outm.GaussianEliminationByRows(outm,tmp,s);
  List<Vector<coefficient> > out;
  for(int i=0; i<outm.NumRows; i++)
  { Vector<coefficient> v;
    v.SetSize(d);
    for(int j=0; j<d; j++)
      v[j] = outm.elements[i][j];
    if(v.IsEqualToZero())
      return out;
    out.AddOnTop(v);
  }
  return out;
}

// we're going to guess at integers
template <typename coefficient>
bool pdiv(List<coefficient> &p, int a)
{ List<coefficient> q;
  coefficient lastround = p[0];
  for(int i=1; i<p.size; i++)
  { q.AddOnTop(lastround);
    lastround = p[i] - lastround*a;
  }
  if(lastround == 0)
  { p = q;
    return true;
  }
  return false;
}

template <typename coefficient>
List<int> factorpoly(List<coefficient> p, int maxfac)
{ List<int> factors;
  for(int i1=0; i1<maxfac; i1++)
    for(int i2=1; i2>=-1; i2-=2)
    { int i = i1*i2;
      while(pdiv(p,i))
      { if(!factors.Contains(i))
          factors.AddOnTop(i);
        if(p.size == 1)
          return factors;
      }
    }
  return factors;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveColumnSpace(Matrix<coefficient>& M)
{  M.Transpose();
   Matrix<coefficient> dummy1;
   Selection dummy2;
   M.GaussianEliminationByRows(M,dummy1,dummy2);
   List<Vector<coefficient> > out;
   bool zerov;
   for(int i=0; i<M.NumRows; i++)
   {  Vector<coefficient> v;
      v.MakeZero(M.NumCols); // initializing is not necessary
      zerov = true;
      for(int j=0; j<M.NumCols; j++)
      {  v[j] = M.elements[i][j];
         if(zerov && M.elements[i][j]!=0)
            zerov = false;
      }
      if(zerov)
         return out;
      out.AddOnTop(v);
   }
}

// guess at integers
List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational> &M, int checkDivisorsOf=0)
{ int n = M.NumCols;
  List<List<Vector<Rational> > > spaces;
  int found = 0;
// for(int i=0; found < n; i++){
//   if((i!=0) && (checkDivisorsOf%i!=0))
//     continue;
  UDPolynomial<Rational> p;
  p.AssignMinPoly(M);
  for(int ii=0; ii<2*n+2; ii++) // lol, this did end up working though
  { int i = ((ii+1)/2) * (2*(ii%2)-1); // 0,1,-1,2,-2,3,-3,...
//    std::cout << "checking " << i << " found " << found << std::endl;
    Rational r = i;
    if(p(r) == 0)
    { Matrix<Rational> M2 = M;
      List<Vector<Rational> > V;
      M2.GetEigenspaceModifyMe(r, V);
      found += V.size;
      spaces.AddOnTop(V);
      if(found == M.NumCols)
        break;
    }
  }
  return spaces;
}

template <typename coefficient>
Vector<coefficient> PutInBasis(const Vector<coefficient> &v, const List<Vector<coefficient> > &B)
{  Vector<coefficient> w;
   w.MakeZero(B.size);
   Matrix<coefficient> M;
   M.MakeZeroMatrix(B.size);
   for(int i=0; i<B.size; i++)
   {  for(int j=0; j<v.size; j++)
         w[i] += B[i][j]*v[j];
      for(int j=0; j<B.size; j++)
         for(int k=0; k<v.size; k++)
            M.elements[i][j] += B[i][k] * B[j][k];
   }
   M.Invert();
   Vector<coefficient> v2 = M*w;
   Vector<coefficient> v3;
   v3.MakeZero(v.size);
   for(int i=0; i<B.size; i++)
      v3 += B[i] * v2[i];
   if(!(v3 == v))
   {  Vector<coefficient> out;
      return out;
   }
   return v2;
}

template <typename coefficient>
ElementWeylGroupRing<coefficient> ActOnGroupRing(const WeylGroup& G, int g, const ElementWeylGroupRing<coefficient>& v)
{ ElementWeylGroupRing<coefficient> out;
  out.MakeZero();
  for(int i=0; i<v.size(); i++)
    out.AddMonomial(v.theCoeffs[i], G.theElements[g]*v[i]);
  return out;
}

// this function name is a lie
template <typename coefficient>
bool is_isotypic_component(WeylGroup& G, const List<Vector<coefficient> >& V)
{ // pre-initial test: V isn't empty
  if(V.size == 0)
    return false;
  // initial test: dimension of V is a square
  int n = sqrt(V.size);
  if(n*n != V.size)
    return false;
  // more expensive test: character of V has unit Norm
  ClassFunction<coefficient> X;
  X.G = &G;
  X.data.SetSize(G.conjugacyClasses.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
  {  int g = G.conjugacyClasses[i][0];
    coefficient tr = 0;
    for(int j=0; j<V.size; j++)
    {  Vector<coefficient> v = ActOnGroupRing(G,g,V[j]);
        v = PutInBasis(v,V);
        tr += v[j];
    }
    X.data[i] = tr;
  }
  // (4, -1, -1/2, 1/2, -1, 1/4, 1/4, 1, 1/2, -7/2)[1] haha yeah right
  if(X.Norm() != n)
    return false;
  // okay now do the really hard test
  Matrix<coefficient> M = GetMatrix(X);
  List<List<Vector<coefficient> > > spaces = eigenspaces(M);
  if(spaces.size != 2)
    return false;
  if(spaces[1].size != V.size)
    return false;
  if(intersection(spaces[1],V).size != V.size)
    return false;
  return true;
}

Matrix<Rational> MatrixInBasis(const ClassFunction<Rational>& X, const List<Vector<Rational> >& B)
{ List<Vector<Rational> > rows;
  for(int i=0; i<B.size; i++)
  { Vector<Rational> v;
    v.MakeZero(B[0].size);
    rows.AddOnTop(v);
  }
  for(int i1 =0; i1<X.G->conjugacyClasses.size; i1++)
    for(int i2=0; i2<X.G->conjugacyClasses[i1].size; i2++)
      for(int j=0; j<X.G->size(); j++)
        for(int k=0; k<B.size; k++)
        { int l = X.G->MultiplyElements(X.G->conjugacyClasses[i1][i2],j);
          rows[k][l] = X.data[i1]*B[k][j];
        }
  Matrix<Rational> M;
  M.init(rows.size, rows.size);
  for(int i=0; i<rows.size; i++)
  { Vector<Rational> v = PutInBasis(rows[i],B);
    for(int j=0; j<rows.size; j++)
      M.elements[i][j] = v[j];
  }
  return M;
}

ElementMonomialAlgebra<ElementWeylGroup<WeylGroup>, Rational> FromClassFunction(const ClassFunction<Rational>& X)
{ ElementMonomialAlgebra<ElementWeylGroup<WeylGroup>, Rational> out;
  for(int i=0; i<X.G->conjugacyClasses.size; i++)
    for(int j=0; j<X.G->conjugacyClasses[i].size; j++)
      out.AddMonomial(X.G->theElements[X.G->conjugacyClasses[i][j]],X.data[i]);
  return out;
}


template <typename coefficient>
Matrix<coefficient> GetMatrix(const ClassFunction<coefficient>& X)
{  Matrix<coefficient> M;
   M.MakeZeroMatrix(X.G->N);
   for(int i1=0; i1<X.G->ccCount; i1++)
   {  for(int i2=0; i2<X.G->ccSizes[i1]; i2++)
      {  int i=X.G->conjugacyClasses[i1][i2];
         for(int j=0; j<X.G->N; j++)
         {  M.elements[X.G->MultiplyElements(i,j)][j] = X.data[i1];
         }
      }
   }
   return M;
}

bool WeylGroup::VerifyChartable(bool printresults)const
{ bool okay = true;
  if(printresults)
    std::cout << "one" << '\n';
  for(int i=0; i<this->conjugacyClasses.size; i++)
  { if(printresults)
      std::cout << this->characterTable[i].Norm() << std::endl;
    if(this->characterTable[i].Norm() != 1)
      okay = false;
  }
  if(printresults)
    std::cout << "zero" << '\n';
  for(int i=0; i<this->ConjugacyClassCount(); i++)
    for(int j=0; j<this->ConjugacyClassCount(); j++)
    { if(j==i)
        continue;
      if(this->characterTable[i].InnerProduct(this->characterTable[j]) != 0)
        okay = false;
      if(printresults)
        std::cout << this->characterTable[i].InnerProduct(this->characterTable[j]) << std::endl;
    }
  return okay;
}

List<List<Rational> > WeylGroup::GetTauSignatures()
{ MacroRegisterFunctionWithName("WeylGroup::GetTauSignatures");
  this->ComputeIrreducibleRepresentationsThomasVersion();
  ClassFunction<Rational> signRep;
  signRep.G = this;
  signRep.data.SetSize(this->ConjugacyClassCount());
  for(int i=0; i<this->ConjugacyClassCount(); i++)
    signRep.data[i] =this->theElements[conjugacyClasses[i][0]].Sign();
  Selection sel;
  sel.init(this->CartanSymmetric.NumCols);
  int numCycles=MathRoutines::TwoToTheNth(sel.MaxSize);

  List<WeylGroup> PSGs;
  PSGs.SetSize(numCycles-2);
  List<List<int> > ccBackMaps;
  ccBackMaps.SetSize(PSGs.size);
  ElementWeylGroup<WeylGroup> g;
  g.owner = this;

  for (int i=0; i<PSGs.size; i++)
  { sel.incrementSelection();
    std::cout << "\n" << sel.ToString() << "\n";
    int d = sel.CardinalitySelection;
    PSGs[i].CartanSymmetric.init(d,d);
    for(int ii=0; ii<d; ii++)
      for(int jj=0; jj<d; jj++)
        PSGs[i].CartanSymmetric.elements[ii][jj] = this->CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];
    std::cout << PSGs[i].CartanSymmetric.ToString();
    // ComputeInitialCharacters gets the character of the sign representation
    // as characterTable[1]
    PSGs[i].ComputeInitialCharacters();

    ccBackMaps[i].SetSize(PSGs[i].conjugacyClasses.size);
    for(int j=0; j<PSGs[i].conjugacyClasses.size; j++)
    { ElementWeylGroup<WeylGroup> h = PSGs[i].theElements[PSGs[i].conjugacyClasses[j][0]];
      g.generatorsLastAppliedFirst.SetSize(h.generatorsLastAppliedFirst.size);
      for(int k=0; k<h.generatorsLastAppliedFirst.size; k++)
        g.generatorsLastAppliedFirst[k] = sel.elements[h.generatorsLastAppliedFirst[k]];
      int gi = this->theElements.GetIndex(g);
      bool notFound=true;
      for(int k=0; notFound && k<this->conjugacyClasses.size; k++)
        for(int l=0; notFound && l<this->conjugacyClasses[k].size; l++)
          if(this->conjugacyClasses[k][l] == gi)
          { ccBackMaps[i][j] = k;
            notFound=false;
          }
    }
    std::cout << ccBackMaps[i] << std::endl;
  }
  List<List<Rational> > tauSignatures;
  tauSignatures.SetSize(this->ConjugacyClassCount());
  ClassFunction<Rational> Xi; // this will be scribbled on over and over
  Xi.data.SetSize(this->ConjugacyClassCount()); // too big, but only allocated once
  for(int i=0; i<this->ConjugacyClassCount(); i++)
  { tauSignatures[i].SetSize(numCycles);
    tauSignatures[i][0] = true;
    for(int j=0; j<PSGs.size; j++)
    { for(int k=0; k<PSGs[j].ConjugacyClassCount(); k++)
        Xi.data[k] = this->characterTable[i].data[ccBackMaps[j][k]];
      Rational x = PSGs[j].characterTable[1].InnerProduct(Xi);
      if(x == 0)
        tauSignatures[i][j+1] = false;
      else
        tauSignatures[i][j+1] = true;
    }
    if(signRep.InnerProduct(this->characterTable[i]) == 0)
      tauSignatures[i][numCycles-1] = false;
    else
      tauSignatures[i][numCycles-1] = true;

    std::cout << tauSignatures[i] << std::endl;
  }
  return tauSignatures;
}
