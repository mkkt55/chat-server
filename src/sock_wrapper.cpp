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

int SockWrapper::GetFd() {
    return fd;
}

bool SockWrapper::OnRecv() {
    int n = read(fd, recvBuf, 1024);
    printf("Read %d byte(s) from fd: %d, recvBuf: %s\n", n, fd, recvBuf);
    if (n == 0) {
        if (close(fd) == -1) {
            printf("Close socket %d error... errno: %d\n", fd, errno);
        }
        printf("Close socket %d\n", fd);
        return false;
    }
    if (n == -1) {
        printf("Read socket %d error... errno: %d\n", fd, errno);
        if (close(fd) == -1) {
            printf("Close socket %d error... errno: %d\n", fd, errno);
        }
        printf("Close socket %d\n", fd);
        return false;
    }

    recvLen += n;

    TryReadAndDeal();
    return true;
}

bool SockWrapper::TryReadAndDeal() {
    while (1) {
        printf("Loop, recvLen: %d, status: %d\n", recvLen, status);
        bool loop = false;
        switch (status)
        {
        case Empty:
        case WaitHeader:
            // Must be ">=" not ">" 
            if (recvLen >= sizeof(Header)) {
                parseHeader();
                status = WaitBody;
                loop = true;
            }
            break;
        case WaitBody:
            // Must be ">=" not ">" 
            if (recvLen >= sizeof(Header) + header.bodyLen) {
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

    printf("header flag: %d, protoId: %d, bodyLen: %d\n", header.flag, header.protoId, header.bodyLen);
    return true;
}

bool SockWrapper::dealOnePack() {
    LogicHandler* p = LogicHandler::Instance();
    NetPack pack;
    int len = header.bodyLen;
    pack.len = len;
    pack.protoId = header.protoId;
    memcpy(pack.buffer, recvBuf + sizeof(header), pack.len);

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
    printf("Handled one pack from fd: %d, left buffer length: %d\n", fd, recvLen);
    if (!p->HandlePack(&pack)) {
        printf("Fail to handle pack fd: %d\n", fd);
    }
    return true;
}

}