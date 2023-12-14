#pragma once

#include "Renderer/VertexBuffer.hpp"
#include "Renderer/VertexBufferLayout.hpp"

class VertexArray
{
    public:
        VertexArray();
        ~VertexArray();

        void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

        void Bind() const;
        void Unbind() const;
    private:
        uint m_id;
};