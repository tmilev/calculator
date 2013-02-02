//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader3_already_included
#define vpfHeader3_already_included

#include "vpfHeader1.h"
static ProjectInformationInstance ProjectInfoVpfHeader3
(__FILE__, "Header, ListReferences and HashedListReferences implementation. ");


//class ListReferences is to be used in the same way as class List.
//The essential difference between ListReferences and List is in the way the objects are
//stored in memory. A copy of each object of ListReferences
// is allocated with an individual copy constructor (call of new Object; rather than new Object[size];),
//and a pointer to the so allocated memory is stored.
//Motivation for this class: when a pointer/reference to an object is requested,
//the class returns a pointer to the individually allocated object.
//This piece of memory is allocated exactly once,
//i.e. the individual piece of memory never moves, and is thus completely safe
//to pass a reference of the object around. This reference will not expire,
//should the list be resized.
//
//Use guideline.
//ListReferences is slower, but pointers
//to data stored in ListReferences are safe to share.
//You should use ListReferences/HashedListReferences
//for objects that will be shared and passed accross functions.
//Those include mostly large
//objects and objects contained in the Object container of the calulator.
//Data that is not meant to be shared
//or that is otherwise small (lists of ints, monomials, Rationals, etc.)
//should be kept in simple Lists and copied around when needed.
//So long as a List is const, it is safe to pass references to data from the List.
//Whenever in doubt, use ListReferences.

template <class Object>
class ListReferences
{
public:
  List<Object*> theReferences;
  int size;
  Object& operator[](int i)const
  { if (i<0 || i>=this->size)
    { std::cout << "This is a programing error: attempting to access "
      << " element of index " << i << " in ListReferences that has only "
      << this->size << " elements. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (this->theReferences[i]==0)
    { std::cout << "This is a programing error: element of index "
      << i << " in ListReferences has zero pointer. This is not allowed. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return *this->theReferences[i];
  }
  void AllocateElements(int newSize);
  void SetSize(int newSize)
  { this->AllocateElements(newSize);
    this->size=newSize;
  }
  void SetExpectedSize(int theSize)
  { int newSize=(theSize*6)/5;
    if (newSize>0)
      this->AllocateElements(newSize);
  }
  void KillAllElements();
  void KillElementIndex(int i);
  void AddOnTop(const Object& o);
  int GetIndex(const Object& o)const;
  bool ContainsExactlyOnce(const Object& o)const
  { bool result=false;
    for (int i=0; i<this->size; i++)
      if ((*this)[i]==o)
      { if (result)
          return false;
        result=true;
      }
    return result;
  }
  int AddNoRepetitionOrReturnIndexFirst(const Object& o)
  { int indexOfObject=this->GetIndex(o);
    if (indexOfObject==-1)
    { this->AddOnTop(o);
      return this->size-1;
    }
    return indexOfObject;
  }
  ListReferences():size(0)
  {}
  ~ListReferences()
  { this->KillAllElements();
  }
};

template<class Object>
void ListReferences<Object>::AllocateElements(int newSize)
{ if (newSize<0)
  { std::cout << "This is a programming error: requested to set "
    << "negative size " << newSize << " of List of References. If a "
    << " List is to be set empty, then one should call SetSize(0), rather "
    << " than provide a negative argument to SetSize."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (newSize>this->theReferences.size)
  { int oldReferencesSize=this->theReferences.size;
    this->theReferences.SetSize(newSize);
    for (int i=oldReferencesSize; i<newSize; i++)
      this->theReferences[i]= (new Object);
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter+=newSize-oldReferencesSize;
  ParallelComputing::CheckPointerCounters();
#endif
  }
}

template<class Object>
void ListReferences<Object>::KillAllElements()
{ for (int i =0; i<this->theReferences.size; i++)
  { delete this->theReferences[i];
#ifdef CGIversionLimitRAMuse
    ParallelComputing::GlobalPointerCounter--;
    ParallelComputing::CheckPointerCounters();
#endif
    this->theReferences[i]=0;
  }
  this->theReferences.size=0;
  this->size=0;
}

template<class Object>
void ListReferences<Object>::AddOnTop(const Object& o)
{ this->SetSize(this->size+1);
  (*this)[this->size-1]=o;
}

template<class Object>
int ListReferences<Object>::GetIndex(const Object& o) const
{ for (int i=0; i<this->size; i++)
    if ((*this)[i]==o)
      return i;
  return -1;
}

template <class Object, unsigned int hashFunction(const Object&)=Object::HashFunction>
class HashedListReferences : public HashTemplate<Object, ListReferences<Object>, hashFunction>
{
};
#endif
