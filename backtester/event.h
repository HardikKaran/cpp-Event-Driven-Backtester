#pragma once

#include <string>
#include <chrono>
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
    EventType getEventType() const override { 
      return EventType::SIGNAL; 
    }

    std::string symbol;
    std::chrono::system_clock::time_point datetime;
    SignalType signalType;

    /*
    Parameters:
    symbol - The ticker symbol, e.g. 'GOOG'.
    datetime - The timestamp at which the signal was generated.
    signalType - 'LONG' or 'SHORT'.
    */
    SignalEvent(std::string symbol, 
      std::chrono::system_clock::time_point datetime,
      SignalType signalType)
        : symbol(symbol), datetime(datetime), signalType(signalType) {

      }
};

class OrderEvent : public Event {
  /*
  Handles the event of sending an Order to an execution system.
  The order contains a symbol (e.g. GOOG), a type (market or limit),
  quantity and a direction.
  */

  public:
    EventType getEventType() const override { 
      return EventType::ORDER; 
    }

    std::string symbol;
    OrderType orderType;
    unsigned long quantity;
    DirectionType direction;

    /*
    Parameters:
    symbol - The instrument to trade.
    order_type - 'MKT' or 'LMT' for Market or Limit.
    quantity - Non-negative integer for quantity.
    direction - 'BUY' or 'SELL' for long or short.
    */
    OrderEvent(std::string symbol, 
    OrderType orderType,
    unsigned long quantity,
    DirectionType direction)
      : symbol(symbol), orderType(orderType), quantity(quantity), direction(direction) {

      }
};

class FillEvent : public Event {
  /*
  Encapsulates the notion of a Filled Order, as returned
  from a brokerage. Stores the quantity of an instrument
  actually filled and at what price. In addition, stores
  the commission of the trade from the brokerage.
  */

  public:
    EventType getEventType() const override { 
      return EventType::FILL; 
    }

    std::chrono::system_clock::time_point timeIndex;
    std::string symbol;
    std::string exchange;
    unsigned long quantity;
    DirectionType direction;
    long double fillCost;
    long double commission;

    static double calcCommission(unsigned long quantity, long double fillCost) {
      /*
      Calculates the fees of trading based on Interactive Brokers 
      'Fixed' pricing for US Stocks (SmartRouted).
      
      Source: https://www.interactivebrokers.com/en/pricing/commissions-stocks.php
      */

      double commPerShare = 0.005;
      double minComm = 1.00;
      double maxPercent = 1.0;

      double baseComm = std::max(minComm, commPerShare * quantity);

      double tradeVal = quantity * fillCost;
      double maxCost = (maxPercent / 100.0) * tradeVal;

      double fullCost = std::min(baseComm, maxCost);

      return fullCost;
    }

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
    FillEvent(std::chrono::system_clock::time_point timeIndex,
    std::string symbol,
    std::string exchange,
    unsigned long quantity,
    DirectionType direction,
    long double fillCost,
    long double commission = 0) 
      : timeIndex(timeIndex), symbol(symbol), exchange(exchange), quantity(quantity), 
        direction(direction), fillCost(fillCost), commission(commission) {
          if(this->commission == 0) {
            this->commission = calcCommission(quantity, fillCost);
          }
        }
};
