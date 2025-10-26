//
// Created by pedro-souza on 22/10/2025.
//
#include "Packet.h"
#include "NetUtils.h"
#include  "Platforms.h"
#include <cstring>
#include <cstdio>

size_t DataField::GetValueSize() const{
    switch(type) {
        case DataType::UINT32_TYPE: return sizeof(uint32_t);
        case DataType::INT32_TYPE: return sizeof(int32_t);
        case DataType::FLOAT_TYPE: return sizeof(float);
        case DataType::DOUBLE_TYPE: return sizeof(double);
        case DataType::BOOL_TYPE: return sizeof(bool);
        case DataType::STRING_TYPE: return std::get<std::string>(value).size();
    }
    return 0;
}

std::string DataField::GetTypeString() const{
    switch(type) {
        case DataType::UINT32_TYPE: return "UINT 32";
        case DataType::INT32_TYPE: return "INT 32";
        case DataType::FLOAT_TYPE: return "FLOAT";
        case DataType::DOUBLE_TYPE: return "DOUBLE";
        case DataType::BOOL_TYPE: return "BOOL";
        case DataType::STRING_TYPE: return "STRING";
    }
    return "";
}

NetPacket::NetPacket()
:sync1(PACKET_SYNC_BYTES)
,sync2(PACKET_SYNC_BYTES)
,state(PACKET_HOLD)
,sequence(0)
,flag(0)
,nonce(0)
,length(0)
,checksum(0)
,data{}
{}

NetPacket::NetPacket(const uint16_t _sequence, const uint8_t _flag, const uint32_t _nonce)
:sync1(PACKET_SYNC_BYTES)
,sync2(PACKET_SYNC_BYTES)
,state(PACKET_HOLD)
,sequence(_sequence)
,flag(_flag)
,nonce(_nonce)
,length(0)
,checksum(0)
,data{}
{}

void NetPacket::AddField(const DataField& field)  {
    if (state != PACKET_HOLD) {
        return;
    }

    const DataType fieldType = field.GetType();
    const size_t fieldSize = field.GetValueSize();

    size_t totalNeeded;
    if (fieldType == DataType::STRING_TYPE) {
        totalNeeded = sizeof(DataType) + sizeof(size_t) + fieldSize;
    }else {
        totalNeeded = sizeof(DataType) + fieldSize;
    }


    if (length + totalNeeded > MAX_PACKET_DATA_BYTES) {
        return;
    }
    size_t offset = length;

    std::memcpy(data + offset, &fieldType, sizeof(DataType));
    offset += sizeof(DataType);

    switch (fieldType) {
        case DataType::UINT32_TYPE: {
            const auto value = field.GetValue<uint32_t>();
            std::memcpy(data + offset, &value, fieldSize);
            break;
        }
        case DataType::INT32_TYPE: {
            const auto value = field.GetValue<int32_t>();
            std::memcpy(data + offset, &value, fieldSize);
            break;
        }
        case DataType::FLOAT_TYPE: {
            const auto value = field.GetValue<float>();
            std::memcpy(data + offset, &value, fieldSize);
            break;
        }
        case DataType::DOUBLE_TYPE: {
            const auto value = field.GetValue<double>();
            std::memcpy(data + offset, &value, fieldSize);
            break;
        }
        case DataType::BOOL_TYPE: {
            const auto value = field.GetValue<bool>();
            std::memcpy(data + offset, &value, fieldSize);
            break;
        }
        case DataType::STRING_TYPE: {
            const auto value = field.GetValue<std::string>();

            const auto len = static_cast<size_t>(value.size());
            std::memcpy(data + offset, &len, sizeof(size_t));
            offset += sizeof(size_t);

            std::memcpy(data + offset, value.data(), fieldSize);
            break;
        }
        default:
            break;
    }
    offset += fieldSize;
    length = offset;
}

void NetPacket::BuildPacket() {
    if (state != PACKET_HOLD) {
        return;
    }

    sync1 = htonl(sync1);
    sync2 = htonl(sync2);
    sequence = htons(sequence);
    nonce = htonl(nonce);

    state = PACKET_READY;

    const uint16_t _length = length;
    length = htons(length);

    const size_t packetSize = PACKET_HEADER_BYTES + _length;
    checksum = NetUtils::getNetChecksum(this, packetSize);
    checksum = htons(checksum);
}

