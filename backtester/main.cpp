#include <iostream>
#include <queue>
#include <memory>
#include <vector>
#include <string>

#include "event.h"
#include "data_handler.h"

int main() {
    // Create event queue for communication with the system
    std::queue<std::shared_ptr<Event>> events;

    // Define file directory and dummy file
    std::string csvDir = "symbol_data";
    std::vector<std::string> symbolList = {"AAPL"};
    
    std::cout << "-----Starting Data Handler-----" << std::endl;

    // Creates DataHandler object and trigger opening of csv files
    HistoricCSVDataHandler dataHandler(events, csvDir, symbolList);

    std::cout << "-----Data Handler object created-----" << std::endl;
    std::cout << "-----Starting Backtest loop-----" << std::endl;

    // Run simulation loop
    for (int i = 0; i < 4; i++) {
        std::cout << "Row: " << i + 1 << ", Updating bars" << std::endl;

        // Handler ticks forward
        dataHandler.updateBars();

        // Check if Handler pushed event to queue
        if (!events.empty()) {
            // Get event from front of queue
            std::shared_ptr<Event> event = events.front();
            events.pop();

            // Check if event contains a Market event
            if  (event->getEventType() == EventType::MARKET) {
                std::cout << "-----Market Event-----" << std::endl;

                // Get the latest data from DataHandler (future implementation with Strategy)
                std::vector<Bar> latestBars = dataHandler.getLatestBars("AAPL");

                // Display latest bar's values
                if (!latestBars.empty()) {
                    Bar bar = latestBars.back();
                    std::cout << "-----Latest Bar for AAPL-----" << std::endl;
                    std::cout << "Date: " << bar.date << std::endl;
                    std::cout << "Close: " << bar.close << std::endl;
                    std::cout << "Returns: " << bar.returns << std::endl;
                }
            }
        }

        else {
            std::cout << "-----No Events-----" << std::endl;
        }
    }

    return 0;
}