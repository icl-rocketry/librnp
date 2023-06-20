#pragma once

#include "rnp_networkmanager.h"

#include <functional>
#include <memory>

/**
 * @brief Network service class
 *
 * @author Kiran de Silva
 */
class RnpNetworkService {
public:
    /**
     * @brief Construct a new Rnp Network Service object
     *
     * @author Kiran de Silva
     *
     * @param[in] ServiceID Service identifier
     */
    RnpNetworkService(const uint8_t ServiceID) : _ServiceID(ServiceID){};

    /**
     * @brief Get the network callback functor bound to the current instance of
     * the class
     *
     * @author Kiran de Silva
     *
     * @return PacketHandlerCb Network callback functor
     */
    PacketHandlerCb getCallback() {
        // Return the network callback functor
        return [this](packetptr_t packetptr) {
            networkCallback(std::move(packetptr));
        };
    };

    /**
     * @brief Get the service identifier
     *
     * @author Kiran de Silva
     *
     * @return uint8_t Service identifier
     */
    uint8_t getServiceID() {
        // Return the service identifier
        return _ServiceID;
    };

    /**
     * @brief Destroy the Rnp Network Service object
     *
     * @author Kiran de Silva
     */
    virtual ~RnpNetworkService(){};

private:
    /**
     * @brief Service identifier
     *
     * Service ID is constant during the duration of the object lifetime by
     * design as the service would need to be un-registered from the network
     * manager before changing its service ID
     */
    const uint8_t _ServiceID;

    /**
     * @brief Virtual function for the network callback
     *
     * @author Kiran de Silva
     *
     * @param[in] packetptr Pointer to a serialized packet
     */
    virtual void networkCallback(packetptr_t packetptr) = 0;
};