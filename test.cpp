#include "vpf.h"
// ^ that is first for precompilation

#include "vpfCharacters.h"
#include "vpfGraph.h"
//#include "testlib.h"
#include <iostream>

// this is one reason test.cpp isn't even compiled into the actual calculator
FormatExpressions testformat;

List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational> &M, int checkDivisorsOf=0);

template <typename coefficient>
List<VectorSpace<coefficient> > GetEigenspaces(const Matrix<coefficient> &M)
{ List<List<Vector<coefficient> > > es = eigenspaces(M);
  List<VectorSpace<coefficient> > vs;
  for(int spi=0; spi<es.size; spi++)
  { VectorSpace<coefficient> V;
    for(int j=0; j<es[spi].size; j++)
      V.AddVector(es[spi][j]);
    vs.AddOnTop(V);
  }
  return vs;
}

template <typename coefficient>
List<VectorSpace<coefficient> > GetLeftEigenspaces(Matrix<coefficient> M)
{ M.Transpose();
  return GetEigenspaces(M);
}

// As in Schneider, 1990
// well, so far not using any of his improvements
template <typename somegroup>
List<Vector<Rational> > ComputeCharacterTable(somegroup &G)
{ if(G.conjugacyClasses.size == 0)
    G.ComputeConjugacyClasses();
  List<int> classmap;
  classmap.SetSize(G.theElements.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
    for(int j=0; j<G.conjugacyClasses[i].size; j++)
      classmap[G.conjugacyClasses[i][j]] = i;
  Matrix<Rational> form;
  form.MakeZeroMatrix(G.conjugacyClasses.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
    form.elements[i][i] = G.conjugacyClasses[i].size;
  List<VectorSpace<Rational> > spaces;
  VectorSpace<Rational> bigspace;
  bigspace.MakeFullRank(G.conjugacyClasses.size);
  spaces.AddOnTop(bigspace);
  for(int i=0; i<G.conjugacyClasses.size; i++)
  { Matrix<Rational> M;
    std::cout << "Getting class matrix " << i << std::endl;
    M = GetClassMatrix(G,i,&classmap);
    List<VectorSpace<Rational> > es = GetEigenspaces(M);
    for(int esi=0; esi<es.size; esi++)
    { int spsize = spaces.size;
      for(int spi=0; spi<spsize; spi++)
      { if(spaces[spi].rank == 1)
          continue;
        VectorSpace<Rational> V = spaces[spi].Intersection(es[esi]);
        if((V.rank > 0) and (V.rank < spaces[spi].rank))
        { VectorSpace<Rational> W = es[esi].OrthogonalComplement(&spaces[spi],&form);
          spaces[spi] = V;
          spaces.AddOnTop(W);
          if(spaces.size == G.conjugacyClasses.size)
            goto got_chars;
        }
      }
    }
  }
  got_chars:
  List<Vector<Rational> > chars;
  chars.SetSize(spaces.size);
  for(int i=0; i<spaces.size; i++)
    chars[i] = spaces[i].GetCanonicalBasisVector(0);
/*  HashedList<VectorSpace<Rational> > sps;
  List<Vector<Rational> > chars;
  for(int i=0; i<G.conjugacyClasses.size; i++)
  { Matrix<Rational> M;
    M = GetClassMatrix(G,i);
    std::cout << M.ToString(&testformat) << std::endl;
    List<VectorSpace<Rational> > spsi = GetEigenspaces(M);
    for(int spi=0; spi<spsi.size; spi++)
      sps.AddOnTop(spsi[spi]);
  }
  for(int i=0; i<sps.size; i++)
    if(sps[i].rank == 1){
      Vector<Rational> X = sps[i].GetCanonicalBasisVector(0);
      if(!chars.Contains(X))
      { chars.AddOnTop(sps[i].GetCanonicalBasisVector(0));
        if(chars.size == G.conjugacyClasses.size)
          goto got_chars;
      }
    }
  std::cout << chars << std::endl;
  { // to scope newspaces so that the compiler doesn't complain about the goto
    // skipping its initialization
  List<VectorSpace<Rational> > newspaces = sps;
  while(newspaces.size > 0)
  { VectorSpace<Rational> sp = newspaces.PopLastObject();
    for(int i=0; i<sps.size; i++)
    { VectorSpace<Rational> sp2 = sp.Intersection(sps[i]);
      if(sps.GetIndex(sp2) == -1)
      { sps.AddOnTop(sp2);
        newspaces.AddOnTop(sp2);
        if(sp2.rank == 1)
        { Vector<Rational> X = sp2.GetCanonicalBasisVector(0);
          if(!chars.Contains(X))
          { chars.AddOnTop(X);
            std::cout << chars.size << ' ';
            if(chars.size == G.conjugacyClasses.size)
            { goto got_chars;
            }
          }
        }
      }
    }
  }
  }
  got_chars:
  */
  std::cout << chars << std::endl;
  for(int i=0; i<chars.size; i++)
  { Rational x = G.GetHermitianProduct(chars[i],chars[i]);
    int x2 = x.GetDenominator().GetUnsignedIntValueTruncated();
    x2 = sqrt(x2);
    chars[i] *= x2;
    if(chars[i][0] < 0)
      chars[i] *= -1;
    std::cout << x2 << std::endl;
  }
  chars.QuickSortAscending();
  return chars;
}

template <typename somegroup>
Matrix<Rational> GetClassMatrix(const somegroup &G, int cci, List<int>* classmap = 0)
{ List<int> invl;
  invl.SetSize(G.conjugacyClasses[cci].size);
  for(int i=0; i<G.conjugacyClasses[cci].size; i++)
    invl[i] = G.Invert(G.conjugacyClasses[cci][i]);
  Matrix<int> M;
  M.MakeZeroMatrix(G.conjugacyClasses.size);
  for(int t=0; t<G.conjugacyClasses.size; t++)
    for(int xi=0; xi<invl.size; xi++)
    { int yi = G.Multiply(invl[xi],G.conjugacyClasses[t][0]);
      int ci;
      if(classmap)
        ci = (*classmap)[yi];
      else
      { for(ci=0; ci<G.conjugacyClasses.size; ci++)
          for(int cj=0; cj<G.conjugacyClasses[ci].size; cj++)
            if(G.conjugacyClasses[ci][cj] == yi)
              goto okaddit;
      }
      okaddit: M.elements[t][ci] += 1;
    }
  Matrix<Rational> out;
  out.init(M.NumRows, M.NumCols);
  for(int i=0; i<M.NumRows; i++)
    for(int j=0; j<M.NumCols; j++)
      out.elements[i][j] = M.elements[i][j];
  return out;
}



template <typename element>
class FiniteGroup
{ public:
  List<element> theElements;
  List<int> lengths; // sure why not
  bool MakeFrom(const List<element>& generators, int MaxElements=60000);

  List<int> generators;
  List<List<int> > conjugacyClasses;
  void ComputeConjugacyClasses();
  void GetSignCharacter(Vector<Rational>& out);
  Rational GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const;
};

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

void GetTauSignaturesFromSubgroup(const WeylGroup& G, const List<int>& gens, List<bool>& out)
{ List<ElementWeylGroup> genes;
  genes.SetSize(gens.size);
  for(int i=0; i<gens.size; i++)
    genes[i] = G.theElements[gens[i]];
  FiniteGroup<ElementWeylGroup> H;
  H.MakeFrom(genes);
  Vector<Rational> HXs;
  H.GetSignCharacter(HXs);

  List<int> ccPreimages;
  ccPreimages.SetSize(H.conjugacyClasses.size);
  for(int i=0; i<H.conjugacyClasses.size; i++)
  { int gi = G.theElements.GetIndex(H.theElements[H.conjugacyClasses[i][0]]);
    for(int ci=0; ci<G.conjugacyClasses.size; ci++)
      for(int cj=0; cj<G.conjugacyClasses[ci].size; cj++)
        if(G.conjugacyClasses[ci][cj] == gi)
        { ccPreimages[i] = ci;
          goto endloop;
        }
    endloop:;
  }

  out.SetSize(G.irreps.size);
  Vector<Rational> HXi;
  HXi.SetSize(H.conjugacyClasses.size);
  for(int i=0; i<G.irreps.size; i++)
  { Vector<Rational> GXi = G.irreps[i].GetCharacter();
    for(int j=0; j<HXi.size; j++)
      HXi[j] = GXi[ccPreimages[j]];
    if(H.GetHermitianProduct(HXs,HXi) == 0)
      out[i] = false;
    else
      out[i] = true;
  }
}



class WeylSubgroup: public WeylGroup
{
  public:
  WeylGroup *parent;
  List<int> generatorPreimages;
  List<int> ccPreimages;
  List<bool> tauSignature;
  void ComputeTauSignature();
};


void ParabolicSubgroup(WeylGroup* G, const Selection sel, WeylSubgroup& out)
{ out.init();
  out.parent = G;
  int d = sel.CardinalitySelection;
  out.CartanSymmetric.init(d,d);
  for(int ii=0; ii<d; ii++)
      for(int jj=0; jj<d; jj++)
        out.CartanSymmetric.elements[ii][jj] = G->CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];
  out.generatorPreimages.SetSize(d);
  for(int i=0; i<d; i++)
    out.generatorPreimages[i] = sel.elements[i] + 1;
}

// this is incorrect.  Do not use it.
void PseudoParabolicSubgroupNope(WeylGroup* G, const Selection& sel, WeylSubgroup& out)
{ out.init();
  out.parent = G;
  int d = sel.CardinalitySelection+1;
  out.generatorPreimages.SetSize(d);
  // not sure if this is actually how this works
  for(int i=0; i<d-1; i++)
   out.generatorPreimages[i] = i+1;

  out.CartanSymmetric.init(d,d);
  for(int ii=0; ii<d-1; ii++)
    for(int jj=0; jj<d-1; jj++)
      out.CartanSymmetric.elements[ii][jj] = G->CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];

  // the highest root was the last one in the RootSystem
  Vector<Rational> hr = G->RootSystem[G->RootSystem.size-1];

  for(int i=0; i<d-1; i++)
  { Vector<Rational> v;
    G->CartanSymmetric.GetVectorFromRow(sel.elements[i],v);
    Rational x = v.ScalarEuclidean(hr);
    out.CartanSymmetric.elements[i][d-1] = x;
    out.CartanSymmetric.elements[d-1][i] = x;
  }
  out.CartanSymmetric.elements[d-1][d-1] = hr.ScalarProduct(hr,G->CartanSymmetric);
  out.generatorPreimages[d-1] = G->GetRootReflection(G->RootSystem.size-1);
}

