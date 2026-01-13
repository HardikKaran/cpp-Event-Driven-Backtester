#include <iostream>

#include "strategy.h"

BuyAndHoldStrategy::BuyAndHoldStrategy(DataHandler* data, 
                                       std::queue<std::shared_ptr<Event>>& events,
                                       std::vector<std::string> symbolList)

    : data(data), events(events), symbolList(symbolList) {
        for (const auto& s : symbolList) {
            boughtStatus[s] = false;
        }
    }