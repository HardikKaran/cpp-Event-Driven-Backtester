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

    // Expecting: Date, Open, High, Low, Close, Adj Close, Volume
    if (row.size() < 7) {
        return false;
    }

    try {
        outBar.symbol = symbol;

        // Parse Date (YYYY-MM-DD)
        std::tm tm {};
        std::stringstream dateSS(row[0]);
        dateSS >> std::get_time(&tm, "%Y-%m-%d");
        outBar.date = std::mktime(&tm);

        // Parse Bar values
        outBar.open     = std::stod(row[1]);
        outBar.high     = std::stod(row[2]);
        outBar.low      = std::stod(row[3]);
        outBar.close    = std::stod(row[4]);
        outBar.adjClose = std::stod(row[5]);
        outBar.vol      = std::stol(row[6]);
        outBar.returns  = 0.0; // Calculated later

        return true;

    } catch (...) { // Forwars the unknown number of arguements 
        return false;
    }
}

void HistoricCSVDataHandler::alignAndPadData(const std::string& symbol, std::map<time_t, Bar>& rawTempData) {
    std::vector<Bar>& finalBars = symbolData[symbol];
    Bar previousBar = {};
    bool firstBarFound = false;

    // Iterate through the set of all dates in masterTimeIndex
    for (time_t date : timeIndexSet) {
        Bar currentBar;

        // Case A: Data exists for this date
        if (rawTempData.count(date)) {
            currentBar = rawTempData[date];
            
            // Calculate Returns
            if (firstBarFound) {
                currentBar.returns = (currentBar.adjClose - previousBar.adjClose) / previousBar.adjClose;
            } else {
                currentBar.returns = 0.0;
            }

            previousBar = currentBar;
            firstBarFound = true;
            finalBars.push_back(currentBar);
        }

        // Case B: Data missing -> Pad Forward
        else if (firstBarFound) {
            currentBar = previousBar;
            currentBar.date = date; // Update date to current union date
            currentBar.returns = 0.0; // No price change
            
            finalBars.push_back(currentBar);
            // Previous bar remains the same
        }

        // Case C: Missing data BEFORE the first bar exists (e.g. Google didn't exist in 1990)
        else {
            // Do nothing
        }
    }

    
}