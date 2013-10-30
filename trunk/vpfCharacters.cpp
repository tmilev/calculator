//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math3_Characters.h"
#include "vpfHeader2Math4_Graph.h"
static ProjectInformationInstance ProjectInfoVpfCharacters(__FILE__, "Finite group characters sandbox. Work in progress by Thomas. ");


extern FormatExpressions consoleFormat;

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
  if (this->irreps.size>0)
  { out << this->irreps.size << "out of " << this->conjugacyClasses.size << " irreducible finite dimensional representations "
    << " were computed. ";
    for (int i=0; i<this->irreps.size; i++)
    { out << "<br>Representation V_{" << i+1 << "}<br>";
      out << this->irreps[i].ToString(theFormat);
    }
  }
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

void CoxeterGroup::MakeFrom(const DynkinType& D)
{ D.GetCartanSymmetric(this->CartanSymmetric);
  ComputeElements();
}

void CoxeterGroup::MakeFrom(const Matrix<Rational>& M)
{ this->CartanSymmetric = M;
  ComputeElements();
}

void CoxeterGroup::ComputeElements(){
//    std::cout << "MakeFrom called" << std::endl;
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
    if(rho.size == 0)
      ComputeElements();
    rhoOrbit = GetOrbit(rho);
    N = rhoOrbit.size;
}

void ElementWeylGroup::MakeCanonical()
{ this->CheckInitialization();
  if (this->owner->rho.size==0)
    this->owner->ComputeRho(false);
  Vector<Rational> theVector=this->owner->rho;
  this->owner->ActOn(*this, theVector);
  int theRank=this->owner->GetDim();
  this->reflections.SetSize(0);
  while (theVector!=this->owner->rho)
    for (int i=0; i<theRank; i++)
      if (this->owner->GetScalarProdSimpleRoot(theVector, i)<0)
      { this->owner->SimpleReflection(i, theVector);
        this->reflections.AddOnTop(i);
        break;
      }
  this->reflections.ReverseOrderElements();
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

void WeylGroup::GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output)const
{ MacroRegisterFunctionWithName("WeylGroup::GetSimpleReflectionMatrix");
  int rank=this->GetDim();
  output.MakeIdMatrix(rank);
  for (int j=0; j<rank; j++)
    output(indexSimpleRoot,j)-=
    (this->CartanSymmetric(indexSimpleRoot,j) / CartanSymmetric(indexSimpleRoot, indexSimpleRoot) )*2;
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
  this->conjugacyClasses.ReservE(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->theElements.size);
  int theRank=this->GetDim();
  Vector<Rational> theRhoImage;
  //this->rhoOrbit.GrandMasterConsistencyCheck();
  //this->theElements.GrandMasterConsistencyCheck();
  for (int i=0; i<this->theElements.size; i++)
    if (!Accounted[i])
    { // provably unnecessary
      Accounted[i] = true;
      this->conjugacyClasses.SetSize(conjugacyClasses.size+1);
      this->conjugacyClasses.LastObject()->SetSize(0);//<-needed in case the group is being recomputed
      this->conjugacyClasses[this->conjugacyClasses.size-1].AddOnTop(i);
      for (int j=0; j<this->conjugacyClasses[this->conjugacyClasses.size-1].size; j++)
        for (int k=0; k<theRank; k++)
        { theRhoImage=this->rho;
          this->SimpleReflection(k, theRhoImage);
          this->ActOn(this->conjugacyClasses[this->conjugacyClasses.size-1][j], theRhoImage);
          this->SimpleReflection(k, theRhoImage);
          int accountedIndex=this->rhoOrbit.GetIndex(theRhoImage);
          if(!Accounted[accountedIndex])
          { this->conjugacyClasses[this->conjugacyClasses.size-1].AddOnTop(accountedIndex);
            Accounted[accountedIndex] = true;
          }
        }
      this->conjugacyClasses[conjugacyClasses.size-1].QuickSortAscending();
    }
  int checkNumElts=0;
  for (int i=0; i<this->conjugacyClasses.size; i++)
    checkNumElts+=this->conjugacyClasses[i].size;
  if (this->theElements.size!=checkNumElts)
    crash << "This is a programming error: there are total of " << checkNumElts << " elements in the various conjugacy classes while the group has "
    << this->theElements.size << " elements" << crash;
  this->conjugacyClasses.QuickSortAscending();
  //std::cout << "weyl group of type " << this->theDynkinType.ToString() << " has " << this->conjugacyClasses.size << "conjugacy classes" << std::endl;
  //for(int i=0; i<conjugacyClasses.size; i++)
  //  std::cout << i << " " << conjugacyClasses[i].size << " " << conjugacyClasses[i] << std::endl;
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
    crash << "This is a programming error: group element indexed by " << i << " but the rho-orbit of the group has only " << this->rhoOrbit.size
    << " elements. The programmer either gave a bad index, or did not compute the rho orbit. " << crash;
  return CoxeterElement(this, this->DecomposeTodorsVector(rhoOrbit[i]));
}

