#include "simplecommandpacket.h"

#include "../rnp_packet.h"
#include "../rnp_serializer.h"

SimpleCommandPacket::~SimpleCommandPacket(){};

SimpleCommandPacket::SimpleCommandPacket(const uint8_t _command,
                                         const uint32_t _arg)
    : RnpPacket(0, 0, size()), command(_command), arg(_arg){};

SimpleCommandPacket::SimpleCommandPacket(const RnpPacketSerialized &packet)
    : RnpPacket(packet, size()) {
    // Deserialize packet and store
    getSerializer().deserialize(*this, packet.getBody());
};

void SimpleCommandPacket::serialize(std::vector<uint8_t> &buf) {
    // Serialize packet into buffer
    RnpPacket::serialize(buf);

    // Extract buffer size
    size_t bufsize = buf.size();

    // Resize buffer to include the packet
    buf.resize(bufsize + size());

    // Copy packet onto end of buffer
    std::memcpy(buf.data() + bufsize, getSerializer().serialize(*this).data(),
                size());
};

command_t CommandPacket::getCommand(const RnpPacketSerialized &packet) {
    // Extract packet header size
    const size_t headerSize = packet.header.size();

    // Declare command identifier
    command_t commandID;

    // Extract command identifier from packet
    std::memcpy(&commandID, packet.packet.data() + headerSize,
                sizeof(command_t));

    // Return command identifier
    return commandID;
}
