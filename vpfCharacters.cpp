#include "vpf.h"
#include "vpfCharacters.h"
#include "vpfGraph.h"

CoxeterGroup::CoxeterGroup(const DynkinType D){
    Matrix<Rational> M;
    D.GetCartanSymmetric(M);
    this->CartanSymmetric = M;
    this->nGens = M.NumCols;
    int n = M.NumCols;
    HashedList<Vector<Rational> > roots;

    Vector<Rational> v;
    for(int i=0;i<nGens;i++){
        v.MakeEi(n,i);
        roots.AddOnTopNoRepetition(this->GetOrbit(v));
    }

    this->rootSystem = roots;
    Vector<Rational> rho;
    rho.MakeZero(n);
    for(int i=0;i<roots.size;i++){
        bool usethis = true;
        for(int j=0; j<n; j++)
            if(roots[i][j] < 0){
                usethis = false;
                break;
            }
        if(usethis == true)
            rho += roots[i];
    }
    this->rho = rho/2;
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
            x += v[j] * CartanSymmetric.elements[l[i]][j];
        v[l[i]] -= x * 2 / CartanSymmetric.elements[l[i]][l[i]];
    }
    return v;
}

Vector<Rational> CoxeterGroup::ComposeTodorsVector(const List<int> &l) const{
    return CoxeterGroup::ApplyList(l,rho);
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

    Character Xe;
    Xe.G = *this;
    Xe.SetExpectedSize(ccCount);
    for(int i=0; i<ccCount; i++)
        Xe.AddOnTop(1);
    characterTable.AddOnTop(Xe);

    Character Xsgn;
    Xsgn.G = *this;
    Xsgn.SetExpectedSize(ccCount);
    for(int i=0; i<ccCount; i++)
        Xsgn.AddOnTop(TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetDeterminant().floorIfSmall());
    characterTable.AddOnTop(Xsgn);

    // does not actually belong in the character table
    Character Xstd;
    Xstd.G = *this;
    Xstd.SetExpectedSize(ccCount);
    for(int i=0; i<ccCount; i++)
        Xstd.AddOnTop(TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetTrace().floorIfSmall());
    Xstd = Xstd-Xe;
    characterTable.AddOnTop(Xstd);
}

Vector<Rational> CoxeterGroup::SimpleReflection(int i, const Vector<Rational> &v) const{
    Rational x=0;
    for(int j=0;j<this->nGens;j++)
        x += this->CartanSymmetric.elements[i][j] * v[j];
    Vector<Rational> w = v;
    w[i] -= x * 2 / CartanSymmetric.elements[i][i];
    return w;
}


//This will be incorrect if it's ever extended to a complex type
int Character::IP(const Character &other) const{
    int acc = 0;
    for(int i=0;i<G.ccCount;i++)
        acc +=  this->TheObjects[i] * other[i] * G.ccSizes[i];
    return acc/G.N;
}

int Character::norm() const {
    return this->IP(*this);
}

// The next three functions are practically identical
Character Character::operator*(const Character &other) const{
    Character l;
    l.G = G;
    l.SetExpectedSize(G.ccCount);
    for(int i=0; i<G.ccCount; i++)
        l.AddOnTop(this->TheObjects[i] * other[i]);
    return l;
}

Character Character::Sym2() const{
    Character l;
    l.G = G;
    l.SetExpectedSize(G.ccCount);
    for(int i=0; i<G.ccCount; i++){
        l.AddOnTop((this->TheObjects[i] * this->TheObjects[i] + this->TheObjects[G.squares[i]])/2);
    }
    return l;
}

Character Character::Alt2() const{
    Character l;
    l.G = G;
    l.SetExpectedSize(G.ccCount);
    for(int i=0; i<G.ccCount; i++){
        l.AddOnTop((this->TheObjects[i] * this->TheObjects[i] - this->TheObjects[G.squares[i]])/2);
    }
    return l;
}

Character Character::operator+(const Character &other) const{
    Character l;
    l.G = G;
    l.SetExpectedSize(G.ccCount);
    for(int i=0; i<G.ccCount; i++){
        l.AddOnTop(this->TheObjects[i] + other[i]);
    }
    return l;
}

Character Character::operator-(const Character &other) const{
    Character l;
    l.G = G;
    l.SetExpectedSize(G.ccCount);
    for(int i=0; i<G.ccCount; i++){
        l.AddOnTop(this->TheObjects[i] - other[i]);
    }
    return l;
}
