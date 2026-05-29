#pragma once
#include <cstdint>
#include <expected>
#include <stb_image/stb_image.h>

#include "errors/Error.hpp"
#include <toolkit/filesystem/path.hpp>

// Created by Gordie Novak on 2/27/26.
// used to automatically allocate/deallocate image data
// from stb.

namespace pg {

    /// images will automatically destroy their data unless
    /// explicitly moved.
    struct Image {

        uint8_t* pixels;
        const uint16_t w, h;

        static std::expected<Image, Error> make(const path& path) {
            if (!path.is_regular_file()) {
                return PG_MAKE_UERR("Path ", path.c_str(), " is not a regular valid file to load from.");
            }

            int width, height, channels;
            uint8_t* pixels = stbi_load(
                path.expand().c_str(),
                &width, &height,
                &channels, 0);

            if (!pixels) {
                return PG_MAKE_UERR("STB_Image reported failure with error: ", stbi_failure_reason());
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