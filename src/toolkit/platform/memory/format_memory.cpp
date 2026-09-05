/* Created by Gordie Novak on 8/16/26.
 * Purpose:
 */
#include <iomanip>
#include <toolkit/platform/memory/format_memory.hpp>

/**
 * Converts a raw byte count into a structured MemInfo object.
 * @param bytes The raw number of bytes.
 * @param precision The decimal precision (defaults to NONE_PRECISION for auto-precision).
 * @param type The target scale unit (defaults to NONE_B to automatically determine the best unit).
 */
pg::mem::MemInfo pg::mem::formatBytes(uintmax_t bytes, Unit type, u16 precision) {
    Unit target_type = type;
    auto final_number = static_cast<float>(bytes);

    if (type == AUTO) {
        // Auto-detect the most reasonable unit tier
        int scale = 0;
        double current_bytes = static_cast<double>(bytes);

        while (current_bytes >= 1024.0 && scale < EB) {
            current_bytes /= 1024.0;
            scale++;
        }
        target_type = static_cast<Unit>(scale);
        final_number = static_cast<float>(current_bytes);
    } else {
        // Convert to the explicit unit specified by the user
        double divisor = 1.0;
        for (int i = 0; i < static_cast<int>(type); ++i) {
            divisor *= 1024.0;
        }
        final_number = static_cast<float>(static_cast<double>(bytes) / divisor);
    }

    return MemInfo(final_number, target_type, precision);
}

#include <sstream>

std::string pg::mem::MemInfo::to_string(const bool space) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << number;
    if (space) ss << ' ';
    ss << memTypes[mem_type];
    return ss.str();
}

pg::mem::MemInfo::MemInfo(float final_number, Unit unit, u16 precision) :
    number(final_number), mem_type(unit), precision(precision) {}
