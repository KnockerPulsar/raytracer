#pragma once
#include <string>

using std::string;

string ProgStr(int currVal, int maxVal) {
  return "[ " + std::to_string(currVal) + " / " + std::to_string(maxVal) + " ]";
}

string ProgBar(int currVal, int minVal, int maxVal, int maxChars) {
  float percentage = (float)currVal / (maxVal - minVal);
  int   chars      = (int)(percentage * maxChars);
  return string(chars, 'X');
}