#pragma once

#include <memory>
#include <string>
#include <vector>

#include "rnp_interface.h"

/**
 * @brief Loopback Information structure
 *
 * @warning Empty structure
 *
 * @author Kiran de Silva
 */
struct LoopbackInfo : public RnpInterfaceInfo {};

/**
 * @brief Loopback class
 *
 * @author Kiran de Silva
 */
class Loopback : public RnpInterface {
public:
    /**
     * @brief Construct a new Loopback object
     *
     * @author Kiran de Silva
     *
     * @param[in] name Loopback name
     */
    Loopback(const std::string name = "loopback1");

    /**
     * @brief Set up Loopback
     *
     * @author Kiran de Silva
     */
    void setup() override;

    /**
     * @brief Update Loopback
     *
     * @author Kiran de Silva
     */
    void update() override;

    /**
     * @brief Send packet
     *
     * @author Kiran de Silva
     *
     * @param[in] data Packet
     */
    void sendPacket(RnpPacket &data) override;

    /**
     * @brief Get Loopback information
     *
     * @author Kiran de Silva
     *
     * @return const RnpInterfaceInfo* Loopback information
     */
    const RnpInterfaceInfo *getInfo() override {
        // Return Loopback information
        return &info;
    };

    /**
     * @brief Destroy the Loopback object
     * 
     * @author Kiran de Silva
     */
    ~Loopback(){};

private:
    /// @brief Loopback information
    LoopbackInfo info;
};