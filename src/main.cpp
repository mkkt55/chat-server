#include "chat_server.h"
#include<iostream>

int main() {
    chat::CChatServer server;
    if (!server.Init()) {
       std::cout << "Init fail!\n";
    }
    server.Run();
    return 0;
}

