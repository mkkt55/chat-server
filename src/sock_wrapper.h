#pragma once
#include <cstdint>

namespace chat {

enum RecvStatus {
    Empty,
    WaitHeader,
    WaitBody,
    Closed
};

class Header {
    public:
        int32_t flag;
        int32_t protoId;
        int32_t bodyLen;
};

class SockWrapper {
    public:
        SockWrapper(int fd);
        int GetFd();
        bool OnRecv();
        bool TryReadAndDeal();
        bool WritePack();
        void DebugInfo();
    private:
        bool parseHeader();
        bool dealOnePack();
        
    private:
        int fd = -1;
        RecvStatus status = Empty;
        Header header;
        char recvBuf[1024] = {0};
        int recvLen = 0;
        char sendBuf[1024] = {0};
        int sendLen = 0;
};

}