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
    for (const auto& s : symbolList) {
      latestSymbolData[s] = {};
      barIndex[s] = 0;
    }

    openConvertCSVFiles();
  }