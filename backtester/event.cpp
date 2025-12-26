#include <string>
#include <chrono>

enum class OrderType {
  LONG,
  SHORT
};

enum class EventType {
  MARKET,
  SIGNAL,
  ORDER,
  FILL
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
      return EventType::MARKET; 
    }

    std::string symbol;
    std::chrono::system_clock::time_point datetime;
    OrderType signalType;

    /*
    Parameters:
    symbol - The ticker symbol, e.g. 'GOOG'.
    datetime - The timestamp at which the signal was generated.
    signalType - 'LONG' or 'SHORT'.
    */
    SignalEvent(std::string symbol, 
      std::chrono::system_clock::time_point datetime,
       OrderType signalType)
      : symbol(symbol), datetime(datetime), signalType(signalType) {

      }
};