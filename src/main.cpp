#include "chat_server.h"
#include<iostream>

int main() {
    chat::CChatServer server;
    if (!server.Init()) {
       std::cout << "Init fail!\n";
       return 1;
    }
    server.Run();
    return 0;
}

