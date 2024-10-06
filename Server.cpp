#include "Server.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <sstream>
#include <sqlite3.h>
#include "json.hpp" // Include the nlohmann/json header

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

// Function to URL-decode strings
std::string urlDecode(const std::string& eString) {
    std::string result;
    char ch;
    int i, ii;
    for (i = 0; i < eString.length(); i++) {
        if (int(eString[i]) == 37) { // '%'
            sscanf(eString.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            result += ch;
            i = i + 2;
        } else if (eString[i] == '+') {
            result += ' ';
        } else {
            result += eString[i];
        }
    }
    return result;
}

// Function to parse form data
void parseFormData(const std::string& body, std::map<std::string, std::string>& formData) {
    std::vector<std::string> pairs;
    boost::split(pairs, body, boost::is_any_of("&"));
    for (const auto& pair : pairs) {
        std::vector<std::string> kv;
        boost::split(kv, pair, boost::is_any_of("="));
        if (kv.size() == 2) {
            formData[urlDecode(kv[0])] = urlDecode(kv[1]);
        }
    }
}

// Function to parse JSON data
void parseJsonData(const std::string& body, std::map<std::string, std::string>& formData) {
    try {
        json j = json::parse(body);
        if (j.contains("email")) formData["email"] = j.at("email").get<std::string>();
        if (j.contains("league")) formData["league"] = j.at("league").get<std::string>();
        if (j.contains("team")) formData["team"] = j.at("team").get<std::string>();
        if (j.contains("game_date")) formData["game_date"] = j.at("game_date").get<std::string>();
    } catch (json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

// Function to add CORS headers
void addCorsHeaders(http::response<http::string_body>& res) {
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Access-Control-Allow-Methods", "POST, OPTIONS");
    res.set("Access-Control-Allow-Headers", "Content-Type");
}

// Function to initialize the database and create the table
void initializeDatabase(sqlite3** db_) {
    char* errMsg = nullptr;

    // Use absolute path to ensure consistency
    const char* dbPath = "/Users/camdenbirkeland/Desktop/CoEdChatApp/forfeits.db";
    int rc = sqlite3_open(dbPath, db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(*db_) << std::endl;
        sqlite3_close(*db_);
        *db_ = nullptr;
        return;
    }

    const char* sqlCreateTable = R"(
        CREATE TABLE IF NOT EXISTS forfeits (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT NOT NULL,
            league TEXT NOT NULL,
            team TEXT NOT NULL,
            game_date TEXT NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    rc = sqlite3_exec(*db_, sqlCreateTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Database initialized successfully." << std::endl;
    }
}

Server::Server(net::io_context& ioc, unsigned short port)
    : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)), db_(nullptr) {
    initializeDatabase(&db_);
}

Server::~Server() {
    if (db_) {
        sqlite3_close(db_);
    }
}

void Server::run() {
    doAccept();
}

void Server::doAccept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Accepted a new connection." << std::endl;
                handleRequest(std::move(socket));
            } else {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }
            doAccept(); // Continue accepting new connections
        });
}

