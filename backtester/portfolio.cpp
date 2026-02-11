#include <iostream>

#include "portfolio.h"

NaivePortfolio::NaivePortfolio(DataHandler* bars, 
                               std::queue<std::shared_ptr<Event>>& events, 
                               std::string startDate, 
                               double initialCapital)
    : bars(bars), events(events), startDate(startDate), initialCapital(initialCapital) {

    this->symbolList = bars->getSymbolList(); 

    // Initialise the tracking containers
    constructAllPositions();
    constructCurrentHoldings();
    constructAllHoldings();
}

void NaivePortfolio::constructAllPositions() {
    std::map<std::string, long> initialPos;

    for (const auto& sL : symbolList) {
        initialPos[sL] = 0;
    }

    currentPositions = initialPos;

    allPositions.push_back(initialPos);
}

void NaivePortfolio::constructAllHoldings() {
    std::map<std::string, double> initialHoldings;

    for (const auto& symbol : symbolList) {
        initialHoldings[symbol] = 0.0;
    }

    initialHoldings["cash"] = initialCapital;
    initialHoldings["commission"] = 0.0;
    initialHoldings["total"] = initialCapital;

    allHoldings.push_back(initialHoldings);
}

void NaivePortfolio::constructCurrentHoldings() {
    for (const auto& sL : symbolList) {
        currentHoldings[sL] = 0.0;
    }

    currentHoldings["cash"] = initialCapital;
    currentHoldings["commission"] = 0.0;
    currentHoldings["total"] = initialCapital;
}

void NaivePortfolio::updateTimeIndex(std::shared_ptr<Event> event) {
    // Grab the latest bars for each symbol
    std::map<std::string, std::vector<Bar>> latestBars;
    for (const auto& s : symbolList) {
        latestBars[s] = bars->getLatestBars(s, 1);
    }

    // Update positions
    std::map<std::string, long> dp;
    for (const auto& s : symbolList) {
        dp[s] = currentPositions[s];
    }
    allPositions.push_back(dp);

    // Update holdings
    std::map<std::string, double> dh;
    for (const auto& s : symbolList) {
        dh[s] = 0.0;
    }
    dh["cash"] = currentHoldings["cash"];
    dh["commission"] = currentHoldings["commission"];
    dh["total"] = currentHoldings["cash"];

    for (const auto& s : symbolList) {
        // Approximation to the real value
        double marketValue = currentPositions[s] * latestBars[s][0].adjClose;
        dh[s] = marketValue;
        dh["total"] += marketValue;
    }

    allHoldings.push_back(dh);
}

