// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_time_date.h"
#include "general_lists.h"
#include "math_general.h"

TimeWrapper::TimeWrapper() {
  this->timeGM.tm_gmtoff = 0;
  this->timeGM.tm_hour = 0;
  this->timeGM.tm_isdst = 0;
  this->timeGM.tm_mday = 0;
  this->timeGM.tm_min = 0;
  this->timeGM.tm_mon = 0;
  this->timeGM.tm_sec = 0;
  this->timeGM.tm_wday = 0;
  this->timeGM.tm_yday = 0;
  this->timeGM.tm_year = 0;
  this->timeGM.tm_zone = nullptr;

  this->timeLocal.tm_gmtoff = 0;
  this->timeLocal.tm_hour = 0;
  this->timeLocal.tm_isdst = 0;
  this->timeLocal.tm_mday = 0;
  this->timeLocal.tm_min = 0;
  this->timeLocal.tm_mon = 0;
  this->timeLocal.tm_sec = 0;
  this->timeLocal.tm_wday = 0;
  this->timeLocal.tm_yday = 0;
  this->timeLocal.tm_year = 0;
  this->timeLocal.tm_zone = nullptr;
}

bool TimeWrapper::assignMonthDayYear(const std::string& input, std::stringstream& commentsOnFailure) {
  this->assignLocalTime(); //<-hopefully this initialized time properly ...
  List<char> theDelimiters;
  theDelimiters.addOnTop('/');
  theDelimiters.addOnTop('-');
  theDelimiters.addOnTop('.');
  List<std::string> output;
  StringRoutines::stringSplitExcludeDelimiters(input, theDelimiters, output);
  if (output.size < 3) {
    commentsOnFailure << "Failed to extract a M/D/Y date from: " << input;
    return false;
  }
  int month = atoi(output[0].c_str());
  int day = atoi(output[1].c_str());
  int year = atoi(output[2].c_str());
  this->timeGM.tm_sec = 59;
  this->timeGM.tm_min = 59;
  this->timeGM.tm_hour = 23;
  this->timeGM.tm_mday = day;
  this->timeGM.tm_mon = month - 1;
  this->timeGM.tm_year = year - 1900;
  mktime(&this->timeGM);
  return true;
}

void TimeWrapper::computeTimeStringNonReadable() {
  std::stringstream out;
  out << this->timeGM.tm_year << "-" << this->timeGM.tm_mon << "-"
  << this->timeGM.tm_mday
  << "-" << this->timeGM.tm_hour << "-"
  << this->timeGM.tm_min
  << "-"
  << this->timeGM.tm_sec;
  this->theTimeStringNonReadable = out.str();
}

void TimeWrapper::assignLocalTime() {
  std::time_t rawtime;
  time(&rawtime);
  this->timeGM = *std::gmtime(&rawtime);
  this->timeLocal = *localtime(&rawtime);
  mktime(&this->timeGM);
  mktime(&this->timeLocal);
  this->computeTimeStringNonReadable();
}

std::string TimeWrapper::toStringSecondsToDaysHoursSecondsString(double input, bool includeSeconds, bool beShort) {
  std::stringstream out;
  out.precision(2);
  bool isPositive = (input > 0);
  if (!isPositive) {
    input *= - 1;
  }
  int days = static_cast<int>(FloatingPoint::Floor(input / (24 * 3600)));
  if (beShort && days > 0) {
    double daysfloat = input / (24 * 3600);
    out << "~" << daysfloat << " day(s)";
    return out.str();
  }
  input -= days * 24 * 3600;
  if (!isPositive) {
    out << "-(";
  }
  out << "~";
  if (days > 0) {
    out << days << " day(s) ";
  }
  if (input > 0) {
    out << std::fixed << input / 3600 << " hour(s)";
  }
  if (includeSeconds && !beShort) {
    out << std::fixed << static_cast<int>(static_cast<int>(input) / 60)
    << " minute(s) " << static_cast<int>(static_cast<int>(input) % 60) << " second(s).";
  }
  if (!isPositive) {
    out << ")";
  }
  return out.str();
}

double TimeWrapper::subtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(TimeWrapper& other) {
  return this->subtractAnotherTimeFromMeInSeconds(other) / 3600;
}

double TimeWrapper::subtractAnotherTimeFromMeInSeconds(TimeWrapper& other) {
  return difftime(mktime(&this->timeGM), mktime(&other.timeGM));
}

std::string TimeWrapper::toStringYMDHMS(const tm* input) {
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d-%I:%M:%S", input);
  return std::string(buffer);
}

std::string TimeWrapper::toStringGM() const {
  return this->toStringYMDHMS(&this->timeGM);
}

std::string TimeWrapper::toStringLocal() const {
  return this->toStringYMDHMS(&this->timeLocal);
}

std::string TimeWrapper::toString() const {
  std::stringstream out;
  out
  <<        this->timeGM.tm_year
  << " " << this->timeGM.tm_mon
  << " " << this->timeGM.tm_mday
  << " " << this->timeGM.tm_hour
  << " " << this->timeGM.tm_min
  << " " << this->timeGM.tm_sec;
  return out.str();
}

void TimeWrapper::operator=(const std::string& input) {
  std::string inputNoDashes;
  inputNoDashes.reserve(input.size());
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (input[i] != '-') {
      inputNoDashes.push_back(input[i]);
    } else {
      inputNoDashes.push_back(' ');
    }
  }
  std::stringstream inputStream;
  inputStream << inputNoDashes;
  inputStream.seekg(0);
  inputStream >> this->timeGM.tm_year;
  inputStream >> this->timeGM.tm_mon;
  inputStream >> this->timeGM.tm_mday;
  inputStream >> this->timeGM.tm_hour;
  inputStream >> this->timeGM.tm_min;
  inputStream >> this->timeGM.tm_sec;
  this->computeTimeStringNonReadable();
}
