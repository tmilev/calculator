//this small project prepares a static vector_partition.html.cpp file from the
//vector_partition.html file.
//You do not need to use this program. You can directly modify the vector_partition.html.cpp file.
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
//This is needed since to avoid reading a static file on the file system

#include <assert.h>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>

void getPathTemp(char* path, std::string& output)
{ int length=0;
  if (path==0) return;
  while (path[length]!=0)
    length++;
  output= path;
  for (int i=length-1;i>=0;i--)
#ifndef WIN32
    if (path[i]=='/')
    { output.resize(i+1);
      return;
    }
#else
    if (path[i]=='\\')
    { output.resize(i+1);
      return;
    }
#endif
}

void htmlTocpp(std::string& input, std::string& output)
{ std::stringstream out;
  out << "output << \" ";
  for (unsigned int i=0; i<input.length();i++)
  { if (input[i]=='\"')
      out << "\\\"";
    else
    { if (input[i]=='\\')
        out << "\\\\";
      else
      { out << input[i];
      }
    }
  }
  out << "\\n\";";
  output = out.str();
}

std::string thePath;

void writeStatic(const std::string& fileIn, const std::string& fileoutName, int functionIndex)
{ std::fstream fileHeaderHtml;
  std::string tempS, tempS2;
  tempS2= thePath;
  tempS2.append(fileoutName);
  std::fstream fileout;
  fileout.open(tempS2.c_str(), std::fstream::out| std::fstream::trunc);
  fileout << "#include \"vpf.h\"\n";
  fileout << "ProjectInformationInstance ProjectInfoStaticHtmlcpp(__FILE__, \"Cpp file containing html strings. The file is automatically generated from the RootSystem.html file. \");\n\n";
  tempS=thePath;
  tempS.append(fileIn);
  fileHeaderHtml.open(fileIn.c_str(), std::fstream::in|std::fstream::out);
  std::stringstream BufferIO;
  fileHeaderHtml.clear(std::ios::goodbit);
  fileHeaderHtml.seekp(0, std::ios_base::end);
  fileHeaderHtml.seekg(0);
  char buffer[10000];
  while (!fileHeaderHtml.eof())
  { fileHeaderHtml.read(buffer, 5000);
		BufferIO.write(buffer, fileHeaderHtml.gcount());
  }
  fileHeaderHtml.close();
  BufferIO << "\n<EndOfFile>";
  BufferIO.seekg(0);
  fileout << "void static_html" << functionIndex << "(std::stringstream& output){\n";
  for (;;)
  { BufferIO.getline(buffer, 5000);
    tempS=buffer;
    if (tempS=="<!>")
    { fileout << "}\n";
      functionIndex++;
      fileout << "void static_html" << functionIndex << "(std::stringstream& output){\n";
    }
    if (tempS=="<EndOfFile>")
      break;
    htmlTocpp(tempS, tempS2);
    fileout << tempS2 << "\n";
  }
  fileout << "}\n";
  fileout.flush();
 }

int main(int argc, char **argv)
{ std::string tempS2, tempS3;
  getPathTemp(argv[0], thePath);
  thePath.append("../../");
//  writeStatic("vector_partition.html", "vector_partition.html.cpp", 1);
  writeStatic("RootSystem.html", "RootSystem.html.cpp", 2);
  return 0;
}
