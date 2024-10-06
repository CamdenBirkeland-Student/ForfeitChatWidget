#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "Message.h"

class ChatMessage : public Message {
public:
    ChatMessage(const std::string& senderID, const std::string& content);
    ~ChatMessage();

    void display() const override;
};

#endif // CHATMESSAGE_H