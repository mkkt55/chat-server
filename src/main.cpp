#include "chat_server.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <string>

int g_nPort = 15000;
int g_nInactiveWaitSecond = 180;
int g_nReconnWaitSecond = 120;
bool terminate = false;

void SigHandler(int sig) {
    printf("\nReceive signal: %d\n", sig);
    if (sig == SIGINT || sig == SIGTERM) {
        terminate = true;
    }
}

bool readConfig() {
    using namespace std;
    ifstream fin;
    fin.open("./config");
    if (!fin.is_open()) {
        printf("Open config file fail, check it first...");
        return false;
    }

    unordered_map<string, string> cfgMap;
    char finBuffer[1024];
    string line;
    while(fin.good() && !fin.eof())
    {
        fin.getline(finBuffer, 1024);
        line = finBuffer;
        string delim = "=";
        int pos = line.find('=');
        if (pos != std::string::npos)
        {
            cfgMap[line.substr(0, pos)] = line.substr(pos + 1, line.size() - pos);
        }
    }

    {
        auto iter = cfgMap.find("port");
        if (iter == cfgMap.end()) {
            return false;
        }
        int nVal = stoi(iter->second);
        g_nPort = nVal;
    }
    {
        auto iter = cfgMap.find("inactive_wait_second");
        if (iter == cfgMap.end()) {
            return false;
        }
        int nVal = stoi(iter->second);
        g_nInactiveWaitSecond = nVal;
    }
    {
        auto iter = cfgMap.find("reconn_wait_second");
        if (iter == cfgMap.end()) {
            return false;
        }
        int nVal = stoi(iter->second);
        g_nReconnWaitSecond = nVal;
    }

    fin.close();
    return true;
}

int main() {
    if (!readConfig()) {
        exit(1);
    }
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

