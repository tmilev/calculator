#include "vpf.h"
#include "vpfCharacters.h"
#include "vpfGraph.h"

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfCharacters
(__FILE__, "Experimental code by Thomas: finite group characters sandbox. Not fully implemented yet.");


template<>
typename List<CoxeterElement>::OrderLeftGreaterThanRight FormatExpressions::GetMonOrder<CoxeterElement>()
{ return 0;
}

//--------------Implementing some features of some finite Coxeter groups
std::string CoxeterGroup::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
//  out << "this: " << this
  out << "Symmetric Cartan matrix: ";
  out << CGI::GetHtmlMathSpanPure(this->CartanSymmetric.ToString(theFormat));
  out << "<br>Conjugacy classes (total " << this->conjugacyClasses.size
  << ").\n";
  CoxeterElement tempElt;
  for (int i=0; i<this->conjugacyClasses.size; i++)
  { out << "<hr>Conjugacy class " << i+1 << " contains " << this->conjugacyClasses[i].size
    << " elements indexed by (the indexing order respects the Bruhat order): <br>";
    for (int j=0; j<this->conjugacyClasses[i].size; j++)
    { out << this->conjugacyClasses[i][j]+1;
      if (j!=this->conjugacyClasses[i].size-1)
        out << ", ";
    }
    out << "<br> elements of the class written in reflection notation: ";
    for (int j=0; j<this->conjugacyClasses[i].size; j++)
    { tempElt= this->GetCoxeterElement(this->conjugacyClasses[i][j]);
      out << tempElt.ToString(theFormat);
      if (j!=this->conjugacyClasses[i].size-1)
        out << ", ";
    }
  }
  return out.str();
}

void CoxeterGroup::MakeFrom(const DynkinType& D){
//    std::cout << "MakeFrom called" << std::endl;
    D.GetCartanSymmetric(this->CartanSymmetric);
    this->nGens = this->CartanSymmetric.NumCols;
    int n = this->CartanSymmetric.NumCols;
    HashedList<Vector<Rational> > roots;

    Vector<Rational> v;
    for(int i=0;i<nGens;i++){
        v.MakeEi(n,i);
        roots.AddOnTopNoRepetition(this->GetOrbit(v));
    }

    this->rootSystem = roots;
    Vector<Rational> derp;
    derp.MakeZero(n);
    for(int i=0;i<roots.size;i++){
        bool usethis = true;
        for(int j=0; j<n; j++)
            if(roots[i][j] < 0){
                usethis = false;
                break;
            }
        if(usethis == true){
//            std::cout << roots[i] << std::endl;
            derp += roots[i];
        }
    }
    this->rho = derp/2;
//    std::cout << this->rho << std::endl;
}

HashedList<Vector<Rational> > CoxeterGroup::GetOrbit(const Vector<Rational> &vv) const{
    List<Vector<Rational> > newones;
    HashedList<Vector<Rational> > orbit;
    newones.AddOnTop(vv);
    orbit.AddOnTop(vv);

    Vector<Rational> v,w;
    while(newones.size > 0){
        v = newones.PopLastObject();
        for(int i=0;i<nGens;i++){
            w = SimpleReflection(i,v);
            if(!orbit.Contains(w)){
                newones.AddOnTop(w);
                orbit.AddOnTop(w);
            }
        }
    }
    return orbit;
}

void CoxeterGroup::ComputeRhoOrbit(){
    rhoOrbit = GetOrbit(rho);
    N = rhoOrbit.size;
}

List<int> CoxeterGroup::DecomposeTodorsVector(const Vector<Rational> &vv) const{
    List<int> l;
    Vector<Rational> v=vv,w;
    while(v != rho){
        for(int i=0; i<nGens; i++){
            w = SimpleReflection(i,v);
            if(w>v){
                v = w;
                l.AddOnTop(i);
                break;
            }
        }
    }
    return l;
}

Vector<Rational> CoxeterGroup::ApplyList(const List<int> &l, const Vector<Rational> &vv) const{
    Vector<Rational> v = vv;
    for(int i = l.size-1; i>=0; i--){
        Rational x=0;
        for(int j=0; j<nGens; j++)
            x += v[j] * CartanSymmetric(l[i],j);
            //CartanSymmetric(i,j) is slower than CartanSymmetric.elements[i][j] but includes
            // index checking.
        v[l[i]] -= x * 2 / CartanSymmetric(l[i], l[i]);
    }
    return v;
}

Vector<Rational> CoxeterGroup::ComposeTodorsVector(const List<int> &l) const
{   return CoxeterGroup::ApplyList(l,rho);
}

Matrix<Rational> CoxeterGroup::SimpleReflectionMatrix(int i) const
{ Matrix<Rational> out;
  out.MakeIdMatrix(nGens);
  for(int j=0; j<nGens; j++)
    out.elements[i][j] -= CartanSymmetric.elements[i][j] / CartanSymmetric.elements[i][i] *2;
  return out;
}

Vector<Rational> CoxeterGroup::SimpleConjugation(int i, const Vector<Rational> &vv) const{
    Vector<Rational> v = SimpleReflection(i,rho);
    v = ApplyList(DecomposeTodorsVector(vv),v);
    return SimpleReflection(i,v); // one extra copy here
}

