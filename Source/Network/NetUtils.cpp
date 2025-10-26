//
// Created by pedro-souza on 21/10/2025.
//

#include "NetUtils.h"
#include <ctime>
#include <cstring>

uint32_t NetUtils::generateNonce() {
    const auto timestamp = static_cast<uint32_t>(time(nullptr));
    const uint32_t random_val = rand() & 0xFFFF;
    return (timestamp << 16) | random_val;
}

uint16_t NetUtils::getNetChecksum(const void *data, const size_t data_size){
    const auto *buf = static_cast<const uint8_t *>(data);
    uint32_t sum = 0;

    for (size_t i = 0; i + 1 < data_size; i += 2) {
        uint16_t word;
        memcpy(&word, buf + i, sizeof(uint16_t));
        sum += word;
    }

    if(data_size % 2 != 0){
        sum += static_cast<uint16_t>(buf[data_size - 1] << 8);
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return static_cast<uint16_t>(~sum);
}