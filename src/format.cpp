#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

// INPUT: Long int measuring seconds
// OUTPUT: HHHH:MM:SS
std::string Format::ElapsedTime(long seconds) {
  int HH;
  int MM;
  int SS;

  MM = seconds / 60;
  SS = seconds % 60;

  HH = MM / 60;
  MM = MM % 60;

  // To format to HHHH:MM:SS
  std::ostringstream formattedTime;
  formattedTime << std::setfill('0') << std::setw(4) << std::to_string(HH)
                << ":" << std::setfill('0') << std::setw(2)
                << std::to_string(MM) << ":" << std::setfill('0')
                << std::setw(2) << std::to_string(SS);

  return formattedTime.str();
}