void WeylSubgroup::ComputeTauSignature()
{ if(this->ccPreimages.size == 0)
  { this->ccPreimages.SetSize(this->conjugacyClasses.size);
    for(int i=0; i<this->conjugacyClasses.size; i++)
    { ElementWeylGroup g;
      g.owner = this->parent;
      for(int j=0; j<this->theElements[this->conjugacyClasses[i][0]].size; j++)
        g.AddListOnTop(this->parent->theElements[this->generatorPreimages[this->theElements[this->conjugacyClasses[i][0]][j]]]);
      g.MakeCanonical();
      int gi = this->parent->theElements.GetIndex(g);
      for(int ci=0; ci<this->parent->conjugacyClasses.size; ci++)
        for(int cj=0; cj<this->parent->conjugacyClasses[ci].size; cj++)
          if(this->parent->conjugacyClasses[ci][cj] == gi)
          { this->ccPreimages[i] = ci;
            goto endloop;
          }
      endloop:;
    }
  }
  Vector<Rational> Xs;
  this->GetSignCharacter(Xs);
  Vector<Rational> Xi;
  Xi.SetSize(this->conjugacyClasses.size);
  this->tauSignature.SetSize(this->parent->irreps.size);
  for(int i=0; i<this->parent->irreps.size; i++)
  { Vector<Rational> Xip = this->parent->irreps[i].GetCharacter();
    for(int j=0; j<Xi.size; j++)
      Xi[j] = Xip[this->ccPreimages[j]];
    if(this->GetHermitianProduct(Xs,Xi) == 0)
      this->tauSignature[i] = false;
    else
      this->tauSignature[i] = true;
  }
}

void AllTauSignatures(WeylGroup* G, List<List<bool> >& tauSignatures, bool pseudo=false)
{ Selection sel;
  sel.init(G->CartanSymmetric.NumCols);
  int numCycles=MathRoutines::TwoToTheNth(sel.MaxSize);
  List<List<bool> > tss;
  tss.SetSize(numCycles-2);
  for(int i=0; i<numCycles-2; i++)
  { sel.incrementSelection();
    WeylSubgroup H;
    ParabolicSubgroup(G,sel,H);
    H.ComputeConjugacyClasses();
    H.ComputeTauSignature();
    tss[i] = H.tauSignature;
    std::cout << H.CartanSymmetric.ToString(&testformat) << std::endl;
  }


  Vector<Rational> Xs;
  G->GetSignCharacter(Xs);
  List<bool> tsg;
  tsg.SetSize(G->irreps.size);
  for(int i=0; i<G->irreps.size; i++)
    tsg[i] =  G->irreps[i].GetCharacter() == Xs;
  tss.AddOnTop(tsg);

  if(pseudo){
    std::cout << "pseudo-parabolics" << std::endl;
    int hr = G->GetRootReflection(G->RootSystem.size-1);
    sel.init(G->CartanSymmetric.NumCols);
    for(int i=0; i<numCycles-1; i++)
    { List<int> gens;
      for(int j=0; j<sel.CardinalitySelection; j++)
        gens.AddOnTop(sel.elements[j]+1);
      gens.AddOnTop(hr);
      List<bool> ts;
      GetTauSignaturesFromSubgroup(*G,gens,ts);
      tss.AddOnTop(ts);
      sel.incrementSelection();
    }
  }

  // we will need the sign character for the group


  tauSignatures.SetSize(G->irreps.size);
  for(int i=0; i<G->irreps.size; i++)
  { tauSignatures[i].SetSize(tss.size+1);
    tauSignatures[i][0] = 1;
    for(int j=1; j<tss.size+1; j++)
      tauSignatures[i][j] = tss[j-1][i];
  }

  for(int i=0; i<G->irreps.size; i++)
  { std::cout << G->irreps[i].GetCharacter() << std::endl;
    for(int j=0; j<tauSignatures[i].size; j++)
      std::cout << tauSignatures[i][j] << ' ';
    std::cout << std::endl;
  }
}



/*
  for(int j=0; j<PSGs[i].ccCount; j++)
  { CoxeterElement h = PSGs[i].GetCoxeterElement(PSGs[i].conjugacyClasses[j][0]);
    g.reflections.SetSize(h.reflections.size);
    for(int k=0; k<h.reflections.size; k++)
      g.reflections[k] = sel.elements[h.reflections[k]];
    int gi = this->GetIndexOfElement(g);
    for(int k=0; k<this->ccCount; k++)
      for(int l=0; l<this->conjugacyClasses[k].size; l++)
        if(this->conjugacyClasses[k][l] == gi)
        { ccBackMaps[i][j] = k;
          goto endloop;
        }
    endloop:;
  }
*/


/* dunno why i thought it was fun to spend so much time on this
class f127
{ public:
  int n;

  f127 operator+(const f127 right) const;
  void operator+=(const f127 right);
  void operator-();
  f127 operator-(const f127 right) const;
  void operator-=(const f127 right);
  f127 operator*(const f127 right) const;
  void operator*=(const f127 right);
  void Invert();
  f127 operator/(const f127 right) const;
  void operator/=(const f127 right);
  void operator=(const int rhs);
}

f127 f127::operator+(const f127 right) const
{ return (n+right.n)%127;
}

void f127::operator+=(const f127 right)
{ n += right.n;
  n %= 127;
}

f127 f127::operator-(const f127 right) const
{ int x = (n-right.n)%127;
  f127 out;
  out.n = x>=0?x:-x;
  return out;
}

void f127::operator-=(const f127 right)
{ n -= right.n;
  n %= 127;
  if(n<0)
    n = -n;
}

f127 f127::operator*(const f127 right) const
{ return (n*right.n)%127;
}

void f127::operator*=(const f127 right)
{ n *= right.n;
  n %= 127;
}

void f127::Invert()
{ int x = 0;
  int y = 1;
  int a = 127;
  int b = n;
  int m = 1
  n = 0;
  while(b != 0)
  { int q = a / b;
    int r = a % b;
    a = b;
    b = r;
    int s = m - q*x;
    int t = n - q*y;
    m = x;
    n = y;
    x = s;
    y = t;
  }
}

f127 operator/(const f127 right) const;
{ tmp = right;
  tmp.Invert();
  return *this *tmp;
}

void operator/=(const f127 right)
{ tmp = right;
  tmp.Invert();
  *this *= tmp;
}
*/





