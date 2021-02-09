#include "chat_server.h"
#include <iostream>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

bool terminate = false;

void SigHandler(int sig) {
    printf("\nReceive signal: %d\n", sig);
    if (sig == SIGINT || sig == SIGTERM) {
        terminate = true;
    }
}

int main() {
    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);
    chat::CChatServer server;
    if (!server.Init()) {
       std::cout << "Init fail!\n";
       return 1;
    }
    for (; !terminate; ) {
        server.Run();
    }
    server.Stop();
    return 0;
}

