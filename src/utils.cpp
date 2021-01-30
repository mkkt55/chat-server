#include "utils.h"
#include <ctime>

namespace chat {

int32_t Gen32Uuid() {
    static int32_t Last32Uuid = 1000000;
    Last32Uuid++;
    return Last32Uuid;
}

int32_t GetNowTime() {
    return time(nullptr);
}

}