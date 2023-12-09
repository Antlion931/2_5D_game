#include "Renderer/VertexBufferLayout.hpp"

template<>
void VertexBufferLayout::Push<float>(uint count)
{
    m_Elements.push_back({count, GL_FLOAT, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template<>
void VertexBufferLayout::Push<uint>(uint count)
{
    m_Elements.push_back({count, GL_UNSIGNED_INT, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template<>
void VertexBufferLayout::Push<u_char>(uint count)
{
    m_Elements.push_back({count, GL_UNSIGNED_BYTE, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}