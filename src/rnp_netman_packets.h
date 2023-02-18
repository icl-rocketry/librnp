#pragma once

#include "rnp_header.h"
#include "rnp_networkmanager.h"
#include "rnp_packet.h"
#include "rnp_routingtable.h"
#include "rnp_serializer.h"

#include <array>

/**
 * @brief Enumerate for Network Manager Types
 *
 * @todo Save and Reset may benefit from a security key
 *
 * @author Kiran de Silva
 */
enum class NETMAN_TYPES : uint8_t {
    /// @brief Ping request
    PING_REQ = 1,

    /// @brief Ping result
    PING_RES = 2,

    /// @brief Set address
    SET_ADDRESS = 3,

    /// @brief Set route
    SET_ROUTE = 4,

    /// @brief Set type
    SET_TYPE = 5,

    /// @brief Set no route action
    SET_NOROUTEACTION = 6,

    /// @brief Set route generation
    SET_ROUTEGEN = 7,

    /// @brief Save Network Manager configuration
    SAVE_CONF = 8,

    /// @brief Reset Network Manager configuration
    RESET_NETMAN = 9,

};

/**
 * @brief Generic base packet
 *
 * @author Kiran de Silva
 *
 * @tparam TYPE Packet type
 */
template <uint8_t TYPE>
using GenericRnpPacket_Base =
    BasicDataPacket<uint32_t, TYPE,
                    static_cast<uint8_t>(DEFAULT_SERVICES::NETMAN)>;

/**
 * @brief Generic base packet (no type)
 *
 * @author Kiran de Silva
 */
using GenericRnpPacket =
    GenericRnpPacket_Base<0>; // when we are reading data we dont care about the
                              // type

/**
 * @brief Ping packet
 *
 * @author Kiran de Silva
 */
using PingPacket =
    GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::PING_REQ)>;

/**
 * @brief Set address packet
 *
 * @author Kiran de Silva
 */
using SetAddressPacket =
    GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_ADDRESS)>;

/**
 * @brief Set type packet
 *
 * @author Kiran de Silva
 */
using SetTypePacket =
    GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_TYPE)>;

/**
 * @brief Set no route action packet
 *
 * @author Kiran de Silva
 */
using SetNoRouteActionPacket = GenericRnpPacket_Base<static_cast<uint8_t>(
    NETMAN_TYPES::SET_NOROUTEACTION)>;

/**
 * @brief Set route generation packet
 *
 * @author Kiran de Silva
 */
using SetRouteGenPacket =
    GenericRnpPacket_Base<static_cast<uint8_t>(NETMAN_TYPES::SET_ROUTEGEN)>;

/**
 * @brief Packet class for setting routes
 *
 * @author Kiran de Silva
 *
 */
class SetRoutePacket : public RnpPacket {
    /// @brief Address type
    enum class ADDRESS_TYPE { NOTYPE = 0, STRING = 1 };

private:
    /**
     * @brief Get the packet Serializer
     *
     * @author Kiran de Silva
     *
     * @return constexpr auto Serializer
     */
    static constexpr auto getSerializer() {
        auto ret = RnpSerializer(
            &SetRoutePacket::destination, &SetRoutePacket::iface,
            &SetRoutePacket::metric, &SetRoutePacket::address_type,
            &SetRoutePacket::address_len);
        return ret;
    }

public:
    /**
     * @brief Destroy the Set Route Packet object
     *
     * @author Kiran de Silva
     */
    ~SetRoutePacket();

    /**
     * @brief Create the Route Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] destination Destination
     * @param[in] route Route
     */
    SetRoutePacket(const uint8_t destination, const Route &route);

    /**
     * @brief Create the Route Packet object
     *
     * @author Kiran de Silva
     *
     * @param[in] packet Serialized packet
     */
    SetRoutePacket(const RnpPacketSerialized &packet);

    /**
     * @brief Serialize Route Packet into buffer
     *
     * @author Kiran de Silva
     *
     * @param[out] buf Buffer
     */
    void serialize(std::vector<uint8_t> &buf) override;

    /**
     * @brief Get the Route from the Route Packet
     *
     * @author Kiran de Silva
     *
     * @return Route Route
     */
    Route getRoute();

    // data members
    /// @brief Destination address
    uint8_t destination;

    /// @brief Interface
    uint8_t iface;

    /// @brief Number of hops
    uint8_t metric;

    /// @brief Address type
    uint8_t address_type;

    /// @brief Address length
    uint8_t address_len;

    /// @brief Address data
    std::array<uint8_t, 32> address_data;

    /**
     * @brief Get the size of the Set Route Packet
     *
     * @author Kiran de Silva
     *
     * @return constexpr size_t Route Packet size
     */
    static constexpr size_t size() {
        // Return size of packet (including payload which is not in the
        // serializer)
        return getSerializer().member_size() + sizeof(address_data);
    };
};
