#pragma once

#include <glad/gl.h>

#include <sys/types.h>
#include <vector>
#include <cassert>

struct VertexBufferElement 
{
    uint count;
    uint type;
    u_char normalized;

    static uint GetSizeOfType(uint type)
    {
        switch (type)
        {
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_UNSIGNED_INT: return sizeof(GLuint);
            case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
        }
        return 0;
    }
};

class VertexBufferLayout
{
    public:
        VertexBufferLayout() : m_Stride(0) {};

        template<typename T>
        void Push(uint count)
        {
            assert(false);
        }

        inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
        inline uint GetStride() const { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements;
        uint m_Stride;
};
