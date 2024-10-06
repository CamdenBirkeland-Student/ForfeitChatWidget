#include "Administrator.h"
#include "ChatMessage.h"
#include <iostream>

Administrator::Administrator(const std::string& username, const std::string& passwordHash)
    : User(username, passwordHash) {
}

Administrator::~Administrator() {
}

void Administrator::manageSchedules() {
    std::cout << "Administrator " << username << " is managing schedules." << std::endl;
    // Implement schedule management logic here
}

void Administrator::broadcastMessage(const std::string& messageContent) {
    ChatMessage message(username, messageContent);
    message.display();
    // Implement logic to broadcast the message to all users
}