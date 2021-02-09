#pragma once
#include "sock_wrapper.h"
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

namespace chat {
  
const int MAX_CONN = 5;
const int MAX_EVENT = 10;

class CChatServer {
    public:
        CChatServer() {}
        ~CChatServer() {}
        void PrintInfo();
        bool Init(int port = 15000);
        bool Run();
        bool Stop();
    private:
        int epollfd;
        int evc = 0;
        struct epoll_event ev, events[MAX_EVENT];
        struct sockaddr_in listenAddr; 
        int addrLen;
        int loopCount = 0;
        SockWrapper* m_oListenWrapper;
    private:
        bool m_bHasInit = false;
};

}