/*
template <typename coefficient>
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition(List<ClassFunction<coefficient> >& ct, List<CoxeterRepresentation<coefficient> > &gr)
{  List<CoxeterRepresentation<coefficient> > out;
   if(GetNumberOfComponents() == 1)
   { if(ct.GetIndex(character) == -1)
     { std::cout << "new irrep found, have " << ct.size << std::endl;
       ct.AddOnTop(character);
       gr.AddOnTop(*this);
     }
     out.AddOnTop(*this);
     return out;
   }
   List<Vector<coefficient> > Vb = basis;
   for(int i=0; i<ct.size; i++)
   { if(character.IP(ct[i])!=0)
     { std::cout << "contains irrep " << i << std::endl;
       Matrix<coefficient> M = ClassFunctionMatrix(ct[i]);
       Vb = intersection(Vb,DestructiveKernel(M));
     }
   }
   if((Vb.size < basis.size) && (Vb.size > 0))
   { CoxeterRepresentation<coefficient> V;
     V.G = G;
     V.gens = gens;
     V.classFunctionMatrices = classFunctionMatrices;
     V.basis = Vb;
     V = V.Reduced();
     std::cout << "Decomposing remaining subrep " << V.GetCharacter() << std::endl;
     return V.Decomposition(ct,gr);
   }
   if(Vb.size == 0)
    return out;
   SpaceTree<Rational> st;
   st.space = basis;
   for(int cfi=0; cfi<G->ccCount; cfi++)
   {  ClassFunction<coefficient> cf;
      cf.G = G;
      cf.MakeZero();
      cf[cfi] = 1;
      std::cout << "getting matrix " << cf << std::endl;
      Matrix<coefficient> A = ClassFunctionMatrix(cf);
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

   for(int i=0; i<leaves.size; i++)
   {  CoxeterRepresentation<coefficient> outeme;
      outeme.G = G;
      outeme.gens = gens;
      outeme.classFunctionMatrices = classFunctionMatrices;
      outeme.basis = leaves[i];
      out.AddOnTop(outeme.Reduced());
   }
   return out;
}
*/

/*template <typename coefficient>
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition() const
{ List<CoxeterRepresentation<coefficient> > out;
  if(GetNumberOfComponents() == 1)
  { out.AddOnTop(*this);
    return out;
  }
  List<List<Vector<coefficient> > > spaces;
  spaces.AddOnTop(basis);
  List<bool> is_component;
  is_component.AddOnTop(false);
  int dim_components = 0;
  for(int cfi=0; cfi<G->ccCount; cfi++)
  { if(dim_components == basis.size)
      break;
    ClassFunction<coefficient> cf;
    cf.G = G;
    cf.MakeZero();
    cf[cfi] = 1;
    std::cout << "getting matrix" << cf << std::endl;
    Matrix<coefficient> A = ClassFunctionMatrix(cf);
    std::cout << A.ToString(&testformat) << std::endl;
    std::cout << "getting eigenspaces" << std::endl;
    List<List<Vector<coefficient> > > es = eigenspaces(A);
    for(int i=0; i<es.size; i++)
    { if(dim_components == basis.size)
        break;
      int spaces_to_check = spaces.size;
      for(int j=0; j<spaces_to_check; j++)
      { if(is_component[j])
          continue;
        List<Vector<coefficient> > middle = intersection(es[i],spaces[j]);
        if(!((0<middle.size)&&(middle.size<spaces[j].size)))
          continue;
        std::cout << "eigenspace " << i << " intersects with space " << j << std::endl;
        List<Vector<coefficient> > right = relative_complement(spaces[j],middle);
        spaces[j] = middle;
        spaces.AddOnTop(right);
        CoxeterRepresentation<coefficient> midr;
        midr.G = G;
        midr.gens = gens;
        midr.basis = middle;
        if(midr.GetNumberOfComponents() == 1)
        { is_component[j] = true;
          dim_components += middle.size;
        }
        CoxeterRepresentation<coefficient> rightr;
        rightr.G = G;
        rightr.gens = gens;
        rightr.basis = right;
        if(rightr.GetNumberOfComponents() == 1)
        { is_component.AddOnTop(true);
          dim_components += right.size;
        } else
        { is_component.AddOnTop(false);
        }
      }
    }
  }
  std::cout << "spaces.size " << spaces.size << std::endl;
  for(int i=0; i<spaces.size; i++)
  { CoxeterRepresentation<coefficient> s;
    s.G = this->G;
    s.gens = this->gens;
    s.basis = spaces[i];
    out.AddOnTop(s);
  }
  std::cout << "decomposition might be complete, found " << out.size << " components" << std::endl;
  return out;
}*/


