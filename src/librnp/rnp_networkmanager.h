#pragma once

#include <array>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "loopback.h"
#include "rnp_header.h"
#include "rnp_interface.h"
#include "rnp_packet.h"
#include "rnp_routingtable.h"

/// @brief Packet pointer type
using packetptr_t = std::unique_ptr<RnpPacketSerialized>;

/// @brief Packet buffer type
using packetBuffer_t = std::queue<packetptr_t>;

/// @brief Packet callback handler type
using PacketHandlerCb = std::function<void(packetptr_t)>;

/// @brief Logging callback type
using LogCb_t = std::function<void(const std::string &)>;

/**
 * @brief Enumerate for node type
 *
 * @author Kiran de Silva
 */
enum class NODETYPE : uint8_t {
    /**
     * @brief Leaf node
     *
     * Only accepts packets addressed to this node and dumps any others
     */
    LEAF = 0,

    /**
     * @brief Hub node
     *
     * Accepts all packets and forwards to the correct interface (promiscous
     * mode)
     */
    HUB = 1
};

/**
 * @brief Enumerate for no route action
 *
 * @author Kiran de Silva
 */
enum class NOROUTE_ACTION : uint8_t {
    /**
     * @brief Dump the packet
     */
    DUMP = 0,

    /**
     * @brief Broadcast the packet to the specifed interfaces
     *
     * @warning Be careful of packet duplication: no de-duplication service has
     * been written yet
     */
    BROADCAST = 1,
};

/**
 * @brief Enumerate for default services
 *
 * @author Kiran de Silva
 */
enum class DEFAULT_SERVICES : uint8_t {
    /**
     * @brief No service
     *
     * Reserved for 'special' uses. On the ground station, nodes on RNP can send
     * packets to the the command service on the groundstation without being
     * forwarded to connected IP clients. NOSERVICE on a node which is a hub
     * also allows packets to be forwarded to the debug interface allowing the
     * debug interface to communicate to any node on the network. This only
     * happens if the packet is addressed to a node.
     */
    NOSERVICE = 0,

    /**
     * @brief Netmanager service
     */
    NETMAN = 1,

    /**
     * @brief Command processor service
     */
    COMMAND = 2,
};

/**
 * @brief Structure for network manager configuration
 *
 * @author Kiran de Silva
 */
struct RnpNetworkManagerConfig {
    /// @brief Current address
    uint8_t currentAddress;

    /// @brief Node type
    NODETYPE nodeType;

    /// @brief No route action
    NOROUTE_ACTION noRouteAction;

    /// @brief Flag for route generation
    bool routeGenEnabled;
};

/// @brief Implementation of the save config function
using SaveConfigImpl =
    std::function<bool(RnpNetworkManagerConfig const &config)>;

/**
 * @brief Network Manager class
 *
 * @todo Provide implementation for loading from JSON (including the NVS loads).
 * @todo Provide implementation in routing table for loading and serializing
 * to/from JSON.
 *
 * @author Kiran de Silva
 */
class RnpNetworkManager {

public:
    /**
     * @brief Construct a new Rnp Network Manager object
     *
     * Default constructor with default settings
     *
     * @author Kiran de Silva
     *
     * @param[in] address Address (default: 0)
     * @param[in] nodeType Node type (default: leaf)
     * @param[in] enableLogging Logging flag (default: false)
     */
    RnpNetworkManager(const uint8_t address = 0,
                      const NODETYPE nodeType = NODETYPE::LEAF,
                      const bool enableLogging = false);

    /**
     * @brief Construct a new Rnp Network Manager object
     *
     * @author Kiran de Silva
     *
     * @param[in] config Network manager configuration
     * @param[in] enableLogging Logging flag (default: flase)
     */
    RnpNetworkManager(const RnpNetworkManagerConfig config,
                      const bool enableLogging = false);