void Server::handleRequest(tcp::socket socket) {
    auto self = shared_from_this();
    auto buffer = std::make_shared<beast::flat_buffer>();
    auto stream = std::make_shared<beast::tcp_stream>(std::move(socket));
    auto req = std::make_shared<http::request<http::string_body>>();

    http::async_read(*stream, *buffer, *req,
        [this, self, stream, buffer, req](beast::error_code ec, std::size_t bytes_transferred) mutable {
            if (!ec) {
                std::cout << "Received request: " << req->method_string() << " " << req->target() << std::endl;
                
                if (req->method() == http::verb::options) {
                    std::cout << "Handling OPTIONS request." << std::endl;
                    // Handle CORS preflight request
                    http::response<http::string_body> res{http::status::ok, req->version()};
                    addCorsHeaders(res);
                    res.set(http::field::content_type, "text/plain");
                    res.body() = "";
                    res.prepare_payload();

                    http::async_write(*stream, res,
                        [stream](beast::error_code ec, std::size_t) {
                            if (ec) {
                                std::cerr << "Error writing OPTIONS response: " << ec.message() << std::endl;
                            } else {
                                std::cout << "OPTIONS response sent successfully." << std::endl;
                            }
                            beast::error_code shutdown_ec;
                            stream->socket().shutdown(tcp::socket::shutdown_send, shutdown_ec);
                        });
                    return; // Exit after handling OPTIONS
                }
                else if (req->method() == http::verb::post && req->target() == "/submit_forfeit") {
                    std::cout << "Handling POST /submit_forfeit request." << std::endl;
                    // Determine the content type
                    std::string contentType;
                    if (req->find(http::field::content_type) != req->end()) {
                        // Convert boost::beast::string_view to std::string
                        contentType = std::string(req->at(http::field::content_type));
                    } else {
                        contentType = "";
                    }

                    std::map<std::string, std::string> formData;

                    if (contentType.find("application/json") != std::string::npos) {
                        // Parse JSON data
                        parseJsonData(req->body(), formData);
                        std::cout << "Parsed JSON data: ";
                        for (const auto& [key, value] : formData) {
                            std::cout << key << "=" << value << " ";
                        }
                        std::cout << std::endl;
                    } else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
                        // Parse form data
                        parseFormData(req->body(), formData);
                        std::cout << "Parsed form data: ";
                        for (const auto& [key, value] : formData) {
                            std::cout << key << "=" << value << " ";
                        }
                        std::cout << std::endl;
                    } else {
                        // Unsupported content type
                        std::cerr << "Unsupported Content-Type: " << contentType << std::endl;
                        http::response<http::string_body> res{http::status::unsupported_media_type, req->version()};
                        addCorsHeaders(res);
                        res.set(http::field::content_type, "text/plain");
                        res.body() = "Unsupported Media Type";
                        res.prepare_payload();

                        http::async_write(*stream, res,
                            [stream](beast::error_code ec, std::size_t) {
                                if (ec) {
                                    std::cerr << "Error writing Unsupported Media Type response: " << ec.message() << std::endl;
                                } else {
                                    std::cout << "Unsupported Media Type response sent successfully." << std::endl;
                                }
                                beast::error_code shutdown_ec;
                                stream->socket().shutdown(tcp::socket::shutdown_send, shutdown_ec);
                            });
                        return;
                    }

                    // Validate required fields
                    if (formData.find("email") == formData.end() ||
                        formData.find("league") == formData.end() ||
                        formData.find("team") == formData.end() ||
                        formData.find("game_date") == formData.end()) {
                        std::cerr << "Missing required fields." << std::endl;
                        http::response<http::string_body> res{http::status::bad_request, req->version()};
                        addCorsHeaders(res);
                        res.set(http::field::content_type, "text/plain");
                        res.body() = "Missing required fields.";
                        res.prepare_payload();

                        http::async_write(*stream, res,
                            [stream](beast::error_code ec, std::size_t) {
                                if (ec) {
                                    std::cerr << "Error writing Bad Request response: " << ec.message() << std::endl;
                                } else {
                                    std::cout << "Bad Request response sent successfully." << std::endl;
                                }
                                beast::error_code shutdown_ec;
                                stream->socket().shutdown(tcp::socket::shutdown_send, shutdown_ec);
                            });
                        return;
                    }

                    // Insert the forfeit notice into the database
                    insertForfeitNotice(formData["email"], formData["league"], formData["team"], formData["game_date"]);

                    // Send a response
                    http::response<http::string_body> res{http::status::ok, req->version()};
                    addCorsHeaders(res);
                    res.set(http::field::content_type, "text/plain");
                    res.body() = "Forfeit notice received.";
                    res.prepare_payload();

                    http::async_write(*stream, res,
                        [stream](beast::error_code ec, std::size_t) {
                            if (ec) {
                                std::cerr << "Error writing OK response: " << ec.message() << std::endl;
                            } else {
                                std::cout << "OK response sent successfully." << std::endl;
                            }
                            beast::error_code shutdown_ec;
                            stream->socket().shutdown(tcp::socket::shutdown_send, shutdown_ec);
                        });
                }
                else {
                    std::cerr << "Received unknown request: " << req->method_string() << " " << req->target() << std::endl;
                    // Handle unknown requests
                    http::response<http::string_body> res{http::status::not_found, req->version()};
                    addCorsHeaders(res);
                    res.set(http::field::server, "CoEdChatApp");
                    res.set(http::field::content_type, "text/plain");
                    res.body() = "Not Found";
                    res.prepare_payload();

                    http::async_write(*stream, res,
                        [stream](beast::error_code ec, std::size_t) {
                            if (ec) {
                                std::cerr << "Error writing Not Found response: " << ec.message() << std::endl;
                            } else {
                                std::cout << "Not Found response sent successfully." << std::endl;
                            }
                            beast::error_code shutdown_ec;
                            stream->socket().shutdown(tcp::socket::shutdown_send, shutdown_ec);
                        });
                }
            } else {
                std::cerr << "Error reading request: " << ec.message() << std::endl;
            }
        });
}

void Server::insertForfeitNotice(const std::string& email, const std::string& league, const std::string& team, const std::string& game_date) {
    if (!db_) {
        std::cerr << "Database connection is not available." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(db_mutex_); // Lock the mutex for thread safety

    const char* sqlInsert = "INSERT INTO forfeits (email, league, team, game_date) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db_, sqlInsert, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, league.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, team.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, game_date.c_str(), -1, SQLITE_TRANSIENT);

    // Execute statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db_) << std::endl;
    } else {
        std::cout << "Forfeit notice inserted successfully." << std::endl;
    }

    // Clean up
    sqlite3_finalize(stmt);
}