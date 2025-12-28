#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <utility>

#include <event.h>

struct Bar {
  std::string symbol;
  std::chrono::year_month_day date;
  double open;
  double high;
  double low;
  double close;
  double vol;
};

class DataHandler {
  public:
    virtual ~DataHandler() = default;

    virtual std::vector<Bar> getLatestBars(std::string symbol, int N = 1) = 0;
    
    virtual void updateBars() = 0;
};