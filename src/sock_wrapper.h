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
        char flag;
        uint32_t protoId;
        uint32_t bodyLen;
};

const int HeaderLength = 9;
const int SockReadBufferLength = 1024;
const int SockWriteBufferLength = 1024;

class SockWrapper {
    public:
        SockWrapper(int fd);
        int GetFd();
        bool OnRecv();
        bool TryReadAndDeal();
        
        template <typename T>
        bool SendPack(char flag, T proto) {
            auto str = proto.SerializeAsString();
            SendPack(flag, proto.id(), str.size(), str.c_str());
        }
        bool SendPack(char flag, int protoId, int bodyLen, const char* body);
        void DebugInfo();
    private:
        bool parseHeader();
        bool dealOnePack();
        
    private:
        int fd = -1;
        RecvStatus status = Empty;
        Header header;
        char recvBuf[SockReadBufferLength] = {0};
        int recvLen = 0;
        char sendBuf[SockWriteBufferLength] = {0};
        int sendLen = 0;
};

}