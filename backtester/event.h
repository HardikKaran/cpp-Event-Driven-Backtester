#pragma once

#include <string>
#include <ctime>
#include <algorithm>

enum class EventType {
  MARKET,
  SIGNAL,
  ORDER,
  FILL
};

enum class SignalType {
  LONG,
  SHORT
};

enum class OrderType {
  MKT,
  LMT
};

enum class DirectionType {
  BUY,
  SELL
};

class Event {
  /*
  Event is base class providing an interface for all subsequent 
  (inherited) events, that will trigger further events in the 
  trading infrastructure. (future-proofing design)
  */

  public:
    virtual ~Event() = default;
    virtual EventType getEventType() const = 0;
};

class MarketEvent : public Event {
  /*
  Handles the event of receiving a new market update with 
  corresponding bars.
  */

  public: 
    MarketEvent();
    EventType getEventType() const override;
};

class SignalEvent : public Event {
  /*
  Handles the event of sending a Signal from a Strategy object.
  This is received by a Portfolio object and acted upon.
  */

  public: 
    /*
    Parameters:
    symbol - The ticker symbol, e.g. 'GOOG'.
    datetime - The timestamp at which the signal was generated.
    signalType - 'LONG' or 'SHORT'.
    */
   
    SignalEvent(const std::string& symbol, time_t datetime, SignalType signalType);
    EventType getEventType() const override;

    std::string symbol;
    time_t datetime;
    SignalType signalType;
};

class OrderEvent : public Event {
  /*
  Handles the event of sending an Order to an execution system.
  The order contains a symbol (e.g. GOOG), a type (market or limit),
  quantity and a direction.
  */

  public:
    /*
    Parameters:
    symbol - The instrument to trade.
    order_type - 'MKT' or 'LMT' for Market or Limit.
    quantity - Non-negative integer for quantity.
    direction - 'BUY' or 'SELL' for long or short.
    */
    OrderEvent(const std::string& symbol, OrderType orderType, unsigned long quantity, 
      DirectionType direction);
    EventType getEventType() const override;

    std::string symbol;
    OrderType orderType;
    unsigned long quantity;
    DirectionType direction;
};

class FillEvent : public Event {
  /*
  Encapsulates the notion of a Filled Order, as returned
  from a brokerage. Stores the quantity of an instrument
  actually filled and at what price. In addition, stores
  the commission of the trade from the brokerage.
  */

  public:
    /*
    If commission is not provided, the Fill object will
    calculate it based on the trade size and Interactive
    Brokers fees.

    Parameters:
    timeindex - The bar-resolution when the order was filled.
    symbol - The instrument which was filled.
    exchange - The exchange where the order was filled.
    quantity - The filled quantity.
    direction - The direction of fill ('BUY' or 'SELL')
    fill_cost - The holdings value in dollars.
    commission - An optional commission sent from IB.
    */
    FillEvent(time_t timeIndex, std::string symbol,
    std::string exchange, unsigned long quantity, DirectionType direction,
    long double fillCost, long double commission = 0);
    EventType getEventType() const override;

    static double calcCommission(unsigned long quantity, long double fillCost);

    time_t timeIndex;
    std::string symbol;
    std::string exchange;
    unsigned long quantity;
    DirectionType direction;
    long double fillCost;
    long double commission;
};
