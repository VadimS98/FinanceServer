cmake_minimum_required(VERSION 3.25)
project(FinApp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск зависимостей через vcpkg
find_package(Crow CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Tesseract CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)  # Добавлено

add_executable(finapp
    src/main.cpp 
    src/database/db_manager.cpp
    src/database/expenses_dao.cpp
    src/api/expenses_api.cpp
 "src/utils/convert_utils.h")

# Пути к заголовкам
target_include_directories(finapp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

# Линковка библиотек
target_link_libraries(finapp PRIVATE 
    Crow::Crow 
    cpr::cpr 
    SQLite::SQLite3 
    Tesseract::libtesseract
    nlohmann_json::nlohmann_json  # Добавлено
)
