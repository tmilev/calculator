//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "webserver.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include "vpfHeader8HtmlSnippets.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");
WebServer theWebServer;

#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors

#ifdef MACRO_use_open_ssl
//installation of these headers in ubuntu:
//sudo apt-get install libssl-dev
//on opensuse:
//sudo yast -i libopenssl-devel
//Instructions: look at the examples folder in the openssl.
//openssl tutorial (couldn't make it work myself):
//http://www.ibm.com/developerworks/library/l-openssl/
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct SSLdata{
public:
  int errorCode;
  SSL* ssl;
  X509* client_cert;
  SSL_CTX* ctx;
  std::string otherCertificateIssuerName, otherCertificateSubjectName;
  SSLdata()
  { this->errorCode=-1;
    this->ssl=0;
    this->client_cert=0;
    this->ctx=0;
  }
};

SSLdata theSSLdata;
#endif
////////////////////////////////////////////
bool WebWorker::CheckConsistency()
{ auto oldOutputFn = stOutput.theOutputFunction;
  stOutput.theOutputFunction=0;
  if (this->flagDeallocated)
    crash << "Use after free of webworker." << crash;
  if (this->parent==0)
    crash << "Parent of web worker is not initialized." << crash;
  if (this->indexInParent==-1)
    crash << "Index in parent is bad. " << crash;
  if (this->connectionID==-1)
    crash << "Connection id is bad. " << crash;
  stOutput.theOutputFunction=oldOutputFn;
  return true;
}

bool WebWorker::ReceiveAll()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAll");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ReceiveAllHttpSSL();
  //theLog << logger::red << "DEBUG: Worker " << this->indexInParent+1 << " receiving all. " << logger::endL;
  return this->ReceiveAllHttp();
}

std::string WebWorker::openIndentTag(const std::string& theTag)
{ std::stringstream out;
  for (int i=0; i<this->indentationLevelHTML; i++)
    out << "  ";
  out << theTag << "\n";
  this->indentationLevelHTML++;
  return out.str();
}

std::string WebWorker::closeIndentTag(const std::string& theTag)
{ std::stringstream out;
  this->indentationLevelHTML--;
  for (int i=0; i<this->indentationLevelHTML; i++)
    out << "  ";
  out << theTag << "\n";
  return out.str();
}

bool WebWorker::IsAllowedAsRequestCookie(const std::string& input)
{ return input!="login" && input!="logout"
  && input!="changePassword"
  && input!="changePasswordPage"
  && input!="activateAccount";
}

std::string HtmlSnippets::GetJavascriptStandardCookies()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << HtmlSnippets::GetJavascriptCookieFunctions()
  << "function storeSettings()\n"
  << "{ theCalculatorForm=document.getElementById(\"mainInputID\");  \n"
  << "  //alert(theCalculatorForm.style.width);\n"
  << "  if (theCalculatorForm!=null){\n"
  << "    addCookie(\"widthCalculatorText\", theCalculatorForm.style.width, 100);  \n"
  << "    addCookie(\"heightCalculatorText\", theCalculatorForm.style.height, 100);\n"
  << "  }\n"
//  << " storeSettingsProgress();\n"
  << "}\n";
  out
  << "function storeSettingsProgress(){\n";
  if (theGlobalVariables.GetWebInput("debugFlag")!="")
    out << "  addCookie(\"debugFlag\", \"" << theGlobalVariables.GetWebInput("debugFlag") << "\", 100, false);  \n";
  if (theGlobalVariables.GetWebInput("studentSection")!="")
    out << "  addCookie(\"studentSection\", \"" << theGlobalVariables.GetWebInput("studentSection") << "\", 100, true);  \n";
  if (theGlobalVariables.GetWebInput("studentView")!="")
    out << "  addCookie(\"studentView\", \"" << theGlobalVariables.GetWebInput("studentView") << "\", 100, true);  \n";
  if (WebWorker::IsAllowedAsRequestCookie(theGlobalVariables.userCalculatorRequestType))
    out << "  addCookie(\"request\", \"" << theGlobalVariables.userCalculatorRequestType << "\", 100, false);\n";
  if ( theGlobalVariables.userCalculatorRequestType!="" &&
       theGlobalVariables.userCalculatorRequestType!="calculator")
  { if (theGlobalVariables.GetWebInput("fileName")!="")
      out << "  addCookie(\"fileName\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"), false)
      << "\", 100, false);\n";
    if (theGlobalVariables.GetWebInput("courseHome")!="")
      out << "  addCookie(\"courseHome\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false)
      << "\", 100, false);\n";
    if (theGlobalVariables.GetWebInput("topicList")!="")
      out << "  addCookie(\"topicList\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false)
      << "\", 100, false);\n";
  }
  out
  << "}\n";
  out
  << "function storeSettingsSecurity(){\n";
  if (theGlobalVariables.flagLoggedIn)
  { out << "   addCookie(\"authenticationToken\", \""
    << theGlobalVariables.userDefault.actualAuthenticationToken.value << "\", 150, true);"
    << "//150 days is a little longer than a semester\n"
    << "  addCookie(\"username\", \"" << theGlobalVariables.userDefault.username.value << "\", 150, true);\n";
  }
  out << "}\n";
  out
//  << "function getCalculatorCGIsettings(){\n"
//  << "  result =\"examStatus=\"+getCookie(\"examStatus\");\n"
//  << "  result+=\"&username=\"+getCookie(\"username\");\n"
//  << "  result+=\"&authenticationToken=\"+getCookie(\"authenticationToken\");\n"
//  << "  result+=\"&fileName=\"+getCookie(\"fileName\");\n"
//  << "  return result;\n"
//  << "}\n"
  << "function loadSettings(){\n"
  << "  storeSettingsSecurity();\n"
  << "  storeSettingsProgress();\n"
  << "  theCalculatorForm=document.getElementById(\"mainInputID\");  \n"
  << "  if (theCalculatorForm!=null){\n"
  << "    theOldWidth=getCookie(\"widthCalculatorText\");\n"
  << "    theOldHeight=getCookie(\"heightCalculatorText\");\n"
  << "    theCalculatorForm.style.width  = theOldWidth;\n"
  << "    theCalculatorForm.style.height = theOldHeight;\n"
  << "  }\n"
  << "  if (document.getElementById(\"debugFlag\")!=null)\n "
  << "    if(getCookie(\"debugFlag\")!='')\n"
  << "      document.getElementById(\"debugFlag\").value=getCookie(\"debugFlag\");\n"
  << "  if (document.getElementById(\"studentView\")!=null)\n "
  << "    if(getCookie(\"studentView\")!='')\n"
  << "      document.getElementById(\"studentView\").value=getCookie(\"studentView\");\n"
  << "  if (document.getElementById(\"studentSection\")!=null)\n "
  << "    if(getCookie(\"studentSection\")!='')\n"
  << "      document.getElementById(\"studentSection\").value=getCookie(\"studentSection\");\n"
  << "  if (document.getElementById(\"request\")!=null)\n "
  << "    if(getCookie(\"request\")!='')\n"
  << "    { var theReq=getCookie(\"request\");\n"
  << "      if(theReq=='template' || theReq=='calculator' || theReq=='exercise' || "
  << "         theReq=='scoredQuiz' || theReq=='exerciseNoLogin')\n"
  << "        document.getElementById(\"request\").value=getCookie(\"request\");\n"
  << "    }"
  << "  if (document.getElementById(\"fileName\")!=null)\n "
  << "    if(getCookie(\"fileName\")!='')\n"
  << "      document.getElementById(\"fileName\").value=getCookie(\"fileName\");\n"
  << "  if (document.getElementById(\"courseHome\")!=null)\n "
  << "    if(getCookie(\"courseHome\")!='')\n"
  << "      document.getElementById(\"courseHome\").value=getCookie(\"courseHome\");\n"
  << "  if (document.getElementById(\"authenticationToken\")!=null)\n"
  << "    if(getCookie(\"authenticationToken\")!='')\n"
  << "      document.getElementById(\"authenticationToken\").value=getCookie(\"authenticationToken\");\n ";
  out
  << "  if (document.getElementById(\"username\")!=null)\n"
  << "    if(document.getElementById(\"username\").value=='' || document.getElementById(\"username\").value==null)\n "
  << "      if(getCookie(\"username\")!='')\n"
  << "        document.getElementById(\"username\").value=getCookie(\"username\");\n ";
  out << "}\n";
  out << " </script>\n";
  return out.str();
}

std::string WebWorker::GetJavaScriptIndicatorBuiltInServer()
{ MacroRegisterFunctionWithName("WebWorker::GetJavaScriptIndicatorBuiltInServer");
  std::stringstream out;
  out << "\n<script type=\"text/javascript\" id=\"progressReportJavascript\"> \n";
  out << "var isPaused=false;\n";
  out << "var isFinished=false;\n";
  out << "var timeIncrementInTenthsOfSecond=20;//measured in tenths of a second\n";
  out << "var timeOutCounter=0;//measured in tenths of a second\n";
  out << "var currentWorkerNumber=-1;//\n";
  out << "\nfunction progressReport()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  clearTimeout(this.timeoutID);\n";
  out << "  var progReport = document.getElementById(\"idProgressReport\");	\n";
  out << "  var requestStatus = document.getElementById(\"idProgressReportRequestStatus\");	\n";
  out << "  var progReportTimer = document.getElementById(\"idProgressReportTimer\");	\n";
  out << "  if(isPaused)\n";
  out << "    return;\n";
  out << "  progReportTimer.innerHTML =\"<hr>Refreshing every \"+timeIncrementInTenthsOfSecond/10+\" second(s). Client time: ~\"+ Math.floor(timeOutCounter/10)+\" second(s)<br>\";\n";
  out << "  timeOutCounter+=timeIncrementInTenthsOfSecond;\n";
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameExecutable
  << "?request=indicator&mainInput=\"+currentWorkerNumber;\n";
  out << "  var https = new XMLHttpRequest();\n";
  //out << "alert(sURL);\n";
  out << "  https.open(\"GET\",sURL,true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    newReportString= https.responseText;\n"
  << "    if (https.responseText==\"finished\")\n"
  << "    { isFinished=true;\n"
  << "      requestStatus.innerHTML=\"<span style='color:green'><b>Computation finished.</b></span>\";\n"
  << "      return;\n"
  << "    }\n"
  << "    if (https.responseText!=\"\")\n"
  << "    { progReport.innerHTML=newReportString+\"<hr>\";\n"
  << "      requestStatus.innerHTML='';"
  << "    }else"
  << "      requestStatus.innerHTML=\"<span style='color:red'><b>Empty response</b></span>\";"
  << "  }\n"
  ////////////////////////////////////////////
//  << "  requestStatus.innerHTML = \"<span style='color:orange'><b>Request sent</b></span>\";\n"
  << "  https.send(null);\n";
//  out << "  if (oRequest.status==200)\n";
  out << "   this.timeoutID =window.setTimeout(\"progressReport()\",timeIncrementInTenthsOfSecond*100);\n";
  out << " }\n";
  out << "function SendTogglePauseRequest()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var requestStatus = document.getElementById(\"idProgressReportRequestStatus\");	\n";
  out << "  var pauseRequest = new XMLHttpRequest();\n";
  out << "  pauseURL  = \"" << theGlobalVariables.DisplayNameExecutable
  << "?request=pause&mainInput=\"+currentWorkerNumber;\n";
  out << "  pauseRequest.open(\"GET\",pauseURL,true);\n";
  out << "  pauseRequest.onload = function() {\n";
  out << "    if (pauseRequest.status!=200)\n";
  out << "      return;\n";
  out << "    requestStatus.innerHTML=pauseRequest.responseText;\n";
  out << "    if(pauseRequest.responseText==\"paused\")\n";
  out << "      isPaused=true;\n";
  out << "    if(pauseRequest.responseText==\"unpaused\")\n";
  out << "      isPaused=false;\n";
  out << "    if (isPaused)\n";
  out << "      document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Continue\";\n";
  out << "    else\n";
  out << "      document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Pause\";\n";
  out << "    document.getElementById(\"idProgressReportRequestStatus\").innerHTML=pauseRequest.responseText;\n";
  out << "    if (!isPaused)\n";
  out << "      progressReport();\n";
  out << "   }\n";
  out << "  pauseRequest.send(null);\n";
  out << "}\n";
//  out << " progressReport();";
//  out << " var newReportString=\"\";\n";
  out << " </script>\n";
  out << " \n";
  out << " \n";
  return out.str();
}

#ifdef MACRO_use_open_ssl
void SSL_write_Wrapper(SSL* inputSSL, const std::string& theString)
{ MacroRegisterFunctionWithName("SSL_write_Wrapper");
  int err =  SSL_write (inputSSL, theString.c_str(), theString.size());
  if ((err)==-1)
  { ERR_print_errors_fp(stderr);
    crash << "Errors while calling SSL_write. " << crash;
  }
}

#endif // MACRO_use_open_ssl
//http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
//openssl req -x509 -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 1001
//Alternatively:
//certificate with certificate signing request:
//openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout privateKey.key
//then get the CSR.csr file to a signing authority,
//from where you get the signedFileCertificate1 and signedFileCertificate3
const std::string fileCertificate= "certificates/cert.pem";
const std::string fileKey= "certificates/key.pem";
const std::string signedFileCertificate1= "certificates/2_calculator-algebra.org.crt";
//const std::string signedFileCertificate2= "certificates/1_Intermediate.crt";
const std::string signedFileCertificate3= "certificates/1_root_bundle.crt";
const std::string signedFileKey= "certificates/privateKey.key";

void WebServer::initSSL()
{ MacroRegisterFunctionWithName("WebServer::initSSL");
  if (!theGlobalVariables.flagSSLisAvailable)
  { theLog << logger::red << "SSL is DISABLED." << logger::endL;
    return;
  }
#ifdef MACRO_use_open_ssl
  //////////////////////////////////////////////////////////////////////////
  std::string fileCertificatePhysical, fileKeyPhysical,
  singedFileCertificate1Physical, signedFileCertificate3Physical,
  signedFileKeyPhysical;

  FileOperations::GetPhysicalFileNameFromVirtual(signedFileCertificate1, singedFileCertificate1Physical, true, true);
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileCertificate3, signedFileCertificate3Physical, true, true);
  FileOperations::GetPhysicalFileNameFromVirtual (fileCertificate, fileCertificatePhysical, true, true);
  FileOperations::GetPhysicalFileNameFromVirtual(fileKey, fileKeyPhysical, true, true);
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileKey, signedFileKeyPhysical, true, true);
  //std::cout << "\n\nproject base: " << theGlobalVariables.PhysicalPathProjectBase;
  //std::cout << "\n\nfileKey physical: " << fileKeyPhysical;
  //////////////////////////////////////////////////////////
  if (!FileOperations::FileExistsVirtual(fileCertificate, true, true) ||
      !FileOperations::FileExistsVirtual(fileKey, true, true))
  { theLog << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
    theGlobalVariables.flagSSLisAvailable=false;
    return;
  }
  theLog << logger::green << "SSL is available." << logger::endL;
  const SSL_METHOD *meth;
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  meth = SSLv23_method();
  //TLSv1_2_server_method();

  theSSLdata.ctx = SSL_CTX_new (meth);
  if (!theSSLdata.ctx)
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to create CTX object." << crash;
  }
  //////////Safari web browser: no further use of foul language necessary
/*  if (!SSL_CTX_set_cipher_list(theSSLdata.ctx,
  "ALL:!ECDHE-ECDSA-AES256-SHA:!ECDHE-ECDSA-AES128-SHA:!ECDHE-ECDSA-RC4-SHA:!ECDHE-ECDSA-DES-CBC3-SHA"
  ))
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to set cipher list." << crash;
  } else
    theLog << logger::orange << "Restricted ciphers to workaround Safari's bugs. " << logger::endL;
*/


  if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate3Physical.c_str()) <=0)
  { theLog << logger::purple << "Found no officially signed certificate, trying self-signed certificate. "
    << logger::endL;
    if (SSL_CTX_use_certificate_file(theSSLdata.ctx, fileCertificatePhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(theSSLdata.ctx, fileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(4);
    }
  } else
  { theLog << logger::green << "Found officially signed certificate... " << logger::endL;
//    if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate2.c_str()) <=0)
//    { ERR_print_errors_fp(stderr);
//      exit(3);
//    }
    if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, singedFileCertificate1Physical.c_str()) <=0)
    { ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(theSSLdata.ctx, signedFileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(4);
    }
    theGlobalVariables.flagCertificatesAreOfficiallySigned=true;
  }
  if (!SSL_CTX_check_private_key(theSSLdata.ctx))
  { fprintf(stderr,"Private key does not match the certificate public key\n");
    exit(5);
  }
////////Safari web browser: no further use of foul language necessary.
//  SSL_CTX_set_options(theSSLdata.ctx, SSL_OP_SAFARI_ECDHE_ECDSA_BUG);
////////
#endif // MACRO_use_open_ssl
}

void WebServer::SSLfreeResources()
{ if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return;
#ifdef MACRO_use_open_ssl
  theGlobalVariables.flagUsingSSLinCurrentConnection=false;
  SSL_free (theSSLdata.ssl);
  theSSLdata.ssl=0;
#endif // MACRO_use_open_ssl
}

void WebServer::SSLfreeEverythingShutdownSSL()
{ if (!theGlobalVariables.flagSSLisAvailable)
    return;
#ifdef MACRO_use_open_ssl
  theGlobalVariables.flagSSLisAvailable=false;
  SSL_CTX_free (theSSLdata.ctx);
  theSSLdata.ctx=0;
#endif // MACRO_use_open_ssl
}

void WebServer::SSLServerSideHandShake()
{ if (!theGlobalVariables.flagSSLisAvailable)
    return;
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return;
//  logOpenSSL << "Initiating SSL handshake. " << logger::endL;
#ifdef MACRO_use_open_ssl
  MacroRegisterFunctionWithName("WebServer::SSLServerSideHandShake");
//  theLog << "Got to here 1" << logger::endL;
  theSSLdata.ssl = SSL_new(theSSLdata.ctx);
//  theLog << "Got to here 1.05" << logger::endL;
  if(theSSLdata.ssl==0)
  { logOpenSSL << logger::red << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
//  theLog << "Got to here 1.1" << logger::endL;
  SSL_set_fd (theSSLdata.ssl, this->GetActiveWorker().connectedSocketID);
//  theLog << "Got to here 1.2" << logger::endL;
  int maxNumHandshakeTries=1;
//  int theError=-1;
  for (int i=0; i<maxNumHandshakeTries; i++)
  { theSSLdata.errorCode = SSL_accept (theSSLdata.ssl);
    this->flagSSLHandshakeSuccessful=false;
    if (theSSLdata.errorCode!=1)
    { if (theSSLdata.errorCode==0)
        logOpenSSL << "OpenSSL handshake not successful in a controlled manner. ";
      else
        logOpenSSL << "OpenSSL handshake not successful with a fatal error. ";
      logOpenSSL << "Attempt " << i+1 << " out of " << maxNumHandshakeTries << " failed. ";
      switch(SSL_get_error(theSSLdata.ssl, theSSLdata.errorCode))
      {
      case SSL_ERROR_NONE:
        logOpenSSL << logger::red << "No error reported, this shouldn't happen. " << logger::endL;
        maxNumHandshakeTries=1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        logOpenSSL << logger::red << "The TLS/SSL connection has been closed (possibly cleanly). " << logger::endL;
        maxNumHandshakeTries=1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        logOpenSSL << logger::red << " During regular I/O: repeat needed (not implemented). " << logger::endL;
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        logOpenSSL << logger::red << " During handshake negotiations: repeat needed (not implemented). "
        << logger::endL;
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        logOpenSSL << logger::red << " Application callback set by SSL_CTX_set_client_cert_cb(): "
        << "repeat needed (not implemented). "
        << logger::endL;
        maxNumHandshakeTries=1;
        break;
  //    case SSL_ERROR_WANT_ASYNC:
  //      logOpenSSL << logger::red << "Asynchronous engine is still processing data. "
  //      << logger::endL;
  //      break;
      case SSL_ERROR_SYSCALL:
        logOpenSSL << logger::red << "Error: some I/O error occurred. "
        << logger::endL;
        maxNumHandshakeTries=1;
        break;
      case SSL_ERROR_SSL:
        logOpenSSL << logger::red << "A failure in the SSL library occurred. "
        << logger::endL;
//        theError=ERR_get_error(3ssl);
//        if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
//          maxNumHandshakeTries=1;
        break;
      default:
        logOpenSSL << logger::red << "Unknown error. " << logger::endL;
        maxNumHandshakeTries=1;
        break;
      }
      logOpenSSL << "Retrying connection in 0.5 seconds...";
      theGlobalVariables.FallAsleep(500000);
    } else
    { this->flagSSLHandshakeSuccessful=true;
      break;
    }
  }
//  theLog << "Got to here 1.3" << logger::endL;
//    CHK_SSL(err);
//  theLog << "Got to here 2" << logger::endL;
  /* Get the cipher - opt */
  /* Get client's certificate (note: beware of dynamic allocation) - opt */
/*  theSSLdata.client_cert = SSL_get_peer_certificate (theSSLdata.ssl);
  if (theSSLdata.client_cert != NULL)
  { char* tempCharPtr=0;
    logIO << logger::purple << "SSL connection using: " << SSL_get_cipher (theSSLdata.ssl) << logger::endL;
    tempCharPtr = X509_NAME_oneline (X509_get_subject_name (theSSLdata.client_cert), 0, 0);
    if (tempCharPtr==0)
    { logIO << "X509_NAME_oneline return null; this is not supposed to happen. " << logger::endL;
      crash << "X509_NAME_oneline return null; this is not supposed to happen. " << crash;
    }
    theSSLdata.otherCertificateSubjectName=tempCharPtr;
    OPENSSL_free(tempCharPtr);
    logIO << "Client certificate:" << logger::endL
    << "Subject: " << theSSLdata.otherCertificateSubjectName << logger::endL;
    tempCharPtr = X509_NAME_oneline (X509_get_issuer_name  (theSSLdata.client_cert), 0, 0);
    if (tempCharPtr==0)
    { logIO << "X509_NAME_oneline return null; this is not supposed to happen. " << logger::endL;
      crash << "X509_NAME_oneline return null; this is not supposed to happen. " << crash;
    }
    theSSLdata.otherCertificateIssuerName=tempCharPtr;
    OPENSSL_free(tempCharPtr);
    logIO << "Issuer name: " << theSSLdata.otherCertificateIssuerName << logger::endL;
    X509_free (theSSLdata.client_cert);
  } else
    logIO << logger::purple << "SSL connection using: " << SSL_get_cipher (theSSLdata.ssl) << ". "
    << logger::normalColor << "No client certificate." << logger::endL;
    */
#endif // MACRO_use_open_ssl
}

bool WebWorker::ReceiveAllHttpSSL()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttpSSL");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return true;
#ifdef MACRO_use_open_ssl
  unsigned const int bufferSize=60000;
  char buffer[bufferSize];
//  std::cout << "Got thus far 9" << std::endl;
//  std::cout.flush();
  if (this->connectedSocketID==-1)
    crash << "Attempting to receive on a socket with ID equal to -1. " << crash;
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 30 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO,(void*)(&tv), sizeof(timeval));
  int numBytesInBuffer= SSL_read(theSSLdata.ssl, &buffer, bufferSize-1);
  double numSecondsAtStart=theGlobalVariables.GetElapsedSeconds();
  if (numBytesInBuffer<0 || numBytesInBuffer>(signed)bufferSize)
  { std::stringstream out;
    out << "Socket::ReceiveAllHttpSSL on socket " << this->connectedSocketID << " failed. ";
    ERR_print_errors_fp(stderr);
    this->displayUserInput=out.str();
    theLog << out.str() << logger::endL;
    return false;
  }
  this->messageHead.assign(buffer, numBytesInBuffer);
  this->ParseMessageHead();
  if (this->requestTypE==WebWorker::requestTypes::requestPost)
    this->displayUserInput="POST " + this->addressGetOrPost;
  else
    this->displayUserInput="GET " + this->addressGetOrPost;
  if (this->ContentLength<=0)
  { //logIO << " exiting successfully" << logger::endL;
    return true;
  }
  if (this->messageBody.size()==(unsigned) this->ContentLength)
    return true;
