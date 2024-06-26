# Wireshark Information

Some wireshark notes (including writing lua dissectors) that I'm taking so I have all the references in one place.

- [Wireshark Information](#wireshark-information)
  - [Videos:](#videos)
  - [Wireshark Architecture Block Diagram](#wireshark-architecture-block-diagram)
  - [Wireshark Docs:](#wireshark-docs)
    - [Official Docs](#official-docs)
    - [Official Videos](#official-videos)
    - [Lua-Specific resources](#lua-specific-resources)
    - [Sample Captures to Work With](#sample-captures-to-work-with)
    - [Packet Generators / Tools](#packet-generators--tools)
    - [My Docs](#my-docs)
    - [Other Docs](#other-docs)
  - [Dissectors](#dissectors)
    - [Videos](#videos-1)
    - [3rd Party Git Repos and Docs](#3rd-party-git-repos-and-docs)


## Videos:

## Wireshark Architecture Block Diagram
![Functional Diagram](images/ws-function-blocks.svg)

## Wireshark Docs:
### Official Docs
[User Guide - One Page](https://www.wireshark.org/docs/wsug_html/)  
[Dev Guide - One Page](https://www.wireshark.org/docs/wsdg_html/)
[Wireshark Wiki](https://wiki.wireshark.org/)

[Hansang Bae Traces Files](https://app.box.com/v/HSBBook)

### Official Videos
[Sharkfest Videos](https://www.youtube.com/channel/UCHBY7sUVdWK4bOSe7khG0UA)

### Lua-Specific resources
[Dev Guide - Lua Intro](https://www.wireshark.org/docs/wsdg_html/#wsluarm)  
[Dev Guide - Lua API](https://www.wireshark.org/docs/wsdg_html/#wsluarm_modules)
[Wireshark Wiki - LUA](https://wiki.wireshark.org/Lua/)
[Wireshark WSLUA Readme](https://gitlab.com/wireshark/wireshark/-/blob/master/doc/README.wslua)
[Wireshark Gitlab Wiki Link](https://gitlab.com/wireshark/wireshark/-/wikis/home)
[Wireshark How to Dissect Anything](https://wiki.wireshark.org/HowToDissectAnything)

[Wireshark Lua Files](https://github.com/wireshark/wireshark/tree/master/test/lua)
[Lua Examples - Wiki](https://wiki.wireshark.org/Lua/Examples)
[Lua Post-Dissector](https://wiki.wireshark.org/Lua/Examples/PostDissector)
[Lua Contrib](https://gitlab.com/wireshark/editor-wiki/-/wikis/Contrib)
[Peter Wu Postdissector Example](https://gist.github.com/Lekensteyn/288ea8c2f8efdf87819f8489a75b5475)
[Peter Wu Gists](https://gist.github.com/Lekensteyn)
[Lua Post-Dissector Filter Cols](https://wiki.wireshark.org/Lua/Examples/filtcols)
[Port-Independent (Heuristic) Dissectors](https://mika-s.github.io/wireshark/lua/dissector/2018/12/30/creating-port-independent-wireshark-dissectors-in-lua.html)

[Using Wireshark Lua Standalone](https://osqa-ask.wireshark.org/questions/30507/use-wiresharks-lua-interpreter-stand-alone/)
[String Handling In Dissectors](https://wiki.wireshark.org/Development/String-handling-in-dissectors)

### Sample Captures to Work With
[Captures in WShark Source](https://github.com/wireshark/wireshark/tree/master/test/captures)
[Wireshark Sample Captures - Gitlab](https://gitlab.com/wireshark/wireshark/-/wikis/SampleCaptures)
[Wireshark Contrib Page - Gitlab](https://gitlab.com/wireshark/wireshark/-/wikis/Contrib)
**Important!**[Chris Maynard - Dissector 101 - Lua and C "Foo" Protocol](https://www.wireshark.org/lists/wireshark-dev/202110/msg00008.html)


### Packet Generators / Tools
[Scapy Packet Generation](https://0xbharath.github.io/art-of-packet-crafting-with-scapy/scapy/inspecting_packets/index.html)

### My Docs
[Wireshark For Beginners Notes](wireshark_for_beginners.md)
[Lua Dissector Info](lua_dissectors.md)
[Handy utilities](handy_utilities.md)

### Other Docs
**Important!**
[Tshark Dev Guide](https://tshark.dev/)
[Aerospike Lua Dissector](https://github.com/aerospike-community/aerospike-wireshark-plugin/tree/master)
[Aerospike Dissector Presentation](http://shakthimaan.com/downloads/Shooting-the-trouble-down-to-the-Wireshark-Lua-plugin.pdf)

## Dissectors
### Videos
[Extending Wireshark with Lua](https://www.youtube.com/watch?v=wG0tN9BDh1A)
[Lua Programming for Wireshark](https://www.youtube.com/playlist?list=PLedx6HwQji1O-SHD3_lCdSxMUT56d5y9Y)


### 3rd Party Git Repos and Docs
[Mikas Tech Blog - Dissector HOWTOs](https://mika-s.github.io/topics/)
[Hadriel Kaplan - DNS Dissector lua](https://github.com/wireshark/wireshark/blob/master/test/lua/dissector.lua)
[Multi-Layer Dissector](https://ask.wireshark.org/question/20217/how-to-create-multi-layer-lua-dissector/)
[Didier Stevens](https://blog.didierstevens.com/2014/05/12/video-packet-class-wireshark-lua-protocol-dissectors/)
[Kaos Lua Wireshark](https://github.com/kaos/wireshark-plugins)
[Katai Struct to Lua](https://github.com/joushx/kaitai-to-wireshark)
