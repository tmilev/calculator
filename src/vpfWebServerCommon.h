//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfWebServerCommon_h_already_included
#define vpfWebServerCommon_h_already_included
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
ProjectInformationInstance projectInfoInstanceWebServerCommonImplementation(__FILE__, "Web server common code implementation.");

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
  && input!="changePasswordPage";
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
       theGlobalVariables.userCalculatorRequestType!="compute")
  { if (theGlobalVariables.GetWebInput("fileName")!="")
      out << "  addCookie(\"fileName\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"))
      << "\", 100, false);\n";
    if (theGlobalVariables.GetWebInput("currentExamHome")!="")
      out << "  addCookie(\"currentExamHome\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"))
      << "\", 100, false);\n";
  }
  out
  << "}\n";
  out
  << "function storeSettingsSecurity(){\n";
  if (theGlobalVariables.flagLoggedIn)
  { out << "   addCookie(\"authenticationToken\", \""
    << theWebServer.GetActiveWorker().authenticationToken << "\", 150, true);"
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
  << "      if(getCookie(\"request\")!='logout' && getCookie(\"request\")!='login' )\n"
  << "        document.getElementById(\"request\").value=getCookie(\"request\");\n"
  << "  if (document.getElementById(\"fileName\")!=null)\n "
  << "    if(getCookie(\"fileName\")!='')\n"
  << "      document.getElementById(\"fileName\").value=getCookie(\"fileName\");\n"
  << "  if (document.getElementById(\"currentExamHome\")!=null)\n "
  << "    if(getCookie(\"currentExamHome\")!='')\n"
  << "      document.getElementById(\"currentExamHome\").value=getCookie(\"currentExamHome\");\n"
  << "  if (document.getElementById(\"authenticationToken\")!=null)\n"
  << "    if(getCookie(\"authenticationToken\")!='')\n"
  << "      document.getElementById(\"authenticationToken\").value=getCookie(\"authenticationToken\");\n ";
  if (!theGlobalVariables.flagRunningAceWebserver)
    out
    << "  if (document.getElementById(\"usernameHidden\")!=null)\n"
    << "    if(getCookie(\"username\")!='')\n"
    << "      document.getElementById(\"usernameHidden\").value=getCookie(\"username\");\n ";
  else
    out
    << "  if (document.getElementById(\"username\")!=null)\n"
    << "    if(getCookie(\"username\")!='')\n"
    << "      document.getElementById(\"username\").value=getCookie(\"username\");\n ";
  out << "}\n";
  out << " </script>\n";
  return out.str();
}

