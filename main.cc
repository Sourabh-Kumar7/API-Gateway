#include <drogon/drogon.h>
#include <iostream> // Add this

int main() {
    std::cout << "Starting Drogon initialization..." << std::endl; // Debug 1
    //Set HTTP listener address and port
    drogon::app().setLogLevel(trantor::Logger::kDebug);
    drogon::app().addListener("0.0.0.0", 5555);
    
    std::cout << "Initialization complete. Running server..." << std::endl; // Debug 2
    
    drogon::app().run();
    
    std::cout << "Server stopped." << std::endl; // Debug 3
    return 0;
}