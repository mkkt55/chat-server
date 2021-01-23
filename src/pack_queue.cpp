#include "pack_queue.h"

namespace chat {
int PackQueue::PushPack(NetPack pack) {
    s_quePack.push(pack);
    return s_quePack.size();
}
int PackQueue::PopPack(NetPack &pack) {
    NetPack tmp = s_quePack.front();
    pack = tmp;
    s_quePack.pop();
    return s_quePack.size();
}

std::queue<NetPack> PackQueue::s_quePack;
} // namespace chat