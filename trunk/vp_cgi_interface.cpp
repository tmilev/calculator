#include <exception>
#include <iostream>

#include "./cgicc-3.2.9/cgicc/Cgicc.h"
#include "./cgicc-3.2.9/cgicc/HTTPHTMLHeader.h"
#include "./cgicc-3.2.9/cgicc/HTMLClasses.h"

#include "./cgicc-3.2.9/cgicc/CgiDefs.h"
#include "./cgicc-3.2.9/cgicc/Cgicc.h"
#include "./cgicc-3.2.9/cgicc/HTTPHTMLHeader.h"
#include "./cgicc-3.2.9/cgicc/HTMLClasses.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{ cgicc::Cgicc formData;
  // Send HTTP header: Content-type: text/html
  // Print: <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
  std::cout << cgicc::HTTPHTMLHeader() <<cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict)<< std::endl;
  // Print: <html lang="en" dir="LTR">
  std::cout << cgicc::html().set("lang", "EN").set("dir", "LTR") << std::endl;
  // Set up the HTML document
  std::cout << "<HTML><HEAD><TITLE>Test</TITLE></HEAD><BODY><H2>Apache 2 sucks... no wait, it rocks:)</H2></BODY></HTML>"
            << std::endl;
  std::cout << cgicc::body() << cgicc::html();
  return 0;   // To avoid Apache errors.
}
