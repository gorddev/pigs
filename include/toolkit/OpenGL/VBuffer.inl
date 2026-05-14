#pragma once
// ************************************************ //

    template<typename V>
        requires(Is_Vertex<V>)
    VBuffer<V>::VBuffer(VBuffer&& other) noexcept
            : vao(other.vao), vbo(other.vbo), mbo(other.mbo), cbo(other.cbo), vertexCount(other.vertexCount) {
        other.vao = 0;
        other.vbo = 0;
        other.mbo = 0;
        other.cbo = 0;
        other.vertexCount = 0;
    }

    // ************************************************ //

    template<typename V>
        requires(Is_Vertex<V>)
    VBuffer<V>& VBuffer<V>::operator=(VBuffer&& other) noexcept {
        std::swap(vao, other.vao);
        std::swap(vbo, other.vbo);
        std::swap(mbo, other.mbo);
        std::swap(cbo, other.cbo);
        std::swap(vertexCount, other.vertexCount);
        return *this;
    }

    // ************************************************ //

    template<typename V>
        requires(Is_Vertex<V>)
    VBuffer<V> VBuffer<V>::make(const V vertices[], const size_t numVertices) {
        VBuffer buffer{};

        glGenVertexArrays(1, &buffer.vao);
        glBindVertexArray(buffer.vao);

        glGenBuffers(1, &buffer.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(V),
            vertices, GL_STATIC_DRAW);

        // allow the vertex type to setup its own attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
        V::setup_attributes();

        // unbind our vertex array.
        glBindVertexArray(0);

        buffer.vertexCount = numVertices;
        buffer.mbo = 0;
        buffer.cbo = 0;

        return buffer;
    }

    // ************************************************ //

    template<typename V>
        requires(Is_Vertex<V>)
    err::Errcode VBuffer<V>::genMatrixVBO(VBuffer<V>& buffer, size_t shaderLoc) {
        if (shaderLoc % 4 != 0 || shaderLoc < V::num_attributes) {
            GAN_WriteLog("VertexBuffer::genMatrixVBO()", "shaderLoc parameter is not valid, as it must"
                                "be divisible by 4 and less than", V::num_attributes, ". Provided value: ", shaderLoc);
            return err::ERR;
        }
        glBindVertexArray(buffer.vao);
        glGenBuffers(1, &buffer.mbo); //< generate buffer into our new vbo
        glBindBuffer(GL_ARRAY_BUFFER, buffer.mbo);
        for (size_t i = 0; i < 4; i++) {
            glEnableVertexAttribArray(i + shaderLoc);

            glVertexAttribPointer(
                shaderLoc + i, 4, GL_FLOAT, GL_FALSE,
                sizeof(float)*16,   //< size of 4x4 float matrix
                reinterpret_cast<void*>(sizeof(float)*4*i));    //< offset we render to

            glVertexAttribDivisor(shaderLoc + i, 1);
        }
        return err::OK;
    }

    // ************************************************ //
    
    template<typename V>
        requires(Is_Vertex<V>)
    err::Errcode VBuffer<V>::genColorVBO(VBuffer& buffer, size_t shaderLoc) {
        if (shaderLoc <= 2) {
            GAN_WriteLog("VertexBuffer::genColorVBO()",
                "shaderLoc parameter is not valid, as it"
                "overrides the pos, uv, and norm shader locations"
                "which range from 1-2. Provided shaderLoc: ", shaderLoc);
            return err::ERR;
        }
        glBindVertexArray(buffer.vao);
        glGenBuffers(1, &buffer.cbo);
        glBindBuffer(GL_ARRAY_BUFFER, buffer.cbo);
        glVertexAttribPointer(
            shaderLoc, 1,
            GL_FLOAT, GL_FALSE, sizeof(float)*4,
            nullptr);
        glEnableVertexAttribArray(shaderLoc);
        return err::OK;
    }

    template<typename V>
        requires(Is_Vertex<V>)
    VBuffer<V>::~VBuffer() {
        glBindVertexArray(vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &mbo);
        glDeleteBuffers(1, &cbo);

        glDeleteVertexArrays(1, &vao);
        glBindVertexArray(0);
    }