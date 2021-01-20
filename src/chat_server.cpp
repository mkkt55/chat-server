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
   bind(listenfd, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
   if (listen(listenfd, 10) != 0) {
      return false;
   }
   return true;
}

bool CChatServer::Run() {
   while(1)
   {
      char receiveBuff[1024];
      char sendBuff[1024];
      int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
      snprintf(sendBuff, sizeof(sendBuff), "Hello, your file descriptor: ", connfd, "\n");
      write(connfd, sendBuff, strlen(sendBuff));
      std::cout << sendBuff << std::endl;
      if (read(connfd, receiveBuff, 1024) != -1) {
         std::cout << "From socket: " << connfd << " info: " << receiveBuff << std::endl;
         write(connfd, receiveBuff, strlen(sendBuff));
      }

      close(connfd);
      sleep(1);
   }
   return true;
}

bool CChatServer::Stop() {
   close(listenfd);
   return true;
}

}
