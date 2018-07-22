#ifndef vpfWEBAPI_HEADER_H
#define vpfWEBAPI_HEADER_H
#include "vpfMacros.h"

static ProjectInformationInstance vpfWEBAPI_HEADER_Hinstance(__FILE__, "Header, web api strings. ");


class WebAPI
{
public:
  static std::string appNoCache;
  static std::string app;

  static std::string calculatorSetProblemData;
  static std::string calculatorHTML;
  static std::string calculatorOnePageJS;
  static std::string calculatorOnePageJSWithHash;
  static std::string HeaderCacheControl;
};

#endif
