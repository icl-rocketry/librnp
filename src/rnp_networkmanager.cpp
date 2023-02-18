#include "rnp_networkmanager.h"

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "rnp_default_address.h"
#include "rnp_netman_packets.h"
#include "rnp_packet.h"
#include "rnp_routingtable.h"

#if defined(ARDUINO)
#include <Arduino.h>
#endif

RnpNetworkManager::RnpNetworkManager(const uint8_t address,
                                     const NODETYPE nodeType,
                                     const bool enableLogging)
    : serviceLookup(1),
      _config({address, nodeType, NOROUTE_ACTION::DUMP, false}),
      routingtable(1), _loggingEnabled(enableLogging) {
    // Add loopback interface
    addInterface(&lo);

    // Generate default routes
    generateDefaultRoutes();
};

RnpNetworkManager::RnpNetworkManager(const RnpNetworkManagerConfig config,
                                     const bool enableLogging)
    : serviceLookup(1), _config(config), routingtable(1),
      _loggingEnabled(enableLogging) {

    // Add loopback interface
    addInterface(&lo);

    // Generate default routes
    generateDefaultRoutes();
};

void RnpNetworkManager::update() {
    // Iterate through the interface list
    for (auto iface_ptr : ifaceList) {
        // Check that interface exists
        if (iface_ptr != nullptr) {
            // Update interface
            iface_ptr->update();
        }
    }

    // Route packets
    routePackets();
}

void RnpNetworkManager::reset() {
    // Replace the current routing table with the originally loaded table
    routingtable = _basetable;

    // Generate default routes
    generateDefaultRoutes();
}

void RnpNetworkManager::reconfigure(const RnpNetworkManagerConfig config,
                                    const RoutingTable newroutingtable) {
    // Load the new routing table first so that if there is a mismatch between
    // the new configuration address and the routing table, we can communicate
    // with the node
    setRoutingTable(newroutingtable);

    // Load configuration
    loadconfig(config);
};

void RnpNetworkManager::loadconfig(const RnpNetworkManagerConfig config) {
    // Set address to clean up old address loopback route
    setAddress(config.currentAddress);

    // Set the network configuration
    _config = config;
};

void RnpNetworkManager::setRoutingTable(const RoutingTable newroutingtable) {
    // Keep the base routing table intact in case this action disrupts the
    // newtork configuration
    routingtable = newroutingtable;

    // Generate default routes so that we can communicate with the node
    generateDefaultRoutes();
};

void RnpNetworkManager::sendPacket(RnpPacket &packet) {
    // Increment the number of hops of the packet
    packet.header.hops += 1;

    // Extract the destination from the packet header
    uint8_t destination = packet.header.destination;

    // Get the route to the destination from the routing table
    std::optional<Route> route = routingtable.getRoute(destination);

    // Check if no route exists
    if (!route) {
        // Check the no route action
        switch (_config.noRouteAction) {
        case NOROUTE_ACTION::DUMP: { // Dump the packet
            return;
        }
        case NOROUTE_ACTION::BROADCAST: { // Broadcast the packet
            // Function for broadcast the packet on a given interface
            auto broadcastPacket = [&packet, this](uint8_t ifaceID) {
                // Dump the packet if broadcast is attempted on the same
                // interface as it was received
                if ((ifaceID == packet.header.src_iface) ||
                    (ifaceID ==
                     static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK))) {
                    return;
                }

                // Broadcast the packet on the specified interface
                sendByRoute({ifaceID, 0, {}}, packet);
            };

            // Broadcast over listed or all available interfaces
            if (_broadcastList.size() == 0) {
                // Iterate through interfaces
                for (int i = 0; i < ifaceList.size(); i++) {
                    // Broadcast packet on interface
                    broadcastPacket(i);
                }
            } else {
                // Iterate through broadcast interfaces
                for (uint8_t ifaceID : _broadcastList) {
                    // Broadcast packet on interface
                    broadcastPacket(ifaceID);
                }
            }
        }
        default: { // Dump the packet
            return;
        }
        }
    }

    // Determine if the packet is being forwarded
    if (packet.header.source != _config.currentAddress) {
        // Dump the packet if forwarding is attempted on the same interface as
        // it was received
        if (packet.header.src_iface == route.value().iface) {
            return;
        }
    }

    // Send the packet via the route
    sendByRoute(route.value(), packet);
}

