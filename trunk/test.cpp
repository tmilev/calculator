#include "vpf.h"
// ^ that is first for precompilation

#include "vpfCharacters.h"
#include "vpfGraph.h"
#include "testlib.h"
#include <iostream>



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

int chartable[10][10] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, -1, -1, 1, 1, -1, 1, 1, -1, -1},
{3, 1, 1, -1, 1, 0, 0, -1, -1, -3},
{3, -1, -1, -1, 1, 0, 0, -1, 1, 3},
{2, 0, -2, 2, 0, 1, -1, 0, 0, -2},
{2, 0, 2, 2, 0, -1, -1, 0, 0, 2},
{1, 1, -1, 1, -1, -1, 1, -1, 1, -1},
{1, -1, 1, 1, -1, 1, 1, -1, -1, 1},
{3, -1, 1, -1, -1, 0, 0, 1, 1, -3},
{3, 1, -1, -1, -1, 0, 0, 1, -1, 3}};


int main(void){
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



  ElementAssociativeAlgebra<CoxeterElement, Rational> x;
  ElementAssociativeAlgebra<CoxeterElement, Rational> y;

  x.MakeZero();
  CoxeterElement tempE;
  tempE.owner = &G;
  tempE.reflections.AddOnTop(1);

  y.MakeZero();
  CoxeterElement tempE2;
  tempE2.owner = &G;
  tempE2.reflections.AddOnTop(0);

  x.AddMonomial(tempE, 3);
  y.AddMonomial(tempE2,5);
  x*=y;
  std::cout << "\n\n\n\nHERE I AM: " << x.ToString() << "\n\n\n";

  ElementAssociativeAlgebra<CoxeterElement, Rational> all;
  for(int i=0; i<G.rhoOrbit.size; i++){
    CoxeterElement tmp = CoxeterElement(&G, G.DecomposeTodorsVector(G.rhoOrbit[i]));
    all.AddMonomial(tmp,1);
  }

  x *= all;
  std::cout << all.ToString() << std::endl;
  std::cout << x.ToString() << std::endl;

/*    std::cout << G.rho << std::endl;
    G.ComputeRhoOrbit();
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

//    ElementAssociativeAlgebra<FiniteGroupElement, Rational> a;
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
