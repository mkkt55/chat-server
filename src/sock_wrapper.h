#pragma once
#include <cstdint>
#include <set>
#include <vector>

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
    private:
        static std::set<SockWrapper*> s_setInSockWrapper;
        static std::set<SockWrapper*> s_setSockWaitDel;
    public:
        static SockWrapper* New(int );
        static bool Del(SockWrapper* sw);
        static int Clear();
    private:
        SockWrapper(int fd);
        ~SockWrapper();
    public:
        int GetFd();
        int OnRecv();
        
        template <typename T>
        bool SendPack(char flag, T proto) {
            auto str = proto.SerializeAsString();
            SendPack(flag, proto.id(), str.size(), str.c_str());
        }
        bool SendPack(char flag, int protoId, int bodyLen, const char* body);
        void DebugInfo();
    private:
        bool tryReadAndDeal();
        bool parseHeader();
        bool dealOnePack();
        bool handleAuth(NetPack *pPack);
        
    private:
        int fd = -1;
        RecvStatus recvStatus = WaitHeader;
        ConnStatus connStatus = Okay;
        Header header;
        bool authed = false;
        Client* client = nullptr;
        char recvBuf[SockReadBufferLength] = {0};
        int recvLen = 0;
        char sendBuf[SockWriteBufferLength] = {0};
        int sendLen = 0;
};

}