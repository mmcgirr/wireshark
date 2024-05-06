# Wireshark Lua Dissectors

## A note from one of the core wireshark devs on "internal usage"
```block
if you mean automated testing (i.e., so it can be repeated for regression, etc.), then what we do in Wireshark itself to test them is run them via a shell script by using tshark with the "-X lua_script:<script>" option, in verbose mode output to a text file, and then verify the output file has what we expect it to have. (for example by using another Lua script that checks the output text file)

If you clone the repo, or browse it online, you can see that in the test/suite-wslua.sh file, which is the Lua API test suite shell script. And the Lua scripts that are tested are in the test/lua directory.
```
from here: https://stackoverflow.com/questions/32120692/testing-wireshark-plugins

## Basic Dissector
<details> 
  <summary>dissector.lua code listing</summary>

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
</details>

### Sample capture that illustrates the dissector in action.
<details>
<summary>Sample pcapng for dissector.lua</summary>

</details>

### Notes on the basic dissector
<details>
<summary>Line by line summary</summary>
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
</details>

------

