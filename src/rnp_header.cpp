#include "rnp_header.h"

#include <cstring>
#include <stdexcept>

RnpHeader::RnpHeader() {}

RnpHeader::~RnpHeader() {}

RnpHeader::RnpHeader(uint8_t destinationService, uint8_t packetType,
                     uint16_t packetSize)
    : packet_len{packetSize}, source_service{0},
      destination_service{destinationService}, type{packetType}, hops{0} {};

RnpHeader::RnpHeader(const std::vector<uint8_t> &data) {
    // Throw error if the buffer is too small for deserialization
    if (data.size() < size()) {
        throw std::runtime_error("Buffer too small to deserialize header from");
    };

    // Deserialize header
    getSerializer().deserialize(*this, data);
}

void RnpHeader::serialize(std::vector<uint8_t> &buf) const {
    // Extract buffer size
    size_t bufsize = buf.size();

    // Resize buffer to include header
    buf.resize(bufsize + size());

    // Copy header to the end of the buffer
    std::memcpy(buf.data() + bufsize, getSerializer().serialize(*this).data(),
                size());
};
