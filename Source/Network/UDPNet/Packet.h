#ifndef PACKET_H
#define PACKET_H

#include "Platforms.h"

#define PACKET_HEADER_BYTES 20
#define MAX_PACKET_DATA_BYTES 1024
#define PACKET_SYNC_BYTES 0x554E4554
#define MAX_PACKET_DATA_STRING_BYTES 256

#define SYN_FLAG 0x01
#define SYN_ACK_FLAG 0x02
#define ACK_FLAG 0x03
#define DATA_FLAG 0x04
#define END_FLAG 0x05
#define RST_FLAG 0x06

#define PACKET_HOLD 1
#define PACKET_READY 2

#pragma pack(1)
typedef struct {
    uint32_t sync_1;
    uint32_t sync_2;
    uint16_t sequence;
    uint8_t flag;
    uint32_t nonce;
    uint8_t state;
    uint16_t length;
    uint16_t checksum;
    char data[MAX_PACKET_DATA_BYTES];
} UdpNetPacket;
#pragma pack(0)

typedef enum {
    UINT32_TYPE = 0,
    INT32_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    BOOL_TYPE,
    STRING_TYPE
} DataType;

typedef struct {
    DataType type;
    union {
        uint32_t uint32_value;
        int32_t  int32_value;
        float float_value;
        double double_value;
        uint8_t bool_value;
        char string_value[MAX_PACKET_DATA_STRING_BYTES];
    } value;
} UdpNetPacketDataField;

// init the packet header values
void init_packet(UdpNetPacket *p, uint16_t sequence, uint8_t flag, uint32_t nonce);

// add the informed data type to the packet
void add_uint32_to_packet(UdpNetPacket *p, uint32_t value);
void add_int32_to_packet(UdpNetPacket *p, int32_t value);
void add_float_to_packet(UdpNetPacket *p, float value);
void add_double_to_packet(UdpNetPacket *p, double value);
void add_bool_to_packet(UdpNetPacket *p, uint8_t value);
void add_string_to_packet(UdpNetPacket *p, const char *value);

// set the packet fields to network order and calculate the checksum
void build_packet(UdpNetPacket *p);

// check if a build packet is valid or not
int is_valid_packet(UdpNetPacket *p);

// print the packet header and data fields
void print_packet(UdpNetPacket *p);

#endif //PACKET_H