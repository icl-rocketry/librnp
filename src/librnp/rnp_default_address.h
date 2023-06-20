#pragma once

#include <stdint.h>

/**
 * @brief Enumerate for default addresses
 *
 * @author Kiran de Silva
 */
enum class DEFAULT_ADDRESS : uint8_t {
    /**
     * @brief Null address
     *
     * Only route is debug on this address. This address can be pinged to get
     * the current address.
     */
    NOADDRESS = 0,

    /// @brief Debug address
    DEBUG = 1,

    /// @brief Rocket address
    ROCKET = 2,

    /// @brief Groundstation gateway address
    GROUNDSTATION_GATEWAY = 3,

    /// @brief Groundstation address
    GROUNDSTATION = 4,
};