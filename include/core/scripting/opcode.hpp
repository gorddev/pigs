#pragma once
#include <cstdint>

namespace pg::script {
    enum class OpCode : uint8_t {
        OpLoadTrue,      // Push boolean true onto the data stack
        OpLoadFalse,     // Push boolean false onto the data stack
        OpLoadInt,       // Push a 32-bit integer onto the data stack
        OpSetSymbol,     // Assign the top stack value to a Symbol ID
        OpGetSymbol,     // Read a Symbol ID onto the top of the stack
        OpHalt           // Stop execution
    };
}