void RnpNetworkManager::sendByRoute(const Route &route, RnpPacket &packet) {
    // Get the interface identifier
    uint8_t ifaceID = route.iface;

    // Dump the packet if tramission is attempted on the loopback interface, and
    // the destination and current address do not match
    if (ifaceID == static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK) &&
        (packet.header.destination != _config.currentAddress)) {
        // Log the bad route
        log("[E] Bad route: destination and current address do not match when "
            "sending over the loopback interface");
        return;
    }

    // Get the interface pointer
    std::optional<RnpInterface *> iface_ptr = getInterface(ifaceID);

    // Dump the packet if an invalid interface is returned
    if (!iface_ptr) {
        log("[E] Invalid/non-existent interface");
        return;
    }

    // Update the packet header link layer address
    packet.header.lladdress = route.address;

    // Send the packet over the interface
    iface_ptr.value()->sendPacket(packet);
};

void RnpNetworkManager::setAddress(const uint8_t address) {
    // Get the current route from the routing table
    auto currentRoute = routingtable.getRoute(_config.currentAddress);

    // Ensure that we do no delete a new route if this is called after a new
    // routing table is assigned
    if (currentRoute && (currentRoute.value().iface ==
                         static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK))) {
        routingtable.deleteRoute(_config.currentAddress);
    }

    // Update the current address
    _config.currentAddress = address;

    // Generate default routes
    generateDefaultRoutes();
};

void RnpNetworkManager::setNodeType(const NODETYPE nodeType) {
    // Update the node type
    _config.nodeType = nodeType;
};

void RnpNetworkManager::addInterface(RnpInterface *iface) {
    // Get the interface identifier
    uint8_t ifaceID = iface->getID();

    // Check if the identifier is greater than the size of the interface list
    if (ifaceID >= ifaceList.size()) {
        // Resize interface list
        ifaceList.resize(ifaceID + 1);
    }

    // Check if the interface is already defined in the interface list
    if ((ifaceList.at(ifaceID) != nullptr) &&
        (ifaceList.at(ifaceID) != iface)) {
        // Log interface clash
        log("[E] Non-unique interface identifier");
    }

    // Add interface to list
    ifaceList.at(ifaceID) = iface;

    // Set interface packet buffer
    iface->setPacketBuffer(&packetBuffer);
};

std::optional<RnpInterface *>
RnpNetworkManager::getInterface(const uint8_t ifaceID) {
    // Check if the identifier is greater than the size of the interface list
    if (ifaceID >= ifaceList.size()) {
        // Log out of range
        log("[E] Interface identifier is out of range");

        // Return null interface
        return {};
    }

    // Extract interface pointer
    RnpInterface *iface_ptr = ifaceList.at(ifaceID);

    // Check for null interface
    if (iface_ptr == nullptr) {
        // Return null interface
        return {};
    }

    // Return interface pointer
    return {iface_ptr};
};

void RnpNetworkManager::removeInterface(const uint8_t ifaceID) {
    // Check if the identifier is greater than the size of the interface list
    if (ifaceID >= ifaceList.size()) {
        log("[E] Interface identifier is out of range");
        return;
    }

    // Extract interface pointer
    RnpInterface *iface_ptr = ifaceList[ifaceID];

    // Set null packet buffer
    iface_ptr->setPacketBuffer(nullptr);

    // Set null pointer in interface list
    ifaceList.at(ifaceID) = nullptr;

    // Erase the last element from the interface list if the index is the final
    // element in the vector
    if (ifaceID == ifaceList.size() - 1) {
        ifaceList.erase(ifaceList.end());
    }
};

void RnpNetworkManager::removeInterface(RnpInterface *iface) {
    // Get the interface identifier
    uint8_t ifaceID = iface->getID();

    // Remove the interface from the interface list
    removeInterface(ifaceID);
};

const RnpInterfaceInfo *
RnpNetworkManager::getInterfaceInfo(const uint8_t ifaceID) {
    // Get the interface pointer
    auto iface_ptr = getInterface(ifaceID);

    // Return null pointer if the interface does not exist
    if (!iface_ptr) {
        return nullptr;
    }

    // Return the interface information
    return iface_ptr.value()->getInfo();
};

void RnpNetworkManager::registerService(const uint8_t serviceID,
                                        PacketHandlerCb packetHandler) {
    // Prevent adding a service with identifier = 0
    if (serviceID == 0) {
        // Log the attempt
        log("[E] registerService-> Illegal Service identifier provided");
        return;
    }

    // Check if the service identifier is greater than the size of the service
    // list
    if (serviceID >= serviceLookup.size()) {
        // Resize the service lsit
        serviceLookup.resize(serviceID + 1);
    }

    // Set the packet handler
    serviceLookup.at(serviceID) = packetHandler;
}

