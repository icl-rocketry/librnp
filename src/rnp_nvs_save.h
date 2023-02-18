#pragma once

#include <string>

#include "rnp_networkmanager.h"

#if (defined ESP32 && defined ARDUINO)
#include "Preferences.h"

namespace RnpNvsSave {

    /**
     * @brief Save network configuration to non-volatile storage
     *
     * @warning Routing table not stored
     *
     * @author Kiran de Silva
     *
     * @param[in] config Network configuration
     * @return true Save unsuccessful
     * @return false Save successful
     */
    static bool SaveToNVS(RnpNetworkManagerConfig const &config) {
        // Declare preferences
        Preferences pref;

        // Return error if preferences does not start with Rnp_Config
        if (!pref.begin("Rnp_Config")) {
            return true;
        }

        // Declare error variable
        int error = 0;

        // Save configuration
        pref.putUChar("address", config.currentAddress) ?: error++;
        pref.putUChar("nodeType", static_cast<uint8_t>(config.nodeType)) ?: error++;
        pref.putUChar("noRouteAction", static_cast<uint8_t>(config.noRouteAction)) ?: error++;
        pref.putBool("routeGen", config.routeGenEnabled) ?: error++;

        // Return error status
        return error;
    };

    /**
     * @brief Read network configuration from non-volatile storage
     *
     * @warning Routing table not loaded
     *
     * @todo Rewrite preferences for identifying incorrect reads
     *
     * @author Kiran de Silva
     *
     * @param[out] config Network configuration
     * @return true Read not successful
     * @return false Read successful
     */
    static bool ReadFromNVS(RnpNetworkManagerConfig &config) {
        // Declare preferences
        Preferences pref;

        // Return error if preferences does not start with Rnp_Config
        if (!pref.begin("Rnp_Config")) {
            return true;
        }

        // Load configuration
        config.currentAddress = pref.getUChar("address", 0);
        config.nodeType = static_cast<NODETYPE>(pref.getUChar("nodeType"));
        config.noRouteAction = static_cast<NOROUTE_ACTION>(pref.getUChar("noRouteAction"));
        config.routeGenEnabled = pref.getBool("routeGen", config.routeGenEnabled);

        // Return unsuccessful read if the current address is illegal
        if (config.currentAddress == 0) {
            return true;
        }

        // Return successful read
        return false;
    };
}; // namespace RnpNvsSave

#else

#include <iostream>

namespace RnpNvsSave {

    /**
     * @brief Print network configuration on standard output
     *
     * @warning Routing table not printed
     *
     * @author Kiran de Silva
     *
     * @param[in] config Configuration
     * @return true N/A
     * @return false Output successful
     */
    static bool SaveToNVS(RnpNetworkManagerConfig const &config) {
        // Print configuration
        std::cout << "Address:" + std::to_string(config.currentAddress) << "\n";
        std::cout << "nodeType:" + std::to_string(static_cast<uint8_t>(config.nodeType))<< "\n";
        std::cout << "noRouteAction:" + std::to_string(static_cast<uint8_t>(config.noRouteAction)) << "\n";
        std::cout << "routeGenEnabled:" + std::to_string(config.routeGenEnabled) << "\n";

        // Return successful output
        return false;
    };

    /**
     * @brief Set fake network configuration
     *
     * @warning Routing table not set
     *
     * @author Kiran de Silva
     *
     * @param[out] config Network configuration
     * @return true N/A
     * @return false Input successful
     */
    static bool ReadFromNVS(RnpNetworkManagerConfig &config) {
        // Set fake data
        config.currentAddress = 10;
        config.nodeType = static_cast<NODETYPE>(1);
        config.noRouteAction = static_cast<NOROUTE_ACTION>(1);
        config.routeGenEnabled = 1;

        // Return successful input
        return false;
    }

}; // namespace RnpNvsSave

#endif