#pragma once
#include <string>

class EmailService {
public:
    virtual ~EmailService() = default;
    virtual void sendEmail(const std::string& to, const std::string& subject, const std::string& body) = 0;
};
