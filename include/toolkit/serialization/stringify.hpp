#pragma once
#include <format>
#include <string_view>
#include <meta>

namespace pg {

    template<typename>
    struct can_display_with_std_format : std::false_type {};
    template<typename T>
        requires requires(T t) {std::format("{}", t);}
    struct can_display_with_std_format<T> : std::true_type{};

    template<typename T>
        requires(std::is_class_v<T>)
    std::string formatStruct(const T& t, int max_depth = 2, int indent = 0) {

        std::string ret;
        std::string ind_str = std::string(indent, ' ');

        [[maybe_unused]] bool counter = false;
        ret = std::format("\x1B[33mstruct \x1B[36m{}\x1B[0m {{", std::meta::display_string_of(^^T));
        template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unprivileged()))) {
            if (!counter) {
                ret += '\n';
                counter = true;
            }
            if constexpr (std::is_enum_v<typeof(t.[:m:])>) {
                ret += ind_str + std::format("  \x1B[32m{} \x1B[35m\x1B[1m \x1B[0m= {};\n",
                    std::meta::display_string_of(^^typeof(t.[:m:])),
                    std::meta::identifier_of(m),
                    pg::ser::enumToString(t.[:m:])
                );
            } else if constexpr(std::formattable<typeof(t.[:m:]), char>) {
                ret += ind_str + std::format("  \x1B[32m{} \x1B[35m\x1B[1m{} \x1B[0m= {};\n",
                    std::meta::display_string_of(^^typeof(t.[:m:])),
                    std::meta::identifier_of(m),
                    t.[:m:]
                );
            } else {
                if (max_depth > 0) {
                    ret += ind_str + "\x1B[29m\x1B[1m  " + std::meta::identifier_of(m) + "\x1B[0m = " + formatStruct(t.[:m:], max_depth - 1, indent + 2) + '\n';
                } else {
                    if constexpr (std::meta::has_identifier(m)) {
                        ret += ind_str + std::format("  \x1B[32m{} \x1B[35m{} \x1B[0m : sizeof({});\n",
                            std::meta::display_string_of(^^typeof(t.[:m:])),
                            std::meta::identifier_of(m),
                            sizeof(T)
                        );
                    } else {
                        ret += ind_str + std::format("  \x1B[32m{}\x1B[0m : sizeof({});\n",
                            std::meta::display_string_of(^^typeof(t.[:m:])),
                            sizeof(T)
                        );
                    }

                }
            }
        }

        ret += ind_str + "};";

        return ret;
    }

}
