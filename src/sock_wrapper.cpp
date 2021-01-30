#include "sock_wrapper.h"
#include <unistd.h>
#include <cstdio>
#include <string>
#include <sys/errno.h>
#include <netinet/in.h>

#include "logic_handler.h"
#include "utils.h"
#include "cs.pb.h"
#include "client.h"

using namespace std;
using namespace main;

namespace chat {

std::vector<SockWrapper*> SockWrapper::s_vecSockWrapper;
int SockWrapper::curClearIndex = 0;

SockWrapper* SockWrapper::ReuseOrNew(int fd, bool isListenSock) {
    if (s_vecSockWrapper.size() < fd + 1) { // 这里曾忘记+1导致变core
        s_vecSockWrapper.resize(fd + 1, nullptr);
        printf("[SockWrapper] Resize fd %d -> %p\n", fd, s_vecSockWrapper[fd]);
    }
    if (s_vecSockWrapper[fd] == nullptr) {
        s_vecSockWrapper[fd] = new SockWrapper(fd, isListenSock);
        printf("[SockWrapper] New fd %d\n", fd);
    }
    else {
        printf("[SockWrapper] Reuse fd %d\n", fd);
        if (s_vecSockWrapper[fd]->connStatus == InUse) {
            printf("[Error] %s, %s, %s Reuse SockWrapper InUse\n", __FILE__, __LINE__, __FUNCTION__);
        }
    }
    s_vecSockWrapper[fd]->onNewOrReuse();
    return s_vecSockWrapper[fd];
}

bool SockWrapper::onNewOrReuse() {
    connStatus = InUse;
    recvStatus = WaitHeader;
    authed = false;
    client = nullptr;
    recvLen = 0;
    lastActiveTime = GetNowTime();
    return true;
}

bool SockWrapper::SafeCloseAndWaitReuse(SockWrapper* sw) {
    if (sw->connStatus == Closed || sw->connStatus == Error) {
        printf("[ERROR] %s, %s, %s, Recv after close, sock fd %d", __FILE__, __LINE__, __FUNCTION__, sw->fd);
        return false;
    }
    sw->onCloseOrError(Closed);
    printf("[SockWrapper] All: ");
    for (int i = 0; i < s_vecSockWrapper.size(); i++) {
        printf("%d=%p ", i, s_vecSockWrapper[i]);
    }
    printf("\n");
    return true;
}

int SockWrapper::ClearInactive() {
    int nNow = GetNowTime();
    for (int i = 0; i < s_vecSockWrapper.size(); i++) {
        curClearIndex++;
        if (curClearIndex >= s_vecSockWrapper.size()) {
            curClearIndex = 0;
        }
        if (s_vecSockWrapper[curClearIndex] == nullptr) {
            continue;
        }
        auto *p = s_vecSockWrapper[curClearIndex];
        if (p->connStatus != InUse || p->isListenSock) {
            continue;
        }
        if (nNow - p->lastActiveTime > 60) {
            printf("[SockWrapper] Clear inactive socket，fd %d\n", p->fd);
            SafeCloseAndWaitReuse(p);
        }
        // 找到一个活跃的就等下个Frame再清理之后的，现在为了测试不这么做
        else {
        //     break;
        }
    }
    return 0;
}

void printBuffer(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
}

SockWrapper::SockWrapper(int fd, bool isListenSock) {
    this->fd = fd;
    this->isListenSock = isListenSock;
}

SockWrapper::~SockWrapper() {}

int SockWrapper::GetFd() {
    return fd;
}

bool SockWrapper::onCloseOrError(ConnStatus status) {
    connStatus = status;
    if (close(fd) == -1) {
        printf("Close socket %d error... errno: %d\n", fd, errno);
        connStatus = Error;
    }
    printf("Close SocketConn for fd %d, status %d\n", fd, connStatus);
    Client::UnbindConn(client, this);
    client = nullptr;
    return true;
}

int SockWrapper::OnRecv() {
    if (connStatus == Closed || connStatus == Error) {
        printf("[ERROR] %s, %s, %s, Recv after close, sock fd %d", __FILE__, __LINE__, __FUNCTION__, fd);
        return false;
    }
    updateLastActiveTime();
    int n = read(fd, recvBuf + recvLen, SockReadBufferLength);
    // printf("Read %d byte(s) from fd: %d, recvBuf length: %d\n", n, fd, strlen(recvBuf));
    // printf("RecvBuf: ");
    // printBuffer(recvBuf, 64);
    if (n == 0) {
        onCloseOrError(Closed);
        return n;
    }
    if (n == -1) {
        printf("Read socket %d error... errno: %d\n", fd, errno);
        onCloseOrError(Error);
        return n;
    }

    recvLen += n;

    tryReadAndDeal();
    return n;
}

bool SockWrapper::tryReadAndDeal() {
    while (1) {
        // printf("Loop, recvLen: %d, recvStatus: %d\n", recvLen, recvStatus);
        bool loop = false;
        switch (recvStatus)
        {
        case WaitHeader:
            // Must be ">=" not ">" 
            if (recvLen >= HeaderLength) {
                parseHeader();
                recvStatus = WaitBody;
                loop = true;
            }
            break;
        case WaitBody:
            // Must be ">=" not ">" 
            if (recvLen >= HeaderLength + header.bodyLen) {
                dealOnePack();
                recvStatus = WaitHeader;
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
    if (connStatus == Closed || connStatus == Error) {
        printf("[ERROR] %s, %s, %s, Recv after close, sock fd %d", __FILE__, __LINE__, __FUNCTION__, fd);
        return false;
    }
    char header[HeaderLength];
    header[0] = flag;

    *((uint32_t*)(header + 1)) = htonl(protoId);
    *((uint32_t*)(header + 5)) = htonl(bodyLen);
    int nh = send(fd, header, HeaderLength, 0);
    if (-1 == nh) {
        printf("Send proto pack to sock %d FAIL... errno: %d\n", fd, errno);
        onCloseOrError(Error);
        return false;
    }
    int nb = send(fd, body, bodyLen, 0);
    if (-1 == nb) {
        printf("Send proto pack to sock %d FAIL... errno: %d\n", fd, errno);
        onCloseOrError(Error);
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
    NetPack pack;
    int bodyLen = header.bodyLen;
    pack.len = bodyLen;
    pack.protoId = header.protoId;
    pack.pClient = client;
    memcpy(pack.buffer, recvBuf + HeaderLength, pack.len);

    char *src = recvBuf + HeaderLength + bodyLen;
    char *dest = recvBuf;
    int copyLen = recvLen - HeaderLength - bodyLen;
    // printf("Copy forward: %d bytes from read buffer %p to read buffer %p\n", copyLen, dest, src);
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

    if (authed) {
        if (!LogicHandler::Instance()->HandlePack(&pack)) {
            printf("Fail to handle pack fd: %d\n", fd);
        }
    }
    else {
        handleAuth(&pack);
    }
    
    return true;
}

bool SockWrapper::handleAuth(NetPack *pPack) {
    if (connStatus == Closed || connStatus == Error) {
        return false;
    }
    if (pPack->protoId != main::login_req_id) {
        return false;
    }
    login_req req;
    login_resp ack;
    if (!req.ParseFromArray(pPack->buffer, pPack->len)) {
        ack.set_error(err_parsing_proto);
        SendPack<login_resp>(12, ack);
        printf("Auth FAIL, conn fd: %d\n", fd);
        return false;
    }

    string auth;
    if (!req.auth().empty()) {
        auth = req.auth();
    }
    else {
        auth = to_string(Gen32Uuid());
        printf("new auth: %s\n", auth.c_str());
        ack.set_auth(auth);
    }
    ack.set_error(err_none);
    authed = true;
    printf("Auth complete, start to bind conn fd: %d\n", fd);
    client = Client::BindOneAndRet(auth, this);
    printf("Bind fd: %d to Client success\n", fd);

    SendPack<login_resp>(12, ack);
    printf("Handle auth OK, req pack: %s\n", req.DebugString().c_str());
    printf("Handle auth OK, ack pack: %s\n", ack.DebugString().c_str());
    
    return true;
}


bool SockWrapper::updateLastActiveTime() {
    lastActiveTime = GetNowTime();
    return true;
}

}