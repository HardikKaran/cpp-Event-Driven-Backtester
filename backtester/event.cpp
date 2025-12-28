#include "event.h"

// MarketEvent instantiated
MarketEvent::MarketEvent() {}
EventType MarketEvent::getEventType() const { 
  return EventType::MARKET; 
}
 
// SignalEvent instantiated
SignalEvent::SignalEvent(const std::string& symbol, 
  std::chrono::system_clock::time_point datetime, SignalType signalType) 
: symbol(symbol), datetime(datetime), signalType(signalType) {}
EventType SignalEvent::getEventType() const {
  return EventType::SIGNAL; // Override 
}

// OrderEvent instantiated
OrderEvent::OrderEvent(const std::string& symbol, OrderType orderType, 
  unsigned long quantity, DirectionType direction)
: symbol(symbol), orderType(orderType), quantity(quantity), direction(direction) {}
EventType OrderEvent::getEventType() const {
  return EventType::ORDER;
}

// Commission calculation 
double calcCommission(unsigned long quantity, long double fillCost) {
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

// FillEvent instantiated
FillEvent::FillEvent(std::chrono::system_clock::time_point timeIndex, std::string symbol,
  std::string exchange, unsigned long quantity, DirectionType direction,
  long double fillCost, long double commission) 
: timeIndex(timeIndex), symbol(symbol), exchange(exchange), quantity(quantity), 
direction(direction), fillCost(fillCost), commission(commission) {
  if(this->commission == 0) {
    this->commission = calcCommission(quantity,fillCost);
  }
}

EventType FillEvent::getEventType() const {
  return EventType::FILL;
}
  