#pragma once

#include <vector>

#include "../rnp_packet.h"
#include "../rnp_serializer.h"

/// @brief Command identifier type
using command_t = uint8_t;

namespace CommandPacket {

    /**
     * @brief Get command identifier from a command service packet
     *
     * @author Kiran de Silva
     *
     * @param[in] packet Packet
     * @return command_t Command identifier
     */
    command_t getCommand(const RnpPacketSerialized &packet);

}; // namespace CommandPacket

/**
 * @brief Simple Command Packet class
 *
 * @author Kiran de Silva
 */
class SimpleCommandPacket : public RnpPacket {
private:
    /**
     * @brief Get the Command Packet Serializer
     *
     * @author Kiran de Silva
     *
     * @return constexpr auto Command packet serializer
     */
    static constexpr auto getSerializer() {
        auto ret = RnpSerializer(&SimpleCommandPacket::command,
                                 &SimpleCommandPacket::arg);
        return ret;
    }

public:
    /**
     * @brief Destroy the Simple Command Packet object
     *
     * @author Kiran de Silva
     */
    ~SimpleCommandPacket();

    /**
     * @brief Construct a new Simple Command Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] command Command
     * @param[in] arg Command argument
     */
    SimpleCommandPacket(const uint8_t command, const uint32_t arg);

    /**
     * @brief Deserialize Command Packet from serialized data
     *
     * @author Kiran de Silva
     *
     * @param[in] packet Packet
     */
    SimpleCommandPacket(const RnpPacketSerialized &packet);

    /**
     * @brief Serialize into provided buffer
     *
     * @author Kiran de Silva
     *
     * @param[out] buf Output buffer
     */
    void serialize(std::vector<uint8_t> &buf) override;

    /// @brief Command
    command_t command;

    /// @brief Command Argument
    uint32_t arg;

    /**
     * @brief Get command packet size
     *
     * @author Kiran de Silva
     *
     * @return constexpr size_t Command packet size
     */
    static constexpr size_t size() {
        // Return command packet size
        return getSerializer().member_size();
    }
};