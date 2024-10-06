#include "Message.h"
#include <iostream>
#include <ctime>

Message::Message(const std::string& senderID, const std::string& content)
    : senderID(senderID), content(content) {
    timestamp = std::time(nullptr);
}

Message::~Message() {
}

void Message::display() const {
    std::cout << "[" << std::ctime(&timestamp) << "] " << senderID << ": " << content << std::endl;
}

std::string Message::getSenderID() const {
    return senderID;
}

std::time_t Message::getTimestamp() const {
    return timestamp;
}

std::string Message::getContent() const {
    return content;
}