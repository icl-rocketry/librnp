#pragma once

#include <stdint.h>

/**
 * @brief Enumerate for keeping track of RNP packet types
 *
 * @author Andrei Paduraru
 */
enum class RNP_TYPES : uint8_t {
    /// @brief Simple command packet
    SIMPLE_COMMAND = 0,
    
    /// @brief Network Manager - Ping request
    PING_REQ = 1,

    /// @brief Network Manager - Ping result
    PING_RES = 2,

    /// @brief Network Manager - Set address
    SET_ADDRESS = 3,

    /// @brief Network Manager - Set route
    SET_ROUTE = 4,

    /// @brief Network Manager - Set type
    SET_TYPE = 5,

    /// @brief Network Manager - Set no route action
    SET_NOROUTEACTION = 6,

    /// @brief Network Manager - Set route generation
    SET_ROUTEGEN = 7,

    /// @brief Network Manager - Save Network Manager configuration
    SAVE_CONF = 8,

    /// @brief Network Manager - Reset Network Manager configuration
    RESET_NETMAN = 9,

    /// @brief Kermit - Processed sensor packet.
    KERMIT_PROCESSED = 103,

    /// @brief Kermit - Raw ADC packet.
    KERMIT_RAW = 104,

    /// @brief Network Manager - Get Node info
    NODEINFO = 254,

};