//  this->messageBody.clear();
//  theLog << "Content-length parsed to be: " << this->ContentLength
//  << "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  if (this->ContentLength>10000000)
  { this->CheckConsistency();
    error="Content-length parsed to be more than 10 million bytes, aborting.";
    theLog << this->error << logger::endL;
    this->displayUserInput=this->error;
    return false;
  }
  this->remainingBytesToSenD=(std::string) "HTTP/1.1 100 Continue\r\n\r\n";
  this->SendAllBytesNoHeaders();
  this->remainingBytesToSenD.SetSize(0);
  std::string bufferString;
  while ((signed) this->messageBody.size()<this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds()-numSecondsAtStart>180)
    { this->error= "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
//    logIO << logger::blue << "about to read ..." << logger::endL;
    numBytesInBuffer= SSL_read(theSSLdata.ssl, &buffer, bufferSize-1);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput=this->error;
      return false;
    }
    if (numBytesInBuffer<0)
    { if (errno==EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno==EIO || errno==ENOBUFS
          || errno==ENOMEM)
        continue;
      this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      ERR_print_errors_fp(stderr);
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->messageBody+=bufferString;
  }
  if ((signed) this->messageBody.size()!=this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length " << this->messageBody.size()
    << " yet I expected a message of length " << this->ContentLength << ". More details follow. "
    << this->ToStringMessageFullUnsafe();
    logIO << out.str() << logger::endL;
  }
  return true;
  #else
  return false;
  #endif
}

void WebWorker::SendAllBytesHttpSSL()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttpSSL");
#ifdef MACRO_use_open_ssl
  if (this->remainingBytesToSenD.size==0)
    return;
  this->CheckConsistency();
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    crash << "Error: WebWorker::SendAllBytesHttpSSL called while flagUsingSSLinCurrentConnection is set to false. " << crash;
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    //theReport.SetStatus("Socket::SendAllBytes failed: connectedSocketID=-1. WebWorker::SendAllBytes - finished.");
    return;
  }
  theLog << "SSL Worker " << this->indexInParent+1 << " sending " << this->remainingBytesToSenD.size << " bytes in chunks of: ";
  double startTime=theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending=0;
  int timeOutInSeconds =20;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO,(void*)(&tv), sizeof(timeval));
  while (this->remainingBytesToSenD.size>0)
  { if (theGlobalVariables.GetElapsedSeconds()-startTime>timeOutInSeconds)
    { theLog << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent =  SSL_write (theSSLdata.ssl, this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
    if (numBytesSent<0)
    { ERR_print_errors_fp(stderr);
      theLog << "WebWorker::SendAllBytes failed: SSL_write error. " << logger::endL;
      return;
    }
    if (numBytesSent==0)
      numTimesRunWithoutSending++;
    else
      numTimesRunWithoutSending=0;
    theLog << numBytesSent;
    this->remainingBytesToSenD.Slice(numBytesSent, this->remainingBytesToSenD.size-numBytesSent);
    if (this->remainingBytesToSenD.size>0)
      theLog << ", ";
    if (numTimesRunWithoutSending>3)
    { theLog << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
    theLog.flush();
  }
  theLog << "... done." << logger::endL;
#endif
}

bool ProgressReportWebServer::flagServerExists=true;
const int WebServer::maxNumPendingConnections=1000000;

ProgressReportWebServer::ProgressReportWebServer(const std::string& inputStatus)
{ this->flagDeallocated=false;
  this->indexProgressReport=theWebServer.theProgressReports.size;
  this->SetStatus(inputStatus);
}

ProgressReportWebServer::ProgressReportWebServer()
{ this->flagDeallocated=false;
  this->indexProgressReport=theWebServer.theProgressReports.size;
}

ProgressReportWebServer::~ProgressReportWebServer()
{ if (!this->flagServerExists)
    return;
  theWebServer.theProgressReports.SetSize(this->indexProgressReport);
  this->flagDeallocated=true;
}

void ProgressReportWebServer::SetStatus(const std::string& inputStatus)
{ MacroRegisterFunctionWithName("ProgressReportWebServer::SetStatus");
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing || !this->flagServerExists)
    return;
  if (!theGlobalVariables.flagRunningBuiltInWebServer)
    return;
  theWebServer.CheckConsistency();
//  theLog << logger::endL << logger::red << "SetStatus: outputFunction: "
//  << (int) stOutput.theOutputFunction << logger::endL;
  MutexRecursiveWrapper& safetyFirst=theGlobalVariables.MutexWebWorkerStaticFiasco;
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 2" << std::endl;
  safetyFirst.LockMe();
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 3" << std::endl;
  if (this->indexProgressReport>=theWebServer.theProgressReports.size)
    theWebServer.theProgressReports.SetSize(this->indexProgressReport+1);
  theWebServer.theProgressReports[this->indexProgressReport]=inputStatus;
//    std::cout << "SetStatus: passed mutex section" << std::endl;
  std::stringstream toBePiped;
  for (int i=0; i<theWebServer.theProgressReports.size; i++)
    if (theWebServer.theProgressReports[i]!="")
      toBePiped << "<br>" << theWebServer.theProgressReports[i];
  safetyFirst.UnlockMe();
  // theLog << logger::endL << logger::red << "SetStatus before the issue: outputFunction: "
  // << (int) stOutput.theOutputFunction << logger::endL;
  if (!theGlobalVariables.flagRunningBuiltInWebServer)
    return;
  theWebServer.GetActiveWorker().pipeWorkerToServerWorkerStatus.WriteAfterEmptying(toBePiped.str());
}

void WebServer::Signal_SIGINT_handler(int s)
{ MacroRegisterFunctionWithName("WebServer::Signal_SIGINT_handler");
  theLog << "Signal interrupt handler called with input: " << s;
//  << ". Waiting for children to exit... " << logger::endL;
  theWebServer.ReleaseActiveWorker();
  theWebServer.ReleaseNonActiveWorkers();
  while(waitpid(-1, NULL, WNOHANG | WEXITED) > 0)
  { }
  theLog << "All children have exited. " << logger::endL;
  exit(0);
}

void WebServer::Signal_SIGCHLD_handler(int s)
{ (void) s; //avoid unused parameter warning, portable.
  if (theWebServer.flagThisIsWorkerProcess)
    return;
  theLog << "Received SIGCHLD signal." << logger::endL;
  theWebServer.flagReapingChildren=true;
  theWebServer.ReapChildren();
  theWebServer.flagReapingChildren=false;
}

void WebServer::ReapChildren()
{ MacroRegisterFunctionWithName("WebServer::ReapChildren");
  int waitResult=0;
  int exitFlags= WNOHANG| WEXITED;
  do
  { waitResult= waitpid(-1, NULL, exitFlags);
//    theLog << "waitresult is: " << waitResult << logger::endL;
    if (waitResult>0)
      for (int i=0; i<this->theWorkers.size; i++)
        if (this->theWorkers[i].ProcessPID==waitResult)
        { this->theWorkers[i].pipeWorkerToServerControls.WriteNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY("close");
          this->theWorkers[i].flagInUse=false;
          this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
          theLog << logger::green << "Child with pid " << waitResult << " successfully reaped. " << logger::endL;
          this->NumProcessesReaped++;
        }
  }while (waitResult>0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string WebWorker::ToStringMessageShortUnsafe(FormatExpressions* theFormat)const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  bool useHtml=theFormat==0 ? false: theFormat->flagUseHTML;
  std::string lineBreak= useHtml ? "<br>\n" : "\r\n";
  out << lineBreak;
  if (this->requestTypE==this->requestGet)
    out << "GET ";
  else if (this->requestTypE==this->requestPost)
    out << "POST ";
  else if (this->requestTypE==this->requestChunked)
    out << "GET or POST **chunked** " ;
  else
    out << "Request type undefined.";
  out << "<hr>Address get or post:\n" << CGI::StringToHtmlString(this->addressGetOrPost, true);
  out << lineBreak << "\nAddress computed:\n" << CGI::StringToHtmlString(this->addressComputed, true);
  out << lineBreak << "\nArgument computed:\n" << CGI::StringToHtmlString(this->argumentComputed, true);
  out << lineBreak << "\nVirtual file/directory name:\n" << CGI::StringToHtmlString(this->VirtualFileName, true);
  out << lineBreak << "\nRelative physical file/directory name:\n" << CGI::StringToHtmlString(this->RelativePhysicalFileNamE, true);
  out << lineBreak << "\nExecutable url:\n" << theGlobalVariables.DisplayNameExecutable;
  out << lineBreak << "\nPhysical address project base:\n" << theGlobalVariables.PhysicalPathProjectBase;
  out << lineBreak << "\nPhysical address server base:\n" << theGlobalVariables.PhysicalPathServerBasE;
  out << lineBreak << "\nPhysical address output folder:\n" << theGlobalVariables.PhysicalPathHtmlFolder;
  return out.str();
}

std::string WebWorker::ToStringMessageFullUnsafe()const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  out << this->ToStringMessageUnsafe();
  if (this->theStrings.size>0)
  { out << "<hr>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];
  }
  return out.str();
}

std::string WebWorker::ToStringMessageUnsafe()const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  out << this->ToStringMessageShortUnsafe(&tempFormat);
  out << "<hr>";
  out << "Main address: " << CGI::StringToHtmlString(this->addressGetOrPost, true) << "<br>";
  if (this->requestTypE==this->requestGet)
    out << "GET " << CGI::StringToHtmlString(this->addressGetOrPost, true);
  if (this->requestTypE==this->requestPost)
    out << "POST " << CGI::StringToHtmlString(this->addressGetOrPost, true);
  if (this->requestTypE==this->requestChunked)
    out << "POST or GET **chunked** " << CGI::StringToHtmlString(this->addressGetOrPost, true);
  if (this->flagKeepAlive)
    out << "<br><b>Keeping alive.</b><br>";
  else
    out << "<br><b>NOT</b> keeping alive.<br>";
  out << "<br>Cookies (" << this->cookies.size << " total):";
  for (int i=0; i<this->cookies.size; i++)
    out << "<br>" << this->cookies[i];
  out << "\n<hr>\nFull message head:<br>\n" << CGI::StringToHtmlString(this->messageHead, true);
  out << "\n<hr>\nFull message body:<br>\n" << CGI::StringToHtmlString(this->messageBody, true);
  return out.str();
}

void WebWorker::resetMessageComponentsExceptRawMessage()
{ this->addressComputed="";
  this->addressGetOrPost="";
  this->argumentComputed="";
  this->RelativePhysicalFileNamE="";
  this->VirtualFileName="";
  this->displayUserInput="";
  this->theStrings.SetSize(0);
  this->requestTypE=this->requestUnknown;
  this->ContentLength=-1;
}

static std::stringstream standardOutputStreamAfterTimeout;
void WebWorker::StandardOutputAfterTimeOut(const std::string& input)
{ standardOutputStreamAfterTimeout << input;
//  theLog << logger::endL << logger::green << " Standard output: " << standardOutputStreamAfterTimeout.str()
//  << logger::endL;
}

std::string WebWorker::GetNavigationPage()
{ MacroRegisterFunctionWithName("WebWorker::GetNavigationPage");
  std::stringstream out;
  out << "<html><body>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts" << "\">Accounts</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=logout" << "\">Log out</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=database" << "\">Database</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage" << "\">Change password</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=statusPublic" << "\">Server status</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=calculator" << "\">Calculator</a>";
  out << "</body></html>";
  return out.str();
}

std::string WebWorker::GetDatabaseOneEntry()
{ MacroRegisterFunctionWithName("WebWorker::GetDatabaseOneEntry");
  std::stringstream out;
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagLoggedIn)
    out << "<b>Browsing database allowed only for logged-in admins.</b>";
  else
    out << theRoutines.ToStringOneEntry();
#else
out << "<b>Database not available. </b>";
#endif // MACRO_use_MySQL
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string WebWorker::GetDatabasePage()
{ MacroRegisterFunctionWithName("WebWorker::GetDatabasePage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << HtmlSnippets::GetJavascriptStandardCookies()
  << HtmlSnippets::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
//  out << "<problemNavigation>" << theGlobalVariables.ToStringNavigation() << "</problemNavigation>\n";
std::stringstream dbOutput;
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagLoggedIn)
    dbOutput << "Browsing database allowed only for logged-in admins.";
  else
    dbOutput << "Database: " << DatabaseStrings::theDatabaseName
    << "<br>Database user: " << DatabaseStrings::databaseUser
    <<  "<section>" << theRoutines.ToStringCurrentTableHTML() << "</section>";
#else
dbOutput << "<b>Database not available. </b>";
#endif // MACRO_use_MySQL
  out << HtmlInterpretation::GetNavigationPanelWithGenerationTime();
  out << dbOutput.str();
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

bool WebWorker::ExtractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments)
{ MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromCookies");
  MapLisT<std::string, std::string, MathRoutines::hashString> newlyFoundArgs;
  bool result=true;
  for (int i=0; i<this->cookies.size; i++)
    if (!CGI::ChopCGIStringAppend(this->cookies[i], newlyFoundArgs, argumentProcessingFailureComments))
      result=false;
  for (int i=0; i<newlyFoundArgs.size(); i++)
  { if (theGlobalVariables.webArguments.Contains(newlyFoundArgs.theKeys[i]))
      continue; //<-if a key is already given cookie entries are ignored.
    //argumentProcessingFailureComments << "Found new cookie key: " << newlyFoundArgs.theKeys[i] << "<br>";
    std::string trimmed=newlyFoundArgs.theValues[i];
    if (trimmed.size()>0)
      if (trimmed[trimmed.size()-1]==';')
        trimmed= trimmed.substr(0,trimmed.size()-1);
    //<-except the last cookie, cookies have extra semicolumn at the end, trimming.
    bool isGood=true;
    if (newlyFoundArgs.theKeys[i]=="request") //<- we are careful about
    { //reading arbitrary requests from the cookie. Those may effectively deny login
      //to a person who does not know to change the request type from the web address.
      //To prevent that we refuse to read requests from cookies except for the
      //whitelist below.
      isGood=false;
      if (trimmed=="template" ||
          trimmed=="exercise" ||
          trimmed=="database" ||
          trimmed=="accounts" ||
          trimmed=="calculator" ||
          trimmed=="scoredQuiz"
          )
        isGood=true;
    }
    if (isGood)
      theGlobalVariables.webArguments.SetKeyValue(newlyFoundArgs.theKeys[i], trimmed);
  }
  return result;
}

bool WebWorker::ExtractArgumentsFromMessage
(const std::string& input, std::stringstream& argumentProcessingFailureComments,
int recursionDepth)
{ MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromMessage");
//  stOutput << "DEBUG: here I am.";
  if (recursionDepth>1)
  { argumentProcessingFailureComments << "Error: input string encoded too many times";
    return false;
  }
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs=
  theGlobalVariables.webArguments;
  if (!CGI::ChopCGIStringAppend(input, theArgs, argumentProcessingFailureComments))
    return false;
  if (theGlobalVariables.flagRunningApache)
    if (this->addressComputed=="")
      this->addressComputed=theGlobalVariables.GetWebInput("request");
  if (theArgs.Contains("doubleURLencodedInput"))
  { std::string newInput=theGlobalVariables.GetWebInput("doubleURLencodedInput");
    theArgs.Clear();
    return this->ExtractArgumentsFromMessage
    (newInput, argumentProcessingFailureComments, recursionDepth+1);
  }
  return true;
}

bool WebWorker::Login(std::stringstream& argumentProcessingFailureComments)
{ MacroRegisterFunctionWithName("WebWorker::Login");
  //this function returns false if something unexpected during the login procedure.
  //Returning true does not necessarily mean the login information was accepted.
  //Returning false guarantees the login information was not accepted.
  theGlobalVariables.flagLoggedIn=false;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs= theGlobalVariables.webArguments;
  UserCalculatorData& theUser= theGlobalVariables.userDefault;
  theUser.username= CGI::URLStringToNormal(theGlobalVariables.GetWebInput("username"), false);
  theUser.enteredAuthenticationToken=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("authenticationToken"), false);
  if (theUser.enteredAuthenticationToken!="")
    this->flagAuthenticationTokenWasSubmitted=true;
  /////////////////////////////////////////////
  theUser.enteredPassword=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("password"), false);
  //this may need a security audit: the URLStringToNormal and GetWebInput
  //functions may leave traces of (freed) memory
  //of the old password. Could only be used
  //if the attacker has control of the server executable - which probably means we
  //already are in big trouble - so this really shouldn't be such a big deal.
  /////////////////////////////////////////////
  this->flagPasswordWasSubmitted=(theUser.enteredPassword!="");
  if (this->flagPasswordWasSubmitted)
    this->flagMustLogin=true;
  if (!this->flagPasswordWasSubmitted && !this->flagAuthenticationTokenWasSubmitted)
    return !this->flagMustLogin;
  if (this->flagPasswordWasSubmitted)
  { this->flagAuthenticationTokenWasSubmitted=false;
    theUser.enteredAuthenticationToken="";
  }
  if (theUser.username.value=="" || !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return false;
  if ((theGlobalVariables.userCalculatorRequestType=="activateAccount" || theGlobalVariables.userCalculatorRequestType=="changePassword")
      && theGlobalVariables.userDefault.enteredPassword=="")
  { theGlobalVariables.userDefault.enteredActivationToken=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("activationToken"), false);
    //argumentProcessingFailureComments << "DEBUG: entered activatino token: " << theGlobalVariables.userDefault.enteredActivationToken.value;
  }
  bool changingPass=theGlobalVariables.userCalculatorRequestType=="changePassword" ||
  theGlobalVariables.userCalculatorRequestType=="activateAccount";
  if (changingPass)
    theUser.enteredAuthenticationToken = "";
  if (theUser.enteredAuthenticationToken!="" || theUser.enteredPassword!="" || theUser.enteredActivationToken!="")
    theGlobalVariables.flagLoggedIn= DatabaseRoutinesGlobalFunctions::LoginViaDatabase
    (theUser, &argumentProcessingFailureComments);
  theUser.resetPassword();
  if (!theGlobalVariables.flagLoggedIn)
  { theGlobalVariables.userDefault.resetAuthenticationTokenAndPassword();
    argumentProcessingFailureComments << "<b>Invalid user or password. </b>";
  } else
  { //in case the user logged in with password, we need
    //to give him/her the correct authentication token
    theGlobalVariables.SetWebInpuT("authenticationToken", theUser.actualAuthenticationToken.GetDataNoQuotes());
  }
  theArgs.SetKeyValue("password", "********************************************");
  return true;
}

std::string WebWorker::GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken)
{ MacroRegisterFunctionWithName("WebWorker::GetHtmlHiddenInputs");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "";
  std::stringstream out;
  if (includeAuthenticationToken)
    out << "<input type=\"hidden\" id=\"authenticationToken\" name=\"authenticationToken\">\n";
  if (includeUserName)
    out << "<input type=\"hidden\" id=\"username\" name=\"username\">\n";
  //the values of the hidden inputs will be filled in via javascript
  if (this->flagFoundMalformedFormInput)
    out << "<b>Your input formed had malformed entries.</b>";
  out
//  << "<input type=\"hidden\" id=\"debugFlag\" name=\"debugFlag\">\n"
  << "<input type=\"hidden\" id=\"studentView\" name=\"studentView\">\n"
  << "<input type=\"hidden\" id=\"studentSection\" name=\"studentSection\">\n"
  << "<input type=\"hidden\" id=\"courseHome\" name=\"courseHome\">\n"
  << "<input type=\"hidden\" id=\"fileName\" name=\"fileName\">\n"
  ;
  return out.str();
}

void WebWorker::OutputResultAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputResultAfterTimeout");
  std::stringstream out;
//  out << theParser.ToStringOutputSpecials();
  if (standardOutputStreamAfterTimeout.str().size()!=0)
    out << standardOutputStreamAfterTimeout.str() << "<hr>";
  out << theParser.ToStringOutputAndSpecials();
  std::fstream outputTimeOutFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (outputTimeOutFile, "output/" + theGlobalVariables.RelativePhysicalNameOutpuT, false, true, false);
  outputTimeOutFile << "<html><body>" << out.str() << "</body></html>";
  outputTimeOutFile.close();
  WebWorker::OutputSendAfterTimeout(out.str());
}

void WebWorker::OutputCrashAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputCrashAfterTimeout");
  theLog << logger::red << theWebServer.ToStringActiveWorker() << ": crashing AFTER timeout!" << logger::endL;
  WebWorker::OutputSendAfterTimeout(standardOutputStreamAfterTimeout.str());
  theWebServer.SignalActiveWorkerDoneReleaseEverything();
}

void WebWorker::OutputSendAfterTimeout(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::OutputSendAfterTimeout");
  theGlobalVariables.flagTimedOutComputationIsDone=true;
  theLog << "WebWorker::StandardOutputPart2ComputationTimeout called with worker number "
  << theWebServer.GetActiveWorker().indexInParent+1 << "." << logger::endL;
  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked();
  theLog << logger::blue << "Sending result through indicator pipe." << logger::endL;

  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  logBlock << logger::blue << "Final output written to indicator, blocking until data "
  << "is received on the other end." << logger::endL;
  if(!theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut())
  {}
  else
  {//requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
    theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked();
  }
//  theLog << logger::red << "Result data is received, sending \"finished\"." << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying("finished");
  theLog << logger::red << "\"finished\" sent through indicator pipe, waiting." << logger::endL;
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut();
}

