#ifndef FORFEITNOTICE_H
#define FORFEITNOTICE_H

#include "Message.h"
#include <string>

class ForfeitNotice : public Message {
private:
    std::string gameID;
    std::string reason;

public:
    ForfeitNotice(const std::string& senderID, const std::string& gameID, const std::string& reason);
    ~ForfeitNotice();

    void display() const override;

    std::string getGameID() const;
    std::string getReason() const;
};

#endif // FORFEITNOTICE_H