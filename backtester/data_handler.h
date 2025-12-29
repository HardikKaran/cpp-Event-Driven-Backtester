#pragma once

#include <vector>
#include <map>
#include <string>
#include <queue>
#include <memory>
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
  /*
  The goal of a (derived) DataHandler object is to output a generated
  set of bars (OLHCVI) for each symbol requested. 

  This will replicate how a live strategy would function as current
  market data would be sent "down the pipe". Thus a historic and live
  system will be treated identically by the rest of the backtesting suite.
  */

  public:
    virtual ~DataHandler() = default;

    /*
    Returns the last N bars from the latest_symbol list,
    or fewer if less bars are available.
    */
    virtual std::vector<Bar> getLatestBars(std::string symbol, int N = 1) = 0;
    
    /*
    Pushes the latest bar to the latest symbol structure
    for all symbols in the symbol list.
    */
    virtual void updateBars() = 0;
};

class HistoricCSVDataHandler : public DataHandler {
  public:
    HistoricCSVDataHandler(std::queue<std::shared_ptr<Event>>& events, 
      std::string csvDir, std::vector<std::string> symbolList);

    std::vector<Bar> getLatestBars(std::string symbol, int N = 1) override;

    void updateBars() override;

  private: 
    std::queue<std::shared_ptr<Event>>& events;
    std::string csvDir;
    std::vector<std::string> symbolList;
    std::map<std::string, std::vector<Bar>> symbolData;
    std::map<std::string, std::vector<Bar>> latestSymbolData;
    bool contBacktest = true;

    void openConvertCSVFiles();

    std::map<std::string, int> barIndex; // Tracks where in list
    bool getNextBar(const std::string& symbol, Bar& outBar); // Get next bar for specific symbol
};