void WebWorker::ParseMessageHead()
{ MacroRegisterFunctionWithName("WebWorker::ParseMessageHead");
  this->resetMessageComponentsExceptRawMessage();
  std::string buffer;
  buffer.reserve(this->messageHead.size());
  this->theStrings.SetExpectedSize(50);
  this->cookies.SetExpectedSize(30);
  this->theStrings.size=0;
  this->cookies.size=0;
  this->connectionFlags.size=0;
  this->messageBody="";
  this->flagKeepAlive=false;
  this->ContentLength=-1;
  for (unsigned i =0; i<this->messageHead.size(); i++)
    if (this->messageHead[i]!=' ' && this->messageHead[i]!='\n' && this->messageHead[i]!='\r')
    { buffer.push_back(this->messageHead[i]);
      if (i==this->messageHead.size()-1)
        this->theStrings.AddOnTop(buffer);
    } else
    { if (buffer!="")
      { this->theStrings.AddOnTop(buffer);
        buffer="";
      }
      if (i>0)
        if (this->messageHead[i]=='\n' && this->messageHead[i-1]=='\r')
          this->theStrings.AddOnTop("\n");
    }
  for (int i=0; i<this->theStrings.size; i++)
    if (this->theStrings[i]=="GET")
    { this->requestTypE=this->requestGet;
      i++;
      if (i<this->theStrings.size)
        this->addressGetOrPost=this->theStrings[i];
    } else if (this->theStrings[i]=="POST")
    { this->requestTypE=this->requestPost;
      i++;
      //Short post messages may be attached to the message head
      //if that is the case the message does not end in \n
      if (i<this->theStrings.size)
      { this->addressGetOrPost=this->theStrings[i];
        if (*this->theStrings.LastObject()!="\n")
          this->messageBody=*this->theStrings.LastObject();
        else
          this->messageBody="";
      }
    } else if (this->theStrings[i]=="HEAD")
    { this->requestTypE=this->requestHead;
      i++;
      if (i<this->theStrings.size)
        this->addressGetOrPost=this->theStrings[i];
    } else if ( this->theStrings[i]=="transfer-coding:" ||
                this->theStrings[i]=="Transfer-coding:" ||
                this->theStrings[i]=="Transfer-Coding:" )
    { i++;
      if (i<this->theStrings.size)
        if(this->theStrings[i]=="chunked" ||
           this->theStrings[i]=="chunked;"||
           this->theStrings[i]=="Chunked"||
           this->theStrings[i]=="Chunked;"
           )
          this->requestTypE=this->requestChunked;
    } else if ((this->theStrings[i]=="Content-Length:" || this->theStrings[i]=="Content-length:" ||
                this->theStrings[i]=="content-length:")
               && i+1<this->theStrings.size)
    { if (this->theStrings[i+1].size()<10000)
      { LargeIntUnsigned theLI;
        if (theLI.AssignStringFailureAllowed(this->theStrings[i+1], true))
          if (!theLI.IsIntegerFittingInInt(&this->ContentLength))
            this->ContentLength=-1;
      }
    } else if (this->theStrings[i]=="Host:" || this->theStrings[i]=="host:")
    { i++;
      if (i<this->theStrings.size)
        this->hostWithPort=this->theStrings[i];
    } else if (this->theStrings[i]=="Cookie:" || this->theStrings[i]=="cookie:")
    { i++;
      for (; i<this->theStrings.size; i++)
      { if (this->theStrings[i]=="\n")
          break;
        this->cookies.AddOnTop(this->theStrings[i]);
      }
    } else if (this->theStrings[i]=="Connection:" || this->theStrings[i]=="connection:")
    { i++;
      for (; i<this->theStrings.size; i++)
      { if (this->theStrings[i]=="\n")
          break;
        this->connectionFlags.AddOnTop(this->theStrings[i]);
        if (this->theStrings[i]=="keep-alive" || this->theStrings[i]=="Keep-Alive" || this->theStrings[i]=="Keep-alive")
          this->flagKeepAlive=true;
      }
    }
  theGlobalVariables.hostWithPort=this->hostWithPort;
//  std::cout << "Got thus far 14" << std::endl;
}

bool WebWorker::ReceiveAllHttp()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttp");
  unsigned const int bufferSize=60000;
  char buffer[bufferSize];
  //theLog << logger::red << "DEBUG: got to here: worker" << this->indexInParent+1 << " " << logger::endL;
  if (this->connectedSocketID==-1)
    crash << "Attempting to receive on a socket with ID equal to -1. " << crash;
//  std::cout << "Got thus far 10" << std::endl;
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO,(void*)(&tv), sizeof(timeval));
  double numSecondsAtStart=theGlobalVariables.GetElapsedSeconds();
  int numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
  int numFailedReceives=0;
  bool result=true;
  while (numBytesInBuffer<0 || numBytesInBuffer>(signed)bufferSize)
  { std::stringstream out;
    out << "Socket::ReceiveAll on socket " << this->connectedSocketID << " failed. Error: "
    << this->parent->ToStringLastErrorDescription();
    numFailedReceives++;
    if (numFailedReceives>5)
    { out << ". 5+ failed receives so far, aborting. ";
      this->displayUserInput=out.str();
      this->error=out.str();
      logIO << out.str() << logger::endL;
      result=false;
      break;
    }
    logIO << out.str() << logger::endL;
    numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
  }
  this->messageHead.assign(buffer, numBytesInBuffer);
  if (numBytesInBuffer==0)
    return true;
  if (this->messageHead.size()==0)
    return false;
  this->ParseMessageHead();
  if (this->requestTypE==WebWorker::requestTypes::requestPost)
    this->displayUserInput="POST " + this->addressGetOrPost;
  else if (this->requestTypE==WebWorker::requestTypes::requestGet)
    this->displayUserInput="GET " + this->addressGetOrPost;
  else if (this->requestTypE==WebWorker::requestTypes::requestHead)
    this->displayUserInput="HEAD " + this->addressGetOrPost;
  else if (this->requestTypE==WebWorker::requestTypes::requestChunked)
    this->displayUserInput="GET or POST **chunked** " + this->addressGetOrPost;
  else
    this->displayUserInput="UNKNOWN REQUEST " + this->addressGetOrPost;
  if (this->ContentLength<=0)
    return true;
  if (this->messageBody.size()==(unsigned) this->ContentLength)
    return true;
  //this->messageBody.clear();

  if (this->ContentLength>10000000)
  { this->CheckConsistency();
    error="Content-length parsed to be more than 10 million bytes, aborting.";
    theLog << this->error << logger::endL;
    this->displayUserInput=this->error;
    return false;
  }
  //theLog << logger::red << "DEBUG: got to before continue, worker: " << this->indexInParent+1 << " " << logger::endL;
  this->remainingBytesToSenD=(std::string) "HTTP/1.1 100 Continue\r\n\r\n";
  this->SendAllBytesNoHeaders();
  this->remainingBytesToSenD.SetSize(0);
  std::string bufferString;
  //theLog << logger::red << "DEBUG: expecting: " << this->ContentLength << " bytes " << logger::endL;
  while ((signed) this->messageBody.size()<this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds()-numSecondsAtStart>180)
    { this->error= "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    //theLog << logger::red << "DEBUG: about to receive...  " << logger::endL;
    numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    //theLog << logger::red << "DEBUG: received! "  << logger::endL;

    if (numBytesInBuffer<0)
    { if (errno==EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno==EIO //|| errno==ENOBUFS
          || errno==ENOMEM)
        continue;
      this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      logIO << logger::red << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->messageBody+=bufferString;
  }
  if ((signed) this->messageBody.size()!=this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length " << this->addressGetOrPost.size()
    << " yet I expected a message of length " << this->ContentLength << ".";
    this->error=out.str();
    logIO << out.str() << logger::endL;
  }
  return result;
}

void WebWorker::SendDisplayUserInputToServer()
{ MacroRegisterFunctionWithName("WebWorker::SendDisplayUserInputToServer");
  if (this->displayUserInput.size()>3000)
    this->displayUserInput.resize(3000);
  this->pipeWorkerToServerUserInput.WriteAfterEmptying(this->displayUserInput);
//  theLog << logger::blue << "Piping " << this->displayUserInput << " to the server. " << logger::endL;
}

void WebWorker::ExtractHostInfo()
{ MacroRegisterFunctionWithName("WebWorker::ExtractHostInfo");
  unsigned int i=0;
  for (i=0; i<this->hostWithPort.size(); i++)
    if (this->hostWithPort[i]==':')
      break;
  if (i<this->hostWithPort.size())
    this->hostNoPort=this->hostWithPort.substr(0, i);
  else
    this->hostNoPort=this->hostWithPort;
  theGlobalVariables.hostWithPort=this->hostWithPort;
  theGlobalVariables.hostNoPort= this->hostNoPort;
}

void WebWorker::ExtractAddressParts()
{ MacroRegisterFunctionWithName("WebWorker::ExtractAdressParts");
  bool found=false;
  for (signed i=0; i<(signed) this->addressGetOrPost.size(); i++)
    if (this->addressGetOrPost[i]=='?')
    { this->addressComputed=this->addressGetOrPost.substr(0, i);
      this->argumentComputed=this->addressGetOrPost.substr(i+1, std::string::npos);
      found=true;
      break;
    }
  if (!found)
    this->addressComputed=this->addressGetOrPost;
  if (this->messageBody!="")
    this->argumentComputed=this->messageBody;
  if (theGlobalVariables.flagRunningApache && this->argumentComputed=="")
  { this->argumentComputed=this->addressComputed;
    this->addressComputed="";
  }
}

std::string WebWorker::GetHeaderConnectionClose()
{ std::stringstream out;
  out << "Connection: close";
  return out.str();
}

std::string WebWorker::GetHeaderSetCookie()
{ if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "";
  std::stringstream out;
  if (theGlobalVariables.userDefault.username.value!="")
  { out << "Set-Cookie: " << "username="
    << theGlobalVariables.userDefault.username.GetDataNoQuotes()
    << "; Path=/; Expires=Sat, 01 Jan 2030 20:00:00 GMT; Secure";
    if (theGlobalVariables.userDefault.actualAuthenticationToken.value!="")
      out << "\r\n";
  }
  if (theGlobalVariables.userDefault.actualAuthenticationToken.value!="")
    out << "Set-Cookie: " << "authenticationToken="
    << theGlobalVariables.userDefault.actualAuthenticationToken.GetDataNoQuotes()
    << "; Path=/; Expires=Sat, 01 Jan 2030 20:00:00 GMT; Secure";
  return out.str();
}

void WebWorker::SetHeadeR(const std::string& httpResponseNoTermination, const std::string& remainingHeaderNoTermination)
{ MacroRegisterFunctionWithName("WebWorker::SetHeader");
  if (theGlobalVariables.flagRunningApache)
  { if (remainingHeaderNoTermination!="")
      stOutput << remainingHeaderNoTermination << "\r\n";
    if (theGlobalVariables.flagLoggedIn && WebWorker::GetHeaderSetCookie()!="")
      stOutput << WebWorker::GetHeaderSetCookie() << "\r\n";
    stOutput << "\r\n";
    return;
  }
  std::stringstream out;
  out << httpResponseNoTermination << "\r\n";
  out << this->GetHeaderConnectionClose() << "\r\n";
  if (remainingHeaderNoTermination!="")
    out << remainingHeaderNoTermination << "\r\n";
  if (theGlobalVariables.flagLoggedIn && WebWorker::GetHeaderSetCookie()!="")
    out << WebWorker::GetHeaderSetCookie() << "\r\n";
  std::string finalHeader=out.str();
  this->remainingHeaderToSend.SetSize(0);
  this->remainingHeaderToSend.SetExpectedSize(finalHeader.size());
  for (unsigned i =0; i<finalHeader.size(); i++)
    this->remainingHeaderToSend.AddOnTop(finalHeader[i]);
}

void WebWorker::SetHeaderOKNoContentLength()
{ MacroRegisterFunctionWithName("WebWorker::SetHeaderOKNoContentLength");
  this->SetHeadeR("HTTP/1.0 200 OK", "Content-Type: text/html; charset=utf-8\r\nAccess-Control-Allow-Origin: *");
  this->flagDoAddContentLength=true;
}

void WebWorker::SanitizeVirtualFileName()
{ MacroRegisterFunctionWithName("WebWorker::SanitizePhysicalFileName");
  std::string resultName;
  resultName.reserve(this->VirtualFileName.size());
  bool foundslash=false;
  for (signed i=(signed) this->VirtualFileName.size()-1; i>=0; i--)
  { bool isOK=true;
    if (foundslash && this->VirtualFileName[i]=='.')
      if (i>0 && this->VirtualFileName[i-1]=='.')
      { this->flagFileNameSanitized=true;
        isOK=false;
      }
    if (isOK)
      resultName.push_back(this->VirtualFileName[i]);
    if (this->VirtualFileName[i]=='/')
      foundslash=true;
  }
  this->VirtualFileName="";
  for (int i=resultName.size()-1; i>=0;i--)
    this->VirtualFileName.push_back(resultName[i]);
}

int WebWorker::ProcessCalculatorExamples()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorExamples");
  this->SetHeaderOKNoContentLength();
  stOutput << theParser.ToStringFunctionHandlers();
  return 0;
}

int WebWorker::ProcessServerStatusPublic()
{ MacroRegisterFunctionWithName("WebWorker::ProcessServerStatusPublic");
  this->SetHeaderOKNoContentLength();
  stOutput << theWebServer.ToStringStatusPublic();
  return 0;
}

int WebWorker::ProcessServerStatus()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestServerStatus");
  this->SetHeaderOKNoContentLength();
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << "</head>"
  << "<body>";
  std::stringstream pageBody;
  pageBody << "<section>";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    pageBody << " <table><tr><td style=\"vertical-align:top\">"
    << this->parent->ToStringStatusAll() << "</td><td>"
    << theGlobalVariables.ToStringHTMLTopCommandLinuxSystem()
    << "</td></tr></table>";
  else
    pageBody << "<b>Viewing server status available only to logged-in admins.</b>";
  pageBody << "</section>";
  if (theGlobalVariables.flagLoggedIn)
    out << HtmlInterpretation::GetNavigationPanelWithGenerationTime();
  out << pageBody.str();
  out << "</body></html>";
  stOutput << out.str();
  return 0;
}

int WebWorker::ProcessPauseWorker()
{ MacroRegisterFunctionWithName("WebWorker::ProcessPauseWorker");
  this->SetHeaderOKNoContentLength();
  std::string theMainInput=theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput=="")
  { stOutput << "<b>To pause a worker please provide the worker number in the mainInput field.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(theMainInput.c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>User requested worker number " << inputWebWorkerNumber
    << " out of " << this->parent->theWorkers.size << " which is out of range. </b>";
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. Total number of workers: "
    << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  if (inputWebWorkerIndex==this->indexInParent)
  { stOutput << "Worker " << inputWebWorkerIndex
    << " attempts to pause itself, this is not allowed.";
    return 0;
  }
  theLog << "Proceeding to toggle worker pause." << logger::endL;
  WebWorker& otherWorker=this->parent->theWorkers[inputWebWorkerIndex];
//  if (theGlobalVariables.flagLogInterProcessCommunication)
//  theLog << "About to read pipeServerToWorker..." << logger::endL;
  if (!otherWorker.PauseWorker.CheckPauseIsRequested())
  { otherWorker.PauseWorker.RequestPausePauseIfLocked();
    stOutput << "paused";
    return 0;
  }
  stOutput << "unpaused";
  otherWorker.PauseWorker.ResumePausedProcessesIfAny();
  return 0;
}

int WebWorker::ProcessMonitor()
{ MacroRegisterFunctionWithName("WebWorker::ProcessMonitor");
  this->SetHeaderOKNoContentLength();
  theLog << "Processing get monitor." << logger::endL;
  std::string theMainInput=theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput=="")
  { stOutput << "<b>Monitor takes as argument the number of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(theMainInput.c_str());
  stOutput << "<html><body>"
  << this->GetJavaScriptIndicatorBuiltInServer()
  << "<script language=\"javascript\">\n"
  << "currentWorkerNumber=" << inputWebWorkerNumber << ";\n"
  << "progressReport();\n"
  <<" </script>"
  << "</body></html>";
  return 0;
}

int WebWorker::ProcessComputationIndicator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicator");
  this->SetHeaderOKNoContentLength();
  theLog << "Processing get request indicator." << logger::endL;
  std::string theMainInput=theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput=="")
  { stOutput << "<b>To get a computation indicator you need to supply the number "
    << "of the child process in the mainInput field.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(theMainInput.c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>Indicator error. Worker number "
    << inputWebWorkerNumber << " is out of range: there are "
    << this->parent->theWorkers.size
    << " workers. </b>";
    return 0;
  }
  std::stringstream theErrorMessageStream;
  theErrorMessageStream
  << "<span style=\"color:red\">Your computation may have terminated unexpectedly. The maximum number of "
  << " connections/computations you can run is: "
  << theWebServer.MaxNumWorkersPerIPAdress << ". </span>"
  << "At the time of writing, you use up 1 connection per computation, "
  << "1 - 4 connections for each web browser request (depending on the browser) "
  << "and 1 per computation window request. The "
  << "most recent error message reported by the "
  << "worker you want to monitor is: "
  << this->parent->theWorkers[inputWebWorkerIndex].pingMessage;
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Indicator error. Worker number "
    << inputWebWorkerNumber << " is not in use. "
    << theErrorMessageStream.str()
    << " Total number of workers: "
    << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  if (inputWebWorkerIndex==this->indexInParent)
  { stOutput << "<b>Indicator error. Worker number "
    << inputWebWorkerNumber << " requested to monitor itself. "
    << " This is not allowed. " << theErrorMessageStream.str() << "</b>";
    return 0;
  }
  WebWorker& otherWorker=this->parent->theWorkers[inputWebWorkerIndex];
  if (otherWorker.userAddress.theObject!=this->userAddress.theObject)
  { stOutput << "User address: " << this->userAddress.theObject
    << " does not coincide with the monitored address, this is not allowed. ";
    return 0;
  }
  if (otherWorker.flagUsingSSLInWorkerProcess!=this->flagUsingSSLInWorkerProcess)
  { stOutput << "Monitoring https connections over http, "
    << "http connection over https is not allowed. ";
    return 0;
  }
//  theLog << "Worker " << this->parent->activeWorker
//  << consoleYellow(" piping 'indicator'" ) << logger::endL;
  otherWorker.pipeServerToWorkerRequestIndicator.WriteAfterEmptying("!");
//  theLog << "'indicator' piped, waiting for return." << logger::endL;
  otherWorker.pipeWorkerToServerIndicatorData.Read();
//  theLog << "indicator returned." << logger::endL;
  if (otherWorker.pipeWorkerToServerIndicatorData.lastRead.size>0)
  { std::string outputString;
    outputString.assign(otherWorker.pipeWorkerToServerIndicatorData.lastRead.TheObjects, otherWorker.pipeWorkerToServerIndicatorData.lastRead.size);
    //theLog << logger::yellow << "Indicator string read: " << logger::blue << outputString << logger::endL;
    stOutput << outputString;
    otherWorker.PauseComputationReportReceived.ResumePausedProcessesIfAny();
  }
//  stOutput << "<b>Not implemented: request for indicator for worker " << inputWebWorkerNumber
//  << " out of " << this->parent->theWorkers.size << ".</b>";
  return 0;
}

void WebWorker::WriteProgressReportToFile(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::WriteProgressReportToFile");
  theLog << logger::green << "Progress report written to file: " << theGlobalVariables.RelativePhysicalNameProgressReport << logger::endL;
  std::fstream theFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/" + theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false))
    FileOperations::OpenFileCreateIfNotPresentVirtual
    (theFile, "output/progressReport_failed_to_create_file.html",
     false, true, false);
  theFile << standardOutputStreamAfterTimeout.str() << "<hr>" << input;
  theFile.flush();
  theFile.close();
}

void WebWorker::PipeProgressReportToParentProcess(const std::string& input)
{ if (!this->flagProgressReportAllowed)
    return;
  MacroRegisterFunctionWithName("WebWorker::PipeProgressReportToParentProcess");
  static int counter=0;
  counter++;
//  std::stringstream debugStream1, debugStream2;
  //debugStream1 << "PipeProgressReportToParentProcess called " << counter << " times. Calling pause...";
  //ProgressReportWebServer theReport(debugStream1.str());
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
//    theLog << "about to potentially block " << logger::endL;
  //debugStream2 << "PipeProgressReportToParentProcess called "
  //<< counter << " times. Pause passed...";
  //std::cout << debugStream2.str();
  //theReport.SetStatus(debugStream2.str());
  if (this->PauseWorker.CheckPauseIsRequested())
  { std::cout << "DEBUG: PipeProgressReportToParentProcess: pausing as requested...";
    this->WriteProgressReportToFile(input);
  }
  this->PauseWorker.PauseIfRequested();     //if pause was requested, here we block
//    theLog << "(possible) block passed" << logger::endL;
//  theReport.SetStatus("PipeProgressReportToParentProcess: computing...");
  this->pipeServerToWorkerRequestIndicator.Read();
  if (this->pipeServerToWorkerRequestIndicator.lastRead.size==0)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
    return;
  }
  if (theGlobalVariables.flagTimedOutComputationIsDone)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
    return;
  }
//  if (theGlobalVariables.flagLogInterProcessCommunication)
//  theLog << " data written!";
  //theReport.SetStatus("PipeProgressReportToParentProcess: piping computation process...");
  this->pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  //theReport.SetStatus("PipeProgressReportToParentProcess: exiting 1...");
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
  //theReport.SetStatus("PipeProgressReportToParentProcess: exiting 2...");
}

int WebWorker::ProcessFolder()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  this->SetHeaderOKNoContentLength();
  std::stringstream out;
  out << "<html><body>";
//  out << this->ToString();
  if (this->RelativePhysicalFileNamE.size()>0)
    if (this->RelativePhysicalFileNamE[this->RelativePhysicalFileNamE.size()-1]!='/')
      this->RelativePhysicalFileNamE.push_back('/');
  if (this->addressGetOrPost.size()>0)
    if (this->addressGetOrPost[this->addressGetOrPost.size()-1]!='/')
      this->addressGetOrPost.push_back('/');
  if (this->flagFileNameSanitized)
  { out << "<hr>The virtual file name I extracted was: " << this->VirtualFileName
    << "<br>However, I do not allow folder names that contain dots. "
    << " Therefore I have sanitized the main address to: " << this->RelativePhysicalFileNamE;
  }
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNamesUnsecure(this->RelativePhysicalFileNamE, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address "
    << this->RelativePhysicalFileNamE << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->addressGetOrPost << "<br>Virtual name: "
  << this->VirtualFileName << "<hr>";
  List<std::string> theFolderNamesHtml, theFileNamesHtml;
  for (int i=0; i<theFileNames.size; i++)
  { std::stringstream currentStream;
    bool isDir= (theFileTypes[i]==".d");
//    theLog << logger::red << "Current file name: " << CGI::StringToURLString(theFileNames[i]) << logger::endL;
    currentStream << "<a href=\"" << this->addressGetOrPost << CGI::StringToURLString(theFileNames[i], false);
    if (isDir)
      currentStream << "/";
//    else
//      theLog <<
    currentStream << "\">" << theFileNames[i];
    if (isDir)
      currentStream << "/";
    currentStream << "</a><br>";
    if (isDir)
      theFolderNamesHtml.AddOnTop(currentStream.str());
    else
      theFileNamesHtml.AddOnTop(currentStream.str());
  }
  theFolderNamesHtml.QuickSortAscending();
  theFileNamesHtml.QuickSortAscending();
  for (int i =0; i< theFolderNamesHtml.size; i++)
    out << theFolderNamesHtml[i];
  for (int i =0; i< theFileNamesHtml.size; i++)
    out << theFileNamesHtml[i];
  //out << "DEBUG: " << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  //out << this->ToStringMessageFullUnsafe();
  out << "</body></html>";
  stOutput << out.str();
  return 0;
}

