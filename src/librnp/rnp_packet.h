#pragma once

#include "rnp_header.h"

#include <vector>

// Forward declaration
class RnpPacketSerialized;

/**
 * @brief Packet class
 *
 * @author Kiran de Silva
 */
class RnpPacket {
public:
    /**
     * @brief Destroy the Rnp Packet object
     *
     * @author Kiran de Silva
     */
    virtual ~RnpPacket();

    /**
     * @brief Construct a new Rnp Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] packetService Packet service
     * @param[in] packetType Packet type
     * @param[in] packetSize Packet size
     */
    RnpPacket(const uint8_t packetService, const uint8_t packetType,
              const uint16_t packetSize);

    /**
     * @brief Construct a new Rnp Packet object from a header
     *
     * @author Kiran de Silva
     *
     * @param[in] header Packet header
     */
    RnpPacket(const RnpHeader header);

    /**
     * @brief Deserialization Constructor with size checking
     *
     * Throws std::runtime_error if the expected size does not match the size
     * decoded in the header or in the provided buffer
     *
     * @author Kiran de Silva
     *
     * @param[in] serializedPacket Reference to RnpPacketSerialzed object
     * containing raw packet data
     * @param[in] size Expected size of packet body
     */
    RnpPacket(const RnpPacketSerialized &serializedPacket, size_t size);

    /**
     * @brief Serialize packet to provided buffer
     *
     * If this is called on a non-casted instance, it will only return the
     * serialized header
     *
     * @author Kiran de Silva
     *
     * @param[out] buf Output buffer
     */
    virtual void serialize(std::vector<uint8_t> &buf);

    /// @brief Header
    RnpHeader header;
};

/**
 * @brief Serialized Packet class
 *
 * @author Kiran de Silva
 */
class RnpPacketSerialized : public RnpPacket {
public:
    /**
     * @brief Destroy the Rnp Packet Serialized object
     *
     * @author Kiran de Silva
     */
    ~RnpPacketSerialized();

    /**
     * @brief Extract header from serialized byte stream
     *
     * @param[in] bytes Byte stream of serialized packet
     */
    RnpPacketSerialized(const std::vector<uint8_t> &bytes);

    /**
     * @brief Re-serialize header back into packet. Make sure to do this if the
     * header is modified.
     *
     * @author Kiran de Silva
     */
    void reserializeHeader();

    /**
     * @brief Copies the internally stored packet to the output buffer
     *
     * The header is automatically re-serialised
     *
     * @author Kiran de Silva
     *
     * @param[out] buf Output
     */
    void serialize(std::vector<uint8_t> &buf) override;

    /**
     * @brief Extract packet body from packet
     *
     * @author Kiran de Silva
     *
     * @return std::vector<uint8_t> Serialized packet body
     */
    std::vector<uint8_t> getBody() const;

    /**
     * @brief Get the size of the packet Body
     *
     * @author Kiran de Silva
     *
     * @return size_t Packet body size
     */
    size_t getBodySize() const;

    /// @brief Serialized packet (including header and body)
    std::vector<uint8_t> packet;
};

/**
 * @brief Templated data packet for basic types
 *
 * @author Kiran de Silva
 *
 * @tparam T Packet data type
 * @tparam SERVICE Packet service
 * @tparam TYPE Packet type
 */
template <class T, uint8_t SERVICE, uint8_t TYPE>
class BasicDataPacket : public RnpPacket {
public:
    /**
     * @brief Destroy the Basic Data Packet object
     *
     * @author Kiran de Silva
     */
    ~BasicDataPacket(){};

    /**
     * @brief Construct a new Basic Data Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] _data Data member
     */
    BasicDataPacket(const T _data)
        : RnpPacket(SERVICE, TYPE, size()), data(_data){};

    /**
     * @brief Deserialize Generic Data Packet
     *
     * @author Kiran de Silva
     *
     * @param[in] packet packet
     */
    BasicDataPacket(const RnpPacketSerialized &packet)
        : RnpPacket(packet, size()) {
        // Copy packet into data
        std::memcpy(&data, packet.getBody().data(), size());
    };

    /**
     * @brief Serialize packet into buffer
     *
     * @author Kiran de Silva
     *
     * @param[out] buf Output buffer
     */
    void serialize(std::vector<uint8_t> &buf) override {
        // Serialize buffer
        RnpPacket::serialize(buf);

        // Extract buffer size
        size_t bufsize = buf.size();

        // Resize buffer to include packet
        buf.resize(bufsize + size());

        // Copy packet to the end of the buffer
        std::memcpy(buf.data() + bufsize, &data, size());
    };

    /// @brief Packet data
    T data;

    /**
     * @brief Get the size of the packet
     *
     * @author Kiran de Silva
     *
     * @return constexpr size_t Size of the packet
     */
    static constexpr size_t size() {
        // Return the size of the packet
        return sizeof(T);
    };
};

/**
 * @brief Base Message Packet class
 *
 * @author Kiran de Silva
 *
 * @tparam SERVICE Packet service
 * @tparam TYPE Packet type
 */
template <uint8_t SERVICE, uint8_t TYPE>
class MessagePacket_Base : public RnpPacket {
public:
    /**
     * @brief Destroy the Base Message Packet object
     *
     * @author Kiran de Silva
     */
    ~MessagePacket_Base(){};

    /**
     * @brief Construct a new Base Message Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] msg Packet message
     */
    MessagePacket_Base(const std::string msg)
        : RnpPacket(SERVICE, TYPE, msg.size()), _msg(msg){};

    /**
     * @brief Construct a new Base Message Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] packetData Serialized packet data
     */
    MessagePacket_Base(const RnpPacketSerialized &packetData)
        : RnpPacket(packetData.header) {
        // Assign message from packet data
        _msg.assign(packetData.packet.begin() + header.size(),
                    packetData.packet.end());
    };

    /**
     * @brief Serialize into the output buffer
     *
     * @author Kiran de Silva
     *
     * @param[out] buf Output buffer
     */
    void serialize(std::vector<uint8_t> &buf) override {
        // Serialize buffer
        RnpPacket::serialize(buf);

        // Extract buffer size
        size_t bufsize = buf.size();

        // Resize buffer to include message
        buf.resize(bufsize + size());

        // Copy message to the end of the buffer
        std::memcpy(buf.data() + bufsize, _msg.data(), size());
    };

    /// @brief Packet message
    std::string _msg;

    /**
     * @brief Get the size of the message
     *
     * @author Kiran de Silva
     *
     * @return constexpr size_t Size of the message
     */
    size_t size() {
        // Return the size of the message
        return _msg.size();
    };
};

// Declare default Message Packet
using MessagePacket_Default = MessagePacket_Base<0, 0>;