//
// Created by pedro-souza on 12/10/2025.
//

#ifndef NETUTILS_H
#define NETUTILS_H

#include "Platforms.h"

uint32_t generate_nonce();
uint16_t get_net_checksum(const void *data, size_t data_size);

#endif //NETUTILS_H