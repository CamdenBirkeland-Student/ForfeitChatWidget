#include "User.h"
#include <iostream>

User::User(const std::string& username, const std::string& passwordHash)
    : username(username), passwordHash(passwordHash) {
}

User::~User() {
}

bool User::authenticate(const std::string& password) {
    // For simplicity, compare the password directly
    // In a real application, use proper password hashing and verification
    if (password == passwordHash) {
        sessionID = "Session123"; // Simulate session creation
        return true;
    }
    return false;
}

void User::logout() {
    sessionID.clear();
    std::cout << username << " has logged out." << std::endl;
}

std::string User::getUsername() const {
    return username;
}

std::string User::getSessionID() const {
    return sessionID;
}