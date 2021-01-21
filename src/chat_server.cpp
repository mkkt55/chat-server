#include "chat_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<cstring>

#include "cs.pb.h"

using namespace std;

const int MAX_CONN = 10;

namespace chat {

void CChatServer::PrintInfo() {
   std::cout << "Hello" <<std::endl;
}

bool CChatServer::Init() {
   struct sockaddr_in listenAddr; 
   listenAddr.sin_family = AF_INET;
   listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   listenAddr.sin_port = htons(15000); 

   listenfd = socket(PF_INET, SOCK_STREAM, 0);
   if (listenfd == -1) {
      cout << "Create listen socket fail...\n";
      return false;
   }
   if (bind(listenfd, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) == -1) {
      cout << "Create listen socket fail...\n";
      return false;
   }
   if (listen(listenfd, MAX_CONN) == -1) {
      return false;
   }
   return true;
}

bool CChatServer::Run() {
   while(1)
   {
      char recvBuf[1024] = {0};
      char sendBuf[1024] = {0};
      struct sockaddr_in addr;
      int len;
      int connfd = accept(listenfd, (struct sockaddr*)&addr, (socklen_t*)&len);
      if (connfd == -1) {
         std::cout << "Accept socket fail..." << std::endl;
         continue;
      }
      std::cout << "Accept socket, fd: " << connfd << std::endl;
      while (int n = read(connfd, recvBuf, 1024) != 0) {
         // printf("Receive from socket: %d, length: %d byte(s), content:%s\n", connfd, n, recvBuf);
         if (n == -1) {
            std::cout << "Read socket error..." << std::endl;
            if (close(connfd) == -1) {
               std::cout << "Close socket error, fd: " << connfd << "..." << std::endl;
            }
            continue;
         }
      }
      main::login_req loginReq;
      loginReq.set_id(main::login_resp_id);
      if (!loginReq.ParseFromArray(recvBuf, strlen(recvBuf))) {
         std::cout << "Parse proto fail...";
      }
      // snprintf(sendBuf, sizeof(sendBuf), "Hello, your file descriptor: ", connfd, "\n");
      // write(connfd, sendBuf, strlen(sendBuf));
      // std::cout << sendBuf << std::endl;
      std::cout << "Get pack id: " << loginReq.id() << "\n";
      if (close(connfd) == -1) {
         std::cout << "Close socket error, fd: " << connfd << "..." << std::endl;
      }
   }
   return true;
}

bool CChatServer::Stop() {
   close(listenfd);
   return true;
}

}
