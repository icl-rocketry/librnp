#include "printer.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "rnp_interface.h"

Printer::Printer(const uint8_t id, const std::string name)
    : RnpInterface(id, name){};

void Printer::setup(){};

void Printer::update(){};

void Printer::sendPacket(RnpPacket &data) {
    // Declare output buffer
    std::vector<uint8_t> serializedData;

    // Serialize packet
    data.serialize(serializedData);

    // Declare string stream
    std::stringstream aout;

    // Shift title into stream
    aout << "Packet:\n";

    // Shift packet header into stream
    aout << RnpHeader::print(data.header).str() << "\n";

    // Shift packet elements into stream
    for (auto &elem : serializedData) {
        aout << std::hex << std::to_string((int)elem) << ",";
    }

    // Shift newline into stream
    aout << "\n";

#ifdef ARDUINO
    // Shift stream into serial output
    Serial.println(aout.str().c_str());
#else
    // Shift stream into standard output
    std::cout << aout.str() << std::endl;
#endif
};
