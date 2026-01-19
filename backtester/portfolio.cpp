#include <iostream>

#include "portfolio.h"

NaivePortfolio::NaivePortfolio(DataHandler* bars, 
                               std::queue<std::shared_ptr<Event>>& events, 
                               std::string startDate, 
                               double initialCapital)
    : bars(bars), events(events), startDate(startDate), initialCapital(initialCapital) {

    this->symbolList = bars->getSymbolList(); 

    // Initialize the tracking containers
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

void NaivePortfolio::constructCurrentHoldings() {
    for (const auto& sL : symbolList) {
        currentHoldings[sL] = 0.0;
    }

    currentHoldings["cash"] = initialCapital;
    currentHoldings["commission"] = 0.0;
    currentHoldings["total"] = initialCapital;
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