#pragma once

#include <memory>
#include <string>
#include <vector>

#include "rnp_interface.h"

/**
 * @brief Printer class
 *
 * @author Kiran de Silva
 */
class Printer : public RnpInterface {
public:
    /**
     * @brief Construct a new Printer object
     *
     * @author Kiran de Silva
     *
     * @param[in] id Identifier
     * @param[in] name Name
     */
    Printer(const uint8_t id, const std::string name = "Printer");

    /**
     * @brief Set up Printer
     *
     * @author Kiran de Silva
     */
    void setup() override;

    /**
     * @brief Update Printer
     *
     * @author Kiran de Silva
     */
    void update() override;

    /**
     * @brief Send packet
     *
     * On Arduino, the packet is printed over serial. Otherwise, it is printed
     * on standard output.
     *
     * @author Kiran de Silva
     *
     * @param[in] data Packet
     */
    void sendPacket(RnpPacket &data) override;

    /**
     * @brief Get Printer info
     *
     * @author Kiran de Silva
     *
     * @return const RnpInterfaceInfo* Printer info
     */
    const RnpInterfaceInfo *getInfo() override {
        // Return Printer info
        return &info;
    };

    /**
     * @brief Destroy the Printer object
     *
     * @author Kiran de Silva
     */
    ~Printer(){};

private:
    /// @brief Printer info
    RnpInterfaceInfo info;
};