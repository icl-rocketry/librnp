#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "rnp_packet.h"

namespace NetworkCallbackMap {

    /**
     * @brief Hash function for a pair of uint8_t
     *
     * @warning Used in librrc
     *
     * @author Kiran de Silva
     */
    static constexpr auto pair_hash_func =
        [](const std::pair<uint8_t, uint8_t> &p) {
            return (static_cast<std::size_t>(p.first) << 8) +
                   static_cast<std::size_t>(p.second);
        };

} // namespace NetworkCallbackMap

/**
 * @brief Type alias of an unordered map using the pair hash function
 *
 * @warning Used in librrc
 *
 * @author Kiran de Silva
 */
using NetworkCallbackMap_t = std::unordered_map<
    std::pair<uint8_t, uint8_t>,
    std::function<void(std::unique_ptr<RnpPacketSerialized>)>,
    decltype(NetworkCallbackMap::pair_hash_func)>;