int CoxeterGroup::GetIndexOfElement(const CoxeterElement &g) const
{ Vector<Rational> v = rho;
  for(int i=g.reflections.size-1; i>-1; i--)
    v = SimpleReflection(g.reflections[i],v);
  return rhoOrbit.GetIndex(v);
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
Matrix<coefficient> CoxeterRepresentation<coefficient>::MatrixOfElement(int g)
{ CoxeterElement gg = G->GetCoxeterElement(g);
  return elements.GetElement(gg, gens);
}

//template <typename coefficient>
//void CoxeterRepresentation<coefficient>::PrintMatrixOfElement(int g)
//{ std::cout << MatrixOfElement(g).ToString(&consoleFormat) << std::endl;
//}


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
void CoxeterRepresentation<coefficient>::MultiplyBy
(const CoxeterRepresentation<coefficient>& other, CoxeterRepresentation<coefficient>& output) const
{ //lazy programmers handling://////
  if (&output==this || &output==&other)
  { CoxeterRepresentation<coefficient> thisCopy, otherCopy;
    thisCopy=*this;
    otherCopy=other;
    thisCopy.MultiplyBy(otherCopy, output);
    return;
  }
  //////////////////////////////////
  if (this->G!=other.G)
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
  output.gens.SetSize(gens.size);
  for(int i=0; i<gens.size; i++)
    output.gens[i].AssignTensorProduct(gens[i],other.gens[i]);
  /* would that it be this simple
  if((classFunctionMatrices.size > 0) && (other.classFunctionMatrices.size > 0))
  { U.classFunctionMatrices.SetSize(G->ccCount);
    for(int i=0; i<classFunctionMatrices.size; i++)
    { if((classFunctionMatrices[i].NumCols>0) && (other.classFunctionMatrices[i].NumCols>0))
      U.classFunctionMatrices[i].AssignTensorProduct(classFunctionMatrices[i],other.classFunctionMatrices[i]);
    }
  }*/
  output.G = G;
}

template <typename coefficient>
void CoxeterRepresentation<coefficient>::ClassFunctionMatrix
(ClassFunction<coefficient>& inputCF, Matrix<coefficient>& outputMat)
{ outputMat.MakeZeroMatrix(this->gens[0].NumRows);
  if(classFunctionMatrices.size == 0)
    classFunctionMatrices.SetSize(G->ccCount);
  for(int cci=0; cci<this->G->ccCount; cci++)
  { if(inputCF[cci] == 0)
      continue;
    if(classFunctionMatrices[cci].NumCols == 0)
    { std::cout << "Generating class function matrix " << cci << " with dimension " << gens[0].NumCols << "(cc has " << G->conjugacyClasses[cci].size << ")" << std::endl;
      classFunctionMatrices[cci].MakeZeroMatrix(gens[0].NumCols);
      for(int icci=0; icci<this->G->conjugacyClasses[cci].size; icci++)
      { //Matrix<coefficient> Mi;
        //Mi.MakeIdMatrix(this->gens[0].NumCols);
        classFunctionMatrices[cci] += MatrixOfElement(G->conjugacyClasses[cci][icci]);
        //for(int gi=g.reflections.size-1; ; gi--)
        //{  if(gi < 0)
        //      break;
        //    Mi.MultiplyOnTheRight(gens[g.reflections[gi]]);
        //}
        //classFunctionMatrices[cci] += Mi;
      }
    }
    for(int i=0; i<outputMat.NumRows; i++)
      for(int j=0; j<outputMat.NumCols; j++)
        outputMat.elements[i][j]+= classFunctionMatrices[cci].elements[i][j] * inputCF[cci];
  }
//  std::cout << outputMat.ToString(&consoleFormat) << std::endl;

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
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition()
{ Matrix<coefficient> splittingOperatorMatrix;
  List<CoxeterRepresentation<coefficient> > out;
  List<Vector<Rational> > splittingMatrixKernel;
  if(GetNumberOfComponents() == 1)
  { if(G->characterTable.GetIndex(this->character) == -1)
    { std::cout << "new irrep found, have " << G->characterTable.size << std::endl;
      G->characterTable.AddOnTop(this->character);
      G->irreps.AddOnTop(*this);
    }
    out.AddOnTop(*this);
    return out;
  }
  List<Vector<coefficient> > Vb = basis;
  List<Vector<coefficient> > tempVectors;
  for(int i=0; i<G->characterTable.size; i++)
    if(this->character.IP(G->characterTable[i])!=0)
    { std::cout << "contains irrep " << i << std::endl;
      ClassFunctionMatrix(G->characterTable[i], splittingOperatorMatrix);
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);
      intersection(Vb, splittingMatrixKernel, tempVectors);
      Vb=tempVectors;
    }
  if((Vb.size < basis.size) && (Vb.size > 0))
  { std::cout << "calculating remaining subrep... ";
    CoxeterRepresentation<coefficient> V;
    V.G = G;
    V.gens = gens;
    V.basis = Vb;
    V = V.Reduced();
    std::cout << "done" << std::endl;
    std::cout << "Decomposing remaining subrep " << V.GetCharacter() << std::endl;
    return V.Decomposition();
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
  for(int cfi=G->ccCount-1; cfi>=0; cfi--)
  { ClassFunction<coefficient> cf;
    cf.G = G;
    cf.MakeZero();
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
  { CoxeterRepresentation<coefficient> outeme;
    outeme.G = G;
    outeme.gens = gens;
    outeme.basis = es[i];
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
    std::cout << A.ToString(&consoleFormat) << std::endl;
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
{ if(this->character.data.size)
    return this->character;
  this->character.G = G;
  this->character.data.SetSize(G->ccCount);
  Matrix<coefficient> M;
  for(int cci=0; cci < G->ccCount; cci++)
    this->character.data[cci] = MatrixOfElement(G->conjugacyClasses[cci][0]).GetTrace();
  return this->character;
}

template <typename coefficient>
coefficient CoxeterRepresentation<coefficient>::GetNumberOfComponents()
{  ClassFunction<coefficient> X;
   X = GetCharacter();
   return X.norm();
}

template <typename coefficient>
bool CoxeterRepresentation<coefficient>::operator>(CoxeterRepresentation<coefficient>& right)
{ return GetCharacter() > right.GetCharacter();
}

void WeylGroup::GetSignCharacter(Vector<Rational>& out)
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  out.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<this->conjugacyClasses.size; i++)
  { int yn = this->theElements[this->conjugacyClasses[i][0]].reflections.size % 2;
    if(yn == 0)
      out[i] = 1;
    else
      out[i] = -1;
  }
}



void WeylGroup::StandardRepresentation(WeylGroupRepresentation<Rational>& output)
{ MacroRegisterFunctionWithName("WeylGroup::StandardRepresentation");
  this->CheckInitializationFDrepComputation();
  output.reset(this);
//  output.theElementImages.SetSize(this->theElements.size);
  for(int i=0; i<this->GetDim(); i++)
  { this->GetSimpleReflectionMatrix(i, output.theElementImages[i+1]);
    output.theElementIsComputed[i+1]=true;
  }
  output.GetCharacter();
  if (!this->irreps.Contains(output))
    this->irreps.AddOnTop(output);
}

void WeylGroup::GetStandardRepresentationMatrix(int g, Matrix<Rational>& output) const
{ // the rank is the number of rows of the Cartan form
  output.init(this->CartanSymmetric.NumRows,this->CartanSymmetric.NumRows);
  for(int i=0; i<this->CartanSymmetric.NumRows; i++)
  { Vector<Rational> v;
    v.MakeEi(this->CartanSymmetric.NumRows,i);
    this->ActOn(g,v);
    for(int j=0; j<this->CartanSymmetric.NumRows; j++)
      output.elements[j][i] = v[j];
  }
}


CoxeterRepresentation<Rational> CoxeterGroup::StandardRepresentation()
{ List<Matrix<Rational> > gens;
  for(int i=0; i<this->nGens; i++)
  { Matrix<Rational> geni = this->SimpleReflectionMatrix(i);
    gens.AddOnTop(geni);
  }
  CoxeterRepresentation<Rational> out;
  out.G = this;
  out.gens = gens;
  out.basis.MakeEiBasis(this->nGens);
  out.GetCharacter();
  return out;
}

void CoxeterGroup::ComputeIrreducibleRepresentations()
{  if(squares.size == 0)
      ComputeSquares();
   CoxeterRepresentation<Rational> sr = this->StandardRepresentation();
   this->irreps.AddOnTop(sr);
   this->characterTable.SetSize(0);
   characterTable.AddOnTop(sr.GetCharacter());
   std::cout << sr.GetCharacter() << std::endl;
   List<CoxeterRepresentation<Rational> > newspaces;
   newspaces.AddOnTop(sr);
   List<CoxeterRepresentation<Rational> > incompletely_digested;
   while((newspaces.size > 0) || (incompletely_digested.size > 0))
   {  CoxeterRepresentation<Rational> nspace;
      if(newspaces.size > 0)
        nspace = newspaces.PopLastObject();
      else
      { nspace = incompletely_digested[0];
        incompletely_digested.RemoveIndexShiftDown(0);
      }
      CoxeterRepresentation<Rational> tspace = sr * nspace;
      tspace.character.data.size = 0;
      std::cout << "Decomposing" << tspace.GetCharacter() << std::endl;
      List<CoxeterRepresentation<Rational> > spaces = tspace.Decomposition();
//      tspace = nspace * sr;
//      tspace.character.data.size = 0;
//      std::cout << "Decomposing (right tensor)" << tspace.GetCharacter() << std::endl;
//      spaces.AddListOnTop(tspace.Decomposition());
      for(int spi = 0; spi < spaces.size; spi++)
      {  if(spaces[spi].GetNumberOfComponents() == 1)
         {  if(!characterTable.Contains(spaces[spi].GetCharacter()))
            {  characterTable.AddOnTop(spaces[spi].GetCharacter());
               irreps.AddOnTop(spaces[spi]);
               newspaces.AddOnTop(spaces[spi]);
               std::cout << "we have " << irreps.size << " irreps" << std::endl;
            }
         } else {
            incompletely_digested.AddOnTop(spaces[spi]);
         }
      }
      if(irreps.size == ccCount)
         break;
      for(int spi=0; spi<incompletely_digested.size; spi++)
      { for(int ci=0; ci<characterTable.size; ci++)
        { if(incompletely_digested[spi].GetCharacter().IP(characterTable[ci]) != 0)
          { std::cout << "incompletely digested " << incompletely_digested[spi].GetCharacter() << "will now be further decomposed" << std::endl;
            List<CoxeterRepresentation<Rational> > shards = incompletely_digested[spi].Decomposition();
            incompletely_digested.RemoveIndexShiftDown(spi);
            for(int shi=0; shi<shards.size; shi++)
            { if(shards[shi].GetNumberOfComponents() == 1)
              { if(!characterTable.Contains(shards[shi].GetCharacter()))
                { characterTable.AddOnTop(shards[shi].GetCharacter());
                  irreps.AddOnTop(spaces[spi]);
                  newspaces.AddOnTop(spaces[spi]);
                  std::cout << "we have " << irreps.size << " irreps (got from shards)" << std::endl;
                }
              }
              else {
                incompletely_digested.AddOnTop(shards[shi]);
              }
            }
            break;
          }
        }
      }
    }
    irreps.QuickSortAscending();
    characterTable.QuickSortAscending();
//  chartable.QuickSortAscending();
   for(int i=0; i<characterTable.size; i++)
   {  std::cout << characterTable[i] << '\n';
      for(int j=0; j<irreps[i].gens.size; j++)
         std::cout << irreps[i].gens[j].ToString() << '\n';
   }
   for(int i=0; i<characterTable.size; i++)
   { std::cout << characterTable[i] << '\n';
   }
   std::cout << characterTable.size << std::endl;
}

// trial division is pretty good.  the factors of 2 were cleared earlier
// by masking. dividing by 3 a few times earlier and only checking
// 1 and 5 mod 6 would require more complicated iteration that +=2
void factor_integer_l(int n, List<int> f)
{  for(int i=2; i<FloatingPoint::sqrt(n); i+=2)
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
void getunion
(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W, List<Vector<coefficient> >& output)
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
void intersection
(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W,
 List<Vector<coefficient> >& output)
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

template <typename coefficient>
std::string CoxeterRepresentation<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << "Character: " << this->character.ToString(theFormat);
  out << "<br>The simple generators (" << this->gens.size << " total):<br> ";
  std::stringstream forYourCopyConvenience;
  for (int i=0; i<this->gens.size; i++)
  { std::stringstream tempStream;
    tempStream << "s_" << i+1 << ":=MatrixRationals{}" << this->gens[i].ToString(theFormat) << "; \\\\\n";
    forYourCopyConvenience << tempStream.str();
    out << CGI::GetHtmlMathSpanPure("\\begin{array}{l}"+ tempStream.str()+"\\end{array}", 3000);
  }
  out << "<br>For your copy convenience: <br>" << forYourCopyConvenience.str();
  return out.str();
}

List<List<bool> > CoxeterGroup::GetTauSignatures()
{ ComputeIrreducibleRepresentations();
  ClassFunction<Rational> signRep;
  signRep.G = this;
  signRep.data.SetSize(ccCount);
  for(int i=0; i<ccCount; i++)
    signRep.data[i] = TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetDeterminant();


  Selection sel;
  sel.init(CartanSymmetric.NumCols);
  int numCycles=MathRoutines::TwoToTheNth(sel.MaxSize);

  List<CoxeterGroup> PSGs;
  PSGs.SetSize(numCycles-2);
  List<List<int> > ccBackMaps;
  ccBackMaps.SetSize(PSGs.size);
  CoxeterElement g;
  g.owner = this;

  for (int i=0; i<PSGs.size; i++)
  { sel.incrementSelection();
    std::cout << "\n" << sel.ToString() << "\n";
    int d = sel.CardinalitySelection;
    PSGs[i].CartanSymmetric.init(d,d);
    for(int ii=0; ii<d; ii++)
      for(int jj=0; jj<d; jj++)
        PSGs[i].CartanSymmetric.elements[ii][jj] = CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];
    std::cout << PSGs[i].CartanSymmetric.ToString();
    // ComputeInitialCharacters gets the character of the sign representation
    // as characterTable[1]
    PSGs[i].ComputeInitialCharacters();

    ccBackMaps[i].SetSize(PSGs[i].ccCount);
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
    std::cout << ccBackMaps[i] << std::endl;
  }

  List<List<bool> > tauSignatures;
  tauSignatures.SetSize(this->ccCount);
  ClassFunction<Rational> Xi; // this will be scribbled on over and over
  Xi.data.SetSize(this->ccCount); // too big, but only allocated once
  for(int i=0; i<this->ccCount; i++)
  { tauSignatures[i].SetSize(numCycles);
    tauSignatures[i][0] = true;
    for(int j=0; j<PSGs.size; j++)
    { for(int k=0; k<PSGs[j].ccCount; k++)
        Xi.data[k] = this->characterTable[i].data[ccBackMaps[j][k]];
      Rational x = PSGs[j].characterTable[1].IP(Xi);
      if(x == 0)
        tauSignatures[i][j+1] = false;
      else
        tauSignatures[i][j+1] = true;
    }
    if(signRep.IP(this->characterTable[i]) == 0)
      tauSignatures[i][numCycles-1] = false;
    else
      tauSignatures[i][numCycles-1] = true;

    std::cout << tauSignatures[i] << std::endl;
  }
  return tauSignatures;
}
