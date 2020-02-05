# RockÉTSd

`rocketsd` is a "daemon" that bridges between serial input (UART) and external software, namely InfluxDB and [CuteStation](https://github.com/ngc7293/cutestation). It loads protocol message/node IDs from an XML file but not packet structure. 

`rocketsd` is not intented to be general-purpose software, it is tailored to the need of [RockÉTS](https://clubrockets.ca), but I do however strive to display it as an example of CuteStation daemon implementation, with the fool's hope that someone else will use my software one day :).

This repository also includes `xmlprotoc.py`, a Python3 script that reads the same protocol XML file and outputs a C-language with messge/node id defines (`#define MESSAGE_ID`).

## Dependencies

This project has the following dependencies:

 - Qt 5.12 (the path is currently hard-coded in the makefile, you will have to update it locally)
 - Protobuf (for CuteStation endpoint)
 - nlohmann::json
 - CMake
 - Ninja (optionnal)
 - Python 3 for `xmlprotoc.py`

Additionnally:

 - CuteStation
 - InfluxDB

## Compiling

```bash
mkdir build; cd build;
cmake .. -GNinja
ninja
```

## Running

Configuration is not yet done, so `protocol.xml` must be in your current directory.

```bash
build/rocketsd
```