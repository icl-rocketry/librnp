--[[
    Author: WDissectorGen
    Language: Lua
    Date: 2022-04-06
    Description: Wireshark Dissector for rnp
]]--

rnp = Proto("rnp", "rnp Protocol")

-- Fields Declaration Section
start_byte_field=ProtoField.uint8("rnp.header.start_byte","Start Byte",base.HEX)
packet_len_field=ProtoField.uint16("rnp.header.packet_len","Packet Length",base.DEC)
uid_field=ProtoField.uint16("rnp.header.uid","Unique Identifier",base.DEC)
source_service_field=ProtoField.uint8("rnp.header.source_service","Source Service",base.DEC)
destination_service_field=ProtoField.uint8("rnp.header.destination_service","Destination Service",base.DEC)
type_field=ProtoField.uint8("rnp.header.type","Packet Type",base.DEC)
source_field=ProtoField.uint8("rnp.header.source","Source Address",base.DEC)
destination_field=ProtoField.uint8("rnp.header.destination","Destination Address",base.DEC)
hops_field=ProtoField.uint8("rnp.header.hops","Hops",base.DEC)

rnp.fields = {
    start_byte_field,
packet_len_field,
uid_field,
source_service_field,
destination_service_field,
type_field,
source_field,
destination_field,
hops_field
}

-- Dissector Callback Declaration
function rnp.dissector(buffer, pinfo, tree)
    length = buffer:len()
    if length == 0 then return end

    -- Adds protocol name to protocol column
    pinfo.cols.protocol = rnp.name
    
    -- Creates the subtree
    local subtree = tree:add(rnp, buffer(),"rnp Protocol Data")

    -- Local Variables Declaration
    local start_byte = buffer(0,1)
local packet_len = buffer(1,2)
local uid = buffer(3,2)
local source_service = buffer(5,1)
local destination_service = buffer(6,1)
local type = buffer(7,1)
local source = buffer(8,1)
local destination = buffer(9,1)
local hops = buffer(10,1)

    -- Adds Variables to the subtree
    subtree:add(start_byte_field, start_byte)
subtree:add_le(packet_len_field, packet_len)
subtree:add_le(uid_field, uid)
subtree:add(source_service_field, source_service)
subtree:add(destination_service_field, destination_service)
subtree:add(type_field, type)
subtree:add(source_field, source)
subtree:add(destination_field, destination)
subtree:add(hops_field, hops)
end



local udp_port = DissectorTable.get("udp.port")
udp_port:add(8888, rnp)
