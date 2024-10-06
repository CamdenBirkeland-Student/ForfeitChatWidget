#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "Message.h"
#include <string>
#include <queue>
#include <mutex>

class MessageHandler {
private:
    std::queue<Message*> messageQueue;
    std::mutex queueMutex;

public:
    MessageHandler();
    ~MessageHandler();

    void processMessage(Message* message);
    void dispatchMessage(Message* message, const std::string& recipientID);
};

#endif // MESSAGEHANDLER_H