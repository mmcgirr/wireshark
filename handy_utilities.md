# Handy Utilities
A collection of various man pages, IDEs, Language Servers, etc. that help in development & analysis.

- [Handy Utilities](#handy-utilities)
  - [Wireshark (official) Tools \& Traffic Generators](#wireshark-official-tools--traffic-generators)
    - [Wireshark external tools and tips](#wireshark-external-tools-and-tips)
  - [IDEs](#ides)
    - [VSCode](#vscode)
      - [Lua Language Server (WIP)](#lua-language-server-wip)
    - [ZeroBrane Studio](#zerobrane-studio)
      - [MobDebug - lua debug hook](#mobdebug---lua-debug-hook)
  - [Command Line Utilities](#command-line-utilities)
    - [XXD](#xxd)
      - [xxd notes / useful tips](#xxd-notes--useful-tips)
  - [3rd Party Repos](#3rd-party-repos)
    - [Wirebait (no longer in development)](#wirebait-no-longer-in-development)


## Wireshark (official) Tools & Traffic Generators
### Wireshark external tools and tips
[Tools & Tips](https://wiki.wireshark.org/Tools#Traffic_generators)

## IDEs 
<details>

### VSCode 
Not 100% sure how to do this, but it would be glorious if I could figure out how to attach mobdebug to wireshark and use vscode as the lua IDE

#### Lua Language Server (WIP)
[LuaLS Wiki](https://luals.github.io/wiki/)

This would be a part of using vscode / vscodium as lua wireshark "live" debugger - good for intellisense, etc.  However, I'm not 100% sure on how this actually works (primarily definition files - creating the wireshark lua api definition file).  There is an example at []()
[Definition Files](https://luals.github.io/wiki/definition-files/)

Addons probably has useful examples of how definition files are made / have been made.
[LuaLS Addons](https://github.com/LuaLS/LLS-Addons)

LuaCATS has other examples of definition files / addons
[LuaCATs All Repos](https://github.com/orgs/LuaCATS/repositories)

### ZeroBrane Studio
A useful IDE which allows for relatively easy "live debug" of wireshark lua scripts running in wireshark.

Not a fan of the folding in the IDE, however.

#### MobDebug - lua debug hook
See here: [Mobdebug](https://github.com/pkulchenko/MobDebug/tree/master)
</details>

## Command Line Utilities

<details>

### XXD
Platforms
* Git Bash (windows)
* CLI (linux)

#### xxd notes / useful tips
[xxd hex string to binary](https://unix.stackexchange.com/questions/113491/can-xxd-be-used-to-output-the-binary-representation-of-hex-number-not-a-string)

</details>

## 3rd Party Repos

<details>

### Wirebait (no longer in development)

[Wirebait](https://github.com/MarkoPaul0/WireBait)

This is no longer in development by the author - but the repo is still up, and there have been forks.  Would be interesting to look at the forks and the repo for how he's doing things.

</details>

