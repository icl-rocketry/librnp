#include "loopback.h"

#include <memory>
#include <string>
#include <vector>

#include "rnp_interface.h"

Loopback::Loopback(const std::string name)
    : RnpInterface((uint8_t)DEFAULT_INTERFACES::LOOPBACK, name){};

void Loopback::setup(){};

void Loopback::update(){};

void Loopback::sendPacket(RnpPacket &data) {
    // Return if no buffer is present
    if (_packetBuffer == nullptr) {
        return;
    }

    // Declare buffer
    std::vector<uint8_t> serializedData;

    // Serialize packet into buffer
    data.serialize(serializedData);

    // Get pointer to packet buffer
    auto packet_ptr = std::make_unique<RnpPacketSerialized>(serializedData);

    // Update packet source interface
    packet_ptr->header.src_iface = getID();

    // Push packet on to interface packet buffer
    _packetBuffer->push(std::move(packet_ptr));
};