void RnpNetworkManager::unregisterService(const uint8_t serviceID) {
    // Prevent removing a service with identifier = 0
    if (serviceID == 0) {
        // Log the attempt
        log("[E] unregisterService-> Illegal Service ID provided");
        return;
    }

    // Check if the service identifier is greater than the size of the service
    // list
    if (serviceID >= serviceLookup.size()) {
        // Log the error
        log("[E] service ID out of range");
        return;
    }

    // Set a null packet handler
    serviceLookup.at(serviceID) = PacketHandlerCb{};

    // Erase the last element from the service list if the index is the final
    // element in the vector
    if (serviceID == serviceLookup.size() - 1) {
        serviceLookup.erase(serviceLookup.end());
    }
}

void RnpNetworkManager::setNoRouteAction(const NOROUTE_ACTION action,
                                         const std::vector<uint8_t> ifaces) {
    // Set the no route action
    _config.noRouteAction = action;

    // Set the broadcast list
    _broadcastList = ifaces;
}

void RnpNetworkManager::generateDefaultRoutes() {
    // Set the loopback route
    routingtable.setRoute(_config.currentAddress, Route{0, 1, {}});

    // Set the debug port route
    routingtable.setRoute(static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG),
                          Route{1, 1, {}});
};

void RnpNetworkManager::routePackets() {
    // Return if the packet buffer is empty
    if (packetBuffer.empty()) {
        return;
    }

    // Take "ownership" of packet at the top of the buffer
    packetptr_t packet_ptr = std::move(packetBuffer.front());

    // Remove packet from the top of the buffer
    packetBuffer.pop();

    // Check if automatic route generation is enabled
    if (_config.routeGenEnabled) {
        // Get the route to the source node
        std::optional<Route> currentRoute =
            routingtable.getRoute(packet_ptr->header.source);

        // If a route does not exist, generate a new route
        if (!currentRoute) {
            // Create a new route
            Route newroute{packet_ptr->header.src_iface,
                           packet_ptr->header.hops,
                           packet_ptr->header.lladdress};

            // Add the new route to the routing table
            routingtable.setRoute(packet_ptr->header.source, newroute);
        }
    }

    // Check if the packet is from debug and has no address
    if ((packet_ptr->header.source ==
         static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG)) &&
        (packet_ptr->header.destination ==
         static_cast<uint8_t>(DEFAULT_ADDRESS::NOADDRESS))) {

        // Process packets to no address on the debug interface as local packets
        packet_ptr->header.destination = _config.currentAddress;
    }

    // Forward the packet if the current address is not its destination
    if (packet_ptr->header.destination != _config.currentAddress) {
        forwardPacket(*packet_ptr);
        return;
    }

    // Dump the packet if it is addressed to the current node, but was not
    // received over the loopback interface
    if ((packet_ptr->header.source == _config.currentAddress) &&
        (packet_ptr->header.src_iface !=
         static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK))) {
        return;
    }

    // Extract the packet's destination service
    uint8_t packetService = packet_ptr->header.destination_service;

    // Check the packet's destination service
    switch (packetService) {
    case static_cast<uint8_t>(DEFAULT_SERVICES::NOSERVICE): {
        // Set the destination address to the debug address
        packet_ptr->header.destination =
            static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG);

        // Send the packet
        sendPacket(*packet_ptr);
        break;
    }
    case static_cast<uint8_t>(DEFAULT_SERVICES::NETMAN): {
        // Mpve the packet to the network manager service
        NetManHandler(std::move(packet_ptr));
        break;
    }
    default: // pass packet to service handler
    {
        // Check if the service identifier is greater than the size of the
        // service list
        if (packetService >= serviceLookup.size()) {
            // Dump the packet
            return;
        }

        // Extract the packet callback handler
        PacketHandlerCb callback = serviceLookup.at(packetService);

        // Check for an empty packet callback handler
        if (!callback) {
            // Dump the packet
            return;
        }

        // Call the packet callback handler
        callback(std::move(packet_ptr));
        break;
    }
    }
};

