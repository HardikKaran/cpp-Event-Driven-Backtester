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

void NaivePortfolio::updatePositionsFromFill(std::shared_ptr<FillEvent> fill) {
    // Check whether the fill is a buy or sell
    int fillDir = 0;
    if (fill->direction == DirectionType::BUY) {
        fillDir = 1;
    }
    if (fill->direction == DirectionType::SELL) {
        fillDir = -1;
    }

    // Update positions list with new quantities
    currentPositions[fill->symbol] += fillDir * static_cast<long>(fill->quantity);
}

void NaivePortfolio::updateHoldingsFromFill(std::shared_ptr<FillEvent> fill) {
    int fillDir = 0;
    if (fill->direction == DirectionType::BUY) {
        fillDir = 1;
    }
    if (fill->direction == DirectionType::SELL) {
        fillDir = -1;
    }

    // Update holdings list with new quantities
    double fillCost = bars->getLatestBars(fill->symbol)[0].adjClose; // Close price
    double cost = fillDir * fillCost * fill->quantity;
    currentHoldings[fill->symbol] += cost;
    currentHoldings["commission"] += fill->commission;
    currentHoldings["cash"] -= (cost + fill->commission);
    currentHoldings["total"] -= (cost + fill->commission);
}

void NaivePortfolio::updateFill(std::shared_ptr<FillEvent> event) {
    if (event->getEventType() == EventType::FILL) {
        updatePositionsFromFill(event);
        updateHoldingsFromFill(event);
    }
}

void NaivePortfolio::generateNaiveOrder(std::shared_ptr<SignalEvent> signal) {
    std::string symbol = signal->symbol;
    SignalType direction = signal->signalType;

    unsigned long mktQuantity = 100;  // Constant quantity (no strength field)
    long curQuantity = currentPositions[symbol];
    OrderType orderType = OrderType::MKT;

    std::shared_ptr<OrderEvent> order = nullptr;

    if (direction == SignalType::LONG && curQuantity == 0) {
        order = std::make_shared<OrderEvent>(symbol, orderType, mktQuantity, DirectionType::BUY);
    }
    if (direction == SignalType::SHORT && curQuantity == 0) {
        order = std::make_shared<OrderEvent>(symbol, orderType, mktQuantity, DirectionType::SELL);
    }

    if (direction == SignalType::EXIT && curQuantity > 0) {
        order = std::make_shared<OrderEvent>(symbol, orderType, static_cast<unsigned long>(std::abs(curQuantity)), DirectionType::SELL);
    }
    if (direction == SignalType::EXIT && curQuantity < 0) {
        order = std::make_shared<OrderEvent>(symbol, orderType, static_cast<unsigned long>(std::abs(curQuantity)), DirectionType::BUY);
    }

    if (order != nullptr) {
        events.push(order);
    }
}

void NaivePortfolio::updateSignal(std::shared_ptr<SignalEvent> event) {
    if (event->getEventType() == EventType::SIGNAL) {
        generateNaiveOrder(event);
    }
}

void NaivePortfolio::createEquityCurveDataframe() {
    equityCurve.clear();

    for (size_t i = 0; i < allHoldings.size(); i++) {
        double total = allHoldings[i]["total"];
        double returns = 0.0;
        double equityCurveVal = 1.0;

        if (i > 0) {
            double prevTotal = allHoldings[i - 1]["total"];
            returns = (prevTotal != 0.0) ? (total - prevTotal) / prevTotal : 0.0;
        }

        if (i > 0) {
            equityCurveVal = equityCurve[i - 1]["equity_curve"] * (1.0 + returns);
        }

        std::map<std::string, double> row;
        row["total"] = total;
        row["returns"] = returns;
        row["equity_curve"] = equityCurveVal;
        equityCurve.push_back(row);
    }
}

