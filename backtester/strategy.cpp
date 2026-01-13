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

void BuyAndHoldStrategy::calculateSignals() {
    /*
    For "Buy and Hold" we generate a single signal per symbol
    and then no additional signals. This means we are 
    constantly long the market from the date of strategy
    initialisation.

    Called in main.cpp when MarketEvent occurred
    */
    for (const auto& s : symbolList) {
        std::vector<Bar> bars = data->getLatestBars(s, 1);

        if (!bars.empty()) {
            if (!boughtStatus[s]) {
                Bar latestBar = bars[0];

                auto signal = std::make_shared<SignalEvent>(
                    latestBar.symbol, 
                    latestBar.date, 
                    SignalType::LONG
                );

                events.push(signal);
                boughtStatus[s] = true;
                
                std::cout << "LONG " << s << " at " << latestBar.close << std::endl;
            }
        }
    }
}