Matrix<Rational> CoxeterGroup::TodorsVectorToInvMatrix(const Vector<Rational> &vv) const{
    Matrix<Rational> M;
    M.MakeIdMatrix(nGens);

    Vector<Rational> v,w;
    v.MakeZero(nGens);
    for(int i=0;i<nGens;i++){
        v[i] = vv[i];
    }

    while(v != rho){
        for(int i=0; i<nGens; i++){
            w = SimpleReflection(i,v);
            if(w>v){
                v = w;
                for(int j=0; j<nGens; j++){
                    Rational x=0;
                    for(int k=0; k<nGens; k++)
                        x += M.elements[k][j] * CartanSymmetric.elements[i][k];
                    M.elements[i][j] -= x*2 / CartanSymmetric.elements[i][i];
                }
                break;
            }
        }
    }
    return M;
}

Matrix<Rational> CoxeterGroup::TodorsVectorToMatrix(const Vector<Rational> &v) const{
    Matrix<Rational> M;
    M.MakeIdMatrix(nGens);

    List<int> l = DecomposeTodorsVector(v);

    // there's probably a better way to do this and i just suck at linear algebra


    for(int i = l.size-1;i>=0;i--){
        for(int j=0; j<nGens; j++){
            Rational x=0;
            for(int k=0; k<nGens; k++)
                x += M.elements[k][j] * CartanSymmetric.elements[l[i]][k];
            M.elements[l[i]][j] -= x*2 / CartanSymmetric.elements[l[i]][l[i]];
        }
    }

    return M;
}

int CoxeterGroup::MultiplyElements(int i, int j) const
{ return rhoOrbit.GetIndex(ApplyList(DecomposeTodorsVector(rhoOrbit[i]),rhoOrbit[j]));
}

int CoxeterGroup::operator()(int i, int j) const
{ return MultiplyElements(i,j);
}

void CoxeterGroup::ComputeConjugacyClasses(){
    if(rhoOrbit.size == 0)
        ComputeRhoOrbit();
    Graph G = Graph(rhoOrbit.size,nGens);
    for(int i=0;i<rhoOrbit.size;i++){
        for(int j=0;j<nGens;j++){
            G.AddEdge(i,rhoOrbit.GetIndex(SimpleConjugation(j,rhoOrbit[i])));
        }
    }
    conjugacyClasses = G.DestructivelyGetConnectedComponents();
    ccCount = conjugacyClasses.size;
    ccSizes.SetExpectedSize(ccCount);
    for(int i=0;i<ccCount;i++)
        ccSizes.AddOnTop(conjugacyClasses[i].size);
}

void CoxeterGroup::ComputeSquares(){
    if(conjugacyClasses.size == 0)
        ComputeConjugacyClasses();

    List<int> l;
    l.SetExpectedSize(ccCount);

    for(int i=0;i<ccCount;i++){
        int n,m;
        n = conjugacyClasses[i][0];
        Vector<Rational> v = ApplyList(DecomposeTodorsVector(rhoOrbit[n]),rhoOrbit[n]);
        int vi = rhoOrbit.GetIndex(v);
        for(int i=0; i<ccCount; i++){
            if(conjugacyClasses[i].BSContains(vi)){
                m = i;
                break;
            }
        }
        l.AddOnTop(m);
    }
    squares = l;
}


void CoxeterGroup::ComputeInitialCharacters(){
  if(squares.size == 0)
    ComputeSquares();

  ClassFunction<Rational> Xe;
  Xe.G = this;
  Xe.data.SetExpectedSize(ccCount);
  for(int i=0; i<ccCount; i++)
    Xe.data.AddOnTop(1);
  characterTable.AddOnTop(Xe);

  ClassFunction<Rational> Xsgn;
  Xsgn.G = this;
  Xsgn.data.SetExpectedSize(ccCount);
  for(int i=0; i<ccCount; i++)
    Xsgn.data.AddOnTop(TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetDeterminant().floorIfSmall());
  characterTable.AddOnTop(Xsgn);

    // may not actually belong in the character table
  ClassFunction<Rational> Xstd;
  Xstd.G = this;
  Xstd.data.SetExpectedSize(ccCount);
  for(int i=0; i<ccCount; i++)
    Xstd.data.AddOnTop(TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetTrace().floorIfSmall());
//  Xstd = Xstd.ReducedWithChars();
  characterTable.AddOnTop(Xstd);

/*
  List<Character> allChars;
  allChars.AddListOnTop(characterTable);
  ClassFunction X,Y;
  int n;

  List<ClassFunction> products;
  for(int loopvar=1;loopvar<50;loopvar++)
  { for(int i=0; i<allChars.size; i++)
    { for(int j=0; j<allChars.size; j++)
      { products.AddOnTop(allChars[i]*allChars[j]);
      }
    }
    for(int i=0; i<allChars.size; i++)
    { for(int j=0; j<allChars.size; j++)
      { for(int k=0; k<allChars.size; k++)
        { products.AddOnTop(allChars[i]*allChars[j]);
        }
      }
    }
    while(products.size > 0)
    { X = products.PopLastObject();
      Y = X.ReducedWithChars(allChars);
      n = Y.norm();
      if(n>=1)
      { std::cout << X << Y << std::endl;
        if(n==1)
        { characterTable.AddOnTop(Y);
        }
        allChars.AddOnTop(Y);
      }
    }

    ClearCharsList(allChars);

  }*/

/*  List<ClassFunction> powers;
  powers.AddOnTop(Xstd);
  for(int loopvar=1;loopvar<50;loopvar++)
  {

    powers.AddOnTop(powers[loopvar-1]*Xstd);
  }

  for(int i=0; i<powers.size;i++){
    X = powers[i].ReducedWithChars(allChars);
    n = X.norm();
    if(n>=1){
      if(n==1)
        characterTable.AddOnTop(X);
      allChars.AddOnTop(X);
    }
  }
*/
  /*for(int i=0;i<allChars.size;i++)
  {
    X = allChars[i].Sym2();
    X = X.ReducedWithChars(allChars);
    n = X.norm();
    if(n>=1)
    { std::cout << allChars[i] << X << std::endl;
    }
    if(n==1){
      characterTable.AddOnTop(X);
      allChars.AddOnTop(X);
    }
    else if (n>1){
      allChars.AddOnTop(X);
    }

    X = allChars[i].Alt2();
    X = X.ReducedWithChars(allChars);
    n = X.norm();
    if(n>=1)
    { std::cout << allChars[i] << X << std::endl;
    }
    if(n==1){
      characterTable.AddOnTop(X);
      allChars.AddOnTop(X);
    }
    else if (n>1){
      allChars.AddOnTop(X);
    }
  }*/
  /*
  std::cout << "allchars " << allChars.size << '\n';
  for(int i=0;i<allChars.size;i++){
    std::cout << allChars[i] << '\n';
  }
  std::cout << "\n\n";
*/
}

