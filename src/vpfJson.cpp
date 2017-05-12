#include "vpfJson.h"

#include <sys/stat.h>
#include <cassert>

static ProjectInformationInstance ProjectInfoVpfJSON(__FILE__, "Implementation JSON.");

void JSData::operator=(const bool other)
{ this->reset();
  this->type = this->JSbool;
  this->boolean = other;
}

void JSData::operator=(const double other)
{ this->reset();
  this->type = this->JSnumber;
  this->number = other;
}

void JSData::operator=(const std::string& other)
{ this->reset();
  this->type = this->JSstring;
  this->string = other;
}

JSData& JSData::operator[](int i)
{ this->type = this->JSarray;
  if(this->list.size < i+1)
    this->list.SetSize(i+1);
  return this->list[i];
}

JSData JSData::GetValue(const std::string& key)
{ int theIndex=this->GetKeyIndex(key);
  if (theIndex!=-1)
    return this->obj[theIndex].value;
  JSData result;
  result.type=JSData::JSUndefined;
  return result;
}

bool JSData::HasKey(const std::string& key)
{ return this->GetKeyIndex(key)!=-1;
}

int JSData::GetKeyIndex(const std::string& key)
{ for(int i=0; i<this->obj.size; i++)
    if(this->obj[i].key == key)
      return i;
  return -1;
}

