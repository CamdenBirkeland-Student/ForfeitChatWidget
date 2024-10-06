#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "User.h"
#include <string>

class Administrator : public User {
public:
    Administrator(const std::string& username, const std::string& passwordHash);
    ~Administrator();

    void manageSchedules();
    void broadcastMessage(const std::string& messageContent);
};

#endif // ADMINISTRATOR_H