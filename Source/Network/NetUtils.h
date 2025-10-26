//
// Created by pedro-souza on 21/10/2025.
//

#ifndef NETUTILS_H
#define NETUTILS_H

#include <cstdint>
#include <cstdlib>

namespace NetUtils {
    uint32_t generateNonce();
    uint16_t getNetChecksum(const void *data, size_t data_size);
};


#endif //NETUTILS_H