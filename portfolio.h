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