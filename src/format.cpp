#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
  // reference: https://github.com/hishamhm/htop/blob/59166da773d8a9a97c8f1b087346865bc8cda9fa/UptimeMeter.c#L44
  int hour;
  int minutes;
  int second;
  char result[100];
  
  hour = (seconds / 3600) % 24;
  minutes = (seconds / 60) % 60;
  second = seconds % 60;
  sprintf(result, "%02d:%02d:%02d", hour, minutes, second);
  
  return result;
}