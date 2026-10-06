#pragma once

#include <string>
#include <toolkit/apidef.h>

#include "Metrics.hpp"
#include "core/rendering/OpenGL/VBuffer.hpp"

/* Created by Gordie Novak on 8/20/26.
 * Purpose:
 */

namespace pg {
    struct TextMesh {
        VBuffer<FVertex> buffer;
        int vertexCount = 0;
        bool isDirty = true;
        std::string currentText = "";

        // Call this only once when initializing the debug menu
        TextMesh() : buffer(VBuffer<FVertex>::make(nullptr, 0)) {}

        // Call this ONLY when text string characters actually change
        void UpdateText(const std::string& newText, float startX, float startY, float fontSize, const FontMetrics& font, int atlasW, int atlasH) {
            if (currentText == newText && !isDirty) return; // Skip entirely if nothing changed!

            currentText = newText;
            isDirty = false;

            std::vector<float> vertices;
            vertices.reserve(newText.size() * 24); // 6 vertices * 4 floats per char
            float cursorX = startX;

            for (char c : newText) {
                if (!font.glyphs.contains(c)) continue;
                const GlyphData& g = font.glyphs.at(c);

                float uMin = g.atlasBounds.left / atlasW;
                float vMin = g.atlasBounds.bottom / atlasH;
                float uMax = g.atlasBounds.right / atlasW;
                float vMax = g.atlasBounds.top / atlasH;

                float xMin = cursorX + (g.planeBounds.left * fontSize);
                float yMin = startY + (g.planeBounds.bottom * fontSize);
                float xMax = cursorX + (g.planeBounds.right * fontSize);
                float yMax = startY + (g.planeBounds.top * fontSize);

                float quad[] = {
                    xMin, yMax, uMin, vMax,   xMin, yMin, uMin, vMin,   xMax, yMin, uMax, vMin,
                    xMin, yMax, uMin, vMax,   xMax, yMin, uMax, vMin,   xMax, yMax, uMax, vMax
                };
                vertices.insert(vertices.end(), std::begin(quad), std::end(quad));
                cursorX += g.advance * fontSize;
            }

            vertexCount = vertices.size() / 4;

            buffer.glBindVBO();
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        }

        // Call this every single frame
        void Draw() {
            if (vertexCount == 0) return;
            buffer.glBind();
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }

    };
}
