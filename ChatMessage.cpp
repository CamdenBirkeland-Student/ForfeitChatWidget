#include "ChatMessage.h"
#include <iostream>

ChatMessage::ChatMessage(const std::string& senderID, const std::string& content)
    : Message(senderID, content) {
}

ChatMessage::~ChatMessage() {
}

void ChatMessage::display() const {
    std::cout << "[Chat] ";
    Message::display();
}