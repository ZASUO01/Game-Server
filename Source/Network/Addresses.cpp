//
// Created by pedro-souza on 21/10/2025.
//

#include "Addresses.h"
#include <vector>
#include <system_error>

void Addresses::initAddrAnyV4(struct sockaddr_in *addr, const uint16_t port) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

static std::string getPublicIpByDnsV4(const std::string& dns_ip) {
    const int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return "";
    }


    // Configura timeout
    constexpr struct timeval timeout = {1, 0};
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0 ||
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            return "";
        }

    struct sockaddr_in dns_addr{};
    dns_addr.sin_family = AF_INET;
    dns_addr.sin_port = htons(53);

    if (inet_pton(AF_INET, dns_ip.c_str(), &dns_addr.sin_addr) <= 0) {
        return "";
    }

    if (connect(sock, reinterpret_cast<struct sockaddr*>(&dns_addr), sizeof(dns_addr)) < 0) {
        return "";
    }

    struct sockaddr_in local_addr{};
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sock, reinterpret_cast<struct sockaddr*>(&local_addr), &addr_len) < 0) {
        return "";
    }

    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &local_addr.sin_addr, ip_str, INET_ADDRSTRLEN) == nullptr) {
        throw std::system_error(errno, std::system_category(), "inet_ntop failed");
    }

    return (ip_str);
}

std::string Addresses::getLanIpV4() {
    static const std::vector<std::string> dnsServers = {
        "8.8.8.8",      // Google
        "1.1.1.1",      // Cloudflare
        "9.9.9.9",      // Quad9
        "208.67.222.222" // OpenDNS
    };

    for (const auto& dnsServer : dnsServers) {
        std::string ip_str;
        if (ip_str = getPublicIpByDnsV4(dnsServer); !ip_str.empty()) {
           return ip_str;
       }
    }

    return "";
}

