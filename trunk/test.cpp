#include "vpf.h"
// ^ that is first for precompilation

#include "vpfCharacters.h"
#include "vpfGraph.h"
#include "testlib.h"
#include <iostream>

// this is one reason test.cpp isn't even compiled into the actual calculator
FormatExpressions testformat;

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
List<Vector<coefficient> > DestructiveKernel(Matrix<coefficient> M)
{ Matrix<coefficient> MM; // idk what this does
  Selection s;
  M.GaussianEliminationByRows(M,MM,s);
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
  return V;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveEigenspace(Matrix<coefficient> M, coefficient l)
{ for(int i=0; i<M.NumCols; i++)
  M.elements[i][i] -= l;
  return DestructiveKernel(M);
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
//  std::cout << "w " << w << std::endl;
//  std::cout << "M\n" << M.ToString(&testformat) << std::endl;
  M.Invert();
//  std::cout << "M⁻¹\n" << M.ToString(&testformat) << std::endl;
  Vector<coefficient> v2 = M*w;
  Vector<coefficient> v3;
  v3.MakeZero(v.size);
  for(int i=0;i<B.size;i++)
    v3 += B[i] * v2[i];
  if(!(v3 == v))
  { std::cout << "v did not belong to B " << v-v3 << "left over" << std::endl;
  }
  return v2;
}

Matrix<Rational> MatrixInBasis(const Character<Rational> &X, const List<Vector<Rational> > &B)
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
  std::cout << rows << std::endl;
  Matrix<Rational> M;
  M.init(rows.size, rows.size);
  for(int i=0; i<rows.size; i++)
  { Vector<Rational> v = PutInBasis(rows[i],B);
    for(int j=0; j<rows.size; j++)
      M.elements[i][j] = v[j];
  }
  return M;
}


ElementMonomialAlgebra<CoxeterElement, Rational> FromCharacter(const Character<Rational>& X)
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
Matrix<coefficient> GetMatrix(const Character<coefficient>& X)
{ Matrix<coefficient> M;
  M.MakeZeroMatrix(X.G->N);
  for(int i1=0; i1<X.G->ccCount; i1++)
  { for(int i2=0; i2<X.G->ccSizes[i1]; i2++)
    { int i=X.G->conjugacyClasses[i1][i2];
      for(int j=0; j<X.G->N; j++)
      { M.elements[X.G->MultiplyElements(i,j)][j] = X.data[i1]/X.G->N;
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
  D.MakeSimpleType('B',3);
  Matrix<Rational> M;
  D.GetCartanSymmetric(M);
  std::cout << M << std::endl;

  CoxeterGroup G;
  G.MakeFrom(D);
  G.ComputeInitialCharacters();
  for(int i=0;i<G.characterTable.size;i++)
  std::cout << G.characterTable[i] << std::endl;

  for(int i=0;i<G.ccCount;i++){
    int n = G.conjugacyClasses[i][0];
    Matrix<Rational> M = G.TodorsVectorToMatrix(G.rhoOrbit[n]);
    std::cout << i << charpoly(M) << "\n\n";
  //  std::cout << M << '\n';
  //  std::cout << "\n\n";
  }

  G.characterTable.size = 0;
  for(int i=0; i<10; i++){
    Character<Rational> X;
    X.G = &G;
    for(int j=0; j<10; j++){
      X.data.AddOnTop(chartable[i][j]);
    }
    G.characterTable.AddOnTop(X);
  }

  std::cout << G.characterTable << std::endl;
/*
  GroupRingElement<Rational> XeP;
  XeP.MakeFromCharacter(G.characterTable[0]);
  std::cout << XeP.data << std::endl;

  GroupRingElement<Rational> E;
  E.MakeEi(&G,0);
  std::cout << E << std::endl;
  std::cout << XeP*E << std::endl;

  GroupRingElement<Rational> XstdP;
  XstdP.MakeFromCharacter(G.characterTable[2]);
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

  ElementMonomialAlgebra<CoxeterElement, Rational> Xstdp = FromCharacter(G.characterTable[2]);
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
  Matrix<Rational> ct;
  ct.init(G.ccCount,G.ccCount);
  for(int i=0; i<G.ccCount; i++)
    for(int j=0; j<G.ccCount; j++)
    { ct.elements[i][j] = chartable[i][j];
      ct.elements[i][j] /= G.N;
      ct.elements[i][j] *= G.ccSizes[j];
    }
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

  Character<Rational> X;
  X.G = &G;
  X.data = powers[5];
  std::cout << X << std::endl;

  List<Character<Rational> > tstct;
  for(int i=0;i<G.ccCount;i++)
    std::cout << X.IP(G.characterTable[i]) << std::endl;

  Matrix<Rational> XP;
  XP = GetMatrix(G.characterTable[2]);
  GroupRingElement<Rational> XPP;
  Vector<Rational> vv;
  XPP.MakeFromCharacter(G.characterTable[2]);
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

  Matrix<Rational> XP;
  for(int i=0; i<G.ccCount; i++)
  {
  XP = GetMatrix(G.characterTable[i]);
  XP *= G.characterTable[i].data[0];
  std::cout << G.characterTable[i] << std::endl;
  std::cout << DestructiveEigenspace(XP,one) << std::endl;
  }

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
    Character X = G.characterTable[2] - G.characterTable[0];
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
    std::string s;
    std::cin >> s;
    return 0;
}
