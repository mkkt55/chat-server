#pragma once
#include<queue>
#include "sock_wrapper.h"

namespace chat
{
class NetPack {
public:
    int protoId = 0;
    int len;
    char buffer[1024];
    SockWrapper *pConn;
};

class PackQueue {
private:
    PackQueue();
public:
    static int PushPack(NetPack pack);
    static int PopPack(NetPack &pack);
private:
    static std::queue<NetPack> s_quePack;
};

} // namespace chat
