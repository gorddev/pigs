#pragma once

#include <meta>
#include <iterator>

#include "serialdef.hpp"

#include "enumToString.hpp"
#include "serialize_options.hpp"
#include "concepts/byte_containers.hpp"
#include "concepts/is_array_like.hpp"
#include "errors/Error.hpp"

namespace pg::ser {

    /// A single instance of serialization info for a single serialized object.
    struct SerInfo {
        std::string_view name;
        std::span<const bin_t> type;
        u16 typeSize{};
        std::string_view bytes;
    };

    #define PG_Ser_Warn(warning, ...) \
        if(pg::ser::warnings.warning) \
            PG_ErrAdd(err, __VA_ARGS__, " [warnings."#warning"]")

    template<typename T, typename BV> requires(is_byte_view<BV>)
    [[nodiscard]] Err deserializeStruct(T& t, const BV raw_bytes, Err* error = nullptr) {
        Err err = Err{};
        if (!error)
            error = &err;


        std::unordered_map<std::string_view, SerInfo> var_map;

        // Move past the size byte.
        auto bytes = std::string_view(reinterpret_cast<const char*>(raw_bytes.data())+2, raw_bytes.size()-2);

        for (size_t cursor = 0; cursor < bytes.size(); ) {
            SerInfo info;

            // STEP 1: Find the name of the variable.
            const size_t nameStart = cursor;
            // Move to the first ':' indicator.
            while (bytes[cursor++] != ':' && cursor < bytes.size());
            // Store the type of the variable
            std::string_view varName = std::string_view(bytes.data()+nameStart, cursor - nameStart-1);
            info.name = varName;

            // STEP 2: Find the type of the variable
            const size_t typeStart = cursor;
            auto type = static_cast<bin_t>(bytes[cursor]);
            while (!isMemcpyCompatable(type) && type != bin_t::NAMED_STRUCT) {
                cursor++;
                type = static_cast<bin_t>(bytes[cursor]);
            }
            info.type = std::span(reinterpret_cast<const bin_t*>(&bytes[typeStart]), ++cursor - typeStart);

            // move thru the setting var list.
            auto type_size_identifier = info.type.data();
            while (*type_size_identifier == bin_t::SETTING_VAR) {
                type_size_identifier++;
            }

            if (const auto size = enumToSize(*type_size_identifier)) {
                info.typeSize = size;
            } else {
                std::memcpy(&info.typeSize, &bytes[cursor], sizeof(u16));
                cursor += sizeof(u16);
            }

            // STEP 5: Jump forward the length of the data block.

            info.bytes = std::string_view(&bytes[cursor], info.typeSize);
            cursor += info.typeSize;

            var_map.emplace(varName, info);
        }

        des_DistributeStructMembers(t, var_map, *error);
        if (!warnings.surly)
            error->squash();
        else if (error->is_evil() && error == &err) {
            PG_Ser_Warn(surly, "Deserializer returned with errors.");
        }
        return err;
    }

    template<typename T>
    size_t des_FillStructMembers(T& t, const SerInfo& info, Err& err) {

        using elemType = is_array_like_struct<T>::value_type;
        if constexpr (std::is_const_v<T> || std::is_const_v<elemType>) {
            PG_Ser_Warn(const_data_member, "The type \"", std::meta::display_string_of(^^T), "\" cannot be modified via deserialization due to constness.");
            return info.typeSize;
        } else {
            auto get_memcpy_obj_size = [&info]() {
                u16 obj_size;
                if (const u16 d = enumToSize(info.type[1])) obj_size = d;
                else std::memcpy(&obj_size, &info.type[2], sizeof(u16));
                return obj_size;
            };

            auto move_up_info = [&info](const size_t modifier=2) {
                return SerInfo{
                    .type = std::span(info.type.data() + 1, info.type.size()-1),
                    .typeSize = info.typeSize,
                    .bytes = std::string_view(info.bytes.data()+modifier, info.bytes.size()-modifier)
                };
            };

            switch (info.type[0]) {
            case bin_t::ARRAY_LIKE:
                if constexpr(!is_array_like<r<T>>)
                    PG_Ser_Warn(dev_debug, "Found a non-array like in the bin_t::ARRAY_LIKE case of type ", std::meta::display_string_of(^^T));
                else if constexpr(is_array_like<r<T>>) {
                    if (isMemcpyCompatable(info.type[1])) {
                        u16 obj_size = get_memcpy_obj_size();
                        static_assert(!std::is_void_v<typename is_array_like_struct<r<T>>::value_type>);
                        if (obj_size != sizeof(typename is_array_like_struct<r<T>>::value_type)) {
                            PG_Ser_Warn(incompatible_size, "Sizes don't match of recorded ",
                                    obj_size, " vs. typed ",
                                    sizeof(typename is_array_like_struct<T>::value_type));
                        }
                        u16 num_elem;
                        std::memcpy(&num_elem, info.bytes.data(), sizeof(u16));

                        // Check to may sure we aren't out of bounds
                        if constexpr (!is_vector_v<T>) {
                            if (std::size(t) < num_elem) {
                                PG_Ser_Warn(array_too_small, "Attempting to squash ", num_elem, " elements into a container of size ", std::size(t));
                                num_elem = std::size(t);
                            }
                        } else
                            t.resize(num_elem);
                        // Finally memcpy the information in
                        std::memcpy(std::data(t), info.bytes.data() + sizeof(u16), obj_size * num_elem);
                        return obj_size*num_elem+sizeof(u16);
                    }
                    size_t cursor = 2;
                    for (auto& i : t) {
                        cursor += des_FillStructMembers(i, move_up_info(cursor), err);
                    }
                    return 0;
                }
            break;
            case bin_t::SETTING_VAR:
                if constexpr(is_specialization_of_v<SettingVar, T>) {
                    if (isMemcpyCompatable(info.type[1])) {
                        std::memcpy(std::data(t), info.bytes.data(), get_memcpy_obj_size());
                    }
                    else
                        des_FillStructMembers(*reinterpret_cast<r<typename T::value_type>*>(std::data(t)), move_up_info(), err);
                } else {
                    PG_Ser_Warn(mismatched_type, "Mismatched SETTING_VAR with a type: ", enumToString(typeToEnum<r<T>>()));
                }
            break;
            case bin_t::FLOAT32:
            case bin_t::FLOAT64:
            case bin_t::U_BYTE:
            case bin_t::BYTE:
            case bin_t::U_SHORT:
            case bin_t::SHORT:
            case bin_t::U_INT:
            case bin_t::INT:
            case bin_t::U_LONG:
            case bin_t::LONG:
                if constexpr (std::is_trivially_copyable_v<T>) {
                    if (typeToEnum<r<T>>() != info.type[0]) {
                        PG_Ser_Warn(mismatched_type, "Skipping ", enumToString(info.type[0]),
                            " mismatched type with: ", enumToString(typeToEnum<r<T>>()));
                    } else {
                        std::memcpy(&t, info.bytes.data(), enumToSize(info.type[0]));
                    }
                } else {
                    PG_Ser_Warn(data_copy_error, "Cannot copy binary primitive data into non-trivially copyable type.");
                }
            break;

            case bin_t::TRIVIAL_STRUCT:
            case bin_t::UNION:
                if constexpr (std::is_trivially_copyable_v<T>) {
                    if (typeToEnum<r<T>>()!=info.type[0]) {
                        PG_Ser_Warn(mismatched_type, "Skipping ", enumToString(info.type[0]), " mismatched type with: ", enumToString(typeToEnum<r<T>>()));
                        break;
                    }
                    u16 obj_size;
                    std::memcpy(&obj_size, info.bytes.data()-2, sizeof(u16));
                    std::memcpy(&t, info.bytes.data() + sizeof(u16), obj_size);
                } else PG_Ser_Warn(data_copy_error, "Union is not trivially copyable");
            break;
            case bin_t::NAMED_STRUCT:
                if constexpr(std::is_class_v<T>)
                    auto _  = deserializeStruct(t, std::span(info.bytes.data()-sizeof(u16), info.bytes.size()+2), &err);
            break;
            default:
                PG_Ser_Warn(dev_debug, "Missing type implementation.");
            }
        }
        return 0;
    }

    template<typename T>
    void des_DistributeStructMembers(T& t, const std::unordered_map<std::string_view, SerInfo>& var_map, Err& err) {
        constexpr auto ctx = std::meta::access_context::unprivileged();
        template for (constexpr std::meta::info member : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {
            if (var_map.contains(std::meta::identifier_of(member))) {
                std::string_view ident = std::meta::identifier_of(member);
                des_FillStructMembers(t.[:member:], var_map.at(ident), err);
            } else {
                PG_Ser_Warn(missing_serial, "Missing serial for: ", std::meta::identifier_of(member));
            }
        }
    }

}
