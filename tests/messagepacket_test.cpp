
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>

#include <iomanip>
#include <bitset>
#include <array>

#include <fstream>

#include <librnp/rnp_networkmanager.h>






int main()
{
    using MessagePacket = MessagePacket_Base<10,10>;
    MessagePacket msgp("Test Packet!");
    std::vector<uint8_t> sermsgp;
    msgp.serialize(sermsgp);

    for (auto elem : sermsgp){
        std::cout<<std::hex<<(int)elem<<",";
    }

    RnpPacketSerialized newpacket(sermsgp);

    MessagePacket decoded(newpacket);
    std::cout<<RnpHeader::print(decoded.header).str()<<std::endl;
    std::cout<<decoded._msg<<std::endl;

    return 0;
}