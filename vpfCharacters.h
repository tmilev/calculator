#include "vpf.h"

class Character;

class FiniteGroup{
    public:
    int N;
    int ccCount;
    List<int> ccSizes;
    List<int> squares;
    List<Character> characterTable;
};

class Character: public List<int>{
    public:
    FiniteGroup G;

    int IP(const Character &other) const;
    int norm() const;
    Character operator*(const Character &other) const;
    Character Sym2() const;
    Character Alt2() const;
    Character operator+(const Character &other) const;
    Character operator-(const Character &other) const;

};

class CoxeterGroup: public FiniteGroup{
    public:
    CoxeterGroup(DynkinType D);
//    void MakeFromDynkinType(DynkinType D);
    Matrix<Rational> CartanSymmetric;
    HashedList<Vector<Rational> > rootSystem;
    Vector<Rational> rho;

    // this part needs to be revisited before we try E8
    HashedList<Vector<Rational> > rhoOrbit;
    List<List<int> > conjugacyClasses;

    int nGens;
    Vector<Rational> SimpleReflection(int i, const Vector<Rational> &right) const;
    HashedList<Vector<Rational> > GetOrbit(const Vector<Rational> &v) const;
    void ComputeRhoOrbit();
    Matrix<Rational> TodorsVectorToInvMatrix(const Vector<Rational> &v) const;
    Matrix<Rational> TodorsVectorToMatrix(const Vector<Rational> &v) const;
    // note: the list is backwards
    List<int> DecomposeTodorsVector(const Vector<Rational> &v) const;
    Vector<Rational> ComposeTodorsVector(const List<int> &l) const;
    Vector<Rational> ApplyList(const List<int> &l, const Vector<Rational> &v) const;
    Vector<Rational> SimpleConjugation(int i, const Vector<Rational> &v) const;
    void ComputeConjugacyClasses();
    void ComputeSquares();
    void ComputeInitialCharacters();
};