std::string WebWorker::GetJavaScriptIndicatorBuiltInServer(int inputIndex, bool callProgressReport)
{ MacroRegisterFunctionWithName("WebWorker::GetJavaScriptIndicatorBuiltInServer");
  std::stringstream out;
  out << " <!>\n";
//  out << "\n<br>\n<button onclick=\"progressReport()\">Manual report</button>";
  out << "\n<br>\n<button id=\"idButtonSendTogglePauseRequest\" onclick=\"SendTogglePauseRequest()\">Pause</button>";
  out << "<span id=\"idPauseToggleServerResponse\"></span>\n";
  out << "<span id=\"idProgressReportTimer\"></span>\n";
  out << "\n<br>\n<div id=\"idProgressReport\"></div>\n";
  out << "\n<script type=\"text/javascript\" id=\"progressReportJavascript\"> \n";
  out << "var isPaused=false;\n";
  out << "var isFinished=false;\n";
  out << "var timeIncrementInTenthsOfSecond=4;//measured in tenths of a second\n";
  out << "var timeOutCounter=0;//measured in tenths of a second\n";
  out << "\nfunction progressReport()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var progReport = document.getElementById(\"idProgressReport\");	\n";
  out << "  var progReportTimer = document.getElementById(\"idProgressReportTimer\");	\n";
  out << "  if(isPaused)\n";
  out << "    return;\n";
  out << "  progReportTimer.innerHTML =\"<hr>Refreshing every \"+timeIncrementInTenthsOfSecond/10+\" second(s). Client time: ~\"+ Math.floor(timeOutCounter/10)+\" second(s)<br>\";\n";
  out << "  progReportTimer.style.display = '';\n";
  out << "  progReport.style.display = '';\n";
  out << "  timeOutCounter+=timeIncrementInTenthsOfSecond;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  if (inputIndex==-1)
    theLog << logger::red << "Worker index in parent is -1!!!" << logger::endL;
  else
    theLog << "Worker index: " << inputIndex << logger::endL;
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameExecutableWithPath << "?request=indicator&mainInput="
  << inputIndex+1 << "\";\n";
  out << "  oRequest.open(\"GET\",sURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  oRequest.send(null)\n";
  out << "  if (oRequest.status==200)\n";
  out << "  { newReportString= oRequest.responseText;\n";
  out << "    if (oRequest.responseText==\"finished\")\n";
  out << "    { isFinished=true;\n";
  out << "      document.getElementById(\"idPauseToggleServerResponse\").innerHTML=\"Computation finished.\";\n";
  out << "      return;\n";
  out << "    }\n";
  out << "    if (oRequest.responseText!=\"\")\n";
  out << "      progReport.innerHTML=newReportString+\"<hr>\";\n";
  out << "  }\n";
  out << "   window.setTimeout(\"progressReport()\",timeIncrementInTenthsOfSecond*100);\n";
  out << " }\n";
  out << "function SendTogglePauseRequest()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var pauseRequest = new XMLHttpRequest();\n";
  theLog << "Generating indicator address for worker number " << inputIndex+1 << "." << logger::endL;
  out << "  pauseURL  = \"" << theGlobalVariables.DisplayNameExecutableWithPath
  << "?request=pause&mainInput=" << inputIndex+1 << "\";\n";
  out << "  pauseRequest.open(\"GET\",pauseURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  pauseRequest.send(null)\n";
  out << "  if (pauseRequest.status!=200)\n";
  out << "    return;\n";
  out << "  if(pauseRequest.responseText==\"paused\")\n";
  out << "    isPaused=true;\n";
  out << "  if(pauseRequest.responseText==\"unpaused\")\n";
  out << "    isPaused=false;\n";
  out << "  if (isPaused)\n";
  out << "    document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Continue\";\n";
  out << "  else\n";
  out << "    document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Pause\";\n";
  out << "  document.getElementById(\"idPauseToggleServerResponse\").innerHTML=pauseRequest.responseText;\n";
  out << "  if (!isPaused)\n";
  out << "    progressReport();\n";
  out << "}\n";
//  out << " progressReport();";
//  out << " var newReportString=\"\";\n";
  if (callProgressReport)
    out << "progressReport();\n";
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
//from where you get the signedFileCertificate1 andsignedFileCertificate3
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

  FileOperations::GetPhysicalFileNameFromVirtual
  (signedFileCertificate1, singedFileCertificate1Physical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (signedFileCertificate3, signedFileCertificate3Physical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (fileCertificate, fileCertificatePhysical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (fileKey, fileKeyPhysical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (signedFileKey, signedFileKeyPhysical);
  //std::cout << "\n\nproject base: " << theGlobalVariables.PhysicalPathProjectBase;
  //std::cout << "\n\nfileKey physical: " << fileKeyPhysical;
  //////////////////////////////////////////////////////////
  if (!FileOperations::FileExistsVirtual(fileCertificate) ||
      !FileOperations::FileExistsVirtual(fileKey))
  { theLog << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
    theGlobalVariables.flagSSLisAvailable=false;
    return;
  }
  theLog << logger::green << "SSL is available." << logger::endL;
  const SSL_METHOD *meth;
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  meth = TLSv1_2_server_method();
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

#endif