    /**
     * @brief Reconfigure newtork manager
     *
     * @author Kiran de Silva
     *
     * @param[in] config Network manager configuration
     * @param[in] newroutingtable Routing table
     */
    void reconfigure(const RnpNetworkManagerConfig config,
                     const RoutingTable newroutingtable);

    /**
     * @brief Update network manager
     *
     * Runs update routine on all interfaces in the iflist, and routePacket
     * command to process any received packets
     *
     * @author Kiran de Silva
     */
    void update();

    /**
     * @brief Reset the networking configuration
     *
     * Includes resetting the routing table to the originally loaded
     * configuration
     *
     * @author Kiran de Silva
     */
    void reset();

    /**
     * @brief Load network manager configuration
     *
     * @author Kiran de Silva
     *
     * @param[in] config Network manager configuration
     */
    void loadconfig(const RnpNetworkManagerConfig config);

    /**
     * @brief Set the routing table
     *
     * @author Kiran de Silva
     *
     * @param[in] newroutingtable Routing table
     */
    void setRoutingTable(const RoutingTable newroutingtable);

    /**
     * @brief Send a packet
     *
     * All routing information should be contained within the header of the
     * packet
     *
     * @author Kiran de Silva
     *
     * @param[in] packet Packet
     */
    void sendPacket(RnpPacket &packet);

    /**
     * @brief Send a packet over a given route
     *
     * @param[in] route Route
     * @param[in] packet Packet
     */
    void sendByRoute(const Route &route, RnpPacket &packet);

    /**
     * @brief Set the address of the node
     *
     * Removes the previous loopback route, and generates a new loopback to
     * ensure that one always exists
     *
     * @author Kiran de Silva
     *
     * @param[in] address Node address
     */
    void setAddress(const uint8_t address);

    /**
     * @brief Get the address of the node
     *
     * @author Kiran de Silva
     *
     * @return uint8_t Node address
     */
    uint8_t getAddress() {
        // Return the address of the node
        return _config.currentAddress;
    };

    /**
     * @brief Set the node type
     *
     * @author Kiran de Silva
     *
     * @param[in] nodeType Node type
     */
    void setNodeType(const NODETYPE nodeType);

    /**
     * @brief Get the node type
     *
     * @author Kiran de Silva
     *
     * @return NODETYPE Node type
     */
    NODETYPE getNodeType() {
        // Return the node type
        return _config.nodeType;
    };

    /**
     * @brief Add interface to the interface list
     * 
     * @todo Implement handling of interface clashes
     *
     * @author Kiran de Silva
     *
     * @param[in] iface Pointer to interface object
     */
    void addInterface(RnpInterface *iface);

    /**
     * @brief Get the interface object from the interface list
     *
     * @author Kiran de Silva
     *
     * @param[in] ifaceID Interface identifier
     * @return std::optional<RnpInterface *> Pointer to interface object
     */
    std::optional<RnpInterface *> getInterface(const uint8_t ifaceID);

    /**
     * @brief Remove interface from the interface list
     *
     * @author Kiran de Silva
     *
     * @param[in] ifaceID Interface identifier
     */
    void removeInterface(const uint8_t ifaceID);

    /**
     * @brief Remove interface from the interface list
     *
     * @author Kiran de Silva
     *
     * @param[in] iface Pointer to interface object
     */
    void removeInterface(RnpInterface *iface);

    /**
     * @brief Get the interface information
     *
     * @author Kiran de Silva
     *
     * @param[in] ifaceID Interface identifier
     * @return const RnpInterfaceInfo* Pointer to the interface information
     */
    const RnpInterfaceInfo *getInterfaceInfo(const uint8_t ifaceID);

    /**
     * @brief Get a list of interfaces
     *
     * @todo Implement
     *
     * @author Kiran de Silva
     *
     * @return std::string List of interfaces
     */
    std::string getInterfaceList() {
        // Return blank string
        return "";
    };

