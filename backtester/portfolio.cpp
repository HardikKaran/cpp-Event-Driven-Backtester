#include <iostream>

#include "portfolio.h"

NaivePortfolio::NaivePortfolio(DataHandler* bars, 
                               std::queue<std::shared_ptr<Event>>& events, 
                               std::string startDate, 
                               double initialCapital)
    : bars(bars), events(events), startDate(startDate), initialCapital(initialCapital) {

    this->symbolList = bars->getSymbols(); 

    // Initialize the tracking containers
    constructAllPositions();
    constructCurrentHoldings();
    constructAllHoldings();
}