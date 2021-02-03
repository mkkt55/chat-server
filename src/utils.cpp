#include "utils.h"
#include <ctime>

namespace chat {

int32_t Gen32Uuid() {
    static int32_t Last32Uuid = 10000;
    Last32Uuid++;
    return Last32Uuid;
}

std::string GenAuth() {
    static int32_t LastUuid = 999;
    LastUuid++;
    return "u" + std::to_string(LastUuid);
}

int32_t GenRoomUuid() {
    static int32_t LastUuid = 99;
    LastUuid++;
    return LastUuid;
}

int32_t GetNowTime() {
    return time(nullptr);
}

}