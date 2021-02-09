#pragma once
#include <cstdint>
#include <set>
#include <vector>
#include <unordered_map>

namespace chat {

enum RecvStatus {
    WaitHeader,
    WaitBody
};

enum ConnStatus {
    InUse,
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

struct NetHeader {
    char flag;
    uint32_t protoId;
    uint32_t bodyLen;
};

const int HeaderLength = 9;
const int SockReadBufferLength = 1024;
const int SockWriteBufferLength = 1024;

class SockWrapper {
    // Static members and functions, used as Mgr and factory
    private:
        static std::vector<SockWrapper*> s_vecSockWrapper;
        static int curClearIndex;
    public:
        static SockWrapper* ReuseOrNew(int fd, bool isListenSock);
        static bool SafeCloseAndWaitReuse(SockWrapper* sw);
        static int ClearInactive();
        static int ClearOnTermination();
    private:
        SockWrapper(int fd, bool isListenSock);
        ~SockWrapper();
    
    // Read stream and form NetPack to logic
    public:
        int GetFd();
        int OnRecv();
        template <typename T>
        bool SendPack(char flag, T proto) {
            auto str = proto.SerializeAsString();
            return SendPack(flag, proto.id(), str.size(), str.c_str());
        }
        bool SendPack(char flag, int protoId, int bodyLen, const char* body);
    private:
        bool tryReadAndDeal();
        bool parseHeader();
        bool dealOnePack();
    private:
        int fd = -1;
        RecvStatus recvStatus = WaitHeader;
        NetHeader header;
        char recvBuf[SockReadBufferLength] = {0};
        int recvLen = 0;

    // Bind with client
    private:
        bool handleAuth(NetPack *pPack);
    private:
        bool authed = false;
        Client* client = nullptr;
    
    // On state change
    private:
        bool onNewOrReuse();
        bool onCloseOrError(ConnStatus);
        ConnStatus connStatus = InUse;
    
    // Flag for clear inactive
    private:
        bool updateLastActiveTime();
        int32_t lastActiveTime = 0;
        bool isListenSock = false;

    // Debug info print
    public:
        void DebugInfo();
};

}