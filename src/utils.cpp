#include "utils.h"

namespace chat {

int Gen32Uuid() {
    static int32_t Last32Uuid = 1000000;
    Last32Uuid++;
    return Last32Uuid;
}

}