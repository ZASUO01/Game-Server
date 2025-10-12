//
// Created by pedro-souza on 12/10/2025.
//

#ifndef ADDRESSES_H
#define ADDRESSES_H

#include "Platforms.h"

void init_any_addr_v4(struct sockaddr_in *addr, uint16_t port);
char *get_lan_ip_v4();

#endif //ADDRESSES_H