Vector<Rational> CoxeterGroup::SimpleReflection(int i, const Vector<Rational> &v) const{
    Rational x=0;
    for(int j=0;j<this->nGens;j++)
        x += this->CartanSymmetric.elements[i][j] * v[j];
    Vector<Rational> w = v;
    w[i] -= x * 2 / CartanSymmetric.elements[i][i];
    return w;
}

CoxeterElement CoxeterGroup::GetCoxeterElement(int i)const
{ MacroRegisterFunctionWithName("CoxeterGroup::GetCoxeterElement");
  if (this->rhoOrbit.size<=i || i<0)
  { std::cout << "This is a programming error: group element indexed by " << i
    << " but the rho-orbit of the group has only " << this->rhoOrbit.size
    << " elements. The programmer either gave a bad index, or did not compute the rho orbit. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return CoxeterElement(this, this->DecomposeTodorsVector(rhoOrbit[i]));
}

//-------------------------------CoxeterElement--------------------------

void CoxeterElement::canonicalize()
{ if(reflections.size < 2)
    return;
  reflections = owner->DecomposeTodorsVector(owner->ComposeTodorsVector(reflections));
}

void CoxeterElement::operator*=(const CoxeterElement& other)
{ reflections = owner->DecomposeTodorsVector(owner->ApplyList(reflections, owner->ComposeTodorsVector(other.reflections)));
}

void CoxeterElement::operator=(const CoxeterElement& other)
{ this->reflections=other.reflections;
  this->owner=other.owner;
}

bool CoxeterElement::operator==(const CoxeterElement& other)const
{ // this is all stuffed on one line because the auto keyword is only in cxx11
  if (this->owner!=other.owner)
    return false;
  return owner->ComposeTodorsVector(reflections) == owner->ComposeTodorsVector(other.reflections);
}

unsigned int CoxeterElement::HashFunction(const CoxeterElement& input)
{ //input.reflections.HashFunction() doesn't work at the moment (int::HashFunciton does not exist)
  return MathRoutines::ListIntsHash(input.reflections);
}

std::string CoxeterElement::ToString(FormatExpressions* theFormat)const
{ //if (this->owner==0)
  //  return "(not initialized)";
  bool useReflectionNotation=theFormat==0? false : theFormat->flagUseReflectionNotation;
  std::stringstream out;
  if (useReflectionNotation)
  { out << "{";
    if (this->reflections.size==0)
      out << "id";
    for (int i=0; i<this->reflections.size; i++)
      out << "s_{" << this->reflections[i]+1 << "}";//      << "^{" << this->owner << "}";
    out << "}";
    return out.str();
  }
  out << "(";
  for(int i=0;i<reflections.size;i++){
    out << reflections[i];
    if(i<reflections.size-1)
      out << ", ";
  }
  out << ")";
  return out.str();
}



template <typename coefficient>
void Basis<coefficient>::AddVector(const Vector<coefficient>& v)
{ if(basis.NumCols == 0)
  { basis.init(v.size,v.size);
    basis.NumRows = 0;
  }
  if(basis.NumRows == basis.NumCols)
  { std::cout << "Programming error: attempting to add the "<< basis.NumRows << " vector to a Basis of degree " << basis.NumCols << std::endl;
    assert(false);
  }
  haveGramMatrix = false;
  for(int i = 0; i<v.size; i++)
      basis.elements[basis.NumRows][i] = v[i];
  basis.NumRows++;
}

template <typename coefficient>
void Basis<coefficient>::ComputeGramMatrix()
{ int r = basis.NumRows;
  int d = basis.NumCols;
  gramMatrix.MakeZeroMatrix(r);
  for(int i=0; i<r; i++)
    for(int j=0; j<r; j++)
      for(int k=0; k<d; k++)
        gramMatrix.elements[i][j] += basis.elements[i][k] * basis.elements[j][k];
   gramMatrix.Invert();
   haveGramMatrix = true;
}

template <typename coefficient>
Vector<coefficient> Basis<coefficient>::PutInBasis(const Vector<coefficient>& v)
{ if(!haveGramMatrix)
    ComputeGramMatrix();
  return gramMatrix*(basis*v);
}

template <typename coefficient>
Matrix<coefficient> Basis<coefficient>::PutInBasis(const Matrix<coefficient>& in)
{ if(!haveGramMatrix)
    ComputeGramMatrix();
  return gramMatrix*(basis*in);
}


template <typename coefficient>
bool VectorSpace<coefficient>::AddVector(const Vector<coefficient>& v)
{ Vector<coefficient> tmp = v;
  return AddVectorDestructively(tmp);
}

template <typename coefficient>
bool VectorSpace<coefficient>::AddVectorDestructively(Vector<coefficient>& v)
{ if(fastbasis.NumRows == 0)
  { fastbasis.MakeZeroMatrix(v.size);
    degree = v.size;
    if(v.IsEqualToZero())
    { rank = 0;
      return false;
    }
    for(int i=0; i<v.size; i++)
      fastbasis.elements[0][i] = v[i];
    rank = 1;
    return true;
  }
  int jj=0;
  for(int i=0; i<fastbasis.NumRows; i++)
  { while((jj<v.size)&&(v[jj] == 0))
      jj++;
    if(jj==v.size)
      return false;
    int j = i;
    for(;(j<fastbasis.NumCols)&&(fastbasis.elements[i][j] == 0); j++);
    if(jj<j)
    { // memmove()
      for(int bi=degree-1; bi>i; bi--)
        for(int bj=0; bj<fastbasis.NumCols; bj++)
          fastbasis.elements[bi][bj] = fastbasis.elements[bi-1][bj];
      for(int bj=0; bj<fastbasis.NumCols; bj++)
        fastbasis.elements[i][bj] = v[bj];
      Matrix<coefficient> one;
      Selection two;
      fastbasis.GaussianEliminationByRows(fastbasis, one, two);
      rank++;
      return true;
    }
    if(jj>j)
      continue;
    coefficient x = -v[jj]/fastbasis.elements[i][j];
    for(int jjj=jj; jjj<v.size; jjj++)
      v[jjj] += x*fastbasis.elements[i][jjj];
  }
  if(v.IsEqualToZero())
    return false;
  //realloc()
  Matrix<coefficient> tmp = fastbasis;
  fastbasis.init(fastbasis.NumRows+1,fastbasis.NumCols);
  for(int i=0; i<tmp.NumRows; i++)
    for(int j=0; j<tmp.NumCols; j++)
      fastbasis.elements[i][j] = tmp.elements[i][j];
  for(int j=0; j<fastbasis.NumCols; j++)
    fastbasis.elements[fastbasis.NumRows-1][j] = v[j];
  rank++;
  return true;
}

template <typename coefficient>
bool VectorSpace<coefficient>::AddVectorToBasis(const Vector<coefficient>& v)
{ if(AddVector(v))
  { basis.AddVector(v);
    return true;
  }
  return false;
}



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
//  std::cout << in.ToString(&testformat) << '\n' << BM.ToString(&testformat) << '\n' << out.ToString(&testformat) << std::endl;
}



