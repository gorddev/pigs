#pragma once
#include <cstdint>
#include <expected>
#include <filesystem>
#include <stb_image/stb_image.h>

#include <core/errors/error-structs/files/FileNotExistsError.hpp>
#include <core/errors/error-structs/library/STBImageError.hpp>
#include <core/filesystem/path.hpp>

// Created by Gordie Novak on 2/27/26.
// used to automatically allocate/deallocate image data
// from stb.



namespace pg {

    /// images will automatically destroy their data unless
    /// explicitly moved.
    struct Image {

        uint8_t* pixels;
        const uint16_t w, h;

        static expected<
          Image,
          err::FileNotExists,
          err::STBImage
        > make(const path& path) {
            if (!std::filesystem::exists(path)) {
                return PG_UErrNew(err::FileNotExists, .file_name = path.c_str());
            }

            int width, height, channels;
            uint8_t* pixels = stbi_load(
                path,
                &width, &height,
                &channels, 0);

            if (!pixels) {
                return PG_UErrNew(err::STBImage, .failure_reason = stbi_failure_reason());
            }

            return Image(pixels, width, height);
        }

        Image(const Image&)             = delete;
        Image operator=(const Image&)   = delete;
        Image(Image&& other) noexcept
            : pixels(other.pixels),
              w(other.w), h(other.h)
        {
            other.pixels = nullptr;
        }
        Image operator=(Image&& other) = delete;

        ~Image() {
            if (pixels) {
                stbi_image_free(pixels);
            }
        }

    private:
        Image(uint8_t* pixels, uint16_t w, uint16_t h)
            : pixels(pixels), w(w), h(h) {}
    };

}