int WebWorker::ProcessFile()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFile");
  if (!FileOperations::FileExistsUnsecure(this->RelativePhysicalFileNamE))
  { //std::cout << "ere be i: file name not found" << std::endl;
    this->SetHeadeR("HTTP/1.0 404 Object not found", "Content-Type: text/html");
//    if (stOutput.theOutputFunction==0)
    stOutput << "<html><body>";
    stOutput << HtmlInterpretation::GetNavigationPanelWithGenerationTime();
    stOutput << "<b>File does not exist.</b>";
    if (this->flagFileNameSanitized)
    { stOutput << "<hr>You requested virtual file: " << this->VirtualFileName
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the address to a relative physical address: " << this->RelativePhysicalFileNamE;
    }
    stOutput << "<br><b> Address:</b> "
    << CGI::StringToHtmlString(this->addressGetOrPost, true) << "<br><b>Virtual file name:</b> "
    << CGI::StringToHtmlString(this->VirtualFileName, true)
    << "<br><b>Computed relative physical file name:</b> "
    << CGI::StringToHtmlString(this->RelativePhysicalFileNamE, true)
    ;
    stOutput << "<hr><hr><hr>Message details:<br>"
    << this->ToStringMessageUnsafe()
    << this->ToStringMessageFullUnsafe()
    ;
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=
  FileOperations::GetFileExtensionWithDot(this->RelativePhysicalFileNamE);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, this->RelativePhysicalFileNamE, false, false, !isBinary))
  { this->SetHeaderOKNoContentLength();
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> ";
    if (this->flagFileNameSanitized)
    { stOutput << "<hr>You requested virtual file: " << this->VirtualFileName
      << "<br>However, I do not allow addresses that contain dots. "
      << " Therefore I have sanitized the address to a relative physical address: " << this->RelativePhysicalFileNamE;
    }
    stOutput << "<br><b> File display name: </b>"
    << this->addressGetOrPost << "<br><b>Virtual file name: </b>"
    << this->VirtualFileName << "</body></html>";
    return 0;
  }  
  theFile.seekp(0, std::ifstream::end);
  unsigned int fileSize=theFile.tellp();
  if (fileSize>50000000)
  { this->SetHeadeR( "HTTP/1.0 413 Payload Too Large", "");
    stOutput << "<html><body><b>Error: user requested file: "
    << this->VirtualFileName
    << " but it is too large, namely, " << fileSize
    << " bytes.</b></body></html>";
    return 0;
  }
  std::stringstream theHeader;
  bool withCacheHeader=false;
  theHeader << "HTTP/1.0 200 OK\r\n"
  << this->GetMIMEtypeFromFileExtension(fileExtension)
  << "Access-Control-Allow-Origin: *\r\n";
  for (int i=0; i<this->parent->addressStartsSentWithCacheMaxAge.size; i++)
    if (MathRoutines::StringBeginsWith(this->VirtualFileName, this->parent->addressStartsSentWithCacheMaxAge[i]))
    { theHeader << "Cache-Control: max-age=12960000\r\n";
      withCacheHeader=true;
      break;
    }
  std::stringstream debugBytesStream;
  if (theGlobalVariables.UserDebugFlagOn() && fileExtension==".html")
  { debugBytesStream << "<!-- DEBUG info: ";
    if (withCacheHeader)
      debugBytesStream << "Sent with Cache-Control header.\n ";
    debugBytesStream << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
    << "-->";
  }
  if (theGlobalVariables.UserDebugFlagOn() && fileExtension==".appcache")
  { debugBytesStream << "#" << this->GetMIMEtypeFromFileExtension(fileExtension);
  }
  if (theGlobalVariables.flagRunningApache)
  { this->SetHeaderOKNoContentLength();
    stOutput << "<html><body>"
    << "<a href=\"" << this->VirtualFileName << "\">" << this->VirtualFileName << "</a>"
    << "</body></html>";
    stOutput << debugBytesStream.str();
    return 0;
  }
  unsigned int totalLength=fileSize+debugBytesStream.str().size();
  theHeader << "Content-length: " << totalLength << "\r\n";
  theHeader << this->GetHeaderConnectionClose() << "\r\n";
  std::string theCookie=this->GetHeaderSetCookie();
  if (theCookie!="")
    theHeader << theCookie << "\r\n";
  theHeader << "\r\n";
  this->QueueStringForSendingNoHeadeR(theHeader.str());
  if (this->requestTypE==this->requestHead)
  { this->SendAllBytesNoHeaders();
    return 0;

  }
  const int bufferSize=64*1024;
  this->bufferFileIO.SetSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  int numBytesRead=theFile.gcount();
  ///////////////////
//  theLog << "*****Message summary begin\r\n" << theHeader.str();
//  theLog << "Sending file  " << this->RelativePhysicalFileName; << " with file extension " << fileExtension
//  << ", file size: " << fileSize;
//  theLog << "\r\n*****Message summary end\r\n";
  ///////////////////

  while (numBytesRead!=0)
  { this->bufferFileIO.SetSize(numBytesRead);
    this->QueueBytesForSendingNoHeadeR(this->bufferFileIO);
    this->bufferFileIO.SetSize(bufferSize);
    theFile.read(this->bufferFileIO.TheObjects, this->bufferFileIO.size);
    numBytesRead=theFile.gcount();
  }
  stOutput << debugBytesStream.str();
  this->SendAllBytesNoHeaders();
  return 0;
}

void WebWorker::reset()
{ this->connectedSocketID=-1;
  this->ProcessPID=-1;
  this->connectedSocketIDLastValueBeforeRelease=-1;
  this->connectionID=-1;
  this->indexInParent=-1;
  this->parent=0;
  this->indentationLevelHTML=0;
  this->displayUserInput="";
  this->requestTypE=this->requestUnknown;
  this->flagDoAddContentLength=false;
  this->flagFileNameSanitized=false;
  this->timeOfLastPingServerSideOnly=-1;
  this->timeServerAtWorkerStart=-1;
  this->flagAuthenticationTokenWasSubmitted=false;
  this->flagFoundMalformedFormInput=false;
  this->flagPasswordWasSubmitted=false;
  this->flagProgressReportAllowed=false;
  this->flagKeepAlive=false;
  this->flagMustLogin=true;
  this->flagDidSendAll=false;
  this->flagUsingSSLInWorkerProcess=false;
  theGlobalVariables.flagUsingSSLinCurrentConnection=false;
  theGlobalVariables.flagLoggedIn=false;
  theGlobalVariables.userDefault.reset();
  this->RelativePhysicalFileNamE="";
  this->Release();
}

WebWorker::WebWorker()
{ this->flagDeallocated=false;
  this->reset();
}

bool WebWorker::IamActive()
{ if (this->parent==0 || this->indexInParent==-1)
    return false;
  return this->parent->activeWorker==this->indexInParent;
}

bool WebWorker::IsFileExtensionOfBinaryFile(const std::string& fileExtension)
{ if (fileExtension==".png")
    return true;
  if (fileExtension==".mp4")
    return true;
  if (fileExtension==".mpeg")
    return true;
  if (fileExtension==".jpg")
    return true;

  return false;
}

void WebWorker::WrapUpConnection()
{ MacroRegisterFunctionWithName("WebWorker::WrapUpConnection");
  this->pipeWorkerToServerControls.WriteAfterEmptying("close");
  this->Release();
  theGlobalVariables.flagComputationCompletE=true;
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing=true;
}

void WebWorker::SendAllAndWrapUp()
{ MacroRegisterFunctionWithName("WebWorker::SendAllAndWrapUp");
  if (!this->IamActive())
  { theLog << logger::red << "Signal done called on non-active worker" << logger::endL;
    return;
  }
  this->SendAllBytesWithHeaders();
  this->WrapUpConnection();
}

WebWorker::~WebWorker()
{ //Workers are not allowed to release resources in the destructor:
  //a Worker's destructor is called when expanding List<WebWorker>.
  this->flagDeallocated=true;
}

std::string WebWorker::GetMIMEtypeFromFileExtension(const std::string& fileExtension)
{ MacroRegisterFunctionWithName("WebWorker::GetMIMEtypeFromFileExtension");
  if (fileExtension==".html")
    return "Content-Type: text/html\r\n";
  if (fileExtension==".txt")
    return "Content-Type: text/plain\r\n";
  if (fileExtension==".png")
    return "Content-Type: image/png\r\n";
  if (fileExtension==".js")
    return "Content-Type: text/javascript\r\n";
  if (fileExtension==".ico")
    return "Content-Type: image/x-icon\r\n";
  if (fileExtension==".css")
    return "Content-Type: text/css\r\n";
  if (fileExtension==".eot")
    return "Content-Type: application/vnd.ms-fontobject\r\n";
  if (fileExtension==".ttf")
    return "Content-Type: application/octet-stream\r\n";
  if (fileExtension==".svg")
    return "Content-Type: image/svg+xml\r\n";
  if (fileExtension==".appcache")
    return "Content-Type: text/cache-manifest\r\n";
  if (fileExtension==".woff")
    return "Content-Type: application/font-woff\r\n";
  return "Content-Type: application/octet-stream\r\n";
}

int WebWorker::ProcessUnknown()
{ MacroRegisterFunctionWithName("WebWorker::ProcessUnknown");
  this->SetHeadeR("HTTP/1.0 501 Method Not Implemented", "Content-Type: text/html");
  stOutput << "<b>Requested method is not implemented. </b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessageUnsafe();

  logHttpErrors << logger::red << "Method not implemented. Message follows. " << logger::endL
  << this->ToStringMessageFullUnsafe() << logger::endL;
  return 0;
}

bool WebWorker::ShouldDisplayLoginPage()
{ if (theGlobalVariables.userCalculatorRequestType=="login")
    return true;
  if (theGlobalVariables.flagUsingSSLinCurrentConnection && !theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.userCalculatorRequestType!="calculator")
    return true;
  if (theGlobalVariables.UserRequestMustBePromptedToLogInIfNotLoggedIn() && !theGlobalVariables.flagLoggedIn)
    return true;
  return false;
}

std::string WebWorker::GetJavaScriptIndicatorFromHD()
{ std::stringstream out;
  out << " <!>\n";
  out << " <script type=\"text/javascript\"> \n";
  out << " var timeOutCounter=0;\n";
//  out << " var newReportString=\"\";\n";
  out << " var showProgress=false;";
  out << " function progressReport()\n";
  out << "{ var el = document.getElementById(\"idProgressReport\");	\n";
  out << "  if (!showProgress) \n";
  out << "  { el.style.display = 'none';\n";
  out << "    return;";
  out << "  }\n";
  out << "  el.style.display = '';\n";
  out << "  timeOutCounter++;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameExecutable << "\";\n";
  out << "  oRequest.open(\"GET\",sURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  oRequest.send(null)\n";
  out << "  if (oRequest.status==200)\n";
  out << "  { newReportString= oRequest.responseText;\n";
  out << "    el.innerHTML= \"<hr>Refreshing each second. Client time: ~\"+ timeOutCounter+\" second(s)<br>\" +newReportString+\"<hr>\";\n";
  out << "  }\n";
  out << "   window.setTimeout(\"progressReport()\",1000);\n";
  out << " }\n";
  out << " </script>\n";
  out << CGI::GetHtmlButton
  ("progressReportButton", "showProgress=!showProgress; progressReport()", "expand/collapse progress report");
  out << "<br><div "
  //<< "src=\"" << this->indicatorFileNameDisplaY << "\" "
  << "id=\"idProgressReport\" style=\"display:none\">\n";
  out << " </div>\n";
  out << " \n";
  out << " \n";
  return out.str();
}

std::string WebWorker::GetInsecureConnectionAngryMessage()
{ MacroRegisterFunctionWithName("WebWorker::GetInsecureConnectionAngryMessage");
  std::stringstream out;
  out
  << "<b>This is a reduced security connection.</b><br>"
  << "For secure connection: close safari & open in another browser: "
  << "<a href=\"https://calculator-algebra.org/calculator\">https://calculator-algebra.org/calculator</a>."
  ;
  return out.str();
}

std::string WebWorker::GetAuthenticationToken(const std::string& reasonForNoAuthentication)
{ MacroRegisterFunctionWithName("WebWorker::GetAuthenticationToken");
//  std::stringstream out;
//  out << "DEBUG: username: " << theGlobalVariables.userDefault
//  << "\npassword: " << theGlobalVariables.GetWebInput("password");
  if (theGlobalVariables.flagLoggedIn && theGlobalVariables.flagUsingSSLinCurrentConnection)
    return theGlobalVariables.userDefault.actualActivationToken.value;
  return reasonForNoAuthentication;
}

std::string WebWorker::GetJavascriptSubmitLoginInfo()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitLoginInfo(){\n"
  << "  spanOutput = document.getElementById('loginResult');\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id loginResult MISSING! </span>\";\n"
  << "  }\n"
  << "  theRequest='getAuthenticationToken?';\n"
  << "  theRequest+='username=' + encodeURIComponent(document.getElementById('username').value)+'&';\n"
  << "  theRequest+='password='+ encodeURIComponent(document.getElementById('password').value);\n"
  << "  var https = new XMLHttpRequest();\n"
////////////////////////////////////////////
  << "  https.open(\"GET\", theRequest, true);\n"
/////////////////or/////////////////////////
//  << "  https.open(\"GET\", \"" << theGlobalVariables.DisplayNameExecutableWithPath << "\""
//  << "+ \"?\"+theRequest"
//  << ", true);\n"
////////////////////////////////////////////
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    spanOutput.textContent=https.responseText;\n"
  << "    addCookie(\"authenticationToken\", https.responseText, 150, true);\n"
  << "  }\n"
////////////////////////////////////////////
  << "  https.send(theRequest);\n"
/////////////////or/////////////////////////
//  << "  https.send();\n"
////////////////////////////////////////////
  << "}\n"
  << "</script>";
  return out.str();
}

std::string WebWorker::GetLoginHTMLinternal(const std::string& reasonForLogin)
{ MacroRegisterFunctionWithName("WebWorker::GetLoginHTMLinternal");
  std::stringstream out;
  out << reasonForLogin;
  out << "<form name=\"login\">";
  out <<  "User name:\n"
  << "<input type=\"text\" id=\"username\" name=\"username\" placeholder=\"username\" ";
  if (theGlobalVariables.GetWebInput("username")!="")
    out << "value=\"" << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("username"), false) << "\"";
  out << "required>";
  out << "</input>\n";
  out << "<br>Password: ";
  out << "<input type=\"password\" id=\"password\" name=\"password\" placeholder=\"password\" autocomplete=\"on\">";
  out << this->GetHtmlHiddenInputs(false, false);
  out << "<input type=\"hidden\" name=\"request\" id=\"request\"";
  if (theGlobalVariables.userCalculatorRequestType!="logout" &&
      theGlobalVariables.userCalculatorRequestType!="login")
  { out << " value=\"" << theGlobalVariables.userCalculatorRequestType << "\"";
  }
  out << ">";
  out << "<button type=\"submit\" value=\"Submit\" ";
  out << "action=\"" << theWebServer.GetActiveWorker().addressComputed;
//  if (theWebServer.GetActiveWorker().argumentComputed!="")
//    out << "?" << theWebServer.GetActiveWorker().argumentComputed;
  out << "\"";
  out << ">Login</button>";
//  if (theGlobalVariables.flagRunningApache)
//    out << "action=\"" << theGlobalVariables.DisplayNameCalculatorApacheQ
//    << theGlobalVariables.userCalculatorRequestType << "\"";
  out << "</form>";
//  out << "<button onclick=\"submitLoginInfo();\">Login</button>";
  out << "<span id=\"loginResult\"></span>";
//  out << "<br><br><small>No account yet? We are sorry but automatic registration has not been implemented yet.<br>"
//  << " If you are our students please contact us by email and we'll register you.<br>Everyone else, please che.</small>";
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  //out << "<hr><hr>DEBUG: " << this->ToStringMessageFullUnsafe();
  return out.str();
}

std::string WebWorker::GetChangePasswordPage()
{ MacroRegisterFunctionWithName("WebWorker::GetChangePasswordPage");
  std::stringstream out;
  out << "<html>";
  out << "<head>";
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitChangePassRequest(){\n"
  << "  spanVerification = document.getElementById(\"passwordChangeResult\");\n"
  << "  inputUser= document.getElementById(\"username\");\n";
  if (theGlobalVariables.userCalculatorRequestType=="activateAccount")
    out << "  inputActivationToken= document.getElementById(\"activationToken\");\n";
  else
    out << "  inputPassword= document.getElementById(\"password\");\n";
  out
  << "  inputNewPassword= document.getElementById(\"newPassword\");\n"
  << "  inputReenteredPassword= document.getElementById(\"reenteredPassword\");\n"
  << "  params=\"request=changePassword\";\n"
  << "  params+=\"&username=\" + encodeURIComponent(inputUser.value) \n";
  if (theGlobalVariables.userCalculatorRequestType=="activateAccount")
    out << "          + \"&activationToken=\"+encodeURIComponent(inputActivationToken.value)\n";
  else
    out  << "          + \"&password=\"+encodeURIComponent(inputPassword.value)\n";
  out << "          + \"&newPassword=\"+encodeURIComponent(inputNewPassword.value)\n"
  << "          + \"&reenteredPassword=\"+encodeURIComponent(inputReenteredPassword.value)\n"
  << "  ;\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameExecutable << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  ;
  out
  << "  https.onload = function() {\n"
  << "    spanVerification.innerHTML=https.responseText;\n"
  << "  }\n"
  << "  https.send(params);\n"
  << "}\n"
  << "</script>";
  out << "</head>";
  out << "<body> ";
//    << "  window.location='calculator?username='+GlobalUser+'&authenticationToken='+GlobalAuthenticationToken;";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
//  out << "<form name=\"login\" id=\"login\" action=\"calculator\" method=\"GET\" accept-charset=\"utf-8\">";
  out
  <<  "User: " << theGlobalVariables.userDefault.username.value
  << "<input type=\"hidden\" id=\"username\" placeholder=\"username\" "
  << "value=\"" << theGlobalVariables.userDefault.username.value << "\" "
  << "required>";

  if (theGlobalVariables.userCalculatorRequestType=="activateAccount")
    out << "<input type=\"hidden\" id=\"activationToken\" value=\""
    << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("activationToken"), false) << "\">";
  else
    out << "<br>Password: "
    << "<input type=\"password\" id=\"password\" placeholder=\"password\">\n";
  out << "<br>New password: \n"
  << "<input type=\"password\" id=\"newPassword\" placeholder=\"password\">\n"
  << "<br>Re-enter new password: \n"
  << "<input type=\"password\"  id=\"reenteredPassword\" placeholder=\"password\" "
  << "onkeyup=\"if (event.keyCode == 13) submitChangePassRequest();\">\n"
  << "<button  onclick=\"submitChangePassRequest() \"> Submit</button>\n"
  << "<span id=\"passwordChangeResult\"> </span>\n"
//  << "</form>"
  ;
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

int WebWorker::ProcessChangePassword()
{ MacroRegisterFunctionWithName("WebWorker::ProcessChangePassword");
  //stOutput << " ere i am";
//  if (theGlobalVariables.UserDebugFlagOn())
//  theGlobalVariables.SetWebInpuT("debugFlag", "true");
//  stOutput << "DEBUG: " << this->ToStringCalculatorArgumentsHumanReadable();
  this->SetHeaderOKNoContentLength();
  UserCalculatorData& theUser=theGlobalVariables.userDefault;
  theUser.enteredAuthenticationToken="";
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.flagUsingSSLinCurrentConnection)
  { stOutput << "<span style=\"color:red\"><b> Password change available only to logged in users via SSL.</b></span>";
    return 0;
  }
  std::string newPassword=theGlobalVariables.GetWebInput("newPassword");
  std::string reenteredPassword=theGlobalVariables.GetWebInput("reenteredPassword");
  if (newPassword!=reenteredPassword)
  { stOutput << "<span style=\"color:red\"><b> Passwords don't match.</b></span>";
    return 0;
  }
  std::stringstream commentsOnFailure;
  std::string newAuthenticationToken;
  if (!DatabaseRoutinesGlobalFunctions::SetPassword
      (theUser.username.value, newPassword, newAuthenticationToken, commentsOnFailure))
  { stOutput << "<span style=\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
    return 0;
  }
  if (!DatabaseRoutinesGlobalFunctions::SetEntry
      (theUser.username.value, DatabaseStrings::usersTableName, "activationToken", "activated", commentsOnFailure))
    stOutput << "<span style=\"color:red\"><b>Failed to set activationToken: "
    << commentsOnFailure.str() << "</b></span>";

  stOutput << "<span style=\"color:green\"> <b>Password change successful. </b></span>";
  stOutput
  << "<meta http-equiv=\"refresh\" content=\"0; url='"
  << theGlobalVariables.DisplayNameExecutable  << "?request=logout"
  << "&username="
  << theGlobalVariables.userDefault.username.GetDataNoQuotes() << "&activationToken=&authenticationToken=&"
  << "'\" />"
  ;
  return 0;
}

int WebWorker::ProcessCompute()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCompute");
  this->SetHeaderOKNoContentLength();
//  theParser.initComputationStats();
  theParser.inputString=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone;
  theGlobalVariables.initOutputReportAndCrashFileNames
  (CGI::StringToURLString(theParser.inputString, false), theParser.inputString);

  //std::cout << "DEBUG: input string: " << theParser.inputString;
  //std::cout.flush();
  ////////////////////////////////////////////////
  //  the initialization below moved to the start of the web server!
  //  theParser.init();
  ////////////////////////////////////////////////
  this->flagProgressReportAllowed=true;
  theParser.Evaluate(theParser.inputString);
  this->flagProgressReportAllowed=false;
  if (theGlobalVariables.flagRunningBuiltInWebServer)
    if (theGlobalVariables.flagOutputTimedOut)
    { this->OutputResultAfterTimeout();
      return 0;
    }
  stOutput << theParser.ToStringOutputAndSpecials();
  if (theGlobalVariables.UserDebugFlagOn())
    stOutput << this->ToStringMessageFullUnsafe();

  theGlobalVariables.flagComputationCompletE=true;
  return 0;
}

