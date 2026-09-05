#pragma once

#include <ankerl-hash-map/dense_map.hpp>

/* Created by Gordie Novak on 8/20/26.
 * Purpose:
 */

namespace pg {

    template<typename T>
    using string_map = ankerl::unordered_dense::map<std::string, T>;

}
