# Wireshark Lua Dissectors

## Basic Dissector
```lua 
-- dissector.lua

-- Create a protocol
parent = Proto("Parent", "Parent Protocol")

-- Add the fields to the protocol
parent.fields = {

}

-- Dissect packets associated with the protocol port
function parent.dissector(buffer, pinfo, tree)
    -- Check the length of the incoming buffer, if it's zero, there's nothing to do, return
    length = buffer:len()
    if length == 0 then
        return
    end

    -- Add the name "Parent" to the "protocol" column of wireshark packet pane
    pinfo.cols.protocol = parent.name

    -- Create a tree item in the packet details pane
    -- Add the text "Parent Data" to the new tree and
    -- associate the data with that text
    local subtree = tree:add(parent, buffer(), "Parent Data")
end

-- Register the dissector on certain protocol ports
-- this dissector should handle packets that come in on 
-- tcp port 10000, and udp port 4325
local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(10000, parent)
local udp_port = DissectorTable.get("udp.port")
udp_port:add(4325, parent)
```

buffer => The "data" portion of a given frame  
pinfo  => the packet information pane of wireshark  
tree   => the packet details pane of wireshark

Question: Is there a way for lua to access the packet bytes pane?
Answer: 

Question: what is the difference between buffer:len() and buffer.len()?  
Answer: same thing, except buffer:len() is the same as saying buffer.len(buffer)

```lua
buffer = {
    function len()
        --some sort of length function here.
    end
}
```

------