int WebWorker::ProcessCalculator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculator");
  this->SetHeaderOKNoContentLength();
  //std::cout << "DEBUG: got to here";
  theParser.inputString=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"),false);
  theParser.flagShowCalculatorExamples=(theGlobalVariables.GetWebInput("showExamples")=="true");
  stOutput << "<html><head> <title>calculator version  "
  << __DATE__ << ", " << __TIME__ << "</title>";
  stOutput << CGI::GetJavascriptMathjax();
  stOutput << CGI::GetJavascriptInjectCalculatorResponseInNode();
  stOutput << CGI::GetCalculatorStyleSheetWithTags();
  stOutput << HtmlSnippets::GetJavascriptHideHtml();
  stOutput << HtmlSnippets::GetJavascriptStandardCookies();
  stOutput << HtmlSnippets::GetJavascriptSubmitMainInputIncludeCurrentFile();
  stOutput << HtmlSnippets::GetJavascriptCanvasGraphics();
  stOutput << CGI::GetJavascriptAutocompleteWithTags();
  stOutput << CGI::GetJavascriptInitializeButtons();
  stOutput << CGI::GetJavascriptMathQuillMatrixSupport();
  stOutput << CGI::GetMathQuillStyleSheetWithTags();

  stOutput << this->GetJavaScriptIndicatorBuiltInServer();
  stOutput << CGI::GetJavascriptCalculatorPage();

  stOutput << "\n\n<script language=\"javascript\">\n"
  << "  var theAutocompleteDictionary = [\n  ";
  for (int i=0; i<theParser.autoCompleteKeyWords.size; i++)
    if (theParser.autoCompleteKeyWords[i].size()>2)
    { stOutput << "\"" << theParser.autoCompleteKeyWords[i] << "\"";
      if (i!=theParser.autoCompleteKeyWords.size-1)
        stOutput << ", ";
    }
  stOutput << "];\n";
  stOutput << "</script>\n";

  stOutput << "\n</head>\n<body onload=\"loadSettings(); initializeButtons(); initializeCalculatorPage();";
  if (theParser.inputString!="")
  { stOutput << "submitStringAsMainInput"
    << "(document.getElementById('mainInputID').value, "
    << " 'calculatorOutput', 'compute', onLoadDefaultFunction, 'mainComputationStatus');";
  }
  stOutput << "\">\n";
  stOutput << "<problemNavigation>" << theGlobalVariables.ToStringNavigation()
  << "</problemNavigation>\n";


  stOutput << this->openIndentTag("<table><!-- Outermost table, 3 cells (3 columns 1 row)-->");
  stOutput << this->openIndentTag("<tr style=\"vertical-align:top\">");
  stOutput << this->openIndentTag("<td><!-- Cell containing the output table-->");
  stOutput << this->openIndentTag("<table><!-- Output table, 2 cells (1 column 2-3 rows) -->");

  int startingIndent=this->indentationLevelHTML;
  stOutput << this->openIndentTag("<tr style=\"vertical-align:top\">");
  stOutput << this->openIndentTag("<td><!-- cell with input, autocomplete space and output-->");
  stOutput << this->openIndentTag("<table><!--table with input, autocomplete space and output-->");
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td style=\"vertical-align:top\"><!-- input form here -->");
  std::string civilizedInputSafish;
  if (CGI::StringToHtmlStringReturnTrueIfModified(theParser.inputString, civilizedInputSafish, false))
    stOutput << "Your input has been treated normally, however the return string "
    << "of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. <br>";
  stOutput << this->GetHtmlHiddenInputs(true, true);
  stOutput << "<input type=\"hidden\" name=\"request\" id=\"request\" value=\"compute\">\n";
  stOutput << "<textarea rows=\"3\" cols=\"30\" name=\"mainInput\" id=\"mainInputID\" "
  << "style=\"white-space:normal\" "
  << "onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {"
  << "submitStringAsMainInput(document.getElementById('mainInputID').value,"
  << " 'calculatorOutput', 'compute', onLoadDefaultFunction, 'mainComputationStatus');"
  << " event.preventDefault();"
  << "}\" "
  << "onkeyup=  \"suggestWord(); mQHelpCalculator();\", "
  << "onkeydown=\"suggestWord(); mQHelpCalculator(); arrowAction(event); \", "
  << "onmouseup=\"suggestWord(); mQHelpCalculator();\", "
  << "oninput=  \"suggestWord(); mQHelpCalculator();\""
  << ">";
  stOutput << civilizedInputSafish;
  stOutput << "</textarea>\n";
  stOutput << "<br>";
  stOutput << "<button title=\"Shift+Enter=shortcut from input text box. \" "
  << "name=\"Go\" onclick=\""
  << "submitStringAsMainInput(document.getElementById('mainInputID').value, 'calculatorOutput', 'compute', onLoadDefaultFunction, 'mainComputationStatus'); event.preventDefault();"
  << "\"> ";
  stOutput << "Go" << "</button>"
  << "<span id=\"mainComputationStatus\"></span>";
  stOutput << this->closeIndentTag("</td>");

  stOutput << this->openIndentTag("<td style=\"vertical-align:top\">");
  stOutput << "<table style=\"vertical-align:top\"><tr><td style=\"vertical-align:top\">";
  stOutput << "<span id=\"mqProblemSpan\" style=\"white-space:nowrap\">Equation assistant</span>";
  stOutput << "</td></tr><tr><td>";
  stOutput << "<span style=\"vertical-align:top\" id=\"mainInputMQfield\"></span>";
  stOutput << "</td></tr></table>";
  stOutput << this->closeIndentTag("</td>");


  stOutput << this->openIndentTag("<td style=\"vertical-align:top\"><!--Autocomplete space here -->");
  stOutput << "<span style=\"white-space:nowrap\" id=\"idAutocompleteHints\">"
  << "Ctrl+space = autocomplete<br>ctrl+arrow = select</span>";
  stOutput << "<br>";
  stOutput << "<div id=\"idAutocompleteSpan\" "
  << "style=\"vertical-align:top; display:block; overflow:auto;\" "
  << " height=\"100px\" "
  << "></div>";
  stOutput << "<span id=\"idAutocompleteDebug\"></span>";
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table>");
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  if (startingIndent!=this->indentationLevelHTML)
    crash << "Non-balanced html tags. " << crash;
  if (theGlobalVariables.UserDebugFlagOn())
    stOutput << "<tr><td>"
    << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
    << "</td></tr>";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  theGlobalVariables.flagComputationCompletE=true;
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td>");
  stOutput << "<span id=\"calculatorOutput\"></span>";
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table><!--table with input, autocomplete space and output-->");
  stOutput << this->closeIndentTag("</td>");

//  bool displayClientMessage=theWebServer.flagUsingBuiltInServer && theWebServer.activeWorker!=-1;
  //displayClientMessage=false;
  stOutput << "<td valign=\"top\">";
  theGlobalVariables.theSourceCodeFiles().QuickSortAscending();
  stOutput << theGlobalVariables.ToStringSourceCodeInfo();
  stOutput << "<hr>";
  stOutput << theParser.ToString();

  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table><!-- outermost table-->");

  stOutput << "</body></html>";
  stOutput << "<!--";
  ProgressReport theReport;
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { std::stringstream out;
    out << "\n\ncommand: " << theParser.SystemCommands[i] << "\n" ;
    theReport.Report(out.str());
    theGlobalVariables.CallSystemNoOutput(theParser.SystemCommands[i]);
  }
  stOutput << "-->";
  return 0;
}

int WebWorker::ProcessChangePasswordPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessChangePasswordPage");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetChangePasswordPage();
  return 0;
}

int WebWorker::ProcessLogout()
{ MacroRegisterFunctionWithName("WebWorker::ProcessLogout");
  this->SetHeaderOKNoContentLength();
  DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
  theGlobalVariables.userDefault.resetAuthenticationTokenAndPassword();
  stOutput << this->GetLoginPage();
  return 0;
}

int WebWorker::ProcessSelectCourse()
{ MacroRegisterFunctionWithName("WebWorker::ProcessTemplate");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetSelectCourse();
  return 0;
}

int WebWorker::ProcessTemplate()
{ MacroRegisterFunctionWithName("WebWorker::ProcessTemplate");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetPageFromTemplate();
  //if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  //  stOutput << "<!--" << this->ToStringMessageFullUnsafe() << "-->";
  return 0;
}

int WebWorker::ProcessTopicTable()
{ MacroRegisterFunctionWithName("WebWorker::ProcessTopicTable");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetTopicTable();
  return 0;
}

int WebWorker::ProcessExamPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessExamPage");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetExamPage();
  return 0;
}

int WebWorker::ProcessExamPageInterpreter()
{ stOutput << this->GetExamPageInterpreter();
  return 0;
}

int WebWorker::ProcessNavigation()
{ MacroRegisterFunctionWithName("WebWorker::ProcessNavigation");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetNavigationPage();
  return 0;
}

int WebWorker::ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication)
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetAuthenticationToken");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetAuthenticationToken(reasonForNoAuthentication);
  return 0;
}

int WebWorker::ProcessLoginPage(const std::string& reasonForLogin)
{ MacroRegisterFunctionWithName("WebWorker::ProcessLoginPage");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetLoginPage(reasonForLogin);
  return 0;
}

int WebWorker::ProcessSetProblemDatabaseInfo()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSetProblemDatabaseInfo");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetSetProblemDatabaseInfoHtml();
  return 0;
}

int WebWorker::ProcessAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAddUserEmails");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetAddUserEmails();
  return 0;
}

int WebWorker::ProcessModifyPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessModifyPage");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::ModifyProblemReport();
  return 0;
}

int WebWorker::ProcessAssignTeacherToSection()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAssignTeacherToSection");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::AddTeachersSections();
  return 0;
}

int WebWorker::ProcessScores()
{ MacroRegisterFunctionWithName("WebWorker::ProcessScores");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetScoresPage();
  return 0;
}

int WebWorker::ProcessAccounts()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAccounts");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetAccountsPage(this->hostWithPort);
  return 0;
}

int WebWorker::ProcessDatabase()
{ MacroRegisterFunctionWithName("WebWorker::ProcessDatabase");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetDatabasePage();
  return 0;
}

int WebWorker::ProcessDatabaseOneEntry()
{ MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseOneEntry");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetDatabaseOneEntry();
  return 0;
}

int WebWorker::ProcessEditPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessEditPage");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetEditPageHTML();
  return 0;
}

int WebWorker::ProcessClonePage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessClonePage");
  this->SetHeaderOKNoContentLength();
  stOutput << this->GetClonePageResult();
  return 0;
}

int WebWorker::ProcessProblemGiveUp()
{ MacroRegisterFunctionWithName("WebWorker::ProcessProblemGiveUp");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetAnswerOnGiveUp();
  return 0;
}

int WebWorker::ProcessProblemSolution()
{ MacroRegisterFunctionWithName("WebWorker::ProcessProblemSolution");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::GetProblemSolution();
  return 0;
}

int WebWorker::ProcessSubmitProblemPreview()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblemPreview");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::SubmitProblemPreview();
  return 0;
}

std::string WebWorker::GetClonePageResult()
{ MacroRegisterFunctionWithName("WebWorker::GetClonePageResult");
  this->SetHeaderOKNoContentLength();
  return HtmlInterpretation::ClonePageResult();
}

std::string WebWorker::GetAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::GetAddUserEmails");
  return HtmlInterpretation::AddUserEmails(this->hostWithPort);
}

std::string HtmlInterpretation::ModifyProblemReport()
{ MacroRegisterFunctionWithName("WebWorker::GetModifyProblemReport");
  bool shouldProceed=theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights();
  if (shouldProceed)
    shouldProceed= theGlobalVariables.flagUsingSSLinCurrentConnection;
  if (!shouldProceed)
    return "<b>Modifying problems allowed only for logged-in admins under ssl connection. </b>";
  std::string mainInput=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  std::string fileName=
  CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"), false);
  std::fstream theFile;
  std::stringstream out;
  if (!FileOperations::OpenFileVirtual(theFile, fileName, false, true, false))
  { out << "<b><span style=\"color:red\">Failed to open file: " << fileName << ". </span></b>";
    return out.str();
  }
  theFile << mainInput;
  theFile.close();
  out << "<b><span style=\"color:green\">Written content to file: "
  << fileName << ". </span></b>";
  return out.str();
}

std::string WebWorker::GetEditPageHTML()
{ MacroRegisterFunctionWithName("WebWorker::GetEditPageHTML");
  this->SetHeaderOKNoContentLength();
  return HtmlInterpretation::GetEditPageHTML();
}

std::string WebWorker::GetExamPageInterpreter()
{ MacroRegisterFunctionWithName("WebWorker::GetExamPageInterpreter");
  this->SetHeaderOKNoContentLength();
  return HtmlInterpretation::GetExamPageInterpreter();
}

int WebWorker::ProcessSubmitProblem()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblem");
  this->SetHeaderOKNoContentLength();
  stOutput << HtmlInterpretation::SubmitProblem();
  return 0;
}

std::string WebWorker::GetLoginPage(const std::string& reasonForLogin)
{ MacroRegisterFunctionWithName("WebWorker::GetLoginPage");
  std::stringstream out;
  out << "<html>"
  << HtmlSnippets::GetJavascriptStandardCookies()
  << WebWorker::GetJavascriptSubmitLoginInfo()
  << "<body";
  out << " onload=\"loadSettings();";
  out << "\"";
  out << ">\n";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  //out << "DEBUG: " << this->ToStringMessageFullUnsafe();
//  out << WebWorker::ToStringCalculatorArgumentsHumanReadable();
  out << WebWorker::GetLoginHTMLinternal(reasonForLogin) << "</body></html>";
  return out.str();
}

bool WebWorker::CheckRequestsAddressesReturnFalseIfModified()
{ MacroRegisterFunctionWithName("WebWorker::CheckRequestsAddressesReturnFalseIfModified");
  bool stateNotModified=true;
  if ((this->addressComputed=="/" || this->addressComputed=="" ||
      (this->addressComputed==theGlobalVariables.DisplayNameExecutable &&
       theGlobalVariables.userCalculatorRequestType==""))
      && !theGlobalVariables.flagSSLisAvailable)
  { this->addressComputed=theGlobalVariables.DisplayNameExecutable;
    theGlobalVariables.userCalculatorRequestType="calculator";
    stateNotModified=false;
  }
  bool shouldFallBackToDefaultPage=false;
  if (theGlobalVariables.flagLoggedIn)
  { if (this->addressComputed=="/" || this->addressComputed=="")
      shouldFallBackToDefaultPage=true;
    else if (this->addressComputed==theGlobalVariables.DisplayNameExecutable &&
             theGlobalVariables.userCalculatorRequestType=="")
      shouldFallBackToDefaultPage=true;
    else if (theGlobalVariables.userCalculatorRequestType=="template" &&
             theGlobalVariables.GetWebInput("courseHome")=="")
      shouldFallBackToDefaultPage=true;
  }
  if (shouldFallBackToDefaultPage)
  { this->addressComputed="/html/selectCourse.html";
    theGlobalVariables.userCalculatorRequestType="selectCourse";
    stateNotModified=false;
  }
  return stateNotModified;
}

