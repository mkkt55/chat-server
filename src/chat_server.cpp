#include "chat_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/errno.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

#include "sock_wrapper.h"
#include "client.h"

using namespace std;

const int MAX_CONN = 5;
const int MAX_EVENT = 10;

namespace chat {

void CChatServer::PrintInfo() {
   std::cout << "Hello" <<std::endl;
}

bool CChatServer::Init() {
   struct sockaddr_in listenAddr; 
   listenAddr.sin_family = AF_INET;
   listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   listenAddr.sin_port = htons(15000); 

   int listenfd = socket(PF_INET, SOCK_STREAM, 0);
   if (listenfd == -1) {
      printf("Create listen socket fail... errno: %d\n", errno);
      return false;
   }
   if (bind(listenfd, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) == -1) {
      printf("Bind listen socket fail... errno: %d\n", errno);
      return false;
   }
   if (listen(listenfd, MAX_CONN) == -1) {
      printf("Listen for listen socket fail... errno: %d\n", errno);
      return false;
   }
   printf("Listen fd: %d\n", listenfd);
   m_oListenWrapper = SockWrapper::ReuseOrNew(listenfd, true);

   struct epoll_event ev;
   epollfd = epoll_create1(0);
   if (epollfd == -1) {
      printf("Create epoll fail... errno: %d\n", errno);
      return false;
   }

   ev.events = EPOLLIN;
   // Caution! m_oListenWrapper already a ptr, do not use "&m_oListenWrapper"
   ev.data.ptr = m_oListenWrapper;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
      printf("Add listen socket to epoll fail... errno: %d\n", errno);
      return false;
   }
   return true;
}

bool CChatServer::Run() {
   int evc = 0;
   struct epoll_event ev, events[MAX_EVENT];
   struct sockaddr_in listenAddr; 
   int addrLen;
   int loopCount = 0;
   while (1) {
      loopCount++;
      evc = epoll_wait(epollfd, events, MAX_EVENT, 1000);
      if (evc == -1) {
            printf("Epoll wait return fail... errno: %d\n", errno);
            continue;
      }
      for (int i = 0; i < evc; i++) {
         printf("---------------------- loop count %d ------------------------\n", loopCount);
         printf("Epoll event %d, SockWrapper addr: %p, sockfd: %d\n", i, (SockWrapper*)events[i].data.ptr, ((SockWrapper*)events[i].data.ptr)->GetFd());
         if (events[i].data.ptr == m_oListenWrapper) {
            int insock = accept(m_oListenWrapper->GetFd(), (struct sockaddr *) &listenAddr, (socklen_t*)&addrLen);
            if (insock == -1) {
               printf("Accept coming sock fail... errno: %d\n", errno);
               continue;
            }
            printf("Accept coming sock, fd: %d\n", insock);
            ev.events = EPOLLIN;
            SockWrapper* sw = SockWrapper::ReuseOrNew(insock, false);
            // Caution! sw already a ptr, do not use "&sw"
            ev.data.ptr = sw;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, insock, &ev) == -1) {
               printf("Epoll add coming sock fail... errno: %d\n", errno);
            }
         } else {
            ((SockWrapper*)events[i].data.ptr)->OnRecv();
            // if (((SockWrapper*)events[i].data.ptr)->OnRecv() < 1) {
            //    SockWrapper* sw = (SockWrapper*)events[i].data.ptr;
            //    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, sw->GetFd(), &ev) == -1) {
            //       printf("Epoll remove sock fail... errno: %d\n", errno);
            //    }
            //    SockWrapper::Del(sw);
            // }
         }
      }
      SockWrapper::ClearInactive();
      Client::ClearUnbind();
   }
   return true;
}

bool CChatServer::Stop() {
   close(m_oListenWrapper->GetFd());
   return true;
}

}
