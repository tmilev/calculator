 // The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_general_time_date_ALREADY_INCLUDED
#define header_general_time_date_ALREADY_INCLUDED

#include "macros.h"
#include <ctime>

class TimeWrapper {
public:
  tm timeGM;
  tm timeLocal;
  std::string timeStringNonReadable;
  void assignLocalTime();
  void computeTimeStringNonReadable();
  std::string toString() const;
  static std::string toStringYMDHMS(const tm* input);
  std::string toStringGM() const;
  std::string toStringLocal() const;
  void operator=(const std::string& inputTime);
  double subtractAnotherTimeFromMeInSeconds(TimeWrapper& other);
  double subtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(
    TimeWrapper& other
  );
  bool assignMonthDayYear(
    const std::string& input, std::stringstream& commentsOnFailure
  );
  static std::string toStringSecondsToDaysHoursSecondsString(
    double input, bool includeSeconds, bool beShort
  );
  TimeWrapper();
};

#endif // header_general_time_date_ALREADY_INCLUDED
