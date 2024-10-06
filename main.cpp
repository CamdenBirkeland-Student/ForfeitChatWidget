#include "Server.h"
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <memory>
#include <sqlite3.h> // Include SQLite header

int main() {
    try {
        boost::asio::io_context io_context;

        // Handle SIGINT and SIGTERM for graceful shutdown
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](boost::system::error_code /*ec*/, int /*signo*/) {
            std::cout << "\nSignal received, shutting down server." << std::endl;
            io_context.stop();
        });

        // Create a shared pointer to the Server
        auto server = std::make_shared<Server>(io_context, 8080);
        server->run();

        std::cout << "Server is running on port 8080" << std::endl;

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}