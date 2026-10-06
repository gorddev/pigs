#pragma once

#include "Random.hpp"
#include <core/rendering/OpenGL/Texture.hpp>
#include <vector>

class Collin {
  const pg::u32 width = 400;
  const pg::u32 height = 400;
  const pg::u32 num_colors = 5;
  const pg::u32 num_pixels = width * height;

  // Two bit arrays so we can hot-swap texture making.
  std::vector<pg::u8> bit_array[2];
  pg::Texture tex[2];

  enum which_buf { A = 0, B = 1 } w_buf = A;

public:
  Collin(pg::u32 width, pg::u32 height, pg::u32 numColors)
      : width(width), height(height), num_colors(numColors),
        num_pixels(width * height), w_buf(A) {
    bit_array[A].resize(width * height);
    fill_random_range(this->bit_array[A], 0, Collin::num_colors - 1);
    bit_array[B] = bit_array[A];
    std::cerr << w_buf << std::endl;
  }

  void init() {
    tex[A] = PG_Unwrap(pg::Texture::make2D(bit_array[A].data(), width, height,
                                           pg::ScaleMode::PG_PIXEL, GL_R8,
                                           GL_RED, GL_UNSIGNED_BYTE));
    tex[B] = PG_Unwrap(pg::Texture::make2D(bit_array[A].data(), width, height,
                                           pg::ScaleMode::PG_PIXEL, GL_R8,
                                           GL_RED, GL_UNSIGNED_BYTE));
    std::cerr << w_buf << std::endl;
  }

  template <size_t ChunkSize, size_t N>
  constexpr auto split_into_spans(pg::u8 (&arr)[N]) {
    static_assert(
        N % ChunkSize == 0,
        "C-array total size must be perfectly divisible by the chunk size!");

    constexpr size_t NumChunks = N / ChunkSize;
    std::array<std::span<pg::u8, ChunkSize>, NumChunks> result;
    for (size_t i = 0; i < NumChunks; ++i) {
      result[i] = std::span<pg::u8, ChunkSize>(&arr[i * ChunkSize], ChunkSize);
    }
    return result;
  }

  void glBindTex() {
  	tex[w_buf].glBind(GL_TEXTURE0);
  }

  /**
   * Sweeps an 8-bit image channel. If any immediately adjacent neighbor pixel
   * is exactly 1 greater than the current pixel, that neighbor is changed
   * to match the current pixel's value.
   */
  void tick(const bool (&select_arr)[8]) {
      const int rbuf = w_buf;
      const int wbuf = 1 - w_buf;

      auto& r = bit_array[rbuf];
      auto& w = bit_array[wbuf];

      const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
      const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

      for (int y = 0; y < height; ++y) {
          for (int x = 0; x < width; ++x) {
              int currentIdx = y * width + x;

              pg::u8 currentVal = r[currentIdx];

              for (int i = 0; i < 8; i += 2) {
                  if (!select_arr[i])
                      continue;

                  int nx = x + dx[i];
                  int ny = y + dy[i];

                  if (nx >= 0 && nx < width &&
                      ny >= 0 && ny < height) {

                      int neighborIdx = ny * width + nx;

                      if (r[neighborIdx] == (currentVal + 1) % num_colors) {
                          w[neighborIdx] = currentVal;
                      }
                  }
              }
          }
      }

      // Upload the buffer we just wrote.
      tex[wbuf].overwrite(
          w.data(),
          GL_RED,
          GL_UNSIGNED_BYTE
      );

      // The newly written buffer becomes the display/read buffer.
      w_buf = (which_buf)wbuf;
  }

};
