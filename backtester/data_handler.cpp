#include <data_handler.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <fstream>
#include <sstream>

HistoricCSVDataHandler::HistoricCSVDataHandler(std::queue<std::shared_ptr<Event>>& events,
  std::string csvDir, std::vector<std::string> symbolList)
  : events(events), csvDir(csvDir), symbolList(symbolList), contBacktest(true) {

    // Initialise latestSymbolData map for each symbol
    for (const auto& s : symbolList) {
      latestSymbolData[s] = {};
      barIndex[s] = 0;
    }

    openConvertCSVFiles();
  }

  std::vector<Bar> HistoricCSVDataHandler::getLatestBars(std::string symbol, int N) {
    // Check if symbol exists
    if (latestSymbolData.find(symbol) == latestSymbolData.end()) {
      std::cerr << "Symbol not available" << std::endl;
    }

    const auto& bars = latestSymbolData[symbol];

    // If we have fewer than N bars, return N
    if (bars.size() < static_cast<size_t>(N)) {
      return bars;
    }

    return std::vector<Bar> (bars.end() - N, bars.end());
  }

  