#include "execution.h"

#include <ctime>
#include <memory>

SimulatedExecutionHandler::SimulatedExecutionHandler(std::queue<std::shared_ptr<Event>>& events)
    : events(events) {}

void SimulatedExecutionHandler::executeOrder(std::shared_ptr<OrderEvent> event) {
    if (event->getEventType() == EventType::ORDER) {
        auto fillEvent = std::make_shared<FillEvent>(
            std::time(nullptr),
            event->symbol,
            "ARCA",
            event->quantity,
            event->direction,
            0  // fillCost = None equivalent
        );
        events.push(fillEvent);
    }
}