int WebWorker::ServeClient()
{ MacroRegisterFunctionWithName("WebWorker::ServeClient");
  theGlobalVariables.flagComputationStarted=true;
  this->flagMustLogin=true;
  theGlobalVariables.IndicatorStringOutputFunction=WebServer::PipeProgressReportToParentProcess;
//  std::cout << "GOT TO HERE 0" << std::endl;
  if (this->requestTypE!=this->requestGet && this->requestTypE!=this->requestPost
      && this->requestTypE!=this->requestHead)
  { if (this->requestTypE!=this->requestUnknown &&
        this->requestTypE!=this->requestChunked)
      crash << "Something is wrong: request type does not have any of the expected values. " << crash;
    this->ProcessUnknown();
    return 0;
  }
//  std::cout << "GOT TO HERE" << std::endl;
  if (theGlobalVariables.theThreads.size<=1)
    crash << "Number of threads must be at least 2 in this point of code..." << crash;
  this->ExtractHostInfo();
  this->ExtractAddressParts();
//  std::cout << "GOT TO HERE 2" << std::endl;
  std::stringstream argumentProcessingFailureComments;
  this->flagArgumentsAreOK=true;
  if (!this->ExtractArgumentsFromMessage(this->argumentComputed, argumentProcessingFailureComments))
    this->flagArgumentsAreOK=false;
  if (!this->ExtractArgumentsFromCookies(argumentProcessingFailureComments))
    this->flagArgumentsAreOK=false;
  theGlobalVariables.userCalculatorRequestType="";
  if (this->addressComputed==theGlobalVariables.DisplayNameExecutable)
    theGlobalVariables.userCalculatorRequestType=theGlobalVariables.GetWebInput("request");
  this->flagMustLogin=this->parent->RequiresLogin(theGlobalVariables.userCalculatorRequestType, this->addressComputed);
  if (this->flagMustLogin && !theGlobalVariables.flagUsingSSLinCurrentConnection && theGlobalVariables.flagSSLisAvailable)
  { std::stringstream redirectStream, newAddressStream;
    newAddressStream << "https://" << this->hostNoPort;
    if (!this->parent->flagPort8155)
      newAddressStream << ":" << this->parent->httpSSLPort;
    if (this->addressGetOrPost.size()!=0)
      if (this->addressGetOrPost[0]!='/')
        this->addressGetOrPost='/'+this->addressGetOrPost;
    newAddressStream << this->addressGetOrPost;
    if (theGlobalVariables.flagRunningApache)
      redirectStream << "Content-Type: text/html\r\n";
    redirectStream << "Location: " << newAddressStream.str();
    //double fixme;
    this->SetHeadeR("HTTP/1.0 301 Moved Permanently", redirectStream.str());
    //this->SetHeaderOKNoContentLength();
    stOutput << "<html><body>Address available through secure (SSL) connection only. "
    << "Click <a href=\"" << newAddressStream.str() << "\">here</a> if not redirected automatically. ";
    if (theGlobalVariables.flagRunningApache)
      stOutput << "To avoid seeing this message, <b><span \"style=color:red\">please use the secure version:</span></b> "
      << "<a href=\"https://" << this->hostNoPort << "\">https://" << this->hostNoPort
      << "</a>. If using bookmarks, don't forget to re-bookmark to the secure site. ";
    stOutput << "</body></html>";
    return 0;
  }
  this->flagArgumentsAreOK=this->Login(argumentProcessingFailureComments);
  //stOutput << "DEBUG: this->flagPasswordWasSubmitted: " << this->flagPasswordWasSubmitted;
  if (this->flagPasswordWasSubmitted &&
      theGlobalVariables.userCalculatorRequestType!="changePassword" &&
      theGlobalVariables.userCalculatorRequestType!="activateAccount")
  { std::stringstream redirectedAddress;
    if (this->addressComputed==theGlobalVariables.DisplayNameExecutable)
      redirectedAddress << theGlobalVariables.DisplayNameExecutable << "?request="
      << theGlobalVariables.userCalculatorRequestType << "&";
    else
      redirectedAddress << this->addressComputed << "?";
    for (int i=0; i<theGlobalVariables.webArguments.size(); i++)
      if (theGlobalVariables.webArguments.theKeys[i]!="password" &&
          theGlobalVariables.webArguments.theKeys[i]!="request")
        redirectedAddress << theGlobalVariables.webArguments.theKeys[i] << "="
        << theGlobalVariables.webArguments.theValues[i] << "&";
    std::stringstream headerStream;
    headerStream << "Location: " << redirectedAddress.str();
    //double fixme;
    this->SetHeadeR("HTTP/1.0 303 See other", headerStream.str());
    //this->SetHeaderOKNoContentLength();
    stOutput << "<html><head>"
    << "<meta http-equiv=\"refresh\" content=\"0; url='" << redirectedAddress.str()
    << "'\" />"
    << "</head>"
    << "<body>Click <a href=\"" << redirectedAddress.str() << "\">"
    << " here " << "</a> if your browser does not redirect the page automatically. ";

    //stOutput << "<hr>DEBUG: addressComputed: <br>" << CGI::StringToHtmlString(this->addressComputed)
    //<< "<hr>addressToRedirectTo: <br>\n" << CGI::StringToHtmlString(redirectedAddress.str())
    //<< "<hr>argumentComputed: <br>\n" << CGI::StringToHtmlString(this->argumentComputed)
    //<< "<hr>addressGetOrPost: <br>" << CGI::StringToHtmlString(this->addressGetOrPost)
    //<< "<hr>MessageBody: <br>\n" << CGI::StringToHtmlString(this->messageBody)
    //<< " <br>MessageHead: <br>\n" << CGI::StringToHtmlString(this->messageHead)
    //<< HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
    //<< this->ToStringMessageFullUnsafe();
    stOutput << "</body></html>";
    return 0;
  }
  if (!theGlobalVariables.flagLoggedIn &&
       theGlobalVariables.flagUsingSSLinCurrentConnection &&
       this->flagMustLogin
      )
  { if(theGlobalVariables.userCalculatorRequestType!="logout" &&
       theGlobalVariables.userCalculatorRequestType!="login")
    { argumentProcessingFailureComments << "<b>Accessing: ";
      if (theGlobalVariables.userCalculatorRequestType!="")
        argumentProcessingFailureComments << theGlobalVariables.userCalculatorRequestType;
      else
        argumentProcessingFailureComments << "[html base folder]";
      argumentProcessingFailureComments << " requires login. </b>";
    }
    return this->ProcessLoginPage(argumentProcessingFailureComments.str());
  }
  if (argumentProcessingFailureComments.str()!="")
    theGlobalVariables.SetWebInpuT("error", argumentProcessingFailureComments.str());
  this->errorCalculatorArguments=argumentProcessingFailureComments.str();
  this->CheckRequestsAddressesReturnFalseIfModified();

  if (theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
      theGlobalVariables.userCalculatorRequestType=="database")
    return this->ProcessDatabase();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
           theGlobalVariables.userCalculatorRequestType=="databaseOneEntry")
    return this->ProcessDatabaseOneEntry();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType=="accounts")
    return this->ProcessAccounts();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
           theGlobalVariables.userCalculatorRequestType=="navigation")
    return this->ProcessNavigation();
  else if (theGlobalVariables.userCalculatorRequestType=="calculatorExamples")
    return this->ProcessCalculatorExamples();
  else if (theGlobalVariables.GetWebInput("error")!="")
    stOutput << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("error"), false);
  else if (this->errorCalculatorArguments!="")
    stOutput << this->errorCalculatorArguments;
  else if (theGlobalVariables.userCalculatorRequestType=="status")
    return this->ProcessServerStatus();
  else if (theGlobalVariables.userCalculatorRequestType=="statusPublic")
    return this->ProcessServerStatusPublic();
  else if (theGlobalVariables.flagAllowProcessMonitoring)
  { if (theGlobalVariables.userCalculatorRequestType=="monitor")
      return this->ProcessMonitor();
    else if (theGlobalVariables.userCalculatorRequestType=="pause")
      return this->ProcessPauseWorker();
    else if (theGlobalVariables.userCalculatorRequestType=="indicator")
      return this->ProcessComputationIndicator();
  }
  //unless the worker is an server monitor, it has no access to communication channels of the other workers
  this->parent->ReleaseNonActiveWorkers();
  //stOutput << this->GetHeaderOKNoContentLength();
  //stOutput << "<html><body> got to here pt 1";
  if (theGlobalVariables.userCalculatorRequestType=="setProblemData")
    return this->ProcessSetProblemDatabaseInfo();
  else if (theGlobalVariables.userCalculatorRequestType=="changePassword")
    return this->ProcessChangePassword();
  else if (theGlobalVariables.userCalculatorRequestType=="changePasswordPage" ||
           theGlobalVariables.userCalculatorRequestType=="activateAccount")
    return this->ProcessChangePasswordPage();
  else if (theGlobalVariables.userCalculatorRequestType=="login")
    return this->ProcessLoginPage();
  else if (theGlobalVariables.userCalculatorRequestType=="logout")
    return this->ProcessLogout();
  else if ((theGlobalVariables.userCalculatorRequestType=="addEmails"||
            theGlobalVariables.userCalculatorRequestType=="addUsers") &&
            theGlobalVariables.flagLoggedIn)
    return this->ProcessAddUserEmails();
  else if (theGlobalVariables.userCalculatorRequestType=="setTeacher"
           && theGlobalVariables.flagLoggedIn)
    return this->ProcessAssignTeacherToSection();
  else if ((theGlobalVariables.userCalculatorRequestType=="submitProblem" ||
            theGlobalVariables.userCalculatorRequestType=="submitExercise")
            && theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblem();
  else if (theGlobalVariables.userCalculatorRequestType=="scores" && theGlobalVariables.flagLoggedIn)
    return this->ProcessScores();
  else if (theGlobalVariables.UserGuestMode() && theGlobalVariables.userCalculatorRequestType=="submitExerciseNoLogin")
    return this->ProcessSubmitProblem();
  else if ((theGlobalVariables.userCalculatorRequestType=="problemGiveUp" &&
            theGlobalVariables.flagLoggedIn) ||
            theGlobalVariables.userCalculatorRequestType== "problemGiveUpNoLogin")
    return this->ProcessProblemGiveUp();
  else if ((theGlobalVariables.userCalculatorRequestType=="problemSolution" &&
            theGlobalVariables.flagLoggedIn) ||
            theGlobalVariables.userCalculatorRequestType== "problemSolution")
    return this->ProcessProblemSolution();
  else if ((theGlobalVariables.userCalculatorRequestType=="submitProblemPreview" ||
            theGlobalVariables.userCalculatorRequestType=="submitExercisePreview" ) &&
           theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblemPreview();
  else if (theGlobalVariables.userCalculatorRequestType=="submitExercisePreviewNoLogin" &&
           theGlobalVariables.UserGuestMode())
    return this->ProcessSubmitProblemPreview();
  else if ((theGlobalVariables.userCalculatorRequestType=="scoredQuiz" ||
            theGlobalVariables.userCalculatorRequestType=="exercise"))
  { if (theGlobalVariables.UserSecureNonAdminOperationsAllowed())
      return this->ProcessExamPage();
    else
      return this->ProcessLoginPage();
  } else if (theGlobalVariables.userCalculatorRequestType=="exerciseNoLogin")
  { //stOutput << "DEBUG: Got to here; exerciseNoLogin";
    return this->ProcessExamPage();
  } else if (theGlobalVariables.userCalculatorRequestType=="template")
    return this->ProcessTemplate();
  else if (theGlobalVariables.userCalculatorRequestType=="topicTable")
    return this->ProcessTopicTable();
  else if (theGlobalVariables.userCalculatorRequestType=="editPage")
    return this->ProcessEditPage();
  else if (theGlobalVariables.userCalculatorRequestType=="modifyPage")
    return this->ProcessModifyPage();
  else if (theGlobalVariables.userCalculatorRequestType=="clonePage")
    return this->ProcessClonePage();
  else if (theGlobalVariables.userCalculatorRequestType=="calculator")
    return this->ProcessCalculator();
  else if (theGlobalVariables.userCalculatorRequestType=="compute")
  { //std::cout << "DEBUG: got to here!";
    //std::cout.flush();
    return this->ProcessCompute();
  }
//  stOutput << "<html><body> got to here pt 2";
  this->VirtualFileName=CGI::URLStringToNormal(this->addressComputed,true);
  this->SanitizeVirtualFileName();
//    std::cout << "GOT TO file names!" << std::endl;
  if (!FileOperations::GetPhysicalFileNameFromVirtual
      (this->VirtualFileName, this->RelativePhysicalFileNamE, theGlobalVariables.UserDefaultHasAdminRights()))
  { //  std::cout << "GOT TO not found!" << std::endl;
    this->SetHeadeR("HTTP/1.0 404 Object not found", "Content-Type: text/html");
    stOutput << "<html><body><b>File name deemed unsafe. "
    << "Please note that folder names are not allowed to contain dots and file names "
    << "are not allowed to start with dots.</b> There may be additional restrictions "
    << "on file names added for security reasons.</body></html>";
    return 0;
  }
  if (FileOperations::IsFolderUnsecure(this->RelativePhysicalFileNamE))
    return this->ProcessFolder();
  return this->ProcessFile();
}

void WebWorker::ResetPipesNoAllocation()
{ MacroRegisterFunctionWithName("WebWorker::ResetPipesNoAllocation");
  this->PauseComputationReportReceived.ResetNoAllocation();
  this->PauseWorker.ResetNoAllocation();
  this->PauseIndicatorPipeInUse.ResetNoAllocation();
  this->pipeWorkerToServerTimerPing.ResetNoAllocation();
  this->pipeWorkerToServerControls.ResetNoAllocation();
  this->pipeServerToWorkerRequestIndicator.ResetNoAllocation();
  this->pipeWorkerToServerIndicatorData.ResetNoAllocation();
  this->pipeWorkerToServerUserInput.ResetNoAllocation();
  this->pipeWorkerToServerWorkerStatus.ResetNoAllocation();
}

void WebWorker::ReleaseKeepInUseFlag()
{ MacroRegisterFunctionWithName("WebWorker::ReleaseMyPipe");
  this->pipeWorkerToServerTimerPing.Release();
  this->pipeWorkerToServerControls.Release();
  this->pipeServerToWorkerRequestIndicator.Release();
  this->pipeWorkerToServerIndicatorData.Release();
  this->pipeWorkerToServerUserInput.Release();
  this->pipeWorkerToServerWorkerStatus.Release();
  this->PauseComputationReportReceived.Release();
  this->PauseWorker.Release();
  this->PauseIndicatorPipeInUse.Release();
  this->timeOfLastPingServerSideOnly=-1;
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release()
{ this->ReleaseKeepInUseFlag();
  this->flagInUse=false;
}

void WebWorker::OutputShowIndicatorOnTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputShowIndicatorOnTimeout");
  MutexLockGuard theLock(this->PauseIndicatorPipeInUse.mutexForProcessBlocking.GetElement());
//  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
  theGlobalVariables.flagOutputTimedOut=true;
  theGlobalVariables.flagTimedOutComputationIsDone=false;
  //  ProgressReportWebServer theReport("WebServer::OutputShowIndicatorOnTimeout");
  theLog << logger::blue << "Computation timeout, sending progress indicator instead of output. " << logger::endL;
  if (theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed)
  { stOutput << "Your computation is taking more than " << theGlobalVariables.MaxComputationTimeBeforeWeTakeAction
    << " seconds. ";
  }
  if (!theGlobalVariables.flagAllowProcessMonitoring)
  { stOutput << "Monitoring computations is not allowed on this server (to avoid server load).<br> "
    << "If you want to carry out a long computation you will need to install the calculator on your own machine.<br> "
    << "At the moment, the only way to do that is by setting the variable "
    << "theGlobalVariables.flagAllowProcessMonitoring to true, "
    << "achieved through manually compiling the calculator from source.<br> ";
    return;
  }
  stOutput << "A progress indicator, as reported by your current computation, is displayed below. "
  << "When done, your computation result will be displayed below. ";
  stOutput << " <!>\n";
//  out << "\n<br>\n<button onclick=\"progressReport()\">Manual report</button>";
  if (this->indexInParent<0)
    crash << "Index of worker is smaller than 0, this shouldn't happen. " << crash;
  stOutput << "\n<script language=\"javascript\">\n"
  << "isFinished=false;\n "
  << "isPaused=false;\n "
  << "currentWorkerNumber=" << this->indexInParent+1 << ";\n "
  << "progressReport();\n"
  << "</script>"
//  << "DEBUG: javascript: " << "progressReport();"
  ;
  stOutput << "\n<br>\n<button id=\"idButtonSendTogglePauseRequest\" onclick=\"SendTogglePauseRequest()\">Pause</button>";
  stOutput << "\n<span id=\"idProgressReportRequestStatus\"></span>";
  stOutput << "<span id=\"idProgressReportTimer\"></span>\n";
  stOutput << "\n<br>\n<div id=\"idProgressReport\"></div>\n";

  this->SendAllBytesWithHeaders();
  for (int i=0; i<this->parent->theWorkers.size; i++)
    if (i!=this->indexInParent)
      this->parent->theWorkers[i].Release();
  //this->parent->Release(this->pipeServerToWorkerControls[0]);
  //this->parent->Release(this->pipeServerToWorkerControls[1]);
  //this->parent->Release(this->pipeWorkerToServerControls[0]);
  //this->parent->Release(this->pipeWorkerToServerControls[1]);
//  this->parent->Release(this->pipeServerToWorkerIndicator[0]);
//  this->parent->Release(this->pipeServerToWorkerIndicator[1]);
//  this->parent->Release(this->pipeWorkerToServerIndicator[0]);
//  this->parent->Release(this->pipeWorkerToServerIndicator[1]);
  this->parent->Release(this->connectedSocketID);
  //set flags properly:
  //we need to rewire the standard output and the crashing mechanism:
  crash.CleanUpFunction=WebWorker::OutputCrashAfterTimeout;
  //note that standard output cannot be rewired in the beginning of the function as we use the old stOutput
  stOutput.theOutputFunction=WebWorker::StandardOutputAfterTimeOut;
  //theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: continuing computation.");
  this->PauseIndicatorPipeInUse.mutexForProcessBlocking.GetElement().UnlockMe();
  //theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: exiting function.");
//  this->SignalIamDoneReleaseEverything();
//  theLog << consoleGreen("Indicator: released everything and signalled end.") << logger::endL;
}

std::string WebWorker::ToStringStatus()const
{ std::stringstream out;
//  if (theWebServer.currentlyConnectedAddresses.GetCoefficientsSum()!=theWebServer.
  out << "<br>Worker " << this->indexInParent+1;
  if (this->flagInUse)
  { if (this->parent->activeWorker==this->indexInParent)
      out << ", <span style=\"color:green\"><b>current process</b></span>";
    else
      out << ", <b>in use</b>";
    out << ", <a href=\"calculator?request=monitor&mainInput=" << this->indexInParent +1 << "\">monitor process "
    << this->indexInParent +1 << "</a>";
  } else
    out << ", not in use";
  if (this->displayUserInput!="")
    out << ", user input: <span style=\"color:blue\">" << this->displayUserInput << "</span>";
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->ProcessPID!=0)
    out << this->ProcessPID;
  else
    out << "(not accessible)";
  out << ", socketID: ";
  if (this->connectedSocketID==-1)
    out << "released in current process, value before release: " << this->connectedSocketIDLastValueBeforeRelease;
  else
    out << this->connectedSocketID;
  out << ". ";
  out << " Server time at last ping: " << this->timeOfLastPingServerSideOnly << " seconds. ";
  if (this->pingMessage!="")
    out << " Message at last ping: " << this->pingMessage;
  if (this->status!="")
    out << "<br><span style=\"color:red\"><b> Status: " << this->status << "</b></span><br>";
  out << "Pipe indices: " << this->pipeWorkerToServerControls.ToString()
  << ", " << this->pipeWorkerToServerTimerPing.ToString()
  << ", " << this->pipeServerToWorkerRequestIndicator.ToString()
  << ", " << this->pipeWorkerToServerIndicatorData.ToString()
  << ", " << this->pipeWorkerToServerUserInput.ToString()
  << ", " << this->pipeWorkerToServerWorkerStatus.ToString()
  << ", " << this->PauseWorker.ToString()
  << ", " << this->PauseComputationReportReceived.ToString()
  << ", " << this->PauseIndicatorPipeInUse.ToString()
  ;
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of WebServer." << crash;
  return true;
}

void WebServer::ReleaseEverything()
{ this->SSLfreeResources();
  ProgressReportWebServer::flagServerExists=false;
  for (int i=0; i<this->theWorkers.size; i++)
    this->theWorkers[i].Release();
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=0;
  //theGlobalVariables.WebServerTimerPing=0;
  theGlobalVariables.IndicatorStringOutputFunction=0;
  theGlobalVariables.PauseUponUserRequest=0;
  this->activeWorker=-1;
  //theLog << logger::red << "DEBUG: About to close: " << this->listeningSocketHTTP << logger::endL;
  if (this->listeningSocketHTTP!=-1)
    close(this->listeningSocketHTTP);
  //theLog << logger::red << "DEBUG: Just closed: " << this->listeningSocketHTTP << logger::endL;
  this->listeningSocketHTTP=-1;
  if (this->listeningSocketHttpSSL!=-1)
    close(this->listeningSocketHttpSSL);
  //theLog << logger::red << "DEBUG: Just closed: " << this->listeningSocketHttpSSL << logger::endL;
  this->listeningSocketHttpSSL=-1;
}

WebServer::~WebServer()
{ this->flagDeallocated=true;
}

void WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone()
{ //theLog << logger::red << ("Got THUS far") << logger::endL;
//  theLog << "here am i";
  MacroRegisterFunctionWithName("WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone");
  theWebServer.GetActiveWorker().OutputShowIndicatorOnTimeout();
//  stOutput << "What the hell";
//  stOutput.Flush();
}

void WebServer::FlushActiveWorker()
{ theWebServer.GetActiveWorker().SendAllBytesWithHeaders();
}

void WebServer::SendStringThroughActiveWorker(const std::string& theString)
{ theWebServer.GetActiveWorker().QueueStringForSendingWithHeadeR(theString, false);
}

void WebServer::PipeProgressReportToParentProcess(const std::string& theString)
{ theWebServer.GetActiveWorker().PipeProgressReportToParentProcess(theString);
}

WebServer::WebServer()
{ this->flagPort8155=true;
  this->flagDeallocated=false;
  this->flagTryToKillOlderProcesses=true;
  this->activeWorker=-1;
  this->timeLastExecutableModification=-1;
  this->listeningSocketHTTP=-1;
  this->listeningSocketHttpSSL=-1;
  this->highestSocketNumber=-1;
  this->flagSSLHandshakeSuccessful=false;
  this->flagReapingChildren=false;
  this->MaxNumWorkersPerIPAdress=8;
  this->MaxTotalUsedWorkers=20;
  this->NumFailedSelectsSoFar=0;
  this->NumSuccessfulSelectsSoFar=0;
  this->NumProcessesReaped=0;
  this->NumProcessAssassinated=0;
  this->NumConnectionsSoFar=0;
  this->NumWorkersNormallyExited=0;
  this->WebServerPingIntervalInSeconds=20;
  this->flagThisIsWorkerProcess=false;
  this->requestStartsNotNeedingLogin.AddOnTop("compute");
  this->requestStartsNotNeedingLogin.AddOnTop("calculator");
  this->requestStartsNotNeedingLogin.AddOnTop("calculatorExamples");
  this->requestStartsNotNeedingLogin.AddOnTop("exerciseNoLogin");
  this->requestStartsNotNeedingLogin.AddOnTop("submitExerciseNoLogin");
  this->requestStartsNotNeedingLogin.AddOnTop("submitExercisePreviewNoLogin");
  this->requestStartsNotNeedingLogin.AddOnTop("problemGiveUpNoLogin");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/MathJax-2.6-latest/");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("MathJax-2.6-latest/");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/jquery.min.js");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/jquery.min.js");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/jquery-ui.css");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/jquery-ui.css");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/jquery-ui.min.css");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/jquery-ui.min.css");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/images");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/images");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/mathquill.css");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/mathquill.css");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/mathquill.min.js");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/mathquill.min.js");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common-calculator/mathquill.min-matrix.js");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("html-common-calculator/mathquill.min-matrix.js");
  this->addressStartsNotNeedingLogin.AddOnTop("cache.appcache");
  this->addressStartsNotNeedingLogin.AddOnTop("/cache.appcache");
  this->addressStartsNotNeedingLogin.AddOnTop("favicon.ico");
  this->addressStartsNotNeedingLogin.AddOnTop("/favicon.ico");
  this->addressStartsNotNeedingLogin.AddOnTop("html-common/");
  this->addressStartsNotNeedingLogin.AddOnTop("/html-common/");
  this->addressStartsNotNeedingLogin.AddOnTop("html-common-calculator/");
  this->addressStartsNotNeedingLogin.AddOnTop("/html-common-calculator/");
  this->addressStartsNotNeedingLogin.AddOnTop("css/");
  this->addressStartsNotNeedingLogin.AddOnTop("/css/");
  this->addressStartsNotNeedingLogin.AddOnTop("javascriptlibs/");
  this->addressStartsNotNeedingLogin.AddOnTop("/javascriptlibs/");
  this->addressStartsNotNeedingLogin.AddOnTop("MathJax-2.6-latest/");
  this->addressStartsNotNeedingLogin.AddOnTop("/MathJax-2.6-latest/");
  this->addressStartsNotNeedingLogin.AddOnTop("login");
  this->addressStartsNotNeedingLogin.AddOnTop("/login");  
  //NO! Adding "logout", for example: this->addressStartsNotNeedingLogin.AddOnTop("logout");
  //is FORBIDDEN! Logging someone out definitely requires authentication (imagine someone
  //asking for logouts on your account once every second: this would be fatal as proper logout resets
  //the authentication tokens.

}

WebWorker& WebServer::GetActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  void (*oldOutputFunction)(const std::string& stringToOutput) =stOutput.theOutputFunction;
  stOutput.theOutputFunction=0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrupt) web server
  //to report the error over the Internet.
  if (this->activeWorker<0 || this->activeWorker>=this->theWorkers.size)
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  stOutput.theOutputFunction=oldOutputFunction;//<-the web server is in order,
  //therefore we restore the ability to report crashes over the internet.
  return this->theWorkers[this->activeWorker];
}

void WebServer::SignalActiveWorkerDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebServer::SignalActiveWorkerDoneReleaseEverything");
  if (theWebServer.activeWorker==-1)
    return;
  theWebServer.GetActiveWorker().SendAllAndWrapUp();
  theWebServer.activeWorker=-1;
}

void WebServer::ReleaseActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::ReleaseActiveWorker");
  if (this->activeWorker==-1)
    return;
  this->GetActiveWorker().Release();
  this->activeWorker=-1;
}

void WebServer::WorkerTimerPing(double pingTime)
{
  if (theWebServer.activeWorker==-1)
  { if (!theGlobalVariables.flagComputationFinishedAllOutputSentClosing)
      crash << "WebServer::WorkerTimerPing called when the computation is not entirely complete. " << crash;
    return;
  }
  std::stringstream outTimestream;
  outTimestream << "Worker: " << pingTime << " seconds passed. ";
  theWebServer.GetActiveWorker().pipeWorkerToServerTimerPing.WriteAfterEmptying(outTimestream.str());
}

void WebServer::ReleaseNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->theWorkers[i].ReleaseKeepInUseFlag();
}

void WebServer::ReleaseSocketsNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseSocketsNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->Release(this->theWorkers[i].connectedSocketID);
}

bool WebServer::EmergencyRemoval_LastCreatedWorker()
{ this->GetActiveWorker().Release();
  this->activeWorker=-1;
  this->theWorkers.SetSize(this->theWorkers.size-1);
  return false;
}

