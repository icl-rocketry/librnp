#pragma once

#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

/**
 * @brief Structure for network routes
 *
 * @author Kiran de Silva *
 */
struct Route {
    /// @brief Inteface
    uint8_t iface;

    /// @brief Metric
    uint8_t metric;

    /// @brief Address
    std::variant<std::monostate, std::string> address;
};

/**
 * @brief Class for a routing table
 *
 * @author Kiran de Silva
 */
class RoutingTable {
public:
    /**
     * @brief Construct a new Routing Table object
     *
     * @author Kiran de Silva
     */
    RoutingTable(){};

    /**
     * @brief Construct a new Routing Table object
     *
     * @author Kiran de Silva
     *
     * @param[in] destinations Route destinations
     */
    RoutingTable(int destinations) : _table(destinations){};

    /**
     * @brief Return routing table size
     *
     * @author Kiran de Silva
     *
     * @return size_t Routing table size
     */
    size_t size() {
        // Return routing table size
        return _table.size();
    };

    /**
     * @brief Sets a route for a given destination
     *
     * @warning If a route already exists for a given destination, it will be
     * overwritten
     *
     * @author Kiran de Silva
     *
     * @param[in] destination
     * @param[in] entry
     */
    void setRoute(const uint8_t destination, const Route &entry) {
        // Resize the table if the destination exceeds the table size
        if (destination >= _table.size()) {
            _table.resize(destination + 1);
        }

        // Set route for the given destination
        _table.at(destination) = {true, entry};
    };

    /**
     * @brief Get the route for a given destination
     *
     * @author Kiran de Silva
     *
     * @param[in] destination Destination
     * @return std::optional<Route> Route
     */
    std::optional<Route> getRoute(const uint8_t destination) {
        // Return a blank route if the destination is out of bounds
        if (destination >= _table.size()) {
            return {};
        }

        // Extract route from routing table
        std::pair<bool, Route> result = _table.at(destination);

        // Return a blank route if there is no data at index
        if (!std::get<bool>(result)) {
            return {};
        }

        // Return the route
        return {std::get<Route>(result)};
    };

    /**
     * @brief Delete a route for a given destination
     *
     * @author Kiran de Silva
     *
     * @param[in] destination Destination
     */
    void deleteRoute(const uint8_t destination) {
        // Return if no route for the given destination
        if (destination >= _table.size()) {
            return;
        }

        // Fully erase route if it is the last destination
        if (destination == (_table.size() - 1)) {
            _table.erase(_table.end());
            return;
        }

        // Set blank route
        _table.at(destination) = {false, Route()};
    }

    /**
     * @brief Clear the routing table
     *
     * @author Kiran de Silva
     */
    void clearTable() { _table.clear(); }

    /**
     * @brief Load routing table from JSON
     *
     * @author Kiran de Silva
     */
    void loadJson(){}; // loads table from supplied json string

    /**
     * @brief Save routing table to JSON
     *
     * @author Kiran de Silva
     */
    void saveJson(){};

    /**
     * @brief Convert routing table to string stream
     *
     * @author Kiran de Silva
     *
     * @return std::stringstream Routing table string stream
     */
    std::stringstream printTable() {
        // Declare string stream
        std::stringstream sout;

        // Output header
        sout << ">>>ROUTING TABLE<<<"
             << "\n";
        sout << "|destination|iface|metric|link layer address|"
             << "\n";

        // Iterate through elements
        for (size_t i = 0; i < _table.size(); i++) {
            // Extract element
            auto elem = _table.at(i);

            // Print element number
            sout << "| " << i;

            // Check if there is no route
            if (!std::get<bool>(elem)) {
                // Print no route
                sout << " | - NO ROUTE - "
                     << "\n";
            } else {
                // Extract route
                Route r = std::get<Route>(elem);

                // Output interface and metric
                sout << " | " << (int)r.iface << " | " << (int)r.metric
                     << " | ";

                // Check for address
                if (std::holds_alternative<std::monostate>(r.address)) {
                    // cant visit the variant as
                    // std::monostate does not have a ostream
                    // operator defined

                    // Output lack of address
                    sout << " - NO ADDRESS - |";
                } else if (std::holds_alternative<std::string>(r.address)) {
                    // Output address
                    sout << std::get<std::string>(r.address) << " |";
                } else {
                    // Output invalid address
                    sout << " INVALID ADDRESS TYPE |";
                }
            }

            // Output newline
            sout << "\n";
        }

        // Return string stream
        return sout;
    }

private:
    /// @brief Routing table
    std::vector<std::pair<bool, Route>> _table;
};
