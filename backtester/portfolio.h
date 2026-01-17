#pragma once

#include <vector>
#include <map>
#include <string>
#include <queue>
#include <memory>

#include "event.h"
#include "data_handler.h"

class Portfolio {
    /*
    The Portfolio class handles the positions and market
    value of all instruments at a resolution of a "bar",
    i.e. secondly, minutely, 5-min, 30-min, 60 min or EOD.
    */

public:
    virtual ~Portfolio() = default;

    /*
    Acts on a SignalEvent to generate new orders 
    based on the portfolio logic.
    */
    virtual void updateSignal(std::shared_ptr<SignalEvent> event) = 0;

    /*
    Updates the portfolio current positions and holdings 
    from a FillEvent.
    */
    virtual void updateFill(std::shared_ptr<FillEvent> event) = 0;
};

class NaivePortfolio : public Portfolio {
    /*
    The NaivePortfolio object is designed to send orders to
    a brokerage object with a constant quantity size blindly,
    i.e. without any risk management or position sizing. It is
    used to test simpler strategies such as BuyAndHoldStrategy.
    */
public:
    /*
    Parameters:
    bars - The DataHandler object with current market data.
    events - The Event Queue object.
    startDate - The start date (bar) of the portfolio.
    initialCapital - The starting capital in USD.
    */
    NaivePortfolio(DataHandler* bars, 
                   std::queue<std::shared_ptr<Event>>& events, 
                   std::string startDate, 
                   double initialCapital = 100000.0);

    // Override the pure virtual functions
    void updateSignal(std::shared_ptr<SignalEvent> event) override;
    void updateFill(std::shared_ptr<FillEvent> event) override;

    // For equity history if needed later
    // std::vector<std::map<std::string, double>>& getHistory();

private:
    DataHandler* bars;
    std::queue<std::shared_ptr<Event>>& events;
    std::vector<std::string> symbolList;
    std::string startDate;
    double initialCapital;
    
    std::map<std::string, long> currentPositions;
    std::vector<std::map<std::string, long>> allPositions;

    std::map<std::string, double> currentHoldings;
    std::vector<std::map<std::string, double>> allHoldings;

    void constructAllPositions();
    void constructAllHoldings();
    void constructCurrentHoldings();
};