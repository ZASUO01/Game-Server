//
// Created by pedro-souza on 12/10/2025.
//
#include "Packet.h"
#include "NetUtils.h"

static size_t get_field_size(UdpNetPacketDataField *pf);
static void add_field_to_packet(UdpNetPacket *p, UdpNetPacketDataField *pf);

void init_packet(UdpNetPacket *p, uint16_t sequence, uint8_t flag, uint32_t nonce){
    p->sync_1 = PACKET_SYNC_BYTES;
    p->sync_2 = PACKET_SYNC_BYTES;
    p->state = PACKET_HOLD;
    p->sequence = sequence;
    p->flag = flag;
    p->nonce = nonce;
    p->length = 0;
    p->checksum = 0;
    memset(p->data, 0, MAX_PACKET_DATA_BYTES);
}

void add_uint32_to_packet(UdpNetPacket *p, uint32_t value){
    UdpNetPacketDataField pf;
    pf.type = UINT32_TYPE;
    pf.value.uint32_value = value;
    add_field_to_packet(p, &pf);
}

void add_int32_to_packet(UdpNetPacket *p, int32_t value){
    UdpNetPacketDataField pf;
    pf.type = INT32_TYPE;
    pf.value.int32_value = value;
    add_field_to_packet(p, &pf);
}

void add_float_to_packet(UdpNetPacket *p, float value){
    UdpNetPacketDataField pf;
    pf.type = FLOAT_TYPE;
    pf.value.float_value = value;
    add_field_to_packet(p, &pf);
}

void add_double_to_packet(UdpNetPacket *p, double value){
    UdpNetPacketDataField pf;
    pf.type = DOUBLE_TYPE;
    pf.value.double_value = value;
    add_field_to_packet(p, &pf);
}

void add_bool_to_packet(UdpNetPacket *p, uint8_t value){
    UdpNetPacketDataField pf;
    pf.type = BOOL_TYPE;
    pf.value.bool_value = value;
    add_field_to_packet(p, &pf);
}

void add_string_to_packet(UdpNetPacket *p, const char *value){
    UdpNetPacketDataField pf;
    pf.type = STRING_TYPE;
    strncpy(pf.value.string_value, value, MAX_PACKET_DATA_STRING_BYTES - 1);
    pf.value.string_value[MAX_PACKET_DATA_STRING_BYTES - 1] = '\0';
    add_field_to_packet(p, &pf);
}

void build_packet(UdpNetPacket *p){
    if(p->state == PACKET_HOLD){
        p->state = PACKET_READY;

        p->sync_1 = htonl(PACKET_SYNC_BYTES);
        p->sync_2 = htonl(PACKET_SYNC_BYTES);

        p->sequence = htons(p->sequence);
        p->nonce = htonl(p->nonce);

        uint16_t _length = p->length;
        p->length = htons(p->length);

        size_t packet_size = PACKET_HEADER_BYTES + _length;
        p->checksum = get_net_checksum(p, packet_size);
        p->checksum = htons(p->checksum);
    }
}

int is_valid_packet(UdpNetPacket *p){
    if(p->state != PACKET_READY){
        return -1;
    }

    if(p->sync_1 != htonl(PACKET_SYNC_BYTES) ||
       p->sync_2 != htonl(PACKET_SYNC_BYTES)) {
        return -1;
    }

    uint16_t _length = ntohs(p->length);
    if(_length > MAX_PACKET_DATA_BYTES){
        return -1;
    }

    UdpNetPacket temp;
    memcpy(&temp, p, sizeof(UdpNetPacket));
    temp.checksum = 0;

    size_t packet_size = PACKET_HEADER_BYTES + _length;
    uint16_t calculated_checksum = get_net_checksum(&temp, packet_size);

    if(calculated_checksum != ntohs(p->checksum)){
        return -1;
    }

    return 0;
}

