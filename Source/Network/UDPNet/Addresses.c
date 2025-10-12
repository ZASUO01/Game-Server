//
// Created by pedro-souza on 12/10/2025.
//

#include "Addresses.h"

static char *get_public_ip_by_dns_v4(const char *dns_ip);

void init_any_addr_v4(struct sockaddr_in *addr, uint16_t port){
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

char *get_lan_ip_v4(){
    static const char *dns_servers[] = {
        "8.8.8.8",      // Google
        "1.1.1.1",      // Cloudflare
        "9.9.9.9",      // Quad9
        "208.67.222.222" // OpenDNS
    };

    int num_servers = 4;

    for(int i = 0; i < num_servers; i++){
        char *ip = get_public_ip_by_dns_v4(dns_servers[i]);
        if(ip != NULL){
            return ip;
        }
    }

    return NULL;
}

static char *get_public_ip_by_dns_v4(const char *dns_ip){
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0){
        return NULL;
    }

    struct timeval timeout = {1, 0};
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in dns_addr;
    dns_addr.sin_family = AF_INET;
    dns_addr.sin_port = htons(53);

    if(inet_pton(AF_INET, dns_ip, &dns_addr.sin_addr) <= 0){
        close(sock_fd);
        return NULL;
    }

    if (connect(sock_fd, (struct sockaddr*)&dns_addr, sizeof(dns_addr)) < 0) {
        close(sock_fd);
        return NULL;
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sock_fd, (struct sockaddr*)&local_addr, &addr_len) < 0) {
        close(sock_fd);
        return NULL;
    }

    char *ip = malloc(INET_ADDRSTRLEN);
    if (ip == NULL) {
        close(sock_fd);
        return NULL;
    }

    if (inet_ntop(AF_INET, &local_addr.sin_addr, ip, INET_ADDRSTRLEN) == NULL) {
        free(ip);
        close(sock_fd);
        return NULL;
    }

    close(sock_fd);
    return ip;
}