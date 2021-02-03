#pragma once
#include <cstdint>
#include <string>

namespace chat {

int32_t Gen32Uuid();
std::string GenAuth();
int32_t GenRoomUuid();
int32_t GetNowTime();

}