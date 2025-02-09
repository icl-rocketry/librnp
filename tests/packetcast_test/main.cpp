
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>

#include <iomanip>
#include <bitset>
#include <array>



#include <librnp/rnp_networkmanager.h>
#include <librnp/default_packets/simplecommandpacket.h>






int main()
{
    //construct command packet
    SimpleCommandPacket command(1,10);

    //cast to RnpPacket reference
    RnpPacket &rnpPacket_ref = static_cast<RnpPacket&>(command);

    //cast back to SimpleCommandPacket reference

    SimpleCommandPacket &command_ref = static_cast<SimpleCommandPacket&>(rnpPacket_ref);

    std::cout<<std::to_string(command_ref.command)<<" - "<<std::to_string(command_ref.arg)<<std::endl;
    return 0;
}