/*
template <typename coefficient>
class polynom1al
{
  public:
  List<coefficient> data;


  void AddMonomial(coefficient c, int i)
  { while(this->data.size < i+1)
    { this->data.AddOnTop(0);
    }
    this->data[i] += c;
  }


  // if only cxx had lisp macros...
  polynom1al<coefficient> operator+(const polynom1al &right) const
  { polynom1al<coefficient> out;
    out.data.SetSize((right.data.size>this->data.size)?right.data.size:this->data.size);
    int i=0;
    for(; i<this->data.size && i<right.data.size; i++)
      out.data[i] = this->data[i] + right.data[i];
    for(; i<this->data.size; i++)
      out.data[i] = this->data[i];
    for(; i<right.data.size; i++)
      out.data[i] = right->data[i];
    return out;
  }

  polynom1al<coefficient> operator-(const polynom1al &right) const
  { polynom1al<coefficient> out;
    out.data.SetSize((right.data.size>this->data.size)?right.data.size:this->data.size);
    int i=0;
    for(; i<this->data.size && i<right.data.size; i++)
      out.data[i] = this->data[i] - right.data[i];
    for(; i<this->data.size; i++)
      out.data[i] = this->data[i];
    for(; i<right.data.size; i++)
      out.data[i] = -right->data[i];
    return out;
  }

  polynom1al<coefficient> operator*(const polynom1al &right) const
  { polynom1al<coefficient> out;
    out.data.SetSize(tmp.data.size*right.data.size);
    for(int i=0; i<this->data.size; i++)
      for(int j=0; j<right.data.size; j++)
        out.data[i*j] = this->data[i]*right.data[j];
    return out;
  }
  void operator*=(const polynom1al &right)
  { polynom1al<coefficient> tmp = *this;
    this->data.SetSize(tmp.data.size*right.data.size)
    for(int i=0; i<tmp.data.size; i++)
      for(int j=0; j<right.data.size; j++)
        this->data[i*j] = tmp.data[i]*right.data[j];
  }

  void Invert()
  { for(int i=0; i<this->data.size; i++)
      this->data[i].Invert();
  }

  bool IsEqualToZero()
  { return this->data.size == 0;
  }


  static List<polynom1al<coefficient> > LagrangeInterpolants(List<int> points)
  {

  }

  polynom1al(){}
  polynom1al(int c){this->data.AddOnTop(c);}


  void operator=(int right)
  { this->data.SetSize(1);
    this->data[0] = right;

  }
  void operator=(coefficient right)
  { this->data.SetSize(1);
    this->data[0] = right;
  }

};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const polynom1al<coefficient>& p)
{ out << p.data;
  return out;
}
*/
template <typename coefficient>
bool space_contains(const List<Vector<coefficient> > &V, Vector<coefficient> v)
{  if(v.IsEqualToZero())
   {  if(V.size == 0)
         return false;
      return true;
   }
   int i=0;
   while(true)
   {  int vi = v.GetIndexFirstNonZeroCoordinate();
      while(true)
      {  if(i==V.size)
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
List<Vector<coefficient> > getunion(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W)
{  if(V.size == 0)
      return W;
   int d = V[0].size;
   Matrix<coefficient> M;
   M.init(V.size+W.size,d);
   for(int i=0; i<V.size; i++)
      for(int j=0; j<d; j++)
         M.elements[i][j] = V[i][j];
   for(int i=0; i<W.size; i++)
      for(int j=0; j<d; j++)
         M.elements[V.size+i][j] = W[i][j];
   Matrix<coefficient> one;
   Selection two;
   M.GaussianEliminationByRows(M,one,two);
   List<Vector<coefficient> > out;
   for(int i=0; i<M.NumRows; i++)
   {  Vector<coefficient> v;
      v.SetSize(d);
      for(int j=0; j<d; j++)
         v[j] = M.elements[i][j];
      if(v.IsEqualToZero())
         break;
      out.AddOnTop(v);
   }
   return out;
}

template<typename coefficient>
List<Vector<coefficient> > intersection(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W)
{  if((V.size==0) or (W.size==0))
   {  List<Vector<coefficient> > out;
      return out;
   }
   int d = V[0].size;

   Matrix<coefficient> MV;
   MV.init(V.size, d);
   for(int i=0; i<V.size; i++)
      for(int j=0; j<d; j++)
         MV.elements[i][j] = V[i][j];
   List<Vector<coefficient> > Vperp = DestructiveKernel(MV);

   Matrix<coefficient> MW;
   MW.init(W.size, d);
   for(int i=0; i<W.size; i++)
      for(int j=0; j<d; j++)
         MW.elements[i][j] = W[i][j];
   List<Vector<coefficient> > Wperp = DestructiveKernel(MW);

   Matrix<coefficient> M;
   M.init(Vperp.size+Wperp.size,d);
   int i=0;
   for(; i<Vperp.size; i++)
      for(int j=0; j<d; j++)
         M.elements[i][j] = Vperp[i][j];
   for(; i<Vperp.size+Wperp.size; i++)
      for(int j=0; j<d; j++)
         M.elements[i][j] = Wperp[i-Vperp.size][j];
   return DestructiveKernel(M);
}

template <typename coefficient>
List<Vector<coefficient> > orthogonal_complement(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &WW)
{  List<Vector<coefficient> > W = intersection(V,WW);
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
   {  for(int j=0; j<d; j++)
      {  coefficient r = V[i][j];
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
   {  Vector<coefficient> v;
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
{  List<coefficient> q;
   coefficient lastround = p[0];
   for(int i=1; i<p.size; i++)
   {  q.AddOnTop(lastround);
      lastround = p[i] - lastround*a;
   }
   if(lastround == 0)
   {  p = q;
      return true;
   }
   return false;
}

template <typename coefficient>
List<int> factorpoly(List<coefficient> p, int maxfac)
{  List<int> factors;
   for(int i1=0; i1<maxfac; i1++)
   {  for(int i2=1; i2>=-1; i2-=2)
      {  int i = i1*i2;
         while(pdiv(p,i))
         {  if(!factors.Contains(i))
            {  factors.AddOnTop(i);
            }
            if(p.size == 1)
               return factors;
         }
      }
   }
   return factors;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveKernel(Matrix<coefficient> &M)
{  Matrix<coefficient> MM; // idk what this does
   Selection s;
//  std::cout << "DestructiveKernel: GaussianEliminationByRows" << std::endl;
   M.GaussianEliminationByRows(M,MM,s);
//  std::cout << "DestructiveKernel: calculating kernel space" << std::endl;
   List<Vector<coefficient> > V;
   for(int i=0; i<s.selected.size; i++)
   {  if(!s.selected[i])
         continue;
      Vector<coefficient> v;
      v.MakeEi(M.NumCols,i);
      int rowmap = 0;
      for(int j=0; j<M.NumCols; j++)
         if(!s.selected[j])
         {  v[j] = -M.elements[rowmap][i];
            rowmap++;
         }
      V.AddOnTop(v);
   }
//  std::cout << "DestructiveKernel: done" << std::endl;
   return V;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveEigenspace(Matrix<coefficient> &M, const coefficient &l)
{  for(int i=0; i<M.NumCols; i++)
      M.elements[i][i] -= l;
   return DestructiveKernel(M);
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
Vector<coefficient> ActOnGroupRing(const CoxeterGroup& G, int g, const Vector<coefficient> &v)
{  Vector<coefficient> out;
   out.MakeZero(G.N);
   for(int i=0; i<G.N; i++)
      if(v[i] != 0)
         out[G.MultiplyElements(g,i)] = v[i];
   return out;
}

// this function name is a lie
template <typename coefficient>
bool is_isotypic_component(CoxeterGroup &G, const List<Vector<coefficient> > &V)
{  // pre-initial test: V isn't empty
   if(V.size == 0)
      return false;
   // initial test: dimension of V is a square
   int n = sqrt(V.size);
   if(n*n != V.size)
      return false;
   // more expensive test: character of V has unit norm
   ClassFunction<coefficient> X;
   X.G = &G;
   X.data.SetSize(G.ccCount);
   for(int i=0; i<G.ccCount; i++)
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
   if(X.norm() != n)
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

int skipcount(int n, const List<int>& l)
{  int o = 0;
   for(int i=0; i<l.size; i++)
   {  if(l[i] == n)
         return -1;
      if(l[i]<n)
      {  o++;
      }
   }
   return n-o;
}

int popcnt(int i) // from the internet
{  i = i - ((i >> 1) & 0x55555555);
   i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
   return ((((i + (i >> 4)) & 0xF0F0F0F) * 0x1010101) & 0xffffffff) >> 24;
}

List<List<int> > powersetgrade(int G, const List<int>& l)
{  List<List<int> > out;
   for(int i=0; i<1<<l.size; i++)
   {  if(popcnt(i)==G)
      {  List<int> outi;
         for(int j=0; j<l.size; j++)
         {  if(i&(1<<j))
               outi.AddOnTop(j);
         }
         out.AddOnTop(outi);
      }
   }
   return out;
}

template<typename Element>
Element minor_det(const Matrix<Element>& M, const List<int>& l)
{  Matrix<Element> MM;
   MM.init(M.NumRows-l.size,M.NumCols-l.size);
   for(int i=0; i<M.NumRows; i++)
   {  int ii = skipcount(i,l);
      if(ii == -1)
         continue;
      for(int j=0; j<M.NumCols; j++)
      {  int jj = skipcount(j,l);
         if(jj == -1)
            continue;
         MM.elements[ii][jj] = M.elements[i][j];
      }
   }
//  Element x;
//  MM.ComputeDeterminantOverwriteMatrix(&x);
//  return x;
   return MM.GetDeterminant();
}

template<typename Element>
List<Element> charpoly(const Matrix<Element>& M)
{  List<Element> p;
   p.SetSize(M.NumRows+1);
   List<int> rowids;
   for(int i=0; i<M.NumRows; i++)
      rowids.AddOnTop(i);
   for(int i=0; i<M.NumRows; i++)
   {  Element acc = 0;
      List<List<int> > ps = powersetgrade(i,rowids);
      for(int si=0; si < ps.size; si++)
         acc += minor_det(M,ps[si]);
      if(i%2==0)
         acc = -acc;
      p[M.NumRows-i] = acc;
   }
   p[0] = ((M.NumRows%2)==0)? -1 : 1;

   return p;
}

/*Rational Matrix<Rational>::GetTrace(){
    Rational acc = 0;
    for(int i=0; i<this.NumCols; i++){
        acc += this.elements[i][j];
    }
    return acc;
}*/

int chartable[10][10] =
{  {1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
   {1, -1, -1,  1,  1, -1,  1,  1, -1, -1},
   {3,  1,  1, -1,  1,  0,  0, -1, -1, -3},
   {3, -1, -1, -1,  1,  0,  0, -1,  1,  3},
   {2,  0, -2,  0,  0,  1, -1,  2,  0, -2},
   {2,  0,  2,  0,  0, -1, -1,  2,  0,  2},
   {1,  1, -1, -1, -1, -1,  1,  1,  1, -1},
   {1, -1,  1, -1, -1,  1,  1,  1, -1,  1},
   {3, -1,  1,  1, -1,  0,  0, -1,  1, -3},
   {3,  1, -1,  1, -1,  0,  0, -1, -1,  3}
};

int main(void)
{  testformat.flagUseHTML = false;
   testformat.flagUseLatex = false;
   /*
       Rational zero = Rational(0,1);
       Rational one = Rational(1,1);
       Rational two = Rational(2,1);
       std::cout << zero*one << "\n" << zero+one << one*two << one+two << two*two << std::endl;

   //    Vector<Rational> v;
   //    v.SetSize(3);
   //    v="(1/2,3,-1/5)";
   //    v[1].AssignString("1/3");
     //  v[2]=5;
       Vector<Rational> v;
       v = "(1/3,1,2)";
       std::cout << v << std::endl;

       Vector<Rational> w;
       w.SetSize(3);
       w[0] = 0;
       w[1] = 1;
       w[2] = 2;
       std::cout << w << std::endl;

       Matrix<Rational> M;
       M.Resize(3,3,false,&zero);
       M.ActOnVectorColumn(v,zero);
       std::cout << M << std::endl;
       std::cout << v << std::endl;
   */
   /*
       Rational one = 1;
       DynkinType D;
       D.MakeSimpleType('B',3,&one);
       WeylGroup G;
       G.MakeFromDynkinType(D);
       std::cout << G << std::endl;
       std::cout << G.CartanSymmetric << std::endl;
       G.ComputeAllElements();
       std::cout << G << std::endl;
   */
   /*
       Vector<Rational> v1; v1 = "(1,0)";
       Vector<Rational> v2; v2 = "(0,1)";

       std::cout << D << std::endl;
       D.GetCartanSymmetric(M);
       std::cout << M << std::endl;
       Vectors<Rational> V;
       G.ComputeWeylGroupAndRootsOfBorel(V);
       std::cout << G.GetReport() << std::endl;
       std::cout << "printing G " << G << std::endl;
       G.GetMatrixOfElement(0,M);


       Graph *g = new Graph(6,3);
       g->AddEdge(0,1);
       g->AddEdge(1,2);
       g->AddEdge(2,3);
       g->AddEdge(4,5);
       g->AddEdge(0,3);
       std::cout << "g" << g->edges << std::endl;
       std::cout << g->data << std::endl;
       List<List<int> > ccs;
       ccs = g->DestructivelyGetConnectedComponents();
       std::cout << ccs << std::endl;

       std::cout << G.RootsOfBorel << std::endl;

       std::cout << G[0] << '\n' << G[1] << '\n' << G[2] << '\n' << std::endl;
       M = GetMatrixOfElement(G,G[6]);
       std::cout << GetMatrixOfElement(G,G[6]) << std::endl;
       std::cout << G[6] << ' ' << M.GetDeterminant() << ' ' << M(0,0)+M(1,1)+M(2,2) << std::endl;
   */
/*

   DynkinType D;
   D.MakeSimpleType('G',2);
   Matrix<Rational> M;
   D.GetCartanSymmetric(M);
   std::cout << M << std::endl;

//  Matrix<f127> M2;
//  M2.init(M.NumRows, M.NumCols);
//  for(int i=0; i<M.NumRows; i++)
//    for(int j=0; j<M.NumCols; j++)
//     M2.elements[i][j].n = M.elements[i][j].GetNumerator().GetIntValueTruncated();
  CoxeterGroup G;
  G.CartanSymmetric = M;
  G.ComputeIrreducibleRepresentations();
  CoxeterRepresentation<Rational> r = G.irreps[0]*G.irreps[1];
  Matrix<Rational> cfm;
  ClassFunction<Rational> cf;
  cf = G.irreps[0].GetCharacter();
  r.ClassFunctionMatrix(cf,cfm);
  std::cout << cfm.ToString(&testformat) << std::endl;
*/

  /*
   CoxeterGroup G;
   G.MakeFrom(M);
   G.ComputeInitialCharacters();
   for(int i=0; i<G.characterTable.size; i++)
      std::cout << G.characterTable[i] << std::endl;
  */
   /*
     for(int i=0;i<G.ccCount;i++){
       int n = G.conjugacyClasses[i][0];
       Matrix<Rational> M = G.TodorsVectorToMatrix(G.rhoOrbit[n]);
       std::cout << i << charpoly(M) << "\n\n";
     //  std::cout << M << '\n';
     //  std::cout << "\n\n";
     }

     G.characterTable.size = 0;
     for(int i=0; i<10; i++){
       ClassFunction<Rational> X;
       X.G = &G;
       for(int j=0; j<10; j++){
         X.data.AddOnTop(chartable[i][j]);
       }
       G.characterTable.AddOnTop(X);
     }

     std::cout << G.characterTable << std::endl;
   */
   /*
     GroupRingElement<Rational> XeP;
     XeP.MakeFromClassFunction(G.characterTable[0]);
     std::cout << XeP.data << std::endl;

     GroupRingElement<Rational> E;
     E.MakeEi(&G,0);
     std::cout << E << std::endl;
     std::cout << XeP*E << std::endl;

     GroupRingElement<Rational> XstdP;
     XstdP.MakeFromClassFunction(G.characterTable[2]);
     std::cout << XstdP << std::endl;

     std::cout << "\nhi there\n";
     for(int i=0; i<G.N; i++)
     { E.MakeEi(&G,i);
       std::cout << XstdP*E << std::endl;
     }

     std::cout << GetMatrix(G.characterTable[0]) << std::endl;
     std::cout << GetMatrix(G.characterTable[2]) << std::endl;

     E.MakeEi(&G,33);
     std::cout << XstdP*E << std::endl;
     std::cout << GetMatrix(G.characterTable[2])*E.data << std::endl;

     ElementMonomialAlgebra<CoxeterElement, Rational> Xstdp = FromClassFunction(G.characterTable[2]);
     std::cout << Xstdp << std::endl;
     std::cout << "here they are" << std::endl;
     List<ElementMonomialAlgebra<CoxeterElement, Rational> > l;
     for(int i=0; i<G.N; i++)
     { ElementMonomialAlgebra<CoxeterElement, Rational> tmp;
       tmp.AddMonomial(G.GetCoxeterElement(i),1);
       tmp *= Xstdp;
       l.AddOnTop(tmp);
       std::cout << tmp << '\n' << std::endl;
     }
     Xstdp.GaussianEliminationByRowsDeleteZeroRows(l);
     std::cout << l.size << std::endl;
   */
   /*
     Matrix<Rational> ct;
     ct.init(G.ccCount,G.ccCount);
     for(int i=0; i<G.ccCount; i++)
       for(int j=0; j<G.ccCount; j++)
       { ct.elements[i][j] = chartable[i][j];
         ct.elements[i][j] /= G.N;
         ct.elements[i][j] *= G.ccSizes[j];
       }
     */
   /*
   Vectors<Rational> powers;
   Vectors<Rational> powers2;
   Vector<Rational> v;
   v = G.characterTable[2].data;
   std::cout << v << '\n' << ct*v << '\n';
   powers.AddOnTop(v);
   for(int i=1; i<50; i++)
   { Vector<Rational> v;
     v = G.characterTable[2].data;
     for(int j=0; j<G.ccCount; j++)
       v[j] *= powers[i-1][j];
     Vector<Rational> w = ct*v;
     std::cout << v << '\n' << w << '\n';
     powers.AddOnTop(v);
     powers2.AddOnTop(w);
   }
   std::cout << powers2.GetDim() << std::endl;
   std::cout << powers.GetDim() << std::endl;

   ClassFunction<Rational> X;
   X.G = &G;
   X.data = powers[5];
   std::cout << X << std::endl;

   List<ClassFunction<Rational> > tstct;
   for(int i=0;i<G.ccCount;i++)
     std::cout << X.IP(G.characterTable[i]) << std::endl;

   Matrix<Rational> XP;
   XP = GetMatrix(G.characterTable[2]);
   GroupRingElement<Rational> XPP;
   Vector<Rational> vv;
   XPP.MakeFromClassFunction(G.characterTable[2]);
   GroupRingElement<Rational> vP;
   for(int i=0; i<G.N; i++)
   { vv.MakeEi(G.N,i);
     vP.MakeEi(&G,i);
     std::cout << (XP*vv == (XPP*vP).data) << ' ';
   }
   */
   //  Vectors<Rational> V;
   //  XP.GetZeroEigenSpace(V);
   //  std::cout << V << std::endl;
   //  std::cout << V.GetDim() << std::endl;

   /*
   Matrix<Rational> A;
   A.init(3,3);
   Rational one = 1;

   A(0,0) = 1; A(0,1) = 2; A(0,2) = 3;
   A(1,0) = 4; A(1,1) = 5; A(1,2) = 6;
   A(2,0) = 7; A(2,1) = 8; A(2,2) = 9;
   std::cout << Kernel(A) << std::endl;

   A(0,0) = 1; A(0,1) = 0; A(0,2) = 0;
   A(1,0) = 0; A(1,1) = 1; A(1,2) = 0;
   A(2,0) = 0; A(2,1) = 0; A(2,2) = 0;
   std::cout << Kernel(A) << std::endl;
   std::cout << Eigenspace(A,one) << std::endl;
   */
   /*
     Matrix<Rational> A;
     Rational one = 1;
     A.init(6,6);
     A(0,0) = 2; A(0,1) =-1; A(0,2) =-1; A(0,3) = 0; A(0,4) = 0; A(0,5) = 0;
     A(1,0) =-1; A(1,1) = 2; A(1,2) =-1; A(1,3) = 0; A(1,4) = 0; A(1,5) = 0;
     A(2,0) =-1; A(2,1) =-1; A(2,2) = 2; A(2,3) = 0; A(2,4) = 0; A(2,5) = 0;
     A(3,0) = 0; A(3,1) = 0; A(3,2) = 0; A(3,3) = 2; A(3,4) =-1; A(3,5) =-1;
     A(4,0) = 0; A(4,1) = 0; A(4,2) = 0; A(4,3) =-1; A(4,4) = 2; A(4,5) =-1;
     A(5,0) = 0; A(5,1) = 0; A(5,2) = 0; A(5,3) =-1; A(5,4) =-1; A(5,5) = 2;
     A /= 3;
     std::cout << DestructiveEigenspace(A,one) << std::endl;

     Rational zero = 0;
     Rational negone = -1;
     List<List<Rational> > ll;
     GlobalVariables g;
     A.FindZeroEigenSpacE(ll,one,negone,zero,g);
     std::cout << ll << std::endl;
   */
   /*
     Matrix<Rational> XP;
     for(int i=0; i<G.ccCount; i++)
     {
     XP = GetMatrix(G.characterTable[i]);
     XP *= G.characterTable[i].data[0];
     std::cout << G.characterTable[i] << std::endl;
     std::cout << DestructiveEigenspace(XP,one) << std::endl;
     }
   */
   /*
     Rational three = 3;
     List<Rational> p;
     p.AddOnTop(one); p.AddOnTop(three); p.AddOnTop(three); p.AddOnTop(one);
     std::cout << factorpoly(p,5) << std::endl;
     p.size = 0;
     p.AddOnTop(one); p.AddOnTop(three); p.AddOnTop(one); p.AddOnTop(three);
     std::cout << factorpoly(p,5) << std::endl;

     Vector<Rational> v;
     List<Vector<Rational> > B;
     v.AssignString("(1,1,0)");
     B.AddOnTop(v);
     v.AssignString("(1,0,1)");
     B.AddOnTop(v);
     v.AssignString("(0,1,1)");
     B.AddOnTop(v);
     v.AssignString("(2,2,2)");
     std::cout << PutInBasis(v,B) << std::endl;
     B.size = 0;
     v.AssignString("(1,1,0,0)");
     B.AddOnTop(v);
     v.AssignString("(1,0,0,1)");
     B.AddOnTop(v);
     v.AssignString("(0,0,1,1)");
     B.AddOnTop(v);
     std::cout << "4-vector tests" << std::endl;
     v.AssignString("(1,0,0,1)");
     std::cout << PutInBasis(v,B) << std::endl;
     v.AssignString("(0,1,1,0)");
     std::cout << PutInBasis(v,B) << std::endl;

     M = GetMatrix(G.characterTable[2]);
     M *= 3;
     B = DestructiveEigenspace(M,one);
     std::cout << B << std::endl;

     Matrix<Rational> BM;
     BM.init(B.size,B[0].size);
     for(int i=0; i<B.size; i++)
     { for(int j=0; j<B[0].size; j++)
       { BM.elements[i][j] = B[i][j];
       }
     }

     std::cout << BM.ToString(&testformat) << std::endl;
     for(int i=0; i<BM.NumRows; i++)
     { std::cout << PutInBasis(B[i],B) << std::endl;
     }

     int ge = 37;
     M.init(B.size,B.size);
     for(int i=0; i<B.size; i++)
     { Vector<Rational> v;
       v.MakeZero(B[0].size);
       for(int j=0; j<B[0].size; j++)
       { v[G.MultiplyElements(ge,j)] = B[i][j];
       }
       v = PutInBasis(v,B);
       for(int j=0;j<B.size;j++)
       { M.elements[i][j] = v[j];
       }
     }
     std::cout << M.ToString(&testformat) << std::endl;



     List<List<Vector<Rational> > > spaces;
     M = GetMatrix(G.characterTable[2]);
     B = DestructiveColumnSpace(M);
     spaces.AddOnTop(B);
     List<ClassFunction<Rational> > Xs;
     Xs.AddOnTop(G.characterTable[2]);
     std::cout << spaces << std::endl;
     { int i=1;
       Xs.AddOnTop(Xs[i-1]*Xs[i-1]);
       M = GetMatrix(Xs[i]);
   //    List<Rational> p = charpoly(M);
   //    std::cout << p << std::endl;
   //    List<int> ls = factorpoly(p,Xs[i].data[0].floorIfSmall());
   //    std::cout << ls << std::endl;
       B = DestructiveColumnSpace(M);
       std::cout << B << std::endl;
     }
   */
   /*

     Matrix<Rational> a;
     Matrix<Rational> b;
     a.init(2,2);
     b.init(2,2);
     a(0,0) = -1; a(0,1) = -1; a(1,0) =  1; a(1,1) =  0;
     b(0,0) =  1; b(0,1) =  0; b(1,0) = -1; b(1,1) = -1;
     M.AssignTensorProduct(b,b);
     std::cout << M.ToString(&testformat) << std::endl;
   */
   /*
       std::cout << "this orbit has " << G.rhoOrbit.size << std::endl;
       std::cout << G.rhoOrbit << std::endl;
       Vector<Rational> v = G.rhoOrbit[34];
       M = G.TodorsVectorToMatrix(v);
       std::cout << M*G.rho << " derp " << v << std::endl;
   */
//    for(int i=0;i<G.rhoOrbit.size;i++){
//    std::cout << G.TodorsVectorToMatrix(G.rhoOrbit[i]) << "\n\n";
//    }
//    std::cout << std::endl;
   /*
       std::cout << v << G.DecomposeTodorsVector(v) << G.ComposeTodorsVector(G.DecomposeTodorsVector(v)) << std::endl;

       G.ComputeConjugacyClasses();
       std::cout << G.conjugacyClasses << std::endl;
       std::cout << G.ccSizes << std::endl;
       G.ComputeSquares();
       std::cout << G.squares << std::endl;
       G.ComputeInitialCharacters();
       std::cout << G.characterTable << std::endl;
       std::cout << G.characterTable[0].norm() << G.characterTable[1].norm() << G.characterTable[2].norm() << std::endl;

   //    ElementMonomialAlgebra<FiniteGroupElement, Rational> a;
       ClassFunction X = G.characterTable[2] - G.characterTable[0];
       std::cout << X << X.norm() << std::endl;

       List<Vector<Rational> > l;
       for(int i=0;i<G.rootSystem.size;i++){
           bool usethis = true;
           for(int j=0; j<4; j++)
               if(G.rootSystem[i][j] < 0){
                   usethis = false;
                   break;
               }
           if(usethis == true)
               l.AddOnTop(G.rootSystem[i]);
       }
       std::cout << l << std::endl;
   */
   /*
       std::string* S = [
       ""

       ]
   */
   /*    std::cout << "polynom1al" << std::endl;
       polynom1al<Rational> zeropoly;
       Matrix<polynom1al<Rational> > MP;
       MP.MakeZeroMatrix(G.N, zeropoly);
       for(int i1=0; i1<G.ccCount; i1++)
       { for(int i2=0; i2<G.ccSizes[i1]; i2++)
         { int i=G.conjugacyClasses[i1][i2];
           for(int j=0; j<G.N; j++)
           { M.elements[G.MultiplyElements(i,j)][j] = G.characterTable[2].data[i1]/G.N;
           }
         }
       }
       for(int i=0; i<G.N; i++)
       { MP.elements[i][i].AddMonomial(-1,1);
       }

       polynom1al<Rational> cp;

       cp = MP.GetDeterminant();

       std::cout << cp << std::endl;
   */
//    List<bool> isod;
//    isod.AddOnTop(false);
//    int isodd = 0;
   /*    List<Vector<Rational> > bigspace;
       List<List<Vector<Rational> > > spaces;
       for(int i=0; i<G.N; i++)
       {Vector<Rational> v;
         v.MakeEi(G.N,i);
         bigspace.AddOnTop(v);
       }
       spaces.AddOnTop(bigspace);
       Rational one = 1;
       Rational zero = 0;
       for(int ci=0; ci<G.ccCount; ci++)
       { ClassFunction<Rational> X;
         X.G = &G;
         for(int ii=0; ii<G.ccCount; ii++)
         { if(ii==ci)
             X.data.AddOnTop(one);
             continue;
           X.data.AddOnTop(zero);
         }

       }
       */
   /*    ClassFunction<Rational> Xs = G.characterTable[2];
       ClassFunction<Rational> Xcs = G.characterTable[1]*G.characterTable[2];
       List<List<ClassFunction<Rational> > > chars;
       List<ClassFunction<Rational> > charsi;
       charsi.AddOnTop(G.characterTable[0]);
       chars.AddOnTop(charsi);
       HashedList<ClassFunction<Rational> > usedchars;
       int ci = 0;
       int cj = 0;
       while(true)
       { if(!usedchars.Contains(chars[ci][cj]))
         { usedchars.AddOnTop(chars[ci][cj]);
           std::cout << "matrix for " << ci << " " << cj << " " << chars[ci][cj] << std::endl;
           Matrix<Rational> MM = GetMatrix(chars[ci][cj]);
           std::cout << "eigenspaces" << std::endl;
           List<List<Vector<Rational> > > es = eigenspaces(MM,G.N);
           std::cout << "splitting spaces" << std::endl;
           for(int i=0; i<es.size; i++)
           { int spaces_to_check = spaces.size;
             for(int j=0; j<spaces_to_check; j++)
             { //if(isod[j])
               //  continue;
               if(spaces[j].size < 9)
                 continue;
               std::cout << "intersect eigenspace " << i << " with space " << j << std::endl;
               List<Vector<Rational> > middle = intersection(es[i], spaces[j]);
               std::cout << "result is " << middle.size << "dimensional" << std::endl;
               if(0 < middle.size and middle.size < spaces[j].size)
               { std::cout << "finding relative complement" << std::endl;
                 List<Vector<Rational> > right = relative_complement(spaces[j],es[i]);
                 std::cout << "relative complement is " << right.size << "dimensional" << std::endl;
                 spaces[j] = middle;
                 //if(is_isotypic_component(G,middle))
                 //{ isod[j] = true;
                 //  isodd += middle.size;
                 //  std::cout << "intersection was an isotypic component" << std::endl;
                 //}
                 spaces.AddOnTop(right);
                 //if(is_isotypic_component(G,right))
                 //{ isod.AddOnTop(true);
                 //  isodd += right.size;
                 //  std::cout << "complement was an isotypic component" << std::endl;
                 //} else
                 //{ isod.AddOnTop(false);
                 //}
                 std::cout << "moving on" << std::endl;
               }
             }
           }
   //        if(isodd >= G.N)
   //          break;
           if(spaces.size == G.ccCount)
             break;
         }
         std::cout << "finding next character" << std::endl;
         if(ci!=0)
         { ci--;
           cj++;
           chars[ci].AddOnTop(chars[ci][cj-1]*Xcs);
         } else
         { ci=cj+1;
           cj=0;
           List<ClassFunction<Rational> > cil;
           cil.AddOnTop(chars[ci-1][0]*Xs);
           chars.AddOnTop(cil);
         }
       }
       std::cout << "spaces" << std::endl;
       std::cout << spaces.size << std::endl;
       for(int i=0; i<spaces.size; i++)
         std::cout << spaces[i].size << " ";
       std::cout << std::endl;
       //std::cout << isod << std::endl;
       //std::cout << isodd << std::endl;
   */
   /*    int i=0; j=0;
       while(spaces.size > 0)
       {

       }
   */

   /*    std::cout << V2.GetCharacter() << std::endl;
       CoxeterRepresentation<Rational> V2x2 = V2*V2;
       std::cout << V2x2.GetCharacter() << std::endl;
       for(int i=0; i<G.nGens; i++)
       { std::cout << V2x2.gens[i].ToString(&testformat);
         std::cout << V2x2.gens[i].GetDeterminant() << ' ' << V2x2.gens[i].GetTrace() << std::endl;
       }
       std::cout << "class function matrices" << std::endl;
       for(int cfi=0; cfi<G.ccCount; cfi++)
       { ClassFunction<Rational> cf;
         cf.G = &G;
         cf.MakeZero();
         cf[cfi] = 1;
         std::cout << cfi << std::endl;
         std::cout << V2x2.ClassFunctionMatrix(cf).ToString(&testformat) << std::endl;
       }
       List<CoxeterRepresentation<Rational> > Vs;
       Vs = V2x2.Decomposition();
       for(int i=0; i<Vs.size; i++)
         std::cout << Vs[i].Reduced().GetCharacter() << std::endl;
     */

     /*Matrix<Rational> A;
     A.init(6,6);
     A(0,0) = 2; A(0,1) =-1; A(0,2) =-1; A(0,3) = 0; A(0,4) = 0; A(0,5) = 0;
     A(1,0) =-1; A(1,1) = 2; A(1,2) =-1; A(1,3) = 0; A(1,4) = 0; A(1,5) = 0;
     A(2,0) =-1; A(2,1) =-1; A(2,2) = 2; A(2,3) = 0; A(2,4) = 0; A(2,5) = 0;
     A(3,0) = 0; A(3,1) = 0; A(3,2) = 0; A(3,3) = 2; A(3,4) =-1; A(3,5) =-1;
     A(4,0) = 0; A(4,1) = 0; A(4,2) = 0; A(4,3) =-1; A(4,4) = 2; A(4,5) =-1;
     A(5,0) = 0; A(5,1) = 0; A(5,2) = 0; A(5,3) =-1; A(5,4) =-1; A(5,5) = 2;
     std::cout << "polynomial test" << std::endl;
     UDPolynomial<Rational> p = MinPoly(A);
     std::cout << p.data.size << std::endl;
     std::cout << p << std::endl;
*/

   /*std::cout << '[';
   for(int i=0; i<G.ccCount; i++)
     std::cout << G.conjugacyClasses[i][0] << ", ";
   std::cout << ']' << std::endl;
   for(int i=0; i<G.ccCount; i++)
     std::cout << i << ' ' << G.ccSizes[i] << ' ' << G.GetCoxeterElement(G.conjugacyClasses[i][0]) << std::endl;
   for(int i=0; i<G.ccCount; i++)
     std::cout << G.GetCoxeterElement(G.conjugacyClasses[i][0]) << ", ";*/
 //  G.ComputeIrreducibleRepresentations();
   /*
   List<ClassFunction<Rational> > ct;
   List<CoxeterRepresentation<Rational> > gr;

   CoxeterRepresentation<Rational> sr = G.StandardRepresentation();
   CoxeterRepresentation<Rational> sr2 = sr*sr;

   List<CoxeterRepresentation<Rational> > sr2d = sr2.Decomposition(ct,gr);

   std::cout << "std (x) std = ";
   for(int i=0; i<sr2d.size; i++)
   { std::cout << sr2d[i].GetCharacter() << std::endl;
     for(int j=0; j<sr2d[i].gens.size; j++)
      std::cout << sr2d[i].gens[j].ToString(&testformat) << std::endl;
   }
   */
   GlobalVariables localGlobalVariables;
   localGlobalVariables.SetFeedDataToIndicatorWindowDefault(CGI::makeStdCoutReport);
   WeylGroup G;
   G.MakeArbitrarySimple('E',7,NULL);
   List<Vector<Rational> > chars = ComputeCharacterTable(G);
   for(int i=0; i<chars.size; i++)
     std::cout << chars[i] << std::endl;
/*   G.ComputeIrreducibleRepresentations(&localGlobalVariables);
   Selection sel;
   sel.MakeFullSelection(G.CartanSymmetric.NumCols);
   sel.RemoveSelection(0);
   WeylSubgroup H;
   ParabolicSubgroup(&G,sel,H);
   std::cout << G.CartanSymmetric.ToString(&testformat) << std::endl;
   H.ComputeIrreducibleRepresentations();
   std::cout << "subgroup" << std::endl;
   std::cout << H.ToString(&testformat) << std::endl;
   std::cout << "sigs" << std::endl;
   H.ComputeTauSignature();
   std::cout << H.tauSignature << std::endl;

   List<List<bool> > ts;
   AllTauSignatures(&G,ts,true);
   for(int i=0; i<G.conjugacyClasses.size; i++)
   {

   std::cout << "conjugacy class " << i << std::endl;
    for(int j=0; j<G.conjugacyClasses[i].size; j++)
    {ElementWeylGroup g = G.theElements[G.conjugacyClasses[i][j]];
      for(int k=0; k<g.size; k++)
       std::cout << g[k] << ' ';
      std::cout << std::endl;
    }
    std::cout << std::endl;
   }



// lets print out the character table
  std::cout << "\\[ \\begin{array}{";
  for(int i=0; i<G.conjugacyClasses.size+1; i++)
    std::cout << 'c';
  std::cout << "}\n";
  for(int i=0; i<G.conjugacyClasses.size; i++)
    std::cout << '&' << G.conjugacyClasses[i].size << '\t';
  std::cout << "\\\\" << std::endl;

  for(int i=0; i<G.conjugacyClasses.size; i++)
  { ElementWeylGroup g = G.theElements[G.conjugacyClasses[i][0]];
    std::cout << '&';
    for(int k=0; k<g.size; k++)
      std::cout << g[k] << ' ';
    std::cout << '\t';
  }
  std::cout << "\\\\\n\\hline\n";

  for(int i=0; i<G.irreps.size; i++)
  { for(int j=0; j<G.conjugacyClasses.size; j++)
    { Vector<Rational> Xi = G.irreps[i].GetCharacter();
      std::cout << '&' << Xi[j] << '\t';
    }
    std::cout << "\\\\\n";
  }
  std::cout << "\\end{array} \\]" << std::endl;

// okay now the regular tau signatures
  sel.init(G.CartanSymmetric.NumCols);
  int numCycles=MathRoutines::TwoToTheNth(sel.MaxSize);
  std::cout << "\\[ \\begin{array}{";
  for(int i=0; i<numCycles+1; i++)
    std::cout << 'c';
  std::cout << "}\n";
   for(int i=0; i<numCycles; i++)
   { std::cout << "&<";
     for(int j=0; j<sel.CardinalitySelection; j++)
     { std::cout << sel.elements[j];
       if(j+1<sel.elements.size)
         std::cout << ' ';
     }
     std::cout << ">\t";
     sel.incrementSelection();
   }

   std::cout << "\\\\" << std::endl;
   for(int i=0; i<ts.size; i++)
   { for(int j=0; j<numCycles; j++)
      { std::cout << '&';
        std::cout << ts[i][j] << '\t';
      }
      std::cout << "\\\\\n";
   }
   std::cout << "\\end{array} \\]" << std::endl;

// now the pseudo tau signatures
  if(ts[0].size > numCycles)
  { std::cout << "hr is ";
    ElementWeylGroup hr = G.theElements[G.GetRootReflection(G.RootSystem.size-1)];
    for(int i=0; i<hr.size; i++)
      std::cout << hr[i] << ' ';
    std::cout << std::endl;

    std::cout << "\\[ \\begin{array}{";
    for(int i=0; i<numCycles-1; i++)
      std::cout << 'c';
    std::cout << "}\n";

    sel.init(G.CartanSymmetric.NumCols);
    for(int i=0; i<numCycles-1; i++)
    { std::cout << "&<";
      for(int j=0; j<sel.CardinalitySelection; j++)
        std::cout << sel.elements[j] << ' ';
      std::cout << "hr";
      std::cout << ">\t";
      sel.incrementSelection();
    }

    std::cout << "\\\\" << std::endl;
    for(int i=0; i<ts.size; i++)
    { for(int j=0; j<numCycles; j++)
       { std::cout << '&';
         std::cout << ts[i][j] << '\t';
       }
       std::cout << "\\\\\n";
    }
    std::cout << "\\end{array} \\]" << std::endl;
  }
*/
/*
   std::cout << "Building QG" << std::endl;
   WeylGroupRepresentation<Rational> QG;
   QG.reset(&G);
   for(int g=1; g<G.CartanSymmetric.NumRows+1; g++)
   { Matrix<Rational> M;
     M.MakeZeroMatrix(G.theElements.size);
     for(int i=0; i<G.theElements.size; i++)
     { ElementWeylGroup gg;
       gg = G.theElements[g];
       gg *= G.theElements[i];
       gg.MakeCanonical();
       M.elements[i][G.theElements.GetIndex(gg)] = 1;
     }
     QG.SetElementImage(g,M);
   }

   std::cout << "getting isotypic components of QG" << std::endl;
   List<WeylGroupRepresentation<Rational> > isocomps;
   isocomps.SetSize(G.irreps.size);
   for(int i=0; i<G.irreps.size; i++)
   { Matrix<Rational> M;
     QG.GetClassFunctionMatrix(G.irreps[i].GetCharacter(),M);
     VectorSpace<Rational> V;
     Vectors<Rational> B;
     for(int j=0; j<M.NumCols; j++)
     { Vector<Rational> v;
       M.GetVectorFromColumn(j,v);
       if(!V.AddVector(v))
         continue;
       B.AddOnTop(v);
     }
     Vector<Rational> v;
     QG.Restrict(B,v,isocomps[i]);
   }

   std::cout << "trying to break up one isotypic component" << std::endl;
   int cmpx = 2;
   std::cout << isocomps[cmpx].ToString(&testformat) << std::endl;
   int d = isocomps[cmpx].GetDim();
   std::cout << "the smoothed-out inner product" << std::endl;
   Matrix<Rational> B;
   B.init(d,d);
   for(int i=0; i<d; i++)
     for(int j=0; j<d; j++)
       for(int g=0; g<G.theElements.size; g++)
       { Vector<Rational> v,w;
         isocomps[cmpx].GetElementImage(g).GetVectorFromColumn(i,v);
         isocomps[cmpx].GetElementImage(g).GetVectorFromColumn(j,w);
         B.elements[i][j] = v.ScalarEuclidean(w);
       }
  std::cout << B.ToString(&testformat) << std::endl;

  Vector<Rational> v;
  v.MakeZero(B.NumRows);
  for(int i=0; i<B.NumCols; i++)
    for(int j=0; j<B.NumRows; j++)
      v[j] += B.elements[i][j];
  std::cout << v << std::endl;

  VectorSpace<Rational> WS;
  Vectors<Rational> W;
  for(int g=0; g<G.theElements.size; g++)
  { Vector<Rational> w = isocomps[cmpx].GetElementImage(g) * v;
    if(WS.AddVector(w))
      W.AddOnTop(w);
  }
  WeylGroupRepresentation<Rational> comp;
  Vector<Rational> derp;
  isocomps[cmpx].Restrict(W,derp,comp);

  std::cout << "This should be one component" << std::endl;
  std::cout << comp.ToString(&testformat) << std::endl;
*/



   std::cout << "Rational.TotalSmallAdditions: " << Rational::TotalSmallAdditions;
   std::cout << "\nRational.TotalLargeAdditions: " << Rational::TotalLargeAdditions;
   std::cout << "\nRational.TotalSmallMultiplications: " << Rational::TotalSmallMultiplications;
   std::cout << "\nRational.TotalLargeMultiplications: " << Rational::TotalLargeMultiplications << std::endl;


/*  f127 a;
  a.n = 2;
  f127 b;
  b.n = 5;
  std::cout << (a*b).n << std::endl;
  std::cout << (a/b).n << std::endl;
*/

//   std::string s;
//   std::cin >> s;
   return 0;
}
