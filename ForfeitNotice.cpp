#include "ForfeitNotice.h"
#include <iostream>

ForfeitNotice::ForfeitNotice(const std::string& senderID, const std::string& gameID, const std::string& reason)
    : Message(senderID, "Forfeit Notice"), gameID(gameID), reason(reason) {
}

ForfeitNotice::~ForfeitNotice() {
}

void ForfeitNotice::display() const {
    std::cout << "[Forfeit Notice] ";
    Message::display();
    std::cout << "Game ID: " << gameID << std::endl;
    std::cout << "Reason: " << reason << std::endl;
}

std::string ForfeitNotice::getGameID() const {
    return gameID;
}

std::string ForfeitNotice::getReason() const {
    return reason;
}