bool WebServer::CreateNewActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker!=-1)
  { crash << "Calling CreateNewActiveWorker requires the active worker index to be -1." << crash;
    return false;
  }
  bool found=false;
  for (int i=0; i<this->theWorkers.size; i++)
    if (!this->theWorkers[i].flagInUse)
    { this->activeWorker=i;
      found=true;
      break;
    }
  if (this->activeWorker==-1)
  { this->activeWorker=this->theWorkers.size;
    this->theWorkers.SetSize(this->theWorkers.size+1);
  }
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().parent=this;
  this->GetActiveWorker().pingMessage="";
  if (found)
  { this->theWorkers[this->activeWorker].flagInUse=true;
    return true;
  }
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUse=false; //<-until everything is initialized, we cannot be in use.
  std::stringstream stowstream, wtosStream;
  stowstream << "S->W" << this->activeWorker+1 << ": ";
  wtosStream << "W" << this->activeWorker+1 << "->S: ";
  std::string stow = stowstream.str();
  std::string wtos = wtosStream.str();
  if (!this->GetActiveWorker().PauseComputationReportReceived.CreateMe(stow+"report received"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().PauseWorker.CreateMe(stow+"pause"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe(stow+"ind. pipe busy"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeServerToWorkerRequestIndicator.CreateMe(stow+"request indicator"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerTimerPing.CreateMe(wtos+"ping"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerControls.CreateMe(wtos+"controls"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerIndicatorData.CreateMe(wtos+"indicator data"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerUserInput.CreateMe(wtos+"user input"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerWorkerStatus.CreateMe(wtos+"worker status"))
    return this->EmergencyRemoval_LastCreatedWorker();
  logPlumbing << logger::green
  << "Allocated new worker & plumbing data structures. Total worker data structures: "
  << this->theWorkers.size << ". "
  << logger::endL;
  this->theWorkers[this->activeWorker].flagInUse=true;
  return true;
}

std::string WebServer::ToStringLastErrorDescription()
{ std::stringstream out;
  out << logger::red << "Process " << this->ToStringActiveWorker() << ": " << strerror(errno) << ". ";
  return out.str();
}

std::string WebServer::ToStringActiveWorker()
{ if (theWebServer.activeWorker==-1)
    return "server";
  std::stringstream out;
  out << "worker " << theWebServer.activeWorker;
  return out.str();
}

std::string WebServer::ToStringStatusActive()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusActive");
  if (this->activeWorker==-1)
    return "server.";
  std::stringstream out;
  if (this->activeWorker!=this->GetActiveWorker().indexInParent)
    crash << "Bad index in parent!" << crash;
  out << this->GetActiveWorker().ToStringStatus();
  return out.str();
}

std::string WebServer::ToStringStatusPublicNoTop()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusPublicNoTop");
  if (theGlobalVariables.flagRunningApache)
     return "Running through standard Apache web server, no connection details to display. ";
  std::stringstream out;
  out << "<b>The calculator web server status.</b><hr>";
  out
  << "<br>" << this->GetActiveWorker().timeOfLastPingServerSideOnly
  << " seconds = "
  << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
  (this->GetActiveWorker().timeOfLastPingServerSideOnly, false)
  << " web server uptime. ";
  int approxNumPings=
  this->GetActiveWorker().timeOfLastPingServerSideOnly/this->WebServerPingIntervalInSeconds;
  if (approxNumPings<0)
    approxNumPings=0;
  int numConnectionsSoFarApprox=this->NumConnectionsSoFar-approxNumPings;
  if (numConnectionsSoFarApprox<0)
    numConnectionsSoFarApprox=0;
  out << "~" << numConnectionsSoFarApprox << " actual connections + ~"
  << approxNumPings << " self-test-pings (" << this->NumConnectionsSoFar << " connections total)"
  << " served since last restart. "
  << "This counts one connection per problem answer preview, page visit, progress report ping, etc. "
  ;

  out << "<br>" << this->theWorkers.size  << " = peak number of concurrent connections. "
  << " The number tends to be high as many browsers open more than one connection per page visit. <br>"
  << "<b>The following policies are only temporary and will be relaxed as we roll the ACE webserver into"
  << " production. </b><br>"
  << this->MaxTotalUsedWorkers << " global maximum of simultaneous non-closed connections allowed. "
  << "When the limit is exceeded, a random connection will be terminated. "
  << "<br>  " << this->MaxNumWorkersPerIPAdress
  << " maximum simultaneous connection per IP address. "
  << "When the limit is exceeded, a random connection from that IP address will be terminated. "
  ;
  return out.str();
}

std::string WebServer::ToStringStatusPublic()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusPublic");
  std::stringstream out;
  out << "<html><body>";
  out << this->ToStringStatusPublicNoTop();
  std::string topString= theGlobalVariables.CallSystemWithOutput("top -b -n 1 -s");
  //out << "<br>" << topString;
  std::string lineString, wordString;
  std::stringstream topStream(topString);
  out << "<hr><b>Server machine details.</b><br>";
  for (int i=0; i<5; i++)
  { std::getline(topStream, lineString);
    out << lineString << "<br>\n ";
  }
  out << "The math libraries of the calculator can be found here: "
  << "<a href=\"https://sourceforge.net/p/vectorpartition/code/HEAD/tree/\">calculator source code</a>. "
  << "The rest of the code is property of ACE learning. ";

  out << "</body></html>";
  return out.str();
}

std::string WebServer::ToStringStatusAll()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusAll");
  if (theGlobalVariables.flagRunningApache)
    return "Running through Apache. ";
  std::stringstream out;
  out << this->ToStringStatusPublicNoTop() << "<hr>";
  if (this->activeWorker==-1)
    out << "The process is functioning as a server.";
  else
  { out << "The process is functioning as a worker. The active worker is number " << this->activeWorker+1 << ". ";
    out << "<br>" << this->ToStringStatusActive();
  }
  int numInUse=0;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
      numInUse++;
  for (int i=0; i<this->theWorkers.size; i++)
  { WebWorker& currentWorker=this->theWorkers[i];
    if (!currentWorker.flagInUse)
      continue;
    currentWorker.pipeWorkerToServerWorkerStatus.ReadWithoutEmptying();
    currentWorker.status.assign
    (currentWorker.pipeWorkerToServerWorkerStatus.lastRead.TheObjects,
     currentWorker.pipeWorkerToServerWorkerStatus.lastRead.size);
  }
  out << "<hr><hr>" << numInUse << " workers in use out of total " << this->theWorkers.size << " workers. ";
  out << "Connections: " << this->currentlyConnectedAddresses.ToString();
  for (int i=0; i<this->theWorkers.size; i++)
    out << "<hr>" << this->theWorkers[i].ToStringStatus();
  return out.str();
}

void WebServer::CheckExecutableVersionAndRestartIfNeeded(bool callReload)
{ struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
    return;
  if (this->timeLastExecutableModification!=-1)
    if (this->timeLastExecutableModification!=theFileStat.st_ctime)
    { stOutput << "<b>The server executable was updated, but the server has not been restarted yet. "
      << "Restarting in 0.5 seconds...</b>";
      if (callReload)
        stOutput << "<script language=\"javascript\">setTimeout(resubmit, 500); "
        << " function resubmit() { location.reload(true);}</script>";
      else
        stOutput << "<script language=\"javascript\">setTimeout(resubmit, 500); "
        << " function resubmit() { document.getElementById('formCalculator').submit();}</script>";
      if (this->activeWorker!=-1)
      { this->GetActiveWorker().SendAllBytesWithHeaders();
        this->ReleaseActiveWorker();
      }
      theLog << "Current process spawned from file with time stamp: "
      << this->timeLastExecutableModification
      << "; latest executable has different time stamp: " << theFileStat.st_ctime
      << ". " << logger::red << "RESTARTING." << logger::endL;
      this->Restart();
    }
}

void WebServer::Restart()
{ //theLog << "Killing all copies of the calculator and restarting..." << logger::endL;
  //theLog << "Time limit after restart: " << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;
  if (this->listeningSocketHTTP!=-1)
    this->Release(this->listeningSocketHTTP);
  if (this->listeningSocketHttpSSL!=-1)
    this->Release(this->listeningSocketHttpSSL);  std::stringstream theCommand;
  int timeInteger=(int) theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  theLog << logger::red << " restarting with time limit " << timeInteger << logger::endL;
  theCommand << "killall " << theGlobalVariables.PhysicalNameExecutableNoPath << " \r\n./";
  theCommand << theGlobalVariables.PhysicalNameExecutableNoPath;
  if (theWebServer.flagPort8155)
    theCommand << " server " << " nokill " << timeInteger;
  else
    theCommand << " server8080 " << " nokill " << timeInteger;
//  std::cout << "\nCalling: " << theCommand.str() << "\n";
  theGlobalVariables.CallSystemNoOutput(theCommand.str()); //kill any other running copies of the calculator.
}

void WebServer::initPortsITry()
{ if (this->flagPort8155)
    this->PortsITryHttp.AddOnTop("8155");
  this->PortsITryHttp.AddOnTop("8080");
  this->PortsITryHttp.AddOnTop("8081");
  this->PortsITryHttp.AddOnTop("8082");
  this->PortsITryHttp.AddOnTop("8155");
  if (!theGlobalVariables.flagSSLisAvailable)
    return;
  this->PortsITryHttpSSL.AddOnTop("8166");
  this->PortsITryHttpSSL.AddOnTop("8083");
  this->PortsITryHttpSSL.AddOnTop("8084");
  this->PortsITryHttpSSL.AddOnTop("8085");
}

void WebServer::initListeningSockets()
{ MacroRegisterFunctionWithName("WebServer::initListeningSockets");
  if (listen(this->listeningSocketHTTP, WebServer::maxNumPendingConnections) == -1)
    crash << "Listen function failed on http port." << crash;
  if (theGlobalVariables.flagSSLisAvailable)
    if (listen(this->listeningSocketHttpSSL, WebServer::maxNumPendingConnections) == -1)
      crash << "Listen function failed on https port." << crash;
  this->highestSocketNumber=-1;
  if (this->listeningSocketHTTP!=-1)
  { this->theListeningSockets.AddOnTop(this->listeningSocketHTTP);
    this->highestSocketNumber=MathRoutines::Maximum(this->listeningSocketHTTP, this->highestSocketNumber);
  }
  if (this->listeningSocketHttpSSL!=-1)
  { this->theListeningSockets.AddOnTop(this->listeningSocketHttpSSL);
    this->highestSocketNumber=MathRoutines::Maximum(this->listeningSocketHttpSSL, this->highestSocketNumber);
  }
}

void WebServer::initDates()
{ this->timeLastExecutableModification=-1;
  struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
    return;
  this->timeLastExecutableModification=theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources()
{ MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  this->Release(this->GetActiveWorker().connectedSocketID);
  //<-release socket- communication is handled by the worker.
  this->activeWorker=-1; //<-The active worker is needed only in the child process.
}

bool WebServer::RequiresLogin(const std::string& inputRequest, const std::string& inputAddress)
{ MacroRegisterFunctionWithName("WebServer::AddressRequiresLogin");
  if (inputAddress==theGlobalVariables.DisplayNameExecutable)
    for (int i=0; i<this->requestStartsNotNeedingLogin.size; i++)
      if (MathRoutines::StringBeginsWith(inputRequest, this->requestStartsNotNeedingLogin[i]))
        return false;
  for (int i=0; i<this->addressStartsNotNeedingLogin.size; i++)
    if (MathRoutines::StringBeginsWith(inputAddress, this->addressStartsNotNeedingLogin[i]))
      return false;
  return true;
}

void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{ (void) signal; //avoid unused parameter warning, portable.
  (void) arg;
  crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void fperror_sigaction(int signal)
{ (void) signal;
  crash << "Fatal arithmetic error. " << crash;
  exit(0);
}

void WebServer::TerminateChildSystemCall(int i)
{ this->theWorkers[i].flagInUse=false;
  this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
  kill(this->theWorkers[i].ProcessPID, SIGKILL);
  this->theWorkers[i].ResetPipesNoAllocation();
}

void WebServer::HandleTooManyConnections(const std::string& incomingUserAddress)
{ MacroRegisterFunctionWithName("WebServer::HandleTooManyConnections");
  MonomialWrapper<std::string, MathRoutines::hashString>
  incomingAddress(incomingUserAddress);
  bool purgeIncomingAddress=
  (this->currentlyConnectedAddresses.GetMonomialCoefficient(incomingAddress)>
   this->MaxNumWorkersPerIPAdress);
  if (!purgeIncomingAddress)
    return;
  List<double> theTimes;
  List<int> theIndices;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
      if (this->theWorkers[i].userAddress==incomingAddress)
      { theTimes.AddOnTop(this->theWorkers[i].timeServerAtWorkerStart);
        theIndices.AddOnTop(i);
      }
  theTimes.QuickSortAscending(0, &theIndices);
  for (int j=0; j<theTimes.size; j++)
  { this->TerminateChildSystemCall(theIndices[j]);
    std::stringstream errorStream;
    errorStream
    << "Terminating child " << theIndices[j]+1 << " with PID "
    << this->theWorkers[theIndices[j]].ProcessPID
    << ": address: " << incomingAddress << " opened more than "
    << this->MaxNumWorkersPerIPAdress << " simultaneous connections. ";
    this->theWorkers[theIndices[j]].pingMessage=errorStream.str();
    logProcessKills << logger::red  << errorStream.str() << logger::endL;
    this->NumProcessAssassinated++;
  }
}

void WebServer::RecycleChildrenIfPossible()
{ //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
  MacroRegisterFunctionWithName("WebServer::RecycleChildrenIfPossible");
//  this->ReapChildren();
  int numInUse=0;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
      numInUse++;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
    { this->theWorkers[i].pipeWorkerToServerControls.ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY();
      if (this->theWorkers[i].pipeWorkerToServerControls.lastRead.size>0)
      { this->theWorkers[i].flagInUse=false;
        this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
        theLog << logger::green << "Worker "
        << i+1 << " done, marking for reuse. " << logger::endL;
        numInUse--;
        this->NumWorkersNormallyExited++;
//        waitpid(this->theWorkers[i].ProcessPID, 0, )
      } else
        theLog << logger::orange << "Worker " << i+1 << " not done yet. " << logger::endL;
      this->theWorkers[i].pipeWorkerToServerUserInput.ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY();
      if (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size>0)
        this->theWorkers[i].displayUserInput.assign
        (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.TheObjects,
          this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size);
      this->theWorkers[i].pipeWorkerToServerTimerPing.ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY();
      if (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size>0)
      { this->theWorkers[i].pingMessage.assign
        (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.TheObjects,
         this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size);
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else if (this->theWorkers[i].PauseWorker.CheckPauseIsRequested_UNSAFE_SERVER_USE_ONLY())
      { this->theWorkers[i].pingMessage="worker paused, no pings.";
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else
      { bool presumedDead=false;
        if (this->theWorkers[i].flagInUse)
          if (theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead>0)
            if (theGlobalVariables.GetElapsedSeconds()-
                this->theWorkers[i].timeOfLastPingServerSideOnly>
                theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead)
              presumedDead=true;
        if (presumedDead)
        { this->TerminateChildSystemCall(i);
          std::stringstream pingTimeoutStream;
          pingTimeoutStream << theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly
          << " seconds passed since worker " << i+1
          << " last pinged the server; killing pid: "
          << this->theWorkers[i].ProcessPID << ". ";
          logProcessKills << logger::red << pingTimeoutStream.str() << logger::endL;
          this->theWorkers[i].pingMessage="<span style=\"color:red\"><b>" + pingTimeoutStream.str()+"</b></span>";
          numInUse--;
          this->NumProcessAssassinated++;
        }
      }
    }
  if (numInUse<=this->MaxTotalUsedWorkers)
    return;
  for (int i=0; i<this->theWorkers.size && numInUse>1; i++)
  { if (this->theWorkers[i].flagInUse)
    { this->TerminateChildSystemCall(i);
      std::stringstream errorStream;
      errorStream
      << "Terminating child " << i+1 << " with PID "
      << this->theWorkers[i].ProcessPID
      << ": too many workers in use. ";
      this->theWorkers[i].pingMessage=errorStream.str();
      logProcessKills << logger::red  << errorStream.str() << logger::endL;
      numInUse--;
      this->NumProcessAssassinated++;
    }
  }
}

bool WebServer::initPrepareWebServerALL()
{ MacroRegisterFunctionWithName("WebServer::initPrepareWebServerALL");
  this->initPortsITry();
  if (this->flagTryToKillOlderProcesses)
    this->Restart();
  usleep(10000);
  this->initDates();
  if (!this->initBindToPorts())
    return false;
  this->initPrepareSignals();
  this->initListeningSockets();
  return true;
}

bool WebServer::initBindToPorts()
{ MacroRegisterFunctionWithName("WebServer::initBindToPorts");
  addrinfo hints;
  addrinfo *servinfo=0;
  addrinfo *p=0;
  int yes=1;
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  // loop through all the results and bind to the first we can
  List<std::string>* thePorts=&this->PortsITryHttp;
  int* theListeningSocket=0;
  theListeningSocket=& this->listeningSocketHTTP;
  for (int j=0; j<2; j++, thePorts=& this->PortsITryHttpSSL, theListeningSocket=& this->listeningSocketHttpSSL)
    for (int i=0; i<(*thePorts).size; i++)
    { if ((rv = getaddrinfo(NULL, (*thePorts)[i].c_str(), &hints, &servinfo)) != 0)
      { theLog << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
        return false;
      }
      for(p = servinfo; p != NULL; p = p->ai_next)
      { *theListeningSocket= socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (*theListeningSocket == -1)
        { theLog << "Error: socket failed.\n";
          continue;
        }
        if (setsockopt(*theListeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
          crash << "Error: setsockopt failed, error: \n" << strerror(errno) << crash;
        if (bind(*theListeningSocket, p->ai_addr, p->ai_addrlen) == -1)
        { close(*theListeningSocket);
          *theListeningSocket=-1;
          theLog << "Error: bind failed at port: " << (*thePorts)[i] << ". Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          continue;
        }
        fcntl(*theListeningSocket, F_SETFL, O_NONBLOCK);
        break;
      }
      if (p!=NULL)
      { theLog << logger::yellow << "Successfully bound to port " << (*thePorts)[i] << logger::endL;
        if (j==0)
          this->httpPort=(*thePorts)[i];
        else
          this->httpSSLPort=(*thePorts)[i];
        break;
      }
      freeaddrinfo(servinfo); // all done with this structure
    }
  if (this->listeningSocketHTTP == -1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttp.ToStringCommaDelimited() << "\n" << crash;
  if (theGlobalVariables.flagSSLisAvailable && this->listeningSocketHttpSSL==-1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttpSSL.ToStringCommaDelimited() << "\n" << crash;
  return true;
}

void WebServer::initPrepareSignals()
{ MacroRegisterFunctionWithName("WebServer::initPrepareSignals");
  struct sigaction sa;
  sa.sa_sigaction = &segfault_sigaction;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "Was not able to register SIGSEGV handler. Crashing to let you know. " << crash;
  }
  //catch floating point exceptions
  sa.sa_sigaction=0;
  sa.sa_handler= &fperror_sigaction;
  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGFPE, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "Was not able to register SIGFPE handler. Crashing to let you know. " << crash;
  }
  sa.sa_handler=&WebServer::Signal_SIGINT_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
  sa.sa_handler =  &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
//  sa.sa_handler =  0; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_NOCLDWAIT;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "sigaction returned -1" << crash;
  }
//  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_RESTART;
//  if (sigaction(SIGCHLD, &sa, NULL) == -1)
//    theLog << "sigaction returned -1" << logger::endL;
}

extern void MonitorWebServer();

#include <sys/time.h>
timeval timeBeforeProcessFork;

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  theGlobalVariables.RelativePhysicalNameCrashLog="crash_WebServerRun.html";
  theParser.init();
  theParser.ComputeAutoCompleteKeyWords();
  if (true)
  { int pidServer=fork();
    if (pidServer<0)
      crash << "Failed to create server process. " << crash;
    if (pidServer!=0)
    { MonitorWebServer();//<-this attempts to connect to the server over the internet and restarts if it can't.
      return 0;
    }
  }
  CGI::LoadStrings();
  theParser.flagShowCalculatorExamples=false;
  if (!this->initPrepareWebServerALL())
    return 1;
  theLog << logger::purple << "server: waiting for connections...\r\n" << logger::endL;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size = sizeof their_addr;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  this->initSSL();
  fd_set FDListenSockets;
  this->NumSuccessfulSelectsSoFar=0;
  this->NumFailedSelectsSoFar=0;
  long long previousReportedNumberOfSelects=0;
  int previousServerStatReport=0;
  while(true)
  { // main accept() loop
    //    theLog << logger::red << "select returned!" << logger::endL;
    FD_ZERO(&FDListenSockets);
    for (int i=0; i<this->theListeningSockets.size; i++)
      FD_SET(this->theListeningSockets[i], &FDListenSockets);

    while (select(this->highestSocketNumber+1, & FDListenSockets, 0, 0, 0)==-1)
    { if (this->flagReapingChildren)
        logIO << logger::yellow << "Select interrupted while reaping children. "
        << logger::endL;
      else
        logIO << logger::red << "Select failed: possibly due to reaping children. Error message: "
        << strerror(errno) << logger::endL;
      this->NumFailedSelectsSoFar++;
    }
    gettimeofday(&timeBeforeProcessFork, NULL);
    this->NumSuccessfulSelectsSoFar++;
    if ((this->NumSuccessfulSelectsSoFar+this->NumFailedSelectsSoFar)-previousReportedNumberOfSelects>100)
    { logSocketAccept << logger::blue << this->NumSuccessfulSelectsSoFar << " successful and " << this->NumFailedSelectsSoFar << "="
      << this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar << " total selects. " << logger::endL;
      previousReportedNumberOfSelects=this->NumSuccessfulSelectsSoFar+this->NumFailedSelectsSoFar;
    }
    if (this->NumProcessAssassinated+this->NumProcessesReaped+this->NumWorkersNormallyExited+this->NumConnectionsSoFar-previousServerStatReport>99)
    { previousServerStatReport=this->NumProcessAssassinated+this->NumProcessesReaped+this->NumWorkersNormallyExited+this->NumConnectionsSoFar;
      logProcessStats << "# kill commands: " << this->NumProcessAssassinated
      << " #processes reaped: " << this->NumProcessesReaped
      << " #normally reclaimed workers: " << this->NumWorkersNormallyExited
      << " #connections so far: " << this->NumConnectionsSoFar << logger::endL;
    }
    int newConnectedSocket =-1;
    theGlobalVariables.flagUsingSSLinCurrentConnection=false;
    bool found=false;
    for (int i=theListeningSockets.size-1; i>=0; i--)
      if (FD_ISSET(this->theListeningSockets[i], &FDListenSockets))
      { //if (this->theListeningSockets[i]==this->listeningSocketHTTP)
        newConnectedSocket=accept(this->theListeningSockets[i], (struct sockaddr *)&their_addr, &sin_size);
        if (newConnectedSocket>=0)
        { logIO << logger::green << "Connected via listening socket " << this->theListeningSockets[i]
          << " on socket: " << newConnectedSocket;
          if (this->theListeningSockets[i]==this->listeningSocketHttpSSL)
          { theGlobalVariables.flagUsingSSLinCurrentConnection=true;
            logIO << logger::purple << " (SSL encrypted)." << logger::endL;
          } else
            logIO << logger::yellow << " (non-encrypted)." << logger::endL;
          break;
        } else
        { logSocketAccept << logger::red << "This is not supposed to happen: accept failed. Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          found=true;
        }
      }
    if (newConnectedSocket<0 && !found)
      logSocketAccept << logger::red << "This is not supposed to to happen: select succeeded "
      << "but I found no set socket. " << logger::endL;
    if (newConnectedSocket <0)
      continue;
    inet_ntop
    (their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddressBuffer, sizeof userAddressBuffer);
    this->HandleTooManyConnections(userAddressBuffer);
    this->RecycleChildrenIfPossible();
    if (!this->CreateNewActiveWorker())
    { logPlumbing << logger::purple << "Failed to create an active worker. System error string: "
      << strerror(errno) << logger::endL;
      logIO << logger::red << "Failed to create active worker: closing connection. " << logger::endL;
      close (newConnectedSocket);
      continue;
    }
    /////////////
    this->GetActiveWorker().connectedSocketID=newConnectedSocket;
    this->GetActiveWorker().flagUsingSSLInWorkerProcess=theGlobalVariables.flagUsingSSLinCurrentConnection;
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease=newConnectedSocket;
    this->GetActiveWorker().timeServerAtWorkerStart=
    theGlobalVariables.GetElapsedSeconds();
    this->GetActiveWorker().timeOfLastPingServerSideOnly=
    this->GetActiveWorker().timeServerAtWorkerStart;
    this->NumConnectionsSoFar++;
    this->GetActiveWorker().connectionID=this->NumConnectionsSoFar;
    this->GetActiveWorker().userAddress.theObject=userAddressBuffer;
    this->currentlyConnectedAddresses.AddMonomial(this->GetActiveWorker().userAddress, 1 );
//    theLog << this->ToStringStatus();
    /////////////
    this->GetActiveWorker().ProcessPID=fork(); //creates an almost identical copy of this process.
    //The original process is the parent, the almost identical copy is the child.
    //theLog << "\r\nChildPID: " << this->childPID;
    if (this->GetActiveWorker().ProcessPID==-1)
      logProcessKills << logger::red << "FAILED to spawn a child process. " << logger::endL;
    if (this->GetActiveWorker().ProcessPID==0)
    { // this is the child (worker) process
      int result=this->GetActiveWorker().Run();
      this->ReleaseEverything();
      return result;
    }
    this->ReleaseWorkerSideResources();
  }
  this->ReleaseEverything();
  this->SSLfreeEverythingShutdownSSL();
  return 0;
}

int WebWorker::Run()
{ MacroRegisterFunctionWithName("WebWorker::Run");
  this->CheckConsistency();
  this->parent->flagThisIsWorkerProcess=true;
  this->ResetPipesNoAllocation();
  std::stringstream processNameStream;
  processNameStream << "W" << this->indexInParent+1 << ": ";
  PauseProcess::currentProcessName=processNameStream.str();
  //theGlobalVariables.WebServerTimerPing=this->WorkerTimerPing;
  theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
  crash.CleanUpFunction=WebServer::SignalActiveWorkerDoneReleaseEverything;
  InitializeTimer(&timeBeforeProcessFork);
  CreateTimerThread();
  theWebServer.SSLServerSideHandShake();
  if (theGlobalVariables.flagSSLisAvailable && theGlobalVariables.flagUsingSSLinCurrentConnection &&
      !this->parent->flagSSLHandshakeSuccessful)
  { theGlobalVariables.flagUsingSSLinCurrentConnection=false;
    this->parent->SignalActiveWorkerDoneReleaseEverything();
    this->parent->ReleaseEverything();
    logOpenSSL << logger::red << "ssl fail #: " << this->parent->NumConnectionsSoFar << logger::endL;
    return -1;
  }
  if (theGlobalVariables.flagSSLisAvailable && theGlobalVariables.flagUsingSSLinCurrentConnection)
    logOpenSSL << logger::green << "ssl success #: " << this->parent->NumConnectionsSoFar << ". " << logger::endL;
  /////////////////////////////////////////////////////////////////////////
  stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
  int result=0;
  while (true)
  { if (!this->ReceiveAll())
    { this->SetHeadeR("HTTP/1.0 400 Bad request", "Content-type: text/html");
      stOutput << "<html><body><b>HTTP error 400 (bad request). </b> There was an error with the request. "
      << "One possibility is that the input was too large. "
      << "<br>The error message returned was:<br>"
      << this->error
      << " <hr><hr>The message (part) that was received is: "
      << this->ToStringMessageFullUnsafe()
      << "</body></html>";
      this->SendAllAndWrapUp();
      return -1;
    }
    if (this->messageHead.size()==0)
      break;
    result=this->ServeClient();
    this->SendAllBytesWithHeaders();
    if (!this->flagKeepAlive)
      break;
  }
  this->WrapUpConnection();
//  this->SendAllAndWrapUp();
  //theLog << logger::red << "DEBUG: got to here, pt 4" << logger::endL;
  return result;
}

void WebServer::Release(int& theDescriptor)
{ PauseProcess::Release(theDescriptor);
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::AnalyzeMainArguments(int argC, char **argv)
{ MacroRegisterFunctionWithName("WebServer::AnalyzeMainArguments");
  //std::cout << "Content-Type: text/html\r\n\r\n<html><body>DEBUG: ";
  if (argC<0)
    argC=0;
  theGlobalVariables.programArguments.SetSize(argC);
  //std::cout << "RAND_MAX: " << RAND_MAX;
  //std::cout << "Program arguments: \n";
  for (int i=0; i<argC; i++)
  { theGlobalVariables.programArguments[i]=argv[i];
    //std::cout << "Argument " << i+1 << ": " << theGlobalVariables.programArguments[i] << "\n";
  }
  ////////////////////////////////////////////////////
  theGlobalVariables.flagRunningAce=true;
  theGlobalVariables.flagRunningCommandLine=false;
  theGlobalVariables.flagRunningConsoleTest=false;
  theGlobalVariables.flagRunningApache=false;
  #ifdef MACRO_use_open_ssl
  theGlobalVariables.flagSSLisAvailable=true;
  #endif
  theGlobalVariables.flagRunningBuiltInWebServer=false;
  ////////////////////////////////////////////////////
  if (argC==0)
  { theGlobalVariables.flagRunningCommandLine=true;
    return;
  }
  theGlobalVariables.initDefaultFolderAndFileNames(theGlobalVariables.programArguments[0]);
  if (argC<2)
  { theGlobalVariables.flagRunningApache=true;
    return;
  }
  std::string envRequestMethodApache=WebServer::GetEnvironment("REQUEST_METHOD");
  if (envRequestMethodApache =="GET" || envRequestMethodApache=="POST" || envRequestMethodApache=="HEAD")
  { theGlobalVariables.flagRunningApache=true;
    return;
  }
  std::string& secondArgument=theGlobalVariables.programArguments[1];

  if (secondArgument=="server" || secondArgument=="server8080")
  { if (secondArgument=="server8080")
      theWebServer.flagPort8155=false;
    theGlobalVariables.flagRunningBuiltInWebServer=true;
    theGlobalVariables.flagRunningAce=false;
    theWebServer.flagTryToKillOlderProcesses=true;
    if (argC==2)
      return;
    std::string& thirdArgument=theGlobalVariables.programArguments[2];
    std::string timeLimitString="100";
    std::string killOrder="";
    if (thirdArgument=="nokill")
      killOrder="nokill";
    else
      timeLimitString=thirdArgument;
    if (argC>=4 && killOrder=="nokill")
      timeLimitString=theGlobalVariables.programArguments[3];
    theWebServer.flagTryToKillOlderProcesses=!(killOrder=="nokill");
    Rational timeLimit;
    timeLimit.AssignString(timeLimitString);
    int timeLimitInt=0;
    if (timeLimit.IsIntegerFittingInInt(&timeLimitInt))
    { theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=timeLimitInt;
      std::cout << "Max computation time: " << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << std::endl;
    }
    return;
  }
  ////////////////////////////////
  if (secondArgument=="test")
  { theGlobalVariables.flagRunningConsoleTest=true;
    return;
  }
  theGlobalVariables.flagRunningCommandLine=true;
}

void WebServer::InitializeGlobalVariables()
{ theGlobalVariables.MaxComputationTimeBeforeWeTakeAction=5;
  theGlobalVariables.flagReportEverything=true;
  ParallelComputing::cgiLimitRAMuseNumPointersInList=4000000000;
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  folderSubstitutionsNonSensitive=FileOperations::FolderVirtualLinksNonSensitive();
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  folderSubstitutionsSensitive=FileOperations::FolderVirtualLinksSensitive();
  FileOperations::FolderVirtualLinksULTRASensitive(); //<- allocates data structure
  folderSubstitutionsNonSensitive.Clear();
  folderSubstitutionsNonSensitive.SetKeyValue("output/", "output/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("/output/", "output/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("ProblemCollections/", "ProblemCollections/");
  folderSubstitutionsNonSensitive.SetKeyValue("problemtemplates/", "../problemtemplates/");
  folderSubstitutionsNonSensitive.SetKeyValue("freecalc/", "../freecalc/");
  folderSubstitutionsNonSensitive.SetKeyValue("/html/", "../public_html/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("html/", "../public_html/"); //<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("/html-common/", "../public_html/html-common/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("html-common/", "../public_html/html-common/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("/html-common-calculator/", "./html-common/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("html-common-calculator/", "./html-common/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("/font/", "../public_html/html-common/font/");
  folderSubstitutionsNonSensitive.SetKeyValue("/DefaultProblemLocation/", "../problemtemplates/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("DefaultProblemLocation/", "../problemtemplates/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("pagetemplates/", "../pagetemplates/");
  folderSubstitutionsNonSensitive.SetKeyValue("topiclists/", "../topiclists/");
  folderSubstitutionsNonSensitive.SetKeyValue("/MathJax-2.6-latest/", "../public_html/MathJax-2.6-latest/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("MathJax-2.6-latest/", "../public_html/MathJax-2.6-latest/");

  folderSubstitutionsSensitive.Clear();
  folderSubstitutionsSensitive.SetKeyValue("LogFiles/", "LogFiles/");//<-internal use
  folderSubstitutionsSensitive.SetKeyValue("/LogFiles/", "LogFiles/");//<-coming from webserver
  folderSubstitutionsSensitive.SetKeyValue("crashes/", "LogFiles/crashes/");
}

int main(int argc, char **argv)
{ return WebServer::main(argc, argv);
}

extern int mainTest(List<std::string>& remainingArgs);

int WebServer::main(int argc, char **argv)
{ theGlobalVariables.InitThreadsExecutableStart();
  //use of loggers forbidden before calling   theWebServer.AnalyzeMainArguments(...):
  //we need to initialize first the folder locations relative to the executable.
  MacroRegisterFunctionWithName("main");
  try {
  InitializeGlobalObjects();
  theWebServer.AnalyzeMainArguments(argc, argv);
  theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead=
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit+20;
  //using loggers allowed from now on.
  theWebServer.InitializeGlobalVariables();
  theGlobalVariables.flagAceIsAvailable=
  FileOperations::FileExistsVirtual("MathJax-2.6-latest/", false);
  if ( false &&
      theGlobalVariables.flagRunningBuiltInWebServer)
  { theLog
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: theGlobalVariables.flagAllowProcessMonitoring is set to TRUE. " << logger::endL
    << logger::purple << "************************" << logger::endL
    ;
    theGlobalVariables.flagAllowProcessMonitoring=true;
  }
  if (theGlobalVariables.flagRunningBuiltInWebServer)
  { if (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit<=0)
      theLog
      << logger::purple << "************************" << logger::endL
      << logger::red << "WARNING: no computation time limit set. " << logger::endL
      << logger::purple << "************************" << logger::endL
      ;
    if (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit>500)
      theLog
      << logger::purple << "************************" << logger::endL
      << logger::red << "WARNING: computation time limit is high: "
      << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << " seconds. " << logger::endL
      << logger::purple << "************************" << logger::endL
      ;
  }
  if (theGlobalVariables.flagRunningConsoleTest)
    return mainTest(theGlobalVariables.programArguments);
  if (theGlobalVariables.flagRunningApache)
    return WebServer::mainApache();
  if (theGlobalVariables.flagRunningBuiltInWebServer)
    return theWebServer.Run();
  if (theGlobalVariables.flagRunningCommandLine)
    return WebServer::mainCommandLine();
  }
  catch (...)
  { crash << "Exception caught: something very wrong has happened. " << crash;
  }
  crash << "This point of code is not supposed to be reachable. " << crash;
  return -1;
}

int WebServer::mainCommandLine()
{ MacroRegisterFunctionWithName("main_command_input");
  theGlobalVariables.IndicatorStringOutputFunction=CGI::MakeStdCoutReport;
  //  stOutput << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
  //  return 0;
//std::cout << "Running cmd line. \n";
  theParser.init();
  theParser.inputString=theGlobalVariables.programArguments[0];
  theParser.flagUseHtml=false;
  theParser.Evaluate(theParser.inputString);
  std::fstream outputFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (outputFile, "output/outputFileCommandLine.html", false, true, false);
  stOutput << theParser.outputString;
  outputFile << theParser.outputString;
  stOutput << "\nTotal running time: " << GetElapsedTimeInSeconds() << " seconds. \nOutput written in file ./outputFileCommandLine.html\n";
  return 0;
}

std::string WebServer::GetEnvironment(const std::string& envVarName)
{ char* queryStringPtr=getenv(envVarName.c_str());
  if (queryStringPtr==0)
    return "";
  return queryStringPtr;

}

#include <sys/resource.h> //<- for setrlimit(...) function. Restricts the time the executable can run.
int WebServer::mainApache()
{ MacroRegisterFunctionWithName("main_apache");
  rlimit theLimit;
  theLimit.rlim_cur=30;
  theLimit.rlim_max=60;
  setrlimit(RLIMIT_CPU, &theLimit);
  stOutput.theOutputFunction=0;
  //stOutput << "Content-Type: text/html\r\nSet-cookie: test=1;\r\n\r\nThe output bytes start here:\n";
  theGlobalVariables.IndicatorStringOutputFunction=0;
  theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
  theGlobalVariables.flagComputationStarted=true;
//  stOutput << "<hr>First line<hr>";
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=30; //<-30 second computation time restriction!
  theWebServer.initPrepareSignals();
  CreateTimerThread();
  theWebServer.CreateNewActiveWorker();
  WebWorker& theWorker=theWebServer.GetActiveWorker();
  theParser.init();
  std::cin >> theWorker.messageBody;
  theWebServer.httpSSLPort="443";
  theWebServer.httpPort="80";
  std::string theRequestMethod=WebServer::GetEnvironment("REQUEST_METHOD");
  theWorker.cookiesApache=WebServer::GetEnvironment("HTTP_COOKIE");
  std::string thePort=WebServer::GetEnvironment("SERVER_PORT");
  theGlobalVariables.IPAdressCaller= WebServer::GetEnvironment("REMOTE_ADDR");
  theWorker.hostWithPort=WebServer::GetEnvironment("SERVER_NAME")+":"+thePort;
  theGlobalVariables.hostWithPort=theWorker.hostWithPort;
  theGlobalVariables.hostNoPort=theWorker.hostNoPort;
  std::string theURL = WebServer::GetEnvironment("REQUEST_URI");
  unsigned numBytesBeforeQuestionMark=0;
  for (numBytesBeforeQuestionMark=0; numBytesBeforeQuestionMark<theURL.size(); numBytesBeforeQuestionMark++)
    if (theURL[numBytesBeforeQuestionMark]=='?')
      break;
  theGlobalVariables.DisplayNameExecutable=theURL.substr(0, numBytesBeforeQuestionMark);
  theWorker.addressGetOrPost = theGlobalVariables.DisplayNameExecutable+"?"+ WebServer::GetEnvironment("QUERY_STRING");

  if (thePort=="443")
  { theGlobalVariables.flagUsingSSLinCurrentConnection=true;
    theGlobalVariables.flagSSLisAvailable=true;
  }
  if (theRequestMethod=="GET")
    theWorker.requestTypE=theWorker.requestGet;
  if (theRequestMethod=="POST")
    theWorker.requestTypE=theWorker.requestPost;
  theParser.javaScriptDisplayingIndicator=WebWorker::GetJavaScriptIndicatorFromHD();
  theGlobalVariables.flagComputationCompletE=true;
  MathRoutines::StringSplitExcludeDelimiter(theWorker.cookiesApache, ' ', theWorker.cookies);
  for (int i=0; i<theWorker.cookies.size; i++)
    theWorker.cookies[i]=MathRoutines::StringTrimWhiteSpace(theWorker.cookies[i]);
/*  stOutput << "<html><body>";
  stOutput << "DEBUG: your input, bounced back: "
  << "\n<hr>server port: <br>\n" << thePort
  << "\n<hr>Server base: <br>\n" << theGlobalVariables.PhysicalPathProjectBase
  << "\n<hr>Calculator display name apache: <br>\n" << theGlobalVariables.DisplayNameCalculatorApache
  << "\n<hr>Cookies: <br>\n" << theWorker.cookiesApache;
  for (int i=0; i<theWorker.cookies.size; i++)
    stOutput << "<br>\n" << theWorker.cookies[i] << "\n";
  stOutput << "\n<hr>query string:<br>\n" << theWorker.addressGetOrPost
  << "\n<hr>message body:<br>\n" << theWorker.messageBody
  << "\n<hr>request method:<br>\n" << theRequestMethod
  << "\n</body></html>";
  */
  theWorker.ServeClient();
//  std::string theOutputHeader(theWorker.remainingHeaderToSend.TheObjects, theWorker.remainingHeaderToSend.size);
  //std::string theOutputBody(theWorker.remainingBodyToSend.TheObjects, theWorker.remainingBodyToSend.size);
  //stOutput << theOutputBody;
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing=true;
  return 0;
}

std::string HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsHumanReadable");
  if (!theGlobalVariables.UserDebugFlagOn() )
    return "";
  std::stringstream out;
  out << "<hr>\n";
  out << "Default user: " << theGlobalVariables.userDefault.username.value;
  if (theGlobalVariables.flagLoggedIn)
    out << "\n<br>\nLogged in.";
  out << "\n<br>\nAddress:\n" << CGI::StringToHtmlString(theWebServer.GetActiveWorker().addressComputed, true);
  out << "\n<br>\nRequest:\n" << theGlobalVariables.userCalculatorRequestType;
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out << "\n<br>\n<b>User has admin rights</b>";
  if (theWebServer.RequiresLogin(theGlobalVariables.userCalculatorRequestType, theWebServer.GetActiveWorker().addressComputed))
    out << "\n<br>\nAddress requires login. ";
  else
    out << "\n<br>\nAddress <b>does not</b> require any login. ";
  out << "\n<hr>\n";
  for (int i=0; i<theGlobalVariables.webArguments.size(); i++)
  { out << theGlobalVariables.webArguments.theKeys[i] << ": "
    << CGI::StringToHtmlString(theGlobalVariables.webArguments[i], true);
    if (i!=theGlobalVariables.webArguments.size()-1)
      out << "\n<br>\n";
  }
  out << "\n<hr>\n";
  if (theGlobalVariables.flagRunningBuiltInWebServer)
    out << theWebServer.GetActiveWorker().ToStringMessageUnsafe();
  return out.str();
}

void WebWorker::PrepareFullMessageHeaderAndFooter()
{ MacroRegisterFunctionWithName("WebWorker::PrepareFullMessageHeaderAndFooter");
  this->remainingBytesToSenD.SetSize(0);
  this->remainingBytesToSenD.SetExpectedSize(this->remainingBodyToSend.size+this->remainingHeaderToSend.size+30);
  if (this->remainingHeaderToSend.size==0)
  { if (this->requestTypE!=this->requestHead)
      this->remainingBytesToSenD=this->remainingBodyToSend;
    this->remainingBodyToSend.SetSize(0);
    return;
  }
  this->remainingBytesToSenD=this->remainingHeaderToSend;
  this->remainingHeaderToSend.SetSize(0);
  std::stringstream contentLengthStream;
//  double fixme;
//  if (false)
  if (this->flagDoAddContentLength)
    contentLengthStream << "Content-Length: " << this->remainingBodyToSend.size << "\r\n";
  contentLengthStream << "\r\n";
  std::string contentLengthString=contentLengthStream.str();
  for (unsigned i=0; i<contentLengthString.size(); i++)
    this->remainingBytesToSenD.AddOnTop(contentLengthString[i]);
  //std::string debugString(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
  //std::cout << "DEBUG: headers+ body="
  //<< this->remainingBytesToSenD.size << " + "
  //<< this->remainingBodyToSend.size << "\n" << debugString;
  if (this->requestTypE!=this->requestHead)
    this->remainingBytesToSenD.AddListOnTop(this->remainingBodyToSend);
  this->remainingBodyToSend.SetSize(0);
}

void WebWorker::SendAllBytesWithHeaders()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesWithHeaders");
//  static bool calledOnce=false;
//  if (calledOnce)
//  { crash << "WebWorker::SendAllBytesNoHeaders called more than once" << crash;
//  }
//  calledOnce=true;
  this->PrepareFullMessageHeaderAndFooter();
  //std::string tempS(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
  //theLog << logger::red << "DEBUG Message:\n" << logger::normalColor << tempS << logger::endL;
  this->SendAllBytesNoHeaders();
}

void WebWorker::SendAllBytesHttp()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttp");
  if (this->remainingBytesToSenD.size==0)
    return;
  this->CheckConsistency();
//  firstCallStream << "WebWorker::SendAllBytesHttp called once. Input: \n "
//  << this->ToStringMessageFullUnsafe() << "\nStack trace:\n"
//  << crash.GetStackTraceEtcErrorMessage() ;
//  if (this->flagDidSendAll)
//  { static std::stringstream firstCallStream;
//    firstCallStream
//    << "\n<br>\n"
//    << "WebWorker::SendAllBytesHttp called more than once. "
//    << "Input: \n "
//    << this->ToStringMessageFullUnsafe() << "\nStack trace:\n"
//    << crash.GetStackTraceEtcErrorMessage()
//    << "\n<hr>Sending back:<hr>\n";
//
//    std::string theBytes(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
//    ;
//    firstCallStream << theBytes;
//    logHttpErrors << firstCallStream.str() << logger::endL;
//  }
  this->flagDidSendAll=true;
  //std::string tempS(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
  //theLog << logger::green << "Sending: " << tempS << logger::endL;
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    return;
  }
  theLog << "Sending " << this->remainingBytesToSenD.size << " bytes in chunks of: ";
  theLog.flush();
  double startTime=theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5; // 5 Secs Timeout
  tv.tv_usec = 0; // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending=0;
  int timeOutInSeconds =20;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO,(void*)(&tv), sizeof(timeval));
  while (this->remainingBytesToSenD.size>0)
  { if (theGlobalVariables.GetElapsedSeconds()-startTime>timeOutInSeconds)
    { theLog << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent=send
    (this->connectedSocketID, &this->remainingBytesToSenD[0], this->remainingBytesToSenD.size,0);
    if (numBytesSent<0)
    { if (errno==EAGAIN || errno ==EWOULDBLOCK || errno== EINTR || errno==EIO )
      logIO << "WebWorker::SendAllBytes failed. Error: " << this->parent->ToStringLastErrorDescription() << logger::endL;
      return;
    }
    if (numBytesSent==0)
      numTimesRunWithoutSending++;
    else
      numTimesRunWithoutSending=0;
    theLog << numBytesSent;
    this->remainingBytesToSenD.Slice(numBytesSent, this->remainingBytesToSenD.size-numBytesSent);
    if (this->remainingBytesToSenD.size>0)
      theLog << ", ";
    if (numTimesRunWithoutSending>3)
    { theLog << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
    theLog << logger::endL;
  }
  theLog << "All bytes sent. " << logger::endL;
}

void WebWorker::QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueBytesForSending");
  (void) MustSendAll;
  this->remainingBytesToSenD.AddListOnTop(bytesToSend);
//  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
//    this->SendAllBytes();
}

void WebWorker::QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingWithHeadeR");
  (void) MustSendAll;
  int oldSize=this->remainingBodyToSend.size;
  this->remainingBodyToSend.SetSize(this->remainingBodyToSend.size+stringToSend.size());
  for (unsigned i=0; i<stringToSend.size(); i++)
    this->remainingBodyToSend[i+oldSize]=stringToSend[i];
}

void WebWorker::QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingNoHeadeR");
  (void) MustSendAll;
  int oldSize=this->remainingBytesToSenD.size;
  this->remainingBytesToSenD.SetSize(this->remainingBytesToSenD.size+stringToSend.size());
  for (unsigned i=0; i<stringToSend.size(); i++)
    this->remainingBytesToSenD[i+oldSize]=stringToSend[i];
}

void WebWorker::SendAllBytesNoHeaders()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesNoHeaders");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  { this->SendAllBytesHttpSSL();
    return;
  }
  this->SendAllBytesHttp();
}
