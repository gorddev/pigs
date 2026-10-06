#pragma once
#include <mutex>
#include <filesystem>

#include <toolkit/intdef.h>
#include <toolkit/types/fstring.hpp>
#include <toolkit/platform/executable_dir.hpp>

namespace pg {

    class path;
    class Config;

    namespace files {
        /// Contains the internal buffer for a path.
        thread_local inline class pathBuffer {
            friend class pg::path;
            friend class pg::Config;

            static constexpr u16 buffer_size = 512;
            /// Internal buffer that manages the path
            fstring<buffer_size> str_buffer{};
            u16 buffer_div = 0;
            mutable std::mutex buffer_mutex;

            /// Allows the setting of a base path without const char
            void setBasePath(const std::string_view view) {
                std::lock_guard lock(buffer_mutex);
                str_buffer.clear();
                if (view.length() < buffer_size) {
                    str_buffer << view << '/';
                    buffer_div = str_buffer.length();
                }
            }
            /// Lets you add on a path to the current one
            void concatPath(std::string_view view) {
                std::lock_guard lock(buffer_mutex);
                if (view.length() + buffer_div < buffer_size) {
                    str_buffer.resize(buffer_div);
                    str_buffer << view;
                }

            }

            /// Allows access to the raw underlying data buffer
            const char* data() const {
                std::lock_guard lock(buffer_mutex);
                return str_buffer.c_str();
            }
          public:
            /// Wraps the view of the path buffer.
            std::string_view view() {
                std::lock_guard lock(buffer_mutex);
                return std::string_view(str_buffer.data(), str_buffer.length());
            }
        } p_buffer;
    }

    class path {
        const std::string_view view;
    public:
        template<size_t N>
            requires(N < files::pathBuffer::buffer_size)
        path(const char (&arr)[N]) : view(std::string_view(arr, N)) {}

        path(std::string_view view) : view(view) {}

        [[nodiscard]] const char* c_str() const {
            files::p_buffer.concatPath(view);
            return files::p_buffer.data();
        }

        [[nodiscard]] operator const char*() const {
            return c_str();
        }

        [[nodiscard]] operator std::string_view() const {
            files::p_buffer.concatPath(view);
            return files::p_buffer.view();
        }

        [[nodiscard]] operator std::filesystem::path() const {
            files::p_buffer.concatPath(view);
            return {files::p_buffer.view()};
        }

        [[nodiscard]] path parent() const {
            for (int i = view.length()-1; i >= 1; i--) {
                if (view[i]=='/')
                    return path(view.substr(0, i));
            }
            return path(".");
        }
    };


}
