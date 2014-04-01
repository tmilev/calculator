#include "vpfJson.h"

#include <sys/stat.h>
#include <cassert>

static ProjectInformationInstance ProjectInfoVpfJSON(__FILE__, "Implementation JSON.");

void JSData::operator=(const bool other)
{ this->type = JSBOOL;
  this->boolean = other;
}

void JSData::operator=(const double other)
{ this->type = JSNUM;
  this->number = other;
}

void JSData::operator=(const std::string& other)
{ this->type = JSSTR;
  this->string = other;
}

JSData& JSData::operator[](int i)
{ this->type = JSLIST;
  if(this->list.size < i+1)
    this->list.SetSize(i+1);
  return this->list[i];
}

JSData& JSData::operator[](const std::string& key)
{ this->type = JSOBJ;
  for(int i=0; i<this->obj.size; i++)
    if(this->obj[i].key == key)
      return this->obj[i].value;
  int i = this->obj.size;
  this->obj.SetSize(i+1);
  this->obj[i].key = key;
  return this->obj[i].value;
}

std::string JSData::GetString(const std::string& json, int begin, int end, int* actualend) const
{ // i don't expect this to be bigger then 2147482647 but smaller then 4294967296
  // the compiler whines about comparison between int i and json.size()
  unsigned int i;
  unsigned int minend;
  bool endlt;
  if(end>0 and ((unsigned int) end)<json.size())
  { endlt = true;
    minend = end;
  }
  else
  { endlt = false;
    minend = json.size();
  }
  for(i=begin; i<minend; i++)
    if(json[i] == '"')
    { i += 1;
      break;
    }
  if(i>=minend)
  { stOutput << "GetString: there does not appear to be a string between " << begin << " and " << minend << "\n";
    if(actualend)
      *actualend = minend;
    return json.substr(i,0);
  }
  for(; i<minend; i++)
  { if(json[i] == '"')
    { if(actualend)
        *actualend = i;
      return json.substr(begin+1,i-begin-1);
    }
  }
  if(endlt)
  { stOutput << "string being extracted failed to terminate in a timely manner, ending it at position" << i << "\n";
    if(actualend)
      *actualend = i;
    return json.substr(begin+1,i-begin);
  }
  stOutput << "json ended before string being extracted" << "\n";
  if(actualend)
    *actualend = i;
  return json.substr(begin+1,i-begin);
}

int JSData::AcceptString(const std::string& json, int begin)
{ this->type = JSSTR;
  // i don't expect this to be bigger then 2147482647 but smaller than 4294967296
  // the compiler whines about comparison between int i and json.size()
  unsigned int i = begin+1;
  for(; i<json.size(); i++)
  { if(json[i] == '"')
    { this->string = json.substr(begin+1,i-begin-1);
      return i;
    }
  }
  stOutput << "parse error: input string ended but current data string is incomplete" << "\n";
  this->string = json.substr(begin+1,json.size()-begin-1);
  return json.size();
}

void JSData::ExtractScalar(const std::string& json, int begin, int end)
{ if(begin==end)
    return;
  std::string sbs = json.substr(begin,end-begin);
  this->type = JSNUM;
  this->number = atof(sbs.c_str());
}

int JSData::AcceptList(const std::string& json, int begin)
{ this->type = JSLIST;
  unsigned int curobjbgn = begin+1;
  unsigned int i=curobjbgn;
  bool havecurobj = false;
  this->list.SetSize(0);
  //this code needs to be fixed. Appears to be too complicated.
  //check for empty list first: a dirty hack but no time atm. This functino needs a rewrite.
  for (i=curobjbgn; i<json.size(); i++)
    if (json[i]==']')
      return i;
    else if (json[i]!=' ')
      break;
  for(i=curobjbgn; i<json.size(); i++)
  { if(json[i] == ']')
    { if(!havecurobj)
      { // hack: should instead test in extractScalar for any type of reasonable
        // scalar.  don't.  no need to figure that out yet.
        if(i>curobjbgn)
        { this->list.SetSize(this->list.size+1);
          this->list[this->list.size-1].ExtractScalar(json, curobjbgn, i);
        }
      }
      return i;
    }
    if(json[i] == '[')
    { this->list.SetSize(this->list.size+1);
      i = this->list[this->list.size-1].AcceptList(json,i);
      havecurobj = true;
      continue;
    }
    if(json[i] == '"')
    { this->list.SetSize(this->list.size+1);
      i = this->list[this->list.size-1].AcceptString(json,i);
      havecurobj = true;
      continue;
    }
    if(json[i] == '{')
    { this->list.SetSize(this->list.size+1);
      i = this->list[this->list.size-1].AcceptObject(json,i);
      havecurobj = true;
      continue;
    }
    if(json[i] == ',')
    { if(!havecurobj)
      { this->list.SetSize(this->list.size+1);
        this->list[this->list.size-1].ExtractScalar(json, curobjbgn, i);
      }
      havecurobj = false;
      curobjbgn = i+1;
      continue;
    }
  }
  stOutput << "parse error: string ended but list is incomplete" << "\n";
  return i;
}

