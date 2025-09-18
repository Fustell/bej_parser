# BEJ Parser

BEJ Parser converts bej(Binary-encoded JSON) to JSON using schema dictionaries and saves in file.

# Dependencies
- GNU compiler
- CMake 3.14+
- Unity framework(fetch automatically)

# Build
```
mkdir build
cd build
cmake ..
cmake --build .
```
File `bej-parser` will appear in the `./build/src` folder

# Example of usage
```
$ ./bej-parser 
Usage: ./bej-parser <schema_dict.bin> <payload.bin> [output.json]
```
Some dummy data located in `./tests/dummy_dictionaries` and `./test/dummy_data`
```
$ ./bej-parser ../tests/dummy_dictionaries/Message_v1.bin ../tests/dummy_data/message_bej.bin ../message_decoded.json
Decoded JSON written to ../message_decoded.json
$ cat ../message_decoded.json 
{
    "MessageId": "Base.1.8.PropertyValueError",
    "Message": "The value for the property MemorySize is not valid.",
    "MessageArgs": [
        "MemorySize",
        "4096"
    ],
    "Severity": "Warning",
    "Resolution": "Correct the property value and retry the operation.",
    "RelatedProperties": [
        "/Systems/1/Memory/1/CapacityMiB",
        "/Systems/1/Memory/1/SpeedMHz"
    ]
}
```
```
$ ./bej-parser ../tests/dummy_dictionaries/Memory_v1.bin ../tests/dummy_data/memory_bej.bin ../memory_decoded.json
Decoded JSON written to ../memory_decoded.json
$ cat ../memory_decoded.json 
{
    "CapacityMiB": 65536,
    "DataWidthBits": 64,
    "AllowedSpeedsMHz": [
        2400,
        3200
    ],
    "ErrorCorrection": "NoECC",
    "MemoryLocation": {
        "Channel": 0,
        "Slot": 0
    },
    "IsRankSpareEnabled": true,
    "PartNumber": null,
    "Manufacturer": "Some"
}
```
# Testing
For running tests use
```
ctest -V
```
# Details
- Supports data
  - Integer
  - String
  - Boolean
  - Enum
  - Set
  - Array
  - Null
- Annotation logic is not implemented
- Code written in Google style(but macros are written to snake case in upper register)
- No dynamic memory allocation is used for dictionary entries (preallocated buffers only), as a result maximum dictionary entries per subset is 512
---
