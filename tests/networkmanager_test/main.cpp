
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>

#include <iomanip>
#include <bitset>
#include <array>

#include <fstream>

#include <librnp/rnp_networkmanager.h>
#include "mockInterface.h"



RnpNetworkManager networkmanager(254,NODETYPE::LEAF,true,200);
MockInterface mock0(1,"mock0");

int main()
{

    //register logging callback
    networkmanager.setLogCb([](const std::string& message){std::cout << message << std::endl;});

    // register debug interface
    networkmanager.addInterface(&mock0);

    //generate default network routes in the routing table
    networkmanager.generateDefaultRoutes();            

    //setup default config of network manager
    networkmanager.enableAutoRouteGen(false);
    networkmanager.setNoRouteAction(NOROUTE_ACTION::BROADCAST,{1});

    std::vector<uint8_t> serializedData;

    using MessagePacket = MessagePacket_Base<10,10>;
    MessagePacket msgp("Test Packet!");

    msgp.header.source = 0;
    msgp.header.destination = 0;
    msgp.serialize(serializedData);
    
    mock0.placeOnPacketBuffer(std::make_unique<RnpPacketSerialized>(serializedData)); // this should fail
    networkmanager.update(); // run update loop

    msgp.header.source = 254;
    msgp.header.destination = 2;

    serializedData.clear();
    msgp.serialize(serializedData);

    mock0.placeOnPacketBuffer(std::make_unique<RnpPacketSerialized>(serializedData)); // this should fail
    networkmanager.update();

    msgp.header.start_byte = 0xFF;
    
    serializedData.clear();
    msgp.serialize(serializedData);

    mock0.placeOnPacketBuffer(std::make_unique<RnpPacketSerialized>(serializedData)); // this should fail
    networkmanager.update();
    

    return 0;
}