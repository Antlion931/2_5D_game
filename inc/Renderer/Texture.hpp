#pragma once

#include <glad/gl.h>

#include <iostream>
#include <filesystem>

typedef unsigned char uchar;

class Texture
{
    public:
        Texture(const std::filesystem::path& path);
        ~Texture();

        void Bind() const;
        void Bind(uint slot);
        void Unbind() const;

        auto GetTextureParameters() const {
            struct {
                int widht;
                int height;
                int nrChannels;
            } params;

            params.widht = m_width;
            params.height = m_height;
            params.nrChannels = m_nrChannels;

            return params;
        }
    private:
        uint m_id;
        uint m_slot = 0;

        int m_width, m_height, m_nrChannels;

        // TODO: Different texture wrap modes and filters
        // GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER
}; // class Texture