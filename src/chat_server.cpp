#include "chat_server.h"
#include <sys/errno.h>
#include <sys/socket.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

#include "sock_wrapper.h"
#include "client.h"

using namespace std;

namespace chat {

void CChatServer::PrintInfo() {
   std::cout << "Hello" <<std::endl;
}

bool CChatServer::Init(int port /*= 15000*/) {
   if (m_bHasInit) {
      return false;
   }
   struct sockaddr_in listenAddr; 
   listenAddr.sin_family = AF_INET;
   listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   listenAddr.sin_port = htons(port); 

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
   m_bHasInit = true;
   return true;
}

bool CChatServer::Run() {
   if (!m_bHasInit) {
      Init();
   }
   loopCount++;
   evc = epoll_wait(epollfd, events, MAX_EVENT, 1000);
   if (evc == -1 && errno != EINTR) {
      printf("Epoll wait return fail... errno: %d\n", errno);
      return false;
   }
   for (int i = 0; i < evc; i++) {
      printf("---------------------- loop count %d ------------------------\n", loopCount);
      printf("Epoll event %d, SockWrapper addr: %p, sockfd: %d\n", i, (SockWrapper*)events[i].data.ptr, ((SockWrapper*)events[i].data.ptr)->GetFd());
      if (events[i].data.ptr == m_oListenWrapper) {
         int insock = accept(m_oListenWrapper->GetFd(), (struct sockaddr *) &listenAddr, (socklen_t*)&addrLen);
         if (insock == -1) {
            printf("Accept coming sock fail... errno: %d\n", errno);
            return false;
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
   return true;
}

bool CChatServer::Stop() {
   printf("\nServer start terminating...\n");
   int count = SockWrapper::ClearOnTermination();
   printf("Server terminated, close sock count: %d, listen sock counted.\nGoodBye~\n", count);
   return true;
}

}
