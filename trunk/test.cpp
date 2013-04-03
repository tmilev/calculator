#include "vpf.h"
// ^ that is first for precompilation

#include "vpfCharacters.h"
#include "vpfGraph.h"
#include "testlib.h"
#include <iostream>

// this is one reason test.cpp isn't even compiled into the actual calculator
FormatExpressions testformat;

template <typename coefficient>
class SpaceTree
{ public:
  List<Vector<coefficient> > space;
  List<SpaceTree<coefficient> > subspaces;

  void PlaceInTree(const List<Vector<coefficient> >& V);
  void GetLeaves(List<List<Vector<coefficient> > >& leaves) const;
  void DisplayTree() const;
};

template <typename coefficient>
void SpaceTree<coefficient>::PlaceInTree(const List<Vector<coefficient> > &V)
{ for(int i=0; i<subspaces.size; i++)
  { List<Vector<coefficient> > U = intersection(V,subspaces[i].space);
    if(U.size == 0)
      continue;
    if(U.size == subspaces[i].space.size)
      return;
    subspaces[i].PlaceInTree(U);
  }
  List<Vector<coefficient> > W;
  for(int i=0; i<subspaces.size; i++)
    W = getunion(W,subspaces[i].space);
  if(intersection(W,V).size != V.size)
  { SpaceTree<coefficient> vst;
    vst.space = V;
    subspaces.AddOnTop(vst);
  }
}

template <typename coefficient>
void SpaceTree<coefficient>::GetLeaves(List<List<Vector<coefficient> > >& leaves) const
{ if(subspaces.size > 0)
    for(int i=0; i<subspaces.size; i++)
      subspaces[i].GetLeaves(leaves);
  else
    leaves.AddOnTop(space);
}

template <typename coefficient>
void SpaceTree<coefficient>::DisplayTree() const
{ std::cout << '[' << space.size << ',';
  for(int i=0; i<subspaces.size; i++)
    subspaces[i].DisplayTree();
  std::cout << ']';
}


// ActionSpace has all const operations because it is a lightweight wrapper
// of a list of matrices and a list of vectors
template <typename coefficient>
class ActionSpace
{ public:
  CoxeterGroup *G;
  List<Vector<coefficient> > basis;
  List<Matrix<coefficient> > gens;

  ActionSpace(){};

  ActionSpace<coefficient> operator*(const ActionSpace<coefficient>& other) const;

  ClassFunction<coefficient> GetCharacter() const;
  coefficient GetNumberOfComponents() const;
  Matrix<coefficient> ClassFunctionMatrix(ClassFunction<coefficient> cf) const;
  List<ActionSpace<coefficient> > Decomposition() const;
  ActionSpace<coefficient> Reduced() const;
};

template <typename coefficient>
ActionSpace<coefficient> ActionSpace<coefficient>::operator*(const ActionSpace<coefficient>& other) const
{ int Vd = this->basis[0].size;
  int Wd = other.basis[0].size;
  int Ud = Vd*Wd;
  List<Vector<coefficient> > Ub;
  for(int vi=0; vi<this->basis.size; vi++)
  { for(int wi=0; wi<other.basis.size; wi++)
    { Vector<coefficient> u;
      u.SetSize(Ud);
      for(int i=0; i<Vd; i++)
        for(int j=0; j<Wd; j++)
          u[i*Wd+j] = this->basis[vi][i] * other.basis[wi][j];
      Ub.AddOnTop(u);
    }
  }
  List<Matrix<coefficient> > Ug;
  for(int i=0; i<this->gens.size; i++)
  { Matrix<coefficient> M;
    M.AssignTensorProduct(this->gens[i],other.gens[i]);
    Ug.AddOnTop(M);
  }
  ActionSpace U;
  U.basis = Ub;
  U.gens = Ug;
  U.G = this->G;
  return U;
}

