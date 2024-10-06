#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <memory>
#include <sqlite3.h>
#include <mutex>

class Server : public std::enable_shared_from_this<Server> {
public:
    Server(boost::asio::io_context& ioc, unsigned short port);
    ~Server();
    void run();

private:
    void doAccept();
    void handleRequest(boost::asio::ip::tcp::socket socket);
    void insertForfeitNotice(const std::string& email, const std::string& league, const std::string& team, const std::string& game_date);

    boost::asio::ip::tcp::acceptor acceptor_;
    sqlite3* db_; // Persistent database connection
    std::mutex db_mutex_; // Mutex to protect SQLite operations
};

#endif // SERVER_H