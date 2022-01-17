//packet.cpp
#include "rnp_header.h"
#include <stdexcept>
#include <cstring>


RnpHeader::RnpHeader() {}
RnpHeader::~RnpHeader() {}

RnpHeader::RnpHeader(uint8_t destinationService, uint8_t packetType, uint16_t packetSize):
packet_len{packetSize},
source_service{0},//default is zero
destination_service{destinationService},
type{packetType},
hops{0}
{};

RnpHeader::RnpHeader(const std::vector<uint8_t> &data) {
	getSerializer().deserialize(*this, data); 
}

void RnpHeader::serialize(std::vector<uint8_t>& buf) const{ 

	size_t bufsize = buf.size();
	buf.resize(bufsize + size());

	std::memcpy(buf.data() + bufsize,
		   getSerializer().serialize(*this).data(),
		   size()
		   ); 
};
