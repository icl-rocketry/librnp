#pragma once

#include <memory>
#include <string>
#include <vector>

#include <librnp/rnp_interface.h>
#include <librnp/printer.h>

/**
 * @brief MockInterface class
 *
 * @author Kiran de Silva
 */
class MockInterface : public Printer {
public:
    /**
     * @brief Construct a new MockInterface object
     *
     * @author Kiran de Silva
     *
     * @param[in] id Identifier
     * @param[in] name Name
     */
    MockInterface(const uint8_t id, const std::string name = "MockInterface"):
    Printer(id,name)
    {};

    void placeOnPacketBuffer(std::unique_ptr<RnpPacketSerialized> packet_ptr)
    {

        if (_packetBuffer == nullptr) {
            return;
        }

        // Update packet source interface
        packet_ptr->header.src_iface = getID();

        // Push packet on to interface packet buffer
        _packetBuffer->push(std::move(packet_ptr));
    }

    /**
     * @brief Destroy the MockInterface object
     *
     * @author Kiran de Silva
     */
    ~MockInterface(){};

private:
    /// @brief MockInterface info
    RnpInterfaceInfo info;
};