void RnpNetworkManager::NetManHandler(packetptr_t packet_ptr) {
    // Check header type
    switch (static_cast<NETMAN_TYPES>(packet_ptr->header.type)) {
    case NETMAN_TYPES::PING_REQ: { // Ping request
        // Deserialize packet
        PingPacket pong(*packet_ptr);

        // Swap destination and source addresses for response
        std::swap(pong.header.destination, pong.header.source);

        // Set response type
        pong.header.type = (uint8_t)NETMAN_TYPES::PING_RES;

        // Set response service to request service
        pong.header.destination_service = pong.header.source_service;

        // Update source service
        pong.header.source_service = (uint8_t)DEFAULT_SERVICES::NETMAN;

        // Send response
        sendPacket(pong);

        // Log sent response
        log("Ping sent");
        break;
    }
    case NETMAN_TYPES::PING_RES: { // Ping response
        // Deserialize packet
        GenericRnpPacket packet(*packet_ptr);

        // Log received ping
        log("Ping received with systime of " + std::to_string(packet.data));
        break;
    }
    case NETMAN_TYPES::SET_ADDRESS: { // Set address
        // Deserialize packet
        GenericRnpPacket packet(*packet_ptr);

        // Set address from packet data
        setAddress(static_cast<uint8_t>(packet.data));

        // Log address change
        log("Node address is now " +
            std::to_string(static_cast<uint8_t>(packet.data)));
        break;
    }
    case NETMAN_TYPES::SET_ROUTE: { // Set route
        // Deserialize packet
        SetRoutePacket setroutepacket(*packet_ptr);

        // Set route
        routingtable.setRoute(setroutepacket.destination,
                              setroutepacket.getRoute());

        // Log route change
        log("Route for Node " + std::to_string(setroutepacket.destination) +
            " has been updated");
        break;
    }
    case NETMAN_TYPES::SET_TYPE: { // Set node type
        // Deserialize packet
        GenericRnpPacket packet(*packet_ptr);

        // Set node type
        _config.nodeType = static_cast<NODETYPE>(packet.data);

        // Log change to node type
        log("Node type is now " +
            std::to_string(static_cast<uint8_t>(packet.data)));
        break;
    }
    case NETMAN_TYPES::SET_NOROUTEACTION: { // Set no route action
        // Deserialize packet
        GenericRnpPacket packet(*packet_ptr);

        // Set no route action
        _config.noRouteAction = static_cast<NOROUTE_ACTION>(packet.data);

        // Log change in no route action
        log("Node NoRouteAction is now " +
            std::to_string(static_cast<uint8_t>(packet.data)));
        break;
    }
    case NETMAN_TYPES::SET_ROUTEGEN: { // Set automatic route generation
        // Deserialize packet
        GenericRnpPacket packet(*packet_ptr);

        // Set automatic route generation
        _config.routeGenEnabled = static_cast<bool>(packet.data);

        // Log change in automatic route generation
        log("Node RouteGen is now " +
            std::to_string(static_cast<bool>(packet.data)));
        break;
    }
    case NETMAN_TYPES::SAVE_CONF: { // Save configuration
        // Check for save configuration implementation
        if (_saveConfigImpl) {
            // Save configuration, log if successful
            _saveConfigImpl(_config) ? log("Configuration Failed to Save!")
                                     : log("Configuration Saved!");
        } else {
            // Log lack of save configuration implementation
            log("No Save Configuration Implementation Provided - Not Saved!");
        }

        break;
    }
    case NETMAN_TYPES::RESET_NETMAN: { // Reset network manager
        // Reset network manager
        reset();
        break;
    }
    default: {
        break;
    }
    }
};

void RnpNetworkManager::forwardPacket(RnpPacket &packet) {
    // Check if the packet is from debug and has no address
    if (packet.header.source == static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG) &&
        packet.header.source_service ==
            static_cast<uint8_t>(DEFAULT_SERVICES::NOSERVICE)) {
        // Update source address to node's current address
        packet.header.source = _config.currentAddress;
    } else if (_config.nodeType != NODETYPE::HUB) {
        // Dump packet if the node is not a hub
        return;
    };

    // Send the packet
    sendPacket(packet);
}

void RnpNetworkManager::log(const std::string &msg) {
    // Do nothing if logging is disabled
    if (!_loggingEnabled) {
        return;
    }

    // Use default logging if no logging callback is set
    if (!_logcb) {
#if defined(ARDUINO)
        // Print log message to serial
        Serial.println(msg.c_str());
#else
        // Print log message to standard output
        std::cout << msg << "\n";
#endif
        return;
    }

    // Log via the logging callback
    _logcb(msg);
};