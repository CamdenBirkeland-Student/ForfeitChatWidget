#include "MessageHandler.h"
#include <iostream>

MessageHandler::MessageHandler() {
}

MessageHandler::~MessageHandler() {
    // Clean up any remaining messages in the queue
    while (!messageQueue.empty()) {
        delete messageQueue.front();
        messageQueue.pop();
    }
}

void MessageHandler::processMessage(Message* message) {
    // Lock the queue mutex to ensure thread safety
    std::lock_guard<std::mutex> lock(queueMutex);
    // Add the message to the queue
    messageQueue.push(message);
    std::cout << "Message from " << message->getSenderID() << " has been processed." << std::endl;

    // For simplicity, we'll dispatch the message immediately
    dispatchMessage(message, "recipientID"); // Replace "recipientID" with actual logic
}

void MessageHandler::dispatchMessage(Message* message, const std::string& recipientID) {
    // Implement the logic to dispatch the message to the recipient
    // For now, we'll just display the message
    std::cout << "Dispatching message to " << recipientID << ":" << std::endl;
    message->display();
}