#pragma once
#include <cstdint>

namespace chat {

enum RecvStatus {
    WaitHeader,
    WaitBody
};

enum ConnStatus {
    Empty,
    Okay,
    Closed,
    Error
};

class Client;

class NetPack {
public:
    int protoId = 0;
    int len;
    char buffer[1024];
    Client *pClient;
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
        int OnRecv();
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
        bool handleAuth(NetPack *pPack);
        
    private:
        int fd = -1;
        RecvStatus recvStatus = WaitHeader;
        ConnStatus connStatus = Okay;
        Header header;
        bool authed = false;
        Client* client;
        char recvBuf[SockReadBufferLength] = {0};
        int recvLen = 0;
        char sendBuf[SockWriteBufferLength] = {0};
        int sendLen = 0;
};

}