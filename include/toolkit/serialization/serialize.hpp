#pragma once

#include <toolkit/containers/SettingVar.hpp>
#include <meta>
#include <cstring>

#include "serialdef.hpp"
#include <toolkit/intdef.h>
#include "concepts/byte_containers.hpp"

namespace pg::ser {

    /// primary function that serializes a struct into an array of bytes.
    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void serializeNamedStruct(const T& data, BV& bytes);

    /// serializes a struct's member variable into the byte buffer.
    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void serializeStructMember(std::string_view var_name, const T& m, BV& bytes);

    /// Writes the raw bytes of the object `m` into the byte buffer. Should be called sparingly.
    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void writeRaw(const T& m, BV& bytes) {
        const size_t start_size = bytes.size();
        bytes.resize(start_size + sizeof(T));
        std::memcpy(&bytes[start_size], &m, sizeof(T));
    }

    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void writeObjSize(BV& bytes) {
        static_assert(sizeof(T) < std::numeric_limits<u16>::max());
        constexpr u16 t_size = static_cast<uint16_t>(sizeof(T));
        writeRaw(t_size, bytes);
    }

    /// Writes a string into the byte buffer.
    template<typename BV> requires(is_byte_vector<BV>)
    void writeStringBytes(const std::string& str, BV& bytes) {
        const u16 t_size = static_cast<uint16_t>(str.length());
        writeRaw(t_size, bytes);
        const size_t start_size = bytes.size();
        bytes.resize(start_size + str.length());
        std::memcpy(&bytes[start_size], str.data(), str.length());
    }


    /// Saves the struct's character identifier into the byte buffer.
    template<typename T, typename BV>
        requires(is_byte_vector<BV>)
    void serializeStructType(BV& bytes) {
        // Next, we write the type (represented by this lambda)
        auto writeTypeChar = [&](bin_t type) {
            bytes.resize(bytes.size() + 1);
            bytes.back() = static_cast<BV::value_type>(type);
        };

        // Write the type character to the bit buffer
        writeTypeChar(typeToEnum<r<T>>());
        // Then we write object information.
        if constexpr(is_memcpy_compatable<r<T>>) {
            if constexpr(!std::is_fundamental_v<r<T>>)
                writeObjSize<T>(bytes);
        }
        else if constexpr(is_container_t<T>) {
            if constexpr(is_array_like<T>) {
                serializeStructType<typename is_array_like_struct<T>::value_type>(bytes);
            } else serializeStructType<typename T::value_type>(bytes);
        }
        else if constexpr (!std::is_class_v<T>) static_assert(false, "type is not a memory copyable object or a container type");
    }

    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void serializeStructData(const T& m, BV& bytes) {
        // STATICALLY COPYABLE
        if constexpr(is_statically_copyable<r<T>>) {
            writeRaw(m, bytes);
        } // SETTING_VAR
        else if constexpr(is_specialization_of_v<SettingVar, T>) {
            serializeStructData(m.setting, bytes);
        } //VECTOR
        else if constexpr(is_array_like<T>) {
            writeRaw(static_cast<u16>(m.size()), bytes);
            for (const auto& i : m) {
                serializeStructData(i, bytes);
            }
        } // CLASS
        else if constexpr(!std::is_class_v<T>)  // do nothing on being a regular struct
            static_assert(false, "something went wrong in the serialize struct data template");
    }

    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void serializeStructMember(const std::string_view var_name, const T& m, BV& bytes) {
        // First thing, we always write the name of the variable.
        const size_t old_size = bytes.size();
        bytes.resize(old_size + var_name.length() + sizeof(':'));
        std::memcpy(&bytes[old_size], var_name.data(), var_name.length());
        bytes.back() = static_cast<BV::value_type>(':');

        // Next we write the type bytes
        serializeStructType<r<T>>(bytes);

        // Finally we write the struct data
        if constexpr(std::is_class_v<T> && !has_static_layout<T> && !is_container_t<T>) {
            serializeNamedStruct(m, bytes);
        } else serializeStructData(m, bytes);
    }

    template<typename T, typename BV> requires(is_byte_vector<BV>)
    void serializeNamedStruct(const T& data, BV& bytes) {
        // Plain vector of bytes to store all the information
        size_t start_size = bytes.size();
        // Reserve early because we that most likely that is our minimum size
        bytes.reserve(bytes.size() + sizeof(T));
        // Reserve two bytes for the size of this struct.
        bytes.resize(bytes.size() + sizeof(u16));

        constexpr auto ctx = std::meta::access_context::unprivileged();

        template for (constexpr std::meta::info member : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {
            if constexpr(std::meta::has_identifier(member)) {
                serializeStructMember(std::meta::identifier_of(member), data.[:member:], bytes);
            } else static_assert(false, "All members in serialized classes must be named.");
        }

        u16 struct_size = static_cast<u16>(bytes.size() - start_size - sizeof(u16));
        std::memcpy(&bytes[start_size], &struct_size, sizeof(u16));
    }
}
