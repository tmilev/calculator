#include <exception>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "polyhedra.h"

int main(int argc, char **argv)
{ std::cout << "Content-Type: text/html\n\n";
  std::cout <<  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
            << std::endl;
  std::cout << "<html lang=\"en\" dir=\"LTR\">"<< std::endl;
  std::cout << "<HTML><HEAD><TITLE>Test</TITLE></HEAD><BODY><H2>Apache 2 sucks... no wait, it rocks:)</H2>\n";
  std::string tempS;
  std::cin >> tempS;
  std::cout<< tempS;
  std::cout << "</body></html>\n"
            << "<body></html>";
  return 0;   // To avoid Apache errors.
}