void NetPacket::PrintPacket() const {
    if (state != PACKET_READY) {
        return;
    }

    const uint16_t _length = ntohs(length);

    printf("==== PACKET HEADER ====\n");
    printf("SEQUENCE: %hu\n", ntohs(sequence));
    printf("FLAG:   %hhu\n", flag);
    printf("NONCE:     %u\n", ntohl(nonce));
    printf("LENGTH: %hu\n", _length);
    printf("==== CONTROL FIELDS ====\n");
    printf("SYNC1:    %#x\n", ntohl(sync1));
    printf("SYNC2:    %#x\n", ntohl(sync2));
    printf("CHECKSUM: %#x\n", ntohl(checksum));

    if (_length > 0) {
        printf("==== DATA FIELDS ====\n");
        const auto fields = GetFields();

        for (const auto& field : fields) {
            printf("%s |  ", field.GetTypeString().c_str());

            switch (field.GetType()) {
                case DataType::UINT32_TYPE:
                    printf("%u", field.GetValue<uint32_t>());
                    break;
                case DataType::INT32_TYPE:
                    printf("%d", field.GetValue<int32_t>());
                    break;
                case DataType::FLOAT_TYPE:
                    printf("%f", field.GetValue<float>());
                    break;
                case DataType::DOUBLE_TYPE:
                    printf("%f", field.GetValue<double>());
                    break;
                case DataType::BOOL_TYPE:
                    printf("%s", field.GetValue<bool>() ? "true" : "false");
                    break;
                case DataType::STRING_TYPE:
                    printf("%s", field.GetValue<std::string>().c_str());
                    break;
                default:
                    printf("[Unknown Type]");
                    break;
            }
            printf("\n");
        }
    }
}

bool NetPacket::IsValid() const {
    if (state != PACKET_READY) {
        return false;
    }

    if(sync1 != htonl(PACKET_SYNC_BYTES) ||
       sync2 != htonl(PACKET_SYNC_BYTES)) {
        return false;
    }

    const uint16_t _length = ntohs(length);
    if(_length > MAX_PACKET_DATA_BYTES){
        return false;
    }

    NetPacket temp{};
    std::memcpy(&temp, this, sizeof(NetPacket));
    temp.checksum = 0;

    const size_t packetSize = PACKET_HEADER_BYTES + _length;

    if (const uint16_t calculatedChecksum = NetUtils::getNetChecksum(&temp, packetSize);
        calculatedChecksum != ntohs(checksum)) {
        return false;
    }

    return true;
}

std::vector<DataField> NetPacket::GetFields() const {
    size_t _length = length;
    if (state == PACKET_READY) {
        _length = ntohs(_length);
    }

    std::vector<DataField> fields;
    size_t offset = 0;

    while (offset < _length) {
        if (offset + sizeof(DataType) > _length) break;

        DataType fieldType;
        std::memcpy(&fieldType, data + offset, sizeof(DataType));
        offset += sizeof(DataType);

        switch (fieldType) {
            case DataType::UINT32_TYPE: {
                if (offset + sizeof(uint32_t) > _length) return fields;
                uint32_t value;
                std::memcpy(&value, data + offset, sizeof(value));
                offset += sizeof(value);
                fields.emplace_back(value);
                break;
            }
            case DataType::INT32_TYPE: {
                if (offset + sizeof(int32_t) > _length) return fields;
                int32_t value;
                std::memcpy(&value, data + offset, sizeof(value));
                offset += sizeof(value);
                fields.emplace_back(value);
                break;
            }
            case DataType::FLOAT_TYPE: {
                if (offset + sizeof(float) > _length) return fields;
                float value;
                std::memcpy(&value, data + offset, sizeof(value));
                offset += sizeof(value);
                fields.emplace_back(value);
                break;
            }
            case DataType::DOUBLE_TYPE: {
                if (offset + sizeof(double) > _length) return fields;
                double value;
                std::memcpy(&value, data + offset, sizeof(value));
                offset += sizeof(value);
                fields.emplace_back(value);
                break;
            }
            case DataType::BOOL_TYPE: {
                if (offset + sizeof(bool) > _length) return fields;
                uint8_t byteVal;
                std::memcpy(&byteVal, data + offset, sizeof(byteVal));
                offset += sizeof(byteVal);
                fields.emplace_back(byteVal != 0);
                break;
            }
            case DataType::STRING_TYPE: {
                if (offset + sizeof(size_t) > _length) return fields;
                size_t strLen;
                std::memcpy(&strLen, data + offset, sizeof(strLen));
                offset += sizeof(size_t);

                if (offset + strLen > _length) return fields;
                std::string value(data + offset, strLen);
                offset += strLen;

                fields.emplace_back(value.data());
                break;
            }
            default:
                return fields;
        }
    }

    return fields;
}

uint16_t NetPacket::GetLength() const {
    if (state != PACKET_READY) {
        return length;
    }

    return htons(length);
}