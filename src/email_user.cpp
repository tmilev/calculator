#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "html_routines.h"
#include "user.h"

std::string EmailRoutines::webAdress = "";
std::string EmailRoutines::sendEmailFrom = "";
std::string EmailRoutines::mailApiKeyFileName = "certificates/mailgun-api.txt";
List<bool> EmailRoutines::recognizedEmailCharacters;

bool EmailRoutines::sendEmailWithMailGun(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("EmailRoutines::sendEmailWithMailGun");
  if (this->sendEmailFrom == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Emails have not been setup on this instance of the calculator. "
      << "The website admin needs to set field: "
      << "sendEmailFrom inside file configuration/configuration.json. ";
    }
    return false;
  }
  std::string mailGunKey;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      EmailRoutines::mailApiKeyFileName,
      mailGunKey,
      true,
      true,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Could not find mailgun key. "
      << "The key must be located in file: \n"
      << "[certificates/mailgun-api.txt]. \n"
      << "The file must be uploaded manually to the server. ";
    }
    return false;
  }
  mailGunKey = StringRoutines::stringTrimWhiteSpace(mailGunKey);
  global
  << "Sending email via "
  << "https://api.mailgun.net/v3/"
  << EmailRoutines::sendEmailFrom
  << "/messages "
  << Logger::endL;
  std::stringstream commandToExecute;
  commandToExecute << "curl -s --user 'api:" << mailGunKey << "' ";
  commandToExecute
  << "https://api.mailgun.net/v3/"
  << EmailRoutines::sendEmailFrom
  << "/messages ";
  commandToExecute
  << "-F from='Automated Email "
  << "<noreply@"
  << EmailRoutines::sendEmailFrom
  << ">' ";
  if (this->toEmail == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Receiver address is missing. ";
    }
    return false;
  }
  if (this->subject == "" && this->emailContent == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty emails not allowed. ";
    }
    return false;
  }
  // WARNING: calling command line
  // that includes user inputs.
  // Those MUST be sanitized,
  // else an attacker would use username with "
  // characters
  // to execute arbitrary code.
  commandToExecute
  << "-F to=\""
  << HtmlRoutines::convertStringEscapeQuotesAndBackslashes(this->toEmail)
  << "\" "
  << "-F subject=\""
  << HtmlRoutines::convertStringEscapeQuotesAndBackslashes(this->subject)
  << "\" "
  << "-F text=\""
  << HtmlRoutines::convertStringEscapeQuotesAndBackslashes(
    this->emailContent
  )
  << "\"";
  if (global.flagDebugLogin) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "<b style='color:red'>Since this is a login debug, "
      << "I am not sending your activation email.</b> "
      << "To actually (try to) send the email, simply copy+paste "
      << "the command in your console. ";
      global.comments
      << "<br><b>Email command:</b><br>"
      << HtmlRoutines::convertStringToHtmlString(
        commandToExecute.str(), false
      )
      << "<br>";
    }
    return true;
  }
  std::string commandResult =
  global.externalCommandReturnOutput(commandToExecute.str());
  global << commandResult << Logger::endL;
  bool success = commandResult.find("Forbidden") == std::string::npos;
  if (!success && commentsOnFailure != nullptr) {
    int start =
    MathRoutines::maximum(
      0, static_cast<int>(mailGunKey.size() - 17)
    );
    std::string apiKey = mailGunKey.substr(start);
    *commentsOnFailure
    << "Failed to send your email from our automated account: "
    << EmailRoutines::sendEmailFrom
    << ". This is an error in our web site's email setup. "
    << "If you can, please take a screenshot and file us a bug report! "
    << "Mailgun api key: ..."
    << apiKey
    << ". ";
  }
  if (commentsGeneralSensitive != nullptr) {
    *commentsGeneralSensitive << "Result: ";
    *commentsGeneralSensitive
    << HtmlRoutines::convertStringToHtmlString(commandResult, true);
  }
  return success;
}

List<bool>& EmailRoutines::getRecognizedEmailChars() {
  if (recognizedEmailCharacters.size == 0) {
    recognizedEmailCharacters.initializeFillInObject(256, false);
    std::string characters =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    characters += "0123456789";
    characters += "@";
    characters += "!#$%&'*+-/=?.";
    for (unsigned i = 0; i < characters.size(); i ++) {
      recognizedEmailCharacters[
        static_cast<unsigned char>(characters[i])
      ] =
      true;
    }
  }
  return EmailRoutines::recognizedEmailCharacters;
}

bool EmailRoutines::isOKEmail(
  const std::string& input, std::stringstream* commentsOnError
) {
  STACK_TRACE("EmailRoutines::isOKEmail");
  if (input.size() == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Empty email not allowed. ";
    }
    return false;
  }
  int totalAts = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (
      !EmailRoutines::getRecognizedEmailChars()[
        static_cast<unsigned char>(input[i])
      ]
    ) {
      if (commentsOnError != nullptr) {
        *commentsOnError
        << "Email: "
        << input
        << " contains the unrecognized character "
        << input[i]
        << ". ";
      }
      return false;
    }
    if (input[i] == '@') {
      totalAts ++;
    }
  }
  if (totalAts != 1 && commentsOnError != nullptr) {
    *commentsOnError
    << "Email: "
    << input
    << " is required to have exactly one @ character. ";
  }
  return totalAts == 1;
}

EmailRoutines::EmailRoutines() {
  this->smtpWithPort = "smtp.gmail.com:587";
}
