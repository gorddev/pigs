#pragma once

#include <string>

#include "toolkit/intdef.h"

namespace pg::mem {

    struct MemInfo;

    enum Unit : char {
        B, KB, MB, GB, TB, PB, EB, AUTO
    };

    constexpr const char* memTypes[]=
        {"B", "KB", "MB", "GB", "TB", "PB", "EB", "Unknown"};


    MemInfo formatBytes(uintmax_t bytes, Unit type = AUTO, u16 precision = 4);

    struct MemInfo {
        float number;
        Unit mem_type;
        std::string to_string(bool space = false);
    private:
        u16 precision;

        MemInfo() = default;

        MemInfo(float final_number, Unit unit, u16 precision);

        friend MemInfo formatBytes(uintmax_t, Unit, u16);
    };
}
