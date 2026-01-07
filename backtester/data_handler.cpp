#include <data_handler.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <fstream>
#include <sstream>

HistoricCSVDataHandler::HistoricCSVDataHandler(std::queue<std::shared_ptr<Event>> &events,
                                               std::string csvDir, std::vector<std::string> symbolList)
    : events(events), csvDir(csvDir), symbolList(symbolList), contBacktest(true) {
    // Initialise latestSymbolData map for each symbol
    for (const auto &s : symbolList) {
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

    const auto &bars = latestSymbolData[symbol];

    // If we have fewer than N bars, return N
    if (bars.size() < static_cast<size_t>(N)) {
        return bars;
    }

    return std::vector<Bar>(bars.end() - N, bars.end());
}

void HistoricCSVDataHandler::updateBars() {
    bool newBarAdd = false;

    for (const auto& s : symbolList) {
        // Check if end of data reached for symbol s
        if (barIndex[s] >= symbolData[s].size()) {
            continue;
        }

        // Get next bar from symbolData
        Bar bar = symbolData[s][barIndex[s]];

        // Push bar to live simulation
        latestSymbolData[s].push_back(bar);

        barIndex[s]++;
        newBarAdd = true;
    }

    // If at least one symbol had a new bar, push a MarketEvent
    if (newBarAdd) {
        events.push(std::shared_ptr<MarketEvent>());
    }
    else {
        contBacktest = false; // No more data left
    }
}

void HistoricCSVDataHandler::openConvertCSVFiles() {
    // Temp storage: symbol -> (date -> bar)
    std::map<std::string, std::map<time_t, Bar>> rawDataStore;

    // Read files + build master index
    for (const auto& s : symbolList) {
        std::string filePath = csvDir + "/" + s + ".csv";
        std::ifstream file(filePath);

        if (!file.is_open()) {
            std::cerr << "Error opening file" << std::endl;
            continue;
        }

        std::string line;
        std::getline(file, line); // Skip header line

        while (std::getline(file, line)) {
            Bar bar;
            if (parseCSVLine(line, s, bar)) {
                rawDataStore[s][bar.date] = bar;
                timeIndexSet.insert(bar.date); // Add unique date to set
            }
        }
        file.close();
    }

    // Convert to dataframe like pandas
    for (const auto& symbol : symbolList) {
        alignAndPadData(symbol, rawDataStore[symbol]);
    }
}

bool HistoricCSVDataHandler::parseCSVLine(const std::string& line, const std::string& symbol, Bar& outBar) {
    std::stringstream ss(line);
    std::string segment;
    std::vector<std::string> row;

    // Add row values to vector from a row in the csv
    while (std::getline(ss, segment, ',')) {
        row.push_back(segment);
    }
    
}