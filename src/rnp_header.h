#pragma once

#include <sstream>
#include <variant>
#include <vector>

#include "rnp_serializer.h"

/**
 * @brief Packet Header class
 *
 * @author Kiran de Silva
 */
class RnpHeader {

private:
    /**
     * @brief Get the Header serializer
     *
     * @author Kiran de Silva
     *
     * @return constexpr auto Header serializer
     */
    static constexpr auto getSerializer() {
        // Generate serializer
        auto ret = RnpSerializer(&RnpHeader::start_byte, &RnpHeader::packet_len,
                                 &RnpHeader::uid, &RnpHeader::source_service,
                                 &RnpHeader::destination_service,
                                 &RnpHeader::type, &RnpHeader::source,
                                 &RnpHeader::destination, &RnpHeader::hops);

        //  Return serializer
        return ret;
    }

public:
    /**
     * @brief Construct a new Header object
     *
     * @author Kiran de Silva
     */
    RnpHeader();

    /**
     * @brief Construct a new Header object
     *
     * @author Kiran de Silva
     *
     * @param[in] packetService Service
     * @param[in] packetType Type
     * @param[in] packetSize Size
     */
    RnpHeader(const uint8_t packetService, const uint8_t packetType,
              const uint16_t packetSize);

    /**
     * @brief Construct a new Header object
     *
     * Construct by deserializing existing packet
     *
     * @param[in] data Packet
     */
    RnpHeader(const std::vector<uint8_t> &data);

    /**
     * @brief Destroy the Header object
     *
     * @author Kiran de Silva
     */
    ~RnpHeader();

    /**
     * @brief Serialize Header
     *
     * @todo Implement serialize with offset
     *
     * @author Kiran de Silva
     *
     * @param[in] buf Output buffer
     */
    void serialize(std::vector<uint8_t> &buf) const;

    /// @brief Header start byte
    uint8_t start_byte = 0xAF;

    /// @brief Packet payload length
    uint16_t packet_len = 0x00;

    /// @brief Packet unique identifier
    uint16_t uid = 0x00000000;

    /// @brief Source service identifier
    uint8_t source_service = 0x00;

    /// @brief Destination service identifier
    uint8_t destination_service = 0x00;

    /// @brief Packet type
    uint8_t type = 0x00;

    /// @brief Packet source address
    uint8_t source = 0x00;

    /// @brief Packet destination address
    uint8_t destination = 0x00;

    /// @brief Number of hops (unused)
    uint8_t hops = 0x00;

    /// @brief Source interface
    uint8_t src_iface;

    /// @brief Link layer address
    std::variant<std::monostate, std::string> lladdress;

    /**
     * @brief Get size of Header
     *
     * @author Kiran de Silva
     *
     * @return constexpr size_t Header size
     */
    static constexpr size_t size() {
        // Return header size
        return getSerializer().member_size();
    }

    /**
     * @brief Generate string stream from Header
     *
     * @author Kiran de Silva
     *
     * @param[in] header Header
     * @return std::stringstream Header as string stream
     */
    static std::stringstream print(const RnpHeader &header) {
        // Declare string stream
        std::stringstream aout;

        // Shift title to stream
        aout << ">>>HEADER<<<\n";

        // Shift header contents to stream
        aout << "start_byte: " << (int)header.start_byte << "\n";
        aout << "packet_len: " << (int)header.packet_len << "\n";
        aout << "uid: " << (int)header.uid << "\n";
        aout << "source_service: " << (int)header.source_service << "\n";
        aout << "destination_service: " << (int)header.destination_service
             << "\n";
        aout << "type: " << (int)header.type << "\n";
        aout << "source: " << (int)header.source << "\n";
        aout << "destination: " << (int)header.destination << "\n";
        aout << "hops: " << (int)header.hops << "\n";

        // Return stream
        return aout;
    }

    /**
     * @brief Generate response Header
     *
     * @author Kiran de Silva
     *
     * @param[in] requestHeader Request header
     * @param[out] responseHeader Response header
     */
    static void generateResponseHeader(const RnpHeader &requestHeader,
                                       RnpHeader &responseHeader) {
        // Copy unique identifier to response header
        responseHeader.uid = requestHeader.uid;

        // Switch source and destination addresses and services
        responseHeader.source = requestHeader.destination;
        responseHeader.source_service = requestHeader.destination_service;
        responseHeader.destination = requestHeader.source;
        responseHeader.destination_service = requestHeader.source_service;
    }
};