int JSData::AcceptObject(const std::string& json, int begin)
{ this->type = JSOBJ;
  unsigned int i=begin+1;
  unsigned int curobjbgn = i;
  bool isvaluetime = false;
  bool havecurobj = false;
  for(; i<json.size(); i++)
  { if(json[i] == '}')
    { if(!isvaluetime)
      { stOutput << "parse error: } in key, character " << i << " key will be ignored" << "\n";
        return i;
      }
      if(!havecurobj)
        this->obj[this->obj.size-1].value.ExtractScalar(json,curobjbgn,i-1);
      return i;
    }
    if(json[i] == ':')
    { if(isvaluetime)
      { stOutput << "parse error: : in value, second key ignored" << "\n";
        curobjbgn = i+1;
        continue;
      }
      isvaluetime = true;
      this->obj.SetSize(this->obj.size+1);
      this->obj[this->obj.size-1].key = this->GetString(json,curobjbgn,i);
      curobjbgn = i+1;
      havecurobj = false;
    }
    if(json[i] == ',')
    { if(!isvaluetime)
      { stOutput << "parse error: , in key, key ignored";
        curobjbgn = i+1;
        continue;
      }
      if(!havecurobj)
        this->obj[this->obj.size-1].value.ExtractScalar(json, curobjbgn, i-1);
      isvaluetime = false;
      curobjbgn = i+1;
    }
    if(isvaluetime)
    { if(json[i] == '[')
      { i = this->obj[this->obj.size-1].value.AcceptList(json,i);
        havecurobj = true;
        continue;
      }
      if(json[i] == '"')
      { i = this->obj[this->obj.size-1].value.AcceptString(json,i);
        havecurobj = true;
        continue;
      }
      if(json[i] == '{')
      { i = this->obj[this->obj.size-1].value.AcceptObject(json,i);
        havecurobj = true;
        continue;
      }
    }
  }
  stOutput << "parse error: string ended but object is incomplete" << "\n";
  return i;
}

void JSData::readfile(const char* filename)
{ std::ifstream ifp(filename);
  if(!ifp.is_open())
    return;
  struct stat f;
  stat(filename,&f);
  std::string json;
  json.resize(f.st_size);
  ifp.read(&json[0], json.size());
  this->readstring(json);
}

void JSData::readstring(const std::string& json)
{ for(unsigned int i=0; i<json.size(); i++)
  { if(json[i] == '"')
    { this->AcceptString(json,0);
      return;
    }
    if(json[i] == '[')
    { this->AcceptList(json,0);
      return;
    }
    if(json[i] == '{')
    { this->AcceptObject(json,0);
      return;
    }
  }
  this->ExtractScalar(json,0,json.size());
}

template <typename somestream>
somestream& JSData::IntoStream(somestream& out) const
{ switch(this->type)
  { case JSNULL:
      out << "null";
      return out;
    case JSNUM:
      out << this->number;
      return out;
    case JSBOOL:
      if(this->boolean == true)
        out << "true";
      else
        out << "false";
      return out;
    case JSSTR:
      out << '"' << this->string << '"';
      return out;
    case JSLIST:
      out << '[';
      for(int i=0; i<this->list.size; i++)
      { this->list[i].IntoStream(out);
        if(i!=this->list.size-1)
          out << ',';
      }
      out << ']';
      return out;
    case JSOBJ:
      out << '{';
      for(int i=0; i<this->obj.size; i++)
      { out << '"' << this->obj[i].key << '"';
        out << ':';
        this->obj[i].value.IntoStream(out);
        if(i!=this->obj.size-1)
          out << ',';
      }
      out << '}';
      return out;
  }
  //unreachable
  assert(false);
  return out;
}

void JSData::writefile(const char* filename) const
{ std::ofstream out;
  out.open(filename);
  this->IntoStream(out);
}

std::string JSData::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const JSData& data)
{ return data.IntoStream(out);
}

