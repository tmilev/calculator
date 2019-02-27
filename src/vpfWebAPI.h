#ifndef vpfWEBAPI_HEADER_H
#define vpfWEBAPI_HEADER_H
#include "vpfMacros.h"

static ProjectInformationInstance vpfWEBAPI_HEADER_Hinstance(__FILE__, "Header, web api strings. ");

class WebAPI {
public:
  static std::string appNoCache;
  static std::string app;
  struct request {
    static std::string calculatorPage;
    static std::string setProblemData;
    static std::string changePassword;
    static std::string examplesJSON;
    static std::string activateAccount;
    static std::string activateAccountJSON;
    static std::string calculatorHTML;
    static std::string onePageJS;
    static std::string onePageJSWithHash;
    static std::string userInfoJSON;
    static std::string editPage;
    static std::string selectCourseJSON;
    static std::string serverStatusJSON;
  };
  struct result {
    static std::string resultHtml;
    static std::string syntaxErrors;
    static std::string resultLabel;
    static std::string badInput;
    static std::string comments;
  };
  struct databaseParameters {
    static std::string entryPoint;
    static std::string labels;
    static std::string operation;
    static std::string fetch;
  };
  static std::string HeaderCacheControl;
  static std::string problemContent;
  static std::string commentsProblem;
  static std::string commentsServer;
  static std::string problemFileName;
  static std::string problemId;
  static std::string problemSingleDeadline;
};

#endif
