//
// Created by pedro-souza on 21/10/2025.
//

#ifndef ADDRESSES_H
#define ADDRESSES_H

#include "Platforms.h"
#include <cstdint>
#include <string>

namespace Addresses {
    void initAddrAnyV4(struct sockaddr_in *addr, uint16_t port);
    std::string getLanIpV4();
};

#endif //ADDRESSES_H