void print_packet(UdpNetPacket *p){
    if(p->state == PACKET_READY){
        uint16_t _length = ntohs(p->length);

        printf("==== PACKET HEADER ====\n");
        printf("SEQUENCE: %hu\n", ntohs(p->sequence));
        printf("FLAG:   %hhu\n", p->flag);
        printf("NONCE:     %u\n", ntohl(p->nonce));
        printf("LENGTH: %hu\n", _length);
        printf("==== CONTROL FIELDS ====\n");
        printf("SYNC1:    %#x\n", ntohl(p->sync_1));
        printf("SYNC2:    %#x\n", ntohl(p->sync_2));
        printf("CHECKSUM: %#x\n", ntohl(p->checksum));
        if(_length > 0){
            printf("==== DATA FIELDS ====\n");
            size_t offset = 0;

            while(offset < _length){
                DataType type;
                memcpy(&type, p->data + offset, sizeof(DataType));
                offset += sizeof(DataType);

                switch (type){
                    case UINT32_TYPE:
                        uint32_t uint32_val;
                        memcpy(&uint32_val, p->data + offset, sizeof(uint32_t));
                        printf("UINT32: %u\n", uint32_val);
                        offset += sizeof(uint32_t);
                        break;
                    case INT32_TYPE:
                        int32_t int32_val;
                        memcpy(&int32_val, p->data + offset, sizeof(int32_t));
                        printf("INT32: %d\n", int32_val);
                        offset += sizeof(int32_t);
                        break;
                    case FLOAT_TYPE:
                        float float_val;
                        memcpy(&float_val, p->data + offset, sizeof(float));
                        printf("FLOAT: %f\n", float_val);
                        offset += sizeof(float);
                        break;
                    case DOUBLE_TYPE:
                        double double_val;
                        memcpy(&double_val, p->data + offset, sizeof(double));
                        printf("DOUBLE: %f\n", double_val);
                        offset += sizeof(double);
                        break;
                    case BOOL_TYPE:
                        uint8_t bool_val;
                        memcpy(&bool_val, p->data + offset, sizeof(uint8_t));
                        printf("BOOL: %hhu\n", bool_val);
                        offset += sizeof(uint8_t);
                        break;
                    case STRING_TYPE:
                        char *str_val = malloc(strlen(p->data + offset) + 1);
                        strcpy(str_val, p->data + offset);
                        printf("%s\n", str_val);
                        offset += strlen(p->data + offset) + 1;
                        free(str_val);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

static size_t get_field_size(UdpNetPacketDataField *pf){
    switch (pf->type){
        case UINT32_TYPE:
        case INT32_TYPE:
            return sizeof(uint32_t);
        case FLOAT_TYPE:
            return sizeof(float);
        case DOUBLE_TYPE:
            return sizeof(double);
        case BOOL_TYPE:
            return sizeof(uint8_t);
        case STRING_TYPE:
            return strlen(pf->value.string_value) + 1;
        default:
            return 0;
    }
}

static void add_field_to_packet(UdpNetPacket *p, UdpNetPacketDataField *pf){
    if(p->state != PACKET_HOLD){
        return;
    }

    size_t offset = (size_t)p->length;

    if(offset + get_field_size(pf) > MAX_PACKET_DATA_BYTES){
        return;
    }

    memcpy(p->data + offset, &pf->type, sizeof(DataType));
    offset += sizeof(DataType);

    switch (pf->type){
        case UINT32_TYPE:
            memcpy(p->data + offset, &pf->value.uint32_value, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            break;
        case INT32_TYPE:
            memcpy(p->data + offset, &pf->value.int32_value, sizeof(int32_t));
            offset += sizeof(int32_t);
            break;
        case FLOAT_TYPE:
            memcpy(p->data + offset, &pf->value.float_value, sizeof(float));
            offset += sizeof(float);
            break;
        case DOUBLE_TYPE:
            memcpy(p->data + offset, &pf->value.double_value, sizeof(double));
            offset += sizeof(double);
            break;
        case BOOL_TYPE:
            memcpy(p->data + offset, &pf->value.bool_value, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            break;
        case STRING_TYPE:
            strcpy(p->data + offset, pf->value.string_value);
            offset += strlen(pf->value.string_value) + 1;
            break;
        default:
            break;
    }

    p->length = offset;
}

