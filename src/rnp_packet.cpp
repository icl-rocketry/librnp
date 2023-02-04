#include "rnp_packet.h"
#include "rnp_header.h"

#include <vector>

RnpPacket::~RnpPacket(){};

RnpPacket::RnpPacket(const uint8_t packetService, const uint8_t packetType,
                     const uint16_t packetSize)
    : header(packetService, packetType, packetSize){};

RnpPacket::RnpPacket(const RnpHeader header) : header(header){};

RnpPacket::RnpPacket(const RnpPacketSerialized &serializedPacket, size_t size)
    : header(serializedPacket.header) {
    // Throw error if header size does not match expected size
    if (header.packet_len != size) {
        throw std::runtime_error("Header size does not match expected size!");
    }

    // Throw error if body size does not match expected size
    if (serializedPacket.getBodySize() != size) {
        throw std::runtime_error("Buffer len does not match expected size!");
    }
};

void RnpPacket::serialize(std::vector<uint8_t> &buf) {
    // Serialize the header to the buffer
    header.serialize(buf);
}

RnpPacketSerialized::~RnpPacketSerialized(){};

RnpPacketSerialized::RnpPacketSerialized(const std::vector<uint8_t> &bytes)
    : RnpPacket(RnpHeader(bytes)), packet(bytes){};

void RnpPacketSerialized::reserializeHeader() {
    // Declare serialized header bytes buffer
    std::vector<uint8_t> header_serialized;

    // Serialize header to the buffer
    header.serialize(header_serialized);

    // Replace original header in the serialized packet
    std::memcpy(packet.data(), header_serialized.data(),
                header_serialized.size());
}

void RnpPacketSerialized::serialize(std::vector<uint8_t> &buf) {
    // Re-serialize header
    reserializeHeader();

    // Extract buffer size
    size_t bufsize = buf.size();

    // Resize buffer to include the packet
    buf.resize(bufsize + packet.size());

    // Copy the packet to the buffer
    std::memcpy(buf.data() + bufsize, packet.data(), packet.size());
};

std::vector<uint8_t> RnpPacketSerialized::getBody() const {
    // Return the packet body
    return std::vector<uint8_t>{packet.begin() + header.size(), packet.end()};
}

size_t RnpPacketSerialized::getBodySize() const {
    // Return the the size of the body, without returning a negative number
    return (packet.size() < header.size()) ? (0)
                                           : (packet.size() - header.size());
}