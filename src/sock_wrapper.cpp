#include "sock_wrapper.h"
#include <unistd.h>
#include <cstdio>
#include <sys/errno.h>
#include <netinet/in.h>

#include "logic_handler.h"

using namespace std;

namespace chat {

SockWrapper::SockWrapper(int _fd) {
    fd = _fd;
}

bool SockWrapper::OnRecv() {
    int n = read(fd, recvBuf, 1024);
    if (n == 0) {
        if (close(fd) == -1) {
            printf("Close socket %d error... errno: %d", fd, errno);
        }
        return false;
    }
    if (n == -1) {
        printf("Read socket %d error... errno: %d", fd, errno);
        if (close(fd) == -1) {
            printf("Close socket %d error... errno: %d", fd, errno);
        }
        return false;
    }

    recvLen += n;

    TryReadAndDeal();
    return true;
}

bool SockWrapper::TryReadAndDeal() {
    while (1) {
        bool loop = false;
        switch (status)
        {
        case Empty:
        case WaitHeader:
            if (recvLen > sizeof(Header)) {
                parseHeader();
                status = WaitBody;
                loop = true;
            }
            break;
        case WaitBody:
            if (recvLen > sizeof(Header) + header.bodyLen) {
                dealOnePack();
                status = WaitHeader;
                loop = true;
            }
        default:
            break;
        }
        if (!loop) {
            break;
        }
    }

    
    // main::login_req loginReq;
    // loginReq.set_id(main::login_resp_id);
    // if (!loginReq.ParseFromArray(recvBuf, strlen(recvBuf))) {
    //     std::cout << "Parse proto fail...";
    // }
    // // snprintf(sendBuf, sizeof(sendBuf), "Hello, your file descriptor: ", connfd, "\n");
    // // write(connfd, sendBuf, strlen(sendBuf));
    // // std::cout << sendBuf << std::endl;
    // std::cout << "Get pack id: " << loginReq.id() << "\n";
    return true;
}

bool SockWrapper::WritePack() {
    return true;
}

bool SockWrapper::parseHeader() {
    int32_t *p = (int32_t*)recvBuf;
    header.flag = ntohl(*p);
    p++;
    header.protoId = ntohl(*p);
    p++;
    header.bodyLen = ntohl(*p);

    printf("header flag: %d, protoId: %d, bodyLen: %d", header.flag, header.protoId, header.bodyLen);
    return true;
}

bool SockWrapper::dealOnePack() {
    LogicHandler* p = LogicHandler::Instance();
    NetPack pack;
    int len = header.bodyLen;
    pack.len = len;
    pack.protoId = header.protoId;
    memcpy(pack.buffer, recvBuf + sizeof(header), pack.len);
    p->HandlePack(&pack);

    char *src = recvBuf + sizeof(header) + len;
    char *dest = recvBuf;
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    
    recvLen -= sizeof(header) + len;
    return true;
}

}