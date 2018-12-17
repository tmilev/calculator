#ifndef vpfWEBAPI_HEADER_H
#define vpfWEBAPI_HEADER_H
#include "vpfMacros.h"

static ProjectInformationInstance vpfWEBAPI_HEADER_Hinstance(__FILE__, "Header, web api strings. ");


class WebAPI
{
public:
  static std::string appNoCache;
  static std::string app;

  static std::string calculatorCalculatorPage;
  static std::string calculatorSetProblemData;
  static std::string calculatorChangePassword;
  static std::string calculatorActivateAccount;
  static std::string calculatorActivateAccountJSON;
  static std::string calculatorHTML;
  static std::string calculatorOnePageJS;
  static std::string calculatorOnePageJSWithHash;
  static std::string calculatorUserInfoJSON;
  class databaseParameters
  {
  public:
    static std::string entryPoint;
    static std::string labels;
    static std::string operation;
    static std::string fetch;
  };
  static std::string HeaderCacheControl;
  static std::string problemContent;
  static std::string problemFileName;
  static std::string problemId;
  static std::string problemSingleDeadline;
};

#endif