template <typename coefficient>
Matrix<coefficient> ActionSpace<coefficient>::ClassFunctionMatrix(ClassFunction<coefficient> cf) const
{ Matrix<coefficient> M;
  M.MakeZeroMatrix(this->gens[0].NumRows);
  for(int cci=0; cci<this->G->ccCount; cci++)
  { if(cf[cci] == 0)
      continue;
    for(int icci=0; icci<this->G->conjugacyClasses[cci].size; icci++)
    { Matrix<coefficient> Mi;
      Mi.MakeIdMatrix(this->gens[0].NumCols);
      //Mi *= cf[cci];
      CoxeterElement g = this->G->GetCoxeterElement(this->G->conjugacyClasses[cci][icci]);
      for(int gi=g.reflections.size-1; ; gi--)
      { if(gi < 0)
          break;
        Mi.MultiplyOnTheRight(gens[g.reflections[gi]]);
      }
      //std::cout << Mi.ToString(&testformat) << std::endl;
      Mi *= cf[cci];
      M += Mi;
    }
  }
  return M;
}

template <typename coefficient>
ActionSpace<coefficient> ActionSpace<coefficient>::Reduced() const
{ int d = this->basis.size;
  Matrix<coefficient> GM;
  GM.init(d, d);
  for(int i=0; i<d; i++)
    for(int j=0; j<d; j++)
      GM.elements[i][j] = this->basis[i].ScalarEuclidean(this->basis[j]);
  GM.Invert();
  List<Matrix<coefficient> > newgens;
  for(int geni=0; geni<this->gens.size; geni++)
  { Matrix<coefficient> M;
    M.init(d, d);
    for(int i=0; i<d; i++)
      for(int j=0; j<d; j++)
        M.elements[i][j] = this->basis[i].ScalarEuclidean(this->gens[geni]*this->basis[j]);
    M.MultiplyOnTheLeft(GM);
    newgens.AddOnTop(M);
  }
  ActionSpace<coefficient> out;
  out.G = this->G;
  out.gens = newgens;
  List<Vector<Rational> > outbasis;
  for(int i=0; i<d; i++) // this should go in its own function at some point
  { Vector<coefficient> v;
    v.MakeEi(d,i);
    outbasis.AddOnTop(v);
  }
  out.basis = outbasis;
  return out;
}

