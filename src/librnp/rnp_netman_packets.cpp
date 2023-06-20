#include "rnp_netman_packets.h"

#include <cstring>

#include "rnp_header.h"
#include "rnp_networkmanager.h"
#include "rnp_packet.h"
#include "rnp_routingtable.h"
#include "rnp_serializer.h"

SetRoutePacket::~SetRoutePacket(){};

SetRoutePacket::SetRoutePacket(const uint8_t dest, const Route &route)
    : RnpPacket(static_cast<uint8_t>(DEFAULT_SERVICES::NETMAN),
                static_cast<uint8_t>(NETMAN_TYPES::SET_ROUTE), size()),
      destination(dest), iface(route.iface), metric(route.metric) {
    // Switch address type
    switch (route.address.index()) {
    case 0: { // No type
        // Set address type and length
        address_type = (uint8_t)ADDRESS_TYPE::NOTYPE;
        address_len = 0;
        break;
    }
    case 1: { // String
        // set address string, type, and length
        const std::string &address_string =
            std::get<std::string>(route.address);
        address_type = (uint8_t)ADDRESS_TYPE::STRING;
        address_len = address_string.size();
        std::memcpy(address_data.data(), address_string.data(), address_len);
        break;
    }
    default: { // Unrecognised type
        // Set address type and length
        address_type = (uint8_t)ADDRESS_TYPE::NOTYPE;
        address_len = 0;
        break;
    }
    }
};

SetRoutePacket::SetRoutePacket(const RnpPacketSerialized &packet)
    : RnpPacket(packet, size()) {
    // Deserialize packet
    getSerializer().deserialize(*this, packet.getBody());

    // Calculate offset (not including the payload)
    const size_t offset = header.size() + getSerializer().member_size();

    // Copy the address data
    std::memcpy(address_data.data(), packet.packet.data() + offset,
                address_len);
};

void SetRoutePacket::serialize(std::vector<uint8_t> &buf) {
    // Serialize packet to buffer
    RnpPacket::serialize(buf);

    // Extract buffer and member sizes
    const size_t bufsize = buf.size();
    const size_t member_size = getSerializer().member_size();

    // Resize buffer
    buf.resize(bufsize + size());

    // Copy data serialized by the serializer
    std::memcpy(buf.data() + bufsize, getSerializer().serialize(*this).data(),
                member_size);

    // Copy data not serialized by the serializer
    std::memcpy(buf.data() + bufsize + member_size, address_data.data(),
                address_len);
}

Route SetRoutePacket::getRoute() {
    // Create route
    Route ret{iface, metric};

    // Switch based on address type
    switch (address_type) {
    case (uint8_t)ADDRESS_TYPE::NOTYPE: { // No type
        // Return route
        return ret;
    }
    case (uint8_t)ADDRESS_TYPE::STRING: { // String
        // Create string from address data (unsigned char* -> char*)
        std::string address(reinterpret_cast<char *>(address_data.data()),
                            address_len);

        // Update route address
        ret.address = address;

        // Return route
        return ret;
    }
    default: { // Unrecognised type
        // Return route
        return ret;
    }
    }
};