// g gets eaten up by this algorithm lol
template <typename coefficient>
Matrix<coefficient> TrixTree<coefficient>::GetElement(CoxeterElement &g, const List<Matrix<coefficient> > &gens)
{ if(M.NumCols == 0)
    M.MakeIdMatrix(gens[0].NumCols);
  if(g.reflections.size == 0)
    return M;
  int i = g.reflections.PopLastObject();
  if(others.size == 0)
    others.SetSize(gens.size);
  if(others[i].M.NumCols == 0)
    others[i].M = M * gens[i];
  return others[i].GetElement(g,gens);
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
{  for(int i=0; i<subspaces.size; i++)
   {  List<Vector<coefficient> > U = intersection(V,subspaces[i].space);
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
   {  SpaceTree<coefficient> vst;
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
CoxeterRepresentation<coefficient> CoxeterRepresentation<coefficient>::operator*(const CoxeterRepresentation<coefficient>& other) const
{ CoxeterRepresentation<coefficient> U;

  int Vd = this->basis[0].size;
   int Wd = other.basis[0].size;
   int Ud = Vd*Wd;
   for(int vi=0; vi<this->basis.size; vi++)
   {  for(int wi=0; wi<other.basis.size; wi++)
      {  Vector<coefficient> u;
         u.SetSize(Ud);
         for(int i=0; i<Vd; i++)
            for(int j=0; j<Wd; j++)
               u[i*Wd+j] = this->basis[vi][i] * other.basis[wi][j];
         U.basis.AddOnTop(u);
      }
   }

   U.gens.SetSize(gens.size);
   for(int i=0; i<gens.size; i++)
     U.gens[i].AssignTensorProduct(gens[i],other.gens[i]);

   /* would that it be this simple
   if((classFunctionMatrices.size > 0) && (other.classFunctionMatrices.size > 0))
   { U.classFunctionMatrices.SetSize(G->ccCount);
     for(int i=0; i<classFunctionMatrices.size; i++)
     { if((classFunctionMatrices[i].NumCols>0) && (other.classFunctionMatrices[i].NumCols>0))
        U.classFunctionMatrices[i].AssignTensorProduct(classFunctionMatrices[i],other.classFunctionMatrices[i]);
     }
   }*/


   U.G = G;
   return U;
}

template <typename coefficient>
Matrix<coefficient> CoxeterRepresentation<coefficient>::ClassFunctionMatrix(ClassFunction<coefficient> cf)
{  Matrix<coefficient> M;
   M.MakeZeroMatrix(this->gens[0].NumRows);
   if(classFunctionMatrices.size == 0)
    classFunctionMatrices.SetSize(G->ccCount);
   for(int cci=0; cci<this->G->ccCount; cci++)
   {  if(cf[cci] == 0)
         continue;
      if(classFunctionMatrices[cci].NumCols == 0)
      { std::cout << "Generating class function matrix " << cci << " with dimension " << gens[0].NumCols << "(cc has " << G->conjugacyClasses[cci].size << ")" << std::endl;
        classFunctionMatrices[cci].MakeZeroMatrix(gens[0].NumCols);
        for(int icci=0; icci<this->G->conjugacyClasses[cci].size; icci++)
        { //Matrix<coefficient> Mi;
          //Mi.MakeIdMatrix(this->gens[0].NumCols);
          CoxeterElement g = G->GetCoxeterElement(G->conjugacyClasses[cci][icci]);
          classFunctionMatrices[cci] += elements.GetElement(g,gens);
          //for(int gi=g.reflections.size-1; ; gi--)
          //{  if(gi < 0)
          //      break;
          //    Mi.MultiplyOnTheRight(gens[g.reflections[gi]]);
          //}
          //classFunctionMatrices[cci] += Mi;
        }
      }
      for(int i=0; i<M.NumRows; i++)
        for(int j=0; j<M.NumCols; j++)
          M.elements[i][j] += classFunctionMatrices[cci].elements[i][j] * cf[cci];
   }
   return M;
}

template <typename coefficient>
CoxeterRepresentation<coefficient> CoxeterRepresentation<coefficient>::Reduced() const
{  int d = basis.size;

   Matrix<coefficient> GM;
   GM.init(d, d);
   for(int i=0; i<d; i++)
      for(int j=0; j<d; j++)
         GM.elements[i][j] = this->basis[i].ScalarEuclidean(this->basis[j]);
   GM.Invert();

 /*
   Matrix<coefficient> BM;
   BM.init(d,basis[0].size);
   for(int i=0; i<d; i++)
    for(int j=0; j<basis[0].size; j++)
      BM.elements[i][j] = basis[i][j];
   Matrix<coefficient> one;
   Selection two;
   BM.GaussianEliminationByRows(BM,one,two);
*/

   CoxeterRepresentation<coefficient> out;
   out.gens.SetSize(gens.size);
   for(int i=0; i<gens.size; i++)
//     MatrixInBasisFast(out.gens[i], gens[i], BM);
     MatrixInBasis(out.gens[i],gens[i],basis,GM);

   out.G = G;
   for(int i=0; i<d; i++) // this should go in its own function at some point
   {  Vector<coefficient> v;
      v.MakeEi(d,i);
      out.basis.AddOnTop(v);
   }

   if(classFunctionMatrices.size > 0)
   { out.classFunctionMatrices.SetSize(G->ccCount);
     for(int i=0; i<classFunctionMatrices.size; i++)
      { if(classFunctionMatrices[i].NumRows > 0)
        { //MatrixInBasisFast(out.classFunctionMatrices[i],classFunctionMatrices[i],BM);
          MatrixInBasis(out.classFunctionMatrices[i],classFunctionMatrices[i],basis,GM);
        }
      }

   }
   return out;
}

template <typename coefficient>
VectorSpace<coefficient> CoxeterRepresentation<coefficient>::FindDecentBasis() const
{ VectorSpace<coefficient>  V;
  int d = gens[0].NumCols;
  for(int geni=0; geni<gens.size; geni++)
  { List<Vector<coefficient> > ess = eigenspaces(gens[geni]);
    for(int essi=0; essi<ess.size; essi++)
    { for(int esi=0; esi<ess[essi].size; esi++)
      { // the best laid coding practices of mice and men oft go astray
        if(!V.AddVectorToBasis(ess[essi][esi]))
          return V;
      }
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

template <typename coefficient>
ClassFunction<coefficient> CoxeterRepresentation<coefficient>::GetCharacter()
{  if(character.data.size)
     return character;

   character.G = G;
   character.data.SetSize(G->ccCount);
   for(int cci=0; cci < G->ccCount; cci++)
   {  CoxeterElement g;
      g = G->GetCoxeterElement(G->conjugacyClasses[cci][0]);
      Matrix<coefficient> M;
      M.MakeIdMatrix(gens[0].NumRows);
      for(int gi=0; gi<g.reflections.size; gi++)
         M.MultiplyOnTheRight(gens[g.reflections[gi]]);
      character.data[cci] = M.GetTrace();
   }
   return character;
}

template <typename coefficient>
coefficient CoxeterRepresentation<coefficient>::GetNumberOfComponents()
{  ClassFunction<coefficient> X;
   X = GetCharacter();
   return X.norm();
}

CoxeterRepresentation<Rational> CoxeterGroup::StandardRepresentation()
{  List<Matrix<Rational> > gens;
   for(int i=0; i<this->nGens; i++)
   {  Matrix<Rational> geni = this->SimpleReflectionMatrix(i);
      gens.AddOnTop(geni);
   }
   CoxeterRepresentation<Rational> out;
   out.G = this;
   out.gens = gens;
   Vectors<Rational> outb;
   outb.MakeEiBasis(this->nGens);
   out.basis = outb;
   return out;
}

void CoxeterGroup::ComputeIrreducibleRepresentations()
{  CoxeterRepresentation<Rational> sr = this->StandardRepresentation();
   ClassFunction<Rational> cfmgen;
   cfmgen.G = this;
   cfmgen.data.SetSize(this->ccCount);
   for(int i=0; i<this->ccCount; i++)
     cfmgen.data[i] = 1;
   sr.ClassFunctionMatrix(cfmgen);
   this->irreps.AddOnTop(sr);
   this->characterTable.SetSize(0);
   characterTable.AddOnTop(sr.GetCharacter());
   std::cout << sr.GetCharacter() << std::endl;
   List<CoxeterRepresentation<Rational> > newspaces;
   newspaces.AddOnTop(sr);
   while(newspaces.size > 0)
   {  CoxeterRepresentation<Rational> nspace = newspaces.PopLastObject();
      for(int irri = 0; irri < irreps.size; irri++)
      {  CoxeterRepresentation<Rational> tspace = nspace * irreps[irri];
         std::cout << "Decomposing " << tspace.GetCharacter() << std::endl;
         List<CoxeterRepresentation<Rational> > spaces = tspace.Decomposition(characterTable,irreps);
         for(int spi = 0; spi < spaces.size; spi++)
         {  if(spaces[spi].GetNumberOfComponents() == 1)
            {  if(!characterTable.Contains(spaces[spi].GetCharacter()))
               {  characterTable.AddOnTop(spaces[spi].GetCharacter());
                  irreps.AddOnTop(spaces[spi]);
                  newspaces.AddOnTop(spaces[spi]);
                  std::cout << "we have " << irreps.size << " irreps" << std::endl;
               }
            }
         }
         if(irreps.size == ccCount)
            break;
      }
      if(irreps.size == ccCount)
         break;
   }
//  chartable.QuickSortAscending();
   for(int i=0; i<characterTable.size; i++)
   {  std::cout << characterTable[i] << '\n';
      for(int j=0; j<irreps[i].gens.size; j++)
         std::cout << irreps[i].gens[j].ToString(0) << '\n';
   }
   for(int i=0; i<characterTable.size; i++)
   { std::cout << characterTable[i] << '\n';
   }
   std::cout << characterTable.size << std::endl;
}



template <typename integral>
integral gcd(integral a, integral b)
{  integral temp;
   while(!(b==0))
   {  temp= a % b;
      a=b;
      b=temp;
   }
   return a;
}

template <typename integral>
integral lcm(integral a, integral b)
{ std::cout << "lcm" << a << ',' << b << std::endl;
  std::cout << (a*b)/gcd(a,b) << std::endl;
  return (a*b)/gcd(a,b);
}

// trial division is pretty good.  the factors of 2 were cleared earlier
// by masking. dividing by 3 a few times earlier and only checking
// 1 and 5 mod 6 would require more complicated iteration that +=2
void factor_integer_l(int n, List<int> f)
{  for(int i=2; i<sqrt(n); i+=2)
   {  if(n%i==0)
      {  f.AddOnTop(i);
         factor_integer_l(n/i,f);
         return;
      }
   }
}

/* It might be a good idea to give an error for attempts to factor 0 */
List<int> factor_integer(int n)
{  List<int> f;
   if(n==0)
      return f;
   if(n<0)
      n = -n;
   if(n==1)
      return f;
   while((n&1) == 0)
   {  f.AddOnTop(2);
      n = n>>1;
   }
   factor_integer_l(n,f);
   return f;
}



int ithfactor(int i, List<int> f)
{  int acc = 1;
   for(int j=0; j<f.size; j++)
      if(i&i<<j)
         acc = acc * f[j];
   return acc;
}


// Univariate dense polynomials.
template <typename coefficient>
class UDPolynomial
{
public:
   // "So the last shall be first, and the first last" -- Matthew 20:12
   List<coefficient> data;

//  UDPolynomial<coefficient> operator+(const UDPolynomial<coefficient>& right) const;
   void operator+=(const UDPolynomial<coefficient>& right);
//  UDPolynomial<coefficient> operator-(const UDPolynomial<coefficient>& right) const;
   void operator-=(const UDPolynomial<coefficient>& right);
   UDPolynomial<coefficient> operator*(const UDPolynomial<coefficient>& right) const;
//  UDPolynomial<coefficient> operator*(const coefficient& right) const;
   void operator*=(const coefficient& right);
   UDPolynomial<coefficient> TimesXn(int n) const;
// Quick divisibility test
// bool DivisibleBy(const UDPolynomial<coefficient>& divisor) const;
   struct DivisionResult<UDPolynomial<coefficient> > DivideBy(const UDPolynomial<coefficient>& right) const;
   UDPolynomial<coefficient> operator/(const UDPolynomial<coefficient>& divisor) const;
   UDPolynomial<coefficient> operator%(const UDPolynomial<coefficient>& divisor) const;
   void operator/=(const coefficient& right);
   coefficient operator()(const coefficient& x) const;
   void ClearDenominators();
   void FormalDerivative();
   void SquareFree();
   List<coefficient> GetRoots() const;
   void DoKronecker() const;
//  static List<UDPolynomial<coefficient> > LagrangeInterpolants(List<coefficient> xs);
   coefficient& operator[](int i) const;
   bool operator<(const UDPolynomial<coefficient>& right) const;
   bool operator==(int other) const;
};

template <typename coefficient>
coefficient& UDPolynomial<coefficient>::operator[](int i) const
{ return data[i];
}

template <typename coefficient>
coefficient UDPolynomial<coefficient>::operator()(const coefficient &x) const
{  coefficient acc = 0;
   coefficient y = 1;
   for(int i=0; i<data.size; i++)
   {  acc += y*data[i];
      y *= x;
   }
   return acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator+=(const UDPolynomial<coefficient>& right)
{  int t = min(right.data.size, data.size);
   for(int i=0; i<t; i++)
      data[i] += right.data[i];

   if(right.data.size > data.size)
   {  int n = data.size;
      data.SetSize(right.data.size);
      for(int i=n; i<right.data.size; i++)
         data[i] = right.data[i];
   }
   else
      while((data.size != 0) and (data[data.size-1] != 0))
         data.size--;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator-=(const UDPolynomial<coefficient>& right)
{  // int t = min(right.data.size, data.size); // wtf lol
  int t = right.data.size;
  if(data.size < t)
    t = data.size;

  for(int i=0; i<t; i++)
    data[i] -= right.data[i];

  if(right.data.size > data.size)
  {  int n = data.size;
     data.SetSize(right.data.size);
     for(int i=n; i<right.data.size; i++)
       data[i] = -right.data[i];
  }
  else
    while((data.size != 0) and (data[data.size-1] == 0))
      data.size--;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator*(const UDPolynomial<coefficient>& right) const
{  UDPolynomial<coefficient> out;
   out.data.SetSize(data.size+right.data.size-1);
   for(int i=0; i<out.data.size; i++)
      out.data[i] = 0;
   for(int i=0; i<data.size; i++)
      for(int j=0; j<right.data.size; j++)
         out.data[i+j] += data[i]*right.data[j];
   return out;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::TimesXn(int n) const
{  UDPolynomial<coefficient> out;
   out.data.SetSize(data.size+n);
   for(int i=0; i<n; i++)
     out.data[i] = 0;
   // not memcpy()
   for(int i=0; i<data.size; i++)
      out.data[i+n] = data[i];

   return out;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator*=(const coefficient& right)
{  for(int i=0; i<data.size; i++)
      data[i] *= right;
}

template <typename coefficient>
struct DivisionResult<UDPolynomial<coefficient> > UDPolynomial<coefficient>::DivideBy(const UDPolynomial<coefficient>& divisor) const
{  struct DivisionResult<UDPolynomial<coefficient> > out;
   out.remainder = *this;
   if(data.size < divisor.data.size)
      return out;
   int r = data.size - divisor.data.size + 1;
   out.quotient.data.SetSize(r);
   for(int i=r-1; i!=-1; i--)
   {  if(out.remainder.data.size - divisor.data.size != i)
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
  {if(data.size == 0)
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
{  int acc = 1;
   for(int i=0; i<data.size; i++)
      acc = lcm(acc,data[i].GetDenominator());
   *this *= acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::FormalDerivative()
{  if(data.size < 2)
      data.size = 0;
   for(int i=1; i<data.size; i++)
      data[i-1] = i*data[i];
   data.size--;
}

template <typename coefficient>
bool UDPolynomial<coefficient>::operator<(const UDPolynomial<coefficient>& right) const
{  if(data.size < right.data.size)
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
std::ostream& operator<<(std::ostream& out, const UDPolynomial<coefficient>& p)
{ if(p.data.size == 0)
    out << 0;
  else
  { for(int i=p.data.size-1; i!=0; i--)
    { if(p.data[i] == 0)
        continue;
      if(i!=p.data.size-1)
        out << " + ";
      out << p.data[i];
      if(i>1)
        out << "x^" << i;
      else if(i==1)
        out << "x";
    }
  }
  return out;
}

//incorrect, for now
template <typename coefficient>
UDPolynomial<coefficient> MinPoly(Matrix<coefficient> M)
{ int n = M.NumCols;
  UDPolynomial<coefficient> real_out;
  real_out.data.SetSize(1);
  real_out.data[0] = 1;
  for(int col = 0; col < n; col++)
  { VectorSpace<coefficient> vs;
    Vector<coefficient> v,w;
    v.MakeEi(n,0);
    vs.AddVectorToBasis(v);
    for(int i=0; i<n; i++)
    { w = M*v;
      if(!vs.AddVectorToBasis(w))
        break;
      v = w;
    }
    Vector<coefficient> p = vs.basis.PutInBasis(w);
    UDPolynomial<coefficient> out;
    out.data.SetSize(p.size+1);
    for(int i=0; i<p.size; i++)
      out.data[i] = p[i];
    out.data[p.size] = 1;
    real_out = lcm(real_out,out);
  }
  return real_out;
}



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


// guess at integers
List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational> &M, int checkDivisorsOf=0)
{  int n = M.NumCols;
   List<List<Vector<Rational> > > spaces;
   int found = 0;
//  for(int i=0; found < n; i++){
//    if((i!=0) && (checkDivisorsOf%i!=0))
//      continue;
   UDPolynomial<Rational> p = MinPoly(M);
   for(int ii=0; ii<2*n+2; ii++) // lol, this did end up working though
   {  int i = ((ii+1)/2) * (2*(ii%2)-1); // 0,1,-1,2,-2,3,-3,...
      std::cout << "checking " << i << " found " << found << std::endl;
      Rational r = i;
      if(p(r) == 0)
      { Matrix<Rational> M2 = M;
        List<Vector<Rational> > V = DestructiveEigenspace(M2,r);
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

Matrix<Rational> MatrixInBasis(const ClassFunction<Rational> &X, const List<Vector<Rational> > &B)
{  List<Vector<Rational> > rows;
   for(int i=0; i<B.size; i++)
   {  Vector<Rational> v;
      v.MakeZero(B[0].size);
      rows.AddOnTop(v);
   }
   for(int i1 =0; i1<X.G->ccCount; i1++)
   {  for(int i2=0; i2<X.G->conjugacyClasses[i1].size; i2++)
      {  for(int j=0; j<X.G->N; j++)
         {  for(int k=0; k<B.size; k++)
            {  int l = X.G->MultiplyElements(X.G->conjugacyClasses[i1][i2],j);
               rows[k][l] = X.data[i1]*B[k][j];
            }
         }
      }
   }
   Matrix<Rational> M;
   M.init(rows.size, rows.size);
   for(int i=0; i<rows.size; i++)
   {  Vector<Rational> v = PutInBasis(rows[i],B);
      for(int j=0; j<rows.size; j++)
         M.elements[i][j] = v[j];
   }
   return M;
}


ElementMonomialAlgebra<CoxeterElement, Rational> FromClassFunction(const ClassFunction<Rational>& X)
{  ElementMonomialAlgebra<CoxeterElement, Rational> out;
   for(int i=0; i<X.G->ccCount; i++)
   {  for(int j=0; j<X.G->ccSizes[i]; j++)
      {  CoxeterElement tmp = X.G->GetCoxeterElement(X.G->conjugacyClasses[i][j]);
         out.AddMonomial(tmp,X.data[i]);
      }
   }
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

bool VerifyChartable(const CoxeterGroup &G, bool printresults = false)
{  bool okay = true;
   if(printresults)
      std::cout << "one" << '\n';
   for(int i=0; i<G.ccCount; i++)
   {  if(printresults)
         std::cout << G.characterTable[i].norm() << std::endl;
      if(G.characterTable[i].norm() != 1)
         okay = false;
   }
   if(printresults)
      std::cout << "zero" << '\n';
   for(int i=0; i<G.ccCount; i++)
      for(int j=0; j<G.ccCount; j++)
      {  if(j==i)
            continue;
         if(G.characterTable[i].IP(G.characterTable[j]) != 0)
            okay = false;
         if(printresults)
            std::cout << G.characterTable[i].IP(G.characterTable[j]) << std::endl;
      }
   return okay;
}

bool CommandList::innerWeylGroupIrrepsAndCharTable
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!theCommands.innerWeylGroupConjugacyClasses(theCommands, input, output))
    return false;
  if (!output.IsOfType<CoxeterGroup>())
    return true;
  CoxeterGroup& theGroup=output.GetValuENonConstUseWithCaution<CoxeterGroup>();
  theGroup.ComputeInitialCharacters();
  std::stringstream out;
  for(int i=0; i<theGroup.characterTable.size; i++)
    out << theGroup.characterTable[i] << "<br>";
  theGroup.ComputeIrreducibleRepresentations();
  out << theGroup.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerWeylGroupConjugacyClasses(CommandList& theCommands, const
 Expression& input, Expression& output)
{ SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input, thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  CoxeterGroup tmpG;
  tmpG.MakeFrom(thePointer->theWeyl.theDynkinType);
  output.AssignValue(tmpG, theCommands);
  CoxeterGroup& theGroup=output.GetValuENonConstUseWithCaution<CoxeterGroup>();
  if (theGroup.CartanSymmetric.NumRows>4)
    return output.AssignValue<std::string>
    ("I have been instructed not to do this for Weyl groups of rank greater \
     than 4 because of the size of the computation.", theCommands);
  theGroup.ComputeConjugacyClasses();
  return true;
}

bool CommandList::innerCoxeterElement(CommandList& theCommands, const Expression
& input, Expression& output)
{ //if (!input.IsSequenceNElementS(2))
  //return output.SetError("Function Coxeter element takes two arguments.", theCommands);
  if(input.children.size<2){
    return output.SetError("Function CoxeterElement needs to know what group the element belongs to", theCommands);
  }
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  List<int> theReflections;
  for(int i=2; i<input.children.size; i++){
    int tmp;
    if (!input[i].IsSmallInteger(& tmp))
      return false;
    theReflections.AddOnTop(tmp-1);
  }
  CoxeterGroup theGroup;
  theGroup.MakeFrom(thePointer->theWeyl.theDynkinType);
  CoxeterElement theElt;
  int indexOfOwnerGroupInObjectContainer=
  theCommands.theObjectContainer.theCoxeterGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  //std::cout << "Group type: " << theGroup.ToString() << "<br>Index in container: "
  //<< indexOfOwnerGroupInObjectContainer;

  theElt.owner=&theCommands.theObjectContainer.theCoxeterGroups[indexOfOwnerGroupInObjectContainer];
  //std::cout << "<br>theElt.owner: " << theElt.owner;
//  std::cout << "<b>Not implemented!!!!!</b> You requested reflection indexed by " << theReflection;
  for(int i=0; i<theReflections.size; i++){
    if (theReflections[i] >= thePointer->GetRank() || theReflections[i] < 0)
      return output.SetError("Bad reflection index", theCommands);
  }
//  std::cout << "\n" << theGroup.rho << " " << theElt.owner->rho << std::endl;
  theElt.reflections=(theReflections);
  theElt.canonicalize();
  return output.AssignValue(theElt, theCommands);
}

bool CommandList::innerClassFunction(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerClassFunction");
  SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  CoxeterGroup theGroup;
  theGroup.MakeFrom(thePointer->theWeyl.theDynkinType);
  CoxeterElement theElt;
  int indexOfOwnerGroupInObjectContainer=
  theCommands.theObjectContainer.theCoxeterGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  theElt.owner=&theCommands.theObjectContainer.theCoxeterGroups[indexOfOwnerGroupInObjectContainer];

  theGroup.ComputeInitialCharacters();
  if(input.IsListNElements(3))
  { int theIndex;
    if(!input[2].IsSmallInteger(&theIndex))
      return output.SetError("Character index must be an integer", theCommands);
    if(theIndex < 0 || theIndex > theGroup.ccCount)
      return output.SetError("Character index must be between 0 and the number of conjugacy classes", theCommands);
    if(theIndex >= theGroup.characterTable.size)
      return output.SetError("Unfortunately, tom doesn't know how to calculate that one.  sorry.", theCommands);
    return output.AssignValue(theGroup.characterTable[theIndex], theCommands);
  }

  if(input.IsListNElements(theGroup.ccCount+1+1))
  { int n = theGroup.ccCount;
    List<int> X;
    for(int i=0; i<n; i++)
    { int tmp;
      if (!input[i].IsSmallInteger(& tmp))
      { theCommands.Comments
        << "<hr>While computing innerClassFunction, got user input: " << input[i].ToString()
        << " which is not a small integer - possible user typo?";
        return false;
      }
      X.AddOnTop(tmp);
    }
    ClassFunction<Rational> theChar;
    theChar.G = &theGroup;
    theChar.data = X;
    return output.AssignValue(theChar, theCommands);
  }
  return output.SetError("Class functions may be selected by character index or entered by hand.", theCommands);
}

