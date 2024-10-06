#ifndef USER_H
#define USER_H

#include <string>

class User {
protected:
    std::string username;
    std::string passwordHash;
    std::string sessionID;

public:
    User(const std::string& username, const std::string& passwordHash);
    virtual ~User();

    virtual bool authenticate(const std::string& password);
    void logout();

    std::string getUsername() const;
    std::string getSessionID() const;
};

#endif // USER_H