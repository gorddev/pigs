#pragma once

#include <meta>
#include <print>

#include "enum_to_string.hpp"
#include "Options.hpp"
#include "serialdef.hpp"
#include <cstring>

#include "concepts/byte_containers.hpp"
#include "concepts/is_array_like.hpp"

namespace pg::ser {



    struct SerInfo {
        std::span<const bin_t> type;
        u16 typeSize;
        std::string_view bytes;

        [[nodiscard]] std::string stringify() const {
            return std::format("SerInfo:\n  TypeSize={}\n  bytes_ptr,size={{{},{}}}",
                typeSize, (void*)bytes.data(), bytes.length());
        }
    };

    template<typename T>
    void desdistributeStructMembers(const T& t, const std::unordered_map<std::string_view, SerInfo>& map);

    template<typename T, typename BV> requires(is_byte_view<BV>)
    void deserializeStruct(T& t, const BV raw_bytes) {
        std::unordered_map<std::string_view, SerInfo> var_map;

        // Move past the size byte.
        auto bytes = std::string_view(reinterpret_cast<const char*>(raw_bytes.data())+2, raw_bytes.size()-2);

        for (size_t cursor = 0; cursor < bytes.size(); ) {
            SerInfo info;

            // STEP 1: Find the name of the variable.
            const size_t nameStart = cursor;
            // Move to the first ':' indicator.
            while (bytes[cursor++] != ':' && cursor!=bytes.size());
            // Store the type of the variable
            std::string_view varName = std::string_view(bytes.data()+nameStart, cursor - nameStart-1);

            // STEP 2: Find the type of the variable
            const size_t typeStart = cursor;
            int stack = 0;
            auto type = static_cast<bin_t>(bytes[cursor]);
            while (!isFundamentalSize(type)) {
                cursor++;
                stack++;
                type = static_cast<bin_t>(bytes[cursor]);
            }
            info.type = std::span(reinterpret_cast<const bin_t*>(&bytes[typeStart]), ++cursor - typeStart);
            for (auto& i: info.type) {
                //std::println("Type: {}", enum_to_string(static_cast<bin_t>(i)));
            }


            if (auto size = enumToSize(static_cast<bin_t>(info.type.front()))) {
                info.typeSize = size;
            } else  {
                std::memcpy(&info.typeSize, &bytes[cursor], sizeof(u16));
                cursor += sizeof(u16);
            }

            // STEP 5: Jump forward the length of the data block.
            info.bytes = std::string_view(&bytes[cursor], info.typeSize);
            cursor += info.typeSize;

            var_map.emplace(varName, info);
        }

        desdistributeStructMembers(t, var_map);
    }

    template<typename T>
    size_t desFillStructMembers(T& t, const SerInfo& info) {
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

        auto primitive_pattern = [&](const bin_t type){

        };

        switch (info.type[0]) {
        case bin_t::ARRAY_LIKE:
            if constexpr(!is_array_like<r<T>>)
                throw std::runtime_error("not array oops oops");
            else {
                if (isMemcpyCompatable(info.type[1])) {
                    u16 obj_size = get_memcpy_obj_size();
                    if (obj_size != sizeof(typename is_array_like_struct<r<T>>::value_type)) {
                        std::println("Sizes don't match of recorded {} vs. typed {}", obj_size, sizeof(typename is_array_like_struct<T>::value_type));
                        break;
                    }
                    u16 num_elem;
                    std::memcpy(&num_elem, info.bytes.data(), sizeof(u16));

                    // Check to may sure we aren't out of bounds
                    if constexpr (!is_vector_v<T>) {
                        if (std::size(t) < num_elem) {
                            std::println("Attempting to squash {} elements into a container of size {}", num_elem, std::size(t));
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
                    cursor += desFillStructMembers(i, move_up_info(cursor));
                }
                return 0;
            }
        break;
        case bin_t::SETTING_VAR:
            if constexpr(is_specialization_of_v<SettingVar, T>) {
                if (isMemcpyCompatable(info.type[1]))
                    std::memcpy(std::data(t), info.bytes.data(), get_memcpy_obj_size());
                else
                    desFillStructMembers(*reinterpret_cast<T::value_type*>(std::data(t)), move_up_info());
            } else std::println("Mismatched SETTING_VAR with a type: ", enum_to_string(typeToEnum<r<T>>()));
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
            if (typeToEnum<r<T>>()!=info.type[0])
                std::println("Skipping {} mismatched type with: {}", enumToString(info.type[0]), enum_to_string(typeToEnum<r<T>>()));
            else std::memcpy(&t, info.bytes.data(), enumToSize(bin_t::FLOAT32));
        break;
        case bin_t::TRIVIAL_STRUCT:
        case bin_t::UNION:
            if (typeToEnum<r<T>>()!=info.type[0]) {
                std::println("Skipping {} mismatched type with: {}", enumToString(info.type[0]), enum_to_string(typeToEnum<r<T>>()));
                break;
            }
            u16 obj_size;
            std::memcpy(&obj_size, &info.type[2], sizeof(u16));
            std::memcpy(&t, info.bytes.data() + sizeof(u16), obj_size);
        break;
        case bin_t::NAMED_STRUCT:
            if constexpr(std::is_class_v<T>)
                deserializeStruct(t, std::span(info.bytes.data()+sizeof(u16), info.bytes.size()-2));
        break;
        default:
            std::println("No implementation yet");
        }
        return 0;
    }

    template<typename T>
    void desdistributeStructMembers(T& t, const std::unordered_map<std::string_view, SerInfo>& var_map) {
        constexpr auto ctx = std::meta::access_context::unprivileged();
        template for (constexpr std::meta::info member : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {
            if (var_map.contains(std::meta::identifier_of(member))) {
                std::string_view ident = std::meta::identifier_of(member);
                desFillStructMembers(t.[:member:], var_map.at(ident));
            } else {
                std::println("Missing serial for: {}", std::meta::identifier_of(member));
            }
        }

    }



}
