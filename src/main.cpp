#include "chat_server.h"

using namespace std;


int main() {
    chat::CChatServer *server = new chat::CChatServer();
    server->PrintInfo();
    return 0;
}

