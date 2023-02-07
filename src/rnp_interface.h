#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "rnp_packet.h"

// copied from rnp_networkmanager
using packetptr_t = std::unique_ptr<RnpPacketSerialized>;
using packetBuffer_t = std::queue<packetptr_t>;

/**
 * @brief Enumerate for default interfaces
 *
 * @author Kiran de Silva
 */
enum class DEFAULT_INTERFACES : uint8_t {
    /// @brief Loopback interface
    LOOPBACK = 0,

    /// @brief USB/Serial interface
    USBSERIAL = 1,
};

/**
 * @brief Structure for Interface Information
 *
 * @author Kiran de Silva
 */
struct RnpInterfaceInfo {
    /// @brief Interface status (UP/DOWN)
    bool state;

    /// @brief Interface error
    bool error;

    /// @brief Maximum Transmitable Unit
    size_t MTU;

    /// @brief Receive error
    uint8_t rxerror;

    /// @brief Transmit error
    uint8_t txerror;

    /**
     * @brief Destroy the Interface Information structure
     *
     * @author Kiran de Silva
     */
    virtual ~RnpInterfaceInfo(){};
};

/**
 * @brief Interface class
 *
 * @author Kiran de Silva
 */
class RnpInterface {
public:
    /**
     * @brief Construct a new Interface object
     *
     * @author Kiran de Silva
     *
     * @param[in] id Interface identifier
     * @param[in] name Interface name
     */
    RnpInterface(const uint8_t id, const std::string name)
        : _packetBuffer(nullptr), _id(id), _name(name){};

    /**
     * @brief Set up Interface
     *
     * @author Kiran de Silva
     */
    virtual void setup() = 0;

    /**
     * @brief Send packet
     *
     * @author Kiran de Silva
     *
     * @param[in] data Packet
     */
    virtual void sendPacket(RnpPacket &data) = 0;

    /**
     * @brief Update Interface
     *
     * @author Kiran de Silva
     */
    virtual void update() = 0;

    /**
     * @brief Get Interface information
     *
     * @todo Implement
     *
     * @author Kiran de Silva
     *
     * @return const RnpInterfaceInfo* Interface information
     */
    virtual const RnpInterfaceInfo *getInfo() = 0;

    /**
     * @brief Destroy the Interface object
     *
     * @author Kiran de Silva
     */
    virtual ~RnpInterface(){};

    /**
     * @brief Set the Packet Buffer object
     *
     * @author Kiran de Silva
     *
     * @param[in] buffer Buffer
     */
    void setPacketBuffer(packetBuffer_t *buffer) { _packetBuffer = buffer; };

    /**
     * @brief Get the Interface identifier
     *
     * @author Kiran de Silva
     *
     * @return uint8_t Interface identifier
     */
    uint8_t getID() {
        // Return identifier
        return _id;
    }

    /**
     * @brief Get the Interface name
     *
     * @author Kiran de Silva
     *
     * @return std::string Interface name
     */
    std::string getName() {
        // Return name
        return _name;
    };

protected:
    /// @brief Packet buffer
    packetBuffer_t *_packetBuffer;

    /// @brief Interface unique identifier
    const uint8_t _id;

    /// @brief Interface name
    const std::string _name;
};
