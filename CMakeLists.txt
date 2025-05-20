cmake_minimum_required(VERSION 3.25)
project(FinApp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск зависимостей через vcpkg
find_package(Crow CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Tesseract CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)

add_executable(finapp
    src/main.cpp

    # Database
    src/database/db_manager.cpp
    src/database/expenses_dao.cpp
    src/database/users_dao.cpp
    src/database/currency_rates_dao_sqlite.cpp
    src/database/receipts_dao.cpp

    # API
    src/api/expenses_api.cpp
    src/api/auth_api.cpp
    src/api/currency_rates_api.cpp
    src/api/receipts_api.cpp
    src/api/websocket_api.cpp

    # Middleware
    src/api/middleware/auth_middleware.cpp

    # Services
    # src/services/email_service.cpp
    # src/services/config_service.cpp

    # Utils (если есть .cpp)

    # Модели (если есть .cpp)
    # src/models/user.cpp
    # src/models/receipt.cpp
 "src/database/currency_rates_dao_sqlite.h")

# Пути к заголовкам
target_include_directories(finapp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api/middleware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Линковка библиотек
target_link_libraries(finapp PRIVATE 
    Crow::Crow 
    cpr::cpr 
    SQLite::SQLite3 
    Tesseract::libtesseract
    nlohmann_json::nlohmann_json
    jwt-cpp::jwt-cpp
)
