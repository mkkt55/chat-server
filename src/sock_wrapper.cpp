#include "sock_wrapper.h"
#include <unistd.h>
#include <cstdio>
#include <string>
#include <sys/errno.h>
#include <netinet/in.h>

#include "logic_handler.h"

using namespace std;

namespace chat {

void printBuffer(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
}

SockWrapper::SockWrapper(int _fd) {
    fd = _fd;
}

int SockWrapper::GetFd() {
    return fd;
}

bool SockWrapper::OnRecv() {
    int n = read(fd, recvBuf + recvLen, SockReadBufferLength);
    printf("Read %d byte(s) from fd: %d, recvBuf length: %d\n", n, fd, strlen(recvBuf));
    printf("RecvBuf: ");
    printBuffer(recvBuf, 64);
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
            if (recvLen >= HeaderLength) {
                parseHeader();
                status = WaitBody;
                loop = true;
            }
            break;
        case WaitBody:
            // Must be ">=" not ">" 
            if (recvLen >= HeaderLength + header.bodyLen) {
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

bool SockWrapper::SendPack(char flag, int protoId, int bodyLen, const char* body) {
    char header[HeaderLength];
    header[0] = flag;

    *((uint32_t*)(header + 1)) = htonl(protoId);
    *((uint32_t*)(header + 5)) = htonl(bodyLen);
    int nh = send(fd, header, HeaderLength, 0);
    if (-1 == nh) {
        printf("Send proto pack to sock %d FAIL... errno: %d\n", fd, errno);
        return false;
    }
    int nb = send(fd, body, bodyLen, 0);
    if (-1 == nb) {
        printf("Send proto pack to sock %d FAIL... errno: %d\n", fd, errno);
        return false;
    }
    printf("Send Proto pack OK, %d byte(s) header, %d byte(s) body, into fd %d\n", nh, nb, fd);
    return true;
}

bool SockWrapper::parseHeader() {
    header.flag = *recvBuf;
    uint32_t *p = (uint32_t*)(recvBuf + 1);
    header.protoId = ntohl(*p);
    p++;
    header.bodyLen = ntohl(*p);

    printf("header flag: %d, protoId: %d, bodyLen: %d\n", header.flag, header.protoId, header.bodyLen);
    return true;
}

bool SockWrapper::dealOnePack() {
    LogicHandler* p = LogicHandler::Instance();
    NetPack pack;
    int bodyLen = header.bodyLen;
    pack.len = bodyLen;
    pack.protoId = header.protoId;
    pack.pConn = this;
    memcpy(pack.buffer, recvBuf + HeaderLength, pack.len);

    char *src = recvBuf + HeaderLength + bodyLen;
    char *dest = recvBuf;
    int copyLen = recvLen - HeaderLength - bodyLen;
    printf("Copy forward: %d bytes from read buffer %p to read buffer %p\n", copyLen, dest, src);
    while (copyLen > 0)
    {
        *dest = *src;
        dest++;
        src++;
        copyLen--;
    }
    *dest = '\0';
    
    recvLen -= HeaderLength + bodyLen;
    printf("Handled one pack from fd: %d, left buffer length: %d\n", fd, recvLen);
    if (!p->HandlePack(&pack)) {
        printf("Fail to handle pack fd: %d\n", fd);
    }
    return true;
}

}