JSData& JSData::operator[](const std::string& key)
{ this->type = this->JSObject;
  for (int i=0; i<this->obj.size; i++)
    if (this->obj[i].key == key)
      return this->obj[i].value;
  int i = this->obj.size;
  this->obj.SetSize(i+1);
  this->obj[i].key = key;
  return this->obj[i].value;
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

bool JSData::IsValidElement()
{ return
  this->type==this->JSnull   ||
  this->type==this->JSbool   ||
  this->type==this->JSnumber ||
  this->type==this->JSstring ||
  this->type==this->JSarray  ||
  this->type==this->JSObject;
}

void JSData::TryToComputeType()
{ if (this->type!=this->JSUndefined)
    return;
  if (this->string=="")
    return;
  if (this->string=="null")
  { this->reset();
    this->type=this->JSnull;
    return;
  }
  if (this->string=="true")
  { this->reset();
    this->type=this->JSbool;
    this->boolean=true;
    return;
  }
  if (this->string=="false")
  { this->reset();
    this->type=this->JSbool;
    this->boolean=false;
    return;
  }
  if (this->string.size()>0)
    if (this->string[0]=='-' || MathRoutines::isADigit(this->string[0]))
    { this->number=  std::stod(this->string);
      this->string="";
      this->type=JSData::JSnumber;
      return;
    }
  this->type=JSData::JSstring;
}

bool JSData::Tokenize
(const std::string& input, List<JSData>& output)
{ output.SetSize(0);
  output.SetExpectedSize(input.size());
  JSData currentElt;
  bool inQuotes=false;
  for (unsigned i=0; i<input.size(); i++)
  { if (input[i]=='"')
    { if (currentElt.type==currentElt.JSstring)
      { output.AddOnTop(currentElt);
        currentElt.reset();
        inQuotes=false;
      } else
      { currentElt.TryToComputeType();
        if (currentElt.type!=currentElt.JSUndefined)
          output.AddOnTop(currentElt);
        currentElt.reset();
        currentElt.type=currentElt.JSstring;
        inQuotes=true;
      }
      continue;
    }
    if (inQuotes && currentElt.type==currentElt.JSstring)
    { currentElt.string+=input[i];
      continue;
    }
    if (input[i]==' ' || input[i]=='\r' || input[i]=='\n')
    { currentElt.TryToComputeType();
      if (currentElt.type!=currentElt.JSUndefined)
      { output.AddOnTop(currentElt);
        currentElt.reset();
      }
      continue;
    }
    if (input[i]=='{' || input[i]=='}' ||
        input[i]=='[' || input[i]==']' ||
        input[i]==':' || input[i]== ',')
    { currentElt.TryToComputeType();
      if (currentElt.type!=JSData::JSUndefined)
        output.AddOnTop(currentElt);
      currentElt.reset();
      if (input[i]=='{')
        currentElt.type=currentElt.JSopenBrace;
      if (input[i]=='[')
        currentElt.type=currentElt.JSopenBracket;
      if (input[i]=='}')
        currentElt.type=currentElt.JScloseBrace;
      if (input[i]==']')
        currentElt.type=currentElt.JScloseBracket;
      if (input[i]==':')
        currentElt.type=currentElt.JScolon;
      if (input[i]==',')
        currentElt.type=currentElt.JScomma;
      output.AddOnTop(currentElt);
      currentElt.reset();
      continue;
    }
    currentElt.string+=input[i];
  }
  return true;
}

bool JSData::readstring
(const std::string& json, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("JSData::readstring");
  List<JSData> theTokenS;
  JSData::Tokenize(json, theTokenS);
  if (theTokenS.size==0)
    return false;
  JSHashData pair;
  List<JSData> readingStack;
  JSData emptyElt;
  for (int i=0; i<JSData::numEmptyTokensAtStart; i++)
    readingStack.AddOnTop(emptyElt);
  readingStack.AddOnTop(theTokenS[0]);
  for (int i=0; ; )
  { JSData& last= readingStack[readingStack.size-1];
    JSData& secondToLast=readingStack[readingStack.size-2];
    JSData& thirdToLast=readingStack[readingStack.size-3];
    JSData& fourthToLast=readingStack[readingStack.size-4];
    //JSData& fifthToLast=theTokenS[i-4];
    if (fourthToLast.type==JSData::JSopenBrace &&
        thirdToLast.type==JSData::JSstring &&
        secondToLast.type==JSData::JScolon &&
        last.IsValidElement())
    { pair.key=thirdToLast.string;
      pair.value=last;
      fourthToLast.obj.AddOnTop(pair);
      readingStack.SetSize(readingStack.size-3);
      continue;
    }
    if (secondToLast.type==JSData::JSopenBracket && last.IsValidElement())
    { secondToLast.list.AddOnTop(last);
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.type==JSData::JSopenBracket && last.type==JSData::JScomma)
    { readingStack.RemoveLastObject();
      continue;
    }
    if ((secondToLast.type==JSData::JSopenBrace ||
         secondToLast.type==JSData::JSopenBracket)
        &&
        last.type==JSData::JScomma)
    { readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.type==JSData::JSopenBrace &&
        last.type==JSData::JScloseBrace)
    { secondToLast.type=JSData::JSObject;
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.type==JSData::JSopenBracket &&
        last.type==JSData::JScloseBracket)
    { secondToLast.type=JSData::JSarray;
      readingStack.RemoveLastObject();
      continue;
    }
    i++;
    if (i>=theTokenS.size)
      break;
    readingStack.AddOnTop(theTokenS[i]);
  }
//  stOutput << "DEBUG: " << "go to here finally. ";
  if (readingStack.size!=JSData::numEmptyTokensAtStart+1)
  { if (commentsOnFailure!=0)
    { std::stringstream calculatorInput;
      calculatorInput
      << "TestJSON(\""
      << HtmlRoutines::ConvertStringToBackslashEscapedString(json)
      << "\")";
      *commentsOnFailure << "<hr>Failed to parse your JSON. Input:\n<br>\n "
      << "<a href=\""
      << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&mainInput="
      << HtmlRoutines::ConvertStringToURLString(calculatorInput.str(),false)
      << "\">"
      << HtmlRoutines::ConvertStringToHtmlString(json, true)
      << "</a>"
      << "<br>Result:<br>\n ";
      for (int i=JSData::numEmptyTokensAtStart; i<readingStack.size; i++)
        *commentsOnFailure << i << ": " << readingStack[i].ToString(true) << "\n<br>\n";
    }
    return false;
  }
  if (JSData::numEmptyTokensAtStart<readingStack.size)
    *this=readingStack[JSData::numEmptyTokensAtStart];
  return true;
}

template <typename somestream>
somestream& JSData::IntoStream(somestream& out, int indentation, bool useHTML) const
{ std::string theIndentation="";
  for (int i=0; i<indentation; i++)
  { if (!useHTML)
      theIndentation+=" ";
    else
      theIndentation+="&nbsp;";
  }
  out << theIndentation;
  std::string newLine= useHTML ? "\n<br>\n" : "\n";
  indentation++;
  switch(this->type)
  { case JSnull:
      out << "null";
      return out;
    case JSnumber:
      out << this->number;
      return out;
    case JSbool:
      if(this->boolean == true)
        out << "true";
      else
        out << "false";
      return out;
    case JSstring:
      out << '"' << this->string << '"';
      return out;
    case JSarray:
      out << "[" << newLine;
      for(int i=0; i<this->list.size; i++)
      { this->list[i].IntoStream(out, indentation, useHTML);
        if(i!=this->list.size-1)
          out << ", ";
      }
      out << newLine << ']';
      return out;
    case JSObject:
      out << "{" << newLine;
      for(int i=0; i<this->obj.size; i++)
      { out << '"' << this->obj[i].key << '"';
        out << ':';
        this->obj[i].value.IntoStream(out, indentation, useHTML);
        if(i!=this->obj.size-1)
          out << ", ";
      }
      out << newLine << '}';
      return out;
    case JSopenBrace:
      if (useHTML)
        out << "<b>";
      out << "{";
      if (useHTML)
        out << "</b>";
      return out;
    case JScloseBrace:
      if (useHTML)
        out << "<b>";
      out << "}";
      if (useHTML)
        out << "</b>";
      return out;
    case JSopenBracket:
      if (useHTML)
        out << "<b>";
      out << "[";
      if (useHTML)
        out << "</b>";
      return out;
    case JScloseBracket:
      if (useHTML)
        out << "<b>";
      out << "]";
      if (useHTML)
        out << "</b>";
      return out;
    case JScolon:
      if (useHTML)
        out << "<b>";
      out << ":";
      if (useHTML)
        out << "</b>";
      return out;
    case JScomma:
      if (useHTML)
        out << "<b>";
      out << ",";
      if (useHTML)
        out << "</b>";
      return out;
    case JSUndefined:
      if (useHTML)
        out << "<b>";
      out << "undefined";
      if (useHTML)
        out << "</b>";
      return out;
    case JSerror:
      if (useHTML)
        out << "<b>";
      out << "error";
      if (useHTML)
        out << "</b>";
      return out;
    default:
      break;
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

void JSData::reset()
{ this->type=this->JSUndefined;
  this->boolean=false;
  this->number=0;
  this->string="";
  this->list.SetSize(0);
  this->obj.SetSize(0);
}

std::string JSData::ToString(bool useHTML) const
{ std::stringstream out;
  this->IntoStream(out, 2, useHTML);
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const JSData& data)
{ return data.IntoStream(out);
}
