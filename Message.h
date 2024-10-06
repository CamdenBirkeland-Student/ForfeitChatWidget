#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <ctime>

class Message {
protected:
    std::string senderID;
    std::time_t timestamp;
    std::string content;

public:
    Message(const std::string& senderID, const std::string& content);
    virtual ~Message();

    virtual void display() const;
    std::string getSenderID() const;
    std::time_t getTimestamp() const;
    std::string getContent() const;
};

#endif // MESSAGE_H