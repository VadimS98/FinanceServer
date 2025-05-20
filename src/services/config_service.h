#pragma once
#include <string>

class ConfigService {
public:
    virtual ~ConfigService() = default;
    virtual std::string getDbPath() const = 0;
    virtual int getServerPort() const = 0;
    // и другие параметры по мере необходимости
};
