#pragma once
#include <crow.h>

class WebSocketAPI {
public:
    void registerWebSocket(crow::SimpleApp& app);
    // В будущем: обработчики для WebSocket-соединений
};
