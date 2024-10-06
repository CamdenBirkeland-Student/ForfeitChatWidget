#include "TeamRepresentative.h"
#include "ForfeitNotice.h"
#include "ChatMessage.h"
#include <iostream>

TeamRepresentative::TeamRepresentative(const std::string& username, const std::string& passwordHash)
    : User(username, passwordHash) {
}

TeamRepresentative::~TeamRepresentative() {
}

void TeamRepresentative::reportForfeit(const std::string& gameID, const std::string& reason) {
    ForfeitNotice notice(username, gameID, reason);
    notice.display();
    // Implement logic to send the forfeit notice to the server or relevant parties
}

void TeamRepresentative::sendMessage(const std::string& messageContent) {
    ChatMessage message(username, messageContent);
    message.display();
    // Implement logic to send the chat message to the server or recipients
}