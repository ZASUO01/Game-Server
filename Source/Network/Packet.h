//
// Created by pedro-souza on 22/10/2025.
//

#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <cstdlib>
#include <variant>
#include <string>
#include <vector>

enum class DataType : uint8_t {
    UINT32_TYPE = 0,
    INT32_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    BOOL_TYPE,
    STRING_TYPE
};

class DataField {
public:
    explicit DataField(uint32_t value) : type(DataType::UINT32_TYPE), value(value) {}
    explicit DataField(int32_t value) : type(DataType::INT32_TYPE), value(value) {}
    explicit DataField(float value) : type(DataType::FLOAT_TYPE), value(value) {}
    explicit DataField(double value) : type(DataType::DOUBLE_TYPE), value(value) {}
    explicit DataField(bool value) : type(DataType::BOOL_TYPE), value(value) {}
    explicit DataField(const char *value) : type(DataType::STRING_TYPE), value(std::string(value)) {}

    // getters
    [[nodiscard]] DataType GetType() const { return type; }
    [[nodiscard]] std::string GetTypeString() const;

    template<typename T>
    T GetValue() const { return std::get<T>(value); }

    [[nodiscard]] size_t GetValueSize() const;



private:
    DataType type;
    std::variant<uint32_t, int32_t, float, double, bool, std::string> value;
};

#pragma pack(1)
class NetPacket {
public:
    NetPacket();
    NetPacket(uint16_t _sequence, uint8_t _flag, uint32_t _nonce);

    void AddField(const DataField& field);
    void BuildPacket();
    void PrintPacket() const;
    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] std::vector<DataField> GetFields() const;
    [[nodiscard]] uint16_t GetLength() const;
    [[nodiscard]] uint8_t GetFlag() const {return flag;}

    // public packet constants
    static constexpr size_t MAX_PACKET_DATA_BYTES = 1024;
    static constexpr size_t MAX_PACKET_DATA_STRING_BYTES = 256;
    static constexpr size_t PACKET_HEADER_BYTES = 20;

    // public packet flags
    static constexpr uint8_t SYN_FLAG = 0x01;
    static constexpr uint8_t SYN_ACK_FLAG = 0x02;
    static constexpr uint8_t ACK_FLAG = 0x03;
    static constexpr uint8_t DATA_FLAG = 0x04;
    static constexpr uint8_t END_FLAG = 0x05;
    static constexpr uint8_t RST_FLAG = 0x06;

private:
    // packet content
    uint32_t sync1;
    uint32_t sync2;
    uint8_t state;
    uint16_t sequence;
    uint8_t flag;
    uint32_t nonce;
    uint16_t length;
    uint16_t checksum;
    char data[NetPacket::MAX_PACKET_DATA_BYTES];

    // packet constants
    static constexpr uint32_t PACKET_SYNC_BYTES = 0x554E4554;
    static constexpr uint8_t PACKET_HOLD = 1;
    static constexpr uint8_t PACKET_READY = 2;
};
#pragma pack(0)

#endif // PACKET_H