    /**
     * @brief Register a packet callback handler for a specified service
     * identifier.
     *
     * Thread safety is up to the implementation of the callback. Check that the
     * expected and received packet lengths match.
     *
     * @author Kiran de Silva
     *
     * @param[in] serviceID Service identifier
     * @param[in] packetHandler Callback handler
     */
    void registerService(const uint8_t serviceID,
                         PacketHandlerCb packetHandler);

    /**
     * @brief Remove callback by service identifier.
     *
     * @warning Ensure that this is called if the callback passed gets
     * destroyed, otherwise there will be undefined behaviour.
     *
     * @author Kiran de Silva
     *
     * @param[in] serviceID Service identifier
     */
    void unregisterService(const uint8_t serviceID);

    /**
     * @brief Pass a logging callback to allow logging of errors from the
     * network manager.
     *
     * @author Kiran de Silva
     *
     * @param[in] logcb Logging callback
     */
    void setLogCb(LogCb_t logcb) {
        // Set callback
        _logcb = logcb;
    };

    /**
     * @brief Set the flag for automatic route generation
     *
     * When enabled, routes will be learned by analyzing packets coming from
     * previously unknown sources.
     *
     * @author Kiran de Silva
     *
     * @param[in] setting Route generation setting
     */
    void enableAutoRouteGen(const bool setting) {
        // Set the automatic route generation flag
        _config.routeGenEnabled = setting;
    };

    /**
     * @brief Set the action if no route is found in the routing table
     *
     * @author Kiran de Silva
     *
     * @param[in] action No route action
     * @param[in] ifaces List of interface identifiers for broadcasting the
     * packet
     */
    void setNoRouteAction(const NOROUTE_ACTION action,
                          const std::vector<uint8_t> ifaces = {});

    /**
     * @brief Generate the loopback route and debug port route
     *
     * @author Kiran de Silva
     */
    void generateDefaultRoutes();

    /**
     * @brief Update the base table to the current routing table
     *
     * @author Kiran de Silva
     */
    void updateBaseTable() {
        // Update the base table with the current routing table
        _basetable = routingtable;
    };

    /**
     * @brief Set the save configuration implementation to save the current
     * configuration in memory
     *
     * @author Kiran de Silva
     *
     * @param[in] saveConfigImpl Save configuration implementation
     */
    void setSaveConfigImpl(SaveConfigImpl saveConfigImpl) {
        // Set the save configuration
        _saveConfigImpl = saveConfigImpl;
    };

private:
    /**
     * @brief Process any received packet
     *
     * @author Kiran de Silva
     */
    void routePackets();

    /**
     * @brief Forward packet
     *
     * @author Kiran de Silva
     *
     * @param[in] packet Packet
     */
    void forwardPacket(RnpPacket &packet);

    /**
     * @brief Internal network management packet handler
     *
     * @todo Handle ping response
     * 
     * @author Kiran de Silva
     *
     * @param[in] packet_ptr Pointer to packet
     */
    void NetManHandler(packetptr_t packet_ptr);

    /// @brief Packet buffer
    packetBuffer_t packetBuffer;

    /// @brief Service lookup
    std::vector<PacketHandlerCb> serviceLookup;

    /// @brief Loopback (owned by the network manager by default)
    Loopback lo;

private:
    /// @brief Interface list
    std::vector<RnpInterface *> ifaceList;

    /// @brief Broadcast interfaces list
    std::vector<uint8_t> _broadcastList;

    /// @brief Network manager configuration
    RnpNetworkManagerConfig _config;

    /// @brief Network manager configuration save implementation
    SaveConfigImpl _saveConfigImpl;

    /// @brief Routing table
    RoutingTable routingtable;

    /// @brief Copy of the initial routing table
    RoutingTable _basetable;

    /// @brief Logging flag
    const bool _loggingEnabled;

    /// @brief Logging function callback
    LogCb_t _logcb;

    /**
     * @brief Log message
     *
     * @author Kiran de Silva
     *
     * @param[in] msg Message
     */
    void log(const std::string &msg);
};
