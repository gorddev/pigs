#pragma once

#include <fstream>
#include <ankerl-hash-map/dense_map.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace pg {
    struct Bounds {
        float left = 0, bottom = 0, right = 0, top = 0;
    };

    struct GlyphData {
        int unicode;
        double advance;
        Bounds planeBounds;
        Bounds atlasBounds;
    };

    struct FontMetrics {
        double size;
        double distanceRange;
        ankerl::unordered_dense::map<int, GlyphData> glyphs;
    };

    inline FontMetrics LoadFontAtlasData(const std::string& jsonPath) {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            std::puts(std::string("Failed to open atlas JSON file!");
        }

        json data;
        file >> data;

        FontMetrics font;

        font.size = data["atlas"]["size"].get<double>();
        font.distanceRange = data["atlas"]["distanceRange"].get<double>();

        for (const auto& g : data["glyphs"]) {
            GlyphData glyph;
            glyph.unicode = g["unicode"].get<int>();
            glyph.advance = g.value("advance", 0.0);

            if (g.contains("planeBounds")) {
                auto pb = g["planeBounds"];
                glyph.planeBounds = { pb["left"], pb["bottom"], pb["right"], pb["top"] };
            }

            if (g.contains("atlasBounds")) {
                auto ab = g["atlasBounds"];
                glyph.atlasBounds = { ab["left"], ab["bottom"], ab["right"], ab["top"] };
            }

            font.glyphs[glyph.unicode] = glyph;
        }

        return font;
    }
}
