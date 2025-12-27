#include <string>
#include <chrono>

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
    EventType getEventType() const override { 
      return EventType::MARKET; 
    }

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
      : symbol(symbol), orderType(orderType), quantity(quantity), direction(direction){

      }
};