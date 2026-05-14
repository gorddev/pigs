#pragma once
#include <cstdint>
#include <stb_image/stb_image.h>

#include "path.hpp"
// Created by Gordie Novak on 2/27/26.
// used to automatically allocate/deallocate image data
// from stb.

namespace pig {

    /// images will automatically destroy their data unless
    /// explicitly moved.
    struct image {

        uint8_t* pixels;
        const uint16_t w, h;

        static std::optional<image> make(const path& path) {
            if (!path.is_regular_file()) {
                GAN_AppendLog("image::make()",
                    "Path ", path.c_str(), "reports being invalid.");
                return std::nullopt;
            }

            int width, height;
            uint8_t* pixels = stbi_load(
                path.expand().c_str(),
                &width, &height,
                nullptr, 0);

            if (!pixels) {
                GAN_WriteLog("image::make()",
                    "STB_Image reported failure with error: ", stbi_failure_reason());
                return std::nullopt;
            }

            return image(pixels, width, height);
        }

        image(const image&)             = delete;
        image operator=(const image&)   = delete;
        image(image&& other) noexcept
            : pixels(other.pixels),
              w(other.w), h(other.h)
        {
            other.pixels = nullptr;
        }
        image operator=(image&& other) = delete;

        ~image() {
            if (pixels)
                stbi_image_free(pixels);
        }

    private:
        image(uint8_t* pixels, uint16_t w, uint16_t h)
            : pixels(pixels), w(w), h(h) {}
    };

}