template <typename coefficient>
List<ActionSpace<coefficient> > ActionSpace<coefficient>::Decomposition() const
{ SpaceTree<Rational> st;
  st.space = basis;
  for(int cfi=0; cfi<G->ccCount; cfi++)
  { ClassFunction<coefficient> cf;
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
  List<ActionSpace<coefficient> > out;
  for(int i=0; i<leaves.size; i++)
  { ActionSpace<coefficient> outeme;
    outeme.G = G;
    outeme.gens = gens;
    outeme.basis = leaves[i];
    out.AddOnTop(outeme.Reduced());
  }
  return out;
}

/*template <typename coefficient>
List<ActionSpace<coefficient> > ActionSpace<coefficient>::Decomposition() const
{ List<ActionSpace<coefficient> > out;
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
        ActionSpace<coefficient> midr;
        midr.G = G;
        midr.gens = gens;
        midr.basis = middle;
        if(midr.GetNumberOfComponents() == 1)
        { is_component[j] = true;
          dim_components += middle.size;
        }
        ActionSpace<coefficient> rightr;
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
  { ActionSpace<coefficient> s;
    s.G = this->G;
    s.gens = this->gens;
    s.basis = spaces[i];
    out.AddOnTop(s);
  }
  std::cout << "decomposition might be complete, found " << out.size << " components" << std::endl;
  return out;
}*/

template <typename coefficient>
ClassFunction<coefficient> ActionSpace<coefficient>::GetCharacter() const
{ ClassFunction<coefficient> X;
  X.G = G;
  X.data.SetSize(G->ccCount);
  for(int cci=0; cci < G->ccCount; cci++)
  { CoxeterElement g;
    g = G->GetCoxeterElement(G->conjugacyClasses[cci][0]);
    Matrix<coefficient> M;
    M.MakeIdMatrix(gens[0].NumRows);
    for(int gi=0; gi<g.reflections.size; gi++)
      M.MultiplyOnTheRight(gens[g.reflections[gi]]);
    X.data[cci] = M.GetTrace();
  }
  return X;
}

template <typename coefficient>
coefficient ActionSpace<coefficient>::GetNumberOfComponents() const
{ ClassFunction<coefficient> X;
  X = GetCharacter();
  return X.norm();
}

// /home/user/vectorpartition-code/test.cpp:103:26: error: there are no arguments to ‘MakeBasis’ that depend on a template parameter, so a declaration of ‘MakeBasis’ must be available [-fpermissive]
// /home/user/vectorpartition-code/test.cpp:103:26: note: (if you use ‘-fpermissive’, G++ will accept your code, but allowing the use of an undeclared name is deprecated)
// template <typename coefficient>
List<Vector<Rational> > MakeBasis(int d)
{ List<Vector<Rational> > out;
  for(int i=0; i<d; i++)
  { Vector<Rational> v;
    v.MakeEi(d,i);
    out.AddOnTop(v);
  }
  return out;
}

ActionSpace<Rational> StandardRepresentation(CoxeterGroup& G)
{ List<Matrix<Rational> > gens;
  for(int i=0; i<G.nGens; i++)
  { Matrix<Rational> geni = G.SimpleReflectionMatrix(i);
    gens.AddOnTop(geni);
  }
  ActionSpace<Rational> out;
  out.G = &G;
  out.gens = gens;
  List<Vector<Rational> > outb = MakeBasis(G.nGens);
  out.basis = outb;
  return out;
}

List<ActionSpace<Rational> > ComputeIrreducibleRepresentations(CoxeterGroup& G)
{ List<ActionSpace<Rational> > irreps;
  ActionSpace<Rational> sr = StandardRepresentation(G);
  irreps.AddOnTop(sr);
  List<ClassFunction<Rational> > chartable;
  chartable.AddOnTop(sr.GetCharacter());
  std::cout << sr.GetCharacter() << std::endl;
  List<ActionSpace<Rational> > newspaces;
  newspaces.AddOnTop(sr);
  while(newspaces.size > 0)
  { ActionSpace<Rational> nspace = newspaces.PopLastObject();
    for(int irri = 0; irri < irreps.size; irri++)
    { ActionSpace<Rational> tspace = nspace * irreps[irri];
      std::cout << "Decomposing " << tspace.GetCharacter() << std::endl;
      List<ActionSpace<Rational> > spaces = tspace.Decomposition();
      for(int spi = 0; spi < spaces.size; spi++)
      { if(spaces[spi].GetNumberOfComponents() == 1)
        { if(!chartable.Contains(spaces[spi].GetCharacter()))
          { chartable.AddOnTop(spaces[spi].GetCharacter());
            irreps.AddOnTop(spaces[spi]);
            newspaces.AddOnTop(spaces[spi]);
            std::cout << "we have " << irreps.size << " irreps" << std::endl;
          }
        }
      }
      if(irreps.size == G.ccCount)
        break;
    }
    if(irreps.size == G.ccCount)
      break;
  }
//  chartable.QuickSortAscending();
  for(int i=0; i<chartable.size; i++)
  { std::cout << chartable[i] << '\n';
    for(int j=0; j<irreps[i].gens.size; j++)
      std::cout << irreps[i].gens[j].ToString(&testformat) << '\n';
  }
  return irreps;
}
/*
template <typename vector>
class LinearSpace
{ public:
  List<vector> basis;

  void AddToSpace(const vector &v);
  int dim() const;
  bool Contains(const vector &v) const;
};

void AddToSpace(const vector &v)
{
}
*/
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
List<Vector<coefficient> > getunion(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W)
{ if(V.size == 0)
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
  { Vector<coefficient> v;
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
{ if((V.size==0) or (W.size==0))
  { List<Vector<coefficient> > out;
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
List<Vector<coefficient> > relative_complement(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &WW)
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
  { for(int i2=1; i2>=-1; i2-=2)
    { int i = i1*i2;
      while(pdiv(p,i))
      { if(!factors.Contains(i))
        { factors.AddOnTop(i);
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
{ Matrix<coefficient> MM; // idk what this does
  Selection s;
//  std::cout << "DestructiveKernel: GaussianEliminationByRows" << std::endl;
  M.GaussianEliminationByRows(M,MM,s);
//  std::cout << "DestructiveKernel: calculating kernel space" << std::endl;
  List<Vector<coefficient> > V;
  for(int i=0;i<s.selected.size;i++)
  { if(!s.selected[i])
      continue;
    Vector<coefficient> v;
    v.MakeEi(M.NumCols,i);
    int rowmap = 0;
    for(int j=0;j<M.NumCols;j++)
      if(!s.selected[j])
      {v[j] = -M.elements[rowmap][i];
       rowmap++;
      }
    V.AddOnTop(v);
  }
//  std::cout << "DestructiveKernel: done" << std::endl;
  return V;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveEigenspace(Matrix<coefficient> &M, const coefficient &l)
{ for(int i=0; i<M.NumCols; i++)
    M.elements[i][i] -= l;
  return DestructiveKernel(M);
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveColumnSpace(Matrix<coefficient>& M)
{ M.Transpose();
  Matrix<coefficient> dummy1;
  Selection dummy2;
  M.GaussianEliminationByRows(M,dummy1,dummy2);
  List<Vector<coefficient> > out;
  bool zerov;
  for(int i=0; i<M.NumRows; i++)
  { Vector<coefficient> v;
    v.MakeZero(M.NumCols); // initializing is not necessary
    zerov = true;
    for(int j=0; j<M.NumCols; j++)
    { v[j] = M.elements[i][j];
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
//  for(int i=0; found < n; i++){
//    if((i!=0) && (checkDivisorsOf%i!=0))
//      continue;
  for(int ii=0; ii<2*n+2; ii++) // lol, this did end up working though
  { int i = ((ii+1)/2) * (2*(ii%2)-1); // 0,1,-1,2,-2,3,-3,...
    std::cout << "checking " << i << " found " << found << std::endl;
    Matrix<Rational> M2 = M;
    Rational r = i;
    List<Vector<Rational> > V = DestructiveEigenspace(M2,r);
    if(V.size > 0){
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
{ Vector<coefficient> w;
  w.MakeZero(B.size);
  Matrix<coefficient> M;
  M.MakeZeroMatrix(B.size);
  for(int i=0;i<B.size;i++)
  { for(int j=0; j<v.size; j++)
      w[i] += B[i][j]*v[j];
    for(int j=0; j<B.size; j++)
      for(int k=0; k<v.size; k++)
        M.elements[i][j] += B[i][k] * B[j][k];
  }
  M.Invert();
  Vector<coefficient> v2 = M*w;
  Vector<coefficient> v3;
  v3.MakeZero(v.size);
  for(int i=0;i<B.size;i++)
    v3 += B[i] * v2[i];
  if(!(v3 == v))
  { Vector<coefficient> out;
    return out;
  }
  return v2;
}

template <typename coefficient>
Vector<coefficient> ActOnGroupRing(const CoxeterGroup& G, int g, const Vector<coefficient> &v)
{ Vector<coefficient> out;
  out.MakeZero(G.N);
  for(int i=0; i<G.N; i++)
    if(v[i] != 0)
      out[G.MultiplyElements(g,i)] = v[i];
  return out;
}

// this function name is a lie
template <typename coefficient>
bool is_isotypic_component(CoxeterGroup &G, const List<Vector<coefficient> > &V)
{ // pre-initial test: V isn't empty
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
  { int g = G.conjugacyClasses[i][0];
    coefficient tr = 0;
    for(int j=0; j<V.size; j++)
    { Vector<coefficient> v = ActOnGroupRing(G,g,V[j]);
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

Matrix<Rational> MatrixInBasis(const ClassFunction<Rational> &X, const List<Vector<Rational> > &B)
{ List<Vector<Rational> > rows;
  for(int i=0;i<B.size;i++)
  { Vector<Rational> v;
    v.MakeZero(B[0].size);
    rows.AddOnTop(v);
  }
  for(int i1 =0; i1<X.G->ccCount; i1++)
  { for(int i2=0; i2<X.G->conjugacyClasses[i1].size; i2++)
    { for(int j=0; j<X.G->N; j++)
      { for(int k=0; k<B.size; k++)
        { int l = X.G->MultiplyElements(X.G->conjugacyClasses[i1][i2],j);
          rows[k][l] = X.data[i1]*B[k][j];
        }
      }
    }
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


ElementMonomialAlgebra<CoxeterElement, Rational> FromClassFunction(const ClassFunction<Rational>& X)
{ ElementMonomialAlgebra<CoxeterElement, Rational> out;
  for(int i=0; i<X.G->ccCount; i++)
  { for(int j=0; j<X.G->ccSizes[i]; j++)
    { CoxeterElement tmp = X.G->GetCoxeterElement(X.G->conjugacyClasses[i][j]);
      out.AddMonomial(tmp,X.data[i]);
    }
  }
  return out;
}


template <typename coefficient>
Matrix<coefficient> GetMatrix(const ClassFunction<coefficient>& X)
{ Matrix<coefficient> M;
  M.MakeZeroMatrix(X.G->N);
  for(int i1=0; i1<X.G->ccCount; i1++)
  { for(int i2=0; i2<X.G->ccSizes[i1]; i2++)
    { int i=X.G->conjugacyClasses[i1][i2];
      for(int j=0; j<X.G->N; j++)
      { M.elements[X.G->MultiplyElements(i,j)][j] = X.data[i1];
      }
    }
  }
  return M;
}

int skipcount(int n, const List<int>& l)
{ int o = 0;
  for(int i=0;i<l.size;i++)
  { if(l[i] == n)
      return -1;
    if(l[i]<n)
    { o++;
    }
  }
  return n-o;
}

int popcnt(int i) // from the internet
{ i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return ((((i + (i >> 4)) & 0xF0F0F0F) * 0x1010101) & 0xffffffff) >> 24;
}

List<List<int> > powersetgrade(int G, const List<int>& l)
{ List<List<int> > out;
  for(int i=0; i<1<<l.size; i++)
  { if(popcnt(i)==G){
      List<int> outi;
      for(int j=0;j<l.size;j++){
        if(i&(1<<j))
          outi.AddOnTop(j);
      }
      out.AddOnTop(outi);
    }
  }
  return out;
}

template<typename Element>
Element minor_det(const Matrix<Element>& M, const List<int>& l)
{
  Matrix<Element> MM;
  MM.init(M.NumRows-l.size,M.NumCols-l.size);
  for(int i=0; i<M.NumRows; i++)
  { int ii = skipcount(i,l);
    if(ii == -1)
      continue;
    for(int j=0; j<M.NumCols; j++)
    { int jj = skipcount(j,l);
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
{ List<Element> p;
  p.SetSize(M.NumRows+1);
  List<int> rowids;
  for(int i=0; i<M.NumRows; i++)
    rowids.AddOnTop(i);
  for(int i=0; i<M.NumRows; i++)
  { Element acc = 0;
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
{
{1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
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

bool VerifyChartable(const CoxeterGroup &G, bool printresults = false){
  bool okay = true;
  if(printresults)
    std::cout << "one" << '\n';
  for(int i=0; i<G.ccCount; i++)
  { if(printresults)
      std::cout << G.characterTable[i].norm() << std::endl;
    if(G.characterTable[i].norm() != 1)
      okay = false;
  }
  if(printresults)
    std::cout << "zero" << '\n';
  for(int i=0; i<G.ccCount; i++)
    for(int j=0; j<G.ccCount; j++)
    { if(j==i)
        continue;
      if(G.characterTable[i].IP(G.characterTable[j]) != 0)
        okay = false;
      if(printresults)
        std::cout << G.characterTable[i].IP(G.characterTable[j]) << std::endl;
    }
  return okay;
}


int main(void){
  testformat.flagUseHTML = false;
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
    G.ComputeWeylGroup();
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


  DynkinType D;
  D.MakeSimpleType('F',4);
  Matrix<Rational> M;
  D.GetCartanSymmetric(M);
  std::cout << M << std::endl;

  CoxeterGroup G;
  G.MakeFrom(D);
  G.ComputeInitialCharacters();
  for(int i=0;i<G.characterTable.size;i++)
  std::cout << G.characterTable[i] << std::endl;
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

//  Vectors<Rational> V;
//  XP.FindZeroEigenSpace(V);
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
    ActionSpace<Rational> V2x2 = V2*V2;
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
    List<ActionSpace<Rational> > Vs;
    Vs = V2x2.Decomposition();
    for(int i=0; i<Vs.size; i++)
      std::cout << Vs[i].Reduced().GetCharacter() << std::endl;
  */

    List<ActionSpace<Rational> > irreps = ComputeIrreducibleRepresentations(G);

    std::string s;
    std::cin >> s;
    return 0;
}
