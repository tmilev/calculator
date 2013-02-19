#ifndef vpfCharacterHeader
#define vpfCharacterHeader
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
    CoxeterGroup(){
      this->nGens=-1;
    }
    CoxeterGroup(const DynkinType& D){
      this->MakeFrom(D);
    }
    void MakeFrom(const DynkinType& D);
//    void MakeFromDynkinType(DynkinType D);
    Matrix<Rational> CartanSymmetric;
    HashedList<Vector<Rational> > rootSystem;
    Vector<Rational> rho;

    // this part needs to be revisited before we try E8
    HashedList<Vector<Rational> > rhoOrbit;
    List<List<int> > conjugacyClasses;

    int nGens;
    std::string ToString(FormatExpressions* theFormat=0)const
    { return this->CartanSymmetric.ToString(theFormat);
    }
    bool operator==(const CoxeterGroup& other)const
    { return this->CartanSymmetric==other.CartanSymmetric;
    }
    void operator=(const CoxeterGroup& other)
    { this->CartanSymmetric=other.CartanSymmetric;
      this->rootSystem=other.rootSystem;
      this->rhoOrbit=other.rhoOrbit;
      this->conjugacyClasses=other.conjugacyClasses;
      this->nGens=other.nGens;
    }
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

class CoxeterElement{
  public:
  List<int> reflections;
  CoxeterGroup* owner;
  CoxeterElement():owner(0){}
  static unsigned int HashFunction(const CoxeterElement& input)
  { std::cout << "<b>Coxeter::Hashfunctions not implemented Yet!!!!!!!";
    return 1;
  }
  std::string ToString(FormatExpressions* theFormat=0)
  { if (this->owner==0)
      return "(not initialized)";
    std::cout << "<b>CoxeterElement::toString not implemented</b>";
    std::stringstream out;
    out << "coxeter element owner: ";
    return this->owner->ToString(theFormat);
  }
  void operator*=(const CoxeterElement& other)
  { std::cout << "<b>This operation is not implemented Yet!!!!!!!";
  }
  void operator=(const CoxeterElement& other)
  { this->reflections=other.reflections;
    this->owner=other.owner;
  }
  bool operator==(const CoxeterElement& other)const
  { std::cout << "<b>CoxeterElement::operator== not implemented yet!!!!!!</b>";
    return false;
  }
};

#endif
