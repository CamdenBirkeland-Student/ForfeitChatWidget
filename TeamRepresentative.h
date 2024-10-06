#ifndef TEAMREPRESENTATIVE_H
#define TEAMREPRESENTATIVE_H

#include "User.h"
#include <string>

class TeamRepresentative : public User {
public:
    TeamRepresentative(const std::string& username, const std::string& passwordHash);
    ~TeamRepresentative();

    void reportForfeit(const std::string& gameID, const std::string& reason);
    void sendMessage(const std::string& messageContent);
};

#endif // TEAMREPRESENTATIVE_H